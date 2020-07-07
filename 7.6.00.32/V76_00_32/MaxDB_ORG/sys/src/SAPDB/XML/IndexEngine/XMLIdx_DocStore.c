/*!**********************************************************************

  module: XMLIdx_DocStore.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Index Engine

  description:  Document Store Handling

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2003-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/IndexEngine/XMLIdx_ErrorCodes.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_DocStore.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Memory/XMLMem_Memory.h"
#include "SAPDB/XML/Common/XMLCom_Common.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define PROPERTY_ID_DOC_CLASS_ID            "00000000000000000000000000000000000000000000000D"
#define PROPERTY_ID_PARSE_STATE             "00000000000000000000000000000000000000000000000E"
#define PROPERTY_ID_GET_CONTENT_LENGTH      "000000000000000000000000000000000000000000000004"

#define PROPERTY_VALUE_DOC_HAS_TO_BE_PARSED_LEN             1
#define PROPERTY_VALUE_DOC_HAS_TO_BE_PARSED                 "1"

#define PROPERTY_VALUE_DOC_PARSING_IN_PROCESS_LEN           1
#define PROPERTY_VALUE_DOC_PARSING_IN_PROCESS               "2"

#define PROPERTY_VALUE_DOC_PARSING_HAS_BEEN_FINISHED_LEN    1
#define PROPERTY_VALUE_DOC_PARSING_HAS_BEEN_FINISHED        "3"

/*---------------------------------------------------------------------------*/

#define SQL_SELECT_NEXT_DOC_TO_PARSE    \
    "SELECT P1.CId FROM WebDAV_Property P1 WHERE \
     P1.Property_Id = x'"PROPERTY_ID_PARSE_STATE"' AND \
     P1.Property_Short_Value = '"PROPERTY_VALUE_DOC_HAS_TO_BE_PARSED"' AND \
     ROWNO < 2 \
     WITH LOCK (IGNORE) EXCLUSIVE"

#define SQL_SELECT_DOC_CLASS_ID    \
    "SELECT P1.Property_Short_Value FROM WebDAV_Property P1 WHERE \
     P1.CId = ? AND P1.Property_Id = x'"PROPERTY_ID_DOC_CLASS_ID"'"

#define SQL_UPDATE_DOC_STATE_IN_PROCESS \
    "UPDATE WebDAV_Property SET Property_Short_Value = '"PROPERTY_VALUE_DOC_PARSING_IN_PROCESS"' \
     WHERE Property_Id = x'"PROPERTY_ID_PARSE_STATE"' AND CId = ?"

#define SQL_SELECT_CONTENT \
    "SELECT Short_Content, Content FROM WebDAV_Container WHERE CId = ?"

#define SQL_UPDATE_STATE   \
    "UPDATE WebDAV_Property SET Property_Short_Value = ?   \
     WHERE Property_Id = x'"PROPERTY_ID_PARSE_STATE"' AND CId = ?"

#define SQL_GET_CONTENT_LENGTH\
    "SELECT Property_Short_Value FROM WebDAV_Property WHERE CId = ? AND \
     Property_Id = x'"PROPERTY_ID_GET_CONTENT_LENGTH"'"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool CreateDocStoreData( XMLIdx_DocStoreData   *hDocStoreData,
                               XMLError_Error         hError );

void DestroyDocStoreData( void *userData );

SAPDB_Bool InitDocStoreData( XMLIdx_DocStoreData  hDocStoreData );

SAPDB_Bool GetContentLength( XMLSP_Session   hSessionDocStore,
                             XMLError_Error  hError );

/***********************************************************************

	Variables

 ***********************************************************************/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool DocStore_SetState( XMLSP_Session         hSessionDocStore,
                              XMLId_Id              docId,
                              XMLIdx_IndexingState  state,
                              XMLError_Error        hError )
{

    SAPDB_Bool          rc = SAPDB_TRUE;
	SQLRETURN           sqlRC = 0;
    SQLHENV             hEnv = NULL;
    SQLHDBC             hDBC = NULL;
    XMLIdx_DocStoreData hDocStoreData = NULL;

    /* Inits */
    if (!hSessionDocStore) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);

        return SAPDB_FALSE;
    }

    hEnv = SP_GetHEnv(hSessionDocStore);
    hDBC = SP_GetHDBC(hSessionDocStore);
    hDocStoreData = (XMLIdx_DocStoreData) SP_GetUserData(hSessionDocStore);

    if (!hDocStoreData) {
        if (!CreateDocStoreData(&hDocStoreData, hError)) {
            /* todo: Error handling */
            return SAPDB_FALSE;
        }

        SP_SetUserData(hSessionDocStore, hDocStoreData, DestroyDocStoreData);
    }

    /* If no statement handle exists, create a new one */
    if (hDocStoreData->hStmtSetState == 0) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hDocStoreData->hStmtSetState);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSetState, sqlRC);
		    return SAPDB_FALSE;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hDocStoreData->hStmtSetState, (SQLCHAR*)SQL_UPDATE_STATE, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSetState, sqlRC);

            SQLFreeStmt(hDocStoreData->hStmtSetState, SQL_DROP);
            hDocStoreData->hStmtSetState = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hDocStoreData->hStmtSetState, 1,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hDocStoreData->state, DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN,
                                &hDocStoreData->stateIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSetState, sqlRC);

            SQLFreeStmt(hDocStoreData->hStmtSetState, SQL_DROP);
            hDocStoreData->hStmtSetState = 0;

		    return SAPDB_FALSE;
	    }

        sqlRC = SQLBindParameter(hDocStoreData->hStmtSetState, 2,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hDocStoreData->stateResourceId, XMLID_MAX_ID_LEN,
                                 &hDocStoreData->stateResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSetState, sqlRC);

            SQLFreeStmt(hDocStoreData->hStmtSetState, SQL_DROP);
            hDocStoreData->hStmtSetState = 0;

		    return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hDocStoreData->hStmtSetState, SQL_CLOSE);
    }

	/* Set current input values */
    SAPDB_memcpy(hDocStoreData->stateResourceId, docId, XMLID_MAX_ID_LEN);

    switch (state) {
    case XMLIDX_STATE_DOC_PARSING_HAS_BEEN_FINISHED:
        Com_StrMaxCopy(hDocStoreData->state,
                       PROPERTY_VALUE_DOC_PARSING_HAS_BEEN_FINISHED,
                       DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN);
        hDocStoreData->stateIndicator = PROPERTY_VALUE_DOC_PARSING_HAS_BEEN_FINISHED_LEN;
        break;
    default:
        /* Error handling */
        Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_UNKNOWN_STATE, XMLIDX_ERR_TEXT_UNKNOWN_STATE);

        return SAPDB_FALSE;
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hDocStoreData->hStmtSetState);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSetState, sqlRC);

        return SAPDB_FALSE;
	}

    Error_Clear(hError);

	/* Return */
	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool  DocStore_SelectNextDoc( XMLSP_Session     hSessionDocStore,
                                    XMLId_Id          docId,
                                    XMLId_Id          docClassId,
                                    XMLError_Error    hError )
{

    SQLRETURN           sqlRC = 0;
    SQLHENV             hEnv = NULL;
    SQLHDBC             hDBC = NULL;
    XMLIdx_DocStoreData hDocStoreData = NULL;

    /* Inits */
    if (!hSessionDocStore) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);

        return SAPDB_FALSE;
    }

    hEnv = SP_GetHEnv(hSessionDocStore);
    hDBC = SP_GetHDBC(hSessionDocStore);
    hDocStoreData = (XMLIdx_DocStoreData) SP_GetUserData(hSessionDocStore);

    if (!hDocStoreData) {
        if (!CreateDocStoreData(&hDocStoreData, hError)) {
            /* todo: Error handling */
            return SAPDB_FALSE;
        }

        SP_SetUserData(hSessionDocStore, hDocStoreData, DestroyDocStoreData);
    }

    /* REMARK: Transaction already started by listener */

    /* If no statement handle exists, create a new one       */
    if (hDocStoreData->hStmtSelectNextDoc == 0) {
        /* Allocat new statement handle */
        sqlRC = SQLAllocStmt(hDBC, &hDocStoreData->hStmtSelectNextDoc);
        if (sqlRC != 0) {
            /* Error handling */
            /*
            SP_CancelTransaction(hSession, hError);
            */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectNextDoc, sqlRC);

            hDocStoreData->hStmtSelectNextDoc = NULL;

            return SAPDB_FALSE;
        }

        /* Prepare statement */
        sqlRC = SQLPrepare(hDocStoreData->hStmtSelectNextDoc,
                          (SQLCHAR*)SQL_SELECT_NEXT_DOC_TO_PARSE,
                           strlen(SQL_SELECT_NEXT_DOC_TO_PARSE));
        if (sqlRC != 0) {
            /* Error handling */
            /*
            SP_CancelTransaction(hSession, hError);
            */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectNextDoc, sqlRC);

            SQLFreeStmt(hDocStoreData->hStmtSelectNextDoc, SQL_DROP);
            hDocStoreData->hStmtSelectNextDoc = NULL;

            return SAPDB_FALSE;
        }

        /* Bind output parameters */
        sqlRC = SQLBindCol(hDocStoreData->hStmtSelectNextDoc, 1, SQL_C_BINARY,
                           hDocStoreData->cId, XMLID_MAX_ID_LEN,
                          &hDocStoreData->cIdIndicator);
	    if ( sqlRC != 0 ) {
            /* Error handling */
            /*
            SP_CancelTransaction(hSession, hError);
            */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectNextDoc, sqlRC);

            SQLFreeStmt(hDocStoreData->hStmtSelectNextDoc, SQL_DROP);
            hDocStoreData->hStmtSelectNextDoc = NULL;

		    return SAPDB_FALSE;
	    }
    } else {
        /* Close statement handle */
        SQLFreeStmt(hDocStoreData->hStmtSelectNextDoc, SQL_CLOSE);
    }

    /* Select next document to parse */
    sqlRC = SQLExecute(hDocStoreData->hStmtSelectNextDoc);
    if (sqlRC != 0) {
        /* Error handling */
        /*
        SP_CancelTransaction(hSession, hError);
        */
        Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectNextDoc, sqlRC);

        return SAPDB_FALSE;
    }

    sqlRC = SQLFetch(hDocStoreData->hStmtSelectNextDoc);
    if (sqlRC != 0 && sqlRC != SQL_NO_DATA) {
        /* Error handling */
        /*
        SP_CancelTransaction(hSession, hError);
        */
        Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectNextDoc, sqlRC);

        return SAPDB_FALSE;
    }

    /* Get doc class id                                      */
    if (sqlRC != SQL_NO_DATA) {
        /* If no statement handle exists, create a new one       */
        if (hDocStoreData->hStmtSelectDocClassId == 0) {
            /* Allocat new statement handle */
            sqlRC = SQLAllocStmt(hDBC, &hDocStoreData->hStmtSelectDocClassId);
            if (sqlRC != 0) {
                /* Error handling */
                /*
                SP_CancelTransaction(hSession, hError);
                */
                Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectDocClassId, sqlRC);

                hDocStoreData->hStmtSelectNextDoc = NULL;

                return SAPDB_FALSE;
            }

            /* Prepare statement */
            sqlRC = SQLPrepare(hDocStoreData->hStmtSelectDocClassId,
                              (SQLCHAR*)SQL_SELECT_DOC_CLASS_ID,
                               strlen(SQL_SELECT_DOC_CLASS_ID));
            if (sqlRC != 0) {
                /* Error handling */
                /*
                SP_CancelTransaction(hSession, hError);
                */
                Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectDocClassId, sqlRC);

                SQLFreeStmt(hDocStoreData->hStmtSelectDocClassId, SQL_DROP);
                hDocStoreData->hStmtSelectDocClassId = NULL;

                return SAPDB_FALSE;
            }

            /* Bind input parameters */
            sqlRC = SQLBindParameter(hDocStoreData->hStmtSelectDocClassId, 1,
                                     SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                     hDocStoreData->cId, XMLID_MAX_ID_LEN,
                                     &hDocStoreData->cIdIndicator);
	        if (sqlRC != 0) {
                /* Error handling */
                /*
                SP_CancelTransaction(hSession, hError);
                */
                Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectDocClassId, sqlRC);

                SQLFreeStmt(hDocStoreData->hStmtSelectDocClassId, SQL_DROP);
                hDocStoreData->hStmtSelectDocClassId = NULL;

                return SAPDB_FALSE;
	        }

            /* Bind output parameters */
            sqlRC = SQLBindCol(hDocStoreData->hStmtSelectDocClassId, 1, SQL_C_CHAR,
                            hDocStoreData->docClassIdString, XMLID_MAX_ID_STRING_LEN+1,
                            &hDocStoreData->docClassIdStringIndicator);
	        if ( sqlRC != 0 ) {
                /* Error handling */
                /*
                SP_CancelTransaction(hSession, hError);
                */
                Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectDocClassId, sqlRC);

                SQLFreeStmt(hDocStoreData->hStmtSelectDocClassId, SQL_DROP);
                hDocStoreData->hStmtSelectDocClassId = NULL;

		        return SAPDB_FALSE;
	        }
        } else {
            /* Close statement handle */
            SQLFreeStmt(hDocStoreData->hStmtSelectDocClassId, SQL_CLOSE);
        }

        /* Select next document to parse */
        sqlRC = SQLExecute(hDocStoreData->hStmtSelectDocClassId);
        if (sqlRC != 0) {
            /* Error handling */
            /*
            SP_CancelTransaction(hSession, hError);
            */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectDocClassId, sqlRC);

            return SAPDB_FALSE;
        }

        sqlRC = SQLFetch(hDocStoreData->hStmtSelectDocClassId);
        if (sqlRC != 0) {
            /* Error handling */
            /*
            SP_CancelTransaction(hSession, hError);
            */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtSelectDocClassId, sqlRC);

            return SAPDB_FALSE;
        }

        /* Update its parse state to in process */
        /* Get statement handle to update document state   */
        /* If no statement handle exists, create a new one */
        if (hDocStoreData->hStmtUpdateDocumentState == 0) {
            /* Allocat new statement handle */
            sqlRC = SQLAllocStmt(hDBC, &hDocStoreData->hStmtUpdateDocumentState);
            if (sqlRC != 0) {
                /* Error handling */
                /*
                SP_CancelTransaction(hSession, hError);
                */
                Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtUpdateDocumentState, sqlRC);

                hDocStoreData->hStmtUpdateDocumentState = NULL;

                return SAPDB_FALSE;
            }

            /* Prepare statement */
            sqlRC = SQLPrepare(hDocStoreData->hStmtUpdateDocumentState,
                              (SQLCHAR*)SQL_UPDATE_DOC_STATE_IN_PROCESS,
                               strlen(SQL_UPDATE_DOC_STATE_IN_PROCESS));
            if (sqlRC != 0) {
                /* Error handling */
                /*
                SP_CancelTransaction(hSession, hError);
                */
                Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtUpdateDocumentState, sqlRC);

                SQLFreeStmt(hDocStoreData->hStmtUpdateDocumentState, SQL_DROP);
                hDocStoreData->hStmtUpdateDocumentState = NULL;

                return SAPDB_FALSE;
            }

            /* Bind input parameters */
            sqlRC = SQLBindParameter(hDocStoreData->hStmtUpdateDocumentState, 1,
                                     SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                     hDocStoreData->cId, XMLID_MAX_ID_LEN,
                                     &hDocStoreData->cIdIndicator);
	        if (sqlRC != 0) {
                /* Error handling */
                /*
                SP_CancelTransaction(hSession, hError);
                */
                Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtUpdateDocumentState, sqlRC);

                SQLFreeStmt(hDocStoreData->hStmtUpdateDocumentState, SQL_DROP);
                hDocStoreData->hStmtUpdateDocumentState = NULL;

                return SAPDB_FALSE;
	        }
        } else {
            /* Close statement handle */
            SQLFreeStmt(hDocStoreData->hStmtUpdateDocumentState, SQL_CLOSE);
        }

        /* Update its parse state to in process */
        sqlRC = SQLExecute(hDocStoreData->hStmtUpdateDocumentState);
        if (sqlRC != 0) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtUpdateDocumentState, sqlRC);
            if (!Error_IsODBCState(hError, XMLERROR_ODBC_STATE_01S04)) {
                return SAPDB_FALSE;
            }
        }
    } else {
        /*
        SP_EndTransaction(hSession, hError);
        */
        Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_NO_DOC_TO_INDEX, XMLIDX_ERR_TEXT_NO_DOC_TO_INDEX);

        return SAPDB_TRUE;    
    }

    /* End transaction */
    /*
    SP_EndTransaction(hSession, hError);
    */

    /* Convert doc class id */
    Id_StringAsId(hDocStoreData->docClassIdString, hDocStoreData->docClassId);
    SAPDB_memcpy(docClassId, hDocStoreData->docClassId, XMLID_MAX_ID_LEN);

    /* Set doc id */
	SAPDB_memcpy(docId, hDocStoreData->cId, XMLID_MAX_ID_LEN);
    
    /* Set error code to ok */
    Error_Clear(hError);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool DocStore_ReadDoc( XMLIdx_Work      hWork,
                             unsigned char  **buffer,
                             SAPDB_Int       *bufferLength,
                             SAPDB_Int       *endOfDoc,
                             XMLError_Error   hError )
{

    SQLRETURN           sqlRC = 0;
    SQLHENV             hEnv = NULL;
    SQLHDBC             hDBC = NULL;
    XMLIdx_DocStoreData hDocStoreData = NULL;
    SQLLEN              firstBufferLength = 0;
    SQLLEN              nextBufferLength = 0;

    /* Inits */
    if (!hWork || !buffer || !bufferLength || !endOfDoc || !hError) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);

        return SAPDB_FALSE;
    }

    hEnv = SP_GetHEnv(hWork->hSessionDocStore);
    hDBC = SP_GetHDBC(hWork->hSessionDocStore);
    hDocStoreData = SP_GetUserData(hWork->hSessionDocStore);

    if (!hDocStoreData) {
        if (!CreateDocStoreData(&hDocStoreData, hError)) {
            /* todo: Error handling */
            return SAPDB_FALSE;
        }

        SP_SetUserData(hWork->hSessionDocStore, hDocStoreData, DestroyDocStoreData);
    }

    /* Set input parameters */
    SAPDB_memcpy(hDocStoreData->getContentResourceId, hWork->docId, XMLID_MAX_ID_LEN);

    /* If no statement handle exists, create a new one */
    if (hDocStoreData->hStmtGetContent == 0) {
        /* Allocat new statement handle */
        sqlRC = SQLAllocStmt(hDBC, &hDocStoreData->hStmtGetContent);
        if (sqlRC != 0) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContent, sqlRC);

            hDocStoreData->hStmtGetContent = 0;

            return SAPDB_FALSE;
        }

        /* Prepare statement */
        sqlRC = SQLPrepare(hDocStoreData->hStmtGetContent,
                          (SQLCHAR*)SQL_SELECT_CONTENT,
                           strlen(SQL_SELECT_CONTENT));
        if (sqlRC != 0) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContent, sqlRC);

            SQLFreeStmt(hDocStoreData->hStmtGetContent, SQL_DROP);
            hDocStoreData->hStmtGetContent = 0;

            return SAPDB_FALSE;
        }
    }

    if (hDocStoreData->firstBlockDone == SAPDB_FALSE) {
        /* Get content length */
        if(!GetContentLength(hWork->hSessionDocStore, hError)) {
            return SAPDB_FALSE;
        }

        /* Check buffer length                                   */
        /* The size of the first block hast to be at least 8000! */
        if (DOCSTORE_MAX_CONTENT_BUFFER < 8000) {
            /* Error handling */
            Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_BUFFER_IS_TOO_SMALL,
                                            XMLIDX_ERR_TEXT_BUFFER_IS_TOO_SMALL);

            return SAPDB_FALSE;
        }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hDocStoreData->hStmtGetContent, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hDocStoreData->getContentResourceId, XMLID_MAX_ID_LEN,
                                 &hDocStoreData->getContentResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContent, sqlRC);


            SQLFreeStmt(hDocStoreData->hStmtGetContent, SQL_DROP);
            hDocStoreData->hStmtGetContent = 0;

		    return SAPDB_FALSE;
	    }

        hDocStoreData->firstBlockDone = SAPDB_TRUE;

        sqlRC = SQLBindCol(hDocStoreData->hStmtGetContent, 1, SQL_C_BINARY,
                           hDocStoreData->contentBuffer, DOCSTORE_MAX_CONTENT_BUFFER,
                           &firstBufferLength);
	    if ( sqlRC != 0 ) {
            /* Error handling */
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContent, sqlRC);

            SQLFreeStmt(hDocStoreData->hStmtGetContent, SQL_DROP);
            hDocStoreData->hStmtGetContent = 0;

		    return SAPDB_FALSE;
	    }
    
        sqlRC = SQLExecute(hDocStoreData->hStmtGetContent);
	    if ( sqlRC != 0 ) {
            /* Error handling */

		    return SAPDB_FALSE;
	    }

        sqlRC = SQLFetch(hDocStoreData->hStmtGetContent);
	    if ( sqlRC != 0 ) {
            /* Error handling */

		    return SAPDB_FALSE;
	    }
    }

    /* Do we have more data? */
    if (hDocStoreData->numberContentLength > firstBufferLength) {
        /* Fill buffer with content */
        sqlRC = SQLGetData(hDocStoreData->hStmtGetContent, 2, SQL_C_BINARY, (char*)hDocStoreData->contentBuffer + firstBufferLength,
                           DOCSTORE_MAX_CONTENT_BUFFER - firstBufferLength, &nextBufferLength);
	    if ( sqlRC != 0 ) {
            switch (sqlRC) {
            case SQL_NO_DATA:
                *bufferLength = hDocStoreData->numberContentLength;
                *buffer = hDocStoreData->contentBuffer;
                *endOfDoc = 1;
                hDocStoreData->firstBlockDone = SAPDB_FALSE;
                SQLFreeStmt(hDocStoreData->hStmtGetContent, SQL_CLOSE);

                return SAPDB_TRUE;

            case SQL_SUCCESS_WITH_INFO:
                *bufferLength = firstBufferLength + (DOCSTORE_MAX_CONTENT_BUFFER - firstBufferLength);
                *buffer = hDocStoreData->contentBuffer;
                *endOfDoc = 0;
                return SAPDB_TRUE;

            default:
                /* Error handling */
                *bufferLength = 0;
                *buffer = hDocStoreData->contentBuffer;
                *endOfDoc = 1;
                hDocStoreData->firstBlockDone = SAPDB_FALSE;

                SQLFreeStmt(hDocStoreData->hStmtGetContent, SQL_DROP);
                hDocStoreData->hStmtGetContent = 0;

		        return SAPDB_FALSE;
            }
        } else {
            *bufferLength = nextBufferLength;
        }
    } else {
        *bufferLength = hDocStoreData->numberContentLength;
    }

    *buffer = hDocStoreData->contentBuffer;
    *endOfDoc = 1;
    hDocStoreData->firstBlockDone = SAPDB_FALSE;
    SQLFreeStmt(hDocStoreData->hStmtGetContent, SQL_CLOSE);

    return SAPDB_TRUE;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

SAPDB_Bool InitDocStoreData( XMLIdx_DocStoreData  hDocStoreData )
{

    memset(hDocStoreData->cId, 0, XMLID_MAX_ID_LEN);
    hDocStoreData->cIdIndicator = XMLID_MAX_ID_LEN ;

    memset(hDocStoreData->docClassId, 0, XMLID_MAX_ID_LEN);
    hDocStoreData->docClassIdString[0] = '\0';
    hDocStoreData->docClassIdStringIndicator = XMLID_MAX_ID_LEN;

    hDocStoreData->hStmtSelectNextDoc = 0;
    hDocStoreData->hStmtUpdateDocumentState = 0;
    hDocStoreData->hStmtSelectDocClassId = 0;

    /* Get content */
    hDocStoreData->hStmtGetContent = 0;
    hDocStoreData->hStmtGetContentLength = 0;
    hDocStoreData->firstBlockDone = SAPDB_FALSE;
    memset(hDocStoreData->getContentResourceId, 0, XMLID_MAX_ID_LEN);
    hDocStoreData->getContentResourceIdIndicator = XMLID_MAX_ID_LEN;
    memset(hDocStoreData->contentBuffer, 0, DOCSTORE_MAX_CONTENT_BUFFER);
    memset(hDocStoreData->propertyContentLength, 0, DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN); 
    hDocStoreData->propertyContentLengthIndicator = 0;
    hDocStoreData->numberContentLength = 0;

    /* Set state */
    hDocStoreData->hStmtSetState = 0;
    memset(hDocStoreData->state, 0, DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN);
    hDocStoreData->stateIndicator = 0;
    memset(hDocStoreData->stateResourceId, 0, XMLID_MAX_ID_LEN);
    hDocStoreData->stateResourceIdIndicator = XMLID_MAX_ID_LEN;

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool CreateDocStoreData( XMLIdx_DocStoreData   *hDocStoreData,
                               XMLError_Error         hError )
{

    SAPDB_Bool  ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_xmlidx_doc_store_data), (SAPDB_UInt1**) hDocStoreData, &ok );
	if (!ok) {
        *hDocStoreData = NULL;

        return SAPDB_FALSE;
	}

    if (!InitDocStoreData(*hDocStoreData)) {
        /* todo: Error Handling */

        XMLMem_Free((SAPDB_UInt1*)hDocStoreData);

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

void DestroyDocStoreData( void *userData )
{

    XMLIdx_DocStoreData hDocStoreData = (XMLIdx_DocStoreData)userData;

    if (!hDocStoreData) {
        return;
    }

    /* Free statement handles */
    if (hDocStoreData->hStmtSelectNextDoc) {
        SQLFreeStmt(hDocStoreData->hStmtSelectNextDoc, SQL_DROP);
    }

    if (hDocStoreData->hStmtSelectDocClassId) {
        SQLFreeStmt(hDocStoreData->hStmtSelectDocClassId, SQL_DROP);
    }

    if (hDocStoreData->hStmtUpdateDocumentState) {
        SQLFreeStmt(hDocStoreData->hStmtUpdateDocumentState, SQL_DROP);
    }

    if (hDocStoreData->hStmtGetContent) {
        SQLFreeStmt(hDocStoreData->hStmtGetContent, SQL_DROP);
    }

    if (hDocStoreData->hStmtGetContentLength) {
        SQLFreeStmt(hDocStoreData->hStmtGetContentLength, SQL_DROP);
    }

    if (hDocStoreData->hStmtSetState) {
        SQLFreeStmt(hDocStoreData->hStmtSetState, SQL_DROP);
    }

    /* Free itself */
    XMLMem_Free((SAPDB_UInt1*)hDocStoreData);

}

/*===========================================================================*/

SAPDB_Bool GetContentLength( XMLSP_Session   hSessionDocStore,
                             XMLError_Error  hError )
{

	SQLRETURN   sqlRC = 0;
    SQLHENV             hEnv = NULL;
    SQLHDBC             hDBC = NULL;
    XMLIdx_DocStoreData hDocStoreData = NULL;

    /* Inits */
    hEnv = SP_GetHEnv(hSessionDocStore);
    hDBC = SP_GetHDBC(hSessionDocStore);
    hDocStoreData = SP_GetUserData(hSessionDocStore);

    if (!hDocStoreData) {
        /* todo: Error handling */
        return SAPDB_FALSE;
    }

    /* Statement already parsed? */
    if (!hDocStoreData->hStmtGetContentLength) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hDocStoreData->hStmtGetContentLength);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContentLength, sqlRC);

    	    SQLFreeStmt(hDocStoreData->hStmtGetContentLength, SQL_DROP);
            hDocStoreData->hStmtGetContentLength= 0;

		    return SAPDB_FALSE;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hDocStoreData->hStmtGetContentLength, (SQLCHAR*)SQL_GET_CONTENT_LENGTH, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContentLength, sqlRC);

    	    SQLFreeStmt(hDocStoreData->hStmtGetContentLength, SQL_DROP);
            hDocStoreData->hStmtGetContentLength = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hDocStoreData->hStmtGetContentLength, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hDocStoreData->getContentResourceId, XMLID_MAX_ID_LEN,
                                  &hDocStoreData->getContentResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContentLength, sqlRC);


            SQLFreeStmt(hDocStoreData->hStmtGetContentLength, SQL_DROP);
            hDocStoreData->hStmtGetContentLength = 0;

		    return SAPDB_FALSE;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(hDocStoreData->hStmtGetContentLength, 1, SQL_C_CHAR,
                           hDocStoreData->propertyContentLength, DOCSTORE_MAX_PROPERTY_SHORT_VALUE_LEN,
                           &hDocStoreData->propertyContentLengthIndicator);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hEnv, hDBC, hDocStoreData->hStmtGetContentLength, sqlRC);

    	    SQLFreeStmt(hDocStoreData->hStmtGetContentLength, SQL_DROP);
            hDocStoreData->hStmtGetContentLength = 0;

		    return SAPDB_FALSE;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(hDocStoreData->hStmtGetContentLength, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hDocStoreData->hStmtGetContentLength);
	if ( sqlRC != 0 ) {
        /* todo: Error handling */

		return SAPDB_FALSE;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(hDocStoreData->hStmtGetContentLength);
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_Set(XMLERROR_POS, hError, XMLIDX_ERR_CODE_COULD_NOT_DETERMINE_CONTENT_LENGTH,
                                            XMLIDX_ERR_TEXT_COULD_NOT_DETERMINE_CONTENT_LENGTH);
        } else {
            /* todo: Error handling */
        }

		return SAPDB_FALSE;
	}

    hDocStoreData->numberContentLength = atol(hDocStoreData->propertyContentLength);

	/* Return */
	return SAPDB_TRUE;

}

/***********************************************************************

	End

 ***********************************************************************/
