/*!
  \file    DBMSrvScd_Job.hpp
  \author  MarcW
  \brief   Job declaration

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

#include "DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp"
#include "DBM/Srv/Process/DBMSrvProc_IOManagedProcess.hpp"
#include "ToolsCommon/Tools_List.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

#if !defined(DBMSrvScd_Job_hpp)
#define DBMSrvScd_Job_hpp

// forward declaration
class DBMSrvScd_JobList;

/*!
    @brief entity subject to scheduling

    An instance of this class is something that can be scheduled. In contains all
    necessary information: a command to run, a user id under which to run the command,
    and flags whether this job is active and whether this job should only run once.

    Also, a job has either a plan when it should be executed or a precedessor. In this case it
    is executed after the predecessor is finished.

    A job runs its command with the help of the class DBMSrvProc_IOManagedProcess. A job
    is identified by an ID, that is assigned to it by the scheduler.

    The methods dealing with successors: concerned other jobs are never deleted, just
    references to them.

    @see DBMSrvProc_IOManagedProcess
    @see DBMSrvScd_Plan
    @see DBMSrvScd_Scheduler
*/
class DBMSrvScd_Job {
public:
    /*!
        @brief the stated of a job
    */
    enum JobState {
        Waiting,             /*!< this job is not executed at the moment */
        Running,             /*!< this job is executed at the moment */
        ReadyForSuccessors   /*!< this job has finished an executing. It's sucessors,
                                  if it has any, can be scheduled set to waiting now. */
    };

    static const int m_ReturnCodeUnknown;

    /*!
        @brief constructor for a job which its own plan

        Constructs a new job object.
        @param aID [IN] id of this job
        @param aUserID [IN] user id with which to run this job
        @param aActiveFlag [IN] active flag of this job. Job is only executed at the planned
               time or if the predecessor has finished, it it is active.
        @param aRunOnceFlag [IN] indicates, if this job is supposed to run only once.
        @param aCommandLine [IN] dbmcli command to be executed including parameters
        @param aPlan [IN] describes, when this job is to be executed. The plan is not copied,
               aPlan is deleted in destructor, a reference to a stack variable must not
               be passed.
    */
    DBMSrvScd_Job(
            const int aID,
            const char* aUserID,
            const bool aActiveFlag,
            const bool aRunOnceFlag,
            const char* aCommandLine,
            DBMSrvScd_Plan* aPlan);

    /*!
        @brief constructor for a job that is a successor of another job

        Constructs a new job object.
        @param aID [IN] id of this job
        @param aUserID [IN] user id with which to run this job
        @param aActiveFlag [IN] active flag of this job. Job is only executed at the planned
               time or if the predecessor has finished, it it is active.
        @param aRunOnceFlag [IN] indicates, if this job is supposed to run only once.
        @param aCommandLine [IN] dbmcli command to be executed including parameters
        @param aPredecessingJobID [IN] id of the predecessing job
    */
    DBMSrvScd_Job(
            const int aID,
            const char* aUserID,
            const bool aActiveFlag,
            const bool aRunOnceFlag,
            const char* aCommandLine,
            const int aPredecessingJobID );


    /*!
        @brief constructor for a job from a serialized job

        Constructs a new job object.
        @param  aJobDescription [IN] cunstructs a full blown job object from a serialized
                job.
        @see serialize()
    */
    DBMSrvScd_Job( const char* aJobDescription );
    
    /*! 
        @brief desctructor

        Deletes plan, so the plan must not be used after deletion of this job.
        @see DBMSrvScd_Job::DBMSrvScd_Job()
        @see serialize()
        @see getUserID()
        
    */
    virtual ~DBMSrvScd_Job();

    /*!
        @brief get job's ID

        @return job's ID
    */
    int getID() const {return m_MyID;}

    /*!
        @brief get job's user id

        @return reference to job's user id
    */
    const char* getUserID() const {return m_UserID;}

    /*!
        @brief get job's command line

        @return command line in new memory. Caller must delete it after use.
    */
    char* getCommand() const;

    /*!
        @brief get job's plan

        @return reference to job's plan
    */
    const DBMSrvScd_Plan* getPlan() const {return m_MyPlan;}

    /*
        @brief check whether a job must be executed now

        This method checks the plan, if the job must be run now.
        @return true if the job has no plan (is a successor) or the job's plan says yes, false otherwise
        @see DBMSrvScd_Plan
    */
    bool mustExecuteNow();

    /*!
        @brief execute a job

        If the job is not currently running, it is run immediately.
        If the job has a plan (is no a successor of some other job), the last execution time is updated
        @return true if the job is being run now, false otherwise
        @see DBMSrvScd_Plan
    */
    bool execute();

    /*!
        @brief get the pid of the DBMServer process, that executes this job

        The result is valid only if the job's state is Running
        @return pid of the DBMServer process, that executes this job
        @see JobState
        @see getState()
        
    */
    RTE_OSPid getPid();

    /*!
        @brief get job's current status

        @return job's current status
    */
    JobState getState();

    /*!
        @brief informs jobs, that it's successors are scheduled by the scheduler now.

        This method is called from the scheduler.
        @see DBMSrvScd_Scheduler
    */
    void setSuccessorsScheduled();
    
    /*! @brief activates a job */
    void activate();

    /*! @brief deactivates a job */
    void deactivate() {m_Active = false;}

    /*!
        @brief get activity status

        @return true is job is active, false otherwise
    */
    bool isActive() const {return m_Active;}

    /*!
        @brief get runonce status

        @return true is job is suppsed to run only once, false otherwise
    */
    bool runOnce() const {return m_RunOnce;}

    /*! @brief make jobe be run only once */
    void setRunOnce() {m_RunOnce = true;}

    /*!
        get id of predecessor

        If this job has a predecessor instead of an own plan, return it's id.
        Calling this makes sense only if this jobs has a predecessor.
        @see hasPredecessor()
    */
    int getPredecessor() const {return m_PredecessingJobID;}

    /*!
        @brief check whether this job has a predecessor

        @return true if this job has a predecessor (instead of a plan), false otherwise
    */
    bool hasPredecessor() const {return m_PredecessingJobID!=-1;}

    /*!
        @brief check whether this job has a plan

        @return true if this job has a plan (instead of a predecessor), false otherwise
    */
    bool hasPlan() const {return m_MyPlan != NULL;}

    /*!
        @brief add a job as a successor to this job

        When a job has no plan, it has a predecessor. If added to the scheduler, the
        job without the plan is added as a successor to it predecessor. This information
        is not serialized in the job. One job can be a successor of several jobs.
    */
    void addSuccessor(DBMSrvScd_Job* successorJob);

    /*! @brief removes the successor ids from this job */
    void clearSuccessors();

    /*!
        @brief check whether this job has successors

        @return true if this job has successors, false otherwise
    */
    bool hasSuccessors();

    /*!
        @brief remove successor from this job
        
        @param aSuccID [IN] id of job to me removed from list of successors
     */
    void removeSuccessor(const int aSuccID);

    /*!
        @brief get all successors of this job

        @param onlyActiveJobs [IN] if true, only the active successors are in
               the returned list. If false, all succesors are in the list.
        @return list of jobs. The list must be deleted by the caller.
    */
    DBMSrvScd_JobList* getSuccessors(const bool onlyActiveJobs);

    /*!
        @brief to be called regularly if job is running

        This method updates the information in the job. E.g. the output of the process,
        that is part of this job is read and buffered.
    */
    void touch();

    /*!
        @brief set name of database on which command shall be executed

        This information is not stored in the job. Instead, it is provided by the
        scheduler after instanciation.
        @param aDBName [IN] name of database on which command shall be executed
    */
    void setDBName( const char* aDBName );

    /*!
        @brief caller is responsible for memory allocated here!

        The log is 0-terminated. logSize includes the terminator.
    */
    char* getLog(size_t& logSize);

    /*!
        @brief caller is responsible for memory allocated here!

        The reply is 0-termianted. replySize includes the terminator.
    */
    char* getReply(size_t& replySize);

    int getLastReturnCode() const;
    RTE_OSPid getLastServerPid() const;
    char* getLastExecutionTimeString() const;

    /*!
        @brief serialize this job

        From the returned string reference, a job object can be created using the appropriate
        constructor. New memory is allocated. Id, user id, command line,
        active flag, run once flag and predecessor resp. plan are contained.
    */
    char* serialize();

private:
    // to avoid copying
    DBMSrvScd_Job(const DBMSrvScd_Job& otherJob) {}

    int m_MyID;
    char* m_UserID;
    char* m_DBName;
    bool m_Active;
    bool m_RunOnce;
    char* m_CommandLine;
    DBMSrvScd_Plan* m_MyPlan;
    int m_PredecessingJobID;
    DBMSrvProc_IOManagedProcess* m_Process;
    DBMSrvScd_JobList* m_SuccessingJobs;

    struct tm m_LastExecutionTime;
    int m_LastReturnCode;
    RTE_OSPid m_LastServerPid;

    void setID(int aID) {m_MyID = aID;}
    char* getPasswordForUser();
    void createBackgroundProcess();
    
    void setToNow(struct tm& nowOut);

    static const char* SEPSTRING;
    static const char* ENDSTRING;
    static const char SEPCHAR;

    friend class DBMSrvScd_Persistor;
};

/*!
    @brief list of jobs

    Allows serialization and deserialization of a list of jobs as well as
    all other list operations.
*/
class DBMSrvScd_JobList: public Tools_List<DBMSrvScd_Job*> {
public:
    /*!
        @brief constructs a list of jobs from the serialized job list

        @param aJobListDescription [IN] serialized job list
        @see serialize()
    */
    DBMSrvScd_JobList( const char* aJobListDescription);

    /*! @brief constructs an empty job list */
    DBMSrvScd_JobList();

    /*! @brief removes the list, not the referenced jobs */
    virtual ~DBMSrvScd_JobList() {};

    /*!
        @brief serialize the list

        New memory is allocated.
        @return reference to the serialized job list.
    */
    char* serialize();

    /*!
        @brief deletes jobs in a job list

        Convenience method, since the list does not delete the referenced objects
        @param aJobList [IN] reference to a job list
        @see ~DBMSrvScd_JobList()
    */
    static void deleteJobsInList(DBMSrvScd_JobList& aJobList);

private:
    static const char* const SEPSTRING;
    static const char* const ENDSTRING;
};

#endif //DBMSrvScd_Job_hpp

