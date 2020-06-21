/*!
  -----------------------------------------------------------------------------
  module: vls25.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS

  special area: Replication Server
 
  description:  Implementation of transformation routines for numeric values
                
  version:      7.6.
  -----------------------------------------------------------------------------
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

*/
#include <assert.h>

#include "gls00.h"
#include "hls05.h"      // String help functions
#include "hls07.h"
#include "hls25.h"
#include "hls98msg.h"

/*!  
  -----------------------------------------------------------------------------
  Chapter: Locale functions
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
 
  function:     ls25_CheckIntRange - inline

  -----------------------------------------------------------------------------

  description:  Checks range of integer and smallint (already packed) db-internal
                numbers.
                
                The reference values are 20 bytes long - so the caller must make
                sure that the packed numbers are at least of the same length or
                longer. Otherwise the result is undefined.

  arguments:    pszValue   [IN]  - Value to check
                DataType   [IN]  - Data type of value

  returntype:   tsp00_NumError_Enum
  returnvalue:  num_ok           - everything is fine
                num_invalid      - invalid range
  -----------------------------------------------------------------------------
*/
inline tsp00_NumError_Enum
ls25_CheckIntRange(tsp00_Addr pszValue, tsp00_DataType_Enum DataType);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls25_Transform

  -----------------------------------------------------------------------------

  description:  Performs special operations like scaling, truncating/rounding
                on a packed number. The values for scaling, ... are delivered
                by the caller.
                The number given back by this function has the right length
                and fraction defined by the database.

                The caller must ensure that the value to transform is 20 bytes
                long. Otherwise the result is undefined.

  arguments:    pszValue    [IN/OUT]  - number string to transform and result
                                         that'll given back
                lValueLen   [IN/OUT]  - length of number before and after operation
                Option          [IN]  - structure contains information on scaling,
                                         truncating, rounding
                FieldBasicInfo  [IN]  - structure with defined
                                         lengths and fraction (database defined) 
                ExtLenFrac[]    [IN]  - array with 2 members: external length and
                                         number of fraction digits; necessary for
                                         extracting numbers

  returntype:   tsp00_NumError_Enum
  returnvalue:  num_ok         - no error
                num_trunc
                num_overflow
                num_invalid

  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_Transform(tsp00_Addr          pszValue,
               tsp00_Int4&         lValueLen,
               tls00_FieldOption & Option,
               tls00_FieldBasics & FieldBasicInfo,
               tsp00_Int4          ExtLenFrac[] = NULL);


/*!
  -----------------------------------------------------------------------------

  function:     ls25_CharToNumber

  -----------------------------------------------------------------------------

  description:  Converts string to db internal number format

  arguments:    pszNumber       [OUT] - converted value
                pszValue        [IN]  - string value
                lValueLen       [IN]  - length of string value
                FieldBasicInfo  [IN]  - structure with defined
                                         lengths and fraction (database defined)
                FileFormat      [IN]  - structure with definition of user given
                                         number specs (like digit grouping and
                                         decimal symbol)
                bAnyOperation   [IN]  - indicates additional operations on number
                                         (round, scale, ...)

  returntype:   tsp00_NumError_Enum
  returnvalue:  num_ok                - no error
                num_trunc
                num_overflow
                num_invalid
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_CharToNumber(tsp00_Addr         pszNumber,
                  tsp00_Addr         pszValue,
                  tsp00_Int4         lValueLen,
                  tls00_FieldBasics &FieldBasicInfo,
                  tls00_FileFormat  &FileFormat,
                  bool               bAnyOperation);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls25_IntegerToNumber

  -----------------------------------------------------------------------------

  description:  Converts integer value (binary) to db internal number format

  arguments:    pszNumber       [OUT] - converted value
                pszValue        [IN]  - integer value
                lValueLen       [IN]  - length of integer value
                FieldBasicInfo  [IN]  - structure with defined
                                         lengths and fraction (database defined)
                LoadServerSwap  [IN]  - swap behavior of server
                FileFormat      [IN]  - structure with definition of user given
                                         number specs (like digit grouping and
                                         decimal symbol)
                bAnyOperation   [IN]  - indicates additional operations on number
                                         (round, scale, ...)

  returntype:   tsp00_NumError_Enum
  returnvalue:  num_ok                - no error
                num_trunc
                num_overflow
                num_invalid
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_IntegerToNumber(tsp00_Addr           pszNumber,
                     tsp00_Addr           pszValue,
                     tsp00_Int4           lValueLen,
                     tls00_FieldBasics   &FieldBasicInfo,
                     tsp00_SwapKind_Enum  LoadServerSwap,
                     tls00_FileFormat    &FileFormat,
                     bool                 bAnyOperation);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls25_RealToNumber

  -----------------------------------------------------------------------------

  description:  Converts real value (binary) to db internal number format

  arguments:    pszNumber       [OUT] - converted value
                pszValue        [IN]  - real value
                lValueLen       [IN]  - length of real value
                FieldBasicInfo  [IN]  - structure with defined
                                         lengths and fraction (database defined)
                bAnyOperation   [IN]  - indicates additional operations on number
                                         (round, scale, ...)

  returntype:   tsp00_NumError_Enum
  returnvalue:  num_ok                - no error
                num_trunc
                num_overflow
                num_invalid
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_RealToNumber(tsp00_Addr         pszNumber,
                  tsp00_Addr         pszValue,
                  tsp00_Int4         lValueLen,
                  tls00_FieldBasics &FieldBasicInfo,
                  bool               bAnyOperation);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls25_DecimalToNumber

  -----------------------------------------------------------------------------

  description:  Converts decimal value (binary) to db internal number format

  arguments:    pszNumber       [OUT] - converted value
                pszValue        [IN]  - decimal value
                lValueLen       [IN]  - length of decimal value
                ColFormat       [IN]  - structure with info on column to load
                FieldBasicInfo  [IN]  - structure with defined
                                         lengths and fraction (database defined)
                bAnyOperation   [IN]  - indicates additional operations on number
                                         (round, scale, ...)

  returntype:   tsp00_NumError_Enum
  returnvalue:  num_ok                - no error
                num_trunc
                num_overflow
                num_invalid
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_DecimalToNumber(tsp00_Addr         pszNumber,
                     tsp00_Addr         pszValue,
                     tsp00_Int4         lValueLen,
                     tls00_FieldFormat &ColFormat,
                     tls00_FieldBasics &FieldBasicInfo,
                     bool               bAnyOperation);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls25_ZonedToNumber - local function

  -----------------------------------------------------------------------------

  description:  Converts zoned value (binary) to db internal number format

  arguments:    pszNumber       [OUT] - converted value
                pszValue        [IN]  - zoned value
                lValueLen       [IN]  - length of zoned value
                ColFormat       [IN]  - structure with info on column to load
                FieldBasicInfo  [IN]  - structure with defined
                                         lengths and fraction (database defined)
                bAnyOperation   [IN]  - indicates additional operations on number
                                         (round, scale, ...)

  returntype:   tsp00_NumError_Enum
  returnvalue:  num_ok                - no error
                num_trunc
                num_overflow
                num_invalid
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_ZonedToNumber(tsp00_Addr         pszNumber,
                   tsp00_Addr         pszValue,
                   tsp00_Int4         lValueLen,
                   tls00_FieldFormat &ColFormat,
                   tls00_FieldBasics &FieldBasicInfo,
                   bool               bAnyOperation);




/*
  -----------------------------------------------------------------------------
  function:     ls25_CheckIntRange - inline
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_CheckIntRange(tsp00_Addr pszValue, tsp00_DataType_Enum DataType)
{
    assert( (DataType == dsmallint) || (DataType == dinteger) );    // No other data type accepted.

    if (dsmallint == DataType)
    {
        if ( (memcmp(pszValue, csp_minsint, 20) < 0 ? 1 : 0) ||     // pszValue < csp_minsint
             (memcmp(pszValue, csp_maxsint, 20) > 0 ? 1 : 0) )      // pszValue > csp_maxsint
        {
            return num_invalid;
        }
    }
    else if (dinteger == DataType)
    {
        if ( (memcmp(pszValue, csp_minlint, 20) < 0 ? 1 : 0) ||     // pszValue < csp_minlint
             (memcmp(pszValue, csp_maxlint, 20) > 0 ? 1 : 0) )      // pszValue > csp_maxlint
        {
            return num_invalid;
        }
    }

    return num_ok;
}
// ls25_CheckIntRange()


/*
  -----------------------------------------------------------------------------
  function:     ls25_Transform
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_Transform(tsp00_Addr          pszValue,
               tsp00_Int4&         lValueLen,
               tls00_FieldOption & Option,
               tls00_FieldBasics & FieldBasicInfo,
               tsp00_Int4          ExtLenFrac[])
{
    ROUTINE_DBG_MEO00 ("ls25_Transform");

    tsp00_NumError Result;
    Result.becomes(num_ok);

    // Length in digits pszValue SHOULD have for insert (db defined)
    tsp00_Int4 lLengthInDigits = FieldBasicInfo.fbLength_ls00;
    if (NULL != ExtLenFrac)
    {
        lLengthInDigits = ExtLenFrac[0];
    }

    // Length in byte pszValue SHOULD have. Return value of functions.  
    tsp00_Int4 lLengthInBytes = FieldBasicInfo.fbInOutLength_ls00 - 1;
    
    // Fraction value defined by database
    tsp00_Int4 lFraction = FieldBasicInfo.fbFraction_ls00;
    if (NULL != ExtLenFrac)
    {
        lFraction = ExtLenFrac[1];
    }


    if ( (true == Option.fopScale_ls00) && (0 != Option.fopScVal_ls00) )
    {
        s51scale(*((tsp00_Number*)pszValue), Option.fopScVal_ls00, Result);
        if (num_ok != Result)
        {
            // The procedure returns the following values:
            //  num_ok       or
            //  num_overflow    This value will only be returned if the number goes beyond
            //                  the largest admissible internal number (i.e. >64 digits)
            return num_overflow;
        }
    }

    // The next operations are actually only necessary if the number contains decimal places
    if (true == Option.fopRound_ls00)
    {
        s51round(pszValue,              // number to round (source)
                 1,                     // position in pszValue-buffer where number starts (source)
                 lValueLen,             // Length of number in bytes (!!!)
                 Option.fopRTVal_ls00,  // Rounding value
                 pszValue,              // number rounded (destination)
                 1,                     // position in pszValue-buffer where number starts (destination)
                 lLengthInDigits,       // demanded length of rounded number in digits (destination)
                 lFraction,             // demanded number of decimal places (destination)
                 lLengthInBytes,        // Length of number in bytes, filled by s51round
                 Result);
    }
    else if (true == Option.fopTrunc_ls00)
    {
        s51trunc(pszValue,              // number to truncate (source)
                 1,                     // position in pszValue-buffer where number starts (source)
                 lValueLen,             // Length of number in bytes (!!!)
                 Option.fopRTVal_ls00,  // truncate value
                 pszValue,              // number truncated (destination)
                 1,                     // position in pszValue-buffer where number starts (destination)
                 lLengthInDigits,       // demanded length of truncated number in digits (destination)
                 lFraction,             // demanded number of decimal places (destination)
                 lLengthInBytes,        // Length of number in bytes, filled by s51trunc
                 Result);
    }
    else
    {   //  No truncating, no rounding.
        //  So stutz (directly german: etwas zurechstutzen) the number to the right length.
        if ( (FieldBasicInfo.fbDataType_ls00 != dfloat) && (ExtLenFrac == NULL) )
        {
            s51trunc(pszValue,
                     1,
                     lLengthInBytes,
                     lFraction,
                     pszValue,
                     1,
                     lLengthInDigits,
                     lFraction,
                     lLengthInBytes,
                     Result);
        }
    }
    if (num_ok != Result)
    {
        return Result;
    }

    //* Check for odd len and remove some trash from the last byte
    if (1 == (1 & lLengthInDigits))
    {
        // Number are packed: 2 digits per byte;
        // a number with odd len may have trash in the last byte:
        // for instance the positive number 12345 has 5 digits; the internal presentation in hex
        // is 12 34 50 where each two digits are one byte. instead of 50 may be there is
        // 5e. So we have to remove the e which is done by dividing and multiplying by 16.
        // 0x50/16 = 5 * 16 = 0x50
        pszValue[lLengthInDigits - 1] = int(pszValue[lLengthInDigits - 1]/16) * 16;
    }
            
    // Assign the right length.
    lValueLen = lLengthInBytes;
    
    return Result;
}
// ls25_Transform()


/*
  -----------------------------------------------------------------------------
  function:     ls25_CharToNumber
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_CharToNumber(tsp00_Addr         pszNumber,
                  tsp00_Addr         pszValue,
                  tsp00_Int4         lValueLen,
                  tls00_FieldBasics &FieldBasicInfo,
                  tls00_FileFormat  &FileFormat,
                  bool               bAnyOperation)
{
    ROUTINE_DBG_MEO00 ("ls25_CharToNumber");

    tsp00_NumError Result;
    Result.becomes(num_ok);
    
    // Adjust the fraction value. In case of float data type in the database
    // we have to assign -1 to prevent the function from truncating.
    tsp00_Int4 lFraction = -1;
    if(dfloat != FieldBasicInfo.fbDataType_ls00)
    {
        lFraction = FieldBasicInfo.fbFraction_ls00;
    }
    
    //*
    //*     Transfom
    //* If the user did not specify any rounding/scaling/truncating factor we can use
    //* the database defined length (in digits) for the NUMBER to create. In case the
    //* user specified a factor we will use the largest possible length for
    //* NUMBERS: MAX_NUMERICLEN_LS00 (= 38).
    //*
    tsp00_Int4  lNumDigits = (false == bAnyOperation) ? FieldBasicInfo.fbLength_ls00 :
                                                        MAX_NUMERICLEN_LS00;

    if ( (NO_THSD_GROUPS_LS00 == FileFormat.ffoThsdChar_ls00) &&     // Standard format.
         (POINT_LS00 == FileFormat.ffoDecPoint_ls00) )
    {       
        s43pstr(pszNumber, 1, lNumDigits, lFraction, pszValue, 1, lValueLen, Result);
    }
    else                                                                // No standard format (decimal symbol
    {                                                                   // and digit grouping symbol extra defined)
        tsp00_DecimalPresentation DP;
        DP.thousand_token = FileFormat.ffoThsdChar_ls00;
        DP.zero_point     = FileFormat.ffoDecPoint_ls00;
        
        s44epstr(pszNumber, 1, lNumDigits, lFraction, pszValue, 1, lValueLen, DP, Result);
    }
    return Result;
}
// ls25_CharToNumber()


/*
  -----------------------------------------------------------------------------
  function:     ls25_IntegerToNumber
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_IntegerToNumber(tsp00_Addr           pszNumber,
                     tsp00_Addr           pszValue,
                     tsp00_Int4           lValueLen,
                     tls00_FieldBasics   &FieldBasicInfo,
                     tsp00_SwapKind_Enum  LoadServerSwap,
                     tls00_FileFormat    &FileFormat,
                     bool                 bAnyOperation)
{
    ROUTINE_DBG_MEO00 ("ls25_IntegerToNumber");

    tsp00_NumError Result;
    Result.becomes(num_ok);
    
    tsp00_Int4 lLongIntValue = 0;   // First transform the binary value into an integer value.

    switch (lValueLen)
    {
        case 1:     // 1 byte value
        {
            lLongIntValue = pszValue[0];
            break;
        }
        case 2:     // 2 byte value
        {
            lLongIntValue = ls07Swap2BHexToShort(pszValue, FileFormat.ffoIntSpec_ls00, LoadServerSwap);
            break;
        }
        case 4:     // 4 byte value
        {
            lLongIntValue = ls07Swap4BHexToLong(pszValue, FileFormat.ffoIntSpec_ls00, LoadServerSwap);
            break;
        }
    }
    
    //*
    //*     Transform
    //* We suppose at this point, that noone gets the idea to load a float value
    //* using the integer format. If so, the value is truncated anyway so
    //* we can use here the database defined fraction.
    //*
    
    // scale, trunc, round ?
    tsp00_Int4  lNumDigits = (false == bAnyOperation) ? FieldBasicInfo.fbLength_ls00 :
                                                        MAX_NUMERICLEN_LS00;
    
    s41plint(pszNumber, 1, lNumDigits, FieldBasicInfo.fbFraction_ls00, lLongIntValue, Result);
    
    return Result;
}
// ls25_IntegerToNumber()


/*
  -----------------------------------------------------------------------------
  function:     ls25_RealToNumber
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_RealToNumber(tsp00_Addr         pszNumber,
                  tsp00_Addr         pszValue,
                  tsp00_Int4         lValueLen,
                  tls00_FieldBasics &FieldBasicInfo,
                  bool               bAnyOperation)
{
    ROUTINE_DBG_MEO00 ("ls25_RealToNumber");

    tsp00_NumError Result;
    Result.becomes(num_ok);

    tsp00_Shortreal fShortReal = 0;
    tsp00_Longreal  fLongReal  = 0;

    // Adjust fraction value. In case of float data type in the database we have to assign
    // -1 to prevent the function from truncating.
    tsp00_Int4 lFraction = -1;
    if(dfloat != FieldBasicInfo.fbDataType_ls00)
    {
        lFraction = FieldBasicInfo.fbFraction_ls00;
    }
    
    //*
    //*     Transform
    //* If the user did not specify any round/scale/trunc factor we use the db defined
    //* length (in digits) for the NUMBER to create. In case the user specified a factor
    //* we will use the largest possible length for NUMBERS: MAX_NUMERICLEN_LS00 (= 38).
    //*
    
    // scale, trunc, round ?
    tsp00_Int4  lNumDigits = (false == bAnyOperation) ? FieldBasicInfo.fbLength_ls00 :
                                                        MAX_NUMERICLEN_LS00;

    if (MAX_SHORTREAL_LS00 == lValueLen)
    {
        fShortReal = ls07FourByteToSReal(pszValue);
        s41psrel(pszNumber, 1, lNumDigits, lFraction, fShortReal, Result);
    }
    else if (MAX_LONGREAL_LS00 == lValueLen)
    {
        fLongReal = ls07EightByteToLReal(pszValue);
        s41plrel(pszNumber, 1, lNumDigits, lFraction, fLongReal, Result);
    }
    else
    {
        Result.becomes(num_invalid);
    }
    
    return Result;
}
// ls25_RealToNumber()


/*
  -----------------------------------------------------------------------------
  function:     ls25_DecimalToNumber
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_DecimalToNumber(tsp00_Addr         pszNumber,
                     tsp00_Addr         pszValue,
                     tsp00_Int4         lValueLen,
                     tls00_FieldFormat &ColFormat,
                     tls00_FieldBasics &FieldBasicInfo,
                     bool               bAnyOperation)
{
    ROUTINE_DBG_MEO00 ("ls25_DecimalToNumber");

    tsp00_NumError Result;
    Result.becomes(num_ok);

    // Copy original string to a decimal string with the right length of 20 bytes
    tsp00_Decimal pszDec;
    SAPDB_memcpy(pszDec, pszValue, lValueLen); 

    // Adjust the length of the user given value (in digits). Leading NULL-characters
    // will be handled by the function s41pdec.
    tsp00_Int4 lDecLenInDigits = 2*lValueLen - 1;
    
    // Adjust the fraction value. In case of float data type in the database we have to assign
    // -1 to prevent the function from truncating.
    tsp00_Int4 lFraction = -1;
    if(dfloat != FieldBasicInfo.fbDataType_ls00)
    {
        lFraction = FieldBasicInfo.fbFraction_ls00;
    }
    
    //*
    //*     Transform
    //* If the user did not specify any round/scale/trunc factor we use the db defined
    //* length (in digits) for the NUMBER to create. In case the user specified a factor
    //* we will use the largest possible length for NUMBERS: MAX_NUMERICLEN_LS00 (= 38).
    //*
    
    // scale, trunc, round ?
    tsp00_Int4 lNumDigits = (false == bAnyOperation) ? FieldBasicInfo.fbLength_ls00
                                                     : MAX_NUMERICLEN_LS00;
        
    s41pdec(pszNumber, 1, lNumDigits, lFraction, pszDec, lDecLenInDigits, ColFormat.ffoFrac_ls00, Result);
                
    return Result;
}
// ls25_DecimalToNumber()


/*
  -----------------------------------------------------------------------------
  function:     ls25_ZonedToNumber - local function
  -----------------------------------------------------------------------------
*/
tsp00_NumError_Enum
ls25_ZonedToNumber(tsp00_Addr          pszNumber,
                   tsp00_Addr          pszValue,
                   tsp00_Int4          lValueLen,
                   tls00_FieldFormat & ColFormat,
                   tls00_FieldBasics & FieldBasicInfo,
                   bool                bAnyOperation)
{
    ROUTINE_DBG_MEO00 ("ls25_ZonedToNumber");

    tsp00_NumError Result;
    Result.becomes(num_ok);

    // Copy the original string into a zoned string with the right length of 20 bytes
    tsp00_Zoned myZoned;
    SAPDB_memcpy(myZoned, pszValue, lValueLen); 

    // The values written in the infile may be padded with space characters. Remove them.
    // This will only adjust the length, not really remove something from the original string.
    ls05StrTrimRight(myZoned, lValueLen);
    
    // The length of the user given value corresponds to the length in digits.
    //  No further action needed.
    
    // Adjust the fraction value. In case of float data type in the database
    //  we have to assign -1 to prevent the function from truncating.
    tsp00_Int4 lFraction = -1;
    if(dfloat != FieldBasicInfo.fbDataType_ls00)
    {
        lFraction = FieldBasicInfo.fbFraction_ls00;
    }
    
    //*
    //*     Now transform the zoned into an internal NUMBER. If the user did not specify
    //*     any rounding/scaling/truncating factor we can use the database defined length
    //*     (in digits) for the NUMBER to create. In case the user specified a factor we
    //*     will use the largest possible length for NUMBERS: MAX_NUMERICLEN_LS00 (= 38).
    //*
    tsp00_Int4 lNumDigits = (false == bAnyOperation) ? FieldBasicInfo.fbLength_ls00
                                                     : MAX_NUMERICLEN_LS00;
        
    s41pzone(pszNumber,
             1,
             lNumDigits,
             lFraction,
             myZoned,
             lValueLen,
             ColFormat.ffoFrac_ls00,
             Result);
                
    return Result;
}
// ls25_ZonedToNumber()


//******************** GLOBALE FUNCTIONS ***************************************


/*
  -----------------------------------------------------------------------------
  function:     ls25PrepareNumbers
  -----------------------------------------------------------------------------
*/
int
ls25PrepareNumbers(tsp00_Addr          *pszValue,
                   tsp00_Int4&          lValueLen,
                   tls00_Column&        Column,
                   tls00_FieldBasics   &FieldBasicInfo,
                   tsp00_SwapKind_Enum  LoadServerSwap,
                   tls00_FileFormat    &FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls25PrepareNumbers");

    tsp00_NumError_Enum lRetValue = num_ok;         // may be: num_ok, num_trunc, num_overflow, num_invalid
    tls00_FieldFormat ColFormat = Column.colFormat_ls00;
    tls00_FieldOption ColOption = Column.colOption_ls00;

    
    //  ALWAYS allocate the biggest possible number of bytes for the string which
    //  holds the NUMBER. Make sure the string is initialized to 0
    int lNumLenInBytes = (int)(MAX_NUMERICLEN_LS00/2) + 1;  // corresponds to sizeof(tsp00_Number)
    
    tsp00_Addr pszNumber = new char[lNumLenInBytes];    
    memset(pszNumber, '\0', lNumLenInBytes);

    switch (ColFormat.ffoFormat_ls00)
    {
        case ioChar_els00:
        {
            // Length of the value MUST NOT exceed the biggest valid number length.
            if ((lValueLen <= 0) || (lValueLen > NUMSTR_MXSP00) )
            {
                lRetValue = num_overflow;
            }
            else
            {
                lRetValue = ls25_CharToNumber(pszNumber, *pszValue, lValueLen,
                                              FieldBasicInfo, FileFormat, ColOption.fopAnyOperation_ls00);
            }
            break;
        }
        case ioInteger_els00:
        {
            lRetValue = ls25_IntegerToNumber(pszNumber, *pszValue, lValueLen, FieldBasicInfo,
                                             LoadServerSwap, FileFormat, ColOption.fopAnyOperation_ls00);
            break;
        }
        case ioReal_els00:
        {
            lRetValue = ls25_RealToNumber(pszNumber, *pszValue, lValueLen,
                                          FieldBasicInfo, ColOption.fopAnyOperation_ls00);
            break;
        }
        case ioDecimal_els00:
        {
            lRetValue = ls25_DecimalToNumber(pszNumber, *pszValue, lValueLen, ColFormat,
                                             FieldBasicInfo, ColOption.fopAnyOperation_ls00);
            break;
        }
        case ioZoned_els00:
        {
            lRetValue = ls25_ZonedToNumber(pszNumber, *pszValue, lValueLen, ColFormat,
                                           FieldBasicInfo, ColOption.fopAnyOperation_ls00);
            break;
        }
        default:
        {
            // this point will never be reached (normally) because the format specification
            // are already checked while parsing the command;
            // give error back
            lRetValue = num_invalid;
        }
    }

    if ( (num_ok == lRetValue) || (num_trunc == lRetValue) )
    {
        if (true == ColOption.fopAnyOperation_ls00)                 // no error so far: check for trunc/scale.
        {   
            lRetValue = ls25_Transform(pszNumber, lNumLenInBytes, ColOption, FieldBasicInfo);
        }
        
        if ( (num_ok == lRetValue) || (num_trunc == lRetValue) )    // Check integer range.
        {
            if ( (dsmallint == FieldBasicInfo.fbDataType_ls00) || (dinteger == FieldBasicInfo.fbDataType_ls00) )
            {
                lRetValue = ls25_CheckIntRange(pszNumber, FieldBasicInfo.fbDataType_ls00);
            }
        }
    }

    int lResult = errOK_els00;      // Define return value for this function.

    // ATTENTION: 2 pass through !!!
    switch (lRetValue)
    {
        case num_trunc:
        {
            lResult = errNumberTruncated_els98;     // return warning; adjust number anyway
        }
        case num_ok:
        {
            // Filling the number with zeroes if the length is less than in_out_l bytes
            // is not necessary because the number is initialized to 0.
            *pszValue = pszNumber;                              // Reassign pointer to value!!!
            lValueLen = FieldBasicInfo.fbInOutLength_ls00 - 1;  // Assign right value if not yet happened
            break;
        }
        case num_overflow:
        default:                                    // case num_invalid:
        {
            lResult = errInvalidNumber_els98;
            delete pszNumber;
        }
    }
    
    return lResult;
}
// ls25_PrepareNumbers()


/*
  -----------------------------------------------------------------------------
  function:     ls25ExtractNumbers
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls25ExtractNumbers(tsp00_Addr            pszValue,
                              tsp00_Addr           &pszDestData,
                              tsp00_Longint        &lDestDataLength,
                              tls00_Column         *Column,
                              tls00_FieldBasics    *FInfo,
                              tls00_FileFormat     *FileFormat,
                              tsp00_SwapKind_Param  LoadServerSwap,
                              tsp00_Int4            ExtLenFrac[])
{
    ROUTINE_DBG_MEO00 ("ls25ExtractNumbers");

    tsp00_Int4        rc         = errOK_els00;

    tls00_FieldFormat *ColFormat = &Column->colFormat_ls00;
    
    tsp00_Int4 lDestLen  = Column->colFPos_ls00.fpoEnd_ls00 - Column->colFPos_ls00.fpoStart_ls00 + 1;
    if (ColFormat->ffoIsHex_ls00)
    {
        lDestLen = lDestLen / 2;
    }

    tsp00_Addr pszDest   = pszDestData;
    
    tsp00_NumError_Enum eResult = num_ok;
    tsp00_NumError      Result;
    Result.becomes(num_ok);

    if (true == Column->colOption_ls00.fopAnyOperation_ls00)
    {
        tsp00_Int4 lDbLength = FInfo->fbInOutLength_ls00 - 1;
        
        // pass lenght of number (= inoutlen of column in db)
        eResult = ls25_Transform(pszValue, lDbLength, Column->colOption_ls00, *FInfo, ExtLenFrac);
    }

    if ( (num_ok == eResult) || (num_trunc == eResult) )
    {
        switch (ColFormat->ffoFormat_ls00)
        {
            case ioChar_els00:
            {
                tsp00_Int4 lFraction = ExtLenFrac[1];
                if (dfloat == FInfo->fbDataType_ls00)
                {
                    lFraction = csp_float_frac;
                }

                if ( (NO_THSD_GROUPS_LS00 == FileFormat->ffoThsdChar_ls00) &&
                     (POINT_LS00 == FileFormat->ffoDecPoint_ls00) )           // standard format.
                {       
                    s42gstr(pszValue,
                            1,
                            ExtLenFrac[0],  // output length of the number in digits (not in bytes!)
                            lFraction,      // number of decimal places or FLOAT
                            FInfo->fbLength_ls00,  // internal length of number in digits (lSourceLen <= lOrigLen possible)
                            pszDest,
                            1,
                            lDestLen,       // length of the result
                            Result);
                }
                else    // No standard format (decimal sign and thsd char are defined)
                {   
                    tsp00_DecimalPresentation DP;
                    DP.thousand_token = FileFormat->ffoThsdChar_ls00;
                    DP.zero_point     = FileFormat->ffoDecPoint_ls00;
        
                    s44egstr(pszValue,
                             1,
                             ExtLenFrac[0],     // output length of the number in digits (not in bytes!)
                             lFraction,         // number of decimal places or FLOAT
                             pszDest,
                             1,
                             lDestLen,          // length of the result
                             DP,
                             Result);
                }
                break;
            }
            case ioInteger_els00:
            {
                switch (lDestLen)
                {
                    case 1:     // 1 byte value
                    {
                        _TCHAR szTempDest[2]     = {'\0'};
                        tsp00_Int2 lSmallIntDest = 0;
                        
                        s40gsint(pszValue,
                                 1,
                                 FInfo->fbLength_ls00,  // db length in digits
                                 lSmallIntDest,
                                 Result);
                        if ( (num_overflow != Result) && (num_invalid != Result) )
                        {
                            ls07Swap2BIntToChar(lSmallIntDest,
                                                LoadServerSwap,
                                                FileFormat->ffoIntSpec_ls00,
                                                szTempDest);
                        }
                        pszDest[0] = szTempDest[0];
                        
                        break;
                    }
                    case 2:     // 2 byte value
                    {
                        tsp00_Int2 lSmallIntDest = 0;
                        s40gsint(pszValue,
                                 1,
                                 FInfo->fbLength_ls00,  // db length in digits
                                 lSmallIntDest,
                                 Result);
                        if ( (num_overflow != Result) && (num_invalid != Result) )
                        {
                            ls07Swap2BIntToChar(lSmallIntDest,
                                                LoadServerSwap,
                                                FileFormat->ffoIntSpec_ls00,
                                                pszDest);
                        }
                        break;
                    }
                    case 4:     // 4 byte value
                    {
                        tsp00_Int4 lIntDest = 0;
                        s40glint(pszValue,
                                 1,
                                 FInfo->fbLength_ls00,  // db length in digits
                                 lIntDest,
                                 Result);
                        if ( (num_overflow != Result) && (num_invalid != Result) )
                        {
                            ls07Swap4BIntToChar(lIntDest, LoadServerSwap, FileFormat->ffoIntSpec_ls00, pszDest);
                        }
                        break;
                    }
                }
                break;
            }
            case ioDecimal_els00:
            {
                tsp00_Decimalc myDest;
                tsp00_Int4     lFraction = ExtLenFrac[1];

                lDestLen = ExtLenFrac[0];
                
                // At this point external fraction may only be GT computed fraction;
                // if so we need to adapt length and fraction again
                if (Column->colFormat_ls00.ffoFrac_ls00 != ExtLenFrac[1])
                {
                    lDestLen += Column->colFormat_ls00.ffoFrac_ls00 - ExtLenFrac[1];
                    lFraction = Column->colFormat_ls00.ffoFrac_ls00;
                }

                s40gdec(pszValue,
                        1,
                        FInfo->fbLength_ls00,   // db length in digits
                        myDest,
                        (2 * lDestLen) - 1,     // external length in digits
                        lFraction,
                        Result);
                // Switch pointer in any case even if result is faulty or nonsense
                SAPDB_memcpy(pszDest, myDest.asCharp(), lDestLen);

                break;
            }
            case ioReal_els00:
            {
                switch (lDestLen)
                {
                    case 4:     // 4 byte value
                    {
                        tsp00_Shortreal pszMySReal;
                        s40gsrel(pszValue,
                                 1,
                                 FInfo->fbLength_ls00,  // db length in digits
                                 pszMySReal,
                                 Result);

                        // Switch pointer in any case even if result is faulty or nonsense
                        SAPDB_memcpy(pszDest, REINTERPRET_CAST(_TCHAR*, &pszMySReal), 4);
                        break;
                    }
                    case 8:     // 8 byte value
                    {
                        tsp00_Longreal pszMyLReal;
                        s40glrel(pszValue,
                                 1,
                                 FInfo->fbLength_ls00,  // db length in digits
                                 pszMyLReal,
                                 Result);
                        
                        // Switch pointer in any case even if result is faulty or nonsense
                        SAPDB_memcpy(pszDest, REINTERPRET_CAST(_TCHAR*, &pszMyLReal), 8);
                        break;
                    }
                }
                break;
            }
            case ioZoned_els00:
            {
                tsp00_Zonedc myDest;
                tsp00_Int4   lFraction = ExtLenFrac[1];

                lDestLen = ExtLenFrac[0];
                // At this point external fraction may only be GT computed fraction;
                // if so we need to adapt length and fraction again
                if (Column->colFormat_ls00.ffoFrac_ls00 != ExtLenFrac[1])
                {
                    lDestLen += Column->colFormat_ls00.ffoFrac_ls00 - ExtLenFrac[1];
                    lFraction = Column->colFormat_ls00.ffoFrac_ls00;
                }

                s40gzone(pszValue, 1, FInfo->fbLength_ls00, myDest, lDestLen, lFraction, Result);
                //                          |                         |
                //                     db length in digits         external length in digits

                // Switch pointer in any case even if result is faulty or nonsense
                SAPDB_memcpy(pszDest, myDest.asCharp(), lDestLen);
            }
        }
    }
    else        // if ( (num_ok == eResult) || (num_trunc == eResult) )
    {
        Result.becomes(eResult);
    }


    // Discard old pointer here even if the result is faulty or nonsense;
    lDestDataLength = lDestLen;

    switch (Result)
    {
        case num_ok:
        {
            rc = errOK_els00;
            break;
        }
        case num_trunc:
        {
            rc = errNumberTruncated_els98;
            break;
        }
        case num_overflow:
        {
            memset(pszDest, _T('*'), lDestLen);
            rc = errNumberOverflow_els98;
            break;
        }
        default:        // = num_invalid:
        {
            memset(pszDest, _T('*'), lDestLen);
            rc = errInvalidNumber_els98;
            break;
        }
    }

    return rc;
}
// ls25ExtractNumbers()