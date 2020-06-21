/*!================================================================
 module:    RowSetPos.cpp

 responsible: D030044

 special area: Rowset
 description:  Tests positioning on rowset by setPos().

 see:
 
 change history:


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




 ===================================================================*/

#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"

#define TEST_STRING_LEN 256

IFR_Bool memory_ok_ignored=true;

class RowSetPos : public IFR_TestCase
{
public:
  RowSetPos ()
    : IFR_TestCase ("RowSetPos")
  {}

  virtual IFR_Bool run ()
  {
    dropTable ("ROWSETPOS");
    m_success = m_success && execSQL ("CREATE TABLE ROWSETPOS (A INTEGER, B CHAR(1000) ASCII)");

    int insertedRows = 500;
    int rowsetSizes[] = { 20, 40, 1, 50, 10, 66, -1 };

    this->insertRows (insertedRows);
    this->testSetPos (insertedRows, rowsetSizes);

    execSQL ("COMMIT WORK");
    return m_success;
  }

private:
  enum NavDir {
    first_C,
    last_C,
    next_C,
    previous_C,
    absolute_C,
    relative_C
  };

  void insertRows (int insertedRows) {
    IFR_Retcode rc;
    int i;

    IFR_String a256 ("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaa", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    execSQL ("DELETE FROM ROWSETPOS");
    IFR_String cmdInsert ("INSERT INTO ROWSETPOS VALUES (?, ?)",
                          IFR_StringEncodingAscii,
                          getAllocator(), memory_ok_ignored);
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_INT4, &i, 0, 4);
    tc_errorcheck (stmtPrepare)->bindParameter (2, IFR_HOSTTYPE_ASCII, (void*)a256.getBuffer(), 
						0, TEST_STRING_LEN);
    for (i = 1; i <= insertedRows; i++) {
      char rr[5];
      int bw;
      bw = sprintf (rr, "%5d", i);
      memcpy ((void *) a256.getBuffer(), rr, bw); 
      rc = tc_errorcheck (stmtPrepare)->execute ();
      if (rc != IFR_OK) {
          logerror (stmtPrepare->error().getErrorText ());
      }
    }
  }

  void checkObject (IFR_RowSet *rowset, IFR_Int4 refval) {
    IFR_Int4 val;
    IFR_Length ind;

    val = 0;
    if (rowset->getObject (1, &val, sizeof (val), IFR_HOSTTYPE_INT4, &ind, false) != IFR_OK)
      logerror ("Error in getObject.");
    if (val != refval)
      logerror ("Wrong value returned by getObject().");
    
  }

  void testSetPos (IFR_ResultSet *rs, IFR_RowSet *rowset, int rssize, int start) {
    if (rssize == 0)
      rssize = 1;
    if (rowset->setPos (rssize) != IFR_OK)
       logerror ("Error in setPos.");
    checkObject (rowset, start+rssize-1);    
  }

  void navigateRowSet (IFR_ResultSet *rs, IFR_RowSet *rowset, int rssize, int start) {
    testSetPos (rs, rowset, rssize/2, start);
    testSetPos (rs, rowset, rssize/3, start);
    testSetPos (rs, rowset, 1, start); 
    testSetPos (rs, rowset, 2*rssize/3, start);    
  }

  void navigateResultSet (IFR_ResultSet *rs, IFR_RowSet *rowset, int ind, enum NavDir dir) {
    IFR_Retcode rc = IFR_OK;

    switch (dir) {
    case first_C:
      rc = rs->first ();
      break;
    case last_C:
      rc = rs->last ();
      break;
    case next_C:
      rc = rs->next ();
      break;
    case previous_C:
      rc = rs->previous ();
      break;
    case absolute_C:
      rc = rs->absolute (ind);
      break;
    case relative_C:
      rc = rs->relative (ind);
      break;
    default:
      rc = IFR_NOT_OK;
      break;
    }
    if (rc == IFR_OK) {
      rc = rowset->fetch ();
    } else {
      logerror ("Error in navigate.");
    }
    if (rc != IFR_OK) {
      logerror ("Error in fetch.");
    }
  }

  void testSetPos (int insertedRows, int rowsetSizes[]) {
    int i;
    IFR_Retcode rc;
    char *cdata = new char[insertedRows*TEST_STRING_LEN];
    IFR_Length *ind = new IFR_Length[insertedRows];
    IFR_Int4 *idata = new IFR_Int4[insertedRows];
    const char * cmdSelect = "SELECT A, B FROM ROWSETPOS ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    IFR_ResultSet *rs;
    IFR_RowSet *rowset;
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    if (stmt->isQuery ()) {
      rs = stmt->getResultSet ();
    } else {
      logerror ("Error. No resultset returned.");
    }

    memset (ind, 0, sizeof(IFR_Length)*insertedRows);
    if (rs->bindColumn (1, IFR_HOSTTYPE_INT4, idata, ind, 4) != IFR_OK)
      logerror ("Error in bind column.");
    
    if (rs->bindColumn (2, IFR_HOSTTYPE_ASCII, cdata, ind, TEST_STRING_LEN) != IFR_OK)
      logerror ("Error in bind column.");

    i = 0;
    while (rowsetSizes[i] != -1) {
      rs->setRowSetSize (rowsetSizes[i]);
      rowset = rs->getRowSet ();

      navigateResultSet (rs, rowset, 0, first_C);

      navigateRowSet (rs, rowset, rowsetSizes[i], 1);

      navigateResultSet (rs, rowset, 0, next_C);
      navigateResultSet (rs, rowset, 0, next_C);

      navigateRowSet (rs, rowset, rowsetSizes[i], 2*rowsetSizes[i]+1);

      navigateResultSet (rs, rowset, insertedRows-rowsetSizes[i], absolute_C);

      navigateRowSet (rs, rowset, rowsetSizes[i], insertedRows-rowsetSizes[i]);

      if (rowset->setPos (rowsetSizes[i]+1) == IFR_OK)
	logerror ("Error in setPos(). setPos() should have failed.");

      if (rowset->setPos (0) == IFR_OK)
	logerror ("Error in setPos(). setPos() should have failed.");

      navigateResultSet (rs, rowset, 0, previous_C);

      navigateRowSet (rs, rowset, rowsetSizes[i], insertedRows-2*rowsetSizes[i]);

      navigateResultSet (rs, rowset, 0, first_C);
      navigateResultSet (rs, rowset, 2, relative_C);

      navigateRowSet (rs, rowset, rowsetSizes[i], 3);

      i++;
    }
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new RowSetPos ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
