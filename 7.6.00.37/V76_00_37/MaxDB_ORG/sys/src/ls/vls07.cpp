/*
  -----------------------------------------------------------------------------
  module: vls07.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Numerical and code conversion functions
                
  version:      7.6.
  -----------------------------------------------------------------------------
    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

*/

#include <assert.h>

#include "gls00.h"
#include "hls07.h"
#include "hls98msg.h"

const unsigned char pHexToByteMap[256] = {
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
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

/*  
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  function:     ls07_x2c
  -----------------------------------------------------------------------------

  description:  Converts single hex value (2 Byte) to corresponding binary
                value (1 Byte).

  arguments:    pHexValue    [IN]    - pointer, hexadecimal value, 2 Byte will
                                        be taken into account
                ByteValue    [OUT]   - single char byte that holds the binary
                                        value after conversion

  returnvalue:  errOK_els00             - NO errors
                errInvalidHex_els98     - invalid hex value (contains other characters
                                          than 0-9 and a-f)
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls07_x2c(char *pHexValue, unsigned char &ByteValue);


/*  
  -----------------------------------------------------------------------------
  EndChapter: Private functions declaration
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  Chapter: Global function implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls07ConvertHexToByte
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07ConvertHexToByte(tsp00_Addr  pszHexValue,
                     tsp00_Int4  lHexValueLen,
                     tsp00_Addr  *pszByteValue,
                     tsp00_Int4& lByteValueLen)
{
    ROUTINE_DBG_MEO00 ("ls07ConvertHexToByte");

    //assert(NULL != pszHexValue);

    tsp00_Int4 RetValue = errOK_els00;

    if (0 != lHexValueLen)
    {
        bool        bInvalidValue = false;

        tsp00_Addr  pszMyValue = new char[lByteValueLen = 2*lHexValueLen];      // No terminating 0!!!

        s41pbyte(pszMyValue, 1, lByteValueLen, pszHexValue, 1, lHexValueLen, bInvalidValue);
        if (true == bInvalidValue)
        {
            delete [] pszMyValue;
            RetValue = errInvalidHex_els98;
        }
        else
        {
            *pszByteValue = pszMyValue;
        }
    }

    return RetValue;
}
//  ls07ConvertHexToByte()


/*
  -----------------------------------------------------------------------------
  function:     ls07ConvertHexToByteEx
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07ConvertHexToByteEx(char           *pszHexValue,
                       tsp00_Longint   lHexValueLen,
                       unsigned char  *pszByteValue,
                       tsp00_Longint  &lByteValueLen)
{
    ROUTINE_DBG_MEO00 ("ls07ConvertHexToByteEx");

    // we're expecting pszByteValue allocated

    tsp00_Int4    rc   = errOK_els00;
    tsp00_Longint half = lHexValueLen/2;

    if (lByteValueLen > half)
    {
        lByteValueLen = half;
    }

    if (0 != lHexValueLen)
    {
        tsp00_Longint len = 0;
        for (len; ( (len < lByteValueLen) && (errOK_els00 == rc) ); ++len)
        {
            rc = ls07_x2c(&pszHexValue[2*len], pszByteValue[len]);
        }
    }

    return rc;
}
// ls07ConvertHexToByteEx()


/*
  -----------------------------------------------------------------------------
  function:     ls07ConvertByteToHex
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07ConvertByteToHex(tsp00_Addr   pszByteValue,
                     tsp00_Int4   lByteLen,
                     tsp00_Addr   *pszHexValue,
                     tsp00_Int4 & lHexLen)
{
    ROUTINE_DBG_MEO00 ("ls07ConvertByteToHex");

    tsp00_Int4 rc         = errOK_els00;
    bool       bTruncated = false;

    tsp00_Int4 lMyLen = 2*lByteLen;
    tsp00_Addr pszMyValue = new char[2*lByteLen + 1];

    
    s40gbyte(pszByteValue, 1, lByteLen, pszMyValue, 1, lMyLen, bTruncated);

    pszMyValue[lMyLen] = '\0';

    *pszHexValue = pszMyValue;
    lHexLen      = lMyLen;

    return rc;
}
// ls07ConvertByteToHex()


/*
  -----------------------------------------------------------------------------
  function:     ls07ConvertByteToHexEx
  -----------------------------------------------------------------------------
*/
void
ls07ConvertByteToHexEx(tsp00_Addr pszByteValue,
                       tsp00_Int4 lByteLen,
                       tsp00_Addr pszHexValue)
{
    ROUTINE_DBG_MEO00 ("ls07ConvertByteToHexEx");

    bool       bTruncated = false;
    tsp00_Int4 lMyLen     = lByteLen + lByteLen;
    
    s40gbyte(pszByteValue, 1, lByteLen, pszHexValue, 1, lMyLen, bTruncated);

    return;
}
// ls07ConvertByteToHexEx()


/*
  -----------------------------------------------------------------------------
  function:     ls07Swap2BIntToChar
  -----------------------------------------------------------------------------
*/
void
ls07Swap2BIntToChar(tsp00_Int2          nValue,
                    tsp00_SwapKind_Enum SSwap,
                    tsp00_SwapKind_Enum DSwap,
                    tsp00_Addr          pszBuffer)
{
    ROUTINE_DBG_MEO00 ("ls07Swap2BIntToChar");

    assert(2 == sizeof(nValue));

    if (SSwap == DSwap)
    {
        SAPDB_memcpy( pszBuffer, &nValue, sizeof(nValue) );
    }
    else
    {
        pszBuffer[0] = (REINTERPRET_CAST(char*,&nValue))[1];
        pszBuffer[1] = (REINTERPRET_CAST(char*,&nValue))[0];

    }
}
//  ls07Swap2BIntToChar()


/*
  -----------------------------------------------------------------------------
  function:     ls07Swap4BIntToChar
  -----------------------------------------------------------------------------
*/
void
ls07Swap4BIntToChar(tsp00_Uint4         nValue,
                    tsp00_SwapKind_Enum SSwap,
                    tsp00_SwapKind_Enum DSwap,
                    tsp00_Addr          pszBuffer)
{
    ROUTINE_DBG_MEO00 ("ls07Swap4BIntToChar");

    if (SSwap == DSwap)
    {
        SAPDB_memcpy( pszBuffer, &nValue, sizeof(nValue) );
    }
    else if ( ((sw_normal == SSwap) && (sw_full_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_full_swapped == SSwap)) )
    {
        pszBuffer[0] = (REINTERPRET_CAST(char*,&nValue))[3];
        pszBuffer[1] = (REINTERPRET_CAST(char*,&nValue))[2];
        pszBuffer[2] = (REINTERPRET_CAST(char*,&nValue))[1];
        pszBuffer[3] = (REINTERPRET_CAST(char*,&nValue))[0];
    }
    else if ( ((sw_normal == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_part_swapped == SSwap)) )
    {
        pszBuffer[0] = (REINTERPRET_CAST(char*,&nValue))[1];
        pszBuffer[1] = (REINTERPRET_CAST(char*,&nValue))[0];
        pszBuffer[2] = (REINTERPRET_CAST(char*,&nValue))[3];
        pszBuffer[3] = (REINTERPRET_CAST(char*,&nValue))[2];
    }
    else if ( ((sw_full_swapped == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_full_swapped == DSwap) && (sw_part_swapped == SSwap)) )
    {
        pszBuffer[0] = (REINTERPRET_CAST(char*,&nValue))[2];
        pszBuffer[1] = (REINTERPRET_CAST(char*,&nValue))[3];
        pszBuffer[2] = (REINTERPRET_CAST(char*,&nValue))[0];
        pszBuffer[3] = (REINTERPRET_CAST(char*,&nValue))[1];
    }
}
//  ls07Swap4BIntToChar()

/*
  -----------------------------------------------------------------------------
  function:     ls07Swap4BHexToLong
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls07Swap4BHexToLong(tsp00_Addr           pszValue,
                               tsp00_SwapKind_Enum  SSwap,
                               tsp00_SwapKind_Enum  DSwap)
{
    ROUTINE_DBG_MEO00 ("ls07Swap4BHexToLong");

    union IntegerUnion
    {
        tsp00_Int4 lIntValue;
        char       szIntValue[4];
    } u;

    
    if (SSwap == DSwap)
    {
        SAPDB_memcpy( u.szIntValue, pszValue, sizeof(u.lIntValue) );
    }
    else if ( ((sw_normal == SSwap) && (sw_full_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_full_swapped == SSwap)) )
    {
        u.szIntValue[0] = pszValue[3];
        u.szIntValue[1] = pszValue[2];
        u.szIntValue[2] = pszValue[1];
        u.szIntValue[3] = pszValue[0];
    }
    else if ( ((sw_normal == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_part_swapped == SSwap)) )
    {
        u.szIntValue[0] = pszValue[1];
        u.szIntValue[1] = pszValue[0];
        u.szIntValue[2] = pszValue[3];
        u.szIntValue[3] = pszValue[2];
    }
    else if ( ((sw_full_swapped == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_full_swapped == DSwap) && (sw_part_swapped == SSwap)) )
    {
        u.szIntValue[0] = pszValue[2];
        u.szIntValue[1] = pszValue[3];
        u.szIntValue[2] = pszValue[0];
        u.szIntValue[3] = pszValue[1];
    }

    return u.lIntValue;
}
//  ls07Swap4BHexToLong()


/*
  -----------------------------------------------------------------------------
  function:     ls07Swap2BHexToShort
  -----------------------------------------------------------------------------
*/
short ls07Swap2BHexToShort(tsp00_Addr           pszValue,
                           tsp00_SwapKind_Enum  SSwap,
                           tsp00_SwapKind_Enum  DSwap)
{
    ROUTINE_DBG_MEO00 ("ls07Swap2BHexToShort");

    union IntegerUnion
    {
        short lIntValue;
        char  szIntValue[2];
    } u;

    if (SSwap == DSwap)
    {
        SAPDB_memcpy( u.szIntValue, pszValue, sizeof(u.lIntValue) );
    }
    else
    {
        u.szIntValue[0] = pszValue[1];
        u.szIntValue[1] = pszValue[0];
    }

    return u.lIntValue;
}
//  ls07Swap2BHexToShort()



/*
  -----------------------------------------------------------------------------
  function:     ls07Swap2BInt
  -----------------------------------------------------------------------------
*/
tsp00_Int2
ls07Swap2BInt(tsp00_Int2 lSource,
              tsp00_SwapKind_Enum  SSwap,
              tsp00_SwapKind_Enum  DSwap)
{
    ROUTINE_DBG_MEO00 ("ls07Swap2BInt");

    union IntegerUnion
    {
        tsp00_Int2 lIntValue;
        char       szIntValue[2];
    } u;

    

    if (SSwap == DSwap)
    {
        u.lIntValue = lSource;
        //SAPDB_memcpy( u.szIntValue, pszValue, sizeof(u.lIntValue) );
    }
    else
    {
        u.szIntValue[0] = REINTERPRET_CAST(char*, &lSource)[1];
        u.szIntValue[1] = REINTERPRET_CAST(char*, &lSource)[0];
    }

    return u.lIntValue;
}
//  ls07Swap2BInt()


/*
  -----------------------------------------------------------------------------
  function:     ls07Swap4BInt
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07Swap4BInt(tsp00_Int4 lSource,
              tsp00_SwapKind_Enum  SSwap,
              tsp00_SwapKind_Enum  DSwap)
{
    ROUTINE_DBG_MEO00 ("ls07Swap4BInt");

    tsp00_Int4 lTarget = 0;
    
    if (SSwap == DSwap)
    {
        lTarget = lSource;
    }
    else if ( ((sw_normal == SSwap) && (sw_full_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_full_swapped == SSwap)) )
    {
        REINTERPRET_CAST(char*, &lTarget)[0] = REINTERPRET_CAST(char*, &lSource)[3];
        REINTERPRET_CAST(char*, &lTarget)[1] = REINTERPRET_CAST(char*, &lSource)[2];
        REINTERPRET_CAST(char*, &lTarget)[2] = REINTERPRET_CAST(char*, &lSource)[1];
        REINTERPRET_CAST(char*, &lTarget)[3] = REINTERPRET_CAST(char*, &lSource)[0];
    }
    else if ( ((sw_normal == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_part_swapped == SSwap)) )
    {
        REINTERPRET_CAST(char*, &lTarget)[0] = REINTERPRET_CAST(char*, &lSource)[1];
        REINTERPRET_CAST(char*, &lTarget)[1] = REINTERPRET_CAST(char*, &lSource)[0];
        REINTERPRET_CAST(char*, &lTarget)[2] = REINTERPRET_CAST(char*, &lSource)[3];
        REINTERPRET_CAST(char*, &lTarget)[3] = REINTERPRET_CAST(char*, &lSource)[2];
    }
    else if ( ((sw_full_swapped == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_full_swapped == DSwap) && (sw_part_swapped == SSwap)) )
    {
        REINTERPRET_CAST(char*, &lTarget)[0] = REINTERPRET_CAST(char*, &lSource)[2];
        REINTERPRET_CAST(char*, &lTarget)[1] = REINTERPRET_CAST(char*, &lSource)[3];
        REINTERPRET_CAST(char*, &lTarget)[2] = REINTERPRET_CAST(char*, &lSource)[0];
        REINTERPRET_CAST(char*, &lTarget)[3] = REINTERPRET_CAST(char*, &lSource)[1];
    }

    return lTarget;
}
//  ls07Swap4BInt()


/*
  -----------------------------------------------------------------------------
  function:     ls07Swap4BCharInPlace
  -----------------------------------------------------------------------------
*/
void
ls07Swap4BCharInPlace(tsp00_Addr          pszSource,
                      tsp00_SwapKind_Enum SSwap,
                      tsp00_SwapKind_Enum DSwap)
{
    ROUTINE_DBG_MEO00 ("ls07Swap4BCharInPlace");

    char c0 = 0;
    char c1 = 0;

    if (SSwap == DSwap)
    {
        return;
    }
    else if ( ((sw_normal == SSwap) && (sw_full_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_full_swapped == SSwap)) )
    {
        c0 = pszSource[0];
        c1 = pszSource[1];

        pszSource[0] = pszSource[3];
        pszSource[1] = pszSource[2];
        pszSource[2] = c1;
        pszSource[3] = c0;
    }
    else if ( ((sw_normal == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_normal == DSwap) && (sw_part_swapped == SSwap)) )
    {
        c0 = pszSource[0];
        c1 = pszSource[2];

        pszSource[0] = pszSource[1];
        pszSource[1] = c0;
        pszSource[2] = pszSource[3];
        pszSource[3] = c1;
    }
    else if ( ((sw_full_swapped == SSwap) && (sw_part_swapped == DSwap)) ||
              ((sw_full_swapped == DSwap) && (sw_part_swapped == SSwap)) )
    {
        c0 = pszSource[0];
        c1 = pszSource[1];

        pszSource[0] = pszSource[2];
        pszSource[1] = pszSource[3];
        pszSource[2] = c0;
        pszSource[3] = c1;
    }

    return;
}
// ls07Swap4BCharInPlace


/*
  -----------------------------------------------------------------------------
  function:     ls07FourByteToSReal
  -----------------------------------------------------------------------------
*/
tsp00_Shortreal ls07FourByteToSReal(tsp00_Addr  pszValue)
{
    ROUTINE_DBG_MEO00 ("ls07FourByteToSReal");

    union ShortRealUnion
    {
        tsp00_Shortreal fShort;
        char            szShortChar[4];
    } u;

    SAPDB_memcpy( u.szShortChar, pszValue, sizeof(tsp00_Shortreal) );

    return  u.fShort;
}
//  ls07FourByteToSReal()


/*
  -----------------------------------------------------------------------------
  function:     ls07EightByteToLReal
  -----------------------------------------------------------------------------
*/
tsp00_Longreal ls07EightByteToLReal(tsp00_Addr  pszValue)
{
    ROUTINE_DBG_MEO00 ("ls07EightByteToLReal");

    union LongRealUnion
    {
        tsp00_Longreal fLong;
        char           szLongChar[8];
    } u;

    SAPDB_memcpy( u.szLongChar, pszValue, sizeof (u.fLong) );

    return  u.fLong;
}
//  ls07EightByteToLReal()



/*
  -----------------------------------------------------------------------------
  function:     ls07NumberToInt
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls07NumberToInt(tsp00_Addr pszNumberString, tsp00_Int4& Dest)
{
    ROUTINE_DBG_MEO00 ("ls07NumberToInt");
    
    tsp00_NumError Result;
    tsp00_Int4     rc     = errOK_els00;

    Result.becomes(num_ok);

    s40g4int(pszNumberString, 1, Dest, Result);
    switch (Result)
    {
        case num_ok:
        {
            break;
        }
        case num_trunc:
        {   // Eigentlich nur eine warnung. Individuell von jedem aufrufer zu behandeln
            rc = errNumberTruncated_els98;
            break;
        }
        case num_overflow:
        {
            rc = errNumberOverflow_els98;
            break;
        }
        default:        // entspricht num_invalid
        {
            rc = errInvalidNumber_els98;
        }
    }

    return rc;
}
//  ls07NumberToInt()



/*
  -----------------------------------------------------------------------------
  function:     ls07IntToNumber
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07IntToNumber(tsp00_Int4 Source, tsp00_Addr pszNumberString)
{
    ROUTINE_DBG_MEO00 ("ls07IntToNumber");
    
    tsp00_NumError Result;
    tsp00_Int4     rc     = errOK_els00;

    Result.becomes(num_ok);

    s41p4int(pszNumberString, 1, Source, Result);
    switch (Result)
    {
        case num_ok:
        {
            break;
        }
        case num_trunc:
        {   // Eigentlich nur eine warnung. Individuell von jedem aufrufer zu behandeln
            rc = errNumberTruncated_els98;
            break;
        }
        case num_overflow:
        {
            rc = errNumberOverflow_els98;
            break;
        }
        default:        // entspricht num_invalid
        {
            rc = errInvalidNumber_els98;
        }
    }

    return rc;
}
// ls07IntToNumber()


/*
  -----------------------------------------------------------------------------
  function:     ls07CheckIntValueRange
  -----------------------------------------------------------------------------
*/
int
ls07CheckIntValueRange(_TCHAR *pszValue)
{
    int rc = 0;     // initialize to error
    
    size_t lLen = _tcslen(pszValue);

    if (lLen <= 10)
    {
        size_t i = 0;
        for (i; ( (i < lLen) && (0 != isdigit(pszValue[i]) ) ); ++i);
        if (lLen == i)      // no not digit found
        {
            rc = 1;
            if (10 == lLen)
            {
                if (0 > memcmp("2147483647", pszValue, lLen) )
                {
                    rc = 0;
                }
            }
        }
    }

    return rc;
}
// ls07CheckValueRange()


/*
  -----------------------------------------------------------------------------
  function:     ls07IntCompare
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07IntCompare(const void *arg1, const void *arg2)
{
    if (*((tsp00_Int4*)arg1) < *((tsp00_Int4*)arg2))
    {
        return -1;
    }
    else if (*((tsp00_Int4*)arg1) > *((tsp00_Int4*)arg2))
    {
        return 1;
    }

    return 0;
}
// ls07IntCompare()

/*
  -----------------------------------------------------------------------------
  EndChapter: Global function implementation
  -----------------------------------------------------------------------------
*/


/*
  -----------------------------------------------------------------------------
  Chapter: Private function implementation
  -----------------------------------------------------------------------------
*/

static tsp00_Int4
ls07_x2c(char *pHexValue, unsigned char &ByteValue)
{
    tsp00_Int4 rc = errOK_els00;

    unsigned char c0 = pHexToByteMap[pHexValue[0]];
    unsigned char c1 = pHexToByteMap[pHexValue[1]];

    if ( (0xFF != c0) && (0xFF != c1) )
    {
        ByteValue = (c0<<4) + c1;
    }
    else
    {
        rc = errInvalidHex_els98;
    }
    return rc;

/* Alternative:
   register unsigned char digit;

   // - ANDing with 0xdf makes lower case letters upper case
   // - not handled are wrong hex values
   digit = (pHexValue[0] >= 'A' ? ((pHexValue[0] & 0xdf) - 'A')+10 : (pHexValue[0] - '0'));
   digit *= 16;
   digit += (pHexValue[1] >= 'A' ? ((pHexValue[1] & 0xdf) - 'A')+10 : (pHexValue[1] - '0'));

   return(digit);
*/
}
// ls07_x2c()


/*
  -----------------------------------------------------------------------------
  EndChapter: Private function implementation
  -----------------------------------------------------------------------------
*/
