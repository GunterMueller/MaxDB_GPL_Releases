/*
  -----------------------------------------------------------------------------
  module: vls24.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  Default and NULL value handling
                
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

#include "hls05.h"
#include "hls07.h"
#include "hls16_condition.h"
#include "hls24.h"
#include "hls25.h"
#include "hls99.h"
#include "hsp30.h"      // fuer spezielle laengenberechnung bei default-werten
#include "hls98msg.h"


/*
Mask describes assignment values and its allowed data types

                                            time            user
                            date    time    stamp   user    group
                    bit nr  mask    mask    mask    mask    mask
    dfixed,           0      0       0       0       0       0
    dfloat,           1      0       0       0       0       0
    dcha,             2      1       1       1       1       1
    dche,             3      1       1       1       1       1
    dchb,             4      1       1       1       1       1
    drowid,           5      0       0       0       0       0
    dstra,            6      0       0       0       0       0
    dstre,            7      0       0       0       0       0
    dstrb,            8      0       0       0       0       0
    dstrdb,           9      0       0       0       0       0
    ddate,           10      1       0       0       0       0
    dtime,           11      0       1       0       0       0
    dvfloat,         12      0       0       0       0       0
    dtimestamp,      13      0       0       1       0       0
    dunknown,        14      0       0       0       0       0
    dnumber,         15      0       0       0       0       0
    dnonumber,       16      0       0       0       0       0
    dduration,       17      0       0       0       0       0
    ddbyteebcdic,    18      0       0       0       0       0
    dlonga,          19      0       0       0       0       0
    dlonge,          20      0       0       0       0       0
    dlongb,          21      0       0       0       0       0
    dlongdb,         22      0       0       0       0       0
    dboolean,        23      0       0       0       0       0
    dunicode,        24      0       0       0       1       1
    ddtfiller1,      25      0       0       0       0       0
    ddtfiller2,      26      0       0       0       0       0
    ddtfiller3,      27      0       0       0       0       0
    ddtfiller4,      28      0       0       0       0       0
    dsmallint,       29      0       0       0       0       0
    dinteger,        30      0       0       0       0       0
    dvarchara,       31      1       1       1       1       1
    dvarchare,       32      1       1       1       1       1
    dvarcharb,       33      1       1       1       1       1
    dstruni,         34      1       1       1       0       0
    dlonguni,        35      0       0       0       0       0
    dvarcharuni,     36      1       1       1       1       1
    dudt,            37      0       0       0       0       0
    dabaptabhandle   38      0       0       0       0       0
*/
static char  szDateMask_ls24[39]      = {0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,0,1,0,0};
static char  szTimeMask_ls24[39]      = {0,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,0,1,0,0};
static char  szTimeStampMask_ls24[39] = {0,0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,0,1,0,0};
static char  szUserMask_ls24[39]      = {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0};
static char  szUserGroupMask_ls24[39] = {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0};


/*
  -----------------------------------------------------------------------------
  Chapter: Private functions declaration
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls24_CheckNILCondition
  -----------------------------------------------------------------------------

  description:  

  arguments:    

  returnvalue:  
  -----------------------------------------------------------------------------
*/

static bool
ls24_CheckNILCondition(tls00_String     &Value,
                       tls00_String      DataLine,
                       tls00_CondSpec   &NILCond,
                       tls00_FileFormat &FileFormat);

/*
  -----------------------------------------------------------------------------
  function:     ls24_SetNullDefault
  -----------------------------------------------------------------------------

  description:  Defines a NULL default value for fastload: the undef-byte is
                included in the inserted string.
                For fix-long fields the string will be filled up to the value
                length in the database given by sp7fi_in_out_l.

  attention:    No length checks done. If the buffer is smaller than the value
                inserted the behavior is undefined.

  arguments:    FieldInfo     [IN]   - field information structure
                pszBuffer     [OUT]  - buffer to be filled
                lBufferLength [IN]   - length of buffer to be filled; not used

  returnvalue:  lLength              - Length of default value
  -----------------------------------------------------------------------------
*/
inline tsp00_Int4
ls24_SetNullDefault(tls00_FieldBasics&  FieldBasicInfo,
                    tsp7_len_type_Enum  LengthType,
                    tsp00_Addr          pszBuffer,
                    tsp00_Int4          lBufferLength);


/*
  -----------------------------------------------------------------------------
  function:     ls24_SetDateDefault
  -----------------------------------------------------------------------------

  description:  Defines a date default value. No def-byte is inserted. The
                default value is given in databases internal presentation
                (i.e. 8 bytes long).
                The default is defined using sqlDatTime function. This function
                will be called every 1000 times. For all other calls a static
                variable is used to hold the date.

  attention:    No length checks done. If the buffer is smaller than the value
                inserted the behavior is undefined.

  arguments:    pszValue     [OUT] - buffer to be filled
                lValLength   [IN]  - length of buffer to be filled; not used

  returnvalue:  lLength              - Length of default value
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls24_SetDateDefault(tsp00_Addr pszValue, tsp00_Int4 lValLength)
{
    ROUTINE_DBG_MEO00 ("ls24_SetDateDefault");

    tsp00_Time  sqlTime;
    sqldattime (*(REINTERPRET_CAST(tsp00_Date*, pszValue)), sqlTime);
    return DATE_MXSP00;
}


/*
  -----------------------------------------------------------------------------
  function:     ls24_SetTimeDefault
  -----------------------------------------------------------------------------

  description:  Defines a time default value. No def-byte is inserted. The
                default value is given in databases internal presentation
                (i.e. 8 bytes long).
                The default is defined using sqlDatTime function.

  attention:    No length checks done. If the buffer is smaller than the value
                inserted the behavior is undefined.

  arguments:    pszValue     [OUT] - buffer to be filled
                lValLength   [IN]  - length of buffer to be filled; not used

  returnvalue:  lLength              - Length of default value
  -----------------------------------------------------------------------------
*/
inline tsp00_Int4
ls24_SetTimeDefault(tsp00_Addr pszValue, tsp00_Int4 lValLength)
{
    ROUTINE_DBG_MEO00 ("ls24_SetTimeDefault");

    tsp00_Date sqlDate;
    sqldattime(sqlDate, *(REINTERPRET_CAST(tsp00_Time*, pszValue)) );         
    return TIME_MXSP00;
}

/*
  -----------------------------------------------------------------------------
  function:     ls24_SetTimeStampDefault
  -----------------------------------------------------------------------------

  description:  Defines a time stamp default value. No def-byte is inserted. The
                default value is given in databases internal presentation
                (i.e. 20 bytes long).
                The default is defined using sqlDatTime and sqlClock functions.

  attention:    No length checks done. If the buffer is smaller than the value
                inserted the behavior is undefined.

  arguments:    pszValue     [OUT] - buffer to be filled
                lValLength   [IN]  - length of buffer to be filled; not used

  returnvalue:  lLength              - Length of default value
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls24_SetTimeStampDefault(tsp00_Addr pszValue, tsp00_Int4 lValLength);


/*
  -----------------------------------------------------------------------------
   function:     ls24_SetUserDefault
  -----------------------------------------------------------------------------

  description:  Defines a user default value. No def-byte is inserted.
                The user default is ALWAYS inserted into columns of variable length.
                This function thus cuts off the leading blanks which results in a
                special length computation.
                The value inserted is defined early when connecting to the database
                and is held by one of the structure members of tls00_PredefDefault.

  attention:    No length checks done. If the buffer is smaller than the value
                inserted the behavior is undefined.
                The memory for the value must already be allocated before calling.

  arguments:    pszValue        [OUT]   - Buffer that will be filled with the user value
                lValLength      [OUT]   - Length of buffer to be filled. Not used so far.
                pszDefaultValue [IN]    - Value to be inserted.

  returnvalue:  lLength               - Length of default value
  -----------------------------------------------------------------------------
*/
inline tsp00_Int4
ls24_SetUserDefault(tsp00_Addr pszValue,
                    tsp00_Int4 lValLength,
                    tsp00_Addr pszDefaultValue)
{
    ROUTINE_DBG_MEO00 ("ls24_SetUserDefault");

    int lLength = s30lnr(pszDefaultValue, '\x20', 1, STATIC_CAST( tsp00_Int4, strlen(pszDefaultValue) ) );
    SAPDB_memcpy(pszValue, pszDefaultValue, lLength);
    return lLength;
}


/*
  -----------------------------------------------------------------------------
   function:     ls24_SetUserGroupDefault
  -----------------------------------------------------------------------------

  description:  Defines a user group default value.
                The user group default is ALWAYS inserted into columns of variable length.
                This function thus cuts off the leading blanks which results in a
                special length computation.
                The value inserted is defined early when connecting to the database
                and is held by one of the structure members of tls00_PredefDefault.

  attention:    No length checks done. If the buffer is smaller than the value
                inserted the behavior is undefined.
                The memory for the value must already be allocated before calling.

  arguments:    pszValue        [OUT]   - Buffer that will be filled with the user value
                lValLength      [OUT]   - Length of buffer to be filled. Not used so far.
                pszDefaultValue [IN]    - Value to be inserted.

  returnvalue:  lLength               - Length of default value
  -----------------------------------------------------------------------------
*/
inline tsp00_Int4
ls24_SetUserGroupDefault(tsp00_Addr pszValue,
                         tsp00_Int4 lValLength,
                         tsp00_Addr pszDefaultValue)
{
    ROUTINE_DBG_MEO00 ("ls24_SetUserGroupDefault");
    int lLength = s30lnr(pszDefaultValue, '\x20', 1, STATIC_CAST( tsp00_Int4, strlen(pszDefaultValue) ) );
    SAPDB_memcpy(pszValue, pszDefaultValue, lLength);
    
    return lLength;
}


/*
  -----------------------------------------------------------------------------
   function:     ls24_GetBooleanConstants
  -----------------------------------------------------------------------------

  description:  Checks values of data type boolean for true/false constants.
                It is assumed that the value delivered for checking is already
                filled with any value.

  arguments:    Value     [OUT]   - Value to check for boolean spec; if the value  
                                     is an boolean, it is adapted to the
                                     respective database value.
                fFormat   [OUT]   - Format description

  returnvalue:  errOK_els00             - no error
                errInvalidBool_els98    - Unrecognized value for boolean data type
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls24_GetBooleanConstants(tls00_String&      Value,
                         tls00_FileFormat&  fFormat);


/*
  -----------------------------------------------------------------------------
   function:     ls24_GetDateTimeConstants
  -----------------------------------------------------------------------------

  description:  Convert a user given date/time/timestamp value into internal format
                (USABLE for FASTLOAD ONLY).
                Because the value delivered by the user may be shorter than the
                internal value in those cases new memory is allocated (with the
                right length) and the value is inserted here. The value pointer
                will then be switched to the new location and the memory must be
                freed by the caller.

  arguments:    pszValue     [IN/OUT]  - User delivered value; contains value
                                          in internal format on return
                lValLength   [OUT]     - Length of delivered value
                FieldInfo    [IN]      - contains all information of the column
                myFormat     [IN]      - Information on the value format.

  returnvalue:  errOK_els00            - success
                errWrongDateTime_els98 - error converting or unknown type
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls24_GetDateTimeConstants(tsp00_Addr          *pszValue,
                          tsp00_Int4          &lValueLength,
                          tsp00_DataType_Enum  DataType,
                          tls00_FileFormat&    myFormat);

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
  function:     ls24InsertDefaultVal
  -----------------------------------------------------------------------------
*/
int
ls24InsertDefaultVal(tls00_FieldBasics&   FieldBasicInfo,
                     tsp7_len_type_Enum   ColLenType,
                     tsp7_default_Enum    ColDefault,
                     tsp00_Addr           pszBuffer,
                     tsp00_Int4           lBufferLength,
                     tls00_PredefDefault *DefaultStruct,
                     tls00_String        *pszOtherDefault)
{
    ROUTINE_DBG_MEO00 ("ls24InsertDefaultVal");

    tsp00_Int4 lValueLen = -1;      // initialize to error condition

    *pszBuffer++ = ls07GetDefByte(FieldBasicInfo.fbDataType_ls00);
    
    switch (ColDefault)
    {
        case sp7_no_default:    // NULL value
        {
            // ls24_SetNullDefault defines own def byte which is the undef byte
            // Additionally the value is in case of fix length filled up to the
            // given length
            lValueLen = ls24_SetNullDefault(FieldBasicInfo, ColLenType, --pszBuffer, lBufferLength);
            break;
        }
        case sp7_default_user:
        {
            lValueLen = 1 + ls24_SetUserDefault(pszBuffer, lBufferLength, DefaultStruct->pdUserDefault);
            break;
        }
        case sp7_default_usergroup:
        {
            lValueLen = 1 + ls24_SetUserGroupDefault(pszBuffer, lBufferLength, DefaultStruct->pdUserGroupDefault);
            break;
        }
        case sp7_default_date:
        {
            lValueLen = 1 + ls24_SetDateDefault(pszBuffer, lBufferLength);
            break;
        }
        case sp7_default_time:
        {
            lValueLen = 1 + ls24_SetTimeDefault(pszBuffer, lBufferLength);
            break;
        }
        case sp7_default_stamp:
        {
            tsp00_Int4 rc = ls24SurrogateIncrease(&DefaultStruct->pdStamp_ls00[3]);
            if (errOK_els00 == rc)
            {
                // Attention; the default value in pdStamp_ls00 contains the def byte already
                SAPDB_memcpy(--pszBuffer, DefaultStruct->pdStamp_ls00.asCharp(), sizeof(DefaultStruct->pdStamp_ls00));
                lValueLen = 9;
            }
            else
            {
                lValueLen = -1;
            }
            break;
        }
        case sp7_default_timestamp:
        {
            lValueLen = 1 + ls24_SetTimeStampDefault(pszBuffer, lBufferLength);
            break;
        }
        case sp7_default_true:
        {
            *pszBuffer = '1';
            lValueLen = 2;
            break;
        }
        case sp7_default_false:
        {
            *pszBuffer = 0;
            lValueLen = 2;
            break;
        }
        case sp7_other_default:
        {
            //*
            //* User defined default:
            //* pszOtherDefault contains the right value including the def-byte and
            //* the right length for fix- and variable length cols.
            //* In case of var length cols the length is the value length after trimming
            //* it (delivered by the kernel). It does not necessarily equal fbInOutLength_ls00.
            //* No codeset conversion necessary: value is delivered using the kernel codeset!!!
            //*
            assert(pszOtherDefault->strAddr_ls00 != NULL);  // Assert here because it defaults to NULL - see prototype

            // Because we fill the buffer for every value with the def-byte 
            // we have to move the pointer back by one.
            // Delivered default value always includes the def-byte !
            lValueLen = pszOtherDefault->strLeng_ls00;
            SAPDB_memcpy(--pszBuffer, pszOtherDefault->strAddr_ls00, lValueLen);
            
            break;
        }
        default:
        {
            // not implemented yet are 
            //  sp7_default_serial
            //  sp7_default_uid

            lValueLen = -1;     // Unknown or not implemented default
            break;
        }
    }
    
    return lValueLen;
}
//  ls24InsertDefaultVal()


/*
  -----------------------------------------------------------------------------
  function:     ls24InsertAssignedVal
  -----------------------------------------------------------------------------
*/
int ls24InsertAssignedVal(tls00_Column*         pColumn,
                          tls00_FieldBasics&    FieldBasicInfo,
                          tsp00_Addr            pszValue,
                          tsp00_Int4            lBufferLength,
                          tls00_PredefDefault   *DefaultStruct)
{
    ROUTINE_DBG_MEO00 ("ls24_InsertAssignedVal");
    
    tsp00_Int4      lLength = errIncompatibleConst_els98;      // Initialize to error condition
    tsp00_DataType  DType   = FieldBasicInfo.fbDataType_ls00;

    switch (pColumn->colAssign_ls00.fasAsgnTyp_ls00)
    {
        case asDate_els00:
        {
            if ( (1 == szDateMask_ls24[DType]) && (FieldBasicInfo.fbLength_ls00 >= DATE_MXSP00) )
            {
                lLength = ls24_SetDateDefault(pszValue, lBufferLength);
            }
            break;
        }
        case asTime_els00:
        {
            if ( (1 == szTimeMask_ls24[DType]) && (FieldBasicInfo.fbLength_ls00 >= TIME_MXSP00) )
            {
                lLength = ls24_SetTimeDefault(pszValue, lBufferLength);
            }
            break;
        }
        case asTimeStamp_els00:
        {
            if ( (1 == szTimeStampMask_ls24[DType]) && (FieldBasicInfo.fbLength_ls00 >= TIMESTAMP_MXSP00) )
            {
                lLength = ls24_SetTimeStampDefault(pszValue, lBufferLength);
            }
            break;
        }
        case asUser_els00:
        {
            if ( (1 == szUserMask_ls24[DType]) && (FieldBasicInfo.fbLength_ls00 >= KNL_IDENTIFIER_SIZE ) )
            {
                lLength = ls24_SetUserDefault(pszValue, lBufferLength, DefaultStruct->pdUserDefault);
            }
            break;
        }
        case asUsergroup_els00:
        {
            if ( (1 == szUserMask_ls24[DType]) && (FieldBasicInfo.fbLength_ls00 >= KNL_IDENTIFIER_SIZE) )
            {
                lLength = ls24_SetUserGroupDefault(pszValue, lBufferLength, DefaultStruct->pdUserGroupDefault);
            }
            break;
        }
        case asSeqno_els00:     // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129727
        {
            if (1 == szMap_NumberDataType_ls00[DType])
            {
                pColumn->colFormat_ls00.ffoFormat_ls00 = ioInteger_els00;
        
                tls00_FileFormat _auxFileFormat;
                memset(&_auxFileFormat, 0, sizeof(tls00_FileFormat));
                _auxFileFormat.ffoIntSpec_ls00.becomes(SERVER_SWAPPING_LS00);

                SAPDB_Char* _pszSeqNo = reinterpret_cast<SAPDB_Char*>(&pColumn->colAssign_ls00.fasSeqNo_ls00);
                SAPDB_Int4 _len  = sizeof(pColumn->colAssign_ls00.fasSeqNo_ls00);

                SAPDB_Int4 rcNO = ls25PrepareNumbers(&_pszSeqNo,
                                                     _len,
                                                     *pColumn,
                                                     FieldBasicInfo,
                                                     SERVER_SWAPPING_LS00, //pFLRec->flrSwapKind_ls00,
                                                     _auxFileFormat);
                if (errOK_els00 != rcNO)
                {
                    lLength = rcNO;
                }
                else
                {
                    memcpy(pszValue, _pszSeqNo, _len);
                    lLength = _len;
                    delete [] _pszSeqNo;

                    pColumn->colAssign_ls00.fasSeqNo_ls00 += pColumn->colAssign_ls00.fasSeqInc_ls00;
                }
            }
        }
        /*default:
        {
            // unknown type of assingment; should rather not happen because it is already catched in parser
            // default is set with initializing the return value to error
        }*/
    }

    return lLength;
}
//  ls24InsertAssignedVal()


/*
  -----------------------------------------------------------------------------
  function:     ls24SpecialFormat
  -----------------------------------------------------------------------------
*/
int ls24SpecialFormat(tls00_String&         Value,
                      tls00_FieldBasics&    FieldBasicInfo,
                      tls00_FileFormat&     FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls24SpecialFormat");

    int lResult = errOK_els00;
    
    //*
    //*     - Check if boolean values are specified.
    //*     - Prepare Date/Time/Timestamp values for insert.
    //*
    switch (FieldBasicInfo.fbDataType_ls00)
    {
        case dboolean:
        {

            lResult = ls24_GetBooleanConstants(Value, FileFormat);
            break;
        }
        case ddate:
        case dtime:
        case dtimestamp:
        {
            lResult = ls24_GetDateTimeConstants(&Value.strAddr_ls00,
                                                Value.strLeng_ls00,
                                                FieldBasicInfo.fbDataType_ls00,
                                                FileFormat);
        }
    }

    return lResult;
}
//  ls24SpecialFormat()


/*
  -----------------------------------------------------------------------------
  function:     ls24ExtractDateTime
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls24ExtractDateTime(tsp00_Addr           pszSrcValue,
                    tsp00_Addr           pszDestValue,
                    tsp00_Longint       &lDestValueLength,
                    tsp00_DataType_Enum  DataType,
                    tls00_FileFormat    *FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls24ExtractDateTime");
    
    tsp00_Int4 rc = errOK_els00;

    tin_date_mask OutputMask;
    
    switch (DataType)
    {
        case ddate:
        {
            i41dput(FileFormat->ffoSpecialDateMask_ls00, pszSrcValue, OutputMask);
            break;
        }
        case dtime:
        {
            i41tput(FileFormat->ffoSpecialTimeMask_ls00, pszSrcValue, OutputMask);
            break;
        }
        case dtimestamp:
        {
            i41tsput(FileFormat->ffoSpecialTimeStampMask_ls00, pszSrcValue, OutputMask);
            break;
        }
        default:        // Error!!! Should actually not happen if the function is properly used.
        {
            rc = -1;
        }
    }

    SAPDB_memcpy(pszDestValue, OutputMask.msk, lDestValueLength = OutputMask.mlen);

    return rc;
}
// ls24ExtractDateTime()


/*
  -----------------------------------------------------------------------------
  function:     ls24GetNILConstants - overloaded (FOR FASTLOAD)
  -----------------------------------------------------------------------------
*/
int ls24GetNILConstants(tls00_String&       Value,
                        tls00_String        DataLine,
                        tsp00_ColMode_Enum  ColumnMode,
                        tls00_CondSpec&     NILCond,
                        tsp7_default_Enum   Default,
                        tls00_FileFormat &  FileFormat)
{
    assert(mod_dropped != ColumnMode);

    ROUTINE_DBG_MEO00 ("ls24GetNILConstants (Fastload)");

    int  lResult = errOK_els00;

    bool bNull = ls24_CheckNILCondition(Value, DataLine, NILCond, FileFormat);

    if (true == bNull)
    {
        // We're checking here ColumnMode that may have the following values:
        // mod_syskey, mod_key, mod_mandatory, mod_optional and mod_dropped
        // mod_syskey and mod_key:  those columns are key columns; should not appear at this place
        // mod_mandatory:           Not NULL column; But because we insert in case of nut null
        //                          with default the default value we have to check here for
        //                          a default
        // mod_optional:            column allows NULL-values
        // mod_dropped:             shouldn't appear at this place
        switch (ColumnMode)
        {
            case mod_optional:
            {
                Value.strLeng_ls00 = INSERT_NULL;
                break;
            }
            case mod_mandatory:
            case mod_syskey:
            case mod_key:
            {
                if (sp7_no_default == Default)
                {
                    lResult = errNullNotAllowed_els98;
                }
                else
                {
                    Value.strLeng_ls00 = INSERT_DEFAULT;        // NULL not allowed but default defined
                }
                break;
            }
        }
    }
    //else
        // means the user did not specifiy a NULL value but a real value;
        // this is handled with errOK_els00 and must be handled additionally by the caller

    return lResult;
}
//  ls24GetNULLConstants()


/*
  -----------------------------------------------------------------------------
  function:     ls24GetNILConstants - overloaded (FOR DATALOAD)
  -----------------------------------------------------------------------------
*/
int ls24GetNILConstants(tls00_String&       Value,
                        tls00_String        DataLine,
                        tsp1_param_opt_set  ColumnMode,
                        tls00_CondSpec&     NILCond,
                        tls00_FileFormat&   FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls24GetNILConstants (Dataload)");

    int  lResult = errOK_els00;

    bool bNull = ls24_CheckNILCondition(Value, DataLine, NILCond, FileFormat);

    if (true == bNull)
    {
        // If there is a default defined we 
        if (true == ColumnMode.includes(sp1ot_default))
        {
            Value.strLeng_ls00 = INSERT_DEFAULT;
        }
        if ( true == ColumnMode.includes(sp1ot_optional) )
        {
            Value.strLeng_ls00 = INSERT_NULL;
        }
        else if (true == ColumnMode.includes(sp1ot_mandatory))
        {
            if (false == ColumnMode.includes(sp1ot_default))
            {
                lResult = errNullNotAllowed_els98;
            }
            else
            {
                Value.strLeng_ls00 = INSERT_DEFAULT;
            }
        }   
    }
    //else
        // means the user did not specifiy a NULL value but a real value;
        // this is handled with errOK_els00 and must be handled additionally by the caller

    return lResult;
}
//  ls24GetNILConstants()



// Used to increase key and stamp values: the significant numbers are 6
/*
  -----------------------------------------------------------------------------
  function:     ls24SurrogateIncrease
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls24SurrogateIncrease(unsigned char* pValue)
{
    ROUTINE_DBG_MEO00 ("ls24SurrogateIncrease");

    // we operate with constant length of 6 - this is the count of significant numbers in key/stamp values
    int lLength = 6;

    unsigned char* p = &pValue[lLength - 1];   // Move pointer to the end of value

    //*
    //*     In case the leftmost possible byte to set is 255 check for the
    //*     rest of the bytes to see if they are also set to 255.
    //*     In this case stop delivering key values - the highest value is
    //*     reached (2 to the power of 48 possibilities).
    //*
    if (0xFF == *pValue)
    {
        int i = 1;
        for (i; ( (i < lLength) && (0xFF == pValue[i]) ); ++i)
            ;

        if (i == lLength)
        {
            return errSyskeyExceeded_els98;
        }
    }

    if (*p == 255)      //  In case last byte (Key[7]) = FF, it may not become 0 BUT 1 (requested by db)
    {
        *p-- = 1;
        lLength--;
    }

    while ((lLength--) > 0)     //  Set the bytes.
    {
        if (*p < 255)
        {
            *p += 1;
            break;
        }
        else
            *p-- = 1;
    }

    return errOK_els00;
}
// ls24SurrogateIncrease()

/*
  -----------------------------------------------------------------------------
  EndChapter: Public functions implementation
  -----------------------------------------------------------------------------
*/



/*
  -----------------------------------------------------------------------------
  Chapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls24_CheckNILCondition
  -----------------------------------------------------------------------------
*/
static bool
ls24_CheckNILCondition(tls00_String     &Value,
                       tls00_String      DataLine,
                       tls00_CondSpec   &NILCond,
                       tls00_FileFormat &FileFormat)
{
    ROUTINE_DBG_MEO00 ("ls24_CheckNILCondition");

    bool bNull = false;

    if (true == NILCond.cosDefNull_ls00)
    {
        // In case the DEFAULT NULL spec has been chosen for the column a simple
        // comparison with the global NULL spec is done. Upon comparison the shorter
        // value is filled with blanks.

        if (INSERT_EMPTY_VALUE == Value.strLeng_ls00)
        {
            bNull = ls05StrCmpPadded(REINTERPRET_CAST(SAPDB_Byte*, NILCond.cosCond_ls00->conCompVal_ls00),
                                     NILCond.cosCond_ls00->conValLen_ls00,
                                     REINTERPRET_CAST(SAPDB_Byte*, Value.strAddr_ls00),
                                     0,
                                     FileFormat.ffoCodeType_ls00);

        }
        else
        {
            bNull = ls05StrCmpPadded(REINTERPRET_CAST(SAPDB_Byte*, NILCond.cosCond_ls00->conCompVal_ls00),
                                     NILCond.cosCond_ls00->conValLen_ls00,
                                     REINTERPRET_CAST(SAPDB_Byte*, Value.strAddr_ls00),
                                     Value.strLeng_ls00,
                                     FileFormat.ffoCodeType_ls00);
        }
    }
    else
    {
        // In case a special NULL condition is specified for the column we call a routine
        // that handles more complex comparisons where logical operators may be involved.
        bNull = ls16CheckCondition(NILCond, DataLine, FileFormat);
    }
    return bNull;
}
// ls24_CheckNILCondition()


/*
  -----------------------------------------------------------------------------
  function:     ls24_SetNullDefault
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls24_SetNullDefault(tls00_FieldBasics&  FieldBasicInfo,
                    tsp7_len_type_Enum  LengthType,
                    tsp00_Addr          pszBuffer,
                    tsp00_Int4          lBufferLength)
{
    ROUTINE_DBG_MEO00 ("ls24_SetNullDefault");

    tsp00_Int4  lLength = 1;        // This length corresponds to the length of the 
                                    // col if it is of any other type than sp7_fix_len
    *pszBuffer++ = (char)csp_undef_byte;
            
    if (sp7_fix_len == LengthType)
    {
        memset(pszBuffer, '\0', FieldBasicInfo.fbInOutLength_ls00);  // Fill buffer with zeroes
        lLength = FieldBasicInfo.fbInOutLength_ls00;                 // Length is in_out_length of that field
    }
    return lLength;
}
// ls24_SetNullDefault()

/*
  -----------------------------------------------------------------------------
  function:     ls24_SetTimeStampDefault
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls24_SetTimeStampDefault(tsp00_Addr pszValue, tsp00_Int4 lValLength)
{
    ROUTINE_DBG_MEO00 ("ls24_SetTimeStampDefault");
    
    //*
    //*     This functionality is somewhat tricky:
    //*     A timestamp value is only 20 bytes long. But adding all - time, date 
    //*     and microseconds - we would get a string that is 22 bytes long. The time
    //*     value is 8 bytes long where the first 2 bytes are filled with 00. These
    //*     must be skipped and only the 'last' 6 bytes of the time value are used.
    //*
    //*     And the microseconds are not the real value for the time one would get
    //*     using a clock. The used value gotten by a call to sqlclock is the
    //*     microsecond-part of the time that is elapsed since this process has started.
    //*     (the LZU assures that this value will never exceed 999999). This assures
    //*     at least a unique value.
    //*
    tsp00_Int4  sec, microsec;
    tsp00_Time  sqlTime;
        
    // Get date and time. The date value is already the first string saved in 
    // the default value.
    sqldattime(*(REINTERPRET_CAST(tsp00_Date*, pszValue)), sqlTime);
    
    sqlclock(&sec, &microsec);      // Here only the microseconds are of interest.
    assert(microsec < 1000000);     // To make sure the LZU is right.
    
    // Time value: Skip the first two bytes (00) of the time value.
    SAPDB_memcpy(pszValue + DATE_MXSP00, sqlTime + 2, TIME_MXSP00 - 2);
    
    // Timestamp value
    sprintf(pszValue + (TIMESTAMP_MXSP00 - 6), "%06ld", microsec);
    
    return TIMESTAMP_MXSP00;
}
// ls24_SetTimeStampDefault()


/*
  -----------------------------------------------------------------------------
   function:     ls24_GetBooleanConstants
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls24_GetBooleanConstants(tls00_String&      Value,
                         tls00_FileFormat&  fFormat)
{
    ROUTINE_DBG_MEO00 ("ls24_GetBooleanConstants");


    if (true == ls05StrCmpWFill(fFormat.ffoTrueVal_ls00, fFormat.ffoTValLen_ls00,
                                Value.strAddr_ls00,      Value.strLeng_ls00))
    {
        Value.strAddr_ls00[0] = (char)1;
        Value.strLeng_ls00    = 1;
    }
    else if (true == ls05StrCmpWFill(fFormat.ffoFalseVal_ls00, fFormat.ffoFValLen_ls00,
                                     Value.strAddr_ls00,       Value.strLeng_ls00))
    {
        Value.strAddr_ls00[0] = 0;
        Value.strLeng_ls00    = 1;
    }
    else
    {
        return errInvalidBool_els98;
    }

    return errOK_els00;
}
// ls24_GetBooleanConstants()


/*
  -----------------------------------------------------------------------------
   function:     ls24_GetDateTimeConstants
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls24_GetDateTimeConstants(tsp00_Addr           *pszValue,
                          tsp00_Int4           &lValueLength,
                          tsp00_DataType_Enum   DataType,
                          tls00_FileFormat&     myFormat)
{
    ROUTINE_DBG_MEO00 ("ls24_GetDateTimeConstants");

    tsp00_Int4   lError        = errOK_els00;
    
    _TCHAR       szTmp[TIMESTAMP_MXSP00];
    tsp00_Uint2  lNeededLength = DATE_MXSP00;      // Default. Overwritten for time and timestamp
                                                  // This is the length the value will have after
                                                  // transforming it to internal presentation

    // Trim values before further processing
    if (BLANK_LS00 == ((*pszValue)[lValueLength-1]))
    {
        ls05StrTrimRight(*pszValue, lValueLength);
    }

    if (BLANK_LS00 == *pszValue[0])
    {
        tsp00_Longint l = lValueLength;
        
        ls05StrTrimLeftEx(*pszValue, l);
        lValueLength = STATIC_CAST(tsp00_Int4, l);
    }

    switch (DataType)
    {
        case ddate:
        {
            i41dget(myFormat.ffoSpecialDateMask_ls00, *pszValue, lValueLength, szTmp, lError);
            break;
        }
        case dtime:
        {
            lNeededLength = TIME_MXSP00;
            i41tget(myFormat.ffoSpecialTimeMask_ls00, *pszValue, lValueLength, szTmp, lError);
            break;
        }
        case dtimestamp:
        {
            lNeededLength = TIMESTAMP_MXSP00;
            i41tsget(myFormat.ffoSpecialTimeStampMask_ls00, *pszValue, lValueLength, szTmp, lError);
            break;
        }
        default:        // Error!!! Should actually not happen if the function is properly used.
        {
            lError = -1;
        }
    }

    if (errOK_els00 != lError)
    {
        lError = errWrongDateTime_els98;
    }
    else
    {
        if (lValueLength < lNeededLength)
        {
            // We need to return a persistent char pointer; this should happen rather seldom
            _TCHAR* pszBuffer = new _TCHAR[lNeededLength];
            SAPDB_memcpy(pszBuffer, szTmp, lNeededLength);
            *pszValue = pszBuffer;                        // Switch the pointer to new location.
        }
        else
        {
            if (0 != strncmp(*pszValue, szTmp, lNeededLength))
            {
                SAPDB_memcpy(*pszValue, szTmp, lNeededLength);  // Use existing pointer.
            }
        }
        lValueLength = lNeededLength;                     // Assign right length for value.
    }

    return lError;
}
// ls24_GetDateTimeConstants()

/*
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/
