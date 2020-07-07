/*!**********************************************************************

  module: WDVXIS_Service.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV XML Indexing Service

  description:  Service

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



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





************************************************************************/

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

/*---------------------------------------------------------------------------*/

#include "SAPDB/WebDAV/XIS/WDVXIS_Service.h"
#include "SAPDB/WebDAV/XIS/WDVXIS_ErrorCodes.h"

#include "SAPDB/XML/Memory/XMLMem_Memory.h"
#include "SAPDBCommon/SAPDB_string.h"

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

#define ID_ROOT "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

#define WDVXIS_MAX_PROPERTY_SHORT_VALUE_LEN             900

#define WDVXIS_MAX_NAME_SPACE_LEN                       1000
#define WDVXIS_MAX_PROPERTY_NAME_PREFIX_LEN             800
#define WDVXIS_MAX_RESOURCE_NAME_LEN                    1000
#define WDVXIS_MAX_SQL_STMT_LEN                         8192
#define WDVXIS_MAX_URI_STRING_LEN                       1000

/*---------------------------------------------------------------------------*/

#define SQL_SET_INDEX                                                       \
    "INSERT WebDAV_Property (CId, Property_Id, Property_Short_Value)               \
     SELECT ?, PM.Id, ? FROM WebDAV_Property_Management PM, WebDAV_Name_Space NS   \
     WHERE NS.Name_Space = ? AND PM.Name_Prefix = ? AND PM.Name_Space_Id = NS.Id"

/*---------------------------------------------------------------------------*/

#define SQL_INSERT_PROPERTY \
     "INSERT INTO WebDAV_Property_Management SET Id = ?, Name_Space_Id = ?, Name_Prefix = ?"

/*---------------------------------------------------------------------------*/

#define SQL_SELECT_NAME_SPACE_ID    \
    "SELECT Id FROM WebDAV_Name_Space WHERE Name_Space = ?"

#define SQL_INSERT_NAME_SPACE	 \
    "INSERT INTO WebDAV_Name_Space SET Id = ?, Name_Space = ?"

/*---------------------------------------------------------------------------*/

#define SQL_RETRIEVE_DOCS_PREFIX    \
    "SELECT I.CId, I.Name \
     FROM \
     WebDAV_Inode I, WebDAV_Property DC, WebDAV_Property IDX, WebDAV_Name_Space NS, WebDAV_Property_Management PM \
     WHERE \
         I.CId = DC.CId \
     AND DC.CId = IDX.CId \
     AND NS.Id = PM.Name_Space_Id \
     AND PM.Id = IDX.Property_Id \
     AND "

#define SQL_RETRIEVE_DOCS_DOC_CLASS_DESC  \
       "DC.Property_Id = x'00000000000000000000000000000000000000000000000D' \
        AND DC.Property_Short_Value = "

#define SQL_RETRIEVE_DOCS_INDEX_DESC  \
       "PM.Name_Prefix = "

#define SQL_RETRIEVE_DOCS_INDEX_VALUE_DESC \
       " AND IDX.Property_Short_Value = "

/*---------------------------------------------------------------------------*/

#define SQL_SELECT_PARENT_BY_CID    \
    "SELECT T2.CID, T2.Name FROM WEBDAV_Inode T1, WEBDAV_Inode T2 WHERE T1.PId = T2.CId And T1.CId = ?"

 /****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

typedef char    *WDVXIS_NameSpace;
typedef char     WDVXIS_NameSpaceString[WDVXIS_MAX_NAME_SPACE_LEN+1];

typedef char    *WDVXIS_PropertyName;
typedef char     WDVXIS_PropertyNameString[WDVXIS_MAX_PROPERTY_NAME_PREFIX_LEN+1];

typedef char    *WDVXIS_PropertyShortValue;
typedef char     WDVXIS_PropertyShortValueBuffer[WDVXIS_MAX_PROPERTY_SHORT_VALUE_LEN+1];

typedef char    *WDVXIS_ResourceName;
typedef char     WDVXIS_ResourceNameString[WDVXIS_MAX_RESOURCE_NAME_LEN+1];

typedef char    *WDVXIS_SQLStmt;
typedef char     WDVXIS_SQLStmtString[WDVXIS_MAX_SQL_STMT_LEN+1];

typedef char    *WDVXIS_URI;
typedef char     WDVXIS_URIString[WDVXIS_MAX_URI_STRING_LEN+1];

typedef struct st_wdvxis_index_store_data   *WDVXIS_IndexStoreData;
typedef struct st_wdvxis_search_data        *WDVXIS_SearchData;

struct st_wdvxis_index_store_data {
    SQLHSTMT                        hStmtSetIndex;
    SQLHSTMT                        hStmtCreateProperty;
    SQLHSTMT                        hStmtGetNameSpaceId;
    SQLHSTMT                        hStmtCreateNameSpace;

    XMLId_Id                        resourceId;
    SQLLEN                          resourceIdIndicator;

    WDVXIS_NameSpaceString          indexNameSpace;
    SQLLEN                          indexNameSpaceIndicator;
    WDVXIS_PropertyNameString       indexName;
    SQLLEN                          indexNameIndicator;

    XMLId_Id                        nameSpaceId;
    SQLLEN                          nameSpaceIdIndicator;
    XMLId_Id                        propertyId;
    SQLLEN                          propertyIdIndicator;

    WDVXIS_PropertyShortValueBuffer indexValue;
    SQLLEN                          indexValueIndicator;
};

struct st_wdvxis_search_data {
    SQLHSTMT                    hStmtSearchDocs;

    XMLId_Id                    docId;
    SQLLEN                      docIdIndicator;
    WDVXIS_ResourceNameString   docName;
    SQLLEN                      docNameIndicator;

    SQLHSTMT                    hStmtGetParent;

    XMLId_Id                    resourceId;
    SQLLEN                      resourceIdIndicator;
    XMLId_Id                    parentId;
    SQLLEN                      parentIdIndicator;
    WDVXIS_ResourceNameString   parentName;
    SQLLEN                      parentNameIndicator;
};

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

SAPDB_Bool CreateIndexStoreData( WDVXIS_IndexStoreData  *hIndexStoreData,
                                 XMLError_Error          hError );

SAPDB_Bool InitIndexStoreData( WDVXIS_IndexStoreData    hIndexStoreData,
                               XMLError_Error           hError );

void DestroyIndexStoreData( void *userData );

SAPDB_Bool CreateSearchData( WDVXIS_SearchData  *hSearchData,
                             XMLError_Error      hError );

SAPDB_Bool InitSearchData( WDVXIS_SearchData    hSearchData,
                           XMLError_Error       hError );

void DestroySearchData( void *userData );

SAPDB_Bool CreateProperty( XMLSP_Session    hIndexStoreSession,
                           char            *nameSpace,
                           char            *propertyName,
                           XMLError_Error   hError );

SAPDB_Bool GetNameSpaceId( XMLSP_Session   hIndexStoreSession,
                           char           *nameSpace,
                           XMLId_Id        nameSpaceId,
                           XMLError_Error  hError );

SAPDB_Bool CreateNameSpace( XMLSP_Session   hIndexStoreSession,
                            char           *nameSpace,
                            XMLId_Id        nameSpaceId,
                            XMLError_Error  hError );

void AddWhereRow( char                 *sqlStmt,
                  XMLQSLib_WhereRow     whereRow );

void AddIndexToStmt( WDVXIS_SQLStmt         sqlStmt,
                     XMLQSLib_WhereRow     *whereRow );

SAPDB_Bool GetURIById( SQLHDBC               hDBCDocStore,
                       SQLHENV               hEnvDocStore,
                       WDVXIS_SearchData     hSearchData,
                       char                 *docURI,
                       SAPDB_UInt            docURILen,
                       XMLError_Error        hError );

SAPDB_Bool GetParentById( SQLHDBC               hDBCDocStore,
                          SQLHENV               hEnvDocStore,
                          WDVXIS_SearchData     hSearchData,
                          XMLId_Id              resourceId,
				          XMLId_Id              parentId,
                          WDVXIS_ResourceName   parentName,
                          XMLError_Error        hError );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool SAPDBXML_Init( void            **userData,
                          XMLError_Error    hError )
{

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SAPDBXML_Exit( void    *userData )
{

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SAPDBXML_SetIndexValue( XMLSP_Session   hSessionIndexStore,
                                   XMLId_Id        resourceId,
                                   char           *indexNameSpace,
                                   char           *indexName,
                                   char           *indexValue,
                                   XMLError_Error  hError )
{

    SAPDB_Bool              rc = SAPDB_TRUE;
	SQLRETURN               sqlRC = 0;
    SQLHENV                 hEnv = SP_GetHEnv(hSessionIndexStore);
    SQLHDBC                 hDBC = SP_GetHDBC(hSessionIndexStore);
    WDVXIS_IndexStoreData   hIndexStoreData = (WDVXIS_IndexStoreData)SP_GetUserData(hSessionIndexStore);

    /* Inits */
    if (!hSessionIndexStore) {
        /* todo: Error Handling */
        return SAPDB_FALSE;
    }

    if (!hIndexStoreData) {
        if (!CreateIndexStoreData(&hIndexStoreData, hError)) {
            return SAPDB_FALSE;
        }
        SP_SetUserData(hSessionIndexStore, hIndexStoreData, DestroyIndexStoreData);
    }

    /* If no statement handle exists, create a new one */
    if (hIndexStoreData->hStmtSetIndex == 0) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hIndexStoreData->hStmtSetIndex);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);
		    return SAPDB_FALSE;
	    }

	    /* Prepare statement */
        sqlRC = SQLPrepare(hIndexStoreData->hStmtSetIndex, (SQLCHAR*)SQL_SET_INDEX, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtSetIndex, SQL_DROP);
            hIndexStoreData->hStmtSetIndex = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hIndexStoreData->hStmtSetIndex, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hIndexStoreData->resourceId, XMLID_MAX_ID_LEN,
                                 &hIndexStoreData->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtSetIndex, SQL_DROP);
            hIndexStoreData->hStmtSetIndex = 0;

		    return SAPDB_FALSE;
	    }

        sqlRC = SQLBindParameter(hIndexStoreData->hStmtSetIndex, 2,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hIndexStoreData->indexValue, WDVXIS_MAX_PROPERTY_SHORT_VALUE_LEN,
                                &hIndexStoreData->indexValueIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtSetIndex, SQL_DROP);
            hIndexStoreData->hStmtSetIndex = 0;

		    return SAPDB_FALSE;
	    }

        sqlRC = SQLBindParameter(hIndexStoreData->hStmtSetIndex, 3,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hIndexStoreData->indexNameSpace, WDVXIS_MAX_NAME_SPACE_LEN,
                                &hIndexStoreData->indexNameSpaceIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtSetIndex, SQL_DROP);
            hIndexStoreData->hStmtSetIndex = 0;

		    return SAPDB_FALSE;
	    }

        sqlRC = SQLBindParameter(hIndexStoreData->hStmtSetIndex, 4,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hIndexStoreData->indexName, WDVXIS_MAX_PROPERTY_NAME_PREFIX_LEN,
                                &hIndexStoreData->indexNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtSetIndex, SQL_DROP);
            hIndexStoreData->hStmtSetIndex = 0;

		    return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hIndexStoreData->hStmtSetIndex, SQL_CLOSE);
    }

	/* Set current input values */
    SAPDB_memcpy(hIndexStoreData->resourceId, resourceId, XMLID_MAX_ID_LEN);
    Com_StrMaxCopy(hIndexStoreData->indexNameSpace, indexNameSpace, WDVXIS_MAX_NAME_SPACE_LEN);
    Com_StrMaxCopy(hIndexStoreData->indexName, indexName, WDVXIS_MAX_PROPERTY_NAME_PREFIX_LEN);
    Com_StrMaxCopy(hIndexStoreData->indexValue, indexValue, WDVXIS_MAX_PROPERTY_SHORT_VALUE_LEN);

	/* Execute SQL */
	sqlRC = SQLExecute(hIndexStoreData->hStmtSetIndex);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);

        if (Error_IsODBCState(hError, WDVXIS_ERR_ODBC_STATE_NO_ROWS_UPDATED_OR_DELETED) == SAPDB_TRUE) {
            /* Maybe the property is unknown in system? */
            if(CreateProperty(hSessionIndexStore, hIndexStoreData->indexNameSpace, hIndexStoreData->indexName, hError)) {
                /* Try this proppatch again */
	            sqlRC = SQLExecute(hIndexStoreData->hStmtSetIndex);
	            if ( sqlRC != 0 ) {
                    Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtSetIndex, sqlRC);
                } else {
                    rc = SAPDB_TRUE;
                }
            }

            if (!rc) {
		        Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_COULD_NOT_SET_INDEX_VALUE,
                                                WDVXIS_ERR_TEXT_COULD_NOT_SET_INDEX_VALUE);
            }
        }

        if (!rc) {
		    return SAPDB_FALSE;
        }
	}

    /* Clear error */
	Error_Clear(hError);

	/* Return */
	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SAPDBXML_RetrieveDocuments( SQLHDBC                    hDBCDocStore,
                                       SQLHENV                    hEnvDocStore,
                                       XMLQSLib_WhereClause       hWhereClause,
                                       void                     **userData,
                                       XMLQSLib_DestroyUserData  *funcDestroyUserData,
                                       XMLError_Error             hError )
{

    char                sqlStmt[8000] = SQL_RETRIEVE_DOCS_PREFIX;
    XMLQSLib_WhereRow   whereRow = NULL;
    WDVXIS_SearchData   hSearchData = (WDVXIS_SearchData)*userData;
	SQLRETURN           sqlRC = 0;

    /* Checks */
    if (!hDBCDocStore || !userData || !hWhereClause || !hError) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);
		return SAPDB_FALSE;
    }

    /* Inits */
    hSearchData = (WDVXIS_SearchData)*userData;

    /* Build SQL statement */
    AddWhereRow(sqlStmt, hWhereClause->row);

    /* Do we have search data already? */
    if (!hSearchData) {
        if (!CreateSearchData(&hSearchData, hError)) {
            return SAPDB_FALSE;
        }
        *userData = hSearchData;
        *funcDestroyUserData = DestroySearchData;
    }

    /* Statement handle already there? */
    if (!hSearchData->hStmtSearchDocs) {
	    sqlRC = SQLAllocStmt(hDBCDocStore, &hSearchData->hStmtSearchDocs);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtSearchDocs, sqlRC);
		    return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hSearchData->hStmtSearchDocs, SQL_CLOSE);
    }

    /* Bind output columns */
    sqlRC = SQLBindCol(hSearchData->hStmtSearchDocs, 1, SQL_C_BINARY,
                        hSearchData->docId, XMLID_MAX_ID_LEN,
                        &hSearchData->docIdIndicator);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtSearchDocs, sqlRC);

        SQLFreeStmt(hSearchData->hStmtSearchDocs, SQL_DROP);
        hSearchData->hStmtSearchDocs = 0;

		return SAPDB_FALSE;
	}

    sqlRC = SQLBindCol(hSearchData->hStmtSearchDocs, 2, SQL_C_CHAR,
                        hSearchData->docName, WDVXIS_MAX_RESOURCE_NAME_LEN,
                        &hSearchData->docNameIndicator);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtSearchDocs, sqlRC);

        SQLFreeStmt(hSearchData->hStmtSearchDocs, SQL_DROP);
        hSearchData->hStmtSearchDocs = 0;

		return SAPDB_FALSE;
	}

    /* Execute statement */
    sqlRC = SQLExecDirect(hSearchData->hStmtSearchDocs, (SQLCHAR*)sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtSearchDocs, sqlRC);

		Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_RETRIEVE_DOCS_FAILED,
                                        WDVXIS_ERR_TEXT_RETRIEVE_DOCS_FAILED);

        return SAPDB_FALSE;
	}

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SAPDBXML_GetRetrievedDocument( SQLHDBC          hDBCDocStore,
                                          SQLHENV          hEnvDocStore,
                                          void            *userData,
                                          char            *docURI,
                                          SAPDB_UInt       docURILen,
                                          XMLError_Error   hError )
{

    WDVXIS_SearchData   hSearchData = NULL;
	SQLRETURN           sqlRC = 0;

    /* Checks */
    if (!userData || !hError) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);
		return SAPDB_FALSE;
    }

    /* Inits */
    hSearchData = (WDVXIS_SearchData)userData;

    /* Fetch found doc ids */
    sqlRC = SQLFetch(hSearchData->hStmtSearchDocs);
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
		    Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_NO_DOCS_FOUND,
                                            WDVXIS_ERR_TEXT_NO_DOCS_FOUND);

            return SAPDB_TRUE;
        } else {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtSearchDocs, sqlRC);

		    Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_RETRIEVE_DOCS_FAILED,
                                            WDVXIS_ERR_TEXT_RETRIEVE_DOCS_FAILED);

            return SAPDB_FALSE;
        }
	}

    /* Get path of found docs */
    if(!GetURIById(hDBCDocStore, hEnvDocStore, hSearchData, docURI, docURILen, hError)) {
		Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_RETRIEVE_DOCS_FAILED,
                                        WDVXIS_ERR_TEXT_RETRIEVE_DOCS_FAILED);

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

SAPDB_Bool CreateIndexStoreData( WDVXIS_IndexStoreData  *hIndexStoreData,
                                 XMLError_Error          hError )
{

    SAPDB_Bool  ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_wdvxis_index_store_data), (SAPDB_UInt1**) hIndexStoreData, &ok );
	if (!ok) {
        *hIndexStoreData = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!InitIndexStoreData(*hIndexStoreData, hError)) {
        XMLMem_Free(*hIndexStoreData);
        *hIndexStoreData = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool InitIndexStoreData( WDVXIS_IndexStoreData    hIndexStoreData,
                               XMLError_Error           hError )
{

    hIndexStoreData->hStmtSetIndex = 0;
    hIndexStoreData->hStmtCreateProperty = 0;
    hIndexStoreData->hStmtGetNameSpaceId = 0;
    hIndexStoreData->hStmtCreateNameSpace = 0;

    memset(hIndexStoreData->resourceId, 0, XMLID_MAX_ID_LEN);
    hIndexStoreData->resourceIdIndicator = XMLID_MAX_ID_LEN;

    hIndexStoreData->indexNameSpace[0] = '\0';
    hIndexStoreData->indexNameSpaceIndicator = WDVXIS_MAX_NAME_SPACE_LEN+1;
    hIndexStoreData->indexName[0] = '\0';
    hIndexStoreData->indexNameIndicator = WDVXIS_MAX_PROPERTY_NAME_PREFIX_LEN+1;

    memset(hIndexStoreData->nameSpaceId, 0, XMLID_MAX_ID_LEN);
    hIndexStoreData->nameSpaceIdIndicator = XMLID_MAX_ID_LEN;
    memset(hIndexStoreData->propertyId, 0, XMLID_MAX_ID_LEN);
    hIndexStoreData->propertyIdIndicator = XMLID_MAX_ID_LEN;

    hIndexStoreData->indexValue[0] = '\0';
    hIndexStoreData->indexValueIndicator = WDVXIS_MAX_PROPERTY_SHORT_VALUE_LEN+1;

    return SAPDB_TRUE;

}

/*===========================================================================*/

void DestroyIndexStoreData( void *userData )
{

    WDVXIS_IndexStoreData  hIndexStoreData = userData;

    if (!hIndexStoreData) {
        return;
    }

    /* Free statement handles */
     if (!hIndexStoreData->hStmtSetIndex) {
        SQLFreeStmt(hIndexStoreData->hStmtSetIndex, SQL_DROP);
    }

     if (!hIndexStoreData->hStmtCreateProperty) {
        SQLFreeStmt(hIndexStoreData->hStmtCreateProperty, SQL_DROP);
    }

     if (!hIndexStoreData->hStmtGetNameSpaceId) {
        SQLFreeStmt(hIndexStoreData->hStmtGetNameSpaceId, SQL_DROP);
    }

    if (!hIndexStoreData->hStmtCreateNameSpace) {
        SQLFreeStmt(hIndexStoreData->hStmtCreateNameSpace, SQL_DROP);
    }

    /* Destroy service itself */
    XMLMem_Free(hIndexStoreData);

}

/*===========================================================================*/

SAPDB_Bool CreateSearchData( WDVXIS_SearchData  *hSearchData,
                             XMLError_Error          hError )
{

    SAPDB_Bool  ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_wdvxis_search_data), (SAPDB_UInt1**) hSearchData, &ok );
	if (!ok) {
        *hSearchData = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!InitSearchData(*hSearchData, hError)) {
        XMLMem_Free(*hSearchData);
        *hSearchData = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool InitSearchData( WDVXIS_SearchData    hSearchData,
                           XMLError_Error       hError )
{

    hSearchData->hStmtSearchDocs = 0;
    hSearchData->hStmtGetParent  = 0;

    memset(hSearchData->docId, 0, XMLID_MAX_ID_LEN);
    hSearchData->docIdIndicator = XMLID_MAX_ID_LEN;

    hSearchData->docName[0] = '\0';
    hSearchData->docNameIndicator = WDVXIS_MAX_RESOURCE_NAME_LEN+1;

    memset(hSearchData->resourceId, 0, XMLID_MAX_ID_LEN);
    hSearchData->resourceIdIndicator = XMLID_MAX_ID_LEN;
    memset(hSearchData->parentId, 0, XMLID_MAX_ID_LEN);
    hSearchData->parentIdIndicator = XMLID_MAX_ID_LEN;
    hSearchData->parentName[0] = '\0';
    hSearchData->parentNameIndicator = WDVXIS_MAX_RESOURCE_NAME_LEN+1;

    return SAPDB_TRUE;

}

/*===========================================================================*/

void DestroySearchData( void *userData )
{

    WDVXIS_SearchData hSearchData = userData;

    if (!hSearchData) {
        return;
    }

    /* Free statement handles */
     if (!hSearchData->hStmtSearchDocs) {
        SQLFreeStmt(hSearchData->hStmtSearchDocs, SQL_DROP);
    }

    if (!hSearchData->hStmtGetParent) {
        SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
    }

    /* Destroy service itself */
    XMLMem_Free(hSearchData);

}

/*===========================================================================*/

SAPDB_Bool CreateProperty( XMLSP_Session    hIndexStoreSession,
                           char            *nameSpace,
                           char            *propertyName,
                           XMLError_Error   hError )
{

    SQLRETURN               sqlRC = 0;
    SQLHENV                 hEnv = 0;
    SQLHDBC                 hDBC = 0;
    WDVXIS_IndexStoreData   hIndexStoreData = NULL;
    XMLId_Id                nameSpaceId;
    XMLId_Id                propertyId;

    /* Inits */
    if (!hIndexStoreSession) {
        /* todo: Error handling */
        return SAPDB_FALSE;
    }

    hEnv = SP_GetHEnv(hIndexStoreSession);
    hDBC = SP_GetHDBC(hIndexStoreSession);
    hIndexStoreData = SP_GetUserData(hIndexStoreSession);

    /* Is name space known? */
    if(!GetNameSpaceId(hIndexStoreSession, nameSpace, nameSpaceId, hError)) {
        if (hError->code == WDVXIS_ERR_CODE_UNKNOWN_NAME_SPACE) {
            /* Create name space */
            if (!CreateNameSpace(hIndexStoreSession, nameSpace, nameSpaceId, hError)) {
                return SAPDB_FALSE;
            }
        } else {
            return SAPDB_FALSE;
        }
    }

    /* Get new property id */
    if (!Id_GetNext(propertyId, hError)) {
        return SAPDB_FALSE;
    }

    /* Get ODBC statement handle */
    if (!hIndexStoreData->hStmtCreateProperty) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hIndexStoreData->hStmtCreateProperty);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateProperty, sqlRC);

		    return SAPDB_FALSE;
	    }

	    /* Prepare statement handle */
	    sqlRC = SQLPrepare(hIndexStoreData->hStmtCreateProperty, (SQLCHAR*)SQL_INSERT_PROPERTY, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateProperty, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateProperty, SQL_DROP);
            hIndexStoreData->hStmtCreateProperty = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(hIndexStoreData->hStmtCreateProperty, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hIndexStoreData->propertyId, XMLID_MAX_ID_LEN,
                                 &hIndexStoreData->propertyIdIndicator);
	    if (sqlRC != 0) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateProperty, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateProperty, SQL_DROP);
            hIndexStoreData->hStmtCreateProperty = 0;

            return SAPDB_FALSE;
	    }

        sqlRC = SQLBindParameter(hIndexStoreData->hStmtCreateProperty, 2,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hIndexStoreData->nameSpaceId, XMLID_MAX_ID_LEN,
                                 &hIndexStoreData->nameSpaceIdIndicator);
	    if (sqlRC != 0) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateProperty, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateProperty, SQL_DROP);
            hIndexStoreData->hStmtCreateProperty = 0;

            return SAPDB_FALSE;
	    }

        sqlRC = SQLBindParameter(hIndexStoreData->hStmtCreateProperty, 3,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hIndexStoreData->indexName, WDVXIS_MAX_PROPERTY_NAME_PREFIX_LEN,
                                &hIndexStoreData->indexNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateProperty, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateProperty, SQL_DROP);
            hIndexStoreData->hStmtCreateProperty = 0;

		    return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hIndexStoreData->hStmtCreateProperty, SQL_CLOSE);
    }

    /* Set input parameters */
    SAPDB_memcpy(hIndexStoreData->propertyId, propertyId, XMLID_MAX_ID_LEN);
    SAPDB_memcpy(hIndexStoreData->nameSpaceId, nameSpaceId, XMLID_MAX_ID_LEN);
    Com_StrMaxCopy(hIndexStoreData->indexName, propertyName, WDVXIS_MAX_PROPERTY_NAME_PREFIX_LEN);

    /* Execute statement */
    sqlRC = SQLExecute(hIndexStoreData->hStmtCreateProperty);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateProperty, sqlRC);

		return SAPDB_FALSE;
	}

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool GetNameSpaceId( XMLSP_Session    hIndexStoreSession,
                           char            *nameSpace,
                           XMLId_Id         nameSpaceId,
                           XMLError_Error   hError )
{

    SQLRETURN               sqlRC = 0;
    SQLHENV                 hEnv = 0;
    SQLHDBC                 hDBC = 0;
    WDVXIS_IndexStoreData   hIndexStoreData = NULL;

    /* Inits */
    if (!hIndexStoreSession) {
        /* todo: Error handling */
        return SAPDB_FALSE;
    }

    hEnv = SP_GetHEnv(hIndexStoreSession);
    hDBC = SP_GetHDBC(hIndexStoreSession);
    hIndexStoreData = SP_GetUserData(hIndexStoreSession);

    /* Get ODBC statement handle */
    if (!hIndexStoreData->hStmtGetNameSpaceId) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hIndexStoreData->hStmtGetNameSpaceId);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtGetNameSpaceId, sqlRC);
		    return SAPDB_FALSE;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hIndexStoreData->hStmtGetNameSpaceId, (SQLCHAR*)SQL_SELECT_NAME_SPACE_ID, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtGetNameSpaceId, sqlRC);
		    return SAPDB_FALSE;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(hIndexStoreData->hStmtGetNameSpaceId, 1,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hIndexStoreData->indexNameSpace, WDVXIS_MAX_NAME_SPACE_LEN,
                                &hIndexStoreData->indexNameSpaceIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtGetNameSpaceId, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateNameSpace, SQL_DROP);
            hIndexStoreData->hStmtCreateNameSpace = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(hIndexStoreData->hStmtGetNameSpaceId, 1, SQL_C_BINARY,
                           hIndexStoreData->nameSpaceId, XMLID_MAX_ID_LEN,
                           &hIndexStoreData->nameSpaceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtGetNameSpaceId, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateNameSpace, SQL_DROP);
            hIndexStoreData->hStmtCreateNameSpace = 0;

		    return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hIndexStoreData->hStmtGetNameSpaceId, SQL_CLOSE);
    }

    /* Set input parameters */
    Com_StrMaxCopy(hIndexStoreData->indexNameSpace, nameSpace, WDVXIS_MAX_NAME_SPACE_LEN);

	/* Execute SQL */
	sqlRC = SQLExecute(hIndexStoreData->hStmtGetNameSpaceId);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtGetNameSpaceId, sqlRC);
		return SAPDB_FALSE;
	}

	/* Fetch first rowset */
    sqlRC = SQLFetch(hIndexStoreData->hStmtGetNameSpaceId);

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_UNKNOWN_NAME_SPACE,
                                            WDVXIS_ERR_TEXT_UNKNOWN_NAME_SPACE);
        } else {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtGetNameSpaceId, sqlRC);
        }

	    return SAPDB_FALSE;
    }

    /* Set return value */
    SAPDB_memcpy(nameSpaceId, hIndexStoreData->nameSpaceId, XMLID_MAX_ID_LEN);

	/* Return */
	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool CreateNameSpace( XMLSP_Session   hIndexStoreSession,
                            char           *nameSpace,
                            XMLId_Id        nameSpaceId,
                            XMLError_Error  hError )
{

    SQLRETURN               sqlRC = 0;
    SQLHENV                 hEnv = 0;
    SQLHDBC                 hDBC = 0;
    WDVXIS_IndexStoreData   hIndexStoreData = NULL;

    /* Inits */
    if (!hIndexStoreSession) {
        /* todo: Error handling */
        return SAPDB_FALSE;
    }

    hEnv = SP_GetHEnv(hIndexStoreSession);
    hDBC = SP_GetHDBC(hIndexStoreSession);
    hIndexStoreData = SP_GetUserData(hIndexStoreSession);

    /* Get new name space id */
    if (!Id_GetNext(nameSpaceId, hError)) {
        return SAPDB_FALSE;
    }

    /* Get ODBC statement handle */
    if (!hIndexStoreData->hStmtCreateNameSpace) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hIndexStoreData->hStmtCreateNameSpace);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateNameSpace, sqlRC);

		    return SAPDB_FALSE;
	    }

	    /* Prepare statement handle */
	    sqlRC = SQLPrepare(hIndexStoreData->hStmtCreateNameSpace, (SQLCHAR*)SQL_INSERT_NAME_SPACE, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateNameSpace, sqlRC);

		    return SAPDB_FALSE;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(hIndexStoreData->hStmtCreateNameSpace, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hIndexStoreData->nameSpaceId, XMLID_MAX_ID_LEN,
                                 &hIndexStoreData->nameSpaceIdIndicator);
	    if (sqlRC != 0) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateNameSpace, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateNameSpace, SQL_DROP);
            hIndexStoreData->hStmtCreateNameSpace = 0;

            return SAPDB_FALSE;
	    }

        sqlRC = SQLBindParameter(hIndexStoreData->hStmtCreateNameSpace, 2,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hIndexStoreData->indexNameSpace, WDVXIS_MAX_NAME_SPACE_LEN,
                                &hIndexStoreData->indexNameSpaceIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateNameSpace, sqlRC);

            SQLFreeStmt(hIndexStoreData->hStmtCreateNameSpace, SQL_DROP);
            hIndexStoreData->hStmtCreateNameSpace = 0;

		    return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hIndexStoreData->hStmtCreateNameSpace, SQL_CLOSE);
    }

    /* Set input parameters */
    SAPDB_memcpy(hIndexStoreData->nameSpaceId, nameSpaceId, XMLID_MAX_ID_LEN);
    Com_StrMaxCopy(hIndexStoreData->indexNameSpace, nameSpace, WDVXIS_MAX_NAME_SPACE_LEN);

    /* Execute statement */
    sqlRC = SQLExecute(hIndexStoreData->hStmtCreateNameSpace);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hIndexStoreData->hStmtCreateNameSpace, sqlRC);

		return SAPDB_FALSE;
	}

    return SAPDB_TRUE;

}

/*===========================================================================*/

void  AddWhereRow( WDVXIS_SQLStmt       sqlStmt,
                   XMLQSLib_WhereRow    whereRow )
{

    /* Build SQL statement */
    while (whereRow) {
        strncat(sqlStmt, "(", WDVXIS_MAX_SQL_STMT_LEN);

        if (whereRow->idxObject == XMLQSLIB_IDXOBJECT_DOCCLASS) {
            /* Add doc class */
            strncat(sqlStmt, SQL_RETRIEVE_DOCS_DOC_CLASS_DESC, WDVXIS_MAX_SQL_STMT_LEN);
            strncat(sqlStmt, "'", WDVXIS_MAX_SQL_STMT_LEN);
            strncat(sqlStmt, whereRow->value, WDVXIS_MAX_SQL_STMT_LEN);
            strncat(sqlStmt, "'", WDVXIS_MAX_SQL_STMT_LEN);

            whereRow = whereRow->nextRow;
            if (whereRow && whereRow->idxObject == XMLQSLIB_IDXOBJECT_XMLINDEX) {
                strncat(sqlStmt, " AND (", WDVXIS_MAX_SQL_STMT_LEN);
                AddIndexToStmt(sqlStmt, &whereRow);
                strncat(sqlStmt, ")", WDVXIS_MAX_SQL_STMT_LEN);
            }
        } else {
            AddIndexToStmt(sqlStmt, &whereRow);
        }

        strncat(sqlStmt, ")", WDVXIS_MAX_SQL_STMT_LEN);

        if (whereRow && !(whereRow = whereRow->nextRow)) {
            strncat(sqlStmt, " OR ", WDVXIS_MAX_SQL_STMT_LEN);
        }
    }

}

/*===========================================================================*/

void AddIndexToStmt( WDVXIS_SQLStmt         sqlStmt,
                     XMLQSLib_WhereRow     *whereRow )
{   

    XMLQSLib_WhereRow   currentWhereRow = *whereRow;

    /* Extend SQL statement */
    while (currentWhereRow && currentWhereRow->idxObject == XMLQSLIB_IDXOBJECT_XMLINDEX) {
        /* Add index */
        strncat(sqlStmt, "(", WDVXIS_MAX_SQL_STMT_LEN);

        strncat(sqlStmt, SQL_RETRIEVE_DOCS_INDEX_DESC, WDVXIS_MAX_SQL_STMT_LEN);
        strncat(sqlStmt, "'", WDVXIS_MAX_SQL_STMT_LEN);
        strncat(sqlStmt, currentWhereRow->name, WDVXIS_MAX_SQL_STMT_LEN);
        strncat(sqlStmt, "'", WDVXIS_MAX_SQL_STMT_LEN);

        strncat(sqlStmt, SQL_RETRIEVE_DOCS_INDEX_VALUE_DESC, WDVXIS_MAX_SQL_STMT_LEN);
        strncat(sqlStmt, "'", WDVXIS_MAX_SQL_STMT_LEN);
        strncat(sqlStmt, currentWhereRow->value, WDVXIS_MAX_SQL_STMT_LEN);
        strncat(sqlStmt, "'", WDVXIS_MAX_SQL_STMT_LEN);

        strncat(sqlStmt, ")", WDVXIS_MAX_SQL_STMT_LEN);

        *whereRow = currentWhereRow->nextRow;
        currentWhereRow = *whereRow;

        if (currentWhereRow && currentWhereRow->idxObject == XMLQSLIB_IDXOBJECT_XMLINDEX) {
            strncat(sqlStmt, " OR ", WDVXIS_MAX_SQL_STMT_LEN);
        }
    }

}

/*===========================================================================*/

SAPDB_Bool GetURIById( SQLHDBC               hDBCDocStore,
                       SQLHENV               hEnvDocStore,
                       WDVXIS_SearchData     hSearchData,
                       char                 *docURI,
                       SAPDB_UInt            docURILen,
                       XMLError_Error        hError )
{

    WDVXIS_URIString            tmpUri = "";
    WDVXIS_URIString            newUri = "";
    SAPDB_Bool                  rootFound;
    XMLId_Id                    currentId;
    XMLId_Id                    parentId;
    WDVXIS_ResourceNameString   parentName = "";

    /* Checks */
    if (!hDBCDocStore || !hEnvDocStore || !hSearchData || !docURI || !hError) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);
		return SAPDB_FALSE;
    }

	/* Is cId root? */
    if (memcmp(hSearchData->docId, ID_ROOT, XMLID_MAX_ID_LEN) == 0) {
        strcpy(docURI, "/");

		return SAPDB_TRUE;
	}

    /* Set return value */
    Com_StrMaxCopy(newUri, hSearchData->docName, WDVXIS_MAX_URI_STRING_LEN);

    /* Get whole uri */
    rootFound = SAPDB_FALSE;
    SAPDB_memcpy(currentId, hSearchData->docId, XMLID_MAX_ID_LEN);
    do {
        /* Get parent of docId */
        if (!GetParentById(hDBCDocStore, hEnvDocStore, hSearchData, currentId, parentId, parentName, hError)) {
            if (hError->code == WDVXIS_ERR_CODE_NO_MORE_PARENTS) {
                rootFound = SAPDB_TRUE;
            } else {
                /* Error handling */
                Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_CAN_NOT_GET_DOC_PATH,
                                                WDVXIS_ERR_TEXT_CAN_NOT_GET_DOC_PATH);
		        return SAPDB_FALSE;
            }
        }

        /* Build new uri */
        if (rootFound == SAPDB_TRUE) {
            sp77sprintf(tmpUri, WDVXIS_MAX_URI_STRING_LEN, "/%s", newUri);
        } else {
            sp77sprintf(tmpUri, WDVXIS_MAX_URI_STRING_LEN, "%s/%s", parentName, newUri);
            SAPDB_memcpy(currentId, parentId, XMLID_MAX_ID_LEN);
        }
        Com_StrMaxCopy(newUri, tmpUri, WDVXIS_MAX_URI_STRING_LEN);
    } while (rootFound != SAPDB_TRUE);

    /* Set return value */
    Com_StrMaxCopy(docURI, newUri, docURILen);

    Error_Clear(hError);

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool GetParentById( SQLHDBC               hDBCDocStore,
                          SQLHENV               hEnvDocStore,
                          WDVXIS_SearchData     hSearchData,
                          XMLId_Id              resourceId,
				          XMLId_Id              parentId,
                          WDVXIS_ResourceName   parentName,
                          XMLError_Error        hError )
{

	SQLRETURN       sqlRC = 0;
	XMLId_IdString  idString = "";

    /* Checks */
    if (!hDBCDocStore || !hEnvDocStore || !hSearchData || !resourceId || !parentId || !parentName || !hError) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);
		return SAPDB_FALSE;
    }

	/* Allocat statement handle */
    if (!hSearchData->hStmtGetParent) {
	    sqlRC = SQLAllocStmt(hDBCDocStore, &hSearchData->hStmtGetParent);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtGetParent, sqlRC);

            SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
            hSearchData->hStmtGetParent = 0;

		    return SAPDB_FALSE;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hSearchData->hStmtGetParent, (SQLCHAR*)SQL_SELECT_PARENT_BY_CID, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtGetParent, sqlRC);

            SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
            hSearchData->hStmtGetParent = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hSearchData->hStmtGetParent, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hSearchData->resourceId, XMLID_MAX_ID_LEN,
                                 &hSearchData->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtGetParent, sqlRC);

            SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
            hSearchData->hStmtGetParent = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(hSearchData->hStmtGetParent, 1, SQL_C_BINARY,
                        hSearchData->parentId, XMLID_MAX_ID_LEN,
                        &hSearchData->parentIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtGetParent, sqlRC);

            SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
            hSearchData->hStmtGetParent = 0;

		    return SAPDB_FALSE;
	    }

        sqlRC = SQLBindCol(hSearchData->hStmtGetParent, 2, SQL_C_CHAR,
                        hSearchData->parentName, WDVXIS_MAX_RESOURCE_NAME_LEN,
                        &hSearchData->parentNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtGetParent, sqlRC);

            SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
            hSearchData->hStmtGetParent = 0;

		    return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hSearchData->hStmtGetParent, SQL_CLOSE);
    }

    /* Set input parameters */
    SAPDB_memcpy(hSearchData->resourceId, resourceId, XMLID_MAX_ID_LEN);

	/* Execute SQL */
	sqlRC = SQLExecute(hSearchData->hStmtGetParent);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtGetParent, sqlRC);

        SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
        hSearchData->hStmtGetParent = 0;

		return SAPDB_FALSE;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(hSearchData->hStmtGetParent);

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_Set(XMLERROR_POS, hError, WDVXIS_ERR_CODE_NO_MORE_PARENTS, WDVXIS_ERR_TEXT_NO_MORE_PARENTS);
        } else {
            Error_SetODBC(XMLERROR_POS, hError, hEnvDocStore, hDBCDocStore, hSearchData->hStmtGetParent, sqlRC);

            SQLFreeStmt(hSearchData->hStmtGetParent, SQL_DROP);
            hSearchData->hStmtGetParent = 0;
        }

		return SAPDB_FALSE;
	}

    SAPDB_memcpy(parentId, hSearchData->parentId, XMLID_MAX_ID_LEN);
    Com_StrMaxCopy(parentName, hSearchData->parentName, WDVXIS_MAX_RESOURCE_NAME_LEN);

	/* Return */
	return SAPDB_TRUE;

}

/***********************************************************************

	End

 ***********************************************************************/
