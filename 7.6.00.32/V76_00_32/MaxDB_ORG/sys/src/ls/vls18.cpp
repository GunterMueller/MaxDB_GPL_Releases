/*
  -----------------------------------------------------------------------------
  module: vls18.cpp
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: Replication Server
 
  description:  Implementation of data file row and value functions
                
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
#include "hls05.h"
#include "hls07.h"
#include "hls18.h"
#include "hls19_check.h"
#include "hls98msg.h"
#include "hls99.h"

#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"

#define _INTERNAL_FILE_ID_    _T("vls18.cpp")

#define NL_UNDEFINED_LS18                           0
#define NL_SINGLE_DELIMITER_LS18                    1
#define NL_DELIMITER_PAIR_LS18                      2
#define NL_SEPARATOR_FOLLOWED_BY_DELIMITER_LS18     3

tsp77encoding const *pEncoding_ls18[8] = {sp77encodingAscii,
                                          sp77encodingAscii,
                                          sp77encodingAscii,
                                          sp77encodingUTF8,
                                          sp77encodingAscii,
                                          sp77encodingUCS2,
                                          sp77encodingUCS2Swapped,
                                          sp77encodingUCS2Native};
//*
//*     CSV parser states
//* These definitions are similar to pythons CSV-module definitions
//*
typedef enum {
    START_RECORD,
    START_FIELD,
    IN_FIELD,
    IN_QUOTED_FIELD
}
tls18_CSV_Parser_State;


typedef enum {
    IN_STRING,
    DOUBLE_QUOTED_VALUE,
    SINGLE_QUOTED_VALUE
}
tls18_Normalize__state;

/*
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration 
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls18_CheckForNLInDataA
  -----------------------------------------------------------------------------
*/
SAPDB_Bool
ls18_CheckForNLInDataA(tls00_Buffer2* pBuffer, tls00_Pattern* pPattern, tls00_FileFormat* pFileFormat);


/*
  -----------------------------------------------------------------------------
  function:     ls18_GetCSVDataA
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls18_GetCSVDataA(char          *pString,
                 tsp00_Uint4    lStrLen,
                 tls00_Pattern *pPattern,
                 int            lCount,
                 tsp00_Uint4    lPosToRead[],
                 tls00_CodeType CodeType,
                 tls00_String   pValue[],
                 int           &lMissingIndex);

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
  function:     ls18GetRowLength
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetRowLength(tls00_MultColSpec      &Columns,
                 tls00_TableDescription &TableInfo,
                 //bool                    bBinary,    // defaults to false
                 bool                    bUseLastColPos,
                 bool                    bParam)     // defaults to false
{
    ROUTINE_DBG_MEO00 ("ls18GetRowLength");

    tsp00_Int4 lBytes = 0;    // This will be the return value
            
    if (true == bUseLastColPos)
    {
        //  Formatted (text and binary) --> Max position defines the count of bytes to read
        TRACE_PRNF_MLS99(("ls18GetRowLength", "using last column position"));
        
        int i = 0;
        for (i; i < Columns.mcsCount_ls00; i++)
        {
            if ( Columns.mcsColumn_ls00[i]->colFPos_ls00.fpoEnd_ls00 > lBytes )
            {
                lBytes = Columns.mcsColumn_ls00[i]->colFPos_ls00.fpoEnd_ls00;
            }
        }
        TRACE_PRNF_MLS99(("ls18GetRowLength", "Bytes per row: %d\n", lBytes));
    }
    else
    { 
        // Compressed in text mode
        // Special treatment for LONG columns: DEF_MAX_LONG_VALUE_LS00 Bytes is max. length
        
        // Data is saved in ASCII. We count 1 byte per character/digit.
        // The character count = sp7fi_len.
        // characters:              sp7fi_len = sp7fi_in_out_l - 1
        // numbers (integers ...):  sp7fi_len = number of digits.
        // If we rely in case of text files on the position specification in the load command
        // the read function would need to reallocate space any time the positions give a
        // wrong length. To prevent this we use the internal length.
        tls00_FieldBasics   *pBasicInfo;        //PTS 1107309 (S.S.)

        int i = 0;
        for (i; i < TableInfo.tdFieldCount_ls00; ++i)
        {
            if (true == bParam)
            {
                pBasicInfo = &TableInfo.tdParamArray_ls00[i]->piBasicInfo_ls00;
                TRACE_PRNF_MLS99(("ls18GetRowLength", "Params used\n"));
            }
            else
            {
                pBasicInfo = &TableInfo.tdFieldsArray_ls00[i]->fiBasics_ls00;
                TRACE_PRNF_MLS99(("ls18GetRowLength", "Fields used\n"));
            }

            if (1 == szMap_LongDataType_ls00[pBasicInfo->fbDataType_ls00])    // LONG column
            {
                lBytes += DEF_MAX_LONG_VALUE_LS00;  // Add delimiters and separators
                
                TRACE_PRNF_MLS99(("ls18GetRowLength", "Long column: + %d bytes\n", DEF_MAX_LONG_VALUE_LS00));
            }
            else
            {
                lBytes += pBasicInfo->fbLength_ls00;

                TRACE_PRNF_MLS99(("ls18GetRowLength", "Column: + %d bytes\n", pBasicInfo->fbLength_ls00));
            }
        }
        lBytes += (Columns.mcsCount_ls00*3 - 1);    // Add delimiters and separators
        
        TRACE_PRNF_MLS99(("ls18GetRowLength", "Including dels and seps bytes per row: %d\n", lBytes));
    }

    return lBytes;
}
//  ls18GetRowLength()

/*
  -----------------------------------------------------------------------------
  function:     ls18GetRowLengthForExtract
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetRowLengthForExtract(tls00_MultColSpec      *Columns,
                           tls00_ColumnData       *pColumnData,
                           tls00_TableDescription *TableInfo,
                           tls00_FileFormat       *FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls18GetRowLengthForExtract");

    tsp00_Int4 lBytes = 0;  // the return value
    int        i      = 0;

    if (true == FileFormat->ffoFormatted_ls00)
    {
        lBytes = ls18GetRowLength(*Columns, *TableInfo, true, true);
    }
    else
    {
        lBytes = ls18GetRowLength(*Columns, *TableInfo, false, true);

        for (i=0; i < Columns->mcsCount_ls00; i++)
        {
            tls00_Column *myColumn = Columns->mcsColumn_ls00[i];
            if (true == myColumn->colIsLit_ls00)        // literal is null terminated
            {
                // add bytes for delimiters und separators
                lBytes += 3 + STATIC_CAST( tsp00_Int4, myColumn->colName_ls00.length() );
                continue;
            }

            // We use the length check somewhat incorrectly: we specify a value length of 0
            // and get the right length back
            tsp00_Int4 rc = errOK_els00;
            tsp00_Int4 ValLen = 0;
            tsp00_Int4 NILValLen = 0;
        
            rc = ls19CheckLengthForExtract(0,
                                           myColumn,
                                           &TableInfo->tdParamArray_ls00[pColumnData[i].cdPosInStmt_ls00]->piBasicInfo_ls00,
                                           FileFormat,
                                           ValLen);     // needed length

            rc = ls19CheckNILValLength(0,
                                       myColumn,
                                       TableInfo->tdParamArray_ls00[pColumnData[i].cdPosInStmt_ls00]->piMode_ls00,
                                       FileFormat,
                                       NILValLen);
            
            lBytes += (NILValLen > ValLen) ? NILValLen: ValLen;
        }
    }

    return lBytes;
}
//  ls18GetRowLengthForExtract()


/*
  -----------------------------------------------------------------------------
  function:     ls18GetRecordLength
  -----------------------------------------------------------------------------
*/
tsp00_Int2
ls18GetRecordLength(tls00_TableDescription & TableInfo, bool bParam)
{
    ROUTINE_DBG_MEO00 ("ls18GetRecordLength");

    tsp00_Int2 Length = 0;
    tsp00_Int4 i      = 0;

    if (true == bParam)     // work with params
    {
        //* record length corresponds to the buffer position plus
        //* length of parameter for this position. But the buffer position
        //* are not sorted ascending --> find max
        tsp00_Int2 Max = 0;
        
        for(i=0; i < TableInfo.tdFieldCount_ls00; ++i)
        {
            tls00_ParamInfo *Param = TableInfo.tdParamArray_ls00[i];

            Max = Param->piBasicInfo_ls00.fbInOutLength_ls00 + Param->piBufpos_ls00 - 1;
            if(Length < Max)
            {
                Length = Max;
            }
        }
    }
    else        // work with fields; THIS IS MAINLY (ONLY) FOR FASTLOAD !!!!
    {
        Length = 8;     // 8 byte header in every record

        for(i=0; i < TableInfo.tdFieldCount_ls00; ++i)
        {
            tls00_FieldInfo *FI = TableInfo.tdFieldsArray_ls00[i];
            
            Length += FI->fiBasics_ls00.fbInOutLength_ls00;

            // depending on length type of column some bytes need to be added
            // the following length types exist: sp7_fix_len, sp7_var_len, sp7_var_len_long
            switch (FI->fiLengthType_ls00)
            {
                case sp7_var_len:
                {
                    //  syskey or last column of key are variable long ===> skip this column
                    if ( (mod_key    != FI->fiColMode_ls00) &&
                         (mod_syskey != FI->fiColMode_ls00) )
                    {
                        Length += 1;        // add the length byte to the record length
                    }
                    break;
                }
                case sp7_var_len_long:
                {
                    Length += 2;            // add length bytes to the record length
                    break;
                }
                default:    // corresponds to sp7_fix_len
                {
                    break;      // nothing to add
                }
            } //switch (FI->fiLengthType_ls00)

        } //for(i=0; i < TableInfo.tdFieldCount_ls00; ++i)
    }

    return Length;
}
// ls18GetRecordLength()


/*
  -----------------------------------------------------------------------------
  function:     ls18RecordInit
  -----------------------------------------------------------------------------
*/
void
ls18RecordInit(tsp00_Int2 ColumnCount, tls00_String *& DataArray, tls00_String & pszString)
{
    ROUTINE_DBG_MEO00 ("ls18RecordInit");

    tsp00_Int4 i = 0;

    for (i; i<ColumnCount; ++i)
    {
        if (NULL != DataArray[i].strAddr_ls00)
        {
            // If the pointer is within string (e.g. &pszString[0] < ptr < &pszString[lLength]),
            // set simply to NULL. Otherwise also delete the pointer.
            if ( (DataArray[i].strAddr_ls00 < &pszString.strAddr_ls00[0]) ||
                 (DataArray[i].strAddr_ls00 > &pszString.strAddr_ls00[pszString.strLeng_ls00]) )
            {
                delete [] DataArray[i].strAddr_ls00;
            }
            DataArray[i].strAddr_ls00 = NULL;
        }
        DataArray[i].strLeng_ls00 = INSERT_EMPTY_VALUE;
    }
}
//  ls18RecordInit()


/*
  -----------------------------------------------------------------------------
  function:     ls18RecordInit - overloaded
  -----------------------------------------------------------------------------
*/
void
ls18RecordInit(tsp00_Int2      ColumnCount,
               tls00_String *& DataArray,
               tsp00_Addr      pszString,
               tsp00_Int4      lStrLen)
{
    ROUTINE_DBG_MEO00 ("ls18RecordInit");

    tsp00_Int4 i = 0;

    for (i; i<ColumnCount; ++i)
    {
        if (NULL != DataArray[i].strAddr_ls00)
        {
            // If the pointer is within string (e.g. &pszString[0] < ptr < &pszString[lLength]),
            // set simply to NULL. Otherwise also delete the pointer.
            if ( (DataArray[i].strAddr_ls00 < &pszString[0]) ||
                 (DataArray[i].strAddr_ls00 > &pszString[lStrLen]) )
            {
                delete [] DataArray[i].strAddr_ls00;
            }
            DataArray[i].strAddr_ls00 = NULL;
        }
        if (INSERT_ASSIGNED != DataArray[i].strLeng_ls00)
        {
            DataArray[i].strLeng_ls00 = INSERT_EMPTY_VALUE;
        }
    }
}
//  ls18RecordInit()


/*
  -----------------------------------------------------------------------------
  function:     ls18RecordConstructor
  -----------------------------------------------------------------------------
*/
tsp00_Int2
ls18RecordConstructor(tsp00_Int2 ColumnCount, tls00_String *& DataArray)
{
    ROUTINE_DBG_MEO00 ("ls18RecordConstructor");

    tsp00_Int2  rc  = errOK_els00;

    DataArray = new tls00_String[ColumnCount];
    if (NULL == DataArray)
    {
        rc = errNoMoreMemory_els98;
    }
    else
    {
        tsp00_Int4 i = 0;
            
        for (i; i< ColumnCount; i++)
        {
            DataArray[i].strLeng_ls00 = INSERT_EMPTY_VALUE;
            DataArray[i].strAddr_ls00 = NULL;
        }
    }
    return rc;
}
//  ls20RecordConstructor()


/*
  -----------------------------------------------------------------------------
   function:     ls18RecordDestructor
  -----------------------------------------------------------------------------
*/
void
ls18RecordDestructor(tsp00_Int2 ColumnCount, tls00_String *& DataArray)
{
    ROUTINE_DBG_MEO00 ("ls18RecordDestructor");

    delete [] DataArray;
    DataArray = NULL;
}
// ls18RecordDestructor()


/*
  -----------------------------------------------------------------------------
  function:     ls18GetUnformattedValue
  -----------------------------------------------------------------------------
*/
int
ls18GetUnformattedValue(tsp00_Addr      pszLine,
                        tsp00_Int4      LineLength,
                        tsp00_Int4      lIndex,
                        char            szSep[],    // 2 byte char array
                        char            szQuot[],   // 2 byte char array
                        tls00_String*   Value)
{
    ROUTINE_DBG_MEO00 ("ls18GetUnformattedValue");
    
    tsp00_Int4 rc = errOK_els00;

    char                szFind[3];                  // Null terminated string containing ',"'
    static tsp00_Int4   lBeginPos;
    static tsp00_Int4   lFoundPos = -1;
    static tsp00_Int4   lQuotPos1 = 0;
    static tsp00_Int4   lQuotPos2 = 0;
    static tsp00_Int4   HitNo;
    static tsp00_Int4   lLength;

    if(-1 != LineLength)
    {
        lBeginPos = lQuotPos1 = lQuotPos2 = HitNo = 0;
        lFoundPos = -1;
        lLength = LineLength;
    }

    // lIndex may not be LT HitNo; we assume ascending sorting
    if (HitNo != lIndex)
    {
        sprintf(szFind, "%c%c", szSep[0], szQuot[0]);
        lQuotPos2 = 0;
        
        while ( (HitNo != lIndex) && (lBeginPos < lLength) )
        {
            lBeginPos += lFoundPos + 1;
            if (lBeginPos > lLength)    // If value is GT length of input string --> stop
            {
                break;
            }            
            // Find separator or Delimiter in pszLine
            lFoundPos = STATIC_CAST( tsp00_Int4, strcspn(&pszLine[lBeginPos], szFind) );
            
            if (szQuot[0] == pszLine[lFoundPos+lBeginPos])      // No delimiter found so far, sure!
            {
                lBeginPos += lFoundPos + 1;
                
                lFoundPos = STATIC_CAST( tsp00_Int4, strcspn(&pszLine[lBeginPos], szQuot) );   // Find 2nd delimiter
                if ((lFoundPos + lBeginPos) == lLength)
                {
                    rc = errCorruptDataFile_els98;
                    break;
                }
                
                lQuotPos1 = lBeginPos;
                lQuotPos2 = lFoundPos;
                continue;
            }
            else                // found char is no delimiter so it's a separator --> increase number of found values.
            {
                HitNo += 1;
                if (HitNo != lIndex)
                {
                    lQuotPos1 = lQuotPos2 = 0;
                }
            }
        }   // end while ( (HitNo != lIndex) && (lBeginPos < lLength) )
    }   // end if (HitNo != lIndex)

    // There are less than lIndex values in the line
    if (errOK_els00 == rc)
    {
        if (lIndex > HitNo)         
        {
            rc = errMissingData_els98;
        }
        else    // (lIndex == HitNo)
        {
            // if no value found (= the length positions are 0): Don't do anything.
            // This should be handled by the caller. The value contains a length
            // indicating to insert the default value.
            if (0 != lQuotPos2)     // Delimiters found in the value.
            {
                Value->strLeng_ls00 = lQuotPos2;
                Value->strAddr_ls00 = &pszLine[lQuotPos1];
            }
            else
            {
                if (0 != lFoundPos)
                {
                    Value->strLeng_ls00 = lFoundPos;
                    Value->strAddr_ls00 = &pszLine[lBeginPos];
                }
            }
        }
    }
    
    return rc;
}
//  ls18GetUnformattedValue()


/*
  -----------------------------------------------------------------------------
  function:     ls18GetCSVData
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls18GetCSVData(char          *pString,
               tsp00_Uint4    lStrLen,
               tls00_Pattern *pPattern,
               int            lCount,
               tsp00_Uint4    lPosToRead[],
               tls00_CodeType CodeType,
               tls00_String   pValue[],
               int           &lMissingIndex)
{
    ROUTINE_DBG_MEO00 ("ls18GetCSVData");

    SAPDB_Int4 rc = errOK_els00;

    const tsp77encoding* _pEncoding  = pEncoding_ls18[CodeType];

    //*
    //*     Faster function in case of ASCII data
    //*
    if (csp_ascii == _pEncoding->EncodingType) {
        return ls18_GetCSVDataA(pString, lStrLen, pPattern, lCount, lPosToRead, CodeType, pValue, lMissingIndex);
    }

    tsp77charConst              _Separator;
    tsp77charConst              _Delimiter;
    char*                       pszBuffer        = pString;
    SAPDB_UInt                  _lCharSize       = _pEncoding->fixedCharacterSize;            // Initialize for ASCII
    const tsp77charTable* const _pCharTable      = _pEncoding->charTable;

    // This is the length in Bytes!
    //ATTENTION: A HACK! Consider trailing NULL as last field separator !?
    SAPDB_UInt4                 _myFakedLen      = lStrLen + _pCharTable->terminator.byteCount;
    SAPDB_UInt4                 _myStrLen        = 0;
    bool                        bSingleDelimiter = false;
    tls18_CSV_Parser_State      _state           = START_RECORD;
    SAPDB_Int4                  _myCount         = 0;
    SAPDB_Int4                  _myPosToRead     = 0;

    tls00_String                _pIterator;
    _pIterator.strAddr_ls00 = NULL;
    _pIterator.strLeng_ls00 = 0;


    if (csp_utf8 == _pEncoding->EncodingType)
    {
        _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, pszBuffer));
    }

    // As the pattern consists of the sequence 'delimiter_separator-delimiter' we may conclude
    // from the pattern the separator
    _Separator.byteCount = pPattern->lpatPatternLen_ls00 - 2*pPattern->lpatDelimiterLen_ls00;
    SAPDB_memcpy(_Separator.bytes, pPattern->ppatPattern_ls00 + pPattern->lpatDelimiterLen_ls00, _Separator.byteCount);

    if (0 == pPattern->lpatDelimiterLen_ls00)
    {
        _Delimiter.byteCount = _pCharTable->terminator.byteCount;
        memset(_Delimiter.bytes, 0, sizeof(_Delimiter.bytes));
        if (csp_unicode == _pEncoding->EncodingType)
        {
            _Delimiter.bytes[1] = 2;    // make it something unprintable
        }
        else
        {
            _Delimiter.bytes[0] = 2;    // make it something unprintable
        }
    }
    else
    {
        _Delimiter.byteCount = pPattern->lpatDelimiterLen_ls00;

        memset(_Delimiter.bytes, 0, sizeof(_Delimiter.bytes));
        SAPDB_memcpy(_Delimiter.bytes, pPattern->ppatDelimit_ls00, _Delimiter.byteCount);
    }

    do
    {
        if (0 == memcmp(_Delimiter.bytes, &pszBuffer[_myStrLen], _Delimiter.byteCount))
        {   // DELIMITER FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a delimiter!
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen + _lCharSize];
                    _state = IN_QUOTED_FIELD;
                    break;
                }
                case IN_QUOTED_FIELD:
                {
                    bSingleDelimiter = !bSingleDelimiter;

                    if (true == bSingleDelimiter)
                    {
                        // we have (perhaps) found a closing delimiter --> compute value length (delim length
                        // is already excluded)
                        if (NULL != _pIterator.strAddr_ls00)
                        {
                            _pIterator.strLeng_ls00 = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                        }
                    }
                    break;
                }
                case IN_FIELD:
                {   // could happen as the values might be enclosed in delimiters or not;
                    // in case delimiter is defined and we come here: handle as normal char
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> same as START_RECORD
                    // reset value pointer
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen + _lCharSize];
                    _state = IN_QUOTED_FIELD;
                    break;
                }
            }
        }
        else if ( (0 == memcmp(_Separator.bytes, &pszBuffer[_myStrLen], _Separator.byteCount)) ||
                  (0 == memcmp(_pCharTable->terminator.bytes, &pszBuffer[_myStrLen], _pCharTable->terminator.byteCount) ) )
        {
            // SEPARATOR FOUND --> except of one case this is the transition to a new value
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a separator!
                    // empty value; leave the ptr as NULL ptr
                    _pIterator.strAddr_ls00 = NULL;
                    _pIterator.strLeng_ls00   = 0;
                    _state = START_FIELD;

                    bSingleDelimiter = false;
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> could be an empty value?
                    // leave the state as it is --> it's a new START_FIELD
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00   = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }

                    bSingleDelimiter = false;                   
                    break;
                }
                case IN_QUOTED_FIELD:
                {
                    if (true == bSingleDelimiter)
                    {   // if bSingleDelimiter is set to true we have found the separator following
                        // the last delimiter that encloses the value --> define the value and
                        // set state to something like 'OUTSIDE VALUE'
                        _state = START_FIELD;

                        bSingleDelimiter = false;
                    }
                    //else  // we're in a value --> nothing to switch, nothing to do
                    break;
                }
                case IN_FIELD:
                {   // found the end of a value --> define the value and
                    // set state to something like 'OUTSIDE VALUE'
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00   = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }

                    bSingleDelimiter = false;

                    _state = START_FIELD;
                    break;
                }
            }

            //*
            //*     Assign read value to value array member
            //*
            if (START_FIELD == _state)
            {   // We have to handle the situation that the user specified the same read positions several times
                // user given positions start with 1 but we count beginning with 0
                while ( (_myCount < lCount) && (_myPosToRead == (lPosToRead[_myCount] - 1)) )
                {
                    if ( (_pIterator.strLeng_ls00 > 0) && (INSERT_ASSIGNED != pValue[_myCount].strLeng_ls00) )
                    {   // leave the string undefined and string length = INSERT_EMPTY_VALUE
                        // if the length is 0
                        pValue[_myCount].strLeng_ls00 = _pIterator.strLeng_ls00;
                        pValue[_myCount].strAddr_ls00 = _pIterator.strAddr_ls00;      // assign the value
                    }
                    ++_myCount;
                }
                ++_myPosToRead;    // for every transition even if the value is not the one to read

                //*
                //*     Generally - this is the transition to a new value!
                //*    
                if (_myStrLen < lStrLen)        // make sure not to assign a value outside the read line
                {
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen + _lCharSize];
                    _pIterator.strLeng_ls00 = 0;
                }
            }
        }
        else if (0 == memcmp(_pCharTable->nl.bytes, &pszBuffer[_myStrLen], _pCharTable->nl.byteCount))
        {   // NL FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a nl -> empty line !!!
                    // don't change the state as this means the next character begins a new record
                    break;
                }
                case START_FIELD:       
                {   // somewhere outside a value --> last value;could be an empty value ?
                    // change the state to START_RECORD as this means the next character begins a new record
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00   = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }
                    _state = START_RECORD;
                    break;
                }
                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character if it is inside a value parsing;
                    // in case it is outside (bSingleDelimiter is true) it must be the end of that record as all records are
                    //  per definitionem limited by new lines -> set the state to START_RECORD
                    if (true == bSingleDelimiter)
                    {
                        _state = START_RECORD;
                    }
                    break;
                }
                case IN_FIELD:
                {   // last value in record reached;
                    // change the state to START_RECORD as this means the next character begins a new record
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00 = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }
                    _state = START_RECORD;
                    break;
                }
            }
        }
        else if (0 != _pEncoding->isSpace(&pszBuffer[_myStrLen]))        // isspace stops for \r and \n, too; so handle this earlier
        {   // SPACE CHAR FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a space -> might be outside a value
                    // 1. could be the empty value though
                    //    example: here begins a new record -->|  "value1","value2",...
                    // 2. it IS a value if the delimiter is defined empty
                    //    example: here begins a new record -->|    testvalue1,testvalue2,...

                    if (0 != pPattern->lpatDelimiterLen_ls00) {
                        _state = START_FIELD;
                    }
                    else {
                        _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen];
                        _state = IN_FIELD;
                    }
                    break;
                }
/*                case START_FIELD:
                {   // somewhere outside a value --> could be an empty value?
                    // leave the state as it is
                    break;
                }
                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
                case IN_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
*/
            }
        }
        else
        {   // SOMETHING ELSE FOUND
            switch (_state)
            {
                case START_RECORD:      
                {   // this is the initial state; first char found is a non-special character
                    //  --> first value is an undelimited one
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen];

                    _state = IN_FIELD;
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> next value is an undelimited one
                    _state = IN_FIELD;
                    //setting ptr to value is done when the separator is found
                    break;
                }
/*                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
                case IN_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
*/
            }
        }

        _myStrLen += _lCharSize;
        if (csp_utf8 == _pEncoding->EncodingType)
        {
            _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, &pszBuffer[_myStrLen]));
        }
    }
    while ( (START_RECORD != _state) && (_myStrLen < _myFakedLen) && (_myCount < lCount) ) ;

    // Check if we found all patterns but should have read some more values
    if (errOK_els00 == rc)
    {
        if (_myCount < lCount)
        {
            lMissingIndex = _myCount;
            rc = errMissingData_els98;
        }
    }

    return rc;
}
//ls18GetCSVData()


/*
  -----------------------------------------------------------------------------
  function:     ls18NormalizeString
  -----------------------------------------------------------------------------
*/
void
ls18NormalizeString(const char    *pString,
                    tsp00_Uint4    lStrLen,     // length in characters!
                    char*          pszNormalizedString,     // must be allocated with lStrLen length * char size (in Bytes) 
                    tls00_CodeType CodeType)
{
    ROUTINE_DBG_MEO00 ("ls18NormalizeString");

    SAPDB_Int4 rc = errOK_els00;

    if (NULL == pszNormalizedString)
    {
        return;
    }

    SAPDB_Char*                 _pszBuffer       = CONST_CAST(SAPDB_Char*, pString);
    const tsp77encoding*        _pEncoding       = pEncoding_ls18[CodeType];
    tsp77charConst              _szSingleQuote;
    tsp77charConst              _szDoubleQuote;
    const tsp77charTable* const _pCharTable      = _pEncoding->charTable;

    SAPDB_Int4                  _cntWhiteSpaces  = 0;
    bool                        _bLonelyQuote    = false;

    SAPDB_UInt4                 i                = 0;                                    // This is the length in Bytes!
    tls18_Normalize__state      _state           = IN_STRING;
    SAPDB_UInt                  _lCharSize       = _pEncoding->fixedCharacterSize;       // Initialize for ASCII
    if (csp_utf8 == _pEncoding->EncodingType)
    {
        _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, _pszBuffer));
    }

    // Assign quote characters to working variables
    memset(_szSingleQuote.bytes, 0, sizeof(_szSingleQuote.bytes));
    memset(_szDoubleQuote.bytes, 0, sizeof(_szDoubleQuote.bytes));
    if (csp_ascii == _pEncoding->EncodingType)
    {
        _szSingleQuote.byteCount = 1;
        _szSingleQuote.bytes[0] = '\'';

        _szDoubleQuote.byteCount = 1;
        _szDoubleQuote.bytes[0] = '"';
    }
    else
    {
        if (csp_utf8 == _pEncoding->EncodingType)
        {
            _szSingleQuote.byteCount = 1;
            _szSingleQuote.bytes[0] = '\'';

            _szDoubleQuote.byteCount = 1;
            _szDoubleQuote.bytes[0] = '"';
        }
        else        // Must be UCS2
        {
            SAPDB_Byte _szSign[2];
            SAPDB_Char _szIgnoreErr[256];
            tsp00_Longuint _len;

            _szSign[1] = 0;
            _szSign[0] = '\'';
            _szSingleQuote.byteCount = _pCharTable->terminator.byteCount;
            ls05ConvertToUCS2Simple(REINTERPRET_CAST(SAPDB_Byte*, _szSingleQuote.bytes),
                                    _len,
                                    CodeType,
                                    _szSign,
                                    1,
                                    ctAscii_els00, _szIgnoreErr);
            _szSingleQuote.byteCount = _len;


            _szSign[0] = '"';
            _szDoubleQuote.byteCount = _pCharTable->terminator.byteCount;
            ls05ConvertToUCS2Simple(REINTERPRET_CAST(SAPDB_Byte*, _szDoubleQuote.bytes),
                                    _len,
                                    CodeType,
                                    _szSign,
                                    1,
                                    ctAscii_els00, _szIgnoreErr);
            _szDoubleQuote.byteCount = _len;
        }
    }

    SAPDB_Char* _ptrNormalized = pszNormalizedString;

    do
    {
        if (0 != _pEncoding->isSpace(_pszBuffer))
        {
            switch (_state)
            {
                case IN_STRING:
                {
                    ++_cntWhiteSpaces;
                    break;
                }
                case SINGLE_QUOTED_VALUE:         // ATTENTION: DOUBLE FALL THROUGH
                case DOUBLE_QUOTED_VALUE:
                {
                    if (true == _bLonelyQuote)
                    {
                        _state = IN_STRING;
                        _bLonelyQuote = !_bLonelyQuote;
                        ++_cntWhiteSpaces;
                    }
                    else
                    {
                        if (1 == _lCharSize)
                        {
                            *pszNormalizedString = *_pszBuffer;
                        }
                        else
                        {
                            SAPDB_memcpy(pszNormalizedString, _pszBuffer, _lCharSize);
                        }
                        pszNormalizedString += _lCharSize;
                    }
                    break;
                }
            }
        }
        else if (sp77charIsEqual(&_szDoubleQuote, _pszBuffer, _szDoubleQuote.byteCount))
        {
            switch (_state)
            {
                case IN_STRING:
                {
                    if (0 != _cntWhiteSpaces)
                    {
                        SAPDB_memcpy(pszNormalizedString, _pCharTable->blank.bytes, _pCharTable->blank.byteCount);
                        pszNormalizedString += _pCharTable->blank.byteCount;
                        _cntWhiteSpaces = 0;
                    }
                    _state = DOUBLE_QUOTED_VALUE;
                    break;
                }
                case DOUBLE_QUOTED_VALUE:
                {
                    // Check if quoted string ends here
                    _bLonelyQuote = !_bLonelyQuote;
                    if (true == _bLonelyQuote)
                    {
                        _state = IN_STRING;
                        _bLonelyQuote = false;          // reset
                    }
                    break;
                }
                case SINGLE_QUOTED_VALUE:
                {
                    break;
                }
            }   // end switch (_state)

            if (1 == _lCharSize)
            {
                *pszNormalizedString = *_pszBuffer;
            }
            else
            {
                SAPDB_memcpy(pszNormalizedString, _pszBuffer, _lCharSize);
            }
            pszNormalizedString += _lCharSize;
            
        }
        else if (sp77charIsEqual(&_szSingleQuote, _pszBuffer, _szSingleQuote.byteCount))
        {
            switch (_state)
            {
                case IN_STRING:
                {
                    if (0 != _cntWhiteSpaces)
                    {
                        SAPDB_memcpy(pszNormalizedString, _pCharTable->blank.bytes, _pCharTable->blank.byteCount);
                        pszNormalizedString += _pCharTable->blank.byteCount;
                        _cntWhiteSpaces = 0;
                    }
                    _state = SINGLE_QUOTED_VALUE;
                    break;
                }
                case SINGLE_QUOTED_VALUE:
                {
                    // Check if quoted string ends here
                    _bLonelyQuote = !_bLonelyQuote;
                    if (true == _bLonelyQuote)
                    {
                        _state = IN_STRING;
                        _bLonelyQuote = false;          // reset
                    }
                    break;
                }
                case DOUBLE_QUOTED_VALUE:
                {
                    _bLonelyQuote = !_bLonelyQuote;
                    break;
                }
            } // end switch()
            
            if (1 == _lCharSize)
            {
                *pszNormalizedString = *_pszBuffer;
            }
            else
            {
                SAPDB_memcpy(pszNormalizedString, _pszBuffer, _lCharSize);
            }
            pszNormalizedString += _lCharSize;
        }
        else        // normal character
        {
            switch (_state)
            {
                case IN_STRING:
                {
                    if (0 != _cntWhiteSpaces)
                    {
                        SAPDB_memcpy(pszNormalizedString, _pCharTable->blank.bytes, _pCharTable->blank.byteCount);
                        pszNormalizedString += _pCharTable->blank.byteCount;
                        _cntWhiteSpaces = 0;
                    }
                    break;
                }
                case SINGLE_QUOTED_VALUE:         // ATTENTION: double fall through
                case DOUBLE_QUOTED_VALUE:
                {
                    if (true == _bLonelyQuote)      // quoted value ends here
                    {
                        _state = IN_STRING;
                        _bLonelyQuote = !_bLonelyQuote;
                    }
                    break;
                }
            }
            if (1 == _lCharSize)
            {
                *pszNormalizedString = *_pszBuffer;
            }
            else
            {
                SAPDB_memcpy(pszNormalizedString, _pszBuffer, _lCharSize);
            }
            pszNormalizedString += _lCharSize;
        }

        i          += _lCharSize;
        _pszBuffer += _lCharSize;
        if (csp_utf8 == _pEncoding->EncodingType)
        {
            _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, _pszBuffer));
        }
    }
    while (i < lStrLen);

    *pszNormalizedString = 0;
    pszNormalizedString = _ptrNormalized;

    return;
}
//ls18NormalizeString()


/*
  -----------------------------------------------------------------------------
  function:     ls18PutUnformattedValue
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls18PutUnformattedValue(tsp00_Addr        pszBuffer,
                        tsp00_Longint     lBufLen,
                        tsp00_Int4        lColumnCount,
                        tls00_ColumnData *pColumnData,
                        tls00_Pattern    *pPattern)
{
    ROUTINE_DBG_MEO00 ("ls18PutUnformattedValue");
    
    tsp00_Longint rc = errOK_els00;

    tsp00_Longint lBytesWritten = 0;
    tsp00_Addr    pszData       = NULL;
    tsp00_Addr    pszBufPtr     = pszBuffer;
    tsp00_Int4    Cnt           = 0;

    SAPDB_Int4           k           = 0;
    // external encoding is the same for all values
    const tsp77encoding* _pEncoding  = pEncoding_ls18[pColumnData->cdExtEncoding_ls00]; 
    SAPDB_Int4           _lCharSize  = _pEncoding->fixedCharacterSize;

    //*
    //*     Insert delimiter at the begin of the string to write
    //*
    if (0 != pPattern->lpatDelimiterLen_ls00)           // no delimiter -> nothing to insert here
    {
        SAPDB_memcpy(pszBufPtr, pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00);
        lBytesWritten += pPattern->lpatDelimiterLen_ls00;
        pszBufPtr     += pPattern->lpatDelimiterLen_ls00;
    }

    //*
    //*     Write values to buffer
    //*
    for (SAPDB_Int4 i = 0; i < lColumnCount; ++i)
    {
        if (true == pColumnData[i].cdPtrUsed_ls00)
        {
            pszData = pColumnData[i].cdPtrToData_ls00;
        }
        else
        {
            pszData = pColumnData[i].cdDataBuffer_ls00;
        }

        //*
        //*     Insert value in right code type and hex if requested
        //* In case of char data
        //*  - double delimiters found in data
        //*  - write new line dependent on operating system running on
//TODOTODO BUFFERLAENGE MUSS NOCH UEBERPRUEFT WERDEN

        if ( (ioChar_els00  == pColumnData[i].cdExtDataType_ls00)  &&
             (0             != pPattern->lpatDelimiterLen_ls00)    &&
             ( (1 == szMap_CharDataType_ls00[pColumnData[i].cdSQLDataType_ls00]) &&
               (1 != szMap_BlobDataType_ls00[pColumnData[i].cdSQLDataType_ls00]) ) )
        {
            for (k = 0; (k < pColumnData[i].cdDataLen_ls00); k += _lCharSize)
            {
                if (ctUTF8_els00 == pColumnData[i].cdExtEncoding_ls00)
                {
                    _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, &pszData[k]));
                }

                if ( 0 == memcmp(&pszData[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00) )
                {
                    SAPDB_memcpy(pszBufPtr, &pszData[k], pPattern->lpatDelimiterLen_ls00);
                    pszBufPtr     += pPattern->lpatDelimiterLen_ls00;
                    lBytesWritten += pPattern->lpatDelimiterLen_ls00;
                }
                else if (0 == memcmp(&pszData[k], _pEncoding->charTable->nl.bytes, _pEncoding->charTable->nl.byteCount))
                {
#if defined(_WIN32)
                    // Put a additional carriage return in for Windows
                    SAPDB_memcpy(pszBufPtr, _pEncoding->charTable->cr.bytes, _pEncoding->charTable->cr.byteCount);
                    pszBufPtr     += _pEncoding->charTable->cr.byteCount;
                    lBytesWritten += _pEncoding->charTable->cr.byteCount;
#endif
                }
                SAPDB_memcpy(pszBufPtr, &pszData[k], _lCharSize);
                pszBufPtr += _lCharSize;
            }
            lBytesWritten += pColumnData[i].cdDataLen_ls00;
        }
        else
        {
            SAPDB_memcpy(pszBufPtr, pszData, pColumnData[i].cdDataLen_ls00);

            lBytesWritten += pColumnData[i].cdDataLen_ls00;
            pszBufPtr     += pColumnData[i].cdDataLen_ls00;
        }

        //*
        //*     Insert value separator(pattern)
        //*
        if (i < (lColumnCount - 1) )
        {
            SAPDB_memcpy(pszBufPtr, pPattern->ppatPattern_ls00, pPattern->lpatPatternLen_ls00);
            lBytesWritten += pPattern->lpatPatternLen_ls00;
            pszBufPtr     += pPattern->lpatPatternLen_ls00;
        }
    }   // end for (int i = 0; i < lColumnCount; ++i)

    //*
    //*     Insert delimiter at the end of the data record
    //*
    if (0 != pPattern->lpatDelimiterLen_ls00)       // no delimiter -> nothing to insert
    {
        SAPDB_memcpy(pszBufPtr, pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00);

        lBytesWritten += pPattern->lpatDelimiterLen_ls00;
    }

    pszBufPtr     = NULL;
    return lBytesWritten;
}
//  ls18PutUnformattedValue()


/*
  -----------------------------------------------------------------------------
  function:     ls18GetCompactValue
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetCompactValue(char              *pString,
                    tls00_MetaData    *pMeta,
                    tsp00_Uint4        lCount,
                    tsp00_Uint4        lPosToRead[],
                    tls00_String       pValue[],
                    tls00_MultColSpec *pMultCol,
                    int               &lMissingIndex)
{
    ROUTINE_DBG_MEO00 ("ls18GetCompactValue");

    tsp00_Int4 rc = errOK_els00;

    tsp00_Uint4 i         = 0;
    size_t      lOffset   = 0;          // defines start of string values
    tsp00_Int4  len       = 0;          // used for handling of long value lengths
    tsp00_Uint4 lMaxPos   = lPosToRead[lCount-1];
    tsp00_Uint4 i_longpos = 0;          // position index of long column pos
    tsp00_Uint4 n_longpos = 0;          // new position index of long column pos used within loop
    tsp00_Uint4 i_pospos  = 0;          // index of positions to read
    
    tls00_ParamInfo_LONG *pLCI    = NULL;
    bool                  bIsLong = false;


    for (i; ( (i < lMaxPos) && (errOK_els00 == rc) ); ++i)
    {
        if (i > pMeta->mdFieldCount_ls00)
        {   // trying to read data fields with position numbers higher
            // than count of fields in data file must fail
            lMissingIndex = i;
            rc = errMissingData_els98;
        }
        else
        {
            // Assign length to value length member; be careful with longs
            if ( 0         != pMeta->mdLongFieldCount_ls00 &&
                 i_longpos <  pMeta->mdLongFieldCount_ls00  )   // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124387
            {
                if ((i+1) == pMeta->pmdLongFieldPos_ls00[i_longpos])
                {
                    // Attention: long length is built using 2 entries of pmdLengthArray_ls00
                    //            to be able to represent values gt 32767 bytes
                    SAPDB_memcpy(&len, &pMeta->pmdLengthArray_ls00[i + i_longpos], sizeof(tsp00_Int4));
                    bIsLong = true;
                }
                else
                {
                    len = pMeta->pmdLengthArray_ls00[i + i_longpos];
                }
            }
            else
            {
                len = pMeta->pmdLengthArray_ls00[i + i_longpos];
            }

            if ((i+1) == lPosToRead[i_pospos])
            {
                pValue[i].strLeng_ls00 = ( (0 == len) && (bIsLong == false) ) ? INSERT_EMPTY_VALUE : len;

                if (bIsLong == false)
                {
                    if ( (INSERT_NULL != pValue[i].strLeng_ls00) && (INSERT_EMPTY_VALUE != pValue[i].strLeng_ls00) )
                    {
                        pValue[i].strAddr_ls00 = &pString[lOffset];
                    }
                }
                else
                {
                    // Adapt long file info for this column!
                    // index i corresponds to index in multcol structure
                    pLCI = &pMultCol->mcsColumn_ls00[i]->colLongInfo;

                    if (INSERT_NULL != pValue[i].strLeng_ls00)
                    {
                        pLCI->loiLength_ls00          = len;
                        pLCI->loiPart_ls00            = true;   // Long value is part of the data file
                        pLCI->loiCountOfSendData_ls00 = 0L;     // count of already send data (to kernel)
                    }

                    if (0 == i_longpos)
                    {
                        pLCI->loiFileOffset_ls00 = pLCI->loiFileStruct_ls00->vfFilePos_ls00;
                    }
                    else
                    {
                        n_longpos = pMeta->pmdLongFieldPos_ls00[i_longpos - 1] - 1;

                        pLCI->loiFileOffset_ls00 =
                                pMultCol->mcsColumn_ls00[n_longpos]->colLongInfo.loiFileOffset_ls00;
                        if (INSERT_NULL != pValue[n_longpos].strLeng_ls00)
                        {
                            ls07AddLargeNumbers(pLCI->loiFileOffset_ls00, pValue[n_longpos].strLeng_ls00);
                        }
                    }
                }

                ++i_pospos;
            }

            if ( (pValue[i].strLeng_ls00 > 0) && (bIsLong == false) )
            {
                lOffset += pValue[i].strLeng_ls00;
            }
            else if (true == bIsLong)
            {
                ++i_longpos;
                bIsLong = false;
            }
        }
    }   // end for (i; ( (i < lMaxPos) && (errOK_els00 == rc) ); ++i)

    return rc;
}
// ls18GetCompactValue()


/*
  -----------------------------------------------------------------------------
  function:     ls18PutCompactValue
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls18PutCompactValue(tls00_Buffer2     *pszMeta,
                    tsp00_Addr         pszBuffer,
                    tsp00_Longint     lBufLen,
                    tls00_ColumnData  *pColumnData,
                    tls00_MultColSpec *pColumnDesc)
{
    ROUTINE_DBG_MEO00 ("ls18PutCompactValue");

    tsp00_Addr    pszData = NULL;
    tsp00_Int4    vlen    = 0;      // value length = 4 Bytes if LONGs and 2 Bytes if not LONGs
    tsp00_Int2    auxlen  = 0;      // used to write value length to meta buffer
    tsp00_Longint rlen    = 0;      // record length


    for (tsp00_Int i = 0; i < pColumnDesc->mcsCount_ls00; ++i, vlen = 0)
    {
        if (true == pColumnData[i].cdPtrUsed_ls00)
        {
            pszData = pColumnData[i].cdPtrToData_ls00;
        }
        else
        {
            pszData = pColumnData[i].cdDataBuffer_ls00;
        }


        if (true == pColumnData[i].cdIsNILValue_ls00)
        {
            // No value to write to buffer; length of INSERT_NULL indicates NULL value
            vlen = INSERT_NULL;
        }
        else
        {
            if (1 == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
            {
                vlen = STATIC_CAST(tsp00_Int4, pColumnDesc->mcsColumn_ls00[i]->colLongInfo.loiLength_ls00);
            }
            else
            {
                vlen = STATIC_CAST(tsp00_Int4, pColumnData[i].cdDataLen_ls00);
                SAPDB_memcpy(pszBuffer, pszData, vlen);

                // Adapt record buffer values
                rlen      += vlen;
                pszBuffer += vlen;
            }
        }

        if (1 == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00])
        {
            SAPDB_memcpy(&(STATIC_CAST(_TCHAR*, pszMeta->pBufPtr_ls00)[pszMeta->lBufPos_ls00]), &vlen, sizeof(tsp00_Int4));
            pszMeta->lBufPos_ls00 += sizeof(tsp00_Int4);
        }
        else
        {
            auxlen = STATIC_CAST(tsp00_Int2, vlen);
            SAPDB_memcpy(&(STATIC_CAST(_TCHAR*, pszMeta->pBufPtr_ls00)[pszMeta->lBufPos_ls00]), &auxlen, sizeof(tsp00_Int2));
            pszMeta->lBufPos_ls00 += sizeof(tsp00_Int2);
        }
    }

    return rlen;
}
// ls18PutCompactValue()


/*
  -----------------------------------------------------------------------------
   function:     ls18GetFormattedValue
  -----------------------------------------------------------------------------
*/
int
ls18GetFormattedValue(tsp00_Addr    pszLine,
                      tsp00_Int4    LineLength,
                      tsp00_Int2&   lStartPos,
                      tsp00_Int2&   lEndPos,
                      tls00_String* Value)
{
    ROUTINE_DBG_MEO00 ("ls18GetFormattedValue");
    
    // The start position starts at 1 in the user file. So we have to subtract 1 from
    // the start position to be on the right position, because C starts counting with 0.
    // In case the StartPos exceeds LineLength simply return. The caller has 
    // to handle this case.

    int rc = errOK_els00;
    int MyStartPos = lStartPos - 1;

    if (MyStartPos <= LineLength)
    {
        // If lEndPos is 0 the length of the value evaluates to 1.
        if (0 == lEndPos)
        {
            Value->strLeng_ls00 = 1;
        }
        else
        {
            Value->strLeng_ls00 = (lEndPos > LineLength) ? (LineLength - MyStartPos) : (lEndPos - MyStartPos);
        }
        Value->strAddr_ls00 = &pszLine[MyStartPos];
    }
    else
    {
        rc = errMissingData_els98;
    }

    return rc;
}
//  ls18GetFormattedValue()

                      
/*
  -----------------------------------------------------------------------------
   function:     ls18GetFormattedValueEx
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18GetFormattedValueEx(tsp00_Addr         pszLine,
                        tsp00_Int4         LineLength,
                        tls00_MultColSpec *pMultCol,
                        tsp00_Uint4        lReadFieldsCnt,
                        tsp00_Uint4        lValToColIndex[],
                        tls00_String       pValue[],
                        tsp00_Int4        &lMissingIndex)
{
    ROUTINE_DBG_MEO00 ("ls18GetFormattedValueEx");
    
    // The start position starts at 1 in the user file. So we have to subtract 1 from
    // the start position to be on the right position, because C starts counting with 0.
    // In case the StartPos exceeds LineLength simply return. The caller has 
    // to handle this case.

    int rc = errOK_els00;
    int spos = 0;
    int epos = 0;

    tls00_Column *pCol = NULL;

    for (tsp00_Uint4 i=0; ( (errOK_els00 == rc) && (i < lReadFieldsCnt) ); ++i)
    {
        pCol = pMultCol->mcsColumn_ls00[lValToColIndex[i]];

        if (asSeqno_els00 == pCol->colAssign_ls00.fasAsgnTyp_ls00)          // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
        {   // the only possible assignment at the moment in IMPORT ... CSV;
            // in this case the length is already set to INSERT_ASSIGNED and should not be changed
            continue;
        }

        spos = pCol->colFPos_ls00.fpoStart_ls00 - 1;
        epos = pCol->colFPos_ls00.fpoEnd_ls00;
        if (spos <= LineLength)
        {
            // If lEndPos is 0 the length of the value evaluates to 1.
            if (0 == epos)
            {
                pValue[i].strLeng_ls00 = 1;
            }
            else
            {
                pValue[i].strLeng_ls00 = (epos > LineLength) ? (LineLength - spos) : (epos - spos);
            }
            pValue[i].strAddr_ls00 = &pszLine[spos];
        }
        else
        {
            rc = errMissingData_els98;
            lMissingIndex = i;
        }
    }

    return rc;
}
// ls18GetFormattedValueEx()


/*
  -----------------------------------------------------------------------------
   function:     ls18PutFormattedValue
  -----------------------------------------------------------------------------
*/
// Kann auch binaere daten ausgeben
tsp00_Longint
ls18PutFormattedValue(tsp00_Addr         pszBuffer,        // points to pszBuffer[StartPos]
                      tsp00_Longint      lBufLen,          // defines valid length EndPos - StartPos + 1
                      tls00_ColumnData  *pColumnData,
                      tls00_MultColSpec *pColumnDesc)
                      
{
    ROUTINE_DBG_MEO00 ("ls18PutFormattedValue");

    tsp00_Addr           pszData  = NULL;
    tsp00_Longint        spos     = 0;      // starting position of single data value in record built here
    tsp00_Longint        vlen     = 0;      // length of single data value
    tsp00_Longint        rlen     = 0;      // overall length of record built here
    tsp00_SwapKind_Enum  CurrSwap = ls07DefineSwap(NULL);

    // In case the file format is FORMATTED we initialize the output buffer will fill
    // bytes to prevent later (time consuming) filling for each value.
    tsp00_DataType tmpDataType;
    tmpDataType.becomes(dcha);

    ls05InsertFillBytes(REINTERPRET_CAST(unsigned char*, pszBuffer),
                        0,
                        tmpDataType,
                        lBufLen,
                        (ctIgnore_els00 < pColumnData->cdExtEncoding_ls00),
                        CurrSwap);


    for (tsp00_Int i = 0; i < pColumnDesc->mcsCount_ls00; ++i)
    {
        if (true == pColumnData[i].cdPtrUsed_ls00)
        {
            pszData = pColumnData[i].cdPtrToData_ls00;
        }
        else
        {
            pszData = pColumnData[i].cdDataBuffer_ls00;
        }

        //*
        //*     Define starting position to write in buffer
        //*
        //* Take into account that strings have to be written left-justified
        //* but numbers right-justified
        //* Additionally define the maximum allowed length for this single value
        //*
        spos = pColumnDesc->mcsColumn_ls00[i]->colFPos_ls00.fpoStart_ls00 - 1;
        vlen = pColumnDesc->mcsColumn_ls00[i]->colFPos_ls00.fpoEnd_ls00 - spos;

        if ( (1 == szMap_NumberDataType_ls00[pColumnData[i].cdSQLDataType_ls00]) &&
             (false == pColumnData[i].cdIsNILValue_ls00)                          )
        {
            spos = pColumnDesc->mcsColumn_ls00[i]->colFPos_ls00.fpoEnd_ls00 - pColumnData[i].cdDataLen_ls00;
        }

        //*
        //*     Insert value
        //*
        if (pColumnData[i].cdDataLen_ls00 < vlen)
        {
            SAPDB_memcpy(&pszBuffer[spos], pszData, pColumnData[i].cdDataLen_ls00);

            // In case of binary fields (like truly hexadecimal) fill the remaining
            // Bytes of the field with null
            if ( (1     == szMap_BlobDataType_ls00[pColumnData[i].cdSQLDataType_ls00]) &&
                 (0     == szMap_LongDataType_ls00[pColumnData[i].cdSQLDataType_ls00]) &&
                 (false == pColumnData[i].cdExtHexRepresentation_ls00)                  )
            {
                tmpDataType.becomes(dfixed);

                ls05InsertFillBytes(REINTERPRET_CAST(unsigned char*, &pszBuffer[spos]),
                                    pColumnData[i].cdDataLen_ls00,
                                    tmpDataType,
                                    vlen,
                                    (ctAscii_els00 != pColumnData->cdExtEncoding_ls00),
                                    CurrSwap);
            }
        }
        else
        {
            SAPDB_memcpy(&pszBuffer[spos], pszData, vlen);
        }

        if (rlen < pColumnDesc->mcsColumn_ls00[i]->colFPos_ls00.fpoEnd_ls00 )
        {
            rlen = pColumnDesc->mcsColumn_ls00[i]->colFPos_ls00.fpoEnd_ls00;
        }
    }   // end for (int i = 0; i < lColumnCount; ++i)

    return rlen;
}
// ls18PutFormattedValue()


/*
  -----------------------------------------------------------------------------
  function:     ls18BindDataToCol
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18BindDataToCol(tsp00_Addr              pszBuffer,
                   tsp00_Int4              BufLength,
                   tls00_TableDescription *pTableDesc,
                   tls00_ColumnData        ColData[],
                   tsp00_Int4              ColCount,
                   bool                    bUnicodeOI,
                   tsp00_Addr              pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls18BindDataToCol");
    
    tsp00_Longint rc = errOK_els00;

    // The start position starts at 1 in the packet buffer. So we have to subtract 1 from
    // the start position to be on the right position.
    // In case the StartPos exceeds LineLength simply return. The caller has 
    // to handle this case.

    // InOutLength includes the def-byte, too.
    // Because numbers are always given binary the value buffer is not NULL terminated
    tsp00_Int4       undef_pos = 0;
    tls00_ParamInfo *pPI       = NULL;

    for (tsp00_Int4 i = 0; ( (i < ColCount) && (errOK_els00 == rc) ); ++i)
    {
        //*
        //*     Define parameter description position in table description structure
        //*
        pPI = pTableDesc->tdParamArray_ls00[ColData[i].cdPosInStmt_ls00];

        undef_pos = pPI->piBufpos_ls00 - 1;
        if (undef_pos <= BufLength)             // Check if position is still in delivered buffer
        {
            if ( UNDEF_BYTE_LS00 == STATIC_CAST(unsigned char, pszBuffer[undef_pos]) )
            {
                //*
                //*     NULL value found
                //*
                ColData[i].cdDataLen_ls00 = INSERT_NULL;
            }
            else
            {
                //*
                //*     REAL value found
                //*
                //* In case of long values we find only the descriptor at this place. The long descriptor
                //* MUST not be further processed here but simply stored for later processing.
                //*

                ColData[i].cdDataLen_ls00   = ColData[i].cdSQLLen_ls00 - 1;
                ColData[i].cdPtrToData_ls00 = &pszBuffer[pPI->piBufpos_ls00];
                ColData[i].cdPtrUsed_ls00   = true;

                //*
                //*     FIRST TRANSFORMATION of delivered value: bring it into 'internal'
                //*     encoding (ascii so far) to process it if the order interface used
                //*     is UNICODE (i.e. UCS2).
                //*
                //* Transformation to 'internal' encoding for values of type
                //*  - date, time, timestamp (value must be brought into the right format and
                //*    transformation routines may only handle ascii data)
                //* No transformation for values of all other types because
                //*  - boolean - delivered as binary 0 or 1 and thus not depending on encoding
                //*  - long - real long values gotten later when processing the descriptor
                //*  - numeric - delivered in internal (db number) representation
                //*  - char/varchar - transformed later with regards to the external representation

                if ( (true == bUnicodeOI)                                                           &&
                     ( (1        == szMap_SpecConstDataType_ls00[ColData[i].cdSQLDataType_ls00]) &&
                       (dboolean != ColData[i].cdSQLDataType_ls00)                                )  )
                {
                    // to indicate in the function call the length of the available buffer for the data
                    ColData[i].cdDataLen_ls00 = ColData[i].cdDataBufferLen_ls00;
                    rc = ls05ConvertFromUCS2(REINTERPRET_CAST(unsigned char*, ColData[i].cdDataBuffer_ls00),
                                             ColData[i].cdDataLen_ls00,
                                             ctAscii_els00,
                                             REINTERPRET_CAST(unsigned char*, ColData[i].cdPtrToData_ls00),
                                             ColData[i].cdSQLLen_ls00 - 1,
                                             ColData[i].cdSQLDataType_ls00,
                                             pszErrText);
                    if (errOK_els00 == rc)
                    {
                        ColData[i].cdPtrUsed_ls00 = false;
                    }
                }
            }

        }   // end if (undef_pos <= BufLength)

    }   // end for (i; ( (i < ColCount) && (errOK_els00 == rc) ); ++i)
    
    return STATIC_CAST(tsp00_Int4, rc);
}
//  ls18BindDataToCol()


/*
  -----------------------------------------------------------------------------
  function:     ls18ConstructPattern
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls18ConstructPattern(tls00_Pattern  *pPattern,
                     tsp00_Addr      pszSeparator,
                     tsp00_Addr      pszDelimiter,
                     tls00_CodeType  ExtEncoding,
                     tsp00_Addr      pszErrText)
{
    ROUTINE_DBG_MEO00 ("ls18ConstructPattern");

    tsp00_Longint rc = errOK_els00;
    // pattern is build this way 'szDelimiter_szSeparator_szDelimiter';
    // example could be ","

    tsp00_Int4 lDelLen = STATIC_CAST(tsp00_Int4, _tcslen(pszDelimiter));
    tsp00_Int4 lPatLen = STATIC_CAST(tsp00_Int4,_tcslen(pszSeparator)) + 2 * lDelLen;


    pPattern->lpatDelimiterLen_ls00 = lDelLen;  // fits for ascii, too
    pPattern->lpatPatternLen_ls00   = lPatLen;  // fits for ascii, too

    if ( true == ls00IsUCS2(&ExtEncoding) )
    {
        pPattern->lpatPatternLen_ls00   = 2*lPatLen;
        pPattern->lpatDelimiterLen_ls00 = 2*lDelLen;
    }
    else if (ctUTF8_els00 == ExtEncoding)
    {
        pPattern->lpatPatternLen_ls00   = 6*lPatLen;        // to be safe
        pPattern->lpatDelimiterLen_ls00 = 6*lDelLen;        // to be safe
    }

    pPattern->ppatPattern_ls00 = new char[pPattern->lpatPatternLen_ls00 + 1];   // terminating 0; makes no sense for ucs2
    pPattern->ppatDelimit_ls00 = new char[pPattern->lpatDelimiterLen_ls00 + 1]; // in case of length 0 only to be sure not working with a null ptr

    //*
    //*     Initialize pattern and delimiter to ascii encoding
    //*
    if (0 != pPattern->lpatDelimiterLen_ls00)
    {
        pPattern->ppatPattern_ls00[0] = pszDelimiter[0];
        pPattern->ppatPattern_ls00[1] = pszSeparator[0];
        pPattern->ppatPattern_ls00[2] = pszDelimiter[0];
        pPattern->ppatPattern_ls00[3] = 0;

        pPattern->ppatDelimit_ls00[0] = pszDelimiter[0];
        pPattern->ppatDelimit_ls00[1] = 0;
    }
    else
    {
        pPattern->ppatPattern_ls00[0] = pszSeparator[0];
        pPattern->ppatPattern_ls00[1] = 0;

        pPattern->ppatDelimit_ls00[0] = 0;
    }

    //*
    //*     Convert to UCS2 or UTF8 if necessary
    //*
    if (true == ls00IsUCS2(&ExtEncoding))
    {
        tsp00_Longuint lDestLen = 0;

        tsp00_Addr pszTmp = new _TCHAR[lPatLen + 1];
        SAPDB_memcpy(pszTmp, pPattern->ppatPattern_ls00, lPatLen);
        pszTmp[lPatLen] = 0;

        lDestLen = pPattern->lpatPatternLen_ls00;
        rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, pPattern->ppatPattern_ls00),
                                     lDestLen,
                                     ExtEncoding,
                                     REINTERPRET_CAST(unsigned char*, pszTmp),
                                     lPatLen,
                                     ctAscii_els00,
                                     pszErrText);
        if (errOK_els00 == rc)
        {
            pPattern->lpatPatternLen_ls00 = STATIC_CAST(tsp00_Longint, lDestLen);

            if (0 != pPattern->lpatDelimiterLen_ls00)
            {
                // Reuse of lDestLen!
                lDestLen = pPattern->lpatDelimiterLen_ls00;
            
                rc = ls05ConvertToUCS2Simple(REINTERPRET_CAST(unsigned char*, pPattern->ppatDelimit_ls00),
                                             lDestLen,
                                             ExtEncoding,
                                             REINTERPRET_CAST(unsigned char*, pszDelimiter),
                                             lDelLen,
                                             ctAscii_els00,
                                             pszErrText);
                if (errOK_els00 == rc)
                {
                    pPattern->lpatDelimiterLen_ls00 = STATIC_CAST(tsp00_Longint, lDestLen);
                }
            }
        }

        if (NULL != pszTmp)
        {
            delete [] pszTmp;
            pszTmp = NULL;
        }
    }
    else if (ctUTF8_els00 == ExtEncoding)
    {
        Tools_DynamicUTF8String Tmp;

        rc = ls05ConvertToUTF8(Tmp,
                               REINTERPRET_CAST(unsigned char*, pPattern->ppatPattern_ls00),
                               lPatLen,
                               ctAscii_els00,
                               pszErrText);
        if (errOK_els00 == rc)
        {
            pPattern->lpatPatternLen_ls00 = STATIC_CAST(tsp00_Longint, Tmp.BasisSize());
            SAPDB_memcpy(pPattern->ppatPattern_ls00, Tmp.StrPtr(), pPattern->lpatPatternLen_ls00);
            pPattern->ppatPattern_ls00[pPattern->lpatPatternLen_ls00] = 0;

            if (0 != pPattern->lpatDelimiterLen_ls00)
            {
                // Reuse of Tmp!
                rc = ls05ConvertToUTF8(Tmp,
                                       REINTERPRET_CAST(unsigned char*, pszDelimiter),
                                       lDelLen,
                                       ctAscii_els00,
                                       pszErrText);
                if (errOK_els00 == rc)
                {
                    pPattern->lpatDelimiterLen_ls00 = STATIC_CAST(tsp00_Longint, Tmp.BasisSize());
                    SAPDB_memcpy(pPattern->ppatDelimit_ls00, Tmp.StrPtr(), pPattern->lpatDelimiterLen_ls00);
                    pPattern->ppatDelimit_ls00[pPattern->lpatDelimiterLen_ls00] = 0;
                }
            }
        }
    }
    //else      // supposing ascii; it's done with initialization of pattern


    //*
    //*     Build pattern fail function
    //*
    if (errOK_els00 == rc)
    {
        pPattern->plpatFailure_ls00 = new tsp00_Int4[pPattern->lpatPatternLen_ls00];

        ls18ComputePatFailFunc(pPattern);
    }
    return STATIC_CAST(tsp00_Int4, rc);
}
// ls18ConstructPattern()


/*
  -----------------------------------------------------------------------------
  function:     ls18ComputePatFailFunc
  -----------------------------------------------------------------------------
*/
void
ls18ComputePatFailFunc(tls00_Pattern *pPattern)
{
    ROUTINE_DBG_MEO00 ("ls18ComputePatFailFunc");

    // Now compute pattern failure function
    tsp00_Addr  pPat   = pPattern->ppatPattern_ls00;
    tsp00_Int4 *plFail = pPattern->plpatFailure_ls00;
    
    plFail[0] = -1;
    tsp00_Int4 i = 0;
    int        j = 1;

    for(j; j < pPattern->lpatPatternLen_ls00; ++j)
    {
        i = plFail[j-1];
        while ( (pPat[j] != pPat[i+1]) && (i >= 0) )
        {
            i = plFail[i];
        }

        if (pPat[j] == pPat[i+1])
        {
            plFail[j] = i+1;
        }
        else
        {
            plFail[j] = -1;
        }
    }

    return;
}
// ls18ComputePatFailFunc()


/*
  -----------------------------------------------------------------------------
  function:     ls18DestructPattern
  -----------------------------------------------------------------------------
*/
void
ls18DestructPattern(tls00_Pattern *pPattern)
{
    ROUTINE_DBG_MEO00 ("ls18DestructPattern");

    if (NULL != pPattern)
    {
        if (NULL != pPattern->ppatPattern_ls00)
        {
            delete [] pPattern->ppatPattern_ls00;
            pPattern->ppatPattern_ls00 = NULL;
        }

        if (NULL != pPattern->ppatDelimit_ls00)
        {
            delete [] pPattern->ppatDelimit_ls00;
            pPattern->ppatDelimit_ls00 = NULL;
        }
        
        if (NULL != pPattern->plpatFailure_ls00)
        {
            delete [] pPattern->plpatFailure_ls00;
            pPattern->plpatFailure_ls00 = NULL;
        }
    }

    return;
}
// ls18DestructPattern()

/*
  -----------------------------------------------------------------------------
  function:     ls18UndoubleDelimitersInCharData
  -----------------------------------------------------------------------------
*/
void
ls18UndoubleDelimitersInCharData(tls00_String*   pValue,
                                 tls00_Pattern*  pPattern,
                                 tls00_CodeType  CodeType)
{
    //*
    //*     Check for delimiters and 'un'double them - this should be of any
    //*     interest only for data meant for char columns of any encoding
    //*
    if (0 != pPattern->lpatDelimiterLen_ls00) 
    {
        SAPDB_UInt  _lCharSize     = sizeof(char);              // initialize for ASCII
        SAPDB_UInt  v              = 0;
        SAPDB_UInt  k              = 0;
        SAPDB_UInt  _lBufLen       = pValue->strLeng_ls00;
        char*       _pszWrkBuffer  = pValue->strAddr_ls00;   // Coming here pValue->strAddr_ls00 is still a ptr into
                                                        // the line read from data file; in case of found delimiters
                                                        // within the data the function will allocate memory for
                                                        // pValue->strAddr_ls00
        if (ctUTF8_els00 != CodeType)
        {
            if (CodeType > ctIgnore_els00)
            {
                _lCharSize = sp77encodingUCS2->fixedCharacterSize;
            }

            // step size is 2 chars
            for (k = _lCharSize; (k < _lBufLen); k += 2*_lCharSize)        // find first delimter
            {
                if (0 == memcmp(&_pszWrkBuffer[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                {
                    if (0 == memcmp(&_pszWrkBuffer[k - _lCharSize], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                    {
                        v = k;
                        k += _lCharSize;
                    }
                    else if ((k + _lCharSize) <= _lBufLen)
                    {
                        if (0 == memcmp(&_pszWrkBuffer[k + _lCharSize], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                        {
                            v  = k + _lCharSize;                  // initialize for next for loop
                            k += 2 * _lCharSize;                  // skip 2nd delimiter
                        }

                    }   // end if ((k+1) <= _lBufLen)

                    if (0 != v)
                    {
                        // As we have found a doubled delimiter we need to allocate space for
                        // this value to copy as the origin (the read line from data file) shouldn't be changed
                        pValue->strAddr_ls00 = new char[pValue->strLeng_ls00];
                        SAPDB_memcpy(pValue->strAddr_ls00, _pszWrkBuffer, v);
                        pValue->strLeng_ls00 -= _lCharSize;
                        break;
                    }
                }   // end if (*pPattern->ppatDelimit_ls00 == _pszWrkBuffer[k])
            }   // end for (k=1; (k < _lBufLen); k+=2)

            // If there was a first delimiter we have to copy the rest, too
            while (k < _lBufLen)
            {
                SAPDB_memcpy(&pValue->strAddr_ls00[v], &_pszWrkBuffer[k], _lCharSize);
                
                if (0 == memcmp(&_pszWrkBuffer[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                {
                    if ( (k + _lCharSize) <= _lBufLen)
                    {
                        if (0 == memcmp(&_pszWrkBuffer[k + _lCharSize], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                        {
                            pValue->strLeng_ls00 -= _lCharSize;
                            k                    += _lCharSize;    // skip 2nd delimiter
                        }
                    }
                }
                k += _lCharSize;
                v += _lCharSize;
            }   // end for (k, v; k < _lBufLen; ++k, ++v)

        }
        else    //if (ctUTF8_els00 == CodeType)
        {
            _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, _pszWrkBuffer));

            // step size is 2 chars
            k = _lCharSize; 
            while (k < _lBufLen)
            {
                if (0 == memcmp(&_pszWrkBuffer[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                {
                    if (0 == memcmp(&_pszWrkBuffer[k - _lCharSize], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                    {
                        v  = k;                // initialize for next for loop
                        k += pPattern->lpatDelimiterLen_ls00;       // skip 2nd (just evaluated char) delimiter
                    }
                    else
                    {
                        k += pPattern->lpatDelimiterLen_ls00;
                        if (k < _lBufLen)
                        {
                            if (0 == memcmp(&_pszWrkBuffer[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                            {
                                v  = k;      // initialize for next for loop
                                k += pPattern->lpatDelimiterLen_ls00;       // skip 2nd (just evaluated char) delimiter
                            }
                        }
                    }   // end if ((k+1) <= _lBufLen)
                    
                    if (0 != v)
                    {
                        // As we have found a doubled delimiter we need to allocate space for
                        // this value to copy as the origin (the read line from data file) shouldn't be changed
                        pValue->strAddr_ls00 = new char[pValue->strLeng_ls00];
                        SAPDB_memcpy(pValue->strAddr_ls00, _pszWrkBuffer, v);
                        pValue->strLeng_ls00 -= pPattern->lpatDelimiterLen_ls00;
                        break;
                    }
                    else
                    {
                        _lCharSize  = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, &_pszWrkBuffer[k]));
                        k          += _lCharSize;
                    }
                }   // end if (0 == memcmp(&_pszWrkBuffer[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                else
                {
                    _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, &_pszWrkBuffer[k]));
                    k += _lCharSize;
                    _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, &_pszWrkBuffer[k]));
                    k += _lCharSize;
                }
            }   // end for (k=1; (k < _lBufLen); k+=2)

            // If there was a first delimiter we have to copy the rest, too
            while (k < _lBufLen)
            {
                _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, &_pszWrkBuffer[k]));

                SAPDB_memcpy(&pValue->strAddr_ls00[v], &_pszWrkBuffer[k], _lCharSize);

                v += _lCharSize;

                if (0 == memcmp(&_pszWrkBuffer[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                {
                    k += pPattern->lpatDelimiterLen_ls00;

                    if (k < _lBufLen)
                    {
                        if (0 == memcmp(&_pszWrkBuffer[k], pPattern->ppatDelimit_ls00, pPattern->lpatDelimiterLen_ls00))
                        {
                            pValue->strLeng_ls00 -= pPattern->lpatDelimiterLen_ls00;
                            k                    += pPattern->lpatDelimiterLen_ls00;    // skip 2nd delimiter
                        }
                    }
                }
                else
                {
                    k += _lCharSize;
                }
            }   // end while (k < _lBufLen)


/*          This is an alternative implementation for UTF8 using the UTF8 string class; the shortcoming is
            only: it is way too slow!

            Tools_DynamicUTF8String _delimUTF8;
            _delimUTF8.AssignRaw(REINTERPRET_CAST(unsigned char*, pPattern->ppatDelimit_ls00), pPattern->lpatDelimiterLen_ls00);


            Tools_DynamicUTF8String srcUTF8;
            srcUTF8.AssignRaw(REINTERPRET_CAST(unsigned char*, _pszWrkBuffer), _lBufLen);

            Tools_DynamicUTF8String destUTF8;

            SAPDB_UInt4 size = srcUTF8.Size();

            // step size is 2 chars
            for (k = 1; (k < size); k += 2)        // find first delimter
            {
                if (0 == _delimUTF8.Compare(srcUTF8.SubStrElem(k, 1)))
                {
                    if (0 == _delimUTF8.Compare(srcUTF8.SubStrElem(k-1, 1)))
                    {
                        v = k;
                        k += 1;
                    }
                    else if ((k + 1) <= size)
                    {
                        if (0 == _delimUTF8.Compare(srcUTF8.SubStrElem(k+1, 1)))
                        {
                            v  = k + 1;                  // initialize for next for loop
                            k += 2;                  // skip 2nd delimiter
                        }

                    }   // end if ((k+1) <= _lBufLen)

                    if (0 != v)
                    {
                        // As we have found a doubled delimiter we need to allocate space for
                        // this value to copy as the origin (the read line from data file) shouldn't be changed
                        pValue->strAddr_ls00 = new char[pValue->strLeng_ls00];

                        destUTF8.Append(srcUTF8, 0, v);
                        pValue->strLeng_ls00 -= _delimUTF8.Size();
                        break;
                    }
                }   // end if (*pPattern->ppatDelimit_ls00 == _pszWrkBuffer[k])
            }   // end for (k=1; (k < _lBufLen); k+=2)



            for (k, v; k < size; ++k, ++v)
            {
                destUTF8.Append(srcUTF8.GetIteratorAt(k), srcUTF8.GetIteratorAt(k+1));
                
                if (0 == _delimUTF8.Compare(srcUTF8.SubStrElem(k, 1)))
                {
                    if ( (k + 1) <= size)
                    {
                        if (0 == _delimUTF8.Compare(srcUTF8.SubStrElem(k+1, 1)))
                        {
                            //pValue->strLeng_ls00 -= 1;
                            ++k;    // skip 2nd delimiter
                        }
                    }
                }
            }   // end for (k, v; k < _lBufLen; ++k, ++v)

            if (0 != v)
            {
                pValue->strLeng_ls00 = destUTF8.BasisSize();
                SAPDB_memcpy(pValue->strAddr_ls00, destUTF8.StrPtr(), pValue->strLeng_ls00);              
            } 
*/

        }   // end else of if (ctUTF8_els00 == CodeType)

    }   // end if (0 != pPattern->lpatDelimiterLen_ls00) 
}
//  ls18UndoubleDelimitersInCharData()


/*!
  -----------------------------------------------------------------------------
  function:     ls18CheckForNLInData
  -----------------------------------------------------------------------------
*/
SAPDB_Bool
ls18CheckForNLInData(tls00_Buffer2* pBuffer, tls00_Pattern* pPattern, tls00_FileFormat* pFileFormat)
{
    const tsp77encoding* _pEncoding  = pEncoding_ls18[pFileFormat->ffoCodeType_ls00];
    SAPDB_Bool           bNLInData   = SAPDB_FALSE;

    if (true == pFileFormat->ffoFormatted_ls00)
    {
        bNLInData = (pFileFormat->ffoBytesPerLine_ls00 > (pBuffer->lBufPos_ls00 + _pEncoding->charTable->nl.byteCount));
    }
    else
    {
        //*
        //*     Faster function in case of ASCII data
        //*
        if (csp_ascii == _pEncoding->EncodingType) {
            return ls18_CheckForNLInDataA(pBuffer, pPattern, pFileFormat);
        }

        tsp77charConst              _Separator;
        char*                       pszBuffer        = STATIC_CAST(char*, pBuffer->pBufPtr_ls00);
        SAPDB_UInt                  _lCharSize       = _pEncoding->fixedCharacterSize;            // Initialize for ASCII
        const tsp77charTable* const _pCharTable      = _pEncoding->charTable;

        // This is the length in Bytes!
        //ATTENTION: A HACK! Consider trailing NULL as last field separator !?
        SAPDB_UInt4                 _myFakedLen      = pBuffer->lBufPos_ls00 + _pCharTable->terminator.byteCount;
        SAPDB_UInt4                 _myStrLen        = 0;
        bool                        bSingleDelimiter = false;
        int                         _state      = START_RECORD;

        if (csp_utf8 == _pEncoding->EncodingType)
        {
            _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, pszBuffer));
        }

        // As the pattern consists of the sequence 'delimiter_separator-delimiter' we may conclude
        // from the pattern the separator
        _Separator.byteCount = pPattern->lpatPatternLen_ls00 - 2*pPattern->lpatDelimiterLen_ls00;
        SAPDB_memcpy(_Separator.bytes, pPattern->ppatPattern_ls00 + pPattern->lpatDelimiterLen_ls00, _Separator.byteCount);

        do
        {
            if (0 == memcmp(pPattern->ppatDelimit_ls00, &pszBuffer[_myStrLen], pPattern->lpatDelimiterLen_ls00))
            {   // DELIMITER FOUND
                switch (_state)
                {
                    case START_RECORD:      // this is the initial state; first char found is a delimiter!
                    {
                        _state = IN_QUOTED_FIELD;
                        break;
                    }
                    case IN_QUOTED_FIELD:
                    {
                        bSingleDelimiter = !bSingleDelimiter;
                        break;
                    }
                    case IN_FIELD:
                    {
                        // could happen as the values might be enclosed in delimiters or not;
                        // in case delimiter is defined and we come here: handle as normal char
                        break;
                    }
                    case START_FIELD:       // somewhere outside a value --> same as START_RECORD
                    {
                        _state = IN_QUOTED_FIELD;
                        break;
                    }
                }
            }
            else if ( (0 == memcmp(_Separator.bytes, &pszBuffer[_myStrLen], _Separator.byteCount)) ||
                      (0 == memcmp(_pCharTable->terminator.bytes, &pszBuffer[_myStrLen], _pCharTable->terminator.byteCount) ) )
            {
                // SEPARATOR FOUND --> except one case this is the transition to a new value
                switch (_state)
                {
                    case START_RECORD:      // this is the initial state; first char found is a separator!
                    {                       // could be the empty value
                        _state = START_FIELD;
                        bSingleDelimiter = false;
                        break;
                    }
                    case START_FIELD:       // somewhere outside a value --> could be an empty value ?
                    {                           // leave the state as it is --> it's a new START_FIELD
                        bSingleDelimiter = false;                   
                        break;
                    }
                    case IN_QUOTED_FIELD:
                    {
                        if (true == bSingleDelimiter)
                        {
                            // if bSingleDelimiter is set to true we have found the separator following
                            // the last delimiter that encloses the value --> define the value and
                            // set state to something like 'OUTSIDE VALUE'

                            _state = START_FIELD;
                            bSingleDelimiter = false;
                        }
                        //else  // we're in a value --> nothing to switch, nothing to do
                        break;
                    }
                    case IN_FIELD:
                    {
                        // found the end of a value --> define the value and
                        // set state to something like 'OUTSIDE VALUE'

                        bSingleDelimiter = false;
                        _state = START_FIELD;
                        break;
                    }
                }
            }
            // EIGENTLICH WERDEN WIR IN DEN DATEN KEIN NEWLINE FINDEN; DA IMMER NUR DATEN BIS NEWLINE GELIEFERT WERDEN
            else if (0 == memcmp(_pCharTable->nl.bytes, &pszBuffer[_myStrLen], _pCharTable->nl.byteCount))
            {   // NL FOUND
                switch (_state)
                {
                    case START_RECORD:      // this is the initial state; first char found is a nl -> empty line !!!
                    {
                        // don't change the state as this means the next character begins a new record
                        break;
                    }
                    case START_FIELD:       // somewhere outside a value --> last value;could be an empty value ?
                    {                               // change the state to START_RECORD
                                                    //  as this means the next character begins a new record
                        _state = START_RECORD;
                        break;
                    }
                    case IN_QUOTED_FIELD:
                    {
                        // nothing to do; handle as normal character if it is inside a value parsing;
                        // in case it is outside (bSingleDelimiter is true) it must be the end of that record as all records are
                        //  per definitionem limited by new lines -> set the state to START_RECORD
                        if (true == bSingleDelimiter)
                        {
                            _state = START_RECORD;
                        }
                        break;
                    }
                    case IN_FIELD:  // last value in record reached;
                    {                               // change the state to START_RECORD
                                                    //  as this means the next character begins a new record
                        _state = START_RECORD;
                        break;
                    }
                }
            }
            else if (0 != _pEncoding->isSpace(&pszBuffer[_myStrLen]))        // isspace stops for \r and \n, too; so handle this earlier
            {
                // SPACE CHAR FOUND
                switch (_state)
                {
                    case START_RECORD:      // this is the initial state; first char found is a space -> might be outside a value
                    {                       // could be the empty value though
                        // example: here begins a new record -->|  "value1","value2",...
                        _state = START_FIELD;
                        break;
                    }
                    case START_FIELD:       // somewhere outside a value --> could be an empty value ?
                    {                               // leave the state as it is???
                        break;
                    }
                    case IN_QUOTED_FIELD:
                    {
                        // nothing to do; handle as normal character
                        break;
                    }
                    case IN_FIELD:
                    {
                        // nothing to do; handle as normal character
                        break;
                    }
                }
            }
            else
            {   // SOMETHING ELSE FOUND
                switch (_state)
                {
                    case START_RECORD:      // this is the initial state; first char found is a non-special character
                    {                       //  --> first value is an undelimited one
                        _state = IN_FIELD;
                        break;
                    }
                    case START_FIELD:       // somewhere outside a value --> next value is an undelimited one
                    {
                        _state = IN_FIELD;
                        //setting ptr to value is done when the separator is found
                        break;
                    }
                    /*case IN_QUOTED_FIELD:     // nothing to do; handle as normal character
                    {
                        break;
                    }*/
                    /*case IN_FIELD:            // nothing to do; handle as normal character
                    {
                        break;
                    }*/
                }
            }
            _myStrLen += _lCharSize;
            if (csp_utf8 == _pEncoding->EncodingType)
            {
                _lCharSize = SAPDB_UTF8Basis::CharacterSize(REINTERPRET_CAST(SAPDB_UTF8*, &pszBuffer[_myStrLen]));
            }
        }
        while ( (START_RECORD != _state) && (_myStrLen < _myFakedLen) );

        if (IN_QUOTED_FIELD == _state) {
            bNLInData = SAPDB_TRUE;
        }

    }   // end else of if (true == pFileFormat->ffoFormatted_ls00)

    if (SAPDB_TRUE == bNLInData) {
        // 'Repair' record: put new line in ('\x0A')
        SAPDB_memcpy (&(STATIC_CAST(char*, pBuffer->pBufPtr_ls00)[pBuffer->lBufPos_ls00]),
                _pEncoding->charTable->nl.bytes, _pEncoding->charTable->nl.byteCount);
        pBuffer->lBufPos_ls00 += _pEncoding->charTable->nl.byteCount;
    }

    return bNLInData;
}
// ls18CheckForNLInData()

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
  function:     ls18_GetCSVDataA
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls18_GetCSVDataA(char          *pString,
                 tsp00_Uint4    lStrLen,
                 tls00_Pattern *pPattern,
                 int            lCount,
                 tsp00_Uint4    lPosToRead[],
                 tls00_CodeType CodeType,
                 tls00_String   pValue[],
                 int           &lMissingIndex)
{
    ROUTINE_DBG_MEO00 ("ls18GetCSVDataA");

    SAPDB_Int4 rc = errOK_els00;

    tls18_CSV_Parser_State  _state           = START_RECORD;
    char*                   pszBuffer        = pString;
    SAPDB_UInt4             _myStrLen        = 0;
    SAPDB_Int4              _myCount         = 0;
    SAPDB_Int4              _myPosToRead     = 0;
    bool                    bSingleDelimiter = false;
    tls00_String            _pIterator;
    _pIterator.strAddr_ls00 = NULL;
    _pIterator.strLeng_ls00 = 0;


    //ATTENTION: A HACK! Consider trailing NULL as last field separator !?
    SAPDB_UInt4             _myFakedLen      = lStrLen + sizeof(char);

    register char           _separator       = pPattern->ppatPattern_ls00[1];
    register char           _delimiter       = pPattern->ppatDelimit_ls00[0];
    if (0 == pPattern->lpatDelimiterLen_ls00)
    {
        _delimiter = 2;     // make it something unprintable
        _separator = pPattern->ppatPattern_ls00[0];
    }


    do
    {
        if (_delimiter == pszBuffer[_myStrLen])
        {   // DELIMITER FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a delimiter!
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen + sizeof(char)];
                    _state = IN_QUOTED_FIELD;
                    break;
                }
                case IN_QUOTED_FIELD:
                {
                    bSingleDelimiter = !bSingleDelimiter;

                    if (true == bSingleDelimiter)
                    {   // we have (perhaps) found a closing delimiter --> compute value length (delim length
                        // is already excluded)
                        if (NULL != _pIterator.strAddr_ls00)
                        {
                            _pIterator.strLeng_ls00 = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                        }
                    }
                    break;
                }
                case IN_FIELD:
                {   // could happen as the values might be enclosed in delimiters or not;
                    // in case delimiter is defined and we come here: handle as normal char
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> same as START_RECORD
                    // reset value pointer
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen + sizeof(char)];
                    _state = IN_QUOTED_FIELD;
                    break;
                }
            }
        }
        else if ( (_separator == pszBuffer[_myStrLen]) || (0 == pszBuffer[_myStrLen]) )
        {
            // SEPARATOR FOUND --> except of one case this is the transition to a new value
            switch (_state)
            {
                case START_RECORD:      
                {   // this is the initial state; first char found is a separator!
                    // empty value; leave the ptr as NULL ptr
                    _pIterator.strAddr_ls00 = NULL;
                    _pIterator.strLeng_ls00   = 0;
                    _state = START_FIELD;

                    bSingleDelimiter = false;
                    break;
                }
                case START_FIELD:       
                {   // somewhere outside a value --> could be an empty value ?
                    // leave the state as it is --> it's a new START_FIELD
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00 = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }

                    bSingleDelimiter = false;                   
                    break;
                }
                case IN_QUOTED_FIELD:
                {
                    if (true == bSingleDelimiter)
                    {
                        // if bSingleDelimiter is set to true we have found the separator following
                        // the last delimiter that encloses the value --> define the value and
                        // set state to something like 'OUTSIDE VALUE'
                        _state = START_FIELD;

                        bSingleDelimiter = false;
                    }
                    //else  // we're in a value --> nothing to switch, nothing to do
                    break;
                }
                case IN_FIELD:
                {   // found the end of a value --> define the value and
                    // set state to something like 'OUTSIDE VALUE'
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00   = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }

                    bSingleDelimiter = false;

                    _state = START_FIELD;
                    break;
                }
            }

            //*
            //*     Assign read value to value array member
            //*
            if (START_FIELD == _state)
            {
                // We have to handle the situation that the user specified the same read positions several times
                // user given positions start with 1 but we count beginning with 0
                while ( (_myCount < lCount) && (_myPosToRead == (lPosToRead[_myCount] - 1)) )
                {
                    if ( (_pIterator.strLeng_ls00 > 0) && (INSERT_ASSIGNED != pValue[_myCount].strLeng_ls00) )
                    {   // leave the string undefined and string length = INSERT_EMPTY_VALUE
                        // if the length is 0
                        pValue[_myCount].strLeng_ls00 = _pIterator.strLeng_ls00;
                        pValue[_myCount].strAddr_ls00 = _pIterator.strAddr_ls00;      // assign the value
                    }
                    ++_myCount;
                }
                ++_myPosToRead;        // for every transition even if the value is not the one to read

                //*
                //*     Generally - this is the transition to a new value!
                //*    
                if (_myStrLen < lStrLen)        // make sure not to assign a value outside the read line
                {
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen + sizeof(char)];
                    _pIterator.strLeng_ls00 = 0;
                }
            }
        }
        else if ('\n' == pszBuffer[_myStrLen])
        {   // NL FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a nl -> empty line !!!
                    // don't change the state as this means the next character begins a new record
                    break;
                }
                case START_FIELD:       
                {   // somewhere outside a value --> last value;could be an empty value ?
                    // change the state to START_RECORD as this means the next character begins a new record
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00   = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }
                    _state = START_RECORD;
                    break;
                }
                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character if it is inside a value parsing;
                    // in case it is outside (bSingleDelimiter is true) it must be the end of that record as all records are
                    //  per definitionem limited by new lines -> set the state to START_RECORD
                    if (true == bSingleDelimiter)
                    {
                        _state = START_RECORD;
                    }
                    break;
                }
                case IN_FIELD:
                {   // last value in record reached;
                    // change the state to START_RECORD as this means the next character begins a new record
                    if (NULL != _pIterator.strAddr_ls00)
                    {
                        _pIterator.strLeng_ls00   = &pszBuffer[_myStrLen] - _pIterator.strAddr_ls00;
                    }
                    _state = START_RECORD;
                    break;
                }
            }
        }
        else if (0 != isspace(pszBuffer[_myStrLen]))        // isspace stops for \r and \n, too; so handle this earlier
        {   // SPACE CHAR FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a space -> might be outside a value
                    // 1. could be the empty value though
                    //    example: here begins a new record -->|  "value1","value2",...
                    // 2. it IS a value if the delimiter is defined empty
                    //    example: here begins a new record -->|    testvalue1,testvalue2,...

                    if (2 != _delimiter) {
                        _state = START_FIELD;
                    }
                    else {
                        _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen];
                        _state = IN_FIELD;
                    }
                    break;
                }
/*                case START_FIELD:
                {   // somewhere outside a value --> could be an empty value?
                    // could be inside a value if the delimiter is defined empty
                    // leave the state as it is???
                    break;
                }
                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
                case IN_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
*/
            }
        }
        else
        {   // SOMETHING ELSE FOUND
            switch (_state)
            {
                case START_RECORD:      
                {   // this is the initial state; first char found is a non-special character
                    //  --> first value is an undelimited one
                    _pIterator.strAddr_ls00 = &pszBuffer[_myStrLen];

                    _state = IN_FIELD;
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> next value is an undelimited one
                    _state = IN_FIELD;
                    //setting ptr to value is done when the separator is found
                    break;
                }
/*                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
                case IN_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
*/
            }
        }

        ++_myStrLen;
    }
    while ( (START_RECORD != _state) && (_myStrLen < _myFakedLen) && (_myCount < lCount) ) ;

    // Check if we found all patterns but should have read some more values
    if (errOK_els00 == rc)
    {
        if (_myCount < lCount)
        {
            lMissingIndex = _myCount;
            rc = errMissingData_els98;
        }
    }

    return rc;
}
// ls18_GetCSVDataA()


/*!
  -----------------------------------------------------------------------------
  function:     ls18_CheckForNLInDataA
  -----------------------------------------------------------------------------
*/
SAPDB_Bool
ls18_CheckForNLInDataA(tls00_Buffer2* pBuffer, tls00_Pattern* pPattern, tls00_FileFormat* pFileFormat)
{
    SAPDB_Bool       bNLInData        = SAPDB_FALSE;
    char*            pszBuffer        = STATIC_CAST(char*, pBuffer->pBufPtr_ls00);

    // This is the length in Bytes!
    //ATTENTION: A HACK! Consider trailing NULL as last field separator !?
    SAPDB_UInt4     _myFakedLen      = pBuffer->lBufPos_ls00 + sizeof(char);
    SAPDB_UInt4     _myStrLen        = 0;
    bool            bSingleDelimiter = false;
    int             _state      = START_RECORD;

    // As the pattern consists of the sequence 'delimiter_separator-delimiter' we may conclude
    // from the pattern the separator
    register char   _separator        = pPattern->ppatPattern_ls00[1];
    register char   _delimiter        = pPattern->ppatDelimit_ls00[0];

    do
    {
        if (_delimiter == pszBuffer[_myStrLen])
        {   // DELIMITER FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a delimiter!
                    _state = IN_QUOTED_FIELD;
                    break;
                }
                case IN_QUOTED_FIELD:
                {
                    bSingleDelimiter = !bSingleDelimiter;
                    break;
                }
                case IN_FIELD:
                {   // could happen as the values might be enclosed in delimiters or not;
                    // in case delimiter is defined and we come here: handle as normal char
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> same as START_RECORD
                    _state = IN_QUOTED_FIELD;
                    break;
                }
            }
        }
        else if ( (_separator == pszBuffer[_myStrLen]) || (0 == pszBuffer[_myStrLen]) )
        {
            // SEPARATOR FOUND --> except one case this is the transition to a new value
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a separator!
                    // could be the empty value
                    _state = START_FIELD;
                    bSingleDelimiter = false;
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> could be an empty value ?
                    // leave the state as it is --> it's a new START_FIELD
                    bSingleDelimiter = false;                   
                    break;
                }
                case IN_QUOTED_FIELD:
                {
                    if (true == bSingleDelimiter)
                    {   // if bSingleDelimiter is set to true we have found the separator following
                        // the last delimiter that encloses the value --> define the value and
                        // set state to something like 'OUTSIDE VALUE'

                        _state = START_FIELD;
                        bSingleDelimiter = false;
                    }
                    //else  // we're in a value --> nothing to switch, nothing to do
                    break;
                }
                case IN_FIELD:
                {   // found the end of a value --> define the value and
                    // set state to something like 'OUTSIDE VALUE'

                    bSingleDelimiter = false;
                    _state = START_FIELD;
                    break;
                }
            }
        }
        // EIGENTLICH WERDEN WIR IN DEN DATEN KEIN NEWLINE FINDEN; DA IMMER NUR DATEN BIS NEWLINE GELIEFERT WERDEN
        else if ('\n' == pszBuffer[_myStrLen])
        {   // NL FOUND
            switch (_state)
            {
                case START_RECORD:
                {    // this is the initial state; first char found is a nl -> empty line !!!
                    // don't change the state as this means the next character begins a new record
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> last value;could be an empty value ?
                    // change the state to START_RECORD as this means the next character begins a new record
                    _state = START_RECORD;
                    break;
                }
                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character if it is inside a value parsing;
                    // in case it is outside (bSingleDelimiter is true) it must be the end of that record as all records are
                    //  per definitionem limited by new lines -> set the state to START_RECORD
                    if (true == bSingleDelimiter)
                    {
                        _state = START_RECORD;
                    }
                    break;
                }
                case IN_FIELD:
                {   // last value in record reached;
                    // change the state to START_RECORD as this means the next character begins a new record
                    _state = START_RECORD;
                    break;
                }
            }
        }
        else if (0 != isspace(pszBuffer[_myStrLen]))        // isspace stops for \r and \n, too; so handle this earlier
        {   // SPACE CHAR FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a space -> might be outside a value
                    // could be the empty value though
                    // example: here begins a new record -->|  "value1","value2",...
                    _state = START_FIELD;
                    break;
                }
/*                case START_FIELD:
                {   // somewhere outside a value --> could be an empty value ?
                    // leave the state as it is???
                    break;
                }
                case IN_QUOTED_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
                case IN_FIELD:
                {   // nothing to do; handle as normal character
                    break;
                }
*/
            }
        }
        else
        {   // SOMETHING ELSE FOUND
            switch (_state)
            {
                case START_RECORD:
                {   // this is the initial state; first char found is a non-special character
                    //  --> first value is an undelimited one
                    _state = IN_FIELD;
                    break;
                }
                case START_FIELD:
                {   // somewhere outside a value --> next value is an undelimited one
                    _state = IN_FIELD;
                    //setting ptr to value is done when the separator is found
                    break;
                }
                /*case IN_QUOTED_FIELD:     // nothing to do; handle as normal character
                {
                    break;
                }*/
                /*case IN_FIELD:            // nothing to do; handle as normal character
                {
                    break;
                }*/
            }
        }
        _myStrLen += sizeof(char);
    }
    while ( (START_RECORD != _state) && (_myStrLen < _myFakedLen) );

    if (IN_QUOTED_FIELD == _state) {
        bNLInData = SAPDB_TRUE;

        // 'Repair' record: put new line in ('\x0A')
        STATIC_CAST(char*, pBuffer->pBufPtr_ls00)[pBuffer->lBufPos_ls00] = '\n';
        pBuffer->lBufPos_ls00 += sizeof(char);
    }

    return bNLInData;
}
// ls18_CheckForNLInDataA()

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/
