/*****************************************************************************/
/*!
  @file         SQLTab_SysUpdStatWanted.cpp
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Implementation file for class SQLTab_SysUpdStatWanted.

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

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Tables/SQLTab_SysUpdStatWanted.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"

#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_ColumnDescriptor.hpp"
#include "SQLManager/SQLMan_InternalTrans.hpp"
#include "SQLManager/SQLMan_ChangedUserScope.hpp"
#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/SQLMan_ChangedUserScope.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatementScope.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"

#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"


#include "hbd01.h"
#include "hbd02.h"
#include "hak01.h"
#include "hak06.h"
#include "hak28.h"
#include "ggg00.h"
#include "hgg01.h" // g01mblock*
#include "hgg01_2.h" // g01event_init
#include "hgg01_3.h" // g01unicode, g01glob
#include "heo67.h" // vinsert_event

/*===========================================================================*
 *  CODE                                                                     *
 *===========================================================================*/

namespace {
/// SQL return code for cancelled command
const SAPDB_Int cCommandCancelled = -102;

/// layout of record in SYSUPDSTATWANTED (no column compression)
struct RecordLayout {
    SAPDB_Int2 recLen;
    SAPDB_Int2 keyLen;
    SAPDB_Int2 varColCount;
    SAPDB_Int2 varColOffset;
    SAPDB_Byte defByteKey;
    SAPDB_Byte schemaName[ sizeof(SQLMan_Identifier) ];
    SAPDB_Byte defByteKey1;
    SAPDB_Byte ownerName[ sizeof(SQLMan_Identifier) ];
    SAPDB_Byte defByteKey2;
    SAPDB_Byte tableName[ sizeof(SQLMan_Identifier) ];
    SAPDB_Byte defByteKey3;
    SAPDB_Byte columnName[ sizeof(SQLMan_Identifier) ];
};

} // end anonymous namespace

/**************************************************************************/

SQLTab_SysUpdStatWanted::SQLTab_SysUpdStatWanted(SQLMan_Context& context)
    : m_context( context )
{}

/**************************************************************************/

SQLMan_Identifier
SQLTab_SysUpdStatWanted::GetSchema() const
{
    return SQLMan_Identifier( g01glob.sysuser_name );
}

/**************************************************************************/

SAPDB_Bool
SQLTab_SysUpdStatWanted::GetSchema(
    SQLMan_Identifier& schemaName,
    Msg_List&          errMsg)
{
    schemaName = SQLMan_Identifier( g01glob.sysuser_name );
    return true;
}

/**************************************************************************/

SQLMan_Identifier
SQLTab_SysUpdStatWanted::GetName() const
{
    return SQLMan_Identifier( a01_i_sysupdstatwanted );
}

/**************************************************************************/

SAPDB_Bool
SQLTab_SysUpdStatWanted::GetName(
    SQLMan_Identifier& name,
    Msg_List&          errMsg)
{
    name = this->GetName();
    return true;
}

/**************************************************************************/

void
SQLTab_SysUpdStatWanted::CheckAndCreateUpdStatEvent()
{
    Catalog_TableDescriptor myTableDesc(
        m_context,
        this->GetSchema(),
        this->GetName(),
        /*ConsiderResultTables*/ false,
        /*ConsiderSYSDDTables*/  false );
    if ( ! myTableDesc ) {
        SAPDBERR_ASSERT_STATE( false );
        m_context.ResetReturnCode();
        return;
    }

    Msg_List errMsg;
    SAPDB_Int recordCount;
    if ( myTableDesc.GetRecordCount( recordCount, errMsg )
         != Catalog_TableDescriptor::gfcRcOk ) {
        SAPDBERR_ASSERT_STATE( false );
        errMsg.ClearMessageList();
        return;
    }

    if ( recordCount == 1 ) {
        tsp31_event_description eventDescription;

        g01event_init( eventDescription);
        eventDescription.sp31ed_ident.becomes( sp31ei_upd_stat_wanted );
        eventDescription.sp31ed_text_len = 25;
        eventDescription.sp31ed_priority.becomes( sp31ep_low );
        eventDescription.sp31ed_text_value.rawAssign(
            "UPDATE STATISTICS WANTED      " );
        vinsert_event( eventDescription );
    }
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::RequestColumnIterator
SQLTab_SysUpdStatWanted::GetRequestColumnIterator(
    const Catalog_TableDescriptor& tableDesc) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::GetRequestColumnIterator", SQLMan_Trace, 5);

    return RequestColumnIterator(
        m_context.TransContext(), tableDesc );
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::RequestColumnIterator::RequestColumnIterator(
    SQLMan_TransContext&     transContext,
    const Catalog_TableDescriptor& tableDesc)
    : m_trans( transContext ),
      m_tableDesc( &tableDesc ),
      m_firstCall( true )
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::RequestColumnIterator::RequestColumnIterator",
        SQLMan_Trace, 5);

    if ( ! tableDesc ) {
        m_tableDesc = 0;
        return;
    }

    SQLTab_SysUpdStatWanted::BuildKey(
        m_tableDesc->GetSchemaName(),
        m_tableDesc->GetOwnerName(),
        m_tableDesc->GetTableName(),
        SQLMan_Identifier(),
        m_curKey);
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::RequestColumnIterator::RequestColumnIterator(
    const RequestColumnIterator& it)
    : m_trans( it.m_trans )
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::RequestColumnIterator::copy-const",
        SQLMan_Trace, 5);

    if ( &it == this ) {
        return;
    }

    m_curKey    = it.m_curKey;
    m_tableDesc = it.m_tableDesc;
}

/**************************************************************************/

SAPDB_Bool
SQLTab_SysUpdStatWanted::RequestColumnIterator::Next(
    SQLMan_Identifier& columnName)
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::RequestColumnIterator::Next",
        SQLMan_Trace, 5);

    tgg00_Rec recBuf;

    if ( ( memcmp (
               a28updstatwanted_tree.fileName_gg00(),
               cgg_zero_fn,
               sizeof( a28updstatwanted_tree.fileName_gg00() ) ) == 0 )
         || ( m_tableDesc == 0 ) ) {
        return false;
    }

    // retrieve record, check for exact key during first call

    // do not use a28updstatwanted_tree directly because we do not
    // want its root page set (and get e_old_fileversion later on)
    SAPDBERR_ASSERT_STATE( a28updstatwanted_tree.fileRoot_gg00() == NIL_PAGE_NO_GG00 );
    tgg00_FileId htreeId = a28updstatwanted_tree;

    b02next_record(
        m_trans,
        htreeId,
        m_curKey,
        m_firstCall,
        recBuf );

    if ( m_firstCall && (m_trans.trError_gg00 == e_key_not_found) ) {
        m_trans.trError_gg00 = e_ok;
    }
    m_firstCall = false;

    if ( (m_trans.trError_gg00 == e_no_next_record)
         || (m_trans.trError_gg00 == e_key_not_found) ) {
        m_trans.trError_gg00 = e_ok;
        return false;
    }

    if ( m_trans.trError_gg00 != e_ok ) {
        // FIXME: set errMsg
        SAPDBERR_ASSERT_STATE( false );
        m_trans.trError_gg00 = e_ok;
        return false;
    }

    m_trans.trError_gg00 = e_ok;

    // update key
    SAPDB_MemCopyNoCheck(
        &m_curKey, recBuf.recBuf_gg00(),
        recBuf.recKeyLen_gg00() + cgg_rec_key_offset );

    SQLMan_Identifier schemaName;
    SQLMan_Identifier ownerName;
    SQLMan_Identifier tableName;
    SQLTab_SysUpdStatWanted::ParseRecord(
        recBuf,
        schemaName,
        ownerName,
        tableName,
        columnName);

    if ( ( schemaName != m_tableDesc->GetSchemaName() )
         || ( ownerName != m_tableDesc->GetOwnerName() )
         || ( tableName != m_tableDesc->GetTableName() ) ) {
        return false;
    }

    return true;
}

/**************************************************************************/

void SQLTab_SysUpdStatWanted::BuildKey(
    const SQLMan_Identifier& schemaName,
    const SQLMan_Identifier& ownerName,
    const SQLMan_Identifier& tableName,
    const SQLMan_Identifier& columnName,
    tgg00_Lkey&              key)
{
    SAPDBTRACE_ROUTINE_DEBUG (
        "SQLTab_SysUpdStatWanted::BuildKey", SQLMan_Trace, 5 );

    SAPDB_Byte defByte;
    if ( g01unicode ) {
        defByte = csp_unicode_def_byte;
    } else {
        defByte = csp_ascii_blank;
    }

    const SAPDB_Int colNameOffset =
        cgg_rec_key_offset + 3 * sizeof( SQLMan_Identifier ) + 4;

    RecordLayout* rec = reinterpret_cast<RecordLayout*>( &key );

    rec->defByteKey  = defByte;
    rec->defByteKey1 = defByte;
    rec->defByteKey2 = defByte;
    rec->defByteKey3 = defByte;

    SAPDB_MemCopyNoCheck(
        rec->schemaName, &schemaName, sizeof( schemaName ) );

    SAPDB_MemCopyNoCheck(
        rec->ownerName, &ownerName, sizeof( ownerName ) );

    SAPDB_MemCopyNoCheck(
        rec->tableName, &tableName, sizeof( tableName ) );

    SAPDB_MemCopyNoCheck(
        rec->columnName, &columnName, columnName.GetLength() );

    rec->keyLen = static_cast<SAPDB_Int2>(
        3 * sizeof( SQLMan_Identifier ) + 4 + columnName.GetLength() );

    rec->recLen = cgg_rec_key_offset + rec->keyLen;
    rec->varColCount  = 0;
    rec->varColOffset = 0;
}

/**************************************************************************/

void SQLTab_SysUpdStatWanted::ParseRecord(
    const tgg00_Rec&    recBuf,
    SQLMan_Identifier&  schemaName,
    SQLMan_Identifier&  ownerName,
    SQLMan_Identifier&  tableName,
    SQLMan_Identifier&  columnName)
{
    SAPDBTRACE_ROUTINE_DEBUG (
        "SQLTab_SysUpdStatWanted::ParseRecord", SQLMan_Trace, 5 );

    const SAPDB_Int colNameOffset =
        cgg_rec_key_offset + 3 * sizeof( SQLMan_Identifier ) + 4;

    const RecordLayout* rec =
        reinterpret_cast<const RecordLayout*>( &recBuf );

    SAPDB_MemCopyNoCheck(
        &schemaName, rec->schemaName, sizeof( schemaName ) );

    SAPDB_MemCopyNoCheck(
        &ownerName, rec->ownerName, sizeof( ownerName ) );

    SAPDB_MemCopyNoCheck(
        &tableName, rec->tableName, sizeof( tableName ) );

    columnName.SetBlank();
    SAPDB_MemCopyNoCheck(
        &columnName, rec->columnName, rec->recLen - colNameOffset );
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::InsertRc
SQLTab_SysUpdStatWanted::InsertRow(
    Catalog_TableDescriptor& tableDesc,
    const SQLMan_Identifier& columnName,
    Msg_List&                errMsg)
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::InsertRow-tableDescColName",
        SQLMan_Trace, 5 );

    if ( columnName.IsBlank()
         && tableDesc.GetSysUpdStatWantedInsertedFlag() ) {
        return irOkay;
    }

    if ( tableDesc.IsArchiveTable() ) {
        return irNoStatisticsPossible;
    }

    if ( tableDesc.GetStatisticsSamplingInfo() == 0 ) {
        return irOkay;
    }

    SQLMan_Identifier schemaName( tableDesc.GetSchemaName() );
    SQLMan_Identifier ownerName( tableDesc.GetOwnerName() );
    SQLMan_Identifier tableName( tableDesc.GetTableName() );

    const InsertRc rc = this->InsertRow(
        schemaName, ownerName, tableName, columnName, errMsg );

    if ( (rc == irOkay) ) {
        tableDesc.SetSysUpdStatWantedInsertedFlag( true );
    } else {
        switch ( rc ) {
            case irFailed:
            case irLockRowFailed:
            case irCancelled:
                return rc;
            case irSystemError:
            default:
                return irSystemError;
        }
    }

    this->CheckAndCreateUpdStatEvent();

    return irOkay;
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::InsertRc
SQLTab_SysUpdStatWanted::InsertRow(
    Catalog_TableDescriptor& tableDesc,
    Msg_List&                errMsg)
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::InsertRow-tableDesc", SQLMan_Trace, 5 );

    return
        this->InsertRow( tableDesc, SQLMan_Identifier(), errMsg );
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::InsertRc
SQLTab_SysUpdStatWanted::InsertRow(
    Catalog_TableDescriptor&        tableDesc,
    const Catalog_ColumnDescriptor& columnDesc,
    Msg_List&                       errMsg)
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::InsertRow-tableColDesc", SQLMan_Trace, 5 );

    if ( columnDesc.IsLongDataType() ) {
        return irNoStatisticsPossible;
    }

    SQLMan_Identifier columnName( columnDesc.GetColumnName() );

    if ( columnDesc.IsSysKey() ) {
        // just enter request for table itself
        columnName.SetBlank();
    }

    return
        this->InsertRow( tableDesc, columnName, errMsg );
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::InsertRc
SQLTab_SysUpdStatWanted::InsertRow(
    const SQLMan_Identifier&  schemaName,
    const SQLMan_Identifier&  ownerName,
    const SQLMan_Identifier&  tableName,
    const SQLMan_Identifier&  columnName,
    Msg_List&                 errMsg)
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::InsertRow", SQLMan_Trace, 5 );

    if ( (tableName == this->GetName())
         && (schemaName == this->GetSchema()) ) {
        // ignore requests to enter ourself; this will easily deadlock
        // on the catalog records
        return irOkay;
    }

    if ( (m_context.a_proc_compile != pct_none)
         || (m_context.a_init_ddl != no_ddl) ) {
        // stacked SQL support is flaky, so better avoid it
        return irOkay;
    }

    SQLMan_ChangedUserScope changedUserScope(
        m_context, this->GetSchema() );
    if ( ! m_context.IsOk() ) {
        return irSystemError;
    }

    SQLMan_InternalTrans internalTrans( m_context );

    SAPDB_Bool transOk = internalTrans.NewTrans( errMsg );

    // TODO: don't insert via mess block but with internal SQL in
    // commented block
    if ( this->InsertRowWithMessBlock(
             schemaName, ownerName, tableName, columnName )
         != irOkay ) {
        internalTrans.Close( SQLMan_InternalTrans::doRollback, errMsg );
        errMsg.ClearMessageList();
        return irFailed;
    }

//     char cStmtText[] =
//         "INSERT SYSDBA.SYSUPDSTATWANTED"
//         " (schemaname, owner, tablename, columnname)"
//         " VALUES"
//         " (?, ?, ?, ?) NOWAIT";

//     KSQL_PreparedStatementScope stmtScope( m_context.GetAllocator() );

//     if ( stmtScope.Initialize( m_context, cStmtText, errMsg )
//         != KSQL_PreparedStatementScope::initRcOk ) {
//         errMsg.Overrule(
//             Msg_List(
//                 Msg_List::Error,
//                 SDBMSG_SQLMAN_UPDSTATWANTED_INTERNAL_INSERT_STMT_FAILED )
//             );
//         Msg_List savedMsg = errMsg;
//         internalTrans.Close( SQLMan_InternalTrans::doRollback, errMsg );
//         errMsg.Overrule( savedMsg );

//         return irSystemError;
//     }

//     SAPDB_Int colNo = 0;
//     SAPDBFields_AttributedField curField;

//     stmtScope.GetStatement().bindIdentifier4Input(
//         ++colNo, const_cast<SQLMan_Identifier*>( &schemaName ) );

//     stmtScope.GetStatement().bindIdentifier4Input(
//         ++colNo, const_cast<SQLMan_Identifier*>( &ownerName ) );

//     stmtScope.GetStatement().bindIdentifier4Input(
//         ++colNo, const_cast<SQLMan_Identifier*>( &tableName ) );

//     stmtScope.GetStatement().bindIdentifier4Input(
//         ++colNo, const_cast<SQLMan_Identifier*>( &columnName ) );

//     // statement execution
//     KSQL_Statement::SQLCode sqlReturnCode = stmtScope.GetStatement().execute();
//     if ( sqlReturnCode != 0 ) {
//         InsertRc rc;
//         switch ( sqlReturnCode ) {
//             case -102:
//                 rc = irCancelled;
//                 break;
//             case -20:
//             case 200:
//                 // duplicate key can be ignored
//                 rc = irOkay;
//                 break;
//             case 400:
//             case 500:
//                 rc = irLockRowFailed;
//                 break;
//             default:
//                 errMsg = Msg_List(
//                     Msg_List::Warning,
//                     SDBMSG_SQLMAN_UPDSTATWANTED_INSERT_FAILED,
//                     Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTATWANTED_INSERT_FAILED__ERRORTEXT,
//                              SQLManErr_Interface::GetInstance().GetErrorText(
//                                  sqlReturnCode ) )
//                     );
//                 rc = irFailed;
//         }

//         // in any case there is nothing to commit, so roll back
//         Msg_List savedMsg = errMsg;
//         internalTrans.Close( SQLMan_InternalTrans::doRollback, errMsg );
//         errMsg.Overrule( savedMsg );
//         return rc;
//     }

    if ( ! internalTrans.Close( SQLMan_InternalTrans::doCommit, errMsg ) ) {
        return irSystemError;
    }

    return irOkay;
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::InsertRc
SQLTab_SysUpdStatWanted::InsertRowWithMessBlock(
    const SQLMan_Identifier&  schemaName,
    const SQLMan_Identifier&  ownerName,
    const SQLMan_Identifier&  tableName,
    const SQLMan_Identifier&  columnName)
{
    tgg00_BasisError bErr;
    tgg00_StackEntry stack[2];
    tgg00_MessBlock  mblock;
    tgg00_QualBuf    qual;
    tgg00_Rec        data;
    Msg_List         errMsg;

    Catalog_TableDescriptor myTableDesc(
        m_context,
        this->GetSchema(),
        this->GetName(),
        /*ConsiderResultTables*/ false,
        /*ConsiderSYSDDTables*/  false );
    if ( ! myTableDesc ) {
        errMsg.ClearMessageList();
        return irFailed;
    }

    g01mblock_init( m_context.TransContext(), m_insert, mm_nil, mblock );
    g01stack_init(
        mblock,
        (tgg00_StackListPtr) stack,
        sizeof( stack ),
        &qual,
        sizeof( qual ) );
    g01datapart_init(
        mblock,
        (tgg00_DataPartPtr) &data,
        sizeof( data ) );

    const Catalog_TableDescriptor::BuildFileIdRc bfRc =
        myTableDesc.BuildFileId( mblock.mb_qual()->mtree(), errMsg );
    if ( bfRc != Catalog_TableDescriptor::bfRcOk ) {
        errMsg.ClearMessageList();
        return irFailed;
    }

    mblock.mb_qual()->mtree().fileHandling_gg00() =
        tgg00_HandlingSet::build( 1, hsNoWait_egg00 );
    mblock.mb_data()->mbp_varcol_offset() = 0;
    mblock.mb_data()->mbp_varcol_cnt()    = 0;
    SQLTab_SysUpdStatWanted::BuildKey(
        schemaName,
        ownerName,
        tableName,
        columnName,
        mblock.mb_data()->mbp_key() );
    mblock.mb_data()->mbp_rec().len() =
        cgg_rec_key_offset + mblock.mb_data()->mbp_keylen();

    a06lsend_mess_buf( m_context, mblock, false, bErr );

    m_context.ResetReturnCode();
    return irOkay;
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::DeleteRc
SQLTab_SysUpdStatWanted::DeleteRow(
    Catalog_TableDescriptor&        tableDesc,
    const Catalog_ColumnDescriptor& columnDesc,
    Msg_List&                       errMsg)
{
    return this->DeleteRow(
        tableDesc.GetSchemaName(),
        tableDesc.GetOwnerName(),
        tableDesc.GetTableName(),
        columnDesc.GetColumnName(),
        errMsg );
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::DeleteRc
SQLTab_SysUpdStatWanted::DeleteRow(
    Catalog_TableDescriptor& tableDesc,
    Msg_List&                errMsg)
{
    const DeleteRc rc =
        this->DeleteRow(
            tableDesc.GetSchemaName(),
            tableDesc.GetOwnerName(),
            tableDesc.GetTableName(),
            SQLMan_Identifier(),
            errMsg );

    if ( rc == delRcOkay ) {
        tableDesc.SetSysUpdStatWantedInsertedFlag( false );
    }

    return rc;
}

/**************************************************************************/

SQLTab_SysUpdStatWanted::DeleteRc
SQLTab_SysUpdStatWanted::DeleteRow(
    const SQLMan_Identifier&  schemaName,
    const SQLMan_Identifier&  ownerName,
    const SQLMan_Identifier&  tableName,
    const SQLMan_Identifier&  columnName,
    Msg_List&                 errMsg)
{
    SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysUpdStatWanted::DeleteRow-identifiers", SQLMan_Trace, 5 );

    const char cStmtText[] =
        "Delete SysUpdStatWanted where"
        " schemaname = ?"
        " and owner = ?"
        " and tablename = ?"
        " and columnname = ?";

    SQLMan_ChangedUserScope sysdbaScope( m_context, g01glob.sysuser_name );
    KSQL_PreparedStatementScope stmtScope( m_context.GetAllocator() );

    if ( stmtScope.Initialize( m_context, cStmtText, errMsg )
        != KSQL_PreparedStatementScope::initRcOk ) {

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDSTATWANTED_INTERNAL_DELETE_STMT_FAILED )
            );
        return delRcSystemError;
    }

    SAPDB_Int colNo = 0;
    SAPDB_Int index = 0;
    bool      ok    = true;

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &schemaName );

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &ownerName );
    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &tableName );

    ok &= stmtScope.GetStatement().bindIdentifier4Input( ++colNo, &columnName );

    if ( ! ok ) {

        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_UPDSTATWANTED_INTERNAL_DELETE_PARAM_BIND_ERR
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
                        SDBMSG_SQLMAN_UPDSTATWANTED_DELETE_FAILED,
                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDSTATWANTED_DELETE_FAILED__ERRORTEXT,
                                SQLManErr_Interface::GetInstance().GetErrorText(
                                    sqlReturnCode ) ),

                        Msg_Arg(SDBMSGTAG_SQLMAN_UPDSTATWANTED_DELETE_FAILED__RETURNCODE,
                                sqlReturnCode ) )
                    );

                return delRcSystemError;
        }

    }

    return delRcOkay;
}

/**************************************************************************/
