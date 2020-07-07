/*!
  @file           Data_SplitSpaceReader.cpp
  @author         UweH
  @brief          methodes for Data_SplitSpaceReader

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
#include "DataAccess/Data_SplitSpaceReader.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

/* --------------------------------------------------------------------------- */
void Data_SplitSpaceReader::TakeNextPart()
{
    ++m_CurrentPart;
    
    if ( m_CurrentPart > m_Space.CurrentPart() )
        return;

    m_Space.GetPart (m_CurrentPart, m_CurrentLength, m_CurrentValue);

    m_CurrentPartRestLength = m_CurrentLength;
}

/* --------------------------------------------------------------------------- */
Data_SplitSpaceReader::Result
Data_SplitSpaceReader::Reserve (SAPDB_UInt   Length,
                                SAPDB_Byte* &Space)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_SplitSpaceReader::Reserve", DataChain_Trace, 5);
    SAPDBTRACE_WRITELN (DataChain_Trace, 6, "Length: " << Length);

    if ( Length > m_CurrentPartRestLength )
        return spaceToSmall;

    Space = m_CurrentValue + (m_CurrentLength - m_CurrentPartRestLength);

    m_CurrentPartRestLength -= Length;

    if ( m_CurrentPartRestLength <= 0 )
        TakeNextPart ();

    if ( m_CurrentPartRestLength > 0 )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "more space available: " << m_CurrentPartRestLength);
        return moreSpaceAvailable;
    }
    return ok;
}

/* --------------------------------------------------------------------------- */
Data_SplitSpaceReader::Result
Data_SplitSpaceReader::Read (SAPDB_UInt              TargetPartLength,
                             SAPDB_Byte*            &TargetPart,
                             SAPDBMem_IRawAllocator &Allocator,
                             bool                   &bAllocatorWasUsed)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_SplitSpaceReader::Read1", DataChain_Trace, 5);
    SAPDBTRACE_WRITELN (DataChain_Trace, 6, "length: " << TargetPartLength);

    bAllocatorWasUsed = false;
    
    if ( ! m_SpaceCanBeReleased ) // PTS 1114994 UH 2002-04-17
    {
        Result result = Reserve (TargetPartLength, TargetPart);
        if ( spaceToSmall != result )
            return result;
    }

    TargetPart = reinterpret_cast <SAPDB_Byte*> (Allocator.Allocate(TargetPartLength));
    if ( NULL == TargetPart )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "space too small: targetpart is nil");
        return spaceToSmall;
    }
    
    bAllocatorWasUsed = true;
    
    SAPDB_UInt CurrentCopyLength     = 0;
    SAPDB_UInt CurrentRestCopyLength = TargetPartLength;

    if ( m_CurrentPart > m_Space.CurrentPart() )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "no more to read1: " << m_Space.CurrentPart());
        return noMoreToRead;
    }

    do
    {
        if ( m_CurrentPartRestLength <= 0 )
        {
            TakeNextPart ();
            if ( m_CurrentPartRestLength <= 0 )
            {
                SAPDBTRACE_WRITELN (DataChain_Trace, 6, "no more to read2: " << m_CurrentPartRestLength);
                return noMoreToRead;
            }
        }

        CurrentCopyLength = CurrentRestCopyLength;

        if ( CurrentCopyLength > m_CurrentPartRestLength )
            CurrentCopyLength = m_CurrentPartRestLength;

        SAPDBErr_MessageList errlist;
        if ( ! SAPDB_MemCopy( TargetPart     + (TargetPartLength - CurrentRestCopyLength  ),
                              m_CurrentValue + (m_CurrentLength  - m_CurrentPartRestLength),
                              CurrentCopyLength,
                              "DataAccess", __CONTEXT__, errlist ) )
            RTE_Crash(errlist);

        CurrentRestCopyLength   -= CurrentCopyLength;
        m_CurrentPartRestLength -= CurrentCopyLength;
    }
    while ( CurrentRestCopyLength > 0 );

    if ( m_CurrentPartRestLength <= 0 )
        TakeNextPart ();

    if ( m_CurrentPartRestLength >  0 )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "more space available: " << m_CurrentPartRestLength);
        return moreSpaceAvailable;
    }
    return ok;
}

/* --------------------------------------------------------------------------- */
Data_SplitSpaceReader::Result
Data_SplitSpaceReader::Read (SAPDB_Byte* TargetPart,
                             SAPDB_UInt  TargetPartLength)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_SplitSpaceReader::Read2", DataChain_Trace, 5);
    SAPDBTRACE_WRITELN (DataChain_Trace, 6, "length: " << TargetPartLength);

    SAPDB_UInt CurrentCopyLength     = 0;
    SAPDB_UInt CurrentRestCopyLength = TargetPartLength;

    if ( m_CurrentPart > m_Space.CurrentPart() )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "no more to read1: " << m_Space.CurrentPart());
        return noMoreToRead;
    }

    do
    {
        if ( m_CurrentPartRestLength <= 0 )
            TakeNextPart ();

        if ( m_CurrentPartRestLength <= 0 )
        {
            SAPDBTRACE_WRITELN (DataChain_Trace, 6, "no more to read2: " << m_CurrentPartRestLength);
            return noMoreToRead;
        }

        CurrentCopyLength = CurrentRestCopyLength;

        if ( CurrentCopyLength > m_CurrentPartRestLength )
            CurrentCopyLength = m_CurrentPartRestLength;

        SAPDBErr_MessageList errlist;
        if ( ! SAPDB_MemCopy( TargetPart     + (TargetPartLength - CurrentRestCopyLength  ),
                              m_CurrentValue + (m_CurrentLength  - m_CurrentPartRestLength),
                              CurrentCopyLength,
                              "DataAccess", __CONTEXT__, errlist ) )
            RTE_Crash(errlist);

        CurrentRestCopyLength   -= CurrentCopyLength;
        m_CurrentPartRestLength -= CurrentCopyLength;
    }
    while ( CurrentRestCopyLength > 0 );

    if ( m_CurrentPartRestLength <= 0 )
        TakeNextPart ();

    if ( m_CurrentPartRestLength > 0 )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "more space available: " << m_CurrentPartRestLength);
        return moreSpaceAvailable;
    }
	return ok;
}

/* --------------------------------------------------------------------------- */
void Data_SplitSpaceReader::WriteToTrace (const char* title) const
{
    Kernel_VTrace trace;
    if ( title != NULL )
        trace << title << NewLine;

    trace << "part: "          << m_CurrentPart
          << ", length: "      << m_CurrentLength
          << ", restlength: "  << m_CurrentPartRestLength
          << (m_SpaceCanBeReleased?"releasable":"not releasable") // PTS 1114994 UH 2002-04-17
          << FlushLine;

    if ( ! m_Space.IsAssigned() )
    {
        trace << "not assigned." << FlushLine;
        return;
    }

    trace << "space: length: " << m_Space.Length()
          << FlushLine;
    PartNo      part;
    SAPDB_UInt  length;
    SAPDB_Byte* value;
    if ( m_Space.CurrentPart().IsValid() )
    {
        for (part = 0; part <= m_Space.CurrentPart(); ++part)
        {
            m_Space.GetPart (part, length, value);
            trace << "spacepart: " << part
                  << ", length: "  << length
                  << FlushLine;
        }
    }
}


/* --------------------------------------------------------------------------- */
void Data_SplitSpaceReader::AppendContextInfo (SAPDB_Char  const * const  filename,
                                               SAPDB_UInt4 const          linenumber,
                                               SAPDBErr_MessageList      &errlist,
                                               SAPDB_Char  const * const  title) const
{
    errlist = errlist +
        Data_Exception  (filename,linenumber, DATA_SPLIT_SPACE_READER_INFO,
                         (title==0?"SplitSpaceReader":title),
                         SAPDB_ToString(m_CurrentPart),
                         SAPDB_ToString(m_CurrentLength),
                         SAPDB_ToString(m_CurrentPartRestLength),
                         (m_SpaceCanBeReleased?"releasable":"not releasable"));
    m_Space.AppendContextInfo(filename,linenumber,errlist,title);
}
