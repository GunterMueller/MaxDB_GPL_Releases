/****************************************************************************

  module      : XMLIMAPI_DocClass.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-29  10:32
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


	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_DocClass.h"
	#include "SAPDBCommon/SAPDB_string.h"


	#define SQL_INSERT_DOCCLASS \
			"INSERT INTO XML_DOCUMENTCLASS \
				SET DCID = ?, \
					NAME = ?, \
					DESCRIPTION = ?"

	#define SQL_UPDATE_DOCCLASS \
			"UPDATE XML_DOCUMENTCLASS \
				SET DESCRIPTION = ? \
			 WHERE \
				DCID = ?"

	#define SQL_DELETE_DOCCLASS \
			"DELETE XML_DOCUMENTCLASS \
			 WHERE DCID = ?"

	#define SQL_GET_DOCCLASS \
			"SELECT	\
				D.\"DCID\", \
				D.\"NAME\", \
				D.\"DESCRIPTION\" \
			 FROM	\
				\"XML_DOCUMENTCLASS\" D\
			 WHERE	\
				D.\"DCID\" = ?"

	#define	SQL_GET_DOCCLASS_ID \
			"SELECT	\
				D.\"DCID\" \
			 FROM	\
				\"XML_DOCUMENTCLASS\" D\
			 WHERE	\
				D.\"NAME\" = ?"

	#define	SQL_GET_DOCCLASS_NAME \
			"SELECT	\
				D.\"NAME\" \
			 FROM	\
				\"XML_DOCUMENTCLASS\" D\
			 WHERE	\
				D.\"DCID\" = ?"

	#define SQL_GET_DOCCLASSES \
			"SELECT	\
				D.\"DCID\", \
				D.\"NAME\", \
				D.\"DESCRIPTION\" \
			FROM \
				\"XML_DOCUMENTCLASS\" D\
			ORDER BY D.\"NAME\""

	#define SQL_GET_DOCCLASSES_EXCL \
			"SELECT	\
				D.\"DCID\", \
				D.\"NAME\", \
				D.\"DESCRIPTION\" \
			FROM \
				\"XML_DOCUMENTCLASS\" D\
			%s\
			ORDER BY D.\"NAME\""

	#define SQL_GET_DOCCLASSES_INCL \
			"SELECT	\
				D.\"DCID\", \
				D.\"NAME\", \
				D.\"DESCRIPTION\" \
			FROM \
				\"XML_DOCUMENTCLASS\" D\
			%s\
			ORDER BY D.\"NAME\""

	#define SQL_DELETE_ASSIGNMENT \
			"DELETE \
				\"XML_ASSIGN_DC_IDX\"\
			 WHERE \
				\"DCID\" = ?"

	#define SQL_INSERT_ASSIGNMENT \
			"INSERT INTO \
				\"XML_ASSIGN_DC_IDX\" \
			 SET \
				DCID = ?,\
				IDXID = ?"

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassCreate(XMLIMAPI_Handle	handle,
											XMLIMAPI_Char		*name,
											XMLIMAPI_Char		*description,
										    XMLIMAPI_XmlIndexList	assigned)
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

		if (!handle->stmtDocClassCreate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_INSERT_DOCCLASS, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtDocClassParam.docClassName, 
							  129,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtDocClassParam.description, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtDocClassCreate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtDocClassCreate, SQL_CLOSE );
		}

		/* set values of parameters */
		/* - get unique id for doc class */
		if (!XMLIMAPI_IdGetNext(handle,handle->stmtDocClassID)) {
			return XMLIMAPI_False;
		}

		strcpy(handle->stmtDocClassParam.docClassName,name);
		strcpy(handle->stmtDocClassParam.description,description);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtDocClassCreate)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtDocClassCreate, state, text, &nativeODBCCode);
			if (nativeODBCCode==250) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_DOCCLASS_ALLREADY_EXISTS,
							 XMLIMAPI_ERR_MAPI_TEXT_DOCCLASS_ALLREADY_EXISTS);
			} else {
				addSQLErrorItem(handle, handle->stmtDocClassCreate, rc);
				SQLFreeStmt( handle->stmtDocClassCreate, SQL_DROP );
				handle->stmtDocClassCreate = 0;
			}
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}


		/* 2. assign XML Indices to new Class */
		if (!docClassAssignXmlIndex(handle,
									handle->stmtDocClassID,
									assigned)) {
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassUpdate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassUpdate(XMLIMAPI_Handle	handle,
											XMLIMAPI_Id		id,
											XMLIMAPI_Char		*description,
										    XMLIMAPI_XmlIndexList	assigned)
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

		if (!handle->stmtDocClassUpdate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_UPDATE_DOCCLASS, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtDocClassParam.description, 
							  XMLIMAPI_MAX_ID_LEN,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtDocClassUpdate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtDocClassUpdate, SQL_CLOSE );
		}

		/* set values of parameters */
		SAPDB_memcpy(handle->stmtDocClassID,id,XMLIMAPI_MAX_ID_LEN);
		strcpy(handle->stmtDocClassParam.description,description);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtDocClassUpdate)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtDocClassUpdate, rc);
			SQLFreeStmt( handle->stmtDocClassUpdate, SQL_DROP );
			handle->stmtDocClassUpdate = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		/* 2. assign XML Indices to new Class */
		if (!docClassAssignXmlIndex(handle,
									handle->stmtDocClassID,
									assigned)) {
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassDelete
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassDelete(XMLIMAPI_Handle	handle,
											XMLIMAPI_Id		id)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_XmlIndexList		oldAssignments;


		/* clean up */
		Rollback(handle);
	
		/* 1. delete all assigned XML Indices not assigned to any other doc class */
		XMLIMAPI_XmlIndexListCreate(&oldAssignments);
		if (!docClassAssignXmlIndex(handle,
									id,
									oldAssignments)) {
			Rollback(handle);
			return XMLIMAPI_False;
		}
		XMLIMAPI_XmlIndexListDestroy(oldAssignments);

		/* 2. delete docclass */
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtDocClassDelete) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_DELETE_DOCCLASS, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtDocClassDelete = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtDocClassDelete, SQL_CLOSE );
		}

		/* set values of parameters */
		SAPDB_memcpy(handle->stmtDocClassID,id,XMLIMAPI_MAX_ID_LEN);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtDocClassDelete)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtDocClassDelete, rc);
			SQLFreeStmt( handle->stmtDocClassDelete, SQL_DROP );
			handle->stmtDocClassDelete = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassAssignXmlIndex
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassAssignXmlIndex(XMLIMAPI_Handle	handle,
													XMLIMAPI_Id		docClassId,
													XMLIMAPI_XmlIndexList	assigned)
	{
		if (!docClassAssignXmlIndex(handle,
									docClassId,
									assigned)) {
			Rollback(handle);
			return XMLIMAPI_False;
		}
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_DocClassGet
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGet(XMLIMAPI_Handle	handle,
										 XMLIMAPI_Id		id,
										 XMLIMAPI_DocClass *docClass)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtGetDocClass) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_DOCCLASS, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtDocClassCol.docClassID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtDocClassCol.docClassName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtDocClassCol.description), 
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtGetDocClass = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtGetDocClass, SQL_CLOSE );
		}

		/* set values of parameters */
		SAPDB_memcpy(handle->stmtDocClassID,id,XMLIMAPI_MAX_ID_LEN);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetDocClass)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetDocClass, rc);
			SQLFreeStmt( handle->stmtGetDocClass, SQL_DROP );
			handle->stmtGetDocClass = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetDocClass);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(docClass->docClassID,
					   handle->stmtDocClassCol.docClassID);
				strcpy(docClass->docClassName,
					   handle->stmtDocClassCol.docClassName);
				strcpy(docClass->description,
					   handle->stmtDocClassCol.description);

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_DOCCLASS);
				} else {
					addSQLErrorItem(handle, handle->stmtGetDocClass, rc);
					SQLFreeStmt( handle->stmtGetDocClass, SQL_DROP );
					handle->stmtGetDocClass = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassGetId
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetId(XMLIMAPI_Handle		handle,
										   XMLIMAPI_Char		*docClassName,
										   XMLIMAPI_Id			*docClassId)
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

		if (!handle->stmtGetDocClassId) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_DOCCLASS_ID, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol( SqlStmt, 1, SQL_C_BINARY, docClassId, XMLIMAPI_MAX_ID_LEN, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtDocClassParam.docClassName, 
							  129,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtGetDocClassId = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtGetDocClassId, SQL_CLOSE );
		}

		/* set value of parameter */
		strcpy(handle->stmtDocClassParam.docClassName,docClassName);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetDocClassId)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetDocClassId, rc);
			SQLFreeStmt( handle->stmtGetDocClassId, SQL_DROP );
			handle->stmtGetDocClassId = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetDocClassId);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_DOCCLASS);
				} else {
					addSQLErrorItem(handle, handle->stmtGetDocClassId, rc);
					SQLFreeStmt( handle->stmtGetDocClassId, SQL_DROP );
					handle->stmtGetDocClassId = 0;
				}
				return XMLIMAPI_False;
			}
		}
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassGetName
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetName(XMLIMAPI_Handle		handle,
										   XMLIMAPI_Id			id,
										   XMLIMAPI_Char		*docClassName)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		long						foobar;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_IdString			idString;
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtGetDocClassName) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_DOCCLASS_NAME, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&docClassName, 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtGetDocClassName = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtGetDocClassName, SQL_CLOSE );
		}

		/* set values of parameters */
		SAPDB_memcpy(handle->stmtDocClassID,id,XMLIMAPI_MAX_ID_LEN);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetDocClassName)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetDocClassName, rc);
			SQLFreeStmt( handle->stmtGetDocClassName, SQL_DROP );
			handle->stmtGetDocClassName = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetDocClassName);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_DOCCLASS);
					XMLIMAPI_IdAsString(id,idString);
					strcpy(docClassName,idString);
				} else {
					addSQLErrorItem(handle, handle->stmtGetDocClassName, rc);
					SQLFreeStmt( handle->stmtGetDocClassName, SQL_DROP );
					handle->stmtGetDocClassName = 0;
				}
				return XMLIMAPI_False;
			}
		}
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_DocClassGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetFirst(XMLIMAPI_Handle	handle,
											  XMLIMAPI_DocClass *docClass)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtGetDocClasses) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_DOCCLASSES, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtDocClassCol.docClassID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtDocClassCol.docClassName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtDocClassCol.description), 
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtGetDocClasses = SqlStmt;

		} else {
			SQLFreeStmt(handle->stmtGetDocClasses, SQL_CLOSE );
		}

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetDocClasses)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetDocClasses, rc);
			SQLFreeStmt( handle->stmtGetDocClasses, SQL_DROP );
			handle->stmtGetDocClasses = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetDocClasses);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(docClass->docClassID,
					   handle->stmtDocClassCol.docClassID);
				strcpy(docClass->docClassName,
					   handle->stmtDocClassCol.docClassName);
				strcpy(docClass->description,
					   handle->stmtDocClassCol.description);

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_DOCCLASS);
				} else {
					addSQLErrorItem(handle, handle->stmtGetDocClasses, rc);
					SQLFreeStmt( handle->stmtGetDocClasses, SQL_DROP );
					handle->stmtGetDocClasses = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_DocClassGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetNext(XMLIMAPI_Handle	handle,
											 XMLIMAPI_DocClass *docClass)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtGetDocClasses;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(docClass->docClassID,
				   handle->stmtDocClassCol.docClassID);
			strcpy(docClass->docClassName,
				   handle->stmtDocClassCol.docClassName);
			strcpy(docClass->description,
				   handle->stmtDocClassCol.description);

			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_DOCCLASSES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_DOCCLASSES);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				handle->stmtGetDocClasses = 0;
			}
			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_DocClassExclGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassExclGetFirst(XMLIMAPI_Handle	handle,
												XMLIMAPI_DocClassList exclude,
									  			XMLIMAPI_DocClass *docClass)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_Char				__temp[MAX_SQLSTR_LEN_WD00],__temp2[MAX_SQLSTR_LEN_WD00],__where[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_DocClass			*__docClass;
		XMLIMAPI_Bool				__first;

	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (handle->stmtGetDocClassesExcl != 0)
			SQLFreeStmt(handle->stmtGetDocClassesExcl, SQL_CLOSE );

		rc = SQLAllocStmt(SqlDBC, &SqlStmt);
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		/* build the where clause depending on the exclude list */
		if (XMLIMAPI_DocClassListGetFirst(exclude,&__docClass)) {
			strcpy(__temp,"WHERE DCID <> X'%s' %s");
			__first = XMLIMAPI_True;
			do {
				if (__first) {
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,__docClass->docClassID,"%s %s %s");
					__first = XMLIMAPI_False;
				} else {
					sp77sprintf(__temp2,MAX_SQLSTR_LEN_WD00,__temp,"AND","DCID <> X'%s'","%s");
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp2,__docClass->docClassID,"%s %s %s");
				}
				strcpy(__temp,__where);
			} while(XMLIMAPI_DocClassListGetNext(exclude,&__docClass));
			sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,"","","");
		} else {
			strcpy(__where,"");
		}
		sp77sprintf(stmtStr,MAX_SQLSTR_LEN_WD00,SQL_GET_DOCCLASSES_EXCL,__where);

		rc = SQLPrepare(SqlStmt, (SQLCHAR*)stmtStr, SQL_NTS );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}
				
		rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
						&(handle->stmtDocClassCol.docClassID), 
						55, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
						&(handle->stmtDocClassCol.docClassName), 
						129, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
						&(handle->stmtDocClassCol.description), 
						513, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		handle->stmtGetDocClassesExcl = SqlStmt;

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetDocClassesExcl)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetDocClassesExcl, rc);
			SQLFreeStmt( handle->stmtGetDocClassesExcl, SQL_DROP );
			handle->stmtGetDocClassesExcl = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetDocClassesExcl);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(docClass->docClassID,
					   handle->stmtDocClassCol.docClassID);
				strcpy(docClass->docClassName,
					   handle->stmtDocClassCol.docClassName);
				strcpy(docClass->description,
					   handle->stmtDocClassCol.description);

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_DOCCLASS);
				} else {
					addSQLErrorItem(handle, handle->stmtGetDocClassesExcl, rc);
					SQLFreeStmt( handle->stmtGetDocClassesExcl, SQL_DROP );
					handle->stmtGetDocClassesExcl = 0;
				}
				return XMLIMAPI_False;
			}
		}
 	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_DocClassExclGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassExclGetNext(XMLIMAPI_Handle	handle,
											   XMLIMAPI_DocClass *docClass)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtGetDocClassesExcl;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(docClass->docClassID,
				   handle->stmtDocClassCol.docClassID);
			strcpy(docClass->docClassName,
				   handle->stmtDocClassCol.docClassName);
			strcpy(docClass->description,
				   handle->stmtDocClassCol.description);

			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_DOCCLASSES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_DOCCLASSES);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				handle->stmtGetDocClassesExcl = 0;
			}
			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_DocClassInclGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassInclGetFirst(XMLIMAPI_Handle	handle,
												XMLIMAPI_DocClassList include,
									  			XMLIMAPI_DocClass *docClass)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_Char				__temp[MAX_SQLSTR_LEN_WD00],__temp2[MAX_SQLSTR_LEN_WD00],__where[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_DocClass			*__docClass;
		XMLIMAPI_Bool				__first;

	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (handle->stmtGetDocClassesIncl != 0)
			SQLFreeStmt(handle->stmtGetDocClassesIncl, SQL_CLOSE );

		rc = SQLAllocStmt(SqlDBC, &SqlStmt);
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		/* build the where clause depending on the exclude list */
		if (XMLIMAPI_DocClassListGetFirst(include,&__docClass)) {
			strcpy(__temp,"WHERE DCID = X'%s' %s");
			__first = XMLIMAPI_True;
			do {
				if (__first) {
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,__docClass->docClassID,"%s %s %s");
					__first = XMLIMAPI_False;
				} else {
					sp77sprintf(__temp2,MAX_SQLSTR_LEN_WD00,__temp,"OR","DCID = X'%s'","%s");
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp2,__docClass->docClassID,"%s %s %s");
				}
				strcpy(__temp,__where);
			} while(XMLIMAPI_DocClassListGetNext(include,&__docClass));
			sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,"","","");
		} else {
			strcpy(__where,"");
		}
		sp77sprintf(stmtStr,MAX_SQLSTR_LEN_WD00,SQL_GET_DOCCLASSES_INCL,__where);

		rc = SQLPrepare(SqlStmt, (SQLCHAR*)stmtStr, SQL_NTS );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}
				
		rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
						&(handle->stmtDocClassCol.docClassID), 
						55, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
						&(handle->stmtDocClassCol.docClassName), 
						129, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
						&(handle->stmtDocClassCol.description), 
						513, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		handle->stmtGetDocClassesIncl = SqlStmt;

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetDocClassesIncl)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetDocClassesIncl, rc);
			SQLFreeStmt( handle->stmtGetDocClassesIncl, SQL_DROP );
			handle->stmtGetDocClassesIncl = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetDocClassesIncl);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(docClass->docClassID,
					   handle->stmtDocClassCol.docClassID);
				strcpy(docClass->docClassName,
					   handle->stmtDocClassCol.docClassName);
				strcpy(docClass->description,
					   handle->stmtDocClassCol.description);

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_DOCCLASS);
				} else {
					addSQLErrorItem(handle, handle->stmtGetDocClassesIncl, rc);
					SQLFreeStmt( handle->stmtGetDocClassesIncl, SQL_DROP );
					handle->stmtGetDocClassesIncl = 0;
				}
				return XMLIMAPI_False;
			}
		}
 	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_DocClassInclGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassInclGetNext(XMLIMAPI_Handle	handle,
											   XMLIMAPI_DocClass *docClass)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtGetDocClassesIncl;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(docClass->docClassID,
				   handle->stmtDocClassCol.docClassID);
			strcpy(docClass->docClassName,
				   handle->stmtDocClassCol.docClassName);
			strcpy(docClass->description,
				   handle->stmtDocClassCol.description);

			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_DOCCLASSES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_DOCCLASSES);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				handle->stmtGetDocClassesIncl = 0;
			}
			return XMLIMAPI_False;
		}

	}


	/*------------------------------------------------------------------------
		HELPER FUNCTIONS
	--------------------------------------------------------------------------*/

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassListCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassListCreate(XMLIMAPI_DocClassList *list)
	{
		XMLIMAPI_Bool			ok = XMLIMAPI_False;
		XMLIMAPI_DocClassList	newList = NULL;

		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_docclass_list), (XMLIMAPI_UInt1**) &newList, &ok );
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
		Function:	XMLIMAPI_DocClassListDestroy
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassListDestroy(XMLIMAPI_DocClassList list)
	{
		XMLIMAPI_DocClassListItem	current,next;
		
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
		Function:	XMLIMAPI_DocClassListInit
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassListInit(XMLIMAPI_DocClassList list)
	{
		XMLIMAPI_DocClassListItem	current,next;
		
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
		Function:	XMLIMAPI_DocClassListAddItem
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassListAddItem(XMLIMAPI_DocClassList	list, 
												 XMLIMAPI_DocClass		*docClass)
	{
		XMLIMAPI_DocClassListItem		newListItem;
		XMLIMAPI_DocClassP				newItem;
		XMLIMAPI_Bool					ok = XMLIMAPI_False;
		
		if (!list) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_docclass_list_item), (XMLIMAPI_UInt1**) &newListItem, &ok );
		if (ok == XMLIMAPI_True) {

			sqlallocat( sizeof(struct st_api_doc_class), (XMLIMAPI_UInt1**) &newItem, &ok );
			if (ok == XMLIMAPI_True) {

				strcpy(newItem->docClassID,docClass->docClassID);
				strcpy(newItem->docClassName,docClass->docClassName);
				strcpy(newItem->description,docClass->description);

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
		Function:	XMLIMAPI_DocClassListGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassListGetFirst(XMLIMAPI_DocClassList	list, 
												  XMLIMAPI_DocClass		**docClass)
	{
		if(!docClass)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		*docClass = list->first->item;
		list->curr = list->first;
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_DocClassListGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassListGetNext(XMLIMAPI_DocClassList		list, 
												 XMLIMAPI_DocClass			**docClass)
	{
		if(!docClass)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		if(!list->curr)
			return XMLIMAPI_False;
		list->curr = list->curr->next;
		if(!list->curr)
			return XMLIMAPI_False;
		*docClass = list->curr->item;
		return XMLIMAPI_True;

	}

	/*------------------------------------------------------------------------
		Function:	docClassAssignXmlIndex
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool docClassAssignXmlIndex(XMLIMAPI_Handle		handle,
										  XMLIMAPI_Id			docClassId,
										  XMLIMAPI_XmlIndexList	assigned)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		int							i;
		XMLIMAPI_XmlIndexList		oldAssignments;
		XMLIMAPI_XmlIndex			xmlIndex;
		XMLIMAPI_XmlIndex			*__xmlIndex;
		XMLIMAPI_Id					__xmlIndexId;

		XMLIMAPI_XmlIndexListCreate(&oldAssignments);
		
		if (assigned) {

			/* get all xml indexes assigned to this document class */
			if (XMLIMAPI_XmlIndexGetFirstByDocClass(handle,docClassId,&xmlIndex)) {
				XMLIMAPI_XmlIndexListCreate(&oldAssignments);
				do {
					XMLIMAPI_XmlIndexListAddItem(oldAssignments,&xmlIndex);
				} while (XMLIMAPI_XmlIndexGetNextByDocClass(handle,&xmlIndex));
			}

			/* 1. delete all assignments on database not valid any more */
			if(!deleteAssignedXmlIndices(handle,docClassId)) {
				XMLIMAPI_XmlIndexListDestroy(oldAssignments);
				return XMLIMAPI_False;
			}

			/* 2. insert the new assignments not existing on database */
			if(!insertAssignedXmlIndices(handle,docClassId,assigned)) {
				XMLIMAPI_XmlIndexListDestroy(oldAssignments);
				return XMLIMAPI_False;
			}

			/* 3. delete all old assignments no longer assigned to any other document class */
			if (XMLIMAPI_XmlIndexListGetFirst(oldAssignments,&__xmlIndex)) {
				do {
					XMLIMAPI_IdStringAsId(__xmlIndex->IndexID,__xmlIndexId);
					if (XMLIMAPI_XmlIndexCheckAssign(handle,__xmlIndexId)) {
						/* is not assigned anymore -> delete it */
						XMLIMAPI_XmlIndexDelete(handle,__xmlIndexId,XMLIMAPI_False);
					}
				} while(XMLIMAPI_XmlIndexListGetNext(oldAssignments,&__xmlIndex));
			}

		}

		XMLIMAPI_XmlIndexListDestroy(oldAssignments);

		return XMLIMAPI_True;
	}


	/*------------------------------------------------------------------------
		Function:	deleteAssignedXmlIndices
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool deleteAssignedXmlIndices(XMLIMAPI_Handle    handle,
											XMLIMAPI_Id		docClassId)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_ErrorItem			errorItem;
		XMLIMAPI_ErrorState		errorState;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtDocClassDeleteAssign) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_DELETE_ASSIGNMENT, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtDocClassDeleteAssign = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtDocClassDeleteAssign, SQL_CLOSE );
		}

		/* set values of parameters */
		SAPDB_memcpy(handle->stmtDocClassID,docClassId,XMLIMAPI_MAX_ID_LEN);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtDocClassDeleteAssign)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtDocClassDeleteAssign, rc);
			XMLIMAPI_GetLastError(handle,&errorItem);
			XMLIMAPI_GetErrorState(errorItem,&errorState);
			if (strcmp(errorState,XMLIMAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)!=0 &&
				strcmp(errorState,XMLIMAPI_ERR_SQL_STATE_MORE_THAN_ONE_ROW_UPDATE)!=0) {
				SQLFreeStmt( handle->stmtDocClassDeleteAssign, SQL_DROP );
				handle->stmtDocClassDeleteAssign = 0;
				return XMLIMAPI_False;
			} 
		}

		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	insertAssignedXmlIndices
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool insertAssignedXmlIndices(XMLIMAPI_Handle    handle,
											XMLIMAPI_Id		docClassId,
											XMLIMAPI_XmlIndexList	list)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_XmlIndex			*__xmlIndex;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtDocClassInsertAssign) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_INSERT_ASSIGNMENT, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXmlIndexID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtDocClassInsertAssign = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtDocClassInsertAssign, SQL_CLOSE );
		}

		/* set values of parameters */
		SAPDB_memcpy(handle->stmtDocClassID,docClassId,XMLIMAPI_MAX_ID_LEN);

		/* loop the list of xml indexes to insert them one by one */
		if (XMLIMAPI_XmlIndexListGetFirst(list,&__xmlIndex)) {
			do {
				XMLIMAPI_IdStringAsId(__xmlIndex->IndexID,handle->stmtXmlIndexID);
				/* execute statement */
				if ((rc = SQLExecute(handle->stmtDocClassInsertAssign)) != SQL_SUCCESS) {
					addSQLErrorItem(handle, handle->stmtDocClassInsertAssign, rc);
					if (!XMLIMAPI_IsError(handle, XMLIMAPI_ERR_TYPE_SQL, XMLIMAPI_ERR_SQL_CODE_DUPLICATE_KEY)) {
						SQLFreeStmt( handle->stmtDocClassInsertAssign, SQL_DROP );
						handle->stmtDocClassInsertAssign = 0;
						return XMLIMAPI_False;
					}
				} else {
					SQLFreeStmt( handle->stmtDocClassInsertAssign, SQL_CLOSE );
				}
			} while (XMLIMAPI_XmlIndexListGetNext(list,&__xmlIndex));
		}

		return XMLIMAPI_True;
	}


