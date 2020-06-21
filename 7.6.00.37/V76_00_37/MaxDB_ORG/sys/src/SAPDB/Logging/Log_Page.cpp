/*!
  @file           Log_Page.cpp
  @author         UweH
  @author         MartinB
  @ingroup        Logging
  @brief          Handler for log pages
  @see            Kernel_IPage.html
                  Kernel_Page.html
                  Kernel_Page74.html

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

#include "Logging/Log_Page.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Log.h"

//------------------------------------------------------------------------------

void Log_Page::AppendContextInfo (SAPDBErr_MessageList      &errlist,
                                  SAPDB_Char  const * const  title) const
{
    errlist.AddDetail( SDBMSG_LOG_PAGE_INFO1,
                       Msg_Arg(SDBMSGTAG_LOG_PAGE_INFO1__TITLE,            title!=0?title:"LogPage"),
                       Msg_Arg(SDBMSGTAG_LOG_PAGE_INFO1__FIRST_IOSEQUENCE, SAPDB_ToString(this->ReadFirstWriterIOSequenceNo())),
                       Msg_Arg(SDBMSGTAG_LOG_PAGE_INFO1__LAST_IOSEQUENCE,  SAPDB_ToString(this->ReadLastWriterIOSequenceNo())),
                       Msg_Arg(SDBMSGTAG_LOG_PAGE_INFO1__LOG_QUEUE_COUNT,  SAPDB_ToString(this->ReadQueueID())),
                       Msg_Arg(SDBMSGTAG_LOG_PAGE_INFO1__LOG_IO_SEQUENCE,  SAPDB_ToString(this->ReadQueueIOSequenceNo())) );
}

//------------------------------------------------------------------------------

Log_Page::~Log_Page()
{
    if ( ! IsAssigned() || m_Frame.GetLength() == 0 )
        return;
        
    if ( LogPage_Check.ChecksLevel(5) )
    {
        SAPDBErr_MessageList errlist;
        if ( ! Verify(false,errlist) )
        {
            WriteToTrace();

            if ( LogPage_Check.ChecksLevel(6) )
                RTE_Crash(errlist);

            RTE_Message(errlist);
            errlist.ClearMessageList();
        }
    }
}

//------------------------------------------------------------------------------

SAPDB_Bool Log_Page::Verify(bool                  isAdmin,
                            SAPDBErr_MessageList &errlist) const
{
    if ( ! IsAssigned() )
        return true;
        
    if ( ! Kernel_Page74::Verify(isAdmin,errlist) )
        return false;
    
    if ( ReadPageType() != Kernel_LogPageType )
    {
        errlist.AppendNewMessage( Msg_List( Msg_List::Error, SDBMSG_LOG_PAGE_TYPE_INCORRECT,
                                            Msg_Arg(SDBMSGTAG_LOG_PAGE_TYPE_INCORRECT__TYPE, SAPDB_ToString(Kernel_LogPageType)),
                                            Msg_Arg(SDBMSGTAG_LOG_PAGE_TYPE_INCORRECT__UINT4,SAPDB_ToString(ReadPageType())) ) );
        AppendContextInfo(errlist,"Verify1");
        return false;
    }

    // Check length and alignment of log entries

    const OffsetType   firstfreeoffset = MinSpaceOffset();
    SAPDB_UInt4        entrylength;
    SAPDB_UInt         entryNo         = 1;

    for ( OffsetType offset = sizeof(PersistentHeader);
                     offset < firstfreeoffset;
                     offset += CalcNearestAlignedSize(entrylength) , ++entryNo ) // PTS 1139685 mb 2005-01-12
    {
        entrylength = *(reinterpret_cast<const SAPDB_UInt4*>(m_Frame.GetPointer(offset,sizeof(SAPDB_UInt4))));

        if ( offset + entrylength  > firstfreeoffset )
        {
            errlist.AppendNewMessage( Msg_List( Msg_List::Error, SDBMSG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED,
                                                Msg_Arg(SDBMSGTAG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED__ENTRY_COUNT, SAPDB_ToString(entryNo)),
                                                Msg_Arg(SDBMSGTAG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED__NUMBER1,     SAPDB_ToString(offset + entrylength)),
                                                Msg_Arg(SDBMSGTAG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED__NUMBER2,     SAPDB_ToString(firstfreeoffset)) ) );
            AppendContextInfo(errlist,"Verify2");
            return false;
        }
    }
    
    return true;
}

//------------------------------------------------------------------------------

void Log_Page::WriteToTrace() const
{
    Kernel_VTrace trace;
    
    trace << "------------- Log_Page::WriteToTrace() begin -------------------" << NewLine;
    trace << "LastWriteIOSequence: " << ReadLastWriterIOSequenceNo() << NewLine;
    if ( ReadPageType() == Kernel_LogPageType )
        trace << "PageType:         LogPage" << NewLine;
    else
        trace << "PageType:         invalid (" << ReadPageType() << ")" << NewLine;
    trace << "ParityAlgo:       " << ReadParityAlgorithm() << NewLine;
    if ( ReadParityFromHeader() == ReadParityFromTrailer() )
        trace << "ParityBytes:      ok" << NewLine;
    else
        trace << "ParityBytes:      " << ReadParityFromHeader() << "/" << ReadParityFromTrailer() << NewLine;
    // --------------
    trace << "Timestamp:        " << ReadDate() << "/" << ReadTime() << NewLine;
    trace << "QueueInfo:        " << ReadQueueID() << "." << ReadQueueIOSequenceNo() << NewLine;
    trace << "OffsetCount:      " << ReadOffsetCount()
          << " (FirstWriteIOSequence: " << ReadFirstWriterIOSequenceNo() << ")" << NewLine;
    if (FirstEntryIncomplete())
        trace << "First LogEntry is a continuation from a previous logpage" << NewLine;
    else
        trace << "First LogEntry is complete on this page" << NewLine;
    
    trace << "FirstFreeOffset:  " << ReadFirstFreeOffset() << NewLine;

    OffsetType offset;
    
    trace << "OffsetList:";
    for (offset = LOG_PAGE_FIRST_OFFSET; offset <= ReadOffsetCount(); ++offset)
        trace << "offset (" << offset << "): " << ReadOffsetOnPage(offset) << NewLine;

    if ( LogPage_Trace.TracesLevel (8) )
    {
        const SAPDB_UInt4* entrylength;
        const OffsetType   firstfreeoffset = ReadFirstFreeOffset();
        SAPDB_UInt         entryno         = 0;
    
        for ( offset = MinSpaceOffset();
              offset < firstfreeoffset;
              offset += CalcNearestAlignedSize(*entrylength) ) // PTS 1139685 mb 2005-01-12
        {
            // if offset is invalid the next call will crash
            entrylength = reinterpret_cast<const SAPDB_UInt4*>(m_Frame.GetPointer(offset,sizeof(SAPDB_UInt4)));
            if ( offset + *entrylength <= firstfreeoffset )
                trace << "#" << entryno << " @ " << offset << ", " << *entrylength << NewLine;
            else
                trace << "#" << entryno << " @ " << offset << ", " << *entrylength << "(" << firstfreeoffset-offset << ")" << NewLine;
            ++entryno;
        }
    }
    trace << "------------- Log_Page::WriteToTrace() end -------------------\n";
}

//------------------------------------------------------------------------------
void Log_Page::Truncate(Log_IOSequenceNo newLastIOSeq)
{
    while (!IsEmpty() && (ReadLastWriterIOSequenceNo() != newLastIOSeq))
    {
        if ( ReadOffsetCount() > 1 )
        {
            PersistentHeaderPtr()->m_OffsetCount -= 1;
            WriteWriterIOSequenceNo(ReadLastWriterIOSequenceNo() - 1 );
        }
        else
        {
            ResetOffsetCount();
        }
    }
    WriteQueueIOSequenceNo( 0 );
    UpdateParity();
}


