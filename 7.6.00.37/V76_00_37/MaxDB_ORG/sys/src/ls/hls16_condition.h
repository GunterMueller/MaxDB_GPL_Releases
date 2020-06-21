/*!
  -----------------------------------------------------------------------------
 
  module: hls16_condition.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Server
 
  description:  Definition of exported functions for condition checking and
                condition converting

  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1999-2005 SAP AG-2000
 
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

#ifndef HLS16_CONDITION
#define HLS16_CONDITION

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
 
  function:     ls16CheckCondition

  -----------------------------------------------------------------------------

  description:  Recursive function to evaluate a complex condition
                (for instance: checking POS 1-3 <> '===' AND POS 4-7 <= 900 AND
                ... OR ...).
                Before calling this function ls16ConvertCondition has to be
                called to make sure the comparison constants are converted to
                the right data type.
                Calls local function ls16_EvaluateCondition.

  arguments:    ConditionSpec   [IN]    - structure of condition specification 
                                           (actually a tree built of simple conditions)
                DataLine        [IN]    - the data line where the data to be
                                           compared has to be extracted from.
                FileFormat      [IN]    - FileFormat structure to know
                                           how to extract the data

  returnvalue:  true        - condition is fullfilled
                false       - condition not fullfilled
  -----------------------------------------------------------------------------
*/
bool
ls16CheckCondition(tls00_CondSpec   &ConditionSpec,
                   tls00_String      DataLine,
                   tls00_FileFormat &FileFormat);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls16ConvertCondition

  -----------------------------------------------------------------------------

  description:  Recursive function to convert comparison constants in a complex
                condition to the data type of the data.
                The possible data types are CHAR (no conversion), DECIMAL, INTEGER,
                ZONED and REAL.
                Conversion takes place for numerical values!
                To convert the comparison constant first the constant is
                converted to float and then REconverted to the data type of
                the data.
                In case of DEFAULT NULL specification for a column this value is
                converted to the encoding type of the data file and if necessary
                to hex representation, too. For this purpose the function allocates
                space that must be released by the caller
                (ConditionSpec.cosCond_ls00->conCompVal_ls00).

  arguments:    ConditionSpec   [IN]    - structure of condition specification 
                                           (actually a tree built of simple conditions)
                Swap            [IN]    - swap behavior of the machine.
                FileFormat      [IN]    - FileFormat structure to know
                                           how to convert the constants
                pszErrText      [OUT]   - Error text

  returnvalue:  errOK_els00             - no error, constants converted
                errInvalidNumber_els98  - in case the constant could not be converted
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls16ConvertCondition(tls00_CondSpec&      ConditionSpec,
                     tsp00_SwapKind_Enum  Swap,
                     tls00_FileFormat    &FileFormat,
                     tsp00_Addr           pszErrText);


//*
//*     Prototypes (vsp40) for compatibility reasons with pascal sources
//*
externPascal void s40glrel(tsp00_Addr      pszSource,
                           tsp00_Int4      lSourcePos,
                           tsp00_Int4      lSourceLen,
                           tsp00_Longreal& lDestination,        // das ist ein typedef auf double
                           tsp00_NumError& lResult);

#endif //HLS16_CONDITION