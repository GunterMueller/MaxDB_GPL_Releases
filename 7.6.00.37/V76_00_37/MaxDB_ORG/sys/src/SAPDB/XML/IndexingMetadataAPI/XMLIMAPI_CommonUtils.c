/****************************************************************************

  module      : XMLIMAPI_CommonUtils.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-02  10:00
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

	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_CommonUtils.h"

	#define	SQL_ROLLBACK_WORK \
			"ROLLBACK"

	#define	SQL_COMMIT_WORK \
			"COMMIT"

					
	/*------------------------------------------------------------------------
		Function: Commit
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool Commit(XMLIMAPI_Handle handle)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtCommit) {

			/* Allocat statement handle */
			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if ( rc != 0 ) {
				addSQLErrorItem(handle, SqlStmt, rc);
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_COMMIT_WORK, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtCommit = SqlStmt;
		} else {
			SQLFreeStmt( handle->stmtCommit, SQL_CLOSE );
		}

		/* Execute statement */
		rc = SQLExecute(handle->stmtCommit);
		if ( rc != 0 ) {
			addSQLErrorItem(handle, handle->stmtCommit, rc);
			SQLFreeStmt(handle->stmtCommit, SQL_DROP);
			return XMLIMAPI_False;
		}

		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function: Rollback
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool Rollback(XMLIMAPI_Handle handle)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtRollback) {

			/* Allocat statement handle */
			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if ( rc != 0 ) {
				addSQLErrorItem(handle, SqlStmt, rc);
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_ROLLBACK_WORK, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtRollback = SqlStmt;
		} else {
			SQLFreeStmt( handle->stmtRollback, SQL_CLOSE );
		}

		/* Execute statement */
		rc = SQLExecute(handle->stmtRollback);
		if ( rc != 0 ) {
			addSQLErrorItem(handle, handle->stmtRollback, rc);
			SQLFreeStmt(handle->stmtRollback, SQL_DROP);
			return XMLIMAPI_False;
		}

		return XMLIMAPI_True;
	}
			
	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_createHandle
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_createHandle(XMLIMAPI_Handle *handle, SQLHENV henv, SQLHDBC hDBC)
	{
		XMLIMAPI_Bool				ok = XMLIMAPI_False;
		XMLIMAPI_Handle	newHandle = NULL;

		if ( !handle ) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_xmlimapi_handle), (XMLIMAPI_UInt1**) &newHandle, &ok );
		if (ok == XMLIMAPI_True) {

			newHandle->sqlHdbc = hDBC;
			newHandle->sqlHenv = henv;

			/* statement handles */
			newHandle->stmtCommit = 0;
			newHandle->stmtRollback = 0;
			newHandle->stmtXmlIndexCreate = 0;
			newHandle->stmtXmlIndexUpdate = 0;
			newHandle->stmtXmlIndexDelete = 0;
			newHandle->stmtGetXmlIndex = 0;
			newHandle->stmtGetXmlIndices = 0;
			newHandle->stmtGetXmlIndicesIncl = 0;
			newHandle->stmtIndicesForDC = 0;
			newHandle->stmtIndicesForDCs = 0;
			newHandle->stmtIndicesForDCInfoExcl = 0;
			newHandle->stmtXmlIndexCheckAssign = 0;

			newHandle->stmtDocClassCreate = 0;
			newHandle->stmtDocClassUpdate = 0;
			newHandle->stmtDocClassDelete = 0;
			newHandle->stmtGetDocClass = 0;
			newHandle->stmtGetDocClassId = 0;
			newHandle->stmtGetDocClassName = 0;
			newHandle->stmtGetDocClasses = 0;
			newHandle->stmtGetDocClassesExcl = 0;
			newHandle->stmtGetDocClassesIncl = 0;
			newHandle->stmtDocClassDeleteAssign = 0;
			newHandle->stmtDocClassInsertAssign = 0;

			newHandle->stmtServiceCreate = 0;
			newHandle->stmtServiceUpdate = 0;
			newHandle->stmtServiceDelete = 0;
			newHandle->stmtServiceGet = 0;
			newHandle->stmtServiceGetId = 0;
			newHandle->stmtServiceGetList = 0;
			newHandle->stmtServiceGetByName = 0;
			
			newHandle->stmtSessionPoolCreate = 0;
			newHandle->stmtSessionPoolUpdate = 0;
			newHandle->stmtSessionPoolDelete = 0;
			newHandle->stmtSessionPoolCheckIS = 0;
			newHandle->stmtSessionPoolGet = 0;
			newHandle->stmtSessionPoolGetList = 0;
			newHandle->stmtSessionPoolGetAssign = 0;

			newHandle->stmtXieRegisterInsertIdxLib = 0;
			newHandle->stmtXieRegisterUpdateIdxLib = 0;
			newHandle->stmtXieRegisterInsertXie = 0;
			newHandle->stmtXieRegisterUpdateXie = 0;
			newHandle->stmtXieSetAlive = 0;
			newHandle->stmtXieGetRefresh = 0;
			newHandle->stmtXieUnSetRefresh = 0;
			newHandle->stmtXieUnRegister = 0;
			newHandle->stmtXieGet = 0;
			newHandle->stmtXieGetList = 0;

			/* helpers outside of existing structures */
			newHandle->stmtIdIndicator = XMLIMAPI_MAX_ID_LEN;
			strcpy(newHandle->stmtXmlIndexID,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
			strcpy(newHandle->stmtDocClassID,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
			strcpy(newHandle->stmtXmlIndexID2,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
			strcpy(newHandle->stmtXmlIndexID3,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
			strcpy(newHandle->stmtXmlIndexID4,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
			strcpy(newHandle->stmtXmlIndexID5,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");

			/* XmlIndex for BindCol */
			strcpy(newHandle->stmtXmlIndexCol.IndexID,"");
			strcpy(newHandle->stmtXmlIndexCol.indexName,"");
			strcpy(newHandle->stmtXmlIndexCol.description,"");
			strcpy(newHandle->stmtXmlIndexCol.xpathBasePath,"");
			strcpy(newHandle->stmtXmlIndexCol.xpathValuePath,"");

			/* XmlIndexLong for BindParam */
			strcpy(newHandle->stmtXmlIndexParam.IndexID,"");
			strcpy(newHandle->stmtXmlIndexParam.indexName,"");
			strcpy(newHandle->stmtXmlIndexParam.description,"");
			strcpy(newHandle->stmtXmlIndexParam.xpathBasePath,"");
			strcpy(newHandle->stmtXmlIndexParam.xpathValuePath,"");
			
			/* DocClass for BindParam */
			strcpy(newHandle->stmtDocClassParam.docClassID,"");
			strcpy(newHandle->stmtDocClassParam.docClassName,"");
			strcpy(newHandle->stmtDocClassParam.description,"");

			/* DocClass for BindCol */
			strcpy(newHandle->stmtDocClassCol.docClassID,"");
			strcpy(newHandle->stmtDocClassCol.docClassName,"");
			strcpy(newHandle->stmtDocClassCol.description,"");

			/* Service for BindParam */
			strcpy(newHandle->stmtServiceParam.ServiceID,"");
			strcpy(newHandle->stmtServiceParam.ServiceName,"");
			strcpy(newHandle->stmtServiceParam.ServiceDesc,"");
			strcpy(newHandle->stmtServiceParam.DocStoreID,"");
			strcpy(newHandle->stmtServiceParam.IdxStoreID,"");
			strcpy(newHandle->stmtServiceParam.isSynchronous,"");

			/* Service for BindCol */
			strcpy(newHandle->stmtServiceCol.ServiceID,"");
			strcpy(newHandle->stmtServiceCol.ServiceName,"");
			strcpy(newHandle->stmtServiceCol.ServiceDesc,"");
			strcpy(newHandle->stmtServiceCol.DocStoreID,"");
			strcpy(newHandle->stmtServiceCol.IdxStoreID,"");
			strcpy(newHandle->stmtServiceCol.isSynchronous,"");

			/* SessionPool for BindParam */
			strcpy(newHandle->stmtSessionPoolParam.PoolID,"");
			strcpy(newHandle->stmtSessionPoolParam.PoolName,"");
			strcpy(newHandle->stmtSessionPoolParam.description,"");
			strcpy(newHandle->stmtSessionPoolParam.DbNode,"");
			strcpy(newHandle->stmtSessionPoolParam.DbName,"");
			strcpy(newHandle->stmtSessionPoolParam.DbUser,"");
			strcpy(newHandle->stmtSessionPoolParam.DbPassword,"");
			strcpy(newHandle->stmtSessionPoolParam.TraceFile,"");
			strcpy(newHandle->stmtSessionPoolParam.Assignments,"");

			/* SessionPool for BindCol */
			strcpy(newHandle->stmtSessionPoolCol.PoolID,"");
			strcpy(newHandle->stmtSessionPoolCol.PoolName,"");
			strcpy(newHandle->stmtSessionPoolCol.description,"");
			strcpy(newHandle->stmtSessionPoolCol.DbNode,"");
			strcpy(newHandle->stmtSessionPoolCol.DbName,"");
			strcpy(newHandle->stmtSessionPoolCol.DbUser,"");
			strcpy(newHandle->stmtSessionPoolCol.DbPassword,"");
			strcpy(newHandle->stmtSessionPoolCol.TraceFile,"");
			strcpy(newHandle->stmtSessionPoolCol.Assignments,"");

			/* Xie Admin for BindParam */
			strcpy(newHandle->stmtXieAdminParam.ID,"");
			strcpy(newHandle->stmtXieAdminParam.XieName,"");
			strcpy(newHandle->stmtXieAdminParam.Node,"");
			strcpy(newHandle->stmtXieAdminParam.InstRoot,"");
			strcpy(newHandle->stmtXieAdminParam.LogFile,"");
			strcpy(newHandle->stmtXieAdminParam.description,"");
			strcpy(newHandle->stmtXieAdminParam.Alive,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingLibID,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingLibName,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingLibDesc,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingLibVersion,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingServiceID,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingServiceName,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingServiceDesc,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingServiceDocStoreName,"");
			strcpy(newHandle->stmtXieAdminParam.IndexingServiceIdxStoreName,"");
			strcpy(newHandle->stmtXieAdminParam.Refresh,"");
			
			/* Xie Admin for BindCol */
			strcpy(newHandle->stmtXieAdminCol.ID,"");
			strcpy(newHandle->stmtXieAdminCol.XieName,"");
			strcpy(newHandle->stmtXieAdminCol.Node,"");
			strcpy(newHandle->stmtXieAdminCol.InstRoot,"");
			strcpy(newHandle->stmtXieAdminCol.LogFile,"");
			strcpy(newHandle->stmtXieAdminCol.description,"");
			strcpy(newHandle->stmtXieAdminCol.Alive,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingLibID,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingLibName,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingLibDesc,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingLibVersion,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingServiceID,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingServiceName,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingServiceDesc,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingServiceDocStoreName,"");
			strcpy(newHandle->stmtXieAdminCol.IndexingServiceIdxStoreName,"");
			strcpy(newHandle->stmtXieAdminCol.Refresh,"");
			
			newHandle->errorList = NULL;

			/* Construct and init error list */
			if (!createErrorList(newHandle)) {
				return XMLIMAPI_False;
			}

			/* init Id generator */
			IdInit();

			*handle = newHandle;

			return XMLIMAPI_True;
		} else {
			*handle = NULL;

			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_destroyHandle
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_destroyHandle(XMLIMAPI_Handle handle)
	{
		if ( !handle ) {
			return XMLIMAPI_False;
		}

		/* destroy error list */
		destroyErrorList(handle);

		sqlfree((XMLIMAPI_UInt1*)handle);
		return XMLIMAPI_True;
	}

	void setErrorList( XMLIMAPI_Handle        handle,
					   XMLIMAPI_ErrorList  errorList )
	{

		handle->errorList = errorList;

	}

	/*===========================================================================*/

	void getErrorList( XMLIMAPI_Handle        handle,
					   XMLIMAPI_ErrorList *errorList )
	{
		*errorList = handle->errorList;

	}

	void strMaxCopy(char    *destination,
		 			char    *source,
					size_t   sizeToCopy )
	{

		strncpy(destination, source, sizeToCopy);
		destination[sizeToCopy-1] = '\0';

	}

