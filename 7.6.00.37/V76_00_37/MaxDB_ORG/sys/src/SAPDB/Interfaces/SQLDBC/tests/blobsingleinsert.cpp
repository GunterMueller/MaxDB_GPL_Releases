/*!
  @file           blobsingleinsert.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Checks various ways to insert LONG data.

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#include <string.h>

#define MAXDATASIZE  (128 * 1024) 

int blobinsert_sizes[] = {
    0, 1, 2, 3, 4, 5,
    32766, 32767, 32768, 32769,
    65530, 65535, 65536
};


class BlobInsert
    : public SQLDBC_TestCase
{
public:
    BlobInsert()
    :SQLDBC_TestCase("BlobInsert")
    {}

    virtual SQLDBC_Bool prepare()
    {
        dropTable("BLOBINSERT_TEST");
        execSQL("CREATE TABLE BLOBINSERT_TEST(K INTEGER PRIMARY KEY, V LONG VARCHAR ASCII)");
        char *templatedata = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        
        for(int i=0; i<MAXDATASIZE; ++i) {
            testdata[i] = templatedata[i % 26];
            readdata[i] = 0;
        }
        
        return true;
    }
    
    virtual SQLDBC_Bool run()
    {
        m_success = runInsert();
        m_success = m_success && runSelect();
        return m_success;
    }

    SQLDBC_Bool runInsert()
    {
        SQLDBC_PreparedStatement *insert_statement=tc_errorcheck(connection())->createPreparedStatement();
        insert_statement->prepare("INSERT INTO BLOBINSERT_TEST (K,V) VALUES (?,?)");
        SQLDBC_Length bloblen_ind;
        SQLDBC_Int4   blobidx;
        SQLDBC_Length blobidx_ind = sizeof(SQLDBC_Int4);
        tc_errorcheck(insert_statement)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &blobidx, &blobidx_ind, sizeof(SQLDBC_Int4));
        tc_errorcheck(insert_statement)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, &testdata, &bloblen_ind, MAXDATASIZE);
        for(int i=0; i<sizeof(blobinsert_sizes)/sizeof(int); ++i) {
            blobidx = blobinsert_sizes[i];
            bloblen_ind = blobidx;
            tc_errorcheck(insert_statement)->execute();
        }
        connection()->releaseStatement(insert_statement);
        connection()->commit();
        m_success && checkData();
        return m_success;
    }

    SQLDBC_Bool checkData()
    {
        SQLDBC_PreparedStatement *sel_statement = tc_errorcheck(connection())->createPreparedStatement();
        sel_statement->prepare("SELECT V FROM BLOBINSERT_TEST WHERE K = ?");
        SQLDBC_Int4 blobidx = 0;
        SQLDBC_Length blobidx_ind = sizeof(SQLDBC_Int4);
        tc_errorcheck(sel_statement)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &blobidx, &blobidx_ind, sizeof(SQLDBC_Int4));
        for(int i=0; i<sizeof(blobinsert_sizes)/sizeof(int); ++i) {
            blobidx = blobinsert_sizes[i];
            tc_errorcheck(sel_statement)->execute();
            SQLDBC_ResultSet *rs=sel_statement->getResultSet();
            tc_errorcheck(rs)->next();
            SQLDBC_Length blob_length;
            memset(readdata, 0, MAXDATASIZE);
            tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_ASCII, readdata, &blob_length, MAXDATASIZE, false);
            if(blob_length != blobidx) {
                logerror("Expected BLOB of length %d, found %d.", blobidx, blob_length);
            } else if(blob_length) {
                if(memcmp(testdata, readdata, blob_length) != 0) {
                    logerror("Found wrong LONG data at pos %d.", blob_length);
                }
            }
        }
        connection()->releaseStatement(sel_statement);
        return m_success;
    }
    
    SQLDBC_Bool runSelect()
    {
        SQLDBC_PreparedStatement *sel_statement = tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(sel_statement)->prepare("SELECT K, V FROM BLOBINSERT_TEST ORDER BY K");
        tc_errorcheck(sel_statement)->execute();
        SQLDBC_Int4 blobidx;
        SQLDBC_Length blobidx_ind;
        SQLDBC_Length blob_ind;
        SQLDBC_ResultSet *rs=sel_statement->getResultSet();
        for(int i=0; i<sizeof(blobinsert_sizes)/sizeof(int); ++i) {
            tc_errorcheck(rs)->next();
            memset(readdata, 0, MAXDATASIZE);
            tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &blobidx, &blobidx_ind, sizeof(SQLDBC_Int4));
            tc_errorcheck(rs)->getObject(2, SQLDBC_HOSTTYPE_ASCII, &readdata, &blob_ind, MAXDATASIZE);
            if((blobidx != blob_ind) || (blobidx != blobinsert_sizes[i])) {
                logerror("Unexpected size in blob idx %d ind %d val %d", blobidx, blob_ind, blobinsert_sizes[i]);
            } else if(memcmp(readdata, testdata, blob_ind)) {
                logerror("Unexpected data in blob idx %d ind %d val %d", blobidx, blob_ind, blobinsert_sizes[i]);
            }
        }
        connection()->releaseStatement(sel_statement);
        return m_success;
    }

private:
    char testdata[MAXDATASIZE];
    char readdata[MAXDATASIZE];
};





int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new BlobInsert());
  if (testframe.run ()) 
      return 0;
  
  return 1;
}


