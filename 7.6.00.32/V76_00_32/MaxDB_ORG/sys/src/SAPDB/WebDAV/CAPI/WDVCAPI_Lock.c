/*!**********************************************************************

  module: WDVCAPI_Lock.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Lock Handling

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Lock.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Property.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_INSERT_LOCK_DEPTH   \
    "INSERT INTO WEBDAV_Property Set CId = X'%s', Property_Id = X'"ID_PROPERTY_LOCK_DEPTH_STRING"', Property_Short_Value = '%d'"

#define SQL_INSERT_LOCK_TIMEOUT   \
    "INSERT INTO WEBDAV_Property Set CId = X'%s', Property_Id = X'"ID_PROPERTY_LOCK_TIMEOUT_STRING"', Property_Short_Value = '%d'"

#define SQL_INSERT_LOCK_SCOPE   \
    "INSERT INTO WEBDAV_Property Set CId = X'%s', Property_Id = X'"ID_PROPERTY_LOCK_SCOPE_STRING"', Property_Short_Value = '%d'"

#define SQL_INSERT_LOCK_TYPE   \
    "INSERT INTO WEBDAV_Property Set CId = X'%s', Property_Id = X'"ID_PROPERTY_LOCK_TYPE_STRING"', Property_Short_Value = '%d'"

#define SQL_INSERT_LOCK_OWNER   \
    "INSERT INTO WEBDAV_Property Set CId = X'%s', Property_Id = X'"ID_PROPERTY_LOCK_OWNER_STRING"', Property_Short_Value = '%s'"

#define SQL_UPDATE_LOCK_ID  \
    "UPDATE WEBDAV_Property Set Property_Short_Value = '%s' WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_ID_STRING"'"

#define SQL_INSERT_LOCK_ID  \
    "INSERT WEBDAV_Property Set Property_Short_Value = '%s', CId = X'%s', Property_Id = X'"ID_PROPERTY_LOCK_ID_STRING"'"

#define SQL_DELETE_LOCK_ID   \
    "DELETE WEBDAV_Property WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_ID_STRING"'"

#define SQL_DELETE_LOCK_DEPTH   \
    "DELETE WEBDAV_Property WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_DEPTH_STRING"'"

#define SQL_DELETE_LOCK_TIMEOUT   \
    "DELETE WEBDAV_Property WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_TIMEOUT_STRING"'"

#define SQL_DELETE_LOCK_SCOPE   \
    "DELETE WEBDAV_Property WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_SCOPE_STRING"'"

#define SQL_DELETE_LOCK_TYPE   \
    "DELETE WEBDAV_Property WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_TYPE_STRING"'"

#define SQL_DELETE_LOCK_OWNER   \
    "DELETE WEBDAV_Property WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_OWNER_STRING"'"

#define SQL_INIT_LOCK_ID    \
    "UPDATE WEBDAV_Property Set Property_Short_Value = '"ID_INITIAL_STRING"' WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_ID_STRING"'"

#define SQL_IS_RESOURCE_LOCKED  \
    "SELECT 1 FROM WEBDAV_Property WHERE CId = X'%s' AND Property_Id = X'"ID_PROPERTY_LOCK_DEPTH_STRING"'"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_wdvcapi_lock_id_list {
    WDVCAPI_URIString   lockUri;
    WDVCAPI_LockId      lockId;
    WDVCAPI_LockIdList  nextLockId;
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Lock_LockResource( WDVCAPI_WDV                  wdv,
                            WDVCAPI_Resource             lockResource,
                            WDVCAPI_Depth                lockDepth,
                            WDVCAPI_LockTimeout          lockTimeout,
                            WDVCAPI_LockScope            lockScope,
                            WDVCAPI_LockType             lockType,
                            WDVCAPI_LockOwner            lockOwner,
                            WDVCAPI_LockId               lockId,
                            WDVCAPI_ClientErrorHandling  clientErrorHandling,
                            void                        *clientData,
                            WDV_Bool                    *lockOk );

WDV_Bool Lock_CheckLocking( WDVCAPI_WDV                  wdv,
                            WDV_Bool                     checkResourceItself,
                            WDVCAPI_Resource             resource,
                            WDVCAPI_ClientErrorHandling  clientErrorHandling,
                            void                        *clientData,
                            WDV_Bool                    *lockOk );

WDV_Bool Lock_CreateLock( WDVCAPI_WDV                  wdv,
                          WDVCAPI_Resource             lockResource,
                          WDVCAPI_Depth                lockDepth,
                          WDVCAPI_LockTimeout          lockTimeout,
                          WDVCAPI_LockScope            lockScope,
                          WDVCAPI_LockType             lockType,
                          WDVCAPI_LockOwner            lockOwner,
                          WDVCAPI_LockId               lockId );

WDV_Bool Lock_RefreshLock( WDVCAPI_WDV                  wdv,
                           WDVCAPI_Resource             lockResource,
                           WDVCAPI_LockTimeout          lockTimeout,
                           WDVCAPI_LockId               lockId );

WDV_Bool Lock_DropLock( WDVCAPI_WDV                  wdv,
                        WDVCAPI_Resource             lockResource );

WDV_Bool Lock_SendErrorToClient( WDVCAPI_WDV                  wdv,
                                 WDVCAPI_Resource             resource,
                                 WDVCAPI_ClientErrorHandling  clientErrorHandling,
                                 void                        *clientData );

WDV_Bool Lock_DestroyFreeListItem( WDVCAPI_LockHandle hLock );

WDV_Bool Lock_AddLockToFreeList( WDVCAPI_WDV         wdv,
                                 WDVCAPI_LockHandle *lockFreeList,
                                 WDVCAPI_LockHandle  hLock );

WDV_Bool Lock_GetLockFromFreeList( WDVCAPI_WDV         wdv,
                                   WDVCAPI_LockHandle *lockFreeList,
                                   WDVCAPI_LockHandle *hLock );

WDV_Bool Lock_CreateIdListItem( WDVCAPI_WDV           wdv,
                                WDVCAPI_LockIdList   *lockIdListItem,
                                WDVCAPI_URI           lockUri,
                                WDVCAPI_LockIdString  lockIdString );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_LockCreateHandle( WDVCAPI_WDV          wdv,
                                   WDVCAPI_LockHandle  *hLock )
{

	WDV_Bool            ok = WDV_False;
    WDVCAPI_LockHandle *lockFreeList = NULL;

	if ( !wdv || !hLock) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    /* Inits */
    *hLock = NULL;

    /* Get handle of the cached lock handle list */
    WDV_GetLockFreeList(wdv, &lockFreeList);

    /* Search available lock handle in cache */
    if (!Lock_GetLockFromFreeList(wdv, lockFreeList, hLock)) {
        return WDV_False;
    }

    /* Don't we have any free handle? */
    if (!*hLock) {
        /* We have to allocat new handle */
	    WDVCAPI_Allocat( sizeof(struct st_wdvcapi_lock_description), (WDV_UInt1**) hLock, &ok );
	    if (ok == WDV_False) {
		    return WDV_False;
	    }

	    /* Init lock handle */
	    Lock_InitHandle(wdv, *hLock);
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_LockDestroyHandle( WDVCAPI_WDV         wdv,
                                    WDVCAPI_LockHandle  hLock )
{

    WDVCAPI_LockHandle *lockFreeList = NULL;

	if (!wdv || !hLock) {
		return WDV_False;
	}

    /* Get handle of the cached lock handle list */
    WDV_GetLockFreeList(wdv, &lockFreeList);

    /* Add handle to the cached handle list */
    if (!Lock_AddLockToFreeList(wdv, lockFreeList, hLock)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_Lock( WDVCAPI_WDV			        wdv,
                       WDVCAPI_URI			        lockUri,
					   WDVCAPI_Depth				lockDepth,
					   WDVCAPI_LockTimeout          lockTimeout,
					   WDVCAPI_LockScope			lockScope,
					   WDVCAPI_LockType				lockType,
					   WDVCAPI_LockOwner			lockOwner,
					   WDVCAPI_LockIdString         lockIdString,
                       WDVCAPI_ClientErrorHandling  clientErrorHandling,
                       void                        *clientData )
{

    WDVCAPI_Resource        lockResource = NULL;
    WDVCAPI_LockHandle      hLock = NULL;
    WDV_Bool                lockOk = WDV_True;
    WDVCAPI_LockId          lockId;

    /* Checks */
    if (!wdv || !lockUri) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
    }

    /* Is uri the deleted items folder? */
    if(strncmp(lockUri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
                                WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER);

        return WDV_False;
    }

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Convert id string to id */
    if (!lockIdString) {
        WDVCAPI_IdInitValue(lockId);
    } else {
        WDVCAPI_IdStringAsId(lockIdString, lockId);
    }

    /* Create resource objects */
    if (!Resource_CreateHandle(wdv, &lockResource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get lock resources by uri */
    if (!Resource_GetByUri(wdv, lockUri, lockResource)) {
        if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
            if (!Resource_CreateNullResource(wdv, lockUri, lockResource)) {
        	    Resource_DestroyHandle(wdv, lockResource);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        } else {
        	Resource_DestroyHandle(wdv, lockResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    /* Is something locked in URI? */
    if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
	    Resource_DestroyHandle(wdv, lockResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    if (Resource_IsLockInURI(wdv, lockResource, hLock)) {
        if (!WDVCAPI_IdsAreIdentical(hLock->id, lockId)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				               WDVCAPI_ERR_CODE_LOCK_EXISTS,
				               WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            WDVCAPI_LockDestroyHandle(wdv, hLock);
	        Resource_DestroyHandle(wdv, lockResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    WDVCAPI_LockDestroyHandle(wdv, hLock);

    /* Is lock resource itself locked? */
    if (Resource_IsLocked(lockResource, NULL)) {
        /* Refresh lock */
        if (!Lock_RefreshLock(wdv, lockResource, lockTimeout, lockId)) {
            Resource_DestroyHandle(wdv, lockResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* End transaction */
        WDV_EndTransaction(wdv);

        Resource_DestroyHandle(wdv, lockResource);

        return WDV_True;
    }

    /* Lock resource                                             */
    /* If resource doesn't exist a null resource will be created */
    if (!Lock_LockResource(wdv, lockResource, lockDepth, lockTimeout, lockScope, lockType,
                           lockOwner, lockId, clientErrorHandling, clientData, &lockOk)) {
        Resource_DestroyHandle(wdv, lockResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }
    
    /* Destroy resources */
    Resource_DestroyHandle(wdv, lockResource);

    /* Resource has been locked? */
    if (lockOk == WDV_True) {
        /* End transaction */
        WDV_EndTransaction(wdv);

        /* Set lock id string */
        WDVCAPI_IdAsString(lockId, lockIdString);

        return WDV_True;
    } else {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				           WDVCAPI_ERR_CODE_LOCK_FOUND_IN_SUBTREE,
				           WDVCAPI_ERR_TEXT_LOCK_FOUND_IN_SUBTREE);

        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_Unlock( WDVCAPI_WDV			wdv,
                         WDVCAPI_URI			lockUri,
                         WDVCAPI_LockIdString   lockIdString )
{

    WDVCAPI_Resource        lockResource = NULL;
    WDVCAPI_LockHandle      hLock = NULL;
    WDV_Bool                lockOk = WDV_True;
    WDVCAPI_LockId          lockId;

    /* Checks */
    if (!wdv || !lockUri) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
    }

    /* Is uri the deleted items folder? */
    if(strncmp(lockUri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
                                WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER);

        return WDV_False;
    }

    /* Convert id string to id */
    WDVCAPI_IdStringAsId(lockIdString, lockId );

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Create resource objects */
    if (!Resource_CreateHandle(wdv, &lockResource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get lock resources by uri */
    if (!Resource_GetByUri(wdv, lockUri, lockResource)) {
        Resource_DestroyHandle(wdv, lockResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Is uri locked? */
    if (Resource_IsLocked(lockResource, NULL)) {
        /* Create lock handle */
        if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
            Resource_DestroyHandle(wdv, lockResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Get lock description */
        if (!Lock_GetLockDescription(wdv, lockResource, hLock)) {
            WDVCAPI_LockDestroyHandle(wdv, hLock);
            Resource_DestroyHandle(wdv, lockResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Is it your own lock? */
        if (memcmp(hLock->id, lockId, WDVCAPI_MAX_ID_LEN) != 0) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_LOCK_EXISTS,
                               WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            WDVCAPI_LockDestroyHandle(wdv, hLock);
            Resource_DestroyHandle(wdv, lockResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }

        /* What is the state of the resource? */
        if (Resource_IsNullResource(lockResource)) {
            if (!Delete_Inode(wdv, lockResource, WDV_False)) {
                WDVCAPI_LockDestroyHandle(wdv, hLock);
                Resource_DestroyHandle(wdv, lockResource);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        } else {
            /* Drop lock */
            if (!Lock_DropLock(wdv, lockResource)) {
                WDVCAPI_LockDestroyHandle(wdv, hLock);
                Resource_DestroyHandle(wdv, lockResource);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        }

        WDVCAPI_LockDestroyHandle(wdv, hLock);
        Resource_DestroyHandle(wdv, lockResource);

        /* End transaction */
        WDV_EndTransaction(wdv);

        return WDV_True;
    } else {
        Resource_DestroyHandle(wdv, lockResource);

        /* End transaction */
        WDV_EndTransaction(wdv);

        return WDV_True;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_LockCreateIdList( WDVCAPI_WDV         wdv,
                                   WDVCAPI_LockIdList *lockIdList )
{

	if ( !wdv || !lockIdList) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	/* Init lock id list */
    *lockIdList = NULL;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_LockDropIdList( WDVCAPI_WDV         wdv,
                                 WDVCAPI_LockIdList *lockIdList )
{

    WDVCAPI_LockIdList  nextLockIdList = NULL;

	if ( !wdv || !lockIdList) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    while (*lockIdList) {
        nextLockIdList = (*lockIdList)->nextLockId;

        WDVCAPI_Free(*lockIdList);

        *lockIdList = nextLockIdList;
    }

    *lockIdList = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_LockAddIdToList( WDVCAPI_WDV           wdv,
                                  WDVCAPI_LockIdList   *lockIdList,
                                  WDVCAPI_URI           lockUri,
                                  WDVCAPI_LockIdString  lockIdString )
{

    WDVCAPI_LockIdList  newLockIdListItem = NULL;

	if ( !wdv || !lockIdList || !lockIdString) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    /* Is uri the deleted items folder? */
    if(strncmp(lockUri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
                                WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER);

        return WDV_False;
    }

    /* Allocat new item */
    if (!Lock_CreateIdListItem(wdv, &newLockIdListItem, lockUri, lockIdString)) {
        return WDV_False;
    }

    /* Reorganize list */
    if (!*lockIdList) {
        *lockIdList = newLockIdListItem;
    } else {
        newLockIdListItem->nextLockId = *lockIdList;
        *lockIdList = newLockIdListItem;
    }

    return WDV_True;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Lock_DestroyFreeList( WDVCAPI_LockHandle   hLock )
{

    WDVCAPI_LockHandle itemToDelete = NULL;

	if (!hLock) {
		return WDV_False;
	}

    while (hLock) {
        itemToDelete = hLock;
        hLock = hLock->nextFreeLock;

        Lock_DestroyFreeListItem(itemToDelete);
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_Copy( WDVCAPI_WDV			wdv,
                    WDVCAPI_LockHandle  hDestinationLock,
                    WDVCAPI_LockHandle  hSourceLock )
{

    SAPDB_memcpy(hDestinationLock, hSourceLock, sizeof(struct st_wdvcapi_lock_description));

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_ResetHandle( WDVCAPI_WDV           wdv,
                           WDVCAPI_LockHandle    hLock )
{

    WDVCAPI_IdInitValue(hLock->resourceId);
    WDVCAPI_IdInitValue(hLock->id);

    hLock->depth = WDVCAPI_DEPTH_UNDEFINED;
    hLock->timeout = 0;
    hLock->scope = WDVCAPI_LOCK_SCOPE_UNDEFINED;
    hLock->type = WDVCAPI_LOCK_TYPE_UNDEFINED;
    hLock->owner[0] = '\0';

    hLock->nextFreeLock = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_IsLockIdInList( WDVCAPI_WDV           wdv,
                              WDVCAPI_LockIdList    lockIdList,
                              WDVCAPI_URI           lockUri,
                              WDVCAPI_LockId        lockId )
{

    WDVCAPI_LockIdList  nextLockIdList = lockIdList;

	if ( !wdv || !lockId) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    while (nextLockIdList) {
        if (WDVCAPI_IdsAreIdentical(nextLockIdList->lockId, lockId)) {
            if (nextLockIdList->lockUri[0] == '\0' || strcmp(nextLockIdList->lockUri, lockUri) == 0) {
                return WDV_True;
            }
        }

        nextLockIdList = nextLockIdList->nextLockId;
    }

    return WDV_False;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Lock_DestroyFreeListItem( WDVCAPI_LockHandle hLock )
{

    /* Free lock handle itself */
    WDVCAPI_Free((WDV_UInt1*) hLock );

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_InitHandle( WDVCAPI_WDV           wdv,
                          WDVCAPI_LockHandle    hLock )
{

    WDVCAPI_IdInitValue(hLock->resourceId);
    WDVCAPI_IdInitValue(hLock->id);

    hLock->depth = WDVCAPI_DEPTH_UNDEFINED;
    hLock->timeout = 0;
    hLock->scope = WDVCAPI_LOCK_SCOPE_UNDEFINED;
    hLock->type = WDVCAPI_LOCK_TYPE_UNDEFINED;
    hLock->owner[0] = '\0';

    hLock->nextFreeLock = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_LockResource( WDVCAPI_WDV                  wdv,
                            WDVCAPI_Resource             lockResource,
                            WDVCAPI_Depth                lockDepth,
                            WDVCAPI_LockTimeout          lockTimeout,
                            WDVCAPI_LockScope            lockScope,
                            WDVCAPI_LockType             lockType,
                            WDVCAPI_LockOwner            lockOwner,
                            WDVCAPI_LockId               lockId,
                            WDVCAPI_ClientErrorHandling  clientErrorHandling,
                            void                        *clientData,
                            WDV_Bool                    *lockOk )
{

    /* Inits */
    *lockOk = WDV_True;

    /* Create lock */
    if (!Lock_CreateLock(wdv, lockResource, lockDepth, lockTimeout, lockScope, lockType, lockOwner, lockId)) {
        return WDV_False;
    }

    /* Is that all or do we have to lock the childs too? */
    if (lockDepth == WDVCAPI_DEPTH_INFINITE) {
        /* Check locking of all childs */
        if (!Lock_CheckLocking(wdv, WDV_False, lockResource, clientErrorHandling, clientData, lockOk)) {
            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_CheckLocking( WDVCAPI_WDV                  wdv,
                            WDV_Bool                     checkResourceItself,
                            WDVCAPI_Resource             resource,
                            WDVCAPI_ClientErrorHandling  clientErrorHandling,
                            void                        *clientData,
                            WDV_Bool                    *lockOk )
{

	SQLRETURN           sqlRC = 0;
    SQLHDBC             hDBC = 0;
	SQLHSTMT            hStmt = 0;
	SQLCHAR             sqlStmt [256] = "";
	WDVCAPI_IdString    idString = "";
    WDV_Bool            childFound = WDV_False;
    WDVCAPI_Resource    childResource = NULL;
    WDV_Bool            subLockOk;

    /* Inits */
    *lockOk = WDV_True;

    /* Create child resource handle */
    if (!Resource_CreateHandle(wdv, &childResource)) {
        return WDV_False;
    }

    /* Collect the childs of the resource */
    if (!Resource_CollectChilds(wdv, resource, childResource, &childFound)) {
        Resource_DestroyHandle(wdv, childResource);

        return WDV_False;
    }

    /* Check locking of all childs */
    while (childFound) {
        subLockOk = WDV_True;

        /* Lock resource */
        if (!Lock_CheckLocking(wdv, WDV_True, childResource, clientErrorHandling, clientData, &subLockOk)) {
            Resource_DestroyHandle(wdv, childResource);

            return WDV_False;
        }

        if (subLockOk == WDV_False) {
            *lockOk = WDV_False;
        }

        /* Get next child resource to check locking */
        if (!Resource_GetNextFromCollection(wdv, resource, childResource, &childFound)) {
            Resource_DestroyHandle(wdv, childResource);

            return WDV_False;
        }
    }

    /* Destroy child resource handle */
    Resource_DestroyHandle(wdv, childResource);

    /* Check resource itself? */
    if (checkResourceItself) {
        /* Get id string */
        Resource_GetIdAsString(resource, idString);

	    /* Build SQL statement */
		sp77sprintf(sqlStmt, 256, SQL_IS_RESOURCE_LOCKED, idString);

        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
    	    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
	    }

	    /* Execute SQL */
	    sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hStmt, sqlRC);
    	    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
	    }

	    /* Fetch rowset */
        sqlRC = SQLFetch(hStmt);

	    /* Error handling */
	    if (sqlRC != 0) {
            if (sqlRC != SQL_NO_DATA) {
                *lockOk = WDV_False;
                Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

                /* Send error description to client */
                Lock_SendErrorToClient(wdv, resource, clientErrorHandling, clientData);

		        SQLFreeStmt(hStmt, SQL_DROP);

		        return WDV_False;
            }
        } else {
            *lockOk = WDV_False;
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				               WDVCAPI_ERR_CODE_LOCK_EXISTS,
				               WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            /* Send error description to client */
            Lock_SendErrorToClient(wdv, resource, clientErrorHandling, clientData);
        }

	    /* Free statement handle */
	    SQLFreeStmt(hStmt, SQL_DROP);
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_CreateLock( WDVCAPI_WDV                  wdv,
                          WDVCAPI_Resource             lockResource,
                          WDVCAPI_Depth                lockDepth,
                          WDVCAPI_LockTimeout          lockTimeout,
                          WDVCAPI_LockScope            lockScope,
                          WDVCAPI_LockType             lockType,
                          WDVCAPI_LockOwner            lockOwner,
                          WDVCAPI_LockId               lockId )
{

	WDVCAPI_Id                          resourceId;
	WDVCAPI_IdString                    lockIdString = "";
    WDVCAPI_PropertyShortValueBuffer    propertyShortValueBuffer;

	/* Checks */

    /* Get cid if resource already exists */
    Resource_GetId(lockResource, resourceId);

    /* Insert lock depth */
    sp77sprintf(propertyShortValueBuffer, WDV_MAX_PROPERTY_SHORT_VALUE_LEN, "%d", lockDepth);
    if (!Property_SetShortValue(wdv, resourceId, ID_PROPERTY_LOCK_DEPTH, propertyShortValueBuffer)) {
        return WDV_False;
    }

    /* Insert lock timeout */
    sp77sprintf(propertyShortValueBuffer, WDV_MAX_PROPERTY_SHORT_VALUE_LEN, "%d", lockTimeout);
    if (!Property_SetShortValue(wdv, resourceId, ID_PROPERTY_LOCK_TIMEOUT, propertyShortValueBuffer)) {
        return WDV_False;
    }

    /* Insert lock scope */
    sp77sprintf(propertyShortValueBuffer, WDV_MAX_PROPERTY_SHORT_VALUE_LEN, "%d", lockScope);
    if (!Property_SetShortValue(wdv, resourceId, ID_PROPERTY_LOCK_SCOPE, propertyShortValueBuffer)) {
        return WDV_False;
    }

    /* Insert lock type */
    sp77sprintf(propertyShortValueBuffer, WDV_MAX_PROPERTY_SHORT_VALUE_LEN, "%d", lockType);
    if (!Property_SetShortValue(wdv, resourceId, ID_PROPERTY_LOCK_TYPE, propertyShortValueBuffer)) {
        return WDV_False;
    }

    /* Insert lock owner */
    if (!Property_SetShortValue(wdv, resourceId, ID_PROPERTY_LOCK_OWNER, lockOwner)) {
        return WDV_False;
    }

    /* Get new lock id */
    if (!WDVCAPI_IdGetNext(wdv, lockId)) {
		return WDV_False;
    }

    /* Insert lock id */
    WDVCAPI_IdAsString(lockId, lockIdString);
    if (!Property_SetShortValue(wdv, resourceId, ID_PROPERTY_LOCK_ID, lockIdString)) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_DropLock( WDVCAPI_WDV                  wdv,
                        WDVCAPI_Resource             lockResource )
{

	SQLRETURN             sqlRC = 0;
	SQLHDBC               hDBC = 0;
	SQLHSTMT              hStmt = 0;
    SQLCHAR               sqlStmt [256] = "";
	WDVCAPI_IdString      idString = "";

    /* Get cid if resource already exists */
    Resource_GetIdAsString(lockResource, idString);

	/* Get DB handles */
	if (!GetDBC(wdv, &hDBC)) {
		Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_INVALID_HANDLE,
				            WDVCAPI_ERR_TEXT_INVALID_HANDLE );

		return WDV_False;
	}

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);

    /* Delete lock depth */
	sp77sprintf(sqlStmt, 256, SQL_DELETE_LOCK_DEPTH, idString);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    sqlRC = SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Delete lock timeout */
	sp77sprintf(sqlStmt, 256, SQL_DELETE_LOCK_TIMEOUT, idString);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    sqlRC = SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Delete lock scope */
	sp77sprintf(sqlStmt, 256, SQL_DELETE_LOCK_SCOPE, idString);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    sqlRC = SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Delete lock type */
	sp77sprintf(sqlStmt, 256, SQL_DELETE_LOCK_TYPE, idString);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    sqlRC = SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Delete lock owner */
	sp77sprintf(sqlStmt, 256, SQL_DELETE_LOCK_OWNER, idString);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    sqlRC = SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Init lock id */
	sp77sprintf(sqlStmt, 256, SQL_DELETE_LOCK_ID, idString);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_RefreshLock( WDVCAPI_WDV                  wdv,
                           WDVCAPI_Resource             lockResource,
                           WDVCAPI_LockTimeout          lockTimeout,
                           WDVCAPI_LockId               lockId )
{

    /* Today we only support infinite as a timeout value */
    /* so there is no need for a refresh                 */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_GetDescriptionByLockId( WDVCAPI_WDV          wdv,
                                      WDVCAPI_LockIdString lockIdString,
                                      WDVCAPI_LockHandle   hLock )
{

    WDVCAPI_Id          cId;
    WDVCAPI_Resource    resource = NULL;

    /* Create resource handle */
    if (!Resource_CreateHandle(wdv, &resource)) {
        return WDV_False;
    }

	/* Get cId of lock id */
    if (!Property_GetResourceIdByShortValueAndPropertyId(wdv, ID_PROPERTY_LOCK_ID, lockIdString, cId)) {
        Resource_DestroyHandle(wdv, resource);
        return WDV_False;
    }

    /* Get resource by resource id (it is unique) */
    if (!Resource_GetByCId(wdv, cId, resource)) {
        Resource_DestroyHandle(wdv, resource);
        return WDV_False;
    }

    /* Get lock description */
    if (!Lock_GetLockDescription(wdv, resource, hLock)) {
        Resource_DestroyHandle(wdv, resource);
        return WDV_False;
    }

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, resource);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_GetLockDescription( WDVCAPI_WDV          wdv,
                                  WDVCAPI_Resource     lockResource,
                                  WDVCAPI_LockHandle   hLock )
{

    WDVCAPI_PropertyShortValueBuffer    propertyValue = "";
    WDVCAPI_Id                          cId;

    /* Get resource id */
    Resource_GetId(lockResource, hLock->resourceId);
    
	/* Get lock id */
    if (!Property_GetShortValue(wdv, hLock->resourceId, ID_PROPERTY_LOCK_ID, propertyValue)) {
        return WDV_False;
    }
    WDVCAPI_IdStringAsId(propertyValue, hLock->id);

	/* Get lock depth */
    if (!Property_GetShortValue(wdv, hLock->resourceId, ID_PROPERTY_LOCK_DEPTH, propertyValue)) {
        return WDV_False;
    }
    hLock->depth = atoi(propertyValue);

	/* Get lock scope */
    if (!Property_GetShortValue(wdv, hLock->resourceId, ID_PROPERTY_LOCK_SCOPE, propertyValue)) {
        return WDV_False;
    }
    hLock->scope = atoi(propertyValue);

	/* Get lock type */
    if (!Property_GetShortValue(wdv, hLock->resourceId, ID_PROPERTY_LOCK_TYPE, propertyValue)) {
        return WDV_False;
    }
    hLock->type = atoi(propertyValue);

	/* Get lock timeout */
    if (!Property_GetShortValue(wdv, hLock->resourceId, ID_PROPERTY_LOCK_TIMEOUT, propertyValue)) {
        return WDV_False;
    }
    hLock->timeout = atol(propertyValue);

	/* Get lock owner */
    if (!Property_GetShortValue(wdv, hLock->resourceId, ID_PROPERTY_LOCK_OWNER, propertyValue)) {
        return WDV_False;
    }
    strcpy(hLock->owner, propertyValue);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_SendErrorToClient( WDVCAPI_WDV                  wdv,
                                 WDVCAPI_Resource             resource,
                                 WDVCAPI_ClientErrorHandling  clientErrorHandling,
                                 void                        *clientData )
{

    WDVCAPI_ErrorItem   lastError = NULL;
    WDVCAPI_URIString   currentUri = "";

    /* Get last error */
    if (WDVCAPI_GetLastError(wdv, &lastError)) {
        /* Get uri of failed resource */
        Resource_GetUriByChild(wdv, resource, currentUri);

        /* Call client callback function for error handling */
        clientErrorHandling(clientData, currentUri, lastError);
    } else {
        Resource_DestroyHandle(wdv, resource);

        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_AddLockToFreeList( WDVCAPI_WDV         wdv,
                                 WDVCAPI_LockHandle *lockFreeList,
                                 WDVCAPI_LockHandle  hLock )
{

    if (!wdv || !lockFreeList || !hLock) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    hLock->nextFreeLock = *lockFreeList;
    *lockFreeList = hLock;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_GetLockFromFreeList( WDVCAPI_WDV         wdv,
                                   WDVCAPI_LockHandle *lockFreeList,
                                   WDVCAPI_LockHandle *hLock )
{

    if (!wdv || !lockFreeList || !hLock) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    *hLock = *lockFreeList;

    if (*lockFreeList) {
        *lockFreeList = (*lockFreeList)->nextFreeLock;
    }

    /* Reset lock */
    if (*hLock) {
        (*hLock)->nextFreeLock = NULL;
        if (!Lock_ResetHandle(wdv, *hLock)) {
            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Lock_CreateIdListItem( WDVCAPI_WDV           wdv,
                                WDVCAPI_LockIdList   *lockIdListItem,
                                WDVCAPI_URI           lockUri,
                                WDVCAPI_LockIdString  lockIdString )
{

	WDV_Bool            ok = WDV_False;
    WDVCAPI_LockIdList  newLockIdListItem = NULL;

	if ( !wdv || !lockIdListItem) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    /* We have to allocat new handle */
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_lock_id_list), (WDV_UInt1**) &newLockIdListItem, &ok );
	if (ok == WDV_False) {
        *lockIdListItem = NULL;

		return WDV_False;
	}

    /* Init new item */
    if (!lockUri) {
        newLockIdListItem->lockUri[0] = '\0';
    } else {
        Common_StrMaxCopy(newLockIdListItem->lockUri, lockUri, WDV_MAX_URI_LEN);
    }

    WDVCAPI_IdStringAsId(lockIdString, newLockIdListItem->lockId);
    newLockIdListItem->nextLockId = NULL;

    /* Set return value */
    *lockIdListItem = newLockIdListItem;

	return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
