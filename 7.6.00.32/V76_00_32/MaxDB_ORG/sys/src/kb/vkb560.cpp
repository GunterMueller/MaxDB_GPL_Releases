/*!***************************************************************************

  module      : vkb560.cpp
  responsible : UweH
  special area: KB_Logging
  last changed: 2000-03-15
  Copyright (c) 2000-2005 SAP AG
  see also    :
  description : interface to new Logging component



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


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo56.h"   // vsleep
#include "heo58.h"   // tsp0058_RTEInfo
#include "hgg01_3.h" // g01pid
#include "hbd91.h"   // bd91StartOMSGarbageCollection
#include "hkb57_1.h" // k57restartrec
#include "hkb57.h"   // save_restart_rec
#include "vsp0031.h" // needed by hkb560.h
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_History.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "IOManager/IOMan_ILogInfo.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Transaction/Trans_Context.hpp"

#include "hkb560.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *   LOCAL FUNCTIONS (PROTOTYPES)                                            *
 *===========================================================================*/

/*-----------------------------------------------------------------------------------*/

inline Log_SavepointReason MapToSavepointReason (tgg00_MessType2_Param MessType2)
{
    switch (MessType2)
    {
    case mm_user:
        return Log_SVPReasonUserRequested;
    case mm_trans:
        return Log_SVPReasonTransRequested;
    case mm_end_read_only:
        return Log_SVPReasonEndReadOnly;
    case mm_read:
        return Log_SVPReasonEndLoad;
    case mm_index:
        return Log_SVPReasonCreateIndex;
    case mm_test:
        return Log_SVPReasonCheckTable;
    case mm_config:
        return Log_SVPReasonCatalogMigration;
    default:
        return Log_SVPReasonUnknown;
    }
}

/*-----------------------------------------------------------------------------------*/

inline Log_Transaction&
GetLogTrans (tgg00_TransContext &Trans,
                   const char *        Msg)
{
    Log_Transaction* LogTrans = REINTERPRET_CAST(Log_Transaction*,Trans.trLogContext_gg00);
    if ( LogTrans == NULL )
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, Msg) );
    return *LogTrans;
}

/*-----------------------------------------------------------------------------------*/

inline void HandleErrlist(SAPDBErr_MessageList &errlist)
{
    if ( SQLMan_Context::GetContext() == 0 )
    {
        RTE_Message(errlist);
        return;
    }

    SAPDBErr_MessageList &globalErrlist = SQLMan_Context::GetContext()->GetErrorList();
    if ( ! globalErrlist.IsEmpty() )
        RTE_Message(globalErrlist);
    globalErrlist = errlist;
}
/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560ResetStatistics (tsp00_TaskId taskid)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560ResetStatistics", LogVolume_Trace, 5);
    if ( ! Log_Volume::IsReadyForWriting() )
        return;
    Log_Volume::Instance().ResetStatistics(taskid);
}

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560GetStatistics (tsp00_TaskId         taskId,
                    tkb00_LogDeviceInfo &LogDeviceInfo,
                    tkb00_LogQueueInfo  &LogQueueInfo)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetStatistics", LogVolume_Trace, 5);

    LogDeviceInfo.ldiPagesUsed_kb00     = 0;
    LogDeviceInfo.ldiPagesNotSaved_kb00 = 0;
    LogDeviceInfo.ldiPagesSinceBup_kb00 = 0;
    LogDeviceInfo.ldiSaveptCount_kb00   = 0;

    LogQueueInfo.lqiCountGroup_kb00      = 0;
    LogQueueInfo.lqiCountInsert_kb00     = 0;
    LogQueueInfo.lqiCountRequest_kb00    = 0;
    LogQueueInfo.lqiCountWait_kb00       = 0;
    LogQueueInfo.lqiAvgSumPagewaits_kb00 = 0;
    LogQueueInfo.lqiLogQueueSize_kb00    = 0;
    LogQueueInfo.lqiMaxQueueUsed_kb00    = 0;
    LogQueueInfo.lqiMaxWaitLogpage_kb00  = 0;

    pasbool     available;
    SAPDB_UInt4 deviceSize;
    SAPDB_UInt4 auxUsed;
    SAPDB_UInt4 auxNotSaved;

    kb560GetArchiveLogDeviceInfo ( taskId, 0, available,
                                   deviceSize,
                                   auxUsed,
                                   auxNotSaved );
                                   
    LogDeviceInfo.ldiPagesUsed_kb00     = auxUsed;
    LogDeviceInfo.ldiPagesNotSaved_kb00 = auxNotSaved;
    LogDeviceInfo.ldiSaveptCount_kb00   = Log_Savepoint::GetLastSavepointId();

    if ( available == 0 )
        return;
        
    SAPDB_UInt4 auxWaitForQueueFree;
    SAPDB_UInt4 auxQueueSize;
    SAPDB_UInt4 auxMaxQueueUsed;
    SAPDB_UInt4 auxCountRequest;
    SAPDB_UInt4 auxCountWaitForIO;
    SAPDB_UInt4 auxMaxWait;
    SAPDB_UInt4 auxCountGroupCommit;
    
    kb560GetArchiveLogQueueInfo ( taskId, 0, available,
                                  auxQueueSize,
                                  auxMaxQueueUsed,
                                  auxCountRequest,
                                  auxCountWaitForIO,
                                  auxCountGroupCommit,
                                  auxWaitForQueueFree,
                                  auxMaxWait );

    LogQueueInfo.lqiLogQueueSize_kb00   = auxQueueSize;
    LogQueueInfo.lqiMaxQueueUsed_kb00   = auxMaxQueueUsed;
    LogQueueInfo.lqiCountInsert_kb00    = auxCountRequest;
    LogQueueInfo.lqiCountWait_kb00      = auxCountWaitForIO;
    LogQueueInfo.lqiCountRequest_kb00   = auxWaitForQueueFree;
    LogQueueInfo.lqiMaxWaitLogpage_kb00 = auxMaxWait;

    LogQueueInfo.lqiCountGroup_kb00     = auxCountGroupCommit;
    if ( auxCountRequest > 0 )
        LogQueueInfo.lqiAvgSumPagewaits_kb00 = auxCountWaitForIO / auxCountRequest;
} 

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560GetArchiveLogDeviceInfo (tsp00_TaskId   taskId,
                              SAPDB_UInt4    logno,
                              pasbool       &available,
                              SAPDB_UInt4   &deviceSize,
                              SAPDB_UInt4   &pagesUsed,
                              SAPDB_UInt4   &pagesNotSaved)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetArchiveLogDeviceInfo", LogVolume_Trace, 5);

    available             = 1;
    deviceSize            = 0;
    pagesUsed             = 0;
    pagesNotSaved         = 0;
    SAPDB_UInt pendingLogSize;
    Log_Volume::Instance().GetDeviceUsage (deviceSize, pagesUsed, pagesNotSaved, pendingLogSize);
}

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560GetArchiveLogQueueInfo (tsp00_TaskId    taskId,
                             SAPDB_UInt4     logno,
                             pasbool        &available,
                             SAPDB_UInt4    &queueSize,
                             SAPDB_UInt4    &maxPagesUsedInQueue,
                             SAPDB_UInt4    &countReserveRequests,
                             SAPDB_UInt4    &countWaitForIO,
                             SAPDB_UInt4    &auxCountGroupCommit,
                             SAPDB_UInt4    &countWaitForQueueFree,
                             SAPDB_UInt4    &maxWaitForPageIO)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetArchiveLogQueueInfo", LogVolume_Trace, 5);

    available             = 1;
    queueSize             = 0;
    maxPagesUsedInQueue   = 0;
    countReserveRequests  = 0;
    countWaitForIO        = 0;
    countWaitForQueueFree = 0;
    maxWaitForPageIO      = 0;
    Log_Volume::Instance().GetQueueUsage ( queueSize,
                        maxPagesUsedInQueue,
                        countReserveRequests,
                        countWaitForIO,
                        auxCountGroupCommit,
                        countWaitForQueueFree,
                        maxWaitForPageIO );
}

/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetLogDBIdent (             // PTS 1113550 mb 2002-03-22 new
    tsp00_Line            VAR_ARRAY_REF  dbident)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetLogDBIdent", LogVolume_Trace, 5);
    SAPDB_Char *auxdbident = reinterpret_cast<SAPDB_Char*>(&dbident);
    Log_Volume::Instance().GetLogDBIdent (auxdbident);
}    

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560DumpTransaction (Log_Transaction *pTrans,
                      void*           &pData,
                      SAPDB_UInt4     &dataLength)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560DumpTransaction", LogTrans_Trace, 5);
    Log_Transaction::GetDumpInfo (pTrans, pData, dataLength);
}
                            
/*-----------------------------------------------------------------------------------*/
externPascal pasbool
kb560VerifyOpenTransAndHistory (tgg00_TransContext &trans,
                                pasbool             isCold)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560VerifyOpenTransAndHistory", LogTrans_Trace, 5);
    SAPDBErr_MessageList errlist;
    bool result = Log_Savepoint::Verify (trans, isCold == 1, errlist);
    result &= Log_History::GetInstance().Verify(trans, isCold == 1, errlist);
    HandleErrlist(errlist);
    return result;
}
                            
/*-----------------------------------------------------------------------------------*/
externPascal void
kb560CreateHistoryManager (tgg00_TransContext &Trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560CreateHistoryManager", LogHistory_Trace, 5);
    SAPDBErr_MessageList errlist;
    if ( ! Log_History::CreateInstance (Trans,errlist) )
    {
        HandleErrlist(errlist);
        if(Trans.trError_gg00==e_ok) Trans.trError_gg00 = e_no_more_memory;
    }
}

/*-----------------------------------------------------------------------------------*/
externPascal void kb560DropHistoryManager ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560DropHistoryManager", LogHistory_Trace, 5);
	Log_History::DropInstance();
}

/*-----------------------------------------------------------------------------------*/
externPascal pasbool kb560TransactionIsOpen (void* TransPtr)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560TransactionIsOpen", LogTrans_Trace, 5);
    if ( NULL == TransPtr )
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"TransPtr is assigned") );
	// PTS 1114125 UH 2002-02-12 use of IsOpenInRedo()
    Log_Transaction &trans = *(reinterpret_cast<Log_Transaction*>(TransPtr));
    return trans.IsOpenInRedo();
}

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560InitSavepointParam (tkb00_SaveptParam &param)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560InitSavepointParam", LogTrans_Trace, 5);

    param.svpId_kb00               = 0;
    param.svpReason_kb00           = Log_SVPReasonUnknown;
    param.svpFiller1_kb00          = 0;
    param.svpOpenTransCnt_kb00     = 0;

    param.svpIOsequence_kb00       = SAPDB_MAX_UINT4;
    param.svpStartOffset_kb00      = SAPDB_MAX_UINT4;

    param.svpStartEntryOffset_kb00 = 0;
    param.svpStartEntryType_kb00   = Log_NoOp;

    param.svpFiller2_kb00          = 0;
    param.svpFiller3_kb00          = 0;

    param.svpOpenTransRoot_kb00    = NIL_PAGE_NO_GG00;
    param.svpHistoryRoot_kb00      = NIL_PAGE_NO_GG00;

    param.svpDate_kb00             = KERNEL_INVALID_DATE;
    param.svpTime_kb00             = KERNEL_INVALID_TIME;

    param.svpStartEntryDate_kb00   = KERNEL_INVALID_DATE;
    param.svpStartEntryTime_kb00   = KERNEL_INVALID_TIME;

    SAPDB_MemFillNoCheck (&param.svpFiller4_kb00, 0, sizeof(param.svpFiller4_kb00));

    param.svpFiller5_kb00          = 0;
    param.svpFiller6_kb00          = 0;
}
/*-----------------------------------------------------------------------------------*/
externPascal void
kb560StartSavepoint (Trans_Context         &context,
                     tgg00_MessType2_Param  MessType2)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560StartSavepoint", LogTrans_Trace, 5);
    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepoint (context.Task(), MapToSavepointReason(MessType2), context.ErrorList()) )
        context.OldTrans().trError_gg00 = e_start_server_task_error;
}

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560StartSavepointAndWait (Trans_Context         &context,
                            tgg00_MessType2_Param  MessType2)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560StartSavepointAndWait", LogTrans_Trace, 5);
    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait (context.Task(), MapToSavepointReason(MessType2), context.ErrorList()) )
        context.OldTrans().trError_gg00 = e_start_server_task_error;
}

/*-----------------------------------------------------------------------------------*/
externPascal void
kb560SavepointCoordinator (Trans_Context &context)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560SavepointCoordinator", LogTrans_Trace, 5);
    Log_Savepoint::Instance().Coordinator (context.OldTrans());
}

/*-----------------------------------------------------------------------------------*/

externPascal void
kb560CreateLogTransaction (tgg00_TransContext &Trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560CreateLogTransaction", LogTrans_Trace, 5);

    SAPDBMem_IRawAllocator& Allocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00));

    Log_Transaction      *LogTrans = new (Allocator) Log_Transaction ();
    SAPDBErr_MessageList  errlist;

    if ( LogTrans == NULL )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"allocate of logtrans");
        HandleErrlist(errlist);
        if(Trans.trError_gg00==e_ok) Trans.trError_gg00 = e_no_more_memory;
        return;
    }

    if ( ! LogTrans->Initialize(Trans,errlist) )
    {
        HandleErrlist(errlist);
        if(Trans.trError_gg00==e_ok) Trans.trError_gg00 = e_no_more_memory;
        return;
    }
    
    Trans.trLogContext_gg00 = LogTrans;
}

/*-----------------------------------------------------------------------------------*/

externPascal void
kb560DeleteLogTransaction (tgg00_TransContext &Trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560DeleteLogTransaction", LogTrans_Trace, 5);

    SAPDBErr_MessageList  errlist;
    Log_Transaction      *LogTrans = REINTERPRET_CAST(Log_Transaction*,Trans.trLogContext_gg00);
    if ( LogTrans == NULL ) //PTS 1125799 mb 2003-12-02 tolerate NULL-transaction
    {
        Kernel_VTrace() << "kb560DeleteLogTransaction: trying to delete NULL transaction" 
                        << FlushLine;
    }
    else
    {
        SAPDBMem_IRawAllocator& allocator = LogTrans->Allocator();
        if(!LogTrans->Delete(errlist))
        {
            HandleErrlist(errlist);
            if(Trans.trError_gg00==e_ok) Trans.trError_gg00 = e_no_more_memory;
            return;
        }
        destroy(LogTrans, allocator);
        Trans.trLogContext_gg00 = NULL;
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void
kb560CommitLogTransaction (tgg00_TransContext &Trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560CommitLogTransaction", LogTrans_Trace, 5);
    SAPDBErr_MessageList errlist;
    if (! GetLogTrans (Trans, "CommitLogTrans: ptr=nil ").Commit(errlist) )
    {
        HandleErrlist(errlist);
        if(Trans.trError_gg00==e_ok)
            Trans.trError_gg00 = e_redo_log_write_error;
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void
kb560RollbackLogTransAction (tgg00_TransContext    &Trans,
                             tgg00_MessType2_Param  RollbackKind,
                             tgg00_SubtransNo       SubtransNo)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560RollbackLogTransaction", LogTrans_Trace, 5);

    Log_Transaction::RollbackKind newRollbackKind;
    switch ( RollbackKind )
    {
    case mm_rollback:
        newRollbackKind = Log_Transaction::endOfTrans;
        break;
    case mm_nil:
        newRollbackKind = Log_Transaction::command;
        break;
    case mm_subtrans:
        newRollbackKind = Log_Transaction::subTrans;
        break;
    default:
        SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "RollbackKind: " << RollbackKind);
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                      "unknown RollbackKind") );
    }

    SAPDBErr_MessageList errlist;

    Log_Transaction::RollbackResult Result =
        GetLogTrans (Trans, "RollbackLogTrans:ptr=nil").Rollback (newRollbackKind,
                                                                  SubtransNo,
                                                                  Log_EntrySequence(),
                                                                  errlist);

    Trans.trError_gg00 = kb560ConvertRollbackResultToBasisError (Result);

    if ( Trans.trError_gg00 != e_ok )
        HandleErrlist(errlist);
}

/*-----------------------------------------------------------------------------------*/

externPascal tgg00_BasisError kb560ConvertRollbackResultToBasisError (
    tsp00_Int4                           result)
{
    switch ( Log_Transaction::RollbackResult(result) )
    {
    case Log_Transaction::undoOk:
        return e_ok;
    case Log_Transaction::undoNoOpenSubtrans:
        return e_no_open_subtrans;
    case Log_Transaction::undoLogFull:
        return e_log_full;
    default:
        // case undoReadError, undoWrongEntry, undoExecuteError, undoWriteError
        return e_undo_log_execute_error;
    }
}

/*-----------------------------------------------------------------------------------*/
externPascal tgg00_BasisError kb560ConvertWriteRedoEntryResultToBasisError (
    tsp00_Int4                           result)
{
    switch ( Log_Transaction::LogState(result) )
    {
    case Log_Transaction::noError:
        return e_ok;
    case Log_Transaction::logFull:
        return e_log_full;
    case Log_Transaction::logError:
        return e_redo_log_write_error;
    default:
        return e_redo_log_write_error;
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void
kb560BeginSubtransLogTransaction (tgg00_TransContext &Trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560BeginSubtransLogTransaction", LogTrans_Trace, 5);
    GetLogTrans (Trans, "BeginSubtLogTrns:ptr=nil").BeginSubtrans();
}

/*-----------------------------------------------------------------------------------*/

externPascal void
kb560EndSubtransLogTransaction (tgg00_TransContext &Trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560EndSubtransLogTransaction", LogTrans_Trace, 5);
    if ( ! GetLogTrans (Trans, "EndSubtrLogTrans:ptr=nil").EndSubtrans() )
        Trans.trError_gg00 = e_no_open_subtrans;
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb560GetFirstKnownIOSeqBeforeRestoreLog (
    Log_IOSequenceNo&                    FirstIOSeq,
    pasbool               VAR_VALUE_REF  firstIOSeqIsValid)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetFirstKnownIOSeqBeforeRestoreLog", LogVolume_Trace, 5);

    Log_Volume &log  = Log_Volume::Instance();
    
    Log_RawDeviceOffset     offset;
    log.GetFirstKnownBeforeRestoreLog(offset,FirstIOSeq);
    
    firstIOSeqIsValid = FirstIOSeq.IsValid();
}    

/*-----------------------------------------------------------------------------------*/

externPascal pasbool kb560GetLogInfo (
    tsp00_TaskId                         taskid,
    tsp00_Int4            VAR_VALUE_REF  date,
    tsp00_Int4            VAR_VALUE_REF  time,
    tsp00_Int4            VAR_VALUE_REF  flushmode,
    tsp00_Int4            VAR_VALUE_REF  devicestate,
    tsp00_Uint4           VAR_VALUE_REF  queuecount,
    tsp00_Uint4           VAR_VALUE_REF  cycleendoffset,
    tsp00_Uint4           VAR_VALUE_REF  oldestnotsavedoffset,
    Log_IOSequenceNo&                    oldestnotsavedsequence,
    tsp00_Uint4           VAR_VALUE_REF  lastknownoffset,
    Log_IOSequenceNo&                    lastknownsequence,
    tsp00_Uint4           VAR_VALUE_REF  backupcount,
    tsp00_Int4            VAR_VALUE_REF  untildate,
    tsp00_Int4            VAR_VALUE_REF  untiltime,
    tsp00_Uint4           VAR_VALUE_REF  untilclearloweroffset,
    tsp00_Uint4           VAR_VALUE_REF  untilclearupperoffset,
    Log_IOSequenceNo&                    untilsequenceafterredo,
    Log_IOSequenceNo&                    savepointsequence,
    tsp00_Uint4           VAR_VALUE_REF  savepointdevoffset,
    tsp00_Uint4           VAR_VALUE_REF  savepointoffset)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetLogInfo", LogVolume_Trace, 5);
    Log_Volume &log = Log_Volume::Instance();

    Kernel_Date              auxdate;
    Kernel_Time              auxtime;
    Log_FlushMode            auxflushmode;
    Log_DeviceState          auxdevicestate;
    SAPDB_UInt2              auxqueuecount;
    Log_RawDeviceOffset      auxcycleendoffset;
    Log_RawDeviceOffset      auxoldestnotsavedoffset;
    Log_IOSequenceNo         auxoldestnotsavedsequence;
    Log_RawDeviceOffset      auxlastknownoffset;
    Log_IOSequenceNo         auxlastknownsequence;
    SAPDB_UInt4              auxbackupcount;
    Kernel_Date              auxuntildate;
    Kernel_Time              auxuntiltime;
    Log_RawDeviceOffset      auxuntilclearloweroffset;
    Log_RawDeviceOffset      auxuntilclearupperoffset;
    Log_IOSequenceNo         auxuntilsequenceafterredo;
    Log_IOSequenceNo         auxsavepointsequence;
    Log_RawDeviceOffset      auxsavepointdevoffset;
    Data_PageFrameOffsetType auxsavepointoffset;

    bool isvalid = log.GetLogInfo ( taskid,
                         auxdate,
                         auxtime,
                         auxflushmode,
                         auxdevicestate,
                         auxqueuecount,
                         auxcycleendoffset,
                         auxoldestnotsavedoffset,
                         auxoldestnotsavedsequence,
                         auxlastknownoffset,
                         auxlastknownsequence,
                         auxbackupcount,
                         auxuntildate,
                         auxuntiltime,
                         auxuntilclearloweroffset,
                         auxuntilclearupperoffset,
                         auxuntilsequenceafterredo,
                         auxsavepointsequence,
                         auxsavepointdevoffset,
                         auxsavepointoffset );
    
    if ( ! isvalid )
        return false;
        
    date                   = auxdate;
    time                   = auxtime;
    flushmode              = auxflushmode;
    devicestate            = auxdevicestate;
    queuecount             = auxqueuecount;
    cycleendoffset         = auxcycleendoffset.RawValue();
    oldestnotsavedoffset   = auxoldestnotsavedoffset.RawValue();
    oldestnotsavedsequence = auxoldestnotsavedsequence;
    lastknownoffset        = auxlastknownoffset.RawValue();
    lastknownsequence      = auxlastknownsequence;
    backupcount            = auxbackupcount;
    untildate              = auxuntildate;
    untiltime              = auxuntiltime;
    untilclearloweroffset  = auxuntilclearloweroffset.RawValue();
    untilclearupperoffset  = auxuntilclearupperoffset.RawValue();
    untilsequenceafterredo = auxuntilsequenceafterredo;
    savepointsequence      = auxsavepointsequence;
    savepointdevoffset     = auxsavepointdevoffset.RawValue();
    savepointoffset        = auxsavepointoffset;
    
    return true;
}

/*-----------------------------------------------------------------------------------*/
externPascal pasbool kb560LogIsFull ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560LogIsFull", LogVolume_Trace, 5);
    return Log_Volume::Instance().LogIsFull();
}


/*-----------------------------------------------------------------------------------*/

externPascal void k560check_logsave_allowed (
    tsp00_TaskId                         TaskId,
    tgg00_BasisError      VAR_VALUE_REF  BasisError)
{
    SAPDBTRACE_ROUTINE_DEBUG ("k560check_logsave_allowed", LogVolume_Trace, 5);

    Kernel_Date              date;
    Kernel_Time              time;
    Log_FlushMode            flushmode;
    Log_DeviceState          devicestate;
    SAPDB_UInt2              queuecount;
    Log_RawDeviceOffset      cycleendoffset;
    Log_RawDeviceOffset      oldestnotsavedoffset;
    Log_IOSequenceNo         oldestnotsavedsequence;
    Log_RawDeviceOffset      lastknownoffset;
    Log_IOSequenceNo         lastknownsequence;
    SAPDB_UInt4              backupcount;
    Kernel_Date              untildate;
    Kernel_Time              untiltime;
    Log_RawDeviceOffset      untilclearloweroffset;
    Log_RawDeviceOffset      untilclearupperoffset;
    Log_IOSequenceNo         untilsequenceafterredo;
    Log_IOSequenceNo         savepointsequence;
    Log_RawDeviceOffset      savepointdevoffset;
    Data_PageFrameOffsetType savepointoffset;

    Log_Volume &log = Log_Volume::Instance();

    if (log.GetLogInfo (TaskId,
                              date,
                              time,
                              flushmode,
                              devicestate,
                              queuecount,
                              cycleendoffset,
                              oldestnotsavedoffset,
                              oldestnotsavedsequence,
                              lastknownoffset,
                              lastknownsequence,
                              backupcount,
                              untildate,
                              untiltime,
                              untilclearloweroffset,
                              untilclearupperoffset,
                              untilsequenceafterredo,
                              savepointsequence,
                              savepointdevoffset,
                              savepointoffset ))
    {
        if (Log_DeviceStateOkay == devicestate)
        {
            BasisError = e_ok;
        }
        else
        {
            BasisError = e_backup_history_lost;
            RTE_Message(Log_Exception(__CONTEXT__,LOG_AUTOSAVE_ABORTED_HIST_LOST));
        }
    }
    else
    {
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                 "k560check_logsave_allowed: ArchiveLog not initialized ") );
    }

    // PTS 1128223 mb 2004-04-03 check for AutoOverWrite 
    if (log.IsAutomaticOverwrite(TaskId))
    {
        BasisError = e_log_autooverwrite_on;
        RTE_Message(Log_Exception(__CONTEXT__,LOG_SAVE_DENIED_AUTOOVERWRITE));
    }

    // PTS 1128223 mb 2004-04-03 check for LogWriting switched off
    if (! log.IsDevspaceEnabled(TaskId))
    {
        BasisError = e_log_writing_disabled;
        RTE_Message(Log_Exception(__CONTEXT__,LOG_SAVE_DENIED_WRITING_OFF));
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal tsp00_Int4 kb560GetLogDeviceSize ()
{
    return Log_Volume::Instance().GetDeviceSize();
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb560SetDeviceStateOkay (
    tsp00_TaskId                         taskid,
    tgg00_BasisError      VAR_VALUE_REF  TrError)
{
    // PTS 1113190 UH 2001-12-19 new
    SAPDBTRACE_ROUTINE_DEBUG ("kb560SetDeviceStateOkay", LogVolume_Trace, 5);
    if ( ! Log_Volume::Instance().SetVolumeState(taskid, Log_DeviceStateOkay) )
        TrError = e_disk_not_accessible;
}

/* -------------------------------------------------------------------------------- */
externPascal void kb560GetHistFileInfo (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_Int4            VAR_VALUE_REF  HistFileNo,
    tgg91_TransNo         VAR_VALUE_REF  FirstTransId,
    pasbool               VAR_VALUE_REF  FirstTransIsUsed,
    tgg91_TransNo         VAR_VALUE_REF  LastTransId,
    pasbool               VAR_VALUE_REF  LastTransIsUsed,
    tsp00_Int4            VAR_VALUE_REF  PageCount,
    pasbool               VAR_VALUE_REF  bFileIsEmpty)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetHistFileInfo", LogVolume_Trace, 5);
    Log_History::HistoryFileInfo  info;
    Log_History                  &history = Log_History::GetInstance();
    SAPDB_UInt                    pagecount;

    if ( HistFileNo >= tsp00_Int4(history.GetNumberOfHistoryFiles()) )
    {
        HistFileNo = -1;
        return;
    }
    
    history.GetInfo (Trans.trTaskId_gg00, HistFileNo, info, pagecount);

    FirstTransId     = info.oldestTransNo;
    FirstTransIsUsed = 0;
    LastTransId      = info.youngestTransNo;
    LastTransIsUsed  = 0;
    PageCount        = tsp00_Int4(pagecount); // ++++++++++
    bFileIsEmpty     = info.oldestTransNo.gg90IsNil()
                       &&
                       info.youngestTransNo.gg90IsNil();
                       
    HistFileNo       = HistFileNo + 1;
}

/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetLogTransactionInfo (
    Log_Transaction*                     ptrans,
    tsp00_PageNo          VAR_VALUE_REF  undoroot,
    tsp00_PageNo          VAR_VALUE_REF  undolast,
    tsp00_Int4            VAR_VALUE_REF  undopagecount,
    tsp00_Int4            VAR_VALUE_REF  lastundosequence)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560GetLogTransactionInfo", LogTrans_Trace, 5);

    undoroot         = NIL_PAGE_NO_GG00;
    undolast         = NIL_PAGE_NO_GG00;
    undopagecount    = 0;
    lastundosequence = -1;

    if ( ptrans == NULL ) return;

    Data_PageNo          auxundoroot;
    Data_PageNo          auxundolast;
    SAPDB_UInt           auxundopagecount;
    Log_EntrySequence    auxlastundosequence;
    SAPDBErr_MessageList errlist;

    if(!ptrans->GetInfo ( auxundoroot,
                          auxundolast,
                          auxundopagecount,
                          auxlastundosequence,
                          errlist ) )
	{
        HandleErrlist(errlist);
	}

    undoroot         = auxundoroot;
    undolast         = auxundolast;
    undopagecount    = auxundopagecount;
    lastundosequence = auxlastundosequence;
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb560InvalidateIOSeq (
    Log_IOSequenceNo&                    ioSeq)
{
    // PTS 1116063 mb 2002-06-05 new
    ioSeq.Invalidate();
}

/*-----------------------------------------------------------------------------------*/

externPascal pasbool kb560IsValidIOSeq (
    const Log_IOSequenceNo&                 ioSeq)
{
    // PTS 1116063 mb 2002-06-05 new
    return ioSeq.IsValid();
}

/*-----------------------------------------------------------------------------------*/

externPascal pasbool kb560IsSuccessor (
    Log_IOSequenceNo&                    predecessor,
    Log_IOSequenceNo&                    successor)
// new with PTS 1126639 mb 2003-12-22
{
    if ((predecessor+1) == successor)
    {
        return true;
    }
    else
    {
        RTE_Message(Log_Exception(__CONTEXT__, LOG_NON_CONTINUOUS_IOSEQ,
                                  SAPDB_ToString(predecessor),
                                  SAPDB_ToString(successor)));
        return false;
    }
}

/*-----------------------------------------------------------------------------------*/
externPascal pasbool k560IsSavepointAllowed ()
{
    // PTS 1114053 UH 2002-10-22 new
    SAPDBTRACE_ROUTINE_DEBUG ("k560IsSavepointAllowed", LogVolume_Trace, 5);
	return Log_Savepoint::Instance().IsSavepointAllowed();
}

/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetSavepointStatistics (
    tsp00_Int4            VAR_VALUE_REF  lastDurationSec,
    tsp00_Int4            VAR_VALUE_REF  lastDurationMicroSec,
    tsp00_Int4            VAR_VALUE_REF  lastWrittenPages,
    tsp00_Int4            VAR_VALUE_REF  lastIOcount,
    tsp00_Int4            VAR_VALUE_REF  avgWrittenPages,
    tsp00_Int4            VAR_VALUE_REF  avgIOcount)
{
    // PTS 1119276 UH 2002-12-10 new
    Log_Savepoint::Instance().GetSavepointStatistics (lastDurationSec,
                                                 lastDurationMicroSec,
                                                 lastWrittenPages,
                                                 lastIOcount,
                                                 avgWrittenPages,
                                                 avgIOcount);
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetSavepointReasonString (
    tsp00_Uint1                          reasonid,
    tsp00_C40             VAR_ARRAY_REF  resonstring)
{
    if ( reasonid >= Log_SavepointReason_DO_NOT_USE )
        return;
    SAPDB_UInt copyLength = sizeof(resonstring) > strlen(Log_SavepointReasonStrings[reasonid])
                            ? strlen(Log_SavepointReasonStrings[reasonid])
                            : sizeof(resonstring);
	SAPDB_MemMoveNoCheck (resonstring, Log_SavepointReasonStrings[reasonid], copyLength);
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560InsertToWaitForPrepare (
    tsp00_TaskId                         taskid,
    tgg00_WaitContext     VAR_VALUE_REF  waitcontext)
{
    RTETask_ITask &task = *RTETask_ITask::ByTaskID(taskid);
    Log_Savepoint::Instance().InsertToWaitForPrepare (task, waitcontext);
}
/*-----------------------------------------------------------------------------------*/
externPascal tsp00_Uint4 kb560LogQueueSize ()
{
    return Log_Volume::QueueSize();
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560SetTraceMaxBufferLength (
    tsp00_Int4                           length)
{
    if ( length > 0 )
        Kernel_TraceMaxBufferLength = length;
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetSys1CatalogTreeId(tgg00_FileId VAR_VALUE_REF treeId)
{
    treeId = Catalog_Interface::GetInstance().GetSys1CatalogTreeId();
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetSys2CatalogTreeId(tgg00_FileId VAR_VALUE_REF treeId)
{
    treeId = Catalog_Interface::GetInstance().GetSys2CatalogTreeId();
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetSys1CatalogTabId(
    tgg00_Surrogate VAR_VALUE_REF tabId)
{
    tabId = Catalog_Interface::GetInstance().GetSys1CatalogTabId();
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560GetSys2CatalogTabId(
    tgg00_Surrogate VAR_VALUE_REF tabId)
{
    tabId = Catalog_Interface::GetInstance().GetSys2CatalogTabId();
}
/*-----------------------------------------------------------------------------------*/
externPascal pasbool kb560IsSys1CatalogTable(
    tgg00_Surrogate VAR_VALUE_REF tabId)
{
    return Catalog_Interface::GetInstance().IsSys1CatalogTable( tabId );
}
/*-----------------------------------------------------------------------------------*/
externPascal pasbool kb560IsSys2CatalogTable(
    tgg00_Surrogate VAR_VALUE_REF tabId)
{
    return Catalog_Interface::GetInstance().IsSys2CatalogTable( tabId );
}
/*-----------------------------------------------------------------------------------*/
externPascal pasbool kb560IsCatalogTable(
    tgg00_Surrogate VAR_VALUE_REF tabId)
{
    return Catalog_Interface::GetInstance().IsCatalogTable( tabId );
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560SetSys1CatalogTreeId(
    tgg00_FileId VAR_VALUE_REF treeId)
{
    Catalog_Interface::GetInstance().SetSys1CatalogTreeId( treeId );
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560SetSys2CatalogTreeId(
    tgg00_FileId VAR_VALUE_REF treeId)
{
    Catalog_Interface::GetInstance().SetSys2CatalogTreeId( treeId );
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb560DetermineLocalRedoBufferSizeForTransactions ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb560DetermineLocalRedoBufferSizeForTransactions", LogVolume_Trace, 5);
	Log_Transaction::DetermineLocalRedoBufferSize();
}
