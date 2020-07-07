/*!
    @ingroup        Restart
    @file           Rst_RedoTrafficControl.cpp      
    @author         JuergenA
    @author         UweH
    @brief          defines the classes to synchronize the parallel redo
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
#include "Restart/Rst_IRedoTrafficControl.hpp"
#include "Restart/Rst_RedoTrafficControl.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "KernelCommon/Kernel_IAdminBackupRecovery.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Restart/Rst_Exceptions.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "ServerTasks/SrvTasks_JobDropAuxFiles.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Rst.h"
#include "gsp00.h"
#include "ggg00.h"
#include "ggg92.h"   // used by hkb51.h
#include "heo55k.h"  // vbegexcl,vendexcl,visexcl
#include "heo56.h"   // vsuspend
#include "hgg01.h"   //g01maxuser()
#include "hgg08.h"   // g08redocontrol
#include "hkb50.h"   // k50RegisterTransaction
#include "hkb51.h"
#include "hkb53.h"   // k53server_drop_aux_files
/// suspend reason used when no more free transentries in the lockist exist
#define RST_WAIT_FOR_FREE_TRANS_ENTRIES 217
/// suspend reason used when too much transactions were read by the reader and inserted to the list
#define RST_WAIT_FOR_FINISHED_JOBS 227
/// suspend reason used when nothing to do
#define LOG_WAIT_FOR_VACANT_ACTION   231
/// suspend reason used waiting for another trans, which has to be commited before
#define LOG_WAIT_FOR_PRECEDING_TRANS 232
/// WAIT_REASON_EMPTY_LIST
#define WAIT_REASON_EMPTY_LIST       228
/*!
    @class Rst_RedoSync
    @brief class to get exclusive access to the redo lists
 */
class Rst_RedoSync
{
private:
    /// taskid needed for the leaving of the associated region
    tsp00_TaskId m_CallingTaskId;
    
public:
    /// enters g08redocontrol region
    Rst_RedoSync (tsp00_TaskId CallingTaskId)
    {
        m_CallingTaskId = CallingTaskId;
        vbegexcl (m_CallingTaskId, g08redocontrol);
    }
    /// leaves g08redocontrol region
    ~Rst_RedoSync ()
    {
        vendexcl (m_CallingTaskId, g08redocontrol);
    }
    /// returns bool if caller is owner of critical section
    static bool IsInRegion(tsp00_TaskId taskid)
    {
        return visexcl(taskid, g08redocontrol);
    }
    /// critical section can explicitely entered
    static void EnterRegion(tsp00_TaskId taskid)
    {
        vbegexcl (taskid, g08redocontrol);
    }
    /// critical section can explicitely left
    static void LeaveRegion(tsp00_TaskId taskid)
    {
        vendexcl (taskid, g08redocontrol);
    }
};

//----------------------------------------------------------------------------

bool Rst_RedoTrafficControl::ExecuteJobs (tsp00_TaskId          taskid,
                                          tgg00_TransContext   &oldcontext,
                                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::ExecuteJobs", LogTrans_Trace, 5);
    
    Rst_JobEntry      *pJob;
    bool               WaitInserted;
    bool               EndOfLog = false;
    tgg00_WaitContext  waitContext;
    
    errlist.ClearMessageList();  // PTS 1135265 mb 2005-04-28 
    while ( ! EndOfLog
            &&
            ! m_redoIsAborted )
    {    
        // PTS 1135265 mb 2005-04-28 always print and clear the message list in order to avoid
        // growing messagelists
        if ( ! errlist.IsEmpty() )
        {
            RTE_Message(errlist);
            errlist.ClearMessageList();
        }
            
        WaitInserted = false;
        pJob         = NULL;
        
        {
            Rst_RedoSync RedoSync (taskid);
            
            if ( NULL == m_pFirstVacancy
                 ||
                 m_RedoProcessingDisabled ) // PTS 1124684 UH 2003-10-16
            {
                EndOfLog = m_EndOfLog;
                if ( ! EndOfLog )
                {
                    m_VacantJobWait.InsertAsFirst(taskid, waitContext);
                    WaitInserted = true;
                }
            }
            else
            {
                pJob = m_pFirstVacancy;
                
                m_pFirstVacancy = pJob->GetNext();
                
                if (NULL == m_pFirstVacancy)
                {
                    Rst_IoSeqEntry *pCurrIoSeq = pJob->GetIoSeqEntry()->GetNext();
                    
                    if (NULL != pCurrIoSeq)
                    {
                        // every IoSeqEntry has at least one job entry (scan of IoSeqList not necessary) 
                        m_pFirstVacancy = pCurrIoSeq->GetFirstJobEntry ();
                    }            
                }
            }
        }
        
        // outside RedoSync region
        
        if ( NULL == pJob )
        {
            if (WaitInserted)
                vsuspend (taskid, LOG_WAIT_FOR_VACANT_ACTION);
        }
        else
        {
            if ( m_redoIsAborted )
                break;
                
            oldcontext.trIndex_gg00        = pJob->GetTransIndex();
            oldcontext.trWriteTransId_gg00 = pJob->GetTransaction().GetRedoTransNo();
            
            int           checkindex  = oldcontext.trIndex_gg00;
            tgg91_TransNo redoTransNo = oldcontext.trWriteTransId_gg00;

			pJob->GetTransaction().InitializeForRedo (oldcontext);

			if ( LogTrans_Trace.TracesLevel(6) )
				Kernel_VTrace() << "REDOTRAFFIC: start T" << redoTransNo.gg90GetInt4()
								<< ", EOTSEQ: " << pJob->GetIoSeqEntry()->GetIoSeq()
								<< ", KIND: " << (pJob->GetUndoOrRedo()==Log_Commit?"redo":"undo")
                                << NewLine;
                                
            switch ( pJob->GetUndoOrRedo() )
            {
            case Log_Commit:

                if ( pJob->GetTransaction().EntriesToRedoExist() )
                {
                    Log_Transaction::RedoResult RedoResult;

                    oldcontext.trError_gg00 = e_ok;

                    RedoResult = pJob->GetTransaction().Redo(errlist);
                    
                    if ( oldcontext.trError_gg00 != e_ok
                         ||
                         Log_Transaction::redoOk != RedoResult )
                    {
                        if ( oldcontext.trError_gg00      != e_cancelled 
                             &&
                             Log_Transaction::redoAborted != RedoResult) // PTS ? 2005-01-03
                        {
                            Kernel_AppendBasisError(oldcontext.trError_gg00,errlist,"RedoError");
                            errlist.Overrule( Msg_List(Msg_List::Error,SDBMSG_RST_REDO_EXECUTE_FAILED,
                                                     Msg_Arg("TRANS_ID",   SAPDB_ToString(oldcontext.trWriteTransId_gg00.gg90GetInt4())),
                                                     Msg_Arg("REASON",     "redo"),
                                                     Msg_Arg("RETURNCODE", SAPDB_ToString(RedoResult)),
                                                     Msg_Arg("ERRORTEXT",  Log_Transaction::RedoResultString[RedoResult]) ));
                            return false;
                        }
                        m_redoIsAborted = true;
                        oldcontext.trError_gg00 = e_ok;
                    }
                    // synchronize with other commits
                    bool waitInserted = false;
                    if ( CheckIoSeqError == CheckIoSeq (taskid, waitContext,
                                                        pJob->GetIoSeqEntry()->GetIoSeq(),
                                                        waitInserted,
                                                        errlist) )
                    {
                        return false;
                    }
                    if ( waitInserted )
                        WaitForIoSeq (taskid);
                }
                break;

            case Log_Rollback:

                if ( pJob->GetTransaction().UndoEntriesExist() )
                {
                    Log_Transaction::RollbackResult RollbackResult;

                    bool waitInserted = false;
                    if ( CheckIoSeqError == CheckIoSeq (taskid, waitContext,
                                                        pJob->GetIoSeqEntry()->GetIoSeq(),
                                                        waitInserted,
                                                        errlist) )
                    {
                        return false;
                    }
                    if ( waitInserted )
                        WaitForIoSeq (taskid);

                    if ( ! m_redoIsAborted )
                    {
                        oldcontext.trError_gg00 = e_ok;
                        
                        RollbackResult = pJob->GetTransaction().Rollback ( Log_Transaction::endOfTrans,
                                                                           LOG_NIL_SUBTRANS_NO,
                                                                           Log_EntrySequence(),
                                                                           errlist );
                    
                        if ( oldcontext.trError_gg00 != e_ok
                             ||
                             Log_Transaction::undoOk != RollbackResult )
                        {
                            if ( oldcontext.trError_gg00 != e_cancelled )
                            {
                                Kernel_AppendBasisError(oldcontext.trError_gg00,errlist,"UndoError");
                                errlist.Overrule ( Msg_List(Msg_List::Error,SDBMSG_RST_REDO_EXECUTE_FAILED,
                                                         Msg_Arg("TRANS_ID",   SAPDB_ToString(oldcontext.trWriteTransId_gg00.gg90GetInt4())),
                                                         Msg_Arg("REASON",     "undo"),
                                                         Msg_Arg("RETURNCODE", SAPDB_ToString(RollbackResult)),
                                                         Msg_Arg("ERRORTEXT",  Log_Transaction::RollbackResultString[RollbackResult]) ));
                                return false;
                            }
                            m_redoIsAborted = true;
                            oldcontext.trError_gg00 = e_ok;
                        }
                    }
                }
                break;
                
            default:
                RTE_Crash( Restart_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                           "Rst_RedoTrafficControl::ExecuteJobs() invalid redo type") );
            }
            
            if ( ! m_redoIsAborted )
            {
                if ( checkindex != oldcontext.trIndex_gg00 )
                    RTE_Crash( Restart_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                               "transindex mismatch") );
    
                // The following sequence of CleanUp is important  !!!
                
                RemoveJob (taskid, pJob);
                
                if(!pJob->GetTransaction().Delete(errlist))
                    return false;
    
                k51remove_locks                 ( taskid,
                                                  oldcontext.trIndex_gg00,
                                                  oldcontext.trState_gg00 );
                k51del_transentry_from_locklist ( taskid,
                                                  oldcontext.trIndex_gg00,
                                                  oldcontext );
    
                // really drop all dropped files
                oldcontext.trError_gg00 = e_ok;
                
                SrvTasks_Job::Result result = SrvTasks_JobDropAuxFiles::ExecuteByRedoTask(oldcontext,errlist);
                
                if ( result != SrvTasks_Job::ok )
                {
                    if ( oldcontext.trError_gg00 != e_cancelled )
                    {
                        errlist.AddDetail(SDBMSG_RST_DROP_AUX_FILE_FAILED,
                                          Msg_Arg("TRANS_ID",SAPDB_ToString(oldcontext.trWriteTransId_gg00.gg90GetInt4())));
                        Kernel_AppendBasisError (oldcontext.trError_gg00, errlist, "JobDropAuxFile");
                        return false;
                    }
                    m_redoIsAborted = true;
                    oldcontext.trError_gg00 = e_ok;
                }
    
                oldcontext.trWriteTransId_gg00.gg90SetNil();
                oldcontext.trLogContext_gg00 = NULL;
                oldcontext.trIndex_gg00      = cgg_nil_transindex;
            }
        }
    }
    if ( m_redoIsAborted )
        Kernel_IAdminBackupRecovery::Instance().
        AbortRedoLogByRestoreComponent(taskid, oldcontext.trError_gg00);
    return true;
}
//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::SetEndOfLogRead (tsp00_TaskId LogReaderTaskId)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::SetEndOfLogRead", LogTrans_Trace, 5);
    Rst_RedoSync RedoSync (LogReaderTaskId);
    m_EndOfLog = true;
    m_VacantJobWait.ResumeAll();
}
//----------------------------------------------------------------------------
Log_IOSequenceNo Rst_RedoTrafficControl::GetLastIOSequence (tsp00_TaskId taskid) const
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::GetLastIOSequence", LogTrans_Trace, 5);
    Rst_RedoSync RedoSync (taskid); // ENTER REGION
    return m_IoSeqList.GetLastIoSeq();
}

//----------------------------------------------------------------------------
bool Rst_RedoTrafficControl::IsAborted () const
{
    return m_redoIsAborted;
}

//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::RedoReaderWaitState::TryToResumeTask( tsp00_TaskId taskid,
                                                                   WaitReason   reason)
{
    if ( GetWaitingTask(reason) != cgg_nil_pid )
    {
        if ( m_TaskId == taskid )
            return;
        
        vresume(m_TaskId);
        m_TaskId     = cgg_nil_pid;
        m_WaitReason = isNotWaiting;
    }
}

//----------------------------------------------------------------------------
bool Rst_RedoTrafficControl::Abort (tsp00_TaskId taskid)
{
    Rst_RedoSync RedoSync (taskid);
	return AbortIntern (taskid);
}

//----------------------------------------------------------------------------
bool Rst_RedoTrafficControl::AbortIntern (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTrafficControl::AbortIntern", LogTrans_Trace, 5);

    if ( ! Rst_RedoSync::IsInRegion(taskid) )
        RTE_Crash( Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "AbortIntern() called with region") );

    m_redoIsAborted = true;
    
    if ( LogTrans_Trace.TracesLevel(7) )
        WriteToTrace("REDO MANAGER ABORTING");

    m_VacantJobWait.ResumeAll();

    TryToResumeRedoReader(taskid, true);
    
    Rst_JobEntry   *jobToRemove = 0;
    Rst_IoSeqEntry *ioseqlist    = m_IoSeqList.GetFirst();
    if ( 0 == ioseqlist )
        return true;

    while ( ioseqlist != 0 )
    {
        m_pFirstVacancy = ioseqlist->GetFirstJobEntry();
        while ( m_pFirstVacancy != 0 )
        {
            jobToRemove     = m_pFirstVacancy;
            m_pFirstVacancy = m_pFirstVacancy->GetNext();
            RemoveJobIntern (taskid, jobToRemove);
        }
        ioseqlist = ioseqlist->GetNext();
    }

    if ( ! m_IoSeqList.IsEmpty() )
        return false;
    
    return true;
}

//----------------------------------------------------------------------------

bool Rst_RedoTrafficControl::AbortIfAllRedoTasksAreWaiting (tsp00_TaskId          taskid,
                                                            SAPDBErr_MessageList &errlist,
                                                            bool                  alreadyInRegion)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::AbortIfAllRedoTasksAreWaiting", LogTrans_Trace, 5);

    if ( ! alreadyInRegion )
        Rst_RedoSync::EnterRegion(taskid);

    if ( m_WaitingRedoTasks >= m_NumberOfStartedRedoTasks
         &&
         m_RedoReaderWaitState.IsWaiting() )
    {
        AbortIntern(taskid);
        errlist = Msg_List(Msg_List::Error, SDBMSG_RST_REDO_TASK_DEADLOCK);
        AppendContextInfo(__CONTEXT__, errlist, "AbortIfAllRedoTasksAreWaiting");
        if ( ! alreadyInRegion )
            Rst_RedoSync::LeaveRegion(taskid);
        return false;
    }
    
    if ( ! alreadyInRegion )
        Rst_RedoSync::LeaveRegion(taskid);
	return true;
}

//----------------------------------------------------------------------------

bool Rst_RedoTrafficControl::CheckJobCount (tsp00_TaskId          taskid,
                                            SAPDBErr_MessageList &errlist)
{
    // UH 2005-01-03 new
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::CheckJobCount", LogTrans_Trace, 5);

    bool waitForFinishedJobs = false;
    
    Rst_RedoSync::EnterRegion(taskid);
        
    if ( m_removed > m_inserted )
    {
        Rst_RedoSync::LeaveRegion(taskid);
        errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "More Redojobs finished then started");
        return false;
    }

    if ( m_inserted - m_removed > m_NumberOfStartedRedoTasks )
    {
        waitForFinishedJobs = true;
        if ( ! m_RedoReaderWaitState.TaskWillWait(taskid, RedoReaderWaitState::waitUntilNextJobIsFinished) )
        {
            Rst_RedoSync::LeaveRegion(taskid);
            errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                        "m_RedoReaderWaitState.TaskWillWait(taskid, waitUntilNextJobIsFinished)");
            return false;
        }

        if ( ! AbortIfAllRedoTasksAreWaiting (taskid, errlist, true) )
        {
            Rst_RedoSync::LeaveRegion(taskid);
            return false;
        }
    }

    Rst_RedoSync::LeaveRegion(taskid);

    if ( waitForFinishedJobs )
        vsuspend(taskid, RST_WAIT_FOR_FINISHED_JOBS);

    return true;
}

//----------------------------------------------------------------------------

bool Rst_RedoTrafficControl::WaitForFinishedRedoTransaction (tsp00_TaskId          taskid,
                                                             SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::WaitForFinishedRedoTransaction", LogTrans_Trace, 5);

    if ( ! m_RedoReaderWaitState.TaskWillWait(taskid, RedoReaderWaitState::waitUntilFreeTransEntriesExist) )
        return false;

    if ( ! AbortIfAllRedoTasksAreWaiting (taskid, errlist) )
        return false;

    vsuspend(taskid, RST_WAIT_FOR_FREE_TRANS_ENTRIES);

    return true;
}

//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::TryToResumeRedoReader(tsp00_TaskId taskid,
                                                   bool         alreadyInRegion)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::TryToResumeRedoReader", LogTrans_Trace, 5);

    if ( ! alreadyInRegion )
        Rst_RedoSync::EnterRegion(taskid);

    m_RedoReaderWaitState.TryToResumeTask(taskid, RedoReaderWaitState::waitUntilFreeTransEntriesExist);

    if ( ! alreadyInRegion )
        Rst_RedoSync::LeaveRegion(taskid);
}

//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::SetNumberOfRedoTasks(tsp00_TaskId taskid,
                                                  SAPDB_UInt   startedRedoExecutionTasks)
{
	Rst_RedoSync RedoSync (taskid); // ENTER REGION
    m_NumberOfStartedRedoTasks = startedRedoExecutionTasks;
}

//----------------------------------------------------------------------------

bool
Rst_RedoTrafficControl::InsertJob (tsp00_TaskId          LogReaderTaskId,
                                   tgg00_TransIndex      TransIndex,
                                   Log_IOSequenceNo      EndOfTransIoSeq,
                                   Log_ActionType        UndoOrRedo,
                                   Log_IOSequenceNo      oldestKnownEOTSequence, // PTS 1124684 UH 2003-10-16
                                   bool                  checkAscendingSequence, // PTS 1113641 UH 2002-01-25
                                   Log_Transaction      &Transaction,
                                   SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::InsertJob", LogTrans_Trace, 5);

    if ( EndOfTransIoSeq.IsInvalid() )
    {
    	Kernel_VTrace() << "REDOTRAFFIC: insert T" << Transaction.GetRedoTransNo().gg90GetInt4()
        	            << ", EOTSEQ: " << EndOfTransIoSeq
            	        << ", KIND: " << (UndoOrRedo==Log_Commit?"redo":"undo")
                        << NewLine;
        RTE_Crash( Restart_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "EndOfTransIoSeq must not be NIL") );
    }

    if ( ! CheckNewJobEntry ( LogReaderTaskId,
                              TransIndex,
                              EndOfTransIoSeq,
                              Transaction.GetRedoTransNo(),
                              errlist ) )
	{
        return false;
	}
    
    Rst_RedoSync RedoSync (LogReaderTaskId); // ENTER REGION
    
	if ( LogTrans_Trace.TracesLevel(6) )
    	Kernel_VTrace() << "REDOTRAFFIC: insert T" << Transaction.GetRedoTransNo().gg90GetInt4()
        	            << ", EOTSEQ: " << EndOfTransIoSeq
            	        << ", KIND: " << (UndoOrRedo==Log_Commit?"redo":"undo")
                        << NewLine;

    Rst_JobEntry *pJobEntry = GetJobEntry (TransIndex);
    
    if (NULL == pJobEntry)
    {
        errlist = Restart_Exception(__CONTEXT__, RST_NO_FREE_REDO_JOB,
                                    SAPDB_ToString(TransIndex),
                                    SAPDB_ToString(EndOfTransIoSeq),
                                    SAPDB_ToString(Transaction.GetRedoTransNo().gg90GetInt4()) );
        WriteToTrace("InsertJob: no free entries");
        return false;
    }

    if ( m_OldestKnownEOTSequence.IsInvalid() )
        m_OldestKnownEOTSequence = oldestKnownEOTSequence; // PTS 1124684 UH 2003-10-16
        
    Rst_IoSeqEntry *pEotIoSeqEntry = m_IoSeqList.SearchEntry(EndOfTransIoSeq);  // PTS 1124684 UH 2003-10-16
    
    if ( 0 == pEotIoSeqEntry )
    {
        // EndOfTransIoSeq is a new io sequence which must be added to the list
        // at the correct position
        if ( checkAscendingSequence // PTS 1113641 UH 2002-01-25
             &&
             m_IoSeqList.GetLastIoSeq().IsValid() // if list is empty this is nil
             // PTS 1120201 UH 2003-02-17
             &&
             Log_IOSequenceNo::LeftIsYounger(m_IoSeqList.GetLastIoSeq(), EndOfTransIoSeq,
                                             m_OldestKnownEOTSequence) // PTS 1124684 UH 2003-10-16
           )
        {
            // PTS 1113641 UH 2002-01-25
            errlist = Restart_Exception(__CONTEXT__, RST_INSERT_IOSEQUENCE_FAILED,
                                        SAPDB_ToString(EndOfTransIoSeq),
                                        SAPDB_ToString(m_IoSeqList.GetLastIoSeq()),
                                        SAPDB_ToString(m_OldestKnownEOTSequence) );
            WriteToTrace("InsertJob: new io sequence must be older");
            return false;
        }
        
        pEotIoSeqEntry = GetAndRemoveFromFreeIoSeqList ();
        
        if ( 0 == pEotIoSeqEntry )
        {
            errlist = Restart_Exception(__CONTEXT__, RST_INSERT_IOSEQUENCE_FAILED,
                                        SAPDB_ToString(EndOfTransIoSeq) );
            WriteToTrace("InsertJob: no free ioseqlist entry");
            return false;
        }
        
        pEotIoSeqEntry->Init(EndOfTransIoSeq);
        
        // PTS 1124684 UH 2003-10-16 use Insert() instead of Append()
        m_IoSeqList.Insert (pEotIoSeqEntry, m_OldestKnownEOTSequence);
    }

    pJobEntry->Init(UndoOrRedo, pEotIoSeqEntry, Transaction);
    
    pEotIoSeqEntry->AppendJobEntry (pJobEntry); // PTS 1124684 UH 2003-10-16
    
    if ( 0 == m_pFirstVacancy)
        m_pFirstVacancy = pJobEntry;

    if ( checkAscendingSequence ) // PTS 1124684 UH 2003-10-16
        m_VacantJobWait.ResumeAll();

    // The global locklist must use now the Log_Transaction::m_Context of pJobEntry
    // This must be synchronized with the savepoint coordinator.

    kb50EnterTransRegion   (LogReaderTaskId, TransIndex);
    k50RegisterTransaction (TransIndex, &pJobEntry->GetTransaction());
    kb50LeaveTransRegion   (LogReaderTaskId, TransIndex);
    
    ++m_inserted;

    if ( LogTrans_Trace.TracesLevel(7) )
        WriteToTrace("InsertJob: OK");

    return true;
}
//----------------------------------------------------------------------------
bool Rst_RedoTrafficControl::CheckNewJobEntry (tsp00_TaskId          taskid,
                                            tgg00_TransIndex      index,
                                            Log_IOSequenceNo      iosequence,
                                            const tgg91_TransNo  &transno,
                                            SAPDBErr_MessageList &errlist)
{
    Rst_RedoSync    RedoSync (taskid); // ENTER REGION
    Rst_IoSeqEntry *pEotIoSeqEntry = NULL;
    Rst_JobEntry   *pJobEntry      = NULL;
    
    pEotIoSeqEntry = m_IoSeqList.GetFirst();

	while ( pEotIoSeqEntry != NULL )
	{
		pJobEntry = pEotIoSeqEntry->GetFirstJobEntry();
		while ( pJobEntry != NULL )
		{
            if ( index == pJobEntry->GetTransIndex() )
            {
                errlist = Restart_Exception(__CONTEXT__, RST_DUPLICATE_REDO_JOB,
                                            SAPDB_ToString(index),
                                            SAPDB_ToString(iosequence),
                                            SAPDB_ToString(transno.gg90GetInt4()) );
                WriteToTrace("Rst_RedoTrafficControl::CheckNewJobEntry() duplicate index");
                return false;
            }
            if ( transno == pJobEntry->GetTransaction().GetRedoTransNo() )
            {
                errlist = Restart_Exception(__CONTEXT__, RST_DUPLICATE_REDO_TRANSNO,
                                            SAPDB_ToString(index),
                                            SAPDB_ToString(iosequence),
                                            SAPDB_ToString(transno.gg90GetInt4()) );
                WriteToTrace("Rst_RedoTrafficControl::CheckNewJobEntry() duplicate transno");
                return false;
            }
			pJobEntry = pJobEntry->GetNext();
		}
		pEotIoSeqEntry = pEotIoSeqEntry->GetNext();
	}
    return true;
}
//----------------------------------------------------------------------------
const char * Rst_RedoTrafficControl::RedoReaderWaitState::WaitReasonStrings [] =
{
    "isNotWaiting",
    "waitUntilRedoListIsEmpty",
    "waitUntilNextJobIsFinished",
    "waitUntilFreeTransEntriesExist",
    "anyReason"
};

//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::AppendContextInfo (SAPDB_Char  const * const  filename,
                                                SAPDB_UInt4 const          linenumber,
                                                SAPDBErr_MessageList      &errlist,
                                                SAPDB_Char  const * const  title) const
{
    if ( title != 0 )
        errlist.AddDetail (SDBMSG_RST_REDO_CONTROL_INFO_TITLE, Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO_TITLE__TITLE,title));
    
    errlist.AddDetail( SDBMSG_RST_REDO_CONTROL_INFO1,
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO1__LOG_IO_SEQUENCE,    SAPDB_ToString(m_OldestKnownEOTSequence)),
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO1__TASKS_AVAILABLE,    SAPDB_ToString(m_NumberOfStartedRedoTasks)),
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO1__MAX_NUM_REDO_TRANS, SAPDB_ToString(m_NumberRedoTransactions)) );
    errlist.AddDetail( SDBMSG_RST_REDO_CONTROL_INFO2,
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO2__END_OF_LOG,    (m_EndOfLog==0              ?"false":"true")),
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO2__REDO_ABORTED,  (m_redoIsAborted==0         ?"false":"true")),
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO2__REDO_DISABLED, (m_RedoProcessingDisabled==0?"false":"true")));
    errlist.AddDetail( SDBMSG_RST_REDO_CONTROL_INFO3,
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO3__TASK_ID, SAPDB_ToString(m_RedoReaderWaitState.CurrentTask())),
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO3__REASON,  m_RedoReaderWaitState.WaitReasonStrings[m_RedoReaderWaitState.CurrentReason()]),
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO3__NUMBER,  SAPDB_ToString(m_WaitingRedoTasks)) );
    errlist.AddDetail( SDBMSG_RST_REDO_CONTROL_INFO4,
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO4__NUMBER,  SAPDB_ToString(m_removed)),
                       Msg_Arg(SDBMSGTAG_RST_REDO_CONTROL_INFO4__NUMBER1, SAPDB_ToString(m_inserted)) );
}
//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::WriteToTrace (const char *title) const
{
    Rst_IoSeqEntry *pEotIoSeqEntry = NULL;
    Rst_JobEntry   *pJobEntry      = NULL;
    
	if ( title != NULL )
		Kernel_VTrace() << title;

    pEotIoSeqEntry = m_IoSeqList.GetFirst();


	Kernel_VTrace() << "REDOLIST:";
	Kernel_VTrace() << "oldestknown                     : " << m_OldestKnownEOTSequence;
	Kernel_VTrace() << "Maximum allowed RedoTrans       : " << m_NumberRedoTransactions;
	Kernel_VTrace() << "Started Redo tasks              : " << m_NumberOfStartedRedoTasks;
	Kernel_VTrace() << "Currently waiting RedoTrans     : " << m_WaitingRedoTasks;
	Kernel_VTrace() << "Transactions inserted "  << m_inserted << " and removed "	<< m_removed;
    Kernel_VTrace() << "RedoTaskWaiting: T" << m_RedoReaderWaitState.CurrentTask()
                    << ", reason: "         << m_RedoReaderWaitState.WaitReasonStrings[m_RedoReaderWaitState.CurrentReason()];
    Kernel_VTrace() << "END OF LOG: " << m_EndOfLog
                    << ", ABORTED: "  << m_redoIsAborted
                    << ", DISABLED: " << m_RedoProcessingDisabled
                    << FlushLine;

	while ( pEotIoSeqEntry != NULL )
	{
        Kernel_VTrace() << "=== IOSEQ ===: " << pEotIoSeqEntry->GetIoSeq();

		pJobEntry = pEotIoSeqEntry->GetFirstJobEntry();
		while ( pJobEntry != NULL )
		{
			Kernel_VTrace() << "T" << pJobEntry->GetTransaction().GetRedoTransNo().gg90GetInt4()
				            << ", index: " << pJobEntry->GetTransIndex()
							<< (pJobEntry->GetUndoOrRedo()==Log_Commit?", redo":", undo")
                            << (m_pFirstVacancy==pJobEntry?" => FirstVacant":"");
			pJobEntry = pJobEntry->GetNext();
		}
		pEotIoSeqEntry = pEotIoSeqEntry->GetNext();
	}
}
//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::RemoveJob (tsp00_TaskId  RedoTaskId,
                                        Rst_JobEntry *pJobEntry)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::RemoveJob", LogTrans_Trace, 5);
    Rst_RedoSync RedoSync (RedoTaskId);
    RemoveJobIntern (RedoTaskId,pJobEntry);
}
//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::RemoveJobIntern (tsp00_TaskId  RedoTaskId,
                                              Rst_JobEntry *pJobEntry)
{
	if ( LogTrans_Trace.TracesLevel(6) )
    	Kernel_VTrace() << "REDOTRAFFIC: remove T" << pJobEntry->GetTransaction().GetRedoTransNo().gg90GetInt4()
        	            << ", EOTSEQ: " << pJobEntry->GetIoSeqEntry()->GetIoSeq()
            	        << ", KIND: " << (pJobEntry->GetUndoOrRedo()==Log_Commit?"redo":"undo");
    
    Rst_IoSeqEntry *pEotSeqEntry = pJobEntry->GetIoSeqEntry();
    
 	if ( ! m_redoIsAborted )
 	{
        tgg00_TransIndex transindex = pJobEntry->GetTransIndex();
        kb50EnterTransRegion   (RedoTaskId, transindex);
        k50RegisterTransaction (transindex, NULL);
        kb50LeaveTransRegion   (RedoTaskId, transindex);
 	}

    pEotSeqEntry->RemoveJobEntry (pJobEntry);
 
    if ( pEotSeqEntry->IsEmptyJobList() )
    {
        const SAPDB_UInt resumedRedoTasks = pEotSeqEntry->TryResumeTasks();

        if ( m_WaitingRedoTasks >= resumedRedoTasks  )
            m_WaitingRedoTasks -= resumedRedoTasks;
        else
            m_WaitingRedoTasks = 0; // ++++++ ERROR

        m_IoSeqList.Remove   (pEotSeqEntry);
        PutIntoFreeIoSeqList (pEotSeqEntry); // added by UH 2001-07-03
    }
    
    if ( m_IoSeqList.IsEmpty() )
        m_RedoReaderWaitState.TryToResumeTask(RedoTaskId, RedoReaderWaitState::waitUntilRedoListIsEmpty);
    
    m_RedoReaderWaitState.TryToResumeTask(RedoTaskId, RedoReaderWaitState::waitUntilNextJobIsFinished);
    
    ++m_removed;
    // WriteToTrace("REDOTRAFFIC: after remove");
}
//----------------------------------------------------------------------------
SAPDB_UInt Rst_RedoTrafficControl::GetMaximumNumberOfTransactions() const
{
    // PTS 1134103 UH 2005-03-07 new
    return m_NumberRedoTransactions;
}
//----------------------------------------------------------------------------
bool
Rst_RedoTrafficControl::GetTransactions (tsp00_TaskId            taskid,
                                         Rst_Transactions       &listOfTransToSkip,
                                         Msg_List               &errlist) const
{
    // PTS 1134103 UH 2005-03-07 new
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::GetTransactions", LogTrans_Trace, 5);

    Rst_RedoSync RedoSync (taskid); // ENTER REGION

    if ( ! m_RedoProcessingDisabled )
	{
        errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "m_RedoProcessingDisabled must be true");
        WriteToTrace ("Rst_RedoTrafficControl::GetTransactions: m_RedoProcessingDisabled must be true");
        return false;
	}

	for ( Rst_IoSeqEntry *pEotIoSeqEntry = m_IoSeqList.GetFirst();
          pEotIoSeqEntry != 0;
          pEotIoSeqEntry = pEotIoSeqEntry->GetNext() )
	{
		for ( Rst_JobEntry *pJobEntry = pEotIoSeqEntry->GetFirstJobEntry();
              pJobEntry != 0;
              pJobEntry = pJobEntry->GetNext() )
		{
            if ( ! listOfTransToSkip.Add (pJobEntry->GetTransaction().GetRedoTransNo()) )
            {
                errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "Adding transno to list");
                WriteToTrace ("Rst_RedoTrafficControl::GetTransactions: Adding transno to list failed");
                listOfTransToSkip.Delete();
                return false;
            }
		}
	}
    return true;
}
//----------------------------------------------------------------------------
Rst_RedoTrafficControl::Rst_RedoTrafficControl (SAPDBMem_IRawAllocator &RawAllocator,
                                                volatile bool          &redoIsAborted,
                                                SAPDB_UInt             &inserted,
                                                SAPDB_UInt             &removed)
: m_NumberRedoTransactions    (0)
, m_NumberOfStartedRedoTasks  (0)
, m_EndOfLog                  (false)
, m_pFirstVacancy             (0)
, m_WaitingRedoTasks          (0)
, m_pIoSeqContainer           (0)
, m_pJobContainer             (0)
, m_RawAllocator              (RawAllocator)
, m_redoIsAborted             (redoIsAborted)
, m_inserted                  (inserted)
, m_removed                   (removed)
, m_RedoProcessingDisabled    (true) // PTS 1124684 UH 2003-10-16
{}
//----------------------------------------------------------------------------
Rst_IoSeqEntry* Rst_RedoTrafficControl::GetAndRemoveFromFreeIoSeqList ()
{
    return m_IoSeqFreeList.GetAndRemoveAny();
}
//----------------------------------------------------------------------------
Rst_JobEntry* Rst_RedoTrafficControl::GetJobEntry (tgg00_TransIndex TransIndex)
{
    if (cgg_nil_transindex == TransIndex)
        return (NULL);
    
    Rst_JobEntry* entry = (m_pJobContainer+TransIndex-1);
    
    if ( TransIndex != entry->GetTransIndex() )
    {
        RTE_Crash( Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                   "Rst_RedoTrafficControl::GetJobEntry: transindex mismatch") );
    }
    return entry;
}
//----------------------------------------------------------------------------
bool Rst_RedoTrafficControl::Initialize ()
{  
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::Initialize", LogTrans_Trace, 5);
    
    if ( m_NumberRedoTransactions > 0 )
    {
        SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "NOT OK (m_NumberRedoTransactions > 0)");
        return false;
    }
    
    m_NumberRedoTransactions = k51number_of_trans_entries();
    
    if ( 0 == m_pIoSeqContainer )
    {
        newarray (m_pIoSeqContainer, m_NumberRedoTransactions, m_RawAllocator);
        if (NULL == m_pIoSeqContainer)
        {
            SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "NOT OK (NULL == m_pIoSeqContainer)");
            return false;
        }
    }
    
    if ( 0 == m_pJobContainer )
    {
        newarray (m_pJobContainer, m_NumberRedoTransactions, m_RawAllocator);
        if ( 0 == m_pJobContainer )
        {
            SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "NOT OK (NULL == m_pJobContainer)");
            return false;
        }
    }
    
    SAPDB_UInt i;
    
    for (i = 0; i < m_NumberRedoTransactions; ++i)
        m_IoSeqFreeList.Append (m_pIoSeqContainer+i);

    for (i = 0; i < m_NumberRedoTransactions; ++i)
        (m_pJobContainer+i)->SetOwnIndex (i+1);

    return true;
}
//----------------------------------------------------------------------------
Rst_RedoTrafficControl::~Rst_RedoTrafficControl()
{
    if ( m_NumberRedoTransactions == 0 )
        return;
    if ( NULL != m_pIoSeqContainer )
        destroyarray (m_pIoSeqContainer,m_NumberRedoTransactions,m_RawAllocator);
    if ( NULL != m_pJobContainer )
        destroyarray (m_pJobContainer,m_NumberRedoTransactions,m_RawAllocator);
    m_NumberRedoTransactions = 0;
}
//----------------------------------------------------------------------------
Rst_RedoTrafficControl::CheckIoSeqResult
Rst_RedoTrafficControl::CheckIoSeq (tsp00_TaskId          RedoTaskId,
                                    tgg00_WaitContext    &WaitContext,
                                    Log_IOSequenceNo      IoSeq,
                                    bool                 &waitInserted,
                                    SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::CheckIoSeq", LogTrans_Trace, 5);
    
    Rst_RedoSync RedoSync (RedoTaskId);

    if ( m_redoIsAborted )
        return CheckIoSeqRedoIsAborted;
    
    if (! m_IoSeqList.WaitForIoSeq (RedoTaskId, WaitContext, IoSeq,
                                    m_OldestKnownEOTSequence, waitInserted, errlist))
    {
        WriteToTrace ("REDOTRAFFIC: CheckIoSeq ERROR");
        return CheckIoSeqError;
    }
    
    if ( waitInserted )
    {
        ++m_WaitingRedoTasks;
        if ( ! AbortIfAllRedoTasksAreWaiting(RedoTaskId, errlist, true) )
            return CheckIoSeqRedoIsAborted;
    }
        
    return CheckIoSeqOk;
}
//----------------------------------------------------------------------------
bool Rst_RedoTrafficControl::WaitForIoSeq (tsp00_TaskId RedoTaskId)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::WaitForIoSeq", LogTrans_Trace, 5);
    vsuspend (RedoTaskId, LOG_WAIT_FOR_PRECEDING_TRANS);
    {
        Rst_RedoSync RedoSync (RedoTaskId);
        return m_redoIsAborted;
    }
}
//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::WaitForAllRedoTasks ( tsp00_TaskId RedoTaskId )
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::WaitForAllRedoTasks", LogTrans_Trace, 5);
    {
        Rst_RedoSync RedoSync (RedoTaskId);
        if ( m_IoSeqList.IsEmpty() )
            return;
        if ( ! m_RedoReaderWaitState.TaskWillWait(RedoTaskId, RedoReaderWaitState::waitUntilRedoListIsEmpty) )
            RTE_Crash( SAPDBErr_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                       "Rst_RedoTrafficControl::WaitForAllRedoTasks: m_TaskWaitingForEmptyList != nil_pid") );
        m_VacantJobWait.ResumeAll();
    }
    vsuspend (RedoTaskId, WAIT_REASON_EMPTY_LIST);
}
//----------------------------------------------------------------------------
void Rst_RedoTrafficControl::EnableRedoProcessing (tsp00_TaskId LogReaderTaskId)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoTraffic::EnableRedoProcessing", LogTrans_Trace, 5);

    Rst_RedoSync RedoSync (LogReaderTaskId);

    Rst_IoSeqEntry *firstIoSeqEntry = m_IoSeqList.GetFirst();
    m_RedoProcessingDisabled        = false;
    m_pFirstVacancy = firstIoSeqEntry == 0
                      ? 0
                      : firstIoSeqEntry->GetFirstJobEntry();
    m_VacantJobWait.ResumeAll();
}
