/*!
  @file           rowset.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check rowset navigation without getting a new reference 
                  to the rowset object.
                  
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
#include <string.h>

#define ROWSETSIZE (3)
#define MAXROWS (10)

class RowSetNav;
class ResultSet  {
public:
  ResultSet(RowSetNav *testcase);
  bool next();
  bool first();
  bool last();
  bool previous();
  bool absolute(int row);  
  bool relative(int row);  
  bool CheckRow();

private:
  const static int Before;
  const static int Last;
  int m_actualrow;
  int m_rowsetsize;
  int m_resultsetsize;
  SQLDBC_ResultSet *m_resultset;
  SQLDBC_TestCase *m_testcase;
};


class RowSetNav : public SQLDBC_TestCase
{
public:
  RowSetNav ()
    : SQLDBC_TestCase ("RowSetNav")
  {}

  void Init() {
    dropTable("RowSetNav");
    m_success = m_success && execSQL("CREATE TABLE RowSetNav (A INT)");
    for (int i = 1; i <= MAXROWS && m_success; i++) {
      char sql[300];
      sprintf(sql, "INSERT INTO RowSetNav values (%d)", i);
      m_success = m_success && execSQL(sql);
    }
    m_stmt = connection()->createStatement();
    if(!m_stmt) {
      logerror("createStatement failed\n");
    }
    setMaxRows(MAXROWS);
  }

  bool executeQuery() {
    SQLDBC_Retcode rc;
    m_stmt->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
    {tc_errorcheckrc(rc, m_stmt)->execute("SELECT * from RowSetNav");}
    m_result = m_stmt->getResultSet();
    if(!m_result) {
      logerror("getResultSet failed\n");
    }
    m_result->setRowSetSize(ROWSETSIZE);
    loginfo("set RowSetSize to %d\n", ROWSETSIZE);
    return (!m_success);
  }

  void Free() {
    dropTable("RowSetNav");
  }

  void Sequence() {
    loginfo("Sequence\n");

    ResultSet t1(this);
    
    t1.next();   
    t1.next();   
    t1.next();
    t1.next();
    t1.next();
    t1.next();
    t1.first();
    t1.next();
    t1.last();
    t1.next();
    t1.first();
    t1.next();
    t1.previous();
    t1.previous();    
    t1.next();
    t1.absolute(2);  // row 2
    t1.relative(-1); // row 1 
    t1.relative(1);  // row 2
    t1.relative(-1); // row 1
    t1.relative(-1); // row not found
    t1.relative(-1); // row not found
    t1.relative(1);  // row 1

    t1.absolute(4);  // row 4
  }

  void FirstCall() {
    loginfo("FirstCall\n");

    ResultSet t0(this);
    t0.first();
    
    ResultSet t1(this);
    t1.next();
    
    ResultSet t2(this);
    t2.last();

    ResultSet t3(this);
    t3.previous();

    ResultSet t4(this);
    t4.absolute(3);

    ResultSet t5(this);
    t5.absolute(0);
    
    ResultSet t6(this);
    t6.absolute(11);

    ResultSet t7(this);
    t7.relative(3);    

    ResultSet t8(this);
    t8.relative(-3);    
  }

  virtual SQLDBC_Bool run ()
  {
    Init();
    FirstCall();
    Sequence();
    Free();
    return m_success;
  }

  void setResultSet(SQLDBC_ResultSet *result) { m_result = result; };
  SQLDBC_ResultSet *getResultSet() { return m_result; };

  void setMaxRows(int rows) { m_maxrows = rows; };
  int  getMaxRows() { return m_maxrows; };

private:
  SQLDBC_ResultSet *m_result;
  SQLDBC_Statement *m_stmt;
  int m_maxrows;
};

ResultSet::ResultSet(RowSetNav *testcase)
{
  testcase->executeQuery();
  m_actualrow = Before;
  m_rowsetsize = testcase->getResultSet()->getRowSetSize();
  m_resultsetsize = testcase->getMaxRows();
  m_resultset = testcase->getResultSet();
  m_testcase = testcase;
}

//-----------------------------
bool ResultSet::next() {
  SQLDBC_Retcode rc;
  m_testcase->loginfo("next()\n");
  
  if (m_actualrow == Last) {
    {
      rc=m_resultset->next();
    }
    if (rc != SQLDBC_NO_DATA_FOUND) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_NO_DATA_FOUND);
      return false;      
    }
    m_testcase->loginfo("rc = %d\n", rc);      
    return true;      
  }

  if (m_actualrow == Before)
    m_actualrow = 1;
  else {
    if (m_actualrow+m_rowsetsize > m_resultsetsize) {
      rc = m_resultset->next();
      if (rc != SQLDBC_NO_DATA_FOUND) {
        m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_NO_DATA_FOUND);
        return false;      
      }
      m_actualrow = Last;
      m_testcase->loginfo("rc = %d\n", rc);      
      return true;
    }
    m_actualrow += m_rowsetsize;
  }

  rc = m_resultset->next();
  if (rc != SQLDBC_OK) {
    m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_OK);
    return false;
  }
  else
    return CheckRow();
}
//-----------------------------
bool ResultSet::first() {
  SQLDBC_Retcode rc;
  m_testcase->loginfo("first()\n");
  m_actualrow = 1;
  rc = m_resultset->first();
  if (rc != SQLDBC_OK) {
    m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_OK);
    return false;
  }
  else
    return CheckRow();
}

//-----------------------------
bool ResultSet::last() {
  SQLDBC_Retcode rc;
  bool ret=true;
  m_testcase->loginfo("last()\n");
  m_actualrow = m_resultsetsize-m_rowsetsize+1;
  rc = m_resultset->last();
  if (rc != SQLDBC_OK) {
    m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_OK);
    ret = false;
  }
  else {
    ret = CheckRow();
    m_actualrow = Last;
  }
  return ret;
}

//-----------------------------
bool ResultSet::previous() {
  SQLDBC_Retcode rc;
  bool ret=true;
  m_testcase->loginfo("previous()\n");
  if (m_actualrow == Before) {
    rc = m_resultset->previous();
    if (rc != SQLDBC_NO_DATA_FOUND) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_NO_DATA_FOUND);
      return false;      
    }
    return true;
  }
  if (m_actualrow == Last) {
    m_actualrow = m_resultsetsize;
  }
  if (m_actualrow-m_rowsetsize > 0) {
    m_actualrow -= m_rowsetsize;
    rc = m_resultset->previous();
    if (rc != SQLDBC_OK) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_OK);
      return false;
    }
    ret = CheckRow();
  }
  else {
    m_actualrow = Before;
    rc = m_resultset->previous();
    if (rc != SQLDBC_NO_DATA_FOUND) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_OK);
      return false;
    }
    m_testcase->loginfo("rc = %d\n", rc);      
    ret = true;
  }
  return ret;
}

//-----------------------------
bool ResultSet::absolute(int row) {
  SQLDBC_Retcode rc;
  bool ret=true;
  m_testcase->loginfo("absolute(%d)\n", row);
  rc = m_resultset->absolute(row);
  if (row > m_resultsetsize) {
    m_testcase->loginfo("rc = %d\n", rc);      
    if (rc != SQLDBC_NO_DATA_FOUND) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_NO_DATA_FOUND);
      return false;
    }
    return true;
  }
  if (row <= 0) {
    m_testcase->loginfo("rc = %d\n", rc);      
    if (rc != SQLDBC_NO_DATA_FOUND) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_NO_DATA_FOUND);
      return false;
    }
    return true;
  }
  if (rc != SQLDBC_OK) {
    m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_OK);    
    return false;
  }
  m_actualrow = row;    
  ret = CheckRow();
  return ret;
}

//-----------------------------
bool ResultSet::relative(int row) {
  SQLDBC_Retcode rc;
  bool ret=true;
  int tmprow;
  m_testcase->loginfo("relative(%d)\n", row);
  rc = m_resultset->relative(row);
  tmprow = m_actualrow;
  if (m_actualrow == Before)
    tmprow = 0;
  if (m_actualrow == Last)
    tmprow = m_resultsetsize;
  if (tmprow+row > m_resultsetsize) {
    m_testcase->loginfo("rc = %d\n", rc);      
    if (rc != SQLDBC_NO_DATA_FOUND) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_NO_DATA_FOUND);
      return false;
    }
    m_actualrow = Last;
    return true;
  }
  if (tmprow+row <= 0) {
    m_testcase->loginfo("rc = %d\n", rc);      
    if (rc != SQLDBC_NO_DATA_FOUND) {
      m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_NO_DATA_FOUND);
      return false;
    }
    m_actualrow = Before;
    return true;
  }
  if (rc != SQLDBC_OK) {
    m_testcase->logerror("Wrong errorcode returned (%d) expected (%d)\n", rc, SQLDBC_OK);    
    return false;
  }
  m_actualrow = tmprow+row;    
  ret = CheckRow();
  return ret;
}
  
bool ResultSet::CheckRow() {
  int ival;
  SQLDBC_Length ind;
  // position the cursor
  SQLDBC_RowSet *rowset = m_resultset->getRowSet();
  if(!rowset) {
    m_testcase->logerror("getRowSet failed\n");
  }        
  rowset->getObject(1, SQLDBC_HOSTTYPE_INT4, &ival, &ind, sizeof(ival));
  m_testcase->loginfo("ival = %d\n", ival);
  if (ival != m_actualrow) {
    m_testcase->logerror("Wrong row (%d) expected (%d)\n", ival, m_actualrow);
    return false;
  }
  return true;
}

const int ResultSet::Before = -1;
const int ResultSet::Last   = -2;

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new RowSetNav());
  if (testframe.run ()) 
    return 0;

  return 1;
}


