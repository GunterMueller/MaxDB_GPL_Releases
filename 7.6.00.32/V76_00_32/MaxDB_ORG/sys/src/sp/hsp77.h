/*!***************************************************************************

  module:        hsp77.h

  -------------------------------------------------------------------------

  responsible:   DanielD, JoergM

  special area:  sprintf functions interface

  description:   Replacement for ANSI C sprintf and vsprintf functions

  Format description: %[FLAGS][WIDTH][.PREC][SIZE][TYPE]

  FLAGS,SIZE and TYPE are described below. WIDTH and PREC are the width
  and precision specifier. These are either positiv integer numbers or
  asterik '*'. Each asterik needs an additional integer argument that is
  used as width or precision respectivly. Note that precision is separated
  by a single dot.

  Recognized format FLAGS:

  - left padding

  + sign wanted. Influences %d,%i,%u,%e,%E,%f,%F,%g,%G

  Space(' ') Padding uses spaces

  0 Padding uses 0

  = (1 additional argument parsed) input encoding for string. Influences %s,%S,%r,%R

  ' interprets all width and precision specification as bytes instead of character

  # Use alternate form. %o force leading '0', %x force leading '0x', %X force leading
    '0X', %e,%E,%f,%g and %G force decimal point even if result would contain no
    decimal point.


  Recognized format SIZE:

  h force %d,%i,%o,%u,%x and %X to apply to a short or unsigned short

  l force %d,%i,%o,%u,%x and %X to apply to a long or unsigned long

  L force %d,%i,%o,%u,%x and %X to apply to a long or unsigned long

  q force %d,%i,%o,%u,%x and %X to apply to a long long or
    unsigned long long (quad word)

  Not implemented... L force %e,%E,%f,%g and %G to apply to a long double argument


  Recognized format TYPE:

  %c (char) Character

  %d (int) Integer

  %e (double) Scientific format number (small letter e)
  %E (double) Scientific format number (capital letter E)

  %f (double) Floating point number
  %F (double) Floating point number

  %g (double) Floating point or scientific format number (small letter e)
  %G (double) Floating point or scientific format number (capital letter E)

  %i (int) Integer

  %n (pointer to short (%hn) , int (%n) or long (%ln) ) Parameter is a pointer to
     a place where number of so far parsed bytes are stored (output not influenced)

  %o (unsigned) Octal number

  %p (void *) Pointer (unsigned long) as hex number

  %r (string) String hexdump (small letters)
  %R (string) String hexdump (capital letters)

  %s (string) Character string (Default ASCII)
  %S (string) Character string (Default UCS2Native)

  %u (unsigned) Unsigned integer

  %x (unsigned) Hexadecimal number (small letters)
  %X (unsigned) Hexadecimal number (capital letters)

  last changed: 2000-12-07  15:22
  Original Author: DanielD

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//


*****************************************************************************/

#ifndef HSP77_H
#define HSP77_H

#include <stdarg.h>

#include "geo00.h"

#if defined(OSF1) && defined(__cplusplus)
#pragma message disable noctobutconrefm
#endif

#if defined(__cplusplus)
#define externCGlobal extern "C"
#else
#define externCGlobal extern
#endif

/*!
  Chapter: Character constants
 */

/*!
  Declaration: tsp77charConst
  Description: encoding independant representation of a character.

    Use as in SAPDB_memcpy (target, charConst->bytes, charConst->byteCount).
 */
typedef struct t_sp77charConst {
    int byteCount;
    char bytes [4];
} tsp77charConst;
/*! EndDeclaration */

/*!
    Function: sp77charIsEqual
    Description: compares raw bytes with a character constant
    returns:

        - TRUE when then bytes in <Param>buf</> are equivalent
            to <Param>charConst</>
        - FALSE otherwise
 */
externC teo00_Bool sp77charIsEqual (
    const tsp77charConst * charConst,
    const void           * buf,
    int                    bufLen);

/*!
    Function: sp77charMove
    Description: copies a charater into <Param>buf</>
    returns:
        - number of bytes copied if successful
        - -1 if <Param>buf</> is too small
 */
externC int sp77charMove (
    void * buf,
    int    bufLen,
    const tsp77charConst * charConst);

/*!
  Declaration: tsp77charTable
  Description: a collection of commonly used character constants

    - indicator: used at the start of files to detect the encoding
    - terminator: the null character used to signal the end of a string
    - nl: new line
    - cr: carriage return
    - blank: the character
 */
typedef struct t_sp77charTable {
    const tsp77charConst indicator;
    const tsp77charConst terminator;
    const tsp77charConst nl;
    const tsp77charConst cr;
    const tsp77charConst blank;
    const tsp77charConst dosEOF;
}tsp77charTable;
/*!
  EndChapter: Character constants
 */
/*!
  Chapter: tsp77encoding_Structure
  Description: Structure describing a character encoding

  An encoding contains all features and member functions needed
  for sprintf to work properly except conversion between other encodings.
 */
typedef struct t_sp77encoding {
  /*!
    Decleration: encodingClass
    Description: Identifier which is identical for all encoding classes

    This allows to automatically detect wrong or overwritten
    encoding arguments
   */
  const char **encodingClass;

  /*!
    Decleration: whichEncodingClass
    Description: Identifier which is unique for this encoding classes

    This is always a zero terminated 7bit ASCII string, that allows
    easy trace outputs too.
   */
  const char *whichEncodingClass;

  /*!
    Decleration: EncodingType
    Description: Identifier which is used as input encoding type argument
   */
  const int EncodingType;

  /*!
    Function: asciiCopy
    Description: Copy given ascii characters (Literal) in target buffer.

    Adjusts target buffer pointer and target buffer length

    Arguments: target [inout] the target buffer
               targetLength [inout] the target buffer length
               literal [in] the ascii literal
               literalLength [in]length in bytes
    Return value: true on error, false on success */
  int (*asciiCopy) (void **target,
                    unsigned int *targetLength,
                    const char *literal,
                    unsigned int literalLength);

  /*!
    Function: stringInfo
    Description: Retrieve information about given string

    String is handled as a C_String, that is a termination character
    sequence stops parsing.

    Arguments: buffer [in] the string buffer to parse
               bufferLength [in] the buffer length in bytes or characters
               bufferLengthIsInBytes [in] if true bufferLength is in bytes
               charLength [out] the number of characters found (excluding ZERO terminator)
               byteLength [out] the number of bytes found (excluding ZERO terminator)
               isTerminated [out] if true string is zero terminated
               isCorrupted [out] if true string contains corrupted chars
               isExhausted [out] if true string has partial last char
    Return value: true if corrupted or exhausted, false is ok */

  int (*stringInfo) (const void *buffer,
                     unsigned int bufferLength,
                     int bufferLengthIsInBytes,
                     unsigned int *charLength,
                     unsigned int *byteLength,
                     int  *isTerminated,
                     int *isCorrupted,
                     int *isExhausted);

  /*!
    Function: stringComplete
    Description: Test the last character in the buffer for completeness

    Since some encodings allow characters to consist of mulitbyte sequences,
    this routine allows to check for completeness of these sequences. Usefull
    if parsing an input stream.
    String is NOT handled as a C_String, that is termination character are ignored

    Arguments: buffer [in] the string buffer to parse
               bufferLengthInBytes [in] the buffer length in bytes
               completeLengthInBytes [out] the length in bytes of all complete characters
    Return value: true if last character only partial, false else
   */
  int (*stringComplete) (const void *buffer,
                         unsigned int bufferLengthInBytes,
                         unsigned int *completeLengthInBytes);

  /*!
    Function: fillString
    Description: Fill a target buffer with given number of padding characters

    Adjust target buffer pointer and target buffer length
    Arguments: target [inout] pointer to pointer to current target buffer position (updated)
               targetLength [inout] pointer to remaining target buffer size (updated)
               padLength [in] number of padding characters to fill
               padChar [in] the padding character
    Return value: none
   */
  void (*fillString) (void **target,
                      unsigned int *targetLength,
                      unsigned int padLength,
                      char padChar);

  /* PTS 1108620 */
  /*!
    Function: countPadChars
    Description: Count the padding character at end of given buffer

    The padding character is expanded to the given encoding and the number of these
    padding character found is returned.

    Arguments: target [in] pointer to current target buffer
               targetLength [in] target buffer size (in bytes)
               padChar [in] the padding character
    Return value: number of padding characters (not bytes!) found at end of buffer
   */
  unsigned int (*countPadChars) ( void *target,
                                  unsigned int targetLength,
                                  char padChar);

  /*!
    Function: isAscii7
    Description: Test on character being representing a 7bit ASCII character

    Test the given character on being a 7bit Ascii character
    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a 7bit ASCII character, false else
   */
  int (*isAscii7) (const void *testCharacter);

  /*!
    Function: isBlank
    Description: Test the given character on being a tab or space.

    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a blank, false else
   */
  int (*isBlank) (const void *testCharacter);

  /*!
    Function: isSpace
    Description: Test the given character on being a whitespace.

    A whitespace is one out of (space,tab,newline,formfeed,carriage return)

    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a whitespace, false else
   */
  int (*isSpace) (const void *testCharacter);

  /*!
    Function: isControl
    Description: Test the given character on being a control character.

    Arguments: testCharacter [in] Pointer to chracter to be tested
    Return value: true if is a control character, false else
   */
  int (*isControl) (const void *testCharacter);

  /*!
    Declaration: fixedCharacterSize
    Description: Fixed character size (0 if variable character size)
   */
  int      fixedCharacterSize;

  /*!
    Declaration: terminatorSize
    Description: Terminator character size (0 if no terminator character for this encoding)
   */
  int      terminatorSize;
  /*!
    Declaration: charTable
    Description: commonly used character constants
   */
  const tsp77charTable * const charTable;
} tsp77encoding;
/*!
  Endchapter: tsp77encoding_Structure
 */

/*!
  Chapter: Encodings
 */
/*!
  Declaration: sp77encodingAscii
  Description: 8Bit Ascii String encoding

  Character strings as 8bit Ascii
 */
externCGlobal const tsp77encoding * const sp77encodingAscii;
/*!
  Declaration: sp77encodingUCS2
  Description: SAPDB UCS2 String encoding

  SAPDB interpretation of UCS2: HighByte,LowByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS2;
/*!
  Declaration: sp77encodingUCS2Swapped
  Description: SAPDB swapped UCS2 encoding

  SAPDB interpretation of swapped UCS2: LowByte,HighByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS2Swapped;
/*!
  Declaration: sp77encodingUCS2Native
  Description: Machine dependend UCS2 encoding

  Native (machine dependend UCS2): Machine byte order
 */
/* PTS 1110303 */
#if defined(WIN32) || (defined(LINUX) && !(defined(S390X)||defined(SDBonPPC64))) || defined(OSF1) || defined (FREEBSD) || (defined (SOLARIS) && (defined(I386) || defined(X86_64)))
#define sp77encodingUCS2Native sp77encodingUCS2Swapped
#else
#define sp77encodingUCS2Native sp77encodingUCS2
#endif
/*!
  Declaration: sp77encodingUCS4
  Description: SAPDB UCS4 String encoding

  SAPDB interpretation of UCS4: HighByte,LowByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS4;
/*!
  Declaration: sp77encodingUCS4Swapped
  Description: SAPDB swapped UCS4 encoding

  SAPDB interpretation of swapped UCS4: LowByte,HighByte
 */
externCGlobal const tsp77encoding * const sp77encodingUCS4Swapped;
/*!
  Declaration: sp77encodingUCS4Native
  Description: Machine dependend UCS4 encoding

  Native (machine dependend UCS4): Machine byte order
 */
/* PTS 1110303 */
#if defined(WIN32) || (defined(LINUX) && !(defined(S390X)||defined(SDBonPPC64))) || defined(OSF1) || defined (FREEBSD)
#define sp77encodingUCS4Native sp77encodingUCS4Swapped
#else
#define sp77encodingUCS4Native sp77encodingUCS4
#endif
/*!
  Declaration: sp77encodingUTF8
  Description: UTF8 String encoding

  UTF8 Character strings
 */
externCGlobal const tsp77encoding * const sp77encodingUTF8;
/*!
  Endchapter: Encodings
 */

/*!
  Chapter: Prototypes
 */
/*!
  Function:     sp77sprintf

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for sprintf

  Since the POSIX API does not support an snprintf function, usage of sprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.

  Arguments:
    buf [out] pointer to the result buffer space
    buflen [in] size of buffer (including ZERO termination byte)
    format [in] print format string
    ... [in] variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77sprintf (
    char *buf,
    int buflen,
    const char *format,
    ...);

/*!
  Function:     sp77vsprintf

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for vsprintf

  Since the POSIX API does not support an vsnprintf function, usage of vsprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.

  Arguments:
    buf [out] pointer to the result buffer space
    buflen [in] size of buffer (including ZERO termination byte)
    format [in] print format string see
    see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

    args [in] "stdarg.h C/C++ compatible" variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77vsprintf (
    char *buf,
    int buflen,
    const char *format,
    va_list args);

/*!
  Function:     sp77sprintfUnicode

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for sprintf with addition choice of output encoding

  Identical to sp77sprintf but with additional encoding parameter, that allows
  to specify output encoding.

  Since the POSIX API does not support an snprintf function, usage of sprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.

  NOTE Since buflen cannot be estimated in characters, if UTF8 characters are
  wanted, it only makes sense to specify the result buffer length in bytes!

  Arguments:
    encoding [in] The wanted encoding for output buffer
    buf [out] pointer to the result buffer space
    buflen [in] size of result buffer in bytes (including ZERO termination bytes)
    format [in] print format string
    ... [in] variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77sprintfUnicode (
    const tsp77encoding * encoding,
    void *buf,
    int buflen,
    const char *format,
    ...);

/*!
  Function:     sp77vsprintfUnicode

  see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

  Description:  Replacement for vsprintf with additional choice of output encoding

  Identical to sp77vsprintf but with additional encoding parameter, that allows
  to specify output encoding.

  Since the POSIX API does not support an vsnprintf function, usage of vsprintf
  is discouraged!

  Advantages of these routine: If buflen is correct overwriting of bufferspace
  is prevented. The routine are reentrant (multithread save).

  On error the negativ count of sucessfully converted parameters is returned.

  Additional format %r,%R convert a string into a hexadecimal representation.
  Additional format %S convert an unicode string.

  An additional format modifier '=' was added, to allow to specify input encoding
  for unicode strings. This modifier is used in %r,%R and %S and otherwise ignored.

  If the modifier is found, two arguments are needed. First the input encoding
  (see gsp00.h: csp_ascii,csp_unicode,csp_unicode_swap,csp_unicode_native and csp_utf8)
  follwed by the string in the given encoding.

  Example

  sp77vsprintfUnicode(sp77encondigUTF8, buffer, len, "%-20=S", csp_unicode, UCS2Str)

  'buffer' is filled with UTF8 characters. Input is a 'HighByte,LowByte' UCS2 string.
  Maximum buffer size is len bytes (additional characters are silently discarded.

  NOTE Since 'len' cannot be estimated in characters, if UTF8 characters are
  wanted, it only makes sense to specify the result buffer length in bytes!

  Arguments:
    encoding [in] The wanted encoding for output buffer
    buf [out] pointer to the result buffer space
    buflen [in] size of result buffer in bytes (including ZERO termination bytes)
    format [in] print format string see (ALWAYS ascii)
    see also: http:\\pwww.bea.sap-ag.de\Doku\Posix\libc\printf.html

    args [in] "stdarg.h C/C++ compatible" variable argument list

  return value: number of bytes actually written, or neg-1 on error
    <UL>
        <LI>zero or positive: Number of bytes written into buffer excluding terminator
        <LI>negative: Error in format or buffer too small, negative count
                      of arguments successfully parsed. If buffer was exceeded
                      the null termination is not necessarily added!!!
    </UL>
 */
externC int sp77vsprintfUnicode (
    const tsp77encoding * encoding,
    void *buf,
    int buflen,
    const char *format,
    va_list args);

/*!
  Function: sp77nativeUnicodeEncoding
  Description: Return machine dependend native unicode encoding

  For example Intel architectures are normally used as LSB first, SPARC is MSB first.

  Return value: sp77encodingUCS2 on MSB first machines, sp77encodingUCS2Swapped on LSB first machines
 */
externC const tsp77encoding *sp77nativeUnicodeEncoding();

/*!
  Function: sp77isValidEncoding
  returns: true when <Param>encoding</> is a pointer to an encoding class.
 */
externC teo00_Bool sp77isValidEncoding (const tsp77encoding * encoding);

/*!
  Endchapter: Prototypes
 */
#endif

