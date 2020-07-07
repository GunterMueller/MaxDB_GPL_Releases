/*!
  @file           IFR_Datatypes.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_DataConv
  @brief          SQL and Hostvartypes
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
#ifndef IFR_DATATYPES_H
#define IFR_DATATYPES_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/Interfaces/Runtime/IFR_Types.h"

IFR_BEGIN_NAMESPACE

/**
 * Constants used from <code>gsp00</code>. These constants are delivered in the short
 * infos or provided by the application at parameter binding time.
 * See <code>tsp00_DataType_Enum</code> for the original.
 */
enum IFR_SQLType {
    IFR_SQLTYPE_MIN   = SQLDBC_SQLTYPE_MIN,                   //!< Minumum value for checks.

    IFR_SQLTYPE_FIXED = SQLDBC_SQLTYPE_FIXED,
    IFR_SQLTYPE_FLOAT = SQLDBC_SQLTYPE_FLOAT,
    IFR_SQLTYPE_CHA = SQLDBC_SQLTYPE_CHA,                 //!< The data type <code>CHAR ASCII</code>.
    IFR_SQLTYPE_CHE = SQLDBC_SQLTYPE_CHE,
    IFR_SQLTYPE_CHB = SQLDBC_SQLTYPE_CHB,
    IFR_SQLTYPE_ROWID = SQLDBC_SQLTYPE_ROWID,
    IFR_SQLTYPE_STRA = SQLDBC_SQLTYPE_STRA,
    IFR_SQLTYPE_STRE = SQLDBC_SQLTYPE_STRE,
    IFR_SQLTYPE_STRB = SQLDBC_SQLTYPE_STRB,
    IFR_SQLTYPE_STRDB = SQLDBC_SQLTYPE_STRDB,
    IFR_SQLTYPE_DATE = SQLDBC_SQLTYPE_DATE,
    IFR_SQLTYPE_TIME = SQLDBC_SQLTYPE_TIME,
    IFR_SQLTYPE_VFLOAT = SQLDBC_SQLTYPE_VFLOAT,
    IFR_SQLTYPE_TIMESTAMP = SQLDBC_SQLTYPE_TIMESTAMP,
    IFR_SQLTYPE_UNKNOWN = SQLDBC_SQLTYPE_UNKNOWN,
    IFR_SQLTYPE_NUMBER = SQLDBC_SQLTYPE_NUMBER,
    IFR_SQLTYPE_NONUMBER = SQLDBC_SQLTYPE_NONUMBER,
    IFR_SQLTYPE_DURATION = SQLDBC_SQLTYPE_DURATION,
    IFR_SQLTYPE_DBYTEEBCDIC = SQLDBC_SQLTYPE_DBYTEEBCDIC,
    IFR_SQLTYPE_LONGA = SQLDBC_SQLTYPE_LONGA,
    IFR_SQLTYPE_LONGE = SQLDBC_SQLTYPE_LONGE,
    IFR_SQLTYPE_LONGB = SQLDBC_SQLTYPE_LONGB,
    IFR_SQLTYPE_LONGDB = SQLDBC_SQLTYPE_LONGDB,
    IFR_SQLTYPE_BOOLEAN = SQLDBC_SQLTYPE_BOOLEAN,
    IFR_SQLTYPE_UNICODE = SQLDBC_SQLTYPE_UNICODE,
    IFR_SQLTYPE_DTFILLER1 = SQLDBC_SQLTYPE_DTFILLER1,
    IFR_SQLTYPE_DTFILLER2 = SQLDBC_SQLTYPE_DTFILLER2,
    IFR_SQLTYPE_DTFILLER3 = SQLDBC_SQLTYPE_DTFILLER3,
    IFR_SQLTYPE_DTFILLER4 = SQLDBC_SQLTYPE_DTFILLER4,
    IFR_SQLTYPE_SMALLINT = SQLDBC_SQLTYPE_SMALLINT,
    IFR_SQLTYPE_INTEGER = SQLDBC_SQLTYPE_INTEGER,
    IFR_SQLTYPE_VARCHARA = SQLDBC_SQLTYPE_VARCHARA,
    IFR_SQLTYPE_VARCHARE = SQLDBC_SQLTYPE_VARCHARE,
    IFR_SQLTYPE_VARCHARB = SQLDBC_SQLTYPE_VARCHARB,
    IFR_SQLTYPE_STRUNI = SQLDBC_SQLTYPE_STRUNI,
    IFR_SQLTYPE_LONGUNI = SQLDBC_SQLTYPE_LONGUNI,
    IFR_SQLTYPE_VARCHARUNI = SQLDBC_SQLTYPE_VARCHARUNI,
    IFR_SQLTYPE_UDT = SQLDBC_SQLTYPE_UDT,
    IFR_SQLTYPE_ABAPTABHANDLE = SQLDBC_SQLTYPE_ABAPTABHANDLE,
    IFR_SQLTYPE_DWYDE         = SQLDBC_SQLTYPE_DWYDE,
    
    IFR_SQLTYPE_MAX = SQLDBC_SQLTYPE_MAX            //!< Maximum value for checks.
};

/**
 * String representation of SQL type.
 * @param sqltype The SQL type.
 */
const char *IFR_SQLTypeToString(IFR_SQLType sqltype, char *buffer, IFR_Length bufferlength=IFR_NTS);


#ifndef ODBC_TYPES_DEFINED

/**
 * Date struct as defined by ODBC.
 */
struct SQL_DATE_STRUCT
{
  IFR_Int2 year;      //!< The value of the year field must be between 0 and 9999, inclusive
  IFR_UInt2 month;    //!< The value of the month field must be between 1 and 12, inclusive
  IFR_UInt2 day;      //!< The value of the day field must be between 0 and 31, inclusive depending from the leap year
};


/**
 * Time struct as defined by ODBC.
 */
struct SQL_TIME_STRUCT
{
  IFR_UInt2 hour;   //!< The value of the hour field must be between 0 and 23, inclusive
  IFR_UInt2 minute; //!< The value of the minute field must be between 0 and 59, inclusive
  IFR_UInt2 second; //!< The value of the seconds field must be between 0 and 59, inclusive
};

/**
 * Timestamp struct as defined by ODBC.
 */
struct SQL_TIMESTAMP_STRUCT
{
    IFR_Int2 year;    //!< see: SQL_DATE_STRUCT
    IFR_UInt2 month;  //!< see: SQL_DATE_STRUCT
    IFR_UInt2 day;    //!< see: SQL_DATE_STRUCT
    IFR_UInt2 hour;   //!< see: SQL_TIME_STRUCT
    IFR_UInt2 minute; //!< see: SQL_TIME_STRUCT
    IFR_UInt2 second; //!< see: SQL_TIME_STRUCT
    IFR_UInt4 fraction; //!< see: SQL_TIME_STRUCT
};

#ifndef SQL_MAX_NUMERIC_LENGTH
/**
 * Maximum number of bytes a numeric will occupy in the <code>SQL_NUMERIC_STRUCT</code>.
 */  
#  define SQL_MAX_NUMERIC_LENGTH 16
#endif

#ifndef SQL_MAX_NUMERIC_DIGITS
/**
 * Maximum number of digits that reside in an SQL Numeric.
 */
#  define SQL_MAX_NUMERIC_DIGITS 38
#endif


/**
 * A decimal number, as defined by ODBC.
 */
struct SQL_NUMERIC_STRUCT
{
    IFR_Int1  precision; //!< number of digits
    IFR_Int1  scale;     //!< number of digits after the decimal seperator
    IFR_Int1  sign;      //!< 1 if positive, 0 if negative
    IFR_UInt1 val[SQL_MAX_NUMERIC_LENGTH];   //!< 38 decimal digits can be at most 129 bits. For compatibility, we use 16 here.
};

#endif

#ifndef WIN32
/**
 * A GUID, as defined by ODBC and/or Windows.
 */
struct  GUID
{
  IFR_UInt4 Data1;              //!< clock low value
  IFR_UInt2 Data2;              //!< clock middle value
  IFR_UInt2 Data3;              //!< clock high value
  IFR_UInt1 Data4[8];           //!< node value 
};
#endif

/**
 * Types of host (native) variables. 
 * @todo Allow unterminated characterstrings for ASCII, UCS2 and UTF8
 * @todo Allow UCS2Swapped and UCS2
 */
enum IFR_HostType {
    IFR_HOSTTYPE_MIN              = SQLDBC_HOSTTYPE_MIN, //!< Minimum value for checks.

    IFR_HOSTTYPE_PARAMETER_NOTSET = SQLDBC_HOSTTYPE_PARAMETER_NOTSET, //!< The default value for unset parameters.

    
    IFR_HOSTTYPE_BINARY = SQLDBC_HOSTTYPE_BINARY, //!< Binary data, length must be provided. No conversion is applied.
    
    IFR_HOSTTYPE_ASCII = SQLDBC_HOSTTYPE_ASCII, //!< An ASCII string.

    IFR_HOSTTYPE_UCS2_NATIVE =  SQLDBC_HOSTTYPE_UCS2_NATIVE, //!< An zero terminated UCS2 string (in native byte order).

    IFR_HOSTTYPE_UTF8 = SQLDBC_HOSTTYPE_UTF8,     //!< An UTF8 string.

    IFR_HOSTTYPE_UINT1 = SQLDBC_HOSTTYPE_UINT1,   //!< An unsigned 8 bit integer number.
    IFR_HOSTTYPE_INT1 = SQLDBC_HOSTTYPE_INT1,     //!< A signed 8 bit integer number.

    IFR_HOSTTYPE_UINT2 = SQLDBC_HOSTTYPE_UINT2,   //!< An unsigned 16 bit integer number.
    IFR_HOSTTYPE_INT2 = SQLDBC_HOSTTYPE_INT2,     //!< A signed 16 bit integer number.

    IFR_HOSTTYPE_UINT4 = SQLDBC_HOSTTYPE_UINT4,   //!< An unsigned 32 bit integer number.
    IFR_HOSTTYPE_INT4 = SQLDBC_HOSTTYPE_INT4,     //!< A signed 32 bit integer number.

    IFR_HOSTTYPE_UINT8 = SQLDBC_HOSTTYPE_UINT8,   //!< An unsigned 64 bit integer number.
    IFR_HOSTTYPE_INT8 = SQLDBC_HOSTTYPE_INT8,     //!< A signed 64 bit integer number.
    
    IFR_HOSTTYPE_DOUBLE = SQLDBC_HOSTTYPE_DOUBLE, //!< A <code>double</code> value.
    IFR_HOSTTYPE_FLOAT = SQLDBC_HOSTTYPE_FLOAT,   //!< A <code>float</code> value.

    IFR_HOSTTYPE_ODBCDATE = SQLDBC_HOSTTYPE_ODBCDATE, //!< An ODBC <code>SQL_DATE_STRUCT</code> struct.
    IFR_HOSTTYPE_ODBCTIME = SQLDBC_HOSTTYPE_ODBCTIME, //!< An ODBC <code>SQL_TIME_STRUCT</code> struct.
    IFR_HOSTTYPE_ODBCTIMESTAMP = SQLDBC_HOSTTYPE_ODBCTIMESTAMP, //!< An ODBC <code>SQL_TIMESTAMP_STRUCT</code> struct.

    IFR_HOSTTYPE_ODBCNUMERIC = SQLDBC_HOSTTYPE_ODBCNUMERIC, //!< An ODBC <code>SQL_NUMERIC_STRUCT</code> struct.

    IFR_HOSTTYPE_GUID = SQLDBC_HOSTTYPE_GUID, //!< An <code>GUID</code>. 
       
    IFR_HOSTTYPE_UCS2        =  SQLDBC_HOSTTYPE_UCS2, //!< An zero terminated UCS2 string (in none swapped byte order Ascii 'A' is coded as '0x0041).
    IFR_HOSTTYPE_UCS2_SWAPPED=  SQLDBC_HOSTTYPE_UCS2_SWAPPED, //!< An zero terminated UCS2 string (in swapped byte order Ascii 'A' is coded as 0x4100').

    IFR_HOSTTYPE_BLOB       = SQLDBC_HOSTTYPE_BLOB,        //!< Large binary object (pointer to LOB handle).
    IFR_HOSTTYPE_ASCII_LOB = SQLDBC_HOSTTYPE_ASCII_CLOB,  //!< Large character object (pointer to LOB handle).
    IFR_HOSTTYPE_UTF8_LOB  = SQLDBC_HOSTTYPE_UTF8_CLOB,   //!< Large unicode character object (pointer to LOB handle).
    IFR_HOSTTYPE_UCS2_LOB  = SQLDBC_HOSTTYPE_UCS2_CLOB,   //!< Large unicode character object (pointer to LOB handle).
    IFR_HOSTTYPE_UCS2_SWAPPED_LOB = SQLDBC_HOSTTYPE_UCS2_SWAPPED_CLOB, //!< Large unicode character object (pointer to LOB handle).

    IFR_HOSTTYPE_STREAM = SQLDBC_HOSTTYPE_STREAM, //!< ABAP/OMS Stream.

    IFR_HOSTTYPE_RAWHEX  = SQLDBC_HOSTTYPE_RAWHEX, // Raw hex dump.
    
    IFR_HOSTTYPE_DECIMAL         = SQLDBC_HOSTTYPE_DECIMAL, // Decimal type
    IFR_HOSTTYPE_OMS_PACKED_8_3  = SQLDBC_HOSTTYPE_OMS_PACKED_8_3,
    IFR_HOSTTYPE_OMS_PACKED_15_3  = SQLDBC_HOSTTYPE_OMS_PACKED_15_3,
    IFR_HOSTTYPE_OMS_TIMESTAMP  = SQLDBC_HOSTTYPE_OMS_TIMESTAMP,
    IFR_HOSTTYPE_OMS_ASCII      = SQLDBC_HOSTTYPE_OMS_ASCII,

    IFR_HOSTTYPE_USERDEFINED = SQLDBC_HOSTTYPE_USERDEFINED, //!< User-defined, must supply a parameter converter.

    IFR_HOSTTYPE_MAX = IFR_HOSTTYPE_USERDEFINED  //!< Maximum value for checks.
};

/**
 * Wrapper for date/time format.
 */
class IFR_DateTimeFormat 
{
public:
    /**
     * The possible date/time formats.
     */
    enum Format {
        None_C = SQLDBC_DateTimeFormat::None_C,     //!< Unknown, not set.
        Normal_C = SQLDBC_DateTimeFormat::Normal_C, //!< Normal (aka INTERNAL).
        Iso_C = SQLDBC_DateTimeFormat::Iso_C,       //!< ISO 
        Usa_C = SQLDBC_DateTimeFormat::Usa_C,       //!< USA
        Eur_C = SQLDBC_DateTimeFormat::Eur_C,       //!< Europe
        Jis_C = SQLDBC_DateTimeFormat::Jis_C,       //!< Japan
        OracleDate_C = SQLDBC_DateTimeFormat::OracleDate_C, 
        WasAnsiNowIsSameAsIso_C = SQLDBC_DateTimeFormat::WasAnsiNowIsSameAsIso_C, 
        TsEur_C = SQLDBC_DateTimeFormat::TsEur_C,  //!< @deprecated (same as Eur_C).
        Future_C = 9  //!< Filler.
    };
    
    inline static const char *toString(Format format)
    {
        switch(format) {
        case None_C:
            return "UNKNOWN";
        case Normal_C:
            return "INTERNAL";
        case Iso_C:
            return "ISO";
        case Usa_C:
            return "USA";
        case Eur_C:
            return "EUR";
        case Jis_C:
            return "JIS";
        case OracleDate_C:
            return "ORACLE";
        case WasAnsiNowIsSameAsIso_C:
            return "ISO";
        case TsEur_C:
            return "EUR";
        default:
            return "(unknown/illegal)";
        }
    }
};

/**
 * String representation of host type.
 * @param hosttype The host type for which a sttring representation 
 * @return A constant string describing the host type.
 */
const char *IFR_HostTypeToString(const IFR_HostType hosttype);

inline IFR_Bool 
IFR_HostTypeIsLOB(IFR_HostType hosttype)
{
    return  hosttype==IFR_HOSTTYPE_BLOB     || 
        hosttype == IFR_HOSTTYPE_ASCII_LOB  ||
        hosttype == IFR_HOSTTYPE_UTF8_LOB   || 
        hosttype == IFR_HOSTTYPE_UCS2_LOB ||
        hosttype == IFR_HOSTTYPE_UCS2_SWAPPED_LOB;
}

inline IFR_Bool
IFR_HostTypeIsIntegral(IFR_HostType hosttype)
{
    return 
        hosttype == IFR_HOSTTYPE_INT1   ||
        hosttype == IFR_HOSTTYPE_UINT1  ||
        hosttype == IFR_HOSTTYPE_INT2   ||
        hosttype == IFR_HOSTTYPE_UINT2  ||             
        hosttype == IFR_HOSTTYPE_INT4   ||
        hosttype == IFR_HOSTTYPE_UINT4  ||
        hosttype == IFR_HOSTTYPE_INT8   ||
        hosttype == IFR_HOSTTYPE_UINT8  ||
        hosttype == IFR_HOSTTYPE_DOUBLE ||
        hosttype == IFR_HOSTTYPE_FLOAT  ||
        hosttype == IFR_HOSTTYPE_DECIMAL ||
        hosttype == IFR_HOSTTYPE_ODBCDATE ||
        hosttype == IFR_HOSTTYPE_ODBCTIME ||
        hosttype == IFR_HOSTTYPE_ODBCTIMESTAMP ||
        hosttype == IFR_HOSTTYPE_ODBCNUMERIC ||
        hosttype == IFR_HOSTTYPE_GUID ||
        hosttype == IFR_HOSTTYPE_OMS_TIMESTAMP ||
        hosttype == IFR_HOSTTYPE_OMS_PACKED_15_3 ||
        hosttype == IFR_HOSTTYPE_OMS_PACKED_8_3;
}



struct IFR_LongDesc
{
  enum ValMode {
    DataPart_C = 0,
    AllData_C = 1,
    LastData_C = 2,
    NoData_C = 3,
    NoMoreData_C = 4,
    LastPutval_C = 5,
    DataTrunc_C = 6,
    Close_C = 7,
    Error_C = 8,
    StartposInvalid_C = 9
  };
};

IFR_END_NAMESPACE

#endif //IFRUTIL_DATATYPE_H
