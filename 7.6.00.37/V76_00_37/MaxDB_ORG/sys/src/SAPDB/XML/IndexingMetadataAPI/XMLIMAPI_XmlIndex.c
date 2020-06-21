/****************************************************************************

  module      : XMLIMAPI_XmlIndex.c

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


	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_XmlIndex.h"
	#include "SAPDBCommon/SAPDB_string.h"

	#define SQL_INSERT_XMLINDEX \
			"INSERT INTO XML_XMLINDEX \
				SET IDXID = ?, \
					NAME = ?, \
					XPATHBASE = ?, \
					XPATHVALUE = ?, \
					DESCRIPTION = ?"

	#define SQL_UPDATE_XMLINDEX \
			"UPDATE XML_XMLINDEX \
				SET XPATHBASE = ?, \
					XPATHVALUE = ?, \
					DESCRIPTION = ? \
			 WHERE \
				IDXID = ?"

	#define SQL_DELETE_XMLINDEX \
		    "DELETE XML_XMLINDEX \
			 WHERE IDXID = ?"

	#define SQL_XMLINDEX_CHECK_ASSIGN \
			"SELECT	\
				D.\"NAME\" \
			 FROM \
				\"XML_ASSIGN_DC_IDX\" A,\
				\"XML_DOCUMENTCLASS\" D\
			 WHERE \
				((A.\"IDXID\" = ?)) \
				AND \
				A.\"DCID\"= D.\"DCID\""

	#define SQL_GET_XMLINDICES \
			"SELECT	\
				X.\"IDXID\",\
				X.\"NAME\",\
				X.\"DESCRIPTION\",\
				X.\"XPATHBASE\",\
				X.\"XPATHVALUE\"\
			FROM \
				\"XML_XMLINDEX\" X\
			ORDER BY X.\"NAME\""
			
	#define SQL_GET_XMLINDICES_INCL \
			"SELECT	\
				X.\"IDXID\",\
				X.\"NAME\",\
				X.\"DESCRIPTION\",\
				X.\"XPATHBASE\",\
				X.\"XPATHVALUE\"\
			FROM \
				\"XML_XMLINDEX\" X\
			%s\
			ORDER BY X.\"NAME\""
			
	#define SQL_GET_XMLINDEX \
			"SELECT	\
				X.\"IDXID\",\
				X.\"NAME\",\
				X.\"DESCRIPTION\",\
				X.\"XPATHBASE\",\
				X.\"XPATHVALUE\"\
			 FROM \
				\"XML_XMLINDEX\" X\
			 WHERE \
				X.\"IDXID\" = ?"

	#define SQL_GET_XMLINDEX_FOR_DOCCLASS \
			"SELECT	\
			    X.\"IDXID\",\
				X.\"NAME\",\
				X.\"DESCRIPTION\",\
				X.\"XPATHBASE\",\
				X.\"XPATHVALUE\"\
			 FROM \
				\"XML_XMLINDEX\" X,\
				\"XML_ASSIGN_DC_IDX\" A\
			 WHERE \
				((A.\"DCID\" = ?))\
				AND \
				A.\"IDXID\"= X.\"IDXID\"\
			 ORDER BY\
				X.\"NAME\""
			
	#define SQL_GET_XMLINDEX_FOR_DOCCLASSES \
			"SELECT	\
			    D.\"DCID\", \
				D.\"NAME\", \
				X.\"IDXID\",\
				X.\"NAME\"\
			 FROM \
				\"XML_XMLINDEX\" X,\
				\"XML_ASSIGN_DC_IDX\" A,\
				\"XML_DOCUMENTCLASS\" D\
			 WHERE \
				%s\
				A.\"IDXID\"= X.\"IDXID\"\
				AND \
				D.\"DCID\" = A.\"DCID\""
			
	#define SQL_GET_XMLINDEX_FOR_DOCCLASS_INFO_EXCL \
			"SELECT	DISTINCT\
				X.\"IDXID\",\
				X.\"NAME\",\
				X.\"DESCRIPTION\",\
				X.\"XPATHBASE\",\
				X.\"XPATHVALUE\"\
			FROM \
				\"XML_XMLINDEX\" X\
			WHERE \
				%s\
			ORDER BY X.\"NAME\""

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexCreate(XMLIMAPI_Handle	handle,
										  XMLIMAPI_XmlIndex	*xmlIndex)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_ErrorItem			errorItem;
		XMLIMAPI_ErrorState			errorState;
		XMLIMAPI_ErrorStateString  	state = "";
		XMLIMAPI_ErrorTextString   	text = "";
		XMLIMAPI_ErrorCode         	nativeODBCCode = 0;

		/* clean up */
		Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtXmlIndexCreate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_INSERT_XMLINDEX, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXmlIndexID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXmlIndexParam.indexName, 
							  129,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXmlIndexParam.xpathBasePath, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,4,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXmlIndexParam.xpathValuePath, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,5,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXmlIndexParam.description, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtXmlIndexCreate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXmlIndexCreate, SQL_CLOSE );
		}

		/* set values of parameters */
		/* - get unique id for index */
		if (!XMLIMAPI_IdGetNext(handle,handle->stmtXmlIndexID)) {
			return XMLIMAPI_False;
		}
		XMLIMAPI_IdAsString(handle->stmtXmlIndexID,xmlIndex->IndexID);

		/* - set name space id to default ns 
			(has to be changed when entering a namespace is available) */
/*		SAPDB_memcpy(handle->stmtXmlIndexCreateNSID,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",XMLIMAPI_MAX_ID_LEN);*/

		strcpy(handle->stmtXmlIndexParam.indexName,xmlIndex->indexName);
		strcpy(handle->stmtXmlIndexParam.xpathBasePath,xmlIndex->xpathBasePath);
		strcpy(handle->stmtXmlIndexParam.xpathValuePath,xmlIndex->xpathValuePath);
		strcpy(handle->stmtXmlIndexParam.description,xmlIndex->description);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtXmlIndexCreate)) != SQL_SUCCESS) {
			setOdbcErrorMessage(handle->sqlHenv,handle->sqlHdbc,handle->stmtXmlIndexCreate, state, text, &nativeODBCCode);
			if (nativeODBCCode==250) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_XMLINDEX_ALLREADY_EXISTS,
							 XMLIMAPI_ERR_MAPI_TEXT_XMLINDEX_ALLREADY_EXISTS);
			} else {
				addSQLErrorItem(handle, handle->stmtXmlIndexCreate, rc);
				SQLFreeStmt( handle->stmtXmlIndexCreate, SQL_DROP );
			}
			handle->stmtXmlIndexCreate = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	}


	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexUpdate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexUpdate(XMLIMAPI_Handle	handle,
											XMLIMAPI_XmlIndex	*xmlIndex)
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

		if (!handle->stmtXmlIndexUpdate) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_UPDATE_XMLINDEX, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXmlIndexParam.xpathBasePath, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,2,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXmlIndexParam.xpathValuePath, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,3,SQL_PARAM_INPUT,SQL_C_CHAR,SQL_CHAR,
 							  0,0, handle->stmtXmlIndexParam.description, 
							  513,
							  NULL);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,4,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXmlIndexID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtXmlIndexUpdate = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXmlIndexUpdate, SQL_CLOSE );
		}

		/* set values of parameters */
		XMLIMAPI_IdStringAsId(xmlIndex->IndexID,handle->stmtXmlIndexID);
		strcpy(handle->stmtXmlIndexParam.xpathBasePath,xmlIndex->xpathBasePath);
		strcpy(handle->stmtXmlIndexParam.xpathValuePath,xmlIndex->xpathValuePath);
		strcpy(handle->stmtXmlIndexParam.description,xmlIndex->description);

		/* execute statement */
		if ((rc = SQLExecute(handle->stmtXmlIndexUpdate)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtXmlIndexUpdate, rc);
			SQLFreeStmt( handle->stmtXmlIndexUpdate, SQL_DROP );
			handle->stmtXmlIndexUpdate = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		}

		Commit(handle);
		return XMLIMAPI_True;
	
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexDelete
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexDelete(XMLIMAPI_Handle	handle,
										  XMLIMAPI_Id		id,
										  XMLIMAPI_Bool		alone)
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
		if (alone)
			Rollback(handle);
	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtXmlIndexDelete) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_DELETE_XMLINDEX, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXmlIndexID, 
							  XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtXmlIndexDelete = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtXmlIndexDelete, SQL_CLOSE );
		}

		/* set values of parameters */
		SAPDB_memcpy(handle->stmtXmlIndexID,id,XMLIMAPI_MAX_ID_LEN);
		
		/* execute statement */
		if ((rc = SQLExecute(handle->stmtXmlIndexDelete)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtXmlIndexDelete, rc);
			SQLFreeStmt( handle->stmtXmlIndexDelete, SQL_DROP );
			handle->stmtXmlIndexDelete = 0;
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
		Function: XMLIMAPI_XmlIndexCheckAssign	
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexCheckAssign(XMLIMAPI_Handle	handle,
												 XMLIMAPI_Id		id)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_Char				msg[XMLIMAPI_MAX_ERROR_TEXT_LEN];
		XMLIMAPI_Char				text[XMLIMAPI_MAX_ERROR_TEXT_LEN];
		XMLIMAPI_Char				docclass1[512];
		XMLIMAPI_Char				docclass2[512];
		XMLIMAPI_Char				docclass3[512];
		int							count = 0;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		/* no prepared statement yet */
		if (handle->stmtXmlIndexCheckAssign != 0) {
			SQLFreeStmt( handle->stmtXmlIndexCheckAssign, SQL_CLOSE );
		}

		rc = SQLAllocStmt(SqlDBC, &SqlStmt);
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_XMLINDEX_CHECK_ASSIGN, SQL_NTS );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}
			
		rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
						&handle->stmtDocClassCol.docClassName, 
						129, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							0,0, handle->stmtXmlIndexID, 
							XMLIMAPI_MAX_ID_LEN,
							&handle->stmtIdIndicator);
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		handle->stmtXmlIndexCheckAssign = SqlStmt;


		/* set values of parameters */
		SAPDB_memcpy(handle->stmtXmlIndexID,id,XMLIMAPI_MAX_ID_LEN);
		strcpy(msg,"");
		
		/* execute statement */
		if ((rc = SQLExecute(handle->stmtXmlIndexCheckAssign)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtXmlIndexCheckAssign, rc);
			SQLFreeStmt( handle->stmtXmlIndexCheckAssign, SQL_DROP );
			handle->stmtXmlIndexCheckAssign = 0;
			/* clean up */
			Rollback(handle);
			return XMLIMAPI_False;
		} else {
			while (TRUE) {
			
				rc = SQLFetch(handle->stmtXmlIndexCheckAssign);
				
				if ((rc == SQL_SUCCESS) ||
					(rc == SQL_SUCCESS_WITH_INFO)) {
					count++;
					if (count == 1)
						strcpy(docclass1,handle->stmtDocClassCol.docClassName);
					else if (count == 2) 
						strcpy(docclass2,handle->stmtDocClassCol.docClassName);
					else if (count == 3)
						strcpy(docclass3,handle->stmtDocClassCol.docClassName);
					else if (count == 4)
						break;
				} else if (rc == 100) {
					break;
				} else {
					return XMLIMAPI_False;
				}
			}
			if (count == 0) {
				/* no assignment exists -> delete is ok */
				return XMLIMAPI_True;
			} else {
				/* there is at least one assignment -> delete not ok */
				switch (count) {
					case 1:
						{
							sprintf(msg,CHECKASSIGN_MSG1,docclass1);
							break;
						}
					case 2:
						{
							sprintf(msg,CHECKASSIGN_MSG2,docclass1,docclass2);
							break;
						}
					case 3:
						{
							sprintf(msg,CHECKASSIGN_MSG3,docclass1,docclass2,docclass3);
							break;
						}
					case 4:
						{
							sprintf(msg,CHECKASSIGN_MSG4,docclass1,docclass2,docclass3);
							break;
						}
				}
				sprintf(text,XMLIMAPI_ERR_MAPI_TEXT_XMLINDEX_ASSIGNED_TO_DOCCLASS,msg);
				addErrorItem(handle,
								XMLIMAPI_ERR_TYPE_MAPI,
								XMLIMAPI_ERR_MAPI_CODE_XMLINDEX_ASSIGNED_TO_DOCCLASS,
								text);
				return XMLIMAPI_False;
			}
		}
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGet
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGet(XMLIMAPI_Handle	handle,
										 XMLIMAPI_Id		xmlIndexId,
										 XMLIMAPI_XmlIndex  *xmlIndex)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtGetXmlIndex) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_XMLINDEX, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.IndexID), 
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.indexName), 
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.description),
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR,
							&(handle->stmtXmlIndexCol.xpathBasePath),
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR,
							&(handle->stmtXmlIndexCol.xpathValuePath),
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtXmlIndexID, XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtGetXmlIndex = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtGetXmlIndex, SQL_CLOSE );
		}

		/* set document class id */
		SAPDB_memcpy(handle->stmtXmlIndexID,xmlIndexId,XMLIMAPI_MAX_ID_LEN);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetXmlIndex)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetXmlIndex, rc);
			SQLFreeStmt( handle->stmtGetXmlIndex, SQL_DROP );
			handle->stmtGetXmlIndex = 0;
			/* clean up */
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetXmlIndex);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strMaxCopy(xmlIndex->IndexID,handle->stmtXmlIndexCol.IndexID,55);
				strMaxCopy(xmlIndex->indexName,handle->stmtXmlIndexCol.indexName,513);
				strMaxCopy(xmlIndex->description,handle->stmtXmlIndexCol.description,513);
				strMaxCopy(xmlIndex->xpathBasePath,handle->stmtXmlIndexCol.xpathBasePath,513);
				strMaxCopy(xmlIndex->xpathValuePath,handle->stmtXmlIndexCol.xpathValuePath,513);

				/* Clean Up */
				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX);
				} else {
					addSQLErrorItem(handle, handle->stmtGetXmlIndex, rc);
					SQLFreeStmt( SqlStmt, SQL_DROP );
					handle->stmtGetXmlIndex = 0;
				}
				/* clean up */
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetFirst	
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirst(XMLIMAPI_Handle	handle,
											  XMLIMAPI_XmlIndex  *xmlIndex)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtGetXmlIndices) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_XMLINDICES, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.IndexID), 
							54, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.indexName), 
							128, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.description),
							512, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR,
							&(handle->stmtXmlIndexCol.xpathBasePath),
							512, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR,
							&(handle->stmtXmlIndexCol.xpathValuePath),
							512, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtGetXmlIndices = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtGetXmlIndices, SQL_CLOSE );
		}


		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetXmlIndices)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetXmlIndices, rc);
			SQLFreeStmt( handle->stmtGetXmlIndices, SQL_DROP );
			handle->stmtGetXmlIndices = 0;
			/* clean up */
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetXmlIndices);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(xmlIndex->IndexID,
					   handle->stmtXmlIndexCol.IndexID);
				strcpy(xmlIndex->indexName,
					   handle->stmtXmlIndexCol.indexName);
				strcpy(xmlIndex->description,
					   handle->stmtXmlIndexCol.description);
				strcpy(xmlIndex->xpathBasePath,
					   handle->stmtXmlIndexCol.xpathBasePath);
				strcpy(xmlIndex->xpathValuePath,
					   handle->stmtXmlIndexCol.xpathValuePath);

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX);
				} else {
					addSQLErrorItem(handle, handle->stmtGetXmlIndices, rc);
					SQLFreeStmt( handle->stmtGetXmlIndices, SQL_DROP );
					handle->stmtGetXmlIndices = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNext(XMLIMAPI_Handle	handle,
											 XMLIMAPI_XmlIndex	*xmlIndex)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtGetXmlIndices;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(xmlIndex->IndexID,
				   handle->stmtXmlIndexCol.IndexID);
			strcpy(xmlIndex->indexName,
				   handle->stmtXmlIndexCol.indexName);
			strcpy(xmlIndex->description,
				   handle->stmtXmlIndexCol.description);
			strcpy(xmlIndex->xpathBasePath,
				   handle->stmtXmlIndexCol.xpathBasePath);
			strcpy(xmlIndex->xpathValuePath,
				   handle->stmtXmlIndexCol.xpathValuePath);

			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_INDICES);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				handle->stmtGetXmlIndices = 0;
			}
			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetFirstIncl	
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstIncl(XMLIMAPI_Handle	handle,
												XMLIMAPI_XmlIndex  *xmlIndex,
												XMLIMAPI_XmlIndexList	include)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_XmlIndex			*__xmlIndex;
		XMLIMAPI_Bool				__first;
		XMLIMAPI_Char				__temp[MAX_SQLSTR_LEN_WD00],__temp2[MAX_SQLSTR_LEN_WD00],__where[MAX_SQLSTR_LEN_WD00];

	
		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		/* no prepared statement yet */

		if (handle->stmtGetXmlIndicesIncl != 0) {
			SQLFreeStmt( handle->stmtGetXmlIndicesIncl, SQL_CLOSE );
		}

		rc = SQLAllocStmt(SqlDBC, &SqlStmt);
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}
		/* build where clause */
		if (XMLIMAPI_XmlIndexListGetFirst(include,&__xmlIndex)) {
			strcpy(__temp,"WHERE IDXID = X'%s' %s");
			__first = XMLIMAPI_True;
			do {
				if (__first) {
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,__xmlIndex->IndexID,"%s %s %s");
					__first = XMLIMAPI_False;
				} else {
					sp77sprintf(__temp2,MAX_SQLSTR_LEN_WD00,__temp,"OR","IDXID = X'%s'","%s");
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp2,__xmlIndex->IndexID,"%s %s %s");
				}
				strcpy(__temp,__where);
			} while(XMLIMAPI_XmlIndexListGetNext(include,&__xmlIndex));
			sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,"","","");
		} else {
			strcpy(__where,"WHERE IDXID = X'000000000000000000000000000000000000000000000000'");
		}
		sp77sprintf(stmtStr,MAX_SQLSTR_LEN_WD00,SQL_GET_XMLINDICES_INCL,__where);

		rc = SQLPrepare(SqlStmt, (SQLCHAR*)stmtStr, SQL_NTS );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}
			
		rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
						&(handle->stmtXmlIndexCol.IndexID), 
						55, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
						&(handle->stmtXmlIndexCol.indexName), 
						129, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
						&(handle->stmtXmlIndexCol.description),
						513, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR,
						&(handle->stmtXmlIndexCol.xpathBasePath),
						513, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR,
						&(handle->stmtXmlIndexCol.xpathValuePath),
						513, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		handle->stmtGetXmlIndicesIncl = SqlStmt;


		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtGetXmlIndicesIncl)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtGetXmlIndicesIncl, rc);
			SQLFreeStmt( handle->stmtGetXmlIndicesIncl, SQL_DROP );
			handle->stmtGetXmlIndicesIncl = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtGetXmlIndicesIncl);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(xmlIndex->IndexID,
					   handle->stmtXmlIndexCol.IndexID);
				strcpy(xmlIndex->indexName,
					   handle->stmtXmlIndexCol.indexName);
				strcpy(xmlIndex->description,
					   handle->stmtXmlIndexCol.description);
				strcpy(xmlIndex->xpathBasePath,
					   handle->stmtXmlIndexCol.xpathBasePath);
				strcpy(xmlIndex->xpathValuePath,
					   handle->stmtXmlIndexCol.xpathValuePath);

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX);
				} else {
					addSQLErrorItem(handle, handle->stmtGetXmlIndicesIncl, rc);
					SQLFreeStmt( handle->stmtGetXmlIndicesIncl, SQL_DROP );
					handle->stmtGetXmlIndicesIncl = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetNextIncl
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextIncl(XMLIMAPI_Handle	handle,
												XMLIMAPI_XmlIndex	*xmlIndex)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtGetXmlIndicesIncl;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(xmlIndex->IndexID,
				   handle->stmtXmlIndexCol.IndexID);
			strcpy(xmlIndex->indexName,
				   handle->stmtXmlIndexCol.indexName);
			strcpy(xmlIndex->description,
				   handle->stmtXmlIndexCol.description);
			strcpy(xmlIndex->xpathBasePath,
				   handle->stmtXmlIndexCol.xpathBasePath);
			strcpy(xmlIndex->xpathValuePath,
				   handle->stmtXmlIndexCol.xpathValuePath);

			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_INDICES);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				handle->stmtGetXmlIndicesIncl = 0;
			}
			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetFirstByDocClass
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstByDocClass(XMLIMAPI_Handle		handle,
														XMLIMAPI_Id			docClassId,
														XMLIMAPI_XmlIndex  *xmlIndex)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		if (!handle->stmtIndicesForDC) {
			/* no prepared statement yet */

			rc = SQLAllocStmt(SqlDBC, &SqlStmt);
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLPrepare(SqlStmt, (SQLCHAR*)SQL_GET_XMLINDEX_FOR_DOCCLASS, SQL_NTS );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}
				
			rc = SQLBindCol(SqlStmt, 1, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.IndexID),
							55, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 2, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.indexName),
							129, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 3, SQL_C_CHAR, 
							&(handle->stmtXmlIndexCol.description),
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR,
							&(handle->stmtXmlIndexCol.xpathBasePath),
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindCol(SqlStmt, 5, SQL_C_CHAR,
							&(handle->stmtXmlIndexCol.xpathValuePath),
							513, NULL );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			rc = SQLBindParameter(SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							  0,0, handle->stmtDocClassID, XMLIMAPI_MAX_ID_LEN,
							  &handle->stmtIdIndicator );
			if (rc != SQL_SUCCESS) {
				addSQLErrorItem(handle, SqlStmt, rc);
				SQLFreeStmt( SqlStmt, SQL_DROP );
				return XMLIMAPI_False;
			}

			handle->stmtIndicesForDC = SqlStmt;

		} else {
			SQLFreeStmt( handle->stmtIndicesForDC, SQL_CLOSE );
		}

		/* set document class id */
		SAPDB_memcpy(handle->stmtDocClassID,docClassId,XMLIMAPI_MAX_ID_LEN);

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtIndicesForDC)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtIndicesForDC, rc);
			SQLFreeStmt( handle->stmtIndicesForDC, SQL_DROP );
			handle->stmtIndicesForDC = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtIndicesForDC);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(xmlIndex->IndexID,handle->stmtXmlIndexCol.IndexID);
				strcpy(xmlIndex->indexName,handle->stmtXmlIndexCol.indexName);
				strcpy(xmlIndex->description,handle->stmtXmlIndexCol.description);
				strcpy(xmlIndex->xpathBasePath,handle->stmtXmlIndexCol.xpathBasePath);
				strcpy(xmlIndex->xpathValuePath,handle->stmtXmlIndexCol.xpathValuePath);

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX_ASSIGNED,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX_ASSIGNED);
				} else {
					addSQLErrorItem(handle, handle->stmtIndicesForDC, rc);
					handle->stmtIndicesForDC = 0;
					SQLFreeStmt( handle->stmtIndicesForDC, SQL_DROP );
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetNextByDocClass
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextByDocClass(XMLIMAPI_Handle	handle,
													   XMLIMAPI_XmlIndex *xmlIndex)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtIndicesForDC;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(xmlIndex->IndexID,handle->stmtXmlIndexCol.IndexID);
			strcpy(xmlIndex->indexName,handle->stmtXmlIndexCol.indexName);
			strcpy(xmlIndex->description,handle->stmtXmlIndexCol.description);
			strcpy(xmlIndex->xpathBasePath,handle->stmtXmlIndexCol.xpathBasePath);
			strcpy(xmlIndex->xpathValuePath,handle->stmtXmlIndexCol.xpathValuePath);
			
			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_INDICES);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				handle->stmtIndicesForDC = 0;
				SQLFreeStmt( SqlStmt, SQL_DROP );
			}
			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetFirstForDocClasses
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstForDocClasses(XMLIMAPI_Handle		handle,
														 XMLIMAPI_DocClassList	docClasses,
														 XMLIMAPI_DocClass		*docClass,
														 XMLIMAPI_XmlIndex		*xmlIndex)
	{
		SQLHSTMT					SqlStmt;
		SQLHENV						SqlEnv;
		SQLHDBC						SqlDBC;
		SQLRETURN					rc;
		XMLIMAPI_Char				stmtStr[MAX_SQLSTR_LEN_WD00];
		long						foobar;
		XMLIMAPI_Char				__where[MAX_SQLSTR_LEN_WD00],__temp[MAX_SQLSTR_LEN_WD00],__temp2[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_DocClass			*__docClass;
		XMLIMAPI_Bool				__first;

		SqlEnv = handle->sqlHenv;
		SqlDBC = handle->sqlHdbc;

		/* no prepared statement yet */

		rc = SQLAllocStmt(SqlDBC, &SqlStmt);
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		/* build where clause */
		if (XMLIMAPI_DocClassListGetFirst(docClasses,&__docClass)) {
			handle->flgNoDocClasses = XMLIMAPI_False;
			strcpy(__temp,"%s %s");
			__first = XMLIMAPI_True;
			do {
				if (__first) {
					sp77sprintf(__temp2,MAX_SQLSTR_LEN_WD00,__temp,"D.DCID = X'%s'","%s");
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp2,__docClass->docClassID,"%s %s");
					__first = XMLIMAPI_False;
				} else {
					sp77sprintf(__temp2,MAX_SQLSTR_LEN_WD00,__temp,"AND D.DCID = X'%s'","%s");
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp2,__docClass->docClassID,"%s %s");
				}
				strcpy(__temp,__where);
			} while(XMLIMAPI_DocClassListGetNext(docClasses,&__docClass));
			sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,"AND","","");
		} else {
			handle->flgNoDocClasses = XMLIMAPI_True;
			strcpy(__where,"");
		}
		sp77sprintf(stmtStr,MAX_SQLSTR_LEN_WD00,SQL_GET_XMLINDEX_FOR_DOCCLASSES,__where);

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
						&(handle->stmtXmlIndexCol.IndexID),
						55, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		rc = SQLBindCol(SqlStmt, 4, SQL_C_CHAR, 
						&(handle->stmtXmlIndexCol.indexName),
						129, NULL );
		if (rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, SqlStmt, rc);
			SQLFreeStmt( SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		handle->stmtIndicesForDCs = SqlStmt;

		/* select execute statement */
		if ((rc = SQLExecute(handle->stmtIndicesForDCs)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtIndicesForDCs, rc);
			SQLFreeStmt( handle->stmtIndicesForDCs, SQL_DROP );
			handle->stmtIndicesForDCs = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			rc = SQLFetch(handle->stmtIndicesForDCs);
			
			if ((rc == SQL_SUCCESS) ||
				(rc == SQL_SUCCESS_WITH_INFO)) {

				if (handle->flgNoDocClasses == XMLIMAPI_False) {
					strcpy(docClass->docClassID,handle->stmtDocClassCol.docClassID);
					strcpy(docClass->docClassName,handle->stmtDocClassCol.docClassName);
					strcpy(xmlIndex->IndexID,handle->stmtXmlIndexCol.IndexID);
					strcpy(xmlIndex->indexName,handle->stmtXmlIndexCol.indexName);
				} else {
					strcpy(docClass->docClassID,"*");
					strcpy(docClass->docClassName,"*");
					strcpy(xmlIndex->IndexID,handle->stmtXmlIndexCol.IndexID);
					strcpy(xmlIndex->indexName,handle->stmtXmlIndexCol.indexName);
				}

				return XMLIMAPI_True;

			} else {
				if (rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX);
				} else {
					addSQLErrorItem(handle, handle->stmtIndicesForDCs, rc);
					handle->stmtIndicesForDCs = 0;
					SQLFreeStmt( handle->stmtIndicesForDCs, SQL_DROP );
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetNextForDocClasses
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextForDocClasses(XMLIMAPI_Handle	handle,
														XMLIMAPI_DocClass *docClass,
													    XMLIMAPI_XmlIndex *xmlIndex)
	{

		SQLHSTMT					SqlStmt;
		SQLRETURN					rc;

		SqlStmt = handle->stmtIndicesForDCs;

		rc = SQLFetch(SqlStmt);
		
		if ((rc == SQL_SUCCESS) ||
			(rc == SQL_SUCCESS_WITH_INFO)) {

			if (handle->flgNoDocClasses == XMLIMAPI_False) {
				strcpy(docClass->docClassID,handle->stmtDocClassCol.docClassID);
				strcpy(docClass->docClassName,handle->stmtDocClassCol.docClassName);
				strcpy(xmlIndex->IndexID,handle->stmtXmlIndexCol.IndexID);
				strcpy(xmlIndex->indexName,handle->stmtXmlIndexCol.indexName);
			} else {
				strcpy(docClass->docClassID,"*");
				strcpy(docClass->docClassName,"*");
				strcpy(xmlIndex->IndexID,handle->stmtXmlIndexCol.IndexID);
				strcpy(xmlIndex->indexName,handle->stmtXmlIndexCol.indexName);
			}
			
			return XMLIMAPI_True;

		} else {
			if (rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_INDICES);
			} else {
				addSQLErrorItem(handle, SqlStmt, rc);
				handle->stmtIndicesForDCs = 0;
				SQLFreeStmt( SqlStmt, SQL_DROP );
			}
			return XMLIMAPI_False;
		}

	}


	/*------------------------------------------------------------------------
		HELPER FUNCTIONS
	--------------------------------------------------------------------------*/

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexListCreate
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListCreate(XMLIMAPI_XmlIndexList *list)
	{
		XMLIMAPI_Bool			ok = XMLIMAPI_False;
		XMLIMAPI_XmlIndexList	newList = NULL;

		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_xmlindex_list), (XMLIMAPI_UInt1**) &newList, &ok );
		if (ok == XMLIMAPI_True) {

			newList->first = NULL;
			newList->last = NULL;
			newList->curr = NULL;
			newList->count = 0;
			*list = newList;

			return XMLIMAPI_True;
		} else {
			*list = NULL;

			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexListDestroy
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListDestroy(XMLIMAPI_XmlIndexList list)
	{
		XMLIMAPI_XmlIndexListItem	current,next;
		
		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			sqlfree((XMLIMAPI_UInt1*)current->item);
			sqlfree((XMLIMAPI_UInt1*)current);
		}

		sqlfree((XMLIMAPI_UInt1*)list);
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexListInit
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListInit(XMLIMAPI_XmlIndexList list)
	{
		XMLIMAPI_XmlIndexListItem	current,next;
		
		if ( !list ) {
			return XMLIMAPI_False;
		}
    
		/* track down the list to destroy every single item */
		next = list->first;
		while (next) {
			current = next;
			next = current->next;
			sqlfree((XMLIMAPI_UInt1*)current->item);
			sqlfree((XMLIMAPI_UInt1*)current);
		}

		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;
		list->count = 0;

		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexListAddItem
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListAddItem(XMLIMAPI_XmlIndexList	list, 
												 XMLIMAPI_XmlIndex		*xmlIndex)
	{
		XMLIMAPI_XmlIndexListItem		newListItem;
		XMLIMAPI_XmlIndexP				newItem;
		XMLIMAPI_Bool					ok = XMLIMAPI_False;
		
		if (!list) {
			return XMLIMAPI_False;
		}
    
		sqlallocat( sizeof(struct st_api_xmlindex_list_item), (XMLIMAPI_UInt1**) &newListItem, &ok );
		if (ok == XMLIMAPI_True) {

			sqlallocat( sizeof(struct st_api_xml_index), (XMLIMAPI_UInt1**) &newItem, &ok );
			if (ok == XMLIMAPI_True) {

				strMaxCopy(newItem->IndexID,xmlIndex->IndexID,55);
				strMaxCopy(newItem->indexName,xmlIndex->indexName,513);
				strMaxCopy(newItem->description,xmlIndex->description,513);
				strMaxCopy(newItem->xpathBasePath,xmlIndex->xpathBasePath,513);
				strMaxCopy(newItem->xpathValuePath,xmlIndex->xpathValuePath,513);

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
			list->count++;

			return XMLIMAPI_True;
		} else {

			return XMLIMAPI_False;
		}

	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexListGetFirst
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListGetFirst(XMLIMAPI_XmlIndexList	list, 
												  XMLIMAPI_XmlIndex		**xmlIndex)
	{
		if(!xmlIndex)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		*xmlIndex = list->first->item;
		list->curr = list->first;
		return XMLIMAPI_True;
	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexListGetNext
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListGetNext(XMLIMAPI_XmlIndexList		list, 
												 XMLIMAPI_XmlIndex			**xmlIndex)
	{
		if(!xmlIndex)
			return XMLIMAPI_False;
		if(!list->first)
			return XMLIMAPI_False;
		if(!list->curr)
			return XMLIMAPI_False;
		list->curr = list->curr->next;
		if(!list->curr)
			return XMLIMAPI_False;
		*xmlIndex = list->curr->item;
		return XMLIMAPI_True;

	}

	/*------------------------------------------------------------------------
		Function:	XMLIMAPI_XmlIndexListGetLength
	--------------------------------------------------------------------------*/
	XMLIMAPI_UInt4 XMLIMAPI_XmlIndexListGetLength(XMLIMAPI_XmlIndexList		list)
	{
		if(!list)
			return -1;
		return list->count;

	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetFirstByDocClassInfoExcl	
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstByDocClassInfoExcl(XMLIMAPI_Handle handle,
						 									  XMLIMAPI_Id docClassId,
															  XMLIMAPI_XmlIndex *xmlIndex,
															  XMLIMAPI_XmlIndexList exclude)
	{
		SQLHSTMT					__SqlStmt;
		SQLHENV						__SqlEnv;
		SQLHDBC						__SqlDBC;
		SQLRETURN					__rc;
		XMLIMAPI_Char				__stmtStr[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_Char				__exclStr[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_Char				__temp[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_Char				__temp2[MAX_SQLSTR_LEN_WD00];
		XMLIMAPI_Char				__where[MAX_SQLSTR_LEN_WD00];
		long						__foobar;
		XMLIMAPI_UInt4				__i;
		XMLIMAPI_XmlIndex			*__xmlIndex;
		XMLIMAPI_Bool				__first;

		__SqlEnv = handle->sqlHenv;
		__SqlDBC = handle->sqlHdbc;

		/* no prepare because statement is build at runtime */
		/* build where clause */
		if (XMLIMAPI_XmlIndexListGetFirst(exclude,&__xmlIndex)) {
			strcpy(__temp,"X.\"IDXID\" NOT IN (X'%s' %s)");
			__first = XMLIMAPI_True;
			do {
				if (__first) {
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,__xmlIndex->IndexID,"%s %s %s");
					__first = XMLIMAPI_False;
				} else {
					sp77sprintf(__temp2,MAX_SQLSTR_LEN_WD00,__temp,",","X'%s'","%s");
					sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp2,__xmlIndex->IndexID,"%s %s %s");
				}
				strcpy(__temp,__where);
			} while(XMLIMAPI_XmlIndexListGetNext(exclude,&__xmlIndex));
			sp77sprintf(__where,MAX_SQLSTR_LEN_WD00,__temp,"","","");
		} else {
			strcpy(__where,"X.\"IDXID\" NOT IN (X'000000000000000000000000000000000000000000000000')");
		}
		sp77sprintf(__stmtStr,MAX_SQLSTR_LEN_WD00,SQL_GET_XMLINDEX_FOR_DOCCLASS_INFO_EXCL,__where);

		/* 2. alloc statement */
		__rc = SQLAllocStmt(__SqlDBC, &__SqlStmt);
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		/* 3. prepare statement */
		__rc = SQLPrepare(__SqlStmt, (SQLCHAR*) __stmtStr, SQL_NTS );
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		/* 4. bind Columns */
		__rc = SQLBindCol(__SqlStmt, 1, SQL_C_CHAR, 
						&(handle->stmtXmlIndexCol.IndexID), 
						55, NULL );
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		__rc = SQLBindCol(__SqlStmt, 2, SQL_C_CHAR, 
						&(handle->stmtXmlIndexCol.indexName), 
						129, NULL );
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		__rc = SQLBindCol(__SqlStmt, 3, SQL_C_CHAR, 
						&(handle->stmtXmlIndexCol.description),
						513, NULL );
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		__rc = SQLBindCol(__SqlStmt, 4, SQL_C_CHAR,
						&(handle->stmtXmlIndexCol.xpathBasePath),
						513, NULL );
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

		__rc = SQLBindCol(__SqlStmt, 5, SQL_C_CHAR,
						&(handle->stmtXmlIndexCol.xpathValuePath),
						513, NULL );
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}

/*		__rc = SQLBindParameter(__SqlStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_BINARY,
 							0,0, handle->stmtDocClassID, 
							XMLIMAPI_MAX_ID_LEN,
							&handle->stmtIdIndicator);
		if (__rc != SQL_SUCCESS) {
			addSQLErrorItem(handle, __SqlStmt, __rc);
			SQLFreeStmt( __SqlStmt, SQL_DROP );
			return XMLIMAPI_False;
		}
*/
		handle->stmtIndicesForDCInfoExcl = __SqlStmt;

		/* set document class id */
/*		SAPDB_memcpy(handle->stmtDocClassID,docClassId,XMLIMAPI_MAX_ID_LEN);
*/
		/* select execute statement */
		if ((__rc = SQLExecute(handle->stmtIndicesForDCInfoExcl)) != SQL_SUCCESS) {
			addSQLErrorItem(handle, handle->stmtIndicesForDCInfoExcl, __rc);
			SQLFreeStmt( handle->stmtIndicesForDCInfoExcl, SQL_DROP );
			handle->stmtIndicesForDCInfoExcl = 0;
			return XMLIMAPI_False;
		} else {
			/* Fetch a block of data and print each row. 
			   On an error, display a message and exit.  */
			__rc = SQLFetch(handle->stmtIndicesForDCInfoExcl);
			
			if ((__rc == SQL_SUCCESS) ||
				(__rc == SQL_SUCCESS_WITH_INFO)) {

				strcpy(xmlIndex->IndexID,
					   handle->stmtXmlIndexCol.IndexID);
				strcpy(xmlIndex->indexName,
					   handle->stmtXmlIndexCol.indexName);
				strcpy(xmlIndex->description,
					   handle->stmtXmlIndexCol.description);
				strcpy(xmlIndex->xpathBasePath,
					   handle->stmtXmlIndexCol.xpathBasePath);
				strcpy(xmlIndex->xpathValuePath,
					   handle->stmtXmlIndexCol.xpathValuePath);
				return XMLIMAPI_True;

			} else {
				if (__rc == 100) {
					addErrorItem(handle,
								 XMLIMAPI_ERR_TYPE_MAPI,
								 XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX_ASSIGNED,
								 XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX_ASSIGNED);
					SQLFreeStmt( handle->stmtIndicesForDCInfoExcl, SQL_DROP );
				} else {
					addSQLErrorItem(handle, __SqlStmt, __rc);
					SQLFreeStmt( handle->stmtIndicesForDCInfoExcl, SQL_DROP );
					handle->stmtIndicesForDCInfoExcl = 0;
				}
				return XMLIMAPI_False;
			}
		}
 
	}

	/*------------------------------------------------------------------------
		Function: XMLIMAPI_XmlIndexGetNextByDocClassInfoExcl
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextByDocClassInfoExcl(XMLIMAPI_Handle	handle,
															   XMLIMAPI_XmlIndex	*xmlIndex)
	{

		SQLHSTMT					__SqlStmt;
		SQLRETURN					__rc;

		__SqlStmt = handle->stmtIndicesForDCInfoExcl;

		__rc = SQLFetch(__SqlStmt);
		
		if ((__rc == SQL_SUCCESS) ||
			(__rc == SQL_SUCCESS_WITH_INFO)) {

			strcpy(xmlIndex->IndexID,
				   handle->stmtXmlIndexCol.IndexID);
			strcpy(xmlIndex->indexName,
				   handle->stmtXmlIndexCol.indexName);
			strcpy(xmlIndex->description,
				   handle->stmtXmlIndexCol.description);
			strcpy(xmlIndex->xpathBasePath,
				   handle->stmtXmlIndexCol.xpathBasePath);
			strcpy(xmlIndex->xpathValuePath,
				   handle->stmtXmlIndexCol.xpathValuePath);

			return XMLIMAPI_True;

		} else {
			if (__rc == 100) {
				addErrorItem(handle,
							 XMLIMAPI_ERR_TYPE_MAPI,
							 XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES,
							 XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_INDICES);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
			} else {
				addSQLErrorItem(handle, __SqlStmt, __rc);
				SQLFreeStmt( __SqlStmt, SQL_DROP );
				handle->stmtIndicesForDCInfoExcl = 0;
			}
			return XMLIMAPI_False;
		}

	}


