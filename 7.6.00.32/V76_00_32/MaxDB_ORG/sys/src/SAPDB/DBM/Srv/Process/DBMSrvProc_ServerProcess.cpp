/*!
  \file    DBMSrvProc_ServerProcess.cpp
  \author  MarcW
  \brief   server process implementation

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

#include "gcn00.h"
#include "hcn50.h"
#include "heo01.h"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp"
#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmExecuteServerprocess.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

DBMSrvProc_ServerProcess::DBMSrvProc_ServerProcess(
            const DBMSrvProc_ServerProcessCommandLineConverter::ProgramKey aProgramKey,
            DBMSrvProc_ArgumentList& aArgList )
            : m_Started(false) {

    DBMSrvProc_ArgumentList theArgList;
    m_ErrorMessages.ClearMessageList();

    // if communication is local between us and client, we must start server process
    // remotely, otherwise we can start it in process
    // The detour of starting a process through a service is only necessary, if we're on
    // windows and we have local communication!!

#if defined (WIN32)
    bool localCommunication(cn00DBMServerData::vcontrol()->bIsLocalCommunication);

    // if we use U3 - environment variable SDB_PORTABLE_ROOT has a value - we don't use
    // the x_server
    if (localCommunication) {
        tsp00_Pathc    Path;
        tsp01_RteError RteError;
        // check the environment variable (SDB_PORTABLE_ROOT), function returns true if set or false
        // if not set
        localCommunication = !sqlGetPortableInstallationRootPath (Path, TERM_WITHOUT_DELIMITER_EO01, &RteError);
    } // end if
#else
    bool localCommunication(false);
#endif

    if( localCommunication )
        theArgList.AddArgument("dbm_execute_serverprocess");

    char keyBuf[128];
    sprintf(keyBuf, "%d", (int) aProgramKey);
    theArgList.AddArgument(keyBuf);
    theArgList.addAllArgumentsOf(aArgList);
    if( localCommunication ) {
        DBMCli_String serverName(m_LocalHostText);
        DBMCli_String dbName(cn00DBMServerData::vcontrol()->dbname);
        DBMCli_String userName;
        userName = cn00DBMServerData::vcontrol()->pCurrentUser->getUserName();
        userName += ',';
        userName += cn00DBMServerData::vcontrol()->pCurrentUser->getClearMasterPwd();

        DBMCli_Database myDB(
            serverName,
            dbName,
            userName,
            m_ErrorMessages,
            false);

        do {
            if( !m_ErrorMessages.IsEmpty() || !myDB.Connect(m_ErrorMessages) )
                break;

            if( !myDB.Command(theArgList.GiveCommandLine(), m_ErrorMessages) )
                break;
            
            // now check result
            DBMCli_Result cmdResult = myDB.GetResult();
            if( !cmdResult.Check(m_ErrorMessages) )
                break;

            m_Started = true;
            return;
        } while(true);
        // starting server process returned an error
        char* commandLineCopy(NULL);
        DBMSrvMsg_Error removePasswordError;
        if( DBMSrvProc_ServerProcessCommandLineConverter::removePasswordFromCommandLine(theArgList.GiveCommandLine(), commandLineCopy, removePasswordError) ) {
            m_ErrorMessages.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOSERVERPROCESSRUNNING,Msg_Arg(SDBMSGTAG_DBMSRV_NOSERVERPROCESSRUNNING__DBMSRV_COMP,commandLineCopy)));
            delete commandLineCopy;
        }
        else
            m_ErrorMessages.Overrule(removePasswordError);
    }
    else {
        DBMSrvCmdDbm_DbmExecuteServerprocess cmdLogoffsave;
        cmdLogoffsave.runInProcess(theArgList.GiveCommandLine(), m_ErrorMessages);
        m_Started = m_ErrorMessages.IsEmpty();
        if( !m_Started ) {
            char* commandLineCopy(NULL);
            DBMSrvMsg_Error removePasswordError;
            if( DBMSrvProc_ServerProcessCommandLineConverter::removePasswordFromCommandLine(theArgList.GiveCommandLine(), commandLineCopy, removePasswordError) ) {
                m_ErrorMessages.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOSERVERPROCESSRUNNING,Msg_Arg(SDBMSGTAG_DBMSRV_NOSERVERPROCESSRUNNING__DBMSRV_COMP,commandLineCopy)));
                delete commandLineCopy;
            }
            else
                m_ErrorMessages.Overrule(removePasswordError);
        }
    }
}

const char* const DBMSrvProc_ServerProcess::m_LocalHostText = "localhost";
