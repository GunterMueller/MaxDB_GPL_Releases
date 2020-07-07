/*!
  @file           Log_SeqLogDevReader.hpp
  @author         MartinB
  @ingroup        LogVolume   
  @brief          This reader gives a sequential access to the log pages on a log device
  @see            

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



#ifndef LOG_SEQ_LOG_DEV_READER_HPP
#define LOG_SEQ_LOG_DEV_READER_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Logging/Log_Types.hpp"
#include "Logging/Log_RawDeviceIterator.hpp"
#include "Logging/Log_ClusterAddress.hpp"
#include "IOManager/IOMan_ILogIO.hpp"
#include "IOManager/IOMan_ILogInfo.hpp"
#include "Logging/Log_ISequentialReader.hpp"
#include "Restart/Rst_StandbySynchronizer.hpp"

/*!
   @brief          This offers an iterator, to access afterimages from the
                   archive log.
 */
class Log_SeqLogDevReader : public Log_ISequentialReader
{

public: 

    /// constructor
    Log_SeqLogDevReader    (Log_RawDeviceOffset          deviceBegin,
                            Log_RawDeviceOffset          deviceEnd,
                            IOMan_ILogIO            &pageReader,
                            Rst_StandbySynchronizer &standbySynchronizer,
                            SAPDBMem_IRawAllocator  &alloc)
                           :
    m_NextReadPosition     (deviceBegin,  deviceBegin, deviceEnd), 
    m_PageReader           (pageReader),
    m_StandbySynchronizer  (standbySynchronizer),
    m_Pagebuffer           (alloc, UTF8( "Log_SeqLogDevReader::m_Pagebuffer ")),
    m_PageBufferIter       (m_Pagebuffer),
    m_NumPagesReturned     (0),
    m_PagesInBuffer        (0)
    {}

    /// initializer method (i.e. allocates memory for prefetching pages PTS 1126157)
    void Initialize()
    {
        SAPDBTRACE_METHOD_DEBUG ("Log_SeqLogDevReader::Initialize", LogVolume_Trace, 5);

        m_TotalBufferSize = IOMan_ILogInfo::GetInstance().GetInstance().GetLogIOBlockCount();
        if ( ! m_Pagebuffer.Initialize(m_TotalBufferSize))
        {
            RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_pagebuffer.Initialize()") );
        }

        Log_FrameAllocator::Instance().AllocateContinuousFrames(m_Pagebuffer, m_TotalBufferSize);
    }
    
    /// deallocates all dynamically allocated memory
    virtual void Delete()
    {
        SAPDBTRACE_METHOD_DEBUG ("Log_SeqLogDevReader::Delete", LogVolume_Trace, 5);
        Log_FrameAllocator & frameAllocator = Log_FrameAllocator::Instance();

        Log_FrameAllocator::Instance().FreeContinuousFrames(m_Pagebuffer, m_TotalBufferSize);
        m_Pagebuffer.Delete();
    }
    
    /// sets the actual page and IOSeq according to the given
    virtual ReaderStatus  SetPosition(   Log_RawDeviceOffset      offset)
    {
        SAPDBTRACE_METHOD_DEBUG ("Log_SeqLogDevReader::SetPosition", LogVolume_Trace, 5);
        m_NextReadPosition.SetPosition(offset);
        if (offset.IsValid())
        {
            return invalid_Offset;
        }
        else
        {
            return reader_ok;
        }
    }
    
    /// gets the actual offset of the last page read (invalid for a tape)
    virtual Log_RawDeviceOffset  GetPosition() const
    {
        return GetPositionIter().GetPosition();
    }

    /// gets the actual offset of the last page read (invalid for a tape)
    virtual Log_RawDeviceIterator  GetPositionIter() const
    {
        Log_RawDeviceIterator currentPosition = m_NextReadPosition;
        --currentPosition;
        return currentPosition;
    }

protected:
    
    /// reads the actual page and then increments the actual position
    virtual ReaderStatus  ReadNextPage(  tsp00_TaskId             taskId,
                                         boolean                 *RTECancelFlag,
                                         Log_Page                &logPage)
    {
        SAPDBTRACE_METHOD_DEBUG ("Log_SeqLogDevReader::ReadNextPage", LogVolume_Trace, 5);

        if (m_NextReadPosition.GetPosition().IsValid())
        {
            ReadLogPageBuffered (taskId, logPage);

            // For Hotstandby: suspend the ReaderTask if the last permitted page had been read
            m_StandbySynchronizer.CheckSuspendAfterReadLogPage(taskId, m_NextReadPosition.GetPosition(), 
                                                               logPage.ReadDate(), logPage.ReadTime());
            
            ++m_NextReadPosition;
            return reader_ok;
        }
        else
        {
            return invalid_Offset;
        }
    }
    
private: 
    /// reads a logpage  from the prefetch-buffer. If neccessary a prefetch is executed.
    /// (new for PTS 1126157)
    void ReadLogPageBuffered ( tsp00_TaskId   taskId,
                               Log_Page      &logPage)
    {
        /// page has not been read => prefetch as many pages as possible
        if (m_NumPagesReturned >= m_PagesInBuffer)
        {
            m_NumPagesReturned = 0;
            m_PageBufferIter.Begin();
            
            if (m_StandbySynchronizer.GetLastReadableOffset().IsValid())
            // the standby in a hotstandby configuration must stop reading before reaching the last
            // readable offset
            {
                m_PagesInBuffer = m_NextReadPosition.GetDistanceTo(m_StandbySynchronizer.GetLastReadableOffset()) + 1;
                if (m_PagesInBuffer > m_TotalBufferSize)
                {
                    m_PagesInBuffer = m_TotalBufferSize;
                }
            }
            else
            {
                m_PagesInBuffer = m_TotalBufferSize;
            }
            
            // do not read with wrap around, but make a break at the end of the LogVolume
            if (m_NextReadPosition.GetEndDistance() + 1 < m_PagesInBuffer)
            {
                m_PagesInBuffer = m_NextReadPosition.GetEndDistance() + 1;
            }

            Log_ClusterAddress clusterAdress(m_NextReadPosition.GetPosition(), m_PagesInBuffer);
            m_PageReader.ReadLogPages(taskId, m_Pagebuffer, clusterAdress);
        }

        // return the page
        SAPDBERR_ASSERT_STATE(!m_PageBufferIter.End());
        logPage.SetFrame((*m_PageBufferIter).GetFrame());
        ++m_NumPagesReturned;
        ++m_PageBufferIter;
    }

    ///  handle to read pages from log device.
    IOMan_ILogIO                  &m_PageReader;

    /// buffer for prefetching pages (new with PTS 1126157 mb 2004-01-07)
    IOMan_LogPages                 m_Pagebuffer;

    /// number of pages allocated in m_PageBuffer
    SAPDB_UInt                     m_TotalBufferSize;

    /// points to the next page to be returned
    IOMan_LogPagesIterator         m_PageBufferIter;

    ///  iterator over log page offsets, points to the next page to be read
    Log_RawDeviceIterator          m_NextReadPosition;
    
    /// number of pages actually read into m_PageBuffer
    SAPDB_UInt                     m_PagesInBuffer;
    
    /// number of pages actually returned of of m_PageBuffer
    SAPDB_UInt                     m_NumPagesReturned;
    
    /// The synchronization point for checking the offset, at which the reader has to be suspended
    Rst_StandbySynchronizer        &m_StandbySynchronizer;
};


#endif // LOG_SEQ_LOG_DEV_READER_HPP
