/*!================================================================
 module:    Blot's001.pp

 responsible: D030044

 special area: Longs
 description:  Tests handling of blobs.

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



class Blobtst001 : public IFR_TestCase
{
public:
  Blobtst001 ()
    : IFR_TestCase ("Blobtst001")
  {}

    int my_memcmp(char *a, char *b, int size) {
        for(int i=0; i<size; ++i) {
            if(a[i] != b[i]) {
                logerror("DIFFERENCE AT POSITION: %d", i);
                logerror("A SNIPPET %.10s", (a + i - 5));
                logerror("B SNIPPET %.10s", (b + i - 5));
                logerror("THE FOUND CHAR IS %d", (int)b[i]);
                return a[i] - b[i];
            }
        }
        return 0;
    }


  virtual IFR_Bool run ()
  {
#if 0
    lob_insert (1, 10, 20);
    lob_insert (1, 20000, 20000);
    lob_insert (800, 100, 11);
    lob_insert (20, 50000, 11);
    lob_insert (20, 18000, 19000);
    lob_insert (1, 18000, 19000);
#endif
    lob_insert (20, 1024*1024, 64);
    // lob_insert (20, 40000, 64);
    lob_select (2, 40*1024, 20);

    return m_success;
  }

  void makeCharBuffer(char *result, IFR_size_t size, IFR_Bool upper)
  {
      char *udata= (char *) "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      char *ldata= (char *) "abcdefghijklmnopqrstuvwxyz";
      char *data;
      if (upper)
          data = udata;
      else
          data = ldata;
      for(IFR_size_t i=0; i<size; ++i) {
          result[i]=data[i%26];
      }
  }

    IFR_Bool lob_check (IFR_Int4 start, IFR_Int4 testcount, IFR_Int4 arraysize, IFR_Bool upper, char *str, IFR_Bool terminated, char *info)
  {
    int i;
    char *strp;
    char *cmpStr = new char[arraysize*testcount];

    strp = cmpStr;
    for (i = 0; i < testcount; i++) {
      char tb[6];

      makeCharBuffer (strp, arraysize, upper);

      sprintf (tb, "%5d", start);
      memcpy (strp, tb, 5);
      int sz = terminated?arraysize-1:arraysize;
      if (my_memcmp (strp, str+i*arraysize, sz)) {
          logerror("ERROR IN LOB CHECK %s TEST COUNT %d", info, (i+1));
      }
      strp += arraysize;
      start++;
    }

    delete[] cmpStr;

    return m_success;
  }
  
  IFR_Bool lob_insert (IFR_Int4 testcount, IFR_Int4 arraysize1, IFR_Int4 arraysize2)
    {
    int i;
    IFR_Retcode rc;
    IFR_String cmdInsert ("INSERT INTO BLOBTST001 VALUES (?, ?, ?, ?)", 
                          IFR_StringEncodingAscii,
                          getAllocator(), memory_ok_ignored);
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();

    dropTable ("BLOBTST001");
    m_success = m_success && execSQL ("CREATE TABLE BLOBTST001 (I INTEGER, A LONG BYTE, B LONG VARCHAR ASCII, C VARCHAR(30) ASCII)");
    
    IFR_Int4 *intarr = new int[testcount];
    char *str1, *str2, *str3;
    char *testStr1 = new char[arraysize1*testcount];
    char *testStr2 = new char[arraysize2*testcount];
    char *testStr3 = new char[27*testcount];
    IFR_Length *ind0 = new IFR_Length[testcount];
    IFR_Length *ind1 = new IFR_Length[testcount];
    IFR_Length *ind2 = new IFR_Length[testcount];
    IFR_Length *ind3 = new IFR_Length[testcount];
    IFR_Int4 t1len = arraysize2;
    IFR_Int4 t2len = IFR_NTS;    

    memset (ind0, 0, sizeof (IFR_Length)*testcount);
    memset (ind1, 0, sizeof (IFR_Length)*testcount);
    memset (ind2, 0, sizeof (IFR_Length)*testcount);
    memset (ind3, 0, sizeof (IFR_Length)*testcount);

    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_INT4, intarr, ind0, 
						sizeof (IFR_Int4));
    tc_errorcheck (stmtPrepare)->bindParameter (2, IFR_HOSTTYPE_BINARY, testStr1, ind1, 
						arraysize1);
    tc_errorcheck (stmtPrepare)->bindParameter (3, IFR_HOSTTYPE_ASCII, testStr2, ind2, 
						arraysize2);
    tc_errorcheck (stmtPrepare)->bindParameter (4, IFR_HOSTTYPE_ASCII, testStr3, ind3, 
						26);
    tc_errorcheck (stmtPrepare)->setRowArraySize (testcount);
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());

    str1 = testStr1;
    str2 = testStr2;
    str3 = testStr3;
    for (i = 0; i < testcount; i++) {
      char tb[6];

      makeCharBuffer (str1, arraysize1, true);
      makeCharBuffer (str2, arraysize2, false);
      makeCharBuffer (str3, 26, true);
      str3[26] = '\0';

      sprintf (tb, "%5d", i+1);
      memcpy (str1, tb, 5);
      memcpy (str2, tb, 5);
      ind1[i] = arraysize1;
      ind2[i] = arraysize2;
      ind3[i] = 26;
      intarr[i] = i+1;

      str1 += arraysize1;
      str2 += arraysize2;
      str3 += 26;
    }

    rc = tc_errorcheck (stmtPrepare)->executeBatch ();
    if (rc != IFR_OK) {
      logerror ("Error in executeBatch ()");
    }
    const IFR_Int4 *statusarray = stmtPrepare->getRowStatus();
    for (i = 0; i < testcount; i++) {
      if (statusarray[i] != SQLDBC_SUCCESS_NO_INFO) {
	logerror ("Row status at %d contains %d but should contain %d ", i, statusarray[i], SQLDBC_SUCCESS_NO_INFO);
      }
    }

    execSQL ("COMMIT WORK");

    connection ()->releaseStatement (stmtPrepare);

    delete[] ind2;
    delete[] ind1;
    delete[] ind0;
    delete[] testStr3;
    delete[] testStr2;
    delete[] testStr1;
    delete[] intarr;

    return m_success;
  }

  IFR_Bool lob_select (IFR_Int4 testcount, IFR_Int4 arraysize1, IFR_Int4 arraysize2)
  {
    IFR_Retcode rc;
    char cbuf[11];
    char buf[10000];
    char buf2[40000];
    char *c1data = new char[testcount*arraysize1];
    char *c2data = new char[testcount*arraysize2];
    char *c3data = new char[testcount*26];
    IFR_Length *ind = new IFR_Length[testcount];
    IFR_Int4 *idata = new IFR_Int4[testcount];
    const char *cmdSelect = "SELECT I, A, B, C FROM BLOBTST001";
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

    memset (ind, 0, sizeof(IFR_Int4)*testcount);
    if (rs->bindColumn (1, IFR_HOSTTYPE_INT4, idata, ind, 4) != IFR_OK)
      logerror ("Error in bind column.");
    
    if (rs->bindColumn (2, IFR_HOSTTYPE_BINARY, c1data, ind, arraysize1, false) != IFR_OK)
      logerror ("Error in bind column.");

    if (rs->bindColumn (3, IFR_HOSTTYPE_ASCII, c2data, ind, arraysize2, false) != IFR_OK)
      logerror ("Error in bind column.");

    if (rs->bindColumn (4, IFR_HOSTTYPE_ASCII, c3data, ind, 26, false) != IFR_OK)
      logerror ("Error in bind column.");

    rs->setRowSetSize (testcount);

    if (rs->first () != IFR_OK)
      logerror ("Error in fetch first.");  

    IFR_RowSet *rowset = rs->getRowSet ();

    do {
      rc = rowset->getObject (4, cbuf, sizeof (cbuf), IFR_HOSTTYPE_UCS2, ind, true);
    } while (rc == IFR_DATA_TRUNC);
    rc = rowset->getObject (4, cbuf, sizeof (cbuf), IFR_HOSTTYPE_UCS2, ind, true);


    if (rs->absolute (1) != IFR_OK)
      logerror ("Error in fetch next.");

    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_BINARY, ind, true);
    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_BINARY, ind, true);
    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_BINARY, ind, true);
    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (2, buf, sizeof (buf), IFR_HOSTTYPE_BINARY, ind, true);

    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (3, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);

    memset (buf, 0, sizeof (buf));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (3, buf, sizeof (buf), IFR_HOSTTYPE_ASCII, ind, true);

    memset (buf2, 0, sizeof (buf2));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (2, buf2, sizeof (buf2), IFR_HOSTTYPE_ASCII, ind, true);
    memset (buf2, 0, sizeof (buf2));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (2, buf2, sizeof (buf2), IFR_HOSTTYPE_ASCII, ind, true);
    memset (buf2, 0, sizeof (buf2));
    memset (ind, 0, sizeof (IFR_Length) * testcount);
    rowset->getObject (2, buf2, sizeof (buf2), IFR_HOSTTYPE_ASCII, ind, true);

    do {
      rc = rowset->getObject (4, cbuf, sizeof (cbuf), IFR_HOSTTYPE_ASCII, ind, true);
    } while (rc == IFR_DATA_TRUNC);

    
    memset(c1data, 87, testcount*arraysize1);
    memset(c2data, 87, testcount*arraysize2);
    rc = rowset->fetch();
    if(rc != IFR_OK && rc != IFR_DATA_TRUNC) {
        logerror("RETCODE %d found, instead of OK", rc);
    }    
    lob_check (1, testcount, arraysize1, true, c1data, true, "c1data");
    lob_check (1, testcount, arraysize2, false, c2data, true, "c2data");

    if (rowset->setPos (1) != IFR_OK)
      logerror ("Error in setPos.");
    
    if (rs->last () != IFR_OK)
      logerror ("Error in fetch next.");

    rc = rowset->fetch();
    if(rc != IFR_OK && rc != IFR_DATA_TRUNC) {
        logerror ("Error in fillRowSet.");
    }
    
    lob_check (19, testcount, arraysize1, true, c1data, true, "c1data 19");
    lob_check (19, testcount, arraysize2, false, c2data, true, "c2data 19");

    rs->close ();

    delete[] ind;
    delete[] idata;
    delete[] c3data;
    delete[] c2data;
    delete[] c1data;

    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new Blobtst001 ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
