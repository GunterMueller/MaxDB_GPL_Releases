/*!
  \file    DBMSrvBHist_EBIDList.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   implementing a class for managing a list of external backup
           ID's (including type and date of the backup)

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end


\endif
*/

//
//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDList.hpp"
#include "SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_MediaGroup.hpp"
#include "SAPDB/DBM/Srv/DBMSrv_Reply.hpp"
#include "hcn30.h"
#include "hcn35.h"
#include "hcn35b.h"
#include "hcn90.h"


DBMSrvBHist_EBIDList::DBMSrvBHist_EBIDList()
{
    Current=List.end();
}

DBMSrvBHist_EBIDList::~DBMSrvBHist_EBIDList()
{
    Free();
}

tcn00_Error DBMSrvBHist_EBIDList::GetEBIDs(VControlDataT * vcontrol,
                                           CommandT      * command,
                                           char          * replyData,
                                           int           * replyLen,
                                           int             replyLenMax,
                                           int             FromMedium)
{
    tcn00_Error rc=OK_CN00;
    DBMSrv_Reply Reply(replyData, replyLen, replyLenMax);

    char MediumName[255];
    char DBName[255];
    char HostName[255];

    DBMSrvBTools_ToolEnum BackupToolType;

    tcn003_MediaGroup * MediaGroup=0;
    tcn003_Medium     * Medium=0;

    MediumName[0]='\0';
    DBName[0]='\0';
    HostName[0]='\0';

    // get medium name or backup tool string
    if(OK_CN00==rc)
    {
        if(cn90GetToken(command->args, MediumName, 1, 255))
            RemoveDoublequotes(MediumName);
        else
            rc=cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00); //we need a medium, if it's not there we have an error
    }

    if(FromMedium)
    {
        tcn003_Media Media;

        if(OK_CN00==rc)
            rc=cn30ReadMedia(vcontrol->dbname,  replyData, replyLen, replyLenMax, &Media);
            
        if(OK_CN00==rc)
        {
            rc=cn30FindMediumByName(replyData, replyLen, &Media, MediumName, true, &MediaGroup, &Medium);

            if(ERR_MENOTFOUND_CN00==rc)
            {
                Reply.startWithOK();
                rc=cn30FindMediaGroupByName(replyData, replyLen, &Media, MediumName, &MediaGroup);
            }

            if(OK_CN00==rc)
            {
                BackupToolType=DBMSrvBMedia_MediaGroup::BackupTool(Medium, MediaGroup);

                if(DBMSrvBTools_ToolEnum::None==BackupToolType)
                    rc=Reply.startWithError(ERR_BAWRONGMEDIUM_CN00);
            }
        }

        cn30FreeMedia(&Media); //we don't need the media information any longer
    }
    else
        BackupToolType=DBMSrvBTools_ToolEnum(MediumName);

    if(OK_CN00==rc)
    {
        cn90GetToken(command->args, DBName, 2, 255);     // if the parameter is not supplied, DBName[0]=='\0'
        cn90GetToken(command->args, HostName, 3, 255);   // if the parameter is not supplied, HostName[0]=='\0'

        if(DBMSrvBTools_ToolEnum::None!=BackupToolType) //if an external backup tool is specified be the medium/magic string
        {
            tcn35b_DBMExternalBackupController * Tool=0;

            rc=TheDBMExternalBackupControllerFactory_cn35b.ConstructADBMExternalBackupController(Tool,
                                                                                                 replyData,
                                                                                                 replyLen,
                                                                                                 tcn35d_BackupInfo::GiveBackupInfo,
                                                                                                 BackupToolType,
                                                                                                 vcontrol->dbname,
                                                                                                 '\0'==DBName[0] ? 0 :DBName,
                                                                                                 '\0'==HostName[0] ? 0 : HostName);

            if(OK_CN00==rc)
            {
                Tool->ListEBIDsOfAvailableBackups(*this);
                rc=Tool->GiveLastError();                       // report all errors (also if tool is just not installed)
            }

            if(0!=Tool)
                delete Tool;
        }
    }

    return rc;
}

tcn00_Error  DBMSrvBHist_EBIDList::ListNextEBIDs(VControlDataT * vcontrol,
                                                 char          * replyData,
                                                 int           * replyLen,
                                                 int             replyLenMax)
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_Part *>::iterator OldCurrent=Current;

    if(List.end()==Current)
        Current=List.begin();

    char * pCurrPos;
    char * pFlagPos;

    size_t Length=0;

    bool IsFull=false;

    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrPos=replyData+strlen(replyData);

    sprintf(pCurrPos, "%s", CONT_FLAG_CONTINUE);
    pFlagPos=pCurrPos;
    pCurrPos+=strlen(pCurrPos);
    
    size_t MaxUsableBufferSize=replyLenMax-strlen(replyData)-1;

    while(List.end()!=Current && !IsFull) // not at end of list and reply buffer not full     
    {
        IsFull=(*Current)->PrintEBIDInto(pCurrPos, MaxUsableBufferSize, 1); // get also extended information (type and date/time)

        if(!IsFull) // go to the next list element only, if all was ok (enough space in reply buffer)
            ++Current;
    }

    if(List.end()==Current)
        strncpy(pFlagPos, CONT_FLAG_END, strlen(CONT_FLAG_END));
    else
        strncpy(pFlagPos, CONT_FLAG_CONTINUE, strlen(CONT_FLAG_CONTINUE));

    if(OK_CN00==rc)
    {
        if(List.end()!=Current)
        {
            vcontrol->szNextCommand.rawAssign("backup_ext_ids_listnext");
            vcontrol->nNextCommandSkip = 1;
        }
    }
    else
        Current=OldCurrent;
    
    return rc;
}

tcn00_Error DBMSrvBHist_EBIDList::AddToEnd(char                         * replyData,
                                           int                          * replyLen,
                                           DBMSrvBHist_Part::EBIDStatus   Status,
                                           const char                   * Key,
                                           size_t                         KeyLength,
                                           const char                   * Label,
                                           size_t                         LabelLength,
                                           const char                   * EBID,
                                           size_t                         EBIDLength,
                                           const char                   * Type,
                                           const char                   * DateTime)
{
    tcn00_Error rc=OK_CN00;

    DBMSrvBHist_Part * NewPart=new DBMSrvBHist_Part(DBMSrvBHist_Part::PartFromExternalBackupTool);

    if(0==NewPart)
        rc=ERR_MEM_CN00;
    else
    {
        rc=NewPart->SetToEBIDInfo(Status,
                                  Key,
                                  KeyLength,
                                  Label,
                                  LabelLength,
                                  EBID,
                                  EBIDLength,
                                  Type,
                                  DateTime);

        if(OK_CN00==rc && !List.push_back(NewPart))
            rc=ERR_MEM_CN00;
    }

    return rc;
}

void DBMSrvBHist_EBIDList::Free()
{
    Tools_List<DBMSrvBHist_Part *>::iterator h=List.begin();

    while(h!=List.end())
    {
        delete (*h);
        h=List.erase(h);
    }
}

Tools_List<DBMSrvBHist_Part *> & DBMSrvBHist_EBIDList::GiveList()
{
    return List;
}

void DBMSrvBHist_EBIDList::RemoveDoublequotes(char * String)
{
    size_t Length=strlen(String);

    if(Length>1 &&              //if the string has more than one character and
       '"'==String[0] &&        //the first is a '"' and
       '"'==String[Length-1])   //the last is a '"'
    {
        size_t i;

        // do a secure strncpy(String, String+1, Length-2);
        for(i=0; i<Length-2; i++) 
            String[i]=String[i+1];  //take all chars but not the first and the last
    }
}
