/*!
  @file           Log_QueueRingBuffer.hpp
  @author         TillL
  @author         UweH
  @author         MartinB
  @ingroup        LogVolume   
  @brief          Circular list of log queue entries

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

#ifndef LOG_QUEUE_RINGBUFFER_HPP
#define LOG_QUEUE_RINGBUFFER_HPP

#include "Logging/Log_QueueEntry.hpp"
#include "Container/Container_CircularList.hpp"
#include "IOManager/IOMan_LogPages.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"

#include "hgg01_3.h"   // g01page_size

// dummy implementation
#include "RunTime/RTE_ISystem.hpp"

/// Allocate PageFrames for Logging-purposes
class Log_FrameAllocator
{
public:
    /// shortcut
    typedef Kernel_Page::PageFrame PageFrame;

    /// returns the singleton. If neccessary, the singleton is constructed
    static Log_FrameAllocator& Instance()
    {
        SAPDBMEM_STATIC_RAW_ALLOCATE (Space, sizeof(Log_FrameAllocator));
        if (m_Instance == 0)
            m_Instance = new (Space) Log_FrameAllocator ();
        return *m_Instance;
    }

    /// allocates a pageframe 
    PageFrame New()
    {
        const SAPDB_UInt pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();

        SAPDB_Byte* p = reinterpret_cast<SAPDB_Byte*>
                            (RTE_ISystem::Instance().AllocSystemPages(pageSize));
        if ( NULL == p )
            RTE_Crash( Log_Exception(__CONTEXT__,
                                          SAPDBERR_ASSERT_STATE_FAILED,
                                          "Log_FrameAllocator::New() failed.") );
        
        return PageFrame(p, pageSize);
    }
    
    /// Allocates continuous memory and intializes the according pageframe 
    /// (PTS 1126157 mb 2003-01-08)
    void AllocateContinuousFrames(      IOMan_LogPages    &pageVector,
                                  const SAPDB_UInt         numPages)
    {
        const SAPDB_UInt pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();
        SAPDB_Byte* p = reinterpret_cast<SAPDB_Byte*>
                            (RTE_ISystem::Instance().AllocSystemPages(pageSize*numPages));
        if ( NULL == p )
            RTE_Crash( Log_Exception(__CONTEXT__,
                                          SAPDBERR_ASSERT_STATE_FAILED,
                                          "Log_FrameAllocator::New() failed.") );
        for (SAPDB_UInt i=0; i < numPages; i++)
        {
            PageFrame pageFrame(p, pageSize);
            Log_Page  logPage(pageFrame,1,Log_IOSequenceNo()); // initialize frames too for destructor Verify Check
            pageVector.Append(logPage);
            p += pageSize;
        }
    }
        
    /// deallocates a pageframe
    void Free(PageFrame& frame)
    {
        RTE_ISystem::Instance().FreeSystemPages(frame.GetDataPtr(),
            FrameCtrl_ICommon::GetInstance().GetPageSize());
        frame.Deassign();
    }
    /// deallocates a pages allocated by AllocateContinuousFrames
    void FreeContinuousFrames(      IOMan_LogPages    &pageVector,
                              const SAPDB_UInt         numPages)
    {
        IOMan_LogPagesIterator  bufferIter(pageVector);
        bufferIter.Begin();
        Log_Page & firstPage = *bufferIter;
        PageFrame & pageFrame = firstPage.GetFrame();
        RTE_ISystem::Instance().FreeSystemPages(pageFrame.GetDataPtr(),
                                                FrameCtrl_ICommon::GetInstance().GetPageSize()*numPages);
    }
    
private:
    Log_FrameAllocator(){}
    static Log_FrameAllocator* m_Instance;
};
 

/*!*****************************************************************************

   class: Log_QueueRingBuffer

   description: Circular list of log queue entries

*******************************************************************************/
class Log_QueueRingBuffer : 
public Container_CircularList<Log_QueueEntry>
{
public:

    /*!-------------------------------------------------------------------------
    declaration : List
    description:  Parent type
    --------------------------------------------------------------------------*/
    typedef Container_CircularList<Log_QueueEntry>              List;
    /*!-------------------------------------------------------------------------
    declaration : QueueEntry
    description:  log queue entry
    --------------------------------------------------------------------------*/
    typedef Log_QueueEntry                                      QueueEntry;
    /*!-------------------------------------------------------------------------
    declaration : EntryCount
    description:  any number of log queue entries
    --------------------------------------------------------------------------*/
    typedef List::SizeType                                      EntryCount;
    /*!-------------------------------------------------------------------------
    declaration : QueueID
    description:  identifier for log queues
    --------------------------------------------------------------------------*/
    typedef Log_QueueID                                         QueueID;
    /*!-------------------------------------------------------------------------
    declaration : FrameAllocator
    description:  allocator for page frames
    --------------------------------------------------------------------------*/
    typedef Log_FrameAllocator                                  FrameAllocator;

    /*!-------------------------------------------------------------------------
    function:     Log_QueueRingBuffer()
    description:  Constructor

    arguments:    queueid  [in] log queue id
                  alloc    [in] allocator to be used for entry allocation
                  frameres [in] frame resource from which to allocate page frames
    return value: none
    --------------------------------------------------------------------------*/
    Log_QueueRingBuffer
        (Allocator&                     alloc,
         FrameAllocator&                framealloc)
        :
         List(alloc),
         m_FrameAlloc(framealloc)
    {}

    /*!-------------------------------------------------------------------------
    function:     Initialize()
    description:  Initializes buffer with given number of elements

                  - 'count' entries and 'count' page frames are allocated

    arguments:    count [in]    number of entries to be created
    return value: (SAPDB_Bool) true if successful
    --------------------------------------------------------------------------*/
    SAPDB_Bool Initialize
        (EntryCount                     count);

    /*!-------------------------------------------------------------------------
    function:     ~Log_QueueRingBuffer()
    description:  Destructor

                  - Deallocates all page frames and all entries.
                  - Overrides ~List()

    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    ~Log_QueueRingBuffer()
    {
        Delete();
    }

    /*!-------------------------------------------------------------------------
    function:     Delete()
    description:  Deletes the element at a given iterator position

                  - After the operation, 'pos' points to the entry behind 
                    the deleted one.
                  - Overrides List::Delete(pos)

    arguments:    pos [in/out]  iterator delete position
    return value: none
    --------------------------------------------------------------------------*/
    void Delete
        (Iterator&                      pos);

    /*!-------------------------------------------------------------------------
    function:     Delete()
    description:  Deallocates all entries and returns all frames

                  - Overrides List::Delete()

    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    void Delete();

    /* -----------------------------------------------------------------------*/
    /*! chapter: Status functions                                             */
    /*!-------------------------------------------------------------------------
    function:     GetEntryCount()
    description:  Returns the actual entry count of the ring buffer
    arguments:    none
    return:       (EntryCount) entry count
    --------------------------------------------------------------------------*/
    EntryCount GetEntryCount() const
    {
        return GetSize();
    }
    /* -----------------------------------------------------------------------*/
    /*! endchapter: Status functions                                          */
    /* -----------------------------------------------------------------------*/

private:

    /*--------------------------------------------------------------------------
    declaration : m_FrameAlloc
    description:  allocator for frames
    --------------------------------------------------------------------------*/
    FrameAllocator&                  m_FrameAlloc;
};
/*!*****************************************************************************

   endclass: Log_QueueRingBuffer

*******************************************************************************/





#endif // LOG_QUEUE_RINGBUFFER_HPP
