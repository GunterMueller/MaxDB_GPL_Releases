/*!
  @file           typesindicator.cpp
  @author         D042427
  @ingroup        Tests
  @brief          check indicators after conversion
  @keywords       indicator, conversion




	ConversionCheck
		- checks if conversions (SQL type => host type) are possible

    A matrix is defined below which contains all possible combinations (SQL types vs. host types)
      * Different SQL modes are not tested...
      * Certain host types are not covered:
           ~ SQLDBC_HOSTTYPE_MIN / SQLDBC_HOSTTYPE_PARAMETER_NOTSET
           ~ SQLDBC_HOSTTYPE_GUID
           ~ SQLDBC_HOSTTYPE_MAX / SQLDBC_HOSTTYPE_USERDEFINED
    For every combination a valid value for insertion is given. This value should be convertable (e.g. '1234' for CHAR => INT4)



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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"

#define NUM_HOSTTYPES 19
// #define NUM_SQLTYPES 17

struct MatrixEntry {
    bool ConversionAllowed;
//  char SQLType[50];
    SQLDBC_HostType HostType;
    char value[100]; // string which will be embedded into an SQL statement string
    SQLDBC_Length ExpectedIndicator;
    bool terminate;
};

// rows and columns are changed in the matrix above...
struct MatrixRow {
    char SQLType[50];
    bool IsUnicode;
    MatrixEntry Entry[NUM_HOSTTYPES]; // one entry for each host type
};


// conversion matrix (convert from SQL type to host type):
MatrixRow Matrix[] = {
  { "CHAR(30) ASCII", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'test1'", 30, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'test12'", 30, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'test123'", 30, true},
    { true, SQLDBC_HOSTTYPE_UINT1,         "'25'", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "'-25'", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "'300'", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "'-300'", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "'70000'", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "'-70000'", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "'90000000'", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "'-90000000'", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "'-2.34'", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "'-4.567'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'17:12:30.000000'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25 17:12:30.000000'", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'256'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'abcde'", 60, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'abcde'", 60, true}
      }},

    { "CHAR(30) UNICODE", true, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'test1'", 60, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'test12'", 30, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'test123äöü'", 33, true}, // 'äöü' are not in ASCII-7 ==> They are encoded as 2 bytes each
    { true, SQLDBC_HOSTTYPE_UINT1,         "'25'", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "'-25'", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "'300'", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "'-300'", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "'70000'", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "'-70000'", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "'90000000'", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "'-90000000'", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "'-2.34'", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "'-4.567'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'17:12:30.000000'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25 17:12:30.000000'", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'478264'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'abcde'", 60, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'abcde'", 60, true}
        }},
  { "CHAR(30) BYTE", false, {  // not fully consistent, only UCS2 isn't supported.
    { true,  SQLDBC_HOSTTYPE_BINARY,        "x'616263'", 30, true}, // 'abc'
    { true, SQLDBC_HOSTTYPE_ASCII,         "x'6465666768'", 30, true}, // 'defgh'
    { true,  SQLDBC_HOSTTYPE_UTF8,          "x'696a6b6c'", 30, false}, // 'ijkl'
    { false,  SQLDBC_HOSTTYPE_UINT1,         "x'323535'", 1, true}, // '255'    (biggest value for UINT1)
    { false, SQLDBC_HOSTTYPE_INT1,          "x'2d313238'", 1, true}, // '-128'    (smallest value for INT1)
    { false, SQLDBC_HOSTTYPE_UINT2,         "x'323536'", 2, true}, // '256'    (does not fit into UINT2)
    { false, SQLDBC_HOSTTYPE_INT2,          "x'2d313239'", 2, true}, // '-129'    (does not fit into INT2)
    { false, SQLDBC_HOSTTYPE_UINT4,         "x'34323934393637323935'", 4, true}, // '4294967295'    (biggest value for UINT4)
    { false, SQLDBC_HOSTTYPE_INT4,          "x'2d32313437343833363438'", 4, true}, // '-2147483648'    (smallest value for INT4)
    { false, SQLDBC_HOSTTYPE_UINT8,         "x'34323934393637323936'", 8, true}, // '4294967296'    (does not fit into UINT4)
    { false, SQLDBC_HOSTTYPE_INT8,          "x'2d32313437343833363439'", 8, true}, // '-2147483649'    (does not fit into INT4)
    { false, SQLDBC_HOSTTYPE_DOUBLE,        "x'2d322e3334'", 8, true}, // '-2.34'
    { false, SQLDBC_HOSTTYPE_FLOAT,         "x'2d342e353637'", 4, true}, // '-4.567'
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "x'313938382d31322d3235'", 6, true}, // '1988-12-25'
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "x'31373a31323a33302e303030303030'", 6, true}, // '17:12:30.000000'
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "x'313938382d31322d32352031373a31323a33302e303030303030'", 16, true}, // '1988-12-25 17:12:30.000000'
    { false, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "x'343738323634'", 19, true}, // '478264'
    { false, SQLDBC_HOSTTYPE_UCS2,          "x'6162636465'", 60, true}, // 'abcde'
    { false, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "x'6162636465'", 60, true} // 'abcde'
      }},
  { "VARCHAR(30) ASCII", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'test1'",  5, true}, 
    { true, SQLDBC_HOSTTYPE_ASCII,         "'test12'", 6, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'test123äöü'", 13, true}, // 'äöü' are not in ASCII-7 ==> They are encoded as 2 bytes each
    { true, SQLDBC_HOSTTYPE_UINT1,         "'255'", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "'-128'", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "'256'", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "'-129'", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "'4294967295'", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "'-2147483648'", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "'4294967296'", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "'-2147483649'", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "'-2.34'", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "'-4.567'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'17:12:30.000000'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25 17:12:30.000000'", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'478264'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'abcde'", 10, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'abcdeäöü'", 16, true}
      }},

  { "VARCHAR(30) UNICODE", true, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'test1'",  60, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'test12'", 6, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'test123äöü'", 13, true}, // 'äöü' are not in ASCII-7 ==> They are encoded as 2 bytes each
    { true, SQLDBC_HOSTTYPE_UINT1,         "'255'", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "'-128'", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "'256'", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "'-129'", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "'4294967295'", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "'-2147483648'", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "'4294967296'", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "'-2147483649'", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "'-2.34'", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "'-4.567'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'17:12:30.000000'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25 17:12:30.000000'", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'478264'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'abcde'", 10, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'abcdeäöü'", 16, true}
      }},

  { "VARCHAR(30) BYTE", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "x'616263'", 3, true}, // 'abc'
    { true, SQLDBC_HOSTTYPE_ASCII,         "x'6465666768'", 5, true}, // 'defgh'
    { true, SQLDBC_HOSTTYPE_UTF8,          "x'696a6b6c'", 4, false}, // 'ijkl'
    { false, SQLDBC_HOSTTYPE_UINT1,         "x'323535'", 1, true}, // '255'    (biggest value for UINT1)
    { false, SQLDBC_HOSTTYPE_INT1,          "x'2d313238'", 1, true}, // '-128'    (smallest value for INT1)
    { false, SQLDBC_HOSTTYPE_UINT2,         "x'323536'", 2, true}, // '256'    (does not fit into UINT2)
    { false, SQLDBC_HOSTTYPE_INT2,          "x'2d313239'", 2, true}, // '-129'    (does not fit into INT2)
    { false, SQLDBC_HOSTTYPE_UINT4,         "x'34323934393637323935'", 4, true}, // '4294967295'    (biggest value for UINT4)
    { false, SQLDBC_HOSTTYPE_INT4,          "x'2d32313437343833363438'", 4, true}, // '-2147483648'    (smallest value for INT4)
    { false, SQLDBC_HOSTTYPE_UINT8,         "x'34323934393637323936'", 8, true}, // '4294967296'    (does not fit into UINT4)
    { false, SQLDBC_HOSTTYPE_INT8,          "x'2d32313437343833363439'", 8, true}, // '-2147483649'    (does not fit into INT4)
    { false, SQLDBC_HOSTTYPE_DOUBLE,        "x'2d322e3334'", 8, true}, // '-2.34'
    { false, SQLDBC_HOSTTYPE_FLOAT,         "x'2d342e353637'", 4, true}, // '-4.567'
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "x'313938382d31322d3235'", 6, true}, // '1988-12-25'
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "x'31373a31323a33302e303030303030'", 6, true}, // '17:12:30.000000'
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "x'313938382d31322d32352031373a31323a33302e303030303030'", 16, true}, // '1988-12-25 17:12:30.000000'
    { false, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "x'343738323634'", 19, true}, // '478264'
    { false, SQLDBC_HOSTTYPE_UCS2,          "x'6162636465'", 60, true}, // 'abcde'
    { false, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "x'6162636465'", 60, true} // 'abcde'
  }},

  { "LONG ASCII", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'test1'", 5, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'test12'", 6, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'test123äöü'", 13, true}, // 'äöü' are not in ASCII-7 ==> They are encoded as 2 bytes each
    { true, SQLDBC_HOSTTYPE_UINT1,         "'255'", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "'-128'", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "'256'", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "'-129'", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "'4294967295'", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "'-2147483648'", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "'4294967296'", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "'-2147483649'", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "'-2.34'", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "'-4.567'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'17:12:30'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25 17:12:30.000000'", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'478264'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'abcde'", 10, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'abcdeäöü'", 16, true}
  }},

  { "LONG UNICODE", true, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'test1'", 10, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'test12'", 6, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'test123äöü'", 13, true}, // 'äöü' are not in ASCII-7 ==> They are encoded as 2 bytes each
    { true, SQLDBC_HOSTTYPE_UINT1,         "'255'", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "'-128'", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "'256'", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "'-129'", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "'4294967295'", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "'-2147483648'", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "'4294967296'", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "'-2147483649'", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "'-2.34'", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "'-4.567'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'17:12:30'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25 17:12:30.000000'", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'478264'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'abcde'", 10, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'abcdeäöü'", 16, true}
  }},

  { "LONG BYTE", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "x'616263'", 3, true}, // 'abc'
    { true, SQLDBC_HOSTTYPE_ASCII,         "x'6465666768'", 5, true}, // 'defgh'
    { true, SQLDBC_HOSTTYPE_UTF8,          "x'696a6b6c'", 4, false}, // 'ijkl'
    { false,SQLDBC_HOSTTYPE_UINT1,         "x'323535'", 1, true}, // '255'    (biggest value for UINT1)
    { false, SQLDBC_HOSTTYPE_INT1,          "x'2d313238'", 1, true}, // '-128'    (smallest value for INT1)
    { false, SQLDBC_HOSTTYPE_UINT2,         "x'323536'", 2, true}, // '256'    (does not fit into UINT2)
    { false, SQLDBC_HOSTTYPE_INT2,          "x'2d313239'", 2, true}, // '-129'    (does not fit into INT2)
    { false, SQLDBC_HOSTTYPE_UINT4,         "x'34323934393637323935'", 4, true}, // '4294967295'    (biggest value for UINT4)
    { false, SQLDBC_HOSTTYPE_INT4,          "x'2d32313437343833363438'", 4, true}, // '-2147483648'    (smallest value for INT4)
    { false, SQLDBC_HOSTTYPE_UINT8,         "x'34323934393637323936'", 8, true}, // '4294967296'    (does not fit into UINT4)
    { false, SQLDBC_HOSTTYPE_INT8,          "x'2d32313437343833363439'", 8, true}, // '-2147483649'    (does not fit into INT4)
    { false, SQLDBC_HOSTTYPE_DOUBLE,        "x'2d322e3334'", 8, true}, // '-2.34'
    { false, SQLDBC_HOSTTYPE_FLOAT,         "x'2d342e353637'", 4, true}, // '-4.567'
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "x'313938382d31322d3235'", 6, true}, // '1988-12-25'
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "x'31373a31323a3330'", 6, true}, // '17:12:30.000000'
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "x'313938382d31322d32352031373a31323a33302e303030303030'", 16, true}, // '1988-12-25 17:12:30.000000'
    { false, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "x'343738323634'", 19, true}, // '478264'
    { true, SQLDBC_HOSTTYPE_UCS2,          "x'6162636465'", 10, true}, // 'abcde'
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "x'6162636465'", 10} // 'abcde'
  }},

  { "BOOLEAN", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "TRUE", 1, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "FALSE", 5, true}, // ???
    { true, SQLDBC_HOSTTYPE_UTF8,          "TRUE", 4, true}, // ???
    { true, SQLDBC_HOSTTYPE_UINT1,         "FALSE", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "TRUE", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "FALSE", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "TRUE", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "FALSE", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "TRUE", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "FALSE", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "TRUE", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "TRUE", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "TRUE", 4, true},
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "FALSE", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "TRUE", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "FALSE", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "TRUE", 19, true}, // ???
    { true, SQLDBC_HOSTTYPE_UCS2,          "FALSE", 10, true}, // ???
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "TRUE", 8, true} // ???
  }},

  { "FIXED(14, 3)", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "12345", 8, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "123", 7, true}, // returns '123.000'
    { true, SQLDBC_HOSTTYPE_UTF8,          "1234", 8, true}, // returns '1234.000'
    { true, SQLDBC_HOSTTYPE_UINT1,         "255", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "-128", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "'256'", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "'-129'", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "'4294967295'", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "'-2147483648'", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "'4294967296'", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "'-2147483649'", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "3872.762", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "-182.4", 4, true},
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "16723", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "9823772", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "6526563.23", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "728763", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "123456", 20, true}, // returns '123456.000'
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "12", 12, true} // returns '12.000'
  }},

  { "FLOAT", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "34.8723", 9, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "8374.23", 7, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          ".654", 5, true}, // '0.654' should be returned
    { true, SQLDBC_HOSTTYPE_UINT1,         "255", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "-128", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "256", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "-129", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "4294967295", 4, true},
    { true, SQLDBC_HOSTTYPE_INT4,          "-2147483648", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "4294967296", 8, true},
    { true, SQLDBC_HOSTTYPE_INT8,          "-2147483649", 8, true},
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "3872.762", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "-182.4", 4, true},
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "7634.23", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "827.33", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, ".87234", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "12345", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "6518", 8, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "4.444", 10, true}
  }},

  { "INTEGER", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "123", 6, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "1234", 4, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "-12345", 6, true},
    { true, SQLDBC_HOSTTYPE_UINT1,         "255", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "-128", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "256", 2, true},
    { true, SQLDBC_HOSTTYPE_INT2,          "-129", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "2147483647", 4, true}, // INTEGER is INT4 => bigger / smaller values cannot be stored
    { true, SQLDBC_HOSTTYPE_INT4,          "-2147483648", 4, true},
    { true, SQLDBC_HOSTTYPE_UINT8,         "2147483647", 8, true}, // - " -
    { true, SQLDBC_HOSTTYPE_INT8,          "-2147483648", 8, true}, // - " -
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "124356", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "-12345", 4, true},
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "1243567", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "124356", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "12345", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "1243567", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "-123", 8, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "12345678", 16, true}
  }},

  { "SMALLINT", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "123", 4, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "1234", 4, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "-12345", 6, true},
    { true, SQLDBC_HOSTTYPE_UINT1,         "255", 1, true},
    { true, SQLDBC_HOSTTYPE_INT1,          "-128", 1, true},
    { true, SQLDBC_HOSTTYPE_UINT2,         "256", 2, true}, // SMALLINT is INT2 => bigger / smaller values cannot be stored
    { true, SQLDBC_HOSTTYPE_INT2,          "-129", 2, true},
    { true, SQLDBC_HOSTTYPE_UINT4,         "32767", 4, true}, // - " -
    { true, SQLDBC_HOSTTYPE_INT4,          "-32768", 4, true}, // - " -
    { true, SQLDBC_HOSTTYPE_UINT8,         "32767", 8, true}, // - " -
    { true, SQLDBC_HOSTTYPE_INT8,          "-32768", 8, true}, // - " -
    { true, SQLDBC_HOSTTYPE_DOUBLE,        "12435", 8, true},
    { true, SQLDBC_HOSTTYPE_FLOAT,         "-12345", 4, true},
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "1243", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "12435", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "123", 16, true},
    { true, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "12435", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "-123", 8, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "12345", 10, true}
  }},

  { "DATE", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'1988-12-25'", 10, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'1988-12-25'", 10, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'1988-12-25'", 10, true},
    { false, SQLDBC_HOSTTYPE_UINT1,         "'1988-12-25'", 1, true},
    { false, SQLDBC_HOSTTYPE_INT1,          "'1988-12-25'", 1, true},
    { false, SQLDBC_HOSTTYPE_UINT2,         "'1988-12-25'", 2, true},
    { false, SQLDBC_HOSTTYPE_INT2,          "'1988-12-25'", 2, true},
    { false, SQLDBC_HOSTTYPE_UINT4,         "'1988-12-25'", 4, true},
    { false, SQLDBC_HOSTTYPE_INT4,          "'1988-12-25'", 4, true},
    { false, SQLDBC_HOSTTYPE_UINT8,         "'1988-12-25'", 8, true},
    { false, SQLDBC_HOSTTYPE_INT8,          "'1988-12-25'", 8, true},
    { false, SQLDBC_HOSTTYPE_DOUBLE,        "'1988-12-25'", 8, true},
    { false, SQLDBC_HOSTTYPE_FLOAT,         "'1988-12-25'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25'", 6, true},
    { false, SQLDBC_HOSTTYPE_ODBCTIME,      "'1988-12-25'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25'", 16, true},
    { false, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'1988-12-25'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'1988-12-25'", 20, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'1988-12-25'", 20, true}
  }},

  { "TIME", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'17:12:30.123000'", 8, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'17:12:30.123000'", 8, true},  // should return '17:12:30'
    { true, SQLDBC_HOSTTYPE_UTF8,          "'17:12:30.123000'", 8, true},  // should return '17:12:30'
    { false, SQLDBC_HOSTTYPE_UINT1,         "'17:12:30.123000'", 1, true},
    { false, SQLDBC_HOSTTYPE_INT1,          "'17:12:30.123000'", 1, true},
    { false, SQLDBC_HOSTTYPE_UINT2,         "'17:12:30.123000'", 2, true},
    { false, SQLDBC_HOSTTYPE_INT2,          "'17:12:30.123000'", 2, true},
    { false, SQLDBC_HOSTTYPE_UINT4,         "'17:12:30.123000'", 4, true},
    { false, SQLDBC_HOSTTYPE_INT4,          "'17:12:30.123000'", 4, true},
    { false, SQLDBC_HOSTTYPE_UINT8,         "'17:12:30.123000'", 8, true},
    { false, SQLDBC_HOSTTYPE_INT8,          "'17:12:30.123000'", 8, true},
    { false, SQLDBC_HOSTTYPE_DOUBLE,        "'17:12:30.123000'", 8, true},
    { false, SQLDBC_HOSTTYPE_FLOAT,         "'17:12:30.123000'", 4, true},
    { false, SQLDBC_HOSTTYPE_ODBCDATE,      "'17:12:30.123000'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'17:12:30.123000'", 6, true},
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'17:12:30.123000'", 16, true},
    { false, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'17:12:30.123000'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'17:12:30.123000'", 16, true},  // should return '17:12:30'
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'17:12:30.123000'", 16, true}  // should return '17:12:30'
  }},

  { "TIMESTAMP", false, {
    { true, SQLDBC_HOSTTYPE_BINARY,        "'1988-12-25 17:12:30.000000'", 26, true},
    { true, SQLDBC_HOSTTYPE_ASCII,         "'1988-12-25 17:12:30.000000'", 26, true},
    { true, SQLDBC_HOSTTYPE_UTF8,          "'1988-12-25 17:12:30.000000'", 26, true},
    { false, SQLDBC_HOSTTYPE_UINT1,         "'1988-12-25 17:12:30.000000'", 1, true},
    { false, SQLDBC_HOSTTYPE_INT1,          "'1988-12-25 17:12:30.000000'", 1, true},
    { false, SQLDBC_HOSTTYPE_UINT2,         "'1988-12-25 17:12:30.000000'", 2, true},
    { false, SQLDBC_HOSTTYPE_INT2,          "'1988-12-25 17:12:30.000000'", 2, true},
    { false, SQLDBC_HOSTTYPE_UINT4,         "'1988-12-25 17:12:30.000000'", 4, true},
    { false, SQLDBC_HOSTTYPE_INT4,          "'1988-12-25 17:12:30.000000'", 4, true},
    { false, SQLDBC_HOSTTYPE_UINT8,         "'1988-12-25 17:12:30.000000'", 8, true},
    { false, SQLDBC_HOSTTYPE_INT8,          "'1988-12-25 17:12:30.000000'", 8, true},
    { false, SQLDBC_HOSTTYPE_DOUBLE,        "'1988-12-25 17:12:30.000000'", 8, true},
    { false, SQLDBC_HOSTTYPE_FLOAT,         "'1988-12-25 17:12:30.000000'", 4, true},
    { true, SQLDBC_HOSTTYPE_ODBCDATE,      "'1988-12-25 17:12:30.000000'", 6, true}, // ???
    { true, SQLDBC_HOSTTYPE_ODBCTIME,      "'1988-12-25 17:12:30.000000'", 6, true}, // ???
    { true, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, "'1988-12-25 17:12:30.000000'", 16, true},
    { false, SQLDBC_HOSTTYPE_ODBCNUMERIC,   "'1988-12-25 17:12:30.000000'", 19, true},
    { true, SQLDBC_HOSTTYPE_UCS2,          "'1988-12-25 17:12:30.000000'", 52, true},
    { true, SQLDBC_HOSTTYPE_UCS2_SWAPPED,  "'1988-12-25 17:12:30.000000'", 52, true}
  }}
};


/* -----   ConversionCheck   -----
 *
 * checks if conversions (SQL type => host type) are possible
 * for more details see above
 *
 * precondition for this test case: UnicodeDatabase (for some test cases)
 *
 * for each SQL type (row in matrix):
 *   1. create table with two columns (id INTEGER, val SQLType)
 *   for each host type (entry in row):
 *     2. INSERT one row (take value for second column from MatrixEntry)
 *     3. execute SELECT, navigate to first row in result set
 *     4. getObject: check if conversion succeeds / fails
 *     5. was the indicator properly set?
 *
 */
class ConversionCheck : public SQLDBC_TestCase
{

public:
	ConversionCheck ()
		: SQLDBC_TestCase ("ConversionCheck")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
		loginfo("*     check conversion / indicator                  *");
		loginfo("*****************************************************");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{

        // for each SQL type
      for (int SQLTypeIndex = 0; SQLTypeIndex < sizeof(Matrix) / sizeof(MatrixRow) ; SQLTypeIndex++) {
            
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
            
            sprintf(strSQL, "tableConversionCheck%d", SQLTypeIndex);
            dropTable(strSQL);
            
            // 1.
//		  loginfo("1. create table with two columns (id INTEGER, val %s)", row->SQLType);
            sprintf(strSQL, "CREATE TABLE tableConversionCheck%d (id INTEGER, val %s)", SQLTypeIndex, row->SQLType);
            execSQL(strSQL);
            
            
            // for each host type
            for (int HostTypeIndex = 0; HostTypeIndex < NUM_HOSTTYPES; HostTypeIndex++) {
                
                MatrixEntry *entry = &row->Entry[HostTypeIndex];
                
                char HostTypeName[100];
                HostTypeToStr(HostTypeName, sizeof(HostTypeName), entry->HostType);
                loginfo("  check val = %s, fetch as %s: should %s", entry->value, HostTypeName,
                        (entry->ConversionAllowed) ? "SUCCEED" : "FAIL");
                
                
                // 2.
//		    loginfo("  2. INSERT one row (val = %s)", entry->value);
                sprintf(strSQL, "INSERT INTO tableConversionCheck%d (id, val) VALUES (%d, %s)", SQLTypeIndex, HostTypeIndex, entry->value);
                execSQL(strSQL);
                
                // 3.
//		    loginfo("  3. execute SELECT, navigate to first row in result set");
                SQLDBC_Statement *stmt = connection()->createStatement();
                sprintf(strSQL, "SELECT val FROM tableConversionCheck%d WHERE id = %d", SQLTypeIndex, HostTypeIndex);
                tc_errorcheck(stmt)->execute(strSQL);
                
                SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
                if (!rs) {
                    logerror("No result set was returned. Test case must exit");
                    return m_success;
                }
                tc_errorcheck(rs)->next();
                
                // 4.
//		    loginfo("  4. getObject: check if conversion succeeds / fails");
                union {
                    char buffer[1024];
                    SAPDB_Int8 large_int;
                } data;
                
                SQLDBC_Length ind=SQLDBC_NTS;
                
                SQLDBC_Retcode rc;
                if (entry->ConversionAllowed) {
                    tc_errorcheckrc(rc, rs)->getObject(1, entry->HostType, data.buffer, &ind, sizeof(data.buffer), entry->terminate);
                    if (rc != SQLDBC_OK && rc != SQLDBC_NOT_OK) // we already checked for SQLDBC_NOT_OK...
                        logerror("getObject() returned %d. Expected was SQLDBC_OK", rc);
                    
                    // 5.
//        loginfo("  5. was the indicator properly set?");
                    if (ind != entry->ExpectedIndicator) {
                        logerror("Wrong indicator was returned: %d. Epected was %d for database type %s", ind, entry->ExpectedIndicator, row->SQLType);
                    }
                    
                } else {
                    
                    rc = rs->getObject(1, entry->HostType, data.buffer, &ind, sizeof(data.buffer));
                    if (rc == SQLDBC_OK)
                        logerror("Conversion succeeded although it was expected to fail for database type %s.", row->SQLType);
                }
                
                connection()->releaseStatement(stmt);
            } // next host type
            
            sprintf(strSQL, "tableConversionCheck%d", SQLTypeIndex);
            dropTable(strSQL);
            
        } // next SQL type
        
        return m_success;
	}
};

int main (int argc, char **argv)
{
	// Initalize from command line params
	SQLDBC_TestFrame testframe (argc, argv);

	testframe.addTestCase (new ConversionCheck());

	if (testframe.run ()) 
		return 0;

	return 1;
}


