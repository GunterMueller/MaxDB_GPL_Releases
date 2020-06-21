/*!
  @file           Log_Writer.hpp
  @author         UweH
  @author         MartinB
  @author         TillL
  @ingroup        Logging     
  @brief          Log writer

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
#ifndef LOG_WRITER_HPP
#define LOG_WRITER_HPP

#include "Container/Container_Vector.hpp"
#include "Logging/Log_Queue.hpp"
#include "Logging/Log_RawDeviceIterator.hpp"
#include "Logging/Log_InfoPage.hpp"
#include "Logging/Log_WriterTaskSynchronizer.hpp"
#include "IOManager/IOMan_ILogIO.hpp"
#include "KernelCommon/DatabaseEvents/KernelDBE_Event.hpp"
#include "hgg01.h" //g01maxuser()

class Log_Volume;
class Trans_Context;

/// info page is written out every ... cycles ("forced" write)
#define LOG_WRITER_INFOPAGEWRITECYCLE 50
 
/// Number of logpages reserved for performing a restart with a full log.
/// This reserve is needed because every successfull restart needs a logpage
/// to write a Savepoint(Restart) Entry on it.
#define LOG_RESTART_RESERVE_SPACE   10

/// value for the IOSequence which is written first of all
#define LOG_INITIAL_IOSEQ (Log_IOSequenceNo(0))
// #define LOG_INITIAL_IOSEQ (Log_IOSequenceNo(0)-5000 )

/*!
   @class  Log_Writer
   @brief  Log writer manager class

   Log_Writer retrieves filled log pages from log queues and writes them out
   to the archive log devices.
   Log_Writer asks Log_Queue for filled log pages to be written
   out to log device.
 */
class Log_Writer
{
public:
    /// vector of log queues (PTS 11135304 mb 2005-05-25)
    typedef Container_Vector<Log_Queue *> QueueVector;

private:
    /// one request of one queue: queueptr, pagecount, pages
    struct QueueRequest
    {
        /// pointer to the queue
        Log_Queue*     pQueue;
        /// number of pages in the queue
        SAPDB_UInt     pageCount;
        /// initializes all member variables
        void Clear()
        {
            pQueue    = 0;
            pageCount = 0;
        }
        /// default constructor of an empty request
        QueueRequest()
        {
            Clear();
        }
        /// constructs an queue-request on a specific queue
        QueueRequest( Log_Queue*     newpQueue,
                      SAPDB_UInt     newpageCount )
        {
            pQueue    = newpQueue;
            pageCount = newpageCount;
        }
    };
    /// requests of several queues
    typedef Container_Vector<QueueRequest> QueueRequests;

public:
    /*!
       @brief Constructor
       @param alloc [in] raw allocator to be used
       @param logqueues [in] Vector of queues that serves this log writer
       @param pagewriter [in] writer of raw log device
       @param writersync [in] writer task synchronization object
       
       The standard events are added here.
     */

    Log_Writer (SAPDBMem_IRawAllocator     &alloc,
                QueueVector                &logqueues,
                IOMan_ILogIO               &pagewriter,
                Log_WriterTaskSynchronizer &writersync);
    /*!
       @brief          Initializes writer structures
       @param          writerioseqno [in] ioseqno of the last log page that has been flushed
       @param          writeiter [in] iterator at position on device where to write
                                 next block
       @param          archivelog pointer the the LogVolume, the writer uses
       @param          infopage [in] log info page handler
       @param          infopageoffs [in] offset of log info page on raw device
       @param          pagevecsize [in] size of the page vector
       @return         (SAPDB_Bool) true if successful

       \li Performs memory allocation of writer structures.
       \li The parameter 'pagevecsize' gives the optimum i/o count of
           page frames to be used by the writer.
       \li 'writeiter' is positioned at the current write position
           on log device.
       \li 'logfullmargin' is a security margin which must be respected
           by the log writer. In normal log operation, the log writer
           may not write to device blocks within a margin of
           'logfullmargin' blocks from the oldest required block.
     */

    SAPDB_Bool Initialize (Log_IOSequenceNo       writerioseqno,
                           Log_RawDeviceIterator  writeiter,
                           Log_Volume            *archivelog,
                           Log_InfoPage           infopage,
                           Log_RawDeviceOffset    infopageoffs,
                           SAPDB_UInt             pagevecsize)
    {
        m_IOSequenceNo         = writerioseqno.IsValid() ? Log_IOSequenceNo(writerioseqno+1)
                                                         : LOG_INITIAL_IOSEQ;
        m_WriteIterator        = writeiter;
        m_ArchiveLog           = archivelog;
        m_LogInfoPage          = infopage;
        m_LogInfoPageOffset    = infopageoffs;
        m_LogUsageEvents.Rescale(UserLogSize()); // PTS 1124876 UH 2003-10-23
        
        if (m_Allocated == false)
        {
            if (!m_QueueRequests.Initialize(pagevecsize)        ||
                !m_RequestPages.Initialize(pagevecsize) ||
                !m_RequestPagesHelper.Initialize(pagevecsize))
            {
                Delete();
                return false;
            }
            m_Allocated = true;
        }
        return true;
    }

    /*!
       @brief          Updates writer state
       @param          taskid [in] id of calling task
       @param          writerioseqno [in] ioseqno to be used for the next page
       @param          writeoffset [in] raw device offset to be used for next i/o
       @param          endoffset [in] raw device end offset
     */

    void SetState (tsp00_TaskId        taskid,
                   Log_IOSequenceNo    writerioseqno,
                   Log_RawDeviceOffset writeoffset,
                   Log_RawDeviceOffset     endoffset)
    {
        m_IOSequenceNo = writerioseqno.IsValid() ? writerioseqno : LOG_INITIAL_IOSEQ;
        m_WriteIterator.SetPosition(writeoffset);
        SetDeviceEnd(endoffset);
    }

    /*!
       @brief          Sets log device end offset
       @param          offset [in] device end offset

       - 'offset' is the raw device offset behind the last block                 to be accessed by the writer.
       - It must be at least as large as the current end offset.
     */
    void SetDeviceEnd (Log_RawDeviceOffset offset)
    {
        if (offset > m_WriteIterator.GetEndOffset())
            m_NewDeviceEndOffset = offset;
    }

    /*!
       @brief          Deletes writer structures
       @return         none
     */
    void Delete() 
    {
        m_QueueRequests.Delete();
        m_RequestPages.Delete();
        m_RequestPagesHelper.Delete();
    }

    /*!
       @brief          This must be used, if a log-recovery is started from init-config
                       and not with a data-recovery.
       @param          newIOSequence the IOSequence, the writer will use when writing the next page.
       @param          newWritePosition the position on the log, where the writing will start
     */
    void SetWritePositionAndIOSeq(Log_IOSequenceNo      newIOSequence,
                                  Log_RawDeviceIterator newWritePosition)
    {
        // PTS 1117079 UH 2002-08-06 do not assign invalid but instead assign 0
        m_IOSequenceNo = newIOSequence.IsValid() ? newIOSequence
                                                 : LOG_INITIAL_IOSEQ;
        m_WriteIterator = newWritePosition;
        m_PagesFreeOnDevice = FreePagesForLogWriter(); // PTS 1129443 mb 2004-05-03
    }
    
    /*!
       @brief          Starts the log writer

       The log writer is requesting log pages for output from the
       associated log queues. Log queues are served in a loop.
     */
    void Run();

    /*!
       @brief Stops the log writer

       After the current i/o, the writer stops execution.
     */
    void Stop()
    {
        m_Active = false;
    }

	/// Returns the current i/o sequence number used by the writer.
    Log_IOSequenceNo GetIOSequenceNo() const
    {
        return m_IOSequenceNo;
    }

    /*!
       @brief Returns the current write offset on the raw device
       This is the position where the next log page will be written.
     */
    Log_RawDeviceOffset GetWriteOffset() const
    {
        return m_WriteIterator.GetPosition();
    }

    /// Returns the current write position as an iterator
    const Log_RawDeviceIterator& GetPosition() const
    {
        return m_WriteIterator;
    }

	/// Returns the number of pages which can be used for writing.
    Log_DeviceBlockCount UserLogSize() const;

	/// Returns the number of pages which are needed for backup or restart.
    Log_DeviceBlockCount UsedPages() const;

    /// Returns the size of pages which were added but are not yet enabled.
    Log_DeviceBlockCount PendingSize() const
    {
        Log_RawDeviceIterator endIter (m_WriteIterator.GetBeginOffset(),
                                       m_WriteIterator.GetEndOffset(),
                                       m_NewDeviceEndOffset);

        return m_NewDeviceEndOffset.IsValid() ? endIter.GetDistanceTo(m_NewDeviceEndOffset) : 0;
    }

    /// returns true if no user can write more redo log
	bool LogIsFull() const
	{
		SAPDBTRACE_METHOD_DEBUG ("Log_Writer::LogIsFull", LogVolume_Trace, 5);
		return UsedPages() >= UserLogSize();
	}
    
    /// returns true, if commit/savepoints are possible. I.E. more than the restart-reserve-pages
    /// are free on the LogVolume, so that the LogWirter would not be suspended
    /// PTS 1130928 mb 2004-08-10
    bool PagesAreFreeForWrite() const
    {
        return FreePagesForLogWriter() > 0;
    }

    /*!
       @brief          returns the number of pages reserved for special purposes
       @param          numPagesInQueues
       @return         the number of pages not writable by normal operation
     */
    Log_DeviceBlockCount CalculateReservedPages(SAPDB_UInt numPagesInQueues)
    {
        m_PagesNotAvailableForUsers = numPagesInQueues + g01maxuser() + 1 + LOG_RESTART_RESERVE_SPACE;
        return m_PagesNotAvailableForUsers; // PTS 1134810 UH 2005-04-12
    }

    /*!
       @brief        Triggers the log save component if enough pages for log save
                     available.  This is called by the TimeOut task.
       @param        trans [in/out]
    */
    void CheckForLogSaveAndSavepoint(Trans_Context &trans);

    /*!
       @brief        The logwriter is suspended until Resume() is called.
                     The returned iosequence is the last used iosequence.
       @param        taskid     [in]
       @param        iosequence [out]
       @return       Returns false if this is called more then by one task.
    */
    // PTS 1114053 UH 2002-10-22 new
    bool SuspendAndGetLastWrittenIOSequence ( tsp00_TaskId            taskid,
                                              Log_IOSequenceNo &iosequence );

    /*!
       @brief     The logwriter is resumed after suspended by
                  SuspendAndGetLastWrittenIOSequence.
    */
    // PTS 1114053 UH 2002-10-22 new
    void ResumeByUser ();

    /// returns true if log writing is suspended
    bool IsSuspendedByUser ()
    {
        return m_SuspendedByUser;
    }
    
    /// adds a new event for the log usage
    bool AddLogFillingEvent (const SAPDB_Int              threshold,
                             const tsp31_event_prio_Param priority)
    {
		// PTS 1124876 UH 2003-10-23 new
        return m_LogUsageEvents.AddEvent (threshold, true, priority, true);
    }

    /// deletes a event for the log usage
    bool DelLogFillingEvent (const SAPDB_Int threshold)
    {
		// PTS 1124876 UH 2003-10-23 new
        return m_LogUsageEvents.DelEvent (threshold, true);
    }

    /// write internal information to the knltrace
    void WriteToTrace (const char * title = 0) const;

private:
    
    /*!
    @brief        Returns the oldest offset on the log device that may not
                  be overwritten by the log writer
    @return       (Log_RawDeviceOffset) 
    */
    Log_RawDeviceOffset GetOldestRequiredOffset() const;

    /// This returns the pagecount which can be used by the logwriter in online mode.
    Log_DeviceBlockCount FreePagesForLogWriter() const;
    
    /*!
    @brief        All pages get there writer iosequence and a timestamp
                  A consistency check is done.
    @param        replacelastpage     [in]
    @param        firstWrittenPosition    [out]  Iterator, which points to the position of 
                                                 the first written page
    */
    void PrepareAndFlushPageVector (SAPDB_Bool             replacelastpage,
                                    Log_RawDeviceIterator &firstWrittenPosition);

    /*!
    @brief        The pages are written with the iomanager to the log devspace.
    @param        pages [in] vector of pages, which shall be flushed 
    */
    void FlushPages (IOMan_LogPages &pages);
    
    /// The added device can now be accessed by the writer.
    void EnableAddedLogDevice();

    /*!
    @brief        Checks if the old cycle is still needed (by backup).
    @return       true, if the new logVolume may be really added now
    */
    bool EnableAddedLogDeviceIsPossible() const
    {
         return m_NewDeviceEndOffset.IsValid()
                &&
                (m_WriteIterator.GetPosition().RawValue()) > (GetOldestRequiredOffset().RawValue());
    }

    /*!
    @brief Checks if pagesFreeOnDevice are enough pages for allowing user-tasks the writing onto the log
    @param pagesFreeOnDevice [in]
    
    pagesFreeOnDevice should not include LOG_RESTART_RESERVE_SPACE.
    It is expected that pagesFreeOnDevice are determined by FreePagesForLogWriter().
    */
    bool EnoughPagesFreeForUserTaskWrite(const Log_DeviceBlockCount pagesFreeOnDevice ) const
    {
        // PTS 1115488 mb 2002-05-03 new
        return pagesFreeOnDevice > m_PagesNotAvailableForUsers;
    }

    /*!
    @brief        Sets the state of the Queue to LogFull, which causes user tasks to be suspended
    @param        allowOnlySavepointCommit [in]
    @param        taskId [in] used for region-access

                  if allowOnlySavePointCommit = TRUE then sets all log_queues
                  to Log_Full, so that only commits, rollbacks and savepoints
                  may be written into the queues until a save_log frees log_pages.
                  Otherwise it opens the Log_Queues for every write operation
                  new for PTS 1115488 mb 2002-05-03
    */
    void SetLogFullQueueState(tsp00_TaskId taskId,
                              bool          allowOnlySavepointCommit);

    /// If m_SuspendedByUser is true this method lets wait until m_SuspendedByUser is false.
    void WaitForUserResume(); // PTS 1114053 UH 2002-10-22 new

    /// This triggers an event if logfull
    void TriggerEvent();

    /// This triggers an event if logsegment full
    void TriggerEvent(const Log_IOSequenceNo &oldestNotSaved);

    /// description: vector of log queues that serve this writer
    QueueVector&                        m_LogQueues;

    /// writes log pages to raw log device
    IOMan_ILogIO&                       m_PageWriter;

    ///  log writer task synchronization object
    Log_WriterTaskSynchronizer&         m_WriterTaskSync;

    ///  indicates whether writer structures have been allocated
    SAPDB_Bool                          m_Allocated;

    ///  indicates whether writer is active
    SAPDB_Bool                          m_Active;

    ///  actual writer i/o sequence number: the last page in i/o has been
    ///  marked with this seqno (m_IOSequenceNo may be invalid if not yet
    ///  any log has been written)
    Log_IOSequenceNo                    m_IOSequenceNo;
    ///  counts frames that have been flushed since info page was flushed
    SAPDB_UInt                          m_FrameCounter;
    ///  contains new device offset (if add log devspace has occurred)
    Log_RawDeviceOffset                     m_NewDeviceEndOffset;

    /// collection of page frames used for write i/o requests
    IOMan_LogPages                      m_RequestPages;
    
    /// another collection of page frames (required for split i/o)
    IOMan_LogPages                      m_RequestPagesHelper;

    ///  queues and pagecounts per queue
    QueueRequests                       m_QueueRequests;

    ///  iterator at actual write offset on raw device
    Log_RawDeviceIterator               m_WriteIterator;

    /// the arhive log handler for this writer.
    Log_Volume                         *m_ArchiveLog;

    /// log infopage handler
    Log_InfoPage                        m_LogInfoPage;

    /// offset of log info page on raw device
    Log_RawDeviceOffset                 m_LogInfoPageOffset;

    /// for checking queue-pages before writing in quick kernel
    Log_IOSequenceNo                    m_CheckQSequence;

    /// for checking queue-pages before writing in quick kernel
    Log_IOSequenceNo                    m_CheckWSequence;
    
    /// for checking queue-pages before writing in quick kernel
    SAPDB_UInt                          m_CheckOffsetCount;
    
    /// number of logpages reserved totally for special purposes
    Log_DeviceBlockCount                m_PagesNotAvailableForUsers;

    /// If this is true, the logwriter must wait until explicitely
    /// resumed by user.  (PTS 1114053 UH 2002-10-22 new)
    bool                                m_SuspendedByUser;

    /// If this is not nil_task_id there is a user waiting
    /// that the logwriter suspends.  (PTS 1114053 UH 2002-10-22 new)
    tsp00_TaskId                        m_UserWaitingForSuspend;    

    ///  holds the last time a savepoint was triggered
    SAPDB_Int                           m_LastSavepointTriggerTime;
    
    ///  event handler for log volume filling
    KernelDBE_EventSet                  m_LogUsageEvents; // PTS 1124876 UH 2003-10-23 added
    
    /// holds the number of currently free pages. If It's zero, then it's calculated
    /// again, because it does not hold an exact value.
    Log_DeviceBlockCount                m_PagesFreeOnDevice;  // PTS 1129443 mb 2004-05-03
    
    /// This is used to send the event on logsegment full
    Log_IOSequenceNo                    m_LastEventOldestNotSaved; // PTS UH 2004-11-17 added

};
#endif // LOG_WRITER_HPP
