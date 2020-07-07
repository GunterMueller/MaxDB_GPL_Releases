/*!
    @ingroup        Restart
    @file           Rst_IoSeqList.hpp
    @author         UweH
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
#ifndef RST_REDO_IO_SEQ_LIST_H
#define RST_REDO_IO_SEQ_LIST_H

#include "Restart/Rst_Exceptions.hpp"

/// forward declaration
class Rst_IoSeqEntry;

/*!
   @class          Rst_DoubleChainEntry
   @brief          a double chained entry
 */
class Rst_DoubleChainEntry
{
private:
    /// pointer to the successor-node in the chain
    Rst_DoubleChainEntry *m_pNext;
    /// pointer to the predecessor in the chain
    Rst_DoubleChainEntry *m_pPrev;
public:
    /// constructor
    Rst_DoubleChainEntry ()
    : m_pNext (NULL),
      m_pPrev (NULL)
    {}
	/// returns the previous entry
    Rst_DoubleChainEntry *GetPrev () const
    {
        return m_pPrev;
    }
	/// returns the following entry
    Rst_DoubleChainEntry *GetNext () const
    {
        return m_pNext;
    }
	/// inserts an entry after the given entry
    /// if the given prev entry is 0 then the entry is added as new first
    void Insert (Rst_DoubleChainEntry* &pFirstListEntry,
                 Rst_DoubleChainEntry* &pLastListEntry,
                 Rst_DoubleChainEntry*  pPrevListEntry)
    {
		// PTS 1124684 UH 2003-10-16 written from earlier Append()
        if (0 == pFirstListEntry)
        {
            m_pPrev         = NULL;
            m_pNext         = NULL;
            pFirstListEntry = this;
            pLastListEntry  = this;
            return;
        }

        m_pPrev = pPrevListEntry;

        if (pPrevListEntry == 0)
        {
            m_pNext         = pFirstListEntry;
            pFirstListEntry = this;
        }
        else
        {
            m_pNext                 = pPrevListEntry->m_pNext;
            pPrevListEntry->m_pNext = this;
        }

        if ( m_pNext != 0 )
            m_pNext->m_pPrev = this;

        if (pPrevListEntry == pLastListEntry)
            pLastListEntry = this;
    }
    /// removes this entry from the entry list
    void Remove (Rst_DoubleChainEntry *&pFirstListEntry,
                 Rst_DoubleChainEntry *&pLastListEntry)
    {
        if (NULL == m_pPrev)
            pFirstListEntry = m_pNext;
        else
            m_pPrev->m_pNext = m_pNext;
        
        if (NULL == m_pNext)
            pLastListEntry = m_pPrev;
        else
            m_pNext->m_pPrev = m_pPrev;
        
        m_pPrev = NULL;
        m_pNext = NULL;
    }
};
/*!
   @class Rst_DoubleChainList
   @brief a double chained list without any allocation
 */
class Rst_DoubleChainList
{
private:
    /// this is the first element in the chain
    Rst_DoubleChainEntry *m_pFirst;
    /// this is the last element in the chain
    Rst_DoubleChainEntry *m_pLast;
public:
    /// constructor
    inline Rst_DoubleChainList ()
    : m_pFirst (NULL),
      m_pLast  (NULL)
    {}
	/// inserts an entry after the given entry if it is 0 then it is inserted at first
    void Insert (Rst_DoubleChainEntry*  pEntry,
                 Rst_DoubleChainEntry*  pPrevEntry)
    {
        pEntry->Insert (m_pFirst, m_pLast, pPrevEntry);
    }
    /*!
        @brief Inserts an entry at the end of the list
        @param pEntry [in] the pointer to the entry to be appended
     */
    void Append (Rst_DoubleChainEntry *pEntry)
    {
        pEntry->Insert (m_pFirst, m_pLast, m_pLast);
    }
	/// removes an entry from the list and returns the pointer to the removed entry
    Rst_DoubleChainEntry *GetAndRemoveAny ()
    {
        Rst_DoubleChainEntry *pEntry = m_pFirst;
        Remove (pEntry);
        return (pEntry);
    }
    /// returns the pointer to the first entry of the list
    Rst_DoubleChainEntry *GetFirst () const
    {
        return m_pFirst;
    }
	/// returns the pointer to the last entry of the list
    inline Rst_DoubleChainEntry *GetLast () const
    {
        return m_pLast;
    }
	/// returns true, if the list is empty
    bool IsEmpty () const
    {
        return NULL == m_pFirst;
    }
    /*!
        @brief  removes the entry from the list
        @param  pEntry [in] the pointer to the entry to be removed
     */
    void Remove (Rst_DoubleChainEntry *pEntry)
    {
        if ( pEntry != NULL )
            pEntry->Remove (m_pFirst, m_pLast);
    }
};
/*!
    @class          Rst_JobEntry
    @brief          an entry of the job container list
 */
class Rst_JobEntry : public Rst_DoubleChainEntry
{
private:
    /// transaction index of this job
    tgg00_TransIndex  m_OwnIndex;
    /// flag, which denotes, whether this job is executing an rollbackward or rollforward
    Log_ActionType    m_UndoOrRedo; 
    /// pointer to the corresponding IOSequence related task-information
    Rst_IoSeqEntry   *m_pIoSeqEntry;
    /// transaction context
    Log_Transaction   m_Transaction;
public:
    /// default constructor sets all nil
    Rst_JobEntry()
    : m_OwnIndex   (cgg_nil_transindex),
      m_UndoOrRedo (Log_NoOp),
      m_pIoSeqEntry(NULL)
    {}
    /// returns pointer to the corresponding IoSeqEntry
    Rst_IoSeqEntry *GetIoSeqEntry () const
    {
        return (m_pIoSeqEntry);
    }
	/// returns the following entry
    Rst_JobEntry *GetNext () const
    {
        return ( REINTERPRET_CAST (Rst_JobEntry*, Rst_DoubleChainEntry::GetNext()) );
    }
	/// returns pointer of the transaction handle of this job
    Log_Transaction &GetTransaction ()
    {
        return m_Transaction;
    }
	/// returns TransIndex of this job
    tgg00_TransIndex GetTransIndex () const
    {
        return (m_OwnIndex);
    }
	/// returns the action type commit or rollback
    Log_ActionType GetUndoOrRedo () const
    {
        return (m_UndoOrRedo);
    }
	/// initializes the job entry
    void Init (Log_ActionType   UndoOrRedo,
               Rst_IoSeqEntry  *pIoSeqEntry,
               Log_Transaction &transaction)
    {
        m_UndoOrRedo   = UndoOrRedo;
        m_pIoSeqEntry  = pIoSeqEntry;
        m_Transaction.CopyFromRedoReaderToRedoJobList(transaction);
    }
	/// assigns the OwnIndex
    void SetOwnIndex (tgg00_TransIndex OwnIndex)
    {
        m_OwnIndex = OwnIndex;
    }
};
/*!
    @class  Rst_IoSeqEntry
    @brief  entry of a task wait stack
 */
class Rst_IoSeqEntry: public Rst_DoubleChainEntry
{
private:
    /// The IOSequence of the waiting task
    Log_IOSequenceNo    m_IoSeq;
    /// the list of jobs, waiting for this IOSequence
    Rst_DoubleChainList m_JobList;
    /// the corresponding task-id's, which are waiting for this IOSequence
    Kernel_TaskWaitList m_WaitStack;
public:
    /// returns the first job entry
    Rst_JobEntry* GetFirstJobEntry () const
    {
        return static_cast<Rst_JobEntry*>(m_JobList.GetFirst());
    }
	/// returns the previous entry
    Rst_IoSeqEntry* GetPrev () const
    {
        return static_cast<Rst_IoSeqEntry*>(Rst_DoubleChainEntry::GetPrev());
    }
    /// returns the following entry
    Rst_IoSeqEntry* GetNext () const
    {
        return static_cast<Rst_IoSeqEntry*>(Rst_DoubleChainEntry::GetNext());
    }
	/// returns the I/O sequence contained in the entry
    Log_IOSequenceNo GetIoSeq () const
    {
        return m_IoSeq;
    }
	/// initializes the I/O sequence entry
    void Init (Log_IOSequenceNo IoSeq)
    {
        m_IoSeq = IoSeq;
    }
	/// appends a job into the job list
    void AppendJobEntry (Rst_JobEntry *pJobEntry)
    {
        m_JobList.Append (pJobEntry);
    }
	/// inserts a task id into the WaitStack for this IoSeq
    void InsertWaitTask (tsp00_TaskId       TaskId,
                         tgg00_WaitContext &WaitContext)
    {
        m_WaitStack.InsertAsFirst(TaskId, WaitContext);
    }
	/// returns true, if the job list is empty
    bool IsEmptyJobList () const
    {
        return ( m_JobList.IsEmpty() );
    }
	/// removes a job entry from the job list for this IoSeq
    void RemoveJobEntry (Rst_JobEntry *pJobEntry)
    {
        m_JobList.Remove (pJobEntry);
    }
	/// resumes all tasks waiting for this IoSeq
    SAPDB_UInt TryResumeTasks()
    {
        if ( m_JobList.IsEmpty() )
        {
            const SAPDB_UInt numResumedTasks = m_WaitStack.NumberOfWaitingTasks();
            m_WaitStack.ResumeAll();
            return numResumedTasks;
        }
        return 0;
    }
};
/*!
    @class Rst_IoSeqList
    @brief a list containing I/O sequence entries
 */
class Rst_IoSeqList: public Rst_DoubleChainList
{
public:
    /*!
        @brief   removes an entry from the list
        @return  the pointer to the removed entry
     */
    Rst_IoSeqEntry* GetAndRemoveAny ()
    {
        return static_cast<Rst_IoSeqEntry*>(Rst_DoubleChainList::GetAndRemoveAny());
    }
	/// the pointer to the first entry of the list
    Rst_IoSeqEntry* GetFirst () const
    {
        return static_cast<Rst_IoSeqEntry*>(Rst_DoubleChainList::GetFirst());
    }
	/// returns the pointer to the last entry of the list
    Rst_IoSeqEntry* GetLast () const
    {
        return static_cast<Rst_IoSeqEntry*>(Rst_DoubleChainList::GetLast());
    }
	/// returns the I/O sequence of the last entry, nil if list is empty
    Log_IOSequenceNo GetLastIoSeq () const
    {
        if ( IsEmpty() )
            return Log_IOSequenceNo();
        return GetLast()->GetIoSeq();
    }
    /// inserts a new entry at the correct position
    void Insert(Rst_IoSeqEntry   *pEntry,
                Log_IOSequenceNo  oldestKnownEOTSequence)
    {
		// PTS 1124684 UH 2003-10-16 new
        Rst_DoubleChainList::Insert( pEntry, 
                                     SearchPrevEntry(pEntry->GetIoSeq(),
                                                     oldestKnownEOTSequence) );
    }
    /// appends a new entry at the end
    void Append(Rst_IoSeqEntry *pEntry)
    {
        Rst_DoubleChainList::Append(pEntry);
    }
    /*!
        @brief finds the largest IoSeqEntry which is less IoSeq parameter and waits there
        @param RedoTaskId [in] the callers taskid
        @param WaitContext [in/out] used for waiting
        @param IoSeq [in] sequence of the log entry handled by the redo task
        @param FirstIoSeq [in] This is used to compare overflown io sequences.
        @param WaitInserted [out] true, if the task id is inserted into a wait list
        @param errlist [out] contains an error-description upon unsuccessful completition
     */
    bool WaitForIoSeq (tsp00_TaskId          RedoTaskId,
                       tgg00_WaitContext    &WaitContext,
                       Log_IOSequenceNo      IoSeq,
                       Log_IOSequenceNo      FirstIoSeq,
                       bool                 &WaitInserted,
                       SAPDBErr_MessageList &errlist)
    {
        SAPDBTRACE_METHOD_DEBUG ("Rst_IoSeqList::WaitForIoSeq", LogTrans_Trace, 5);
        
        WaitInserted = false;
        
        if ( GetFirst() == 0 )
            return true; // list is empty
            
        if ( LogTrans_Trace.TracesLevel(6) )
            Kernel_VTrace() << "REDOTRAFFIC: check ioseq: " << IoSeq
                            << ", first eotseq: " << GetFirst()->GetIoSeq() << NewLine;

        if ( IoSeq.IsInvalid() ) // PTS 1140414 UH 2006-03-01
            return false;

        if ( IoSeq == GetFirst()->GetIoSeq()
             ||
             // PTS 1120201 UH 2003-02-17
             Log_IOSequenceNo::LeftIsOlder(IoSeq, GetFirst()->GetIoSeq(), FirstIoSeq) )
            return true;
            
        Rst_IoSeqEntry *pCurr = GetFirst();
     
        if ( NULL == pCurr )
        {
            errlist = Restart_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"pCurr not nil.");
            return false;
        }
     
        Rst_IoSeqEntry *pNext = pCurr->GetNext();
    
        if ( NULL == pNext )
        {
            errlist = Restart_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"pNext not nil.");
            return false;
        }
                       
        while ( Log_IOSequenceNo::LeftIsOlder(pNext->GetIoSeq(), IoSeq, FirstIoSeq) ) // PTS 1120201 UH 2003-02-17
        {
            pCurr = pNext;
            pNext = pNext->GetNext();
        }
    
        if ( Log_IOSequenceNo::LeftIsOlder(pCurr->GetIoSeq(), IoSeq, FirstIoSeq) )// PTS 1120201 UH 2003-02-17
        {
            pCurr->InsertWaitTask (RedoTaskId, WaitContext);
            WaitInserted = true;
        }
        if ( WaitInserted && LogTrans_Trace.TracesLevel(6) )
            Kernel_VTrace() << "REDOTRAFFIC: wait for eotseq: " << pCurr->GetIoSeq();
        return true;
    }
    /// search an entry which has the greatest ioseq which is smaller then the given
    /// If the list is empty or the given ioseq is smaller then all 0 is returned.
    Rst_IoSeqEntry* SearchPrevEntry(const Log_IOSequenceNo IoSeq,
                                    const Log_IOSequenceNo oldestKnownEOTSequence)
    {
		// PTS 1124684 UH 2003-10-16 new
        if ( GetFirst() == 0 || GetLast() == 0)
            return 0;

        const Log_IOSequenceNo firstIoSeq = GetFirst()->GetIoSeq();
        const Log_IOSequenceNo lastIoSeq  = GetLast()->GetIoSeq();

        for ( Rst_IoSeqEntry* pCurrentEntry = GetLast();
              pCurrentEntry != 0;
              pCurrentEntry = pCurrentEntry->GetPrev() )
        {
            if ( Log_IOSequenceNo::LeftIsYounger(IoSeq, pCurrentEntry->GetIoSeq(), oldestKnownEOTSequence) )
                return pCurrentEntry;
        }
        return 0;
    }
    /// search an entry which has the same ioseq if none is found 0 is returned.
    Rst_IoSeqEntry* SearchEntry(const Log_IOSequenceNo IoSeq)
    {
		// PTS 1124684 UH 2003-10-16 new
        for ( Rst_IoSeqEntry* pCurrentEntry = GetLast();
              pCurrentEntry != 0;
              pCurrentEntry = pCurrentEntry->GetPrev() )
        {
            if ( IoSeq == pCurrentEntry->GetIoSeq() )
                return pCurrentEntry;
        }
        return 0;
    }
};

#endif  /* RST_REDO_IO_SEQ_LIST_H */

