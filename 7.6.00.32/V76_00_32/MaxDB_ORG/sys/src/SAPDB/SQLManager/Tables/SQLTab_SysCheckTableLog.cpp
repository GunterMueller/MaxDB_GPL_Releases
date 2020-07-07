/*****************************************************************************/
/*!
 @file    SQLTab_SysCheckTableLog.cpp
 @author  MartinKi

 @brief Implementation of class SQLTab_SysCheckTableLog.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#include "SQLManager/SQLMan_SavedErrorScope.hpp"
#include "SQLManager/SQLMan_ChangedUserScope.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"

#include "SAPDB/Messages/Msg_List.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "SQLManager/Tables/SQLTab_SysCheckTableLog.hpp"

#include "heo52.h" // vtimestamp
#include "hgg01_3.h" // g01glob

/**************************************************************************/

SQLTab_SysCheckTableLog::SQLTab_SysCheckTableLog(SQLMan_Context& context)
    : m_context( context )
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysCheckTableLog::SQLTab_SysCheckTableLog", SQLMan_Trace, 1);
}

/**************************************************************************/

SQLTab_SysCheckTableLog::ReturnCode
SQLTab_SysCheckTableLog::InsertRow(
    const SAPDBFields_IFieldSequence& values,
    const SAPDB_Bool                  bTryUpdate,
    Msg_List&                         errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysCheckTableLog::InsertRow-fields", SQLMan_Trace, 1);

    enum ColumnIndices {
        eSchemaIndex = 0,
        eTableIndex,
        eTimeStampIndex,
        eMsgNoIndex
    };

    SAPDB_Int colNo = 0;
    bool      ok    = true;
    KSQL_Statement::SQLCode     sqlReturnCode = 0;
    SAPDBFields_AttributedField curField;

    // save current error state then reset it to be able to go on
    SQLMan_SavedErrorScope  newSavedErrorScope( m_context );
    m_context.ResetReturnCode();

    // switch to sysuser to be able to do insert
    SQLMan_ChangedUserScope sysuserScope( m_context, g01glob.sysuser_name );
    if ( !m_context.IsOk() )
    {
        SAPDB_Int auxError = m_context.ReturnCode();
        m_context.ResetReturnCode();
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_CHECKTABLELOG_SWITCH_USER_FAILED,
            Msg_Arg( SDBMSGTAG_SQLMAN_CHECKTABLELOG_SWITCH_USER_FAILED__ERRORTEXT,
                     SQLManErr_Interface::GetInstance().GetErrorText(
                         auxError ) )
            );
        return SystemError;
    }

    SQLMan_SQLStatementContext newStatementScope( m_context );
    if ( !m_context.IsOk() )
    {
        SAPDB_Int auxError = m_context.ReturnCode();
        m_context.ResetReturnCode();
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_CHECKTABLELOG_NEWCONTEXT_FAILED,
            Msg_Arg( SDBMSGTAG_SQLMAN_CHECKTABLELOG_NEWCONTEXT_FAILED__RETURNCODE,
                     SQLManErr_Interface::GetInstance().GetErrorText(
                         auxError ) )
            );
        return SystemError;
    }

    KSQL_Connection con;
    if ( ! con.isConnected() ) {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_CHECKTABLELOG_CREATESTATEMENT_FAILED,
            Msg_Arg( SDBMSGTAG_SQLMAN_CHECKTABLELOG_CREATESTATEMENT_FAILED__RETURNCODE,
                     SQLManErr_Interface::GetInstance().GetErrorText(
                         m_context.ReturnCode() ) )
            );
        return SystemError;
    }

    // statement prepare
    KSQL_PreparedStatement stmt = con.createPreparedStatement();

    if ( bTryUpdate ) {
        const char updStmtText[] =
            "UPDATE SYSDBA.SYSCHECKTABLELOG"
            " SET executed_at = ?, excecuted_at = ? WHERE"
            " schemaname     = ?"
            " and tablename  = ?"
            " and msgno      = 0";

        if ( (sqlReturnCode = stmt.prepare( updStmtText )) != 0 ) {
            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_CHECKTABLELOG_PREPARE_UPDATE_FAILED,
                Msg_Arg( SDBMSGTAG_SQLMAN_CHECKTABLELOG_PREPARE_UPDATE_FAILED__ERRORTEXT,
                         SQLManErr_Interface::GetInstance().GetErrorText(
                             sqlReturnCode ) )
                );
            SAPDBERR_ASSERT_STATE( sqlReturnCode == 0 );
            return SystemError;
        }

        // executed_at
        ok = ( values.GetField( eTimeStampIndex, curField, errMsg ) ==
               SAPDBFields_IFieldSequence::fieldFound );
        stmt.bindTimestampInput( ++colNo, curField.GetDataPtr() );

        // excecuted_at
        ok &= ( values.GetField( eTimeStampIndex, curField, errMsg ) ==
                SAPDBFields_IFieldSequence::fieldFound );
        stmt.bindTimestampInput( ++colNo, curField.GetDataPtr() );

        // schemaname
        ok &= ( values.GetField( eSchemaIndex, curField, errMsg ) ==
                SAPDBFields_IFieldSequence::fieldFound );
        stmt.bindIdentifier4Input( ++colNo, curField.GetDataPtr() );

        // tablename
        ok &= ( values.GetField( eTableIndex, curField, errMsg ) ==
                SAPDBFields_IFieldSequence::fieldFound );
        stmt.bindIdentifier4Input( ++colNo, curField.GetDataPtr() );

        if ( ! ok ) {
            errMsg = Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_CHECKTABLELOG_PREPARE_PARAMS_FAILED );
            SAPDBERR_ASSERT_STATE( ok );
            return SystemError;
        }

        // statement execution
        sqlReturnCode = stmt.execute();

        if ( sqlReturnCode == 0 ) {
            return Okay;
        }

        if ( sqlReturnCode != 100 ) {
            switch ( sqlReturnCode ) {
                case -102:
                    return Cancelled;
                case -20:
                case 200:
                    return DuplicateKey;
                case 500:
                    return LockRowFailed;
                default:
                    return InsertFailed;
            }
        }
    }

    /*
     * insert new row
     */
    const char stmtText[] =
        "INSERT SYSDBA.SYSCHECKTABLELOG ("
        " schemaname, tablename,"
        " executed_at,"
        " msgno,"
        " owner, excecuted_at)"
        " VALUES"
        " (?, ?, ?, ?, ?, ?) UPDATE DUPLICATES WITH COMMIT";

    if ( (sqlReturnCode = stmt.prepare( stmtText )) != 0 ) {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_CHECKTABLELOG_PREPARE_INSERT_FAILED,
            Msg_Arg( SDBMSGTAG_SQLMAN_CHECKTABLELOG_PREPARE_INSERT_FAILED__ERRORTEXT,
                     sqlReturnCode )
            );
        SAPDBERR_ASSERT_STATE( sqlReturnCode == 0 );
        return SystemError;
    }

    colNo = 0;
    ok    = true;

    // schemaname
    ok &= ( values.GetField( eSchemaIndex, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    stmt.bindIdentifier4Input( ++colNo, curField.GetDataPtr() );

    // tablename
    ok &= ( values.GetField( eTableIndex, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    stmt.bindIdentifier4Input( ++colNo, curField.GetDataPtr() );

    // executed_at
    ok &= ( values.GetField( eTimeStampIndex, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindTimestampInput( ++colNo, curField.GetDataPtr() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // msgno
    ok &= ( values.GetField( eMsgNoIndex, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindInt4Input(
            ++colNo,
            *reinterpret_cast<const SAPDB_Int4*>( curField.GetDataPtr() ));
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // owner
    ok &= ( values.GetField( eSchemaIndex, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    stmt.bindIdentifier4Input( ++colNo, curField.GetDataPtr() );

    // excecuted_at
    ok &= ( values.GetField( eTimeStampIndex, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindTimestampInput( ++colNo, curField.GetDataPtr() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    if ( ! ok ) {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_CHECKTABLELOG_PREPARE_PARAMS_FAILED );
        SAPDBERR_ASSERT_STATE( ok );
        return SystemError;
    }

    // statement execution
    sqlReturnCode = stmt.execute();
    if ( sqlReturnCode != 0 ) {
        switch ( sqlReturnCode ) {
            case -102:
                return Cancelled;
            case -20:
            case 200:
                return DuplicateKey;
            case 500:
                return LockRowFailed;
            default:
                return InsertFailed;
        }
    }

    return Okay;
}

/**************************************************************************/

SQLTab_SysCheckTableLog::ReturnCode
SQLTab_SysCheckTableLog::InsertRow(
    const SQLMan_Identifier&   schemaName,
    const SQLMan_Identifier&   tableName,
    const RTESys_SQLTimestamp& executionTimeStamp,
    const SAPDB_Int            msgNo,
    Msg_List&                  errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysCheckTableLog::InsertRow-withTimeStamp", SQLMan_Trace, 1);

    SAPDB_Int4      c_nullInt     = 0;
    const SAPDB_Int c_columnCount = 6;

    const SQLMan_Identifier c_emptyName;

    SAPDBFields_AttributedField curField;
    SAPDBFields_FieldMap fields( m_context.GetAllocator(), c_columnCount );
    SAPDB_Int colNo = 0;

    // schemaname
    curField.Assign(
        const_cast<SQLMan_Identifier*>( &schemaName ), sizeof( schemaName ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // tablename
    curField.Assign(
        const_cast<SQLMan_Identifier*>( &tableName ), sizeof( tableName ) );
    fields.Fill( colNo++, c_columnCount, curField );


    // executed_at

    // AIX compiler does not allow const_cast for RTESys_SQLTimestamp,
    // therefore we have to copy...
    RTESys_SQLTimestamp executionTimeStampCpy;
    SAPDB_MemCopyNoCheck(
        executionTimeStampCpy,
        executionTimeStamp,
        sizeof( RTESys_SQLTimestamp ) );
    curField.Assign(
        executionTimeStampCpy,
        sizeof( executionTimeStamp ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // msgno
    curField.Assign(
        const_cast<SAPDB_Int*>( &msgNo ), sizeof( msgNo ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // owner
    curField.Assign(
        const_cast<SQLMan_Identifier*>( &schemaName ), sizeof( schemaName ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // exCecuted_at

    // AIX compiler does not allow const_cast for RTESys_SQLTimestamp,
    // therefore we have to copy...
    SAPDB_MemCopyNoCheck(
        executionTimeStampCpy,
        executionTimeStamp,
        sizeof( RTESys_SQLTimestamp ) );
    curField.Assign(
        executionTimeStampCpy,
        sizeof( executionTimeStamp ) );
    fields.Fill( colNo++, c_columnCount, curField );

    SAPDBERR_ASSERT_STATE( colNo == c_columnCount );

    return this->InsertRow(
        fields,
        msgNo == 0, // update existing row if no error has occured
        errMsg );
}

/**************************************************************************/

SQLTab_SysCheckTableLog::ReturnCode
SQLTab_SysCheckTableLog::InsertRow(
    const SQLMan_Identifier&   schemaName,
    const SQLMan_Identifier&   tableName,
    const SAPDB_Int            msgNo,
    Msg_List&                  errMsg)
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLTab_SysCheckTableLog::InsertRow-withoutTimeStamp", SQLMan_Trace, 1);

    RTESys_SQLTimestamp timeStamp;
    vtimestamp( *reinterpret_cast<tsp00_Timestamp*>(&timeStamp) );

    return this->InsertRow( schemaName, tableName, timeStamp, msgNo, errMsg );
}

/**************************************************************************/
