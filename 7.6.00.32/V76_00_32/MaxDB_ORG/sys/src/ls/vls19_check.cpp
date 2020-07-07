/*
  -----------------------------------------------------------------------------
   module: vls19_check.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS und d033893

  special area: SAP DB Loader
 
  description:  Some check functionality
                
  version:      7.6.
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

#include "gls00.h"
#include "hls05.h"
#include "hls07.h"
#include "hls16_condition.h"
#include "hls98msg.h"
#include "hls99.h"
#include "hls19_check.h"


//*********************** L O C A L   F U N C T I O N S ***********************

/*! 
  -----------------------------------------------------------------------------
  Specification
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
 
  function:     ls19_CheckFixLength - local function

  -----------------------------------------------------------------------------

  description:  Checks the length specified for a numerical column.
                Is the length too short the needed length is given back together
                with an error.
                In case too less fraction digits are specified NeededLength
                contains the necessary count of fraction digits.

  attention:    For decimal or zoned values with specified count of external
                fraction the value ExtLenFrac may be adapted.

                ExtLenFrac defaults to NULL because this function is also used
                to compute length of buffer at compressed DATA format, and at time
                of computing this variable has no useful value!

  arguments:    ValueLength   [IN]  - column length
                FieldInfo     [IN]  - field info structure
                Column        [IN]  - column structure
                FileFormat    [IN]  - file format structure
                NeededLength  [OUT] - needed length to write complete NULL value
                                       representation; useful only if return code
                                       not ok
                ExtLenFrac    [IN]  - 2-dim array, contains count of integer and
                                       fraction digits; defaults to NULL

  returnvalue:  errWrongFieldLength_els98      - wrong length spec
                errIntValueOverflow_els98      - warning
                errWrongDecimalLength_els98    - admissible length of numbers in 
                                                  BCD representation exceeded
                errIncompatibleDataTypes_els98 - incompatible DATA types (db values
                                                  with fraction cannot be written as
                                                  integers)
                errWrongZonedLength_els98      - too less fraction digits for BCD or 
                                                  zoned values
                errOK_els00                    - no error
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls19_CheckFixLength(const tsp00_Int4   ValueLength,
                    tls00_FieldBasics *FieldInfo,
                    tls00_Column      *Column,
                    tls00_FileFormat  *FileFormat,
                    tsp00_Int4        &NeededLength,
                    tsp00_Int4         ExtLenFrac[] = NULL)
{
    ROUTINE_DBG_MEO00 ("ls19_CheckFixLength");

    tsp00_Int4          rc        = errOK_els00;
    tls00_FieldOption & FOption   = Column->colOption_ls00;

    tsp00_Int4 lThsdCharNum = 0;
    tsp00_Int4 lFracPart = FieldInfo->fbFraction_ls00;
    tsp00_Int4 lIntPart  = FieldInfo->fbLength_ls00 - lFracPart;
    
    if (NULL != ExtLenFrac)
    {
        lIntPart  = ExtLenFrac[0] - ExtLenFrac[1];      // ExtLenFrac contains complete length!
        if (lIntPart < 0)
        {
            lIntPart = 0;
        }
        lFracPart = ExtLenFrac[1];
    }
    
    switch (Column->colFormat_ls00.ffoFormat_ls00)
    {
        case ioChar_els00:
        {
            if (dfloat != FieldInfo->fbDataType_ls00)
            {
                if (NO_THSD_GROUPS_LS00 != FileFormat->ffoThsdChar_ls00)
                {
                    lThsdCharNum = (lIntPart - 1) / 3;      // should result in integer again
                }

                NeededLength = lIntPart + lFracPart + 2 + lThsdCharNum; // decimal symbol, sign and digit grouping symbol
                if (0 == lFracPart)
                {
                    NeededLength -= 1;                  // sign and digit grouping symbol; no decimal symbol
                }
                else if (0 == lIntPart)
                {
                    NeededLength -= (lThsdCharNum - 1);     // decimal symbol, sign, one integer digit, no digit grouping symbol
                }
            }
            else
            {   // representation '(+-)4.8E+12' --> sign (1 byte), dec symbol (1 byte), exp (4 byte)
                // ExtLenFrac array cannot be used because it is not filled for float values
                NeededLength = FieldInfo->fbLength_ls00 + 6;
            }
            break;
        }
        case ioInteger_els00:
        {
            // if fraction > 0 reject, because no fraction possible: incompatible DATA types
            if ( (lFracPart > 0) || (dfloat == FieldInfo->fbDataType_ls00) )
            {
                rc = errIncompatibleDataTypes_els98;
                break;
            }
            else
            {
                NeededLength = ValueLength;

                if (dinteger == FieldInfo->fbDataType_ls00)
                {
                    if (4 != ValueLength)
                    {
                        rc = errIntValueOverflow_els98;
                    }
                }
                else if (dsmallint == FieldInfo->fbDataType_ls00)
                {
                    if (1 == ValueLength)
                    {
                        rc = errIntValueOverflow_els98;
                    }
                }

                // Ok, 'normal' fixed and integer values which generate overflow with too many
                // integer digits
                if ( (2 == ValueLength) && (lIntPart > 4) ||
                     (4 == ValueLength) && (lIntPart > 9) )
                {
                    rc = errIntValueOverflow_els98;
                }
            }
            break;
        }
        case ioDecimal_els00:
        {
            tsp00_Int4 lFracDiff = Column->colFormat_ls00.ffoFrac_ls00 - lFracPart;
            
            // reject if less fraction digits than defined in db (or computed)
            if (lFracDiff < 0)
            {
                rc = errFractionTooSmall_els98;
                NeededLength = lFracPart;
                break;
            }
            else
            {   // consider also external fraction
                lFracPart = Column->colFormat_ls00.ffoFrac_ls00;
                lIntPart += lFracDiff;
            }

            NeededLength = ((lIntPart + lFracPart) / 2) + 1;
            if (NeededLength > DECIMAL_MXSP00)
            {
                rc = errWrongDecimalLength_els98;
            }
            else
            {
                if (NeededLength > ValueLength)     // more than admissible count of digits?
                {
                    rc = errWrongFieldLength_els98;
                }
            }
            
            break;
        }
        case ioReal_els00:
        {
            // values must be 4 or 8 byte long which is check at parsing time --> nothing to do
            break;
        }
        case ioZoned_els00:
        {
            tsp00_Int4 lFracDiff = Column->colFormat_ls00.ffoFrac_ls00 - lFracPart;
            
            // reject if less fraction digits than defined in db (or computed)
            if (lFracDiff < 0)
            {
                rc = errFractionTooSmall_els98;
                NeededLength = FieldInfo->fbFraction_ls00;
                break;
            }
            else
            {   
                // consider also external fraction
                lFracPart = Column->colFormat_ls00.ffoFrac_ls00;
                lIntPart += lFracDiff;
            }

            NeededLength = ((lIntPart + lFracPart) / 2) + 1;
            if (NeededLength > ZONED_MXSP00)
            {
                rc = errWrongZonedLength_els98;
            }
            else
            {
                if (NeededLength > ValueLength)     // more than admissible count of digits?
                {
                    rc = errWrongFieldLength_els98;
                }
            }

            break;
        }
        default:
        {
            break;
        }
    }

    return rc;
}
// ls19_CheckFixLength()


//*********************** G L O B A L   F U N C T I O N S *********************

/*
  -----------------------------------------------------------------------------
  function:     ls19CheckFormatSpecLength
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckFormatSpecLength(tls00_FieldFormat *Format, tsp00_Int4 ValueLength)
{
    ROUTINE_DBG_MEO00 ("ls19CheckFormatSpecLength");

    tsp00_Int4 rc = errOK_els00;

    switch (Format->ffoFormat_ls00)
    {
        case ioInteger_els00:
        {
            if (false == Format->ffoIsHex_ls00)
            {
                if (  (3 == ValueLength) || (ValueLength > 4) )
                {
                    rc = errWrongIntegerLength_els98;
                }
            }
            else
            {
                if ( (2 != ValueLength) && (4 != ValueLength) && (8 != ValueLength) )
                {
                    rc = errWrongIntegerLength_els98;
                }
            }
            break;
        }
        case ioReal_els00:
        {
            if (false == Format->ffoIsHex_ls00)
            {
                if ( (4 != ValueLength) && (8 != ValueLength) )
                {
                    rc = errWrongRealLength_els98;
                }
            }
            else
            {
                if ( (8 != ValueLength) && (16 != ValueLength) )
                {
                    rc = errWrongRealLength_els98;
                }
            }
            break;
        }
        case ioDecimal_els00:
        {
            if (false == Format->ffoIsHex_ls00)
            {
                if (ValueLength > DECIMAL_MXSP00)
                {
                    rc = errWrongDecimalLength_els98;
                }
            }
            else
            {
                if ( (ValueLength > (DECIMAL_MXSP00 + DECIMAL_MXSP00)) ||
                     (0 != (ValueLength % 2)) )
                {
                    rc = errWrongDecimalLength_els98;
                }
            }
            break;
        }
        case ioZoned_els00:
        {
            if (false == Format->ffoIsHex_ls00)
            {
                if (ValueLength > ZONED_MXSP00)
                {
                    rc = errWrongZonedLength_els98;
                }
            }
            else
            {
                if ( (ValueLength > (ZONED_MXSP00 + ZONED_MXSP00)) ||
                     (0 != (ValueLength % 2)) )
                {
                    rc = errWrongZonedLength_els98;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return rc;
}
// ls19CheckFormatSpecLength()


/*
  -----------------------------------------------------------------------------
  function:     ls19CheckNILValLength
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckNILValLength(const tsp00_Int4    ValueLength,
                      tls00_Column       *Column,
                      tsp1_param_opt_set  ColumnMode,
                      tls00_FileFormat   *FileFormat,
                      tsp00_Int4         &NeededLength)
{
    ROUTINE_DBG_MEO00 ("ls19CheckNILValLength");

    tsp00_Int4 rc = errOK_els00;

    if ( (false == ColumnMode.includes(sp1ot_mandatory)) || 
         (true == ColumnMode.includes(sp1ot_default)) )
    {
        // check only if NULL is allowed
        NeededLength = FileFormat->ffoNSpecLen_ls00;
        tsp00_Addr pszNullVal = FileFormat->ffoNullSpec_ls00;
        if(0 != Column->colAssign_ls00.fasAsgnLen_ls00)
        {
            NeededLength = Column->colAssign_ls00.fasAsgnLen_ls00;
            pszNullVal = Column->colAssign_ls00.fasAsgnVal_ls00;
        }

        if (true == Column->colFormat_ls00.ffoIsHex_ls00)
        {
            NeededLength += NeededLength;
        }
        
        if (ValueLength < NeededLength)
        {
            rc = errNILValueTruncated_els98;
        }
    }

    return rc;
}
// ls19CheckNILValLength()


/*
  -----------------------------------------------------------------------------
   function:     ls19CheckNILValCompatibility
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckNILValCompatibility(tls00_Column       *Column,
                             tsp1_param_opt_set ColumnMode,
                             tls00_FileFormat   *FileFormat,
                             tsp00_Addr         pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls19CheckNILValCompatibility");

    tsp00_Int4 rc = errOK_els00;

    if ( (false == ColumnMode.includes(sp1ot_mandatory)) ||     // check only if NULL is allowed
         (true == ColumnMode.includes(sp1ot_default)) )
    {
        if (ioChar_els00 != Column->colFormat_ls00.ffoFormat_ls00)
        {
            if (asUnknown_els00 == Column->colAssign_ls00.fasAsgnTyp_ls00)
            {                                           // column not of type char but default NULL representation
                rc = errIncompatibleNullValue_els98;    // should be used --> error
            }
            else                        // Coming here a explicite NULL rep must have been specified
            {                           // column structure contains the value
                tsp00_SwapKind_Enum Swap;
                Swap = ls07DefineSwap(NULL);

                // We need to use a struct thats actually not used for dataextract but
                // expected by the called function
                Column->colNullCond_ls00 = new tls00_CondSpec;
                memset(Column->colNullCond_ls00, 0, sizeof(tls00_CondSpec));

                Column->colNullCond_ls00->cosCond_ls00 = new tls00_Condition;
                memset(Column->colNullCond_ls00->cosCond_ls00, 0, sizeof(tls00_Condition));

                tls00_Condition *myCond = Column->colNullCond_ls00->cosCond_ls00;

                // same values as for column definition
                myCond->conFormat_ls00  = Column->colFormat_ls00.ffoFormat_ls00;
                myCond->conFrac_ls00    = Column->colFormat_ls00.ffoFrac_ls00;
                myCond->conCompVal_ls00 = Column->colAssign_ls00.fasAsgnVal_ls00;
                myCond->conValLen_ls00  = Column->colAssign_ls00.fasAsgnLen_ls00;
                myCond->conStart_ls00   = Column->colFPos_ls00.fpoStart_ls00; 
                myCond->conEnd_ls00     = Column->colFPos_ls00.fpoEnd_ls00;

                rc = ls16ConvertCondition(*Column->colNullCond_ls00, Swap, *FileFormat, pszErrText);
                if (errOK_els00 == rc)
                {
                    if (ioChar_els00 != Column->colFormat_ls00.ffoFormat_ls00)
                    {
                        Column->colAssign_ls00.fasAsgnVal_ls00 = STATIC_CAST(char*, myCond->conBinCompVal_ls00);
                        Column->colAssign_ls00.fasAsgnLen_ls00 = myCond->conValLen_ls00;
                    }
                }
                else
                {
                    // error text is already logged
                    rc = errIncompatibleDataTypes_els98;
                }

                // delete unnecessary struct
                if (NULL != Column->colNullCond_ls00->cosCond_ls00)
                {
                    delete Column->colNullCond_ls00->cosCond_ls00;
                    Column->colNullCond_ls00->cosCond_ls00 = NULL;
                }
                if (NULL != Column->colNullCond_ls00)
                {
                    delete Column->colNullCond_ls00;
                    Column->colNullCond_ls00 = NULL;
                }
            }
        }
    }

    return rc;
}
// ls19CheckNILValCompatibility()


/*
  -----------------------------------------------------------------------------
  function:     ls19CalcExtLenFrac
  -----------------------------------------------------------------------------
*/
void
ls19CalcExtLenFrac(tls00_FieldBasics & FieldInfo,
                   tls00_FieldOption & ColOption,
                   tsp00_Int4          RecalcLenFrac[])
{
    ROUTINE_DBG_MEO00 ("ls19CalcExtLenFrac");

    tsp00_Int4 lFracPart = FieldInfo.fbFraction_ls00;
    tsp00_Int4 lIntPart  = FieldInfo.fbLength_ls00 - FieldInfo.fbFraction_ls00;

    if (csp_float_frac != FieldInfo.fbFraction_ls00)
    {
        if (true == ColOption.fopAnyOperation_ls00)      // any option specified
        {
            if (true == ColOption.fopScale_ls00)
            {
                lIntPart  += ColOption.fopScVal_ls00;
                lFracPart -= ColOption.fopScVal_ls00;

                if (lIntPart < 0)       // only if scale specified
                {
                    lIntPart = 0;
                }

                if (lFracPart < 0)
                {
                    lFracPart = 0;
                }
            }

            if (true == ColOption.fopRound_ls00)
            {
                lFracPart = ColOption.fopRTVal_ls00;
            }
            else
            {
                if (true == ColOption.fopTrunc_ls00)
                {
                    lFracPart = ColOption.fopTrunc_ls00;
                }
            }
        }
    }

    RecalcLenFrac[0] = lIntPart + lFracPart;    // new external length (complete)
    RecalcLenFrac[1] = lFracPart;               // new fraction

    if (RecalcLenFrac[0] > MAX_NUMERICLEN_LS00)
    {
        RecalcLenFrac[0] = MAX_NUMERICLEN_LS00;
        RecalcLenFrac[1] = csp_float_frac;
    }

    return;
}
// ls19CalcExtLenFrac()


/*
  -----------------------------------------------------------------------------
  function:     ls19CheckLengthForExtract
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckLengthForExtract(const tsp00_Int4   ValueLength,
                          tls00_Column      *Column,
                          tls00_FieldBasics *pFieldInfo,
                          tls00_FileFormat  *pFileFormat,
                          tsp00_Int4        &NeededLength,
                          tsp00_Int4         ExtLenFrac[])
{
    ROUTINE_DBG_MEO00 ("ls19CheckLengthForExtract");

    tsp00_Int4 rc = errOK_els00;

    NeededLength = pFieldInfo->fbLength_ls00;    // first assignment that may be corrected

    // Check date and time values (masks were generated at parsing time)
    switch (pFieldInfo->fbDataType_ls00)
    {
        case ddate:
        {
            if (dtFreeMask_els00 == pFileFormat->ffoDateType_ls00)
            {
                NeededLength = pFileFormat->ffoSpecialDateMask_ls00.mlen;
            }
            break;
        }
        case dtime:
        {
            if (dtFreeMask_els00 == pFileFormat->ffoTimeType_ls00)
            {
                NeededLength = pFileFormat->ffoSpecialTimeMask_ls00.mlen;
            }
            break;
        }
        case dtimestamp:
        {
            if (dtFreeMask_els00 == pFileFormat->ffoTimeStampType_ls00)
            {
                NeededLength = pFileFormat->ffoSpecialTimeStampMask_ls00.mlen;
            }
            break;
        }
        case dboolean:          // check bool values
        {
            NeededLength = (pFileFormat->ffoTValLen_ls00 >= pFileFormat->ffoFValLen_ls00) ?
                            pFileFormat->ffoTValLen_ls00 :
                            pFileFormat->ffoFValLen_ls00;
            break;
        }
        case dfixed:
        case dinteger:
        case dsmallint:
        case dfloat:
        {
            rc = ls19_CheckFixLength(ValueLength, pFieldInfo, Column, pFileFormat, NeededLength, ExtLenFrac);
            break;
        }
        case dstra:
        case dstre:
        case dstrb:
        case dstrdb:
        case dstruni:
        case dlonga:
        case dlonge:
        case dlongb:
        case dlongdb:
        case dlonguni:         // LONGs
        {
            if (0 == ValueLength)
            {
                // This happens only upon initialization for dataextract where no column
                // information is given; we assume here that position numbers may not exceed a
                // value of 10 digits: startpos-endpos
                NeededLength = 21;
            }
            else
            {
                NeededLength = ValueLength;
            }
            break;
        }
        case dchb:                              // BLOBS
        case dvarcharb:
        {
            // - if the file is of binary format and hex is not specified for the column the length
            //   of the value corresponds to the db length;
            // - if the file is of binary format and hex is specified for the column the length
            //   of the value is twice the db length;
            // - if th file is of text format (formatted or compressed) the length is twice the db length
            //   because we dump the values as readable hex strings; if additionally hex is specified this
            //   is ignored because already taken into account
            if ( (false == pFileFormat->ffoBinary_ls00) ||
                 ( (true == pFileFormat->ffoBinary_ls00) && (true == Column->colFormat_ls00.ffoIsHex_ls00) ) )
            {
                NeededLength += NeededLength;
            }
        }
        default:
        {
            break;              // all char types
        }
    }

    //*
    //*     Now we need to consider external encoding type and hexadecimal representation
    //*
    //* adapt length if hex output but datatype is not BLOB (for blob done in switch stmt)
    //*
    if (errOK_els00 == rc)
    {
        tsp00_Int4 factor = 1;
        bool       bHex   = ( (true == Column->colFormat_ls00.ffoIsHex_ls00) &&
                              (1 != szMap_BlobDataType_ls00[pFieldInfo->fbDataType_ls00]) );

        if ( true == ls00IsUCS2(&pFileFormat->ffoCodeType_ls00) )
        {
            if (true == bHex)
            {
                factor = 8;
            }
            else
            {
                factor = 2;
            }
        }
        else if ( (ctAscii_els00 == pFileFormat->ffoCodeType_ls00) || (ctUTF8_els00 == pFileFormat->ffoCodeType_ls00) )
        {
            if (true == bHex)
            {
                factor = 2;
            }
        }
    
        NeededLength *= factor;


        if (ValueLength < NeededLength)
        {
            rc = errWrongFieldLength_els98;
        }
    }

    return rc;
}
// ls19CheckLengthForExtract()


/*
  -----------------------------------------------------------------------------
  function:     ls19SortColumns
  -----------------------------------------------------------------------------
*/
void
ls19SortColumns(tls00_MultColSpec *MultCol)
{
    ROUTINE_DBG_MEO00 ("ls19SortColumns");

    int        i     = 0;
    tsp00_Int4 Count = MultCol->mcsCount_ls00;
    
    if (0 == Count)
    {
        return;
    }
    
    tsp00_Int4 (*SortArray)[2] = new tsp00_Int4[MultCol->mcsCount_ls00][2]; // 2-dim array

    for (i=0; i < Count; ++i)
    {
        SortArray[i][0] = MultCol->mcsColumn_ls00[i]->colFPos_ls00.fpoStart_ls00;
        SortArray[i][1] = i;
    }
    
    qsort((void *)SortArray, Count, sizeof(tsp00_Int4) + sizeof(tsp00_Int4), ls07IntCompare);

    tls00_Column *myNewColArray[csp_max_fields];
    for (i=0; i < Count; ++i)
    {
        myNewColArray[i] = MultCol->mcsColumn_ls00[SortArray[i][1]];
    }

    for (i=0; i < Count; ++i)
    {
        MultCol->mcsColumn_ls00[i] = NULL;      // alten pointer 'entfernen'
        MultCol->mcsColumn_ls00[i] = myNewColArray[i];
    }

    delete [] SortArray;

    return;
}
// ls19SortColumns()


/*
  -----------------------------------------------------------------------------
  function:     ls19CheckDataTypeCompatibility
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls19CheckDataTypeCompatibility(tls00_Column      *Column,
                               tls00_FieldBasics *FieldInfo)
{
    ROUTINE_DBG_MEO00 ("ls19CheckDataTypeCompatibility");

    tsp00_Int4 rc = errOK_els00;

    switch (Column->colFormat_ls00.ffoFormat_ls00)
    {
        case ioChar_els00:
        {
            break;      // any internal DATA type may be written as char
        }
        case ioInteger_els00:
        {
            if ( (dfixed    != FieldInfo->fbDataType_ls00) &&
                 (dsmallint != FieldInfo->fbDataType_ls00) &&
                 (dinteger  != FieldInfo->fbDataType_ls00) )
            {
                rc = errIncompatibleDataTypes_els98;
            }
            break;
        }
        case ioDecimal_els00:           // two times pass through
        case ioReal_els00:
        case ioZoned_els00:
        {
            if (0 == szMap_NumberDataType_ls00[FieldInfo->fbDataType_ls00])
            {
                rc = errIncompatibleDataTypes_els98;
            }
            break;
        }
        default:                        // oracle number and oracle date not used --> error
        {
            rc = errIncompatibleDataTypes_els98;
            break;
        }
    }

    return rc;
}
// ls19CheckDataTypeCompatibility()

