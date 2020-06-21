/*!**********************************************************************

  module: WDVCAPI_Proppatch.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Property Patch

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_DELETE_PROPERTY                                     \
    "DELETE WEBDAV_Property WHERE CId = X'%s' AND Property_Id =        \
     (SELECT PM.Id FROM WEBDAV_Property_Management PM, WEBDAV_Name_Space NS   \
      WHERE NS.Name_Space = '%s' AND PM.Name_Prefix = '%s' AND PM.Name_Space_Id = NS.Id )"

#define SQL_SET_PROPERTY                                                    \
    "INSERT WEBDAV_Property (CId, Property_Id, Property_Short_Value)               \
     SELECT X'%s', PM.Id, '%s' FROM WEBDAV_Property_Management PM, WEBDAV_Name_Space NS   \
     WHERE NS.Name_Space = '%s' AND PM.Name_Prefix = '%s' AND PM.Name_Space_Id = NS.Id"

#define SQL_UPDATE_PROPERTY \
    "UPDATE WEBDAV_Property SET Property_Short_Value = '%s' WHERE CId = X'%s' AND Property_Id = X'%s'"

#define SQL_GET_DOC_CLASS_PROPERTIES \
    "SELECT DC.Property_Short_Value, IDX.Property_Short_Value \
     FROM WebDAV_Property DC, WebDAV_Property IDX \
     WHERE DC.CId = ? AND DC.Property_Id = x'00000000000000000000000000000000000000000000000D' \
     AND   IDX.CId = DC.CId AND IDX.Property_Id = x'00000000000000000000000000000000000000000000000E' \
     WITH LOCK EXCLUSIVE"

#define SQL_UPDATE_DOC_CLASS \
    "UPDATE WebDAV_Property \
     SET Property_Short_Value = ? \
     WHERE Property_Id = x'00000000000000000000000000000000000000000000000D' \
     AND CId = ?"

#define SQL_UPDATE_INDEXING_STATE \
    "UPDATE WebDAV_Property \
     SET Property_Short_Value = '"XMLINDEXING_STATE_NEW"' \
     WHERE Property_Id = x'00000000000000000000000000000000000000000000000E' \
     AND CId = ?"

#define SQL_INSERT_DOC_CLASS \
    "INSERT INTO WebDAV_Property \
     SET Property_Short_Value = ?, \
         Property_Id = x'00000000000000000000000000000000000000000000000D', \
         CId = ?"

#define SQL_INSERT_INDEXING_STATE \
    "INSERT INTO WebDAV_Property \
     SET Property_Short_Value = '"XMLINDEXING_STATE_NEW"', \
         Property_Id = x'00000000000000000000000000000000000000000000000E', \
         CId = ?"

#define SQL_DELETE_INDEX_VALUE_PROPS \
    "DELETE Webdav_Property " \
    "WHERE CId = ? " \
    "  AND Property_Id IN (SELECT PM.Id " \
    "                      FROM " \
    "                        WebDAV_Name_Space NS, WebDAV_Property_Management PM " \
    "                      WHERE " \
    "                            NS.Name_SPACE = 'SAPDB' " \
    "                        AND NS.Id = PM.Name_Space_Id " \
    "                        AND PM.Name_Prefix IN (SELECT Idx.Name " \
    "                                               FROM " \
    "                                                 XML_ASSIGN_DC_IDX DC, XML_XMLIndex Idx " \
    "                                               WHERE " \
    "                                                 DC.IdxId = Idx.IdxId " \
    "                                               AND DC.DCId = ? " \
    "                                              ) " \
    "                     ) " \
    "  OR (    CId = ? " \
    "      AND Property_Id IN (x'00000000000000000000000000000000000000000000000D', " \
    "                          x'00000000000000000000000000000000000000000000000E' " \
    "                         )" \
    "     )"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_proppatch_name_space          *ProppatchNameSpace;
typedef struct st_proppatch_processed_property  *ProppatchProcessedProperty;

struct st_proppatch_name_space {
    WDVCAPI_NameSpaceShortcutString nameSpaceShortcut;
    WDVCAPI_NameSpaceString         nameSpace;
    ProppatchNameSpace              nextNameSpace;
};

struct st_proppatch_processed_property {
    WDVCAPI_NameSpaceShortcut       nameSpaceShortcut;
    WDVCAPI_PropertyNameString      propertyName;
    WDVCAPI_ProppatchAccessType     proppatchAccessType;
    WDVCAPI_PropertyStatus          propertyStatus;
    WDVCAPI_ErrorItem               error;
    ProppatchProcessedProperty      nextProcessedProperty;
};

struct st_wdvcapi_proppatch_handle {

    WDVCAPI_URIString                   uri;
    WDVCAPI_IdString                    uriId;
    WDVCAPI_NameSpaceShortcutCounter    nameSpaceShortcutCounter;
    ProppatchNameSpace                  nameSpaceList;
    ProppatchNameSpace                  nextNameSpace;
    ProppatchNameSpace                  lastNameSpace;
    ProppatchProcessedProperty          processedPropertyList;
    ProppatchProcessedProperty          nextProcessedProperty;
    ProppatchProcessedProperty          lastProcessedProperty;

};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Proppatch_InitHandle( WDVCAPI_WDV               wdv,
                               WDVCAPI_ProppatchHandle   hProppatch,
                               WDVCAPI_URI               uri );

WDV_Bool Proppatch_RemoveProperty( WDVCAPI_WDV              wdv,
                                   WDVCAPI_ProppatchHandle  hProppatch,
                                   WDVCAPI_NameSpace        nameSpace,
                                   WDVCAPI_PropertyName     propertyName );

WDV_Bool Proppatch_SetPropertyShortValue( WDVCAPI_WDV                   wdv,
                                          WDVCAPI_ProppatchHandle       hProppatch,
                                          WDVCAPI_NameSpace             nameSpace,
                                          WDVCAPI_PropertyName          propertyName,
                                          WDVCAPI_PropertyShortValue    propertyShortValue );

WDV_Bool Proppatch_AddToProcessedList( WDVCAPI_WDV                  wdv,
                                       WDVCAPI_ProppatchHandle      hProppatch,
                                       WDVCAPI_NameSpace            nameSpace,
                                       WDVCAPI_PropertyName         propertyName,
                                       WDVCAPI_ProppatchAccessType  proppatchAccessType,
                                       WDVCAPI_ErrorItem            errorItem );

WDV_Bool Proppatch_InitProcessedProperty( WDVCAPI_WDV                   wdv,
                                          WDVCAPI_ProppatchHandle       hProppatch,
                                          ProppatchProcessedProperty    newProcessedProperty,
                                          WDVCAPI_NameSpace             nameSpace,
                                          WDVCAPI_PropertyName          propertyName,
                                          WDVCAPI_ProppatchAccessType   proppatchAccessType,
                                          WDVCAPI_ErrorItem             errorItem );

WDV_Bool Proppatch_AddToNameSpaceList( WDVCAPI_WDV                  wdv,
                                       WDVCAPI_ProppatchHandle      hProppatch,
                                       WDVCAPI_NameSpace            nameSpace,
                                       WDVCAPI_NameSpaceShortcut   *nameSpaceShortcut );

WDV_Bool Proppatch_SearchNameSpaceInList( WDVCAPI_WDV                wdv,
                                          ProppatchNameSpace         nameSpaceList,
                                          WDVCAPI_NameSpace          nameSpaceToFind,
                                          WDVCAPI_NameSpaceShortcut *foundNameSpaceShortcut );

WDV_Bool Proppatch_InitNameSpace( WDVCAPI_WDV                wdv,
                                  WDVCAPI_ProppatchHandle    hProppatch,
                                  ProppatchNameSpace         newNameSpace,
                                  WDVCAPI_NameSpace          nameSpace,
                                  WDVCAPI_NameSpaceShortcut *nameSpaceShortcut );

WDV_Bool Proppatch_DestroyNameSpaceList( WDVCAPI_WDV        wdv,
                                         ProppatchNameSpace nameSpaceList);

WDV_Bool Proppatch_DestroyProcessedPropertyList( WDVCAPI_WDV                wdv,
                                                 ProppatchProcessedProperty processedPropertyList);

WDV_Bool Proppatch_GetDocClassProperties( WDVCAPI_WDV   wdv,
                                          WDVCAPI_URI   uri,
                                          WDV_Bool      withLockCheck );

WDV_Bool Proppatch_UpdateDocClassProperties( WDVCAPI_WDV                wdv,
                                             WDVCAPI_PropertyShortValue docClassId );

WDV_Bool Proppatch_InsertDocClassProperties( WDVCAPI_WDV                wdv,
                                             WDVCAPI_PropertyShortValue docClassId );

WDV_Bool Proppatch_DeleteIndexValues( WDVCAPI_WDV   wdv );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_ProppatchCreate( WDVCAPI_WDV			    wdv,
                                  WDVCAPI_URI               uri,
                                  WDVCAPI_LockIdString      lockIdString,
                                  WDVCAPI_ProppatchHandle  *hProppatch )
{

	WDV_Bool                ok = WDV_False;
    WDVCAPI_ProppatchHandle newProppatch = NULL;
    WDVCAPI_Resource        resource = NULL;
    WDVCAPI_LockId          lockId;
    WDVCAPI_LockHandle      hLock = NULL;

	if (!wdv || !hProppatch) {
		return WDV_False;
	}

    /* Is uri the deleted items folder? */
    if(strncmp(uri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
                                WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER);

        return WDV_False;
    }

    /* Inits */
    *hProppatch = NULL;

    if (!lockIdString) {
        WDVCAPI_IdInitValue(lockId);
    } else {
        WDVCAPI_IdStringAsId(lockIdString, lockId);
    }
    
    /* Allocat memory for the proppatch handle */
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_proppatch_handle), (WDV_UInt1**) &newProppatch, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

    /* Start transaction */
    WDV_StartTransaction(wdv);

	/* Init proppatch handle */
    if (!Proppatch_InitHandle(wdv, newProppatch, uri)) {
        WDVCAPI_Free((WDV_UInt1*) newProppatch );

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Check uri              */
    /* Create resource handle */
    if (!Resource_CreateHandle(wdv, &resource)) {
        WDVCAPI_Free((WDV_UInt1*) newProppatch );

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get resource by uri */
    if (!Resource_GetByUri(wdv, newProppatch->uri, resource)) {
        Resource_DestroyHandle(wdv, resource);
        WDVCAPI_Free((WDV_UInt1*) newProppatch );

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Is something locked in URI? */
    if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
	    Resource_DestroyHandle(wdv, resource);
        WDVCAPI_Free((WDV_UInt1*) newProppatch );

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
            WDVCAPI_Free((WDV_UInt1*) newProppatch );

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    WDVCAPI_LockDestroyHandle(wdv, hLock);

    /* Get id as string */
	Resource_GetIdAsString(resource, newProppatch->uriId);

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, resource);

    /* Set return value */
    *hProppatch = newProppatch;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchDestroy( WDVCAPI_WDV              wdv,
                                   WDVCAPI_ProppatchHandle  hProppatch )
{

    /* Destroy processed property list */
    Proppatch_DestroyProcessedPropertyList(wdv, hProppatch->processedPropertyList);

    /* Destroy name space list */
    Proppatch_DestroyNameSpaceList(wdv, hProppatch->nameSpaceList);
    
    /* Free proppatch handle itself */
    WDVCAPI_Free((WDV_UInt1*) hProppatch );

    /* End transaction */
    WDV_EndTransaction(wdv);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchRemove( WDVCAPI_WDV               wdv,
                                  WDVCAPI_ProppatchHandle   hProppatch,
                                  WDVCAPI_NameSpace         nameSpace,
                                  WDVCAPI_PropertyName      propertyName,
                                  WDVCAPI_ErrorItem         errorItem )
{

    WDV_Bool            rc;
    WDVCAPI_ErrorItem   myErrorItem = NULL;

    /* Remove property */
    if (!errorItem) {
        if ((rc = Proppatch_RemoveProperty(wdv, hProppatch, nameSpace, propertyName)) == WDV_False) {
            WDVCAPI_GetLastError(wdv, &myErrorItem);
            errorItem = myErrorItem;
        }
    } else {
        rc = WDV_False;

        /* Add provided error item to the wdv error list */
        if (!AddErrorItemToList(wdv, errorItem)) {
            return WDV_False;
        }
    }

    /* Insert property in processed list */
    if (!Proppatch_AddToProcessedList(wdv, hProppatch, nameSpace, propertyName,
                                      WDVCAPI_PROPPATCH_ACCESS_TYPE_REMOVE, errorItem)) {
        return WDV_False;
    }

    return rc;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchSetShortValue( WDVCAPI_WDV                wdv,
                                         WDVCAPI_ProppatchHandle    hProppatch,
                                         WDVCAPI_NameSpace          nameSpace,
                                         WDVCAPI_PropertyName       propertyName,
                                         WDVCAPI_PropertyShortValue propertyShortValue,
                                         WDVCAPI_ErrorItem          errorItem )
{

    WDV_Bool            rc;
    WDVCAPI_ErrorItem   myErrorItem = NULL;

    /* Set property with short value */
    if (!errorItem) {
        if ((rc = Proppatch_SetPropertyShortValue(wdv, hProppatch, nameSpace, propertyName, propertyShortValue)) == WDV_False) {
            WDVCAPI_GetLastError(wdv, &myErrorItem);
            errorItem = myErrorItem;
        }
    } else {
        rc = WDV_False;

        /* Add provided error item to the wdv error list */
        if (!AddErrorItemToList(wdv, errorItem)) {
            return WDV_False;
        }
    }

    /* Insert property in processed list */
    if (!Proppatch_AddToProcessedList(wdv, hProppatch, nameSpace, propertyName,
                                      WDVCAPI_PROPPATCH_ACCESS_TYPE_SET, errorItem)) {
        return WDV_False;
    }

    return rc;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchSetDocClass( WDVCAPI_WDV                  wdv,
                                       WDVCAPI_URI                  uri,
                                       WDVCAPI_PropertyShortValue   docClassId )
{

    WDVCAPI_PropertyShortValueBuffer    currentDocClassId;
    WDVCAPI_PropertyShortValueBuffer    currentIndexingState;

    /* Is uri the deleted items folder? */
    if(strncmp(uri, WDVCAPI_DELETED_ITEMS_FOLDER, WDVCAPI_DELETED_ITEMS_FOLDER_LEN) == 0) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
                                WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER);

        return WDV_False;
    }

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Doc class property already exist? */
    if (Proppatch_GetDocClassProperties(wdv, uri, WDV_True)) {
        /* Check indexing state */
        if (strcmp(wdv->dcMgtIndexingState, XMLINDEXING_STATE_IN_PROCESS_AS_STRING) == 0) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            /* Update of doc class not possible yet */
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                    WDVCAPI_ERR_CODE_INDEXING_STATE_NOT_APPROPRIATE,
				                    WDVCAPI_ERR_TEXT_INDEXING_STATE_NOT_APPROPRIATE);

            return WDV_False;
        }

        /* Delete index values */
        if (!Proppatch_DeleteIndexValues(wdv)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Insert doc class and indexing state property */
        if (!Proppatch_InsertDocClassProperties(wdv, docClassId)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    } else {
        /* Properties not there? */
        if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_NO_DOC_CLASS_PROPERTY)) {
            /* Insert doc class and indexing state property */
            if (!Proppatch_InsertDocClassProperties(wdv, docClassId)) {
                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        } else {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    /* End transaction */
    WDV_EndTransaction(wdv);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchGetDocClassProperties( WDVCAPI_WDV                wdv,
                                                 WDVCAPI_URI                uri,
                                                 WDVCAPI_PropertyShortValue docClassId,
                                                 WDVCAPI_PropertyShortValue indexingState )
{

    WDVCAPI_Resource    resource = NULL;

    /* Checks */
    if (!wdv || !uri || !docClassId || !indexingState) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
				                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

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

    /* Get properties */
    if (!Proppatch_GetDocClassProperties(wdv, uri, WDV_False)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    strcpy(docClassId, wdv->dcMgtDocClassId);

    if (strcmp(wdv->dcMgtIndexingState, XMLINDEXING_STATE_NEW) == 0) {
        strcpy(indexingState, XMLINDEXING_STATE_NEW_AS_STRING);
    } else if (strcmp(wdv->dcMgtIndexingState, XMLINDEXING_STATE_IN_PROCESS) == 0) {
        strcpy(indexingState, XMLINDEXING_STATE_IN_PROCESS_AS_STRING);
    } else if (strcmp(wdv->dcMgtIndexingState, XMLINDEXING_STATE_FINISHED) == 0) {
        strcpy(indexingState, XMLINDEXING_STATE_FINISHED_AS_STRING);
    } else {
        strcpy(indexingState, "");
    }

    /* Cancel transaction */
    WDV_CancelTransaction(wdv);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchGetFirstProcessedNameSpace( WDVCAPI_WDV                wdv,
                                                      WDVCAPI_ProppatchHandle    hProppatch,
                                                      WDVCAPI_NameSpaceShortcut *nameSpaceShortcut,
                                                      WDVCAPI_NameSpace         *nameSpace )
{

    /* Checks */
    if (!wdv || !hProppatch) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

    if (nameSpaceShortcut) {
        if (hProppatch->nameSpaceList) {
            *nameSpaceShortcut = hProppatch->nameSpaceList->nameSpaceShortcut;
        } else {
            *nameSpaceShortcut = NULL;
        }
    }

    if (nameSpace) {
        if (hProppatch->nameSpaceList) {
            *nameSpace = hProppatch->nameSpaceList->nameSpace;
        } else {
            *nameSpace = NULL;
        }
    }

    /* Set next list item */
    if (hProppatch->nameSpaceList) {
        hProppatch->nextNameSpace = hProppatch->nameSpaceList->nextNameSpace;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchGetNextProcessedNameSpace( WDVCAPI_WDV                wdv,
                                                     WDVCAPI_ProppatchHandle    hProppatch,
                                                     WDVCAPI_NameSpaceShortcut *nameSpaceShortcut,
                                                     WDVCAPI_NameSpace         *nameSpace )
{

    /* Checks */
    if (!wdv || !hProppatch) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

    if (nameSpaceShortcut) {
        if (hProppatch->nextNameSpace) {
            *nameSpaceShortcut = hProppatch->nextNameSpace->nameSpaceShortcut;
        } else {
            *nameSpaceShortcut = NULL;
        }
    }

    if (nameSpace) {
        if (hProppatch->nextNameSpace) {
            *nameSpace = hProppatch->nextNameSpace->nameSpace;
        } else {
            *nameSpace = NULL;
        }
    }

    /* Set next list item */
    if (hProppatch->nextNameSpace) {
        hProppatch->nextNameSpace = hProppatch->nextNameSpace->nextNameSpace;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchGetFirstProcessedProperty( WDVCAPI_WDV                     wdv,
                                                     WDVCAPI_ProppatchHandle         hProppatch,
                                                     WDVCAPI_NameSpaceShortcut      *nameSpaceShortcut,
                                                     WDVCAPI_PropertyName           *propertyName,
                                                     WDVCAPI_ProppatchAccessType    *proppatchAccessType,
                                                     WDVCAPI_PropertyStatus         *propertyStatus,
                                                     WDVCAPI_ErrorItem              *error )
{

    /* Checks */
    if (!wdv || !hProppatch) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

    if (nameSpaceShortcut) {
        if (hProppatch->processedPropertyList) {
            *nameSpaceShortcut = hProppatch->processedPropertyList->nameSpaceShortcut;
        } else {
            *nameSpaceShortcut = NULL;
        }
    }

    if (propertyName) {
        if (hProppatch->processedPropertyList) {
            *propertyName = hProppatch->processedPropertyList->propertyName;
        } else {
            *propertyName = NULL;
        }
    }

    if (propertyStatus) {
        if (hProppatch->processedPropertyList) {
            *propertyStatus = hProppatch->processedPropertyList->propertyStatus;
        } else {
            *propertyStatus = WDVCAPI_PROPERTY_STATUS_UNDEFINED;
        }
    }

    if (proppatchAccessType) {
        if (hProppatch->processedPropertyList) {
            *proppatchAccessType = hProppatch->processedPropertyList->proppatchAccessType;
        } else {
            *proppatchAccessType = WDVCAPI_PROPPATCH_ACCESS_TYPE_UNDEFINED;
        }
    }

    if (error) {
        if (hProppatch->processedPropertyList) {
            *error = hProppatch->processedPropertyList->error;
        } else {
            *error = NULL;
        }
    }

    /* Set next list item */
    if (hProppatch->processedPropertyList) {
        hProppatch->nextProcessedProperty = hProppatch->processedPropertyList->nextProcessedProperty;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_ProppatchGetNextProcessedProperty( WDVCAPI_WDV                     wdv,
                                                    WDVCAPI_ProppatchHandle         hProppatch,
                                                    WDVCAPI_NameSpaceShortcut      *nameSpaceShortcut,
                                                    WDVCAPI_PropertyName           *propertyName,
                                                    WDVCAPI_ProppatchAccessType    *proppatchAccessType,
                                                    WDVCAPI_PropertyStatus         *propertyStatus,
                                                    WDVCAPI_ErrorItem              *error )
{

    /* Checks */
    if (!wdv || !hProppatch) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

    if (nameSpaceShortcut) {
        if (hProppatch->nextProcessedProperty) {
            *nameSpaceShortcut = hProppatch->nextProcessedProperty->nameSpaceShortcut;
        } else {
            *nameSpaceShortcut = NULL;
        }
    }

    if (propertyName) {
        if (hProppatch->nextProcessedProperty) {
            *propertyName = hProppatch->nextProcessedProperty->propertyName;
        } else {
            *propertyName = NULL;
        }
    }

    if (proppatchAccessType) {
        if (hProppatch->nextProcessedProperty) {
            *proppatchAccessType = hProppatch->nextProcessedProperty->proppatchAccessType;
        } else {
            *proppatchAccessType = WDVCAPI_PROPPATCH_ACCESS_TYPE_UNDEFINED;
        }
    }

    if (propertyStatus) {
        if (hProppatch->nextProcessedProperty) {
            *propertyStatus = hProppatch->nextProcessedProperty->propertyStatus;
        } else {
            *propertyStatus = WDVCAPI_PROPERTY_STATUS_UNDEFINED;
        }
    }

    if (error) {
        if (hProppatch->nextProcessedProperty) {
            *error = hProppatch->nextProcessedProperty->error;
        } else {
            *error = NULL;
        }
    }

    /* Set next list item */
    if (hProppatch->nextProcessedProperty) {
        hProppatch->nextProcessedProperty = hProppatch->nextProcessedProperty->nextProcessedProperty;
    }

    return WDV_True;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Proppatch_RemoveProperty( WDVCAPI_WDV              wdv,
                                   WDVCAPI_ProppatchHandle  hProppatch,
                                   WDVCAPI_NameSpace        nameSpace,
                                   WDVCAPI_PropertyName     propertyName )
{

	SQLRETURN           sqlRC = 0;
    SQLHDBC             hDBC = 0;
	SQLHSTMT            hStmt = 0;
	SQLCHAR             sqlStmt [1024] = "";
    WDVCAPI_Resource    resource = NULL;

    /* Checks */
    if (!wdv || !hProppatch || !nameSpace || !propertyName) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

	/* Build SQL statement */
	sp77sprintf(sqlStmt, 1024, SQL_DELETE_PROPERTY, hProppatch->uriId, nameSpace, propertyName);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

        if (!WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED) &&
            !WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED)) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_COULD_NOT_REMOVE_PROPERTY,
                                WDVCAPI_ERR_TEXT_COULD_NOT_REMOVE_PROPERTY);

    		return WDV_False;
        }
	}

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_SetPropertyShortValue( WDVCAPI_WDV                   wdv,
                                          WDVCAPI_ProppatchHandle       hProppatch,
                                          WDVCAPI_NameSpace             nameSpace,
                                          WDVCAPI_PropertyName          propertyName,
                                          WDVCAPI_PropertyShortValue    propertyShortValue )
{

	SQLRETURN           sqlRC = 0;
    SQLHDBC             hDBC = 0;
	SQLHSTMT            hStmt = 0;
	SQLCHAR             sqlStmt [1024] = "";
    WDV_Bool            rc = WDV_False;
    WDVCAPI_IdString    propertyIdString = "";

    /* Checks */
    if (!wdv || !hProppatch || !nameSpace || !propertyName) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

	/* Build SQL statement */
    if (strcmp(nameSpace, WDV_NAME_SPACE_DAV) == 0 &&
        (strcmp(propertyName, WDV_PROPERTY_NAME_DISPLAYNAME) == 0 || strcmp(propertyName, WDV_PROPERTY_NAME_GETCONTENTTYPE) == 0)) {

        if (strcmp(propertyName, WDV_PROPERTY_NAME_DISPLAYNAME) == 0) {
            strcpy(propertyIdString, ID_PROPERTY_DISPLAYNAME_STRING);
        } else if (strcmp(propertyName, WDV_PROPERTY_NAME_GETCONTENTTYPE) == 0) {
            strcpy(propertyIdString, ID_PROPERTY_RESOURCE_TYPE_STRING);
        }

	    sp77sprintf(sqlStmt, 1024, SQL_UPDATE_PROPERTY, propertyShortValue, hProppatch->uriId, propertyIdString);
    } else {
	    sp77sprintf(sqlStmt, 1024, SQL_SET_PROPERTY, hProppatch->uriId, propertyShortValue, nameSpace, propertyName);
    }

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

        if (WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED)) {
            /* Maybe the property is unknown in system? */
            if (Property_Create(wdv, nameSpace, propertyName)) {
                /* Try this proppatch again */
	            sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	            if ( sqlRC != 0 ) {
                    Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
                } else {
                    rc = WDV_True;
                }
            }

            if (!rc) {
		        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_COULD_NOT_SET_PROPERTY,
                                    WDVCAPI_ERR_TEXT_COULD_NOT_SET_PROPERTY);
            }
        }

        if (!rc) {
	        /* Free statement handle */
	        sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
        }
	}

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_AddToProcessedList( WDVCAPI_WDV                  wdv,
                                       WDVCAPI_ProppatchHandle      hProppatch,
                                       WDVCAPI_NameSpace            nameSpace,
                                       WDVCAPI_PropertyName         propertyName,
                                       WDVCAPI_ProppatchAccessType  proppatchAccessType,
                                       WDVCAPI_ErrorItem            errorItem )
{

	WDV_Bool                    ok = WDV_False;
    ProppatchProcessedProperty  newProcessedProperty = NULL;

	if (!wdv || !hProppatch || !nameSpace || !propertyName) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_proppatch_processed_property),
                (WDV_UInt1**) &newProcessedProperty, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init new processed property */
	if (!Proppatch_InitProcessedProperty(wdv, hProppatch, newProcessedProperty,
                                         nameSpace, propertyName, proppatchAccessType, errorItem)) {
        /* Free new processed property */
        WDVCAPI_Free((WDV_UInt1*) newProcessedProperty);

        return WDV_False;
    }

    /* Do we have an initial property? */
    if (!hProppatch->processedPropertyList) {
        hProppatch->processedPropertyList = newProcessedProperty;
    } else {
        hProppatch->lastProcessedProperty->nextProcessedProperty = newProcessedProperty;
    }

    /* Store last processed property */
    hProppatch->lastProcessedProperty = newProcessedProperty;

	return WDV_True;


}

/*===========================================================================*/

WDV_Bool Proppatch_InitProcessedProperty( WDVCAPI_WDV                   wdv,
                                          WDVCAPI_ProppatchHandle       hProppatch,
                                          ProppatchProcessedProperty    newProcessedProperty,
                                          WDVCAPI_NameSpace             nameSpace,
                                          WDVCAPI_PropertyName          propertyName,
                                          WDVCAPI_ProppatchAccessType   proppatchAccessType,
                                          WDVCAPI_ErrorItem             errorItem )
{

	if (!wdv || !newProcessedProperty) {
		return WDV_False;
	}

    /* Initialize new processed property */
    if (!Proppatch_AddToNameSpaceList(wdv, hProppatch, nameSpace, &newProcessedProperty->nameSpaceShortcut)) {
        return WDV_False;
    }

    strncpy(newProcessedProperty->propertyName, propertyName, WDV_MAX_PROPERTY_NAME_PREFIX_LEN);
    newProcessedProperty->propertyName[WDV_MAX_PROPERTY_NAME_PREFIX_LEN] = '\0';

    newProcessedProperty->proppatchAccessType = proppatchAccessType;

    if (errorItem) {
        newProcessedProperty->propertyStatus = WDVCAPI_PROPERTY_STATUS_FAILED;
        newProcessedProperty->error = errorItem;
    } else {
        newProcessedProperty->propertyStatus = WDVCAPI_PROPERTY_STATUS_OK;
        newProcessedProperty->error = NULL;
    }

    newProcessedProperty->nextProcessedProperty = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_AddToNameSpaceList( WDVCAPI_WDV                  wdv,
                                       WDVCAPI_ProppatchHandle      hProppatch,
                                       WDVCAPI_NameSpace            nameSpace,
                                       WDVCAPI_NameSpaceShortcut   *nameSpaceShortcut )
{

	WDV_Bool                    ok = WDV_False;
    WDVCAPI_NameSpaceShortcut   foundNameSpaceShortcut = NULL;
    ProppatchNameSpace          newNameSpace = NULL;

	if (!wdv || !hProppatch || !nameSpace) {
		return WDV_False;
	}

    /* Name space already exists? */
    if (!Proppatch_SearchNameSpaceInList(wdv, hProppatch->nameSpaceList, nameSpace, &foundNameSpaceShortcut)) {
        return WDV_False;
    }
    if (foundNameSpaceShortcut) {
        *nameSpaceShortcut = foundNameSpaceShortcut;
        return WDV_True;
    }
    
	WDVCAPI_Allocat( sizeof(struct st_proppatch_name_space),
                (WDV_UInt1**) &newNameSpace, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init new name space */
	if (!Proppatch_InitNameSpace(wdv, hProppatch, newNameSpace, nameSpace, nameSpaceShortcut)) {
        /* Free new name space */
        WDVCAPI_Free((WDV_UInt1*) newNameSpace);

        return WDV_False;
    }

    /* Do we have an initial name space? */
    if (!hProppatch->nameSpaceList) {
        hProppatch->nameSpaceList = newNameSpace;
    } else {
        hProppatch->lastNameSpace->nextNameSpace = newNameSpace;
    }

    /* Store last name space */
    hProppatch->lastNameSpace = newNameSpace;

	return WDV_True;


}

/*===========================================================================*/

WDV_Bool Proppatch_InitNameSpace( WDVCAPI_WDV                wdv,
                                  WDVCAPI_ProppatchHandle    hProppatch,
                                  ProppatchNameSpace         newNameSpace,
                                  WDVCAPI_NameSpace          nameSpace,
                                  WDVCAPI_NameSpaceShortcut *nameSpaceShortcut )
{

    WDVCAPI_Id  newId;

	if (!wdv || !newNameSpace || !nameSpace || !nameSpaceShortcut) {
		return WDV_False;
	}

    /* Initialize new name space */
    strncpy(newNameSpace->nameSpace, nameSpace, WDV_MAX_NAME_SPACE_LEN);
    newNameSpace->nameSpace[WDV_MAX_NAME_SPACE_LEN] = '\0';

    /* Get new shortcut id */
    hProppatch->nameSpaceShortcutCounter++;
    sp77sprintf(newNameSpace->nameSpaceShortcut,
                WDV_MAX_NAME_SPACE_SHORTCUT_LEN,
                "%d", hProppatch->nameSpaceShortcutCounter);

    /* Init next pointer */
    newNameSpace->nextNameSpace = NULL;

    /* Set return value */
    *nameSpaceShortcut = newNameSpace->nameSpaceShortcut;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_SearchNameSpaceInList( WDVCAPI_WDV                wdv,
                                          ProppatchNameSpace         nameSpaceList,
                                          WDVCAPI_NameSpace          nameSpaceToFind,
                                          WDVCAPI_NameSpaceShortcut *foundNameSpaceShortcut )
{

    ProppatchNameSpace  currentNameSpace = NULL;

    /* Checks */
    if (!wdv || !nameSpaceToFind || !foundNameSpaceShortcut) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        if (foundNameSpaceShortcut) {
            *foundNameSpaceShortcut = NULL;
        }

        return WDV_False;
    }

    /* Inits */
    *foundNameSpaceShortcut = NULL;

    /* Search name space list */
    currentNameSpace = nameSpaceList;
    while(currentNameSpace) {
        if (strcmp(currentNameSpace->nameSpace, nameSpaceToFind) == 0) {
            *foundNameSpaceShortcut = currentNameSpace->nameSpaceShortcut;
            return WDV_True;
        }

        /* Get next name space from list */
        currentNameSpace = currentNameSpace->nextNameSpace;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_InitHandle( WDVCAPI_WDV               wdv,
                               WDVCAPI_ProppatchHandle   hProppatch,
                               WDVCAPI_URI               uri )
{

    strncpy(hProppatch->uri, uri, WDV_MAX_URI_LEN);
    hProppatch->uri[WDV_MAX_URI_LEN] = '\0';
    memset(hProppatch->uriId, 0, WDVCAPI_MAX_ID_LEN);
    hProppatch->nameSpaceShortcutCounter = 0;
    hProppatch->nameSpaceList = NULL;
    hProppatch->nextNameSpace = NULL;
    hProppatch->lastNameSpace = NULL;
    hProppatch->processedPropertyList = NULL;
    hProppatch->nextProcessedProperty = NULL;
    hProppatch->lastProcessedProperty = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_DestroyNameSpaceList( WDVCAPI_WDV        wdv,
                                         ProppatchNameSpace nameSpaceList)
{

    ProppatchNameSpace  currentNameSpace = nameSpaceList;
    ProppatchNameSpace  nextNameSpace = NULL;

    while(currentNameSpace) {
        nextNameSpace = currentNameSpace->nextNameSpace;
        WDVCAPI_Free((WDV_UInt1*) currentNameSpace);
        currentNameSpace = nextNameSpace;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_DestroyProcessedPropertyList( WDVCAPI_WDV                wdv,
                                                 ProppatchProcessedProperty processedPropertyList)
{

    ProppatchProcessedProperty  currentProperty= processedPropertyList;
    ProppatchProcessedProperty  nextProperty = NULL;

    while(currentProperty) {
        nextProperty = currentProperty->nextProcessedProperty;
        WDVCAPI_Free((WDV_UInt1*) currentProperty);
        currentProperty = nextProperty;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_GetDocClassProperties( WDVCAPI_WDV   wdv,
                                          WDVCAPI_URI   uri,
                                          WDV_Bool      withLockCheck )
{

    SQLRETURN           sqlRC = 0;
    WDVCAPI_Resource    resource = NULL;
    WDVCAPI_LockHandle  hLock = NULL;

    /* Get resource id */
    if (!Resource_CreateHandle(wdv, &resource)) {
        return WDV_False;
    }
    if (!Resource_GetByUri(wdv, uri, resource)) {
        return WDV_False;
    }

    /* Check locks? */
    if (withLockCheck == WDV_True) {
        /* Is something locked in URI? */
        if (!WDVCAPI_LockCreateHandle(wdv, &hLock)) {
	        Resource_DestroyHandle(wdv, resource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        if (Resource_IsLockInURI(wdv, resource, hLock)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_LOCK_EXISTS,
				                WDVCAPI_ERR_TEXT_LOCK_EXISTS);

            WDVCAPI_LockDestroyHandle(wdv, hLock);
	        Resource_DestroyHandle(wdv, resource);

            return WDV_False;
        }

        WDVCAPI_LockDestroyHandle(wdv, hLock);
    }

    /* Set input parameter */
    Resource_GetId(resource, wdv->dcMgtResourceId);

    /* Free resource */
    Resource_DestroyHandle(wdv, resource);
    resource = NULL;

    /* Statement handle already there? */
    if (!wdv->dcMgtStmtGetDocClassProperties) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->dcMgtStmtGetDocClassProperties);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtGetDocClassProperties, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_DROP);
            wdv->dcMgtStmtGetDocClassProperties = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->dcMgtStmtGetDocClassProperties, (SQLCHAR *) SQL_GET_DOC_CLASS_PROPERTIES, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtGetDocClassProperties, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_DROP);
            wdv->dcMgtStmtGetDocClassProperties = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->dcMgtStmtGetDocClassProperties, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->dcMgtResourceId, WDVCAPI_MAX_ID_LEN,
                                &wdv->dcMgtResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtGetDocClassProperties, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_DROP);
            wdv->dcMgtStmtGetDocClassProperties = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(wdv->dcMgtStmtGetDocClassProperties, 1, SQL_C_CHAR,
                           wdv->dcMgtDocClassId, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                          &wdv->dcMgtDocClassIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtGetDocClassProperties, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_DROP);
            wdv->dcMgtStmtGetDocClassProperties = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindCol(wdv->dcMgtStmtGetDocClassProperties, 2, SQL_C_CHAR,
                           wdv->dcMgtIndexingState, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                          &wdv->dcMgtIndexingStateIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtGetDocClassProperties, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_DROP);
            wdv->dcMgtStmtGetDocClassProperties = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_CLOSE);
    }

    /* Execute statement */
    sqlRC = SQLExecute(wdv->dcMgtStmtGetDocClassProperties);
    if (sqlRC != 0) {
        Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtGetDocClassProperties, sqlRC);
    	SQLFreeStmt(wdv->dcMgtStmtGetDocClassProperties, SQL_DROP);
        wdv->dcMgtStmtGetDocClassProperties = 0;

		return WDV_False;
    }

    /* Fetch result */
    sqlRC = SQLFetch(wdv->dcMgtStmtGetDocClassProperties);
    if (sqlRC != 0) {
        Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtGetDocClassProperties, sqlRC);

		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_NO_DOC_CLASS_PROPERTY,
				                WDVCAPI_ERR_TEXT_NO_DOC_CLASS_PROPERTY);

        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_UpdateDocClassProperties( WDVCAPI_WDV                wdv,
                                             WDVCAPI_PropertyShortValue docClassId )
{

    SQLRETURN   sqlRC = 0;

    /* Update document class */
    /* Statement handle already there? */
    if (!wdv->dcMgtStmtUpdateDocClass) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->dcMgtStmtUpdateDocClass);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtUpdateDocClass, SQL_DROP);
            wdv->dcMgtStmtUpdateDocClass = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->dcMgtStmtUpdateDocClass, (SQLCHAR *) SQL_UPDATE_DOC_CLASS, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtUpdateDocClass, SQL_DROP);
            wdv->dcMgtStmtUpdateDocClass = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->dcMgtStmtUpdateDocClass, 1,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 wdv->dcMgtDocClassId, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                                &wdv->dcMgtDocClassIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtUpdateDocClass, SQL_DROP);
            wdv->dcMgtStmtUpdateDocClass = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->dcMgtStmtUpdateDocClass, 2,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->dcMgtResourceId, WDVCAPI_MAX_ID_LEN,
                                &wdv->dcMgtResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtUpdateDocClass, SQL_DROP);
            wdv->dcMgtStmtUpdateDocClass = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(wdv->dcMgtStmtUpdateDocClass, SQL_CLOSE);
    }

    /* Set input parameters */
    strcpy(wdv->dcMgtDocClassId, docClassId);

    /* Execute statement */
    sqlRC = SQLExecute(wdv->dcMgtStmtUpdateDocClass);
    if (sqlRC != 0) {
        Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateDocClass, sqlRC);
    	SQLFreeStmt(wdv->dcMgtStmtUpdateDocClass, SQL_DROP);
        wdv->dcMgtStmtUpdateDocClass = 0;

		return WDV_False;
    }

    /* Update indexing state */
    /* Statement handle already there? */
    if (!wdv->dcMgtStmtUpdateIndexingState) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->dcMgtStmtUpdateIndexingState);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateIndexingState, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtUpdateIndexingState, SQL_DROP);
            wdv->dcMgtStmtUpdateIndexingState = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->dcMgtStmtUpdateIndexingState, (SQLCHAR *) SQL_UPDATE_INDEXING_STATE, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateIndexingState, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtUpdateIndexingState, SQL_DROP);
            wdv->dcMgtStmtUpdateIndexingState = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->dcMgtStmtUpdateIndexingState, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->dcMgtResourceId, WDVCAPI_MAX_ID_LEN,
                                &wdv->dcMgtResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateIndexingState, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtUpdateIndexingState, SQL_DROP);
            wdv->dcMgtStmtUpdateIndexingState = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(wdv->dcMgtStmtUpdateIndexingState, SQL_CLOSE);
    }

    /* Set input parameters */
    strcpy(wdv->dcMgtIndexingState, XMLINDEXING_STATE_NEW);

    /* Execute statement */
    sqlRC = SQLExecute(wdv->dcMgtStmtUpdateIndexingState);
    if (sqlRC != 0) {
        Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtUpdateIndexingState, sqlRC);
    	SQLFreeStmt(wdv->dcMgtStmtUpdateIndexingState, SQL_DROP);
        wdv->dcMgtStmtUpdateIndexingState = 0;

		return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_InsertDocClassProperties( WDVCAPI_WDV                wdv,
                                             WDVCAPI_PropertyShortValue docClassId )
{

    SQLRETURN   sqlRC = 0;

    /* Something to do? */
    if (!docClassId) {
        return WDV_True;
    }

    /* Insert document class */
    /* Statement handle already there? */
    if (!wdv->dcMgtStmtInsertDocClass) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->dcMgtStmtInsertDocClass);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtInsertDocClass, SQL_DROP);
            wdv->dcMgtStmtInsertDocClass = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->dcMgtStmtInsertDocClass, (SQLCHAR *) SQL_INSERT_DOC_CLASS, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtInsertDocClass, SQL_DROP);
            wdv->dcMgtStmtInsertDocClass = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->dcMgtStmtInsertDocClass, 1,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 wdv->dcMgtDocClassId, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                                &wdv->dcMgtDocClassIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtInsertDocClass, SQL_DROP);
            wdv->dcMgtStmtInsertDocClass = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->dcMgtStmtInsertDocClass, 2,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->dcMgtResourceId, WDVCAPI_MAX_ID_LEN,
                                &wdv->dcMgtResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertDocClass, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtInsertDocClass, SQL_DROP);
            wdv->dcMgtStmtInsertDocClass = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(wdv->dcMgtStmtInsertDocClass, SQL_CLOSE);
    }

    /* Set input parameters */
    strcpy(wdv->dcMgtDocClassId, docClassId);

    /* Execute statement */
    sqlRC = SQLExecute(wdv->dcMgtStmtInsertDocClass);
    if (sqlRC != 0) {
        Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertDocClass, sqlRC);
    	SQLFreeStmt(wdv->dcMgtStmtInsertDocClass, SQL_DROP);
        wdv->dcMgtStmtInsertDocClass = 0;

		return WDV_False;
    }

    /* Insert indexing state */
    /* Statement handle already there? */
    if (!wdv->dcMgtStmtInsertIndexingState) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->dcMgtStmtInsertIndexingState);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertIndexingState, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtInsertIndexingState, SQL_DROP);
            wdv->dcMgtStmtInsertIndexingState = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->dcMgtStmtInsertIndexingState, (SQLCHAR *) SQL_INSERT_INDEXING_STATE, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertIndexingState, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtInsertIndexingState, SQL_DROP);
            wdv->dcMgtStmtInsertIndexingState = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->dcMgtStmtInsertIndexingState, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->dcMgtResourceId, WDVCAPI_MAX_ID_LEN,
                                &wdv->dcMgtResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertIndexingState, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtInsertIndexingState, SQL_DROP);
            wdv->dcMgtStmtInsertIndexingState = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(wdv->dcMgtStmtInsertIndexingState, SQL_CLOSE);
    }

    /* Set input parameters */
    strcpy(wdv->dcMgtIndexingState, XMLINDEXING_STATE_NEW);

    /* Execute statement */
    sqlRC = SQLExecute(wdv->dcMgtStmtInsertIndexingState);
    if (sqlRC != 0) {
        Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtInsertIndexingState, sqlRC);
    	SQLFreeStmt(wdv->dcMgtStmtInsertIndexingState, SQL_DROP);
        wdv->dcMgtStmtInsertIndexingState = 0;

		return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Proppatch_DeleteIndexValues( WDVCAPI_WDV   wdv )
{

    SQLRETURN   sqlRC = 0;

    /* Statement handle already there? */
    if (!wdv->dcMgtStmtDeleteIndexValues) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->dcMgtStmtDeleteIndexValues);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtDeleteIndexValues, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_DROP);
            wdv->dcMgtStmtDeleteIndexValues = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->dcMgtStmtDeleteIndexValues, (SQLCHAR *) SQL_DELETE_INDEX_VALUE_PROPS, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtDeleteIndexValues, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_DROP);
            wdv->dcMgtStmtDeleteIndexValues = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->dcMgtStmtDeleteIndexValues, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->dcMgtResourceId, WDVCAPI_MAX_ID_LEN,
                                &wdv->dcMgtResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtDeleteIndexValues, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_DROP);
            wdv->dcMgtStmtDeleteIndexValues = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->dcMgtStmtDeleteIndexValues, 2,
                                 SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 wdv->dcMgtDocClassId, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                                &wdv->dcMgtDocClassIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtDeleteIndexValues, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_DROP);
            wdv->dcMgtStmtDeleteIndexValues = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->dcMgtStmtDeleteIndexValues, 3,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->dcMgtResourceId, WDVCAPI_MAX_ID_LEN,
                                &wdv->dcMgtResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtDeleteIndexValues, sqlRC);
    	    SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_DROP);
            wdv->dcMgtStmtDeleteIndexValues = 0;

		    return WDV_False;
	    }
    } else {
        SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_CLOSE);
    }

    /* Input parameters should be set already */

    /* Execute statement */
    sqlRC = SQLExecute(wdv->dcMgtStmtDeleteIndexValues);
    if (sqlRC != 0) {
        Error_AddSQLErrorItem(wdv, wdv->dcMgtStmtDeleteIndexValues, sqlRC);
        if (!WDVCAPI_IsState(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED)) {
    	    SQLFreeStmt(wdv->dcMgtStmtDeleteIndexValues, SQL_DROP);
            wdv->dcMgtStmtDeleteIndexValues = 0;

		    return WDV_False;
        }
    }

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
