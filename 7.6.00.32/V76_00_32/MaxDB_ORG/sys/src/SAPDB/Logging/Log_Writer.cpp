/*!*****************************************************************************
  module:       Log_Writer.cpp
  responsible:  UweH
  author:       UweH,TillL
  special area: Archive log
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

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_ExtendedState.hpp"
#include "Logging/Log_Writer.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_Messages.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Transaction/Trans_Context.hpp"

#include "hkb38_1.h"
#include "hkb57_1.h" // k57restartrec
#include "hgg01_3.h"
#include "heo52.h"   // vclock
#include "heo60.h"   // vinsert_event
#include "vsp0031.h" // tsp31_event_description
#include "hgg01_2.h" // g01event_init
// -----------------------------------------------------------------------------
// suspend reasons
// -----------------------------------------------------------------------------
/// suspend reason for the Log_Writer, if the Log_Volume is full
#define LOG_DEVICE_FULL_SUSPEND    241
// PTS 1114053 UH 2002-10-22 new
/// suspend reason for a task, which waits for the Log-Writer to be suspended
#define WAIT_FOR_LOGWRITER_SUSPEND 247
// PTS 1114053 UH 2002-10-22 new
/// suspend reason for the Log_Writer, if it is suspended by a user command
#define LOGWRITER_USER_SUSPEND     248

/*******************************************************************************

   class: Log_Writer

*******************************************************************************/

/* global constant of class Log_Writer */
/// describes the minimal distance in percent of the log_size
/// from the writer-position to the last written savepoint
#define LOGWRITER_MIN_DIST_TO_SAVEPOINT  33

/// 5000 iosequences must be written before a savepoint must be triggered.
#define MIN_IOSEQUENCES_BEFORE_SAVEPOINT 5000

// -----------------------------------------------------------------------------

void Log_Writer::TriggerEvent()
{
    // PTS UH 2004-11-17 new
    tsp31_event_description desc;
    g01event_init (desc);   
    desc.sp31ed_ident.becomes    (sp31ei_logfull);
    desc.sp31ed_priority.becomes (sp31ep_high);
    vinsert_event (desc);
}

// -----------------------------------------------------------------------------

void Log_Writer::TriggerEvent(const Log_IOSequenceNo &oldestNotSaved)
{
    // PTS UH 2004-11-17 new
    if ( oldestNotSaved.IsInvalid() )
        return;

    if ( m_LastEventOldestNotSaved.IsInvalid()
         &&
         oldestNotSaved != m_LastEventOldestNotSaved )
    {
        m_LastEventOldestNotSaved = oldestNotSaved;
        tsp31_event_description desc;
        g01event_init (desc);   
        desc.sp31ed_ident.becomes    (sp31ei_logsegmentfull);
        desc.sp31ed_priority.becomes (sp31ep_low);
        vinsert_event (desc);
    }
}

// -----------------------------------------------------------------------------

Log_Writer::Log_Writer (SAPDBMem_IRawAllocator     &alloc,
                        QueueVector                &logqueues,
                        IOMan_ILogIO               &pagewriter,
                        Log_WriterTaskSynchronizer &writersync)
: m_LogQueues         (logqueues),
  m_PageWriter        (pagewriter),
  m_WriterTaskSync    (writersync),
  m_QueueRequests     (alloc),
  m_RequestPages      (alloc, UTF8( "Log_Writer::WriteLogPages" )),
  m_RequestPagesHelper(alloc, UTF8( "Log_Writer::WriteLogPages" )),
  m_IOSequenceNo      (Log_IOSequenceNo()),  // invalid
  m_Active            (false),
  m_Allocated         (false),
  m_FrameCounter      (0),
  m_CheckQSequence    (Log_IOSequenceNo()),  // invalid
  m_CheckWSequence    (Log_IOSequenceNo()),  // invalid
  m_CheckOffsetCount  (0),
  m_SuspendedByUser   (false),          // PTS 1114053 UH 2002-10-22 new
  m_UserWaitingForSuspend (cgg_nil_pid),// PTS 1114053 UH 2002-10-22 new
  m_LogUsageEvents    (sp31ei_log_above_limit, sp31ei_nil), // PTS 1124743 UH 2003-10-21
  m_PagesFreeOnDevice (0) // PTS 1129443 mb 2004-05-03
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::Log_Writer", LogVolume_Trace, 5);
    m_LogUsageEvents.EnableEventing();
}

// -----------------------------------------------------------------------------

void Log_Writer::Run()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::Run", LogVolume_Trace, 5);
    
    m_Active = true;

    // ----------------------------------------------------------
    // check if writer has been initialized
    // ----------------------------------------------------------
    if (m_Allocated == false)
    {
        // writer not yet initialized:
        m_WriterTaskSync.Suspend(LOG_WRITER_NOT_INITIALIZED);
        // in the meantime, the writer might have been deactivated:
        if (m_Active == false)
            return;
        // writer may still not be initialized:
        if (m_Allocated == false)
            return;
    }

    // ----------------------------------------------------------
    // check for for added log volume to be activated
    // ----------------------------------------------------------
    if ( EnableAddedLogDeviceIsPossible() )
        EnableAddedLogDevice();

    // ----------------------------------------------------------
    // set start position for queue loop:
    // ----------------------------------------------------------
    m_PagesFreeOnDevice = FreePagesForLogWriter(); // PTS 1130059 mb 2004-06-09
    
    if ( EnoughPagesFreeForUserTaskWrite(m_PagesFreeOnDevice) )                   // PTS 1115488 mb 2002-05-03
    {
        // enable write for everybody into the Log_Queue
        SetLogFullQueueState(m_WriterTaskSync.GetTaskID(), false);
    }

    SAPDB_RangeIterator<int, int> currentQueueIndex(0, 0, m_LogQueues.GetSize()-1); // PTS 1135304 mb 2005-05-25

    while (m_Active == true)
    {
        // ----------------------------------------------------------
        // check for log full and suspend if necessary
        // ----------------------------------------------------------
        while ( m_PagesFreeOnDevice == 0 )
        {
            RTE_Message(Log_Exception(__CONTEXT__,LOG_IS_FULL));

            if ( LogVolume_Trace.TracesLevel(6) )
                WriteToTrace("LOG FULL");
            
            m_WriterTaskSync.Suspend(LOG_DEVICE_FULL_SUSPEND);
            
            if (m_Active == false)
                return;
            
            m_PagesFreeOnDevice = FreePagesForLogWriter();
            
            if ( ! EnoughPagesFreeForUserTaskWrite(m_PagesFreeOnDevice) )                   // PTS 1115488 mb 2002-05-03
            {
                // only commit,rollback and savepoint are allowed due to log full
                SetLogFullQueueState(m_WriterTaskSync.GetTaskID(), true);
            }
        }

        // ----------------------------------------------------------
        // get pages for i/o:
        // ----------------------------------------------------------

        SAPDB_Bool firstflushedagain = false; // indicates whether last written page on device must be overwritten

		m_RequestPages.Clear();
        m_QueueRequests.Clear();

        // ----------------------------------------------------------
        // loop associated log queues; accumulate log pages for i/o
        // ----------------------------------------------------------

        SAPDB_Int numOfQueuesChecked = 0;
        m_WriterTaskSync.InitCycle();
        int absCurrentQueueIndex; // PTS 1138219 mb 2005-10-07
        
        // check the number of queues which actually has to be scanned (PTS 1141276 mb 2006-05-05)
        const SAPDB_Int4 queuesActive = m_ArchiveLog->GetNumQueuesActiveDirty();
        if (currentQueueIndex.GetEndOffset() != (queuesActive - 1))
        {
            if (queuesActive > 0)
            {
                if (currentQueueIndex.GetEndOffset() > (queuesActive - 1))
                // the number of active queues has been decreased
                {
                    // do not check the unused queues anymore
                    if (currentQueueIndex.GetPosition() > (queuesActive - 1))
                    {
                        currentQueueIndex.SetPosition(0);
                    }
                }
                currentQueueIndex.SetEndOffset(queuesActive-1);
            }
        }
        do 
        {
            absCurrentQueueIndex = currentQueueIndex.GetPosition();
            Log_Queue & currentQueue = *m_LogQueues[absCurrentQueueIndex];
            SAPDB_UInt pagecount = 0;
            
            bool firstFlushedAgainOfThisQueue;
            bool useOnlyQueue0;

            currentQueue.GetOutput (m_PagesFreeOnDevice - m_RequestPages.GetPageCount(),
                                    m_RequestPages,
                                    pagecount,
                                    firstFlushedAgainOfThisQueue,
                                    useOnlyQueue0);

            SAPDBERR_ASSERT_STATE(absCurrentQueueIndex == currentQueue.GetID());
            if (0 == absCurrentQueueIndex) // only queue 0 may run in MinimizeSpace-Mode
                firstflushedagain = firstFlushedAgainOfThisQueue;
            if (pagecount > 0)
            {
                #ifdef SAPDB_SLOW
                if ( 0 != absCurrentQueueIndex && firstFlushedAgainOfThisQueue )
                    RTE_Crash( Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "firstFlushedAgainOfThisQueue == false") );
                #endif
                m_QueueRequests.InsertEnd (QueueRequest(&currentQueue, pagecount));
            }

            if ( useOnlyQueue0 )
            {
                currentQueueIndex.SetPosition(0);
                break;
            }

            ++currentQueueIndex;
            ++numOfQueuesChecked;
        }
        while ( ! m_RequestPages.IsFull()
                &&
                (numOfQueuesChecked < queuesActive)
                &&
                (m_PagesFreeOnDevice > m_RequestPages.GetPageCount()) );
        
        // ----------------------------------------------------------
        // check whether there is output available:
        // ----------------------------------------------------------

        if (m_RequestPages.IsEmpty())
        {
            // no output available but free pages on the log
            m_WriterTaskSync.SuspendIfNoResumeCalled();
            // in the meantime, the writer might have been deactivated:
            if (m_Active == false)
                return;
        }
        else
        {
            Log_IOSequenceNo        ioseqnoBeforeFlush = m_IOSequenceNo;
            Log_RawDeviceIterator   firstWrittenPosition;

            // prepare and flush log pages:
            this->PrepareAndFlushPageVector(firstflushedagain, firstWrittenPosition);

            // remind queues that writer is ready:
            for (SAPDB_UInt i=0; i<m_QueueRequests.GetSize(); i++)
            {
                m_QueueRequests[i].pQueue->WriterTaskReady ( m_WriterTaskSync.GetTaskID(), 
                                                             ioseqnoBeforeFlush, 
                                                             firstWrittenPosition,
                                                             m_QueueRequests[i].pageCount );

                ioseqnoBeforeFlush   += m_QueueRequests[i].pageCount;
                firstWrittenPosition += m_QueueRequests[i].pageCount;
            }
            m_PagesFreeOnDevice -= m_RequestPages.GetPageCount();
            if (firstflushedagain)
                ++m_PagesFreeOnDevice;
            

            // ----------------------------------------------------------
            // check for for added log volume to be activated
            // ----------------------------------------------------------
    
            if ( EnableAddedLogDeviceIsPossible() )
            {
                EnableAddedLogDevice();
                m_PagesFreeOnDevice = FreePagesForLogWriter();
            }

            if ( ! EnoughPagesFreeForUserTaskWrite(m_PagesFreeOnDevice) )                   // PTS 1115488 mb 2002-05-03
            {
                m_PagesFreeOnDevice = FreePagesForLogWriter();
                if ( ! EnoughPagesFreeForUserTaskWrite(m_PagesFreeOnDevice) )                   // PTS 1115488 mb 2002-05-03
                {
                    // only commit,rollback and savepoint are allowed due to log full
                    SetLogFullQueueState(m_WriterTaskSync.GetTaskID(), true);
                }
            }
        }

        // ----------------------------------------------------------
        // check whether logwriter should be suspended by user
        // ----------------------------------------------------------
        if ( m_SuspendedByUser )
            WaitForUserResume();
            
    } // mainloop while (m_Active)
}

// -----------------------------------------------------------------------------

void Log_Writer::PrepareAndFlushPageVector
    (SAPDB_Bool             firstflushedagain,
     Log_RawDeviceIterator &firstWrittenPosition)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::PrepareAndFlushPageVector", LogVolume_Trace, 5);

    // ----------------------------------------------------------
    // determine write position on raw device:
    // ----------------------------------------------------------
    m_FrameCounter += m_RequestPages.GetPageCount();            //PTS 1117709 mb 2002-08-05
    if (firstflushedagain == true)
    {
        // works only in single-queue mode:
        --m_WriteIterator;
        --m_FrameCounter;                                       //PTS 1117709 mb 2002-08-05
    }
    
    firstWrittenPosition = m_WriteIterator; 
    
    SAPDB_Bool must_flush_infopage = 
                    m_FrameCounter >= LOG_WRITER_INFOPAGEWRITECYCLE
                    ||
                    m_LogInfoPage.ReadDeviceState() == Log_DeviceStateCleared
                    ||
                    m_LogInfoPage.ReadDeviceState() == Log_DeviceStatePartiallyCleared; //PTS 1111525 mb 2002-01-29
    
    // ----------------------------------------------------------
    // mark pages with writer i/o seq. numbers:
    // ----------------------------------------------------------

    IOMan_LogPagesIterator  iter( m_RequestPages );
    Kernel_DateTime         dateTime;
    dateTime.DetermineTime();

    if ( LogVolume_Check.ChecksLevel(5)
         &&
	 ! m_LogInfoPage.ReadAutoOverwrite()
         &&
         m_WriteIterator.GetPosition() != m_LogInfoPage.ReadOldestNotSavedOffset()
         &&
         m_WriteIterator.GetDistanceFrom(m_LogInfoPage.ReadOldestNotSavedOffset()) + m_RequestPages.GetPageCount()
         >
         m_WriteIterator.GetSize() )
    {
        m_LogInfoPage.WriteToTrace();
        for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize(); qid++)
        {
            m_LogQueues[qid]->WriteToTrace();
        }
        WriteToTrace("WRITER");
	if ( LogVolume_Check.ChecksLevel(7) )
	    RTE_Crash( SAPDBErr_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
	               "OldestNotSaved offset must not be overwritten") );
	else
	    RTE_Message( SAPDBErr_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
	                 "OldestNotSaved offset must not be overwritten") );
    }

    for ( iter.Begin(); ! iter.End(); ++iter )
    {
        Log_Page &page = *iter;
        
        page.WriteWriterIOSequenceNo(m_IOSequenceNo);
        page.WriteDate              (dateTime.GetDate());
        page.WriteTime              (dateTime.GetTime());
        ++m_IOSequenceNo;
    }
    
    #ifdef SAPDB_QUICK
    if ( LogVolume_Check.ChecksLevel(5) )
    {
        iter.Begin();
        if ( m_CheckQSequence.IsValid() )
        {
            if ( m_CheckQSequence == (*iter).ReadQueueIOSequenceNo()
                 &&
                 ! firstflushedagain )
                RTE_Crash( Log_Exception(__CONTEXT__,
                                              SAPDBERR_ASSERT_STATE_FAILED,
                                              "Log_Writer queue seq mismatch") );
        }
        m_CheckQSequence = (*iter).ReadQueueIOSequenceNo();
        
        Log_IOSequenceNo lastWsequence = (*iter).ReadLastWriterIOSequenceNo();
        SAPDB_UInt   offsetcount   = (*iter).ReadOffsetCount();
        if ( LogVolume_Trace.TracesLevel(6) )
            Kernel_VTrace() << "lastWsequence: " << lastWsequence
                            << ", offsetcount: " << offsetcount
                            << ", firstflushedagain: " << firstflushedagain
                            << NewLine;
        
        if ( m_CheckWSequence.IsValid() && firstflushedagain )
        {
            if ( m_CheckWSequence+1 != (*iter).ReadLastWriterIOSequenceNo() )
                RTE_Crash( Log_Exception(__CONTEXT__,
                                              SAPDBERR_ASSERT_STATE_FAILED,
                                              "Log_Writer writer seq mismatch") );
            if ( m_CheckOffsetCount+1 != (*iter).ReadOffsetCount() )
                RTE_Crash( Log_Exception(__CONTEXT__,
                                              SAPDBERR_ASSERT_STATE_FAILED,
                                              "Log_Writer offsetcount mismatch") );
        }
    
        iter.End();
        m_CheckWSequence   = (*iter).ReadLastWriterIOSequenceNo();
        m_CheckOffsetCount = (*iter).ReadOffsetCount();
    }
    #endif
    
    // ----------------------------------------------------------
    // flush page vector:
    // ----------------------------------------------------------
    Log_DeviceBlockCount freespace = m_WriteIterator.GetEndDistance() + 1;
    if ( freespace > m_RequestPages.GetPageCount() )
    {
        // write whole cluster at once
        this->FlushPages(m_RequestPages);
    }
    else if ( freespace == m_RequestPages.GetPageCount() )
    {
        if ( EnableAddedLogDeviceIsPossible() )
            EnableAddedLogDevice();
        // write whole cluster at one:
        this->FlushPages(m_RequestPages);
    }
    else if ( EnableAddedLogDeviceIsPossible() )
    {
        EnableAddedLogDevice();
        // write whole cluster at once
        this->FlushPages(m_RequestPages);
    }
    else
    {
        // must write two chunks:
        SAPDB_UInt             i;
        IOMan_LogPagesIterator iter( m_RequestPages );

        // write first page cluster:
        m_RequestPagesHelper.Clear();

        for (i=0, iter.Begin(); i<freespace; ++i, ++iter)
        {
            m_RequestPagesHelper.Append( *iter );
        }
        this->FlushPages(m_RequestPagesHelper);

        // write second page cluster:
        m_RequestPagesHelper.Clear();
        for (i=freespace; i<m_RequestPages.GetPageCount(); ++i, ++iter)
        {
            m_RequestPagesHelper.Append( *iter );
        }
        this->FlushPages(m_RequestPagesHelper);
    }

    // update and write out info page if required:
    if (must_flush_infopage)
    {
        m_ArchiveLog->UpdateInfoPageFromWriter ( m_WriterTaskSync.GetTaskID(),
                                                 m_IOSequenceNo-1,
                                                 (m_WriteIterator-1).GetPosition() );
        if (m_FrameCounter >= LOG_WRITER_INFOPAGEWRITECYCLE)
            m_FrameCounter -= LOG_WRITER_INFOPAGEWRITECYCLE;
    }
}

// -----------------------------------------------------------------------------
void Log_Writer::EnableAddedLogDevice()
{
    if (m_NewDeviceEndOffset.RawValue() < 
        (m_WriteIterator.GetEndOffset().RawValue() + m_RequestPages.GetPageCount()))
    {
        // fatal error: must increase log size for a sufficient amount
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Log_Writer increase log size failed"));
    }
    // ----------------------------------------------------------
    // - add log devspace has occurred
    // - end of old log device has been reached
    // - expand write cycle to new device now!
    // ----------------------------------------------------------
    m_WriteIterator.SetEndOffset(m_NewDeviceEndOffset);
    // ----------------------------------------------------------
    // make new cycle related log size persistent first:
    // ----------------------------------------------------------
    m_ArchiveLog->ChangeEndOffsetAfterAddDevSpace( m_WriterTaskSync.GetTaskID(),
                                                   m_NewDeviceEndOffset );

    m_LogUsageEvents.Rescale(UserLogSize()); // PTS 1124876 UH 2003-10-23

    m_NewDeviceEndOffset.Invalidate();
}

// -----------------------------------------------------------------------------

void Log_Writer::FlushPages ( IOMan_LogPages &pages)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::FlushPages", LogVolume_Trace, 5);

    Log_ClusterAddress  addr (m_WriteIterator.GetPosition(), pages.GetPageCount());
 
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, ", offset: " << m_WriteIterator.GetPosition() << ", pages: " << pages.GetPageCount());
    
    m_PageWriter.WriteLogPages (m_WriterTaskSync.GetTaskID(), pages, addr);

    m_WriteIterator += pages.GetPageCount();
}

// -----------------------------------------------------------------------------

Log_RawDeviceOffset Log_Writer::GetOldestRequiredOffset() const
{
    Log_RawDeviceOffset saveptOffset         = m_LogInfoPage.ReadSavepointEntryOffset();
    Log_RawDeviceOffset oldestNotSavedOffset = m_LogInfoPage.ReadOldestNotSavedOffset();

    if ( saveptOffset.IsInvalid())
    {
        if ( oldestNotSavedOffset.IsValid() )
            return oldestNotSavedOffset;
    }
    else
    {
        if (oldestNotSavedOffset.IsValid())
        {
            if (m_LogInfoPage.ReadOldestNotSavedIOSequenceNo().IsValid())
            {
                if (m_LogInfoPage.ReadSavepointIOSequenceNo().IsValid())
                {
                    // two valid values - return the offset of the lower (older) sequence
                    // PTS 1124727 mb 2003-10-20 also take care of eventual overflows
                    if (Log_IOSequenceNo::LeftIsOlder(m_LogInfoPage.ReadOldestNotSavedIOSequenceNo(),
                                                      m_LogInfoPage.ReadSavepointIOSequenceNo(),
                                                      m_LogInfoPage.ReadOldestKnownIOSequenceNo()))
                    {
                        return oldestNotSavedOffset;
                    }
                    else
                    {
                        return saveptOffset;
                    }
                }
            }                    
        }
        else //oldestNotSavedOffset.IsInvalid()
        {
            return saveptOffset;
        }
    }

    // no valid value at all => the first offset must be available for restart
    return m_WriteIterator.GetBeginOffset();

}

// -----------------------------------------------------------------------------
Log_DeviceBlockCount Log_Writer::FreePagesForLogWriter() const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::FreePagesForLogWriter", LogVolume_Trace, 5);

    if ( m_IOSequenceNo.IsInvalid() )
        return m_WriteIterator.GetSize() - LOG_RESTART_RESERVE_SPACE;

    Log_RawDeviceIterator firstFreePage = m_WriteIterator;

    ++firstFreePage;
    
    Log_DeviceBlockCount distance = firstFreePage.GetDistanceTo(GetOldestRequiredOffset());
    
    // PTS 1111862 MB 2001-09-19
    // in case of a restart the LOG_FULL_MARGIN pages are used
    // for writing the first savepoint onto the log_device
    if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() )
        return distance;
    
    // in case of normal operation there are LOG_FULL_MARGIN pages reserved
    // for the restart-case
    if (distance < LOG_RESTART_RESERVE_SPACE)
        return 0;

	return distance - LOG_RESTART_RESERVE_SPACE;
}

// -----------------------------------------------------------------------------

void Log_Writer::CheckForLogSaveAndSavepoint(Trans_Context &context)
{
    // PTS 1117895 UH 2002-10-21 now called by timeout task
    
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::CheckForLogSaveAndSavepoint", LogVolume_Trace, 5);
    
    //
    // This is called by the timeout task !!
    //

	if ( context.OldTrans().trError_gg00 != e_ok )
        return;
    
    // reading dirty - autosave task may run parallel and change these values
    const Log_RawDeviceIterator copiedWriterIterator = m_WriteIterator;
    const Log_RawDeviceOffset   oldestNotSavedOffset =
                                 m_LogInfoPage.ReadOldestNotSavedOffset().IsValid()
                                 ? m_LogInfoPage.ReadOldestNotSavedOffset()
                                 : copiedWriterIterator.GetBeginOffset();
    const Log_DeviceBlockCount  notsaveddist         = copiedWriterIterator.GetDistanceFrom(oldestNotSavedOffset);
    const SAPDB_UInt            autosaveDistance     = m_ArchiveLog->GetAutosaveDistance();

    if ( LogIsFull() ) // PTS UH 2004-11-17 added
        TriggerEvent();

    if ( notsaveddist >= autosaveDistance ) // PTS 1113361 mb 2002-01-11
    {
        TriggerEvent(m_LogInfoPage.ReadOldestNotSavedIOSequenceNo()); // PTS UH 2004-11-17 added
        k38trigger_autosave (context.OldTrans(), false);
        if ( context.OldTrans().trError_gg00 != e_ok )
        {
            RTE_Message(context.ErrorList());
            context.ErrorList().ClearMessageList();
            context.OldTrans().trError_gg00 = e_ok;
        }
    }

    // PTS 1124876 UH 2003-10-23
    // rewritten without usage of percentage values
    // added Eventhandling
    
    const Log_DeviceBlockCount logsize   = UserLogSize();
    const Log_DeviceBlockCount usedPages = UsedPages();

    m_LogUsageEvents.Watch(usedPages, false);

    // check if a savepoint has to be written

    if (usedPages * 3 > logsize * 2) // usedPages > 2/3 logsize
    {
        Log_RawDeviceOffset startOffset = k57restartrec->rstLastSavept_kb00().svpStartOffset_kb00;

        if ( startOffset.IsInvalid() )
            startOffset = copiedWriterIterator.GetBeginOffset();
            
        if ( copiedWriterIterator.GetDistanceTo(startOffset) * 3  < logsize )
        {
            if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepoint (context.Task(), Log_SVPReasonLogWriter, context.ErrorList()) )
				RTE_Message(context.ErrorList());
            return;
        }
    }

    // if any log was written but not as much as checked before
    // then check if the time defined in RESTART_TIME is reached since last savepoint
    // GetIOSequenceNo() returns the NEXT iosequence which is to be used.
    // writerIOseq - 1 == svpIOseq => nothing was written to the log.

    if ( Log_IOSequenceNo::LeftIsYounger (GetIOSequenceNo(),
                                          k57restartrec->rstLastSavept_kb00().svpIOsequence_kb00
                                                          + 1 + MIN_IOSEQUENCES_BEFORE_SAVEPOINT,
                                          m_LogInfoPage.ReadOldestKnownIOSequenceNo()))
    {
        Kernel_DateTime delayedTime;

        delayedTime.DetermineDelayedTime(g01restart_time); // PTS 1139588 UH 2006-01-10 

        if ( ! delayedTime.OlderThan( k57restartrec->rstLastSavept_kb00().svpDate_kb00,
                                      k57restartrec->rstLastSavept_kb00().svpTime_kb00) )
        {
            if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepoint (context.Task(), Log_SVPReasonTime, context.ErrorList()) )
				RTE_Message(context.ErrorList());
        }
    }
}

// -----------------------------------------------------------------------------

Log_DeviceBlockCount Log_Writer::UsedPages() const 
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::UsedPages", LogVolume_Trace, 5);
    Log_DeviceBlockCount userLogSize = UserLogSize();
    Log_DeviceBlockCount usedPages   = m_WriteIterator.GetDistanceFrom(GetOldestRequiredOffset());
    return usedPages > userLogSize ? userLogSize : usedPages;
}

// -----------------------------------------------------------------------------

Log_DeviceBlockCount Log_Writer::UserLogSize() const
{
    // PTS 1124876 UH 2003-10-23 new
    if ( m_WriteIterator.GetSize() < m_PagesNotAvailableForUsers ) // PTS 1134810 UH 2005-04-12
        return 0;
	else
        return m_WriteIterator.GetSize() - m_PagesNotAvailableForUsers; // PTS 1134810 UH 2005-04-12
}

// -----------------------------------------------------------------------------

void Log_Writer::SetLogFullQueueState( tsp00_TaskId taskId,
                                       bool         allowOnlySavepointCommit )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::SetLogFullQueueState", LogVolume_Trace, 5);

    if ( allowOnlySavepointCommit )
    {
        Kernel_ExtendedState::UpdateLogvolumeFullState(1);
        RTE_Message(Log_Exception(__CONTEXT__,LOG_IS_FULL));
    }
    else
        Kernel_ExtendedState::UpdateLogvolumeFullState(0);

    for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize(); qid++) // PTS 1135304 
    {
        m_LogQueues[qid]->SetLogFullState(taskId, allowOnlySavepointCommit );
    } 
}

// -----------------------------------------------------------------------------
bool Log_Writer::SuspendAndGetLastWrittenIOSequence ( tsp00_TaskId            taskid,
                                                      Log_IOSequenceNo &iosequence )
{
    // PTS 1114053 UH 2002-10-22 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::SuspendAndGetLastWrittenIOSequence", LogVolume_Trace, 5);

    if ( m_SuspendedByUser )
        return false;

    m_SuspendedByUser       = true;
    m_UserWaitingForSuspend = taskid;
    
    m_WriterTaskSync.Resume();
    
    RTE_Message( Log_Exception(__CONTEXT__, LOG_WRITER_USER_SUSPENDED) );

    vsuspend(taskid, WAIT_FOR_LOGWRITER_SUSPEND);

    // The logwriter is suspended now.
    iosequence = m_IOSequenceNo;
    
    return true;
}

// -----------------------------------------------------------------------------
void Log_Writer::ResumeByUser ()
{
    // PTS 1114053 UH 2002-10-22 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::ResumeByUser", LogVolume_Trace, 5);
    RTE_Message( Log_Exception(__CONTEXT__, LOG_WRITER_USER_RESUMED) );
    m_SuspendedByUser = false;
    m_WriterTaskSync.Resume();
}

// -----------------------------------------------------------------------------
void Log_Writer::WaitForUserResume ()
{
    // PTS 1114053 UH 2002-10-22 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::WaitForUserResume", LogVolume_Trace, 5);
    if ( m_UserWaitingForSuspend != cgg_nil_pid )
    {
        vresume(m_UserWaitingForSuspend);
        m_UserWaitingForSuspend = cgg_nil_pid;
    }
    
    RTE_Message( Log_Exception(__CONTEXT__,
                               LOG_WRITER_SUSPENDED_BY_USER,
                               SAPDB_ToString(m_IOSequenceNo)) );
    
    do
        m_WriterTaskSync.Suspend (LOGWRITER_USER_SUSPEND);
    while (m_SuspendedByUser);
}

// -----------------------------------------------------------------------------
void Log_Writer::WriteToTrace (const char * title) const
{
    // PTS 1134810 UH 2005-04-12 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Writer::WriteToTrace", LogVolume_Trace, 5);
    
    Kernel_VTrace trace;
    
    if ( title != 0 )
        trace << title << FlushLine;

	trace << "Allocated:  " << m_Allocated << ", Active: " << m_Active << FlushLine;
    trace << "IOSequence: " << m_IOSequenceNo;
    trace << ( m_IOSequenceNo.IsInvalid() ? ", Log was never written." : "" ) << FlushLine;
    trace << "Pages since last flush info page            : " << m_FrameCounter << FlushLine;
    trace << "New volume end offset after 'add log volume': " << m_NewDeviceEndOffset << FlushLine;
    trace << "Pages not available for users               : " << m_PagesNotAvailableForUsers << FlushLine;
    trace << "Pages free on volume                        : " << m_PagesFreeOnDevice << FlushLine;
    trace << "FreePagesForLogWriter                       : " << FreePagesForLogWriter() << FlushLine;
    trace << "Last savepoint trigger time                 : " << m_LastSavepointTriggerTime << FlushLine;
    trace << "Current write offset                        : " << m_WriteIterator.GetPosition() << FlushLine;
    trace << "Loginfopage Offset                          : " << m_LogInfoPageOffset << FlushLine;
    trace << "Loginfopage Offset                          : " << m_LogInfoPageOffset << FlushLine;
    trace << "Check Queue Sequence                        : " << m_CheckQSequence << FlushLine;
    trace << "Check Writer Sequence                       : " << m_CheckWSequence << FlushLine;
    trace << "Check Offset Count                          : " << m_CheckOffsetCount << FlushLine;
    trace << "Suspended by user                           : " << m_SuspendedByUser << FlushLine;
    trace << "User waiting for suspend                    : T"<< m_UserWaitingForSuspend << FlushLine;
}
