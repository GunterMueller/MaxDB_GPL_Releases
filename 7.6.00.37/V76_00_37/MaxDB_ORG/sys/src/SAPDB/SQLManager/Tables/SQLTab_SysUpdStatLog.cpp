/*****************************************************************************/
/*!
 @file    SQLTab_SysUpdStatLog.cpp
 @author  MartinKi

 @brief Implementation of class SQLTab_SysUpdStatLog.

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
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

#include "SAPDB/Messages/Msg_List.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "SQLManager/Tables/SQLTab_SysUpdStatLog.hpp"

/**************************************************************************/

SQLTab_SysUpdStatLog::SQLTab_SysUpdStatLog(SQLMan_Context& context)
    : m_context( context )
{}

/**************************************************************************/

SQLTab_SysUpdStatLog::ReturnCode
SQLTab_SysUpdStatLog::InsertRow(
    const SAPDBFields_IFieldSequence& values,
    const UpdateBehaviour             updateBehaviour,
    const CommitBehaviour             commitBehaviour,
    Msg_List&                         errMsg)
{

    SQLMan_SQLStatementContext newStatementScope( m_context );
    if ( !m_context.IsOk() )
    {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_UPDSTATLOG_NEWCONTEXT_FAILED,
            Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTATLOG_NEWCONTEXT_FAILED__RETURNCODE,
                     SQLManErr_Interface::GetInstance().GetErrorText(
                         m_context.ReturnCode() ) )
            );
        return SystemError;
    }

    KSQL_Connection con;
    if ( ! con.isConnected() ) {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_UPDSTATLOG_CREATESTATEMENT_FAILED,
            Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTATLOG_CREATESTATEMENT_FAILED__RETURNCODE,
                     SQLManErr_Interface::GetInstance().GetErrorText(
                         m_context.ReturnCode() ) )
            );
        return SystemError;
    }

    m_context.SetInDDLTrigger();

    // statement prepare
    KSQL_PreparedStatement stmt = con.createPreparedStatement();

    const int c_maxStmtLen = 500;
    char stmtText[ c_maxStmtLen+1 ] =
        "INSERT SYSDBA.SYSUPDSTATLOG ("
        " schemaname, tablename,"
        " tableid,"
        " columnname,"
        " indexname,"
        " executed_at, implicit, sample_pct, sample_row,"
        " execution_start, execution_end, session,"
        " termid,"
        " seqno)"
        " VALUES"
        " (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    if ( updateBehaviour == UpdateDuplicates ) {
        SAPDB_strncat( stmtText, " UPDATE DUPLICATES", c_maxStmtLen );
    }

    if ( commitBehaviour == WithCommit ) {
        SAPDB_strncat( stmtText, " WITH COMMIT", c_maxStmtLen );
    }

    const KSQL_Statement::SQLCode prepRc = stmt.prepare( stmtText );
    if ( prepRc != 0 ) {
        errMsg =
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_UPDSTATLOG_INTERNAL_PREPARE_FAILED,
                Msg_Arg(SDBMSGTAG_SQLMAN_UPDSTATLOG_INTERNAL_PREPARE_FAILED__RETURNCODE,
                        prepRc) );
        return SystemError;
    }

    SAPDB_Int colNo = 0;
    SAPDB_Int index = 0;
    bool      ok    = true;
    SAPDBFields_AttributedField curField;

    ok &= ( values.GetField( index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    stmt.bindIdentifier4Input( ++colNo, curField.GetDataPtr() ); // schemaname

    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    stmt.bindIdentifier4Input( ++colNo, curField.GetDataPtr() ); // tablename


    // tableid
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bind(
            ++colNo, curField.GetDataPtr(), curField.GetLength() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // columnname
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindIdentifier4Input(
            ++colNo, curField.GetDataPtr() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // indexname
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindIdentifier4Input(
            ++colNo,
            reinterpret_cast<const char*>( curField.GetDataPtr() ) );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // executed_at
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindTimestampInput( ++colNo, curField.GetDataPtr() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // implicit
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindAsciiInput(
            ++colNo,
            reinterpret_cast<const char*>( curField.GetDataPtr() ),
            curField.GetLength() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // sample_pct
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
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

    // sample_row
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
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

    // execution_start
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindTimestampInput( ++colNo, curField.GetDataPtr() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // execution_end
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindTimestampInput( ++colNo, curField.GetDataPtr() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // session
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindInt4Input(
            ++colNo,
            *reinterpret_cast<const SAPDB_Int4*>( curField.GetDataPtr() ) );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // termid
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    if ( ok
         && (curField.GetAttribute()
             == SAPDBFields_AttributedField::ValueAttribute)
        ) {
        stmt.bindAsciiInput(
            ++colNo,
            reinterpret_cast<const char*>( curField.GetDataPtr() ),
            curField.GetLength() );
    } else {
        stmt.bindNullValue( ++colNo );
    }

    // seqNo
    ok &= ( values.GetField( ++index, curField, errMsg ) ==
            SAPDBFields_IFieldSequence::fieldFound );
    stmt.bindInt4Input(
        ++colNo,
        *reinterpret_cast<const SAPDB_Int4*>(curField.GetDataPtr()) );

    SAPDBERR_ASSERT_STATE( ok );
    if ( ! ok ) {
        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_SQLMAN_UPDSTATLOG_PREPARE_FAILED
            );
        return SystemError;
    }

    // statement execution
    KSQL_Statement::SQLCode sqlReturnCode = stmt.execute();
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
                errMsg = Msg_List(
                    Msg_List::Warning,
                    SDBMSG_SQLMAN_UPDSTATLOG_INSERT_FAILED,
                    Msg_Arg( SDBMSGTAG_SQLMAN_UPDSTATLOG_INSERT_FAILED__ERRORTEXT,
                             SQLManErr_Interface::GetInstance().GetErrorText(
                                 sqlReturnCode ) )
                    );
                return InsertFailed;
        }
    }

    return Okay;
}

/**************************************************************************/

SQLTab_SysUpdStatLog::ReturnCode
SQLTab_SysUpdStatLog::InsertRow(
    const UpdateBehaviour      updateBehaviour,
    const CommitBehaviour      commitBehaviour,
    const SQLMan_Identifier&   schemaName,
    const SQLMan_Identifier&   tableName,
    const SQLMan_Identifier&   columnName,
    const SQLMan_Identifier&   indexName,
    const FileDir_FileNo&      tableFileNo,
    const RTESys_SQLTimestamp& executionTimeStamp,
    const SAPDB_Int            samplePercent,
    const SAPDB_Int            sampleRows,
    const RTESys_SQLTimestamp& beginTimeStamp,
    const RTESys_SQLTimestamp& endTimeStamp,
    const SAPDB_Int            sessionId,
    const tsp00_TermId&        termId,
    const SAPDB_Int            seqNo,
    Msg_List&                  errMsg)
{
    SAPDB_Int4      c_nullInt     = 0;
    const SAPDB_Int c_columnCount = 14;

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

    // tableid
    curField.Assign(
        const_cast<char*>( tableFileNo.GetCharArray() ),
        sizeof( FileDir_FileNo ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // columnname
    curField.Assign(
        const_cast<SQLMan_Identifier*>( &columnName ), sizeof( columnName ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // indexname
    curField.Assign(
        const_cast<SQLMan_Identifier*>( &indexName ),
        sizeof( indexName ) );
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

    // implicit
    const char c_No[] = "NO";
    curField.Assign( const_cast<char*>(c_No), sizeof( c_No ) - 1 );
    fields.Fill( colNo++, c_columnCount, curField );

    // sample_pct
    curField.Assign(
        const_cast<SAPDB_Int*>( &samplePercent ), sizeof( samplePercent ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // sample_row
    curField.Assign(
        const_cast<SAPDB_Int*>( &sampleRows ), sizeof( sampleRows ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // execution start
    RTESys_SQLTimestamp beginTimeStampCpy;
    SAPDB_MemCopyNoCheck(
        beginTimeStampCpy,
        beginTimeStamp,
        sizeof( RTESys_SQLTimestamp ) );
    curField.Assign(
        beginTimeStampCpy,
        sizeof( beginTimeStamp ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // execution end
    RTESys_SQLTimestamp endTimeStampCpy;
    SAPDB_MemCopyNoCheck(
        endTimeStampCpy,
        endTimeStamp,
        sizeof( RTESys_SQLTimestamp ) );
    curField.Assign(
        endTimeStampCpy,
        sizeof( endTimeStamp ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // session
    curField.Assign(
        const_cast<SAPDB_Int*>( &sessionId ), sizeof( sessionId ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // termid
    curField.Assign(
        const_cast<tsp00_TermId*>( &termId ), sizeof( termId ) );
    fields.Fill( colNo++, c_columnCount, curField );

    // seqNo
    curField.Assign(
        const_cast<SAPDB_Int*>( &seqNo ), sizeof( seqNo ) );
    fields.Fill( colNo, c_columnCount, curField );

    return this->InsertRow(
        fields,
        updateBehaviour,
        commitBehaviour,
        errMsg );
}

/**************************************************************************/
