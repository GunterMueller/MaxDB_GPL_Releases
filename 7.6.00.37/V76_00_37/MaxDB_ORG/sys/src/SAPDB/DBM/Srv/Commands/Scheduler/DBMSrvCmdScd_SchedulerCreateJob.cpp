/*!
  \file    DBMSrvCmdScd_SchedulerCreateJob.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   schedule a job

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

#include "hcn50.h"

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCreateJob.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp"
#include "DBM/Srv/Scheduler/DBMSrvScd_Job.hpp"
#include "DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"
#include "ToolsCommon/Tools_Arguments.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

DBMSrvCmdScd_SchedulerCreateJob::DBMSrvCmdScd_SchedulerCreateJob()
: DBMSrvCmdScd_SchedulerCommand( DBMSrv_Command::KeySchedulerCreateJob)
{
    m_longHelp=
        "@command scheduler_create_job This command schedules dbmcli commands. A scheduled command "
            "is called a job. A job runs with the credentials of the DBM user that created it. "
            "The DBM user, that created a job is called its owner. A job can run either "
            "at a specified time or after finishing of a predecessing job. This command can be "
            "executed no matter whether the scheduler is running or not. To have jobs executed, "
            "the scheduler must be running (use command scheduler_start to start it)."
        "@preconditions You have the DBM operator authorizations SchedulerMgm or Scheduling."
        "@syntax scheduler_create_job <time>|<pre_job_id> <command> [-o[nce]]"
        "@param <time> time at which <command> is to be executed. Format is HH:mm:ss."
        "@param <pre_job_id> After job with this id finished, this job is executed."
        "@param <command> command to be executed, including parameters. If it contains whitespace, "
            "enclose in double quotes."
        "@param -o[nce] This job is executed only once. It will be deleted after the command "
            "finished (and hence never be executed again)."
        "@reply OK<NL>"
            "<new_job_id>"
        "@replyvalue <new_job_id> ID of the new job";
}

tcn00_Error DBMSrvCmdScd_SchedulerCreateJob::runLockProtected(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrvCmdScd_SchedulerReply& theSchedReply) {

    theSchedReply.startWithOK();

    int updateID(-1);

    // do we have a predecessor or a time?
    int predecessorID(-1);
    struct tm inputTime;

    if( !isNumeric(command->oArguments.getValue(1))) {
        // it's no integer, check if it's a time
        tcn00_Error timeOk =
            convertToTimeStruct(command->oArguments.getValue(1),
                                inputTime,
                                theSchedReply);
        if( timeOk != OK_CN00 )
            return timeOk;
    } else
        predecessorID = atoi(command->oArguments.getValue(1).CharPtr());

    // check whether a run once job shall be created
    bool runOnce = command->oArguments.getCount() == 3 && 
                       (command->oArguments.getName(3) == m_TextOnce ||
                        command->oArguments.getName(3) == m_TextO);

    // now create a job object
    DBMSrvScd_Job* pJob(NULL);
    if( predecessorID > -1 )
        pJob = new DBMSrvScd_Job(
                        updateID,
                        getUser(),
                        true,
                        runOnce,
                        command->oArguments.getValue(2).CharPtr(),
                        predecessorID);
    else
        pJob = new DBMSrvScd_Job(
                        updateID,
                        getUser(),
                        true,
                        runOnce,
                        command->oArguments.getValue(2).CharPtr(),
                        new DBMSrvScd_Plan(inputTime));


    // now send object to scheduler
    DBMSrvMsg_Error errOut;
    
    DBMSrvShM_LineAccessorScheduler laSch(errOut);
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);

    if( laSch.setInfoType(DBMSrvShM_LineAccessorScheduler::IT_SerializedJobForCreation, errOut) ) {
        char* serJob = pJob->serialize();
        delete pJob;
        bool success(laSch.setVariablePart(serJob, errOut));
        delete[] serJob;
        if( !success )
            return theSchedReply.startWithMessageList(errOut);
    }
    else
        return theSchedReply.startWithMessageList(errOut);

    if( !(laSch.setUserName(getUser(), errOut) && laSch.setRecipientScheduler(errOut)) )
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

const char* const DBMSrvCmdScd_SchedulerCreateJob::m_TextOnce = "once";
const char* const DBMSrvCmdScd_SchedulerCreateJob::m_TextO = "o";
