/*!
  \file    Loader_Types.hpp
  \author  SteffenS
  \brief   main include for FASTLOAD API

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

#ifndef LOADER_TYPES_HPP
#define LOADER_TYPES_HPP


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

/* Do not set anything if BIT64 is set from MaxDB Development Tools */
#if !defined(BIT64)

#if defined(__osf__) 
#    define BIT64 1
#endif

#if (defined(__linux__) && defined(__ia64__))
#    define BIT64 1
#endif

#if (defined(__linux__) && defined(__s390x__)) 
/* __s390x__ is 64-bit, __s390__ would be 32 bit. */
#    define BIT64 1
#endif

#if (defined(__linux__) && defined(__x86_64__) )
#    define BIT64 1
#endif

#if (defined(__linux__) && defined(__powerpc64__))
/* __powerpc__ would be 32 bit. */
#    define BIT64 1
#endif

#if defined(_WIN64)
#    define BIT64 1
#endif

#if defined(_AIX) && defined(__64BIT__) 
#  define BIT64 1
#endif

#if (defined(sun) || defined(__sun)) 
#  include <sys/isa_defs.h>
#  if defined(_LP64) 
#    define BIT64 1
#  endif
#endif

#if (defined(__hpux)||defined(sinix)) && defined(__LP64__)
#  define BIT64 1
#endif

/* #if !defined(BIT64) */
#endif


#if (defined (_AIX) && __IBMCPP__ < 500) || ( (defined(sun) || defined(__sun)) && !defined(_BOOL) ) \
  || !defined (__cplusplus)
/** 
 * This macro has been defined for compilers to which the
 * <code>bool</code> type is unknown,
 * which is the case with Sun and some AIX compilers.
 */
# define CPP_BOOL_NOT_IMPLEMENTED
#endif

#ifdef CPP_BOOL_NOT_IMPLEMENTED
#if defined(_WIN32) || defined(_AIX) || defined(__linux__) || defined(__osf__) \
 || defined(__hpux) || defined(sinix) || (defined(sun) || defined(__sun))
/** 
 * Definition of the boolean type, this is either the C++ <code>bool</code>
 * type, or if missing a <tt>typedef</tt> to an <tt>unsigned char</tt>.
 */
typedef unsigned char LOADER_Bool;
/** 
 * @deprecated
 * Type definition for the 'bool' type as a helper for old sources
 */
typedef LOADER_Bool  bool;
#endif
#else
typedef bool LOADER_Bool;
#endif 


#if defined(_WIN32) \
    || (defined(__linux__) && !defined(__powerpc__) && !defined(__s390x__)) \
    ||  defined(__osf__) \
    || (defined(__sun) && (defined(__i386) || defined(__amd64)))
#   define LOADER_SWAPPED_UCS2
#endif


/** 
 * <code>true</code> value for the <code>LOADER_Bool</code> type. */
#define LOADER_TRUE          ((LOADER_Bool)1)
/** 
 *<code>false</code> value for the <code>LOADER_Bool</code> type. */
#define LOADER_FALSE         ((LOADER_Bool)0)

#if defined(_WIN64)
typedef unsigned __int64    LOADER_ULong;      /* long (fast, unsigned) */
typedef signed __int64      LOADER_Long;       /* long (fast, signed)   */
#else
/** 
 * An unsigned long type, which has 64-bit length on 64-bit
 * platforms, and 32 bit length on 32-bit platforms.
 */
typedef unsigned long       LOADER_ULong;      /* long (fast, unsigned) */
/** 
 * A signed long type, which has 64-bit length on 64-bit
 * platforms, and 32 bit length on 32-bit platforms.
 */
typedef signed long         LOADER_Long;       /* long (fast, signed)   */
#endif

/** 
 * Type definition for an unsigned-byte.
 */
typedef unsigned char       LOADER_Byte;


/*--------------------------------------------------------------------------
declaration: LOADER_{Byte | Char}
description: basic data types with size 1 byte
--------------------------------------------------------------------------*/
typedef unsigned char       LOADER_Byte;         /* byte (1 byte, unsigned) */
typedef char                LOADER_Char;         /* char (1 byte, signed)   */


/*--------------------------------------------------------------------------
declaration: LOADER_{UInt1 | Int1 | UInt2 | Int2 | UInt4 | Int4}
description: basic integer data types with named size (1, 2 and 4 bytes)
--------------------------------------------------------------------------*/
typedef unsigned char       LOADER_UInt1;        /* integer (1 byte, unsigned) */
typedef signed char         LOADER_Int1;         /* integer (1 byte, signed)   */
typedef unsigned short      LOADER_UInt2;        /* integer (2 byte, unsigned) */
typedef signed short        LOADER_Int2;         /* integer (2 byte, signed)   */
typedef unsigned int        LOADER_UInt4;        /* integer (4 byte, unsigned) */
typedef signed int          LOADER_Int4;         /* integer (4 byte, signed)   */


/*--------------------------------------------------------------------------
declaration: LOADER_{UInt | Int}
description: basic integer data types for fastest execution
             (machine-dependent size)
--------------------------------------------------------------------------*/
typedef unsigned int        LOADER_UInt;      /* integer (fast, unsigned) */
typedef signed int          LOADER_Int;       /* integer (fast, signed)   */

/*--------------------------------------------------------------------------
declaration: LOADER_{ULong | Long}
description: basic long data types for fastest execution
             (machine-dependent size)
--------------------------------------------------------------------------*/
#if defined(_WIN64)
typedef unsigned __int64    LOADER_ULong;      /* long (fast, unsigned) */
typedef signed __int64      LOADER_Long;       /* long (fast, signed)   */
#else
typedef unsigned long       LOADER_ULong;      /* long (fast, unsigned) */
typedef signed long         LOADER_Long;       /* long (fast, signed)   */
#endif


#if defined(_WIN32) || defined(_WIN64)
/* Unsigned 8-byte integer. */
typedef unsigned __int64    LOADER_UInt8;
/* Signed 8-byte integer. */
typedef signed __int64      LOADER_Int8;
#else
/* Unsigned 8-byte integer. */
typedef unsigned long long LOADER_UInt8;
/* Signed 8-byte integer. */
typedef long long          LOADER_Int8;
#endif

/*--------------------------------------------------------------------------
declaration: LOADER_{UTF8 | UTF16 | UCS2 | UCS4 }
description: Basis element types of unicode characters
--------------------------------------------------------------------------*/
typedef LOADER_Byte             LOADER_UTF8;
typedef LOADER_UInt2            LOADER_UTF16;
typedef LOADER_UInt2            LOADER_UCS2;
typedef LOADER_UInt4            LOADER_UCS4;

#if defined(BIT64)
typedef LOADER_Int8        LOADER_Length;
#else
/**
 * @ingroup SQLDBC
 * Type definition for the length of certain argument.
 * This type is used for indicator/length variables in parameter binding.
 */
typedef LOADER_Int4        LOADER_Length;
#endif


// Length indicator values
#define LOADER_NULL_DATA        -1      // keep it synchronous with ODBC and SQLDBC
#define LOADER_NTS              -3      // keep it synchronous with ODBC and SQLDBC


//#define LOADER_KNLID_SIZE 65
const short  LOADER_KNL_ID_SIZE = 64; // = sizeof(tsp00_KnlIdentifier); --> not including the null termination byte

typedef struct LOADER_String
{
    LOADER_UInt4  lStringLength;
    LOADER_Char*  pszStringData;
}
LOADER_String;


enum LOADER_SQLType
{
  LOADER_SQLTYPE_MIN       = 0,            /*!< Minimum value internally used for checks. */
  LOADER_SQLTYPE_FIXED     = LOADER_SQLTYPE_MIN, /*!< Data type <code>FIXED</code>. */
  LOADER_SQLTYPE_FLOAT     = 1,            /*!< Data type <code>FLOAT</code>. */
  LOADER_SQLTYPE_CHA       = 2,            /*!< Data type <code>CHAR ASCII</code>. */
  LOADER_SQLTYPE_CHE       = 3,            /*!< @deprecated Data type <code>CHAR EBCDIC</code>. */
  LOADER_SQLTYPE_CHB       = 4,            /*!< Data type <code>CHAR BYTE</code>. */
  LOADER_SQLTYPE_ROWID     = 5,            /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_STRA      = 6,            /*!< Data type <code>LONG ASCII</code>. */
  LOADER_SQLTYPE_STRE      = 7,            /*!< @deprecated Data type <code>LONG EBCDIC</code>. */
  LOADER_SQLTYPE_STRB      = 8,            /*!< Data type <code>LONG BYTE</code>. */
  LOADER_SQLTYPE_STRDB     = 9,            /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_DATE      = 10,           /*!< Data type <code>DATE</code> (SQL mode INTERNAL). */
  LOADER_SQLTYPE_TIME      = 11,           /*!< Data type <code>TIME</code> (SQL mode INTERNAL). */
  LOADER_SQLTYPE_VFLOAT    = 12,           /*!< Data type <code>FLOAT</code> (output of arithmetic expressions). */
  LOADER_SQLTYPE_TIMESTAMP = 13,           /*!< Data type <code>TIMESTAMP</code> (SQL mode INTERNAL), or <code>DATE</code> (SQL mode Oracle). */
  LOADER_SQLTYPE_UNKNOWN   = 14,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_NUMBER    = 15,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_NONUMBER  = 16,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_DURATION  = 17,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_DBYTEEBCDIC = 18,         /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_LONGA     = 19,           /*!< @deprecated Data type <code>LONG ASCII</code>. */
  LOADER_SQLTYPE_LONGE     = 20,           /*!< @deprecated Data type <code>LONG EBCDIC</code>. */
  LOADER_SQLTYPE_LONGB     = 21,           /*!< @deprecated Data type <code>LONG BYTE</code>. */
  LOADER_SQLTYPE_LONGDB    = 22,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_BOOLEAN   = 23,           /*!< Data type <code>BOOLEAN</code>. */
  LOADER_SQLTYPE_UNICODE   = 24,           /*!< Data type <code>CHAR UNICODE</code>. */
  LOADER_SQLTYPE_DTFILLER1 = 25,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_DTFILLER2 = 26,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_DTFILLER3 = 27,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_DTFILLER4 = 28,           /*!< @deprecated Internally used. */
  LOADER_SQLTYPE_SMALLINT  = 29,           /*!< Data type <code>SMALLINT</code>. */
  LOADER_SQLTYPE_INTEGER   = 30,           /*!< Data type <code>INTEGER</code>. */
  LOADER_SQLTYPE_VARCHARA  = 31,           /*!< Data type <code>VARCHAR ASCII</code>. */
  LOADER_SQLTYPE_VARCHARE  = 32,           /*!< @deprecated Data type <code>VARCHAR EBCDIC</code>. */
  LOADER_SQLTYPE_VARCHARB  = 33,           /*!< Data type <code>VARCHAR BYTE</code>. */
  LOADER_SQLTYPE_STRUNI    = 34,           /*!< Data type <code>LONG UNICODE</code>. */
  LOADER_SQLTYPE_LONGUNI   = 35,           /*!< @deprecated Data type <code>LONG UNICODE</code>. */
  LOADER_SQLTYPE_VARCHARUNI = 36,          /*!< Data type <code>VARCHAR UNICODE</code>. */
  LOADER_SQLTYPE_UDT       = 37,           /*!< Data type used for C++ Stored Procedures. */
  LOADER_SQLTYPE_ABAPTABHANDLE  = 38,       /*!< Data type used for C++ Stored Procedures. */
  LOADER_SQLTYPE_DWYDE     = 39,           /*!< Data type used for C++ Stored Procedures. */
  LOADER_SQLTYPE_MAX = LOADER_SQLTYPE_DWYDE            /*!< Maximum value for checks. */
};


// von tsp00_SwapKind
typedef enum LOADER_SwapKind
{
    LOADER_SWAPKIND_DUMMY           = 0,
    LOADER_SWAPKIND_NORMAL          = 1,
    LOADER_SWAPKIND_SWAPPED         = 2,
    LOADER_SWAPKIND_PART_SWAPPED    = 3
} LOADER_SwapKind;


typedef struct LOADER_SQLColumnBasic
{
    LOADER_SQLType          SQLDataType;            // sp7fi_data_type (tsp00_DataType)
    LOADER_Int1              SQLFraction;            // sp7fi_frac
    LOADER_Int2              SQLLength;              // sp7fi_len
    LOADER_Int2              SQLInOutLength;         // sp7fi_in_out_l
} LOADER_SQLColumnBasic;


typedef enum LOADER_SQLColumnMode
{
    LOADER_COLMODE_SYSKEY        = 0,
    LOADER_COLMODE_KEY           = 1,
    LOADER_COLMODE_MANDATORY     = 2,
    LOADER_COLMODE_OPTIONAL      = 3,
    LOADER_COLMODE_DROPPED       = 4
} LOADER_SQLColumnMode;


typedef enum LOADER_SQLColumnLengthType
{
    LOADER_COLLENTYPE_FIX        = 0,
    LOADER_COLLENTYPE_VAR        = 1,
    LOADER_COLLENTYPE_VAR_LONG   = 2
} LOADER_SQLColumnLengthType;


typedef enum LOADER_SQLDefault              // corresponds to tsp7_default_Enum
{
    LOADER_SQLDEFAULT_NONE,
    LOADER_SQLDEFAULT_USER,
    LOADER_SQLDEFAULT_USERGROUP,
    LOADER_SQLDEFAULT_DATE,
    LOADER_SQLDEFAULT_TIME,
    LOADER_SQLDEFAULT_STAMP,
    LOADER_SQLDEFAULT_TIMESTAMP,
    LOADER_SQLDEFAULT_UID,
    LOADER_SQLDEFAULT_TRUE,
    LOADER_SQLDEFAULT_FALSE,
    LOADER_SQLDEFAULT_OTHER,
    LOADER_SQLDEFAULT_SERIAL,
    LOADER_SQLDEFAULT_UTCDATE,
    LOADER_SQLDEFAULT_UTCDIFF,
    LOADER_SQLDEFAULT_TIMEZONE,
    LOADER_SQLDEFAULT_SYSDBA
} LOADER_SQLDefault;

// corresponds to IFR_StringEncodingType
typedef enum LOADER_StringEncoding
{
    LOADER_STRINGENCODING_UNKNOWN     = 0,
    LOADER_STRINGENCODING_ASCII       = 1,
    LOADER_STRINGENCODING_UCS2        = 2,
    LOADER_STRINGENCODING_UCS2SWAPPED = 3,
    LOADER_STRINGENCODING_UTF8        = 4,
    LOADER_STRINGENCODING_UCS2NATIVE  = 5,
    LOADER_STRINGENCODING_BINARY      = 6    
} LOADER_StringEncoding;



// Diese struktur ist angelehnt an tsp7_ffield_info, hat allerdings den Filler nicht und dafuer die
// zusaetzliche Variable DefaultValue
typedef struct LOADER_SQLColumnInfo                         //tsp7_ffield_info
{
    LOADER_Int2                     StmtColumnNo;           // sp7fi_stmt_col_no
    LOADER_SQLColumnBasic           ColumnBasics;
    LOADER_Int2                     DefinitionColumnNo;       // sp7fi_def_col_no
    LOADER_SQLColumnMode            ColumnMode;             // sp7fi_col_mode
    LOADER_SQLColumnLengthType      LengthType;             // sp7fi_len_type
    LOADER_SQLDefault               DefaultType;            // sp7fi_default
    LOADER_String                   DefaultValue;
} LOADER_SQLColumnInfo;




// Die folgende struktur ist eine Nachbildung der struktur tsp7_table_description aus vsp007.
// Dabei wurde allerdings auf die union verzichtet und die struktur tsp7_table_description_false
// nicht genutzt, da damit sonst sofort 80k speicher belegt werden wuerden.
// Ausserdem ist zusaetzlich eine struktur aufgenommen worden, die die spaltennamen der
// tabelle enthalten kann. Diese muss aber von der jeweiligen funktionalitaet nach eigenen
// erfordernissen gefuellt werden.

// Diese Struktur wird für FASTLOAD und allgemein fuer das Extrahieren/Laden von PAGES benoetigt
/*
typedef struct Loader_TableDescription
{
    LOADER_Char*           pszAuthID;              // DB Identifier: max 32 char (= 64 Byte in UNICODE case)
    LOADER_Char*           pszTableName;           // DB Identifier: max 32 char (= 64 Byte in UNICODE case)

    LOADER_Char**          pszColumnNames;         // DB Identifiers (array of ptrs): max 32 char (= 64 Byte in UNICODE case)

    LOADER_Int2            lColumnCount;            // sp7t_field_cnt_F
    LOADER_SwapKind        KernelSwap;             // sp7t_kernel_swap_F
    LOADER_Int2            lKernelLoc;             // sp7t_kernel_loc_F
    LOADER_Int2            lConstraintCount;       // sp7t_constraint_cnt_F
    LOADER_Int2            lFiller;                // sp7t_filler_F

    LOADER_SQLColumnInfo*  pColumnInfoArray;           // sp7t_fields_F    csp_max_fields = 1024
} Loader_TableDescription;
*/

typedef enum LOADER_SQLMode
{
     LOADER_SQLMODE_INTERNAL    = 2,    /*!< SQL mode INTERNAL. */
     LOADER_SQLMODE_ANSI        = 3,    /*!< @deprecated SQL mode ANSI. */
     LOADER_SQLMODE_DB2         = 4,    /*!< @deprecated SQL mode DB2. */
     LOADER_SQLMODE_ORACLE      = 5,    /*!< SQL mode ORACLE. */
     LOADER_SQLMODE_SAPR3       = 6     /*!< SQL mode used for SAP R/3 */
} LOADER_SQLMode;


/**
 * @ingroup LOADER
 * Macro for decimal binding.
 * @param digits The number of decimal digits.
 * @param fraction The fraction.
 * @return A value that can be assigned to an indicator to describe
 *         the structure of a BCD value.
 */
#define LOADER_LEN_DECIMAL(digits, fraction)        SQLDBC_LEN_DECIMAL(digits, fraction)

/**
 * @ingroup LOADER
 * Macro for length of decimal number.
 * @param digits The number of decimal digits.
 * @param fraction The fraction.
 * @return The length in bytes needed to store a BCD number with 
 *         @c digits digits, having a fraction of @c fraction.
 */
#define LOADER_DECIMAL_LENGTH(digits, fraction)     SQLDBC_DECIMAL_LENGTH(digits, fraction)


/**
 * @ingroup LOADER
 * Macro for extracting the number of digits of an indicator value.
 * @param indicator The indicator value. <b>The value is used twice in this macro.</b>
 * @return The number of digits, or -1 if the indicator value is invalid.
 */ 
#define LOADER_DECIMAL_DIGITS(indicator)            SQLDBC_DECIMAL_DIGITS(indicator)


/**
 * @ingroup LOADER
 * Macro for extracting the number of digits of an indicator value.
 * @param indicator The indicator value. <b>The value is used twice in this macro.</b>
 * @return The number of digits, or -1 if the indicator value is invalid.
 */ 
#define LOADER_DECIMAL_FRACTION(indicator) SQLDBC_DECIMAL_FRACTION(indicator)


typedef enum LOADER_HostType
{
    LOADER_HostType_MIN                             = 0,        // < Minimum value for checks.

    LOADER_HostType_PARAMETER_NOTSET                = 0,   // < The default value for unset parameters.


    LOADER_HostType_BINARY                          =  1,       // < Binary data, length must be provided. No conversion is applied.

    LOADER_HostType_ASCII                           =  2,       // < An zero-terminated ASCII string zero terminated.

    LOADER_HostType_UTF8                            =  4,       // < An zero-terminated UTF8 string.

    LOADER_HostType_UINT1                           =  5,       // < An unsigned 8-bit integer.
    LOADER_HostType_INT1                            =  6,       // < A signed 8-bit integer.

    LOADER_HostType_UINT2                           =  7,       // < An unsigned 16-bit integer.
    LOADER_HostType_INT2                            =  8,       // < A signed 16-bit integer.

    LOADER_HostType_UINT4                           =  9,       // < An unsigned 32-bit integer.
    LOADER_HostType_INT4                            = 10,       // < A signed 32-bit integer.

    LOADER_HostType_UINT8                           = 11,       // < An unsigned 64-bit integer.
    LOADER_HostType_INT8                            = 12,       // < A signed 64-bit integer.

    LOADER_HostType_DOUBLE                          = 13,       // < A <code>double</code> value.
    LOADER_HostType_FLOAT                           = 14,       // < A <code>float</code> value.

    LOADER_HostType_UCS2                            =  20,      // < An zero-terminated UCS2 string (in none swapped byte order ASCII 'A' is coded as '0x0041).
    LOADER_HostType_UCS2_SWAPPED                    =  21,      // < An zero-terminated UCS2 string (in swapped byte order ASCII 'A' is coded as 0x4100').

#if defined (LOADER_SWAPPED_UCS2)
    LOADER_HostType_UCS2_NATIVE                     =  LOADER_HostType_UCS2_SWAPPED, // < An zero-terminated UCS2 string (in native byte order).
#else
    LOADER_HostType_UCS2_NATIVE                     =  LOADER_HostType_UCS2, // < An zero-terminated UCS2 string (in native byte order).
#endif

    LOADER_HostType_DECIMAL                         = 29, /*<! BCD encoded decimal number. */

    LOADER_HostType_SAP_LONG_BINARY                 = 30, /*<! Special host type for SAP systems. */
    LOADER_HostType_SAP_LONG_VARCHAR_ASCII          = 31, /*<! Special host type for SAP systems. */
    LOADER_HostType_SAP_LONG_VARCHAR_UTF8           = 32, /*<! Special host type for SAP systems. */
    LOADER_HostType_SAP_LONG_VARCHAR_UCS2           = 33, /*<! Special host type for SAP systems. */
    LOADER_HostType_SAP_LONG_VARCHAR_UCS2_SWAPPED   = 34, /*<! Special host type for SAP systems. */
#if defined (LOADER_SWAPPED_UCS2)
    LOADER_HostType_SAP_LONG_VARCHAR_UCS2_NATIVE    =  LOADER_HostType_SAP_LONG_VARCHAR_UCS2_SWAPPED, // < An zero-terminated UCS2 string (in native byte order).
#else
    LOADER_HostType_SAP_LONG_VARCHAR_UCS2_NATIVE    =  LOADER_HostType_SAP_LONG_VARCHAR_UCS2, // < An zero-terminated UCS2 string (in native byte order).
#endif

    LOADER_HostType_USERDEFINED                     = 100, // < User-defined, must supply a parameter converter.

    LOADER_HostType_MAX                             = LOADER_HostType_USERDEFINED  // < Maximum value for checks.
}
LOADER_HostType;


typedef  enum LOADER_DateTimeFormat     // taken from SQLDBC_Types.h; corresponds to tgg00_DateTimeFormat
{
    LOADER_DateTimeFormat_None                  = 0,    /*!< Unknown, not set. */
    LOADER_DateTimeFormat_Internal              = 1,    /*!< Normal (aka INTERNAL). */
    LOADER_DateTimeFormat_Iso                   = 2,    /*!< ISO    (ODBC, JDBC) */
    LOADER_DateTimeFormat_Usa                   = 3,    /*!< USA */
    LOADER_DateTimeFormat_Eur                   = 4,    /*!< Europe */
    LOADER_DateTimeFormat_Jis                   = 5,    /*!< Japan */
    LOADER_DateTimeFormat_OracleDate            = 6,    /*!< @deprecated Unused. */
    LOADER_DateTimeFormat_WasAnsiNowIsSameAsIso = 7,    /*!< @deprecated Unused */
    LOADER_DateTimeFormat_TsEur                 = 8     /*!< @deprecated (same as Eur_C). */
}LOADER_DateTimeFormat;


typedef enum LOADER_Retcode
{
     LOADER_OK                      = 0,     //!< Function call successful.
     LOADER_NOT_OK                  = 1,      //!< Function call not successful. Further information can be found in the corresponding error object.
     LOADER_DATA_TRUNC              = 2,      //!< Data was truncated during the call.
     LOADER_OVERFLOW                = 3      //!< Signalizes a numeric overflow.
}
LOADER_Retcode;


typedef enum LOADER_TraceLevel
{
    LOADER_TRACE_NONE               = 0,
    LOADER_TRACE_LEVEL_1            = 32,
    LOADER_TRACE_LEVEL_2            = 64
}
LOADER_TraceLevel;

#endif //LOADER_TYPES_HPP