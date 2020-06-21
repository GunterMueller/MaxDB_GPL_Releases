/*!
  @file           truncnames.cpp
  @author         D042427
  @ingroup        Tests
  @brief          checks if the truncation of meta data succeeds...
  @keywords       truncation, ascii, ucs2, utf-8
  @bugs           bbb: unicode SQL statement, getCursorName as ascii not truncated -> wrong bufferLength
                  ccc: unicode SQL statement, getCursorName as unicode truncated -> wrong bufferLength
                  ddd: http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130288


	PreparedStatementAscii
		- check getCursorName, getTableName

	PreparedStatementUnicode
		- check getCursorName, getTableName (Unicode) with ASCII statement
		- check getCursorName, getTableName (Unicode) with unicode statement

  PreparedStatementUTF8
		- check getCursorName, getTableName (UTF8) with ASCII statement

  StatementAscii
		- check getCursorName, getTableName

  StatementUnicode
		- check getCursorName, getTableName (Unicode) with ASCII statement
		- check getCursorName, getTableName (Unicode) with unicode statement

  StatementUTF8
		- check getCursorName, getTableName (UTF8) with ASCII statement

	ParameterMetaDataAscii
		- check getParameterName

	ParameterMetaDataUnicode
		- check getParameterName (Unicode)

	ParameterMetaDataUTF8
		- check getParameterName (UTF-8)

  ResultSetMetaDataAscii
    - check getColumnName

  ResultSetMetaDataUnicode
    - check getColumnName (Unicode)

  ResultSetMetaDataUTF8
    - check getColumnName (UTF-8)


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

/**
 * gets length of UCS2 string (in bytes - not in characters)
 **/
int strlenUCS2(UCS2CHAR *str) {
  int pos = 0;
  UCS2CHAR *temp = str;
  while (*temp != 0x0000) {
    temp++;
    pos++;
    pos++;
  }
  return pos;
}




/* -----   PreparedStatementAscii   -----
 *
 * check getCursorName, getTableName
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. prepare and execute SELECT statement (ASCII)
 * 4. getCursorName (buffer size big enough [100]), store name
 * 5. getCursorName (buffer size = 0)
 * 6. getCursorName (buffer size too small)
 * 7. getTableName (buffer size big enough [100]), store name
 * 8. getTableName (buffer size = 0)
 * 9. getTableName (buffer size too small)
 *
 */
class PreparedStatementAscii : public SQLDBC_TestCase
{

public:
	PreparedStatementAscii ()
		: SQLDBC_TestCase ("PreparedStatementAscii")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("* PreparedStatement: cursor name, table name [ascii]*");
		loginfo("*****************************************************");

		dropTable("tablePreparedStatementAscii");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tablePreparedStatementAscii (i INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tablePreparedStatementAscii (i) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tablePreparedStatementAscii");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();
    char name[100];
    char buffer[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

		// 3.
		loginfo("3. prepare and execute SELECT statement (ASCII)");
    tc_errorcheck(pstmt)->prepare("SELECT * FROM tablePreparedStatementAscii FOR UPDATE");
    tc_errorcheck(pstmt)->execute();

		// 4.
		loginfo("4. getCursorName (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(pstmt)->getCursorName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getCursorName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getCursorName (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 7.
		loginfo("7. getTableName (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(pstmt)->getTableName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("table name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 8.
		loginfo("8. getTableName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9.
		loginfo("9. getTableName (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);

    connection()->releaseStatement(pstmt);

		return m_success;
	}
};

/* -----   PreparedStatementUnicode   -----
 *
 * check getCursorName, getTableName (Unicode) with ASCII statement
 * check getCursorName, getTableName (Unicode) with unicode statement
 *
 * precondition for this test case: UNICODE database
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. prepare and execute SELECT statement (UNICODE)
 * 4. getCursorName ASCII (buffer size big enough [100]), store name
 * 5. getCursorName ASCII (buffer size = 0)
 * 6. getCursorName ASCII (buffer size too small)
 * 7. getCursorName UNICODE (buffer size big enough [100]), store name
 * 8. getCursorName UNICODE (buffer size = 0)
 * 9. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)
 * 9b. getCursorName UNICODE (buffer size too small - not fitting for truncation)
 * 10. getTableName ASCII (buffer size big enough [100]), store name
 * 11. getTableName ASCII (buffer size = 0)
 * 12. getTableName ASCII (buffer size too small)
 * 13. getTableName UNICODE (buffer size big enough [100]), store name
 * 14. getTableName UNICODE (buffer size = 0)
 * 15. getTableName UNICODE (buffer size too small - exactly fitting for truncation)
 * 15b. getTableName UNICODE (buffer size too small - not fitting for truncation)
 *
 * 16. prepare and execute SELECT statement (ASCII)
 * 17. getCursorName UNICODE (buffer size big enough [100]), store name
 * 18. getCursorName UNICODE (buffer size = 0)
 * 19. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)
 * 19b. getCursorName UNICODE (buffer size too small - not fitting for truncation)
 * 20. getTableName UNICODE (buffer size big enough [100]), store name
 * 21. getTableName UNICODE (buffer size = 0)
 * 22. getTableName UNICODE (buffer size too small - exactly fitting for truncation)
 * 22b. getTableName UNICODE (buffer size too small - not fitting for truncation)
 *
 */
class PreparedStatementUnicode : public SQLDBC_TestCase
{

public:
	PreparedStatementUnicode ()
		: SQLDBC_TestCase ("PreparedStatementUnicode")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*******************************************************");
    loginfo("* PreparedStatement: cursor name, table name [unicode]*");
		loginfo("*******************************************************");

    if (!connection()->isUnicodeDatabase()) {
      loginfo("We do not have a unicode database...");
      loginfo("We can't perform these tests...");
      return m_success;
    }

		dropTable("tablePreparedStatementUnicode");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tablePreparedStatementUnicode (i INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tablePreparedStatementUnicode (i) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tablePreparedStatementUnicode");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
    // check precondition
    if (!connection()->isUnicodeDatabase())
      return m_success;

		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();
    char name[100];
    UCS2CHAR name_u[100];
    char buffer[100];
    UCS2CHAR buffer_u[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

    char asciiSQL[100];
    UCS2CHAR unicodeSQL[100];

		// 3.
		loginfo("3. prepare and execute SELECT statement (UNICODE)");
    sprintf(asciiSQL, "SELECT * FROM tablePreparedStatementUnicode FOR UPDATE");
    convertToUCS2(unicodeSQL, sizeof(unicodeSQL), false, asciiSQL);
    tc_errorcheck(pstmt)->prepare((char *)unicodeSQL, SQLDBC_StringEncodingUCS2);
    tc_errorcheck(pstmt)->execute();

		// 4.
		loginfo("4. getCursorName ASCII (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(pstmt)->getCursorName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    // bbb
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getCursorName ASCII (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getCursorName ASCII (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 7.
		loginfo("7. getCursorName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(pstmt)->getCursorName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlenUCS2(name_u))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 8.
		loginfo("8. getCursorName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9.
		loginfo("9. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9b.
		loginfo("9b. getCursorName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, pstmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 10.
		loginfo("10. getTableName ASCII (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(pstmt)->getTableName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    // bbb
    if (nameLen >= sizeof(name)) {
      logerror("Table name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 11.
		loginfo("11. getTableName ASCII (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 12.
		loginfo("12. getTableName ASCII (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 13.
		loginfo("13. getTableName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(pstmt)->getTableName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("Table name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    if (nameLen != strlenUCS2(name_u))
      logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 14.
		loginfo("14. getTableName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 15.
		loginfo("15. getTableName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 15b.
		loginfo("15b. getTableName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, pstmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 16.
		loginfo("16. prepare and execute SELECT statement (ASCII)");
    tc_errorcheck(pstmt)->prepare("SELECT * FROM tablePreparedStatementUnicode FOR UPDATE");
    tc_errorcheck(pstmt)->execute();


		// 17.
		loginfo("17. getCursorName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(pstmt)->getCursorName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlenUCS2(name_u))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 18.
		loginfo("18. getCursorName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 19.
		loginfo("19. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 19b.
		loginfo("19b. getCursorName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, pstmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 20.
		loginfo("20. getTableName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(pstmt)->getTableName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("Table name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");


		// 21.
		loginfo("21. getTableName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 22.
		loginfo("22. getTableName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 22b.
		loginfo("22b. getTableName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, pstmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(pstmt);

		return m_success;
	}
};


/* -----   PreparedStatementUTF8   -----
 *
 * check getCursorName, getTableName (UTF8) with ASCII statement
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. prepare and execute SELECT statement (ASCII)
 * 4. set cursor name to non ASCII-127 name
 * 5. getCursorName UTF8 (buffer size big enough [100]), store name
 * 6. getCursorName UTF8 (buffer size = 0)
 * 7. getCursorName UTF8 (buffer size too small - exactly fitting for truncation)
 * 8. getCursorName UTF8 (buffer size too small - not fitting for truncation)
 * 9. getTableName UTF8 (buffer size big enough [100]), store name
 * 10. getTableName UTF8 (buffer size = 0)
 * 11. getTableName UTF8 (buffer size too small - exactly fitting for truncation)
 * 12. getCursorName UTF8 (buffer size too small - not fitting for truncation)
 *
 */
class PreparedStatementUTF8 : public SQLDBC_TestCase
{

public:
	PreparedStatementUTF8 ()
		: SQLDBC_TestCase ("PreparedStatementUTF8")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("* PreparedStatement: cursor name, table name [utf-8]*");
		loginfo("*****************************************************");

		dropTable("ההההההה");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE ההההההה (i INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO ההההההה (i) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("ההההההה");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();
    char name[100];
    char buffer[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

		// 3.
		loginfo("3. prepare and execute SELECT statement (ASCII)");
    tc_errorcheck(pstmt)->prepare("SELECT * FROM ההההההה FOR UPDATE");
    tc_errorcheck(pstmt)->execute();

		// 4.
		loginfo("4. set cursor name to non ASCII-127 name");
    tc_errorcheck(pstmt)->setCursorName("צצצצצצצצצצ", SQLDBC_NTS, SQLDBC_StringEncodingAscii);

    // 5.
		loginfo("5. getCursorName (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(pstmt)->getCursorName(name, SQLDBC_StringEncodingUTF8, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 6.
		loginfo("6. getCursorName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getCursorName(buffer, SQLDBC_StringEncodingUTF8, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 7.
		loginfo("7. getCursorName UTF8 (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;

    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off the last character: 2 bytes
    // so the new buffer length must be:
    bufSize = nameLen-1;

    tc_errorcheckrc(rc, pstmt)->getCursorName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);

    // 8.
    loginfo("8. getCursorName UTF8 (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;

    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off half of the last character: 1 byte
    // so the new buffer length must be:
    bufSize = nameLen;

    tc_errorcheckrc(rc, pstmt)->getCursorName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-2)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] == '\0')
        logerror("Zero-termination was set at the wrong place: string has not been truncated correctly.");
      else
        if (buffer[bufSize-2] != '\0')
          logerror("Zero-termination was not set");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);

		// 9.
		loginfo("9. getTableName UTF8 (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(pstmt)->getTableName(name, SQLDBC_StringEncodingUTF8, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("table name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 10.
		loginfo("10. getTableName UTF8 (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, pstmt)->getTableName(buffer, SQLDBC_StringEncodingUTF8, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 11.
		loginfo("11. getTableName UTF8 (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off the last two characters: 3 bytes
    // so the new buffer length must be:
    bufSize = nameLen-2;

    tc_errorcheckrc(rc, pstmt)->getTableName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    // 12.
    loginfo("12. getTableName UTF8 (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off the last one and a half characters: 2 bytes
    // so the new buffer length must be:
    bufSize = nameLen - 1;

    tc_errorcheckrc(rc, pstmt)->getTableName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-2)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] == '\0')
        logerror("Zero-termination was set at the wrong place: string has not been truncated correctly.");
      else
        if (buffer[bufSize-2] != '\0')
          logerror("Zero-termination was not set");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(pstmt);

		return m_success;
	}
};



/* -----   StatementAscii   -----
 *
 * check getCursorName, getTableName
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. execute SELECT statement (ASCII)
 * 4. getCursorName (buffer size big enough [100]), store name
 * 5. getCursorName (buffer size = 0)
 * 6. getCursorName (buffer size too small)
 * 7. getTableName (buffer size big enough [100]), store name
 * 8. getTableName (buffer size = 0)
 * 9. getTableName (buffer size too small)
 *
 */
class StatementAscii : public SQLDBC_TestCase
{

public:
	StatementAscii ()
		: SQLDBC_TestCase ("StatementAscii")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("* Statement: cursor name, table name [ascii]        *");
		loginfo("*****************************************************");

		dropTable("tableStatementAscii");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tableStatementAscii (i INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tableStatementAscii (i) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableStatementAscii");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_Statement *stmt = connection()->createStatement();
    char name[100];
    char buffer[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

		// 3.
		loginfo("3. execute SELECT statement (ASCII)");
    tc_errorcheck(stmt)->execute("SELECT * FROM tableStatementAscii FOR UPDATE");

		// 4.
		loginfo("4. getCursorName (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(stmt)->getCursorName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getCursorName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getCursorName (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 7.
		loginfo("7. getTableName (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(stmt)->getTableName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("table name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 8.
		loginfo("8. getTableName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9.
		loginfo("9. getTableName (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);

    connection()->releaseStatement(stmt);

		return m_success;
	}
};

/* -----   StatementUnicode   -----
 *
 * check getCursorName, getTableName (Unicode) with ASCII statement
 * check getCursorName, getTableName (Unicode) with unicode statement
 *
 * precondition for this test case: UNICODE database
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. execute SELECT statement (UNICODE)
 * 4. getCursorName ASCII (buffer size big enough [100]), store name
 * 5. getCursorName ASCII (buffer size = 0)
 * 6. getCursorName ASCII (buffer size too small)
 * 7. getCursorName UNICODE (buffer size big enough [100]), store name
 * 8. getCursorName UNICODE (buffer size = 0)
 * 9. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)
 * 9b. getCursorName UNICODE (buffer size too small - not fitting for truncation)
 * 10. getTableName ASCII (buffer size big enough [100]), store name
 * 11. getTableName ASCII (buffer size = 0)
 * 12. getTableName ASCII (buffer size too small)
 * 13. getTableName UNICODE (buffer size big enough [100]), store name
 * 14. getTableName UNICODE (buffer size = 0)
 * 15. getTableName UNICODE (buffer size too small - exactly fitting for truncation)
 * 15b. getTableName UNICODE (buffer size too small - not fitting for truncation)
 *
 * 16. execute SELECT statement (ASCII)
 * 17. getCursorName UNICODE (buffer size big enough [100]), store name
 * 18. getCursorName UNICODE (buffer size = 0)
 * 19. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)
 * 19b. getCursorName UNICODE (buffer size too small - not fitting for truncation)
 * 20. getTableName UNICODE (buffer size big enough [100]), store name
 * 21. getTableName UNICODE (buffer size = 0)
 * 22. getTableName UNICODE (buffer size too small - exactly fitting for truncation)
 * 22b. getTableName UNICODE (buffer size too small - not fitting for truncation)
 *
 */
class StatementUnicode : public SQLDBC_TestCase
{

public:
	StatementUnicode ()
		: SQLDBC_TestCase ("StatementUnicode")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*******************************************************");
    loginfo("* Statement: cursor name, table name [unicode]        *");
		loginfo("*******************************************************");

    if (!connection()->isUnicodeDatabase()) {
      loginfo("We do not have a unicode database...");
      loginfo("We can't perform these tests...");
      return m_success;
    }

		dropTable("tableStatementUnicode");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tableStatementUnicode (i INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tableStatementUnicode (i) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableStatementUnicode");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
    // check precondition
    if (!connection()->isUnicodeDatabase())
      return m_success;

		SQLDBC_Statement *stmt = connection()->createStatement();
    char name[100];
    UCS2CHAR name_u[100];
    char buffer[100];
    UCS2CHAR buffer_u[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

    char asciiSQL[100];
    UCS2CHAR unicodeSQL[100];

		// 3.
		loginfo("3. execute SELECT statement (UNICODE)");
    sprintf(asciiSQL, "SELECT * FROM tablePreparedStatementUnicode FOR UPDATE");
    convertToUCS2(unicodeSQL, sizeof(unicodeSQL), false, asciiSQL);
    tc_errorcheck(stmt)->execute((char *)unicodeSQL, SQLDBC_StringEncodingUCS2);

		// 4.
		loginfo("4. getCursorName ASCII (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(stmt)->getCursorName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    // bbb
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getCursorName ASCII (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getCursorName ASCII (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getCursorName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 7.
		loginfo("7. getCursorName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(stmt)->getCursorName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlenUCS2(name_u))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 8.
		loginfo("8. getCursorName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9.
		loginfo("9. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9b.
		loginfo("9b. getCursorName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, stmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 10.
		loginfo("10. getTableName ASCII (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(stmt)->getTableName(name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    // bbb
    if (nameLen >= sizeof(name)) {
      logerror("Table name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 11.
		loginfo("11. getTableName ASCII (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 12.
		loginfo("12. getTableName ASCII (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getTableName(buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 13.
		loginfo("13. getTableName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(stmt)->getTableName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("Table name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    if (nameLen != strlenUCS2(name_u))
      logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 14.
		loginfo("14. getTableName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 15.
		loginfo("15. getTableName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 15b.
		loginfo("15b. getTableName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, stmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 16.
		loginfo("16. execute SELECT statement (ASCII)");
    tc_errorcheck(stmt)->execute("SELECT * FROM tablePreparedStatementUnicode FOR UPDATE");


		// 17.
		loginfo("17. getCursorName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(stmt)->getCursorName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlenUCS2(name_u))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 18.
		loginfo("18. getCursorName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 19.
		loginfo("19. getCursorName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 19b.
		loginfo("19b. getCursorName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, stmt)->getCursorName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 20.
		loginfo("20. getTableName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    tc_errorcheck(stmt)->getTableName((char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("Table name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");


		// 21.
		loginfo("21. getTableName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 22.
		loginfo("22. getTableName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 22b.
		loginfo("22b. getTableName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    tc_errorcheckrc(rc, stmt)->getTableName((char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // ccc
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(stmt);

		return m_success;
	}
};


/* -----   StatementUTF8   -----
 *
 * check getCursorName, getTableName (UTF8) with ASCII statement
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. execute SELECT statement (ASCII)
 * 4. set cursor name to non ASCII-127 name
 * 5. getCursorName UTF8 (buffer size big enough [100]), store name
 * 6. getCursorName UTF8 (buffer size = 0)
 * 7. getCursorName UTF8 (buffer size too small - exactly fitting for truncation)
 * 8. getCursorName UTF8 (buffer size too small - not fitting for truncation)
 * 9. getTableName UTF8 (buffer size big enough [100]), store name
 * 10. getTableName UTF8 (buffer size = 0)
 * 11. getTableName UTF8 (buffer size too small - exactly fitting for truncation)
 * 12. getTableName UTF8 (buffer size too small - not fitting for truncation)
 *
 */
class StatementUTF8 : public SQLDBC_TestCase
{

public:
	StatementUTF8 ()
		: SQLDBC_TestCase ("StatementUTF8")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("* Statement: cursor name, table name [utf-8]        *");
		loginfo("*****************************************************");

		dropTable("ההההההה");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE ההההההה (i INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO ההההההה (i) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("ההההההה");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_Statement *stmt = connection()->createStatement();
    char name[100];
    char buffer[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

		// 3.
		loginfo("3. execute SELECT statement (ASCII)");
    tc_errorcheck(stmt)->execute("SELECT * FROM ההההההה FOR UPDATE");

		// 4.
		loginfo("4. set cursor name to non ASCII-127 name");
    tc_errorcheck(stmt)->setCursorName("צצצצצצצצצצ", SQLDBC_NTS, SQLDBC_StringEncodingAscii);

    // 5.
		loginfo("5. getCursorName (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(stmt)->getCursorName(name, SQLDBC_StringEncodingUTF8, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 6.
		loginfo("6. getCursorName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getCursorName(buffer, SQLDBC_StringEncodingUTF8, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 7.
		loginfo("7. getCursorName UTF8 (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;

    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off the last character: 2 bytes
    // so the new buffer length must be:
    bufSize = nameLen-1;

    tc_errorcheckrc(rc, stmt)->getCursorName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);

    // 8.
    loginfo("8. getCursorName UTF8 (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;

    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off half of the last character: 1 byte
    // so the new buffer length must be:
    bufSize = nameLen;

    tc_errorcheckrc(rc, stmt)->getCursorName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-2)) {
      logerror("getCursorName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] == '\0')
        logerror("Zero-termination was set at the wrong place: string has not been truncated correctly.");
      else
        if (buffer[bufSize-2] != '\0')
          logerror("Zero-termination was not set");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getCursorName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);

		// 9.
		loginfo("9. getTableName UTF8 (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    tc_errorcheck(stmt)->getTableName(name, SQLDBC_StringEncodingUTF8, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("table name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 10.
		loginfo("10. getTableName UTF8 (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    tc_errorcheckrc(rc, stmt)->getTableName(buffer, SQLDBC_StringEncodingUTF8, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 11.
		loginfo("11. getTableName UTF8 (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off the last two characters: 3 bytes
    // so the new buffer length must be:
    bufSize = nameLen-2;

    tc_errorcheckrc(rc, stmt)->getTableName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    // 12.
    loginfo("12. getTableName UTF8 (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off the last one and a half characters: 2 bytes
    // so the new buffer length must be:
    bufSize = nameLen - 1;

    tc_errorcheckrc(rc, stmt)->getTableName(buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-2)) {
      logerror("getTableName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] == '\0')
        logerror("Zero-termination was set at the wrong place: string has not been truncated correctly.");
      else
        if (buffer[bufSize-2] != '\0')
          logerror("Zero-termination was not set");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getTableName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(stmt);

		return m_success;
	}
};



/* -----   ParameterMetaDataAscii   -----
 *
 * check getParameterName
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. prepare INSERT statement (ASCII), retrieve ParameterMetaData
 * 4. getParameterName ASCII (buffer size big enough [100]), store name
 * 5. getParameterName ASCII (buffer size = 0)
 * 6. getParameterName ASCII (buffer size too small)
 *
 */
class ParameterMetaDataAscii : public SQLDBC_TestCase
{

public:
	ParameterMetaDataAscii ()
		: SQLDBC_TestCase ("ParameterMetaDataAscii")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("* ParameterMetaData: parameter name [ascii]         *");
		loginfo("*****************************************************");

		dropTable("tableParameterMetaDataAscii");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tableParameterMetaDataAscii (int_column INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tableParameterMetaDataAscii (int_column) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableParameterMetaDataAscii");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();
    char name[100];
    char buffer[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

		// 3.
		loginfo("3. prepare INSERT statement (ASCII), retrieve ParameterMetaData");
    tc_errorcheck(pstmt)->prepare("INSERT INTO tableParameterMetaDataAscii (int_column) VALUES (?)");
    SQLDBC_ParameterMetaData *md = tc_errorcheck(pstmt)->getParameterMetaData();
    if (!md) {
      logerror("no meta data was returned. test case must exit");
      return m_success;
    }

		// 4.
		loginfo("4. getParameterName ASCII (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    md->getParameterName(1, name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("parameter name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getCursorName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    rc = md->getParameterName(1, buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getParameterName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getCursorName (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    rc = md->getParameterName(1, buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getParameterName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getParameterName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(pstmt);

		return m_success;
	}
};



/* -----   ResultSetMetaDataAscii   -----
 *
 * check getColumnName
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. execute SELECT statement (ASCII), get result set, retrieve ResultSetMetaData
 * 4. getColumnName ASCII (buffer size big enough [100]), store name
 * 5. getColumnName ASCII (buffer size = 0)
 * 6. getColumnName ASCII (buffer size too small)
 *
 */
class ResultSetMetaDataAscii : public SQLDBC_TestCase
{

public:
	ResultSetMetaDataAscii ()
		: SQLDBC_TestCase ("ResultSetMetaDataAscii")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("* ResultSetMetaData: column name [ascii]            *");
		loginfo("*****************************************************");

		dropTable("tableResultSetMetaDataAscii");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tableResultSetMetaDataAscii (int_column INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tableResultSetMetaDataAscii (int_column) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableResultSetMetaDataAscii");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_Statement *stmt = connection()->createStatement();
    char name[100];
    char buffer[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

		// 3.
		loginfo("3. execute SELECT statement (ASCII), get result set, retrieve ResultSetMetaData");
    tc_errorcheck(stmt)->execute("SELECT * FROM tableResultSetMetaDataAscii");
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    if (!rs) {
      logerror("no result set was returned. test case must exit");
      return m_success;
    }
    SQLDBC_ResultSetMetaData *md = tc_errorcheck(rs)->getResultSetMetaData();
    if (!md) {
      logerror("no meta data was returned. test case must exit");
      return m_success;
    }

		// 4.
		loginfo("4. getColumnName ASCII (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    md->getColumnName(1, name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("column name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getColumnName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    rc = md->getColumnName(1, buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getColumnName (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    rc = md->getColumnName(1, buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(stmt);

		return m_success;
	}
};


/* -----   ResultSetMetaDataUnicode   -----
 *
 * check getColumnName (Unicode)
 *
 * precondition for this test case: UNICODE database
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. execute SELECT statement (UNICODE), get result set, retrieve ResultSetMetaData
 * 4. getColumnName ASCII (buffer size big enough [100]), store name
 * 5. getColumnName ASCII (buffer size = 0)
 * 6. getColumnName ASCII (buffer size too small)
 * 7. getColumnName UNICODE (buffer size big enough [100]), store name
 * 8. getColumnName UNICODE (buffer size = 0)
 * 9. getColumnName UNICODE (buffer size too small - exactly fitting for truncation)
 * 9b. getColumnName UNICODE (buffer size too small - not fitting for truncation)
 *
 * 10. execute SELECT statement (ASCII), get result set, retrieve ResultSetMetaData
 * 11. getColumnName UNICODE (buffer size big enough [100]), store name
 * 12. getColumnName UNICODE (buffer size = 0)
 * 13. getColumnName UNICODE (buffer size too small - exactly fitting for truncation)
 * 13b. getColumnName UNICODE (buffer size too small - not fitting for truncation)
 *
 */
class ResultSetMetaDataUnicode : public SQLDBC_TestCase
{

public:
	ResultSetMetaDataUnicode ()
		: SQLDBC_TestCase ("ResultSetMetaDataUnicode")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*******************************************************");
    loginfo("* ResultSetMetaData: column name [unicode]            *");
		loginfo("*******************************************************");

    if (!connection()->isUnicodeDatabase()) {
      loginfo("We do not have a unicode database...");
      loginfo("We can't perform these tests...");
      return m_success;
    }

		dropTable("tableResultSetMetaDataUnicode");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tableResultSetMetaDataUnicode (int_column INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tableResultSetMetaDataUnicode (int_column) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableResultSetMetaDataUnicode");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
    // check precondition
    if (!connection()->isUnicodeDatabase())
      return m_success;

		SQLDBC_Statement *stmt = connection()->createStatement();
    char name[100];
    UCS2CHAR name_u[100];
    char buffer[100];
    UCS2CHAR buffer_u[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

    char asciiSQL[100];
    UCS2CHAR unicodeSQL[100];

		// 3.
		loginfo("3. execute SELECT statement (UNICODE), get result set, retrieve ResultSetMetaData");
    sprintf(asciiSQL, "SELECT * FROM tableResultSetMetaDataUnicode");
    convertToUCS2(unicodeSQL, sizeof(unicodeSQL), false, asciiSQL);
    tc_errorcheck(stmt)->execute((char *)unicodeSQL, SQLDBC_StringEncodingUCS2);
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    if (!rs) {
      logerror("no result set was returned. test case must exit");
      return m_success;
    }
    SQLDBC_ResultSetMetaData *md = tc_errorcheck(rs)->getResultSetMetaData();
    if (!md) {
      logerror("no meta data was returned. test case must exit");
      return m_success;
    }

		// 4.
		loginfo("4. getColumnName ASCII (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    md->getColumnName(1, name, SQLDBC_StringEncodingAscii, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getColumnName ASCII (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    rc = md->getColumnName(1, buffer, SQLDBC_StringEncodingAscii, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getColumnName ASCII (buffer size too small)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;
    bufSize = nameLen;

    rc = md->getColumnName(1, buffer, SQLDBC_StringEncodingAscii, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 7.
		loginfo("7. getColumnName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    md->getColumnName(1, (char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlenUCS2(name_u))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 8.
		loginfo("8. getColumnName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    rc = md->getColumnName(1, (char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9.
		loginfo("9. getColumnName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    rc = md->getColumnName(1, (char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 9b.
		loginfo("9b. getColumnName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    rc = md->getColumnName(1, (char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 10.
		loginfo("10. execute SELECT statement (ASCII), get result set, retrieve ResultSetMetaData");
    tc_errorcheck(stmt)->execute("SELECT * FROM tableResultSetMetaDataUnicode");
    rs = tc_errorcheck(stmt)->getResultSet();
    if (!rs) {
      logerror("no result set was returned. test case must exit");
      return m_success;
    }
    md = tc_errorcheck(rs)->getResultSetMetaData();
    if (!md) {
      logerror("no meta data was returned. test case must exit");
      return m_success;
    }


		// 11.
		loginfo("11. getColumnName UNICODE (buffer size big enough [100]), store name");
    memset(name_u, '\0', sizeof(name_u));
    nameLen = -99;
    md->getColumnName(1, (char *)name_u, SQLDBC_StringEncodingUCS2, sizeof(name_u), &nameLen);
    convertUCS2ToASCII(name, sizeof(name), false, name_u, sizeof(name_u));
    if (nameLen >= sizeof(name_u)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name_u));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlenUCS2(name_u))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlenUCS2(name_u));


		// 12.
		loginfo("12. getColumnName UNICODE (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    convertToUCS2(buffer_u, sizeof(buffer_u), false, buffer);
    bufLen = -97;

    rc = md->getColumnName(1, (char *)buffer_u, SQLDBC_StringEncodingUCS2, 0, &bufLen);
    // check if buffer is untouched
    if (buffer_u[0] != 0x7900) // 'y' as unicode
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 13.
		loginfo("13. getColumnName UNICODE (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen;

    rc = md->getColumnName(1, (char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-2)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 13b.
		loginfo("13b. getColumnName UNICODE (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer_u, 'z', sizeof(buffer)-1);
    buffer_u[sizeof(buffer)/2-1] = 0x0000;
    bufLen = -96;
    bufSize = nameLen+1;

    rc = md->getColumnName(1, (char *)buffer_u, SQLDBC_StringEncodingUCS2, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer_u, name_u, bufSize-3)) {
      convertUCS2ToASCII(buffer, sizeof(buffer), false, buffer_u, sizeof(buffer_u));
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer_u[bufSize / 2 - 1] != 0x0000)
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(stmt);

		return m_success;
	}
};



/* -----   ResultSetMetaDataUTF8   -----
 *
 * check getColumnName (UTF-8)
 *
 * 1. create table with one column
 * 2. insert one row
 *
 * 3. execute SELECT statement (ASCII), get result set, retrieve ResultSetMetaData
 * 4. getColumnName UTF8 (buffer size big enough [100]), store name
 * 5. getColumnName UTF8 (buffer size = 0)
 * 6. getColumnName UTF8 (buffer size too small - exactly fitting for truncation)
 * 7. getColumnName UTF8 (buffer size too small - not fitting for truncation)
 *
 */
class ResultSetMetaDataUTF8 : public SQLDBC_TestCase
{

public:
	ResultSetMetaDataUTF8 ()
		: SQLDBC_TestCase ("ResultSetMetaDataUTF8")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		loginfo("*****************************************************");
    loginfo("* ResultSetMetaData: column name [utf-8]            *");
		loginfo("*****************************************************");

		dropTable("tableResultSetMetaDataUTF8");

		// 1.
		loginfo("1. create table with one column");
		execSQL("CREATE TABLE tableResultSetMetaDataUTF8 (הההההההההה INTEGER)");

		// 2.
		loginfo("2. insert one row");
		execSQL("INSERT INTO tableResultSetMetaDataUTF8 (הההההההההה) VALUES (1)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("tableResultSetMetaDataUTF8");

		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_Statement *stmt = connection()->createStatement();
    char name[100];
    char buffer[100];
    SQLDBC_Length bufLen, nameLen, bufSize;
    SQLDBC_Retcode rc;

		// 3.
		loginfo("3. execute SELECT statement (ASCII), get result set, retrieve ResultSetMetaData");
    tc_errorcheck(stmt)->execute("SELECT * FROM tableResultSetMetaDataUTF8 FOR UPDATE");
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    if (!rs) {
      logerror("no result set was returned. test case must exit");
      return m_success;
    }
    SQLDBC_ResultSetMetaData *md = tc_errorcheck(rs)->getResultSetMetaData();
    if (!md) {
      logerror("no meta data was returned. test case must exit");
      return m_success;
    }

		// 4.
		loginfo("4. getColumnName (buffer size big enough [100]), store name");
    memset(name, '\0', sizeof(name));
    nameLen = -99;
    md->getColumnName(1, name, SQLDBC_StringEncodingUTF8, sizeof(name), &nameLen);
    if (nameLen >= sizeof(name)) {
      logerror("cursor name is too big... This test case only allows name shorter than %d characters", sizeof(name));
      return m_success;
    }
    if (nameLen == -99)
      logerror("bufferLength was not set");
    else
      if (nameLen != strlen(name))
        logerror("bufferLength was set to %d. The zero-termination is at position %d", nameLen, strlen(name));


		// 5.
		loginfo("5. getColumnName (buffer size = 0)");
    // init buffer to 'yyy...'
    memset(buffer, 'y', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -97;

    rc = md->getColumnName(1, buffer, SQLDBC_StringEncodingUTF8, 0, &bufLen);
    // check if buffer is untouched
    if (buffer[0] != 'y')
      logerror("Buffer was touched although buffer size was set to 0");
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


		// 6.
		loginfo("6. getColumnName UTF8 (buffer size too small - exactly fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;

    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off the last character: 2 bytes
    // so the new buffer length must be:
    bufSize = nameLen-1;

    rc = md->getColumnName(1, buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-1)) {
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] != '\0')
        logerror("Zero-termination was not set.");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);

    // 7.
    loginfo("7. getColumnName UTF8 (buffer size too small - not fitting for truncation)");
    // init buffer to 'zzz...'
    memset(buffer, 'z', sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';
    bufLen = -96;

    // calculate new buffer length:
    // the utf-8 string has (nameLen + 1) bytes
    // let us now cut off half of the last character: 1 byte
    // so the new buffer length must be:
    bufSize = nameLen;

    rc = md->getColumnName(1, buffer, SQLDBC_StringEncodingUTF8, bufSize, &bufLen);
    // check if the right stuff is inserted into the buffer
    if (memcmp(buffer, name, bufSize-2)) {
      logerror("getColumnName returned %s. Expected was %s", buffer, name);
    } else {
      if (buffer[bufSize-1] == '\0')
        logerror("Zero-termination was set at the wrong place: string has not been truncated correctly.");
      else
        if (buffer[bufSize-2] != '\0')
          logerror("Zero-termination was not set");
    }
    if (bufLen != nameLen)
      logerror("bufferLength was set to %d. Expected was %d", bufLen, nameLen);
    if (rc != SQLDBC_DATA_TRUNC)
      logerror("getColumnName returned Retcode %d. Expected was %d", rc, SQLDBC_DATA_TRUNC);


    connection()->releaseStatement(stmt);

		return m_success;
	}
};



int main (int argc, char **argv)
{
	// Initalize from command line params
	SQLDBC_TestFrame testframe (argc, argv);

	testframe.addTestCase (new PreparedStatementAscii());
	testframe.addTestCase (new PreparedStatementUnicode());
	testframe.addTestCase (new PreparedStatementUTF8());

	testframe.addTestCase (new StatementAscii());
	testframe.addTestCase (new StatementUnicode());
	testframe.addTestCase (new StatementUTF8());

//	testframe.addTestCase (new ParameterMetaDataAscii());  // skip it because we cannot get the parameter name

	testframe.addTestCase (new ResultSetMetaDataAscii());
	testframe.addTestCase (new ResultSetMetaDataUnicode());
	testframe.addTestCase (new ResultSetMetaDataUTF8());

	if (testframe.run ()) 
		return 0;

	return 1;
}


