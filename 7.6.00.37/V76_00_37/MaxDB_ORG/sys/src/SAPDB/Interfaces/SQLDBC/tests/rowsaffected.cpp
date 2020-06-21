/*!
  @file           rowsaffected.cpp
  @author         D030044
  @ingroup        Tests
  @brief          Checks getRowsAffected() function.

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

class RowsAffectedTest
  : public SQLDBC_TestCase
{
public:
  RowsAffectedTest()
    : SQLDBC_TestCase("RowsAffectedTest")
  {}
  
  virtual SQLDBC_Bool run() {
    SQLDBC_Statement *stmt = connection()->createStatement();
    SQLDBC_PreparedStatement *pStmt = connection()->createPreparedStatement();
    int id1[100];
    int id2[200];
    int rowsaff, i;
    
    dropTable("int_test");
    m_success = m_success && execSQL("create table int_test (I1 INTEGER, I2 INTEGER, PRIMARY KEY(I1))");

    tc_errorcheck(pStmt)->prepare("insert into int_test values (?, ?)");
    
    tc_errorcheck(pStmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, id1, NULL, 0);
    tc_errorcheck(pStmt)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, id2, NULL, 0);
    
    tc_errorcheck(pStmt)->setBatchSize(100);
    
    for (i = 0; i < 100; i++) {
      id1[i] = i;
      id2[i] = i;
    }
    
    tc_errorcheck(pStmt)->execute();
    rowsaff = pStmt->getRowsAffected();
    if (rowsaff == 100) {
      loginfo("%d Records inserted\n", rowsaff);
    } else {
      logerror("Wrong row count %d. Expected 100", rowsaff);
    }
    
    connection()->commit();
    
    for (i = 0; i < 100; i++) {
      id1[i] = 150-i;
      id2[i] = 150-i;
    }
    
    tc_expectederrorcheck(pStmt, 200)->execute();
    rowsaff = pStmt->getRowsAffected();
    if (rowsaff == 51) {
      loginfo("%d Records inserted\n", rowsaff);
    } else {
      logerror("Wrong row count %d. Expected 51", rowsaff);
    }
    
    connection()->commit();

    return m_success;
  }
};

class RowNotFound
  : public SQLDBC_TestCase
{
public:
  RowNotFound()
    : SQLDBC_TestCase("RowNotFound")
  {}
  
  virtual SQLDBC_Bool run() {
    int rowsaff;

    char *sel = (char*)"SELECT * FROM DUAL where dummy = '0'";

    SQLDBC_PreparedStatement *pStmt = connection()->createPreparedStatement();

    tc_errorcheck(pStmt)->prepare(sel);
    
    tc_errorcheck(pStmt)->execute();
    rowsaff = pStmt->getRowsAffected();
    if (rowsaff != 0)
      logerror("Wrong row count %d. Expected 0", rowsaff);      


    sel = (char*)"SELECT * FROM DUAL where 0 = 1";
    tc_errorcheck(pStmt)->prepare(sel);
    
    tc_errorcheck(pStmt)->execute();
    rowsaff = pStmt->getRowsAffected();
    if (rowsaff != 0)
      logerror("Wrong row count %d. Expected 0", rowsaff);      

    sel = (char*)"SELECT * FROM DUAL where rowno <= 1";
    tc_errorcheck(pStmt)->prepare(sel);
    
    tc_errorcheck(pStmt)->execute();
    rowsaff = pStmt->getRowsAffected();
    if (rowsaff != 1)
      logerror("Wrong row count %d. Expected 1", rowsaff);      

    SQLDBC_ResultSet *rs = tc_errorcheck(pStmt)->getResultSet();
    tc_errorcheck(rs)->next();

    tc_errorcheck(pStmt)->execute();


    return m_success;

  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new RowsAffectedTest ());
  testframe.addTestCase (new RowNotFound());
  if (testframe.run ()) 
      return 0;
  
  return 1;
}
