/*!
  \file    DBMSrvScd_Scheduler.cpp
  \author  MarcW
  \brief   scheduler implementation

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
#include "hni35.h"

#include "DBM/Srv/Scheduler/DBMSrvScd_Scheduler.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMScd.h"

const int DBMSrvScd_Scheduler::m_RunloopSleepTime(1000); //milliseconds

DBMSrvScd_Scheduler::DBMSrvScd_Scheduler(const char* aDbName, const char* folder)
        : m_JobList(NULL),
          m_dbName(new char[strlen(aDbName)+1]),
          m_Persistor(new DBMSrvScd_Persistor(folder)),
          m_CmdLogger(DBMLog_Logger::getInstance()),
          m_LaSch(NULL),
          m_IsValid(false),
          m_MustTerminate(false) {

    Msg_List errConstructor;
    DBMSrvMsg_Error laSchError;

    m_LaSch = new DBMSrvShM_LineAccessorScheduler(laSchError);
    if( m_LaSch != NULL ) {
        if( m_LaSch->setSchedulerPid(laSchError) ) {
            // that is me...
            
            // log start
            m_CmdLogger.addMessageList(
                Msg_List(Msg_List::Info, SDBMSG_DBMSCD_ACTION_START));
            
            SAPDB_strcpy(m_dbName, aDbName);
            m_JobList = m_Persistor->loadJobs();
            prepareJobs();
            m_IsValid = true;
        }
        else {
            errConstructor.Overrule(laSchError);
            errConstructor.Overrule(Msg_List(Msg_List::Error, SDBMSG_DBMSCD_NO_PID_ENTERED));
        }
    }
    else
        errConstructor.Overrule(laSchError);
    
    if( !m_IsValid ) {
        errConstructor.Overrule(
            Msg_List(Msg_List::Error, SDBMSG_DBMSCD_NO_START));

        m_CmdLogger.addMessageList(errConstructor);
    }
}

void DBMSrvScd_Scheduler::prepareJobs() {

    DBMSrvScd_JobList::iterator jobIt = NULL;
    for( jobIt = m_JobList->begin(); jobIt != m_JobList->end(); ) {
        DBMSrvScd_Job* aJob = *jobIt;
        aJob->setDBName(m_dbName);
        if( aJob->hasPredecessor() ) {
            DBMSrvScd_Job* predJob = getJob(aJob->getPredecessor(), true);
            if( predJob != NULL ) {
                predJob->addSuccessor(aJob);
                jobIt = m_JobList->erase(jobIt);
            }
        }
        else
            jobIt++;
    }
}

DBMSrvScd_Job* DBMSrvScd_Scheduler::getJob(const int aJobID, const bool searchSuccessors ) {
    return getJobRecursive(m_JobList, aJobID, searchSuccessors);
}

DBMSrvScd_Job* DBMSrvScd_Scheduler::getJobRecursive(
            DBMSrvScd_JobList* aJobList,
            const int aJobID,
            const bool searchSuccessors ) {

    DBMSrvScd_JobList::iterator jobIt = NULL;
    for( jobIt = aJobList->begin(); jobIt != aJobList->end(); jobIt++ ) {
        if( (*jobIt)->getID() == aJobID )
            return *jobIt;
        else if( searchSuccessors && (*jobIt)->hasSuccessors() ) {
            DBMSrvScd_JobList* succList = (*jobIt)->getSuccessors(false);
            DBMSrvScd_Job* pJob = getJobRecursive( succList, aJobID, searchSuccessors);
            delete succList;
            if( pJob != NULL )
                return pJob;
        }
    }
    return NULL;
}

DBMSrvScd_Scheduler::~DBMSrvScd_Scheduler() {
    if( m_Persistor != NULL )
        delete m_Persistor;
    
    if( m_JobList != NULL ) {
        deleteAllJobs(m_JobList);
        delete m_JobList;
    }
    
    if( m_dbName != NULL )
        delete m_dbName;
    
    if( m_LaSch != NULL )
        delete m_LaSch;
}

bool DBMSrvScd_Scheduler::userIsOwnerOrAdmin(DBMSrvScd_Job &aJob) {
    if( userIsAdmin() )
        return true;
    else {
        char* currentUserName(NULL);
        DBMSrvMsg_Error errOut;
        if( m_LaSch->getUserName(currentUserName, errOut) ) {
            bool isOwner(strcmp(aJob.getUserID(), currentUserName)==0);
            delete currentUserName;
            return isOwner;
        }
        else {
            m_CmdLogger.addMessageList(errOut);
            return false;
        }
    }
}

bool DBMSrvScd_Scheduler::userIsAdmin() {
    
    char* currentUserName(NULL);
    DBMSrvMsg_Error errOut;

    if( m_LaSch->getUserName(currentUserName, errOut) ) {
        tcn00_UserNamec userName;
        userName.rawAssign(currentUserName);
        delete currentUserName;

        tsp00_DbNamec dbName;
        dbName.rawAssign(m_dbName);

        cn50DBMUser currentUser(dbName, userName);
        return currentUser.isServerRight(DBMRightSCDAdmin_CN50);
    }
    else {
        m_CmdLogger.addMessageList(errOut);
        return false;
    }
}

void DBMSrvScd_Scheduler::deleteAllJobs(DBMSrvScd_JobList* aJobList) {
    DBMSrvScd_JobList::iterator jobIt = NULL;
    for( jobIt = aJobList->begin(); jobIt != aJobList->end(); jobIt++ ) {
        if( (*jobIt)->hasSuccessors() ) {
            DBMSrvScd_JobList* succList = (*jobIt)->getSuccessors(false);
            deleteAllJobs(succList);
            delete succList;
        }
    }
    DBMSrvScd_JobList::deleteJobsInList(*aJobList);
}
/*
void DBMSrvScd_Scheduler::printjobs( DBMSrvScd_JobList* aJobList, int tabs ) {
    FILE* f = fopen( "c:\\mp\\mp_job.txt","a+");
    DBMSrvScd_JobList::iterator jobIt = NULL;
    for( jobIt = aJobList->begin(); jobIt != aJobList->end(); jobIt++ ) {
        for(int j=tabs; j>0; --j )
            fprintf(f, "  ");
        fprintf(f, "id: %d\n", (*jobIt)->getID());
        if( (*jobIt)->hasSuccessors() ) {
            DBMSrvScd_JobList* kids = (*jobIt)->getSuccessors(false);
            printjobs(kids, tabs+1);
            delete kids;
        }
    }
    fclose(f);
}
*/
void DBMSrvScd_Scheduler::run() {
    while( true ) {
        communicate();
        if( m_MustTerminate )
            break;
 
        //printjobs(m_JobList,0);

        DBMSrvScd_JobList::iterator jobIt = NULL;
        bool doIncrement(true);
        for( jobIt = m_JobList->begin(); jobIt != m_JobList->end(); ) {
            doIncrement = true;
            DBMSrvScd_Job* aJob = *jobIt;
            DBMSrvScd_Job::JobState theJobState = aJob->getState();
            if( theJobState == DBMSrvScd_Job::Running ) {
                aJob->touch();
            }
            else if( theJobState == DBMSrvScd_Job::Waiting ) {
                if( aJob->mustExecuteNow() ) {
                    aJob->execute();
                    // log planned execution time
                    logPlannedTime(aJob);

                    // persist the job to save it's new last execution time and serving pid
                    m_Persistor->persistJob(aJob);
                    // log process id of DBMServer that we use
                    m_CmdLogger.addMessageList(
                        Msg_List(
                            Msg_List::Info,
                            SDBMSG_DBMSCD_JOBSERVEDBYPID,
	                        Msg_Arg("SCDJOBID",(int)aJob->getID()),
                            Msg_Arg("PID",(int)aJob->getPid()),
                            Msg_Arg("PIDASHEX", (int)aJob->getPid(), SAPDB_ToStringClass::hex)));
                }
            }
            else if( theJobState == DBMSrvScd_Job::ReadyForSuccessors ) {
                // log answer
                //logJobReply(aJob);
                
                // add aJob's successors to list
                DBMSrvScd_JobList* successingJobs;
                successingJobs = aJob->getSuccessors(true);
                m_JobList->splice(m_JobList->end(), *successingJobs);
                delete successingJobs;
                
                // tell job that successors are scheduled
                aJob->setSuccessorsScheduled();

                if( aJob->runOnce() )
                    aJob->deactivate();
                
                // persist the job to save it's new returncode (and maybe inactivated status)
                m_Persistor->persistJob(aJob);

                if( aJob->hasPredecessor() ) {
                    jobIt = m_JobList->erase(jobIt);
                    doIncrement = false;
                }
            }
            if( doIncrement )
                jobIt++;
        } // this is the end of one loop through the scheduled jobs
        RTE_ISystem::DoSleep(m_RunloopSleepTime);
    }
}

void DBMSrvScd_Scheduler::communicate() {

    // check cancellation flag
    DBMSrvMsg_Error errOut;
    bool shmActionOkay(m_LaSch->isCancelled( m_MustTerminate, errOut));

    if( !shmActionOkay ) {
        // there is a communication problem, we terminate
        m_CmdLogger.addMessageList(errOut);
        m_MustTerminate = true;
        return;
    }

    if( m_MustTerminate )
        m_CmdLogger.addMessageList(
            Msg_List(Msg_List::Info, SDBMSG_DBMSCD_ACTION_TERMINATION));


    if( !m_MustTerminate ) {
        shmActionOkay = m_LaSch->isSchedulerCommand(errOut);
        // something to do for us
        if( shmActionOkay ) {
            DBMSrvShM_LineAccessorScheduler::InfoType infoType;
            shmActionOkay = m_LaSch->getInfoType(infoType, errOut);
            if( shmActionOkay ) {
                switch( infoType ) {
                    case DBMSrvShM_LineAccessorScheduler::IT_SerializedJobForCreation:
                        // the variable part contains a serialized job
                        m_CmdLogger.addMessageList(
                            Msg_List(Msg_List::Info, SDBMSG_DBMSCD_ACTION_INSERTJOB));
                        insertJob();
                        break;
                    case DBMSrvShM_LineAccessorScheduler::IT_RequestJobList:
                        // client wants a list of all jobs
                        m_CmdLogger.addMessageList(
                            Msg_List(Msg_List::Info, SDBMSG_DBMSCD_ACTION_RETURNJOBLIST));
                        returnJobList();
                        break;
                    case DBMSrvShM_LineAccessorScheduler::IT_ActivateJob:
                        // client wants to activate a job
                        m_CmdLogger.addMessageList(
                            Msg_List(Msg_List::Info, SDBMSG_DBMSCD_ACTION_ACTIVATEJOB));
                        activateJob();
                        break;
                    case DBMSrvShM_LineAccessorScheduler::IT_DeactivateJob:
                        // client wants to deactivate a job
                        m_CmdLogger.addMessageList(
                            Msg_List(Msg_List::Info, SDBMSG_DBMSCD_ACTION_DEACTIVATEJOB));
                        deactivateJob();
                        break;
                    case DBMSrvShM_LineAccessorScheduler::IT_DeleteJob:
                        // client wants to delete a job
                        m_CmdLogger.addMessageList(
                            Msg_List(Msg_List::Info, SDBMSG_DBMSCD_ACTION_DELETEJOB));
                        deleteJob();
                        break;
                    default:
                        // should never be seen in log
                        m_CmdLogger.addMessageList(
                            Msg_List(Msg_List::Error, SDBMSG_DBMSCD_ERRUNKNOWNCOMMAND));
                        break;
                }
            }
        }
        if( !errOut.IsEmpty() ) {
            m_CmdLogger.addMessageList(errOut);
            m_MustTerminate = true;
        }
    }

    if( m_MustTerminate ) {
        // clean up
        errOut.ClearMessageList();
        if( !(m_LaSch->removeSchedulerPid(errOut) && m_LaSch->unCancel(errOut) && m_LaSch->clearContent(errOut)) )
            m_CmdLogger.addMessageList(errOut);
    }

    // answer was prepared now, return shm control to client
    errOut.ClearMessageList();
    if( !m_LaSch->setRecipientClient(errOut) )
        m_CmdLogger.addMessageList(errOut);
}

void DBMSrvScd_Scheduler::changeJobStateTo(const bool newStateIsActive) {

    char* tmp(NULL);
    DBMSrvMsg_Error errOut;

    if( !m_LaSch->getVariablePart(tmp, errOut) ) {
        m_CmdLogger.addMessageList(errOut);
        return;
    }

    int jobID(atoi(tmp));
    delete tmp;

    DBMSrvScd_Job* theJob = getJob(jobID, true);
    if( theJob == NULL ) {
        answerJobNoExistError();
    }
    else {
        if( userIsOwnerOrAdmin(*theJob) ) {
            if( newStateIsActive )
                theJob->activate();
            else
                theJob->deactivate();
            // persist job's new state
            m_Persistor->persistJob(theJob);
            answerOkay();
        }
        else {
            answerUserNotOwnerError();
        }
    }
}

void DBMSrvScd_Scheduler::deleteJob() {

    char* tmp(NULL);
    DBMSrvMsg_Error errOut;

    if( !m_LaSch->getVariablePart(tmp, errOut) ) {
        m_CmdLogger.addMessageList(errOut);
        return;
    }

    int jobID(atoi(tmp));
    delete tmp;

    DBMSrvScd_Job* theJob = getJob(jobID, true);
    if( theJob == NULL ) {
        answerJobNoExistError();
    }
    else {
        if( userIsOwnerOrAdmin(*theJob) ) {
            if( theJob->hasSuccessors() ) {
                // cannot delete
                answerJobHasSuccessorsError(theJob);
            }
            else {
                // we can delete it
                m_Persistor->removeJob(theJob->getID());
                if( theJob->hasPredecessor() ) {
                    DBMSrvScd_Job* predJob = getJob(theJob->getPredecessor(), true);
                    if( predJob != NULL ) 
                        predJob->removeSuccessor(theJob->getID());
                }
                // it can be at root level even with a predecessor...
                for( DBMSrvScd_JobList::iterator jobIt = m_JobList->begin(); 
                                            jobIt != m_JobList->end(); jobIt++) {
                    if( ((*jobIt)->getID() == theJob->getID()) ) {
                        m_JobList->erase(jobIt);
                        delete theJob;
                        break;
                    }
                }
                answerOkay();
            }
        }
        else {
            answerUserNotOwnerError();
        }
    }
}

void DBMSrvScd_Scheduler::insertJob() {
    
    char* serJob(NULL);
    DBMSrvMsg_Error errOut;
    
    if( !m_LaSch->getVariablePart(serJob, errOut) ) {
        m_CmdLogger.addMessageList(errOut);
        return;
    }
    DBMSrvScd_Job* newJob = new DBMSrvScd_Job(serJob);
    delete serJob;

    newJob->setDBName(m_dbName);

    // if it has a predecessor, check if it exists
    if( newJob->hasPredecessor() ) {
        DBMSrvScd_Job* pred = getJob(newJob->getPredecessor(), true);
        if( pred == NULL ) {
            // error, predecessor does not exist
            if( !m_LaSch->setInfoType(DBMSrvShM_LineAccessorScheduler::IT_ErrorPredecessorNotExist, errOut) )
                m_CmdLogger.addMessageList(errOut);
            delete newJob;
            return;
        }
        else {
            // after persisting the job has a new ID
            if( pred->runOnce() )
                newJob->setRunOnce();

            m_Persistor->persistJob(newJob);
            pred->addSuccessor(newJob);
        }
    }
    else {
        // after persisting the job has a new ID
        m_Persistor->persistJob(newJob);
        m_JobList->push_back(newJob);
    }

    // prepare answer
    char idbuf[32]; 
    sprintf(idbuf, "%d", newJob->getID());
    if( !(m_LaSch->setInfoType(DBMSrvShM_LineAccessorScheduler::IT_JobID, errOut) && m_LaSch->setVariablePart(idbuf, errOut)) )
        m_CmdLogger.addMessageList(errOut);
}

void DBMSrvScd_Scheduler::returnJobList() {
    
    DBMSrvMsg_Error errOut;
    DBMSrvScd_JobList* persistedJobList(m_Persistor->loadJobs());
    
    if( m_LaSch->setInfoType(DBMSrvShM_LineAccessorScheduler::IT_SerializedJobList, errOut) ) {
        char* jl(persistedJobList->serialize());
        DBMSrvScd_JobList::deleteJobsInList(*persistedJobList);
        delete persistedJobList;
        m_LaSch->setVariablePart(jl, errOut);
        delete jl;
    }

    if( !errOut.IsEmpty() )
        m_CmdLogger.addMessageList(errOut);
}

void DBMSrvScd_Scheduler::answerOkay() {
    DBMSrvMsg_Error errOut;
    if( !m_LaSch->setInfoType(DBMSrvShM_LineAccessorScheduler::IT_None, errOut) )
        m_CmdLogger.addMessageList(errOut);
}

void DBMSrvScd_Scheduler::answerUserNotOwnerError() {
    DBMSrvMsg_Error errOut;
    if( !(m_LaSch->setInfoType(DBMSrvShM_LineAccessorScheduler::IT_ErrorWrongUser, errOut) &&
          m_LaSch->setUserName("", errOut) &&
          m_LaSch->setVariablePart("", errOut)) )
        m_CmdLogger.addMessageList(errOut);
}

void DBMSrvScd_Scheduler::answerJobNoExistError() {
    DBMSrvMsg_Error errOut;
    if( !(m_LaSch->setInfoType(DBMSrvShM_LineAccessorScheduler::IT_ErrorJobNotExist, errOut) &&
          m_LaSch->setUserName("", errOut) &&
          m_LaSch->setVariablePart("", errOut)) )
        m_CmdLogger.addMessageList(errOut);

}

void DBMSrvScd_Scheduler::answerJobHasSuccessorsError(DBMSrvScd_Job* aJob) {
    DBMSrvMsg_Error errOut;

    if( m_LaSch->setInfoType(DBMSrvShM_LineAccessorScheduler::IT_ErrorJobHasSuccessors, errOut) &&
        m_LaSch->setUserName("", errOut) ) {
        DBMSrvScd_JobList* succList = aJob->getSuccessors(false);
        char* jl(succList->serialize());
        delete succList;
        m_LaSch->setVariablePart(jl, errOut);
        delete[] jl;
    }
    if( !errOut.IsEmpty() )
        m_CmdLogger.addMessageList(errOut);
}

void DBMSrvScd_Scheduler::logPlannedTime(DBMSrvScd_Job* aJob) {
    do {
        char* jCmd(aJob->getCommand());
        if( jCmd == NULL )
            break;
        if( aJob->hasPlan() ) {
            char outDate[256];
            char outTime[256];
            struct tm lastTime = aJob->getPlan()->getLastPlannedExecutionTime();
            sprintf(outDate, "%d-%02d-%02d", lastTime.tm_year, lastTime.tm_mon, lastTime.tm_mday);
            sprintf(outTime, "%02d:%02d:%02d", lastTime.tm_hour, lastTime.tm_min, lastTime.tm_sec);
            m_CmdLogger.addMessageList(
                Msg_List(
                    Msg_List::Info,
                    SDBMSG_DBMSCD_JOBEXECUTEDAT,
	                Msg_Arg("SCDJOBID",(int)aJob->getID()),
                    Msg_Arg("DATE",outDate),
                    Msg_Arg("TIME",outTime),
                    Msg_Arg("DBMCMD", jCmd)));

        }
        else {
            m_CmdLogger.addMessageList(
                Msg_List(
                    Msg_List::Info,
                    SDBMSG_DBMSCD_JOBEXECUTEDAFTER,
	                Msg_Arg("SCDJOBID",(int)aJob->getID()),
                    Msg_Arg("SCDJOBIDPRED",(int)aJob->getPredecessor()),
                    Msg_Arg("DBMCMD", jCmd)));
        }

        if( jCmd != NULL )
            delete[] jCmd;
        return;
    } while (true);

    // error handling
    m_CmdLogger.addMessageList(
        Msg_List(
            Msg_List::Error,
            SDBMSG_DBMSCD_ERRLOGPLANNEDEXECUTION,
	        Msg_Arg("SCDJOBID",(int)aJob->getID())));
}

void DBMSrvScd_Scheduler::logJobReply(DBMSrvScd_Job* aJob) {

    // print the reply itself
    size_t replySize(0);
    char* reply = aJob->getReply(replySize);
    if( reply != NULL ) {
    m_CmdLogger.addMessageList(
        Msg_List(
            Msg_List::Info,
            SDBMSG_DBMSCD_JOBREPLY,
	        Msg_Arg("SCDJOBID",(int)aJob->getID()),
            Msg_Arg("SCDJOBREPLY",reply)));
        delete reply;
    }
    else
        //m_CmdLogger.addInfoString(m_LogErrorNoReply, jobId);
        m_CmdLogger.addMessageList(
            Msg_List(
                Msg_List::Error,
                SDBMSG_DBMSCD_ERRNOJOBREPLY,
	            Msg_Arg("SCDJOBID",(int)aJob->getID())));

    // print footer of the reply
    char outDate[256];
    char outTime[256];
    if( aJob->hasPlan() ) {
        struct tm lastTime = aJob->getPlan()->getLastPlannedExecutionTime();
        sprintf(outDate, "%02d/%02d/%d", lastTime.tm_mday, lastTime.tm_mon, lastTime.tm_year);
        sprintf(outTime, "%02d:%02d:%02d", lastTime.tm_hour, lastTime.tm_min, lastTime.tm_sec);
        m_CmdLogger.addMessageList(
            Msg_List(
                Msg_List::Info,
                SDBMSG_DBMSCD_JOBFINISHEDAT,
	            Msg_Arg("SCDJOBID",(int)aJob->getID()),
                Msg_Arg("DATE",outDate),
                Msg_Arg("TIME",outTime)));

    }
    else {
        m_CmdLogger.addMessageList(
            Msg_List(
                Msg_List::Info,
                SDBMSG_DBMSCD_JOBFINISHEDAFTER,
	            Msg_Arg("SCDJOBID",(int)aJob->getID()),
                Msg_Arg("SCDJOBID",(int)aJob->getPredecessor())));

    }
}
