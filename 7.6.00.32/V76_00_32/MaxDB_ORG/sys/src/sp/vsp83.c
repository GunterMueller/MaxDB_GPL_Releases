/*!**************************************************************************

  module       : vsp83.c

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Unicode support
  description : UTF8 Conversion

  Pure C version of UTF8 conversion

  last changed: 2000-09-22  9:31
  first created:2000-08-28  17:21

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

#include "hsp83.h"
#include <stdlib.h> /* for malloc */
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/* constants in ucs-4 domain */
#define SP83_UCS4_REPLACEMENTCHAR      0x0000FFFDU
#define SP83_MAX_UCS2                  0x0000FFFFU
#define SP83_MAX_UTF16                 0x0010FFFFU
#define SP83_MAX_UCS4                  0x7FFFFFFFU

#define SP83_UCS4_HALFSHIFT            10
#define SP83_UCS4_HALFBASE             0x00010000U

/* constants in utf-16 domain */
#define SP83_UCS4_HALFMASK             0x000003FFU

#define SP83_UTF16_SRGT_HIGH_START     0x0000D800U
#define SP83_UTF16_SRGT_HIGH_END       0x0000DBFFU
#define SP83_UTF16_SRGT_LOW_START      0x0000DC00U
#define SP83_UTF16_SRGT_LOW_END	       0x0000DFFFU

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  Local variables (IMPLEMENTATION)                                         *
 *===========================================================================*/

static unsigned int sp83_UTF8ElementSize[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6 };

static tsp00_Byte sp83_LeadingByteMark[7] = 
    {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

static tsp00_Uint4 sp83_UTF8Offsets[7] =
    {0U, 0x00000000U, 0x00003080U, 0x000E2080U, 
         0x03C82080U, 0xFA082080U, 0x82082080U };

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

tsp83UTF8_ConversionResult sp83UTF8ConvertFromUCS2(
  const tsp81_UCS2Char *  srcBeg,
  const tsp81_UCS2Char *  srcEnd,
        tsp81_UCS2Char const ** srcAt,
  int                     srcSwapped,
  tsp00_Byte           *  destBeg,
  tsp00_Byte           *  destEnd,
  tsp00_Byte           ** destAt
)
{
  tsp83UTF8_ConversionResult   result                = sp83UTF8Convert_Success;
  register const tsp81_UCS2Char * srcIter  = srcBeg;
  register tsp00_Byte           * destIter = destBeg;
  register tsp00_Uint4            ucs4char;
  register tsp00_Uint4            utf16elem2;     /* must be ucs4! */
  register unsigned int           bytesToWrite;
  int firstIndex = ( srcSwapped ? 0 : 1 );

    /* loop whole utf-16 string */
    while (srcIter < srcEnd) {

        /* -----------------------------------------------------
           convert next utf-16 character to ucs-4:
           ----------------------------------------------------- */
        ucs4char = srcIter->c[firstIndex] + (srcIter->c[1-firstIndex]<<8);
        ++srcIter;

        if (ucs4char >= SP83_UTF16_SRGT_HIGH_START && 
            ucs4char <= SP83_UTF16_SRGT_HIGH_END)
        {
            if (srcIter == srcEnd)
            {
                /* error: 2nd utf-16 character missing in source */
                result = sp83UTF8Convert_SourceExhausted;
                --srcIter;
                break;
            }

            utf16elem2 = srcIter->c[firstIndex] + (srcIter->c[1-firstIndex]<<8);
            if (utf16elem2 >= SP83_UTF16_SRGT_LOW_START && 
                utf16elem2 <= SP83_UTF16_SRGT_LOW_END) 
            {
                ucs4char = ((ucs4char - SP83_UTF16_SRGT_HIGH_START) << SP83_UCS4_HALFSHIFT) 
                         +
    	                   utf16elem2 - SP83_UTF16_SRGT_HIGH_END 
                         +
                           SP83_UCS4_HALFBASE;
                ++srcIter;
            }
        }

        /* -----------------------------------------------------
           determine byte size of next utf-8 character
           ----------------------------------------------------- */
             if (ucs4char <  0x80)           { bytesToWrite = 1; } 
        else if (ucs4char <  0x800)          { bytesToWrite = 2; } 
        else if (ucs4char <  0x10000)        { bytesToWrite = 3; } 
        else if (ucs4char <  0x200000)       { bytesToWrite = 4; } 
        else if (ucs4char <  0x4000000)      { bytesToWrite = 5; } 
        else if (ucs4char <= SP83_MAX_UCS4)  { bytesToWrite = 6; } 
        else
            { bytesToWrite = 2; 
              ucs4char     = SP83_UCS4_REPLACEMENTCHAR; }

        /* -----------------------------------------------------
           check if dest buffer is exhausted
           ----------------------------------------------------- */
        if (destIter + bytesToWrite > destEnd) 
        {
            result = sp83UTF8Convert_TargetExhausted;
            --srcIter;            
            break;
        }

        destIter += bytesToWrite;
        /* -----------------------------------------------------
           write utf-8 bytes to destination
           ----------------------------------------------------- */
        switch (bytesToWrite) 
        {	/* note: code falls through cases! */
            case 6:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 5:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 4:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 3:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 2:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 1:	*--destIter =  ucs4char | sp83_LeadingByteMark[bytesToWrite];
        }
        destIter += bytesToWrite;
    }

    *srcAt  = srcIter;
    *destAt = destIter;

    return result;
}

/*---------------------------------------------------------------------------*/

tsp83UTF8_ConversionResult sp83UTF8ConvertToUCS2(
  const tsp00_Byte       *  srcBeg,
  const tsp00_Byte       *  srcEnd,
        tsp00_Byte const ** srcAt,
  tsp81_UCS2Char         *  destBeg,
  tsp81_UCS2Char         *  destEnd,
  tsp81_UCS2Char         ** destAt,
  int                       destSwapped
)
{
  tsp83UTF8_ConversionResult   result       = sp83UTF8Convert_Success;
  register const tsp00_Byte * srcIter      = srcBeg;
  register tsp81_UCS2Char   * destIter     = destBeg;
  register tsp00_Uint4        ucs4char;
  register unsigned int       bytesToWrite;
  int                         firstIndex   = ( destSwapped ? 0 : 1 );

  /* loop whole utf-8 string */
  while (srcIter < srcEnd)
  {
    bytesToWrite = sp83_UTF8ElementSize[*srcIter];

    if (bytesToWrite == 0)
    {
        /* error: utf-8 string corrupted */
        result = sp83UTF8Convert_SourceCorrupted;
        break;
    }

    if (srcIter + bytesToWrite > srcEnd) 
    {
        /* error: utf-8 bytes missing in source */
        result = sp83UTF8Convert_SourceExhausted; 
        break;
    }

    ucs4char    = 0;

    switch(bytesToWrite) 
    {	/* note: code falls through cases! */
    case 6:	ucs4char += *srcIter++; ucs4char <<= 6;
    case 5:	ucs4char += *srcIter++; ucs4char <<= 6;
    case 4:	ucs4char += *srcIter++; ucs4char <<= 6;
    case 3:	ucs4char += *srcIter++; ucs4char <<= 6;
    case 2:	ucs4char += *srcIter++; ucs4char <<= 6;
    case 1:	ucs4char += *srcIter++; break;
    }

    ucs4char -= sp83_UTF8Offsets[bytesToWrite];

    if (ucs4char > SP83_MAX_UTF16)
    {
      ucs4char = SP83_UCS4_REPLACEMENTCHAR;
    }

    if (ucs4char <= SP83_MAX_UCS2) 
    {
      if (destIter >= destEnd) 
      {
          /* dest buffer exhausted */
          srcIter -= bytesToWrite;
          result = sp83UTF8Convert_TargetExhausted; 
          break;
      }
      destIter->c[firstIndex] = (tsp00_Byte)(ucs4char & 0xFF);
      destIter->c[1-firstIndex] = (tsp00_Byte)((ucs4char>>8) & 0xFF);
      ++destIter;
    } 
    else 
    {
      tsp00_Uint4 copyUTF16;

      if (destIter + 1 >= destEnd) 
      {
          /* dest buffer exhausted */
          srcIter -= bytesToWrite;
          result = sp83UTF8Convert_TargetExhausted; 
          break;
      }
      ucs4char  -= SP83_UCS4_HALFBASE;

      copyUTF16  = (ucs4char >> SP83_UCS4_HALFSHIFT) 
                  + SP83_UTF16_SRGT_HIGH_START;

      destIter->c[firstIndex] = (tsp00_Byte)(copyUTF16 & 0xFF);
      destIter->c[1-firstIndex] = (tsp00_Byte)((copyUTF16>>8) & 0xFF);
      ++destIter;

      copyUTF16   = (ucs4char & SP83_UCS4_HALFMASK) 
                   + SP83_UTF16_SRGT_LOW_START;

      destIter->c[firstIndex] = (tsp00_Byte)(copyUTF16 & 0xFF);
      destIter->c[1-firstIndex] = (tsp00_Byte)((copyUTF16>>8) & 0xFF);
      ++destIter;
    }
  }

  *srcAt  = srcIter;
  *destAt = destIter;

  return result;
}

/*---------------------------------------------------------------------------*/

tsp83UTF8_ConversionResult sp83UTF8_Bytelen(const tsp00_Byte * srcBeg,
                                           const unsigned int srcLimit,
                                           unsigned int *charCountUTF8,
                                           unsigned int *byteCountUTF8)
{
  tsp83UTF8_ConversionResult  result                = sp83UTF8Convert_Success;
  register const tsp00_Byte * srcIter               = srcBeg;
    /* PTS 1110410 */
  unsigned int                utf8CharactersFound   = 0;
  unsigned int                utf8BytesLeftToParse  = srcLimit;

  /* loop whole utf-8 string: */
  while ( utf8BytesLeftToParse > 0 )
  {
    register unsigned int     bytesToWrite;
    register unsigned int     checkOffset;

    if ( *srcIter == 0 )
    {
      break;
    }

    bytesToWrite = sp83_UTF8ElementSize[*srcIter];

    if ( bytesToWrite == 0 )
    {
      /* error: utf-8 string corrupted */
      result = sp83UTF8Convert_SourceCorrupted;
      break;
    }

    if (bytesToWrite > utf8BytesLeftToParse )
    {
      /* error: last utf-8 character incomplete */
      result = sp83UTF8Convert_SourceExhausted;
      break;
    }

    for ( checkOffset = 1; checkOffset < bytesToWrite; ++checkOffset )
    {
      /* all follow up bytes must have bit 8 SET and bit 7 NOT SET */
      if ( (srcIter[checkOffset] & 0xC0) != 0x80 )
      {
        result = sp83UTF8Convert_SourceCorrupted;
        break;
      }
    }

    if ( checkOffset < bytesToWrite )
    {
      break;
    }

    srcIter += bytesToWrite;
    /* PTS 1110410 */
    utf8BytesLeftToParse -= bytesToWrite;
    ++utf8CharactersFound;
  }

  *charCountUTF8 = utf8CharactersFound;
  *byteCountUTF8 = ( srcIter - srcBeg );

  return result;
}

/*---------------------------------------------------------------------------*/

tsp83UTF8_ConversionResult sp83UTF8fromASCII(const tsp00_Char   * srcBeg,
                                            const unsigned int   srcLimit,
                                             unsigned int *srcBytesParsed,
                                                  tsp00_Byte   * destBeg,
                                            const unsigned int   destLimit,
                                                  unsigned int * byteCountUTF8)
{
  tsp83UTF8_ConversionResult   result                 = sp83UTF8Convert_Success;
  register const tsp00_Char * srcIter                = srcBeg;
  register tsp00_Byte       * destIter               = destBeg;
  register tsp00_Byte       * destEnd                = destBeg + destLimit;
  unsigned int                asciiCharsLeftToParse  = srcLimit;

  /* loop whole ascii string */
  while ( asciiCharsLeftToParse-- > 0 )
  {
    register tsp00_Byte       asciiChar;
    /* -----------------------------------------------------
       check if dest buffer is exhausted
       ----------------------------------------------------- */
    if (destIter + 1 > destEnd) 
    {
      result = sp83UTF8Convert_TargetExhausted;
      break;
    }

    /* -----------------------------------------------------
       get next ASCII char leading byte
       ----------------------------------------------------- */
    asciiChar = *srcIter++;

    if ( (asciiChar & 0x80) == 0 )
    {
      *destIter++ = asciiChar;
    }
    else
    {
      /* -----------------------------------------------------
         check if dest buffer is exhausted
         ----------------------------------------------------- */
      if (destIter + 2 > destEnd) 
      {
        result = sp83UTF8Convert_TargetExhausted;
        break;
      }

      /* -----------------------------------------------------
         split ascii character into two UTF8 Characters
         ----------------------------------------------------- */
      destIter[1] = 0x80 | ( asciiChar & 0x3F );
      destIter[0] = sp83_LeadingByteMark[2] | ( ( asciiChar&0xFF ) >> 6 ) ;
      destIter += 2;
    }
  }

  *byteCountUTF8 = ( destIter - destBeg );
  *srcBytesParsed = srcLimit - asciiCharsLeftToParse-1;
  return result;
}

/*---------------------------------------------------------------------------*/

tsp83UTF8_ConversionResult sp83UTF8toASCII(const tsp00_Byte * srcBeg,
                                          const unsigned int srcLimit,
                                           unsigned int *srcBytesParsed,
                                           tsp00_Char * destBeg,
                                          const unsigned int destLimit,
                                          const tsp00_Char replaceChar,
                                          unsigned int *replaceCount,
                                          unsigned int *byteCountASCII)
{
  tsp83UTF8_ConversionResult  result                = sp83UTF8Convert_Success;
  register const tsp00_Byte * srcIter               = srcBeg;
  register tsp00_Char       * destIter              = destBeg;
  register tsp00_Char       * destEnd               = destBeg + destLimit;
  signed long                utf8BytesLeftToParse  = srcLimit;

  /* Reset count of replaced (nonASCII UTF8 Characters) */
  *replaceCount = 0;

  /* loop whole utf-8 string: */
  while ( utf8BytesLeftToParse > 0 )
  {
    register unsigned int     bytesToRead;
    register tsp00_Byte       utf8char;
    /* -----------------------------------------------------
       check if dest buffer is exhausted:
       ----------------------------------------------------- */
    if (destIter + 1 > destEnd) 
    {
      result = sp83UTF8Convert_TargetExhausted;
      break;
    }

    bytesToRead = sp83_UTF8ElementSize[*srcIter];

    if ( bytesToRead == 0 )
    {
      /* error: utf-8 string corrupted */
      result = sp83UTF8Convert_SourceCorrupted;
      break;
    }

    if (bytesToRead == 1)
    {
      *destIter++ = *srcIter;
    }
    else
    {
      /* -----------------------------------------------------
         get next UTF8 char:
         ----------------------------------------------------- */
      utf8char = *srcIter;

      if ( utf8char > 0xC3 ) /* Ascii limitation !!! */
      {
        if ( replaceChar != 0 )
        {
          *destIter++ = replaceChar;
          ++*replaceCount;
        }
        else
        {
          result = sp83UTF8Convert_SourceCorrupted;
          break;
        }
      }
      else
      {
        if ( utf8char == sp83_LeadingByteMark[bytesToRead] )
        {
          /* 2 byte sequence for single sequence character ... */
          result = sp83UTF8Convert_SourceCorrupted;
          break;
        }

        if ( ( srcIter[1] & sp83_LeadingByteMark[bytesToRead] ) != 0x80 )
        {
          /* indicate correct error position */
          ++srcIter;
          result = sp83UTF8Convert_SourceCorrupted;
          break;
        }

        *destIter++ = ((utf8char & 0x3) << 6) | (srcIter[1] & 0x3F) ;
      }
    }

    srcIter += bytesToRead;
    utf8BytesLeftToParse -= bytesToRead;
  }

  *byteCountASCII = ( destIter - destBeg );
  *srcBytesParsed = srcLimit - utf8BytesLeftToParse;
  return result;
}

/*---------------------------------------------------------------------------*/

int sp83UTF8StringInfo( const void *buffer, 
                        unsigned int bufferLength, 
                        int bufferLengthIsInBytes, 
                        unsigned int *pCharCount, 
                        unsigned int *pByteCount, 
                        int *pIsTerminated, 
                        int *pIsCorrupted, 
                        int *pIsExhausted)
{
  tsp00_Byte *ptr = (tsp00_Byte *)buffer;
  tsp00_Byte firstByteOfChar;
  unsigned int byteIndex;
  unsigned int charIndex;
  unsigned int byteSize;
  unsigned int checkOffset;

  *pIsExhausted = *pIsTerminated = *pIsCorrupted = false;

  for ( charIndex = byteIndex = 0; ; ++charIndex, byteIndex += byteSize )
  {
    if ( bufferLengthIsInBytes )
    {
	if ( byteIndex >= bufferLength )
		break;
    }
    else
    {
	if ( charIndex >= bufferLength )
		break;
    }

    firstByteOfChar = ptr[byteIndex];
    
    if ( firstByteOfChar == 0 )
    {
      *pIsTerminated = true;
      break;
    }

    byteSize = sp83_UTF8ElementSize[firstByteOfChar];

    if ( byteSize == 0 )
    {
      *pIsCorrupted = true;
      break;
    }

    if ( bufferLengthIsInBytes )
    {
      if ( (byteIndex + byteSize) > bufferLength )
      {
        *pIsExhausted = true;
        break;
      }
    }

    for ( checkOffset = 1; checkOffset < byteSize; ++checkOffset )
    {
      /* all follow up bytes must have bit 8 SET and bit 7 NOT SET */
      if ( (ptr[byteIndex + checkOffset] & 0xC0) != 0x80 )
      {
        *pIsCorrupted = true;
        break;
      }
    }

    if ( checkOffset < byteSize )
    {
      break;
    }
  }

  *pCharCount = charIndex;
  *pByteCount = byteIndex;

  return ((*pIsExhausted) || (*pIsCorrupted));
}

/*---------------------------------------------------------------------------*/

int sp83UTF8StringComplete( const void *buffer,
                            unsigned int bufferLengthInBytes,
                            unsigned int *completeLengthInBytes )
{
  tsp00_Byte bytesToStrip = 0;

  if ( bytesToStrip < bufferLengthInBytes )
  {
    const tsp00_Byte *ptr = ((const tsp00_Byte *)buffer) + bufferLengthInBytes;
    tsp00_Byte byteToCheck;

    byteToCheck = *--ptr;

    /* If 7bit Ascii at end of buffer check is not needed... */
    if ( (byteToCheck & 0x80) == 0x80 )
    {
      bytesToStrip = 1;

      /* Search until leadin character was found */
      /* or beginning of buffer reached */
      while ( (byteToCheck & 0xC0) != 0xC0 
           && (ptr > (const tsp00_Byte *)buffer) )
      {
        byteToCheck = *--ptr;
        ++bytesToStrip;
      }

      /* If complete sequence was found substract it from */
      /* the number of bytes to strip */
      if ( ( (byteToCheck & 0xC0) == 0xC0 )
        && ( sp83_UTF8ElementSize[byteToCheck] <= bytesToStrip ) )
      {
        bytesToStrip -= sp83_UTF8ElementSize[byteToCheck];
      }
    }
  }

  *completeLengthInBytes = bufferLengthInBytes - bytesToStrip;

  return ( *completeLengthInBytes < bufferLengthInBytes );
}

/*---------------------------------------------------------------------------*/

tsp83UTF8_ConversionResult sp83UTF8StringToUpper( tsp00_Byte *srcBeg, 
                                                  tsp00_Byte **srcAt,
                                                  unsigned int srcLimit )
{
  tsp83UTF8_ConversionResult  result       = sp83UTF8Convert_Success;
  register tsp00_Byte       * srcEnd = srcBeg + srcLimit;
  register tsp00_Byte       * srcIter      = srcBeg;
  register tsp00_Uint4        ucs4char;
  register tsp00_Uint4        uppercaseChar;
  register unsigned int       bytesToWrite;

  /* loop whole utf-8 string */
  while (srcIter < srcEnd)
  {
    bytesToWrite = sp83_UTF8ElementSize[*srcIter];

    if (bytesToWrite == 0)
    {
        /* error: utf-8 string corrupted */
        result = sp83UTF8Convert_SourceCorrupted;
        break;
    }

    if (srcIter + bytesToWrite > srcEnd) 
    {
        /* error: utf-8 bytes missing in source */
        result = sp83UTF8Convert_SourceExhausted; 
        break;
    }

    ucs4char    = 0;

    switch(bytesToWrite) 
    {	/* note: code falls through cases! */
      case 6:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 5:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 4:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 3:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 2:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 1:	ucs4char += *srcIter++; break;
      default:
        break;
    }

    ucs4char -= sp83_UTF8Offsets[bytesToWrite];

    if (ucs4char > SP83_MAX_UTF16)
    {
      ucs4char = SP83_UCS4_REPLACEMENTCHAR;
    }

    if (ucs4char <= SP83_MAX_UCS2) 
    {
      uppercaseChar = sp81UCS2ToUpper((tsp00_Uint2)ucs4char);
      if ( uppercaseChar != ucs4char )
      {
        switch (bytesToWrite) 
        {	/* note: code falls through cases! */
          case 6:	*--srcIter = (uppercaseChar | 0x80) & 0xBF; uppercaseChar >>= 6;
          case 5:	*--srcIter = (uppercaseChar | 0x80) & 0xBF; uppercaseChar >>= 6;
          case 4:	*--srcIter = (uppercaseChar | 0x80) & 0xBF; uppercaseChar >>= 6;
          case 3:	*--srcIter = (uppercaseChar | 0x80) & 0xBF; uppercaseChar >>= 6;
          case 2:	*--srcIter = (uppercaseChar | 0x80) & 0xBF; uppercaseChar >>= 6;
          case 1:	*--srcIter =  uppercaseChar | sp83_LeadingByteMark[bytesToWrite];
          default:
            break;
        }
        srcIter += bytesToWrite;
      }
    }
  }

  *srcAt  = srcIter;

  return result;
}

/*---------------------------------------------------------------------------*/

tsp83UTF8_ConversionResult sp83UTF8StringToLower( tsp00_Byte *srcBeg, 
                                                  tsp00_Byte **srcAt,
                                                  unsigned int srcLimit )
{
  tsp83UTF8_ConversionResult  result       = sp83UTF8Convert_Success;
  register tsp00_Byte       * srcEnd = srcBeg + srcLimit;
  register tsp00_Byte       * srcIter      = srcBeg;
  register tsp00_Uint4        ucs4char;
  register tsp00_Uint4        lowercaseChar;
  register unsigned int       bytesToWrite;

  /* loop whole utf-8 string */
  while (srcIter < srcEnd)
  {
    bytesToWrite = sp83_UTF8ElementSize[*srcIter];

    if (bytesToWrite == 0)
    {
        /* error: utf-8 string corrupted */
        result = sp83UTF8Convert_SourceCorrupted;
        break;
    }

    if (srcIter + bytesToWrite > srcEnd) 
    {
        /* error: utf-8 bytes missing in source */
        result = sp83UTF8Convert_SourceExhausted; 
        break;
    }

    ucs4char    = 0;

    switch(bytesToWrite) 
    {	/* note: code falls through cases! */
      case 6:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 5:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 4:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 3:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 2:	ucs4char += *srcIter++; ucs4char <<= 6;
      case 1:	ucs4char += *srcIter++; break;
      default:
        break;
    }

    ucs4char -= sp83_UTF8Offsets[bytesToWrite];

    if (ucs4char > SP83_MAX_UTF16)
    {
      ucs4char = SP83_UCS4_REPLACEMENTCHAR;
    }

    if (ucs4char <= SP83_MAX_UCS2) 
    {
      lowercaseChar = sp81UCS2ToLower((tsp00_Uint2)ucs4char);
      if ( lowercaseChar != ucs4char )
      {
        switch (bytesToWrite) 
        {	/* note: code falls through cases! */
          case 6:	*--srcIter = (lowercaseChar | 0x80) & 0xBF; lowercaseChar >>= 6;
          case 5:	*--srcIter = (lowercaseChar | 0x80) & 0xBF; lowercaseChar >>= 6;
          case 4:	*--srcIter = (lowercaseChar | 0x80) & 0xBF; lowercaseChar >>= 6;
          case 3:	*--srcIter = (lowercaseChar | 0x80) & 0xBF; lowercaseChar >>= 6;
          case 2:	*--srcIter = (lowercaseChar | 0x80) & 0xBF; lowercaseChar >>= 6;
          case 1:	*--srcIter =  lowercaseChar | sp83_LeadingByteMark[bytesToWrite];
          default:
            break;
        }
        srcIter += bytesToWrite;
      }
    }
  }

  *srcAt  = srcIter;

  return result;
}

/*---------------------------------------------------------------------------*/
/* PTS 1109583 */
int sp83UTF8ArrayFromUCS2Array (
 int               myArgc, 
 tsp81_UCS2Char ** UCS2argv,
 char          *** pReturnedUTF8argv )
{
  char **UTF8argv;
  int    argcIndex;
  
  *pReturnedUTF8argv = (char **)0;

  UTF8argv = (char **)malloc((myArgc + 1) * sizeof(char *));
  for ( argcIndex = 0; argcIndex < myArgc; ++argcIndex )
  {
    tsp00_Uint4 UCS2len = sp81UCS2strlen(UCS2argv[argcIndex]);
    tsp81_UCS2Char const * srcAt;
    tsp00_Byte * destAt;
    short swapTest = 1;

    /* Each UCS2 argument is now converted into an UTF8 arguments */
    /* 6 bytes per character is absolut maximum... */
    UTF8argv[argcIndex] = (char *)malloc(UCS2len * 6 + 1);

    if ( UTF8argv[argcIndex] == NULL )
    {
      return false;
    }

    /* Convert into UTF8 */
    memset(UTF8argv[argcIndex], 0, UCS2len * 6 + 1);
    if ( sp83UTF8ConvertFromUCS2(UCS2argv[argcIndex], 
                                 UCS2argv[argcIndex]+UCS2len,
                                 &srcAt,
                                 (*(char *)&swapTest) == 1,
                                 (tsp00_Byte *)(UTF8argv[argcIndex]),
                                 (tsp00_Byte *)(UTF8argv[argcIndex]+(UCS2len * 6)),
                                 &destAt)
      != sp83UTF8Convert_Success )
    {
      return false;
    }
  }
  UTF8argv[myArgc] = NULL;

  *pReturnedUTF8argv = UTF8argv;
  return true;
}
/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
