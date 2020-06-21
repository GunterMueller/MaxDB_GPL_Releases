/*!
    @ingroup        Restart
    @file           Rst_RedoManager.cpp
    @author         UweH
    @author         MartinB
    @author         JuergenA
    @brief          parallel undo/redo of transactions in order to restart the database
*/
/*

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



 */
#include "Restart/Rst_Exceptions.hpp"
#include "Restart/Rst_RedoManager.hpp"
#include "Restart/Rst_RedoTrafficControl.hpp"
#include "Restart/Rst_IRedoTrafficControl.hpp"
#include "Restart/Rst_RedoReadTask.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "ServerTasks/SrvTasks_JobRedo.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Rst.h"
#include "Transaction/Trans_Context.hpp"
#include "ggg00.h"
#include "hgg01.h" // g01maxuser
// --------------------------------------------------------------------------
void Rst_RedoManager::CheckForClearLog(       tsp00_TaskId          taskid,
                                        const Rst_LogEntryIterator &iter,
                                              bool                 &lastPageCleared)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::CheckForClearLog", LogTrans_Trace, 5);
    Log_Volume         &log = Log_Volume::Instance();
    Log_IOSequenceNo    lastIOSequenceOnLog = iter.GetLastProcessedIOSeq();
    
    lastPageCleared = false;
    if (!log.LogIsEmpty())
    // also for an emtpy log the DB has to be restartable
    {
        if (Log_VolumeIterator::Iter_untilReached == iter.GetFinalState())
        {
            if (iter.GetLastReadOffset().IsValid())                    // PTS 1111525 mb 2002-01-24 restore log until
            // some or all pages from the log_device had been read
            {
                Log_IOSequenceNo         firstIOSeqOfLogVolume;
                Log_RawDeviceOffset      firstKnownOffset;
                
                log.GetFirstKnownBeforeRestoreLog(firstKnownOffset, firstIOSeqOfLogVolume);
                Log_RawDeviceOffset firstToBeCleared = iter.GetLastReadOffset();
                Log_RawDeviceIterator lastToBeCleared = 
                        --(Log_RawDeviceIterator( log.LogicalLogBeginOffset(),
                                                  firstKnownOffset,
                                                  log.LogicalLogEndOffset()));
                log.ClearLogPartially ( taskid,
                                        firstToBeCleared,
                                        lastToBeCleared.GetPosition(),
                                        lastIOSequenceOnLog,
                                        lastPageCleared);
            }
            else
            // no pages of the log_device had been read => clear them all
            {
                log.ClearLogComplete (taskid,
                                      lastIOSequenceOnLog+1);
                lastPageCleared = true;
            }
        }
    }
}
// --------------------------------------------------------------------------
Rst_RedoManager::RedoLogResult
Rst_RedoManager::RedoLog (      tsp00_TaskId          taskid,
                                tgg00_TransContext   &oldcontext,
                                Log_RawDeviceOffset   lastRedoOffset,
                          const Kernel_Date           untilDate,     // PTS 1111525 mb 2001-01-15
                          const Kernel_Time           untilTime,     // PTS 1111525 mb 2001-01-15
                          const Log_IOSequenceNo      untilIOSeq,    // PTS 1121539 mb 2003-06-02
                                Rst_RedoKind          redokind,
                                SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG  ("Rst_RedoManager::RedoLog", LogTrans_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (oldcontext.trError_gg00,    LogTrans_Trace, 5);
    
    m_redoIsAborted = false;
    m_untilDate = untilDate;
    m_untilTime = untilTime;

    if (untilIOSeq.IsValid())				// PTS 1124727 mb 2003-10-20
    {
        m_untilIOSequence = untilIOSeq;
        ++m_untilIOSequence;
    }
    
    errlist.ClearMessageList();
    
    // The allocator usage is OK here, if all resources of the redo traffic control
    // is allocated by the redo manager task and not by the redo execution task !!!!
    SAPDBMem_IRawAllocator         &allocator = Trans_Context::GetContext(oldcontext).Allocator();
    Rst_LogEntryIterator            logIter     (m_untilDate, m_untilTime, m_untilIOSequence, lastRedoOffset, redokind);
    Rst_RedoReadTask                redoReader  (logIter, *this, m_redoIsAborted);
    Rst_RedoTrafficControl          redoControl (allocator, m_redoIsAborted, m_transread, m_transredone);

    if ( ! redoControl.Initialize() )
        return RedoLogAllocateFailed;
        
    m_pRedoTrafficControl = &redoControl;
    m_pRedoReadIterator   = &logIter;
    m_pRedoReader         = &redoReader;
    
    RTETask_ITask                         &task = Trans_Context::GetContext(oldcontext).Task();
    SrvTasks_WaitingTask                   waitingTask (task);
    SrvTasks_JobList<SrvTasks_JobRedo>     redojobs    (allocator);
    SrvTasks_JobRedo                      *newRedoJob;
    RedoLogResult                          result                    = RedoLogOk;
    SrvTasks_Job::ExecuteByServerResult    executeResult             = SrvTasks_Job::executeOK;
    SAPDB_UInt                             startedRedoExecutionTasks;
    SAPDB_UInt                             availableRedoTasks        = SrvTasks_ICoordinator::Instance().GetAvailableServerTasks()
                                                                       - 1 /*redo reader*/;

    if ( availableRedoTasks < redoControl.PossibleNumberOfRedoTransactions() )
    {
        errlist = Msg_List(Msg_List::Info,SDBMSG_RST_MORE_USER_THEN_REDO_TASKS,
                           Msg_Arg(SDBMSGTAG_RST_MORE_USER_THEN_REDO_TASKS__TASKS_WANTED,    SAPDB_ToString(redoControl.PossibleNumberOfRedoTransactions())),
                           Msg_Arg(SDBMSGTAG_RST_MORE_USER_THEN_REDO_TASKS__TASKS_AVAILABLE, SAPDB_ToString(availableRedoTasks)));
        RTE_Message(errlist);
        errlist.ClearMessageList();
    }

    // redo tasks must be started before read task
    
    if ( result == RedoLogOk )
    {
        for ( startedRedoExecutionTasks = 0;
              startedRedoExecutionTasks < availableRedoTasks;
              ++startedRedoExecutionTasks )
        {
            newRedoJob = new (allocator) SrvTasks_JobRedo ( jobRedoLogExecutor,
                                                            SrvTasks_Job::Attributes(startedRedoExecutionTasks),
                                                            &waitingTask );
            if ( 0 == newRedoJob )
            {
                result = RedoLogAllocateFailed;
                break;
            }
    
            executeResult = newRedoJob->ExecuteByServer(task,errlist,&waitingTask);
    
            if ( executeResult != SrvTasks_Job::ok )
            {
                if ( executeResult = SrvTasks_Job::executeNoServerAvailable )
                    destroy(newRedoJob, allocator);
                else
                    result = RedoLogExecuteTaskError;
                break;
            }
            
            if ( ! redojobs.InsertEnd(newRedoJob) )
            {
                destroy(newRedoJob, allocator);
                result = RedoLogAllocateFailed;
                break;
            }
        }
        
        if ( startedRedoExecutionTasks > 0 )
            redoControl.SetNumberOfRedoTasks(taskid, startedRedoExecutionTasks);
    }
    
    if ( result == RedoLogOk )
    {
        newRedoJob = new (allocator) SrvTasks_JobRedo ( jobRedoLogReader,
                                                        SrvTasks_Job::Attributes(0),
                                                        &waitingTask );
        if ( 0 == newRedoJob )
            result = RedoLogAllocateFailed;
        else
        {
            executeResult = newRedoJob->ExecuteByServer(task,errlist,&waitingTask);

            if ( executeResult != SrvTasks_Job::ok )
                result = RedoLogExecuteTaskError;
            else
                if ( ! redojobs.InsertEnd(newRedoJob) )
                {
                    destroy(newRedoJob, allocator);
                    result = RedoLogAllocateFailed;
                }
        }
    }

    if ( oldcontext.trRteCommPtr_gg00->to_cancel
         ||
         m_abortReasonFromRestoreComponent == e_cancelled )
    {
        m_redoIsAborted = true;
    }
    
    if ( result != RedoLogOk || m_redoIsAborted )
    {
        redojobs.AbortAll(waitingTask);
        AbortByRestoreComponent(task.ID(), e_redo_log_execute_error);
    }
    else
    {
        bool                 isAborted = false;
        SrvTasks_Job::Result jobresult;
        redojobs.WaitForAll(waitingTask,isAborted,jobresult,errlist);
        if ( isAborted )
        {
            m_redoIsAborted = true;
            AbortByRestoreComponent(task.ID(), e_redo_log_execute_error);
        }
        else
            if ( jobresult != SrvTasks_Job::ok )
                result = RedoLogExecuteTaskError;
    }

    logIter.GetReadPosition(m_LastEntryRead); // PTS 1123675 UH 2003-08-20

    m_pRedoTrafficControl = 0;
    m_pRedoReader         = 0;
    m_pRedoReadIterator   = 0;

    if ( ! m_redoIsAborted && result == RedoLogOk )
    {
        Msg_List output;
        redoControl.AppendContextInfo(__CONTEXT__, output, "End of redo processing");
        RTE_Message(output);
    }

    if ( m_abortReasonFromRestoreComponent != e_ok
         &&
         m_abortReasonFromRestoreComponent != e_cancelled )
    {
        Kernel_AppendBasisError (m_abortReasonFromRestoreComponent, errlist, "Error of Restore Component");
    }

    if ( m_redoIsAborted && result == RedoLogOk )
    {
        if ( redokind == Rst_RedoKindRestoreLog )
        {
            // PTS 1127077 2004-01-12 the reason must not be "Restart"
            Trans_Context &context = Trans_Context::GetContext(oldcontext);
            if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait (context.Task(), Log_SVPReasonDistance, context.ErrorList()) )
                return RedoLogSavepointNotStarted;
        }
        return RedoLogCancelled;
    }

    bool lastReadPageCleared;
    CheckForClearLog(oldcontext.trTaskId_gg00, logIter, lastReadPageCleared);

    Log_Volume       &log            = Log_Volume::Instance();
    Log_IOSequenceNo  nextIOSeqOnLog = logIter.GetLastProcessedIOSeq();
    
    if ( redokind == Rst_RedoKindRestoreLog
         ||
         ! log.LogIsEmpty() )
    {
        ++nextIOSeqOnLog;
    }

    Log_RawDeviceIterator nextWritePosition;
    
    if ( logIter.GetLastReadOffset().IsValid()
         &&
         ! log.LogIsEmpty() )
    {
        // there had been something read from the log and
        // it had not been cleared by CheckForClearLog()
        if (!lastReadPageCleared)
        {
            nextWritePosition = ++(Log_RawDeviceIterator (log.LogicalLogBeginOffset(),
                                                          logIter.GetLastReadOffset(), 
                                                          log.LogicalLogEndOffset()));
        }
        else
        {
            nextWritePosition = Log_RawDeviceIterator (log.LogicalLogBeginOffset(),
                                                       logIter.GetLastReadOffset(), 
                                                       log.LogicalLogEndOffset());
        }
    }
    else
    {
        nextWritePosition = Log_RawDeviceIterator (log.LogicalLogBeginOffset(),
                                                   log.LogicalLogBeginOffset(),
                                                   log.LogicalLogEndOffset());
    }
    
    log.UpdateIOSequenceAndOffset (taskid, nextIOSeqOnLog, nextWritePosition);

    // PTS 1126112 UH 2003-12-09 moved to Log_Savepoint::Coordinator()
    // Log_Savepoint::Instance().EnableSavepointWriteOnLog ();
    oldcontext.trError_gg00 = e_ok;
    if ( result == RedoLogOk && redokind == Rst_RedoKindRestoreLog )
    {
        Log_EntryInfo entryInfo;
        GetLastEntryRead (entryInfo);
        if ( entryInfo.iosequence.IsValid() )
        {
            // PTS 1127077 2004-01-12 the reason must not be "Restart"
            Trans_Context &context = Trans_Context::GetContext(oldcontext);
            if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait(context.Task(), Log_SVPReasonDistance, context.ErrorList()) )
                return RedoLogSavepointNotStarted;
        }
        oldcontext.trError_gg00 = e_cancelled;
    }
    return result;
}
// --------------------------------------------------------------------------
void Rst_RedoManager::Abort(tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::Abort", LogTrans_Trace, 5);
    m_redoIsAborted = true;
    if ( m_pRedoTrafficControl != 0 )
        m_pRedoTrafficControl->Abort(taskid);
}
// --------------------------------------------------------------------------
void Rst_RedoManager::GetProgressInfo( SAPDB_UInt &transread,
                                       SAPDB_UInt &transredone )
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::GetProgressInfo", LogTrans_Trace, 5);
    transread   = m_transread;
    transredone = m_transredone;
}
// --------------------------------------------------------------------------
Log_RawDeviceOffset Rst_RedoManager::GetLastRedoReadOffset()
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::GetLastRedoReadOffset", LogTrans_Trace, 5);
    if ( 0 == m_pRedoReadIterator )
        return Log_RawDeviceOffset(); 
    return  m_pRedoReadIterator->GetCurrentReadOffset();
}
// --------------------------------------------------------------------------
void Rst_RedoManager::SetLastRedoReadOffset( Log_RawDeviceOffset lastRedoOffset, tsp00_TaskId taskId  )
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::SetLastRedoReadOffset", LogTrans_Trace, 5);
    if ( 0 != m_pRedoReadIterator )
    {
        Log_Volume     &log = Log_Volume::Instance();
        if (   lastRedoOffset >= log.LogicalLogBeginOffset()
            && lastRedoOffset <= log.LogicalLogEndOffset())
        {
            m_pRedoReadIterator->SetLastRedoReadOffset(lastRedoOffset, taskId);
        }
        else
        {
            RTE_Message(Restart_Exception(__CONTEXT__, RST_INVALID_REDO_OFFSET,
                  SAPDB_ToString(lastRedoOffset,_T_d)));
        }
    }
}
// --------------------------------------------------------------------------
void Rst_RedoManager::SuspendRedoReader(tsp00_TaskId          taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::SuspendRedoReader", LogTrans_Trace, 5);
    if ( m_pRedoReadIterator != 0 )
    {
        m_pRedoReadIterator->SuspendRedoReader(taskid);
    }
}
// --------------------------------------------------------------------------
void Rst_RedoManager::ResumeRedoReader(const Kernel_Date         untilDate,
                                       const Kernel_Time         untilTime)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::ResumeRedoReader", LogTrans_Trace, 5);
    if ( m_pRedoReadIterator != 0 )
    {
        m_untilDate = untilDate;
        m_untilTime = untilTime;
        m_pRedoReadIterator->ResumeReaderForRestart();
    }
}
// --------------------------------------------------------------------------
void Rst_RedoManager::GetLastEntryRead(Log_EntryInfo &entryInfo)
{
    // PTS 1123675 UH 2003-08-20 new
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::GetLastEntryRead", LogTrans_Trace, 5);
    if ( m_pRedoReadIterator != 0 )
        m_pRedoReadIterator->GetReadPosition(m_LastEntryRead); 
    entryInfo = m_LastEntryRead;
}
// --------------------------------------------------------------------------
void Rst_RedoManager::TryToResumeRedoReader(tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::TryToResumeRedoReader", LogTrans_Trace, 5);
    if ( m_pRedoTrafficControl == 0 )
        return;
    m_pRedoTrafficControl->TryToResumeRedoReader(taskid);
}
// --------------------------------------------------------------------------
bool Rst_RedoManager::ReadLogAndCreateRedoFiles(tsp00_TaskId          taskid,
                                                tgg00_TransContext   &oldcontext,
                                                SAPDBErr_MessageList &errlist)
{
    // PTS 1123675 UH 2003-08-20 new
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoManager::ReadLogAndCreateRedoFiles", LogTrans_Trace, 5);
    if ( 0 == m_pRedoReader )
        return false;
	const Rst_RedoReadTask::ReadLogResult result  =
        m_pRedoReader->ReadLogAndCreateRedoFiles(taskid, oldcontext, errlist);
    switch ( result )
    {
        case Rst_RedoReadTask::RedoReadOk:
            return true;
        case Rst_RedoReadTask::RedoReadAborted:
            oldcontext.trError_gg00 = e_cancelled;
            return true;
        default:
            return false;
    }
    // never been reached => compiler warning:  return true;
}
/*===========================================================================*
*  END OF CODE                                                               *
*============================================================================*/
