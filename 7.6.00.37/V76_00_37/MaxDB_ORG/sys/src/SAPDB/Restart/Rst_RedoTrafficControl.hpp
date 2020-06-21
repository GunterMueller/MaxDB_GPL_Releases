/*!
    @ingroup        Restart
    @file           Rst_RedoTrafficControl.hpp
    @author         UweH
    @brief          defines singleton class RedoTrafficControl
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
#ifndef RST_REDO_TRAFFIC_CONTROL_H
#define RST_REDO_TRAFFIC_CONTROL_H

#include "Restart/Rst_IRedoTrafficControl.hpp"
#include "Restart/Rst_IoSeqList.hpp"

/*!
   @class  Rst_RedoTrafficControl
   @brief  lists to synchronize the parallel redo (singleton)
 */
class Rst_RedoTrafficControl: public Rst_IRedoTrafficControl
{
public:
    /// initialization
    Rst_RedoTrafficControl (SAPDBMem_IRawAllocator &RawAllocator,
                            volatile bool          &redoIsAborted,
                            SAPDB_UInt             &inserted,
                            SAPDB_UInt             &removed);
    /*!
        @brief allocates the I/O sequence container and the job container

        The transactions of the database are limited by NumberOfTransEntries.
        if every transaction is closed at a different LogPage, so everey transaction
        has a different IoSeq. Therefore the IoSeqContainer has NumberOfTransEntries
        elements. The TransIndex has a range 1..NumberOfTransEntries. To avoid
        arithmetic expressions when accessing the JobContainer vector, the vector
        consists of NumberOfTransEntries+1 elements (0 .. n+1).
     */
    bool Initialize ();
	/// deallocation of containers
    ~Rst_RedoTrafficControl();
	/// returns the allocator used to allocate this singleton
    SAPDBMem_IRawAllocator& GetRawAllocatorUsed ()
    {
        return m_RawAllocator;
    }
    /// returns the number of redo tasks
    SAPDB_UInt PossibleNumberOfRedoTransactions() const
    {
        return m_NumberRedoTransactions;
    }
    void SetNumberOfRedoTasks(tsp00_TaskId taskid,
                              SAPDB_UInt   startedRedoExecutionTasks);
    /// This is called by a finished transaction to resume in case of redo a waiting redo reader
    void TryToResumeRedoReader(tsp00_TaskId taskid,
                               bool         alreadyInRegion = false);
    /// This is called by the redo reader to prevent that all transentries become used
    /// The caller waits if g01maxuser jobs were already inserted to the redo list.
    virtual bool CheckJobCount (tsp00_TaskId          taskid,
                                SAPDBErr_MessageList &errlist); // UH 2005-01-03
    /// This is called by the RedoReader if no more transentries are available
    /// The caller is suspended until one RedoTask is finished.
    virtual bool WaitForFinishedRedoTransaction (tsp00_TaskId          taskid,
                                                 SAPDBErr_MessageList &errlist);
	/// Inserts a job. This method is provided for the log reader task.
    virtual bool InsertJob (tsp00_TaskId          LogReaderTaskId,
                            tgg00_TransIndex      TransIndex,
                            Log_IOSequenceNo      EndOfTransIoSeq,
                            Log_ActionType        UndoOrRedo,
                            Log_IOSequenceNo      oldestKnownEOTSequence, // PTS 1124684 UH 2003-10-16
                            bool                  checkAscendingSequence, // PTS 1113641 UH 2002-01-25
                            Log_Transaction      &Transaction,
                            SAPDBErr_MessageList &errlist);
	/// executes vacant jobs. This method is provided for redo tasks.
    virtual bool ExecuteJobs (tsp00_TaskId          taskid,
                              tgg00_TransContext   &oldcontext,
                              SAPDBErr_MessageList &errlist);
	/// the log reader task sets the state: end of log read
    virtual void SetEndOfLogRead (tsp00_TaskId LogReaderTaskId);
    /*!
        @brief  a redo task checks if preceding transactions are closed
        @param  RedoTaskId [in] 
        @param  WaitContext [in/out] 
        @param  IoSeq [in] sequence of the log entry handled by the redo task
        @param  waitInserted [out] if true the caller has to wait
        @param  errlist [out] error messages
        @return CheckIoSeqResult on any error
     */
    virtual CheckIoSeqResult CheckIoSeq (tsp00_TaskId          RedoTaskId,
                                         tgg00_WaitContext    &WaitContext,
                                         Log_IOSequenceNo      IoSeq,
                                         bool                 &waitInserted,
                                         SAPDBErr_MessageList &errlist);
    /// a redo task waits until preceding transactions are closed
    virtual bool WaitForIoSeq (tsp00_TaskId RedoTaskId);
    /// This is used to wait until the whole list is empty
    virtual void WaitForAllRedoTasks ( tsp00_TaskId RedoTaskId );
    /// The log reader calls this after recreation of all open transactions from last savepoint
    virtual void EnableRedoProcessing (tsp00_TaskId LogReaderTaskId);
	/// writes some basic information into the trace 
	virtual void WriteToTrace (const char *title = 0) const;
    /// write some additional information into the given errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
	/// This returns the youngest iosequence inserted to the redo list
	virtual Log_IOSequenceNo GetLastIOSequence (tsp00_TaskId taskid) const;
    /// Abort all redo tasks if waiting for something then wake them up
    virtual bool Abort (tsp00_TaskId taskid);
    /// returns true if redo is aborted
    virtual bool IsAborted () const;
    /// maximum number of transactions which are configure
    virtual SAPDB_UInt GetMaximumNumberOfTransactions() const;
    /// a list of transno's is returned, which is currently known by redo traffic control
	virtual bool GetTransactions (tsp00_TaskId            taskid,
                                  Rst_Transactions       &listOfTransToSkip,
                                  Msg_List               &errlist) const; // PTS 1134103 UH 2005-03-07 new
private:
	/// returns an IoSeqEntry which is removed from the free list
    Rst_IoSeqEntry *GetAndRemoveFromFreeIoSeqList ();
	/// returns the pointer to the job entry specified by its TransIndex
    Rst_JobEntry *GetJobEntry (tgg00_TransIndex TransIndex);
    /// puts an IoSeqEntry into the free list
    void PutIntoFreeIoSeqList (Rst_IoSeqEntry *pFreeEntry)
    {
        m_IoSeqFreeList.Append (pFreeEntry);
    }
	/// calls RemoveJobIntern in region
    void RemoveJob (tsp00_TaskId  RedoTaskId,
                    Rst_JobEntry *pJobEntry);
	/// a redo task removes a job and resumes waiting tasks if possible
    void RemoveJobIntern (tsp00_TaskId  RedoTaskId,
                          Rst_JobEntry *pJobEntry);
	/// checks the joblist
    bool CheckNewJobEntry (tsp00_TaskId          taskid,
                        tgg00_TransIndex      index,
                        Log_IOSequenceNo      iosequence,
                        const tgg91_TransNo  &transno,
                        SAPDBErr_MessageList &errlist);
    /// abort if all redo tasks are waiting
    bool AbortIfAllRedoTasksAreWaiting (tsp00_TaskId          taskid,
                                        SAPDBErr_MessageList &errlist,
                                        bool                  alreadyInRegion = false);
    /// internally used abort method
    bool AbortIntern (tsp00_TaskId taskid);
private:
    class RedoReaderWaitState
    {
    public:
    
        enum WaitReason
        {
            isNotWaiting,
            waitUntilRedoListIsEmpty,
            waitUntilNextJobIsFinished,
            waitUntilFreeTransEntriesExist,
            anyReason,
            WaitReason_NOT_USED
        };
        
        static const char * WaitReasonStrings [WaitReason_NOT_USED];

    public:
        /// sets all to zero
        RedoReaderWaitState()
        : m_TaskId(cgg_nil_pid)
        , m_WaitReason(isNotWaiting)
        {}
        /// registers a task to wait, if false is returned another task is waiting
        bool TaskWillWait(tsp00_TaskId task,
                       WaitReason   reason)
        {
            if (m_TaskId != cgg_nil_pid)
                return false;
            m_TaskId     = task;
            m_WaitReason = reason;
            return true;
        }
        /// returns true if any task is registered to wait
        bool IsWaiting() const
        {
            return m_TaskId != cgg_nil_pid;
        }
        /// returns true if a task is waiting and the reason matches
        tsp00_TaskId GetWaitingTask(WaitReason reason) const
        {
            return (reason == m_WaitReason || reason == anyReason)
                   ? m_TaskId
                   : cgg_nil_pid;
        }
        /// return s the taskid of currently registered task
        tsp00_TaskId CurrentTask() const
        {
            return m_TaskId;
        }
        /// returns the currently stored waiting reason
        WaitReason CurrentReason() const
        {
            return m_WaitReason;
        }
        /// returns true if a task is waiting and the reason matches
        void TryToResumeTask(tsp00_TaskId taskid,
                             WaitReason   reason);
        private:
            tsp00_TaskId m_TaskId;
            WaitReason   m_WaitReason;
    };
private:
    /// maximum number of concurrent running transactions
    SAPDB_UInt              m_NumberRedoTransactions;
    /// This is the number of redo execution tasks which could be started
    SAPDB_UInt              m_NumberOfStartedRedoTasks;
    /// flag which indicates the end of the Redo
    bool                    m_EndOfLog;
    /// List of idle ReDo-Tasks
    Kernel_TaskWaitList     m_VacantJobWait;
    /// redo reader task waiting in CloseOpenTrans
    RedoReaderWaitState     m_RedoReaderWaitState;
    /// number of waiting redo tasks
    /// This is used to determine a lack of resources (a deadlock in general)
	SAPDB_UInt              m_WaitingRedoTasks;
    /// list of RedoTransactions sorted by the EndOfTrans-IOSequence
    Rst_IoSeqList           m_IoSeqList;
    /// list of free RedoTransactions
    Rst_IoSeqList           m_IoSeqFreeList;
    /// pointer to next jobentry, which has to be redone
    Rst_JobEntry           *m_pFirstVacancy;
    /// List of jobs per IOSequence (sorted by IOSequence)
    Rst_IoSeqEntry         *m_pIoSeqContainer;
    /// Array of transactions. All Tranactions of one IOSequence are linked.
    Rst_JobEntry           *m_pJobContainer;
    /// used for dynamic allocating memory
    SAPDBMem_IRawAllocator &m_RawAllocator;
    /// global flag, which inicates, whether the Redo had been aborted
    volatile bool          &m_redoIsAborted;
    /// number of jobs inserted
	SAPDB_UInt             &m_inserted;
    /// number of jobs removed
	SAPDB_UInt             &m_removed;
    /// this flag prevents the redo tasks to start before all transactions are restored from savepoint
    bool                    m_RedoProcessingDisabled; // PTS 1124684 UH 2003-10-16
    Log_IOSequenceNo        m_OldestKnownEOTSequence; // PTS 1124684 UH 2003-10-16
};

#endif  /* RST_REDO_TRAFFIC_CONTROL_H */

