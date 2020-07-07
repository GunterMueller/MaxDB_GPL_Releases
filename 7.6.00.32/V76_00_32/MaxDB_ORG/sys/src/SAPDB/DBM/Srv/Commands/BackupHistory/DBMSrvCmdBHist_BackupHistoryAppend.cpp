/*!
    \file    DBMSrvCmdBHist_BackupHistoryAppend.cpp
    \author  TiloH
    \ingroup DBMServer commands
    \brief   definition of a class handling DBM Server command
             backup_history_append

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#include "hcn36.h"
#include "hcn42.h"
#include "hcn90.h"

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileWriterEBF.hpp"
#include "SAPDB/DBM/Srv/Commands/BackupHistory/DBMSrvCmdBHist_BackupHistoryAppend.hpp"
#include "SAPDB/ToolsCommon/Tools_List.hpp"

DBMSrvCmdBHist_BackupHistoryAppend::DBMSrvCmdBHist_BackupHistoryAppend()
    :DBMSrv_Command(DBMSrv_Command::KeyBackupHistoryAppend, false, 0, true) //not obsolete, but hidden
{
    m_longHelp=
        "@command backup_history_append With this command you append something to the External Backup History."
        "@preconditions You have the DBM operator authorization Backup."
        "@syntax backup_history_append <new_history_line> ... "
        "@param <new_history_line> The line that should be inserted into the External Backup History."
        "@reply OK";
}

tcn00_Error DBMSrvCmdBHist_BackupHistoryAppend::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & reply)
{
    tcn00_Error     rc=OK_CN00;
    DBMSrvMsg_Error error;

    const char * historyLine=0;
    const char * historyLineEnd=0;

    Tools_List<char *> lineList;
    Tools_List<char *>::iterator lineListIterator;

    int parameterCount=1;


    cn90GetStartAndEndOfToken(command->args, historyLine, historyLineEnd, parameterCount++); //get the first parameter

    //analyze the list of parameters
    while(0!=historyLine && historyLine!=historyLineEnd)
    {
        char * historyLineCopy=0;

        // if the value contains white spaces, it is enclosed in a pair of double quotes
        cn36_GetStringBetweenDoubleQuotesOrWhiteSpaces(historyLine, historyLineEnd);

        //make a zero terminated copy of the line and append it to a list
        if(!cn36_StrNAllocCpy(historyLineCopy, historyLine, historyLineEnd-historyLine) ||
            !lineList.push_back(historyLineCopy))
        {
            rc=reply.startWithError(ERR_MEM_CN00);
        }

        cn90GetStartAndEndOfToken(command->args, historyLine, historyLineEnd, parameterCount++); //get the next parameter
    }

    if(OK_CN00==rc)
    {
        DBMSrvBHist_FileWriterEBF dbmebf;
        tsp00_Pathc               fileName;

        //get the absolute file name of dbm.ebf
        cn42GetFileName(cn00DBMServerData::vcontrol()->dbname, FGET_BACKEBF_CN42, fileName);

        //open dbm.ebf
        if(!dbmebf.openFile((char *)fileName, error))
            rc=reply.startWithMessageList(error);
        else
        {
            bool errorWasJustInvalidLine=false;

            for(lineListIterator=lineList.begin(); (OK_CN00==rc || errorWasJustInvalidLine) && lineListIterator!=lineList.end(); lineListIterator++)
            {
                DBMSrvBHist_Part part(DBMSrvBHist_Part::PartFromEBIDFile); //not really from dbm.ebf but for dbm.ebf
                error.ClearMessageList();

                if(part.SetToEBID(*lineListIterator, error))
                {
                    if(!dbmebf.appendEntry(part, error))
                    {
                        errorWasJustInvalidLine=false;              //errors during append overrule a DBMSrvMsg_Error::BHIST_EXT_INVALID_LINE
                        rc=reply.startWithMessageList(error); 
                    }
                }
                else
                {
                    errorWasJustInvalidLine=(DBMSrvMsg_Error::DBMSrv_BHIST_EXT_INVALID_LINE==error);

                    if(OK_CN00==rc || !errorWasJustInvalidLine) //no error occured yet or last error was serious 
                        rc=reply.startWithMessageList(error);
                }
            }

            if(!dbmebf.closeFile(error))
                rc=reply.startWithMessageList(error);
        }
    }

    for(lineListIterator=lineList.begin(); lineListIterator!=lineList.end(); lineListIterator++)
        cn36_StrDealloc(*lineListIterator);
    
    reply.startWithOKIfRCIsOk(rc);

    return rc;
}

tcn00_Error DBMSrvCmdBHist_BackupHistoryAppend::runCommand(
    VControlDataT * vcontrol,
    CommandT      * command,
    char          * replyData,
    int           * replyLen,
    int             replyLenMax)
{
    DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
    DBMSrvCmdBHist_BackupHistoryAppend commandObject;
    
    return commandObject.run(vcontrol, command, reply);
}
