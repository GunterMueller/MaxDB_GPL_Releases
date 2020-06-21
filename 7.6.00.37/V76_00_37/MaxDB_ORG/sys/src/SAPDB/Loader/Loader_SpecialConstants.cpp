/*!
  \file    Loader_SpecialConstants.cpp
  \author  SteffenS
  \brief   Implementation of conversion of special constants

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

\endif
*/
#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Common.hpp"
#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_SpecialConstants.hpp"
#include "Loader/Loader_StringConversion.hpp"

#include "heo02.h"      // definition of function sqldattime
#include "vin00.h"      // date and time conversion


static LOADER_Int4 setTimeStampDefault(LOADER_Byte*  pszBuffer);



// -----------------------------------------------------------------------------
// function:     setTimeStampDefault
// -----------------------------------------------------------------------------
LOADER_Int4
setTimeStampDefault(LOADER_Byte* pszBuffer)
{
    //*
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
    LOADER_Int4  sec, microsec;
    tsp00_Time  sqlTime;
        
    // Get date and time. The date value is already the first string saved in 
    // the default value.
    sqldattime(*(REINTERPRET_CAST(tsp00_Date*, pszBuffer)), sqlTime);
    
    sqlclock(&sec, &microsec);      // Here only the microseconds are of interest.

    // Time value: Skip the first two bytes (00) of the time value.
    SAPDB_memcpy(pszBuffer + DATE_MXSP00, sqlTime + 2, TIME_MXSP00 - 2);

//TODOTODO tuning potential:
    // Timestamp value
    sp77sprintf(reinterpret_cast<LOADER_Char*>(pszBuffer) + (TIMESTAMP_MXSP00 - 6), 6, "%06ld", microsec);
    
    return TIMESTAMP_MXSP00;
}
// setTimeStampDefault()


/*
  -----------------------------------------------------------------------------
  function:     increaseSysKey
  -----------------------------------------------------------------------------
*/
LOADER_ErrorCode
Loader_SpecialConstants::increaseSysKey(LOADER_Byte* pValue)
{
    // we operate with constant length of 6 - this is the count of significant numbers in key/stamp values
    LOADER_Int4 lLength = 6;

    LOADER_Byte* p = &pValue[lLength - 1];   // Move pointer to the end of value

    //*
    //*     In case the leftmost possible byte to set is 255 check for the
    //*     rest of the bytes to see if they are also set to 255.
    //*     In this case stop delivering key values - the highest value is
    //*     reached (2 to the power of 48 possibilities).
    //*
    if (0xFF == *pValue)
    {
        LOADER_Int4 i = 1;
        for (i; ( (i < lLength) && (0xFF == pValue[i]) ); ++i)
            ;

        if (i == lLength)
        {
            return LOA_ERR_SYSKEY_EXCEEDED;
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

    return LOA_ERR_OK;
}
// increaseSysKey()


/*
  -----------------------------------------------------------------------------
  function:     convertToBoolean
  -----------------------------------------------------------------------------
*/
LOADER_ErrorCode
Loader_SpecialConstants::convertToBoolean(Loader_ColumnData*  pColumnData)
{
    LOADER_ErrorCode rc = LOA_ERR_OK;

    switch (pColumnData->HostType)
    {
        case LOADER_HostType_UINT1:
        {
            if (0 != *reinterpret_cast<LOADER_UInt1*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_INT1:
        {
            if (0 != *reinterpret_cast<LOADER_Int1*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_UINT2:
        {
            if (0 != *reinterpret_cast<LOADER_UInt2*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_INT2:
        {
            if (0 != *reinterpret_cast<LOADER_Int2*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_UINT4:
        {
            if (0 != *reinterpret_cast<LOADER_UInt4*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_INT4:
        {
            if (0 != *reinterpret_cast<LOADER_Int4*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_UINT8:
        {
            if (0 != *reinterpret_cast<LOADER_UInt8*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_INT8:
        {
            if (0 != *reinterpret_cast<LOADER_Int8*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_DOUBLE:
        {
            if(0.0 == *reinterpret_cast<double*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        case LOADER_HostType_FLOAT:
        {
            if(0.0f == *reinterpret_cast<float*>(pColumnData->pPtrToData))
            {
                pColumnData->pDataBuffer[0] = 1;
            }
            else
            {
                pColumnData->pDataBuffer[0] = 0;
            }
            break;
        }
        default:
        {
            rc = LOA_ERR_INVALID_HOSTTYPE;
        }
    }

    return rc;
}
// convertToBoolean()


/*
  -----------------------------------------------------------------------------
  function:     convertDateTimeValues
  -----------------------------------------------------------------------------
*/
// All possible kernel format (ISO, EUR, USA, JIS(, INTERNAL) ) may be converted to INTERNAL
LOADER_Int4
Loader_SpecialConstants::convertDateTimeValues(Loader_ColumnData*     pColumnData,
                                               LOADER_DateTimeMask*   pDateTimeMask,
                                               LOADER_DateTimeFormat  DateTimeFormat)
{
    LOADER_Int4   lError        = 0;
    LOADER_UInt2  _len_internal = DATE_MXSP00;      // Default. Overwritten for time and timestamp
                                                    // This is the length the value will have after
                                                    // transforming it to internal presentation
    LOADER_Byte*  _szPtr        = (LOADER_TRUE == pColumnData->PtrUsed_ls00) ? pColumnData->pPtrToData : pColumnData->pDataBuffer;

    // Coming here the values are actually not modified, only trimmed right as they are
    // ALWAYS of a CHAR hosttype --> we do not trim right again

    if (LOADER_BLANK == _szPtr[0])
    {
        LOADER_StrTrimLeftEx(reinterpret_cast<LOADER_Char*>(_szPtr), pColumnData->lDataLength);
    }

    if (LOADER_TRUE == pColumnData->PtrUsed_ls00)       // no need to copy the result
    {
        switch (pColumnData->SQLColumnInfo.ColumnBasics.SQLDataType)
        {
            case LOADER_SQLTYPE_DATE:
            {
                i41dget(*pDateTimeMask,
                        reinterpret_cast<LOADER_Char*>(pColumnData->pPtrToData),
                        pColumnData->lDataLength,
                        reinterpret_cast<LOADER_Char*>(pColumnData->pDataBuffer),
                        lError);
                break;
            }
            case LOADER_SQLTYPE_TIME:
            {
                _len_internal = TIME_MXSP00;
                i41tget(*pDateTimeMask,
                        reinterpret_cast<LOADER_Char*>(pColumnData->pPtrToData),
                        pColumnData->lDataLength,
                        reinterpret_cast<LOADER_Char*>(pColumnData->pDataBuffer),
                        lError);
                break;
            }
            case LOADER_SQLTYPE_TIMESTAMP:
            {
                _len_internal = TIMESTAMP_MXSP00;
                i41tsget(*pDateTimeMask,
                         reinterpret_cast<LOADER_Char*>(pColumnData->pPtrToData),
                         pColumnData->lDataLength,
                         reinterpret_cast<LOADER_Char*>(pColumnData->pDataBuffer),
                         lError);
                break;
            }
            default:        // Error!!! Should actually not happen if the function is properly used.
            {
                lError = -1;
            }
        }

        if (0 == lError)
        {
            if (LOADER_DateTimeFormat_Internal != DateTimeFormat)
            {
                // We need to return a persistent char pointer; this should happen rather seldom
                pColumnData->lDataLength = _len_internal;
                pColumnData->PtrUsed_ls00 = LOADER_FALSE;
            }
        }
    }
    else
    {
        LOADER_Char _szTmp[TIMESTAMP_MXSP00];         // sufficient for all different value types

        switch (pColumnData->SQLColumnInfo.ColumnBasics.SQLDataType)
        {
            case LOADER_SQLTYPE_DATE:
            {
                i41dget(*pDateTimeMask,
                        reinterpret_cast<LOADER_Char*>(_szPtr),
                        pColumnData->lDataLength,
                        _szTmp,
                        lError);
                break;
            }
            case LOADER_SQLTYPE_TIME:
            {
                _len_internal = TIME_MXSP00;
                i41tget(*pDateTimeMask,
                        reinterpret_cast<LOADER_Char*>(_szPtr),
                        pColumnData->lDataLength,
                        _szTmp,
                        lError);
                break;
            }
            case LOADER_SQLTYPE_TIMESTAMP:
            {
                _len_internal = TIMESTAMP_MXSP00;
                i41tsget(*pDateTimeMask,
                         reinterpret_cast<LOADER_Char*>(_szPtr),
                         pColumnData->lDataLength,
                         _szTmp,
                         lError);
                break;
            }
            default:        // Error!!! Should actually not happen if the function is properly used.
            {
                lError = -1;
            }
        }

        if (0 == lError)
        {
            // If the data has already been delivered in INTERNAL format we do not need to copy it again;
            // for all other formats we need to copy
            if (LOADER_DateTimeFormat_Internal != DateTimeFormat)
            {
                pColumnData->lDataLength = _len_internal;
                SAPDB_memcpy(_szPtr, _szTmp, pColumnData->lDataLength);                         
            }
        }
    }

    return lError;
}
// convertDateTimeValues()


/*
  -----------------------------------------------------------------------------
  function:     InsertDefaultValue
  -----------------------------------------------------------------------------
*/
// CHAR DEFAULTS LIKE USER OR USERGROUP MUST ALREADY BE TRIMMED COMING HERE!!!
LOADER_Int4
Loader_SpecialConstants::InsertDefaultValue(Loader_ColumnData*  pFieldData,
                                            LOADER_Byte*        pszBuffer,
                                            LOADER_Int4         lBufferLength)
{
    LOADER_Int4   lValueLen = -1;               // initialize to error condition
    LOADER_Byte*  pszBuf    = pszBuffer;        // for stamp default and user defined default

    if (LOADER_SQLDEFAULT_NONE == pFieldData->SQLColumnInfo.DefaultType)      // NULL value
    {
        lValueLen = 1;                          // This length corresponds to the length of the 
        *pszBuffer++ = (char)csp_undef_byte;    // col if it is of any other type than LOADER_COLLENTYPE_FIX

        if (LOADER_COLLENTYPE_FIX == pFieldData->SQLColumnInfo.LengthType)
        {
            lValueLen = pFieldData->SQLColumnInfo.ColumnBasics.SQLInOutLength;    // Length is in_out_length of that field
        }
    }
    else
    {
        *pszBuffer = pFieldData->cDefByte;
        pszBuffer += pFieldData->DataOffset;
        lValueLen  = pFieldData->DataOffset;
    
        switch (pFieldData->SQLColumnInfo.DefaultType)
        {
            case LOADER_SQLDEFAULT_USER:            // Attention: double fall through
            case LOADER_SQLDEFAULT_USERGROUP:
            {
                SAPDB_memcpy(pszBuffer, 
                             pFieldData->SQLColumnInfo.DefaultValue.pszStringData,
                             pFieldData->SQLColumnInfo.DefaultValue.lStringLength);
                lValueLen += pFieldData->SQLColumnInfo.DefaultValue.lStringLength;
                break;
            }
            case LOADER_SQLDEFAULT_DATE:
            {
                tsp00_Time  sqlTime;
                sqldattime(*(REINTERPRET_CAST(tsp00_Date*, pszBuffer)), sqlTime);
                lValueLen += DATE_MXSP00;
                break;
            }
            case LOADER_SQLDEFAULT_TIME:
            {
                tsp00_Date sqlDate;
                sqldattime(sqlDate, *(REINTERPRET_CAST(tsp00_Time*, pszBuffer)) );         
                lValueLen += TIME_MXSP00;
                break;
            }
            case LOADER_SQLDEFAULT_STAMP:
            {
                LOADER_ErrorCode rc = increaseSysKey(reinterpret_cast<LOADER_Byte*>(&pFieldData->SQLColumnInfo.DefaultValue.pszStringData[3]));
                if (LOA_ERR_OK == rc)
                {
                    // Attention; the default value in pdStamp_ls00 contains the def byte already
                    SAPDB_memcpy(pszBuf, pFieldData->SQLColumnInfo.DefaultValue.pszStringData, LOADER_STAMP_LENGTH);
                    lValueLen = LOADER_STAMP_LENGTH;
                }
                break;
            }
            case LOADER_SQLDEFAULT_TIMESTAMP:
            {
                lValueLen += setTimeStampDefault(pszBuffer);
                break;
            }
            case LOADER_SQLDEFAULT_TRUE:
            {
                *pszBuffer = '1';
                lValueLen = 2;
                break;
            }
            case LOADER_SQLDEFAULT_FALSE:
            {
                *pszBuffer = 0;
                lValueLen = 2;
                break;
            }
            case LOADER_SQLDEFAULT_OTHER:
            {
                //*
                //* User defined default:
                //* pszOtherDefault contains the right value including the def-byte and
                //* the right length for fix- and variable length cols.
                //* In case of var length cols the length is the value length after trimming
                //* it (delivered by the kernel). It does not necessarily equal fbInOutLength_ls00.
                //* No codeset conversion necessary: value is delivered using the kernel codeset!!!
                //*

                // Because we fill the buffer for every value with the def-byte 
                // we have to move the pointer back by one.
                // Delivered default value always includes the def-byte !
                SAPDB_memcpy(pszBuf,
                             pFieldData->SQLColumnInfo.DefaultValue.pszStringData,
                             pFieldData->SQLColumnInfo.DefaultValue.lStringLength);
                lValueLen = pFieldData->SQLColumnInfo.DefaultValue.lStringLength;
                break;
            }
            default:
            {
                // not implemented yet are 
                //  sp7_default_serial
                //  sp7_default_uid

                //lValueLen = -1;     // Unknown or not implemented default
                break;
            }
        }
    }
    return lValueLen;
}
//  InsertDefaultValue()
