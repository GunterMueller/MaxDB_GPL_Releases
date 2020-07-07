/*!*********************************************************************

    @file         SAPDBTrace_HexBuf.cpp
    
    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
***********************************************************************/

#include "SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp"

#include <string.h>
#include "SAPDBCommon/SAPDB_OStream.hpp"

/*----------------------------------------*/

SAPDBTrace_HexBuf::SAPDBTrace_HexBuf (
    const void * bufParm, 
    unsigned int lenParm)
{
    this->buf = static_cast<const unsigned char *>(bufParm);
    this->offset = 0;
    this->len = lenParm;
}

/*----------------------------------------*/

SAPDBTrace_HexBuf::SAPDBTrace_HexBuf (
    const void * bufParm, 
    unsigned int offsetParm, 
    unsigned int lenParm)
{
    this->buf = static_cast<const unsigned char *>(bufParm);
    this->offset = offsetParm;
    this->len = lenParm;
}

/*----------------------------------------*/

static const char hexDigitsC [] = "0123456789abcdef";

SAPDB_OStream & SAPDBTrace_HexBuf::Print (
    SAPDB_OStream & ostream) const
{
    const unsigned int bytesPerLineC = 16;
    const int lineSizeC = 79;
    char line [lineSizeC + 2];
    const unsigned char * bufPtr = this->buf + this->offset;
    unsigned int endPos = this->offset + this->len;
    int chunkPos;

    for (unsigned int pos = this->offset; pos < endPos; pos += bytesPerLineC, bufPtr += bytesPerLineC) {
        memset (line, ' ', lineSizeC);
        int linePos;
        /*
         * write position (4 bytes)
         */
        linePos = 0;
        unsigned int tmpPos = pos;
        for (int posDigit = 3; posDigit >= 0; --posDigit) {
            char hexDigit = hexDigitsC [tmpPos % 16];
            line [linePos + posDigit] = hexDigit;
            tmpPos /= 16;
        }
        /*
         * write 16 * (hex hex blank)
         */
        linePos = 6;
        int bytesInLine = (endPos - pos) > bytesPerLineC 
                ? bytesPerLineC 
                : (endPos - pos);
        for (chunkPos = 0; chunkPos < bytesInLine; ++chunkPos) {
            unsigned char byte = bufPtr [chunkPos];
            line [linePos] = hexDigitsC [byte / 16];
            line [linePos + 1] = hexDigitsC [byte % 16];
            linePos += 3;
        }
        /*
         * write | (16 * plain) |
         */
        linePos = 54;
        line [linePos] = '|';
        ++linePos;
        for (chunkPos = 0; chunkPos < bytesInLine; ++chunkPos) {
            unsigned char byte = bufPtr [chunkPos];
            if (byte < 32) {
                byte = '.';
            }
            line [linePos] = byte;
            ++linePos;
        }
        line [linePos] = '|';
        line [linePos + 1] = '\0';
        /*
         * output
         */        
        ostream << line;
        ostream.Flush ();
    }
    return ostream;
}

/*----------------------------------------*/

