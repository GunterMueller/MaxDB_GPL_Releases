/*****************************************************************************/
/*!
 @file    SQLMan_UpdateStatistics.cpp
 @author  MartinKi

 @brief Implementation of class SQLMan_UpdateStatistics.

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

#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_ColumnDescriptor.hpp"
#include "SQLManager/Tables/SQLTab_SysUpdStatWanted.hpp"
#include "SQLManager/Tables/SQLTab_SysUpdStatLog.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"

#include "DataAccess/Data_PrimTreeStatisticsCoordinator.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"

#include "KernelCommon/ParameterNames/KernelParam_DataAccess.hpp"

#include "SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/System/RTESys_Time.h"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hak01.h" // a01_i_asterisk
#include "hak05.h" // a05_identifier_get
#include "hak06.h" // a06get_username
#include "hak10.h" // a10_cache_delete
#include "hak28.h" // a28lock_table_for_updstat
#include "hak51.h" // a51_internal_trans
#include "hak52.h" // a52_ex_commit_rollback
#include "hkb64.h" // k64set_treeleafnodes
#include "hgg06.h" // gg06SessionGetInt4
#include "heo52.h" // vtimestamp

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// default of 0 means 'use parallel update statistics and determine
/// number of servers automatically'
SAPDB_Int4 SQLMan_UpdateStatistics::m_noParallelServers = 0;

/// default of 0 means 'use classic update statistics algorithm'
SAPDB_Int SQLMan_UpdateStatistics::m_sampleAlgoNo = 0;

/**************************************************************************/

void SQLMan_UpdateStatistics::InitKernelParameters(
    Msg_List& errMsg)
{
	SAPDBTRACE_ROUTINE_DEBUG (
        "SQLMan_UpdateStatistics::InitKernelParameters", SQLMan_Trace, 1);

    SAPDB_Int8 paramVal = 0;
    if ( RTEConf_ParameterAccess::Instance()->GetInteger(
             KERNELPARAM_UPDATESTAT_PARALLEL_SERVERS,
             paramVal,
             errMsg) ) {
        if ( -1 > paramVal ) {
            paramVal = -1;
        }
    }
    m_noParallelServers = static_cast<SAPDB_Int>(paramVal);

    paramVal = 0;
    if ( RTEConf_ParameterAccess::Instance()->GetInteger(
             KERNELPARAM_UPDATESTAT_SAMPLE_ALGO,
             paramVal,
             errMsg) ) {
        if ( 0 > paramVal ) {
            paramVal = 0;
        }
    }
    m_sampleAlgoNo = static_cast<SAPDB_Int>(paramVal);
}

/**************************************************************************/

SQLMan_UpdateStatistics::SQLMan_UpdateStatistics(
    SQLMan_Context&            context,
    const tsp00_TermId&        termId,
    const SAPDB_Int4           sampleRows,
    const SAPDB_Int4           samplePercentage)
    : m_context( context ),
      m_updStatColInfo( context.GetAllocator() ),
      m_sampleRowCount( sampleRows ),
      m_samplePercentage( samplePercentage )
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::SQLMan_UpdateStatistics", SQLMan_Trace, 1);

    SAPDB_MemCopyNoCheck( m_termId, &termId, sizeof( m_termId ) );
}

/**************************************************************************/

SAPDB_Bool SQLMan_UpdateStatistics::Initialize(Msg_List& errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::Initialize", SQLMan_Trace, 1);

    // perform implicit commit and free all locks
    const pasbool bRelease = true;
    a52_ex_commit_rollback( m_context, m_commit, ! bRelease, ! bRelease );

    if ( m_context.ReturnCode() != 0 ) {
        return false;
    }

    vtimestamp( *reinterpret_cast<tsp00_Timestamp*>( &m_startTimeStamp ) );

    if ( m_requestFile.GetFileNo().IsValid() ) {
        m_requestFile.Destroy( m_context.TransContext(), errMsg );
    }

    m_updStatColInfo.Init();

    return m_requestFile.Create( m_context.TransContext(), errMsg ).IsValid();
}

/**************************************************************************/

SQLMan_UpdateStatistics::~SQLMan_UpdateStatistics()
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::SQLMan_UpdateStatistics", SQLMan_Trace, 1);

    Msg_List errMsg;
    m_requestFile.Destroy( m_context.TransContext(), errMsg );
}

/**************************************************************************/

void
SQLMan_UpdateStatistics::UpdateStatisticsColumn(
    Msg_List& errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::UpdateColumnStatistics", SQLMan_Trace, 1);

    if ( ! m_context.IsOk() ) {
        return;
    }

    // use subroutine to have Catalog_TableDescriptor destroyed before
    // calling ProcessRequest (to avoid having an object pin table
    // info into catalog cache)
    this->AnalyzeUpdateStatisticsColumnCommand( errMsg );
    if ( ! m_context.IsOk() ) {
        return;
    }

    ReturnCode rc = this->ProcessRequests( errMsg );
    if ( rc != Okay ) {
        switch ( rc ) {
            case Cancelled:
                m_context.ThrowError( e_cancelled, 1 );
                break;
            default:
                m_context.ThrowError( e_system_error, 1 );
        }
        return;
    }

    // perform implicit commit and free all locks
    const SAPDB_Bool bRelease = true;
    a52_ex_commit_rollback( m_context, m_commit, ! bRelease, ! bRelease );

    return;
}

/**************************************************************************/

void
SQLMan_UpdateStatistics::AnalyzeUpdateStatisticsColumnCommand(
    Msg_List& errMsg)
{
    /*
     * retrieve table info, check privileges and enter into sysupdstatwanted
     */
    SQLMan_Identifier tableName;
    SQLMan_Identifier schemaName;
    SQLMan_Identifier columnName;

    SAPDB_Int parseTreeIdx = (*m_context.a_ap_tree)[ 1 ].n_lo_level;
    if ( (*m_context.a_ap_tree)[ parseTreeIdx ].n_symb == s_authid ) {
        a06get_username( m_context, parseTreeIdx, schemaName );
    }
    SAPDB_Int stmtPos = (*m_context.a_ap_tree)[ parseTreeIdx ].n_pos();
    a05_identifier_get( m_context, parseTreeIdx, sizeof (tableName), &tableName[0] );

    const SAPDB_Bool bThrowError           = true;
    const SAPDB_Bool bConsiderResultTables = true;
    const SAPDB_Bool bConsiderSYSDDTables  = true;
    Catalog_TableDescriptor tableDesc(
        m_context, schemaName, tableName,
        ! bConsiderResultTables, ! bConsiderSYSDDTables );
    if ( ! tableDesc ) {
        if ( m_context.IsOk() ) {
            m_context.ThrowError( e_unknown_tablename, stmtPos, tableName );
        }
        return;
    }

    // check privileges
    if ( (m_context.GetCurrentUserId() != tableDesc.GetOwnerId() )
         && ! m_context.IsSysdbaUser()
         && ! m_context.IsControlUser() ) {
        m_context.ThrowError( e_missing_privilege, stmtPos, tableName );
        return;
    }

    if ( tableDesc.IsArchiveTable()
         || tableDesc.IsView()
         || tableDesc.IsSynonym() ) {
        m_context.ThrowError( e_invalid_tabletype, stmtPos, tableName );
        return;
    }

    if ( this->IgnoreTable( tableDesc ) ) {
        return;
    }

    // store column names
    m_updStatColInfo.tableFileNo = tableDesc.GetTableFileNo();

    SAPDB_Int columnCount = 1;
    stmtPos      = (*m_context.a_ap_tree)[ parseTreeIdx ].n_pos();
    parseTreeIdx = (*m_context.a_ap_tree)[ parseTreeIdx ].n_sa_level;
    while ( (parseTreeIdx != 0) && m_context.IsOk() ) {
        a05_identifier_get(
            m_context, parseTreeIdx, sizeof (columnName), &columnName[0] );
        stmtPos      = (*m_context.a_ap_tree)[ parseTreeIdx ].n_pos();
        parseTreeIdx = (*m_context.a_ap_tree)[ parseTreeIdx ].n_sa_level; // next col
        if ( (columnCount == 1)
             && (columnName == a01_i_asterisk) ) {

            if ( parseTreeIdx != 0 ) {
                // there are no more columns allowed
                m_context.ThrowError( e_missing_delimiter, stmtPos );
                return;
            }

            Catalog_TableDescriptor::ColumnIterator columnIt( tableDesc.Begin() );
            for ( Catalog_TableDescriptor::ColumnIterator
                      colItEnd( tableDesc.End() );
                  columnIt != colItEnd; ++columnIt ) {

                if ( this->AreStatisticsUnnecessary( tableDesc, *columnIt )
                     || ! this->AreStatisticsPossible( *columnIt ) ) {
                    continue;
                }

                const SAPDB_Bool insertOk =
                    m_updStatColInfo.columnNames.InsertEnd(
                        (*columnIt).GetColumnName() );
                if ( ! insertOk ) {
                    m_context.ThrowError( e_no_more_memory );
                }

            }
        } else {
            Catalog_TableDescriptor::ColumnIterator columnIt(
                tableDesc.FindColumn( columnName ) );
            if ( ! columnIt ) {
                m_context.ThrowError( e_unknown_columnname, stmtPos, columnName );
                return;
            }

            if ( ! this->AreStatisticsPossible(
                     *columnIt, bThrowError, stmtPos ) ) {
                return;
            }

            const SAPDB_Bool insertOk =
                m_updStatColInfo.columnNames.InsertEnd(
                    (*columnIt).GetColumnName() );
            if ( ! insertOk ) {
                m_context.ThrowError( e_no_more_memory );
            }
        }
        ++columnCount;
    }

    if ( ! m_context.IsOk() ) {
        return;
    }

    /*
     * determine number of pages to sample
     */
    SAPDB_Int4 noOfPagesToSample;
    SAPDB_Bool ok = this->DetermineSamplePageCount(
        tableDesc, noOfPagesToSample, errMsg );
    if ( ! ok ) {
        // error message is set by DetermineSamplePageCount
        m_context.ThrowError( e_system_error, 1 );
        return;
    }

    /*
     * process table
     */
    ReturnCode rc = this->AddTableStatRequest(
        tableDesc.GetTableFileNo(), noOfPagesToSample, errMsg );
    if ( rc != Okay ) {
        // error message set by AddTableStatRequest
        m_context.ThrowError( e_system_error, 1 );
        return;
    }
}

/**************************************************************************/

/*!
 * @brief Returns true if statistics for columnDesc need not be
 * determined because they can be inferred from other data that is
 * available.
 */
SAPDB_Bool SQLMan_UpdateStatistics::AreStatisticsUnnecessary(
    const Catalog_TableDescriptor&  tableDesc,
    const Catalog_ColumnDescriptor& columnDesc) const
{
	SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateStatistics::AreStatisticsUnnecessary", SQLMan_Trace, 1 );

    if ( columnDesc.IsSysKey()
         || ( columnDesc.IsKeyColumn()
              && ( tableDesc.GetKeyColumnCount() == 1 ) 
              && ( ! tableDesc.HasVirtualKey() ) ) ) {
        // syskeys are always unique
        // keys that are made of a single column are always unique
        return true;
    }

    return false;
}

/**************************************************************************/

SAPDB_Bool SQLMan_UpdateStatistics::AreStatisticsPossible(
    const Catalog_ColumnDescriptor& columnDesc,
    SAPDB_Bool                      bThrowErrorIfNot,
    SAPDB_Int                       stmtPos)
{
	SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateStatistics::AreStatisticsPossible", SQLMan_Trace, 1 );

    if ( columnDesc.IsLongDataType() ) {
        if ( bThrowErrorIfNot ) {
            m_context.ThrowError(
                e_command_for_string_not_allow, stmtPos,
                columnDesc.GetColumnName() );
        }
        return false;
    }

    return true;
}

/**************************************************************************/

SAPDB_Bool SQLMan_UpdateStatistics::IgnoreTable(
    const Catalog_TableDescriptor& tableDesc)
{
	SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateStatistics::IgnoreTable", SQLMan_Trace, 1 );

    if ( ( (m_samplePercentage == cStatValUndefined)
           ||(m_samplePercentage == cStatValNotSet) )
         && ( (m_sampleRowCount == cStatValUndefined)
              ||(m_sampleRowCount == cStatValNotSet) ) ) {
         return tableDesc.GetStatisticsSamplingInfo() == 0;
    }

    return false;
}

/**************************************************************************/

SAPDB_Bool SQLMan_UpdateStatistics::DetermineSamplePageCount(
    const Catalog_TableDescriptor& tableDesc,
    SAPDB_Int4&                    noOfPagesToSample,
    Msg_List&                      errMsg)
{
	SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_UpdateStatistics::DetermineSamplePageCount", SQLMan_Trace, 1 );

    const SAPDB_Int cMinimalSamplePageCount = 100;

    SAPDB_Int4 samplePercentage = m_samplePercentage;
    SAPDB_Int4 sampleRowCount   = m_sampleRowCount;

    if ( samplePercentage == cStatValUndefined ) {
        if ( tableDesc.GetStatisticsSamplingInfo() < 0 ) {
            samplePercentage = -tableDesc.GetStatisticsSamplingInfo();
        } else {
            sampleRowCount = tableDesc.GetStatisticsSamplingInfo();
        }
    }

    if ( (samplePercentage == 100)
         || ((sampleRowCount == cStatValUndefined)
             && (samplePercentage == cStatValUndefined))
         || ((sampleRowCount == cStatValNotSet)
             && (samplePercentage == cStatValNotSet)) ) {
        noOfPagesToSample = 0;
        return true;
    }

    SAPDB_Int pageCount;
    SAPDB_Bool ok = this->GetTablePageCount(
        tableDesc.GetTableFileNo(), pageCount, errMsg );

    if ( ! ok ) {
        // error message is set by GetTablePageCount
        return false;
    }

    if ( pageCount < cMinimalSamplePageCount ) {
        noOfPagesToSample = 0;
        return true;
    }

    if ( sampleRowCount > 0 ) {
        noOfPagesToSample = sampleRowCount /
            (cak_page80percent / tableDesc.GetAverageRowLength() );

    } else {
        noOfPagesToSample = pageCount * samplePercentage / 100;
    }

    if ( noOfPagesToSample < cMinimalSamplePageCount ) {
        noOfPagesToSample = cMinimalSamplePageCount;
    }

    if ( noOfPagesToSample > (pageCount * 1.5) ) {
        // this implies that we try to sample more rows than the
        // table actually contains, so just calculate precise
        // statistics:
        noOfPagesToSample = 0;
    }
    return true;
}

/**************************************************************************/

SAPDB_Bool SQLMan_UpdateStatistics::GetTablePageCount(
    const FileDir_FileNo& tableFileNo,
    SAPDB_Int&            pageCount,
    Msg_List&             errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::GetTablePageCount", SQLMan_Trace, 1);

    FileDir_IDirectory& fDir =
        FileDir_IDirectory::SessionInstance( m_context.TransContext() );

    FileDir_Table tableFile;
    FileDir_ReturnCode fDirRc = fDir.GetTableFile(
        m_context.TransContext().trTaskId_gg00,
        tableFileNo, tableFile, errMsg );

    if ( ! tableFile.IsAssigned() ) {
        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDSTAT_GET_TABLE_FAILED,
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__FILENO,
                         FileDir_FileNoToString( tableFileNo ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERROR_NUMBER,
                         SAPDB_ToString( fDirRc ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERRORTEXT,
                         SQLManErr_Interface::GetInstance().GetErrorText( 0 ) )
                )
            );
        SAPDBERR_ASSERT_STATE( tableFile.IsAssigned() );
        return false;
    }

    if ( tableFile.AreCountersInitialized() ) {
        pageCount = tableFile.GetLeafPageCount(
            m_context.TransContext().trTaskId_gg00 );
        return true;
    }

    // counter is not in file directory, use eval
    const SAPDB_Bool cSample = true;
    tgg00_BasisError bErr;
    SQLMan_FileId tableFileId;
    tableFile.BuildFileId( tableFileId );

    a06get_page_cnt( m_context, tableFileId, cSample, pageCount, bErr );

    if ( bErr != e_ok ) {
        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDSTAT_GET_TABLE_FAILED,
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__FILENO,
                         FileDir_FileNoToString( tableFileNo ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERROR_NUMBER,
                         SAPDB_ToString( bErr ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERRORTEXT,
                         SQLManErr_Interface::GetInstance().GetErrorText(
                             bErr ) )
                )
            );
        SAPDBERR_ASSERT_STATE( bErr == e_ok );
        return false;
    }

    return true;
}

/**************************************************************************/

SQLMan_UpdateStatistics::ReturnCode
SQLMan_UpdateStatistics::AddTableStatRequest(
    const FileDir_FileNo&   tableFileNo,
    const SAPDB_Int4        noOfPagesToSample,
    Msg_List&               errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::AddTableStatRequest", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( m_requestFile.GetFileNo().IsValid() );
    if ( m_requestFile.GetFileNo().IsInvalid() ) {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_NOT_INITIALIZED );
        return SystemError;
    }

    Data_UpdStatRequestFile::ReturnCode retCode =
        m_requestFile.AddTable(
            m_context.TransContext(),
            tableFileNo,
            noOfPagesToSample,
            errMsg );

    if ( retCode != Data_UpdStatRequestFile::Okay ) {
        switch ( retCode ) {
            case Data_UpdStatRequestFile::FileNotFound:
                // table has probably been deleted, ignore
                SAPDBTRACE_WRITELN(
                    SQLMan_Trace, 1,
                    "table not found: " << tableFileNo << ", skipping" );
                break;
            default:
                errMsg.AddDetail(
                    SDBMSG_SQLMAN_UPDSTAT_ADDTOREQUESTFILE_FAILED,
                    Msg_Arg( "FILENO",
                             FileDir_FileNoToString( tableFileNo ) ),
                    Msg_Arg( "FILENO1",
                             FileDir_FileNoToString( m_requestFile.GetFileNo() ) )
                    );
                return SystemError;
        }
    }

    return Okay;
}

/**************************************************************************/

SQLMan_UpdateStatistics::ReturnCode
SQLMan_UpdateStatistics::ProcessRequests(
    Msg_List& errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::ProcessRequests", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( m_requestFile.GetFileNo().IsValid() );

    if ( m_requestFile.GetFileNo().IsInvalid() ) {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_NOT_INITIALIZED );
        return SystemError;
    }

    // make sure catalog records will be locked and that we don't use
    // the shared catalog cache
    m_context.SetLocalCacheUsageOnly();
    m_context.a_is_ddl.becomes( ddl_update_statistics );
    const SAPDB_Bool bIsRollback = true;
    a10_cache_delete( m_context, ! bIsRollback );
    if ( ! m_context.IsOk() ) {
        return SystemError;
    }

#ifdef SAPDB_SLOW
    // if the cache is not completely cleared there is a chance that
    // we will run into -8 later on; make sure we notice this
    // potential problem with slowknl
    if ( ! a10NoTableInfoInCache( m_context ) ) {
        return SystemError;
    }
#endif

    Data_PrimTreeStatisticsCoordinator statCoord(
        m_context.TransContext(), *this, m_requestFile );

    Data_PrimTreeStatisticsCoordinator::ReturnCode statRc =
        statCoord.UpdateStatistics( errMsg );

    if ( statRc != Data_PrimTreeStatisticsCoordinator::Okay ) {
        switch ( statRc ) {
            case Data_PrimTreeStatisticsCoordinator::Cancelled:
            {
                return Cancelled;
            }
            default:
                return SystemError;
        }
    }

    return Okay;
}

/**************************************************************************/

SQLMan_UpdateStatistics::ReturnCode
SQLMan_UpdateStatistics::UpdateStatisticRecords(
    InternalTransInfo&                 internalTrans,
    Catalog_TableStatisticsDescriptor& statisticsDesc,
    const StatisticsInfo&              statisticsInfo,
    Msg_List&                          errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::UpdateStatisticRecords", SQLMan_Trace, 1);

    if ( m_context.GetTransContext().Task().CancelPending() ) {
        return Cancelled;
    }

    AutoSwitchToInternal switchToInternalTrans( internalTrans );

    if ( m_context.GetTransContext().Task().CancelPending() ) {
        return Cancelled;
    }

    RTETask_ITask& task =
        *RTETask_ITask::ByTaskID( m_context.TransContext().trTaskId_gg00 );

    SAPDBERR_ASSERT_STATE( statisticsInfo.GetTableFileNo().IsValid() );

    Catalog_TableDescriptor tableDesc(
        m_context, statisticsInfo.GetTableFileNo() );
    if ( ! tableDesc ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "table not found in catalog: "
            << statisticsInfo.GetTableFileNo()
            << ", rc = " << m_context.ReturnCode() );

        if ( m_context.ReturnCode() != 0 ) {
            const SAPDB_Int errorCode = m_context.ReturnCode();
            m_context.ResetReturnCode();

            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDSTAT_GET_TABLE_FAILED,
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__FILENO,
                        FileDir_FileNoToString(
                            statisticsInfo.GetTableFileNo() ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERROR_NUMBER,
                         SAPDB_ToString( errorCode ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERRORTEXT,
                         SQLManErr_Interface::GetInstance().GetErrorText(
                             errorCode ) )
                );
            return SystemError;
        }
        return Okay;
    }

    FileDir_IDirectory& fDir =
        FileDir_IDirectory::SessionInstance( m_context.TransContext() );

    FileDir_Table tableFile;
    fDir.GetTableFile(
        m_context.TransContext().trTaskId_gg00,
        statisticsInfo.GetTableFileNo(), tableFile, errMsg );
    if ( ! tableFile.IsAssigned() ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "fileNo not found in fdir: "
            << statisticsInfo.GetTableFileNo() << ", skipping" );
        return Okay;
    }

    statisticsDesc.SetRowCount( statisticsInfo.GetRecordCount() );
    statisticsDesc.SetPageCount( statisticsInfo.GetLeafPageCount() );

    // write log table entry for table (empty columnname)
    ReturnCode retCode =
        this->UpdateSysTables(
            tableDesc,
            0,
            statisticsInfo,
            0,
            errMsg );

    SAPDB_Int seqNo = 1;
    if ( (retCode == Okay)
         && ( tableDesc.GetKeyColumnCount() == 1 ) 
         && ( ! tableDesc.HasVirtualKey() ) ) {
        Catalog_TableDescriptor::KeyColumnIterator keyCol = tableDesc.BeginKey();
        SAPDBERR_ASSERT_STATE( keyCol );
        statisticsDesc.SetColumnStatistics(
            (*keyCol).RecColNo(),
            statisticsInfo.GetRecordCount() );

        const Catalog_ColumnDescriptor& colDesc = *keyCol;

        retCode = this->UpdateSysTables(
            tableDesc,
            &colDesc,
            statisticsInfo,
            seqNo,
            errMsg );

        ++seqNo;
    }

    for ( SAPDB_UInt i = 0;
          (retCode == Okay) && (i < statisticsInfo.GetDistinctValueCount());
          ++i, ++seqNo ) {

        statisticsDesc.SetColumnStatistics(
            statisticsInfo.GetColumnNumbers()[ i ],
            statisticsInfo.GetDistinctValues()[ i ] );

        Catalog_TableDescriptor::ColumnIterator columnIt =
            tableDesc.GetColumn(
                statisticsInfo.GetColumnNumbers()[ i ] );

        const Catalog_ColumnDescriptor& colDesc = *columnIt;

        retCode = this->UpdateSysTables(
            tableDesc,
            &colDesc,
            statisticsInfo,
            seqNo,
            errMsg );
    }

    if ( retCode == LockTimeout ) {
        // ignore lock conflicts that occured during insert into
        // SYSUPDSTATLOG
        retCode = Okay;
    }

    SAPDB_Int rc = 0;
    if ( retCode == Okay ) {

        if ( ! statisticsDesc.Store() ) {
            switch ( m_context.ReturnCode() ) {
                case -102:
                    retCode = Cancelled;
                    break;
                case -60:
                case -911:
                case 600:
                    // could not get lock to update statistics
                    retCode = LockTimeout;
                    break;
                default:
                    errMsg = Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDSTAT_STORE_STATISTICS_FAILED,
                        Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_STORE_STATISTICS_FAILED__FILENO,
                                 FileDir_FileNoToString(
                                     statisticsInfo.GetTableFileNo()) ),
                        Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_STORE_STATISTICS_FAILED__ERRORTEXT,
                                 SQLManErr_Interface::GetInstance().GetErrorText(
                                     m_context.ReturnCode() ))
                        );
            }
            m_context.ResetReturnCode();
        }
        // make sure we don't miss any errors that were set in m_context only
        rc = m_context.ReturnCode();
        m_context.ResetReturnCode();

        if ( (rc == 0) && (retCode == Okay) ) {
            this->UpdateTreeLeafNodes(
                tableFile,
                statisticsInfo.GetLeafPageCount() );

            rc = m_context.ReturnCode();
            m_context.ResetReturnCode();
        }
    }

    if ( (retCode == Okay) && (rc == 0) ) {
        internalTrans.Close( true );
    } else {
        internalTrans.Close( false );
        m_context.ResetReturnCode();
        if ( (rc == -102) || (retCode == Cancelled) ) {
            return Cancelled;
        } else if ( retCode != Okay ) {
            return retCode;
        } else {
            return SystemError;
        }
    }

    return Okay;
}

/**************************************************************************/

SQLMan_UpdateStatistics::ReturnCode
SQLMan_UpdateStatistics::UpdateSysTables(
    Catalog_TableDescriptor&        tableDesc,
    const Catalog_ColumnDescriptor* columnDesc,
    const StatisticsInfo&           statisticsInfo,
    SAPDB_Int                       seqNo,
    Msg_List&                       errMsg)
{
    SQLTab_SysUpdStatWanted updStatWanted( m_context );
    SQLTab_SysUpdStatWanted::DeleteRc delRc;
    SQLMan_Identifier       columnName;

    if ( columnDesc != 0 ) {
        columnDesc->GetColumnName( columnName );
        delRc = updStatWanted.DeleteRow( tableDesc, *columnDesc, errMsg );
    } else {
        delRc = updStatWanted.DeleteRow( tableDesc, errMsg );
    }

    if ( delRc != SQLTab_SysUpdStatWanted::delRcSystemError ) {
        errMsg.ClearMessageList();
    } else {
        return SystemError;
    }

    if ( ( columnDesc != 0 ) && columnDesc->IsSysKey() ) {
        return Okay;
    }

    return this->UpdateStatisticsLogTable(
        tableDesc,
        statisticsInfo,
        columnName,
        seqNo,
        errMsg );
}

/**************************************************************************/

SQLMan_UpdateStatistics::ReturnCode
SQLMan_UpdateStatistics::UpdateStatisticsLogTable(
    const Catalog_TableDescriptor& tableDesc,
    const StatisticsInfo&          statisticsInfo,
    const SQLMan_Identifier&       columnName,
    SAPDB_Int                      seqNo,
    Msg_List&                      errMsg)
{
    SQLTab_SysUpdStatLog logTable( m_context );
    //
    // add log table entry
    //

    // sample_pct
    SAPDB_Int4 samplePercent = m_samplePercentage;
    if ( statisticsInfo.IsWholeTableProcessed() ) {
        // file was processed completely
        samplePercent = 100;
    }

    // sample_row
    SAPDB_Int4 sampleRows = m_sampleRowCount;
    if ( statisticsInfo.IsWholeTableProcessed() ) {
        // file was processed completely
        sampleRows = 0;
    }

    if ( (samplePercent == cStatValUndefined)
         || (sampleRows == cStatValUndefined) ) {
        const SAPDB_Int tableSampleInfo =
            tableDesc.GetStatisticsSamplingInfo();
        if ( tableSampleInfo < 0 ) {
            samplePercent = - tableSampleInfo;
            sampleRows    = 0;
        } else {
            samplePercent = 0;
            sampleRows    = tableSampleInfo;
        }
    }

    // session
    SAPDB_Int4 sessionId = gg06SessionGetInt4(
        m_context.TransContext().trSessionId_gg00 );

    SQLMan_Identifier indexName;
    SQLTab_SysUpdStatLog::ReturnCode tabReturnCode;

    do {

        tabReturnCode =
            logTable.InsertRow(
                SQLTab_SysUpdStatLog::UpdateDuplicates,
                SQLTab_SysUpdStatLog::WithoutCommit,
                tableDesc.GetSchemaName(),
                tableDesc.GetTableName(),
                columnName,
                indexName,
                statisticsInfo.GetTableFileNo(),
                m_startTimeStamp,
                samplePercent,
                sampleRows,
                statisticsInfo.GetBeginTimeStamp(),
                statisticsInfo.GetEndTimeStamp(),
                sessionId,
                m_termId,
                seqNo,
                errMsg );

        // crash on all errors in slow kernel
        SAPDBERR_ASSERT_STATE(
            (tabReturnCode == SQLTab_SysUpdStatLog::Okay)
            || (tabReturnCode == SQLTab_SysUpdStatLog::LockRowFailed)
            || (tabReturnCode == SQLTab_SysUpdStatLog::DuplicateKey)
            || (tabReturnCode == SQLTab_SysUpdStatLog::Cancelled) );
        ++seqNo;
    } while ( tabReturnCode == SQLTab_SysUpdStatLog::DuplicateKey );

    if ( tabReturnCode != SQLTab_SysUpdStatLog::Okay ) {
        if ( tabReturnCode == SQLTab_SysUpdStatLog::Cancelled ) {
            return Cancelled;
        }

        if ( tabReturnCode == SQLTab_SysUpdStatLog::SystemError ) {
            errMsg.Overrule(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_SQLMAN_UPDSTAT_INSERT_FAILED )
                );

            return SystemError;
        }

        if ( tabReturnCode == SQLTab_SysUpdStatLog::LockRowFailed ) {
            errMsg = Msg_List(
                Msg_List::Info,
                SDBMSG_SQLMAN_UPDSTAT_INSERT_LOCK_COLLISION );
            RTE_Message( errMsg );
            errMsg.ClearMessageList();
            // do not try again
            return LockTimeout;
        }

        // ignore errors in fast kernel
        errMsg.Overrule(
            Msg_List(
                Msg_List::Warning,
                SDBMSG_SQLMAN_UPDATESTAT_IGNORE_INSERT_RETURN,
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDATESTAT_IGNORE_INSERT_RETURN__FILENO,
                         FileDir_FileNoToString(
                             statisticsInfo.GetTableFileNo() ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDATESTAT_IGNORE_INSERT_RETURN__RETURNCODE,
                         SAPDB_ToString( tabReturnCode ) )
                ) );
        RTE_Message( errMsg );
        errMsg.ClearMessageList();
    }

    return Okay;
}

/**************************************************************************/

SQLMan_UpdateStatistics::RequestReturnCode
SQLMan_UpdateStatistics::GetRequestInfoAndLockTable(
    const FileDir_FileNo&              tableFileNo,
    InternalTransInfo&                 internalTrans,
    Catalog_TableStatisticsDescriptor& statisticsDesc,
    ColumnNumberVector&                columnNumbers,
    StackCodeVector&                   stackCode,
    Msg_List&                          errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_UpdateStatistics::GetRequestInfoAndLockTable", SQLMan_Trace, 1);

    if ( m_context.GetTransContext().Task().CancelPending() ) {
        return RequestCancelled;
    }

    if ( ! internalTrans.NewTrans() ) {
        return RequestNewTransFailed;
    }

    AutoSwitchToInternal switchToInternalTrans( internalTrans );

    if ( m_context.GetTransContext().Task().CancelPending() ) {
        return RequestCancelled;
    }

    // make sure no one else is trying to update statistics (this has
    // to be done before touching any catalog information if we want
    // to make sure that we won't run into -8 later on)
    tgg00_BasisError bErr;
    SQLMan_Surrogate tableSurrogate;
    tableSurrogate.rawAssign( tableFileNo.GetCharArray() );
    a28lock_table_for_updstat( m_context, tableSurrogate, bErr );

    if ( bErr != e_ok ) {
        switch ( bErr ) {
            case e_request_timeout:
                return RequestLockTimeout;
            case e_cancelled:
                return RequestCancelled;
            default:
                errMsg = Msg_List(
                    Msg_List::Error,
                    SDBMSG_SQLMAN_UPDSTAT_LOCK_TABLE_FAILED,
                    Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_LOCK_TABLE_FAILED__FILENO,
                             FileDir_FileNoToString( tableFileNo ) ),
                    Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_LOCK_TABLE_FAILED__RETURNCODE,
                             SQLManErr_Interface::GetInstance().GetBaseErrorText( bErr ) )
                    );
                return RequestSystemError;
        }
    }

    // get table info
    Catalog_TableDescriptor tableDesc( m_context, tableFileNo );
    if ( ! tableDesc ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "table not found in catalog: "
            << tableFileNo
            << ", rc = " << m_context.ReturnCode() );

        if ( m_context.ReturnCode() != 0 ) {
            const SAPDB_Int errorCode = m_context.ReturnCode();
            m_context.ResetReturnCode();

            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDSTAT_GET_TABLE_FAILED,
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__FILENO,
                        FileDir_FileNoToString( tableFileNo ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERROR_NUMBER,
                         SAPDB_ToString( errorCode ) ),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_GET_TABLE_FAILED__ERRORTEXT,
                         SQLManErr_Interface::GetInstance().GetErrorText(
                             errorCode ) )
                );
            return RequestSystemError;
        }

        return RequestTableNotFound;
    }

    SAPDBERR_ASSERT_STATE(
        (! tableDesc.IsView()) && (! tableDesc.IsSynonym()) );
    if ( tableDesc.IsView() || tableDesc.IsSynonym() ) {
        const char* tableType;
        if ( tableDesc.IsView() ) {
            tableType = "VIEW";
        } else {
            tableType = "SYNONYM";
        }
        errMsg =
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDSTAT_INVALID_TABLE_TYPE,
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_INVALID_TABLE_TYPE__TABLE_TYPE,
                         tableType),
                Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_INVALID_TABLE_TYPE__FILENO,
                         FileDir_FileNoToString( tableFileNo ) )
                );
        return RequestSystemError;
    }

    // get and lock table statistics records (shared)
    const bool bForUpdate = true;
    if ( ! tableDesc.GetTableStatisticsDescriptor(
             ! bForUpdate, statisticsDesc ) ) {
        const int getStatRetCode = m_context.ReturnCode();
        m_context.ResetReturnCode();
        switch ( getStatRetCode ) {
            case -913:
            case -51:
            case -50:
            case 500:
                return RequestLockTimeout;
            case -102:
                return RequestCancelled;
            default:
                errMsg = Msg_List(
                    Msg_List::Error,
                    SDBMSG_SQLMAN_UPDSTAT_LOCK_TABLE_FAILED,
                    Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_LOCK_TABLE_FAILED__FILENO,
                             FileDir_FileNoToString( tableFileNo ) ),
                    Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTAT_LOCK_TABLE_FAILED__RETURNCODE,
                             SAPDB_ToString( getStatRetCode ) )
                    );
                return RequestSystemError;
        }
    }

    // add requests for columns in SYSUPDSTATWANTED
    SQLTab_SysUpdStatWanted updStatWanted( m_context );

    SQLMan_Identifier columnName;
    for (SQLTab_SysUpdStatWanted::RequestColumnIterator requestIt =
             updStatWanted.GetRequestColumnIterator( tableDesc );
         requestIt.Next( columnName ); ) {

        Catalog_TableDescriptor::ColumnIterator colIt = tableDesc.Begin();
        Catalog_TableDescriptor::ColumnIterator colItEnd = tableDesc.End();
        for ( ; colIt != colItEnd; ++colIt ) {

            if ( columnName == (*colIt).GetColumnName() ) {
                // make sure statistics can be determined for column
                // in question
                if ( ! this->AreStatisticsPossible( *colIt )
                    || this->AreStatisticsUnnecessary( tableDesc, *colIt ) ) {
                    break;
                }

                const SAPDB_Bool ok =
                    this->InsertIntoColumnList(
                        *colIt, columnNumbers, stackCode, errMsg );

                if ( ! ok ) {
                    return RequestSystemError;
                }

                break;
            }
        }
    }

    // add requests for key columns, columns that have an index defined
    // on them and those columns that already have statistics
    // information
    Catalog_TableDescriptor::ColumnIterator colIt    = tableDesc.Begin();
    Catalog_TableDescriptor::ColumnIterator colItEnd = tableDesc.End();
    for ( ; colIt != colItEnd; ++colIt ) {

        if ( ! ( ((*colIt).IsKeyColumn() && (!(*colIt).IsSysKey()))
                 || (*colIt).IsIndexed() )
             && ( ! statisticsDesc.ExistsColumnStatistics(
                      (*colIt).RecColNo() ) )
            ) {
            continue;
        }

        if ( this->AreStatisticsUnnecessary( tableDesc, *colIt ) ) {
            continue;
        }

        const SAPDB_Bool ok =
            this->InsertIntoColumnList(
                *colIt, columnNumbers, stackCode, errMsg );

        if ( ! ok ) {
            return RequestSystemError;
        }
    }

    // add requests for columns explicitly named in an 'Update
    // Statistics Column ...' statement
    if ( m_updStatColInfo.tableFileNo == tableFileNo ) {

        SQLMan_Identifier reqColName;

        IdentifierVector::Iterator reqColIt =
            m_updStatColInfo.columnNames.Begin();
        IdentifierVector::Iterator reqColItEnd =
            m_updStatColInfo.columnNames.End();
        for ( ; reqColIt != reqColItEnd; ++reqColIt ) {

            columnName = *reqColIt;

            Catalog_TableDescriptor::ColumnIterator colIt = tableDesc.Begin();
            Catalog_TableDescriptor::ColumnIterator colItEnd = tableDesc.End();
            for ( ; colIt != colItEnd; ++colIt ) {

                if ( columnName == (*colIt).GetColumnName() ) {
                    // make sure statistics can be determined for column
                    // in question
                    if ( ! this->AreStatisticsPossible( *colIt )
                         || this->AreStatisticsUnnecessary( tableDesc, *colIt ) ) {
                        break;
                    }

                    const SAPDB_Bool ok =
                        this->InsertIntoColumnList(
                            *colIt, columnNumbers, stackCode, errMsg );

                    if ( ! ok ) {
                        return RequestSystemError;
                    }

                    break;
                }
            }
        }
    }

    return RequestOkay;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_UpdateStatistics::InsertIntoColumnList(
    const Catalog_ColumnDescriptor& colDesc,
    ColumnNumberVector&             columnNumbers,
    StackCodeVector&                stackCode,
    Msg_List&                       errMsg)
{
    if ( this->IsInColumnList( colDesc, columnNumbers ) ) {
        return true;
    }

    if ( (! columnNumbers.InsertEnd( colDesc.RecColNo() ) )
         || (! stackCode.InsertEnd( colDesc.GetStackCode() ) )
        ) {
        errMsg =
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_NO_MEMORY,
                Msg_OptArg(
                    "MEMREQUEST",
                    SAPDB_ToString(
                        sizeof( colDesc.RecColNo() )
                        + sizeof( colDesc.GetStackCode() ) ) )
                );
        return false;
    }

    return true;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_UpdateStatistics::IsInColumnList(
    const Catalog_ColumnDescriptor& colDesc,
    const ColumnNumberVector&       columnNumbers)
{
    ColumnNumberVector::ConstIterator colNoIt = columnNumbers.Begin();
    for ( ColumnNumberVector::ConstIterator colNoItEnd = columnNumbers.End();
          colNoIt != colNoItEnd;
          ++colNoIt ) {

        if ( *colNoIt == colDesc.RecColNo() ) {
            return true;
        }
    }
    return false;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_UpdateStatistics::UpdateTreeLeafNodes(
    const FileDir_Table& tableFile,
    const SAPDB_Int      leafPageCount)
{
    SQLMan_FileId tableFileId;

    tableFile.BuildFileId( tableFileId );
    tableFileId.fileLeafNodes_gg00() = leafPageCount;

    k64set_treeleafnodes(
        m_context.GetTransContext().OldTrans(), tableFileId );

    if ( m_context.GetTransContext().OldTrans().trError_gg00 != e_ok ) {
        return false;
    }

    // purge entry from local cache because the cached version has the
    // wrong treeleafcount
    tgg00_SysInfoKey syskey;
    syskey.sreclen       = cgg_rec_key_offset + mxak_standard_sysk;
    syskey.skeylen       = mxak_standard_sysk;
    syskey.svarcoloffset = 0;
    syskey.svarcolcnt    = 0;

    syskey.stableid().rawAssign( tableFile.GetFileNo().GetCharArray() );
    syskey.sentrytyp().rawAssign( cak_etable );
    syskey.slinkage().rawAssign( cak_init_linkage );

    a10_key_del( m_context, syskey );

    return m_context.IsOk();
}

/**************************************************************************/

SAPDB_Bool
SQLMan_UpdateStatistics::InternalTransInfo::NewTrans()
{
    a51_internal_trans( m_parent.m_context, m_oldTrans );
    if ( m_parent.m_context.IsOk()
         && ( m_parent.m_context.GetTransContext().OldTrans().trError_gg00 == e_ok) ) {
        m_internalTransContext = m_parent.m_context.TransContext();
        m_transSet = true;
        m_internalTransSet = true;
    }

    if ( ! m_parent.m_context.IsOk()
         || ( m_parent.m_context.GetTransContext().OldTrans().trError_gg00 != e_ok ) ) {
        m_parent.m_context.GetTransContext().OldTrans().trError_gg00 = e_ok;
        m_parent.m_context.ResetReturnCode();
        return false;
    }

    return true;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_UpdateStatistics::InternalTransInfo::Close(SAPDB_Bool doCommit)
{
    if ( m_transSet ) {
        this->SwitchToInternal();

        tgg00_MessType mType;
        if ( doCommit )
        {
            mType.becomes( m_commit );
        }
        else
        {
            mType.becomes( m_rollback );
        }
        m_parent.m_context.TransContext().trError_gg00 = e_ok;
        a51close_internal_trans( m_parent.m_context, m_oldTrans, mType );

        m_internalTransSet = false;
        m_transSet         = false;
    }

    if ( ! m_parent.m_context.IsOk()
         || ( m_parent.m_context.GetTransContext().OldTrans().trError_gg00 != e_ok) ) {
        m_parent.m_context.GetTransContext().OldTrans().trError_gg00 = e_ok;
        m_parent.m_context.ResetReturnCode();
        return false;
    }

    return true;
}

/**************************************************************************/

void
SQLMan_UpdateStatistics::InternalTransInfo::SwitchToInternal()
{
    if ( m_transSet && ! m_internalTransSet ) {
        m_parent.m_context.TransContext() = m_internalTransContext;
        m_internalTransSet       = true;
    }
}

/**************************************************************************/

void
SQLMan_UpdateStatistics::InternalTransInfo::SwitchToParent()
{
    if ( m_transSet && m_internalTransSet ) {
        m_parent.m_context.TransContext() = m_oldTrans.utrTrans();
        m_internalTransSet       = false;
    }
}

/**************************************************************************/

SQLMan_UpdateStatistics::InternalTransInfo::~InternalTransInfo()
{
    if ( m_transSet ) {
        SAPDB_Bool c_doCommit = true;
        this->Close( ! c_doCommit );
    }
}

/**************************************************************************/
