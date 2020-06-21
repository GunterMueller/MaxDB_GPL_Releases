/*!
  @file           Log_Page.hpp
  @author         UweH
  @ingroup        Logging
  @brief          Handler for archive log pages

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



#ifndef LOG_PAGE_HPP
#define LOG_PAGE_HPP


#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_Page74.hpp"
#include "DataAccess/Data_Types.hpp"
#include "Logging/Log_Types.hpp"

///  alignment for log page entries
#define LOG_PAGE_ALIGNMENT          4

/// the index of the first free offset
#define LOG_PAGE_FIRST_OFFSET       1

/// the value for a continued entry 
#define LOG_PAGE_CONTINUED_ENTRY    1
/*!
    @brief          Archive log page handler class (concrete class)
 */

class Log_Page : public Kernel_Page74
{

public:
    
    /// i/o sequence number of log pages
    typedef Log_IOSequenceNo                    IOSequenceNo;
    
    /// identifier for log queues
    typedef Log_QueueID                         QueueID;
    
    /// number of times a log page has been written to device
    typedef SAPDB_UInt1                         OffsetCount;
    
    /// length of a log page entry
    typedef SAPDB_UInt4                         EntryLength;
    
    /// Default constructor
    Log_Page();

    /*!
       @brief          Constructor; assigns frame

       \li  With this constructor, a frame is assigned which contains 
            an initialized or filled log page.
       \li  Does not change frame contents.
     */

    Log_Page
        (const PageFrame&               frame);
   
    /*!
       @brief          Constructor; assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          quid [in] queue id number
       @param          quseqno [in] queue i/o sequence number
     */
    Log_Page
        (const PageFrame&               frame, 
         QueueID                        quid,    
         IOSequenceNo                   quseqno);
   
    /*!
       @brief          might check page
       @return         none
     */
    ~Log_Page();

    /*!
       @brief          Assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          quid [in] queue id number
       @param          quseqno [in] queue i/o sequence number
     */
    void SetAndInitializeFrame
        (const PageFrame&               frame, 
         QueueID                        quid,    
         IOSequenceNo                   quseqno);

    /*!
       @brief          Initializes frame contents
       @param          quid [in] queue id number
       @param          quseqno [in] queue i/o sequence number
     */
    void InitializeFrame
        (QueueID                        quid,    
         IOSequenceNo                   quseqno);

    /// Dumps frame contents to destination frame
    bool CopyFrameTo ( Kernel_Page &dest ) const
    {
        return dest.GetFrame().Copy(m_Frame);
    }

    /*!
       @brief          Clears the page

       @li      After this operation, the page is considered empty.
       @li      Queue i/o sequence number and i/o sequence number are invalidated. First entry position is reset.
     */
    void Clear();

    /*!
       @brief          checks the page
       @return         true, if everything checked on the page is ok.
     */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const;
    
    /*!
       @brief          Resets the page
       @param          quseqno [in] queue i/o sequence number

       \li After this operation, the page is considered empty.
       \li A new queue i/o sequence number is set and the                  i/o sequence number is invalidated.                 First entry position is reset.
     */
    void Reset
        (IOSequenceNo                   quseqno);

    /*!
       @brief          returns true of no logentry was written into the page.
     */
    inline bool IsEmpty() const;

    /*!
       @brief          Writes log writer i/o sequence number to frame
       @param          ioseqno [in] i/o sequence number to be written. 
     */
    void WriteWriterIOSequenceNo
        (Log_IOSequenceNo                   ioseqno);

    /*!
       @brief          Reads log writer i/o sequence number from frame
     */
    IOSequenceNo ReadFirstWriterIOSequenceNo() const;

    /*!
       @brief          Reads log writer i/o sequence number from frame
     */

    IOSequenceNo ReadLastWriterIOSequenceNo() const;
         
    /*!
       @brief          Checks whether the specified IOSequence is located in this page. This
                       method takes care of a iosequence-overflow in the page
                       (PTS 1124727 mb 2003-10-16)
     */
    bool IOSequenceIsOnPage(Log_IOSequenceNo ioSeq) const;

    /*!
       @brief          Writes the date-stamp into the logpage
     */
    void WriteDate (Kernel_Date date);

    /*!
       @brief          Reads the date-stamp from the logpage
     */
    Kernel_Date ReadDate() const;

    /*!
       @brief          Writes the time-stamp into the logpage
     */
    void WriteTime (Kernel_Time time);

    /*!
       @brief          Reads the reads-stamp from the logpage
     */
    Kernel_Time ReadTime() const;

    /*!
       @brief          Writes queue i/o sequence number to frame
       @param          quseqno [in] queue i/o sequence number to be written. 
     */
    void WriteQueueIOSequenceNo
        (Log_IOSequenceNo                   quseqno);

    /*!
       @brief          Reads queue i/o sequence number from frame
     */
    IOSequenceNo ReadQueueIOSequenceNo() const;

    /*!
       @brief          Writes queue id to frame
       @param          quid [in] queue id to be written
     */
    void WriteQueueID
        (QueueID                        quid);

    /*!
       @brief          Reads queue id from frame
     */
    QueueID ReadQueueID() const;

    /*!
       @brief          Reads number of offsets stored in page trailer from frame
       @return         (OffsetType) actual write position
     */
    OffsetCount ReadOffsetCount() const;
    /*!
       @brief          Reads the flag whether the first entry on this page is a continued entry (PTS 1131467)
       @return         true, if the first entry is a continuation-entry
     */
    SAPDB_Bool FirstEntryIncomplete() const;

    /*!
       @brief          Reads offset of specified offset number from frame
       @param          offsetno [in] number between 1 and ReadOffsetCount()
       @return         (OffsetType) entry offset
     */
    OffsetType ReadOffsetOnPage
        (SAPDB_UInt                     offsetno) const;

    /*!
       @brief          Writes offset of actual write position to frame
       @param          offset [in] actual offset to be written
     */
    void WriteFirstFreeOffset
        (OffsetType                     offset);

    /*!
       @brief          Reads offset of actual write position from frame
       @return         (OffsetType) actual write position
     */
    OffsetType ReadFirstFreeOffset() const;

    /*!
       @brief          Checks whether there is at least specified space on the page
       @param          size [in] requested space in bytes
       @return         (SAPDB_Bool) true if space available
     */
    SAPDB_Bool HasFreeSpaceAvailable
        (SizeType                       size) const;

    /*!
       @brief          Calculates free space that can be reserved "en block".
       @param          currentoffset If greater than 0 then the space is
                       calculated from that offset on.
       @return         (SizeType) free space
     */

    SizeType FreeSpaceAvailable(OffsetType currentoffset = 0) const;

    /*!
       @brief          Returns the maximum free space that can be reserved on an 
                       empty log page
       @return         (SizeType) max. free space
     */
    SizeType MaxFreeSpace() const;

    /*!
       @brief      Returns size of page header in bytes
                   This is static function which is used in
                   admin-mode, when no Log_Page is accessible
    */
    static SizeType MinSpaceOffset() ;

    /*!
       @brief          Reserves a specified amount of space on a page and returns
              pointer to it
       @param          restsize [in] this size is needed
       @param          reservesize [in] this size is only reserved
       @return         (SAPDB_Byte*) pointer to reserved space
     */

    SAPDB_Byte* Reserve
        (SizeType                       restsize,
         SizeType                       reservesize);

    /*!
       @brief          Returns the page alignment in bytes
       @return         (SizeType) page alignment

       - Space is reserved with the returned alignment.
     */

    static SizeType Alignment();

    /*!
        @brief  Increments i/o write count and writes it to frame
    */
    void IncOffsetCount();

    /*!
    	@brief  Sets the flag m_ContinuedEntry to true (PTS 1131467)
    */
    void SetContinuedEntry();

    /*!
        @brief  Returns size of the frame in bytes
        @return (SizeType) frame size
    */
    SizeType GetLength() const;

    /*!
        @brief  truncates the entries on the logpage until the given iosequence 
                is reached. If the given IOSequence is lower than the lowest
                IOSequence on this page, then the whole page is cleared.
    */
    void Truncate (Log_IOSequenceNo newLastIOSeq);

    /*!
        @brief  dumps the LogPage to the trace
    */
    void WriteToTrace() const;

    /// some important information of the page is appended to the messagelist 
    virtual void AppendContextInfo (SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;

    /*!
       @brief          Dereferences parts of split spaces in log pages.
     */

    class Iterator
    {
    /// necessary to access internals in a page
    friend class Log_Page;
    public:
        
        /// TRUE, if it can be dereferenced.
        bool IsValid() const
        {
            if ( 0 == m_Page )
                return false;
            if ( ! m_Page->IsAssigned() )
                return false;
            return m_Page->OffsetIsValid(m_Offset);
        }
        
        /// resets all internal members. IsValid will return false.
        void Invalidate()
        {
            m_Offset = 0;
            m_Page   = 0;
        }
        
        /// This returns the entry, which is currently referenced.
        SAPDB_Byte* operator * ()
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            return m_Page->GetEntry(m_Offset);
        }
        
        /// This sets the iterator to the next entry in page.
        Iterator& operator ++ ()
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            m_Offset = m_Page->GetOffsetOfNextEntry(m_Offset);
            if ( ! m_Page->OffsetIsValid (m_Offset) )
                Invalidate();
            return *this;
        }
        
        /// Returns the Length of current part of the Split record.
        /// This is the length, which is currently found in the actual page.
        EntryLength Length () const
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            return m_Page->GetEntryLength(m_Offset);
        }
        
        /// The rest length of current entry is given. It may be longer then
        ///  the current space in page is.
        EntryLength RestLength () const
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            return m_Page->GetRestEntryLength (m_Offset);
        }
        
        /// returns true, if the entry the iterator points to is spread over
        /// multiple pages and is not the head of the entry.
        bool IsTailOfAnEntry () const
        {
            return     m_Page->FirstEntryIncomplete() 
                   && (GetPosition() == Log_Page::MinSpaceOffset());
        }
         
        /// The corresponding IOSequence to the current referenced
        /// logentry is returned. If IsValid() is false, the invalid
        /// iosequence is returned.
        IOSequenceNo GetIOSequence () const
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            SAPDBERR_ASSERT_STATE( ((Length() + sizeof(EntryLength)) & 0xffff0000) == 0);

            return m_Page->GetIOSequence (m_Offset+OffsetType(Length() + sizeof(EntryLength)));
        }
        
        /// This is the queue id in which this entry was written.
        QueueID GetQueueId () const
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            return m_Page->ReadQueueID();
        }
        
        /// The QueueIOSequence is returned.
        IOSequenceNo GetQueueIOSequenceNo () const
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            return m_Page->ReadQueueIOSequenceNo();
        }

        /// At this timestamp the last log entry in this log page was written.
        void GetLastEntryTimestamp ( Kernel_Date &date,
                                     Kernel_Time &time ) const
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            date = m_Page->ReadDate();
            time = m_Page->ReadTime();
        }
        
        /// This time is the date of the last log entry in this log page.
        Kernel_Date GetLastEntryDate () const
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            return m_Page->ReadDate();
        }
        
        /// This returns the current offset of the iterator.
        OffsetType GetPosition () const
        {
            return m_Offset;
        }
        
        /// This is sets the iterator to a specific offset in the page.
        void SetPosition (OffsetType offset)
        {
            m_Offset = offset;
        }
        
        /// This is sets the iterator to a new page.
        void SetPage (Log_Page   &Page)
        {
            m_Page   = &Page;
        }

    protected: 

        /// may only be used by the Log_Page
        Iterator ()
        : m_Offset (0),
          m_Page   (0)
        {}

        /// may only be used by the Log_Page
        Iterator (Log_Page &Page,
                  OffsetType      Offset)
        : m_Offset (Offset),
          m_Page   (&Page)
        {}

        /// may only be used by the Log_Page
        Iterator (Log_Page &Page)
        : m_Offset (0),
          m_Page   (&Page)
        {}

    private: 

        /// The Logpage used to reference entries in it.
        Log_Page *m_Page;

        /// This refers to the current record: (m_pFrame + m_RecordOffset)
        OffsetType m_Offset;
    };
    
    /*!
        @brief      Returns iterator
    */
    Iterator GetIterator()
    {
        return Iterator (*this);
    }

    /*!
       @brief          Returns iterator on first entry on page
     */
    void Begin (Iterator &iter)
    {
        iter.SetPage(*this);
        iter.SetPosition ( CalcNearestAlignedSize(GetHeaderSize()) );
    }

    /*!
       @brief          Returns the offset of the next entry in the page seen
                       from the given offset.
                       If the returned offset is invalid for current page,
                       the next entry or the next part of current entry is found
                       in the following log page.
     */
    OffsetType GetOffsetOfNextEntry(OffsetType entryOffset)
    {
        SAPDBERR_ASSERT_STATE( ((GetRestEntryLength(entryOffset) + sizeof(EntryLength)) & 0xffff0000) == 0);

        return entryOffset + CalcNearestAlignedSize
                             (Log_Page::SizeType(GetRestEntryLength(entryOffset)+sizeof(EntryLength)));
    }
    /*!
       @brief          Returns true, if the offset in the valid range.
     */
    SAPDB_Bool OffsetIsValid (OffsetType offset) const
    {
        return offset >= CalcNearestAlignedSize(GetHeaderSize())
               &&
               offset < ReadFirstFreeOffset();
    }
    /*!
       @brief          Returns the length of the entry part in this page.
     */
    EntryLength GetEntryLength(OffsetType offset) const
    {
        const EntryLength restlen         = GetRestEntryLength(offset);
        const OffsetType  firstFreeOffset = ReadFirstFreeOffset();
        if ( offset + restlen + sizeof(EntryLength) <= firstFreeOffset ) 
            return restlen;
        else
            return firstFreeOffset - offset - sizeof(EntryLength);
    }
    /*!
       @brief          Returns the length of all entry parts from this one on.
     */
    EntryLength GetRestEntryLength(OffsetType offset) const
    {
        SAPDBERR_ASSERT_STATE( OffsetIsValid(offset) );
        return *(reinterpret_cast<const EntryLength*>(m_Frame.GetPointer(offset,sizeof(EntryLength))))
               - sizeof(EntryLength);
    }

    /*!
       @brief          Returns a readable pointer to the space referenced by offset.
     */
    SAPDB_Byte* GetEntry(OffsetType offset)
    {
        SAPDBERR_ASSERT_STATE( OffsetIsValid(offset) );
        return reinterpret_cast<SAPDB_Byte*>(m_Frame.GetPointer(offset,sizeof(SAPDB_Byte)))
               + sizeof(EntryLength);
    }
    /*!
       @brief          The corresponding IOSequence to the current referenced
                       logentry is returned. If IsValid() is false, the invalid
                       iosequence is returned.
     */
    IOSequenceNo GetIOSequence (OffsetType offset) const
    {
    
        OffsetType firstFreeOffset = ReadFirstFreeOffset();
        
        if ( offset < CalcNearestAlignedSize(GetHeaderSize())
             ||
             offset > firstFreeOffset )
            return IOSequenceNo();

        if ( offset == firstFreeOffset )
            return ReadLastWriterIOSequenceNo(); // for continued entries

        const OffsetCount maxoffsetcount = ReadOffsetCount();

        SAPDB_UInt        offsetno       = LOG_PAGE_FIRST_OFFSET;
        OffsetType        testoffset     = ReadOffsetOnPage (offsetno);
        
        while ( offset > testoffset && offsetno <= maxoffsetcount )
        {
            ++offsetno;
            testoffset = ReadOffsetOnPage (offsetno);
        }
        SAPDBERR_ASSERT_STATE( offsetno <= maxoffsetcount );
        
		// PTS 1120201 UH 2003-02-17
        const IOSequenceNo entrySequence = ReadFirstWriterIOSequenceNo() + (offsetno - LOG_PAGE_FIRST_OFFSET);
        
        return entrySequence.IsValid() ? entrySequence : ++(IOSequenceNo(entrySequence));
    }

protected:

    /// the header of each logpage
    struct PersistentHeader
    {
        Kernel_Page74::PersistentHeader m_KernelPage74Header;   ///< standard header
        SAPDB_UInt4 m_LastFlushDate;        ///< data of the last flush of the page
        SAPDB_UInt4 m_LastFlushTime;        ///< time of the last flush of the page
        SAPDB_UInt4 m_QueueIOSequenceNo;    ///< SequenceNo in the queue
        SAPDB_UInt2 m_QueueID;              ///< Id of the Queue in which the Page had been
        SAPDB_UInt1 m_OffsetCount;          ///< number of offsets
        SAPDB_UInt1 m_ContinuedEntry;       ///< lowest bit set, if the first Entry on this page is a conituation entry (PTS 1131467)
    };

private:

    /// Compile time size check for persistent header
    typedef SAPDB_Persistent < PersistentHeader, PersistentHeader,
          sizeof(Kernel_Page74::PersistentHeader) +
          sizeof(SAPDB_UInt4) +
          sizeof(SAPDB_UInt4) +
          sizeof(SAPDB_UInt4) +
          sizeof(SAPDB_UInt2) +
          sizeof(SAPDB_UInt1) +
          sizeof(SAPDB_UInt1) >        CheckedPersistentHeader;

    /// the trailer remains the same as in the Kernel_Page
    typedef Kernel_Page74::PersistentTrailer   PersistentTrailer;

    ///  Returns pointer to page header of the frame
    PersistentHeader* PersistentHeaderPtr();

    ///  Returns pointer to page header of the frame
    const PersistentHeader* PersistentHeaderPtr() const;

    ///  Returns pointer to page trailer of the frame
    SAPDB_Byte* PersistentTrailerPtr();

    ///  Returns pointer to page trailer of the frame
    const SAPDB_Byte* PersistentTrailerPtr() const;

protected:

    ///  Returns size of page header in bytes
    SizeType GetHeaderSize() const;

    /// Returns size of page trailer in bytes 
    /// @attention trailer size is not fixed!
    SizeType GetTrailerSize() const;

    ///  Sets i/o write count to zero and writes it to frame
    void ResetOffsetCount();
    
    /// Sets the flag m_ContinuedEntry to false (PTS 1131467)
    void ResetContinuedEntry();

    ///  Calculates aligned offset on the page next to given offset. 
    ///  The returned offset is greater or equal 'offset'.
    OffsetType CalcNearestAlignedOffset
        (OffsetType                     offset) const;

    ///  Calculates aligned size for a given size
    ///  The returned offset is greater or equal 'size'.
    SizeType CalcNearestAlignedSize
        (SizeType                       size) const;

    ///  Calculates max. aligned offset on the page for append operations
    OffsetType MaxAlignedOffset() const;

};


//------------------------------------------------------------------------------

inline Log_Page::Log_Page()
{}

//------------------------------------------------------------------------------

inline Log_Page::Log_Page
    (const PageFrame&               frame)
    : 
    Kernel_Page74(frame)
{}

//------------------------------------------------------------------------------

inline Log_Page::Log_Page
    (const PageFrame&               frame, 
     QueueID                        quid,    
     IOSequenceNo                   quseqno)
    : 
     Kernel_Page74(frame)
{
    InitializeFrame(quid, quseqno);
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::SetAndInitializeFrame
    (const PageFrame&               frame, 
     QueueID                        quid,    
     IOSequenceNo                   quseqno)
{
    SetFrame(frame);
    InitializeFrame(quid, quseqno);
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::InitializeFrame
    (QueueID                        quid,    
     IOSequenceNo                   quseqno)
{
    Kernel_Page74::InitializeFrame (Kernel_LogPageType);
    WriteQueueID                   (quid);
    Reset(quseqno);
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::Clear()
{
    WriteQueueIOSequenceNo (IOSequenceNo());
    WriteWriterIOSequenceNo(IOSequenceNo());
    WriteDate              (0);
    WriteTime              (0);
    ResetOffsetCount       ();
    ResetContinuedEntry    ();
}

//------------------------------------------------------------------------------

inline bool
    Log_Page::IsEmpty() const
{
    return ReadFirstFreeOffset() == CalcNearestAlignedOffset(GetHeaderSize());
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::Reset
    (IOSequenceNo                   quseqno)
{
    WriteQueueIOSequenceNo (quseqno);
    WriteWriterIOSequenceNo(IOSequenceNo());
    WriteDate              (0);
    WriteTime              (0);
    ResetOffsetCount       ();
    ResetContinuedEntry    ();
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::WriteWriterIOSequenceNo
    (Log_IOSequenceNo                   ioseqno)
{
    WritePageId(ioseqno.RawValue());
}

//------------------------------------------------------------------------------

inline Log_Page::IOSequenceNo 
    Log_Page::ReadLastWriterIOSequenceNo() const
{
    return  Log_IOSequenceNo(ReadPageId());
}

//------------------------------------------------------------------------------

inline Log_Page::IOSequenceNo 
    Log_Page::ReadFirstWriterIOSequenceNo() const
{
	// PTS 1120201 UH 2003-02-17
    IOSequenceNo lastIOSequence = ReadLastWriterIOSequenceNo();

    if ( lastIOSequence.IsInvalid() )
        return lastIOSequence;

    // PTS 1124727 mb 2003-10-27 overflow-handling now in class Log_IOSequenceNo
    return lastIOSequence - (ReadOffsetCount() - LOG_PAGE_FIRST_OFFSET);
}

//------------------------------------------------------------------------------
inline bool Log_Page::IOSequenceIsOnPage(Log_IOSequenceNo ioSeq) const
{
    if (ReadLastWriterIOSequenceNo().IsSmallerWithoutOverflowCheck(ReadFirstWriterIOSequenceNo()))
    // there has been a iosequence-overflow on this page 
    {
        return (  ReadFirstWriterIOSequenceNo().IsSmallerEqualWithoutOverflowCheck(ioSeq)
                  &&
                  ioSeq.IsSmallerWithoutOverflowCheck(SAPDB_MAX_UINT4))
               ||
                  ReadLastWriterIOSequenceNo().IsSmallerEqualWithoutOverflowCheck(ioSeq);
    }
    else
    {
        return ReadFirstWriterIOSequenceNo().IsSmallerEqualWithoutOverflowCheck(ioSeq)
               &&
               ioSeq.IsSmallerEqualWithoutOverflowCheck( ReadLastWriterIOSequenceNo());
    }
}

//------------------------------------------------------------------------------
inline void Log_Page::WriteDate (Kernel_Date date)
{
    PersistentHeaderPtr()->m_LastFlushDate = date;
}

//------------------------------------------------------------------------------
inline Kernel_Date Log_Page::ReadDate() const
{
    return PersistentHeaderPtr()->m_LastFlushDate;
}

//------------------------------------------------------------------------------
inline void Log_Page::WriteTime (Kernel_Time time)
{
    PersistentHeaderPtr()->m_LastFlushTime = time;
}

//------------------------------------------------------------------------------
inline Kernel_Time Log_Page::ReadTime() const
{
    return PersistentHeaderPtr()->m_LastFlushTime;
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::WriteQueueIOSequenceNo
    (Log_IOSequenceNo                   quseqno)
{
    PersistentHeaderPtr()->m_QueueIOSequenceNo = quseqno.RawValue();
}

//------------------------------------------------------------------------------

inline Log_Page::IOSequenceNo
    Log_Page::ReadQueueIOSequenceNo() const
{
    return IOSequenceNo(PersistentHeaderPtr()->m_QueueIOSequenceNo);
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::WriteQueueID
    (QueueID                        quid)
{
    PersistentHeaderPtr()->m_QueueID = quid;
}

//------------------------------------------------------------------------------

inline Log_Page::QueueID 
    Log_Page::ReadQueueID() const
{
    return PersistentHeaderPtr()->m_QueueID;
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::IncOffsetCount()
{
    OffsetType actoffset = ReadFirstFreeOffset();
    OffsetCount count = ReadOffsetCount();
    PersistentHeaderPtr()->m_OffsetCount = count+1;
    WriteFirstFreeOffset(actoffset);
}

//------------------------------------------------------------------------------

inline Log_Page::OffsetCount 
    Log_Page::ReadOffsetCount() const
{
    return PersistentHeaderPtr()->m_OffsetCount;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool 
    Log_Page::FirstEntryIncomplete() const
{
    return ((PersistentHeaderPtr()->m_ContinuedEntry & LOG_PAGE_CONTINUED_ENTRY) != 0);
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::SetContinuedEntry()
{
    PersistentHeaderPtr()->m_ContinuedEntry = PersistentHeaderPtr()->m_ContinuedEntry  | LOG_PAGE_CONTINUED_ENTRY;
}
//------------------------------------------------------------------------------

inline void 
    Log_Page::ResetContinuedEntry()
{
    PersistentHeaderPtr()->m_ContinuedEntry = PersistentHeaderPtr()->m_ContinuedEntry -
                                              (PersistentHeaderPtr()->m_ContinuedEntry  & LOG_PAGE_CONTINUED_ENTRY);
}

//------------------------------------------------------------------------------

inline Log_Page::OffsetType 
    Log_Page::ReadOffsetOnPage
    (SAPDB_UInt                     offsetno) const
{
    SAPDBERR_ASSERT_STATE(((    m_Frame.GetLength()
                              - sizeof(PersistentTrailer) 
                              - offsetno * sizeof(OffsetType)
                            ) & 0xFFFF0000)
                            == 0);
    
    OffsetType offs = OffsetType (    m_Frame.GetLength()
                                    - sizeof(PersistentTrailer) 
                                    - offsetno * sizeof(OffsetType));

    const OffsetType* offsptr = reinterpret_cast<const OffsetType*>
                                    (m_Frame.GetPointer(offs, sizeof(OffsetType)));

    return *offsptr;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool 
    Log_Page::HasFreeSpaceAvailable
    (SizeType                       size) const
{
    return FreeSpaceAvailable() >= size;
}

//------------------------------------------------------------------------------

inline Log_Page::SizeType 
    Log_Page::FreeSpaceAvailable(OffsetType currentoffset) const
{
    if ( currentoffset == 0 )
        currentoffset = ReadFirstFreeOffset();
    OffsetType firstfreeoffs = currentoffset + sizeof(EntryLength);

    SAPDBERR_ASSERT_STATE(((   m_Frame.GetLength()
                             - GetTrailerSize()
                             - sizeof(OffsetType))
                            & 0xFFFF0000)
                           ==0);
    OffsetType traileroffs   = m_Frame.GetLength() - GetTrailerSize()
                               - (OffsetType)sizeof(OffsetType);         //PTS 1111869 MB 01-09-20
                               
                               // conversion from 'size_t' to 'Data_PageFrame::OffsetType', 
                               
                               // the offset list may be incremented before writing
                               // so we must be pessimistic here
    
    return (firstfreeoffs >= traileroffs) ?
            0 : traileroffs - firstfreeoffs;
}

//------------------------------------------------------------------------------

inline Log_Page::SizeType 
    Log_Page::MaxFreeSpace() const
{
    return FreeSpaceAvailable(GetHeaderSize());
}

//------------------------------------------------------------------------------

inline SAPDB_Byte* Log_Page::Reserve (SizeType restsize,
                                      SizeType reservesize)
{
    SAPDBERR_ASSERT_STATE(HasFreeSpaceAvailable(reservesize));

    SizeType   internReserve = reservesize + sizeof (EntryLength);
    SizeType   lengthval     = restsize + sizeof (EntryLength);
    OffsetType actoffset     = ReadFirstFreeOffset();

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "restsize: "<<restsize<<", reservesize: "<<reservesize);
    
    if ( restsize == reservesize )
    {
        // only try to align, if the requested size fits into one page
        internReserve = CalcNearestAlignedSize(internReserve);
        lengthval     = lengthval;
    }

    WriteFirstFreeOffset(actoffset + internReserve);

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "actoffset: "<<actoffset<<" => "<<actoffset+internReserve);

    SAPDB_Byte* space = m_Frame.GetPointer(actoffset, internReserve);
    *(reinterpret_cast<EntryLength*> (space)) = lengthval;
    return space + sizeof (EntryLength);
}

//------------------------------------------------------------------------------

inline Log_Page::SizeType 
    Log_Page::Alignment()
{
    return LOG_PAGE_ALIGNMENT;
}

//------------------------------------------------------------------------------

inline Log_Page::PersistentHeader* 
    Log_Page::PersistentHeaderPtr()
{
    return reinterpret_cast<PersistentHeader*>(m_Frame.GetDataPtr());
}

//------------------------------------------------------------------------------

inline const Log_Page::PersistentHeader* 
    Log_Page::PersistentHeaderPtr() const
{
    return reinterpret_cast<const PersistentHeader*>(m_Frame.GetDataPtr());
}

//------------------------------------------------------------------------------

inline SAPDB_Byte* 
    Log_Page::PersistentTrailerPtr()
{
    return m_Frame.GetPointer(m_Frame.GetLength() - GetTrailerSize());
}

//------------------------------------------------------------------------------

inline const SAPDB_Byte* 
    Log_Page::PersistentTrailerPtr() const
{
    return m_Frame.GetPointer(m_Frame.GetLength() - GetTrailerSize());
}

//------------------------------------------------------------------------------

inline Log_Page::SizeType 
    Log_Page::GetHeaderSize() const
{
    return sizeof(PersistentHeader);
}

inline Log_Page::SizeType  
Log_Page::MinSpaceOffset() 
{
    return sizeof(PersistentHeader);
}

//------------------------------------------------------------------------------

inline Log_Page::SizeType 
    Log_Page::GetTrailerSize() const
{
    return 
        sizeof(PersistentTrailer) 
        + (ReadOffsetCount()) * sizeof(OffsetType);
}

//------------------------------------------------------------------------------

inline Log_Page::SizeType 
    Log_Page::GetLength() const
{
    return m_Frame.GetLength();
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::WriteFirstFreeOffset
    (OffsetType                     offset)
{
    OffsetType* offsptr = reinterpret_cast<OffsetType*>(PersistentTrailerPtr());
    *offsptr = offset;
}

//------------------------------------------------------------------------------

inline Log_Page::OffsetType 
    Log_Page::ReadFirstFreeOffset() const
{
    const OffsetType* offsptr = reinterpret_cast<const OffsetType*>(PersistentTrailerPtr());
    return *offsptr;
}

//------------------------------------------------------------------------------

inline void 
    Log_Page::ResetOffsetCount()
{
    PersistentHeaderPtr()->m_OffsetCount = LOG_PAGE_FIRST_OFFSET;
    WriteFirstFreeOffset(CalcNearestAlignedOffset(GetHeaderSize()));
}

//------------------------------------------------------------------------------

inline Log_Page::OffsetType 
    Log_Page::CalcNearestAlignedOffset
    (OffsetType                     offset) const
{
#if LOG_PAGE_ALIGNMENT != 4
#error must adapt alignment in Log_Page::CalcNearestAlignedOffset()
#endif

    // 4 byte aligned offset:
    return ((offset + LOG_PAGE_ALIGNMENT - 1) >> 2) << 2;

    // 8 byte aligned offset:
    // return ((offset + 7) >> 3) << 3;
}

//------------------------------------------------------------------------------

inline Log_Page::SizeType 
    Log_Page::CalcNearestAlignedSize
    (SizeType                       size) const
{
    return CalcNearestAlignedOffset(size);
}

//------------------------------------------------------------------------------

inline Log_Page::OffsetType 
    Log_Page::MaxAlignedOffset() const
{
    static OffsetType offs 
            = CalcNearestAlignedOffset
              (m_Frame.GetLength() - 
               CalcNearestAlignedSize(GetTrailerSize()));
    return offs;
}

#endif // LOG_PAGE_HPP
            
