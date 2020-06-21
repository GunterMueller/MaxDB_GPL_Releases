/*!
  @file           typesCheck.cpp
  @author         D042427
  @ingroup        Tests
  @brief          1. checks if all types and constants all available 
                  2. check if functions return valid constant (Retcode and SQLType)
  @keywords       types, Retcode, SQLType
  @todo	          Retcodes SQLDBC_OVERFLOW, SQLDBC_SUCCESS_WITH_INFO not tested yet

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

class tC_Types_Consts : public SQLDBC_TestCase
{

public:
  tC_Types_Consts ()
    : SQLDBC_TestCase ("tC_Types_Consts")
  {}

  virtual SQLDBC_Bool run ()
  {
	/*********************************************************/
	/*                                                       */
	/*  *** Check types. Make will detect defects... ***     */
	/*                                                       */
	/*********************************************************/

	SQLDBC_ULong ul = 0;
	loginfo("SQLDBC_ULong is available");

	SQLDBC_Long l = 1;
	loginfo("SQLDBC_Long is available");

	SQLDBC_Byte b = 2;
	loginfo("SQLDBC_Byte is available");

	SQLDBC_size_t st = 3;
	loginfo("SQLDBC_size_t is available");

	SQLDBC_UTF8 utf8 = 4;
	loginfo("SQLDBC_UTF8 is available");

	SQLDBC_UInt1 ui1 = 5;
	loginfo("SQLDBC_UInt1 is available");

	SQLDBC_Int1 i1 = 6;
	loginfo("SQLDBC_Int1 is available");

	SQLDBC_UInt2 ui2 = 7;
	loginfo("SQLDBC_UInt2 is available");

	SQLDBC_Int2 i2 = 8;
	loginfo("SQLDBC_Int2 is available");

	SQLDBC_UInt4 ui4 = 9;
	loginfo("SQLDBC_UInt4 is available");

	SQLDBC_Int4 i4 = 10;
	loginfo("SQLDBC_Int4 is available");

	SQLDBC_UInt8 ui8 = 11;
	loginfo("SQLDBC_UInt8 is available");

	SQLDBC_Int8 i8 = 12;
	loginfo("SQLDBC_Int8 is available");

	SQLDBC_Length le = 13;
	loginfo("SQLDBC_Length is available");

	SQLDBC_UInt ui = 14;
	loginfo("SQLDBC_Int4 is available");

	SQLDBC_Int i = 15;
	loginfo("SQLDBC_Int is available");

	SQLDBC_StringEncoding se = SQLDBC_StringEncodingType::Ascii;
	loginfo("SQLDBC_StringEncoding is available");

	/*********************************************************/
	/*                                                       */
	/*  *** Check constants. Make will detect defects... *** */
	/*                                                       */
	/*********************************************************/

	/*** SQLDBC_Retcode ***/
	SQLDBC_Retcode rc;
	rc = SQLDBC_INVALID_OBJECT;
	rc = SQLDBC_OK;
	rc = SQLDBC_NOT_OK;
	rc = SQLDBC_DATA_TRUNC;
	rc = SQLDBC_OVERFLOW;
	rc = SQLDBC_SUCCESS_WITH_INFO;
	rc = SQLDBC_NO_DATA_FOUND;
	rc = SQLDBC_NEED_DATA;
	loginfo("All SQLDBC_Retcode constants available");

    /*** SQLDBC_Rowstatus ***/
	SQLDBC_Rowstatus rs;
	rs = SQLDBC_EXECUTE_FAILED;
	rs = SQLDBC_SUCCESS_NO_INFO;
	loginfo("All SQLDBC_Rowstatus constants available");

	/*** SQLDBC_SQLMode ***/
	SQLDBC_SQLMode sqlm;
	sqlm = SQLDBC_INTERNAL;
	sqlm = SQLDBC_ANSI;
	sqlm = SQLDBC_DB2;
	sqlm = SQLDBC_ORACLE;
	loginfo("All SQLDBC_SQLMode constants available");

	/*** SQLDBC_SQLType ***/
	SQLDBC_SQLType sqlt;
	sqlt = SQLDBC_SQLTYPE_MIN;
	sqlt = SQLDBC_SQLTYPE_FIXED;
	sqlt = SQLDBC_SQLTYPE_FLOAT;
	sqlt = SQLDBC_SQLTYPE_CHA;
	sqlt = SQLDBC_SQLTYPE_CHE;
	sqlt = SQLDBC_SQLTYPE_CHB;
	sqlt = SQLDBC_SQLTYPE_ROWID;
	sqlt = SQLDBC_SQLTYPE_STRA;
	sqlt = SQLDBC_SQLTYPE_STRE;
	sqlt = SQLDBC_SQLTYPE_STRB;
	sqlt = SQLDBC_SQLTYPE_STRDB;
	sqlt = SQLDBC_SQLTYPE_DATE;
	sqlt = SQLDBC_SQLTYPE_TIME;
	sqlt = SQLDBC_SQLTYPE_VFLOAT;
	sqlt = SQLDBC_SQLTYPE_TIMESTAMP;
	sqlt = SQLDBC_SQLTYPE_UNKNOWN;
	sqlt = SQLDBC_SQLTYPE_NUMBER;
	sqlt = SQLDBC_SQLTYPE_NONUMBER;
	sqlt = SQLDBC_SQLTYPE_DURATION;
	sqlt = SQLDBC_SQLTYPE_DBYTEEBCDIC;
	sqlt = SQLDBC_SQLTYPE_LONGA;
	sqlt = SQLDBC_SQLTYPE_LONGE;
	sqlt = SQLDBC_SQLTYPE_LONGB;
	sqlt = SQLDBC_SQLTYPE_LONGDB;
	sqlt = SQLDBC_SQLTYPE_BOOLEAN;
	sqlt = SQLDBC_SQLTYPE_UNICODE;
	sqlt = SQLDBC_SQLTYPE_DTFILLER1;
	sqlt = SQLDBC_SQLTYPE_DTFILLER2;
	sqlt = SQLDBC_SQLTYPE_DTFILLER3;
	sqlt = SQLDBC_SQLTYPE_DTFILLER4;
	sqlt = SQLDBC_SQLTYPE_SMALLINT;
	sqlt = SQLDBC_SQLTYPE_INTEGER;
	sqlt = SQLDBC_SQLTYPE_VARCHARA;
	sqlt = SQLDBC_SQLTYPE_VARCHARE;
	sqlt = SQLDBC_SQLTYPE_VARCHARB;
	sqlt = SQLDBC_SQLTYPE_STRUNI;
	sqlt = SQLDBC_SQLTYPE_LONGUNI;
	sqlt = SQLDBC_SQLTYPE_VARCHARUNI;
	sqlt = SQLDBC_SQLTYPE_UDT;
	sqlt = SQLDBC_SQLTYPE_ABAPTABHANDLE;
	sqlt = SQLDBC_SQLTYPE_DWYDE;
	sqlt = SQLDBC_SQLTYPE_MAX;
	loginfo("All SQLDBC_SQLType constants available");

	/*** SQLDBC_HostType ***/
	SQLDBC_HostType ht;
	ht = SQLDBC_HOSTTYPE_MIN;
	ht = SQLDBC_HOSTTYPE_PARAMETER_NOTSET;
	ht = SQLDBC_HOSTTYPE_BINARY;
	ht = SQLDBC_HOSTTYPE_ASCII;
	ht = SQLDBC_HOSTTYPE_UTF8;
	ht = SQLDBC_HOSTTYPE_UINT1;
	ht = SQLDBC_HOSTTYPE_INT1;
	ht = SQLDBC_HOSTTYPE_UINT2;
	ht = SQLDBC_HOSTTYPE_INT2;
	ht = SQLDBC_HOSTTYPE_UINT4;
	ht = SQLDBC_HOSTTYPE_INT4;
	ht = SQLDBC_HOSTTYPE_UINT8;
	ht = SQLDBC_HOSTTYPE_INT8;
	ht = SQLDBC_HOSTTYPE_DOUBLE;
	ht = SQLDBC_HOSTTYPE_FLOAT;
	ht = SQLDBC_HOSTTYPE_ODBCDATE;
	ht = SQLDBC_HOSTTYPE_ODBCTIME;
	ht = SQLDBC_HOSTTYPE_ODBCTIMESTAMP;
	ht = SQLDBC_HOSTTYPE_ODBCNUMERIC;
	ht = SQLDBC_HOSTTYPE_GUID;
	ht = SQLDBC_HOSTTYPE_UCS2;
	ht = SQLDBC_HOSTTYPE_UCS2_SWAPPED;
	ht = SQLDBC_HOSTTYPE_UCS2_NATIVE;
	ht = SQLDBC_HOSTTYPE_USERDEFINED;
	ht = SQLDBC_HOSTTYPE_MAX;
	loginfo("All SQLDBC_HostType constants available");

	/*** SQLDBC_SQLWarningCode ***/
	SQLDBC_SQLWarningCode sqlwc;
	sqlwc = SQLDBC_SQLWARNING_DATA_TRUNCATED;
	sqlwc = SQLDBC_SQLWARNING_MAX;
	loginfo("All SQLDBC_SQLWarningCode constants available");

	return m_success;
  }
};

class tC_Retcodes : public SQLDBC_TestCase
{

public:
  tC_Retcodes ()
    : SQLDBC_TestCase ("tC_Retcodes")
  {}

  virtual SQLDBC_Bool run ()
  {
	SQLDBC_Connection *conn = connection();
	if (!conn || !conn->isConnected()) {
		logerror("Session is not connected. Test case must exit");
		return 0;
	}

	/*********************************************************/
	/*                                                       */
	/* *** Call functions we expect to return the codes      */
	/*     we are about to check ***                         */
	/*                                                       */
	/*********************************************************/

	SQLDBC_Retcode rc;
	SQLDBC_Statement *stmt = conn->createStatement();
	if (!stmt) {
		logerror("Statement was not created. Test case must exit");
		return 0;
	}

	/*****************/
	/*** SQLDBC_OK ***/
	/*****************/
	loginfo("Check SQLDBC_NOT_OK...");
	rc = stmt->execute(NULL);
	if (rc != SQLDBC_NOT_OK)
		logerror("Unexpected return code: %d. Expected was SQLDBC_NOT_OK [%d]", rc, SQLDBC_NOT_OK);

	/*********************/
	/*** SQLDBC_NOT_OK ***/
	/*********************/
	loginfo("Check SQLDBC_OK...");
	rc = stmt->execute("SELECT 'TEST' FROM Dual");
	if (rc != SQLDBC_OK)
		logerror("Unexpected return code: %d. Expected was SQLDBC_OK [%d]", rc, SQLDBC_OK);

	/*************************/
	/*** SQLDBC_DATA_TRUNC ***/
	/*************************/
	loginfo("Check SQLDBC_DATA_TRUNC...");
    SQLDBC_ResultSet *rs = stmt->getResultSet();
	if (!rs) {
		logerror("No result set was returned. Test case must exit");
		return 0;
	}
	tc_errorcheck(rs)->next();
	SQLDBC_Length ind = SQLDBC_NTS;
	char cdata[100];
	rc = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, 4);
	if (rc != SQLDBC_DATA_TRUNC)
		logerror("Unexpected return code: %d. Expected was SQLDBC_DATA_TRUNC [%d]", rc, SQLDBC_DATA_TRUNC);

	/****************************/
	/*** SQLDBC_NO_DATA_FOUND ***/
	/****************************/
	loginfo("Check SQLDBC_NO_DATA_FOUND...");
	// pre-condition: previous test was successful
	if (rc == SQLDBC_DATA_TRUNC)
	{
		// fetch once again to get the rest of the string
		rc = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata));
		if (rc == SQLDBC_OK)
		{
			rc = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata));
			if (rc != SQLDBC_NO_DATA_FOUND)
				logerror("Unexpected return code: %d. Expected was SQLDBC_DATA_TRUNC [%d]", rc, SQLDBC_DATA_TRUNC);
		}
	}

    connection()->releaseStatement(stmt);
	/***********************/
	/*** SQLDBC_OVERFLOW ***/
	/***********************/
	// Retcode not uses yet


	/********************************/
	/*** SQLDBC_SUCCESS_WITH_INFO ***/
	/********************************/
	// Retcode not uses yet

	/************************/
	/*** SQLDBC_NEED_DATA ***/
	/************************/
	loginfo("Check SQLDBC_NEED_DATA...");
	SQLDBC_PreparedStatement *pstmt = conn->createPreparedStatement();
	if (!pstmt) {
		logerror("PreparedStatement was not created. Test case must exit");
		return 0;
	}
	tc_errorcheck(pstmt)->prepare("SELECT * FROM DUAL WHERE 1=?");
	int val;
	ind = SQLDBC_DATA_AT_EXEC;
	tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &val, &ind, sizeof(val));
	rc = pstmt->execute();
	if (rc != SQLDBC_NEED_DATA)
		logerror("Unexpected return code: %d. Expected was SQLDBC_NEED_DATA [%d]", rc, SQLDBC_NEED_DATA);

    connection()->releaseStatement(pstmt);
    
	/*****************************/
	/*** SQLDBC_INVALID_OBJECT ***/
	/*****************************/
	loginfo("Check SQLDBC_INVALID_OBJECT...");
	SQLDBC_Connection *conn2 = 0;
	rc = conn2->commit();
	if (rc != SQLDBC_INVALID_OBJECT)
		logerror("Unexpected return code: %d. Expected was SQLDBC_NOT_OK [%d]", rc, SQLDBC_INVALID_OBJECT);

	return m_success;
  }
};

class tC_SQLTypes : public SQLDBC_TestCase
{

private:
	SQLDBC_Bool checkSQLType(SQLDBC_Statement *stmt, const char *strSQL, SQLDBC_SQLType expectedSQLType)
	{
		SQLDBC_Retcode rc;
		
		tc_errorcheckrc(rc, stmt)->execute(strSQL);
		SQLDBC_ResultSet *rs = stmt->getResultSet();
		if (!rs) {
			logerror("No result set was returned");
			return 0;
		}

		SQLDBC_ResultSetMetaData *md = rs->getResultSetMetaData();
		if (!md) {
			logerror("No meta data was returned");
			return 0;
		}

		if (md->getColumnCount() != 1) {
			logerror("Expected number of columns were returned: %d, Expected was 1", md->getColumnCount());
		} else {
			if (md->getColumnType(1) != expectedSQLType)
				logerror("The wrong SQLType was returned: %d. Expected was %d", md->getColumnType(1), expectedSQLType);
		}

		return m_success;
	}

public:
  tC_SQLTypes ()
    : SQLDBC_TestCase ("tC_SQLTypes")
  {}

  virtual SQLDBC_Bool prepare ()
  {
	dropTable("testtable123");
	return m_success;
  }

  virtual SQLDBC_Bool release ()
  {
	dropTable("testtable123");
	return m_success;
  }

  virtual SQLDBC_Bool run ()
  {
	SQLDBC_Retcode rc;
	SQLDBC_Connection *conn = connection();
	conn->setSQLMode(SQLDBC_INTERNAL);

	if (!conn->isConnected())
	{
		logerror("Session is not connected. Test case must exit");
		return 0;
	}

	loginfo("/*** We are%s working with a unicode database ***/", conn->isUnicodeDatabase() ? "" : " NOT");

	/*********************************************************/
	/*                                                       */
	/*  *** Current state: Connected (mode: INTERNAL) ***    */
	/*                                                       */
	/*********************************************************/

	/*** CREATE TABLE ***/
	loginfo("create table with all possible data types...");
	SQLDBC_Statement *stmt = conn->createStatement();
	if (!stmt) {
		logerror("No statement object was created. Test case must exit");
		return 0;
	}

	if (conn->isUnicodeDatabase())
	{
		/*** Unicode database ***/
		tc_errorcheckrc(rc, stmt)->execute("CREATE TABLE testtable123 (\
				vfixed FIXED(5), \
				vfloat FLOAT, \
				vcharascii CHAR(10) ASCII, \
				vcharbyte CHAR(5) BYTE, \
				vlongascii LONG ASCII, \
				vlongbyte LONG BYTE, \
				vdate DATE, \
				vtime TIME, \
				vtimestamp TIMESTAMP, \
				vboolean BOOLEAN, \
				vcharunicode CHAR(20) UNICODE, \
				vsmallint SMALLINT, \
				vinteger INTEGER, \
				vvarcharascii VARCHAR(20) ASCII, \
				vvarcharbyte VARCHAR(5) BYTE, \
				vlongunicode LONG UNICODE, \
				vvarcharunicode VARCHAR(20) UNICODE)");

		tc_errorcheckrc(rc, stmt)->execute("INSERT INTO testtable123 VALUES (\
				0, 0, 'CHAR ASCII', x'ffff00ff00', 'LONG ASCII', x'bb00ff', \
				'1988-12-25', '17:12:30.000000', \
				'1988-12-25 17:12:30.000000', TRUE, 'CHAR UNICODE', \
				0, 0, 'VARCHAR ASCII', x'ffff', 'LONG UNICODE', 'VARCHAR UNICODE')");
	}
	else
	{
		/*** Pure ascii database ***/
		tc_errorcheckrc(rc, stmt)->execute("CREATE TABLE testtable123 (\
				vfixed FIXED(5), \
				vfloat FLOAT, \
				vcharascii CHAR(10) ASCII, \
				vcharbyte CHAR(5) BYTE, \
				vlongascii LONG ASCII, \
				vlongbyte LONG BYTE, \
				vdate DATE, \
				vtime TIME, \
				vtimestamp TIMESTAMP, \
				vboolean BOOLEAN, \
				vsmallint SMALLINT, \
				vinteger INTEGER, \
				vvarcharascii VARCHAR(20) ASCII, \
				vvarcharbyte VARCHAR(5) BYTE)");

		tc_errorcheckrc(rc, stmt)->execute("INSERT INTO testtable123 VALUES (\
				0, 0, 'CHAR ASCII', x'ffff00ff00', 'LONG ASCII', x'bb00ff', \
				'1988-12-25', '17:12:30.000000', \
				'1988-12-25 17:12:30.000000', TRUE, \
				0, 0, 'VARCHAR ASCII', x'ffff')");
	}


	/*********************************************************/
	/*                                                       */
	/*  *** Current state: Table created, data inserted ***  */
	/*                                                       */
	/*********************************************************/

	/*** SELECT [FIXED] ***/
	loginfo("Check FIXED <--> SQLDBC_SQLTYPE_FIXED...");
	m_success &= checkSQLType(stmt, "SELECT vfixed FROM Testtable123", SQLDBC_SQLTYPE_FIXED);
    
	/*** SELECT [FLOAT] ***/
	loginfo("Check FLOAT <--> SQLDBC_SQLTYPE_FLOAT...");
	m_success &= checkSQLType(stmt, "SELECT vfloat FROM Testtable123", SQLDBC_SQLTYPE_FLOAT);
    
	/*** SELECT [CHAR ASCII] ***/
	loginfo("Check CHAR ASCII <--> SQLDBC_SQLTYPE_CHA...");
	m_success &= checkSQLType(stmt, "SELECT vcharascii FROM Testtable123", SQLDBC_SQLTYPE_CHA);
    
	/*** SELECT [CHAR BYTE] ***/
	loginfo("Check CHAR BYTE <--> SQLDBC_SQLTYPE_CHB...");
	m_success &= checkSQLType(stmt, "SELECT vcharbyte FROM Testtable123", SQLDBC_SQLTYPE_CHB);
    
	/*** SELECT [LONG ASCII] ***/
	loginfo("Check LONG ASCII <--> SQLDBC_SQLTYPE_STRA...");
	m_success &= checkSQLType(stmt, "SELECT vlongascii FROM Testtable123", SQLDBC_SQLTYPE_STRA);
    
	/*** SELECT [LONG BYTE] ***/
	loginfo("Check LONG BYTE <--> SQLDBC_SQLTYPE_STRB...");
	m_success &= checkSQLType(stmt, "SELECT vlongbyte FROM Testtable123", SQLDBC_SQLTYPE_STRB);
    
	/*** SELECT [DATE] ***/
	loginfo("Check DATE <--> SQLDBC_SQLTYPE_DATE...");
	m_success &= checkSQLType(stmt, "SELECT vdate FROM Testtable123", SQLDBC_SQLTYPE_DATE);
    
	/*** SELECT [TIME] ***/
	loginfo("Check TIME <--> SQLDBC_SQLTYPE_TIME...");
	m_success &= checkSQLType(stmt, "SELECT vtime FROM Testtable123", SQLDBC_SQLTYPE_TIME);
    
	/*** SELECT [TIMESTAMP] ***/
	loginfo("Check TIMESTAMP <--> SQLDBC_SQLTYPE_TIMESTAMP...");
	m_success &= checkSQLType(stmt, "SELECT vtimestamp FROM Testtable123", SQLDBC_SQLTYPE_TIMESTAMP);
    
	/*** SELECT [BOOLEAN] ***/
	loginfo("Check BOOLEAN <--> SQLDBC_SQLTYPE_BOOLEAN...");
	m_success &= checkSQLType(stmt, "SELECT vboolean FROM Testtable123", SQLDBC_SQLTYPE_BOOLEAN);
    
	/*** SELECT [SMALLINT] ***/
	loginfo("Check SMALLINT <--> SQLDBC_SQLTYPE_SMALLINT...");
	m_success &= checkSQLType(stmt, "SELECT vsmallint FROM Testtable123", SQLDBC_SQLTYPE_SMALLINT);
    
	/*** SELECT [INTEGER] ***/
	loginfo("Check INTEGER <--> SQLDBC_SQLTYPE_INTEGER...");
	m_success &= checkSQLType(stmt, "SELECT vinteger FROM Testtable123", SQLDBC_SQLTYPE_INTEGER);
    
	/*** SELECT [VARCHAR ASCII] ***/
	loginfo("Check VARCHAR ASCII <--> SQLDBC_SQLTYPE_VARCHARA...");
	m_success &= checkSQLType(stmt, "SELECT vvarcharascii FROM Testtable123", SQLDBC_SQLTYPE_VARCHARA);
    
	/*** SELECT [VARCHAR BYTE] ***/
	loginfo("Check VARCHAR BYTE <--> SQLDBC_SQLTYPE_VARCHARB...");
	m_success &= checkSQLType(stmt, "SELECT vvarcharbyte FROM Testtable123", SQLDBC_SQLTYPE_VARCHARB);

	if (conn->isUnicodeDatabase())
	{
		/*** SELECT [CHAR UNICODE] ***/
		loginfo("Check CHAR UNICODE <--> SQLDBC_SQLTYPE_UNICODE...");
		m_success &= checkSQLType(stmt, "SELECT vcharunicode FROM Testtable123", SQLDBC_SQLTYPE_UNICODE);
    
		/*** SELECT [LONG UNICODE] ***/
		loginfo("Check LONG UNICODE <--> SQLDBC_SQLTYPE_STRUNI...");
		m_success &= checkSQLType(stmt, "SELECT vlongunicode FROM Testtable123", SQLDBC_SQLTYPE_STRUNI);
    
		/*** SELECT [VARCHAR UNICODE] ***/
		loginfo("Check VARCHAR UNICODE <--> SQLDBC_SQLTYPE_VARCHARUNI...");
		m_success &= checkSQLType(stmt, "SELECT vvarcharunicode FROM Testtable123", SQLDBC_SQLTYPE_VARCHARUNI);
	}
    
    conn->releaseStatement(stmt);
    
	// SQLDBC_SQLTYPE_UDT, SQLDBC_SQLTYPE_ABAPTABHANDLE, SQLDBC_SQLTYPE_DWYDE cannot be checked

	return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new tC_Types_Consts());
  testframe.addTestCase (new tC_Retcodes());
  testframe.addTestCase (new tC_SQLTypes());
  if (testframe.run ()) 
    return 0;

  return 1;
}


