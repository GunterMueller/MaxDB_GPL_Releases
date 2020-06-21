/*!================================================================
 module:    lobtypestest.cpp

 responsible: D030044

 special area: Lob Type conversion
 description:  Check program that checks conversions.

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

#define CHECK_STRING_Z(xxinsertstr, xxcomparestr, xxinhosttype, xxcmphosttype, xxindex, xxtablename) \
CheckString(xxinsertstr, IFR_NTS, xxcomparestr, xxinhosttype, xxcmphosttype, xxindex, #xxtablename)

//----------------------------------------------------------------------

static IFR_size_t ucs2strlen(char *s)
{
  IFR_size_t i = 0;
  while (true) {
      if(s[i]== 0 && s[i+1]==0) {
          return i;
      }
      i += 2;
  }
  return i;
}

class LobAsciiTest : public IFR_TestCase
{
public:
  LobAsciiTest(char *test = "LobAsciiTest")
    : IFR_TestCase (test)
  {}

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

  void CheckString(char *xxinsertstr, IFR_Int4 xxstrlen, char *xxcomparestr, IFR_HostType xxinhosttype, 
                   IFR_HostType xxcmphosttype, int xxindex, char *xxtablename)
    
  {
    int xxlen;
    IFR_Retcode rc;
    int terminatorsize;
    int xx_compareterminatorsize;
    if (xxinhosttype == IFR_HOSTTYPE_UCS2_NATIVE) {
        terminatorsize = 2;
        xxlen = (xxstrlen == IFR_NTS) ? ucs2strlen(xxinsertstr)+terminatorsize : xxstrlen+terminatorsize;
    } else {
        terminatorsize = 1;
        xxlen = (xxstrlen == IFR_NTS) ? strlen(xxinsertstr)+1 : xxstrlen+1;
    }
    if(xxcmphosttype == IFR_HOSTTYPE_UCS2_NATIVE) {
        xx_compareterminatorsize = 2;
    } else {
        xx_compareterminatorsize = 1;
    }
    char *x1=new char[xxlen];
    char *x2=new char[xxlen];
      
    if (xxinhosttype == IFR_HOSTTYPE_UCS2_NATIVE) {
      memcpy (x1, xxinsertstr, xxlen-2);
      x1[xxlen-1] = '\0';
      x1[xxlen-2] = '\0';
    } else {
      strcpy (x1, xxinsertstr);
    }
    memcpy(x2, x1, xxlen);
    IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
    IFR_Length x1len=xxstrlen;
    tc_errorcheck(p1)->bindParameter(1, xxinhosttype, x1, &x1len, xxlen);
    char szSQL[500];
    sprintf(szSQL, "INSERT INTO \"%s\" VALUES (%d, ?)", xxtablename, xxindex);
    tc_errorcheck(p1)->prepare(szSQL, IFR_StringEncodingAscii);
    tc_errorcheck(p1)->execute();
    if(memcmp(x1, x2, xxlen)!=0) {
      logerror("Insert modified the data for \"%s\".", xxinsertstr);
    }
    if (xxcmphosttype == IFR_HOSTTYPE_UCS2_NATIVE) {
      xxlen = ucs2strlen(xxcomparestr)+2;
    } else {
      xxlen = strlen(xxcomparestr)+1;
    }
    delete [] x2;
    x2=new char[xxlen];
    if (xxcmphosttype == IFR_HOSTTYPE_UCS2_NATIVE) {
        memcpy (x2, xxcomparestr, xxlen-2);
        x2[xxlen-1] = '\0';
        x2[xxlen-2] = '\0';
    } else {
        strcpy (x2, xxcomparestr);
    }
    char *x3=new char[xxlen];
    memset(x3, 0, xxlen);
    IFR_PreparedStmt *p2=tc_errorcheck(connection())->createPreparedStatement();
    IFR_Length x3len=0;
    // tc_errorcheck(p2)->bindParameter(1, xxhosttype, x3, &x3len, xxlen);
    sprintf(szSQL, "SELECT V FROM \"%s\" WHERE K=%d", xxtablename, xxindex);
    tc_errorcheck(p2)->prepare(szSQL, IFR_StringEncodingAscii);
    tc_errorcheck(p2)->execute();
    IFR_ResultSet *resultset = p2->getResultSet();
    resultset->bindColumn (1, xxcmphosttype, x3, &x3len, xxlen);
    resultset->next();
    resultset->setRowSetSize (1);
    IFR_RowSet *rowset = resultset->getRowSet ();
    rowset->fetch ();
    if(memcmp(x2, x3, xxlen)!=0) {
        logerror("FETCH: Value mismatch in value \"%s\" inserted was \"%s\".", xxcomparestr, xxinsertstr);
    }
    int bsizes[] = { 100000, 10, 0 };
    int lp = 0;
    while (bsizes[lp]) {
      char *buf2 = new char[bsizes[lp] + terminatorsize];
      IFR_Length *ind = new IFR_Length[1];
      char *cp = x2;
      int cplen = MIN(bsizes[lp], xxlen);
      int rest = xxlen;
      rc = rowset->setPos (1);
      do {
          memset (buf2, '\0', bsizes[lp] + xx_compareterminatorsize);
          rc = rowset->getObject (1, buf2, bsizes[lp] + xx_compareterminatorsize, xxcmphosttype, ind, true);    
          if (memcmp (cp, buf2, MIN(cplen, rest))) {
            logerror("GETOBJECT:  bsize = %d value mismatch in value \"%s\" inserted was \"%s\".", bsizes[lp], xxcomparestr, xxinsertstr);
          break;
        }
        cp += cplen;
        rest -= cplen;
      } while (rc == IFR_DATA_TRUNC);
      delete [] buf2;
      delete [] ind;
      lp++;
    }
    delete [] x1;
    delete [] x2;
    delete [] x3;
    connection()->releaseStatement(p1);
    connection()->releaseStatement(p2);
  }

  virtual IFR_Bool run ()
  {
    dropTable("LOBASCII_TEST");
    m_success = m_success && execSQL("CREATE TABLE LOBASCII_TEST (K INT, V LONG ASCII)");

    CHECK_STRING_Z("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN",
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN",
                   IFR_HOSTTYPE_ASCII,
                   IFR_HOSTTYPE_ASCII,
                   1,
                   LOBASCII_TEST);

    execSQL ("COMMIT WORK");

    return m_success;
  }
};

class LobUnicodeTest : public LobAsciiTest
{
public:
  LobUnicodeTest()
    : LobAsciiTest ("LobUnicodeTest")
  {}  

  virtual IFR_Bool run ()
  {
    if (!connection()->isUnicodeDatabase ()) {
      return m_success;
    }
    dropTable("LOBUNICODE_TEST");
    m_success = m_success && execSQL("CREATE TABLE LOBUNICODE_TEST (K INT, V LONG UNICODE)");

    char *x = new char[82];
    convertToUCS2((unsigned short*)x,82,"ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN");
    CHECK_STRING_Z(x,
                   x,
                   IFR_HOSTTYPE_UCS2_NATIVE,
                   IFR_HOSTTYPE_UCS2_NATIVE,
                   1,
                   LOBUNICODE_TEST);
#if 0
    int szBuf = 1024*1024;
    char *buf = new char[szBuf];
    char *y = new char[szBuf*2 + 2];
    makeCharBuffer (buf, szBuf, true);
    buf[szBuf-1] = '\0';
    convertToUCS2((unsigned short *)y,szBuf*2,buf);
    y[szBuf * 2] = '\0';
    y[szBuf * 2 + 1] = '\0';
    CHECK_STRING_Z(y,
                   buf,
                   IFR_HOSTTYPE_UCS2_NATIVE,
                   IFR_HOSTTYPE_ASCII,
                   2,
                   LOBUNICODE_TEST);
#endif
    execSQL ("COMMIT WORK");

    delete [] x;
#if 0
    delete [] y;
    delete [] buf;
#endif

    return m_success;
  }
};

int main(int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new LobAsciiTest ());
  testframe.addTestCase (new LobUnicodeTest ());
  
  if (testframe.run ()) return 0;
  
  return 1;
}
