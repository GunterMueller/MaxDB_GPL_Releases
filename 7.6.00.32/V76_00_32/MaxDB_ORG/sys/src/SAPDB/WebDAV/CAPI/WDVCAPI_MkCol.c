/*!**********************************************************************

  module: WDVCAPI_MkCol.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Making collections

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_CREATE_INODE    "INSERT INTO WEBDAV_Inode SET PId = ?, CId = ?, NAME = ?"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_mkcol_handle {
	SQLHSTMT    hStmt;

    WDVCAPI_Id                  pId;
    SQLLEN                      pIdIndicator;

    WDVCAPI_Id                  cId;
    SQLLEN                      cIdIndicator;

    WDVCAPI_ResourceNameString  name;
    SQLLEN                      nameIndicator;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool MkCol_InitHandle( WDVCAPI_WDV          wdv,
                           WDVCAPI_MkColHandle  hMkCol );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_MkCol( WDVCAPI_WDV             wdv,
					    WDVCAPI_URI             uri,
                        WDVCAPI_LockIdString    lockIdString )
{

	SQLRETURN                        sqlRC = 0;
	SQLHDBC                          hDBC = 0;
	WDVCAPI_ResourceNameString       parentName = "";
	WDVCAPI_Id                       newResourceId;
	WDVCAPI_Resource                 parent = NULL;
	WDVCAPI_Resource                 newResource = NULL;
	WDVCAPI_ErrorItem                errorItem = NULL;
    WDVCAPI_LockHandle               hLock = NULL;
    WDVCAPI_LockId                   lockId;
    WDVCAPI_PropertyShortValueBuffer propertyShortValueBuffer = "";
    WDVCAPI_MkColHandle              hMkCol = NULL;

	/* Checks */
	if (!wdv || !uri) {
		return WDV_False;
	}

    /* Is uri the deleted items folder? */
    if(strncmp(uri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
                                WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER);

        return WDV_False;
    }

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Get MkCol handle */
    WDV_GetHandleMkCol(wdv, &hMkCol);

    /* Inits */
    if (!lockIdString) {
        WDVCAPI_IdInitValue(lockId);
    } else {
        WDVCAPI_IdStringAsId(lockIdString, lockId);
    }

	/* Split URI into parent and collection name */
	SplitURI( wdv, uri, parentName, hMkCol->name );
	if ( strlen(hMkCol->name) < 1 ) {
		Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_ROOT_ALREADY_EXIST,
				            WDVCAPI_ERR_TEXT_ROOT_ALREADY_EXIST);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

	/* Create parent resource */
	if (!Resource_CreateHandle(wdv, &parent)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

	/* Create new resource */
	if (!Resource_CreateHandle(wdv, &newResource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

	/* Does parent exist? */
	if(!Resource_GetByUri(wdv, parentName, parent) && !Resource_IsNullResource(parent)) {
		if(WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
			Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_PARENT_DOESNT_EXIST,
				                WDVCAPI_ERR_TEXT_PARENT_DOESNT_EXIST );
		}

		Resource_DestroyHandle(wdv, parent);
		Resource_DestroyHandle(wdv, newResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

    /* The resource type of parent describes a collection, does it? */
    if (strcmp(parent->resourceType, PROPERTY_VALUE_COLLECTION) != 0) {
        /* Parent has to be a collection */
        Resource_DestroyHandle(wdv, parent);
		Resource_DestroyHandle(wdv, newResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION,
                           WDVCAPI_ERR_TEXT_PARENT_NOT_A_COLLECTION);

		return WDV_False;
    }

	/* Does new collection already exist? */
	if(Resource_GetByUri( wdv, uri, newResource )) {
        if (!Resource_IsNullResource(newResource)) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_COLLECTION_ALREADY_EXIST,
				                WDVCAPI_ERR_TEXT_COLLECTION_ALREADY_EXIST );
		    Resource_DestroyHandle(wdv, parent);
		    Resource_DestroyHandle(wdv, newResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }
	} else {
		if(!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
			Resource_DestroyHandle(wdv, parent);
			Resource_DestroyHandle(wdv, newResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

			return WDV_False;
		}
	}

    /* Store parent id */
	Resource_GetId(parent, hMkCol->pId);

	/* Destroy parent resource */
	Resource_DestroyHandle(wdv, parent);

    /* Is new resource already locked */
    if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
	    Resource_DestroyHandle(wdv, newResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    if (Resource_IsLockInURI(wdv, newResource, hLock)) {
        if (!WDVCAPI_IdsAreIdentical(hLock->id, lockId)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				               WDVCAPI_ERR_CODE_LOCK_EXISTS,
				               WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            WDVCAPI_LockDestroyHandle(wdv, hLock);
	        Resource_DestroyHandle(wdv, newResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    WDVCAPI_LockDestroyHandle(wdv, hLock);

    /* If new resource doesn't exist as a null resource, we have to create it */
    if (!Resource_IsNullResource(newResource)) {
	    /* Destroy new resource handle */
	    Resource_DestroyHandle(wdv, newResource);

        /* Get new id for new collection */
        if (!WDVCAPI_IdGetNext(wdv, hMkCol->cId)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }

        /* Statement already prepared? */
        if (!hMkCol->hStmt) {
	        /* Get DB handles */
	        if (!GetDBC(wdv, &hDBC)) {
		        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                    WDVCAPI_ERR_CODE_INVALID_HANDLE,
				                    WDVCAPI_ERR_TEXT_INVALID_HANDLE );

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

		        return WDV_False;
	        }

	        /* Allocat statement handle */
	        sqlRC = SQLAllocStmt(hDBC, &hMkCol->hStmt);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, hMkCol->hStmt, sqlRC );
                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
	        }

	        /* Parse SQL */
	        sqlRC = SQLPrepare(hMkCol->hStmt, (SQLCHAR *)SQL_CREATE_INODE, SQL_NTS);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, hMkCol->hStmt, sqlRC);
		        SQLFreeStmt(hMkCol->hStmt, SQL_DROP);
                hMkCol->hStmt = 0;

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
	        }

            /* Bind input parameter */
            sqlRC = SQLBindParameter( hMkCol->hStmt, 1,
                                      SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                      hMkCol->pId, WDVCAPI_MAX_ID_LEN,
                                      &hMkCol->pIdIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, hMkCol->hStmt, sqlRC);

                SQLFreeStmt(hMkCol->hStmt, SQL_DROP);
                hMkCol->hStmt = 0;

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

		        return WDV_False;
	        }

            sqlRC = SQLBindParameter( hMkCol->hStmt, 2,
                                      SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                      hMkCol->cId, WDVCAPI_MAX_ID_LEN,
                                      &hMkCol->cIdIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, hMkCol->hStmt, sqlRC);

                SQLFreeStmt(hMkCol->hStmt, SQL_DROP);
                hMkCol->hStmt = 0;

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

		        return WDV_False;
	        }

            sqlRC = SQLBindParameter( hMkCol->hStmt, 3,
                                      SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                      hMkCol->name, WDV_MAX_RESOURCE_NAME_LEN + 1,
                                      &hMkCol->nameIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, hMkCol->hStmt, sqlRC);

                SQLFreeStmt(hMkCol->hStmt, SQL_DROP);
                hMkCol->hStmt = 0;

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

		        return WDV_False;
	        }
        }

        hMkCol->nameIndicator = SQL_NTS;

	    /* Execute SQL */
	    sqlRC = SQLExecute(hMkCol->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hMkCol->hStmt, sqlRC );

    		if(!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_CODE_DUPLICATE_KEY)) {
		        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                    WDVCAPI_ERR_CODE_COLLECTION_ALREADY_EXIST,
				                    WDVCAPI_ERR_TEXT_COLLECTION_ALREADY_EXIST );
            }

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

		    return WDV_False;
	    }

        /* Insert live properties */
        if (!Property_CreateLiveProperties(wdv, hMkCol->cId, hMkCol->name, PROPERTY_VALUE_COLLECTION, PROPERTY_VALUE_SAPDBWWW_DIRECTORY, "0", WDV_False)) {
		    return WDV_False;
        }

        /*
        * Insert displayname *
        if (!Property_SetShortValue(wdv, hMkCol->cId, ID_PROPERTY_DISPLAYNAME, hMkCol->name)) {
            * Cancel transaction *
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }

        * Insert resource type *
        if (!Property_SetShortValue(wdv, hMkCol->cId, ID_PROPERTY_RESOURCE_TYPE, PROPERTY_VALUE_COLLECTION)) {
            * Cancel transaction *
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }

        * Insert content type *
        if (!Property_SetShortValue(wdv, hMkCol->cId, ID_PROPERTY_GET_CONTENT_TYPE, PROPERTY_VALUE_SAPDBWWW_DIRECTORY)) {
            * Cancel transaction *
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }

        * Insert content length *
        if (!Property_SetShortValue(wdv, hMkCol->cId, ID_PROPERTY_GET_CONTENT_LENGTH, "0")) {
            * Cancel transaction *
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }

        * Insert getlastmodified *
        if (!Property_SetLastModified(wdv, hMkCol->cId)) {
            * Cancel transaction *
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }
        */
    } else {
        /* Get id of the resource */
        Resource_GetId(newResource, newResourceId);

	    /* Destroy new resource handle */
	    Resource_DestroyHandle(wdv, newResource);

        /* Set resource state to default. The resource state could be set */
        /* to null resource, if the resource is created by a lock         */
        if (!Resource_SetState(wdv, newResourceId, WDVCAPI_RESOURCE_STATE_DEFAULT)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        if (!Property_UpdateShortValue(wdv, newResourceId, ID_PROPERTY_RESOURCE_TYPE, PROPERTY_VALUE_COLLECTION)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        if (!Property_UpdateShortValue(wdv, newResourceId, ID_PROPERTY_GET_CONTENT_TYPE, PROPERTY_VALUE_SAPDBWWW_DIRECTORY)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    /* End transaction */
    WDV_EndTransaction(wdv);

	return WDV_True;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool MkCol_CreateHandle( WDVCAPI_WDV          wdv,
                             WDVCAPI_MkColHandle *hMkCol )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hMkCol) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat( sizeof(struct st_mkcol_handle), (WDV_UInt1**) hMkCol, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init property set short value handle */
	MkCol_InitHandle(wdv, *hMkCol);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool MkCol_DestroyHandle( WDVCAPI_WDV         wdv,
                              WDVCAPI_MkColHandle hMkCol )
{

	if (!wdv || !hMkCol) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hMkCol->hStmt) {
        SQLFreeStmt(hMkCol->hStmt, SQL_DROP);
        hMkCol->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hMkCol);

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool MkCol_InitHandle( WDVCAPI_WDV          wdv,
                           WDVCAPI_MkColHandle  hMkCol )
{

    hMkCol->hStmt = 0;

    memset(hMkCol->cId, 0, WDVCAPI_MAX_ID_LEN);
    memset(hMkCol->pId, 0, WDVCAPI_MAX_ID_LEN);
    memset(hMkCol->name, 0, WDV_MAX_RESOURCE_NAME_LEN + 1);

    hMkCol->cIdIndicator = WDVCAPI_MAX_ID_LEN;
    hMkCol->pIdIndicator = WDVCAPI_MAX_ID_LEN;
    hMkCol->nameIndicator = SQL_NTS;

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
