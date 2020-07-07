/*!
  @file           Data_SplitSpaceWriter.hpp
  @author         UweH
  @brief          methodes for Data_SplitSpaceWriter

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
#include "DataAccess/Data_SplitSpaceWriter.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "DataAccess/Data_Types.hpp"

/* --------------------------------------------------------------------------- */
void Data_SplitSpaceWriter::TakeNextPart()
{
    if ( m_pSplitSpace == 0 )
        return;
        
    ++m_CurrentPart;

    if ( m_CurrentPart > m_pSplitSpace->CurrentPart() )
        return;

    m_pSplitSpace->GetPart (m_CurrentPart, m_CurrentLength, m_CurrentValue);

    m_CurrentPartRestLength = m_CurrentLength;
}

/* --------------------------------------------------------------------------- */
Data_SplitSpaceWriter::Result
Data_SplitSpaceWriter::Reserve (SAPDB_UInt   Length,
                                SAPDB_Byte* &Space)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_SplitSpaceWriter::Reserve", DataChain_Trace, 5);
    SAPDBTRACE_WRITELN (DataChain_Trace, 6, "length: " << Length);
    
    Space     = 0;
    
    if ( Length > m_CurrentPartRestLength )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "space too small: " << m_CurrentPartRestLength);
        return spaceToSmall;
    }

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
Data_SplitSpaceWriter::Result
Data_SplitSpaceWriter::Write (const SAPDB_Byte     *SourcePart,
                              const SAPDB_UInt      SourcePartLength)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_SplitSpaceWriter::Write", DataChain_Trace, 5);
    SAPDBTRACE_WRITELN (DataChain_Trace, 6, "SourcePartLength: " << SourcePartLength);

    SAPDB_UInt CurrentCopyLength     = 0;
    SAPDB_UInt CurrentRestCopyLength = SourcePartLength;

    if( CurrentRestCopyLength <= 0 )
    {
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "no more to write: " << CurrentRestCopyLength);
        return noMoreToWrite;
    }

    do
    {
        if ( m_CurrentPartRestLength <= 0 )
            TakeNextPart ();

        if ( m_CurrentPartRestLength <= 0 )
        {
            SAPDBTRACE_WRITELN (DataChain_Trace, 6, "space too small: " << m_CurrentPartRestLength);
            return spaceToSmall;
        }

        CurrentCopyLength = CurrentRestCopyLength;

        if ( CurrentCopyLength > m_CurrentPartRestLength )
            CurrentCopyLength = m_CurrentPartRestLength;

        SAPDBErr_MessageList errlist;
        if ( NULL != SourcePart )
        {
            if ( ! SAPDB_MemCopy( m_CurrentValue + (m_CurrentLength  - m_CurrentPartRestLength),
                                  SourcePart     + (SourcePartLength - CurrentRestCopyLength  ),
                                  CurrentCopyLength,
                                  "DataAccess", __CONTEXT__, errlist ) )
                RTE_Crash(errlist);
        }

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
void Data_SplitSpaceWriter::WriteToTrace (const char* title) const
{
    Kernel_VTrace trace;

    if ( title != NULL )
        trace << title << NewLine;

    trace << "writer: part: "  << m_CurrentPart
          << ", length: "      << m_CurrentLength
          << ", restlength: "  << m_CurrentPartRestLength
          << FlushLine;

    if ( ! IsAssigned() )
    {
        trace << "not assigned." << FlushLine;
        return;
    }
        
    trace << "space: length: " << m_pSplitSpace->Length()
          << FlushLine;
    PartNo      part;
    SAPDB_UInt  length;
    SAPDB_Byte* value;
    if ( m_pSplitSpace->CurrentPart().IsValid() )
    {
        for (part = 0; part <= m_pSplitSpace->CurrentPart(); ++part)
        {
            m_pSplitSpace->GetPart (part, length, value);
            trace << "spacepart: " << part
                  << ", length: "  << length
                  << FlushLine;
        }
    }
}

/* --------------------------------------------------------------------------- */
void Data_SplitSpaceWriter::AppendContextInfo (SAPDB_Char  const * const  filename,
                                               SAPDB_UInt4 const          linenumber,
                                               SAPDBErr_MessageList      &errlist,
                                               SAPDB_Char  const * const  title) const
{
    errlist = errlist +
        Data_Exception  (filename,linenumber, DATA_SPLIT_SPACE_READER_INFO,
                         (title==0?"SplitSpaceWriter":title),
                         SAPDB_ToString(m_CurrentPart),
                         SAPDB_ToString(m_CurrentLength),
                         SAPDB_ToString(m_CurrentPartRestLength),
                         "");
    if ( m_pSplitSpace != 0 )
        m_pSplitSpace->AppendContextInfo(filename,linenumber,errlist,title);
}
