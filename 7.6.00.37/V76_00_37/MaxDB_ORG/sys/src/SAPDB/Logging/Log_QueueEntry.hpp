/*!
  @file           Log_QueueEntry.hpp
  @author         UweH
  @author         TillL
  @ingroup        Logging
  @brief          QueueEntry (== "control block") of the log queue

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
#ifndef LOG_QUEUE_ENTRY_HPP
#define LOG_QUEUE_ENTRY_HPP

#include "gsp00.h"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "Container/Container_Vector.hpp"
#include "Logging/Log_Page.hpp"
#include "Logging/Log_Exceptions.hpp"
/*!
   @class          Log_QueueEntry
   @brief          Entry of log queue
 */
class Log_QueueEntry
{
public:
    /// handler of log page frames
    typedef Log_Page                                    Page;
    /// identifier of kernel task
    typedef tsp00_TaskId                                TaskID;
    /// any number of tasks
    typedef SAPDB_UInt4                                 TaskCount;
    /// log queue entry
    typedef Log_QueueEntry                              QueueEntry;
    /// pointer to log queue entry
    typedef Log_QueueEntry*                             EntryPointer;
    /// frame containing a log page
    typedef Kernel_Page::PageFrame                      PageFrame;
    /// raw allocator
    typedef SAPDBMem_IRawAllocator                      Allocator;
    /// Default constructor
    Log_QueueEntry
        (Allocator&                     alloc)
        :
         m_IOpositions(alloc),
         m_LockedByWriter(false),
         m_BusyTaskCount(0),
         m_IsDirty(false)
    {}
    /*!
        @brief          Default constructor
        @param          alloc [in/out] the used allocator
        @param          frame [in] frame where log page is located
     */
    Log_QueueEntry
        (Allocator&                     alloc,
         const PageFrame&               frame)
        :
         m_IOpositions(alloc),
         m_LockedByWriter(false),
         m_BusyTaskCount(0),
         m_Page(frame),
         m_IsDirty(false)
    {}
    /*!
        @brief          Sets page frame
        @param          frame [in] frame where log page is located
     */
    void SetFrame
        (const PageFrame&               frame)
    {
        m_Page.SetFrame(frame);
    }
    /// Returns reference to associated page
    Page& GetPage()
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_Page;
    }
    /// Returns const reference to associated page
    const Page& GetPage() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_Page;
    }
    /// returns true if  a page has been assigned to this entry
    SAPDB_Bool PageIsAssigned() const
    {
        return m_Page.IsAssigned();
    }
    /// Returns actual busy task count (synchronized)
    TaskCount GetBusyTaskCount() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_BusyTaskCount.GetValue();
    }
    /// Increments busy task count (synchronized)
    void IncBusyTaskCount()
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        SAPDBERR_ASSERT_ARGUMENT(m_BusyTaskCount.GetValue() < 1000); // too many tasks indicate error
        m_BusyTaskCount.Increment();
    }
    /// Decrements busy task count (synchronized)
    void DecBusyTaskCount()
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        SAPDBERR_ASSERT_ARGUMENT(m_BusyTaskCount.GetValue() > 0);
        m_BusyTaskCount.Decrement();
    }
    /// sets a flag, that this page will be written by the logwriter
    void MarkForIO()
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        m_LockedByWriter = true;
    }
    /// returns true, if this page will be written by the log writer
    bool IsMarkedForIO() const
    {
        SAPDBERR_ASSERT_STATE(PageIsAssigned());
        return m_LockedByWriter;
    }
    /// space is reserved on the log page but not yet written
    void SetDirty()
    {
        m_IsDirty = true;
    }
    /// the log page was written by the log writer
    void UnsetDirty()
    {
        m_IsDirty = false;
    }
    /// returns true if the log page contains reserved space which is not written by the log writer
    bool IsDirty() const
    {
        return m_IsDirty;
    }
    /*!
        @brief   Registers specified user task as waiting for the i/o of theassociated page
        @param   usertaskid [in] id of user task to be registered
        @param   waitcontext [in/out] this is used for io wait
        @param   wioseqno [out] a place where the writer can store the used iosequence
        @param   offset [out] a place where the writer can store the used offset
        @return  true if successful
     */
    SAPDB_Bool RegisterForIOWait
        (TaskID                            usertaskid,
         Kernel_TaskWaitList::WaitContext &waitcontext,
         Log_IOSequenceNo                 &wioseqno,
         Log_RawDeviceOffset              &offset)
    {
        SAPDBTRACE_METHOD_DEBUG ("Log_QueueEntry::RegisterForIOWait", LogVolume_Trace, 5);
        
        if (m_IOpositions.IsFull())
            if (!m_IOpositions.Reserve(m_IOpositions.GetSize() + 1))
                return false;

        m_IOpositions.InsertEnd( IOposition(&wioseqno, &offset) );

        m_IOWaitStack.InsertAsFirst(usertaskid, waitcontext);
        
        return true;
    }
    /*!
        @brief  Resumes all waiting tasks
        @param  writertaskid [in] id of calling writer task
        @param  wioseqno [in] writer i/o seqno with which this entry was written
        @param  offset [in] writer offset with which this entry was written

       - This command is to be invoked when the writer task has flushed the page to device.
       - All user tasks that have been suspended with the 'RegisterForIOWait()' method are resumed.
     */
    void ResumeWaitingTasks
         (TaskID                        writertaskid,
          Log_IOSequenceNo              wioseqno,
          Log_RawDeviceOffset           offset)
    {
        SAPDBTRACE_METHOD_DEBUG ("Log_QueueEntry::ResumeWaitingTasks", LogVolume_Trace, 5);

        SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "ioseqno: "<<wioseqno<<", offset: "<<offset);
        
        for (SAPDB_UInt i = 0; i < m_IOpositions.GetSize(); i++)
        {
            *(m_IOpositions[i].pIOsequence)   = wioseqno;
            *(m_IOpositions[i].pDeviceOffset) = offset;
        }    
        if ( ! m_IOpositions.Resize(0) )
            // PTS 1117126 UH 2002-08-07 if-clause added
            RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "m_IOpositions.Resize(0) failed") );

        m_IOWaitStack.ResumeAll();
        
        m_LockedByWriter = false;
    }
    /// returns true if there are any tasks waiting for the page i/o
    SAPDB_Bool TasksAreWaitingForIO ()
    {
        return !m_IOWaitStack.IsEmpty();
    }
    /// Get the exact count of waiting tasks
    SAPDB_UInt GetCountOfWaitingTasks ()
    {
        return m_IOWaitStack.NumberOfWaitingTasks();
    }
protected:
    /// information from the writer for the caller
    struct IOposition
    {
        Log_IOSequenceNo    *pIOsequence;       ///< IOSequence of the waiting entry
        Log_RawDeviceOffset *pDeviceOffset;     ///< offset on the device of the waiting entry
        /// default constructor
        IOposition()
        {
            pIOsequence   = NULL;
            pDeviceOffset = NULL;
        }
        /// construction with allvalues given
        IOposition (Log_IOSequenceNo    *ioseq,
                    Log_RawDeviceOffset *offset)
        {
            pIOsequence   = ioseq;
            pDeviceOffset = offset;
        }
    };
    /// log page handler
    Page m_Page;
    /// this is true, if the writer will write the page
    bool m_LockedByWriter;
    /// is set on first Reserve() and reset if this page is given to the log writer
    bool m_IsDirty;
    /// number of user tasks currently writing on associated frame
    RTESync_InterlockedCounter<TaskCount> m_BusyTaskCount;
    /// user tasks in 'EOT' state waiting for write i/o ready
    Kernel_TaskWaitList m_IOWaitStack;
    /// vector of pointers to i/o sequence numbers and offsets
    Container_Vector<IOposition>  m_IOpositions;
};
#endif // LOG_QUEUE_ENTRY_HPP
