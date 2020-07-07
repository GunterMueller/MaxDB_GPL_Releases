/*!
    @file           Log_Transaction.hpp
    @author         UweH
    @ingroup        Logging
    @brief          This is the interface between the transaction and the logging component.

    Log_Transaction is a class which holds all information for a write
    transaction to do a rollback or rollforward.
    It is generated at write trans begin and destroyed at the end of
    commit or rollback.
    The transentry in the KB-locklist has pointer of this object.
    The information can made persistent in a savepoint file, which
    holds all open transactions at savepoint time.

\if EMIT_LICENCE
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

\endif
*/
#ifndef LOG_TRANSACTION_HPP
#define LOG_TRANSACTION_HPP

#include "ggg00.h"
#include "ggg92.h"

#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_SplitSpaceWriter.hpp"
#include "DataAccess/Data_SplitSpaceReader.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"
#include "Container/Container_Stack.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_UndoFile.hpp"
#include "Logging/Log_ActionMultiple.hpp"
#include "Restart/Rst_RedoFile.hpp"

class SAPDBMem_IRawAllocator;
class Log_Queue;
class Log_BeforeImage;
class Log_AfterImageSpace;
class Log_IAction;

/// constant for "no subtransaction"
#define LOG_NIL_SUBTRANS_NO                 0

/// constant for initializing Undo/Redo-sequence-numbers
#define LOG_FIRST_ENTRY_SEQUENCE            0

/// default size for the allocation of the subtransaction stack
#define LOG_DEFAULT_SUBTRANS_STACK_CAPACITY 100

/// defines a transaction

class Log_Transaction
{

/// the OpenTransfile needs access to the transaction-internals (i.e. persistent structure)
friend class Log_OpenTransFile;

public:

    /// context, in which the transaction is running
    enum TransType
    {
        online,             ///< in a normal user context at online time.
        redoRead,           ///< by the redo reader task to rebuild the transaction.
        redoExecute,        ///< by a redo executen task to really redo or rollback the transaction.
        illegalTransType    ///< illegal value
    };

    /// state of the transaction
    enum TransState
    {
        opened,             ///< the transaction is currently open
        committed,          ///< the transaction is commited
        rollbacked,         ///< the transaction is rollbacked
        illegalTransState   ///< the transaction has an illegal state
    };

    /// the reason for a rollback
    enum RollbackKind
    {
        endOfTrans,             ///< rollback of transaction
        command,                ///< rollback of kb05-command (useful only <= 7.3)
        subTrans,               ///< rollback of a subtransaction
        illegalRollbackKind     ///< illegal value
    };

    /// the result of operations, which write to the logVolume
    enum LogState
    {
        noError,        ///< everthing's fine
        logFull,        ///< the log is full
        logError        ///< write-error on the logVolume
    };

public:

    /// This is used to create a new write transaction.
    Log_Transaction ()
    : m_pTrans        (NULL),
      m_pAllocator    (NULL),
      m_MaxSubtransNo (LOG_NIL_SUBTRANS_NO),
      m_pSubtransStack(NULL),
      m_pUndoFile     (NULL),
      m_pRedoFile     (NULL),
      m_pQueue        (NULL),
      m_TransType     (online),
      m_WasOpenInLastSavepoint (false)
    {}
    
    /// returns true, if initialized
    bool IsInitialized();
    
    /// returns true, if no allocation failure.
    bool Initialize (tgg00_TransContext   &Trans,
                     SAPDBErr_MessageList &errlist);
    
    /// this deallocates alle resources allocated by Initialize().
    bool Delete (SAPDBErr_MessageList &errlist,
                 bool                  checkLocalRedoLogBuffer = true);
    
    /// returns true, if initialized
    bool IsInitializedForRedoCopy();
    
    /// returns true, if no allocation failure.
    bool InitializeForRedoCopy (      tgg00_TransContext     &redoReaderTrans,
                                const tgg91_TransNo          &transNo,
                                const Log_QueueID             queueID, // PTS 1137832 mb 2005-09-21
                                      SAPDBMem_IRawAllocator &allocator,
                                      SAPDBErr_MessageList   &errlist);
    
    /// this deallocates alle resources allocated by InitializeForRedoCopy().
    bool DeleteAfterRedoCopy (Msg_List &errlist);
    
    /// true, if initialized
    bool IsInitializedForRedo();
    
    /// returns true, if no allocation failure.
    bool InitializeForRedo (tgg00_TransContext &redoExecuteTrans);
    
    /// this deallocates alle resources allocated by InitializeForRedo().
    bool DeleteAfterRedo (SAPDBErr_MessageList &errlist);

    /// the pointers are not assigned.
    /// @attention this opertor MUST not copy the spinlock, because the RTE must allocate
    /// a new spinlock during undo/redo. So in effect this copy-operator is only neccessary
    /// for EXCLUDING members from copying
    Log_Transaction& CopyFromRedoReaderToRedoJobList (const Log_Transaction& srcTrans);
    
    /// writes information about this transaction into the trace.
    void WriteToTrace (const char *Title) const;

    /// appends context information to the given messagelist
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            SAPDBErr_MessageList      &errlist,
                            SAPDB_Char  const * const  title = 0) const;
    
    /// returns the allocator used by this transaction
    SAPDBMem_IRawAllocator& Allocator() const
    {
        SAPDBERR_ASSERT_STATE (m_pAllocator != NULL);
        return *m_pAllocator;
    }
    
    /// used by redo to get the open trans info.
    const tgg91_TransNo& GetRedoTransNo() const
    {
        return m_Context.TransNo;
    }
    
    /// used by redo to synchronize a already finished transaction.
    Log_IOSequenceNo GetRedoEotSequence() const
    {
        return m_Context.EOT_IOSequenceNo;
    }
    
    /// used by redo reader to not copy entries, already copied
    Log_EntrySequence GetLastCopiedRedoEntrySequence() const
    {
        return m_Context.LastCopiedRedoEntrySequence;
    }

    /// returns the ID of the queue the transactions has been assigned to during online processing
    /// (new with PTS 1137832)
    Log_QueueID GetQueueID() const
    {
        return m_Context.QueueID;
    }

    /// returns one of: open, committed, rollbacked
    TransState GetTransState() const
    {
        if ( m_Context.TransStatePersistent <  illegalTransState )
            return TransState(m_Context.TransStatePersistent);
        else
            return illegalTransState;
    }
    
    /// returns one of: online, redoRead, redoExecute
    TransType GetTransType() const
    {
        return m_TransType;
    }
    
    /// returns the oldest known iosequence from the log volume
    Log_IOSequenceNo GetOldestKnownIOSequence() const
    {
        // PTS 1127009 UH 2004-01-09 new
        return m_OldestKnownIOSequence;
    }
    
    /// returns a reference to the redofile. This must only be called, if a redofile
    /// acually exists (e.g. during copying of logentries) (PTS 1134117 mb 2005-03-22 *)
    Rst_RedoFile & GetRedoFile()
    {
        SAPDBERR_ASSERT_STATE(m_pRedoFile != NULL);
        
        return *m_pRedoFile;
    }
    
    bool IsOpenInRedo() const
    {
      	// PTS 1114125 UH 2002-02-12 new
        return m_TransType == redoRead
               &&
               GetRedoEotSequence().IsInvalid();
    }

    /*!
       @brief This begins a sub transaction.

       A sub transaction can be separately rollbacked.
       trSubtransNo becomes a new Subtransno.
    */
    void BeginSubtrans();

    /*!
       @return false, if subtrans begin is missing.
       @brief  This ends a sub transaction.
     */
    bool EndSubtrans();

    /// This TransState becomes committed. And must be open before.
    bool Commit (SAPDBErr_MessageList &errlist);

    /// the result of a redo-operation
    enum RedoResult
    {
        redoOk,                 ///< no error
        redoReadError,          ///< error while reading an entry
        redoWrongEntry,         ///< an error in an entry had been detected
        redoExecuteError,       ///< an entry could not executed
        redoEndOfTransFailed,   ///< EndOfTransHandling() failed
        redoAborted,            ///< The redo was aborted
        RedoResult_SIZE
    };
    
    /// for output of the RollbackResult
    static const char *RedoResultString[RedoResult_SIZE];

    /// The transaction is redone by a redo task.
    RedoResult Redo(SAPDBErr_MessageList &errlist);

    /// the result of an rollback
    enum RollbackResult
    {
        undoOk,                 ///< no error
        undoNoOpenSubtrans,     ///< there was no open subtransaction
        undoReadError,          ///< error while reading entries
        undoWrongEntry,         ///< an error in an entry had been detected
        undoExecuteError,       ///< error while undoing changes
        undoWriteError,         ///< error while writing
        undoLogFull,            ///< the log is full
        undoInitNotCorrect,     ///< not correct initialization
        undoTempRollbackFailed, ///< temp rollback failed
        undoEndOfTransFailed,   ///< eot handling failed
        RollbackResult_SIZE
    };

    /// for output of the RollbackResult
    static const char *RollbackResultString[RollbackResult_SIZE];

    /*!
       @brief          The TransState becomes rollbacked and must be open before.
       @param          rollbackKind [in] - endOfTrans,command,subTrans
       @param          SubtransNo [in] if LOG_NIL_SUBTRANS_NO, then the complete transaction is rollbacked
       @param          undoStopSequence [in] - only used during redo
       @param          errlist Contains error information upon completion
       @return         RollbackResult
     */
    RollbackResult Rollback (RollbackKind          rollbackKind,
                             tgg00_SubtransNo      SubtransNo,
                             Log_EntrySequence     undoStopSequence,
                             SAPDBErr_MessageList &errlist); // PTS 1113230 UH 2001-12-20
    /// returns true if the trans is currently running in rollback
    bool IsRollback() const
    {
        return m_pTrans == 0 ? false : m_pTrans->trState_gg00.includes(tsRollback_egg00);
    }
    /// The given BeforeImage is stored for rollback purpose.
    Log_UndoFile::WriteBeforeImageResult
         WriteUndoEntry (      Log_BeforeImage &BeforeImage, // PTS 1121659 UH 2003-04-30
                         const Log_IAction     &Action,
                         Data_PageNo           &pageno,
                         Data_PageOffset       &offset,
                         SAPDBErr_MessageList  &errlist);
    
    /// The given AfterImage is stored for redo purpose.
    LogState WriteRedoEntry (const Log_IAction    &Action,
                             SAPDBErr_MessageList &errlist);
    
    /// The space is reserved and must be released after copy.
    /// If the file does not exist, it is created.
    Data_SplitSpaceWriter ReserveSpaceForRedoEntryCopy
        (Data_SplitRecordLength Length,
         Data_RecordLength      MinimalLength,
         Log_EntrySequence      EntrySequence,
         Log_IOSequenceNo       IOSequence,
         SAPDBErr_MessageList  &errlist);
    
    /// An already reserved space (e.g. written in a redosavepoint) 
    /// is used to build up a writer for continuing the copying of
    /// the redoentry
    Data_SplitSpaceWriter ReReserveSpaceForRedoEntryCopy
        (Data_SplitRecordLength CurrentRestlength,
         SAPDBErr_MessageList  &errlist);

    /// Ressources of the complete copied entry are released.
    bool FinishCopiedRedoEntry(SAPDBErr_MessageList &errlist);
    
    /// The RedoFile is released without changes
    bool ReleaseIncompleteRedoEntry( SAPDBErr_MessageList &errlist );
    
    /// All resources for copying are released.
    bool ReleaseSpaceAfterRedoEntryCopy(bool                  undoChanges, 
                                        SAPDBErr_MessageList &errlist);

    ///  redo file is truncated and if necessary the UndoFile is partially rollbacked.
    bool RedoPartialRollback (Log_EntrySequence     UndoStopSequence,
                              Log_EntrySequence     RedoStopSequence,
                              SAPDBErr_MessageList &errlist);
    
    /// if during redo a transactions end was found. it set here,
    /// so a following savepoint can use this on redo/redo.
    void SetEOTInRedo (TransState        eotState,
                       Log_IOSequenceNo  eotSequence);
    
    /// During copy of redoentries the copying may be interrupted. This has to be
    /// announced to the transaction
    void SetLastRedoEntryIncomplete()
    {
        m_Context.LastRedoEntryIsComplete = SAPDB_FALSE;
    }
    
    /// During copy of redoentries the copying may be interrupted. If the entry is complete
    /// copied this has to be announced to the transaction.
    void SetLastRedoEntryComplete()
    {
        m_Context.LastRedoEntryIsComplete = SAPDB_TRUE;
    }
    
    /// returns true if the last copied entry in the redofile is complete
    bool LastRedoEntryIsComplete()
    {
        return (SAPDB_TRUE == m_Context.LastRedoEntryIsComplete);
    }
    
    /// if during redo a transactions end was found. it set here,
    /// so a following savepoint can use this on redo/redo.
    /// @returns       true, if trans was recreated from last savepoint.
    bool WasOpenInLastSavepoint()
    {
        return m_WasOpenInLastSavepoint;
    }

    /// true, if this transaction needs to be written during savepoint
    bool IsSavepointRelevant();
    
    /// used for writing the savepoint file
    static SAPDB_UInt GetPersistentLength()
    {
        return sizeof (Persistent);
    }
    
    /// used for writing the savepoint file
    static SAPDB_UInt GetOldPersistentLength()
    {
        return sizeof (OldPersistentLayout);
    }
    
    /// used for writing the savepoint file
    void WritePersistentFormat(SAPDB_Byte* Space,
                               SAPDB_UInt  Length);
    
    /// Used for recreating a transaction out of a savepoint file.
    void ReadPersistentFormat(const SAPDB_Byte* Space,
                              const SAPDB_UInt        Length);

    /// this can be used by the locklist to dump with the transentry.
    static void GetDumpInfo (Log_Transaction *pTrans,
                             void*           &pData,
                             SAPDB_UInt4     &dataLength)
    {
        if ( pTrans == NULL )
        {
            pData      = NULL;
            dataLength = 0;
            return;
        }
        pData      = &(pTrans->m_Context);
        dataLength = sizeof(pTrans->m_Context);
    }
    
    /// returns true, if any undo was written by the transaction.
    bool UndoEntriesExist() const
    {
        return m_Context.LastUndoEntrySequence.IsValid();
    }
    
    /// returns true, if any undo was written by the transaction.
    SAPDB_UInt GetUndoEntryCount() const
    {
        return UndoEntriesExist() ? m_Context.LastUndoEntrySequence + 1 : 0;
    }
    
    /// returns true, if any redo was written by the transaction.
    bool RedoEntriesExist() const
    {
        return m_Context.LastRedoEntrySequence.IsValid();
    }
    
    /// returns true, if any redo entry was copied for redo by the transaction.
    bool EntriesToRedoExist() const
    {
        return m_Context.LastCopiedRedoEntrySequence.IsValid();
    }

    /// updates undo/redo information from the Undo/Redo-files
    /*!
    @param  oldestKnownIOSeq the oldestKnownIOSequence of this transaction
    @param  errlist error list
    @return false on any error
    */
    bool UpdateUndoRedoInfoForRestart(const Log_IOSequenceNo  oldestKnownIOSeq,
                                      SAPDBErr_MessageList   &errlist);

    /// returns information about the transaction
    bool GetInfo ( Data_PageNo          &undoroot,
                   Data_PageNo          &undolast,
                   SAPDB_UInt           &undopagecount,
                   Log_EntrySequence    &lastundosequence,
                   SAPDBErr_MessageList &errlist )
    {
        // PTS 1115468 UH 2002-05-13 new
        // MUST BE CALLED IN TRANS REGION FOR THIS TRANSACTION
        undoroot         = m_Context.UndoFileRootPageNo;
        undolast         = m_Context.UndoFileLastPageNo;
        lastundosequence = m_Context.LastUndoEntrySequence;
        if ( 0 == m_pUndoFile
             ||
             ! m_pUndoFile->IsCreated() )
        {
            undopagecount = 0;
            return true;
        }
        undopagecount = m_pUndoFile->GetPageCount();
        return true;
    }
    
    /// this must be used by old interfaces
    tgg00_TransContext& GetOldTransContext()
    {
        return *m_pTrans;
    }

    /// During redo copy the local entries are restored and distributed to the redo file
    bool RestoreLocalRedoEntries(const Log_IOSequenceNo      iosequence,
                                       SAPDBErr_MessageList &errlist);
	/// returns a split space writer which works on the local redo entry buffer
    Data_SplitSpaceWriter GetWriterForLocalRedoEntries(SAPDB_Byte           *pActionHead,
                                                       SAPDB_UInt            sizeToReserve,
                                                       SAPDBErr_MessageList &errlist);
	/// This resets the internal redo entries and empties the buffer
    void ResetLocalRedoEntries()
    {
        m_LocalRedoEntries.Reset();
    }
    /// This reads the paramter LOCAL_REDO_LOG_BUFFER_SIZE and sets local variable
    static void DetermineLocalRedoBufferSize();

    /// This returns true, if a local redobuffer is used (PTS 1137832)
    static SAPDB_UInt GetLocalRedoBufferSize();
private: 

    /// sets the state of the transaction
    void SetTransState (TransState State)
    {
        m_Context.TransStatePersistent = State;
    }
    
    /// ends a transaction
    bool EndOfTransHandling (TransState            State,
                             SAPDBErr_MessageList &errlist);

    /// pops the Subtransstack until a specified is found
    tgg00_SubtransNo PopUntilSubtransFound (tgg00_SubtransNo   SubtransNo,
                                            Log_EntrySequence &UndoStopSequence,
                                            Log_EntrySequence &RedoStopSequence);

    /// writes the endOfTransEntry on the log
    bool WriteEndOfTransEntry (TransState            State,
                               SAPDBErr_MessageList &errlist);

    /// creates an undofile for this transaction
    bool CreateUndoFile(SAPDBErr_MessageList &errlist);

    /// drops the undofile for this transaction
    bool DropUndoFile(SAPDBErr_MessageList &errlist);

    /// creates an redoFile for this transaction
    bool CreateRedoFile(tgg00_TransContext   &redoReaderTrans,
                        SAPDBErr_MessageList &errlist);

    /// drops the redofile
    bool DropRedoFile(SAPDBErr_MessageList &errlist);

    /// the undoFile is unlinked from this transaction and inserted into the history
    bool GiveUndoFileToHistory(SAPDBErr_MessageList &errlist);

// protected: NOT ALLOWED BECAUSE OF TH USAGE OF "Persistent(const OldPersistentLayout oldLayout)" UH 08-02-2006
public:

    /// description of a subtransaction
    struct SubtransStackElem
    {
        tgg00_SubtransNo  SubtransNo;       ///< number of the subtrans (4 byte)
        Log_EntrySequence UndoStopSequence; ///< sequence where an undo shall stop (4 byte)
        Log_EntrySequence RedoStopSequence; ///< sequence where an redo shall end (4 byte)
    };

    /// defines a stack of sub-transactions
    typedef Container_Stack<SubtransStackElem> SubtransStack;

    /// defines the old entrylayout for migration purposes
    struct OldPersistentLayout
    {
        tgg91_TransNo     TransNo;                      ///< 6  byte
        SAPDB_Byte        TransStatePersistent;         ///< 1  byte
        SAPDB_Bool1       Filler;                       ///< 1  byte
        Log_EntrySequence LastUndoEntrySequence;        ///< 4  byte
        Log_EntrySequence LastRedoEntrySequence;        ///< 4  byte
        Log_EntrySequence LastCopiedRedoEntrySequence;  ///< 4  byte
        SAPDB_Int4        UndoFileRootPageNo;           ///< 4  byte
        SAPDB_Int4        UndoFileLastPageNo;           ///< 4  byte
        SAPDB_Int4        RedoFileRootPageNo;           ///< 4  byte
        SAPDB_Int4        RedoFileLastPageNo;           ///< 4  byte
        Log_IOSequenceNo  EOT_IOSequenceNo;             ///< 4  bytes
    };

    /// defines the persistent structure of a transEntry in the opentransfile
    /// (40 bytes)
    struct Persistent
    {
        tgg91_TransNo     TransNo;                      ///< 6  byte
        SAPDB_Byte        TransStatePersistent;         ///< 1  byte
        SAPDB_Bool1       Filler1;                      ///< 1  byte
        Log_EntrySequence LastUndoEntrySequence;        ///< 4  byte
        Log_EntrySequence LastRedoEntrySequence;        ///< 4  byte
        Log_EntrySequence LastCopiedRedoEntrySequence;  ///< 4  byte
        SAPDB_Int4        UndoFileRootPageNo;           ///< 4  byte
        SAPDB_Int4        UndoFileLastPageNo;           ///< 4  byte
        SAPDB_Int4        RedoFileRootPageNo;           ///< 4  byte
        SAPDB_Int4        RedoFileLastPageNo;           ///< 4  byte
        Log_IOSequenceNo  EOT_IOSequenceNo;             ///< 4  bytes
        Log_QueueID       QueueID;                      ///< 2  bytes
        SAPDB_Bool1       LastRedoEntryIsComplete;      ///< 1  byte;
        SAPDB_Bool1       Filler2;                      ///< 1  byte;
                                                        
        /// constructor
        Persistent()
        {
            init();
        }
        
        /// converts an old layout into new the layout
        Persistent(const OldPersistentLayout oldLayout)
        {

            // copy old parts of the layout
            TransNo                         = oldLayout.TransNo;
            TransStatePersistent            = oldLayout.TransStatePersistent;
            Filler1                         = oldLayout.Filler;
            LastUndoEntrySequence           = oldLayout.LastUndoEntrySequence;
            LastRedoEntrySequence           = oldLayout.LastRedoEntrySequence;
            LastCopiedRedoEntrySequence     = oldLayout.LastCopiedRedoEntrySequence;
            UndoFileRootPageNo              = oldLayout.UndoFileRootPageNo;
            UndoFileLastPageNo              = oldLayout.UndoFileLastPageNo;
            RedoFileRootPageNo              = oldLayout.RedoFileRootPageNo;
            RedoFileLastPageNo              = oldLayout.RedoFileLastPageNo;
            EOT_IOSequenceNo                = oldLayout.EOT_IOSequenceNo;
            // initialize new parts of the layout
            QueueID.Invalidate();
            LastRedoEntryIsComplete         = SAPDB_TRUE;
            Filler2                         = 0;

            Kernel_VTrace() << "Created Transaction from old layout " << TransNo.gg90GetInt4();
        }
        
        /// initializer
        void init()
        {
            TransNo.gg90SetNil();
            TransStatePersistent        = 0;
            Filler1                     = 0;
            LastUndoEntrySequence.Invalidate();
            LastRedoEntrySequence.Invalidate();
            LastCopiedRedoEntrySequence.Invalidate();
            UndoFileRootPageNo    = SAPDB_MAX_INT4;
            UndoFileLastPageNo    = SAPDB_MAX_INT4;
            RedoFileRootPageNo    = SAPDB_MAX_INT4;
            RedoFileLastPageNo    = SAPDB_MAX_INT4;
            EOT_IOSequenceNo.Invalidate();
            QueueID.Invalidate();
            LastRedoEntryIsComplete = SAPDB_TRUE;
            Filler2                 = 0;
        }
    };

private: // member

    /// the old pascal-transaction context
    tgg00_TransContext*    m_pTrans;

    /// the allocator, used for dynamically allocating memory
    SAPDBMem_IRawAllocator* m_pAllocator;

    /// the persistent context
    Persistent              m_Context;

    /// maximal used subtrans-number up to now 
    tgg00_SubtransNo        m_MaxSubtransNo;

    /// the stack of currently open subtransactions
    SubtransStack          *m_pSubtransStack;

    /// currently used undo-file
    Log_UndoFile           *m_pUndoFile;

    /// currently used redo-file
    Rst_RedoFile           *m_pRedoFile;

    /// the queue used for writing something onto the LogVolume
    Log_Queue              *m_pQueue;

    /// the transactiontype 
    TransType               m_TransType;

    /// true, if this transaction was open in the restart-savepoint
    bool                    m_WasOpenInLastSavepoint;
    
    /// oldest known io sequence, which is used to correctly sort during redo
    Log_IOSequenceNo        m_OldestKnownIOSequence; // PTS 1127009 UH 2004-01-09 added

    /// This synchronises the access to the persistent context 
    /// @attention this MUST not copied in the copy-operator, because the RTE must allocate
    /// a new spinlock for undo/redo
    mutable RTESync_Spinlock m_PersistentAccess;

    /// redo entries which are written as a block to the redo log
    Log_ActionMultiple m_LocalRedoEntries; // PTS 1131107 UH 2004-08-24 added
};

// PTS 1117320 UH 2002-10-28 former Log_NoAction
/*! --------------------------------------------------------------------------------
class:        Log_ActionEndOfTransaction
*/
class Log_ActionEndOfTransaction : public Log_IAction
{

public:

    /// constructor
    Log_ActionEndOfTransaction(Log_ActionType type)
    :Log_IAction(type)
    {}
    
    virtual bool WaitForLogWriting () const
    {
        return true;
    }
    
    virtual bool ForceLogWriting () const
    {
        return true;
    }
    
    /// initializes the transaction with its commit-timestamp
    void Initialize(const Kernel_DateTime timeStamp)
    {
        m_commitTimestamp.CommitDate = timeStamp.GetDate();
        m_commitTimestamp.CommitTime = timeStamp.GetTime();
    }
    
    virtual SAPDB_UInt GetMinimalPersistentLength() const
    { 
        return GetPersistentLength();
    }
    
    virtual SAPDB_UInt GetPersistentLength() const
    {
        return sizeof(CommitTimestamp);
    }
    
    virtual bool       WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                              SAPDBErr_MessageList  &errlist) const
    {
        CommitTimestamp                       *timestamp;
        const Data_SplitSpaceWriter::Result    result =
            Writer.Reserve (sizeof(CommitTimestamp), reinterpret_cast<SAPDB_Byte*&>(timestamp));
        if ( Data_SplitSpaceWriter::ok != result )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "WriteResult is ok");
            Writer.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        *timestamp = m_commitTimestamp;
        return true;
    }
    
    virtual bool       ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                             SAPDBMem_IRawAllocator &Allocator,
                                             SAPDBErr_MessageList   &errlist)
    {
        CommitTimestamp                       *timestamp;
        const Data_SplitSpaceReader::Result    result = 
            Reader.Reserve (sizeof(CommitTimestamp), reinterpret_cast<SAPDB_Byte*&>(timestamp));
        if ( Data_SplitSpaceReader::ok != result )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "ReadResult is ok");
            Reader.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        m_commitTimestamp = *timestamp;
        return true;
    }
    
    virtual void       Execute (tgg00_TransContext &TransContext) const
    {}
    
    virtual void       WriteToTrace (const char * Title) const
    {
        if ( Title != NULL )
        {
            Kernel_VTrace() << Title << "commit timestamp: " << GetCommitDate()<< " " << GetCommitTime();
        }
        else
        {
            Kernel_VTrace()          << "commit timestamp: " << GetCommitDate()<< " " << GetCommitTime();
        }
    }

    /// if this returns false a local redo entry cash must be flushed
    virtual bool LocalRedoEntryCachingPossible() const
    {
        return false;
    }

    /// reads the date-part commit-timestamp
    const Kernel_Date GetCommitDate() const
    {
        return m_commitTimestamp.CommitDate;
    }

    /// reads the time-part commit-timestamp
    const Kernel_Time GetCommitTime() const
    {
        return m_commitTimestamp.CommitTime;
    }

private:

    /// pysical layout of the LogEntry in the undo/redo-file
    typedef struct
    {
        Kernel_Date CommitDate; ///< Date when the commit happened
        Kernel_Time CommitTime; ///< Time when the commit happened
    } CommitTimestamp;
    
    /// in memory representation of the partial rollback
    CommitTimestamp m_commitTimestamp;
    
};
/*! endclass: Log_ActionEndOfTransaction */


#endif  /* LOG_TRANSACTION_HPP */
