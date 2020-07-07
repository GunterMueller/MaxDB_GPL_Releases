/*!***************************************************************************

  module:        vsp77.c

  -------------------------------------------------------------------------

  responsible:   DanielD, JoergM

  special area:  sprintf functions interface

  description:   Replacement for ANSI C sprintf and vsprintf functions

  Original Author: Daniel Dittmar
  last changed: 2000-12-07  15:44

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html
  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG-2000




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "SAPDBCommon/SAPDB_string.h"

#ifdef FREEBSD
#define _GNU_SOURCE
#include <float.h>
#endif

#if defined(LINUX) || defined(FREEBSD)
#define __USE_ISOC9X 1
#define __USE_ISOC99 1
#endif /* LINUX, FREEBSD */
#include <math.h>
#include <stdarg.h>

#include "hsp77.h"
#include "hsp83.h"
#include "hsp78_0.h"

#define BETWEEN_MSP77(low, val, up) ((low <= val) && (val <= up))
#define MIN_MSP77(a,b)              ((a) < (b) ? (a) : (b))
#undef ROUTINE_DBG_MSP00
#define ROUTINE_DBG_MSP00(routine)

#ifdef TEST
#include <stdio.h>
FILE           *stdTrace;
#endif

typedef int cbool;
#define TRUE  1
#define FALSE 0
#define UNDEF (-1)

typedef unsigned int t_len;

/*
 * t_sp77printfFormat structure
 *
 * Contains all format specific items for a single argument.
 * Except 'encoding' all members of this structure are reset
 * while parsing each new format character specification.
 */
typedef struct t_sp77printfFormat {
    t_len    width;     /* parsed width value or '-1' if not given */
    t_len    precision; /* parsed precision value or '-1' if not given */
    int      argSize;   /* current SIZE or '-1' if not given */
    int      argChar;   /* current TYPE value */
    struct {
      cbool justifyLeft;   /* '-' modifier bool */
      cbool sign;          /* '+' modifier bool */
      cbool space;         /* ' ' modifier bool */
      cbool alternate;     /* '#' modifier bool */
      cbool zeros;         /* '0' modifier bool */
      cbool byteScale;     /* ''' modifier bool */
      const tsp77encoding *inputEncoding; /* encoding argument value */
    } flags;

    /*  output encoding: Set only once for all arguments! */
    const tsp77encoding * encoding;
} tsp77_printfFormat;

/*---------------------------------------------------------------------------*/

teo00_Bool sp77charIsEqual (
    const tsp77charConst * charConst,
    const void           * buf,
    int                    bufLen)
{
    teo00_Bool result;

    if (bufLen < charConst->byteCount) {
        result = FALSE;
    }
    else if (memcmp (buf, charConst->bytes, charConst->byteCount) == 0) {
        result = TRUE;
    }
    else {
        result = FALSE;
    }
    return result;
}

/*---------------------------------------------------------------------------*/

int sp77charMove (
    void * buf,
    int    bufLen,
    const tsp77charConst * charConst)
{
    if (charConst->byteCount == 0) {
        return 0;
    }
    if (bufLen < charConst->byteCount) {
        return 0;
    }
    SAPDB_memcpy (buf, charConst->bytes, charConst->byteCount);
    return charConst->byteCount;
}


/*---------------------------------------------------------------------------*/
/* stringInfo Encoding member routines */

/*---------------------------------------------------------------------------*/
static cbool sp77_UCS4OneByte (const tsp81_UCS4Char * testChar)
{
    return testChar->c [0] == '\0' && testChar->c [1] == '\0' && testChar->c [2] == '\0';
}

/*---------------------------------------------------------------------------*/
static cbool sp77_UCS4SwappedOneByte (const tsp81_UCS4Char * testChar)
{
    return testChar->c [1] == '\0' && testChar->c [2] == '\0' && testChar->c [3] == '\0';
}


static cbool sp77_AsciiStringInfo(const void *buffer,
                            t_len maxBufferLength,
                            cbool bufferLengthIsInBytes,
                            t_len *pCharCount,
                            t_len *pByteCount,
                            cbool *pIsTerminated,
                            cbool *pIsCorrupted,
                            cbool *pIsExhausted);

/*---------------------------------------------------------------------------*/
/* stringComplete Encoding member routines */
static cbool sp77_AsciiStringComplete(const void *buffer,
                            t_len bufferLengthInBytes,
                            t_len *completeLengthInBytes)
{
  /* ASCII always complete... */
  *completeLengthInBytes = bufferLengthInBytes;
  return ( *completeLengthInBytes < bufferLengthInBytes );
}
/*---------------------------------------------------------------------------*/
static cbool sp77_UCS2StringComplete(const void *buffer,
                            t_len bufferLengthInBytes,
                            t_len *completeLengthInBytes)
{
  /* UCS2 incomplete if uneven number of bytes in buffer */
  *completeLengthInBytes = (bufferLengthInBytes & ~1);
  return ( *completeLengthInBytes < bufferLengthInBytes );
}
/*---------------------------------------------------------------------------*/
static cbool sp77_UCS4StringComplete(const void *buffer,
                            t_len bufferLengthInBytes,
                            t_len *completeLengthInBytes)
{
  /* UCS4 incomplete if bufferLengthInBytes not dividable by 4 */
  int mod = bufferLengthInBytes % 4;
  *completeLengthInBytes = bufferLengthInBytes - mod;
  return ( mod != 0 );
}

/*---------------------------------------------------------------------------*/
/* fillString Encoding member routines */
static void sp77_AsciiFillString(void **buffer,
                            t_len *buflen,
                            t_len  padCount,
                            tsp00_Char padChar)
{
  t_len fillCount = padCount;
  if ( fillCount > *buflen ) fillCount = *buflen;

  if ( fillCount > 0 )
  {
    SAPDB_memset(*buffer, padChar, fillCount);
    *buffer = (void *)(((char *)*buffer)+fillCount);
    *buflen -= fillCount;
  }
}
/*---------------------------------------------------------------------------*/
static void sp77_UCS2FillString( void **buffer, t_len *buflen,
                            t_len  padCount, tsp00_Char padChar)
{
  sp81UCS2FillString( buffer, buflen, padCount, padChar, false);
}
/*---------------------------------------------------------------------------*/
static void sp77_UCS2SwappedFillString( void **buffer, t_len *buflen,
                            t_len  padCount, tsp00_Char padChar)
{
  sp81UCS2FillString( buffer, buflen, padCount, padChar, true);
}
/*---------------------------------------------------------------------------*/
static void sp77_UCS4FillString( void **buffer, t_len *buflen,
                            t_len  padCount, tsp00_Char padChar)
{
  sp81UCS4FillString( buffer, buflen, padCount, padChar, false);
}
/*---------------------------------------------------------------------------*/
static void sp77_UCS4SwappedFillString( void **buffer, t_len *buflen,
                            t_len  padCount, tsp00_Char padChar)
{
  sp81UCS4FillString( buffer, buflen, padCount, padChar, true);
}

/*---------------------------------------------------------------------------*/
/* count padding Encoding member routines */
static t_len sp77_AsciiCountPads(void *buffer,
                                 t_len  buflen,
                                 char padChar)
{
  const tsp00_Char *ptr = ((const tsp00_Char *)buffer)+buflen-1;
  const tsp00_Char *endPtr = ptr;

  while ( buflen-- > 0 )
  {
    if ( *ptr != padChar )
      break;
    --ptr;
  }
  return (endPtr - ptr);
}
/*---------------------------------------------------------------------------*/
static t_len sp77_UCS2CountPads( void *buffer,
                                 t_len  buflen,
                                 char padChar)
{
  const tsp81_UCS2Char *ptr = (const tsp81_UCS2Char *)(((const tsp00_Char *)buffer)+buflen-sizeof(tsp81_UCS2Char));
  const tsp81_UCS2Char *endPtr = ptr;

  while ( buflen > 1 )
  {
    buflen -= sizeof(tsp81_UCS2Char);
    if ( ptr->c[0] != 0 || ptr->c[1] != padChar )
      break;
    --ptr;
  }
  return (endPtr - ptr);
}
/*---------------------------------------------------------------------------*/
static t_len sp77_UCS2SwappedCountPads(void *buffer,
                                       t_len  buflen,
                                       char padChar)
{
  const tsp81_UCS2Char *ptr = (const tsp81_UCS2Char *)(((const tsp00_Char *)buffer)+buflen-sizeof(tsp81_UCS2Char));
  const tsp81_UCS2Char *endPtr = ptr;

  while ( buflen > 1 )
  {
    buflen -= sizeof(tsp81_UCS2Char);
    if ( ptr->c[0] != padChar || ptr->c[1] != 0 )
      break;
    --ptr;
  }
  return (endPtr - ptr);
}

/*---------------------------------------------------------------------------*/
static t_len sp77_UCS4CountPads(void *buffer,
                                       t_len  buflen,
                                       char padChar)
{
  const tsp81_UCS4Char *ptr = (const tsp81_UCS4Char *)(((const tsp00_Char *)buffer)+buflen-sizeof(tsp81_UCS4Char));
  const tsp81_UCS4Char *endPtr = ptr;

  while ( buflen > 1 )
  {
    buflen -= sizeof(tsp81_UCS2Char);
    if ( ptr->c[3] != padChar || sp77_UCS4OneByte (ptr) )
      break;
    --ptr;
  }
  return (endPtr - ptr);
}

/*---------------------------------------------------------------------------*/
static t_len sp77_UCS4SwappedCountPads(void *buffer,
                                       t_len  buflen,
                                       char padChar)
{
  const tsp81_UCS4Char *ptr = (const tsp81_UCS4Char *)(((const tsp00_Char *)buffer)+buflen-sizeof(tsp81_UCS4Char));
  const tsp81_UCS4Char *endPtr = ptr;

  while ( buflen > 1 )
  {
    buflen -= sizeof(tsp81_UCS2Char);
    if ( ptr->c[0] != padChar || sp77_UCS4SwappedOneByte (ptr) )
      break;
    --ptr;
  }
  return (endPtr - ptr);
}

/*---------------------------------------------------------------------------*/
/* isAscii7 Encoding member routines */
static int sp77_AsciiIsAscii7(const void *testChar)
{
  return ( ( (*(const tsp00_Byte *)testChar) & ~0x7F ) == 0 );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2IsAscii7( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[0] == 0 && sp77_AsciiIsAscii7(&UCS2testChar->c[1]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2SwappedIsAscii7( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[1] == 0 && sp77_AsciiIsAscii7(&UCS2testChar->c[0]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4IsAscii7( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return (sp77_UCS4OneByte (UCS4testChar) && sp77_AsciiIsAscii7(&UCS4testChar->c[3]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4SwappedIsAscii7( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return (sp77_UCS4SwappedOneByte (UCS4testChar) && sp77_AsciiIsAscii7(&UCS4testChar->c[0]) );
}

/*---------------------------------------------------------------------------*/
/* isBlank Encoding member routines */
static int sp77_AsciiIsBlank(const void *testChar)
{
  const tsp00_Char theChar = *(const tsp00_Char *)testChar;
  return ( theChar == ' ' || theChar == '\t' );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2IsBlank( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[0] == 0 && sp77_AsciiIsBlank(&UCS2testChar->c[1]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2SwappedIsBlank( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[1] == 0 && sp77_AsciiIsBlank(&UCS2testChar->c[0]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4IsBlank( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return ( sp77_UCS4OneByte (UCS4testChar) && sp77_AsciiIsBlank(&UCS4testChar->c[3]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4SwappedIsBlank( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return ( sp77_UCS4SwappedOneByte (UCS4testChar) && sp77_AsciiIsBlank(&UCS4testChar->c[0]) );
}

/*---------------------------------------------------------------------------*/
/* isSpace Encoding member routines */
static int sp77_AsciiIsSpace(const void *testChar)
{
  return ( isspace(*(const tsp00_Uint1 *)testChar) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2IsSpace( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[0] == 0 && sp77_AsciiIsSpace(&UCS2testChar->c[1]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2SwappedIsSpace( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[1] == 0 && sp77_AsciiIsSpace(&UCS2testChar->c[0]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4IsSpace( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return ( sp77_UCS4OneByte (UCS4testChar) && sp77_AsciiIsSpace(&UCS4testChar->c[3]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4SwappedIsSpace( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return ( sp77_UCS4SwappedOneByte (UCS4testChar) && sp77_AsciiIsSpace(&UCS4testChar->c[0]) );
}

/*---------------------------------------------------------------------------*/
/* isControl Encoding member routines */
static int sp77_AsciiIsControl(const void *testChar)
{
  return ( iscntrl(*(const tsp00_Char *)testChar) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2IsControl( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[0] == 0 && sp77_AsciiIsControl(&UCS2testChar->c[1]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS2SwappedIsControl( const void *testChar )
{
  const tsp81_UCS2Char *UCS2testChar = (const tsp81_UCS2Char *)testChar;
  return ( UCS2testChar->c[1] == 0 && sp77_AsciiIsControl(&UCS2testChar->c[0]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4IsControl( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return ( sp77_UCS4OneByte (UCS4testChar) && sp77_AsciiIsControl(&UCS4testChar->c[3]) );
}
/*---------------------------------------------------------------------------*/
static int sp77_UCS4SwappedIsControl( const void *testChar )
{
  const tsp81_UCS4Char *UCS4testChar = (const tsp81_UCS4Char *)testChar;
  return ( sp77_UCS4SwappedOneByte (UCS4testChar) && sp77_AsciiIsControl(&UCS4testChar->c[0]) );
}

/*---------------------------------------------------------------------------*/
/* asciiCopy routines */
static cbool sp77_AsciiAsciiCopy(void **, t_len *, const tsp00_Char *, t_len);
static cbool sp77_UCS2AsciiCopy(void **, t_len *, const tsp00_Char *, t_len);
static cbool sp77_UCS2SwappedAsciiCopy(void **, t_len *, const tsp00_Char *, t_len);
static cbool sp77_UCS4AsciiCopy(void **, t_len *, const tsp00_Char *, t_len);
static cbool sp77_UCS4SwappedAsciiCopy(void **, t_len *, const tsp00_Char *, t_len);
static cbool sp77_UTF8AsciiCopy(void **, t_len *, const tsp00_Char *, t_len);

/* This is not static to allow others to expand encoding classes */
/* static */ const tsp00_Char *sp77EncodingClass = "sp77_Encoding";

static tsp77charTable charTableAscii = {
    {0, {'\0', '\0', '\0', '\0'}}, /* indicator */
    {1, {'\0', '\0', '\0', '\0'}}, /* terminator */
    {1, {'\n', '\0', '\0', '\0'}}, /* nl */
    {1, {'\r', '\0', '\0', '\0'}}, /* cr */
    {1, {' ',  '\0', '\0', '\0'}},  /* blank */
    {1, {'\x1a',  '\0', '\0', '\0'}}  /* dosEOF */
};

static tsp77encoding encodingAscii =
{
  &sp77EncodingClass,
  "encodingAscii",
  csp_ascii,
  sp77_AsciiAsciiCopy,
  sp77_AsciiStringInfo,
  sp77_AsciiStringComplete,
  sp77_AsciiFillString,
  sp77_AsciiCountPads,
  sp77_AsciiIsAscii7,
  sp77_AsciiIsBlank,
  sp77_AsciiIsSpace,
  sp77_AsciiIsControl,
  sizeof(char),
  sizeof(char),
  &charTableAscii
};
const tsp77encoding * const sp77encodingAscii = &encodingAscii;

static tsp77charTable charTableUCS2 = {
    {2, {'\xfe', '\xff', '\0', '\0'}}, /* indicator */
    {2, {'\0', '\0', '\0', '\0'}}, /* terminator */
    {2, {'\0', '\n', '\0', '\0'}}, /* nl */
    {2, {'\0', '\r', '\0', '\0'}}, /* cr */
    {2, {'\0', ' ', '\0', '\0'}},  /* blank */
    {2, {'\0', '\x1a', '\0', '\0',}}  /* dosEOF */
};

static tsp77encoding encodingUCS2 =
{
  &sp77EncodingClass,
  "encodingUCS2",
  csp_unicode,
  sp77_UCS2AsciiCopy,
  sp81UCS2StringInfo,
  sp77_UCS2StringComplete,
  sp77_UCS2FillString,
  sp77_UCS2CountPads,
  sp77_UCS2IsAscii7,
  sp77_UCS2IsBlank,
  sp77_UCS2IsSpace,
  sp77_UCS2IsControl,
  sizeof(tsp81_UCS2Char),
  sizeof(tsp81_UCS2Char),
  &charTableUCS2
};
const tsp77encoding * const sp77encodingUCS2 = &encodingUCS2;

static tsp77charTable charTableUCS2Swapped = {
    {2, {'\xff', '\xfe', '\0', '\0'}}, /* indicator */
    {2, {'\0', '\0', '\0', '\0'}}, /* terminator */
    {2, {'\n', '\0', '\0', '\0'}}, /* nl */
    {2, {'\r', '\0', '\0', '\0'}}, /* cr */
    {2, {' ',  '\0', '\0', '\0'}},  /* blank */
    {2, {'\x1a',  '\0', '\0', '\0'}}  /* dosEOF */
};

static tsp77encoding encodingUCS2Swapped =
{
  &sp77EncodingClass,
  "encodingUCS2Swapped",
  csp_unicode_swap,
  sp77_UCS2SwappedAsciiCopy,
  sp81UCS2StringInfo,
  sp77_UCS2StringComplete,
  sp77_UCS2SwappedFillString,
  sp77_UCS2SwappedCountPads,
  sp77_UCS2SwappedIsAscii7,
  sp77_UCS2SwappedIsBlank,
  sp77_UCS2SwappedIsSpace,
  sp77_UCS2SwappedIsControl,
  sizeof(tsp81_UCS2Char),
  sizeof(tsp81_UCS2Char),
  &charTableUCS2Swapped
};
const tsp77encoding * const sp77encodingUCS2Swapped = &encodingUCS2Swapped;

static tsp77charTable charTableUCS4 = {
    {4, {'\xfe', '\xff', '\0', '\0'}}, /* indicator */
    {4, {'\0', '\0', '\0', '\0'}}, /* terminator */
    {4, {'\0', '\0', '\0', '\n'}}, /* nl */
    {4, {'\0', '\0', '\0', '\r'}}, /* cr */
    {4, {'\0', '\0', '\0', ' '}},  /* blank */
    {4, {'\0', '\0', '\0', '\x1a',}}  /* dosEOF */
};

static tsp77encoding encodingUCS4 =
{
  &sp77EncodingClass,
  "encodingUCS4",
  csp78_ucs4,
  sp77_UCS4AsciiCopy,
  sp81UCS4StringInfo,
  sp77_UCS4StringComplete,
  sp77_UCS4FillString,
  sp77_UCS4CountPads,
  sp77_UCS4IsAscii7,
  sp77_UCS4IsBlank,
  sp77_UCS4IsSpace,
  sp77_UCS4IsControl,
  sizeof(tsp81_UCS4Char),
  sizeof(tsp81_UCS4Char),
  &charTableUCS4
};
const tsp77encoding * const sp77encodingUCS4 = &encodingUCS4;

static tsp77charTable charTableUCS4Swapped = {
    {4, {'\xff', '\xfe', '\0', '\0'}}, /* indicator */
    {4, {'\0', '\0', '\0', '\0'}}, /* terminator */
    {4, {'\n', '\0', '\0', '\0'}}, /* nl */
    {4, {'\r', '\0', '\0', '\0'}}, /* cr */
    {4, {' ',  '\0', '\0', '\0'}},  /* blank */
    {4, {'\x1a',  '\0', '\0', '\0'}}  /* dosEOF */
};

static tsp77encoding encodingUCS4Swapped =
{
  &sp77EncodingClass,
  "encodingUCS4Swapped",
  csp78_ucs4_swap,
  sp77_UCS4SwappedAsciiCopy,
  sp81UCS4StringInfo,
  sp77_UCS4StringComplete,
  sp77_UCS4SwappedFillString,
  sp77_UCS4SwappedCountPads,
  sp77_UCS4SwappedIsAscii7,
  sp77_UCS4SwappedIsBlank,
  sp77_UCS4SwappedIsSpace,
  sp77_UCS4SwappedIsControl,
  sizeof(tsp81_UCS4Char),
  sizeof(tsp81_UCS4Char),
  &charTableUCS4Swapped
};
const tsp77encoding * const sp77encodingUCS4Swapped = &encodingUCS4Swapped;

static tsp77charTable charTableUTF8 = {
    {3, {'\xef', '\xbb', '\xbf', '\0'}}, /* indicator */
    {1, {'\0', '\0', '\0', '\0'}}, /* terminator */
    {1, {'\n', '\0', '\0', '\0'}}, /* nl */
    {1, {'\r', '\0', '\0', '\0'}}, /* cr */
    {1, {' ',  '\0', '\0', '\0'}},  /* blank */
    {1, {'\x1a',  '\0', '\0', '\0'}}  /* dosEOF */
};

static tsp77encoding encodingUTF8 =
{
  &sp77EncodingClass,
  "encodingUTF8",
  csp_utf8,
  sp77_UTF8AsciiCopy,
  sp83UTF8StringInfo,
  sp83UTF8StringComplete,
  sp77_AsciiFillString,
  sp77_AsciiCountPads,
  sp77_AsciiIsAscii7,
  sp77_AsciiIsBlank,
  sp77_AsciiIsSpace,
  sp77_AsciiIsControl,
  0,
  1,
  &charTableUTF8
};
const tsp77encoding * const sp77encodingUTF8 = &encodingUTF8;

/*---------------------------------------------------------------------------*/

const tsp77encoding * sp77nativeUnicodeEncoding()
{
  static short swapTest = 1;
  return ( (*(char *)&swapTest) ? sp77encodingUCS2Swapped : sp77encodingUCS2 );
}

/*----------------------------------------------------------------------------------*/
/* Function: sp77_<encoding>StringInfo                                              */
/* Description: Functions that report encoding specific information about C-strings */
/*                                                                                  */
/* C-Strings means that a string is terminated by a termination character! If no    */
/* termination character is found within byteLength, the byteLength terminates the  */
/* C-String.                                                                        */
/*                                                                                  */
/* Character and byte count may differ for specific encodings. Counts reported do   */
/* never contains termination character. A partial character is also not contained. */
/*                                                                                  */
/* pIsTerminated is pure information, usefull to decide whether to add own          */
/* termination character. Return value is not influenced.                           */
/*                                                                                  */
/* pIsExhausted may be used as warning, but if set 'FALSE' is returned.             */
/*                                                                                  */
/* pIsCorrupted is an error and if set 'FALSE' is returned.                         */
/*                                                                                  */
/* Even if an error was detected, pCharCount and pByteParsed are always valid.      */
/*                                                                                  */
/* Arguments: buffer [in] Buffer containing input string                            */
/*            byteLength [in] Size of buffer (not necessarily string length!!!)     */
/*            pCharCount [out] Number of character in buffer up to termination      */
/*            pBytesParsed [out] Number of bytes in buffer parsed up to termination */
/*            pIsTerminated [out] Flag: Terminated by a compete ZERO terminator     */
/*            pIsCorrupted [out] Flag: Input character not in valid encoding        */
/*            pIsExhausted [out] Flag: Last input character was only partial        */
/* Return value: TRUE if no error detected, FALSE else                              */
/*----------------------------------------------------------------------------------*/
static cbool sp77_AsciiStringInfo(const void *buffer,
                                  t_len maxByteLength,
                                  cbool bufferLengthIsInBytes, /* ignored*/
                                  t_len *pCharCount,
                                  t_len *pByteCount,
                                  cbool *pIsTerminated,
                                  cbool *pIsCorrupted,
                                  cbool *pIsExhausted)
{
  tsp00_Byte *ptr = (tsp00_Byte *)buffer;
  t_len charIndex;
  cbool result = TRUE;

  *pIsTerminated = *pIsCorrupted = *pIsExhausted = FALSE;

  for ( charIndex = 0; charIndex < maxByteLength; charIndex++ )
  {
    if ( ptr[charIndex] == 0 )
    {
      *pIsTerminated = TRUE;
      break;
    }
  }

  *pCharCount = *pByteCount = charIndex;

  return ((*pIsExhausted) || (*pIsCorrupted));
}

/*-------------------------------------------------------------------------------*/
/* Function: sp77_<encoding>AsciiCopy                                            */
/* Functions that copy Ascii literals into output target for different encodings */
/* Return value: TRUE if not enaugh space in target buffer, FALSE if copied      */
/*-------------------------------------------------------------------------------*/
static cbool sp77_AsciiAsciiCopy(
void **pTarget,
t_len *pTargetLength,
const tsp00_Char *Literal,
t_len LiteralLength)
{
  int ForceQuickExit = FALSE;
  t_len outLen = MIN_MSP77 (LiteralLength, *pTargetLength);

  if ( outLen > 0 )
  {
    SAPDB_memcpy(*pTarget, Literal, outLen);
    *pTarget = ((char *)*pTarget)+outLen;
    *pTargetLength -= outLen;
  }
  else
  {
    ForceQuickExit = TRUE;
  }
  return ForceQuickExit;
}

/*---------------------------------------------------------------------------*/

static cbool sp77_UCS2AsciiCopy(
void **pTarget,
t_len *pTargetLength,
const tsp00_Char *Literal,
t_len LiteralLength)
{
  t_len charWritten = 0;
  charWritten = sp81ASCIItoUCS2( (tsp81_UCS2Char *)*pTarget,
                                  *pTargetLength,
                                  FALSE,
                                  &charWritten,
                                  (const tsp00_Byte *)Literal,
                                  LiteralLength);

  if ( charWritten > 0 )
  {
    *pTarget = (void *)( ((tsp81_UCS2Char *)*pTarget) + charWritten );
    *pTargetLength -= charWritten * sizeof(tsp81_UCS2Char);
  }

  return (charWritten == 0);
}

/*---------------------------------------------------------------------------*/

static cbool sp77_UCS2SwappedAsciiCopy(
void **pTarget,
t_len *pTargetLength,
const tsp00_Char *Literal,
t_len LiteralLength)
{
  t_len charWritten = 0;

  charWritten = sp81ASCIItoUCS2( (tsp81_UCS2Char *)*pTarget, *pTargetLength, TRUE, &charWritten, (const tsp00_Byte *)Literal, LiteralLength);

  if ( charWritten > 0 )
  {
    *pTarget = (void *)( ((tsp81_UCS2Char *)*pTarget) + charWritten );
    *pTargetLength -= charWritten;
  }

  return (charWritten == 0);
}

/*---------------------------------------------------------------------------*/

static cbool sp77_UCS4AsciiCopy(
void **pTarget,
t_len *pTargetLength,
const tsp00_Char *Literal,
t_len LiteralLength)
{
  t_len charWritten = 0;

  charWritten = sp81ASCIItoUCS4( (tsp81_UCS4Char *)*pTarget, *pTargetLength, FALSE, &charWritten, (const tsp00_Byte *)Literal, LiteralLength);

  if ( charWritten > 0 )
  {
    *pTarget = (void *)( ((tsp81_UCS4Char *)*pTarget) + charWritten );
    *pTargetLength -= charWritten;
  }

  return (charWritten == 0);
}

/*---------------------------------------------------------------------------*/

static cbool sp77_UCS4SwappedAsciiCopy(
void **pTarget,
t_len *pTargetLength,
const tsp00_Char *Literal,
t_len LiteralLength)
{
  t_len charWritten = 0;

  charWritten = sp81ASCIItoUCS4( (tsp81_UCS4Char *)*pTarget, *pTargetLength, TRUE, &charWritten, (const tsp00_Byte *)Literal, LiteralLength);

  if ( charWritten > 0 )
  {
    *pTarget = (void *)( ((tsp81_UCS4Char *)*pTarget) + charWritten );
    *pTargetLength -= charWritten;
  }

  return (charWritten == 0);
}

/*---------------------------------------------------------------------------*/

static cbool sp77_UTF8AsciiCopy(
void **pTarget,
t_len *pTargetLength,
const tsp00_Char *Literal,
t_len LiteralLength)
{
  t_len bytesWritten;
  t_len srcBytesParsed;

  if ( sp83UTF8fromASCII(Literal, LiteralLength, &srcBytesParsed, (tsp00_Byte *)*pTarget, *pTargetLength, &bytesWritten)
      != sp83UTF8Convert_Success )
  {
    return TRUE;
  }

  *pTarget = (void *) (((tsp00_Byte *)*pTarget) + bytesWritten);
  *pTargetLength -= bytesWritten;

  return FALSE;
}

/*---------------------------------------------------------------------------*/

teo00_Bool sp77isValidEncoding (const tsp77encoding * encoding)
{
    if (encoding == NULL) {
        return FALSE;
    }
    if (encoding->encodingClass != &sp77EncodingClass) {
        return FALSE;
    }
    /* more tests? */
    return TRUE;
}

/*---------------------------------------*/
/* <<<<<< Padding output routines >>>>>> */
/*---------------------------------------*/

/*------------------------------------------*/
/* sp77_PutPadded for padding all encodings */
/*------------------------------------------*/

static t_len
sp77_PutPadded (
    void              **buf,
    t_len              *bufLen,
    const tsp00_Char   *arg,
    t_len               moveLen,
    tsp77_printfFormat *printfFormat)
{
  /* ROUTINE_DBG_MSP00("sp77_PutPadded"); */
  tsp78ConversionResult convertResult;
  cbool           padLeft = FALSE;
  cbool           padRight = FALSE;
  t_len           padLen;

  t_len           charLength;
  t_len           byteLength;
  cbool           isTerminated;
  cbool           isCorrupted;
  cbool           isExhausted;
  t_len           bytesParsed;
  t_len           bytesWritten;

  if ( printfFormat->flags.inputEncoding->stringInfo( arg, moveLen, true,
	&charLength, &byteLength, &isTerminated, &isCorrupted, &isExhausted) )
  {
    if ( isCorrupted ) return 0;
  }

  if ( printfFormat->flags.byteScale )
  {
    /* Width is specified in bytes */
    padLen = printfFormat->width - MIN_MSP77(printfFormat->width,byteLength);
  }
  else
  {
    /* Width is specified in characters */
    padLen = printfFormat->width - MIN_MSP77(printfFormat->width,charLength);
  }

  if (padLen > 0)
  {
    if (printfFormat->flags.justifyLeft)
        padRight = TRUE;
    else
        padLeft = TRUE;
  }

  if (padLeft)
  {
    printfFormat->encoding->fillString( buf, bufLen, padLen,
  			  	(char)(printfFormat->flags.zeros ? '0' : ' ') );
  }

  convertResult = sp78convertString(
               		        printfFormat->encoding,
				*buf, *bufLen, &bytesWritten,
			        false,
                                printfFormat->flags.inputEncoding,
				arg, byteLength, &bytesParsed );
  *buf = (void *)(((char *)*buf)+bytesWritten);
  *bufLen -= bytesWritten;
  if ( convertResult != sp78_Ok )
  {
    return 0;
  }


  if (padRight)
  {
    printfFormat->encoding->fillString( buf, bufLen, padLen,
  			  	(char)(printfFormat->flags.zeros ? '0' : ' ') );
  }

  return padLen + bytesWritten;
}

/*-------------------------------------------*/
/* sp77_PutHexPadded for padding hex strings */
/*-------------------------------------------*/

static const tsp00_Char lowDigits[17] =
   {'0', '1', '2', '3', '4',
   '5', '6', '7', '8', '9',
   'a', 'b', 'c', 'd', 'e', 'f', 'x'};
static const tsp00_Char upDigits[17] =
   {'0', '1', '2', '3', '4',
   '5', '6', '7', '8', '9',
   'A', 'B', 'C', 'D', 'E', 'F', 'X'};

/*----------------------------------------*/

static t_len
sp77_PutHexPadded (
    void              **buf,
    t_len              *bufLen,
    const tsp00_Byte   *arg,
    t_len               moveLen,
    tsp77_printfFormat    *printfFormat)
{
  /* ROUTINE_DBG_MSP00("sp77_PutHexPadded"); */
  cbool       padLeft = FALSE;
  cbool       padRight = FALSE;
  t_len       padLen;
  char       *tempBufferPtr;
  char       *tempBuffer;
  t_len       i;
  const tsp00_Char * digits = lowDigits;
  t_len       inputLen;

  if (isupper (printfFormat->argChar))
  {
    digits = upDigits;
  }

  if ( printfFormat->flags.inputEncoding->fixedCharacterSize > 1 )
  {
    inputLen = moveLen/printfFormat->flags.inputEncoding->fixedCharacterSize;
  }
  else
  {
    inputLen = moveLen;
  }

  padLen = printfFormat->width - MIN_MSP77(printfFormat->width,inputLen);

  tempBufferPtr = tempBuffer = alloca( 2*(padLen + moveLen) );

  if (padLen > 0)
  {
    if (printfFormat->flags.justifyLeft)
        padRight = TRUE;
    else
        padLeft = TRUE;
  }

  if (padLeft)
  {
    SAPDB_memset ( tempBuffer, ' ', 2*padLen);
    tempBuffer += 2*padLen;
  }

  for ( i = 0; i < moveLen; i++ )
  {
    tempBuffer [0] = digits [arg [i] / 16];
    tempBuffer [1] = digits [arg [i] % 16];
    tempBuffer += 2;
  }

  if (padRight)
  {
    SAPDB_memset ( tempBuffer, ' ', 2*padLen);
    tempBuffer += 2*padLen;
  }

  if ( printfFormat->encoding->asciiCopy(buf, bufLen, tempBufferPtr, 2*(padLen + moveLen)) )
  {
    return 0;
  }

  return 2*(padLen + moveLen);
}

/*----------------------------------------*/
/* Begin of Convertion routines           */
/*----------------------------------------*/

/*----------------------------------------*/

static t_len
sp77_unsignedConv (
    void **buf,
    t_len *bufLen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    /* ROUTINE_DBG_MSP00 ("sp77_unsignedConv"); */
    tsp00_Uint8       arg;
    char              tmpBuf[40];
    t_len             count = 0;
    unsigned          base;
    t_len             ownWritten = 0;
    t_len             converted;
    char              prefixBuffer[3];

    const tsp00_Char *       digits = lowDigits;

    switch (printfFormat->argSize) {
        case 'q':
            arg = va_arg (*args, tsp00_Uint8);

            break;
        case 'l':
        case 'L':
            arg = va_arg (*args, unsigned long);

            break;
#ifndef __GNUC__
        case 'h':
            arg = va_arg (*args, unsigned short);

            break;
#endif /* __GNUC__ */
        default:
            arg = va_arg (*args, t_len);

            break;
    }
    switch (printfFormat->argChar) {
        case 'o':
            base = 8;
            break;
        case 'u':
            base = 10;
            break;
        case 'x':
            base = 16;
            break;
        case 'X':
            base = 16;
            digits = upDigits;
            break;
        default:
            base = 10;
            break;
    }
    while (arg > 0) {
        tmpBuf[sizeof (tmpBuf) - count - 1] = digits[arg % base];
        ++count;
        arg /= base;
    }
    if (printfFormat->precision == UNDEF)
        printfFormat->precision = 1;
    while (count < printfFormat->precision) {
        tmpBuf[sizeof (tmpBuf) - count - 1] = '0';
        ++count;
    }
    if (printfFormat->flags.alternate) {
        switch (printfFormat->argChar) {
            case 'x':
            case 'X':
                if (printfFormat->flags.zeros
                    && !printfFormat->flags.justifyLeft )
                {
                  prefixBuffer[0] = '0';
                  prefixBuffer[1] = digits[16];
                  if ( printfFormat->encoding->asciiCopy(buf, bufLen, prefixBuffer, 2) )
                  {
                    return 0;
                  }
                  ownWritten = 2;
                  printfFormat->width -= 2;
                }
                else
                {
                  tmpBuf[sizeof (tmpBuf) - count - 1] = digits[16];
                  tmpBuf[sizeof (tmpBuf) - count - 2] = '0';
                  count += 2;
                }
                break;
            case 'o':
                if (tmpBuf[sizeof (tmpBuf) - count] != '0')
                {
                  tmpBuf[sizeof (tmpBuf) - count - 1] = '0';
                  ++count;
                }
                break;
            default:
                /* nothing required */
                break;
        }
    }

    converted = sp77_PutPadded (buf, bufLen,
        tmpBuf + sizeof (tmpBuf) - count, count, printfFormat);

    return ( converted > 0 ? (ownWritten + converted) : 0 );
}

/*----------------------------------------*/

static t_len
sp77_signedConv (
    void **buf,
    t_len *bufLen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    /* ROUTINE_DBG_MSP00 ("sp77_signedConv"); */
    tsp00_Int8      arg;
    tsp00_Uint8     uarg;
    char            signChar;
    char            tmpBuf[40];
    t_len           count = 0;
    t_len           ownWritten = 0;
    t_len           converted;

    switch (printfFormat->argSize) {
        case 'q':
            arg = va_arg (*args, tsp00_Int8);

            break;

        case 'l':
        case 'L':
            arg = va_arg (*args, long);

            break;

#ifndef __GNUC__
        case 'h':
            arg = va_arg (*args, short);

            break;
#endif /* __GNUC__ */
        default:
            arg = va_arg (*args, int);

            break;
    }
    uarg = (tsp00_Uint8) arg;
    if (arg < 0) {
        signChar = '-';
        uarg = (tsp00_Uint8) -arg;
    }
    else if (printfFormat->flags.sign)
        signChar = '+';
    else if (printfFormat->flags.space)
        signChar = ' ';
    else
        signChar = '\0';
    while (uarg > 0) {
        tmpBuf[sizeof (tmpBuf) - count - 1] = (char) ((uarg % 10) + '0');
        ++count;
        uarg /= 10;
    }
    if (printfFormat->precision == UNDEF)
        printfFormat->precision = 1;
    while (count < printfFormat->precision) {
        tmpBuf[sizeof (tmpBuf) - count - 1] = '0';
        ++count;
    }
    if (signChar != '\0')
    {
        if ( printfFormat->flags.zeros
         && !printfFormat->flags.justifyLeft )
        {
          if ( printfFormat->encoding->asciiCopy (buf, bufLen, &signChar, 1) )
          {
            return 0;
          }
          ownWritten = 1;
          --printfFormat->width;
        }
        else
        {
          tmpBuf[sizeof (tmpBuf) - count - 1] = signChar;
          ++count;
        }
    }

    converted = sp77_PutPadded( buf, bufLen,
          tmpBuf + sizeof (tmpBuf) - count, count, printfFormat );

    return ( converted > 0 ? (ownWritten + converted) : 0 );
}

/*----------------------------------------*/
/*
 * 'F' and 'f' format do not differ at all...
 * Precision specifies the number of digits after the decimal point
 * Decimal point is not written if precision is 0
 */
static t_len sp77printDouble ( char  *tmpBuf,
        	               t_len    tmpBuflen,
                               t_len    precision,
		               double arg,
                               cbool forceDecimalPoint)
{
    t_len count = 0;
    register double digitValue;
    register double roundedArg;
    t_len    precDigits;

    roundedArg = arg;
    for ( precDigits = 0; precDigits < precision; precDigits++ )
    {
        roundedArg *= 10.;
    }
    roundedArg = floor(roundedArg+0.5);

    for ( precDigits = precision; precDigits > 0; precDigits-- )
    {
        digitValue = roundedArg;
        roundedArg = floor(roundedArg/10.);
        digitValue -= 10. * roundedArg;
        if ( digitValue < 0. )
        {
            digitValue = 0.;
        }
        tmpBuf[tmpBuflen - count - 1] = (char) (((long)digitValue) + '0');
        ++count;
    }

    if ( precision > 0 || forceDecimalPoint )
    {
        tmpBuf[tmpBuflen - count - 1] = '.';
        ++count;
    }

    do
    {
        digitValue = roundedArg;
        roundedArg = floor(roundedArg/10.);
        digitValue -= 10. * roundedArg;
        tmpBuf[tmpBuflen - count - 1] = (char) (((long)digitValue) + '0');
        ++count;
    } while ( roundedArg > 0 );

    return count;
}

#if defined(SUN) || defined(NMP) || defined(SOLARIS)

#include <ieeefp.h>

#define CLASS_FUNC_SP77(arg_) fpclass(arg_)
#define FP_NANS_SP77       FP_SNAN
#define FP_NANQ_SP77       FP_QNAN
#define FP_NEG_INF_SP77    FP_NINF
#define FP_POS_INF_SP77    FP_PINF
#define FP_NEG_DENORM_SP77 FP_NDENORM
#define FP_POS_DENORM_SP77 FP_PDENORM
#define FP_NEG_ZERO_SP77   FP_NZERO
#define FP_POS_ZERO_SP77   FP_PZERO
#define FP_NEG_NORM_SP77   FP_NNORM
#define FP_POS_NORM_SP77   FP_PNORM

#elif defined(OSF1)

#include <fp_class.h>

#define CLASS_FUNC_SP77(arg_) fp_class(arg_)
#define FP_NANS_SP77       FP_SNAN
#define FP_NANQ_SP77       FP_QNAN
#define FP_NEG_INF_SP77    FP_NEG_INF
#define FP_POS_INF_SP77    FP_POS_INF
#define FP_NEG_DENORM_SP77 FP_NEG_DENORM
#define FP_POS_DENORM_SP77 FP_POS_DENORM
#define FP_NEG_ZERO_SP77   FP_NEG_ZERO
#define FP_POS_ZERO_SP77   FP_POS_ZERO
#define FP_NEG_NORM_SP77   FP_NEG_NORM
#define FP_POS_NORM_SP77   FP_POS_NORM

#elif defined(AIX)

#define CLASS_FUNC_SP77(arg_) class(arg_)
#define FP_NANS_SP77       FP_SNAN
#define FP_NANQ_SP77       FP_QNAN
#define FP_NEG_INF_SP77    FP_MINUS_INF
#define FP_POS_INF_SP77    FP_PLUS_INF
#define FP_NEG_DENORM_SP77 FP_MINUS_DENORM
#define FP_POS_DENORM_SP77 FP_PLUS_DENORM
#define FP_NEG_ZERO_SP77   FP_MINUS_ZERO
#define FP_POS_ZERO_SP77   FP_PLUS_ZERO
#define FP_NEG_NORM_SP77   FP_MINUS_NORM
#define FP_POS_NORM_SP77   FP_PLUS_NORM

#elif defined(FREEBSD)
// well, this there should be someone taking a look at,
// who has a plan of FP stuff ind the freeBSD core
#define CLASS_FUNC_SP77(arg_) _fpclassify(arg_)
#define FP_NANQ_SP77       FP_NAN
#define FP_POS_INF_SP77    FP_INFINITE
#define FP_POS_DENORM_SP77 FP_SUBNORMAL
#define FP_POS_ZERO_SP77   FP_ZERO
#define FP_POS_NORM_SP77   FP_NORMAL

// fpclassify is a macro in 5.0
// in 5.1+ its a function
#ifndef __fpclassifyd
// this is not nice !
int __fpclassifyd(double) __pure2;
int __fpclassifyd(double x)
{
        return 0;
}
#endif

#elif defined(HPUX) || defined(LINUX)

#define CLASS_FUNC_SP77(arg_) fpclassify(arg_)
#define FP_NANQ_SP77       FP_NAN
#define FP_POS_INF_SP77    FP_INFINITE
#define FP_POS_DENORM_SP77 FP_SUBNORMAL
#define FP_POS_ZERO_SP77   FP_ZERO
#define FP_POS_NORM_SP77   FP_NORMAL

#elif defined(WIN32)

#include <float.h>

#define CLASS_FUNC_SP77(arg_) _fpclass(arg_)
#define FP_NANS_SP77       _FPCLASS_SNAN
#define FP_NANQ_SP77       _FPCLASS_QNAN
#define FP_NEG_INF_SP77    _FPCLASS_NINF
#define FP_POS_INF_SP77    _FPCLASS_PINF
#define FP_NEG_DENORM_SP77 _FPCLASS_ND
#define FP_POS_DENORM_SP77 _FPCLASS_PD
#define FP_NEG_ZERO_SP77   _FPCLASS_NZ
#define FP_POS_ZERO_SP77   _FPCLASS_PZ
#define FP_NEG_NORM_SP77   _FPCLASS_NN
#define FP_POS_NORM_SP77   _FPCLASS_PN

#endif

/*
  Invalid doubles tend to produced floating point exception...
  If we found one, we return a symbolic name and the length of that name
 */
static t_len sp77_testForInvalidDouble(char *tmpBuf, t_len tmpBuflen, double arg)
{
#ifndef FREEBSD
    switch( CLASS_FUNC_SP77(arg) )
    {
#if !defined(HPUX) && !defined(LINUX)
    case FP_NANS_SP77:    strcpy( &tmpBuf[tmpBuflen - 4], "NaNS" );
        return 4;
    case FP_NEG_INF_SP77:    strcpy( &tmpBuf[tmpBuflen - 4], "-Inf" );
        return 4;
    case FP_NEG_DENORM_SP77:    strcpy( &tmpBuf[tmpBuflen - 4], "-Den" );
        return 4;
    case FP_NEG_ZERO_SP77:    strcpy( &tmpBuf[tmpBuflen - 4], "-0.0" );
        return 4;
    case FP_NEG_NORM_SP77:
        return 0;
#endif
    case FP_NANQ_SP77:    strcpy( &tmpBuf[tmpBuflen - 4], "NaNQ" );
        return 4;
    case FP_POS_INF_SP77:    strcpy( &tmpBuf[tmpBuflen - 4], "+Inf" );
        return 4;
    case FP_POS_DENORM_SP77:    strcpy( &tmpBuf[tmpBuflen - 4], "+Den" );
        return 4;
    case FP_POS_ZERO_SP77:
        return 0;
    case FP_POS_NORM_SP77:
        return 0;
    default:         strcpy( &tmpBuf[tmpBuflen - 4], "NaN?" );
        return 4;
    }
#else 	/* FREEBSD */
    if(isinf(arg)) { strcpy( &tmpBuf[tmpBuflen - 4], "+Inf" ); return 0; }
    if(isnan(arg)) { strcpy( &tmpBuf[tmpBuflen - 4], "NaNS" ); return 0; }
    // rest to be implemented
#endif
}

/*
 * 'E' and 'e' format differ only in 'ExponentChar'
 * Precision specifies the number of digits after the decimal point
 * Decimal point is not written if precision is 0
 */
t_len sp77printExponentDouble ( char  *tmpBuf,
        	  t_len    tmpBuflen,
		  t_len    precision,
		  double   arg,
		  const tsp00_Char   ExponentChar,
                  cbool    forceDecimalPoint)
{
    t_len count = 0;
    register double digitValue;
    register double roundedArg;
    register double shiftedArg;
    register double multiplier = 1.;
    t_len    precDigits;
    int    Exponent = 0;
    int    NegativExponent = 0;

    shiftedArg = arg;

    if ( shiftedArg != 0.0 )
    {
        /*
         * Shift until shiftedArg is in interval ]1,10[
         */
        while ( shiftedArg >= 10. && shiftedArg != 0.0 )
        {
            Exponent++;
            shiftedArg /= 10.;
        }
        while ( shiftedArg < 1. && shiftedArg != 0.0 )
        {
            Exponent--;
            shiftedArg *= 10.;
        }
    }

    /*
     * Now find the precision multiplier
     */
    for ( precDigits = 0; precDigits < precision; precDigits++ )
    {
        multiplier *= 10.;
    }

    /*
     * Now round the value to precision
     */
    roundedArg = floor((shiftedArg*multiplier)+0.5);
    /*
     * Due to rounding effect, now the value could be above 10
     */
    if ( ( roundedArg / multiplier ) >= 10. )
    {
        Exponent++;
        shiftedArg /= 10.;
        multiplier *= 10.;
        roundedArg = floor((shiftedArg*multiplier)+0.5);
    }

    /*
     * Show exponent
     */
    if ( Exponent < 0 )
    {
         Exponent = -Exponent;
        NegativExponent = 1;
    }
    if ( Exponent < 10 )
    {
        tmpBuf[tmpBuflen - count - 1] = Exponent + '0';
        ++count;
        tmpBuf[tmpBuflen - count - 1] = '0';
        ++count;
    }
    else
    {
	do
        {
	    tmpBuf[tmpBuflen - count - 1] = (Exponent%10) + '0';
            ++count;
	    Exponent /= 10;
	} while( Exponent > 0 );
    }

    if ( NegativExponent )
    {
         tmpBuf[tmpBuflen - count - 1] = '-';
         ++count;
    }
    else
    {
         tmpBuf[tmpBuflen - count - 1] = '+';
         ++count;
    }

    tmpBuf[tmpBuflen - count - 1] = ExponentChar;
    ++count;

    /*
     * If precision is 0, no decimal point is shown
     */
    if ( (precision == UNDEF || precision == 0) && !forceDecimalPoint )
    {
        tmpBuf[tmpBuflen - count - 1] = (char) (((long)roundedArg) + '0');
        ++count;
    }
    else
    {

	for ( precDigits = precision; precDigits > 0; precDigits-- )
	{
        long outputDigit;
	    digitValue = roundedArg;
	    roundedArg = floor(roundedArg/10.);
	    digitValue -= 10. * roundedArg;
        /* due to overflow digitValue could be out of range... */
        /* these out of range digits are mapped to 0 here... */
	    outputDigit = (long)digitValue;
        if ( outputDigit > 9 || outputDigit < 0 ) outputDigit = 0;
        tmpBuf[tmpBuflen - count - 1] = (char) (outputDigit + '0');
	    ++count;
	}

	tmpBuf[tmpBuflen - count - 1] = '.';
	++count;

	do
	{
	    digitValue = roundedArg;
	    roundedArg = floor(roundedArg/10.);
	    digitValue -= 10. * roundedArg;
	    tmpBuf[tmpBuflen - count - 1] = (char) (((long)digitValue) + '0');
	    ++count;
	} while ( roundedArg > 0 );

    }

    return count;
}

/*
 * G format uses 'E' format if Exponet < -4 or >= precision
 *   else it uses 'F' format
 *
 * Precision specifies the number of significant digits!!!
 */
static t_len
sp77printSomeExponentDouble ( tsp00_Char      *tmpBuf,
        	              t_len            tmpBuflen,
		              int              precision,
		              double           arg,
		              const tsp00_Char ExponentChar,
                              cbool            forceDecimalPoint)
{
    register double roundedArg;
    register double shiftedArg;
    register double multiplier = 1.;
    int             precDigits;
    int             Exponent = 0;

    shiftedArg = arg;

    if ( shiftedArg != 0.0 )
    {
        /*
         * Shift until shiftedArg is in interval ]1,10[
         */
        while ( shiftedArg >= 10. )
        {
            Exponent++;
            shiftedArg /= 10.;
        }
        while ( shiftedArg < 1. )
        {
            Exponent--;
            shiftedArg *= 10.;
        }
    }

    /*
     * Now find the precision multiplier
     */
    for ( precDigits = 0; precDigits < precision; precDigits++ )
    {
	multiplier *= 10.;
    }

    /*
     * Now round the value to precision
     */
    roundedArg = floor((shiftedArg*multiplier)+0.5);
    /*
     * Due to rounding effect, now the value could be above 10
     */
    if ( ( roundedArg / multiplier ) >= 10. )
    {
	Exponent++;
	/* calculation is redone in calls below... */
    }

    /*
     * Always at least one digit is significant
     */
    if ( precision == 0 )
    {
	precision = 1;
    }

    if ( Exponent < -4 || Exponent >= precision )
    {
	return sp77printExponentDouble ( tmpBuf,
                                         tmpBuflen,
					 precision - 1,
                                         arg,
				 	 (const tsp00_Char)(ExponentChar == 'G' ? 'E' : 'e'),
                                         forceDecimalPoint);
    }
    else
    {
	return sp77printDouble ( tmpBuf,
                                 tmpBuflen,
				 precision - (Exponent + 1),
                                 arg,
                                 forceDecimalPoint);
    }
}

/*---------------------------------------------------------------------------*/

static t_len
sp77_doubleConv (
    void **buf,
    t_len *bufLen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    double          arg;
    char            signChar;
    char            tmpBuf[250];
    t_len             count = 0;
    t_len             ownWritten = 0;
    t_len             converted;

    arg = va_arg (*args, double);

    count = sp77_testForInvalidDouble(tmpBuf, sizeof(tmpBuf), arg);
    if ( 0 == count )
    {
        if (arg < 0) {
            signChar = '-';
            arg = -arg;
        }
        else if (printfFormat->flags.sign)
            signChar = '+';
        else if (printfFormat->flags.space)
            signChar = ' ';
        else
            signChar = '\0';
        if (signChar != '\0') {
            if (printfFormat->flags.zeros
              && !printfFormat->flags.justifyLeft )
            {
              if ( printfFormat->encoding->asciiCopy(buf, bufLen, &signChar, 1) )
              {
                return 0;
              }
              --printfFormat->width;
              ownWritten = 1;
            }
            else
            {
                tmpBuf[0] = signChar;
                ++count;
            }
        }
        if ( printfFormat->argChar == 'f' || printfFormat->argChar == 'F' )
        {
    	    count = sp77printDouble (tmpBuf,
        		    sizeof (tmpBuf),
			    printfFormat->precision,
			    arg,
                            printfFormat->flags.alternate);
        }
        else if ( printfFormat->argChar == 'e' || printfFormat->argChar == 'E' )
        {
    	    count = sp77printExponentDouble ( tmpBuf,
        		    sizeof (tmpBuf),
			    printfFormat->precision,
        		    arg,
			    (char)printfFormat->argChar,
                            printfFormat->flags.alternate);
        }
        else if ( printfFormat->argChar == 'g' || printfFormat->argChar == 'G' )
        {
    	    count = sp77printSomeExponentDouble ( tmpBuf,
        		    sizeof (tmpBuf),
			    printfFormat->precision,
        		    arg,
			    (char)printfFormat->argChar,
                            printfFormat->flags.alternate);
        }

        if (signChar != '\0')
        {
            if ( count < sizeof(tmpBuf) )
            {
                ++count;
                tmpBuf[sizeof(tmpBuf)-count] = signChar;
            }
            else
            {
                return 0;
            }
        }
    }

    converted = sp77_PutPadded( buf, bufLen,
          tmpBuf + sizeof (tmpBuf) - count, count, printfFormat);

    return ( converted > 0 ? (ownWritten + converted) : 0 );
}

/*----------------------------------------*/

static t_len
sp77_cConv (
    void **buf,
    t_len *bufLen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    /* ROUTINE_DBG_MSP00 ("sp77_cConv"); */
    char            arg;

    arg = (char) va_arg (*args, int);

    return sp77_PutPadded( buf, bufLen, &arg, 1, printfFormat );
}

/*----------------------------------------*/

static t_len
sp77_pConv (
    void **buf,
    t_len *bufLen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    /* ROUTINE_DBG_MSP00 ("sp77_pConv"); */
#if defined(BIT64)
    printfFormat->argSize = 'q';
#else
    printfFormat->argSize = 'l';
#endif
    printfFormat->argChar = 'X';
    return sp77_unsignedConv (buf, bufLen, args, printfFormat);
}

/*----------------------------------------*/

static const tsp00_Char * c_nullString = "(null)";

/*----------------------------------------*/

static t_len
sp77_stringConv (
    void **buf,
    t_len *bufLen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    /* ROUTINE_DBG_MSP00 ("sp77_stringConv"); */
    const tsp00_Char *    argStr;

    t_len    moveLen;
    t_len    charactersWritten;
    cbool    isTerminated;
    cbool    isCorrupted;
    cbool    isExhausted;

    /* the argument is always a byte string */
    argStr = va_arg (*args, tsp00_Char *);

    if (argStr == NULL)
    {
      printfFormat->flags.inputEncoding = sp77encodingAscii;
      argStr = c_nullString;
    }

    /* calculate real len */
    if (printfFormat->precision != UNDEF )
    {
      printfFormat->flags.inputEncoding->stringInfo(
	argStr, printfFormat->precision, printfFormat->flags.byteScale,
	&charactersWritten, &moveLen,
	&isTerminated, &isCorrupted, &isExhausted);
    }
    else
    {
      printfFormat->flags.inputEncoding->stringInfo(
	argStr, (t_len)-1, true,
	&charactersWritten, &moveLen,
	&isTerminated, &isCorrupted, &isExhausted);
    }

    if ( printfFormat->flags.byteScale )
    {
      charactersWritten = moveLen;
    }

    if (printfFormat->width == UNDEF || printfFormat->width == 0)
        printfFormat->width = charactersWritten;

    return sp77_PutPadded ( buf, bufLen, argStr, moveLen, printfFormat );
}

/*----------------------------------------*/

static t_len
sp77_hexConv (
    void **buf,
    t_len *bufLen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    /* ROUTINE_DBG_MSP00 ("sp77_hexConv"); */
    const tsp00_Char * argStr;
    t_len              moveLen;
    t_len              characterWritten;
    cbool    isTerminated;
    cbool    isCorrupted;
    cbool    isExhausted;

    /* the argument is always a single tjk2_byte character string */
    argStr = va_arg (*args, char *);

    if (argStr == NULL)
    {
      printfFormat->flags.inputEncoding = sp77encodingAscii;
      argStr = c_nullString;
    }

    if (printfFormat->precision != UNDEF)
    {
        characterWritten = printfFormat->precision;
        moveLen = printfFormat->precision;
        if ( !printfFormat->flags.byteScale
           && printfFormat->flags.inputEncoding->fixedCharacterSize > 1 )
        {
             moveLen *= printfFormat->flags.inputEncoding->fixedCharacterSize;
        }
    }
    else
    {
        printfFormat->flags.inputEncoding->stringInfo(
	        argStr, (t_len)-1, true,
	        &characterWritten, &moveLen,
	        &isTerminated, &isCorrupted, &isExhausted);
    }

    if ( printfFormat->flags.inputEncoding->fixedCharacterSize < 1 )
    {
        characterWritten = moveLen;
    }

    if (printfFormat->width == UNDEF || printfFormat->width == 0)
        printfFormat->width = characterWritten;

    return sp77_PutHexPadded(buf, bufLen, (const tsp00_Byte *)argStr, moveLen, printfFormat);
}

/*----------------------------------------*/

static t_len
sp77_invalidChar (
    void **buf,
    t_len *buflen,
    va_list * args,
    tsp77_printfFormat * printfFormat)
{
    /* ROUTINE_DBG_MSP00 ("sp77_invalidChar"); */
    return UNDEF;
}

/*----------------------------------------*/
/* End of conversion functions            */
/*----------------------------------------*/

static t_len
sp77_parseWidth (
    const tsp00_Char **formatArg,
    va_list * args)
{
    /* ROUTINE_DBG_MSP00 ("sp77_parseWidth"); */
    const tsp00_Char     *format = *formatArg;
    t_len             result = 0;

    if (*format == '*') {
        ++(*formatArg);
        return va_arg (*args, int);
    }
    if ((*format) >= '0' && (*format<='9')) {
        do {
            result = (result * 10) + (*format - '0');
            ++format;
            *formatArg = format;
        } while (*format >= '0' &&
                 *format <= '9');
        return result;
    }
    else
        return 0;
}

/*----------------------------------------*/

static const tsp00_Char *
sp77_parseFormat (
    const tsp00_Char *format,
    tsp77_printfFormat * printfFormat,
    va_list * args)
{
    /* ROUTINE_DBG_MSP00 ("sp77_parseFormat"); */
    cbool           parsingFlags = TRUE;

    /*M2_ASSERT (*format == '%');*/
    printfFormat->width = UNDEF;
    printfFormat->precision = UNDEF;
    printfFormat->argSize = '\0';
    printfFormat->flags.justifyLeft = FALSE;
    printfFormat->flags.sign = FALSE;
    printfFormat->flags.space = FALSE;
    printfFormat->flags.alternate = FALSE;
    printfFormat->flags.zeros = FALSE;
    printfFormat->flags.byteScale = FALSE;
    printfFormat->flags.inputEncoding = 0;
    while (parsingFlags) {
        ++format;

        switch (*format) {
            case '-':
                printfFormat->flags.justifyLeft = TRUE;
                break;
            case '+':
                printfFormat->flags.sign = TRUE;
                break;
            case ' ':
                printfFormat->flags.space = TRUE;
                break;
            case '#':
                printfFormat->flags.alternate = TRUE;
                break;
            case '0':
                printfFormat->flags.zeros = TRUE;
                break;
            case '\'':
                printfFormat->flags.byteScale = TRUE;
                break;
            case '=':
                printfFormat->flags.inputEncoding = va_arg (*args, tsp77encoding *);
                break;
            default:
                parsingFlags = FALSE;
                break;
        }
    }
    printfFormat->width = sp77_parseWidth (&format, args);
    if (*format == '.') {
        ++format;
        printfFormat->precision = sp77_parseWidth (&format, args);
    }
    if ((*format == 'h') || (*format == 'l') || (*format == 'L') || (*format == 'q')) {
        printfFormat->argSize = *format;
        ++format;
    }
    printfFormat->argChar = *format;

    if ( !printfFormat->flags.inputEncoding )
    {
      if ( *format == 'S' )
        printfFormat->flags.inputEncoding = sp77nativeUnicodeEncoding();
      else
        printfFormat->flags.inputEncoding = sp77encodingAscii;
    }

    if ((*format == 'e') || (*format == 'E')
     || (*format == 'f') || (*format == 'F')
     || (*format == 'g') || (*format == 'G'))
    {
        if ( printfFormat->precision == UNDEF )
	{
	    printfFormat->precision = 6;
	}
    }
    else
    {
      if (printfFormat->flags.byteScale)
      {
        if ( printfFormat->flags.inputEncoding->fixedCharacterSize > 1 )
        {
          if ( printfFormat->width != UNDEF )
            printfFormat->width /= printfFormat->flags.inputEncoding->fixedCharacterSize;
          if ( printfFormat->precision != UNDEF )
	    printfFormat->precision /= printfFormat->flags.inputEncoding->fixedCharacterSize;
  	  printfFormat->flags.byteScale = FALSE;
	}
      }
    }
    ++format;
#ifdef TEST
    fprintf (stdTrace, "Format width: %d; prec: %d; ( ",
        printfFormat->width, printfFormat->precision);
    if (printfFormat->flags.justifyLeft)
        fprintf (stdTrace, "ljust ");
    if (printfFormat->flags.sign)
        fprintf (stdTrace, "sign ");
    if (printfFormat->flags.space)
        fprintf (stdTrace, "space ");
    if (printfFormat->flags.alternate)
        fprintf (stdTrace, "alt ");
    if (printfFormat->flags.zeros)
        fprintf (stdTrace, "zeros ");
    if (printfFormat->flags.byteScale)
        fprintf (stdTrace, "byteScale ");
    fprintf (stdTrace, "inputEncoding %s", printfFormat->flags.inputEncoding->whichEncodingClass );
    fprintf (stdTrace, ") -> %c\n", printfFormat->argChar);
#endif

    return format;
}

/*----------------------------------------*/
/* Function: sp77_convFunction */
/* Description: Do the wanted conversion and return number of characters */
/* written or 0 on error (convertion failed) */
/* Return value: Number of characters written or 0 on error */
typedef t_len     (*sp77_convFunction) (
   void **buf,
   t_len *buflen,
   va_list * args,
   tsp77_printfFormat * printfFormat
);

typedef struct tjk_convFunctionStruct {
    sp77_convFunction f;
} tjk_convFunctionStruct;

/*----------------------------------------*/

static tjk_convFunctionStruct sp77_convLower[26] =
{
    {sp77_invalidChar},          /* 'a' */
    {sp77_invalidChar},          /* 'b' */
    {sp77_cConv},                /* 'c' */
    {sp77_signedConv},           /* 'd' */
    {sp77_doubleConv},           /* 'e' */
    {sp77_doubleConv},           /* 'f' */
    {sp77_doubleConv},           /* 'g' */
    {sp77_invalidChar},          /* 'h' */  /* used as modifier */
    {sp77_signedConv},           /* 'i' */
    {sp77_invalidChar},          /* 'j' */
    {sp77_invalidChar},          /* 'k' */
    {sp77_invalidChar},          /* 'l' */  /* used as modifier */
    {sp77_invalidChar},          /* 'm' */
    {sp77_invalidChar},          /* 'n' */
    {sp77_unsignedConv},         /* 'o' */
    {sp77_pConv},                /* 'p' */
    {sp77_invalidChar},          /* 'q' */  /* used as modifier */
    {sp77_hexConv},              /* 'r' */
    {sp77_stringConv},           /* 's' */
    {sp77_invalidChar},          /* 't' */
    {sp77_unsignedConv},         /* 'u' */
    {sp77_invalidChar},          /* 'v' */
    {sp77_invalidChar},          /* 'w' */
    {sp77_unsignedConv},         /* 'x' */
    {sp77_invalidChar},          /* 'y' */
    {sp77_invalidChar}           /* 'z' */
};

static tjk_convFunctionStruct sp77_convUpper[26] =
{
    {sp77_invalidChar},          /* 'A' */
    {sp77_invalidChar},          /* 'B' */
    {sp77_invalidChar},          /* 'C' */
    {sp77_invalidChar},          /* 'D' */
    {sp77_doubleConv},           /* 'E' */
    {sp77_doubleConv},           /* 'F' */
    {sp77_doubleConv},           /* 'G' */
    {sp77_invalidChar},          /* 'H' */
    {sp77_invalidChar},          /* 'I' */
    {sp77_invalidChar},          /* 'J' */
    {sp77_invalidChar},          /* 'K' */
    {sp77_invalidChar},          /* 'L' */  /* used as modifier */
    {sp77_invalidChar},          /* 'M' */
    {sp77_invalidChar},          /* 'N' */
    {sp77_invalidChar},          /* 'O' */
    {sp77_invalidChar},          /* 'P' */
    {sp77_invalidChar},          /* 'Q' */
    {sp77_hexConv},              /* 'R' */
    {sp77_stringConv},           /* 'S' */
    {sp77_invalidChar},          /* 'T' */
    {sp77_invalidChar},          /* 'U' */
    {sp77_invalidChar},          /* 'V' */
    {sp77_invalidChar},          /* 'W' */
    {sp77_unsignedConv},         /* 'X' */
    {sp77_invalidChar},          /* 'Y' */
    {sp77_invalidChar}           /* 'Z' */
};

/*----------------------------------------*/

/*
 * gcc 3.2.2 needs a real va_list array to work properly, not only a pointer
 */
#if defined(LINUX) && (defined(S390X) || defined(X86_64) || defined(SDBonPPC64))
static int
sp77_vsprintfCommon (
    const tsp77encoding *encoding,
    void *buf,
    t_len buflen,
    const tsp00_Char *format,
    va_list xargs)
#else
static int
sp77_vsprintfCommon (
    const tsp77encoding *encoding,
    void *buf,
    t_len buflen,
    const tsp00_Char *format,
    va_list args)
#endif
{
    /* ROUTINE_DBG_MSP00 ("sp77_vsprintfCommon"); */
    char               *ptr = (char *)buf;
    char               *endPtr = ptr + buflen;
    const tsp00_Char   *textStart;
    t_len               converted = 0;
    tsp77_printfFormat  printfFormat;
    t_len               charsWritten;
    cbool               error = FALSE;
    t_len               literalLen;
#if defined(LINUX) && (defined(S390X) || defined(X86_64) || defined(SDBonPPC64))
    va_list args;
    __va_copy(args,xargs);
#endif

    if ( !encoding || encoding->encodingClass != &sp77EncodingClass )
    {
      return -1;
    }

    /* Make encoding accessable to printfFormat using routines */
    printfFormat.encoding = encoding;

    buflen -= encoding->charTable->terminator.byteCount;  /* leave room for terminator */
    while ((*format != '\0') && (buflen > 0) && !error) {
        /*
         * look for format
         */
        textStart = format;
        while ((*format != '\0') && (*format != '%')) {
            ++format;
        }
        /*
         * copy literal text
         */
        if (format > textStart) {
            literalLen = format - textStart;
            /* check length */
            /* move bytes */
            error = encoding->asciiCopy((void **)&ptr, &buflen, textStart, literalLen);
            if ( error ) continue;
        }

        if (*format == '%') {
            /*
             * parse format
             */
#if defined(SOLARIS) && defined(X86_64)
            format = sp77_parseFormat (format, &printfFormat, args);
#else
            format = sp77_parseFormat (format, &printfFormat, &args);
#endif
            /*
             * call conversion
             */
            switch (printfFormat.argChar) {
                case '\0':
                    error = TRUE;
                    break;
                case '%':
                    error = encoding->asciiCopy((void **)&ptr, &buflen, "%", 1);
                    break;
                case 'n':
                    switch (printfFormat.argSize) {
#ifndef __GNUC__
                        case 'h':
                            *va_arg (args, short *) = (short) (ptr - (char *)buf);

                            break;
#endif /* __GNUC__ */
                        case 'l':
                            *va_arg (args, long *) = ptr - (char *)buf;

                            break;
                        case 'q':
                            *va_arg (args, tsp00_Int8 *) = ptr - (char *)buf;

                            break;
                        case '\0':
                            *va_arg (args, int *) = ptr - (char *)buf;

                            break;
                        default:
                            error = TRUE;
                            break;
                    }
                    break;
                default:
                    if (BETWEEN_MSP77 ('a', printfFormat.argChar, 'z')) {
#if defined(SOLARIS) && defined(X86_64)
                        charsWritten = sp77_convLower[printfFormat.argChar - 'a']
                            .f ((void **)&ptr, &buflen, args, &printfFormat);
#else
                        charsWritten = sp77_convLower[printfFormat.argChar - 'a']
                            .f ((void **)&ptr, &buflen, &args, &printfFormat);
#endif
                    }
                    else if (BETWEEN_MSP77 ('A', printfFormat.argChar, 'Z')) {
#if defined(SOLARIS) && defined(X86_64)
                        charsWritten = sp77_convUpper[printfFormat.argChar - 'A']
                            .f ((void **)&ptr, &buflen, args, &printfFormat);
#else
                        charsWritten = sp77_convUpper[printfFormat.argChar - 'A']
                            .f ((void **)&ptr, &buflen, &args, &printfFormat);
#endif
                    }
                    else {
                        charsWritten = UNDEF;
                    }

                    if (charsWritten != UNDEF) {
                        --converted;
                    }
                    else
                    {
                      error = TRUE;
                    }
                    break;
            }
#ifdef TRACE
            fprintf (stdTrace, "restformat: '%s'\n", format);
#endif
        }
    }

    {
        /*
         * close with zero, even if there was an error
         */
        void *zeroPointer;
        if (ptr >= endPtr) {
            ptr = endPtr - encoding->charTable->terminator.byteCount;
        }
        zeroPointer = (void *)ptr;
        SAPDB_memcpy (zeroPointer, encoding->charTable->terminator.bytes,
            encoding->charTable->terminator.byteCount);
    }

    if (error)
        return converted;
    else
        return (ptr - (char *)buf);
}

/*==========================================*/
/* Exported Functions                       */
/*==========================================*/

/*----------------------------------------*/

int
sp77vsprintfUnicode (
    const tsp77encoding * encoding,
    void *buf,
    int buflen,
    const char *format,
    va_list args)
{
  /* could be negativ, so it is checked here and NOT in vsprintfCommon... */
  if ( buflen <= 0 ) return 0;

    /* ROUTINE_DBG_MSP00 ("sp77vsprintf"); */
  return sp77_vsprintfCommon (encoding, buf, (t_len)buflen, format, args);
}

/*----------------------------------------*/

int
sp77vsprintf (
    char *buf,
    int buflen,
    const char *format,
    va_list args)
{
  /* could be negativ, so it is checked here and NOT in vsprintfCommon... */
  if ( buflen <= 0 ) return 0;

  /* ROUTINE_DBG_MSP00 ("sp77vsprintf"); */
  return sp77_vsprintfCommon (sp77encodingAscii, buf, (t_len)buflen, format, args);
}

/*----------------------------------------*/

int
sp77sprintfUnicode (
    const tsp77encoding *encoding,
    void *buf,
    int buflen,
    const char *format,
    ...)
{
  /* ROUTINE_DBG_MSP00 ("sp77sprintf"); */
  int             result;
  va_list         args;

  if ( buflen <= 0 ) return 0;

  va_start (args, format);
  result = sp77_vsprintfCommon (encoding, buf, (t_len)buflen, format, args);
  va_end (args);

  return result;
}

/*----------------------------------------*/

int
sp77sprintf (
    char *buf,
    int buflen,
    const char *format,
    ...)
{
  /* ROUTINE_DBG_MSP00 ("sp77sprintf"); */
  int             result;
  va_list         args;

  if ( buflen <= 0 ) return 0;

  va_start (args, format);
  result = sp77_vsprintfCommon (sp77encodingAscii, buf, (t_len)buflen, format, args);
  va_end (args);

  return result;
}

/*----------------------------------------*/

#ifdef TEST

static void TestUppercase()
{
  const char * TestString = "tÄäh'ö\"i\"'";
  const char * CompareString = "TÄÄH'ö\"i\"'";

  char UCS2Buffer[100];
  char UCS2SwapBuffer[100];
  char outputBuffer[200];
  int outlen;

  outlen = sp77sprintfUnicode(sp77encodingUCS2, UCS2Buffer, sizeof(UCS2Buffer), "%s", TestString);

  sp81UCS2QuotesContainingStringToupper(UCS2Buffer, outlen);

  sp77sprintf(outputBuffer, sizeof(outputBuffer), "%=s", sp77encodingUCS2, UCS2Buffer);

  if ( strcmp(outputBuffer, CompareString) != 0 )
  {
    printf("TestUppercase nonswapped failed: Original %s Wanted %s result %s\n", TestString, CompareString, outputBuffer );
  }
  else
  {
    printf("TestUppercase OK: Original %s Wanted %s result %s\n", TestString, CompareString, outputBuffer );
  }

  outlen = sp77sprintfUnicode(sp77encodingUCS2Swapped, UCS2SwapBuffer, sizeof(UCS2SwapBuffer), "%s", TestString);

  sp81UCS2QuotesContainingSwappedStringToupper(UCS2SwapBuffer, outlen);

  sp77sprintf(outputBuffer, sizeof(outputBuffer), "%=s", sp77encodingUCS2Swapped, UCS2SwapBuffer);

  if ( strcmp(outputBuffer, CompareString) != 0 )
  {
    printf("TestUppercase swapped failed: Original %s Wanted %s result %s\n", TestString, CompareString, outputBuffer );
  }
  else
  {
    printf("TestUppercase OK: Original %s Wanted %s result %s\n", TestString, CompareString, outputBuffer );
  }
}

static int
sprintfLib (
    char *buf,
    int buflen,
    const tsp00_Char *format,
    ...)
{
    /* ROUTINE_DBG_MSP00 ("sprintfLib"); */
    int             result;
    va_list         args;

    va_start (args, format);
    result = vsprintf (buf, format, args);
    va_end (args);
    return result;
}

typedef int     sprintfFunction (char *, int, const tsp00_Char *,...);
typedef struct t_sprintfFunctionRec {
    sprintfFunction *f;
} t_sprintfFunctionRec;

static t_sprintfFunctionRec sprintfF[2] =
{
    {sp77sprintf},
    {sprintfLib}
};

#define OWN     0
#define LIB     1

static const tsp00_Char * origins[] =
{"own", "lib"};

/*----------------------------------------*/

static void
sp77_testStrings (void)
{
    /* ROUTINE_DBG_MSP00 ("sp77_testStrings"); */
    char            buf[2000];
    int             i;
    const tsp00_Char     *someString = "someString";
    const tsp00_Char     *otherString = "otherString";
    const tsp00_Char     *simpleFormat = "%s";
    const tsp00_Char     *complexFormat = "%20.5s";
    const tsp00_Char     *testFormat = "%s %% %s";

    for (i = OWN; i <= LIB; ++i) {
        sprintfF[i].f (buf, sizeof (buf), testFormat, someString, otherString);
        fprintf (stdout, "%s: '%s'\n", origins[i], buf);
    }
}

/*----------------------------------------*/

static void
sp77_testChars (void)
{
    /* ROUTINE_DBG_MSP00 ("sp77_testChars"); */
    char            buf[2000];
    int             i;
    int             j;
    char            ch;
    const tsp00_Char *       testFormat = "Char <%3c>";

    for (j = 32; j < 125; ++j) {
        ch = (char) j;
        for (i = OWN; i <= LIB; ++i) {
            sprintfF[i].f (buf, sizeof (buf), testFormat, ch);
            fprintf (stdout, "%s: '%s'\n", origins[i], buf);
        }
    }
}

#define c_unsignedFormats 3

/*----------------------------------------*/

static void
sp77_testUnsigned (void)
{
    /* ROUTINE_DBG_MSP00 ("sp77_testUnsigned"); */
    char            buf[2000];
    int             i;
    int             findex;
    t_len    arg = 300;
    const tsp00_Char     *testFormat[c_unsignedFormats];

    testFormat[0] = "%0#10u";
    testFormat[1] = "%0#10X";
    testFormat[2] = "%0#10o";
    for (findex = 0; findex < c_unsignedFormats; ++findex) {
        for (i = OWN; i <= LIB; ++i) {
            sprintfF[i].f (buf, sizeof (buf), testFormat[findex], arg);
            fprintf (stdout, "%s: '%s'\n", origins[i], buf);
        }
    }
}

/*----------------------------------------*/

#define c_signedCases 5

static void
sp77_testSigned (void)
{
    /* ROUTINE_DBG_MSP00 ("sp77_testSigned"); */
    char            buf[2000];
    int             i;
    int             argIndex;
    int             arg[c_signedCases];
    const tsp00_Char     *testFormat = "%5.*d";

    arg[0] = 0;
    arg[1] = 1;
    arg[2] = -1;
    arg[3] = 100;
    arg[4] = -100;
    for (argIndex = 0; argIndex < c_signedCases; ++argIndex) {
        for (i = OWN; i <= LIB; ++i) {
            sprintfF[i].f (buf, sizeof (buf), testFormat, 3, arg[argIndex]);
            fprintf (stdout, "%s: '%s'\n", origins[i], buf);
        }
    }
}

/*----------------------------------------*/

#define c_doubleArgs 8
#define c_doubleFormats 9

static void
sp77_testDouble (void)
{
    /* ROUTINE_DBG_MSP00 ("sp77_testDouble"); */
    char            buf[2000];
    int             i;
    int             aindex;
    int             findex;
    double          args[c_doubleArgs];
    const tsp00_Char     *testFormat[c_doubleFormats];

    args[0] = 12345760.01234567;
    args[1] = 0.000001234567;
    args[2] = 12.3456789012345678901234567890;
    args[3] = 0.01234567;

    args[4] = -12345760.01234567;
    args[5] = -0.000001234567;
    args[6] = -12.3456789012345678901234567890;
    args[7] = -0.01234567;

    testFormat[0] = "%20.14f";
    testFormat[7] = "%f";
    testFormat[1] = "%e";
    testFormat[2] = "%g";
    testFormat[3] = "%#.0f";
    testFormat[4] = "%.0e";
    testFormat[5] = "%#.0g";
    testFormat[6] = "%.7g";
    testFormat[8] = "%20.14E";

    for (aindex = 0; aindex < c_doubleArgs; ++aindex) {
        for (findex = 0; findex < c_doubleFormats; ++findex) {
            for (i = OWN; i <= LIB; ++i) {
                sprintfF[i].f (buf, sizeof (buf), testFormat[findex], args[aindex]);
                fprintf (stdout, "%s: '%s'\n", origins[i], buf);
            }
        }
    }
}

/*----------------------------------------*/

const fillCharC = '-';

static void
sp77_checkBuffer (
    const tsp00_Char * buf,
    int size)
{
    int i;
    for (i = 0; i < size; ++i) {
        if (buf [i] != fillCharC) {
            fprintf (stdout, "mismatch at: %d\n", i);
        }
    }
}

/*----------------------------------------*/

static void
sp77_checkStrlen (
    const tsp00_Char * buf,
    size_t expectedSize)
{
    size_t buflen = strlen (buf);
    if (buflen != expectedSize) {
        fprintf (stdout, "len is %d; expected was %d\n", buflen, expectedSize);
    }
}

/*----------------------------------------*/

static void
sp77_testOverflow (void)
{
    char    TestArea[80];
    char   *underflowArea = &TestArea[0];
    char   *buf = &TestArea[10];
    char   *overflowArea = &TestArea[50];
    char   *endArea = &TestArea[79];
    int     printfResult;
    // #define xfiller "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                   /*1234567890123456789012345678901234567890*/
    #define xfiller "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    #define parameter "parameterString which is quite long and should overflow"

    fprintf (stdout, "sp77_testOverflow () >>>>>>>>>>>>>>>>>>\n");
    SAPDB_memset (underflowArea, fillCharC, 10);
    SAPDB_memset (buf, 'o', 40);
    SAPDB_memset (overflowArea, fillCharC, 29);
    SAPDB_memset (endArea, 0, 1 );
    printfResult = sp77sprintf (buf, 40, xfiller "%s", parameter);
    fprintf (stdout, "own: [%d] %s\n", (t_len)strlen (buf), buf);
    sp77_checkBuffer (underflowArea, 10);
    sp77_checkBuffer (overflowArea, 29);
    sp77_checkStrlen (buf, 39);

    SAPDB_memset (underflowArea, fillCharC, 10);
    SAPDB_memset (buf, 'o', 40);
    SAPDB_memset (overflowArea, fillCharC, 29);
    SAPDB_memset (endArea, 0, 1 );
#if defined(WIN32)
    _snprintf (buf, 40, xfiller "%s", parameter);
#else
    sprintf(buf, "%.40s", xfiller parameter);
#endif
    fprintf (stdout, "lib: [%d] %s\n", (t_len)strlen (buf), buf);
    fprintf (stdout, "<<<<<<<<<<<<<<<<<<<<< sp77_testOverflow ()\n");
    #undef xfiller
    #undef parameter
}

/*----------------------------------------*/

static void
sp77_testUnicode (void)
{
    /* ROUTINE_DBG_MSP00 ("sp77_testUnicode"); */
    tsp81_UCS2Char  uniBuf[100];
    const tsp00_Char     *someString = "12345678 String";
    int             len;
    char            buf[2000];
    const tsp00_Char     *format[7];
    int             i;
    const tsp77encoding * encoding[5];
    int             ii;
    int             outlen = 100;
    void           *buffer = (void *)uniBuf;
    t_len           bufferLength = 100;

    len = (int)strlen (someString);
    if ( sp77nativeUnicodeEncoding() == sp77encodingUCS2 )
    {
      sp77_UCS2AsciiCopy(&buffer, &bufferLength, someString, len);
    }
    else
    {
      sp77_UCS2SwappedAsciiCopy(&buffer, &bufferLength, someString, len);
    }

    uniBuf[len].s = 0;

    format[0] = "'%S'";
    format[1] = "'%20S'";
    format[2] = "'%.5S'";
    format[3] = "'%20.7S'";
    format[4] = "'%-'20.6S'";
    format[5] = "'%-20S'";
    format[6] = "'%=S'";


    encoding[0] = sp77encodingUCS2;
    encoding[1] = sp77encodingUCS2Swapped;
    encoding[2] = sp77encodingUCS2Native;
    encoding[3] = sp77encodingUTF8;
    encoding[4] = sp77encodingAscii;

    for ( ii = 0; ii < 5; ii++ )
    {
      printf("Unicode %s (string is UCS2 native)\n", encoding[ii]->whichEncodingClass);

      for (i = 0; i < 6; ++i)
      {
        outlen = sp77sprintfUnicode(encoding[ii], buf, sizeof (buf), format[i], uniBuf);
        if ( outlen < 0 )
        {
          fprintf(stdout, "Unicode conversion failed: Converted %d", -outlen);
        }
        else
        {
          fprintf(stdout, "%10s:", format[i]);
          fwrite (buf, outlen, 1, stdout);
        }
        fputc ('\n', stdout);
      }

      outlen = sp77sprintfUnicode(encoding[ii], buf, sizeof (buf), format[6], sp77nativeUnicodeEncoding(), uniBuf);
      if ( outlen < 0 )
      {
        fprintf(stdout, "Unicode conversion failed: Converted %d", -outlen);
      }
      else
      {
        fprintf(stdout, "%10s:", format[6]);
        fwrite (buf, outlen, 1, stdout);
      }
      fputc ('\n', stdout);
    }
}

/*----------------------------------------*/

static void
sp77_testRawdump (void)
{
    /* ROUTINE_DBG_MSP00 ("sp77_testUnicode"); */
    tsp81_UCS2Char  uniBuf[100];
    const tsp00_Char     *someString = "some Strüng";
    char            buf[2000];
    int             outlen = 100;

    fprintf(stdout, "RawDumping %%=r: %s\n", someString);

    sp77sprintfUnicode(sp77encodingUCS2, uniBuf, sizeof(uniBuf), "%s", someString);

    outlen = sp77sprintf(buf, sizeof(buf), "%=r", sp77encodingUCS2, uniBuf);
    if ( outlen < 0 )
    {
      fprintf(stdout, "Rawdump conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      fprintf(stdout, "UCS2       :'%s'\n", buf);
    }

    sp77sprintfUnicode(sp77encodingUCS2Swapped, uniBuf, sizeof(uniBuf), "%s", someString);

    outlen = sp77sprintf(buf, sizeof(buf), "%=r", sp77encodingUCS2, uniBuf);
    if ( outlen < 0 )
    {
      fprintf(stdout, "Rawdump conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      fprintf(stdout, "UCS2Swapped:'%s'\n", buf);
    }

    sp77sprintfUnicode(sp77encodingUCS2Native, uniBuf, sizeof(uniBuf), "%s", someString);

    outlen = sp77sprintf(buf, sizeof(buf), "%=r", sp77encodingUCS2, uniBuf);
    if ( outlen < 0 )
    {
      fprintf(stdout, "Rawdump conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      fprintf(stdout, "UCS2Native :'%s'\n", buf);
    }

    sp77sprintfUnicode(sp77encodingUTF8, uniBuf, sizeof(uniBuf), "%s", someString);

    outlen = sp77sprintf(buf, sizeof(buf), "%=r", sp77encodingUTF8, uniBuf);
    if ( outlen < 0 )
    {
      fprintf(stdout, "Rawdump conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      fprintf(stdout, "UTF8 :'%s'\n", buf);
    }

    outlen = sp77sprintf(buf, sizeof(buf), "%r", someString);
    if ( outlen < 0 )
    {
      fprintf(stdout, "Rawdump conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      fprintf(stdout, "Ascii:'%s'\n", buf);
    }

}

/*---------------------------------------------------------------------------*/

static void
sp77_testUTF8UCS2 (void)
{
    const tsp00_Char *someString = "German_äöüÄÖÜß";

    tsp00_Byte        utf8Buf[100];
    tsp00_Char        UCS2Buf[200];
    tsp00_Char        outBuf[100];
    int outlen;

    fprintf(stdout, "UTF8/UCS2 convertion test\n");

    outlen = sp77sprintf(outBuf, sizeof(outBuf), "%8s: %s (%r)", "Original", someString, someString);
    if ( outlen < 0 )
    {
      fprintf(stdout, "Orignal conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      fprintf(stdout, "%d:%s\n", outlen, outBuf);
    }

    outlen = sp77sprintfUnicode(sp77encodingUTF8, utf8Buf, sizeof(utf8Buf), "%s", someString);
    if ( outlen < 0 )
    {
      fprintf(stdout, "UTF8 conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      fprintf(stdout, "%d:%s\n", outlen, utf8Buf);
    }

    outlen = sp77sprintfUnicode(sp77encodingUCS2Native, UCS2Buf, sizeof(UCS2Buf), "%=.*s", sp77encodingUTF8, 9, utf8Buf);
    if ( outlen < 0 )
    {
      fprintf(stdout, "UTF8/UCS2 conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      sp77sprintf(outBuf, sizeof(outBuf), "(=.9s) %d:(%=r)", outlen, sp77encodingUCS2Native, UCS2Buf );
      fprintf(stdout, "%s\n", outBuf);
    }

    outlen = sp77sprintfUnicode(sp77encodingUCS2Native, UCS2Buf, sizeof(UCS2Buf), "%'=.*s", sp77encodingUTF8, 9, utf8Buf);
    if ( outlen < 0 )
    {
      fprintf(stdout, "UTF8/UCS2 conversion failed: Converted %d\n", -outlen);
    }
    else
    {
      sp77sprintf(outBuf, sizeof(outBuf), "('=.9s) %d:(%=r)", outlen, sp77encodingUCS2Native, UCS2Buf );
      fprintf(stdout, "%s\n", outBuf);
    }
}

/*---------------------------------------------------------------------------*/
static const tsp00_Byte someASCII[] = {
     'T',      'ä',      's',      't',   0 };
static const tsp00_Byte someUTF8[] = {
     'T', '\xC3','\xA4', 's',      't',   0 };
static const tsp00_Byte someUCS2[] = {
  0, 'T',   0, 'ä',   0, 's',   0, 't',   0, 0 };
static const tsp00_Byte someUCS2Swapped[] = {
 'T', 0,   'ä', 0,   's', 0,   't', 0,    0, 0 };

static struct {
  const tsp00_Byte *srcBuffer;
  unsigned int srcLength;
  unsigned int srcParsed;
  const tsp00_Byte *destBuffer;
  unsigned int destWritten;
} expectedResult[4*4] = {

  { &someASCII[0],        5,  5, &someASCII[0],       5 },
  { &someASCII[0],        5,  5, &someUCS2[0],        10 },
  { &someASCII[0],        5,  5, &someUCS2Swapped[0], 10 },
  { &someASCII[0],        5,  5, &someUTF8[0],        6 },
  { &someUCS2[0],        10, 10, &someASCII[0],       5 },
  { &someUCS2[0],        10, 10, &someUCS2[0],        10 },
  { &someUCS2[0],        10, 10, &someUCS2Swapped[0], 10 },
  { &someUCS2[0],        10, 10, &someUTF8[0],        6 },
  { &someUCS2Swapped[0], 10, 10, &someASCII[0],       5 },
  { &someUCS2Swapped[0], 10, 10, &someUCS2[0],        10 },
  { &someUCS2Swapped[0], 10, 10, &someUCS2Swapped[0], 10 },
  { &someUCS2Swapped[0], 10, 10, &someUTF8[0],        6 },
  { &someUTF8[0],         6,  6, &someASCII[0],       5 },
  { &someUTF8[0],         6,  6, &someUCS2[0],        10 },
  { &someUTF8[0],         6,  6, &someUCS2Swapped[0], 10 },
  { &someUTF8[0],         6,  6, &someUTF8[0],        6 }
};

void sp77_testStringConvert ()
{
  tsp78ConversionResult convertResult;
  const tsp77encoding *encodings[4];
  tsp00_Byte convertOutput[40];

  unsigned int srcParsed;
  unsigned int destWritten;
  int i,j;

  encodings[0] = sp77encodingAscii;
  encodings[1] = sp77encodingUCS2;
  encodings[2] = sp77encodingUCS2Swapped;
  encodings[3] = sp77encodingUTF8;

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      convertResult = sp78convertString(
        encodings[j],
        convertOutput,
        sizeof(convertOutput),
        &destWritten,
	false,
        encodings[i],
        expectedResult[i*4+j].srcBuffer,
        expectedResult[i*4+j].srcLength,
        &srcParsed);

      if ( convertResult != sp78_Ok )
      {
        printf("stringConvert from %s to %s ERROR: conversion failed\n",
          encodings[i]->whichEncodingClass,
          encodings[j]->whichEncodingClass
        );
        exit(1);
      }

      if ( srcParsed != expectedResult[i*4+j].srcParsed )
      {
        printf("stringConvert from %s to %s ERROR: srcParsed differs\n",
          encodings[i]->whichEncodingClass,
          encodings[j]->whichEncodingClass
        );
        exit(1);
      }

      if ( destWritten != expectedResult[i*4+j].destWritten )
      {
        printf("stringConvert from %s to %s ERROR: destWritten differs\n",
          encodings[i]->whichEncodingClass,
          encodings[j]->whichEncodingClass
        );
        exit(1);
      }

      if ( memcmp(convertOutput, expectedResult[4*i+j].destBuffer, destWritten) != 0 )
      {
        printf("stringConvert from %s to %s ERROR: convertResult differs\n",
          encodings[i]->whichEncodingClass,
          encodings[j]->whichEncodingClass
        );
        exit(1);
      }

      printf("stringConvert from %20s to %20s OK\n",
        encodings[i]->whichEncodingClass,
        encodings[j]->whichEncodingClass
      );
    }
  }
}

/*---------------------------------------------------------------------------*/

static void
sp77_testLongLong (void)
{
  /* ROUTINE_DBG_MSP00 ("sp77_testLongLong"); */

  int shiftCount = 0;
  tsp00_Uint8 aVeryLong;
  char buffer[40];

  aVeryLong = -1;
  for ( shiftCount = 1; shiftCount < 16; shiftCount++ )
  {
    aVeryLong *= 10;
  }

  sp77sprintf(buffer, 40, "%qd", aVeryLong);

  if ( strcmp(buffer, "-1000000000000000") != 0 )
  {
    printf("Signed long long conversion failed: Got %s expected %s\n",
      buffer, "-1000000000000000");
    exit(1);
  }
  else
  {
    printf("Signed long conversion OK\n");
  }

  aVeryLong = 1;
  for ( shiftCount = 1; shiftCount < 16; shiftCount++ )
  {
    aVeryLong *= 10;
  }

  sp77sprintf(buffer, 40, "%qu", aVeryLong);

  if ( strcmp(buffer, "1000000000000000") != 0 )
  {
    printf("Unsigned long long conversion failed: Got %s expected %s\n",
      buffer, "1000000000000000");
    exit(1);
  }
  else
  {
    printf("Unsigned long long conversion OK\n");
  }

  aVeryLong = -1;
  for ( shiftCount = 1; shiftCount < 40; shiftCount++ )
  {
    aVeryLong *= 2;
  }

  sp77sprintf(buffer, 40, "%qx", aVeryLong);

  if ( strcmp(buffer, "ffffff8000000000") != 0 )
  {
    printf("Hex long long conversion failed: Got %s expected %s\n",
      buffer, "ffffff8000000000");
    exit(1);
  }
  else
  {
    printf("Hex long long conversion OK\n");
  }

  sp77sprintf(buffer, 40, "%qX", aVeryLong);

  if ( strcmp(buffer, "FFFFFF8000000000") != 0 )
  {
    printf("Hex long long conversion (uppercase) failed: Got %s expected %s\n",
      buffer, "FFFFFF8000000000");
    exit(1);
  }
  else
  {
    printf("Hex long long conversion (uppercase) OK\n");
  }
}

/*---------------------------------------------------------------------------*/

static void
sp77_testEncoding (void)
{
  /* ROUTINE_DBG_MSP00 ("sp77_testEncoding"); */

  static tsp00_Byte someSpaces[] = { '\0', ' ', '\0', '\t', '\0', '\n', '\0', 'x', '\0' };
  static tsp00_Byte someNonAscii[] = { '\xC3', '\xA4' };

  if ( !sp77encodingAscii->isAscii7(&someSpaces[1])
    ||  sp77encodingAscii->isAscii7(&someNonAscii) )
  {
    printf("isAscii7 for Ascii wrong\n");
    exit(1);
  }

  if ( !sp77encodingUCS2->isAscii7(&someSpaces[0])
    ||  sp77encodingUCS2->isAscii7(&someNonAscii) )
  {
    printf("isAscii7 for UCS2 wrong\n");
    exit(1);
  }

  if ( !sp77encodingUCS2Swapped->isAscii7(&someSpaces[1])
    ||  sp77encodingUCS2Swapped->isAscii7(&someNonAscii) )
  {
    printf("isAscii7 for UCS2Swapped wrong\n");
    exit(1);
  }

  if ( !sp77encodingUTF8->isAscii7(&someSpaces[1])
    ||  sp77encodingUTF8->isAscii7(&someNonAscii) )
  {
    printf("isAscii7 for UTF8 wrong\n");
    exit(1);
  }

  printf("isAscii7 works properly\n");

  if ( !sp77encodingAscii->isBlank(&someSpaces[1])
    || !sp77encodingAscii->isBlank(&someSpaces[3])
    ||  sp77encodingAscii->isBlank(&someSpaces[5]) )
  {
    printf("isBlank for Ascii wrong\n");
    exit(1);
  }

  if ( !sp77encodingUCS2->isBlank(&someSpaces[0])
    || !sp77encodingUCS2->isBlank(&someSpaces[2])
    ||  sp77encodingUCS2->isBlank(&someSpaces[4]) )
  {
    printf("isBlank for UCS2 wrong\n");
    exit(1);
  }

  if ( !sp77encodingUCS2Swapped->isBlank(&someSpaces[1])
    || !sp77encodingUCS2Swapped->isBlank(&someSpaces[3])
    ||  sp77encodingUCS2Swapped->isBlank(&someSpaces[5]) )
  {
    printf("isBlank for UCS2Swapped wrong\n");
    exit(1);
  }

  if ( !sp77encodingUTF8->isBlank(&someSpaces[1])
    || !sp77encodingUTF8->isBlank(&someSpaces[3])
    ||  sp77encodingUTF8->isBlank(&someSpaces[5]) )
  {
    printf("isBlank for UTF8 wrong\n");
    exit(1);
  }

  printf("isBlank works properly\n");

  if ( !sp77encodingAscii->isSpace(&someSpaces[1])
    || !sp77encodingAscii->isSpace(&someSpaces[3])
    || !sp77encodingAscii->isSpace(&someSpaces[5])
    ||  sp77encodingAscii->isSpace(&someSpaces[7]) )
  {
    printf("isSpace for Ascii wrong\n");
    exit(1);
  }

  if ( !sp77encodingUCS2->isSpace(&someSpaces[0])
    || !sp77encodingUCS2->isSpace(&someSpaces[2])
    || !sp77encodingUCS2->isSpace(&someSpaces[4])
    ||  sp77encodingUCS2->isSpace(&someSpaces[6]) )
  {
    printf("isSpace for UCS2 wrong\n");
    exit(1);
  }

  if ( !sp77encodingUCS2Swapped->isSpace(&someSpaces[1])
    || !sp77encodingUCS2Swapped->isSpace(&someSpaces[3])
    || !sp77encodingUCS2Swapped->isSpace(&someSpaces[5])
    ||  sp77encodingUCS2Swapped->isSpace(&someSpaces[7]) )
  {
    printf("isSpace for UCS2Swapped wrong\n");
    exit(1);
  }

  if ( !sp77encodingUTF8->isSpace(&someSpaces[1])
    || !sp77encodingUTF8->isSpace(&someSpaces[3])
    || !sp77encodingUTF8->isSpace(&someSpaces[5])
    ||  sp77encodingUTF8->isSpace(&someSpaces[7]) )
  {
    printf("isSpace for UTF8 wrong\n");
    exit(1);
  }

  printf("isSpace works properly\n");

  if (  sp77encodingAscii->isControl(&someSpaces[1])
    || !sp77encodingAscii->isControl(&someSpaces[3])
    || !sp77encodingAscii->isControl(&someSpaces[5])
    ||  sp77encodingAscii->isControl(&someSpaces[7]) )
  {
    printf("isControl for Ascii wrong\n");
    exit(1);
  }

  if (  sp77encodingUCS2->isControl(&someSpaces[0])
    || !sp77encodingUCS2->isControl(&someSpaces[2])
    || !sp77encodingUCS2->isControl(&someSpaces[4])
    ||  sp77encodingUCS2->isControl(&someSpaces[6]) )
  {
    printf("isControl for UCS2 wrong\n");
    exit(1);
  }

  if (  sp77encodingUCS2Swapped->isControl(&someSpaces[1])
    || !sp77encodingUCS2Swapped->isControl(&someSpaces[3])
    || !sp77encodingUCS2Swapped->isControl(&someSpaces[5])
    ||  sp77encodingUCS2Swapped->isControl(&someSpaces[7]) )
  {
    printf("isControl for UCS2Swapped wrong\n");
    exit(1);
  }

  if (  sp77encodingUTF8->isControl(&someSpaces[1])
    || !sp77encodingUTF8->isControl(&someSpaces[3])
    || !sp77encodingUTF8->isControl(&someSpaces[5])
    ||  sp77encodingUTF8->isControl(&someSpaces[7]) )
  {
    printf("isControl for UTF8 wrong\n");
    exit(1);
  }

  printf("isControl works properly\n");

}


/*---------------------------------------------------------------------------*/

static void
sp77_testPadding (void)
{
  /* ROUTINE_DBG_MSP00 ("sp77_testPadding"); */

  static tsp00_Byte somePadded[] = { "1234XXXX" };
  static tsp00_Byte someUCS2Padded[] = { "\0001\0002\0003\0004\000X\000X\000X\000X" };
  static tsp00_Byte someUCS2SwappedPadded[] = { "1\0002\0003\0004\000X\000X\000X\000X\000" };

  if ( sp77encodingAscii->countPadChars(somePadded, 8, 'X') != 4 )
  {
    printf("countPadChars for Ascii wrong\n");
    exit(1);
  }

  if ( sp77encodingUTF8->countPadChars(somePadded, 8, 'X') != 4 )
  {
    printf("countPadChars for UTF8 wrong\n");
    exit(1);
  }

  if ( sp77encodingUCS2->countPadChars(someUCS2Padded, 16, 'X') != 4 )
  {
    printf("countPadChars for UCS2 wrong\n");
    exit(1);
  }

  if ( sp77encodingUCS2Swapped->countPadChars(someUCS2SwappedPadded, 16, 'X') != 4 )
  {
    printf("countPadChars for UCS2Swapped wrong\n");
    exit(1);
  }
  printf("countPadChars works properly\n");

}

/*---------------------------------------------------------------------------*/

int
main ()
{
    /* ROUTINE_DBG_MSP00 ("main"); */
    stdTrace = fopen ("testf.prt", "w");
    sp77_testLongLong ();
    sp77_testStringConvert ();
    TestUppercase();
    sp77_testUTF8UCS2();
    sp77_testStrings ();
    sp77_testChars ();
    sp77_testUnsigned ();
    sp77_testSigned ();
    sp77_testDouble();
    sp77_testOverflow ();
    sp77_testUnicode ();
    sp77_testRawdump ();
    sp77_testEncoding ();
    sp77_testPadding ();
    /* fprintf(stdout, "All tests passed\n"); */
    fclose (stdTrace);
    return 0;
}
#endif

