/****************************************************************************

  Module      : batchtest.cpp

  -------------------------------------------------------------------------

  responsible : D039759

  special area: Interface Runtime
  description : Batch execute tests.


  last changed: 2001-01-20
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <stdlib.h>
#include <string.h>
class BatchTest
    : public SQLDBC_TestCase
{
public:
    int testcount;
    
    BatchTest()
    :SQLDBC_TestCase("BatchTest")
    {
        testcount=100000;
    }

    SQLDBC_Bool run()
    {
        loginfo("Testing batch insert.");
        m_success = m_success && runBatchInsert(false);
        
//         loginfo("Testing batch delete.");
//         m_success = m_success && runBatchDelete();
//         loginfo("Testing batch with constraint violation.");
//         m_success = m_success && runBatchViolation();
//         loginfo("Testing parameter by address in batch.");
//         m_success = m_success && runParameterAddr();
//         loginfo("Testing rowwise binding and batch.");
//         m_success = m_success && runRowWiseBinding();
//         loginfo("Testing small batches with errors.");
//         m_success = m_success && runSmallError();
        return m_success;
    }
    
    SQLDBC_Bool runSmallError()
    {
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO BATCHINSERT_TEST VALUES(?)");
        char buffer[3 * 20];
        strcpy(buffer, "99999");
        strcpy(buffer + 20, "100000");
        strcpy(buffer + 40, "1abcde");
        SQLDBC_Length li[3]; li[0] = li[1] = li[2] = SQLDBC_NTS;
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, buffer, li, 20);
        tc_errorcheck(ps)->setBatchSize(3);
        tc_expectederrorcheck(ps, -10802)->execute();
        const SQLDBC_Int4 *statusarray=ps->getRowStatus();
        if(statusarray[0] != SQLDBC_SUCCESS_NO_INFO) {
            logerror("Expected SUCCESS_NO_INFO at pos 0.");
        }
        if(statusarray[1] != SQLDBC_SUCCESS_NO_INFO) {
            logerror("Expected SUCCESS_NO_INFO at pos 1.");
        }
        if(statusarray[2] != SQLDBC_EXECUTE_FAILED) {
            logerror("Expected EXECUTE_FAILED at pos 2.");
        }
        connection()->releaseStatement(ps);
        return m_success;
    }

    SQLDBC_Bool runBatchInsert(SQLDBC_Bool loop)
    {
        
        dropTable("BATCHINSERT_TEST");
        m_success = m_success && execSQL("CREATE TABLE BATCHINSERT_TEST (A INT PRIMARY KEY, "
                                         "D1 INTEGER, D2 INTEGER, D3 INTEGER, D4 INTEGER, D5 INTEGER,"
                                         "D6 INTEGER, D7 INTEGER, D8 INTEGER, D9 INTEGER, D10 INTEGER,"
                                         "D11 INTEGER, D12 INTEGER, D13 INTEGER, D14 INTEGER, D15 INTEGER,"
                                         "D16 INTEGER, D17 INTEGER, D18 INTEGER, D19 INTEGER, D20 INTEGER"
                                         ")");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Length *lengthindicator = new SQLDBC_Length[testcount];
        SQLDBC_Int4 *data = new SQLDBC_Int4[testcount];
        for(SQLDBC_Int4 i=1; i<=testcount; ++i) {
            data[i-1]=i;
            lengthindicator[i-1]=sizeof(SQLDBC_Int4);
        }
        SQLDBC_Retcode rc;
    insert_again:
        { 
            tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(4, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(5, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(6, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(7, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(8, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(9, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(10, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(11, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(12, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(13, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(14, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(15, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(16, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(17, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(18, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(19, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(20, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 
            tc_errorcheck(ps)->bindParameter(21, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); 

        }
        { tc_errorcheck(ps)->setBatchSize(testcount); }
        { tc_errorcheck(ps)->prepare("INSERT INTO BATCHINSERT_TEST VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", SQLDBC_StringEncodingAscii); }
        { rc = tc_errorcheck(ps)->executeBatch(); }
        
        if(rc!= SQLDBC_OK) {
            logerror("executeBatch() returned %d", rc);
            return m_success;
        }

        SQLDBC_Int4 ra = tc_errorcheck(ps)->getRowsAffected();
        if(ra != testcount) {
            logerror("Wrong count of affected rows found. Exp %d Fnd %d", testcount, ra);
        }
        
        const SQLDBC_Int4 *statusarray=ps->getRowStatus();
        for(SQLDBC_Int4 j=0; j<testcount; ++j) {
            if(statusarray[j] != SQLDBC_SUCCESS_NO_INFO) {
                logerror("Status array contains error at pos %d, aborting.", (j+1));
                break;
            }
        }
        
        if(loop) {
            loop = false;
            dropTable("BATCHINSERT_TEST");
            m_success = m_success && execSQL("CREATE TABLE BATCHINSERT_TEST (A INT PRIMARY KEY, "
                                             "D1 INTEGER, D2 INTEGER, D3 INTEGER, D4 INTEGER, D5 INTEGER,"
                                             "D6 INTEGER, D7 INTEGER, D8 INTEGER, D9 INTEGER, D10 INTEGER,"
                                             "D11 INTEGER, D12 INTEGER, D13 INTEGER, D14 INTEGER, D15 INTEGER,"
                                             "D16 INTEGER, D17 INTEGER, D18 INTEGER, D19 INTEGER, D20 INTEGER"
                                             ")");
            if(m_success) {
                goto insert_again;
            }
        }

        

        connection()->releaseStatement(ps);
        execSQL("COMMIT");
        delete [] lengthindicator;
        delete [] data;
        return m_success;
    }
    
    SQLDBC_Bool runBatchDelete()
    {
        // will and must run after the 'batchinsert' test
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Length *lengthindicator = new SQLDBC_Length[testcount];
        SQLDBC_Int4 *data = new SQLDBC_Int4[testcount];
        for(SQLDBC_Int4 i=1; i<=testcount; ++i) {
            data[i-1]=i;
            lengthindicator[i-1]=sizeof(SQLDBC_Int4);
        }
        { tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); }
        { tc_errorcheck(ps)->setBatchSize(testcount); }
        { tc_errorcheck(ps)->prepare("DELETE FROM BATCHINSERT_TEST WHERE A=?", SQLDBC_StringEncodingAscii); }
        { tc_errorcheck(ps)->executeBatch(); }
        

        const SQLDBC_Int4 *statusarray=ps->getRowStatus();
        for(SQLDBC_Int4 j=0; j<testcount; ++j) {
            if(statusarray[j] != SQLDBC_SUCCESS_NO_INFO) {
                logerror("Status array contains error at pos %d, aborting.", (j+1));
                break;
            }
        }
        connection()->releaseStatement(ps);
        execSQL("COMMIT");
        delete [] lengthindicator;
        delete [] data;
        return m_success;
    }
    

    SQLDBC_Bool runBatchViolation()
    {
        // will and must run after the 'batchinsert' test
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Length *lengthindicator = new SQLDBC_Length[testcount];
        SQLDBC_Int4 *data = new SQLDBC_Int4[10];
        for(SQLDBC_Int4 i=1; i<=10; ++i) {
            data[i-1]=i;
            lengthindicator[i-1]=sizeof(SQLDBC_Int4);
        }
        data[8-1]=7; // 
        
        { tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); }
        { tc_errorcheck(ps)->setBatchSize(10); }
        { tc_errorcheck(ps)->prepare("INSERT INTO BATCHINSERT_TEST VALUES(?)", SQLDBC_StringEncodingAscii); }
        { tc_expectederrorcheck(ps, 200)->executeBatch(); }
        
        const SQLDBC_Int4 *statusarray=ps->getRowStatus();
        for(SQLDBC_Int4 j=0; j<10; ++j) {
            if(statusarray[j] != SQLDBC_SUCCESS_NO_INFO && j < 7) {
                logerror("Status array contains error for pos %d ", (j+1));
            } else if(statusarray[j] != SQLDBC_EXECUTE_FAILED && j >= 7) {
                logerror("Status array contains no error for pos %d ", (j+1));
            }
        }
        connection()->releaseStatement(ps);
        execSQL("COMMIT");
        delete [] lengthindicator;
        delete [] data;
        return m_success;
    }
    
    SQLDBC_Bool runParameterAddr()
    {
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_Length *lengthindicator=new SQLDBC_Length[3];
        lengthindicator[0]=lengthindicator[1]=lengthindicator[2]=sizeof(SQLDBC_Int4);
        SQLDBC_Int4 **data=new SQLDBC_Int4*[3];
        SQLDBC_Int4 data1=192;
        SQLDBC_Int4 data2=178;
        SQLDBC_Int4 data3=234;
        data[0]=&data1;
        data[1]=&data2;
        data[2]=&data3;            
        
        { tc_errorcheck(ps)->bindParameterAddr(1, SQLDBC_HOSTTYPE_INT4, data, lengthindicator, sizeof(SQLDBC_Int4)); }
        { tc_errorcheck(ps)->setBatchSize(3); }
        { tc_errorcheck(ps)->prepare("INSERT INTO BATCHINSERT_TEST VALUES(?)", SQLDBC_StringEncodingAscii); }
        { tc_errorcheck(ps)->executeBatch(); }
        
        const SQLDBC_Int4 *statusarray=ps->getRowStatus();
        for(SQLDBC_Int4 j=0; j<3; ++j) {
            if(statusarray[j] != SQLDBC_SUCCESS_NO_INFO) {
                logerror("Status array contains error for pos %d ", (j+1));
            } 
        }
        connection()->releaseStatement(ps);
        execSQL("COMMIT");
        delete [] lengthindicator;
        delete [] data;
        return m_success;

    }

    SQLDBC_Bool runRowWiseBinding()
    {
        struct rowdata {
            SQLDBC_Int4 data;
            SQLDBC_Length indicator;
            char     padding[37];
        };
        struct rowdata rows[3];
        rows[0].data=19;
        rows[1].data=20;
        rows[2].data=21;
        rows[0].indicator = rows[1].indicator = rows[2].indicator = sizeof(SQLDBC_Int4);
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        { tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, & (rows[0].data), &(rows[0].indicator), sizeof(SQLDBC_Int4)); }
        { tc_errorcheck(ps)->setBatchSize(3); }
        { tc_errorcheck(ps)->setBindingType(sizeof(struct rowdata)); }
        { tc_errorcheck(ps)->prepare("INSERT INTO BATCHINSERT_TEST VALUES(?)", SQLDBC_StringEncodingAscii); }
        { tc_errorcheck(ps)->executeBatch(); }
        
        const SQLDBC_Int4 *statusarray=ps->getRowStatus();
        for(SQLDBC_Int4 j=0; j<3; ++j) {
            if(statusarray[j] != SQLDBC_SUCCESS_NO_INFO) {
                logerror("Status array contains error for pos %d ", (j+1));
            } 
        }
        connection()->releaseStatement(ps);
        execSQL("COMMIT");
        return m_success;
    }
    
};

//----------------------------------------------------------------------
class BatchDBSCommand
    : public SQLDBC_TestCase
{
public:
   
    BatchDBSCommand()
    :SQLDBC_TestCase("BatchDBSCommand")
    {}

    SQLDBC_Bool run()
    {
        dropTable("BATCHDBSTEST");
        m_success = m_success && execSQL("CREATE TABLE BATCHDBSTEST (A INTEGER)");
        SQLDBC_Statement *s=tc_errorcheck(connection())->createStatement();
        char buffer[1024];
        for(int i=1; i<1000; ++i) {
            sprintf(buffer, "INSERT INTO BATCHDBSTEST VALUES(%d)", i);
            tc_errorcheck(s)->addBatch(buffer, SQLDBC_StringEncodingAscii);
        }
        tc_errorcheck(s)->executeBatch();
        
        tc_errorcheck(s)->clearBatch();
        tc_errorcheck(s)->addBatch("INSERT INTO BATCHDBSTEST VALUES(1000)", SQLDBC_StringEncodingAscii);
        tc_errorcheck(s)->addBatch("INSERT INTO BATCHDBSTEST VALUES(1001)", SQLDBC_StringEncodingAscii);
        tc_errorcheck(s)->addBatch("WRONG AND FAULTY", SQLDBC_StringEncodingAscii);
        tc_errorcheck(s)->addBatch("INSERT INTO BATCHDBSTEST VALUES(1002)", SQLDBC_StringEncodingAscii);
        if(s->getBatchSize() != 4) {
            logerror("Expected 4 rows");
        }
        s->executeBatch();
        const SQLDBC_Int4 *rcarray=s->getRowStatus();
        if(rcarray[0] != 1 ||
           rcarray[1] != 1 ||
           rcarray[2] != SQLDBC_EXECUTE_FAILED ||
           rcarray[3] != 1) {
            logerror("Row status array contains not expected values.");
        }
        return m_success;
    }
};
    
class MassInsert
    : public SQLDBC_TestCase
{
public:
   
  MassInsert()
    :SQLDBC_TestCase("MassInsert")
    ,collength(500)
  {
  }

    SQLDBC_Bool run()
    {
      SQLDBC_Int4 packet_size=0;
      SQLDBC_PreparedStatement *ps_packetsize = connection()->createPreparedStatement();
      tc_errorcheck(ps_packetsize)->prepare("SELECT VALUE INTO ? FROM ACTIVECONFIGURATION WHERE PARAMETERNAME = '_PACKET_SIZE'");
      tc_errorcheck(ps_packetsize)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &packet_size, NULL, sizeof(packet_size));
      tc_errorcheck(ps_packetsize)->execute();
      if (packet_size < 16384) {
        logerror("PACKET_SIZE is to small please change the database configuration.");
        return m_success;
      }      

      /* calculate the number of rows neccessary for using 3 packets */
      SQLDBC_Int4 rows = (packet_size/collength)*3;

      /* generate data */
      col=new char[collength * rows];
      ind=new SQLDBC_Length[rows];
      for(int i=0; i < rows;i++) {
        ind[i] = collength;
        sprintf(col+i*collength, "%d", i);
        memset(col+i*collength+strlen(col+i*collength), 'A', collength-+strlen(col+i*collength));
      }

      CreateAndInsert(rows);
      /* run twice, for parsing again */
      CreateAndInsert(rows);

      dropTable("T4000");
      delete col;
      delete ind;
      return m_success;
    }
    SQLDBC_Bool CreateAndInsert(int rows)
    {
        SQLDBC_Retcode rc;

        dropTable("T4000");
        char sql[300];
        sprintf(sql, "CREATE TABLE T4000 (col1 CHAR(%d), PRIMARY KEY(col1))", collength);
        execSQL(sql);

        SQLDBC_PreparedStatement *pstmt=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(pstmt)->prepare("INSERT INTO T4000 VALUES(?)");
        tc_errorcheck(pstmt)->setBatchSize(rows);
        tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, col, ind, collength);
        tc_errorcheck(pstmt)->execute();
        SQLDBC_Int4 inserted_rows = pstmt->getRowsAffected();

        SQLDBC_Int4 count_rows;
        tc_errorcheck(pstmt)->setBatchSize(1);
        tc_errorcheck(pstmt)->prepare("SELECT count(*) into ? FROM T4000");
        tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &count_rows, NULL, sizeof(count_rows));
        tc_errorcheckrc(rc, pstmt)->execute();
        if (count_rows != rows || inserted_rows != rows)
          logerror("Wrong number of rows inserted (%d), selected (%d), expected (%d)", inserted_rows, count_rows, rows);

        /* check content */
        tc_errorcheck(pstmt)->setBatchSize(1);
        tc_errorcheck(pstmt)->prepare("SELECT col1 FROM T4000 where col1 = ?");
        tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, col+(rows-1)*collength, &ind[rows-1], collength);
        tc_errorcheckrc(rc, pstmt)->execute();
        if (rc != SQLDBC_OK) {
          logerror("Error or row not found after insert");          
        }
        tc_errorcheck(connection())->releaseStatement(pstmt);
        return m_success;
    }
private:
  SQLDBC_Length collength;
  char *col;
  SQLDBC_Length *ind;
};
    

class BatchParseAgain
    : public SQLDBC_TestCase
{
public:
   
    BatchParseAgain()
    :SQLDBC_TestCase("BatchParseAgain")
    {
    }

    SQLDBC_Bool run()
    {
        dropTable("BATCHINSERTPARSEAGAIN");
        execSQL("CREATE TABLE BATCHINSERTPARSEAGAIN(A INTEGER PRIMARY KEY, B VARCHAR(4000) ASCII)");
        
#define BATCHSIZE 48
        static SQLDBC_Length a_length[BATCHSIZE];
        static SQLDBC_Length b_length[BATCHSIZE];
        static SQLDBC_Int4   a[BATCHSIZE];
        static char cdata[BATCHSIZE * 20];
        memset(cdata, 'A', sizeof(cdata));
        for(int i=0; i<BATCHSIZE; ++i) {
            a_length[i] = sizeof(SQLDBC_Int4);
            b_length[i] = 20;
            a[i] = i;
        }
        
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO BATCHINSERTPARSEAGAIN VALUES(?, ?)");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, a, a_length, sizeof(SQLDBC_Int4), false);
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, cdata, b_length, 20, false);
        tc_errorcheck(ps)->setBatchSize(BATCHSIZE);
        
        // now force parse again
         dropTable("BATCHINSERTPARSEAGAIN");
         execSQL("CREATE TABLE BATCHINSERTPARSEAGAIN(A INTEGER PRIMARY KEY, B VARCHAR(4000) ASCII)");
        
        tc_errorcheck(ps)->execute();
        SQLDBC_Int4 rows=ps->getRowsAffected();
        if(rows != BATCHSIZE) {
            logerror("WRONG NUMBER OF ROWS %d", rows);
        }
        connection()->releaseStatement(ps);
        return m_success;
    }
};
        
int main(int argc, char **argv)
{
    SQLDBC_TestFrame testframe(argc, argv);
    testframe.addTestCase(new BatchTest());
//     testframe.addTestCase(new MassInsert());
    testframe.addTestCase(new BatchDBSCommand());
    testframe.addTestCase(new BatchParseAgain());
    if(testframe.run()) return 0;
    return 1;
}
