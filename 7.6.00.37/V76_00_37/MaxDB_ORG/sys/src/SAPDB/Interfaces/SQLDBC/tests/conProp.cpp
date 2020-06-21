/*!
  @file			conProp.cpp
  @author		D042427
  @ingroup		Tests
  @brief		1. check valid connect properties 2. check invalid connect properties 3. check reconnect
  @keywords		connect, 
  @remarks		the following test cases only use the SQLDBC_Connection::connect(*servernode, *serverdb, *username, *password, &properties) method
  @bugs			bbb: too small - no pts message
  @todo			use xuser for the reconnect test case

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"

/* Some additional functions... */
//void setValidProperties

/*class setConProp : public SQLDBC_TestCase
{

public:
  setConProp ()
    : SQLDBC_TestCase ("setConProp")
  {}

  virtual SQLDBC_Bool run ()
  {
	loginfo("Creating connection properties object...");
	SQLDBC_ConnectProperties *cprop = new SQLDBC_ConnectProperties();

	loginfo("print default properties...");
	const char *dummy = cprop->getProperty("COMPNAME");
	loginfo("COMPNAME: %s", dummy);
	loginfo("APPLICATION: %s", cprop->getProperty("APPLICATION"));
	loginfo("UNICODE: %s", cprop->getProperty("UNICODE", "UNC"));

	cprop->setProperty("HOMER", "SIMPSON");
	loginfo("HOMER: %s", cprop->getProperty("HOMER"));

	return m_success;
  }
};*/

class conPropPositiveAndNegative : public SQLDBC_TestCase
{
private:
	SQLDBC_Bool checkConnect(const char *key, const char *value)
	{
		// create connection object
		SQLDBC_Environment env(runtime());
		SQLDBC_Connection *conn = env.createConnection();
		if (!conn) {
			logerror("createConnection does not return a valid object\n");
			return 0;
		}

		loginfo("Check connect with key='%s' value='%s'", key, value);
		SQLDBC_ConnectProperties *cprop = new SQLDBC_ConnectProperties();
		cprop->setProperty(key, value);

		SQLDBC_Retcode rc;
		tc_errorcheckrc(rc, conn)->connect(hostname(), database(), username(), password(), *cprop);
		conn->clearError();
		if (!tc_errorcheck(conn)->isConnected())
		{
			logerror("Session is not connected. Connection was expected");
		}
		else
		{
            tc_errorcheck(conn)->disconnect();
		}

		return 1;
	}
  
	SQLDBC_Bool checkFail(const char *key, const char *value)
	{
		// create connection object
		SQLDBC_Environment env(runtime());
		SQLDBC_Connection *conn = env.createConnection();
		if (!conn) {
			logerror("createConnection does not return a valid object\n");
			return 0;
		}

		loginfo("Check failure with key='%s' value='%s'", key, value);
		SQLDBC_ConnectProperties *cprop = new SQLDBC_ConnectProperties();
		cprop->setProperty(key, value);

		SQLDBC_Retcode rc;
		rc = conn->connect(hostname(), database(), username(), password(), *cprop);
		if (rc == SQLDBC_OK)
		{
			logerror("Connect succeeded. Expected was failure");
			tc_errorcheck(conn)->disconnect();
		}

		return 1;
	}
  
public:
  conPropPositiveAndNegative ()
    : SQLDBC_TestCase ("PositiveAndNegative")
  {}

  virtual SQLDBC_Bool run ()
  {
	loginfo("Property: COMPNAME...");
	// how does this COMPNAME work...?

	// if no connection object can created abort test case
	if (!checkConnect("COMPNAME", "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCD")) return 0;
	// Check more than 64 characters: why does it succeed?
//	checkConnect("COMPNAME", "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDE");
//	checkConnect("COMPNAME", "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDE");

	loginfo("Property: APPLICATION...");
	checkConnect("APPLICATION", "CAL");
	// not all already implemented application abbreviations have to be tested:
	// the db kernel also accepts unknown abbreviations
	checkConnect("APPLICATION", "new");
	checkFail("APPLICATION", "ZYXW");
	checkFail("APPLICATION", "");
	checkFail("APPLICATION", "A");
	checkFail("APPLICATION", "BC");

	loginfo("Property: APPVERSION...");
	checkConnect("APPVERSION", "10000");
	checkConnect("APPVERSION", "04027");
	checkConnect("APPVERSION", "70700");
	//checkConnect("APPVERSION", "99999"); // This version number is too high (the kernel does not accept this)
	checkFail("APPVERSION", "123456");
	checkFail("APPVERSION", "abced");
	checkFail("APPVERSION", "10xxx");
	checkFail("APPVERSION", "");
	checkFail("APPVERSION", "4321");

	loginfo("Property: SQLMODE...");
	checkConnect("SQLMODE", "INTERNAL");
	checkConnect("SQLMODE", "oRaClE");
	checkConnect("SQLMODE", "ANSI");
	checkConnect("SQLMODE", "DB2");
	checkFail("SQLMODE", "ANYOTHER");
	checkFail("SQLMODE", "");

	loginfo("Property: UNICODE...");
	//checkConnect("UNICODE", "tRue");
	checkConnect("UNICODE", "fAlSe");
	//checkConnect("UNICODE", "1");
	checkConnect("UNICODE", "0");
	//checkFail("UNICODE", ""); // interpreted as false <- not a bug
	//checkFail("UNICODE", "-1"); // interpreted as false <- not a bug
	//checkFail("UNICODE", "2");  // interpreted as false <- not a bug
	//checkFail("UNICODE", "test"); // interpreted as false <- not a bug
	//checkFail("UNICODE", "ANYOTHER"); // interpreted as false <- not a bug

	loginfo("Property: TIMEOUT...");
	checkConnect("TIMEOUT", "0");
	checkConnect("TIMEOUT", "1");
	checkConnect("TIMEOUT", "100");
	//checkConnect("TIMEOUT", "1000000000000000000000000"); // Too big - kernel does not accept it
	checkFail("TIMEOUT", "-1");
	checkFail("TIMEOUT", "");
	checkFail("TIMEOUT", "a");
	checkFail("TIMEOUT", "34h3");

	loginfo("Property: ISOLATIONLEVEL...");
	checkConnect("ISOLATIONLEVEL", "0");
	checkConnect("ISOLATIONLEVEL", "1");
	checkConnect("ISOLATIONLEVEL", "10");
	checkConnect("ISOLATIONLEVEL", "15");
	checkConnect("ISOLATIONLEVEL", "2");
	checkConnect("ISOLATIONLEVEL", "20");
	checkConnect("ISOLATIONLEVEL", "3");
	checkConnect("ISOLATIONLEVEL", "30");
	checkFail("ISOLATIONLEVEL", "-1");
	checkFail("ISOLATIONLEVEL", "11");
	checkFail("ISOLATIONLEVEL", "19");
	checkFail("ISOLATIONLEVEL", "25");
	checkFail("ISOLATIONLEVEL", "4");

	loginfo("Property: PACKETCOUNT...");
	checkConnect("PACKETCOUNT", "UnLiMiTeD");
	checkConnect("PACKETCOUNT", "1");
	checkConnect("PACKETCOUNT", "2");
	checkFail("PACKETCOUNT", "0");
	checkFail("PACKETCOUNT", "-1");
	checkFail("PACKETCOUNT", "-10");
	checkFail("PACKETCOUNT", "ANYOTHER");

	loginfo("Property: STATEMENTCACHESIZE...");
	checkConnect("STATEMENTCACHESIZE", "UnLiMiTeD");
	checkConnect("STATEMENTCACHESIZE", "0");
	checkConnect("STATEMENTCACHESIZE", "10");
	checkFail("STATEMENTCACHESIZE", "-1");
	checkFail("STATEMENTCACHESIZE", "ANYOTHER");
//	checkFail("STATEMENTCACHESIZE", ""); // bug bbb

	return m_success;
  }
};

class conPropReconnect : public SQLDBC_TestCase
{
public:
  conPropReconnect ()
    : SQLDBC_TestCase ("Reconnect")
  {}

  virtual SQLDBC_Bool run ()
  {
	loginfo("Check reconnect with different connect properties...");
	SQLDBC_Retcode rc;

	SQLDBC_Environment env(runtime());

	/*   ORACLE MODE -> INTERNAL MODE   */
	SQLDBC_Connection *conn1 = env.createConnection();
	if (!conn1) {
		logerror("createConnection does not return a valid object\n");
		return 0;
	}
	loginfo("Connect in oracle mode");
	SQLDBC_ConnectProperties *cprop1 = new SQLDBC_ConnectProperties();
	cprop1->setProperty("SQLMODE", "ORACLE");
	tc_errorcheck(conn1)->connect(hostname(), database(), username(), password(), *cprop1);

	loginfo("Disconnect");
	tc_errorcheck(conn1)->disconnect();

	loginfo("Connect in internal mode");
	SQLDBC_ConnectProperties *cprop2 = new SQLDBC_ConnectProperties();
	cprop2->setProperty("SQLMODE", "INTERNAL");
	tc_errorcheck(conn1)->connect(hostname(), database(), username(), password(), *cprop2);

	loginfo("Are we still in oracle mode?");
	SQLDBC_Statement *stmt1 = conn1->createStatement();
	if (!stmt1) {
		logerror("Statement was not created. Test must abort");
		conn1->disconnect();
		return 0;
	}
	dropTable("testtable123");
	loginfo("create table and try to insert");
	tc_errorcheckrc(rc, stmt1)->execute("CREATE TABLE testtable123 (dummy integer)");
	rc = stmt1->execute("INSERT testtable123 VALUES (42)");
	if (rc != SQLDBC_OK)
		logerror("We still have oracle sqlmode. Should be internal");
	else
		loginfo("Insertion successful: SQL mode was changed as expected");
	loginfo("drop table and disconnect");
	tc_errorcheckrc(rc, stmt1)->execute("DROP TABLE testtable123");
	conn1->disconnect();

	/*   transaction isolation  20 -> 30   */
	SQLDBC_Connection *conn2 = env.createConnection();
	if (!conn2) {
		logerror("createConnection does not return a valid object\n");
		return 0;
	}
	loginfo("Connect with isolation level 20");
	SQLDBC_ConnectProperties *cprop3 = new SQLDBC_ConnectProperties();
	cprop3->setProperty("ISOLATIONLEVEL", "20");
	tc_errorcheck(conn2)->connect(hostname(), database(), username(), password(), *cprop3);

	SQLDBC_Int4 il1 = tc_errorcheck(conn2)->getTransactionIsolation();
	if (il1 != 20)
		logerror("The wrong isolation level was set: %d. Expected was 20", il1);

	loginfo("Disconnect");
	tc_errorcheck(conn2)->disconnect();

	loginfo("Connect with isolation level 30");
	SQLDBC_ConnectProperties *cprop4 = new SQLDBC_ConnectProperties();
	cprop4->setProperty("ISOLATIONLEVEL", "30");
	tc_errorcheck(conn2)->connect(hostname(), database(), username(), password(), *cprop4);

	SQLDBC_Int4 il2 = tc_errorcheck(conn2)->getTransactionIsolation();
	if (il2 != 30)
		logerror("The wrong isolation level was set: %d. Expected was 30", il1);
	else
		loginfo("The right level was set");

	loginfo("Disconnect");
	tc_errorcheck(conn2)->disconnect();

	/*   using predefined user -> local properties   */
	// Here we would need xuser...

	return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new conPropPositiveAndNegative());
  testframe.addTestCase (new conPropReconnect());
  if (testframe.run ()) 
    return 0;

  return 1;
}


