/*!================================================================
 module:    FetchTypes.cpp

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

#define CHECK_SIZE 5
IFR_Bool memory_ok_ignored=true;

class FetchTypes : public IFR_TestCase
{
public:
  FetchTypes ()
    : IFR_TestCase ("FetchTypes")
  {}

  virtual IFR_Bool run ()
  {
    dropTable ("FETCHTYPES");
    m_success = m_success && execSQL ("CREATE TABLE FETCHTYPES (A INTEGER, B CHAR(1000) ASCII)");

    int insertedRows[] = { 100, 333, 2000 };

    for (int j = 0; j < sizeof (insertedRows) / sizeof (int); j++) {
      this->insertRows (insertedRows[j]);
      this->beforeFirst (insertedRows[j]);
      this->afterLast (insertedRows[j]);
      this->fetchForward (insertedRows[j]);
      this->fetchBackward (insertedRows[j]);
      this->fetchAbsolute (insertedRows[j], true);
      this->fetchAbsolute (insertedRows[j], false);
      this->fetchRelative (insertedRows[j]);
    }

    execSQL ("COMMIT WORK");
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
    IFR_String cmdInsert ("INSERT INTO FETCHTYPES VALUES (?, \'"
                          , IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    cmdInsert.append(a256, memory_ok_ignored);
    cmdInsert.append("\')", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_INT4, &i, 0, 4);
    for (i = 1; i <= insertedRows; i++) {
      rc = tc_errorcheck (stmtPrepare)->execute ();
      if (rc != IFR_OK) {
          logerror (stmtPrepare->error().getErrorText ());
      }
    }
  }
    
  void beforeFirst (int insertedRows) {
    IFR_Retcode rc;
    const char *cmdSelect = "SELECT A, B FROM FETCHTYPES ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    IFR_ResultSet *rs2 = stmt->getResultSet ();
    rc = rs2->last ();
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    rc = rs2->beforeFirst ();
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    rc = rs2->next ();
    if (insertedRows != 0) {
      if (rc == IFR_NOT_OK) {
	logerror ("Error in rs2->next ()");	
      }
      if (rs2->getRow () != 1) {
	logerror ("Error in rs2->getRow ()");
      }
    } else {
      if (rc != IFR_NO_DATA_FOUND) {
	logerror ("Error in rs2->next ()");	
      }
      if (rs2->getRow () != 0) {
	logerror ("Error in rs2->getRow ()");
      }
    }
    rs2->close ();
  }

  void afterLast (int insertedRows) {
    IFR_Retcode rc;
    const char *cmdSelect = "SELECT A, B FROM FETCHTYPES ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    IFR_ResultSet *rs2 = stmt->getResultSet ();
    rc = rs2->next ();
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    rc = rs2->afterLast ();
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    if (rs2->getRow () != 0) {
      logerror ("Error in rs2->getRow ()");      
    }
    rc = rs2->previous ();
    if (insertedRows != 0) {
      if (rc == IFR_NOT_OK) {
	logerror ("Error in rs2->previous ()");	
      }
      if (rs2->getRow () != insertedRows) {
	logerror ("Error in rs2->getRow ()");
      }
    } else {
      if (rc != IFR_NO_DATA_FOUND) {
	logerror ("Error in rs2->next ()");	
      }
      if (rs2->getRow () != 0) {
	logerror ("Error in rs2->getRow ()");
      }
    }
    rs2->close ();
  }

  void fetchForward (int insertedRows) {
    IFR_Retcode rc;
    const char *cmdSelect = "SELECT A, B FROM FETCHTYPES ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    IFR_ResultSet *rs2 = stmt->getResultSet ();
    IFR_RowSet *rowset2 = rs2->getRowSet ();

    int i = 0;
    int val;
    IFR_Length ind;
    while (((rc = rs2->next ()) != IFR_NO_DATA_FOUND) &&
	   (rc != IFR_NOT_OK)) {
      if (rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false) == IFR_NOT_OK) {
	logerror (rs2->error ().getErrorText ());
      } else {
	if (val != ++i) {
	  logerror ("Error in rs2->getObject ()");
	}
      }      
    }
    if (insertedRows != i) {
      logerror ("Error insertedRows");
    }
    if (rs2->getRow () != 0) {
      logerror ("Error in rs2->getRow ()");
    }
    rs2->close ();
  }

  void fetchBackward (int insertedRows) {
    IFR_Retcode rc;
    const char * cmdSelect ="SELECT A, B FROM FETCHTYPES ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    IFR_ResultSet *rs2 = stmt->getResultSet ();
    IFR_RowSet *rowset2 = rs2->getRowSet ();

    int i = 0;
    if (rs2->last () == IFR_OK) 
      i++;

    int val;
    IFR_Length ind;
    while (((rc = rs2->previous ()) != IFR_NO_DATA_FOUND) &&
	   (rc != IFR_NOT_OK)) {
      if (rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false) == IFR_NOT_OK) {
	logerror (rs2->error ().getErrorText ());
      } else {
	if (val != (insertedRows-(i++))) {
	  logerror ("Error in rs2->getObject ()");
	}
      }      
    }
    if (insertedRows != i) {
      logerror ("Error insertedRows");
    }
    if (rs2->getRow () != 0) {
      logerror ("Error in rs2->getRow ()");
    }
    rs2->close ();
  }

  void fetchAbsolute (int insertedRows, bool forward) {
    IFR_Retcode rc;
    int direction = (forward) ? 1 : -1;
    int offset = (forward) ? 0 : (insertedRows + 1);
    int checkPos[CHECK_SIZE];
    int val;
    IFR_Length ind;
    const char *cmdSelect = "SELECT A, B FROM FETCHTYPES ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    IFR_ResultSet *rs2 = stmt->getResultSet ();
    IFR_RowSet *rowset2 = rs2->getRowSet ();

    int i, k, l;
    for (k = 1, l = insertedRows / 6; k <= 5; k++) {
      checkPos[k-1] = direction * k * l;
    }

    if (insertedRows != 0) {
      if (rs2->absolute (insertedRows) != IFR_OK) {
	logerror ("Error in rs2->absolute (insertedRows)");
      }
      rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (insertedRows != val) {
	logerror ("Error in rs2->getObject ()");
      }
    }
    for (i = 0; i < CHECK_SIZE; i++) {
      if (checkPos[i] != 0) {
	rs2->absolute (checkPos[i]);
        rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
	if (val != (offset + checkPos[i])) {
	  logerror ("Error in rs2->getObject ()");
	}
      }
    }
    int expected = (checkPos[CHECK_SIZE - 1] == 0) ? 0 : (offset + checkPos[CHECK_SIZE-1]);
    if (expected != rs2->getRow ()) {
      logerror ("Error in rs2->getRow ()");
    }
    if (insertedRows != 0) {
      rs2->absolute (2*insertedRows+1);
      if (!rs2->isAfterLast ()) {
	printf ("NOT AFTER LAST\n");
	logerror ("Error: Cursor is not before first.");
      }
      
      if (rs2->getRow () != 0) {
	logerror ("Error: out of range getRow().");
      }
      rs2->previous ();
      rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (val != insertedRows) {
	logerror ("Error: out of range result value.");
      }
      if (rs2->getRow () != insertedRows) {
	logerror ("Error: out of range result value.");
      }

      rs2->absolute (-2*insertedRows+1);
      if (!rs2->isBeforeFirst ()) {
	printf ("NOT AFTER LAST\n");
	logerror ("Error: Cursor is not before first.");
      }

      if (rs2->getRow () != 0) {
	logerror ("Error: out of range getRow().");
      }
      rs2->next ();
      rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (val != 1) {
	logerror ("Error: out of range result value.");
      }
      if (rs2->getRow () != 1) {
	logerror ("Error: out of range result value.");
      }
      rs2->close ();
    }
  }

  void fetchRelative (int insertedRows) {
    IFR_Retcode rc;
    int stepsize = insertedRows / 6;
    int curpos = 0;
    int i, val;
    IFR_Length ind;
    const char *cmdSelect = "SELECT A, B FROM FETCHTYPES ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = stmt->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc == IFR_NOT_OK)
      logerror (stmt->error().getErrorText ());
    IFR_ResultSet *rs2 = stmt->getResultSet ();
    IFR_RowSet *rowset2 = rs2->getRowSet ();

    if (insertedRows == 0) {
      rs2->relative (0);
      connection()->releaseStatement(stmt);
      return;
    }

    if (rs2->relative (1) != IFR_OK) {
      printf ("Failure: No current row.\n");
      rs2->next ();
      curpos++;
    }

    for (i = 1; i <= 5; i++) {
      curpos += stepsize;
      rs2->relative (stepsize);

      rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (val != curpos) {
	logerror ("Error: Forward check result value.");
      }
      if (rs2->getRow () != curpos) {
	logerror ("Error: Forward check getRow ()");
      }
    }

    for (i = 1; i <= 5; i++) {
      curpos -= stepsize;
      rs2->relative (-stepsize);

      rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (val != curpos) {
	logerror ("Error: Backward check result value.");
      }
      if (rs2->getRow () != curpos) {
	logerror ("Error: Backward check getRow ()");
      }
    }

    if (insertedRows != 0) {
      rs2->first ();
      rs2->relative (2*insertedRows+1);
      if (rs2->getRow () != 0) {
	logerror ("Error: Check out of range getRow ().");
      }
      rs2->previous ();
      rowset2->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
      if (val != insertedRows) {
	logerror ("Error: Check out of range result value.");
      }
      if (rs2->getRow () != insertedRows) {
	logerror ("Error: Check out of range getRow ().");
      }
      rs2->relative (-2*insertedRows+1);
      if (rs2->getRow () != 0) {
	logerror ("Error: Check out of range getRow ().");
      }
    }

    rs2->close ();
    connection()->releaseStatement(stmt);
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new FetchTypes ());
  if (testframe.run ()) 
    return 0;

  return 1;
}

