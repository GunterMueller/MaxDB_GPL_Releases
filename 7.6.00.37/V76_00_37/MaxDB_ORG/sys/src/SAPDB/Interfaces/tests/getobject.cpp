/*!================================================================
 module:    getobject.cpp

 responsible: D030044

 special area: 
 description:  Test program for piecewise reading of ascii/binary data

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

IFR_Bool memory_ok_ignored=true;

class CharAsciiTest : public IFR_TestCase
{
public:
  CharAsciiTest (char *test = "CharAsciiTest")
    :IFR_TestCase (test)
  {}

  virtual IFR_Bool run()
  {
    char str1[40];
    char str2[50];

    makeCharBuffer (str1, 40, false);
    makeCharBuffer (str2, 50, true);

    tab_insert (300, str1, 40, str2, 50);
    tab_select (300, str1, 40, str2, 50);

    execSQL("COMMIT WORK");
    return m_success;
  }

  IFR_Bool tab_insert (IFR_Int4 rows, char* str1, IFR_Int4 len1, char *str2, IFR_Int4 len2)
  {
    int i;
    IFR_Retcode rc;
    IFR_String cmdInsert ("INSERT INTO CHARASCII_TEST VALUES (?, ?, ?)", 
        IFR_StringEncodingAscii,
                          getAllocator(), memory_ok_ignored);
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();

    dropTable("CHARASCII_TEST");
    m_success = m_success && execSQL("CREATE TABLE CHARASCII_TEST (K INT, V CHAR(40) ASCII, W VARCHAR(50) ASCII)");
    
    IFR_Int4 *intarr = new int[rows];
    char *p1, *p2;
    char *testStr1 = new char[len1*rows];
    char *testStr2 = new char[len2*rows];
    IFR_Length *ind0 = new IFR_Length[rows];
    IFR_Length *ind1 = new IFR_Length[rows];
    IFR_Length *ind2 = new IFR_Length[rows];

    memset (ind0, 0, sizeof (IFR_Length)*rows);
    memset (ind1, 0, sizeof (IFR_Length)*rows);
    memset (ind2, 0, sizeof (IFR_Length)*rows);

    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_INT4, intarr, ind0, sizeof (IFR_Int4));
    tc_errorcheck (stmtPrepare)->bindParameter (2, IFR_HOSTTYPE_ASCII, testStr1, ind1, len1);
    tc_errorcheck (stmtPrepare)->bindParameter (3, IFR_HOSTTYPE_ASCII, testStr2, ind2, len2);
    tc_errorcheck (stmtPrepare)->setRowArraySize (rows);
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());

    p1 = testStr1;
    p2 = testStr2;
    for (i = 0; i < rows; i++) {
      ind1[i] = len1;
      ind2[i] = len2;
      intarr[i] = i+1;

      memcpy (p1, str1, len1);
      memcpy (p2, str2, len2);

      p1 += len1;
      p2 += len2;
    }

    rc = tc_errorcheck (stmtPrepare)->executeBatch ();
    if (rc != IFR_OK) {
      logerror ("Error in executeBatch ()");
    }
    const IFR_Int4 *statusarray = stmtPrepare->getRowStatus();
    for (i = 0; i < rows; i++) {
      if (statusarray[i] != SQLDBC_SUCCESS_NO_INFO) {
	logerror ("Row status at %d contains %d but should contain %d ", i, statusarray[i], SQLDBC_SUCCESS_NO_INFO);
      }
    }

    execSQL ("COMMIT WORK");

    connection ()->releaseStatement (stmtPrepare);

    delete[] ind2;
    delete[] ind1;
    delete[] ind0;
    delete[] testStr2;
    delete[] testStr1;
    delete[] intarr;

    return m_success;
  }

  IFR_Bool tab_select (IFR_Int4 rows, char* str1, IFR_Int4 len1, char *str2, IFR_Int4 len2)
  {
    IFR_Retcode rc;
    char cbuf[11];
    char buf[10000];
    char buf2[40000];
    char *c1data = new char[rows*len1];
    char *c2data = new char[rows*len2];
    IFR_Length *ind = new IFR_Length[rows];
    IFR_Int4 *idata = new IFR_Int4[rows];
    const char *cmdSelect = "SELECT K, V, W FROM CHARASCII_TEST";
    IFR_Statement *stmt;
    { stmt = tc_errorcheck (connection ())->createStatement (); }
    IFR_ResultSet *rs;
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    if (stmt->isQuery()) {
      rs = stmt->getResultSet ();
    } else {
      logerror ("Error. No resultset returned.");
    }

    memset (ind, 0, sizeof(IFR_Int4)*rows);
    rs->setRowSetSize (rows);

    if (rs->first () != IFR_OK)
      logerror ("Error in fetch first.");  

    IFR_RowSet *rowset = rs->getRowSet ();

    char *p1 = str1;
    char *p2 = str2;
    do {
      rc = rowset->getObject (3, cbuf, sizeof (cbuf), IFR_HOSTTYPE_ASCII, ind, true);
      if (memcmp (cbuf, p2, MIN(ind[0], sizeof (cbuf)-1)))
        logerror ("Error in getObject()");
      p2 += MIN(ind[0], sizeof (cbuf)-1);
    } while (rc == IFR_DATA_TRUNC);
    rc = rowset->getObject (3, cbuf, sizeof (cbuf), IFR_HOSTTYPE_ASCII, ind, true);


    if (rs->absolute (1) != IFR_OK)
      logerror ("Error in fetch next.");

    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rc = rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);
    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rc = rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);
    if (rc != IFR_NO_DATA_FOUND)
      logerror ("Error at getObject(). Wrong return code.");
    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rc = rowset->setPos (5);
    rc = rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);
    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rc = rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);
    if (rc != IFR_NO_DATA_FOUND)
      logerror ("Error at getObject(). Wrong return code.");

    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rowset->getObject (3, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);

    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rowset->getObject (3, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);

    memset (buf2, 0, sizeof (buf2));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rowset->getObject (2, buf2, sizeof (buf2), IFR_HOSTTYPE_ASCII, ind, true);
    memset (buf2, 0, sizeof (buf2));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rowset->getObject (2, buf2, sizeof (buf2), IFR_HOSTTYPE_ASCII, ind, true);
    memset (buf2, 0, sizeof (buf2));
    memset (ind, 0, sizeof (IFR_Length) * rows);
    rowset->getObject (2, buf2, sizeof (buf2), IFR_HOSTTYPE_ASCII, ind, true);

    if (rowset->setPos (1) != IFR_OK)
      logerror ("Error in setPos.");
    
    if (rs->last () != IFR_OK)
      logerror ("Error in fetch next.");

    rs->close ();

    delete[] ind;
    delete[] idata;
    delete[] c2data;
    delete[] c1data;

    return m_success;
  }

  void makeCharBuffer(char *result, IFR_size_t size, IFR_Bool upper)
  {
    char *udata= "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *ldata= "abcdefghijklmnopqrstuvwxyz";
    char *data;
    if (upper)
      data = udata;
    else
      data = ldata;
    for(IFR_size_t i=0; i<size; ++i) {
      result[i]=data[i%26];
    }
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe(argc, argv);
  testframe.addTestCase(new CharAsciiTest());
  if (testframe.run ()) 
    return 0;
  return 1;
}
