/*!
  @file           IFRUtil_VDNNumber.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          VDN Number conversions

  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef IFRUTIL_VDNNUMBER_H
#define IFRUTIL_VDNNUMBER_H

#include "Interfaces/Runtime/IFR_ErrorHndl.h"

IFR_BEGIN_NAMESPACE

/**
 * Utility class for encapsulating VDN Number conversion.
 */
class IFRUtil_VDNNumber
{
public:
    /**
     * Convert a VDN number into a signed 4 byte integer. 
     * @param number the source VDN number.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *        (Same as short field info iolength, including the defined 
     *        byte).
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToInt4(unsigned char *number, IFR_Int4& result, int iolength=21);

    /**
     * Convert a VDN number into an unsigned 4 byte integer. 
     * @param number the source VDN number.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is
     * <code>(<var>number of digits</var> + 1 / 2) + 2</code> (Same as
     * short field info iolength including the defined 
     *        byte)
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToUInt4(unsigned char *number, IFR_UInt4& result, int iolength=21);

    /**
     * Convert a VDN number into a signed 2 byte integer. 
     * @param number the source VDN number without defined byte.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> (Same as short field info iolength
     *        including the defined  byte).  
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToInt2(unsigned char *number, IFR_Int2& result, int iolength=21);

    /**
     * Convert a VDN number into an unsigned 2 byte integer. 
     * @param number the source VDN number without defined byte.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *       (Same as short field info iolength, including the defined byte).
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToUInt2(unsigned char *number, IFR_UInt2& result, int iolength=21);

    /**
     * Convert a VDN number into a signed 1 byte integer. 
     * @param number the source VDN number without defined byte.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *        (Same as short field info iolength, including the defined byte).  
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToInt1(unsigned char *number, IFR_Int1& result, int iolength=21);

    /**
     * Convert a VDN number into an unsigned 1 byte integer. 
     * @param number the source VDN number without defined byte.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *        (Same as short field info iolength, including the defined byte).  
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToUInt1(unsigned char *number, IFR_UInt1& result, int iolength=21);
    
    /**
     * Convert a VDN number into a signed 8 byte integer. 
     * @param number the source VDN number without defined byte.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *        (Same as short field info iolength, including the defined byte).  
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToInt8(unsigned char *number, IFR_Int8& result, int iolength=21);

    /**
     * Convert a VDN number into an unsigned 8 byte integer. 
     * @param number the source VDN number without defined byte.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *        (Same as short field info iolength, including the defined byte).  
     * @return <code>IFR_OK</code> if the conversion went ok, 
     *         <code>IFR_OVERFLOW</code> if the number would
     *         be too small or too large, <code>IFR_DATA_TRUNC</code>
     *         if the number had decimal places which were truncated.
     *         In case of <code>IFR_OVERFLOW</code>, no result is stored
     *         in <code>result</code>.
     */
    static IFR_Retcode numberToUInt8(unsigned char *number, IFR_UInt8& result, int iolength=21);
    
    
    /**
     * Convert a VDN number into a double floating point number.
     * @param number the source VDN number.
     * @param result the destination integer number.
     * @param length The number of digits of the number.
     * @return <code>IFR_OK</code> went ok, <code>IFR_NOT_OK</code> if the
     *        VDN number is illegal.
     */
    static IFR_Retcode numberToDouble(unsigned char *number, double& result, int length);
    
    /**
     * Convert a number into a decimal (BCD) number.
     * @param number The VD-Number.
     * @param length The number of digits of the number.
     * @param decimal The decimal number.
     * @param decimalDigits The number of digits in the decimal.
     * @param decimalFraction The fraction in the decimal.
     */
    static IFR_Retcode numberToDecimal(unsigned char *number,
                                       int length,
                                       unsigned char *decimal,
                                       int decimalDigits,
                                       int decimalFraction);
        
    
    /**
     * Convert a VDN number into a <code>float</code> floating point number.
     * @param number the source VDN number.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *       (Same as short field info iolength, including the defined byte).  
     * @return <code>IFR_OK</code> went ok, <code>IFR_NOT_OK</code> if the
     *        VDN number is illegal.
     */
    static IFR_Retcode numberToFloat(unsigned char *number, float& result, int iolength=21);

    /**
     * Convert a VDN number into a <code>long double</code> floating point number.
     * @param number the source VDN number.
     * @param result the destination integer number.
     * @param iolength the iolength of the VDN number. This is 
     *        <code>(<var>number of digits</var> + 1 / 2) + 2</code> 
     *       (Same as short field info iolength, including the defined byte).  
     * @return <code>IFR_OK</code> went ok, <code>IFR_NOT_OK</code> if the
     *        VDN number is illegal.
     */
    static IFR_Retcode numberToLongDouble(unsigned char *number, long double& result, int iolength=21);
    

    /**
     * Convert a signed 1 byte integer into a VDN number.
     * @param number the number to convert
     * @param result the buffer of the VDN number. Always all 20 possible bytes
     *        are used.
     * @return <code>IFR_OK</code>.
     */
    static IFR_Retcode int1ToNumber(IFR_Int1 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);

    /**
     * Convert an unsigned 1 byte integer into a VDN number.
     * @param number the number to convert
     * @param result The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *               room in the result buffer.
     * @param digits The number of integer digits that may be used. 
     * @return <code>IFR_OK</code> on success, <code>IFR_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static IFR_Retcode uint1ToNumber(IFR_UInt1 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);

    /**
     * Convert a signed 2 byte integer into a VDN number.
     * @param number the number to convert
     * @param result The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *               room in the result buffer.
     * @param digits The number of integer digits that may be used. 
     * @return <code>IFR_OK</code> on success, <code>IFR_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static IFR_Retcode int2ToNumber(IFR_Int2 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);

    /**
     * Convert an unsigned 2 byte integer into a VDN number.
     * @param number the number to convert
     * @param result The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *               room in the result buffer.
     * @param digits The number of integer digits that may be used. 
     * @return <code>IFR_OK</code> on success, <code>IFR_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static IFR_Retcode uint2ToNumber(IFR_UInt2 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);

    /**
     * Convert a signed 4 byte integer into a VDN number.
     * @param number the number to convert
     * @param result The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *               room in the result buffer.
     * @param digits The number of integer digits that may be used. 
     * @return <code>IFR_OK</code> on success, <code>IFR_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static IFR_Retcode int4ToNumber(IFR_Int4 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);

    /**
     * Convert an unsigned 1 byte integer into a VDN number.
     * @param number the number to convert
     * @param result The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *               room in the result buffer.
     * @param digits The number of integer digits that may be used. 
     * @return <code>IFR_OK</code> on success, <code>IFR_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static IFR_Retcode uint4ToNumber(IFR_UInt4 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);

    /**
     * Convert a signed 8 byte integer into a VDN number.
     * @param number the number to convert
     * @param result The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *               room in the result buffer.
     * @param digits The number of integer digits that may be used. 
     * @return <code>IFR_OK</code> on success, <code>IFR_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static IFR_Retcode int8ToNumber(IFR_Int8 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);

    /**
     * Convert an signed 8 byte integer into a VDN number.
     * @param number the number to convert
     * @param result The buffer of the VDN number. There must be <code>(digits + 1) div 2 + 1</code> 
     *               room in the result buffer.
     * @param digits The number of integer digits that may be used. 
     * @return <code>IFR_OK</code> on success, <code>IFR_OVERFLOW</code> if the
     *         number of digits is not sufficient for the value.
     */
    static IFR_Retcode uint8ToNumber(IFR_UInt8 number, unsigned char *result, IFR_Int4 digits, IFR_Bool isfloat=false);


    static IFR_Retcode isSmallInteger(unsigned char *number, IFR_Int4 length);
    
    static IFR_Retcode isInteger(unsigned char *number, IFR_Int4 length);

    /**
     * Converts a double value into a VDN number. Up to <code>length</code> 
     * digits are inserted into the number, the last digit is determined according
     * to proper rounding of the digit that follow in the source number.
     * @param number The number to convert.
     * @param result The destination buffer.
     * @param length The overall number of significant digits.
     * @param precision The number of fractional digits, or -1 if it is a floating point number.
     * @return <code>IFR_OVERFLOW</code> if <code>number</code> is 
     * <ul>
     *   <li>NAN, +INF or -INF</li>
     *   <li>greater than 0.9...E+63 (as many 9 as the <code>length</code>).</li>
     *   <li>less than 0.9...E+63 (as many 9 as the <code>length</code>).</li>
     * </ul>
     */
    static IFR_Retcode doubleToNumber(double number, 
                                      unsigned char *result, 
                                      int length,
                                      int precision);
    
    /**
     * Converts a decimal to a number.
     * @param decimal           The buffer containing the DECIMAL.
     * @param decimal_length    The number of digits of the decimal.
     * @param decimal_precision The precision of the decimal.
     * @param result            The result buffer,
     * @param length            The number of digits of the result.
     * @param precision         The number of digits.
     * @return @c IFR_OVERFLOW on an overflow, @c IFR_NOT_OK on a conversion error,
     *         @c IFR_OK on success.
     */
    static IFR_Retcode decimalToNumber(unsigned char *decimal,
                                       int            decimal_length,
                                       int            decimal_precision,
                                       unsigned char *result, 
                                       int length,
                                       int precision);
    
    
    
    /**
     * Universal method for converting a number into a string. The resulting buffer must have
     * room for <code>length</code> + 2 chars (fixed types) or <code>length</code> + 
     * 7 chars (floating-point types). Fixed-point numbers are converted with their whole
     * precision (trailing zeros after the decimal point). Floating-point numbers which
     * have an absolute value &lt; 1.0E-4 or &gt; 1.0E+7 are displayed in scientific notation.
     * @param number The VDN Number.
     * @param buffer The destination buffer.
     * @param bufferlength The number of <i>bytes</i> in the buffer, including the place for
     *                     the null-terminator.
     * @param encoding The encoding of the buffer.
     * @param fixed Whether the number to be converted is a fixed or a floating-point type.
     * @param length Total number of digits (max. value)
     * @param precision Number of digits after the decimal point.
     * @return <code>IFR_OK</code> if the conversion went ok, <code>IFR_DATA_TRUNC</code> if
     *         there was not enough room for the data in the buffer.
     */
    static IFR_Retcode numberToString(unsigned char *number,
                                      char *buffer, 
                                      IFR_size_t bufferlength,
                                      IFR_StringEncoding encoding,
                                      IFR_Bool fixed,
                                      IFR_Int4 length,
                                      IFR_Int4 precision);


    static IFR_Retcode stringToNumber(char              *buffer,
                                      IFR_size_t         bufferlength,
                                      IFR_StringEncoding encoding,
                                      IFR_Bool          &nan,
                                      unsigned char     *number,
                                      IFR_Bool           fixed,
                                      IFR_Int4           length,
                                      IFR_Int4           precision);


    /**
     * Checks whether significant digits are lost when inserting the
     * provided VDN number (20 bytes) into a shorter buffer. A number
     * will fail in this test not all digits before the decimal point
     * will fit into the length.
     * @param number the VDN number (20 bytes)
     * @param iolength the iolength of the field (same as short field info 
     *        iolength).
     * @return <code>IFR_OK</code> if it is ok, <code>IFR_DATA_TRUNC</code>
     *     if there is insignificant data which is not inserted, 
     *     <code>IFR_OVERFLOW</code> if the number is too big.
     */
    static IFR_Retcode checkVDNNumber(unsigned char*number, int iolength);

private:
    static IFR_Retcode numberToStringAscii(unsigned char *number,
                                           char          *buffer,
                                           IFR_size_t     bufferlength,
                                           IFR_Bool       fixed,
                                           IFR_Int4       length,
                                           IFR_Int4       precision);

    static IFR_Retcode numberToStringUCS2(unsigned char *number,
                                           char          *buffer,
                                           IFR_size_t     bufferlength,
                                           IFR_Bool       fixed,
                                           IFR_Int4       length,
                                           IFR_Int4       precision);
    
    static IFR_Retcode numberToStringUCS2Swapped(unsigned char *number,
                                                 char          *buffer,
                                                 IFR_size_t     bufferlength,
                                                 IFR_Bool       fixed,
                                                 IFR_Int4       length,
                                                 IFR_Int4       precision);

    
    static IFR_Retcode stringToNumberAscii(char              *buffer,
                                           IFR_size_t         bufferlength,
                                           IFR_Bool          &nan,
                                           unsigned char     *number,
                                           IFR_Bool           fixed,
                                           IFR_Int4           length,
                                           IFR_Int4           precision);

    

};

IFR_END_NAMESPACE

#endif
