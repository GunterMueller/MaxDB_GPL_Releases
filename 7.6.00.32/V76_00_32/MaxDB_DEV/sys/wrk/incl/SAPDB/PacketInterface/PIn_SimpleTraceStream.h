/*!================================================================
 module:        PIn_SimpleTraceStream.h

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  Tracing

 description:   Simple Trace Stream

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

#ifndef PIn_SIMPLETRACESTREAM_H
#define PIn_SIMPLETRACESTREAM_H

#include "gsp12.h"

/*!
  Class: PIn_TraceStream
 */
class PIn_TraceStream
{
public:
    /*!
     */
    PIn_TraceStream (isp12_OutputStream * outstream);
    virtual ~PIn_TraceStream ();
    virtual void procEntry (const char * name);
    virtual void procEntryParms (const char * name, const char * fmt, ...);
    virtual void procReturns (const char * fmt, ...);
    virtual void procExit (const char * name);
    virtual void writeln (const char * name, int len = -1);
    virtual void printf (const char * format, ...);
    virtual void vprintf (const char * format, va_list ap);
    virtual void setIndentation (int newIndent);
    virtual void hex (const char * comment, const void * buf, int len);
    virtual void hexFromTo (const char * comment, const void * buf,
        int startPos, int endPos);
    virtual void hexFromFor (const char * comment, const void * buf,
        int startPos, int len);

protected:
    isp12_OutputStream * outstream;
    int nesting;
    int indent;
};

typedef struct PIn_EnumDesc {
    int maxElement;
    const char * defaultFormat;
    const char ** enumStrings;
} PIn_EnumDesc;


#define tmpBufSize_PI 50

/*! Function: PIn_EnumDump

    description: returns a character representation of an enumeration element.
 */
const char * PIn_EnumDump (
    const PIn_EnumDesc * enumDesc,
    int element,
    char * tmpBuf = NULL);


#endif

