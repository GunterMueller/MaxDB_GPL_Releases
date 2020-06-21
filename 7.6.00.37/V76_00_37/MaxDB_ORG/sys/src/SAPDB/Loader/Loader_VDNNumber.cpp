/*!
  \file    Loader_VDNNumber.cpp
  \author  SteffenS
  \brief   VDN intValue conversions

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
#include "Loader/Loader_VDNNumber.hpp"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"


LOADER_ErrorCode
Loader_VDNNumber::int1ToNumber(LOADER_Int1 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    //LOADER_ErrorCode rcLOA = LOA_ERR_OK;

    IFR_Retcode rc = IFRUtil_VDNNumber::int1ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}

LOADER_ErrorCode
Loader_VDNNumber::uint1ToNumber(LOADER_UInt1 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::uint1ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}

LOADER_ErrorCode
Loader_VDNNumber::int2ToNumber(LOADER_Int2 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::int2ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// int2ToNumber()


LOADER_ErrorCode
Loader_VDNNumber::uint2ToNumber(LOADER_UInt2 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::uint2ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// uint2ToNumber()


LOADER_ErrorCode
Loader_VDNNumber::int4ToNumber(LOADER_Int4 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::int4ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// int4ToNumber()


LOADER_ErrorCode
Loader_VDNNumber::uint4ToNumber(LOADER_UInt4 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::uint4ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// uint4ToNumber()


LOADER_ErrorCode
Loader_VDNNumber::int8ToNumber(LOADER_Int8 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::int8ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// int8ToNumber()


LOADER_ErrorCode
Loader_VDNNumber::uint8ToNumber(LOADER_UInt8 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::uint8ToNumber(intValue, pszNumber, digits);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// uint8ToNumber()


LOADER_ErrorCode
Loader_VDNNumber::doubleToNumber(double       doubleValue, 
                                 LOADER_Byte* pszNumber,
                                 LOADER_Int4  length,
                                 LOADER_Int4  precision)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::doubleToNumber(doubleValue, pszNumber, length, precision);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// doubleToNumber()


LOADER_ErrorCode
Loader_VDNNumber::decimalToNumber(LOADER_Byte*    decimal,
                                  LOADER_Int4     decimal_length,
                                  LOADER_Int4     decimal_precision,
                                  LOADER_Byte*    result,
                                  LOADER_Int4     length,
                                  LOADER_Int4     precision)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::decimalToNumber(decimal, decimal_length, decimal_precision, result, length, precision);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// decimalToNumber()


LOADER_ErrorCode
Loader_VDNNumber::stringToNumber(LOADER_Byte*           pszSourceString,
                                 LOADER_UInt4           lStringLength,
                                 LOADER_StringEncoding  encoding,
                                 LOADER_Bool&           nan,
                                 LOADER_Byte*           pszNumber,
                                 LOADER_Bool            fixed,
                                 LOADER_Int4            length,
                                 LOADER_Int4            precision)
{
    IFR_Retcode rc = IFRUtil_VDNNumber::stringToNumber(reinterpret_cast<LOADER_Char*>(pszSourceString),
                                                       lStringLength,
                                                       IFR_StringEncoding(encoding),
                                                       nan, pszNumber, fixed, length, precision);
    if (IFR_OK != rc)
    {
        return (IFR_DATA_TRUNC == rc) ? LOA_ERR_NUMBER_TRUNCATED : LOA_ERR_NUMBER_OVERFLOW;
    }
    return LOA_ERR_OK;
}
// stringToNumber()


LOADER_ErrorCode
Loader_VDNNumber::checkIntRange(LOADER_Byte*    pszNumber,
                                LOADER_Int4     lNumberLength,
                                LOADER_SQLType  DataType)
{
    if ( (LOADER_SQLTYPE_SMALLINT != DataType) && (LOADER_SQLTYPE_INTEGER != DataType ) )
    {
        return LOA_ERR_OK;
    }

    if (LOADER_SQLTYPE_SMALLINT == DataType)
    {
        if ( (memcmp(pszNumber, LOA_MIN_SHORTINT, lNumberLength) < 0 ? 1 : 0) ||     // pszNumber < LOA_MIN_SHORTINT
             (memcmp(pszNumber, LOA_MAX_SHORTINT, lNumberLength) > 0 ? 1 : 0) )      // pszNumber > LOA_MAX_SHORTINT
        {
            return LOA_ERR_NUMBER_OVERFLOW;
        }
    }
    else if (LOADER_SQLTYPE_INTEGER == DataType)
    {
        if ( (memcmp(pszNumber, LOA_MIN_LONGINT, lNumberLength) < 0 ? 1 : 0) ||     // pszNumber < LOA_MIN_LONGINT
             (memcmp(pszNumber, LOA_MAX_LONGINT, lNumberLength) > 0 ? 1 : 0) )      // pszNumber > LOA_MAX_LONGINT
        {
            return LOA_ERR_NUMBER_OVERFLOW;
        }
    }

    return LOA_ERR_OK;
}
// checkIntRange()
