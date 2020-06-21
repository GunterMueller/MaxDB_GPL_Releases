/*!
  \file    DBMSrvScd_Scheduler.hpp
  \author  MarcW
  \brief   scheduler declaration

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

#if !defined(DBMSrvScd_Scheduler_hpp)
#define DBMSrvScd_Scheduler_hpp

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Srv/Scheduler/DBMSrvScd_Persistor.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"

/*!
    @brief Class for scheduling DBM server commands
    
    A scheduled DBM server commands is called a job.
    Each of the jobs is executed according to a plan. The scheduled jobs are persisted
    using a persisence class, to keep the information about the scheduled jobs if the
    scheduler is stopped. 


    @see DBMSrvScd_Job
    @see DBMSrvScd_Plan
    @see DBMSrvScd_Persistor
*/
class DBMSrvScd_Scheduler {
public:
    /*!
        @brief constructor

        @param aDbName [IN] name of db, on which the scheduled DBM server commands
               are to be executed.
        @param persistenceFile [IN] folder in which the persistence file will be placed
    */
    DBMSrvScd_Scheduler(const char* aDbName, const char* persistenceFile);

    /*! @brief destructor */
    ~DBMSrvScd_Scheduler();

    /*!
        @brief main loop 

        The scheduler executable spends all its time in this method, mostly sleeping.
        Checking all scheduled jobs if one must be executed.
    */
    void run();

    /*!
        @brief return true if scheduler can be used
    */
    bool isValid() {return m_IsValid;}

private:
    DBMLog_Logger& m_CmdLogger;

    DBMSrvScd_Persistor* m_Persistor;
    DBMSrvScd_JobList* m_JobList;
    DBMSrvShM_LineAccessorScheduler* m_LaSch;
    char* m_dbName;
    bool m_MustTerminate;
    bool m_IsValid;

    void communicate();

    bool userIsOwnerOrAdmin(DBMSrvScd_Job& aJob);
    bool userIsAdmin();

    void activateJob() {changeJobStateTo(true);}
    void deactivateJob() {changeJobStateTo(false);}
    void deleteJob();
    void insertJob();
    void returnJobList();
    void answerUserNotOwnerError();
    void answerJobHasSuccessorsError(DBMSrvScd_Job* aJob);
    void answerJobNoExistError();
    void answerOkay();

    void prepareJobs();
    DBMSrvScd_Job* getJob(const int aJobID, const bool searchSuccessors);
    DBMSrvScd_Job* getJobRecursive(
            DBMSrvScd_JobList* aJobList,
            const int aJobID,
            const bool searchSuccessors );
    void changeJobStateTo(const bool newStateIsActive);
    void deleteAllJobs(DBMSrvScd_JobList* aJobList);

    void logPlannedTime(DBMSrvScd_Job* aJob);
    void logJobReply(DBMSrvScd_Job* aJob);

/*
    // for debugging
    void printjobs( DBMSrvScd_JobList* aJobList, int tabs );
*/

    static const int m_RunloopSleepTime;
};

#endif //DBMSrvScd_Scheduler_hpp
