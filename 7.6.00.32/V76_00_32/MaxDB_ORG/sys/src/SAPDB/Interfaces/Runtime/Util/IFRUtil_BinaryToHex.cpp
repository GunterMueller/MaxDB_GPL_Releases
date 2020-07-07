/*!
  @file           IFRUtil_BinaryToHex.cpp
  @author         D039759
  @ingroup        Utilities
  @brief
  @see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (C) 2001-2005 SAP AG

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

#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_BinaryToHex.h"
#include "hsp77.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
tsp78ConversionResult
IFRUtil_HexToBinary(void *destbuffer,
                    tsp00_Uint4 destbufferlengthinbytes,
                    tsp00_Uint4 *destbyteswritten,
                    const tsp77encoding *sourceencoding,
                    const void *srcbuffer,
                    tsp00_Uint4  srcbufferlengthinbytes,
                    tsp00_Uint4 *srcbytesparsed)
{
    if(destbyteswritten) {
        (*destbyteswritten) = 0;
    }
    
    if(srcbytesparsed) {
        (*srcbytesparsed) = 0;
    }

    if(destbuffer == 0 || srcbuffer == 0) {
        return sp78_NullArgument;
    }

    unsigned char *src = (unsigned char *) srcbuffer;
    unsigned char *dest = (unsigned char *) destbuffer;
    IFR_UInt4 offset;    // offset to next
    IFR_UInt4 distance;  // distance to significant byte when pointer is at start of char
    if(sourceencoding == sp77encodingAscii || sourceencoding == sp77encodingUTF8) {
        offset   = 1;
        distance = 0; 
    } else if(sourceencoding == sp77encodingUCS2) {
        offset   = 2;
        distance = 1;
    } else if(sourceencoding == sp77encodingUCS2Swapped) {
        offset   = 2;
        distance = 0;
    } else {
        return sp78_BadSourceEncoding;
    }
    
    IFR_UInt4 dest_i = 0;
    IFR_UInt4 src_i = 0;
    IFR_UInt4 l;
    while(dest_i < destbufferlengthinbytes) {
        // check whether the upper bytes are zero
        for(l=0; l<offset; ++l) {
            if(l!=distance && src[src_i + l]) {
                return sp78_SourceCorrupted;
            }
        }

        // convert 1st half-byte
        switch(src[src_i + distance]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            dest[dest_i] = (src[src_i + distance] - '0') << 4;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            dest[dest_i] = (src[src_i + distance] - 'A' + 10) << 4;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            dest[dest_i] = (src[src_i + distance] - 'a' + 10) << 4;
            break;
        default:
            return sp78_SourceCorrupted; // not found any good hex char
        }
        
        src_i += offset;
        
        if(src_i >= srcbufferlengthinbytes) {
            return sp78_SourceExhausted;
        }
        
        // check whether the upper bytes are zero
        for(l=0; l<offset; ++l) {
            if(l!=distance && src[src_i + l]) {
                return sp78_SourceCorrupted;
            }
        }
        
        // convert 2nd half-byte
        switch(src[src_i + distance]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            dest[dest_i] |= (src[src_i + distance] - '0') ;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            dest[dest_i] |= (src[src_i + distance] - 'A' + 10);
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            dest[dest_i] |= (src[src_i + distance] - 'a' + 10);
            break;
        default:
            return sp78_SourceCorrupted; // not found any good hex char
        }
        
        if(destbyteswritten) {
            (*destbyteswritten)++;
        }

        if(srcbytesparsed) {
            (*srcbytesparsed) += (offset << 1);
        }

        src_i += offset;
        
        if(src_i >= srcbufferlengthinbytes) {
            return src_i == srcbufferlengthinbytes ? sp78_Ok : sp78_SourceExhausted;
        }
        
        ++dest_i;
    }
    return sp78_TargetExhausted;
}
                      

//----------------------------------------------------------------------
tsp78ConversionResult IFRUtil_BinaryToHex(const tsp77encoding *targetencoding,
                                          void *destbuffer,
                                          tsp00_Uint4 destbufferlengthinbytes,
                                          tsp00_Uint4 *destbyteswritten,
                                          const void *srcbuffer,
                                          tsp00_Uint4  srcbufferlengthinbytes,
                                          tsp00_Uint4 *srcbytesparsed)
{
    if(destbyteswritten) {
        (*destbyteswritten) = 0;
    }
    
    if(srcbytesparsed) {
        (*srcbytesparsed) = 0;
    }
    if(destbuffer == 0 || srcbuffer == 0) {
        return sp78_NullArgument;
    }

    unsigned char *src = (unsigned char *) srcbuffer;
    unsigned char *dest = (unsigned char *) destbuffer;
    IFR_UInt4 offset;    // offset to next
    IFR_UInt4 distance;  // distance to significant byte when pointer is at start of char

    if(targetencoding == sp77encodingAscii || targetencoding == sp77encodingUTF8) {
        offset   = 1;
        distance = 0; 
    } else if(targetencoding == sp77encodingUCS2) {
        offset   = 2;
        distance = 1;
    } else if(targetencoding == sp77encodingUCS2Swapped) {
        offset   = 2;
        distance = 0;
    } else {
        return sp78_BadTargetEncoding;
    }

    IFR_UInt4 dest_i = 0;
    IFR_UInt4 src_i = 0;
    IFR_UInt4 l;
    const char *map="0123456789ABCDEF";

    while(dest_i + offset < destbufferlengthinbytes 
          && src_i < srcbufferlengthinbytes) {
    
        for(l=0; l<offset; ++l) {
            if(l != distance) {
                dest[dest_i + l] = '\0';
            }
        }
        
        dest[dest_i + distance] = map[src[src_i] >> 4];
        
        dest_i += offset;
        for(l=0; l<offset; ++l) {
            if(l != distance) {
                dest[dest_i + l] = '\0';
            }
        }
        
        dest[dest_i + distance] = map[src[src_i] & 0xF];
        
        dest_i += offset;
        ++src_i;
        
        if(destbyteswritten) {
            (*destbyteswritten) += (offset << 1);
        }
        
        if(srcbytesparsed) {
            ++(*srcbytesparsed);
        }
        
    }

    if(src_i == srcbufferlengthinbytes) {
        return sp78_Ok;
    } else {
        return sp78_TargetExhausted;
    }
}

IFR_END_NAMESPACE
