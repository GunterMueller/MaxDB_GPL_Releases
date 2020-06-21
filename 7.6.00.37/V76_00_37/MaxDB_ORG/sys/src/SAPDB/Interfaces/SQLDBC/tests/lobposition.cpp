/*!
  @file           lobposition.cpp
  @author         D030044
  @ingroup        Tests
  @brief          Checks postioned read of lobs.

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

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

class LobPositionTest
  : public SQLDBC_TestCase
{
public:
  LobPositionTest()
    : SQLDBC_TestCase("LobPositionTest")
  {}

  virtual SQLDBC_Bool run() {
    SQLDBC_Int4 rows[] = { 1, 2, 10 };
    SQLDBC_Int4 loblen1[] = { 1024*1024, 30*1024 };
    SQLDBC_Int4 loblen2[] = {        64,  5*1024 };
    SQLDBC_Int4 lobstart[] = { 1, -1, 20, -20, 10, -10, 5, -5,  4, -4, 3, -3,  2, -2 };
    SQLDBC_Int4 lobarray[] = { 1, 40, 20,   5, 10,   8, 3,  2, 25, 25, 7,  7, 12, 12 };

    for (int i = 0; i < sizeof(rows)/sizeof(SQLDBC_Int4); i++) {
        for (int j = 0; j < MIN(sizeof(loblen1),sizeof(loblen2))/sizeof(SQLDBC_Int4); j++) {
            for (int k = 0; k < MIN(sizeof(lobstart),sizeof(lobarray))/sizeof(SQLDBC_Int4); k++) {
                lob_insert (rows[i], loblen1[j], loblen2[j]);
                loginfo ("rows: %d, loblen1: %d, loblen2: %d, lobstart: %d, lobarray: %d",
                         rows[i], loblen1[j], loblen2[j], lobstart[k], lobarray[k]);
                lob_select (rows[i], loblen1[j], loblen1[j]/lobarray[k], loblen1[j]/(lobarray[k]*lobstart[k]), 
                            loblen2[j], loblen2[j]/lobarray[k], loblen2[j]/(lobarray[k]*lobstart[k]));
            }          
        }
    } 

    lob_insert (10, 1024*1024, 64);
    lob_selectpos (10, 1024*1024, 8*1024, 10, 64, 20, 10);
    lob_selectpos (10, 1024*1024, 20*1024, 10, 64, 20, 10);
    lob_selectpos (10, 1024*1024, 40*1024, 10*1024, 64, 20, 10);
    lob_selectpos (10, 1024*1024, 40*1024, 20*1024, 64, 20, 20);
    lob_selectpos (10, 1024*1024, 40*1024, 30*1024, 64, 20, 30);
    lob_selectpos (10, 1024*1024, 40*1024, 40*1024, 64, 20, 40);
    lob_selectpos (10, 1024*1024, 40*1024, 60*1024, 64, 20, 50);
    lob_selectpos (10, 1024*1024, 40*1024, 100*1024, 64, 20, 60);
    lob_selectpos (10, 1024*1024, 40*1024, 211*1024, 64, 20, 61);
    lob_selectpos (10, 1024*1024, 40*1024, 1023*1024, 64, 20, 64);

    execSQL("COMMIT WORK");
    return m_success;
  }

  void makeCharBuffer(char *result, SQLDBC_size_t size, SQLDBC_Bool upper)
  {
    char *udata= (char *) "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *ldata= (char *) "abcdefghijklmnopqrstuvwxyz";
    char *data;
    if (upper)
      data = udata;
    else
      data = ldata;
    for(SQLDBC_size_t i=0; i<size; ++i) {
      result[i]=data[i%26];
    }
  }

  SQLDBC_Bool checkPositionedRead (SQLDBC_Int4 testcount, 
                                   char *data, 
                                   SQLDBC_Length *ind,
                                   SQLDBC_Int4 start,
                                   SQLDBC_Int4 arraysize,
                                   SQLDBC_Int4 loblen,
                                   SQLDBC_Bool upper)
  {
    SQLDBC_Bool rc = true;
    SQLDBC_Int4 len;
    char *cmpStr = new char[loblen];
    char *p;
    char tb[6];
    
    makeCharBuffer (cmpStr, loblen, upper);
    if (start > 0) {
      p = cmpStr + start - 1;
    } else {
      p = cmpStr + loblen + start;
    }
    len = MIN(arraysize, (cmpStr+loblen) - p);

    for (int i = 0; i < testcount; i++) {
      sprintf (tb, "%5d", i+1);
      memcpy (cmpStr, tb, 5);
      if (memcmp (p, data+i*arraysize, len)){
        logerror ("Error. Wrong data in row %d.", i);
        rc = false;
      }
    }

    delete [] cmpStr;

    return rc;
  }

  SQLDBC_Bool lob_insert (SQLDBC_Int4 testcount, SQLDBC_Int4 arraysize1, SQLDBC_Int4 arraysize2)
  {
      loginfo("LOB INSERT TC=%d, SZ1=%d, SZ2=%d", testcount, arraysize1, arraysize2);  
    int i;
    SQLDBC_Retcode rc;
    SQLDBC_PreparedStatement *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();

    dropTable ("LOBPOSITION");

    if (!m_success)
      return m_success;

    m_success = m_success && execSQL ("CREATE TABLE LOBPOSITION (I INTEGER, A LONG BYTE, B LONG VARCHAR ASCII, C VARCHAR(30) ASCII)");
    
    SQLDBC_Int4 *intarr = new int[testcount];
    char *str1, *str2, *str3;
    char *testStr1 = new char[arraysize1*testcount];
    char *testStr2 = new char[arraysize2*testcount];
    char *testStr3 = new char[27*testcount];
    SQLDBC_Length *ind0 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind1 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind2 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind3 = new SQLDBC_Length[testcount];
    SQLDBC_Int4 t1len = arraysize2;
    SQLDBC_Int4 t2len = SQLDBC_NTS;    

    memset (ind0, 0, sizeof (SQLDBC_Length)*testcount);
    memset (ind1, 0, sizeof (SQLDBC_Length)*testcount);
    memset (ind2, 0, sizeof (SQLDBC_Length)*testcount);
    memset (ind3, 0, sizeof (SQLDBC_Length)*testcount);

    tc_errorcheck (stmtPrepare)->bindParameter (1, SQLDBC_HOSTTYPE_INT4, intarr, ind0, 
						sizeof (SQLDBC_Int4));
    tc_errorcheck (stmtPrepare)->bindParameter (2, SQLDBC_HOSTTYPE_BINARY, testStr1, ind1, 
						arraysize1);
    tc_errorcheck (stmtPrepare)->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, testStr2, ind2, 
						arraysize2);
    tc_errorcheck (stmtPrepare)->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, testStr3, ind3, 
						26);
    tc_errorcheck (stmtPrepare)->setBatchSize (testcount);
    tc_errorcheck (stmtPrepare)->prepare ("INSERT INTO LOBPOSITION VALUES (?, ?, ?, ?)");

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
    if (rc != SQLDBC_OK) {
      logerror ("Error in executeBatch ()");
    }
    const SQLDBC_Int4 *statusarray = stmtPrepare->getRowStatus();
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

  SQLDBC_Bool lob_select (SQLDBC_Int4 testcount, 
                          SQLDBC_Int4 loblen1,
                          SQLDBC_Length arraysize1, 
                          SQLDBC_Length start1,
                          SQLDBC_Int4 loblen2,
                          SQLDBC_Length arraysize2,
                          SQLDBC_Length start2)
  {
      loginfo("LOB SELECT TC=%d", testcount);  
    SQLDBC_Retcode rc;
    char *c1data = new char[testcount*arraysize1];
    char *c2data = new char[testcount*arraysize2];
    char *c3data = new char[testcount*26];
    SQLDBC_Length *ind0 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind1 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind2 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind3 = new SQLDBC_Length[testcount];
    SQLDBC_Int4 *idata = new SQLDBC_Int4[testcount];
    const char *cmdSelect = "SELECT I, A, B, C FROM LOBPOSITION";

    if ((start1 == 0) && (arraysize1 > 0))
      start1 = 1;
    if ((start2 == 0) && (arraysize2 > 0))
      start2 = 1;

    loginfo("Selecting lobs. Rows = %d.", testcount); 
    loginfo("lob1: len = %d, buflen = %d, startpos = %d", loblen1, arraysize1, start1);
    loginfo("lob2: len = %d, buflen = %d, startpos = %d", loblen2, arraysize2, start2);

    SQLDBC_Statement *stmt;
    { stmt = tc_errorcheck (connection ())->createStatement (); }
    SQLDBC_ResultSet *rs;
    rc = stmt->execute (cmdSelect, SQLDBC_StringEncodingAscii);
    if (rc == SQLDBC_NOT_OK)
      logerror (stmt->error().getErrorText ());
    if (stmt->isQuery()) {
      rs = stmt->getResultSet ();
    } else {
      logerror ("Error. No resultset returned.");
    }

    memset (ind0, 0, sizeof(SQLDBC_Int4)*testcount);
    memset (ind1, 0, sizeof(SQLDBC_Int4)*testcount);
    memset (ind2, 0, sizeof(SQLDBC_Int4)*testcount);
    memset (ind3, 0, sizeof(SQLDBC_Int4)*testcount);
    SQLDBC_Length ilen = 4;
    if (rs->bindColumn (1, SQLDBC_HOSTTYPE_INT4, idata, ind0, ilen) != SQLDBC_OK)
      logerror ("Error in bind column 1.");
    
    if (rs->bindColumn (2, SQLDBC_HOSTTYPE_ASCII, c1data, ind1, &start1, arraysize1) != SQLDBC_OK)
       logerror ("Error in bind column 2.");

    if (rs->bindColumn (3, SQLDBC_HOSTTYPE_ASCII, c2data, ind2, &start2, arraysize2) != SQLDBC_OK)
      logerror ("Error in bind column 3.");

    if (rs->bindColumn (4, SQLDBC_HOSTTYPE_ASCII, c3data, ind3, (SQLDBC_Length) 26) != SQLDBC_OK)
      logerror ("Error in bind column 4.");

    rs->setRowSetSize (testcount);

    if (rs->first () != SQLDBC_OK)
      logerror ("Error in fetch first.");  

    SQLDBC_RowSet *rowset = rs->getRowSet ();

    tc_errorcheck(rowset)->fetch ();

    checkPositionedRead (testcount, c1data, ind1, start1, arraysize1, loblen1, true);
    checkPositionedRead (testcount, c2data, ind2, start2, arraysize2, loblen2, false);

    delete[] ind0;
    delete[] ind1;
    delete[] ind2;
    delete[] ind3;
    delete[] idata;
    delete[] c3data;
    delete[] c2data;
    delete[] c1data;

    return m_success;
  }

  SQLDBC_Bool lob_selectpos (SQLDBC_Int4 testcount, 
			     SQLDBC_Int4 loblen1,
			     SQLDBC_Length arraysize1, 
			     SQLDBC_Length start1,
			     SQLDBC_Int4 loblen2,
			     SQLDBC_Length arraysize2,
			     SQLDBC_Length start2)
  {
      loginfo("LOB SELECTPOS TC=%d", testcount);
    SQLDBC_Retcode rc;
    char *c1data = new char[testcount*arraysize1];
    char *c2data = new char[testcount*arraysize2];
    SQLDBC_Length *ind1 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind2 = new SQLDBC_Length[testcount];
    const char *cmdSelect = "SELECT I, A, B, C FROM LOBPOSITION";

    loginfo("Selecting lobs. lob1: len = %d, startpos = %d. lob2: len = %d, startpos = %d",
	    arraysize1, start1, arraysize2, start2);

    SQLDBC_Statement *stmt;
    { stmt = tc_errorcheck (connection ())->createStatement (); }
    SQLDBC_ResultSet *rs;
    rc = stmt->execute (cmdSelect, SQLDBC_StringEncodingAscii);
    if (rc == SQLDBC_NOT_OK)
      logerror (stmt->error().getErrorText ());
    if (stmt->isQuery()) {
      rs = stmt->getResultSet ();
    } else {
      logerror ("Error. No resultset returned.");
    }

    memset (ind1, 0, sizeof(SQLDBC_Int4)*testcount);
    memset (ind2, 0, sizeof(SQLDBC_Int4)*testcount);

    rs->setRowSetSize (testcount);

    if (rs->first () != SQLDBC_OK)
      logerror ("Error in fetch first.");  

    SQLDBC_RowSet *rowset = rs->getRowSet ();

    tc_errorcheck(rowset)->setPos(1);
    memset (c1data, 0, arraysize1);
    memset (ind1, 0, sizeof(SQLDBC_Int4)*testcount);
    rc = rowset->getObject(2, SQLDBC_HOSTTYPE_ASCII, c1data, ind1, arraysize1, start1);
    checkPositionedRead (1, c1data, ind1, start1, arraysize1, loblen1, true);
    while (rc == SQLDBC_DATA_TRUNC) {
      start1 += arraysize1;
      memset (c1data, 0, arraysize1);
      memset (ind1, 0, sizeof(SQLDBC_Int4)*testcount);
      rc = rowset->getObject(2, SQLDBC_HOSTTYPE_ASCII, c1data, ind1, arraysize1);
      checkPositionedRead (1, c1data, ind1, start1, arraysize1, loblen1, true);      
    }
    memset (c2data, 0, arraysize2);
    memset (ind2, 0, sizeof(SQLDBC_Int4)*testcount);
    rc = rowset->getObject(3, SQLDBC_HOSTTYPE_ASCII, c2data, ind2, arraysize2, start2);
    checkPositionedRead (1, c2data, ind2, start2, arraysize2, loblen2, false);
    while (rc == SQLDBC_DATA_TRUNC) {
      start2 += arraysize2;
      memset (c2data, 0, arraysize2);
      memset (ind2, 0, sizeof(SQLDBC_Int4)*testcount);
      rc = rowset->getObject(3, SQLDBC_HOSTTYPE_ASCII, c2data, ind2, arraysize2);
      checkPositionedRead (1, c2data, ind2, start2, arraysize2, loblen2, false);      
    }

    delete[] ind1;
    delete[] ind2;
    delete[] c2data;
    delete[] c1data;

    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new LobPositionTest ());
  if (testframe.run ()) 
      return 0;
  
  return 1;
}
