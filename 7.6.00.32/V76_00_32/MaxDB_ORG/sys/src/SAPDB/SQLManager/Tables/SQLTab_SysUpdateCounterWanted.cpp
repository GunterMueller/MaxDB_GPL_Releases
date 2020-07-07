/*****************************************************************************/
/*!
  @file         SQLTab_SysUpdateCounterWanted.cpp
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Header file for class SQLTab_SysUpdateCounterWanted.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
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

#include "SQLManager/Tables/SQLTab_SysUpdateCounterWanted.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"

#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Types.hpp"

#include "SQLManager/SQLMan_InternalTrans.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatementScope.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_Table.hpp"

#include "Transaction/Trans_Context.hpp"

#include "Messages/Msg_List.hpp"

#include "Messages/Msg_Ucs2Arg.hpp"

#include "hgg01_3.h" // g01glob

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

const SAPDB_UInt cReadChunkSize   = 200;

const SAPDB_Int cCommandCancelled = -102;

/*****************************************************************************/
/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::SQLTab_SysUpdateCounterWanted(
    SQLMan_Context& context)
    : m_context( context )
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::SQLTab_SysUpdateCounterWanted",
        SQLMan_Trace, 5);
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::DeleteRc
SQLTab_SysUpdateCounterWanted::DeleteRow(
    RequestIterator& requestIt,
    Msg_List&        errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::DeleteRow-requestIt",
        SQLMan_Trace, 5);

    SAPDBERR_ASSERT_STATE(
        requestIt.m_iterator != requestIt.m_requestFileNos.End() );

    if ( requestIt.m_iterator == requestIt.m_requestFileNos.End() ) {

        return delRcOk;

    }

    DeleteRc retCode = this->DeleteRow( *requestIt.m_iterator, errMsg );

    if ( retCode == delRcOk ) {

        requestIt.m_requestFileNos.Delete( requestIt.m_iterator );

    }

    return retCode;
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::DeleteRc
SQLTab_SysUpdateCounterWanted::DeleteRow(
    const FileDir_FileNo& requestFileNo,
    Msg_List&             errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::DeleteRow-requestFileNo",
        SQLMan_Trace, 5);

    SQLMan_Identifier schemaName;
    SQLMan_Identifier tableName;
    SQLMan_Identifier indexName;

    const GetKeyRc gkRc =
        SQLTab_SysUpdateCounterWanted::GetRequestKeyValues(
            m_context,
            requestFileNo,
            schemaName,
            tableName,
            indexName,
            errMsg );

    if ( gkRc != gkRcOk ) {

        return (DeleteRc) ( gkRc );

    }

    return this->DeleteRow( schemaName, tableName, indexName, errMsg );
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::DeleteRc
SQLTab_SysUpdateCounterWanted::DeleteRow(
    const SQLMan_Identifier&  schemaName,
    const SQLMan_Identifier&  tableName,
    const SQLMan_Identifier&  indexName,
    Msg_List&                 errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::DeleteRow-identifiers",
        SQLMan_Trace, 5);

    const char cStmtText[] =
        "Delete SysUpdateCounterWanted where"
        " schemaname = ?"
        " and tablename = ?"
        " and indexname = ?";

    KSQL_PreparedStatementScope stmtScope( m_context.GetAllocator() );

    if ( stmtScope.Initialize( m_context, cStmtText, errMsg )
        != KSQL_PreparedStatementScope::initRcOk ) {

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDATECOUNTERWANTED_INTERNAL_DELETE_STMT_FAILED )
            );
        return delRcSystemError;
    }

    SAPDB_Int colNo = 0;
    SAPDB_Int index = 0;
    bool      ok    = true;

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &schemaName );

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &tableName );

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &indexName );

    if ( ! ok ) {

        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_UPDATECOUNTERWANTED_INTERNAL_DELETE_PARAM_BIND_ERR
            );

        return delRcSystemError;
    }

    // statement execution
    KSQL_Statement::SQLCode sqlReturnCode = stmtScope.GetStatement().execute();

    if ( sqlReturnCode != 0 ) {

        switch ( sqlReturnCode ) {
            case 100:
            case -4004:
                return delRcRowNotFound;
            case cCommandCancelled:
                return delRcCancelled;
            case 500:
                return delRcLockRequestTimeout;
            default:

                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_DELETE_FAILED,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    sqlReturnCode ) ),

                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__RETURNCODE,
                                sqlReturnCode ) )
                    );

                return delRcSystemError;
        }

    }

    return delRcOk;
}

/*****************************************************************************/
/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::RequestIterator(
    SQLMan_Context& context)
    :
    m_context( context ),
    m_lastChunkRead( false ),
    m_requestFileNos( context.GetTransContext().Allocator() )
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::RequestIterator",
        SQLMan_Trace, 5);
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::ReturnCode
SQLTab_SysUpdateCounterWanted::RequestIterator::Initialize(
    Msg_List& errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::Initialize",
        SQLMan_Trace, 5);

    m_lastChunkRead = false;

    m_schemaKey.SetBlank();
    m_tableKey.SetBlank();
    m_indexKey.SetBlank();

    m_curFileNo.Invalidate();

    if ( ! m_requestFileNos.Reserve( cReadChunkSize ) ) {

        if ( ! m_requestFileNos.Reserve( cReadChunkSize / 2 ) ) {

            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_NO_MEMORY,
                Msg_OptArg( "MEMREQUEST",
                            sizeof( RequestList::ValueType )
                            * cReadChunkSize / 2) );

            return rcOutOfMemory;
        }

    }

    return this->ReadNextChunk( errMsg );
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::ReturnCode
SQLTab_SysUpdateCounterWanted::RequestIterator::Reinit(
    Msg_List& errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::Reinit",
        SQLMan_Trace, 5);

    return this->Initialize( errMsg );
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::ReturnCode
SQLTab_SysUpdateCounterWanted::RequestIterator::ReadNextChunk(
    Msg_List& errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::Reinit",
        SQLMan_Trace, 5);

    m_requestFileNos.Clear();
    m_iterator = m_requestFileNos.Begin();

    if ( m_lastChunkRead ) {

        return rcOk;

    }

    const SQLMan_Identifier cMySchema( g01glob.sysuser_name );
    const SQLMan_Identifier cMyName( SQLMan_Identifier("SYSUPDATECOUNTERWANTED", /*ASCIISource*/true) );

    const char cStmtText[] =
        "Select schemaname, tablename, indexname From"
        " SysUpdateCounterWanted"
        " Where"
        " schemaname > ?"
        " or (schemaname = ?"
        " and ((tablename = ? and indexname >= ?)"
        " or tablename > ?))"
        " Order by schemaname, tablename, indexname";

    KSQL_PreparedStatementScope stmtScope( m_context.GetAllocator() );

    const KSQL_PreparedStatementScope::InitRc stmtInitRc =
        stmtScope.Initialize( m_context, cStmtText, errMsg );

    if ( stmtInitRc != KSQL_PreparedStatementScope::initRcOk ) {
        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDATECOUNTERWANTED_INTERNAL_ITSELECT_STMT_FAILED
                ) );

        return rcSystemError;
    }

    SAPDB_Int colNo = 0;
    stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &m_schemaKey ); // schemaname
    stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &m_schemaKey ); // schemaname
    stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &m_tableKey );  // tablename
    stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &m_indexKey );  // tablename
    stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &m_tableKey );  // tablename

    // statement execution
    KSQL_Statement::SQLCode sqlReturnCode = stmtScope.GetStatement().execute();

    if ( sqlReturnCode != 0 ) {
        switch ( sqlReturnCode ) {
            case 100:
            case -4004:
                m_lastChunkRead = true;
                return rcOk;
            case cCommandCancelled:
                return rcCancelled;
            default:

                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_SELECT_FAILED,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    sqlReturnCode ) ),

                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__RETURNCODE,
                                sqlReturnCode ) )
                    );

                return rcSystemError;
        }
    }

    KSQL_ResultSet result = stmtScope.GetStatement().getResultSet();

    SQLMan_Identifier schemaName;
    SQLMan_Identifier tableName;
    SQLMan_Identifier indexName;
    const SAPDB_Bool cConsiderResultTables = true;
    const SAPDB_Bool cConsiderSYSDDTables = true;

    while ( (result.next() == 0)
            && (m_requestFileNos.GetSize() < m_requestFileNos.GetCapacity())
            && result.isOK() ) {

        SAPDB_Int idx = 1;

        result.getIdentifier( idx++, schemaName );
        result.getIdentifier( idx++, tableName );
        result.getIdentifier( idx, indexName );

        if ( (schemaName == m_schemaKey)
             && (tableName == m_tableKey)
             && (indexName == m_indexKey) ) {
            // skip row with last key
            continue;
        }

        if ( (schemaName == cMySchema)
             && (tableName == cMyName) ) {
            // silently skip requests related to table
            // SYSUPDATECOUNTERWANTED because this might lead to
            // deadlocks
            const ReturnCode inrc =
                this->RemoveBrokenRow(
                    schemaName,
                    tableName,
                    indexName,
                    errMsg );

            if ( inrc != rcOk ) {
                return inrc;
            }

            continue;
        }

        Catalog_TableDescriptor curTable(
            m_context,
            schemaName,
            tableName,
            ! cConsiderResultTables, ! cConsiderSYSDDTables );

        if ( ! curTable ) {

            if ( ! m_context.IsOk() ) {

                if ( m_context.ReturnCode() == cCommandCancelled ) {
                    return rcCancelled;
                }

                Msg_List  baseMsg  = m_context.GetErrorList();
                SAPDB_Int savedErr = m_context.ReturnCode();
                m_context.ResetReturnCode();

                baseMsg.MarkAsSubMessages();
                baseMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_IT_TABLE_CATALOG_ERR,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_TABLE_CATALOG_ERR__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    savedErr ) ),
                        Msg_Ucs2Arg<SQLMan_Identifier>(
                            SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_TABLE_CATALOG_ERR__SCHEMANAME,
                            schemaName ),
                        Msg_Ucs2Arg<SQLMan_Identifier>(
                            SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_TABLE_CATALOG_ERR__TABLENAME,
                            tableName ),
                        Msg_Arg(
                            SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_TABLE_CATALOG_ERR__RETURNCODE,
                            savedErr ) )
                        );

                errMsg = baseMsg;
                baseMsg.ClearMessageList();

                return rcSystemError;
            }

            const ReturnCode inrc =
                this->RemoveBrokenRow(
                    schemaName,
                    tableName,
                    indexName,
                    errMsg );

            if ( inrc != rcOk ) {
                return inrc;
            }

            continue;
        }

        FileDir_FileNo curFileNo;

        if ( indexName.IsBlank() ) {

            curFileNo = curTable.GetTableFileNo();

        } else {

            ReturnCode iniRc = this->GetIndexFileNo(
                curTable, indexName, curFileNo, errMsg );

            if ( ! curFileNo.IsValid() ) {

                if ( iniRc != rcOk ) {
                    return iniRc;
                }

                iniRc = this->RemoveBrokenRow(
                    schemaName,
                    tableName,
                    indexName,
                    errMsg );

                if ( iniRc != rcOk ) {
                    return iniRc;
                }

                continue;
            }

        }

        SAPDBERR_ASSERT_STATE( curFileNo.IsValid() );

        const SAPDB_Bool insertOk = m_requestFileNos.InsertEnd( curFileNo );
        if ( ! insertOk ) {

            if ( m_requestFileNos.GetSize() == 0 ) {
                return rcOutOfMemory;
            } else {
                break;
            }

        }
    }

    m_schemaKey = schemaName;
    m_tableKey  = tableName;
    m_indexKey  = indexName;

    m_iterator = m_requestFileNos.Begin();

    return rcOk;
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::ReturnCode
SQLTab_SysUpdateCounterWanted::RequestIterator::Next(
    FileDir_FileNo& fileNo,
    Msg_List&       errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::Next",
        SQLMan_Trace, 5);

    if ( m_iterator != m_requestFileNos.End() ) {

        fileNo      = *m_iterator;
        m_curFileNo = *m_iterator;

        ++m_iterator;

        return rcOk;

    } else {

        fileNo.Invalidate();
        m_curFileNo.Invalidate();

        const ReturnCode rc = this->ReadNextChunk( errMsg );
        if ( rc == rcOk ) {

            if ( m_iterator != m_requestFileNos.End() ) {

                fileNo      = *m_iterator;
                m_curFileNo = *m_iterator;

                return rcOk;

            }

        }

        return rc;

    }
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::ReturnCode
SQLTab_SysUpdateCounterWanted::RequestIterator::GetIndexFileNo(
    Catalog_TableDescriptor& tableDesc,
    const SQLMan_Identifier& indexName,
    FileDir_FileNo&          indexFileNo,
    Msg_List&                errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::GetIndexFileNo",
        SQLMan_Trace, 5);

    indexFileNo.Invalidate();

    Catalog_IndexIterator indexIt( tableDesc.GetIndexIterator() );

    while ( indexIt ) {

        if ( (*indexIt).GetName() == indexName ) {

            const Catalog_IndexDescriptor::GetIndexFileNoRc gifnRc =
                (*indexIt).GetIndexFileNo( indexFileNo, errMsg );

            if ( gifnRc == Catalog_IndexDescriptor::gifnRcSystemError ) {
                return rcSystemError;
            }

            return rcOk;

        }

        ++indexIt;
    }

    if ( ! m_context.IsOk() ) {

        switch ( m_context.ReturnCode() ) {
            case cCommandCancelled:
                return rcCancelled;
            default:

                Msg_List  baseMsg  = m_context.GetErrorList();
                SAPDB_Int savedErr = m_context.ReturnCode();
                m_context.ResetReturnCode();

                baseMsg.MarkAsSubMessages();
                baseMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_IT_INDEX_CATALOG_ERR,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_INDEX_CATALOG_ERR__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    savedErr ) ),

                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_INDEX_CATALOG_ERR__RETURNCODE,
                                savedErr ),
                        Msg_Ucs2Arg<SQLMan_Identifier>(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_INDEX_CATALOG_ERR__INDEXNAME,
                                                       indexName ),
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_IT_INDEX_CATALOG_ERR__TABID,
                                FileDir_FileNoToString( tableDesc.GetTableId() ) )
                        ) );

                errMsg = baseMsg;
                baseMsg.ClearMessageList();

                return rcSystemError;
        }

    }

    return rcOk;
}

/*****************************************************************************/

SAPDB_Bool
SQLTab_SysUpdateCounterWanted::RequestIterator::IsEmpty() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::IsEmpty",
        SQLMan_Trace, 5);

    return m_requestFileNos.GetSize() == 0;
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::ReturnCode
SQLTab_SysUpdateCounterWanted::RequestIterator::RemoveBrokenRow(
    const SQLMan_Identifier& schemaName,
    const SQLMan_Identifier& tableName,
    const SQLMan_Identifier& indexName,
    Msg_List&                errMsg )
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::RequestIterator::RemoveBrokenRow",
        SQLMan_Trace, 5);

    SQLMan_InternalTrans internalTrans( m_context );

    SAPDB_Bool transOk = internalTrans.NewTrans( errMsg );
    SAPDBERR_ASSERT_STATE( transOk );

    SQLTab_SysUpdateCounterWanted table( m_context );

    const DeleteRc delRc =
        table.DeleteRow( schemaName, tableName, indexName, errMsg );

    if ( delRc != delRcOk ) {

        switch ( delRc ) {
            case delRcRowNotFound:
            case delRcLockRequestTimeout:
                break;
            case delRcCancelled:
                internalTrans.Close(
                    SQLMan_InternalTrans::doRollback,
                     errMsg );

                return rcCancelled;
            default:
                Msg_List  baseMsg  = m_context.GetErrorList();
                SAPDB_Int savedErr = m_context.ReturnCode();
                m_context.ResetReturnCode();

                baseMsg.MarkAsSubMessages();
                baseMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    savedErr ) ),
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__RETURNCODE,
                                savedErr ),
                        Msg_Ucs2Arg<SQLMan_Identifier>(
                            SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__SCHEMANAME,
                            schemaName ),
                        Msg_Ucs2Arg<SQLMan_Identifier>(
                            SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__TABLENAME,
                            tableName ),
                        Msg_Ucs2Arg<SQLMan_Identifier>(
                            SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__INDEXNAME,
                            indexName )
                        ) );

                errMsg = baseMsg;

                internalTrans.Close(
                    SQLMan_InternalTrans::doRollback,
                     baseMsg );

                baseMsg.MarkAsSubMessages();

                baseMsg.Overrule( errMsg );

                errMsg = baseMsg;

                return rcSystemError;
        }
    }

    if ( internalTrans.Close( SQLMan_InternalTrans::doCommit, errMsg ) ) {

        return rcOk;

    } else {

        Msg_List  savedMsg = m_context.GetErrorList();
        SAPDB_Int savedErr = m_context.ReturnCode();
        m_context.ResetReturnCode();

        errMsg = savedMsg;
        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR,
                Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__ERRORTEXT,
                        SQLManErr_Interface::GetInstance().GetErrorText(
                            savedErr ) ),
                Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__RETURNCODE,
                        savedErr ),
                Msg_Ucs2Arg<SQLMan_Identifier>(
                    SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__SCHEMANAME,
                    schemaName ),
                Msg_Ucs2Arg<SQLMan_Identifier>(
                    SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__TABLENAME,
                    tableName ),
                Msg_Ucs2Arg<SQLMan_Identifier>(
                    SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_REMOVE_BROKEN_ERR__INDEXNAME,
                    indexName )
                ) );

        return rcSystemError;

    }
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::RequestIterator::ReturnCode
SQLTab_SysUpdateCounterWanted::RequestIterator::DeleteBrokenRow(
    Msg_List& errMsg)
{
    SQLMan_InternalTrans internalTrans( m_context );

    SAPDB_Bool transOk = internalTrans.NewTrans( errMsg );
    SAPDBERR_ASSERT_STATE( transOk );

    SQLTab_SysUpdateCounterWanted table( m_context );

    /*
     * we do not have to delete the row from the iterator itself
     * because we only support access via our Next() method, which will
     * return the next entry
     */
    const DeleteRc delRc =
        table.DeleteRow( *m_iterator, errMsg );

    if ( delRc != delRcOk ) {

        switch ( delRc ) {
            case delRcRowNotFound:
            case delRcLockRequestTimeout:
                break;
            case delRcCancelled:
                internalTrans.Close(
                    SQLMan_InternalTrans::doRollback,
                     errMsg );

                return rcCancelled;
            default:
                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_DELETE_BROKEN_ERR,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_DELETE_BROKEN_ERR__FILENO,
                                FileDir_FileNoToString( *m_iterator ) )
                        ) );


                Msg_List baseMsg;
                internalTrans.Close(
                    SQLMan_InternalTrans::doRollback,
                     baseMsg );

                baseMsg.MarkAsSubMessages();

                baseMsg.Overrule( errMsg );

                errMsg = baseMsg;
                baseMsg.ClearMessageList();

                return rcSystemError;
        }
    }

    if ( internalTrans.Close( SQLMan_InternalTrans::doCommit, errMsg ) ) {

        return rcOk;

    } else {

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDATECOUNTERWANTED_DELETE_BROKEN_ERR,
                Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_DELETE_BROKEN_ERR__FILENO,
                        FileDir_FileNoToString( *m_iterator ) )
                ) );

        return rcSystemError;

    }
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::IncrementRc
SQLTab_SysUpdateCounterWanted::IncrementUpdateTryCount(
    const FileDir_FileNo& requestFileNo,
    Msg_List&             errMsg)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLTab_SysUpdateCounterWanted::IncrementUpdateTryCount",
        SQLMan_Trace, 5);

    SQLMan_Identifier schemaName;
    SQLMan_Identifier tableName;
    SQLMan_Identifier indexName;

    const GetKeyRc gkRc =
        SQLTab_SysUpdateCounterWanted::GetRequestKeyValues(
            m_context,
            requestFileNo,
            schemaName,
            tableName,
            indexName,
            errMsg );

    if ( gkRc != gkRcOk ) {

        return (IncrementRc) gkRcOk;

    }


    const char cStmtText[] =
        "Update SysUpdateCounterWanted "
        " Set CancelCount = CancelCount + 1"
        " where"
        " schemaname = ?"
        " and tablename = ?"
        " and indexname = ?";

    KSQL_PreparedStatementScope stmtScope( m_context.GetAllocator() );

    if ( stmtScope.Initialize( m_context, cStmtText, errMsg )
        != KSQL_PreparedStatementScope::initRcOk ) {

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDATECOUNTERWANTED_INTERNAL_DELETE_STMT_FAILED )
            );
        return incRcSystemError;
    }

    SAPDB_Int colNo = 0;
    SAPDB_Int index = 0;
    bool      ok    = true;

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &schemaName );

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &tableName );

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &indexName );

    if ( ! ok ) {

        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_UPDATECOUNTERWANTED_INTERNAL_DELETE_PARAM_BIND_ERR
            );

        return incRcSystemError;
    }

    // statement execution
    KSQL_Statement::SQLCode sqlReturnCode = stmtScope.GetStatement().execute();

    if ( sqlReturnCode != 0 ) {

        switch ( sqlReturnCode ) {
            case 100:
            case -4004:
                return incRcRowNotFound;
            case cCommandCancelled:
                return incRcCancelled;
            case 500:
                return incRcLockRequestTimeout;
            default:

                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_DELETE_FAILED,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    sqlReturnCode ) ),

                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__RETURNCODE,
                                sqlReturnCode ) )
                    );

                return incRcSystemError;
        }

    }

    return incRcOk;
}

/*****************************************************************************/

SQLTab_SysUpdateCounterWanted::GetKeyRc
SQLTab_SysUpdateCounterWanted::GetRequestKeyValues(
    SQLMan_Context&       context,
    const FileDir_FileNo& requestFileNo,
    SQLMan_Identifier&    schemaName,
    SQLMan_Identifier&    tableName,
    SQLMan_Identifier&    indexName,
    Msg_List&             errMsg)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "SQLTab_SysUpdateCounterWanted::GetKey", SQLMan_Trace, 5);

    FileDir_IDirectory& fDir = FileDir_IDirectory::SessionInstance(
        context.TransContext() );

    FileDir_Generic fileEntry;

    FileDir_ReturnCode rc = fDir.GcGetFile(
        context.GetTransContext().TaskId(),
        requestFileNo,
        fileEntry,
        errMsg );

    if ( rc != FileDir_Okay ) {

        switch ( rc ) {
            case FileDir_FileNoNotFound:
                return gkRcRowNotFound;
            default:
                // message should be set by fdir
                return gkRcSystemError;
        }
    }

    FileDir_Index indexEntry;
    FileDir_FileNo tableFileNo = requestFileNo;

    if ( fileEntry.GetFileType() == Data_IndexFile ) {

        indexEntry.Assign( fileEntry );
        tableFileNo = indexEntry.GetParentFileNo();

    }

    Catalog_TableDescriptor tableDesc( context, tableFileNo );
    if ( ! tableDesc ) {

        switch ( context.ReturnCode() ) {
            case cCommandCancelled:
                return gkRcCancelled;
            case 100:
                return gkRcRowNotFound;
            default:
                Msg_List  baseMsg  = context.GetErrorList();
                SAPDB_Int savedErr = context.ReturnCode();
                context.ResetReturnCode();

                baseMsg.MarkAsSubMessages();
                baseMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    savedErr ) ),
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__TABID,
                                FileDir_FileNoToString( tableFileNo ) ),
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_TABLE_CATALOG_ERR__RETURNCODE,
                                savedErr )
                        )
                    );

                errMsg = baseMsg;
                baseMsg.ClearMessageList();

                return gkRcSystemError;
        }

    }

    schemaName = tableDesc.GetSchemaName();
    tableName  = tableDesc.GetTableName();

    if ( indexEntry.IsAssigned() ) {

        Catalog_IndexDescriptor indexDesc = tableDesc.GetIndexDescriptor(
            indexEntry.GetIndexId() );

        if ( ! indexDesc ) {

            switch ( context.ReturnCode() ) {
                case cCommandCancelled:
                    return gkRcCancelled;
                case 100:
                    return gkRcRowNotFound;
                default:
                    Msg_List  savedMsg = context.GetErrorList();
                    SAPDB_Int savedErr = context.ReturnCode();
                    context.ResetReturnCode();

                    errMsg = savedMsg;
                    errMsg.Overrule(
                        Msg_List(
                            Msg_List::Error,
                            SDBMSG_SQLMAN_UPDATECOUNTERWANTED_INDEX_CATALOG_ERR,
                            Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_INDEX_CATALOG_ERR__ERRORTEXT,
                                    SQLManErr_Interface::GetInstance().GetErrorText(
                                        savedErr ) ),
                            Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_INDEX_CATALOG_ERR__INDEX,
                                    indexEntry.GetIndexId() ),
                            Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_INDEX_CATALOG_ERR__TABID,
                                    FileDir_FileNoToString( tableFileNo ) ),
                            Msg_Arg(SDBMSGTAG_SQLMAN_UPDATECOUNTERWANTED_INDEX_CATALOG_ERR__RETURNCODE,
                                    savedErr ) )
                        );

                    return gkRcSystemError;
            }
        }

        indexName = indexDesc.GetName();
    }

    return gkRcOk;
}

/*****************************************************************************/
