/*!
  @file           veo602.c
  @author         JoergM
  @special area   Unicode Common LZU Extensions
  @brief          Contains all unicode extensions shared by kernel and client LZU
  @first created  000-08-04  20:12
  @see            example.html ...


  Currently contained
  eo602_ConvertPascalStringToUTF8Unicode()

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo602.h"
#include "hsp77.h"
#include "SAPDBCommon/SAPDB_string.h"
#if defined(WIN32)
#include <tchar.h>
#endif
#include <locale.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*
  Fixed LOCALE value used...
  Origin: SAP R/3 National Language support...
  In case of problems check agains file:
      xxxxxx.wdf.sap-ag.de:/bas/BIN/src/flat/nlsui1.c
 */

#if defined(AIX)
#define START_LOCALE_EO602 "EN_US.UTF-8"
#elif defined(OSF1)
#define START_LOCALE_EO602 "universal.utf8@ucs4"
#elif defined(HPUX)
#define START_LOCALE_EO602 "univ.utf8"
#elif defined(SUN) || defined(SOLARIS)
#define START_LOCALE_EO602 "en_US.UTF-8"
#elif defined(FREEBSD)
#define START_LOCALE_EO602 "en_US.ISO8859-1"
#elif defined(LINUX)
# if defined(S390X) || defined(SDBonPPC64) || defined(X86_64) || defined(HAS_NPTL)
#  define START_LOCALE_EO602 "en_US.utf8"
# else
#  define START_LOCALE_EO602 "en_US.ISO-8859-1"
# endif
#elif defined(NMP)
#define START_LOCALE_EO602 "en_US.ISO-8859-1"
#elif defined(WIN32)
#define START_LOCALE_EO602 "englisch"
#else
#define START_LOCALE Unknown OS! UTF8 LOCALE not supported !!!
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void eo602_setConsoleEncoding();

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

static const tsp77encoding *eo602_consoleEncoding = 0;

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

tsp78ConversionResult eo602_ConvertPascalStringToUTF8Unicode (
          tsp00_Int4  msgEncoding ,
    const tsp00_Byte *msgTextPascal ,
          tsp00_Uint4 msgTextSizeInBytes ,
          tsp00_Byte *bufferUTF8 ,
          tsp00_Uint4 bufferSizeInBytes )
{
  tsp78ConversionResult result = sp78_Ok;
  register const tsp00_Byte *spaceCharPtr;
                 tsp00_Uint4 stepWidth;
                 tsp00_Uint4 trimmedTextSizeInBytes;

  if ( msgEncoding == csp_unicode_native )
  {
    msgEncoding = sp77nativeUnicodeEncoding()->EncodingType;
  }

  /* Find stepWidth and set spaceCharPtr to last possible space position */
  switch( msgEncoding )
  {
  case csp_unicode:
    stepWidth = 2;
    spaceCharPtr = msgTextPascal + msgTextSizeInBytes - 1;
    break;
  case csp_unicode_swap:
    stepWidth = 2;
    spaceCharPtr = msgTextPascal + msgTextSizeInBytes - stepWidth;
    break;
  default:
    stepWidth = 1;
    spaceCharPtr = msgTextPascal + msgTextSizeInBytes - 1;
    break;
  }

  trimmedTextSizeInBytes = msgTextSizeInBytes;

  /* Loop until begin of string or no more spaces found */
  while ( (spaceCharPtr >= msgTextPascal)
       && (*spaceCharPtr == ' ') )
  {
    spaceCharPtr -= stepWidth;
    trimmedTextSizeInBytes -= stepWidth;
  }

  if ( trimmedTextSizeInBytes > 0 )
  {
    if ( msgEncoding != csp_unicode && msgEncoding != csp_unicode_swap )
    {
      /* Prevent UTF8 buffer overflow... */
      if ( trimmedTextSizeInBytes > bufferSizeInBytes )
      {
        trimmedTextSizeInBytes = bufferSizeInBytes - 1;
      }

      /* We trust the caller... ASCII-7bit and UTF8 are simply copied */
      SAPDB_memcpy(bufferUTF8, msgTextPascal, trimmedTextSizeInBytes);

      /* Terminate buffer with zero */
      bufferUTF8[trimmedTextSizeInBytes] = 0;
    }
    else
    {
      const tsp77encoding *UCS2encoding;
      tsp00_Uint4 srcBytesParsed;
      tsp00_Uint4 destBytesWritten;

      UCS2encoding = ( msgEncoding == csp_unicode_swap
                     ? sp77encodingUCS2Swapped : sp77encodingUCS2 );

      result = sp78convertString( sp77encodingUTF8,
                                  bufferUTF8,
                                  bufferSizeInBytes,
                                  &destBytesWritten,
                                  true, /* Make sure result is 0 terminated */
                                  UCS2encoding,
                                  msgTextPascal,
                                  trimmedTextSizeInBytes,
                                  &srcBytesParsed );
      bufferUTF8[bufferSizeInBytes-1] = 0;
    }
  }
  else
  {
    /* only spaces found, so we return an empty string */
    bufferUTF8[0] = 0;
  }

  return result;
}

/*---------------------------------------------------------------------------*/

const tsp77encoding * eo602getConsoleEncoding()
{
  eo602_setConsoleEncoding();

  return eo602_consoleEncoding;
}

/*---------------------------------------------------------------------------*/

void eo602setStartLocale()
{
  eo602_setConsoleEncoding();

  setlocale(LC_CTYPE, START_LOCALE_EO602);
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

static void eo602_setConsoleEncoding()
{
  if ( !eo602_consoleEncoding )
  {
#if defined(WIN32)
    eo602_consoleEncoding = sp77encodingUCS2Native;
#else
    /*
       This call retrieves the current locale setting, even if named setlocale...
       The interface specification says: If second parameter is NULL the current
       setting is returned...
     */
    char * consoleLocale = setlocale(LC_CTYPE, NULL);
    /*
      Fixed LOCALE value used...
     */
    if ( strcmp(consoleLocale, START_LOCALE_EO602) == 0 )
    {
      eo602_consoleEncoding = sp77encodingUTF8;
    }
    else
    {
      eo602_consoleEncoding = NULL;
    }
#endif
  }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
