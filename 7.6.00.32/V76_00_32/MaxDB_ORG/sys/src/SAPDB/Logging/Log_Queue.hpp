/*!
  @file           Log_Queue.hpp
  @author         UweH
  @author         TillL
  @ingroup        Logging
  @brief          Log queue manager

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
#ifndef LOG_QUEUE_HPP
#define LOG_QUEUE_HPP

#include "gsp00.h" // tsp00_TaskId
#include "heo55k.h"
#include "ggg04.h"
#include "hgg08.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "Container/Container_Vector.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_RawDeviceIterator.hpp"
#include "Logging/Log_WriterTaskSynchronizer.hpp"
#include "Logging/Log_QueueRingBuffer.hpp"
#include "Logging/Log_AfterImageSpace.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "IOManager/IOMan_LogPages.hpp"
#include "RunTime/System/RTESys_MicroTime.h"

/*!
   @class          Log_Queue
   @brief          Log queue manager class

   Log_Queue provides user tasks (transaction contexts) with 
   memory space to be filled archive log entries.
   Log_Writer asks Log_Queue for filled log pages to be written
   out to log device.
 */
class Log_Queue
{
public:
    /// log queue entry
    typedef Log_QueueEntry                                      Entry;
    /// pointer to log queue entry
    typedef Log_QueueEntry*                                     EntryPointer;
    /// any number of log queue entries
    typedef SAPDB_UInt                                          SizeType;
    /// i/o sequence number of log pages
    typedef Log_IOSequenceNo                                    IOSequenceNo;
    /// identifier for log queues
    typedef Log_QueueID                                         QueueID;
    /// log page
    typedef Log_Page                                            Page;
    /// page frame handler
    typedef Kernel_Page::PageFrame                              PageFrame;
	/// vector of page frames
    typedef Container_Vector<PageFrame>                         PageFrameVector;
	/// log queue ring buffer object
    typedef Log_QueueRingBuffer                                 RingBuffer;
    /// allocator for queue entries
    typedef SAPDBMem_IRawAllocator                              Allocator;
	/// allocator for page frames
    typedef Log_FrameAllocator                                  FrameAllocator;
	/// iterator of ring buffer
    typedef RingBuffer::Iterator                                Iterator;
	/// const iterator of ring buffer
    typedef RingBuffer::ConstIterator                           ConstIterator;
	/// identifier of kernel task
    typedef tsp00_TaskId                                        TaskID;
    /// selector for Statistics, which counter shall be returned
    enum QueueCounterSelector
    {
        MonitorLog_OBSOLETE,
        SystemViewLogQueue,
        SystemViewLogQueueReset
    };
    /// selector for Statistics, which values shall be resetted
    enum QueueStatisticSelector
    {
        ResetMonitorLog_OBSOLETE,
        ResetSystemView
    };
    /// kinds of all counters
    enum CounterKinds
    {
        queueUsage = 0,         ///< Number of transactions connected to this queue
        openTrans  = 1,         ///< Number of open transactions writing to this log queue
        waitingForLogIO = 2,    ///< Number of tasks waiting for log write io
        pagesReadyForLogIO = 3, ///< Number of pages ready for write io
        MAX_CounterKind = 4     ///< not used
    };
    /// queue synchronization mode (indicates whether queue requires sync. regions)
    enum SynchronizationMode
    {
        UnSynchronized,
        Synchronized
    };
    /*!
        @brief          Constructor
        @param          queueid [in] log queue id
        @param          syncmode [in] queue synchronization mode
        @param          flushmode [in] append mode for log pages
        @param          writersync [in] writer task synchronization object
        @param          alloc [in] raw allocator to be used by queue
        @param          framealloc [in] frame resource from which to allocate page frames
     */
    Log_Queue
        (QueueID                        queueid,
         SynchronizationMode            syncmode,
         Log_FlushMode                  flushmode,
         Log_WriterTaskSynchronizer&    writersync,
         Allocator&                     alloc,
         FrameAllocator&                framealloc)
        :
         m_QueueID              (queueid),
         m_Synchronized         (syncmode == Synchronized ? true : false),
         m_FlushMode            (flushmode),
         m_MaxDelayLogWriter (false),
         m_WriterTaskSync       (writersync),
         m_Allocator            ( UTF8( "Log_Queue" ), alloc ), 
         m_RingBuffer           (m_Allocator, framealloc),
         m_CurrentAppend        (0),
         m_CurrentOutput        (0),
         m_Allocated            (false),
         m_PagesUsed            (1), // PTS 1115332 UH 2002-04-29 added
         m_LogIsFull(true),
         m_ForceNewPage(false),
         m_lastResettedCounter(ResetSystemView),
         m_FlushModeChanged(false)
    {
        m_MaxDelayLogWriter = gg01_delay_lw;
    }
    /*!
        @brief          Initializes queue structures
        @param          queueID - the numerical ID of the queue
        @param          size [in] queue size; number of entries / page frames
        @return         (SAPDB_Bool) true if successful

        - Performs memory allocation of queue entries and page frames.
     */
    SAPDB_Bool Initialize  (const SAPDB_UInt size)
    {
        // queue needs at least 3 pages to work properly
        SAPDBERR_ASSERT_ARGUMENT(size >= 3);

        if (m_Allocated == false)
        {
            if (!m_RingBuffer.Initialize(size))
                return false;
            m_Allocated = true;
        }

        m_CurrentAppend = m_RingBuffer.Anchor();
        m_CurrentOutput = m_RingBuffer.Anchor();
        m_IOSequenceNo.Invalidate();
        m_QueueSequenceNoOfLastPageOfLastWrittenBlock.Invalidate();
        m_timeOfLastReset = RTESys_MicroSecondTime();
        return true;
    }
    /// Deletes queue structures and performs memory deallocation
    void Delete()
    {
        m_RingBuffer.Delete();
    }
    /// Destructor
    ~Log_Queue()
    {
        Delete();
    }
    /// signal the Log_Queue that a new transaction is using this queue
    void IncrementCounter(CounterKinds kind,
                          SAPDB_Int4   value = 1)
    {
        m_Counter.Increment(kind, value);
    }
    /// signal the Log_Queue that a transaction stops using this queue
    void DecrementCounter(CounterKinds kind,
                          SAPDB_Int4   value = 1)
    {
        m_Counter.Decrement(kind, value);
    }
    /// returns the number of transaction which are using this queue. This method is
    /// not sysnchronized with IncrementUsage() or DecrementUsage()
    SAPDB_UInt4 GetCounterValue(CounterKinds kind)
    {
        return m_Counter.GetValue(kind);
    }
    /// Changes the flushmode of the queue. This needs not to be done in the region,
    /// because GetOutput maintains a consistent view onto m_FlushMode
    /// This mode needs only to be changed for LogQueue #0, because if more than
    /// one queue is used, the mode has to be MaximizeSafety.
    /// @ATTENTION This method has to be called inside the LogRegion
    void ChangeFlushMode (const Log_FlushMode newFlushMode)
    {
        SAPDBTRACE_METHOD_DEBUG ("Log_Queue::ChangeFlushMode", LogVolume_Trace, 5);

        m_FlushMode = newFlushMode;
        m_FlushModeChanged = true;
    }

    /*!
        @brief          Reserves requested memory space on log pages
        @param          usertaskid [in] calling user task id
        @param          waitcontext [in/out] used for waiting for space in queue
        @param          size [in] requested memory size in bytes
        @param          unsplitsize [in] min. unsplit size of first chunk
        @param          useReservedSpace [in] if set to true, also during LogFull space may be reserved
        @param          space [in/out] split space descriptor
        @param          errlist [out] information in case of an error

       - The requested memory of 'size' bytes is reserved by the queue                  on its internal log pages. A split space description of the                 memory chunks is returned in 'space'. 
       - The first chunk must have a size of at least 'unsplitsize'                 bytes. If this space is not available on the current page,                  reservation is extended to the next page(s).
       - If there is currently not enough log space available to                 fulfill the request, the calling task identified by 'usertaskid'                  is suspended. No space at all is reserved for the task in                  this case until resumed.
       - If the Log-Device is full Reserve() will return false, because                 normal operations cannot be logged anymore. In all other cases                 it will return true (PTS 1111924 mb 2001-09-26 )
       - The method expects 'space' to be unassigned.
     */
    bool Reserve 
        (TaskID                            usertaskid,
         Kernel_TaskWaitList::WaitContext &waitcontext,
         SizeType                          size,
         SizeType                          unsplitsize,
         bool                              useReservedSpace, // PTS 1117320 UH 2002-10-28
         Log_AfterImageSpace              &space,
         SAPDBErr_MessageList             &errlist)
    {
        if ( m_LogIsFull // PTS 1111924 mb 2001-09-26
             &&
             ! useReservedSpace ) 
        {
            // PTS 1117320 UH 2002-10-28
            WaitUntilSpaceIsAvailable(usertaskid, waitcontext, WaitForLogFree);
        }

        Log_Page::OffsetType dummy;

        return ReserveSpace(usertaskid, waitcontext, size, unsplitsize, space, dummy, errlist);
    }
    /*!
        @brief Reserves requested memory space for writing out an end-of-transaction entry
        @param usertaskid [in] calling user task id
        @param waitcontext [in/out] used for waiting for space in queue
        @param size [in] requested memory size in bytes
        @param space [out] split space descriptor
        @param entrypos [out] position of entry on page
        @param errlist [out] information in case of an error
     */
    bool ReserveForEOT
        (TaskID                            usertaskid,
         Kernel_TaskWaitList::WaitContext &waitcontext,
         SizeType                          size,
         Log_AfterImageSpace              &space,
         Log_Page::OffsetType             &entrypos,
         SAPDBErr_MessageList             &errlist)
    {
        return ReserveSpace(usertaskid, waitcontext, size, size, space, entrypos, errlist);
    }
    /*!
        @brief Tells the queue that an associated user task is ready
        @param usertaskid [in] calling user task id
        @param space [in/out] split space descriptor

       - A user task calls this method in order to tell the queue                  that previously reserved space is ready for i/o.
       - As a consequence of this call, the writer task is resumed                 if at least one page is available for output.
     */
    void UserTaskReady
        (TaskID                         usertaskid,
         Log_AfterImageSpace&           space);
    /*!
        @brief          Tells the queue that an associated user task is ready with writing an EOT entry
        @param          usertaskid [in] calling user task id
        @param          waitcontext [in/out] this is used to wait for io
        @param          space [in/out] split space descriptor
        @param          wioseqno [out] writer i/o sequence no
        @param          offset [out] offset of page on device

        - A user task calls this method in order to tell the queue                  that previously reserved space is ready for i/o.
        - As a consequence of this call, the writer task is resumed                 if at least one page is available for output.
        - The user task is suspended until the log writer has flushed                  the page. The user task is resumed by the writer.
        - The parameter 'wioseqno' is filled with the writer i/o                 sequence number of the log page on that the entry has been                 written.
     */
    void UserTaskEOTReady
        (TaskID                            usertaskid,
         Kernel_TaskWaitList::WaitContext &waitcontext,
         Log_AfterImageSpace              &space,
         IOSequenceNo                     &wioseqno,
         Log_RawDeviceOffset              &offset);
    /*!
        @brief          When saving Log online, the the last page must be flushed
                        in order to save it. (PTS 1123761 mb 2003-09-04)
    */
    void ForceNewPage (TaskID               backupCoordinator,
                       Log_RawDeviceOffset &offsetOfLastWrittenPage); // PTS  1129651 mb 2004-05-13

    /*!
        @brief          Puts together filled log pages for write i/o
        @param          maxPages [in] maximum number of pages wich can be put into the vector output
        @param          output [in/out] collection of log pages
        @param          pagecount [out] number of added pages
        @param          pageIsFlushedAgain [out] true if first page returned in 'output'
                        must replace (overwrite) the last page returned in the previous call 
                        to GetOutput()
        @param          useOnlyQueue0 [out] this is set to true if flushmode changed

       - Log pages that are ready for i/o are assigned to the vector                  'output'.
       - These pages are marked with queue id and queue i/o                 sequence number.
     */
    void GetOutput (SAPDB_UInt      maxPages,
                    IOMan_LogPages &output,
                    SAPDB_UInt     &pagecount,
                    SAPDB_Bool     &pageIsFlushedAgain,
                    SAPDB_Bool     &useOnlyQueue0);
    /*!
        @brief          Tells the queue that the queue writer has successfully written some pages
        @param          writertaskid [in] calling writer task id
        @param          wioseqno [in] contains the writer i/o sequence no
                                       with which the first of the 'pagecount'
                                       pages was written
        @param          writePosition [in] points to the first written page 
        @param          pagecount [in] number of pages that have been written

        - As a consequence of this call, the user tasks in the                 user task wait queue are resumed.
     */
    void WriterTaskReady
        (TaskID                         writertaskid,
         IOSequenceNo                   wioseqno,
         Log_RawDeviceIterator          writePosition,            // PTS 1125634 mb 2003-11-26
         SAPDB_UInt                     pagecount);
    /*!
       @brief          Sets the state of m_LogIsFull
       @param          writerTaskId [in] calling writer task id
       @param          newState [in] new state of m_LogIsFull
     */
    void SetLogFullState
        (TaskID                         writerTaskId,
         bool                           newState)
    {
        // PTS 1111924 mb 2001-09-26 new
        EnterRegion(writerTaskId);
        if ( m_LogIsFull != newState )
        {
            m_LogIsFull = newState;
            if ( ! m_LogIsFull )
                m_LogFullWaitList.ResumeAll();
        }
        LeaveRegion(writerTaskId);
    }
    /// returns if log queue is marked for LogFull
    bool IsLogFull() const
    {
        return m_LogIsFull;
    }
    /// Returns the queue id
    QueueID GetID() const
    {
        return m_QueueID;
    }
	/// Returns the queue size in page frames
    SAPDB_UInt GetSize() const
    {
        return m_RingBuffer.GetEntryCount();
    }
    /// returns currently set flush mode
    Log_FlushMode GetFlushMode() const
    {
        return m_FlushMode;
    }
	/// Returns number of pages maximal used
    SAPDB_UInt GetMaxPagesUsed(const QueueCounterSelector statSelect) const;
	/// Returns number of user task suspends that have occurred due to queue full
    SAPDB_UInt GetQueueFullSuspendCount(const QueueCounterSelector statSelect) const;
	/// Returns number of requests to reserve space for redo entries.
    SAPDB_UInt GetReserveCount(const QueueCounterSelector statSelect) const;
	/// Returns number of waits for io.
    SAPDB_UInt GetWaitCount(const QueueCounterSelector statSelect) const;
	/// Returns number of group commits
    SAPDB_UInt GetGroupCommitCount(const QueueCounterSelector statSelect) const;
    /// returns the maximum tasks which waited for one page io
    SAPDB_UInt GetMaxCountOfWaitingTasksPerPage(const QueueCounterSelector statSelect) const;
    /// returns the number of physical writes for this queue
    SAPDB_UInt GetPhysWriteCount(const QueueCounterSelector statSelect) const;
    /// resets statistic counter to zero
    void ResetStatistics(tsp00_TaskId taskid, const QueueStatisticSelector selectedCounter);
	/// writes the queues information into the trace.
    SAPDB_UInt8 GetTimeOfLastStatisticsReset()
    {
        return m_timeOfLastReset;
    }
    void WriteToTrace() const;
	/// writes one queue entry to trace
    void WriteEntryToTrace(const Log_QueueEntry &entry,
                           const char           *title           = 0,
                           SAPDB_UInt            entryno         = 0,
                           bool                  isCurrentAppend = false,
                           bool                  isCurrentOutput = false) const;
    /// Enters region if necessary
    void EnterRegion(TaskID taskid) const
    {
        if (m_Synchronized)
            vbegexcl(taskid, g08logqueue0 + m_QueueID);
    }
    /// Leaves region if necessary
    void LeaveRegion(TaskID taskid) const
    {
        if (m_Synchronized)
            vendexcl(taskid, g08logqueue0 + m_QueueID);
    }
private:
    /// structure for accumulating statistical informations
    struct Log_QueueStatistics {
        /// number of suspends that have occurred due to queue full
        SAPDB_UInt          m_QueueFullSuspendCount;
        /// number of reserves that have occurred
        SAPDB_UInt          m_ReserveCount;
        /// number of waits for io that have occurred
        SAPDB_UInt          m_WaitCount;
        /// number of group commits
        SAPDB_UInt          m_GroupCommitCount;
        /// maxuimum number of tasks waiting for one page io
        SAPDB_UInt          m_MaxCountOfWaitingTasksPerPage;
        /// number of pages used maximal
        SAPDB_UInt          m_MaxPagesUsed; 
        /// number of pages used maximal
        SAPDB_UInt          m_WriteCount; 

        /// CTOR
        Log_QueueStatistics() :
            m_QueueFullSuspendCount(0),
            m_ReserveCount(0),
            m_WaitCount(0),
            m_GroupCommitCount(0),
            m_MaxCountOfWaitingTasksPerPage(0),
            m_MaxPagesUsed (0),
            m_WriteCount (0) {}
    };
    /// add two statisitical value-sets
    Log_QueueStatistics CombineStatistics (const Log_QueueStatistics & operand1,
                                           const Log_QueueStatistics & operand2);
    /// template-function for combining the different counters
    template <typename T> T GetMaxQueueCounterValue (const QueueCounterSelector counter,
                                                     const T basevalue,
                                                     const T offset,
                                                     const T currentCounter) const ;
    /// template-function for combining the different counters
    template <typename T> T GetSumQueueCounterValue (const QueueCounterSelector counter,
                                                     const T basevalue,
                                                     const T offset,
                                                     const T currentCounter) const ;
    /// a selector for selecting on which waitingqueue the task shall be inserted
    enum WaitQueueSelector
    {
        WaitForWriterAction,
        WaitForLogFree
    };
    /*!
        @brief Reserves requested space
        @param usertaskid  [in]  calling user task id
        @param size        [in]  requested memory size in bytes
        @param unsplitsize [in]  min. unsplit size of first chunk
        @param space       [out] space to be filled
        @param entrypos    [out] position of entry on page
        @return bool
     */
    bool ReserveSpace
        (TaskID                            usertaskid,
         Kernel_TaskWaitList::WaitContext &waitcontext,
         SizeType                          size,
         SizeType                          unsplitsize,
         Log_AfterImageSpace              &space,
         Log_Page::OffsetType             &entrypos,
         SAPDBErr_MessageList             &errlist);
    /// This must be used if a reserve call cannot find enough space in the queue.
    void WaitUntilSpaceIsAvailable(tsp00_TaskId                      usertaskid,
                                   Kernel_TaskWaitList::WaitContext &waitcontext,
                                   WaitQueueSelector                 queueSelector);
    /// This tries to find the requested space in the queue.
    void FindSpaceToReserve (tsp00_TaskId                      usertask,
                             Kernel_TaskWaitList::WaitContext &waitcontext,
                             SizeType                          size,
                             SizeType                          unsplitsize);
    /// Find the first position from where space can be reserved.
    void FindStartPointToReserve (tsp00_TaskId                      usertask,
                                  Kernel_TaskWaitList::WaitContext &waitcontext,
                                  SizeType                          size,
                                  SizeType                          unsplitsize);
    /*!
        @brief Checks whether referenced log page is available for output

        - This frame is ready for output only if two conditions are fulfilled:
          - (1) there are actually no "busy" user tasks writing on the frame
          - (2a) the current append position is beyond this page, or
            (2b) there are user tasks in 'commit' or 'savepoint complete' 
                 state waiting for the frame to be flushed
                 (in this case the frame doesn't need to be full)
        @param  iter   [in]  points to entry that is to be checked
        @return (SAPDB_Bool) true if available
     */
    SAPDB_Bool OutputAvailableAt  (const Iterator                &iter) const
    {
        Log_QueueEntry& entry = *iter;
        
        return (entry.GetBusyTaskCount() == 0)                      // (1)
               &&
               ((iter != m_CurrentAppend) ||                        // (2a)
                (entry.TasksAreWaitingForIO()));              // (2b)
    }
    /// Returns the maximum size one single log entry can have based on the actual queue size
    SizeType MaxReserveSize() const
    {
        return m_RingBuffer.GetSize() * m_CurrentAppend->GetPage().MaxFreeSpace();
    }
    /*!
        @brief Copies the page at m_CurrentAppend to the next queue entry.
    
        This is used to if a EOT entry was written by the user or
        if the user wants to reserve space and the writer will already
        write the page (IsMarkedForIO()==true)
     */
    void CopyCurrentAppend();
    /// This can copy page contents to another page in the queue or simply use the next page.
    void UseNextPageCausedByIO();
    /// This uses the next queue page (current append is incremented). Space must be available.
    void UseNextPage();
    /// used for allocation
    RTEMem_AllocatorWrapper                 m_Allocator;
    /// ring buffer for queue entries
    RingBuffer                              m_RingBuffer;
    /// iterator at current append position within ring buffer
    Iterator                                m_CurrentAppend;
    /// iterator at current output position within ring buffer (references page that is to be written out next)
    Iterator                                m_CurrentOutput;
    /// log writer task synchronization object
    Log_WriterTaskSynchronizer&             m_WriterTaskSync;
    /// user tasks waiting when queue full 
    Kernel_TaskWaitList                     m_QueueFullWaitList;
    /// user tasks waiting when log is full 
    Kernel_TaskWaitList                     m_LogFullWaitList;
    /// queue i/o sequence number
    IOSequenceNo                            m_IOSequenceNo;
    /// last queue i/o sequence number, which was given to the writer.
    IOSequenceNo                            m_QueueSequenceNoOfLastPageOfLastWrittenBlock;
    /// ID of the log queue
    const QueueID                           m_QueueID;
    /// indicates whether queue requires synchronization
    const SAPDB_Bool                        m_Synchronized;
    /// log queue flush mode
    Log_FlushMode                           m_FlushMode;
    /// indicates whether the Log_Writer shall schedule when the last logpage is only partially filled
    SAPDB_Int4                              m_MaxDelayLogWriter;
    /// indicates whether queue structures have been allocated
    SAPDB_Bool                              m_Allocated;
    /// number of pages currently used
    SAPDB_UInt                              m_PagesUsed; // PTS 1115332 UH 2002-04-29 added
    /// If set to TRUE only reserveEOT will be succeed, calls to reserve() will be rejected
    // PTS 1111924 mb 2001-09-26
    bool                                    m_LogIsFull;
    /// Forces the use of a new page instead of eventually copying the current_append
    bool                                    m_ForceNewPage;
    /// for which statistical value-set has reset called last time
    QueueStatisticSelector                  m_lastResettedCounter;
    /// base-values for statistical values. 
    Log_QueueStatistics                     m_StatisticalBaseValues;
    /// base-values for calculating second set of resettable statistical values. 
    Log_QueueStatistics                     m_StatisticalIntermediateValues;
    /// current values for statistical values. 
    Log_QueueStatistics                     m_StatisticalCurrentValues;
    /// definition of a collection of counters
    struct Counter
    {
        /// the values
        SAPDB_UInt4 counter [MAX_CounterKind];   
        /// set all to 0
        Counter()
        {
            for (int i = queueUsage; i < MAX_CounterKind; ++i )
                counter[i] = 0;
        }
        /// atomic +1
        void Increment(CounterKinds kind,
                       SAPDB_Int    value = 1)
        {
            RTESys_AtomicModify(counter[kind], value);
        }
        /// atomic -1
        void Decrement(CounterKinds kind,
                       SAPDB_Int    value = 1)
        {
            RTESys_AtomicModify(counter[kind], - value);
        }
        /// not sysnchronized with Increment() or Decrement()
        SAPDB_UInt4 GetValue(CounterKinds kind)
        {
            return RTESys_AtomicRead(counter[kind]);
        }
    };
    /// a counter collection
    Counter                                 m_Counter;
    /// timestamp of the last reset command for Log_QueueStatistics
    SAPDB_UInt8                             m_timeOfLastReset;
    /// if true the writer must not write other pages then of queue 0.
    bool                                    m_FlushModeChanged;
};
#endif  // LOG_QUEUE_HPP
