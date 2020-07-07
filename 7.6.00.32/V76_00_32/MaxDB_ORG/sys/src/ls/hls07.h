/*! 
  -----------------------------------------------------------------------------
 
  module: hls07.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Numerical and code conversion functions
                
  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1999-2005 SAP AG-2002
 
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

#ifndef HLS07
#define HLS07

#include "gls00.h"


/* Possible db data types

 dfixed,      dfloat,     dcha,         dche,
 dchb,        drowid,     dstra,        dstre,
 dstrb,       dstrdb,     ddate,        dtime,
 dvfloat,     dtimestamp, dunknown,     dnumber,
 dnonumber,   dduration,  ddbyteebcdic, dlonga,
 dlonge,      dlongb,     dlongdb,      dboolean,
 dunicode,    ddtfiller1, ddtfiller2,   ddtfiller3,
 ddtfiller4,  dsmallint,  dinteger,     dvarchara,
 dvarchare,   dvarcharb,  dstruni,      dlonguni,
 dvarcharuni, dudt,       dabaptabhandle
*/
const char szDefByteArray_ls07[39] =
            {DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_ASCII_LS00,   DEF_BYTE_EBCDIC_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_ASCII_LS00,   DEF_BYTE_EBCDIC_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_ASCII_LS00,   DEF_BYTE_ASCII_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_ASCII_LS00, DEF_BYTE_LS00,         DEF_BYTE_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_ASCII_LS00,
             DEF_BYTE_EBCDIC_LS00,  DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_LS00,
             DEF_BYTE_UNICODE_LS00, DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_ASCII_LS00,
             DEF_BYTE_EBCDIC_LS00,  DEF_BYTE_LS00,       DEF_BYTE_UNICODE_LS00, DEF_BYTE_UNICODE_LS00,
             DEF_BYTE_UNICODE_LS00, DEF_BYTE_LS00,       DEF_BYTE_LS00};

const char szFillByteArray_ls07[39] =
            {DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_ASCII_LS00,   DEF_BYTE_EBCDIC_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_ASCII_LS00,   DEF_BYTE_EBCDIC_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_ASCII_LS00,   DEF_BYTE_ASCII_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_ASCII_LS00, DEF_BYTE_LS00,         DEF_BYTE_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_ASCII_LS00,
             DEF_BYTE_EBCDIC_LS00,  DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_LS00,
             DEF_BYTE_ASCII_LS00,   DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_LS00,
             DEF_BYTE_LS00,         DEF_BYTE_LS00,       DEF_BYTE_LS00,         DEF_BYTE_ASCII_LS00,
             DEF_BYTE_EBCDIC_LS00,  DEF_BYTE_LS00,       DEF_BYTE_ASCII_LS00,   DEF_BYTE_ASCII_LS00,
             DEF_BYTE_ASCII_LS00,   DEF_BYTE_LS00,       DEF_BYTE_LS00};


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07ConvertHexToByte

  -----------------------------------------------------------------------------

  description:  Converts a user given hexadecimal value into an byte string.

                The function allocates memory for the value to return which must
                be freed by the caller.

  arguments:    pszHexValue     [IN]    - hexadecimal value
                lHexValueLen    [IN]    - length of hex value
                pszByteValue    [OUT]   - converted value NOT null terminated
                lByteValueLen   [OUT]   - Length of converted value

  returnvalue:  errOK_els00             - NO errors
                errInvalidHex_els98     - invalid hex value (contains other
                                           characters than 0-9 and a-f)
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07ConvertHexToByte(tsp00_Addr  pszHexValue,
                     tsp00_Int4  lHexValueLen,
                     tsp00_Addr *pszByteValue,
                     tsp00_Int4 &lByteValueLen);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07ConvertHexToByteEx

  -----------------------------------------------------------------------------

  description:  Converts a user given hexadecimal value into an byte string.


  arguments:    pszHexValue     [IN]      - readable hexadecimal value
                lHexValueLen    [IN]      - length of hex value
                pszByteValue    [OUT]     - buffer for converted value (NOT null 
                                             terminated);
                lByteValueLen   [IN/OUT]  - Length of buffer and of converted value
                                             on return

  returnvalue:  errOK_els00             - NO errors
                errInvalidHex_els98     - invalid hex value (contains other
                                           characters than 0-9 and a-f)
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07ConvertHexToByteEx(char           *pszHexValue,
                       tsp00_Longint   lHexValueLen,
                       unsigned char  *pszByteValue,
                       tsp00_Longint  &lByteValueLen);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07ConvertByteToHex

  -----------------------------------------------------------------------------

  description:  Converts a user given byte stream to a hexadecimal stream

                The function allocates memory for the value to return which must
                be freed by the caller.

  arguments:    pszByteValue    [IN]   - value NOT null terminated
                lByteValueLen   [IN]   - Length of value
                pszHexValue     [OUT]  - hexadecimal (converted) value
                lHexValueLen    [OUT]  - length of hex value
                

  returnvalue:  errOK_els00            - NO errors
                errInvalidHex_els98    - invalid hex value (contains other
                                          characters than 0-9 and a-f)
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07ConvertByteToHex(tsp00_Addr  pszByteValue,
                     tsp00_Int4  lByteLen,
                     tsp00_Addr *pszHexValue,
                     tsp00_Int4 &lHexLen);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls07ConvertByteToHexEx

  -----------------------------------------------------------------------------

  description:  Converts a user given byte stream to a hexadecimal stream

  arguments:    pszByteValue  [IN]   - pointer, value NOT null terminated
                lByteValueLen [IN]   - Length of value
                pszHexValue   [OUT]  - hexadecimal (converted) value.
                                       The value is NOT NULL TERMINATED!!!
                                       Space is allocated by the CALLER.

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void
ls07ConvertByteToHexEx(tsp00_Addr pszByteValue,
                       tsp00_Int4 lByteLen,
                       tsp00_Addr pszHexValue);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap2BIntToChar

  -----------------------------------------------------------------------------

  description:  Swaps digits of 2 byte integer and saves the result in 2 byte char

  arguments:    nValue     [IN]    - value to swap
                SSwap      [IN]    - source swap behavior (little endian, big endian)
                DSwap      [IN]    - destination swap behavior
                pszBuffer  [OUT]   - buffer for swapped character value

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void
ls07Swap2BIntToChar(tsp00_Int2          nValue,
                    tsp00_SwapKind_Enum SSwap,
                    tsp00_SwapKind_Enum DSwap,
                    tsp00_Addr          pszBuffer);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap4BIntToChar

  -----------------------------------------------------------------------------

  description:  Swaps digits of 4 byte integer and saves the result in 4 byte char

  arguments:    nValue     [IN]    - value to swap
                SSwap      [IN]    - source swap behavior (little endian, big endian)
                DSwap      [IN]    - destination swap behavior
                pszBuffer  [OUT]   - buffer for swapped character value

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void
ls07Swap4BIntToChar(tsp00_Uint4         nValue,
                    tsp00_SwapKind_Enum SSwap,
                    tsp00_SwapKind_Enum DSwap,
                    tsp00_Addr          pszBuffer);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap4BHexToLong

  -----------------------------------------------------------------------------

  description:  Swaps digits of 4 byte integer and saves the result as long

  arguments:    pszValue   [IN]  - value to swap
                SSwap      [IN]  - source swap behavior (little endian, big endian)
                DSwap      [IN]  - destination swap behavior

  returnvalue:  swapped value as long
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07Swap4BHexToLong(tsp00_Addr          pszValue,
                    tsp00_SwapKind_Enum SSwap,
                    tsp00_SwapKind_Enum DSwap);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap2BHexToShort

  -----------------------------------------------------------------------------

  description:  Swaps digits of 2 byte integer and saves the result as short

  arguments:    pszValue   [IN]    - value to swap
                SSwap      [IN]    - source swap behavior (little endian, big endian)
                DSwap      [IN]    - destination swap behavior

  returnvalue:  swapped value as short
  -----------------------------------------------------------------------------
*/
short
ls07Swap2BHexToShort(tsp00_Addr          pszValue,
                     tsp00_SwapKind_Enum SSwap,
                     tsp00_SwapKind_Enum DSwap);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap2BInt

  -----------------------------------------------------------------------------

  description:  Swaps 2 byte integer.

  arguments:    lSource   [IN]  - source value
                SSwap     [IN]  - source swap behavior (little endian, big endian)
                DSwap     [IN]  - destination swap behavior

  returnvalue:  swapped value
  -----------------------------------------------------------------------------
*/
tsp00_Int2 
ls07Swap2BInt(tsp00_Int2 lSource,
              tsp00_SwapKind_Enum  SSwap,
              tsp00_SwapKind_Enum  DSwap);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap4BInt

  -----------------------------------------------------------------------------

  description:  Swaps 4 byte integer.

  arguments:    lSource   [IN]  - source value
                SSwap     [IN]  - source swap behavior (little endian, big endian)
                DSwap     [IN]  - destination swap behavior

  returnvalue:  swapped value
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07Swap4BInt(tsp00_Int4 lSource,
              tsp00_SwapKind_Enum  SSwap,
              tsp00_SwapKind_Enum  DSwap);



/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap4BCharInPlace

  -----------------------------------------------------------------------------

  description:  Swaps 4 byte char in place.

  arguments:    pszSource [IN]  - source value
                SSwap     [IN]  - source swap behavior (little endian, big endian)
                DSwap     [IN]  - destination swap behavior

  returnvalue:  swapped value
  -----------------------------------------------------------------------------
*/
void
ls07Swap4BCharInPlace(tsp00_Addr          pszSource,
                      tsp00_SwapKind_Enum SSwap,
                      tsp00_SwapKind_Enum DSwap);

              
/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swap2BCharInPlace

  -----------------------------------------------------------------------------

  description:  Swaps 2 byte char in place.

  arguments:    pszSource [IN]  - source value
                SSwap     [IN]  - source swap behavior (little endian, big endian)
                DSwap     [IN]  - destination swap behavior

  returnvalue:  swapped value
  -----------------------------------------------------------------------------
*/
inline void
ls07Swap2BCharInPlace(tsp00_Addr          pszSource,
                      tsp00_SwapKind_Enum SSwap,
                      tsp00_SwapKind_Enum DSwap)
{
    if (SSwap != DSwap)
    {
        char c0 = pszSource[0];

        pszSource[0] = pszSource[1];
        pszSource[1] = c0;
    }

    return;
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07SwapUCS2StringInPlace
  -----------------------------------------------------------------------------

  description:  Swaps every 2 byte in a string.

  arguments:    pszSource  [IN/OUT]  - source value
                lSourceLen [IN]      - source length

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
ls07SwapUCS2StringInPlace(tsp00_Addr  pszSource, tsp00_Longuint  lSourceLen)
{
    char ci;
    for (tsp00_Longuint i = 0; i < lSourceLen-1; i+=2)
    {
        ci             = pszSource[i];
        pszSource[i]   = pszSource[i+1];
        pszSource[i+1] = ci;
    }
    return;
}

/*!
  -----------------------------------------------------------------------------
 
  function:     ls07FourByteToSReal
  -----------------------------------------------------------------------------

  description:  Converts 4 byte hex-value to float (short real).

  arguments:    pszValue   [IN]    - value to convert

  returnvalue:  converted value as short real
  -----------------------------------------------------------------------------
*/
tsp00_Shortreal
ls07FourByteToSReal(tsp00_Addr  pszValue);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07EightByteToLReal
  -----------------------------------------------------------------------------

  description:  Converts 8 byte hex-value to double (long real).

  arguments:    pszValue   [IN]    - pointer, value to convert

  returnvalue:  converted value as long real
  -----------------------------------------------------------------------------
*/
tsp00_Longreal
ls07EightByteToLReal(tsp00_Addr  pszValue);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07NumberToInt
  -----------------------------------------------------------------------------

  description:  Converts number value (internal db format) of length
                mxsp_resnum (=7) to  4 byte int

  arguments:    pszNumberString  [IN]   - value to convert (fixed(10,0))
                Dest             [OUT]  - converted 4 byte integer

  returnvalue:  errOK_els00              - everything went fine
                errNumberTruncated_els98 -
                errNumberOverflow_els98  -
                errInvalidNumber_els98   -
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07NumberToInt(tsp00_Addr pszNumberString, tsp00_Int4& Dest);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls07IntToNumber
  -----------------------------------------------------------------------------

  description:  Converts 4 byte int to number-value (internal db format) of length
                mxsp_resnum (max = 7).

  arguments:    Source           [IN]    - 4 byte integer to convert
                pszNumberString  [OUT]   - buffer for converted value (fixed(10,0))
                
  returnvalue:  errOK_els00              - everything went fine
                errNumberTruncated_els98 -
                errNumberOverflow_els98  -
                errInvalidNumber_els98   -
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls07IntToNumber(tsp00_Int4 Source, tsp00_Addr pszNumberString);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07CheckIntValueRange
  -----------------------------------------------------------------------------

  description:  Checks the range of given number (given as string) as signed int

  arguments:    DataType    [IN]  - data type to get the fill byte for
  
  returnvalue:  1 - range is ok
                0 - range not ok 
  -----------------------------------------------------------------------------
*/
int
ls07CheckIntValueRange(_TCHAR *pszValue);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07GetFillValue - inline function
  -----------------------------------------------------------------------------

  description:  Returns filling char depending on data type

  arguments:    DataType    [IN]  - data type to get the fill byte for
  
  returnvalue:  fill byte
  -----------------------------------------------------------------------------
*/
inline char
ls07GetFillValue(tsp00_DataType_Enum DataType)
{
    if (dche == DataType)
    {
        return DEF_BYTE_EBCDIC_LS00;
    }
    
    return DEF_BYTE_ASCII_LS00;
}
// ls07GetFillValue()


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07GetDefByte
  -----------------------------------------------------------------------------

  description:  Defines the define byte depending on the data type.

  arguments:    DataType    [IN]  - data type to get the def byte for
  
  returnvalue:  defined byte
  -----------------------------------------------------------------------------
*/
inline char
ls07GetDefByte(tsp00_DataType_Enum DataType)
{
    return szDefByteArray_ls07[DataType];
}
// ls07GetDefByte()



/*!
  -----------------------------------------------------------------------------
 
  function:     ls07GetFillByte
  -----------------------------------------------------------------------------

  description:  Defines the fill byte depending on the data type.

  arguments:    DataType    [IN]  - data type to get the fill byte for
  
  returnvalue:  fill byte
  -----------------------------------------------------------------------------
*/
inline char
ls07GetFillByte(tsp00_DataType_Enum DataType)
{
    return szFillByteArray_ls07[DataType];
}
// ls07GetFillByte()


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07AddLargeNumbers (overloaded)
  -----------------------------------------------------------------------------

  description:  Adds values of 2 structures of type tls00_LargeNumber. These
                values are used for file length and positions.

  arguments:    plTarget  [IN/OUT]  - value to add plSource to
                plSource  [IN]      - value to be added to plTarget

  returnvalue:  no
  -----------------------------------------------------------------------------
*/
inline void
ls07AddLargeNumbers(tls00_LargeNumber &lTarget, const tls00_LargeNumber &lSource)
{
    tsp00_Longint lRest = MAX_INT4_SP00 - lTarget.lnuRemainder_ls00;
    if (lSource.lnuRemainder_ls00 >= lRest)
    {
        ++lTarget.lnu2GB_Blocks_ls00;
        lTarget.lnuRemainder_ls00 = lSource.lnuRemainder_ls00 - lRest;
    }
    else
    {
        lTarget.lnuRemainder_ls00 += lSource.lnuRemainder_ls00;
    }
    lTarget.lnu2GB_Blocks_ls00 += lSource.lnu2GB_Blocks_ls00;
    return;
}
// ls07AddLargeNumbers()


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07AddLargeNumbers (overloaded)
  -----------------------------------------------------------------------------

  description:  Adds a value of type long int to value of type 
                tls00_LargeNumber. Values of type tls00_LargeNumber are used 
                for file length and positions.

  arguments:    plTarget  [IN/OUT]  - value to add plSource to
                plSource  [IN]      - value to be added to plTarget

  returnvalue:  no
  -----------------------------------------------------------------------------
*/
inline void
ls07AddLargeNumbers(tls00_LargeNumber &lTarget, tsp00_Longint lSource)
{
    tsp00_Longint lRest = MAX_INT4_SP00 - lTarget.lnuRemainder_ls00;
    if (lSource >= lRest)
    {
        ++lTarget.lnu2GB_Blocks_ls00;
        lTarget.lnuRemainder_ls00 = lSource - lRest;
    }
    else
    {
        lTarget.lnuRemainder_ls00 += lSource;
    }
    return;
}
// ls07AddLargeNumbers()


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07AddLargeNumbers (overloaded)
  -----------------------------------------------------------------------------

  description:  Adds a value of type long int to a value of type tls00_LargeNumber.
                Values of type tls00_LargeNumber are used for file length and
                positions.

  arguments:    lAddend1  [IN]  - first addend
                lAddend2  [IN]  - second addend

  returnvalue:  result of addition
  -----------------------------------------------------------------------------
*/
inline tls00_LargeNumber
ls07AddLargeNumbersEx(const tls00_LargeNumber &lAddend1, tsp00_Longint lAddend2)
{
    tls00_LargeNumber result;
    result = lAddend1;

    tsp00_Longint lRest = MAX_INT4_SP00 - lAddend1.lnuRemainder_ls00;
    if (lAddend2 >= lRest)
    {
        ++result.lnu2GB_Blocks_ls00;
        result.lnuRemainder_ls00 = lAddend2 - lRest;
    }
    else
    {
        result.lnuRemainder_ls00 += lAddend2;
    }
    return result;
}
// ls07AddLargeNumbersEx()


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07SubtractLargeNumbers
  -----------------------------------------------------------------------------

  description:  Subtracts a value of type tls00_LargeNumber from another value
                of the same type. Values of type tls00_LargeNumber are used for
                file length and positions.

  arguments:    lMinuend    [IN]  - value to subtract from
                lSubtrahend [IN]  - value to subtract

  returnvalue:  result of subtraction
  -----------------------------------------------------------------------------
*/
inline tls00_LargeNumber
ls07SubtractLargeNumbers(const tls00_LargeNumber &lMinuend, const tls00_LargeNumber &lSubtrahend)
{
    // Using this function we need to keep in mind that we subtract file positions
    // here. Thus a resulting file position may never become negative!
    
    tls00_LargeNumber result;

    result.lnu2GB_Blocks_ls00 = lMinuend.lnu2GB_Blocks_ls00 - lSubtrahend.lnu2GB_Blocks_ls00;
    result.lnuRemainder_ls00  = lMinuend.lnuRemainder_ls00 - lSubtrahend.lnuRemainder_ls00;

    // The following rules apply:
    // 1. If the difference of counts of 2GB-Blocks is 0 then the difference of the remainders
    //    is the result. The result may be positive or negative or even 0.
    // 2. If the difference of counts of 2GB-Blocks is not 0 we have to consider:
    //   a. Remainder and count of 2GB-Blocks are both pos or neg -> this is the result
    //      example:
    //      lMinuend                                lSubtrahend
    //      0 * blocks-of-2GB + 30 Byte             3 * blocks-of-2GB + 900 Byte
    //      ---> result: -3 * blocks-of-2GB  -870 Byte
    //      That means we need to move the file pointer by 3*2GB backwards and again 870 Byte
    //      backwards to reach the position of 30 Byte in a certain file.
    //
    //   b. Remainder is pos and count of 2GB-Blocks is neg
    //      -> We need to add 1 to the count of 2GB-Blocks and to substract 2GB from
    //         the remainder
    //      example when not using b:
    //      lMinuend                                lSubtrahend
    //      0 * blocks-of-2GB + 999 Byte            3 * blocks-of-2GB + 85 Byte
    //      ---> result: -3 * blocks-of-2GB  +914 Byte
    //      That means we need to move the file pointer by 3*2GB backwards and then 914 Bytes
    //      forward to reach the position of 999 Byte in a certain file.
    //      But this can be simpler done using the rule b
    //      ---> result: (-3 + 1 = -2) * blocks-of-2GB + (914 - 2147483647) Bytes
    //      That means we need to move the file pointer by 2*2GB backwards and again 2147482733 Bytes
    //      backwards to reach the position of 999 Byte in a certain file.
    //
    //   c. Remainder is neg and count of 2GB-Blocks is pos
    //      -> We need to substract 1 from the count of 2GB-Blocks and to add 2GB to
    //         the remainder
    //      example is similar to b

    if (0 != result.lnu2GB_Blocks_ls00)
    {
        if ( ( (result.lnuRemainder_ls00 < 0) && (result.lnu2GB_Blocks_ls00 > 0) ) ||
             ( (result.lnuRemainder_ls00 > 0) && (result.lnu2GB_Blocks_ls00 < 0) ) )
        {
            tsp00_Longint factor = (result.lnu2GB_Blocks_ls00 < 0) ? -1 : 1;
            result.lnu2GB_Blocks_ls00 -= factor;
            result.lnuRemainder_ls00  += factor*MAX_INT4_SP00;
        }
    }
    return result;
}
// ls07SubtractLargeNumbers()


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07DefineSwap - inline function

  -----------------------------------------------------------------------------

  description:  Returns the swap behavior on the machine currently running on

  arguments:    dummy    [IN]  - dummy pointer, not used
  
  returnvalue:  swap kind
  -----------------------------------------------------------------------------
*/
inline tsp00_SwapKind_Enum
ls07DefineSwap(void *dummy)
{
    union
    {
        tsp00_Int4 i4;
        tsp00_C4   c4;
    } uMap;

    uMap.i4 = 65536;
    tsp00_Int4 lSwap = 0;

    for (lSwap; ( (lSwap < 4) && (1 != uMap.c4[lSwap]) ); ++lSwap )
        ;

    return STATIC_CAST(tsp00_SwapKind_Enum, lSwap);
}
// ls07DefineSwap()


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Swapped - inline function

  -----------------------------------------------------------------------------

  description:  Returns the swap behavior on the machine currently running on
                as bool value.

  arguments:    dummy    [IN]  - dummy pointer, not used; usually NULL
  
  returnvalue:  true   - running on 'swapped' memory architecture
                false  - running on 'non swapped' memory architecture
  -----------------------------------------------------------------------------
*/
inline bool
ls07Swapped(void *dummy)
{
    short swapTest = 1;
    return ( *REINTERPRET_CAST(char*, &swapTest) ? true : false );
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07GetDateAndTime - inline function

  -----------------------------------------------------------------------------

  description:  Returns date and time.

  arguments:    pszDate  [OUT]  - date
                pszTime  [OUT]  - time
  
  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
ls07GetDateAndTime(tsp00_Datec *pszDate, tsp00_Timec *pszTime)
{
    tsp00_Date  Date;
    tsp00_Time  Time;
    
    sqldattime(Date, Time);
    
    pszDate->p2c(Date);
    pszTime->p2c(Time);
}


/*!
  -----------------------------------------------------------------------------
 
  function:     ls07Compare

  -----------------------------------------------------------------------------

  description:  Compares integer values

  arguments:    arg1  [IN]  - first value
                arg2  [IN]  - second value

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
extern "C"
{
int
ls07IntCompare(const void *arg1, const void *arg2);
}


//*
//*     Prototypes (s41, s40, ...) for compatibility reasons with pascal sources vsp41, vsp40
//*
externPascal void s41pbyte(tsp00_Addr   pszDestination,
                           tsp00_Int4   lDestPos,
                           tsp00_Int4 & lDestLen,
                           tsp00_Addr   pszSource,
                           tsp00_Int4   lSourcePos,
                           tsp00_Int4   lSourceLen,
                           bool       & bInvalidValue);

// converts 4 byte integer into number (internal db format - fixed(10,0))
externPascal void s41p4int(tsp00_Addr       pszDestination,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4       lSource,
                           tsp00_NumError & Result);

externPascal void s40g4int(tsp00_Addr       Number,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4&      lDestination,
                           tsp00_NumError & lResult);

externPascal void s40gbyte(tsp00_Addr   pszSource,
                           tsp00_Int4   lSourcePos,
                           tsp00_Int4   lSourceLen,
                           tsp00_Addr   pszHexDest,
                           tsp00_Int4   lDestPos,
                           tsp00_Int4   lDestLen,
                           bool       & bTruncated);

#endif  /* HLS07 */

