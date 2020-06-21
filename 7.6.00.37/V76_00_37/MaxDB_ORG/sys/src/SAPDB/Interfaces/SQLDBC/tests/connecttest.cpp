/****************************************************************************

  module      : connecttest.cpp

  -------------------------------------------------------------------------

  responsible : D039759

  special area: Interface Runtime
  description : Tests for some different connect options.


  last changed: 2001-01-20
  see also    :

  -------------------------------------------------------------------------

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




*****************************************************************************/

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

SQLDBC_Bool memory_ok_ignored=true;

class ConnectTest
    : public SQLDBC_TestCase
{
public:
    ConnectTest()
    :SQLDBC_TestCase("ConnectTest")
    {}

    SQLDBC_Bool run()
    {

        SQLDBC_Connection *c1=environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_errorcheck(c1)->connect(m_frame->hostname(),
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       m_frame->username(),
                                       SQLDBC_NTS,
                                       m_frame->password(),
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
        }

        {
          SQLDBC_PreparedStatement *s1 = c1->createPreparedStatement();
          if(s1) {
            tc_errorcheck(s1)->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
            c1->releaseStatement(s1);
          }
        }

        /* second connection */
        {
          SQLDBC_Connection *c2=environment()->createConnection();
          {
            SQLDBC_ConnectProperties p;
            tc_errorcheck(c2)->connect(m_frame->hostname(),
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       m_frame->username(),
                                       SQLDBC_NTS,
                                       m_frame->password(),
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
          }

          {
            SQLDBC_PreparedStatement *s1 = c2->createPreparedStatement();
            if(s1) {
              tc_errorcheck(s1)->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
              c2->releaseStatement(s1);
            }
          }
          environment()->releaseConnection(c2);
        }

        environment()->releaseConnection(c1);

        c1=environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_expectederrorcheck(c1, -10709)->connect("invalid host name",
                                                     SQLDBC_NTS,
                                                      m_frame->database(),
                                                     SQLDBC_NTS,
                                                      m_frame->username(),
                                                     SQLDBC_NTS,
                                                      m_frame->password(),
                                                     SQLDBC_NTS,
                                                     SQLDBC_StringEncodingAscii,
                                                      p);
        }
        environment()->releaseConnection(c1);

        c1=environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_expectederrorcheck(c1, -10709)->connect(m_frame->hostname(),
                                                     SQLDBC_NTS,
                                                      "invalid dbname",
                                                     SQLDBC_NTS,
                                                      m_frame->username(),
                                                     SQLDBC_NTS,
                                                      m_frame->password(),
                                                     SQLDBC_NTS,
                                                     SQLDBC_StringEncodingAscii,
                                                      p);
        }
        environment()->releaseConnection(c1);

        // check for unknown password error
        c1=environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_expectederrorcheck(c1, -4008)->connect(m_frame->hostname(),
                                                     SQLDBC_NTS,
                                                      m_frame->database(),
                                                     SQLDBC_NTS,
                                                      "homer",
                                                     SQLDBC_NTS,
                                                      "simpson",
                                                     SQLDBC_NTS,
                                                      SQLDBC_StringEncodingAscii,
                                                      p);
        }
        environment()->releaseConnection(c1);

        // check invalid parameters
        if (!strcmp("", m_frame->hostname())) {
          // only check with local databases
          c1=environment()->createConnection();
          {
              SQLDBC_ConnectProperties p;
            // NULL as hostname should work
            tc_errorcheck(c1)->connect(NULL,
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       m_frame->username(),
                                       SQLDBC_NTS,
                                       m_frame->password(),
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
          }
          environment()->releaseConnection(c1);
        }

        const char *isnull = 0;
        // check invalid parameters
        c1=environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_expectederrorcheck(c1, -10857)->connect(m_frame->hostname(),
                                                     SQLDBC_NTS,
                                       isnull,
                                                     SQLDBC_NTS,
                                       m_frame->username(),
                                                     SQLDBC_NTS,
                                       m_frame->password(),
                                                     SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
        }
        environment()->releaseConnection(c1);

        // check invalid parameters
        c1=environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_expectederrorcheck(c1, -4008)->connect(m_frame->hostname(),
                                                     SQLDBC_NTS,
                                       m_frame->database(),
                                                     SQLDBC_NTS,
                                       isnull,
                                                     SQLDBC_NTS,
                                       m_frame->password(),
                                                     SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
        }
        environment()->releaseConnection(c1);

        // check invalid parameters
        c1 = environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_expectederrorcheck(c1, -10715)->connect(m_frame->hostname(),
                                                     SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       m_frame->username(),
                                       SQLDBC_NTS,
                                       isnull,
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
        }
        environment()->releaseConnection(c1);

        // check a statement after a failed connection
        c1 = environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            tc_expectederrorcheck(c1, -4008)->connect(m_frame->hostname(),
                                                      SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       "UNKNOWN",
                                       SQLDBC_NTS,
                                       "UNKNOWN",
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
        }
        SQLDBC_PreparedStatement *s1 = c1->createPreparedStatement();
        if(s1) {
            s1->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
            c1->releaseStatement(s1);
        }
        environment()->releaseConnection(c1);

        /* -- disabled, unicode does not really work yet
        // an Unicode client
        c1=new IFR_Connection();
        {
            SQLDBC_ConnectProperties p;
            p.setProperty("UNICODE", "true");
            c1->connect(m_frame->hostname(),
                        SQLDBC_NTS,
                        m_frame->database(),
                        SQLDBC_NTS,
                        m_frame->username(),
                        SQLDBC_NTS,
                        m_frame->password(),
                        SQLDBC_NTS,
                        SQLDBC_StringEncodingAscii,
                        p);
            if(c1->error()) {
                if(c1->error().getErrorCode()!=-4025) {
                    logerror("Connect with UNICODE, error: %d sql state %5s, %s",
                             c1->error().getErrorCode(),
                             c1->error().getSQLState(),
                             c1->error().getErrorText());
                } else {
                    loginfo("Connect with UNICODE, error: %d sql state %5s, %s",
                             c1->error().getErrorCode(),
                             c1->error().getSQLState(),
                             c1->error().getErrorText());
                    loginfo("Unicode was not tested.");
                }
            }
        }
        delete c1;
        */
        // some full-blown example
        c1 = environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            p.setProperty("APPLICATION", "CPC");
            p.setProperty("APPVERSION", "70201");
            p.setProperty("SQLMODE", "ORACLE");
            p.setProperty("TIMEOUT", "5");
            p.setProperty("CACHELIMIT", "100");
            p.setProperty("ISOLATIONLEVEL", "2");
            tc_errorcheck(c1)->connect(m_frame->hostname(),
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       m_frame->username(),
                                       SQLDBC_NTS,
                                       m_frame->password(),
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
        }
        environment()->releaseConnection(c1);
        
        
        c1 = environment()->createConnection();
        {
            SQLDBC_ConnectProperties p;
            p.setProperty("SQL", "TRUE");
            tc_errorcheck(c1)->connect(m_frame->hostname(),
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       m_frame->username(),
                                       SQLDBC_NTS,
                                       m_frame->password(),
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       p);
        }
        environment()->releaseConnection(c1);

        

        return m_success;
    }

};

class XUSERTest
    : public SQLDBC_TestCase
{
public:
    XUSERTest()
    :SQLDBC_TestCase("XUSERTest")
    {}

    SQLDBC_Bool run()
    {

        SQLDBC_Connection *c1=environment()->createConnection();

        SQLDBC_ConnectProperties p;
        p.setProperty("KEY", "unknown");

        // later we should use the properties method only
        tc_expectederrorcheck(c1, -10757)->connect(p);
        p.setProperty("KEY", "SQLOPT");
        // later we should use the properties method only
        tc_errorcheck(c1)->connect(p);

        SQLDBC_PreparedStatement *s1 = c1->createPreparedStatement();
        if(s1) {
          tc_errorcheck(s1)->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
          c1->releaseStatement(s1);
        }

        environment()->releaseConnection(c1);
        return m_success;
    }

};

class SQLOPTDynamicTest
    : public SQLDBC_TestCase
{
public:
    SQLOPTDynamicTest()
    :SQLDBC_TestCase("SQLOPTDynamicTest")
    {}

    SQLDBC_Bool run()
    {
        m_success = m_success && runNormal();
        m_success = m_success && runUpper();
        return m_success;
    }


    SQLDBC_Bool runNormal()
    {
        SQLDBC_Connection *c1=environment()->createConnection();

        SQLDBC_ConnectProperties p;
        p.setProperty("KEY", "SQLOPT");
        // DebugBreak();
        char buffer[200];
        if(hostname() && *hostname()) {
            sprintf(buffer, "SQLOPT=-n %s -d %s -u %s,%s", hostname(), database(), username(), password());
        } else {
            sprintf(buffer, "SQLOPT=-n localhost -d %s -u %s,%s", database(), username(), password());
        }
        putenv(buffer);
        // later we should use the properties method only
        tc_errorcheck(c1)->connect(p);

        SQLDBC_PreparedStatement *s1 = c1->createPreparedStatement();
        if(s1) {
          tc_errorcheck(s1)->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
          c1->releaseStatement(s1);
        }

        environment()->releaseConnection(c1);
        return m_success;
    }


    SQLDBC_Bool runUpper()
    {
        SQLDBC_Connection *c1=environment()->createConnection();

        SQLDBC_ConnectProperties p;
        p.setProperty("KEY", "SQLOPT");
        char buffer[200];

        char usernamebuf[200];
        char passwordbuf[200];

        memset(usernamebuf, 0, sizeof(usernamebuf));
        memset(passwordbuf, 0, sizeof(passwordbuf));

        strcpy(usernamebuf, username());
        strcpy(passwordbuf, password());

        for(int i= 0; i<200; ++i) {
            usernamebuf[i] = tolower(usernamebuf[i]);
            passwordbuf[i] = tolower(passwordbuf[i]);
        }

        if(hostname() && *hostname()) {
            sprintf(buffer, "SQLOPT=-n %s -d %s -u %s,%s", hostname(), database(), usernamebuf, passwordbuf);
        } else {
            sprintf(buffer, "SQLOPT=-n localhost -d %s -u %s,%s", database(), usernamebuf, passwordbuf);
        }

        putenv(buffer);
        // DebugBreak();
        // later we should use the properties method only
        tc_errorcheck(c1)->connect(p);

        SQLDBC_PreparedStatement *s1 = c1->createPreparedStatement();
        if(s1) {
          tc_errorcheck(s1)->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
          c1->releaseStatement(s1);
        }

        environment()->releaseConnection(c1);
        return m_success;
    }

};

class XUSERUnicodeTest
    : public SQLDBC_TestCase
{
public:
    XUSERUnicodeTest()
    :SQLDBC_TestCase("XUSERUnicodeTest")
    {}

    SQLDBC_Bool run()
    {

        SQLDBC_Connection *c1=environment()->createConnection();

        SQLDBC_ConnectProperties p;
        p.setProperty("KEY", "DEFAULT");
        p.setProperty("UNICODE", "1");
        tc_errorcheck(c1)->connect(p);
        SQLDBC_PreparedStatement *s1 = c1->createPreparedStatement();
        if(s1) {
          tc_errorcheck(s1)->prepare("SELECT * FROM DUAL ", SQLDBC_StringEncodingAscii);
          c1->releaseStatement(s1);
        }

        environment()->releaseConnection(c1);
        return m_success;
    }

};


class DisconnectFailure
    : public SQLDBC_TestCase
{
public:
    DisconnectFailure()
    :SQLDBC_TestCase("DisconnectFailure")
    {}

    SQLDBC_Bool run()
    {
        SQLDBC_Connection *c1=environment()->createConnection();
        SQLDBC_ConnectProperties p;
        tc_errorcheck(c1)->connect(m_frame->hostname(),
                                   SQLDBC_NTS,
                                   m_frame->database(),
                                   SQLDBC_NTS,
                                   m_frame->username(),
                                   SQLDBC_NTS,
                                   m_frame->password(),
                                   SQLDBC_NTS,
                                   SQLDBC_StringEncodingAscii,
                                   p);
        tc_errorcheck(c1)->disconnect();
        environment()->releaseConnection(c1);
        return success();
    }
};


class TwoConnect
    : public SQLDBC_TestCase
{
public:
    TwoConnect()
    :SQLDBC_TestCase("TwoConnect")
    {}

    SQLDBC_Bool run()
    {
        SQLDBC_Connection *c1=environment()->createConnection();
        SQLDBC_ConnectProperties p1;
        p1.setProperty("SQLMODE", "SAPR3");
        p1.setProperty("UNICODE", "1");
        p1.setProperty("ISOLATIONLEVEL", "0");
        p1.setProperty("TIMEOUT", "0");
        tc_errorcheck(c1)->connect("ld0276",
                                   SQLDBC_NTS,
                                   "LDB",
                                   SQLDBC_NTS,
                                   "SAPR3",
                                   SQLDBC_NTS,
                                   "SAP",
                                   SQLDBC_NTS,
                                   SQLDBC_StringEncodingAscii,
                                   p1);


        SQLDBC_Connection *c2=environment()->createConnection();
        SQLDBC_ConnectProperties p2;
        p2.setProperty("SQLMODE", "SAPR3");
        p2.setProperty("UNICODE", "1");
        p2.setProperty("ISOLATIONLEVEL", "0");
        p2.setProperty("TIMEOUT", "0");
        tc_errorcheck(c2)->connect("pwdf0679",
                                   SQLDBC_NTS,
                                   "LCF",
                                   SQLDBC_NTS,
                                   "SAPR3",
                                   SQLDBC_NTS,
                                   "SAP",
                                   SQLDBC_NTS,
                                   SQLDBC_StringEncodingAscii,
                                   p2);

        environment()->releaseConnection(c2);
        environment()->releaseConnection(c1);
        return success();
    }
};

class UnicodeNullTerminated
    : public SQLDBC_TestCase
{
public:
    UnicodeNullTerminated()
    :SQLDBC_TestCase("UnicodeNullTerminated")
    {}

    SQLDBC_Bool run()
    {
        if(connection()->isUnicodeDatabase()) {
            UCS2CHAR username[64];
            UCS2CHAR password[64];
            convertToUCS2(username, 64, m_frame->username());
            convertToUCS2(password, 64, m_frame->password());
            
            SQLDBC_Connection *c1=environment()->createConnection();
            SQLDBC_ConnectProperties p;
            p.setProperty("UNICODE", "1");
            tc_errorcheck(c1)->connect(m_frame->hostname(),
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       (char *)username,
                                       SQLDBC_NTS,
                                       (char *)password,
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingUCS2Native,
                                       p);
            environment()->releaseConnection(c1);
        } else {
            loginfo("Not executed (no unicode database)");
        }
        return m_success;
    }
};

class LossLessUTF8
    : public SQLDBC_TestCase
{
public:
    LossLessUTF8()
    :SQLDBC_TestCase("LOSSLESSUTF8")
    {}
    
    SQLDBC_Bool run()
    {
        SQLDBC_Statement *s = connection()->createStatement();
        s->execute("DROP USER \"DB\xC3\xA4\"", SQLDBC_NTS, SQLDBC_StringEncodingUTF8);
        tc_errorcheck(s)->execute("CREATE USER \"DB\xC3\xA4\" PASSWORD \"DB\xC3\xA4\" NOT EXCLUSIVE",
                                  SQLDBC_NTS,
                                  SQLDBC_StringEncodingUTF8);
        connection()->commit();
        
        SQLDBC_Connection *c1=environment()->createConnection();
        SQLDBC_ConnectProperties p;
        p.setProperty("UNICODE", "0");
        tc_errorcheck(c1)->connect(m_frame->hostname(),
                                   SQLDBC_NTS,
                                   m_frame->database(),
                                   SQLDBC_NTS,
                                   "DB\xC3\xA4",
                                   SQLDBC_NTS,
                                   "DB\xC3\xA4",
                                   SQLDBC_NTS,
                                   SQLDBC_StringEncodingUTF8,
                                   p);
        environment()->releaseConnection(c1);

        return m_success;
    }
};

class UnicodeAsciiConnect
    : public SQLDBC_TestCase
{
public:
    UnicodeAsciiConnect()
    :SQLDBC_TestCase("UNICODEASCIICONNECT")
    {}

    SQLDBC_Bool run()
    {
        if(!connection()->isUnicodeDatabase()) {
            UCS2CHAR username[64];
            UCS2CHAR password[64];
            convertToUCS2(username, 64, m_frame->username());
            convertToUCS2(password, 64, m_frame->password());
            SQLDBC_Connection *c1=environment()->createConnection();
            SQLDBC_ConnectProperties p;
            p.setProperty("UNICODE", "1");
            tc_errorcheck(c1)->connect(m_frame->hostname(),
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       (char *)username,
                                       SQLDBC_NTS,
                                       (char *)password,
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingUCS2Native,
                                       p);
        } else {
            loginfo("Not executed (unicode database, should work straight through.");
        }
        return m_success;
    }
    
};



int main(int argc, char **argv)
{
    SQLDBC_TestFrame testframe(argc, argv);
    //testframe.addTestCase(new TwoConnect());
    // only seen in trace, no sense for night test
    // testframe.addTestCase(new DisconnectFailure());
    // testframe.addTestCase(new LossLessUTF8());
#if 0
    testframe.addTestCase(new ConnectTest());
    testframe.addTestCase(new SQLOPTDynamicTest());
    testframe.addTestCase(new UnicodeNullTerminated());
#endif
    testframe.addTestCase(new UnicodeAsciiConnect());
     //testframe.addTestCase(new XUSERUnicodeTest());
    if(testframe.run()) return 0;
    return 1;
}
