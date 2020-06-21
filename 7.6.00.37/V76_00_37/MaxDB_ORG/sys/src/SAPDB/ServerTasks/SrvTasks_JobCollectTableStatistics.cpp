/*****************************************************************************/
/*!
 @file    SrvTasks_JobCollectTableStatistics.cpp
 @author  MartinKi

 @brief Implementation of class SrvTasks_JobCollectTableStatistics.

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

#include "SQLManager/SQLMan_UpdateStatistics.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "ServerTasks/SrvTasks_JobCollectTableStatistics.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "ServerTasks/SrvTasks_JobTypes.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"

#include "DataAccess/Data_PrimTreeStatisticsCoordinator.hpp"
#include "DataAccess/Data_RandomTreeAccess.hpp"
#include "DataAccess/Data_ScanTreeAccess.hpp"
#include "DataAccess/Data_KeyBoundedPageAccess.hpp"
#include "DataAccess/Data_PrimTreeStatistic.hpp"
#include "DataAccess/Data_Types.hpp"

#include "Transaction/Trans_Context.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "KernelCommon/Kernel_VTrace.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hbd30.h"
#include "hbd01.h"
#include "gbd00.h"
#include "gsp00.h"
#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/// Helper class to lock/release files on construction/destruction.
SrvTasks_JobCollectTableStatistics::AutoTreeLock::AutoTreeLock(
    tgg00_TransContext& trans,
    tgg00_FileId&       fileId,
    tbd_current_tree&   current,
    LockType            lock)
    : m_current( current )
{
    if ( trans.trError_gg00 != e_ok ) {
        return;
    }

    bd30GetTree( trans, fileId, m_current, m_select,
                 (lock == LockExclusive), ! SYNC_TREE_LOCK_BD00 );

    m_lockSet = true; // must be set even if trError_gg00 != 0
}

SrvTasks_JobCollectTableStatistics::AutoTreeLock::~AutoTreeLock()
{
    this->Release();
}

void SrvTasks_JobCollectTableStatistics::AutoTreeLock::Release()
{
    if ( m_lockSet ) {
        bd30ReleaseTree( m_current );
    }
    m_lockSet = false;
}

/*************************************************************************/
/*************************************************************************/

SrvTasks_JobCollectTableStatistics::SrvTasks_JobCollectTableStatistics(
    const Attributes&     jobAttributes,
    const FileDir_FileNo& tableFileNo,
    Data_PrimTreeStatisticsCoordinator::DistinctValueVector& distinctValues,
    tsp00_Key               startKey,
    SAPDB_Int               startKeyLen,
    tsp00_Key               stopKey,
    SAPDB_Int               stopKeyLen,
    Data_PrimTreeStatisticsCoordinator::StackCodeVector& stackCode,
    const FileDir_FileNo&   auxFileNo,
    const SAPDB_Int4        noOfPagesToSample,
    SrvTasks_WaitingTask&   waitingTask)
    : SrvTasks_Job( jobUpdateStat, jobAttributes, &waitingTask ),
      m_tableFileNo( tableFileNo ),
      m_stackCode( stackCode ),
      m_auxFileNo( auxFileNo ),
      m_noOfPagesToSample( noOfPagesToSample ),
      m_returnCode( ResultOkay ),
      m_distinctVals( distinctValues ),
      m_leafPageCount( 0 ),
      m_recordCount( 0 ),
      m_startKey( startKey ),
      m_startKeyLen( startKeyLen ),
      m_stopKey( stopKey ),
      m_stopKeyLen( stopKeyLen )
{}

/*************************************************************************/

SrvTasks_JobCollectTableStatistics::~SrvTasks_JobCollectTableStatistics()
{
    SAPDBTRACE_METHOD_DEBUG (
        "SrvTasks_JobCollectTableStatistics::~SrvTasks_JobCollectTableStatistics",
        DataTree_Trace, 5);
}

/*************************************************************************/

const FileDir_FileNo&
SrvTasks_JobCollectTableStatistics::GetAuxFileNo() const
{
    return m_auxFileNo;
}

/*************************************************************************/

bool
SrvTasks_JobCollectTableStatistics::CreateCopy (
    SAPDBMem_IRawAllocator &allocator,
    Msg_List               &errlist,
    SrvTasks_Job*          &jobCopy ) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "SrvTasks_JobCollectTableStatistics::CreateCopy", DataTree_Trace, 5);

    // This is only called if nobody wants to wait for this job.
    // If nobody is waiting then no output field is needed - and copied

    jobCopy = new (allocator) SrvTasks_JobCollectTableStatistics(
        m_Attributes,
        m_tableFileNo,
        m_distinctVals,
        m_startKey,
        m_startKeyLen,
        m_stopKey,
        m_stopKeyLen,
        m_stackCode,
        m_auxFileNo,
        m_noOfPagesToSample,
        *m_WaitingTask
        );

    if ( 0 == jobCopy )
    {
        AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
        return false;
    }
    jobCopy->SetAttributes(m_Attributes);
    return true;
}

/*************************************************************************/

void SrvTasks_JobCollectTableStatistics::InitDistinctValueVector(
    Trans_Context& transContext)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SrvTasks_JobCollectTableStatistics::InitDistinctValueVector",
        DataTree_Trace, 5 );

    // prepare distinctVals; we have to initialize columnCount
    // elements using Resize (instead of Reserve), so the vector has
    // the correct size because statistics computation just changes
    // the array contents but does not touch the size
    m_distinctVals.Clear();
    SAPDB_Int2 columnCount = m_stackCode.GetSize();
    if ( ! m_distinctVals.Resize( columnCount ) ) {
        transContext.ErrorList() =
            Msg_List(
                Msg_List::Error,
                SDBMSG_SRVTASKS_TABLESTATISTICS_OUT_OF_MEMORY,
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_OUT_OF_MEMORY__ALLOC_SIZE,
                         columnCount
                         * sizeof(Data_PrimTreeStatisticsCoordinator::DistinctValueVector
                                  ::ValueType) )
                );
        m_Result     = error;
        m_returnCode = ResultSystemError;
        return;
    }
}

/*************************************************************************/

SrvTasks_Job::Result SrvTasks_JobCollectTableStatistics::ExecuteIntern(
    Trans_Context& transContext)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SrvTasks_JobCollectTableStatistics::CollectStatistics",
        DataTree_Trace, 5 );

    tgg00_TransContext& trans = transContext.OldTrans();
    FileDir_IDirectory& fDir  =
        FileDir_IDirectory::Instance( trans.trIndex_gg00 );

    Msg_List& errMsg = transContext.ErrorList();
    errMsg.ClearMessageList();

    // initialize distinct value vector
    this->InitDistinctValueVector( transContext );
    if ( m_Result == error ) {
        return m_Result;
    }

    /*
     * check if table and aux file exist
     */
    if ( DataTree_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "SrvTasks_JobCollectTableStatistics:" << NewLine
            << "  processing table: " << m_tableFileNo
            << ", jobId = " << this->GetId();
    }

    FileDir_Table tableFile;
    fDir.GetTableFile(
        transContext.TaskId(),
        m_tableFileNo,
        tableFile,
        errMsg );

    if ( ! tableFile.IsAssigned() ) {
        SAPDBTRACE_WRITELN(
            DataTree_Trace, 5, "table not found, skipping");
        m_returnCode = ResultTableFileNotFound;
        return m_Result;
    }

    FileDir_Table auxFile;
    fDir.GetTableFile(
        transContext.TaskId(),
        m_auxFileNo,
        auxFile,
        errMsg );
    if ( ! auxFile.IsAssigned() ) {
        errMsg.Overrule(
            Msg_List(
                Msg_List::Error ,
                SDBMSG_SRVTASKS_TABLESTATISTICS_TEMPFILE_NOT_FOUND,
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_TEMPFILE_NOT_FOUND__FILENO,
                         FileDir_FileNoToString( m_auxFileNo ) )
                ) );
        SAPDBERR_ASSERT_STATE( false );
        m_Result     = error;
        m_returnCode = ResultSystemError;
        return m_Result;
    }

    /*
     * compute statistics
     */
    SAPDB_Int2 columnCount = m_stackCode.GetSize();
    SAPDBERR_ASSERT_STATE( columnCount == m_distinctVals.GetSize() );
    if ( columnCount > 0 ) {

        this->CollectStatistics( trans, tableFile, auxFile, columnCount, errMsg );

    } else {
        // simply get page and record count
        this->DetermineLeafPageAndRecordCount( trans, tableFile, errMsg );

        if ( trans.trError_gg00 == e_file_not_found ) {
            trans.trError_gg00 = e_ok;
            m_returnCode = ResultTableFileNotFound;
            m_Result     = ok;
            return m_Result;
        }
    }

    SAPDBTRACE_WRITELN(
        DataTree_Trace, 5,
        "processing done: trError = "
        << SQLManErr_Interface::GetInstance().GetBaseErrorText(
            trans.trError_gg00) );

    if ( trans.trError_gg00 != e_ok ) {

        if ( trans.trError_gg00 == e_cancelled ) {
            trans.trError_gg00 = e_ok;
            m_Result     = ok;
            m_returnCode = ResultCancelled;
            return m_Result;
        }

        if ( trans.trError_gg00 == e_old_fileversion ) {
            trans.trError_gg00 = e_ok;
            m_Result     = ok;
            m_returnCode = ResultOldFileVersion;
            return m_Result;
        }

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error ,
                SDBMSG_SRVTASKS_TABLESTATISTICS_COLLECT_FAILED,
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_COLLECT_FAILED__FILENO,
                         FileDir_FileNoToString( tableFile.GetFileNo()) ),
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_COLLECT_FAILED__RETURNCODE,
                         SQLManErr_Interface::GetInstance().GetBaseErrorText(
                             trans.trError_gg00) )
                ) );
        trans.trError_gg00 = e_ok;
        m_Result     = error;
        m_returnCode = ResultSystemError;
        return m_Result;
    }
    return m_Result;
}

/*************************************************************************/

void SrvTasks_JobCollectTableStatistics::CollectStatistics(
    tgg00_TransContext& trans,
    FileDir_Table&      tableFile,
    FileDir_Table&      auxFile,
    const SAPDB_Int2&   columnCount,
    Msg_List&           errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SrvTasks_JobCollectTableStatistics::CollectStatistics",
        DataTree_Trace, 5 );

    tgg00_FileId tableFileId;
    tableFile.BuildFileId( tableFileId );

    tbd_current_tree current;
    tbd_current_tree tempCurrent;

    AutoTreeLock tableTreeLock(
        trans, tableFileId, current, AutoTreeLock::LockNonExclusive );

    if ( trans.trError_gg00 != e_ok ) {
        SAPDBTRACE_WRITELN(
            DataTree_Trace, 5, "get tree failed for table xxx: trError");

        // table might have been deleted this very moment
        if ( tableFile.GetFileState() == Data_FileDeleted ) {
            SAPDBTRACE_WRITELN(
                DataTree_Trace, 5, "table has been deleted");
            m_returnCode = ResultTableFileNotFound;
            return;
        }

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error ,
                SDBMSG_SRVTASKS_TABLESTATISTICS_GETTREE_FAILED,
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_GETTREE_FAILED__FILENO,
                         FileDir_FileNoToString( tableFile.GetFileNo()) ),
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_GETTREE_FAILED__RETURNCODE,
                        SQLManErr_Interface::GetInstance().GetBaseErrorText(
                            trans.trError_gg00) )
                ) );
        SAPDBERR_ASSERT_STATE( false );
        m_Result     = error;
        m_returnCode = ResultSystemError;
        return;
    }

    tgg00_FileId auxFileId;
    auxFile.BuildFileId( auxFileId );
    auxFileId.fileType_gg00().clear();
    auxFileId.fileType_gg00().addElement( ftsTemp_egg00 );
    auxFileId.fileBdUse_gg00().addElement( bd_write_acc );

    bd30BuildCurrent( trans, auxFileId, tempCurrent );

    if ( trans.trError_gg00 != e_ok ) {
        errMsg.Overrule(
            Msg_List(
                Msg_List::Error ,
                SDBMSG_SRVTASKS_TABLESTATISTICS_GETTEMP_FAILED,
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_GETTEMP_FAILED__FILENO,
                         FileDir_FileNoToString( tableFile.GetFileNo()) ),
                Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_GETTEMP_FAILED__RETURNCODE,
                        SQLManErr_Interface::GetInstance().GetBaseErrorText(
                            trans.trError_gg00) )
                ) );
        SAPDBERR_ASSERT_STATE( false );
        m_Result     = error;
        m_returnCode = ResultSystemError;
        return;
    }

    tgg00_StEntryAddr pColumnDescription =
        reinterpret_cast<tgg00_StEntryAddr>( &m_stackCode[0] );

    Data_PrimTreeStatistic  primTreeStatistic( current );

    if ( ( m_startKeyLen > 0 ) || ( m_stopKeyLen > 0 ) ) {
        // calculate statistics, multiple jobs per table

        SAPDBERR_ASSERT_STATE( m_noOfPagesToSample == 0 );
        if ( m_noOfPagesToSample != 0 ) {
            // FIXME: no parallelization for sampling
            m_Result           = error;
            m_returnCode       = ResultSystemError;
            return;
        }

        Data_KeyBoundedPageAccess accessor (
            current,
            m_startKey, m_startKeyLen,
            m_stopKey, m_stopKeyLen,
            Data_LruMiddle );

        primTreeStatistic.CalculateColumnStatistic(
            accessor,
            tempCurrent,
            columnCount, 1, columnCount,
            pColumnDescription, m_leafPageCount, m_recordCount,
            *reinterpret_cast<tgg00_ColumnDistinctValues*>(
                m_distinctVals.Begin() )
            );

    } else if ( m_noOfPagesToSample == 0 ) {
        // calculate statistics, don't sample
        Data_SerializableScanTreeAccess scanTreeAccessor( current, Data_LruMiddle );

        primTreeStatistic.CalculateColumnStatistic(
            scanTreeAccessor,
            tempCurrent,
            columnCount, 1, columnCount,
            pColumnDescription, m_leafPageCount, m_recordCount,
            *reinterpret_cast<tgg00_ColumnDistinctValues*>(
                m_distinctVals.Begin() )
            );
    } else {
        const SAPDB_Bool bNoDuplicates = true;

        Data_RandomTreeAccess randomTreeAccessor(
            trans, current, m_noOfPagesToSample, bNoDuplicates );

        if( ! randomTreeAccessor.Initialize( m_noOfPagesToSample )) {
            errMsg.Overrule(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_SRVTASKS_TABLESTATISTICS_INITSAMPLINGITERATOR_FAILED,
                    Msg_Arg( SDBMSGTAG_SRVTASKS_TABLESTATISTICS_INITSAMPLINGITERATOR_FAILED__FILENO,
                             FileDir_FileNoToString( tableFile.GetFileNo()) )
                    ) );
            trans.trError_gg00 = e_ok; // to be on the safe side
            m_Result           = error;
            m_returnCode       = ResultSystemError;
            return;
        }

        if ( SQLMan_UpdateStatistics::GetSampleAlgoNo() > 0 ) {
            AutoTreeLock auxTreeLock(
                trans, auxFileId, tempCurrent, AutoTreeLock::LockExclusive );

            primTreeStatistic.SampleNewColumnStatistic(
                randomTreeAccessor,
                tempCurrent,
                columnCount, 1, columnCount,
                pColumnDescription, m_leafPageCount, m_recordCount,
                *reinterpret_cast<tgg00_ColumnDistinctValues*>(
                    m_distinctVals.Begin() )
            );
        } else {
            primTreeStatistic.SampleColumnStatistic(
                randomTreeAccessor,
                tempCurrent,
                columnCount, 1, columnCount,
                pColumnDescription, m_leafPageCount, m_recordCount,
                *reinterpret_cast<tgg00_ColumnDistinctValues*>(
                    m_distinctVals.Begin() )
                );
        }
    }
}

/*************************************************************************/

void SrvTasks_JobCollectTableStatistics::DetermineLeafPageAndRecordCount(
    tgg00_TransContext& trans,
    FileDir_Table&      tableFile,
    Msg_List&           errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SrvTasks_JobCollectTableStatistics::DetermineLeafPageAndRecordCount",
        DataTree_Trace, 5 );

    if ( tableFile.AreCountersInitialized() ) {

        // take statistics from file directory
        m_leafPageCount = static_cast<Data_PageCount>(
            tableFile.GetLeafPageCount( trans.trTaskId_gg00 ) );
        m_recordCount   = static_cast<Data_PageCount>(
            tableFile.GetEntryCount( trans.trTaskId_gg00 ) );

    } else {

        tgg00_FileId tableFileId;
        tableFile.BuildFileId( tableFileId );

        SAPDB_Int4 leafPageCount;
        SAPDB_Int4 recordCount;

        if ( m_noOfPagesToSample == 0 ) {
            bd01CalculateRecordsAndLeaves(
                trans, tableFileId, leafPageCount, recordCount );
        } else {
            bd01SampleRecordsAndLeaves(
                trans, tableFileId,
                m_noOfPagesToSample, leafPageCount, recordCount );
        }

        m_leafPageCount = leafPageCount;
        m_recordCount   = recordCount;
    }
}

/*************************************************************************/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
