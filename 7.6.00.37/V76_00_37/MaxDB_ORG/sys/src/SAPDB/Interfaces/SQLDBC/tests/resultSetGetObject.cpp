/*!
  @file           resultSetGetObject.cpp
  @author         D042427
  @ingroup        Tests
  @brief          Tests different params on SQLDBC::SQLDBC_ResultSet::getObject
  @todo           http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126025
				  http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126591

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

#include <string.h>
#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"

class resultSetGetObjectIndex : public SQLDBC_TestCase
{

public:
  resultSetGetObjectIndex ()
    : SQLDBC_TestCase ("resultSetGetObjectIndex")
  {}

  void checkParam (SQLDBC_ResultSet *res, const SQLDBC_Int4 index, SQLDBC_Retcode expectedRetcode, SQLDBC_Length expectedLength, int line)
  {
    char cdata[50];
    SQLDBC_Length ind=SQLDBC_NTS;

	SQLDBC_Retcode rc = res->getObject(index, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata));
	if (rc != expectedRetcode)
		logerror("Line %d: getObject() returned %d. Expected was %d. column index = %d", line, rc, expectedRetcode, index);
	if (ind != expectedLength)
		logerror("Line %d: getObject() returned LengthIndicator == %d. Expected was %d", line, ind, expectedLength);
  }

  virtual SQLDBC_Bool run ()
  {
		
    SQLDBC_Retcode rc;
    SQLDBC_Statement *stmt = connection()->createStatement();

    const char *szSQL = "SELECT 'TEST' FROM Dual";
    tc_errorcheckrc(rc, stmt)->execute(szSQL);

	SQLDBC_ResultSet *res = stmt->getResultSet();
	if (!res)
		logerror("No resultset returned");

    tc_errorcheck(res)->next();

	loginfo("Check getObject() with different values for index...");
	checkParam(res, -4, SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res, -21376, SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res, 0, SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res, 1, SQLDBC_OK, 4, __LINE__);
	checkParam(res, 2, SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res, 874356, SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);


	//finish your program with a returncode 
	return m_success;
  }
};

class resultSetGetObjectType : public SQLDBC_TestCase
{

public:
  resultSetGetObjectType ()
    : SQLDBC_TestCase ("resultSetGetObjectType")
  {}

  void checkParam (SQLDBC_ResultSet *res, const SQLDBC_HostType Type, SQLDBC_Retcode expectedRetcode, SQLDBC_Length expectedLength, int line)
  {
      // Need an int8 buffer because of alignment
      SQLDBC_Int8 buffer[40];
      char *cdata = (char *)&buffer;
      
    SQLDBC_Length ind=SQLDBC_NTS;

	SQLDBC_Retcode rc = res->getObject(1, Type, cdata, &ind, 320);
	if (rc != expectedRetcode)
	{
		logerror("Line %d: getObject() returned %d. Expected was %d. HostType = %d", line, rc, expectedRetcode, Type);
	} else {
		if (ind != expectedLength)
			logerror("Line %d: getObject() returned LengthIndicator %d. Expected was %d. HostType = %d", line, ind, expectedLength, Type);
	}
  }

  virtual SQLDBC_Bool run ()
  {
		
    SQLDBC_Retcode rc;
    SQLDBC_Statement *stmt = connection()->createStatement();

	loginfo("Check getObject() with different host types...");

    // Let's test a simple string 'TEST'
	loginfo("*** ascii string ('TEST')...");
	const char *szSQL1 = "SELECT 'TEST' FROM Dual";
    tc_errorcheckrc(rc, stmt)->execute(szSQL1);
	SQLDBC_ResultSet *res1 = stmt->getResultSet();
	if (!res1) logerror("No resultset returned");
    tc_errorcheck(res1)->next();
    
	checkParam(res1, SQLDBC_HOSTTYPE_FLOAT , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res1, SQLDBC_HOSTTYPE_ASCII , SQLDBC_OK, 4, __LINE__);
    
	checkParam(res1, SQLDBC_HOSTTYPE_INT1 , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res1, SQLDBC_HOSTTYPE_UTF8 , SQLDBC_NO_DATA_FOUND, SQLDBC_NTS, __LINE__);
	checkParam(res1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res1, SQLDBC_HOSTTYPE_PARAMETER_NOTSET , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);

    // Let's test an integer '15'
	// fetch it as UTF8, then try to fetch it as integer
	// expected result: although it has already been fetched, it can be fetched again as an integer
	loginfo("*** integer (15)...");
    tc_errorcheckrc(rc, stmt)->execute("SELECT '15' FROM Dual");
	SQLDBC_ResultSet *res2 = stmt->getResultSet();
	if (!res2) logerror("No resultset returned");
    tc_errorcheck(res2)->next();

	loginfo("Fetch as FLOAT => fail");
	checkParam(res2, SQLDBC_HOSTTYPE_FLOAT , SQLDBC_OK, sizeof(float), __LINE__);
	loginfo("Fetch as INT1 => succeed");
	checkParam(res2, SQLDBC_HOSTTYPE_INT1 , SQLDBC_OK, 1, __LINE__);
	loginfo("Fetch as UTF8 => succeed");
	checkParam(res2, SQLDBC_HOSTTYPE_UTF8 , SQLDBC_OK, 2, __LINE__);
	loginfo("Fetch as ASCII => no data found");
	checkParam(res2, SQLDBC_HOSTTYPE_ASCII , SQLDBC_NO_DATA_FOUND, SQLDBC_NTS, __LINE__);

    // Let's test an integer '-5462'
	// fetch it as signed / unsigned integer + neg. tests
	loginfo("*** integer (-5462)...");
	tc_errorcheckrc(rc, stmt)->execute("SELECT '-5462' FROM Dual");
	SQLDBC_ResultSet *res3 = stmt->getResultSet();
	if (!res3) logerror("No resultset returned");
    tc_errorcheck(res3)->next();

	loginfo("Should not succeed because signed <> unsigned");
	checkParam(res3, SQLDBC_HOSTTYPE_UINT1 , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	// Achtung: Das Zweierkomplement passt nicht in 2 Byte
	checkParam(res3, SQLDBC_HOSTTYPE_UINT2 , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	
	checkParam(res3, SQLDBC_HOSTTYPE_UINT4 , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__); 
	// bug to be fixed: http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126591
	
	checkParam(res3, SQLDBC_HOSTTYPE_UINT8 , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
    
	loginfo("It should succeed as signed int >= INT2");
	checkParam(res3, SQLDBC_HOSTTYPE_INT1 , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res3, SQLDBC_HOSTTYPE_INT2 , SQLDBC_OK, 2, __LINE__); 
	checkParam(res3, SQLDBC_HOSTTYPE_INT4 , SQLDBC_OK, 4, __LINE__);
	checkParam(res3, SQLDBC_HOSTTYPE_INT8 , SQLDBC_OK, 8, __LINE__);

	checkParam(res3, SQLDBC_HOSTTYPE_ODBCTIMESTAMP , SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
	checkParam(res3, SQLDBC_HOSTTYPE_FLOAT , SQLDBC_OK, sizeof(float), __LINE__);
	checkParam(res3, SQLDBC_HOSTTYPE_DOUBLE , SQLDBC_OK, sizeof(double), __LINE__);

    // Let's test a timestamp '1988-12-25 17:12:30.000000'
	// fetch it as 
	dropTable("MyTable");
	loginfo("Create table to check timestamps...");
    tc_errorcheckrc(rc, stmt)->execute("CREATE TABLE MyTable (MyTimeStamp TIMESTAMP)");
	tc_errorcheckrc(rc, stmt)->execute("INSERT INTO MyTable VALUES ('1988-12-25 17:12:30.000000')");
	tc_errorcheckrc(rc, stmt)->execute("SELECT * FROM MyTable");
	SQLDBC_ResultSet *res4 = stmt->getResultSet();
	if (!res4) logerror("No resultset returned");
    tc_errorcheck(res4)->next();

	loginfo("getObject() hould succeed for ODBCDATE, ODBCTIMESTAMP, ODBCTIME");
	checkParam(res4, SQLDBC_HOSTTYPE_ODBCDATE , SQLDBC_OK, 6, __LINE__);
	checkParam(res4, SQLDBC_HOSTTYPE_ODBCTIMESTAMP , SQLDBC_OK, 16, __LINE__);
	checkParam(res4, SQLDBC_HOSTTYPE_ODBCTIME , SQLDBC_OK, 6, __LINE__);
	dropTable("MyTable");

    // Let's test a float '2.4E2'
	// fetch it as float, double + neg. tests
	loginfo("Create table to check floats...");
    tc_errorcheckrc(rc, stmt)->execute("CREATE TABLE MyTable (v FLOAT, id INTEGER)");
    tc_errorcheckrc(rc, stmt)->execute("INSERT INTO MyTable VALUES ('2.4E2', 0)");
    tc_errorcheckrc(rc, stmt)->execute("INSERT INTO MyTable VALUES ('23.12987654235476512765476352276E-2', 1)");

	// check the first value
	tc_errorcheckrc(rc, stmt)->execute("SELECT v FROM MyTable WHERE id=0");
	SQLDBC_ResultSet *res5 = stmt->getResultSet();
	if (!res5) logerror("No resultset returned");
    tc_errorcheck(res5)->next();

	loginfo("getObject() should succeed for FLOAT, DOUBLE, INT4");
	checkParam(res5, SQLDBC_HOSTTYPE_FLOAT , SQLDBC_OK, 4, __LINE__); 
	checkParam(res5, SQLDBC_HOSTTYPE_DOUBLE , SQLDBC_OK, 8, __LINE__); 
	checkParam(res5, SQLDBC_HOSTTYPE_INT4 , SQLDBC_OK, 4, __LINE__); // result = 240

	// The second one
	tc_errorcheckrc(rc, stmt)->execute("SELECT v FROM MyTable WHERE id=1");
	res5 = stmt->getResultSet();
	if (!res5) logerror("No resultset returned");
    tc_errorcheck(res5)->next();

	loginfo("getObject() should succeed for FLOAT, DOUBLE; truncated for INT4");
	checkParam(res5, SQLDBC_HOSTTYPE_FLOAT , SQLDBC_OK, 4, __LINE__);
	checkParam(res5, SQLDBC_HOSTTYPE_DOUBLE , SQLDBC_OK, 8, __LINE__);
	checkParam(res5, SQLDBC_HOSTTYPE_INT4 , SQLDBC_DATA_TRUNC, 4, __LINE__);
	dropTable("MyTable");

	//finish the test with a returncode 
	return m_success;
  }
};

class resultSetGetObjectSize : public SQLDBC_TestCase
{

public:
  resultSetGetObjectSize ()
    : SQLDBC_TestCase ("resultSetGetObjectSize")
  {}

  void checkParam (SQLDBC_Statement *stmt, const char *text, const char *expectedReturnValue,
	  const SQLDBC_HostType Type, SQLDBC_Length Size, SQLDBC_Retcode expectedRetcode,
	  SQLDBC_Length expectedLength, int line)
  {
    SQLDBC_Retcode rc;
	char szSQL[100];
	char cdata[50];

	sprintf(szSQL, "SELECT '%s' FROM Dual", text);
	tc_errorcheckrc(rc, stmt)->execute(szSQL);
	SQLDBC_ResultSet *res = stmt->getResultSet();
	if (!res) logerror("No resultset returned");
    tc_errorcheck(res)->next();

	// is Size too big?
	if (Size > 50) { Size = 50; }
	memset(&cdata, 0, sizeof(cdata));

    SQLDBC_Length ind=SQLDBC_NTS;
	rc = res->getObject(1, Type, cdata, &ind, Size);
	if (rc != expectedRetcode)
	{
		logerror("Line %d: getObject() returned %d. Expected was %d. Text = %s, type = %d, size = %d", line, rc, expectedRetcode, text, Type, Size);
	}
	else
	{
		if (ind != expectedLength)
			logerror("Line %d: getObject() returned LengthIndicator == %d. Expected was %d. Size: %d", line, ind, expectedLength, Size);
		if ((expectedReturnValue != NULL) && strcmp(expectedReturnValue, cdata))
			logerror("Line %d: getObject() wrote %s. Expected was %s. Size [of buffer]: %d", line, cdata, expectedReturnValue, Size);

		// check truncated results
		if (rc == SQLDBC_DATA_TRUNC)
		{
            // Remove counting of terminator.
            if(Size > 0) 
                Size--;
			int expectedLengthInd = expectedLength - Size;
			if (expectedLengthInd >= 0)
			{
				rc = res->getObject(1, Type, cdata, &ind, Size);

				if (ind != expectedLengthInd)
					logerror("Line %d. getObject() delivered truncated data. Now fetched once again. New LengthIndicator is %d. Expected %d", line, ind, expectedLengthInd);
			}
		}
	}
  }

  virtual SQLDBC_Bool run ()
  {
		
    SQLDBC_Statement *stmt = connection()->createStatement();

	loginfo("Check different values for parameter 'Size'");

	// let's test some param combinations
	char retString[100];
	loginfo("Should succeed for positive values of Size");
	// Attention: The third byte is needed for the 0-termination
	checkParam(stmt, "TESTTESTTEST", "TE", SQLDBC_HOSTTYPE_ASCII, 3, SQLDBC_DATA_TRUNC, 12, __LINE__); 
	checkParam(stmt, "TESTTESTTEST", "TESTTESTTEST", SQLDBC_HOSTTYPE_ASCII, 20, SQLDBC_OK, 12, __LINE__);

	memset(&retString, 0, sizeof(retString));
	// This does not make sense --> Stupid user
	checkParam(stmt, "TESTTESTTEST", retString, SQLDBC_HOSTTYPE_ASCII, 1, SQLDBC_DATA_TRUNC, 12, __LINE__); 
    
	loginfo("Should not succeed for negative values of Size");
	memset(&retString, 0, sizeof(retString));
	checkParam(stmt, "TESTTESTTEST", retString, SQLDBC_HOSTTYPE_ASCII, -5, SQLDBC_NOT_OK, SQLDBC_NTS, __LINE__);
    loginfo("Check a size of 0.");
	memset(&retString, 0, sizeof(retString));
	checkParam(stmt, "TESTTESTTEST", retString, SQLDBC_HOSTTYPE_ASCII, 0, SQLDBC_DATA_TRUNC, 12, __LINE__); 
	// bug has not been fixed yet: http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126025 mod.

	//finish your program with a returncode 
	return m_success;
  }
};

class resultSetGetObjectNullPointer : public SQLDBC_TestCase
{

public:
	resultSetGetObjectNullPointer ()
		: SQLDBC_TestCase ("resultSetGetObjectNullPointer")
	{}

	virtual SQLDBC_Bool run ()
	{
		SQLDBC_Statement *stmt = connection()->createStatement();

		SQLDBC_Retcode rc;
		char cdata[50];

		tc_errorcheckrc(rc, stmt)->execute("SELECT 'TEST' FROM Dual");
		SQLDBC_ResultSet *res = stmt->getResultSet();
		if (!res) logerror("No resultset returned");
		tc_errorcheck(res)->next();

		memset(&cdata, 0, sizeof(cdata));
		SQLDBC_Length ind=SQLDBC_NTS;

		// LengthIndicator == NULL should be possible
//		loginfo("Check NULL Pointer for LengthIndicator");
//		tc_expectederrorcheck(res, -10305)->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, NULL, 20);

		loginfo("Check NULL Pointer for ParamAdr");
		tc_expectederrorcheck(res, -10305)->getObject(1, SQLDBC_HOSTTYPE_ASCII, NULL, &ind, 20);

		return m_success;
	}
};


int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new resultSetGetObjectIndex());
  testframe.addTestCase (new resultSetGetObjectType());
  testframe.addTestCase (new resultSetGetObjectSize());
  testframe.addTestCase (new resultSetGetObjectNullPointer());
  if (testframe.run ()) 
    return 0;

  return 1;
}


