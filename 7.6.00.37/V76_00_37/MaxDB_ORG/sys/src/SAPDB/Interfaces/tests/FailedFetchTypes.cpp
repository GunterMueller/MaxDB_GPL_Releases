/*!================================================================
 module:    FailedFetchTypes.cpp

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
#include "SAPDB/Interfaces/tests/CrashMe.h"

IFR_Bool memory_ok_ignored=true;

class FailedFetchTypes : public IFR_TestCase
{
public:
  FailedFetchTypes ()
    : IFR_TestCase ("FailedFetchTypes")
  {}

  virtual IFR_Bool run ()
  {
    dropTable ("FETCHTYPES");
    m_success = m_success && execSQL ("CREATE TABLE FETCHTYPES (A INTEGER, B CHAR(1000) ASCII)");

    //    int insertedRows[] = { 100, 333, 2000 };
    int insertedRows[] = { 2000 };

    for (int j = 0; j < sizeof (insertedRows) / sizeof (int); j++) {
      this->insertRows (insertedRows[j]);
      this->testNavigation (insertedRows[j]);
    }

    IFR_Statement *stmt = tc_expectederrorcheck (connection (), -10807)->createStatement ();
    tc_expectederrorcheck(stmt, -10821)->execute ("COMMIT WORK", IFR_StringEncodingAscii);
    return m_success;
  }

private:
  void insertRows (int insertedRows) {
    IFR_Retcode rc;
    int i;

    IFR_String a256 ("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaa", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    execSQL ("DELETE FROM FETCHTYPES");
    IFR_String cmdInsert ("INSERT INTO FETCHTYPES VALUES (?, \'",
                          IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    cmdInsert.append(a256, memory_ok_ignored);
    cmdInsert.append("\')", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();
    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_INT4, &i, 0, 4);
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    for (i = 1; i <= insertedRows; i++) {
      rc = tc_errorcheck (stmtPrepare)->execute ();
      if (rc != IFR_OK)
	logerror (stmtPrepare->error().getErrorText ());
    }
  }

  void testNavigation (int insertedRows) {
    int val;
    IFR_Length ind;
    IFR_Retcode rc;
    const char *cmdSelect = "SELECT A, B FROM FETCHTYPES ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();

    if (stmt->execute (cmdSelect, IFR_StringEncodingAscii) == IFR_NOT_OK) {
      logerror (stmt->error().getErrorText ());
    }

    IFR_ResultSet *rs = stmt->getResultSet ();
    IFR_RowSet *rowset = rs->getRowSet ();
    rc = rs->beforeFirst ();
    if (rc == IFR_NOT_OK) {
      logerror (rs->error().getErrorText ());
    }

    if (rs->getRow () != 0) {
      logerror ("Error in rs->getRow ()");
    }

    rc = rs->afterLast ();
    if (rc == IFR_NOT_OK) {
      logerror (rs->error().getErrorText ());
    }
    if (rs->next () == IFR_NO_DATA_FOUND) {
      loginfo ("Expected failure of rs->next()");
    } else {
      logerror ("Errorcode invalid of rs-next()");
    }

    if (rs->relative (10) == IFR_NOT_OK) {
      loginfo (rs->error().getErrorText ());
    } else {
      logerror ("Errorcode invalid of rs->relative()");
    }

    if (rowset->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false) == IFR_NOT_OK) {
      loginfo ("Expected failure of rs->getInt()");
    } else {
      logerror ("Errorcode invalid of rs->getInt()");
    }

    if (rowset->getObject (24, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false) == IFR_NOT_OK) {
      loginfo ("Expected failure of rs->getInt()");
    } else {
      logerror ("Errorcode invalid of rs->getInt()");
    }

    rs->absolute (1);
    if (rowset->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false) == IFR_OK) {
      loginfo ("Value: %d", val);
    } else {
      logerror ("Errorcode invalid of rs->getInt()");
    }

    rs->afterLast ();
    if (rs->isAfterLast () == false) {
      logerror ("Wrong value of rs->isAfterLast ()");
    }
    rs->next ();
    rs->previous ();
    if (rowset->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false) == IFR_OK) {
      if (val != insertedRows) {
	logerror ("Wrong value from rs->getObject()");
      } else {
	loginfo ("Value: %d", val);
      }
    } else {
      logerror ("Errorcode invalid of rs->getObject()");
    }

    CrashMe chk(connection());    
    chk.BreakConnection();

    // now work without connection to database.
    rs->absolute (1);
    rs->relative (10*insertedRows);
    if (rowset->getObject(insertedRows, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false) == IFR_OK)
      logerror (rs->error().getErrorText ());

    rs->absolute (5*insertedRows);

    rs->close ();
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new FailedFetchTypes ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
