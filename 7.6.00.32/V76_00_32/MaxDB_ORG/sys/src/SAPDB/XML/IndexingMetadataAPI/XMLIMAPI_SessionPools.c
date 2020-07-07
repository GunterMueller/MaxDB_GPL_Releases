/****************************************************************************

  module      : XMLIMAPI_SessionPools.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-21  14:45
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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

	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_SessionPools.h"
	#include "SAPDBCommon/SAPDB_string.h"


	#define SQL_GET_SESSIONPOOLS \
			"SELECT	\
				S.\"ID\", \
				S.\"NAME\",\
				S.\"DESCRIPTION\" \
			 FROM \
				\"XML_SESSIONPOOL\" S\
			 WHERE \
				S.\"ID\" <> X'000000000000000000000000000000000000000000000000'\
			 ORDER BY S.\"NAME\""

	#define SQL_GET_ASSIGNMENT \
			"SELECT \
				COUNT(I.DOCUMENTSTORE), \
				COUNT(I.INDEXSTORE)\
			 FROM \
				XML_INDEXINGSERVICEDESC I \
			 WHERE \
				I.DOCUMENTSTORE = X'?' OR\
				I.INDEXSTORE = X'?'"

	#define SQL_GET_SESSIONPOOL \
			"SELECT	\
				S.\"ID\",\
				S.\"NAME\",\
				S.\"DESCRIPTION\",\
				S.\"DBNODE\",\
				S.\"DBNAME\",\
				S.\"DBUSER\",\
				S.\"DBPASSWORD\",\
				S.\"TRACEFILE\"\
			 FROM \
				\"XML_SESSIONPOOL\" S\
			 WHERE \
				((S.\"ID\" = ?))"

	#define SQL_INSERT_SESSIONPOOL \
			"insert into XML_SESSIONPOOL \
				set Id = ?,\
					Name = ?,\
					Description = ?,\
					DbNode = ?,\
					DbName = ?,\
					DbUser = ?,\
					DbPassword = ?,\
					TraceFile = ?"

	#define SQL_UPDATE_SESSIONPOOL \
			"update XML_SESSIONPOOL \
				set Description = ?,\
					DbNode = ?,\
					DbName = ?,\
					DbUser = ?,\
					DbPassword = ?,\
					TraceFile = ?\
				where \
					Id = ?"

	#define SQL_DELETE_SESSIONPOOL \
			"delete XML_SESSIONPOOL \
			 where Id = ?"

	#define SQL_CHECK_USAGE_SESSIONPOOL \
			"SELECT	\
				I.\"NAME\" \
			 FROM \
				\"XML_INDEXINGSERVICEDESC\" I\
			 WHERE \
				I.DOCUMENTSTORE = ? OR \
				I.INDEXSTORE = ?"

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolCreate(XMLIMAPI_Handle			handle,
											   XMLIMAPI_SessionPool	*sessionpool)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_ErrorStateString  	state = "";
		XMLIMAPI_ErrorTextString   	text = "";
		XMLIMAPI_ErrorCode         	nativeODBCCode = 0;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtSessionPoolCreate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_INSERT_SESSIONPOOL, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.PoolName, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.description, 513, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbNode, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,5,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbName, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,6,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbUser, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,7,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbPassword, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,8,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.TraceFile, 257, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtSessionPoolCreate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtSessionPoolCreate, SQL_CLOSE );
		}

		/* set values of parameters */
		/* - get unique id for index */
		if (!XMLIMAPI_IdGetNext(handle,handle->stmtSessionPoolParam.PoolID)) {
			return XMLIMAPI_False;
		}
		XMLIMAPI_IdAsString(handle->stmtSessionPoolParam.PoolID,sessionpool->PoolID);

		strcpy(handle->stmtSessionPoolParam.PoolName,sessionpool->PoolName);
		strcpy(handle->stmtSessionPoolParam.description,sessionpool->description);
		strcpy(handle->stmtSessionPoolParam.DbNode,sessionpool->DbNode);
		strcpy(handle->stmtSessionPoolParam.DbName,sessionpool->DbName);
		strcpy(handle->stmtSessionPoolParam.DbUser,sessionpool->DbUser);
		strcpy(handle->stmtSessionPoolParam.DbPassword,sessionpool->DbPassword);
		strcpy(handle->stmtSessionPoolParam.TraceFile,sessionpool->TraceFile);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtSessionPoolCreate)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtSessionPoolCreate, state, text, &nativeODBCCode);
			if (nativeODBCCode==250) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_ALLREADY_EXISTS,
							 XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_ALLREADY_EXISTS);
			} else {
				addSQLErrorItem(handle, handle->stmtSessionPoolCreate, rc);
				SQLFreeStmt( handle->stmtSessionPoolCreate, SQL_DROP );
			}
			handle->stmtSessionPoolCreate = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolUpdate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolUpdate(XMLIMAPI_Handle			handle,
											   XMLIMAPI_SessionPool	*sessionpool)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtSessionPoolUpdate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_UPDATE_SESSIONPOOL, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,7,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.description, 513, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbNode, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbName, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbUser, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,5,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.DbPassword, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,6,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtSessionPoolParam.TraceFile, 257, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtSessionPoolUpdate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtSessionPoolUpdate, SQL_CLOSE );
		}

		/* set values of parameters */
		XMLIMAPI_IdStringAsId(sessionpool->PoolID,handle->stmtSessionPoolParam.PoolID);
		strcpy(handle->stmtSessionPoolParam.description,sessionpool->description);
		strcpy(handle->stmtSessionPoolParam.DbNode,sessionpool->DbNode);
		strcpy(handle->stmtSessionPoolParam.DbName,sessionpool->DbName);
		strcpy(handle->stmtSessionPoolParam.DbUser,sessionpool->DbUser);
		strcpy(handle->stmtSessionPoolParam.DbPassword,sessionpool->DbPassword);
		strcpy(handle->stmtSessionPoolParam.TraceFile,sessionpool->TraceFile);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtSessionPoolUpdate)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtSessionPoolUpdate, rc);
			SQLFreeStmt( handle->stmtSessionPoolUpdate, SQL_DROP );
			handle->stmtSessionPoolUpdate = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolDelete
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolDelete(XMLIMAPI_Handle			handle,
											   XMLIMAPI_SessionPool	*sessionpool)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtSessionPoolDelete) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_DELETE_SESSIONPOOL, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtSessionPoolDelete = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtSessionPoolDelete, SQL_CLOSE );
		}

		/* set values of parameters */
		XMLIMAPI_IdStringAsId(sessionpool->PoolID,handle->stmtSessionPoolParam.PoolID);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtSessionPoolDelete)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtSessionPoolDelete, rc);
			SQLFreeStmt( handle->stmtSessionPoolDelete, SQL_DROP );
			handle->stmtSessionPoolDelete = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolCheckUsage
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolCheckUsage(XMLIMAPI_Handle			handle,
												   XMLIMAPI_SessionPool	*sessionpool)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtSessionPoolCheckIS) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_CHECK_USAGE_SESSIONPOOL, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.PoolName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtSessionPoolCheckIS = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtSessionPoolCheckIS, SQL_CLOSE );
		}

		/* set values of parameters */
		XMLIMAPI_IdStringAsId(sessionpool->PoolID,handle->stmtSessionPoolParam.PoolID);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtSessionPoolCheckIS)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtSessionPoolCheckIS, rc);
			SQLFreeStmt( handle->stmtSessionPoolCheckIS, SQL_DROP );
			handle->stmtSessionPoolCheckIS = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		} else {
			rc = SQLFetch(handle->stmtSessionPoolCheckIS);
				
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {
				/* there is at least one indexing service using this session pool */
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_USED,
							 XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_USED);
				return SAPDB_FALSE;
			} else if (rc == SQL_NO_DATA) {
				/* no values of that index stored with any document -> delete is ok */
				Commit(handle);
				return SAPDB_TRUE;
			} else {
				/* there is at least one indexing service using this session pool */
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_INTERNAL,
							 XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_INTERNAL);
				return SAPDB_FALSE;
			}
		}
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolGetAssignment
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGetAssignment(XMLIMAPI_Handle			handle,
												    XMLIMAPI_SessionPool	*sessionpool)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtSessionPoolGetAssign) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_ASSIGNMENT, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.Assignments), 
							10, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtSessionPoolGetAssign = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtSessionPoolGetAssign, SQL_CLOSE );
		}

		/* set values of parameters */
		XMLIMAPI_IdStringAsId(sessionpool->PoolID,handle->stmtSessionPoolParam.PoolID);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtSessionPoolGetAssign)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtSessionPoolGetAssign, rc);
			SQLFreeStmt( handle->stmtSessionPoolGetAssign, SQL_DROP );
			handle->stmtSessionPoolGetAssign = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		} else {
			rc = SQLFetch(handle->stmtSessionPoolGetAssign);
				
			if ((rc != SQL_SUCCESS) &&
				(rc != SQL_SUCCESS_WITH_INFO)) {
				/* there is at least one indexing service using this session pool */
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_INTERNAL,
							 XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_INTERNAL);
				return SAPDB_FALSE;
			} else {
				Commit(handle);
				return SAPDB_TRUE;
			}
		}
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_SessionPoolGet
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGet(XMLIMAPI_Handle		handle,
											XMLIMAPI_Id			poolId,
											XMLIMAPI_SessionPool	*sessionPool)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtSessionPoolGet) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_SESSIONPOOL, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtSessionPoolParam.PoolID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.PoolID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.PoolName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.description), 
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.DbNode), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.DbName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 6, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.DbUser), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 7, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.DbPassword), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 8, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.TraceFile), 
							257, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtSessionPoolGet = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtSessionPoolGet, SQL_CLOSE );
		}

		/* set document class id */
		SAPDB_memcpy(handle->stmtSessionPoolParam.PoolID,poolId,XMLIMAPI_MAX_ID_LEN);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtSessionPoolGet)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtSessionPoolGet, rc);
			SQLFreeStmt( handle->stmtSessionPoolGet, SQL_DROP );
			handle->stmtSessionPoolGet = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtSessionPoolGet);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(sessionPool->PoolID,
					   handle->stmtSessionPoolCol.PoolID);
				strcpy(sessionPool->PoolName,
					   handle->stmtSessionPoolCol.PoolName);
				strcpy(sessionPool->description,
					   handle->stmtSessionPoolCol.description);
				strcpy(sessionPool->DbNode,
					   handle->stmtSessionPoolCol.DbNode);
				strcpy(sessionPool->DbName,
					   handle->stmtSessionPoolCol.DbName);
				strcpy(sessionPool->DbUser,
					   handle->stmtSessionPoolCol.DbUser);
				strcpy(sessionPool->DbPassword,
					   handle->stmtSessionPoolCol.DbPassword);
				strcpy(sessionPool->TraceFile,
					   handle->stmtSessionPoolCol.TraceFile);

				/* get the assignment count */
				XMLIMAPI_SessionPoolGetAssignment(handle,sessionPool);
				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_SESSIONPOOL,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_SESSIONPOOL);
				} else {
					addSQLErrorItem(handle, handle->stmtSessionPoolGet, rc);
					SQLFreeStmt( handle->stmtSessionPoolGet, SQL_DROP );
					handle->stmtSessionPoolGet = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_SessionPoolGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGetFirst(XMLIMAPI_Handle		handle,
												 XMLIMAPI_SessionPool	*sessionPool)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtSessionPoolGetList) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_SESSIONPOOLS, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.PoolID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.PoolName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtSessionPoolCol.description), 
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtSessionPoolGetList = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtSessionPoolGetList, SQL_CLOSE );
		}

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtSessionPoolGetList)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtSessionPoolGetList, rc);
			SQLFreeStmt( handle->stmtSessionPoolGetList, SQL_DROP );
			handle->stmtSessionPoolGetList = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtSessionPoolGetList);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(sessionPool->PoolID,
					   handle->stmtSessionPoolCol.PoolID);
				strcpy(sessionPool->PoolName,
					   handle->stmtSessionPoolCol.PoolName);
				strcpy(sessionPool->description,
					   handle->stmtSessionPoolCol.description);
				strcpy(sessionPool->DbNode,"");
				strcpy(sessionPool->DbName,"");
				strcpy(sessionPool->DbUser,"");
				strcpy(sessionPool->DbPassword,"");
				strcpy(sessionPool->TraceFile,"");

				/* get the assignment count */
				XMLIMAPI_SessionPoolGetAssignment(handle,sessionPool);
				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_SESSIONPOOL,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_SESSIONPOOL);
				} else {
					addSQLErrorItem(handle, handle->stmtSessionPoolGetList, rc);
					SQLFreeStmt( handle->stmtSessionPoolGetList, SQL_DROP );
					handle->stmtSessionPoolGetList = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_SessionPoolGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGetNext(XMLIMAPI_Handle		handle,
												XMLIMAPI_SessionPool	*sessionPool)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtSessionPoolGetList;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(sessionPool->PoolID,
				   handle->stmtSessionPoolCol.PoolID);
			strcpy(sessionPool->PoolName,
				   handle->stmtSessionPoolCol.PoolName);
			strcpy(sessionPool->description,
				   handle->stmtSessionPoolCol.description);
			strcpy(sessionPool->DbNode,"");
			strcpy(sessionPool->DbName,"");
			strcpy(sessionPool->DbUser,"");
			strcpy(sessionPool->DbPassword,"");
			strcpy(sessionPool->TraceFile,"");

			/* get the assignment count */
			XMLIMAPI_SessionPoolGetAssignment(handle,sessionPool);
			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_SESSIONPOOLS,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_SESSIONPOOLS);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				handle->stmtSessionPoolGetList = 0;
			}
			return XMLIMAPI_False;
		}

	}






	/*------------------------------------------------------------------------
		HELPER FUNCTIONS
	--------------------------------------------------------------------------*/

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolListCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListCreate(XMLIMAPI_SessionPoolList *list)
	{
		XMLIMAPI_Bool				ok = XMLIMAPI_False;
		XMLIMAPI_SessionPoolList	newList = NULL;

		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_session_pool_list), (XMLIMAPI_UInt1**) &newList, &ok );
		if (ok == XMLIMAPI_True) {

			newList->first = NULL;
			newList->last = NULL;
			newList->curr = NULL;
			*list = newList;

			return XMLIMAPI_True;
		} else {
			*list = NULL;

			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolListDestroy
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListDestroy(XMLIMAPI_SessionPoolList list)
	{
		XMLIMAPI_SessionPoolListItem	current,next;
		
		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			sqlfree((XMLIMAPI_UInt1*)current);
		}

		sqlfree((XMLIMAPI_UInt1*)list);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolListInit
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListInit(XMLIMAPI_SessionPoolList list)
	{
		XMLIMAPI_SessionPoolListItem	current,next;
		
		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			sqlfree((XMLIMAPI_UInt1*)current);
		}

		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;

		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolListAddItem
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListAddItem(XMLIMAPI_SessionPoolList	list, 
													XMLIMAPI_SessionPool		*sessionPool)
	{
		XMLIMAPI_SessionPoolListItem		newListItem;
		XMLIMAPI_SessionPoolP				newItem;
		XMLIMAPI_Bool						ok = XMLIMAPI_False;
		
		if (!list) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_session_pool_list_item), (XMLIMAPI_UInt1**) &newListItem, &ok );
		if (ok == XMLIMAPI_True) {

			sqlallocat( sizeof(struct st_api_session_pool), (XMLIMAPI_UInt1**) &newItem, &ok );
			if (ok == XMLIMAPI_True) {

				strcpy(newItem->PoolID,sessionPool->PoolID);
				strcpy(newItem->PoolName,sessionPool->PoolName);
				strcpy(newItem->description,sessionPool->description);
				strcpy(newItem->DbNode,sessionPool->DbNode);
				strcpy(newItem->DbName,sessionPool->DbName);
				strcpy(newItem->DbUser,sessionPool->DbUser);
				strcpy(newItem->DbPassword,sessionPool->DbPassword);
				strcpy(newItem->TraceFile,sessionPool->TraceFile);

				newListItem->item = newItem;
			} else {
				return XMLIMAPI_False;
			}

			newListItem->next = NULL;
			if (!list->first) {
				/* first item */
				list->first = newListItem;
				list->last = newListItem;
			} else {
				/* all other items */
				list->last->next = newListItem;
				list->last = newListItem;
			}

			return XMLIMAPI_True;
		} else {

			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolListGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListGetFirst(XMLIMAPI_SessionPoolList	list, 
													 XMLIMAPI_SessionPool		**sessionPool)
	{
		if(!sessionPool)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		*sessionPool = list->first->item;
		list->curr = list->first;
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_SessionPoolListGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListGetNext(XMLIMAPI_SessionPoolList	list, 
													XMLIMAPI_SessionPool		**sessionPool)
	{
		if(!sessionPool)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		if(!list->curr)
			return XMLIMAPI_False;
		list->curr = list->curr->next;
		if(!list->curr)
			return XMLIMAPI_False;
		*sessionPool = list->curr->item;
		return XMLIMAPI_True;

	}

