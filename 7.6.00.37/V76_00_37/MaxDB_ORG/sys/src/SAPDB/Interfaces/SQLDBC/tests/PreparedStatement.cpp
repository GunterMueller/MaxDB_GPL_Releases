/*!
  @file           PreparedStatement.cpp
  @author         D042427
  @ingroup        Tests
  @brief          Checks prepared statements: simple positive, reprepare
  @keywords       template
  @bugs           bbb: "Cannot convert UCS2 data to ASCII for parameter/column (2)"


	SimplePreparedStatement:
		- simple positive tests

  Disconnect:
    - Execute and prepare after having disconnected

  Attributes:
    - check if prepare takes the right attributes and if it leaves them untouched

	Unicode:
		- prepare() using unicode statement

  isQuery:
    - does isQuery always return the right value (before prepare, after INSERT, after SELECT)?

  ParameterMetaData:
    - checks parameter meta data for INSERT and SELECT statements

  ResultSetMetaData:
    - checks result set meta data (before prepare, after SELECT, after INSERT)



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
#include <string.h>

/* How this test case works:
 *
 * 1. prepare SELECT string, execute
 * 2. get result set, check if returned value is the string we inserted earlier
 *
 */
class SimplePreparedStatement : public SQLDBC_TestCase
{

public:
  SimplePreparedStatement ()
    : SQLDBC_TestCase ("SimplePreparedStatement")
  {}

  virtual SQLDBC_Bool run ()
  {
		
    SQLDBC_Retcode rc;
    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

    // 1.
    const char *szSQL = "SELECT 'TEST' FROM Dual";
	tc_errorcheckrc(rc, stmt)->prepare(szSQL);
    tc_errorcheckrc(rc, stmt)->execute();

    // 2.
	SQLDBC_ResultSet *res = stmt->getResultSet();
	if (!res)
		logerror("No resultset returned");

    tc_errorcheck(res)->next();

    char cdata[50];
    SQLDBC_Length ind=SQLDBC_NTS;
    tc_errorcheckrc(rc, res)->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata));
    
    if (strcmp(cdata, "TEST")) {
		logerror("Wrong data returned");
    }

	//finish your program with a returncode 
	return m_success;
  }
};

/* How this test case works:
 *
 * 1. get new connection
 * 2. create table with one column
 * 3. insert one row
 *
 * 4. prepare INSERT
 * 5. disconnect
 * 6. execute -> should fail
 * 7. prepare INSERT -> should fail
 * 8. connect
 * 9. prepare SELECT
 *
 */
class Disconnect : public SQLDBC_TestCase
{
protected:
  SQLDBC_Connection *conn;

public:
	Disconnect ()
		: SQLDBC_TestCase ("Disconnect")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
		loginfo("*     Prepare vs. disconnect...                     *");
		loginfo("*****************************************************");

		dropTable("tableDisconnect");

		// 1. get new connection
		conn = environment()->createConnection();
		tc_errorcheck(conn)->connect(hostname(), database(), username(), password());

		// 2.
		loginfo("create table...");
    SQLDBC_Statement *stmt = conn->createStatement();
    tc_errorcheck(stmt)->execute("CREATE TABLE tableDisconnect (i INTEGER)");

		// 3.
		loginfo("insert one row");
		tc_errorcheck(stmt)->execute("INSERT INTO tableDisconnect (i) VALUES (4)");

    tc_errorcheck(conn)->releaseStatement(stmt);

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
    SQLDBC_Statement *stmt = conn->createStatement();
    tc_errorcheck(stmt)->execute("DROP TABLE tableDisconnect");
    tc_errorcheck(conn)->releaseStatement(stmt);
    tc_errorcheck(conn)->commitWorkRelease();
    environment()->releaseConnection(conn);

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{

		SQLDBC_PreparedStatement *pstmt = conn->createPreparedStatement();

		// 4. 
		loginfo("prepare INSERT");
		tc_errorcheck(pstmt)->prepare("INSERT INTO tableDisconnect (i) VALUES (6)");
        
		// 5. 
		loginfo("commit and disconnect");
		tc_errorcheck(conn)->commitWorkRelease();
		if (conn->isConnected())
		  logerror("Connection still alive although we disconnected...");
        
		// 6. 
		loginfo("execute -> should fail");
		tc_expectederrorcheck(pstmt, -10821)->execute();
        
    // 7. 
    loginfo("prepare INSERT -> should fail");
    tc_expectederrorcheck(pstmt, -10821)->prepare("INSERT INTO tableDisconnect (i) VALUES (10)");

    // 8. 
    loginfo("connect");
    tc_errorcheck(conn)->connect(hostname(), database(), username(), password());
    if (!conn->isConnected())
      logerror("Connect failed...");

    // 9. 
    loginfo("prepare SELECT");
    tc_errorcheck(pstmt)->prepare("SELECT * FROM tableDisconnect");

		return m_success;
	}
};



/* How this test case works:
 *
 * 1. create table with two columns (...)
 * 2. insert one row
 *
 * 3. prepare SELECT
 * 4. prepare SELECT
 * 5. execute and check if the second prepare is taken
 * 6. set attributes (cursor name, batch size, binding type, fetch size, max rows, result set type, concurrency type)
 * 7. prepare INSERT
 * 8. check if attributes have changed (they should all be untouched)
 *
 */
class Attributes : public SQLDBC_TestCase
{

public:
	Attributes ()
		: SQLDBC_TestCase ("Attributes")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
		loginfo("*     does prepare touch any attributes...          *");
		loginfo("*****************************************************");

		dropTable("tableAttributes");

		// 1.
		loginfo("create table...");
		execSQL("CREATE TABLE tableAttributes (i INTEGER, c VARCHAR(10))");

		// 2.
		loginfo("insert data...");
		execSQL("INSERT INTO tableReprepare (i, c) VALUES (1, 'test')");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableAttributes");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();

		// 3.
    loginfo("prepare SELECT");
    tc_errorcheck(pstmt)->prepare("SELECT * FROM tableAttributes WHERE i = ?");

    // 4.
    loginfo("prepare SELECT");
    tc_errorcheck(pstmt)->prepare("SELECT i FROM tableAttributes WHERE i = 1");

    // 5.
    loginfo("execute and check if the second prepare is taken");
    tc_errorcheck(pstmt)->execute();
    SQLDBC_ResultSet *rs = tc_errorcheck(pstmt)->getResultSet();
    if (!rs) {
      logerror("No result set was returned");
      return m_success;
    }
    int res = 99;
    tc_errorcheck(rs)->next();
    tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &res, NULL, 0);
    if (res != 1)
      logerror("Wrong value was returned: %d. Expected was %d", res, 1);

    // 6.
    loginfo("set attributes (cursor name, batch size, binding type, fetch size, max rows, result set type, concurrency type)");
    //...

    // 7.
    loginfo("prepare INSERT");
    tc_errorcheck(pstmt)->prepare("INSERT INTO tableAttributes (i) VALUES (66)");

    // 8.
    loginfo("check if attributes have changed (they should all be untouched)");
    //...

		return m_success;
	}
};

/* How this test case works:
 *
 * 1. create table with three columns
 * 2. insert one row
 *
 * 3. prepare INSERT (unicode string)
 * 4. bind params, execute, SELECT, check result set
 * 5. prepare SELECT (unicode string)
 * 6. bind params, execute, check result set
 *
 */
class Unicode : public SQLDBC_TestCase
{

public:
	Unicode ()
		: SQLDBC_TestCase ("Unicode")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("*     prepare with Unicode string                   *");
		loginfo("*****************************************************");

    if (!connection()->isUnicodeDatabase()) {
      loginfo("We do not have a unicode database...");
      loginfo("We can't perform these tests...");
      return m_success;
    }

		dropTable("tableUnicode");

		// 1.
		loginfo("create table...");
		execSQL("CREATE TABLE tableUnicode (i INTEGER, c VARCHAR(30), i2 INTEGER)");

		// 2.
		loginfo("insert one row...");
		execSQL("INSERT INTO tableUnicode (i, c, i2) VALUES (8, 'test', 7)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableUnicode");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
    if (!connection()->isUnicodeDatabase())
      return m_success;

		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();

		// 3.
		loginfo("prepare INSERT (unicode string)");
    char asciiSQL[100];
    sprintf(asciiSQL, "INSERT INTO tableUnicode (i, c, i2) VALUES (?, ?, ?)");
    UCS2CHAR unicodeSQL[100];
    convertToUCS2(unicodeSQL, sizeof(unicodeSQL), false, asciiSQL);
    tc_errorcheck(pstmt)->prepare((char *)unicodeSQL, SQLDBC_StringEncodingUCS2);

    // 4.
		loginfo("bind params (3), execute, SELECT, check result set");
    int i = 12, i2 = 13;
    char c[30];
    sprintf(c, "test string");
    SQLDBC_Length ind=SQLDBC_NTS;

    tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &i, NULL, 0);
    tc_errorcheck(pstmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, c, &ind, sizeof(c));
    tc_errorcheck(pstmt)->bindParameter(3, SQLDBC_HOSTTYPE_INT4, &i2, NULL, 0);

    tc_errorcheck(pstmt)->execute();

    SQLDBC_Statement *stmt = connection()->createStatement();
    tc_errorcheck(stmt)->execute("SELECT i, c, i2 FROM tableUnicode WHERE i = 12");
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    if (!rs) {
      logerror("No result set returned.");
      return m_success;
    }
    tc_errorcheck(rs)->next();

    // initialize buffers
    int resi = 43, resi2 = 44;
    char resc[40];
    memset(resc, '\0', sizeof(resc));
    SQLDBC_Length resind=SQLDBC_NTS;

    // fetch result
    tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &resi, NULL, 0);
    tc_errorcheck(rs)->getObject(2, SQLDBC_HOSTTYPE_ASCII, resc, &resind, sizeof(resc));
    tc_errorcheck(rs)->getObject(3, SQLDBC_HOSTTYPE_INT4, &resi2, NULL, 0);

    // check result
    if ((i != resi) || (i2 != resi2) || (strcmp(c, resc))) 
      logerror("Wrong data was inserted: %d, '%s', %d. Expcted was %d, '%s', %d", resi, resc, resi2, i, c, i2);

    // 5.
		loginfo("prepare SELECT (unicode string)");
    sprintf(asciiSQL, "SELECT i, c, i2 FROM tableUnicode WHERE i = ? AND i2 = ?");
    convertToUCS2(unicodeSQL, sizeof(unicodeSQL), false, asciiSQL);
    tc_errorcheck(pstmt)->prepare((char *)unicodeSQL, SQLDBC_StringEncodingUCS2);

    // 6.
		loginfo("bind params (2), execute, check result set");
    tc_errorcheck(pstmt)->clearParameters();
    tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &i, NULL, 0);
    tc_errorcheck(pstmt)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, &i2, NULL, 0);

    tc_errorcheck(pstmt)->execute();

    rs = tc_errorcheck(pstmt)->getResultSet();
    if (!rs) {
      logerror("No result set returned.");
      return m_success;
    }
    tc_errorcheck(rs)->next();

    // reinitialize buffers
    resi = 43;
    resi2 = 44;
    memset(resc, '\0', sizeof(resc));
    resind=SQLDBC_NTS;

    // fetch result
    tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &resi, NULL, 0);
//    tc_errorcheck(rs)->getObject(2, SQLDBC_HOSTTYPE_ASCII, resc, &resind, sizeof(resc));
    tc_errorcheck(rs)->getObject(2, SQLDBC_HOSTTYPE_UCS2, resc, &resind, sizeof(resc));
    // bbb
    tc_errorcheck(rs)->getObject(3, SQLDBC_HOSTTYPE_INT4, &resi2, NULL, 0);

    char temp[40];
    convertUCS2ToASCII(temp, sizeof(temp), false, (UCS2CHAR *)resc, sizeof(resc));

    // check result
//    if ((i != resi) || (i2 != resi2) || (strcmp(c, resc))) 
    if ((i != resi) || (i2 != resi2) || (strcmp(c, temp))) 
      logerror("Wrong data was inserted: %d, '%s', %d. Expcted was %d, '%s', %d", resi, temp, resi2, i, c, i2);

		return m_success;
	}
};



/* How this test case works:
 *
 * 1. create table with three columns
 * 2. insert one row
 *
 * 3. check isQurey() -> sould return false
 * 4. prepare SELECT, execute, check isQuery() -> should return true
 * 5. check result set
 * 6. prepare INSERT
 * 7. check isQuery() -> call should not be allowed
 * 8. execute
 * 9. check isQuery() -> should return false
 * 10. check result set
 *
 */
class isQuery : public SQLDBC_TestCase
{

public:
	isQuery ()
		: SQLDBC_TestCase ("isQuery")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
		loginfo("*     does isQuery work properly?                   *");
		loginfo("*****************************************************");

		dropTable("tableIsQuery");

		// 1.
		loginfo("create table...");
		execSQL("CREATE TABLE tableIsQuery (i INTEGER)");

		// 2.
		loginfo("insert data...");
		execSQL("INSERT INTO tableIsQuery (i) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableIsQuery");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();

    // 3. 
    loginfo("check isQurey() -> sould return false");
    if (tc_errorcheck(pstmt)->isQuery())
      logerror("isQuery() returned true. Expected was false");

    // 4. 
    loginfo("prepare SELECT, execute, check isQuery() -> should return true");
    tc_errorcheck(pstmt)->prepare("SELECT i FROM tableIsQuery");
    tc_errorcheck(pstmt)->execute();
    if (!tc_errorcheck(pstmt)->isQuery())
      logerror("isQuery() returned false. Expected was true");

    // 5. 
    loginfo("check result set");
    SQLDBC_ResultSet *rs = tc_errorcheck(pstmt)->getResultSet();
    if (!rs) {
      logerror("No result set was returned.");
      return m_success;
    }
    tc_errorcheck(rs)->next();
    int resi=99;
    tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &resi, NULL, 0);
    if (resi != 1)
      logerror("Wrong data was returned: %d. Expected was %d", resi, 1);
    
    // 6. 
    loginfo("prepare INSERT");
    tc_errorcheck(pstmt)->prepare("INSERT INTO tableIsQuery (i) VALUES (2)");
    
    // 7. 
    loginfo("check isQuery() -> should return false");
    if (tc_errorcheck(pstmt)->isQuery())
      logerror("isQuery() returned true. Expected was false");
    
    // 8. 
    loginfo("execute");
    tc_errorcheck(pstmt)->execute();

    // 9. 
    loginfo("check isQuery() -> should return false");
    if (tc_errorcheck(pstmt)->isQuery())
      logerror("isQuery() returned true. Expected was false");

    // 10.
    loginfo("check result set");
    // perform query
    tc_errorcheck(pstmt)->prepare("SELECT i FROM tableIsQuery WHERE i = 2");
    tc_errorcheck(pstmt)->execute();
    rs = tc_errorcheck(pstmt)->getResultSet();
    if (!rs) {
      logerror("No result set was returned.");
      return m_success;
    }
    tc_errorcheck(rs)->next();
    resi=99;
    tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &resi, NULL, 0);
    if (resi != 2)
      logerror("Wrong data was returned: %d. Expected was %d", resi, 2);
    
    connection()->releaseStatement(pstmt);

		return m_success;
	}
};


/* How this test case works:
 *
 * 1. create table with three columns
 * 2. insert one row
 *
 * 3. prepare SELECT
 * 4. getParameterMetaData
 * 4a. check parameter count
 * 4b. check parameter types
 * 4c. check parameter modes
 * 4d. check parameter lengths
 * 4e. check if parameters are nullable
 * 4f. check parameter names
 * 5. prepare INSERT
 * 6. getParameterMetaData
 * 6a. check parameter count
 * 6b. check parameter types
 * 6c. check parameter modes
 * 6d. check parameter lengths
 * 6e. check if parameters are nullable
 * 6f. check parameter names
 *
 */
#define NUMP 13
#define NUMP2 7

class ParameterMetaData : public SQLDBC_TestCase
{

public:
	ParameterMetaData ()
		: SQLDBC_TestCase ("ParameterMetaData")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
		loginfo("*     is meta data properly set?                    *");
		loginfo("*****************************************************");

		dropTable("tableParameterMetaData");

		// 1.
		loginfo("create table...");
		execSQL("CREATE TABLE tableParameterMetaData (\
				vfixed1 FIXED(5), \
				vfixed2 FIXED(8), \
				vfloat FLOAT, \
				vcharascii CHAR(10) ASCII NOT NULL, \
				vcharbyte CHAR(5) BYTE, \
				vdate DATE, \
				vtime TIME, \
				vtimestamp TIMESTAMP, \
				vboolean BOOLEAN, \
				vsmallint SMALLINT, \
				vinteger INTEGER NOT NULL, \
				vvarcharascii VARCHAR(20) ASCII, \
				vvarcharbyte VARCHAR(7) BYTE)");

		// 2.
		loginfo("insert data...");
		execSQL("INSERT INTO tableParameterMetaData (vcharascii, vinteger) VALUES ('test test', 1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableParameterMetaData");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();
    int i;

    // 3. 
    loginfo("prepare SELECT");
    tc_errorcheck(pstmt)->prepare("SELECT vinteger FROM tableParameterMetaData WHERE \
				vfixed1 = ? AND \
				vfixed2 = ? AND \
				vfloat = ? AND \
				vcharascii = ? AND \
				vcharbyte = ? AND \
				vdate = ? AND \
				vtime = ? AND \
				vtimestamp = ? AND \
				vboolean = ? AND \
				vsmallint = ? AND \
				vinteger = ? AND \
				vvarcharascii = ? AND \
				vvarcharbyte = ?");
    
    // 4. 
    loginfo("getParameterMetaData");
    SQLDBC_ParameterMetaData *md = tc_errorcheck(pstmt)->getParameterMetaData();
    if (!md) {
      logerror("No meta data was returned.");
      return m_success;
    }
    
    // 4a. 
    loginfo("check parameter count");
    SQLDBC_Int2 count = md->getParameterCount();
    if (count != NUMP)
      logerror("getParameterCount() returned %d. Expected was %d", count, 15);

    if (count == NUMP) {
    // 4b. 
    loginfo("check parameter types");
    SQLDBC_SQLType expectedType[NUMP] = {
        SQLDBC_SQLTYPE_FIXED,
        SQLDBC_SQLTYPE_FIXED,
        SQLDBC_SQLTYPE_FLOAT,
        SQLDBC_SQLTYPE_CHA,
        SQLDBC_SQLTYPE_CHB,
        SQLDBC_SQLTYPE_DATE,
        SQLDBC_SQLTYPE_TIME,
        SQLDBC_SQLTYPE_TIMESTAMP,
        SQLDBC_SQLTYPE_BOOLEAN,
        SQLDBC_SQLTYPE_SMALLINT,
        SQLDBC_SQLTYPE_INTEGER,
        SQLDBC_SQLTYPE_VARCHARA,
        SQLDBC_SQLTYPE_VARCHARB};
    for (i=0; i<NUMP; i++) {
      SQLDBC_SQLType type = md->getParameterType(i+1);
      if (type != expectedType[i])
        logerror("getParameterType returned %d for column %d. Expected was %d.", type, i+1, expectedType[i]);
    }
    
    // 4c. 
    loginfo("check parameter modes");
    for (i=0; i<NUMP; i++) {
      SQLDBC_ParameterMetaData::ParameterMode mode = md->getParameterMode(i+1);
      if (mode != SQLDBC_ParameterMetaData::parameterModeIn)
        logerror("getParameterMode returned %d for column %d. Expected was %d.", mode, i+1, SQLDBC_ParameterMetaData::parameterModeIn);
    }
    
    // 4d. 
    loginfo("check parameter lengths");
    SQLDBC_Int4 expectedLength[NUMP] = {5, 8, 16, 10, 5, 10, 8, 26, 1, 5, 10, 20, 7};
    for (i=0; i<NUMP; i++) {
      SQLDBC_Int4 length = md->getParameterLength(i+1);
      if (length != expectedLength[i])
        logerror("getParameterLength returned %d for column %d. Expected was %d.", length, i+1, expectedLength[i]);
    }
    
    // 4e. 
    loginfo("check if parameters are nullable");
    loginfo("Cannot perform this test case: kernel says 'optional' for all columns...");
    
    // 4f. 
    loginfo("check parameter names");
    loginfo("Cannot perform this test case: kernel does not return the names...");
    char expectedName[NUMP][100] = {
				"vfixed1",
				"vfixed2",
				"vfloat",
				"vcharascii",
				"vcharbyte",
				"vdate",
				"vtime",
				"vtimestamp",
				"vboolean",
				"vsmallint",
				"vinteger",
				"vvarcharascii",
        "vvarcharbyte"};
/*    for (i=0; i<NUMP; i++) {
      char name[100];
      memset(name, '\0', sizeof(name));
      SQLDBC_Length namesize = 0;
      md->getParameterName(i+1, name, SQLDBC_StringEncodingAscii, sizeof(name), &namesize);
      name[namesize] = '\0';
      if (strcmp(name, expectedName[i]))
        logerror("getParameterName returned %s for column %d. Expected was %s.", name, i+1, expectedName[i]);
    }*/
    }


    // 5. 
    loginfo("prepare INSERT");
    tc_errorcheck(pstmt)->prepare("INSERT INTO tableParameterMetaData\
      (vboolean, vinteger, vfloat, vfixed1, vvarcharbyte, vtime, vcharascii) VALUES (?, ?, ?, ?, ?, ?, ?)");
    
    // 6. 
    loginfo("getParameterMetaData");
    md = tc_errorcheck(pstmt)->getParameterMetaData();
    if (!md) {
      logerror("No meta data was returned.");
      return m_success;
    }
    
    // 6a. 
    loginfo("check parameter count");
    count = md->getParameterCount();
    if (count != NUMP2)
      logerror("getParameterCount() returned %d. Expected was %d", count, 15);

    if (count == NUMP2) {
    // 6b. 
    loginfo("check parameter types");
    SQLDBC_SQLType expectedType[NUMP2] = {
        SQLDBC_SQLTYPE_BOOLEAN,
        SQLDBC_SQLTYPE_INTEGER,
        SQLDBC_SQLTYPE_FLOAT,
        SQLDBC_SQLTYPE_FIXED,
        SQLDBC_SQLTYPE_VARCHARB,
        SQLDBC_SQLTYPE_TIME,
        SQLDBC_SQLTYPE_CHA};
    for (i=0; i<NUMP2; i++) {
      SQLDBC_SQLType type = md->getParameterType(i+1);
      if (type != expectedType[i])
        logerror("getParameterType returned %d for column %d. Expected was %d.", type, i+1, expectedType[i]);
    }
    
    // 6c. 
    loginfo("check parameter modes");
    for (i=0; i<NUMP2; i++) {
      SQLDBC_ParameterMetaData::ParameterMode mode = md->getParameterMode(i+1);
      if (mode != SQLDBC_ParameterMetaData::parameterModeIn)
        logerror("getParameterMode returned %d for column %d. Expected was %d.", mode, i+1, SQLDBC_ParameterMetaData::parameterModeIn);
    }
    
    // 6e. 
    loginfo("check parameter lengths");
    SQLDBC_Int4 expectedLength[NUMP2] = {1, 10, 16, 5, 7, 8, 10};
    for (i=0; i<NUMP2; i++) {
      SQLDBC_Int4 length = md->getParameterLength(i+1);
      if (length != expectedLength[i])
        logerror("getParameterLength returned %d for column %d. Expected was %d.", length, i+1, expectedLength[i]);
    }
    
    // 6f. 
    loginfo("check if parameters are nullable");
    SQLDBC_ParameterMetaData::ParameterNullBehavior expectedBehaviour[NUMP2] = {
        SQLDBC_ParameterMetaData::parameterNullable,
        SQLDBC_ParameterMetaData::parameterNoNulls,
        SQLDBC_ParameterMetaData::parameterNullable,
        SQLDBC_ParameterMetaData::parameterNullable,
        SQLDBC_ParameterMetaData::parameterNullable,
        SQLDBC_ParameterMetaData::parameterNullable,
        SQLDBC_ParameterMetaData::parameterNoNulls};
    for (i=0; i<NUMP2; i++) {
      SQLDBC_ParameterMetaData::ParameterNullBehavior behaviour = md->isNullable (i+1);
      if (behaviour != expectedBehaviour[i] && behaviour != SQLDBC_ParameterMetaData::parameterNullableUnknown)
        logerror("isNullable returned %d for column %d. Expected was %d.", behaviour, i+1, expectedBehaviour[i]);
    }
    }
    
    connection()->releaseStatement(pstmt);

		return m_success;
	}
};


/* How this test case works:
 *
 * 1. create table with three columns
 * 2. insert one row
 *
 * 3. getResultSetMetaData -> should fail
 * 4. prepare SELECT statement
 * 5. getResultSetMetaData
 * 5a. check column count
 * 5b. check column types
 * 5c. check column lengths
 * 5d. check if columns are nullable
 * 5e. check column names
 * 6. prepare INSERT statement
 * 7. getResultSetMetaData -> should fail
 *
 */

class ResultSetMetaData : public SQLDBC_TestCase
{

public:
	ResultSetMetaData ()
		: SQLDBC_TestCase ("ResultSetMetaData")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
		loginfo("*     is result set meta data properly set?         *");
		loginfo("*****************************************************");

		dropTable("tableResultSetMetaData");

		// 1.
		loginfo("create table...");
		execSQL("CREATE TABLE tableResultSetMetaData (\
				vfixed1 FIXED(5), \
				vfixed2 FIXED(8), \
				vfloat FLOAT, \
				vcharascii CHAR(10) ASCII NOT NULL, \
				vcharbyte CHAR(5) BYTE, \
				vdate DATE, \
				vtime TIME, \
				vtimestamp TIMESTAMP, \
				vboolean BOOLEAN, \
				vsmallint SMALLINT, \
				vinteger INTEGER NOT NULL, \
				vvarcharascii VARCHAR(20) ASCII, \
				vvarcharbyte VARCHAR(7) BYTE)");

		// 2.
		loginfo("insert data...");
		execSQL("INSERT INTO tableResultSetMetaData (vcharascii, vinteger) VALUES ('test test', 1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableResultSetMetaData");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();
    int i;

    // 3.
    loginfo("getResultSetMetaData -> should fail");
    SQLDBC_ResultSetMetaData *md;
    md = tc_expectederrorcheck(pstmt, -10210)->getResultSetMetaData();

    // 4.
    loginfo("prepare SELECT");
    tc_errorcheck(pstmt)->prepare("SELECT \
				vfixed1, vfixed2, vfloat, \
				vcharascii, vcharbyte, vdate, \
				vtime, vtimestamp, vboolean, \
				vsmallint, vinteger, vvarcharascii, vvarcharbyte \
        FROM tableResultSetMetaData");
    
    // 5. 
    loginfo("getResultSetMetaData");
    md = tc_errorcheck(pstmt)->getResultSetMetaData();
    if (!md) {
      logerror("No result set meta data was returned.");
      return m_success;
    }
    
    // 5a. 
    loginfo("check column count");
    SQLDBC_Int2 count = md->getColumnCount();
    if (count != NUMP)
      logerror("getParameterCount() returned %d. Expected was %d", count, 15);

    if (count == NUMP) {
    // 5b. 
    loginfo("check column types");
    SQLDBC_SQLType expectedType[NUMP] = {
        SQLDBC_SQLTYPE_FIXED,
        SQLDBC_SQLTYPE_FIXED,
        SQLDBC_SQLTYPE_FLOAT,
        SQLDBC_SQLTYPE_CHA,
        SQLDBC_SQLTYPE_CHB,
        SQLDBC_SQLTYPE_DATE,
        SQLDBC_SQLTYPE_TIME,
        SQLDBC_SQLTYPE_TIMESTAMP,
        SQLDBC_SQLTYPE_BOOLEAN,
        SQLDBC_SQLTYPE_SMALLINT,
        SQLDBC_SQLTYPE_INTEGER,
        SQLDBC_SQLTYPE_VARCHARA,
        SQLDBC_SQLTYPE_VARCHARB};
    for (i=0; i<NUMP; i++) {
      SQLDBC_SQLType type = md->getColumnType(i+1);
      if (type != expectedType[i])
        logerror("getParameterType returned %d for column %d. Expected was %d.", type, i+1, expectedType[i]);
    }
    
    // 5c. 
    loginfo("check column lengths");
    SQLDBC_Int4 expectedLength[NUMP] = {5, 8, 16, 10, 5, 10, 8, 26, 1, 5, 10, 20, 7};
    for (i=0; i<NUMP; i++) {
      SQLDBC_Int4 length = md->getColumnLength(i+1);
      if (length != expectedLength[i])
        logerror("getParameterLength returned %d for column %d. Expected was %d.", length, i+1, expectedLength[i]);
    }
    
    // 5d. 
    loginfo("check if columns are nullable");
    loginfo("Cannot perform this test case: kernel says 'optional' for all columns...");

    // 5e. 
    loginfo("check column names");
    char expectedName[NUMP][100] = {
				"VFIXED1",
				"VFIXED2",
				"VFLOAT",
				"VCHARASCII",
				"VCHARBYTE",
				"VDATE",
				"VTIME",
				"VTIMESTAMP",
				"VBOOLEAN",
				"VSMALLINT",
				"VINTEGER",
				"VVARCHARASCII",
        "VVARCHARBYTE"};
    for (i=0; i<NUMP; i++) {
      char name[100];
      memset(name, '\0', sizeof(name));
      SQLDBC_Length namesize = 0;
      md->getColumnName(i+1, name, SQLDBC_StringEncodingAscii, sizeof(name), &namesize);
      name[namesize] = '\0';
      if (strcmp(name, expectedName[i]))
        logerror("getParameterName returned %s for column %d. Expected was %s.", name, i+1, expectedName[i]);
    }
    }
    
    // 6. 
    loginfo("prepare INSERT");
    tc_errorcheck(pstmt)->prepare("INSERT INTO tableParameterMetaData\
      (vboolean, vinteger, vfloat, vfixed1, vvarcharbyte, vtime, vcharascii) VALUES (?, ?, ?, ?, ?, ?, ?)");
    
    // 7. 
    loginfo("getResultSetMetaData -> what should happen ?????");
    md = tc_errorcheck(pstmt)->getResultSetMetaData();
    if (md)
      loginfo("Result set meta data was returned. Num columns = %d", md->getColumnCount());
    else
      loginfo("No result set meta data was returned");
    
    connection()->releaseStatement(pstmt);

		return m_success;
	}
};

// check column name for correlated SAP query:
// select MAX((SELECT 1 FROM DUAL WHERE X.TABLENAME = X.TABLENAME )) FROM TABLES X

// TODO: parameterMetaData: names...
// noch mal drüber schauen: unicode...


int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new SimplePreparedStatement());
  testframe.addTestCase (new Disconnect());
  testframe.addTestCase (new Unicode());
  testframe.addTestCase (new isQuery());
  testframe.addTestCase (new ParameterMetaData());
  testframe.addTestCase (new ResultSetMetaData());
  if (testframe.run ()) 
    return 0;

  return 1;
}


