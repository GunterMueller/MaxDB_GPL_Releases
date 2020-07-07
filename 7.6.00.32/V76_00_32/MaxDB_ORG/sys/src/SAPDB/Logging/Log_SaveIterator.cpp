/*!
    @file     Log_SaveIterator.cpp
    @author   MartinB
    @ingroup  Logging
    @brief    implementation of an iterator for saving one segment of LogPages

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

/*****************************************************************************

   class: Log_SaveIterator

******************************************************************************/


#include "Logging/Log_SaveIterator.hpp"
#include "Logging/Log_VolumeIterator.hpp"
#include "Logging/Log_ClusterAddress.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "hgg01.h"


// ---------------------------------------------------------------------------


SAPDB_UInt4         Log_SaveIterator::GetLogBackupCount () const
{
    return m_logBackupCount;
}


// ---------------------------------------------------------------------------
Log_IOSequenceNo    Log_SaveIterator::GetStartIOSequence () const
{
    return m_FirstIOSeqToSave;

}

// ---------------------------------------------------------------------------
Log_IOSequenceNo    Log_SaveIterator::GetEndIOSequence () const
{ 
    return m_LastIOSeqToSave;
}


// ---------------------------------------------------------------------------
Log_RawDeviceOffset    Log_SaveIterator::GetStartPageOffset() const
{ 
    return m_FirstPageToSave.GetPosition();
}



// ---------------------------------------------------------------------------
Log_RawDeviceOffset    Log_SaveIterator::GetEndPageOffset() const
{ 
    return m_LastPageToSave.GetPosition();
}


// ---------------------------------------------------------------------------
Log_DeviceBlockCount    Log_SaveIterator::GetSizeOfCurrentSegment() const
{
    Log_RawDeviceIterator lastPageToSave(m_LastPageToSave);
    // increment lastPageToSave because both pages are to be saved
    ++lastPageToSave;
    return m_FirstPageToSave.GetDistanceTo(lastPageToSave.GetPosition());
}

// ---------------------------------------------------------------------------
Log_DeviceBlockCount    Log_SaveIterator::GetNumPagesLeft() const
{
    Log_RawDeviceIterator lastPageToSave(m_LastPageToSave);
    // increment lastPageToSave because both pages are to be saved
    ++lastPageToSave;
    return m_CurrentPage.GetDistanceTo(lastPageToSave.GetPosition());
}



// ---------------------------------------------------------------------------
void Log_SaveIterator::GetStartTime (   Kernel_Date         &StartDate,
                                        Kernel_Time         &StartTime) const
{ 
    StartDate = m_startDate;
    StartTime = m_startTime;
}


// ---------------------------------------------------------------------------
void Log_SaveIterator::GetEndTime (    Kernel_Date    &EndDate,
                                       Kernel_Time    &EndTime) const
{ 
    EndDate = m_endDate;
    EndTime = m_endTime;
}


// ---------------------------------------------------------------------------
bool    Log_SaveIterator::GetNextLogPages (tsp00_TaskId           taskId,
                                           tkb3_block_ptr         bufferArray,
                                           Log_DeviceBlockCount  &numPagesRead,
                                           Msg_List              &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_SaveIterator::GetNextLogPages", LogVolume_Trace, 5);

    if (m_allPagesProcessed)
    {
        numPagesRead = 0;
        return true;
    }
    
    numPagesRead = m_CurrentPage.GetDistanceTo(m_LastPageToSave.GetPosition())+1;
    if (numPagesRead > Log_DeviceBlockCount(m_pagebufferSize))
    {
        numPagesRead = m_pagebufferSize;
    }
    else
    {
        m_allPagesProcessed = true;
    }


    if (m_CurrentPage.GetEndDistance()+1 >= numPagesRead)
    {
        Log_DeviceBlockCount i=0;
        m_pagebuffer.Clear();
        while (i < numPagesRead)
        {    
            Data_PageFrame        pageFrame( (*bufferArray)+i,
                                                      sizeof(tkb3_page));
            m_logPage.SetFrame(pageFrame);
            m_pagebuffer.Append(m_logPage);
            i++;
        }
        m_LogReader.ReadLogPages(   taskId,
                                    m_pagebuffer,
                                    Log_ClusterAddress( m_CurrentPage.GetPosition(),
                                                        numPagesRead));
        if ( ! CheckPageBuffer(errlist) )
            return false;
    }
    else
    {
        Log_DeviceBlockCount i=0;
        m_pagebuffer.Clear();
        while (i < m_CurrentPage.GetEndDistance()+1)
        {    
            Data_PageFrame        pageFrame( (*bufferArray)+i,
                                                      sizeof(tkb3_page));
            m_logPage.SetFrame(pageFrame);
            m_pagebuffer.Append(m_logPage);
            i++;
        }
        m_LogReader.ReadLogPages(   taskId,
                                    m_pagebuffer,
                                    Log_ClusterAddress( m_CurrentPage.GetPosition(),
                                                        m_CurrentPage.GetEndDistance()+1));
        if ( ! CheckPageBuffer(errlist) )
            return false;
        
        Log_RawDeviceIterator deviceBegin = m_CurrentPage + m_CurrentPage.GetEndDistance()+1;
        m_pagebuffer.Clear();
        while (i < numPagesRead)
        {    
            Data_PageFrame        pageFrame( (*bufferArray)+i,
                                                      sizeof(tkb3_page));
            m_logPage.SetFrame(pageFrame);
            m_pagebuffer.Append(m_logPage);
            i++;
        }
        m_LogReader.ReadLogPages(   taskId,
                                    m_pagebuffer,
                                    Log_ClusterAddress( deviceBegin.GetPosition(),
                                                        numPagesRead-(m_CurrentPage.GetEndDistance()+1)));
        if ( ! CheckPageBuffer(errlist) )
            return false;
    }
    m_CurrentPage += numPagesRead;
    return true;
}

bool Log_SaveIterator::CheckPageBuffer(Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_SaveIterator::CheckPageBuffer", LogVolume_Trace, 5);

    IOMan_LogPagesIterator  pageBufferIter(m_pagebuffer);
    pageBufferIter.Begin();
    Log_RawDeviceIterator   currentPage = m_CurrentPage;
    Log_IOSequenceNo        prevLastIOSeq  = (*pageBufferIter).ReadLastWriterIOSequenceNo();
    Log_IOSequenceNo        prevQueueIOSeq = (*pageBufferIter).ReadQueueIOSequenceNo();
    
    ++pageBufferIter;
    ++currentPage;

    while (!pageBufferIter.End() )
    {
        switch ( Log_VolumeIterator::CheckLogDeviceConsistency ( *pageBufferIter,
                                                                  currentPage.GetPosition(),
                                                                  prevLastIOSeq,
                                                                  prevQueueIOSeq,
                                                                  errlist ) )
        {
            case Log_VolumeIterator::consistent:
                break;
            case Log_VolumeIterator::inconsistent:
                (*pageBufferIter).WriteToTrace();
                WriteToTrace();
                errlist.Overrule (Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "read pages for save log are consistent"));
                return false;
            case Log_VolumeIterator::endOfLogDevice:
                (*pageBufferIter).WriteToTrace();
                WriteToTrace();
                errlist.Overrule( Log_Exception(__CONTEXT__, LOG_READ_IOSEQ_INCONSISTENT,
                                                SAPDB_ToString(currentPage.GetPosition()),
                                                SAPDB_ToString(prevLastIOSeq+1),
                                                SAPDB_ToString((*pageBufferIter).ReadFirstWriterIOSequenceNo()) ) );
                errlist.Overrule (Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "Log end found before end of log backup"));
                return false;
            default:
                (*pageBufferIter).WriteToTrace();
                WriteToTrace();
                errlist.Overrule(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "all returncode handled"));
                break;
        }

        prevLastIOSeq =  (*pageBufferIter).ReadLastWriterIOSequenceNo();
        prevQueueIOSeq = (*pageBufferIter).ReadQueueIOSequenceNo();

        ++pageBufferIter;
        ++currentPage;
    }
    return true;
}
    

// ---------------------------------------------------------------------------
    Log_SaveIterator::Log_SaveIterator(  SAPDBMem_IRawAllocator            &Allocator,
                                         Log_FrameAllocator                &FrameAlloc,
                                         IOMan_ILogIO                      &LogReader)

                                         :
    m_LogReader(LogReader),
    m_pagebuffer(Allocator, UTF8( "Log_SaveIterator::PageCollection ")),
    m_frameAlloc(FrameAlloc),
    m_allPagesProcessed(false),
    m_isValid(false)
{ 
    SAPDBTRACE_METHOD_DEBUG ("Log_SaveIterator::Log_SaveIterator", LogVolume_Trace, 5);
}


// ---------------------------------------------------------------------------
bool 
Log_SaveIterator::Initialize(const tsp00_TaskId                       taskId,
                             const SAPDB_UInt4                        backupCount,
                             const Log_RawDeviceOffset                saveRangeStart,
                             const Log_RawDeviceOffset                saveRangeEnd,
                             const Log_RawDeviceIterator              firstPageToSave,
                             const Log_RawDeviceIterator              lastPageToSave,
                             const SAPDB_Int2                         blockSize,
                             const SAPDB_Bool                         isLastSegmentToSave, // PTS1111987 martinb 2001-10-12
                             const SAPDB_Bool                         repeatable) // PTS 1128703 mb 2004-04-26
{
    SAPDBTRACE_METHOD_DEBUG ("Log_SaveIterator::Initialize", LogVolume_Trace, 5);
    
    if (IsInvalid())
    {
        m_saveRangeStart = saveRangeStart;
        m_saveRangeEnd = saveRangeEnd;
    }
        
    m_LastPageToSave = lastPageToSave;
    m_FirstPageToSave = firstPageToSave;
    m_CurrentPage = firstPageToSave;
    m_allPagesProcessed = false;
    m_pagebufferSize = blockSize;
    m_isLastSegmentToSave = isLastSegmentToSave;
    m_logBackupCount = backupCount;
    m_repeatable = repeatable;

    m_LastIOSeqToSave.Invalidate();
    m_FirstIOSeqToSave.Invalidate();

    if (m_pagebuffer.Initialize(m_pagebufferSize)) 
    {
        m_frame = m_frameAlloc.New();
        if (!m_frame.IsAssigned())
            return false;
            
        m_logPage.SetFrame(m_frame);
        

        m_LogReader.ReadLogPage(
            taskId,
            m_logPage,
            Log_ClusterAddress(m_LastPageToSave.GetPosition(),1));

        m_LastIOSeqToSave = m_logPage.ReadLastWriterIOSequenceNo();
        m_endDate = m_logPage.ReadDate();
        m_endTime= m_logPage.ReadTime();


        m_LogReader.ReadLogPage(
            taskId,
            m_logPage,
            Log_ClusterAddress(m_FirstPageToSave.GetPosition(),1));
        m_FirstIOSeqToSave = m_logPage.ReadFirstWriterIOSequenceNo();
        m_startDate = m_logPage.ReadDate();
        m_startTime= m_logPage.ReadTime();

        m_pagebuffer.Clear();
        m_isValid = true;
        
        return true;
     }
     else
     {
         return false;
     }
}

void Log_SaveIterator::GetNextSegmentRange(Log_RawDeviceOffset &startPoint, Log_RawDeviceOffset &endPoint)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_SaveIterator::GetNextSegmentRange", LogVolume_Trace, 5);

    SAPDBERR_ASSERT_STATE(IsValid());
    
    if (!IsLastSegmentToSave())
    // the last segment was complete => not yet all segments saved
    {
        startPoint = (++m_LastPageToSave).GetPosition();
        endPoint = m_saveRangeEnd;
    }
    else
    // everything saved last time this method was called
    {
        startPoint = m_saveRangeStart;
        endPoint = m_saveRangeEnd;
    }
}

// ---------------------------------------------------------------------------

void Log_SaveIterator::Invalidate()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_SaveIterator::Invalidate", LogVolume_Trace, 5);
    if (IsValid())
    {
        m_pagebuffer.Delete();
        m_frameAlloc.Free(m_frame); 
        m_isValid = false;
    }
}

// ---------------------------------------------------------------------------
void Log_SaveIterator::InvalidateNoLongerRequired()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_SaveIterator::InvalidateNoLongerRequired", LogVolume_Trace, 5);
    if (IsValid())
    {
        if ( IsLastSegmentToSave() )
        {
            Invalidate();
        }
    }
}

// ---------------------------------------------------------------------------
void Log_SaveIterator::WriteToTrace() const
{
    Kernel_VTrace trace;
    trace << "------------- Log_SaveIterator::WriteToTrace() begin -------------------" << NewLine;
    trace << "m_saveRangeStart = " << m_saveRangeStart
          << " m_saveRangeEnd = " << m_saveRangeEnd
          << NewLine;

    trace << "FirstPageToBeSaved =" << m_FirstPageToSave.GetPosition()
          << " LastPageToBeSaved = " << m_LastPageToSave.GetPosition()
          << " currentPosition=" << m_CurrentPage.GetPosition()
          << NewLine;
          
    trace << "FirstIOSeqToBeSaved =" << m_FirstIOSeqToSave
          << " LastIOSeqToBeSaved = " << m_LastIOSeqToSave
          << " LogBackupCount = " << m_logBackupCount << NewLine;;
          
    trace << "IsLastSegmentToSave =" << m_isLastSegmentToSave
          << " allPagesProcessed = " << m_allPagesProcessed
          << " repeatable = " << m_repeatable
          << " isValid = " << m_isValid
          << NewLine;
    trace << "------------- Log_SaveIterator::WriteToTrace() end -------------------" << FlushLine;
}

