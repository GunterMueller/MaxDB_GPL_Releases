/*!
    @ingroup Restart
    @file    Rst_RedoReadTask.hpp
    @author  UweH
    @brief   defines the redo reader
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
#ifndef RST_REDO_READ_TASK_H
#define RST_REDO_READ_TASK_H

#include "Restart/Rst_Types.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "ggg00.h"
#include "gsp00.h"

class Rst_RedoManager;
class Rst_LogEntryIterator;
class Rst_CopyState;
/*!
    @brief A singleton to handle a task which reads the log and creates the redo files
    This current implementation handles one task only.
 */
class Rst_RedoReadTask
{
public:
    /*!
        @brief Constructor of the RedoReadTask
        @param logIter            from here the LogEntries shall be read
        @param redoManager        This is the calling object
        @param redoIsAborted      common flag for abort of the redo
    */
    Rst_RedoReadTask ( Rst_LogEntryIterator   &logIter,
                       Rst_RedoManager        &redoManager,
                       volatile bool          &redoIsAborted );
    ///  Destructor deletes substructure of given logIter
    ~Rst_RedoReadTask();
    enum ReadLogResult
    {
        RedoReadOk,
        RedoReadAborted,
        RedoReadReCreateOpenTransFailed,
        RedoReadCloseOpenTransFailed,
        RedoReadStartEntryNotFound,
        RedoReadError,
        RedoReadSavepointNotFound,
        RedoReadTransHandlingError,
        RedoReadSoftwareTooOld
    };
    /*!
        @brief Reads log entries from the archive log or the log save and creates redo files.

        = REDO READER TASK CODE =
        This is the main function of the redo-reader-task.
     */
    ReadLogResult ReadLogAndCreateRedoFiles (tsp00_TaskId          taskid,
                                             tgg00_TransContext   &oldcontext,
                                             SAPDBErr_MessageList &errlist);
private:
    /// result of InternReadLogAndCreateRedoFile()
    enum ReadLogInternResult
    {
        ReadLogOk,
        ReadLogAborted,
        ReadLogError,
        ReadLogSavepointNotFound,
        ReadLogRegisterTransactionFailed,
        ReadLogCloseOpenTransFailed,
        ReadLogDifferentEotFound,
        ReadLogReadActionError,
        ReadLogEndOfTransHandlingFailed,
        ReadLogSoftwareTooOld,
        ReadLogPartialRollbackError,
        ReadLogMaterializeError,
        ReadLogResourceShortage
    };
    /// defines the vector for describing the state of each log-queue during
    /// copying of the redoentries into the redofile
    typedef Container_Vector<Rst_CopyState> QueueCopyStates;
    /// All open redo transactions are rollbacked and removed.
    ReadLogInternResult InternReadLogAndCreateRedoFile
                                                (tsp00_TaskId             taskid,
                                                 SAPDBMem_IRawAllocator  &allocator,
                                                 tgg00_TransContext      &oldcontext,
                                                 Rst_LogEntryIterator    &iter,
                                                 QueueCopyStates         &copyState,
                                                 SAPDBErr_MessageList    &errlist);
	/// all open redo transactions are rollbacked and removed.
    bool CloseOpenTransactions (tsp00_TaskId                         taskid,
                                SAPDBMem_IRawAllocator              &allocator,
                                Log_IOSequenceNo                     lastIOSequence,
                                SAPDBErr_MessageList                &errlist);
    /*!
        @brief All open transactions of last savepoint are newly registered.

        This reads the open trans file identified by the root page no.
        The needed transentries in the locklist are created.
        The information about what savepoint must be used comes from the restartrecord.
        The transaction contexts are allocated with the given allocator.
        This allocator must not be used by the redo tasks itself.
        The should register their own allocator to work with their own context.
     */
    bool ReCreateOpenTransactions (tsp00_TaskId            taskid,
                                   tgg00_TransContext     &oldcontext,
                                   SAPDBMem_IRawAllocator &allocator,
                                   QueueCopyStates         &copyState,
                                   SAPDBErr_MessageList   &errlist);
	/// This gives the trans context to the given transno.
    /// If the transaction does not exist, it is created and registered.
    bool GetOrCreateRedoTransaction (       tsp00_TaskId             taskid,
                                      const Log_QueueID              queueID, // PTS 1137832 mb 2005-09-21
                                            SAPDBMem_IRawAllocator  &allocator,
                                            tgg00_TransContext      &oldcontext,
                                      const tgg91_TransNo           &redoTransNo,
                                            tgg00_TransIndex        &currTransIndex,
                                            Log_Transaction*        &redoTrans,
                                            SAPDBErr_MessageList    &errlist );
    /// copy the first (and maybe the only) part of a logentry into
    /// the corresponding Redofile. Returns true, if the entry could be
    /// successfully copied.
    /// (new for PTS 1130336 mb 2004-08-12)
    bool StartCopyEntry (Log_Transaction        &transaction, 
                         Rst_LogEntryIterator   &iter,
                         Rst_CopyState          &copyState,
                         Log_AfterImage         &afterimage,
                         SAPDBErr_MessageList   &errlist);
    /// copy the next part of a logentry into the corresponding Redofile.
    /// Returns true, if the entry could be successfully copied.
    /// (new for PTS 1130336 mb 2004-08-12)
    bool ContinueCopyEntry (Rst_LogEntryIterator   &iter,
                            Rst_CopyState          &copyState,
                            SAPDBErr_MessageList   &errlist);
    /// Handles the end of a redo transaction.
    bool EndOfTransaction ( tsp00_TaskId                 taskid,
                            SAPDBMem_IRawAllocator      &allocator,
                            Log_Transaction*            &pTransaction,
                            Log_Transaction::TransState  state,
                            Log_IOSequenceNo             eotSequence,
                            tgg00_TransIndex             transindex,
                            bool                         checkAscendingSequence, // PTS 1113641 UH 2002-01-25
                            SAPDBErr_MessageList        &errlist ); 
    /// Releases all entries, which are partially copied
    bool ReleaseAllPendingEntries ( Container_Vector<Rst_CopyState>  &iterState, 
                                    SAPDBErr_MessageList             &errlist);
                                    
    /// Writes the most interesting parts of the redofile into the kernel-trace
    void WriteToTrace(const char * text, Container_Vector<Rst_CopyState> & iterState);

    /// Asserts that an entry for the given queue is allocated. If this can not
    /// be asserted, the the method returns false.
    bool CheckCopyStateAllocation(Container_Vector<Rst_CopyState> & copyState,const Log_QueueID queueId );


private:
    /// This is a reference to the redo coordinator.
    Rst_RedoManager        &m_RedoManager;
    /// reference to the iterator for reading log-entries
    Rst_LogEntryIterator   &m_logIter;
    /// reference to a flag, which indicates an abort of the Redo
    volatile bool          &m_redoIsAborted;
    /// This holds the oldest eot sequence read from open trans file from beginning savepoint.
    /// in ReCreateOpenTransactions()
    Log_IOSequenceNo        m_OldestKnownEOTSequence; // PTS 1124684 UH 2003-10-16
    /// This is a list of transno's which are already processed by a previous restart and must be ignored by the reader
    Rst_Transactions        m_TransactionsToSkip; // PTS 1134103 UH 2005-03-07
    /// true, if the savepoint which bases the recovery on is a redosavepoint
    bool                    m_lastSavepointDoneDuringRedo;
};
#endif  /* RST_REDO_READ_TASK_H */
