/*!
    @ingroup        Restart
    @file           Rst_RedoReadTask.cpp
    @author         UweH
    @brief          defines a singleton to handle a task which reads the log
                    and creates redo files
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
#include "ggg92.h"   // used by hkb51.h
#include "heo52.h"   // vclock
#include "hgg01_3.h" // g01restart_time
#include "hkb51.h"   // kb51GetRedoTransEntry, kb51CreateRedoTransEntry
#include "hkb50.h"   // k50GetLogTransaction, k50RegisterTransaction
#include "hkb57.h"   // kb57RedoTransNoIncrement
#include "hkb57_1.h" // k57restartrec

#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Container/Container_Vector.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "Restart/Rst_RedoManager.hpp"
#include "Restart/Rst_RedoReadTask.hpp"
#include "Restart/Rst_IRedoTrafficControl.hpp"
#include "Restart/Rst_Exceptions.hpp"
#include "Restart/Rst_LogEntryIterator.hpp"
#include "Logging/Log_VolumeIterator.hpp"
#include "Logging/Log_SeqTapeReader.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_AfterImage.hpp"
#include "Logging/Log_ActionUndoRedo.hpp"
#include "Logging/Log_ActionPartialRollback.hpp"
#include "Logging/Log_ActionSavepoint.hpp"
#include "Logging/Log_OpenTransFile.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Rst.h"

/// EndOfTrans will check the IOSequence of the inserted job
#define CHECK_ASCENDING_EOT_SEQUENCE            true   // PTS 1113641 UH 2002-01-25 

/// every NUMBER_OF_ENTRIES_FOR_NEXT_WRITE entries, a message will be written in knldiag
/// also it is the checked, whether the restart_time has been expired, so that a savepoint
/// has to be written
#define NUMBER_OF_ENTRIES_FOR_NEXT_WRITE        50000

/// Defines the state of the iterator
class Rst_CopyState
{
public:
    /// Default CTOR. This must not used for the first initialization, because
    /// m_isFirstEntry has to be set to TRUE for the first entry. 
    Rst_CopyState()
        :
        m_copyPending(false),
        m_isMultipleEntry(false),
        m_skipIncompleteEntries(false),
        m_transaction(NULL),
        m_alreadyCopied(false)
    {}
    
    /// initializes the state for starting the redo. I.e. the state isFirstEntry has to be
    /// set to true
    /// @param skipIncompleteEntries  Has to be set to true for online-savepoints because
    ///                               only redosavepoints need the continuation copy into
    ///                               the redofile
    void InitializeForRedo(const bool skipIncompleteEntries = false)
    {
        *this = Rst_CopyState();
        if ( skipIncompleteEntries )
        {
            SkipNextIncompleteEntries();
        }
        else
        {
            CopyNextIncompleteEntries(); // PTS 1137218 UH 2005-08-09
        }
    }

    /// Writes Information of the copying-state into the Kernel_VTrace
    void WriteToTrace(const char * text)
    {
        Kernel_VTrace() << " Rst_CopyState :: " << text << NewLine ;
        Kernel_VTrace() <<  " copy pending = " <<   ( m_copyPending     ? "TRUE" : "FALSE") <<
                            " multipleEntry = " <<  ( m_isMultipleEntry ? "TRUE" : "FALSE") <<
                            " already copied = " << ( m_alreadyCopied   ? "TRUE" : "FALSE") <<
                            " skip incomplete entries = " << ( m_skipIncompleteEntries    ? "TRUE" : "FALSE") <<
                            " entrySequence = " << m_entrySequence << NewLine;
        if ( NULL != m_transaction )
        {
            Kernel_VTrace() <<  " transno = " << m_transaction->GetRedoTransNo().gg90GetInt4() << NewLine ;
        }
        else
        {
            Kernel_VTrace() <<  " m_transaction is NULL" << NewLine ;
        }
            
    }        

    /// Finishes the copying of a RedoEntry by releasing all used pages and
    /// locks. Needed values are given as parameters, because members are
    /// not yet given (i.e. called from StartCopyEntry)
    SAPDB_Bool MaterializeEntryInRedoFile(const Log_IOSequenceNo        ioSequence,
                                                Log_Transaction        &transaction,
                                                Data_SplitSpaceWriter  &writer,
                                                SAPDB_Bool             alreadyCopied,
                                                SAPDB_Bool             multipleEntry,
                                                SAPDBErr_MessageList   &errlist)
    {
        if ( LogTrans_Trace.TracesLevel(9) )
        {
            m_transaction = &transaction;
            m_alreadyCopied = alreadyCopied;
            m_isMultipleEntry = multipleEntry;
            
            WriteToTrace("MaterializeEntryInRedoFile");
        }
            
        if ( ! alreadyCopied)
        {
            if ( ! multipleEntry )
            {
                if ( ! transaction.FinishCopiedRedoEntry(errlist)) 
                {
                    writer.WriteToTrace("writer");
                    return false;
                }
            }
            else
            {
                if (! transaction.RestoreLocalRedoEntries (ioSequence,errlist))
                {
                    errlist = errlist + Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"transaction.RestoreLocalRedoEntries");
                    writer.WriteToTrace("writer");
                    return false;
                }
            }
        }
        transaction.SetLastRedoEntryComplete(); 
        *this = Rst_CopyState();
        return true;
    }



    /// Finishes the copying of a RedoEntry by releasing all used pages and
    /// locks. Needed values are taken from the member-variables. (called from
    /// ContinueCopyEntry)
    SAPDB_Bool MaterializeEntryInRedoFile(const Log_IOSequenceNo        ioSequence,
                                                SAPDBErr_MessageList   &errlist )
    {
        return  MaterializeEntryInRedoFile(ioSequence, *m_transaction, m_writer, 
                                           m_alreadyCopied, m_isMultipleEntry, errlist);
    }
    
    void SetMultipleAction( const SAPDB_Bool isMultipleEntry)
    {
        m_isMultipleEntry = isMultipleEntry;
    }

    /// Sets the state of the entry to CopyPending. This means not the
    /// whole entry could be yet copied and more calls to ContinueCopyEntry
    /// are needed in order to finish copying of this entry.
    void SetCopyPending(          Log_Transaction        *transaction,
                        const     Data_SplitSpaceWriter   splitSpaceWriter,
                        const     SAPDB_Bool              alreadyCopied,
                        const     Log_EntrySequence       entrySequence)
    {
        m_copyPending = true;
        m_transaction = transaction;
        m_transaction->SetLastRedoEntryIncomplete();
        m_writer = splitSpaceWriter;
        m_alreadyCopied = alreadyCopied;
        m_entrySequence = entrySequence;

        if ( LogTrans_Trace.TracesLevel(9) )
            WriteToTrace("SetCopyPending");
    }
    
    /// Sets the transaction in this object at the time of the recreation of
    /// the transaction.
    void SetRecreatedOpenTransaction( Log_Transaction        *transaction )
    {
        m_copyPending = true;
        m_transaction = transaction;
        m_transaction->SetLastRedoEntryIncomplete();
        m_writer = Data_SplitSpaceWriter();
        m_alreadyCopied = false;

        if ( LogTrans_Trace.TracesLevel(9) )
            WriteToTrace("SetRecreatedOpenTransaction");
    }

    /// Returns true, if the associated entry is of type Log_ActionMultiple
    SAPDB_Bool IsMultipleEntry () const
    {
        return m_isMultipleEntry;
    }

    /// Returns true, if the associated entry is not complete copied
    SAPDB_Bool IsCopyPending () const
    {
        return m_copyPending;
    }

    /// Returns the transaction of the currently copied entry
    Log_Transaction & GetTransaction() const
    {
        SAPDBERR_ASSERT_STATE (m_transaction != NULL);
        return *m_transaction;
    }

    /// returns the writer, which is used to copy this entry into the Redofile
    Data_SplitSpaceWriter & GetWriter() 
    {
        return m_writer;
    }

    /// returns true, if this entry has already been copied and thus can
    /// be skipped by the Log_Reader
    SAPDB_Bool AlreadyCopied() const
    {
        return m_alreadyCopied;
    }

    /// returns true, if the current entry has to be skipped
    SAPDB_Bool SkipIncompleteEntries() const
    {
        return m_skipIncompleteEntries;
    }

    /// Resets the flag, which defines, whether the current entry has to be skipped
    void CopyNextIncompleteEntries()
    {
        // PTS 1137218 UH 2005-08-09 new
        m_skipIncompleteEntries = false;
    }
        
    /// Resets the flag, which defines, whether the current entry has to be skipped
    void SkipNextIncompleteEntries()
    {
        m_skipIncompleteEntries = true;
    }
    
    /// Reads the redo entry sequence of the entry. Valid only for continuation-entries
    Log_EntrySequence GetRedoEntrySequence() const 
    {
        return m_entrySequence;
    }
        
private:
    SAPDB_Bool            m_copyPending;     ///< true if the last entry has been copied completely
    SAPDB_Bool            m_isMultipleEntry; ///< true if the last entry has been a multiple entry
    SAPDB_Bool            m_alreadyCopied;   ///< true if the entry has been copied already (redo of redo)
    SAPDB_Bool            m_skipIncompleteEntries;    ///< true if the current entry has to be skipped by the read-iterator
    Log_Transaction      *m_transaction;     ///< pointer to the current transaction
    Data_SplitSpaceWriter m_writer;          ///< the writer, which is used to append parts of logentries
    Log_EntrySequence     m_entrySequence;   ///< the sequence of the entry for debugging purposes
};



/*!
    @class Rst_AfterImageHeadSpace
    @brief This is used to implement the interface Data_ISplitSpace for reading purpose.
 */
class Rst_AfterImageHeadSpace : public Data_ISplitSpace
{
public:
	/// returns the number of current part
    virtual PartNo CurrentPart () const
    {
        return 0;
    }
    /// get one part
    virtual void GetPart (PartNo       Part,
                          SAPDB_UInt  &Length,
                          SAPDB_Byte* &Value) const
    {
        Length = m_Length;
        Value  = m_Value;
    }
	/// returns lenght of complete splitspace
    virtual SAPDB_UInt Length() const
    {
        return m_TotalLength;
    }
	/// checks, whether an value is assigned or not
    virtual bool IsAssigned() const
    {
        return m_Value != 0;
    }
    /// Appends the given information to the errorlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
	{
        errlist = errlist + Restart_Exception(filename,linenumber,RST_AFTERIMAGE_HEAD_INFO,
                                              title==0?"":title,
                                              m_Value==0?"":(SAPDB_Char const *)SAPDB_ToString(m_Value, m_Length, SAPDB_ToStringClass::hex),
                                              SAPDB_ToString(m_Length),
                                              SAPDB_ToString(m_TotalLength));
	}
    /*!
    @return false, if space begin is not properly aligned
    @brief  This checks if the split space begins at an aligned position
    */
    virtual bool CheckAlignment() const
    {
         if ( ! IsAssigned() )
             return true;
             
         SAPDB_UInt8 address =
                         #ifdef BIT64
                         reinterpret_cast<SAPDB_UInt8>(m_Value);
                         #else
                         reinterpret_cast<SAPDB_UInt4>(m_Value);
                         #endif
         
         return address % LOG_PAGE_ALIGNMENT == 0;
    }
    /// default constructor
    Rst_AfterImageHeadSpace ()
    : m_Value       (NULL),
      m_Length      (0),
      m_TotalLength (0)
    {}
    /*!
        @brief  assigns a new space
        @param  iter the logentry, which will be assigned to the Afterimage
     */
    void Assign (const Rst_LogEntryIterator& iter)
    {
        m_Value       = iter.GetValuePtr();
        m_Length      = iter.Length();
        m_TotalLength = iter.RestLength();
    }
private:
    /// pointer to the value of the current entry
    SAPDB_Byte *m_Value;
    /// length of the current entry
    SAPDB_UInt m_Length;
    /// total length of the current entry
    SAPDB_UInt m_TotalLength;
};
/* --------------------------------------------------------------------------- */
Rst_RedoReadTask::Rst_RedoReadTask ( Rst_LogEntryIterator   &logIter,
                                     Rst_RedoManager        &redoManager,
                                     volatile bool          &redoIsAborted )
: m_logIter (logIter)
, m_RedoManager(redoManager)
, m_redoIsAborted(redoIsAborted)
{}

/* --------------------------------------------------------------------------- */
Rst_RedoReadTask::~Rst_RedoReadTask()
{
    m_logIter.DeleteIter();
    m_TransactionsToSkip.Delete(); // PTS 1134103 UH 2005-03-07
}

/* --------------------------------------------------------------------------- */
Rst_RedoReadTask::ReadLogResult
Rst_RedoReadTask::ReadLogAndCreateRedoFiles (tsp00_TaskId          taskid,
                                             tgg00_TransContext   &oldcontext,
                                             SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::ReadLogAndCreateRedoFiles", LogTrans_Trace, 5);

    Trans_Context &context = Trans_Context::GetContext(oldcontext);
    Log_Volume    &log     = Log_Volume::Instance();

    // allocate initial array for the queue-states (PTS 1133580 mb 2005-01-25)
    Container_Vector<Rst_CopyState> copyState (context.Allocator());    
    Log_QueueID configuredQueueCount = g01usedmaxlogqueuecount();
    if ( ! copyState.Resize(configuredQueueCount))
    {
        errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                     "Not enough memory: initial copyState.Resize() failed");
        return RedoReadTransHandlingError;
    }
    if ( ! ReCreateOpenTransactions (taskid, oldcontext, context.Allocator(), copyState, errlist) )
	{
	    m_RedoManager.GetRedoTrafficControl()->SetEndOfLogRead (taskid);
        m_TransactionsToSkip.Delete(); // PTS 1134103 UH 2005-03-07
        return RedoReadReCreateOpenTransFailed;
	}

    if ( ! m_logIter.Initialize(taskid, 
                                context.Allocator(), 
                                &(oldcontext.trRteCommPtr_gg00->to_cancel), 
                                m_OldestKnownEOTSequence,
                                errlist ) )
    {
        return RedoReadError;
    }
    

    const ReadLogInternResult result = InternReadLogAndCreateRedoFile(  taskid, 
                                                                        context.Allocator(), 
                                                                        oldcontext, 
                                                                        m_logIter, 
                                                                        copyState,
                                                                        errlist);

    m_logIter.DeleteIter();
    m_TransactionsToSkip.Delete(); // PTS 1134103 UH 2005-03-07

    switch (result)
    {
        case ReadLogOk:
            break;
        case ReadLogAborted:
            m_redoIsAborted = true;
            break;
        case ReadLogSavepointNotFound:
            return RedoReadSavepointNotFound;
        case ReadLogDifferentEotFound:
        case ReadLogReadActionError:
        case ReadLogError:
        case ReadLogMaterializeError:
        case ReadLogPartialRollbackError:
        case ReadLogEndOfTransHandlingFailed:
        case ReadLogRegisterTransactionFailed:
        case ReadLogCloseOpenTransFailed:
            return RedoReadTransHandlingError;
        case ReadLogSoftwareTooOld:
            return RedoReadSoftwareTooOld;
        case ReadLogResourceShortage:
            return RedoReadError;
        default:
            errlist = errlist + Restart_Exception(__CONTEXT__, RST_UNEXPECTED_RESULT,
                                                  "InternReadLogAndCreateRedoFile",
                                                  SAPDB_ToString(result));
    }

    if ( m_redoIsAborted )
    {
        m_RedoManager.GetRedoTrafficControl()->SetEndOfLogRead (taskid);
        errlist = Restart_Exception(__CONTEXT__, RST_REDO_ABORTED);
        return RedoReadAborted;
    }

    if ( ! CloseOpenTransactions (taskid, context.Allocator(), m_logIter.GetLastProcessedIOSeq(), errlist) )
        return RedoReadCloseOpenTransFailed;

    if ( m_logIter.GetLastProcessedIOSeq().IsValid()
         &&
         Log_Savepoint::Instance().StartSavepointAndWait(context.Task(), Log_SVPReasonDistance, errlist)
         !=
         Log_Savepoint::ok )
	{
        RTE_Message(errlist);
        errlist.ClearMessageList();
	}

    m_RedoManager.GetRedoTrafficControl()->SetEndOfLogRead (taskid);

    return RedoReadOk;
}
/* --------------------------------------------------------------------------- */
Rst_RedoReadTask::ReadLogInternResult
Rst_RedoReadTask::InternReadLogAndCreateRedoFile(tsp00_TaskId             taskid,
                                                 SAPDBMem_IRawAllocator  &allocator,
                                                 tgg00_TransContext      &oldcontext,
                                                 Rst_LogEntryIterator    &iter,
                                                 QueueCopyStates         &copyState,
                                                 SAPDBErr_MessageList    &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::InternReadLogAndCreateRedoFile", LogTrans_Trace, 5);
    
    Rst_AfterImageHeadSpace  entryHeadSpace;
    Log_AfterImage           afterImage;
    bool                     isOK            = true;
    bool                     checkForSavepointEntry
                             = Log_Savepoint::GetLastSavepointEntryType() != Log_NoOp; // for migration
    bool                     endOfTransAlreadyProcessed = false; // PTS 1123461 UH 2003-08-28
    tgg00_TransIndex         currTransIndex;
    Log_Transaction         *pTransaction    = NULL;
    SAPDB_UInt               readEntryCount  = 0;
    SAPDB_Int                timeInSec;
    SAPDB_Int                dummyTimeInMSec;
    SAPDB_Int                lastSavepointTriggerTime;
    Log_IOSequenceNo         lastIOSequence;
    ReadLogInternResult      result = ReadLogOk; // PTS 1133003 mb 2004-12-15 

    vclock (&lastSavepointTriggerTime, &dummyTimeInMSec);



    while (    (iter.GetState() == Log_VolumeIterator::Iter_ok )
            && (ReadLogOk == result ))
    {
        if ( m_redoIsAborted )
        {
            m_RedoManager.GetRedoTrafficControl()->SetEndOfLogRead (taskid);
            errlist = Restart_Exception(__CONTEXT__, RST_REDO_ABORTED);
            result = ReadLogAborted;
            break;
        }

        iter.CheckSuspendForStandby(taskid);

        if ( ! CheckCopyStateAllocation(copyState, iter.GetQueueId()))
        {
            errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                         "Not enough memory: CheckCopyStateAllocation for new queue failed");
            result = ReadLogResourceShortage;
            break;
        }

        // reset the state for a queue if the first entry after a restart is read
        if (    iter.QueueSeqNo() == MIN_IOSEQUENCE 
             && copyState[iter.GetQueueId()].IsCopyPending() )
        {
            copyState[iter.GetQueueId()].InitializeForRedo();
        }

        lastIOSequence = iter.IOSeqNo();

        if (copyState[iter.GetQueueId()].SkipIncompleteEntries())
        // PTS 1133580 mb 2005-01-25
        // due to interleaving of multiple queues, a logentry may be interrupted by the savepointentry.
        // In this case the incomplete entry has to be ignored.
        {
            
            if ( checkForSavepointEntry )
            // the first read entry has to be the savepoint-entry or, in case of a redo-savepoint,
            // the start of another valid entry. Thus it must be finished!
            {
                copyState[iter.GetQueueId()].CopyNextIncompleteEntries();
            }
            else
            {
                while (iter.IsTailOfAnEntry() && copyState[iter.GetQueueId()].SkipIncompleteEntries())
                {
                    ++iter;
                }
                copyState[iter.GetQueueId()].CopyNextIncompleteEntries();
            }
        }

        if ( ! copyState[iter.GetQueueId()].IsCopyPending() || checkForSavepointEntry)
        // a new entry is starting
        {
            entryHeadSpace.Assign           (iter);
            Data_SplitSpaceReader reader    (entryHeadSpace);
            if(!afterImage.ReadPersistentFormat (reader, errlist))
            {
                errlist = errlist + Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                                      "Error during reading of after image");
                iter.AppendContextInfo(__CONTEXT__,SAPDB_MAX_UINT4, 0, errlist);
                result = ReadLogError;
                break;
            }
        
            if ( checkForSavepointEntry )
            // check the type of the entry, if it is the savepoint of this restart
            {
                checkForSavepointEntry = false;
                
                if ( afterImage.GetActionType() != Log_Savepoint::GetLastSavepointEntryType() )
                {
                    errlist = Restart_Exception(__CONTEXT__, RST_SAVEPOINT_NOT_FOUND,
                                                Log_GetActionTypeString(Log_Savepoint::GetLastSavepointEntryType()));
                    iter.AppendContextInfo(__CONTEXT__, afterImage.GetSequence(),
                                            afterImage.GetTransNo().gg90GetInt4(),
                                            errlist );
                    errlist = errlist + Restart_Exception(__CONTEXT__, RST_REDO_BEGIN_NOT_FOUND);
                    errlist = errlist + Restart_Exception(__CONTEXT__, RST_HINT_RESTORE_LOG);
                    result = ReadLogSavepointNotFound;
                    break;
                }
            }

            if ( ! afterImage.GetTransNo().gg90IsNil() )
            {
                // PTS 1134103 UH 2005-03-07 skip already processed transactions
                if ( m_TransactionsToSkip.Includes(afterImage.GetTransNo()) )
                {
                    copyState[iter.GetQueueId()].SkipNextIncompleteEntries(); // PTS 1137218 UH 2005-08-09
                                                                        // This causes that this entry is completely skipped
                                                                        // Even the following parts which may exist are later skipped
                                                  
                    #ifdef SAPDB_FAST
                    if ( LogTrans_Trace.TracesLevel(6) )
                    #endif
                        iter.Message ( "entry skipped, because", readEntryCount,
                                       afterImage.GetTransNo().gg90GetInt4(),
                                       afterImage.GetSequence() );
                    ++iter;
                    continue;
                }
                
                if ( ! GetOrCreateRedoTransaction ( taskid,
                                                    iter.GetQueueId(),
                                                    allocator,
                                                    oldcontext,
                                                    afterImage.GetTransNo(),
                                                    currTransIndex,
                                                    pTransaction,
                                                    errlist ) )
                {
                    result = ReadLogRegisterTransactionFailed;
                    if (! errlist.IsEmpty())
                    {
                        RTE_Message(errlist);
                        iter.WriteToTrace("GetOrCreateRedoTransaction failed");
                    }
                    else
                    {
                        iter.WriteToTrace("GetOrCreateRedoTransaction failed but empty errlist");
                    }
                    break;
                }
            }
            else
            {
                currTransIndex = cgg_nil_transindex;
                pTransaction = NULL; // PTS 1133931 mb 2005-02-21
            }
            
            // The read task builds then redo files with a correct transid.
            oldcontext.trWriteTransId_gg00 = afterImage.GetTransNo();
            oldcontext.trIndex_gg00        = currTransIndex;

            // save current read entry position for redo-savepoint
            iter.SetCurrentReadPosition (afterImage.GetActionType());
            ++readEntryCount;
            
            // write progress messages
            if ( LogTrans_Trace.TracesLevel(9)
                 ||
                 (readEntryCount % NUMBER_OF_ENTRIES_FOR_NEXT_WRITE == 0) )
                 iter.Message ( "redo-read", readEntryCount,
                                         afterImage.GetTransNo().gg90GetInt4(),
                                         afterImage.GetSequence() );

            if ( readEntryCount % NUMBER_OF_ENTRIES_FOR_NEXT_WRITE == 0 )
            {
                vclock (&timeInSec, &dummyTimeInMSec);
                if ( lastSavepointTriggerTime + g01restart_time <= timeInSec )
                {
                    lastSavepointTriggerTime = timeInSec;
                    Trans_Context &context = Trans_Context::GetContext(oldcontext);
                    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepoint (context.Task(), Log_SVPReasonDistance, context.ErrorList()) )
                    {
                        RTE_Message(context.ErrorList());
                        context.ErrorList().ClearMessageList();
                        // ignore this error
                    }
                }
            }
            
            endOfTransAlreadyProcessed = false; // PTS 1123461 UH 2003-08-28
            
            if ( pTransaction != 0
                 &&
                 pTransaction->GetTransType() != Log_Transaction::redoRead )
            {
                // PTS 1123461 UH 2003-08-28 new if-clause
                // The context is already given to the redo tasks.
                // Do not insert it again to the redo traffic control.
                // But check the correct EndOfTrans handling.
                if ( (pTransaction->GetTransState() == Log_Transaction::committed
                      &&
                      afterImage.GetActionType() != Log_Commit)
                      /* PTS 1132673 UH 2004-11-25
                         if trans is marked as rollbacked the next entry may be of any kind
                         but it must be the last entry in the log !!!
                      ||
                      (pTransaction->GetTransState() == Log_Transaction::rollbacked
                      &&
                      afterImage.GetActionType() != Log_Rollback)
                      */
                      ||
                      (pTransaction->GetTransState() == Log_Transaction::opened) )
                {
                    const char* foundStr =  afterImage.GetActionType() == Log_Commit
                                           ? "COMMIT"
                                           : "ROLLBACK";
                    
                    errlist = Restart_Exception(__CONTEXT__, RST_DIFFERENT_EOT_FOUND,
                                                foundStr,
                                                SAPDB_ToString(pTransaction->GetTransState()) );
                    iter.AppendContextInfo          ( __CONTEXT__,
                                                      afterImage.GetSequence(),
                                                      afterImage.GetTransNo().gg90GetInt4(),
                                                      errlist );
                    pTransaction->AppendContextInfo ( __CONTEXT__, errlist );
                    result = ReadLogDifferentEotFound;
                    break;
                }
                endOfTransAlreadyProcessed = true;
            }
            
            switch ( afterImage.GetActionType() )
            {
            case Log_Commit:
            {
                SAPDBERR_ASSERT_STATE( iter.RestLength() == iter.Length() );
                Log_ActionEndOfTransaction commitAction (Log_Commit);

                if ( iter.RestLength()
                     ==
                     commitAction.GetPersistentLength() + Log_AfterImage::GetPersistentLength() )
                {
                    // PTS 1124045 mb 2003-09-08 commit-logentry with timestamp for point-in-time recovery
                    if(!commitAction.ReadPersistentFormat (reader, allocator, errlist))
                    {
                        errlist = errlist + Restart_Exception(__CONTEXT__, RST_ERROR_DURING_READ_ACTION);
                        iter.AppendContextInfo          ( __CONTEXT__, afterImage.GetSequence(),
                                                          afterImage.GetTransNo().gg90GetInt4(),
                                                          errlist );
                        pTransaction->AppendContextInfo ( __CONTEXT__, errlist );
                        result = ReadLogReadActionError;
                        break;
                    }
                }
                
                if ( ! iter.UntilDateIsReached(commitAction.GetCommitDate(),commitAction.GetCommitTime()))
                {
                    if ( ! endOfTransAlreadyProcessed ) // PTS 1123461 UH 2003-08-28
                    {
                        if ( ! EndOfTransaction ( taskid,
                                           allocator,
                                           pTransaction,
                                           afterImage.GetActionType() == Log_Commit
                                           ? Log_Transaction::committed
                                           : Log_Transaction::rollbacked,
                                           iter.IOSeqNo(),
                                           currTransIndex,
                                           CHECK_ASCENDING_EOT_SEQUENCE, // PTS 1113641 UH 2002-01-25
                                           errlist ) )
                        {
                            result = ReadLogEndOfTransHandlingFailed;
                            break;
                        }
                    }
                }
                break;
            }
            case Log_Rollback:
                SAPDBERR_ASSERT_STATE( iter.RestLength() == iter.Length() );

                if ( ! endOfTransAlreadyProcessed ) // PTS 1123461 UH 2003-08-28
                {
                    if ( ! EndOfTransaction ( taskid,
                                       allocator,
                                       pTransaction,
                                       afterImage.GetActionType() == Log_Commit
                                       ? Log_Transaction::committed
                                       : Log_Transaction::rollbacked,
                                       iter.IOSeqNo(),
                                       currTransIndex,
                                       CHECK_ASCENDING_EOT_SEQUENCE, // PTS 1113641 UH 2002-01-25
                                       errlist ) )
                    {
                        result = ReadLogEndOfTransHandlingFailed;
                        break;
                    }
                }
                break;
            case Log_FinishOpenTrans:
            {
                SAPDBERR_ASSERT_STATE( iter.RestLength() == iter.Length() );

                // PTS 1121011 mb 2003-03-11 extract kernel-version from savepoint
                Log_ActionSavepoint SVPAction (Log_SVPReasonRestart);
                if(!SVPAction.ReadPersistentFormat (reader, allocator, errlist))
                {
                    errlist = errlist + Restart_Exception(__CONTEXT__, RST_ERROR_DURING_READ_ACTION);
                    iter.AppendContextInfo          ( __CONTEXT__,
                                                      afterImage.GetSequence(),
                                                      afterImage.GetTransNo().gg90GetInt4(),
                                                      errlist );
                    result = ReadLogReadActionError;
                    break;
                }
                if (SVPAction.HasKernelVersionString())
                {
                    if ( ! Kernel_IAdminConfig::Instance().MigrationHandler().
                                CheckCompatibility( Kernel_Migration::log,
                                                    Kernel_Version(SVPAction.GetKernelVersionString()),
                                                    errlist ) )
                    {
                        errlist = errlist + Restart_Exception(__CONTEXT__, RST_SOFTWARE_VERSION_TOO_OLD)
                                  +
                                  Restart_Exception(__CONTEXT__, RST_SAVEPOINT_VERSION,
                                                    SVPAction.GetKernelVersionString());
                        iter.AppendContextInfo     ( __CONTEXT__,
                                                     afterImage.GetSequence(),
                                                     afterImage.GetTransNo().gg90GetInt4(),
                                                     errlist );
                        result = ReadLogSoftwareTooOld;
                        break;
                    }
                }

                if ( ! CloseOpenTransactions (taskid, allocator, lastIOSequence, errlist) )
                {
                    errlist = Restart_Exception(__CONTEXT__, RST_CLOSE_OPEN_TRANS_FAILED);
                    iter.AppendContextInfo     ( __CONTEXT__, 
                                                 afterImage.GetSequence(),
                                                 afterImage.GetTransNo().gg90GetInt4(),
                                                 errlist );
                    result = ReadLogCloseOpenTransFailed;
                    break;
                }
                for ( unsigned int i = 0; i < copyState.GetSize() ; i++)  // PTS 1134170 mb 2005-03-17
                {
                    copyState[i].InitializeForRedo();
                }
                break;
            }
            case Log_PartialRollback:
            {
                SAPDBERR_ASSERT_STATE( iter.RestLength() == iter.Length() );
                Log_ActionPartialRollback Action;
                if(!Action.ReadPersistentFormat (reader, allocator, errlist))
                {
                    errlist = Restart_Exception(__CONTEXT__, RST_ERROR_DURING_READ_ACTION);
                    iter.AppendContextInfo          ( __CONTEXT__, 
                                                      afterImage.GetSequence(),
                                                      afterImage.GetTransNo().gg90GetInt4(),
                                                      errlist );
                    pTransaction->AppendContextInfo ( __CONTEXT__, errlist );
                    result = ReadLogReadActionError;
                    break;
                }
                SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "truncate: inclusive stopseqence: "
                                                       << Action.GetRedoStopSequence());
                // PTS 1113230 UH 2001-12-20 begin
                if ( ! pTransaction->RedoPartialRollback ( Action.GetUndoStopSequence(),
                                                           Action.GetRedoStopSequence(),
                                                           errlist ) )
                {
                    errlist = errlist + Restart_Exception(__CONTEXT__, RST_ERROR_DURING_PARTIAL_ROLLBACK);
                    iter.AppendContextInfo          ( __CONTEXT__, 
                                                      afterImage.GetSequence(),
                                                      afterImage.GetTransNo().gg90GetInt4(),
                                                      errlist );
                    result = ReadLogPartialRollbackError;
                    break;
                }
                // PTS 1113230 UH 2001-12-20 end
                break;
            }
            case Log_SavepointEntry:
                SAPDBERR_ASSERT_STATE( iter.RestLength() == iter.Length() );
                 // nothing to do
                break;
            default:
                if (! StartCopyEntry (*pTransaction, iter, copyState[iter.GetQueueId()], afterImage, errlist))
                {
                    errlist = errlist + Restart_Exception(__CONTEXT__, RST_ERROR_DURING_COPY_ENTRY);
                    result = ReadLogReadActionError;
                    break;
                }
                break;
            }//switch
        }
        // continuation of a logentry
        else
        {
            if (! ContinueCopyEntry (iter, copyState[iter.GetQueueId()], errlist))
            {
                errlist = errlist + Restart_Exception(__CONTEXT__, RST_ERROR_DURING_COPYCONT_ENTRY);
                result = ReadLogReadActionError;
                break;
            }
        }

        if (   (Log_VolumeIterator::Iter_ok == iter.GetState() )
            && (ReadLogOk == result ))
        {
            ++iter;
        }
    } //endwhile

    if (errlist.IsEmpty())
    {
        iter.Message ( "last-redo-read empty errlist", readEntryCount, afterImage.GetTransNo().gg90GetInt4(), afterImage.GetSequence() );
    }
    else
    {
        iter.Message ( "last-redo-read non empty errlist", readEntryCount, afterImage.GetTransNo().gg90GetInt4(), afterImage.GetSequence() );
        RTE_Message(errlist);
    }
    iter.WriteToTrace("InternReadLogAndCreateRedoFile stopped reading");
    Kernel_VTrace() << "InternReadLogAndCreateRedoFile result = " << result;

    if ( m_redoIsAborted )
    // Deallocate memory of all transactions and the redofile.
    // The persistent files must not be dropped
    {
        for ( unsigned int i = 0; i < copyState.GetSize() ; i++) 
        {
            if (copyState[i].IsCopyPending())
            {
                Log_Transaction * trans = & copyState[i].GetTransaction();
                if (trans != NULL)
                {
                    Msg_List tmpErrList;
                    trans->DeleteAfterRedoCopy(tmpErrList);
                    if (! tmpErrList.IsEmpty())
                    {
                        RTE_Message(tmpErrList);
                    }
                    allocator.Deallocate(trans);
                }
            }
        }
    }
	return result;
}
/* --------------------------------------------------------------------------- */
bool Rst_RedoReadTask::CloseOpenTransactions (tsp00_TaskId                     taskid,
                                              SAPDBMem_IRawAllocator          &allocator,
                                              Log_IOSequenceNo                 lastIOSequence,
                                              SAPDBErr_MessageList            &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::CloseOpenTransactions", LogTrans_Trace, 5);
    
    tgg00_TransIndex  openTransIndex = cgg_nil_transindex;
    Log_Transaction  *pTransaction   = 0;

    if ( lastIOSequence.IsInvalid() )
    {
        // PTS 1124684 UH 2003-10-16
        lastIOSequence = m_RedoManager.GetRedoTrafficControl()->GetLastIOSequence(taskid);
        if ( lastIOSequence.IsInvalid() )
            lastIOSequence  = 0;
    }
    
    kb51GetFirstOpenRedoTrans (taskid, openTransIndex);

    while ( openTransIndex != cgg_nil_transindex )
    {
        pTransaction = reinterpret_cast<Log_Transaction*>(k50GetLogTransaction (openTransIndex));
        if ( 0 == pTransaction )
        {
            errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                         "k50GetLogTransaction() returned nothing");
            return false;
        }

        if ( pTransaction->GetRedoEotSequence().IsValid() )
        {
            errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                         "eot sequence is valid although forced to be rollbacked");
            return false;
        }
        
        RTE_Message( Msg_List( Msg_List::Info, SDBMSG_LOG_OPEN_TRANS_IS_ROLLBACKED,
                               Msg_Arg(SDBMSGTAG_LOG_OPEN_TRANS_IS_ROLLBACKED__TRANS_ID, SAPDB_ToString(pTransaction->GetRedoTransNo().gg90GetInt4())),
                               Msg_Arg(SDBMSGTAG_LOG_OPEN_TRANS_IS_ROLLBACKED__NUMBER,   SAPDB_ToString(pTransaction->GetUndoEntryCount())) ) );
                               
        if ( ! EndOfTransaction ( taskid,
                                  allocator,
                                  pTransaction,
                                  Log_Transaction::rollbacked,
                                  lastIOSequence,
                                  openTransIndex,
                                  ! CHECK_ASCENDING_EOT_SEQUENCE, // PTS 1113641 UH 2002-01-25
                                  errlist ) )
        {
            return false;
        }

        // force search at the beginning of the transactions list
        // until there exists no more open transaction
        openTransIndex = cgg_nil_transindex;
        kb51GetFirstOpenRedoTrans (taskid, openTransIndex);
    }//while
    // PTS 1129746 UH 2004-06-18 added wait for all transactions which end at this time
    m_RedoManager.GetRedoTrafficControl()->WaitForAllRedoTasks(taskid);
    return true;
}
/* --------------------------------------------------------------------------- */
bool Rst_RedoReadTask::ReCreateOpenTransactions (tsp00_TaskId            taskid,
                                                 tgg00_TransContext     &oldcontext,
                                                 SAPDBMem_IRawAllocator &allocator,
                                                 QueueCopyStates        &copyState,
                                                 SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::ReCreateOpenTransactions", LogTrans_Trace, 5);

    tkb00_SaveptParam &saveptParam = k57restartrec->rstLastSavept_kb00();


    const SAPDB_Char* openTransFileWithQueueId = "OPEN TRANS FILE WITH QUEUE ID";

    Kernel_Migration::ChangeInfo& queueIdInOpentransfile =
        Kernel_IAdminConfig::Instance().MigrationHandler().SearchForChangeInfo (openTransFileWithQueueId );

    Log_OpenTransFile::EntryLayout opentransFileEntryLayout; 
    if (queueIdInOpentransfile.IsEnabled() ) 
    {
        #ifdef SAPDB_SLOW
        Kernel_VTrace() << "Rst_RedoReadTask::ReCreateOpenTransactions: old filelayout used" << NewLine ;
        #endif
        opentransFileEntryLayout = Log_OpenTransFile::HasNoQueueId; 
    }
    else
    {
        #ifdef SAPDB_SLOW
        Kernel_VTrace() << "Rst_RedoReadTask::ReCreateOpenTransactions: new filelayout used" << NewLine ;
        #endif
        opentransFileEntryLayout = Log_OpenTransFile::HasQueueId; 
    }

    Log_OpenTransFile file ( oldcontext, saveptParam.svpOpenTransRoot_kb00, opentransFileEntryLayout );

    if ( ! file.IsCreated() )
        return true; // nothing to do
    
    errlist.ClearMessageList();
    m_OldestKnownEOTSequence = file.GetOldestKnownIOSequence(errlist); // PTS 1124684 UH 2003-10-16
    if(!errlist.IsEmpty())
        return false;
    if ( LogTrans_Trace.TracesLevel(6) )
        Kernel_VTrace() << "ReCreateOpenTransactions: opentransfile.GetOldestKnownIOSequence() "
                        << m_OldestKnownEOTSequence << NewLine ;
    if (m_OldestKnownEOTSequence.IsInvalid()) 
    // for migration or if no transactions had been open during the savepoint
    {
        if ( saveptParam.svpOpenTransCnt_kb00 > 0 ) // PTS ? 2004-08-04 (opentrans file contains only rollbacked trans)
            m_OldestKnownEOTSequence = 0;
        else
            m_OldestKnownEOTSequence = saveptParam.svpIOsequence_kb00; // PTS 1127009 UH 2004-01-09, PTS 1127704
        if ( LogTrans_Trace.TracesLevel(6) )
            Kernel_VTrace() << "ReCreateOpenTransactions: m_OldestKnownEOTSequence was nil now set to "
                            << m_OldestKnownEOTSequence << NewLine;
    }
    // PTS 1114727 mb 2003-11-04 corrected settting of OldestKnownEOTSequence 
    // PTS 1127009 UH 2004-01-09 update of oldestknown for Log_Volume removed

    errlist.ClearMessageList();
    Log_OpenTransFile::Iterator &iter = file.GetFirstOpenTrans(saveptParam.svpId_kb00,errlist);
    if(!errlist.IsEmpty())
        return false;

    tgg00_TransIndex currentTransIndex;

    // PTS 1134103 UH 2005-03-07
    if ( ! m_TransactionsToSkip.Init ( allocator, m_RedoManager.GetRedoTrafficControl()->GetMaximumNumberOfTransactions() ) )
    {
        errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "m_TransactionsToSkip.Init() failed");
        return false;
    }

    const Kernel_Migration::ChangeInfo &repeatedRedoFlag = Kernel_IAdminConfig::Instance().MigrationHandler().SearchForChangeInfo ("REPEATED_REDO_FLAG_IN_OPEN_TRANS_FILE");
    m_lastSavepointDoneDuringRedo                        = repeatedRedoFlag.IsEnabled() ? false : file.CreatedDuringRedo();
    bool  dontReadLogOfTransAgain;
          
    Log_QueueID queueTmpId;
    for (queueTmpId = 0; queueTmpId < copyState.GetSize(); queueTmpId++)
    {
        copyState[queueTmpId].InitializeForRedo( ! m_lastSavepointDoneDuringRedo );
    }

    if ( m_lastSavepointDoneDuringRedo )
        Kernel_OpInfo (csp3_n_restart) << "Previous restart was interrupted.";

    while ( iter.IsValid() )
    {
        Log_Transaction* newtrans = new (allocator) Log_Transaction ();

        if ( newtrans == NULL )
        {
            errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                         "allocate of Log_Transaction");
            return false;
        }

        newtrans->ReadPersistentFormat( *iter, file.GetSpaceSize() );

        if ( newtrans->GetRedoTransNo().gg90IsNil() )
        {
            // the savepoint may collect such trans entries
            destroy (newtrans, allocator);
            if(!iter.PlusPlus(errlist))
                return false;
            continue;   // <<<<<<<<<<<<< continue with next opentransentry >>>>>>>>>>>>>>>>>
        }
        
        dontReadLogOfTransAgain = ! m_lastSavepointDoneDuringRedo && newtrans->GetTransState() == Log_Transaction::committed;
        
        if ( dontReadLogOfTransAgain )
        {
            if ( ! m_TransactionsToSkip.Add ( newtrans->GetRedoTransNo() ) ) // PTS 1134103 UH 2005-03-07
            {
                errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "m_TransactionsToSkip.Add() failed");
                return false;
            }

            // The transaction is prepared to be completly removed with all its files
            if ( ! newtrans->InitializeForRedo (oldcontext) )
            {
                errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                             "InitializeForRedo() called for delete");
                return false;
            }

            // delete the log transaction context
            oldcontext.trWriteTransId_gg00 = newtrans->GetRedoTransNo();

            if(!newtrans->DeleteAfterRedo(errlist))
                return false;

            oldcontext.trWriteTransId_gg00.gg90SetNil();

            destroy (newtrans, allocator);
            
            // skip to next open trans entry
            if(!iter.PlusPlus(errlist))
                return false;
            continue;   // <<<<<<<<<<<<< continue with next opentransentry >>>>>>>>>>>>>>>>>
        }
        
        if ( ! newtrans->InitializeForRedoCopy (oldcontext,
                                                newtrans->GetRedoTransNo(),
                                                newtrans->GetQueueID(),
                                                allocator,
                                                errlist) )
        {
            errlist = errlist + Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                                 "InitializeForRedoCopy()");
            return false; 
        }

        if ( ! newtrans->UpdateUndoRedoInfoForRestart(m_OldestKnownEOTSequence, errlist) )
        {
            errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                         "UpdateUndoRedoInfoForRestart()");
            return false;
        }

        tgg00_BasisError error = e_ok;
        
        currentTransIndex = cgg_nil_transindex;
        
        kb51CreateRedoTransEntry ( taskid,
                                   newtrans->GetRedoTransNo(),
                                   currentTransIndex,
                                   error );

        if ( error != e_ok )
        {
            Kernel_AppendBasisError(error,errlist,"kb51CreateRedoTransEntry");
            if ( e_too_many_lock_requests == error )
            {
                errlist = errlist
                          +
                          Restart_Exception(__CONTEXT__, RST_NO_TRANS_ENTRY_AVAILABLE,
                                            SAPDB_ToString(newtrans->GetRedoTransNo().gg90GetInt4()))
                          +
                          Restart_Exception(__CONTEXT__, RST_NO_TRANS_ENTRY_AVAILABLE);
            }
            return false;
        }

        k50RegisterTransaction (currentTransIndex, newtrans);

        if ( LogTrans_Trace.TracesLevel(6) )
            newtrans->WriteToTrace("OpenTrans");

        if ( dontReadLogOfTransAgain // PTS 1138299 UH 2005-10-12
             ||
             (newtrans->GetTransState() != Log_Transaction::opened
              &&
              newtrans->GetRedoEotSequence().IsValid()) )
        {
            // PTS 1134260 UH 2005-03-04 begin
            oldcontext.trIndex_gg00        = currentTransIndex;
            oldcontext.trWriteTransId_gg00 = newtrans->GetRedoTransNo();
            // PTS 1134260 UH 2005-03-04 end
            
            if ( ! EndOfTransaction ( taskid,
                               allocator,
                               newtrans,
                               newtrans->GetTransState(),
                               newtrans->GetRedoEotSequence(),
                               currentTransIndex,
                               ! CHECK_ASCENDING_EOT_SEQUENCE, // PTS 1113641 UH 2002-01-25
                               errlist ) )
            {
                return false;
            }

            // PTS 1134260 UH 2005-03-04 begin
            oldcontext.trIndex_gg00        = cgg_nil_transindex;
            oldcontext.trWriteTransId_gg00.gg90SetNil();
            // PTS 1134260 UH 2005-03-04 end
        }
        else if ( ! newtrans->LastRedoEntryIsComplete())
        // the entry has to be completed so add information to the copyState, which will be
        // later used during logreading
        {
            if ( ! CheckCopyStateAllocation(copyState, newtrans->GetQueueID()))
            {
                {
                    errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                                 "Not enough memory: CheckCopyStateAllocation for new queue failed");
                    return false;
                    break;
                }
            }
            copyState[newtrans->GetQueueID()].SetRecreatedOpenTransaction(newtrans);
        }
        
        if( ! iter.PlusPlus(errlist))
            return false;
    }
	
    if ( LogTrans_Trace.TracesLevel(6) )
        m_RedoManager.GetRedoTrafficControl()->
            WriteToTrace("End of Rst_RedoReadTask::ReCreateOpenTransactions()");

    // PTS 1134103 UH 2005-03-07
    if ( ! m_RedoManager.GetRedoTrafficControl()->GetTransactions(taskid, m_TransactionsToSkip, errlist) )
        return false;
            
    // PTS 1124684 UH 2003-10-16
    m_RedoManager.GetRedoTrafficControl()->EnableRedoProcessing(taskid);
    return true;
}
/* --------------------------------------------------------------------------- */
bool
Rst_RedoReadTask::GetOrCreateRedoTransaction (       tsp00_TaskId             taskid,
                                               const Log_QueueID              queueID,
                                                     SAPDBMem_IRawAllocator  &allocator,
                                                     tgg00_TransContext      &oldcontext,
                                               const tgg91_TransNo           &redoTransNo,
                                                     tgg00_TransIndex        &currTransIndex,
                                                     Log_Transaction*        &redoTrans,
                                                     SAPDBErr_MessageList    &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::GetOrCreateRedoTransaction", LogTrans_Trace, 5);

    redoTrans      = 0;
    currTransIndex = cgg_nil_transindex;
    
    kb51GetRedoTransEntry (taskid, redoTransNo, currTransIndex);

	while ( currTransIndex == cgg_nil_transindex )
    {
        if ( ! m_RedoManager.GetRedoTrafficControl()->CheckJobCount(taskid, errlist) ) // UH 2005-01-03 added to reserve some transentries
            return false;

        // The redo read task is registered in the locklist as waiting and the locklist will resume
        // the caller, but the redo reader calls a wait methode of RedoTrafficcontrol to wait there.
        // The RedoTrafficControl is able to detect deadlocks od waiting situations and can
        // wakeup suspended redo tasks.
        kb51CreateRedoTransEntry (taskid, redoTransNo, currTransIndex, oldcontext.trError_gg00);

        if ( m_redoIsAborted )
        {
            oldcontext.trError_gg00 = e_cancelled;
            Kernel_AppendBasisError(oldcontext.trError_gg00, errlist, "Rst_RedoReadTask::GetOrCreateRedoTransaction");
            return false;
        }

        if ( oldcontext.trError_gg00 == e_too_many_lock_requests )
            oldcontext.trError_gg00 = e_ok;

        if ( currTransIndex == cgg_nil_transindex
             &&
             oldcontext.trError_gg00 == e_ok )
        {
            if ( ! m_RedoManager.GetRedoTrafficControl()->WaitForFinishedRedoTransaction(taskid, errlist) )
                return false;
        }
        
        if ( oldcontext.trError_gg00 != e_ok )
        {
            Kernel_AppendBasisError(oldcontext.trError_gg00, errlist, "kb51CreateRedoTransEntry");
            errlist.AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_RST_REDO_READ_GET_TRANSENTRY_FAILED,
                                              Msg_Arg("TRANS_INDEX",SAPDB_ToString(currTransIndex)),
                                              Msg_Arg("TRANS_ID",SAPDB_ToString(redoTransNo.gg90GetInt4())) ));
            return false;
        }
    }

    redoTrans = reinterpret_cast<Log_Transaction*>(k50GetLogTransaction(currTransIndex));

    if ( 0 == redoTrans )
    {
        redoTrans = new (allocator) Log_Transaction ();
        if ( 0 == redoTrans )
        {
            errlist = Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                         "Could not allocate memory for new redo transaction.");
            return false;
        }

        k50RegisterTransaction (currTransIndex, redoTrans);

        if ( ! redoTrans->InitializeForRedoCopy(oldcontext,redoTransNo,queueID,allocator,errlist) )
        {
            errlist = errlist + Restart_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                                "Could not allocate memory for redo transaction. (InitializeForRedoCopy)");
            return false;
        }

        kb57RedoTransNoIncrement (taskid, redoTransNo);
    }

    return true;
}
/* --------------------------------------------------------------------------- */
bool Rst_RedoReadTask::StartCopyEntry (Log_Transaction        &transaction, // PTS 1115065 UH 2002-04-10 return bool
                                       Rst_LogEntryIterator   &iter,
                                       Rst_CopyState          &copyState,
                                       Log_AfterImage         &afterimage,
                                       SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::StartCopyEntry", LogTrans_Trace, 5);
    SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "StartCopyEntry: length:" << iter.RestLength() <<
                                           ", minimal: " << Log_ActionGetMinCopyLength(afterimage.GetActionType()));

    // If entryAlreadyCopied is true then nothing is to do - this is a redo of a redo.
    // This entry was already copied into redo file.
    const bool entryAlreadyCopied = transaction.GetLastCopiedRedoEntrySequence().IsValid()
                                    && (
                                    afterimage.GetSequence() < transaction.GetLastCopiedRedoEntrySequence()
                                    ||
                                    ( afterimage.GetSequence() == transaction.GetLastCopiedRedoEntrySequence()
                                      && 
                                      transaction.LastRedoEntryIsComplete()));

    copyState.SetMultipleAction(Log_MultipleActions == afterimage.GetActionType());
        
	errlist.ClearMessageList();

    Data_SplitSpaceWriter writer = entryAlreadyCopied
                                   ? Data_SplitSpaceWriter()   // PTS 1127732 UH 2004-02-11 use dummy writer
                                   : ! transaction.LastRedoEntryIsComplete() // PTS 1137832
                                        ? transaction.ReReserveSpaceForRedoEntryCopy(iter.RestLength(), errlist)
                                        : copyState.IsMultipleEntry()
                                        ? transaction.GetWriterForLocalRedoEntries
                                                        (iter.GetValuePtr() + Log_AfterImage::GetPersistentLength(),
                                                        iter.RestLength() - Log_AfterImage::GetPersistentLength(),
                                                        errlist)
                                        : transaction.ReserveSpaceForRedoEntryCopy
                                                    ( iter.RestLength(),
                                                      Log_ActionGetMinCopyLength (afterimage.GetActionType(), afterimage.GetPersistentVersion())
                                                      +
                                                      Log_AfterImage::GetPersistentLength(),
                                                      afterimage.GetSequence(),
                                                      iter.IOSeqNo(),
                                                      errlist );
    if ( ! errlist.IsEmpty() )
	{
        iter.AppendContextInfo (__CONTEXT__,
                                transaction.GetRedoTransNo().gg90GetInt4(),
                                afterimage.GetSequence(),
                                errlist);
        return false;
    }
    
    Data_SplitSpaceWriter::Result writeResult = Data_SplitSpaceWriter::ok;

    if ( ! entryAlreadyCopied ) // PTS 1127732 UH 2004-02-11
    {
        SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "StartCopyEntry: write:" << iter.Length());

        if ( ! writer.IsAssigned() )
        {
            const SAPDB_UInt reserveLength = Log_AfterImage::GetPersistentLength()
                                             +
                                             Log_ActionGetMinCopyLength(afterimage.GetActionType());
            errlist = Restart_Exception(__CONTEXT__, RST_COPY_ENTRY_RESERVE_FAILED,
                                        SAPDB_ToString(reserveLength));
            Kernel_VTrace() << "ReserveSpaceForRedoEntryCopy failed, Writer is not assigned.";
            copyState.WriteToTrace("copystate");                        
            iter.WriteToTrace("iterator");
            writer.WriteToTrace("writer");
            iter.AppendContextInfo (__CONTEXT__,
                                    transaction.GetRedoTransNo().gg90GetInt4(),
                                    afterimage.GetSequence(),
                                    errlist);
            transaction.AppendContextInfo ( __CONTEXT__, errlist );
            return false;
        }

        if (copyState.IsMultipleEntry())
        {
            const SAPDB_UInt firstTimeSkipLength = Log_AfterImage::GetHeaderSize()
                                                   +
                                                   Log_ActionMultiple::GetHeaderSize();
            if ( firstTimeSkipLength < iter.Length() )
            {
                // copy the entry without the header of the multiple entry
                SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "StartCopyEntry: multiple write: length = " << iter.Length() << " - " << firstTimeSkipLength);
                writeResult = writer.Write ( iter.GetValuePtr() + firstTimeSkipLength,
                                             iter.Length()      - firstTimeSkipLength );
            }
            else
            {
                // only the header of the multiple entry is in this part and the
                // header needs not to be copied
                SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "CopyEntry: NO WRITE firstTimeSkipLength:" \
                                                        << firstTimeSkipLength << " < iter.Length():" \
                                                        << iter.Length());
                writeResult = Data_SplitSpaceWriter::moreSpaceAvailable;
            }
        }
        else
        {
            SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "StartCopyEntry: simple write: length = " << iter.Length());
            writeResult = writer.Write (iter.GetValuePtr(), iter.Length());
        }
        
        if ( writeResult != Data_SplitSpaceWriter::moreSpaceAvailable
             &&
             writeResult != Data_SplitSpaceWriter::ok )
        {
            Kernel_VTrace() << "writeResult: " << writeResult;
            iter.WriteToTrace("iterator");
            writer.WriteToTrace("writer");
            errlist = Restart_Exception(__CONTEXT__, RST_WRITE_FAILED,
                                        SAPDB_ToString(writeResult));
            iter.AppendContextInfo (__CONTEXT__,
                                    transaction.GetRedoTransNo().gg90GetInt4(),
                                    afterimage.GetSequence(),
                                    errlist);
            writer.AppendContextInfo(__CONTEXT__,errlist);
            transaction.AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    
    const bool entryIsNotCompleteCopied =
                    iter.RestLength() > iter.Length()
                    &&
                    (writeResult == Data_SplitSpaceWriter::moreSpaceAvailable || entryAlreadyCopied);
        
    if ( entryIsNotCompleteCopied )
    {
        copyState.SetCopyPending(&transaction, writer, entryAlreadyCopied, afterimage.GetSequence());
                                  
        if ( ! copyState.IsMultipleEntry() && ! entryAlreadyCopied )
        // release ressource intermediately (PTS 1134170 mb 2005-03-11)
        {
            if ( ! transaction.GetRedoFile().Break(errlist) )
            {
                iter.AppendContextInfo (__CONTEXT__,
                                        transaction.GetRedoTransNo().gg90GetInt4(),
                                        afterimage.GetSequence(),
                                        errlist);
                writer.AppendContextInfo(__CONTEXT__,errlist);
                transaction.AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }            
    }
    else
    {
        return copyState.MaterializeEntryInRedoFile(iter.IOSeqNo(), transaction, writer, 
                                                    entryAlreadyCopied, copyState.IsMultipleEntry(), errlist);
    }
    return true;
}
/* --------------------------------------------------------------------------- */
bool Rst_RedoReadTask::ContinueCopyEntry (Rst_LogEntryIterator   &iter,
                                          Rst_CopyState          &copyState,
                                          SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::ContinueCopyEntry", LogTrans_Trace, 5);

    // If copyState.AlreadyCopied() is true then nothing is to do - this is a redo of a redo.
    // This entry was already copied into redo file.

    SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "ContinueCopyEntry: Restlength:" << iter.RestLength()) ;
    SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "ContinueCopyEntry: TransNo:" << copyState.GetTransaction().GetRedoTransNo().gg90GetInt4());

    errlist.ClearMessageList();

    bool callContinue = true;
    if (    ! copyState.GetTransaction().LastRedoEntryIsComplete()
         && ! copyState.GetWriter().HasSplitSpace() )
    // the last entry has to be completed, but no writer has been assigned yet
    {
        copyState.SetCopyPending( & copyState.GetTransaction(), 
                                    copyState.GetTransaction().ReReserveSpaceForRedoEntryCopy(iter.RestLength(), errlist),
                                    false,
                                    copyState.GetTransaction().GetLastCopiedRedoEntrySequence());
        callContinue = false;
    }

    if ( copyState.AlreadyCopied() )
    {
        if ( iter.RestLength() <= iter.Length() )
            copyState = Rst_CopyState();
        return true;
    }

    if ( ! copyState.IsMultipleEntry() && callContinue )
    {
        if ( ! copyState.GetTransaction().GetRedoFile().Continue(errlist) )
        {
            // get ressource again  (PTS 1134170 mb 2005-03-11)
            iter.AppendContextInfo (__CONTEXT__,
                                    copyState.GetTransaction().GetRedoTransNo().gg90GetInt4(),
                                    copyState.GetRedoEntrySequence(),
                                    errlist);
            copyState.GetWriter().AppendContextInfo(__CONTEXT__,errlist);
            copyState.GetTransaction().AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }

    Data_SplitSpaceWriter         &writer              = copyState.GetWriter();
    Data_SplitSpaceWriter::Result  writeResult         = Data_SplitSpaceWriter::ok;
    
    writer.UpdateAfterBreak(); // PTS 1135389 mb 2005-05-06

    SAPDBTRACE_WRITELN (LogTrans_Trace, 7, "ContinueCopyEntry: write:" << iter.Length());

    writeResult = writer.Write (iter.GetValuePtr(), iter.Length());
    
    if ( writeResult != Data_SplitSpaceWriter::moreSpaceAvailable
         &&
         writeResult != Data_SplitSpaceWriter::ok )
    {
        Kernel_VTrace() << "writeResult: " << writeResult;
        iter.WriteToTrace("iterator");
        writer.WriteToTrace("writer");
        errlist = Restart_Exception(__CONTEXT__, RST_WRITE_FAILED,
                                    SAPDB_ToString(writeResult));
        iter.AppendContextInfo (__CONTEXT__,
                                copyState.GetTransaction().GetRedoTransNo().gg90GetInt4(),
                                copyState.GetRedoEntrySequence(),
                                errlist);
        writer.AppendContextInfo(__CONTEXT__,errlist);
        copyState.GetTransaction().AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    const bool morePartsExpected = writeResult == Data_SplitSpaceWriter::moreSpaceAvailable
                                   &&
                                   iter.RestLength() > iter.Length();

    if ( ! morePartsExpected )
        return copyState.MaterializeEntryInRedoFile(iter.IOSeqNo(), errlist);

    // release ressource intermediately (PTS 1134170 mb 2005-03-11)
    if ( ! copyState.GetTransaction().GetRedoFile().Break(errlist))
    {
        iter.AppendContextInfo (__CONTEXT__,
                                copyState.GetTransaction().GetRedoTransNo().gg90GetInt4(),
                                copyState.GetRedoEntrySequence(),
                                errlist);
        writer.AppendContextInfo(__CONTEXT__,errlist);
        copyState.GetTransaction().AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    return true;
}
/* --------------------------------------------------------------------------- */
bool Rst_RedoReadTask::EndOfTransaction
        ( tsp00_TaskId                 redoReaderTaskid,
          SAPDBMem_IRawAllocator      &allocator,
          Log_Transaction*            &pTransaction,
          Log_Transaction::TransState  state,
          Log_IOSequenceNo             eotSequence,
          tgg00_TransIndex             transindex,
          bool                         checkAscendingSequence, // PTS 1113641 UH 2002-01-25
          SAPDBErr_MessageList        &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_RedoReadTask::EndOfTransaction", LogTrans_Trace, 5);

    pTransaction->SetEOTInRedo ( state, eotSequence );

    if ( LogTrans_Trace.TracesLevel(7) )
        pTransaction->WriteToTrace ("EndOfRedoTrans");

    // PTS 1114946 UH 2002-03-21 begin
    bool             transIsAlreadyInRedoList = false;
    Log_Transaction* pPublicTrans = reinterpret_cast<Log_Transaction*>(k50GetLogTransaction(transindex));

    if ( 0 == pPublicTrans )
    {
        errlist = Restart_Exception(__CONTEXT__, RST_EOT_HANDLING_NO_TRANSENTRY);
        pTransaction->AppendContextInfo ( __CONTEXT__, errlist );
        return false;
    }

    if ( pPublicTrans != pTransaction )
    {
        // there is another log transcontext registered in the public transaction entry in locklist
        // Check if this correct.
        if ( pTransaction->GetTransState()      != pPublicTrans->GetTransState()
             ||
             pTransaction->GetRedoEotSequence() != pPublicTrans->GetRedoEotSequence() )
        {
            errlist = Restart_Exception(__CONTEXT__, RST_EOT_HANDLING_WRONG_CONTEXT);
            pTransaction->AppendContextInfo ( __CONTEXT__, errlist );
            pPublicTrans->AppendContextInfo ( __CONTEXT__, errlist );
            return false;
        }
        transIsAlreadyInRedoList = true;
    }
    else
    {
        // 2004-02-18 UH added
        // This can happen if trans was open in last redo savepoint and last entry read was the commit.
        // So ReCreate has already inserted the transaction into the redo list and after that
        // the redo reader finds the commit.
        // In this case the transaction need not to be inserted.
        transIsAlreadyInRedoList =
            pPublicTrans->GetTransState() == Log_Transaction::illegalTransType;
    }
    // PTS 1114946 UH 2002-03-21 end
    
    bool           isOK    = true;
    Log_ActionType eottype = state ==  Log_Transaction::committed
                                       ? Log_Commit
                                       : Log_Rollback; // <- default

    if ( (eottype == Log_Commit
          &&
          ! pTransaction->EntriesToRedoExist())
         ||
         (eottype == Log_Rollback
          &&
          ! pTransaction->WasOpenInLastSavepoint()) )
    {
        tgg00_TransState dummyState;
        dummyState.clear();
        k51remove_locks ( redoReaderTaskid, transindex, dummyState );
        k51del_transentry_from_locklist ( redoReaderTaskid, transindex,  pTransaction->GetOldTransContext());
        if(!pTransaction->Delete(errlist, false))
            return false;
    }
    else
    {
        if ( ! pTransaction->DeleteAfterRedoCopy(errlist))
        {
            return false;
        }

        if ( ! transIsAlreadyInRedoList ) // PTS 1114946 UH 2002-03-21
        {
            if ( ! m_RedoManager.GetRedoTrafficControl()->
                        InsertJob ( redoReaderTaskid,
                                    transindex, eotSequence, eottype,
                                    m_OldestKnownEOTSequence, // PTS 1124684 UH 2003-10-16
                                    checkAscendingSequence,   // PTS 1113641 UH 2002-01-25
                                    *pTransaction,
                                    errlist ) )
            {
                errlist = errlist + Restart_Exception(__CONTEXT__, RST_EOT_HANDLING_INSERT_JOB_FAILED);
                pTransaction->AppendContextInfo ( __CONTEXT__, errlist );
                return false;
            }
        }
    }

    allocator.Deallocate(pTransaction);
    pTransaction = NULL;
    return true;
}


void Rst_RedoReadTask::WriteToTrace(const char * text, Container_Vector<Rst_CopyState> & copyState)
{
    Log_QueueID queueTmpId;
    for (queueTmpId = 0; queueTmpId < copyState.GetSize(); queueTmpId++)
    {
        copyState[queueTmpId].WriteToTrace(text);
    }
}


bool Rst_RedoReadTask::CheckCopyStateAllocation(
        Container_Vector<Rst_CopyState> & copyState,
        const Log_QueueID queueId )
{
    if ((Log_QueueID)copyState.GetSize() < (queueId+1))
    // react flexible, if the number of queues increases
    {
        Log_QueueID firstNewQueue = copyState.GetSize(); // PTS 1134144 mb 2005-02-28
        if ( ! copyState.Resize(queueId+1))
        {
            return false;
        }

        Log_QueueID queueTmpId;
        for (queueTmpId = firstNewQueue; queueTmpId < copyState.GetSize(); queueTmpId++)
        {
            copyState[queueTmpId].InitializeForRedo( ! m_lastSavepointDoneDuringRedo );
        }
    }
    return true;
}

