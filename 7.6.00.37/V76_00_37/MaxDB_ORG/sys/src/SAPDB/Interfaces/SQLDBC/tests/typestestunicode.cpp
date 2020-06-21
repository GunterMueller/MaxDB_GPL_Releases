/*!================================================================
 module:    typestestunicode.cpp

 responsible: D03109

 special area: Type conversion using unicode commands
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

//----------------------------------------------------------------------

class UnicodeTest : public SQLDBC_TestCase
{
public:
    UnicodeTest(const char *test = "UnicodeTest")
    :SQLDBC_TestCase(test)
    {}

    SQLDBC_Bool run()
    {
        m_success=true;
        char cmd[3000]; 
        char tablename[] = {(char)0xE8, 
				                      (char)0xBD, 
				                      (char)0xAF, 
				                      (char)0xE4, 
				                      (char)0xBB, 
				                      (char)0xB6, 
				                      (char)0xE5, 
				                      (char)0xBC, 
				                      (char)0x80, 
				                      (char)0xE5, 
				                      (char)0x8F, 
				                      (char)0x91, 
				                      (char)0xE5, 
				                      (char)0x95, 
				                      (char)0x86, 
				                      (char)0xE6, 
				                      (char)0x98, 
				                      (char)0xAF, 
				                      (char)0xE8, 
				                      (char)0x8B, 
				                      (char)0xB1, 
				                      (char)0xE9, 
				                      (char)0x9B, 
				                      (char)0x84};

        sprintf (cmd, "DROP TABLE %.*s", sizeof(tablename), tablename);
         SQLDBC_Retcode rc = statement()->execute(cmd, SQLDBC_StringEncodingUTF8);
        
        sprintf (cmd, "CREATE TABLE %.*s (" \
											"C_CHARASCII          Char (10) ASCII," \
											"C_CHARBYTE           Char (10) BYTE," \
											"C_CHARUNICODE        Char (10) UNICODE," \
											"C_VARCHARASCII       Varchar (10) ASCII,"\
											"C_VARCHARBYTE        Varchar (10) BYTE,"\
											"C_VARCHARUNICODE     Varchar (10) UNICODE,"\
											"C_LONGASCII          Long ASCII,"\
											"C_LONGBYTE           Long BYTE"\
											",C_LONGUNICODE        Long UNICODE"\ 
											",C_LONGUNICODE_LONG   Long UNICODE"\ 
											")"	
								, sizeof(tablename), tablename);
                
        rc = statement()->execute(cmd, SQLDBC_StringEncodingUTF8);
        if(rc != SQLDBC_OK) {
            if(statement()->error().getErrorCode()==-3002 
              || statement()->error().getErrorCode()==-10602
              ) {
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
         
				char stringutf8 [] = {(char)0xE8, 
				                      (char)0xBD, 
				                      (char)0xAF, 
				                      (char)0xE4, 
				                      (char)0xBB, 
				                      (char)0xB6, 
				                      (char)0xE5, 
				                      (char)0xBC, 
				                      (char)0x80, 
				                      (char)0xE5, 
				                      (char)0x8F, 
				                      (char)0x91, 
				                      (char)0xE5, 
				                      (char)0x95, 
				                      (char)0x86, 
				                      (char)0xE6, 
				                      (char)0x98, 
				                      (char)0xAF, 
				                      (char)0xE8, 
				                      (char)0x8B, 
				                      (char)0xB1, 
				                      (char)0xE9, 
				                      (char)0x9B, 
				                      (char)0x84};
				char stringAscii[] = {'ä','ö','ü'} ;
				char stringByte [] = {(char)0xFF, (char)0xFE, (char)0xFD, (char)0xFC};
				char stringutf8Long [120000];
				char longval[] = {'a','b','c'} ;
        
        for (int k=0; k<sizeof(stringutf8Long); k++){
          stringutf8Long[k]=stringutf8[k%(sizeof(stringutf8))];	
        }


        SQLDBC_Length indicator[10];
        indicator[0] = sizeof (stringAscii);
        indicator[1] = sizeof (stringByte);
        indicator[2] = sizeof (stringutf8);
        
        indicator[3] = sizeof (stringAscii);
        indicator[4] = sizeof (stringByte);
        indicator[5] = sizeof (stringutf8);
        
        indicator[6] = sizeof (stringAscii);
        indicator[7] = sizeof (stringByte);
        indicator[8] = sizeof (stringutf8);
        indicator[9] = sizeof (stringutf8Long);

        
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Bool memory_ok=true;
        sprintf (cmd, "INSERT INTO %.*s VALUES (?,?,?,?,?,?,?,?,?,?)"
								, sizeof(tablename), tablename);
        ps->prepare(cmd, SQLDBC_StringEncodingUTF8);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, stringAscii, &indicator[0], sizeof(stringAscii));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_BINARY, stringByte, &indicator[1], sizeof(stringByte));
        tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_UTF8, stringutf8,   &indicator[2], sizeof(stringutf8));

        tc_errorcheck(ps)->bindParameter(4, SQLDBC_HOSTTYPE_ASCII, stringAscii, &indicator[3], sizeof(stringAscii));
        tc_errorcheck(ps)->bindParameter(5, SQLDBC_HOSTTYPE_BINARY, stringByte, &indicator[4], sizeof(stringByte));
        tc_errorcheck(ps)->bindParameter(6, SQLDBC_HOSTTYPE_UTF8, stringutf8,   &indicator[5], sizeof(stringutf8));

        tc_errorcheck(ps)->bindParameter(7, SQLDBC_HOSTTYPE_ASCII, stringAscii, &indicator[6], sizeof(stringAscii));
        tc_errorcheck(ps)->bindParameter(8, SQLDBC_HOSTTYPE_BINARY, stringByte, &indicator[7], sizeof(stringByte));
        tc_errorcheck(ps)->bindParameter(9, SQLDBC_HOSTTYPE_UTF8, stringutf8,   &indicator[8], sizeof(stringutf8));
        tc_errorcheck(ps)->bindParameter(10, SQLDBC_HOSTTYPE_UTF8, stringutf8Long,   &indicator[9], sizeof(stringutf8Long));
                
        tc_errorcheck(ps)->execute();

        tc_errorcheck(connection())->commit();


        SQLDBC_PreparedStatement *ps2=tc_errorcheck(connection())->createPreparedStatement();
				
				char stringutf8_out1  [60]; 
				char stringutf8_out2  [sizeof(stringutf8)]; 
				char stringutf8_out3  [sizeof(stringutf8)]; 
        char stringutf8_out4  [sizeof(stringutf8Long)]; 

				char stringAscii_out1  [10]; 
				char stringAscii_out2  [sizeof(stringAscii)]; 
				char stringAscii_out3  [sizeof(stringAscii)]; 

				char stringByte_out1  [10]; 
				char stringByte_out2  [sizeof(stringByte)]; 
				char stringByte_out3  [sizeof(stringByte)]; 
				
				SQLDBC_Length lengthindicator = 0;
        sprintf (cmd, "SELECT * INTO ?,?,?,?,?,?,?,?,?,? FROM  %.*s"
								, sizeof(tablename), tablename);
        tc_errorcheck(ps2)->prepare(cmd, SQLDBC_StringEncodingUTF8);

        tc_errorcheck(ps2)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII,  stringAscii_out1, &indicator[0], sizeof(stringAscii_out1), false);
        tc_errorcheck(ps2)->bindParameter(2, SQLDBC_HOSTTYPE_BINARY, stringByte_out1,  &indicator[1], sizeof(stringByte_out1), false);
        tc_errorcheck(ps2)->bindParameter(3, SQLDBC_HOSTTYPE_UTF8,   stringutf8_out1,  &indicator[2], sizeof(stringutf8_out1), false);

        tc_errorcheck(ps2)->bindParameter(4, SQLDBC_HOSTTYPE_ASCII,  stringAscii_out2, &indicator[3], sizeof(stringAscii_out2), false);
        tc_errorcheck(ps2)->bindParameter(5, SQLDBC_HOSTTYPE_BINARY, stringByte_out2,  &indicator[4], sizeof(stringByte_out2), false);
        tc_errorcheck(ps2)->bindParameter(6, SQLDBC_HOSTTYPE_UTF8,   stringutf8_out2,  &indicator[5], sizeof(stringutf8_out2), false);

        tc_errorcheck(ps2)->bindParameter(7, SQLDBC_HOSTTYPE_ASCII,  stringAscii_out3, &indicator[6], sizeof(stringAscii_out3), false);
        tc_errorcheck(ps2)->bindParameter(8, SQLDBC_HOSTTYPE_BINARY, stringByte_out3,  &indicator[7], sizeof(stringByte_out3), false);
        tc_errorcheck(ps2)->bindParameter(9, SQLDBC_HOSTTYPE_UTF8,   stringutf8_out3,  &indicator[8], sizeof(stringutf8_out3), false);
        tc_errorcheck(ps2)->bindParameter(10,SQLDBC_HOSTTYPE_UTF8,   stringutf8_out4,  &indicator[9], sizeof(stringutf8_out4), false);

        tc_errorcheck(ps2)->execute();
        connection()->releaseStatement(ps2);

        if(memcmp(stringAscii_out1, stringAscii, sizeof(stringAscii))!=0) {
            logerror("Data for column \"C_CHARASCII\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[0] 
            ,stringAscii_out1
            ,sizeof(stringAscii)
            ,stringAscii);
        }
        if(memcmp(stringByte_out1, stringByte, sizeof(stringByte))!=0) {
            logerror("Data for column \"C_CHARBYTE\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[1] 
            ,stringByte_out1
            ,sizeof(stringByte)
            ,stringByte);
        }
        if(memcmp(stringutf8_out1, stringutf8, sizeof(stringutf8))!=0) {
            logerror("Data for column \"C_CHARUNICODE\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[2] 
            ,stringutf8_out1
            ,sizeof(stringutf8)
            ,stringutf8);
        }

        if(memcmp(stringAscii_out2, stringAscii, sizeof(stringAscii))!=0) {
            logerror("Data for column \"C_VARCHARASCII\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[3] 
            ,stringAscii_out2
            ,sizeof(stringAscii)
            ,stringAscii);
        }
        if(memcmp(stringByte_out2, stringByte, sizeof(stringByte))!=0) {
            logerror("Data for column \"C_VARCHARBYTE\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[4] 
            ,stringByte_out2
            ,sizeof(stringByte)
            ,stringByte);
        }
        if(memcmp(stringutf8_out2, stringutf8, sizeof(stringutf8))!=0) {
            logerror("Data for column \"C_VARCHARUNICODE\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[5] 
            ,stringutf8_out2
            ,sizeof(stringutf8)
            ,stringutf8);
        }


        if(memcmp(stringAscii_out3, stringAscii, sizeof(stringAscii))!=0) {
            logerror("Data for column \"C_LONGASCII\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[6] 
            ,stringAscii_out3
            ,sizeof(stringAscii)
            ,stringAscii);
        }
        if(memcmp(stringByte_out3, stringByte, sizeof(stringByte))!=0) {
            logerror("Data for column \"C_LONGBYTE\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[7] 
            ,stringByte_out3
            ,sizeof(stringByte)
            ,stringByte);
        }
        if(memcmp(stringutf8_out3, stringutf8, sizeof(stringutf8))!=0) {
            logerror("Data for column \"C_LONGUNICODE\" differs from expected value. Found %.*s - Expected %.*s"
            ,indicator[8] 
            ,stringutf8_out3
            ,sizeof(stringutf8)
            ,stringutf8);
        }
        
        for (int l=0; l<sizeof(stringutf8_out4); l++){
        	if (stringutf8_out4[l]!=stringutf8[l%(sizeof(stringutf8))]){
            logerror("Data for column \"C_LONGUNICODE_LONG\" differs from expected value at position %d. Found at pos %d %.*s[%c]%.*s - Expected %c"
            ,l
            ,l
            ,10
            ,&stringutf8_out4[l-10]
            ,stringutf8_out4[l]
            ,10
            ,&stringutf8_out4[l+1]
            ,stringutf8[l%(sizeof(stringutf8))]);
            break;
        	}	
        }



        return m_success;
    }

};



//----------------------------------------------------------------------
   int main(int argc, char **argv)
   {
       // Initalize from command line params
       SQLDBC_TestFrame testframe(argc, argv);
       testframe.addTestCase(new UnicodeTest());
       if(testframe.run()) return 0;
       return 1;
   }

