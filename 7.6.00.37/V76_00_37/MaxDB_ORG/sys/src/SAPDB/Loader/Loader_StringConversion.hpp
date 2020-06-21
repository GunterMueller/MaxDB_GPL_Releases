/*!
  \file    Loader_StringConversion.hpp
  \author  SteffenS
  \brief   string conversion routines definition

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#ifndef LOADER_STRINGCONVERSION_HPP
#define LOADER_STRINGCONVERSION_HPP


#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Common.hpp"
#include "Loader/Loader_ErrorCode.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"  // unicode support

const unsigned char Loader_szHexToByteMap[256] = {
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,
 
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 
 0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,
 
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 
 0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,
 
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};




LOADER_ErrorCode
Loader_StrConvertFromUTF8(LOADER_Byte*              pszDestination,
                          LOADER_UInt4&             lDestinationLen,   // reference - contains the real length on return
                          Tools_DynamicUTF8String&  Source,
                          LOADER_StringEncoding     DestEncoding,      // must give a hint on swapped or not - e.g. ctUCS2_els00 or ctUCS2Native_els00 or ctUCS2Swapped_els00
                          LOADER_Char*              pszErrText);


LOADER_ErrorCode
Loader_StrConvertToUCS2NotSwapped(LOADER_Byte*           pszDestination,
                                  LOADER_UInt4&          lDestinationLen,
                                  LOADER_Byte*           pszSource,
                                  LOADER_UInt4           lSourceLen,
                                  LOADER_StringEncoding  SrcEncoding);


/*
  -----------------------------------------------------------------------------

  function:     Loader_StrConvertUCS2ToASCII
  -----------------------------------------------------------------------------

  description:  Converts a string from UCS2 to ASCII Latin1.

  arguments:    pszASCII    [OUT]    - destination string
                lASCIILen   [IN/OUT] - length of destination string in Bytes
                pszUCS2     [IN]     - source string
                lUCS2Len    [IN]     - length of source string in Bytes
                UCS2Type    [IN]     - type of UCS2 encoding (swapped or not)

  returnvalue:  destination string length in lASCIILen

                return code:
                errOK_els00                 - successful conversion
                errInternal_els98           - when partial character in source,
                                                but hit end or insufficient
                                                space in target for conversion
                errConvertingEncoding_els98 - source contains invalid chars or
                                               char sequences
  -----------------------------------------------------------------------------
*/
LOADER_ErrorCode
Loader_StrConvertUCS2ToASCII(LOADER_Byte*           pszASCII,
                             LOADER_UInt4&          lASCIILen,
                             LOADER_Byte*           pszUCS2,
                             LOADER_UInt4           lUCS2Len,
                             LOADER_StringEncoding  SrcEncoding);



// The difference to the other functions is that the function must compute the length of pszSource itself
// if lSourceLength is -1; source should be safely complete and valid!
// lSourceLength is the length in CHARACTERS!!!!!!
// lDestination is the length in Bytes; if given 0 the ppszDestination is allocated! upon return contains number of Bytes written
LOADER_ErrorCode
Loader_StrConvert(LOADER_Char**          ppszDestination,
                  LOADER_StringEncoding  dstEncoding,
                  const LOADER_Char*     pszSource,
                  LOADER_Int4            lSourceLength,
                  LOADER_StringEncoding  srcEncoding);


inline void
Loader_StrTrimRight(LOADER_Byte* pszString, LOADER_Int4& lStrLength, LOADER_Byte cTrimVal)
{
    LOADER_Byte *pszStringEnd = pszString + lStrLength - sizeof(LOADER_Char);

    while (pszString <= pszStringEnd)
    {
        if (cTrimVal != *pszStringEnd)
        {
            break;
        }
        --pszStringEnd;
    }
    lStrLength = pszStringEnd - pszString + sizeof(LOADER_Char);
}

void
Loader_StrTrimRight(LOADER_Byte*      pszString,
                    LOADER_Int4&      lStrLength,
                    LOADER_HostType   CharCodeType,
                    LOADER_Byte       cTrimVal = LOADER_BLANK);

void
Loader_StrTrimRight(LOADER_Byte*           pszString,
                    LOADER_Int4&           lStrLength,
                    tsp77encoding const*   pCharCodeType,
                    LOADER_Byte            cTrimVal = LOADER_BLANK);


void 
LOADER_StrTrimLeftEx(LOADER_Char* pszString,
                     LOADER_Int4& lStrLength);


void
Loader_StrInsertFillBytes(LOADER_Byte*        pszData,
                          LOADER_Int4         lDataLen,
                          Loader_ColumnData*  pFieldData);


LOADER_ErrorCode
Loader_X2C(LOADER_Char*  pszHexString,
           LOADER_Int4   lStringLength,
           LOADER_Byte*  pszBinaryBuffer,
           LOADER_Int4&  lBufferLength);


/*!
  -----------------------------------------------------------------------------
 
  function:     Loader_SwapUCS2StringInPlace
  -----------------------------------------------------------------------------

  description:  Swaps every 2 byte in a string.

  arguments:    pszSource  [IN/OUT]  - source value
                lSourceLen [IN]      - source length

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
Loader_SwapUCS2StringInPlace(LOADER_Byte*  pszSource, LOADER_Int4  lSourceLen);


/*!
  -----------------------------------------------------------------------------
 
  function:     Loader_Swap2BIntToChar
  -----------------------------------------------------------------------------

  description:  Swaps every 2 byte int to char.

  arguments:    
  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
Loader_Swap2BIntToChar(LOADER_Int2       nValue,
                       LOADER_Byte*      pszBuffer,
                       LOADER_Bool       bSwapIt)
{
    if (LOADER_FALSE == bSwapIt)
    {
        pszBuffer[0] = (reinterpret_cast<LOADER_Byte*>(&nValue))[0];
        pszBuffer[1] = (reinterpret_cast<LOADER_Byte*>(&nValue))[1];
    }
    else
    {
        pszBuffer[0] = (reinterpret_cast<LOADER_Byte*>(&nValue))[1];
        pszBuffer[1] = (reinterpret_cast<LOADER_Byte*>(&nValue))[0];
    }
}



/*!
  -----------------------------------------------------------------------------
 
  function:     Loader_Swap4BIntToChar
  -----------------------------------------------------------------------------

  description:  Swaps every 4 byte int to char.

  arguments:    
  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
Loader_Swap4BIntToChar(LOADER_Int4       nValue,
                       LOADER_Byte*      pszBuffer,
                       LOADER_SwapKind  SSwap,
                       LOADER_SwapKind  DSwap);


LOADER_Int4
Loader_Swap4BInt(LOADER_Int4       lSource,
                 LOADER_SwapKind  SSwap,
                 LOADER_SwapKind  DSwap);

#endif //LOADER_STRINGCONVERSION_HPP