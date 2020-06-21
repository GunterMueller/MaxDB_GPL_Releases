/*!
  \file    Loader_StringConversion.cpp
  \author  SteffenS
  \brief   Implementation of string conversion routines

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
//#include <stdio.h>
#include <ctype.h>          // isspace

//#include "SAPDBCommon/SAPDB_string.h"       // SAPDB_memcpy
#include "Loader/Loader_Types.hpp"             // Table information structure tsp7_table_info 
#include "Loader/Loader_Common.hpp"
#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_StringConversion.hpp"
#include "hsp78_0.h"
#include "hsp81.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"  // unicode support

/*
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration 
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------

  function:     Loader_StrConvertUTF8ToUCS2
  -----------------------------------------------------------------------------

  description:  Converts a string from UTF8 to UCS2.

  arguments:    pszUCS2    [OUT]     - destination string
                lUCS2Len   [IN/OUT]  - length of destination string in Bytes
                pszUTF8    [IN]      - source string
                lUTF8Len   [IN]      - length of source string in Bytes
                UCS2Type   [IN]      - type of UCS2 encoding (swapped or not)

  returnvalue:  destination string length in lUCS2Len

                return code:
                errOK_els00                 - successful conversion
                errInternal_els98           - when partial character in source,
                                                but hit end or insufficient
                                                space in target for conversion
                errConvertingEncoding_els98 - source contains invalid chars or
                                               char sequences
  -----------------------------------------------------------------------------
*/
static LOADER_ErrorCode
Loader_StrConvertUTF8ToUCS2(LOADER_Byte*           pszUCS2,
                            LOADER_UInt4&          lUCS2Len,
                            const LOADER_Byte*     pszUTF8,
                            LOADER_UInt4           lUTF8Len,
                            LOADER_StringEncoding  UCS2Type);


/*
  -----------------------------------------------------------------------------

  function:     ls05_ConvertUTF8ToASCII - overloaded
  -----------------------------------------------------------------------------

  description:  Converts a string from UTF8 to ASCII .

  arguments:    pszASCII     [OUT]     - destination string
                lASCIILen    [IN/OUT]  - length of destination string in Bytes
                Destination  [IN]      - source string
                
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
static LOADER_ErrorCode
Loader_StrConvertUTF8ToASCII(LOADER_Char*                     pszASCII,
                             LOADER_UInt4&                    lASCIILen,
                             const Tools_DynamicUTF8String&  Source);


/*
  -----------------------------------------------------------------------------

  function:     ls05_ConvertUTF8ToASCII - overloaded
  -----------------------------------------------------------------------------

  description:  Converts a string from UTF8 to ASCII .

  arguments:    pszASCII   [OUT]     - destination string
                lASCIILen  [IN/OUT]  - length of destination string in Bytes
                pszUTF8    [IN]      - source string
                lUTF8Len   [IN]      - source string length in Bytes
                
  returnvalue:  destination string length in lASCIILen

                return code:
                errOK_els00                 - successful conversion
                errInternal_els98           - when partial character in source,
                                                but hit end or insufficient
                                                space in target for conversion
                errConvertingEncoding_els98 - source contains invalid chars or
                                               char sequences
  -----------------------------------------------------------------------------

static tsp00_Longint
ls05_ConvertUTF8ToASCII(char            *pszASCII,
                        tsp00_Longuint  &lASCIILen,
                        unsigned char   *pszUTF8,
                        tsp00_Longuint   lUTF8Len);
*/

/*
  -----------------------------------------------------------------------------

  function:     ls05_ConvertASCIIToUTF8
  -----------------------------------------------------------------------------

  description:  Converts a string from ASCII to UTF8.

  arguments:    Destination  [OUT]  - destination string
                pszASCII     [IN]   - source string
                lASCIILen    [IN]   - length of source string in Bytes

  returnvalue:  return code:
                errOK_els00                 - successful conversion
                errInternal_els98           - when partial character in source,
                                                but hit end or insufficient
                                                space in target for conversion
                errConvertingEncoding_els98 - source contains invalid chars or
                                               char sequences
  -----------------------------------------------------------------------------

static tsp00_Longint
ls05_ConvertASCIIToUTF8(Tools_DynamicUTF8String &Destination,
                        unsigned char           *pszASCII,
                        tsp00_Longuint           lASCIILen);
*/

/*
  -----------------------------------------------------------------------------

  function:     Loader_StrConvertASCIIToUCS2
  -----------------------------------------------------------------------------

  description:  Converts a string from ASCII to UCS2.

  arguments:    pszUCS2     [IN]     - destination string
                lUCS2Len    [IN/OUT] - length of destination string in Bytes
                pszASCII    [OUT]    - source string
                lASCIILen   [IN]     - length of source string in Bytes
                UCS2Type    [IN]     - type of UCS2 encoding (swapped or not)

  returnvalue:  destination string length in lUCS2Len

                return code:
                errOK_els00                 - successful conversion
                errInternal_els98           - when partial character in source,
                                                but hit end or insufficient
                                                space in target for conversion
                errConvertingEncoding_els98 - source contains invalid chars or
                                               char sequences
  -----------------------------------------------------------------------------
*/
static LOADER_ErrorCode
Loader_StrConvertASCIIToUCS2(LOADER_Byte*           pszUCS2,
                             LOADER_UInt4&          lUCS2Len,
                             const LOADER_Byte*     pszASCII,
                             LOADER_UInt4           lASCIILen,
                             const tsp77encoding*  pUCS2Type);


/*
  -----------------------------------------------------------------------------

  function:     ls05_ConvertASCIIAnyCPToUCS2
  -----------------------------------------------------------------------------

  description:  Converts a string from ASCII code page X to UCS2.

  arguments:    pszUCS2       [IN]     - destination string
                lUCS2Len      [IN/OUT] - length of destination string in Bytes
                pszASCII      [OUT]    - source string
                lASCIILen     [IN]     - length of source string in Bytes
                UCS2Swapped   [IN]     - destination swapped or not
                pCodePage     [IN]     - code page structure to use for conversion

  returnvalue:  destination string length in lUCS2Len

                return code:
                errOK_els00                 - successful conversion
                errInternal_els98           - when partial character in source,
                                                but hit end or insufficient
                                                space in target for conversion
                errConvertingEncoding_els98 - source contains invalid chars or
                                               char sequences
  -----------------------------------------------------------------------------

static tsp00_Longint
ls05_ConvertASCIIAnyCPToUCS2(unsigned char       *pszUCS2,
                             tsp00_Longuint      &lUCS2Len,      // must have the available length!!!
                             const unsigned char *pszASCII,
                             tsp00_Longuint       lASCIILen,
                             int                  UCS2Swapped,
                             tsp81_CodePage      *pCodePage);   // muss immer angegeben sein, wird vom aufrufer gesteuert

*/
/*
  -----------------------------------------------------------------------------

  function:     ls05_ConvertUCS2ToUTF8
  -----------------------------------------------------------------------------

  description:  Converts a string from UCS2 to UTF8.

  arguments:    Destination  [OUT]  - destination string
                pszSource    [IN]   - source string
                lSourceLen   [IN]   - length of source string in Bytes
                UCS2Type     [IN]   - type of UCS2 encoding (swapped or not)

  returnvalue:  return code:
                errOK_els00                 - successful conversion
                errInternal_els98           - when partial character in source,
                                                but hit end or insufficient
                                                space in target for conversion
                errConvertingEncoding_els98 - source contains invalid chars or
                                               char sequences
  -----------------------------------------------------------------------------

static tsp00_Longint
ls05_ConvertUCS2ToUTF8(Tools_DynamicUTF8String  &Destination,
                       unsigned char            *pszSource,
                       tsp00_Longint             lSourceLen,
                       tls00_CodeType            UCS2Type);     // gives a hint on swap type
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05_StrCmpPaddedAscii
  -----------------------------------------------------------------------------
  description:  Compares 2 ASCII values whereas the smaller is filled with blanks 
                before comparison.

  arguments:   pszShorterVal  [IN]  - shorter value
               lShorterLen    [IN]  - length of the shorter value (in Bytes)
               pszLongerVal   [IN]  - longer value
               lLongerLen     [IN]  - length of the longer value (in Bytes)

  returnvalue:  true    - values are equal<br>
                false   - values are not equal
  -----------------------------------------------------------------------------

static LOADER_Bool
ls05_StrCmpPaddedAscii(const LOADER_Byte*  pszShorterVal,
                       LOADER_UInt4        lShorterLen,
                       const LOADER_Byte*  pszLongerVal,
                       LOADER_UInt4        lLongerLen);
*/
/*
  -----------------------------------------------------------------------------
  function:     ls05_StrCmpPaddedUCS2
  -----------------------------------------------------------------------------
  description:  Compares 2 UCS2 values whereas the smaller is filled with blanks 
                before comparison.

  arguments:   pszShorterVal  [IN]  - shorter value
               lShorterLen    [IN]  - length of the shorter value (in Bytes)
               pszLongerVal   [IN]  - longer value
               lLongerLen     [IN]  - length of the longer value (in Bytes)
               CodeType       [IN]  - type of UCS (swapped or not)

  returnvalue:  true    - values are equal<br>
                false   - values are not equal
  -----------------------------------------------------------------------------

static LOADER_Bool
ls05_StrCmpPaddedUCS2(const LOADER_Byte*  pszShorterVal,
                      LOADER_UInt4        lShorterLen,     // length in characters not bytes
                      const LOADER_Byte*  pszLongerVal,
                      LOADER_UInt4        lLongerLen,      // length in characters not bytes
                      tls00_CodeType     CodeType);
*/
/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions declaration 
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  Chapter: Public functions implementation
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  function:     ls05StrToUpr
  -----------------------------------------------------------------------------
*/
// Caller have to make sure the strings coming here are already UTF8!!
void Loader_StrToUpr(LOADER_Char* pszString, LOADER_Int4 lStringLength)
{
    ROUTINE_DBG_MEO00 ("ls05StrToUpr");

    // The function ToUpper() assures inplace replacement; so we can
    // safely copy the new value to the old place; but the performance ...
    Tools_DynamicUTF8String s(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pszString));
    s.ToUpper();
    SAPDB_memcpy(pszString, s.StrPtr(), lStringLength);
}
//  Loader_StrToUpr()


/*
  -----------------------------------------------------------------------------
  function:     ls05StrTrimRight - overloaded
  -----------------------------------------------------------------------------

void ls05StrTrimRight(tsp00_Addr pszString, tsp00_Int4& lStrLength)
{
    ROUTINE_DBG_MEO00 ("ls05StrTrimRight");

    tsp00_Addr  szLast      = NULL;
    tsp00_Addr  pszMyString = pszString;
    int         i           = 0;

    while (i <= lStrLength - 1)
    {
        if (isspace(*pszMyString))
        {
            if (szLast == NULL)
                szLast = pszMyString;
        }
        else
        {
            szLast = NULL;
        }

        pszMyString++;
        i++;
    }

    if (szLast != NULL)
    {
        // calculate string length newly
        lStrLength = STATIC_CAST(tsp00_Int4, (szLast - (pszString)) );
    }
}
// ls05StrTrimRight()
*/

/*
  -----------------------------------------------------------------------------
  function:     Loader_StrTrimRight - overloaded
  -----------------------------------------------------------------------------
*/
void
/*Loader_StrTrimRight(LOADER_Char*      pszString,
                    LOADER_Int4&      lStrLength,
                    LOADER_HostType   CharCodeType,
                    LOADER_Char       cTrimVal)*/
Loader_StrTrimRight(LOADER_Byte*      pszString,
                    LOADER_Int4&      lStrLength,
                    LOADER_HostType   CharCodeType,
                    LOADER_Byte       cTrimVal)
{
    LOADER_Int4 lCount = 0;

    if ( (LOADER_HostType_ASCII == CharCodeType) || (LOADER_HostType_UTF8 == CharCodeType) )
    {
        return Loader_StrTrimRight(pszString, lStrLength, cTrimVal);
    }
    else if (LOADER_HostType_UCS2 == CharCodeType)
    {
        lCount = sp77encodingUCS2->countPadChars(//STATIC_CAST(void*, pszString),
                                                 pszString,
                                                 static_cast<LOADER_UInt4>(lStrLength),
                                                 cTrimVal);
    }
    else if (LOADER_HostType_UCS2_SWAPPED == CharCodeType)
    {
        lCount = sp77encodingUCS2Swapped->countPadChars(pszString,
                                                        static_cast<LOADER_UInt4>(lStrLength),
                                                        cTrimVal);
    }
/*    switch (CharCodeType)
    {
        case LOADER_HostType_UCS2_SWAPPED:
        {
            lCount = sp77encodingUCS2Swapped->countPadChars(STATIC_CAST(void*, pszString),
                                                            STATIC_CAST(tsp00_Uint4, lStrLength),
                                                            cTrimVal);
            break;
        }
        case LOADER_HostType_UCS2:
        {
            lCount = sp77encodingUCS2->countPadChars(STATIC_CAST(void*, pszString),
                                                     STATIC_CAST(tsp00_Uint4, lStrLength),
                                                     cTrimVal);
            break;
        }
//        case LOADER_HostType_UCS2_NATIVE:
//        {
//            lCount = sp77encodingUCS2Native->countPadChars(STATIC_CAST(void*, pszString),
//                                                           STATIC_CAST(tsp00_Uint4, lStrLength),
//                                                           cTrimVal);
//            break;
//        }
        default: // covers ctAscii_els00 and ctUTF8_els00 (trim char may only be US7ASCII)
        {
            Loader_StrTrimRight(pszString, lStrLength, cTrimVal);
        }
    }
*/
    lStrLength -= (2 * lCount); // Attention: in case of ASCII/UTF8 we rely on the initialization of lCount
    return;
}
// Loader_StrTrimRight()



/*
  -----------------------------------------------------------------------------
  function:     Loader_StrTrimRight - overloaded for comfort
  -----------------------------------------------------------------------------
*/
void
/*Loader_StrTrimRight(LOADER_Char*           pszString,
                    LOADER_Int4&           lStrLength,
                    tsp77encoding const*  pCharCodeType,
                    LOADER_Char            cTrimVal)*/
Loader_StrTrimRight(LOADER_Byte*           pszString,
                    LOADER_Int4&           lStrLength,
                    tsp77encoding const*   pCharCodeType,
                    LOADER_Byte            cTrimVal)
{
    if ( (csp_ascii == pCharCodeType->EncodingType) || (csp_utf8 == pCharCodeType->EncodingType) )
    {
        Loader_StrTrimRight(pszString, lStrLength, cTrimVal);
    }
    else
    {
        LOADER_Int4 lCount =  pCharCodeType->countPadChars(pszString,
                                                           static_cast<LOADER_UInt4>(lStrLength),
                                                           cTrimVal);
        lStrLength -= (2 * lCount);
    }
    return;
}
// Loader_StrTrimRight()



void
Loader_SwapUCS2StringInPlace(LOADER_Byte*  pszSource, LOADER_Int4  lSourceLen)
{
    register LOADER_Byte ci;
    for (--lSourceLen; lSourceLen > 0; lSourceLen -= 2)
    {
        ci                        = pszSource[lSourceLen];
        pszSource[lSourceLen]     = pszSource[lSourceLen - 1];
        pszSource[lSourceLen - 1] = ci;
    }
    return;
}

/*
void
//Loader_Swap2BIntToChar(LOADER_Int2      nValue,
//                       LOADER_Byte*     pszBuffer,
//                       LOADER_SwapKind  SSwap,
//                       LOADER_SwapKind  DSwap)
//                       LOADER_Bool       bSwapIt)
Loader_Swap2BIntToChar(LOADER_Int2      nValue,
                       LOADER_Byte*     pszBuffer,
                       LOADER_Bool       bSwapIt)
{
    //if (SSwap == DSwap)
    //{
    //    pszBuffer[0] = (reinterpret_cast<LOADER_Byte*>(&nValue))[0];
    //    pszBuffer[1] = (reinterpret_cast<LOADER_Byte*>(&nValue))[1];
    //}
    //else
    //{
    //    pszBuffer[0] = (reinterpret_cast<LOADER_Byte*>(&nValue))[1];
    //    pszBuffer[1] = (reinterpret_cast<LOADER_Byte*>(&nValue))[0];

    //}

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
// Loader_Swap2BIntToChar()
*/

void
Loader_Swap4BIntToChar(LOADER_Int4      nValue,
                       LOADER_Byte*     pszBuffer,
                       LOADER_SwapKind  SSwap,
                       LOADER_SwapKind  DSwap)
{
    if (SSwap == DSwap)
    {
        SAPDB_memcpy( pszBuffer, &nValue, sizeof(nValue) );
    }
    else if ( ((LOADER_SWAPKIND_NORMAL == SSwap) && (LOADER_SWAPKIND_SWAPPED == DSwap)) ||
              ((LOADER_SWAPKIND_NORMAL == DSwap) && (LOADER_SWAPKIND_SWAPPED == SSwap)) )
    {
        pszBuffer[0] = (reinterpret_cast<LOADER_Byte*>(&nValue))[3];
        pszBuffer[1] = (reinterpret_cast<LOADER_Byte*>(&nValue))[2];
        pszBuffer[2] = (reinterpret_cast<LOADER_Byte*>(&nValue))[1];
        pszBuffer[3] = (reinterpret_cast<LOADER_Byte*>(&nValue))[0];
    }
    else if ( ((LOADER_SWAPKIND_NORMAL == SSwap) && (LOADER_SWAPKIND_PART_SWAPPED == DSwap)) ||
              ((LOADER_SWAPKIND_NORMAL == DSwap) && (LOADER_SWAPKIND_PART_SWAPPED == SSwap)) )
    {
        pszBuffer[0] = (reinterpret_cast<LOADER_Byte*>(&nValue))[1];
        pszBuffer[1] = (reinterpret_cast<LOADER_Byte*>(&nValue))[0];
        pszBuffer[2] = (reinterpret_cast<LOADER_Byte*>(&nValue))[3];
        pszBuffer[3] = (reinterpret_cast<LOADER_Byte*>(&nValue))[2];
    }
    else if ( ((LOADER_SWAPKIND_SWAPPED == SSwap) && (LOADER_SWAPKIND_PART_SWAPPED == DSwap)) ||
              ((LOADER_SWAPKIND_SWAPPED == DSwap) && (LOADER_SWAPKIND_PART_SWAPPED == SSwap)) )
    {
        pszBuffer[0] = (reinterpret_cast<LOADER_Byte*>(&nValue))[2];
        pszBuffer[1] = (reinterpret_cast<LOADER_Byte*>(&nValue))[3];
        pszBuffer[2] = (reinterpret_cast<LOADER_Byte*>(&nValue))[0];
        pszBuffer[3] = (reinterpret_cast<LOADER_Byte*>(&nValue))[1];
    }
}
// Loader_Swap4BIntToChar()


LOADER_Int4
Loader_Swap4BInt(LOADER_Int4       lSource,
                 LOADER_SwapKind  SSwap,
                 LOADER_SwapKind  DSwap)
{
    LOADER_Int4 lTarget = 0;
    
    if (SSwap == DSwap)
    {
        lTarget = lSource;
    }
    else if ( ((LOADER_SWAPKIND_NORMAL == SSwap) && (LOADER_SWAPKIND_SWAPPED == DSwap)) ||
              ((LOADER_SWAPKIND_NORMAL == DSwap) && (LOADER_SWAPKIND_SWAPPED == SSwap)) )
    {
        reinterpret_cast<char*>(&lTarget)[0] = reinterpret_cast<char*>(&lSource)[3];
        reinterpret_cast<char*>(&lTarget)[1] = reinterpret_cast<char*>(&lSource)[2];
        reinterpret_cast<char*>(&lTarget)[2] = reinterpret_cast<char*>(&lSource)[1];
        reinterpret_cast<char*>(&lTarget)[3] = reinterpret_cast<char*>(&lSource)[0];
    }
    else if ( ((LOADER_SWAPKIND_NORMAL == SSwap) && (LOADER_SWAPKIND_PART_SWAPPED == DSwap)) ||
              ((LOADER_SWAPKIND_NORMAL == DSwap) && (LOADER_SWAPKIND_PART_SWAPPED == SSwap)) )
    {
        reinterpret_cast<char*>(&lTarget)[0] = REINTERPRET_CAST(char*, &lSource)[1];
        reinterpret_cast<char*>(&lTarget)[1] = REINTERPRET_CAST(char*, &lSource)[0];
        reinterpret_cast<char*>(&lTarget)[2] = reinterpret_cast<char*>(&lSource)[3];
        reinterpret_cast<char*>(&lTarget)[3] = reinterpret_cast<char*>(&lSource)[2];
    }
    else if ( ((LOADER_SWAPKIND_SWAPPED == SSwap) && (LOADER_SWAPKIND_PART_SWAPPED == DSwap)) ||
              ((LOADER_SWAPKIND_SWAPPED == DSwap) && (LOADER_SWAPKIND_PART_SWAPPED == SSwap)) )
    {
        reinterpret_cast<char*>(&lTarget)[0] = reinterpret_cast<char*>(&lSource)[2];
        reinterpret_cast<char*>(&lTarget)[1] = reinterpret_cast<char*>(&lSource)[3];
        reinterpret_cast<char*>(&lTarget)[2] = reinterpret_cast<char*>(&lSource)[0];
        reinterpret_cast<char*>(&lTarget)[3] = reinterpret_cast<char*>(&lSource)[1];
    }

    return lTarget;
}
//  ls07Swap4BInt()


/*
  -----------------------------------------------------------------------------
  function:     ls05StrTrimLeft - overloaded
  -----------------------------------------------------------------------------

void
ls05StrTrimLeft(tsp00_Addr       pBuffer,
                tsp00_Longint   &lBufferLength,
                char             cTrimChar,
                tls00_CodeType   CodeType)
{
    ROUTINE_DBG_MEO00 ("ls05StrTrimLeft II");

    bool          bdone = false;
    tsp00_Longint i     = 0;

    // Hmm, we suppose that trim char is ALWAYS blank in case of UCS2 !
    if (true == ls00IsUCS2(&CodeType))
    {
        register char c1 = 0;
        register char c2 = 0;
        if (0 == pEncodings_ls00[CodeType]->charTable->blank.bytes[0])
        {
            // UCS2 normal
            c2 = BLANK_LS00;
        }
        else
        {
            // UCS2 swapped
            c1 = BLANK_LS00;
        }
        for (; ( (i < lBufferLength) && (pBuffer[i] == c1) && (pBuffer[i+1] == c2) ); i += 2)
            ;

        if ( (0 != i) && (lBufferLength != i) )
        {
            lBufferLength -= i;
            SAPDB_memmove(pBuffer, &pBuffer[i], lBufferLength*sizeof(char));              
        }
        else if (lBufferLength == i)
        {
            lBufferLength = 0;
        }
    }
	else if (ctUTF8_els00 == CodeType)
	{
		while ( (LOADER_TRUE == pEncodings_ls00[CodeType]->isSpace(&pBuffer[i])) && (i < lBufferLength) )
			++i;

		if ( (0 != i) && (i != lBufferLength) )
		{
			lBufferLength -= i;
			SAPDB_memmove(pBuffer, &pBuffer[i], lBufferLength);
		}
		else if (i == lBufferLength)
		{
			lBufferLength = 0;
		}
	}
    else
    {
        // covers ctAscii_els00 (trim char may only be US7ASCII)
        ls05StrTrimLeftEx(pBuffer, lBufferLength);
    }

//    switch (CodeType)
    //{
    //    case ctUCS2Swapped_els00:
    //    {
    //        for (; ( (i < lBufferLength) && (pBuffer[i] == cTrimChar) && (pBuffer[i+1] == 0) ); i += 2)
    //            ;
    //        bdone = true;
    //    // ATTENTION: Fall through  
    //    }
    //    case ctUCS2_els00:
    //    {
    //        if (false == bdone)
    //        {
    //            for (; ( (i < lBufferLength) && (pBuffer[i] == 0) && (pBuffer[i+1] == cTrimChar) ); i += 2)
    //                ;
    //        }
    //        
    //        if ( (0 != i) && (lBufferLength != i) )
    //        {
    //            lBufferLength -= i;
    //            SAPDB_memmove(pBuffer, &pBuffer[i], lBufferLength*sizeof(char));              
    //        }
    //        else if (lBufferLength == i)
    //        {
    //            lBufferLength = 0;
    //        }
    //        break;
    //    }
    //    default: // covers ctAscii_els00 and ctUTF8_els00 (trim char may only be US7ASCII)
    //    {
    //        ls05StrTrimLeftEx(pBuffer, lBufferLength);
    //    }
    //}
    return;
}
// ls05StrTrimLeft()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05StrTrimRightEx
  -----------------------------------------------------------------------------

void ls05StrTrimRightEx(tsp00_Addr pszString)
{   
    ROUTINE_DBG_MEO00 ("ls05StrTrimRightEx");

    tsp00_Addr pszLast = NULL;
    tsp00_Addr pszS    = pszString;

    while (0 != *pszS)
    {
        if (isspace(*pszS))
        {
            if (NULL == pszLast)
            {
                pszLast = pszS;
            }
        }
        else
        {
            pszLast = NULL;
        }

        ++pszS;
    }
    if (NULL != pszLast)    // truncate at trailing space start
    {
        *pszLast = '\0';
    }
}
// ls05StrTrimRightEx()
*/

/*
  -----------------------------------------------------------------------------
  function:     Loader_StrTrimLeftEx
  -----------------------------------------------------------------------------
*/
void LOADER_StrTrimLeftEx(LOADER_Char* pszString, LOADER_Int4& lStrLength)
{
   LOADER_Int4 _len = 0;

    while ( (isspace(pszString[_len])) && (_len < lStrLength) )
        ++_len;


    if ( (0 != _len) && (_len != lStrLength) )
    {
        lStrLength -= _len;
        SAPDB_memmove(pszString, &pszString[_len], lStrLength);
    }
    else if (_len == lStrLength)
    {
        lStrLength = 0;
    }
}
// Loader_StrTrimLeftEx()


/*
  -----------------------------------------------------------------------------
  function:     ls05StrCmpWFill
  -----------------------------------------------------------------------------

bool ls05StrCmpWFill(const tsp00_Addr pszValue1,
                     tsp00_Int4 lVal1Len,
                     const tsp00_Addr pszValue2,
                     tsp00_Int4 lVal2Len)
{
    ROUTINE_DBG_MEO00 ("ls05StrCmpWFill");

    bool        bEqual = true;

    tsp00_Int4  lCompLen = lVal1Len;
    tsp00_Addr  pszCompVal = pszValue2;

    tsp00_Int4  lDif = (lVal2Len - lCompLen);

    if (lDif < 0)
    {
        lCompLen    = lVal2Len;
        pszCompVal  = pszValue1;
    }

    if (0 == strncmp(pszValue2, pszValue1, lCompLen) )
    {
        if (lDif != 0)
        {
            tsp00_Int2 i = 0;

            pszCompVal = pszCompVal + lCompLen;

            lDif = (lDif > 0) ? lDif : -lDif;
            for (i; i < lDif; ++i)
            {
                if (pszCompVal[i] != ' ')
                {
                    bEqual = false;
                    break;
                }
            }
        }
    }
    else
    {
        bEqual = false;
    }

    return bEqual;
}
// ls05StrCmpWFill()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05StrCmpPadded
  -----------------------------------------------------------------------------

LOADER_Bool
ls05StrCmpPadded(const LOADER_Byte*  pszValue1,
                 LOADER_UInt4        lVal1Len,
                 const LOADER_Byte*  pszValue2,
                 LOADER_UInt4        lVal2Len,
                 tls00_CodeType     CodeType)
{
    ROUTINE_DBG_MEO00 ("ls05StrCmpPadded");

    bool              bEqual        = LOADER_TRUE;
    const LOADER_Byte* pszShorterVal = pszValue1;
    LOADER_UInt4       lShorterLen   = lVal1Len;
    const LOADER_Byte* pszLongerVal  = pszValue2;
    LOADER_UInt4       lLongerLen    = lVal2Len;

    if (lVal2Len < lVal1Len)
    {
        pszShorterVal = pszValue2;
        lShorterLen   = lVal2Len;
        pszLongerVal  = pszValue1;
        lLongerLen    = lVal1Len;
    }

    switch (CodeType)
    {
        // ATTENTION: triple fall through
        case ctUCS2_els00:
        case ctUCS2Swapped_els00:
        case ctUCS2Native_els00:
        {
            bEqual = ls05_StrCmpPaddedUCS2(pszShorterVal, lShorterLen/2, pszLongerVal, lLongerLen/2, CodeType);
            break;
        }
        default: // covers ctAscii_els00 and  ctUTF8_els00
        {
            bEqual = ls05_StrCmpPaddedAscii(pszShorterVal, lShorterLen, pszLongerVal, lLongerLen);
        }
    }

    return bEqual;
}
// ls05StrCmpPadded()
*/

/*
  -----------------------------------------------------------------------------
  function:     Loader_StrInsertFillBytes
  -----------------------------------------------------------------------------
*/
// Attention - optimized (and thus usable) ONLY for UCS2 and ASCII
void
Loader_StrInsertFillBytes(LOADER_Byte*         pszData,
                          LOADER_Int4          lDataLen,
                          Loader_ColumnData*   pFieldData)
{
    // All character based data is filled with the ascii filler ' ' except char byte fields
    LOADER_UInt4  lDif = pFieldData->SQLColumnInfo.ColumnBasics.SQLInOutLength - lDataLen;
    if (0 < lDif)
    {
        if (0 == pFieldData->bUnicodeConversion)
        {
            memset(pszData + lDataLen, pFieldData->cFillByte, lDif);
        }
        else //if (LOADER_STRINGENCODING_UCS2 == Encoding)
        {
            // It is UCS2 here, always
            memset(pszData + lDataLen, 0, lDif);
            for (LOADER_Int4 i = lDataLen + 1;
                 i <= pFieldData->SQLColumnInfo.ColumnBasics.SQLInOutLength;
                 pszData[i] = DEFINED_BYTE_ASCII, i+=2)
                 ;
            /*for (LOADER_Int4 i = lDataLen;
                i < pFieldData->SQLColumnInfo.ColumnBasics.SQLInOutLength;
                pszData[i++] = DEFINED_BYTE, pszData[i++] = DEFINED_BYTE_ASCII)
                ;*/
        }
    }
    return;
}
// Loader_StrInsertFillBytes


/*!
  -----------------------------------------------------------------------------
  function:     ls05AreEncodingsCompatible
  -----------------------------------------------------------------------------

tsp00_Longint
ls05AreEncodingsCompatible(tls00_CodeType DataFile_Encoding,
                           tls00_CodeType DB_Encoding,
                           bool           bLoad)
{
    tsp00_Longint rc = errOK_els00;

    if (true == bLoad)
    {
        if ( (ctAscii_els00 != DataFile_Encoding) && (ctAscii_els00 == DB_Encoding) )
        {
            rc = errIncompatibleEncodings_els98;
        }
    }
    else
    {
        if ( (ctAscii_els00 == DataFile_Encoding) && (ctAscii_els00 != DB_Encoding) )
        {
            rc = errIncompatibleEncodings_els98;
        }
    }
    return rc;
}
// ls05CheckExternalEncoding()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05ConvertToUCS2
  -----------------------------------------------------------------------------

tsp00_Longint
ls05ConvertToUCS2(unsigned char  *pszDestination,
                  tsp00_Longuint &lDestinationLen,
                  unsigned char  *pszSource,
                  tsp00_Longint   lSourceLen,
                  tls00_CodeType  SrcEncoding,
                  tsp00_DataType  DBDataType,
                  tsp00_Addr      pszErrText,
                  tsp81_CodePage *pASCII_CP)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertToUCS2");

    tsp00_Longint rc = errOK_els00;

    // if db data type is binary or if external code type is UCS2  no conversion applied
    // external code type UCS2 is checked by called func
    if ( (dstrb != DBDataType) && (dstrdb != DBDataType) )
    {
        rc = ls05ConvertToUCS2Simple(pszDestination, lDestinationLen, pszSource, lSourceLen, SrcEncoding, pszErrText, pASCII_CP);
    }
    return rc;
}
// ls05ConvertToUCS2()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05ConvertToUCS2Simple
  -----------------------------------------------------------------------------

tsp00_Longint
ls05ConvertToUCS2Simple(unsigned char  *pszDestination,
                        tsp00_Longuint &lDestinationLen,
                        unsigned char  *pszSource,
                        tsp00_Longint   lSourceLen,
                        tls00_CodeType  SrcEncoding,
                        tsp00_Addr      pszErrText,
                        tsp81_CodePage *pASCII_CP)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertToUCS2Simple");

    tsp00_Longint rc = errOK_els00;

    if ( (ctUCS2_els00 != SrcEncoding) && (ctUCS2Swapped_els00 != SrcEncoding) )        // source encoding may not be already ucs2
    {
        if (ctUTF8_els00 == SrcEncoding)
        {
            rc = ls05_ConvertUTF8ToUCS2(pszDestination, lDestinationLen, pszSource, lSourceLen, ctUCS2Native_els00);
        }
        else
        {
            // Coming here code page structure might be NULL-pointer or filled.
            int UCS2Swapped = STATIC_CAST(int, ls07Swapped(NULL));

            rc = ls05_ConvertASCIIAnyCPToUCS2(pszDestination, lDestinationLen, pszSource, lSourceLen, UCS2Swapped, pASCII_CP);
        }
    }

    // Both conversion functions return identical values for
    //  errOK_els00                      : conversion successful; length of converted string is returned in lDestinationLen
    //  errEncodingSourceExhausted_els98 : partial character in source, but hit end
    //  errConvertingEncoding_els98      : source contains invalid chars or char sequences)
    //  errEncodingTargetExhausted_els98 : insuff. space in target for conversion)
    if (errOK_els00 != rc)
    {
        if ( errConvertingEncoding_els98 != rc )
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternal_els98,
                                                                              _INTERNAL_FILE_ID_, __LINE__, rc);
            rc = errInternal_els98;
        }
        else
        {
            if (ctAscii_els00== SrcEncoding)
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "ASCII", "UCS2");
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UTF8", "UCS2");
            }

            rc = errConvertingEncoding_els98;
        }
    }
    return rc;
}
// ls05ConvertToUCS2Simple()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05ConvertToUCS2Simple - overloaded
  -----------------------------------------------------------------------------

// This function converts to a given destination UCS2 (swapped or not) and does
// not define the native swap of the running server
tsp00_Longint
ls05ConvertToUCS2Simple(unsigned char  *pszDestination,
                        tsp00_Longuint &lDestinationLen,
                        tls00_CodeType  DestEncoding,
                        unsigned char  *pszSource,
                        tsp00_Longint   lSourceLen,
                        tls00_CodeType  SrcEncoding,
                        tsp00_Addr      pszErrText,
                        tsp81_CodePage *pASCII_CP)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertToUCS2Simple");

    tsp00_Longint rc = errOK_els00;

    if (false == ls00IsUCS2(&SrcEncoding))        // source encoding may not be already ucs2
    {
        if (ctUTF8_els00 == SrcEncoding)
        {
            rc = ls05_ConvertUTF8ToUCS2(pszDestination, lDestinationLen, pszSource, lSourceLen, DestEncoding);
        }
        else
        {
            // Coming here code page structure might be NULL-pointer or filled.
            //int _Swapped = STATIC_CAST(int, ls07Swapped(NULL));
            int _Swapped = 0;
            if (ctUCS2Swapped_els00 == DestEncoding)
            {
                _Swapped = 1;
            }
            else if (ctUCS2Native_els00 == DestEncoding)
            {
                _Swapped = STATIC_CAST(int, ls07Swapped(NULL));
            }
            rc = ls05_ConvertASCIIAnyCPToUCS2(pszDestination, lDestinationLen, pszSource, lSourceLen, _Swapped, pASCII_CP);
        }
    }

    // Both conversion functions return identical values for
    //  errOK_els00                      : conversion successful; length of converted string is returned in lDestinationLen
    //  errEncodingSourceExhausted_els98 : partial character in source, but hit end
    //  errConvertingEncoding_els98      : source contains invalid chars or char sequences)
    //  errEncodingTargetExhausted_els98 : insuff. space in target for conversion)
    if (errOK_els00 != rc)
    {
        if ( errConvertingEncoding_els98 != rc )
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternal_els98,
                                                                             _INTERNAL_FILE_ID_, __LINE__, rc);
            rc = errInternal_els98;
        }
        else
        {
            if (ctAscii_els00== SrcEncoding)
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                     errConvertingEncoding_els98, "ASCII", "UCS2");
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                     errConvertingEncoding_els98, "UTF8", "UCS2");
            }

            rc = errConvertingEncoding_els98;
        }
    }
    return rc;
}
// ls05ConvertToUCS2Simple()
*/
/*
  -----------------------------------------------------------------------------
  function:     Loader_StrConvertToUCS2NotSwapped
  -----------------------------------------------------------------------------
*/
// Error codes: LOA_ERR_INTERNAL, LOA_ERR_CONVERTING_ENCODING
LOADER_ErrorCode
Loader_StrConvertToUCS2NotSwapped(LOADER_Byte*           pszDestination,
                                  LOADER_UInt4&          lDestinationLen,
                                  LOADER_Byte*           pszSource,
                                  LOADER_UInt4           lSourceLen,
                                  LOADER_StringEncoding  SrcEncoding)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    if ( (LOADER_STRINGENCODING_UCS2 != SrcEncoding) && (LOADER_STRINGENCODING_UCS2SWAPPED != SrcEncoding) )
    {
        if (LOADER_STRINGENCODING_UTF8 == SrcEncoding)
        {
            rc = Loader_StrConvertUTF8ToUCS2(pszDestination, lDestinationLen, pszSource, lSourceLen, LOADER_STRINGENCODING_UCS2);
        }
        else
        {
            rc = Loader_StrConvertASCIIToUCS2(pszDestination, lDestinationLen, pszSource, lSourceLen, sp77encodingUCS2);
        }
    }

    // Both conversion functions return identical values for
    //  errOK_els00                      : conversion successful; length of converted string is returned in lDestinationLen
    //  errEncodingSourceExhausted_els98 : partial character in source, but hit end
    //  errConvertingEncoding_els98      : source contains invalid chars or char sequences)
    //  errEncodingTargetExhausted_els98 : insuff. space in target for conversion)

//TODOTODO FEHLERBEHANDLUNG
    if (0 != rc)
    {
        if (LOA_ERR_CONVERTING_ENCODING != rc)
        {
/*            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternal_els98,
                                                                              _INTERNAL_FILE_ID_, __LINE__, rc);
*/
            rc = LOA_ERR_INTERNAL;
        }
        else
        {
/*            if (ctAscii_els00 == DestEncoding)
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "ASCII", "UCS2");
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UTF8", "UCS2");
            }
*/
            rc = LOA_ERR_CONVERTING_ENCODING;
        }
    }

    return rc;
}
// Loader_StrConvertToUCS2NotSwapped()



/*
  -----------------------------------------------------------------------------
  function:     Loader_StrConvert
  -----------------------------------------------------------------------------
*/

// ATTENTION: Allocates memory for destination
LOADER_ErrorCode
Loader_StrConvert(LOADER_Char**          ppszDestination,
                  LOADER_StringEncoding  dstEncoding,
                  const LOADER_Char*     pszSource,
                  LOADER_Int4            lSourceLength,
                  LOADER_StringEncoding  srcEncoding)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    LOADER_Char*  _pszReturnValue;
    LOADER_UInt4  _buflen           = 0;
    LOADER_UInt4  _charLength       = 0;

    if (srcEncoding == LOADER_STRINGENCODING_ASCII)
    {
        _charLength = (lSourceLength >= 0)? lSourceLength : strlen(pszSource);
    }
    else
    {
        if (lSourceLength >= 0)
        {
            _charLength = lSourceLength;
        }
        else //if (-1 == lSourceLength)
        {
            LOADER_UInt4  _byteLength   = 0;
            LOADER_Int4   _isTerminated = 0;
            LOADER_Int4   _isCorrupted  = 0;
            LOADER_Int4   _isExhausted  = 0;

            pLOADER_Encoding[srcEncoding]->stringInfo(pszSource, -1, 0, &_charLength, &_byteLength, &_isTerminated, &_isCorrupted, &_isExhausted);
        }
        
    }
    _buflen = _charLength * 6;          // just in case it's UTF8
    _pszReturnValue = new LOADER_Char[_buflen + sizeof(char)];

    sp77sprintfUnicode(pLOADER_Encoding[dstEncoding], _pszReturnValue, _buflen,
                        "%=.*S",
                        pLOADER_Encoding[srcEncoding], _charLength, pszSource);

    *ppszDestination = _pszReturnValue;
    return rc;
}


/*
  -----------------------------------------------------------------------------
  function:     ls05ConvertFromUCS2
  -----------------------------------------------------------------------------

tsp00_Longint
ls05ConvertFromUCS2(unsigned char  *pszDestination,
                    tsp00_Longint  &lDestinationLen,
                    tls00_CodeType  DestEncoding,
                    unsigned char  *pszSource,
                    tsp00_Longint   lSourceLen,
                    tsp00_DataType  DBDataType,
                    tsp00_Addr      pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertFromUCS2");

    tsp00_Longint rc = errOK_els00;

    // if db data type is binary or if external code type is UCS2  no conversion applied
    // external code type UCS2 is checked by called func
    if ( (dstrb != DBDataType) && (dstrdb != DBDataType) )
    {
        rc = ls05ConvertFromUCS2Simple(pszDestination,
                                       pszSource,
                                       lDestinationLen,
                                       lSourceLen,
                                       DestEncoding,
                                       ctUCS2Native_els00,
                                       pszErrText);
    }
    
    return rc;
}
// ls05ConvertFromUCS2()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05ConvertFromUCS2Simple - overloaded
  -----------------------------------------------------------------------------

tsp00_Longint
ls05ConvertFromUCS2Simple(unsigned char  *pszDestination,
                          unsigned char  *pszSource,
                          tsp00_Longint  &lDestinationLen,
                          tsp00_Longint   lSourceLen,
                          tls00_CodeType  DestEncoding,
                          tls00_CodeType  SrcEncoding,
                          tsp00_Addr      pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertFromUCS2Simple I");

    tsp00_Longint rc = errOK_els00;

    if (false == ls00IsUCS2(&DestEncoding))        // destination encoding may not be already ucs2
    {
        tls00_CodeType _mySrcEncoding = SrcEncoding;
        if (ctUCS2Native_els00 == _mySrcEncoding)
        {
            if ('\xFF' == sp77encodingUCS2Native->charTable->indicator.bytes[0])    // swapped
            {
                _mySrcEncoding = ctUCS2Swapped_els00;
            }
            else
            {
                _mySrcEncoding = ctUCS2_els00;
            }
        }

        if (ctUTF8_els00 == DestEncoding)
        {
            Tools_DynamicUTF8String s;

            rc = ls05_ConvertUCS2ToUTF8(s, pszSource, lSourceLen, _mySrcEncoding);
            if (errOK_els00 == rc)
            {
                lDestinationLen = s.BasisSize();
                SAPDB_memcpy(pszDestination, s.StrPtr(), lDestinationLen);
                pszDestination[lDestinationLen] = 0;
            }
        }
        else
        {
            rc = ls05_ConvertUCS2ToASCII(pszDestination, lDestinationLen, pszSource, lSourceLen, pEncodings_ls00[_mySrcEncoding]);
        }
    }

   
    // Both conversion functions return identical values for
    //  errOK_els00                      : conversion successful; length of converted string is returned in lDestinationLen
    //  errEncodingSourceExhausted_els98 : partial character in source, but hit end
    //  errConvertingEncoding_els98      : source contains invalid chars or char sequences)
    //  errEncodingTargetExhausted_els98 : insuff. space in target for conversion)
    if (errOK_els00 != rc)
    {
        if (errConvertingEncoding_els98 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternal_els98,
                                                                              _INTERNAL_FILE_ID_, __LINE__, rc);
            rc = errInternal_els98;
        }
        else
        {
            if (ctAscii_els00 == DestEncoding)
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UCS2", "ASCII");
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UCS2", "UTF8");
            }

            rc = errConvertingEncoding_els98;
        }
    }

    return rc;
}
// ls05ConvertFromUCS2Simple()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05ConvertFromUCS2Simple
  -----------------------------------------------------------------------------

tsp00_Longint
ls05ConvertFromUCS2Simple(unsigned char  *pszDestination,
                          unsigned char  *pszSource,
                          tsp00_Longint  &lDestinationLen,
                          tsp00_Longint   lSourceLen,
                          tls00_CodeType  DestEncoding,
                          tsp00_Addr      pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertFromUCS2Simple II");

    tsp00_Longint rc = ls05ConvertFromUCS2Simple(pszDestination,
                                                 pszSource,
                                                 lDestinationLen,
                                                 lSourceLen,
                                                 DestEncoding,
                                                 ctUCS2Native_els00,
                                                 pszErrText);
    return rc;
}
// ls05ConvertFromUCS2Simple()
*/

/*
  -----------------------------------------------------------------------------
  function:     ls05ConvertToUTF8
  -----------------------------------------------------------------------------

tsp00_Longint
ls05ConvertToUTF8(Tools_DynamicUTF8String &Destination,
                  unsigned char           *pszSource,
                  tsp00_Longuint           lSourceLen,
                  tls00_CodeType           SrcEncoding,     // must give a hint on swapped or not - e.g. ctUCS2_els00 or ctUCS2Native_els00 or ctUCS2Swapped_els00
                  tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertToUTF8");

    tsp00_Longint rc = errOK_els00;

    if (ctUTF8_els00 != SrcEncoding)    // destination encoding MUST NOT be already UTF8
    {
        if (ctAscii_els00 == SrcEncoding)
        {
            rc = ls05_ConvertASCIIToUTF8(Destination, pszSource, lSourceLen);
        }
        else
        {
            rc = ls05_ConvertUCS2ToUTF8(Destination, pszSource, lSourceLen, SrcEncoding);
        }
    }

    // Both conversion functions return identical values for
    //  errOK_els00                      : conversion successful
    //  errEncodingSourceExhausted_els98 : partial character in source, but hit end
    //  errConvertingEncoding_els98      : source contains invalid chars or char sequences)
    //  errEncodingTargetExhausted_els98 : insuff. space in target for conversion)
    if (errOK_els00 != rc)
    {
        if ( errConvertingEncoding_els98 != rc )
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternal_els98,
                                                                              _INTERNAL_FILE_ID_, __LINE__, rc);
            rc = errInternal_els98;
        }
        else
        {
            if (ctAscii_els00 == SrcEncoding)
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "ASCII", "UTF8");
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UCS2", "UTF8");
            }
            rc = errConvertingEncoding_els98;
        }
    }
    return rc;
}
// ls05ConvertToUTF8()
*/

/*
  -----------------------------------------------------------------------------
  function:     Loader_StrConvertFromUTF8
  -----------------------------------------------------------------------------
*/
LOADER_ErrorCode
Loader_StrConvertFromUTF8(LOADER_Byte*              pszDestination,
                          LOADER_UInt4&             lDestinationLen,   // reference - contains the real length on return
                          Tools_DynamicUTF8String&  Source,
                          LOADER_StringEncoding     DestEncoding,      // must give a hint on swapped or not - e.g. ctUCS2_els00 or ctUCS2Native_els00 or ctUCS2Swapped_els00
                          LOADER_Char*              pszErrText)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    if (LOADER_STRINGENCODING_UTF8 != DestEncoding)   // destination encoding MUST NOT be already UTF8
    {
        if (LOADER_STRINGENCODING_ASCII == DestEncoding)
        {
            rc = Loader_StrConvertUTF8ToASCII(REINTERPRET_CAST(LOADER_Char*, pszDestination), lDestinationLen, Source);
        }
        else
        {
            rc = Loader_StrConvertUTF8ToUCS2(pszDestination, lDestinationLen, Source.StrPtr(), Source.BasisSize(), DestEncoding);
        }
    }
    // Both conversion functions return identical values for
    //  errOK_els00                      : conversion successful
    //  errEncodingSourceExhausted_els98 : partial character in source, but hit end
    //  errConvertingEncoding_els98      : source contains invalid chars or char sequences)
    //  errEncodingTargetExhausted_els98 : insuff. space in target for conversion)

    if (LOA_ERR_OK != rc)
    {
        if (LOA_ERR_CONVERTING_ENCODING != rc)
        {
/*            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternal_els98,
                                                                              _INTERNAL_FILE_ID_, __LINE__, rc);
*/
            rc = LOA_ERR_INTERNAL;
        }
        else
        {
/*            if (ctAscii_els00 == DestEncoding)
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UTF8", "ASCII");
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UTF8", "UCS2");
            }
*/
            //rc = errConvertingEncoding_els98;
        }
    }
    return rc;
}
// Loader_StrConvertFromUTF8()


/*
//
//
//  -----------------------------------------------------------------------------
//  function:     ls05ConvertFromUTF8 II
//  -----------------------------------------------------------------------------
//
tsp00_Longint
ls05ConvertFromUTF8(unsigned char           *pszDestination,
                    tsp00_Longuint          &lDestinationLen,
                    unsigned char           *pszSource,
                    tsp00_Longuint           lSourceLen,
                    tls00_CodeType           DestEncoding,
                    tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls05ConvertFromUTF8 II");

    tsp00_Longint rc = errOK_els00;

    if (ctUTF8_els00 != DestEncoding)   // destination encoding MUST NOT be already UTF8
    {
        if (ctAscii_els00 == DestEncoding)
        {
            rc = ls05_ConvertUTF8ToASCII(REINTERPRET_CAST(char*, pszDestination), lDestinationLen,
                                         pszSource,                               lSourceLen);
        }
        else
        {
            rc = ls05_ConvertUTF8ToUCS2(pszDestination, lDestinationLen, pszSource, lSourceLen, DestEncoding);
        }
    }
    // Both conversion functions return identical values for
    //  errOK_els00                      : conversion successful
    //  errEncodingSourceExhausted_els98 : partial character in source, but hit end
    //  errConvertingEncoding_els98      : source contains invalid chars or char sequences)
    //  errEncodingTargetExhausted_els98 : insuff. space in target for conversion)
    if (errOK_els00 != rc)
    {
        if (errConvertingEncoding_els98 != rc)
        {
            LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText, errInternal_els98,
                                                                              _INTERNAL_FILE_ID_, __LINE__, rc);
            rc = errInternal_els98;
        }
        else
        {
            if (ctAscii_els00 == DestEncoding)
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UTF8", "ASCII");
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCursor_els98, pszErrText,
                                                                      errConvertingEncoding_els98, "UTF8", "UCS2");
            }
            rc = errConvertingEncoding_els98;
        }
    }
    return rc;
}
// ls05ConvertFromUTF8()


//
//  -----------------------------------------------------------------------------
//  function:     ls05AssignUTF8
//  -----------------------------------------------------------------------------
//
tsp00_Longint
ls05AssignUTF8(Tools_DynamicUTF8String &Destination,
               const unsigned char     *pszSource,
               tsp00_Longuint           lSourceLen,
               tsp00_Addr               pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls05AssignUTF8");

    tsp00_Longint rc = errOK_els00;

    unsigned int charLength       = 0;
    unsigned int byteLength       = 0;
    int          isTerminated     = 0;
    int          isCorrupted      = 0;
    int          isExhausted      = 0;
    int          isCorruptedUTF8  = 0;

    isCorruptedUTF8 = sp77encodingUTF8->stringInfo(pszSource,
                                                   lSourceLen,
                                                   1,
                                                   &charLength,
                                                   &byteLength,
                                                   &isTerminated,
                                                   &isCorrupted,
                                                   &isExhausted);
    if (0 == isCorruptedUTF8)
    {
        Destination.AssignRaw(pszSource, lSourceLen);
    }
    else
    {
        rc = ls05ConvertToUTF8(Destination, CONST_CAST(unsigned char*, pszSource), lSourceLen, ctAscii_els00, pszErrText);
    }

    return rc;
}
// ls05AssignUTF8()
//
//  -----------------------------------------------------------------------------
//  EndChapter: Public functions implementation
//  -----------------------------------------------------------------------------
//
*/
//////////////////////////////////////////////////////////////////////////////////////////////
//
//  -----------------------------------------------------------------------------
//  Chapter: Private functions implementation
//  -----------------------------------------------------------------------------
//
static LOADER_ErrorCode
Loader_StrConvertUTF8ToUCS2(LOADER_Byte*           pszUCS2,
                            LOADER_UInt4&          lUCS2Len,
                            const LOADER_Byte*     pszUTF8,
                            LOADER_UInt4           lUTF8Len,
                            LOADER_StringEncoding  UCS2Type)
{

    LOADER_ErrorCode                  rc           = LOA_ERR_OK;
    Tools_UTF8Basis::ConversionResult rcConversion = Tools_UTF8Basis::Success;

    // Incoming source string is already UTF8
    Tools_DynamicUTF8String s;
    s.AssignRaw(STATIC_CAST(const SAPDB_UTF8*, pszUTF8), STATIC_CAST(SAPDB_UInt, lUTF8Len));

    tsp81_UCS2Char *pszUCS2At = NULL;

    if (LOADER_STRINGENCODING_UCS2NATIVE == UCS2Type)
    {
        rcConversion = s.ConvertToUCS2_Unaligned_Swap(REINTERPRET_CAST(tsp81_UCS2Char*, pszUCS2),
                                                      REINTERPRET_CAST(tsp81_UCS2Char*, (pszUCS2 + lUCS2Len)),
                                                      pszUCS2At);
    }
    else
    {
        bool bDestSwap = (LOADER_STRINGENCODING_UCS2 == UCS2Type) ? false : true;
        rcConversion = s.ConvertToUCS2_Unaligned_Swap(REINTERPRET_CAST(tsp81_UCS2Char*, pszUCS2),
                                                      REINTERPRET_CAST(tsp81_UCS2Char*, (pszUCS2 + lUCS2Len)),
                                                      pszUCS2At,
                                                      bDestSwap);
    }
    if (Tools_UTF8Basis::Success == rcConversion)
    {
        lUCS2Len = REINTERPRET_CAST(unsigned char*, pszUCS2At) - pszUCS2;
    }
    else if (Tools_UTF8Basis::SourceExhausted == rcConversion)
    {
        rc = LOA_ERR_ENCODING_SRC_EXHAUSTED;
    }
    else if (Tools_UTF8Basis::SourceCorrupted == rcConversion)
    {
        rc = LOA_ERR_CONVERTING_ENCODING;
    }
    else
    {
        rc = LOA_ERR_ENCODING_TGT_EXHAUSTED;
    }

    return rc;
}
// Loader_StrConvertUTF8ToUCS2()


static LOADER_ErrorCode
Loader_StrConvertASCIIToUCS2(LOADER_Byte*           pszUCS2,
                             LOADER_UInt4&          lUCS2Len,
                             const LOADER_Byte*     pszASCII,
                             LOADER_UInt4           lASCIILen,
                             const tsp77encoding*  pUCS2Type)
{
//TODOTODO FEHLERCODE

    LOADER_ErrorCode rc = LOA_ERR_OK;

    tsp78ConversionResult rcConversion = sp78_Ok;

    LOADER_UInt4 lUCS2BytesWritten = 0;
    LOADER_UInt4 lASCIIBytesParsed = 0;

    rcConversion = sp78convertBuffer(pUCS2Type,         pszUCS2,  lUCS2Len,  &lUCS2BytesWritten,
                                     sp77encodingAscii, pszASCII, lASCIILen, &lASCIIBytesParsed);
//TODOTODO FEHLERCODES
    if (sp78_Ok == rcConversion)
    {
        lUCS2Len = lUCS2BytesWritten;
    }
    else if (sp78_SourceExhausted == rcConversion)
    {
        rc = LOA_ERR_ENCODING_SRC_EXHAUSTED;
    }
    else if (sp78_SourceCorrupted == rcConversion)
    {
        rc = LOA_ERR_CONVERTING_ENCODING;
    }
    else
    {
        rc = LOA_ERR_ENCODING_TGT_EXHAUSTED;
    }
    return rc;
}
// Loader_StrConvertASCIIToUCS2()

/*
static tsp00_Longint
ls05_ConvertASCIIAnyCPToUCS2(unsigned char       *pszUCS2,
                             tsp00_Longuint      &lUCS2Len,      // must have the available length!!!
                             const unsigned char *pszASCII,
                             tsp00_Longuint       lASCIILen,
                             int                  UCS2Swapped,
                             tsp81_CodePage      *pCodePage)    // muss immer angegeben sein, entweder NULL oder wert
{
    ROUTINE_DBG_MEO00 ("ls05_ConvertASCIIAnyCPToUCS2");

    tsp00_Longint rc = errOK_els00;

    tsp00_Uint4 lCharsConverted = 0;

    lUCS2Len /= 2;

    sp81AnyASCIItoUCS2(REINTERPRET_CAST(tsp81_UCS2Char *, pszUCS2),
                       STATIC_CAST(tsp00_Uint4, lUCS2Len),
                       UCS2Swapped,
                       &lCharsConverted,
                       pszASCII,
                       STATIC_CAST(tsp00_Uint4, lASCIILen),
                       pCodePage);

    lUCS2Len = lCharsConverted*2;

    return rc;
}
// ls05_ConvertASCIIAnyCPToUCS2()


//
//  -----------------------------------------------------------------------------
//  function:     ls05_ConvertUCS2ToUTF8
//  -----------------------------------------------------------------------------
//
static tsp00_Longint
ls05_ConvertUCS2ToUTF8(Tools_DynamicUTF8String  &Destination,
                       unsigned char            *pszSource,
                       tsp00_Longint             lSourceLen,
                       tls00_CodeType            UCS2Type)      // gives a hint on swap type
{
    ROUTINE_DBG_MEO00 ("ls05_ConvertUCS2ToUTF8");

    tsp00_Longint                     rc           = errOK_els00;
    Tools_UTF8Basis::ConversionResult rcConversion = Tools_UTF8Basis::Success;

    if (ctUCS2Native_els00 == UCS2Type)
    {
        rcConversion = Destination.ConvertFromUCS2_Unaligned_Swap(REINTERPRET_CAST(tsp81_UCS2Char *, pszSource),
                                                                  REINTERPRET_CAST(tsp81_UCS2Char *, (pszSource + lSourceLen) ) );
    }
    else
    {
        bool bSrcSwapped = (ctUCS2_els00 == UCS2Type) ? false : true;
        rcConversion = Destination.ConvertFromUCS2_Unaligned_Swap(REINTERPRET_CAST(tsp81_UCS2Char*, pszSource),
                                                                  REINTERPRET_CAST(tsp81_UCS2Char*, (pszSource + lSourceLen)),
                                                                  bSrcSwapped);
    }

    if (Tools_UTF8Basis::SourceExhausted == rcConversion)
    {
        rc = errEncodingSourceExhausted_els98;
    }
    else if (Tools_UTF8Basis::TargetExhausted == rcConversion)
    {
        rc = errEncodingTargetExhausted_els98;
    }
    else if (Tools_UTF8Basis::SourceCorrupted == rcConversion)
    {
        rc = errConvertingEncoding_els98;
    }

    return rc;
}
// ls05_ConvertUCS2ToUTF8()
*/

//
//  -----------------------------------------------------------------------------
//  function:     ls05_ConvertUCS2ToASCII
//  -----------------------------------------------------------------------------
//
LOADER_ErrorCode
Loader_StrConvertUCS2ToASCII(LOADER_Byte*           pszASCII,
                             LOADER_UInt4&          lASCIILen,
                             LOADER_Byte*           pszUCS2,
                             LOADER_UInt4           lUCS2Len,
                             LOADER_StringEncoding  SrcEncoding)
{
    LOADER_ErrorCode      rc           = LOA_ERR_OK;
    tsp78ConversionResult rcConversion = sp78_Ok;

    LOADER_UInt4 lASCIIBytesWritten = 0;
    LOADER_UInt4 lUCS2BytesParsed   = 0;

    rcConversion = sp78convertBuffer(sp77encodingAscii, pszASCII, lASCIILen, &lASCIIBytesWritten,
                                     pLOADER_Encoding[SrcEncoding], pszUCS2, lUCS2Len, &lUCS2BytesParsed);
    if (sp78_Ok == rcConversion)
    {
        lASCIILen = lASCIIBytesWritten;
    }
    else if (sp78_SourceCorrupted == rcConversion)
    {
        rc = LOA_ERR_CONVERTING_ENCODING;
    }
    else
    {
        rc = LOA_ERR_INTERNAL; //errEncodingTargetExhausted_els98 || errEncodingSourceExhausted_els98
    }
    return rc;
}
// Loader_StrConvertUCS2ToASCII()


//
//  -----------------------------------------------------------------------------
//  function:     Loader_StrConvertUTF8ToASCII
//  -----------------------------------------------------------------------------
//
static LOADER_ErrorCode
Loader_StrConvertUTF8ToASCII(LOADER_Char*                    pszASCII,
                             LOADER_UInt4&                   lASCIILen,
                             const Tools_DynamicUTF8String&  Source)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    Tools_UTF8Basis::ConversionResult rcConversion = Tools_UTF8Basis::Success;

    LOADER_Char* pszASCIIEnd = NULL;

    rcConversion = Source.ConvertToASCII_Latin1(pszASCII, (pszASCII + lASCIILen), pszASCIIEnd);
    if (Tools_UTF8Basis::Success == rcConversion)
    {
        lASCIILen = pszASCIIEnd - pszASCII;
    }
    else if (Tools_UTF8Basis::SourceExhausted == rcConversion)
    {
        rc = LOA_ERR_ENCODING_SRC_EXHAUSTED;
    }
    else if (Tools_UTF8Basis::SourceCorrupted == rcConversion)
    {
        rc = LOA_ERR_CONVERTING_ENCODING;
    }
    else
    {
        rc = LOA_ERR_ENCODING_TGT_EXHAUSTED;
    }

    return rc;
}
// Loader_StrConvertUTF8ToASCII()

/*
//
//  -----------------------------------------------------------------------------
//  function:     ls05_ConvertUTF8ToASCII II
//  -----------------------------------------------------------------------------
//
static tsp00_Longint
ls05_ConvertUTF8ToASCII(char            *pszASCII,
                        tsp00_Longuint  &lASCIILen,
                        unsigned char   *pszUTF8,
                        tsp00_Longuint   lUTF8Len)
{
    ROUTINE_DBG_MEO00 ("ls05_ConvertUTF8ToASCII II");

    tsp00_Longint rc = errOK_els00;

    Tools_UTF8Basis::ConversionResult rcConversion = Tools_UTF8Basis::Success;

    // Incoming source string is already UTF8
    Tools_DynamicUTF8String Source;
    Source.AssignRaw(STATIC_CAST(const SAPDB_UTF8*, pszUTF8), STATIC_CAST(SAPDB_UInt, lUTF8Len));

    char *pszASCIIEnd = NULL;

    rcConversion = Source.ConvertToASCII_Latin1(pszASCII, (pszASCII + lASCIILen), pszASCIIEnd);
    if (Tools_UTF8Basis::Success == rcConversion)
    {
        lASCIILen = pszASCIIEnd - pszASCII;
    }
    else if (Tools_UTF8Basis::SourceExhausted == rcConversion)
    {
        rc = errEncodingSourceExhausted_els98;
    }
    else if (Tools_UTF8Basis::SourceCorrupted == rcConversion)
    {
        rc = errConvertingEncoding_els98;
    }
    else
    {
        rc = errEncodingTargetExhausted_els98;
    }

    return rc;
}
// ls05_ConvertUTF8ToASCII()



  //-----------------------------------------------------------------------------
  //function:     ls05_ConvertASCIIToUTF8
  //-----------------------------------------------------------------------------

static tsp00_Longint
ls05_ConvertASCIIToUTF8(Tools_DynamicUTF8String &Destination,
                        unsigned char           *pszASCII,
                        tsp00_Longuint           lASCIILen)
{
    ROUTINE_DBG_MEO00 ("ls05_ConvertASCIIToUTF8");

    tsp00_Longint rc = errOK_els00;

    Tools_UTF8Basis::ConversionResult rcConversion = Tools_UTF8Basis::Success;

    rcConversion = Destination.ConvertFromASCII_Latin1(REINTERPRET_CAST(char*, pszASCII),
                                                       REINTERPRET_CAST(char*, (pszASCII + lASCIILen)) );

    if (Tools_UTF8Basis::SourceExhausted == rcConversion)
    {
        rc = errEncodingSourceExhausted_els98;
    }
    else if (Tools_UTF8Basis::SourceCorrupted == rcConversion)
    {
        rc = errConvertingEncoding_els98;
    }
    else if (Tools_UTF8Basis::TargetExhausted == rcConversion)
    {
        rc = errEncodingTargetExhausted_els98;
    }

    return rc;
}
// ls05_ConvertASCIIToUTF8()


//
//  -----------------------------------------------------------------------------
//  function:     ls05_StrCmpPaddedAscii
//  -----------------------------------------------------------------------------
//
static LOADER_Bool
ls05_StrCmpPaddedAscii(const LOADER_Byte*  pszShorterVal,
                       LOADER_UInt4 lShorterLen,     // length in Bytes
                       const LOADER_Byte* pszLongerVal,
                       LOADER_UInt4 lLongerLen)      // length in Bytes
{
    bool bEqual = LOADER_TRUE;

    if ( 0 != memcmp(pszLongerVal, pszShorterVal, lShorterLen) )
    {
        bEqual = LOADER_FALSE;
    }
    else
    {
        if (lLongerLen != lShorterLen)
        {
            LOADER_Int4 i = lLongerLen - 1;
            for (i; ( (i >= STATIC_CAST(LOADER_Int4, lShorterLen)) && (LOADER_TRUE == bEqual) ); --i)
            {
                bEqual = (BLANK_LS00 == pszLongerVal[i]);
            }
        }
    }
    return bEqual;
}
// ls05_StrCmpPaddedAscii()


//
//  -----------------------------------------------------------------------------
//  function:     ls05_StrCmpPaddedUCS2
//  -----------------------------------------------------------------------------
//
static LOADER_Bool
ls05_StrCmpPaddedUCS2(const LOADER_Byte*  pszShorterVal,
                      LOADER_UInt4        lShorterLen,     // length in characters not bytes
                      const LOADER_Byte*  pszLongerVal,
                      LOADER_UInt4        lLongerLen,      // length in characters not bytes
                      tls00_CodeType     CodeType)
{
    LOADER_Bool bEqual = LOADER_TRUE;

    const tsp81_UCS2Char* _sval = REINTERPRET_CAST(const tsp81_UCS2Char*, pszShorterVal);
    const tsp81_UCS2Char* _lval = REINTERPRET_CAST(const tsp81_UCS2Char*, pszLongerVal);

    if ( 0 != sp81UCS2strncmp(_lval, _sval, lShorterLen) )
    {
        bEqual = LOADER_FALSE;
    }
    else
    {
        if (lLongerLen != lShorterLen)
        {
            LOADER_Int4 i = lLongerLen - 1;
            for (i; ( (i >= STATIC_CAST(LOADER_Int4, lShorterLen)) && (LOADER_TRUE == bEqual) ); --i)
            {
                switch (CodeType)
                {
                    case ctUCS2_els00:
                    {
                        bEqual = (0 != sp77encodingUCS2->isBlank(STATIC_CAST(const void*, &_lval[i])));
                        break;
                    }
                    case ctUCS2Swapped_els00:
                    {
                        bEqual = (0 != sp77encodingUCS2Swapped->isBlank(STATIC_CAST(const void*, &_lval[i])));
                        break;
                    }
                    case ctUCS2Native_els00:
                    {
                        bEqual = (0 != sp77encodingUCS2Native->isBlank(STATIC_CAST(const void*, &_lval[i])));
                        break;
                    }
                }
            }   // end for (tsp00_Uint4 i = lLongerLen; ( (i >= lShorterLen) && (true == bEqual) ); --i)
        }
    }   // end else of if ( 0 != sp81UCS2strncmp(pszLongerVal, pszShorterVal, lShorterLen) )

    return bEqual;
}
// ls05_StrCmpPaddedUCS2()
*/


/** Convert a two-char hex string into the char it represents. **/
LOADER_ErrorCode
Loader_X2C(LOADER_Char*  pszHexString,
           LOADER_Int4   lStringLength,
           LOADER_Byte*  pszBinaryBuffer,
           LOADER_Int4&  lBufferLength)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;
    
    register unsigned char c0 = 0;
    register unsigned char c1 = 0;

    if (0 != lStringLength%2)
    {
        rc = LOA_ERR_INVALID_HEX;
    }
    else
    {
        LOADER_Int4 i = 0;
        LOADER_Int4 j = 0;

        for (i, j; i < lStringLength; i += 2, ++j)
        {
            c0 = Loader_szHexToByteMap[pszHexString[i]];
            c1 = Loader_szHexToByteMap[pszHexString[i+1]];

            if ( (0xFF == c0) || (0xFF == c0))
            {
                rc = LOA_ERR_INVALID_HEX;
                break;
            }

            pszBinaryBuffer[j] = (c0<<4) + c1;
        }
    }
    return rc;
}
// Loader_X2C()