/*!**************************************************************************

  module       : hsp81.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: UCS2 String Support
  description : Function for UCS2 string support

  For R/3 5.0 (UNICODE Release) some UCS2 string functions are needed. This module
  describes the UCS2 specific string functions. All functions in this module begin
  with prefix sp81UCS2_ and a name found in the posix description. The 'char' type
  is replaced by the tsp81_UCS2Char type. Example strlen equivalent is

    unsigned int sp81UCS2_strlen(const tsp81_UCS2Char *);

  Contained are strlen,
                strcat,strncat,
                strcmp,strncmp.
                strcpy,strncpy,
                strchr,strnchr
                strrchr,strnrchr

  Added UCS2 -> ISO8859-1 converting sp81UCS2toASCII and sp81ASCIItoUCS2
  (with SAPDB swapping support).

  Added sp81AnyASCIItoUCS2
  (with SAPDB swapping support).

  Added sp81AnyUCS2toASCII
  (with SAPDB swapping support).

  last changed: 2000-09-08  8:46
  first created:2000-07-27  9:07

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


#ifndef HSP81_H
#define HSP81_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define SP77_IS_ALIGNED_UCS2(_ptr) ( (((tsp00_Longint)_ptr)&1) == 0 )

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/*!
  Chapter: Typedeclarations
 */
/*!
  Declaration: tsp81_UCS2Char
  Description: This type replaces the 'char' used in POSIX string functions.

  Unfortunatly it is not used as an aligned data type!!!!
  Therefore it is a union now...
 */
#if defined(__cplusplus)
union tsp81_UCS2Char { tsp00_Uint2 s; tsp00_Byte c[2]; };
struct tsp81_UCS2Map { tsp00_Uint2 mapping[256]; };
union tsp81_UCS4Char { tsp00_Uint4 s; tsp00_Byte c[4]; };
#else
typedef union { tsp00_Uint2 s; tsp00_Byte c[2]; } tsp81_UCS2Char;
typedef struct t_sp81UCS2Map { tsp00_Uint2 mapping[256]; } tsp81_UCS2Map;
typedef union { tsp00_Uint4 s; tsp00_Byte c[4]; } tsp81_UCS4Char;
#endif

/*!
  Declaration: tsp81_CodePage
  Description: This structure describes ASCII code page mapping information
 */
typedef struct t_sp81CodePage
{
  tsp00_KnlIdentifierc  name;        /* Code page name */
  tsp00_Uint2           map[256];    /* UCS2 code points for all 256 ASCII Character of Code Page ID */
} tsp81_CodePage;

/*!
  EndChapter: Typedeclarations
 */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*!
  Chapter: Preprocessor Definitions
 */

/*!
  Declaration: SP81_UCS2NULL
  Description: A UCS2 NULL pointer
 */
#define SP81_UCS2NULL    ((tsp81_UCS2Char *)0)

/*!
  Declaration: SP81_UCS2EMPTYSTRING
  Description: A UCS2 empty string to replace ""
 */
extern const tsp81_UCS2Char sp81UCS2EmptyString[];
#define SP81_UCS2EMPTYSTRING sp81UCS2EmptyString

/*!
  Endchapter: Preprocessor Definitions
 */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
  Chapter: Prototypes
 */
/*!
  Function: sp81UCS2strlen
  Description: UCS2 equivalent for strlen

  Calculate the number of UCS characters not including the '00' termination.

  Arguments: s [in] The UCS2 zero terminated string
  Return value: Number of UCS2 characters not including '00' termination
 */
externC unsigned int sp81UCS2strlen(const tsp81_UCS2Char *s);

/*!
  Function: sp81UCS2str[n]cat
  Description: UCS2 equivalent for str[n]cat

  sp81_UCS2strcat conCATenates the source string to the destination string,
  using space provided behind the destination string. Return pointer to the
  destination string.

  NOTE that dest and src may not overlap and dest MUST have enaugh space for the
  result! Result is always '00' terminated.

  sp81UCS2strncat differs only from that only atmost 'n' characters are copied
  from source string, if not already a '00' character was detected.

  Arguments: dest [out] The destination string
              src [in] The source string
                n [in] The maximum number of character to concatenate excluding '00'
  Return value: Pointer to destination string
  Prototypes: 2
 */
externC tsp81_UCS2Char *sp81UCS2strcat(tsp81_UCS2Char *dest, const tsp81_UCS2Char *src);
externC tsp81_UCS2Char *sp81UCS2strncat(tsp81_UCS2Char *dest, const tsp81_UCS2Char *src, unsigned int n);

/*!
  Function: sp81UCS2str[n]cmp
  Description: UCS2 equivalent for str[n]cmp

  sp81UCS2strcmp compares two strings s1 and s2. sp81UCS2strncmp compares atmost
  'n' characters.

  Result is the numerical difference of the first tsp81_UCS2Char characters (interpreted
  as unsigned) that differ.

  If string length of s2 is smaller that string length of s1 result is always positiv.

  Arguments:  s1 [in] The first string
              s2 [in] The second string
               n [in] The maximum number of character to compare
  Return value:
          <UL>
          <LI>  0 if identical
          <LI>> 0 if first nonmatching character of s1 is larger then the corresponding character in s2
          <LI>< 0 if first nonmatching character of s1 is smaller then the corresponding character in s2
          </UL>

  Prototypes: 2
 */
externC int sp81UCS2strcmp(const tsp81_UCS2Char *s1, const tsp81_UCS2Char *s2);
externC int sp81UCS2strncmp(const tsp81_UCS2Char *s1, const tsp81_UCS2Char *s2, unsigned int n);

/*!
  Function: sp81UCS2str[n]chr
  Description: UCS2 equivalent for strchr

  sp81UCS2strchr searches for the given character in the given string.
  If character is contained, the pointer to the first position where the
  character is found is returned otherwise SP81_UCS2NULL.

  sp81UCS2strnchr limits the search to the first n characters

  Arguments:  s [in] The string to search in
              c [in] The character to search for
              n [in] Maximum number of characters to search through
  Return value: Pointer to position or SP81_UCS2NULL if not found

  Prototypes: 2
 */
externC tsp81_UCS2Char *sp81UCS2strchr(const tsp81_UCS2Char *s, const tsp81_UCS2Char c);
externC tsp81_UCS2Char *sp81UCS2strnchr(const tsp81_UCS2Char *s, const tsp81_UCS2Char c, unsigned int n);

/*!
  Function: sp81UCS2str[n]rchr
  Description: UCS2 equivalent for strrchr

  sp81UCS2strrchr searches for the given character in the given string
  beginning with last character in string.
  If character is contained, the pointer to the first position where the
  character is found is returned otherwise SP81_UCS2NULL.

  sp81UCS2strnrchr limits the search to the last n characters

  Arguments:  s [in] The string to search in
              c [in] The character to search for
              n [in] Maximum number of characters to search through
  Return value: Pointer to position or SP81_UCS2NULL if not found

  Prototypes: 2
 */
externC tsp81_UCS2Char *sp81UCS2strrchr(const tsp81_UCS2Char *s, const tsp81_UCS2Char c);
externC tsp81_UCS2Char *sp81UCS2strnrchr(const tsp81_UCS2Char *s, const tsp81_UCS2Char c,unsigned int n);

/*!
  Function: sp81UCS2str[n]cpy
  Description: UCS2 equivalent for str[n]cpy

  sp81UCS2strcpy copies source string into space provided for destination string.

  NOTE that dest and src may not overlap and dest MUST have enaugh space for the
  result! Result is always '00' terminated.

  sp81UCS2strncpy copies only atmost 'n' characters from source string into space
  provided for destination string. If '00' character is not contained in the first
  'n' characters of source string, result is not '00' terminated!

  NOTE that dest and src may not overlap and dest MUST have space for 'n' UCS2
  characters (in bytes 2 * n!!!). Result is NOT always '00' terminated, but if
  a '00' character was detected before 'n' character have been copied the
  destination is padded with '00' characters (Single UNIX specification...).

  To make sure your destination is '00' terminated, make it one UCS2 char longer
  and reset it to '0' or use the sequence

    strncpy(dest,src,n);
    dest[n] = SP81_UCS2ZERO;

  Arguments: dest [out] The destination string
              src [in] The source string
                n [in] The maximum number of character to be copied
  Return value: Pointer to destination string

  Prototypes: 2
 */
externC tsp81_UCS2Char *sp81UCS2strcpy(tsp81_UCS2Char *dest, const tsp81_UCS2Char *src);
externC tsp81_UCS2Char *sp81UCS2strncpy(tsp81_UCS2Char *dest, const tsp81_UCS2Char *src, unsigned int n);

/*!
  Function: sp81UCS2toASCII
  Description: Converting UCS2 string into ISO8859-1 ASCII String

  Output is ZERO terminated.

  Swapping is used like always in SAPDB!!!

  Non swapped means HighByte,LowByte

  Swapped means LowByte,HighByte.

  On NT/intel swapped is 'native'!!!!!!

  NOTE Since not all characters are convertable, this routine returns an error
  in case of misformed input.

  Arguments: dest    [out] The destination string
             destLen [in]  The number of characters maximum buffer space
             destOut [out] The number of characters actually converted
              src    [in]  The source string
              srcLen [in]  The maximum number of characters to convert
          srcSwapped [in]  Bool flag true means input bytes are swapped (LowByte,HighByte)
  Return value: 0 on error, numbers of characters converted on success
 */
externC unsigned int sp81UCS2toASCII(tsp00_Byte           *dest,
                                     unsigned int          destLen,
                                     unsigned int         *destOut,
                                     const tsp81_UCS2Char *src,
                                     unsigned int          srcLen,
                                     int                   srcSwapped);
/*!
  Function: sp81ASCIItoUCS2
  Description: Converting ISO8859-1 ASCII String into UCS2 string

  NOTE Since all characters are convertable, this routine cannot return an error

  Swapping is used like always in SAPDB!!!

  Non swapped means HighByte,LowByte

  Swapped means LowByte,HighByte.

  On NT/intel swapped is 'native'!!!!!!

  Arguments: dest    [out] The destination string
             destLen [in]  The number of characters maximum buffer space
             destOut [out] The number of characters actually converted
             destSwapped [in] Bool flag true means UCS2 characters want to be swapped (LowByte,HighByte)
             src    [in] The source string
             srcLen [in] The maximum number of characters to convert
  Return value: numbers of characters converted
 */

externC tsp81_CodePage *sp81CodePage;

externC tsp81_CodePage *sp81SetCodePage(tsp81_CodePage *CodePage);
externC tsp81_CodePage *sp81GetCodePage();

externC unsigned int sp81ASCIItoUCS2(tsp81_UCS2Char   *dest,
                              unsigned int      destLen,
                              int               destSwapped,
                              unsigned int     *destOut,
                              const tsp00_Byte *src,
                              unsigned int      srcLen);

/*!
  Function: sp81ASCIItoUCS4
  Description: Converting ISO8859-1 ASCII String into UCS4 string

  NOTE Since all characters are convertable, this routine cannot return an error

  Swapping is used like always in SAPDB!!!

  Non swapped means HighByte,LowByte

  Swapped means LowByte,HighByte.

  On NT/intel swapped is 'native'!!!!!!

  Arguments: dest    [out] The destination string
             destLen [in]  The number of characters maximum buffer space
             destOut [out] The number of characters actually converted
             destSwapped [in] Bool flag true means UCS4 characters want to be swapped (LowByte,HighByte)
             src    [in] The source string
             srcLen [in] The maximum number of characters to convert
  Return value: numbers of characters converted
 */

externC unsigned int sp81ASCIItoUCS4(tsp81_UCS4Char   *dest,
                              unsigned int      destLen,
                              int               destSwapped,
                              unsigned int     *destOut,
                              const tsp00_Byte *src,
                              unsigned int      srcLen);

/*!
  Function: sp81AnyASCIItoUCS2
  Description: Converting any ASCII String into UCS2 string

  NOTE Since all characters are convertable, this routine cannot return an error

  Swapping is used like always in SAPDB!!!

  Non swapped means HighByte,LowByte

  Swapped means LowByte,HighByte.

  On NT/intel swapped is 'native'!!!!!!

  Arguments: dest    [out] The destination string
             destLen [in]  The number of characters maximum buffer space
             destOut [out] The number of characters actually converted
             destSwapped [in] Bool flag true means UCS2 characters want to be swapped (LowByte,HighByte)
             src    [in] The source string
             srcLen [in] The maximum number of characters to convert
             srcCodePage [in] The code page to use for conversion
  Return value: numbers of characters converted
 */
externC unsigned int sp81AnyASCIItoUCS2(tsp81_UCS2Char   *dest,
                                        unsigned int      destLen,
                                        int               destSwapped,
                                        unsigned int     *destOut,
                                        const tsp00_Byte *src,
                                        unsigned int      srcLen,
                                        tsp81_CodePage   *srcCodePage);

/*!
  Function: sp81AnyASCIItoUCS2
  Description: Converting any ASCII String into UCS4 string

  NOTE Since all characters are convertable, this routine cannot return an error

  Swapping is used like always in SAPDB!!!

  Non swapped means HighByte,LowByte

  Swapped means LowByte,HighByte.

  On NT/intel swapped is 'native'!!!!!!

  Arguments: dest    [out] The destination string
             destLen [in]  The number of characters maximum buffer space
             destOut [out] The number of characters actually converted
             destSwapped [in] Bool flag true means UCS4 characters want to be swapped (LowByte,HighByte)
             src    [in] The source string
             srcLen [in] The maximum number of characters to convert
             srcCodePage [in] The code page to use for conversion
  Return value: numbers of characters converted
 */
externC unsigned int sp81AnyASCIItoUCS4(tsp81_UCS4Char   *dest,
                                        unsigned int      destLen,
                                        int               destSwapped,
                                        unsigned int     *destOut,
                                        const tsp00_Byte *src,
                                        unsigned int      srcLen,
                                        tsp81_CodePage   *srcCodePage);

/*!
  Function: sp81AnyUCS2toASCII
  Description: Converting codeset depended UCS2 String into ASCII string

  Swapping is used like always in SAPDB!!!

  Non swapped means HighByte,LowByte

  Swapped means LowByte,HighByte.

  On NT/intel swapped is 'native'!!!!!!

  NOTE Since not all characters are convertable, this routine returns an error
  in case of misformed input.

  Arguments: dest    [out] The destination string
             destLen [in]  The number of characters maximum buffer space
             destOut [out] The number of characters actually converted
              src    [in]  The source string
              srcLen [in]  The maximum number of characters to convert
          srcSwapped [in]  Bool flag true means input bytes are swapped (LowByte,HighByte)
  Return value: 0 on error, numbers of characters converted on success
 */
externC unsigned int sp81AnyUCS2toASCII(tsp00_Byte     *dest,
                                        unsigned int    destLen,
                                        unsigned int   *destOut,
                                        const tsp81_UCS2Char  *src,
                                        unsigned int    srcLen,
                                        int             srcSwapped,
                                        tsp81_CodePage  *destCodePage);

/*!
  Function: sp81UCS2StringInfo
  Description: Functions that report encoding specific information about UCS2 C-strings

  Typecheck for first parameter is not enforced (void *) to be compatible with tsp77encoding
  function pointer member stringInfo!!!

  UCS2-C-Strings means that a string is terminated by a termination character! If no
  termination character is found within byteLength, the byteLength terminates the
  UCS2-C-String.

  Returned character and byte count differ by a factor of sizeof(UCS2Char).
  Counts reported do never contains the two termination character zero bytes.
  Any partial character is also not contained.

  pIsTerminated is pure information, usefull to decide whether to add own termination if needed

  pIsExhausted is reported may be used as warning, but if set 'false' is returned.

  pIsCorrupted is an error and if set 'false' is returned.

  Even if an error was detected, pCharCount and pByteParsed are always valid.

  Arguments: buffer [in] Buffer containing input string

             bufferLength [in] Size of buffer (not necessarily string length!!!)

         bufferLengthIsInBytes [in] if true bufferLength is given in bytes

             pCharCount [out] Number of character in buffer up to termination

             pBytesParsed [out] Number of bytes in buffer parsed up to termination

             pIsTerminated [out] Flag: Terminated by a compete ZERO terminator

             pIsCorrupted [out] Flag: Input character not in valid encoding

             pIsExhausted [out] Flag: Last input character was only partial

  Return value: true if no error detected, false else
*/
externC int sp81UCS2StringInfo( const void *buffer,
                                 unsigned int bufferLength,
                                 int bufferLengthIsInBytes,
                                 unsigned int *pCharCount,
                                 unsigned int *pByteCount,
                                 int *pIsTerminated,
                                 int *pIsCorrupted,
                                 int *pIsExhausted);

/*!
  Function: sp81UCS4StringInfo
  Description: Functions that report encoding specific information about UCS4 C-strings

  Typecheck for first parameter is not enforced (void *) to be compatible with tsp77encoding
  function pointer member stringInfo!!!

  UCS4-C-Strings means that a string is terminated by a termination character! If no
  termination character is found within byteLength, the byteLength terminates the
  UCS4-C-String.

  Returned character and byte count differ by a factor of sizeof(UCS4Char).
  Counts reported do never contains the two termination character zero bytes.
  Any partial character is also not contained.

  pIsTerminated is pure information, usefull to decide whether to add own termination if needed

  pIsExhausted is reported may be used as warning, but if set 'false' is returned.

  pIsCorrupted is an error and if set 'false' is returned.

  Even if an error was detected, pCharCount and pByteParsed are always valid.

  Arguments: buffer [in] Buffer containing input string

             bufferLength [in] Size of buffer (not necessarily string length!!!)

         bufferLengthIsInBytes [in] if true bufferLength is given in bytes

             pCharCount [out] Number of character in buffer up to termination

             pBytesParsed [out] Number of bytes in buffer parsed up to termination

             pIsTerminated [out] Flag: Terminated by a compete ZERO terminator

             pIsCorrupted [out] Flag: Input character not in valid encoding

             pIsExhausted [out] Flag: Last input character was only partial

  Return value: true if no error detected, false else
*/
externC int sp81UCS4StringInfo( const void *buffer,
                                 unsigned int bufferLength,
                                 int bufferLengthIsInBytes,
                                 unsigned int *pCharCount,
                                 unsigned int *pByteCount,
                                 int *pIsTerminated,
                                 int *pIsCorrupted,
                                 int *pIsExhausted);

/*----------------------------------------------------
  Reference to generated first stage mapping tables
  ----------------------------------------------------*/
extern tsp81_UCS2Map *sp81UCS2UpperCaseMap[256];
extern tsp81_UCS2Map *sp81UCS2LowerCaseMap[256];

/*!
  Function: sp81UCS2QuotesContainingStringToupper
  Description: Convert given UCS2 (HighByte/LowByte) buffer to uppercase in place

  Encoding type of UCS2 Characters is csp_unicode (high byte, low byte).
  If buffer length is not an even number, last byte is not used.

  Quoted parts of string (contained in pairs of single' or ") are not converted to uppercase.

  Arguments: buffer [in] Pointer to buffer space
            bufferLenInBytes [in] Size of bufferspace in bytes ( 2 * number of UCS2 Characters )
 */
externC void sp81UCS2QuotesContainingStringToupper( void *buffer, tsp00_Int4 bufferLenInBytes);

/*!
  Function: sp81UCS2QuotesContainingStringToupper
  Description: Convert given swapped UCS2 (LowByte/HighByte) buffer to uppercase in place

  Encoding type of UCS2 Characters is csp_unicode_swap (low byte, high byte).
  If buffer length is not an even number, last byte is not used.

  Quoted parts of string (contained in pairs of single' or ") are not converted to uppercase.

  Argument: buffer [in] Pointer to buffer space
            bufferLenInBytes [in] Size of bufferspace in bytes ( 2 * number of UCS2 Characters )
 */
externC void sp81UCS2QuotesContainingSwappedStringToupper( void *buffer, tsp00_Int4 bufferLenInBytes);

/*!
  Function: sp81AnyUCS2QuotedStringToupper
  Description: Convert given UCS2 string  to uppercase in place

  Encoding type of UCS2 Characters is depending on lowByteIndex ( 0 = csp_unicode_swap, 1 = csp_unicode )
  If buffer length is not an even number, last byte is not used.

  Quoted parts of string (contained in pairs of single' or ") are not converted to uppercase.

  Argument: buffer [in] Pointer to buffer space
            bufferLenInBytes [in] Size of bufferspace in bytes ( 2 * number of UCS2 Characters )
            lowByteIndex [in] 0 for low/high byteorder, 1 for high/low byteorder
 */
externC void sp81AnyUCS2QuotedStringToupper(
 tsp81_UCS2Char *bufferPtr,
 tsp00_Int4      lengthInUCS2Characters,
 int             lowByteIndex );


/*!
  Function: sp81UCS2StringToupper
  Description: Convert given UCS2 (HighByte/LowByte) buffer to uppercase in place

  Encoding type of UCS2 Characters is csp_unicode (high byte, low byte).
  If buffer length is not an even number, last byte is not used.

  Arguments: buffer [inout] Pointer to buffer space
            bufferLenInBytes [in] Size of bufferspace in bytes ( 2 * number of UCS2 Characters )
 */
externC void sp81UCS2StringToupper( void *buffer, tsp00_Int4 bufferLenInBytes);

/*!
  Function: sp81UCS2SwappedStringToupper
  Description: Convert given swapped UCS2 (LowByte/HighByte) buffer to uppercase in place

  Encoding type of UCS2 Characters is csp_unicode_swap (low byte, high byte).
  If buffer length is not an even number, last byte is not used.

  Argument: buffer [inout] Pointer to buffer space
            bufferLenInBytes [in] Size of bufferspace in bytes ( 2 * number of UCS2 Characters )
 */
externC void sp81UCS2SwappedStringToupper( void *buffer, tsp00_Int4 bufferLenInBytes);

/*!
  Function: sp81UCS2ToUpper
  Description: Convert given native UCS2 key to uppercase
 */
externC tsp00_Uint2 sp81UCS2ToUpper(tsp00_Uint2 KeyToMap);

/*!
  Function: sp81UCS2ToLower
  Description: Convert given native UCS2 key to lowercase
 */
externC tsp00_Uint2 sp81UCS2ToLower(tsp00_Uint2 KeyToMap);

/*!
  Function: sp81UCS2FillString
  Description: Fill given buffer with padding characters
  Padding character is an ascii 7-bit character. buffer and buflen are
  adjusted.

  Arguments: buffer [inout] buffer to fill
             buflen [inout] buffer length in bytes
             padCount [in] number of padding characters wanted
             padChar [in] the used padding character
             bufferSwapped [in] if true buffer contains swapped UCS2
 */
void  sp81UCS2FillString( void **buffer,
                          unsigned int *buflen,
                          unsigned int padCount,
                          tsp00_Char padChar,
                          int    bufferSwapped);
/*!
  Function: sp81UCS4FillString
  Description: Fill given buffer with padding characters
  Padding character is an ascii 7-bit character. buffer and buflen are
  adjusted.

  Arguments: buffer [inout] buffer to fill
             buflen [inout] buffer length in bytes
             padCount [in] number of padding characters wanted
             padChar [in] the used padding character
             bufferSwapped [in] if true buffer contains swapped UCS2
 */
void  sp81UCS4FillString( void **buffer,
                          unsigned int *buflen,
                          unsigned int padCount,
                          tsp00_Char padChar,
                          int    bufferSwapped);
/*!
  EndChapter: Prototypes
 */

#endif  /* HSP81_H */
