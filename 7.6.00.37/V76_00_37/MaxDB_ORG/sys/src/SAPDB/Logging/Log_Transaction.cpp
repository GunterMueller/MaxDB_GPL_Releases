/*!**************************************************************************

  module      : Log_Transaction.cpp

  -------------------------------------------------------------------------

  responsible : UweH

  special area: Logging
  description : 


  last changed: 2001-01-15  15:59
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_UndoFile.hpp"
#include "Restart/Rst_RedoFile.hpp"
#include "Logging/Log_AfterImage.hpp"
#include "Logging/Log_BeforeImage.hpp"
#include "Logging/Log_IAction.hpp"
#include "Logging/Log_ActionUndoRedo.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_History.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "GarbageCollector/GC_IGCController.hpp" /* PTS 1120151 FF 2003-JAN-31 */
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "hkb50.h"  // k50my_state
#include "hkb54.h"  // k54rollback_temp
#include "hkb57.h"  // k57surrogates_to_log
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Log.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
 
// #define TEST_DEBUG 1
 
/*===========================================================================*
 *  LOCAL *
 *===========================================================================*/

/// 0 means is not set
static SAPDB_UInt LocalRedoBufferSize = 0; // PTS 1131107 UH 2004-08-24 added

/* -------------------------------------------------------------------------------- */
static void TraceEntry ( const char*          title,
                         const tgg91_TransNo &TransNo,
                         Log_IOSequenceNo     SubTransNo, // PTS 1124727 mb 2003-10-29
                         Log_EntrySequence    sequence,
                         Log_ActionType       actionType)
{
    Kernel_VTrace() << title
                    << ": T" << TransNo.gg90GetInt4()
                    << "/"   << SubTransNo
                    << " ("  << sequence
                    << ") '" << Log_GetActionTypeString(actionType)
                    << "'";
}

/* -------------------------------------------------------------------------------- */
inline void BeginWriteTransaction(tsp00_TaskId taskid)
{
    // THIS IS EXECUTED WHEN A TRANSACTION BEGINS WITH CHANGES
}

/* -------------------------------------------------------------------------------- */
inline void EndWriteTransaction(tsp00_TaskId taskid)
{
    // THIS IS EXECUTED WHEN A TRANSACTION ENDS WHICH MADE CHANGES
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::CreateUndoFile (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::CreateUndoFile", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE   ( IsInitialized() );
    if ( ! m_pUndoFile->Create (errlist) ) // PTS 1121659 UH 2003-04-30
        return false;
    kb50EnterTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    m_Context.UndoFileRootPageNo = m_pUndoFile->GetRootId().PageNo();
    kb50LeaveTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    return true;
}
                                    
/* -------------------------------------------------------------------------------- */
bool Log_Transaction::DropUndoFile(SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::DropUndoFile", LogTrans_Trace, 5);
    
    if ( m_pTrans != NULL )
        kb50EnterTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    
    m_Context.UndoFileRootPageNo = SAPDB_MAX_INT4;
    
    if ( m_pTrans != NULL )
        kb50LeaveTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    
    if ( m_pUndoFile != NULL
         &&
         m_pUndoFile->IsCreated() )
    {
        if(!m_pUndoFile->Drop(errlist))
            return false;
    }
    return true;
}
                                    
/* -------------------------------------------------------------------------------- */
bool Log_Transaction::CreateRedoFile (tgg00_TransContext   &redoReaderTrans,
                                      SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::CreateRedoFile", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE   ( IsInitializedForRedoCopy() );
    if ( ! m_pRedoFile->Create (errlist) ) // PTS 1121659 UH 2003-04-30
        return false;
    kb50EnterTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    m_Context.RedoFileRootPageNo = m_pRedoFile->GetRootId().PageNo();
    kb50LeaveTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    return true;
}
                                    
/* -------------------------------------------------------------------------------- */
bool Log_Transaction::DropRedoFile(SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::DropRedoFile", LogTrans_Trace, 5);
    
    if ( m_pTrans != NULL )
        kb50EnterTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    
    m_Context.RedoFileRootPageNo = SAPDB_MAX_INT4;
    
    if ( m_pTrans != NULL )
        kb50LeaveTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    
    if ( m_pRedoFile != NULL
         &&
         m_pRedoFile->IsCreated() )
    {
        if(!m_pRedoFile->Drop(errlist))
            return false;
    }
    return true;
}
                                    
/* -------------------------------------------------------------------------------- */
bool Log_Transaction::IsInitialized()
{
    return m_TransType      != illegalTransType && 
           m_pTrans         != NULL             &&
           m_pAllocator     != NULL             &&
           m_pSubtransStack != NULL             &&
           m_pUndoFile      != NULL;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::Initialize (tgg00_TransContext   &Trans,
                                  SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::Initialize", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE ( ! IsInitialized() );

    m_pTrans     = &Trans;
    m_pAllocator = REINTERPRET_CAST(SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00);
    m_pQueue     = NULL;


    if ( Log_Volume::IsReadyForWriting()
         &&
         Log_Volume::Instance().IsDevspaceEnabled(Trans.trTaskId_gg00) )
    {
        Log_Volume::Instance().GetQueueForWriting(Trans.trTaskId_gg00, m_pQueue); //  PTS 1135304 mb 2005-05-24
        m_Context.QueueID = m_pQueue->GetID();               // PTS 1137832 mb 2005-09-21
	}

    m_pSubtransStack = new (Allocator()) SubtransStack (Allocator());
    if ( m_pSubtransStack == NULL )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_pSubtransStack allocation");
        (void)Delete(errlist);
        return false;
    }
    
    m_pUndoFile = new (Allocator()) Log_UndoFile ( Trans,
                                                   m_Context.UndoFileRootPageNo,
                                                   m_Context.UndoFileLastPageNo );
    if ( m_pUndoFile == NULL )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_pUndoFile allocation");
        (void)Delete(errlist);
        return false;
    }

    if ( ! m_pSubtransStack->Initialize(LOG_DEFAULT_SUBTRANS_STACK_CAPACITY) )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_pUndoFile allocation");
        (void)Delete(errlist);
        return false;
    }

	// PTS 1131107 UH 2004-08-24
    if ( LocalRedoBufferSize != 0
         &&
         ! m_LocalRedoEntries.Initialize(Allocator(),LocalRedoBufferSize) )
    {
        Delete(errlist);
        return false;
    }
    
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::Delete(SAPDBErr_MessageList &errlist,
                             bool                  checkLocalRedoLogBuffer)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::Delete", LogTrans_Trace, 5);
    bool result = true;
    if ( m_pUndoFile != NULL  )
    {
        // PTS 1133708 UH 2005-02-01 give file to garbage collector if not empty
        if ( m_Context.LastUndoEntrySequence.IsValid() )
        {
            result = result && GiveUndoFileToHistory (errlist);
        }
        else
            result = result && DropUndoFile(errlist);
        destroy (m_pUndoFile, Allocator());
    }
    else
    {
        // PTS 1133708 UH 2005-02-01 give file to garbage collector if not empty
        if ( m_Context.LastUndoEntrySequence.IsValid() )
        {
            m_pUndoFile = new (Allocator()) Log_UndoFile ( *m_pTrans, m_Context.UndoFileRootPageNo,
                                                                      m_Context.UndoFileLastPageNo);
            if ( m_pUndoFile == 0 )
            {
                errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "allocation of Log_UndoFile failed");
                return false;
            }
            result = result && GiveUndoFileToHistory (errlist);
            destroy (m_pUndoFile, Allocator());
        }
    }
    if (m_pRedoFile != NULL )
    {
        result = result && DropRedoFile(errlist);
        destroy (m_pRedoFile, Allocator());
    }
    if (m_pSubtransStack != NULL)
    {
        destroy (m_pSubtransStack, Allocator());
    }
    
    if ( checkLocalRedoLogBuffer )
    {
        if ( ! m_LocalRedoEntries.IsEmpty() )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"split space buffer must be empty before Delete()");
            result = false;
        }
    }
    else
        m_LocalRedoEntries.Release();
        
    m_LocalRedoEntries.Delete();
    Log_Volume::Instance().ReleaseQueue(m_pTrans->trTaskId_gg00, m_pQueue); //  PTS 1135304 mb 2005-05-24

    m_pTrans          = NULL;
    m_pAllocator      = NULL;
    return result;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::IsInitializedForRedoCopy()
{
    return m_TransType  != illegalTransType
           && 
           m_pRedoFile  != NULL
           &&
           m_pTrans     != NULL
           &&
           m_pAllocator != NULL;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::InitializeForRedoCopy (      tgg00_TransContext     &redoReaderTrans,
                                             const tgg91_TransNo          &transNo,
                                             const Log_QueueID             queueID,
                                                   SAPDBMem_IRawAllocator &allocator,
                                                   SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::InitializeForRedoCopy", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE ( ! IsInitializedForRedoCopy() );

    m_Context.TransNo                 = transNo;
    m_Context.QueueID                 = queueID; // PTS 1137832 mb 2005-09-21
    m_TransType                       = redoRead;
    m_pAllocator                      = &allocator;
    m_pTrans                          = &redoReaderTrans;

    m_pRedoFile = new (allocator) Rst_RedoFile ( redoReaderTrans,
                                                 m_Context.RedoFileRootPageNo,
                                                 m_Context.RedoFileLastPageNo);
    if ( 0 == m_pRedoFile )
    {
        errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                "memory allocation for Rst_RedoFile");
		return false;
    }

	// PTS 1131107 UH 2004-08-24
    if ( LocalRedoBufferSize != 0
         &&
         ! m_LocalRedoEntries.Initialize(Allocator(),LocalRedoBufferSize) )
    {
        m_LocalRedoEntries.WriteToTrace ("Log_Transaction::InitializeForRedoCopy.m_LocalRedoEntries.Initialize()");
        return false;
    }

    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::DeleteAfterRedoCopy(Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::DeleteAfterRedoCopy", LogTrans_Trace, 5);
    if ( m_pRedoFile != 0 && m_pAllocator == 0 )
        RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                   "Log_Transaction::DeleteAfterRedoCopy() allocator not assigned") );
    if ( m_pRedoFile != 0 )
    {
        if ( ! m_pRedoFile->Delete(errlist))
        {
            return false;
        }
        destroy (m_pRedoFile, Allocator()); // UH 2005-02-01 changed Deallocate to destroy()
    }
    m_pAllocator  = 0;
    m_pTrans      = 0;
    
    m_LocalRedoEntries.Delete();
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::IsInitializedForRedo()
{
    return m_TransType  != illegalTransType && 
           m_pTrans     != NULL             &&
           m_pAllocator != NULL;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::InitializeForRedo (tgg00_TransContext &redoExecuteTrans)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::InitializeForRedo", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE   ( m_pRedoFile == NULL );
    SAPDBERR_ASSERT_STATE   ( m_pUndoFile == NULL );
    
    redoExecuteTrans.trLogContext_gg00 = this;
    
    m_pTrans          = &redoExecuteTrans;
    m_pAllocator      = REINTERPRET_CAST(SAPDBMem_IRawAllocator*, redoExecuteTrans.trAllocator_gg00);
    m_TransType       = redoExecute;

    if ( m_Context.UndoFileRootPageNo != SAPDB_MAX_INT4 )
    {
        m_pUndoFile = new (Allocator()) Log_UndoFile ( redoExecuteTrans,
                                                       m_Context.UndoFileRootPageNo,
                                                       m_Context.UndoFileLastPageNo);
        if ( m_pUndoFile == NULL )
        {
            RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"allocation of Log_UndoFile") );
            return false;
        }

        SAPDBERR_ASSERT_STATE   ( m_pUndoFile->IsCreated() );

        Msg_List errlist;
        
        if ( ! m_pUndoFile->RestorePageCountAndRelevanceForGC(errlist) )
        {
            errlist.Overrule(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"access of undo file"));
            RTE_Crash( errlist );
            return false;
        }
    }        

    if ( m_Context.RedoFileRootPageNo != SAPDB_MAX_INT4 )
    {
        m_pRedoFile = new (Allocator()) Rst_RedoFile ( redoExecuteTrans,
                                                       m_Context.RedoFileRootPageNo,
                                                       m_Context.RedoFileLastPageNo);
        if ( m_pRedoFile == NULL )
        {
            RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"allocation of Rst_RedoFile") );
            return false;
        }
    }        
    
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::DeleteAfterRedo(SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::DeleteAfterRedo", LogTrans_Trace, 5);
    return Delete(errlist);
}

/* -------------------------------------------------------------------------------- */
Log_Transaction& Log_Transaction::CopyFromRedoReaderToRedoJobList(const Log_Transaction& srcTrans)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::operator =", LogTrans_Trace, 5);
    m_Context                      = srcTrans.m_Context;
    m_MaxSubtransNo                = srcTrans.m_MaxSubtransNo;
    m_TransType                    = illegalTransType;
    m_WasOpenInLastSavepoint       = srcTrans.m_WasOpenInLastSavepoint;
    m_OldestKnownIOSequence        = srcTrans.m_OldestKnownIOSequence;
    return *this;
}

/* -------------------------------------------------------------------------------- */
void Log_Transaction::WritePersistentFormat (SAPDB_Byte* Space,
                                             SAPDB_UInt  Length)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::WritePersistentFormat", LogTrans_Trace, 5);
    
    SAPDBERR_ASSERT_STATE (Space != NULL);
    SAPDBERR_ASSERT_STATE (Length == GetPersistentLength());

    m_PersistentAccess.Lock();
    *(REINTERPRET_CAST(Persistent*,Space)) = m_Context;
    m_PersistentAccess.Unlock();
}

/* -------------------------------------------------------------------------------- */
void Log_Transaction::ReadPersistentFormat (const SAPDB_Byte   *Space,
                                            const SAPDB_UInt    Length)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::ReadPersistentFormat", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE (Space != NULL);
    SAPDBERR_ASSERT_STATE (Length == GetPersistentLength() || Length == sizeof(OldPersistentLayout));

    if (Length == GetPersistentLength())
    // this is the currently used layout
    {
        m_Context = *(REINTERPRET_CAST(const Persistent*,Space));
    }
    else
    // the older persistent layout has been used
    {
        m_Context = Persistent ( *(REINTERPRET_CAST(const OldPersistentLayout*,Space)) );
    }
    m_WasOpenInLastSavepoint = true;
}

/* -------------------------------------------------------------------------------- */
void Log_Transaction::WriteToTrace (const char *Title) const
{
    if ( Title != NULL )
        Kernel_VTrace() << Title;
    
    Kernel_VTrace trace;
    
    trace         << "Dump of T" << m_Context.TransNo.gg90GetInt4()
                  << (m_TransType==online?" (online):":(m_TransType==redoRead?" (redoRead):":" (redoExecute):")) << " State: "
                  << ((GetTransState() == opened)       ? "open" :
                      (GetTransState() == committed)  ? "committed" :
                      (GetTransState() == rollbacked) ? "rollbacked" : "unknown")
                  << "[" << m_Context.EOT_IOSequenceNo << "]"
                  << (m_WasOpenInLastSavepoint?" was open in last savepoint":"")
                  << FlushLine;
    trace         << "Undo: root: "   << m_Context.UndoFileRootPageNo
                  << ", last: "       << m_Context.UndoFileLastPageNo
                  << ", lastseq: "    << m_Context.LastUndoEntrySequence
                  << FlushLine;
    trace         << "Redo: root: "   << m_Context.RedoFileRootPageNo
                  << ", last: "       << m_Context.RedoFileLastPageNo
                  << ", lastseq: "    << m_Context.LastRedoEntrySequence
                  << ", lastcopseq: " << m_Context.LastCopiedRedoEntrySequence
                  << ", QueueID: " << m_Context.QueueID
                  << ", LastRedoEntryIsComplete: " << (m_Context.LastRedoEntryIsComplete ? "TRUE" : "FALSE") 
                  << FlushLine;

    if ( m_pTrans == NULL )
        trace     << "pTrans: NIL\n";
    else
        trace     << "pTrans: "     << m_pTrans->trSessionId_gg00.gg90GetInt4()
                  << "."            << m_pTrans->trWriteTransId_gg00.gg90GetInt4()
                  << "/"            << m_pTrans->trSubtransId_gg00
                  << " ("           << m_pTrans->trTransId_gg00.gg90GetInt4()
                  << ") view: "     << m_pTrans->trConsistView_gg00.gg90GetInt4()
                  << " task: "      << m_pTrans->trTaskId_gg00
                  << " index: "     << m_pTrans->trIndex_gg00
                  << " error: "     << Kernel_BasisErrorToString(m_pTrans->trError_gg00)
                  << FlushLine;

    if ( m_pQueue == NULL  )
        trace     << "ArchivLog: disabled\n";
    else
        trace     << "ArchivLog: enabled\n";

    if ( m_pUndoFile == NULL )
        trace     << "UndoFile: NULL\n";
    else
        m_pUndoFile->WriteToTrace();

    if ( m_pRedoFile == NULL )
        trace << "RedoFile: NULL\n";
    else
        m_pRedoFile->WriteToTrace();
}

/* -------------------------------------------------------------------------------- */
void Log_Transaction::AppendContextInfo (SAPDB_Char  const * const  filename,
                                         SAPDB_UInt4 const          linenumber,
                                         SAPDBErr_MessageList      &errlist,
                                         SAPDB_Char  const * const  title) const
{
    const char* transTypeStr  = (m_TransType==online?"online":(m_TransType==redoRead?"redoRead":"redoExecute"));
    const char* transStateStr = ((GetTransState() == opened)     ? "open" :
                                 (GetTransState() == committed)  ? "committed" :
                                 (GetTransState() == rollbacked) ? "rollbacked" : "unknown");
    const char* openInLastSVP = (m_WasOpenInLastSavepoint?"open in last savepoint":"");

    errlist = errlist
    + Log_Exception (__CONTEXT__, LOG_TRANS_CONTEXT_HEAD,
                     SAPDB_ToString(m_Context.TransNo.gg90GetInt4()),
                     transTypeStr,
                     transStateStr,
                     SAPDB_ToString(m_Context.EOT_IOSequenceNo),
                     openInLastSVP)
	+ Log_Exception (__CONTEXT__, LOG_TRANS_CONTEXT_FILES,
                     "UndoFile",
                     SAPDB_ToString(m_Context.UndoFileRootPageNo),
                     SAPDB_ToString(m_Context.UndoFileLastPageNo),
                     SAPDB_ToString(m_Context.LastUndoEntrySequence),
                     "")
	+ Log_Exception (__CONTEXT__, LOG_TRANS_CONTEXT_FILES,
                     "RedoFile",
                     SAPDB_ToString(m_Context.RedoFileRootPageNo),
                     SAPDB_ToString(m_Context.RedoFileLastPageNo),
                     SAPDB_ToString(m_Context.LastRedoEntrySequence),
                     SAPDB_ToString(m_Context.LastCopiedRedoEntrySequence));

    if ( m_pQueue == 0 )
        errlist = errlist + Log_Exception (__CONTEXT__, LOG_TRANS_CONTEXT_NO_REDOLOG);

    if ( m_pTrans != 0 )
    {
        errlist = errlist
                + Log_Exception (__CONTEXT__, LOG_TRANS_CONTEXT_SESSION,
                                 SAPDB_ToString(m_pTrans->trSessionId_gg00.gg90GetInt4()),
                                 SAPDB_ToString(m_pTrans->trWriteTransId_gg00.gg90GetInt4()),
                                 SAPDB_ToString(m_pTrans->trSubtransId_gg00))
                + Log_Exception (__CONTEXT__, LOG_TRANS_CONTEXT_TASK_ENTRY,
                                 SAPDB_ToString(m_pTrans->trTaskId_gg00),
                                 SAPDB_ToString(m_pTrans->trIndex_gg00));
        Kernel_AppendBasisError(m_pTrans->trError_gg00, errlist);
    }
}

/* -------------------------------------------------------------------------------- */
void Log_Transaction::BeginSubtrans()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::BeginSubtrans", LogTrans_Trace, 5);
    
    SAPDBERR_ASSERT_STATE ( IsInitialized() );
    SAPDBERR_ASSERT_STATE ( opened == GetTransState() );
    
    ++m_MaxSubtransNo;
    m_pTrans->trSubtransId_gg00 = m_MaxSubtransNo;

    SubtransStackElem NewElem;
    NewElem.SubtransNo       = m_pTrans->trSubtransId_gg00;
    NewElem.UndoStopSequence = m_Context.LastUndoEntrySequence + 1;
    NewElem.RedoStopSequence = m_Context.LastRedoEntrySequence + 1;

    if ( ! m_pSubtransStack->Push(NewElem) )
    {
        RTE_Crash( Log_Exception(__CONTEXT__,
                                      SAPDBERR_ASSERT_STATE_FAILED,
                                      "BeginSubtrans:Push failed") );
    }
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "NewSubtransNo: " << NewElem.SubtransNo << \
                        ", UndoStopSeq: " << NewElem.UndoStopSequence << \
                        ", RedoStopSeq: " << NewElem.RedoStopSequence);
}
    
/* -------------------------------------------------------------------------------- */
bool Log_Transaction::EndSubtrans()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::EndSubtrans", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE ( IsInitialized() );
    SAPDBERR_ASSERT_STATE ( opened == GetTransState() );

    if ( m_pSubtransStack->IsEmpty() )
    {
        return false;
    }

    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "OldSubtransNo: " << m_pSubtransStack->Top().SubtransNo << \
                        ", UndoStopSeq: " << m_pSubtransStack->Top().UndoStopSequence << \
                        ", RedoStopSeq: " << m_pSubtransStack->Top().RedoStopSequence);

    (void)m_pSubtransStack->Pop();

    if ( m_pSubtransStack->IsEmpty() )
    {
        // ++ until temp logging is written new
        // m_MaxSubtransNo             = LOG_NIL_SUBTRANS_NO;
        m_pTrans->trSubtransId_gg00 = LOG_NIL_SUBTRANS_NO;
    }
    else
        m_pTrans->trSubtransId_gg00 = m_pSubtransStack->Top().SubtransNo;

    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "NewSubtransNo: " << m_pTrans->trSubtransId_gg00);
    
    return true;
}
    

/* -------------------------------------------------------------------------------- */
tgg00_SubtransNo Log_Transaction::
PopUntilSubtransFound (tgg00_SubtransNo   SubtransNo,
                       Log_EntrySequence &UndoStopSequence,
                       Log_EntrySequence &RedoStopSequence)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::PopUntilSubtransFound", LogTrans_Trace, 5);
    
    while ( ! m_pSubtransStack->IsEmpty() )
    {
        if ( m_pSubtransStack->Top().SubtransNo == SubtransNo )
        {
            UndoStopSequence = m_pSubtransStack->Top().UndoStopSequence;
            RedoStopSequence = m_pSubtransStack->Top().RedoStopSequence;
            (void)m_pSubtransStack->Pop();
            if ( m_pSubtransStack->IsEmpty() )
            {
                // ++ until temp logging is written new
                // m_MaxSubtransNo = LOG_NIL_SUBTRANS_NO;
                return LOG_NIL_SUBTRANS_NO;
            }
            return m_pSubtransStack->Top().SubtransNo;
        }
        (void)m_pSubtransStack->Pop();
    }
    
    RTE_Crash( Log_Exception(__CONTEXT__,
                                  SAPDBERR_ASSERT_STATE_FAILED,
                                  "PopUntilSubtransFound: SubtransNo not found") );
    return LOG_NIL_SUBTRANS_NO;
}
    
/* -------------------------------------------------------------------------------- */
bool Log_Transaction::EndOfTransHandling (TransState            State,
                                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::EndOfTransHandling", LogTrans_Trace, 5);

    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, \
                        "State: " << ((State == opened)     ? "open" : \
                                      (State == committed)  ? "committed" : \
                                      (State == rollbacked) ? "rollbacked" : "unknown"));

    #ifdef SAPDB_QUICK
    if ( m_Context.LastRedoEntrySequence.IsValid()
         ||
         m_Context.LastUndoEntrySequence.IsValid() )
    {
        if ( (void*)k50GetLogTransaction (m_pTrans->trIndex_gg00) != (void*)this )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Log_Transaction::EndOfTransHandling() logtransptr lost1");
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    #endif
    
    if ( m_pTrans->trState_gg00.includes(tsSurrogateUpd_egg00) )
    {
        k57surrogates_to_log (*m_pTrans);
        if ( m_pTrans->trError_gg00 != e_ok )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }

    kb50EnterTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
    SetTransState (State);
    kb50LeaveTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);

    #ifdef SAPDB_QUICK
    if ( online == m_TransType
         &&
         (m_Context.LastRedoEntrySequence.IsValid()
          ||
          m_Context.LastUndoEntrySequence.IsValid()) )
    {
        if ( (void*)k50GetLogTransaction (m_pTrans->trIndex_gg00) != (void*)this )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Log_Transaction::EndOfTransHandling() logtransptr lost2");
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    #endif

    if ( online == m_TransType
         &&
         (m_Context.LastRedoEntrySequence.IsValid()
         ||
         m_Context.LastUndoEntrySequence.IsValid()) ) // PTS 1136623 UH 2005-08-29
    {
        if ( ! WriteEndOfTransEntry (State, errlist) )
            return false;
    }

    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "EndOfTrans: T" << m_Context.TransNo.gg90GetInt4() <<
                                           " iosequence: " << m_Context.EOT_IOSequenceNo);

    if ( m_Context.LastUndoEntrySequence.IsValid()
         &&
         m_pUndoFile != NULL
         &&
         m_pUndoFile->IsCreated() )
    {
        if ( ! GiveUndoFileToHistory(errlist) )
            return false;

        if ( online == m_TransType )
            EndWriteTransaction(m_pTrans->trTaskId_gg00);
    }

    // reset the persistent part of the transaction
    m_PersistentAccess.Lock();
    m_Context.init();
    m_PersistentAccess.Unlock();

    if ( online == m_TransType )
    {
        (void)m_pSubtransStack->Empty();
        // ++ until temp logging is written new
        // m_MaxSubtransNo                 = LOG_NIL_SUBTRANS_NO;
        m_pTrans->trSubtransId_gg00     = LOG_NIL_SUBTRANS_NO;

        // reevaluate the current Logqueue in case that the UKT has changed due
        // to loadbalancing (PTS 11338762 mb 2005-02-14 
        if ( NULL != m_pQueue )
        // if Logwriting is disabled no queue is needed
        {
            Log_Volume::Instance().GetQueueForWriting(m_pTrans->trTaskId_gg00, m_pQueue); //  PTS 1135304 mb 2005-05-24
            m_Context.QueueID = m_pQueue->GetID();               // PTS 1137832 mb 2005-09-21
        }
    }

    return true;
}

/* -------------------------------------------------------------------------------- */
Log_UndoFile::WriteBeforeImageResult
     Log_Transaction::WriteUndoEntry (      Log_BeforeImage &BeforeImage,
                                      const Log_IAction     &Action,
                                      Data_PageNo           &pageno,
                                      Data_PageOffset       &offset,
                                      SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::WriteUndoEntry", LogTrans_Trace, 5);

    // PTS 1114653 UH 2002-03-05 it must be allowed to write undo entries during redo.
    #ifdef SAPDB_QUICK
    switch (m_TransType)
    {
        case online:
            if ( ! IsInitialized() )
                RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"not initialized correctly"));
            break;
        case redoRead:
            if ( ! IsInitializedForRedoCopy() )
                RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"not initialized correctly"));
            break;
        case redoExecute:
            if ( ! IsInitializedForRedo() )
                RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"not initialized correctly"));
            break;
        default:
            RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_TransType unexpected"));
    }
    #endif

    if ( m_Context.LastUndoEntrySequence.IsValid() )
    {
        SAPDBERR_ASSERT_STATE ( m_pUndoFile->IsCreated() );
        ++m_Context.LastUndoEntrySequence;
        if ( m_pTrans->trWriteTransId_gg00 != m_Context.TransNo )
            RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Log_Transaction::WriteUndoEntry: writetrans differs") );
    }
    else
    {
        SAPDBERR_ASSERT_STATE ( ! m_pUndoFile->IsCreated() );
        if ( ! CreateUndoFile(errlist) ) // PTS 1121659 UH 2003-04-30
        {
            if ( m_TransType != online ) // PTS 1121659 UH 2003-04-30
                RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                           "undo file creation") );
            return Log_UndoFile::writeBeforeImageNoSpaceAvailable; // PTS 1121659 UH 2003-04-30
        }
        m_Context.LastUndoEntrySequence = LOG_FIRST_ENTRY_SEQUENCE;
        m_Context.TransNo = m_pTrans->trWriteTransId_gg00;
        // PTS 1127009 UH 2004-01-09 added m_OldestKnownIOSequence, PTS 1127704 mb 2004-02-11
        m_OldestKnownIOSequence = Log_Savepoint::GetLastSavepointIOSequence();
    }

    #ifdef SAPDB_QUICK
    if ( (void*)k50GetLogTransaction (m_pTrans->trIndex_gg00) != (void*)this )
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                   "Log_Transaction::WriteUndoEntry() logtransptr lost") );
    #endif

    BeforeImage.PrepareForWrite ( Action.GetType(),
                                  Action.GetPersistentVersion(),
                                  m_pTrans->trConsistView_gg00,
                                  m_Context.LastUndoEntrySequence,
                                  m_Context.LastRedoEntrySequence );

	if ( LogTrans_Trace.TracesLevel(7) )
    {
    	TraceEntry ( "write undo entry",
        	         m_Context.TransNo,
            	     m_pTrans->trSubtransId_gg00,
                	 m_Context.LastUndoEntrySequence,
                     BeforeImage.GetActionType() );
        Action.WriteToTrace("write undo entry");
    }
    SAPDBERR_ASSERT_STATE ( m_pUndoFile != NULL && m_pUndoFile->IsCreated() );

    // PTS 1121659 UH 2003-04-30
    const Log_UndoFile::WriteBeforeImageResult writeResult = 
        m_pUndoFile->WriteUndoEntry (BeforeImage, Action, pageno, offset, errlist);
    m_Context.UndoFileLastPageNo = pageno;

    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "beforeimage ins: " << pageno << "." << offset);
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "end: " << (writeResult == Log_UndoFile::writeBeforeImageOk ? "ok" : "ERROR"));

    BeginWriteTransaction(m_pTrans->trTaskId_gg00);
    
    return writeResult; // PTS 1121659 UH 2003-04-30
}


/* --------------------------------------------------------------------------- */
const char * Log_Transaction::RollbackResultString[] =
{
    "Ok",
    "NoOpenSubtrans",
    "ReadError",
    "WrongEntry",
    "ExecuteError",
    "WriteError",
    "LogFull",
    "InitNotCorrect",
    "TempRollbackFailed",
    "EndOfTransFailed"
};

/* -------------------------------------------------------------------------------- */

/// Adds an element to transState on construction and removes it on
/// destruction
class AutoAddDelState
{
private:
    tgg00_TransState&             m_transState;     ///< all states of the transaction including m_element
    const tgg00_TransState__Enum  m_element;        ///< the state which will be removed upon deconstruction

public:
    /// CTOR
    AutoAddDelState (
        tgg00_TransState&            transState,
        const tgg00_TransState__Enum elementToAdd)
        : m_transState (transState),
          m_element (elementToAdd)
    {
        m_transState.addElement (m_element);
    }

    /// DTOR: removes automatically the state which had been set in the CTOR
    ~AutoAddDelState()
    {
        m_transState.delElement (m_element);
    }
};

/* -------------------------------------------------------------------------------- */
Log_Transaction::RollbackResult Log_Transaction::
    Rollback (RollbackKind          rollbackKind,
              tgg00_SubtransNo      SubtransNo,
              Log_EntrySequence     undoStopSequence,
              SAPDBErr_MessageList &errlist ) // PTS 1113230 UH 2001-12-20
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::Rollback", LogTrans_Trace, 5);

    // set tsRollback_egg00 and make sure it is cleared on return:
    AutoAddDelState tempAddRollbackState (m_pTrans->trState_gg00, tsRollback_egg00);

#   ifdef SAPDB_QUICK
    if ( endOfTrans == rollbackKind
         &&
         SubtransNo != LOG_NIL_SUBTRANS_NO )
	{
        errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                  "Log_Transaction::Rollback() givenSubtransNo is not nil but rollbackkind is EndOfTrans");
        AppendContextInfo(__CONTEXT__,errlist);
        return undoInitNotCorrect;
	}
#   endif

    if ( online == m_TransType && ! IsInitialized() )
    {
        errlist = Log_Exception(__CONTEXT__,LOG_TRANS_INIT_NOT_CORRECT, "online mode");
        AppendContextInfo(__CONTEXT__,errlist);
        return undoInitNotCorrect;
    }

    const bool isRedoPartialRollback = undoStopSequence.IsValid(); // PTS 1113230 UH 2001-12-20
    
    if ( m_pTrans != NULL )
        m_pTrans->trError_gg00 = e_ok;
    
    if ( redoExecute == m_TransType && ! IsInitializedForRedo() )
    {
        errlist = Log_Exception(__CONTEXT__,LOG_TRANS_INIT_NOT_CORRECT, "redo mode");
        AppendContextInfo(__CONTEXT__,errlist);
        return undoInitNotCorrect;
    }
    if ( m_TransType != online
         &&
         ! isRedoPartialRollback ) // PTS 1113230 UH 2001-12-20
    {
        if(!DropRedoFile(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return undoInitNotCorrect;
        }
        SAPDBERR_ASSERT_STATE ( m_pUndoFile != NULL );
    }

    RollbackResult Result = undoOk;

    if ( endOfTrans != rollbackKind
         &&
         ! isRedoPartialRollback // PTS 1113230 UH 2001-12-20
         &&
         m_pSubtransStack->IsEmpty() )
    {
        return undoNoOpenSubtrans;
    }

    // ++++ TEMP LOGGING SHOULD BE OBSOLETE
    if ( m_pTrans->trState_gg00.includes (tsTempUpdated_egg00) )
    {
        enum AuxRollbackType { eot, command, subtrans };
        AuxRollbackType auxRollbackType;
        
        if ( rollbackKind == endOfTrans )
        {
            k54rollback_temp (*m_pTrans, mm_rollback);
            auxRollbackType = eot;
        }
        else if ( rollbackKind == command
                  &&
                  m_pTrans->trState_gg00.includes (tsFunctRollbSet_egg00) )
        {
            k54rollback_temp (*m_pTrans, mm_nil);
            auxRollbackType = command;
        }
        else if ( rollbackKind == subTrans )
        {
            k54rollback_temp (*m_pTrans, mm_subtrans);
            auxRollbackType = subtrans;
        }
        if ( m_pTrans->trError_gg00 != e_ok )
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_LOG_TEMP_ROLLBACK_FAILED,
                               Msg_Arg("ERRORTEXT",(auxRollbackType==eot?"EndOfTrans":(auxRollbackType==command)?"Command":"Subtrans")
                               ));
            AppendContextInfo(__CONTEXT__,errlist);
            return undoTempRollbackFailed;
        }
    }
    // ++++ TEMP LOGGING SHOULD BE OBSOLETE

    if ( ! m_Context.LastUndoEntrySequence.IsValid() )
    {
        if ( endOfTrans == rollbackKind )
        {
            if ( ! EndOfTransHandling(rollbacked, errlist) )
            {
                return undoEndOfTransFailed;
            }
        }    
        else
            if ( ! isRedoPartialRollback // PTS 1113230 UH 2001-12-20 
                 &&
                 ! EndSubtrans() )
            {
                return undoNoOpenSubtrans;
            }
        return undoOk;
    }

    SAPDBERR_ASSERT_STATE ( m_pUndoFile->IsCreated() );

    Log_EntrySequence UndoStopSequence = LOG_FIRST_ENTRY_SEQUENCE;
    Log_EntrySequence RedoStopSequence = LOG_FIRST_ENTRY_SEQUENCE;
    Log_EntrySequence PrevSequence     = LOG_FIRST_ENTRY_SEQUENCE;

    if ( isRedoPartialRollback ) // PTS 1113230 UH 2001-12-20 begin
        UndoStopSequence = undoStopSequence;
    else                         // PTS 1113230 UH 2001-12-20 end
    {
        if ( LOG_NIL_SUBTRANS_NO != SubtransNo )
        {
            m_pTrans->trSubtransId_gg00 =
                PopUntilSubtransFound (SubtransNo,
                                       UndoStopSequence,
                                       RedoStopSequence);

            SAPDBTRACE_WRITELN (LogTrans_Trace, 6,
                                "PartialRollback:SubtransNo: " << SubtransNo <<
                                (endOfTrans == rollbackKind?" eot ":command == rollbackKind?" command":" partial ")
                                << "=> UndoStopSequence: " << UndoStopSequence
                                << (UndoStopSequence == m_Context.LastUndoEntrySequence + 1 ? " => nothing to do." : ""));
    
            if ( UndoStopSequence == m_Context.LastUndoEntrySequence + 1 )
            {
                return undoOk;
            }
        }
    }
    
    Log_UndoFile::Iterator& iter = m_pUndoFile->GetIterator();

    if ( ! m_pUndoFile->GetLastUndoEntry(Data_ForUpdate, iter, errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return undoReadError;
    }

    bool                    iterDerefResult = true;
    Data_SplitSpaceReader   Reader (iter.Deref (iterDerefResult, errlist), true);
    Log_BeforeImage         BeforeImage;
    SAPDB_UInt              EntryCount = 0;

    if ( ! iterDerefResult )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return undoReadError;
    }
    
    while ( iter.IsValid() )
    {
        Reader.Reset();
        if (! BeforeImage.ReadPersistentFormat (Reader, errlist))
        {
            Result = undoReadError;
            AppendContextInfo(__CONTEXT__,errlist);
            (void)iter.Invalidate(errlist);
            return undoReadError;
        }
        if ( LogTrans_Trace.TracesLevel(7) )
        {
            iter.WriteToTrace("ITER");
            BeforeImage.WriteToTrace("BEFOREIMAGE");
        }

        if ( LOG_FIRST_ENTRY_SEQUENCE != PrevSequence )
        {
            if ( (PrevSequence - 1) != BeforeImage.GetSequence() )
            {
                Result = undoWrongEntry;
                errlist = Log_Exception(__CONTEXT__,LOG_INVALID_LOG_UNDO_SEQUENCE,
                                        SAPDB_ToString(PrevSequence-1),
                                        SAPDB_ToString(BeforeImage.GetSequence()));
                BeforeImage.AppendContextInfo(errlist);
                Reader.AppendContextInfo(__CONTEXT__,errlist);
                iter.AppendContextInfo(__CONTEXT__,errlist);
                AppendContextInfo(__CONTEXT__,errlist);
                (void)iter.Invalidate(errlist);
                return undoReadError;
            }
        }
        PrevSequence = BeforeImage.GetSequence();

        if ( BeforeImage.IsRollbacked(false)
             ||
             BeforeImage.IsDeleted(false) ) // PTS 1115828 UH 2002-05-17
		{
            if ( LogTrans_Trace.TracesLevel(6) )
            {
                Kernel_VTrace() << "skipped undo entry: (" << BeforeImage.GetSequence() << ") '"
                                << Log_GetActionTypeString(BeforeImage.GetActionType()) << "'";
                if ( LogTrans_Trace.TracesLevel(7) )
                {
                    iter.WriteToTrace("skipped undo entry");
                    BeforeImage.WriteToTrace("skipped undo entry");
                }
            }
		}
        else
        {
            ++EntryCount;

            if ( ! Log_ActionExecuteUndo (*this, BeforeImage, Reader, iter, errlist) )
            {
                (void)iter.Invalidate(errlist);
                return undoExecuteError;
            }
            // This MUST be placed AFTER the undo action.
            if ( BeforeImage.GetActionType() == Log_DeleteObject )
                BeforeImage.SetDeleted(true); // PTS 1115828 UH 2002-05-17
            else
                BeforeImage.SetRollbacked(true); // PTS 1114840 UH 2002-03-14
        }

        if ( (UndoStopSequence != LOG_FIRST_ENTRY_SEQUENCE)
             &&
             (UndoStopSequence == BeforeImage.GetSequence()) )
        {
            SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "ROLLBACK-INFO: stop sequence reached");
            break;
        }

        if(!iter.MinusMinus(errlist))
        {
            iter.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            (void)iter.Invalidate(errlist);
            return undoReadError;
        }

        (void) iter.Deref(iterDerefResult,errlist);  // really assign the space
        if ( ! iterDerefResult )
        {
            (void)iter.Invalidate(errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return undoReadError;
        }
    } // endWhile

    if(!iter.Invalidate(errlist)) // release all resources (pages)
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return undoReadError;
    }

    if ( endOfTrans == rollbackKind )
    {
        if ( ! EndOfTransHandling (rollbacked, errlist) )
        {
            return undoEndOfTransFailed;
        }
        return undoOk;
    }

    // there was anything rollbacked and there are entries,
    // which must be skipped during redo
    if ( EntryCount > 0
         &&
         ! isRedoPartialRollback ) // PTS 1113230 UH 2001-12-20
    {
        switch  ( Log_ActionWritePartialRollback(*this, UndoStopSequence, RedoStopSequence, errlist) )
        {
            case Log_Transaction::noError:    
                break;
            case Log_Transaction::logFull:  
                AppendContextInfo(__CONTEXT__,errlist);
                return undoLogFull;
            case Log_Transaction::logError: 
                AppendContextInfo(__CONTEXT__,errlist);
                return undoWriteError; 
            default:
                WriteToTrace("Log_Transaction::Rollback().WriteRedoEntry()");
                RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                        "Log_Transaction::Rollback().WriteRedoEntry()"));
        }
    }
    return undoOk;
}

/* -------------------------------------------------------------------------------- */
Data_SplitSpaceWriter Log_Transaction::ReserveSpaceForRedoEntryCopy
        (Data_SplitRecordLength Length,
         Data_RecordLength      MinimalLength,
         Log_EntrySequence      EntrySequence,
         Log_IOSequenceNo       IOSequence,
         SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::ReserveSpaceForRedoEntryCopy", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE ( IsInitializedForRedoCopy() );
    if ( ! m_pRedoFile->IsCreated() )
    {
        if(!CreateRedoFile(*m_pTrans,errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return Data_SplitSpaceWriter();
        }
        m_OldestKnownIOSequence = IOSequence; // PTS 1127704 mb 2004-02-11
    }

    errlist.ClearMessageList();
    #ifdef SAPDB_SLOW
    if ( LogTrans_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "Length:        " << Length        << ", MinimalLength: " << MinimalLength;
        Kernel_VTrace() << "EntrySequence: " << EntrySequence << ", IOSequence: " << IOSequence;
    }
    #endif
    Data_SplitSpaceWriter writer = m_pRedoFile->ReserveSpace ( Length, MinimalLength,
                                                               EntrySequence, IOSequence,
                                                               errlist );
	if ( ! errlist.IsEmpty() )
	{
        AppendContextInfo(__CONTEXT__,errlist);
        return writer;
	}
    m_Context.RedoFileLastPageNo          = m_pRedoFile->LastPageNo();
    m_Context.LastCopiedRedoEntrySequence = EntrySequence;
    return writer;
}

Data_SplitSpaceWriter Log_Transaction::ReReserveSpaceForRedoEntryCopy
        (Data_SplitRecordLength CurrentRestlength,
         SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::ReReserveSpaceForRedoEntryCopy", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE ( IsInitializedForRedoCopy() );
    if ( ! m_pRedoFile->IsCreated() )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return Data_SplitSpaceWriter();
    }

    errlist.ClearMessageList();
    #ifdef SAPDB_SLOW
    if ( LogTrans_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "ReReserve: Trans = " << m_Context.TransNo.gg90GetInt4() 
                        << " RestLength = " << CurrentRestlength ;
    }
    #endif
    Data_SplitSpaceWriter writer = m_pRedoFile->ReReserveSpace ( CurrentRestlength, errlist );
    if ( ! errlist.IsEmpty() )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return writer;
    }
    m_Context.RedoFileLastPageNo          = m_pRedoFile->LastPageNo();
    return writer;
}


/* -------------------------------------------------------------------------------- */
bool Log_Transaction::FinishCopiedRedoEntry(SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::FinishCopiedRedoEntry", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE ( IsInitializedForRedoCopy() );
    SAPDBERR_ASSERT_STATE ( m_pRedoFile->IsCreated() );
    if(!m_pRedoFile->ReleaseSpace(false, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}
/* -------------------------------------------------------------------------------- */
bool Log_Transaction::ReleaseIncompleteRedoEntry( SAPDBErr_MessageList &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::ReleaseIncompleteRedoEntry", LogTrans_Trace, 5);
    SAPDBERR_ASSERT_STATE ( IsInitializedForRedoCopy() );
    SAPDBERR_ASSERT_STATE ( m_pRedoFile->IsCreated() );
    if(!m_pRedoFile->ReleaseSpace(true, errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    m_Context.RedoFileLastPageNo = Data_PageNo(); // set to invalid value
    return true;
}
/* -------------------------------------------------------------------------------- */
Log_Transaction::LogState 
Log_Transaction::WriteRedoEntry (const Log_IAction    &Action,
                                 SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::WriteRedoEntry", LogTrans_Trace, 5);
    
    SAPDBERR_ASSERT_STATE ( IsInitialized() );
    #ifdef SAPDB_QUICK
    if ( (void*)k50GetLogTransaction (m_pTrans->trIndex_gg00) != (void*)this )
        RTE_Crash( SAPDBErr_Exception(__FILE__, __LINE__,SAPDBERR_ASSERT_STATE_FAILED,
                   "Log_Transaction::WriteRedoEntry() logtransptr lost") );
    #endif
    
    if ( Action.GetType() == Log_NoOp )
	{
        errlist = Log_Exception(__CONTEXT__,LOG_REDO_WRITE_INVALID_ACTION,
                                Log_GetActionTypeString(Action.GetType()),
                                Log_GetActionTypeString(Action.GetType()) );
        AppendContextInfo(__CONTEXT__,errlist);
        return logError;
	}

    bool              isFirstRedoEntry = false;
    Log_EntrySequence savedLastRedoEntrySequence = m_Context.LastRedoEntrySequence;
    
    if ( m_Context.LastRedoEntrySequence.IsValid() )
    {
        ++m_Context.LastRedoEntrySequence;
        if ( m_pTrans->trWriteTransId_gg00 != m_Context.TransNo )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Log_Transaction::WriteRedoEntry: writetrans differs1");
            AppendContextInfo(__CONTEXT__,errlist);
            return logError;
        }
    }
    else
    {
        isFirstRedoEntry                = true;
        m_Context.LastRedoEntrySequence = LOG_FIRST_ENTRY_SEQUENCE;
        if ( m_Context.LastUndoEntrySequence.IsValid() )
        {
            if ( m_pTrans->trWriteTransId_gg00 != m_Context.TransNo )
            {
                errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                           "Log_Transaction::WriteRedoEntry: writetrans differs2");
                AppendContextInfo(__CONTEXT__,errlist);
                return logError;
            }
        }
        else
            m_Context.TransNo = m_pTrans->trWriteTransId_gg00;
    }

    #ifdef SAPDB_QUICK
    if ( (void*)k50GetLogTransaction (m_pTrans->trIndex_gg00) != (void*)this )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                   "Log_Transaction::WriteRedoEntry() logtransptr lost");
        AppendContextInfo(__CONTEXT__,errlist);
        return logError;
    }
    #endif
    
    if ( LocalRedoBufferSize != 0 // PTS 1131107 UH 2004-08-24
         &&
         Action.GetType() != Log_MultipleActions )
    {
        if ( Action.LocalRedoEntryCachingPossible() )
        {
            if ( m_LocalRedoEntries.Append( m_Context.LastRedoEntrySequence, 
                                            m_Context.LastUndoEntrySequence, 
                                            Action,
                                            errlist) == Log_ActionMultiple::appendOk )
            {
                if ( LogTrans_Check.ChecksLevel(5) )
                    TraceEntry ( "WriteRedoEntry-Append",
                                 m_Context.TransNo,
                                 m_pTrans->trSubtransId_gg00,
                                 m_Context.LastRedoEntrySequence,
                                    Action.GetType() );
                return noError;
            }
            if ( ! m_LocalRedoEntries.IsEmpty() )
            {
                // local buffer is full so flush it and then append current entry
                const LogState result = WriteRedoEntry (m_LocalRedoEntries,errlist);
                if ( result != noError )
                    return result;
                m_LocalRedoEntries.Reset();
                const Log_ActionMultiple::AppendResult appendResult
                    = m_LocalRedoEntries.Append( m_Context.LastRedoEntrySequence, 
                                                 m_Context.LastUndoEntrySequence, 
                                                 Action,
                                                 errlist );
                if ( appendResult == Log_ActionMultiple::appendOk )
                {
                    if ( LogTrans_Check.ChecksLevel(5) )
                        TraceEntry ( "WriteRedoEntry-Append",
                                     m_Context.TransNo,
                                     m_pTrans->trSubtransId_gg00,
                                     m_Context.LastRedoEntrySequence,
                                        Action.GetType() );
                    return noError;
                }
                if ( appendResult == Log_ActionMultiple::appendBufferIsFull )
                {
                    // The entry may be larger then the local redo buffer.
                    // Set a flag in the systemview LOGSTATISTICS.
                }
            }
            // here: appending is not possible write the entry to the log as usual
        }
        else
        {
            // cached redo entries must be flushed before end of trans log entry
            if ( ! m_LocalRedoEntries.IsEmpty() )
            {
                const LogState result = WriteRedoEntry (m_LocalRedoEntries, errlist);
                if ( result != noError )
                    return result;
                m_LocalRedoEntries.Reset();
            }
        }
    }

    Log_AfterImage AfterImage;
    
    AfterImage.PrepareForWrite( Action.GetType(),
                                Action.GetPersistentVersion(),
                                m_Context.TransNo,
                                m_Context.LastRedoEntrySequence,
                                m_Context.LastUndoEntrySequence );
    
    if ( LogTrans_Trace.TracesLevel(6) )
    	TraceEntry ( "WriteRedoEntry",
        	         m_Context.TransNo,
            	     m_pTrans->trSubtransId_gg00,
                	 m_Context.LastRedoEntrySequence,
               		 Action.GetType() );

    const Data_SplitRecordLength LengthToReserve = 
                                    AfterImage.GetPersistentLength() 
                                    +
                                    Action.GetPersistentLength();
    const Data_SplitRecordLength MinLengthToReserve = 
                                    AfterImage.GetPersistentLength() 
                                    +
                                    Action.GetMinimalPersistentLength();
    
    Log_AfterImageSpace Space (Allocator());
    if (!Space.Initialize())
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                    "Log_Transaction::WriteRedoEntry: space init failed");
        AppendContextInfo(__CONTEXT__,errlist);
        return logError;
    }

    if ( m_pQueue != NULL )
    {        
        SAPDB_UInt2       offsetInPage;
        bool              result;
        tgg00_WaitContext waitContext;
    
        if ( Action.WaitForLogWriting() ) 	// PTS 1117320 UH 2002-10-28
        {
            SAPDBERR_ASSERT_ARGUMENT ( LengthToReserve == MinLengthToReserve );
            
            if ( ! isFirstRedoEntry )
                m_pQueue->DecrementCounter(Log_Queue::openTrans);
                
            result = m_pQueue->ReserveForEOT (m_pTrans->trTaskId_gg00,
                                              waitContext,
                                              LengthToReserve,
                                              Space,
                                              offsetInPage,
                                              errlist);
        }
        else
        {
            if ( isFirstRedoEntry )
                m_pQueue->IncrementCounter(Log_Queue::openTrans);
                
            result = m_pQueue->Reserve (  m_pTrans->trTaskId_gg00,
                                          waitContext,
                                          LengthToReserve,
                                          MinLengthToReserve,
                                          Action.ForceLogWriting(), // PTS 1117320 UH 2002-10-28
                                          //|| m_pTrans->trState_gg00.includes(tsDbFullNowait_egg00), // TS 2004-12-02
                                          Space,
                                          errlist);
        }
        if ( ! result )
        {
            m_Context.LastRedoEntrySequence = savedLastRedoEntrySequence;
            AppendContextInfo(__CONTEXT__,errlist);
            AfterImage.AppendContextInfo(errlist);
            Action.AppendContextInfo(__CONTEXT__,errlist);
            return logError;
        }
        SAPDBERR_ASSERT_STATE ( LengthToReserve == Space.Length() );

        Data_SplitSpaceWriter Writer (Space);

        if(!AfterImage.WritePersistentFormat (Writer, errlist))
        {
            AfterImage.AppendContextInfo(errlist);
            Action.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return logError;
        }

        if(!Action.WritePersistentFormat (Writer, errlist))
        {
            AfterImage.AppendContextInfo(errlist);
            Action.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return logError;
        }

        if ( Action.WaitForLogWriting() ) // PTS 1117320 UH 2002-10-28
        {
            Log_RawDeviceOffset offsetOnDevice;

            m_pQueue->UserTaskEOTReady (m_pTrans->trTaskId_gg00,
                                        waitContext,
                                        Space,
                                        m_Context.EOT_IOSequenceNo,
                                        offsetOnDevice);

            SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "EOT-entry written to: " << offsetOnDevice << \
                                              "." << offsetInPage << \
                                              " [" << m_Context.EOT_IOSequenceNo << "]");
        }
        else
        {
            m_pQueue->UserTaskReady (m_pTrans->trTaskId_gg00, Space);
        }
    }
    return noError;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::WriteEndOfTransEntry (TransState            State,
                                            SAPDBErr_MessageList &errlist)
{
    SAPDBERR_ASSERT_STATE (opened != State);
    
    if (State==committed)
    {
        // prepare a commit-entry with timestamp
        Log_ActionEndOfTransaction Action (Log_Commit);
        Kernel_DateTime            currentTime;

        currentTime.DetermineTime();
        Action.Initialize (currentTime);
        if ( WriteRedoEntry(Action, errlist) != noError )
            return false;
    }
    else
    {
        // write simple Rollback-entry
        if ( WriteRedoEntry(Log_ActionEndOfTransaction(Log_Rollback), errlist) != noError )
            return false;
    }
    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::Commit (SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::Commit", LogTrans_Trace, 5);

    SAPDBTRACE_IF (LogTrans_Trace, 6, WriteToTrace ("Log_Transaction::Commit"));
    
    SAPDBERR_ASSERT_STATE (opened == GetTransState());
    
    // ++++ TEMP LOGGING SHOULD BE OBSOLETE
    if ( m_pTrans->trState_gg00.includes (tsTempUpdated_egg00) )
    {
        k54commit_temp (*m_pTrans);
        if ( m_pTrans->trError_gg00 != e_ok )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    // ++++ TEMP LOGGING SHOULD BE OBSOLETE

    if ( ! EndOfTransHandling (committed, errlist) )
        return false;

    return true;
}

/* --------------------------------------------------------------------------- */
const char * Log_Transaction::RedoResultString[] =
{
    "Ok",
    "ReadError",
    "WrongEntry",
    "ExecuteError",
    "EndOfTransFailed",
    "Aborted"
};

/* -------------------------------------------------------------------------------- */
Log_Transaction::RedoResult Log_Transaction::Redo(SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::Redo", LogTrans_Trace, 5);

    SAPDBERR_ASSERT_STATE ( IsInitializedForRedo() );

    SAPDBERR_ASSERT_STATE ( m_pRedoFile != NULL );

    // don't  remove undo file here, remove it at trans-end

    SAPDBTRACE_IF (LogTrans_Trace, 6, WriteToTrace ("CurrentTrans"));
    
    if ( ! m_pRedoFile->IsCreated() )
    {
        if ( ! EndOfTransHandling(committed, errlist) )
            return redoEndOfTransFailed;
        return redoOk;
    }

    Log_AfterImage          AfterImage;
    Log_EntrySequence       CurrSequence       = LOG_FIRST_ENTRY_SEQUENCE;
    Log_IOSequenceNo        IOSequence;
    bool                    redoDerefResult = true;
    Rst_RedoFile::Iterator& iter = m_pRedoFile->GetFirstRedoEntry(redoDerefResult, errlist);
    if ( ! redoDerefResult )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return redoReadError;
    }
    Data_SplitSpaceReader   Reader (iter.Deref(redoDerefResult, errlist), true); // PTS 1114994 UH 2002-04-17 added releasable
    if ( ! redoDerefResult )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return redoReadError;
    }
    if ( m_pTrans->trError_gg00 != e_ok ) // PTS 1117126 UH 2002-08-07
    {
        iter.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        (void)iter.Invalidate(errlist);
        return redoReadError;
    }
    while ( iter.IsValid() )
    {
        Reader.Reset();
        
        if ( ! m_pRedoFile->GetEntryInfo (Reader, IOSequence, CurrSequence, errlist) )
        {
            iter.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            (void)iter.Invalidate(errlist);
            return redoReadError;
        }

        if(!AfterImage.ReadPersistentFormat (Reader, errlist))
        {
            iter.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            (void)iter.Invalidate(errlist);
            return redoReadError;
        }

        if ( m_Context.TransNo.gg90GetInt4() != AfterImage.GetTransNo().gg90GetInt4() )
        {
            errlist = Log_Exception(__CONTEXT__,LOG_REDO_WRONG_TRANSNO,
                                    SAPDB_ToString(m_Context.TransNo.gg90GetInt4()),
                                    SAPDB_ToString(AfterImage.GetTransNo().gg90GetInt4()));
            iter.AppendContextInfo(__CONTEXT__,errlist);
            Reader.AppendContextInfo(__CONTEXT__,errlist);
            AfterImage.AppendContextInfo(errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            (void)iter.Invalidate(errlist);
            return redoWrongEntry;
        }

        if ( AfterImage.IsDone() )
        {
            SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "REDO-INFO: entry skipped");
        }
        else
        {
            switch ( Log_ActionExecuteRedo (*this, AfterImage, IOSequence, Reader, iter, errlist) ) // PTS ? 2005-01-03
            {
                 case Log_ActionExecuteRedoOk:
                    AfterImage.SetIsDone();
                    break;
                 case Log_ActionExecuteRedoIsAborted: // PTS ? 2005-01-03
                    (void)iter.Invalidate(errlist);
                    return redoAborted;
                 case Log_ActionExecuteRedoError:
                     return redoExecuteError;
                 default:
                    (void)iter.Invalidate(errlist);
                    return redoWrongEntry;
            }
        }

        if(!iter.PlusPlus(errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)iter.Invalidate(errlist);
            return redoReadError;
        }
        (void)iter.Deref(redoDerefResult,errlist); // really assign the space
        if(!redoDerefResult)
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)iter.Invalidate(errlist);
            return redoReadError;
        }
    }
    
    // release all resources (pages)
    if(!iter.Invalidate(errlist))
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return redoReadError;
    }

    if ( ! EndOfTransHandling (committed, errlist) )
        return redoEndOfTransFailed;

    return redoOk;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::RedoPartialRollback (Log_EntrySequence     UndoStopSequence,
                                           Log_EntrySequence     RedoStopSequence,
                                           SAPDBErr_MessageList &errlist)
{
    // PTS 1113230 UH 2001-12-20 new (from TruncateRedoFile)
    
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::RedoPartialRollback", LogTrans_Trace, 5);

	if ( GetTransState() != opened )
        // PTS ? UH 2004-12-16
        // if last entry of log is a partial rollback and redo of redo is running
        // the trans may be already marked for rollback and not open any more.
        return true;


    SAPDBERR_ASSERT_STATE   ( m_pRedoFile != NULL );
    
	if ( LogTrans_Trace.TracesLevel(6) )
    {
        Kernel_VTrace() << "UndoStopSequence: " << UndoStopSequence
                        << ", RedoStopSequence: " << RedoStopSequence;
        WriteToTrace("RedoPartialRollback");
    }

    // PTS 1113230 UH 2001-12-20 begin
    if ( m_Context.LastUndoEntrySequence.IsValid()
         &&
         UndoStopSequence.IsValid()
         &&
         UndoStopSequence <= m_Context.LastUndoEntrySequence )
    {
        tgg91_TransNo auxTransNo = m_pTrans->trWriteTransId_gg00;
        
        m_pTrans->trWriteTransId_gg00 = m_Context.TransNo;
        
        // The partial rollback needs to go before the last savepoint.
        // These changes have to be rollbacked.
        m_pUndoFile = new (Allocator()) Log_UndoFile ( *m_pTrans,
                                                       m_Context.UndoFileRootPageNo,
                                                       m_Context.UndoFileLastPageNo );
        if ( m_pUndoFile == NULL )
        {
            errlist = Log_Exception(__CONTEXT__,LOG_ALLOCATION_FAILED, "UndoFile");
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        m_pTrans->trLogContext_gg00 = this;
        RollbackResult result = Rollback (command, LOG_NIL_SUBTRANS_NO, UndoStopSequence, errlist);
        if ( result != undoOk )
        {
            (void)m_pUndoFile->Invalidate(errlist);
            destroy(m_pUndoFile, Allocator());

            errlist = errlist + Log_Exception(__CONTEXT__,LOG_REDO_PARTIAL_ROLLBACK_FAILED,
                                              RollbackResultString[result]);
            return false;
        }
        (void)m_pUndoFile->Invalidate(errlist);
        destroy(m_pUndoFile, Allocator());
        m_pTrans->trLogContext_gg00   = NULL;
        m_pTrans->trWriteTransId_gg00 = auxTransNo;
    }
    // PTS 1113230 UH 2001-12-20 end
        
    if ( ! m_pRedoFile->IsCreated() )
        // The partial rollback entry may be the first entry read after the savepoint
        // for a transaction which was open during savepoint.
        // So there is no redofile which can be truncated.
        return true;
    
    // StopSequence is the last Sequence which was rollbacked
    if ( RedoStopSequence > 0 ) // More then first entry may not be rollbacked/truncated. e.g. NewObj writes only UndoEntry
        m_Context.LastCopiedRedoEntrySequence = RedoStopSequence-1;
    else
        m_Context.LastCopiedRedoEntrySequence.Invalidate();
    // the truncate synchronizes with the prepare phase of the savepoint
    // Truncate() truncates everything which is greater then the given sequence
    if (! m_pRedoFile->Truncate(m_Context.LastCopiedRedoEntrySequence, errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    // After truncation the context value must be updated
    m_Context.RedoFileLastPageNo = m_pRedoFile->LastPageNo(); // PTS 1111344 UH 2001-08-10
    return true;
}

/* -------------------------------------------------------------------------------- */
void Log_Transaction::SetEOTInRedo (TransState        eotState,
                                    Log_IOSequenceNo  eotSequence)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::SetEOTInRedo", LogTrans_Trace, 5);
    
    SAPDBERR_ASSERT_STATE   ( m_pRedoFile != NULL   );
    SAPDBERR_ASSERT_STATE   ( m_TransType != online );
    
    m_Context.TransStatePersistent = eotState; // PTS 1114721 UH 2002-03-07 added again
    m_Context.EOT_IOSequenceNo     = eotSequence;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::IsSavepointRelevant()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::IsSavepointRelevant", LogTrans_Trace, 5);
    
    bool isRelevant = (SAPDB_MAX_INT4 != m_Context.UndoFileRootPageNo
                       ||
                       SAPDB_MAX_INT4 != m_Context.RedoFileRootPageNo);

    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "isRelevant: " << isRelevant);
    
    return isRelevant;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::GiveUndoFileToHistory(SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::GiveUndoFileToHistory", LogTrans_Trace, 5);

    // const bool isNonLivecacheTrans = RTETask_ConnectionType(m_pTrans->trTaskId_gg00) == RTE_OLTP_CONNECT;

    if ( m_pUndoFile->GarbageCollectionNeeded() || m_pUndoFile->GetPageCount() > 1 )
    {
        kb50EnterTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
        m_Context.UndoFileRootPageNo = SAPDB_MAX_INT4;
        kb50LeaveTransRegion (m_pTrans->trTaskId_gg00, m_pTrans->trIndex_gg00);
        
        if(!Log_History::GetInstance().RegisterUndoFile (*m_pTrans, *m_pUndoFile, errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)m_pUndoFile->Invalidate(errlist);
            return false;
        }
        /* If the undo is small and history is not needed but isRelevantForGC it could be self-removed
        if( ! Log_History::GetInstance().RemoveUndoFileByTransaction ( *m_pTrans,
                                                                       m_pTrans->trWriteTransId_gg00,
                                                                       m_Context.UndoFileRootPageNo,
                                                                       m_Context.UndoFileLastPageNo,
                                                                       errlist))
        {
            AppendContextInfo(__CONTEXT__,errlist);
            (void)m_pUndoFile->Invalidate(errlist);
            return false;
        }
        */
    }
    else
    {
        if ( ! DropUndoFile(errlist) )
            return false;
    }

    (void)m_pUndoFile->Invalidate(errlist);

    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::UpdateUndoRedoInfoForRestart(const Log_IOSequenceNo  oldestKnownIOSeq,
                                                   SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::UpdateUndoRedoInfoForRestart", LogTrans_Trace, 5);

    if ( ! IsInitializedForRedoCopy() )
    {
        errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                     "IsInitializedForRedoCopy() failed");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    m_pUndoFile = new (Allocator()) Log_UndoFile ( *m_pTrans,
                                                   m_Context.UndoFileRootPageNo,
                                                   m_Context.UndoFileLastPageNo );
    if ( 0 == m_pUndoFile )
    {
        errlist = Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                     "allocation of Log_UndoFile failed");
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( m_pUndoFile->IsCreated() )
    {
        if ( ! m_pUndoFile->FindLastPageNo(errlist, true) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        #ifdef SAPDB_QUICK
        Kernel_VTrace() << "UpdateUndoRedoInfoForRestart: UndoFileLastPageNo "
                        << m_Context.UndoFileLastPageNo << " -> "
                        << m_pUndoFile->LastPageNo() << FlushLine;
        #endif

        m_Context.UndoFileLastPageNo = m_pUndoFile->LastPageNo();
        
        Log_EntrySequence newlastentrysequence;
        
        if( ! m_pUndoFile->GetLastEntrySequence(newlastentrysequence,errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        
        #ifdef SAPDB_QUICK
        Kernel_VTrace() << "UpdateUndoRedoInfoForRestart: LastUndoEntrySequence "
                        << m_Context.LastUndoEntrySequence << " -> "
                        << newlastentrysequence << FlushLine;
        #endif

        m_Context.LastUndoEntrySequence = newlastentrysequence;
    }

    (void)m_pUndoFile->Invalidate(errlist);
    destroy (m_pUndoFile, Allocator());
    m_pUndoFile = NULL;

    if ( m_pRedoFile->IsCreated() )
    {
        if ( ! m_pRedoFile->FindLastPageNo(errlist, true) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        #ifdef SAPDB_QUICK
        Kernel_VTrace() << "UpdateUndoRedoInfoForRestart: RedoFileLastPageNo "
                        << m_Context.RedoFileLastPageNo << " -> "
                        << m_pRedoFile->LastPageNo() << FlushLine;
        #endif

        m_Context.RedoFileLastPageNo = m_pRedoFile->LastPageNo();
        
        Log_EntrySequence newlastentrysequence;
        
        if( ! m_pRedoFile->GetLastEntrySequence(newlastentrysequence,errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        
        #ifdef SAPDB_QUICK
        Kernel_VTrace() << "UpdateUndoRedoInfoForRestart: LastCopiedRedoEntrySequence "
                        << m_Context.LastCopiedRedoEntrySequence << " -> "
                        << newlastentrysequence << FlushLine;
        #endif

        m_Context.LastCopiedRedoEntrySequence = newlastentrysequence;
    }
    
    m_OldestKnownIOSequence = oldestKnownIOSeq; //PTS 1127704 mb 2004-02-16
    
    return true;
}

/* -------------------------------------------------------------------------------- */
Data_SplitSpaceWriter Log_Transaction::GetWriterForLocalRedoEntries(SAPDB_Byte           *pActionHead,
                                                                    SAPDB_UInt            sizeToReserve,
                                                                    SAPDBErr_MessageList &errlist)
{
	// PTS 1131107 UH 2004-08-24 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::GetWriterForLocalRedoEntries", LogTrans_Trace, 5);
    if (! m_LocalRedoEntries.Reserve(pActionHead, sizeToReserve) )
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_LocalRedoEntries.Reserve()");
    return Data_SplitSpaceWriter(m_LocalRedoEntries.GetBuffer());
}

/* -------------------------------------------------------------------------------- */
bool Log_Transaction::RestoreLocalRedoEntries (const Log_IOSequenceNo      iosequence,
                                                     SAPDBErr_MessageList &errlist)
{
	// PTS 1131107 UH 2004-08-24 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Transaction::RestoreLocalRedoEntries", LogTrans_Trace, 5);

    Log_AfterImage                image;
    SAPDB_UInt                    actionLength;
    const SAPDB_Byte             *pActionSpace;
    Log_ActionMultiple::Iterator  actioniter  = m_LocalRedoEntries.GetFirstAction();

    #ifdef SAPDB_SLOW
    m_LocalRedoEntries.WriteToTrace("Log_Transaction::RestoreLocalRedoEntries()");
    SAPDB_UInt actionno = 1;
    #endif
    
    while ( actioniter.GetAction(image, GetRedoTransNo(), actionLength, pActionSpace) )
    {
        #ifdef SAPDB_SLOW
        Kernel_VTrace() << "MultipleActions: internal #" << actionno++;
        image.WriteToTrace("IMAGE");
        Kernel_VTrace() << "GetLastCopiedRedoEntrySequence() = " << GetLastCopiedRedoEntrySequence();
        #endif
        if ( actionLength > LocalRedoBufferSize )
        {
            Kernel_VTrace() << "actionLength is invalid" << actionLength;
            return false;
        }
        
        if ( (m_Context.LastRedoEntrySequence.IsInvalid()
              ||
              image.GetSequence() >= m_Context.LastRedoEntrySequence)
             &&
             (GetLastCopiedRedoEntrySequence().IsInvalid()
              ||
              image.GetSequence() > GetLastCopiedRedoEntrySequence()) )
        {
            #ifdef SAPDB_SLOW
            Kernel_VTrace() << "IS COPIED";
            #endif
            errlist.ClearMessageList();
            Data_SplitSpaceWriter writer = ReserveSpaceForRedoEntryCopy
                                ( actionLength + image.GetPersistentLength(),
                                  Log_ActionGetMinCopyLength (image.GetActionType(), image.GetPersistentVersion())
                                  + image.GetPersistentLength(),
                                  image.GetSequence(),
                                  iosequence,
                                  errlist );
            if ( ! errlist.IsEmpty() )
                return false;
                
            if ( ! image.WritePersistentFormat(writer, errlist) )
                return false;
                
            if ( writer.Write (pActionSpace, actionLength)
                 !=
                 Data_SplitSpaceWriter::ok )
            {
                ReleaseIncompleteRedoEntry(errlist);
                return false;
            }
            FinishCopiedRedoEntry(errlist);
        }
        else
        {
            #ifdef SAPDB_SLOW
            Kernel_VTrace() << "IS NOT COPIED";
            #endif
        }
        actioniter.Next();
    }
    m_LocalRedoEntries.Reset();
    return true;
}

// -----------------------------------------------------------------------------
void Log_Transaction::DetermineLocalRedoBufferSize()
{
	// PTS 1131107 UH 2004-08-24
    const SAPDB_UTF8 *         paramName    = UTF8("LOCAL_REDO_LOG_BUFFER_SIZE");
    RTEConf_Parameter::Integer size = 0;
    SAPDBErr_MessageList       error;

    if ( RTEConf_ParameterAccess::Instance() == 0 )
    {
        return;
    }

    if ( ! RTEConf_ParameterAccess::Instance()->GetInteger(paramName, size, error) )
    {
        RTE_Message (error);
        return;
    }
    
    if ( size > SAPDB_MAX_UINT4 )
    {
        RTE_Message(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"size <= SAPDB_MAX_UINT4"));
        return;
    }

    LocalRedoBufferSize = static_cast<SAPDB_UInt>(size);

    if ( LocalRedoBufferSize > 0 )    
        RTE_Message(Log_Exception(__CONTEXT__,LOG_LOCAL_REDO_BUFFER_SIZE_SET,
                                  SAPDB_ToString(LocalRedoBufferSize)));
}

SAPDB_UInt Log_Transaction::GetLocalRedoBufferSize()
{
    return LocalRedoBufferSize ;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
