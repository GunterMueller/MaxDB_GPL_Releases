/*!
  @file           FILE
  @author         AUTHOR
  @ingroup        GROUP
  @brief
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

#include <string.h>
#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"

#define BIGDATA_SIZE 300000

class InsertLOB
    : public SQLDBC_TestCase
{
public:
    InsertLOB()
    :SQLDBC_TestCase("LOBTEST")
    {}

    virtual SQLDBC_Bool prepare()
    {
        dropTable("LOLOTEST");
        execSQL("CREATE TABLE LOLOTEST (K INT PRIMARY KEY, L1 LONG ASCII, L2 LONG BYTE)");
        dropTable("LOBMULTEST");
        execSQL("CREATE TABLE LOBMULTEST (K INT PRIMARY KEY, L1 LONG ASCII)");
        return true;
    }

    virtual SQLDBC_Bool run()
    {
        m_success = m_success && runInsert();
        m_success = m_success && runUpdate();
        m_success = m_success && runSelect();
        m_success = m_success && runSelectIntoBig();
        m_success = m_success && runInsertEmptyLong();
        m_success = m_success && runMultiInsert();
        m_success = m_success && runMultiBindParam();
        return m_success;
    }

    SQLDBC_Bool runUpdate()
    {
        SQLDBC_PreparedStatement *psu = tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_LOB l1;
        tc_errorcheck(psu)->prepare("UPDATE LOLOTEST SET L1 = ? WHERE K=1");
        tc_errorcheck(psu)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII_CLOB, &l1, 0, 0);
        tc_errorcheck(psu)->execute();
        SQLDBC_Length ll1=SQLDBC_NTS;
        l1.putData((void *)"Knecht Ruprecht", &ll1);
        tc_errorcheck(psu);
        l1.close();
        tc_errorcheck(psu);
        execSQL("COMMIT");
        return m_success;
    }

    SQLDBC_Bool runSelectInto()
    {
        SQLDBC_PreparedStatement *pssi = tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_LOB l1;
        tc_errorcheck(pssi)->prepare("SELECT L1 INTO ? FROM LOLOTEST WHERE K = 1");
        tc_errorcheck(pssi)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII_CLOB, &l1, 0, 0);
        tc_errorcheck(pssi)->execute();
        char buffer[100];
        memset(buffer, '_', 100);
        buffer[99] = '\0';
        SQLDBC_Retcode rc = l1.getData(buffer, 0, 100);
        if(rc != SQLDBC_OK && rc != SQLDBC_DATA_TRUNC) {
            logerror("WRONG ERROR CODE %d", rc);
            return false;
        }
        if(strcmp("Knecht Ruprecht", buffer) != 0) {
            logerror("FOUND UNEXPECTED %s ", buffer);
            return false;
        }
        rc = l1.close();
        if(rc != SQLDBC_OK) {
            logerror("CANNOT CLOSE LOB OBJECT");
            return false;
        }

        return true;
    }

    SQLDBC_Bool runSelectIntoBig()
    {
        SQLDBC_PreparedStatement *pssi = tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_LOB l1;
        tc_errorcheck(pssi)->prepare("SELECT L2 INTO ? FROM LOLOTEST WHERE K = 1");
        tc_errorcheck(pssi)->bindParameter(1, SQLDBC_HOSTTYPE_BLOB, &l1, 0, 0);
        tc_errorcheck(pssi)->execute();
        char buffer[100];
        SQLDBC_Retcode rc;
        SQLDBC_Length indicator=0;
        do {
            rc = l1.getData(buffer, &indicator, 100);
            // loginfo("INDICATOR IS NOW %d\n", indicator);
        } while((rc==SQLDBC_OK || rc==SQLDBC_DATA_TRUNC) && indicator>0);

        rc = l1.close();
        if(rc != SQLDBC_OK) {
            logerror("CANNOT CLOSE FAT LOB OBJECT");
            return false;
        }
        pssi->prepare("SELECT 123 FROM DUAL");

        return true;
    }

    SQLDBC_Bool runSelect()
    {
        SQLDBC_PreparedStatement *pssi = tc_errorcheck(connection())->createPreparedStatement();
        SQLDBC_LOB l1;
        tc_errorcheck(pssi)->prepare("SELECT L1 FROM LOLOTEST WHERE K = 1");
        tc_errorcheck(pssi)->execute();

        SQLDBC_ResultSet *rs = pssi->getResultSet();

        tc_errorcheck(rs)->bindColumn(1, SQLDBC_HOSTTYPE_ASCII_CLOB, &l1, 0, 0);
        tc_errorcheck(rs)->next();
        tc_errorcheck(rs)->getRowSet()->fetch();

        char buffer[100];
        memset(buffer, '_', 100);
        buffer[99] = '\0';
        SQLDBC_Retcode rc = l1.getData(buffer, 0, 100);
        if(rc != SQLDBC_OK && rc != SQLDBC_DATA_TRUNC) {
            logerror("WRONG ERROR CODE %d", rc);
            return false;
        }
        if(strcmp("Knecht Ruprecht", buffer) != 0) {
            logerror("FOUND UNEXPECTED %s ", buffer);
            return false;
        }
        return true;
    }


    SQLDBC_Bool runInsertEmptyLong()
    {
        char *bdata = new char[BIGDATA_SIZE];
        memset(bdata, 'A', BIGDATA_SIZE);
        SQLDBC_LOB l1;
        SQLDBC_LOB l2;
        SQLDBC_Int4 k;
        SQLDBC_PreparedStatement *s=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(s)->prepare("INSERT INTO LOLOTEST VALUES (?, ?, ?)");
        tc_errorcheck(s)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &k, 0, sizeof(SQLDBC_Int4));
        tc_errorcheck(s)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII_CLOB, &l1, 0, 0);
        tc_errorcheck(s)->bindParameter(3, SQLDBC_HOSTTYPE_BLOB, &l2, 0, 0);
        k=10;
        tc_errorcheck(s)->execute();
        // Nothing of this should be inserted.
        SQLDBC_Length ll1=0;
        l1.putData((void *)"Homer Marge Maggie Lisa Bart", &ll1);
        tc_errorcheck(s);
        l1.close();
        tc_errorcheck(s);
        SQLDBC_Length ll2=BIGDATA_SIZE;
        l2.putData((void *)bdata,
                   &ll2);
        tc_errorcheck(s);
        l2.close();
        tc_errorcheck(s);
        tc_errorcheck(s)->clearParameters();
        tc_errorcheck(s)->prepare("SELECT 1 FROM DUAL");
        tc_errorcheck(s)->execute();
        execSQL("COMMIT");
        delete [] bdata;
        return m_success;

    }



    SQLDBC_Bool runInsert()
    {
        char *bdata = new char[BIGDATA_SIZE];
        memset(bdata, 'A', BIGDATA_SIZE);
        SQLDBC_LOB l1;
        SQLDBC_LOB l2;
        SQLDBC_Int4 k;
        SQLDBC_PreparedStatement *s=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(s)->prepare("INSERT INTO LOLOTEST VALUES (?, ?, ?)");
        tc_errorcheck(s)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &k, 0, sizeof(SQLDBC_Int4));
        tc_errorcheck(s)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII_CLOB, &l1, 0, 0);
        tc_errorcheck(s)->bindParameter(3, SQLDBC_HOSTTYPE_BLOB, &l2, 0, 0);
        k=1;
        tc_errorcheck(s)->execute();
        SQLDBC_Length ll1=SQLDBC_NTS;
        l1.putData((void *)"Homer Marge Maggie Lisa Bart", &ll1);
        tc_errorcheck(s);
        l1.close();
        tc_errorcheck(s);
        SQLDBC_Length ll2=BIGDATA_SIZE;
        l2.putData((void *)bdata,
                   &ll2);
        tc_errorcheck(s);
        l2.close();
        tc_errorcheck(s);
        execSQL("COMMIT");
        delete [] bdata;
        return m_success;
    }

    SQLDBC_Bool runMultiInsert()
    {
        char *data[] = { "Homer",
                         "Maggie",
                         "Bart",
                         "Marge",
                         "Tingeltangel-Bob",
                         "Crusty"
        };
        SQLDBC_Int4 data_size = sizeof(data)/sizeof(char *);
        SQLDBC_Int4 k;
        SQLDBC_LOB  l;
        SQLDBC_Length datalength = SQLDBC_NTS;
        SQLDBC_PreparedStatement *ps = tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO LOBMULTEST VALUES(?, ?)");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &k, 0, sizeof(SQLDBC_Int4));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII_CLOB, &l, 0, 0);
        for(k = 1; k<=data_size; ++k) {
            tc_errorcheck(ps)->execute();
            if(l.putData(data[k-1], &datalength) != SQLDBC_OK) {
                logerror("DATA INSERT FAILED FOR ROW %d", k);
                connection()->releaseStatement(ps);
                return m_success = false;
            }
            l.close();
            tc_errorcheck(ps);
        }
        connection()->releaseStatement(ps);
        execSQL("COMMIT");
        return m_success;
    }


    SQLDBC_Bool runMultiBindParam()
    {
        char *data = "Testing 1 2 3";
        SQLDBC_Int4 k;
        SQLDBC_LOB  l;
        SQLDBC_Length datalength = SQLDBC_NTS;
        SQLDBC_PreparedStatement *ps = tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO LOBMULTEST VALUES(?, ?)");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &k, 0, sizeof(SQLDBC_Int4));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII_CLOB, &l, 0, 0);

        k = 127;
        tc_errorcheck(ps)->execute();
        l.putData(data, &datalength);
        tc_errorcheck(ps);
        l.close();
        tc_errorcheck(ps);
        k=128;
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, data, &datalength, strlen(data));
        tc_errorcheck(ps)->execute();
        return m_success;
    }

};

class LOBLength
    : public SQLDBC_TestCase
{
public:
    LOBLength()
    :SQLDBC_TestCase("LOBLength")
    {}

    SQLDBC_Bool run()
    {
        dropTable("LOBLENGTH");
        execSQL("CREATE TABLE LOBLENGTH(A INTEGER PRIMARY KEY, B LONG BYTE)");
        SQLDBC_Int4 k=1;
        SQLDBC_Length k_len = sizeof(SQLDBC_Int4);
        char        *buf = new char[1024*1024];
        memset(buf, 'A', 1024*1024);
        SQLDBC_Length buf_len = 0;
        SQLDBC_PreparedStatement * ps = connection()->createPreparedStatement();
        tc_errorcheck(ps)->prepare("INSERT INTO LOBLENGTH VALUES(?, ?)");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &k, &k_len, sizeof(SQLDBC_Int4));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_BINARY, buf, &buf_len, 1024 * 1024, false);

        k       = 1;
        buf_len = 0;
        tc_errorcheck(ps)->execute();
        k       = 2;
        buf_len = 17;
        tc_errorcheck(ps)->execute();
        k       = 3;
        buf_len = 250000;
        tc_errorcheck(ps)->execute();
        k       = 4;
        buf_len = 1;
        tc_errorcheck(ps)->execute();
        k       = 5;
        buf_len = 1024*512;
        tc_errorcheck(ps)->execute();
        connection()->commit();
        tc_errorcheck(ps)->clearParameters();
        tc_errorcheck(ps)->prepare("SELECT A, LENGTH(B), B FROM LOBLENGTH ORDER BY A");
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs = tc_errorcheck(ps)->getResultSet();
        SQLDBC_Int4 k_value;
        SQLDBC_Int4 b_length_value;
        SQLDBC_LOB  b_lob;
        tc_errorcheck(rs)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, &k_value, 0, sizeof(SQLDBC_Int4));
        tc_errorcheck(rs)->bindColumn(2, SQLDBC_HOSTTYPE_INT4, &b_length_value, 0, sizeof(SQLDBC_Int4));
        tc_errorcheck(rs)->bindColumn(3, SQLDBC_HOSTTYPE_BLOB, &b_lob, 0, sizeof(SQLDBC_LOB));
        for(int i=1; i<=5; ++i) {
            tc_errorcheck(rs)->next();
            rs->getRowSet()->fetch();
            SQLDBC_Int4 loblength = b_lob.getLength();
            if(loblength != b_length_value) {
                logerror("LENGTH() result and LOB length differ for LOB %d: %d and %d", i, b_length_value, loblength);
                break;
            }
            b_lob.close();
            if(!m_success) {
                break;
            }
        }
        //dropTable("LOBLENGTH);
        return m_success;
    };
};


int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new InsertLOB());
  testframe.addTestCase (new LOBLength());
  if (testframe.run ())
      return 0;

  return 1;
}

