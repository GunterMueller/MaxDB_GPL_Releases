/*!
  -----------------------------------------------------------------------------
 
  module: hls25.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Transformation routines for numeric values

  version:      7.4

  -----------------------------------------------------------------------------
 
  Copyright (c) 1998-2005 SAP AG-2002
 
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

#ifndef HLS25
#define HLS25

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
 
  function:     ls25PrepareNumbers

  -----------------------------------------------------------------------------

  description:  Converts a user given value into a number. The values may be 
                integer, decimal, zoned, real or strings.

                ATTENTION
                The function allocates memory for the value to return which must
                be freed by the caller.

  arguments:    pszValue    [IN/OUT]  - string to transform to number and result
                                         that'll given back
                lValueLen   [IN/OUT]  - value length before and after conversion
                Column          [IN]  - structure with info on column to load
                FieldBasicInfo  [IN]  - structure with defined
                                         lengths and fraction (database defined)
                LoadServerSwap  [IN]  - swap behavior of server
                FileFormat      [IN]  - structure with definition of user given
                                         number specs (like digit grouping and
                                         decimal symbol)

  returntype:   int
  returnvalue:  errOK_els00              - no error
                errNumberTruncated_els98 - warning: number truncated
                errInvalidNumber_els98   - Invalid number
  -----------------------------------------------------------------------------
*/
int
ls25PrepareNumbers(tsp00_Addr          *pszValue,  // pointer to pointer
                   tsp00_Int4&          lValueLen,
                   tls00_Column&        Column,
                   tls00_FieldBasics   &FieldBasicInfo,
                   tsp00_SwapKind_Enum  LoadServerSwap,
                   tls00_FileFormat&    FileFormat);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls25ExtractNumbers

  -----------------------------------------------------------------------------

  description:  Converts a db internal number to external value of type integer,
                decimal, zoned, real or string.
                Handles external hex representatioin.

                ATTENTION
                The function allocates memory for the value to return which must
                be freed by the caller.

  arguments:    pszValue        [IN]  - number to transform to external
                                         
                pszDestData     [OUT] - converted value; caller must allocate 
                                        enough memory;
                                         in case the return value is 
                                            errNumberOverflow_els98 or
                                            errInvalidNumber_els98
                                         pszDestData will be filled with  '*'
                lDestDataLength [OUT] - value length after conversion
                Column          [IN]  - structure with info on column to extract
                FInfo           [IN]  - structure with defined
                                         lengths and fraction (database defined)
                FileFormat      [IN]  - structure with definition of user given
                                         number specs (like digit grouping and
                                         decimal symbol)
                LoadServerSwap  [IN]  - swap behavior of server
                ExtLenFrac      [IN]  - array with 2 members: external length and
                                         number of fraction digits; necessary for
                                         extracting numbers; may not be NULL

  returntype:   tsp00_Int4
  returnvalue:  errOK_els00              - no error
                errNumberTruncated_els98 - warning: number truncated
                errNumberOverflow_els98  - number overflow
                errInvalidNumber_els98   - Invalid number
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls25ExtractNumbers(tsp00_Addr            pszValue,
                              tsp00_Addr           &pszDestData,
                              tsp00_Longint        &lDestDataLength,
                              tls00_Column         *Column,
                              tls00_FieldBasics    *FInfo,
                              tls00_FileFormat     *FileFormat,
                              tsp00_SwapKind_Param  LoadServerSwap,
                              tsp00_Int4            ExtLenFrac[]);


//*
//*     Prototypes (s43...) for compatibility reasons with pascal sources vsp43 and vsp51
//*
externPascal void s43pstr(tsp00_Addr       pszDestination,
                          tsp00_Int4       lDestPos,
                          tsp00_Int4       lDestLen,
                          tsp00_Int4       lDestFrac,
                          tsp00_Addr       pszSource,
                          tsp00_Int4       lSourcePos,
                          tsp00_Int4       lSourceLen,
                          tsp00_NumError & Result);

externPascal void s44epstr(tsp00_Addr                  pszDestination,  
                           tsp00_Int4                  lDestPos,
                           tsp00_Int4                  lDestLen,
                           tsp00_Int4                  lDestFrac,
                           tsp00_Addr                  pszSource,
                           tsp00_Int4                  lSourcePos,
                           tsp00_Int4                  lSourceLen,
                           tsp00_DecimalPresentation & DecimalPres,
                           tsp00_NumError&             Result);

externPascal void s43lfrac(tsp00_Addr   pszSource,
                           tsp00_Int4   lSourcePos,
                           tsp00_Int4   lSourceLen,
                           tsp00_Int4 & lDigLen,
                           tsp00_Int4 & lDigFrac,
                           tsp00_Int4 & lByteLen);
 

externPascal void s51scale(tsp00_Number &   Number,
                           tsp00_Int4       ScalingFactor,
                           tsp00_NumError & lResult);

externPascal void s51round(tsp00_Addr       pszSource,
                           tsp00_Int4       lSourcePos,
                           tsp00_Int4       lSourceLen,
                           tsp00_Int4       lRoundFactor,
                           tsp00_Addr       pszDestination,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4       lDestLen,
                           tsp00_Int4       lFractionFactor,
                           tsp00_Int4&      resbytelen,
                           tsp00_NumError & lResult);

externPascal void s51trunc(tsp00_Addr       pszSource,
                           tsp00_Int4       lSourcePos,
                           tsp00_Int4       lSourceLen,
                           tsp00_Int4       lTruncFactor,
                           tsp00_Addr       pszDestination,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4       lDestLen,
                           tsp00_Int4       lFractionFactor,
                           tsp00_Int4&      resbytelen,
                           tsp00_NumError & lResult);

// function used to convert an integer value to number
externPascal void s41plint(tsp00_Addr   pszDestination,
                           tsp00_Int4   lDestPos,
                           tsp00_Int4   lDestLen,
                           tsp00_Int4   lFractionFactor,
                           tsp00_Int4   lSource,
                           tsp00_NumError&  Result);

// functions used to convert a real value to number
externPascal void s41psrel(tsp00_Addr       pszDestination,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4       lDestLen,
                           tsp00_Int4       lFractionFactor,
                           tsp00_Shortreal  fSource,
                           tsp00_NumError&  Result);

externPascal void s41plrel(tsp00_Addr       pszDestination,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4       lDestLen,
                           tsp00_Int4       lFractionFactor,
                           tsp00_Longreal   fSource,
                           tsp00_NumError&  Result);

externPascal void s41pdec (tsp00_Addr       pszDestination,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4       lDestLen,           // in digits!
                           tsp00_Int4       lFractionFactor,
                           tsp00_Decimal&   fSource,
                           int              lSourceLengthInDigits,
                           int              lSourceFraction,
                           tsp00_NumError&      Result);

externPascal void s41pzone(tsp00_Addr       pszDestination,
                           tsp00_Int4       lDestPos,
                           tsp00_Int4       lDestLen,           // in digits!
                           tsp00_Int4       lFractionFactor,
                           tsp00_Zoned&     fSource,
                           int              lSourceLengthInDigits,
                           int              lSourceFraction,
                           tsp00_NumError&  Result);

externPascal void s42gstr(tsp00_Addr       pszNumber,
                          tsp00_Int4       lSourcePos,
                          tsp00_Int4       lSourceLen,  // output length of the number in digits (not in bytes!)
                          tsp00_Int4       lFrac,       // number of decimal places or FLOAT
                          tsp00_Int4       lOrigLen,    // internal length of the number in digits (lSourceLen <= lOrigLen possible)
                          tsp00_Addr       pszDest,
                          tsp00_Int4       lDestPos,
                          tsp00_Int4 &     lDestLen,    // for strings, length of the result
                          tsp00_NumError & lResult);

externPascal void s44egstr(tsp00_Addr                  pszNumber,
                           tsp00_Int4                  lSourcePos,
                           tsp00_Int4                  lSourceLen,  // output length of the number in digits (not in bytes!)
                           tsp00_Int4                  lFrac,       // number of decimal places or FLOAT
                           tsp00_Addr                  pszDest,
                           tsp00_Int4                  lDestPos,
                           tsp00_Int4 &                lDestLen,    // for strings, length of the result
                           tsp00_DecimalPresentation & DecimalPres,
                           tsp00_NumError&             Result);

externPascal void s40gsint(tsp00_Addr       pszNumber,
                           tsp00_Int4       lSourcePos,
                           tsp00_Int4       lSourceLen, // length of the VDN number in digits (not in bytes!)
                           tsp00_Int2 &     lDest,
                           tsp00_NumError&  Result);

externPascal void s40glint(tsp00_Addr       pszNumber,
                           tsp00_Int4       lSourcePos,
                           tsp00_Int4       lSourceLen, // length of the VDN number in digits (not in bytes!)
                           tsp00_Int4 &     lDest,
                           tsp00_NumError&  Result);


externPascal void s40gdec(tsp00_Addr        pszNumber,
                           tsp00_Int4       lSourcePos,
                           tsp00_Int4       lSourceLen, // length of the VDN number in digits (not in bytes!)
                           tsp00_Decimalc & pszDest,
                           tsp00_Int4       lDestLen,   // number of digits
                           tsp00_Int4       lDestFrac,  // number of fraction digits
                           tsp00_NumError & Result);

externPascal void s40gzone(tsp00_Addr       pszNumber,
                           tsp00_Int4       lSourcePos,
                           tsp00_Int4       lSourceLen,
                           tsp00_Zonedc   & pszDest,     // typedef to char[20] with trailing NULL
                           tsp00_Int4       lDestLen,
                           tsp00_Int4       lDestFrac,
                           tsp00_NumError & Result);

externPascal void s40gsrel(tsp00_Addr        pszNumber,
                           tsp00_Int4        lSourcePos,
                           tsp00_Int4        lSourceLen,
                           tsp00_Shortreal & pszDest,
                           tsp00_NumError  & Result);

externPascal void s40glrel(tsp00_Addr        pszNumber,
                           tsp00_Int4        lSourcePos,
                           tsp00_Int4        lSourceLen,
                           tsp00_Longreal  & pszDest,
                           tsp00_NumError  & Result);

#endif /*HLS25*/