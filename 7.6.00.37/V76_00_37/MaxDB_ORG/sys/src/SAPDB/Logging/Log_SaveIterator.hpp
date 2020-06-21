/*!
    @file     Log_SaveIterator.hpp
    @author   MartinB
    @ingroup  Logging
    @brief    Defines an iterator for saving one segment of LogPages

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


#ifndef Log_SaveIterator_H
#define Log_SaveIterator_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h" // tsp00_TaskId, tsp00_PageNo, tsp00_Line
#include "gkb03.h" // tsp00_TaskId, tsp00_PageNo, tsp00_Line
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "IOManager/IOMan_ILogIO.hpp"
#include "IOManager/IOMan_LogPages.hpp"
#include "KernelCommon/Kernel_Page.hpp"
#include "Logging/Log_Page.hpp"
#include "Logging/Log_RawDeviceIterator.hpp"
#include "Logging/Log_QueueRingBuffer.hpp" //Log_FrameAllocator

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
    @brief Iterates over unsaved pages in a LogDevice
*/
class Log_SaveIterator
{
public:

    /*!
    @brief           returns true, if this iterator is initialized by a successfull
                     call to Initialize()
    */
    const SAPDB_Bool          IsValid() const { return m_isValid; };

    /*!
    @brief           returns true, if this iterator is invalid (i.e. Delete has been called)
    */
    const SAPDB_Bool          IsInvalid() const { return !m_isValid; };

    /*!
    @brief           Invalidates the iterator and releases unneeded ressources unconditionally
    */
    void                      Invalidate();

    /*!
    @brief           Invalidates the iterator and releases unneeded ressources, only if it
                     is no longer needed (not repeatable and last segment saved)
    */
    void                      InvalidateNoLongerRequired();

    /*!
    @brief           returns true, if this my used for repeating a logsave
    */
    const SAPDB_Bool          IsRepeatable() const { return m_repeatable; } ;

    /*!
    @brief           The number of the current Log_SaveIterator will be returned
    */
    SAPDB_UInt4         GetLogBackupCount () const;


    /*!
    @brief           The starting IOSequenceNumber will be returned
    */
    Log_IOSequenceNo    GetStartIOSequence () const;


    /*!
    @brief           The last IOSequenceNumber will be returned
    */
    Log_IOSequenceNo    GetEndIOSequence () const;


    /*!
    @brief           The offset of the first saved page will be returned
    */
    Log_RawDeviceOffset    GetStartPageOffset() const;


    /*!
    @brief           The offset of the first saved page will be returned
    */
    Log_RawDeviceOffset    GetEndPageOffset() const;

    /*!
    @brief           The size of the currently saved segment will be returned. This may
                     me smaller than the LOG_SEGMENT_SIZE, because also incomplete
                     segments may be saved (new with PTS 1138588)
    */
    Log_DeviceBlockCount   GetSizeOfCurrentSegment() const;

    /*!
    @brief           The offset of pages to be saved will be returned
    */
    Log_DeviceBlockCount   GetNumPagesLeft() const;


    /*!
    @brief           The Starttime of the saved log-pages will be returned
    */
    void GetStartTime (    Kernel_Date&         StartDate,
                           Kernel_Time&         StartTime) const;


    /*!
    @brief            The Endtime of the saved log-pages will be returned
    */
    void GetEndTime (    Kernel_Date&         EndDate,
                         Kernel_Time&         EndTime) const;


    /*!
    @param            taskId             [in]
    @param            bufferArray    [out]    The number of pages in the buffer must
                                              be less or equal to the blocksize given
                                              in the instantiation of this object
    @param            numPagesRead   [out]
    @brief            reads the next n pages from the ArchiveLog
                      into the Pages-Vector
    */
    bool GetNextLogPages (tsp00_TaskId                       taskId,
                          tkb3_block_ptr                     bufferArray,
                          Log_DeviceBlockCount              &numPagesRead,
                          Msg_List                          &errlist );


    /*!
    @brief            returns wether this iterator describes a complete log-segment
    */
    bool IsLastSegmentToSave () const
    {
        return m_isLastSegmentToSave;
    }


    /*!
    @brief      constructs an iterator for getting LogPages, which shall be saved 
    */
    Log_SaveIterator(    SAPDBMem_IRawAllocator            &Allocator,
                         Log_FrameAllocator                &FrameAlloc,
                         IOMan_ILogIO                      &LogReader);

    /*!
    @brief            Initializes the data of the iterator - exception are allowed
    */
    bool Initialize( const tsp00_TaskId                       taskId,
                     const SAPDB_UInt4                        backupCount,
                     const Log_RawDeviceOffset                saveRangeStart,
                     const Log_RawDeviceOffset                saveRangeEnd,
                     const Log_RawDeviceIterator              firstPageToSave,
                     const Log_RawDeviceIterator              lastPageToSave,
                     const SAPDB_Int2                         blockSize,
                     const SAPDB_Bool                         isLastSegmentToSave, // PTS1111987 martinb 2001-10-12
                     const SAPDB_Bool                         repeatable); // PTS 1128703 mb 2004-04-26

    /*!
    @brief            returns the staroffset and the endoffset of yet unsaved pages 
    */
    void GetNextSegmentRange(Log_RawDeviceOffset &startPoint, Log_RawDeviceOffset &endPoint);

    /*!
    @brief            writes some information about the current state of the iterator into the
                      kernel-trace (PTS 1127177 mb 2004-01-23)
    */
    void WriteToTrace() const;

private:
    /*!
    @brief            check the m_pagebuffer for consistency
    */
    bool CheckPageBuffer(Msg_List &errlist);

    /// do not allow an assignment operation
    Log_SaveIterator& operator=(const Log_SaveIterator& rhs);
    
        
    /*!
    @brief            The Logdevice from where data will be read
    */
    IOMan_ILogIO             &m_LogReader;

    /*!
    @brief            current position of the iterator
    */
    Log_RawDeviceIterator   m_CurrentPage;

    /*!
    @brief            first page, which will be returned after initialization
    */
    Log_RawDeviceIterator   m_FirstPageToSave;

    /*!
    @brief            last page, which will be returned for saving
    */
    Log_RawDeviceIterator   m_LastPageToSave;
                            
    /*!
    @brief            first IOSeq to be saved 
    */
    Log_IOSequenceNo        m_FirstIOSeqToSave;
                            
    /*!
    @brief            last IOSeq to be saved 
    */
    Log_IOSequenceNo        m_LastIOSeqToSave;

    /*!
    @brief            vector, in which pages will be read
    */
    IOMan_LogPages          m_pagebuffer;

    /*!
    @brief            size of the vector, in which pages will be read
    */
    SAPDB_Int2              m_pagebufferSize;

    /*!
    @brief            allocator which serves the frame-memory
    */
    Log_FrameAllocator&     m_frameAlloc;
    
    /*!
    @brief            page frame for reading special pages
    */
    Kernel_Page::PageFrame  m_frame;
    
    /*!
    @brief            a single logPage used for single page IO during initialization
    */
    Log_Page                m_logPage;
    
    /*!
    @brief            this is the "name" of our backup-part
    */
    SAPDB_UInt4            m_logBackupCount;
    
    /*!
    @brief            write-date of the first page to be saved
    */
    Kernel_Date            m_startDate;
        
    /*!
    @brief            write-date of the last page to be saved
    */
    Kernel_Date            m_endDate;
    
    /*!
    @brief            write-time of the first page to be saved
    */
    Kernel_Time            m_startTime;
    
    /*!
    @brief            write-time of the last page to be saved
    */
    Kernel_Time            m_endTime;
    
    /*!
    @brief            indicates that all pages have been saved
    */
    bool                   m_allPagesProcessed;

    /*!
    @brief            true if this iterator describes a complete segment
    */
    bool                   m_isLastSegmentToSave;

    /*!
    @brief            true if this iterator denotes a range of pages to be saved
    */
    bool                   m_isValid;

    /*!
    @brief            true if this iterator may be used for repeating a log-save
    */
    bool                   m_repeatable;

    /*!
    @brief            gives the startoffset of the total range of pages over multiple segments in
                      order to allow a repeatable save also for multiple segments
    */
    Log_RawDeviceOffset    m_saveRangeStart;

    /*!
    @brief            gives the endoffset of the total range of pages over multiple segments in
                      order to allow a repeatable save also for multiple segments
    */
    Log_RawDeviceOffset    m_saveRangeEnd;

};



/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/

#endif  /* Log_SaveIterator_H */

