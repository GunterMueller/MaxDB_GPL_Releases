/*!
    @ingroup        Restart
    @file           Rst_LogEntryIterator.hpp
    @author         UweH
    @author         MartinB
    @brief          defines a class to read Log_Entries for the Restart/Recovery
                    from a LogBackup and/or Log_Volume


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
#ifndef RST_LOG_ENTRY_ITERATOR_H
#define RST_LOG_ENTRY_ITERATOR_H
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Page.hpp"
#include "Logging/Log_VolumeIterator.hpp"
#include "Restart/Rst_Exceptions.hpp"
#include "Restart/Rst_Types.hpp"
#include "Restart/Rst_StandbySynchronizer.hpp"

class Msg_List;

/*!
    @brief Forward-Iterator, which reads the logPages necessary for the restart
           Represents one log-entry on a log-device or backup.
 */
class Rst_LogEntryIterator
{
public:
	/// describes the current state of the reader
    typedef Log_VolumeIterator::IterState  IterState;
    /// possible kinds of usage
	/// constructor: only assigns references. Memory Allocation is done by Initizalize 
    Rst_LogEntryIterator(Kernel_Date             &untilDate,
                         Kernel_Time             &untilTime,
                         Log_IOSequenceNo        &untilIOSequence,
                         Log_RawDeviceOffset      lastRedoOffset,
                         Rst_RedoKind             redokind)
        :
        m_untilDate(untilDate),
        m_untilTime(untilTime),
        m_untilIOSequence(untilIOSequence),
        m_standbySynchronizer(lastRedoOffset),
        m_currentIter(NULL),
        m_pAllocator(0),
        m_RTECancelFlag(NULL),
        m_RedoKind(redokind),
        m_finalState(Log_VolumeIterator::Iter_unInitialized)
    {}
    /*!
     *   @brief          Initializer: determines which sub-iterators (tape/log-volume)
     *                  are needed and allocates & sets them properly up
     *   @param         taskId   The TaskId of the redoReader (needed for IO/vkb39)
     *   @param         redoReadTaskAllocator servertask allocator of redo reader
     *   @param         RTECancelFlag  reference to the cancel flag of the Transaction
     *   @param         oldestKnownIOSeq  used for comparisons of Log_IOsequence
     *   @param         errlist used to store messages about errors
     *   @return        true, if no error occured - otherwise false
     */
    bool Initialize (       tsp00_TaskId            taskId,
                            SAPDBMem_IRawAllocator &redoReadTaskAllocator,
                            boolean                *RTECancelFlag,
                      const Log_IOSequenceNo        oldestKnownIOSeq,
                            Msg_List               &errlist);
	/// deallocates dynamic allocated memory of the current sub-iterator
    void DeleteIter();
    /// returns the state of the iterator at the end of the reading
    const IterState GetFinalState() const
    {
        return m_finalState;
    }
	/// returns the current state of the reader 
    const IterState GetState() const
    {
        if (NULL != m_currentIter)
            return m_currentIter->GetState();
        else
            return Log_VolumeIterator::Iter_unInitialized;
    }
    /*!
     *  @brief      	reads the next logEntry for the restart. If an end condition
     *                  evaluates to TRUE then the iterator will stop reading.
     */
    Rst_LogEntryIterator& operator ++ ();
	/// writes the current state into the kernel-trace
    void WriteToTrace(const char * title )
    {
        if ( m_currentIter != 0 )
        {
            Kernel_VTrace() << "Rst_LogEntryIterator::m_currentEntry";
            m_currentIter->WriteToTrace(title);
        }
        else
        {
            Kernel_VTrace() << "Rst_LogEntryIterator::WriteToTrace() internal iterator is invalid.";
        }
    }
	/// sets the current entry paramters
    void SetCurrentReadPosition (const Log_ActionType actiontype )
	{
        SAPDBTRACE_METHOD_DEBUG ("Log_RedoReadTask::SetCurrentReadPosition", LogTrans_Trace, 5);

        m_currentEntryAccess.Lock();
        m_currentEntry.iosequence = m_currentIter->GetIOSequence();
        m_currentIter->GetPosition (m_currentEntry.pageoffset, m_currentEntry.entryoffset);
        m_currentEntry.entrytype  = actiontype;
        m_currentIter->GetTimeStamp (m_currentEntry.entrydate, m_currentEntry.entrytime);
        m_currentEntryAccess.Unlock();
	}
    /// returns the position of the last read entry
    void GetReadPosition ( Log_EntryInfo &entryInfo ) const
	{
        SAPDBTRACE_METHOD_DEBUG ("Log_RedoReadTask::GetReadPosition", LogTrans_Trace, 5);

        m_currentEntryAccess.Lock();
        entryInfo = m_currentEntry;
        m_currentEntryAccess.Unlock();

        if ( LogVolume_Trace.TracesLevel(6) )
        {
            entryInfo.WriteToTrace("GetReadPosition");
            entryInfo.Message ( "GetReadPosition", 0, 0, 0 );
        }

	}

    /*!
     *  @brief  prints an message into the kernel trace protocol
     *  @param  title           title-string to be printed
     *  @param  msgcount        number of entries processed until now
     *  @param  transno         current transaction number
     *  @param  entrysequence   current entrysequence
     */
    void Message (const char *title,
                  SAPDB_UInt  msgcount,
                  SAPDB_UInt  transno,
                  SAPDB_UInt  entrysequence) const
    {
        m_currentEntry.Message(title, msgcount, transno, entrysequence);
    }
    /// appends characteristics to messagelist
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            SAPDB_UInt                 transno,
                            SAPDB_UInt                 entrysequence,
                            SAPDBErr_MessageList      &errlist) const
    {
        m_currentEntry.AppendContextInfo(filename,linenumber,transno, entrysequence, errlist);
    }
    /*!
     * @brief   Returns the offset of the last read-access on the logVolume.
     *          Returns an invalid offset, if nothing had been read yet.
     */
    const Log_RawDeviceOffset GetLastReadOffset () const
    {
        return m_lastReadOffset;
    }
    /*!
     * @brief   Returns the offset of the last read-access on the logVolume.
     *          Returns an invalid offset, if nothing had been read yet.
     */
    const Log_RawDeviceOffset GetCurrentReadOffset () const
    {
        return m_currentIter->GetPosition();
    }
    /*!
     * @brief   Returns the offset of the last read-access on the logVolume.
     *          Returns an invalid offset, if nothing had been read yet.
     */
    const Log_IOSequenceNo GetLastProcessedIOSeq () const
    {
        return m_lastProcessedIOSeq;
    }
	/// Returns a pointer to the data of the entry
    SAPDB_Byte* GetValuePtr() const
    {
        return **m_currentIter;
    }
	/// returns the lebgth of the logEntry
    const Log_Page::EntryLength Length() const
    {
        return m_currentIter->Length();
    }
	/// returns the restlength of the logEntry
    const Log_Page::EntryLength RestLength() const
    {
        return m_currentIter->RestLength();
    }
	/// returns the Log-IOSequence of the entry
    const Log_IOSequenceNo IOSeqNo() const
    {
        return m_currentIter->GetIOSequence();
    }
	/// returns the Queue-IOSequence of the entry
    const Log_IOSequenceNo QueueSeqNo() const
    {
        return m_currentIter->GetQueueIOSequenceNo();
    }
    /// returns the identification of the Queue
    Log_VolumeIterator::QueueID GetQueueId () const
    {
        return m_currentIter->GetQueueId();
    }
    
    /// returns true, if the entry the iterator points to is spread over
    /// multiple pages and is not the head of the entry.
    bool IsTailOfAnEntry() const
    {
        return m_currentIter->IsTailOfAnEntry();
    }

	/// returns the type of redo for wich the iterator had been instantiated
    Rst_RedoKind RedoKind() const
    {
        return m_RedoKind;
    }
    
    /// checks whether the point-in-time for recovery had been reached PTS 1124045 mb 2003-09-08
    bool UntilDateIsReached(Kernel_Date readDate, Kernel_Time readTime);
    
    /************************************************************************************************/
    /*! @name HotStandby                                                                            */
    /*  @{                                                                                          */
    /// resumes the RedoReader, which had been suspended during restart-synchronization
    void ResumeReaderForRestart();
    
    /// suspends the RedoReader during a takeover 
    void SuspendRedoReader(tsp00_TaskId     taskId);

    /// Sets the offset of the last readable page for a standby
    void SetLastRedoReadOffset(Log_RawDeviceOffset lastRedoOffset, tsp00_TaskId     taskId);

    
    /// checks if the entry, to which the iterator points can be read completely whithout
    /// beeing suspended. If This is not the case, the ReaderTask will be suspended.
    /// PTS 1128565 mb 2004-03-17
    void CheckSuspendForStandby(tsp00_TaskId     taskId)
    {
        // the entry is split among multiple pages
        if (RestLength() > Length() && m_standbySynchronizer.GetLastReadableOffset().IsValid())
        {
            Log_Page::EntryLength   lengthToBeRead = RestLength() - Length();
            Log_DeviceBlockCount    numPagesToReadContinuous = (lengthToBeRead - 1) / 
                                                               (m_currentIter->MaxFreeSpace()-sizeof(Log_Page::EntryLength)) + 1;
    
            m_standbySynchronizer.CheckSuspendForStandby(taskId,
                                                         m_currentIter->GetPositionIter(),
                                                         numPagesToReadContinuous);
        }
    }
    /*  @}                                                                                          */
    
private:

    /// m_currentEntry is set after a logentry is read and before it is handled.  Handled means copied, etc.
    Log_EntryInfo                    m_currentEntry;
	/// This synchronises the access to m_currentEntry.
    mutable RTESync_Spinlock         m_currentEntryAccess;
    /// offset of the last successfull read. Invalid if the log_volume had not been accessed yet
    Log_RawDeviceOffset         m_lastReadOffset;
	/// last successfull processed IOSequence
    Log_IOSequenceNo            m_lastProcessedIOSeq;
	/// current iterator for reading log-entries either a tape-iter or a log-volume-iterator)
    Log_VolumeIterator         *m_currentIter;
    /// allocator which is used for all allocate/deallocate of the logvolume or logtape iterator
    SAPDBMem_IRawAllocator     *m_pAllocator;
	/// if this date is reached, the redo is stopped
    Kernel_Date                &m_untilDate;
	/// if this time is reached, the redo is stopped
    Kernel_Time                &m_untilTime;
	/// if this time is reached, the redo is stopped
    Log_IOSequenceNo           &m_untilIOSequence;
    /// For hot stand by this is the last readable offset on the device.
    Rst_StandbySynchronizer     m_standbySynchronizer;
    /// pointer to the cancel-flag from the TransContext
    boolean                    *m_RTECancelFlag;
	/// taskid of the readerTask
    tsp00_TaskId                m_taskId;
    /// kind of usage
    Rst_RedoKind                m_RedoKind;
    /// The oldest known IOSequence - used for comparisons between iosequences
    Log_IOSequenceNo            m_oldestKnownIOSeq;
    /// The state of the read-iterator at the end of the reading
    IterState                   m_finalState;
};
#endif  /* RST_LOG_ENTRY_ITERATOR_H */
