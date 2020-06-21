/*!
    @file     Log_VolumeIterator.hpp
    @author   UweH
    @author   MartinB
    @ingroup  Logging
    @brief    This iterator gives access to parts of log entries.

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
#ifndef LOG_VOLUMEITERATOR_HPP
#define LOG_VOLUMEITERATOR_HPP
#include "gsp00.h"   // tsp00_TaskId
#include "IOManager/IOMan_ILogIO.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ISequentialReader.hpp"
/// forward declaration
class Log_FrameAllocator;
/// forward declaration
class Rst_StandbySychronizer;
/*!
    @class Log_VolumeIterator
    @brief This offers an iterator, to access afterimages from the archive log.
 */
class Log_VolumeIterator
{
/// The Log_Volume may be a friend of its own iterator.
friend class Log_Volume;
public:
    /// length of an Log_Entry in a Log-Page
    typedef Log_Page::EntryLength        EntryLength;

    /// Offset of an Log-Entry in a Log_Page
    typedef Log_Page::OffsetType         OffsetType;

    /// Iterator for accessing Log-Entries in a Log_Page
    typedef Log_Page::Iterator           EntryIterator;

    /// Unique number of the write-operation for a log_page
    typedef Log_Page::IOSequenceNo       IOSequenceNo;

    /// unique id of a log-queue associated with a log_page
    typedef Log_Page::QueueID            QueueID;

    /// plain in-memory-structure of a persistent page
    typedef Kernel_Page::PageFrame       PageFrame;
    //
    /// This is the result which may be determined while reading the next logpage from the volume.
    enum CheckLogDeviceResult
    {
        consistent,     ///< the sequence of pages is consistent
        inconsistent,   ///< a logical error in the sequence of pages
        endOfLogDevice  ///< the sequence of log_pages ended
    };

    /// The iterator can become invalid with many different reasons.
    enum IterState
    {	// PTS 1111525 mb 2002-02-20 restore log until: differentiate invalid-reason
        /// iter has valid value
        Iter_ok,
        /// for restoreLogUntil: until-time&date reached
        Iter_untilReached,
        /// everything has been read from this log
        Iter_allRead,
        /// the iterator has not yet been initialized
        Iter_unInitialized
    };
    /// There are several types of iterators on different sources.
    enum IterType
    {
        TapeIterator,           ///< source of pages is a sequence of tapes
        LogVolumeIterator,      ///< source of pages is the log_volume
        IterTypeUninitialized   ///< the iterator is not usable in this state
    };
public:
	/// constructor
    Log_VolumeIterator (tsp00_TaskId                taskId,
                            boolean                *RTECancelFlag,
                            Log_ISequentialReader  *pageReader,
                            Log_IOSequenceNo        lastknown,
                            SAPDBMem_IRawAllocator &alloc,
                            Kernel_Date            &untilDate,    // PTS 1111525 mb 2001-01-15
                            Kernel_Time            &untilTime,    // PTS 1111525 mb 2001-01-15
                            Log_IOSequenceNo       &untilIOSeq,   // PTS 1121539 mb 2003-02-06
                      const IterType                iterType)
    : m_TaskId              (taskId),
      m_PageReader          (pageReader),
      m_Allocator           (alloc),
      m_EntryIter           (m_Page.GetIterator()),
      m_LastKnownIOSequence (lastknown),
      m_UntilDate(untilDate),
      m_UntilTime(untilTime),
      m_UntilIOSeq(untilIOSeq),
      m_State(Iter_unInitialized),
      m_IterType(iterType),
      m_RTECancelFlag(RTECancelFlag)
    {}
	/// there is an io page allocated to read log pages
    void Initialize ();
	/// set the last known iosequence-number
    void SetLastKnownIOSeq (Log_IOSequenceNo lastKnownIOSeq)
    {
        m_LastKnownIOSequence = lastKnownIOSeq;
    }
	/// the resources are released
    void Delete ();
	/// the resources are released
    ~Log_VolumeIterator ();
	/// returns the state of the iterator
    const IterState GetState() const
    {
        // PTS 1111525 mb 2002-02-20 new for restore log until
        if (Iter_ok != m_State)
            return m_State;

        return m_EntryIter.IsValid() ? Iter_ok : Iter_unInitialized;
    }
	/// returns the type of the iterator (either tape or logVolume)
    const IterType GetSourceType() const
    {
        SAPDBERR_ASSERT_STATE(m_IterType != IterTypeUninitialized);
        return m_IterType;
    }
    /*!
        @brief  releases all allocated resources, after that it is invalid.
        @param  state [in] new state of this iterator
     */
    void Invalidate(const IterState state);
	/// dereferences to an entry or a part of an entry.
    SAPDB_Byte* operator * ();
	/// The iterator is set to the next afterimage.
    /// If the end of log is reached, the iterator becomes invalid.
    Log_VolumeIterator& operator ++ ();
	/// This returns the length, which is currently found in the actual page.
    EntryLength Length () const
    {
        SAPDBERR_ASSERT_STATE( GetState() == Iter_ok );
        return m_EntryIter.Length();
    }
	/// The rest length of current entry is given. It may be longer then the current space in page is.
    EntryLength RestLength () const
    {
        SAPDBERR_ASSERT_STATE( GetState() == Iter_ok );
        return m_EntryIter.RestLength();
    }
    /// returns the maximum payload per Log_Page (new with PTS 1128565)
    EntryLength MaxFreeSpace () const
    {
        return m_Page.MaxFreeSpace();
    }
    
	/// The corresponding IOSequence to the current referenced logentry is returned.
    IOSequenceNo GetIOSequence () const
    {
        return m_EntryIter.GetIOSequence();
    }
	/// returns Log_RawDeviceOffset of the current used page (invalid for a tape)
    Log_RawDeviceOffset GetPosition () const
    {
        return m_PageReader->GetPosition();
    }
	/// returns Log_RawDeviceOffset of the current used page (invalid for a tape)
    Log_RawDeviceIterator GetPositionIter () const
    {
        return m_PageReader->GetPositionIter();
    }
	/// This returns the queue id in which this entry was written.
    QueueID GetQueueId () const
    {
        return m_EntryIter.GetQueueId();
    }
    /// returns true, if the entry the iterator points to is spread over
    /// multiple pages and is not the head of the entry.
    bool IsTailOfAnEntry() const
    {
        return m_EntryIter.IsTailOfAnEntry();
    }
	/// The QueueIoSequence of the current log page is returned.
    IOSequenceNo GetQueueIOSequenceNo () const
    {
        return m_EntryIter.GetQueueIOSequenceNo();
	}
	/// This is the timestamp of the last log entry of the current page.
    void GetTimeStamp ( Kernel_Date &date,
                        Kernel_Time &time ) const
    {
        m_EntryIter.GetLastEntryTimestamp (date, time);
    }
    /*!
        @brief writes current position to the trace file.
        format "title: 4711.16 [756/0/76] 310(1350)"

        title      - given by caller
        4711       - offset on log device
        16         - offset in log page
        [756/0/76] - writer io sequence/queue number/queue io sequence
        310        - length of logentry in current page
        1350       - length of logentry in total with 4byte length-field included
     */
    void WriteToTrace (const char* title);
	/// if the criterias are not fullfilled RTE_Crash() is called.
    static CheckLogDeviceResult CheckLogDeviceConsistency
                                          (const Log_Page           &page,
                                           const Log_RawDeviceOffset position,
                                           Log_IOSequenceNo          prevIOSequence,
                                           Log_IOSequenceNo          prevQueueSequence,
                                           Msg_List                 &errlist);
    /// The current entry position is returned
    void GetPosition (Log_RawDeviceOffset &pageoffset,
                      OffsetType          &entryoffset) const
    {
        pageoffset  = m_PageReader->GetPosition();
        entryoffset = m_EntryIter.GetPosition();
    }

    /// checks, if the specified date is reached, sets the state correctly and
    /// returns TRUE, if the date has been reached
    bool UntilDateIsReached(Kernel_Date readDate, Kernel_Time readTime);

protected:
    /// This is used by the ArchiveLog to set the iterator to a position.
    void SetPosition (Log_RawDeviceOffset  pageoffset,
                      OffsetType           entryoffset);
    /// @brief The read-position of the iterator is set to the first entry on the page
    /// @param          pageOffset [in] Adress of the logPage on the LogVolume
    void SetPosition (Log_RawDeviceOffset  pageOffset);
private:
	/// do not allow an assignment operation
    Log_VolumeIterator& operator=(const Log_VolumeIterator& rhs);

    /// checks, if the specified IOsequenceNo is reached, sets the state
    /// correctly and returns TRUE, if the IOSequence has been reached This is
    /// defined inline, because it is called for every increment of the Interator
    inline bool CheckUntilIOSequenceReached();

	/// needed to read pages from log device.
    tsp00_TaskId                   m_TaskId;
	/// handle to read pages sequentially from log device or tape.
    Log_ISequentialReader         *m_PageReader;
	/// raw allocator 
    SAPDBMem_IRawAllocator        &m_Allocator;
	/// the log page to read entries from
    Log_Page                       m_Page;
	/// points to current entry
    EntryIterator                  m_EntryIter;
	/// this determines the highest known iosequence on the logVolume
    Log_IOSequenceNo               m_LastKnownIOSequence;
	/// the date until a restore/restart will be done. This may be changed by Hotstandby
    Kernel_Date                   &m_UntilDate; // PTS 1111525 mb 2001-01-18
	/// the time until a restore/restart will be done. This may be changed by Hotstandby
    Kernel_Time                   &m_UntilTime; // PTS 1111525 mb 2001-01-18
    /// the IOSequence until a restore/restart will be done (inclusive this IOSequence)
    Log_IOSequenceNo              &m_UntilIOSeq;
	/// the current state of this iterator
    IterState                      m_State; // PTS 1111525 mb 2002-02-20
	/// type of the iterator
    IterType                       m_IterType;
	/// reference to the cancel-flag from the TransContext
    boolean                       *m_RTECancelFlag;
};

/*==========================================================================*
*  DEFINITION OF INLINE METHODS                                             *
*==========================================================================*/

inline bool Log_VolumeIterator::CheckUntilIOSequenceReached()
{
    if (m_UntilIOSeq.IsValid())                  // PTS 1121539 2003-06-06
    {
        if (m_UntilIOSeq == m_EntryIter.GetIOSequence()) //PTS 1124727 mb 2003-10-16
        {
            Invalidate(Iter_untilReached);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


#endif // LOG_VOLUMEITERATOR_HPP
