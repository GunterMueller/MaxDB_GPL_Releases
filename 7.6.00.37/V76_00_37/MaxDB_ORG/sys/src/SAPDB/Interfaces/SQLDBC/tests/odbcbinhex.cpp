/*!
  @file           odbcbinhex.cpp
  @author         D039759
  @ingroup        Tests
  @keywords       ODBC HEX BIN

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
#include <string.h>
#include <stdlib.h>

class Hex2Bin : public SQLDBC_TestCase
{

public:
	Hex2Bin ()
  : SQLDBC_TestCase ("Hex2Bin")
	{}

    
    virtual SQLDBC_Bool run ()
    {
        dropTable("SQLDBC_BINHEX");
        execSQL("CREATE TABLE SQLDBC_BINHEX(V1 CHAR(20) BYTE, V2 VARCHAR(20) BYTE, V3 LONG BYTE)");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO SQLDBC_BINHEX (V1, V2, V3) VALUES(?, ?, ?)");
        char *buffer="4142434445464748494A";
        char *bufferfull="4142434445464748494A00000000000000000000";
        char *rbuffer="ABCDEFGHIJ";
        char rbufcmp[20];
        memset(rbufcmp, 0, 20);
        strcpy(rbufcmp, rbuffer);

        SQLDBC_Length ind1=SQLDBC_NTS;
        SQLDBC_Length ind2 = SQLDBC_NTS;
        SQLDBC_Length ind3 = SQLDBC_NTS;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, buffer, &ind1, 20, true);
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, buffer, &ind2, 20, true);
        tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, buffer, &ind3, 20, true);
        tc_errorcheck(ps)->execute();

        char buf1[40];
        memset(buf1, 'X', 40);
        char buf2[40];
        memset(buf2, 'X', 40);
        char buf3[40];
        memset(buf3, 'X', 40);
        SQLDBC_Length buf1_ind = 0;
        SQLDBC_Length buf2_ind = 0;
        SQLDBC_Length buf3_ind = 0;
        
        tc_errorcheck(ps)->prepare("SELECT V1, V2, V3 FROM SQLDBC_BINHEX");
        tc_errorcheck(ps)->execute();

        SQLDBC_ResultSet *rs=ps->getResultSet();
        tc_errorcheck(rs)->next();
        
        // v1
        SQLDBC_RowSet *rws = rs->getRowSet();
        tc_errorcheck(rws)->getObject(1, SQLDBC_HOSTTYPE_BINARY, buf1, &buf1_ind, 20, false);
        if(memcmp(buf1, rbufcmp, 20) != 0) {
            logerror("Output buffers differ for CHAR BYTE");
        }
        if(buf1_ind != 20) {
            logerror("Wrong indicator for CHAR BYTE");
        }

        // VARCHAR BYTE TRIMS '\0' BYTES
        tc_errorcheck(rws)->getObject(2, SQLDBC_HOSTTYPE_BINARY, buf2, &buf2_ind, 20, false);
        if(memcmp(buf2, rbufcmp, 10) != 0) {
            logerror("Output buffers differ for VARCHAR BYTE");
        }
        if(buf2_ind != 10) {
            logerror("Wrong indicator for VARCHAR BYTE");
        }

        tc_errorcheck(rws)->getObject(3, SQLDBC_HOSTTYPE_BINARY, buf3, &buf3_ind, 20, false);
        if(memcmp(buf2, rbufcmp, 10) != 0) {
            logerror("Output buffers differ for LONG BYTE");
        }
        if(buf2_ind != 10) {
            logerror("Wrong indicator for LONG BYTE");
        }

        memset(buf1, 'X', 40);
        memset(buf2, 'X', 40);
        memset(buf3, 'X', 40);
        
        tc_errorcheck(rws)->getObject(1, SQLDBC_HOSTTYPE_ASCII, buf1, &buf1_ind, 40, false);
        if(memcmp(buf1, bufferfull, 40) != 0) {
            logerror("HEX buffers differ for CHAR BYTE");
        }
        if(buf1_ind != 40) {
            logerror("Invalid indicator value for CHAR BYTE");
        }

        tc_errorcheck(rws)->getObject(2, SQLDBC_HOSTTYPE_ASCII, buf2, &buf2_ind, 40, false);
        if(memcmp(buf2, buffer, 20) != 0) {
            logerror("HEX buffers differ for VARCHAR BYTE");
        }
        if(buf2_ind != 20) {
            logerror("Invalid indicator value for VARCHAR BYTE");
        }
        
        tc_errorcheck(rws)->getObject(3, SQLDBC_HOSTTYPE_ASCII, buf3, &buf3_ind, 40, false);
        if(memcmp(buf3, buffer, 20) != 0) {
            logerror("HEX buffers differ for LONG BYTE (as ASCII)");
        }
        if(buf3_ind != 20) {
            logerror("Invalid indicator value for LONG BYTE (as ASCII)");
        }
        
        tc_expectederrorcheck(rws, -10404)->getObject(1, SQLDBC_HOSTTYPE_INT4, buf1, &buf1_ind, 0, false);  
        tc_expectederrorcheck(rws, -10404)->getObject(2, SQLDBC_HOSTTYPE_INT4, buf2, &buf2_ind, 0, false);  
        tc_expectederrorcheck(rws, -10404)->getObject(3, SQLDBC_HOSTTYPE_INT4, buf3, &buf3_ind, 0, false);   

        tc_expectederrorcheck(rws, -10404)->getObject(1, SQLDBC_HOSTTYPE_DOUBLE, buf1, &buf1_ind, 0, false);  
        tc_expectederrorcheck(rws, -10404)->getObject(2, SQLDBC_HOSTTYPE_DOUBLE, buf2, &buf2_ind, 0, false);  
        tc_expectederrorcheck(rws, -10404)->getObject(3, SQLDBC_HOSTTYPE_DOUBLE, buf3, &buf3_ind, 0, false);   

        tc_expectederrorcheck(rws, -10404)->getObject(1, SQLDBC_HOSTTYPE_ODBCNUMERIC, buf1, &buf1_ind, 0, false);  
        tc_expectederrorcheck(rws, -10404)->getObject(2, SQLDBC_HOSTTYPE_ODBCNUMERIC, buf2, &buf2_ind, 0, false);  
        tc_expectederrorcheck(rws, -10404)->getObject(3, SQLDBC_HOSTTYPE_ODBCNUMERIC, buf3, &buf3_ind, 0, false);   

        connection()->releaseStatement(ps);
        connection()->commit();
        return m_success;
    }
};

int main (int argc, char **argv)
{
    static char *argv_copy[1024];
    int i;
    for(i=0; i<argc; ++i) {
        argv_copy[i] = argv[i];
    }
    char *s = strdup("ODBC_BIN2HEX=1");
    
    argv_copy[i++] = (char *) "-p";
    argv_copy[i++] = (char *) s;
    
    // Initalize from command line params
    SQLDBC_TestFrame testframe (argc + 2, argv_copy);

    testframe.addTestCase (new Hex2Bin());
    
    if (testframe.run ()) {
        free(s);
        return 0;
    }
    
    free(s);
    return 1;
}


