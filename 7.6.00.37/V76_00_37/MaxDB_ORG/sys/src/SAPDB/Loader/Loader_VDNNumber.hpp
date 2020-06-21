/*!
  \file    Loader_VDNNumber.hpp
  \author  SteffenS
  \brief   Converter for VDN numbers

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

#ifndef LOADER_VDNNUMBER_HPP
#define LOADER_VDNNUMBER_HPP

#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_ErrorCode.hpp"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"


// from gsp00.h
#define LOA_MAX_LONGINT         "\xCA\x21\x47\x48\x36\x47\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define LOA_MIN_LONGINT         "\x36\x78\x52\x51\x63\x52\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define LOA_MAX_SHORTINT        "\xC5\x32\x76\x70\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define LOA_MIN_SHORTINT        "\x3B\x67\x23\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"


class Loader_VDNNumber
{
public:

    /**
     * Convert a signed 1 byte integer into a VDN number.
     * @param intValue  The number to convert
     * @param pszNumber The buffer of the VDN number. Always all 20 possible bytes
     *        are used.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code>.
     */
    static LOADER_ErrorCode int1ToNumber(LOADER_Int1 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Convert an unsigned 1 byte integer into a VDN number.
     * @param intValue the number to convert
     * @param pszNumber The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *                  room in the result buffer.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code> on success, <code>LOA_ERR_NUMBER_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static LOADER_ErrorCode uint1ToNumber(LOADER_UInt1 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Convert a signed 2 byte integer into a VDN number.
     * @param intValue  The number to convert
     * @param pszNumber The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *                  room in the result buffer.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code> on success, <code>LOA_ERR_NUMBER_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static LOADER_ErrorCode int2ToNumber(LOADER_Int2 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Convert an unsigned 2 byte integer into a VDN number.
     * @param intValue  The number to convert
     * @param pszNumber The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *                  room in the result buffer.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code> on success, <code>LOA_ERR_NUMBER_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static LOADER_ErrorCode uint2ToNumber(LOADER_UInt2 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Convert a signed 4 byte integer into a VDN number.
     * @param intValue  The number to convert
     * @param pszNumber The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *                  room in the result buffer.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code> on success, <code>LOA_ERR_NUMBER_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static LOADER_ErrorCode int4ToNumber(LOADER_Int4 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Convert an unsigned 1 byte integer into a VDN number.
     * @param intValue  The number to convert
     * @param pszNumber The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *                  room in the result buffer.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code> on success, <code>LOA_ERR_NUMBER_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static LOADER_ErrorCode uint4ToNumber(LOADER_UInt4 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Convert a signed 8 byte integer into a VDN number.
     * @param intValue  The number to convert
     * @param pszNumber The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *                  room in the result buffer.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code> on success, <code>LOA_ERR_NUMBER_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static LOADER_ErrorCode int8ToNumber(LOADER_Int8 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Convert an signed 8 byte integer into a VDN number.
     * @param intValue  The number to convert
     * @param pszNumber The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *                  room in the result buffer.
     * @param digits    The number of integer digits that may be used. 
     * @return <code>LOA_ERR_OK</code> on success, <code>LOA_ERR_NUMBER_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static LOADER_ErrorCode uint8ToNumber(LOADER_UInt8 intValue, LOADER_Byte* pszNumber, LOADER_Int4 digits);

    /**
     * Converts a double value into a VDN number. Up to <code>length</code> 
     * digits are inserted into the number, the last digit is determined according
     * to proper rounding of the digit that follow in the source number.
     * @param doubleValue The number to convert.
     * @param pszNumber   The destination buffer.
     * @param length      The overall number of significant digits.
     * @param precision   The number of fractional digits, or -1 if it is a floating point number.
     * @return <code>LOA_ERR_NUMBER_OVERFLOW</code> if <code>number</code> is 
     * <ul>
     *   <li>NAN, +INF or -INF</li>
     *   <li>greater than 0.9...E+63 (as many 9 as the <code>length</code>).</li>
     *   <li>less than 0.9...E+63 (as many 9 as the <code>length</code>).</li>
     * </ul>
     */
    static LOADER_ErrorCode doubleToNumber(double       doubleValue, 
                                           LOADER_Byte* pszNumber,
                                           LOADER_Int4  length,
                                           LOADER_Int4  precision);
    
    /**
     * Converts a decimal to a number.
     * @param decimal           The buffer containing the DECIMAL.
     * @param decimal_length    The number of digits of the decimal.
     * @param decimal_precision The precision of the decimal.
     * @param result            The result buffer,
     * @param length            The number of digits of the result.
     * @param precision         The number of digits.
     * @return @c LOA_ERR_NUMBER_OVERFLOW on an overflow,
     *         @c LOA_ERR_OK on success.
     */
    static LOADER_ErrorCode decimalToNumber(LOADER_Byte*    decimal,
                                            LOADER_Int4     decimal_length,
                                            LOADER_Int4     decimal_precision,
                                            LOADER_Byte*    result,
                                            LOADER_Int4     length,
                                            LOADER_Int4     precision);
    
    /**
     * Method for converting a string into a number. 
     *
     * @param number            The VDN Number.
     * @param pszSourceString   The string to convert.
     * @param lStringLength     The length of the string to convert.
     * @param encoding          The encoding of the string.
     * @param fixed             Whether the number to convert to is a fixed or a floating-point type.
     * @param length            Total number of digits (max. value)
     * @param precision         Number of digits after the decimal point.
     * @return <code>LOA_ERR_OK</code> if the conversion went ok,
     *         <code>LOA_ERR_NUMBER_TRUNCATED</code> if there was not enough room for the data in the buffer,
     *         <code>LOA_ERR_NUMBER_OVERFLOW</code> if the string could not be converted either because it was
     *                                              not a valid number or it did not fit.
     */
    static LOADER_ErrorCode stringToNumber(LOADER_Byte*           pszSourceString,
                                           LOADER_UInt4           lStringLength,
                                           LOADER_StringEncoding  encoding,
                                           LOADER_Bool&           nan,
                                           LOADER_Byte*           pszNumber,
                                           LOADER_Bool            fixed,
                                           LOADER_Int4            length,
                                           LOADER_Int4            precision);



    static LOADER_ErrorCode  checkIntRange(LOADER_Byte*     pszNumber,
                                           LOADER_Int4      lNumberLength,
                                           LOADER_SQLType   DataType);

};

#endif //LOADER_VDNNUMBER_HPP
