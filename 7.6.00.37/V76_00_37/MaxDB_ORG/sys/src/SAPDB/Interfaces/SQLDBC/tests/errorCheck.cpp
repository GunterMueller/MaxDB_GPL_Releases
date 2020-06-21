/*!
  @file         errorCheck.cpp
  @author       D042427
  @ingroup      Tests
  @brief        checks all methods about setting error codes and error texts, etc.
  @keywords     error, errorcode, errortext
  @assumptions	there are some strings defined below. These should not occur within the tested data base instance
  @todo			ResultSet:			Done [State: "Session down" not tested. What methods could be involved?]
  @bugs			http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126252
  @remarks		There are a lot of methods which cannot be forced to fail. These methods are only tested with positive tests

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
/*
		4.	Errorcheck:
		4.1		Produce an error and check if errorcode, sqlcode and sqlstate are set.
		4.2		Check if the errorobject is reset after a successful call (e.g. connect() fail, connect() successful ==> errorobject?)
		4.3		Produce an error and continue with other calls (assuming the error not having happened)

		4.1.	Produzieren eines Fehlers und überprüfen ob errorcode, sqlcode, sqlstate messagetext gesetzt wurde.
				Test ob das errorobject nach Aufruf eines erfolgreichen calls zurückgesetzt wurde.
				z.B. gescheitertes Connect erfolgreiches Connect.
				Dies für alle Funktionen, die einen SQLDBC_Retcode liefern oder dies in dem Returnwert signalisieren (z.B. einen NULL Pointer für nicht vorhandenen Wert der Abfrage).
		4.2.	Produzieren eines Fehlers, ignorieren dieses und Aufruf einer weiteren Funktion, die den Erfolg der vorherigen Funktion voraussetzt.
				z.B gescheitertes connect danach createStatement oder execute.

*/
#define DIRTYHACK
#ifdef DIRTYHACK
  #define private public
  #include "SAPDB/Interfaces/Runtime/IFR_ErrorHndl.h"
#endif

#include <stdarg.h>
#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/SAPDBCommon/SAPDB_sprintf.h"
#include <string.h>

#define INVALIDHOSTNAME		"GerosTestHost987654321"
#define INVALIDDATABASE		"GerosTestDB987654321"
#define INVALIDUSERNAME		"GerosTestUser987654321"
#define INVALIDPASSWORD		"GerosTestPassword987654321"

#define DUMMYERRORCODE   99
#define DUMMYERRORTEXT   "'This text should have been erased'"


class SQLDBC_ErrorCheckTestCase : public SQLDBC_TestCase
{
protected:
	// checks if the specified error object delivers the expected error code and if there is a error text
	void checkErrorObject(const char *funcName, int line,
							SQLDBC_ErrorHndl *errHndl,
							SQLDBC_Int4 expectedErrorCode, SQLDBC_Bool ErrorTextExpected
							)
	{
		// check getErrorCode()
		if (errHndl->getErrorCode() != expectedErrorCode)
			logerror("%s in line %d. getErrorCode() returned %d. Expected was %d", 
				funcName, line, errHndl->getErrorCode(), expectedErrorCode);

		// getSQLState() cannot be checked properly. Thus skip it

		// check getErrorText()
		if ((strlen(errHndl->getErrorText()) > 0) != ErrorTextExpected)
		{
			if (ErrorTextExpected)
				logerror("%s in line %d. getErrorText() returned an empty string", funcName, line);
			else
				logerror("%s in line %d. getErrorText() was expected to return an empty string. It actually returned '%s'", 
					funcName, line, errHndl->getErrorText());
		}
	}							

	void checkReturnValues(const char *funcName, int line,
							SQLDBC_Retcode rc, SQLDBC_Retcode expectedRetcode,
							SQLDBC_ErrorHndl *errHndl,
							SQLDBC_Int4 expectedErrorCode, SQLDBC_Bool ErrorTextExpected
							)
	{
		// check the Retcode
		if (rc != expectedRetcode)
			logerror("%s in line %d. Retcode %d. Expected was %d.", funcName, line, rc, expectedRetcode);

		checkErrorObject(funcName, line, errHndl, expectedErrorCode, ErrorTextExpected);
	}

public:
  SQLDBC_ErrorCheckTestCase (const char *TestCaseName)
    : SQLDBC_TestCase (TestCaseName)
  {
  }

  void clearError (SQLDBC_ConnectionItem *connItem)
	{
		connItem->clearError();
	}

	void createSpecificError (SQLDBC_ConnectionItem *connItem, SQLDBC_Int4 ErrorCode, const char *ErrorText)
	{
#ifdef DIRTYHACK
		IFR_ErrorHndl *err = connItem->error().m_error;

		// set new ErrorCode
		err->m_errorcode = ErrorCode;

		// set new ErrorText
		// handle ugly C-strings...
		if (err->m_message != NULL)
			err->m_allocator->Deallocate(err->m_message);
		err->m_message=(char*) err->m_allocator->Allocate(strlen(ErrorText)+1);
		memcpy(err->m_message, ErrorText, strlen(ErrorText)+1);
#else
		// Cannot create error
		loginfo("Cannot set error code in error object. Test might deliver unexpected results.");
#endif
	}

	void createError(SQLDBC_ConnectionItem *connItem)
	{
		createSpecificError(connItem, DUMMYERRORCODE, DUMMYERRORTEXT);
	}

};



/**
 * Template class for my tests
 */
template <class C>
class SQLDBC_myExpectedErrorCheckClass
//    : public SQLDBC_ErrorCheckClass<C>
{
public:
    /**
     * Constructor.
     * @param ptr pointer to object to be checked (must have <code>error()</code> method.
     * @param tc current test case
     * @param expectederrorcode the error code that is exepected.
     * @param file the file name (use <code>__FILE__</code>)
     * @param line the line number (use <code>__LINE__</code>)
     */
    inline SQLDBC_myExpectedErrorCheckClass(C *ptr, SQLDBC_ErrorCheckTestCase* tc, int expectederrorcode, bool errortextexpected, SQLDBC_Retcode *rc=0, const char *file=0, int line=0)
    {
        m_ptr=ptr;
        m_tc=tc;
        m_file=file;
        m_line=line;
        m_rcptr = rc;
        m_expectederrorcode = expectederrorcode;
        m_errortextexpected = errortextexpected;

		if (m_expectederrorcode)
		{
			// clear error if an error is expected
			m_tc->clearError(this->m_ptr);
		} else {
			// create an error if no error is expected
			m_tc->createError(this->m_ptr);
		}
	}


    /**
     * Copy constructor. We need this as only the last
     * instance in a 'copy chain' should actually barf if
     * the error is not set.
     * @param copy instance to copy. In this instance the error check
     *        will be disabled.
     */
    inline SQLDBC_myExpectedErrorCheckClass(const SQLDBC_myExpectedErrorCheckClass& copy)
    :m_ptr(copy.m_ptr),
     m_tc(copy.m_tc),
     m_file(copy.m_file),
     m_line(copy.m_line),
     m_rcptr(copy.m_rcptr),
     m_expectederrorcode(copy.m_expectederrorcode),
     m_errortextexpected(copy.m_errortextexpected)
    {}
    
    inline ~SQLDBC_myExpectedErrorCheckClass()
    {
				// check error code and error text
                if((this->m_ptr->error().getErrorCode() != m_expectederrorcode) || ((strlen(this->m_ptr->error().getErrorText()) > 0) != m_errortextexpected)) {
                    if(this->m_file==0) {
                        this->m_tc->logerror("Unexpected error code or text: %d sql state %5s, %s, expected was %d.", 
                                       this->m_ptr->error().getErrorCode(),
                                       this->m_ptr->error().getSQLState(),
                                       this->m_ptr->error().getErrorText(),
                                       m_expectederrorcode);
                    } else {
                        this->m_tc->logerror("Unexpected Error: %d sql state %5s, %s in file %s:%d, expected was %d.", 
                                       this->m_ptr->error().getErrorCode(),
                                       this->m_ptr->error().getSQLState(),
                                       this->m_ptr->error().getErrorText(),
                                       this->m_file,
                                       this->m_line,
                                       m_expectederrorcode);
                    }
                }
    }

	inline C* operator->()
    {
        return m_ptr;
    }

    inline const C* operator->() const
    {
        return m_ptr;
    }
    
    inline C& operator *()
    {
        return *m_ptr;
    }

    inline const C& operator *() const
    {
        return *m_ptr;
    }
        
private:
    C*            m_ptr;
    SQLDBC_ErrorCheckTestCase* m_tc;

    const char *  m_file;
    int           m_line;
    SQLDBC_Retcode *m_rcptr;

	int       m_expectederrorcode;
	bool      m_errortextexpected;
    SQLDBC_Bool  m_checkdisabled;
};

template <class C>
inline SQLDBC_myExpectedErrorCheckClass<C>
SQLDBC_myexpectederrorcheck(C* ptr, SQLDBC_ErrorCheckTestCase *tc, int expectederrorcode, bool errortextexpected,
                       SQLDBC_Retcode *rc, const char *file, const int line)
{
    return SQLDBC_myExpectedErrorCheckClass<C>(ptr, tc, expectederrorcode, errortextexpected, rc, file, line);
}

#define checknoerr(x) SQLDBC_myexpectederrorcheck(x, this, 0, false, 0, __FILE__, __LINE__)

#define checkerr(x, expErrorCode) SQLDBC_myexpectederrorcheck(x, this, expErrorCode, true, 0, __FILE__, __LINE__)

#define checknoerrrc(rc, x) SQLDBC_myexpectederrorcheck(x, this, 0, false, &rc, __FILE__, __LINE__),rc=x

#define checkerrrc(rc, x, expErrorCode) SQLDBC_myexpectederrorcheck(x, this, expErrorCode, true, &rc, __FILE__, __LINE__),rc=x




class ResultSet : public SQLDBC_ErrorCheckTestCase
{
public:
  ResultSet ()
    : SQLDBC_ErrorCheckTestCase ("ResultSet")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
	SQLDBC_Retcode rc;
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does not return a valid object\n");
      return 0;
    }

    tc_errorcheck(conn)->connect(hostname(), 
                                 database(), 
                                 username(),
                                 password());

    SQLDBC_Statement *stmt = conn->createStatement();

    SQLDBC_ResultSet *rs2 = stmt->getResultSet();
	if (rs2 != NULL)
		logerror("Line %d. getResultSet() returned result set. Expected was NULL");
    
    tc_errorcheck(stmt)->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
    tc_errorcheck(stmt)->execute("SELECT '15' FROM Dual");
    SQLDBC_ResultSet *rs1 = stmt->getResultSet();

	/*********************************************************/
	/*                                                       */
	/*   *** Current state (expected): query performed ***   */
	/*                                                       */
	/*********************************************************/
	loginfo("Do some checks having a query already performed...");

	loginfo("Check getResultSetMetaData");
	SQLDBC_ResultSetMetaData *metaData1 = checknoerr(rs1)->getResultSetMetaData();
	if (metaData1 == NULL)
		logerror("Line %d. No meta data was returned", __LINE__);

	loginfo("Check getResultCount");
	int rCount1 = checknoerr(rs1)->getResultCount();
	if (rCount1 == 0)
		logerror("Line %d. Return value 0. Expected was something else.", __LINE__);

	loginfo("Check bindColumn");
	int myInt1;
	SQLDBC_Length ind1 = SQLDBC_NTS;
	checknoerrrc(rc, rs1)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, &myInt1, &ind1, sizeof(myInt1));

	loginfo("Check clearColumns");
	checknoerrrc(rc, rs1)->clearColumns();

	loginfo("Check setRowSetSize");
	checknoerr(rs1)->setRowSetSize(10);

	loginfo("Check getRowSetSize");
	SQLDBC_UInt4 rsSize1 = checknoerr(rs1)->getRowSetSize();
	if (rsSize1 != 10)
		logerror("Line %d. Return value %d. Expected was 10.", __LINE__, rsSize1);

	loginfo("Check getRowSet");
	SQLDBC_RowSet *rowSet1 = checknoerr(rs1)->getRowSet();
	if (rowSet1 == NULL)
		logerror("Line %d. No row set was returned", __LINE__);

	loginfo("Check getUpdatableRowSet");
	SQLDBC_UpdatableRowSet *uRowSet1 = checknoerr(rs1)->getUpdatableRowSet();
	if (uRowSet1 == NULL)
		logerror("Line %d. No updatable row set was returned", __LINE__);

	loginfo("Check first");
	checknoerrrc(rc, rs1)->first();

	loginfo("Check next");
	checknoerrrc(rc, rs1)->next();

	loginfo("Check previous");
	checknoerrrc(rc, rs1)->previous();

	loginfo("Check relative");
	checknoerrrc(rc, rs1)->relative(0);

	loginfo("Check last");
	checknoerrrc(rc, rs1)->last();

	loginfo("Check absolute");
	checknoerrrc(rc, rs1)->absolute(1);

	loginfo("Check getRowNumber");
	SQLDBC_UInt4 rNumber1 = checknoerr(rs1)->getRowNumber();
	if (rNumber1 != 1)
		logerror("Line %d. Return value %d. Expected 1", __LINE__, rNumber1);

	loginfo("Check getObject");
	int myInt2;
	SQLDBC_Length ind2=SQLDBC_NTS;
	checknoerrrc(rc, rs1)->getObject(1, SQLDBC_HOSTTYPE_INT4, &myInt2, &ind2, sizeof(myInt2));

	loginfo("Check close");
	checknoerr(rs1)->close();

	/*********************************************************/
	/*                                                       */
	/*  *** Current state (expected): result set closed ***  */
	/*                                                       */
	/*********************************************************/
	loginfo("Do some checks having the result set closed...");

	loginfo("Check getResultSetMetaData");
	SQLDBC_ResultSetMetaData *metaData2 = checkerr(rs1, -10500)->getResultSetMetaData();
	if (metaData2 != NULL)
		logerror("Line %d. Meta data was returned. Expected was NULL", __LINE__);

	loginfo("Check getResultCount");
	int rCount2 = checkerr(rs1, -10500)->getResultCount();
	if (rCount2 != 0)
		logerror("Line %d. Return value %d. Expected was 0.", __LINE__, rCount2);

	loginfo("Check bindColumn");
	int myInt3;
	SQLDBC_Length ind3 = SQLDBC_NTS;
	checkerrrc(rc, rs1, -10500)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, &myInt3, &ind3, sizeof(myInt3));

	loginfo("Check clearColumns");
	checkerrrc(rc, rs1, -10500)->clearColumns();

	loginfo("Check setRowSetSize");
	checkerr(rs1, -10500)->setRowSetSize(10);

	loginfo("Check getRowSetSize");
	SQLDBC_UInt4 rsSize2 = checkerr(rs1, -10500)->getRowSetSize();
	if (rsSize2 != 0)
		logerror("Line %d. Return value %d. Expected was 0.", __LINE__, rsSize2);

	loginfo("Check getRowSet");
	SQLDBC_RowSet *rowSet2 = checkerr(rs1, -10500)->getRowSet();
	if (rowSet2 != NULL)
		logerror("Line %d. Row set was returned. Expected was NULL", __LINE__);

	loginfo("Check getUpdatableRowSet");
	SQLDBC_UpdatableRowSet *uRowSet2 = checkerr(rs1, -10500)->getUpdatableRowSet();
	if (uRowSet2 != NULL)
		logerror("Line %d. Updatable row set was returned. Expected was NULL", __LINE__);

	loginfo("Check first");
	checkerrrc(rc, rs1, -10500)->first();

	loginfo("Check next");
	checkerrrc(rc, rs1, -10500)->next();

	loginfo("Check previous");
	checkerrrc(rc, rs1, -10500)->previous();

	loginfo("Check relative");
	checkerrrc(rc, rs1, -10500)->relative(0);

	loginfo("Check last");
	checkerrrc(rc, rs1, -10500)->last();

	loginfo("Check absolute");
	checkerrrc(rc, rs1, -10500)->absolute(1);

	loginfo("Check getRowNumber");
	SQLDBC_UInt4 rNumber2 = checkerr(rs1, -10500)->getRowNumber();
	if (rNumber2 != 0)
		logerror("Line %d. Return value %d. Expected 0", __LINE__, rNumber2);

	loginfo("Check getObject");
	int myInt4;
	SQLDBC_Length ind4=SQLDBC_NTS;
	checkerrrc(rc, rs1, -10500)->getObject(1, SQLDBC_HOSTTYPE_INT4, &myInt4, &ind4, sizeof(myInt4));

	loginfo("Check close");
	checkerr(rs1, -10500)->close();

	tc_errorcheck(conn)->releaseStatement(stmt);
    env.releaseConnection(conn);

    return m_success;
  }

};

class RowSet : public SQLDBC_ErrorCheckTestCase
{
public:
  RowSet ()
    : SQLDBC_ErrorCheckTestCase ("RowSet")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Retcode rc;
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does not return a valid object\n");
      return 0;
    }

    tc_errorcheck(conn)->connect(hostname(), 
                                 database(), 
                                 username(),
                                 password());
  
    SQLDBC_Statement *stmt = conn->createStatement();

    const char *szSQL = "SELECT 'HELLO' FROM Dual";

    tc_errorcheck(stmt)->execute(szSQL);

	/* Get the result set and navigate to the beginning */
	SQLDBC_ResultSet *resultSet = stmt->getResultSet();

	/* get the row set */
    SQLDBC_RowSet *rset = resultSet->getRowSet();
    tc_errorcheck(rset);
	/*********************************************************/
	/*                                                       */
	/*   *** Current state (expected): invalid pos. in ***   */
	/*   ***              result set. before first row ***   */
	/*                                                       */
	/*********************************************************/
	loginfo("Checking RowSet with invalid position in result set...");

    loginfo("Check fetch()");
    checkerrrc(rc, rset, -10501)->fetch();  

    loginfo("Check getObject()");
    char value1[100];
    SQLDBC_Length indicator1;
    checkerrrc(rc, rset, -10501)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value1, &indicator1, sizeof(value1),SQLDBC_TRUE);  

    loginfo("Check setPos()");
    checkerrrc(rc, rset, -10501)->setPos(1);  
 
	checknoerrrc(rc, resultSet)->first();
	/*********************************************************/
	/*                                                       */
	/*   *** Current state (expected): valid RowSet ***      */
	/*                                                       */
	/*********************************************************/
	loginfo("Checking RowSet with valid position...");

    loginfo("Check clearError()");
    checknoerr(rset)->clearError();  
    
    loginfo("Check fetch()");
    checknoerrrc(rc, rset)->fetch();  

    loginfo("Check getObject()");
    char value2[100];
    SQLDBC_Length indicator2;
    checknoerrrc(rc, rset)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value2, &indicator2, sizeof(value2),SQLDBC_TRUE);  

    loginfo("Check getRowsAffected()");
    SQLDBC_Int4 rowsAffected = checknoerr(rset)->getRowsAffected();  
    loginfo("RowsAffected: %d", rowsAffected);
	if (rowsAffected != 1)
		logerror("Expected value for RowsAffected: 1");

    loginfo("Check getRowStatus()");
    const SQLDBC_Int4 *rowstatus = checknoerr(rset)->getRowStatus();

    loginfo("Check setPos()");
    checknoerrrc(rc, rset)->setPos(1);  
	checkerrrc(rc, rset, -10504)->setPos(0);
	checkerrrc(rc, rset, -10504)->setPos(2);

	// close the result set
	tc_errorcheck(resultSet)->close();
	/*********************************************************/
	/*                                                       */
	/*   *** Current state (expected): result set closed *** */
	/*                                                       */
	/*********************************************************/
	loginfo("Checking RowSet after result set has been closed...");

    loginfo("Check fetch()");
    checkerrrc(rc, rset, -10500)->fetch();  

    loginfo("Check getObject()");
    char value3[100];
    SQLDBC_Length indicator3;
    checkerrrc(rc, rset, -10500)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value3, &indicator3, sizeof(value3),SQLDBC_TRUE);  

    loginfo("Check setPos()");
    checkerrrc(rc, rset, -10500)->setPos(1);  

	// set up the row set once again
    tc_errorcheck(stmt)->execute(szSQL);
	resultSet = stmt->getResultSet();
	tc_errorcheck(resultSet)->first();
    rset = resultSet->getRowSet();

	// now disconnect
	tc_errorcheck(conn)->disconnect();
 
	/*********************************************************/
	/*                                                       */
	/*   *** Current state (expected): connection down ***   */
	/*                                                       */
	/*********************************************************/
	loginfo("Checking RowSet being disconnected...");

    loginfo("Check fetch()");
    checkerrrc(rc, rset, -10821)->fetch();  

    loginfo("Check getObject()");
    char value4[100];
    SQLDBC_Length indicator4;
    checkerrrc(rc, rset, -10821)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value4, &indicator4, sizeof(value4),SQLDBC_TRUE);  

    loginfo("Check setPos()");
    checkerrrc(rc, rset, -10821)->setPos(1);  

    return m_success;  
    }
};

class Statement : public SQLDBC_ErrorCheckTestCase
{
public:
  Statement ()
    : SQLDBC_ErrorCheckTestCase ("Statement")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
	SQLDBC_Retcode rc;
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does not return a valid object\n");
      return 0;
    }

    conn->connect(hostname(), 
                  database(), 
                  username(),
                  password());
	if (!conn->isConnected())
	{
		logerror("%s line %d: Test case is in unexpected state (not connected). Test case must exit.");
		return 0;
	}

     SQLDBC_Statement *stmt = conn->createStatement();
	if (!stmt)
	{
		logerror("createStatement returned NULL. Test case must exit.");
		return 0;
	}

	/*********************************************************/
	/*                                                       */
	/*   *** Current state (expected): CONNECTED ***         */
	/*                                                       */
	/*********************************************************/
	loginfo("Do some checks while being properly connected...");

	loginfo("Check addBatch()");
    checknoerrrc(rc, stmt)->addBatch("PING");
  
    loginfo("Check clearBatch()");
    checknoerr(stmt)->clearBatch();  

	loginfo("Check executeBatch() with empty batch list");
	checknoerrrc(rc, stmt)->executeBatch();
	if (rc != SQLDBC_NO_DATA_FOUND)
		logerror("Return code %d. Expected was SQLDBC_NO_DATA_FOUND [%d]", rc, SQLDBC_NO_DATA_FOUND);

    loginfo("Check execute(const char* sql)");
    checknoerrrc(rc, stmt)->execute("PING"); 
     
    loginfo("Check execute(");
    loginfo("            const char* sql,");
    loginfo("            const SQLDBC_StringEncoding encoding)");
    checknoerrrc(rc, stmt)->execute("PING", SQLDBC_StringEncodingAscii); 

    loginfo("Check execute(");
    loginfo("            const char* sql,");
    loginfo("            const SQLDBC_Length sqlLength,"); 
    loginfo("            const SQLDBC_StringEncoding encoding)");
    checknoerrrc(rc, stmt)->execute("PING", 4, SQLDBC_StringEncodingAscii); 

    loginfo("Check SQLDBC_Statement.executeBatch()");
    checknoerrrc(rc, stmt)->addBatch("PING");
    checknoerr(stmt)->executeBatch();  

    loginfo("Check SQLDBC_Statement.getBatchSize()");
    SQLDBC_UInt4 bs = checknoerr(stmt)->getBatchSize();  
    loginfo("BatchSize: %d", bs);
    
    loginfo("Check SQLDBC_Statement.getCursorName()");
    char cname[100]; SQLDBC_Length cLen=0;
    checknoerrrc(rc, stmt)->getCursorName(cname, SQLDBC_StringEncodingAscii ,sizeof(cname),&cLen);  
    loginfo("Cursorname: %.*s", cLen, cname);
    
    loginfo("Check SQLDBC_Statement.getMaxRows()");
    SQLDBC_UInt4 mr = checknoerr(stmt)->getMaxRows();  
    loginfo("MaxRows: %d", mr);

    loginfo("Check SQLDBC_Statement.getResultSet()");
    SQLDBC_ResultSet* rs = checknoerr(stmt)->getResultSet();  

    loginfo("Check SQLDBC_Statement.getResultSetConcurrencyType()");
    const SQLDBC::SQLDBC_Statement::ConcurrencyType ct = checknoerr(stmt)->getResultSetConcurrencyType();  
    loginfo("getResultSetConcurrencyType: %d", (int)ct);
    
    loginfo("Check SQLDBC_Statement.getResultSetType()");
    SQLDBC::SQLDBC_Statement::ResultSetType rt = checknoerr(stmt)->getResultSetType();  
    loginfo("ResultSetType: %d", (int)rt);
    
    loginfo("Check SQLDBC_Statement.getRowsAffected()");
    SQLDBC_Int4 ra = checknoerr(stmt)->getRowsAffected();  
    loginfo("RowsAffected: %d", ra);

    loginfo("Check SQLDBC_Statement.getRowStatus()");
    const SQLDBC_Int4* rStat = checknoerr(stmt)->getRowStatus();  
    
    loginfo("Check SQLDBC_Statement.getTableName()");
    char tname[100]; SQLDBC_Length tLen=0;
    checknoerrrc(rc, stmt)->getTableName(tname, SQLDBC_StringEncodingAscii ,sizeof(tname),&tLen);  
    loginfo("Cursorname: %.*s", tLen, tname);
    
    loginfo("Check SQLDBC_Statement.isQuery()");
    SQLDBC_Bool isQ = checknoerr(stmt)->isQuery();  
    loginfo("isQuery: %s", (isQ)?"TRUE":"FALSE");
    
    loginfo("Check SQLDBC_Statement.setCursorName()");
    checknoerr(stmt)->setCursorName("Homer_Simpson", sizeof("Homer_Simpson"), SQLDBC_StringEncodingAscii);  
    
    loginfo("Check SQLDBC_Statement.setMaxRows()");
    checknoerr(stmt)->setMaxRows(1);  

    loginfo("Check SQLDBC_Statement.setResultSetConcurrencyType()");
    checknoerr(stmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_UPDATABLE);  
    checknoerr(stmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_READ_ONLY);  
    
    loginfo("Check SQLDBC_Statement.setResultSetType()");
    checknoerr(stmt)->setResultSetType(SQLDBC_Statement::SCROLL_INSENSITIVE);  
    checknoerr(stmt)->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);  

	/*********************************************************/
	/*                                                       */
	/*   *** Current state (expected): NOT CONNECTED ***     */
	/*                                                       */
	/*********************************************************/
	loginfo("Do some checks while not being connected...");

    conn->releaseStatement(stmt);
    env.releaseConnection(conn);

    return m_success;
  }

};

class PreparedStatement : public SQLDBC_ErrorCheckTestCase
{
public:
  PreparedStatement ()
    : SQLDBC_ErrorCheckTestCase ("PreparedStatement")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Retcode rc;
	SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does not return a valid object\n");
      return 0;
    }

    conn->connect(hostname(), database(), username(), password());
  
    SQLDBC_PreparedStatement *pstmt = conn->createPreparedStatement();
	tc_errorcheck(pstmt);
    
	/*********************************************************/
	/*                                                       */
	/*    *** Current state (expected): connected     ***    */
	/*                          no stmt prepared yet  ***    */
	/*                                                       */
	/*********************************************************/
	loginfo("Do some checks being properly connected...");
	loginfo("No statement prepared yet...");

    loginfo("Check bindParameter()");
    SQLDBC_Int4 val1 = 1; SQLDBC_Length ind1 = 0;
    checknoerrrc(rc, pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &val1, &ind1, sizeof(val1), SQLDBC_TRUE);

    loginfo("Check execute()");
    checkerrrc(rc, pstmt, -10210)->execute(); 
     
    loginfo("Check executeBatch()");
    checkerr(pstmt, -10210)->executeBatch();  

    loginfo("Check getParameterMetaData()");
    SQLDBC_ParameterMetaData* pm1 = checknoerr(pstmt)->getParameterMetaData();  
	if (pm1)
		logerror("No meta data was expected. Actually data was returned");

    loginfo("Check getResultSet()");
    SQLDBC_ResultSet* rs1 = checknoerr(pstmt)->getResultSet();  

    loginfo("Check isQuery()");
    SQLDBC_Bool isQ1 = checknoerr(pstmt)->isQuery();  
    loginfo("isQuery: %s", (isQ1)?"TRUE":"FALSE");
    

    loginfo("Check addBatch()");
        rc = checkerr(pstmt, -10220)->addBatch("PING");



	// now prepare...
    loginfo("Check prepare()");
    SQLDBC_Int4 val2 = 1; SQLDBC_Length ind2 = 0;
    checknoerrrc(rc, pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &val2, &ind2, sizeof(val2), SQLDBC_TRUE);
	checkerrrc(rc, pstmt, -10601)->prepare(NULL);
    checknoerrrc(rc, pstmt)->prepare("SELECT * FROM DUAL WHERE 1=?");

	/*********************************************************/
	/*                                                       */
	/*    *** Current state (expected): connected     ***    */
	/*                         stmt properly prepared ***    */
	/*                                                       */
	/*********************************************************/
	loginfo("Do some checks having a statement properly prepared...");

    loginfo("Check bindParameter()");
    SQLDBC_Int4 val = 1; SQLDBC_Length ind = 0;
	checkerrrc(rc, pstmt, -10300)->bindParameter(0, SQLDBC_HOSTTYPE_INT4, &val, &ind, sizeof(val), SQLDBC_TRUE);
    checknoerrrc(rc, pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &val, &ind, sizeof(val), SQLDBC_TRUE);
	// right here it is sufficient to check only one host type, since other tests cover all of them...

    loginfo("Check bindParameterAddr()");
    SQLDBC_Int4 * valP = &val;
	checkerr(pstmt, -10300)->bindParameterAddr(0, SQLDBC_HOSTTYPE_INT4, &valP, &ind, sizeof(val), SQLDBC_TRUE);  
    checknoerr(pstmt)->bindParameterAddr(1, SQLDBC_HOSTTYPE_INT4, &valP, &ind, sizeof(val), SQLDBC_TRUE);  

	// -----------------------------------------------------------------
	// these two batch operations should not be allowed for prepared statements
	// the other three batch operations are needed
    loginfo("Check addBatch()");
    checkerrrc(rc, pstmt, -10220)->addBatch("PING");
  
    loginfo("Check clearBatch()");
    checkerr(pstmt, -10220)->clearBatch();  
	// -----------------------------------------------------------------

	loginfo("Check executeBatch()");
    checknoerrrc(rc, pstmt)->executeBatch();
	tc_errorcheck(pstmt)->clearParameters();
    checkerrrc(rc, pstmt, -10304)->executeBatch(); // prepared, but column not bound

    loginfo("Check setBatchSize()");
    checkerrrc(rc, pstmt, -10856)->setBatchSize(0);
    checknoerrrc(rc, pstmt)->setBatchSize(10);

    loginfo("Check getBatchSize()");
    SQLDBC_UInt4 bs = checknoerr(pstmt)->getBatchSize();  
    loginfo("BatchSize: %d", bs);
    
    loginfo("Check clearError()");
    checknoerr(pstmt)->clearError();  

    loginfo("Check clearParameters()");
    checknoerr(pstmt)->clearParameters();  

    loginfo("Check execute()");
    checknoerr(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &val, &ind, sizeof(val), SQLDBC_TRUE);  
	checkerr(pstmt, -10603)->execute(); // SQL command would generate a result set (batch size = 10)
	pstmt->clearError();
	tc_errorcheck(pstmt)->clearParameters();
	tc_errorcheck(pstmt)->setBatchSize(1);
	checkerr(pstmt, -10304)->execute(); // prepared, but column not bound
    checknoerr(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &val, &ind, sizeof(val), SQLDBC_TRUE);  
    checknoerrrc(rc, pstmt)->execute(); // finally it should succeed
     
    loginfo("Check getCursorName()");
    char cname[100]; SQLDBC_Length cLen=0;
    checknoerrrc(rc, pstmt)->getCursorName(cname, SQLDBC_StringEncodingAscii ,sizeof(cname),&cLen);  
    loginfo("Cursorname: %.*s", cLen, cname);
    
    loginfo("Check getMaxRows()");
    SQLDBC_UInt4 mr = checknoerr(pstmt)->getMaxRows();  
    loginfo("MaxRows: %d", mr);

    loginfo("Check getParameterMetaData()");
    SQLDBC_ParameterMetaData* pm = checknoerr(pstmt)->getParameterMetaData();  
	if (!pm)
		logerror("Meta data was expected. Actually no data was returned");

    loginfo("Check getResultSet()");
    SQLDBC_ResultSet* rs = checknoerr(pstmt)->getResultSet();  

    loginfo("Check getResultSetConcurrencyType()");
    const SQLDBC::SQLDBC_Statement::ConcurrencyType ct = checknoerr(pstmt)->getResultSetConcurrencyType();  
    loginfo("getResultSetConcurrencyType: %d", (int)ct);
    
    loginfo("Check getResultSetType()");
    SQLDBC::SQLDBC_Statement::ResultSetType rt = checknoerr(pstmt)->getResultSetType();  
    loginfo("ResultSetType: %d", (int)rt);
    
    loginfo("Check getRowsAffected()");
    SQLDBC_Int4 ra = checknoerr(pstmt)->getRowsAffected();  
    loginfo("RowsAffected: %d", ra);

    loginfo("Check getRowStatus()");
    const SQLDBC_Int4* rStat = checknoerr(pstmt)->getRowStatus();  
    
    loginfo("Check getTableName()");
    char tname[100]; SQLDBC_Length tLen=0;
    rc = checknoerr(pstmt)->getTableName(tname, SQLDBC_StringEncodingAscii ,sizeof(tname),&tLen);  
    loginfo("Table name: %.*s", tLen, tname);
    
    loginfo("Check isQuery()");
    SQLDBC_Bool isQ = checknoerr(pstmt)->isQuery();  
    loginfo("isQuery: %s", (isQ)?"TRUE":"FALSE");
    

	// In the following two cases no data is expected since no "data at execute" was defined
	// both methods should return with an error message
    loginfo("Check nextParameter()");
    SQLDBC_Int2 index; void* addr;
    rc = tc_expectederrorcheck(pstmt, -10201)->nextParameter(index, addr);  
    
    loginfo("Check SQLDBC_PreparedStatement.putData()");
    const char* putDataVal = "Homer Simpson"; SQLDBC_Length putDataLen= sizeof ("Homer Simpson");
    rc = tc_expectederrorcheck(pstmt, -10201)->putData((void*)putDataVal, &putDataLen);  


    loginfo("Check prepare()");
    rc = checknoerr(pstmt)->prepare("SELECT * FROM DUAL");
    rc = checknoerr(pstmt)->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
    rc = checknoerr(pstmt)->prepare("SELECT * FROM DUAL", sizeof("SELECT * FROM DUAL") ,SQLDBC_StringEncodingAscii);

    loginfo("Check setBindingType()");
    checknoerrrc(rc, pstmt)->setBindingType(0);

    loginfo("Check setCursorName()");
    checknoerr(pstmt)->setCursorName("Homer_Simpson", sizeof("Homer_Simpson"), SQLDBC_StringEncodingAscii);  
    
    loginfo("Check setMaxRows()");
    checknoerr(pstmt)->setMaxRows(1);  

    loginfo("Check setResultSetConcurrencyType()");
    checknoerr(pstmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_UPDATABLE);  
    checknoerr(pstmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_READ_ONLY);  
    
    loginfo("Check setResultSetType()");
    checknoerr(pstmt)->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);  
    checknoerr(pstmt)->setResultSetType(SQLDBC_Statement::SCROLL_INSENSITIVE);  

	// now disconnect
	tc_errorcheck(conn)->disconnect();
	/*********************************************************/
	/*                                                       */
	/*    *** Current state (expected): disconnected  ***    */
	/*                                                       */
	/*********************************************************/
	loginfo("Do some checks being disconnected...");

	// The only method which should show different behaviour when being disconnected is execute()
    loginfo("Check execute()");
    checkerrrc(rc, pstmt, -10821)->execute(); 

    env.releaseConnection(conn);

    return m_success;
  }

};

class Connection : public SQLDBC_ErrorCheckTestCase
{
public:
  Connection ()
    : SQLDBC_ErrorCheckTestCase ("Connection")
  {}

  virtual SQLDBC_Bool run ()
  {
      SQLDBC_Environment env(runtime());

      // let createConnection fail: but how?
      SQLDBC_Connection *conn = env.createConnection();
      if (!conn) {
        logerror("createConnection does not return a valid object\n");
        return 0;
      }


      SQLDBC_Retcode rc;

	  // --- connect() ---
	  loginfo("Check connect()...");

	  // let connect fail
	  loginfo("Check connect() with invalid host name");
      checkerrrc(rc, conn, -10709)->connect(INVALIDHOSTNAME, database(), username(), password());
	  
	  loginfo("Check connect() with invalid data base name");
      checkerrrc(rc, conn, -10709)->connect(hostname(), INVALIDDATABASE, username(), password());
	  
	  loginfo("Check connect() with NULL as data base name");
      checkerrrc(rc, conn, -10857)->connect(hostname(), NULL, username(), password());
	  
	  loginfo("Check connect() with invalid user name");
	  checkerrrc(rc, conn, -4008)->connect(hostname(), database(), INVALIDUSERNAME, password());

	  loginfo("Check connect() with NULL as user name");
	  checkerrrc(rc, conn, -10715)->connect(hostname(), database(), NULL, password());

	  loginfo("Check connect() with invalid password");
	  checkerrrc(rc, conn, -4008)->connect(hostname(), database(), username(), INVALIDPASSWORD);

	  loginfo("Check connect() with NULL as password");
	  checkerrrc(rc, conn, -10715)->connect(hostname(), database(), username(), NULL);

	  /*********************************************************/
	  /*                                                       */
	  /*    *** Current state (expected): NOT connected ***    */
	  /*                                                       */
	  /*********************************************************/
	  loginfo("Check functions while not being connected...");

	  loginfo("Check SQLDBC_Connection.isConnected()");
	  SQLDBC_Bool ic1 = checknoerr(conn)->isConnected();
	  if (ic1)
	  {
		  logerror("%s line %d: Test case is in unexpected state (connected) or function isConnected() does not work.", __FILE__, __LINE__);
		  return 0;
	  }
    
	  loginfo("Check SQLDBC_Connection.clearError()");
	  checknoerr(conn)->clearError();

	  loginfo("Check SQLDBC_Connection.cancel()");
	  checkerrrc(rc, conn, -10821)->cancel();

	  loginfo("Check SQLDBC_Connection.commit()");
	  checkerrrc(rc, conn, -10821)->commit();

	  loginfo("Check SQLDBC_Connection.close()");
	  checkerrrc(rc, conn, -10821)->close();

	  loginfo("Check SQLDBC_Connection.rollback()");
	  checkerrrc(rc, conn, -10821)->rollback();

	  loginfo("Check SQLDBC_Connection.disconnect()");
	  checkerrrc(rc, conn, -10821)->disconnect();



	  // let connect finally succeed
	  loginfo("Finally establish a connection");
      checknoerrrc(rc, conn)->connect(hostname(), database(), username(), password());

	  // does it succeed a second time?
//	  loginfo("Reconnect");
//      checknoerrrc(rc, conn)->connect(hostname(), database(), username(), password());
	  // bug to be fixed: http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126252
	  

	  /*********************************************************/
	  /*                                                       */
	  /*    *** Current state (expected): CONNECTED ***        */
	  /*                                                       */
	  /*********************************************************/
	  loginfo("Check functions while being connected...");

	  loginfo("Check SQLDBC_Connection.isConnected()");
	  SQLDBC_Bool ic2 = checknoerr(conn)->isConnected();
	  if (!ic2)
	  {
		  logerror("%s line %d: Test case is in unexpected state (not connected) or function isConnected() does not work.");
		  return 0;
	  }
    
	  loginfo("Check SQLDBC_Connection.cancel()");
	  checknoerrrc(rc, conn)->cancel();

	  loginfo("Check SQLDBC_Connection.commit()");
	  checknoerrrc(rc, conn)->commit();

	  loginfo("Check SQLDBC_Connection.rollback()");
	  checknoerrrc(rc, conn)->rollback();

	  loginfo("Check SQLDBC_Connection.commitWorkRelease()");
	  checknoerr(conn)->commitWorkRelease();

	  // connect again
	  loginfo("Establish a connection once again");
      checknoerrrc(rc, conn)->connect(hostname(), database(), username(), password());

	  loginfo("Check SQLDBC_Connection.createPreparedStatement()");
	  SQLDBC_PreparedStatement* ps = checknoerr(conn)->createPreparedStatement();
	  
	  loginfo("Check SQLDBC_Connection.createStatement()");
	  SQLDBC_Statement*         st = checknoerr(conn)->createStatement();
	  
	  loginfo("Check SQLDBC_Connection.disconnect()");
	  checknoerr(conn)->disconnect();
	  
	  // connect again
	  checknoerrrc(rc, conn)->connect(hostname(), database(), username(), password()); 
	  
	  loginfo("Check SQLDBC_Connection.getAutoCommit()");
	  SQLDBC_Bool ac = checknoerr(conn)->getAutoCommit();
    
    loginfo("Check SQLDBC_Connection.getDateTimeFormat()");
    SQLDBC_DateTimeFormat::Format dtf = checknoerr(conn)->getDateTimeFormat();
    
    loginfo("Check SQLDBC_Connection.getKernelVersion()");
    SQLDBC_Int4 version = checknoerr(conn)->getKernelVersion();
    
    loginfo("Check SQLDBC_Connection.getTransactionIsolation()");
    SQLDBC_Int4 isolevel = checknoerr(conn)->getTransactionIsolation();
    
    loginfo("Check SQLDBC_Connection.isUnicodeDatabase()");
    SQLDBC_Bool iu = checknoerr(conn)->isUnicodeDatabase();

    loginfo("Check SQLDBC_Connection.releaseStatement()");
    checknoerr(conn)->releaseStatement(ps);
    checknoerr(conn)->releaseStatement(st);
    
    loginfo("Check SQLDBC_Connection.rollback()");
    checknoerr(conn)->rollback();
    
    loginfo("Check SQLDBC_Connection.setAutoCommit()");
    checknoerr(conn)->setAutoCommit(SQLDBC_FALSE);
    
    loginfo("Check SQLDBC_Connection.setSQLMode()");
    checknoerr(conn)->setSQLMode(SQLDBC_INTERNAL);
    
    loginfo("Check SQLDBC_Connection.setTransactionIsolation()");
    checknoerr(conn)->setTransactionIsolation(1);
    
    env.releaseConnection(conn);

    return m_success;
  }

};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
//   testframe.addTestCase (new Connection()); 
//   testframe.addTestCase (new PreparedStatement());
  testframe.addTestCase (new ResultSet());
//   testframe.addTestCase (new RowSet());
//   testframe.addTestCase (new Statement());
  if (testframe.run ()) 
    return 0;

  return 1;
}


