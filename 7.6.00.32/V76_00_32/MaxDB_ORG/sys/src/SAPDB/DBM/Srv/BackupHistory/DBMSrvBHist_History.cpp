/*!
    \file    DBMSrvBHist_History.cpp
    \author  TiloH
    \ingroup backup history handling by the DBMServer
    \brief   implementing a class for managing the information of the backup
             history files

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2006 SAP AG

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


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <stdlib.h>

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_EBIDHistoryFile.hpp"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_HistoryFile.hpp"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_MediaHistoryFile.hpp"

#include "hcn35.h"
#include "hcn35b.h"
#include "hcn42.h"
#include "hcn90.h"
#include "heo06.h"


//-----------------------------------------------------------------------------
// members of class DBMSrvBHist_History
//-----------------------------------------------------------------------------

DBMSrvBHist_History::DBMSrvBHist_History()
{
    CheckEBIDs=false;

    VControl=0;
}

DBMSrvBHist_History::~DBMSrvBHist_History()
{
    Free();
}

void DBMSrvBHist_History::Free()
{
    //delete the objects referenced by the elements of History
    for(Tools_List<DBMSrvBHist_Backup *>::iterator i=History.begin(); i!=History.end(); ++i)
        delete (*i);

    History.clear();
}

tcn00_Error DBMSrvBHist_History::Open(VControlDataT * vcontrol,
                                      int             CheckAvailabilityOfEBIDs,
                                      char          * replyData,
                                      int           * replyLen,
                                      int             replyLenMax,
                                      int             UseMediaHistory)
{
    tcn00_Error rc=OK_CN00;

    if(CheckAvailabilityOfEBIDs)
        CheckEBIDs=true;
    else
        CheckEBIDs=false;

    setVControlReplyBufferAndStandardErrorHandler(vcontrol, replyData, replyLen, replyLenMax);

    rc=ReadHistories(UseMediaHistory);

    return rc;
}

tcn00_Error DBMSrvBHist_History::AnalyzeLogBackup(tcn003_BackupResult * Result)
{
    tcn00_Error rc=OK_CN00;

    char replyData[1000];
    int  replyLen;

    char szStamp[20];
    DBMSrvBHist_HistoryFile TempBackupHistory;

    setVControlReplyBufferAndStandardErrorHandler(cn00DBMServerData::vcontrol(), replyData, &replyLen, 999);

    //read the history file (forward) into a list. if backwards reading is implemented, this
    //part has to be implemented into the rest of the function
    if(OK_CN00==rc)
        rc=TempBackupHistory.readFile(reply);

    if(OK_CN00==rc)
        History.splice(History.end(), TempBackupHistory.getHistoryList());  //move the content of TempBackupHistory to History

    if(OK_CN00==rc)
    {
        // compute backup timestamp
        if(20>strlen(Result->szDBStamp1Date)+strlen(Result->szDBStamp1Time+2))
        {
            SAPDB_strcpy(szStamp, Result->szDBStamp1Date);
            strcat(szStamp, Result->szDBStamp1Time+2);
        }
        else
            rc=reply.startWithError(ERR_CN00);
    }

    if(OK_CN00==rc)
    {
        Tools_List<DBMSrvBHist_Backup*>::reverse_iterator Backup=History.rbegin();
        Tools_List<DBMSrvBHist_Backup*>::reverse_iterator Match=History.rend();
        int                                               lineNumber=1;

        for(; Backup!=History.rend() && Match==History.rend(); ++Backup, ++lineNumber)
        {
            if((*Backup)->HasTimeStamp(szStamp))
                Match=Backup; //last log backup matching Result
        }

        if(Match!=History.rend())
        {
            while(OK_CN00==rc && Backup!=History.rend() && (*Backup)->HasSameKeyAs(**Match))
            {
                const char * StartDate=(*Backup)->GiveStartDate();
                const char * DBStamp1 =(*Backup)->GiveDBStamp1();

                if(strlen(StartDate)<19 || strlen(DBStamp1)<19)
                {
                    DBMSrvMsg_Error error(SDBMSG_DBMSRV_KNLDIRTY,
                                          Msg_Arg(SDBMSGTAG_DBMSRV_KNLDIRTY__FILENAME, TempBackupHistory.getFileName()),
                                          Msg_Arg(SDBMSGTAG_DBMSRV_KNLDIRTY__LINE_NO, lineNumber));
                    rc=reply.startWithMessageList(error);
                }
                else
                {
                    sprintf(Result->szDate, "%c%c%c%c%c%c%c%c", StartDate[0],
                                                                StartDate[1],
                                                                StartDate[2],
                                                                StartDate[3],
                                                                StartDate[5],
                                                                StartDate[6],
                                                                StartDate[8],
                                                                StartDate[9]);
                    sprintf(Result->szTime, "00%c%c%c%c%c%c",   StartDate[11],
                                                                StartDate[12],
                                                                StartDate[14],
                                                                StartDate[15],
                                                                StartDate[17],
                                                                StartDate[18]);

                    sprintf(Result->szPagesTransfered, "%d", atoi(Result->szPagesTransfered) + atoi((*Backup)->GivePagesTransfered()));

                    cn36_StrNCpy(Result->szFirstLogPageNo, (*Backup)->GiveStartLogPageAsString(), sizeof(Result->szFirstLogPageNo)-1);

                    sprintf(Result->szDBStamp1Date, "%c%c%c%c%c%c%c%c", DBStamp1[0],
                                                                        DBStamp1[1],
                                                                        DBStamp1[2],
                                                                        DBStamp1[3],
                                                                        DBStamp1[5],
                                                                        DBStamp1[6],
                                                                        DBStamp1[8],
                                                                        DBStamp1[9]);
                    sprintf(Result->szDBStamp1Time, "00%c%c%c%c%c%c",   DBStamp1[11],
                                                                        DBStamp1[12],
                                                                        DBStamp1[14],
                                                                        DBStamp1[15],
                                                                        DBStamp1[17],
                                                                        DBStamp1[18]);

                    sprintf(Result->szVolumes, "%d", atoi(Result->szVolumes) + atoi((*Backup)->GiveVolumes()));
                }

                ++Backup;
                ++lineNumber;
            }
        }
    }

    return rc;
}

tcn00_Error DBMSrvBHist_History::ReadBasicHistories(const char * DBName,
                                                    char       * replyData,
                                                    int        * replyLen)
{
    tcn00_Error rc=OK_CN00;

    CheckEBIDs=false; //don't check availability of external backup

    setVControlAndReplyBuffer(cn00DBMServerData::vcontrol(), replyData, replyLen, 100);

    return ReadHistories();
}

void DBMSrvBHist_History::ReduceToFileLogBackupsWithName(const char * AbsoluteName,
                                                         const char * DefaultDirectory)
{
    Tools_List<DBMSrvBHist_Backup *>::iterator Backup=History.begin();

    while(Backup!=History.end())
    {
        if((*Backup)->IsALogBackup() && (*Backup)->MatchesFileNameElseReduce(AbsoluteName, DefaultDirectory))
            ++Backup;
        else
        {
            delete (*Backup);
            Backup=History.erase(Backup); //erase everything that is not a matching log backup
        }
    }
}

int DBMSrvBHist_History::ContainsFileBackup(const char                 *  BaseName,
                                            const char                 *  labelFromBackup,
                                            const tcn36_DateTimeString  & backupStartDateTime,
                                            const char                 *& BackupKey,
                                            const char                 *& BackupLabel)
{
    int rc=0;

    Tools_List<DBMSrvBHist_Backup *>::iterator Backup=History.begin();

    BackupKey=BackupLabel=0;

    while(Backup!=History.end())
    {
        if((*Backup)->ContainsFileBackup(BaseName, labelFromBackup, backupStartDateTime)) //label and time must match
        {
            rc=1;
            BackupKey=(*Backup)->GiveKey();
            BackupLabel=(*Backup)->GiveLabel();
            Backup=History.end();
        }
        else
            ++Backup;
    }

    return rc;
}

const Tools_List<DBMSrvBHist_Backup *> & DBMSrvBHist_History::GetHistoryList() const
{
    return History;
}

tcn00_Error DBMSrvBHist_History::ReadHistories(int UseMediaHistory)
{
    tcn00_Error rc=OK_CN00;

    //read dbm.knl, dbm.mdf and also dbm.ebf (it contains medium information of stage backups) 

    DBMSrvBHist_HistoryFile      TempBackupHistory;
    DBMSrvBHist_MediaHistoryFile TempMediaBackupHistory;
    DBMSrvBHist_EBIDHistoryFile  TempExternalBackupHistory;

    //read the history files (forward) into lists. if backwards reading is implemented, this
    //part has to be implemented into the join algorithm
    if(OK_CN00==rc)
        rc=TempBackupHistory.readFile(reply);

    if(OK_CN00==rc)
        History.splice(History.end(), TempBackupHistory.getHistoryList());  //move the content of TempBackupHistory to History

    if(OK_CN00==rc && UseMediaHistory)
        rc=TempMediaBackupHistory.readFile(reply);

    if(OK_CN00==rc)
        rc=TempExternalBackupHistory.readFile(reply);

    //now join the 3 lists into the final backup history structure
    if(OK_CN00==rc && UseMediaHistory)
        rc=JoinHistoryWithMediaHistory(TempMediaBackupHistory.getMediaList());

    if(OK_CN00==rc)
        rc=JoinHistoryWithEBIDHistory(TempExternalBackupHistory.getEBIDList());

    //check availability of external backups
    if(OK_CN00==rc && CheckEBIDs)
        rc=CheckAvailabilityOfExternalBackups(TempExternalBackupHistory.getEBIDList());

    //manipulate return code for external backup tools
    for(Tools_List<DBMSrvBHist_Backup *>::iterator Backup=History.begin(); OK_CN00==rc && Backup!=History.end(); ++Backup)
    {
        rc=(*Backup)->AdjustReturnCode();

        if(OK_CN00!=rc)
            reply.startWithError(rc);
    }

    return rc;
}

enum DBMSrvBHist_History_Type{Data=0, Log=1};
enum DBMSrvBHist_History_ListType{Backups, Media, EBIDs, EBIDsTool};

tcn00_Error DBMSrvBHist_History::JoinHistoryWithMediaHistory(Tools_List<DBMSrvBHist_Part *> & MediaList)
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_Backup *>::reverse_iterator Backup=History.rbegin();
    Tools_List<DBMSrvBHist_Part *>::reverse_iterator   Medium=MediaList.rbegin();
    Tools_List<DBMSrvBHist_Part *>::iterator           MediumHelp;

    Tools_List<DBMSrvBHist_Backup *> UnresolvedBackups[2];
    Tools_List<DBMSrvBHist_Part *>   UnresolvedMedia[2];

    DBMSrvBHist_History_ListType CurrentList=Backups;

    while(OK_CN00==rc && (Backup!=History.rend() || Medium!=MediaList.rend()))
    {
        int FoundAMatch=0;

        //during one loop check the last backup against the unresolved media
        if(Backups==CurrentList && Backup!=History.rend())
        {
            DBMSrvBHist_History_Type Type=(*Backup)->IsALog()?Log:Data; //handle log backups separate from other entries

            Tools_List<DBMSrvBHist_Part *>::iterator UnresolvedMedium=UnresolvedMedia[Type].begin();

            while(OK_CN00==rc && !FoundAMatch && UnresolvedMedium!=UnresolvedMedia[Type].end()) //check backup against all unresolved backups
            {
                if((*Backup)->HasKeyAndLabel((*UnresolvedMedium)->GiveKey(), (*UnresolvedMedium)->GiveLabel())) //we found a medium matching Backup
                {
                    FoundAMatch=1;
                    UnresolvedBackups[Type].clear();                                              //all backups of same type and newer than Backup can never match anything anymore (dbm.knl+dbm.mdf are written strictly ordered by time for Log and for the rest)
                    //all newer medium information of same type can also not match anything (dbm.knl+dbm.mdf are written strictly ordered by time for Log and for the rest)
                    for(MediumHelp=UnresolvedMedia[Type].begin(); MediumHelp!=UnresolvedMedium; ++MediumHelp) //so delete all those non-matching media parts
                        delete (*MediumHelp);
                    UnresolvedMedia[Type].erase(UnresolvedMedia[Type].begin(), UnresolvedMedium); //and then throw away the corresponding (now illegal pointing) list elements

                    do //append all matching media (parallel backups, backups with replace) to Backup
                    {
                        rc=(*Backup)->AddPart(*UnresolvedMedium);

                        if(OK_CN00==rc)
                            UnresolvedMedium=UnresolvedMedia[Type].erase(UnresolvedMedium); //don't delete corresponding media part, because (*Backup) is now pointing to it
                        else
                            reply.startWithError(rc);
                    }
                    while(OK_CN00==rc &&                                    //append as long as nothing went wrong ...
                          UnresolvedMedium!=UnresolvedMedia[Type].end() &&  //... and as long as there are unresolved backups
                          (*Backup)->HasKeyAndLabel((*UnresolvedMedium)->GiveKey(), (*UnresolvedMedium)->GiveLabel())); //and as long key and label still match
                }
                else
                    ++UnresolvedMedium; //it just did not match, have a look onto the next
            }

            if(OK_CN00==rc && !UnresolvedBackups[Type].push_back(*Backup)) //consider backup as unresolved because it could be one with a replacement
                rc=reply.startWithError(ERR_MEM_CN00);

            ++Backup;
        }

        //during another loop check the last medium against the unresolved backups
        if(Media==CurrentList && Medium!=MediaList.rend())
        {
            DBMSrvBHist_History_Type Type=(*Medium)->IsALog()?Log:Data; //determine type of backup (handle log backups separate from other entries)

            Tools_List<DBMSrvBHist_Backup *>::iterator UnresolvedBackup=UnresolvedBackups[Type].begin();

            while(OK_CN00==rc && !FoundAMatch && UnresolvedBackup!=UnresolvedBackups[Type].end())
            {
                if((*UnresolvedBackup)->HasKeyAndLabel((*Medium)->GiveKey(), (*Medium)->GiveLabel()))
                {
                    FoundAMatch=1;
                    UnresolvedBackups[Type].erase(UnresolvedBackups[Type].begin(), UnresolvedBackup); //all backups of the same type and newer than UnresolvedBackup can never match anything anymore (dbm.knl+dbm.mdf are written strictly ordered by time for Log and for the rest)
                    //all newer medium information of same type can also not match anything (dbm.knl+dbm.mdf are written strictly ordered by time for Log and for the rest)
                    for(MediumHelp=UnresolvedMedia[Type].begin(); MediumHelp!=UnresolvedMedia[Type].end(); ++MediumHelp) //so delete all those non-matching media parts
                        delete (*MediumHelp);
                    UnresolvedMedia[Type].clear(); //and then throw away the corresponding (now illegal pointing) list elements

                    do
                    {
                        rc=(*UnresolvedBackup)->AddPart(*Medium);

                        if(OK_CN00==rc)
                            Medium=MediaList.erase(Medium); //don't delete corresponding media part, because (*Backup) is now pointing to it or it was already deleted by AddPart
                        else
                            reply.startWithError(rc);
                    }
                    while(OK_CN00==rc &&                     //append as long as nothing went wrong ...
                          Medium!=MediaList.rend() &&        //... and as long as there are unresolved backups
                          (*UnresolvedBackup)->HasKeyAndLabel((*Medium)->GiveKey(), (*Medium)->GiveLabel())); //and as long key and label still match
                }
                else
                    ++UnresolvedBackup; //it just did not match, have a look onto the next medium
            }

            if(OK_CN00==rc && !FoundAMatch)
            {
                do
                {
                    if(UnresolvedMedia[Type].push_back(*Medium))
                        Medium=MediaList.erase(Medium);
                    else
                        rc=reply.startWithError(ERR_MEM_CN00);
                }
                while(OK_CN00==rc &&
                      Medium!=MediaList.rend() &&
                      (*Medium)->HasSameKeyAndLabelAs(*UnresolvedMedia[Type].back()));
            }
        }

        if(Backups==CurrentList)    //work alternating on backup and media lists
            CurrentList=Media;
        else
            CurrentList=Backups;
    }

    //get rid of all parts still unresolved (they never had a chance to show up in the backup list :( )
    for(MediumHelp=MediaList.begin(); MediumHelp!=MediaList.end(); ++MediumHelp)
        delete (*MediumHelp);

    for(MediumHelp=UnresolvedMedia[Data].begin(); MediumHelp!=UnresolvedMedia[Data].end(); ++MediumHelp)
        delete (*MediumHelp);

    for(MediumHelp=UnresolvedMedia[Log].begin(); MediumHelp!=UnresolvedMedia[Log].end(); ++MediumHelp)
        delete (*MediumHelp);

    return rc;
}

tcn00_Error DBMSrvBHist_History::JoinHistoryWithEBIDHistory(Tools_List<DBMSrvBHist_Part *> & EBIDList)
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_Backup *>::reverse_iterator Backup=History.rbegin();
    Tools_List<DBMSrvBHist_Part *>::reverse_iterator   EBID=EBIDList.rbegin();

    Tools_List<DBMSrvBHist_Backup *> UnresolvedBackups;
    Tools_List<DBMSrvBHist_Part *>   UnresolvedEBIDs;

    Tools_List<DBMSrvBHist_Backup *>::iterator LastDataMatch=UnresolvedBackups.end();

    DBMSrvBHist_History_ListType CurrentList=Backups;

    while(OK_CN00==rc && (Backup!=History.rend() || EBID!=EBIDList.rend()))
    {
        int FoundAMatch=0;

        if(Backups==CurrentList && Backup!=History.rend())
        {
            Tools_List<DBMSrvBHist_Part *>::iterator UnresolvedEBID=UnresolvedEBIDs.begin();

            while(OK_CN00==rc && !FoundAMatch && UnresolvedEBID!=UnresolvedEBIDs.end())
            {
                if((*Backup)->HasKeyAndLabel((*UnresolvedEBID)->GiveKey(), (*UnresolvedEBID)->GiveLabel())) //we found a EBID matching a Backup
                {
                    FoundAMatch=1;

                    if(((*Backup)->IsAData() || (*Backup)->IsAPages()) && LastDataMatch!=UnresolvedBackups.end())
                        UnresolvedBackups.erase(UnresolvedBackups.begin(), ++LastDataMatch);  //we can forget about any backups upto (including) the last matched data/pages backup, because those backups can never have created a matching entry in the dbm.ebf before UnresolvedEBID and therefore in the part of EBIDList not yet processed
                        //we can not forget about anything in UnresolvedEBIDs, because of the archive_stage-commands

                    //add the first match
                    rc=(*Backup)->AddPart(*UnresolvedEBID);

                    if(OK_CN00==rc)
                        UnresolvedEBID=UnresolvedEBIDs.erase(UnresolvedEBID);
                    else
                        reply.startWithError(rc);

                    //search for other matches for the same backup (e.g. from other archived copies)
                    while(OK_CN00==rc &&                            //search as long as nothing went wrong ...
                          UnresolvedEBID!=UnresolvedEBIDs.end())   //... and as long as there are any unresolved EBIDs
                    {
                        if((*Backup)->HasKeyAndLabel((*UnresolvedEBID)->GiveKey(), (*UnresolvedEBID)->GiveLabel()))//find all, where key and label match
                        {
                            rc=(*Backup)->AddPart(*UnresolvedEBID);

                            if(OK_CN00==rc)
                                UnresolvedEBID=UnresolvedEBIDs.erase(UnresolvedEBID);
                            else
                                reply.startWithError(rc);
                        }
                        else
                            ++UnresolvedEBID; //ignore all non-matching Unresolved EBIDs
                    }

                    if(OK_CN00==rc && !UnresolvedBackups.push_back(*Backup))  //consider backup as unresolved, as parts of other copies may follow
                        rc=reply.startWithError(ERR_MEM_CN00);

                    if(OK_CN00==rc && ((*Backup)->IsAData() || (*Backup)->IsAPages()))
                        LastDataMatch=(--UnresolvedBackups.end());  //the newly added data backup is the last match
                }
                else
                    ++UnresolvedEBID; //it just did not match, have a look onto the next
            }

            if(OK_CN00==rc && !FoundAMatch && !UnresolvedBackups.push_back(*Backup))
                rc=reply.startWithError(ERR_MEM_CN00);

            ++Backup;
        }

        if(EBIDs==CurrentList && EBID!=EBIDList.rend())
        {
            Tools_List<DBMSrvBHist_Backup *>::iterator UnresolvedBackup=UnresolvedBackups.begin();

            while(OK_CN00==rc && !FoundAMatch && UnresolvedBackup!=UnresolvedBackups.end())
            {
                if((*UnresolvedBackup)->HasKeyAndLabel((*EBID)->GiveKey(), (*EBID)->GiveLabel()))
                {
                    FoundAMatch=1;

                    if(((*UnresolvedBackup)->IsAData() || (*UnresolvedBackup)->IsAPages()) && LastDataMatch!=UnresolvedBackups.end())
                        UnresolvedBackups.erase(UnresolvedBackups.begin(), ++LastDataMatch);  //we can forget about any backups upto (including) the last matched data/pages backup

                    do
                    {
                        rc=(*UnresolvedBackup)->AddPart(*EBID);

                        if(OK_CN00==rc)
                            ++EBID;
                        else
                            reply.startWithError(rc);
                    }
                    while(OK_CN00==rc &&                                                                  //append as long as nothing went wrong ...
                          EBID!=EBIDList.rend() &&                                                        //... and as long as there are unresolved backups
                          (*UnresolvedBackup)->HasKeyAndLabel((*EBID)->GiveKey(), (*EBID)->GiveLabel())); //and as long key and label still match

                    if(OK_CN00==rc && ((*UnresolvedBackup)->IsAData() || (*UnresolvedBackup)->IsAPages()))
                        LastDataMatch=UnresolvedBackup;  //UnresolvedBackup is now the last match
                }
                else
                    ++UnresolvedBackup; //it just did not match, have a look onto the next medium
            }

            if(!FoundAMatch)
            {
                do
                {
                    if(!UnresolvedEBIDs.push_back(*EBID))
                        rc=reply.startWithError(ERR_MEM_CN00);
                    
                    if(OK_CN00==rc)
                        ++EBID;
                }
                while(OK_CN00==rc &&
                      EBID!=EBIDList.rend() &&
                      (*EBID)->HasSameKeyAndLabelAs(*UnresolvedEBIDs.back()));
            }
        }

        if(Backups==CurrentList)    //work alternating on backup list and EBID list
            CurrentList=EBIDs;
        else
            CurrentList=Backups;
    }

    return rc;
}

tcn00_Error DBMSrvBHist_History::JoinEBIDHistoryWithToolData(const DBMSrvBTools_ToolEnum      ToolType,
                                                             Tools_List<DBMSrvBHist_Part *> & EBIDList,
                                                             Tools_List<DBMSrvBHist_Part *> & EBIDsFromTool)
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_Part *>::reverse_iterator EBID=EBIDList.rbegin();
    Tools_List<DBMSrvBHist_Part *>::reverse_iterator ToolEBID=EBIDsFromTool.rbegin();

    Tools_List<DBMSrvBHist_Part *> UnresolvedEBIDs;
    Tools_List<DBMSrvBHist_Part *> UnresolvedToolEBIDs;

    DBMSrvBHist_History_ListType CurrentList=EBIDs;

    while(OK_CN00==rc && (EBID!=EBIDList.rend() || ToolEBID!=EBIDsFromTool.rend()))
    {
        int FoundAMatch=0;

        if(EBIDs==CurrentList && EBID!=EBIDList.rend())
        {
            if((*EBID)->GiveUsedBackupTool()==ToolType)
            {
                Tools_List<DBMSrvBHist_Part *>::iterator UnresolvedToolEBID=UnresolvedToolEBIDs.begin();

                while(OK_CN00==rc && !FoundAMatch && UnresolvedToolEBID!=UnresolvedToolEBIDs.end())
                {
                    if((*EBID)->HasSameEBIDAs(**UnresolvedToolEBID))
                    {
                        FoundAMatch=1;
                        (*EBID)->SetEBIDStatus(**UnresolvedToolEBID); //take over the determined status
                    
                        UnresolvedToolEBID=UnresolvedToolEBIDs.erase(UnresolvedToolEBID);   //that one is now resolved
                    }
                    else
                        ++UnresolvedToolEBID;
                }

                if(!FoundAMatch && !UnresolvedEBIDs.push_back(*EBID))
                    rc=reply.startWithError(ERR_MEM_CN00);
            }

            ++EBID;
        }

        if(EBIDsTool==CurrentList && ToolEBID!=EBIDsFromTool.rend())
        {
            Tools_List<DBMSrvBHist_Part *>::iterator UnresolvedEBID=UnresolvedEBIDs.begin();

            while(OK_CN00==rc && !FoundAMatch && UnresolvedEBID!=UnresolvedEBIDs.end())
            {
                if((*UnresolvedEBID)->HasSameEBIDAs(**ToolEBID))
                {
                    FoundAMatch=1;
                    (*UnresolvedEBID)->SetEBIDStatus(**ToolEBID); //take over the determined status

                    UnresolvedEBID=UnresolvedEBIDs.erase(UnresolvedEBID);   //that one is now resolved
                }
                else
                    ++UnresolvedEBID;
            }

            if(!FoundAMatch && !UnresolvedToolEBIDs.push_back(*ToolEBID))
                rc=reply.startWithError(ERR_MEM_CN00);

            ++ToolEBID;
        }

        if(EBIDs==CurrentList)    //work alternating on the EBID list from history and the EBID list from the backup tool
            CurrentList=EBIDsTool;
        else
            CurrentList=EBIDs;
    }

    return rc;
}

tcn00_Error DBMSrvBHist_History::CheckAvailabilityOfExternalBackups(Tools_List<DBMSrvBHist_Part *> & EBIDList)
{
    tcn00_Error             rc=OK_CN00;
    DBMSrvBTools_ToolEnum * BackupToolType=0;
    const int               NumberOfBackupTools=DBMSrvBTools_ToolEnum::NumberOfBackupToolConnectors();

    //as we are now asking the backup tools, all EBIDs must change status from UNKOWN to UNAVAILABLE (because from most tools we get just a list of available backups)
    for(Tools_List<DBMSrvBHist_Part *>::iterator h=EBIDList.begin(); h!=EBIDList.end(); ++h)
        (*h)->SetEBIDStatusToUnavailable();

    BackupToolType=new DBMSrvBTools_ToolEnum[NumberOfBackupTools+1];

    if(0==BackupToolType)
        rc=reply.startWithError(ERR_MEM_CN00);
    else
    {
        int i;
        Tools_List<DBMSrvBHist_Backup *>::iterator Backup;

        for(i=0; i<NumberOfBackupTools; ++i)
            BackupToolType[i]=DBMSrvBTools_ToolEnum(DBMSrvBTools_ToolEnum::None);

        //determine the used backup tools (from the already joined history) and init BackupToolType with that information
        for(Backup=History.begin(); Backup!=History.end(); ++Backup)
            (*Backup)->MarkUsedExternalBackupTools(BackupToolType);

        //determine the available backups for every used external backup tool
        //and check against all backups known through pHistoryEBIDs
        for(i=0; i<NumberOfBackupTools; ++i)
            if(BackupToolType[i]!=DBMSrvBTools_ToolEnum::None)
            {
                tcn35b_DBMExternalBackupController * CurrentTool=0;

                rc=TheDBMExternalBackupControllerFactory_cn35b.ConstructADBMExternalBackupController(CurrentTool,
                                                                                                     reply.giveData(),
                                                                                                     reply.giveLength(),
                                                                                                     tcn35d_BackupInfo::GiveBackupInfo,
                                                                                                     BackupToolType[i],
                                                                                                     VControl->dbname);

                if(OK_CN00==rc)   //BackupController was constructed successfully
                {
                    DBMSrvBHist_EBIDList AvailableEBIDs;

                    if(!CurrentTool->ListEBIDsOfAvailableBackups(AvailableEBIDs))   // do not use CurrentTool->GiveLastError() if Tool is not installed anymore
                        rc=CurrentTool->GiveLastError();                            // do not ignore "real" errors

                    setStandardErrorHandler(); //reset the error handler changed by the backup controller object

                    if(OK_CN00==rc)                //all ok
                        rc=JoinEBIDHistoryWithToolData(BackupToolType[i], EBIDList, AvailableEBIDs.GiveList());
                }
                //else    //error was already reported

                if(0!=CurrentTool)
                    delete CurrentTool;

                setStandardErrorHandler(); //reset the error handler changed by the backup controller object
            }
            
        delete [] BackupToolType; // free the allocated memory
    }

    return rc;
}

void DBMSrvBHist_History::setVControlAndReplyBuffer(VControlDataT * NewVControl,
                                                    char          * NewReplyData,
                                                    int           * NewReplyLen,
                                                    int             NewReplyLenMax)
{
    VControl   =NewVControl;

    reply.setTo(NewReplyData, NewReplyLen, NewReplyLenMax);
}

void DBMSrvBHist_History::setStandardErrorHandler()
{
    tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.SetAsCurrentErrorHandler();
    tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.SetReplyArrayTo(0, 0);
}

void DBMSrvBHist_History::setVControlReplyBufferAndStandardErrorHandler(VControlDataT * NewVControl,
                                                                        char          * NewReplyData,
                                                                        int           * NewReplyLen,
                                                                        int             NewReplyLenMax)
{
    setVControlAndReplyBuffer(NewVControl, NewReplyData, NewReplyLen, NewReplyLenMax);
    setStandardErrorHandler();
}
