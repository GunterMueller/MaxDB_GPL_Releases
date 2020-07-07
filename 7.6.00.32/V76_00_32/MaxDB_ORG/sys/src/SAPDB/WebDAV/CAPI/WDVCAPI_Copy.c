/*!**********************************************************************

  module: WDVCAPI_Copy.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Copy

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Copy.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Copy_CopyResource( WDVCAPI_WDV                  wdv,
                            WDVCAPI_Resource             resource,
                            WDVCAPI_Resource             destinationParentResource,
                            WDVCAPI_URI                  destinationUri,
                            WDVCAPI_ResourceName         destinationName,
                            WDVCAPI_Overwrite            overwrite,
                            WDVCAPI_LockIdList           destinationLockIdList,
                            WDV_Bool                     recursive,
                            WDV_Bool                     move,
                            WDVCAPI_ClientErrorHandling  clientErrorHandling,
                            void                        *clientData,
                            WDV_Bool                    *destinationCreated,
                            WDV_Bool                    *copyOk );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_Copy( WDVCAPI_WDV			        wdv,
                       WDVCAPI_URI			        sourceUri,
					   WDVCAPI_URI			        destinationUri,
					   WDVCAPI_Depth		        depth,
					   WDVCAPI_Overwrite	        overwrite,
                       WDVCAPI_LockIdList           destinationLockIdList,
                       WDVCAPI_ClientErrorHandling  clientErrorHandling,
                       void                        *clientData,
                       WDV_Bool                    *destinationCreated )
{

    TRACE_OUT(">> WDVCAPI_Copy()\n");

    if ( !wdv ) {
		return WDV_False;
	}

    /* Is uri the deleted items folder? */
    if(strncmp(destinationUri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
                                WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER);

        return WDV_False;
    }


    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Copy uri */
    if (!Copy_Move(wdv, sourceUri, destinationUri, depth, overwrite, destinationLockIdList,
                   WDV_False, clientErrorHandling, clientData, destinationCreated )) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* End transaction */
    WDV_EndTransaction(wdv);

    TRACE_OUT("<< WDVCAPI_Copy()\n");

    return WDV_True;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Copy_Move( WDVCAPI_WDV			         wdv,
                    WDVCAPI_URI			         sourceUri,
                    WDVCAPI_URI			         destinationUri,
					WDVCAPI_Depth		         depth,
					WDVCAPI_Overwrite	         overwrite,
                    WDVCAPI_LockIdList           destinationLockIdList,
                    WDV_Bool                     move,
                    WDVCAPI_ClientErrorHandling  clientErrorHandling,
                    void                        *clientData,
                    WDV_Bool                    *destinationCreated )
{

    WDVCAPI_Resource            sourceResource = NULL;
    WDVCAPI_Resource            destinationParentResource = NULL;
    WDVCAPI_Resource            destinationResource = NULL;
    WDVCAPI_Id                  sourceId;
    WDVCAPI_URIString           destinationParentUri = "";
    WDVCAPI_ResourceNameString  destinationName = "";
    WDV_Bool                    subCopyOk = WDV_True;
    WDVCAPI_LockHandle          hLock = NULL;

    TRACE_OUT(">> Copy_Move()\n");

    /* Checks */
    if (!wdv || !sourceUri || !destinationUri) {
        if (wdv) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

		return WDV_False;
    }

    /* Inits */
    *destinationCreated = WDV_False;

    /* Is source and destination equal? */
    TRACE_OUT(">> Copy_Move()::Is source and destination equal?\n");
    if (strcmp(sourceUri, destinationUri) == 0) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION,
                           WDVCAPI_ERR_TEXT_EQUAL_RESOURCE_AND_DESTINATION);

		return WDV_False;
    }

    /* Is destination child of resource? */
    TRACE_OUT(">> Copy_Move()::Is destination child of resource?\n");
    if (Common_IsUriChild(wdv, sourceUri, destinationUri)) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_DESTINATION_IS_CHILD_OF_SOURCE,
                           WDVCAPI_ERR_TEXT_DESTINATION_IS_CHILD_OF_SOURCE);

		return WDV_False;
    }

    /* Create resource objects */
    TRACE_OUT(">> Copy_Move()::Create resource objects\n");
    if (!Resource_CreateHandle(wdv, &sourceResource)) {
        return WDV_False;
    }

    if (!Resource_CreateHandle(wdv, &destinationParentResource)) {
        Resource_DestroyHandle(wdv, sourceResource);
        return WDV_False;
    }

    /* Get source resources by uri */
    TRACE_OUT(">> Copy_Move()::Get source resources by uri\n");
    if (!Resource_GetByUri(wdv, sourceUri, sourceResource)) {
        Resource_DestroyHandle(wdv, destinationParentResource);
        Resource_DestroyHandle(wdv, sourceResource);
        return WDV_False;
    }

    /* Split destination path into parent and destination name */
    TRACE_OUT(">> Copy_Move()::SplitURI()\n");
	SplitURI(wdv, destinationUri, destinationParentUri, destinationName );

    /* Get destination's parent resource */
    if (!Resource_GetByUri(wdv, destinationParentUri, destinationParentResource)) {
        Resource_DestroyHandle(wdv, destinationParentResource);
        Resource_DestroyHandle(wdv, sourceResource);

        if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST,
                               WDVCAPI_ERR_TEXT_DESTINATION_URI_DOESNT_EXIST);
        }

        return WDV_False;
    }
    TRACE_OUT(">> Copy_Move()::Get destination's parent resource\n");

    /* Is destination resource or something in the uri already locked? */
    if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
        Resource_DestroyHandle(wdv, destinationParentResource);
        Resource_DestroyHandle(wdv, sourceResource);

        return WDV_False;
    }
    TRACE_OUT(">> Copy_Move()::Is destination resource or something in the uri already locked?\n");

    if (Resource_IsLockInURI(wdv, destinationParentResource, hLock)) {
        if (!Lock_IsLockIdInList(wdv, destinationLockIdList, destinationParentUri, hLock->id)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                    WDVCAPI_ERR_CODE_LOCK_EXISTS,
				                    WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            WDVCAPI_LockDestroyHandle(wdv, hLock);

            Resource_DestroyHandle(wdv, destinationParentResource);
            Resource_DestroyHandle(wdv, sourceResource);

            return WDV_False;
        }
    }
    TRACE_OUT(">> Copy_Move()::Resource_IsLockInURI()\n");

    WDVCAPI_LockDestroyHandle(wdv, hLock);
    TRACE_OUT(">> Copy_Move()::WDVCAPI_LockDestroyHandle()\n");

    if(move == WDV_True && strncmp(sourceUri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        /* Overwrite existing resource? */
        if (!Resource_CreateHandle(wdv, &destinationResource)) {
            Resource_DestroyHandle(wdv, sourceResource);
            Resource_DestroyHandle(wdv, destinationParentResource);

            return WDV_False;
        }

        if (Resource_GetByUri(wdv, destinationUri, destinationResource)) {
            if (destinationCreated) {
                *destinationCreated = WDV_False;
            }

            switch(overwrite) {
            case WDVCAPI_DELETE_BEFORE_OVERWRITE:
                if (!Delete_Inode(wdv, destinationResource, WDV_False)) {
                    Resource_DestroyHandle(wdv, sourceResource);
                    Resource_DestroyHandle(wdv, destinationParentResource);
                    Resource_DestroyHandle(wdv, destinationResource);

                    return WDV_False;
                }

                break;

            case WDVCAPI_NO_OVERWRITE:
		        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                        WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_ALREADY_EXIST,
                                        WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_ALREADY_EXIST);
                Resource_DestroyHandle(wdv, sourceResource);
                Resource_DestroyHandle(wdv, destinationParentResource);
                Resource_DestroyHandle(wdv, destinationResource);

                return WDV_False;

            default:
		        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                        WDVCAPI_ERR_CODE_UNDEFINED_OVERWRITE,
                                        WDVCAPI_ERR_TEXT_UNDEFINED_OVERWRITE);
                Resource_DestroyHandle(wdv, sourceResource);
                Resource_DestroyHandle(wdv, destinationParentResource);
                Resource_DestroyHandle(wdv, destinationResource);

                return WDV_False;
            }
        } else {
            if (destinationCreated) {
                *destinationCreated = WDV_True;
            }
        }

        Resource_DestroyHandle(wdv, destinationResource);

        /* Insert deleted item into destination folder */
        if (!Resource_SetParent(wdv, destinationParentResource, sourceResource, destinationName)) {
            /* Destroy resources */
            Resource_DestroyHandle(wdv, sourceResource);
            Resource_DestroyHandle(wdv, destinationParentResource);

            return WDV_False;
        }

        /* Set resource display name, if different */
        if (strcmp(sourceResource->name, destinationName) == 0) {
            if (!Property_UpdateShortValue(wdv, sourceResource->id, ID_PROPERTY_DISPLAYNAME, destinationName)) {
            }

            /* Delete property original location */
            if (!Property_DeleteProperty(wdv, sourceResource->id, ID_PROPERTY_ORIGINAL_LOCATION)) {
            }
        }
    } else {
        /* Get copy list depending of depth */
        switch(depth) {
        case WDVCAPI_DEPTH_0:
            /* Copy root resource itself */
            if (!Copy_CopyResource(wdv, sourceResource, destinationParentResource, destinationUri, destinationName, overwrite, destinationLockIdList,
                                WDV_False, WDV_False, clientErrorHandling, clientData, destinationCreated, &subCopyOk)) {
                /* Destroy resources */
                Resource_DestroyHandle(wdv, sourceResource);
                Resource_DestroyHandle(wdv, destinationParentResource);

                return WDV_False;
            }

            break;

        case WDVCAPI_DEPTH_1:
            /* Is not allowed for copy */
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_DEPTH_1_IS_NOT_ALLOWED,
                            WDVCAPI_ERR_TEXT_DEPTH_1_IS_NOT_ALLOWED);

            /* Destroy resources */
            Resource_DestroyHandle(wdv, sourceResource);
            Resource_DestroyHandle(wdv, destinationParentResource);

            return WDV_False;

            break;

        case WDVCAPI_DEPTH_INFINITE:
            /* Start subtransaction */
            WDV_StartSubTransaction(wdv);
            /* Copy or move resource recursive */
            if (!Copy_CopyResource(wdv, sourceResource, destinationParentResource, destinationUri, destinationName, overwrite, destinationLockIdList,
                                WDV_True, move, clientErrorHandling, clientData, destinationCreated, &subCopyOk)) {
                /* Destroy resources */
                Resource_DestroyHandle(wdv, sourceResource);
                Resource_DestroyHandle(wdv, destinationParentResource);

                /* Cancel subtransaction */
                WDV_CancelSubTransaction(wdv);

                return WDV_False;
            } else {
                /* If we move the resource and the sub copies were ok, then delete source resource */
                if (move == WDV_True && subCopyOk == WDV_True) {
                    if (!Delete_Delete(wdv, sourceUri, NULL, WDV_False, WDV_False)) {
                        Resource_DestroyHandle(wdv, destinationParentResource);
                        Resource_DestroyHandle(wdv, sourceResource);

		                Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                        WDVCAPI_ERR_CODE_COULD_NOT_DELETE_MOVED_RESOURCE_OR_COLLECTION,
                                        WDVCAPI_ERR_TEXT_COULD_NOT_DELETE_MOVED_RESOURCE_OR_COLLECTION);

                        /* Cancel subtransaction */
                        WDV_CancelSubTransaction(wdv);

                        return WDV_False;
                    }
                }

                /* End subtransaction */
                WDV_EndSubTransaction(wdv);
            }

            break;

        default:
            /* Is not allowed for copy */
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_WRONG_DEPTH,
                            WDVCAPI_ERR_TEXT_WRONG_DEPTH);

            /* Destroy resources */
            Resource_DestroyHandle(wdv, sourceResource);
            Resource_DestroyHandle(wdv, destinationParentResource);

            return WDV_False;

            break;
        }
    }

    /* Destroy resources */
    Resource_DestroyHandle(wdv, sourceResource);
    Resource_DestroyHandle(wdv, destinationParentResource);

    TRACE_OUT("<< Copy_Move()\n");

    return WDV_True;

}

/*===========================================================================*/

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Copy_CopyResource( WDVCAPI_WDV                  wdv,
                            WDVCAPI_Resource             resource,
                            WDVCAPI_Resource             destinationParentResource,
                            WDVCAPI_URI                  destinationUri,
                            WDVCAPI_ResourceName         destinationName,
                            WDVCAPI_Overwrite            overwrite,
                            WDVCAPI_LockIdList           destinationLockIdList,
                            WDV_Bool                     recursive,
                            WDV_Bool                     move,
                            WDVCAPI_ClientErrorHandling  clientErrorHandling,
                            void                        *clientData,
                            WDV_Bool                    *destinationCreated,
                            WDV_Bool                    *copyOk )
{

    WDV_Bool                            childFound = WDV_False;
    WDVCAPI_Resource                    childResource = NULL;
    WDVCAPI_ResourceName                childResourceName;
    WDVCAPI_Resource                    newDestinationResource = NULL;
    WDV_Bool                            resourceAlreadyExists = WDV_True;
    WDVCAPI_ErrorItem                   lastError = NULL;
    WDVCAPI_URIString                   currentUri = "";
    WDV_Bool                            subCopyOk;
    WDVCAPI_LockHandle                  hLock = NULL;
    WDVCAPI_PropertyShortValueBuffer    propertyShortValue = "";

    TRACE_OUT(">> Copy_CopyResource()\n");

    if (!destinationCreated || !copyOk) {
        return WDV_False;
    }

    /* Create new destination resource handle */
    if (!Resource_CreateHandle(wdv, &newDestinationResource)) {
        return WDV_False;
    }

    /* Create new inode, if necessary */
    if (!Resource_CreateInode(wdv, destinationParentResource, destinationName, WDVCAPI_RESOURCE_STATE_DEFAULT,
                              resource->compressedLength, overwrite, newDestinationResource,
                             &resourceAlreadyExists, WDV_True)) {
        Resource_DestroyHandle(wdv, newDestinationResource);

        if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_ALREADY_EXIST)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT,
                               WDVCAPI_ERR_TEXT_OVERWRITE_CONFLICT);
        }

        return WDV_False;
    }

    /* If resource already exists, check for locks */
    if (resourceAlreadyExists == WDV_True) {
        /* Is resource or something in the uri already locked? */
        if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
            return WDV_False;
        }

        /* Resource_CreateInode returns only lock description for the current resource */
        /* But this is enough, because we create the destination tree step by step and */
        /* check the locks for every step. If there was a lock collision we wouldn't   */
        /* be here                                                                     */
        if (Resource_IsLockInURI(wdv, newDestinationResource, hLock)) {
            if (!Lock_IsLockIdInList(wdv, destinationLockIdList, destinationUri, hLock->id)) {
		        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                        WDVCAPI_ERR_CODE_LOCK_EXISTS,
				                        WDVCAPI_ERR_TEXT_LOCK_EXISTS);

                WDVCAPI_LockDestroyHandle(wdv, hLock);

                Resource_DestroyHandle(wdv, newDestinationResource);

                return WDV_False;
            }
        }

        WDVCAPI_LockDestroyHandle(wdv, hLock);
    }

    /* Get resource type of the source */
    if (!Property_GetShortValue(wdv, resource->id, ID_PROPERTY_RESOURCE_TYPE, propertyShortValue)) {
        Resource_DestroyHandle(wdv, newDestinationResource);

        return WDV_False;
    }

    /* If source is not a collection copy container */
    if (strcmp(propertyShortValue, PROPERTY_VALUE_COLLECTION) != 0) {
        /* Copy container */
        if (!Resource_CopyContainer(wdv, resource, newDestinationResource)) {
            Resource_DestroyHandle(wdv, newDestinationResource);

            return WDV_False;
        }
    }

    /* Copy properties */
    if (move == WDV_True) {
        if (!Resource_CopyProperties(wdv, resource, newDestinationResource, WDV_False)) {
            Resource_DestroyHandle(wdv, newDestinationResource);

            return WDV_False;
        }
    } else {
        if (!Resource_CopyProperties(wdv, resource, newDestinationResource, WDV_False)) {
            Resource_DestroyHandle(wdv, newDestinationResource);

            return WDV_False;
        }
    }

    /* Is that all or do we have to copy the childs too? */
    if (recursive == WDV_True) {
        /* Create child resource handle */
        if (!Resource_CreateHandle(wdv, &childResource)) {
            Resource_DestroyHandle(wdv, newDestinationResource);

            return WDV_False;
        }

        /* Collect the childs of the resource */
        if (!Resource_CollectChilds(wdv, resource, childResource, &childFound)) {
            Resource_DestroyHandle(wdv, childResource);
            Resource_DestroyHandle(wdv, newDestinationResource);

            return WDV_False;
        }

        /* Copy all childs */
        while (childFound) {
            /* Get next resource's name */
            if (!Resource_GetName(childResource, &childResourceName)) {
                Resource_DestroyHandle(wdv, childResource);
                Resource_DestroyHandle(wdv, newDestinationResource);

                return WDV_False;
            }

            /* Get uri of destination child resource */
            sp77sprintf(currentUri, WDV_MAX_URI_LEN, "%s%s%s", destinationUri, WDV_PATH_SEPARATOR_STR, childResourceName);

            subCopyOk = WDV_True;

            /* Start subtransaction */
            WDV_StartSubTransaction(wdv);

            /* Copy resource */
            if (!Copy_CopyResource(wdv, childResource, newDestinationResource, currentUri, childResourceName, overwrite, destinationLockIdList,
                                   recursive, move, clientErrorHandling, clientData, destinationCreated, &subCopyOk)) {
                /* Cancel subtransaction */
                WDV_CancelSubTransaction(wdv);

                Resource_DestroyHandle(wdv, childResource);
                Resource_DestroyHandle(wdv, newDestinationResource);

                *copyOk = WDV_False;

                /* Get last error */
                if (WDVCAPI_GetLastError(wdv, &lastError)) {
                    /* Get uri of child resource */
                    Resource_GetUriByChild(wdv, childResource, currentUri);

                    /* Call client callback function for error handling */
                    clientErrorHandling(clientData, currentUri, lastError);
                } else {
                    return WDV_False;
                }
            } else {
                /* If we move the resource and the sub copies were ok, then delete source resource */
                if (move == WDV_True && subCopyOk == WDV_True) {
                    /* Get uri of failed resource */
                    Resource_GetUriByChild(wdv, childResource, currentUri);

                    if (!Delete_Delete(wdv, currentUri, NULL, WDV_False, WDV_False)) {
                        /* Cancel subtransaction */
                        WDV_CancelSubTransaction(wdv);

                        *copyOk = WDV_False;

                        /* Get last error */
                        if (WDVCAPI_GetLastError(wdv, &lastError)) {
                            /* Call client callback function for error handling */
                            clientErrorHandling(clientData, currentUri, lastError);
                        } else {
                            return WDV_False;
                        }
                    } else {
                        /* End subtransaction */
                        WDV_EndSubTransaction(wdv);
                    }
                } else {
                    /* End subtransaction */
                    WDV_EndSubTransaction(wdv);
                }
            }

            /* Get next child resource to copy */
            if (!Resource_GetNextFromCollection(wdv, resource, childResource, &childFound)) {
                Resource_DestroyHandle(wdv, childResource);
                Resource_DestroyHandle(wdv, newDestinationResource);

                return WDV_False;
            }
        }

        /* Destroy child resource handle */
        Resource_DestroyHandle(wdv, childResource);
    }

    /* Destroy new destination resource handle */
    Resource_DestroyHandle(wdv, newDestinationResource);

    /* Resource has been created? */
    if (resourceAlreadyExists == WDV_False) {
        *destinationCreated = WDV_True;
    } else {
        *destinationCreated = WDV_False;
    }

    TRACE_OUT("<< Copy_CopyResource()\n");

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
