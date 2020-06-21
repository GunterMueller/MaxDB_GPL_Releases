/*!
  @file           FailedStmt.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check wrong execution of SQL Statements.
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
#include <stdio.h>
#include "Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"
#include "SAPDB/Interfaces/tests/CrashMe.h"
#include "SAPDB/Interfaces/tests/PacketSpy.h"

static IFR_Bool memory_ok_ignored=true;

class FailedSQL : public IFR_TestCase
{
public:
  FailedSQL ()
    : IFR_TestCase ("FailedSQL")
  {}

  virtual IFR_Bool run ()
  {
    //    connection()->enablePacketTrace();
    // check wrong SQL
    this->execSTMT((char*)"WRONG SQL", IFR_StringEncodingAscii, -3005);
    // check empty SQL
    this->execSTMT((char*)"", IFR_StringEncodingAscii, -10601);

    if (!connection()->isUnicodeDatabase()) {
      // check real UTF8 SQL
      this->execSTMT("SELECT 'T\xC3\xA4st' FROM DUAL", IFR_StringEncodingUTF8, -10602);
      // check UTF8 CursorName
      this->execCursor((char*)"T\xC3\xA4st", IFR_StringEncodingUTF8, -10808);
    }

    // check a big statement
    this->longSTMT();
    
    // check with a broken connection
    this->crashSTMT((char*)"SELECT * FROM DUAL");

    // check result space exhausted (releases the session)
    this->crashSTMT((char*)"SELECT * FROM DUAL", -904);

    return m_success;
  }

private : 
  void execSTMT(char *szSQL, IFR_StringEncoding encoding, IFR_Int4 expected_error)
  {
    IFR_Retcode rc;
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    // IFR_String sql(szSQL);

    loginfo("Check SQL statement\n");
    loginfo("SQL='%s'\n", szSQL);
    // do not call tc_expectederrorcheck because it clears error()
    rc = stmt->execute (szSQL, encoding);
    IFR_Int4 error_code = stmt->error().getErrorCode();
    if (rc != IFR_OK && error_code == expected_error) {
      loginfo ("Expected error (%d) has been returned.\n", error_code);      
    }
    else {
      logerror("Wrong error code (%d) has been returned. Error code should be (%d).\n", error_code, expected_error);      
    }
    loginfo("Check hasResultSet\n");
    if (stmt->getResultSet() != 0) {
      logerror("Methode hasResultSet returns a wrong value.\n");      
    }
    else
      loginfo("Statement has no resultset.\n");
      
    loginfo("Check getResultSet\n");
    IFR_ResultSet *result = stmt->getResultSet();
    if (result) {
      logerror("Statement should not return a resultset.\n");      
    }
    else
      loginfo("getResultSet returns NULL.\n");
    
    // execute a correct SQL command to see if the errorcode is cleared.
    rc = stmt->execute ("SELECT * FROM DUAL", IFR_StringEncodingAscii);
    if (rc != IFR_OK)
      logerror("Wrong returncode (%d) returned.\n", rc);
    error_code = stmt->error().getErrorCode();
    if (error_code != 0)
      logerror("Wrong error code (%d) returned.\n", error_code);      
  }

  void execCursor(char *szCursorName, IFR_StringEncoding encoding, IFR_Int4 expected_error)
  {
    IFR_Retcode rc;
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    // IFR_String sql(szSQL);

    loginfo("Check setCursorName\n");

    tc_errorcheck(stmt)->setCursorName(szCursorName, IFR_NTS, encoding);

    rc = stmt->execute("SELECT * FROM DUAL", IFR_StringEncodingAscii);
    IFR_Int4 error_code = stmt->error().getErrorCode();
    if (rc != IFR_OK && error_code == expected_error) {
      loginfo ("Expected error (%d) has been returned.\n", error_code);      
    }
    else {
      logerror("Wrong error code (%d) has been returned. Error code should be (%d).\n", error_code, expected_error);      
    }
    loginfo("Check hasResultSet\n");
    if (stmt->getResultSet() != 0) {
      logerror("Methode hasResultSet returns a wrong value.\n");      
    }
    else
      loginfo("Statement has no resultset.\n");
    
    connection()->releaseStatement(stmt);
  }

  void longSTMT()
  {
    IFR_Retcode rc;
    IFR_Int4 PacketSize;
    char *SQL = 0;
    rc = getPacketSize(PacketSize);
    if (rc != IFR_OK) {
      printf("Can't determine PACKET_SIZE\n");
      m_success = false;
    }
    else {
      printf("PacketSize = %d\n", PacketSize);
      SQL = (char*)malloc(PacketSize);
      if (!SQL) {
        logerror ("Memory allocation failed.\n");
      }
      else {
        memset(SQL, 'x', PacketSize);
        SQL[PacketSize-1] = '\0';
      }
      IFR_PreparedStmt *stmt = tc_errorcheck (connection())->createPreparedStatement ();
      IFR_String sql(SQL, IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
      rc = tc_expectederrorcheck(stmt, -10901)->prepare(sql.getBuffer(), sql.getEncoding());
      if (rc != IFR_OK) {
          loginfo("Errormessage is OK because statement too long\n");
      }
    }
    execSQL ("COMMIT WORK");

    if(SQL) {
        free(SQL);
    }
  }
  
  void crashSTMT(char *szSQL)
  {
    IFR_Statement *stmt; 
    { stmt = tc_errorcheck (connection ())->createStatement (); }
    IFR_String sql(szSQL, IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);

    loginfo("Break down the connection\n");
    CrashMe chk(connection());
    chk.BreakConnection();
    tc_expectederrorcheck(stmt, -10807)->execute (szSQL, IFR_StringEncodingAscii);

    if (m_success)
      loginfo("crashSTMT SUCCESS\n");
    else
      loginfo("crashSTMT FAILED\n");
  }

  void crashSTMT(char *szSQL, IFR_Int4 sql_error)
  {
    PacketSpyRuntime *packetspy = new PacketSpyRuntime();
    IFR_Environment *env = new IFR_Environment (packetspy);
    IFR_Connection *conn = env->getConnection();
    conn->connect(hostname(), IFR_NTS, database(), IFR_NTS, username(), IFR_NTS, password(), IFR_NTS, IFR_StringEncodingAscii, connectproperties());

    IFR_Statement *stmt; 
    stmt = tc_errorcheck(conn)->createStatement();
    IFR_String sql(szSQL, IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    IFR_String sqlError("", IFR_StringEncodingAscii, conn->allocator, memory_ok_ignored);
    packetspy->setSQLError(sql_error, sqlError);

    tc_expectederrorcheck(stmt, sql_error)->execute(szSQL, IFR_StringEncodingAscii);

    if (m_success)
      loginfo("crashSTMT with %d SUCCESS\n", sql_error);
    else
      loginfo("crashSTMT with %d FAILED\n", sql_error);
  }

  IFR_Connection *reconnect()
  {
    loginfo("Reconnect\n");
    IFR_Connection *conn = environment()->getConnection();
    tc_errorcheck(conn)->connect(hostname(), IFR_NTS, database(), IFR_NTS, username(), IFR_NTS, password(), IFR_NTS, IFR_StringEncodingAscii, connectproperties());
    return conn;
  }

};


int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new FailedSQL ());
  if (testframe.run ()) 
    return 0;

  return 1;
}




