/*!
  @file           paramtypes.cpp
  @author         D042427
  @ingroup        Tests
  @brief          checks bindParameter with the different host types
  @keywords       bindParameter, conversion


	checkBinding
		- check bindParam for each host type / SQL type combination



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

#undef ODBC_TYPES_DEFINED

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/Interfaces/ODBC/sqldbc_sqltypes.h"
#include "hsp81.h"
#include "hsp77.h"

#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#endif

struct BinaryEntry {
  bool ConversionAllowed;
  SQLDBC_Length Size;
  unsigned char value[30];
};

struct AsciiEntry {
  bool ConversionAllowed;
  char value[60];
};

struct UInt1Entry {
  bool ConversionAllowed;
  SQLDBC_UInt1 value;
};

struct Int1Entry {
  bool ConversionAllowed;
  SQLDBC_Int1 value;
};

struct UInt2Entry {
  bool ConversionAllowed;
  SQLDBC_UInt2 value;
};

struct Int2Entry {
  bool ConversionAllowed;
  SQLDBC_Int2 value;
};

struct UInt4Entry {
  bool ConversionAllowed;
  SQLDBC_UInt4 value;
};

struct Int4Entry {
  bool ConversionAllowed;
  SQLDBC_Int4 value;
};

struct UInt8Entry {
  bool ConversionAllowed;
  SQLDBC_UInt8 value;
};

struct Int8Entry {
  bool ConversionAllowed;
  SQLDBC_Int8 value;
};

struct DoubleEntry {
  bool ConversionAllowed;
  double value;
};

struct FloatEntry {
  bool ConversionAllowed;
  float value;
};

struct DateEntry {
  bool ConversionAllowed;
  SQL_DATE_STRUCT value;
};

struct TimeEntry {
  bool ConversionAllowed;
  SQL_TIME_STRUCT value;
};

struct TimestampEntry {
  bool ConversionAllowed;
  SQL_TIMESTAMP_STRUCT value;
};

struct NumericEntry {
  bool ConversionAllowed;
  SQL_NUMERIC_STRUCT value;
};

struct DecimalEntry {
  bool ConversionAllowed;
  unsigned char value[8];
  SQLDBC_Length size;
};

struct LOBEntry {
  bool ConversionAllowed;
  SQLDBC_LOB *value;
};
struct MatrixRow {
  char SQLType[20];
  bool IsUnicode;
  BinaryEntry binary;
  AsciiEntry ascii;
  AsciiEntry utf8;
  UInt1Entry uint1;
  Int1Entry int1;
  UInt2Entry uint2;
  Int2Entry int2;
  UInt4Entry uint4;
  Int4Entry int4;
  UInt8Entry uint8;
  Int8Entry int8;
  DoubleEntry dbl;
  FloatEntry flt;
  DateEntry date;
  TimeEntry time;
  TimestampEntry timestamp;
  NumericEntry numeric;
  AsciiEntry ucs2;
  AsciiEntry ucs2swapped;
  LOBEntry blob;
  LOBEntry asciiclob;
  LOBEntry utf8clob;
  LOBEntry ucs2clob;
  DecimalEntry decimal;
};

/**
 * This matrix is very to the one in typesindicator.cpp
 *   because most conversions are bidirectional
 * 
 *
 * The UTF8, UCS2, UCS2_swapped entries in the matrix are ASCII. They have to converted before running the test case
 * This conversion takes place in prepare() of checkBinding
 */
SQLDBC_LOB ll1;
static MatrixRow Matrix[] = {
  // 12345678901234567890
  { "CHAR(30) ASCII     ", false 
    ,{ true, 5, "test1"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ true, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ true, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ true, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },
  // 12345678901234567890
  { "CHAR(30) UNICODE   ", true
    ,{ true, 10, { 0x00, 0x74, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x31} } // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ true, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ true, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ true, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },
  // 12345678901234567890
  { "CHAR(30) BYTE      ", false
    ,{ true, 5, "test1"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ false, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ false, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ false, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ false, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ false, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ false, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ false, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ false, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ false, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ false, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  // 12345678901234567890
  { "VARCHAR(30) ASCII  ", false 
    ,{ true, 5, "test1"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ true, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ true, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ true, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "VARCHAR(30) UNICODE", true
    ,{ true, 10, { 0x00, 0x74, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x31} } // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ true, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ true, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ true, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "VARCHAR(30) BYTE", false
    ,{ true, 5, "test1"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ false, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ false, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ false, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ false, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ false, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ false, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ false, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ false, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ false, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ false, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "LONG ASCII", false
    ,{ true, 5, "test1"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ true, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ true, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ true, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{true, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "LONG UNICODE", true
    ,{ true, 10, { 0x00, 0x74, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x31} } // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ true, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ true, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ true, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{true, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "LONG BYTE", false 
    ,{ true, 5, "test1"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "test12"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "test123הצü"} // SQLDBC_HOSTTYPE_UTF8
    ,{ false, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ false, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ false, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ false, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ false, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ false, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ false, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ false, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ false, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ false, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "abcde"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{true, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{true, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "BOOLEAN", false
    ,{ true, 1, "1"} // SQLDBC_HOSTTYPE_BINARY
    ,{ false, "FALSE"} // SQLDBC_HOSTTYPE_ASCII
    ,{ false, "TRUE"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 1} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, 1} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 1} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, 1} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 1} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, 1} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 1} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, 1} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 1.0} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, 1.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {1, 0, 1, 1}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ false, "TRUE"} // SQLDBC_HOSTTYPE_UCS2
    ,{ false, "TRUE"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "FIXED(14, 3)", false
    ,{ true, 8, { 0xca, 0x42, 0x94, 0x96, 0x72, 0x95, 0x00, 0x00 } } // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "123"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "1234"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.802} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "13579.13"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "13579.13"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{true, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "FLOAT", false 
    ,{ true, 9, {0xc4, 0x83, 0x74, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00}} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "8374.23"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, ".654"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "13579.13"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "13579.13"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{true, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "DOUBLE PRECISION", false
    ,{ true, 11, {0xc5, 0x13, 0x57, 0x91, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "8374.23"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, ".654"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 4294967295} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 4294967296ULL} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -8736465622LL} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.8023} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "13579.13"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "13579.13"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{true, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },
  { "INTEGER", false 
    ,{ true, 6, {0xc4, 0x12, 0x34, 0x00, 0x00, 0x00}} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "1234"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "-12345"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 2147483647} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -23876437} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 94967296} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -36465622} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.0} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {4, 0, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "13579"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "13579"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{true, { 0x00, 0x00, 0x00, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,0)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "SMALLINT", false 
    ,{ true, 4, {0xc3, 0x25, 0x50, 0x00}} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "1234"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "-12345"} // SQLDBC_HOSTTYPE_UTF8
    ,{ true, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ true, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ true, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ true, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ true, 32767} // SQLDBC_HOSTTYPE_UINT4
    ,{ true, -32768} // SQLDBC_HOSTTYPE_INT4
    ,{ true, 32767} // SQLDBC_HOSTTYPE_UINT8
    ,{ true, -32768} // SQLDBC_HOSTTYPE_INT8
    ,{ true, 17862.0} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ true, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ true, {2, 0, 0, {52, 24}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "13579"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "13579"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{true, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x99, 0x99 }, SQLDBC_LEN_DECIMAL(14,0)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "DATE", false 
    ,{ true, 10, "1988-12-25"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "1988-12-25"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "1988-12-25"} // SQLDBC_HOSTTYPE_UTF8
    ,{ false, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ false, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ false, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ false, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ false, 32767} // SQLDBC_HOSTTYPE_UINT4
    ,{ false, -32768} // SQLDBC_HOSTTYPE_INT4
    ,{ false, 32767} // SQLDBC_HOSTTYPE_UINT8
    ,{ false, -32768} // SQLDBC_HOSTTYPE_INT8
    ,{ false, 17862.0} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ false, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ true, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "1988-12-25"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "1988-12-25"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "TIME", false 
    ,{ true, 8, "17:12:30"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "17:12:30"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "17:12:30"} // SQLDBC_HOSTTYPE_UTF8
    ,{ false, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ false, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ false, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ false, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ false, 32767} // SQLDBC_HOSTTYPE_UINT4
    ,{ false, -32768} // SQLDBC_HOSTTYPE_INT4
    ,{ false, 32767} // SQLDBC_HOSTTYPE_UINT8
    ,{ false, -32768} // SQLDBC_HOSTTYPE_INT8
    ,{ false, 17862.0} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ false, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ true, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ false, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "17:12:30"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "17:12:30"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  },

  { "TIMESTAMP", false 
    ,{ true, 26, "1988-12-25 17:12:30.000000"} // SQLDBC_HOSTTYPE_BINARY
    ,{ true, "1988-12-25 17:12:30.000000"} // SQLDBC_HOSTTYPE_ASCII
    ,{ true, "1988-12-25 17:12:30.000000"} // SQLDBC_HOSTTYPE_UTF8
    ,{ false, 255} // SQLDBC_HOSTTYPE_UINT1
    ,{ false, -128} // SQLDBC_HOSTTYPE_INT1
    ,{ false, 256} // SQLDBC_HOSTTYPE_UINT2
    ,{ false, -129} // SQLDBC_HOSTTYPE_INT2
    ,{ false, 32767} // SQLDBC_HOSTTYPE_UINT4
    ,{ false, -32768} // SQLDBC_HOSTTYPE_INT4
    ,{ false, 32767} // SQLDBC_HOSTTYPE_UINT8
    ,{ false, -32768} // SQLDBC_HOSTTYPE_INT8
    ,{ false, 17862.0} // SQLDBC_HOSTTYPE_DOUBLE
    ,{ false, -263.0} // SQLDBC_HOSTTYPE_FLOAT
    ,{ false, {2004, 7, 20}} // SQLDBC_HOSTTYPE_ODBCDATE
    ,{ false, {10, 55, 35}} // SQLDBC_HOSTTYPE_ODBCTIME
    ,{ true, {2004, 7, 20, 10, 55, 35, 6523}} // SQLDBC_HOSTTYPE_ODBCTIMESTAMP
    ,{ false, {4, 2, 1, {52, 24, 156, 51}}} // SQLDBC_HOSTTYPE_ODBCNUMERIC
    ,{ true, "1988-12-25 17:12:30.000000"} // SQLDBC_HOSTTYPE_UCS2
    ,{ true, "1988-12-25 17:12:30.000000"} // SQLDBC_HOSTTYPE_UCS2_SWAPPED
    ,{false, &ll1} // SQLDBC_HOSTTYPE_BLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_ASCII_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UTF8_CLOB
    ,{false, &ll1} // SQLDBC_HOSTTYPE_UCS2_CLOB
    ,{false, { 0x12, 0x34, 0x56, 0x78, 0x98, 0x76, 0x54, 0x3C }, SQLDBC_LEN_DECIMAL(14,3)} // SQLDBC_HOSTTYPE_DECIMAL
  }

};

#define NUM_SQLTYPES (sizeof(Matrix)/sizeof(Matrix[0]))

static const char hexchar[] ={ '0', '1', '2', '3', '4', '5', '6', '7',
                         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

//----------------------------------------------------------------------
static inline void hexifystr(const char *dest,
                             const char *src,
                             int srclen)
{
    char *p=(char*)dest;
    for(int i=0; i<srclen; ++i) {
        *p=hexchar[ (src[i] >> 4) & 0xF ];
        ++p;
        *p=hexchar[ (src[i]) & 0xF ];
        ++p;
    }
    return;
}

/*!
  Function:     HostTypeValToStr

  see also: 
  
  Description:  Converts char* to char*
    
  Arguments:
    szCHAR      [out] output buffer (CHAR)
    maxLenCHAR  [in]  maximum length of szCHAR
    HostType    [in]  host type
    Val         [in]  input buffer
    ValLength   [in]  Length of the input buffer

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: buffer too small or unknown host type
    </UL>
 */
SQLDBC_Retcode HostTypeValToStr(char *szCHAR, SQLDBC_Length maxLenCHAR, SQLDBC_HostType HostType,
  void *Val, SQLDBC_Length ValLength) {

  char temp[1024];
  temp[0] = '\0';
  switch (HostType) {
    case SQLDBC_HOSTTYPE_MIN: 
      {sprintf(temp, "[min]"); break;}
//    case SQLDBC_HOSTTYPE_PARAMETER_NOTSET: 
//      {sprintf(temp, "???"); break;}
    case SQLDBC_HOSTTYPE_BINARY: 
      { strcpy(temp, "0x");
        hexifystr(temp+2, (char*)Val, (ValLength<(sizeof(temp)-2)/2) ? ValLength : (sizeof(temp)-2)/2); 
        temp[2+ValLength*2]='\0';
        break;}
    case SQLDBC_HOSTTYPE_ASCII: 
      {memcpy(temp, (void *)Val, (ValLength<sizeof(temp)) ? ValLength : sizeof(temp)); temp[(ValLength<sizeof(temp)) ? ValLength : sizeof(temp)] = '\0'; break;}
    case SQLDBC_HOSTTYPE_UTF8: 
      {convertUTF8ToASCII(temp, sizeof(temp), (char *)Val, ValLength); break;}
    case SQLDBC_HOSTTYPE_UINT1: 
      {sprintf(temp, "%u", *(SQLDBC_UInt1 *)Val); break;}
    case SQLDBC_HOSTTYPE_INT1: 
      {sprintf(temp, "%d", *(SQLDBC_Int1 *)Val); break;}
    case SQLDBC_HOSTTYPE_UINT2: 
      {sprintf(temp, "%u", *(SQLDBC_UInt2 *)Val); break;}
    case SQLDBC_HOSTTYPE_INT2: 
      {sprintf(temp, "%d", *(SQLDBC_Int2 *)Val); break;}
    case SQLDBC_HOSTTYPE_UINT4: 
      {sprintf(temp, "%u", *(SQLDBC_UInt4 *)Val); break;}
    case SQLDBC_HOSTTYPE_INT4: 
      {sprintf(temp, "%d", *(SQLDBC_Int4 *)Val); break;}
    case SQLDBC_HOSTTYPE_UINT8: 
      {sprintf(temp, "%u", *(SQLDBC_UInt8 *)Val); break;}
    case SQLDBC_HOSTTYPE_INT8: 
      {sprintf(temp, "%d", *(SQLDBC_Int8 *)Val); break;}
    case SQLDBC_HOSTTYPE_DOUBLE: 
      {sprintf(temp, "%f", *(double *)Val); break;}
    case SQLDBC_HOSTTYPE_FLOAT: 
      {sprintf(temp, "%f", *(float *)Val); break;}
    case SQLDBC_HOSTTYPE_ODBCDATE:
      {
        SQL_DATE_STRUCT *date = (SQL_DATE_STRUCT *)Val;
        sprintf(temp, "%d-%d-%d", date->year, date->month, date->day); break;
      }
    case SQLDBC_HOSTTYPE_ODBCTIME:
      {
        SQL_TIME_STRUCT *time = (SQL_TIME_STRUCT *)Val;
        sprintf(temp, "%d:%d:%d", time->hour, time->minute, time->second); break;
      }
    case SQLDBC_HOSTTYPE_ODBCTIMESTAMP:
      {
        SQL_TIMESTAMP_STRUCT *timestamp = (SQL_TIMESTAMP_STRUCT *)Val;
        sprintf(temp, "%d-%d-%d %d:%d:%d.%d", timestamp->year, timestamp->month, timestamp->day, timestamp->hour, timestamp->minute, timestamp->second, timestamp->fraction); break;
      }
    case SQLDBC_HOSTTYPE_ODBCNUMERIC:
      {
        SQL_NUMERIC_STRUCT *numeric = (SQL_NUMERIC_STRUCT *)Val;
        if (numeric->precision > 4)
          sprintf(temp, "[odbcnumeric");
        {
          int val=0, factor=1, divisor=1;
          int i;
          for (i=0; i<numeric->precision; i++) {
            val += factor * numeric->val[i];
            factor *= 256;
          }
          for (i=0; i<numeric->scale; i++) divisor *= 10;
          sprintf(temp, "%s%f", (numeric->sign) ? "" : "-", (double)val / (double)divisor);
        }
        break;
      }
    case SQLDBC_HOSTTYPE_GUID: 
      {sprintf(temp, "[guid]"); break;}
    case SQLDBC_HOSTTYPE_UCS2: 
      {convertUCS2ToASCII(temp, sizeof(temp), false, (UCS2CHAR *)Val, ValLength); break;}
    case SQLDBC_HOSTTYPE_UCS2_SWAPPED: 
      {convertUCS2ToASCII(temp, sizeof(temp), true, (UCS2CHAR *)Val, ValLength); break;}
//    case SQLDBC_HOSTTYPE_UCS2_NATIVE: 
//      {convertUCS2ToASCII(temp, sizeof(temp), (UCS2CHAR *)Val, ValLength); break;}
    case SQLDBC_HOSTTYPE_USERDEFINED: 
      {sprintf(temp, "[userdefined]"); break;}
//    case SQLDBC_HOSTTYPE_MAX: 
//      {sprintf(temp, "[max]"); break;}
  }
  if (strlen(temp) > (size_t)(maxLenCHAR-1)) return SQLDBC_NOT_OK;

  strcpy(szCHAR, temp);
  return SQLDBC_OK;
}


/* -----   checkBinding   -----
 *
 * short description (see above)
 *
 * precondition for this test case: Platform must be ???, UnicodeDatabase (for some test cases)
 *
 * for each SQL type (row in matrix):
 *   1. create table with two columns (id INTEGER, val SQLType)
 *   for each host type (entry in row):
 *     2. prepare INSERT
 *     3. bindParameter with given host type and value
 *     4. execute ==> should SUCCEED / FAIL according to the given matrix
 *     5. in case of success: check if something was inserted into the table
 *
 */
class checkBinding : public SQLDBC_TestCase
{
private:
  int HostTypeIndex;


  void checkConversion(bool ConversionAllowed, SQLDBC_HostType HostType, void *value, SQLDBC_Length Size, int SQLTypeIndex) {

        HostTypeIndex++;

        char strSQL[200];
        static char HostTypeName[100];
        static char HostTypeVal[100];
        HostTypeToStr(HostTypeName, sizeof(HostTypeName), HostType);
        HostTypeValToStr(HostTypeVal, sizeof(HostTypeVal), HostType, value, Size);
        loginfo("  bind %s: val %s. Should %s", HostTypeName, HostTypeVal,
          (ConversionAllowed) ? "SUCCEED" : "FAIL");

		    SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();

        // 2.
		    loginfo("  2. prepare INSERT");
        sprintf(strSQL, "INSERT INTO tablecheckBinding%d (id, val) VALUES (%d, ?)", SQLTypeIndex, HostTypeIndex);
        tc_errorcheck(pstmt)->prepare(strSQL);

        // 3.
		    loginfo("  3. bindParameter with given host type and value");
        SQLDBC_Length ind = Size;
        tc_errorcheck(pstmt)->bindParameter(1, HostType, value, &ind, Size);

        // 4.
		    loginfo("  4. execute ==> should SUCCEED / FAIL according to the given matrix");
        SQLDBC_Retcode rc;
        rc = pstmt->execute();

        if (ConversionAllowed) {
          if (rc == SQLDBC_OK) {

            // 5.
              loginfo("  5. in case of success: check if something was inserted into the table");
            SQLDBC_Statement *stmt = tc_errorcheck(connection())->createStatement();
            sprintf(strSQL, "SELECT val FROM tablecheckBinding%d WHERE id = %d", SQLTypeIndex, HostTypeIndex);
            tc_errorcheck(stmt)->execute(strSQL);

            SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();

            if (rs) {
              SQLDBC_Length ind = Size;
              if (HostType == SQLDBC_HOSTTYPE_DECIMAL) {
                Size = SQLDBC_DECIMAL_LENGTH(SQLDBC_DECIMAL_DIGITS(Size), SQLDBC_DECIMAL_FRACTION(Size));
              }
              void* p = alloca(Size);
              tc_errorcheck(rs)->bindColumn(1, HostType, p, &ind, Size, false);
              if (rs->next() != SQLDBC_OK) 
                logerror("Nothing was inserted into the database");
              else
                tc_errorcheck(rs)->getRowSet()->fetch();
            } else {
              logerror("No result set was returned");
            }

            connection()->releaseStatement(stmt);

          } else {
            SQLDBC_ErrorHndl *error = &pstmt->error();
            if (error->getErrorCode() == 0)
              logerror("%d was returned but no error code was set", rc);
            else
              logerror("Binding failed although it was expected to succeed: %d. %s", error->getErrorCode(), error->getErrorText());
          }
        } else {
          if (rc == SQLDBC_OK)
            logerror("Binding succeeded although it was expected to fail");
        }

        connection()->releaseStatement(pstmt);

  }


public:
	checkBinding ()
		: SQLDBC_TestCase ("checkBinding")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
		loginfo("*  checks bindParameter with different host types   *");
		loginfo("*****************************************************");

    // create valid UTF8, UCS2, UCS2swapped strings within the matrix
    for (int i=0; i<NUM_SQLTYPES; i++) {
      char temp[100];
      int len;

      // convert UTF8
      convertToUTF8(temp, sizeof(temp), Matrix[i].utf8.value);
      sprintf(Matrix[i].utf8.value, temp);

      // convert UCS2
      len = (int)strlen(Matrix[i].ucs2.value);
      convertToUCS2((UCS2CHAR *)temp, sizeof(temp), false, Matrix[i].ucs2.value);
      memcpy(Matrix[i].ucs2.value, temp, 2*len+2);

      // convert UCS2 swapped
      len = (int)strlen(Matrix[i].ucs2swapped.value);
      convertToUCS2((UCS2CHAR *)temp, sizeof(temp), true, Matrix[i].ucs2swapped.value);
      memcpy(Matrix[i].ucs2swapped.value, temp, 2*len+2);
    }

		dropTable("tablecheckBinding");
		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{

      SQLDBC_Statement *stmt = connection()->createStatement();

    // for each SQL type
    for (int SQLTypeIndex = 0; SQLTypeIndex < NUM_SQLTYPES; SQLTypeIndex++) {

      char strSQL[200];
      MatrixRow *row = &Matrix[SQLTypeIndex];
      
      // check if we need a unicode database
      if (row->IsUnicode && !connection()->isUnicodeDatabase()) {
		    loginfo("*********************************************");
        loginfo("*     We need a UNICODE database to check '%s'", row->SQLType);
    		loginfo("*********************************************");
        continue;
      }

		  loginfo("*********************************************");
	  	loginfo("*     check %s", row->SQLType);
  		loginfo("*********************************************");

      sp77sprintf(strSQL, sizeof(strSQL), "tablecheckBinding%d", SQLTypeIndex);
      dropTable(strSQL);
      
      // 1.
      loginfo("%d. create table with two columns (id INTEGER, val %s)", (int)SQLTypeIndex, 
              row->SQLType);
      sp77sprintf(strSQL, sizeof(strSQL), "CREATE TABLE tablecheckBinding%d (id INTEGER, val %s)", 
                  (int)SQLTypeIndex, 
                  (const char *)row->SQLType);
		  execSQL(strSQL);
      
      // for each host type
      HostTypeIndex = 0;

      checkConversion(row->ascii.ConversionAllowed,  SQLDBC_HOSTTYPE_ASCII,  row->ascii.value,  strlen(row->ascii.value), SQLTypeIndex);
      checkConversion(row->utf8.ConversionAllowed,  SQLDBC_HOSTTYPE_UTF8,  row->utf8.value,  strlen(row->utf8.value), SQLTypeIndex);
      checkConversion(row->ucs2.ConversionAllowed, SQLDBC_HOSTTYPE_UCS2, row->ucs2.value, 2*sp81UCS2strlen((tsp81_UCS2Char *)row->ucs2.value), SQLTypeIndex);
      checkConversion(row->ucs2swapped.ConversionAllowed, SQLDBC_HOSTTYPE_UCS2_SWAPPED, row->ucs2swapped.value, 2*sp81UCS2strlen((tsp81_UCS2Char *)row->ucs2swapped.value), SQLTypeIndex);
      checkConversion(row->binary.ConversionAllowed, SQLDBC_HOSTTYPE_BINARY, row->binary.value, row->binary.Size, SQLTypeIndex);
      checkConversion(row->uint1.ConversionAllowed, SQLDBC_HOSTTYPE_UINT1, &row->uint1.value, sizeof(SQLDBC_UInt1), SQLTypeIndex);
      checkConversion(row->int1.ConversionAllowed, SQLDBC_HOSTTYPE_INT1, &row->int1.value, sizeof(SQLDBC_Int1), SQLTypeIndex);
      checkConversion(row->uint2.ConversionAllowed, SQLDBC_HOSTTYPE_UINT2, &row->uint2.value, sizeof(SQLDBC_UInt2), SQLTypeIndex);
      checkConversion(row->int2.ConversionAllowed, SQLDBC_HOSTTYPE_INT2, &row->int2.value, sizeof(SQLDBC_Int2), SQLTypeIndex);
      checkConversion(row->uint4.ConversionAllowed, SQLDBC_HOSTTYPE_UINT4, &row->uint4.value, sizeof(SQLDBC_UInt4), SQLTypeIndex);
      checkConversion(row->int4.ConversionAllowed, SQLDBC_HOSTTYPE_INT4, &row->int4.value, sizeof(SQLDBC_Int4), SQLTypeIndex);
      checkConversion(row->uint8.ConversionAllowed, SQLDBC_HOSTTYPE_UINT8, &row->uint8.value, sizeof(SQLDBC_UInt8), SQLTypeIndex);
      checkConversion(row->int8.ConversionAllowed, SQLDBC_HOSTTYPE_INT8, &row->int8.value, sizeof(SQLDBC_Int8), SQLTypeIndex);
      checkConversion(row->dbl.ConversionAllowed, SQLDBC_HOSTTYPE_DOUBLE, &row->dbl.value, sizeof(double), SQLTypeIndex);
      checkConversion(row->flt.ConversionAllowed, SQLDBC_HOSTTYPE_FLOAT, &row->flt.value, sizeof(float), SQLTypeIndex);
      checkConversion(row->decimal.ConversionAllowed, SQLDBC_HOSTTYPE_DECIMAL, &row->decimal.value, row->decimal.size, SQLTypeIndex);
      checkConversion(row->date.ConversionAllowed, SQLDBC_HOSTTYPE_ODBCDATE, &row->date.value, sizeof(SQL_DATE_STRUCT), SQLTypeIndex);
      checkConversion(row->time.ConversionAllowed, SQLDBC_HOSTTYPE_ODBCTIME, &row->time.value, sizeof(SQL_TIME_STRUCT), SQLTypeIndex);
      checkConversion(row->timestamp.ConversionAllowed, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &row->timestamp.value, sizeof(SQL_TIMESTAMP_STRUCT), SQLTypeIndex);
      checkConversion(row->numeric.ConversionAllowed, SQLDBC_HOSTTYPE_ODBCNUMERIC, &row->numeric.value, sizeof(SQL_NUMERIC_STRUCT), SQLTypeIndex);
      checkConversion(row->asciiclob.ConversionAllowed, SQLDBC_HOSTTYPE_ASCII_CLOB, row->asciiclob.value, sizeof(*row->asciiclob.value), SQLTypeIndex);
      checkConversion(row->utf8clob.ConversionAllowed, SQLDBC_HOSTTYPE_UTF8_CLOB, row->utf8clob.value, sizeof(*row->utf8clob.value), SQLTypeIndex);
      checkConversion(row->ucs2clob.ConversionAllowed, SQLDBC_HOSTTYPE_UCS2_CLOB, row->ucs2clob.value, sizeof(*row->ucs2clob.value), SQLTypeIndex);
      checkConversion(row->blob.ConversionAllowed, SQLDBC_HOSTTYPE_BLOB, row->blob.value, sizeof(*row->blob.value), SQLTypeIndex);
      //      checkConversion(ptrue, SQLDBC_HOSTTYPE_RAWHEX, &row->blob.value, 2, SQLTypeIndex);
    } // next SQL type

		return m_success;
	}
};

int main (int argc, char **argv)
{
#ifdef XXX
  char *p = (char*)malloc(sizeof(Matrix[0])*2+1);
  for(int a=0;a<NUM_SQLTYPES;a++) {
    hexifystr(p, (char*)&Matrix[a], sizeof(Matrix[0]));
    p[sizeof(Matrix[0])*2]=0;
    printf("%s\n", p);
  }   
#endif
	// Initalize from command line params
	SQLDBC_TestFrame testframe (argc, argv);
	testframe.addTestCase (new checkBinding());
        int ret=1;
	if (testframe.run ())
          ret = 0;
	return ret;
}
