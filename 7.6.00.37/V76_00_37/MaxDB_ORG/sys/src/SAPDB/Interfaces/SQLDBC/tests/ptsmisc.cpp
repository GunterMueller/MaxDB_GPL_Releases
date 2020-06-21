/*!
  @file           ptsmisc.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Miscellanous tests, most from errors reported in SQLDBC.
  

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

// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127276
class PTS1127276
    : public SQLDBC_TestCase
{
public:
    PTS1127276()
    : SQLDBC_TestCase("PTS1127276")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1127276");
        execSQL("CREATE TABLE PTS1127276(V NUMBER(15))");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1127276 VALUES (?)");
        SQLDBC_Length ind=SQLDBC_NTS;
        const char *data="-123456789012345";
        tc_errorcheck(ps)->bindParameter(1,SQLDBC_HOSTTYPE_ASCII, (void *)data, &ind, 17);
        tc_errorcheck(ps)->execute();
        
        char          result[256];
        SQLDBC_Length result_length;
        tc_errorcheck(ps)->prepare("SELECT * FROM PTS1127276");
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs=tc_errorcheck(ps)->getResultSet();
        rs->next();
        rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, result, &result_length, sizeof(result));
        if(strcmp(result, data) != 0) {
            logerror("Inserted %s, found %s", data, result);
        }
        connection()->releaseStatement(ps);
        connection()->commit();
        return m_success;
    }
};

// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127270
class PTS1127270
    : public SQLDBC_TestCase
{
public:
    PTS1127270()
    : SQLDBC_TestCase("PTS1127270")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1127270");
        execSQL("CREATE TABLE PTS1127270(V NUMBER(15))");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1127270 VALUES (?)");
        SQLDBC_Length ind=SQLDBC_NTS;
        const char *data="+000000000000000";
        tc_errorcheck(ps)->bindParameter(1,SQLDBC_HOSTTYPE_ASCII, (void *)data, &ind, 17);
        tc_errorcheck(ps)->execute();
        
        char          result[256];
        SQLDBC_Length result_length;
        tc_errorcheck(ps)->prepare("SELECT * FROM PTS1127270");
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs=tc_errorcheck(ps)->getResultSet();
        rs->next();
        rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, result, &result_length, sizeof(result));
        if(strcmp(result, "0") != 0) {
            logerror("Inserted %s, found %s", data, result);
        }
        connection()->releaseStatement(ps);
        connection()->commit();
        return m_success;
    }
};

// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127269
class PTS1127269
    : public SQLDBC_TestCase
{
public:
    PTS1127269()
    : SQLDBC_TestCase("PTS1127269")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1127269");
        execSQL("CREATE TABLE PTS1127269(V NUMBER(15))");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1127269 VALUES (?)");
        SQLDBC_Length ind=SQLDBC_NTS;
        const char *data="borkborkbork";
        tc_errorcheck(ps)->bindParameter(1,SQLDBC_HOSTTYPE_ASCII, (void *)data, &ind, 17);
        SQLDBC_Retcode rc=tc_expectederrorcheck(ps, -10802)->execute();
        if(rc != SQLDBC_NOT_OK) {
            logerror("PreparedStatement->execute() delivered no error.");
        }
        connection()->releaseStatement(ps);
        connection()->commit();
        return m_success;
    }
};

//----------------------------------------------------------------------
class PTS1127266
    : public SQLDBC_TestCase
{
public:
    PTS1127266()
    : SQLDBC_TestCase("PTS1127266")
    {}

    SQLDBC_Bool run() 
    {
        dropTable("PTS1127266");
        execSQL("CREATE TABLE PTS1127266 (K VARCHAR(4) PRIMARY KEY, V NUMBER(15))");
        SQLDBC_PreparedStatement *ps = tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1127266 VALUES (?, ?)");
        char cmassdata[] = "AAAABBBBCCCCDDDDEEEEFFFFAAAAGGGGHHHH";
        SQLDBC_Length cind[] = { 4,4,4,4,4,4,4,4,4 };
        int  imassdata[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        ps->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, cmassdata, cind, 4, false);
        ps->bindParameter(2, SQLDBC_HOSTTYPE_INT4,  imassdata, 0, sizeof(int), false);
        ps->setBatchSize(1);
        ps->execute();
        execSQL("DELETE FROM PTS1127266");
        ps->setBatchSize(9);
        SQLDBC_Retcode rc=tc_expectederrorcheck(ps, -1)->execute();
        if(rc == SQLDBC_OK) {
            logerror("Got OK, expected error.");
        } else {
            loginfo("RETURN Code is %d", (SQLDBC_Int4)rc);
        }
        
        return m_success;
    }
};

class PTS1127267 
    : public SQLDBC_TestCase
{
public:
    PTS1127267()
    : SQLDBC_TestCase("PTS1127267")
    {}
    
    SQLDBC_Bool run() 
    {
        dropTable("PTS1127267");
        execSQL("CREATE TABLE PTS1127267 (K INTEGER PRIMARY KEY, V VARCHAR (3000))");
        connection()->setAutoCommit(false);
        checkInsert(10, 8); execSQL("DELETE FROM PTS1127267");
        checkInsert(100, 99); execSQL("DELETE FROM PTS1127267");
        checkInsert(1000, 707); execSQL("DELETE FROM PTS1127267");
        return m_success;
    }

    void checkInsert(int count, int errorpos) {
        if(!m_success) {
            return;
        }
        
        loginfo("CHECK FOR %d ROWS WITH PK VIOLATION AT %d", count, errorpos);
        
        SQLDBC_Int4 *imassdata=new SQLDBC_Int4[count];
        SQLDBC_Length *imassdata_ind=new SQLDBC_Length[count];
            SQLDBC_Length *cmassdata_ind=new SQLDBC_Length[count];
        for(int i=0; i<count; ++i) {
            imassdata[i]=i;
            imassdata_ind[i] = 4;
            cmassdata_ind[i] = 200;
        }
        imassdata[errorpos] = 0; // Key violation
        char *cmassdata=new char[200 * count];
        memset(cmassdata, 'A', 200 * count);

        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1127267 VALUES(?, ?)");
        ps->bindParameter(1, SQLDBC_HOSTTYPE_INT4,  imassdata, imassdata_ind, 4, false);
        ps->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, cmassdata, cmassdata_ind, 200, false);
        ps->setBatchSize(count);
        SQLDBC_Retcode rc=tc_expectederrorcheck(ps, -1)->execute();
        switch(rc) {
        case SQLDBC_NO_DATA_FOUND:
            if(errorpos == 0) {
                break;
            }
        case SQLDBC_SUCCESS_WITH_INFO:
            if(errorpos != 0) {
                break;
            }
        default:
            logerror("Unexpected return code during INSERT.");
        }
        const SQLDBC_Int4 *rs=ps->getRowStatus();
        int k;
        for(k=0; k<count; ++k) {
            if(rs[k]==SQLDBC_EXECUTE_FAILED) {
                break;
            }
            // loginfo("STATUS[%d] = %d", k, rs[k]);
        }
        if(k != errorpos) {
            logerror("EXPECTED ERROR AT POS %d", errorpos);
        }
        connection()->releaseStatement(ps);
        delete [] imassdata;
        delete [] cmassdata;
        delete [] imassdata_ind;
        delete [] cmassdata_ind;
            
    }
        
};


//----------------------------------------------------------------------
class PTS1127510
    : public SQLDBC_TestCase
{
public:
  PTS1127510()
    : SQLDBC_TestCase("PTS1127510")
  {}

  SQLDBC_Bool prepare() 
  {
    dropTable("PTS1127510");
    m_success |= execSQL("CREATE TABLE PTS1127510 (K VARCHAR(4) PRIMARY KEY, V NUMBER(15))");
    if (!m_success) 
      return m_success;
    SQLDBC_PreparedStatement *ps = tc_errorcheck(connection())->createPreparedStatement();
    tc_errorcheck(ps)->prepare("INSERT INTO PTS1127510 VALUES (?, ?)");
    char cmassdata[] = "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIII";
    SQLDBC_Length cind[] = { 4,4,4,4,4,4,4,4,4 };
    int  imassdata[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, cmassdata, cind, 4, false);
    tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_INT4,  imassdata, 0, sizeof(int), false);
    tc_errorcheck(ps)->setBatchSize(sizeof(imassdata)/sizeof(imassdata[0]));
    tc_errorcheck(ps)->execute();
    tc_errorcheck(connection())->releaseStatement(ps);
    return m_success;
  }
  
  SQLDBC_Bool run() 
  {
    SQLDBC_PreparedStatement *ps = tc_errorcheck(connection())->createPreparedStatement();
    tc_errorcheck(ps)->prepare("UPDATE PTS1127510 set k = ? where v = ?");
    SQLDBC_Length cind[] = { 99, 99, 99, 99, 99, 99, 99, 99, 99 };
    char cmassdata[] = "aaaabbbbccccdddd";
    int  imassdata[] = { 10, 11, 12, 13 };
    int count = sizeof(imassdata)/sizeof(imassdata[0]);
    tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, cmassdata, cind, 4, false);
    tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_INT4,  imassdata, 0, sizeof(int), false);
    tc_errorcheck(ps)->setBatchSize(count);
    SQLDBC_Retcode rc=tc_errorcheck(ps)->execute();
    if(rc != SQLDBC_NO_DATA_FOUND) {
      logerror("Returncode should SQLDBC_NO_DATA_FOUND. Returned error was %d.", (SQLDBC_Int4)rc);
    } else {
      const SQLDBC_Int4 rows = tc_errorcheck(ps)->getRowsAffected();
      if (rows != 0) {
        logerror("Wrong number of rows affected. No rows should be modified." );        
      }
      const SQLDBC_Int4 *rs=ps->getRowStatus();
      int k;
      for(k=0; k < count; ++k) {
        if(rs[k] != 0) {
          logerror("Wrong row status rs[%d]=%d returned. Should return 0", k, rs[k] );
          break;
        }
      }
    }
    
    return m_success;
  }
};

//----------------------------------------------------------------------
// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129091
class PTS1129091
    : public SQLDBC_TestCase
{
public:
    PTS1129091()
    : SQLDBC_TestCase("PTS1129091")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1129091");
        execSQL("CREATE TABLE PTS1129091(V LONG ASCII)");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1129091 VALUES (?)");
        SQLDBC_Length ind=SQLDBC_NULL_DATA;
        const char *data=0;
        tc_errorcheck(ps)->bindParameter(1,SQLDBC_HOSTTYPE_ASCII, 0, &ind, 0);
        tc_errorcheck(ps)->execute();
        return m_success;
    }
};


// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129646
class PTS1129646
    : public SQLDBC_TestCase
{
public:
    PTS1129646()
    :SQLDBC_TestCase("PTS1129646")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1129646");
        execSQL("CREATE TABLE PTS1129646 (K INTEGER PRIMARY KEY, V LONG ASCII)");
        execSQL("INSERT INTO PTS1129646 VALUES(1, 'Homer, Marge, Bart, Maggie, Lisa, Knecht Ruprecht')");
        execSQL("INSERT INTO PTS1129646 VALUES(2, 'Fred, Wilma, Pebbles, Dino')");
        
        char dummybuffer[200];
        strcpy(dummybuffer, "NOT FILLED");
        SQLDBC_Length dummybuffer_length = 0;
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        ps->prepare("SELECT V INTO ? FROM PTS1129646 WHERE K = ?");
        SQLDBC_Int4 index=1;
        SQLDBC_Length index_length=sizeof(SQLDBC_Int4);
        ps->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, dummybuffer, &dummybuffer_length, 0);
        ps->bindParameter(2, SQLDBC_HOSTTYPE_INT4, &index, &index_length, sizeof(SQLDBC_Int4));
        ps->execute();
        if(strcmp(dummybuffer, "NOT FILLED")) {
            logerror("buffer was %s, expected was an untouched buffer.", dummybuffer);
            return failure();
        }
        char buffer[200];
        memset(buffer, 'A', 199);
        buffer[199]='\0';
        SQLDBC_Length bufferlength;
        ps->getObject(1, SQLDBC_HOSTTYPE_ASCII, buffer, &bufferlength, 200, true);
        if(strcmp(buffer, "Homer, Marge, Bart, Maggie, Lisa, Knecht Ruprecht")) {
            logerror("BUFFER CONTENT IS %s", buffer);
            return failure();
        }
        memset(buffer, 'A', 199);
        buffer[199]='\0';
        index = 2;
        ps->execute();
        ps->getObject(1, SQLDBC_HOSTTYPE_ASCII, buffer, &bufferlength, 200, true);
        if(strcmp(buffer, "Fred, Wilma, Pebbles, Dino")) {
            logerror("BUFFER CONTENT IS %s", buffer);
            return failure();
        }
        return success();
    }
};

//----------------------------------------------------------------------
// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129648
// Not a real test, it only shows the use of block cursors with precompiler 
// semantics.
class PTS1129648
    : public SQLDBC_TestCase
{
public:
    PTS1129648()
    :SQLDBC_TestCase("PTS1129648")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1129648");
        execSQL("CREATE TABLE PTS1129648(K INTEGER PRIMARY KEY, B CHAR (300) ASCII)");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        
        ps->prepare("INSERT INTO PTS1129648 VALUES(?, ?)");
        SQLDBC_Int4 kval;
        char bval[300];
        ps->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &kval, 0, sizeof(SQLDBC_Int4));
        ps->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, bval, 0, 300, false);
        for(kval = 1; kval <= 64; ++kval) {
            char c = 'A' + ((kval % 26) -1);
            memset(bval, c, 300);
            ps->execute();
        }
        
        ps->prepare("SELECT K, B FROM PTS1129648 ORDER BY K ASC");
        ps->execute();
        SQLDBC_ResultSet *rs=ps->getResultSet();
        
        char *bvalres;
        bvalres = new char[300 * 63];
        int kvalres[63];
        SQLDBC_Length bvalres_ind[63];
        SQLDBC_Length kvalres_ind[63];
        rs->bindColumn(1, SQLDBC_HOSTTYPE_INT4, kvalres, kvalres_ind, sizeof(SQLDBC_Int4), false);
        rs->bindColumn(2, SQLDBC_HOSTTYPE_ASCII, bvalres, bvalres_ind, 300, false);
        rs->setRowSetSize(63);
        rs->setFetchSize(63);
        rs->next();
        rs->getRowSet()->fetch();
        rs->getRowSet()->getRowsAffected();
        rs->relative(rs->getRowSetSize());
        rs->setRowSetSize(27);
        rs->getRowSet()->fetch();
        rs->getRowSet()->getRowsAffected();
        connection()->releaseStatement(ps);
        return success();
    }
};

class PTS1130036
    : public SQLDBC_TestCase
{
public:
    PTS1130036()
    :SQLDBC_TestCase("PTS1130036")
    {}
    
    SQLDBC_Bool run()
    {
        dropTable("PTS1130036");
        execSQL("CREATE TABLE PTS1130036(A VARCHAR(20) PRIMARY KEY)");
        SQLDBC_PreparedStatement *ps = tc_errorcheck(connection())->createPreparedStatement();
        unsigned short ucs2[100];
        convertToUCS2(ucs2, sizeof(ucs2), false, "INSERT INTO PTS1130036 VALUES('BORKBORKBORK')");
        tc_errorcheck(ps)->prepare((char*)ucs2, SQLDBC_StringEncodingUCS2);
        tc_errorcheck(ps)->execute();
        tc_expectederrorcheck(ps, -1)->execute();
        connection()->releaseStatement(ps);
        return m_success;
    }
};

// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130048
class PTS1130048
    : public SQLDBC_TestCase
{
public:
    PTS1130048()
    :SQLDBC_TestCase("PTS1130048")
    {}
    
    SQLDBC_Bool run()
    {
        dropTable("PTS1130048");
        execSQL("CREATE TABLE PTS1130048(A CHAR(20))");
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1130048 VALUES (?)");
        SQLDBC_Length ind= SQLDBC_NTS;
        ps->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, (void *)"1234567890123456789012345", &ind, 35, false);
        ps->execute();
        return m_success;
    }

};

// Was in a CSN message, unrelated to SQLDBC.
class XXX
    : public SQLDBC_TestCase
{
public:
    XXX()
    :SQLDBC_TestCase("XXX")
    {}
    
    SQLDBC_Bool run()
    {
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        tc_errorcheck(ps)
          ->prepare( "SELECT                                                                  "
                     "DISTINCT T_00.\"PARTNER\",                                              "
                     "         T_00.\"PARTNER_GUID\"                                          "  
                     "FROM                                                                    "                      
                     "\"BUT000\" T_00 , \"BUT100\" T_01                                       "                          
                     "WHERE                                                                   "                     
                     "(T_01.\"MANDT\" (+) = ? AND T_00.\"PARTNER\" = T_01.\"PARTNER\" (+) )   "
                     "AND                                                                     " 
                     "T_00.\"CLIENT\" = ? AND T_00.\"PARTNER\" = ? AND T_00.\"XBLCK\" = ?     "                            
                     "GROUP BY                                                                "                      
                     "T_00.\"TYPE\", T_00.\"PARTNER\", T_00.\"PARTNER_GUID\", T_01.\"RLTYP\"  "                           
                     "HAVING                                                                  "                      
                     "T_00.\"TYPE\" = ? AND ( T_01.\"RLTYP\" = ? OR T_01.\"RLTYP\" = ? ) OR   "
                     "T_00 .\"TYPE\" = ?");

        char *A0 = "021";
        char *A1 = "021";
        char *A2 = "0000000308";
        char *A3 = " ";
        char *A4 = "1";
        char *A5 = "WFM001";
        char *A6 = "BUP003";
        char *A7 = "2";
        
        SQLDBC_Length A0_L = SQLDBC_NTS;
        SQLDBC_Length A1_L = SQLDBC_NTS;
        SQLDBC_Length A2_L = SQLDBC_NTS;
        SQLDBC_Length A3_L = SQLDBC_NTS;
        SQLDBC_Length A4_L = SQLDBC_NTS;        
        SQLDBC_Length A5_L = SQLDBC_NTS;
        SQLDBC_Length A6_L = SQLDBC_NTS;
        SQLDBC_Length A7_L = SQLDBC_NTS;        
        
        ps->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, A0, &A0_L, 3);
        ps->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, A1, &A1_L, 3);
        ps->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, A2, &A2_L, 10);
        ps->bindParameter(4, SQLDBC_HOSTTYPE_ASCII, A3, &A3_L, 1);
        ps->bindParameter(5, SQLDBC_HOSTTYPE_ASCII, A4, &A4_L, 1);
        ps->bindParameter(6, SQLDBC_HOSTTYPE_ASCII, A5, &A5_L, 6);
        ps->bindParameter(7, SQLDBC_HOSTTYPE_ASCII, A6, &A6_L, 6);
        ps->bindParameter(8, SQLDBC_HOSTTYPE_ASCII, A7, &A7_L, 1);
        tc_errorcheck(ps)->execute();

        return m_success;
    }

};

//----------------------------------------------------------------------
class PTS1130384
    : public SQLDBC_TestCase
{
public:
    PTS1130384()
    :SQLDBC_TestCase("PTS1130384")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1130384");
        execSQL("CREATE TABLE PTS1130384(A NUMBER(20))");
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1130384 VALUES (?)");
        unsigned short data[32];
        memset(data, 0, sizeof(data));
        data[0] = '+';
        data[1] = '0';
        data[2] = '2';
        data[3] = '0';
        data[4] = '0';
        data[5] = '4';
        data[6] = '0';
        data[7] = '6';
        SQLDBC_Length datalength = 16;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_UCS2_NATIVE, data, &datalength, 32, false);
        tc_errorcheck(ps)->execute();

        dropTable("PTS1130384B");
        execSQL("CREATE TABLE PTS1130384B(A CHAR (8) BYTE, B VARCHAR(100))");
        tc_errorcheck(ps)->prepare("INSERT INTO PTS1130384B (B, A) VALUES( 'AAA', ?)");
        unsigned char data2[] = { 0xF6, 0xE2, 0xF9, 0x42, 0xAC, 0x96, 0xE7, 0xB4 };
        SQLDBC_Length l2=8;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_BINARY, &data2, &l2, 8, false);
        tc_errorcheck(ps)->execute();
        connection()->releaseStatement(ps);
        

        return m_success;
    }
};

class PTS1132829
    : public SQLDBC_TestCase
{
public:
    PTS1132829()
    :SQLDBC_TestCase("PTS1132829")
    {}
    
    char *makeBuffer(int size, int pattern)
    {
        char *result=new char[size];
        memset(result, pattern, size);
        return result;
    }
    

    SQLDBC_Bool run() 
    {
        if(!connection()->isUnicodeDatabase()) {
            loginfo("No test (no unicode  database)");
            return true;
        }


         dropTable("PTS1132829");
         execSQL("CREATE TABLE PTS1132829(MODULE_NAME VARCHAR (40) UNICODE,"
                 "                              NUM        NUMBER(10,0),"
                 "                              NAME       LONG UNICODE,"
                 "                              BYTES_SIZE  NUMBER(10,0),"
                 "                              BYTES      LONG BYTE,"
                 "                              CERTS_SIZE NUMBER(10,0),"
                 "                              CERTS      LONG BYTE,"
                 "                              PRIMARY KEY (MODULE_NAME, NUM)"
                 ")");
        connection()->commit();
        SQLDBC_PreparedStatement *ps_insert = connection()->createPreparedStatement();
        SQLDBC_PreparedStatement *ps_update = connection()->createPreparedStatement();
        ps_insert->prepare("INSERT INTO PTS1132829 VALUES (?, ?, ?, ?, ?, ?, ?)");
        
        char           p1[]      = "AP_TTE_EXTERNAL                         ";
        SQLDBC_Length  p1_length = SQLDBC_NTS;
        SQLDBC_Int4    p2        = 42;
        SQLDBC_Length  p2_length = sizeof(SQLDBC_Int4);
        char           p3[]      = "com.sap.dbtech.jdbc.DriverSapDB";
        SQLDBC_Length  p3_length = SQLDBC_NTS;
        SQLDBC_Int4    p4        = 100;
        SQLDBC_Length  p4_length = sizeof(SQLDBC_Int4);
        char          *p5        = makeBuffer(100, 0xAA);
        SQLDBC_Length  p5_length = 100;
        SQLDBC_Int4    p6        = 100;
        SQLDBC_Length  p6_length = sizeof(SQLDBC_Int4);
        char          *p7        = makeBuffer(100, 0xBB);
        SQLDBC_Length  p7_length = 100;
        
        ps_insert->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, p1, &p1_length, 40, false);
        ps_insert->bindParameter(2, SQLDBC_HOSTTYPE_INT4,  &p2, &p2_length, sizeof(SQLDBC_Int4), false);
        ps_insert->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, p3, &p3_length, 10000, false);
        ps_insert->bindParameter(4, SQLDBC_HOSTTYPE_INT4,  &p4, &p4_length, sizeof(SQLDBC_Int4), false);
        ps_insert->bindParameter(5, SQLDBC_HOSTTYPE_BINARY, p5, &p5_length, p5_length, false);
        ps_insert->bindParameter(6, SQLDBC_HOSTTYPE_INT4,  &p6, &p6_length, sizeof(SQLDBC_Int4), false);
        ps_insert->bindParameter(7, SQLDBC_HOSTTYPE_BINARY, p7, &p7_length, p7_length, false);
        tc_errorcheck(ps_insert)->execute();
        connection()->commit();
        connection()->setAutoCommit(false);
        {
            char *p ="UPDATE \"PTS1132829\" SET \"NAME\" = ? , \"BYTES_SIZE\" = ? , "
                "\"BYTES\" = ? , \"CERTS_SIZE\" = ? , \"CERTS\" = ? WHERE \"MODULE_NAME\" = ? AND \"NUM\" = ?";
            int ucssize =(strlen(p) + 1) * sizeof(UCS2CHAR);
            UCS2CHAR *ucs2p = new UCS2CHAR[strlen(p) + 1];
            memset(ucs2p, 0, ucssize);
            convertToUCS2(ucs2p, ucssize, p);
            tc_errorcheck(ps_update)->prepare((const char *)ucs2p, SQLDBC_StringEncodingUCS2Native);
        } 
        
        //         for(int k=0; k<512; ++k) 
        {
            char           p1_ascii[]      = "com/sap/taxes/external/imp/TaxData.class";
            UCS2CHAR       *p1=new UCS2CHAR[42];
            convertToUCS2(p1, 84, p1_ascii);  
            SQLDBC_Length  p1_length = strlen(p1_ascii) * 2;

            SQLDBC_Int4    p2        = 43253;
            SQLDBC_Length  p2_length = sizeof(SQLDBC_Int4);
            char          *p3        = makeBuffer(43253, 0xEE);
            SQLDBC_Length  p3_length = 43253;
            SQLDBC_Int4    p4        = 0;
            SQLDBC_Length  p4_length = sizeof(SQLDBC_Int4);
            char          *p5        = 0;
            SQLDBC_Length  p5_length = 0;
            char          *p6_ascii        = "AP_TTE_EXTERNAL                         ";
            UCS2CHAR      *p6 = new UCS2CHAR[strlen(p6_ascii) + 2 ];
            convertToUCS2(p6, 2* (strlen(p6_ascii) + 2), p6_ascii);
            SQLDBC_Length  p6_length = strlen(p6_ascii) * 2;
            SQLDBC_Int4    p7        = 42;
            SQLDBC_Length  p7_length = sizeof(SQLDBC_Int4);
            
            ps_update->bindParameter(1, SQLDBC_HOSTTYPE_UCS2_NATIVE,  p1, &p1_length, 80, false);
            ps_update->bindParameter(2, SQLDBC_HOSTTYPE_INT4,   &p2, &p2_length, sizeof(SQLDBC_Int4), false);
            ps_update->bindParameter(3, SQLDBC_HOSTTYPE_BINARY, p3, &p3_length, p3_length, false);
            ps_update->bindParameter(4, SQLDBC_HOSTTYPE_INT4,   &p4, &p4_length, p4_length, false);
            ps_update->bindParameter(5, SQLDBC_HOSTTYPE_BINARY, p5, &p5_length, p5_length, false);
            ps_update->bindParameter(6, SQLDBC_HOSTTYPE_UCS2_NATIVE,  p6, &p6_length, p6_length, false);
            ps_update->bindParameter(7, SQLDBC_HOSTTYPE_INT4,   &p7, &p7_length, p7_length, false);
            ps_update->execute();
            delete [] p1;
            delete [] p3;
            delete [] p6;
        }
        connection()->rollback();
        connection()->rollback();
        
        // connection()->releaseStatement(ps_insert);
        connection()->releaseStatement(ps_update);
        return m_success;
    }
    
};

class PTS1132884
    : public SQLDBC_TestCase
{
public:
    PTS1132884()
    :SQLDBC_TestCase("PTS1132884")
    {}

    SQLDBC_Bool run()
    {
//         dropTable("PTS1132884_A");
//         dropTable("PTS1132884_B");
//         execSQL("CREATE TABLE PTS1132884_A (A CHAR(3) ASCII, B CHAR(30) ASCII)");
//         execSQL("CREATE TABLE PTS1132884_B (A CHAR(3) ASCII, B CHAR(30) ASCII)");
//         execSQL("INSERT INTO PTS1132884_A VALUES('000', 'AAA')");
//         execSQL("INSERT INTO PTS1132884_B VALUES('000', 'AAA')");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        ps->prepare("SELECT T_00 . \"MANDT\" , T_00 . \"BNAME\" FROM \"USRBF2\" T_00 WHERE T_00 . \"MANDT\" = ? AND "
                    "NOT EXISTS ( SELECT * FROM \"USRBF3\" T_100 WHERE T_100 . \"MANDT\" = T_00 . \"MANDT\" AND T_100 . \"BNAME\" = T_00 . \"BNAME\" )");
        // tc_errorcheck(ps)->prepare("SELECT A, B FROM PTS1132884_A T_00 WHERE T_00.A = ? AND "
        // " NOT EXISTS (SELECT * FROM PTS1132884_B T_100 WHERE T_100.A=T_00.A AND T_100.B=T_00.B)");
        char *buffer="000";
        SQLDBC_Length ind = SQLDBC_NTS;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, buffer, &ind, 3);
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs = ps->getResultSet();
        if(rs->next() == SQLDBC_OK) {
            logerror("GOT RESULT, expected none.");
        } else {
            loginfo("GOT NO RESULT");
        }
        return m_success;
    }
};

class PTS1132940
    : public SQLDBC_TestCase
{
public:
    PTS1132940()
    :SQLDBC_TestCase("PTS1132940")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1132940");
        execSQL("CREATE TABLE PTS1132940(A INTEGER, B LONG BYTE)");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        ps->prepare("INSERT INTO PTS1132940 VALUES(?, ?)");
        SQLDBC_Length p1_indicator=sizeof(SQLDBC_Int4);
        SQLDBC_Int4   p1 = 17;
        SQLDBC_LOB    p2;
        SQLDBC_Length p2_indicator=0;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &p1, &p1_indicator, sizeof(SQLDBC_Int4));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_BLOB, &p2, &p2_indicator, 0);
        // force parse again
        dropTable("PTS1132940");
        execSQL("CREATE TABLE PTS1132940(A INTEGER, B LONG BYTE)");
        tc_errorcheck(ps)->execute();

        SQLDBC_Length ind=5;
        p2.putData((void *)"ABCDE", &ind);
        if(ps->error()) {
            logerror("Could not put data");
        }
        p2.close();
        connection()->releaseStatement(ps);
        return m_success;
    }
};

class LONGTRASH
    : public SQLDBC_TestCase
{
public:
    LONGTRASH()
    :SQLDBC_TestCase("LONGTRASH")
    {}

    SQLDBC_Bool run()
    {
        dropTable("LONGTRASH");
// | Column Name | Type          | Length | Nullable | KEYPOS |
// | ----------- | ------------- | ------ | -------- | ------ |
// | RELID       | VARCHAR ASCII | 2      | NO       | 1      |
// | OBJID       | VARCHAR ASCII | 30     | NO       | 2      |
// | OBJVERS     | VARCHAR ASCII | 1      | NO       | 3      |
// | SRTF2       | FIXED         | 10     | NO       | 4      |
// | TIMESTMP    | FIXED         | 15     | YES      |        |
// | VERSION     | FIXED         | 10     | YES      |        |
// | CLUSTR      | FIXED         | 5      | YES      |        |
// | CLUSTD      | LONG BYTE     | -      | YES      |        |
        execSQL("CREATE TABLE LONGTRASH(RELID VARCHAR(2) ASCII, "
                "                       OBJID VARCHAR(30) ASCII, "
                "                       OBJVERS VARCHAR(1) ASCII, "
                "                       SRTF2 FIXED(10), "
                "                       TIMESTMP FIXED(15), "
                "                       VERSION FIXED(10), "
                "                       CLUSTR FIXED(5), "
                "                       CLUSTD LONG BYTE, "
                "                       PRIMARY KEY(RELID, OBJID, OBJVERS, SRTF2) "
                ")");
        
        
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        ps->prepare("INSERT INTO LONGTRASH ( RELID , OBJID , OBJVERS , SRTF2 , TIMESTMP , VERSION , CLUSTR , CLUSTD ) VALUES( ? , ? , ? , ? , ? , ? , ? , ? )");

        char           p1b[8][2];
        SQLDBC_Length  p1b_ind[8];
        char           p2b[8][30];
        SQLDBC_Length  p2b_ind[8];
        char           p3b[8][1];
        SQLDBC_Length  p3b_ind[8];
        SQLDBC_Int4    p4b[8];
        SQLDBC_Length  p4b_ind[8];
        char           p5b[8][16];
        SQLDBC_Length  p5b_ind[8];            
        SQLDBC_Int4    p6b[8];
        SQLDBC_Length  p6b_ind[8];
        SQLDBC_Int2    p7b[8];
        SQLDBC_Length  p7b_ind[8];
        char           p8b[8][3902];
        SQLDBC_Length  p8b_ind[8];
     
        for(int i=0; i<8; ++i) {
            
            memcpy(p1b[i], "BL", 2);
            p1b_ind[i] = 2;
            memcpy(p2b[i], "0WBMP_CHART_LIB_01            ", 30);
            p2b_ind[i]=30;
            p3b[i][0] = 'A';
            p3b_ind[i]=1;
            p4b[i] = i;
            p4b_ind[i]=0;
            memcpy(p5b[i],"+000000000000000", 16);
            p5b_ind[i]=16;
            p6b[i]=0;
            p6b_ind[i]=0;
            p7b[i] = 2116;
            p7b_ind[i] = 0;
            memset(p8b[i], 'A' + i, 3902);
            p8b_ind[i]=2118;
        }
        
        ps->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, p1b, p1b_ind, p1b_ind[0]);
        ps->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, p2b, p2b_ind, p2b_ind[0]);
        ps->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, p3b, p3b_ind, p3b_ind[0]);
        ps->bindParameter(4, SQLDBC_HOSTTYPE_INT4,  p4b, p4b_ind, 4);
        ps->bindParameter(5, SQLDBC_HOSTTYPE_ASCII, p5b, p5b_ind, p5b_ind[0]);
        ps->bindParameter(6, SQLDBC_HOSTTYPE_INT4,  p6b, p6b_ind, 4);
        ps->bindParameter(7, SQLDBC_HOSTTYPE_INT2,  p7b, p7b_ind, 2);
        ps->bindParameter(8, SQLDBC_HOSTTYPE_BINARY, p8b, p8b_ind, 3902);
        ps->setBatchSize(8);
        tc_errorcheck(ps)->execute();
        p4b[0] = 8;
        ps->setBatchSize(1);
        tc_errorcheck(ps)->execute();

/*
        char          *p1     = "BL";
        SQLDBC_Length  p1_ind = 2;
        char          *p2     = "0WBMP_CHART_LIB_01            ";
        SQLDBC_Length  p2_ind = 30;
        char          *p3     = "A";
        SQLDBC_Length  p3_ind = 1;
        SQLDBC_Int4    p4     = 8;
        SQLDBC_Length  p4_ind = 0;
        char          *p5     = "+000000000000000";
        SQLDBC_Length  p5_ind = 16;
        SQLDBC_Int4    p6     = 0;
        SQLDBC_Length  p6_ind = 0;
        SQLDBC_Int2    p7     = 2116;
        SQLDBC_Length  p7_ind = 0;
        char           p8[3902];
        memset(p8, 'X', 2118);
        SQLDBC_Length  p8_ind = 2118;
*/




        return m_success;
    }
};

/*
class PTSXXX
    : public SQLDBC_TestCase
{
public:
    PTSXXX()
    : SQLDBC_TestCase("PTSXXX")
    {}

    SQLDBC_Bool run()
    {
        connection()->setAutoCommit(false);
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        SQLDBC_Int4 p1          = 0;
        SQLDBC_Length p1_length = sizeof(SQLDBC_Int4);
        
        char        p2;
        SQLDBC_Length p2_length = 0;

        char        p3;
        SQLDBC_Length p3_length = 0;
            
        char        p4 = 'E';
        SQLDBC_Length p4_length = 1;

        char*       p5 = (char *)"0ANALYSIS_PATTERN             ";
        SQLDBC_Length p5_length = 30;
        
        char        p6 = 'D';
        SQLDBC_Length p6_length = 1;
        
        char*       p7 = (char *)"000";
        SQLDBC_Length p7_length = 3;

        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &p1, &p1_length, sizeof(SQLDBC_Int4), false);
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_BINARY, &p2, &p2_length, 1234, false);
        tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, &p3, &p3_length, 1234, false);
        tc_errorcheck(ps)->bindParameter(4, SQLDBC_HOSTTYPE_ASCII, &p4, &p4_length, 1, false);
        tc_errorcheck(ps)->bindParameter(5, SQLDBC_HOSTTYPE_ASCII, p5, &p5_length, 30, false);
        tc_errorcheck(ps)->bindParameter(6, SQLDBC_HOSTTYPE_ASCII, &p6, &p6_length, 1, false);
        tc_errorcheck(ps)->bindParameter(7, SQLDBC_HOSTTYPE_ASCII, p7, &p7_length, 3, false);
        
        char *p2_r = 0;
        char *p3_r = new char[74131];
        memset(p3_r, 'X', 74131);
        
        p2_length = 0;
        p3_length = 74131;

        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_BINARY, p2_r, &p2_length, 0, false);
        tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, p3_r, &p3_length, 74131, false);
        
        tc_errorcheck(ps)->prepare("UPDATE \"RSZWBTMPDATA\" SET \"BYTELEN\" = ? ,"
                    " \"DATA\" = ? , \"DATASTR\" = ? , \"EDITLANG\" = ? "
                    "WHERE \"OBJID\" = ? AND \"OBJVERS\" = ? AND \"VERSION\" = ? ");
        tc_errorcheck(ps)->execute();
        connection()->rollback(); 
        return m_success;
    }
};
*/


// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1138841
class PTS1138841
    : public SQLDBC_TestCase
{
public:
    PTS1138841()
    : SQLDBC_TestCase("PTS1138841")
    {}

    SQLDBC_Bool run()
    {
        connection()->setAutoCommit(false);
        dropTable("PTS1138841");
        execSQL("CREATE TABLE PTS1138841 (COL1 VARCHAR(20))");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        tc_expectederrorcheck(ps, -10333)->prepare("SELECT COL1 INTO ?,? FROM PTS1138841 WHERE COL1=?");
        return m_success;
    }
};

class PTS1139484
    : public SQLDBC_TestCase
{
public:
    PTS1139484()
    : SQLDBC_TestCase("PTS1139484")
    {}

    SQLDBC_Bool run()
    {
        dropTable("PTS1139484");
        execSQL("CREATE TABLE PTS1139484(A VARCHAR (30) ASCII PRIMARY KEY, B LONG ASCII, C VARCHAR (30) ASCII, D LONG ASCII, E LONG ASCII)");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO PTS1139484 VALUES (?, ?, ?, ?, ?)");
        char          p1[31];
        strcpy(p1, "HALLO");
        SQLDBC_Length l1=5;
        SQLDBC_LOB    p2;
        SQLDBC_Length l2 = 0;
        char          p3[31];
        strcpy(p3, "HALLO");
        SQLDBC_Length l3=5;
        SQLDBC_LOB    p4;
        SQLDBC_Length l4 = 0;
        SQLDBC_LOB    p5;
        SQLDBC_Length l5 = 0;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, p1, &l1, 31, true);
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII_CLOB, &p2, &l2, 0);
        tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, p1, &l1, 31, true);
        tc_errorcheck(ps)->bindParameter(4, SQLDBC_HOSTTYPE_ASCII_CLOB, &p4, &l4, 0);
        tc_errorcheck(ps)->bindParameter(5, SQLDBC_HOSTTYPE_ASCII_CLOB, &p5, &l5, 0);
        tc_errorcheck(ps)->execute();
        
        SQLDBC_Length i=5;
        p2.putData((void *)"ABCDE", &i);
        if(ps->error()) {
            logerror("Could not put data");
        }
        p2.close();
        if(ps->error()) {
            logerror("Could not put data");
        }
        i=0;
        p4.putData(0, &i);
        if(ps->error()) {
            logerror("Could not put data");
        }
        p4.close();
        if(ps->error()) {
            logerror("Could not put data");
        }
        i=1;
        p5.putData((void *)"ABCDE", &i);
        if(ps->error()) {
            logerror("Could not put data");
        }
        p5.close();
        tc_errorcheck(ps)->setBatchSize(1);
        strcpy(p1, "AAAAA");
        tc_errorcheck(ps)->execute();
        connection()->releaseStatement(ps);
        return m_success;
        
    }
};

int main (int argc, char **argv)
{
    // Initalize from command line params
    SQLDBC_TestFrame testframe (argc, argv);
    

    
//     testframe.addTestCase(new LONGTRASH()); // memory overwrite, only visible when debugging
//     testframe.addTestCase (new PTS1132884()); // only read only on R/3 tables
    testframe.addTestCase (new PTS1138841());
    testframe.addTestCase (new PTS1132940());
    testframe.addTestCase (new PTS1132829());
    testframe.addTestCase (new PTS1130384());
    testframe.addTestCase (new PTS1130048()); 
    testframe.addTestCase (new PTS1130036());
    testframe.addTestCase (new PTS1129648());
    testframe.addTestCase (new PTS1129646());
    testframe.addTestCase (new PTS1127267());
    testframe.addTestCase (new PTS1127276());
    testframe.addTestCase (new PTS1127270());
    testframe.addTestCase (new PTS1127269());
    testframe.addTestCase (new PTS1127266());
    testframe.addTestCase (new PTS1127510());
    testframe.addTestCase (new PTS1129091());
    testframe.addTestCase (new PTS1139484());
    
    if (testframe.run ()) 
        return 0;
    return 1;
}


