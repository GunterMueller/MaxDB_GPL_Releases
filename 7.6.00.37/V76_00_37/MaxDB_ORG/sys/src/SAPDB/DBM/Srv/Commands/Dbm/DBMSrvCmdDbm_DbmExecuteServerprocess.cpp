/*!
  \file    DBMSrvCmd_DbmExecuteServerProcess.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   command dbm_execute_serverprocess

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

#include "hni33.h"

#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmExecuteServerprocess.hpp"
#include "DBM/Srv/Process/DBMSrvProc_DeamonProcess.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#ifdef _WIN32 
  #define DEVICE_NULL  "nul"
#else
  #define DEVICE_NULL  "/dev/null"
#endif


DBMSrvCmdDbm_DbmExecuteServerprocess::DBMSrvCmdDbm_DbmExecuteServerprocess()
    : DBMSrv_Command( DBMSrv_Command::KeyDbmExecuteServerprocess, false ) {
} // end DBMSrvCmdDbm_DbmExecuteServerprocess::DBMSrvCmdDbm_DbmExecuteServerprocess

tcn00_Error DBMSrvCmdDbm_DbmExecuteServerprocess::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {
    
    theReply.startWithOK();

    Msg_List executionMessages;
    executionMessages.ClearMessageList();

    runInProcess(command->args, executionMessages);

    if( executionMessages.IsEmpty() )
        return OK_CN00;
    else {
        return theReply.startWithMessageList(executionMessages);
    }
} // end DBMSrvCmdDbm_DbmExecuteServerprocess::run

void DBMSrvCmdDbm_DbmExecuteServerprocess::runInProcess(const char* aCommandLine,
        Msg_List& aMessageList) {

    // commandline converter
    DBMSrvProc_ServerProcessCommandLineConverter theConverter(aCommandLine);
    if( !theConverter.isValid() ) {
        // fill some error into message list
        char* commandLineCopy(NULL);
        DBMSrvMsg_Error removePasswordError;
        if( theConverter.removePasswordFromCommandLine(aCommandLine, commandLineCopy, removePasswordError) ) {
            aMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_INSERVERPROCESSSTARTER,Msg_Arg(SDBMSGTAG_DBMSRV_INSERVERPROCESSSTARTER__DBMSRV_COMP,commandLineCopy)));
            delete commandLineCopy;
        }
        else
            aMessageList.Overrule(removePasswordError);
        return;
    }

    bool errorFound(false);
    if( theConverter.isPipeCall() ) {
      errorFound = ( 0 != Tools_PipeCall::CallProgram(
                              theConverter.getConvertedCommandLineReference(),
                              Tools_PipeCall::CallAsynchron,
                              DEVICE_NULL,
                              DEVICE_NULL) );
    }
    else {
        DBMSrvProc_DeamonProcess logoffsaveProcess(
                theConverter.getConvertedCommandLineReference());
        if( !logoffsaveProcess.isRunning() ) {
            logoffsaveProcess.getMessagesInto(aMessageList);
            errorFound = true;
        }
        //printf(">> the pid is %d\n", (int)logoffsaveProcess.getPid());
    }
    if( errorFound ) {
        char* commandLineCopy(NULL);
        DBMSrvMsg_Error removePasswordError;
        if( theConverter.removePasswordFromCommandLine(theConverter.getConvertedCommandLineReference(), commandLineCopy, removePasswordError) ) {
            aMessageList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_INSERVERPROCESSSTARTER,Msg_Arg(SDBMSGTAG_DBMSRV_INSERVERPROCESSSTARTER__DBMSRV_COMP,commandLineCopy)));
            delete commandLineCopy;
        }
        else
            aMessageList.Overrule(removePasswordError);
    }
} // end DBMSrvCmdDbm_DbmExecuteServerprocess::runInProcess
