/*!
  \file    DBMSrvFrm_Server.cpp
  \author  MarcW
  \ingroup DBMServer framework
  \brief   the representation of the real world's server

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

#include <stdlib.h>

#include "hcn20.h"
#include "hcn31.h"
#include "hcn32.h"
#include "hcn40.h"
#include "hcn50.h"

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_HistorySession.hpp"
#include "DBM/Srv/Framework/DBMSrvFrm_Server.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp"

#if !defined(WIN32) && !defined(DBMCLI)
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "RunTime/RTE_UNIXAuthenticate.hpp"
#endif

DBMSrvFrm_Server DBMSrvFrm_Server::m_Instance;

DBMSrvFrm_Server::DBMSrvFrm_Server()
    : m_Started(false),
      m_LoggedConnect(false) {

    // nothing should happen here except simple initialisations
    // if there is something, that needs a DBMSrvMsg_Error, use m_CreationError
    
    // init random numer generation (required for shared memory lock resolution)
    srand((unsigned int)RTESys_ProcessID());
}

bool DBMSrvFrm_Server::start(bool dbmcliMode) {

    if( m_Started )
        return m_CtrlservopenError.IsEmpty();
    else {
        m_Started = true;

#if !defined(WIN32)
        if (!dbmcliMode) {
            /*
                in "clean" installations, this call does nothing. But through copying and executable
                files from other installations into another one, the file access permissions of the
                DBMServer executable may be destroyed (especially the 's'-bit).
                If a DBMServer with destroyed file access permissions creates files, the files might
                become inaccessible for other processes that run with a "correct" user.
                This call tries to reset them to correct values if they are incorrect. If this fails,
                an error is reported.
                This call should be somewhere in RTE code...
            */
            Msg_List setOwnerError;
            if( !RTE_SetSapdbOwnerAndGroupId(setOwnerError) ) {
                m_CtrlservopenError.Overrule(DBMSrvMsg_RTEError(setOwnerError));
                return false;
            }
        } // end if (!dbmcliMode)
#endif

        RTESys_FillProtocolTimeStamp(m_CtrlservopenTime);

        // initialize exception handler
        if (!dbmcliMode) {
            m_ExceptionHandler.initialize(m_CtrlservopenError);
        } // end if

        if( !m_CtrlservopenError.IsEmpty() )
            return false;

        // now that exception handling is in place:
        // do all initialisation work here (errors can occur!)
        // ...

        return true;
    }
}

bool DBMSrvFrm_Server::stop( DBMSrvMsg_Error& errOut ) {

    if( m_Started ) {
        // do all cleanup work here (errors can occur!)

        m_Started = false;
        cn00DBMServerData::vcontrol()->active = false;
        
        // log and return the start error ctrlservopenError
        if( !m_CtrlservopenError.IsEmpty() ) {
            DBMLog_Logger::getInstance().addMessageList(m_CtrlservopenError);
            errOut.Overrule(m_CtrlservopenError);
        }

        // log disconnect
        logDisconnect();

        cn00DBMServerData::vcontrol()->stop(errOut); // this does a lot, errors can occur!

        // detach from shared memory
        DBMSrvMsg_Error closeError;
        if (!DBMSrvShM_ShmAdmin::TheAdmin().close(closeError)) {
            // log close error of shared memory, this will be a "panic log" because of the 
            // DBMLog_SynchronizedLogFile does not work without shared memory.
            DBMLog_Logger::getInstance().addMessageList(closeError);
            errOut.Overrule(closeError);
        }

        // clean up logger
        // destruction of the Logger object and its members does not need shared memory
        // so we can delete it after closing the shared memory
        DBMLog_Logger::getInstance().freeResources();
    }
    else {
        // already stopped, do nothing
    }

    return errOut.IsEmpty();
}

void DBMSrvFrm_Server::handleCommunicationError(tsp01_CommErr errorCode) {
        
    logConnect();

    char buf[128];
    sprintf(buf, "%d", (int)(errorCode));
    DBMLog_Logger::getInstance().addMessageList(
        DBMSrvMsg_Error(SDBMSG_DBMSRV_CONNLOST, Msg_Arg("DBMSRV_COMP", buf)));
}

void DBMSrvFrm_Server::logConnect() {
    if( !m_LoggedConnect && cn00DBMServerData::vcontrol() != NULL ) {
        DBMLog_Logger::getInstance().addMessageList(Msg_List(Msg_List::Info,SDBMSG_DBMSRV_COMM_CLIENT_CONNECT,
            Msg_Arg(SDBMSGTAG_DBMSRV_COMM_CLIENT_CONNECT__TIME, m_CtrlservopenTime),
            Msg_Arg(SDBMSGTAG_DBMSRV_COMM_CLIENT_CONNECT__PID,cn00DBMServerData::vcontrol()->m_ClientPIDonClientMachine),
            Msg_Arg(SDBMSGTAG_DBMSRV_COMM_CLIENT_CONNECT__HOSTNAME,cn00DBMServerData::vcontrol()->m_ClientMachineID.Empty()?"(no client)":cn00DBMServerData::vcontrol()->m_ClientMachineID.CharPtr())));
        m_LoggedConnect = true;
    }
}

void DBMSrvFrm_Server::logDisconnect() {
    if( m_LoggedConnect || !m_CtrlservopenError.IsEmpty() ) {
        DBMLog_Logger::getInstance().addMessageList(Msg_List(Msg_List::Info,  SDBMSG_DBMSRV_COMM_CLIENT_DISCONNECT,
        Msg_Arg(SDBMSGTAG_DBMSRV_COMM_CLIENT_DISCONNECT__PID,cn00DBMServerData::vcontrol()->m_ClientPIDonClientMachine),
        Msg_Arg(SDBMSGTAG_DBMSRV_COMM_CLIENT_DISCONNECT__HOSTNAME,cn00DBMServerData::vcontrol()->m_ClientMachineID.Empty()?"(no client)":cn00DBMServerData::vcontrol()->m_ClientMachineID.CharPtr())));
    }
}

VControlDataT * cn00DBMServerData::m_vcontrol = NULL;

VControlDataT::~VControlDataT() {
    if (pCurrentUser != NULL)
        delete pCurrentUser;
}

void VControlDataT::start() {
    dbname.rawAssign("");
    dbroot.rawAssign("");
    szNextCommand.rawAssign("");
    szLastParameter.rawAssign("");
}


bool VControlDataT::stop( DBMSrvMsg_Error& errOut ) {
    if( m_Stopped )
        return true;
    
    m_Stopped = true;
    
    DBMSrvMsg_Error releaseAllError;
    DBMSrvKnl_GlobalSessions::ReleaseAll(releaseAllError);
    if( !releaseAllError.IsEmpty() ) {
        errOut.Overrule(releaseAllError);
        return false;
    }
    
    cn20FreeXParamSection(this);
    cn31FreeBackupSession(this);
    cn31FreeBackupStageSession(this);
    DBMSrvBHist_HistorySession::freeGlobalSession(this);
    cn32FreeEBIDList(this);
    cn40FreeCommandObj(this);
    cn20FreeParamHistory(this);
    
    return true;
}
