/*!
  @file           pspytest.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Test of the Packet Spy clazz.
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
#include <string.h>
#include "Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"
#include "SAPDB/Interfaces/tests/PacketSpy.h"
#include "SAPDB/Interfaces/tests/CrashMe.h"

#define MAXROWS (2000)
#define ROWSETSIZE (21)
#define COLSIZE (30)
#define ROWFMT "ROW%d"

#define UNUSEDFMT "NOROW%d"
IFR_Bool memory_ok_ignored=true;

class pSpyTest : public IFR_TestCase
{
public:
  pSpyTest()
    :IFR_TestCase("pSpyTest")
  {}

    IFR_Bool Test2() {
        IFR_Retcode rc;
        PacketSpyRuntime *packetspy = new PacketSpyRuntime();
        IFR_Environment *env = new IFR_Environment (packetspy);
        IFR_Connection *conn = env->getConnection();
        conn->connect(hostname(), IFR_NTS, database(), IFR_NTS, username(), IFR_NTS, password(), IFR_NTS, IFR_StringEncodingAscii);
        CrashMe chk(conn);
        if(!chk.CheckConnection()) {
            logerror ("Not connected");      
            return m_success;
        }

    IFR_PreparedStmt *stmt;
    { stmt = tc_errorcheck (conn)->createPreparedStatement (); }
    IFR_StringEncoding encoding = (conn->isUnicodeDatabase()) ? 
      IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
    IFR_String sql(encoding, conn->allocator);

    sql.append("SELECT * FROM DUAL", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);
    
    // A test wich returns a errortext part on the next request/receive

    const char *sqlmsg = "simulated sql error message";
    {
      // The next recieve call will return with the given sql code
      // and an errotext part containing the sqlmsg

      const IFR_String tmp(sqlmsg, IFR_StringEncodingAscii, conn->allocator, memory_ok_ignored);
      packetspy->setSQLError(-3005, tmp);

	  rc = stmt->prepare(sql.getBuffer(), sql.getEncoding());
    }
    if (rc == IFR_OK) {
      logerror ("The packetspy SQLError doesn't work.");
      return m_success;
    }
    if (stmt->error().getErrorCode() != -3005) {
      logerror ("Wrong sql code %d expected -3005");
      return m_success;      
    }
    printf("Check sql code SUCCESS\n");              

    // Check the returned sql error message. 
    // It should set by the packet spy in a error text part
    const char *msg = stmt->error().getErrorText();
    loginfo("%s", msg);
    int msglen = strlen(msg);
    int sqlmsglen = strlen(sqlmsg);
    char *c;
    for(c = (char *)msg; *c != '\0'; c++) {
      if (msglen >= sqlmsglen && !memcmp(c, sqlmsg, sqlmsglen)) {
        printf("Check sql error message SUCCESS\n");          
        break;
      }
    }
    if (c == '\0') {
      logerror ("Wrong sql error message (%s)", msg);
    }    
    return m_success;
    }

  IFR_Bool Test1() {
    IFR_Retcode rc;
    PacketSpyRuntime *packetspy = new PacketSpyRuntime();
    IFR_Environment *env = new IFR_Environment (packetspy);
    IFR_Connection *conn = env->getConnection();
    conn->connect(hostname(), IFR_NTS, database(), IFR_NTS, username(), IFR_NTS, password(), IFR_NTS, IFR_StringEncodingAscii);
    CrashMe chk(conn);
    if(!chk.CheckConnection()) {
        logerror ("Not connected");      
        return m_success;
    }
      
    IFR_PreparedStmt *stmt;
    { stmt = tc_errorcheck (conn)->createPreparedStatement (); }

    IFR_StringEncoding encoding = (conn->isUnicodeDatabase()) ? 
      IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
      
    IFR_String sql(encoding, conn->allocator);

    sql.append("SELECT * FROM DUAL", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);

    const char *errmsg = "commErrNotOk_esp01";
    IFR_String name ("", IFR_StringEncodingAscii, conn->allocator, memory_ok_ignored);
    {
      // Next request/recieve call will return the given error code
      packetspy->setRTEError(commErrNotOk_esp01, errmsg);

      // Within the prepare the packetspy simulates the RTE error message
      rc = stmt->prepare(sql.getBuffer(), sql.getEncoding());
    }
    
    if (rc == IFR_OK) {
      logerror ("The packetspy RTEError doesn't work.");
      return m_success;
    }
    if (stmt->error().getErrorCode() != -10807) {
      logerror ("Wrong error code %d expected -10807", stmt->error().getErrorCode());
      return m_success;      
    }
    printf("Check error code SUCCESS\n");              

    // Check the returned message. It should set by the packet spy
    const char *msg = stmt->error().getErrorText();
    loginfo("%s", msg);
    int msglen = strlen(msg);
    int errmsglen = strlen(errmsg);
    const char *c = msg;
    for(; *c != '\0'; c++) {
      if (msglen >= errmsglen && !memcmp(c, errmsg, errmsglen)) {
        printf("Check rte error message SUCCESS\n");          
        break;
      }
    }
    if (*c == '\0') {
      logerror ("Wrong rte error message (%s)", msg);
    }
    
    // The execution should fail because th command wasn't prepared
    { rc = tc_expectederrorcheck (stmt, -10821)->execute(); }
    
    return m_success;
  }

  virtual IFR_Bool run()
  {
    m_success = true;
    Test2();
    Test1();
    return m_success;
  } // end run
};

int main(int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe(argc, argv);
  testframe.addTestCase(new pSpyTest());
  if(testframe.run()) 
    return 0;

  return 1;
}


