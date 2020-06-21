/*!
  \file    DBMSrvScd_Persistor.hpp
  \author  MarcW
  \brief   Persistence for Jobs

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

#include "DBM/Srv/Scheduler/DBMSrvScd_Job.hpp"
#include "ToolsCommon/Tools_Properties.hpp"

#if !defined(DBMSrvScd_Persistor_hpp)
#define DBMSrvScd_Persistor_hpp

/*!
    @brief the scheduler's persistence

    The scheduler must store the scheduled jobs on disk, in case it is terminated. Immediately
    after a job is scheduled, it is stored on disk by the scheduler using the persistor.
    If the persistor is requested to store a job, it assigns a new id to it, if it has no
    id yet.
*/
class DBMSrvScd_Persistor {
public:
    /*!
        @brief constructor

        @param folder [IN] folder is which the persistence file is to be created (the file
               name is fixed).
    */
    DBMSrvScd_Persistor( const char* folder );

    /*! @brief destructor */
    ~DBMSrvScd_Persistor() {delete m_Persistence;}

    /*!
        @brief writes a job to disk
        
        If it has no id yet, a new id is generated. After the call returned,
        aJob has an id.
        @param aJob [IN] job to be persisted
        @return true if job could be persisted, false otherwise
    */
    bool persistJob( DBMSrvScd_Job* aJob);

    /*!
        @brief remove a job from disk

        @param aJobID [IN] id of job to be removed
        @return true if job could be removed, false otherwise
    */
    bool removeJob( int aJobID);

    /*!
        @brief load jobs from disk

        Jobs are loaded from disk and returned in a list of jobs. the Caller is
        responsible for the memory of the returned list and the jobs.
        @return list of jobs, list is empty if there are no persisted jobs.
    */
    DBMSrvScd_JobList* loadJobs();

    /*! @brief for cn42 file administration */
    static const char* const m_FileNameBase;

    /*! @brief for cn42 file administration */
    static const char* const m_FileNameExtension;

private:
    Tools_Properties* m_Persistence;

    static const char* const m_KeyFormat;
    static const char* const m_FileName;
};

#endif //DBMSrvScd_Persistor_hpp
