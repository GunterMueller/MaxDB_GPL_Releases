/*!
  @file           multiplefetch.cpp
  @author         D030044
  @ingroup        Tests
  @brief          Checks multiple calls of fetch on same rowset.

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

class MultipleFetchTest
  : public SQLDBC_TestCase
{
public:
  MultipleFetchTest()
    : SQLDBC_TestCase("MultipleFetchTest")
  {}

  virtual SQLDBC_Bool run() {
    fetch_insert (1000, 100);
    fetch_select (10);
    fetch_select (111);
    fetch_select (5);

    return m_success;
  }

  SQLDBC_Bool fetch_insert (SQLDBC_Int4 rows, SQLDBC_Int4 strlength)
  {
    int i;
    SQLDBC_Retcode rc;
    SQLDBC_PreparedStatement *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();

    dropTable ("MFETCHTEST");
    m_success = m_success && execSQL ("CREATE TABLE MFETCHTEST (I INTEGER, A VARCHAR(410) ASCII)");

    char *str1;
    SQLDBC_Int4 *intarr = new SQLDBC_Int4[rows];
    char *testStr1 = new char[strlength*rows];
    SQLDBC_Length *ind0 = new SQLDBC_Length[rows];
    SQLDBC_Length *ind1 = new SQLDBC_Length[rows];

    memset (ind0, 0, sizeof (SQLDBC_Length)*rows);
    memset (ind1, 0, sizeof (SQLDBC_Length)*rows);

    tc_errorcheck (stmtPrepare)->bindParameter (1, SQLDBC_HOSTTYPE_INT4, intarr, ind0, 
						sizeof (SQLDBC_Int4));
    tc_errorcheck (stmtPrepare)->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, testStr1, ind1, strlength);
    tc_errorcheck (stmtPrepare)->setBatchSize (rows);
    tc_errorcheck (stmtPrepare)->prepare ("INSERT INTO MFETCHTEST VALUES (?, ?)");

    str1 = testStr1;
    for (i = 0; i < rows; i++) {
      char tb[6];
      memset (str1, 'a', strlength);
      sprintf (tb, "%5d", i+1);
      memcpy (str1, tb, 5);
      ind1[i] = strlength;
      intarr[i] = i+1;
      str1 += strlength;
    }

    rc = tc_errorcheck (stmtPrepare)->executeBatch ();
    if (rc != SQLDBC_OK) {
      logerror ("Error in executeBatch ()");
    }

    const SQLDBC_Int4 *statusarray = stmtPrepare->getRowStatus();
    for (i = 0; i < rows; i++) {
      if (statusarray[i] != SQLDBC_SUCCESS_NO_INFO) {
	logerror ("Row status at %d contains %d but should contain %d ", i, statusarray[i], SQLDBC_SUCCESS_NO_INFO);
      }
    }

    execSQL ("COMMIT WORK");

    connection ()->releaseStatement (stmtPrepare);

    delete[] ind1;
    delete[] ind0;
    delete[] intarr;
    delete[] testStr1;

    return m_success;
  }

  SQLDBC_Bool fetch_select (SQLDBC_Int4 rowsetsize)
  {    
    SQLDBC_Retcode rc;
    SQLDBC_Int4 *intarr = new SQLDBC_Int4[rowsetsize];
    SQLDBC_Length *ind1 = new SQLDBC_Length[rowsetsize];

    const char *cmdSelect = "SELECT I FROM MFETCHTEST";
    SQLDBC_Statement *stmt;
    { stmt = tc_errorcheck (connection ())->createStatement (); }
    stmt->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
    SQLDBC_ResultSet *rs;
    rc = stmt->execute (cmdSelect, SQLDBC_StringEncodingAscii);
    if (rc == SQLDBC_NOT_OK)
      logerror (stmt->error().getErrorText ());
    if (stmt->isQuery()) {
      rs = stmt->getResultSet ();
    } else {
      logerror ("Error. No resultset returned.");
    }

    memset (ind1, 0, sizeof(SQLDBC_Int4)*rowsetsize);
    
    if (rs->bindColumn (1, SQLDBC_HOSTTYPE_INT4, intarr, ind1, sizeof(SQLDBC_Int4)) != SQLDBC_OK)
       logerror ("Error in bind column 1.");

    rs->setRowSetSize (rowsetsize);

    if (rs->first () != SQLDBC_OK)
      logerror ("Error in fetch first.");  

    SQLDBC_RowSet *rowset = rs->getRowSet ();
    memset (intarr, 0, rowsetsize*sizeof(SQLDBC_Int4));
    tc_errorcheck(rowset)->fetch ();
    checkValues (intarr, rowsetsize, 1);

    tc_errorcheck(rowset)->setPos (rowsetsize/2);

    tc_errorcheck(rowset)->fetch ();
    checkValues (intarr, rowsetsize, 1);

    tc_errorcheck(rowset)->fetch ();
    checkValues (intarr, rowsetsize, 1);

    if (rs->next () != SQLDBC_OK)
      logerror ("Error in fetch next."); 
    memset (intarr, 0, rowsetsize*sizeof(SQLDBC_Int4));
    tc_errorcheck(rowset)->fetch ();
    checkValues (intarr, rowsetsize, rowsetsize+1);

    tc_errorcheck(rowset)->setPos (rowsetsize/3);

    tc_errorcheck(rowset)->fetch ();
    checkValues (intarr, rowsetsize, rowsetsize+1);

    if (rs->next () != SQLDBC_OK)
      logerror ("Error in fetch next."); 

    tc_errorcheck(rowset)->fetch ();
    checkValues (intarr, rowsetsize, 2*rowsetsize+1);
    memset (intarr, 0, rowsetsize*sizeof(SQLDBC_Int4));
    if (rs->next () != SQLDBC_OK)
      logerror ("Error in fetch next."); 
    memset (intarr, 0, rowsetsize*sizeof(SQLDBC_Int4));
    tc_errorcheck(rowset)->fetch ();
    checkValues (intarr, rowsetsize, 3*rowsetsize+1);

    return m_success;
  }

  void checkValues (SQLDBC_Int4 *intarr, SQLDBC_Int4 rowsetsize, SQLDBC_Int4 start) {
    int i;

    for (i = 0; i < rowsetsize; i++) {
      if (intarr[i] != start+i)
	logerror ("Error in row %d. Value is %d. Value expected: %d.\n",
		  i+1, intarr[i], start+i);
    }
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new MultipleFetchTest ());
  if (testframe.run ()) 
      return 0;
  
  return 1;
}
