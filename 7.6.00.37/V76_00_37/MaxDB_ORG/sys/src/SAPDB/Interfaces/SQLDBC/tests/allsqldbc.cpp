/*!
  @file           allsqldbc.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check the linkage of all of SAP Database Connectivity
                  methode by calling all of them.
  @todo           Half done, not all functions are called.
                  
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

class ParameterMetaData : public SQLDBC_TestCase
{
public:
  ParameterMetaData ()
    : SQLDBC_TestCase ("ParameterMetaData")
  {}

  virtual SQLDBC_Bool run ()
  {
      SQLDBC_Environment env(runtime());
    
      SQLDBC_Connection *conn = env.createConnection();
      if (!conn) {
        logerror("createConnection does't not return a valid object\n");
        return 0;
      }
      SQLDBC_Retcode rc;
      tc_errorcheckrc(rc, conn)->connect(hostname(), 
                                    database(), 
                                    username(),
                                          password());
    
      SQLDBC_PreparedStatement *stmt = conn->createPreparedStatement();

      const char *szSQL = "SELECT 'ONE' from dual where '1' = ?";

      tc_errorcheckrc(rc, stmt)->prepare(szSQL);

      SQLDBC_ParameterMetaData *pinfo = stmt->getParameterMetaData();

      if (!pinfo) {
        logerror("No parameter metadata exsist.\n");
        return m_success;
      }
      
      SQLDBC_Int2 nparam = pinfo->getParameterCount();

      if (nparam != 1) {
        logerror("Wrong number of parameter (%d) in statment expected 1.\n", nparam);
        return m_success;
      }

      char cdata[20];
      SQLDBC_Length bufferLength=0;
      rc = pinfo->getParameterName(1, cdata,
                                                  SQLDBC_StringEncodingAscii,
                                                  sizeof(cdata),
                                                  &bufferLength);

      loginfo("paramtername = '%s'\n", cdata);

	  int mparam = pinfo->getParameterMode(1);
	  switch(mparam) {
	  case(0): {
		  loginfo("parametermode = 'parameterModeUnknown'\n");
		  break;
	  }
	  case(1): {
		  loginfo("parametermode = 'parameterModeIn'\n");
		  break;
	  }
	  case(2): {
		  loginfo("parametermode = 'parameterModeInOut'\n");
		  break;
	  }
	  case(4): {
		  loginfo("parametermode = 'parameterModeOut'\n");
		  break;
	  }
	  }//switch

	  int aparam = pinfo->isNullable(1);
	  switch(aparam) {
	  case(0): {
		  loginfo("parameternullbehavior = 'parameterNoNulls'\n");
		  break;
	  }
	  case(1): {
		  loginfo("parameternullbehavior = 'parameterNullable'\n");
		  break;
	  }
	  case(2): {
		  loginfo("parameternullbehavior = 'parameterNullableUnknown'\n");
		  break;
	  }
	  }//switch

    char pdata[10];
    sprintf(pdata, "1");

    SQLDBC_Length ind=SQLDBC_NTS;
    tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, pdata, &ind, sizeof(pdata), SQLDBC_TRUE);
      
    tc_errorcheckrc(rc, stmt)->execute();

    SQLDBC_ParameterMetaData* psmd = tc_errorcheck(stmt)->getParameterMetaData();
    
    loginfo("Check SQLDBC_PreparedStatement.getParameterCount()");
    SQLDBC_Int2 pc = psmd->getParameterCount();
    loginfo("ParamCount: %d", pc);
      
    loginfo("Check SQLDBC_PreparedStatement.getParameterLength()");
    SQLDBC_Int2 plength = psmd->getParameterLength(1);
    loginfo("ParameterLength: %d", plength);

    loginfo("Check SQLDBC_PreparedStatement.getParameterMode()");
    SQLDBC_ParameterMetaData::ParameterMode pMode = psmd->getParameterMode(1);
    loginfo("ParameterLength: %d", pMode);
    
    loginfo("Check SQLDBC_PreparedStatement.getParameterName()");
    char paramName[100]; SQLDBC_Length bufLen;
    rc = psmd->getParameterName(1, paramName, SQLDBC_StringEncodingAscii,
                                        sizeof(paramName),&bufLen);
    loginfo("ParameterName: %.*s", bufLen, paramName);

    loginfo("Check SQLDBC_PreparedStatement.getParameterType()");
    SQLDBC_SQLType ptype = psmd->getParameterType(1);
    loginfo("ParameterType: %d", ptype);
    
    loginfo("Check SQLDBC_PreparedStatement.getPhysicalLength()");
    SQLDBC_Int4 pPhysLen = psmd->getPhysicalLength(1);
    loginfo("PhysicalLength: %d", pPhysLen);

    loginfo("Check SQLDBC_PreparedStatement.getPrecision()");
    SQLDBC_Int4 pPrec = psmd->getPrecision(1);
    loginfo("Precision: %d", pPrec);

    loginfo("Check SQLDBC_PreparedStatement.getScale()");
    SQLDBC_Int4 pScale = psmd->getScale(1);
    loginfo("Scale: %d", pScale);

    loginfo("Check SQLDBC_PreparedStatement.isNullable()");
    SQLDBC_ParameterMetaData::ParameterNullBehavior pNullBeh = psmd->isNullable(1);
    loginfo("isNullable: %d", pNullBeh);

    tc_errorcheck(conn)->releaseStatement(stmt);
    env.releaseConnection(conn);

    return m_success;
  }

};

class ResultSetMetaData : public SQLDBC_TestCase
{
public:
  ResultSetMetaData ()
    : SQLDBC_TestCase ("SQLDBC_ResultSetMetaData")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    {tc_errorcheck(conn)->connect(hostname(), 
                                 database(), 
                                 username(),
                                  password());}
  
    SQLDBC_PreparedStatement *stmt = conn->createPreparedStatement();

    const char *szSQL = "SELECT dummy from dual";

    {tc_errorcheck(stmt)->prepare(szSQL);}

    SQLDBC_ResultSetMetaData *pinfo = stmt->getResultSetMetaData();

    if (!pinfo) {
      logerror("No column metadata exsist.\n");
      return m_success;
    }
    
    SQLDBC_Int2 ncol = pinfo->getColumnCount();

    if (ncol != 1) {
      logerror("Wrong number of parameter (%d) in statment expected 1.\n", ncol);
      return m_success;
    }

    char cdata[20];
    SQLDBC_Length bufferLength=0;
    SQLDBC_Retcode rc = pinfo->getColumnName(1, 
                                             cdata,
                                             SQLDBC_StringEncodingAscii,
                                             sizeof(cdata),
                                             &bufferLength);

    loginfo("column name = '%s'\n", cdata);

    SQLDBC_ResultSetMetaData* rsmd = tc_errorcheck(stmt)->getResultSetMetaData();
    
    loginfo("Check SQLDBC_ResultSetMetaData.getColumnCount()");
    SQLDBC_Int2 pc = rsmd->getColumnCount();
    loginfo("ColumnCount: %d", pc);
      
    loginfo("Check SQLDBC_ResultSetMetaData.getColumnLength()");
    SQLDBC_Int2 plength = rsmd->getColumnLength(1);
    loginfo("ColumnLength: %d", plength);

    loginfo("Check SQLDBC_ResultSetMetaData.getColumnName()");
    char paramName[100]; SQLDBC_Length bufLen;
    rc = rsmd->getColumnName(1, paramName, SQLDBC_StringEncodingAscii,
                                        sizeof(paramName),&bufLen);
    loginfo("ColumnName: %.*s", bufLen, paramName);

    loginfo("Check SQLDBC_ResultSetMetaData.getColumnType()");
    SQLDBC_SQLType ptype = rsmd->getColumnType(1);
    loginfo("ColumnType: %d", ptype);
    
    loginfo("Check SQLDBC_ResultSetMetaData.getPhysicalLength()");
    SQLDBC_Int4 pPhysLen = rsmd->getPhysicalLength(1);
    loginfo("PhysicalLength: %d", pPhysLen);

    loginfo("Check SQLDBC_ResultSetMetaData.getPrecision()");
    SQLDBC_Int4 pPrec = rsmd->getPrecision(1);
    loginfo("Precision: %d", pPrec);

    loginfo("Check SQLDBC_ResultSetMetaData.getScale()");
    SQLDBC_Int4 pScale = rsmd->getScale(1);
    loginfo("Scale: %d", pScale);

    loginfo("Check SQLDBC_ResultSetMetaData.isNullable()");
    SQLDBC_ResultSetMetaData::ColumnNullBehavior pNullBeh = rsmd->isNullable(1);
    loginfo("isNullable: %d", pNullBeh);

    loginfo("Check SQLDBC_ResultSetMetaData.isNullable()");
    SQLDBC_Bool pWrite = rsmd->isWritable(1);
    loginfo("isNullable: %s", (pWrite)?"TRUE":"FALSE");

    tc_errorcheck(conn)->releaseStatement(stmt);
    env.releaseConnection(conn);

    return m_success;
  }

};

class ResultSet : public SQLDBC_TestCase
{
public:
  ResultSet ()
    : SQLDBC_TestCase ("ResultSet")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    tc_errorcheck(conn)->connect(hostname(), 
                                 database(), 
                                 username(),
                                 password());
    
    SQLDBC_PreparedStatement *s1 = conn->createPreparedStatement();
    tc_errorcheck(s1)->prepare("DECLARE HUBBA CURSOR FOR SELECT * FROM DUAL");
    
    tc_errorcheck(s1)->execute();

    loginfo("Check SQLDBC_Statement.getCursorName() for named queries.");
    char cname[100]; SQLDBC_Length cLen = 0;
    SQLDBC_Retcode rc = tc_errorcheck(s1)->getCursorName(cname, SQLDBC_StringEncodingAscii, sizeof(cname), &cLen);  
    if (memcmp (cname, "HUBBA", cLen)) {
      logerror("Wrong cursor name %.*s. Should be HUBBA.", cLen, cname);
    } else {
      loginfo("Cursorname: %.*s", cLen, cname);
    }
    conn->releaseStatement(s1);
  
    SQLDBC_Statement *stmt = conn->createStatement();

    const char *szSQL = "SELECT tablename from tables";

    if (tc_errorcheck(conn)->isUnicodeDatabase()) {
      loginfo("Check SQLDBC_StringEncodingUCS2Native");
      SQLDBC_PreparedStatement *prpstmt = conn->createPreparedStatement();
      
      char cdata[128];
      SAPDB_sprintfUnicode (SAPDB_EncodingUCS2Native, cdata, sizeof(cdata), szSQL);
      tc_errorcheck(prpstmt)->prepare(cdata, SQLDBC_StringEncodingUCS2Native);
      conn->releaseStatement(prpstmt);
    }
    tc_errorcheck(stmt)->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
    tc_errorcheck(stmt)->execute(szSQL);

    SQLDBC_ResultSet *rs = stmt->getResultSet();

    loginfo("Check SQLDBC_ResultSet.absolute()");
    rc = tc_errorcheck(rs)->absolute(1);  
    
    loginfo("Check SQLDBC_ResultSet.bindColumn()");
    char value[100];
    SQLDBC_Length indikator;
    rc = tc_errorcheck(rs)->bindColumn(1, SQLDBC_HOSTTYPE_ASCII, (void*)value, &indikator,
                                      sizeof(value),SQLDBC_TRUE);  
    
    loginfo("Check SQLDBC_ResultSet.clearError()");
    tc_errorcheck(rs)->clearError();  

    loginfo("Check SQLDBC_ResultSet.clearWarnings()");
    tc_errorcheck(rs)->clearWarnings();  

    loginfo("Check SQLDBC_ResultSet.close()");
    tc_errorcheck(rs)->close();  

    loginfo("Check SQLDBC_ResultSet.error()");
    tc_errorcheck(stmt)->execute(szSQL);
    rs = tc_errorcheck(stmt)->getResultSet();
    SQLDBC_ErrorHndl eh = tc_errorcheck(rs)->error();  

    loginfo("Check SQLDBC_ResultSet.first()");
    rc = tc_errorcheck(rs)->first();  

    loginfo("Check SQLDBC_ResultSet.getObject()");
    tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value, &indikator,
                                      sizeof(value),SQLDBC_TRUE);

    SQLDBC_Length         StartPos = 1;
    loginfo("Check SQLDBC_ResultSet.getObject() with StartPos");
    tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value, &indikator,
                                      sizeof(value), StartPos, SQLDBC_TRUE);

    loginfo("Check SQLDBC_ResultSet.getResultCount()");
    SQLDBC_Int4 resc = tc_errorcheck(rs)->getResultCount();  
    loginfo("ResultCount: %d", resc);

    loginfo("Check SQLDBC_ResultSet.getResultSetMetaData()");
    SQLDBC_ResultSetMetaData* rsmd = tc_errorcheck(rs)->getResultSetMetaData();  
    
    loginfo("Check SQLDBC_ResultSet.getRowNumber()");
    SQLDBC_UInt4 rnum = tc_errorcheck(rs)->getRowNumber();  
    loginfo("RowNumber: %d", rnum);

    loginfo("Check SQLDBC_ResultSet.getRowSet()");
    SQLDBC_RowSet* rset = tc_errorcheck(rs)->getRowSet();
    
    loginfo("Check SQLDBC_ResultSet.getRowSetSize()");
    SQLDBC_UInt4 rsetSize = tc_errorcheck(rs)->getRowSetSize();  
    loginfo("RowNumber: %d", rsetSize);

    loginfo("Check SQLDBC_ResultSet.getStatement()");
    SQLDBC_Statement* rstmt = tc_errorcheck(rs)->getStatement();

    loginfo("Check SQLDBC_ResultSet.isUpdatable()");
    SQLDBC_Bool rbool = tc_errorcheck(rs)->isUpdatable();
    
    loginfo("Check SQLDBC_ResultSet.last()");
    rc = tc_errorcheck(rs)->last();
    
    loginfo("Check SQLDBC_ResultSet.next()");
    rc = tc_errorcheck(rs)->next();  
    
    loginfo("Check SQLDBC_ResultSet.previous()");
    rc = tc_errorcheck(rs)->previous();  

    loginfo("Check SQLDBC_ResultSet.relative()");
    rc = tc_errorcheck(rs)->relative(0); 
    
    loginfo("Check SQLDBC_ResultSet.setFetchSize");
    tc_errorcheck(rs)->setFetchSize(1);
    
    loginfo("Check SQLDBC_ResultSet.setRowSetSize()");
    tc_errorcheck(rs)->setRowSetSize(1); 
    
    loginfo("Check SQLDBC_ResultSet.warning()");
    SQLDBC_SQLWarning* warn = tc_errorcheck(rs)->warning();
    
    conn->releaseStatement(stmt);  
    env.releaseConnection(conn);

    return m_success;
  }

};

class RowSet : public SQLDBC_TestCase
{
public:
  RowSet ()
    : SQLDBC_TestCase ("RowSet")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    tc_errorcheck(conn)->connect(hostname(), 
                                 database(), 
                                 username(),
                                 password());
  
    SQLDBC_Statement *stmt = conn->createStatement();

    const char *szSQL = "SELECT tablename from tables";

    tc_errorcheck(stmt)->execute(szSQL);

    SQLDBC_RowSet *rset = stmt->getResultSet()->getRowSet();
    tc_errorcheck(rset);

    loginfo("Check RowSet.clearError()");
    tc_errorcheck(rset)->clearError();  
    
    loginfo("Check RowSet.clearWarnings()");
    tc_errorcheck(rset)->clearWarnings();
    
    loginfo("Check RowSet.error()");
    SQLDBC_ErrorHndl erhdl = tc_errorcheck(rset)->error();

    loginfo("Check RowSet.fetch()");
    SQLDBC_Retcode rc = tc_expectederrorcheck(rset, -10501)->fetch();  

    loginfo("Check RowSet.getObject()");
    char value[100];
    SQLDBC_Length indikator;
    rc = tc_expectederrorcheck(rset, -10501)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value, &indikator,
                                      sizeof(value),SQLDBC_TRUE);  

    loginfo("Check RowSet.getObject() with StartPos");
    SQLDBC_Length StartPos = 1;
    rc = tc_expectederrorcheck(rset, -10501)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value, &indikator,
                                      sizeof(value), StartPos, SQLDBC_TRUE);  

    loginfo("Check RowSet.getRowsAffected()");
    SQLDBC_Int4 rowsAffected = tc_errorcheck(rset)->getRowsAffected();  
    loginfo("RowsAffected: %d", rowsAffected);

    loginfo("Check RowSet.getRowStatus()");
    const SQLDBC_Int4 *rowstatus = tc_errorcheck(rset)->getRowStatus();

    loginfo("Check RowSet.setPos()");
    rc = tc_expectederrorcheck(rset, -10501)->setPos(1);  
 
    loginfo("Check RowSet.warning()");
    SQLDBC_SQLWarning* warn = tc_errorcheck(rset)->warning();  

    loginfo("Check RowSet.clearError()");
    tc_errorcheck(rset)->clearError();
    
    return m_success;  
    }
};

class UpdatableRowSet : public SQLDBC_TestCase
{
public:
  UpdatableRowSet ()
    : SQLDBC_TestCase ("UpdatableRowSet")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    tc_errorcheck(conn)->connect(hostname(), 
                                 database(), 
                                 username(),
                                 password());
  
    SQLDBC_Statement *stmt = conn->createStatement();

    execSQL ("DROP TABLE UPDATABLE");
    execSQL ("CREATE TABLE UPDATABLE ( I INTEGER )");
    execSQL ("INSERT INTO UPDATABLE values ( 1 )");
    execSQL ("COMMIT WORK");

    const char *szSQL = "SELECT * from UPDATABLE";

    tc_errorcheck(stmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_UPDATABLE);  
    tc_errorcheck(stmt)->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
    tc_errorcheck(stmt)->execute(szSQL);

    SQLDBC_UpdatableRowSet *rset = stmt->getResultSet()->getUpdatableRowSet();
    tc_errorcheck(rset);

    loginfo("Check RowSet.clearError()");
    tc_errorcheck(rset)->clearError();  
    
    loginfo("Check RowSet.clearWarnings()");
    tc_errorcheck(rset)->clearWarnings();
    
    loginfo("Check RowSet.error()");
    SQLDBC_ErrorHndl erhdl = tc_errorcheck(rset)->error();

    loginfo("Check RowSet.fetch()");
    SQLDBC_Retcode rc = tc_expectederrorcheck(rset, -10501)->fetch();  

    loginfo("Check RowSet.getObject()");
    char value[100];
    SQLDBC_Length indikator;
    rc = tc_expectederrorcheck(rset, -10501)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value, &indikator,
                                      sizeof(value),SQLDBC_TRUE);  

    loginfo("Check RowSet.getObject() with StartPos");
    SQLDBC_Length StartPos = 1;
    rc = tc_expectederrorcheck(rset, -10501)->getObject(1, SQLDBC_HOSTTYPE_ASCII, (void*)value, &indikator,
                                      sizeof(value), StartPos, SQLDBC_TRUE);  

    loginfo("Check RowSet.getRowsAffected()");
    SQLDBC_Int4 rowsAffected = tc_errorcheck(rset)->getRowsAffected();  
    loginfo("RowsAffected: %d", rowsAffected);

    loginfo("Check RowSet.getRowStatus()");
    const SQLDBC_Int4 *rowstatus = tc_errorcheck(rset)->getRowStatus();

    loginfo("Check RowSet.setPos()");
    rc = tc_expectederrorcheck(rset, -10501)->setPos(1);  
 
    loginfo("Check RowSet.insertAllRows()");
    rc = tc_expectederrorcheck(rset, -10510)->insertAllRows();  

    loginfo("Check RowSet.insertOneRow()");
    rc = tc_expectederrorcheck(rset, -10510)->insertOneRow();  

    loginfo("Check RowSet.insertOneRow()");
    rc = tc_expectederrorcheck(rset, -10510)->insertRow(1);  

    loginfo("Check RowSet.updateRow()");
    rc = tc_expectederrorcheck(rset, -10510)->updateRow(1);  

    loginfo("Check RowSet.deleteRow()");
    tc_errorcheck(rset)->deleteRow(1);  

    loginfo("Check RowSet.clearError()");
    tc_errorcheck(rset)->clearError();
    
    return m_success;  
    }
};

class Statement : public SQLDBC_TestCase
{
public:
  Statement ()
    : SQLDBC_TestCase ("Statement")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    conn->connect(hostname(), 
                  database(), 
                  username(),
                  password());
  
    SQLDBC_Statement *stmt = conn->createStatement();

    loginfo("Check SQLDBC_Statement.addBatch()");
    SQLDBC_Retcode rc = tc_errorcheck(stmt)->addBatch("PING");
  
    loginfo("Check SQLDBC_Statement.clearBatch()");
    tc_errorcheck(stmt)->clearBatch();  

    loginfo("Check SQLDBC_Statement.clearError()");
    tc_errorcheck(stmt)->clearError();  

    loginfo("Check SQLDBC_Statement.error()");
    SQLDBC::SQLDBC_ErrorHndl eh = tc_errorcheck(stmt)->error();
    
    loginfo("Check SQLDBC_Statement.execute(const char* sql)");
    rc = tc_errorcheck(stmt)->execute("PING"); 
     
    loginfo("Check SQLDBC_Statement.execute(");
    loginfo("            const char* sql,");
    loginfo("            const SQLDBC_StringEncoding encoding)");
    rc = tc_errorcheck(stmt)->execute("PING", SQLDBC_StringEncodingAscii); 

    loginfo("Check SQLDBC_Statement.execute(");
    loginfo("            const char* sql,");
    loginfo("            const SQLDBC_Length sqlLength,"); 
    loginfo("            const SQLDBC_StringEncoding encoding)");
    rc = tc_errorcheck(stmt)->execute("PING", 4, SQLDBC_StringEncodingAscii); 

    loginfo("Check SQLDBC_Statement.executeBatch()");
    rc = tc_errorcheck(stmt)->addBatch("PING");
    tc_errorcheck(stmt)->executeBatch();  

    loginfo("Check SQLDBC_Statement.getBatchSize()");
    SQLDBC_UInt4 bs = tc_errorcheck(stmt)->getBatchSize();  
    loginfo("BatchSize: %d", bs);
    
    loginfo("Check SQLDBC_Statement.getCursorName()");
    char cname[100]; SQLDBC_Length cLen=0;
    rc = tc_errorcheck(stmt)->getCursorName(cname, SQLDBC_StringEncodingAscii ,sizeof(cname),&cLen);  
    loginfo("Cursorname: %.*s", cLen, cname);
    
    loginfo("Check SQLDBC_Statement.getMaxRows()");
    SQLDBC_UInt4 mr = tc_errorcheck(stmt)->getMaxRows();  
    loginfo("MaxRows: %d", mr);

    loginfo("Check SQLDBC_Statement.getResultSet()");
    SQLDBC_ResultSet* rs = tc_errorcheck(stmt)->getResultSet();  

    loginfo("Check SQLDBC_Statement.getResultSetConcurrencyType()");
    const SQLDBC::SQLDBC_Statement::ConcurrencyType ct = tc_errorcheck(stmt)->getResultSetConcurrencyType();  
    loginfo("getResultSetConcurrencyType: %d", (int)ct);
    
    loginfo("Check SQLDBC_Statement.getResultSetType()");
    SQLDBC::SQLDBC_Statement::ResultSetType rt = tc_errorcheck(stmt)->getResultSetType();  
    loginfo("ResultSetType: %d", (int)rt);
    
    loginfo("Check SQLDBC_Statement.getRowsAffected()");
    SQLDBC_Int4 ra = tc_errorcheck(stmt)->getRowsAffected();  
    loginfo("RowsAffected: %d", ra);

    loginfo("Check SQLDBC_Statement.getRowStatus()");
    const SQLDBC_Int4* rStat = tc_errorcheck(stmt)->getRowStatus();  
    
    loginfo("Check SQLDBC_Statement.getTableName()");
    char tname[100]; SQLDBC_Length tLen=0;
    rc = tc_errorcheck(stmt)->getTableName(tname, SQLDBC_StringEncodingAscii ,sizeof(tname),&tLen);  
    loginfo("Cursorname: %.*s", tLen, tname);
    
    loginfo("Check SQLDBC_Statement.isQuery()");
    SQLDBC_Bool isQ = tc_errorcheck(stmt)->isQuery();  
    loginfo("isQuery: %s", (isQ)?"TRUE":"FALSE");
    
    loginfo("Check SQLDBC_Statement.setCursorName()");
    tc_errorcheck(stmt)->setCursorName("Homer_Simpson", sizeof("Homer_Simpson"), SQLDBC_StringEncodingAscii);  
    
    loginfo("Check SQLDBC_Statement.setMaxRows()");
    tc_errorcheck(stmt)->setMaxRows(1);  

    loginfo("Check SQLDBC_Statement.setResultSetConcurrencyType()");
    tc_errorcheck(stmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_UPDATABLE);  
    tc_errorcheck(stmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_READ_ONLY);  
    
    loginfo("Check SQLDBC_Statement.setResultSetType()");
    tc_errorcheck(stmt)->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);  
    tc_errorcheck(stmt)->setResultSetType(SQLDBC_Statement::SCROLL_INSENSITIVE);  

    loginfo("Check SQLDBC_Statement.setResultSetConcurrencyType()");
    SQLDBC_SQLWarning* warn = tc_errorcheck(stmt)->warning(); 

    conn->releaseStatement(stmt);
    env.releaseConnection(conn);

    return m_success;
  }

};

class PreparedStatement : public SQLDBC_TestCase
{
public:
  PreparedStatement ()
    : SQLDBC_TestCase ("PreparedStatement")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    conn->connect(hostname(), 
                  database(), 
                  username(),
                  password());
  
    SQLDBC_PreparedStatement *pstmt = conn->createPreparedStatement();
    
    loginfo("Check SQLDBC_Statement.addBatch()");
        SQLDBC_Retcode rc = tc_expectederrorcheck(pstmt, -10220)->addBatch("PING");

    loginfo("Check SQLDBC_PreparedStatement.bindParameter()");
    rc = tc_errorcheck(pstmt)->prepare("SELECT * FROM DUAL WHERE 1=?");
    SQLDBC_Int4 val = 1; SQLDBC_Length ind = 0;
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_BINARY, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_DOUBLE, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_FLOAT, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_GUID, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT1 , &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT2, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT8, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCDATE, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCNUMERIC, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIME, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ODBCTIMESTAMP, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_PARAMETER_NOTSET, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UCS2, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UCS2_NATIVE, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UCS2_SWAPPED, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UINT1, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UINT2, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UINT4, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UINT8, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_USERDEFINED, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_UTF8, &val, &ind, sizeof(val), SQLDBC_TRUE);
    rc = tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &val, &ind, sizeof(val), SQLDBC_TRUE);

    loginfo("Check SQLDBC_PreparedStatement.bindParameterAddr()");
    SQLDBC_Int4 * valP = &val;
    tc_errorcheck(pstmt)->bindParameterAddr(1, SQLDBC_HOSTTYPE_INT4, &valP, &ind, sizeof(val), SQLDBC_TRUE);  

    loginfo("Check SQLDBC_PreparedStatement.addBatch()");
    rc = pstmt->addBatch("PING");
    tc_expectederrorcheck(pstmt,-10220);
  
    loginfo("Check SQLDBC_PreparedStatement.clearBatch()");
    tc_expectederrorcheck(pstmt,-10220)->clearBatch();  

    loginfo("Check SQLDBC_PreparedStatement.clearError()");
    tc_errorcheck(pstmt)->clearError();  

    loginfo("Check SQLDBC_PreparedStatement.clearParameters()");
    tc_errorcheck(pstmt)->clearParameters();  

    loginfo("Check SQLDBC_PreparedStatement.clearWarnings()");
    tc_errorcheck(pstmt)->clearWarnings();  

    loginfo("Check SQLDBC_PreparedStatement.error()");
    SQLDBC::SQLDBC_ErrorHndl eh = tc_errorcheck(pstmt)->error();
    
    loginfo("Check SQLDBC_PreparedStatement.execute(const char* sql)");
    tc_errorcheck(pstmt)->bindParameterAddr(1, SQLDBC_HOSTTYPE_INT4, &valP, &ind, sizeof(val), SQLDBC_TRUE);  
    rc = tc_errorcheck(pstmt)->execute(); 
     
    loginfo("Check SQLDBC_PreparedStatement.executeBatch()");
    tc_errorcheck(pstmt)->executeBatch();  

    loginfo("Check SQLDBC_PreparedStatement.getBatchSize()");
    SQLDBC_UInt4 bs = tc_errorcheck(pstmt)->getBatchSize();  
    loginfo("BatchSize: %d", bs);
    
    loginfo("Check SQLDBC_PreparedStatement.getCursorName()");
    char cname[100]; SQLDBC_Length cLen=0;
    rc = tc_errorcheck(pstmt)->getCursorName(cname, SQLDBC_StringEncodingAscii ,sizeof(cname),&cLen);  
    loginfo("Cursorname: %.*s", cLen, cname);
    
    loginfo("Check SQLDBC_PreparedStatement.getMaxRows()");
    SQLDBC_UInt4 mr = tc_errorcheck(pstmt)->getMaxRows();  
    loginfo("MaxRows: %d", mr);

    loginfo("Check SQLDBC_PreparedStatement.getParameterMetaData()");
    SQLDBC_ParameterMetaData* pm = tc_errorcheck(pstmt)->getParameterMetaData();  

    loginfo("Check SQLDBC_PreparedStatement.getResultSet()");
    SQLDBC_ResultSet* rs = tc_errorcheck(pstmt)->getResultSet();  

    loginfo("Check SQLDBC_PreparedStatement.getResultSetConcurrencyType()");
    const SQLDBC::SQLDBC_Statement::ConcurrencyType ct = tc_errorcheck(pstmt)->getResultSetConcurrencyType();  
    loginfo("getResultSetConcurrencyType: %d", (int)ct);
    
    loginfo("Check SQLDBC_PreparedStatement.getResultSetType()");
    SQLDBC::SQLDBC_Statement::ResultSetType rt = tc_errorcheck(pstmt)->getResultSetType();  
    loginfo("ResultSetType: %d", (int)rt);
    
    loginfo("Check SQLDBC_PreparedStatement.getRowsAffected()");
    SQLDBC_Int4 ra = tc_errorcheck(pstmt)->getRowsAffected();  
    loginfo("RowsAffected: %d", ra);

    loginfo("Check SQLDBC_PreparedStatement.getRowStatus()");
    const SQLDBC_Int4* rStat = tc_errorcheck(pstmt)->getRowStatus();  
    
    loginfo("Check SQLDBC_PreparedStatement.getTableName()");
    char tname[100]; SQLDBC_Length tLen=0;
    rc = tc_errorcheck(pstmt)->getTableName(tname, SQLDBC_StringEncodingAscii ,sizeof(tname),&tLen);  
    loginfo("Cursorname: %.*s", tLen, tname);
    
    loginfo("Check SQLDBC_PreparedStatement.isQuery()");
    SQLDBC_Bool isQ = tc_errorcheck(pstmt)->isQuery();  
    loginfo("isQuery: %s", (isQ)?"TRUE":"FALSE");
    
    loginfo("Check SQLDBC_PreparedStatement.nextParameter()");
    SQLDBC_Int2 index; void* addr;
    rc = tc_expectederrorcheck(pstmt, -10201)->nextParameter(index, addr);  
    
    loginfo("Check SQLDBC_PreparedStatement.prepare()");
    rc = tc_errorcheck(pstmt)->prepare("SELECT * FROM DUAL");
    rc = tc_errorcheck(pstmt)->prepare("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
    rc = tc_errorcheck(pstmt)->prepare("SELECT * FROM DUAL", sizeof("SELECT * FROM DUAL") ,SQLDBC_StringEncodingAscii);

    loginfo("Check SQLDBC_PreparedStatement.putData()");
    char* putDataVal = (char*)"Homer Simpson"; SQLDBC_Length putDataLen= sizeof ("Homer Simpson");
    rc = tc_expectederrorcheck(pstmt, -10201)->putData((void*)putDataVal, &putDataLen);  

    loginfo("Check SQLDBC_PreparedStatement.setBatchSize()");
    rc = tc_errorcheck(pstmt)->setBatchSize(1);

    loginfo("Check SQLDBC_PreparedStatement.setBindingType()");
    rc = tc_errorcheck(pstmt)->setBindingType(0);

    loginfo("Check SQLDBC_PreparedStatement.setCursorName()");
    tc_errorcheck(pstmt)->setCursorName("Homer_Simpson", sizeof("Homer_Simpson"), SQLDBC_StringEncodingAscii);  
    
    loginfo("Check SQLDBC_PreparedStatement.setMaxRows()");
    tc_errorcheck(pstmt)->setMaxRows(1);  

    loginfo("Check SQLDBC_PreparedStatement.setResultSetConcurrencyType()");
    tc_errorcheck(pstmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_UPDATABLE);  
    tc_errorcheck(pstmt)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_READ_ONLY);  
    
    loginfo("Check SQLDBC_PreparedStatement.setResultSetType()");
    tc_errorcheck(pstmt)->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);  
    tc_errorcheck(pstmt)->setResultSetType(SQLDBC_Statement::SCROLL_INSENSITIVE);  

    loginfo("Check SQLDBC_PreparedStatement.setResultSetConcurrencyType()");
    SQLDBC_SQLWarning* warn = tc_errorcheck(pstmt)->warning(); 

    conn->releaseStatement(pstmt);
    env.releaseConnection(conn);

    return m_success;
  }

};

class Connection : public SQLDBC_TestCase
{
public:
  Connection ()
    : SQLDBC_TestCase ("Connection")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    SQLDBC_Retcode rc = conn->connect(hostname(), 
                                      database(), 
                                      "UNKNOWN",
                                      "UNKNOWN");

    if (rc != SQLDBC_NOT_OK) {
      logerror("connect doesn't return SQLDBC_NOT_OK\n");
      return 0;
    }

    if(!conn->error()) {
      logerror("conn->error() doesn't returns a true value\n");
      return 0;
    }
    
    SQLDBC_ErrorHndl error = conn->error();

    SQLDBC_Int4 errorcode = error.getErrorCode();

    if(errorcode != -4008) {
      logerror("Wrong errorcode(%d) returned expected -4008\n", errorcode);
      return 0;
    }
    
    const char* szErrorText = error.getErrorText();

    if (!szErrorText) {
      logerror("Wrong getErrorText returns a NULL value\n");
      return 0;
    }

    const char* szSQLState = error.getSQLState();

    if (!szSQLState) {
      logerror("Wrong getSQLState returns a NULL value\n");
      return 0;
    }

    SQLDBC_SQLWarning *warning = conn->warning();
    if (warning) {
      logerror("no warning should return\n");
      return 0;
    }

    /**************************/

    loginfo("Check connect to %s", database());
    rc = tc_errorcheck(conn)->connect(hostname(), database(), username(), password()); 

    loginfo("Check SQLDBC_Connection.cancel()");
    rc = conn->cancel();
    if (rc != SQLDBC_NO_DATA_FOUND)
      logerror("Check SQLDBC_Connection.cancel()");      

    loginfo("Check SQLDBC_Connection.clearError()");
    tc_errorcheck(conn)->clearError();

    loginfo("Check SQLDBC_Connection.clearWarnings()");
    tc_errorcheck(conn)->clearWarnings();

    loginfo("Check SQLDBC_Connection.close()");
    tc_errorcheck(conn)->close();
    
    rc = tc_errorcheck(conn)->connect(hostname(), database(), username(), password()); 

    loginfo("Check SQLDBC_Connection.commit()");
    rc = tc_errorcheck(conn)->commit();
    loginfo("Check SQLDBC_Connection.commitWorkRelease()");
    tc_errorcheck(conn)->commitWorkRelease();

    rc = tc_errorcheck(conn)->connect(hostname(), database(), username(), password()); 

    loginfo("Check SQLDBC_Connection.createPreparedStatement()");
    SQLDBC_PreparedStatement* ps = tc_errorcheck(conn)->createPreparedStatement();

    loginfo("Check SQLDBC_Connection.createStatement()");
    SQLDBC_Statement*         st = tc_errorcheck(conn)->createStatement();

    loginfo("Check SQLDBC_Connection.createStatement()");
    tc_errorcheck(conn)->disconnect();
    
    rc = tc_errorcheck(conn)->connect(hostname(), database(), username(), password()); 

    loginfo("Check SQLDBC_Connection.error()");
    SQLDBC_ErrorHndl err = tc_errorcheck(conn)->error();
    
    loginfo("Check SQLDBC_Connection.getAutoCommit()");
    SQLDBC_Bool ac = tc_errorcheck(conn)->getAutoCommit();
    
    loginfo("Check SQLDBC_Connection.getDateTimeFormat()");
    SQLDBC_DateTimeFormat::Format dtf = tc_errorcheck(conn)->getDateTimeFormat();
    
    loginfo("Check SQLDBC_Connection.getKernelVersion()");
    SQLDBC_Int4 version = tc_errorcheck(conn)->getKernelVersion();
    
    loginfo("Check SQLDBC_Connection.getTransactionIsolation()");
    SQLDBC_Int4 isolevel = tc_errorcheck(conn)->getTransactionIsolation();
    
    loginfo("Check SQLDBC_Connection.isConnected()");
    SQLDBC_Bool ic = tc_errorcheck(conn)->isConnected();
    
    loginfo("Check SQLDBC_Connection.isUnicodeDatabase()");
    SQLDBC_Bool iu = tc_errorcheck(conn)->isUnicodeDatabase();

    loginfo("Check SQLDBC_Connection.releaseStatement()");
    tc_errorcheck(conn)->releaseStatement(ps);
    tc_errorcheck(conn)->releaseStatement(st);
    
    loginfo("Check SQLDBC_Connection.rollback()");
    tc_errorcheck(conn)->rollback();
    
    loginfo("Check SQLDBC_Connection.setAutoCommit()");
    tc_errorcheck(conn)->setAutoCommit(SQLDBC_FALSE);
    
    loginfo("Check SQLDBC_Connection.setSQLMode()");
    tc_errorcheck(conn)->setSQLMode(SQLDBC_INTERNAL);
    
    loginfo("Check SQLDBC_Connection.setTransactionIsolation()");
    tc_errorcheck(conn)->setTransactionIsolation(1);
    
    loginfo("Check SQLDBC_Connection.warning()");
    SQLDBC::SQLDBC_SQLWarning* wa = tc_errorcheck(conn)->warning();
    
    env.releaseConnection(conn);

    return m_success;
  }

};

class Environment : public SQLDBC_TestCase
{
public:
  Environment ()
    : SQLDBC_TestCase ("Environment")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }
    env.releaseConnection(conn);
    
    conn = env.createConnection(env.getAllocator());
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }   
    env.releaseConnection(conn);

    const char *SDKVersion = env.getSDKVersion();
    printf("getSDKVersion     = %s\n", SDKVersion);

    const char *LibraryVersion = env.getLibraryVersion();
    printf("getLibraryVersion = %s\n", LibraryVersion);


    conn = env.createConnection(env.getAllocator());
    tc_errorcheck(conn)->connect(hostname(), database(), username(), password());
    env.releaseConnection(conn);
    return m_success;
  }
};

class ErrorHndl : public SQLDBC_TestCase
{
public:
  ErrorHndl ()
    : SQLDBC_TestCase ("ErrorHndl")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    conn->connect(hostname(), 
                  database(), 
                  "WRONGLOGIN",
                  "NOPASSWORD");
  
    SQLDBC_ErrorHndl eh = conn->error();

    loginfo("Check ErrorHndl.getErrorCode()");
    SQLDBC_Int4 ec = eh.getErrorCode();
    loginfo("ErrorCode: %d", ec);

    loginfo("Check ErrorHndl.getErrorText()");
    const char* et = eh.getErrorText();
    loginfo("ErrorText: %s", et);

    loginfo("Check ErrorHndl.getSQLState()");
    const char* sqlstate = eh.getSQLState();
    loginfo("SQLState: %s", sqlstate);
    
    return m_success;   
  }
};

class SQLWarning : public SQLDBC_TestCase
{
public:
  SQLWarning ()
    : SQLDBC_TestCase ("SQLWarning")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Environment env(runtime());
  
    SQLDBC_Connection *conn = env.createConnection();
    if (!conn) {
      logerror("createConnection does't not return a valid object\n");
      return 0;
    }

    conn->connect(hostname(), 
                  database(), 
                  "WRONGLOGIN",
                  "NOPASSWORD");
  
    SQLDBC_SQLWarning* warn = conn->warning();

    loginfo("Not Implemented so far");
    //SQLDBC_Int4 ec = warn->getWarningCode();
    //loginfo("ErrorCode: %d", ec);

    //loginfo("Check SQLWarning.getErrorText()");
    //const char* et = eh.getErrorText();
    //loginfo("ErrorText: %s", et);

    //loginfo("Check SQLWarning.getSQLState()");
    //const char* sqlstate = eh.getSQLState();
    //loginfo("SQLState: %s", sqlstate);
    return m_success;   
  }
};

class ConnectProperties : public SQLDBC_TestCase
{
public:
  ConnectProperties ()
    : SQLDBC_TestCase ("ConnectProperties")
  {}

  virtual SQLDBC_Bool run ()
  {
  
    SQLDBC_ConnectProperties *cprop = new SQLDBC_ConnectProperties();

    loginfo("Check ConnectProperties.getProperty()");
    const char* homer = cprop->getProperty("HOMER");
    const char* simpson = cprop->getProperty("SIMPSON","BART");
    loginfo("getProperty: %s", (char*)simpson);
    
    loginfo("Check ConnectProperties.getProperty()");
    cprop->setProperty("HOMER","SIMPSON");

    SQLDBC_ConnectProperties *cprop2 = new SQLDBC_ConnectProperties(*cprop);
    return m_success;   
  }
};
    
int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new ConnectProperties());
  testframe.addTestCase (new Connection());
//class SQLDBC_DateTimeFormat     - no methods for testing
  testframe.addTestCase (new Environment());
  testframe.addTestCase (new ErrorHndl());
  testframe.addTestCase (new ParameterMetaData());
  testframe.addTestCase (new PreparedStatement());
  testframe.addTestCase (new ResultSet());
  testframe.addTestCase (new ResultSetMetaData());
  testframe.addTestCase (new RowSet());
  testframe.addTestCase (new UpdatableRowSet());
  testframe.addTestCase (new SQLWarning());
//class SQLDBC_StringEncodingType - no methods for testing
  testframe.addTestCase (new Statement());
  
  if (testframe.run ()) 
    return 0;

  return 1;
}


