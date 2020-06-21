/*!================================================================
 module:    MassNavigation.cpp

 responsible: D030044

 special area: Resultset
 description:  Tests result set cursor mass navigation.

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

class MassNavigation : public IFR_TestCase
{
public:
  MassNavigation ()
    : IFR_TestCase ("MassNavigation")
  {}

  virtual IFR_Bool run ()
  {
    dropTable ("MASSNAVIGATION");
    m_success = m_success && execSQL ("CREATE TABLE MASSNAVIGATION (A INTEGER, B CHAR(1000) ASCII)");

    int insertedRows[] = { 200, 500 };
    int rowsetSizes[] = { 20, 40, 1, 50, 10, 66, -1 };

    for (int j = 0; j < sizeof (insertedRows) / sizeof (int); j++) {
        this->insertRows (insertedRows[j]);
        this->fetchAbsolute (insertedRows[j], rowsetSizes);
         this->fetchNext (insertedRows[j], rowsetSizes);
         this->fetchPrevious (insertedRows[j], rowsetSizes);
        this->fetchLast (insertedRows[j], rowsetSizes);
        this->fetchRelative (insertedRows[j], rowsetSizes);
    }

    execSQL ("COMMIT WORK");
    return m_success;
  }

private:
  void insertRows (int insertedRows) {
    IFR_Retcode rc;
    int i;
    
    IFR_String a256 ("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaa", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    execSQL ("DELETE FROM MASSNAVIGATION");
    IFR_String cmdInsert ("INSERT INTO MASSNAVIGATION VALUES (?, ?)", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_INT4, &i, 0, 4);
    tc_errorcheck (stmtPrepare)->bindParameter (2, IFR_HOSTTYPE_ASCII, (void*)a256.getBuffer(), 
						0, TEST_STRING_LEN);
    for (i = 1; i <= insertedRows; i++) {
      char rr[6];
      int bw;
      bw = sprintf (rr, "%5d", i);
      memcpy ((void *) a256.getBuffer(), rr, bw); 
      rc = tc_errorcheck (stmtPrepare)->execute ();
      if (rc != IFR_OK) {
          logerror (stmtPrepare->error().getErrorText ());
      }
    }
    connection()->releaseStatement(stmtPrepare);
  }

  void ClearData (IFR_Int4 *idata, char *cdata, int insertedRows) {
      memset (idata, 0, insertedRows*sizeof(IFR_Int4));
      memset (cdata, 0, insertedRows*TEST_STRING_LEN*sizeof(char));
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
    if (rs->getFetchedRows () < (unsigned) rssize)
      rssize = rs->getFetchedRows () / 2;

    if (rowset->setPos (rssize) != IFR_OK)
      logerror ("Error in setPos.");
    checkObject (rowset, start+rssize-1);    
  }

  void checkValues (int row, IFR_ResultSet *rs, IFR_Int4 *idata, char *cdata, int clen) 
    {
        unsigned int i;
        if (rs->getFetchedRows () == 0) {
            logerror ("Error: getFetchedRows == 0.");
            return;
        }
        for (i = row; i < row+rs->getFetchedRows (); i++) {
            char rr[8];
            int bw;
            if (idata[i-row] != i) {
                logerror ("Error in values.");
                break;
            }
            bw = sprintf (rr, "%5d", i);
            if (memcmp (cdata+(i-row)*clen, rr, bw)) {
                logerror ("Error in values.");
                break;	
            }
        }
    }

  void fetchAbsolute (int insertedRows, int rowsetSizes[]) {
      int i;
      IFR_Retcode rc;
      char *cdata = new char[insertedRows*TEST_STRING_LEN];
      IFR_Length *ind = new IFR_Length[insertedRows];
      IFR_Int4 *idata = new IFR_Int4[insertedRows];
      const char* cmdSelect = "SELECT A, B FROM MASSNAVIGATION ORDER BY A";
      IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
      IFR_ResultSet *rs;
      IFR_RowSet *rowset;
      stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
      rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
      if (rc == IFR_NOT_OK)
          logerror (stmt->error().getErrorText ());
      if (stmt->getResultSet() != 0) {
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
          int rssize = rowsetSizes[i] > 1 ? rowsetSizes[i] / 2 : 1;
          rs->setRowSetSize (rowsetSizes[i]);
          rowset = rs->getRowSet ();
          ClearData (idata, cdata, insertedRows);
          if (rs->first () != IFR_OK)
              logerror ("Error in fetch first.");
          if (rowset->fetch () == IFR_NOT_OK)
              logerror ("Error in fetch.");
          checkValues (1, rs, idata, cdata, TEST_STRING_LEN);
          ClearData (idata, cdata, insertedRows);
          
          testSetPos (rs, rowset, rssize, 1);
          
          if (rs->absolute (insertedRows/4) != IFR_OK)
              logerror ("Error in fetch absolute.");
          if (rowset->fetch () == IFR_NOT_OK)
              logerror ("Error in fetch.");
          checkValues (insertedRows/4, rs, idata, cdata, TEST_STRING_LEN);
          ClearData (idata, cdata, insertedRows);
          
          testSetPos (rs, rowset, rssize, insertedRows/4);
          
          if (rs->absolute (insertedRows*3/4) != IFR_OK)
              logerror ("Error in fetch absolute.");
          if (rowset->fetch () == IFR_NOT_OK)
              logerror ("Error in fetch.");
          checkValues (insertedRows*3/4, rs, idata, cdata, TEST_STRING_LEN);
          ClearData (idata, cdata, insertedRows);
          
          testSetPos (rs, rowset, rssize, insertedRows*3/4);
          
          if (rs->absolute (-insertedRows/4) != IFR_OK)
              logerror ("Error in fetch absolute.");
          if (rowset->fetch () == IFR_NOT_OK)
              logerror ("Error in fetch.");
          checkValues (insertedRows-insertedRows/4+1, rs, idata, cdata, TEST_STRING_LEN);
          ClearData (idata, cdata, insertedRows);
          
          testSetPos (rs, rowset, rssize, insertedRows-insertedRows/4+1);
          i++;
      }
      rs->close ();
      delete[] ind;
      delete[] idata;
      delete[] cdata;
  }

  void fetchNext (int insertedRows, int rowsetSizes[]) {
    int i;
    IFR_Retcode rc;
    char *cdata = new char[insertedRows*TEST_STRING_LEN];
    IFR_Length *ind = new IFR_Length[insertedRows];
    IFR_Int4 *idata = new IFR_Int4[insertedRows];
    const char * cmdSelect = "SELECT A, B FROM MASSNAVIGATION ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    IFR_ResultSet *rs;
    IFR_RowSet *rowset;
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
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
      int rssize = rowsetSizes[i] > 1 ? rowsetSizes[i] / 2 : 1;
      rs->setRowSetSize (rowsetSizes[i]);
      rowset = rs->getRowSet ();
      ClearData (idata, cdata, insertedRows);
      if (rs->first () != IFR_OK)
	logerror ("Error in fetch first.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 1);
      
      if (rs->next () != IFR_OK)
	logerror ("Error in fetch next.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (rowsetSizes[i]+1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, rowsetSizes[i]+1);

      if (rs->next () != IFR_OK)
	logerror ("Error in fetch next.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (2*rowsetSizes[i]+1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 2*rowsetSizes[i]+1);

      if (rs->next () != IFR_OK)
	logerror ("Error in fetch next.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (3*rowsetSizes[i]+1, rs, idata, cdata, TEST_STRING_LEN);

      testSetPos (rs, rowset, rssize, 3*rowsetSizes[i]+1);

      i++;
    }
    rs->close ();
    delete[] ind;
    delete[] idata;
    delete[] cdata;
  }

  void fetchPrevious (int insertedRows, int rowsetSizes[]) {
    int i;
    IFR_Retcode rc;
    char *cdata = new char[insertedRows*TEST_STRING_LEN];
    IFR_Length *ind = new IFR_Length[insertedRows];
    IFR_Int4 *idata = new IFR_Int4[insertedRows];
    const char *cmdSelect = "SELECT A, B FROM MASSNAVIGATION ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    IFR_ResultSet *rs;
    IFR_RowSet *rowset;
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    if (stmt->isQuery()) {
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
      int rssize = rowsetSizes[i] > 1 ? rowsetSizes[i] / 2 : 1;
      rs->setRowSetSize (rowsetSizes[i]);
      rowset = rs->getRowSet ();
      ClearData (idata, cdata, insertedRows);
      if (rs->first () != IFR_OK)
	logerror ("Error in fetch first.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 1);

      if (rs->next () != IFR_OK)
	logerror ("Error in fetch next.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (rowsetSizes[i]+1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, rowsetSizes[i]+1);

      if (rs->next () != IFR_OK)
	logerror ("Error in fetch next.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (2*rowsetSizes[i]+1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 2*rowsetSizes[i]+1);

      if (rs->previous () != IFR_OK)
	logerror ("Error in fetch previous.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (rowsetSizes[i]+1, rs, idata, cdata, TEST_STRING_LEN);

      testSetPos (rs, rowset, rssize, rowsetSizes[i]+1);

      i++;
    }
    rs->close ();
    delete[] ind;
    delete[] idata;
    delete[] cdata;
  }

  void fetchLast (int insertedRows, int rowsetSizes[]) {
    int i;
    IFR_Retcode rc;
    char *cdata = new char[insertedRows*TEST_STRING_LEN];
    IFR_Length *ind = new IFR_Length[insertedRows];
    IFR_Int4 *idata = new IFR_Int4[insertedRows];
    const char *cmdSelect = "SELECT A, B FROM MASSNAVIGATION ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    IFR_ResultSet *rs;
    IFR_RowSet *rowset;
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
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
      int rssize = rowsetSizes[i] > 1 ? rowsetSizes[i] / 2 : 1;
      rs->setRowSetSize (rowsetSizes[i]);
      rowset = rs->getRowSet ();
      ClearData (idata, cdata, insertedRows);
      if (rs->first () != IFR_OK)
	logerror ("Error in fetch first.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 1);

      if (rs->last () != IFR_OK)
	logerror ("Error in fetch last.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (insertedRows-rowsetSizes[i]+1, rs, idata, cdata, TEST_STRING_LEN);

      testSetPos (rs, rowset, rssize, insertedRows-rowsetSizes[i]+1);

      i++;
    }
    rs->close ();
    delete[] ind;
    delete[] idata;
    delete[] cdata;
  }

  void fetchRelative (int insertedRows, int rowsetSizes[]) {
    int i;
    IFR_Retcode rc;
    char *cdata = new char[insertedRows*TEST_STRING_LEN];
    IFR_Length *ind = new IFR_Length[insertedRows];
    IFR_Int4 *idata = new IFR_Int4[insertedRows];
    const char *cmdSelect = "SELECT A, B FROM MASSNAVIGATION ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    IFR_ResultSet *rs;
    IFR_RowSet *rowset;
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
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
      int rssize = rowsetSizes[i] > 1 ? rowsetSizes[i] / 2 : 1;
      rs->setRowSetSize (rowsetSizes[i]);
      rowset = rs->getRowSet ();
      ClearData (idata, cdata, insertedRows);
      if (rs->first () != IFR_OK)
	logerror ("Error in fetch first.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (1, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 1);

      if (rs->relative (2) != IFR_OK)
	logerror ("Error in fetch relative.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (3, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 3);

      if (rs->relative (-1) != IFR_OK)
	logerror ("Error in fetch relative.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (2, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 2);

      if (rs->relative (insertedRows/2) != IFR_OK)
	logerror ("Error in fetch relative.");
      if (rowset->fetch () == IFR_NOT_OK)
	logerror ("Error in fetch.");
      checkValues (2+insertedRows/2, rs, idata, cdata, TEST_STRING_LEN);
      ClearData (idata, cdata, insertedRows);

      testSetPos (rs, rowset, rssize, 2+insertedRows/2);

      i++;
    }
    rs->close ();
    delete[] ind;
    delete[] idata;
    delete[] cdata;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new MassNavigation ());
  if (testframe.run ()) 
    return 0;

  return 1;
}

