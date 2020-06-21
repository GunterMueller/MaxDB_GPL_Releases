/*!
  @file           lobselectinto.cpp
  @author         D030044
  @ingroup        Tests
  @brief          Checks select into command with long columns.

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

class LobSelectInto
  : public SQLDBC_TestCase
{
public:
  LobSelectInto()
    : SQLDBC_TestCase("LobSelectInto")
  {}

  virtual SQLDBC_Bool run() {
    lob_insert (2, 1024*1024, 1024*1024);
    lob_selectinto (1024*1024, 1024*50);
    lob_selectinto (1024*1024, 1024*10);
    lob_selectinto (1024*1024, 1024*1023);
    lob_insert (2, 1024, 1024);
    lob_selectinto (1024, 256);
    lob_selectinto (1024, 512);
    lob_selectinto (1024, 1024);
    lob_selectinto (1024, 600);

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


  SQLDBC_Bool checkData (SQLDBC_Int4 nr,
			 char *data, 
			 SQLDBC_Length *ind,
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
    p = cmpStr;
    len = MIN(arraysize, (cmpStr+loblen) - p);

    sprintf (tb, "%5d", nr);
    memcpy (cmpStr, tb, 5);
    if (my_memcmp (p, data, len)) {
        rc = false;
    }

    delete [] cmpStr;

    return rc;
  }

  SQLDBC_Bool checkPositionedRead (SQLDBC_Int4 testcount, 
				   SQLDBC_Retcode rc,
                                   char *data, 
                                   SQLDBC_Length *ind,
                                   SQLDBC_Int4 start,
                                   SQLDBC_Int4 arraysize,
                                   SQLDBC_Int4 loblen,
                                   SQLDBC_Bool upper)
  {
    SQLDBC_Bool rc1 = true;
    SQLDBC_Int4 len;
    char *cmpStr = new char[loblen];
    char *p;
    char tb[6];

    if ((start > loblen) && (rc != SQLDBC_NO_DATA_FOUND)) {
      logerror ("Error. Wrong returncode. Expected %d. Was %d.", SQLDBC_NO_DATA_FOUND, rc);
    }
    
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
        rc1 = false;
      }
    }

    delete [] cmpStr;

    return rc1;
  }

  SQLDBC_Bool lob_insert (SQLDBC_Int4 testcount, SQLDBC_Int4 arraysize1, SQLDBC_Int4 arraysize2)
  {
      loginfo("lob_insert(%d, %d, %d)", testcount, arraysize1, arraysize2);
    int i;
    SQLDBC_Retcode rc;
    SQLDBC_PreparedStatement *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();

    dropTable ("LOBSELECTINTO");
    m_success = m_success && execSQL ("CREATE TABLE LOBSELECTINTO (I INTEGER, A LONG BYTE, B LONG BYTE)");
    
    SQLDBC_Int4 *intarr = new int[testcount];
    char *str1, *str2;
    char *testStr1 = new char[arraysize1*testcount];
    char *testStr2 = new char[arraysize2*testcount];
    SQLDBC_Length *ind0 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind1 = new SQLDBC_Length[testcount];
    SQLDBC_Length *ind2 = new SQLDBC_Length[testcount];
    SQLDBC_Int4 t1len = arraysize2;
    SQLDBC_Int4 t2len = SQLDBC_NTS;    

    memset (ind0, 0, sizeof (SQLDBC_Length)*testcount);
    memset (ind1, 0, sizeof (SQLDBC_Length)*testcount);
    memset (ind2, 0, sizeof (SQLDBC_Length)*testcount);

    tc_errorcheck (stmtPrepare)->bindParameter (1, SQLDBC_HOSTTYPE_INT4, intarr, ind0, 
						sizeof (SQLDBC_Int4));
    tc_errorcheck (stmtPrepare)->bindParameter (2, SQLDBC_HOSTTYPE_BINARY, testStr1, ind1, 
						arraysize1);
    tc_errorcheck (stmtPrepare)->bindParameter (3, SQLDBC_HOSTTYPE_BINARY, testStr2, ind2, 
						arraysize2);
    tc_errorcheck (stmtPrepare)->setBatchSize (testcount);
    tc_errorcheck (stmtPrepare)->prepare ("INSERT INTO LOBSELECTINTO VALUES (?, ?, ?)");

    str1 = testStr1;
    str2 = testStr2;
    for (i = 0; i < testcount; i++) {
      char tb[6];

      makeCharBuffer (str1, arraysize1, true);
      makeCharBuffer (str2, arraysize2, false);

      sprintf (tb, "%5d", i+1);
      memcpy (str1, tb, 5);
      memcpy (str2, tb, 5);
      ind1[i] = arraysize1;
      ind2[i] = arraysize2;
      intarr[i] = i+1;

      str1 += arraysize1;
      str2 += arraysize2;
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
    delete[] testStr2;
    delete[] testStr1;
    delete[] intarr;

    return m_success;
  }

  SQLDBC_Bool lob_selectinto (SQLDBC_Int4 arraysize, SQLDBC_Int4 loblen) {
      loginfo("lob_selectinto(%d, %d)", arraysize, loblen);
      SQLDBC_Retcode rc;
      SQLDBC_Length *ind1 = new SQLDBC_Length[1];
      SQLDBC_Length *ind2 = new SQLDBC_Length[1];
      char *data1 = new char[loblen];
      char *data2 = new char[loblen];
      
      SQLDBC_PreparedStatement *stmt = tc_errorcheck (connection())->createPreparedStatement ();
      rc = tc_errorcheck (stmt)->prepare("SELECT A INTO ? FROM LOBSELECTINTO WHERE I = 1");
      ind1[0] = 0;
      if (rc == SQLDBC_OK) {
          tc_errorcheck (stmt)->bindParameter (1, SQLDBC_HOSTTYPE_BINARY, data1, ind1, loblen, false);
      }
      if (rc == SQLDBC_OK)
          rc = tc_errorcheck (stmt)->execute();
      
      SQLDBC_Length start = loblen;
      memset (data1, 0, loblen);
      ind1[0] = 0;
      loginfo("EXECUTE DONE, START IS %d", start);
      rc = stmt->getObject (1, SQLDBC_HOSTTYPE_BINARY, data1, ind1, loblen, start, false);
      if (!checkPositionedRead (1, rc, data1, ind1, start, loblen, arraysize, true))
          logerror ("Error in data.");
      
      start += loblen;
      while (rc == SQLDBC_DATA_TRUNC) {
          // loginfo("START IS %d", start);
          rc = stmt->getObject (1, SQLDBC_HOSTTYPE_BINARY, data1, ind1, loblen, false);
          if (!checkPositionedRead (1, rc, data1, ind1, start, loblen, arraysize, true))
              logerror ("Error in data.");
          start += loblen;
          memset (data1, 0, loblen);
          ind1[0] = 0;
      }
      
      tc_errorcheck (stmt)->clearParameters ();

      rc = tc_errorcheck (stmt)->prepare("SELECT A, B INTO ?, ? FROM LOBSELECTINTO WHERE I = 2");
      ind1[0] = 0;
      ind2[0] = 0;
      memset(data1, 87, loblen);
      memset(data2, 87, loblen);
      if (rc == SQLDBC_OK) {
          tc_errorcheck (stmt)->bindParameter (1, SQLDBC_HOSTTYPE_BINARY, data1, ind1, loblen, false);
          tc_errorcheck (stmt)->bindParameter (2, SQLDBC_HOSTTYPE_BINARY, data2, ind2, loblen, false);
      }
      // DebugBreak();
      if (rc == SQLDBC_OK)
          rc = tc_errorcheck (stmt)->execute();
      
      if (!checkData (2, data1, ind1, arraysize, loblen, true))
          logerror ("Error in data1.");
      if (!checkData (2, data2, ind2, arraysize, loblen, false))
          logerror ("Error in data2.");
      
    tc_errorcheck (stmt)->clearParameters ();

    rc = tc_errorcheck (stmt)->prepare("SELECT I, A, B INTO ?, ?, ? FROM LOBSELECTINTO WHERE I = 2");
    ind1[0] = 0;
    ind2[0] = 0;
    SQLDBC_Int4 val;
    SQLDBC_Length ind = 0;
    if (rc == SQLDBC_OK) {
      tc_errorcheck (stmt)->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &val, &ind, sizeof(val));
      tc_errorcheck (stmt)->bindParameter (2, SQLDBC_HOSTTYPE_BINARY, data1, ind1, loblen, false);
      tc_errorcheck (stmt)->bindParameter (3, SQLDBC_HOSTTYPE_BINARY, data2, ind2, loblen, false);
    }
    if (rc == SQLDBC_OK)
      rc = tc_errorcheck (stmt)->execute();

    if (val != 2)
      logerror ("Error in val.");
    if (!checkData (2, data1, ind1, arraysize, loblen, true))
      logerror ("Error in data1.");
    if (!checkData (2, data2, ind2, arraysize, loblen, false))
      logerror ("Error in data2.");
    
    delete [] ind1;
    delete [] ind2;
    delete [] data1;
    delete [] data2;

    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new LobSelectInto ());
  if (testframe.run ()) 
      return 0;
  
  return 1;
}
