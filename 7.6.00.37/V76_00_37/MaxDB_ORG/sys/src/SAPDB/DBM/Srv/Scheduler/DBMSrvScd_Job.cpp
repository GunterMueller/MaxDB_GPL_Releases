/*!
  \file    DBMSrvScd_Job.cpp
  \author  MarcW
  \brief   plan implementation

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

#include <stdio.h>
#include <stdlib.h>

#include "hcn50.h"

#include "DBM/Srv/Scheduler/DBMSrvScd_Job.hpp"

const char* DBMSrvScd_Job::SEPSTRING = "<jp>";
const char DBMSrvScd_Job::SEPCHAR = '.';
const char* DBMSrvScd_Job::ENDSTRING = "</jp>";

const int DBMSrvScd_Job::m_ReturnCodeUnknown = -25000;

DBMSrvScd_Job::DBMSrvScd_Job(
            const int aID, 
            const char* aUserID,
            const bool aActiveFlag,
            const bool aRunOnceFlag,
            const char* aCommandLine,
            DBMSrvScd_Plan* aPlan)
        : m_MyID(aID),
          m_UserID(new char[strlen(aUserID)+1]),
          m_DBName(NULL),
          m_SuccessingJobs(NULL),
          m_MyPlan(aPlan),
          m_Process(NULL),
          m_Active(aActiveFlag),
          m_RunOnce(aRunOnceFlag),
          m_CommandLine(new char[strlen(aCommandLine)+1]),
          m_LastReturnCode(m_ReturnCodeUnknown),
          m_LastServerPid(0),
          m_PredecessingJobID(-1) {

    // reset time struct
    int* pInt=(int*)&m_LastExecutionTime;
    for( int i=0; i<8; i++ )
        *(pInt+i) = 0;
              
    SAPDB_strcpy(m_UserID, aUserID);
    SAPDB_strcpy(m_CommandLine, aCommandLine);
}

DBMSrvScd_Job::DBMSrvScd_Job(
            const int aID, 
            const char* aUserID,
            const bool aActiveFlag,
            const bool aRunOnceFlag,
            const char* aCommandLine,
            const int aPredecessingJobID)
        : m_MyID(aID),
          m_UserID(new char[strlen(aUserID)+1]),          
          m_DBName(NULL),
          m_SuccessingJobs(NULL),
          m_MyPlan(NULL),
          m_Process(NULL),
          m_Active(aActiveFlag),
          m_RunOnce(aRunOnceFlag),
          m_CommandLine(new char[strlen(aCommandLine)+1]),
          m_LastReturnCode(m_ReturnCodeUnknown),
          m_LastServerPid(0),
          m_PredecessingJobID(aPredecessingJobID) {

    // reset time struct
    int* pInt=(int*)&m_LastExecutionTime;
    for( int i=0; i<8; i++ )
        *(pInt+i) = 0;

    SAPDB_strcpy(m_UserID, aUserID);
    SAPDB_strcpy(m_CommandLine, aCommandLine);
}

DBMSrvScd_Job::DBMSrvScd_Job( const char* aJobDescription )
        : m_MyID(-1),
          m_UserID(NULL),
          m_DBName(NULL),
          m_SuccessingJobs(NULL),
          m_MyPlan(NULL),
          m_Process(NULL),
          m_Active(false),
          m_RunOnce(false),
          m_CommandLine(NULL),
          m_LastReturnCode(m_ReturnCodeUnknown),
          m_LastServerPid(0),
          m_PredecessingJobID(-1) {

    // reset time struct
    int* pInt=(int*)&m_LastExecutionTime;
    for( int i=0; i<8; i++ )
        *(pInt+i) = 0;

    char serializedJob[4096];
    SAPDB_strcpy(serializedJob, aJobDescription);

    char* end(NULL);
    char* start(NULL);

    // get ID
    end = strstr(serializedJob+strlen(SEPSTRING), SEPSTRING);
    *end = '\0';
    m_MyID = atoi(serializedJob+strlen(SEPSTRING));
    *end = SEPSTRING[0];

    // get user id
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    m_UserID = new char[(end-start)+1];
    *end = '\0';
    SAPDB_strcpy(m_UserID, start);
    *end = SEPSTRING[0];

    // get commandLine
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    m_CommandLine = new char[(end-start)+1];
    *end = '\0';
    SAPDB_strcpy(m_CommandLine, start);
    *end = SEPSTRING[0];

    // get active flag
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    m_Active = (start[0] == '1');

    // get run once flag
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    m_RunOnce = (start[0] == '1');

    // get predecessing job id
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    m_PredecessingJobID = atoi(start);

    // get last serverpid
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    m_LastServerPid = atoi(start);

    // get last return code
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    m_LastReturnCode = atoi(start);

    // get last execution time
    start = end + strlen(SEPSTRING);
    end = strstr(start, SEPSTRING);
    if( strlen(start) > 0 ) {
        // there is a last execution time
        int* pInt = (int*)&m_LastExecutionTime;
        for( int i=0; i<8; i++ ) {
            *(pInt+i) = atoi(start);
            start = strchr(start, SEPCHAR) + 1;
        }
    }

    // get plan if there is one
    start = end + strlen(SEPSTRING);
    end = strstr(start, ENDSTRING);
    *end = '\0';
    if( strlen(start) > 0 ) {
        // there is a plan
        m_MyPlan = new DBMSrvScd_Plan(start);
    }
    *end = ENDSTRING[0];
}

int DBMSrvScd_Job::getLastReturnCode() const {
    return m_LastReturnCode;
}

RTE_OSPid DBMSrvScd_Job::getLastServerPid() const {
    return m_LastServerPid;
}

char* DBMSrvScd_Job::getLastExecutionTimeString() const {
    char* pt = new char[20]; // definition os asctime
    if( pt != NULL ) {
        sprintf(pt, "%04d-%02d-%02d %02d:%02d:%02d",
            m_LastExecutionTime.tm_year + 1900,
            m_LastExecutionTime.tm_mon + 1,
            m_LastExecutionTime.tm_mday,
            m_LastExecutionTime.tm_hour,
            m_LastExecutionTime.tm_min,
            m_LastExecutionTime.tm_sec);
    }
    return pt;
}

DBMSrvScd_Job::~DBMSrvScd_Job() {

    delete[] m_CommandLine;

    if( m_UserID != NULL )
        delete[] m_UserID;

    if( m_MyPlan != NULL )
        delete m_MyPlan;

    if( m_Process != NULL )
        delete m_Process;

    if( m_SuccessingJobs != NULL )
        delete m_SuccessingJobs;

    if( m_DBName != NULL )
        delete[] m_DBName;
}

void DBMSrvScd_Job::setDBName( const char* aDBName ) {
    if( m_DBName != NULL )
        delete[] m_DBName;

    m_DBName = new char[strlen(aDBName)+1];
    SAPDB_strcpy( m_DBName, aDBName );
}

char* DBMSrvScd_Job::serialize() {

    // length of command line
    size_t serLen(strlen(m_CommandLine));

    // add length of my id
    int id = m_MyID;
    if( id == 0 )
        serLen++;
    else if( id < 0 ) {
        serLen++;
        id = -id;
    }

    while( id > 0 ) {
        serLen++;
        id = id / 10;
    }
    
    // add length of user id
    serLen += strlen(m_UserID);

    // add one char for the active flag
    serLen++;

    // add one char for the run  once flag
    serLen++;

    // add length of predecessing job id
    id = m_PredecessingJobID;
    if( id >= 0 ) {
        if( id == 0 )
            serLen++;
        while( id > 0 ) {
            serLen++;
            id = id / 10;
        }
    }
    else
        // it's -1
        serLen += 2;

    // add length of serving pid
    RTE_OSPid aPid = m_LastServerPid;
    serLen++;
    while( aPid > 0 ) {
        serLen++;
        aPid = aPid / 10;
    }

    // add length of last return code
    id = m_LastReturnCode;
    serLen++;
    if( id < 0 ) {
        id = -id;
        serLen++;
    }
    while( id > 0 ) {
        serLen++;
        id = id / 10;
    }
    
    // add length of the last execution time struct
    size_t letLen(0);
    int* pInt = (int*) &m_LastExecutionTime;
    for( int i=0; i<8; i++ ) {
        int part = *(pInt+i);
        if( part == 0 )
            letLen++;
        while( part > 0 ) {
            letLen++;
            part = part / 10;
        }
        letLen++; // for the separator
    }
    char* serLastExecutionTime(new char[letLen+1]);
    if( serLastExecutionTime != NULL ) {
        sprintf( serLastExecutionTime,
            "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c",
            m_LastExecutionTime.tm_sec,
            SEPCHAR,
            m_LastExecutionTime.tm_min,
            SEPCHAR,
            m_LastExecutionTime.tm_hour,
            SEPCHAR,
            m_LastExecutionTime.tm_mday,
            SEPCHAR,
            m_LastExecutionTime.tm_mon,
            SEPCHAR,
            m_LastExecutionTime.tm_year,
            SEPCHAR,
            m_LastExecutionTime.tm_wday,
            SEPCHAR,
            m_LastExecutionTime.tm_yday,
            SEPCHAR);

        serLen += letLen;
    }
    else {
        serLastExecutionTime = new char[1];
        serLastExecutionTime[0] = '\0';
    }

    // add length of serialized plan if there is one
    char* serPlan(NULL);
    if( m_MyPlan != NULL )
        serPlan = m_MyPlan->serialize();

    if( serPlan == NULL ) {
        serPlan = new char[1];
        serPlan[0] = '\0';
    }

    serLen += strlen(serPlan);

    // add length of separators
    serLen += strlen(SEPSTRING) * 10;
    serLen += strlen(ENDSTRING);

    // one terminator
    serLen++;
    
    // now create it
    char* serializedJob = new char[serLen];
    if( serializedJob != NULL ) {
        sprintf(serializedJob, "%s%d%s%s%s%s%s%c%s%c%s%d%s%d%s%d%s%s%s%s%s",
            SEPSTRING,
            m_MyID,
            SEPSTRING,
            m_UserID,
            SEPSTRING,
            m_CommandLine,
            SEPSTRING,
            m_Active?'1':'0',
            SEPSTRING,
            m_RunOnce?'1':'0',
            SEPSTRING,
            m_PredecessingJobID,
            SEPSTRING,
            m_LastServerPid,
            SEPSTRING,
            m_LastReturnCode,
            SEPSTRING,
            serLastExecutionTime,
            SEPSTRING,
            serPlan,
            ENDSTRING);
    }

    if( serLastExecutionTime != NULL )
        delete[] serLastExecutionTime;
    if( serPlan != NULL )
        delete[] serPlan; 
    return serializedJob;
}

char* DBMSrvScd_Job::getCommand() const {
    char* cmd = new char[strlen(m_CommandLine)+1];
    SAPDB_strcpy(cmd, m_CommandLine);
    return cmd;
}

bool DBMSrvScd_Job::hasSuccessors() {
    return m_SuccessingJobs != NULL && !m_SuccessingJobs->empty();
}

char* DBMSrvScd_Job::getPasswordForUser() {
    tsp00_DbNamec dbName;
    tcn00_UserNamec userName;
    dbName.rawAssign(m_DBName);
    userName.rawAssign(m_UserID);

    cn50DBMUser theDBMUser( dbName, userName );
    tsp00_Namec pw = theDBMUser.getClearMasterPwd();
    char* password = new char[strlen(pw.asCharp())+1];
    SAPDB_strcpy(password, pw.asCharp());
    return password;
}

void DBMSrvScd_Job::touch() {
    m_Process->digestNextCommandAnswerPart();
}

void DBMSrvScd_Job::createBackgroundProcess() {
    char* password = getPasswordForUser();
    m_Process = new DBMSrvProc_IOManagedProcess( m_DBName, m_UserID, password, m_CommandLine);
    delete password;
}

RTE_OSPid DBMSrvScd_Job::getPid() {
    if( m_Process == NULL )
        return 0;
    else
        return m_Process->getServerPid();
}

bool DBMSrvScd_Job::execute() {
    if( m_Process != NULL && m_Process->isRunning() )
        return false;
    else {
        if( m_Process != NULL ) {
            delete m_Process;
            m_Process = NULL;
        }
        if( m_MyPlan != NULL )
            // for the plan to be able to answer the question itsTimeToRun()
            m_MyPlan->updateExecutionTimes();
        createBackgroundProcess();
        
        // for the records: last execution time
        setToNow(m_LastExecutionTime);

        // for the records: serving pid
        m_LastServerPid = m_Process->getServerPid();

        return true;
    }
}

bool DBMSrvScd_Job::mustExecuteNow() {
    if( !m_Active )
        return false;

    if( m_MyPlan == NULL ) {
        // there is no own plan, we're a successor
        return !(m_Process != NULL && m_Process->isRunning());
    }

    // ask the plan
    if( m_MyPlan->itsTimeToRun() ) {
        if( m_Process != NULL && m_Process->isRunning() )
            return false;
        return true;
    }
    else
        return false;
}

void DBMSrvScd_Job::setToNow(struct tm& nowOut) {
    time_t now;
    time(&now);
    struct tm* nowStatic = localtime(&now);
    nowOut = *nowStatic;
}

DBMSrvScd_Job::JobState DBMSrvScd_Job::getState() {
    if( m_Process == NULL )
        return Waiting;
    else {
        if( m_Process->isRunning() )
            return Running;
        else {
            return ReadyForSuccessors;
        }
    }
}

void DBMSrvScd_Job::setSuccessorsScheduled() {
    if( m_Process != NULL ) {
        // save the returncode
        if( !m_Process->getReturnCode(m_LastReturnCode) ) {
            // we could not get the returncode
            m_LastReturnCode = m_ReturnCodeUnknown;
        }
        delete m_Process;
        m_Process = NULL;
    }
}

void DBMSrvScd_Job::addSuccessor(DBMSrvScd_Job* successorJob) {
    bool alreadyContained(false);

    if( m_SuccessingJobs == NULL )
        m_SuccessingJobs = new DBMSrvScd_JobList();
    else {
        DBMSrvScd_JobList::iterator jobIt = NULL;
        // no doubles
        for( jobIt = m_SuccessingJobs->begin(); !alreadyContained && jobIt != m_SuccessingJobs->end(); jobIt++ )
            alreadyContained = ((*jobIt)->getID() == successorJob->getID());
    }

    if( !alreadyContained )
        m_SuccessingJobs->push_back(successorJob);
}

void DBMSrvScd_Job::removeSuccessor(const int aSuccID) {
    if( m_SuccessingJobs == NULL )
        return;

    DBMSrvScd_JobList::iterator jobIt = m_SuccessingJobs->begin();
    bool notFound((*jobIt)->getID() != aSuccID);
    for( ; notFound && jobIt != m_SuccessingJobs->end(); jobIt++ )
        notFound = (*jobIt)->getID() != aSuccID;

    if( !notFound )
        m_SuccessingJobs->erase(jobIt);
}

void DBMSrvScd_Job::clearSuccessors() {
    delete m_SuccessingJobs;
    m_SuccessingJobs = new DBMSrvScd_JobList();
}

void DBMSrvScd_Job::activate() {
    m_Active = true;
    if( m_MyPlan != NULL )
        m_MyPlan->updateExecutionTimes();
}


DBMSrvScd_JobList* DBMSrvScd_Job::getSuccessors(const bool onlyActiveJobs) {
    DBMSrvScd_JobList* jl = new DBMSrvScd_JobList();
    if( hasSuccessors() ) {
        DBMSrvScd_JobList::iterator jobIt = NULL;
        for( jobIt = m_SuccessingJobs->begin(); jobIt != m_SuccessingJobs->end(); jobIt++ ) {
            if( !onlyActiveJobs || (*jobIt)->isActive() )
                jl->push_back(*jobIt);
        }
    }
    return jl;
}

char* DBMSrvScd_Job::getLog(size_t& logSize) {
    if( m_Process == NULL ) {
        logSize = 0;
        return NULL;
    }
    else
        return m_Process->getLog(logSize);
}

char* DBMSrvScd_Job::getReply(size_t& replySize) {
    if( m_Process == NULL ) {
        replySize = 0;
        return NULL;
    }
    else
        return m_Process->getReply(replySize);
}

const char* const DBMSrvScd_JobList::SEPSTRING = "<jvp>";
const char* const DBMSrvScd_JobList::ENDSTRING = "</jvp>";

DBMSrvScd_JobList::DBMSrvScd_JobList()
    : Tools_List<DBMSrvScd_Job*>() {}

// we assume that the string is a correct job vector description
DBMSrvScd_JobList::DBMSrvScd_JobList( const char* aJobListDescription)
    : Tools_List<DBMSrvScd_Job*>() {

   Tools_DynamicUTF8String serializedJobList(aJobListDescription);
   if( serializedJobList.Length() > strlen(SEPSTRING) + strlen(ENDSTRING) ) {
        // the serialized vector is not empty
        serializedJobList = serializedJobList.SubStr(
            (Tools_DynamicUTF8String::BasisElementIndex)strlen(SEPSTRING));
        Tools_DynamicUTF8String::BasisElementIndex end(0);
        while( !serializedJobList.Empty() ) {
            push_back(new DBMSrvScd_Job(serializedJobList.CharPtr()));
            end = serializedJobList.Find(SEPSTRING);
            if( end != Tools_DynamicUTF8String::NPos ) {
                serializedJobList = serializedJobList.SubStr(
                    end +
                    (Tools_DynamicUTF8String::BasisElementIndex)strlen(SEPSTRING));
            }
            else {
                end = serializedJobList.Find(ENDSTRING);
                serializedJobList = serializedJobList.SubStr(
                    end +
                    (Tools_DynamicUTF8String::BasisElementIndex)strlen(ENDSTRING));
            }
        }
    }
}

char* DBMSrvScd_JobList::serialize() {
    Tools_DynamicUTF8String serializedJobList;
    serializedJobList.Erase();

    serializedJobList += SEPSTRING;
    iterator jobIt = NULL;
    for( jobIt = begin(); jobIt != end(); ) {
        DBMSrvScd_Job* aJob = *jobIt;
        char* sj(aJob->serialize());
        serializedJobList += sj;
        delete[] sj;
        if( ++jobIt != end() )
            serializedJobList += SEPSTRING;
    }
    serializedJobList += ENDSTRING;
    char* serList(new char[serializedJobList.Length()+1]);
    SAPDB_strcpy(serList, serializedJobList.CharPtr());
    return serList;
}

void DBMSrvScd_JobList::deleteJobsInList(DBMSrvScd_JobList& aJobList) {
    for( iterator jobIt = aJobList.begin(); jobIt != aJobList.end(); ++jobIt ) {
        if( *jobIt != NULL ) {
            delete *jobIt;
            *jobIt = NULL;
        }
    }
}
