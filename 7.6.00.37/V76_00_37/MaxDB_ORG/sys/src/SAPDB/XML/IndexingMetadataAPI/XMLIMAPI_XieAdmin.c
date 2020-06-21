/****************************************************************************

  module      : XMLIMAPI_XieAdmin.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-22  9:44
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


	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_XieAdmin.h"
	#include "SAPDBCommon/SAPDB_string.h"

	#define SQL_INSERT_XIE \
			"INSERT INTO XML_INDEXENGINE \
				(Id, Name, Node, Description, InstRoot, LogFile, Alive, Refresh, ISID, ILIBID)\
			VALUES \
				(?,?,?,?,?,\
				?,'0001-01-01 00:00:00.000000', '1', ?,\
				(SELECT ID FROM XML_INDEXINGLIB WHERE NAME = ? AND VERSION = ?))"

	#define SQL_INSERT_XIE_IDXLIB \
			"INSERT INTO XML_INDEXINGLIB SET \
				Id = ?,\
				Name = ?,\
				Description = ?,\
				Version = ?"

	#define SQL_UPDATE_XIE \
			"UPDATE XML_INDEXENGINE SET \
				Description = ?,\
				InstRoot = ?,\
				ISID = ?,\
				ILIBID = (SELECT ID FROM XML_INDEXINGLIB WHERE NAME = ? AND VERSION = ?),\
				LogFile = ?,\
				Alive = DEFAULT,\
				Refresh = '1'\
			 WHERE \
				Name = ? AND Node = ?"

	#define SQL_UPDATE_XIE_IDXLIB \
			"UPDATE XML_INDEXINGLIB SET \
				Description = ?\
			 WHERE \
				Name = ? AND Version = ?"

	#define SQL_GET_XIE \
			"SELECT	\
				I.Id,\
				I.Name,\
				I.Node,\
				I.Description,\
				I.InstRoot,\
				I.LogFile,\
				I.Alive,\
				IS.Id,\
				IS.Name,\
				IS.Description,\
				DOCSP.Name,\
				IDXSP.Name,\
				IL.Id,\
				IL.Name,\
				IL.Version,\
				IL.Description\
			 FROM \
				\"XML_INDEXENGINE\" I,\
				\"XML_INDEXINGSERVICEDESC\" IS,\
				\"XML_INDEXINGLIB\" IL,\
				\"XML_SESSIONPOOL\" DOCSP,\
				\"XML_SESSIONPOOL\" IDXSP\
			 WHERE \
				I.\"ID\" = ? AND\
				I.ISID = IS.ID AND\
				I.ILIBID = IL.ID AND\
				IS.DocumentStore = DOCSP.ID AND\
				IS.IndexStore = IDXSP.ID"

	#define SQL_GET_XIE_LIST \
			"SELECT	\
				I.Id,\
				I.Name,\
				I.Node,\
				I.Description,\
				I.InstRoot,\
				I.LogFile,\
				I.Alive,\
				IS.Id,\
				IS.Name,\
				IS.Description,\
				DOCSP.Name,\
				IDXSP.Name,\
				IL.Id,\
				IL.Name,\
				IL.Version,\
				IL.Description\
			 FROM \
				\"XML_INDEXENGINE\" I,\
				\"XML_INDEXINGSERVICEDESC\" IS,\
				\"XML_INDEXINGLIB\" IL,\
				\"XML_SESSIONPOOL\" DOCSP,\
				\"XML_SESSIONPOOL\" IDXSP\
			 WHERE \
				I.ISID = IS.ID AND\
				I.ILIBID = IL.ID AND\
				IS.DocumentStore = DOCSP.ID AND\
				IS.IndexStore = IDXSP.ID \
			 ORDER BY I.\"NODE\", I.\"NAME\""

	#define SQL_GET_XIE_REFRESH \
			"Select refresh \
			 from XML_INDEXENGINE \
			 where \
			 Name = ? AND\
			 Node = ?"

	#define SQL_SET_XIE_ALIVE \
			"update XML_INDEXENGINE (Alive) values (DEFAULT) where Name = ? AND Node = ?"

	#define SQL_UNSET_XIE_REFRESH \
			"update XML_INDEXENGINE (Refresh) values ('0') where Name = ? AND Node = ?"

	#define SQL_DELETE_XIE \
			"delete XML_INDEXENGINE where Name = ? AND Node = ?"

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieRegister
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieRegister(XMLIMAPI_Handle   handle,
									   XMLIMAPI_Char *engineServerNode,
									   XMLIMAPI_Char *engineFileName,
									   XMLIMAPI_Char *registerName,
									   XMLIMAPI_Char *registerDescription,
									   XMLIMAPI_Char *registerLogFile,
									   XMLIMAPI_Char *indexingServiceName,
									   XMLIMAPI_Char *indexingLibFileName,
									   XMLIMAPI_Char *indexingLibVersion,
									   XMLIMAPI_Char *indexingLibDescription)
	{
		SQLHSTMT					__SqlStmt;
		SQLHENV						__SqlEnv;
		SQLHDBC						__SqlDBC;
		SQLRETURN					__rc;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorState			__errorState;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorStateString  	__state = "";
		XMLIMAPI_ErrorTextString   	__text = "";
		XMLIMAPI_ErrorCode         	__nativeODBCCode = 0;
		XMLIMAPI_Bool				__performIdxLibUpdate;
		XMLIMAPI_Bool				__performXieUpdate;
		XMLIMAPI_Id					__serviceId;
		XMLIMAPI_ServiceDesc		__service;

		/* clean up */
		Rollback(handle);
	
		__SqlEnv = handle->sqlHenv;
		__SqlDBC = handle->sqlHdbc;

		/* Registering a XML Index Engine (XIE) consists of following steps:
		   1. check the existence of the indexing service
		   2. if not existent create the indexing service
		   3. Insert the IndexingLib into XML_INDEXINGLIB: o.k.-> 3.;
		   4. Update the IndexingLib in XML_INDEXINGLIB
		   5. Insert the XIE into XML_INDEXENGINE: o.k.-> 5.; (set implicit REFRESH = true, Alive to initial timestamp)
		   6. Update the XIE in XML_INDEXINGLIB (set implicit REFRESH = true, Alive to initial timestamp)
		   7. end
	   */
		/* Step 1 */
		if (!XMLIMAPI_ServiceGetId(handle,indexingServiceName,&__serviceId)) {
			XMLIMAPI_GetLastError(handle,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			if (__errorType == XMLIMAPI_ERR_TYPE_SQL) {
				return XMLIMAPI_False;
			}
			/* indexing service not yet existing */
			/* Step 2 */
			/* create the indexing service with the given name */
			strcpy(__service.ServiceName,indexingServiceName);
			strcpy(__service.ServiceDesc,registerDescription);
			XMLIMAPI_IdInitValue(__serviceId);
			XMLIMAPI_IdAsString(__serviceId,__service.DocStoreID);
			XMLIMAPI_IdAsString(__serviceId,__service.IdxStoreID);
			strcpy(__service.isSynchronous,"0");
			if (!XMLIMAPI_ServiceCreate(handle,&__service,XMLIMAPI_False)) {
				return XMLIMAPI_False;
			} else {
				if (!XMLIMAPI_ServiceGetId(handle,indexingServiceName,&__serviceId)) {
					return XMLIMAPI_False;
				}
			}
		}
		/* Step 3 */
		__performIdxLibUpdate = XMLIMAPI_False;
		if (!handle->stmtXieRegisterInsertIdxLib) {
			/* no prepared statement yet */
			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_INSERT_XIE_IDXLIB, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXieAdminParam.IndexingLibID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.IndexingLibName, 512, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.IndexingLibDesc, 512, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.IndexingLibVersion, 128, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			handle->stmtXieRegisterInsertIdxLib = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieRegisterInsertIdxLib, SQL_CLOSE );
		}

		/* set values of parameters */
		/* - get unique id for index */
		if (!XMLIMAPI_IdGetNext(handle,handle->stmtXieAdminParam.IndexingLibID)) {
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		strcpy(handle->stmtXieAdminParam.IndexingLibName,indexingLibFileName);
		strcpy(handle->stmtXieAdminParam.IndexingLibDesc,indexingLibDescription);
		strcpy(handle->stmtXieAdminParam.IndexingLibVersion,indexingLibVersion);

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieRegisterInsertIdxLib)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtXieRegisterInsertIdxLib, __state, __text, &__nativeODBCCode);
			if (__nativeODBCCode==250) {
				/* o.k. the lib is allready in existence -> perform an update to change the description (if changed) */
				__performIdxLibUpdate = XMLIMAPI_True;
			} else {
				/* some other failure took place -> leave here with error */
				addSQLErrorItem(handle, handle->stmtXieRegisterInsertIdxLib, __rc);
				SQLFreeStmt( handle->stmtXieRegisterInsertIdxLib, SQL_DROP );
				handle->stmtXieRegisterInsertIdxLib = 0;
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
		}

		/* the indexing lib allready was registert -> update it */
		/* Step 4 */
		if (__performIdxLibUpdate){
			if (!handle->stmtXieRegisterUpdateIdxLib) {
				/* no prepared statement yet */
				__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}
				__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_UPDATE_XIE_IDXLIB, SQL_NTS );
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}
					
				__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.IndexingLibDesc, 512, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.IndexingLibName, 512, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.IndexingLibVersion, 128, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				handle->stmtXieRegisterUpdateIdxLib = __SqlStmt;

			} else {
				SQLFreeStmt( handle->stmtXieRegisterUpdateIdxLib, SQL_CLOSE );
			}

			/* set values of parameters */
			strcpy(handle->stmtXieAdminParam.IndexingLibName,indexingLibFileName);
			strcpy(handle->stmtXieAdminParam.IndexingLibDesc,indexingLibDescription);
			strcpy(handle->stmtXieAdminParam.IndexingLibVersion,indexingLibVersion);

			/* execute statement */
			if ((__rc = SQLExecute(handle->stmtXieRegisterUpdateIdxLib)) != SQL_SUCCESS) {
				/* some other failure took place -> leave here with error */
				addSQLErrorItem(handle, handle->stmtXieRegisterUpdateIdxLib, __rc);
				SQLFreeStmt( handle->stmtXieRegisterUpdateIdxLib, SQL_DROP );
				handle->stmtXieRegisterUpdateIdxLib = 0;
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
		}

		/* the indexing lib was successfully registered */
		__performXieUpdate = XMLIMAPI_False;
		/* Step 5 */
		if (!handle->stmtXieRegisterInsertXie) {
			/* no prepared statement yet */

			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_INSERT_XIE, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindParameter(__SqlStmt,7,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
									0,0, handle->stmtXieAdminParam.IndexingServiceID, 55, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,8,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.IndexingLibName, 512, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,9,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.IndexingLibVersion, 128, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXieAdminParam.ID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.XieName, 128, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.Node, 257, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.description, 512, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,5,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.InstRoot, 512, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,6,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.LogFile, 512, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			handle->stmtXieRegisterInsertXie = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieRegisterInsertXie, SQL_CLOSE );
		}

		/* set values of parameters */
		/* - get unique id for index */
		if (!XMLIMAPI_IdGetNext(handle,handle->stmtXieAdminParam.ID)) {
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}
		XMLIMAPI_IdAsString(__serviceId,handle->stmtXieAdminParam.IndexingServiceID);
		strcpy(handle->stmtXieAdminParam.XieName,registerName);
		strcpy(handle->stmtXieAdminParam.Node,engineServerNode);
		strcpy(handle->stmtXieAdminParam.description,registerDescription);
		strcpy(handle->stmtXieAdminParam.InstRoot,engineFileName);
		strcpy(handle->stmtXieAdminParam.IndexingServiceName,indexingServiceName);
		strcpy(handle->stmtXieAdminParam.IndexingLibName,indexingLibFileName);
		strcpy(handle->stmtXieAdminParam.IndexingLibVersion,indexingLibVersion);
		strcpy(handle->stmtXieAdminParam.LogFile,registerLogFile);

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieRegisterInsertXie)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtXieRegisterInsertXie, __state, __text, &__nativeODBCCode);
			if (__nativeODBCCode==250) {
				/* Index Engine allready exists -> update it */
				__performXieUpdate = XMLIMAPI_True;
			} else {
				addSQLErrorItem(handle, handle->stmtXieRegisterInsertXie, __rc);
				SQLFreeStmt( handle->stmtXieRegisterInsertXie, SQL_DROP );
				handle->stmtXieRegisterInsertXie = 0;
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
		}

		/* update the Index Engine, if necessary */
		/* Step 6 */
		if (__performXieUpdate) {
			if (!handle->stmtXieRegisterUpdateXie) {
				/* no prepared statement yet */

				__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_UPDATE_XIE, SQL_NTS );
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}
					
				__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.description, 512, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.InstRoot, 512, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.IndexingServiceID, 55, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.IndexingLibName, 512, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,5,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.IndexingLibVersion, 128, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,6,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.LogFile, 512, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,7,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.XieName, 128, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				__rc = SQLBindParameter(__SqlStmt,8,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 								0,0, handle->stmtXieAdminParam.Node, 257, NULL);
				if (__rc != SQL_SUCCESS) {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( __SqlStmt, SQL_DROP );
					/* clean up */
					Rollback(handle);
					return XMLIMAPI_False;
				}

				handle->stmtXieRegisterUpdateXie = __SqlStmt;

			} else {
				SQLFreeStmt( handle->stmtXieRegisterUpdateXie, SQL_CLOSE );
			}

			/* set values of parameters */
			/* - get unique id for index */
			if (!XMLIMAPI_IdGetNext(handle,handle->stmtXieAdminParam.ID)) {
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			XMLIMAPI_IdAsString(__serviceId,handle->stmtXieAdminParam.IndexingServiceID);
			strcpy(handle->stmtXieAdminParam.XieName,registerName);
			strcpy(handle->stmtXieAdminParam.Node,engineServerNode);
			strcpy(handle->stmtXieAdminParam.description,registerDescription);
			strcpy(handle->stmtXieAdminParam.InstRoot,engineFileName);
			strcpy(handle->stmtXieAdminParam.IndexingServiceName,indexingServiceName);
			strcpy(handle->stmtXieAdminParam.IndexingLibName,indexingLibFileName);
			strcpy(handle->stmtXieAdminParam.IndexingLibVersion,indexingLibVersion);
			strcpy(handle->stmtXieAdminParam.LogFile,registerLogFile);

			/* execute statement */
			if ((__rc = SQLExecute(handle->stmtXieRegisterUpdateXie)) != SQL_SUCCESS) {
				addSQLErrorItem(handle, handle->stmtXieRegisterUpdateXie, __rc);
				SQLFreeStmt( handle->stmtXieRegisterUpdateXie, SQL_DROP );
				handle->stmtXieRegisterUpdateXie = 0;
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieSetAliveGetRefresh
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieSetAliveGetRefresh(XMLIMAPI_Handle   handle,
												 XMLIMAPI_Char	   *xiename,
												 XMLIMAPI_Char	   *xienode,
												 XMLIMAPI_Bool	   *refresh)
	{
		SQLHSTMT					__SqlStmt;
		SQLHENV						__SqlEnv;
		SQLHDBC						__SqlDBC;
		SQLRETURN					__rc;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorState			__errorState;
		XMLIMAPI_ErrorStateString  	__state = "";
		XMLIMAPI_ErrorTextString   	__text = "";
		XMLIMAPI_ErrorCode         	__nativeODBCCode = 0;

		/* clean up */
		Rollback(handle);
	
		__SqlEnv = handle->sqlHenv;
		__SqlDBC = handle->sqlHdbc;

		/* Set alive for the given XIE */
		if (!handle->stmtXieSetAlive) {
			/* no prepared statement yet */
			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_SET_XIE_ALIVE, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.XieName, 128, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.Node, 257, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			handle->stmtXieSetAlive = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieSetAlive, SQL_CLOSE );
		}

		strcpy(handle->stmtXieAdminParam.XieName,xiename);
		strcpy(handle->stmtXieAdminParam.Node,xienode);

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieSetAlive)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtXieSetAlive, __state, __text, &__nativeODBCCode);
			if (strcmp(__state,XMLIMAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)==0) {
				/* the record was not found -> given XIE (Name,Node) does not exist */
				addErrorItem(handle,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_XIE_DOES_NOT_EXIST,XMLIMAPI_ERR_MAPI_TEXT_XIE_DOES_NOT_EXIST);
			} else {
				addSQLErrorItem(handle, handle->stmtXieSetAlive, __rc);
				SQLFreeStmt( handle->stmtXieSetAlive, SQL_DROP );
				handle->stmtXieSetAlive = 0;
			}
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		/* Get refresh for the given XIE */
		__SqlStmt = 0;
		if (!handle->stmtXieGetRefresh) {
			/* no prepared statement yet */
			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_GET_XIE_REFRESH, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindCol(__SqlStmt, 1, SQL_C_CHAR, 
							  &handle->stmtXieAdminCol.Refresh, 
								512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt, 1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.XieName, 128, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.Node, 257, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			handle->stmtXieGetRefresh = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieGetRefresh, SQL_CLOSE );
		}

		strcpy(handle->stmtXieAdminParam.XieName,xiename);
		strcpy(handle->stmtXieAdminParam.Node,xienode);

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieGetRefresh)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtXieGetRefresh, __state, __text, &__nativeODBCCode);
			if (strcmp(__state,XMLIMAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)==0) {
				/* the record was not found -> given XIE (Name,Node) does not exist */
				addErrorItem(handle,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_XIE_DOES_NOT_EXIST,XMLIMAPI_ERR_MAPI_TEXT_XIE_DOES_NOT_EXIST);
			} else {
				addSQLErrorItem(handle, handle->stmtXieGetRefresh, __rc);
				SQLFreeStmt( handle->stmtXieGetRefresh, SQL_DROP );
				handle->stmtXieGetRefresh = 0;
			}
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		} else {
			/* get the result */
			__rc = SQLFetch(handle->stmtXieGetRefresh);

			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, handle->stmtXieGetRefresh, __rc);
				SQLFreeStmt( handle->stmtXieGetRefresh, SQL_DROP );
				handle->stmtXieGetRefresh = 0;
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			} else {
				if (strcmp(handle->stmtXieAdminCol.Refresh,"1")==0) 
					*refresh = XMLIMAPI_True;
				else
					*refresh = XMLIMAPI_False;
			}
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieUnSetRefresh
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieUnSetRefresh(XMLIMAPI_Handle   handle,
								 		   XMLIMAPI_Char	 *xiename,
										   XMLIMAPI_Char	 *xienode)
	{
		SQLHSTMT					__SqlStmt;
		SQLHENV						__SqlEnv;
		SQLHDBC						__SqlDBC;
		SQLRETURN					__rc;
		XMLIMAPI_ErrorStateString  	__state = "";
		XMLIMAPI_ErrorTextString   	__text = "";
		XMLIMAPI_ErrorCode         	__nativeODBCCode = 0;

		/* clean up */
		Rollback(handle);
	
		__SqlEnv = handle->sqlHenv;
		__SqlDBC = handle->sqlHdbc;

		if (!handle->stmtXieUnSetRefresh) {
			/* no prepared statement yet */
			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_UNSET_XIE_REFRESH, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.XieName, 128, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.Node, 257, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			handle->stmtXieUnSetRefresh = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieUnSetRefresh, SQL_CLOSE );
		}

		strcpy(handle->stmtXieAdminParam.XieName,xiename);
		strcpy(handle->stmtXieAdminParam.Node,xienode);

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieUnSetRefresh)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtXieUnSetRefresh, __state, __text, &__nativeODBCCode);
			if (strcmp(__state,XMLIMAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)==0) {
				/* the record was not found -> given XIE (Name,Node) does not exist */
				addErrorItem(handle,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_XIE_DOES_NOT_EXIST,XMLIMAPI_ERR_MAPI_TEXT_XIE_DOES_NOT_EXIST);
			} else {
				addSQLErrorItem(handle, handle->stmtXieUnSetRefresh, __rc);
				SQLFreeStmt( handle->stmtXieUnSetRefresh, SQL_DROP );
				handle->stmtXieUnSetRefresh = 0;
			}
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieUnRegister
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieUnRegister(XMLIMAPI_Handle   handle,
										 XMLIMAPI_Char *xiename,
										 XMLIMAPI_Char *xienode)
	{
		SQLHSTMT					__SqlStmt;
		SQLHENV						__SqlEnv;
		SQLHDBC						__SqlDBC;
		SQLRETURN					__rc;
		XMLIMAPI_ErrorStateString  	__state = "";
		XMLIMAPI_ErrorTextString   	__text = "";
		XMLIMAPI_ErrorCode         	__nativeODBCCode = 0;

		/* clean up */
		Rollback(handle);
	
		__SqlEnv = handle->sqlHenv;
		__SqlDBC = handle->sqlHdbc;

		if (!handle->stmtXieUnRegister) {
			/* no prepared statement yet */
			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_DELETE_XIE, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.XieName, 128, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXieAdminParam.Node, 257, NULL);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}

			handle->stmtXieUnRegister = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieUnRegister, SQL_CLOSE );
		}

		strcpy(handle->stmtXieAdminParam.XieName,xiename);
		strcpy(handle->stmtXieAdminParam.Node,xienode);

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieUnRegister)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtXieUnRegister, __state, __text, &__nativeODBCCode);
			if (strcmp(__state,XMLIMAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)==0) {
				/* the record was not found -> given XIE (Name,Node) does not exist */
				addErrorItem(handle,XMLIMAPI_ERR_TYPE_MAPI,XMLIMAPI_ERR_MAPI_CODE_XIE_DOES_NOT_EXIST,XMLIMAPI_ERR_MAPI_TEXT_XIE_DOES_NOT_EXIST);
			} else {
				addSQLErrorItem(handle, handle->stmtXieUnRegister, __rc);
				SQLFreeStmt( handle->stmtXieUnRegister, SQL_DROP );
				handle->stmtXieUnRegister = 0;
			}
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieAdminGet
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminGet(XMLIMAPI_Handle   handle,
									   XMLIMAPI_Id		xieadminId,
									   XMLIMAPI_XieAdmin *xieadmin)
	{
		SQLHSTMT					__SqlStmt;
		SQLHENV						__SqlEnv;
		SQLHDBC						__SqlDBC;
		SQLRETURN					__rc;
		XMLIMAPI_ErrorStateString  	__state = "";
		XMLIMAPI_ErrorTextString   	__text = "";
		XMLIMAPI_ErrorCode         	__nativeODBCCode = 0;

		__SqlEnv = handle->sqlHenv;
		__SqlDBC = handle->sqlHdbc;

		/* Get refresh for the given XIE */
		__SqlStmt = 0;
		if (!handle->stmtXieGet) {
			/* no prepared statement yet */
			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_GET_XIE, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindCol(__SqlStmt, 1, SQL_C_CHAR, 
							  &handle->stmtXieAdminCol.ID, 
								55, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 2, SQL_C_CHAR, 
							  &handle->stmtXieAdminCol.XieName, 
								128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 3, SQL_C_CHAR, &handle->stmtXieAdminCol.Node, 257, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 4, SQL_C_CHAR, &handle->stmtXieAdminCol.description, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 5, SQL_C_CHAR, &handle->stmtXieAdminCol.InstRoot, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 6, SQL_C_CHAR, &handle->stmtXieAdminCol.LogFile, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 7, SQL_C_CHAR, &handle->stmtXieAdminCol.Alive, 50, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 8, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceID, 55, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 9, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceName, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 10, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceDesc, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 11, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceDocStoreName, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 12, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceIdxStoreName, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 13, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibID, 55, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 14, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibName, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 15, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibVersion, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 16, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibDesc, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXieAdminParam.ID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtXieGet = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieGet, SQL_CLOSE );
		}

		/* set Xie Id */
		SAPDB_memcpy(handle->stmtXieAdminParam.ID,xieadminId,XMLIMAPI_MAX_ID_LEN);

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieGet)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtXieGet, __rc);
			SQLFreeStmt( handle->stmtXieGet, SQL_DROP );
			handle->stmtXieGet = 0;
			return XMLIMAPI_False;
		} else {
			/* get the result */
			__rc = SQLFetch(handle->stmtXieGet);

			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, handle->stmtXieGet, __rc);
				SQLFreeStmt( handle->stmtXieGet, SQL_DROP );
				handle->stmtXieGet = 0;
				return XMLIMAPI_False;
			} else {
				strcpy(xieadmin->ID,handle->stmtXieAdminCol.ID);
				strcpy(xieadmin->XieName,handle->stmtXieAdminCol.XieName);
				strcpy(xieadmin->Node,handle->stmtXieAdminCol.Node);
				strcpy(xieadmin->description,handle->stmtXieAdminCol.description);
				strcpy(xieadmin->InstRoot,handle->stmtXieAdminCol.InstRoot);
				strcpy(xieadmin->LogFile,handle->stmtXieAdminCol.LogFile);
				strcpy(xieadmin->Alive,handle->stmtXieAdminCol.Alive);
				strcpy(xieadmin->IndexingServiceID,handle->stmtXieAdminCol.IndexingServiceID);
				strcpy(xieadmin->IndexingServiceName,handle->stmtXieAdminCol.IndexingServiceName);
				strcpy(xieadmin->IndexingServiceDesc,handle->stmtXieAdminCol.IndexingServiceDesc);
				strcpy(xieadmin->IndexingServiceDocStoreName,handle->stmtXieAdminCol.IndexingServiceDocStoreName);
				strcpy(xieadmin->IndexingServiceIdxStoreName,handle->stmtXieAdminCol.IndexingServiceIdxStoreName);
				strcpy(xieadmin->IndexingLibID,handle->stmtXieAdminCol.IndexingLibID);
				strcpy(xieadmin->IndexingLibName,handle->stmtXieAdminCol.IndexingLibName);
				strcpy(xieadmin->IndexingLibVersion,handle->stmtXieAdminCol.IndexingLibVersion);
				strcpy(xieadmin->IndexingLibDesc,handle->stmtXieAdminCol.IndexingLibDesc);
			}
		}

		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieAdminGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminGetFirst(XMLIMAPI_Handle   handle,
										    XMLIMAPI_XieAdmin *xieadmin)
	{
		SQLHSTMT					__SqlStmt;
		SQLHENV						__SqlEnv;
		SQLHDBC						__SqlDBC;
		SQLRETURN					__rc;
		XMLIMAPI_ErrorStateString  	__state = "";
		XMLIMAPI_ErrorTextString   	__text = "";
		XMLIMAPI_ErrorCode         	__nativeODBCCode = 0;

		__SqlEnv = handle->sqlHenv;
		__SqlDBC = handle->sqlHdbc;

		/* Get refresh for the given XIE */
		__SqlStmt = 0;
		if (!handle->stmtXieGetList) {
			/* no prepared statement yet */
			__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
			__rc = SQLPrepare(__SqlStmt, (SQLCHAR*)SQL_GET_XIE_LIST, SQL_NTS );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				/* clean up */
				Rollback(handle);
				return XMLIMAPI_False;
			}
				
			__rc = SQLBindCol(__SqlStmt, 1, SQL_C_CHAR, 
							  &handle->stmtXieAdminCol.ID, 
								55, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 2, SQL_C_CHAR, 
							  &handle->stmtXieAdminCol.XieName, 
								128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 3, SQL_C_CHAR, &handle->stmtXieAdminCol.Node, 257, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 4, SQL_C_CHAR, &handle->stmtXieAdminCol.description, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 5, SQL_C_CHAR, &handle->stmtXieAdminCol.InstRoot, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 6, SQL_C_CHAR, &handle->stmtXieAdminCol.LogFile, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 7, SQL_C_CHAR, &handle->stmtXieAdminCol.Alive, 50, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 8, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceID, 55, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 9, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceName, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 10, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceDesc, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 11, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceDocStoreName, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 12, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingServiceIdxStoreName, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 13, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibID, 55, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 14, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibName, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 15, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibVersion, 128, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			__rc = SQLBindCol(__SqlStmt, 16, SQL_C_CHAR, &handle->stmtXieAdminCol.IndexingLibDesc, 512, NULL );
			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtXieGetList = __SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXieGetList, SQL_CLOSE );
		}

		/* execute statement */
		if ((__rc = SQLExecute(handle->stmtXieGetList)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtXieGetList, __rc);
			SQLFreeStmt( handle->stmtXieGetList, SQL_DROP );
			handle->stmtXieGetList = 0;
			return XMLIMAPI_False;
		} else {
			/* get the result */
			__rc = SQLFetch(handle->stmtXieGetList);

			if (__rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, handle->stmtXieGetList, __rc);
				SQLFreeStmt( handle->stmtXieGetList, SQL_DROP );
				handle->stmtXieGetList = 0;
				return XMLIMAPI_False;
			} else {
				strcpy(xieadmin->ID,handle->stmtXieAdminCol.ID);
				strcpy(xieadmin->XieName,handle->stmtXieAdminCol.XieName);
				strcpy(xieadmin->Node,handle->stmtXieAdminCol.Node);
				strcpy(xieadmin->description,handle->stmtXieAdminCol.description);
				strcpy(xieadmin->InstRoot,handle->stmtXieAdminCol.InstRoot);
				strcpy(xieadmin->LogFile,handle->stmtXieAdminCol.LogFile);
				strcpy(xieadmin->Alive,handle->stmtXieAdminCol.Alive);
				strcpy(xieadmin->IndexingServiceID,handle->stmtXieAdminCol.IndexingServiceID);
				strcpy(xieadmin->IndexingServiceName,handle->stmtXieAdminCol.IndexingServiceName);
				strcpy(xieadmin->IndexingServiceDesc,handle->stmtXieAdminCol.IndexingServiceDesc);
				strcpy(xieadmin->IndexingServiceDocStoreName,handle->stmtXieAdminCol.IndexingServiceDocStoreName);
				strcpy(xieadmin->IndexingServiceIdxStoreName,handle->stmtXieAdminCol.IndexingServiceIdxStoreName);
				strcpy(xieadmin->IndexingLibID,handle->stmtXieAdminCol.IndexingLibID);
				strcpy(xieadmin->IndexingLibName,handle->stmtXieAdminCol.IndexingLibName);
				strcpy(xieadmin->IndexingLibVersion,handle->stmtXieAdminCol.IndexingLibVersion);
				strcpy(xieadmin->IndexingLibDesc,handle->stmtXieAdminCol.IndexingLibDesc);
			}
		}

		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieAdminGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminGetNext(XMLIMAPI_Handle   handle,
										   XMLIMAPI_XieAdmin *xieadmin)
	{
		/* get the result */
		SQLRETURN					__rc;
		
		__rc = SQLFetch(handle->stmtXieGetList);

		if (__rc != SQL_SUCCESS) {
			if (__rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_XIES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_XIES);
			} else {
				addSQLErrorItem(handle, handle->stmtXieGetList, __rc);
				SQLFreeStmt( handle->stmtXieGetList, SQL_DROP );
				handle->stmtXieGetList = 0;
			}
			return XMLIMAPI_False;
		} else {
			strcpy(xieadmin->ID,handle->stmtXieAdminCol.ID);
			strcpy(xieadmin->XieName,handle->stmtXieAdminCol.XieName);
			strcpy(xieadmin->Node,handle->stmtXieAdminCol.Node);
			strcpy(xieadmin->description,handle->stmtXieAdminCol.description);
			strcpy(xieadmin->InstRoot,handle->stmtXieAdminCol.InstRoot);
			strcpy(xieadmin->LogFile,handle->stmtXieAdminCol.LogFile);
			strcpy(xieadmin->Alive,handle->stmtXieAdminCol.Alive);
			strcpy(xieadmin->IndexingServiceID,handle->stmtXieAdminCol.IndexingServiceID);
			strcpy(xieadmin->IndexingServiceName,handle->stmtXieAdminCol.IndexingServiceName);
			strcpy(xieadmin->IndexingServiceDesc,handle->stmtXieAdminCol.IndexingServiceDesc);
			strcpy(xieadmin->IndexingServiceDocStoreName,handle->stmtXieAdminCol.IndexingServiceDocStoreName);
			strcpy(xieadmin->IndexingServiceIdxStoreName,handle->stmtXieAdminCol.IndexingServiceIdxStoreName);
			strcpy(xieadmin->IndexingLibID,handle->stmtXieAdminCol.IndexingLibID);
			strcpy(xieadmin->IndexingLibName,handle->stmtXieAdminCol.IndexingLibName);
			strcpy(xieadmin->IndexingLibVersion,handle->stmtXieAdminCol.IndexingLibVersion);
			strcpy(xieadmin->IndexingLibDesc,handle->stmtXieAdminCol.IndexingLibDesc);
		}
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieAdminListCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminListCreate(XMLIMAPI_XieAdminList *list)
	{
		XMLIMAPI_Bool			ok = XMLIMAPI_False;
		XMLIMAPI_XieAdminList	newList = NULL;

		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_xieadmin_list), (XMLIMAPI_UInt1**) &newList, &ok );
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
		Function:	XMLIMAPI_XieAdminListDestroy
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminListDestroy(XMLIMAPI_XieAdminList list)
	{
		XMLIMAPI_XieAdminListItem	current,next;
		
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
		Function:	XMLIMAPI_XieAdminListInit
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminListInit(XMLIMAPI_XieAdminList list)
	{
		XMLIMAPI_XieAdminListItem	current,next;
		
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
		Function:	XMLIMAPI_XieAdminListAddItem
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminListAddItem(XMLIMAPI_XieAdminList	list, 
												 XMLIMAPI_XieAdmin		*XieAdmin)
	{
		XMLIMAPI_XieAdminListItem		newListItem;
		XMLIMAPI_XieAdminP				newItem;
		XMLIMAPI_Bool					ok = XMLIMAPI_False;
		
		if (!list) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_xieadmin_list_item), (XMLIMAPI_UInt1**) &newListItem, &ok );
		if (ok == XMLIMAPI_True) {

			sqlallocat( sizeof(struct st_api_xie_admin), (XMLIMAPI_UInt1**) &newItem, &ok );
			if (ok == XMLIMAPI_True) {

				strcpy(newItem->ID,XieAdmin->ID);
				strcpy(newItem->XieName,XieAdmin->XieName);
				strcpy(newItem->description,XieAdmin->description);
				strcpy(newItem->Node,XieAdmin->Node);
				strcpy(newItem->InstRoot,XieAdmin->InstRoot);
				strcpy(newItem->IndexingServiceID,XieAdmin->IndexingServiceID);
				strcpy(newItem->IndexingServiceName,XieAdmin->IndexingServiceName);
				strcpy(newItem->IndexingServiceDesc,XieAdmin->IndexingServiceDesc);
				strcpy(newItem->IndexingServiceDocStoreName,XieAdmin->IndexingServiceDocStoreName);
				strcpy(newItem->IndexingServiceIdxStoreName,XieAdmin->IndexingServiceIdxStoreName);
				strcpy(newItem->IndexingLibID,XieAdmin->IndexingLibID);
				strcpy(newItem->IndexingLibName,XieAdmin->IndexingLibName);
				strcpy(newItem->IndexingLibDesc,XieAdmin->IndexingLibDesc);
				strcpy(newItem->IndexingLibVersion,XieAdmin->IndexingLibVersion);
				strcpy(newItem->LogFile,XieAdmin->LogFile);
				strcpy(newItem->Alive,XieAdmin->Alive);
				strcpy(newItem->Refresh,XieAdmin->Refresh);

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
		Function:	XMLIMAPI_XieAdminListGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminListGetFirst(XMLIMAPI_XieAdminList	list, 
												  XMLIMAPI_XieAdmin		**XieAdmin)
	{
		if(!XieAdmin)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		*XieAdmin = list->first->item;
		list->curr = list->first;
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XieAdminListGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminListGetNext(XMLIMAPI_XieAdminList		list, 
												 XMLIMAPI_XieAdmin			**XieAdmin)
	{
		if(!XieAdmin)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		if(!list->curr)
			return XMLIMAPI_False;
		list->curr = list->curr->next;
		if(!list->curr)
			return XMLIMAPI_False;
		*XieAdmin = list->curr->item;
		return XMLIMAPI_True;

	}

