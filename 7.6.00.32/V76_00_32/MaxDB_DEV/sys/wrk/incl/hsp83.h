/*!**************************************************************************

  module       : hsp83.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Unicode support
  description : UTF8 Conversion

  Pure C version of UTF8 conversion

  last changed: 2000-09-12  21:20
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


#ifndef HSP83_H
#define HSP83_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hsp81.h"

 /*!
  Declaration: sp83UTF8Convert_ConversionResult
  Description: Result of UTF8 Conversion functions

  sp83UTF8Convert_Success conversion successful

  sp83UTF8Convert_SourceExhausted partial character in source, but hit end

  sp83UTF8Convert_SourceCorrupted source contains invalid chars or char sequences

  sp83UTF8Convert_TargetExhausted insuff. room in target for conversion
 */
typedef enum 
{
  sp83UTF8Convert_Success,
  sp83UTF8Convert_SourceExhausted,
  sp83UTF8Convert_SourceCorrupted,
  sp83UTF8Convert_TargetExhausted,
  sp83UTF8Convert_Error
} tsp83UTF8_ConversionResult;
/*! EndDeclaration */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
  Function: sp83UTF8ConvertFromUCS2
  Description: Convertion from UCS2 to UTF8

  End pointer and 'At' pointer always point behind last element of buffer or result

  Arguments:  srcBeg [in] Pointer to first UCS2 Character
              srcEnd [in] Pointer to last+1 UCS2 Character
              srcAt  [out] Returned pointer to last+1 scanned UCS2 Character
              srcSwapped [in] UCS2 characters are swapped
             destBeg [in] Pointer to first UTF8 Character
             destEnd [in] Pointer to last+1 UTF8 Character
             destAt  [out] Returned pointer to last+1 written UTF8 Character
  Return value: Conversion result
 */
externC tsp83UTF8_ConversionResult sp83UTF8ConvertFromUCS2(
  const tsp81_UCS2Char *  srcBeg,
  const tsp81_UCS2Char *  srcEnd,
        tsp81_UCS2Char const ** srcAt,
  int                     srcSwapped,
  tsp00_Byte           *  destBeg,
  tsp00_Byte           *  destEnd,
  tsp00_Byte           ** destAt
);

/*!
  Function: sp83UTF8ConvertToUCS2
  Description: Convertion from UTF8 to UCS2

  End pointer and 'At' pointer always point behind last element of buffer or result

  Arguments:  srcBeg [in] Pointer to first UTF8 Character
              srcEnd [in] Pointer to last+1 UTF8 Character
              srcAt  [in] Returned pointer to last+1 scanned UTF8 Character
             destBeg [in] Pointer to first UCS2 Character
             destEnd [in] Pointer to last+1 UCS2 Character
             destAt  [in] Returned pointer to last+1 written UCS2 Character
             destSwapped [in] UCS2 characters are to be swapped
  Return value: Conversion result
 */
externC tsp83UTF8_ConversionResult sp83UTF8ConvertToUCS2(
  const tsp00_Byte       *  srcBeg,
  const tsp00_Byte       *  srcEnd,
        tsp00_Byte const **  srcAt,
  tsp81_UCS2Char         *  destBeg,
  tsp81_UCS2Char         *  destEnd,
  tsp81_UCS2Char         ** destAt,
  int                       destSwapped
);

/*!
  Function: sp83UTF8_Bytelen
  Description: Calculate number of UTF8 Character in given UTF8 string

  Additional the number of bytes is returned, which may differ from number
  of characters. If srcLimit is specified large enaugh the routine replaces
  an UTF8 specific strlen...

  Arguments:  srcBeg [in] Pointer to first UTF8 Character
              srcLimit [in] Maximum number of bytes to parse
              charCountUTF8 [out] Number of UTF8 characters parsed
              byteCountUTF8 [out] Number of bytes parsed
  Return value: Conversion result
 */
externC tsp83UTF8_ConversionResult sp83UTF8_Bytelen(const tsp00_Byte * srcBeg,
                                                       const unsigned int srcLimit,
                                                       unsigned int *charCountUTF8,
                                                       unsigned int *byteCountUTF8);

/*!
  Function: sp83UTF8fromASCII
  Description: Convert ASCII ISO8859-LATIN1 8-Bit character string in UTF8 string

  All ASCII characters have a valid counterpart in UTF8, but size differs for characters
  with code > 127. Therefore target buffer must be twice as large as source length for 
  security reason...

  Arguments:  srcBeg [in] Pointer to first ASCII Character
              srcLimit [in] Maximum number of ASCII Character to parse
              srcBytesParsed [out] Number of bytes parsed from the source buffer
              destBeg [in] Pointer to target buffer for UTF8 characters
              destLimit [in] target buffer size (including ZERO termination)
              byteCountUTF8 [out] Number of bytes written in target buffer
  Return value: Conversion result
 */
externC tsp83UTF8_ConversionResult sp83UTF8fromASCII(const tsp00_Char * srcBeg,
                                                        const unsigned int srcLimit,
                                                        unsigned int *srcBytesParsed,
                                                              tsp00_Byte * destBeg,
                                                        const unsigned int destLimit,
                                                        unsigned int *byteCountUTF8);

/*!
  Function: sp83UTF8toASCII
  Description: Convert UTF8 into ISO8859-LATIN1 8-Bit ASCII character

  If UTF8 character found in input stream, that is not contained in 
  ISO8859-LATIN1 the replaceChar is used instead. The number of replacedChars
  is returned.

  Arguments:  srcBeg [in] Pointer to first UTF8 Character
              srcLimit [in] Maximum number of UTF8 Character to parse
              destBeg [in] Pointer to ASCII target buffer
              destLimit [in] Maximum number of ASCII Character to write (including ZERO termination)
              replaceChar [in] Character to be used for replacement of unknown UTF8 chars or 0
              replaceCount [out] Number of characters replaced in output stream
              byteCountASCII [out] Number of bytes written
  Return value: Conversion result
 */
externC tsp83UTF8_ConversionResult sp83UTF8toASCII(const tsp00_Byte * srcBeg,
                                                      const unsigned int srcLimit,
                                                      unsigned int *srcBytesParsed,
                                                            tsp00_Char * destBeg,
                                                      const unsigned int destLimit,
                                                      const tsp00_Char replaceChar,
                                                      unsigned int *replaceCount,
                                                      unsigned int *byteCountASCII);

/*!
  Function: sp83UTF8StringInfo
  Description: Functions that report encoding specific information about UTF8 C-strings

  Typecheck for first parameter is not enforced (void *) to be compatible with tsp77encoding
  function pointer member stringInfo!!!

  UTF8-C-Strings means that a string is terminated by a zero! If no
  zero is found within byteLength, the byteLength terminates the
  UTF8-C-String.

  Returned character and byte count differ depending on the characters found in string.
  Counts reported do never contains the termination zero character byte.
  Any partial character is also not contained.

  pIsTerminated is pure information, usefull to decide whether to add own termination if needed

  pIsExhausted is reported may be used as warning, but if set 'false' is returned.

  pIsCorrupted is an error and if set 'false' is returned.

  Even if an error was detected, pCharCount and pByteParsed are always valid.

  Arguments: buffer [in] Buffer containing input string

             bufferLength [in] Size of buffer (not necessarily string length!!!)

             bufferLengthIsInBytes [in] if true length is in bytes else in chars

             pCharCount [out] Number of character in buffer up to termination

             pBytesParsed [out] Number of bytes in buffer parsed up to termination

             pIsTerminated [out] Flag: Terminated by a compete ZERO terminator

             pIsCorrupted [out] Flag: Input character not in valid encoding

             pIsExhausted [out] Flag: Last input character was only partial

  Return value: true if no error detected, false else
*/
externC int sp83UTF8StringInfo( const void *buffer, 
                                unsigned int bufferLength, 
                                int bufferLengthIsInBytes, 
                                unsigned int *pCharCount, 
                                unsigned int *pByteCount, 
                                int *pIsTerminated, 
                                int *pIsCorrupted, 
                                int *pIsExhausted);

/*!
  Function: sp83UTF8StringComplete
  Description: Functions that report the length of all complete characters in bytes

  Zero termination characters are ignored by this routine.
  It only looks at the end of the buffer and tries to find
  the first introduction character. If found the number of
  bytes needed to form this character is known and compared
  with the number of bytes parsed. If not enough bytes in the
  buffer false is returned and the number of bytes up to the
  introduction character is returned.

  This routine expects a non corrupted source buffer. If the buffer
  is corrupted, completeLengthInBytes return value is undefined.

  Arguments: buffer [in] Buffer containing input string

             bufferLengthInBytes [in] Size of buffer in bytes

             completeLengthInBytes [out] Number of bytes in buffer includinglast complete UTF8 char

  Return value: true if buffer contained only complete chars, false else
*/
externC int sp83UTF8StringComplete( const void *buffer, 
                                unsigned int bufferLengthInBytes, 
                                unsigned int *completeLengthInBytes);

/*!
  Function: sp83UTF8StringToUpper
  Description: Convert all UTF8 string character in place to upper case
  Arguments: srcBeg [in] Pointer to begin of buffer
             srcAt [out] Pointer behin last character parsed
             bufferLengthInBytes [in] Buffer length in bytes NOT characters
  Return value: Conversion result
 */
externC tsp83UTF8_ConversionResult sp83UTF8StringToUpper( tsp00_Byte *srcBeg, 
                                                          tsp00_Byte **srcAt,
                                                          unsigned int bufferLengthInBytes );
 
/*!
  Function: sp83UTF8StringToLower
  Description: Convert all UTF8 string character in place to lower case
  Arguments: srcBeg [in] Pointer to begin of buffer
             srcAt [out] Pointer behin last character parsed
             bufferLengthInBytes [in] Buffer length in bytes NOT characters
  Return value: Conversion result
 */
externC tsp83UTF8_ConversionResult sp83UTF8StringToLower( tsp00_Byte *srcBeg, 
                                                          tsp00_Byte **srcAt,
                                                          unsigned int bufferLengthInBytes );

/*!
  Function: sp83UTF8ArrayFromUCS2Array
  Description: Convert all UCS2 strings given into UTF8 string array

  Usefull helper routine to convert argv[] or envp[] arrays...

  Arguments: myArgc [in] Number of UCS2 strings to convert
             UCS2argv [in] Pointer to array of UCS2 strings
             pReturnedUTF8argv [out] Returned array (myArgc+1) entries, last entry NULL
  Return value: true if ok, false if conversion fails or no memory. 
             If call failed, *pReturnedUTF8argv is set to NULL.
 */
externC int sp83UTF8ArrayFromUCS2Array (
 int               myArgc, 
 tsp81_UCS2Char ** UCS2argv,
 char          *** pReturnedUTF8argv );

#endif  /* HSP83_H */
