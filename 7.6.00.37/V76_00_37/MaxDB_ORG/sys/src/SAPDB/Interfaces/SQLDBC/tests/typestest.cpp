/*!================================================================
 module:    typestest.cpp

 responsible: D039759

 special area: Type conversion
 description:  Check program that checks conversions.

 see:

 change history:


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




 ===================================================================*/

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(WIN32)
#  include <float.h>
#  define ISNAN(x) _isnan(x)
#  define ISINF(x) (!_finite(x))
#elif defined(LINUX) || defined(HPUX)
#  define ISNAN(x) isnan(x)
#  define ISINF(x) isinf(x)
#elif defined(SUN) || defined(SOLARIS) || defined(SNI)
#include <ieeefp.h>
#  define ISNAN(x) isnan(x)
#  define ISINF(x) (!finite(x))
#elif defined(OSF1) || defined(AIX)
#  define ISNAN(x) isnan(x)
#  define ISINF(x) (!finite(x))
#else
#error Platform not supported.
#endif

#ifdef _WIN32
#include "RunTime/System/RTESys_MSWindows.h"
#else
#  if !defined(SVR4)
#    if defined(LINUX) || defined(AIX)
#      include <time.h>
#    endif
#    include <sys/time.h>
#  else
#    ifdef _POSIX_SOURCE
#      undef _POSIX_SOURCE
#      include <sys/time.h>
#      define _POSIX_SOURCE
#    else
#      include <sys/time.h>
#    endif
#  endif
#endif

#ifndef ODBC_TYPES_DEFINED

struct SQL_DATE_STRUCT
{
  SQLDBC_Int2 year;   
  SQLDBC_UInt2 month; 
  SQLDBC_UInt2 day;   
};


struct SQL_TIME_STRUCT
{
  SQLDBC_UInt2 hour;  
  SQLDBC_UInt2 minute;
  SQLDBC_UInt2 second;
};

struct SQL_TIMESTAMP_STRUCT
{
    SQLDBC_Int2 year; 
    SQLDBC_UInt2 month;
    SQLDBC_UInt2 day;  
    SQLDBC_UInt2 hour; 
    SQLDBC_UInt2 minute; 
    SQLDBC_UInt2 second; 
    SQLDBC_UInt4 fraction;
};

#ifndef SQL_MAX_NUMERIC_LENGTH
#  define SQL_MAX_NUMERIC_LENGTH 16
#endif

#ifndef SQL_MAX_NUMERIC_DIGITS
#  define SQL_MAX_NUMERIC_DIGITS 38
#endif


struct SQL_NUMERIC_STRUCT
{
    SQLDBC_Int1  precision;
    SQLDBC_Int1  scale;    
    SQLDBC_Int1  sign;     
    SQLDBC_UInt1 val[SQL_MAX_NUMERIC_LENGTH];
};

#endif

#ifndef WIN32
struct  GUID
{
    SQLDBC_UInt4 Data1;  
    SQLDBC_UInt2 Data2;  
    SQLDBC_UInt2 Data3;  
    SQLDBC_UInt1 Data4[8];
};
#endif


#define MAX_SQLDBC_INT1  ((SQLDBC_Int1)  127)
#define MIN_SQLDBC_INT1  ((SQLDBC_Int1) -128)
#define MAX_SQLDBC_UINT1  ((SQLDBC_UInt1) 255)
#define MIN_SQLDBC_UINT1  ((SQLDBC_UInt1) 0)
#define MAX_SQLDBC_INT2   ((SQLDBC_Int2)32767)
#define MIN_SQLDBC_INT2   ((SQLDBC_Int2)(-MAX_SQLDBC_INT2 - 1))
#define MAX_SQLDBC_UINT2  ((SQLDBC_UInt2)65535)
#define MIN_SQLDBC_UINT2  ((SQLDBC_UInt2)0)
#define MAX_SQLDBC_INT4   ((SQLDBC_Int4)2147483647)
#define MIN_SQLDBC_INT4   ((SQLDBC_Int4)(-MAX_SQLDBC_INT4 - 1))
#define MAX_SQLDBC_UINT4   ((SQLDBC_UInt4)4294967295U)
#define MIN_SQLDBC_UINT4   ((SQLDBC_UInt4)0)
#ifdef LINUX
#define SQLDBC_INT64_CONSTANT(x) x##LL
#define SQLDBC_UINT64_CONSTANT(x) x##ULL
#elif defined(WIN32)
#define SQLDBC_INT64_CONSTANT(x) x##I64
#define SQLDBC_UINT64_CONSTANT(x) x##UI64
#else
#define SQLDBC_INT64_CONSTANT(x) x##L
#define SQLDBC_UINT64_CONSTANT(x) x##UL
#endif
#define MAX_SQLDBC_INT8   ((SQLDBC_Int8)SQLDBC_INT64_CONSTANT(9223372036854775807))
#if defined(HPUX) && defined(BIT64)
#  define MIN_SQLDBC_INT8  ((SQLDBC_Int8)-9223372036854775808L)
#else
#  define MIN_SQLDBC_INT8   ((SQLDBC_Int8)SQLDBC_INT64_CONSTANT(-9223372036854775808))
#endif
#define MAX_SQLDBC_UINT8  ((SQLDBC_UInt8)SQLDBC_UINT64_CONSTANT(18446744073709551615))
#define MIN_SQLDBC_UINT8  ((SQLDBC_UInt8)SQLDBC_UINT64_CONSTANT(0))



template <class Integer>
Integer shred(Integer value)
{
    return ~value;
}

#if defined(SVR4) || defined(SOLARIS)
template <>
#endif
double shred(double value)
{
    return value * rand();
}

#define CHECK_INTEGRALTYPE_WP(xxtype, xxhosttype, xxvalue, xxindex, xxtablename, xxpformat)                         \
  do {                                                                                                              \
    xxtype x1;                                                                                                      \
    x1=(xxtype)xxvalue;                                                                                             \
    SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();                                    \
    SQLDBC_Length x1len=(SQLDBC_Length) sizeof(xxtype);                                                                   \
    tc_errorcheck(p1)->bindParameter(1, xxhosttype, &x1, &x1len, (SQLDBC_Length)sizeof(xxtype));                       \
    tc_errorcheck(p1)->prepare("INSERT INTO " #xxtablename " VALUES(" #xxindex ", ?)", SQLDBC_StringEncodingAscii);    \
    tc_errorcheck(p1)->execute();                                                                                   \
    xxtype x2;                                                                                                      \
    x2 = (xxtype) shred((xxtype)xxvalue);                                                                           \
    SQLDBC_PreparedStatement *p2=tc_errorcheck(connection())->createPreparedStatement();                                    \
    SQLDBC_Length x2len=(SQLDBC_Length) sizeof(xxtype);                                                                   \
    tc_errorcheck(p2)->bindParameter(1, xxhosttype, &x2, &x2len, (SQLDBC_Length) sizeof(xxtype));                      \
    tc_errorcheck(p2)->prepare("SELECT V INTO ? FROM " #xxtablename " WHERE K=" #xxindex, SQLDBC_StringEncodingAscii); \
    tc_errorcheck(p2)->execute();                                                                                   \
    if(x2 != x1) {                                                                                                  \
        logerror("Value mismatch from inserted value " #xxvalue                                                     \
                 " for host type " #xxhosttype ", " xxpformat " found.", x2);                                       \
    }  else {                                                                                                       \
      if(x2 != (xxtype) xxvalue) {                                                                                  \
        logerror("Value mismatch in value " #xxvalue " for host type " #xxhosttype ", " xxpformat " found.", x2);   \
      }                                                                                                             \
    }                                                                                                               \
    connection()->releaseStatement(p1);                                                                             \
    connection()->releaseStatement(p2);                                                                             \
  } while(0)


#define CHECK_INTEGER(xxtype, xxhosttype, xxvalue, xxindex, xxtablename) \
  CHECK_INTEGRALTYPE_WP(xxtype, xxhosttype, xxvalue, xxindex, xxtablename, "%d")

#define CHECK_FLOAT(xxtype, xxhosttype, xxvalue, xxindex, xxtablename) \
  CHECK_INTEGRALTYPE_WP(xxtype, xxhosttype, xxvalue, xxindex, xxtablename, "%f")


#define CHECK_STRING_Z(xxinsertstr, xxcomparestr, xxhosttype, xxindex, xxtablename) \
CheckString((char *)xxinsertstr, SQLDBC_NTS, (char *)xxcomparestr, xxhosttype, xxindex,(char *) #xxtablename)

#define CHECK_STRING(xxinsertstr, xxstrlen, xxcomparestr, xxhosttype, xxindex, xxtablename) \
CheckString((char *)xxinsertstr, xxstrlen, (char *)xxcomparestr, xxhosttype, xxindex, (char *) #xxtablename)

//----------------------------------------------------------------------
class CharAsciiTest : public SQLDBC_TestCase
{
public:
    CharAsciiTest(const char *test = "CharAsciiTest")
    :SQLDBC_TestCase(test)
    {}

  void CheckString(char *xxinsertstr, SQLDBC_Int4 xxstrlen, char *xxcomparestr, SQLDBC_HostType xxhosttype,
                    int xxindex, char *xxtablename)

  {
      SQLDBC_Length xxlen = (xxstrlen == SQLDBC_NTS) ? strlen(xxinsertstr)+1 : xxstrlen+1;
      if (xxhosttype == SQLDBC_HOSTTYPE_UCS2_NATIVE)
        xxlen *= 2;
      char *x1=new char[xxlen];
      char *x2=new char[xxlen];
      if (xxhosttype == SQLDBC_HOSTTYPE_UCS2_NATIVE) {
        convertToUCS2((unsigned short*)x1, xxlen, xxinsertstr);
        if (xxstrlen != SQLDBC_NTS)
          xxstrlen = xxstrlen*2;
      }
      else
        strcpy(x1, xxinsertstr);
      memcpy(x2, x1, xxlen);
      SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();
      SQLDBC_Length x1len=xxstrlen;
      tc_errorcheck(p1)->bindParameter(1, xxhosttype, x1, &x1len, xxlen);
      char szSQL[500];
      sprintf(szSQL, "INSERT INTO \"%s\" VALUES (%d, ?)", xxtablename, xxindex);
      tc_errorcheck(p1)->prepare(szSQL, SQLDBC_StringEncodingAscii);
      tc_errorcheck(p1)->execute();
      if(memcmp(x1, x2, xxlen)!=0) {
        logerror("Insert modified the data for \"%s\".", xxinsertstr);
      }
      xxlen = strlen(xxcomparestr)+1;
      if (xxhosttype == SQLDBC_HOSTTYPE_UCS2_NATIVE)
        xxlen *= 2;
      delete [] x2;
      x2=new char[xxlen];
      if (xxhosttype == SQLDBC_HOSTTYPE_UCS2_NATIVE)
        convertToUCS2((unsigned short*)x2, xxlen, xxcomparestr);
      else
        strcpy(x2, xxcomparestr);
      char *x3=new char[xxlen];
      memset(x3, 0, xxlen);
      SQLDBC_PreparedStatement *p2=tc_errorcheck(connection())->createPreparedStatement();
      SQLDBC_Length x3len=0;
      tc_errorcheck(p2)->bindParameter(1, xxhosttype, x3, &x3len, xxlen);
      sprintf(szSQL, "SELECT V INTO ? FROM \"%s\" WHERE K=%d", xxtablename, xxindex);
      tc_errorcheck(p2)->prepare(szSQL, SQLDBC_StringEncodingAscii);
      tc_errorcheck(p2)->execute();
      if(memcmp(x2, x3, xxlen)!=0) {
        logerror("Value mismatch in value \"%s\" inserted was \"%s\".", xxcomparestr, xxinsertstr);
      }
      delete [] x1;
      delete [] x2;
      delete [] x3;
      connection()->releaseStatement(p1);
      connection()->releaseStatement(p2);
  }

    virtual SQLDBC_Bool run()
    {
        dropTable("CHARASCII_TEST");
        m_success = m_success && execSQL("CREATE TABLE CHARASCII_TEST (K INT, V CHAR (40) ASCII)");


        CHECK_STRING_Z("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                       "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                       SQLDBC_HOSTTYPE_ASCII,
                       1,
                       CHARASCII_TEST);

        CHECK_STRING_Z("",
                       "                                        ",
                       SQLDBC_HOSTTYPE_ASCII,
                       2,
                       CHARASCII_TEST);

        CHECK_STRING_Z("AA",
                       "AA                                      ",
                       SQLDBC_HOSTTYPE_ASCII,
                       3,
                       CHARASCII_TEST);

        CHECK_STRING_Z("  AA  AA  ",
                       "  AA  AA                                ",
                       SQLDBC_HOSTTYPE_ASCII,
                       4,
                       CHARASCII_TEST);

        CHECK_STRING("1", 1,
                     "1                                       ",
                     SQLDBC_HOSTTYPE_ASCII,
                     5,
                     CHARASCII_TEST);

        CHECK_STRING("1", 1,
                     "1                                       ",
                     SQLDBC_HOSTTYPE_UCS2_NATIVE,
                     6,
                     CHARASCII_TEST);

        CHECK_STRING("1234567890123456789012345678901234567890", 40,
                     "1234567890123456789012345678901234567890",
                     SQLDBC_HOSTTYPE_ASCII,
                     7,
                     CHARASCII_TEST);

        CHECK_STRING("1234567890123456789012345678901234567890", 40,
                     "1234567890123456789012345678901234567890",
                     SQLDBC_HOSTTYPE_UCS2_NATIVE,
                     8,
                     CHARASCII_TEST);

        CHECK_STRING("  34  78  ", 10,
                     "  34  78                                ",
                     SQLDBC_HOSTTYPE_UCS2_NATIVE,
                     9,
                     CHARASCII_TEST);

        CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, 0, 100, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, 0, 101, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, 0, 102, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 0, 103, CHARASCII_TEST);


        CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, 0, 104, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, 0, 105, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, 0, 106, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 0, 107, CHARASCII_TEST);

        CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, -128, 108, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, 127, 109, CHARASCII_TEST);

        CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, -32768, 110, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, 32767, 111, CHARASCII_TEST);


        CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, (-2147483647 - 1), 112, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, 2147483647, 113, CHARASCII_TEST);


#if defined(WIN32)
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, _I64_MIN, 114, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, _I64_MAX, 115, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, _UI64_MAX, 116, CHARASCII_TEST);
#elif (defined(UNIX) && !defined(BIT64)) || defined(LINUX)
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 9223372036854775807LL , 114, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, -9223372036854775808LL, 115, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 18446744073709551615ULL, 116, CHARASCII_TEST);
#else
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 9223372036854775807 , 114, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, -9223372036854775808, 115, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 18446744073709551615UL, 116, CHARASCII_TEST);
#endif
        CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, 4294967295, 117, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, (unsigned short)65535, 118, CHARASCII_TEST);
        CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, (unsigned char)255, 119, CHARASCII_TEST);

        execSQL("COMMIT WORK");
        return m_success;
    }
};


//----------------------------------------------------------------------
class CharUnicodeTest : public CharAsciiTest
{
public:
    CharUnicodeTest()
    :CharAsciiTest("CharUnicodeTest")
    {}

    virtual SQLDBC_Bool run()
    {
      if (!connection()->isUnicodeDatabase()) {
        return m_success;
      }
        dropTable("CHARUNICODE_TEST");
        m_success = m_success && execSQL("CREATE TABLE CHARUNICODE_TEST (K INT, V CHAR (40) UNICODE)");


        CHECK_STRING_Z("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                       "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                       SQLDBC_HOSTTYPE_ASCII,
                       1,
                       CHARUNICODE_TEST);

        CHECK_STRING_Z("",
                       "                                        ",
                       SQLDBC_HOSTTYPE_ASCII,
                       2,
                       CHARUNICODE_TEST);

        CHECK_STRING_Z("AA",
                       "AA                                      ",
                       SQLDBC_HOSTTYPE_ASCII,
                       3,
                       CHARUNICODE_TEST);

        CHECK_STRING_Z("  AA  AA  ",
                       "  AA  AA                                ",
                       SQLDBC_HOSTTYPE_ASCII,
                       4,
                       CHARUNICODE_TEST);

        CHECK_STRING_Z("1",
                       "1                                       ",
                       SQLDBC_HOSTTYPE_UCS2_NATIVE,
                       5,
                       CHARUNICODE_TEST);

        CHECK_STRING_Z("1234567890123456789012345678901234567890",
                       "1234567890123456789012345678901234567890",
                       SQLDBC_HOSTTYPE_UCS2_NATIVE,
                       6,
                       CHARUNICODE_TEST);

        CHECK_STRING  ("  34  78  ", 10,
                       "  34  78                                ",
                       SQLDBC_HOSTTYPE_UCS2_NATIVE,
                       7,
                       CHARUNICODE_TEST);

        CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, 0, 100, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, 0, 101, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, 0, 102, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 0, 103, CHARUNICODE_TEST);


        CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, 0, 104, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, 0, 105, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, 0, 106, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 0, 107, CHARUNICODE_TEST);

        CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, -128, 108, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, 127, 109, CHARUNICODE_TEST);

        CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, -32768, 110, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, 32767, 111, CHARUNICODE_TEST);

        CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, (-2147483647 - 1), 112, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, 2147483647, 113, CHARUNICODE_TEST);

#if defined(WIN32)
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, _I64_MIN, 114, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, _I64_MAX, 115, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, _UI64_MAX, 116, CHARUNICODE_TEST);
#elif (defined(UNIX) && !defined(BIT64)) || defined(LINUX)
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 9223372036854775807LL , 114, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, -9223372036854775808LL, 115, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 18446744073709551615ULL, 116, CHARUNICODE_TEST);
#else
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 9223372036854775807 , 114, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, -9223372036854775808, 115, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 18446744073709551615UL, 116, CHARUNICODE_TEST);
#endif

        CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, 4294967295, 117, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, (unsigned short)65535, 118, CHARUNICODE_TEST);
        CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, (unsigned char)255, 119, CHARUNICODE_TEST);

        execSQL("COMMIT WORK");
        return m_success;
    }
};

//----------------------------------------------------------------------
class Byte1Test : public CharAsciiTest
{
public:
    Byte1Test()
    :CharAsciiTest("Byte1Test")
    {}

    virtual SQLDBC_Bool run()
    {
      dropTable("BYTE1_TEST");
      m_success = m_success && execSQL("CREATE TABLE BYTE1_TEST (K INT, V CHAR (1) BYTE)");


      CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_BINARY, 127, 1, BYTE1_TEST);
      CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_BINARY, 255, 2, BYTE1_TEST);
      execSQL("COMMIT WORK");
      return m_success;
    }
};


class NumericTest : public SQLDBC_TestCase
{
public:
    NumericTest()
    :SQLDBC_TestCase("NumericTest")
    {}

    SQLDBC_Bool runInteger()
    {
        dropTable("INTEGER_TEST");
        m_success = m_success && execSQL("CREATE TABLE INTEGER_TEST (K INT, V INTEGER)");

         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, 17, 1, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, 17, 2, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, 17, 3, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 17, 4, INTEGER_TEST);

         CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, 17, 5, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, 17, 6, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, 17, 7, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 17, 8, INTEGER_TEST);

         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, -17, 9, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, -17, 10, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, -17, 11, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, -17, 12, INTEGER_TEST);

         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, MIN_SQLDBC_INT1, 13, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, MAX_SQLDBC_INT1, 14, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, MIN_SQLDBC_INT2, 15, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, MAX_SQLDBC_INT2, 16, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, MIN_SQLDBC_INT4, 17, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, MAX_SQLDBC_INT4, 18, INTEGER_TEST);

/*
         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, MIN_SQLDBC_INT8, 19, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, MAX_SQLDBC_INT8, 20, INTEGER_TEST);
*/
         CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, MAX_SQLDBC_UINT1, 21, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, MAX_SQLDBC_UINT2, 22, INTEGER_TEST);
         // CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, MAX_SQLDBC_UINT4, 23, INTEGER_TEST);
         // CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, MAX_SQLDBC_UINT8, 24, INTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, -32730, 25, INTEGER_TEST);
         return m_success;
    }

    SQLDBC_Bool runBigInteger()
    {
        dropTable("BIGINTEGER_TEST");
        m_success = m_success && execSQL("CREATE TABLE BIGINTEGER_TEST (K INT, V FIXED(38,0))");

         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, 17, 1, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, 17, 2, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, 17, 3, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, 17, 4, BIGINTEGER_TEST);

         CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, 17, 5, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, 17, 6, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, 17, 7, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, 17, 8, BIGINTEGER_TEST);

         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, -17, 9, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, -17, 10, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, -17, 11, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, -17, 12, BIGINTEGER_TEST);

         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, MIN_SQLDBC_INT1, 13, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int1, SQLDBC_HOSTTYPE_INT1, MAX_SQLDBC_INT1, 14, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, MIN_SQLDBC_INT2, 15, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int2, SQLDBC_HOSTTYPE_INT2, MAX_SQLDBC_INT2, 16, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, MIN_SQLDBC_INT4, 17, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_Int4, SQLDBC_HOSTTYPE_INT4, MAX_SQLDBC_INT4, 18, BIGINTEGER_TEST);

         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, MIN_SQLDBC_INT8, 19, BIGINTEGER_TEST);

         CHECK_INTEGER(SQLDBC_Int8, SQLDBC_HOSTTYPE_INT8, MAX_SQLDBC_INT8, 20, BIGINTEGER_TEST);

         CHECK_INTEGER(SQLDBC_UInt1, SQLDBC_HOSTTYPE_UINT1, MAX_SQLDBC_UINT1, 21, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt2, SQLDBC_HOSTTYPE_UINT2, MAX_SQLDBC_UINT2, 22, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt4, SQLDBC_HOSTTYPE_UINT4, MAX_SQLDBC_UINT4, 23, BIGINTEGER_TEST);
         CHECK_INTEGER(SQLDBC_UInt8, SQLDBC_HOSTTYPE_UINT8, MAX_SQLDBC_UINT8, 24, BIGINTEGER_TEST);

         return m_success;
    }


    virtual SQLDBC_Bool run()
    {
        m_success = m_success && runInteger();
        m_success = m_success && runBigInteger();
        execSQL("COMMIT WORK");
        return m_success;
    }

};

class TimeCharTest
    : public SQLDBC_TestCase
{
public:
    TimeCharTest()
    :SQLDBC_TestCase("TimeCharTest")
    {}

    virtual SQLDBC_Bool run()
    {

        dropTable("TIMECHAR_TEST");
        m_success = m_success && execSQL("CREATE TABLE TIMECHAR_TEST (K INT, V CHAR(8))");
        m_success = m_success && execSQL("INSERT INTO TIMECHAR_TEST (K, V) VALUES (1, '15:32:56')");

        SQL_TIME_STRUCT tm1;
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len=0;
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIME, &tm1, &len, sizeof(SQL_TIME_STRUCT));
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM TIMECHAR_TEST WHERE K=1", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p1)->execute();
        }
        if(tm1.hour!=15 ||
           tm1.minute!=32 ||
           tm1.second!=56) {
            logerror("Difference, expected 15:32:56, found %.2d:%.2d:%.2d", tm1.hour,
                     tm1.minute, tm1.second);
        }
        connection()->releaseStatement(p1);



       SQL_TIME_STRUCT tm2;
       tm2.hour=14;
       tm2.minute=35;
       tm2.second=42;

       SQLDBC_PreparedStatement* p2=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len=0;
           tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIME, &tm2, &len, sizeof(SQLDBC_HOSTTYPE_ODBCTIME));
           tc_errorcheck(p2)->prepare("INSERT INTO TIMECHAR_TEST VALUES (2, ?)", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p2)->execute();
       }
       connection()->releaseStatement(p2);


       SQL_TIME_STRUCT tm3;
       SQLDBC_PreparedStatement *p3=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len = 0;
           tc_errorcheck(p3)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIME, &tm3, &len, sizeof(SQL_TIME_STRUCT));
           tc_errorcheck(p3)->prepare("SELECT V INTO ? FROM TIMECHAR_TEST WHERE K=2", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p3)->execute();
       }
       connection()->releaseStatement(p3);

       if(tm2.hour!=tm3.hour ||
          tm2.minute!=tm3.minute ||
          tm2.second!=tm3.second) {
           logerror("Wrong %.2d:%.2d:%.2d time, expected 14:35:42.");
       }
       execSQL("COMMIT WORK");
       return m_success;
    }
};

class TimeTimeTest
    : public SQLDBC_TestCase
{
public:
    TimeTimeTest()
    :SQLDBC_TestCase("TimeTimeTest")
    {}

    virtual SQLDBC_Bool run()
    {
        dropTable("TIMETIME_TEST");
        m_success = m_success && execSQL("CREATE TABLE TIMETIME_TEST (K INT, V TIME)");
        m_success = m_success && execSQL("INSERT INTO TIMETIME_TEST (K, V) VALUES (1, '15:32:56')");

        SQL_TIME_STRUCT tm1;
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();

        {
            SQLDBC_Length len = 0;
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIME, &tm1, &len, sizeof(SQL_TIME_STRUCT));
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM TIMETIME_TEST WHERE K=1", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p1)->execute();
        }

        if(tm1.hour!=15 ||
           tm1.minute!=32 ||
           tm1.second!=56) {
            logerror("Difference, expected 15:32:56, found %.2d:%.2d:%.2d", tm1.hour,
                     tm1.minute, tm1.second);
        }
        connection()->releaseStatement(p1);

        SQL_TIME_STRUCT tm2;
        tm2.hour=14;
        tm2.minute=35;
        tm2.second=42;

        SQLDBC_PreparedStatement* p2=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len = sizeof(SQL_TIME_STRUCT);
            tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIME, &tm2, &len, sizeof(SQL_TIME_STRUCT));
            tc_errorcheck(p2)->prepare("INSERT INTO TIMETIME_TEST VALUES (2, ?)", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p2)->execute();
        }
        connection()->releaseStatement(p2);

        SQL_TIME_STRUCT tm3;
        SQLDBC_PreparedStatement *p3=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len = 0;
            tc_errorcheck(p3)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIME, &tm3, &len, sizeof(SQL_TIME_STRUCT));
            tc_errorcheck(p3)->prepare("SELECT V INTO ? FROM TIMETIME_TEST WHERE K=2", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p3)->execute();
        }
        connection()->releaseStatement(p3);
        if(tm2.hour!=tm3.hour ||
           tm2.minute!=tm3.minute ||
           tm2.second!=tm3.second) {
            logerror("Wrong %.2d:%.2d:%.2d time, expected 14:35:42.");
        }
        execSQL("COMMIT WORK");
        return m_success;

    }
};



class DateCharTest
    : public SQLDBC_TestCase
{
public:
    DateCharTest()
    :SQLDBC_TestCase("DateCharTest")
    {}

    virtual SQLDBC_Bool run()
    {

        dropTable("DATECHAR_TEST");
        m_success = m_success && execSQL("CREATE TABLE DATECHAR_TEST (K INT, V CHAR(10))");
        m_success = m_success && execSQL("INSERT INTO DATECHAR_TEST (K, V) VALUES (1, '2002-05-06')");

        SQL_DATE_STRUCT dt1;
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len = 0;
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCDATE, &dt1, &len, sizeof(SQL_DATE_STRUCT));
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM DATECHAR_TEST WHERE K=1", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p1)->execute();
        }
        if(dt1.day!=6 || dt1.month!=5 || dt1.year!=2002) {
            logerror("Difference, expected 2002-05-06, found %.4d-%.2d-%.2d", dt1.year,
                     dt1.month, dt1.day);
        }
        connection()->releaseStatement(p1);



       SQL_DATE_STRUCT dt2;
       dt2.year=2002;
       dt2.month=7;
       dt2.day=31;

       SQLDBC_PreparedStatement* p2=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len=0;
           tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCDATE, &dt2, &len, sizeof(SQLDBC_HOSTTYPE_ODBCDATE));
           tc_errorcheck(p2)->prepare("INSERT INTO DATECHAR_TEST VALUES (2, ?)", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p2)->execute();
       }
        connection()->releaseStatement(p2);

       SQL_DATE_STRUCT dt3;
       SQLDBC_PreparedStatement *p3=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len = 0;
           tc_errorcheck(p3)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCDATE, &dt3, &len, sizeof(SQL_DATE_STRUCT));
           tc_errorcheck(p3)->prepare("SELECT V INTO ? FROM DATECHAR_TEST WHERE K=2", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p3)->execute();
       }
       connection()->releaseStatement(p3);

       if(!memcmp(&dt2, &dt3, sizeof(SQL_DATE_STRUCT)) == 0) {
           logerror("Difference, expected 2002-07-31, found %.4d-%.2d-%.2d", dt3.year,
                    dt3.month, dt3.day);
       }
       execSQL("COMMIT WORK");
       return m_success;
    }
};

class DateDateTest
    : public SQLDBC_TestCase
{
public:
    DateDateTest()
    :SQLDBC_TestCase("DateDateTest")
    {}

    virtual SQLDBC_Bool run()
    {

        dropTable("DATEDATE_TEST");
        m_success = m_success && execSQL("CREATE TABLE DATEDATE_TEST (K INT, V DATE)");
        m_success = m_success && execSQL("INSERT INTO DATEDATE_TEST (K, V) VALUES (1, '2002-05-06')");

        SQL_DATE_STRUCT dt1;
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len = 0;
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCDATE, &dt1, &len, sizeof(SQL_DATE_STRUCT));
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM DATEDATE_TEST WHERE K=1", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p1)->execute();
        }
        if(dt1.day!=6 || dt1.month!=5 || dt1.year!=2002) {
            logerror("Difference, expected 2002-05-06, found %.4d-%.2d-%.2d", dt1.year,
                     dt1.month, dt1.day);
        }
        connection()->releaseStatement(p1);
        

       SQL_DATE_STRUCT dt2;
       dt2.year=2002;
       dt2.month=7;
       dt2.day=31;

       SQLDBC_PreparedStatement* p2=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len=0;
           tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCDATE, &dt2, &len, sizeof(SQLDBC_HOSTTYPE_ODBCDATE));
           tc_errorcheck(p2)->prepare("INSERT INTO DATEDATE_TEST VALUES (2, ?)", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p2)->execute();
       }
       connection()->releaseStatement(p2);

       SQL_DATE_STRUCT dt3;
       SQLDBC_PreparedStatement *p3=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len = 0;
           tc_errorcheck(p3)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCDATE, &dt3, &len, sizeof(SQL_DATE_STRUCT));
           tc_errorcheck(p3)->prepare("SELECT V INTO ? FROM DATEDATE_TEST WHERE K=2", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p3)->execute();
       }
       connection()->releaseStatement(p3);
        
       if(!memcmp(&dt2, &dt3, sizeof(SQL_DATE_STRUCT)) == 0) {
           logerror("Difference, expected 2002-07-31, found %.4d-%.2d-%.2d", dt3.year,
                    dt3.month, dt3.day);
       }
       execSQL("COMMIT WORK");
       return m_success;
    }
};


class FloatFloatTest
    : public SQLDBC_TestCase
{
public:
    FloatFloatTest()
    :SQLDBC_TestCase("FloatFloatTest")
    {}

    void checkValue(int key, double value, const char *tablename)
    {
        double x1;
        x1=value;
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Length x1len=sizeof(double);
        tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_DOUBLE, &x1, &x1len, sizeof(double));
        char insert_stm[1024];
        sprintf(insert_stm, "INSERT INTO  %s VALUES(%d, ?)", tablename, key);
        tc_errorcheck(p1)->prepare(insert_stm, SQLDBC_StringEncodingAscii);
        tc_errorcheck(p1)->execute();
        double x2;
        x2 = shred((double)value);
        SQLDBC_PreparedStatement *p2=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Length x2len=sizeof(double);
        tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_DOUBLE, &x2, &x2len, sizeof(double));
        char select_stm[1024];
        sprintf(select_stm, "SELECT V INTO ? FROM  %s  WHERE K=%d", tablename, key);
        tc_errorcheck(p2)->prepare(select_stm, SQLDBC_StringEncodingAscii);
        tc_errorcheck(p2)->execute();
        char error[1024];
        // AS INEXACT AS JAVA
        if(fabs(x2 -x1) > 1.0e-13) {
            sprintf(error, "Value mismatch from inserted value %.38f for host type SQLDBC_HOSTTYPE_DOUBLE, %.38f found.",
                    x1, x2);
            logerror(error);
        }
        connection()->releaseStatement(p1);
        connection()->releaseStatement(p2);
    }

    virtual SQLDBC_Bool run()
    {
        return run38() && run10();
    }

    SQLDBC_Bool run38()
    {

        dropTable("FLOATFLOAT_TEST38");
        m_success = m_success && execSQL("CREATE TABLE FLOATFLOAT_TEST38 (K INT, V FLOAT(38))");

        double test_array[] = { 0.0, 7.0, 1.0, -1.0, 42.7, 3.1, 3.14, 3.141, 3.1415, 3.14159,
                                3.141592, 3.14159265, 91.5, 100.7 };
        for(int i=0; i<sizeof(test_array)/sizeof(double); ++i) {
            checkValue(i+1, test_array[i], "FLOATFLOAT_TEST38");
        }
        execSQL("COMMIT WORK");
        return m_success;
    }

    SQLDBC_Bool run10()
    {
        dropTable("FLOATFLOAT_TEST10");
        m_success = m_success && execSQL("CREATE TABLE FLOATFLOAT_TEST10 (K INT, V FLOAT(10))");

        double test_array[] = { -42.7, 0.0, 7.0, 1.0, -1.0, 42.7, 3.1, 3.14, 3.141, 91.5, -100.7 };
        for(int i=0; i<sizeof(test_array)/sizeof(double); ++i) {
            checkValue(i+1, test_array[i], "FLOATFLOAT_TEST10");
        }
        execSQL("COMMIT WORK");
        return m_success;
    }

};

//----------------------------------------------------------------------
class FloatNANTest
    : public SQLDBC_TestCase
{
public:
    FloatNANTest()
    :SQLDBC_TestCase("FloatNANTest")
    {}

    virtual SQLDBC_Bool run()
    {
        dropTable("FLOATNAN_TEST");
        m_success = m_success && execSQL("CREATE TABLE FLOATNAN_TEST (A FLOAT, B FLOAT)");
        m_success = m_success && execSQL("INSERT INTO FLOATNAN_TEST VALUES (1.0, 0.0)");
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Length x1len=sizeof(double);
        double x1;
        tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_DOUBLE, &x1, &x1len, sizeof(double));
        tc_errorcheck(p1)->prepare("SELECT A/B INTO ? FROM FLOATNAN_TEST", SQLDBC_StringEncodingAscii);
        tc_errorcheck(p1)->execute();
        if(!ISNAN(x1)) {
            logerror("Expected NAN output through division by zero in data.");
        }
// disabled, kernel croaks on these values
#if 0
        SQLDBC_PreparedStatement *p2=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(p2)->prepare("INSERT INTO FLOATNAN_TEST VALUES (?, ?)", SQLDBC_StringEncodingAscii);
        tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_DOUBLE, &x1, &x1len, sizeof(double));
        tc_errorcheck(p2)->bindParameter(2, SQLDBC_HOSTTYPE_DOUBLE, &x1, &x1len, sizeof(double));
        tc_errorcheck(p2)->execute();
        connection()->releaseStatement(p2);
#endif
        execSQL("COMMIT WORK");
        connection()->releaseStatement(p1);
        return m_success;
    }
};


//----------------------------------------------------------------------
class NumericCharTest
    : public SQLDBC_TestCase
{
public:
    NumericCharTest()
    :SQLDBC_TestCase("NumericCharTest")
    {}

    SQLDBC_Bool run()
    {
        m_success = runFixedInsert();
        m_success = m_success && runFixed();
        m_success = m_success && runFloat();
        return m_success;
    }

    SQLDBC_Bool runFixedInsert()
    {
        dropTable("FIXEDCHAR_INSERTTEST");
        m_success = m_success && execSQL("CREATE TABLE FIXEDCHAR_INSERTTEST (V FIXED(10,3))");
#define CHECK_VALUE(v, vcmp)                                                                                    \
        do {                                                                                                    \
            execSQL("DELETE FROM FIXEDCHAR_INSERTTEST");                                                        \
            char inbuffer[256];                                                                                 \
            SQLDBC_Length inbufferlength=SQLDBC_NTS;                                                                  \
            strcpy(inbuffer, v);                                                                                \
            SQLDBC_PreparedStatement *p0=tc_errorcheck(connection())->createPreparedStatement();                        \
            tc_errorcheck(p0)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, inbuffer, &inbufferlength, 256);            \
            tc_errorcheck(p0)->prepare("INSERT INTO FIXEDCHAR_INSERTTEST VALUES (?)", SQLDBC_StringEncodingAscii); \
            tc_errorcheck(p0)->execute();                                                                       \
            char outbuffer[256];                                                                                \
            SQLDBC_Length outbufferlen=256;                                                                        \
            SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();                        \
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, outbuffer, &outbufferlen, 256);             \
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM FIXEDCHAR_INSERTTEST", SQLDBC_StringEncodingAscii);   \
            tc_errorcheck(p1)->execute();                                                                       \
            m_success = (strcmp(vcmp, outbuffer) == 0);                                                         \
            loginfo("Value expected: |%s| found |%s|", vcmp, outbuffer);                                        \
            connection()->releaseStatement(p0);                                                                 \
            connection()->releaseStatement(p1);                                                                 \
        } while(0)

        CHECK_VALUE("111.111", "111.111");
        CHECK_VALUE("1", "1.000");
        CHECK_VALUE("0", "0");
        CHECK_VALUE("-1", "-1.000");
        CHECK_VALUE("1e3", "1000.000");
        CHECK_VALUE("+0000000000015.00", "15.000");
        CHECK_VALUE(".123", "0.123");
        CHECK_VALUE("0.123", "0.123");
        CHECK_VALUE("1.001", "1.001");
        CHECK_VALUE("0.001", "0.001");
        CHECK_VALUE("+000000.00", "0.000");
#undef CHECK_VALUE
        return m_success;
    }


    SQLDBC_Bool runFixed()
    {
        dropTable("FIXEDCHAR_TEST");
        m_success = m_success && execSQL("CREATE TABLE FIXEDCHAR_TEST (V FIXED(10,3))");

#define CHECK_VALUE( v )                                                                                \
        do {                                                                                            \
            execSQL("DELETE FROM FIXEDCHAR_TEST");                                                      \
            execSQL("INSERT INTO FIXEDCHAR_TEST VALUES (" v ")");                                       \
            char buffer[256];                                                                           \
            SQLDBC_Length bufferlen=256;                                                                   \
            SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();                \
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, buffer, &bufferlen, 256);           \
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM FIXEDCHAR_TEST", SQLDBC_StringEncodingAscii); \
            tc_errorcheck(p1)->execute();                                                               \
            m_success = (strcmp(v, buffer) == 0);                                                       \
            /* loginfo("Value expected: |%s| found |%s|", v, buffer); */                                \
           connection()->releaseStatement(p1);                                                          \
        } while(0)

        CHECK_VALUE("11.200");
        CHECK_VALUE("11.201");
        CHECK_VALUE("0");
        CHECK_VALUE("0.001");
        CHECK_VALUE("0.100");
        CHECK_VALUE("100.000");
        CHECK_VALUE("9999999.999");
        CHECK_VALUE("-9999999.999");

        execSQL("COMMIT WORK");
        return m_success;
    }

    SQLDBC_Bool runFloat()
    {
        dropTable("FLOATCHAR_TEST");
        m_success = m_success && execSQL("CREATE TABLE FLOATCHAR_TEST (V FLOAT(20))");

#undef CHECK_VALUE
#define CHECK_VALUE( v )                                                                                \
        do {                                                                                            \
            execSQL("DELETE FROM FLOATCHAR_TEST");                                                      \
            execSQL("INSERT INTO FLOATCHAR_TEST VALUES (" v ")");                                       \
            char buffer[256];                                                                           \
            SQLDBC_Length bufferlen=256;                                                                   \
            SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();                \
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, buffer, &bufferlen, 256);           \
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM FLOATCHAR_TEST", SQLDBC_StringEncodingAscii); \
            tc_errorcheck(p1)->execute();                                                               \
            m_success = (strcmp(v, buffer) == 0);                                                       \
            /* loginfo("Value expected: |%s| found |%s|", v, buffer); */                                \
           connection()->releaseStatement(p1);                                                          \
        } while(0)

        CHECK_VALUE("0");
        CHECK_VALUE("0.1");
        CHECK_VALUE("0.11");
        CHECK_VALUE("0.111");
        CHECK_VALUE("1E25");
        CHECK_VALUE("1E-25");
        CHECK_VALUE("1.1E25");
        CHECK_VALUE("1.111E25");
        CHECK_VALUE("1.111E-25");
        CHECK_VALUE("3.14159265");
        CHECK_VALUE("1.111E-62");
        CHECK_VALUE("1.111E62");
        execSQL("COMMIT WORK");
        return m_success;
    }



};

//----------------------------------------------------------------------
class UCS2CharTest : public SQLDBC_TestCase
{
public:
    UCS2CharTest(const char *test = "UCS2CharTest")
    :SQLDBC_TestCase(test)
    {}

    SQLDBC_Bool run()
    {
        m_success=true;
        execSQL("DROP TABLE UCS2_TEST");
        SQLDBC_Retcode rc = statement()->execute("CREATE TABLE UCS2_TEST(K INT PRIMARY KEY, V VARCHAR(200) UNICODE)",
                                              SQLDBC_StringEncodingAscii);
        if(rc != SQLDBC_OK) {
            if(statement()->error().getErrorCode()==-3002) {
                loginfo("Skipping unicode test for non-unicode database.");
                return m_success=true;
            }
            logerror("Error: %d sql state %5s, %s in file %s:%d",
                     statement()->error().getErrorCode(),
                     statement()->error().getSQLState(),
                     statement()->error().getErrorText(),
                     __FILE__,
                     __LINE__);
            return m_success=false;
        }

        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Bool memory_ok=true;
        ps->prepare("INSERT INTO UCS2_TEST VALUES (1, 'TESTDATA')");
        tc_errorcheck(ps)->execute();
        connection()->releaseStatement(ps);

        SQLDBC_PreparedStatement *ps2=tc_errorcheck(connection())->createPreparedStatement();
        const char *data_expected = "T\0E\0S\0T\0D\0A\0T\0A\0\0\0";
        char data[18];
        SQLDBC_Length lengthindicator = 0;
        tc_errorcheck(ps2)->prepare("SELECT V INTO ? FROM UCS2_TEST WHERE K=1", SQLDBC_StringEncodingAscii);
        tc_errorcheck(ps2)->bindParameter(1, SQLDBC_HOSTTYPE_UCS2_SWAPPED, data, &lengthindicator, 18);
        tc_errorcheck(ps2)->execute();
        connection()->releaseStatement(ps2);
        if(memcmp(data, data_expected, 18)!=0) {
            logerror("Data differs from expected value.");
        }
        return m_success;
    }

};

//----------------------------------------------------------------------
class SQLNumericTest
    : public SQLDBC_TestCase
{
public:
    SQLNumericTest()
    :SQLDBC_TestCase("SQLNumericTest")
    {}

    SQLDBC_Bool run()
    {
        m_success = runNumericInputTest("FIXED(10,5)", "2A", 0, 1, "42.00000");
        m_success = m_success && runNumericInputTest("FIXED(10,5)", "2A", 0, 0, "-42.00000");
        m_success = m_success && runNumericInputTest("FIXED(10,5)", "2A", 1, 0, "-4.20000");
        m_success = m_success && runNumericInputTest("FIXED(10,5)", "2A", 4, 0, "-0.00420");
        m_success = m_success && runNumericInputTest("FIXED(38,0)", "9210", 0, 1, "4242");
        m_success = m_success && runNumericInputTest("FIXED(38,0)", "B2578702", 0, 1, "42424242");
        m_success = m_success && runNumericInputTest("FIXED(10,5)", "00", 0, 0, "0.00000");
        m_success = m_success && runNumericInputTest("VARCHAR(255)", "2A", 0, 0, "-42");
        m_success = m_success && runNumericInputTest("VARCHAR(255)", "2A", 1, 0, "-4.2");
        m_success = m_success && runNumericInputTest("VARCHAR(255)", "2A", 4, 0, "-0.0042");
        m_success = m_success && runNumericInputTest("VARCHAR(255)", "9210", 0, 1, "4242");
        m_success = m_success && runNumericInputTest("VARCHAR(255)", "B2578702", 0, 1, "42424242");


        // m_success = m_success && runNumericOutputTest("FIXED(38,0)", "B2578702", 0, 1, "42424242");
        return m_success;
    }

    SQLDBC_Bool runNumericInputTest(const char *columntype,
                                 const char *hexstr,
                                 SQLDBC_Int4    scale,
                                 SQLDBC_Int4    sign,
                                 const char *stringrep)
    {
        execSQL("DROP TABLE SQLNUM_TEST");
        char createTableCmd[1024];
        sprintf(createTableCmd, "CREATE TABLE SQLNUM_TEST(V %s)", columntype);

        { tc_errorcheck(statement())->execute(createTableCmd, SQLDBC_StringEncodingAscii); }
        if(m_success==false) { return false; }

        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        SQL_NUMERIC_STRUCT testvalue;
        initNumeric(testvalue, hexstr, scale, sign);
        SQLDBC_Length numericLength=sizeof(SQL_NUMERIC_STRUCT);
        { tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCNUMERIC, &testvalue, &numericLength, numericLength); }
        { tc_errorcheck(ps)->prepare("INSERT INTO SQLNUM_TEST VALUES (?)", SQLDBC_StringEncodingAscii); }
        { tc_errorcheck(ps)->execute(); }
        connection()->commit();
        connection()->releaseStatement(ps);

        SQLDBC_PreparedStatement *psread=tc_errorcheck(connection())->createPreparedStatement();
        char charvalue[1024];
        SQLDBC_Length charvalueind = 0;

        { tc_errorcheck(psread)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, charvalue, &charvalueind, 1024); }
        { tc_errorcheck(psread)->prepare("SELECT V INTO ? FROM SQLNUM_TEST", SQLDBC_StringEncodingAscii); }
        { tc_errorcheck(psread)->execute(); }

        if(strcmp(charvalue, stringrep) != 0) {
            logerror("Expected value %s, found %s.", stringrep, charvalue);
        }

        connection()->releaseStatement(psread);

        return m_success;
    }

    SQLDBC_Bool runNumericOutputTest(const char *columntype,
                                 const char *hexstr,
                                 SQLDBC_Int4    scale,
                                 SQLDBC_Int4    sign,
                                 const char *stringrep)
    {
        execSQL("DROP TABLE SQLNUM_TEST");
        char createTableCmd[1024];
        sprintf(createTableCmd, "CREATE TABLE SQLNUM_TEST(V %s)", columntype);

        { tc_errorcheck(statement())->execute(createTableCmd, SQLDBC_StringEncodingAscii); }
        if(m_success==false) { return false; }

        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        { tc_errorcheck(ps)->prepare("INSERT INTO SQLNUM_TEST VALUES (?)", SQLDBC_StringEncodingAscii); }
        SQLDBC_Length stringLength=strlen(stringrep);
        { tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, (void *)stringrep, &stringLength, stringLength); }
        { tc_errorcheck(ps)->execute(); }
        connection()->commit();
        connection()->releaseStatement(ps);
        SQL_NUMERIC_STRUCT testvalue;
        SQL_NUMERIC_STRUCT checkvalue;
        initNumeric(checkvalue, hexstr, scale, sign);
        SQLDBC_Length numericLength=sizeof(SQL_NUMERIC_STRUCT);
        SQLDBC_PreparedStatement *psread=tc_errorcheck(connection())->createPreparedStatement();
        { tc_errorcheck(psread)->prepare("SELECT V INTO ? FROM SQLNUM_TEST", SQLDBC_StringEncodingAscii); }
        { tc_errorcheck(psread)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCNUMERIC, &testvalue, &numericLength, numericLength); }
        { tc_errorcheck(psread)->execute(); }

        if(memcmp(testvalue.val, checkvalue.val, 16) != 0) {
            logerror("Error, mantissa did not match for %s ", stringrep);
        }
        if(testvalue.scale != checkvalue.scale) {
            logerror("Error, scale found is % instead of %d", (SQLDBC_Int4)testvalue.scale, (SQLDBC_Int4)checkvalue.scale);
        }

        connection()->releaseStatement(psread);

        return m_success;
    }



    void initNumeric(SQL_NUMERIC_STRUCT& num,
                     const char *hexstr,
                     SQLDBC_Int4    scale,
                     SQLDBC_Int4    sign)
    {
        memset(&num, 0, sizeof(SQL_NUMERIC_STRUCT));
        num.scale = (SQLDBC_Int1)scale;
        num.sign  = (SQLDBC_UInt1)sign;

        SQLDBC_Length l=strlen(hexstr);
        SQLDBC_Int4 valindex=0;
        if(l>38) 
            l=38;
        for(SQLDBC_Int4 i=0; i< l; ++i) {
            if(i & 1) {
                num.val[valindex] |= getValue(hexstr[i]);
                ++valindex;
            } else {
                num.val[valindex] = getValue(hexstr[i]) << 4;
            }
        }
        return;
    }

    unsigned char getValue(char hexchar)
    {
        if(hexchar >= '0' && hexchar <= '9') {
            return hexchar - '0';
        } else if(hexchar >='A' && hexchar <='F') {
            return 10 + (hexchar - 'A');
        } else if(hexchar >='a' && hexchar <='a') {
            return 10 + (hexchar - 'a');
        } else {
            return 0;
        }
    }

};

class DecimalTest
    : public SQLDBC_TestCase
{
public:
    DecimalTest()
    :SQLDBC_TestCase("DecimalTest")
    {}

    
    SQLDBC_Bool run()
    {
         m_success |= runDecimal();
         m_success |= runDecimal2();
         m_success |= runFixed();
         m_success |= runDecimal3();
//        m_success |= runFixed2();
        return m_success;
    }
    
    SQLDBC_Bool runDecimal()
    {
        dropTable("DECIMALTEST");
        execSQL("CREATE TABLE DECIMALTEST(A FIXED(15,5))");
        unsigned char num[8];
        num[0] = 0x12;
        num[1] = 0x34;
        num[2] = 0x56;
        num[3] = 0x78;
        num[4] = 0x98;
        num[5] = 0x76;
        num[6] = 0x54;
        num[7] = 0x3C;
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO DECIMALTEST VALUES (?)");
        SQLDBC_Length num_length = SQLDBC_LEN_DECIMAL(15,5);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DECIMAL, num, &num_length, 8, false);
        tc_errorcheck(ps)->execute();
        connection()->commit();
        
        
        tc_errorcheck(ps)->prepare("SELECT A INTO ? FROM DECIMALTEST");
        tc_errorcheck(ps)->clearParameters();

        unsigned char numout[8];
        SQLDBC_Length buffer_length = SQLDBC_LEN_DECIMAL(15,5);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DECIMAL, &numout, &buffer_length, 8, false);
        tc_errorcheck(ps)->execute();
        if(memcmp(numout, num, 8) == 0) {
            loginfo("BCD output was successful");
        } else {
            logerror("BCD output failed.");
        }

        
        connection()->releaseStatement(ps);
        return m_success;
    }
    
    SQLDBC_Bool runDecimal3()
    {
        dropTable("DECIMAL3");
        execSQL("CREATE TABLE DECIMAL3(A FIXED(14,3))");
        unsigned char num[8];
        num[0] = 0x12;
        num[1] = 0x34;
        num[2] = 0x56;
        num[3] = 0x78;
        num[4] = 0x98;
        num[5] = 0x76;
        num[6] = 0x54;
        num[7] = 0x0C;
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO DECIMAL3 VALUES (?)");
        SQLDBC_Length num_length = SQLDBC_LEN_DECIMAL(14,3);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DECIMAL, num, &num_length, 8, false);
        tc_errorcheck(ps)->execute();
        connection()->commit();
        
        
        tc_errorcheck(ps)->prepare("SELECT A INTO ? FROM DECIMAL3");
        tc_errorcheck(ps)->clearParameters();

        unsigned char numout[8];
        SQLDBC_Length buffer_length = SQLDBC_LEN_DECIMAL(14,3);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DECIMAL, &numout, &buffer_length, 8, false);
        tc_errorcheck(ps)->execute();
        if(memcmp(numout, num, 8) == 0) {
            loginfo("BCD output was successful");
        } else {
            logerror("BCD output failed.");
        }

        
        connection()->releaseStatement(ps);
        return m_success;
    }

    SQLDBC_Bool runDecimal2()
    {
        dropTable("DECIMALTEST");
        execSQL("CREATE TABLE DECIMALTEST(A FIXED(15,5))");
        unsigned char num[8];
        num[0] = 0x12;
        num[1] = 0x34;
        num[2] = 0x56;
        num[3] = 0x78;
        num[4] = 0x9C;
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO DECIMALTEST VALUES (?)");
        SQLDBC_Length num_length = SQLDBC_LEN_DECIMAL(9,0);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DECIMAL, num, &num_length, 5, false);
        tc_errorcheck(ps)->execute();
        connection()->commit();
        
        
        tc_errorcheck(ps)->prepare("SELECT A INTO ? FROM DECIMALTEST");
        tc_errorcheck(ps)->clearParameters();

        unsigned char numout[5];
        SQLDBC_Length buffer_length = SQLDBC_LEN_DECIMAL(9,0);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DECIMAL, &numout, &buffer_length, 5, false);
        tc_errorcheck(ps)->execute();
        if(memcmp(numout, num, 5) == 0) {
            loginfo("BCD output was successful");
        } else {
            logerror("BCD output failed.");
        }

        
        connection()->releaseStatement(ps);
        return m_success;
    }
    
    SQLDBC_Bool runFixed() 
    {
        dropTable("DECIMALTEST_OMS");
        execSQL("CREATE TABLE DECIMALTEST_OMS(OMSPACKED83 FIXED(15, 3))");
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        ps->prepare("INSERT INTO DECIMALTEST_OMS VALUES (?)");
        unsigned char num[8];
        num[0] = 0x55;
        num[1] = 0x55;
        num[2] = 0x55;
        num[3] = 0x55;
        num[4] = 0x55;
        num[5] = 0x55;
        num[6] = 0x55;
        num[7] = 0x5D;
        SQLDBC_Length num_length=8;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_OMS_PACKED_8_3, num, &num_length, 8);
        tc_errorcheck(ps)->execute();
        connection()->commit();
        tc_errorcheck(ps)->prepare("SELECT OMSPACKED83 INTO ? FROM DECIMALTEST_OMS");
        tc_errorcheck(ps)->clearParameters();
        char buffer[18];
        SQLDBC_Length buffer_length = 18;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, buffer, &buffer_length, 18, true);
        tc_errorcheck(ps)->execute();
        if(strcmp(buffer, "-555555555555.555")==0) {
            loginfo("Correct value found.");
        } else {
            logerror("Found incorrect value %s.", buffer);
        }
        unsigned char numout[8];
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_OMS_PACKED_8_3, &numout, &buffer_length, 8, false);
        tc_errorcheck(ps)->execute();
        if(memcmp(numout, num, 8) == 0) {
            loginfo("BCD output was successful");
        } else {
            logerror("BCD output failed.");
        }
        connection()->releaseStatement(ps);
        return m_success;
    }
    SQLDBC_Bool runFixed2() 
    {
        dropTable("DECIMALTEST_OMS");
        execSQL("CREATE TABLE DECIMALTEST_OMS(OMSPACKED15_3 FIXED(15, 3))");
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        ps->prepare("INSERT INTO DECIMALTEST_OMS VALUES (?)");
        unsigned char num[8];
        num[0] = 0x55;
        num[1] = 0x55;
        num[2] = 0x55;
        num[3] = 0x55;
        num[4] = 0x55;
        num[5] = 0x55;
        num[6] = 0x55;
        num[7] = 0x5D;
        SQLDBC_Length num_length=8;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_OMS_PACKED_15_3, num, &num_length, 8);
        tc_errorcheck(ps)->execute();
        connection()->commit();
        tc_errorcheck(ps)->prepare("SELECT OMSPACKED15_3 INTO ? FROM DECIMALTEST_OMS");
        tc_errorcheck(ps)->clearParameters();
        char buffer[18];
        SQLDBC_Length buffer_length = 18;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, buffer, &buffer_length, 18, true);
        tc_errorcheck(ps)->execute();
        if(strcmp(buffer, "-555555555555.555")==0) {
            loginfo("Correct value found.");
        } else {
            logerror("Found incorrect value %s.", buffer);
        }
        unsigned char numout[8];
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_OMS_PACKED_15_3, &numout, &buffer_length, 8, false);
        tc_errorcheck(ps)->execute();
        if(memcmp(numout, num, 8) == 0) {
            loginfo("BCD output was successful");
        } else {
            logerror("BCD output failed.");
        }
        connection()->releaseStatement(ps);
        return m_success;
    }
 };

class TimestampTimeTest
    : public SQLDBC_TestCase
{
public:
    TimestampTimeTest()
    :SQLDBC_TestCase("TimestampTimeTest")
    {}

    virtual SQLDBC_Bool run()
    {
        dropTable("TIMESTAMPTIME_TEST");
        m_success = m_success && execSQL("CREATE TABLE TIMESTAMPTIME_TEST (K INT, V TIME)");
        m_success = m_success && execSQL("INSERT INTO TIMESTAMPTIME_TEST (K, V) VALUES (1, '15:32:56')");

        SQL_TIMESTAMP_STRUCT tm1;
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();

        {
            SQLDBC_Length len = 0;
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &tm1, &len, sizeof(SQL_TIMESTAMP_STRUCT));
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM TIMESTAMPTIME_TEST WHERE K=1", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p1)->execute();
        }

        if(tm1.hour!=15 ||
           tm1.minute!=32 ||
           tm1.second!=56) {
            logerror("Difference, expected 15:32:56, found %.2d:%.2d:%.2d", tm1.hour,
                     tm1.minute, tm1.second);
        }
        connection()->releaseStatement(p1);

        SQL_TIMESTAMP_STRUCT tm2;
        tm2.year=9999;
        tm2.month=88;
        tm2.day=77;
        tm2.hour=14;
        tm2.minute=35;
        tm2.second=42;
        tm2.fraction=0;

        SQLDBC_PreparedStatement* p2=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len = sizeof(SQL_TIMESTAMP_STRUCT);
            tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &tm2, &len, sizeof(SQL_TIMESTAMP_STRUCT));
            tc_errorcheck(p2)->prepare("INSERT INTO TIMESTAMPTIME_TEST VALUES (2, ?)", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p2)->execute();
        }
        connection()->releaseStatement(p2);

        SQL_TIMESTAMP_STRUCT tm3;
        SQLDBC_PreparedStatement *p3=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len = 0;
            tc_errorcheck(p3)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &tm3, &len, sizeof(SQL_TIMESTAMP_STRUCT));
            tc_errorcheck(p3)->prepare("SELECT V INTO ? FROM TIMESTAMPTIME_TEST WHERE K=2", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p3)->execute();
        }
        connection()->releaseStatement(p3);
        if(tm2.hour!=tm3.hour ||
           tm2.minute!=tm3.minute ||
           tm2.second!=tm3.second) {
            logerror("Wrong %.2d:%.2d:%.2d time, expected 14:35:42.");
        }
#ifdef _WIN32
        SYSTEMTIME systime;
        GetLocalTime(&systime);
        if (tm3.year != systime.wYear ||
            tm3.month != systime.wMonth ||
            tm3.day != systime.wDay) {
          logerror("Wrong %.2d-%.2d-%.4d timestamp date, expected %.2d-%.2d-%.4d.",
                   tm3.day, tm3.month, tm3.year, systime.wDay, systime.wMonth, systime.wYear);
        }
#else
        struct timeval tv;
        gettimeofday(&tv, 0);
        struct tm tmval; 
        localtime_r(&tv.tv_sec,
                    &tmval);
        if (tm3.year != tmval.tm_year ||
            tm3.month != tmval.tm_mon ||
            tm3.day != tmval.tm_mday) {
          logerror("Wrong %.2d-%.2d-%.4d timestamp date, expected %.2d-%.2d-%.4d.",
                   tm3.day, tm3.month, tm3.year, tmval.tm_mday, tmval.tm_mon, tmval.tm_year);
        }
#endif
        execSQL("COMMIT WORK");
        return m_success;

    }
};

class TimestampDateTest
    : public SQLDBC_TestCase
{
public:
    TimestampDateTest()
    :SQLDBC_TestCase("TimestampDateTest")
    {}

    virtual SQLDBC_Bool run()
    {
        dropTable("TIMESTAMPDATE_TEST");
        m_success = m_success && execSQL("CREATE TABLE TIMESTAMPDATE_TEST (K INT, V DATE)");
        m_success = m_success && execSQL("INSERT INTO TIMESTAMPDATE_TEST (K, V) VALUES (1, '2002-05-06')");

        SQL_TIMESTAMP_STRUCT dt1;
        SQLDBC_PreparedStatement *p1=tc_errorcheck(connection())->createPreparedStatement();
        {
            SQLDBC_Length len = 0;
            tc_errorcheck(p1)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &dt1, &len, sizeof(SQL_TIMESTAMP_STRUCT));
            tc_errorcheck(p1)->prepare("SELECT V INTO ? FROM TIMESTAMPDATE_TEST WHERE K=1", SQLDBC_StringEncodingAscii);
            tc_errorcheck(p1)->execute();
        }
        if(dt1.day!=6 || dt1.month!=5 || dt1.year!=2002) {
            logerror("Difference, expected 2002-05-06, found %.4d-%.2d-%.2d", dt1.year,
                     dt1.month, dt1.day);
        }
        connection()->releaseStatement(p1);
        

       SQL_TIMESTAMP_STRUCT dt2;
       dt2.year=2002;
       dt2.month=7;
       dt2.day=31;
       dt2.hour=0;
       dt2.minute=0;
       dt2.second=0;
       dt2.fraction=0;

       SQLDBC_PreparedStatement* p2=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len=0;
           tc_errorcheck(p2)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &dt2, &len, sizeof(SQLDBC_HOSTTYPE_ODBCTIMESTAMP));
           tc_errorcheck(p2)->prepare("INSERT INTO TIMESTAMPDATE_TEST VALUES (2, ?)", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p2)->execute();
       }
       connection()->releaseStatement(p2);

       SQL_TIMESTAMP_STRUCT dt3;
       memset(&dt3, 'x', sizeof(SQL_TIMESTAMP_STRUCT));
       SQLDBC_PreparedStatement *p3=tc_errorcheck(connection())->createPreparedStatement();
       {
           SQLDBC_Length len = 0;
           tc_errorcheck(p3)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &dt3, &len, sizeof(SQL_TIMESTAMP_STRUCT));
           tc_errorcheck(p3)->prepare("SELECT V INTO ? FROM TIMESTAMPDATE_TEST WHERE K=2", SQLDBC_StringEncodingAscii);
           tc_errorcheck(p3)->execute();
       }
       connection()->releaseStatement(p3);
        
       if(dt2.year != dt3.year ||
          dt2.month != dt3.month ||
          dt2.year != dt3.year) {
           logerror("Difference, expected 2002-07-31, found %.4d-%.2d-%.2d", dt3.year,
                    dt3.month, dt3.day);
       }
       if(dt3.hour != 0 || dt3.minute != 0 || dt3.second != 0 || dt3.fraction != 0) {
         logerror("Time fields of timestamp structure are not zero.");
       }
       connection()->commit();
       return m_success;
    }
};

class Num2LongTest 
    : public SQLDBC_TestCase
{
public:
    Num2LongTest()
    :SQLDBC_TestCase("Num2LongTest")
    {}

    virtual SQLDBC_Bool run()
    {
        dropTable("NUM2LONG");
        execSQL("CREATE TABLE NUM2LONG(A INTEGER, B LONG ASCII)");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO NUM2LONG VALUES(?, ?)");
        SQLDBC_Int4 index = 1;
        SQLDBC_Length index_length = sizeof(SQLDBC_Int4);
        SQLDBC_Length value_length;

        double value_double = 17.123;
        value_length = sizeof(double);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &index, &index_length, sizeof(SQLDBC_Int4));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_DOUBLE, &value_double, &value_length, sizeof(double));
        tc_errorcheck(ps)->execute();
        connection()->commit();

        tc_errorcheck(ps)->clearParameters();
        tc_errorcheck(ps)->prepare("SELECT B INTO ? FROM NUM2LONG WHERE A = 1");
        double read_double;
        SQLDBC_Length read_length;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DOUBLE, &read_double, &read_length, sizeof(double));
        tc_errorcheck(ps)->execute();
        if(read_double != value_double) {
            logerror("Inconsitent coversion %f read %f written", read_double, value_double);
        }
        return m_success;    
    }
};


class UTF8ByteTest 
    : public SQLDBC_TestCase
{
public:
    UTF8ByteTest()
    :SQLDBC_TestCase("UTF8ByteTest")
    {}

    virtual SQLDBC_Bool run()
    {
        dropTable("UTF8BYTE");
        execSQL("CREATE TABLE UTF8BYTE(A CHAR (30) BYTE, B VARCHAR (30) BYTE, C LONG BYTE)");
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        const char *str = "test123הצü";
        char utf8str[200]; 
        SQLDBC_Length utf8str_ind = 13;
        convertToUTF8(utf8str, 200, (char *)str);
        ps->prepare("INSERT INTO UTF8BYTE VALUES (?, ?, ?)");
        ps->bindParameter(1, SQLDBC_HOSTTYPE_UTF8, utf8str, &utf8str_ind, 200, true);
        ps->bindParameter(2, SQLDBC_HOSTTYPE_UTF8, utf8str, &utf8str_ind, 200, true);
        ps->bindParameter(3, SQLDBC_HOSTTYPE_UTF8, utf8str, &utf8str_ind, 200, true);
        tc_errorcheck(ps)->execute();
        
        tc_errorcheck(ps)->prepare("SELECT * FROM UTF8BYTE");
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs = tc_errorcheck(ps)->getResultSet();
        tc_errorcheck(rs)->next();
        
        char utf8result[200];
        SQLDBC_Length utf8result_ind = 0;
        memset(utf8result, 0, sizeof(utf8result));
        tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_UTF8, utf8result, &utf8result_ind, 200, false);
        if(m_success) {
            if(memcmp(utf8str, utf8result, 13) != 0) {
                logerror("WRONG VALUE SELECTED FOR CHAR BYTE");
            }
        }
        if(!m_success) { return m_success; }
        memset(utf8result, 0, sizeof(utf8result));
        tc_errorcheck(rs)->getObject(2, SQLDBC_HOSTTYPE_UTF8, utf8result, &utf8result_ind, 200, false);
        if(m_success) {
            if(memcmp(utf8str, utf8result, 13) != 0) {
                logerror("WRONG VALUE SELECTED FOR VARCHAR BYTE");
            }
        }

        if(!m_success) { return m_success; }
        memset(utf8result, 0, sizeof(utf8result));
        tc_errorcheck(rs)->getObject(3, SQLDBC_HOSTTYPE_UTF8, utf8result, &utf8result_ind, 200);
        if(m_success) {
            if(memcmp(utf8str, utf8result, 13) != 0) {
                logerror("WRONG VALUE SELECTED FOR LONG BYTE");
            }
        }
        return m_success;
    }
};


class FloatLimits
    : public SQLDBC_TestCase
{
public:
    FloatLimits()
    :SQLDBC_TestCase("FloatLimits")
    {}

    virtual SQLDBC_Bool run()
    {
        dropTable("FLOATLIMITS");
        execSQL("CREATE TABLE FLOATLIMITS(A INTEGER PRIMARY KEY, B FLOAT(38))");
        execSQL("INSERT INTO FLOATLIMITS VALUES(1, -1e-64)");
        execSQL("INSERT INTO FLOATLIMITS VALUES(2, 1e-64)");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO FLOATLIMITS VALUES(?, ?)");
        
        SQLDBC_Length a_length = sizeof(SQLDBC_Int4);
        SQLDBC_Int4   a = 3;
        char         *b_str= (char *) "-1e-64";
        SQLDBC_Length b_length = SQLDBC_NTS;
        
        ps->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &a, &a_length, sizeof(int));
        ps->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, b_str, &b_length, 10);
        tc_errorcheck(ps)->execute();
        a=4;
        b_str = (char *)"1e-64";
        ps->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, b_str, &b_length, 10);
        tc_errorcheck(ps)->execute();
        
        a = 5;
        double b_double = -1e-64;
        b_length = sizeof(double);
        ps->bindParameter(2, SQLDBC_HOSTTYPE_DOUBLE, &b_double,&b_length, sizeof(double));
        tc_errorcheck(ps)->execute();
        a = 6;
        b_double = 1e-64;
        tc_errorcheck(ps)->execute();
        connection()->commit();

        if(m_success) {
            tc_errorcheck(ps)->prepare("SELECT B FROM FLOATLIMITS");
            char buffer[32];
            tc_errorcheck(ps)->execute();
            SQLDBC_ResultSet *rs = tc_errorcheck(ps)->getResultSet();
            while(tc_errorcheck(rs)->next() == SQLDBC_OK) {
                SQLDBC_Length l=0;
                rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, buffer, &l, sizeof(buffer), true);
                if( ! ( (strcmp(buffer, "-1E-64")==0) || (strcmp(buffer, "1E-64")==0) ) ) {
                    logerror("Invalid value found %s\n", buffer);
                    break;
                }
            }
        }

        return m_success;
    }
};

class FloatTrunc
    : public SQLDBC_TestCase
{
public:
    FloatTrunc()
    :SQLDBC_TestCase("FloatTrunc")
    {}
    
    virtual SQLDBC_Bool run()
    {
        double testnum = 5.5555;
        dropTable("FLOATTRUNC");
        execSQL("CREATE TABLE FLOATTRUNC(A1 INTEGER, A2 SMALLINT, A3 FIXED(1), A4 FIXED(2,1), A5 FIXED(3,2), A6 FIXED(10, 3), A7 FLOAT(1), A8 FLOAT(2), A9 FLOAT(3))");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO FLOATTRUNC VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(4, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(5, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(6, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(7, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(8, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->bindParameter(9, SQLDBC_HOSTTYPE_DOUBLE, &testnum, 0, sizeof(double));
        tc_errorcheck(ps)->execute();
        testnum = -5.5555;
        tc_errorcheck(ps)->execute();
        connection()->commit();
        return m_success;
    }
};


//----------------------------------------------------------------------
int main(int argc, char **argv)
{
    // Initalize from command line params
    SQLDBC_TestFrame testframe(argc, argv);
    testframe.addTestCase(new FloatLimits());
    testframe.addTestCase(new CharAsciiTest());
    testframe.addTestCase(new Byte1Test());
    testframe.addTestCase(new CharUnicodeTest());
    testframe.addTestCase(new NumericTest());
    testframe.addTestCase(new TimeCharTest());
    testframe.addTestCase(new TimeTimeTest());
    testframe.addTestCase(new DateCharTest());
    testframe.addTestCase(new DateDateTest());
    testframe.addTestCase(new FloatFloatTest());
    testframe.addTestCase(new FloatNANTest());
    testframe.addTestCase(new NumericCharTest());
    testframe.addTestCase(new UCS2CharTest());
    testframe.addTestCase(new SQLNumericTest());
    testframe.addTestCase(new DecimalTest());
    testframe.addTestCase(new TimestampTimeTest());
    testframe.addTestCase(new TimestampDateTest());
    testframe.addTestCase(new Num2LongTest());
    testframe.addTestCase(new UTF8ByteTest());
//    testframe.addTestCase(new FloatTrunc()); // requires special connect option
    if(testframe.run()) return 0;
    return 1;
}

