/*!
  @file           IFRUtil_SQLNumeric.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          ODBC SQL_NUMERIC handling.


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
#ifndef IFRUTIL_SQLNUMERIC_H
#define IFRUTIL_SQLNUMERIC_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"

IFR_BEGIN_NAMESPACE

/**
 * Utility class for encapsulating <code>SQL_NUMERIC</code>
 * conversion.
 */
class IFRUtil_SQLNumeric
{
public:
    /**
     * Converts an ASCII string too an SQL_NUMERIC_STRUCT. 
     * @param buffer The ASCII string, null-terminated.
     * @param number The target SQL_NUMERIC_STRUCT.
     * @return @c IFR_OK on success, @c IFR_NOT_OK on conversion
     * error, @c IFR_OVERFLOW on overflow.
     */
    static IFR_Retcode asciiStringToNumeric(const char *buffer,
                                            SQL_NUMERIC_STRUCT& number);

    /**
     * Converts an SQL_NUMERIC_STRUCT into an ASCII string. No trailing
     * terminator is appended. 
     * @param number The <code>SQL_NUMERIC</code> to convert.
     * @param buffer The destination buffer.
     * @param bufferlength The length of the destination buffer in bytes. 
     *                     After the conversion this will contain the
     *                     number of bytes used in the conversion.
     * @return <code>IFR_OK</code> on succes, <code>IFR_DATA_TRUNC</code> if the
     *         buffer is not sufficient to contain the number.
     */
    static IFR_Retcode numericToAsciiString(const SQL_NUMERIC_STRUCT &number,
                                            char                     *buffer,
                                            IFR_size_t               &bufferlength);
    
    /**
     * Converts an SQL_NUMERIC_STRUCT into a VDN Number.
     * @param number    The SQL_NUMERIC_STRUCT to convert.
     * @param vdnnumber The target VDN Number.
     * @param digits    The number of digits the VDN Number may have.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>, or <code>IFR_OVERFLOW</code> 
     *         on error.
     */
    static IFR_Retcode numericToNumber(const SQL_NUMERIC_STRUCT &number,
                                       unsigned char            *vdnnumber,
                                       IFR_Int4                 digits);
    
    /**
     * Converts a VDN Number to a SQL_NUMERIC_STRUCT.
     * @param vdnnumber The source VDN Number.
     * @param length    The number of digits of the VDN number.
     * @param number    The target SQL_NUMERIC_STRUCT.
     * @param precision The precision field of the SQL_NUMERIC_STRUCT.
     * @return <code>IFR_NOT_OK</code> if the conversion fails, <code>IFR_OK</code>
     *         otherwise.
     */
    static IFR_Retcode numberToNumeric(const unsigned char *vdnnumber,
                                       IFR_Int4       length,
                                       SQL_NUMERIC_STRUCT &number,
                                       IFR_UInt1      precision);

    
};

IFR_END_NAMESPACE

#endif
