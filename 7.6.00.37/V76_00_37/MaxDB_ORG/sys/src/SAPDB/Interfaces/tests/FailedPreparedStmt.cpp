/*!
  @file           FailedPreparedStmt.cpp
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

static IFR_Bool memory_ok_ignored=true;

class FailedPreparedSQL : public IFR_TestCase
{
    friend class IFR_TestFrame;
public:
  FailedPreparedSQL ()
    : IFR_TestCase ("FailedPreparedSQL")
  {}

  virtual IFR_Bool run ()
  {
    // check execute a SQL statement without prepare
    this->ExecWithoutPrepare();
    // check wrong SQL
    this->execSTMT((char*)"WRONG SQL", -3005);
    // check empty SQL
    this->execSTMT((char*)"", -10601);
    // check parameter changing and parse again
    this->tabledefChanged();
    // check a big statement
    this->longSTMT();
    // check with a broken connection
    this->crashSTMT((char*)"SELECT * FROM DUAL");
    return m_success;
  }

private : 
  void ExecWithoutPrepare()
  {
    IFR_Retcode rc;
    IFR_PreparedStmt *stmt = tc_errorcheck (connection ())->createPreparedStatement ();

    { rc = tc_expectederrorcheck(stmt, -10210)->execute(); }
    { rc = tc_errorcheck(stmt)->prepare("SELECT * FROM DUAL", IFR_NTS, IFR_StringEncodingAscii); }
    { rc = tc_errorcheck(stmt)->execute(); }

    IFR_PreparedStmt *stmt1;
    { stmt1 = tc_errorcheck(connection())->createPreparedStatement (); }
    { rc = tc_expectederrorcheck(stmt1, -10210)->execute(); }
    { rc = tc_expectederrorcheck(stmt1, -3005)->prepare("WRONG SQL", IFR_NTS, IFR_StringEncodingAscii); }
    { rc = tc_expectederrorcheck(stmt1, -10210)->execute(); }
    if (m_success)
      loginfo("Execute without prepare SUCCESS\n");
    else
        logerror("Execute without prepare FAILED\n");
    connection()->releaseStatement(stmt1);
  }

private : 
  void execSTMT(char *szSQL, IFR_Int4 expected_error)
  {
    IFR_Retcode rc;
    IFR_PreparedStmt *stmt = tc_errorcheck (connection ())->createPreparedStatement ();
    IFR_String sql(szSQL, IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);

    loginfo("SQL='%s'\n", szSQL);
    // do not call tc_expectederrorcheck because it clears error()
    rc = stmt->prepare(szSQL, IFR_NTS, IFR_StringEncodingAscii);
    IFR_Int4 error_code = stmt->error().getErrorCode();
    if (rc != IFR_OK && error_code == expected_error)
      loginfo ("Expected error (%d) has been returned.\n", error_code);      
    else
      logerror("Wrong error code (%d) has been returned. Error code should be (%d).\n", error_code, expected_error);      

    if (stmt->execute() == IFR_OK)
      logerror("Check execution FAILED.\n");      
    else
      loginfo("Check execution SUCCESS.\n");

    if (stmt->getResultSet() != 0)
      logerror("Check hasResultSet FAILED.\n");      
    else
      loginfo("Check hasResultSet SUCCESS.\n");
      
    IFR_ResultSet *result = stmt->getResultSet();
    if (result) {
      logerror("Check getResultSet FAILED\n");      
    }
    else
      loginfo("Check getResultSet SUCCESS\n");
    
    // execute a correct SQL command to see if the errorcode is cleared.
    rc = stmt->prepare ("SELECT * FROM DUAL", IFR_NTS, IFR_StringEncodingAscii);
    if (rc == IFR_OK)
      rc = stmt->execute ();
    if (rc != IFR_OK)
      logerror("Wrong returncode (%d) returned.\n", rc);
    error_code = stmt->error().getErrorCode();
    if (error_code != 0)
      logerror("Wrong error code (%d) returned.\n", error_code);      

    if (m_success)
      loginfo("Check SQL statement SUCCESS.\n");
    else
      logerror("Check SQL statement FAILED.\n");

  }

    void tabledefChanged()
    {
        dropTable("TEST_TABLEDEF");
        m_success = m_success &&  execSQL("CREATE TABLE TEST_TABLEDEF (A INTEGER, B INTEGER)");
        IFR_PreparedStmt *ps=tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("INSERT INTO TEST_TABLEDEF VALUES (?, ?)", IFR_NTS, IFR_StringEncodingAscii);
        IFR_Int4 data=17;
        IFR_Length l=sizeof(IFR_Int4);
        
        { tc_errorcheck(ps)->bindParameter(1, IFR_HOSTTYPE_INT4, &data, &l, sizeof(IFR_Int4));}
        {tc_errorcheck(ps)->bindParameter(2, IFR_HOSTTYPE_INT4, &data, &l, sizeof(IFR_Int4));}
        
        // now shredder the table
        dropTable("TEST_TABLEDEF");
        execSQL("CREATE TABLE TEST_TABLEDEF (A TIMESTAMP, B TIMESTAMP)");
        
        tc_expectederrorcheck(ps, -10404)->execute();
        connection()->releaseStatement(ps);
    }
        
        
    

  void longSTMT()
  {
    IFR_Retcode rc;
    IFR_Int4 PacketSize;
    char *SQL;
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
        for(int i=0; i < PacketSize;i++)
          SQL[i] = '0'+(i%10);
        SQL[PacketSize-1] = '\0';
        SQL[PacketSize-2] = 'E';
        SQL[0] = 'A';
      }
      IFR_PreparedStmt *stmt = tc_errorcheck (connection())->createPreparedStatement ();
      IFR_String sql(SQL, IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
      rc = tc_expectederrorcheck(stmt, -10901)->prepare(sql.getBuffer(), IFR_NTS, sql.getEncoding());
    }
    if (m_success)
      loginfo("Long SQL statement SUCCESS.\n");
    else
      logerror("Long SQL statement FAILED.\n");
  }
  
  IFR_Connection *reconnect()
  {
    loginfo("Reconnect\n");
    IFR_Connection *conn = environment()->getConnection();
    {tc_errorcheck(conn)->connect(hostname(), IFR_NTS, database(), IFR_NTS, username(), IFR_NTS, password(), IFR_NTS, IFR_StringEncodingAscii);}
    return conn;
  }

  void crashSTMT(char *szSQL)
  {

    IFR_Connection *conn = connection();
    IFR_PreparedStmt *stmt = tc_errorcheck (conn)->createPreparedStatement ();
    IFR_String sql(szSQL, IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
      
    CrashMe chk(conn);    
    loginfo("Break down the connection\n");
    chk.BreakConnection();
    
    // The calls should work becaus the parsinfocache is active
    {tc_errorcheck(stmt)->prepare (sql.getBuffer(), IFR_NTS, sql.getEncoding());}
    
    {tc_errorcheck(stmt)->prepare (sql.getBuffer(), IFR_NTS, sql.getEncoding());}
    
    // append one space to the buffer and the statement should reparsed
    sql.append(" ", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);
    {tc_expectederrorcheck(stmt, -10807)->prepare (sql.getBuffer(), IFR_NTS, sql.getEncoding());}
    
    {tc_expectederrorcheck(stmt, -10821)->prepare (sql.getBuffer(), IFR_NTS, sql.getEncoding());}
    
    {tc_errorcheck(conn)->connect(hostname(), IFR_NTS, database(), IFR_NTS, username(), IFR_NTS, password(), IFR_NTS, IFR_StringEncodingAscii);}
    
    {stmt = tc_errorcheck (conn)->createPreparedStatement ();}
    
    {tc_errorcheck(stmt)->prepare(sql.getBuffer(), IFR_NTS, sql.getEncoding());}
    
    // When a parse info cache is enabled, a breakdown of a connection
    // is not immediately visible.
    chk.BreakConnection();
    {tc_expectederrorcheck(stmt, -10807)->execute();}
    {tc_errorcheck(conn)->connect(hostname(), IFR_NTS, database(), IFR_NTS, username(), IFR_NTS, password(), IFR_NTS, IFR_StringEncodingAscii);}
    
    {stmt = tc_errorcheck (conn)->createPreparedStatement ();}
    
    {tc_errorcheck(stmt)->prepare(sql.getBuffer(), IFR_NTS, sql.getEncoding());}
    
    {tc_errorcheck(stmt)->execute();}
    
    if (m_success)
      loginfo("crashSTMT SUCCESS\n");
    else
      loginfo("crashSTMT FAILED\n");
  }
  
};


int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new FailedPreparedSQL ());
  if (testframe.run ()) 
    return 0;

  return 1;
}




