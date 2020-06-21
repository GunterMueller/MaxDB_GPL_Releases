/*!================================================================

 module:        SQLStmt.cpp

 -------------------------------------------------------------------

 responsible:   D025086

 special area:  SQL

 description:   Misc SQL Statement test

 ------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <stdio.h>
#include <string.h>

class SelDual : public SQLDBC_TestCase
{
public:
  SelDual ()
    : SQLDBC_TestCase ("SelDual")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Retcode rc;

    SQLDBC_StringEncoding encoding;

    static char *Cmd = (char *) "SELECT * FROM DUAL";
    UCS2CHAR buffer_w[100];
    char *buffer = (char*)buffer_w;

    if (connection()->isUnicodeDatabase()) {
      convertToUCS2(buffer_w, sizeof(buffer_w), Cmd);
      encoding = SQLDBC_StringEncodingUCS2Native;
    }
    else {
      strcpy (buffer, Cmd);
      encoding = SQLDBC_StringEncodingAscii;
    }
    
    SQLDBC_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    
    rc = tc_errorcheck (stmt)->execute (buffer, encoding);
    if (rc == SQLDBC_OK) {
      printf("Statement successfull executed\n");
    }
    else {
      printf("error : execution failed\n");
      logerror (stmt->error().getErrorText());      
    }
    SQLDBC_ResultSet *resultset = tc_errorcheck (stmt)->getResultSet();
    if (resultset != NULL) {
      printf ("Resultset exists.\n");
    } else {
      printf ("No resultset exists.\n");
    }
    execSQL ("COMMIT WORK");
    return m_success;
  }
};

class SelUpdate : public SQLDBC_TestCase
{
public:
  SelUpdate()
    : SQLDBC_TestCase("SelUpdate")
  {}

  virtual SQLDBC_Bool prepare()
  {    
    dropTable("UPD1");
    m_success |= execSQL("CREATE TABLE UPD1 ( C CHAR(10))");
    return m_success;
  }

  virtual SQLDBC_Bool release()
  {    
    dropTable("UPD1");
    return m_success;
  }

  virtual SQLDBC_Bool run ()
  {

    static char *Cmd = (char *) "SELECT * FROM UPD1 FOR UPDATE";

    SQLDBC_PreparedStatement *pstmt;
    pstmt = tc_errorcheck(connection())->createPreparedStatement ();
    
    tc_errorcheck(pstmt)->prepare(Cmd);

    tc_errorcheck(pstmt)->execute();

    tc_errorcheck(pstmt)->prepare(Cmd);

    return m_success;
  }
};

class SelLike : public SQLDBC_TestCase
{
public:
  SelLike()
    : SQLDBC_TestCase("SelLike")
  {}

  virtual SQLDBC_Bool prepare()
  {    
    dropTable("TATOPA");
    m_success |= execSQL("CREATE TABLE TATOPA ( K1C4 VARCHAR(4), K2C4 VARCHAR(4))");
    return m_success;
  }

  virtual SQLDBC_Bool release()
  {    
    dropTable("TATOPA");
    return m_success;
  }

  virtual SQLDBC_Bool run ()
  {

    static char *Cmd = (char *) "SELECT * FROM TATOPA WHERE K1C4 = ? AND K2C4 LIKE ?";

    SQLDBC_PreparedStatement *pstmt;
    pstmt = tc_errorcheck(connection())->createPreparedStatement ();

    tc_errorcheck(pstmt)->prepare(Cmd);
    
    char K1C4[4];
    SQLDBC_Length indK1C4;

    memset(K1C4, 'A', sizeof(K1C4));
    indK1C4 = sizeof(K1C4);
    tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, K1C4, &indK1C4, sizeof(K1C4), false);

    char K2C4[6];
    SQLDBC_Length indK2C4;

    memset(K2C4, 'B', sizeof(K2C4));
    indK2C4 = sizeof(K2C4);

    tc_errorcheck(pstmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, K2C4, &indK2C4, sizeof(K2C4), false);

    tc_errorcheck(pstmt)->execute();

    return m_success;
  }
};

class ManyPrepare : public SQLDBC_TestCase
{
public:
  ManyPrepare()
    : SQLDBC_TestCase("ManyPrepare")
  {}

  virtual SQLDBC_Bool run ()
  {

    const char *szValue;
    szValue = connectproperties().getProperty("STATEMENTCACHESIZE", NULL);
    if (!szValue) {
      logerror("Test should run with -p STATEMENTCACHESIZE=<n> argument.");
      return false;
    }
    sscanf(szValue, "%d", &m_cachesize);

    static char Cmd[20000];
    SQLDBC_PreparedStatement *pstmt;
    pstmt = tc_errorcheck(connection())->createPreparedStatement ();
    int i;
    for(i=0; i < m_cachesize+10; i++) {
      sprintf(Cmd, "SELECT %d FROM DUAL", i);
      if(!(i%100)) 
        loginfo("%d commands prepared.\n", i);
      tc_errorcheck(pstmt)->prepare(Cmd);
      tc_errorcheck(pstmt)->execute();
      if(!m_success)
        break;
    }

    return m_success;
  }
private:
  SQLDBC_Int2 m_cachesize;
};

class SelInto : public SQLDBC_TestCase
{
public:
  SelInto()
    : SQLDBC_TestCase("SelInto")
  {}

  virtual SQLDBC_Bool prepare()
  {    
    dropTable("TATOPCA");
    m_success |= execSQL("CREATE TABLE TATOPCA ( I4 FIXED(10), \
                                                 K1N5 VARCHAR(5) ASCII, \
                                                 K2C5 VARCHAR(5) ASCII, \
                                                 K3N5 VARCHAR(5) ASCII )");
    m_success |= execSQL("INSERT INTO TATOPCA VALUES ( 99999, \
                                                 '12345', \
                                                 'edcba', \
                                                 '54321' )");
    return m_success;
  }

  virtual SQLDBC_Bool release()
  {    
    dropTable("TATOPCA");
    return m_success;
  }

  virtual SQLDBC_Bool run ()
  {

    
    static char *Cmd = (char *) "SELECT \"I4\" INTO ? FROM \"TATOPCA\" WHERE \"K1N5\" = ? AND \"K2C5\" = ? AND \"K3N5\" = ?";

    SQLDBC_PreparedStatement *pstmt;
    pstmt = tc_errorcheck(connection())->createPreparedStatement ();

    tc_errorcheck(pstmt)->prepare(Cmd);
    
    char K1N5[6];
    SQLDBC_Length indK1N5;
    strcpy(K1N5, "12345");

    char K2C5[6];
    SQLDBC_Length indK2C5;
    strcpy(K2C5, "edcba");

    char K3N5[6];
    SQLDBC_Length indK3N5;
    strcpy(K3N5, "54321");

    indK1N5 = strlen(K1N5);
    tc_errorcheck(pstmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, K1N5, &indK1N5, sizeof(K1N5), false);

    indK2C5 = strlen(K2C5);
    tc_errorcheck(pstmt)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, K2C5, &indK2C5, sizeof(K2C5), false);

    indK3N5 = strlen(K3N5);
    tc_errorcheck(pstmt)->bindParameter(4, SQLDBC_HOSTTYPE_ASCII, K3N5, &indK3N5, sizeof(K3N5), false);

    SQLDBC_Int2 I4=99;
    SQLDBC_Length indI4=0;

    tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT2, &I4, &indI4, sizeof(I4), false);
    
    SQLDBC_Retcode rc = tc_expectederrorcheck(pstmt, -10811)->execute();
    if (rc == SQLDBC_OK) {
      logerror("Wrong returncode received");
    }
    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new SelDual());
  testframe.addTestCase (new SelUpdate());
  testframe.addTestCase (new ManyPrepare());
  testframe.addTestCase (new SelLike());
  testframe.addTestCase (new SelInto());
  if (testframe.run ()) 
    return 0;

  return 1;
}




