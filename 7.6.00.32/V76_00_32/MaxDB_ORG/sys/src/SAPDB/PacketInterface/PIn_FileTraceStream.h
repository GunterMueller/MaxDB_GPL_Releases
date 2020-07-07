/*!================================================================
 module:        PIn_FileTraceStream.h

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  Tracing

 description:   FileTraceStream

 -------------------------------------------------------------------

 Copyright (c) 2000-2005 SAP AG




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




 ===================================================================*/

#ifndef PIn_FILETRACESTREAM_H
#define PIn_FILETRACESTREAM_H

#include "SAPDB/PacketInterface/PIn_SimpleTraceStream.h"

/*!
  Class: PIn_FileTraceStream
 */

extern PIn_TraceStream * PIn_GlobalTrace;

externCpp void PIn_openTrace (const char * fname, bool append = false);

/*!
  Class: PIn_Tracer
 */
class PIn_Tracer
{
public:
    PIn_Tracer (const char * procName);
    ~PIn_Tracer ();
    void trace (int layer, const char *format, ...);

private:
    const char * name;
};

#if defined (USE_IN98TRACER) || defined (USE_PITRACER)
undef TRACER_CLASS_MSP00
#define TRACER_CLASS_MSP00 PIn_Tracer
#endif

#endif

