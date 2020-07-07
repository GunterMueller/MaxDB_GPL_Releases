/*!
  @file           Kernel_Trace.cpp
  @author         UweH
  @ingroup        tracing
  @brief          basic utilities

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
#include "KernelCommon/Kernel_Trace.hpp"

#include "gsp00.h"
#include "ggg00.h"
#include "hgg17.h" // g17basis_err_to_line

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDBCommon/Messages/SDBMsg_KernelCommon.h"

/*----------------------------------------------------------------------------------------------------*/
Kernel_BasisErrorTracer::Kernel_BasisErrorTracer ( tgg00_BasisError &berror,
                                                   SAPDBTrace_Topic &tracetopic,
                                                   SAPDB_UInt        tracelevel,
                                                   char             *title )
: m_error     (berror    ),
  m_tracetopic(tracetopic),
  m_tracelevel(tracelevel),
  m_title     (title     )
{}

/*----------------------------------------------------------------------------------------------------*/
Kernel_BasisErrorTracer::~Kernel_BasisErrorTracer()
{
    if ( e_ok != m_error
         &&
         m_tracetopic.TracesLevel(m_tracelevel) )
    {
        char errorString [sizeof(tsp00_Line)];
        int  stringLength = 0;
        g17basis_err_to_line (m_error, stringLength, errorString);
        errorString[stringLength] = 0;

        if ( m_title != NULL )
            Kernel_VTrace() << m_title << " error: " << errorString;
        else
            Kernel_VTrace() << "error: " << errorString;
    }
}

/*----------------------------------------------------------------------------------------------------*/
Kernel_IsOkTracer::Kernel_IsOkTracer ( bool             &isok,
                                       SAPDBTrace_Topic &tracetopic,
                                       SAPDB_UInt        tracelevel,
                                       char             *title )
: m_isok      (isok      ),
  m_tracetopic(tracetopic),
  m_tracelevel(tracelevel),
  m_title     (title     )
{}

/*----------------------------------------------------------------------------------------------------*/
Kernel_IsOkTracer::~Kernel_IsOkTracer()
{
    if ( ! m_isok
         &&
         m_tracetopic.TracesLevel(m_tracelevel) )
    {
        if ( m_title != NULL )
            Kernel_VTrace() << m_title << " error: NOT OK";
        else
            Kernel_VTrace() << "error: NOT OK";
    }
}

/*----------------------------------------------------------------------------------------------------*/
SAPDB_UInt4 Kernel_TraceMaxBufferLength = SAPDB_MAX_UINT4;

/*----------------------------------------------------------------------------------------------------*/
externCpp void Kernel_TraceBuffer ( const void       *buffer,
                                    SAPDB_UInt        length,
                                    const SAPDB_Char *title,
                                    SAPDB_UInt        size )
{
    // generated output:
    //-----------------------------------------------------
    // Buffer: length: 64, size: 40 (Buffer overflow)
    // Offset: 0
    // off: 0   1   2   ...
    // dec: 255 255 255 ...
    // hex: FF  FF  FF  ...
    // chr: X   X   X   ...
    // Offset: 1888
    // off: 888 889 890
    // dec: 255 255 255
    // hex: FF  FF  FF
    // chr: X   X   X
    //-----------------------------------------------------
    const SAPDB_UInt     bytesPerByte    = 4;
    const SAPDB_UInt     maxBytesPerLine = 20;
          SAPDB_UInt     currentOffset   = 0;
          SAPDB_UInt     offsetInLine;
          SAPDB_Char     traceBuffer [bytesPerByte+1];
          SAPDB_UInt     output;
          Kernel_VTrace  trace;
    
    trace << ( (title != NULL) ? title : "Buffer" )
          << ": length: " << length;
    if ( size > 0 )
        trace << ", size: " << size;

    if ( Kernel_TraceMaxBufferLength < SAPDB_MAX_UINT4 )
        trace << ", buflimit: " << Kernel_TraceMaxBufferLength;
        
    SAPDB_UInt usedLength = length;
    
    if ( usedLength > Kernel_TraceMaxBufferLength )
        usedLength = Kernel_TraceMaxBufferLength;

    if ( size > 0 && usedLength > size )
    {
        trace << " (Buffer overflow)";
        usedLength = size;
    }
    trace << FlushLine;

    typedef enum  { offset=0, decimal=1, hexadecimal=2, character=3 } OutputType;
    
          OutputType  outputType;
    const SAPDB_Char* prefixes[] = {"off: ",  "dec: ", "hex: ", "chr: "};
    const SAPDB_Char* formats [] = {"%02d   ","%02d  ","%02X  ","%c   "};
    
    while ( currentOffset < usedLength )
    {
        trace << "Offset: " << currentOffset << FlushLine;
        for (outputType = offset; outputType <= character; outputType=(OutputType)(outputType+1))
        {
            trace << prefixes[outputType];
            for ( offsetInLine = 0;
                  offsetInLine < maxBytesPerLine
                  &&
                  currentOffset+offsetInLine < usedLength;
                  ++offsetInLine )
            {
                switch (outputType)
                {
                    case offset:
                        output = currentOffset + offsetInLine;
                        break;
                    case decimal:
                    case hexadecimal:
                        output = *((SAPDB_Byte*)buffer + currentOffset + offsetInLine);
                        break;
                    case character:
                        output = *((SAPDB_Byte*)buffer + currentOffset + offsetInLine);
                        if ( character == outputType
                             &&
                             (output > 128 || output < 32) )
                            output = ' ';
                        break;
                }
                SAPDB_sprintf ( traceBuffer, sizeof(traceBuffer),
                                formats[outputType], output );
                trace << traceBuffer;
            }
            trace << FlushLine;
        }
        trace << FlushLine;
        currentOffset += offsetInLine;
    }
}

/* --------------------------------------------------------------------------- */
externCpp SAPDB_Int2 Kernel_BasisErrorToResultCode (const tgg00_BasisError berr)
{
    return SQLManErr_Interface::GetInstance().GetReturnCode(berr,sqlm_internal);
}

/* --------------------------------------------------------------------------- */
externCpp const SAPDB_Char *  Kernel_BasisErrorToResultString (const tgg00_BasisError berr)
{
    return SQLManErr_Interface::GetInstance().GetErrorText(Kernel_BasisErrorToResultCode(berr));
}

/* --------------------------------------------------------------------------- */
externCpp const SAPDB_Char *  Kernel_BasisErrorToString (const tgg00_BasisError berr)
{
    return SQLManErr_Interface::GetInstance().GetBaseErrorText(berr);
}

/* --------------------------------------------------------------------------- */
externCpp void Kernel_AppendBasisError (const tgg00_BasisError  error,
                                        SAPDBErr_MessageList   &errlist,
                                        const char             *title)
{
    if ( error == e_ok )
        return;

    errlist.AddDetail( SDBMSG_KERNELCOMMON_ERROR_OUTPUT,
                       Msg_Arg("TITLE",       title == 0 ? "BasisError" : title),
                       Msg_Arg("BASIS_ERROR", SAPDB_ToString(error)),
                       Msg_Arg("ERRORTEXT",  ( (error > 99 || error < 0 ) ? Kernel_BasisErrorToString(error) : "") ));
}
