/*!================================================================
 module:        PIn_SimpleTraceStream.cpp

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

#include "hsp77.h"
#include "SAPDB/PacketInterface/PIn_SimpleTraceStream.h"

#define printfBufSizeC 500

/*----------------------------------------*/

static void breakIntoLines (
    PIn_TraceStream * stream,
    const char * buf)
{
    const char * pos = buf;
    const char * lastPos = buf;

    while ((pos = strchr (lastPos, '\n')) != NULL) {
        stream->writeln (lastPos, (int)(pos - lastPos));
        lastPos = pos + 1;
    }
    if (strlen (lastPos) > 0) {
        stream->writeln (lastPos);
    }
}

/*----------------------------------------*/

static void nestedPrintf (
    PIn_TraceStream * stream,
    const char * fmt1,
    const char * fmt2,
    const char * closer,
    va_list      ap2,
    ...)
{
    char buf [printfBufSizeC];
    int count;
    va_list         ap1;

    va_start (ap1, ap2);
    count = sp77vsprintf (buf, sizeof (buf), fmt1, ap1);
    count += sp77vsprintf (buf + count, sizeof (buf) - count, fmt2, ap2);
    // todo: check for overflow and truncate buffer
    strcpy (buf + count, closer);
    breakIntoLines (stream, buf);
    va_end (ap1);
}

/*----------------------------------------*/

PIn_TraceStream::PIn_TraceStream (isp12_OutputStream * outstreamP)
{
    this->outstream = outstreamP;
    this->nesting = 0;
    this->indent = 0;
}

/*----------------------------------------*/

PIn_TraceStream::~PIn_TraceStream ()
{
}

/*----------------------------------------*/

void PIn_TraceStream::procEntry (const char * name)
{
    this->printf ("%*s-->%s\n", this->nesting * this->indent, "", name);
    ++this->nesting;
}

/*----------------------------------------*/

void PIn_TraceStream::procEntryParms (
    const char * name,
    const char * fmt,
    ...)
{
    va_list         ap;

    va_start (ap, fmt);
    nestedPrintf (this, "%*s-->%s (", fmt, ")\n", ap,
        this->nesting * this->indent, "", name);
    va_end (ap);
}

/*----------------------------------------*/

void PIn_TraceStream::procReturns (const char * fmt, ...)
{
    va_list         ap;

    va_start (ap, fmt);
    nestedPrintf (this, "%*s***returns: ", fmt, "\n", ap,
        this->nesting * this->indent, "");
    va_end (ap);
}

/*----------------------------------------*/

void PIn_TraceStream::procExit (const char * name)
{
    --this->nesting;
    this->printf ("%*s<--%s\n", this->nesting * this->indent, "", name);
}

/*----------------------------------------*/

void PIn_TraceStream::writeln (const char * name, int len)
{
    if(this && this->outstream) {
        this->outstream->writeln (name, len);
    }
}

/*----------------------------------------*/

void PIn_TraceStream::printf (const char * format, ...)
{
    va_list         ap;

    va_start (ap, format);
    this->vprintf (format, ap);
    va_end (ap);
}

/*----------------------------------------*/

void PIn_TraceStream::vprintf (const char * format, va_list ap)
{
    char buf [printfBufSizeC];

    sp77vsprintf (buf, sizeof (buf), format, ap);
    breakIntoLines (this, buf);
}

/*----------------------------------------*/

void PIn_TraceStream::setIndentation (int newIndent)
{
    this->indent = newIndent;
}

/*----------------------------------------*/

void PIn_TraceStream::hex (const char * comment, const void * buf, int len)
{
    this->hexFromTo (comment, buf, 0, len);
}

/*----------------------------------------*/

void PIn_TraceStream::hexFromTo (
    const char * comment,
    const void * buf,
    int startPos,
    int endPos)
{
    this->hexFromFor (comment, buf, startPos, endPos - startPos);
}

/*----------------------------------------*/

void PIn_TraceStream::hexFromFor (
    const char * comment,
    const void * bufParm,
    int startPos,
    int len)
{
    char line [200];
    int i;
    int currentByte;
    const teo00_Byte * buf = STATIC_CAST (const teo00_Byte *, bufParm);
    static const char hexDigitsC [] = "0123456789abcdef";
    static const int posPosC = 0;
    static const int hexPosC = 10;
    static const int clearPosC = 60;
    static const int chunkSizeC = 16;

    if (comment != NULL) {
        this->writeln (comment);
    }
    while (len > 0) {
        memset (line, ' ', sizeof (line));
        // write pos
        sp77sprintf (line + posPosC, sizeof (line) + posPosC, "%8x", startPos);
        line [posPosC + 8] = ' ';  // sp77printf ended with '\0'
        // write hex
        for (i = 0; (i < chunkSizeC) && (i < len); ++i) {
            currentByte = buf [startPos + i];
            line [hexPosC + 3 * i] = hexDigitsC [currentByte / 16];
            line [hexPosC + 3 * i + 1] = hexDigitsC [currentByte % 16];
        }
        // write clear
        line [clearPosC] = '|';
        for (i = 0; (i < chunkSizeC) && (i < len); ++i) {
            currentByte = buf [startPos + i];
            if (currentByte >= ' ') {
                line [clearPosC + 1 + i] = currentByte;
            }
            else {
                line [clearPosC + 1 + i] = '.';
            }
        }
        line [clearPosC + 1 + i] = '|';
        // write line
        this->writeln (line, clearPosC + 1 + i + 1);
        startPos += chunkSizeC;
        len -= chunkSizeC;
    }
}

/*----------------------------------------*/

const char * PIn_EnumDump (
    const PIn_EnumDesc * enumDesc,
    int element,
    char * tmpBuf)
{
    const char * result;
    static const char * defaultBuf = "Invalid";

    if ((0 <= element) && (element <= enumDesc->maxElement)) {
        result = enumDesc->enumStrings [element];
    }
    else {
        if (tmpBuf != NULL) {
            sp77sprintf (tmpBuf, tmpBufSize_PI, enumDesc->defaultFormat, element);
            result = tmpBuf;
        }
        else {
            result = defaultBuf;
        }
    }
    return result;
}

/*----------------------------------------*/


