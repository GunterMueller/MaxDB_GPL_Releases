/*!
  @file           unterm.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check if no terminated Strings are possible.
  @keywords       SQLDBC_HOSTTYPE_ASCII, terminated, C style, getObject

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/SAPDBCommon/SAPDB_sprintf.h"
#include <string.h>

class UnterminatedString : public SQLDBC_TestCase
{
public:
  UnterminatedString ()
    : SQLDBC_TestCase ("UnterminatedString")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

    const char *szSQL = "SELECT 'ONE', FIXED(10), DATE from dual where '1' = ?";

    tc_errorcheck(stmt)->prepare(szSQL);

    char pdata[10];
    sprintf(pdata, "1");

    SQLDBC_Length ind=SQLDBC_NTS;

    tc_errorcheck(stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, pdata, &ind, sizeof(pdata), SQLDBC_TRUE);
    
    tc_errorcheck(stmt)->execute();
    
    SQLDBC_ResultSet *result = stmt->getResultSet();

    if (!result) {
      logerror("No result set returned.\n");
      return m_success;
    }

    SQLDBC_RowSet *rowset = result->getRowSet ();

    if (!rowset) {
      logerror("No row set returned.\n");
      return m_success;
    }

    
    {tc_errorcheck(result)->next();}

    char cdata[20];

    /* CHARACTER */
    memset(cdata, 'x', sizeof(cdata));
    {tc_errorcheck(rowset)->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata), SQLDBC_FALSE);}

    if (ind != 3) {
      logerror("Wrong length (%d) expected (3) for cdata returned.\n", ind);
      return m_success;
    }
      
    if (cdata[ind] != 'x') {
      logerror("getObject writes out of bounds.\n");
      logerror("cdata should not be terminated with '%x'\n", cdata[ind]);
      return m_success;
    }
      
    loginfo("length=%d, cdata=%.*s", ind, ind, cdata);

    /* NUMERIC */
    memset(cdata, 'x', sizeof(cdata));
    {tc_errorcheck(result)->getObject(2, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata), SQLDBC_FALSE);}

    if (ind != 2) {
      logerror("Wrong length (%d) expected (2) for cdata returned.\n", ind);
      return m_success;
    }
      
    if (cdata[ind] != 'x') {
      logerror("getObject writes out of bounds.\n");
      logerror("cdata should not be terminated with '%x'\n", cdata[ind]);
      return m_success;
    }
      
    loginfo("length=%d, cdata=%.*s", ind, ind, cdata);

    /* DATE */
    memset(cdata, 'x', sizeof(cdata));
    {tc_errorcheck(result)->getObject(3, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata), SQLDBC_FALSE);}

    if (ind != 10) {
      logerror("Wrong length (%d) expected (10) for cdata returned.\n", ind);
      return m_success;
    }
      
    if (cdata[ind] != 'x') {
      logerror("getObject writes out of bounds.\n");
      logerror("cdata should not be terminated with '%x'\n", cdata[ind]);
      return m_success;
    }
      
    loginfo("length=%d, cdata=%.*s", ind, ind, cdata);

    return m_success;
  }

};

class Unterm : public SQLDBC_TestCase
{
public:
  Unterm (char *name)
    : SQLDBC_TestCase (name)
  {
    strcpy(m_name,name); 
  }

  virtual SQLDBC_Bool run ()
  {
    dropTable("UNTERM");

    if (!strcmp(m_name, "VARCHAR"))
      m_success = execSQL("CREATE TABLE UNTERM ( C VARCHAR(20))");

    if (!strcmp(m_name, "CHAR"))
      m_success = execSQL("CREATE TABLE UNTERM ( C CHAR(20))");      
    
    m_stmt = connection()->createPreparedStatement();
    if (!m_stmt) {
      logerror("createPreparedStatement failed");
      return m_success;
    }

    tc_errorcheck(connection())->commit();

    if(!m_success)
      return m_success;
    
    if (!strcmp(m_name, "VARCHAR")) {
        m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"12345678901234567890", 20);

      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"12345678901234", 14);

      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"1234567890123", 13);

      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"1", 1);

      //      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"", 0);

      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"abcdefghijkl", 12);

      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"ABC", 3);

      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"X", 1);

      //      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"", 0);

      m_success = checkValue(SQLDBC_HOSTTYPE_UTF8, (char*)"abcdef", 6);

      m_success = checkValue(SQLDBC_HOSTTYPE_UTF8, (char*)"ghijk", 5);

      m_success = checkValue(SQLDBC_HOSTTYPE_UTF8, (char*)"LMN", 3);

      //      m_success = checkValue(SQLDBC_HOSTTYPE_UTF8, (char*)"", 0);
    }

    if (!strcmp(m_name, "CHAR")) {
      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"12345678901234567890", 20);
      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"12345678901234      ", 20);
      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"1                   ", 20);
      m_success = checkValue(SQLDBC_HOSTTYPE_ASCII, (char*)"                    ", 20);

      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"12345678901234567890", 20);
      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"12345678901234      ", 20);
      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"1                   ", 20);
      m_success = checkValue(SQLDBC_HOSTTYPE_UCS2, (char*)"                    ", 20);
    }

    connection()->releaseStatement(m_stmt);

    return m_success;
  }
  
  SQLDBC_Bool checkValue(const SQLDBC_HostType HostType, char *data,
                         SQLDBC_Length osize)
  {
      loginfo("CHECK VALUE '%.*s'", (int)osize, data);
      SQLDBC_Bool ret_success = m_success;
      
    m_success = true;

    SQLDBC_Length ilen = strlen(data);
    
    unsigned char *idata;

    if (HostType == SQLDBC_HOSTTYPE_UCS2) {
      ilen = ilen*2;
      idata = new unsigned char[ilen+2];
      SAPDB_sprintfUnicode(SAPDB_EncodingUCS2, idata, ilen+2, data);
    }
    else {      
      idata = new unsigned char[ilen+1];
      SAPDB_sprintfUnicode(SAPDB_EncodingAscii, idata, ilen+1, data);
    }

    tc_errorcheck(m_stmt)->prepare("INSERT INTO UNTERM VALUES(?)");

    tc_errorcheck(m_stmt)->bindParameter(1, HostType, idata, &ilen, ilen, SQLDBC_FALSE);    
    tc_errorcheck(m_stmt)->execute();
    
    tc_errorcheck(m_stmt)->prepare("SELECT C FROM UNTERM");
    tc_errorcheck(m_stmt)->execute();
    SQLDBC_ResultSet *result = m_stmt->getResultSet();
    if (m_success && !result)
      logerror("No result set returned.\n");

    SQLDBC_RowSet *rowset = result->getRowSet();

    if (m_success && !rowset) {
      logerror("No rowset returned.\n");
    } else {

    unsigned char *cdata;

    if (HostType == SQLDBC_HOSTTYPE_UCS2) {
      osize = osize*2;
    }

    cdata = new unsigned char[osize+1];

    memset(cdata, 'x', osize+1);
    SQLDBC_Length ind = 0xBADFEED;

    tc_errorcheck(result)->bindColumn(1, HostType, cdata, &ind, osize, SQLDBC_FALSE);

    tc_errorcheck(result)->next();

    tc_errorcheck(rowset)->fetch();

    if (cdata[ilen] != 'x') {
      logerror("Fetch writes out of bounds cdata[%d]=%c.\n", 
               ilen, cdata[ilen]);
    }

    if (ind != ilen) {
      logerror("Wrong length(%d) returned.\n", ind);
    }

    if (memcmp(idata, cdata, ilen)) {
      logerror("Wrong data returned.\n");
    }

    if (m_success) {
      loginfo("'%s' SUCCESS", data);
      m_success = ret_success;
    }
    else
      loginfo("'%s' FAILED", data);
    
    delete cdata;
    delete idata;

    }

    tc_errorcheck(connection())->rollback();

    return m_success;
  }
private:
  SQLDBC_PreparedStatement *m_stmt;  
  char m_name[100];
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new UnterminatedString());
  testframe.addTestCase (new Unterm("VARCHAR"));
//   testframe.addTestCase (new Unterm("CHAR"));
  if (testframe.run ()) 
    return 0;
  return 1;
}


