/*!
  \file    DBMSrvCmdScd_SchedulerActivateJob.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   activate a job

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

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerActivateJob.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

DBMSrvCmdScd_SchedulerActivateJob::DBMSrvCmdScd_SchedulerActivateJob()
: DBMSrvCmdScd_SchedulerCommand( DBMSrv_Command::KeySchedulerActivateJob)
{
    m_longHelp=
        "@command scheduler_activate_job This command activates a scheduled job. This command "
        "can be executed no matter whether the scheduler is running or not."
        "@preconditions You have the DBM operator authorization SchedulerMgm or you are "
            "the job's owner and have the DBM operator authorization Scheduling."
        "@syntax scheduler_activate_job <job_id>"
        "@param <job_id> Id of job to be activated."
        "@reply OK";
}

tcn00_Error DBMSrvCmdScd_SchedulerActivateJob::runLockProtected(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrvCmdScd_SchedulerReply& theSchedReply) {

    theSchedReply.startWithOK();
    DBMSrvMsg_Error errOut;

    // check whether we got a numeric parameter
    if( !isNumeric(command->oArguments.getValue(1)) ) {
        return theSchedReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ILLJOBNUMBER, Msg_Arg(SDBMSGTAG_DBMSRV_ILLJOBNUMBER__DBMSRV_COMP,command->oArguments.getValue(1).CharPtr())));
    }

    // now send request to scheduler
    DBMSrvShM_LineAccessorScheduler laSch(errOut);
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);

    if( !(laSch.setInfoType(DBMSrvShM_LineAccessorScheduler::IT_ActivateJob, errOut) &&
          laSch.setVariablePart(command->oArguments.getValue(1).CharPtr(), errOut) &&
          laSch.setUserName(getUser(), errOut) &&
          laSch.setRecipientScheduler(errOut)) )
        return theSchedReply.startWithMessageList(errOut);

    // start the scheduler if it's not running yet
    DBMSrvCmdScd_SchedulerState cmdGetState;
    bool schedulerUpInitialy(cmdGetState.isSchedulerUp(errOut));
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);
    
    if( !schedulerUpInitialy ) {
        tcn00_Error theRC = startScheduler(theSchedReply);
        if( theRC != OK_CN00 )
            return theRC;
    }

    // wait for answer from scheduler
    if( !laSch.clientWait(errOut) )
        return theSchedReply.startWithMessageList(errOut);

    // now there should be an answer
    DBMSrvShM_LineAccessorScheduler::InfoType infTy;
    char* pVarPart(NULL);
    if( !(laSch.getInfoType(infTy, errOut) && laSch.getVariablePart(pVarPart, errOut)) ) {
        if( pVarPart != NULL )
            delete pVarPart;
        return theSchedReply.startWithMessageList(errOut);
    }

    theSchedReply.appendSchedulerInfo(infTy, pVarPart);

    if( pVarPart != NULL )
        delete pVarPart;

    // we processed what came back, now clear shm line
    if( !laSch.clearContent(errOut) )
        return theSchedReply.startWithMessageList(errOut);

    // stop scheduler if it was not up initially
    if( !schedulerUpInitialy ) {
        tcn00_Error theRC = stopScheduler(theSchedReply);
        if( theRC != OK_CN00 )
            return theRC;
        theSchedReply.appendLine(m_SchedulerNotRunning);
    }

    // return the information we collected in the reply
    return theSchedReply.ReturnCode();
}
