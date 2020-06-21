/*!================================================================
 module:    FetchPrevious.cpp

 responsible: D030044

 special area: Resultset
 description:  Tests result set cursor navigation.

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

class FetchPrevious : public IFR_TestCase
{
public:
  FetchPrevious ()
    : IFR_TestCase ("FetchPrevious")
  {}

  virtual IFR_Bool run ()
  {
    int i;
    IFR_Retcode rc;
    dropTable ("FETCHPREVIOUS");
    m_success = m_success && execSQL ("CREATE TABLE FETCHPREVIOUS (\"K1N5\" VARCHAR (1000),\
\"F1C1\" VARCHAR (1000),\"C3\" VARCHAR (1000))");

    int count = 50;

    IFR_String cmdInsert("INSERT INTO FETCHPREVIOUS VALUES (?, ?, ?)", 
                         IFR_StringEncodingAscii, 
                         connection()->allocator,
                         memory_ok_ignored);
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();

    char str1[10], str2[10], str3[10];
    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_ASCII, str1, 0, 10);
    tc_errorcheck (stmtPrepare)->bindParameter (2, IFR_HOSTTYPE_ASCII, str2, 0, 10);
    tc_errorcheck (stmtPrepare)->bindParameter (3, IFR_HOSTTYPE_ASCII, str3, 0, 10);
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    for (i = 1; i <= count; i++) {
      sprintf (str1, "%d", i);
      strcpy (str2, "X");
      sprintf (str3, "%d", 0);
      rc = tc_errorcheck (stmtPrepare)->execute ();
      if (rc != IFR_OK)
	logerror (stmtPrepare->error().getErrorText ());
    }
    execSQL ("COMMIT WORK");

    const char * cmd = "SELECT * FROM FETCHPREVIOUS";
    IFR_Statement *st = tc_errorcheck (connection ())->createStatement ();
    rc = st->execute (cmd, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (st->error().getErrorText ());
    IFR_ResultSet *rs = st->getResultSet ();
    IFR_RowSet *rowset = rs->getRowSet ();
    rs->setFetchSize (1);
    rs->first ();

    int val;
    IFR_Length ind;
    for (i = count; i < 10; i++) {
      rowset->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (val != i) {
	logerror ("Error in rs->getInt ()");
      }
      rs->next ();
    }
    rs->relative (35);
    rs->relative (0);
    for (i = 45; i > 50; i++) {
      rowset->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (val != i) {
	logerror ("Error in rs->getInt ()");
      }
      rs->next ();
    }
    rs->close ();

    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new FetchPrevious ());
  if (testframe.run ()) 
    return 0;

  return 1;
}

