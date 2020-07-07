/*!**********************************************************************

  module: WDVCAPI_Container.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Container Management

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#include <WDVCAPI_Definition.h>

#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Container.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define MAX_SHORT_CONTENT_BUFFER_SIZE   8000
#define MAX_CONTENT_BUFFER_SIZE         128000

#define SQL_COPY_CONTAINER          "INSERT INTO WEBDAV_Container (CId, Short_Content) \
                                     SELECT ?, Short_Content FROM WEBDAV_Container WHERE CId = ? UPDATE DUPLICATES"
#define SQL_READ_CONTAINER_CONTENT  "SELECT Content FROM WEBDAV_Container WHERE CId = ?"
#define SQL_WRITE_CONTAINER_CONTENT "UPDATE WEBDAV_Container SET Content = ? WHERE CId = ?"
#define SQL_DELETE_CONTAINER        "DELETE WEBDAV_Container WHERE CId = ?"
#define SQL_WRITE_CONTAINER         "INSERT INTO WEBDAV_Container SET CId = ?, Content = ? UPDATE DUPLICATES"
#define SQL_READ_CONTAINER          "SELECT Short_Content, Content FROM WEBDAV_Container WHERE CId = ?"    

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_container_handle {
    SQLHSTMT                    hStmtWriteContainer;
    SQLHSTMT                    hStmtReadContainer;
    SQLHSTMT                    hStmtDeleteContainer;
    SQLHSTMT                    hStmtCopyContainer;
    SQLHSTMT                    hStmtReadContainerContent;
    SQLHSTMT                    hStmtWriteContainerContent;

    WDVCAPI_Id                  resourceId;
    SQLLEN                      resourceIdIndicator;

    WDVCAPI_Id                  copyDestinationId;
    SQLLEN                      copyDestinationIdIndicator;

    WDVCAPI_ContentLength       writeContentLength;
    WDVCAPI_ContentLength       readContentLength;

    char                        shortContent[MAX_CONTENT_BUFFER_SIZE];
    SQLLEN                      shortContentIndicator;

    char                        content[MAX_CONTENT_BUFFER_SIZE];
    SQLLEN                      contentIndicator;

    WDVCAPI_ContainerHandle     nextFreeHandle;
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Container_InitHandle( WDVCAPI_WDV              wdv,
                               WDVCAPI_ContainerHandle  hContainer );

WDV_Bool Container_AddHandleToFreeList( WDVCAPI_WDV              wdv,
                                        WDVCAPI_ContainerHandle *freeList,
                                        WDVCAPI_ContainerHandle  freeListItem );

WDV_Bool Container_GetHandleFromFreeList( WDVCAPI_WDV                wdv,
                                          WDVCAPI_ContainerHandle   *freeList,
                                          WDVCAPI_ContainerHandle   *freeListItem );

WDV_Bool Container_DestroyFreeListItem( WDVCAPI_WDV              wdv,
                                        WDVCAPI_ContainerHandle *hFreeListItem );

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Container_CreateHandle( WDVCAPI_WDV              wdv,
                                 WDVCAPI_ContainerHandle *hContainer )
{

	WDV_Bool                ok = WDV_False;
    WDVCAPI_ContainerHandle hNewContainer = NULL;

	if ( !wdv || !hContainer) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    /* Search available resource handle in cache */
    if (!Container_GetHandleFromFreeList(wdv, &wdv->hContainerFreeList, &hNewContainer)) {
        return WDV_False;
    }
    
    if (!hNewContainer) {
	    WDVCAPI_Allocat(sizeof(struct st_container_handle), (WDV_UInt1**) &hNewContainer, &ok);
	    if (ok == WDV_False) {
		    return WDV_False;
	    }

	    /* Init container handle */
	    Container_InitHandle(wdv, hNewContainer);
    }

    /* Set return value */
    *hContainer = hNewContainer;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_DestroyHandle( WDVCAPI_WDV             wdv,
                                  WDVCAPI_ContainerHandle hContainer )
{

    /* Add handle to the cached handle list */
    if (!Container_AddHandleToFreeList(wdv, &wdv->hContainerFreeList, hContainer)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_DestroyHandleFreeList( WDVCAPI_WDV              wdv,
                                          WDVCAPI_ContainerHandle *hFreeList )
{

    WDVCAPI_ContainerHandle itemToDelete = NULL;


	if ( !wdv || !hFreeList) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    while (*hFreeList) {
        itemToDelete = *hFreeList;
        *hFreeList = (*hFreeList)->nextFreeHandle;

        Container_DestroyFreeListItem(wdv, &itemToDelete);
    }

    *hFreeList = NULL;

	return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Container_Copy( WDVCAPI_WDV              wdv,
                         WDVCAPI_Id               sourceId,
                         WDVCAPI_Id               destinationId )
{
	SQLRETURN               sqlRC = 0;
    SQLPOINTER              valuePointer;
    WDVCAPI_BufferLength    readLength = 0;
    WDVCAPI_ContainerHandle hContainer = NULL;
    WDVCAPI_ErrorItem       errorItem = NULL;
    WDVCAPI_ErrorState      errorState;

    TRACE_OUT(">> Container_Copy()\n");

    if (!Container_CreateHandle(wdv, &hContainer)) {
        return WDV_False;
    }

    /* Statement already prepared? */
    if (!hContainer->hStmtCopyContainer) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &hContainer->hStmtCopyContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtCopyContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtCopyContainer, SQL_DROP);
            hContainer->hStmtCopyContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hContainer->hStmtCopyContainer, (SQLCHAR*) SQL_COPY_CONTAINER, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtCopyContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtCopyContainer, SQL_DROP);
            hContainer->hStmtCopyContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

	    /* Bind input parameter */
        sqlRC = SQLBindParameter(hContainer->hStmtCopyContainer, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hContainer->copyDestinationId, WDVCAPI_MAX_ID_LEN,
                                &hContainer->copyDestinationIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtCopyContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtCopyContainer, SQL_DROP);
            hContainer->hStmtCopyContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hContainer->hStmtCopyContainer, 2, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hContainer->resourceId, WDVCAPI_MAX_ID_LEN,
                                &hContainer->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtCopyContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtCopyContainer, SQL_DROP);
            hContainer->hStmtCopyContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hContainer->hStmtCopyContainer, SQL_CLOSE);
    }

    /* Set input parameter */
    SAPDB_memcpy(hContainer->copyDestinationId, destinationId, WDVCAPI_MAX_ID_LEN);
    SAPDB_memcpy(hContainer->resourceId, sourceId, WDVCAPI_MAX_ID_LEN);

    /* Execute statement */
    sqlRC = SQLExecute(hContainer->hStmtCopyContainer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtCopyContainer, sqlRC);

        Container_DestroyHandle(wdv, hContainer);

		return WDV_False;
	}

    /* Copy container content */
    if (!hContainer->hStmtReadContainerContent) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &hContainer->hStmtReadContainerContent);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainerContent, sqlRC);

            SQLFreeStmt(hContainer->hStmtReadContainerContent, SQL_DROP);
            hContainer->hStmtReadContainerContent = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hContainer->hStmtReadContainerContent, (SQLCHAR*) SQL_READ_CONTAINER_CONTENT, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainerContent, sqlRC);

            SQLFreeStmt(hContainer->hStmtReadContainerContent, SQL_DROP);
            hContainer->hStmtReadContainerContent = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(hContainer->hStmtReadContainerContent, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hContainer->resourceId, WDVCAPI_MAX_ID_LEN,
                                &hContainer->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainerContent, sqlRC);

            SQLFreeStmt(hContainer->hStmtReadContainerContent, SQL_DROP);
            hContainer->hStmtReadContainerContent = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hContainer->hStmtReadContainerContent, SQL_CLOSE);
    }

    if (!hContainer->hStmtWriteContainerContent) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &hContainer->hStmtWriteContainerContent);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

            SQLFreeStmt(hContainer->hStmtWriteContainerContent, SQL_DROP);
            hContainer->hStmtWriteContainerContent = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

        /* Prepare statement */
        sqlRC = SQLPrepare(hContainer->hStmtWriteContainerContent, (SQLCHAR*) SQL_WRITE_CONTAINER_CONTENT, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

            SQLFreeStmt(hContainer->hStmtWriteContainerContent, SQL_DROP);
            hContainer->hStmtWriteContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(hContainer->hStmtWriteContainerContent, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY,
                                 0, 0, (PTR) 1, 0, &hContainer->contentIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

            SQLFreeStmt(hContainer->hStmtWriteContainerContent, SQL_DROP);
            hContainer->hStmtWriteContainerContent = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hContainer->hStmtWriteContainerContent, 2, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hContainer->copyDestinationId, WDVCAPI_MAX_ID_LEN,
                                &hContainer->copyDestinationIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

            SQLFreeStmt(hContainer->hStmtWriteContainerContent, SQL_DROP);
            hContainer->hStmtWriteContainerContent = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hContainer->hStmtWriteContainerContent, SQL_CLOSE);
    }

    hContainer->contentIndicator = SQL_LEN_DATA_AT_EXEC(0);

    /* Execute statement */
    sqlRC = SQLExecute(hContainer->hStmtReadContainerContent);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainerContent, sqlRC);

        Container_DestroyHandle(wdv, hContainer);

		return WDV_False;
	}

    sqlRC = SQLFetch(hContainer->hStmtReadContainerContent);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainerContent, sqlRC);

        Container_DestroyHandle(wdv, hContainer);

		return WDV_False;
	}

   sqlRC = SQLExecute(hContainer->hStmtWriteContainerContent);
	if (sqlRC != SQL_NEED_DATA) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

        Container_DestroyHandle(wdv, hContainer);

		return WDV_False;
	}

    sqlRC = SQLParamData(hContainer->hStmtWriteContainerContent, &valuePointer);
	if ( sqlRC != SQL_NEED_DATA ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

        Container_DestroyHandle(wdv, hContainer);

		return WDV_False;
	}

    /* Copy content */
    do {
        readLength = 0;

        /* Get content */
        sqlRC = SQLGetData(hContainer->hStmtReadContainerContent, 1, SQL_C_BINARY, hContainer->content, MAX_CONTENT_BUFFER_SIZE, (SQLLEN*) &readLength);
	    if ( sqlRC != 0 ) {
            switch (sqlRC) {

            case SQL_NO_DATA:
                readLength = 0;
                break;

            case SQL_SUCCESS_WITH_INFO:
                Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainerContent, sqlRC);
                if (!WDVCAPI_GetLastError(wdv, &errorItem)) {
                    Container_DestroyHandle(wdv, hContainer);

                    return WDV_False;
                }

                WDVCAPI_GetErrorState(errorItem, &errorState);
                if (strcmp(errorState, WDVCAPI_ERR_SQL_STATE_DATA_TRUNCATED)) {
                    Container_DestroyHandle(wdv, hContainer);

                    return WDV_False;
                }

                /* ???Here we have to drop the last error due to memory limits */

                readLength = MAX_CONTENT_BUFFER_SIZE;

                break;

            default:
                Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainerContent, sqlRC);

                Container_DestroyHandle(wdv, hContainer);

		        return WDV_False;
            }
	    }

        /* Put content */
        if (readLength > 0) {
            sqlRC = SQLPutData(hContainer->hStmtWriteContainerContent, hContainer->content, readLength);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

                Container_DestroyHandle(wdv, hContainer);

		        return WDV_False;
	        }

            /* Increment content length */
            hContainer->writeContentLength += readLength;
        }
    } while(readLength == MAX_CONTENT_BUFFER_SIZE);

    /* Flush content buffer */
    if (hContainer->writeContentLength == 0) {
        /* Call SQLPutData to prevent function sequence error */
        sqlRC = SQLPutData(hContainer->hStmtWriteContainerContent, NULL, 0 );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }
    };

    sqlRC = SQLParamData(hContainer->hStmtWriteContainerContent, &valuePointer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainerContent, sqlRC);

        Container_DestroyHandle(wdv, hContainer);

		return WDV_False;
	}

    /* Now we have the content length and can update the resource entry */
    /* We also set the content type here                                */
    /*
    if (!Property_UpdateContentLength(wdv, hContainer->copyDestinationId, hContainer->writeContentLength)) {
        Container_DestroyHandle(wdv, hContainer);

        return WDV_False;
    }
    */

    /* Free handle */
    Container_DestroyHandle(wdv, hContainer);

    TRACE_OUT("<< Container_Copy()\n");

    return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Container_Delete( WDVCAPI_WDV              wdv,
                           WDVCAPI_Id               resourceId )
{

	SQLRETURN               sqlRC = 0;
    WDVCAPI_ContainerHandle hContainer = NULL;

    /* Get handle */
    if (!Container_CreateHandle(wdv, &hContainer)) {
        return WDV_False;
    }

    if (!hContainer->hStmtDeleteContainer) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &hContainer->hStmtDeleteContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtDeleteContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtDeleteContainer, SQL_DROP);
            hContainer->hStmtDeleteContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hContainer->hStmtDeleteContainer, (SQLCHAR*) SQL_DELETE_CONTAINER, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtDeleteContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtDeleteContainer, SQL_DROP);
            hContainer->hStmtDeleteContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }

	    /* Bind input parameter */
        sqlRC = SQLBindParameter(hContainer->hStmtDeleteContainer, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hContainer->resourceId, WDVCAPI_MAX_ID_LEN,
                                &hContainer->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtDeleteContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtDeleteContainer, SQL_DROP);
            hContainer->hStmtDeleteContainer = 0;

            Container_DestroyHandle(wdv, hContainer);

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hContainer->hStmtDeleteContainer, SQL_CLOSE);
    }

    /* Set input parameter */
    SAPDB_memcpy(hContainer->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);

    /* Execute delete statement */
    sqlRC = SQLExecute(hContainer->hStmtDeleteContainer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtDeleteContainer, sqlRC);

        Container_DestroyHandle(wdv, hContainer);

		return WDV_False;
	}

    Container_DestroyHandle(wdv, hContainer);

    return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Container_OpenToWrite( WDVCAPI_WDV               wdv,
                                WDVCAPI_ContainerHandle   hContainer,
                                WDVCAPI_Id                resourceId )
{

	SQLRETURN         sqlRC = 0;
    SQLPOINTER        valuePointer;

    /* Statement already prepared? */
    if (!hContainer->hStmtWriteContainer) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &hContainer->hStmtWriteContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtWriteContainer, SQL_DROP);
            hContainer->hStmtWriteContainer = 0;

		    return WDV_False;
	    }

        /* Prepare statement */
        sqlRC = SQLPrepare(hContainer->hStmtWriteContainer, (SQLCHAR*) SQL_WRITE_CONTAINER, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtWriteContainer, SQL_DROP);
            hContainer->hStmtWriteContainer = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(hContainer->hStmtWriteContainer, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY,
                                 0, 0, (PTR) 1, 0, &hContainer->contentIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtWriteContainer, SQL_DROP);
            hContainer->hStmtWriteContainer = 0;

		    return WDV_False;
	    }

        hContainer->contentIndicator = SQL_LEN_DATA_AT_EXEC(0);
    } else {
        SQLFreeStmt(hContainer->hStmtWriteContainer, SQL_CLOSE);
    }

    /* Set input parameters */
    SAPDB_memcpy(hContainer->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);
    
    sqlRC = SQLExecute(hContainer->hStmtWriteContainer);
	if ( sqlRC != SQL_NEED_DATA ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

		return WDV_False;
	}

    sqlRC = SQLParamData(hContainer->hStmtWriteContainer, &valuePointer);
	if ( sqlRC != SQL_NEED_DATA ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

		return WDV_False;
	}

    /* Init content length */
    hContainer->writeContentLength = 0;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_Write( WDVCAPI_WDV               wdv,
                          WDVCAPI_ContainerHandle   hContainer,
                          WDVCAPI_Buffer            buffer,
                          WDVCAPI_BufferLength      bufferLength )
{

	SQLRETURN         sqlRC = 0;
    
    /* Write buffer into content BLOB */
    sqlRC = SQLPutData(hContainer->hStmtWriteContainer, buffer, bufferLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

		return WDV_False;
	}

    /* Increment content length */
    hContainer->writeContentLength += bufferLength;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_CloseWrite( WDVCAPI_WDV              wdv,
                               WDVCAPI_ContainerHandle  hContainer )
{

	SQLRETURN         sqlRC = 0;
    SQLPOINTER        valuePointer;

    if (hContainer->writeContentLength == 0) {
        /* Call SQLPutData to prevent function sequence error */
        sqlRC = SQLPutData(hContainer->hStmtWriteContainer, NULL, 0 );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

		    return WDV_False;
	    }
    };

    sqlRC = SQLParamData(hContainer->hStmtWriteContainer, &valuePointer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtWriteContainer, sqlRC);

		return WDV_False;
	}

    SQLFreeStmt(hContainer->hStmtWriteContainer, SQL_CLOSE);

    /* Now we have the content length and can update the resource entry */
    /* We also set the content type here                                */
    if (!Property_UpdateContentLength(wdv, hContainer->resourceId, hContainer->writeContentLength)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Container_OpenToRead( WDVCAPI_WDV              wdv,
                               WDVCAPI_ContainerHandle  hContainer,
                               WDVCAPI_Id               resourceId )
{

	SQLRETURN         sqlRC = 0;

    /* Statement already prepared? */
    if (!hContainer->hStmtReadContainer) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &hContainer->hStmtReadContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtReadContainer, SQL_DROP);
            hContainer->hStmtReadContainer = 0;

		    return WDV_False;
	    }

        /* Prepare statement */
        sqlRC = SQLPrepare(hContainer->hStmtReadContainer, (SQLCHAR*) SQL_READ_CONTAINER, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtReadContainer, SQL_DROP);
            hContainer->hStmtReadContainer = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(hContainer->hStmtReadContainer, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hContainer->resourceId, WDVCAPI_MAX_ID_LEN,
                                &hContainer->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtReadContainer, SQL_DROP);
            hContainer->hStmtReadContainer = 0;

		    return WDV_False;
	    }

        /* Bind output parameter */
        sqlRC = SQLBindCol(hContainer->hStmtReadContainer, 1, SQL_C_BINARY,
                           hContainer->shortContent, MAX_SHORT_CONTENT_BUFFER_SIZE,
                          &hContainer->shortContentIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);

            SQLFreeStmt(hContainer->hStmtReadContainer, SQL_DROP);
            hContainer->hStmtReadContainer = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(hContainer->hStmtReadContainer, SQL_CLOSE);
    }

    /* Init input parameter */
    SAPDB_memcpy(hContainer->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);
    
    sqlRC = SQLExecute(hContainer->hStmtReadContainer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);

		return WDV_False;
	}

    sqlRC = SQLFetch(hContainer->hStmtReadContainer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);

		return WDV_False;
	}

    /* Init content length */
    hContainer->readContentLength = 0;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_Read( WDVCAPI_WDV                wdv,
                         WDVCAPI_ContainerHandle    hContainer,
                         WDV_Bool                  *endOfContent )
{

	SQLRETURN               sqlRC = 0;
    WDVCAPI_ErrorItem       errorItem = NULL;
    WDVCAPI_ErrorState      errorState;
    WDVCAPI_BufferLength    readLength = 0;

    /* Fill buffer with content */
    sqlRC = SQLGetData(hContainer->hStmtReadContainer, 2, SQL_C_BINARY, hContainer->content, MAX_CONTENT_BUFFER_SIZE, (SQLLEN*) &readLength);
	if ( sqlRC != 0 ) {
        switch (sqlRC) {

        case SQL_NO_DATA:
            readLength = 0;
            break;

        case SQL_SUCCESS_WITH_INFO:
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);
            if (!WDVCAPI_GetLastError(wdv, &errorItem)) {
                return WDV_False;
            }

            WDVCAPI_GetErrorState(errorItem, &errorState);
            if (strcmp(errorState, WDVCAPI_ERR_SQL_STATE_DATA_TRUNCATED)) {
                return WDV_False;
            }

            /* ???Here we have to drop the last error due to memory limits */

            readLength = MAX_CONTENT_BUFFER_SIZE;

            break;

        default:
            Error_AddSQLErrorItem(wdv, hContainer->hStmtReadContainer, sqlRC);

		    return WDV_False;
        }
	}

    /* End of content reached? */
    if (readLength < MAX_CONTENT_BUFFER_SIZE) {
        *endOfContent = WDV_True;
    } else {
        *endOfContent = WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_CloseRead( WDVCAPI_WDV               wdv,
                              WDVCAPI_ContainerHandle   hContainer )
{

    SQLFreeStmt(hContainer->hStmtReadContainer, SQL_CLOSE);
    hContainer->hStmtReadContainer = 0;

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Container_InitHandle( WDVCAPI_WDV              wdv,
                               WDVCAPI_ContainerHandle  hContainer )
{

    hContainer->hStmtWriteContainer = 0;
    hContainer->hStmtReadContainer = 0;
    hContainer->hStmtDeleteContainer = 0;
    hContainer->hStmtCopyContainer = 0;
    hContainer->hStmtReadContainerContent = 0;
    hContainer->hStmtWriteContainerContent = 0;

    memset(hContainer->resourceId, 0, WDVCAPI_MAX_ID_LEN);
    hContainer->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;

    memset(hContainer->copyDestinationId, 0, WDVCAPI_MAX_ID_LEN);
    hContainer->copyDestinationIdIndicator = WDVCAPI_MAX_ID_LEN;

    hContainer->writeContentLength = 0;
    hContainer->readContentLength = 0;

    memset(hContainer->shortContent, 0, MAX_SHORT_CONTENT_BUFFER_SIZE);
    hContainer->shortContentIndicator = MAX_SHORT_CONTENT_BUFFER_SIZE;

    memset(hContainer->content, 0, MAX_CONTENT_BUFFER_SIZE);
    hContainer->contentIndicator = SQL_LEN_DATA_AT_EXEC(0);

    hContainer->nextFreeHandle = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_AddHandleToFreeList( WDVCAPI_WDV              wdv,
                                        WDVCAPI_ContainerHandle *freeList,
                                        WDVCAPI_ContainerHandle  freeListItem )
{

    if (!wdv || !freeList || !freeListItem) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    freeListItem->nextFreeHandle = *freeList;
    *freeList = freeListItem;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_GetHandleFromFreeList( WDVCAPI_WDV                wdv,
                                          WDVCAPI_ContainerHandle   *freeList,
                                          WDVCAPI_ContainerHandle   *freeListItem )
{

    WDVCAPI_ContainerHandle hItem = NULL;

    if (!wdv || !freeList || !freeListItem) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    *freeListItem = *freeList;

    if (*freeList) {
        *freeList = (*freeList)->nextFreeHandle;
    }

    /* Reset handle */
    hItem = *freeListItem;
    if (hItem) {
        memset(hItem->resourceId, 0, WDVCAPI_MAX_ID_LEN);
        hItem->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;

        hItem->writeContentLength = 0;
        hItem->readContentLength = 0;

        memset(hItem->shortContent, 0, MAX_SHORT_CONTENT_BUFFER_SIZE);
        hItem->shortContentIndicator = MAX_SHORT_CONTENT_BUFFER_SIZE;

        memset(hItem->content, 0, MAX_CONTENT_BUFFER_SIZE);
        hItem->contentIndicator = SQL_LEN_DATA_AT_EXEC(0);

        hItem->nextFreeHandle = NULL;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Container_DestroyFreeListItem( WDVCAPI_WDV              wdv,
                                        WDVCAPI_ContainerHandle *hFreeListItem )
{

    WDVCAPI_ContainerHandle hItem = *hFreeListItem;

    if (!wdv || !hItem) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free SQL statemenets */
    if (hItem->hStmtWriteContainer) {
        SQLFreeStmt(hItem->hStmtWriteContainer, SQL_DROP);
        hItem->hStmtWriteContainer = 0;
    }

    if (hItem->hStmtReadContainer) {
        SQLFreeStmt(hItem->hStmtReadContainer, SQL_DROP);
        hItem->hStmtReadContainer = 0;
    }

    if (hItem->hStmtDeleteContainer) {
        SQLFreeStmt(hItem->hStmtDeleteContainer, SQL_DROP);
        hItem->hStmtDeleteContainer = 0;
    }

    if (hItem->hStmtCopyContainer) {
        SQLFreeStmt(hItem->hStmtCopyContainer, SQL_DROP);
        hItem->hStmtCopyContainer = 0;
    }

    if (hItem->hStmtReadContainerContent) {
        SQLFreeStmt(hItem->hStmtReadContainerContent, SQL_DROP);
        hItem->hStmtReadContainerContent = 0;
    }

    if (hItem->hStmtWriteContainerContent) {
        SQLFreeStmt(hItem->hStmtWriteContainerContent, SQL_DROP);
        hItem->hStmtWriteContainerContent = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hItem);

    *hFreeListItem = NULL;

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
