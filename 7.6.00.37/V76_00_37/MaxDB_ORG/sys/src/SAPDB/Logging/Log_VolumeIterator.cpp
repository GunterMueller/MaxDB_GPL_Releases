/*!
  @file           Log_VolumeIterator.cpp
  @author         UweH
  @author         MartinB
  @ingroup        Archive log
  @brief          This iterator gives access to parts of log entries.
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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "Logging/Log_VolumeIterator.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Restart/Rst_StandbySynchronizer.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

// -----------------------------------------------------------------------------
Log_VolumeIterator::~Log_VolumeIterator ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VolumeIterator::~Log_VolumeIterator", LogVolume_Trace, 5);
}

// -----------------------------------------------------------------------------
void Log_VolumeIterator::Initialize()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VolumeIterator::Initialize", LogVolume_Trace, 5);
    m_State = Iter_ok;
}

// -----------------------------------------------------------------------------
void Log_VolumeIterator::Delete()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VolumeIterator::Delete", LogVolume_Trace, 5);
    m_Page.SetFrame (Log_Page::PageFrame());
    m_PageReader->Delete();
    destroy(m_PageReader,m_Allocator);
    m_State = Iter_unInitialized;
}

// -----------------------------------------------------------------------------
void Log_VolumeIterator::Invalidate(const IterState state)
{
    m_EntryIter.Invalidate();
    m_State = state;
}

// -----------------------------------------------------------------------------
SAPDB_Byte* Log_VolumeIterator::operator * ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VolumeIterator::operator *", LogVolume_Trace, 5);
    SAPDBERR_ASSERT_STATE( GetState() == Iter_ok );
    return *m_EntryIter;
}

// -----------------------------------------------------------------------------
Log_VolumeIterator& Log_VolumeIterator::operator ++ ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VolumeIterator::operator ++", LogVolume_Trace, 5);

    Log_IOSequenceNo prevIOSequence = m_EntryIter.GetIOSequence();
    ++m_EntryIter;

    if ( m_EntryIter.IsValid() )
    // the next entry is still on the current page
    {
        if (CheckUntilIOSequenceReached())
        {
            RTE_Message( Log_Exception(
                         __CONTEXT__,
                         LOG_UNTIL_IOSEQ_REACHED,
                         SAPDB_ToString(prevIOSequence),
                         SAPDB_ToString(m_PageReader->GetPosition()),
                         SAPDB_ToString(m_Page.ReadDate()),
                         SAPDB_ToString(m_Page.ReadTime())));
        }
        return *this;
    }

    if (UntilDateIsReached(m_Page.ReadDate(),m_Page.ReadTime()))
    {
        return *this;
    }

    // ------------------------------------------------------------------------
    // A   N E W   P A G E H A S   T O   B E   R E A D
    // ------------------------------------------------------------------------
    
    // store some page-information for a later knldiag-message
    const Kernel_Date           lastPageDate        = m_Page.ReadDate();
    const Kernel_Time           lastPageTime        = m_Page.ReadTime();
    const Log_RawDeviceOffset   lastReadOffset      = m_PageReader->GetPosition();
    const Log_IOSequenceNo      lastReadIOSequence  = m_Page.ReadLastWriterIOSequenceNo();
    const Log_IOSequenceNo      prevQueueSequence   = m_Page.ReadQueueIOSequenceNo();

    // check if all of the entries are read PTS 1117798 mb 2002-09-11
    if (m_LastKnownIOSequence.IsValid())
    {
        if (prevIOSequence == m_LastKnownIOSequence)
        {
            RTE_Message(Log_Exception( __CONTEXT__, LOG_NORMAL_END_FOUND,
                                       SAPDB_ToString(lastReadOffset),
                                       SAPDB_ToString(lastReadIOSequence) ));
            Invalidate(Iter_allRead);
            return *this;
        }
    }
    
    // A new page has to be read
    Log_ISequentialReader::ReaderStatus setResult = m_PageReader->SetIOSeq(prevIOSequence+(SAPDB_UInt4)1);
    Log_ISequentialReader::ReaderStatus readResult = m_PageReader->ReadNextPage(m_TaskId, m_RTECancelFlag, m_Page);
    if (Log_ISequentialReader::end_of_file == readResult)
    {
        RTE_Message( Log_Exception( __CONTEXT__, LOG_NORMAL_END_FOUND,
                                    SAPDB_ToString(lastReadOffset),
                                    SAPDB_ToString(lastReadIOSequence) ));
        Invalidate(Iter_allRead);
        return *this;
    }
    else if (Log_ISequentialReader::reader_ok != readResult)
    {
        RTE_Crash( Log_Exception( __CONTEXT__, LOG_READ_UNEXPECTED_ERROR,
                                  SAPDB_ToString(m_LastKnownIOSequence),
                                  SAPDB_ToString(prevIOSequence) ));
    }
    
    m_Page.Begin (m_EntryIter);

    Msg_List errlist;
    
    switch ( CheckLogDeviceConsistency ( m_Page,
                                         m_PageReader->GetPosition(),
                                         prevIOSequence,
                                         prevQueueSequence,
                                         errlist ) )
    {
        case inconsistent:
        {
            if (Kernel_IAdminHotStandby::Instance().RoleIsStandby())
                errlist.Overrule(Log_Exception( __CONTEXT__, LOG_STANDBY_TOO_SLOW ));
            else
                errlist.Overrule(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                           "Log_VolumeIterator::operator++() inconsistent log device"));
            Kernel_IAdminRestartShutdown::Instance().Offline(errlist);
            break;
        }
        case endOfLogDevice:
        {
            if ( m_LastKnownIOSequence.IsValid()
                 &&
                 m_LastKnownIOSequence != prevIOSequence )
            {
                errlist.Overrule( Log_Exception(__CONTEXT__, LOG_READ_LAST_KNOWN_NOT_FOUND,
                           SAPDB_ToString(m_PageReader->GetPosition()),
                           SAPDB_ToString(m_LastKnownIOSequence),
                           SAPDB_ToString(prevIOSequence) ) );
                Kernel_IAdminRestartShutdown::Instance().Offline(errlist);
            }
    
            RTE_Message( Log_Exception( __CONTEXT__, LOG_NORMAL_END_FOUND, SAPDB_ToString(lastReadOffset),
                                                                           SAPDB_ToString(lastReadIOSequence) ));
            Invalidate(Iter_allRead);
            break;
        }
        default: // consistent
            if (CheckUntilIOSequenceReached())
            {
                 RTE_Message( Log_Exception( __CONTEXT__, LOG_UNTIL_IOSEQ_REACHED,
                                             SAPDB_ToString(lastReadIOSequence ),
                                             SAPDB_ToString(lastReadOffset),
                                             SAPDB_ToString(lastPageDate),
                                             SAPDB_ToString(lastPageTime)));
            }
            break;
    }

    return *this;
}

// -----------------------------------------------------------------------------
void Log_VolumeIterator::SetPosition (Log_RawDeviceOffset pageoffset,
                                      OffsetType          entryoffset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VolumeIterator::SetPosition", LogVolume_Trace, 5);

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "pageoffset: "<<pageoffset<<", entryoffset: "<<entryoffset);

    m_EntryIter.SetPosition  (entryoffset);
    m_PageReader->SetPosition(pageoffset);
    m_PageReader->ReadNextPage(m_TaskId, m_RTECancelFlag, m_Page);

    const Kernel_Date           lastPageDate        = m_Page.ReadDate();
    const Kernel_Time           lastPageTime        = m_Page.ReadTime();
    const Log_RawDeviceOffset   lastReadOffset      = m_PageReader->GetPosition();
    const Log_IOSequenceNo      lastReadIOSequence  = m_Page.ReadFirstWriterIOSequenceNo();

    if (CheckUntilIOSequenceReached())
    {
         RTE_Message( Log_Exception(
                     __CONTEXT__,
                     LOG_UNTIL_IOSEQ_REACHED,
                     SAPDB_ToString(lastReadIOSequence ),
                     SAPDB_ToString(lastReadOffset),
                     SAPDB_ToString(lastPageDate),
                     SAPDB_ToString(lastPageTime)));
    }
}
// -----------------------------------------------------------------------------
void Log_VolumeIterator::SetPosition (Log_RawDeviceOffset pageOffset)
{
    m_Page.Begin(m_EntryIter);
    SetPosition(pageOffset,m_EntryIter.GetPosition());
}
// -----------------------------------------------------------------------------
void Log_VolumeIterator::WriteToTrace (const char* title)
{
    Kernel_VTrace vtrace;
    switch (GetState())
    {
        case Iter_untilReached:
            vtrace << title << ": Iter_untilReached";
            break;
        case Iter_allRead:
            vtrace << title << ": Iter_allRead";
            break;
        case Iter_unInitialized:
            vtrace << title << ": Iter_unInitialized";
            break;
        case Iter_ok:
            vtrace << title
                   << ": "  << m_PageReader->GetPosition()
                   << "."   << m_EntryIter.GetPosition()
                   << " ["  << m_EntryIter.GetIOSequence()
                   << "/"   << m_EntryIter.GetQueueId()
                   << "] "  << m_EntryIter.Length()
                   << "("  << m_EntryIter.RestLength()
                   << ")";
            break;
        default:
            vtrace << title << ": unexpected state";
    }
}

// -----------------------------------------------------------------------------
Log_VolumeIterator::CheckLogDeviceResult
Log_VolumeIterator::CheckLogDeviceConsistency
    (const Log_Page                &page,
     const Log_RawDeviceOffset      position,
     Log_IOSequenceNo               prevIOSequence,
     Log_IOSequenceNo               prevQueueSequence,
     Msg_List                      &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_VolumeIterator::CheckLogDeviceConsistency", LogVolume_Trace, 5);

    Log_IOSequenceNo  expectedFirstIOSeq(prevIOSequence); //PTS 1124727 mb 2003-10-16
    ++expectedFirstIOSeq;                                 //takes care of overflow
    
    if ( page.ReadLastWriterIOSequenceNo().IsInvalid()
         ||
         ( page.ReadFirstWriterIOSequenceNo().IsSmallerWithoutOverflowCheck(prevIOSequence) //PTS 1124727 mb 2003-10-16
           &&
           (expectedFirstIOSeq != MIN_IOSEQUENCE) /*iosequence was reset*/) ) // PTS 1120201 UH 2003-02-17
	{
        if ( LogVolume_Trace.TracesLevel(6) )
        {
            page.WriteToTrace();
            Kernel_VTrace() << "CheckLogDeviceConsistency1: prevIOSeq: " << prevIOSequence;
        }
        return endOfLogDevice;
	}

    if ( page.ReadFirstWriterIOSequenceNo() != expectedFirstIOSeq )
    {
        if ( LogVolume_Trace.TracesLevel(6) )
            page.WriteToTrace();
        errlist.AppendNewMessage( Log_Exception(__CONTEXT__, LOG_READ_IOSEQ_INCONSISTENT,
                                                SAPDB_ToString(position),
                                                SAPDB_ToString(expectedFirstIOSeq),
                                                SAPDB_ToString(page.ReadFirstWriterIOSequenceNo()) ) );
        return inconsistent;
    }
    else
    {
        return consistent;
    }
}

// -----------------------------------------------------------------------------

bool Log_VolumeIterator::UntilDateIsReached(Kernel_Date readDate, Kernel_Time readTime)
{
    if (   m_UntilDate.IsValid() 
        && m_UntilTime.IsValid()
        && readDate.IsValid() 
        && readTime.IsValid())
    {
        if (! Kernel_DateTime::OlderThan(readDate,readTime,
                                         m_UntilDate, m_UntilTime))
        {
            if (m_EntryIter.IsValid())
            {
                RTE_Message(Log_Exception(  __CONTEXT__,
                                            LOG_UNTIL_DATE_REACHED,
                                            SAPDB_ToString(readDate),
                                            SAPDB_ToString(readTime),
                                            SAPDB_ToString(m_EntryIter.GetPosition()),
                                            SAPDB_ToString(m_EntryIter.GetIOSequence()) ));
            }
            else
            {
                RTE_Message(Log_Exception(  __CONTEXT__,
                                            LOG_UNTIL_DATE_REACHED,
                                            SAPDB_ToString(readDate),
                                            SAPDB_ToString(readTime),
                                            SAPDB_ToString(m_PageReader->GetPosition()),
                                            SAPDB_ToString(m_Page.ReadLastWriterIOSequenceNo()) ));
            }

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

