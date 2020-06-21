/*!
  @file           piecewise.cpp
  @author         D025086, D042427
  @ingroup        Tests
  @brief          USC2 [only for unicode data bases]: insert unicode strings (several puts), different combinations of amounts of data
                  DifferentTypes: late binding (insert) for different host types (FLOAT, ASCII, UINT1, UINT2, INT4), comparison of inserted and fetched data
				  WhereStatement: late binding (select / where clause) for several DATA_AT_EXEC columns (INTEGER + VARCHAR)
				  UpdateStatement: late binding (update) for several DATA_AT_EXEC columns (INTEGER + LONG ASCII), LONG at the beginning, in the middle, at the end
				  InsertAndDisconnect: disconnect while putting data, what can we do after having disconnected? what can we do after having connected again?
				  SeveralLongs: insert into ten LONG columns
				  VeryLongLong: insert different amounts of data (35x74, 536x53, 1000x100 Bytes)
				  FunctionSequence: check [almost] all the combinations of execute(), nextParameter(), putData() for INTEGER + LONG ASCII

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/SAPDBCommon/SAPDB_sprintf.h"
#include <string.h>

typedef struct PutValStruct {
  int addr;
  SQLDBC_Length ind;
  void *value;
  SQLDBC_Length length;
} PutValStruct;

char col1[11] = "abc";
int  col2 = 9999;

/* paramindex 0 is ignored */
PutValStruct p[] = {
  { 0, 0, NULL, 0 },
  { 0, 0, col1, SQLDBC_NTS },
  { 0, 0, &col2, 0 }
};

#define NUMPARAM (sizeof(p)/sizeof(p[0]))-1

int TableNumber = 1;

class PieceWiseStmt : public SQLDBC_TestCase
{
public:
  PieceWiseStmt ()
    : SQLDBC_TestCase ("PieceWiseStmt")
  {}

  ~PieceWiseStmt ()
  {
    dropTable("piecewise");
  }

  virtual SQLDBC_Bool run ()
  {

    dropTable("piecewise");
    execSQL("CREATE TABLE piecewise ( c char(10), i int)");
    int i;
    
    for (i=0;i <= NUMPARAM;i++) {
      p[i].addr = i;
      p[i].ind = SQLDBC_DATA_AT_EXEC;
    }

    SQLDBC_Retcode rc;

    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

    const char *szSQL = "INSERT INTO piecewise (c, i) values (?, ?)";

    tc_errorcheckrc(rc, stmt)->prepare(szSQL);
    
    tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, (void*)p[1].addr, &p[1].ind, sizeof(p[0].addr));

    tc_errorcheckrc(rc, stmt)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, (void*)p[2].addr, &p[2].ind, sizeof(p[0].addr));
    
    tc_errorcheckrc(rc, stmt)->execute();
    
    if (rc != SQLDBC_NEED_DATA) {
      logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
      return m_success;
    }
    
    while(rc == SQLDBC_NEED_DATA) {
      SQLDBC_Int2 index = 0x0BAD;
      void *addr=(void*)0x0BADBEEF;
      tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
      if (rc == SQLDBC_NEED_DATA) {
        loginfo("check index and addr");
        if (index <= 0 || index > NUMPARAM) {
          logerror("Parameterindex (%d) out of range", index);      
          return m_success;
        }      
        if (index != p[index].addr) {
          logerror("Wrong parameterindex returned (%d) expected index (%d)", index, p[index]);      
          return m_success;
        }
        if (addr != (void*)p[index].addr) {
          logerror("Wrong parameter addr returned (%d) expected index (%d)", addr, p[index]);      
          return m_success;
        }
        /* put data */
        SQLDBC_Retcode putrc;
        tc_errorcheckrc(putrc, stmt)->putData(p[index].value, &p[index].length);      
      }
      if (rc == SQLDBC_OK) {
        /* check that index and addr has not been changed */
        loginfo("check untouched");
        if (index != 0x0BAD || addr != (void*)0x0BADBEEF) {
          logerror("index or addr has been touched", index);      
          return m_success;
        }              
      }
    }
    return m_success;
  }

};

class UpdateByIndex
    : public SQLDBC_TestCase
{
public:
    UpdateByIndex()
    :SQLDBC_TestCase ("UpdateByIndex")
    {}
    
    virtual SQLDBC_Bool prepare() 
    {
        dropTable("SQLDBC_LONGUPDATE");
        execSQL("CREATE TABLE SQLDBC_LONGUPDATE(DATA LONG BYTE, A VARCHAR(40) ASCII, B VARCHAR(40) ASCII)");
        execSQL("INSERT INTO SQLDBC_LONGUPDATE (A, B) VALUES ('AVALUE', 'BVALUE')");
        return true;
    }
    
    virtual SQLDBC_Bool run ()
    {
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("UPDATE SQLDBC_LONGUPDATE SET DATA=? WHERE A=? AND B=?");
	char value[40];
        char avalue[40];
        char bvalue[40];
        SQLDBC_Retcode rc;
	strcpy(value, "VALUE");
        strcpy(avalue, "AVALUE");
        strcpy(bvalue, "CVALUE");
        SQLDBC_Length data_at_exec_ind = SQLDBC_DATA_AT_EXEC;
        SQLDBC_Length data_ind = SQLDBC_NTS;
        tc_errorcheckrc(rc, ps)->bindParameter(1, SQLDBC_HOSTTYPE_BINARY, value, &data_ind, sizeof(value), false);
        tc_errorcheckrc(rc, ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, avalue, &data_ind, sizeof(avalue));
        tc_errorcheckrc(rc, ps)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, bvalue, &data_ind, sizeof(bvalue));
        
        rc = ps->execute();
	if (rc != SQLDBC_NO_DATA_FOUND) {
	  logerror ("Expected returncode SQLDBC_NO_DATA_FOUND. Returncode was %d.", rc);
	}

        tc_errorcheckrc(rc, ps)->bindParameter(1, SQLDBC_HOSTTYPE_BINARY, (void*)0, &data_at_exec_ind, 1);

        strcpy(bvalue, "BVALUE");
        rc = ps->execute();
        
        if(rc == SQLDBC_NEED_DATA) {
            char buf[20000]; 
            memset(buf, 'A', sizeof(buf));
            void *pos=0;
            SQLDBC_Int2 index=1;
            rc = ps->nextParameterByIndex(index, pos);
            if(rc==SQLDBC_NEED_DATA) {
                SQLDBC_Length l=sizeof(buf);
                for(int i=0; i< 100; ++i) {
                    rc = tc_errorcheck(ps)->putData(buf, &l);
                    if(rc == SQLDBC_NOT_OK) {
                        logerror("Error in PUTDATA");
                        return false;
                    }
                }
                index = 0;
                ps->nextParameterByIndex(index, pos);
            } else {
                logerror("Expected NEED_DATA from nextParameterByIndex");
            }
        } else {
            logerror("EXPECTED NEED DATA");
        }
        connection()->releaseStatement(ps);
        connection()->commit();
        return m_success;
    }
    
};

class UCS2 : public SQLDBC_TestCase
{
protected:
	void cleanUp (SQLDBC_PreparedStatement *pstmt)
	{
		tc_errorcheck(connection())->releaseStatement(pstmt);

		// drop the table
		char TableName[100];
		sprintf(TableName, "testtable%d", TableNumber);
		dropTable(TableName);

		TableNumber++;
	}

	/* what this test case does:
	 *
	 *  1. creates a unicode table
	 *  2. prepares insert statement
	 *  3. binds parameter (indicator: DATA_AT_EXEC)
	 *  4. execute()
	 *  5. putData() several times (odd number of bytes => no valid unicode strings)
	 *  6. fetch and compare data
	 *
	 */
	virtual SQLDBC_Bool checkNum(const char *strSQLType, SQLDBC_HostType Type, char *val, int sizeofval, SQLDBC_Length numBytes,
		int numTimes, char *resval, int sizeofresval, SQLDBC_Length *resind, SQLDBC_Bool ErrorExpected)
	{
		loginfo("check %s", strSQLType);

		char strSQL[100];
		if (strlen(strSQLType) > 25) {
			logerror("invalid parameter: strSQLType == '%s'", strSQLType);
			return m_success;
		}

		loginfo("create table");
		sprintf(strSQL, "CREATE TABLE testtable%d (v %s)", TableNumber, strSQLType);
		execSQL(strSQL);

		SQLDBC_Retcode rc;
		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

		loginfo("prepare insert statement");
		sprintf(strSQL, "INSERT INTO testtable%d (v) VALUES (?)", TableNumber);
		tc_errorcheckrc(rc, stmt)->prepare(strSQL);

		SQLDBC_Length ind_dae = SQLDBC_DATA_AT_EXEC;
		tc_errorcheckrc(rc, stmt)->bindParameter(1, Type, val, &ind_dae, sizeofval);
		tc_errorcheckrc(rc, stmt)->execute();

		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			cleanUp(stmt);
			return m_success;
		}
		// nextParameter(): check index
		SQLDBC_Int2 index = 0x0BAD;
		void *addr=(void*)0x0BADBEEF;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			cleanUp(stmt);
			return m_success;
		}
//		loginfo("check index and addr");
		if (index != 1) {
			logerror("Wrong parameterindex returned (%d) expected index (%d)", index, 1);      
			cleanUp(stmt);
			return m_success;
		}      
		if (addr != val) {
			logerror("Wrong parameter addr returned.");
			cleanUp(stmt);
			return m_success;
		}

		loginfo("putData %d times [%d bytes]", numTimes, numBytes);
		SQLDBC_Retcode putrc = SQLDBC_OK;
		char *pval = (char *)val;
		int i;
		for (i=0; i<numTimes && (putrc == SQLDBC_OK); i++)
		{
			SQLDBC_Length copy_numBytes = numBytes;

			putrc = stmt->putData(pval, &copy_numBytes);      
			if (ErrorExpected)
			{
				if (putrc != SQLDBC_NOT_OK)
					logerror("putData was expected to fail. Return code: %d", putrc);
				cleanUp(stmt);
				return m_success;
			} else {
				if (putrc != SQLDBC_OK)
				{
					logerror("putData failed");
					cleanUp(stmt);
					return m_success;
				}
			}

			pval += numBytes;
		}

		// call nextParameter once again
		index = 0x0BAD;
		addr = (void*)0x0BADBEEF;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_OK) {
			logerror("Wrong returncode (%d) expected SQLDBC_OK(%d)", rc, SQLDBC_OK);
			cleanUp(stmt);
			return m_success;
		}
		if (index != 0x0BAD || addr != (void*)0x0BADBEEF) {
			logerror("index or addr has been touched", index);      
			cleanUp(stmt);
			return m_success;
		}              

		loginfo("fetch data");
		// fetch data
		sprintf(strSQL, "SELECT v FROM testtable%d", TableNumber);
		tc_errorcheckrc(rc, stmt)->prepare(strSQL);
		tc_errorcheckrc(rc, stmt)->execute();

		SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
		tc_errorcheck(rs)->next();
		tc_errorcheckrc(rc, rs)->getObject(1, SQLDBC_HOSTTYPE_UCS2, resval, resind, sizeofresval);
		if (rc != SQLDBC_OK)
		{
			logerror("getObject failed: %d", rc);
			cleanUp(stmt);
			return m_success;
		}

		cleanUp(stmt);
		return m_success;
	}

public:
	UCS2 ()
		: SQLDBC_TestCase ("UCS2")
	{}

	virtual SQLDBC_Bool prepare ()
	{
//		dropTable("testtable123");
		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
//		dropTable("testtable123");
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		if (connection()->isUnicodeDatabase())
		{
			// prepare string
			char val[1200];
			char *pval = val;
			int i;
			for (i=0; i<50; i++)
			{
				SAPDB_sprintfUnicode(SAPDB_EncodingUCS2, pval, 20, "ABCDEFGHIJKLMN");
				pval += 20;
			}

			char resval[1000];
			SAPDB_sprintfUnicode(SAPDB_EncodingUCS2, resval, sizeof(resval), "This text should have been erased...");
			SQLDBC_Length resind = SQLDBC_NTS;

			// call check method
			m_success &= checkNum("LONG UNICODE", SQLDBC_HOSTTYPE_UCS2, val, sizeof(val), 100, 6, resval, sizeof(resval), &resind, false);
			if (!m_success)
				return m_success;

			// check data
			loginfo("check data");
			if (memcmp(val, resval, resind))
			{
				logerror("Wrong data returned: '%s'. Expected was '%s'", val, pval);
			}
			else
			{
				char valAsc[100];
				SAPDB_sprintfUnicode(SAPDB_EncodingAscii, valAsc, sizeof(valAsc), "%=s", SAPDB_EncodingUCS2, resval);
				loginfo("Data returned: '%s...'", valAsc);
			}


			// second test
			SAPDB_sprintfUnicode(SAPDB_EncodingUCS2, resval, sizeof(resval), "This text should have been erased...");
			resind = SQLDBC_NTS;

			// call check method : should fail since unicode strings with odd length are not allowed
			checkNum("LONG UNICODE", SQLDBC_HOSTTYPE_UCS2, val, sizeof(val), 7, 5, resval, sizeof(resval), &resind, true);


			// third test
			SAPDB_sprintfUnicode(SAPDB_EncodingUCS2, resval, sizeof(resval), "This text should have been erased...");
			resind = SQLDBC_NTS;

			// call check method
			m_success &= checkNum("LONG UNICODE", SQLDBC_HOSTTYPE_UCS2, val, sizeof(val), 150, 3, resval, sizeof(resval), &resind, false);
			if (!m_success)
				return m_success;

			// check data
			loginfo("check data");
			if (memcmp(val, resval, resind))
			{
				logerror("Wrong data returned: '%s'. Expected was '%s'", val, pval);
			}
			else
			{
				char valAsc[100];
				SAPDB_sprintfUnicode(SAPDB_EncodingAscii, valAsc, sizeof(valAsc), "%=s", SAPDB_EncodingUCS2, resval);
				loginfo("Data returned: '%s...'", valAsc);
			}

		}
		else
		{
			loginfo("/********************************************/");
			loginfo("/* Test case cannot be executed completely: */");
			loginfo("/* We do not have a unicode database        */");
			loginfo("/********************************************/");
		}


		return m_success;
	}
};

class DifferentTypes : public SQLDBC_TestCase
{
protected:
	void cleanUp (SQLDBC_PreparedStatement *pstmt)
	{
		tc_errorcheck(connection())->releaseStatement(pstmt);

		// drop the table
		char TableName[100];
		sprintf(TableName, "testtable%d", TableNumber);
		dropTable(TableName);

		TableNumber++;
	}

	/* how this method works:
	 * 
	 *  1. creates a table with one column
	 *  2. prepares insert statement 
	 *  3. binds parameter (given host type, indicator: DATA_AT_EXEC)
	 *  4. execute() -> returns with NEED_DATA
	 *  5. nextParameter() : check if right parameter reference is returned
	 *  6. putData() : use given val and ind
	 *  7. nextParameter() : should return OK since all parameters are filled
	 *  8. fetches data [SELECT *] -> there should be a result set
	 *     resval and resind are filled
	 *
	 */
	SQLDBC_Bool checkType (const char *strSQLType, SQLDBC_HostType Type, void *val, int sizeofval, SQLDBC_Length *ind,
		void *resval, int sizeofresval, SQLDBC_Length *resind)
	{
		char strSQL[100];
		if (strlen(strSQLType) > 25) {
			logerror("invalid parameter: strSQLType == '%s'", strSQLType);
			return m_success;
		}

		sprintf(strSQL, "CREATE TABLE testtable%d (v %s)", TableNumber, strSQLType);
		execSQL(strSQL);

		SQLDBC_Retcode rc;
		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

		sprintf(strSQL, "INSERT INTO testtable%d (v) VALUES (?)", TableNumber);
		tc_errorcheckrc(rc, stmt)->prepare(strSQL);
		SQLDBC_Length ind_dae = SQLDBC_DATA_AT_EXEC;
		tc_errorcheckrc(rc, stmt)->bindParameter(1, Type, val, &ind_dae, sizeofval);
		tc_errorcheckrc(rc, stmt)->execute();

		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			cleanUp(stmt);
			return m_success;
		}
		// nextParameter(): check index
		SQLDBC_Int2 index = 0x0BAD;
		void *addr=(void*)0x0BADBEEF;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			cleanUp(stmt);
			return m_success;
		}
//		loginfo("check index and addr");
		if (index != 1) {
			logerror("Wrong parameterindex returned (%d) expected index (%d)", index, 1);      
			cleanUp(stmt);
			return m_success;
		}      
		if (addr != val) {
			logerror("Wrong parameter addr returned.");
			cleanUp(stmt);
			return m_success;
		}

		/* put data */
		SQLDBC_Retcode putrc;
		tc_errorcheckrc(putrc, stmt)->putData(val, ind);      
		if (putrc != SQLDBC_OK)
		{
			logerror("putData failed");
			cleanUp(stmt);
			return m_success;
		}

		// call nextParameter once again
		index = 0x0BAD;
		addr = (void*)0x0BADBEEF;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_OK) {
			logerror("Wrong returncode (%d) expected SQLDBC_OK(%d)", rc, SQLDBC_OK);
			cleanUp(stmt);
			return m_success;
		}
		if (index != 0x0BAD || addr != (void*)0x0BADBEEF) {
			logerror("index or addr has been touched", index);      
			cleanUp(stmt);
			return m_success;
		}              

		// fetch data
		sprintf(strSQL, "SELECT v FROM testtable%d", TableNumber);
		tc_errorcheckrc(rc, stmt)->prepare(strSQL);
		tc_errorcheckrc(rc, stmt)->execute();
		SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
		tc_errorcheck(rs)->next();
		tc_errorcheckrc(rc, rs)->getObject(1, Type, resval, resind, sizeofresval);
		if (rc != SQLDBC_OK)
		{
			logerror("getObject failed: %d", rc);
			cleanUp(stmt);
			return m_success;
		}
		
		cleanUp(stmt);
		return m_success;
	}

public:
	DifferentTypes ()
		: SQLDBC_TestCase ("DifferentTypes")
	{}

	virtual SQLDBC_Bool prepare ()
	{
//		dropTable("testtable123");
		return m_success;
	}

	virtual SQLDBC_Bool release()
	{
//		dropTable("testtable123");
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		loginfo("Check some host types...");

		/*****************************************/
		/*****          begin FLOAT          *****/
		/*****************************************/
		loginfo("FLOAT...");
		float val2 = (float)1.5E-2;
		SQLDBC_Length ind2 = 0;
		float resval2 = (float)0.0;
		SQLDBC_Length resind2 = SQLDBC_NTS;

		checkType("FLOAT", SQLDBC_HOSTTYPE_FLOAT, &val2, sizeof(val2), &ind2, &resval2, sizeof(resval2), &resind2);

		if (resval2 != val2)
			logerror("Wrong value was returned: %f. Expected was %f", resval2, val2);
		/*****************************************/
		/*****          end FLOAT            *****/
		/*****************************************/
	
		/*****************************************/
		/*****          begin ASCII          *****/
		/*****************************************/
		loginfo("ASCII...");
		char val3[50];
		sprintf(val3, "  test abc");
		SQLDBC_Length ind3 = SQLDBC_NTS;
		char resval3[80];
		sprintf(resval3, "This should have been erased...");
		SQLDBC_Length resind3 = SQLDBC_NTS;

		checkType("VARCHAR(30) ASCII", SQLDBC_HOSTTYPE_ASCII, val3, sizeof(val3), &ind3, resval3, sizeof(resval3), &resind3);

		if (strcmp(resval3, val3))
			logerror("Wrong value was returned: '%s'. Expected was '%s'", resval3, val3);
		/*****************************************/
		/*****          end ASCII            *****/
		/*****************************************/
	
		/*****************************************/
		/*****          begin UINT1          *****/
		/*****************************************/
		loginfo("UINT1...");
    unsigned char val4 = (unsigned char)7;
		SQLDBC_Length ind4 = 0;
		unsigned char resval4 = 0;
		SQLDBC_Length resind4 = SQLDBC_NTS;

		checkType("INTEGER", SQLDBC_HOSTTYPE_UINT1, &val4, sizeof(val4), &ind4, &resval4, sizeof(resval4), &resind4);

		if (resval4 != val4)
			logerror("Wrong value was returned: %d. Expected was %d", resval4, val4);
		/*****************************************/
		/*****          end UINT1            *****/
		/*****************************************/
	
		/*****************************************/
		/*****          begin UINT2          *****/
		/*****************************************/
		loginfo("UINT2...");
		unsigned short val5 = 25;
		SQLDBC_Length ind5 = 0;
		unsigned short resval5 = 0;
		SQLDBC_Length resind5 = SQLDBC_NTS;

		checkType("INTEGER", SQLDBC_HOSTTYPE_UINT2, &val5, sizeof(val5), &ind5, &resval5, sizeof(resval5), &resind5);

		if (resval5 != val5)
			logerror("Wrong value was returned: %d. Expected was %d", resval5, val5);
		/*****************************************/
		/*****          end UINT2            *****/
		/*****************************************/
	
		/*****************************************/
		/*****          begin INT4           *****/
		/*****************************************/
		loginfo("INT4...");
		int val6 = -5;
		SQLDBC_Length ind6 = 0;
		int resval6 = 0;
		SQLDBC_Length resind6 = SQLDBC_NTS;

		checkType("INTEGER", SQLDBC_HOSTTYPE_INT4, &val6, sizeof(val6), &ind6, &resval6, sizeof(resval6), &resind6);

		if (resval6 != val6)
			logerror("Wrong value was returned: %d. Expected was %d", resval6, val6);
		/*****************************************/
		/*****          end INT4             *****/
		/*****************************************/
	
		return m_success;
	}
};


	/* how this test case works
	 *
	 * 1. create table with three columns (int, varchar [DATA_AT_EXEC], int [DATA_AT_EXEC])
	 * 2. insert one row
	 * 3. prepare select statement
	 * 4. bind parameter (DATA_AT_EXEC)
	 * 5. execute -> should return with NEED_DATA
	 * 6. nextParameter -> should return with NEED_DATA (for the int column)
	 * 7. putData several times
	 * 8. nextParameter -> should return with NEED_DATA (for the varchar column)
	 * 9. putData
	 * 10. nextParameter -> should return with SQLDBC_OK (since a result set should be returned)
	 * 11. check result set
	 *
	 */
class WhereStatement : public SQLDBC_TestCase
{
public:
	WhereStatement ()
		: SQLDBC_TestCase ("WhereStatement")
	{}

	virtual SQLDBC_Bool release ()
	{
		dropTable("testtable99999");
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		loginfo("check WHERE statement with DATA_AT_EXEC...");

		dropTable("testtable99999");
		loginfo("create table and insert value");
		execSQL("CREATE TABLE testtable99999 (i INTEGER, v VARCHAR(25) ASCII, j INTEGER)");
		
		SQLDBC_Retcode rc;

		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
		tc_errorcheck(stmt)->prepare("INSERT INTO testtable99999 (i, v, j) VALUES (35, 'ABCDEFGHIJKLMNO', -65)");
		tc_errorcheck(stmt)->execute();

		char val[1000];
		sprintf(val, "ABCDEFGHIJKLMNO");
		int valint = -65;
		SQLDBC_Length ind=SQLDBC_DATA_AT_EXEC;

		loginfo("prepare select statement");
		tc_errorcheckrc(rc, stmt)->prepare("SELECT v, j FROM testtable99999 WHERE v = ? AND j = ?");
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, val, &ind, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, &valint, &ind, sizeof(valint));
    
		tc_errorcheckrc(rc, stmt)->execute();
    
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
    
		SQLDBC_Int2 index = 0x0BAD;
		void *addr=(void*)0x0BAD;


        loginfo("call nextParameter");
        int pcount =0;
        do {
            loginfo("call to nextParameter, pcount = %d", pcount);
            tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
            if (rc != SQLDBC_NEED_DATA) {
                if(rc == SQLDBC_OK && pcount == 2) {
                    break;
                }
                logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
                return m_success;
            }
            if(index == 2) {
                ++pcount;
                // put data (integer)
                loginfo("call putData (integer)");
                SQLDBC_Length l = 4;
                tc_errorcheckrc(rc, stmt)->putData(&valint, &l);
            } else if(index == 1) {
                ++pcount;
                // put data (varchar)
                loginfo("call putData several times (varchar)");
                char *pval = val;
                int i;
                SQLDBC_Length l=3;
                for (i=0; i<5; i++)
                {
                    tc_errorcheckrc(rc, stmt)->putData(pval, &l);
                    if (rc != SQLDBC_OK)
                        return m_success;
                    pval += l;
                }
            } else {
                logerror("Invalid index %d returned.", index);
                return m_success;
            }
        } while(true);
		if (rc != SQLDBC_OK) {
			logerror("nextParameter returned %d. Expected was SQLDBC_OK [%d]", rc, SQLDBC_OK);
			return m_success;
		}

		SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
		if (!rs) {
			logerror("No result was returned");
			return m_success;
		}
		// check result set
		loginfo("check result set");
		char resval[1000];
		SQLDBC_Length resind = SQLDBC_NTS;
		tc_errorcheck(rs)->next();
		tc_errorcheckrc(rc, rs)->getObject(1, SQLDBC_HOSTTYPE_ASCII, resval, &resind, sizeof(resval));
		loginfo("result data: %s", resval);

		return m_success;
	}
};

SQLDBC_Length numB[] = {1, 9, 4, 5, 2, 7, 4, 3, 0, 6, 0, 3, 12, 4}; // sum should be <= 60
#define NUMNUMB (sizeof(numB)/sizeof(numB[0]))

	/* how this test case works
	 *
	 * 1. create table with three columns (LONG ASCII [DATA_AT_EXEC], INTEGER, INTEGER [DATA_AT_EXEC])
	 * 2. insert one row
	 *   3. prepare update statement
	 *   4. bind parameter (DATA_AT_EXEC)
	 *   5. execute -> should return with NEED_DATA
	 *   6. nextParameter -> should return with NEED_DATA: INTEGER
	 *   7. putData once
	 *   8. nextParameter -> should return with NEED_DATA: INTEGER
	 *   9. putData once
	 *  10. nextParameter -> should return with NEED_DATA: LONG
	 *  11. putData several times
	 *  12. nextParameter -> should return with SQLDBC_OK
	 *  13. execute select statement
	 *  14. check result set
	 *
	 * steps 3 to 9 are executed several times (with different column order)
	 *
	 */
class UpdateStatement : public SQLDBC_TestCase
{
public:
	UpdateStatement ()
		: SQLDBC_TestCase ("UpdateStatement")
	{}

	virtual SQLDBC_Bool release ()
	{
		dropTable("testtable99999");
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		loginfo("check UPDATE statement with DATA_AT_EXEC...");

		dropTable("testtable99999");
		loginfo("create table and insert value");
		execSQL("CREATE TABLE testtable99999 (t LONG ASCII, i1 INTEGER, i2 INTEGER)");
		
		SQLDBC_Retcode rc;

		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
		tc_errorcheck(stmt)->prepare("INSERT INTO testtable99999 (t, i1, i2) VALUES ('Erase this', -99, 4235)");
		tc_errorcheck(stmt)->execute();

		// some variables which we are going to use later on (several times)
		char val[1000], resval[1000];
		int valint1, valint2, resvalint1, resvalint2;
		SQLDBC_Length ind_dae = SQLDBC_DATA_AT_EXEC; // let us hope that the value will not be changed (=> is used for call by reference)
		SQLDBC_Int2 index = 0x0BAD;
		void *addr=(void*)0x0BAD;
		SQLDBC_Length l, resind;
		char *pval;
		int i;
		SQLDBC_ResultSet *rs;

		/***************************************************/
		/*        first test: LONG at the beginning        */
		/***************************************************/
		loginfo("LONG column at the beginning...");

		sprintf(val, "ABCDEFGHIJKLMNOPQRSTU"); // 21 signs
		valint1=43;
		valint2=64;

		loginfo("prepare update statement");
		tc_errorcheckrc(rc, stmt)->prepare("UPDATE testtable99999 SET t = ?, i1 = ?, i2 = ?");
		tc_errorcheckrc(rc, stmt)->bindParameter(3, SQLDBC_HOSTTYPE_INT4, &valint2, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, &valint1, &ind_dae, sizeof(val));
    
		tc_errorcheckrc(rc, stmt)->execute();
    
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
    
		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		if (index == 2 || index == 3) {
			// put data (integer)
			loginfo("call putData (integer)");
			l = 4;
			tc_errorcheckrc(rc, stmt)->putData((index == 2) ? &valint1 : &valint2, &l);
		} else {
			logerror("Column no. 2 or 3 [integer] should be processed first.");
			return m_success;
		}

		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (index == 2 || index == 3) {
			// put data (integer)
			loginfo("call putData (integer)");
			l = 4;
			tc_errorcheckrc(rc, stmt)->putData((index == 2) ? &valint1 : &valint2, &l);
		} else {
			logerror("Column no. 2 or 3 [integer] should be processed now.");
			return m_success;
		}

		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);

		// put data
		loginfo("call putData several times: 7x3Bytes (long ascii)");
		pval = val;
		l=3;
		for (i=0; i<7; i++)
		{
			tc_errorcheckrc(rc, stmt)->putData(pval, &l);
			pval += l;
		}

		// call nextParameter once again
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_OK) {
			logerror("nextParameter returned %d. Expected was SQLDBC_OK [%d]", rc, SQLDBC_OK);
			return m_success;
		}

		// select
		tc_errorcheckrc(rc, stmt)->prepare("SELECT i1, i2, t FROM testtable99999");
		tc_errorcheckrc(rc, stmt)->execute();
		rs = tc_errorcheck(stmt)->getResultSet();
		if (!rs) {
			logerror("No result was returned");
			return m_success;
		}
		// get result data
		sprintf(resval, "This text should have been erased");
		resvalint1 = -33;
		resvalint2 = 32545;
		resind = SQLDBC_NTS;
		tc_errorcheck(rs)->next();
		tc_errorcheckrc(rc, rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &resvalint1, &resind, sizeof(resvalint1));
		tc_errorcheckrc(rc, rs)->getObject(2, SQLDBC_HOSTTYPE_INT4, &resvalint2, &resind, sizeof(resvalint2));
		resind = SQLDBC_NTS;
		tc_errorcheckrc(rc, rs)->getObject(3, SQLDBC_HOSTTYPE_ASCII, resval, &resind, sizeof(resval));

		// check result data
		if (strcmp(resval, val) || (valint1 != resvalint1) || (valint2 != resvalint2))
			logerror("Wrong data returned: (%d, %d, '%s'). Expected was (%d, %d, '%s')", resvalint1, resvalint2, resval, valint1, valint2, val);

		/***************************************************/
		/*       second test: LONG in the middle           */
		/***************************************************/
		loginfo("LONG column in the middle...");

		sprintf(val, "ABCDEFGHIJKLMN"); // 14 signs
		valint1=725;
		valint2=-832;

		loginfo("prepare update statement");
		tc_errorcheckrc(rc, stmt)->prepare("UPDATE testtable99999 SET i2 = ?, t = ?, i1 = ?");
		tc_errorcheckrc(rc, stmt)->bindParameter(3, SQLDBC_HOSTTYPE_INT4, &valint1, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &valint1, &ind_dae, sizeof(val));
    
		tc_errorcheckrc(rc, stmt)->execute();
    
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
    
		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		if (index == 1 || index == 3) {
			// put data (integer)
			loginfo("call putData (integer)");
			l = 4;
			tc_errorcheckrc(rc, stmt)->putData((index == 3) ? &valint1 : &valint2, &l);
		} else {
			logerror("Column no. 1 or 3 [integer] should be processed first.");
			return m_success;
		}

		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (index == 1 || index == 3) {
			// put data (integer)
			loginfo("call putData (integer)");
			l = 4;
			tc_errorcheckrc(rc, stmt)->putData((index == 3) ? &valint1 : &valint2, &l);
		} else {
			logerror("Column no. 1 or 3 [integer] should be processed now.");
			return m_success;
		}

		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);

		// put data
		loginfo("call putData several times: 2x7Bytes (long ascii)");
		pval = val;
		l=7;
		for (i=0; i<2; i++)
		{
			tc_errorcheckrc(rc, stmt)->putData(pval, &l);
			pval += l;
		}

		// call nextParameter once again
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_OK) {
			logerror("nextParameter returned %d. Expected was SQLDBC_OK [%d]", rc, SQLDBC_OK);
			return m_success;
		}

		// select
		tc_errorcheckrc(rc, stmt)->prepare("SELECT i1, i2, t FROM testtable99999");
		tc_errorcheckrc(rc, stmt)->execute();
		rs = tc_errorcheck(stmt)->getResultSet();
		if (!rs) {
			logerror("No result was returned");
			return m_success;
		}
		// get result data
		sprintf(resval, "This text should have been erased");
		resvalint1 = -33;
		resvalint2 = 32545;
		tc_errorcheck(rs)->next();
		resind = SQLDBC_NTS;
		tc_errorcheckrc(rc, rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &resvalint1, &resind, sizeof(resvalint1));
		resind = SQLDBC_NTS;
		tc_errorcheckrc(rc, rs)->getObject(2, SQLDBC_HOSTTYPE_INT4, &resvalint2, &resind, sizeof(resvalint2));
		resind = SQLDBC_NTS;
		tc_errorcheckrc(rc, rs)->getObject(3, SQLDBC_HOSTTYPE_ASCII, resval, &resind, sizeof(resval));

		// check result data
		if (strcmp(resval, val) || (valint1 != resvalint1) || (valint2 != resvalint2))
			logerror("Wrong data returned: (%d, %d, '%s'). Expected was (%d, %d, '%s')", resvalint1, resvalint2, resval, valint1, valint2, val);

		/***************************************************/
		/*       third test: LONG at the end               */
		/***************************************************/
		loginfo("LONG column at the end...");

		sprintf(val, "____1____2____3____4____5____6____7____8____9___10___11___12"); // 60 signs
		valint1=725;
		valint2=-832;

		loginfo("prepare update statement");
		tc_errorcheckrc(rc, stmt)->prepare("UPDATE testtable99999 SET i1 = ?, i2 = ?, t = ?");
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &valint1, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, &valint1, &ind_dae, sizeof(val));
    
		tc_errorcheckrc(rc, stmt)->execute();
    
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
    
		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		if (index == 1 || index == 2) {
			// put data (integer)
			loginfo("call putData (integer)");
			l = 4;
			tc_errorcheckrc(rc, stmt)->putData((index == 1) ? &valint1 : &valint2, &l);
		} else {
			logerror("Column no. 1 or 2 [integer] should be processed first.");
			return m_success;
		}

		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (index == 1 || index == 2) {
			// put data (integer)
			loginfo("call putData (integer)");
			l = 4;
			tc_errorcheckrc(rc, stmt)->putData((index == 1) ? &valint1 : &valint2, &l);
		} else {
			logerror("Column no. 1 or 2 [integer] should be processed now.");
			return m_success;
		}

		// call nextParameter
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);

		// put data
		loginfo("call putData several times with different number of bytes (long ascii)");
		pval = val;
		for (i=0; i<NUMNUMB; i++)
		{
			tc_errorcheckrc(rc, stmt)->putData(pval, &numB[i]);
			pval += numB[i];
		}

		// call nextParameter once again
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_OK) {
			logerror("nextParameter returned %d. Expected was SQLDBC_OK [%d]", rc, SQLDBC_OK);
			return m_success;
		}

		// select
		tc_errorcheckrc(rc, stmt)->prepare("SELECT i1, i2, t FROM testtable99999");
		tc_errorcheckrc(rc, stmt)->execute();
		rs = tc_errorcheck(stmt)->getResultSet();
		if (!rs) {
			logerror("No result was returned");
			return m_success;
		}
		// get result data
		pval = resval;
		for (i=0; i<100; i++) {
			*pval = 'x';
			pval++;
		}
		*pval = '\0';
		resvalint1 = -33;
		resvalint2 = 32545;
		tc_errorcheck(rs)->next();
		resind = SQLDBC_NTS;
		tc_errorcheckrc(rc, rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &resvalint1, &resind, sizeof(resvalint1));
		resind = SQLDBC_NTS;
		tc_errorcheckrc(rc, rs)->getObject(2, SQLDBC_HOSTTYPE_INT4, &resvalint2, &resind, sizeof(resvalint2));
		resind = SQLDBC_NTS;
		tc_errorcheckrc(rc, rs)->getObject(3, SQLDBC_HOSTTYPE_ASCII, resval, &resind, sizeof(resval));

		// check result data
		if (strcmp(resval, val) || (valint1 != resvalint1) || (valint2 != resvalint2))
			logerror("Wrong data returned: (%d, %d, '%s'). Expected was (%d, %d, '%s')", resvalint1, resvalint2, resval, valint1, valint2, val);

		/*** done ***/

        connection()->releaseStatement(stmt);
		return m_success;
	}
};


	/* how this test case works
	 *
	 * 1. create table with one column (LONG ASCII)
	 * 2. prepare insert statement
	 * 3. bind parameter (DATA_AT_EXEC)
	 * 4. execute -> should return with NEED_DATA
	 * 5. nextParameter -> should return with NEED_DATA
	 * 6. putData several times
	 * 7. DISCONNECT
	 * 8. can we put data once again???
	 * 9. what does nextParameter return???
	 *
	 * 10. connect again
	 * 11. what can we do now???
	 *
	 */
class InsertAndDisconnect : public SQLDBC_TestCase
{
public:
	InsertAndDisconnect ()
		: SQLDBC_TestCase ("InsertAndDisconnect")
	{}

	virtual SQLDBC_Bool run ()
	{
		loginfo("check disconnect while putting data...");

		SQLDBC_Retcode rc;
		SQLDBC_Environment env(runtime());
		SQLDBC_Connection *conn = env.createConnection();
		if (!conn) {
			logerror("createConnection does not return a valid object\n");
			return 0;
		}

		// connect using the given parameters
		loginfo("connect to database");
		tc_errorcheckrc(rc, conn)->connect(hostname(), database(), username(), password());

		SQLDBC_Statement *st = conn->createStatement();

		st->execute("DROP TABLE testtable1234");
//		dropTable("testtable1234");
		loginfo("create table and insert value");
		tc_errorcheckrc(rc, st)->execute("CREATE TABLE testtable1234 (t LONG ASCII)");
//		execSQL("CREATE TABLE testtable1234 (t LONG ASCII)");
		
		char val[1000];
		sprintf(val, "____1____2____3____4____5____6"); // 30 signs
		SQLDBC_Length ind = SQLDBC_DATA_AT_EXEC;

		// prepare insert statement
		loginfo("prepare insert statement and bind column");
		SQLDBC_PreparedStatement *stmt = conn->createPreparedStatement();
//		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
		tc_errorcheck(stmt)->prepare("INSERT INTO testtable1234 (t) VALUES (?)");
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, val, &ind, sizeof(val));
		tc_errorcheckrc(rc, stmt)->execute();
    
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
    
		SQLDBC_Int2 index = 0x0BAD;
		void *addr=(void*)0x0BAD;
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("Wrong returncode (%d) expected SQLDBC_NEED_DATA(%d)", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		if (index != 1) {
			logerror("Wrong column index returned.");
			return m_success;
		}

		// put data (long ascii)
		loginfo("call putData several times (long ascii)");
		SQLDBC_Length l = 4;
		char *pval = val;
		int i;
		for (i=0; i<5; i++)
		{
			tc_errorcheckrc(rc, stmt)->putData(pval, &l);
			pval += l;
		}

		// now disconnect
		loginfo("Now dare to --DISCONNECT-- ...");
		tc_errorcheck(conn)->disconnect(); 
//		connection()->disconnect(); 

		// put data once again
		loginfo("Try to put data once again");
		rc = stmt->putData(&pval, &l);
		if (rc != SQLDBC_NOT_OK) {
			logerror("putData returned %d. Expected was SQLDBC_NOT_OK [%d]", rc, SQLDBC_NOT_OK);
			return m_success;
		}

		// call nextParameter
		loginfo("call nextParameter");
		rc = stmt->nextParameter(index, addr);
		if (rc != SQLDBC_NOT_OK) {
			logerror("nextParameter returned %d. Expected was SQLDBC_NOT_OK [%d]", rc, SQLDBC_NOT_OK);
			return m_success;
		}

		// connect again
		loginfo("connect again");
		tc_errorcheck(conn)->connect(hostname(), database(), username(), password());

		// put data once again
		loginfo("Try to put data once again");
		rc = stmt->putData(&pval, &l);
		if (rc != SQLDBC_NOT_OK) {
			logerror("putData returned %d. Expected was SQLDBC_NOT_OK [%d]", rc, SQLDBC_NOT_OK);
			return m_success;
		}

		// now call nextParameter once again
		loginfo("call nextParameter");
		rc = stmt->nextParameter(index, addr);
		if (rc != SQLDBC_NOT_OK) {
			logerror("nextParameter returned %d. Expected was SQLDBC_NOT_OK [%d]", rc, SQLDBC_NOT_OK);
			return m_success;
		}


		st->execute("DROP TABLE testtable1234");
		tc_errorcheck(conn)->releaseStatement(st);
		tc_errorcheck(conn)->releaseStatement(stmt);
		tc_errorcheck(conn)->disconnect();
		env.releaseConnection(conn);
		return m_success;
	}
};


	/* how this test case works
	 *
	 * 1. create table with ten columns (all LONG ASCII)
	 * 2. prepare insert statement
	 * 3. bind parameter (8x DATA_AT_EXEC, 2x SQLDBC_NTS)
	 * 4. execute -> should return with NEED_DATA
	 *    5. nextParameter -> should return with NEED_DATA
	 *    6. putData several times
	 * 7. nextParameter -> should return with SQLDBC_OK
	 * 8. execute select statement
	 * 9. check result set
	 *
	 * steps 5, 6 are repeated several times (per column one time)
	 *
	 */
class SeveralLongs : public SQLDBC_TestCase
{
public:
	SeveralLongs ()
		: SQLDBC_TestCase ("SeveralLongs")
	{}

	virtual SQLDBC_Bool release ()
	{
		dropTable("testtable99999");
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		loginfo("check putting data into several LONG columns...");

		SQLDBC_Retcode rc;

		dropTable("testtable99999");
		loginfo("create table and insert value");
		execSQL("CREATE TABLE testtable99999 (t0 LONG ASCII, t1 LONG ASCII, t2 LONG ASCII, t3 LONG ASCII, t4 LONG ASCII, t5 LONG ASCII, t6 LONG ASCII, t7 LONG ASCII, t8 LONG ASCII, t9 LONG ASCII)");
		
		char val[1000];
		sprintf(val, "____1____2____3____4____5____6"); // 30 signs
		SQLDBC_Length ind_dae = SQLDBC_DATA_AT_EXEC;
		SQLDBC_Length ind;

		// prepare insert statement
		loginfo("prepare insert statement and bind the ten columns");
		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
		tc_errorcheck(stmt)->prepare("INSERT INTO testtable99999 (t0, t1, t2, t3, t4, t5, t6, t7, t8, t9) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		ind = SQLDBC_NTS;
		tc_errorcheckrc(rc, stmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, val, &ind, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(4, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(5, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(6, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(7, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		ind = SQLDBC_NTS;
		tc_errorcheckrc(rc, stmt)->bindParameter(8, SQLDBC_HOSTTYPE_ASCII, val, &ind, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(9, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(10, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->execute();
    
		SQLDBC_Int2 index = 0x0BAD;
		void *addr=(void*)0x0BAD;
		loginfo("call nextParameter");
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);

		int numP = 0;
		bool filled[10] = {false, false, false, false, false, false, false, false, false, false};
		loginfo("call putData and nextParameter several times (long ascii)");
		loginfo("Are all columns filled?");
		while (rc == SQLDBC_NEED_DATA) 
		{
			// check column index
			if ((index < 1) || (index > 10) || (index == 2) || (index == 8)) {
				logerror("Invalid column index returned: %d", index);
				return m_success;
			}

			// put data (30 Bytes)
			SQLDBC_Length l = 5;
			char *pval = val;
			int i;
			for (i=0; i<6; i++)
			{
				tc_errorcheckrc(rc, stmt)->putData(pval, &l);
				pval += l;
			}

			filled[index-1] = true;

			// call nextParameter
			rc = stmt->nextParameter(index, addr);

			numP++;
		}

		// how many columns have been filled?
		if (numP != 8)
			logerror("%d columns have been filled [putData]. Expected number: %d", numP, 8);
		else
			loginfo("%d columns have been filled", numP);
		// have the right columns been filled?
		int i;
		for (i=1;i<=10;i++) {
			if (filled[i-1] != ((i != 2) && (i != 8))) {
				if (filled[i-1]) 
					logerror("column %d should not have been filled", i);
				else
					logerror("column %d should have been filled", i);
			}
		}

		// select statement
		tc_errorcheckrc(rc, stmt)->prepare("SELECT * FROM testtable99999");
		tc_errorcheckrc(rc, stmt)->execute();
		SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
		if (!rs) {
			logerror("No result was returned");
			return m_success;
		}
		// get result data
		tc_errorcheck(rs)->next();

		char resval[50];
		for (i=1; i<=10; i++) {

			// init result buffer
			sprintf(resval, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"); // 35 signs

			SQLDBC_Length resind = SQLDBC_NTS;
			tc_errorcheckrc(rc, rs)->getObject(i, SQLDBC_HOSTTYPE_ASCII, &resval, &resind, sizeof(resval));

			// check result data
			if (strcmp(resval, val))
				logerror("Wrong data returned: '%s'. Expected was '%s'", resval, val);
		}


		connection()->releaseStatement(stmt);
		return m_success;
	}
};

	/* how this test case works
	 *
	 * 1. create table with one column (LONG ASCII)
	 * 2. prepare insert statement
	 * 3. bind parameter (DATA_AT_EXEC)
	 * 4. execute -> should return with NEED_DATA
	 * 5. nextParameter -> should return with NEED_DATA
	 * 6. putData several times (VERY OFTEN: put 1MB)
	 * 7. nextParameter -> should return with SQLDBC_OK
	 *
	 * Let's see if putting succeeds
	 *
	 */
class VeryLongLong : public SQLDBC_TestCase
{
protected:
	int colIndex;

	SQLDBC_Bool checkAmount(int numPuts, SQLDBC_Length numBytes)
	{
		loginfo("*************************************************");
		loginfo("*********   Check %dx%d Bytes", numPuts, numBytes);
		loginfo("*************************************************");

		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
		SQLDBC_Retcode rc;

		char val[200];
		sprintf(val, "____1____2____3____4____5____6____7____8____9___10___11___12___13___14___15___16___17___18___19___20"); // 100 signs
		// set appropriate zero-termination
		val[numBytes] = '\0';
		SQLDBC_Length ind_dae = SQLDBC_DATA_AT_EXEC;

		// prepare insert statement
		loginfo("prepare insert statement and bind the column");
		colIndex++;
		char strSQL[300];
		sprintf(strSQL, "INSERT INTO testtableLONG (i, t) VALUES (%d, ?)", colIndex);
		tc_errorcheck(stmt)->prepare(strSQL);
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->execute();
    
		SQLDBC_Int2 index;
		void *addr;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("nextParameter returned %d. Expected was %d [NEED_DATA]", rc, SQLDBC_NEED_DATA);
			return m_success;
		}

		// put data
		loginfo("putting data... [please wait]", numPuts, numBytes);
		int i;
		for (i=0; i<numPuts; i++) {
			tc_errorcheckrc(rc, stmt)->putData(val, &numBytes);
			if (rc != SQLDBC_OK) {
				logerror("putData returned %d. Expected was %d [SQLDBC_OK]", rc, SQLDBC_OK);
				return m_success;
			}
		}

		// call nextParameter
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_OK) {
			logerror("nextParameter returned %d. Expected was %d [SQLDBC_OK]", rc, SQLDBC_OK);
			return m_success;
		}

		// select statement
		loginfo("send select statement and get result set");
		sprintf(strSQL, "SELECT t FROM testtableLONG WHERE i=%d", colIndex);
		tc_errorcheckrc(rc, stmt)->prepare(strSQL);
		tc_errorcheckrc(rc, stmt)->execute();
		SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
		if (!rs) {
			logerror("No result was returned");
			return m_success;
		}
		// get result data
		tc_errorcheck(rs)->next();

		loginfo("fetch all the previously inserted data and compare it... [please wait]");
		int numGets = 0, ExpectedInd = numPuts * numBytes;
		char resval[200];
		numGets = 0;
		do {

			// init result buffer
			sprintf(resval, "x----x----x----x----x----x----x----x----x----x----x----x----x----x----x----x----x----x----x----x----"); // 100 signs
			// set appropriate zero-termination
			resval[numBytes] = '\0';

			SQLDBC_Length resind = SQLDBC_NTS;
			tc_errorcheckrc(rc, rs)->getObject(1, SQLDBC_HOSTTYPE_ASCII, &resval, &resind, numBytes, false);
			if (rc == SQLDBC_NOT_OK) {
				logerror("getObject returned %d. Expected was %d [SQLDBC_DATA_TRUNC] or %d [SQLDBC_OK]", rc, SQLDBC_DATA_TRUNC, SQLDBC_OK);
				logerror("Error code: %d. '%s'", rs->error().getErrorCode(), rs->error().getErrorText());
			}

			// check result data
			if (resind != ExpectedInd)
				logerror("Wrong indicator returned: %d. Expected was %d", resind, ExpectedInd);
			if (strcmp(resval, val))
				logerror("Wrong data returned: '%s'. Expected was '%s'", resval, val);

			// decrease expected indicator
			ExpectedInd = ExpectedInd - numBytes;

			numGets++;
		} while (rc == SQLDBC_DATA_TRUNC);

		// have we received the same amount of data we inserted?
		if (numGets != numPuts) {
			logerror("Amount fetched (%dx%d) differs from amount inserted (%dx%d)", numGets, numBytes, numPuts, numBytes);
		}

		connection()->releaseStatement(stmt);
		return m_success;
	}
public:
	VeryLongLong ()
		: SQLDBC_TestCase ("VeryLongLong")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		dropTable("testtableLONG");
		loginfo("create table");
		execSQL("CREATE TABLE testtableLONG (i INTEGER, t LONG ASCII)");

		colIndex = 0;

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("testtableLONG");
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		loginfo("check putting a lot of data into one LONG column...");

		checkAmount(35, 74);
		checkAmount(536, 53);
		checkAmount(1000, 100);

		return m_success;
	}
};

	/* how this test case works
	 *
	 * 1. create table with several columns (LONGs and INTEGERs)
	 * 2. prepare insert statement
	 * 3. bind parameters (DATA_AT_EXEC)
	 * 4. [NoExecute]: call putData and nextParameter without having an execute command
	 *    1. nextParameter -> NOT_OK
	 *    2. putData -> NOT_OK
	 * 5. [ExecuteTwice]: call execute twice
	 *    1. execute -> NEED_DATA
	 *    2. execute -> NOT_OK
	 * 6. [NoNextParameter]: call putData directly after having called execute()
	 *    1. putData -> NOT_OK
	 * 7. [NoPutForINTEGER]: no putData for INTEGER (call nextParameter twice)
	 *    1. nextParameter -> NEED_DATA
	 *    2. nextParameter -> NOT_OK since no data was put
	 *    3. putData -> NOT_OK (function seuence error)
	 * 8. [NoPutForLONG]: no putData for LONGS
	 *    1. execute -> should reset the scenario, NEED_DATA
	 *    2. nextParameter -> NEED_DATA for INTEGER-column
	 *    3. putData -> OK
	 *    4. nextParameter -> NEED_DATA for LONG-column
	 *    5. put zero bytes -> OK
	 *    6. nextParameter -> NEED_DATA for LONG-column
	 *    7. nextParameter -> NOT_OK since no data was put
	 * 9. [PutTwiceForINTEGER]: 
	 *    1. execute -> should reset the scenario, NEED_DATA
	 *    2. nextParameter -> NEED_DATA for INTEGER-column
	 *    3. putData -> OK
	 *    4. putData -> OK
	 *
	 */
class FunctionSequence : public SQLDBC_TestCase
{
protected:
	char val[200];
	int valint;

	// precondition: valid prepared statement object, statement already prepared, columns bound, no execute() yet
	SQLDBC_Bool NoExecute (SQLDBC_PreparedStatement *stmt)
	{
		loginfo("call nextParameter and putData without having had an execute() call...");

		// nextParameter
		SQLDBC_Int2 index;
		void *addr;
		stmt->clearError();
		tc_expectederrorcheck(stmt, -10201)->nextParameter(index, addr);

		// putData
		tc_expectederrorcheck(stmt, -10201)->putData(&valint, NULL);

		return m_success;
	}

	// precondition: valid prepared statement object, statement already prepared, columns bound, no execute() yet
	SQLDBC_Bool ExecuteTwice (SQLDBC_PreparedStatement *stmt)
	{
		loginfo("call execute() twice...");
		SQLDBC_Retcode rc;

		// execute
		tc_errorcheckrc(rc, stmt)->execute();
		if (rc != SQLDBC_NEED_DATA) {
			logerror("execute returned %d. Expected was %d [NEED_DATA]", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		tc_expectederrorcheck(stmt, -10200)->execute();

		return m_success;
	}

	// precondition: valid prepared statement object, statement already prepared, columns bound, execute() already called
	SQLDBC_Bool NoNextParameter (SQLDBC_PreparedStatement *stmt)
	{
		loginfo("call putData directly after having called execute()...");

		// putData
		tc_expectederrorcheck(stmt, -10201)->putData(&valint, NULL);

		return m_success;
	}

	// precondition: valid prepared statement object, statement already prepared, columns bound, execute() already called
	SQLDBC_Bool NoPutForINTEGER (SQLDBC_PreparedStatement *stmt)
	{
		loginfo("call nextParameter twice for INTEGER...");
		SQLDBC_Retcode rc;

		// next parameter twice
		SQLDBC_Int2 index;
		void *addr;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("nextParameter returned %d. Expected was %d [NEED_DATA]", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		tc_expectederrorcheck(stmt, -10304)->nextParameter(index, addr);
		// the error text should say that 'column (1) has not been bound'

		// put data
		tc_expectederrorcheck(stmt, -10201)->putData(&valint, NULL);

		return m_success;
	}

	// precondition: valid prepared statement object, statement already prepared, columns bound, no execute() yet or failed ops
	SQLDBC_Bool ZeroAndNoPutForLONG (SQLDBC_PreparedStatement *stmt)
	{
		loginfo("put zero bytes into LONG and call nextParameter twice for LONG...");
		SQLDBC_Retcode rc;

		// execute -> should reset the scenario
		tc_errorcheckrc(rc, stmt)->execute();
		if (rc != SQLDBC_NEED_DATA) {
			logerror("execute returned %d. Expected was %d [NEED_DATA]", rc, SQLDBC_NEED_DATA);
			return m_success;
		}

		// call nextParameter
		SQLDBC_Int2 index;
		void *addr;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("nextParameter returned %d. Expected was %d [NEED_DATA]", rc, SQLDBC_NEED_DATA);
			return m_success;
		}

		// put integer
		SQLDBC_Length l = sizeof(valint);
		tc_errorcheckrc(rc, stmt)->putData(&valint, &l);

		// call nextParameter
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("nextParameter returned %d. Expected was %d [NEED_DATA]", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		// put data (zero bytes)
		l = sizeof(val);
		rc = stmt->putData(val, &l); // expected to return SQLDBC_OK

		// call nextParameter twice
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr);
		if (rc != SQLDBC_NEED_DATA) {
			logerror("nextParameter returned %d. Expected was %d [NEED_DATA]", rc, SQLDBC_NEED_DATA);
			return m_success;
		}
		tc_expectederrorcheck(stmt, -10304)->nextParameter(index, addr);
		// the error text should say that 'column (3) has not been bound'

		return m_success;
	}

	// precondition: valid prepared statement object, statement already prepared, columns bound, no execute() yet or failed ops
	SQLDBC_Bool PutTwiceForINTEGER (SQLDBC_PreparedStatement *stmt)
	{
		loginfo("put data twice for INTEGER...");
		SQLDBC_Retcode rc;

		// execute -> should reset the scenario
		tc_errorcheckrc(rc, stmt)->execute(); // expected to return SQLDBC_NEED_DATA

		// call nextParameter
		SQLDBC_Int2 index;
		void *addr;
		tc_errorcheckrc(rc, stmt)->nextParameter(index, addr); // expected to return SQLDBC_NEED_DATA

		// put integer
		SQLDBC_Length l = sizeof(valint);
		tc_errorcheckrc(rc, stmt)->putData(&valint, &l); // expected to return SQLDBC_OK

		// put integer once again
		tc_expectederrorcheck(stmt, -10308)->putData(&valint, &l); 

		return m_success;
	}

public:
	FunctionSequence ()
		: SQLDBC_TestCase ("FunctionSequence")
	{}

	virtual SQLDBC_Bool prepare ()
	{
		dropTable("testtableNoPut");
		loginfo("create table");
		execSQL("CREATE TABLE testtableNoPut (i1 INTEGER, t1 LONG ASCII, i2 INTEGER, t2 LONG ASCII)");

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		dropTable("testtableNoPut");
		return m_success;
	}

	virtual SQLDBC_Bool run ()
	{
		loginfo("check calling nextParameter, putData in different orders...");

		SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
		SQLDBC_Retcode rc;

		sprintf(val, "Homer Simpson");
		valint = 42;
		SQLDBC_Length ind_dae = SQLDBC_DATA_AT_EXEC;

		// prepare insert statement
		loginfo("prepare insert statement and bind the columns");
		tc_errorcheck(stmt)->prepare("INSERT INTO testtableNoPut (i1, t1, i2, t2) VALUES (?, ?, 2, ?)");
		tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &valint, &ind_dae, sizeof(valint));
		tc_errorcheckrc(rc, stmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));
		tc_errorcheckrc(rc, stmt)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, val, &ind_dae, sizeof(val));

		// now the tests...
		m_success &= NoExecute(stmt);
		m_success &= ExecuteTwice(stmt);
		m_success &= NoNextParameter(stmt);
		m_success &= NoPutForINTEGER(stmt);
		m_success &= ZeroAndNoPutForLONG(stmt);
		m_success &= PutTwiceForINTEGER(stmt);

		return m_success;
	}
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new UpdateByIndex());
  testframe.addTestCase (new PieceWiseStmt());
  testframe.addTestCase (new UCS2());
  testframe.addTestCase (new DifferentTypes());
  testframe.addTestCase (new WhereStatement());
  testframe.addTestCase (new UpdateStatement());
  testframe.addTestCase (new InsertAndDisconnect());
  testframe.addTestCase (new SeveralLongs());
  testframe.addTestCase (new VeryLongLong());
  testframe.addTestCase (new FunctionSequence());
  if (testframe.run ()) 
    return 0;

  return 1;
}


