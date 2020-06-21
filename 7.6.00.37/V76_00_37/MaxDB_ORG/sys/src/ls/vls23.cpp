/*! 
  -----------------------------------------------------------------------------
  module: vls23.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: Replication Manager
 
  description:  Build single database record
                
  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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
#include "hls98msg.h"

#include "hls05.h"
#include "hls07.h"      // transformationsroutinen
#include "hls18.h"
#include "hls20.h"
#include "hls23.h"
#include "hls24.h"
#include "hls25.h"
#include "hls30.h"
#include "hls99.h"
#include "hsp30.h"


#define _INTERNAL_FILE_ID_    _T("23")

/*
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration 
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls23_BuildDefault - local function
  -----------------------------------------------------------------------------

  description:  Assigns default or NULL values to a buffer depending on the
                data type and column type.
  
  arguments:    pszDestination  [OUT]  - Buffer to fill
                lDestLen        [OUT]  - Buffer length to be returned
                pFieldInfo      [IN]   - structure with all info on the column
                pDefaultStruct  [IN]   - Structure with info on user and user group
                pszOtherDefault [IN]   - User defined default value to be inserted
                DefaultOrNull   [IN]   - Flags if the value should be a NULL or
                                          a default

  returnvalue:  errOK_els00             - in case of NO errors
                errMissingData_els98    - no data in datafile for column specified in
                                          load command
                errNoDataForNNCol_els98 - no data for not null column
                errUnknownDefault_els98 - kind of default is unknown
  -----------------------------------------------------------------------------
*/
static int
ls23_BuildDefault(tsp00_Addr           pszDestination,
                  tsp00_Longint       &lDestLen,
                  tls00_FieldInfo     *pFieldInfo,
                  tls00_PredefDefault *pDefaultStruct,
                  tls00_String        *pszOtherDefault,
                  tsp00_Int4           bDefaultOrNull);

/*
  -----------------------------------------------------------------------------
  function:     ls23_FillValue - local function
  -----------------------------------------------------------------------------

  description:  Fills prepared values into page buffer. Converts to UCS2 if
                necessary and adjusts the length of the values depending on their
                length type in the db.
  
  arguments:    pszDestination  [OUT] - Buffer to fill
                lBufLength      [OUT] - Buffer length to be returned after filling
                pSrcData        [IN]  - string structure with source data
                SrcDataEncoding [IN]  - encoding of source data
                pCol            [IN]  - structure with all info on the column
                pFLRec          [IN]  - structure with all info needed for fastload
                lValIndex       [IN]  - index of value to fill
                pszErrText      [OUT] - error text

  returnvalue:  errOK_els00             - in case of NO errors
                
                Errors when inserting defaults
                errMissingData_els98    - no data in datafile for column specified in
                                          load command
                errNoDataForNNCol_els98 - no data for not null column
                errInternal_els98       - the kind of default is unknown
                
                Errors when inserting assingments
                errIncompatibleConst_els98  - Error: value and column type are not compatible
                errUnknownAssignment_els98  - unbekanntes assignment

                Errors when converting to UCS2
                errConvertingEncoding_els98 - error converting
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls23_FillValue(tsp00_Addr      pszDestination,
               tsp00_Longint  &lDestLen,
               tls00_String   *pSrcData,
               tls00_CodeType  SrcDataEncoding,
               tls00_Column   *pCol,
               tls00_FloadRec *pFLRec,
               tsp00_Int4      lValIndex,
               tsp00_Addr      pszErrText);



/*
  -----------------------------------------------------------------------------
  function:     ls23_CheckForNULLValue - local function
  -----------------------------------------------------------------------------

  description:  Function to convert values to NULL values if necessary.
  
  arguments:    

  returnvalue:  errOK_els00             - in case of NO errors
                
                errNULLNotAllowed_els98
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls23_CheckForNULLValue(tls00_String*      pRawData,
                       tls00_String*      pInputLine,
                       tls00_Column*      pColumn,
                       tls00_FloadRec*    pFLRec,
                       tls00_FileFormat*  pFormat,
                       SAPDB_UInt4        FieldIndex);


/*
  -----------------------------------------------------------------------------
  function:     ls23_ConvertValues2UnicodeOnPage - local function
  -----------------------------------------------------------------------------

  description:  Converts values to UCS2 in case of UNICODE database.
  
  arguments:    pszDestination  [OUT]    - Buffer to fill
                lDestLen        [IN/OUT] - Buffer length to be returned after filling
                pFieldInfo      [IN]     - 
                SrcDataEncoding [IN]     - encoding of source data
                SwapKind        [IN]     - 
                pszErrText      [OUT]    - error text

  returnvalue:  errOK_els00             - in case of NO errors
                
                Errors when converting to UCS2
                errConvertingEncoding_els98 - error converting
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls23_ConvertValues2UnicodeOnPage(tsp00_Addr           pszDestination,
                                 tsp00_Longint&       lDestLen,
                                 tls00_FieldInfo*     pFieldInfo,
                                 tls00_CodeType       SrcDataEncoding,
                                 tsp00_SwapKind_Enum  SwapKind,
                                 tsp00_Addr           pszErrText);


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
   function:     ls23BuildRecord
  -----------------------------------------------------------------------------
*/
int ls23BuildRecord(tsp00_Addr               Buffer, 
                    tls00_TransformationCmd  *pTransformationCmd,
                    tls00_FloadRec           *pFLRec,
                    tsp1_sqlmode_Enum        SQLMode,
                    tsp00_Addr               ErrText)
{
   ROUTINE_DBG_MEO00 ("ls23BuildRecord");

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // error reporting obj
    tsp00_Longint    rc      = errOK_els00;
    
    tls00_TableDescription *pTI = &pFLRec->flrTableInfo_ls00;
    tls00_FieldInfo        *pFI = NULL;
    tls00_FileFormat       &Format      = pTransformationCmd->trDataFile_ls00.fsFormat_ls00;
    tls00_CodeType          ExtEncoding = Format.ffoCodeType_ls00;
    tls00_MultColSpec      *MultColumns = &pTransformationCmd->trColSpec_ls00;
    tls00_String           *RawData     = pFLRec->flrDataSource_ls00.dasConvertedData;
    tls00_Column           *pCol        = NULL;

    tls00_String            InputLine;
    InputLine.strAddr_ls00 = STATIC_CAST(char*, pFLRec->flrDataSource_ls00.dasInputBuffer.pBufPtr_ls00);
    InputLine.strLeng_ls00 = pFLRec->flrDataSource_ls00.dasInputBuffer.lBufPos_ls00;

    int           j          = 0;
    tsp00_Int4    DataIndex  = 0;
    tsp00_Uint4   FieldIndex = 0;
    bool          bHexValue  = false;
    tsp00_Longint lTrimLen   = 0;

    //*
    //*     First read data from data file
    //*
    tsp00_Int4 lFailIndex = 0;
    if (true == Format.ffoFormatted_ls00)           // read formatted data
    {
        rc = ls18GetFormattedValueEx(InputLine.strAddr_ls00,
                                     InputLine.strLeng_ls00,
                                     &pTransformationCmd->trColSpec_ls00,
                                     pFLRec->flrDataSource_ls00.lReadFieldsCnt_ls00,
                                     pFLRec->flrDataSource_ls00.plDataToColIndex_ls00,
                                     RawData,
                                     lFailIndex);
    }
    else
    {
        rc = ls18GetCSVData(InputLine.strAddr_ls00,
                            InputLine.strLeng_ls00,
                            &pFLRec->flrPattern_ls00,
                            pFLRec->flrDataSource_ls00.lReadFieldsCnt_ls00,
                            pFLRec->flrDataSource_ls00.lReadPos_ls00,
                            ExtEncoding,
                            RawData,
                            lFailIndex);
    }
    if (errOK_els00 != rc)
    {
        if (errMissingData_els98 == rc)
        {
            tsp00_Int4 lIndex = pFLRec->flrDataSource_ls00.plDataToParamIndex_ls00[lFailIndex];
            pMsgObj->ls98Msg(layFastload_els98, ErrText, errMissingData_els98,
                                                         pTI->tdColName_ls00[lIndex]->asCharp() );
        }
    }


    tls00_InOutType InOutType = ioChar_els00;       // needed for conversion to internal db encoding
    //*
    //*     After reading data into special structure process this data
    //*
    tsp00_Uint4 i = 0;
    for (i=0; ( (errOK_els00 == rc) && (i < pFLRec->flrDataSource_ls00.lReadFieldsCnt_ls00) ); ++i)
    {
        FieldIndex = pFLRec->flrDataSource_ls00.plDataToParamIndex_ls00[i];
        pFI        = pTI->tdFieldsArray_ls00[FieldIndex];

        if (0 != MultColumns->mcsCount_ls00)
        {
            pCol      = MultColumns->mcsColumn_ls00[pFI->fiStmtColNo_ls00 - 1];
            InOutType = pCol->colFormat_ls00.ffoFormat_ls00;
        }
        else
        {
            pCol      = NULL;
            InOutType = ioChar_els00;
        }

        bHexValue = false;

        //*
        //*     Specified value in data file is not empty.
        //*
        if (INSERT_EMPTY_VALUE != RawData[i].strLeng_ls00)
        {
            if ( (true         == Format.ffoCompress_ls00) &&
                 (ioChar_els00 == InOutType)               &&
                 (1            == szMap_CharDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) &&
                 (1            != szMap_BlobDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00])  )
            {
                ls18UndoubleDelimitersInCharData(&RawData[i], &pFLRec->flrPattern_ls00, ExtEncoding);
            }

            //*
            //*     Trim values right before transformation - remove trailing blanks
            //*
            //* This has to be done for FORMAT formatted only because then the values
            //* might be padded with blanks upon generating the data
            //*
            if (true == Format.ffoFormatted_ls00)
            {
                // - in case pCol is NULL no columns are specified in the command and thus all
                //   data can only be text data
                // - in case pCol is not NULL have to check if the data is text data
                if ( (NULL == pCol) ||
                     ( (NULL != pCol) && (ioChar_els00 == pCol->colFormat_ls00.ffoFormat_ls00) ) )
                {
                    lTrimLen  = RawData[i].strLeng_ls00;
                    ls05StrTrimRight(RawData[i].strAddr_ls00, lTrimLen, BLANK_LS00, ExtEncoding);
                    RawData[i].strLeng_ls00 = STATIC_CAST(tsp00_Int4, lTrimLen);
                }
            }

            //*
            //*     Check for NULL value representation
            //* - NULL value representation may only be indicated if there are columns specified
            //*   in the FASTLOAD command
            //*
            if ( (NULL != pCol) || (SAPDB_TRUE == pFLRec->flrNullValueCounter_ls00.nvcCount_ls00) )
            {
                rc = ls23_CheckForNULLValue(&RawData[i], &InputLine, pCol, pFLRec, &Format, FieldIndex);
                if (errNullNotAllowed_els98 == rc)
                {
                    pMsgObj->ls98Msg(layGRec_els98, ErrText, errNullNotAllowed_els98,
                                                             pTI->tdColName_ls00[FieldIndex]->asCharp());
                }
                else
                {
                    // In case the NULL-check went without problems we have anyway 2 cases to distinguish:
                    // 1. there is a null-value --> continue with the main for loop
                    // 2. there is a real value
                    if (INSERT_NULL == RawData[i].strLeng_ls00)
                    {
                        continue;
                    }
                }
            }   // end if (NULL != pCol) ...

            //*
            //*     Convert values to internal encoding (ASCII) for further processing
            //*
            if ( (ctAscii_els00 != ExtEncoding) && (ioChar_els00 == InOutType) )
            {
                rc = ls20ConvertValue2InternEncoding(RawData[i],
                                                     pFI->fiBasics_ls00.fbDataType_ls00,
                                                     ExtEncoding,
                                                     ErrText);
                if (errOK_els00 != rc)
                {
                    break;
                }
            }

            //*
            //*     Check for hex values
            //* - HEX value indication is possible only if there are columns specified in the
            //*   FASTLOAD command (example FASTLOAD TABLE <name> <col name> 1 HEX);
            //*   but HEX values are requested also if the column to be loaded is of sql blob type
            //*   which may be the case if no column is specified in the FASTLOAD command, too
            //*

            // (data in HEX form) or (BLOB data type and file format is not binary)?
            if (NULL != pCol)       // this check is only possible if pCol is not NULL
            {
                if (true == pCol->colFormat_ls00.ffoIsHex_ls00)
                {
                    bHexValue = true;
                }
            }
            if ( (true == bHexValue) ||
                 ( (1     == szMap_BlobDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) &&
                   (0     == szMap_LongDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) &&
                   (false == Format.ffoBinary_ls00) ) )
            {                                                       // -->  umwandeln in byte
                tsp00_Addr  pNewValue    = NULL;
                tsp00_Int4  lNewValueLen = 0;

                rc = ls07ConvertHexToByte(RawData[i].strAddr_ls00, RawData[i].strLeng_ls00,
                                          &pNewValue,              lNewValueLen);
                if (errOK_els00 != rc)
                {
                    pMsgObj->ls98Msg(layGRec_els98, ErrText, STATIC_CAST(ErrorNumbers,rc),
                                                             RawData[i].strLeng_ls00, RawData[i].strAddr_ls00);
                }
                else
                {
                    RawData[i].strAddr_ls00 = pNewValue;
                    RawData[i].strLeng_ls00 = lNewValueLen;
                }
            }

            //*
            //*     Check for
            //*  - special constants like date, time, timestamp, boolean
            //*  - NUMBER values (convert to internal db number representation)
            //*
            if ( (errOK_els00 == rc) && (1 == szMap_NumberDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) )
            {
                // KRUECKE FUER UNSAUBERE SCHNITTSTELLE
                tls00_Column myTmpCol;
                if (0 == MultColumns->mcsCount_ls00)
                {
                    memset(&myTmpCol, 0, sizeof(tls00_Column));
                    pCol = &myTmpCol;
                }

                rc = ls25PrepareNumbers(&RawData[i].strAddr_ls00, RawData[i].strLeng_ls00,
                                        *pCol,
                                        pFI->fiBasics_ls00,
                                        pFLRec->flrSwapKind_ls00,
                                        Format);
                if (errOK_els00 != rc)
                {
                    if (errNumberTruncated_els98 != rc)
                    {
                        pMsgObj->ls98Msg(layGRec_els98, ErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                 RawData[i].strLeng_ls00, RawData[i].strAddr_ls00);
                    }
                }
            }
            else if ( (errOK_els00 == rc) && (1 == szMap_SpecConstDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) )
            {
                rc = ls24SpecialFormat(RawData[i], pFI->fiBasics_ls00, Format);
                if (errOK_els00 != rc)
                {
                    // error msg for errInvalidBool_els98 and errWrongDateTime_els98 is similar
                    pMsgObj->ls98Msg(layGRec_els98, ErrText, STATIC_CAST(ErrorNumbers, rc),
                                                             RawData[i].strLeng_ls00, RawData[i].strAddr_ls00);
                }
            }
        }
        else    // of if (INSERT_EMPTY_VALUE != RawData[i].strLeng_ls00)
        {
            //*
            //*     Specified value in data file is either
            //*      - empty (which simply means a value of "") or
            //*      - there is no value but the column is specified in the FASTLOAD command or
            //*      - there is no value because the column is not specified in the FASTLOAD command.
            //*
            //*     In the last case we have to switch the length value from INSERT_EMPTY_VALUE
            //*     to INSERT_DEFAULT.
            //*
            // No data found in data file for column specified in command or no column at all specified in command
            if ( (NULL != pCol) || (SAPDB_TRUE == pFLRec->flrNullValueCounter_ls00.nvcCount_ls00) )
            {
                rc = ls23_CheckForNULLValue(&RawData[i], &InputLine, pCol, pFLRec, &Format, FieldIndex);
            }

            // NULL-value check may reveal that the read value does not represent the NULL value;
            // in this case the length remains unchanged (INSERT_EMPTY_VALUE)
            if (INSERT_EMPTY_VALUE == RawData[i].strLeng_ls00)
            {
                // Check if column is specified in the FASTLOAD command; if not the
                // value to be inserted is signaled by setting the length to INSERT_DEFAULT
                if (0 == pFI->fiStmtColNo_ls00)
                {
                    RawData[i].strLeng_ls00 = INSERT_DEFAULT;
                }
                else
                {
                    // nothing read and null condition not fullfilled; check if data is of data type char
                    if ( (1 == szMap_CharDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) ||
                         (1 == szMap_BlobDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) )
                    {
                        if (sp1sm_oracle == SQLMode)
                        {
                            // if sqlmode = oracle an empty field means to insert NULL; check if it is defined
                            if (mod_optional == pFI->fiColMode_ls00)
                            {
                                RawData[i].strLeng_ls00 = INSERT_NULL;
                            }
                            else
                            {
                                rc = errNullNotAllowed_els98;
                            }
                        }
                        else
                        {
                            // Ok, empty field means to insert BLANKs; we'll set the length to 0 and
                            // when building the page the value is either filled to the needed length
                            // (fixed long) or nothing else to do (variable long)
                            RawData[i].strLeng_ls00 = 0;
                        }

                    }
                    else
                    {
                        rc = errMissingData_els98;
                    }
                }
            }

            if (errOK_els00 != rc)
            {
                pMsgObj->ls98Msg(layDataLoad_els98, ErrText, STATIC_CAST(ErrorNumbers, rc),
                                                             pTI->tdColName_ls00[FieldIndex]->asCharp() );
                break;
            }

        }   // end else of if (INSERT_EMPTY_VALUE != RawData[i].strLeng_ls00)


        //*
        //*     Check the value length. 
        //* Length of numeric values is checked twice because the first time check is done
        //* when converting to internal number format; shouldn't be a problem
        //*
        if ( (errOK_els00 == rc) &&
             (INSERT_DEFAULT != RawData[i].strLeng_ls00) &&
             (INSERT_NULL != RawData[i].strLeng_ls00) &&
             (0 == szMap_NumberDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) )
        {
            tsp00_Uint4 _len = RawData[i].strLeng_ls00;

            if ( (1 == szMap_CharDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) && (ctAscii_els00 != ExtEncoding) )
            {
                if (ctUTF8_els00 != ExtEncoding)
                {
                    _len /= 2;
                }
                else
                {
                    unsigned int charLength    = 0;
                    unsigned int byteLength    = 0;
                    int          isTerminated  = 0;
                    int          isCorrupted   = 0;
                    int          isExhausted   = 0;

                    sp77encodingUTF8->stringInfo(RawData[i].strAddr_ls00,
                                                 _len,
                                                 1,
                                                 &_len,
                                                 &byteLength,
                                                 &isTerminated,
                                                 &isCorrupted,
                                                 &isExhausted);
                }
            }
            rc = ls18CheckValueLength(_len, pFI->fiBasics_ls00);            
            if (rc != errOK_els00)
            {
                // error value may only be: errIncompatibleConst_els98
                pMsgObj->ls98Msg(layGRec_els98, ErrText, STATIC_CAST(ErrorNumbers, rc),
                                                         pTI->tdColName_ls00[FieldIndex]->asCharp());
            }
        }
        
    }   // end for (i=0; ( (errOK_els00 == rc) && (i < pFLRec->flrDataSource_ls00.lReadFieldsCnt_ls00) ); ++i)


    // No problems so far?!
    if (errOK_els00 != rc)
    {
        return STATIC_CAST(tsp00_Int4, rc);
    }


    //*
    //*     Fill the db record buffer.
    //*
    //* Calling function must assure that the buffer has the right length
    //* 
    //* If the column was given in the load command AND was found in the file RawData.strLeng_ls00
    //* contains the length of the data found and RawData.strAddr_ls00 points to the data.
    //* If the row was given in the command BUT no data found in the file RawData.strLeng_ls00=-1 and
    //* RawData.strAddr_ls00=null pointer or something else which should not be evaluated.
    //* If the row was NOT given in the command RawData.strLeng_ls00=0 and RawData.strAddr_ls00=null pointer.
    //*

    //* Recordlayout 7.2 and higher 7.X (04.02.1999):
    //* |------------------------------------------------------------------------------------------ ... ---------- ... -----
    //* | record |  key   | var col| var col|  fix   |  fix   |  var   |  fix   |  fix   |  var   |     | var    |
    //* | length | length | offset | count  | key 1  | key 2  |  key   | col 1  | col 2  | col 1  |     |longchar|
    //* |        |        |        |        |        |        |        |        |        |        |     | col 1  |
    //* |   2    |   2    |   2    |   2    |        |        |        |        |        |        |     |        |
    //* | bytes  | bytes  | bytes  | bytes  |        |        |        |        |        |        |     |        |
    //* |------------------------------------------------------------------------------------------ ... ---------- ... -----
    //*
    //*          <-- KEYLEN_OFFSET_LS23 = 2 bytes : offset fuer den eintrag der keylaenge im datensatz
    //*                   <--   VARCOL_OFFSET_LS23 = 4 bytes : offset fuer den eintrag des beginns variabel langer spalten im datensatz
    //*                            <--  VARCOL_COUNT_OFFSET_LS23 = 6 bytes : offset fuer den eintrag der anzahl variabel langer spalten
    //*                                     <-- HEADER_OFFSET_LS23 = 8 bytes : entspricht den eintraegen fuer die laengen

    // We work here with some constant position offset in the record. Those offsets are defined in
    // the header file.

    tsp00_Longint plBufLen   = LENGTH_OFFSET_LS23;   // Datensatz startet mit laengenbytes --> ueberspring diese
    tsp00_Int2    lRecordLen = LENGTH_OFFSET_LS23;
    tsp00_Int2    lKeyLen    = 0;

    //*
    //*     BEGIN KEY VALUES
    //*
    if (true == pFLRec->flrIsSyskey_ls00)
    {
        //*
        //*     SYSKEY COLUMN
        //*
        //  Increase pFLRec->flrLstKeyAcp_ls00 and write value to buffer, no further checks necessary
        //  pFLRec->flrLenKeyAcp_ls00 is first time filled in ls21_ExecBeginLoad
        rc = ls24SurrogateIncrease(&pFLRec->flrLstKeyAcp_ls00[3]);
        if (errOK_els00 != rc)
        {
            pMsgObj->ls98Msg(layGRec_els98, ErrText, STATIC_CAST(ErrorNumbers, rc));
            return -STATIC_CAST(tsp00_Int4, rc);
        }

        SAPDB_memcpy(&Buffer[plBufLen], pFLRec->flrLstKeyAcp_ls00, pFLRec->flrLenKeyAcp_ls00);
        plBufLen += pFLRec->flrLenKeyAcp_ls00;    // mv bufpointer behind copied value

        lKeyLen += pFLRec->flrLenKeyAcp_ls00;
        
        j = 1;
    }
    else
    {   
        //*
        //*     USER DEFINED KEY COLUMNS
        //*
        //  All key values must be specified in fastload command or none of all is specified.
        //  Data for key columns must be supplied by the user

        //  Keylayout
        //  |-------------------...--------------------------------------- ...
        //  | def  |  fixed  || ... || def  | fixed   || def  |  var    ||  other values
        //  | byte |key value|| ... || byte |key value|| byte |key value||
        //  |-------------------...--------------------------------------- ...
        //
        //  Key values have no length specification in front

        for (j = 0; errOK_els00 == rc; ++j)
        {
            // This for-loop is left when the last key column is processed using a break because
            // the last key column is always a column of length type 'sp7_var_len' or on error (of course)
            DataIndex = pFLRec->flrDataSource_ls00.lFillIndex_ls00[j];

            tsp00_Longint lValLen = 0;
            rc = ls23_FillValue(&Buffer[plBufLen], lValLen,
                                &RawData[DataIndex],
                                ExtEncoding,
                                MultColumns->mcsColumn_ls00[pTI->tdFieldsArray_ls00[j]->fiStmtColNo_ls00 - 1],
                                pFLRec, j, ErrText);
            if (errOK_els00 == rc)
            {
                plBufLen += lValLen;
                lKeyLen  += STATIC_CAST(tsp00_Int2, lValLen);
                if (sp7_fix_len != pTI->tdFieldsArray_ls00[j]->fiLengthType_ls00)
                {
                    //*
                    //*     Variable long key value --> the LAST KEY VALUE OF A SINGLE RECORD
                    //*
                    ++j;        // because we leave here we have to set j to the next value

                    break;      // It's the last key value. So leave here.
                }
            }   // end if (rc == rc)

        } // end for (j = 0; ( (errOK_els00 == rc) && (j < pTI->tdFieldCount_ls00) ); ++j)
        
        
        if (errOK_els00 == rc)
        {
            //*
            //*     CHECK KEY LENGTH
            //*
            if (lKeyLen >= PERM_KEY_MXSP00)
            {
                pMsgObj->ls98Msg(layGRec_els98, ErrText, errKeyLength_els98);
                rc = errKeyLength_els98;
            }
        
            //*
            //*     CHECK KEY ORDER
            //*
            //*  Write key into key-member of fastload structure.
            //*
            tsp00_LcompResult   CompVal;
            CompVal.becomes(l_undef);

            s30cmp(&Buffer[HEADER_OFFSET_LS23], 1, lKeyLen, pFLRec->flrLstKeyAcp_ls00, 1, pFLRec->flrLenKeyAcp_ls00, CompVal);
            if (l_greater == CompVal)
            {
                // key value starts BEHIND record header (BUF_START_POS_LS23 + HEADER_OFFSET_LS23)
                SAPDB_memcpy(pFLRec->flrLstKeyAcp_ls00, &Buffer[HEADER_OFFSET_LS23], lKeyLen);
                pFLRec->flrLenKeyAcp_ls00 = lKeyLen;
            }
            else    // new key is less or result is undefined
            {
                pMsgObj->ls98Msg(layGRec_els98, ErrText, errKeyOrder_els98);
                rc = errKeyOrder_els98;
            }
        }   // end if (errOK_els00 == rc)

    } // end if (true == pFLRec->flrIsSyskey_ls00)


    if (errOK_els00 == rc)
    {
        lRecordLen += lKeyLen;      // computing record length first time
    }
    //*
    //*     END KEY VALUES
    //*



    //*
    //*     BEGIN FIX LONG COLUMNS
    //*
    if ( (errOK_els00 == rc) && (0 != pFLRec->flrFixColLen_ls00) )
    {
        tsp00_Longint lFixValLen = 0;

        for (j; ( (errOK_els00 == rc) && (j < pTI->tdFieldCount_ls00) ); ++j)
        {
            DataIndex = pFLRec->flrDataSource_ls00.lFillIndex_ls00[j];

            if (sp7_fix_len != pTI->tdFieldsArray_ls00[j]->fiLengthType_ls00)
            {
                break;
            }

            lFixValLen = 0;

            rc = ls23_FillValue(&Buffer[plBufLen], lFixValLen,
                                &RawData[DataIndex],
                                ExtEncoding,
                                MultColumns->mcsColumn_ls00[pTI->tdFieldsArray_ls00[j]->fiStmtColNo_ls00 - 1],
                                pFLRec, j, ErrText);
            if (errOK_els00 == rc)
            {
                plBufLen += lFixValLen;
            }
        }

        // Adjust record length: flrFixColLen_ls00 contains def-byte, too (value computed in ls21ExecFastload)
        if (errOK_els00 == rc)
        {
            lRecordLen += pFLRec->flrFixColLen_ls00;
        }
    }   // if (0 != pFLRec->flrFixColLen_ls00)


    //*
    //*     begin VARIABLE LONG COLUMNS
    //*
    // coming here only variable long columns may remain; we process normal long (<= 255 bytes) and very long (> 255 bytes)
    // variable long columns with the same procedure
    if ( (j < pTI->tdFieldCount_ls00) && (errOK_els00 == rc) )
    {
        tsp00_Addr     pszMemo    = NULL;    // To memorize the point where to write the col length
        tls00_Column  *myVarCol   = NULL;
        tsp00_Longint  lVarValLen = 0;
        for (j; ( (errOK_els00 == rc) && (j < pTI->tdFieldCount_ls00) ); ++j)
        {
            DataIndex = pFLRec->flrDataSource_ls00.lFillIndex_ls00[j];

            pFI      = pTI->tdFieldsArray_ls00[j];
            myVarCol = MultColumns->mcsColumn_ls00[pTI->tdFieldsArray_ls00[j]->fiStmtColNo_ls00 - 1];      // fuer eine leichtere bearbeitung
          
            // Skip (but memorize) position for length information of that column
            pszMemo = &Buffer[plBufLen];
            
            // Adjust lengths: 1 byte for long var and 2 bytes for very long var
            if (sp7_var_len == pFI->fiLengthType_ls00)
            {
                plBufLen   += sizeof(char);
                lRecordLen += sizeof(char);
            }
            else //(sp7_var_len_long == pFI->fiLengthType_ls00)
            {
                plBufLen   += 2 * sizeof(char);
                lRecordLen += 2 * sizeof(char);
            }
            
            lVarValLen = 0;     // contains the value length after return of ls23_FillValue

            rc = ls23_FillValue(&Buffer[plBufLen], lVarValLen, &RawData[DataIndex], ExtEncoding, myVarCol, pFLRec, j, ErrText);
            if (errOK_els00 == rc)
            {
                plBufLen   += lVarValLen;
                lRecordLen += STATIC_CAST(tsp00_Int2, lVarValLen);

                // write value length to buffer
                if (sp7_var_len == pFI->fiLengthType_ls00)
                {
                    *pszMemo = STATIC_CAST(char, lVarValLen);       // maximum length for this column type is 255
                }
                else //(sp7_var_len_long == pFI->fiLengthType_ls00)
                {
                    ls07Swap2BIntToChar(STATIC_CAST(tsp00_Int2,lVarValLen),    // Laenge in den puffer schreiben. U.u. noch swappen
                                        pFLRec->flrSwapKind_ls00,
                                        pTI->tdKernelSwap_ls00,
                                        pszMemo);
                }
            }   // end if (errOK_els00 == rc)
        
        }   // end for (j; j < pTI->tdFieldCount_ls00; ++j)
    }

    //*
    //*     end VARIABLE LONG COLUMNS
    //*
    



    //*
    //*     Write record header
    //*

    if (errOK_els00 == rc)
    {
        // key length
        ls07Swap2BIntToChar(lKeyLen, pFLRec->flrSwapKind_ls00, pTI->tdKernelSwap_ls00, &Buffer[KEYLEN_OFFSET_LS23]);

        // Offset of variable long columns; overall length of fix long columns corresponds to
        // the offset of variable long columns
        ls07Swap2BIntToChar(pFLRec->flrFixColLen_ls00,    // computed in ls21 - module
                            pFLRec->flrSwapKind_ls00,
                            pTI->tdKernelSwap_ls00,
                            &Buffer[VARCOL_OFFSET_LS23]);

        // Count of variable long columns (length <= 255 bytes);
        // Even if there are none of those this needs to be mentioned
        ls07Swap2BIntToChar(pFLRec->flrVarColCnt_ls00,    // computed in ls21 - module
                            pFLRec->flrSwapKind_ls00,
                            pTI->tdKernelSwap_ls00,
                            &Buffer[VARCOL_COUNT_OFFSET_LS23]);

        // record length
        ls07Swap2BIntToChar(lRecordLen, pFLRec->flrSwapKind_ls00, pTI->tdKernelSwap_ls00, &Buffer[BUF_START_POS_LS23]);
    }

    if (errOK_els00 != rc)
    {
        lRecordLen = STATIC_CAST(tsp00_Int2, rc);
    }

    return lRecordLen;  // We're done.
}
// ls23BuildRecord()


/*
  -----------------------------------------------------------------------------
  EndChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

//////////////////////////////////////////////////////////////////////////////////////////////
/*
  -----------------------------------------------------------------------------
  Chapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls23_BuildDefault
  -----------------------------------------------------------------------------
*/
static int
ls23_BuildDefault(tsp00_Addr           pszDestination,
                  tsp00_Longint       &lDestLen,
                  tls00_FieldInfo     *pFieldInfo,
                  tls00_PredefDefault *pDefaultStruct,
                  tls00_String        *pszOtherDefault,
                  tsp00_Int4           bDefaultOrNull)
{
    ROUTINE_DBG_MEO00 ("ls23_BuildDefault");

    tsp00_Int4  rc = errOK_els00;

    if (INSERT_DEFAULT == bDefaultOrNull)
    {
        if (0 == pFieldInfo->fiStmtColNo_ls00)      // column not specified in command; check if null/default is allowed
        {
            if ( (mod_mandatory == pFieldInfo->fiColMode_ls00) &&
                 (sp7_no_default == pFieldInfo->fiDefault_ls00) )
            {   
                // if column is not null defined (mandatory) but no default -> reject
                rc = errNoDataForNNCol_els98;
            }
        }
    }

    // If a null value must be inserted we need to make sure that no default is inserted instead
    if (errOK_els00 == rc)
    {
        lDestLen = ls24InsertDefaultVal(pFieldInfo->fiBasics_ls00,
                                        pFieldInfo->fiLengthType_ls00,
                                        pFieldInfo->fiDefault_ls00,
                                        pszDestination,
                                        0,
                                        pDefaultStruct,
                                        pszOtherDefault);
        if (-1 == lDestLen)
        {
            rc = errUnknownDefault_els98;
        }
    }

    return rc;
}
// ls23_BuildDefault()


/*
  -----------------------------------------------------------------------------
  function:     ls23_FillValue
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls23_FillValue(tsp00_Addr      pszDestination,
               tsp00_Longint  &lDestLen,
               tls00_String   *pSrcData,
               tls00_CodeType  SrcDataEncoding,
               tls00_Column   *pCol,
               tls00_FloadRec *pFLRec,
               tsp00_Int4      lValIndex,
               tsp00_Addr      pszErrText)
{

    LoaderMessages *pMsgObj = LoaderMessages::Instance();    // error reporting obj
    tsp00_Longint    rc      = errOK_els00;
    
    tls00_TableDescription &TD  = pFLRec->flrTableInfo_ls00;
    tls00_FieldInfo        *pFI = TD.tdFieldsArray_ls00[lValIndex];

    lDestLen = 0;       // making sure the length is initialized
    tsp00_Int4 lDefByteLen = sizeof(char);
    bool       bDefaultIsUnicode = false;           // flags if the value has to be processed in case of unicode (not if
                                                    // default type is other_default)

    if ( (INSERT_DEFAULT == pSrcData->strLeng_ls00) || (INSERT_NULL == pSrcData->strLeng_ls00) )
    {
        //*
        //*     INSERT DEFAULT OR NULL VALUE
        //*
        rc = ls23_BuildDefault(pszDestination,      // function delivers length of inserted default value
                               lDestLen,            // INCLUDING the defined byte
                               pFI,
                               &pFLRec->flrDefStruct_ls00,
                               pFLRec->flrDefValue_ls00[lValIndex],
                               pSrcData->strLeng_ls00);
        if (errOK_els00 == rc)
        {
            if ( (sp7_other_default == pFI->fiDefault_ls00) ||
                 ( UNDEF_BYTE_LS00 == STATIC_CAST(unsigned char, *pszDestination) ) )
            {                               // In case of user defined default (defined as 
                bDefaultIsUnicode = true;   // 'create table ... col1 char(10) not null DEFAULT 'HALLO' ...')
            }                               // the value is already in right form and length for unicode!;
                                            // same applies if the value is the NULL value
        }
        else
        {
            if (errUnknownDefault_els98 == rc)
            {   //  create internal error
                pMsgObj->ls98Msg(layFastload_els98, pszErrText, errInternal_els98, _INTERNAL_FILE_ID_,
                                                                                   __LINE__,
                                                                                   rc);
                rc = errInternal_els98;
            }
            else
            {
                pMsgObj->ls98Msg(layFastload_els98, pszErrText, STATIC_CAST(ErrorNumbers, rc),
                                                                TD.tdColName_ls00[lValIndex]->asCharp());
            }
        }
    }
    else
    {
        *pszDestination = ls07GetDefByte(pFI->fiBasics_ls00.fbDataType_ls00);   // first byte to insert is defined byte

        if (INSERT_ASSIGNED == pSrcData->strLeng_ls00)
        {
            //*
            //*     INSERT ASSIGNMENT VALUE
            //*

            // Upon success the function delivers the length of the assigned value
            lDestLen = ls24InsertAssignedVal(pCol,
                                             pFI->fiBasics_ls00,
                                             &pszDestination[sizeof(char)],
                                             0,                             //TODOTODO: HIER MUESSTE DER VERFUEGBARE FREIPLATZ ANGEGEBEN WERDEN
                                             &pFLRec->flrDefStruct_ls00);
            if (lDestLen < 0)
            {

                if ( (lDestLen != errInvalidNumber_els98) && (lDestLen != errNumberTruncated_els98) )
                {
                    pMsgObj->ls98Msg(layFastload_els98, pszErrText, STATIC_CAST(ErrorNumbers, lDestLen),
                                                                    TD.tdColName_ls00[lValIndex]->asCharp());
                }
                else    // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
                {
                    // This type of error can be generated only for sequence number assignments!
                    if (lDestLen == errInvalidNumber_els98)
                    {
                        SAPDB_Char _pAux[16];
                        _itot(pCol->colAssign_ls00.fasSeqNo_ls00, _pAux, 10);

                        pMsgObj->ls98Msg(layDataLoad_els98, pszErrText, STATIC_CAST(ErrorNumbers, lDestLen), _tcslen(_pAux), _pAux);
                    }
                }
                rc = lDestLen;
            }
            else
            {
                lDestLen += sizeof(char);       // counting the defined byte
            }
        }
        else
        {
            //*
            //*     INSERT USER DELIVERED VALUE
            //*
            if (0 != pSrcData->strLeng_ls00)        // char values might be empty (consist entirely of blanks)
            {
                SAPDB_memcpy(&pszDestination[sizeof(char)], pSrcData->strAddr_ls00, pSrcData->strLeng_ls00);
                lDestLen += pSrcData->strLeng_ls00 + sizeof(char);
            }
            else
            {
                lDestLen += sizeof(char);
            }
        }
    }

    //*
    //*     UNICODE CONVERSION
    //*
    //* Necessary 
    //*  - if data type is 'dunicode' (all numbers are non unicode stored in the db,
    //*    and the user defined default value comes as right unicode value) or the value is the NULL value
    //*     AND
    //*  - if external encoding type is either UTF8 or ASCII
    //*  - if value is not the empty value
    //* In case of external encoding type UCS2 we have only to check the swap type and
    //* swap the char data to the right type.
    //*
    if ( (rc == errOK_els00 ) &&
         (lDestLen > sizeof(char)) &&       // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127784
         ( (1 == szMap_UnicodeDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) && (false == bDefaultIsUnicode) ) )
    {
        rc = ls23_ConvertValues2UnicodeOnPage(pszDestination,
                                              lDestLen,
                                              pFI,
                                              SrcDataEncoding,
                                              pFLRec->flrSwapKind_ls00,
                                              pszErrText);
    }

    //*
    //*     LENGTH ADJUSTMENT
    //*
    //* Adjust column values depending on the length type - variable (trimming) or fix long (filling)
    //*
    if (rc == errOK_els00 )
    {
        if (sp7_fix_len == pFI->fiLengthType_ls00)
        {
            ls05InsertFillBytes(REINTERPRET_CAST(unsigned char*, pszDestination),
                                lDestLen,
                                pFI->fiBasics_ls00.fbDataType_ls00,
                                pFI->fiBasics_ls00.fbInOutLength_ls00,
                                (1 == szMap_UnicodeDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]),
                                sw_normal);     // all unicode data is inserted as not swapped 
            lDestLen = pFI->fiBasics_ls00.fbInOutLength_ls00;
        }
        else
        {
            //*
            //*     Trim variable long values
            //* the only case where number values may be trimmed is when a number column is
            //* the last (or only) primary key column which is always variable long
            //*
            if (lDestLen > 1)       // more than the required defined byte in the data?
            {
                char c = DEF_BYTE_ASCII_LS00;

                if ( (1 == szMap_NumberDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) ||
                     (1 == szMap_BlobDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) )
                {
                    c = DEF_BYTE_LS00;
                }

                tls00_CodeType CodeType = (1 == szMap_UnicodeDataType_ls00[pFI->fiBasics_ls00.fbDataType_ls00]) ?
                                          ctUCS2_els00 : ctAscii_els00;

                ls05StrTrimRight(pszDestination, lDestLen, c, CodeType);
                // In case the value consists entirely of DEF_BYTES the length is here
                // 'adjusted' to 0. This must be corrected the way that at least the
                // def_byte of the values 'survives'
                if (0 == lDestLen)
                {
                    lDestLen = 1;
                }
            }
        }   // end else of if (sp7_fix_len == pFI->fiLengthType_ls00)
    }

    return rc;
}
// ls23_FillValue()

/*
  -----------------------------------------------------------------------------
  function:     ls23_ConvertValues2UnicodeOnPage
  -----------------------------------------------------------------------------
*/
static tsp00_Longint
ls23_ConvertValues2UnicodeOnPage(tsp00_Addr           pszDestination,
                                 tsp00_Longint&       lDestLen,
                                 tls00_FieldInfo*     pFieldInfo,
                                 tls00_CodeType       SrcDataEncoding,
                                 tsp00_SwapKind_Enum  SwapKind,
                                 tsp00_Addr           pszErrText)
{
    tsp00_Longint rc = errOK_els00;

    if ( true == ls00IsUCS2(&SrcDataEncoding) )
    {
        // This step assumes running the load server on the same machine that stores the data
        //if (sw_full_swapped == pFLRec->flrSwapKind_ls00)
        if (sw_full_swapped == SwapKind)
        {
            ls07SwapUCS2StringInPlace((pszDestination + sizeof(char)), lDestLen - sizeof(char));
        }
    }
    else
    {
        unsigned char szTempBuffer[PAGE_MXSP00]; // a single value may not be bigger than a whole db page

        SAPDB_memcpy(szTempBuffer, pszDestination + sizeof(char), lDestLen - sizeof(char));   // skip defined byte

        tsp00_Longuint lTmpSrcLen  = pFieldInfo->fiBasics_ls00.fbInOutLength_ls00;

        rc = ls05ConvertToUCS2NotSwapped(REINTERPRET_CAST(unsigned char*, (pszDestination + 1) ),
                                         lTmpSrcLen,
                                         szTempBuffer,
                                         lDestLen - sizeof(char),
                                         SrcDataEncoding,
                                         pszErrText);
        if (errOK_els00 == rc)
        {
            lDestLen = lTmpSrcLen + sizeof(char);
        }
    }

    return rc;
}
// ls23_ConvertValues2UnicodeOnPage


/*
  -----------------------------------------------------------------------------
  function:     ls23_CheckForNULLValue
  -----------------------------------------------------------------------------
*/
static SAPDB_Int4
ls23_CheckForNULLValue(tls00_String*      pRawData,
                       tls00_String*      pInputLine,
                       tls00_Column*      pColumn,
                       tls00_FloadRec*    pFLRec,
                       tls00_FileFormat*  pFormat,
                       SAPDB_UInt4        FieldIndex)
{
    SAPDB_Int4 rc = errOK_els00;
    
    tls00_FieldInfo* pFI = pFLRec->flrTableInfo_ls00.tdFieldsArray_ls00[FieldIndex];

    // Attention: It is absolutely necessary to keep the sequence of processing ->
    //            first to check for nvcCount_ls00 and only then check if colNullCond_ls00 != NULL.
    //            This is because the caller has only check if pColumn is NULL or nvcCount_ls00 is TRUE.
    if (SAPDB_TRUE == pFLRec->flrNullValueCounter_ls00.nvcCount_ls00)
    {
        rc = ls24GetNILConstants(*pRawData,
                                 *pInputLine,
                                 pFI->fiColMode_ls00,
                                 *pFLRec->flrNullValueCounter_ls00.nvcAuxConditionSpec_ls00,
                                 pFI->fiDefault_ls00,
                                 *pFormat);
        if ( (errOK_els00 == rc) && (INSERT_NULL == pRawData->strLeng_ls00) )
        {
            ++pFLRec->flrNullValueCounter_ls00.nvcCounter_ls00;
        }
    }
    else if (NULL != pColumn->colNullCond_ls00)
    {                                               
        rc = ls24GetNILConstants(*pRawData, *pInputLine, pFI->fiColMode_ls00, *pColumn->colNullCond_ls00, pFI->fiDefault_ls00, *pFormat);
    }
    return rc;
}
// ls23_CheckForNULLValue()

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/
