/*!
  \file    DBMSrvCmdScd_SchedulerState.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   get scheduler state

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

*/

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"

DBMSrvCmdScd_SchedulerState::DBMSrvCmdScd_SchedulerState()
: DBMSrvCmdScd_SchedulerCommand( DBMSrv_Command::KeySchedulerState)
{
    m_longHelp=
        "@command scheduler_state This command displays the scheduler's state."
        "@preconditions You have the DBM operator authorizations SchedulerMgm or Scheduling."
        "@syntax scheduler_state"
        "@reply OK<NL>[ON|OFF]"
        "@replyvalue ON Scheduler is running and scheduled jobs are executed."
        "@replyvalue OFF Scheduler is not running.";
}

tcn00_Error DBMSrvCmdScd_SchedulerState::runLockProtected(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrvCmdScd_SchedulerReply& theSchedReply) {
    theSchedReply.startWithOK();

    // check if some process has entered its pid in shared memory
    DBMSrvMsg_Error errOut;
    RTE_OSPid schedPid(0);
    int stateIndex(0);
    if( getSchedulerPid(schedPid, errOut) ) {
        stateIndex = schedPid==(RTE_OSPid)0?1:2;

        if( stateIndex == 2 ) {
            // nonzero pid
            RTESys_ProcessState procState(schedPid);
            switch( procState.State() ) {
                case RTESys_ProcessState::Unknown:
                    stateIndex = 0;
                    break;
                case RTESys_ProcessState::Died:
                    stateIndex = 3;
                    break;
                default:
                    // it's alive
                    break;
            }
        }
        theSchedReply.appendLine(m_SchedulerStateText[stateIndex]);
        return OK_CN00;
    }
    else
        return theSchedReply.startWithMessageList(errOut);
}

bool DBMSrvCmdScd_SchedulerState::isSchedulerUp(DBMSrvMsg_Error& errOut) const {
    
    RTE_OSPid pid;
    errOut.ClearMessageList();
    
    if( !getSchedulerPid(pid, errOut) )
        return false;
    RTESys_ProcessState procState(pid);
    return procState.State() == RTESys_ProcessState::Alive;
}

bool DBMSrvCmdScd_SchedulerState::getSchedulerPid(RTE_OSPid& aPid, DBMSrvMsg_Error& errOut) const {
    
    errOut.ClearMessageList();
    DBMSrvShM_LineAccessorScheduler laSch(errOut);
    if( !errOut.IsEmpty() )
        return false;
    
    aPid = (RTE_OSPid)0;
    return laSch.getSchedulerPid(aPid, errOut);
}

const char* const DBMSrvCmdScd_SchedulerState::m_SchedulerStateText[4] = {
    "UNKNOWN",
    "OFF",
    "ON",
    "CRASHED"
};
