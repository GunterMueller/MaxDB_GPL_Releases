/*!
    @file    hls05.h
    @author  SteffenS
    @brief   String functions

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

#ifndef HLS05
#define HLS05

#include "gls00.h"

/*!
    \brief  Character array to map Latin1 UPPER CASE letters to lower case letters.
*/
static unsigned char
lLatin1_ToLowerMap_ls05[256] =
{
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
 0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
 0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
 0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
 0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
 0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
 0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
 0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
 0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
 0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
 0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
 0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF
};


/*!
    \brief  Character array to map Latin1 lower case letters to UPPER CASE letters.
*/
static unsigned char
lLatin1_ToUpperMap_ls05[256] =
{
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
 0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
 0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
 0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
 0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
 0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
 0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
 0x58,0x59,0x5A,0x7B,0x7C,0x7D,0x7E,0x7F,
 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
 0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
 0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
 0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
 0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
 0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,
 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF
};

/*!
    \brief  Converts ASCII 8 bit Latin-1 string in place to UPPER CASE
    \param  pszString      [IN] - the string to convert
    \param  lStringLength  [IN] - length of the string
    \return Nothing
*/
void
ls05StrToUpr(tsp00_Addr pszString, tsp00_Int4 lStringLength);


/*!
    \brief Trims a not NULL terminated ASCII string right

    The string itself is NOT CHANGED. Only its length is adapted.
    Does not work for UNICODE or MBCS strings.

    \param  pszString    [IN] - the string to convert
    \param  lStrLength   [IN] - length of the string
    \return Nothing
*/
void
ls05StrTrimRight(tsp00_Addr pszString, tsp00_Int4& lStrLength);


/*!
    \brief A not NULL terminated ASCII string is trimmed right by a defined character.

           The string itself is NOT CHANGED. Only its length is adapted.
           Does not work for UNICODE or MBCS strings.

    \param  pszString    [IN]     - the string to convert
    \param  lStrLength   [IN/OUT] - length of the string
    \param  cTrimVal     [IN]     - character to remove
    \return Nothing
*/
inline void
ls05StrTrimRight(tsp00_Addr pszString, tsp00_Longint& lStrLength, char cTrimVal)
{
    ROUTINE_DBG_MEO00 ("ls05StrTrimRight II");
    
    while ( (cTrimVal == pszString[lStrLength-1]) &&
            (lStrLength > 0) )
    {
        --lStrLength;
    }
}


/*!
    \brief A not NULL terminated string (ASCII or UNICODE) is trimmed right
           by a defined character (US7ASCII).

           The string itself is NOT CHANGED. Only its length is adapted.
           Length is delivered in bytes not in characters.

    \param  pszString   [IN]      - the string to convert
    \param  lStrLength  [IN/OUT]  - length of the string
    \param  cTrimVal    [IN]      - character to remove - only US7ASCII
    \param  CodeType    [IN]      - defines string encoding

    \return Nothing
*/
void
ls05StrTrimRight(tsp00_Addr      pszString,
                 tsp00_Longint  &lStrLength,
                 char            cTrimVal,
                 tls00_CodeType  CodeType);


/*!
    \brief A not NULL terminated ASCII string is trimmed left.

           Does not work for UNICODE or MBCS strings.

    \param  pszString    [IN]     - the string to convert
    \param  lStrLength   [IN/OUT] - length of the string
    \return Nothing
*/
inline void
ls05StrTrimLeft(tsp00_Addr& pszString, tsp00_Int4& lStrLength)
{
    ROUTINE_DBG_MEO00 ("ls05StrTrimLeft");

    while (isspace(*pszString))
    {
        pszString++;
        lStrLength -= 1;
    }
}


/*!
    \brief Deletes leading chars defined by the caller (in place).

           The encoding type may one of the following: ASCII, UTF8 or UCS2.
           The char to delete must be given as US7ASCII.
           The string itself is modified and the length is adapted.

    \param  pBuffer        [IN/OUT]  - string to trim
    \param  lBufferLength  [IN/OUT]  - string length before and after processing
    \param  cTrimChar      [IN]      - char to remove (only US7ASCII)
    \param  CodeType       [IN]      - encoding type
    \return Nothing
*/
void
ls05StrTrimLeft(tsp00_Addr       pBuffer,
                tsp00_Longint   &lBufferLength,
                char             cTrimChar,
                tls00_CodeType   CodeType);


/*!
    \brief Truncate trailing blanks from NULL terminated ASCII string.

           Does not work for UNICODE or MBCS strings.

    \param  pszString    [IN] - the string to convert
    \return Nothing
*/
void
ls05StrTrimRightEx(tsp00_Addr pszString);


/*!
    \brief Removes leading blanks from a NULL terminated ASCII string.

           The given string length is adapted.

    \param  pszString    [IN]     - the string to convert
    \param  lStrLength   [IN/OUT] - length of the string
    \return Nothing
*/
void
ls05StrTrimLeftEx(tsp00_Addr pszString, tsp00_Longint& lStrLength);


/*!
    \brief Compares 2 values whereas the smaller is filled with blanks 
           before comparison.

    \param   pszValue1   [IN]  - first value
    \param   lVal1Len    [IN]  - length of the first value
    \param   pszValue2   [IN]  - second value
    \param   lVal2Len    [IN]  - length of the second value

    \return  true    - values are equal<br>
             false   - values are not equal
*/
bool
ls05StrCmpWFill(const tsp00_Addr pszValue1,
                tsp00_Int4       lVal1Len,
                const tsp00_Addr pszValue2,
                tsp00_Int4       lVal2Len);

/*!
    \brief Compares 2 values whereas the smaller is filled with blanks 
           before comparison. Takes encoding into account.

    \param   pszValue1   [IN]  - first value
    \param   lVal1Len    [IN]  - length of the first value (in Bytes)
    \param   pszValue2   [IN]  - second value
    \param   lVal2Len    [IN]  - length of the second value (in Bytes)
    \param   CodeType    [IN]  - encoding

    \return  true    - values are equal<br>
             false   - values are not equal
*/
SAPDB_Bool
ls05StrCmpPadded(const SAPDB_Byte*  pszValue1,
                 SAPDB_UInt4        lVal1Len,
                 const SAPDB_Byte*  pszValue2,
                 SAPDB_UInt4        lVal2Len,
                 tls00_CodeType     CodeType);

/*!
    \brief Fills a value left aligned with padding bytes.

           The kind of padding bytes are defined using the db data type.
           Takes UNICODE (UCS2) into accout (with the right swap).

    \param   pszData        [IN]  - value to pad
    \param   lDataLen       [IN]  - length of value
    \param   DBDataType     [IN]  - data type
    \param   lNeededDataLen [IN]  - length to fill to
    \param   bUnicode       [IN]  - value is UCS2 or not
    \param   mySwap         [IN]  - the swap

    \return  Nothing
*/
void
ls05InsertFillBytes(unsigned char       *pszData,
                    tsp00_Longint        lDataLen,
                    tsp00_DataType       DBDataType,
                    tsp00_Longint        lNeededDataLen,
                    bool                 bUnicode,
                    tsp00_SwapKind_Enum  mySwap);


/*!
    \brief Checks if encoding of external fields and DB columns match.

    \param  DataFile_Encoding  [IN] - external field encoding
    \param  DB_Encoding        [IN] - DB column column encoding
    \param  bLoad              [IN] - defines if loading or extracting

    \return errOK_els00                    - External encoding allowed<br>
            errIncompatibleEncodings_els98 - Incompatible external and internal
                                             encodings; a conversion from
                                             external to internal or vice versa
                                             would lose information
*/
tsp00_Longint
ls05AreEncodingsCompatible(tls00_CodeType DataFile_Encoding,
                           tls00_CodeType DB_Encoding,
                           bool           bLoad = true);



/*!
    \brief Converts a UTF8 or ASCII string to native UCS2 encoding.

           Native means that byte order of UCS2 chars correspond to
           machine swap of machine running on.
           Takes db data type into account, i.e. data of data type byte
           is not converted.

    \param  pszDestination  [OUT]     - destination string (ucs2)
    \param  lDestinationLen [IN/OUT]  - length of destination string in byte
    \param  pszSource       [IN]      - source string (ascii or utf8)
    \param  lSourceLen      [IN]      - length of source string
    \param  SrcEncoding     [IN]      - encoding of source string
    \param  DBDataType      [IN]      - db data type
    \param  pszErrText      [OUT]     - error text
    \param  pASCII_CP       [IN]      - code page structure used for conversion;
                                         defaults to NULL pointer;
                                         if NULL given code page ISO8859-1 = Latin1
                                         is used

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source, but hit end or
                                            insufficient space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or char sequences
*/
tsp00_Longint
ls05ConvertToUCS2(unsigned char   *pszDestination,
                  tsp00_Longuint  &lDestinationLen,
                  unsigned char   *pszSource,
                  tsp00_Longint    lSourceLen,
                  tls00_CodeType   SrcEncoding,
                  tsp00_DataType   DBDataType,
                  tsp00_Addr       pszErrText,
                  tsp81_CodePage  *pASCII_CP = NULL);


/*!
    \brief Converts UTF8 or ASCII string to native UCS2 encoding

           Native means that byte order of UCS2 chars corresponds
           to machine swap of machine running on.

    \param  pszDestination  [OUT]  - destination string (ucs2)
    \param  lDestinationLen [IN]   - length of destination string in byte
    \param  pszSource       [IN]   - source string (ascii or utf8)
    \param  lSourceLen      [IN]   - length of source string
    \param  SrcEncoding     [IN]   - encoding of source string
    \param  pszErrText      [OUT]  - error text
    \param  pASCII_CP       [IN]   - code page structure used for conversion;
                                      defaults to NULL pointer;
                                      if NULL given code page ISO8859-1 = Latin1
                                      is used

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient<br>
                                            space in target for conversion
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertToUCS2Simple(unsigned char  *pszDestination,
                        tsp00_Longuint &lDestinationLen,
                        unsigned char  *pszSource,
                        tsp00_Longint   lSourceLen,
                        tls00_CodeType  SrcEncoding,
                        tsp00_Addr      pszErrText,
                        tsp81_CodePage *pASCII_CP = NULL);


/*!
    \brief Converts UTF8 or ASCII string to given UCS2 encoding

    \param  pszDestination  [OUT]  - destination string (ucs2)
    \param  lDestinationLen [IN]   - length of destination string in byte
    \param  DestEncoding    [IN]   - encoding of destination string
    \param  pszSource       [IN]   - source string (ascii or utf8)
    \param  lSourceLen      [IN]   - length of source string
    \param  SrcEncoding     [IN]   - encoding of source string
    \param  pszErrText      [OUT]  - error text
    \param  pASCII_CP       [IN]   - code page structure used for conversion;
                                      defaults to NULL pointer;
                                      if NULL given code page ISO8859-1 = Latin1
                                      is used

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient<br>
                                            space in target for conversion
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertToUCS2Simple(unsigned char  *pszDestination,
                        tsp00_Longuint &lDestinationLen,
                        tls00_CodeType  DestEncoding,
                        unsigned char  *pszSource,
                        tsp00_Longint   lSourceLen,
                        tls00_CodeType  SrcEncoding,
                        tsp00_Addr      pszErrText,
                        tsp81_CodePage *pASCII_CP = NULL);

/*!
    \brief Converts a UTF8 or ASCII string to UCS2 encoding not swapped

    \param  pszDestination  [OUT]     - destination string (ucs2)
    \param  lDestinationLen [IN/OUT]  - length of destination string in byte
    \param  pszSource       [IN]      - source string (ascii or utf8)
    \param  lSourceLen      [IN]      - length of source string
    \param  SrcEncoding     [IN]      - encoding of source string
    \param  pszErrText      [OUT]     - error text

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertToUCS2NotSwapped(unsigned char  *pszDestination,
                            tsp00_Longuint &lDestinationLen,
                            unsigned char  *pszSource,
                            tsp00_Longuint  lSourceLen,
                            tls00_CodeType  SrcEncoding,
                            tsp00_Addr      pszErrText);

/*!
    \brief Converts an UCS2 to UTF8 or ASCII.

           UCS2 is always native UCS2 which means that byte order
           of UCS2 chars correspond to machine swap of machine running on.
           Takes db data type into account, i.e. data of data type byte
           is not converted.

    \param  pszDestination  [OUT]    - destination string (ASCII or UTF8)
    \param  lDestinationLen [IN/OUT] - length of destination string in byte
                                        before and after conversion
    \param  DestEncoding    [IN]     - encoding of destination string
    \param  pszSource       [IN]     - source string (UCS2 native)
    \param  lSourceLen      [IN]     - length of source string
    \param  DBDataType      [IN]     - db data type
    \param  pszErrText      [OUT]    - error text

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertFromUCS2(unsigned char  *pszDestination,
                    tsp00_Longint  &lDestinationLen,
                    tls00_CodeType  DestEncoding,
                    unsigned char  *pszSource,
                    tsp00_Longint   lSourceLen,
                    tsp00_DataType  DBDataType,
                    tsp00_Addr      pszErrText);


/*!
    \brief Converts a UCS2 encoded string to UTF8 or ASCII string 

    \param  pszDestination  [OUT] - destination string (ascii or utf8)
    \param  pszSource       [IN]  - source string (ucs2)
    \param  lDestinationLen [OUT] - length of destination string in byte
    \param  lSourceLen      [IN]  - length of source string
    \param  DestEncoding    [IN]  - encoding of destination string
    \param  SrcEncoding     [IN]  - encoding of source string
    \param  pszErrText      [OUT] - error text

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertFromUCS2Simple(unsigned char  *pszDestination,
                          unsigned char  *pszSource,
                          tsp00_Longint  &lDestinationLen,
                          tsp00_Longint   lSourceLen,
                          tls00_CodeType  DestEncoding,
                          tls00_CodeType  SrcEncoding,
                          tsp00_Addr      pszErrText);


/*!
    \brief Converts a UCS2 native encoded string to UTF8 or ASCII string 

           Native means that byte order of UCS2 chars corresponds to
           machine swap of machine running on.

    \param  pszDestination  [OUT] - destination string (ascii or utf8)
    \param  pszSource       [IN]  - source string (ucs2)
    \param  lDestinationLen [OUT] - length of destination string in byte
    \param  lSourceLen      [IN]  - length of source string
    \param  DestEncoding    [IN]  - encoding of destination string
    \param  pszErrText      [OUT] - error text

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertFromUCS2Simple(unsigned char  *pszDestination,
                          unsigned char  *pszSource,
                          tsp00_Longint  &lDestinationLen,
                          tsp00_Longint   lSourceLen,
                          tls00_CodeType  DestEncoding,
                          tsp00_Addr      pszErrText);


/*!
    \brief Converts a string from UTF8 to UCS2 or ASCII.

    \param  pszDestination  [OUT] - destination string (UCS2 or ASCII)
    \param  lDestinationLen [OUT] - length of destination string in byte
    \param  Source          [IN]  - source string (UTF8)
    \param  DestEncoding    [IN]  - encoding of destination string; in case
                                     of UCS2 it must define the swap, too
    \param  pszErrText      [OUT] - error text

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertFromUTF8(unsigned char           *pszDestination,
                    tsp00_Longuint          &lDestinationLen,
                    Tools_DynamicUTF8String &Source,
                    tls00_CodeType           DestEncoding,
                    tsp00_Addr               pszErrText);


/*!
    \brief Converts a string from UTF8 to UCS2 or ASCII.

    \param  pszDestination  [OUT] - destination string (UCS2 or ASCII)
    \param  lDestinationLen [OUT] - length of destination string in Bytes
    \param  pszSource       [IN]  - source string (UTF8)
    \param  lSourceLen      [IN]  - source string length in Bytes
    \param  DestEncoding    [IN]  - encoding of destination string; in case
                                     of UCS2 it must define the swap, too
    \param  pszErrText      [OUT] - error text

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05ConvertFromUTF8(unsigned char           *pszDestination,
                    tsp00_Longuint          &lDestinationLen,
                    unsigned char           *pszSource,
                    tsp00_Longuint           lSourceLen,
                    tls00_CodeType           DestEncoding,
                    tsp00_Addr               pszErrText);

/*!
    \brief Converts a string from UCS2 or ASCII to UTF8.

    \param  Destination     [OUT] - destination string (UTF8)
    \param  pszSource       [IN]  - source string (UCS2 or ASCII)
    \param  lSourceLen      [IN]  - length of source string in Bytes
    \param  SrcEncoding     [IN]  - encoding of source string; in case
                                     of UCS2 it must define the swap, too
    \param  pszErrText      [OUT] - error text

    \return  errOK_els00                 - successful conversion<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                           char sequences
*/
tsp00_Longint
ls05ConvertToUTF8(Tools_DynamicUTF8String &Destination,
                  unsigned char           *pszSource,
                  tsp00_Longuint           lSourceLen,
                  tls00_CodeType           SrcEncoding,     // must give a hint on swapped or not - e.g. ctUCS2_els00 or ctUCS2Native_els00 or ctUCS2Swapped_els00
                  tsp00_Addr               pszErrText);

/*!
    \brief Assigns a string (ASCII or UTF8) to an UTF8 string.

           The function checks if the string is already valid UTF8 and if
           not converts it to.

    \param  Destination  [OUT] - destination string (UTF8)
    \param  pszSource    [IN]  - source string
    \param  lSourceLen   [IN]  - length of source string in Bytes
    \param  pszErrText   [OUT] - error text

    \return  errOK_els00                 - successful assignment<br>
             errInternal_els98           - when partial character in source,
                                            but hit end or insufficient
                                            space in target for conversion<br>
             errConvertingEncoding_els98 - source contains invalid chars or
                                            char sequences
*/
tsp00_Longint
ls05AssignUTF8(Tools_DynamicUTF8String &Destination,
               const unsigned char     *pszSource,
               tsp00_Longuint           lSourceLen,
               tsp00_Addr               pszErrText);


/*!
    \brief Escapes quote character in a source DB identifier and copies it to
           a destination DB identifier.
           The destination ID is returned as UTF8 encoded string.
           Strings must be allocated prior to calling.
           If the source ID is UCS2 encoded the function decides depending on
           SERVER_SWAPPING_LS00 whether to handle it as swapped or not swapped
           UCS2 string.

    \param  pszSourceID             [IN]  - source string
    \param  lSourceIDLengthInBytes  [IN]  - length of source string
    \param  pszDestinationID        [OUT] - destination string (UTF8)
    \param  bUnicode                [IN]  - whether the string is UCS2 or not
    \param  cQuoteChar              [OUT] - character to escape

    \return  None
*/
void
ls05EscapeQuotesInDBIdentifier(SAPDB_Char* pszSourceID,
                               SAPDB_Int4  lSourceIDLengthInBytes,
                               SAPDB_Char* pszDestinationID,
                               SAPDB_Bool  bUnicode,
                               SAPDB_Char  cQuoteChar);


/*!
    \brief Makes DB Identifier literals to use them in where-clauses in select
           expressions: Escapes single quote character and removes escaping double
           quotes.
           The destination ID is returned as UTF8 encoded string.
           Strings must be allocated prior to calling.
           If the source ID is UCS2 encoded the function decides depending on
           SERVER_SWAPPING_LS00 whether to handle it as swapped or not swapped
           UCS2 string.

    \param  pszSourceID             [IN]  - source string
    \param  lSourceIDLengthInBytes  [OUT] - destination string (UTF8)
    \param  pszDestinationID        [OUT] - destination string (UTF8)

    \return  None
*/
void
ls05MakeDBIdentifierLiteral(SAPDB_Char* pszSourceID,
                            SAPDB_Int4  lSourceIDLengthInBytes,
                            SAPDB_Char* pszDestinationID);
#endif /*HLS05*/