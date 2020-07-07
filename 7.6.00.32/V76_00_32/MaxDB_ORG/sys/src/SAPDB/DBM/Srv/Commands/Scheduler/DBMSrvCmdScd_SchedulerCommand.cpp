/*!
  \file    DBMSrvCmdScd_SchedulerCommand.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   base class for scheduler commands, implements no command

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

#include <time.h>

#include "hcn50.h"

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ArgumentList.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp"
#include "DBM/Srv/Scheduler/DBMSrvScd_Job.hpp"
#include "DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"
#include "ToolsCommon/Tools_Arguments.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

DBMSrvCmdScd_SchedulerReply::~DBMSrvCmdScd_SchedulerReply() {
    m_TheReply.adaptLength();
}

void DBMSrvCmdScd_SchedulerReply::appendSchedulerInfo(
    DBMSrvShM_LineAccessorScheduler::InfoType infTy,
    const char* pVarPart ) {

    switch( infTy ) {
        case DBMSrvShM_LineAccessorScheduler::IT_ErrorWrongUser:
            // error message
            startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOTJOBOWNER));
            break;
        case DBMSrvShM_LineAccessorScheduler::IT_ErrorJobNotExist:
            // error message
            startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_JOBNOTEXIST));
            break;
        case DBMSrvShM_LineAccessorScheduler::IT_ErrorPredecessorNotExist:
            // error message
            startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PREDECESSORNOTEXIST));
            break;
        case DBMSrvShM_LineAccessorScheduler::IT_ErrorJobHasSuccessors:
            // error message
            startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_JOBHASSUCCESSORS));
            appendJobList(pVarPart);
            break;
        case DBMSrvShM_LineAccessorScheduler::IT_JobID:
            // job ID: just display it
            appendLine(pVarPart);
            break;
        case DBMSrvShM_LineAccessorScheduler::IT_SerializedJobList:
            // list of jobs: display them
            appendJobList(pVarPart);
            break;
        case DBMSrvShM_LineAccessorScheduler::IT_None:
            // well, append nothing
            break;
        default: {
            // don't know what to do...
            char answerLine[1024];
            sprintf(answerLine, "%d", (int) infTy);
            Msg_List msgList;
            msgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SCHEDULERUNEXPECTED));
            msgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_UNKNOWNINFOTYPE,
                                             Msg_Arg(SDBMSGTAG_DBMSRV_UNKNOWNINFOTYPE__DBMSRV_COMP1,answerLine),
                                             Msg_Arg(SDBMSGTAG_DBMSRV_UNKNOWNINFOTYPE__DBMSRV_COMP2,pVarPart)));
            startWithMessageList(msgList);
            break;
        }
    }
}

void DBMSrvCmdScd_SchedulerReply::appendJobList(
        const char* aVarPart) {
    DBMSrvScd_JobList jobList(aVarPart);
    char line[1024];
    const char* const formatFormat("%%-%ds%%s");
    char format[128];
    sprintf(format, formatFormat, strlen(m_TextCmd)+1);

    cn50DBMUser* pCurrUser(cn00DBMServerData::vcontrol()->pCurrentUser);
    bool userIsSCDAdmin(pCurrUser->isServerRight(DBMRightSCDAdmin_CN50));


    DBMSrvScd_JobList::iterator jobIt = NULL;
    bool needSpacer(false);
    for( jobIt = jobList.begin(); jobIt != jobList.end(); jobIt++ ) {

        // check whther current user may see this job
        if( !userIsSCDAdmin &&
            strcmp((*jobIt)->getUserID(),(const char* )pCurrUser->getUserName())!=0 ) 
            continue;

        if( needSpacer ) {
            appendLine();
        }
        else
            needSpacer = true;

        // id
        sprintf(line, m_TextID, (*jobIt)->getID() );
        appendLine(line);

        // owner
        sprintf(line, format, m_TextOwner, (*jobIt)->getUserID());
        appendLine(line);

        // status
        sprintf(line, format, m_TextStatus, (*jobIt)->isActive()?m_TextStatusActive:m_TextStatusInactive);
        appendLine(line);

        // runs
        char valBuf[256];
        if( (*jobIt)->hasPlan() ) {
            char* time((*jobIt)->getPlan()->getTimeString());
            sprintf(valBuf, m_TextAt, time, (*jobIt)->runOnce()?m_TextOnce:"");
            delete[] time;
        }
        else 
            sprintf(valBuf, m_TextAfter, (*jobIt)->getPredecessor(), (*jobIt)->runOnce()?m_TextOnce:"");
        sprintf(line, format, m_TextRuns, valBuf);
        appendLine(line);

        // last execution
        char* time((*jobIt)->getLastExecutionTimeString());
        if( 0 == strncmp("1900", time, strlen("1900")) ) {
            // it never ran so far...
            strcpy(valBuf, m_TextLastRunNever);
            sprintf(line, format, m_TextLastRun, valBuf);
            appendLine(line);
        }
        else {
            // it did run already
            sprintf(valBuf, m_TextLastRunAt, time );
            sprintf(line, format, m_TextLastRun, valBuf);
            appendLine(line);
            sprintf(valBuf, m_TextLastRunPid, (int)((*jobIt)->getLastServerPid()));
            sprintf(line, format, "", valBuf);
            appendLine(line);
            if( (*jobIt)->getLastReturnCode() == DBMSrvScd_Job::m_ReturnCodeUnknown ) {
                sprintf(valBuf, m_TextLastRunUnknownReturnCode);
                sprintf(line, format, "", valBuf);
                appendLine(line);
            }
            else {
                sprintf(valBuf,
                        m_TextLastRunReturnCode,
                        (*jobIt)->getLastReturnCode()==0?ANSWER_OK_CN00:ANSWER_ERR_CN00,
                        (*jobIt)->getLastReturnCode());
                sprintf(line, format, "", valBuf);
                appendLine(line);
            }
        }
        delete[] time;

        // command
        char* cmd = (*jobIt)->getCommand();
        sprintf(line, format, m_TextCmd, cmd);
        delete[] cmd;
        appendLine(line);
    }
    DBMSrvScd_JobList::deleteJobsInList(jobList);
}

const char* const DBMSrvCmdScd_SchedulerReply::m_TextID = "ID %d";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextOwner = "owner:";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextOnce = "(once)";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextStatus = "status:";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextStatusActive = "active";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextStatusInactive = "inactive";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextRuns = "runs:";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextAfter = "after %d %s";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextAt = "at %s %s";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextLastRun = "last run:";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextLastRunAt = "%s";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextLastRunPid = "DBM Server PID %d";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextLastRunReturnCode = "return code %s (%d)";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextLastRunUnknownReturnCode = "running";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextLastRunNever = "never";
const char* const DBMSrvCmdScd_SchedulerReply::m_TextCmd = "dbm_command:";

const char* const DBMSrvCmdScd_SchedulerReply::m_ListExplanation =
    "@command scheduler_list_jobs This command lists scheduled jobs. Users with the DBM "
    "operator authorization SchedulerMgm see all scheduled jobs. Users with the DBM "
    "operator authorization Scheduling see only the jobs that they own. This command "
    "can be executed no matter whether the scheduler is running or not."
    "@preconditions You have the DBM operator authorizations SchedulerMgm or Scheduling."
    "@syntax scheduler_list_jobs"
    "@reply OK<NL>"
        "ID <job_id_1><NL>"
        "owner:  <job_owner_1><NL>"
        "status: <job_status_1><NL>"
        "runs   [at: <job_time_1>|after: <job_id_j>] [(once)]<NL>"
        "dbm_command: <job_cmd_1><NL>"
        "<NL>"
        "...<NL>"
        "ID <job_id_n><NL>"
        "owner:  <job_owner_n><NL>"
        "status: <job_status_n><NL>"
        "runs   [at: <job_time_n>|after: <job_id_k>] [(once)]<NL>"
        "dbm_command: <job_cmd_n><NL>"
    "@replyvalue <job_id_i> id of job i"
    "@replyvalue <job_owner_i> owner of job i (DBM user)"
    "@replyvalue <job_status_i> status of job i (can be \"active\" or \"inactive\")"
    "@replyvalue <job_time_i> execution time of job i"
    "@replyvalue <job_cmd_i> command of job i"
    "@replyvalue <job_id_j> after finishing job j, this job is started";

DBMSrvCmdScd_SchedulerCommand::DBMSrvCmdScd_SchedulerCommand(
        CommandKey aCommandKey,
        const char * longHelp,
        bool isHidden)
: DBMSrv_Command( aCommandKey, false, longHelp, isHidden) {
    if( cn00DBMServerData::vcontrol() != NULL && 
        cn00DBMServerData::vcontrol()->pCurrentUser != NULL ) {
        m_User = cn00DBMServerData::vcontrol()->pCurrentUser->getUserName();
    }
    else {
        m_User[0] = '\0';
    }
}

tcn00_Error DBMSrvCmdScd_SchedulerCommand::run(
                    VControlDataT* vcontrol,
                    CommandT* command,
                    DBMSrv_Reply& theReply) {
    tcn00_Error rc(OK_CN00);
    theReply.startWithOK();

    // get resource lock
    DBMSrvMsg_Error lockError;
    DBMSrvShM_LineAccessorResourceLock schedCom(
            DBMSrvShM_InternalResource::SchedulerCommunication, lockError);
    if( !lockError.IsEmpty() )
        return theReply.startWithMessageList(lockError);

    DBMSrvShM_LineAccessorResourceLock::LockState lockState;
    lockState = schedCom.lock(lockError);
    
    if( DBMSrvShM_LineAccessorResourceLock::LockStateLocked != lockState )
        return theReply.startWithMessageList(lockError);

    // resource is locked here

    // let derived class do the work
    DBMSrvCmdScd_SchedulerReply theSchedReply(theReply);
    rc = runLockProtected(vcontrol, command, theSchedReply);

    lockState = schedCom.unlock(lockError);
    if( DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked != lockState )
        rc = theReply.startWithMessageList(lockError);

    return rc;
}

tcn00_Error DBMSrvCmdScd_SchedulerCommand::convertToTimeStruct(
    const Tools_DynamicUTF8String& aTimeString,
    struct tm& aTimeStruct, 
    DBMSrvCmdScd_SchedulerReply& theSchedReply) const {

    theSchedReply.startWithOK();
    bool timeIllegal(false);
    resetTimeStruct(aTimeStruct);


    // first check if we have an offset (in seconds, entered like this: +10)
    if( aTimeString.Length() >= 2 &&
        aTimeString[0] == '+' &&
        isNumeric(aTimeString.SubStr(1)) ) {

        int offset(abs(atoi(aTimeString.SubStr(1).CharPtr())));
        time_t now;
        time(&now);
        struct tm* nowStruct = localtime(&now);

        // max offset is one second less than a 24 hours
        offset = offset % (60*60*24);

        int hoursOffset(offset / (60*60)); //<=23
        int minutesOffset((offset - hoursOffset*60*60) / 60); //<=59
        int secondsOffset(offset - hoursOffset*60*60 - minutesOffset*60); //<=59

        // add offset to now
        if( nowStruct->tm_sec + secondsOffset > 59)
            ++minutesOffset;
        nowStruct->tm_sec  = (nowStruct->tm_sec + secondsOffset) % 60;

        if( nowStruct->tm_min + minutesOffset > 59)
            ++hoursOffset;
        nowStruct->tm_min = (nowStruct->tm_min + minutesOffset) % 60;
        nowStruct->tm_hour = (nowStruct->tm_hour + hoursOffset) % 24;

        // copy result to return structure
        aTimeStruct.tm_hour = nowStruct->tm_hour;
        aTimeStruct.tm_min = nowStruct->tm_min;
        aTimeStruct.tm_sec = nowStruct->tm_sec;
        aTimeStruct.tm_isdst = -1;
        return OK_CN00;
    }


    timeIllegal = aTimeString.Length() != 8;
    if( !timeIllegal ) {
        timeIllegal = aTimeString[2] != ':' ||
                      aTimeString[5] != ':';
    }

    // check hours format
    if( !timeIllegal )
        timeIllegal = !isNumeric(aTimeString.SubStr(0,2));
    if( !timeIllegal ) {
        aTimeStruct.tm_hour = atoi(aTimeString.SubStr(0,2).CharPtr());
        timeIllegal = aTimeStruct.tm_hour < 0 || aTimeStruct.tm_hour > 23;
    }

    // check minutes format
    if( !timeIllegal )
        timeIllegal = !isNumeric(aTimeString.SubStr(3,2));
    if( !timeIllegal ) {
        aTimeStruct.tm_min = atoi(aTimeString.SubStr(3,2).CharPtr());
        timeIllegal = aTimeStruct.tm_min < 0 || aTimeStruct.tm_min > 59;
    }

    // check seconds format
    if( !timeIllegal )
        timeIllegal = !isNumeric(aTimeString.SubStr(6,2));
    if( !timeIllegal ) {
        aTimeStruct.tm_sec = atoi(aTimeString.SubStr(6,2).CharPtr());
        timeIllegal = aTimeStruct.tm_sec < 0 || aTimeStruct.tm_sec > 59;
    }

    if( timeIllegal ) {
        Tools_DynamicUTF8String copy(aTimeString);
        return theSchedReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ILLEGALTIMEFMT,
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_ILLEGALTIMEFMT__DBMSRV_COMP,copy.CharPtr())));
    }
    else
        return OK_CN00;
}

tcn00_Error DBMSrvCmdScd_SchedulerCommand::startScheduler(DBMSrvCmdScd_SchedulerReply& theSchedReply) {

    DBMSrvMsg_Error errOut;
    RTE_OSPid schedulerPid(0);

    DBMSrvShM_LineAccessorScheduler laSch(errOut);
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);

    bool schedulerUp = laSch.getSchedulerPid(schedulerPid, errOut) && schedulerPid!=(RTE_OSPid)0;
    
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);
    
    if( schedulerUp ) {
        RTESys_ProcessState procState(schedulerPid);
        schedulerUp = (procState.State() == RTESys_ProcessState::Alive);        
    }

    if( schedulerUp )
        // its running
        return OK_CN00;

    // now start scheduler

    // set my own pid as scheduler pid because we need a living pid to avoid deletion of line by the starting scheduler
    if( !laSch.setSchedulerPid(errOut) )
        return theSchedReply.startWithMessageList(errOut);

    // build the command line

    DBMSrvProc_ArgumentList argList;
    argList.AddArgument("-d");
    argList.AddArgument(cn00DBMServerData::vcontrol()->dbname.asCharp());
    argList.AddArgument("-f");
    argList.AddArgument(getRundirectory());

    

    DBMSrvProc_ServerProcess schedulerProc(
        DBMSrvProc_ServerProcessCommandLineConverter::KeySchedulerProgram,
        argList);

    if( !schedulerProc.started() ) {
        Msg_List msgList;
        msgList.ClearMessageList();
        schedulerProc.getMessagesInto(msgList);
        msgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOSCHEDULERSTART));
        return theSchedReply.startWithMessageList(msgList);
    }
    else {
        // wait for the scheduler to enter it's pid
        int count(300);
        while(--count>0) {
            if( !laSch.getSchedulerPid(schedulerPid, errOut) )
                return theSchedReply.startWithMessageList(errOut);

            RTESys_ProcessState procState(schedulerPid);
            if( (procState.State() == RTESys_ProcessState::Alive) )
                return OK_CN00;
            RTE_ISystem::DoSleep(100);
        }
        // we get here if the process coould be started but has not
        // entered its pid after 30s... should not happen
        return theSchedReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOSCHEDULERSTART));
    }
}

tcn00_Error DBMSrvCmdScd_SchedulerCommand::stopScheduler(
            DBMSrvCmdScd_SchedulerReply& theSchedReply) {

    RTE_OSPid schedulerPid(0);
    RTESys_ProcessState procState;
    DBMSrvMsg_Error errOut;
    
    DBMSrvShM_LineAccessorScheduler laSch(errOut);
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);


    bool schedulerUp = laSch.getSchedulerPid(schedulerPid, errOut);
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);

    if( schedulerUp ) {
        procState.Open(schedulerPid);
        RTESys_ProcessState::ProcState pState(procState.State());
        schedulerUp = (pState == RTESys_ProcessState::Alive);
    }

    if( schedulerUp ) {
        // its running, we must stop it

        // do this to avoid removal of ShM line in the time interval between termination of scheduler and
        // the removeLayout call a few lines below
        if( !laSch.setSchedulerPid(errOut) )
            return theSchedReply.startWithMessageList(errOut);

        if( !laSch.doCancel(errOut) )
            return theSchedReply.startWithMessageList(errOut);
        
        if( !laSch.setRecipientScheduler(errOut) )
            return theSchedReply.startWithMessageList(errOut);

        if( !laSch.clientWait(errOut) )
            return theSchedReply.startWithMessageList(errOut);

        // wait for 30s max, if still alive after that, it's an error
        for( int waitCount(300); waitCount>0; --waitCount)
            if( procState.State() == RTESys_ProcessState::Alive )
                RTE_ISystem::DoSleep(100);
            else
                break;
        
        if ( procState.State() == RTESys_ProcessState::Alive ) {
                // error could not stop it
                char sPid[32];
                sprintf(sPid, "%d", (int) schedulerPid);
                return theSchedReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOSCHEDULERSTOP, Msg_Arg(SDBMSGTAG_DBMSRV_NOSCHEDULERSTOP__DBMSRV_COMP,sPid)));
        }
        
        // here we remove the line
        if( !laSch.removeLayout(errOut) )
            return theSchedReply.startWithMessageList(errOut);
    }
    return OK_CN00;
}

bool DBMSrvCmdScd_SchedulerCommand::isNumeric( const Tools_DynamicUTF8String& aString) const {
    return aString.FindFirstNotOf("0123456789") == Tools_DynamicUTF8String::NPos;
}

const char* const DBMSrvCmdScd_SchedulerCommand::m_SchedulerNotRunning =
    "Note: The scheduler is OFF! The job will not be executed.";
