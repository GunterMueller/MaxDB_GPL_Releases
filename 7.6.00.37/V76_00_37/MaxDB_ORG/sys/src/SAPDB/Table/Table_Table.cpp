/*!
  @file           Table_Table.cpp
  @ingroup        Table
  @author         UweH

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"
#include "Table/Table_Exceptions.hpp"
#include "KernelCommon/Kernel_BlobDescription.hpp"
#include "KernelCommon/Kernel_IndexDescription.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "Table/Table_UnsortedResult.hpp"

#include "SAPDBCommon/Messages/SDBMsg_Table.h"

#include "Table/Table_Table.hpp"

#include "ggg00.h" // tgg00_TransContext,...
#include "hbd01.h" // b01niltree_id
#include "ggg92.h" // only for hkb50.h
#include "hkb50.h" // k50aux_fn_no
#include "hkb64.h" // k64build_aux_fn_prefix

/* --------------------------------------------------------------------------- */
/// This routine is necessary until trError_gg00 is not used anymore
static inline bool EvaluateError (tgg00_TransContext   &trans,
                                  Msg_List &errlist)
{
    if ( trans.trError_gg00 == e_ok )
        return true;
    Kernel_AppendBasisError(trans.trError_gg00, errlist);
    return false;
}
/* --------------------------------------------------------------------------- */
void Table_Table::WriteToTrace ( const char * title )
{
    Kernel_VTrace trace;

    if ( title != 0 )
        trace << title << FlushLine;

    trace << "TableId: " << m_TableId << FlushLine;
    m_Transaction.WriteToTrace (title);
}

/* --------------------------------------------------------------------------- */
Table_Table::Table_Table( Log_Transaction &transaction )
: m_Transaction (transaction)
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::Table_Table.NewTable", Table_Trace, 5 );
    #ifdef SAPDB_SLOW
    WriteToTrace();
    #endif
}

/* --------------------------------------------------------------------------- */
Table_Table::Table_Table( Log_Transaction &transaction,
                          Table_Id         tableId )
: m_Transaction  (transaction),
  m_TableId      (tableId)
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::Table_Table", Table_Trace, 5 );
    #ifdef SAPDB_SLOW
    WriteToTrace();
    #endif
}

/* --------------------------------------------------------------------------- */
const char * Table_Table::CreateResultToString[] = { "ok","error","permLimit" };

/* --------------------------------------------------------------------------- */
const char * Table_Table::VisibilityToString[] = { "global","local" };

/* --------------------------------------------------------------------------- */
const char * Table_Table::RemoveKindToString[] = { "none","endOfCommand","endOfTrans" };

/* --------------------------------------------------------------------------- */
Table_Table::CreateResult Table_Table::Create ( Visibility            visibility,
                                                RemoveKind            removeKind,
                                                Table_Id             &newTableId,
                                                Msg_List &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::Create", Table_Trace, 5 );

    if ( Table_Trace.TracesLevel(6) )
    {
        Kernel_VTrace()
            << "Visibility: "   << VisibilityToString[visibility]
            << ", removeKind: " << RemoveKindToString[removeKind]
            << FlushLine;
        WriteToTrace("Table_Table::Create.Begin");
    }
    
    if ( IsValid() )
    {
        errlist = Table_Exception( __CONTEXT__, TABLE_ALREADY_CREATED,
                                   SAPDB_ToString(m_TableId) );
        return createError;
    }                                              

    m_Transaction.BeginSubtrans();
    
    tgg00_FileId        oldfileid;
    integer             dummyLength;
    tgg00_TransContext &oldTransContext = m_Transaction.GetOldTransContext(); // necessary for casting
    
    k64build_aux_fn_prefix (oldTransContext.trWriteTransId_gg00, SAPDB_TRUE, oldfileid, dummyLength);
    k50aux_fn_no           (oldTransContext.trIndex_gg00,        SAPDB_TRUE, oldfileid.fileAuxCnt_gg00());

    Log_Transaction::RollbackResult rollbackResult;
    Log_ActionDropFile              undoAction ( oldfileid.fileName_gg00() );
    Log_ActionCreateFile            redoAction ( oldfileid.fileName_gg00(),
                                                 oldfileid.fileVersion_gg00(),
                                                 oldfileid.fileType_gg00() );
    
    undoAction.WriteBeforeImage (oldTransContext);
    if ( ! EvaluateError(oldTransContext, errlist) )
    {
        WriteToTrace("Table_Table::Create.Error");
        rollbackResult = m_Transaction.Rollback ( Log_Transaction::subTrans,
                                                  oldTransContext.trSubtransId_gg00,
                                                  Log_EntrySequence(),
                                                  errlist );
        if ( rollbackResult != Log_Transaction::undoOk )
            errlist.AppendNewMessage( Table_Exception( __CONTEXT__, TABLE_ROLLBACK_SUBTRANS_ERROR,
                                                       Log_Transaction::RollbackResultString[rollbackResult] ) );
        return createError;
    }
    
    redoAction.ExecuteAction (oldTransContext, &oldfileid);
    
    if ( ! EvaluateError(oldTransContext, errlist) )
    {
        WriteToTrace("Table_Table::Create.Error");
        rollbackResult = m_Transaction.Rollback ( Log_Transaction::subTrans,
                                                  oldTransContext.trSubtransId_gg00,
                                                  Log_EntrySequence(),
                                                  errlist );
        if ( rollbackResult != Log_Transaction::undoOk )
            errlist.AppendNewMessage( Table_Exception( __CONTEXT__, TABLE_ROLLBACK_SUBTRANS_ERROR,
                                                       Log_Transaction::RollbackResultString[rollbackResult] ) );
        return createError;
    }
    
    redoAction.WriteAfterImage (oldTransContext);

    if ( ! EvaluateError(oldTransContext, errlist) )
    {
        WriteToTrace("Table_Table::Create.Error");
        rollbackResult = m_Transaction.Rollback ( Log_Transaction::subTrans,
                                                  oldTransContext.trSubtransId_gg00,
                                                  Log_EntrySequence(),
                                                  errlist );
        if ( rollbackResult != Log_Transaction::undoOk )
            errlist.AppendNewMessage( Table_Exception( __CONTEXT__, TABLE_ROLLBACK_SUBTRANS_ERROR,
                                                       Log_Transaction::RollbackResultString[rollbackResult] ) );
        return createError;
    }

    m_TableId  = oldfileid.fileAuxCnt_gg00();
    newTableId = m_TableId;
    
    if ( Table_Trace.TracesLevel(6) )
        WriteToTrace("Table_Table::Create.End");

    if ( ! m_Transaction.EndSubtrans() )
        RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                      "m_Transaction.EndSubtrans()") );
    return createOk;
}


/* --------------------------------------------------------------------------- */
Table_Table::CreateResult Table_Table::Create (const Kernel_BlobDescription &blobDescription,
                                                     Visibility              visibility,
                                                     RemoveKind              removeKind,
                                                     Table_Id               &newTableId,
                                                     Msg_List   &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::Create.WithBlobs", Table_Trace, 5 );

    if ( Table_Trace.TracesLevel(6) )
    {
        Kernel_VTrace()
            << "Visibility: "   << VisibilityToString[visibility]
            << ", removeKind: " << RemoveKindToString[removeKind]
            << FlushLine;
        WriteToTrace("Table_Table::Create.Begin");
    }

    if ( IsValid() )
    {
        WriteToTrace("Table_Table::Create.Error");
        errlist = Table_Exception(__CONTEXT__, TABLE_ALREADY_CREATED, SAPDB_ToString(m_TableId));
        return createError;
    }

	errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_Table::Create.WithBlobs"));
    return createError;

    // return createOk;
}

/* --------------------------------------------------------------------------- */
Table_Table::DropResult Table_Table::Drop ( Msg_List &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::Drop", Table_Trace, 5 );

    if ( Table_Trace.TracesLevel(6) )
        WriteToTrace("Table_Table::Drop.Begin");

    if ( ! IsValid() )
    {
        WriteToTrace("Table_Table::Drop.Error");
        errlist = Table_Exception(__CONTEXT__, TABLE_NOT_IDENTIFIED);
        return dropError;
    }
    
	errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_Table::Drop"));
    return dropError;
}

/* --------------------------------------------------------------------------- */
Table_Table::AddIndexResult Table_Table::AddIndex ( const Kernel_IndexDescription &description,
                                                          Msg_List    &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::AddIndex", Table_Trace, 5 );

    if ( Table_Trace.TracesLevel(6) )
    {
        // description.WriteToTrace();
        WriteToTrace("Table_Table::AddIndex.Begin");
    }

    if ( ! IsValid() )
    {
        WriteToTrace("Table_Table::AddIndex.Error");
        errlist = Table_Exception(__CONTEXT__, TABLE_NOT_IDENTIFIED);
        return addIndexError;
    }
    
	errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_Table::AddIndex"));
    return addIndexError;
}

/* --------------------------------------------------------------------------- */
Table_Table::DeleteIndexResult Table_Table::DeleteIndex ( Kernel_IndexDescription::IndexNumber  indexno,
                                                          Msg_List                 &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::DeleteIndex", Table_Trace, 5 );

    if ( Table_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "indexno: " << indexno << FlushLine;
        WriteToTrace("Table_Table::DeleteIndex.Begin");
    }

    if ( ! IsValid() )
    {
        WriteToTrace("Table_Table::DeleteIndex.Error");
        errlist = Table_Exception(__CONTEXT__, TABLE_NOT_IDENTIFIED);
        return deleteIndexError;
    }
    
	errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_Table::DeleteIndex"));
    return deleteIndexError;
}

/* --------------------------------------------------------------------------- */
Table_Table::InsertResult Table_Table::Insert ( const SAPDBFields_IFieldSequence &fields,
                                                      Msg_List       &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::Insert", Table_Trace, 5 );

    if ( Table_Trace.TracesLevel(6) )
    {
        // fields.WritetoTrace();
        WriteToTrace("Table_Table::Insert.Begin");
    }

    if ( ! IsValid() )
    {
        WriteToTrace("Table_Table::Insert.Error");
        errlist = Table_Exception(__CONTEXT__, TABLE_NOT_IDENTIFIED);
        return insertError;
    }
    
	errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_Table::Insert"));
    return insertError;
}

/* --------------------------------------------------------------------------- */
bool Table_Table::Select ( Table_UnsortedResult &result,
                           Msg_List &errlist )
{
    SAPDBTRACE_METHOD_DEBUG( "Table_Table::Select1", Table_Trace, 5 );

    if ( Table_Trace.TracesLevel(6) )
        WriteToTrace("Table_Table::Select.Begin");

    if ( ! IsValid() )
    {
        WriteToTrace("Table_Table::Select1.Error");
        errlist = Table_Exception(__CONTEXT__, TABLE_NOT_IDENTIFIED);
        return false;
    }
    
	errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_Table::Select1"));
    return false;
}

