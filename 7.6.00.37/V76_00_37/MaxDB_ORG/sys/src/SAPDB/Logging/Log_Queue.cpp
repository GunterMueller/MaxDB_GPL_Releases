/*!*****************************************************************************

  module:       Log_Queue.cpp

  ------------------------------------------------------------------------------

  responsible:  UweH

  author:       TillL

  special area: Logging

  description:  see .hpp

  see also:

  Copyright (c) 2000-2005 SAP AG

  ------------------------------------------------------------------------------

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


*******************************************************************************/

#include "heo56.h"

#include "Logging/Log_Queue.hpp"
 
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_Volume.hpp"

// -----------------------------------------------------------------------------
// Suspend reasons for log queue:
// -----------------------------------------------------------------------------
#define LOG_QUEUE_FULL_SUSPEND     233
#define LOG_DEVICE_FULL_SUSPEND    246
#define LOG_QUEUE_IO_WAIT_SUSPEND  234

// -----------------------------------------------------------------------------
#define CHECK_INCREMENT_OFFSETS true

/*******************************************************************************

   class: Log_Queue

*******************************************************************************/

// -----------------------------------------------------------------------------
void Log_Queue::FindStartPointToReserve (tsp00_TaskId                      usertask,
                                         Kernel_TaskWaitList::WaitContext &waitcontext,
                                         SizeType                          size,
                                         SizeType                          unsplitsize)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::FindStartPointToReserve", LogVolume_Trace, 5);

    bool     wasInterupted;
    SizeType available;

    do
    {
        wasInterupted = false;
        
        // ----------------------------------------------------------
        // check if the log writer is already on CurrentAppend
        // if so take next page or wait
        // ----------------------------------------------------------

        if ( m_CurrentAppend->IsMarkedForIO() )
        {
            WaitUntilSpaceIsAvailable(usertask, waitcontext, WaitForWriterAction);
            // immediately check previous conditions again
            wasInterupted = true;
            continue;
        }
        
        SAPDBERR_ASSERT_ARGUMENT( ! m_CurrentAppend->IsMarkedForIO() );
        
        
        // ----------------------------------------------------------
        // check if the the unsplit part fits
        // ----------------------------------------------------------

        available = m_CurrentAppend->GetPage().FreeSpaceAvailable();

        if ( available < unsplitsize )
        {
            Iterator iter = m_CurrentAppend;

            if ( ++iter == m_CurrentOutput )
            {
                WaitUntilSpaceIsAvailable(usertask, waitcontext, WaitForWriterAction);
                // immediately check previous conditions again
                wasInterupted = true;
                continue;
            }
            
            UseNextPage();
            
            available = m_CurrentAppend->GetPage().FreeSpaceAvailable();

            if ( available < unsplitsize )
            {
                WriteToTrace();
                RTE_Message( Log_Exception(__CONTEXT__,
                                           LOG_QUEUE_SPACE_AVAILABLE,
                                           SAPDB_ToString(size),
                                           SAPDB_ToString(unsplitsize),
                                           SAPDB_ToString(available)) );
                RTE_Crash( Log_Exception(__CONTEXT__,
                                              SAPDBERR_ASSERT_STATE_FAILED,
                                              "Log_Queue::SpaceAvailable() no space for unsplit part") );
            }
        }
    }
    while ( wasInterupted );

    SAPDBERR_ASSERT_ARGUMENT( available >= unsplitsize );
}
// -----------------------------------------------------------------------------
void Log_Queue::FindSpaceToReserve (tsp00_TaskId                      usertask,
                                    Kernel_TaskWaitList::WaitContext &waitcontext,
                                    SizeType                          size,
                                    SizeType                          unsplitsize)
{
    SizeType wanted         = size;
    SizeType available;
    bool     wasInterupted;

    do
    {    
        wanted = size; // PTS 1129382 mb 2004-04-28

        wasInterupted = false;
        
        FindStartPointToReserve (usertask, waitcontext, size, unsplitsize);

        Iterator iter = m_CurrentAppend;
        
        while ( wanted > 0 )
        {
            available = iter->GetPage().FreeSpaceAvailable();

            if ( available > wanted )
                available = wanted;
            else
            {
                ++iter;
                
                if ( iter == m_CurrentOutput )
                {
                    WaitUntilSpaceIsAvailable(usertask, waitcontext, WaitForWriterAction);
                    // immediately check previous conditions again
                    wasInterupted = true;
                    break;
                }
            }
            wanted -= available;
        }
    }
    while ( wasInterupted );
}
// -----------------------------------------------------------------------------
bool
Log_Queue::ReserveSpace ( TaskID                            usertaskid,
                          Kernel_TaskWaitList::WaitContext &waitcontext,
                          SizeType                          size,
                          SizeType                          unsplitsize,
                          Log_AfterImageSpace              &space,
                          Log_Page::OffsetType             &entrypos,
                          SAPDBErr_MessageList             &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::ReserveSpace", LogVolume_Trace, 5);

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "size: " << size << ", unsplit: " << unsplitsize);
    SAPDBERR_ASSERT_ARGUMENT(size >= unsplitsize);
    SAPDBERR_ASSERT_ARGUMENT(!space.IsAssigned());

    if (size > MaxReserveSize())
    {
        // fatal error: requested size is larger than entire queue size
        WriteToTrace();
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                "requested size issmaller then the complete queue size");
        errlist =  Log_Exception(__CONTEXT__,LOG_QUEUE_SPACE_AVAILABLE,
                                 SAPDB_ToString(size),
                                 SAPDB_ToString(unsplitsize),
                                 SAPDB_ToString(MaxReserveSize()));
        return false;
    }

    EnterRegion(usertaskid); 

    ++m_StatisticalCurrentValues.m_ReserveCount;
    
    // ----------------------------------------------------------
    // find a place which can hold the requested size
    // ----------------------------------------------------------
    
    FindSpaceToReserve (usertaskid, waitcontext, size, unsplitsize);

    // ----------------------------------------------------------
    // really reserve space now on the pages and
    // add pages needed to hold the requested space
    // ----------------------------------------------------------
    
    Page     &page      = m_CurrentAppend->GetPage();
    SizeType  available = page.FreeSpaceAvailable();
    SizeType  wanted    = size;
              entrypos  = page.ReadFirstFreeOffset();

    if ( m_IOSequenceNo.IsInvalid() )
    {
        // ReserveSpace() is called for the first time
        m_IOSequenceNo = 0;
        page.WriteQueueIOSequenceNo(m_IOSequenceNo);
        page.WriteQueueID(m_QueueID);
    }

    while ( wanted > 0 )
    {
        Entry &entry = *m_CurrentAppend;
        Page  &page  = entry.GetPage();

        available = page.FreeSpaceAvailable();

        if ( LogVolume_Trace.TracesLevel(7) )
        {
            Kernel_VTrace() << "wanted: " << wanted << ", available: " << available;
            WriteEntryToTrace( *m_CurrentAppend,
                               "ReserveSpace",
                               m_IOSequenceNo.RawValue(), // PTS 1124724 mb 2003-10-29
                               true,
                               m_CurrentAppend==m_CurrentOutput );
        }

        if ( available == 0 )
        {
            WriteToTrace();
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "space for redo entry available");
            return false;
        }

        if (available >= wanted) // UH 2004-04-02 ">" changed to ">="
            available = wanted;
        else
        {
            UseNextPage();
            m_CurrentAppend->GetPage().SetContinuedEntry(); // PTS 1131467
            if ( m_CurrentAppend == m_CurrentOutput )
            {
                WriteToTrace();
                errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                        "LogQueue sync: currentAppend must not meet currentOutput");
                // AppendContextInfo(errlist);
                return false;
            }
        }
        
        if ( LogVolume_Check.ChecksLevel(5) && ! page.Verify(false,errlist) )
        {
            WriteToTrace();
            errlist = errlist + Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "additional check page.Verify()");
            return false;
        }

        space.AppendPart(entry, available, page.Reserve(wanted, available));
        entry.IncBusyTaskCount();
        entry.SetDirty();
        wanted -= available;

        if ( LogVolume_Check.ChecksLevel(5) && ! page.Verify(false,errlist) )
        {
            WriteToTrace();
            errlist = errlist + Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "additional check page.Verify()");
            return false;
        }
    }
    LeaveRegion(usertaskid); 
    return true;
}
// -----------------------------------------------------------------------------
void Log_Queue::WaitUntilSpaceIsAvailable ( tsp00_TaskId                      usertaskid,
                                            Kernel_TaskWaitList::WaitContext &waitcontext,
                                            WaitQueueSelector                 queueSelector)
{
    ++m_StatisticalCurrentValues.m_QueueFullSuspendCount;
    
    if (WaitForLogFree != queueSelector)
    {
        m_QueueFullWaitList.InsertAsLast (usertaskid, waitcontext);
        LeaveRegion(usertaskid);
        if ( LogVolume_Trace.TracesLevel(6) )
            Kernel_VTrace() << "LOG_QUEUE IS FULL: task " << usertaskid << " is suspended";
        vsuspend(usertaskid, LOG_QUEUE_FULL_SUSPEND);
        EnterRegion(usertaskid); 
    }
    else // WaitForLogFree
    {
        // synchronize m_LogIsFull with the enqueing of the task in to the waitlist
        // PTS 1118102 mb 2002-09-26
        EnterRegion(usertaskid);
        if (!m_LogIsFull)
        {
            LeaveRegion(usertaskid);
            return;
        }
        m_LogFullWaitList.InsertAsLast (usertaskid, waitcontext);
        LeaveRegion(usertaskid);                                           // PTS 1118102 mb 2002-09-26
        if ( LogVolume_Trace.TracesLevel(6) )
            Kernel_VTrace() << "LOG_DEVICE IS FULL: task " << usertaskid << " is suspended";
        RTE_Message(Log_Exception(__CONTEXT__,LOG_DEVICE_FULL,SAPDB_ToString(usertaskid)));
        if ( Log_Volume::Instance().IsAutomaticOverwrite(usertaskid) )
        {
            Msg_List errlist;
            if ( Log_Savepoint::Instance().StartSavepoint (*RTETask_ITask::ByTaskID(usertaskid), Log_SVPReasonLogWriter, errlist)
                 !=
                 Log_Savepoint::ok )
            {
                RTE_Message(errlist);
                errlist.ClearMessageList();
            }
        }
        vsuspend(usertaskid, LOG_DEVICE_FULL_SUSPEND);
    }
}
// -----------------------------------------------------------------------------
void Log_Queue::UserTaskReady
    (TaskID                         usertaskid,
     Log_AfterImageSpace&           space)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::UserTaskReady", LogVolume_Trace, 5);

    EnterRegion(usertaskid); 
    
    if ( LogVolume_Trace.TracesLevel(7) )
        WriteEntryToTrace(space.GetPartEntry(0), "UserTaskReady");

    // decrease busy task count
    space.Deassign();

    if (OutputAvailableAt(m_CurrentOutput))
    {
        // There are pages available for output.
        // Resume writer task if not already running.
        m_WriterTaskSync.Resume(); 
    }

    LeaveRegion(usertaskid);
}
// -----------------------------------------------------------------------------
void Log_Queue::UserTaskEOTReady
    (TaskID                            usertaskid,
     Kernel_TaskWaitList::WaitContext &waitcontext,
     Log_AfterImageSpace              &space,
     IOSequenceNo                     &wioseqno,
     Log_RawDeviceOffset              &offset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::UserTaskEOTReady", LogVolume_Trace, 5);

    EnterRegion(usertaskid); 
    
    SAPDBERR_ASSERT_ARGUMENT(space.GetPartCount() == 1);
    Log_QueueEntry& entry = space.GetPartEntry(0);
    // task must wait for i/o
    // task is registered here (for a later resume)
    // task is suspended below (see ***)
    if (!entry.RegisterForIOWait(usertaskid, waitcontext, wioseqno, offset))
    {
        WriteToTrace();
        RTE_Crash( Log_Exception(__CONTEXT__,
                                      SAPDBERR_ASSERT_STATE_FAILED,
                                      "Log_Queue::UserTaskEOTReady: RegisterForIOWait() failed") );
    }

    IncrementCounter(waitingForLogIO);
    
    if ( LogVolume_Trace.TracesLevel(7) )
        WriteEntryToTrace(entry, "UserTaskEOTReady");

    // decrease busy task count
    space.Deassign();
    
    if (OutputAvailableAt(m_CurrentOutput))
    {
        // There are pages available for output.
        // Resume writer task if not already running.
        m_WriterTaskSync.Resume(); 
    }
    
    ++m_StatisticalCurrentValues.m_WaitCount;

    LeaveRegion(usertaskid); 

    // ***
    // task is suspended here if it must wait for the i/o
    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "LOG QUEUE IO WAIT: task " << usertaskid << " suspended");

    vsuspend(usertaskid, LOG_QUEUE_IO_WAIT_SUSPEND);
}
// -----------------------------------------------------------------------------
void Log_Queue::ForceNewPage (TaskID               backupCoordinator,
                              Log_RawDeviceOffset &offsetOfLastWrittenPage)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::ForceNewPage", LogVolume_Trace, 5);
    
    bool                               doSuspend = false;
    Log_IOSequenceNo                   dummyLogIOSequence;
    Kernel_TaskWaitList::WaitContext   waitContext;

    EnterRegion(backupCoordinator); 
    
    if (!m_CurrentAppend->GetPage().IsEmpty())
    // if the currentPage is empty, then it need not be flushed 
    {
        m_ForceNewPage = true;
        if (!m_CurrentAppend->RegisterForIOWait(backupCoordinator, waitContext, dummyLogIOSequence, offsetOfLastWrittenPage))
        {
            WriteToTrace();
            RTE_Crash( Log_Exception(__CONTEXT__,
                                          SAPDBERR_ASSERT_STATE_FAILED,
                                          "Log_Queue::ForceNewPage: RegisterForIOWait() failed") );
        }
    
        m_WriterTaskSync.Resume();
        doSuspend = true;
    }

    LeaveRegion(backupCoordinator);
    
    if (doSuspend)
    {
        vsuspend(backupCoordinator, LOG_QUEUE_IO_WAIT_SUSPEND);
    }
}

// -----------------------------------------------------------------------------
void Log_Queue::GetOutput (SAPDB_UInt      maxPages,
                           IOMan_LogPages &output,
                           SAPDB_UInt     &pagecount,
                           SAPDB_Bool     &replacefirst,
                           SAPDB_Bool     &useOnlyQueue0)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::GetOutput", LogVolume_Trace, 5);

    const SAPDB_UInt LOG_IO_BLOCK_COUNT = output.BlockSize();

    if ( m_MaxDelayLogWriter > 0
         &&
         GetCounterValue(waitingForLogIO) > 0
         &&
         GetCounterValue(openTrans) > 0
         &&
         m_PagesUsed <= LOG_IO_BLOCK_COUNT )
    {
        const SAPDB_UInt8 MAX_TIMEOUT = RTESys_MicroSecTimer() + m_MaxDelayLogWriter;
        
        while ( GetCounterValue(waitingForLogIO) > 0
                &&
                GetCounterValue(openTrans) > 0
                &&
                m_PagesUsed <= LOG_IO_BLOCK_COUNT )
        {
            if ( RTESys_MicroSecTimer() > MAX_TIMEOUT )
                break;
        }
    }

    SAPDB_Bool pageIsFlushedAgain = false;
    replacefirst = false;
    pagecount = 0;
    
    tsp00_TaskId writerTaskId = m_WriterTaskSync.GetTaskID();

    EnterRegion(writerTaskId); 

    IOSequenceNo  lastQueueSeq;
    Kernel_VTrace trace;
    
    if ( m_FlushModeChanged && (m_QueueID == 0)  // only queue 0 may handle the changing of the flushmode
         &&
         m_CurrentOutput->GetBusyTaskCount() == 0
         &&
         m_CurrentOutput == m_CurrentAppend
         &&
         ! m_CurrentOutput->IsDirty() )
	{
        // nothing to flush, so flush mode can be changed immediately
        m_CurrentOutput->GetPage().Clear();
        ++m_CurrentOutput;
        UseNextPage();
        m_FlushModeChanged = false;
        if ( LogVolume_Trace.TracesLevel(5) )
            trace << "Log_Queue::GetOutput FlushModeChanged: reset to false (1)" << FlushLine;
	}
    
    Iterator iter = m_CurrentOutput;
    
    while (     OutputAvailableAt(iter) 
           && ! output.IsFull() 
           &&   (pagecount < maxPages))
    {
        // the same like in UserTaskEOTReady()
        if ( m_CurrentOutput == m_CurrentAppend )
        {
            if ( pagecount > 0 )
            {
                WriteToTrace();
                RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,"invalid pointer") );
            }

            // let users reserve space on the next page in the queue
            UseNextPageCausedByIO();                    //     !!!!! C A U T I O N !!!!!
                                                        // if this is removed, the checks in
                                                        // Log_Writer::PrepareAndFlushPageVector()
                                                        // DO NOT WORK !!
        }

        Page& page = iter->GetPage();
        
        pageIsFlushedAgain = 
                 m_QueueSequenceNoOfLastPageOfLastWrittenBlock.IsValid()
                 &&
                 m_QueueSequenceNoOfLastPageOfLastWrittenBlock == page.ReadQueueIOSequenceNo();
                 
        if ( pagecount == 0 )
        // this is the first page of this block to be written from this queue
        {
            if (pageIsFlushedAgain)
            {
                if (m_QueueID == 0) 
                {
                    if ((m_FlushMode != Log_DeviceMinimizeSpace) &&  (! m_FlushModeChanged))
                    {
                        WriteToTrace();
                        RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                                 "page is flushed again for LogQueueId = 0 and MaximizeSpace") );
                    }
                }
                else
                {
                    WriteToTrace();
                    RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                             "page is flushed again for LogQueueId != 0") );
                }
                    
            }

            if ( m_FlushModeChanged && (m_QueueID == 0))
            {
                if ( LogVolume_Trace.TracesLevel(5) )
                    trace << "Log_Queue::GetOutput FlushModeChanged: reset to false (2)" << FlushLine;
                m_FlushModeChanged = false;
            }

            replacefirst = pageIsFlushedAgain;
        }
        else
        {
            if ( pageIsFlushedAgain )
            {
                WriteToTrace();
                RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                         "more then one copied page") );
            }
        }
        
        // insert into io-vector and mark for io
        // signal, that the writer will write this page out
        // a user task, which will see this flag will copy this page
        iter->MarkForIO();
        
        iter->UnsetDirty();

        output.Append(page);
        ++pagecount;

        if ( LogVolume_Trace.TracesLevel(7) )
            WriteEntryToTrace ( *iter,
                                "GetOutput",
                                pagecount,iter==m_CurrentAppend,
                                iter==m_CurrentOutput );
 
        if (page.IsEmpty())
        {
            WriteToTrace();
            RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,"emptyPage") );
        }

        SAPDB_UInt waitingTasks = iter->GetCountOfWaitingTasks();

        if ( waitingTasks > m_StatisticalCurrentValues.m_MaxCountOfWaitingTasksPerPage )
            m_StatisticalCurrentValues.m_MaxCountOfWaitingTasksPerPage = waitingTasks;

        if ( waitingTasks > 1 )
            ++m_StatisticalCurrentValues.m_GroupCommitCount;
        
        lastQueueSeq = page.ReadQueueIOSequenceNo();
        
        if (iter == m_CurrentAppend)
            break; // there cannot be more pages

        ++iter; // look fo the next logpage in the log queue
        
        // prevent the log writer to get pages, which are copied
        
        if ( iter == m_CurrentAppend
             ||
             lastQueueSeq == iter->GetPage().ReadQueueIOSequenceNo() )
            break;
    }
    
    useOnlyQueue0 = m_FlushModeChanged || (m_FlushMode == Log_DeviceMinimizeSpace);
    
    if (lastQueueSeq.IsValid())
        m_QueueSequenceNoOfLastPageOfLastWrittenBlock = lastQueueSeq;
    LeaveRegion(writerTaskId);
}
// -----------------------------------------------------------------------------
void Log_Queue::WriterTaskReady
    (TaskID                         writertaskid,
     IOSequenceNo                   wioseqno,
     Log_RawDeviceIterator          writePosition,            // PTS 1125634 mb 2003-11-26
     SAPDB_UInt                     pagecount)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::WriterTaskReady", LogVolume_Trace, 5);

    SAPDBERR_ASSERT_ARGUMENT(writertaskid == m_WriterTaskSync.GetTaskID());
    
    EnterRegion(writertaskid); 

    SAPDB_UInt            count            = 0;
    Log_RawDeviceIterator auxWritePosition = writePosition; // UH 2005-10-14 use local copy for operator ++
    // Log_RawDeviceOffset   absolutePosition = writePosition.GetPosition();// PTS 1138219 mb 2005-10-07
    
    while (count < pagecount)
    {
        if ( LogVolume_Trace.TracesLevel(7) )
            WriteEntryToTrace( *m_CurrentOutput,
                               "WriterTaskReady",
                               count,
                               m_CurrentOutput==m_CurrentAppend,
                               true );

        DecrementCounter(waitingForLogIO, m_CurrentOutput->GetCountOfWaitingTasks());
        
        // resume user tasks waiting for write i/o:
        m_CurrentOutput->ResumeWaitingTasks(writertaskid, 
                                            wioseqno + count,
                                            auxWritePosition.GetPosition() );

        ++count;
        
        ++auxWritePosition; // UH 2005-10-14 use local iterator
        
        if ( m_CurrentOutput == m_CurrentAppend )
        {
            if ( count != pagecount )
            {
                // this must not be
                WriteToTrace();
                RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "more pages written then could be") );
            }
        }
        else
        {
            m_CurrentOutput->GetPage().Clear();
            ++m_CurrentOutput;
        }
    }

    // resume user tasks waiting for free queue space
    m_QueueFullWaitList.ResumeAll();

    // PTS 1115332 UH 2002-04-29 begin
    if ( m_PagesUsed < pagecount )
        m_PagesUsed = 1;
    else
        m_PagesUsed -= pagecount;
    // PTS 1115332 UH 2002-04-29 end
    
    m_StatisticalCurrentValues.m_WriteCount += pagecount;
    LeaveRegion(writertaskid); 
}

// -----------------------------------------------------------------------------
void Log_Queue::CopyCurrentAppend()
{
    Log_Page& page      = m_CurrentAppend->GetPage();
    ++m_CurrentAppend;
    ++m_PagesUsed;  // PTS 1135588 mb 1005-05-08
    Log_Page& nextpage  = m_CurrentAppend->GetPage();

    if ( ! page.CopyFrameTo(nextpage) )
    {
        page.WriteToTrace();
        nextpage.WriteToTrace();
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"page.CopyFrameTo(nextpage)"));
	}

    nextpage.IncOffsetCount();

    if ( LogVolume_Trace.TracesLevel(6) )
        Kernel_VTrace() << "IncOffsetCount: new: " << page.ReadOffsetCount();

    // reset values which were already partially be overwritten
    // by the logwriter
    nextpage.WriteWriterIOSequenceNo(IOSequenceNo());
    nextpage.WriteDate(0);
    nextpage.WriteTime(0);
    nextpage.WriteQueueIOSequenceNo(page.ReadQueueIOSequenceNo());
    nextpage.WriteQueueID(m_QueueID);
    nextpage.PrepareForWrite();//+++++ expensive => should be only resetted

    if ( LogVolume_Trace.TracesLevel(7) )
        WriteEntryToTrace( *m_CurrentAppend,
                           "CopyCurrentAppend",
                           1,
                           true,
                           m_CurrentAppend==m_CurrentOutput );
}

// -----------------------------------------------------------------------------
void Log_Queue::WriteToTrace() const
{
    Kernel_VTrace trace;
 
    trace << "Queue No: " << GetID() << ", size: " << GetSize()
          << ", m_QueueSequenceNoOfLastPageOfLastWrittenBlock: " << m_QueueSequenceNoOfLastPageOfLastWrittenBlock
          << NewLine;

    SAPDB_UInt entryno = 0;
    Iterator   iter    = m_CurrentOutput;

    do    
    {
        WriteEntryToTrace( *iter,
                           "AllEntries",
                           entryno,
                           iter == m_CurrentAppend,
                           iter == m_CurrentOutput);
        ++iter;
        ++entryno;
    }
    while ( iter != m_CurrentOutput || entryno < GetSize() );
}

// -----------------------------------------------------------------------------
void Log_Queue::WriteEntryToTrace(const Log_QueueEntry &entry,
                                  const char           *title,
                                  SAPDB_UInt            entryno,
                                  bool                  isCurrentAppend,
                                  bool                  isCurrentOutput) const
{
    Kernel_VTrace trace;
 
    const Log_Page &page = entry.GetPage();
    
    trace << (title != NULL ? title : "")
          << ", QueueEntry #" << entryno << ", seq: "
          << page.ReadQueueIOSequenceNo()
          << ", BusyCount: " << entry.GetBusyTaskCount()
          << ", IsMarkedForIO: " << (entry.IsMarkedForIO()?"true":"false")
          << (isCurrentAppend ? " <- CurrentAppend" : "")
          << (isCurrentOutput ? " <- CurrentOutput" : "") << NewLine;
    
    if ( page.IsEmpty() )
        trace << "empty" << FlushLine;
    else
        page.WriteToTrace();
}

// -----------------------------------------------------------------------------
void Log_Queue::UseNextPageCausedByIO()
{
    if ( m_FlushMode != Log_DeviceMinimizeSpace
         ||
         m_ForceNewPage )
    {
        m_ForceNewPage = false;
        UseNextPage();
        return;
    }

    Page &page = m_CurrentAppend->GetPage();

    if ( page.FreeSpaceAvailable() < page.GetLength() / 3 )
        UseNextPage();
    else
        CopyCurrentAppend();
}

// -----------------------------------------------------------------------------
void Log_Queue::UseNextPage()
{
    ++m_CurrentAppend;
    // PTS 1115332 UH 2002-04-29 begin
    ++m_PagesUsed;
    if ( m_PagesUsed > m_StatisticalCurrentValues.m_MaxPagesUsed )
        m_StatisticalCurrentValues.m_MaxPagesUsed = m_PagesUsed;
    // PTS 1115332 UH 2002-04-29 end
    Page &page = m_CurrentAppend->GetPage();
    ++m_IOSequenceNo;
    page.WriteQueueIOSequenceNo(m_IOSequenceNo);
    page.WriteQueueID(m_QueueID);
}

// -----------------------------------------------------------------------------
template <typename T> T Log_Queue::GetMaxQueueCounterValue (const Log_Queue::QueueCounterSelector counter,
                                                            const T baseMax,
                                                            const T intermediateMax,
                                                            const T currentMax) const
{
    if (Log_Queue::MonitorLog_OBSOLETE == counter)
    {
        if (Log_Queue::ResetMonitorLog_OBSOLETE == m_lastResettedCounter)
        {
            return currentMax;
        }
        else
        {
            return SAPDB_MAX(currentMax, intermediateMax);
        }
    }
    else
    {
        if (Log_Queue::SystemViewLogQueueReset == counter)
        {
            if (Log_Queue::ResetSystemView == m_lastResettedCounter)
            {
                return currentMax;
            }
            else
            {
                return SAPDB_MAX(intermediateMax, currentMax);
            }
        }
        else
        // values since restart
        {
            return SAPDB_MAX(SAPDB_MAX(currentMax, intermediateMax), baseMax);
        }
    }
}

// -----------------------------------------------------------------------------

template <typename T> T Log_Queue::GetSumQueueCounterValue (const QueueCounterSelector counter,
                                                            const T basevalue,
                                                            const T offset,
                                                            const T currentCounter) const
{
    if (Log_Queue::MonitorLog_OBSOLETE == counter)
    {
        if (Log_Queue::ResetMonitorLog_OBSOLETE == m_lastResettedCounter)
        {
            return currentCounter;
        }
        else
        {
            return currentCounter + offset;
        }
    }
    else
    {
        if (Log_Queue::SystemViewLogQueueReset == counter)
        {
            if (Log_Queue::ResetSystemView == m_lastResettedCounter)
            {
                return currentCounter;
            }
            else
            {
                return currentCounter + offset;
            }
        }
        else
        // values since restart
        {
            return currentCounter + offset + basevalue;
        }
    }
}

// -----------------------------------------------------------------------------
SAPDB_UInt Log_Queue::GetMaxPagesUsed(const Log_Queue::QueueCounterSelector statSelect) const
{
    return GetMaxQueueCounterValue( statSelect, 
                                    m_StatisticalBaseValues.m_MaxPagesUsed,
                                    m_StatisticalIntermediateValues.m_MaxPagesUsed,
                                    m_StatisticalCurrentValues.m_MaxPagesUsed);
}
// -----------------------------------------------------------------------------
SAPDB_UInt Log_Queue::GetQueueFullSuspendCount(const Log_Queue::QueueCounterSelector statSelect) const
{
    return GetSumQueueCounterValue( statSelect, 
                                    m_StatisticalBaseValues.m_QueueFullSuspendCount,
                                    m_StatisticalIntermediateValues.m_QueueFullSuspendCount,
                                    m_StatisticalCurrentValues.m_QueueFullSuspendCount);
}
// -----------------------------------------------------------------------------
SAPDB_UInt Log_Queue::GetReserveCount(const Log_Queue::QueueCounterSelector statSelect) const
{
    return GetSumQueueCounterValue( statSelect, 
                                    m_StatisticalBaseValues.m_ReserveCount,
                                    m_StatisticalIntermediateValues.m_ReserveCount,
                                    m_StatisticalCurrentValues.m_ReserveCount);
}
// -----------------------------------------------------------------------------
SAPDB_UInt Log_Queue::GetWaitCount(const Log_Queue::QueueCounterSelector statSelect) const
{
    return GetSumQueueCounterValue( statSelect, 
                                    m_StatisticalBaseValues.m_WaitCount,
                                    m_StatisticalIntermediateValues.m_WaitCount,
                                    m_StatisticalCurrentValues.m_WaitCount);
}
// -----------------------------------------------------------------------------
SAPDB_UInt Log_Queue::GetGroupCommitCount(const Log_Queue::QueueCounterSelector statSelect) const
{
    return GetSumQueueCounterValue( statSelect, 
                                    m_StatisticalBaseValues.m_GroupCommitCount,
                                    m_StatisticalIntermediateValues.m_GroupCommitCount,
                                    m_StatisticalCurrentValues.m_GroupCommitCount);
}
// -----------------------------------------------------------------------------
SAPDB_UInt Log_Queue::GetMaxCountOfWaitingTasksPerPage(const Log_Queue::QueueCounterSelector statSelect) const
{
    return GetMaxQueueCounterValue( statSelect, 
                                    m_StatisticalBaseValues.m_MaxCountOfWaitingTasksPerPage,
                                    m_StatisticalIntermediateValues.m_MaxCountOfWaitingTasksPerPage,
                                    m_StatisticalCurrentValues.m_MaxCountOfWaitingTasksPerPage);
}
// -----------------------------------------------------------------------------
SAPDB_UInt Log_Queue::GetPhysWriteCount(const Log_Queue::QueueCounterSelector statSelect) const
{
    return GetSumQueueCounterValue( statSelect, 
                                    m_StatisticalBaseValues.m_WriteCount,
                                    m_StatisticalIntermediateValues.m_WriteCount,
                                    m_StatisticalCurrentValues.m_WriteCount);
}
// -----------------------------------------------------------------------------
Log_Queue::Log_QueueStatistics Log_Queue::CombineStatistics (const Log_Queue::Log_QueueStatistics & operand1,
                                                             const Log_Queue::Log_QueueStatistics & operand2)
{
    Log_Queue::Log_QueueStatistics result;

    result.m_QueueFullSuspendCount         = operand1.m_QueueFullSuspendCount        + operand2.m_QueueFullSuspendCount;
    result.m_ReserveCount                  = operand1.m_ReserveCount                 + operand2.m_ReserveCount ;
    result.m_WaitCount                     = operand1.m_WaitCount                    + operand2.m_WaitCount ;
    result.m_GroupCommitCount              = operand1.m_GroupCommitCount             + operand2.m_GroupCommitCount ;
    result.m_MaxCountOfWaitingTasksPerPage = SAPDB_MAX(operand1.m_MaxCountOfWaitingTasksPerPage, operand2.m_MaxCountOfWaitingTasksPerPage);
    result.m_MaxPagesUsed                  = SAPDB_MAX(operand1.m_MaxPagesUsed, operand2.m_MaxPagesUsed);
    result.m_WriteCount                    = operand1.m_WriteCount                   + operand2.m_WriteCount ; 

    return result;
}

// -----------------------------------------------------------------------------
void Log_Queue::ResetStatistics(tsp00_TaskId taskid, const Log_Queue::QueueStatisticSelector selectedCounter)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Queue::ResetStatistics", LogVolume_Trace, 5);

    EnterRegion(taskid);

    if (selectedCounter == m_lastResettedCounter)
    {
        m_StatisticalIntermediateValues = CombineStatistics(m_StatisticalIntermediateValues, m_StatisticalCurrentValues);
    }
    else
    {
        m_StatisticalBaseValues = CombineStatistics(m_StatisticalBaseValues, m_StatisticalIntermediateValues) ;
        m_StatisticalIntermediateValues = m_StatisticalCurrentValues;
        
    }
    m_StatisticalCurrentValues = Log_QueueStatistics();
    m_lastResettedCounter = selectedCounter;

    LeaveRegion(taskid); 

    m_timeOfLastReset = RTESys_MicroSecondTime();
}
// -----------------------------------------------------------------------------





