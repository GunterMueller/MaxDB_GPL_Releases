/*****************************************************************************/
/*!
  @file         Data_PrimTreeStatisticsCoordinator.cpp
  @author       MartinKi

  @brief        Implementation file for class
                Data_PrimTreeStatisticsCoordinator.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SQLManager/SQLMan_UpdateStatistics.hpp"
#include "SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp"

#include "DataAccess/Data_PrimTreeStatisticsCoordinator.hpp"
#include "DataAccess/Data_UpdStatRequestFile.hpp"
#include "DataAccess/Data_Types.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Data.h"

#include "ServerTasks/SrvTasks_JobCollectTableStatistics.hpp"
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "Container/Container_Vector.hpp"

// Content of include files
#include "hbd998.h"
#include "hbd01.h"  // PASCAL: Configuration_Parameter
#include "hbd50.h"  // bd50DetermineKeyRangeForServer
#include "hbd30.h"  // bd30GetTree
#include "gbd00.h"  // tbd_current_tree

#include "Trace/Trace_Entry.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill

#include "Messages/Msg_List.hpp"

#include "hgg04.h" // g04build_temp_tree_id
#include "heo52.h" // vtimestamp
#include "heo56.h" // vsleep
#include "KernelCommon/Kernel_VTrace.hpp"

#include "RunTime/System/RTESys_Time.h"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/SAPDB_AutoPtr.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef SrvTasks_JobList<SrvTasks_JobCollectTableStatistics> Srv_StatJobList;

/*========================================================================*
 *  EXTERNAL VARIABLES                                                    *
 *========================================================================*/
/*========================================================================*
 *  GLOBAL VARIABLES                                                      *
 *========================================================================*/
/*========================================================================*
 *  LOCAL VARIABLES                                                       *
 *========================================================================*/

namespace Data_PrimTree_StatisticsCoordinator {

class JobQueue;
class JobInfo;
class TableInfo;

// TODO: one way to reduce allocations would be to use job pool
// instead of always creating/destroying individual jobs

class JobInfo {
private:
    SAPDBMem_IRawAllocator&             m_allocator;
    SrvTasks_JobCollectTableStatistics* m_job;
    TableInfo&                          m_tableInfo;
    SAPDB_Bool                          m_isRunning;
    Data_PrimTreeStatisticsCoordinator::DistinctValueVector* m_pDistinctVals;

    // no copy-construction
    JobInfo(const JobInfo&);
    JobInfo& operator=(const JobInfo&);
public:

    JobInfo(
        SAPDBMem_IRawAllocator& allocator,
        TableInfo&              tableInfo)
        : m_allocator( allocator ),
          m_job( 0 ),
          m_tableInfo( tableInfo ),
          m_isRunning( false ),
          m_pDistinctVals( 0 )
    {}

    /****************************************************************/

    ~JobInfo()
    {
        if ( m_pDistinctVals != 0 ) {
            destroy( m_pDistinctVals, m_allocator );
        }

        if ( m_job != 0 ) {
            this->DestroyJob();
        }
    }

    /****************************************************************/

    SAPDBMem_IRawAllocator& GetAllocator()
    {
        return m_allocator;
    }

    /****************************************************************/

    SAPDB_Bool IsValid() const
    {
        return
            (m_job != 0);
    }

    /****************************************************************/

    Data_PrimTreeStatisticsCoordinator::DistinctValueVector*
    AllocateDistinctValBuffer()
    {
        m_pDistinctVals = new ( m_allocator )
            Data_PrimTreeStatisticsCoordinator::DistinctValueVector(
                m_allocator );

        return m_pDistinctVals;
    }

    /****************************************************************/

    TableInfo& GetTableInfo() const
    {
        return m_tableInfo;
    }

    /****************************************************************/

    void SetJob(SrvTasks_JobCollectTableStatistics& job)
    {
        m_job = &job;
    }

    /****************************************************************/

    SrvTasks_JobCollectTableStatistics& GetJob()
    {
        return *m_job;
    }

    /****************************************************************/

    void DestroyJob()
    {
        destroy( m_job, m_allocator );
        m_job = 0;
    }

    /****************************************************************/

    SAPDB_Int4 GetJobId()
    {
        return m_job->GetId();
    }

    /****************************************************************/

    void SetIsRunning(SAPDB_Bool isRunning)
    {
        m_isRunning = isRunning;
    }

    /****************************************************************/

    SAPDB_Bool IsRunning() const
    {
        return m_isRunning;
    }

    /****************************************************************/
};

/*************************************************************************/

class TableInfo {
private:
    typedef Data_PrimTreeStatisticsCoordinator::StackCodeVector StackCodeVector;
    typedef Data_PrimTreeStatisticsCoordinator::ColumnNumberVector ColumnNumberVector;
    typedef Data_PrimTreeStatisticsCoordinator::DistinctValueVector DistinctValueVector;

    Trans_Context&                    m_trans;
    Data_UpdStatRequestFile::Iterator m_iterator;
    FileDir_FileNo                    m_requestFileNo;
    FileDir_FileNo                    m_auxFileNo;
    DistinctValueVector               m_distinctVals;
    SAPDB_UInt                        m_jobsRunningCount;
    SAPDB_Bool                        m_lastJobInProgress;
    ColumnNumberVector                m_columnNumbers;
    StackCodeVector                   m_stackCode;
    Catalog_TableStatisticsDescriptor m_statisticsDesc;
    SAPDB_Bool                        m_abortSet;
    SAPDB_Bool                        m_initialized;
    SQLMan_UpdateStatistics&          m_updStatObj;
    Data_PrimTreeStatisticsCoordinator::StatisticsInfo m_statisticsInfo;
    tsp00_Key                         m_curStartKey;
    tsp00_Key                         m_curStopKey;
    SAPDB_Int                         m_curStartKeyLen;
    SAPDB_Int                         m_curStopKeyLen;
    SAPDB_Bool                        m_doParallel;
    FileDir_Table                     m_tableFileEntry;
    SQLMan_UpdateStatistics::InternalTransInfo m_internalTransInfo;

    void SetTimeStamp(RTESys_SQLTimestamp& timeStamp)
    {
        vtimestamp( *reinterpret_cast<tsp00_Timestamp*>(&timeStamp) );
    }

    // no copy-construction
    TableInfo(const TableInfo&);
    TableInfo& operator=(const TableInfo&);

public:
    enum ReturnCode {
        Okay,
        TableNotFound,
        LockTimeout,
        ResourcesExhausted,
        Cancelled,
        SystemError
    };

    TableInfo(
        Trans_Context&                     trans,
        SQLMan_UpdateStatistics&           updStatObj,
        Data_UpdStatRequestFile::Iterator& iterator,
        const FileDir_FileNo&              requestFileNo)
        : m_trans( trans ),
          m_iterator( iterator ),
          m_requestFileNo( requestFileNo ),
          m_distinctVals( trans.Allocator() ),
          m_lastJobInProgress( false ),
          m_jobsRunningCount( 0 ),
          m_columnNumbers( trans.Allocator() ),
          m_stackCode( trans.Allocator() ),
          m_abortSet( false ),
          m_initialized( false ),
          m_updStatObj( updStatObj ),
          m_doParallel( false ),
          m_curStartKeyLen( 0 ),
          m_curStopKeyLen( 0 ),
          m_internalTransInfo( updStatObj )
    {
        SAPDBTRACE_METHOD_DEBUG(
            "Data_PrimTreeStatisticCoordinator::TableInfo::TableInfo",
            DataTree_Trace, 5 );
    }

    /****************************************************************/

    ~TableInfo()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "Data_PrimTreeStatisticCoordinator::TableInfo::~TableInfo",
            DataTree_Trace, 5 );

        SAPDBERR_ASSERT_STATE( m_trans.OldTrans().trError_gg00 == e_ok );

        SAPDBERR_ASSERT_STATE(
            (m_jobsRunningCount == 0)
            || m_abortSet );

        tgg00_TransContext& trans = m_trans.OldTrans();
        if ( m_auxFileNo.IsValid() ) {
            tgg00_Surrogate surr;

            surr.rawAssign( m_auxFileNo.GetCharArray() );
            bd998DestroyTempFile( trans, surr );
        }

        SAPDBERR_ASSERT_STATE( trans.trError_gg00 == e_ok );
    }

    /****************************************************************/

    ReturnCode Initialize(
        Msg_List&          errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "Data_PrimTreeStatisticCoordinator::TableInfo::Initialize",
            DataTree_Trace, 5 );

        tgg00_TransContext& trans = m_trans.OldTrans();

        tgg00_FileId fileId;
        this->BuildTempFileId( trans, fileId, 0 );

        bd998CreateTempFile( trans, fileId );

        if ( trans.trError_gg00 != e_ok ) {
            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_UPDSTAT_CREATE_DISTINCT_FILE_FAILED,
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_CREATE_DISTINCT_FILE_FAILED__FILENO,
                         FileDir_FileNoToString(
                             m_iterator.GetTableFileNo() ) ),
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_CREATE_DISTINCT_FILE_FAILED__RETURNCODE,
                         SQLManErr_Interface::GetInstance()
                         .GetBaseErrorText( trans.trError_gg00 ))
                );
            return SystemError;
        }

        m_auxFileNo                            = fileId.fileTabId_gg00();
        m_statisticsInfo.m_tableFileNo = m_iterator.GetTableFileNo();
        m_statisticsInfo.m_wholeTableProcessed =
            ( m_iterator.GetNoOfPagesToSample() == 0 );

        this->SetTimeStamp( m_statisticsInfo.m_beginTimeStamp );

        SQLMan_UpdateStatistics::RequestReturnCode updStatRc =
            m_updStatObj.GetRequestInfoAndLockTable(
                m_iterator.GetTableFileNo(),
                m_internalTransInfo,
                m_statisticsDesc,
                m_columnNumbers,
                m_stackCode,
                errMsg );

        if ( updStatRc != SQLMan_UpdateStatistics::RequestOkay ) {
            switch ( updStatRc ) {
                case SQLMan_UpdateStatistics::RequestTableNotFound:
                    return TableNotFound;
                case SQLMan_UpdateStatistics::RequestLockTimeout:
                    return LockTimeout;
                case SQLMan_UpdateStatistics::RequestCancelled:
                    return Cancelled;
                case SQLMan_UpdateStatistics::RequestNewTransFailed:
                    errMsg =
                        Msg_List(
                            Msg_List::Info,
                            SDBMSG_DATA_UPDSTAT_CREATE_TRANS_FAILED,
                            Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_CREATE_TRANS_FAILED__FILENO,
                                     FileDir_FileNoToString(
                                         m_iterator.GetTableFileNo()) ) );
                    return ResourcesExhausted;
                default:
                    errMsg.AddDetail(
                        SDBMSG_DATA_UPDSTAT_GET_LOCK_TABLE_FAILED,
                        Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_GET_LOCK_TABLE_FAILED__FILENO,
                                 FileDir_FileNoToString(
                                     m_iterator.GetTableFileNo()) )
                        );
                    return SystemError;
            }
        }

        if ( ! m_distinctVals.Resize( m_columnNumbers.GetSize() ) ) {
            errMsg =
                Msg_List(
                    Msg_List::Info,
                    SDBMSG_DATA_UPDSTAT_OUT_OF_MEMORY,
                    Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_OUT_OF_MEMORY__ALLOC_SIZE,
                             m_columnNumbers.GetSize() *
                             sizeof( DistinctValueVector::ValueType ) )
                    );
            return ResourcesExhausted;
        }

        DistinctValueVector::Iterator it    = m_distinctVals.Begin();
        DistinctValueVector::Iterator itEnd = m_distinctVals.End();
        int i = 0;
        for ( ; it != itEnd; ++it ) {
            (*it) = 0;
            ++i;
        }

        /*
         * check if table exists
         */
        FileDir_IDirectory& fDir =
            FileDir_IDirectory::Instance( trans.trIndex_gg00 );

        fDir.GetTableFile(
            trans.trTaskId_gg00,
            m_iterator.GetTableFileNo(),
            m_tableFileEntry,
            errMsg );

        if ( ! m_tableFileEntry.IsAssigned() ) {
            SAPDBTRACE_WRITELN(
                DataTree_Trace, 5, "table not found, skipping");
            return TableNotFound;;
        }

        // check if table should be processed in parallel
        if ( m_statisticsInfo.IsWholeTableProcessed() ) {

            // determine if table should be processed in parallel
            if ( this->AreColumnStatisticsRequested() ) {
                // if there are no column statistics to be determined
                // then the values can be taken from the file
                // directory --> no reason to do this in parallel

                tgg00_FileId tableFileId;
                m_tableFileEntry.BuildFileId( tableFileId );

                tbd_current_tree current;
                bd30GetTree( trans, tableFileId, current, m_select,
                             ! LOCK_TREE_EXCL_BD00,
                             ! SYNC_TREE_LOCK_BD00 );

                if ( trans.trError_gg00 == e_ok ) {
                    if ( current.currRootNptrs_bd00.np_ptr()->nd_level()
                         >= SECOND_INDEX_LEVEL_BD00 ) {
                        m_doParallel = true;
                    }
                }

                bd30ReleaseTree( current );

            }
        }
        /* end parallel */

        m_initialized = true;
        return Okay;
    }

    /****************************************************************/

    ReturnCode PrepareNextTableJob(
        tgg00_TransContext&   trans,
        const SAPDB_Int       jobId,
        JobInfo&              jobInfo,
        SrvTasks_WaitingTask& waitingTask,
        Msg_List&             errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "Data_PrimTreeStatisticCoordinator::TableInfo::PrepareNextTableJob",
            DataTree_Trace, 5 );

        SAPDBERR_ASSERT_STATE( &jobInfo.GetTableInfo() == this );

        if ( ! m_initialized ) {
            ReturnCode retCode = this->Initialize( errMsg );
            if ( retCode != Okay  ) {
                return retCode;
            }
        }

        SAPDB_Int noOfPagesToProcess = m_iterator.GetNoOfPagesToSample();
        DistinctValueVector* pDistinctVals = 0;

        if ( m_doParallel ) {
            SAPDB_Int        avgRecordsPerLeaf;
            pasbool          isLastSlice;
            SAPDB_Int        slicePageCount;
            tbd_current_tree current;
            tgg00_FileId     tableFileId;

            // only lock tree as long as necessary to allow as much
            // exclusive access on tree as possible for other tasks
            m_tableFileEntry.BuildFileId( tableFileId );
            bd30GetTree( trans, tableFileId, current, m_select,
                         ! LOCK_TREE_EXCL_BD00,
                         ! SYNC_TREE_LOCK_BD00 );

            if ( trans.trError_gg00 == e_ok ) {
                bd50DetermineKeyRangeForServer(
                    current,
                    reinterpret_cast<tsp00_KeyPtr>( &m_curStartKey ),
                    m_curStartKeyLen,
                    reinterpret_cast<tsp00_KeyPtr>( &m_curStopKey ),
                    m_curStopKeyLen,
                    slicePageCount,
                    avgRecordsPerLeaf,
                    isLastSlice );
            }

            bd30ReleaseTree( current );

            if ( trans.trError_gg00 != e_ok ) {
                // FIXME: error message
                return SystemError;
            }

            pDistinctVals = jobInfo.AllocateDistinctValBuffer();
            if ( ! pDistinctVals->Resize( m_columnNumbers.GetSize() ) ) {
                errMsg =
                    Msg_List(
                        Msg_List::Info,
                        SDBMSG_DATA_UPDSTAT_OUT_OF_MEMORY,
                        Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_OUT_OF_MEMORY__ALLOC_SIZE,
                                 m_columnNumbers.GetSize() *
                                 sizeof( DistinctValueVector::ValueType ) )
                        );
                return ResourcesExhausted;
            }

            SAPDBTRACE_WRITELN(
                DataTree_Trace, 5, "job should process about "
                << SAPDB_ToString( slicePageCount ) << " pages" );

            m_lastJobInProgress = isLastSlice ? true : false;
        } else {
            pDistinctVals       = &m_distinctVals;
            m_lastJobInProgress = true;
        }

        SrvTasks_Job::Attributes jobAttributes( jobId );
        SrvTasks_JobCollectTableStatistics* pSrvJob =
            new ( jobInfo.GetAllocator() )
            SrvTasks_JobCollectTableStatistics(
                jobAttributes,
                m_iterator.GetTableFileNo(),
                *pDistinctVals,
                m_curStartKey,
                m_curStartKeyLen,
                m_curStopKey,
                m_curStopKeyLen,
                m_stackCode,
                m_auxFileNo,
                noOfPagesToProcess,
                waitingTask);

        if ( pSrvJob == 0 ) {
            errMsg =
                Msg_List(
                    Msg_List::Info,
                    SDBMSG_DATA_UPDSTAT_OUT_OF_MEMORY,
                    Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_OUT_OF_MEMORY__ALLOC_SIZE,
                             sizeof( SrvTasks_JobCollectTableStatistics ) )
                    );
            return ResourcesExhausted;
        }

        jobInfo.SetJob( *pSrvJob );

        ++m_jobsRunningCount;

        return Okay;
    }

    /****************************************************************/

    FileDir_FileNo GetTableFileNo() const
    {
        return m_iterator.GetTableFileNo();
    }

    /****************************************************************/

    SAPDB_UInt GetJobsRunningCount() const
    {
        return m_jobsRunningCount;
    }

    /****************************************************************/

    SAPDB_Bool AreJobsAvailable() const
    {
        return (! m_lastJobInProgress) && (! m_abortSet);
    }

    /****************************************************************/

    SAPDB_Bool IsComplete() const
    {
        return
            (! this->AreJobsAvailable())
            && (m_jobsRunningCount == 0)
            && (! m_abortSet);
    }

    /****************************************************************/

    SAPDB_Bool AreColumnStatisticsRequested() const
    {
        return m_distinctVals.GetSize() > 0;
    }

    /****************************************************************/

    void Abort()
    {
        m_abortSet = true;
    }

    /****************************************************************/

    SAPDB_Bool SetJobFailed(
        tgg00_TransContext& trans,
        JobInfo&            jobInfo,
        Msg_List&           errMsg)
    {
        this->Abort();

        this->CleanupJob( jobInfo );

        return true;
    }

    /****************************************************************/

    ReturnCode SetJobDone(
        tgg00_TransContext& trans,
        JobInfo&            jobInfo,
        Msg_List&           errMsg)
    {
        ReturnCode rc = Okay;
        if ( ! m_abortSet ) {
            m_statisticsInfo.m_leafPageCount +=
                jobInfo.GetJob().GetLeafPageCount();
            m_statisticsInfo.m_recordCount   +=
                jobInfo.GetJob().GetRecordCount();

            if ( m_doParallel ) {
                const DistinctValueVector& jobDistinctValues =
                    jobInfo.GetJob().GetDistinctValues();

                SAPDB_UInt i;
                for ( i = 0; i < jobDistinctValues.GetSize(); ++i ) {
                    m_distinctVals[ i ] += jobDistinctValues[ i ];
                }
            }
        }

        this->CleanupJob( jobInfo );

        if ( this->IsComplete() ) {

            SAPDBERR_ASSERT_STATE( m_tableFileEntry.IsAssigned() );

            // make sure not to use sampled values if possible
            if ( m_tableFileEntry.AreCountersInitialized() ) {
                m_statisticsInfo.m_leafPageCount =
                    m_tableFileEntry.GetLeafPageCount( trans.trTaskId_gg00 );

                m_statisticsInfo.m_recordCount =
                    static_cast<Data_PageCount>(
                        m_tableFileEntry.GetEntryCount( trans.trTaskId_gg00 ) );
            }

            rc =  this->StatisticsComplete( trans, errMsg );
            if ( rc == Okay ) {
                const SAPDB_Bool bIsProcessed = true;
                Data_UpdStatRequestFile::Iterator::ReturnCode itRc =
                    m_iterator.SetRequestProcessed(
                        trans, bIsProcessed, errMsg );
                if ( itRc != Data_UpdStatRequestFile::Iterator::Okay ) {
                    rc = SystemError;
                }
            }
        }

        return rc;
    }

    /****************************************************************/

    ReturnCode StatisticsComplete(
        tgg00_TransContext& trans,
        Msg_List&           errMsg)
    {
        this->SetTimeStamp( m_statisticsInfo.m_endTimeStamp );

        Data_UpdStatRequestFile::Iterator::ReturnCode retCode =
            m_iterator.SetRequestProcessed( trans, true, errMsg );

        if ( retCode != Data_UpdStatRequestFile::Okay ) {
            return SystemError;
        }

        m_statisticsInfo.m_columnNumbers  = &m_columnNumbers;
        m_statisticsInfo.m_distinctValues = &m_distinctVals;

        SQLMan_UpdateStatistics::ReturnCode updRecRetCode =
            m_updStatObj.UpdateStatisticRecords(
                m_internalTransInfo,
                m_statisticsDesc,
                m_statisticsInfo,
                errMsg );

        if ( updRecRetCode != SQLMan_UpdateStatistics::Okay ) {
            switch ( updRecRetCode ) {
                case SQLMan_UpdateStatistics::Cancelled:
                    return Cancelled;
                case SQLMan_UpdateStatistics::RequestLockTimeout:
                    return LockTimeout;
                default:
                    return SystemError;
            }
        }
        return Okay;
    }

    /****************************************************************/

    // Return true, if this was the last job running
    SAPDB_Bool CleanupJob(JobInfo& jobInfo)
    {
        SAPDBERR_ASSERT_STATE( m_jobsRunningCount > 0 );

        --m_jobsRunningCount;
        jobInfo.DestroyJob();

        return m_jobsRunningCount == 0;
    }

    /****************************************************************/

    void BuildTempFileId(
        tgg00_TransContext& trans,
        tgg00_FileId&       fileId,
        SAPDB_Int4          curFileIndex)
    {
        g04build_temp_tree_id( fileId, trans );
        fileId.fileTfn_gg00().becomes( tfnTemp_egg00 );
        fileId.fileTfnTemp_gg00().becomes( ttfnTempTable_egg00 );
        fileId.fileTempCnt_gg00() = curFileIndex;
        fileId.fileType_gg00().addElement( ftsConcurrent_egg00 );
        fileId.fileType_gg00().addElement( ftsShared_egg00 );
    }
};

/*************************************************************************/

class JobQueue
{
private:
    typedef Container_Vector<TableInfo*> TableList;
    typedef Container_Vector<JobInfo*>   JobList;

    tgg00_TransContext&               m_trans;
    Data_UpdStatRequestFile&          m_requestFile;
    SAPDB_Int4                        m_taskIdCounter;
    Data_UpdStatRequestFile::Iterator m_iterator;
    TableList                         m_tableList;
    JobList                           m_runQueue;

public:
    enum ReturnCode {
        Okay,
        ResourcesExhausted,
        NoMoreJobs,
        Cancelled,
        SystemError
    };

    JobQueue(
        tgg00_TransContext&      trans,
        Data_UpdStatRequestFile& requestFile,
        SAPDB_Int2               maxServerTasks,
        Msg_List&                errMsg)
        : m_trans( trans ),
          m_requestFile( requestFile ),
          m_taskIdCounter( 0 ),
          m_iterator ( requestFile.GetIterator( trans ) ),
          m_tableList( Trans_Context::GetContext( trans ).Allocator() ),
          m_runQueue( Trans_Context::GetContext( trans ).Allocator() )
    {
        Data_UpdStatRequestFile::Iterator::ReturnCode itRetCode;
        itRetCode = m_iterator.Next(trans, errMsg );

        m_runQueue.Reserve( maxServerTasks );
        m_tableList.Reserve( maxServerTasks );
    }

    /****************************************************************/

    ~JobQueue()
    {
        Msg_List errMsg;
        tgg00_BasisError auxErr = m_trans.trError_gg00;

        JobList::Iterator jobIt = m_runQueue.Begin();
        for ( JobList::Iterator jobItEnd = m_runQueue.End();
              jobIt != jobItEnd; ++jobIt ) {

            JobInfo* pJobInfo = *jobIt;
            destroy ( pJobInfo,
                      Trans_Context::GetContext( m_trans ).Allocator() );
        }

        TableList::Iterator it = m_tableList.Begin();
        for ( TableList::Iterator itEnd = m_tableList.End();
              it != itEnd; ++it ) {
            TableInfo* pTableInfo = *it;

            pTableInfo->Abort();
            destroy ( pTableInfo,
                      Trans_Context::GetContext( m_trans ).Allocator() );
        }
        m_trans.trError_gg00 = auxErr;
        SAPDBERR_ASSERT_STATE( auxErr == e_ok );
    }

    /****************************************************************/

    ReturnCode RemoveTable(TableInfo*& tableInfo, Msg_List& errMsg)
    {
        // remove table from list
        TableList::Iterator tIt = m_tableList.Begin();
        for ( const TableList::Iterator tItEnd = m_tableList.End();
              tIt != tItEnd;
              ++tIt ) {

            if ( *tIt == tableInfo ) {
                m_tableList.Delete( tIt );
                destroy(
                    tableInfo,
                    Trans_Context::GetContext( m_trans ).Allocator() );
                return Okay;
            }
        }

        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_DATA_UPDSTAT_TABLEINFO_NOT_IN_LIST,
            Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_TABLEINFO_NOT_IN_LIST__FILENO,
                     FileDir_FileNoToString(
                         tableInfo->GetTableFileNo() ) ) );

        return SystemError;
    }

    /****************************************************************/

    ReturnCode PrepareJob(
        SQLMan_UpdateStatistics& updStatObj,
        SrvTasks_WaitingTask&    waitingTask,
        Msg_List&                errMsg)
    {
        SAPDBTRACE_METHOD_DEBUG(
            "Data_PrimTreeStatisticCoordinator::JobQueue::PrepareJob",
            DataTree_Trace, 5 );

        if ( ! m_runQueue.IsEmpty() && ! m_runQueue[ 0 ]->IsRunning() ) {
            return Okay;
        }

        SAPDBMem_IRawAllocator& sessionAlloc =
            Trans_Context::GetContext( m_trans ).Allocator();

        TableInfo* pTabWithJob;
        ReturnCode retCode;

        while ( true ) {

            if ( Trans_Context::GetContext( m_trans ).Task().CancelPending() ) {
                // safety net in case we are confused and would be
                // looping forever
                return Cancelled;
            }

            // check if any of the tables in process can start another
            // job
            retCode = this->PrepareNextAvailableJob(
                pTabWithJob, updStatObj, waitingTask, errMsg );

            // if retCode is != NoMoreJobs we either have a job handy
            // or a problem
            if ( retCode != NoMoreJobs ) {
                return retCode;
            }

            // prepare next table since all other tables are busy
            if ( ! m_iterator.IsValid() ) {
                return NoMoreJobs;
            }

            retCode = this->PrepareNewTable(
                updStatObj,
                errMsg );

            if ( retCode != Okay ) {
                return retCode;
            }

            // advance iterator
            this->AdvanceRequestIterator(
                Trans_Context::GetContext( m_trans ), errMsg );
        }
    }

    /****************************************************************/

    ReturnCode PrepareNextAvailableJob(
        TableInfo*&              pTabWithJob,
        SQLMan_UpdateStatistics& updStatObj,
        SrvTasks_WaitingTask&    waitingTask,
        Msg_List&                errMsg)
    {
        SAPDBMem_IRawAllocator& sessionAlloc =
            Trans_Context::GetContext( m_trans ).Allocator();

        pTabWithJob = 0;
        TableInfo::ReturnCode tabInfRc;

        // if possible, a table is always processed in parallel
        do {
            if ( Trans_Context::GetContext( m_trans ).Task().CancelPending() ) {
                // safety net in case we are confused and would be
                // looping forever
                return Cancelled;
            }

            // determine the table that has the fewest jobs running
            TableList::Iterator it = m_tableList.Begin();
            for ( const TableList::Iterator itEnd = m_tableList.End();
                  it != itEnd;
                  ++it ) {
                if ( (*it)->AreJobsAvailable() ) {
                    if ( (! pTabWithJob)
                         || ( (*it)->GetJobsRunningCount()
                              < pTabWithJob->GetJobsRunningCount() ) ) {
                        pTabWithJob = (*it);
                    }
                }
            }

            if ( pTabWithJob != 0 ) {
                // prepare job
                if ( DataTree_Trace.TracesLevel( 5 ) ) {
                    Kernel_VTrace()
                        << "Data_PrimTreeStatisticsCoordinator:" << NewLine
                        << "  preparing job for table: "
                        << pTabWithJob->GetTableFileNo()
                        << ", runningJobsCount = "
                        << pTabWithJob->GetJobsRunningCount();
                }

                JobInfo* pJobInfo =
                    new (sessionAlloc) JobInfo( sessionAlloc, *pTabWithJob );

                if ( pJobInfo == 0 ) {
                    errMsg =
                        Msg_List(
                            Msg_List::Info,
                            SDBMSG_DATA_UPDSTAT_OUT_OF_MEMORY,
                            Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_OUT_OF_MEMORY__ALLOC_SIZE,
                                     sizeof( JobInfo* ) )
                            );

                    return ResourcesExhausted;
                }

                SAPDB_Int jobId = m_taskIdCounter++;

                if ( DataTree_Trace.TracesLevel( 5 ) ) {
                    Kernel_VTrace()
                        << "  jobId = " << jobId;
                }

                tabInfRc = pTabWithJob->PrepareNextTableJob(
                    m_trans, jobId, *pJobInfo, waitingTask, errMsg );


                if ( tabInfRc == TableInfo::Okay ) {

                    m_runQueue.InsertFront( pJobInfo );

                    return Okay;

                } else {

                    destroy( pJobInfo, sessionAlloc );

                    pTabWithJob->Abort();

                    if ( pTabWithJob->GetJobsRunningCount() == 0 ) {

                        if ( this->RemoveTable( pTabWithJob, errMsg ) != Okay ) {
                            return SystemError;
                        }
                    }

                    if ( (tabInfRc != TableInfo::TableNotFound)
                         && (tabInfRc != TableInfo::LockTimeout) ) {

                        switch ( tabInfRc ) {
                            case TableInfo::Cancelled:
                                return Cancelled;
                            case TableInfo::ResourcesExhausted:
                                return ResourcesExhausted;
                            default:
                                // error message from tableInfo should be enough
                                return SystemError;
                        }
                    }
                }
            }
        } while ( pTabWithJob != 0 );

        return NoMoreJobs;
    }

    /****************************************************************/

    ReturnCode PrepareNewTable(
        SQLMan_UpdateStatistics& updStatObj,
        Msg_List&                errMsg)
    {
        Trans_Context& transContext = Trans_Context::GetContext( m_trans );

        TableInfo* newTable =
            new (transContext.Allocator())
                 TableInfo(
                     transContext,
                     updStatObj,
                     m_iterator,
                     m_requestFile.GetFileNo() );

        if ( newTable == 0 ) {
            errMsg =
                Msg_List(
                    Msg_List::Info,
                    SDBMSG_DATA_UPDSTAT_OUT_OF_MEMORY,
                    Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_OUT_OF_MEMORY__ALLOC_SIZE,
                             sizeof( TableInfo ) )
                    );
            return ResourcesExhausted;
        }

        m_tableList.InsertFront( newTable );

        return Okay;
    }

    /****************************************************************/

    ReturnCode AdvanceRequestIterator(
        Trans_Context& transContext,
        Msg_List&      errMsg)
    {
        if ( ! m_iterator.IsValid() ) {
            return NoMoreJobs;
        }

        Data_UpdStatRequestFile::Iterator::ReturnCode itRetCode;
        itRetCode = m_iterator.Next( transContext.OldTrans(), errMsg );

        if ( itRetCode == Data_UpdStatRequestFile::Iterator::Okay ) {
            SAPDBTRACE_WRITELN(
                DataTree_Trace, 5, "next table will be: "
                << m_iterator.GetTableFileNo() );
        }

        return Okay;
    }

    /****************************************************************/

    JobInfo& GetPreparedJob()
    {
        return *m_runQueue[ 0 ];
    }

    /****************************************************************/

    SAPDB_Bool JobsAvailable() const
    {
        if ( ! m_runQueue.IsEmpty() && ! m_runQueue[ 0 ]->IsRunning() ) {
            return true;
        }

        if ( m_iterator.IsValid() ) {
            return true;
        }

        TableList::ConstIterator it = m_tableList.Begin();
        for ( const TableList::ConstIterator itEnd = m_tableList.End();
              it != itEnd;
              ++it ) {
            if ( (*it)->AreJobsAvailable() ) {
                return true;
            }
        }

        return false;
    }

    /****************************************************************/

    SAPDB_Bool IsJobPrepared() const
    {
        return ! m_runQueue.IsEmpty() && ! m_runQueue[ 0 ]->IsRunning();
    }

    /****************************************************************/

    SAPDB_Bool AllJobsDone() const
    {
        return (! JobsAvailable()) && m_runQueue.IsEmpty();
    }

    /****************************************************************/

    ReturnCode MarkJobDone(
        tgg00_TransContext trans,
        const SAPDB_Int    jobId,
        const SAPDB_Bool   completedSuccessfully,
        Msg_List&          errMsg)
    {

        SAPDBMem_IRawAllocator& sessionAlloc =
            Trans_Context::GetContext( m_trans ).Allocator();

        SAPDB_AutoPtr<JobInfo> pJobInfo( sessionAlloc, 0 );

        JobList::Iterator it = m_runQueue.Begin();
        for ( JobList::Iterator itEnd = m_runQueue.End();
              it != itEnd; ++it ) {
            if ( (*it)->GetJobId() == jobId ) {
                pJobInfo.SetPtr( *it );
                m_runQueue.Delete( it );
                break;
            }
        }

        if ( pJobInfo.GetPtr() == 0 ) {
            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_UPDSTAT_INVALID_JOBID,
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_INVALID_JOBID__JOB_ID,
                         SAPDB_ToString( jobId ) ),
                Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_INVALID_JOBID__SIZE,
                         SAPDB_ToString( m_runQueue.GetSize() ) ) );
            return SystemError;
        }

        TableInfo* tableInfo = &pJobInfo->GetTableInfo();

        if ( tableInfo != 0 ) {
            ReturnCode retCode = Okay;

            if ( completedSuccessfully ) {
                TableInfo::ReturnCode tabRetCode =
                    tableInfo->SetJobDone( trans, *pJobInfo, errMsg );

                if ( tabRetCode != TableInfo::Okay ) {
                    switch ( tabRetCode ) {
                        case TableInfo::Cancelled:
                            retCode = Cancelled;
                            break;
                        case TableInfo::LockTimeout:
                            retCode = Okay; // ignore
                            break;
                        default:
                            // tableInfo should have set error msg
                            retCode = SystemError;
                    }
                }
            } else {
                if ( ! tableInfo->SetJobFailed( trans, *pJobInfo, errMsg ) ) {
                    retCode = SystemError;
                }
            }

            if ( (retCode == Okay) && tableInfo->IsComplete() ) {
                // remove table from list
                if ( this->RemoveTable( tableInfo, errMsg ) != Okay ) {
                    return SystemError;
                }
            }
            return retCode;
        }
        return SystemError;
    }
};

} // end of Data_PrimTree_StatisticsCoordinator namespace

/**************************************************************************/
/**************************************************************************/

Data_PrimTreeStatisticsCoordinator::Data_PrimTreeStatisticsCoordinator(
    tgg00_TransContext&     transContext,
    SQLMan_UpdateStatistics& updStatObj,
    Data_UpdStatRequestFile& updStatRequestFile)
    :
    m_trans(transContext),
    m_updStatObj( updStatObj ),
    m_updStatRequestFile( updStatRequestFile )
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatisticsCoordinator::Data_PrimTreeStatisticsCoordinator",
        DataTree_Trace, 5 );

	if ( e_ok != transContext.trError_gg00 ) {
        return;
    }
}

/**************************************************************************/

Data_PrimTreeStatisticsCoordinator::~Data_PrimTreeStatisticsCoordinator()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatisticsCoordinator::~Data_PrimTreeStatisticsCoordinator",
        DataTree_Trace, 5 );

   // Call Destructor for FileList
}

/**************************************************************************/

Data_PrimTreeStatisticsCoordinator::ReturnCode
Data_PrimTreeStatisticsCoordinator::UpdateStatistics(
    Msg_List& errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatisticsCoordinator::UpdateStatistics",
        DataTree_Trace, 5 );

    RTETask_ITask& task = *RTETask_ITask::ByTaskID( m_trans.trTaskId_gg00 );

    SrvTasks_WaitingTask waitingTask ( task );

    Srv_StatJobList
        serverJobList ( Trans_Context::GetContext( m_trans ).Allocator() );

    SAPDB_Int sUsableServerCount;
    if ( SQLMan_UpdateStatistics::GetNoOfParallelServers() == 0 ) {
        sUsableServerCount = bd01UsableServerForParallelExecution();
    } else {
        sUsableServerCount = SQLMan_UpdateStatistics::GetNoOfParallelServers();
    }

    SAPDB_UInt maxUsableServerCount =
        sUsableServerCount > 1 ? sUsableServerCount : 1;

    if ( maxUsableServerCount > SrvTasks_ICoordinator::Instance().GetMaxServerTasks() ) {
        maxUsableServerCount = SrvTasks_ICoordinator::Instance().GetMaxServerTasks();
    }

    m_usableServerCount = maxUsableServerCount;
    m_resourceShortage  = false;

    if ( ! serverJobList.Reserve( m_usableServerCount ) ) {
        errMsg =
            Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_OUT_OF_MEMORY,
                Msg_Arg( SDBMSGTAG_DATA_OUT_OF_MEMORY__ALLOC_SIZE,
                         m_usableServerCount
                         * sizeof( Srv_StatJobList::ValueType) )
                );
        return SystemError;
    }

    const SAPDB_Int c_maxRetries = 10;
    bool  ok                     = true;
    bool  checkQueue             = false;
    SAPDB_UInt retryCounter      = 0;

    ReturnCode rc      = Okay;
    JobQueue jobQueue(
        m_trans, m_updStatRequestFile, m_usableServerCount, errMsg );

    JobQueue::ReturnCode jobQueueRc;
    while ( (! jobQueue.AllJobsDone()) && ok ) {

        if ( ( jobQueueRc = jobQueue.PrepareJob(
                   m_updStatObj, waitingTask, errMsg ) ) == JobQueue::Okay
            ) {

            JobInfo& newJobInfo = jobQueue.GetPreparedJob();

            SAPDBERR_ASSERT_STATE( newJobInfo.IsValid() );

            if ( m_usableServerCount > 0 ) {
                SrvTasks_Job::ExecuteByServerResult retCode =
                    newJobInfo.GetJob().ExecuteByServer( task, errMsg );

                if ( retCode == SrvTasks_Job::executeOK ) {

                    newJobInfo.SetIsRunning( true );
                    serverJobList.InsertEnd( &newJobInfo.GetJob() );
                    retryCounter = 0;

                    if ( ( m_usableServerCount < maxUsableServerCount )
                         && ( serverJobList.GetSize() == m_usableServerCount )
                        ) {
                        // try one more server but do not retry
                        ++m_usableServerCount;
                        retryCounter       = c_maxRetries + 1;
                        m_resourceShortage = false;
                    }
                } else if ( retCode == SrvTasks_Job::executeNoServerAvailable ) {
                    errMsg.ClearMessageList();
                    if ( retryCounter <= c_maxRetries ) {

                        checkQueue = true;
                        ++retryCounter;
                        vsleep( Trans_Context::GetContext( m_trans ).TaskId(), 1 );

                    } else {
                        // reduce number of available servers and try again
                        if ( serverJobList.GetSize() > 0 ) {
                            m_usableServerCount = serverJobList.GetSize();
                        } else {
                            m_usableServerCount = 1;
                            this->ReduceUsableServerCount( errMsg );
                        }
                        retryCounter = 0;

                        checkQueue = true;
                    }

                } else {
                    ok = false;
                }
            } else {
                if ( this->SelfProcessJob( jobQueue, newJobInfo, errMsg )
                     != Okay ) {
                    ok = false;
                }

                // try server again next time, but do not start retrying
                ++m_usableServerCount;
                retryCounter = c_maxRetries + 1;

                checkQueue = true;
            }
        } else {
            switch ( jobQueueRc ) {
                case JobQueue::NoMoreJobs:
                    break;
                case JobQueue::Cancelled:
                    rc = Cancelled;
                    break;
                case JobQueue::ResourcesExhausted:

                    if ( ! this->ReduceUsableServerCount( errMsg ) ) {
                        ok = false;
                        rc = SystemError;
                    }

                    checkQueue = true;
                    break;
                case JobQueue::SystemError:
                default:
                    // generic error message will be added before return
                    ok = false;
                    break;
            }
        }

        if ( ok
             && ( serverJobList.IsFull()
                  || ( serverJobList.GetSize() >= m_usableServerCount )
                  || (! jobQueue.JobsAvailable())
                  || checkQueue )
             && ( ! serverJobList.IsEmpty() ) ) {

            checkQueue = false;

            Srv_StatJobList::Iterator
                finishedJob;
            if ( serverJobList.WaitForAnyJobFinished(
                     waitingTask, finishedJob )
                ) {

                rc = ProcessResult( jobQueue, *(*finishedJob), errMsg );
                if ( rc != Okay ) {
                    ok = false;
                }

                serverJobList.Delete( finishedJob );

            } else {

                errMsg =
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_DATA_UPDSTAT_JOBLIST_ERROR,
                        Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_JOBLIST_ERROR__SIZE,
                                 serverJobList.GetSize() )
                        );
                SAPDBERR_ASSERT_STATE( false );
                rc = SystemError;
                ok = false;
            }
        }

        if ( Trans_Context::GetContext( m_trans ).Task().CancelPending() ) {
            SAPDBTRACE_WRITELN(
                DataTree_Trace, 1, "trans was cancelled " );
            ok = false;
            rc = Cancelled;
        }
    }

    if ( ! ok ) {
        ok = true;
        serverJobList.AbortAll( waitingTask );
        serverJobList.Clear();

        if ( rc != Cancelled ) {
            // add message that clearly states that the command is aborting
            errMsg.Overrule(
                Msg_List( Msg_List::Error, SDBMSG_DATA_UPDSTAT_ABORTING ) );
        }

        // make sure we have an error code set
        if ( rc == Okay ) {
            rc = SystemError;
        }
    }

    return rc;
}

/**************************************************************************/

Data_PrimTreeStatisticsCoordinator::ReturnCode
Data_PrimTreeStatisticsCoordinator::SelfProcessJob(
    JobQueue& jobQueue,
    JobInfo&  jobInfo,
    Msg_List& errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatisticsCoordinator::SelfProcessJob",
        DataTree_Trace, 5 );

    SAPDBERR_ASSERT_STATE( jobInfo.IsValid() );

    SrvTasks_JobCollectTableStatistics& job = jobInfo.GetJob();

    job.ExecuteDirectly( Trans_Context::GetContext( m_trans ) );

    return ProcessResult( jobQueue, job, errMsg );
}

/**************************************************************************/

Data_PrimTreeStatisticsCoordinator::ReturnCode
Data_PrimTreeStatisticsCoordinator::ProcessResult(
    JobQueue&                           jobQueue,
    SrvTasks_JobCollectTableStatistics& job,
    Msg_List&                           errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatisticsCoordinator::ProcessResult",
        DataTree_Trace, 5 );

    SAPDBTRACE_WRITELN(
        DataTree_Trace, 5, "process result for table: "
        << job.GetTableFileNo() << ", jobId = " << job.GetId() );

    const SAPDB_Bool c_completedSuccessfully = true;
    ReturnCode rc = Okay;

    errMsg.ClearMessageList();
    SrvTasks_JobCollectTableStatistics::ReturnCode childRc =
        job.GetReturnCode( errMsg );

    if ( childRc == SrvTasks_JobCollectTableStatistics::ResultOkay ) {

        JobQueue::ReturnCode jobRetCode = jobQueue.MarkJobDone(
            m_trans, job.GetId(), c_completedSuccessfully, errMsg );

        if ( jobRetCode != JobQueue::Okay ) {
            switch ( jobRetCode ) {
                case JobQueue::Cancelled:
                    rc = Cancelled;
                    break;
                default:
                    rc = SystemError;
            }
        }

    } else {

        switch ( childRc ) {
            case SrvTasks_JobCollectTableStatistics::ResultTableFileNotFound:
                jobQueue.MarkJobDone(
                    m_trans, job.GetId(), ! c_completedSuccessfully, errMsg );
                break;
            case SrvTasks_JobCollectTableStatistics::ResultCancelled:
                jobQueue.MarkJobDone(
                    m_trans, job.GetId(), ! c_completedSuccessfully, errMsg );
                rc = Cancelled;
                break;

            case SrvTasks_JobCollectTableStatistics::ResultOldFileVersion:
                errMsg =
                    Msg_List(
                        Msg_List::Info,
                        SDBMSG_DATA_UPDSTAT_OLD_FILEVERSION,
                        Msg_Arg( SDBMSGTAG_DATA_UPDSTAT_OLD_FILEVERSION__FILENO,
                                 FileDir_FileNoToString( job.GetTableFileNo() ) )
                        );
                RTE_Message( errMsg );
                errMsg.ClearMessageList();
                jobQueue.MarkJobDone(
                    m_trans, job.GetId(), ! c_completedSuccessfully, errMsg );
                break;

            case SrvTasks_JobCollectTableStatistics::ResultSystemError:
                rc = SystemError;
                break;

            default:
                errMsg.AddDetail(
                    SDBMSG_DATA_UPDSTAT_UNEXPECTED_JOB_RESULT,
                    Msg_Arg("RETURNCODE", SAPDB_ToString( rc ) ) );

                SAPDBERR_ASSERT_STATE( false );
                rc = SystemError;
        }
    }

    return rc;
}

/**************************************************************************/

SAPDB_Bool
Data_PrimTreeStatisticsCoordinator::ReduceUsableServerCount(
    Msg_List& errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatisticsCoordinator::ReduceUsableServerCount",
        DataTree_Trace, 5 );

    if ( m_usableServerCount > 0 ) {

        --m_usableServerCount;

        if ( m_usableServerCount == 0 ) {

            if ( ! m_resourceShortage ) {
                m_resourceShortage = true;;

                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Warning,
                        SDBMSG_DATA_UPDSTAT_RESOURCE_SHORTAGE ) );
                RTE_Message( errMsg );
            }
        }
        errMsg.ClearMessageList();

    } else {

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_DATA_UPDSTAT_OUT_OF_RESOURCES ) );

        return false;
    }

    return true;
}

/**************************************************************************/
