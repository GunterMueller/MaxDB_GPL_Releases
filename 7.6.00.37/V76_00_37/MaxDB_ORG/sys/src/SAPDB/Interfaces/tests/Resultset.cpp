/*!================================================================

 module:        Reusltset.cpp

 -------------------------------------------------------------------

 responsible:   D025086

 special area:  SQL

 description:   Example: Simple Resultset test

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

#include <stdio.h>
#include <string.h>
#include "Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"
#include "Interfaces/tests/Util.h"

class HasResultset : public IFR_TestCase
{
public:
  HasResultset()
    :IFR_TestCase("HasResultset")
  {}

  IFR_Bool checkResultset(IFR_ResultSet *resultset) {

    if (!resultset) {
      logerror ("No Resultset returned\n", __FILE__, __LINE__);
      return false;
    }

    IFR_Retcode rc;
    IFR_RowSet *rowset = resultset->getRowSet ();

    if (resultset->getResultCount() == 0) {
      printf("No resultset exists.\n");
    }
    rc = resultset->next();
    if (rc != IFR_OK)
      logerror (resultset->error().getErrorText ());

    char s[11];
    IFR_Length ind;
    strcpy(s, "          ");
    if (rowset->getObject (1, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("Error in getting string.\n");
    else {
      printf ("s: %s\n", s);
      if (strcmp ("ONE       ", s)) {
	logerror ("Retrieved value differs from expected value, %s with length %d.\n", s, strlen(s));
      } else {
	printf ("Retrieved value and expected value identical.\n");
      }
    }

    rc = resultset->next();
    if (rc != IFR_OK)
      logerror (resultset->error().getErrorText ());

    strcpy(s, "          ");
    if (rowset->getObject (1, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("Error in getting string.\n", __FILE__, __LINE__);
    else {
      printf ("s: %s\n", s);
      if (strcmp ("TWO       ", s)) {
	logerror ("Retrieved value differs from expected value.\n");
      } else {
	printf ("Retrieved value and expected value identical.\n");
      }
    }
    return m_success;
  }

  IFR_Bool runPreparedStmt() {
    IFR_Retcode rc;

    IFR_PreparedStmt *stmt = tc_errorcheck (connection ())->createPreparedStatement ();

    IFR_StringEncoding encoding = (connection()->isUnicodeDatabase()) ? 
      IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
      
    IFR_String sql(encoding, connection()->allocator);
    IFR_Bool memory_ok_ignored=true;
    sql.append("SELECT * FROM ABC", IFR_StringEncodingAscii,  IFR_NTS, memory_ok_ignored);

    IFR_String name ("", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);

    stmt->setCursorName (name.getBuffer(), IFR_NTS, name.getEncoding());
      
    rc = tc_errorcheck (stmt)->prepare(sql.getBuffer(), sql.getEncoding());
    if (rc != IFR_OK)
      logerror (stmt->error().getErrorText ());
    
    rc = tc_errorcheck (stmt)->execute();
    if (rc != IFR_OK)
      logerror (stmt->error().getErrorText ());
    
    IFR_ResultSet *resultset = tc_errorcheck (stmt)->getResultSet();

    m_success = m_success && checkResultset(resultset);
    
    return m_success;
  }

  IFR_Bool runStatement() {
    IFR_Retcode rc;

    IFR_StringEncoding encoding = (connection()->isUnicodeDatabase()) ? 
      IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
      
    IFR_String sql(encoding, connection()->allocator);
    IFR_Bool memory_ok_ignored=true;
    sql.append("SELECT * FROM ABC", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);

    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    IFR_String name ("CURSOR1", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);

    stmt->setCursorName (name.getBuffer(), IFR_NTS, name.getEncoding());
      
    rc = tc_errorcheck (stmt)->execute(sql.getBuffer(), sql.getEncoding());
    if (rc != IFR_OK)
      logerror (stmt->error().getErrorText ());
    
    IFR_ResultSet *resultset = tc_errorcheck (stmt)->getResultSet();

    m_success = m_success && checkResultset(resultset);
    
    return m_success;
  }

  virtual IFR_Bool run()
  {
    dropTable ("ABC");

    m_success = m_success && execSQL ("CREATE TABLE ABC (C CHAR (10))");
      
    m_success = m_success && execSQL ("INSERT INTO ABC VALUES ('ONE')");

    m_success = m_success && execSQL ("INSERT INTO ABC VALUES ('TWO')");

    m_success = m_success && runStatement();

    m_success = m_success && runPreparedStmt();
    execSQL("COMMIT WORK");
    return m_success;
  } // end run
};

int main(int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe(argc, argv);
  testframe.addTestCase(new HasResultset());
  if(testframe.run()) 
    return 0;

  return 1;
}


