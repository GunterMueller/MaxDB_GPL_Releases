/*!
  @file           Kernel_Trace.hpp
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
#ifndef KERNEL_TRACE_H
#define KERNEL_TRACE_H

#include "ggg00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

/// forward decl
class SAPDBTrace_Topic;
/// forward decl
class SAPDBErr_MessageList;
/*!
   @class          Kernel_BasisErrorTracer
   @brief          traces basiserror in scope if not e_ok
 */
class Kernel_BasisErrorTracer
{
public:
    /// This is used for scope oriented tracing of the well known tgg00_BasisError
    Kernel_BasisErrorTracer(tgg00_BasisError &berror,
                            SAPDBTrace_Topic &tracetopic,
                            SAPDB_UInt        tracelevel,
                            char             *title = 0);
    /// If this is called the trace is written if error is not e_ok
    ~Kernel_BasisErrorTracer();
private:
    tgg00_BasisError &m_error;
    SAPDBTrace_Topic &m_tracetopic;
    SAPDB_UInt        m_tracelevel;
    const char       *m_title;
};
/*!
	@interface Kernel_IsOkTracer
    @brief     traces basiserror in scope if not e_ok
 */
class Kernel_IsOkTracer
{
public:
    /// This is used for scope oriented tracing of a bool value
    Kernel_IsOkTracer(bool             &isok,
                      SAPDBTrace_Topic &tracetopic,
                      SAPDB_UInt        tracelevel,
                      char             *title = 0);
    /// If this is called the trace is written if the bool value is false
    ~Kernel_IsOkTracer();
private:
    bool             &m_isok;
    SAPDBTrace_Topic &m_tracetopic;
    SAPDB_UInt        m_tracelevel;
    const char       *m_title;
};
/// This macro makes it easier to write scope oriented trace for the basis error
#ifdef SAPDB_SLOW
#define KERNEL_TRACE_BASIS_ERROR(error, topic, level) \
    Kernel_BasisErrorTracer Kernel_BasisErrorTracerObject (error, topic, level)
#else
#define KERNEL_TRACE_BASIS_ERROR(error, topic, level)
#endif
/// This macro makes it easier to write scope oriented trace for as a bool value
#ifdef SAPDB_SLOW
#define KERNEL_TRACE_IS_OK(isok, topic, level) \
    Kernel_IsOkTracer Kernel_IsOkTracerObject (isok, topic, level)
#else
#define KERNEL_TRACE_IS_OK(isok, topic, level)
#endif
/// This is the maximum length to write to the trace. This is used by Kernel_TraceBuffer().
extern SAPDB_UInt4 Kernel_TraceMaxBufferLength;
/// This traces a byte buffer into the vtrace
externCpp void Kernel_TraceBuffer ( const void       *buffer,
                                    SAPDB_UInt        length,
                                    const SAPDB_Char *title   = NULL,
                                    SAPDB_UInt        size    = 0 );
/// converts an internal basis error into an externally known return code
externCpp SAPDB_Int2 Kernel_BasisErrorToResultCode (const tgg00_BasisError berr);
/// converts an internal basis error into a short explanation string
externCpp const SAPDB_Char *  Kernel_BasisErrorToResultString (const tgg00_BasisError berr);
/// converts an internal basis error into a basis error string
externCpp const SAPDB_Char *  Kernel_BasisErrorToString (const tgg00_BasisError berr);
/// to the given errlist is a exception added which describes the given basis error
externCpp void Kernel_AppendBasisError (const tgg00_BasisError  error,
                                        SAPDBErr_MessageList   &errlist,
                                        const char             *title = 0);
#endif // KERNEL_TRACE_H
