/*!
  @file           allrowset.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Checks the methods of the SQLDBC_RowSet navigation.

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

#define DATA_SIZE 5000


class RowSetFeatures
    : public SQLDBC_TestCase
{
public:
    RowSetFeatures()
    :SQLDBC_TestCase("RowSetFeatures")
    {}

    virtual SQLDBC_Bool prepare()
    {
        loginfo("prepare");
        dropTable("SQLDBC_RESULTSET");
        m_success = m_success && execSQL("CREATE TABLE SQLDBC_RESULTSET(K INTEGER PRIMARY KEY, V CHAR(3000))");
        SQLDBC_Int4 i;
        SQLDBC_Int4 iarray[DATA_SIZE];
        SQLDBC_Length ilength=sizeof(SQLDBC_Int4);
        SQLDBC_PreparedStatement *insert_statement=tc_errorcheck(connection())->createPreparedStatement();
        insert_statement->prepare("INSERT INTO SQLDBC_RESULTSET (K) VALUES (?)");
        tc_errorcheck(insert_statement)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &iarray, &ilength, sizeof(SQLDBC_Int4));
        for(i=1; i <= DATA_SIZE; ++i) {
            iarray[i-1]=i;
        }
        tc_errorcheck(insert_statement)->setBatchSize(DATA_SIZE);
        tc_errorcheck(insert_statement)->executeBatch();
       connection()->commit();
        connection()->releaseStatement(insert_statement);
        loginfo("prepare finished");
        return m_success;
    }
    
    // we have the following variables:
    // - rowset size
    // - fetch size
    // - result set size


    virtual SQLDBC_Bool run()
    {
        m_success = runCommonNavigation(1, 1, 100);
        m_success = runCommonNavigation(100, 100, 100);
        m_success = runCommonNavigation(100, 10, 100);
        m_success = runCommonNavigation(10, 100, 100);
        return m_success;
    }


    
    
    SQLDBC_Bool runCommonNavigation(int rowsetsize,
                                    int fetchsize,
                                    int resultsetsize)
    {
        loginfo("Testing Navigation with rowset size %d, fetch size %d on a %d row result set", rowsetsize, fetchsize, resultsetsize);
        runCommonFirst(rowsetsize, fetchsize, resultsetsize);
        runCommonLast(rowsetsize, fetchsize, resultsetsize);
        runSetPos(rowsetsize, fetchsize, resultsetsize);
        return m_success;
    }
    
    SQLDBC_Bool runCommonFirst(int rowsetsize,
                               int fetchsize,
                               int resultsetsize)
    {
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->prepare("SELECT * FROM SQLDBC_RESULTSET WHERE K <= ?");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &resultsetsize, 0,0);
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs=tc_errorcheck(ps)->getResultSet();
        SQLDBC_Int4 *rsarray = (SQLDBC_Int4 *) alloca (sizeof (SQLDBC_Int4) * rowsetsize);
        int i=0;
        for(i=0; i<rowsetsize; ++i) {
            rsarray[i] = -1;
        }
        tc_errorcheck(rs)->setRowSetSize(rowsetsize);
        tc_errorcheck(rs)->setFetchSize(fetchsize);
        tc_errorcheck(rs)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, rsarray, 0, 0);
        if(tc_errorcheck(rs)->first() == SQLDBC_NO_DATA_FOUND) {
            logerror("Expected data in call to first()");
            connection()->releaseStatement(ps);
            
        }
        rs->getRowSet()->fetch();
        for(i=1; i<=rowsetsize; ++i) {
            if(i != rsarray[i-1]) {
                logerror("runCommonFirst: Expected %d at its position, found %d.", i, rsarray[i-1]);
                break;
            }
        }
        return m_success;
    }

    SQLDBC_Bool runSetPos(int rowsetsize,
                          int fetchsize,
                          int resultsetsize)
    {
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->prepare("SELECT * FROM SQLDBC_RESULTSET WHERE K <= ?");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &resultsetsize, 0,0);
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs=tc_errorcheck(ps)->getResultSet();
        SQLDBC_Int4 *rsarray = (SQLDBC_Int4 *) alloca (sizeof (SQLDBC_Int4) * rowsetsize);
        int i=0;
        for(; i<rowsetsize; ++i) {
            rsarray[i] = -1;
        }
        tc_errorcheck(rs)->setRowSetSize(rowsetsize);
        tc_errorcheck(rs)->setFetchSize(fetchsize);
        if(tc_errorcheck(rs)->first() == SQLDBC_NO_DATA_FOUND) {
            logerror("Expected data in call to first()");
            connection()->releaseStatement(ps);
            
        }
        for(i=1; i<=rowsetsize; ++i) {
            SQLDBC_RowSet *rowset=rs->getRowSet();
            rowset->setPos(i);
            SQLDBC_Int4 value;
            getInteger(rowset, value);
            if(i != value) {
                logerror("runSetPos: Expected %d at its position, found %d.", i, value);
                break;
            }
        }
        return m_success;
    }

    
    SQLDBC_Bool runCommonLast(int rowsetsize,
                              int fetchsize,
                              int resultsetsize)
    {
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->prepare("SELECT * FROM SQLDBC_RESULTSET WHERE K <= ?");
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &resultsetsize, 0,0);
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs=tc_errorcheck(ps)->getResultSet();
        SQLDBC_Int4 *rsarray = (SQLDBC_Int4 *) alloca (sizeof (SQLDBC_Int4) * rowsetsize);
        int i=0;
        for(i=0; i<rowsetsize; ++i) {
            rsarray[i] = -1;
        }
        tc_errorcheck(rs)->setRowSetSize(rowsetsize);
        tc_errorcheck(rs)->setFetchSize(fetchsize);
        tc_errorcheck(rs)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, rsarray, 0, 0);
        if(tc_errorcheck(rs)->last() == SQLDBC_NO_DATA_FOUND) {
            logerror("Expected data in call to last()");
            connection()->releaseStatement(ps);
        }
        rs->getRowSet()->fetch();
        for(i=1; i<=rowsetsize; ++i) {
            if(i + (resultsetsize - rowsetsize) != rsarray[i-1]) {
                logerror("runCommonLast: Expected %d at position %i, found %d.", i+resultsetsize-rowsetsize, i, rsarray[i-1]);
                break;
            }
        }
        
        return m_success;
    }

    

    
    void getInteger(SQLDBC_ResultSet* rs, SQLDBC_Int4& value)
    {
        tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &value, 0, 0);
    }

    void getInteger(SQLDBC_RowSet* rs, SQLDBC_Int4& value)
    {
        tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &value, 0, 0);
    }

};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new RowSetFeatures());
  if (testframe.run ()) 
      return 0;
  
  return 1;
}


