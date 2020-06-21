/****************************************************************************

  module      : XMLIMAPI_Service.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-27  14:50
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

	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Service.h"
	#include "SAPDBCommon/SAPDB_string.h"


	#define SQL_INSERT_SERVICE \
			"INSERT INTO XML_INDEXINGSERVICEDESC SET \
				Id = ?,\
				Name = ?,\
				Description = ?,\
				DocumentStore = ?,\
				IndexStore = ?,\
				isSynchronous = '0'"

	#define SQL_UPDATE_SERVICE \
			"UPDATE XML_INDEXINGSERVICEDESC SET \
				Description = ?,\
				DocumentStore = ?,\
				IndexStore = ? \
			 WHERE \
				Id = ?"

	#define SQL_DELETE_SERVICE \
			"DELETE XML_INDEXINGSERVICEDESC \
			 WHERE Id = ?"

	#define SQL_GET_SERVICE \
			"SELECT	\
				I.\"ID\",\
				I.\"NAME\",\
				I.\"DESCRIPTION\",\
				I.\"DOCUMENTSTORE\",\
				I.\"INDEXSTORE\",\
				I.\"ISSYNCHRONOUS\"\
			 FROM \
				\"XML_INDEXINGSERVICEDESC\" I\
			 WHERE \
				I.\"ID\" = ?"

	#define	SQL_GET_SERVICE_ID \
			"SELECT	\
				I.\"ID\" \
			 FROM	\
				\"XML_INDEXINGSERVICEDESC\" I\
			 WHERE	\
				I.\"NAME\" = ?"

	#define SQL_GET_SERVICES \
			"SELECT	\
				I.\"ID\",\
				I.\"NAME\",\
				I.\"DESCRIPTION\",\
				I.\"DOCUMENTSTORE\",\
				I.\"INDEXSTORE\",\
				I.\"ISSYNCHRONOUS\"\
			 FROM \
				\"XML_INDEXINGSERVICEDESC\" I\
			 ORDER BY I.\"NAME\""


	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_ServiceCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceCreate(XMLIMAPI_Handle			handle,
										   XMLIMAPI_ServiceDesc	*service,
										 XMLIMAPI_Bool				alone)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_ErrorItem			errorItem;
		XMLIMAPI_ErrorState		errorState;
		XMLIMAPI_ErrorStateString  	state = "";
		XMLIMAPI_ErrorTextString   	text = "";
		XMLIMAPI_ErrorCode         	nativeODBCCode = 0;

		/* clean up */
		if (alone)
			Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtServiceCreate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_INSERT_SERVICE, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtServiceParam.ServiceID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.ServiceName, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.ServiceDesc, 513, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.DocStoreID, 55, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,5,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.IdxStoreID, 55, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtServiceCreate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtServiceCreate, SQL_CLOSE );
		}

		/* set values of parameters */
		/* - get unique id for index */
		if (!XMLIMAPI_IdGetNext(handle,handle->stmtServiceParam.ServiceID)) {
			return XMLIMAPI_False;
		}

		strcpy(handle->stmtServiceParam.ServiceName,service->ServiceName);
		strcpy(handle->stmtServiceParam.ServiceDesc,service->ServiceDesc);
		strcpy(handle->stmtServiceParam.DocStoreID,service->DocStoreID);
		strcpy(handle->stmtServiceParam.IdxStoreID,service->IdxStoreID);
		strcpy(handle->stmtServiceParam.isSynchronous,service->isSynchronous);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtServiceCreate)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtServiceCreate, state, text, &nativeODBCCode);
			if (nativeODBCCode==250) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_INDEXINGSERVICE_ALLREADY_EXISTS,
							 XMLIMAPI_ERR_MAPI_TEXT_INDEXINGSERVICE_ALLREADY_EXISTS);
			} else {
				addSQLErrorItem(handle, handle->stmtServiceCreate, rc);
				SQLFreeStmt( handle->stmtServiceCreate, SQL_DROP );
			}
			handle->stmtServiceCreate = 0;
			/* clean up */
			if (alone)
				Rollback(handle);
			return XMLIMAPI_False;
		}

		if (alone)
			Commit(handle);
		return XMLIMAPI_True;

	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_ServiceUpdate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceUpdate(XMLIMAPI_Handle			handle,
										   XMLIMAPI_ServiceDesc	*service)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_Id					__serviceId;
		XMLIMAPI_ServiceDesc		__service;
		XMLIMAPI_SessionPool		__sessionpoolDoc;
		XMLIMAPI_SessionPool		__sessionpoolIdx;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		/* 1.step check whether the Document Store / Index Store currently assigned to the Indexing Service are assigned elsewhere 
		     if not, remove them from the session pool table */
		XMLIMAPI_IdStringAsId(service->ServiceID,__serviceId);
		XMLIMAPI_ServiceGet(handle,__serviceId,&__service);
		strcpy(__sessionpoolDoc.PoolID,__service.DocStoreID);
		strcpy(__sessionpoolIdx.PoolID,__service.IdxStoreID);

		if (!handle->stmtServiceUpdate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_UPDATE_SERVICE, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,4,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtServiceParam.ServiceID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.ServiceDesc, 513, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.DocStoreID, 55, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.IdxStoreID, 55, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtServiceUpdate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtServiceUpdate, SQL_CLOSE );
		}

		/* set values of parameters */
		XMLIMAPI_IdStringAsId(service->ServiceID,handle->stmtServiceParam.ServiceID);
		strcpy(handle->stmtServiceParam.ServiceDesc,service->ServiceDesc);
		strcpy(handle->stmtServiceParam.DocStoreID,service->DocStoreID);
		strcpy(handle->stmtServiceParam.IdxStoreID,service->IdxStoreID);
		strcpy(handle->stmtServiceParam.isSynchronous,service->isSynchronous);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtServiceUpdate)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtServiceUpdate, rc);
			SQLFreeStmt( handle->stmtServiceUpdate, SQL_DROP );
			handle->stmtServiceUpdate = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);

		XMLIMAPI_SessionPoolDelete(handle,&__sessionpoolDoc);
		XMLIMAPI_SessionPoolDelete(handle,&__sessionpoolIdx);

		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_ServiceDelete
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDelete(XMLIMAPI_Handle			handle,
										   XMLIMAPI_ServiceDesc	*service)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_ErrorItem			errorItem;
		XMLIMAPI_ErrorState		errorState;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		/* 1. delete the xmlindexingservicedll record */
		if (!handle->stmtServiceDelete) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_DELETE_SERVICE, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtServiceParam.ServiceID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtServiceDelete = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtServiceDelete, SQL_CLOSE );
		}

		/* set values of parameters */
		XMLIMAPI_IdStringAsId(service->ServiceID,handle->stmtServiceParam.ServiceID);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtServiceDelete)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtServiceDelete, rc);
			SQLFreeStmt( handle->stmtServiceDelete, SQL_DROP );
			handle->stmtServiceDelete = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_ServiceGetId
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetId(XMLIMAPI_Handle		handle,
										   XMLIMAPI_Char		*ServiceName,
										   XMLIMAPI_Id			*ServiceId)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		long						foobar;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_Id				id;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtServiceGetId) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_SERVICE_ID, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol( SqlStmt, 1, SQL_C_BINARY, ServiceId, XMLIMAPI_MAX_ID_LEN, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.ServiceName, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtServiceGetId = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtServiceGetId, SQL_CLOSE );
		}

		/* set value of parameter */
		strcpy(handle->stmtServiceParam.ServiceName,ServiceName);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtServiceGetId)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtServiceGetId, rc);
			SQLFreeStmt( handle->stmtServiceGetId, SQL_DROP );
			handle->stmtServiceGetId = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtServiceGetId);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_SERVICE,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_SERVICE);
				} else {
					addSQLErrorItem(handle, handle->stmtServiceGetId, rc);
					SQLFreeStmt( handle->stmtServiceGetId, SQL_DROP );
					handle->stmtServiceGetId = 0;
				}
				return XMLIMAPI_False;
			}
		}
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_ServiceGet
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGet(XMLIMAPI_Handle		handle,
										XMLIMAPI_Id			serviceId,
										XMLIMAPI_ServiceDesc	*service)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtServiceGet) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_SERVICE, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtServiceParam.ServiceID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceDesc), 
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR, 
							&(handle->stmtServiceCol.DocStoreID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR, 
							&(handle->stmtServiceCol.IdxStoreID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 6, SQL_C_CHAR, 
							&(handle->stmtServiceCol.isSynchronous), 
							2, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtServiceGet = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtServiceGet, SQL_CLOSE );
		}

		/* set document class id */
		SAPDB_memcpy(handle->stmtServiceParam.ServiceID,serviceId,XMLIMAPI_MAX_ID_LEN);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtServiceGet)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtServiceGet, rc);
			SQLFreeStmt( handle->stmtServiceGet, SQL_DROP );
			handle->stmtServiceGet = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtServiceGet);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(service->ServiceID,
					   handle->stmtServiceCol.ServiceID);
				strcpy(service->ServiceName,
					   handle->stmtServiceCol.ServiceName);
				strcpy(service->ServiceDesc,
					   handle->stmtServiceCol.ServiceDesc);
				strcpy(service->DocStoreID,
					   handle->stmtServiceCol.DocStoreID);
				strcpy(service->IdxStoreID,
					   handle->stmtServiceCol.IdxStoreID);
				strcpy(service->isSynchronous,
					   handle->stmtServiceCol.isSynchronous);

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_SERVICE,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_SERVICE);
				} else {
					addSQLErrorItem(handle, handle->stmtServiceGet, rc);
					SQLFreeStmt( handle->stmtServiceGet, SQL_DROP );
					handle->stmtServiceGet = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
		return XMLIMAPI_True;

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_ServiceGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetFirst(XMLIMAPI_Handle		handle,
											XMLIMAPI_ServiceDesc	*service)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtServiceGetList) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_SERVICES, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceDesc), 
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR, 
							&(handle->stmtServiceCol.DocStoreID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR, 
							&(handle->stmtServiceCol.IdxStoreID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 6, SQL_C_CHAR, 
							&(handle->stmtServiceCol.isSynchronous), 
							2, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtServiceGetList = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtServiceGetList, SQL_CLOSE );
		}

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtServiceGetList)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtServiceGetList, rc);
			SQLFreeStmt( handle->stmtServiceGetList, SQL_DROP );
			handle->stmtServiceGetList = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtServiceGetList);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(service->ServiceID,
					   handle->stmtServiceCol.ServiceID);
				strcpy(service->ServiceName,
					   handle->stmtServiceCol.ServiceName);
				strcpy(service->ServiceDesc,
					   handle->stmtServiceCol.ServiceDesc);
				strcpy(service->DocStoreID,
					   handle->stmtServiceCol.DocStoreID);
				strcpy(service->IdxStoreID,
					   handle->stmtServiceCol.IdxStoreID);
				strcpy(service->isSynchronous,
					   handle->stmtServiceCol.isSynchronous);

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_SERVICE,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_SERVICE);
				} else {
					addSQLErrorItem(handle, handle->stmtServiceGetList, rc);
					SQLFreeStmt( handle->stmtServiceGetList, SQL_DROP );
					handle->stmtServiceGetList = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
		return XMLIMAPI_True;

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_ServiceGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetNext(XMLIMAPI_Handle		handle,
										    XMLIMAPI_ServiceDesc	*service)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtServiceGetList;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(service->ServiceID,
				   handle->stmtServiceCol.ServiceID);
			strcpy(service->ServiceName,
				   handle->stmtServiceCol.ServiceName);
			strcpy(service->ServiceDesc,
				   handle->stmtServiceCol.ServiceDesc);
			strcpy(service->DocStoreID,
				   handle->stmtServiceCol.DocStoreID);
			strcpy(service->IdxStoreID,
				   handle->stmtServiceCol.IdxStoreID);
			strcpy(service->isSynchronous,
				   handle->stmtServiceCol.isSynchronous);

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_SERVICES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_SERVICES);
			} else {
				addSQLErrorItem(handle, handle->stmtServiceGetList, rc);
				SQLFreeStmt( handle->stmtServiceGetList, SQL_DROP );
				handle->stmtServiceGetList = 0;
			}
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		/* Clean Up */
		Commit(handle);
		return XMLIMAPI_True;
	}


	/*------------------------------------------------------------------------
		GET FUNCTION FOR INDEXENGINE  <BEGIN>		
	--------------------------------------------------------------------------*/
	#define SQL_GET_SERVICE_BY_NAME \
			"SELECT	\
				I.\"ID\",\
				I.\"NAME\",\
				I.\"DESCRIPTION\",\
				SPDOC.DbNode,\
				SPDOC.DbName,\
				SPDOC.DbUser,\
				SPDOC.DbPassword,\
				SPDOC.TraceFile,\
				SPIDX.DbNode,\
				SPIDX.DbName,\
				SPIDX.DbUser,\
				SPIDX.DbPassword,\
				SPIDX.TraceFile\
			 FROM \
				\"XML_INDEXINGSERVICEDESC\" I,\
				\"XML_SESSIONPOOL\" SPDOC,\
				\"XML_SESSIONPOOL\" SPIDX\
			 WHERE \
				I.\"NAME\" = ? AND\
				I.\"DOCUMENTSTORE\" = SPDOC.\"ID\" AND\
				I.\"INDEXSTORE\" = SPIDX.\"ID\""

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_ServiceGetIndexingService
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetIndexingService(XMLIMAPI_Handle			handle,
													 XMLIMAPI_Char				*serviceName,
													 XMLIMAPI_IndexingService	*service)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		int							i;

		XMLIMAPI_ISSessionPool		docStore;
		XMLIMAPI_ISSessionPool		idxStore;
		XMLIMAPI_Id					id;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		/* STEP 1 */
		if (!handle->stmtServiceGetByName) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_SERVICE_BY_NAME, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtServiceParam.ServiceName, 129, NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtServiceCol.ServiceDesc), 
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR, 
							&(docStore.DbNode), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR, 
							&(docStore.DbName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 6, SQL_C_CHAR, 
							&(docStore.DbUser), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 7, SQL_C_CHAR, 
							&(docStore.DbPassword), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 8, SQL_C_CHAR, 
							&(docStore.TraceFile), 
							257, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 9, SQL_C_CHAR, 
							&(idxStore.DbNode), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 10, SQL_C_CHAR, 
							&(idxStore.DbName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 11, SQL_C_CHAR, 
							&(idxStore.DbUser), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 12, SQL_C_CHAR, 
							&(idxStore.DbPassword), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 13, SQL_C_CHAR, 
							&(idxStore.TraceFile), 
							257, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtServiceGetByName = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtServiceGetByName, SQL_CLOSE );
		}

		/* set document class id */
		strncpy(handle->stmtServiceParam.ServiceName,serviceName,128);
		handle->stmtServiceParam.ServiceName[128] = '\0';

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtServiceGetByName)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtServiceGetByName, rc);
			SQLFreeStmt( handle->stmtServiceGetByName, SQL_DROP );
			handle->stmtServiceGetByName = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtServiceGetByName);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(service->ServiceID,handle->stmtServiceCol.ServiceID);
				strcpy(service->ServiceName,handle->stmtServiceCol.ServiceName);
				strcpy(service->ServiceDesc,handle->stmtServiceCol.ServiceDesc);
				strcpy(service->DocStore.DbName,docStore.DbName);
				strcpy(service->DocStore.DbNode,docStore.DbNode);
				strcpy(service->DocStore.DbPassword,docStore.DbPassword);
				strcpy(service->DocStore.DbUser,docStore.DbUser);
				strcpy(service->DocStore.TraceFile,docStore.TraceFile);
				strcpy(service->IdxStore.DbName,idxStore.DbName);
				strcpy(service->IdxStore.DbNode,idxStore.DbNode);
				strcpy(service->IdxStore.DbPassword,idxStore.DbPassword);
				strcpy(service->IdxStore.DbUser,idxStore.DbUser);
				strcpy(service->IdxStore.TraceFile,idxStore.TraceFile);
				
			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_SERVICE,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_SERVICE);
				} else {
					addSQLErrorItem(handle, handle->stmtServiceGetByName, rc);
					SQLFreeStmt( handle->stmtServiceGetByName, SQL_DROP );
					handle->stmtServiceGetByName = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
		return XMLIMAPI_True;

	}

	/*------------------------------------------------------------------------
		GET FUNCTION FOR INDEXENGINE  <END>		
	--------------------------------------------------------------------------*/



	/*------------------------------------------------------------------------
		HELPER FUNCTIONS
	--------------------------------------------------------------------------*/

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_ServiceDescListCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListCreate(XMLIMAPI_ServiceDescList *list)
	{
		XMLIMAPI_Bool				ok = XMLIMAPI_False;
		XMLIMAPI_ServiceDescList	newList = NULL;

		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_service_desc_list), (XMLIMAPI_UInt1**) &newList, &ok );
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
		Function:	XMLIMAPI_ServiceDescListDestroy
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListDestroy(XMLIMAPI_ServiceDescList list)
	{
		XMLIMAPI_ServiceDescListItem	current,next;
		
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
		Function:	XMLIMAPI_ServiceDescListInit
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListInit(XMLIMAPI_ServiceDescList list)
	{
		XMLIMAPI_ServiceDescListItem	current,next;
		
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
		Function:	XMLIMAPI_ServiceDescListAddItem
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListAddItem(XMLIMAPI_ServiceDescList	list, 
													XMLIMAPI_ServiceDesc		*service)
	{
		XMLIMAPI_ServiceDescListItem		newListItem;
		XMLIMAPI_ServiceDescP				newItem;
		XMLIMAPI_Bool						ok = XMLIMAPI_False;
		
		if (!list) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_service_desc_list_item), (XMLIMAPI_UInt1**) &newListItem, &ok );
		if (ok == XMLIMAPI_True) {

			sqlallocat( sizeof(struct st_api_service_desc), (XMLIMAPI_UInt1**) &newItem, &ok );
			if (ok == XMLIMAPI_True) {

				strcpy(newItem->ServiceID,service->ServiceID);
				strcpy(newItem->ServiceName,service->ServiceName);
				strcpy(newItem->ServiceDesc,service->ServiceDesc);
				strcpy(newItem->DocStoreID,service->DocStoreID);
				strcpy(newItem->IdxStoreID,service->IdxStoreID);

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
		Function:	XMLIMAPI_ServiceDescListGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListGetFirst(XMLIMAPI_ServiceDescList	list, 
													 XMLIMAPI_ServiceDesc		**service)
	{
		if(!service)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		*service = list->first->item;
		list->curr = list->first;
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_ServiceDescListGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListGetNext(XMLIMAPI_ServiceDescList	list, 
													XMLIMAPI_ServiceDesc		**service)
	{
		if(!service)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		if(!list->curr)
			return XMLIMAPI_False;
		list->curr = list->curr->next;
		if(!list->curr)
			return XMLIMAPI_False;
		*service = list->curr->item;
		return XMLIMAPI_True;

	}

