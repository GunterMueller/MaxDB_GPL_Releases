/*! 
  -----------------------------------------------------------------------------
 
  module: hls19_check.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  check functions
                
  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 2000-2005 SAP AG-2002
 
  -----------------------------------------------------------------------------



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




*/

#ifndef HLS19_CHECK_H
#define HLS19_CHECK_H

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
 
  function:     ls19CheckFormatSpecLength

  -----------------------------------------------------------------------------

  description:  Checks length of column spec (in command) depending on external
                data type.

  arguments:    InOutType    [IN]  - data type
                ValueLength  [IN]  - column length to check

  returnvalue:  errWrongIntegerLength_els98  - wrong length for integer values
                errWrongRealLength_els98     - wrong length for real values
                errWrongDecimalLength_els98  - wrong length for decimal values
                errWrongZonedLength_els98    - wrong length for zoned values
                errOK_els00                  - everything went fine
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckFormatSpecLength(tls00_FieldFormat *Format, tsp00_Int4 ValueLength);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls19CalcExtLenFrac

  -----------------------------------------------------------------------------

  description:  Computes external length and fraction of values depending of
                scale and trunc values

  arguments:    FieldInfo     [IN]  - field info structure
                ColOption     [IN]  - contains trunc, scale values
                RecalcLenFrac [OUT] - computed values
  returnvalue:  
  -----------------------------------------------------------------------------
*/
void
ls19CalcExtLenFrac(tls00_FieldBasics & FieldInfo,
                   tls00_FieldOption & ColOption,
                   tsp00_Int4          RecalcLenFrac[]);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls19CheckLengthForExtract

  -----------------------------------------------------------------------------

  description:  Checks if the specified length (positions in command) of a column
                fits for values of that column to write.
                In case of bool values the length of bool representation is used.
                
                If the given length is to short the needed length is given back
                together with an error code as return value.
                For LONG values NeededLength is set to ValueLength because in
                that case additional checks need to be done.

  arguments:    ValueLength   [IN]  - specified length in command
                Column        [IN]  - column structure
                FieldInfo     [IN]  - structure with info on length of column in db
                FileFormat    [IN]  - file format structure
                NeededLength  [OUT] - length needed to write a NULL value completely
                                       of interest only if return code is not ok
                ExtLenFrac    [IN]  - 2-dim array, contains count of integer and
                                       fraction digits; defaults to NULL

  returnvalue:  errWrongFieldLength_els98      - wrong length spec
                errIntValueOverflow_els98      - warning
                errWrongDecimalLength_els98    - admissible length of numbers in 
                                                  BCD representation exceeded
                errIncompatibleDataTypes_els98 - incompatible data types (db values
                                                  with fraction cannot be written as
                                                  integers)
                errWrongZonedLength_els98      - too less fraction digits for BCD or 
                                                  zoned values
                errOK_els00                    - no error
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckLengthForExtract(const tsp00_Int4   ValueLength,
                          tls00_Column      *Column,
                          tls00_FieldBasics *FieldInfo,
                          tls00_FileFormat  *FileFormat,
                          tsp00_Int4        &NeededLength,
                          tsp00_Int4         ExtLenFrac[] = NULL);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls19CheckNILValLength

  -----------------------------------------------------------------------------

  description:  Checks if the length specified for a column (positions in command)
                fits to write the NULL representation, tool.
                Considers only those columns that allow NULL values.
                If the length is too less the needed length is returned together
                with a warning.

  arguments:    ValueLength   [IN]  - specified length in command
                Column        [IN]  - column structure
                ColumnMode    [IN]  - indicates if column allows NULL
                FileFormat    [IN]  - file format structure
                NeededLength  [OUT] - length needed to write a NULL value completely
                                       of interest only if return code is not ok

  returnvalue:  errNILValueTruncated_els98 - warning; NULL value truncated
                errOK_els00                - no error
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckNILValLength(const tsp00_Int4    ValueLength,
                      tls00_Column       *Column,
                      tsp1_param_opt_set  ColumnMode,
                      tls00_FileFormat   *FileFormat,
                      tsp00_Int4         &NeededLength);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls19CheckNILValCompatibility

  -----------------------------------------------------------------------------

  description:  Checks if in a colum specification (in command) the given data
                type for a NULL value representation and the external data type
                of that column match.
                If the external data type is not CHAR and the default NULL value
                representation should be used the function generates an error
                because the default NULL value representation is always of type
                CHAR.
                In case the external data type of the specified column is numerical
                and a NULL value representation is explicitely specified the 
                function tries to convert the rep to the proper binary value.
                If this fails an error is generated.

  arguments:    Column      [IN/OUT]  - column structure; variables
                                         colAssign_ls00.fasAsgnVal_ls00 and
                                         colAssign_ls00.fasAsgnLen_ls00 contain
                                         after successful processing the new values
                ColumnMode  [IN]      - indicates if column allows NULL
                FileFormat  [IN]      - file format structure
                pszErrText  [OUT]     - error text

  returnvalue:  errIncompatibleNullValue_els98   - error
                errOK_els00                      - no error

  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckNILValCompatibility(tls00_Column       *Column,
                             tsp1_param_opt_set ColumnMode,
                             tls00_FileFormat   *FileFormat,
                             tsp00_Addr         pszErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls19SortColumns

  -----------------------------------------------------------------------------

  description:  Sort columns by its start position.

  arguments:    MultCol   [IN] - structure containing column specifications

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls19SortColumns(tls00_MultColSpec *MultCol);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls19CheckDataTypeCompatibility

  -----------------------------------------------------------------------------

  description:  Checks if external and internal data type match.

  arguments:    Column       [IN]  - column structure
                FieldInfo    [IN]  - field info structure
                
  returnvalue:  errOK_els00                    - no error
                errIncompatibleDataTypes_els98 - incompatible data types
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckDataTypeCompatibility(tls00_Column      *Column,
                               tls00_FieldBasics *FieldInfo);


#endif // #define HLS19_CHECK_H