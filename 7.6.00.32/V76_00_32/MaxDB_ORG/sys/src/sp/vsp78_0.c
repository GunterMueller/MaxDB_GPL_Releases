/*!**************************************************************************

  module       : vsp78.c

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Ascii/UCS2/UTF8 Convertion Matrix
  description : Contains routines for converting between different formats

  last changed: 2000-10-09  13:55
  first created:2000-08-30  15:11

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hsp78_0.h"
#include "hsp83.h" /* UTF8 + UCS2 conversions */
#include "SAPDBCommon/SAPDB_string.h"

#include <stdio.h>
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define LSB_INDEX_UCS2         1
#define LSB_INDEX_SWAPPED_UCS2 0

#define UCS2_SURROGATE_LOWER_BOUND 0xd8
#define UCS2_SURROGATE_UPPER_BOUND 0xdf

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static tsp78ConversionResult sp78_FromSame(
 const tsp77encoding *destEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten );

/*---------------------------------------------------------------------------*/

static tsp78ConversionResult sp78_FromAsciiToUTF8(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten );

/*---------------------------------------------------------------------------*/

static tsp78ConversionResult sp78_FromAsciiToUCS2(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 int            destLSBIndex );

/*---------------------------------------------------------------------------*/

static tsp78ConversionResult sp78_FromSwapped(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten );

/*---------------------------------------------------------------------------*/

static tsp78ConversionResult sp78_FromUCS2(
 const tsp77encoding *destEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 int            srcLSBIndex,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten );

/*---------------------------------------------------------------------------*/

static tsp78ConversionResult sp78_FromUTF8ToAscii(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten );

/*---------------------------------------------------------------------------*/

static tsp78ConversionResult sp78_FromUTF8ToUCS2(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 int            destLSBIndex );

/*---------------------------------------------------------------------------*/

static tsp78ConversionResult sp78_AddTerminator(
 tsp78ConversionResult oldResult,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten );

/*---------------------------------------------------------------------------*/
/* Calling interface functions */
/*---------------------------------------------------------------------------*/
static tsp78ConversionResult sp78_CallFromSame(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromSame( destEncoding, srcBuffer, srcLengthInBytes, srcBytesParsed,
                        destBuffer, destBufferLengthInBytes, destBytesWritten );
}

static tsp78ConversionResult sp78_CallFromSameUCS2(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
    /* UCS2 character never splitted due to buffer length alignment */
  return sp78_FromSame( destEncoding, srcBuffer, (srcLengthInBytes&(~1)), srcBytesParsed,
                        destBuffer, (destBufferLengthInBytes&(~1)), destBytesWritten );
}

static tsp78ConversionResult sp78_CallFromAsciiToUCS2(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromAsciiToUCS2(
        srcBuffer, srcLengthInBytes, srcBytesParsed,
        destBuffer, destBufferLengthInBytes, destBytesWritten,
        destEncoding->EncodingType == csp_unicode ? LSB_INDEX_UCS2 : LSB_INDEX_SWAPPED_UCS2 );
}

static tsp78ConversionResult sp78_CallFromAsciiToUTF8(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromAsciiToUTF8(
          srcBuffer, srcLengthInBytes, srcBytesParsed,
          destBuffer, destBufferLengthInBytes, destBytesWritten );
}

static tsp78ConversionResult sp78_CallFromSwapped(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromSwapped(
          srcBuffer, srcLengthInBytes, srcBytesParsed,
          destBuffer, destBufferLengthInBytes, destBytesWritten );
}

static tsp78ConversionResult sp78_CallFromUCS2(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromUCS2( destEncoding,
          srcBuffer, srcLengthInBytes, srcBytesParsed, LSB_INDEX_UCS2,
          destBuffer, destBufferLengthInBytes, destBytesWritten );
}

static tsp78ConversionResult sp78_CallFromUCS2Swapped(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromUCS2( destEncoding,
          srcBuffer, srcLengthInBytes, srcBytesParsed, LSB_INDEX_SWAPPED_UCS2,
          destBuffer, destBufferLengthInBytes, destBytesWritten );
}

static tsp78ConversionResult sp78_CallFromUTF8toAscii(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromUTF8ToAscii(
          srcBuffer, srcLengthInBytes, srcBytesParsed,
          destBuffer, destBufferLengthInBytes, destBytesWritten );
}

static tsp78ConversionResult sp78_CallFromUTF8toUCS2(
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten)
{
  return sp78_FromUTF8ToUCS2(
          srcBuffer, srcLengthInBytes, srcBytesParsed,
          destBuffer, destBufferLengthInBytes, destBytesWritten,
          destEncoding->EncodingType == csp_unicode ? LSB_INDEX_UCS2 : LSB_INDEX_SWAPPED_UCS2);
}

static tsp78ConversionResult sp78_CallFromAsciiToUCS4 (
    const tsp77encoding *srcEncoding,
    const void    *srcBuffer,
    tsp00_Uint4    srcLengthInBytes,
    tsp00_Uint4   *srcBytesParsed,
    const tsp77encoding *destEncoding,
    void *         destBuffer,
    tsp00_Uint4    destBufferLengthInBytes,
    tsp00_Uint4   *destBytesWritten)
{
    tsp78ConversionResult result = sp78_Ok;
    tsp00_Uint4 targetLengthInChars = destBufferLengthInBytes/sizeof(tsp81_UCS4Char);

    if ( targetLengthInChars < srcLengthInBytes )
    {
      result = sp78_TargetExhausted;
    }
    *srcBytesParsed = sp81AnyASCIItoUCS4((tsp81_UCS4Char *)destBuffer,
                                        targetLengthInChars,
                                        destEncoding->EncodingType == csp78_ucs4_swap,
                                        destBytesWritten,
                                        srcBuffer,
                                        srcLengthInBytes,
                                        sp81CodePage );
    *destBytesWritten *= sizeof(tsp81_UCS4Char);

    return result;
}

static tsp78ConversionResult sp78_CallFromUCS2toUCS4 (
    const tsp77encoding *srcEncoding,
    const void    *srcBuffer,
    tsp00_Uint4    srcLengthInBytes,
    tsp00_Uint4   *srcBytesParsed,
    const tsp77encoding *destEncoding,
    void *         destBuffer,
    tsp00_Uint4    destBufferLengthInBytes,
    tsp00_Uint4   *destBytesWritten)
{
    unsigned char * sourcePos = (unsigned char *) srcBuffer;
    unsigned char * destPos = (unsigned char *) destBuffer;
    unsigned int charsToConvert;
    unsigned int charsConverted = 0;
    tsp78ConversionResult result = sp78_Ok;
    /*
     * calculate output sizes to prevent overwrite
     */
    if (srcLengthInBytes * 2 > destBufferLengthInBytes) {
        charsToConvert = destBufferLengthInBytes / 4;
        result = sp78_TargetExhausted;
    }
    else {
        charsToConvert = srcLengthInBytes / 2;
    }
    /*
     * switch depending on swapping of source and destination
     */
    if ((srcEncoding->EncodingType == csp_unicode)
        && (destEncoding->EncodingType == csp78_ucs4))
    {
        for (charsConverted = 0; charsConverted < charsToConvert; ++charsConverted) {
            unsigned char low = sourcePos [1];
            unsigned char high = sourcePos [0];
            if ((UCS2_SURROGATE_LOWER_BOUND <= high) && (high <= UCS2_SURROGATE_LOWER_BOUND) ) {
                result = sp78_BadSourceEncoding;
                break;
            }
            destPos [0] = 0;
            destPos [1] = 0;
            destPos [2] = high;
            destPos [3] = low;
            sourcePos += 2;
            destPos += 4;
        }
    }
    else if ((srcEncoding->EncodingType == csp_unicode_swap)
        && (destEncoding->EncodingType == csp78_ucs4_swap))
    {
        for (charsConverted = 0; charsConverted < charsToConvert; ++charsConverted) {
            unsigned char low = sourcePos [0];
            unsigned char high = sourcePos [1];
            if ((UCS2_SURROGATE_LOWER_BOUND <= high) && (high <= UCS2_SURROGATE_LOWER_BOUND) ) {
                result = sp78_BadSourceEncoding;
                break;
            }
            destPos [0] = low;
            destPos [1] = high;
            destPos [2] = 0;
            destPos [3] = 0;
            sourcePos += 2;
            destPos += 4;
        }
    }
    else {
        /*
         * conversion between different swapping currently not supported
         */
        result = sp78_BadTargetEncoding;
    }
    *srcBytesParsed = charsConverted * 2;
    *destBytesWritten = charsConverted * 4;
    return sp78_Ok;
}

static tsp78ConversionResult sp78_CallFromUTF8toUCS4 (
    const tsp77encoding *srcEncoding,
    const void    *srcBuffer,
    tsp00_Uint4    srcLengthInBytes,
    tsp00_Uint4   *srcBytesParsed,
    const tsp77encoding *destEncoding,
    void *         destBuffer,
    tsp00_Uint4    destBufferLengthInBytes,
    tsp00_Uint4   *destBytesWritten)
{
    return sp78_BadTargetEncoding;
}

static tsp78ConversionResult sp78_CallFromUCS4toAscii (
    const tsp77encoding *srcEncoding,
    const void    *srcBuffer,
    tsp00_Uint4    srcLengthInBytes,
    tsp00_Uint4   *srcBytesParsed,
    const tsp77encoding *destEncoding,
    void *         destBuffer,
    tsp00_Uint4    destBufferLengthInBytes,
    tsp00_Uint4   *destBytesWritten)
{
    return sp78_BadSourceEncoding;
}

static tsp78ConversionResult sp78_CallFromUCS4toUCS2 (
    const tsp77encoding *srcEncoding,
    const void    *srcBuffer,
    tsp00_Uint4    srcLengthInBytes,
    tsp00_Uint4   *srcBytesParsed,
    const tsp77encoding *destEncoding,
    void *         destBuffer,
    tsp00_Uint4    destBufferLengthInBytes,
    tsp00_Uint4   *destBytesWritten)
{
    unsigned char * sourcePos = (unsigned char *) srcBuffer;
    unsigned char * destPos = (unsigned char *) destBuffer;
    unsigned int charsToConvert;
    unsigned int charsConverted = 0;
    tsp78ConversionResult result = sp78_Ok;

    /*
     * calculate output sizes to prevent overwrite
     */
    if (srcLengthInBytes > destBufferLengthInBytes * 2) {
        charsToConvert = destBufferLengthInBytes / 2;
        result = sp78_TargetExhausted;
    }
    else {
        charsToConvert = srcLengthInBytes / 4;
    }
    /*
     * switch depending on swapping of source and destination
     */
    if ((srcEncoding->EncodingType == csp78_ucs4)
        && (destEncoding->EncodingType == csp_unicode))
    {
        for (charsConverted = 0; charsConverted < charsToConvert; ++charsConverted) {
            unsigned char low = sourcePos [3];
            unsigned char high = sourcePos [2];
            if (sourcePos [0] != 0 || sourcePos [1] != 0) {
                result = sp78_BadSourceEncoding;
                break;
            }
            if ((UCS2_SURROGATE_LOWER_BOUND <= high) && (high <= UCS2_SURROGATE_LOWER_BOUND) ) {
                result = sp78_BadSourceEncoding;
                break;
            }
            destPos [0] = high;
            destPos [1] = low;
            sourcePos += 4;
            destPos += 2;
        }
    }
    else if ((srcEncoding->EncodingType == csp78_ucs4_swap)
        && (destEncoding->EncodingType == csp_unicode_swap))
    {
        for (charsConverted = 0; charsConverted < charsToConvert; ++charsConverted) {
            unsigned char low = sourcePos [0];
            unsigned char high = sourcePos [1];
            if (sourcePos [2] != 0 || sourcePos [3] != 0) {
                result = sp78_BadSourceEncoding;
                break;
            }
            if ((UCS2_SURROGATE_LOWER_BOUND <= high) && (high <= UCS2_SURROGATE_LOWER_BOUND) ) {
                result = sp78_BadSourceEncoding;
                break;
            }
            destPos [0] = low;
            destPos [1] = high;
            sourcePos += 4;
            destPos += 2;
        }
    }
    else {
        /*
         * conversion between different swapping currently not supported
         */
        result = sp78_BadTargetEncoding;
    }
    *srcBytesParsed = charsConverted * 4;
    *destBytesWritten = charsConverted * 2;
    return result;
}

static tsp78ConversionResult sp78_CallFromUCS4toUTF8 (
    const tsp77encoding *srcEncoding,
    const void    *srcBuffer,
    tsp00_Uint4    srcLengthInBytes,
    tsp00_Uint4   *srcBytesParsed,
    const tsp77encoding *destEncoding,
    void *         destBuffer,
    tsp00_Uint4    destBufferLengthInBytes,
    tsp00_Uint4   *destBytesWritten)
{
    return sp78_BadSourceEncoding;
}

static tsp78ConversionResult sp78_CallFromUCS4toUCS4 (
    const tsp77encoding *srcEncoding,
    const void    *srcBuffer,
    tsp00_Uint4    srcLengthInBytes,
    tsp00_Uint4   *srcBytesParsed,
    const tsp77encoding *destEncoding,
    void *         destBuffer,
    tsp00_Uint4    destBufferLengthInBytes,
    tsp00_Uint4   *destBytesWritten)
{
    return sp78_BadSourceEncoding;
}
/*---------------------------------------------------------------------------*/

tsp78ConvertFunc sp78Converter[6][6] = {
  {
    sp78_CallFromSame,
    sp78_CallFromAsciiToUCS2,
    sp78_CallFromAsciiToUCS2,
    sp78_CallFromAsciiToUTF8,
    sp78_CallFromAsciiToUCS4,
    sp78_CallFromAsciiToUCS4
  },
  {
    sp78_CallFromUCS2,
    sp78_CallFromSameUCS2,
    sp78_CallFromSwapped,
    sp78_CallFromUCS2,
    sp78_CallFromUCS2toUCS4,
    sp78_CallFromUCS2toUCS4
  },
  {
    sp78_CallFromUCS2Swapped,
    sp78_CallFromSwapped,
    sp78_CallFromSameUCS2,
    sp78_CallFromUCS2Swapped,
    sp78_CallFromUCS2toUCS4,
    sp78_CallFromUCS2toUCS4
  },
  {
    sp78_CallFromUTF8toAscii,
    sp78_CallFromUTF8toUCS2,
    sp78_CallFromUTF8toUCS2,
    sp78_CallFromSame,
    sp78_CallFromUTF8toUCS4,
    sp78_CallFromUTF8toUCS4
  },
  {
    sp78_CallFromUCS4toAscii,
    sp78_CallFromUCS4toUCS2,
    sp78_CallFromUCS4toUCS2,
    sp78_CallFromUCS4toUTF8,
    sp78_CallFromUCS4toUCS4,
    sp78_CallFromUCS4toUCS4
  },
  {
    sp78_CallFromUCS4toAscii,
    sp78_CallFromUCS4toUCS2,
    sp78_CallFromUCS4toUCS2,
    sp78_CallFromUCS4toUTF8,
    sp78_CallFromUCS4toUCS4,
    sp78_CallFromUCS4toUCS4
  }
};

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

int sp78getConvertFuncIndex( const tsp77encoding *encoding )
{
  switch( encoding->EncodingType )
  {
  case csp_ascii:
    return 0;
  case csp_unicode:
    return 1;
  case csp_unicode_swap:
    return 2;
  case csp_utf8:
    return 3;
  case csp78_ucs4:
    return 4;
  case csp78_ucs4_swap:
    return 5;
  default:
    break;
  }
  return -1;
}

/*---------------------------------------------------------------------------*/

tsp78ConversionResult sp78convertString(
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 tsp00_Bool     addZeroTerminator,
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcBufferLengthInBytes,
 tsp00_Uint4   *srcBytesParsed)
{
  tsp78ConversionResult result = sp78_Ok;
  int isExhausted = false; /* Indicator that last character in source buffer is incomplete */
  unsigned int completeSrcBufferLengthInBytes;
  int srcEncodingIndex;
  int destEncodingIndex;

  *srcBytesParsed = 0;
  *destBytesWritten = 0;

  if ( !srcEncoding
    || !destEncoding
    || (!srcBuffer && (srcBufferLengthInBytes > 0)) /* PTS 1108693 */
    || !srcBytesParsed
    || !destBuffer
    || !destBytesWritten )
  {
    result = sp78_NullArgument;
  }
  else if ( destBufferLengthInBytes == 0 )
  {
    result = sp78_TargetExhausted;
  }
  else
  {
    srcEncodingIndex = sp78getConvertFuncIndex(srcEncoding);
    if ( srcEncodingIndex == -1 )
    {
      result = sp78_BadSourceEncoding;
    }

    if ( result == sp78_Ok )
    {
      destEncodingIndex = sp78getConvertFuncIndex(destEncoding);
      if ( destEncodingIndex == -1 )
      {
        result = sp78_BadTargetEncoding;
      }
    }

    if ( result == sp78_Ok )
    {
      if ( srcBufferLengthInBytes > 0 )
      {
        /* Find out number of bytes of complete srcBuffer characters */
        isExhausted = srcEncoding->stringComplete(srcBuffer, srcBufferLengthInBytes, &completeSrcBufferLengthInBytes);

        /* Call the correct conversion function */
        result = sp78Converter[srcEncodingIndex][destEncodingIndex](
          srcEncoding,   srcBuffer, completeSrcBufferLengthInBytes, srcBytesParsed,
          destEncoding, destBuffer, destBufferLengthInBytes,        destBytesWritten );
      }
      else
      {
        *srcBytesParsed = 0;
        *destBytesWritten = 0;
      }
    }
  }

  if ( result == sp78_Ok )
  {
    if ( addZeroTerminator )
    {
      result = sp78_AddTerminator( result,
                                   destEncoding,
                                   destBuffer,
                                   destBufferLengthInBytes,
                                   destBytesWritten );
    }
  }

  if ( result == sp78_Ok && isExhausted )
  {
    result = sp78_SourceExhausted;
  }

  return result;
}

/*---------------------------------------------------------------------------*/

tsp78ConversionResult sp78convertBuffer(
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 const tsp77encoding *srcEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcBufferLengthInBytes,
 tsp00_Uint4   *srcBytesParsed)
{
  return sp78convertString(destEncoding, destBuffer, destBufferLengthInBytes, destBytesWritten,
                           false,
                           srcEncoding,  srcBuffer,  srcBufferLengthInBytes,  srcBytesParsed);
}
/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

/* Convert between identical encodings */

static tsp78ConversionResult sp78_FromSame(
 const tsp77encoding *destEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten )
{
    tsp78ConversionResult result  = sp78_Ok;
    tsp00_Uint4 copyLengthInBytes = srcLengthInBytes;

    if ( destBufferLengthInBytes < copyLengthInBytes ) {
        if (destEncoding != sp77encodingAscii) {
            destEncoding->stringComplete(srcBuffer, destBufferLengthInBytes, &copyLengthInBytes);
        } else  {
            copyLengthInBytes = destBufferLengthInBytes;
        }
        result = sp78_TargetExhausted;
    }

    SAPDB_memcpy(destBuffer, srcBuffer, copyLengthInBytes);
    *srcBytesParsed = *destBytesWritten = copyLengthInBytes;

    return result;
}

/*---------------------------------------------------------------------------*/

/* Convert from Ascii to UTF8 */

static tsp78ConversionResult sp78_FromAsciiToUTF8(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten )
{
  tsp78ConversionResult      result     = sp78_Ok;
  tsp83UTF8_ConversionResult convResult = sp83UTF8Convert_Success;

  convResult = sp83UTF8fromASCII( (const tsp00_Char *)srcBuffer,
                                 srcLengthInBytes,
                                 srcBytesParsed,
                                 (tsp00_Byte *)destBuffer,
                                 destBufferLengthInBytes,
                                 destBytesWritten );

  if ( convResult == sp83UTF8Convert_TargetExhausted )
  {
    result = sp78_TargetExhausted;
  }

  return result;
}

/*---------------------------------------------------------------------------*/

/* Convert from Ascii8Bit to UCS2 or UCS2Swapped */

static tsp78ConversionResult sp78_FromAsciiToUCS2(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 int            destLSBIndex )
{
  tsp78ConversionResult result = sp78_Ok;
  tsp00_Uint4 targetLengthInChars = destBufferLengthInBytes/sizeof(tsp81_UCS2Char);

  if ( targetLengthInChars < srcLengthInBytes )
  {
    result = sp78_TargetExhausted;
  }
  if (sp81CodePage)
    *srcBytesParsed = sp81AnyASCIItoUCS2((tsp81_UCS2Char *)destBuffer,
                                      targetLengthInChars,
                                      destLSBIndex == LSB_INDEX_SWAPPED_UCS2,
                                      destBytesWritten,
                                      srcBuffer,
                                      srcLengthInBytes,
                                      sp81CodePage );
  else
  *srcBytesParsed = sp81ASCIItoUCS2((tsp81_UCS2Char *)destBuffer,
                                      targetLengthInChars,
                                      destLSBIndex == LSB_INDEX_SWAPPED_UCS2,
                                      destBytesWritten,
                                      srcBuffer,
                                      srcLengthInBytes );
  *destBytesWritten *= sizeof(tsp81_UCS2Char);

  return result;
}

/*---------------------------------------------------------------------------*/

/* Convert from UCS2 to UCS2Swapped or vice versa */

static tsp78ConversionResult sp78_FromSwapped(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten )
{
  tsp78ConversionResult result = sp78_Ok;

  tsp00_Byte *srcPointer = (tsp00_Byte *)srcBuffer;
  tsp00_Byte *destPointer = (tsp00_Byte *)destBuffer;
  tsp00_Uint4 conversionCount = 0;

  while ( conversionCount < srcLengthInBytes )
  {
    if ( destBufferLengthInBytes < (conversionCount+sizeof(tsp81_UCS2Char)) )
    {
      result = sp78_TargetExhausted;
      break;
    }
    destPointer[conversionCount] = srcPointer[conversionCount+1];
    destPointer[conversionCount+1] = srcPointer[conversionCount];
    conversionCount += sizeof(tsp81_UCS2Char);
  }

  *srcBytesParsed = conversionCount;
  *destBytesWritten = conversionCount;

  return result;
}

/*---------------------------------------------------------------------------*/

/* Convert to ASCII or UTF8 from UCS2 */

static tsp78ConversionResult sp78_FromUCS2(
 const tsp77encoding *destEncoding,
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 int            srcLSBIndex,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten )
{
  tsp78ConversionResult result = sp78_Ok;

  if ( destEncoding->EncodingType == csp_ascii )
  {
    tsp00_Uint4 srcLimit = srcLengthInBytes/sizeof(tsp81_UCS2Char);
    if ( srcLimit > destBufferLengthInBytes )
    {
      srcLimit = destBufferLengthInBytes;
      result = sp78_TargetExhausted;
    }
    *srcBytesParsed = sp81UCS2toASCII( (tsp00_Byte *)destBuffer,
                                        destBufferLengthInBytes,
                                        destBytesWritten,
                                        (tsp81_UCS2Char *)srcBuffer,
                                        srcLimit,
                                        srcLSBIndex == LSB_INDEX_SWAPPED_UCS2 )
                         * sizeof(tsp81_UCS2Char);
    if (*srcBytesParsed > (*destBytesWritten * sizeof(tsp81_UCS2Char))) {
      if (sp81CodePage) {
        *srcBytesParsed = sp81AnyUCS2toASCII( (tsp00_Byte *)destBuffer,
                                           destBufferLengthInBytes,
                                           destBytesWritten,
                                           (tsp81_UCS2Char *)srcBuffer,
                                           srcLimit,
                                           srcLSBIndex == LSB_INDEX_SWAPPED_UCS2, sp81CodePage )
          * sizeof(tsp81_UCS2Char);
        if (*srcBytesParsed > (*destBytesWritten * sizeof(tsp81_UCS2Char))) {
          result = sp78_SourceCorrupted;
        }
      }
      else
        result = sp78_SourceCorrupted;
    }
  }
  else
  {
    tsp83UTF8_ConversionResult convResult = sp83UTF8Convert_Success;
    tsp81_UCS2Char const * nextSourceChar;
    tsp00_Byte           * nextDestChar;

    convResult = sp83UTF8ConvertFromUCS2 (
        (const tsp81_UCS2Char *)srcBuffer,
        ((const tsp81_UCS2Char *)srcBuffer) + srcLengthInBytes/sizeof(tsp81_UCS2Char),
        &nextSourceChar,
        srcLSBIndex == LSB_INDEX_SWAPPED_UCS2,
        (tsp00_Byte *)destBuffer,
        ((tsp00_Byte *)destBuffer) + destBufferLengthInBytes,
        &nextDestChar);

    *srcBytesParsed = (nextSourceChar - (tsp81_UCS2Char *)srcBuffer) * sizeof(tsp81_UCS2Char);
    *destBytesWritten = (nextDestChar - (tsp00_Byte *)destBuffer);

    switch(convResult)
    {
    case sp83UTF8Convert_TargetExhausted:
      result = sp78_TargetExhausted;
      break;
    case sp83UTF8Convert_SourceCorrupted:
      result = sp78_SourceCorrupted;
      break;
    case sp83UTF8Convert_SourceExhausted:
      result = sp78_SourceExhausted;
      break;
    default:
      break;
    }
  }

  return result;
}

/*---------------------------------------------------------------------------*/

/* Convert from UTF8 to Ascii */

static tsp78ConversionResult sp78_FromUTF8ToAscii(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten )
{
  tsp78ConversionResult      result     = sp78_Ok;
  tsp83UTF8_ConversionResult convResult = sp83UTF8Convert_Success;
  unsigned int replaceCount;

  convResult = sp83UTF8toASCII( (const tsp00_Byte *)srcBuffer,
                                 srcLengthInBytes,
                                 srcBytesParsed,
                                 (tsp00_Char *)destBuffer,
                                 destBufferLengthInBytes,
                                 '?',
                                 &replaceCount,
                                 destBytesWritten );

  if ( convResult == sp83UTF8Convert_TargetExhausted )
  {
    result = sp78_TargetExhausted;
  }
  else if ( convResult == sp83UTF8Convert_SourceCorrupted )
  {
    result = sp78_SourceCorrupted;
  }

  return result;
}

/*---------------------------------------------------------------------------*/

/* Convert from UTF8 to UCS2 */

static tsp78ConversionResult sp78_FromUTF8ToUCS2(
 const void    *srcBuffer,
 tsp00_Uint4    srcLengthInBytes,
 tsp00_Uint4   *srcBytesParsed,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten,
 int            destLSBIndex )
{
  tsp78ConversionResult result = sp78_Ok;
  tsp83UTF8_ConversionResult convResult = sp83UTF8Convert_Success;
  tsp00_Uint4 targetLengthInChars = destBufferLengthInBytes/sizeof(tsp81_UCS2Char);
  tsp00_Byte const * nextSourceChar;
  tsp81_UCS2Char * nextDestChar;

  convResult = sp83UTF8ConvertToUCS2 (
        (const tsp00_Byte *)srcBuffer,
        ((tsp00_Byte *)srcBuffer)+srcLengthInBytes,
        &nextSourceChar,
        (tsp81_UCS2Char *)destBuffer,
        ((tsp81_UCS2Char *)destBuffer) + targetLengthInChars,
        &nextDestChar,
        destLSBIndex == LSB_INDEX_SWAPPED_UCS2 );

  *srcBytesParsed = (nextSourceChar - (tsp00_Byte *)srcBuffer);
  *destBytesWritten = (nextDestChar - (tsp81_UCS2Char *)destBuffer) * sizeof(tsp81_UCS2Char);

  switch(convResult)
  {
  case sp83UTF8Convert_TargetExhausted:
    result = sp78_TargetExhausted;
    break;
  case sp83UTF8Convert_SourceCorrupted:
    result = sp78_SourceCorrupted;
    break;
  case sp83UTF8Convert_SourceExhausted:
    result = sp78_SourceExhausted;
    break;
  default:
    break;
  }

  return result;
}

/*---------------------------------------------------------------------------*/

/*
  Function: sp78_AddTerminator
  Description: Add zero termination to destBuffer if needed

  Check on terminator not already added. If not try to add and return final result.

  Arguments: oldResult [in] result so far
             destEncoding [in] destination encoding
             destBuffer [inout] destination buffer
             destBufferLengthInBytes [in] maximum length of destination
             destBytesWritten [inout] number of bytes written (may be modified)
  Return value: final result code
 */
static tsp78ConversionResult sp78_AddTerminator(
 tsp78ConversionResult oldResult,
 const tsp77encoding *destEncoding,
 void *         destBuffer,
 tsp00_Uint4    destBufferLengthInBytes,
 tsp00_Uint4   *destBytesWritten )
{
  tsp78ConversionResult result = oldResult;

  /* terminatorSize must be identical to zero terminator size!!! */
  /* search step for UTF8 is identical to terminator size... */
  tsp00_Uint4 terminatorSize = destEncoding->terminatorSize;
  tsp00_Uint4 destByteIndex;
  tsp00_Uint4 terminatorOffset = 0;
  tsp00_Char *destPointer = (tsp00_Char *)destBuffer;

  if ( *destBytesWritten > terminatorSize )
  {
    destByteIndex = (*destBytesWritten) - terminatorSize;
    for ( terminatorOffset = 0; terminatorOffset < terminatorSize; ++terminatorOffset )
    {
      if ( destPointer[destByteIndex+terminatorOffset] != 0 )
      {
        break;
      }
    }
  }

  if ( terminatorOffset < terminatorSize )
  {
    /* we must add zero termination */
    destByteIndex = *destBytesWritten;

    if ( (destByteIndex + terminatorSize) > destBufferLengthInBytes )
    {
      result = sp78_TargetNotTerminated;
    }
    else
    {
      for ( terminatorOffset = 0; terminatorOffset < terminatorSize; ++terminatorOffset )
      {
        destPointer[destByteIndex+terminatorOffset] = 0;
      }
      *destBytesWritten += terminatorSize;
    }
  }

  return result;
}


/*---------------------------------------------------------------------------*/

const char * sp78errString (tsp78ConversionResult convResult)
{
    static const char * strings [] = {
        "Ok",
        "SourceExhausted",
        "SourceCorrupted",
        "TargetExhausted",
        "TargetNotTerminated",
        "BadSourceEncoding",
        "BadTargetEncoding",
        "NullArgument"
    };
    return strings [convResult];
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
