/*!
  @file           geroTemplate.cpp
  @author         D042427
  @ingroup        Tests
  @brief          Tests different params on SQLDBC::SQLDBC_ResultSet::bindColumn

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

#define myRowSetSize 6

class resultSetBindColumn : public SQLDBC_TestCase
{

public:
  resultSetBindColumn ()
    : SQLDBC_TestCase ("resultSetBindColumn")
  {}

  virtual SQLDBC_Bool prepare ()
  {
	  dropTable("MyTable");
	  execSQL("CREATE TABLE MyTable (id INTEGER)");

	  loginfo("preparing table...");

	  // fill table
	  for (int i=0; i<20; i++)	  
	  {
		  char insertstatement[1024];
		  sprintf(insertstatement, "INSERT INTO MyTable VALUES (%d)", i);
		  execSQL(insertstatement);
	  }

	  return m_success;
  }

  virtual SQLDBC_Bool release ()
  {
	  dropTable("MyTable");

	  return m_success;
  }

  virtual SQLDBC_Bool run ()
  {
	  SQLDBC_Retcode rc;

	  // query
      statement()->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
	  execSQL("SELECT id FROM MyTable");
      
	  // getting the result set
	  SQLDBC_ResultSet *res = statement()->getResultSet(); 
	  if (!res) logerror("No resultset returned");
	  tc_errorcheck(res)->next();

	  // 
	  res->setRowSetSize(myRowSetSize);

	  int myArray[100];
	  memset(&myArray, 0, sizeof(myArray));

	  SQLDBC_Length ind[myRowSetSize];
	  for (int i=0;i<myRowSetSize;i++)
		  ind[i] = SQLDBC_NTS;

	  SQLDBC_Retcode rc1 = res->bindColumn(-3, SQLDBC_HOSTTYPE_INT4, myArray, ind, sizeof(myArray));
	  if (rc1 == SQLDBC_OK) logerror("SQLDBC_ResultSet::bindColumn. Return value SQLDBC_OK for invalid parameter: Index = -3");

	  SQLDBC_Retcode rc2 = res->bindColumn(0, SQLDBC_HOSTTYPE_INT4, myArray, ind, sizeof(myArray));
	  if (rc2 == SQLDBC_OK) logerror("SQLDBC_ResultSet::bindColumn. Return value SQLDBC_OK for invalid parameter: Index = 0");

	  SQLDBC_Retcode rc3 = res->bindColumn(2, SQLDBC_HOSTTYPE_INT4, myArray, ind, sizeof(myArray));
	  if (rc3 == SQLDBC_OK) logerror("SQLDBC_ResultSet::bindColumn. Return value SQLDBC_OK for invalid parameter: Index = 2");

	  res->clearError(); 
	  // still a bug: current error will still be visible after the next call although that call will end successfully

	  tc_errorcheckrc(rc, res)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, myArray, ind, sizeof(myArray));

	  char myStr[1024];

      SQLDBC_RowSet *rowset = res->getRowSet();
	  tc_errorcheckrc(rc, rowset)->setPos(2); // check, if setPos() affects fetch()
	  tc_errorcheckrc(rc, rowset)->fetch();
	  if ((myArray[1] != 1) || (myArray[4] != 4))
	  {
		  sprintf(myStr, "Wrong data fetched.");
		  logerror(myStr);
	  }
	  
	  SQLDBC_Int4 raffected = res->getRowSet()->getRowsAffected();
	  if (raffected != myRowSetSize) 
	  {
		  sprintf(myStr, "After fetch(): Number of affected rows %d. Expected number %d", raffected, myRowSetSize);
		  logerror(myStr);
	  }

	  tc_errorcheckrc(rc, res)->next();
	  tc_errorcheckrc(rc, rowset)->setPos(6); // check, if setPos() affects fetch()
	  tc_errorcheckrc(rc, rowset)->fetch();
	  if ((myArray[2] != 8) || (myArray[5] != 11))
	  {
		  sprintf(myStr, "Wrong data fetched.");
		  logerror(myStr);
	  }
//	  sprintf(myStr, "2: %d %d %d %d", myArray[0], myArray[2], myArray[4], myArray[6]);
//	  loginfo(myStr);

	  int rCount = res->getResultCount(); // return value -1 [meaning: more than one]
	  if (rCount != -1 && rCount != 20) 
	  {
		  sprintf(myStr, "Result count = %d. Expected was -1 or 20", rCount);
		  logerror(myStr);
	  }

	  return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new resultSetBindColumn());
  if (testframe.run ()) 
    return 0;

  return 1;
}


