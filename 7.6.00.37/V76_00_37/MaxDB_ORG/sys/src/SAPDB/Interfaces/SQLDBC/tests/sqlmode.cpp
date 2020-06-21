/*!================================================================

 module:        sqlmode.cpp

 -------------------------------------------------------------------

 responsible:   D025086

 special area:  SQL

 description:   Test setSQLMode

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

class SqlMode : public SQLDBC_TestCase
{
public:
  SqlMode ()
    : SQLDBC_TestCase ("SqlMode")
  {}

  virtual SQLDBC_Bool prepare()
  {    
    dropTable("TESTSQMLODE");
    execSQL("CREATE TABLE TESTSQMLODE ( C CHAR(10), I NUMERIC(10))");    
    return m_success;
  }

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Retcode rc;

    SQLDBC_StringEncoding encoding;

    connection()->setSQLMode(SQLDBC_ORACLE);
    if (connection()->error()) {
      logerror ("setSQLMode = SQLDBC_ORACLE failed");      
    }
    static char *Cmd = (char *) "INSERT TESTSQMLODE VALUES ( ' ', 1)";
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
    
    rc = tc_expectederrorcheck(stmt, -5015)->execute(buffer, encoding);
    if (rc != SQLDBC_OK) {
      printf("INSERT cmd without INTO should not work SQLMODE=ORACLE\n");
    }
    else {
      printf("ERROR : Execution of insert returns wrong errorcode\n");
      logerror (stmt->error().getErrorText());      
    }

    /* set the corect SQLMODE and the insert should work */
    connection()->setSQLMode(SQLDBC_INTERNAL);
    if (connection()->error()) {
      logerror ("setSQLMode = SQLDBC_INTERNAL failed");      
    }
    rc = tc_errorcheck(stmt)->execute(buffer, encoding);
    if (rc == SQLDBC_OK) {
      printf("Insert successfull executed\n");
    }
    else {
      printf("ERROR : Insert failed\n");
      logerror (stmt->error().getErrorText());      
    }

    connection()->rollback();
    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new SqlMode());
  if (testframe.run ()) 
    return 0;

  return 1;
}




