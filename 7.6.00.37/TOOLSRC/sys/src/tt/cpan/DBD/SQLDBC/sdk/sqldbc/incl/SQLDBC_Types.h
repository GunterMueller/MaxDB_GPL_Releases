/*!
  @file           SQLDBC_Types.h
  @author         DL SAP DB INTERFACES
  @ingroup        SQLDBC
  @brief          Header file for basic types and macros
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2004 SAP AG

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
#ifndef SQLDBC_TYPES_H
#define SQLDBC_TYPES_H

/*
 * Define the platform setup.
 * 
 * WIN32           - Microsoft Windows 
 * LINUX           - Linux 
 * SUN             - Sun Solaris (Sparc)
 * SOLARIS         - Sun Solaris
 * HPUX            - HP Unix (HPPA)
 * AIX             - AIX (RS6000)
 * NMP             - Reliant.
 *
 * IA64            - IA 64 architecture.
 * BIT64           - 64 bit platform.
 */

#if defined(_WIN32) && !defined(WIN32)
#define WIN32
#endif 

#if defined(_AIX) && !defined(AIX)
#define AIX  1
#endif

#if defined(__linux__) && !defined(LINUX)
#define LINUX 1
#endif

#if defined(__osf__) && !defined(OSF1)
#define OSF1 1
#  if !defined(BIT64)
#    define BIT64 1
#  endif
#endif

#if defined(sun) && !defined(SUN)
#define SUN 1
#endif

#if defined(sun) && !defined(SPARC)
#defined SOLARIS 1
#endif

#if defined(__hpux) && !defined(HPUX)
#define HPUX 1
#endif

#if defined(sinix) && !defined(NMP)
#  define NMP
#endif

#if defined(__linux__) && defined(__ia64__) 
#  if !defined(IA64)
#    define IA64 1
#  endif
#  if !defined(BIT64)
#    define BIT64 1
#  endif
#endif

#if defined(_WIN64) 
#  if !defined(BIT64)
#    define BIT64 1
#  endif
#  if !defined(IA64)
#    define IA64 1
#  endif
#endif

#if defined(AIX) && defined(__64BIT__) && !defined(BIT64)
#  define BIT64 1
#endif

#if (defined(SUN) || defined(sun) || defined(SOLARIS))
#include <sys/isa_defs.h>
#  if defined(_LP64) && !defined(BIT64)
#    define BIT64 1
#  endif
#endif

#if (defined(HPUX)||defined(NMP)) && defined(__LP64__) && !defined(BIT64)
#  define BIT64 1
#endif

#if (!defined(WIN32) && !defined(AIX) && !defined(OSF1) && !defined(HPUX) && !defined(SUN) && !defined(SOLARIS) && !defined(LINUX) && !defined(NMP))
#error Unknown platform.
#endif

#if (defined (AIX) && __IBMCPP__ < 500) || ( (defined (SUN) || defined (SOLARIS))  && !defined(_BOOL) ) \
  || !defined (__cplusplus)
/** @ingroup SQLDBC
 * This macro has been defined for compilers to which the 
 * <code>bool</code> type is unknown,
 * which is the case with Sun and some AIX compilers.
 */
# define CPP_BOOL_NOT_IMPLEMENTED
#endif

#ifdef CPP_BOOL_NOT_IMPLEMENTED
#if defined(WIN32) || defined(AIX) || defined(LINUX) || defined(OSF1) \
 || defined(HPUX) || defined(NMP) || defined(SUN) || defined(SOLARIS)
/** @ingroup SQLDBC
 * Definition of the boolean type, this is either the C++ <code>bool</code>
 * type, or if missing a <tt>typedef</tt> to an <tt>unsigned char</tt>.
 */
typedef unsigned char SQLDBC_Bool; 
/** @ingroup SQLDBC
 * @deprecated
 * Type definition for the 'bool' type as a helper for old sources
 */
typedef SQLDBC_Bool  bool; 
#endif
#else
typedef bool SQLDBC_Bool;
#endif /* CPP_BOOL_NOT_IMPLEMENTED */

/** @ingroup SQLDBC
 * <code>true</code> value for the <code>SQLDBC_Bool</code> type. */
#define SQLDBC_TRUE          ((SQLDBC_Bool)1)
/** @ingroup SQLDBC 
 *<code>false</code> value for the <code>SQLDBC_Bool</code> type. */
#define SQLDBC_FALSE         ((SQLDBC_Bool)0)

#if defined(_WIN64)
typedef unsigned __int64    SQLDBC_ULong;      /* long (fast, unsigned) */
typedef signed __int64      SQLDBC_Long;       /* long (fast, signed)   */
#else
/** @ingroup SQLDBC 
 * An unsigned long type, which has 64-bit length on 64-bit
 * platforms, and 32 bit length on 32-bit platforms.
 */
typedef unsigned long       SQLDBC_ULong;      /* long (fast, unsigned) */
/** @ingroup SQLDBC 
 * A signed long type, which has 64-bit length on 64-bit
 * platforms, and 32 bit length on 32-bit platforms.
 */
typedef signed long         SQLDBC_Long;       /* long (fast, signed)   */
#endif

/** @ingroup SQLDBC
 * Type definition for an unsigned-byte.
 */
typedef unsigned char       SQLDBC_Byte;

/** @ingroup SQLDBC
 * Type definition for a memory size (<tt>size_t</tt>).
 */
typedef SQLDBC_ULong        SQLDBC_size_t;

/** @ingroup SQLDBC
 * Type definitions for an UTF8 character.
 */
typedef SQLDBC_Byte         SQLDBC_UTF8;

/** @ingroup SQLDBC 
 * Unsigned 1-byte integer. */
typedef unsigned char       SQLDBC_UInt1;
/** @ingroup SQLDBC 
 * Signed 1-byte integer. */
typedef signed char         SQLDBC_Int1;
/** @ingroup SQLDBC 
 * Unsigned 2-byte integer. */
typedef unsigned short      SQLDBC_UInt2;
/** @ingroup SQLDBC 
 * Signed 2-byte integer. */
typedef signed short        SQLDBC_Int2;
/** @ingroup SQLDBC 
 * Unsigned 4-byte integer. */
typedef unsigned int        SQLDBC_UInt4;
/** @ingroup SQLDBC 
 * Signed 4-byte integer. */
typedef signed int          SQLDBC_Int4;         

#if defined(WIN32) || defined(_WIN64)
/** @ingroup SQLDBC 
 * Unsigned 8-byte integer. */
typedef unsigned __int64    SQLDBC_UInt8;      
/** @ingroup SQLDBC 
 * Signed 8-byte integer. */
typedef signed __int64      SQLDBC_Int8;       
#else
/** @ingroup SQLDBC 
 * Unsigned 8-byte integer. */
typedef unsigned long long SQLDBC_UInt8;
/** @ingroup SQLDBC 
 * Signed 8-byte integer. */
typedef long long          SQLDBC_Int8;
#endif

#if defined(BIT64)
typedef SQLDBC_Int8        SQLDBC_Length;
#else
/**
 * @ingroup SQLDBC
 * Type definition for the length of certain argument.
 * This type is used for indicator/length variables in parameter binding.
 */
typedef SQLDBC_Int4        SQLDBC_Length;
#endif

/** @ingroup SQLDBC 
 *  Standard unsigned-integer type of the platform. */
typedef unsigned int       SQLDBC_UInt;

/** @ingroup SQLDBC 
 * Standard signed-integer type of the platform. */
typedef signed int         SQLDBC_Int;

/** @ingroup SQLDBC
 * Indicator of NULL data in input/output.
 */
#define SQLDBC_NULL_DATA     (-1)

/** @ingroup SQLDBC
 * Indicates that the data and length of the parameter will be known only at execution time.
 */
#define SQLDBC_DATA_AT_EXEC  (-2)

/** @ingroup SQLDBC
 * Indicator flag for the length of an zero-terminated string.
 */
#define SQLDBC_NTS           (-3)

/** @ingroup SQLDBC
 * Flag indicating that the SQLDBC cannot determine the total number of bytes.
 */
#define SQLDBC_NO_TOTAL      (-4)

/** @ingroup SQLDBC
 * Flag indicating that the default parameter is to be used.
 */
#define SQLDBC_DEFAULT_PARAM (-5)

/** @ingroup SQLDBC
 * Flag indicating that this parameter is to be ignored.
 */
#define SQLDBC_IGNORE        (-6)

/**
 * @ingroup SQLDBC
 * Offset for IFR_LEN_DATA_AT_EXEC.
 */
#define SQLDBC_LEN_DATA_AT_EXEC_OFFSET (-100)

/**
 * @ingroup SQLDBC
 * Macro for late binding.
 * @param length The length that the parameter will have. This macro is currently unused.
 */
#define SQLDBC_LEN_DATA_AT_EXEC(length) (-(length)+SQLDBC_LEN_DATA_AT_EXEC_OFFSET)

/**
 * @ingroup SQLDBC
 * Return code of functions. This is not an error code,
 * it only indicates the status of the function call. 
 */
#ifndef __cplusplus
typedef
#endif
enum SQLDBC_Retcode { 
     SQLDBC_INVALID_OBJECT           =-10909, //!< Application tries to use an invalid object reference. 
     SQLDBC_OK                       = 0,    //!< Function call successful.
     SQLDBC_NOT_OK                   = 1,    //!< Function call not successful. Further information 
                                             //!< can be found in the corresponding error object. 
     SQLDBC_DATA_TRUNC               =2,    //!< Data was truncated during the call. 
     SQLDBC_OVERFLOW                 =3,    //!< Signalizes a numeric overflow.  
     SQLDBC_SUCCESS_WITH_INFO        =4,    //!< The method succeeded with warnings. 
     SQLDBC_NO_DATA_FOUND            =100,  //!< Data was not found.
     SQLDBC_NEED_DATA                =99    //!< Late binding, data is needed for execution.
}
#ifndef __cplusplus
SQLDBC_Retcode
#endif
;

/**
 * @ingroup SQLDBC
 * Row status for mass operations. The status for an inserted row is either one
 * of these constants, or the row count of the individual column.
 */
#ifndef __cplusplus
typedef
#endif
enum SQLDBC_Rowstatus
{
    SQLDBC_EXECUTE_FAILED            = -2,      //!< Execute of this row failed.
    SQLDBC_SUCCESS_NO_INFO           = -3       //!< Execute succeeded, no information about 
                                                //!< affected rows available.
}
#ifndef __cplusplus
SQLDBC_Rowstatus
#endif
;


/**
 * @ingroup SQLDBC
 * possible SQL modes. 
 */
#ifndef __cplusplus
typedef
#endif
enum SQLDBC_SQLMode {
     SQLDBC_INTERNAL                 =2,    //!< SQL mode INTERNAL.
     SQLDBC_ANSI                     =3,    //!< @deprecated SQL mode ANSI.
     SQLDBC_DB2                      =4,    //!< @deprecated SQL mode DB2.
     SQLDBC_ORACLE                   =5,    //!< SQL mode ORACLE.
     SQLDBC_SAPR3                    =6     //!< SQL mode used for SAP R/3  
}
#ifndef __cplusplus
SQLDBC_SQLMode
#endif
;

#ifdef __cplusplus
class SQLDBC_StringEncodingType
{
public:
  enum Encoding {
    Unknown     = 0,
    Ascii       = 1,
    UCS2        = 2,
    UCS2Swapped = 3,
    UTF8        = 4
  };      
};
typedef SQLDBC_StringEncodingType::Encoding SQLDBC_StringEncoding;
#endif 
typedef enum SQLDBC_StringEncodingType_Encoding
{
    SQLDBC_StringEncodingType_Encoding_Unknown     = 0,
    SQLDBC_StringEncodingType_Encoding_Ascii       = 1,
    SQLDBC_StringEncodingType_Encoding_UCS2        = 2,
    SQLDBC_StringEncodingType_Encoding_UCS2Swapped = 3,
    SQLDBC_StringEncodingType_Encoding_UTF8        = 4
}SQLDBC_StringEncodingType_Encoding;

/**
 * @ingroup SQLDBC
 * Unknown encoding (do not use).
 */
#define SQLDBC_StringEncodingUnknown      SQLDBC_StringEncodingType::Unknown

/**
 * @ingroup SQLDBC
 * Plain 8-bit ASCII encoding (each byte stands for itself).
 */
#define SQLDBC_StringEncodingAscii        SQLDBC_StringEncodingType::Ascii

/**
 * @ingroup SQLDBC
 * UCS2 big-endian encoding.
 */
#define SQLDBC_StringEncodingUCS2         SQLDBC_StringEncodingType::UCS2

/**
 * @ingroup SQLDBC
 * UCS2 little-endian encoding.
 */
#define SQLDBC_StringEncodingUCS2Swapped  SQLDBC_StringEncodingType::UCS2Swapped

/**
 * @ingroup SQLDBC
 * UCS2 little-endian encoding.
 */
#if defined(WIN32) || defined(LINUX) || defined(OSF1)
#define SQLDBC_StringEncodingUCS2Native SQLDBC_StringEncodingType::UCS2Swapped
#else
#define SQLDBC_StringEncodingUCS2Native SQLDBC_StringEncodingType::UCS2
#endif

/**
 * @ingroup SQLDBC 
 * UTF8 encoding. 
 */
#define SQLDBC_StringEncodingUTF8         SQLDBC_StringEncodingType::UTF8

/**
 * @ingroup SQLDBC
 * Constants used for SQL data types. These constants are delivered in the short
 * infos of the database or provided by the application at parameter binding time.
 */
#ifndef __cplusplus
typedef
#endif
enum SQLDBC_SQLType {
  SQLDBC_SQLTYPE_MIN       = 0,            //!< Minimum value internally used for checks.
  SQLDBC_SQLTYPE_FIXED     = SQLDBC_SQLTYPE_MIN, //!< Data type <code>FIXED</code>.
  SQLDBC_SQLTYPE_FLOAT     = 1,            //!< Data type <code>FLOAT</code>.
  SQLDBC_SQLTYPE_CHA       = 2,            //!< Data type <code>CHAR ASCII</code>.
  SQLDBC_SQLTYPE_CHE       = 3,            //!< @deprecated Data type <code>CHAR EBCDIC</code>.
  SQLDBC_SQLTYPE_CHB       = 4,            //!< Data type <code>CHAR BYTE</code>.
  SQLDBC_SQLTYPE_ROWID     = 5,            //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_STRA      = 6,            //!< Data type <code>LONG ASCII</code>.
  SQLDBC_SQLTYPE_STRE      = 7,            //!< @deprecated Data type <code>LONG EBCDIC</code>.
  SQLDBC_SQLTYPE_STRB      = 8,            //!< Data type <code>LONG BYTE</code>.
  SQLDBC_SQLTYPE_STRDB     = 9,            //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_DATE      = 10,           //!< Data type <code>DATE</code> (SQL mode INTERNAL).
  SQLDBC_SQLTYPE_TIME      = 11,           //!< Data type <code>TIME</code> (SQL mode INTERNAL).
  SQLDBC_SQLTYPE_VFLOAT    = 12,           //!< Data type <code>FLOAT</code> (output of arithmetic expressions).
  SQLDBC_SQLTYPE_TIMESTAMP = 13,           //!< Data type <code>TIMESTAMP</code> (SQL mode INTERNAL), or <code>DATE</code> (SQL mode Oracle).
  SQLDBC_SQLTYPE_UNKNOWN   = 14,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_NUMBER    = 15,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_NONUMBER  = 16,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_DURATION  = 17,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_DBYTEEBCDIC = 18,         //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_LONGA     = 19,           //!< @deprecated Data type <code>LONG ASCII</code>.
  SQLDBC_SQLTYPE_LONGE     = 20,           //!< @deprecated Data type <code>LONG EBCDIC</code>.
  SQLDBC_SQLTYPE_LONGB     = 21,           //!< @deprecated Data type <code>LONG BYTE</code>.
  SQLDBC_SQLTYPE_LONGDB    = 22,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_BOOLEAN   = 23,           //!< Data type <code>BOOLEAN</code>.
  SQLDBC_SQLTYPE_UNICODE   = 24,           //!< Data type <code>CHAR UNICODE</code>.
  SQLDBC_SQLTYPE_DTFILLER1 = 25,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_DTFILLER2 = 26,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_DTFILLER3 = 27,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_DTFILLER4 = 28,           //!< @deprecated Internally used.
  SQLDBC_SQLTYPE_SMALLINT  = 29,           //!< Data type <code>SMALLINT</code>.
  SQLDBC_SQLTYPE_INTEGER   = 30,           //!< Data type <code>INTEGER</code>.
  SQLDBC_SQLTYPE_VARCHARA  = 31,           //!< Data type <code>VARCHAR ASCII</code>.
  SQLDBC_SQLTYPE_VARCHARE  = 32,           //!< @deprecated Data type <code>VARCHAR EBCDIC</code>.
  SQLDBC_SQLTYPE_VARCHARB  = 33,           //!< Data type <code>VARCHAR BYTE</code>.
  SQLDBC_SQLTYPE_STRUNI    = 34,           //!< Data type <code>LONG UNICODE</code>.
  SQLDBC_SQLTYPE_LONGUNI   = 35,           //!< @deprecated Data type <code>LONG UNICODE</code>.
  SQLDBC_SQLTYPE_VARCHARUNI = 36,          //!< Data type <code>VARCHAR UNICODE</code>.
  SQLDBC_SQLTYPE_UDT       = 37,           //!< Data type used for C++ Stored Procedures.
  SQLDBC_SQLTYPE_ABAPTABHANDLE = 38,       //!< Data type used for C++ Stored Procedures.
  SQLDBC_SQLTYPE_DWYDE     = 39,           //!< Data type used for C++ Stored Procedures.
  SQLDBC_SQLTYPE_MAX = SQLDBC_SQLTYPE_DWYDE            //!< Maximum value for checks.
}
#ifndef __cplusplus
SQLDBC_SQLType
#endif
;

/**
 * @ingroup SQLDBC
 * Types of host (native) variables. 
 */
#ifndef __cplusplus
typedef
#endif
enum SQLDBC_HostType {
    SQLDBC_HOSTTYPE_MIN         = 0, //!< Minimum value for checks.

    SQLDBC_HOSTTYPE_PARAMETER_NOTSET = 0, //!< The default value for unset parameters.

    
    SQLDBC_HOSTTYPE_BINARY      =  1, //!< Binary data, length must be provided. No conversion is applied.
    
    SQLDBC_HOSTTYPE_ASCII       =  2, //!< An zero-terminated ASCII string zero terminated.

    SQLDBC_HOSTTYPE_UTF8        =  4, //!< An zero-terminated UTF8 string.

    SQLDBC_HOSTTYPE_UINT1       =  5, //!< An unsigned 8-bit integer.
    SQLDBC_HOSTTYPE_INT1        =  6, //!< A signed 8-bit integer.

    SQLDBC_HOSTTYPE_UINT2       =  7,  //!< An unsigned 16-bit integer.
    SQLDBC_HOSTTYPE_INT2        =  8, //!< A signed 16-bit integer.

    SQLDBC_HOSTTYPE_UINT4       =  9, //!< An unsigned 32-bit integer.
    SQLDBC_HOSTTYPE_INT4        = 10, //!< A signed 32-bit integer.

    SQLDBC_HOSTTYPE_UINT8       = 11, //!< An unsigned 64-bit integer.
    SQLDBC_HOSTTYPE_INT8        = 12, //!< A signed 64-bit integer.
    
    SQLDBC_HOSTTYPE_DOUBLE      = 13, //!< A <code>double</code> value.
    SQLDBC_HOSTTYPE_FLOAT       = 14, //!< A <code>float</code> value.

    SQLDBC_HOSTTYPE_ODBCDATE    = 15, //!< An ODBC <code>SQL_DATE_STRUCT</code> struct.
    SQLDBC_HOSTTYPE_ODBCTIME    = 16, //!< An ODBC <code>SQL_TIME_STRUCT</code> struct.
    SQLDBC_HOSTTYPE_ODBCTIMESTAMP = 17, //!< An ODBC <code>SQL_TIMESTAMP_STRUCT</code> struct.

    SQLDBC_HOSTTYPE_ODBCNUMERIC = 18, //!< An ODBC <code>SQL_NUMERIC_STRUCT</code> struct.

    SQLDBC_HOSTTYPE_GUID        = 19, //!< An <code>GUID</code>. 
       
    SQLDBC_HOSTTYPE_UCS2        =  20, //!< An zero-terminated UCS2 string (in none swapped byte order ASCII 'A' is coded as '0x0041).
    SQLDBC_HOSTTYPE_UCS2_SWAPPED=  21, //!< An zero-terminated UCS2 string (in swapped byte order ASCII 'A' is coded as 0x4100').

#if defined(WIN32) || defined(LINUX) || defined(OSF1)
    SQLDBC_HOSTTYPE_UCS2_NATIVE =  SQLDBC_HOSTTYPE_UCS2_SWAPPED, //!< An zero-terminated UCS2 string (in native byte order).
#else
    SQLDBC_HOSTTYPE_UCS2_NATIVE =  SQLDBC_HOSTTYPE_UCS2, //!< An zero-terminated UCS2 string (in native byte order).
#endif
    SQLDBC_HOSTTYPE_BLOB         = 22, //!< Large binary object (pointer to LOB handle).
    SQLDBC_HOSTTYPE_ASCII_CLOB   = 23, //!< Large character object (pointer to LOB handle).
    SQLDBC_HOSTTYPE_UTF8_CLOB    = 24, //!< Large unicode character object (pointer to LOB handle).
    SQLDBC_HOSTTYPE_UCS2_CLOB    = 25, //!< Large unicode character object (pointer to LOB handle).
    SQLDBC_HOSTTYPE_UCS2_SWAPPED_CLOB = 26, //!< Large unicode character object (pointer to LOB handle).
#if defined(WIN32) || defined(LINUX) || defined(OSF1)
    SQLDBC_HOSTTYPE_UCS2_NATIVE_CLOB =  SQLDBC_HOSTTYPE_UCS2_SWAPPED_CLOB, 
#else
    SQLDBC_HOSTTYPE_UCS2_NATIVE_CLOB =  SQLDBC_HOSTTYPE_UCS2_CLOB, 
#endif
    SQLDBC_HOSTTYPE_STREAM      = 27,  //!< ABAP/OMS Stream.

    SQLDBC_HOSTTYPE_USERDEFINED = 100, //!< User-defined, must supply a parameter converter.

    SQLDBC_HOSTTYPE_MAX = SQLDBC_HOSTTYPE_USERDEFINED  //!< Maximum value for checks.
}
#ifndef __cplusplus
SQLDBC_HostType
#endif
;

/**
 * A list of all kinds of warnings that are supported by this interfaces.
 */
#ifndef __cplusplus
typedef
#endif
enum SQLDBC_SQLWarningCode
{
    SQLDBC_SQLWARNING_DATA_TRUNCATED = 1, //!< Input or output data has been truncated.
    SQLDBC_SQLWARNING_MAX                 //!< For checks.
}
#ifndef __cplusplus
SQLDBC_SQLWarningCode
#endif
;
#ifdef __cplusplus
struct SQLDBC_DateTimeFormat 
{
    /**
     * The possible date/time formats.
     */
    enum Format {
        None_C = 0,      //!< Unknown, not set.
        Normal_C = 1,    //!< Normal (aka INTERNAL).
        Iso_C = 2,       //!< ISO    (ODBC, JDBC)
        Usa_C = 3,       //!< USA
        Eur_C = 4,       //!< Europe
        Jis_C = 5,       //!< Japan
        OracleDate_C = 6, //!< @deprecated Unused.
        WasAnsiNowIsSameAsIso_C = 7, //!< @deprecated Unused
        TsEur_C = 8      //!< @deprecated (same as Eur_C).
    };
};
#endif

    /**
     * The possible date/time formats.
     */
typedef  enum SQLDBC_DateTimeFormat_Format {
        SQLDBC_DateTimeFormat_Format_None_C = 0,      //!< Unknown, not set.
        SQLDBC_DateTimeFormat_Format_Normal_C = 1,    //!< Normal (aka INTERNAL).
        SQLDBC_DateTimeFormat_Format_Iso_C = 2,       //!< ISO    (ODBC, JDBC)
        SQLDBC_DateTimeFormat_Format_Usa_C = 3,       //!< USA
        SQLDBC_DateTimeFormat_Format_Eur_C = 4,       //!< Europe
        SQLDBC_DateTimeFormat_Format_Jis_C = 5,       //!< Japan
        SQLDBC_DateTimeFormat_Format_OracleDate_C = 6, //!< @deprecated Unused.
        SQLDBC_DateTimeFormat_Format_WasAnsiNowIsSameAsIso_C = 7, //!< @deprecated Unused
        SQLDBC_DateTimeFormat_Format_TsEur_C = 8      //!< @deprecated (same as Eur_C).
    }SQLDBC_DateTimeFormat_Format;

#endif
