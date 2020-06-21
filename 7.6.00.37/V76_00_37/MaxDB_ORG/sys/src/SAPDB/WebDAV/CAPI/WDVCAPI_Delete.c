/*!**********************************************************************

  module: WDVCAPI_Delete.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Delete

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

#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Delete.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define DELETED_ITEMS_FOLDER_ID     "X'000000000000000000000000000000000000000000000001'"

/* To delete all container entries according to an id */
#define SQL_DELETE_CONTAINER        "DELETE WEBDAV_CONTAINER WHERE CID = ?"

/* To delete all properties according to an id */
#define SQL_DELETE_PROPERTY         "DELETE WEBDAV_PROPERTY WHERE CID = ?"

/* To delete all inode entries where the id to delete is the parent */
#define SQL_DELETE_INODE_WHERE_PID  "DELETE WEBDAV_INODE WHERE PID = ?"

/* To delete the inode entry itself */
#define SQL_DELETE_INODE_WHERE_CID  "DELETE WEBDAV_INODE WHERE CID = ?"

/* Recursive select to collect all sub inodes to an inode wich is to be deleted */
#define SQL_SELECT_INODES_TO_DELETE \
    "DECLARE C CURSOR FOR WITH RECURSIVE InodesToDelete(InodeToDelete) AS \
     (                                                          \
        SELECT CId                                              \
        FROM WEBDAV_Inode                                       \
        WHERE PId = ?                                           \
     UNION ALL                                                  \
        SELECT CId                                              \
        FROM WEBDAV_Inode, InodesToDelete                       \
        WHERE PId = InodeToDelete                               \
     )                                                          \
     SELECT InodeToDelete                                       \
     FROM InodesToDelete"

/* Insert inode to delete into deleted items folder */
#define SQL_DELETE_INSERT_INTO_DELETED_ITEMS    \
    "UPDATE WEBDAV_INODE SET PId = "DELETED_ITEMS_FOLDER_ID" WHERE CId = ?"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_delete_handle {
    /* Statement handles */
    SQLHSTMT                hStmtContainer;
    SQLHSTMT                hStmtInodePId;
    SQLHSTMT                hStmtInodeCId;
    SQLHSTMT                hStmtProperty;
    SQLHSTMT                hStmtInodesToDelete;
    SQLHSTMT                hStmtInsertIntoDeletedItems;

    /* Inode to delete */
    WDVCAPI_Id              inodeToDelete;
    SQLLEN                  inodeToDeleteIndicator;

    /* Sub inode to delete */
    WDVCAPI_Id              subInodeToDelete;
    SQLLEN                  subInodeToDeleteIndicator;
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Delete_InitHandle( WDVCAPI_WDV          wdv,
                            WDVCAPI_DeleteHandle hDelete );

WDV_Bool Delete_PrepareInodesToDelete( WDVCAPI_WDV           wdv,
                                       WDVCAPI_DeleteHandle  hDelete );

WDV_Bool Delete_SelectInodesToDelete( WDVCAPI_WDV           wdv,
                                      WDVCAPI_DeleteHandle  hDelete );

WDV_Bool Delete_GetNextInodeToDelete( WDVCAPI_WDV           wdv,
                                      WDVCAPI_DeleteHandle  hDelete );

WDV_Bool Delete_PrepareContainer( WDVCAPI_WDV           wdv,
                                  WDVCAPI_DeleteHandle  hDelete );
    
WDV_Bool Delete_DeleteContainer( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete );

WDV_Bool Delete_PrepareInodePId( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete );
    
WDV_Bool Delete_DeleteInodePId( WDVCAPI_WDV           wdv,
                                WDVCAPI_DeleteHandle  hDelete );

WDV_Bool Delete_PrepareInodeCId( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete );
    
WDV_Bool Delete_DeleteInodeCId( WDVCAPI_WDV           wdv,
                                WDVCAPI_DeleteHandle  hDelete );

WDV_Bool Delete_PrepareProperty( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete );
    
WDV_Bool Delete_DeleteProperty( WDVCAPI_WDV           wdv,
                                WDVCAPI_DeleteHandle  hDelete );

WDV_Bool Delete_DeleteInode( WDVCAPI_WDV            wdv,
                             WDVCAPI_DeleteHandle   hDelete );

WDV_Bool Delete_DeleteIntoDeletedItems( WDVCAPI_WDV            wdv,
                                        WDVCAPI_URI            uri,
                                        WDVCAPI_LockIdString   lockIdString );

WDV_Bool Delete_SetParentDeletedItems( WDVCAPI_WDV      wdv,
                                       WDVCAPI_Resource resource );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_Delete( WDVCAPI_WDV            wdv,
                         WDVCAPI_URI            uri,
                         WDVCAPI_LockIdString   lockIdString,
                         WDV_Bool               deleteIntoDeletedItems )
{

    WDV_Bool    uriInDeletedFolder = WDV_False;

    if (!wdv || !uri) {
        return WDV_False;
    }

    /* Is uri the deleted items folder? */
    if(strcmp(uri, WDVCAPI_DELETED_ITEMS_FOLDER) == 0 ||
       strcmp(uri, WDVCAPI_DELETED_ITEMS_FOLDER"/") == 0) {
        if (!Delete_Delete(wdv, WDVCAPI_DELETED_ITEMS_FOLDER, NULL, WDV_True, WDV_True)) {
            return WDV_False;
        }
    }

    /* Is uri somewhere in the deleted items folder? */
    if(strncmp(uri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        return Delete_Delete(wdv, uri, NULL, WDV_False, WDV_True);
    } else {
        if (deleteIntoDeletedItems == WDV_True) {
            return Delete_DeleteIntoDeletedItems(wdv, uri, lockIdString);
        } else {
            return Delete_Delete(wdv, uri, lockIdString, WDV_True, WDV_False);
        }
    }

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Delete_Delete( WDVCAPI_WDV            wdv,
                        WDVCAPI_URI            uri,
                        WDVCAPI_LockIdString   lockIdString,
                        WDV_Bool               withNewTransaction,
                        WDV_Bool               withAutoCommit )
{

    WDVCAPI_Resource    resource = NULL;
    WDVCAPI_LockHandle  hLock = NULL;
    WDVCAPI_LockId      lockId;

    /* Checks */
    if (!wdv || !uri) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    /* Start transaction */
    if (withNewTransaction == WDV_True) {
        WDV_StartTransaction(wdv);
    }

    /* Inits */
    if (!lockIdString) {
        WDVCAPI_IdInitValue(lockId);
    } else {
        WDVCAPI_IdStringAsId(lockIdString, lockId);
    }

    /* Get resource description */
    if (!Resource_CreateHandle(wdv, &resource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    if (!Resource_GetByUri(wdv, uri, resource)) {
        Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Is something locked in URI? */
    if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
	    Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    if (Resource_IsLockInURI(wdv, resource, hLock)) {
        if (!WDVCAPI_IdsAreIdentical(hLock->id, lockId)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				               WDVCAPI_ERR_CODE_LOCK_EXISTS,
				               WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            WDVCAPI_LockDestroyHandle(wdv, hLock);
	        Resource_DestroyHandle(wdv, resource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    WDVCAPI_LockDestroyHandle(wdv, hLock);

    /* Delete inode */
    if (!Delete_Inode(wdv, resource, withAutoCommit)) {
        Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, resource);

    /* End transaction */
    if (withNewTransaction == WDV_True) {
        WDV_EndTransaction(wdv);
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_DeleteIntoDeletedItems( WDVCAPI_WDV            wdv,
                                        WDVCAPI_URI            uri,
                                        WDVCAPI_LockIdString   lockIdString )
{

    WDVCAPI_Resource            resource = NULL;
    WDVCAPI_Resource            deletedResource = NULL;
    WDVCAPI_LockHandle          hLock = NULL;
    WDVCAPI_LockId              lockId;
    WDVCAPI_URIString           path = "";
    WDVCAPI_URIString           deleteURI = "";
    WDVCAPI_ResourceNameString  name = "";

    /* Checks */
    if (!wdv || !uri) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    /* Inits */
    if (!lockIdString) {
        WDVCAPI_IdInitValue(lockId);
    } else {
        WDVCAPI_IdStringAsId(lockIdString, lockId);
    }

    /* Get resource description */
    if (!Resource_CreateHandle(wdv, &resource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    if (!Resource_GetByUri(wdv, uri, resource)) {
        Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Is something locked in URI? */
    if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
	    Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    if (Resource_IsLockInURI(wdv, resource, hLock)) {
        if (!WDVCAPI_IdsAreIdentical(hLock->id, lockId)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				               WDVCAPI_ERR_CODE_LOCK_EXISTS,
				               WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            WDVCAPI_LockDestroyHandle(wdv, hLock);
	        Resource_DestroyHandle(wdv, resource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    WDVCAPI_LockDestroyHandle(wdv, hLock);

    /* Do we have already a resource called uri in deleted items? */
	SplitURI(wdv, uri, path, name);

    sp77sprintf(deleteURI, WDV_MAX_URI_LEN, "/Deleted Items/%s", name);

    if (!Resource_CreateHandle(wdv, &deletedResource)) {
        WDV_CancelTransaction(wdv);
	    Resource_DestroyHandle(wdv, resource);

        return WDV_False;
    }

    if (!Resource_GetByUri(wdv, deleteURI, deletedResource)) {
        if (!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
            Resource_DestroyHandle(wdv, resource);
            Resource_DestroyHandle(wdv, deletedResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    } else {
        /* First we have to delete item with same name */
        if (!Delete_Inode(wdv, deletedResource, WDV_True)) {
            Resource_DestroyHandle(wdv, resource);
            Resource_DestroyHandle(wdv, deletedResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    WDV_EndTransaction(wdv);

    Resource_DestroyHandle(wdv, deletedResource);

    /* Set deleted items as the new parent of the resource to be deleted */
    WDV_StartTransaction(wdv);

    if (!Delete_SetParentDeletedItems(wdv, resource)) {
        Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Store original location */
    if (!Property_SetShortValue(wdv, resource->id, ID_PROPERTY_ORIGINAL_LOCATION, path)) {
        Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    WDV_EndTransaction(wdv);

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, resource);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_CreateHandle( WDVCAPI_WDV  wdv)
{

	WDV_Bool                ok = WDV_False;
    WDVCAPI_DeleteHandle    hDelete = NULL;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_delete_handle), (WDV_UInt1**) &hDelete, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init delete handle */
	Delete_InitHandle(wdv, hDelete);

    /* Set delete handle in wdv */
    SetDeleteHandle(wdv, hDelete);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_DestroyHandle( WDVCAPI_WDV  wdv)
{

    WDVCAPI_DeleteHandle   hDelete = NULL;

    /* Get delete handle from wdv handle */
    if (!GetDeleteHandle(wdv, &hDelete)) {
        return WDV_False;
    }

    /* Close statement handles */
    if (hDelete->hStmtContainer) {
        SQLFreeStmt(hDelete->hStmtContainer, SQL_DROP);
        hDelete->hStmtContainer = 0;
    }

    if (hDelete->hStmtInodePId) {
        SQLFreeStmt(hDelete->hStmtInodePId, SQL_DROP);
        hDelete->hStmtInodePId = 0;
    }

    if (hDelete->hStmtInodeCId) {
        SQLFreeStmt(hDelete->hStmtInodeCId, SQL_DROP);
        hDelete->hStmtInodeCId = 0;
    }

    if (hDelete->hStmtProperty) {
        SQLFreeStmt(hDelete->hStmtProperty, SQL_DROP);
        hDelete->hStmtProperty = 0;
    }

    if (hDelete->hStmtInodesToDelete) {
        SQLFreeStmt(hDelete->hStmtInodesToDelete, SQL_DROP);
        hDelete->hStmtInodesToDelete = 0;
    }

    if (hDelete->hStmtInsertIntoDeletedItems) {
        SQLFreeStmt(hDelete->hStmtInsertIntoDeletedItems, SQL_DROP);
        hDelete->hStmtInsertIntoDeletedItems = 0;
    }
    
    /* Free delete handle itself */
    WDVCAPI_Free((WDV_UInt1*) hDelete );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_Inode( WDVCAPI_WDV      wdv,
                       WDVCAPI_Resource resource,
                       WDV_Bool         withAutoCommit )
{

    WDVCAPI_DeleteHandle    hDelete = NULL;
    WDVCAPI_Id              resourceId;

    /* Checks */
    if (!wdv || !resource) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    /* Get delete handle */
    if (!GetDeleteHandle(wdv, &hDelete)) {
        return WDV_False;
    }

    /* Get resource description */
    if (!Resource_GetId(resource, resourceId)) {
        Resource_DestroyHandle(wdv, resource);
        return WDV_False;
    }

    SAPDB_memcpy(hDelete->inodeToDelete, resourceId, WDVCAPI_MAX_ID_LEN);
    hDelete->inodeToDeleteIndicator = WDVCAPI_MAX_ID_LEN;

    /* Select inodes to delete */
    if (!Delete_SelectInodesToDelete(wdv, hDelete)) {
        return WDV_False;
    }

    /* Iterate inodes to delete */
    while(Delete_GetNextInodeToDelete(wdv, hDelete)) {
        if (!Delete_DeleteInode(wdv, hDelete)) {
            return WDV_False;
        }

        if (withAutoCommit == WDV_True) {
            WDV_EndTransaction(wdv);
        }
    }

    /* Is there an error occured? */
    if (!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_NO_MORE_TO_DELETE)) {
        return WDV_False;
    }

    /* Delete inode itself */
    if (!Resource_IdIsRoot(hDelete->inodeToDelete) && !Resource_IdIsDeletedItems(resource->id)) {
        /* Set inode to delete */
        SAPDB_memcpy(hDelete->subInodeToDelete, hDelete->inodeToDelete, WDVCAPI_MAX_ID_LEN);

        /* Delete inode */
        if (!Delete_DeleteInode(wdv, hDelete)) {
            return WDV_False;
        }
    }

    if (withAutoCommit == WDV_True) {
        WDV_EndTransaction(wdv);
    }

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Delete_InitHandle( WDVCAPI_WDV          wdv,
                            WDVCAPI_DeleteHandle hDelete )
{

    /* Init statement handles */
    hDelete->hStmtContainer = 0;
    hDelete->hStmtInodePId = 0;
    hDelete->hStmtInodeCId = 0;
    hDelete->hStmtProperty = 0;
    hDelete->hStmtInodesToDelete = 0;
    hDelete->hStmtInsertIntoDeletedItems = 0;

    /* Init inode handle */
    memset(hDelete->inodeToDelete, 0, WDVCAPI_MAX_ID_LEN);
    hDelete->inodeToDeleteIndicator = WDVCAPI_MAX_ID_LEN;

    /* Init sub inode handle */
    memset(hDelete->subInodeToDelete, 0, WDVCAPI_MAX_ID_LEN);
    hDelete->subInodeToDeleteIndicator = WDVCAPI_MAX_ID_LEN;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_PrepareInodesToDelete( WDVCAPI_WDV           wdv,
                                       WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hDelete->hStmtInodesToDelete);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodesToDelete, sqlRC);

		return WDV_False;
	}

    sqlRC = SQLPrepare(hDelete->hStmtInodesToDelete, (SQLCHAR*)SQL_SELECT_INODES_TO_DELETE, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodesToDelete, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodesToDelete, SQL_DROP);
        hDelete->hStmtInodesToDelete = 0;

		return WDV_False;
	}

    /* Bind input parameter */
    sqlRC = SQLBindParameter( hDelete->hStmtInodesToDelete, 1,
                              SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                              hDelete->inodeToDelete, WDVCAPI_MAX_ID_LEN, &hDelete->inodeToDeleteIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodesToDelete, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodesToDelete, SQL_DROP);
        hDelete->hStmtInodesToDelete = 0;

		return WDV_False;
	}

    /* Bind output parameter */
    sqlRC = SQLBindCol( hDelete->hStmtInodesToDelete, 1,
                        SQL_C_BINARY, &hDelete->subInodeToDelete,
                        WDVCAPI_MAX_ID_LEN, &hDelete->subInodeToDeleteIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodesToDelete, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodesToDelete, SQL_DROP);
        hDelete->hStmtInodesToDelete = 0;

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_SelectInodesToDelete( WDVCAPI_WDV           wdv,
                                      WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    
    /* Is statement prepared? */
    if (hDelete->hStmtInodesToDelete == 0) {
        /* Prepare statement */
        if (!Delete_PrepareInodesToDelete(wdv, hDelete)) {
            return WDV_False;
        }
    }

    /* Get next list of inode */
    sqlRC = SQLFreeStmt(hDelete->hStmtInodesToDelete, SQL_CLOSE);

    sqlRC = SQLExecute(hDelete->hStmtInodesToDelete);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodesToDelete, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodesToDelete, SQL_DROP);
        hDelete->hStmtInodesToDelete = 0;

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_GetNextInodeToDelete( WDVCAPI_WDV           wdv,
                                      WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN   sqlRC = 0;
    

	/* Fetch rowset */
    sqlRC = SQLFetch( hDelete->hStmtInodesToDelete );

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA_FOUND) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_NO_MORE_TO_DELETE,
                               WDVCAPI_ERR_TEXT_NO_MORE_TO_DELETE);
        } else {
            Error_AddSQLErrorItem( wdv, hDelete->hStmtInodesToDelete, sqlRC );
		}

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_PrepareContainer( WDVCAPI_WDV           wdv,
                                  WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hDelete->hStmtContainer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtContainer, sqlRC);

		return WDV_False;
	}

    sqlRC = SQLPrepare(hDelete->hStmtContainer, (SQLCHAR*)SQL_DELETE_CONTAINER, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtContainer, sqlRC);

        SQLFreeStmt(hDelete->hStmtContainer, SQL_DROP);
        hDelete->hStmtContainer = 0;

		return WDV_False;
	}

    /* Bind input parameter */
    sqlRC = SQLBindParameter( hDelete->hStmtContainer, 1,
                              SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                              hDelete->subInodeToDelete, WDVCAPI_MAX_ID_LEN, &hDelete->subInodeToDeleteIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtContainer, sqlRC);

        SQLFreeStmt(hDelete->hStmtContainer, SQL_DROP);
        hDelete->hStmtContainer = 0;

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Delete_DeleteContainer( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    
    /* Is statement prepared? */
    if (hDelete->hStmtContainer == 0) {
        /* Prepare statement */
        if (!Delete_PrepareContainer(wdv, hDelete)) {
            return WDV_False;
        }
    }

    /* Delete container entries */
    sqlRC = SQLFreeStmt(hDelete->hStmtContainer, SQL_CLOSE);

    sqlRC = SQLExecute(hDelete->hStmtContainer);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtContainer, sqlRC);

        /* Just a state info? */
        if (!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_CODE_UNDEFINED)) {
            SQLFreeStmt(hDelete->hStmtContainer, SQL_DROP);
            hDelete->hStmtContainer = 0;

            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_PrepareInodePId( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hDelete->hStmtInodePId);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodePId, sqlRC);

		return WDV_False;
	}

    sqlRC = SQLPrepare(hDelete->hStmtInodePId, (SQLCHAR*)SQL_DELETE_INODE_WHERE_PID, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodePId, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodePId, SQL_DROP);
        hDelete->hStmtInodePId = 0;

		return WDV_False;
	}

    /* Bind input parameter */
    sqlRC = SQLBindParameter( hDelete->hStmtInodePId, 1,
                              SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                              hDelete->subInodeToDelete, WDVCAPI_MAX_ID_LEN, &hDelete->subInodeToDeleteIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodePId, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodePId, SQL_DROP);
        hDelete->hStmtInodePId = 0;

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Delete_DeleteInodePId( WDVCAPI_WDV           wdv,
                                WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    
    /* Is statement prepared? */
    if (hDelete->hStmtInodePId == 0) {
        /* Prepare statement */
        if (!Delete_PrepareInodePId(wdv, hDelete)) {
            return WDV_False;
        }
    }

    /* Delete inode pid entries */
    sqlRC = SQLFreeStmt(hDelete->hStmtInodePId, SQL_CLOSE);

    sqlRC = SQLExecute(hDelete->hStmtInodePId);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodePId, sqlRC);

        /* Just a state info? */
        if (!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_CODE_UNDEFINED)) {
            SQLFreeStmt(hDelete->hStmtInodePId, SQL_DROP);
            hDelete->hStmtInodePId = 0;

            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_PrepareInodeCId( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hDelete->hStmtInodeCId);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodeCId, sqlRC);

		return WDV_False;
	}

    sqlRC = SQLPrepare(hDelete->hStmtInodeCId, (SQLCHAR*)SQL_DELETE_INODE_WHERE_CID, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodeCId, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodeCId, SQL_DROP);
        hDelete->hStmtInodeCId = 0;

		return WDV_False;
	}

    /* Bind input parameter */
    sqlRC = SQLBindParameter( hDelete->hStmtInodeCId, 1,
                              SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                              hDelete->subInodeToDelete, WDVCAPI_MAX_ID_LEN, &hDelete->subInodeToDeleteIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodeCId, sqlRC);

        SQLFreeStmt(hDelete->hStmtInodeCId, SQL_DROP);
        hDelete->hStmtInodeCId = 0;

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Delete_DeleteInodeCId( WDVCAPI_WDV           wdv,
                                WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    
    /* Is statement prepared? */
    if (hDelete->hStmtInodeCId == 0) {
        /* Prepare statement */
        if (!Delete_PrepareInodeCId(wdv, hDelete)) {
            return WDV_False;
        }
    }

    /* Delete inode pid entries */
    sqlRC = SQLFreeStmt(hDelete->hStmtInodeCId, SQL_CLOSE);

    sqlRC = SQLExecute(hDelete->hStmtInodeCId);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtInodeCId, sqlRC);

        /* Just a state info? */
        if (!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_CODE_UNDEFINED)) {
            SQLFreeStmt(hDelete->hStmtInodeCId, SQL_DROP);
            hDelete->hStmtInodeCId = 0;

            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_PrepareProperty( WDVCAPI_WDV           wdv,
                                 WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hDelete->hStmtProperty);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtProperty, sqlRC);

		return WDV_False;
	}

    sqlRC = SQLPrepare(hDelete->hStmtProperty, (SQLCHAR*)SQL_DELETE_PROPERTY, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtProperty, sqlRC);

        SQLFreeStmt(hDelete->hStmtProperty, SQL_DROP);
        hDelete->hStmtProperty = 0;

		return WDV_False;
	}

    /* Bind input parameter */
    sqlRC = SQLBindParameter( hDelete->hStmtProperty, 1,
                              SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                              hDelete->subInodeToDelete, WDVCAPI_MAX_ID_LEN, &hDelete->subInodeToDeleteIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtProperty, sqlRC);

        SQLFreeStmt(hDelete->hStmtProperty, SQL_DROP);
        hDelete->hStmtProperty = 0;

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Delete_DeleteProperty( WDVCAPI_WDV           wdv,
                                WDVCAPI_DeleteHandle  hDelete )
{

	SQLRETURN         sqlRC = 0;
    
    /* Is statement prepared? */
    if (hDelete->hStmtProperty == 0) {
        /* Prepare statement */
        if (!Delete_PrepareProperty(wdv, hDelete)) {
            return WDV_False;
        }
    }

    /* Delete inode pid entries */
    sqlRC = SQLFreeStmt(hDelete->hStmtProperty, SQL_CLOSE);

    sqlRC = SQLExecute(hDelete->hStmtProperty);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hDelete->hStmtProperty, sqlRC);

        /* Just a state info? */
        if (!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_CODE_UNDEFINED)) {
            SQLFreeStmt(hDelete->hStmtProperty, SQL_DROP);
            hDelete->hStmtProperty = 0;

            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_DeleteInode( WDVCAPI_WDV            wdv,
                             WDVCAPI_DeleteHandle   hDelete )
{

    /* Delete container entries */
    if (!Delete_DeleteContainer(wdv, hDelete)) {
        return WDV_False;
    }

    /* Delete property entries */
    if (!Delete_DeleteProperty(wdv, hDelete)) {
        return WDV_False;
    }

    /* Delete inode pid entries */
    if (!Delete_DeleteInodePId(wdv, hDelete)) {
        return WDV_False;
    }

    /* Delete inode cid entries */
    if (!Delete_DeleteInodeCId(wdv, hDelete)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Delete_SetParentDeletedItems( WDVCAPI_WDV      wdv,
                                       WDVCAPI_Resource resource )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";
    
    /* Is statement prepared? */
    if (wdv->hDelete->hStmtInsertIntoDeletedItems == 0) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hDelete->hStmtInsertIntoDeletedItems);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hDelete->hStmtInsertIntoDeletedItems, sqlRC);

		    return WDV_False;
	    }

        /* Prepare statement */
        sqlRC = SQLPrepare(wdv->hDelete->hStmtInsertIntoDeletedItems, (SQLCHAR*)SQL_DELETE_INSERT_INTO_DELETED_ITEMS, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hDelete->hStmtInsertIntoDeletedItems, sqlRC);

            SQLFreeStmt(wdv->hDelete->hStmtInsertIntoDeletedItems, SQL_DROP);
            wdv->hDelete->hStmtInsertIntoDeletedItems = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->hDelete->hStmtInsertIntoDeletedItems, 1,
                                SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                wdv->hDelete->inodeToDelete, WDVCAPI_MAX_ID_LEN, &wdv->hDelete->inodeToDeleteIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hDelete->hStmtInsertIntoDeletedItems, sqlRC);

            SQLFreeStmt(wdv->hDelete->hStmtInsertIntoDeletedItems, SQL_DROP);
            wdv->hDelete->hStmtInsertIntoDeletedItems = 0;

		    return WDV_False;
	    }
    } else {
        sqlRC = SQLFreeStmt(wdv->hDelete->hStmtInsertIntoDeletedItems, SQL_CLOSE);
    }

    /* Set input parameters */
    SAPDB_memcpy(wdv->hDelete->inodeToDelete, resource->id, WDVCAPI_MAX_ID_LEN);

    sqlRC = SQLExecute(wdv->hDelete->hStmtInsertIntoDeletedItems);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hDelete->hStmtInsertIntoDeletedItems, sqlRC);

        SQLFreeStmt(wdv->hDelete->hStmtInsertIntoDeletedItems, SQL_DROP);
        wdv->hDelete->hStmtInsertIntoDeletedItems = 0;

        return WDV_False;
    }

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
