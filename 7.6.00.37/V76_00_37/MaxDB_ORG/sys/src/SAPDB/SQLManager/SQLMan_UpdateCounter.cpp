/*****************************************************************************/
/*!
  @file         SQLMan_UpdateCounter.cpp
  @author       MartinKi

  @brief        Header file for class SQLMan_UpdateCounter.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_UpdateCounter.hpp"

#include "SQLManager/Tables/SQLTab_SysUpdateCounterWanted.hpp"
#include "SQLManager/SQLMan_InternalTrans.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"

#include "DataAccess/Data_UpdateCounterSrvTask.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"

#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"
#include "SAPDB/Messages/Msg_List.hpp"

#include "KernelCommon/Kernel_VTrace.hpp"

#include "Logging/Log_ActionSetFileDirCounter.hpp"

#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "vak001.h"
#include "hak06.h"
#include "hkb50.h"  // k50new_write_trans
#include "hkb51.h"  // kb51ExclusiveRequestExists
#include "hbd01.h"  // bd01UsableServerForParallelExecution
#include "hgg06.h"  // gg06IsNilTrans
#include "heo56.h"  // vsleep

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*****************************************************************************/

SAPDB_Int SQLMan_UpdateCounter::m_isRunning = cUpdCntNotRunning;

namespace {

/// List with all running jobs.
typedef SrvTasks_JobList<Data_UpdateCounterSrvTask> Srv_StatJobList;

/*!
 * How long to sleep before starting the update loop. This is the
 * minimum time between sending the command to the kernel, and the
 * kernel actually starting to initialize the file directory counters.
 */
const SAPDB_UInt  cSleepBeforeStartingTime         = 300;
/*!
 * How long to sleep if the database is not idle and no jobs are
 * running before the next check of the database state is done.
 */
const SAPDB_UInt  cSleepWhenBusyTime               = 10;
/*!
 * How long to sleep if the database is not idle but there are still
 * jobs running that need to be collected (much shorter than
 * cSleepWhenBusyTime because we need to collect the jobs fast to free
 * the locks they are holding).
 */
const SAPDB_UInt  cSleepWhenBusyButJobsRunningTime = 2;
/*!
 * How long to sleep if the database is idle and we could be doing
 * some work, but all tables that we are interested in are locked.
 */
const SAPDB_UInt  cSleepWhenTablesLockedTime       = 60;
/*!
 * How long must the database have been idle before we consider it
 * idle.
 */
const SAPDB_ULong cMinIdleTime                     = 5;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class JobInfo
{
private:
    SAPDB_Bool                          m_tableLocked;
    SAPDBMem_IRawAllocator&             m_allocator;
    Data_UpdateCounterSrvTask*          m_pJob;
    FileDir_FileNo                      m_fileNo;
    FileDir_FileNo                      m_tableFileNo;
    SQLMan_InternalTrans                m_internalTrans;

    JobInfo(const JobInfo&);
    JobInfo& operator=(const JobInfo&);
public:
    JobInfo(SQLMan_Context& context)
        : m_allocator( context.GetAllocator() ),
          m_pJob( 0 ),
          m_internalTrans( context ),
          m_tableLocked( false )
    {}

    enum LockRequestRc {
        lrRcOk,
        lrRcLockCollision,
        lrRcFileNotFound,
        lrRcCancelled,
        lrRcSystemError
    };

    enum JobResult {
        jobSuccessful,
        jobFailed
    };

    /****************************************************************/

    SAPDB_Bool Initialize(Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::JobInfo", SQLMan_Trace, 5);

        return m_internalTrans.NewTrans( errMsg );
    }

    /****************************************************************/

    SAPDB_Bool Finalize(Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::Finalize", SQLMan_Trace, 5);

        return m_internalTrans.Close(
            SQLMan_InternalTrans::doRollback,
            errMsg );
    }

    /****************************************************************/

    ~JobInfo()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::~JobInfo", SQLMan_Trace, 5);

        if ( m_pJob != 0 ) {
            this->DestroyJob();
        }
    }

    /****************************************************************/

    SQLMan_InternalTransInfo& GetInternalTransInfo()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::GetInternalTransInfo",
            SQLMan_Trace, 8);

        return m_internalTrans;
    }

    /****************************************************************/

    SAPDB_Bool CommitInternalTrans(Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::CommitInternalTrans",
            SQLMan_Trace, 5);

        return m_internalTrans.CommitInternal( errMsg );
    }

    /****************************************************************/

    SAPDB_Bool IsValid() const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::IsValid", SQLMan_Trace, 5);

        return (m_pJob != 0) && m_fileNo.IsValid();
    }

    /****************************************************************/

    void SetJob(Data_UpdateCounterSrvTask& job)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::SetJob", SQLMan_Trace, 5);

        m_pJob = &job;
    }

    /****************************************************************/

    Data_UpdateCounterSrvTask& GetJob() const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::GetJob", SQLMan_Trace, 8);

        return *m_pJob;
    }

    /****************************************************************/

    void DestroyJob()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::DestroyJob", SQLMan_Trace, 5);

        destroy( m_pJob, m_allocator );
        m_pJob = 0;
    }

    /****************************************************************/

    SAPDBMem_IRawAllocator& GetAllocator()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::GetAllocator", SQLMan_Trace, 8);

        return m_allocator;
    }

    /****************************************************************/

    FileDir_FileNo const& GetFileNo() const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::GetFileNo", SQLMan_Trace, 8);

        return m_fileNo;
    }

    /****************************************************************/

    void SetFileNo(const FileDir_FileNo& fileNo)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::SetFileNo", SQLMan_Trace, 8);

        m_fileNo = fileNo;
    }

    /****************************************************************/

    // only initialized after table has been locked !
    FileDir_FileNo const& GetTableFileNo() const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::GetTableFileNo", SQLMan_Trace, 8);

        return m_tableFileNo;
    }

    /****************************************************************/

    /*!
     * Closes the internal transaction (and commits or rolls back based
     * on jobResult).
     */
    void SetJobDone(JobResult jobResult, Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::SetJobDone", SQLMan_Trace, 5);

        m_internalTrans.Close(
            jobResult == jobSuccessful
            ? SQLMan_InternalTrans::doCommit
            : SQLMan_InternalTrans::doRollback,
            errMsg );

        errMsg.ClearMessageList();

        this->DestroyJob();
    }

    /****************************************************************/

    LockRequestRc LockTable(SQLMan_Context& context, Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::LockTable", SQLMan_Trace, 5);

        SAPDBERR_ASSERT_STATE( m_fileNo.IsValid() );

        if ( ! m_fileNo.IsValid() ) {
            return lrRcSystemError;
        }

        FileDir_IDirectory& fDir = FileDir_IDirectory::SessionInstance(
            context.TransContext() );

        FileDir_Generic fileEntry;
        // we need to use GcGetFile because a file might be marked
        // deleted but not yet committed; we will either
        // fail to get the lock or (if the delete has been committed
        // later on) get our file_not_found later, in which case it is
        // safe to remove the file from SysUpdateCounterWanted
        FileDir_ReturnCode fDirRc = fDir.GcGetFile(
            context.GetTransContext().TaskId(),
            m_fileNo,
            fileEntry,
            errMsg );

        if ( fDirRc == FileDir_FileNoNotFound ) {
            return lrRcFileNotFound;
        }

        SAPDBERR_ASSERT_STATE( fDirRc == FileDir_Okay );

        m_tableFileNo = fileEntry.GetFileNo();

        if ( fileEntry.GetFileType() == Data_IndexFile ) {

            FileDir_Index indexEntry;
            indexEntry.Assign( fileEntry );
            m_tableFileNo = indexEntry.GetParentFileNo();

        }

        tgg00_FileId fileId;
        fileEntry.BuildFileId( fileId );
        fileId.fileTabId_gg00().rawAssign( m_tableFileNo.GetCharArray() );

        return this->SendLockRequest( context, fileId, errMsg );
    }

    /****************************************************************/

    LockRequestRc SendLockRequest(
        SQLMan_Context& context,
        tgg00_FileId&   fileId,
        Msg_List&       errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobInfo::SendLockRequest", SQLMan_Trace, 5);

        SAPDBERR_ASSERT_ARGUMENT( context.IsOk() );

        tgg00_BasisError bErr;

        a06a_mblock_init( context, m_lock, mm_test, fileId );
        context.a_mblock.mb_qual()->mlock_cnt() = 1;
        context.a_mblock.mb_qual()->mtree().fileHandling_gg00() =
            tgg00_HandlingSet::build( 1, hsNoWait_egg00 );

        tgg00_Lock& mlock = *context.a_mblock.mb_qual()->mlock();
        mlock.lockTabId_gg00 = fileId.fileTabId_gg00();
        mlock.lockSite_gg00.rawAssign( cgg_zero_c2 );
        mlock.lockMode_gg00.becomes( lckTabShare_egg00 );
        mlock.lockState_gg00.clear();
        mlock.lockKeyLen_gg00 = 0;
        mlock.lockKeyPos_gg00 = 0;

        a06rsend_mess_buf( context, context.a_mblock, cak_return_req, bErr );

        if ( bErr != e_ok ) {
            switch ( bErr ) {
                case e_lock_collision:
                    return lrRcLockCollision;
                case e_cancelled:
                    return lrRcCancelled;
                default:
                    errMsg = context.GetTransContext().ErrorList();
                    errMsg.Overrule(
                        Msg_List(
                            Msg_List::Error,
                            SDBMSG_SQLMAN_UPDCNT_LOCK_TABLE_FAILED,
                            Msg_Arg(SDBMSGTAG_SQLMAN_UPDCNT_LOCK_TABLE_FAILED__FILENO,
                                    FileDir_FileNoToString( fileId.fileTabId_gg00() )),
                            Msg_Arg(SDBMSGTAG_SQLMAN_UPDCNT_LOCK_TABLE_FAILED__ERRORTEXT,
                                    SQLManErr_Interface::GetInstance()
                                    .GetBaseErrorText( bErr )),

                            Msg_Arg(SDBMSGTAG_SQLMAN_UPDCNT_LOCK_TABLE_FAILED__BASIS_ERROR,
                                    bErr )
                            ) );

                    return lrRcSystemError;
            }
        }

        SAPDBERR_ASSERT_STATE( context.IsOk() );

        return lrRcOk;
    }
};

/*****************************************************************************/
/*****************************************************************************/

class JobList : public Container_Vector<JobInfo*>
{
private:
    typedef Container_Vector<const Data_UpdateCounterSrvTask*> JobPointerList;
    JobPointerList m_cancelledJobsList;

public:
    JobList(SAPDBMem_IRawAllocator& alloc)
        : Container_Vector<JobInfo*>( alloc ),
          m_cancelledJobsList( alloc )
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobList::JobList", SQLMan_Trace, 5);
    }

    ~JobList()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobList::~JobList", SQLMan_Trace, 5);

        for (SAPDB_UInt i = 0; i < this->GetSize(); ++i ) {
            destroy( operator[]( i ), this->GetAllocator() );
        }
        this->Delete();
    }

    SAPDB_Bool Clear(Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobList::Clear", SQLMan_Trace, 5);

        SAPDB_Bool isOk = true;

        for (SAPDB_UInt i = 0; i < this->GetSize(); ++i ) {

            isOk &= this->operator[]( i )->Finalize( errMsg );
            destroy( operator[]( i ), this->GetAllocator() );

        }

        Container_Vector<JobInfo*>::Clear();
        m_cancelledJobsList.Clear();

        return isOk;
    }

    JobInfo* RemoveServerJob(const Data_UpdateCounterSrvTask& serverJob)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobList::RemoveServerJob", SQLMan_Trace, 5);

        for (SAPDB_UInt i = 0; i < this->GetSize(); ++i ) {
            if ( &serverJob == &(*this)[ i ]->GetJob() ) {

                JobInfo* pCompletedJob = (*this)[ i ];
                this->Delete( i );
                this->RemoveFromCancelledList( serverJob );

                return pCompletedJob;

            }
        }
        SAPDBERR_ASSERT_STATE( false );
        return 0;
    }

    SAPDB_Bool MarkJobAsCancelled(const Data_UpdateCounterSrvTask& job)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobList::MarkJobAsCancelled",
            SQLMan_Trace, 5);

        if ( this->IsJobMarkedCancelled( job ) ) {

            return true;

        }

        return m_cancelledJobsList.InsertEnd( &job );

    }

    SAPDB_Bool IsJobMarkedCancelled(const Data_UpdateCounterSrvTask& job) const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobList::IsJobMarkedCancelled",
            SQLMan_Trace, 5);

        JobPointerList::ConstIterator it = m_cancelledJobsList.Begin();
        for (JobPointerList::ConstIterator itEnd = m_cancelledJobsList.End();
             it != itEnd;
             ++it ) {

            if ( (*it) == &job ) {

                return true;

            }

        }

        return false;
    }

private:
    void RemoveFromCancelledList(const Data_UpdateCounterSrvTask& job)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobList::RemoveFromCancelledList",
            SQLMan_Trace, 8);

        JobPointerList::Iterator it = m_cancelledJobsList.Begin();
        for (JobPointerList::Iterator itEnd = m_cancelledJobsList.End();
             it != itEnd;
             ++it ) {

            if ( (*it) == &job ) {

                m_cancelledJobsList.Delete( it );
                return;

            }

        }
    }
};

/*****************************************************************************/
/*****************************************************************************/

class JobPreparation
{
private:
    FileDir_FileNo m_curFileNo;
    SAPDB_Bool     m_advance;
    SAPDB_Bool     m_noMoreJobs;

    typedef SQLTab_SysUpdateCounterWanted::RequestIterator Iterator;

    Iterator m_requestIt;

public:
    enum ReturnCode {
        rcOk,
        rcNoMoreJobs,
        rcResourcesExhausted,
        rcAllTablesLocked,
        rcCancelled,
        rcSystemError
    };

    /****************************************************************/

    JobPreparation(SQLMan_Context& context)
        : m_advance( true ),
          m_noMoreJobs( false ),
          m_requestIt( context )
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobPreparation::JobPreparation",
            SQLMan_Trace, 5);
    }

    /****************************************************************/

    ReturnCode Initialize(SQLMan_Context& context, Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobPreparation::Initialize",
            SQLMan_Trace, 5);

        Iterator::ReturnCode rc = m_requestIt.Initialize( errMsg );

        if ( rc != Iterator::rcOk ) {
            switch ( rc ) {
                case Iterator::rcOutOfMemory:
                    return rcResourcesExhausted;
                case Iterator::rcCancelled:
                    return rcCancelled;
                default:
                    // errMsg should be already set
                    return rcSystemError;
            }
        }

        m_advance    = true;
        m_noMoreJobs = m_requestIt.IsEmpty();

        return rcOk;
    }

    /****************************************************************/

    ReturnCode PrepareJob(
        SQLMan_Context&       context,
        const SAPDB_Int       jobId,
        JobInfo&              jobInfo,
        const JobList&        runningJobsList,
        SrvTasks_WaitingTask& waitingTask,
        Msg_List&             errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobPreparation::PrepareJob",
            SQLMan_Trace, 5);

        if ( m_noMoreJobs ) {
            return rcNoMoreJobs;
        }

        if ( ! jobInfo.Initialize( errMsg ) ) {
            return rcResourcesExhausted;
        }

        // everything in here is in the internal trans context
        SQLMan_AutoSwitchToInternalTrans internalTrans(
            jobInfo.GetInternalTransInfo() );

        SAPDB_Bool secondRun         = false;
        SAPDB_Bool lockedTablesExist = false;

        do {

            if ( m_advance ) {

                const Iterator::ReturnCode rc = m_requestIt.Next(
                        m_curFileNo, errMsg );

                if ( ! m_curFileNo.IsValid() ) {

                    if ( rc == Iterator::rcOk  ) {

                        if ( secondRun ) {

                            if ( lockedTablesExist ) {

                                return rcAllTablesLocked;

                            } else {

                                m_noMoreJobs = true;
                                return rcNoMoreJobs;

                            }
                        }

                        if ( SQLMan_Trace.TracesLevel( 5 ) ) {
                            Kernel_VTrace()
                                << "SQLMan_UpdateCounter: scanning request table one more time";
                        }

                        // it is safer to reinitialize the iterator in
                        // original trans
                        jobInfo.GetInternalTransInfo().SwitchToParent();
                        m_requestIt.Reinit( errMsg );
                        jobInfo.GetInternalTransInfo().SwitchToInternal();

                        if ( m_requestIt.IsEmpty() ) {

                            jobInfo.SetFileNo( FileDir_FileNo() );
                            m_noMoreJobs = true;
                            return rcNoMoreJobs;

                        }

                        secondRun = true;
                        continue;

                    } else {

                        switch ( rc ) {
                            case Iterator::rcCancelled:
                                return rcCancelled;
                            case Iterator::rcOutOfMemory:
                                return rcResourcesExhausted;
                            default:
                                return rcSystemError;
                        }

                    }

                }

            }

            // make sure to advance in case we loop
            m_advance = true;

            if ( context.GetTransContext().Task().CancelPending() ) {

                return rcCancelled;

            }

            if ( this->IsJobRunning( m_curFileNo, runningJobsList ) ) {

                continue;

            }

            jobInfo.SetFileNo( m_curFileNo );

            const JobInfo::LockRequestRc lrRc =
                jobInfo.LockTable( context, errMsg );

            if ( lrRc != JobInfo::lrRcOk ) {

                switch ( lrRc ) {
                    case JobInfo::lrRcLockCollision:
                        if ( SQLMan_Trace.TracesLevel( 5 ) ) {
                            Kernel_VTrace()
                                << "SQLMan_UpdateCounter: lock collision for file "
                                << m_curFileNo;
                        }
                        lockedTablesExist = true;
                        break;
                    case JobInfo::lrRcFileNotFound:
                    {
                        if ( SQLMan_Trace.TracesLevel( 5 ) ) {
                            Kernel_VTrace()
                                << "SQLMan_UpdateCounter: deleting request for missing file "
                                << m_curFileNo;
                        }

                        Iterator::ReturnCode itRc =
                            m_requestIt.DeleteBrokenRow( errMsg );

                        if ( itRc != Iterator::rcOk ) {

                            switch ( itRc ) {
                                case Iterator::rcCancelled:
                                    return rcCancelled;
                                default:
                                    return rcSystemError;
                            }

                        }
                        break;
                    }
                    case JobInfo::lrRcCancelled:
                        return rcCancelled;
                    default:
                        return rcSystemError;
                }

                jobInfo.SetFileNo( FileDir_FileNo() );
                continue;
            }

        } while ( ! jobInfo.GetFileNo().IsValid() );

        SrvTasks_Job::Attributes jobAttributes( jobId );

        Data_UpdateCounterSrvTask* pSrvJob =
            new ( jobInfo.GetAllocator() )
            Data_UpdateCounterSrvTask(
                jobAttributes,
                jobInfo.GetFileNo(),
                waitingTask );

        if ( pSrvJob == 0 ) {

            errMsg = Msg_List(
                Msg_List::Warning,
                SDBMSG_SQLMAN_UPDCNT_OUT_OF_MEMORY_IGNORED,
                Msg_Arg(SDBMSGTAG_SQLMAN_UPDCNT_OUT_OF_MEMORY_IGNORED__ALLOC_SIZE,
                        sizeof( Data_UpdateCounterSrvTask ) ) );

            jobInfo.SetJobDone( JobInfo::jobFailed, errMsg );

            return rcResourcesExhausted;
        }

        jobInfo.SetJob( *pSrvJob );

        m_advance = false;

        return rcOk;
    }

    /****************************************************************/

    SAPDB_Bool AreJobsAvailable() const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobPreparation::AreJobsAvailable",
            SQLMan_Trace, 8);

        return ! m_noMoreJobs;
    }

    /****************************************************************/

    SAPDB_Bool IsJobRunning(
        const FileDir_FileNo& jobFileNo,
        const JobList&        runningJobs) const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobPreparation::IsJobRunning",
            SQLMan_Trace, 8);

        JobList::ConstIterator it = runningJobs.Begin();

        for ( JobList::ConstIterator itEnd = runningJobs.End();
              it != itEnd;
              ++it ) {

            if ( (*it)->GetFileNo() == jobFileNo ) {
                return true;
            }

        }

        return false;
    }

    /****************************************************************/

    void NextJob()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobPreparation::NextJob",
            SQLMan_Trace, 5);

        m_advance = true;
    }
};

/*****************************************************************************/
/*****************************************************************************/

class ServerHandling
{
private:
    SAPDB_UInt           m_usableServerCount;
    RTETask_ITask&       m_task;
    SrvTasks_WaitingTask m_waitingTask;
    Srv_StatJobList      m_serverJobList;

    // AIX doesn't like consts in classes and throws undefined symbols
    enum {
        cMinServerTasks = 1,
        cMaxRetries     = 5
    };

public:
    ServerHandling(RTETask_ITask& task, SAPDBMem_IRawAllocator& alloc)
        : m_task( task ),
          m_waitingTask( task ),
          m_serverJobList( alloc ),
          m_usableServerCount( bd01UsableServerForParallelExecution() )
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::ServerHandling",
            SQLMan_Trace, 5);

        SAPDBERR_ASSERT_STATE(
            (m_usableServerCount > 0)
            && (m_usableServerCount
                <= SrvTasks_ICoordinator::Instance().GetMaxServerTasks()) );

        if ( (m_usableServerCount == 0)
             ||
             (m_usableServerCount
              > SrvTasks_ICoordinator::Instance().GetMaxServerTasks()) ) {
            m_usableServerCount = 1;
        }
    }

    /****************************************************************/

    ~ServerHandling()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::~ServerHandling",
            SQLMan_Trace, 5);

        if ( this->AreJobsRunning() ) {
            this->AbortAll();
        }
    }

    /****************************************************************/

    SAPDB_Bool Initialize(Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::Initialize",
            SQLMan_Trace, 5);

        if ( ! m_serverJobList.Reserve( m_usableServerCount ) ) {

            if ( m_serverJobList.Reserve( cMinServerTasks ) ) {

                m_usableServerCount = cMinServerTasks;

            } else {

                return false;

            }
        }

        return true;
    }

    /****************************************************************/

    SrvTasks_WaitingTask& GetWaitingTask()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::GetWaitingTask",
            SQLMan_Trace, 8);

        return m_waitingTask;
    }

    /****************************************************************/

    enum ExecuteRc {
        exeRcOk,
        exeRcNoMoreServers,
        exeRcBusy,
        exeRcSystemError
    };

    /****************************************************************/

    ExecuteRc ExecuteJob(Data_UpdateCounterSrvTask& job, Msg_List& errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::ExecuteJob",
            SQLMan_Trace, 5);
        SAPDBERR_ASSERT_STATE( m_usableServerCount > 0 );

        if ( m_usableServerCount == 0 ) {
            return exeRcSystemError;
        }

        if ( m_serverJobList.GetSize() >= m_usableServerCount ) {
            if ( SQLMan_Trace.TracesLevel( 6 ) ) {
                Kernel_VTrace()
                    << "SQLMan_UpdateCounter: all servers busy; max. usable: "
                    << m_usableServerCount
                    << "; in use: " << m_serverJobList.GetSize();
            }
            return exeRcNoMoreServers;
        }

        const SAPDB_Bool bIgnoreMyself = true;
        const SAPDB_ULong timeSinceLastCommand =
            TimeElapsedSinceLastCommandExecution( bIgnoreMyself );
        if ( SQLMan_Trace.TracesLevel( 6 ) ) {
            Kernel_VTrace()
                << "SQLMan_UpdateCounter: time since last user command: "
                << timeSinceLastCommand;
        }
        if ( timeSinceLastCommand <= cMinIdleTime ) {
            return exeRcBusy;
        }

        SAPDB_UInt retryCounter = 0;
        SrvTasks_Job::ExecuteByServerResult retCode;
        do {
            retCode = job.ExecuteByServer( m_task, errMsg );

            if ( retCode == SrvTasks_Job::executeOK ) {

                if ( SQLMan_Trace.TracesLevel( 5 ) ) {
                    Kernel_VTrace()
                        << "SQLMan_UpdateCounter: started job for file "
                        << job.GetFileNo();
                }

                m_serverJobList.InsertEnd( &job );
                retryCounter = 0;

            } else if ( retCode == SrvTasks_Job::executeNoServerAvailable ) {

                errMsg.ClearMessageList();
                if ( retryCounter <= cMaxRetries ) {

                    if ( SQLMan_Trace.TracesLevel( 5 ) ) {
                        Kernel_VTrace()
                            << "SQLMan_UpdateCounter: no servers available, will retry, try count "
                            << retryCounter;
                    }

                    ++retryCounter;
                    vsleep( m_task.ID(), 1 );

                } else {

                    return exeRcNoMoreServers;
                }
            } else {
                return exeRcSystemError;
            }
        } while ( retCode != SrvTasks_Job::executeOK );

        return exeRcOk;
    }

    /****************************************************************/

    SAPDB_Bool AbortJob(
        RTETask_ITask&             task,
        Data_UpdateCounterSrvTask& job,
        Msg_List&                  errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::AbortJob",
            SQLMan_Trace, 5);

        if ( SQLMan_Trace.TracesLevel( 5 ) ) {
            Kernel_VTrace()
                << "SQLMan_UpdateCounter sending abort signal to job for file "
                << job.GetFileNo();
        }

        job.Abort( task );
        return true;
    }

    /****************************************************************/

    SAPDB_Bool AreJobsRunning() const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::AreJobsRunning",
            SQLMan_Trace, 8);

        return m_serverJobList.GetSize() > 0;
    }

    /****************************************************************/

    SAPDB_Bool AreServersAvailable() const
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::AreServersAvailable",
            SQLMan_Trace, 8);

        return m_serverJobList.GetSize() < m_usableServerCount;
    }

    /****************************************************************/

    SAPDB_Bool ExistFinishedJobs()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::ExistFinishedJobs",
            SQLMan_Trace, 5);

        Srv_StatJobList::Iterator dummyJobIt;
        SAPDB_UInt                dummyJobCount;

        return m_serverJobList.SearchForFinishedJob(
            m_waitingTask,
            dummyJobIt,
            dummyJobCount );
    }

    /****************************************************************/

    enum WaitRc {
        wtRcJobFinished,
        wtRcNoJobsRunning,
        wtRcSystemError
    };

    /*!
     * returns the first job that has finished
     * returns immediately if no jobs are running
     */
    WaitRc WaitForAnyJob(
        Data_UpdateCounterSrvTask*& pFinishedJob,
        Msg_List&                   errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::WaitForAnyJob",
            SQLMan_Trace, 5);

        pFinishedJob = 0;

        if ( ! this->AreJobsRunning() ) {
            return wtRcNoJobsRunning;
        }

        Srv_StatJobList::Iterator jobIt;

        if ( m_serverJobList.WaitForAnyJobFinished( m_waitingTask, jobIt ) ) {

            pFinishedJob = *jobIt;

            m_serverJobList.Delete( jobIt );

        } else {

            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDCNT_SYSERR_WAITFORJOB );

            return wtRcSystemError;
        }

        return wtRcJobFinished;
    }

    /****************************************************************/

    void AbortAll()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::ServerHandling::AbortAll",
            SQLMan_Trace, 5);

        if ( SQLMan_Trace.TracesLevel( 5 ) ) {
            Kernel_VTrace()
                << "SQLMan_UpdateCounter: aborting all jobs";
        }

        if ( m_serverJobList.GetSize() > 0 ) {

            m_serverJobList.AbortAll( m_waitingTask );

            Srv_StatJobList::Iterator jobIt;

            while ( m_serverJobList.WaitForAnyJobFinished(
                        m_waitingTask, jobIt ) ) {

                m_serverJobList.Delete( jobIt );

            }

        }

        m_serverJobList.Clear(); // free memory
    }
};

/*****************************************************************************/
/*****************************************************************************/

class JobStarter
{
private:
    JobPreparation& m_jobPreparer;
    ServerHandling& m_serverHandling;

    JobInfo*                m_pCurJob;
    SAPDB_UInt              m_jobId;
    SAPDBMem_IRawAllocator& m_alloc;

public:
    JobStarter(
        SAPDBMem_IRawAllocator& alloc,
        JobPreparation&         jobPreparer,
        ServerHandling&         serverHandling)
        : m_alloc( alloc ),
          m_jobPreparer( jobPreparer ),
          m_serverHandling( serverHandling ),
          m_pCurJob( 0 ),
          m_jobId( 0 )
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobStarter::JobStarter",
            SQLMan_Trace, 5);
    }

    /****************************************************************/

    ~JobStarter()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobStarter::~JobStarter",
            SQLMan_Trace, 5);

        SAPDBERR_ASSERT_STATE( m_pCurJob == 0 );
    }

    /****************************************************************/

    enum StartRc {
        startRcOk,
        startRcCancelled,
        startRcOutOfMemory,
        startRcSystemError,
        startRcGoToSleep
    };

    /*!
     * @brief Starts as many jobs as possible.
     */
    StartRc StartJobs(
        SQLMan_Context& context,
        JobList&        jobList,
        SAPDB_Bool&     throwSystemError,
        Msg_List&       errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "SQLMan_UpdateCounter::JobStarter::StartJobs",
            SQLMan_Trace, 5);

        ServerHandling::ExecuteRc executeRc = ServerHandling::exeRcOk;
        JobPreparation::ReturnCode prepRc = JobPreparation::rcNoMoreJobs;

        do {

            if ( context.GetTransContext().Task().CancelPending() ) {
                return startRcCancelled;
            }

            if ( (m_pCurJob == 0)
                 && m_jobPreparer.AreJobsAvailable()
                 && (! throwSystemError) ) {

                m_pCurJob = new ( m_alloc ) JobInfo( context );

                if ( m_pCurJob == 0 ) {

                    errMsg = Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_NO_MEMORY,
                        Msg_OptArg(
                            "MEMREQUEST",
                            sizeof( JobInfo )) );

                    return startRcOutOfMemory;

                }

                prepRc = m_jobPreparer.PrepareJob(
                    context,
                    m_jobId++,
                    *m_pCurJob,
                    jobList,
                    m_serverHandling.GetWaitingTask(),
                    errMsg );

                switch ( prepRc ) {
                    case JobPreparation::rcOk:
                        break;
                    case JobPreparation::rcNoMoreJobs:
                        throwSystemError = ! m_pCurJob->Finalize( errMsg );
                        this->CleanupJob();
                        break;
                    case JobPreparation::rcCancelled:
                        return startRcCancelled;
                    case JobPreparation::rcResourcesExhausted:
                    case JobPreparation::rcAllTablesLocked:
                        throwSystemError = ! m_pCurJob->Finalize( errMsg );
                        this->CleanupJob();

                        if ( ! m_serverHandling.AreJobsRunning() ) {

                            return startRcGoToSleep;

                        }
                        break;
                    default:
                        errMsg.Overrule(
                            Msg_List(
                                Msg_List::Error,
                                SDBMSG_SQLMAN_UPDCNT_PREPARE_FAILED,
                                Msg_Arg(SDBMSGTAG_SQLMAN_UPDCNT_PREPARE_FAILED__FILENO,
                                        FileDir_FileNoToString(
                                            m_pCurJob->GetFileNo() ) )
                                ) );

                        throwSystemError = ! m_pCurJob->Finalize( errMsg );
                        this->CleanupJob();

                        throwSystemError = true;
                }
            }

            if ( m_pCurJob != 0 ) {
                executeRc = m_serverHandling.ExecuteJob(
                    m_pCurJob->GetJob(), errMsg );

                if ( executeRc == ServerHandling::exeRcOk ) {

                    jobList.InsertEnd( m_pCurJob );
                    m_jobPreparer.NextJob();

                    m_pCurJob = 0;

                } else {

                    throwSystemError =
                        ( executeRc == ServerHandling::exeRcSystemError );

                }

            }
        } while ( (m_pCurJob == 0)
                  && (prepRc == JobPreparation::rcOk)
                  && (executeRc == ServerHandling::exeRcOk)
                  && (! throwSystemError) );


        if ( (m_pCurJob != 0)
             && ((executeRc == ServerHandling::exeRcBusy)
                 ||
                 (executeRc == ServerHandling::exeRcNoMoreServers))
             && (! m_serverHandling.ExistFinishedJobs()) ) {

            // release lock on table if it is likely that we will go to sleep
            m_pCurJob->SetJobDone( JobInfo::jobFailed, errMsg );
            throwSystemError = ! m_pCurJob->Finalize( errMsg );
            destroy( m_pCurJob, m_alloc );
        }

        if ( throwSystemError ) {
            return startRcSystemError;
        }

        return startRcOk;
    }

    /*********************************************************************/

    void CleanupJob()
    {
        if ( m_pCurJob != 0 ) {

            Msg_List dummyMsg;
            m_pCurJob->Finalize( dummyMsg );
            destroy( m_pCurJob, m_alloc );

            dummyMsg.ClearMessageList();
        }
    }

    /*********************************************************************/

    void Cleanup()
    {
        CleanupJob();
    }
};

} // end anonymous namespace

/*****************************************************************************/
/*****************************************************************************/

SQLMan_UpdateCounter::SQLMan_UpdateCounter(SQLMan_Context& context)
    :
    m_context( context ),
    m_isInitialized( false )
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::SQLMan_UpdateCounter", SQLMan_Trace, 5);
}

/*****************************************************************************/

SQLMan_UpdateCounter::~SQLMan_UpdateCounter()
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::~SQLMan_UpdateCounter", SQLMan_Trace, 5);

    if ( m_isInitialized ) {
        RTESys_AtomicWrite( m_isRunning, cUpdCntNotRunning );
    }
}

/*****************************************************************************/

SAPDB_Bool
SQLMan_UpdateCounter::Initialize(SQLMan_Context& context)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::Initialize", SQLMan_Trace, 5);

    SAPDB_Int dummyOld;
    if ( RTESys_CompareAndExchange(
             m_isRunning, cUpdCntNotRunning, cUpdCntIsRunning, dummyOld ) ) {

        m_isInitialized = true;

    } else {

        context.ThrowError( e_dbm_command_not_possible );

    }

    return m_isInitialized;
}

/*****************************************************************************/

void SQLMan_UpdateCounter::Execute(SQLMan_Context& context)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::Execute", SQLMan_Trace, 5);

    SAPDBERR_ASSERT_STATE( m_isInitialized );

    if ( (! context.IsSysdbaUser())
         && (! context.IsControlUser()) ) {
        context.ThrowError( e_missing_privilege );

        return;
    }

    RTETask_ITask&             task =
        *RTETask_ITask::ByTaskID( m_context.GetTransContext().TaskId() );

    SQLTab_SysUpdateCounterWanted requestTable( context );
    JobList                    jobList( context.GetAllocator() );
    JobPreparation             jobPreparer( context );
    ServerHandling             serverHandling( task, context.GetAllocator() );
    Msg_List                   errMsg; // = m_context.GetErrorList();

    JobPreparation::ReturnCode prepRc =
        jobPreparer.Initialize( context, errMsg );

    if ( prepRc != JobPreparation::rcOk ) {

        switch ( prepRc ) {
            case JobPreparation::rcResourcesExhausted:
                context.ThrowError( e_no_more_memory );
                break;
            case JobPreparation::rcCancelled:
                context.ThrowError( e_cancelled );
                break;
            default:
                context.ThrowError( e_system_error );
        }

        context.GetErrorList() = errMsg;
        return;
    }

    if ( jobPreparer.AreJobsAvailable() ) {

        errMsg = Msg_List( Msg_List::Info, SDBMSG_SQLMAN_UPDCNT_STARTED );
        RTE_Message( errMsg );
        errMsg.ClearMessageList();

    } else {

        errMsg = Msg_List( Msg_List::Info, SDBMSG_SQLMAN_UPDCNT_NOTHING_TO_DO );
        RTE_Message( errMsg );
        errMsg.ClearMessageList();

        return;
    }

    // wait for some time before commencing
    vsleep( task.ID(), cSleepBeforeStartingTime );

    JobStarter jobStarter(
        context.GetAllocator(),
        jobPreparer,
        serverHandling );

    // throwing a system error in the context while doing internal SQL
    // is a bad idea, therefore only set it in the context once we are
    // done
    m_throwSystemError = false;
    m_isCancelled      = false;
    JobInfo*   curJob  = 0;
    SAPDB_UInt jobId   = 0;
    do {

        const JobStarter::StartRc startRc =
            jobStarter.StartJobs( context, jobList, m_throwSystemError, errMsg );

        if ( startRc != JobStarter::startRcOk ) {

            if ( SQLMan_Trace.TracesLevel( 6 ) ) {
                Kernel_VTrace()
                    << "SQLMan_UpdateCounter: starting job failed; reason: "
                    << startRc;
            }

            switch ( startRc ) {
                case JobStarter::startRcGoToSleep:
                    vsleep( task.ID(), cSleepWhenTablesLockedTime );
                    break;
                case JobStarter::startRcCancelled:
                    m_isCancelled = true;
                    break;
                case JobStarter::startRcOutOfMemory:
                    context.ThrowError( e_no_more_memory );
                    m_throwSystemError = true;
                default:
                    errMsg.Overrule(
                        Msg_List(
                            Msg_List::Error,
                            SDBMSG_SQLMAN_UPDCNT_START_JOB_FAILED
                            ) );
                    m_throwSystemError = true;
            }
        }

        if ( m_throwSystemError ) {
            // do not try and wait for other jobs finishing
            break;
        }

        if ( m_isCancelled
             ||
             context.GetTransContext().Task().CancelPending() ) {

            m_isCancelled = true;
            break;

        }

        // we cannot simply wait for finished jobs, because we need to
        // be able to abort a running statistics collection if a user
        // wants to work with that table;
        // therefore we have to do polling
        if ( serverHandling.ExistFinishedJobs() ) {

            ServerHandling::WaitRc     waitRc;
            Data_UpdateCounterSrvTask* pFinishedSrvJob;

            if ( SQLMan_Trace.TracesLevel( 6 ) ) {
                Kernel_VTrace()
                    << "SQLMan_UpdateCounter: waiting for finished job";
            }

            waitRc = serverHandling.WaitForAnyJob( pFinishedSrvJob, errMsg );

            if ( waitRc == ServerHandling::wtRcJobFinished ) {

                this->ProcessFinishedJob(
                    context, requestTable, jobList, *pFinishedSrvJob, errMsg);

            } else {

                m_throwSystemError = true;

            }

        } else if ( ! m_throwSystemError && ! m_isCancelled ) {

            if ( startRc == JobStarter::startRcGoToSleep ) {

                // we have already been sleeping, start more jobs first
                continue;

            }

            if ( ! serverHandling.AreJobsRunning() ) {

                // kernel is busy
                vsleep( task.ID(), cSleepWhenBusyTime );

            } else {

                this->FreeTablesRequestedByUsers(
                    m_context, jobList, serverHandling, errMsg );

                // do not sleep as long to be able to collect finished
                // tasks quickly
                vsleep( task.ID(), cSleepWhenBusyButJobsRunningTime );
            }

        }

    } while ( ( serverHandling.AreJobsRunning()
                || jobPreparer.AreJobsAvailable() )
              && (! m_throwSystemError)
              && (! m_isCancelled) );

    if ( m_isCancelled || m_throwSystemError ) {

        serverHandling.AbortAll();

        jobStarter.Cleanup(); // necessary to clean up internal transactions

        Msg_List dummyMsg;
        jobList.Clear( dummyMsg ); // necessary to clean up internal transactions

    }

    if ( m_throwSystemError || m_isCancelled ) {

        if ( m_isCancelled ) {

            errMsg = Msg_List( Msg_List::Info, SDBMSG_SQLMAN_UPDCNT_CANCELLED );
            RTE_Message( errMsg );
            errMsg.ClearMessageList();

            context.ThrowError( e_cancelled );
            return;

        } else {

            errMsg.Overrule(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_SQLMAN_UPDCNT_SYSTEM_ERROR
                    ) );

            if ( context.IsOk() ) {

                context.ThrowError( e_system_error );
            }

            m_context.GetErrorList() = errMsg;
            return;

        }
    }

    errMsg = Msg_List( Msg_List::Info, SDBMSG_SQLMAN_UPDCNT_FINISHED );
    RTE_Message( errMsg );
    errMsg.ClearMessageList();
}

/*****************************************************************************/

void
SQLMan_UpdateCounter::ProcessFinishedJob(
    SQLMan_Context&                context,
    SQLTab_SysUpdateCounterWanted& requestTable,
    JobList&                       jobList,
    Data_UpdateCounterSrvTask&     finishedSrvJob,
    Msg_List&                      errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::ProcessFinishedJob", SQLMan_Trace, 5);

    Msg_List savedMsg;

    const SAPDB_Bool wasJobCancelled =
        jobList.IsJobMarkedCancelled( finishedSrvJob );

    JobInfo* pCompletedJob = jobList.RemoveServerJob( finishedSrvJob );

    SAPDBERR_ASSERT_STATE( pCompletedJob != 0 );

    const Data_UpdateCounterSrvTask::ReturnCode jobRc =
        pCompletedJob->GetJob().GetReturnCode( errMsg );

    if ( SQLMan_Trace.TracesLevel( 6 ) ) {
        Kernel_VTrace()
            << "SQLMan_UpdateCounter:  job has finished for file "
            << pCompletedJob->GetJob().GetFileNo();
    }

    if ( (jobRc == Data_UpdateCounterSrvTask::rcOk)
         ||
         (jobRc == Data_UpdateCounterSrvTask::rcFileNotFound)
         ||
         (jobRc == Data_UpdateCounterSrvTask::rcFileBad) ) {

        SQLMan_AutoSwitchToInternalTrans internalTrans(
            pCompletedJob->GetInternalTransInfo() );

        const WriteLogEntryRc logRc =
            this->WriteLogEntry( *pCompletedJob, errMsg );
        if ( logRc != wleRcOk ) {
            switch ( logRc ) {
                case wleRcCancelled:
                    m_isCancelled = true;
                    break;
                default:
                    m_throwSystemError = true;
            }
        }

        if ( logRc == wleRcOk ) {
            SQLTab_SysUpdateCounterWanted::DeleteRc delRc =
                requestTable.DeleteRow( pCompletedJob->GetFileNo(), errMsg );

            if ( delRc != SQLTab_SysUpdateCounterWanted::delRcOk ) {

                switch ( delRc ) {
                    case SQLTab_SysUpdateCounterWanted::delRcRowNotFound:
                    case SQLTab_SysUpdateCounterWanted::delRcLockRequestTimeout:
                        errMsg.ClearMessageList();
                        break;
                    case SQLTab_SysUpdateCounterWanted::delRcCancelled:
                        m_isCancelled = true;
                        break;
                    default:
                        m_throwSystemError = true;
                }
            }
        }

        if ( (! m_throwSystemError) && (! m_isCancelled) ) {
            pCompletedJob->SetJobDone( JobInfo::jobSuccessful, errMsg );
        } else {
            // we do not know whether the log entry was written
            // successfully if the job was cancelled
            savedMsg = errMsg;
            pCompletedJob->SetJobDone( JobInfo::jobFailed, errMsg );
            errMsg.MarkAsSubMessages();
            errMsg.Overrule( savedMsg );
            savedMsg = errMsg;
        }
    } else {

        switch ( jobRc ) {
            case Data_UpdateCounterSrvTask::rcCancelled:
            {
                // a cancelled job will be requeued later on,
                // no need for us to stop processing

                if ( ! wasJobCancelled ) {

                    errMsg = Msg_List(
                        Msg_List::Info,
                        SDBMSG_SQLMAN_UPDCNT_CHECK_TABLE_CANCELLED,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDCNT_CHECK_TABLE_CANCELLED__FILENO,
                                FileDir_FileNoToString( pCompletedJob->GetFileNo() )
                            ) );
                    RTE_Message( errMsg );
                    errMsg.ClearMessageList();

                }

                this->IncrementJobTryCounter(
                    *pCompletedJob,
                    requestTable,
                    errMsg );

                break;
            }
            default:
                m_throwSystemError = true;
        }

        savedMsg = errMsg;
        pCompletedJob->SetJobDone( JobInfo::jobFailed, errMsg );

        errMsg.Overrule( savedMsg );

    }

    pCompletedJob->Finalize( errMsg );

    errMsg.Overrule( savedMsg );

    destroy( pCompletedJob, context.GetAllocator() );
}

/*****************************************************************************/

SQLMan_UpdateCounter::FreeRequestedTablesRc
SQLMan_UpdateCounter::FreeTablesRequestedByUsers(
    SQLMan_Context& context,
    JobList&        jobList,
    ServerHandling& serverHandling,
    Msg_List&       errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::FreeTablesRequestedByUsers", SQLMan_Trace, 5);

    tgg00_TransContext& trans = context.GetTransContext().OldTrans();

    SAPDBERR_ASSERT_STATE( trans.trError_gg00 == e_ok );

    tgg00_Surrogate tabSurr;
    JobList::Iterator it = jobList.Begin();
    for ( ; it != jobList.End(); ++it ) {

        tabSurr.rawAssign( (*it)->GetTableFileNo().GetCharArray() );

        if ( jobList.IsJobMarkedCancelled( (*it)->GetJob() ) ) {
            continue;
        }

        const SAPDB_Bool isTableRequestedByUser =
            kb51ExclusiveRequestExists(
                context.GetTransContext().TaskId(), tabSurr )
            ? true : false;

        if ( trans.trError_gg00 != e_ok ) {

            return frtRcSystemError;

        }

        if ( isTableRequestedByUser ) {

            if ( SQLMan_Trace.TracesLevel( 5 ) ) {
                Kernel_VTrace()
                    << "SQLMan_UpdateCounter: table requested by user "
                    << (*it)->GetTableFileNo()
                    << ", aborting job for file "
                    << (*it)->GetFileNo();
            }

            RTETask_ITask& task =
                *RTETask_ITask::ByTaskID(
                    m_context.GetTransContext().TaskId() );

            const SAPDB_Bool abortOk =
                serverHandling.AbortJob(
                    task,
                    (*it)->GetJob(),
                    errMsg );

            if ( ! abortOk ) {

                return frtRcSystemError;

            }

            // this prevents an info message from appearing in knldiag
            // once the job finishes
            jobList.MarkJobAsCancelled( (*it)->GetJob() );// errors are ignored

        }

    }

    return frtRcOk;
}

/*****************************************************************************/

void
SQLMan_UpdateCounter::IncrementJobTryCounter(
    JobInfo&                       job,
    SQLTab_SysUpdateCounterWanted& requestTable,
    Msg_List&                      errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::IncrementeJobTryCounter", SQLMan_Trace, 5);

    SQLMan_AutoSwitchToInternalTrans internalTrans(
        job.GetInternalTransInfo() );

    SQLTab_SysUpdateCounterWanted::IncrementRc incRc =
        requestTable.IncrementUpdateTryCount(
            job.GetFileNo(), errMsg );

    if ( incRc != SQLTab_SysUpdateCounterWanted::incRcOk ) {

        switch ( incRc ) {
            case SQLTab_SysUpdateCounterWanted::incRcRowNotFound:
            case SQLTab_SysUpdateCounterWanted::incRcLockRequestTimeout:
                errMsg.ClearMessageList();
                break;
            case SQLTab_SysUpdateCounterWanted::incRcCancelled:
                m_isCancelled = true;
                break;
            default:
                m_throwSystemError = true;
        }

    }

    if ( ! m_throwSystemError ) {

        const SAPDB_Bool isOk =
            job.CommitInternalTrans( errMsg );

        if ( ! isOk ) {

            m_throwSystemError = true;

        }

    }
}

/*****************************************************************************/

SQLMan_UpdateCounter::WriteLogEntryRc
SQLMan_UpdateCounter::WriteLogEntry(
    JobInfo&       job,
    Msg_List&      errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::WriteLogEntry", SQLMan_Trace, 5);

    FileDir_Generic fileEntry;

    WriteLogEntryRc logRc = this->WriteSingleLogEntry(
        job.GetFileNo(),
        fileEntry,
        errMsg );

    if ( ( logRc == wleRcOk )
         && ( fileEntry.GetFileType() == Data_TableFile ) ) {
        // short column file counters have been updated implicitly, we
        // need a log entry for those, too
        FileDir_Table tableEntry;
        tableEntry.Assign( fileEntry );

        if ( tableEntry.HasShortColumnFile() ) {

            logRc = this->WriteSingleLogEntry(
                tableEntry.GetShortColumnFileNo(),
                fileEntry,
                errMsg );
        }
    }

    return logRc;
}

/*****************************************************************************/

SQLMan_UpdateCounter::WriteLogEntryRc
SQLMan_UpdateCounter::WriteSingleLogEntry(
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      fileEntry,
    Msg_List&             errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateCounter::WriteLogSingleEntry", SQLMan_Trace, 5);

    FileDir_IDirectory& fDir = FileDir_IDirectory::SessionInstance(
        m_context.GetTransContext() );

    const RTE_TaskId taskId = m_context.GetTransContext().TaskId();

    FileDir_ReturnCode fDirRc = fDir.GetFile(
        taskId,
        fileNo,
        fileEntry,
        errMsg );

    if ( fDirRc != FileDir_Okay ) {
        errMsg.Overrule(
            Msg_List( Msg_List::Error,
                      SDBMSG_SQLMAN_UPDCNT_FILE_NOT_FOUND,
                      Msg_Arg( SDBMSGTAG_SQLMAN_UPDCNT_FILE_NOT_FOUND__FILENO,
                               FileDir_FileNoToString( fileNo ) ),
                      Msg_Arg( SDBMSGTAG_SQLMAN_UPDCNT_FILE_NOT_FOUND__ERRORTEXT,
                               (SAPDB_ToString) (fDirRc) ) ) );
        return wleRcSystemError;
    }

    if ( gg06IsNilTrans( m_context.TransContext().trWriteTransId_gg00 ) ) {
        k50new_write_trans( m_context.TransContext() );
    }

    if ( fileEntry.GetFileType() != Data_TableFile ) {
        Log_ActionSetFileDirCounter action(
            fileEntry.GetFileNo(),
            fileEntry.GetIndexPageCount( taskId ),
            fileEntry.GetLeafPageCount( taskId ),
            fileEntry.GetEntryCount( taskId ),
            0 );

        // no need to execute log action, values have already been set
        // by server task
        action.WriteAfterImage( m_context.GetTransContext().OldTrans() );
    } else {
        FileDir_Table tableEntry;
        tableEntry.Assign( fileEntry );

        Log_ActionSetFileDirCounter action(
            tableEntry.GetFileNo(),
            tableEntry.GetIndexPageCount( taskId ),
            tableEntry.GetLeafPageCount( taskId ),
            tableEntry.GetEntryCount( taskId ),
            tableEntry.GetLobPageCount() );

        // no need to execute log action, values have already been set
        // by server task
        action.WriteAfterImage( m_context.TransContext() );
    }

    if ( m_context.TransContext().trError_gg00 != e_ok ) {

        switch ( m_context.TransContext().trError_gg00 ) {
            case e_cancelled:
                return wleRcCancelled;
            default:
                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDCNT_LOG_ERROR,
                        Msg_Arg( SDBMSGTAG_SQLMAN_UPDCNT_LOG_ERROR__BASIS_ERROR,
                                 SQLManErr_Interface::GetInstance()
                                    .GetBaseErrorText(
                                        m_context.TransContext().trError_gg00 )),
                        Msg_Arg( SDBMSGTAG_SQLMAN_UPDCNT_LOG_ERROR__FILENO,
                                 FileDir_FileNoToString( fileEntry.GetFileNo() )
                            ) ) );
                return wleRcSystemError;
        }
    }

    return wleRcOk;
}

/*****************************************************************************/
