/*
  -----------------------------------------------------------------------------
  module: vls16_condition.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS

  special area: Replication Server
 
  description:  Implementation of condition checking and converting functions
                
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


#include "gls00.h"
#include "hls16_condition.h"
#include "hls05.h"
#include "hls07.h"
#include "hls18.h"
#include "hls25.h"
#include "hls98msg.h"
#include "hls99.h"


/*
  -----------------------------------------------------------------------------
 
  function:     ls16_EvaluateCondition

  -----------------------------------------------------------------------------

  description:  Evaluates a simple condition
                (for instance: checking POS 1-3 <> '===').
                Therefore the value is extracted from the DATA line and compared
                to the constant which has to be converted to the DATA type of the
                DATA.
                Data of type CHAR, ZONED and DECIMAL is compared lexicographically
                using memcmp. The strings have not to be of the same length! If two
                strings of different length are equal in the
                first n characters where n is the length of the shorter string the
                longer string is lexicographically greater then the shorter.
                Data of type INT and REAL is directly compared (i.e. 2 == 4 ).

  arguments:    pCondition   [IN]    - condition structure
                DataLine     [IN]    - DATA line where the DATA to be compared
                                        has to be extracted from.
                FileFormat   [IN]    - FileFormat structure to know
                                        how to extract the DATA

  returnvalue:  true        - condition is fullfilled
                false       - the opposite of the one when true is delivered
  -----------------------------------------------------------------------------
*/
static bool
ls16_EvaluateCondition(tls00_Condition  *pCondition,
                       tls00_String     DataLine,
                       tls00_FileFormat& FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls16_EvaluateCondition");

    bool        bLogicalData    = false;
    tsp00_Int4  lCompData       = 0;
    tsp00_Int4  lResult         = errOK_els00;
    
    tls00_String Value;
    Value.strAddr_ls00 = NULL;
    Value.strLeng_ls00 = 0;

    if (0 == pCondition->conEnd_ls00)   // read unformatted
    {
        lResult = ls18GetUnformattedValue(DataLine.strAddr_ls00,
                                          DataLine.strLeng_ls00,
                                          pCondition->conStart_ls00,
                                          FileFormat.ffoSeperat_ls00,
                                          FileFormat.ffoDelimit_ls00,
                                          &Value);
    }
    else                                // read formatted
    {
        lResult = ls18GetFormattedValue(DataLine.strAddr_ls00,
                                        DataLine.strLeng_ls00,
                                        pCondition->conStart_ls00,
                                        pCondition->conEnd_ls00,
                                        &Value);
    }

    //*
    //*     Compare values depending on there DATA type
    //*
    switch (pCondition->conFormat_ls00)
    {
        case ioChar_els00:
        {
            tsp00_Int4 lMinLength = Value.strLeng_ls00;
            if (Value.strLeng_ls00 != pCondition->conValLen_ls00)
            {
                lMinLength = (Value.strLeng_ls00 > pCondition->conValLen_ls00) ?
                             pCondition->conValLen_ls00                        :
                             Value.strLeng_ls00;
            }

            // result = strncmp(string1, string2, count)
            // result < 0 --> string1 < string2
            // result = 0 --> string1 = string2
            // result > 0 --> string1 > string2
            // comparison operator compares the following: value_from_data_file operator constant_value
            lCompData = memcmp(Value.strAddr_ls00, pCondition->conCompVal_ls00, lMinLength);
            
            // If both string have different length but they're equal in the first lMinLength bytes
            // they're compared by length:
            // String1_length > String2_length   -->   string1 > string2   -->   result > 0
            // String1_length < String2_length   -->   string1 < string2   -->   result < 0
            if ( (0 == lCompData) && (Value.strLeng_ls00 != pCondition->conValLen_ls00) )
            {
                if (Value.strLeng_ls00 > pCondition->conValLen_ls00)
                {
                    lCompData = 1;
                }
                else
                {
                    lCompData = -1;
                }
            }

            break;
        }
        case ioInteger_els00:
        {
            // We assume that the comparison constant has been converted to the type of 
            // values from DATA file before calling this function
            
            // actually we need to check if the length of Value.strAddr_ls00 is really 4 (or 2 or 1)

            tsp00_Int4 lValue     = *(REINTERPRET_CAST(tsp00_Int4*, Value.strAddr_ls00));
            tsp00_Int4 lCompValue = *(STATIC_CAST(tsp00_Int4*, pCondition->conBinCompVal_ls00));
            
            if (lValue < lCompValue)
            {
                lCompData =  -1;
            }
            else if (lValue > lCompValue)
            {
                lCompData = 1;
            }
            else
            {
                lCompData = 0;
            }
            
            break;
        }
        case ioReal_els00:
        {
            tsp00_Longreal lValue     = *(REINTERPRET_CAST(tsp00_Longreal*, Value.strAddr_ls00));
            tsp00_Longreal lCompValue = *(STATIC_CAST(tsp00_Longreal*, pCondition->conBinCompVal_ls00));

            if (lValue < lCompValue)
            {
                lCompData =  -1;
            }
            else if (lValue > lCompValue)
            {
                lCompData = 1;
            }
            else
            {
                lCompData = 0;
            }

            break;
        }
        case ioDecimal_els00:
        {
            // Compare values always using their maximum length; parser and/or converting function
            // must make sure that both are of equal length
            lCompData = memcmp(Value.strAddr_ls00,
                               (STATIC_CAST(tsp00_Decimalc*, pCondition->conBinCompVal_ls00))->asCharp(),
                               sizeof(tsp00_Decimalc));
            break;
        }
        case ioZoned_els00:
        {
            // Compare values always using their maximum length; parser and/or converting function
            // must make sure that both are of equal length
            lCompData = memcmp(Value.strAddr_ls00,
                               (STATIC_CAST(tsp00_Zonedc*, pCondition->conBinCompVal_ls00))->asCharp(),
                               sizeof(tsp00_Zonedc));
            break;
        }
    }


    switch (pCondition->conCompOp_ls00)
    {
        case opEqual_els00:
        {
            bLogicalData = (0 == lCompData) ? true : false;
            break;
        }
        case opSmaller_els00:
        {
            bLogicalData = (lCompData < 0) ? true : false;
            break;
        }
        case opLarger_els00:
        {
            bLogicalData = (lCompData > 0) ? true : false;
            break;
        }
        case opSmallerEqual_els00:
        {
            bLogicalData = ( (lCompData < 0) || (0 == lCompData) ) ? true : false;
            break;
        }
        case opLargerEqual_els00:
        {
            bLogicalData = ( (lCompData > 0) || (0 == lCompData) ) ? true : false;
            break;
        }
        case opNotEqual_els00:
        {
            bLogicalData = (0 != lCompData) ? true : false;
            break;
        }
    }

    return bLogicalData;
}
// ls16_EvaluateCondition()


/*
  -----------------------------------------------------------------------------
  function:     ls16CheckCondition
  -----------------------------------------------------------------------------
*/
bool
ls16CheckCondition(tls00_CondSpec&   ConditionSpec,
                   tls00_String      DataLine,
                   tls00_FileFormat& FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls16CheckCondition");
    
    if (NULL == ConditionSpec.cosCond_ls00)
    {
        // In case of a single NOT-condition the right node may be NULL - that has
        // to be catched
        ls16CheckCondition(*ConditionSpec.cosLeft_ls00, DataLine, FileFormat);
        
        if (NULL != ConditionSpec.cosRight_ls00)
        {
            ls16CheckCondition(*ConditionSpec.cosRight_ls00, DataLine, FileFormat);
        }

        switch (ConditionSpec.cosBoolOp_ls00)
        {
            case boNOT_els00:
            {
                ConditionSpec.cosCondValue_ls00 = 
                    !ConditionSpec.cosLeft_ls00->cosCondValue_ls00;
                break;
            }
            case boAND_els00:
            {
                ConditionSpec.cosCondValue_ls00 = 
                    ( ConditionSpec.cosRight_ls00->cosCondValue_ls00 &&
                      ConditionSpec.cosLeft_ls00->cosCondValue_ls00 );
                break;
            }
            case boOR_els00:
            {
                ConditionSpec.cosCondValue_ls00 = 
                    ( ConditionSpec.cosRight_ls00->cosCondValue_ls00 ||
                      ConditionSpec.cosLeft_ls00->cosCondValue_ls00 );
                break;
            }    
        }
    }
    else
    {
        ConditionSpec.cosCondValue_ls00 = ls16_EvaluateCondition(ConditionSpec.cosCond_ls00,
                                                                 DataLine,
                                                                 FileFormat);
    }

    return ConditionSpec.cosCondValue_ls00;
}
// ls16CheckCondition()


/*
  -----------------------------------------------------------------------------
  function:     ls16ConvertCondition
  -----------------------------------------------------------------------------
  description:  Recursive function to convert comparison constants in a complex
                condition to the DATA type of the DATA.
                The possible DATA types are CHAR (no conversion), DECIMAL, INTEGER,
                ZONED and REAL.
                Conversion takes place for numerical values!
                To convert the comparison constant first the constant is
                converted to float and then REconverted to the DATA type of
                the DATA.

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
ls16ConvertCondition(tls00_CondSpec      &ConditionSpec,
                     tsp00_SwapKind_Enum  Swap,
                     tls00_FileFormat    &FileFormat,
                     tsp00_Addr           pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls16ConvertCondition");

    tsp00_Int4 lResult = errOK_els00;
    
    // If pointer cosCond_ls00 in ConditionSpec = NULL the pointer cosLeft_ls00 and
    // cosRight_ls00 can be != NULL but they don't need to. Check it for every call.
    if (NULL == ConditionSpec.cosCond_ls00)
    {
        if (NULL != ConditionSpec.cosLeft_ls00)
        {
            lResult = ls16ConvertCondition(*ConditionSpec.cosLeft_ls00,
                                           Swap,
                                           FileFormat,
                                           pszErrText);
        }
        
        if ( (NULL != ConditionSpec.cosRight_ls00) && (errOK_els00 == lResult) )
        {
            lResult = ls16ConvertCondition(*ConditionSpec.cosRight_ls00,
                                           Swap,
                                           FileFormat,
                                           pszErrText);
        }
    }
    else
    {
        if (true == ConditionSpec.cosDefNull_ls00)
        {
            // cosDefNull_ls00 is then and only than true if the ConditionSpec is not NULL;
            // additionally cosDefNull_ls00 may only be defined in the first condition spec
            // of every column DATA structure
            tls00_Condition* pNC = ConditionSpec.cosCond_ls00;

            //*
            //* Allocate memory for NULL specification value depending on
            //* encoding type and hex representation
            //*
            tsp00_Int4 _alloc_len = 2*FileFormat.ffoNSpecLen_ls00;
            if (true == pNC->conIsHex_ls00)
            {
                if ( (ctUCS2_els00        == FileFormat.ffoCodeType_ls00) ||
                     (ctUCS2Swapped_els00 == FileFormat.ffoCodeType_ls00) ||
                     (ctUCS2Native_els00  == FileFormat.ffoCodeType_ls00)  )
                {
                    _alloc_len *= 2;
                }
            }   // end if (true == pNC->conIsHex_ls00)


            // In case DATA file encoding is UCS2 in any form we need to
            // transform the DEFAULT NULL value representation to UCS2, too.
            if ( (ctUCS2_els00        == FileFormat.ffoCodeType_ls00) ||
                 (ctUCS2Swapped_els00 == FileFormat.ffoCodeType_ls00) ||
                 (ctUCS2Native_els00  == FileFormat.ffoCodeType_ls00)  )
            {
                pNC->conCompVal_ls00 = new char[_alloc_len + 2];

                char* pszTmp = FileFormat.ffoNullSpec_ls00;
                pNC->conValLen_ls00 = FileFormat.ffoNSpecLen_ls00;

                if (true == pNC->conIsHex_ls00)
                {
                    pszTmp = new char[2*FileFormat.ffoNSpecLen_ls00 + 2];
                    ls07ConvertByteToHexEx(FileFormat.ffoNullSpec_ls00, pNC->conValLen_ls00, pszTmp);
                    pNC->conValLen_ls00 *= 2;;
                }
                
                tsp00_Longuint len = _alloc_len;     // available space in pNC->conCompVal_ls00
                lResult =
                    ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, pNC->conCompVal_ls00),
                                            len,
                                            FileFormat.ffoCodeType_ls00,
                                            REINTERPRET_CAST(unsigned char*, pszTmp),
                                            pNC->conValLen_ls00,
                                            ctAscii_els00,
                                            pszErrText);
                pNC->conValLen_ls00 = STATIC_CAST(tsp00_Int4, len);

                if ( (0 != pszTmp) && (pszTmp != FileFormat.ffoNullSpec_ls00) )
                {
                    delete [] pszTmp;
                    pszTmp = NULL;
                }
            }
            else
            {
                if (true == pNC->conIsHex_ls00)
                {
                    pNC->conCompVal_ls00 = new char[_alloc_len + 1];
                    memset(pNC->conCompVal_ls00, 0, (pNC->conValLen_ls00 + 1));
                    ls07ConvertByteToHexEx(FileFormat.ffoNullSpec_ls00, FileFormat.ffoNSpecLen_ls00, pNC->conCompVal_ls00);
                    pNC->conValLen_ls00 = 2*FileFormat.ffoNSpecLen_ls00;
                }
                else
                {
                    pNC->conCompVal_ls00 = FileFormat.ffoNullSpec_ls00;
                    pNC->conValLen_ls00  = FileFormat.ffoNSpecLen_ls00;
                }
            }   // end else of if ( (ctUCS2_els00 == FileFormat.ffoCodeType_ls00) ||...
        }   // if (true == ConditionSpec.cosDefNull_ls00)
        else
        {
            if (ioChar_els00 != ConditionSpec.cosCond_ls00->conFormat_ls00)
            {
                tls00_Column      myColumn;         // build column struct and fieldbasic struct
                tls00_FieldBasics myFieldBasics;

                // Insert only necessary values into built structs
                myColumn.colFormat_ls00.ffoFormat_ls00 = ioChar_els00;
                myColumn.colFormat_ls00.ffoFrac_ls00   = ConditionSpec.cosCond_ls00->conFrac_ls00;
                myColumn.colOption_ls00.fopAnyOperation_ls00 = false;      // no round, trunc, scale possible

                myFieldBasics.fbDataType_ls00.becomes(dfloat);
                myFieldBasics.fbFraction_ls00 = 0;
                myFieldBasics.fbLength_ls00   = MAX_NUMERICLEN_LS00;
                myFieldBasics.fbInOutLength_ls00 = MAX_NUMERICLEN_LS00/2 + 1;   // length in bytes

                // The next pointer is created only because function ls25PrepareNumbers in case of
                // successful conversion throws away the 'old' pointer without freeing the allocated space.
                // --> myTmpStr prevents a memory leak
                tsp00_Addr myTmpStr = ConditionSpec.cosCond_ls00->conCompVal_ls00;

                lResult = ls25PrepareNumbers(&ConditionSpec.cosCond_ls00->conCompVal_ls00,
                                            ConditionSpec.cosCond_ls00->conValLen_ls00,
                                            myColumn,
                                            myFieldBasics,
                                            Swap,
                                            FileFormat);

                if (errOK_els00 == lResult)
                {
                    tsp00_NumError RetValue;
                    RetValue.becomes(num_ok);

                    delete [] myTmpStr;
                    myTmpStr = NULL;

                    // This computed length is the length of the output column. It should have
                    // a comparison and a NULL value
                    tsp00_Int4 lmyValueLength = 0;
                    if (0 == ConditionSpec.cosCond_ls00->conEnd_ls00)       // in case compressed
                    {
                        lmyValueLength = MAX_NUMERICLEN_LS00/2 + 1;
                    }
                    else                                                    // in case formatted
                    {
                        lmyValueLength = ConditionSpec.cosCond_ls00->conEnd_ls00 -
                                                (ConditionSpec.cosCond_ls00->conStart_ls00 - 1);
                    }

                    switch (ConditionSpec.cosCond_ls00->conFormat_ls00)     // convert internal value to requested DATA type
                    {
                        case ioInteger_els00:
                        {
                            tsp00_Int4 lDest;

                            s40glint(ConditionSpec.cosCond_ls00->conCompVal_ls00,
                                    1,
                                    MAX_NUMERICLEN_LS00,
                                    lDest,
                                    RetValue);
                            if (num_ok == RetValue)
                            {
                                ConditionSpec.cosCond_ls00->conBinCompVal_ls00 = new char[sizeof(tsp00_Int4)];

                                ls07Swap4BIntToChar(lDest,
                                                    Swap, 
                                                    FileFormat.ffoIntSpec_ls00,
                                                    STATIC_CAST(char*, ConditionSpec.cosCond_ls00->conBinCompVal_ls00) );

                                ConditionSpec.cosCond_ls00->conValLen_ls00 = sizeof(tsp00_Int4);
                            }
                            break;
                        }
                        case ioDecimal_els00:
                        {
                            tsp00_Decimalc lDest;

                            s40gdec(ConditionSpec.cosCond_ls00->conCompVal_ls00,
                                    1,
                                    MAX_NUMERICLEN_LS00,
                                    lDest,
                                    (lmyValueLength * 2) + 1,
                                    ConditionSpec.cosCond_ls00->conFrac_ls00,
                                    RetValue);
                            if (num_ok == RetValue)
                            {
                                ConditionSpec.cosCond_ls00->conBinCompVal_ls00 = new char[lmyValueLength];

                                SAPDB_memcpy(ConditionSpec.cosCond_ls00->conBinCompVal_ls00, lDest.asCharp(), lmyValueLength);
                                ConditionSpec.cosCond_ls00->conValLen_ls00 = lmyValueLength;
                            }
                            break;
                        }
                        case ioZoned_els00:
                        {
                            tsp00_Zonedc lDest;
                            ConditionSpec.cosCond_ls00->conValLen_ls00 = MAX_NUMERICLEN_LS00/2 + 1;

                            s40gzone(ConditionSpec.cosCond_ls00->conCompVal_ls00,
                                    1,
                                    MAX_NUMERICLEN_LS00,
                                    lDest,
                                    (lmyValueLength * 2) + 1,
                                    ConditionSpec.cosCond_ls00->conFrac_ls00,
                                    RetValue);
                            if (num_ok == RetValue)
                            {
                                ConditionSpec.cosCond_ls00->conBinCompVal_ls00 = new char[lmyValueLength];

                                SAPDB_memcpy(ConditionSpec.cosCond_ls00->conBinCompVal_ls00, lDest.asCharp(), lmyValueLength);
                                ConditionSpec.cosCond_ls00->conValLen_ls00 = lmyValueLength;
                            }
                            break;
                        }
                        case ioReal_els00:
                        {
                            tsp00_Longreal lDest;

                            s40glrel(ConditionSpec.cosCond_ls00->conCompVal_ls00,
                                    1,
                                    MAX_NUMERICLEN_LS00,
                                    lDest,
                                    RetValue);
                            if (num_ok == RetValue)
                            {
                                ConditionSpec.cosCond_ls00->conBinCompVal_ls00 = new char[sizeof(tsp00_Longreal)];

                                SAPDB_memcpy(ConditionSpec.cosCond_ls00->conBinCompVal_ls00,
                                             REINTERPRET_CAST(char*, &lDest),
                                             sizeof(tsp00_Longreal));
                                ConditionSpec.cosCond_ls00->conValLen_ls00 = sizeof(tsp00_Longreal);
                            }
                            break;
                        }
                        default:
                        {
                            RetValue.becomes(num_invalid);
                        }
                    }

                    if (num_ok == RetValue)
                    {
                        lResult = errOK_els00;
                    }
                    else if (num_trunc == RetValue)
                    {
                        lResult = errTruncatedConditionValue_els98;
                    }
                    else
                    {
                        lResult = errInvalidNumCondition_els98;
                    }
                }   // end if (errOK_els00 == lResult) = ls25PrepareNumbers(...
            }   // end if (ioChar_els00 != ConditionSpec.cosCond_ls00->conFormat_ls00)
        }   // end else of if (true == ConditionSpec.cosDefNull_ls00)
        
        if (errOK_els00 != lResult)
        {
            if (errInvalidNumCondition_els98 == lResult)
            {
                LoaderMessages::Instance()->ls98Msg(layCondition_els98, pszErrText,
                                                     errInvalidNumCondition_els98,
                                                     ConditionSpec.cosCond_ls00->conCompVal_ls00);
            }
            else
            {
                LoaderMessages::Instance()->ls98Msg(layCondition_els98, pszErrText,
                                                     errTruncatedConditionValue_els98,
                                                     ConditionSpec.cosCond_ls00->conCompVal_ls00);
            }
        }
    }
    
    return lResult;
}
// ls16ConvertCondition()
