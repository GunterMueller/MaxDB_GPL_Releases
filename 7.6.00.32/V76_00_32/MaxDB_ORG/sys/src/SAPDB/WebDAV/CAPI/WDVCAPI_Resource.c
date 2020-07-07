/*!**********************************************************************

  module: WDVCAPI_Resource.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV Resource handling

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Container.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Property.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Delete.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_SELECT_BY_CID    \
    "SELECT I.CId, I.State, P.Property_Short_Value \
     FROM WEBDAV_Inode I, WEBDAV_Property P WHERE  \
     CId = X'%s' And I.CId = P.CId (+) And P.Property_Id = X'"ID_PROPERTY_LOCK_DEPTH_STRING"'"

#define SQL_SELECT_PARENT_BY_CID    \
    "SELECT T2.CID, T2.Name FROM WEBDAV_Inode T1, WEBDAV_Inode T2 WHERE T1.PId = T2.CId And T1.CId = X'%s'"

#define SQL_SELECT_BY_NAME  \
    "SELECT I.CId, I.State, I.CompressedLength, P.Property_Short_Value, P2.Property_Short_Value, I.XDBDocId \
     FROM WEBDAV_Inode I, WEBDAV_Property P, WEBDAV_Property P2 WHERE  \
     PId = ? AND Name = ? And I.CId = P.CId (+) And I.CId = P2.CId And P.Property_Id (+) = X'"ID_PROPERTY_LOCK_DEPTH_STRING"' And P2.Property_Id = X'"ID_PROPERTY_RESOURCE_TYPE_STRING"'"

#define SQL_SELECT_ROOT  \
    "SELECT X'"ID_ROOT_STRING"', 0, Property_Short_Value \
     FROM WEBDAV_Property WHERE CId = ? AND Property_Id = X'"ID_PROPERTY_LOCK_DEPTH_STRING"'"

#define SQL_SET_STATE   \
    "UPDATE WEBDAV_Inode SET State = %d WHERE CId = X'%s'"

#define SQL_CREATE_INODE    \
    "INSERT INTO WEBDAV_INODE SET PId = ?, CId = ?, Name = ?, State = ?, CompressedLength = ?"

#define SQL_SET_COMPRESSED_LENGTH   \
    "UPDATE WEBDAV_Inode SET CompressedLength = ? WHERE PId = ? AND Name = ?"

#define SQL_GET_COMPRESSED_LENGTH   \
    "SELECT CompressedLength FROM WEBDAV_Inode WHERE PId = ? AND Name = ?"

#define SQL_GET_CHILDS  \
    "SELECT CId, Name FROM WEBDAV_Inode WHERE PId = ?"

#define SQL_GET_CHILDS_WITH_PROPS  \
    "SELECT \
        I.CId, \
        I.Name, \
        I.xdbDocId, \
        RT.Property_Short_Value ResourceType, \
        S.Property_Short_Value Size, \
        SUBSTR(LM.Property_Short_Value,1,19) LastModified \
    FROM \
        WebDAV_Inode I, \
        WebDAV_Property RT, \
        WebDAV_Property S, \
        WebDAV_Property LM \
    WHERE \
        I.PId = ? AND \
        I.CId = RT.CId AND \
        RT.Property_Id = x'000000000000000000000000000000000000000000000002' AND \
        RT.CId = S.CId AND \
        S.Property_Id = x'000000000000000000000000000000000000000000000004' AND \
        S.CId = LM.CId AND \
        LM.Property_Id = x'000000000000000000000000000000000000000000000005' \
    Order By SUBSTR(RT.Property_Short_Value, 1, 30) DESC, SUBSTR(I.Name, 1, 400) ASC"

/* Set parent id */
#define SQL_RESOURCE_SET_PARENT \
    "UPDATE WEBDAV_INODE SET PId = ?, Name = ? WHERE CId = ?"
     
/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_resource_cache_item   *ResourceCacheItem;

/*---------------------------------------------------------------------------*/

struct st_resource_cache {
    ResourceCacheItem   firstCacheItem;
    ResourceCacheItem   lastCacheItem;

    ResourceCacheItem   lastSearchedCacheItem;

    WDV_UInt4           listCount;
};

/*---------------------------------------------------------------------------*/

struct st_resource_cache_item {
    WDVCAPI_URIString           uri;

    WDVCAPI_Id                  parentId;
    WDVCAPI_ResourceNameString  resourceName;

    ResourceCacheItem           nextCacheItem;
};

/*---------------------------------------------------------------------------*/

struct st_resource_handle_create_inode {
    SQLHSTMT                            hStmt;

    WDVCAPI_Id                          parentId;
    SQLLEN                              parentIdIndicator;

    WDVCAPI_Id                          resourceId;
    SQLLEN                              resourceIdIndicator;

	WDVCAPI_ResourceNameString			resourceName;
    SQLLEN                              resourceNameIndicator;

	WDVCAPI_ResourceState				resourceState;
    SQLLEN                              resourceStateIndicator;

    WDV_UInt4                           compressedLength;
    SQLINTEGER                          compressedLengthIndicator;
};

/***********************************************************************

	Variables

 ***********************************************************************/

#ifdef WDVCAPI_MEMCHECK
static int allocCount = 0;
static int cacheCount = 0;
#endif

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Resource_InitHandle( WDVCAPI_WDV      wdv,
                              WDVCAPI_Resource resource );

WDV_Bool Resource_DestroyFreeListItem( WDVCAPI_WDV      wdv,
                                       WDVCAPI_Resource resource );

/*---------------------------------------------------------------------------*/

WDV_Bool Resource_GetByUriLocal( WDVCAPI_WDV        wdv,
				                 WDVCAPI_URI        uri,
				                 WDVCAPI_Resource   resource,
                                 WDVCAPI_LockHandle hLock );

/*---------------------------------------------------------------------------*/

WDV_Bool Resource_AddResourceToFreeList( WDVCAPI_WDV       wdv,
                                         WDVCAPI_Resource *resourceFreeList,
                                         WDVCAPI_Resource  resource );

WDV_Bool Resource_GetResourceFromFreeList( WDVCAPI_WDV       wdv,
                                           WDVCAPI_Resource *resourceFreeList,
                                           WDVCAPI_Resource *resource );

/*---------------------------------------------------------------------------*/

WDV_Bool Resource_InitCache( WDVCAPI_ResourceCache  resourceCache );

WDV_Bool Resource_DestroyCache( WDVCAPI_WDV             wdv,
                                WDVCAPI_ResourceCache   resourceCache );

WDV_Bool Resource_CreateCacheItem( WDVCAPI_WDV          wdv,
                                   ResourceCacheItem   *resourceCacheItem );

WDV_Bool Resource_DestroyCacheItem( WDVCAPI_WDV         wdv,
                                    ResourceCacheItem   resourceCacheItem );

WDV_Bool Resource_AddCacheItem( WDVCAPI_WDV                 wdv,
                                WDVCAPI_URI                 uri,
                                WDVCAPI_Id                  parentId,
                                WDVCAPI_ResourceNameString  resourceName );

WDV_Bool Resource_InitHandleCreateInode( WDVCAPI_WDV                       wdv,
                                         WDVCAPI_ResourceHandleCreateInode hResourceCreateInode );

WDV_Bool Resource_ResetHandle( WDVCAPI_WDV      wdv,
                               WDVCAPI_Resource resource,
                               WDV_Bool         withStmtHandles );

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool Resource_CreateHandle( WDVCAPI_WDV        wdv,
						        WDVCAPI_Resource  *resource )
{

	WDV_Bool            ok = WDV_False;
    WDVCAPI_Resource   *resourceFreeList = NULL;
    WDVCAPI_Resource    newResource = NULL;

    TRACE_OUT(">> Resource_CreateHandle()\n");

	if ( !wdv || !resource) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

    /* Inits */
    *resource = NULL;

    /* Get handle of the cached resource handle list */
    WDV_GetResourceFreeList(wdv, &resourceFreeList);

    /* Search available resource handle in cache */
    if (!Resource_GetResourceFromFreeList(wdv, resourceFreeList, &newResource)) {
        return WDV_False;
    }

    /* Don't we have any free handle? */
    if (!newResource) {
        #ifdef WDVCAPI_MEMCHECK
        allocCount++;
        #endif

        /* We have to allocat new handle */
	    WDVCAPI_Allocat( sizeof(struct st_resource), (WDV_UInt1**) &newResource, &ok );
	    if (ok == WDV_False) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_NO_MEMORY,
                                    WDVCAPI_ERR_TEXT_NO_MEMORY);
		    return WDV_False;
	    }

	    /* Init put handle */
        if (!Resource_InitHandle(wdv, newResource)) {
            WDVCAPI_Free((WDV_UInt1*) newResource );
            return WDV_False;
        }
    }

    /* Set return value */
    *resource = newResource;

    TRACE_OUT("<< Resource_CreateHandle()\n");

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_DestroyFreeList( WDVCAPI_WDV      wdv,
                                   WDVCAPI_Resource resource )
{

    WDVCAPI_Resource    itemToDelete = NULL;

	if ( !resource ) {
		return WDV_False;
	}

    while (resource) {
        itemToDelete = resource;
        resource = resource->nextFreeResource;

        Resource_DestroyFreeListItem(wdv, itemToDelete);
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_DestroyHandle( WDVCAPI_WDV        wdv,
                                 WDVCAPI_Resource   resource )
{
    WDVCAPI_Resource    *resourceFreeList = NULL;

    TRACE_OUT(">> Resource_DestroyHandle()\n");

    #ifdef WDVCAPI_MEMCHECK
    cacheCount++;
    #endif

	if ( !resource ) {
		return WDV_False;
	}

    /* Get handle of the cached resource handle list */
    WDV_GetResourceFreeList(wdv, &resourceFreeList);

    /* Add handle to the cached handle list */
    if (!Resource_AddResourceToFreeList(wdv, resourceFreeList, resource)) {
        return WDV_False;
    }

    TRACE_OUT("<< Resource_DestroyHandle()\n");

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_CreateCache( WDVCAPI_WDV            wdv,
						       WDVCAPI_ResourceCache *resourceCache )
{

	WDV_Bool    ok = WDV_False;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_resource_cache), (WDV_UInt1**) &resourceCache, &ok );
	if (ok == WDV_False) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_NO_MEMORY,
                                WDVCAPI_ERR_TEXT_NO_MEMORY);
		return WDV_False;
	}

	/* Init cache */
    if (!Resource_InitCache(*resourceCache)) {
        WDVCAPI_Free((WDV_UInt1*) *resourceCache);
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_DestroyCache( WDVCAPI_WDV             wdv,
                                WDVCAPI_ResourceCache   resourceCache )
{

    ResourceCacheItem   itemToDelete = NULL;

	if ( !resourceCache ) {
		return WDV_False;
	}

    itemToDelete = resourceCache->firstCacheItem;
    while (itemToDelete) {
        resourceCache->firstCacheItem = itemToDelete->nextCacheItem;

        Resource_DestroyCacheItem(wdv, itemToDelete);

        itemToDelete = resourceCache->firstCacheItem;
    }

    /* Destroy cache itself */
    WDVCAPI_Free((WDV_UInt1*) resourceCache);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetByUri( WDVCAPI_WDV       wdv,
				            WDVCAPI_URI       uri,
				            WDVCAPI_Resource  resource )
{

    return Resource_GetByUriLocal(wdv, uri, resource, resource->currentLockDescription);

}

/*===========================================================================*/

WDV_Bool Resource_ClearLockDescription( WDVCAPI_WDV         wdv,
				                        WDVCAPI_Resource    resource )
{

    if (!Lock_InitHandle(wdv, resource->currentLockDescription)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_IsLockInURI( WDVCAPI_WDV          wdv,
				               WDVCAPI_Resource     resource,
                               WDVCAPI_LockHandle   hLock )
{

    /* If there is an lock found in the URI and is this lock infinite? */
    if (!WDVCAPI_IdIsInitialValue(resource->currentLockDescription->id) &&
         resource->currentLockDescription->depth == WDVCAPI_DEPTH_INFINITE) {
        /* If yes, than return this lock                       */
        /* This should be the upper most lock found in the URI */
        if (hLock && !Lock_Copy(wdv, hLock, resource->currentLockDescription)) {
            return WDV_False;
        }

        return WDV_True;
    } else {
        /* Is the resource itself locked? */
        if (Resource_IsLocked(resource, NULL)) {
            /* Get this lock description */
            if (hLock && !Lock_GetLockDescription(wdv, resource, hLock)) {
                return WDV_False;
            }
            return WDV_True;
        }
    }

    return WDV_False;
}

/*===========================================================================*/

WDV_Bool Resource_GetByName( WDVCAPI_WDV         wdv,
				             WDVCAPI_Resource    parent,
				             WDVCAPI_URI         name,
				             WDVCAPI_Resource    resource )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLHSTMT          hStmt = 0;

    if (parent) {
        /* Is statement already parsed? */
	    if (!resource->hStmtParent) {
            /* Get ODBC handles */
            GetDBC(wdv, &hDBC);

	        /* Allocat statement handle */
	        sqlRC = SQLAllocStmt(hDBC, &resource->hStmtParent);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

	        /* Prepare statement */
	        sqlRC = SQLPrepare( resource->hStmtParent, (SQLCHAR *) SQL_SELECT_BY_NAME, SQL_NTS );
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

            /* Bind input parameter */
            sqlRC = SQLBindParameter(resource->hStmtParent, 1,
                                     SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                     resource->parentId, WDVCAPI_MAX_ID_LEN,
                                     &resource->parentIdIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, resource->hStmtParent, sqlRC);

                SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindParameter(resource->hStmtParent, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                     resource->name, WDV_MAX_RESOURCE_NAME_LEN,
                                     &resource->nameIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );

		        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
		        resource->hStmtParent = 0;

		        return WDV_False;
	        }

            /* Bind output columns */
            sqlRC = SQLBindCol(resource->hStmtParent, 1, SQL_C_BINARY,
                               resource->id, WDVCAPI_MAX_ID_LEN,
                               &resource->idIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindCol(resource->hStmtParent, 2, SQL_C_ULONG,
                               &resource->state, sizeof(resource->state),
                               &resource->stateIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindCol(resource->hStmtParent, 3, SQL_C_ULONG,
                               &resource->compressedLength, sizeof(resource->compressedLength),
                               &resource->compressedLengthIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );

    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindCol(resource->hStmtParent, 4, SQL_C_LONG,
                               &resource->lockDepth, sizeof(resource->lockDepth),
                               &resource->lockDepthIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindCol(resource->hStmtParent, 5, SQL_C_CHAR,
                               resource->resourceType, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                              &resource->resourceTypeIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindCol(resource->hStmtParent, 6, SQL_C_ULONG,
                               &resource->xdbDocId, sizeof(resource->xdbDocId),
                               &resource->xdbDocIdIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtParent, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtParent = 0;

		        return WDV_False;
	        }
        } else {
            /* Close statement */
            SQLFreeStmt(resource->hStmtParent, SQL_CLOSE);
        }

        /* Set input values */
		Resource_GetId(parent, resource->parentId);
        Common_StrMaxCopy(resource->name, name, WDV_MAX_RESOURCE_NAME_LEN);
        resource->nameIndicator = SQL_NTS;

        /* Set statement handle wich is to be used */
        hStmt = resource->hStmtParent;
    } else {
        /* Inits */
        resource->xdbDocId = 0;

        /* Is statement already parsed? */
	    if (!resource->hStmtRoot) {
            /* Get ODBC handles */
            GetDBC(wdv, &hDBC);

	        /* Allocat statement handle */
	        sqlRC = SQLAllocStmt(hDBC, &resource->hStmtRoot);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtRoot, sqlRC );
    	        SQLFreeStmt(resource->hStmtRoot, SQL_DROP);
                resource->hStmtRoot = 0;

		        return WDV_False;
	        }
	        /* Prepare statement */
	        sqlRC = SQLPrepare( resource->hStmtRoot, (SQLCHAR *) SQL_SELECT_ROOT, SQL_NTS );
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtRoot, sqlRC );
    	        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
                resource->hStmtRoot = 0;

		        return WDV_False;
	        }

            /* Bind input parameters */
            sqlRC = SQLBindParameter(resource->hStmtRoot, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                     resource->id, WDVCAPI_MAX_ID_LEN,
                                    &resource->idIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, resource->hStmtRoot, sqlRC );

		        SQLFreeStmt(resource->hStmtRoot, SQL_DROP);
		        resource->hStmtRoot = 0;

		        return WDV_False;
	        }

            /* Bind output columns */
            sqlRC = SQLBindCol(resource->hStmtRoot, 1, SQL_C_BINARY,
                               resource->id, WDVCAPI_MAX_ID_LEN,
                               &resource->idIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtRoot, sqlRC );
    	        SQLFreeStmt(resource->hStmtRoot, SQL_DROP);
                resource->hStmtRoot = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindCol(resource->hStmtRoot, 2, SQL_C_ULONG,
                               &resource->state, sizeof(resource->state),
                               &resource->stateIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtRoot, sqlRC );
    	        SQLFreeStmt(resource->hStmtRoot, SQL_DROP);
                resource->hStmtRoot = 0;

		        return WDV_False;
	        }

            sqlRC = SQLBindCol(resource->hStmtRoot, 3, SQL_C_LONG,
                               &resource->lockDepth, sizeof(resource->lockDepth),
                               &resource->lockDepthIndicator);
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem( wdv, resource->hStmtRoot, sqlRC );
    	        SQLFreeStmt(resource->hStmtRoot, SQL_DROP);
                resource->hStmtRoot = 0;

		        return WDV_False;
	        }
        } else {
            /* Close statement */
            SQLFreeStmt(resource->hStmtRoot, SQL_CLOSE);
        }

        /* Set statement handle wich is to be used */
        hStmt = resource->hStmtRoot;
	}

    /* Copy input parameters */
    SAPDB_memcpy(resource->id, ID_ROOT, WDVCAPI_MAX_ID_LEN);

	/* Execute SQL */
	sqlRC = SQLExecute(hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(hStmt);

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA && !parent) {
            /* The resource being searched is ROOT                          */
            /* So set the fetch values as it has been fetched correctly     */
            /* The SQL statement cannot fetch ROOT because it doesn't exist */
            WDVCAPI_IdInitValue(resource->id);
            resource->state = WDVCAPI_RESOURCE_STATE_DEFAULT;
            resource->compressedLength = 0;
            resource->lockDepthIndicator = SQL_NULL_DATA;
            strcpy(resource->resourceType, PROPERTY_VALUE_COLLECTION);
        } else {
            if (sqlRC == SQL_NO_DATA) {
                Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                        WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST,
                                        WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_DOESNT_EXIST);
            } else {
                Error_AddSQLErrorItem(wdv, hStmt, sqlRC);
            }

		    return WDV_False;
        }
	}

    /* Is there a lock? */
    if (resource->lockDepthIndicator == SQL_NULL_DATA) {
        resource->lockDepth = WDVCAPI_DEPTH_UNDEFINED;
    }

    /* Append to cache */
    /*
    Resource_AddCacheItem(wdv, uri, parentId, name);
    */

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_IsLocked( WDVCAPI_Resource    resource,
                            WDVCAPI_Depth      *lockDepth )
{

    /* Is there a lock? */
    switch(resource->lockDepth) {
    case WDVCAPI_DEPTH_0:
    case WDVCAPI_DEPTH_INFINITE:
        if (lockDepth) {
            *lockDepth = resource->lockDepth;
        }

        return WDV_True;
    default:
        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool Resource_IsNullResource( WDVCAPI_Resource    resource )
{

    /* Is null resource state set? */
    if (resource->state == WDVCAPI_RESOURCE_STATE_NULL_RESOURCE) {
        return WDV_True;
    } else {
        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool Resource_CollectChildsWithProps( WDVCAPI_WDV                 wdv,
                                          WDVCAPI_Resource            parentResource,
                                          WDVCAPI_Resource            childResource,
                                          WDV_Bool                   *childFound )
{

	WDVCAPI_IdString  parentResourceIdString = "";
	SQLRETURN         sqlRC = 0;

    /* Init child resource */
    Resource_InitHandle(wdv, childResource);

    /* Get resource id */
    if (!Resource_GetIdAsString(parentResource, parentResourceIdString)) {
        return WDV_False;
    }

    /* Statement already prepared? */
    if ( !parentResource->hStmtCollectChildsWithProps ) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &parentResource->hStmtCollectChildsWithProps);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );
		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare( parentResource->hStmtCollectChildsWithProps, (SQLCHAR *) SQL_GET_CHILDS_WITH_PROPS, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );
		    return WDV_False;
	    }
    } else {
        /* Close statement */
        SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_CLOSE);
    }

    /* Bind input parameter */
    sqlRC = SQLBindParameter(parentResource->hStmtCollectChildsWithProps, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                             parentResource->id, WDVCAPI_MAX_ID_LEN,
                             &parentResource->idIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
		parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(parentResource->hStmtCollectChildsWithProps, 1, SQL_C_BINARY,
                       childResource->idWithProps, WDVCAPI_MAX_ID_LEN,
                       &childResource->idWithPropsIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
        parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(parentResource->hStmtCollectChildsWithProps, 2, SQL_C_CHAR,
                       childResource->nameWithProps, WDV_MAX_RESOURCE_NAME_LEN,
                       &childResource->nameWithPropsIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
        parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(parentResource->hStmtCollectChildsWithProps, 3, SQL_C_ULONG,
                       &childResource->xdbDocId, 0,
                       &childResource->xdbDocIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
        parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(parentResource->hStmtCollectChildsWithProps, 4, SQL_C_CHAR,
                       childResource->resourceTypeWithProps, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                      &childResource->resourceTypeWithPropsIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
        parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(parentResource->hStmtCollectChildsWithProps, 5, SQL_C_CHAR,
                       childResource->sizeWithProps, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                      &childResource->sizeWithPropsIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
        parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(parentResource->hStmtCollectChildsWithProps, 6, SQL_C_CHAR,
                       childResource->lastModifiedWithProps, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                      &childResource->lastModifiedWithPropsIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
        parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(parentResource->hStmtCollectChildsWithProps);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
        parentResource->hStmtCollectChildsWithProps = 0;

		return WDV_False;
	}

    return Resource_GetNextFromCollectionWithProps(wdv, parentResource, childFound);

}

/*===========================================================================*/

WDV_Bool Resource_GetNextFromCollectionWithProps( WDVCAPI_WDV       wdv,
                                                  WDVCAPI_Resource  parentResource,
                                                  WDV_Bool         *childFound )
{

	SQLRETURN         sqlRC = 0;

    /* Fetch */
    *childFound = WDV_True;
    sqlRC = SQLFetch(parentResource->hStmtCollectChildsWithProps);

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            *childFound = WDV_False;
        } else {
            Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChildsWithProps, sqlRC );

		    SQLFreeStmt(parentResource->hStmtCollectChildsWithProps, SQL_DROP);
            parentResource->hStmtCollectChildsWithProps = 0;

	        return WDV_False;
        }
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_CollectChilds( WDVCAPI_WDV                 wdv,
                                 WDVCAPI_Resource            parentResource,
                                 WDVCAPI_Resource            childResource,
                                 WDV_Bool                   *childFound )
{

	WDVCAPI_IdString  parentResourceIdString = "";
	SQLRETURN         sqlRC = 0;

    /* Init child resource */
    Resource_InitHandle(wdv, childResource);

    /* Get resource id */
    if (!Resource_GetIdAsString(parentResource, parentResourceIdString)) {
        return WDV_False;
    }

    /* Statement already prepared? */
    if ( !parentResource->hStmtCollectChilds ) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &parentResource->hStmtCollectChilds);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChilds, sqlRC );
		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare( parentResource->hStmtCollectChilds, (SQLCHAR *) SQL_GET_CHILDS, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChilds, sqlRC );
		    return WDV_False;
	    }
    } else {
        /* Close statement */
        SQLFreeStmt(parentResource->hStmtCollectChilds, SQL_CLOSE);
    }

    /* Bind input parameter */
    sqlRC = SQLBindParameter(parentResource->hStmtCollectChilds, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                             parentResource->id, WDVCAPI_MAX_ID_LEN,
                             &parentResource->idIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, parentResource->hStmtCollectChilds, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChilds, SQL_DROP);
		parentResource->hStmtCollectChilds = 0;

		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(parentResource->hStmtCollectChilds, 1, SQL_C_BINARY,
                       childResource->id, WDVCAPI_MAX_ID_LEN,
                       &childResource->idIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChilds, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChilds, SQL_DROP);
        parentResource->hStmtCollectChilds = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(parentResource->hStmtCollectChilds, 2, SQL_C_CHAR,
                       childResource->name, WDV_MAX_RESOURCE_NAME_LEN,
                       &childResource->nameIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChilds, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChilds, SQL_DROP);
        parentResource->hStmtCollectChilds = 0;

		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(parentResource->hStmtCollectChilds);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChilds, sqlRC );

		SQLFreeStmt(parentResource->hStmtCollectChilds, SQL_DROP);
        parentResource->hStmtCollectChilds = 0;

		return WDV_False;
	}

    return Resource_GetNextFromCollection(wdv, parentResource, childResource, childFound);

}

/*===========================================================================*/

WDV_Bool Resource_GetNextFromCollection( WDVCAPI_WDV                 wdv,
				                         WDVCAPI_Resource            parentResource,
				                         WDVCAPI_Resource            childResource,
				                         WDV_Bool                   *childFound )
{

	SQLRETURN         sqlRC = 0;

    /* Fetch */
    *childFound = WDV_True;
    sqlRC = SQLFetch( parentResource->hStmtCollectChilds );

	/* Error handling */
	if ( sqlRC != 0 ) {
        *childFound = WDV_False;
        if (sqlRC != SQL_NO_DATA) {
            Error_AddSQLErrorItem( wdv, parentResource->hStmtCollectChilds, sqlRC );

		    SQLFreeStmt(parentResource->hStmtCollectChilds, SQL_DROP);
            parentResource->hStmtCollectChilds = 0;

	        return WDV_False;
        }
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetName( WDVCAPI_Resource      resource,
                           WDVCAPI_ResourceName *name )
{

    *name = resource->name;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetId( WDVCAPI_Resource  resource,
                         WDVCAPI_Id        id )
{

    SAPDB_memcpy(id, resource->id, WDVCAPI_MAX_ID_LEN);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetIdAsString( WDVCAPI_Resource resource,
                                 WDVCAPI_IdString idString )
{

    if (!resource) {
        return WDV_False;
    }
    
    WDVCAPI_IdAsString(resource->id, idString);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_IsRoot( WDVCAPI_Resource  resource )
{

	/* Return */
	if (!memcmp(resource->id, ID_ROOT, WDVCAPI_MAX_ID_LEN)) {
		return WDV_True;
	} else {
		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool Resource_IdIsRoot( WDVCAPI_Id  id )
{

	/* Return */
	if (!memcmp(id, ID_ROOT, WDVCAPI_MAX_ID_LEN)) {
		return WDV_True;
	} else {
		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool Resource_IdIsDeletedItems( WDVCAPI_Id  id )
{

	/* Return */
	if (!memcmp(id, ID_DELETED_ITEMS, WDVCAPI_MAX_ID_LEN)) {
		return WDV_True;
	} else {
		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool Resource_CreateInode( WDVCAPI_WDV            wdv,
                               WDVCAPI_Resource       parent,
                               WDVCAPI_ResourceName   resourceName,
                               WDVCAPI_ResourceState  resourceState,
                               WDV_UInt4              compressedLength,
                               WDVCAPI_Overwrite      overwrite,
                               WDVCAPI_Resource       newResource,
                               WDV_Bool              *resourceAlreadyExists,
                               WDV_Bool               withDisplayname )
{

	SQLRETURN             sqlRC = 0;
	SQLHDBC               hDBC = 0;
    WDVCAPI_Id            parentId;
    WDVCAPI_Id            resourceId;

	/* Checks */
	if (!wdv || !parent || !resourceName || !newResource) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
	}

    /* Get cid if resource already exists */
    if (Resource_GetByName(wdv, parent, resourceName, newResource)) {
        if (resourceAlreadyExists) {
            *resourceAlreadyExists = WDV_True;
        }

        switch(overwrite) {
        case WDVCAPI_DELETE_BEFORE_OVERWRITE:
            if (!Delete_Inode(wdv, newResource, WDV_False)) {
                return WDV_False;
            }

            break;

        case WDVCAPI_OVERWRITE:
            return WDV_True;

        case WDVCAPI_NO_OVERWRITE:
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_ALREADY_EXIST,
                                    WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_ALREADY_EXIST);
		    return WDV_False;

        default:
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_UNDEFINED_OVERWRITE,
                                    WDVCAPI_ERR_TEXT_UNDEFINED_OVERWRITE);
		    return WDV_False;
        }
    } else {
        if (resourceAlreadyExists) {
            *resourceAlreadyExists = WDV_False;
        }

        /* Resource doesn't exist and has to be created */
		if(!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
            /* Another unexpected error */
            return WDV_False;
        }
    }

    /* Get new id */
    if (!WDVCAPI_IdGetNext(wdv, resourceId)) {
        return WDV_False;
    }

    /* Get parent id */
    if (!Resource_GetId(parent, parentId)) {
        return WDV_False;
    }

	/* Statement already prepared? */
	if (!wdv->hResourceCreateInode->hStmt) {
	    /* Get DB handles */
	    if (!GetDBC(wdv, &hDBC)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                    WDVCAPI_ERR_CODE_INVALID_HANDLE,
				                    WDVCAPI_ERR_TEXT_INVALID_HANDLE );

		    return WDV_False;
	    }

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &wdv->hResourceCreateInode->hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hResourceCreateInode->hStmt, sqlRC );
    	    SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_DROP);
            wdv->hResourceCreateInode->hStmt = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hResourceCreateInode->hStmt, (SQLCHAR *) SQL_CREATE_INODE, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hResourceCreateInode->hStmt, sqlRC );
    	    SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_DROP);
            wdv->hResourceCreateInode->hStmt = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->hResourceCreateInode->hStmt, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->hResourceCreateInode->parentId, WDVCAPI_MAX_ID_LEN,
                                 &wdv->hResourceCreateInode->parentIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hResourceCreateInode->hStmt, sqlRC);

            SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_DROP);
            wdv->hResourceCreateInode->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hResourceCreateInode->hStmt, 2,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->hResourceCreateInode->resourceId, WDVCAPI_MAX_ID_LEN,
                                 &wdv->hResourceCreateInode->resourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hResourceCreateInode->hStmt, sqlRC);

            SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_DROP);
            wdv->hResourceCreateInode->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hResourceCreateInode->hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 wdv->hResourceCreateInode->resourceName, WDV_MAX_RESOURCE_NAME_LEN,
                                 &wdv->hResourceCreateInode->resourceNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hResourceCreateInode->hStmt, sqlRC );

		    SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_DROP);
		    wdv->hResourceCreateInode->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hResourceCreateInode->hStmt, 4, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0,
                                 &wdv->hResourceCreateInode->resourceState, 0,
                                 &wdv->hResourceCreateInode->resourceStateIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hResourceCreateInode->hStmt, sqlRC );

		    SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_DROP);
		    wdv->hResourceCreateInode->hStmt = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hResourceCreateInode->hStmt, 5,
                                 SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0,
                                 &wdv->hResourceCreateInode->compressedLength,
                                 sizeof(wdv->hResourceCreateInode->compressedLength),
                                 NULL);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hResourceCreateInode->hStmt, sqlRC);

            SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_DROP);
            wdv->hResourceCreateInode->hStmt = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
		SQLFreeStmt(wdv->hResourceCreateInode->hStmt, SQL_CLOSE);
    }

    /* Set current input values */
    SAPDB_memcpy(wdv->hResourceCreateInode->parentId, parentId, WDVCAPI_MAX_ID_LEN);
    SAPDB_memcpy(wdv->hResourceCreateInode->resourceId, resourceId, WDVCAPI_MAX_ID_LEN);
    wdv->hResourceCreateInode->resourceState = resourceState;
    wdv->hResourceCreateInode->compressedLength = compressedLength;
    Common_StrMaxCopy(wdv->hResourceCreateInode->resourceName, resourceName, WDV_MAX_RESOURCE_NAME_LEN);
    wdv->hResourceCreateInode->resourceNameIndicator = SQL_NTS;

    TRACE_OUT_ARG1(">> Resource_CreateInode()::wdv->hResourceCreateInode->resourceState: %d\n", wdv->hResourceCreateInode->resourceState);
    TRACE_OUT_ARG1(">> Resource_CreateInode()::wdv->hResourceCreateInode->compressedLength: %d\n", wdv->hResourceCreateInode->compressedLength);

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hResourceCreateInode->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hResourceCreateInode->hStmt, sqlRC);

		return WDV_False;
	}

    /* Insert displayname */
    if (withDisplayname == WDV_True) {
        if (!Property_SetShortValue(wdv, resourceId, ID_PROPERTY_DISPLAYNAME, resourceName)) {
		    return WDV_False;
	    }
    }

    /* Init new resource */
    if (!Resource_InitHandle(wdv, newResource)) {
        return WDV_False;
    }

	SAPDB_memcpy(newResource->id, resourceId, WDVCAPI_MAX_ID_LEN);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_CopyContainer( WDVCAPI_WDV        wdv,
                                 WDVCAPI_Resource   source,
                                 WDVCAPI_Resource   destination )
{

    WDVCAPI_Id              sourceId;
    WDVCAPI_Id              destinationId;

    /* Get ids */
    if (!Resource_GetId(source, sourceId)) {
        return WDV_False;
    }

    if (!Resource_GetId(destination, destinationId)) {
        return WDV_False;
    }

    /* Is source equal with destination? */
    if (memcmp(sourceId, destinationId, WDVCAPI_MAX_ID_LEN) == 0) {
        /* Nothing to do */
        return WDV_True;
    }

    /* Copy container source */
    if (!Container_Copy(wdv, sourceId, destinationId)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_CopyProperties( WDVCAPI_WDV       wdv,
                                  WDVCAPI_Resource  source,
                                  WDVCAPI_Resource  destination,
                                  WDV_Bool          withDisplayName )
{

    WDVCAPI_PropertyHandle  hProperty = NULL;
    WDVCAPI_Id              sourceId;
    WDVCAPI_Id              destinationId;

    /* Get ids */
    if (!Resource_GetId(source, sourceId)) {
        return WDV_False;
    }

    if (!Resource_GetId(destination, destinationId)) {
        return WDV_False;
    }

    /* Create property handle */
    if (!Property_CreateHandle(wdv, &hProperty)) {
        return WDV_False;
    }

    /* Copy all properties from source to destination */
    if (!Property_CopyAll(wdv, hProperty, sourceId, destinationId, withDisplayName)) {
        Property_DestroyHandle(wdv, hProperty);
        return WDV_False;
    }

    /* Destroy property handle */
    Property_DestroyHandle(wdv, hProperty);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetUriByChild( WDVCAPI_WDV       wdv,
                                 WDVCAPI_Resource  child,
				                 WDVCAPI_URI       uri )
{

	WDVCAPI_Resource        parentResource = NULL;
    WDVCAPI_Id              currentCId;
    WDVCAPI_URIString       tmpUri = "";
    WDVCAPI_URIString       newUri = "";
    WDV_Bool                rootFound;
    WDVCAPI_ResourceName    currentName;

	/* Checks */
	if (!wdv || !uri || !child ) {
		if ( wdv ) {
			Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                    WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		}

		return WDV_False;
	}

    /* Get child id */
    Resource_GetId(child, currentCId);

	/* Is cId root? */
    if (memcmp(currentCId, ID_ROOT, WDVCAPI_MAX_ID_LEN) == 0) {
        strcpy(uri, WDV_PATH_SEPARATOR_STR);

		return WDV_True;
	}

    /* Get child name */
    Resource_GetName(child, &currentName);

    /* Set return value */
    strcpy(newUri, currentName);

    /* Create resource handle */
    if (!Resource_CreateHandle(wdv, &parentResource)) {
        return WDV_False;
    }

    /* Get whole uri */
    rootFound = WDV_False;
    do {
        /* Get parent of cId */
        if (!Resource_GetParentByCId(wdv, currentCId, parentResource)) {
            /* Is parent root? */
            if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
                SAPDB_memcpy(currentCId, ID_ROOT, WDVCAPI_MAX_ID_LEN);
                strcpy(currentName, WDV_PATH_SEPARATOR_STR);
            } else {
                Resource_DestroyHandle(wdv, parentResource);
                return WDV_False;
            }
        } else {
            /* Get current cId */
            Resource_GetId(parentResource, currentCId);

            /* Get current name */
            Resource_GetName(parentResource, &currentName);
        }

        /* Build new uri */
        if (memcmp(currentCId, ID_ROOT, WDVCAPI_MAX_ID_LEN) == 0) {
            rootFound = WDV_True;
            sp77sprintf(tmpUri, WDV_MAX_URI_LEN, "%s%s", WDV_PATH_SEPARATOR_STR, newUri);
            strcpy(newUri, tmpUri);
        } else {
            sp77sprintf(tmpUri, WDV_MAX_URI_LEN, "%s%s%s", currentName, WDV_PATH_SEPARATOR_STR, newUri);
            strcpy(newUri, tmpUri);
        }
    } while (rootFound != WDV_True);

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, parentResource);

    /* Set return value */
    strcpy(uri, newUri);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetByCId( WDVCAPI_WDV         wdv,
                            WDVCAPI_Id          cId,
				            WDVCAPI_Resource    resource )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLHSTMT          statementHandle = 0;
	SQLCHAR           sqlStatement [256] = "";
	WDVCAPI_IdString  idString = "";

	/* Build SQL statement */
    WDVCAPI_IdAsString(cId, idString);
	sp77sprintf( sqlStatement, 256, SQL_SELECT_BY_CID, idString);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &statementHandle);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( statementHandle, sqlStatement, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(statementHandle, 1, SQL_C_BINARY,
                       resource->id, WDVCAPI_MAX_ID_LEN,
                       &resource->idIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
    	SQLFreeStmt(statementHandle, SQL_DROP);
		return WDV_False;
	}

    sqlRC = SQLBindCol(statementHandle, 2, SQL_C_ULONG,
                       &resource->state, sizeof(resource->state),
                       &resource->stateIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
    	SQLFreeStmt(statementHandle, SQL_DROP);
		return WDV_False;
	}

    sqlRC = SQLBindCol(statementHandle, 3, SQL_C_LONG,
                       &resource->lockDepth, sizeof(resource->lockDepth),
                       &resource->lockDepthIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
    	SQLFreeStmt(statementHandle, SQL_DROP);
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(statementHandle);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
    	SQLFreeStmt(statementHandle, SQL_DROP);
		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch( statementHandle );

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST,
                                    WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_DOESNT_EXIST);
        } else {
            Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
        }

		SQLFreeStmt(statementHandle, SQL_DROP);

		return WDV_False;
	}

	/* Free statement handle */
	SQLFreeStmt(statementHandle, SQL_DROP);

    /* Is there a lock? */
    if (resource->lockDepthIndicator == SQL_NULL_DATA) {
        resource->lockDepth = WDVCAPI_DEPTH_UNDEFINED;
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetParentByCId( WDVCAPI_WDV         wdv,
                                  WDVCAPI_Id          cId,
				                  WDVCAPI_Resource    parent )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLHSTMT          statementHandle = 0;
	SQLCHAR           sqlStatement [256] = "";
	WDVCAPI_IdString  idString = "";

	/* Build SQL statement */
    WDVCAPI_IdAsString(cId, idString);
	sp77sprintf( sqlStatement, 256, SQL_SELECT_PARENT_BY_CID, idString);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &statementHandle);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( statementHandle, sqlStatement, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(statementHandle, 1, SQL_C_BINARY,
                       parent->id, WDVCAPI_MAX_ID_LEN,
                       &parent->idIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

    sqlRC = SQLBindCol(statementHandle, 2, SQL_C_CHAR,
                       parent->name, WDV_MAX_RESOURCE_NAME_LEN,
                       &parent->nameIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(statementHandle);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch( statementHandle );

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST,
                                    WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_DOESNT_EXIST);
        } else {
            Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );

		    SQLFreeStmt(statementHandle, SQL_DROP);
        }

		return WDV_False;
	}

	/* Free statement handle */
	sqlRC = SQLFreeStmt(statementHandle, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_Rename( WDVCAPI_WDV           wdv,
                          WDVCAPI_Resource      resource,
                          WDVCAPI_URI           destinationUri,
                          WDVCAPI_ResourceName  newName,
                          WDVCAPI_Overwrite	    overwrite,
                          WDV_Bool             *destinationCreated )
{

	SQLRETURN             sqlRC = 0;
	SQLHDBC               hDBC = 0;
	SQLHSTMT              hStmt = 0;
    SQLCHAR               sqlStmt [1024] = "";
	WDVCAPI_IdString      resourceIdString = "";

	/* Checks */
	if (!wdv || !resource || !newName || !destinationCreated) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
	}

    /* Default rename. New URL does not exist */
    *destinationCreated = WDV_True;

    /* Get cid */
    if (!Resource_GetIdAsString(resource, resourceIdString)) {
        return WDV_False;
    }

	/* Build SQL statement */
	sp77sprintf(sqlStmt, 1024, "UPDATE WEBDAV_INODE SET Name = '%s' WHERE CId = X'%s'", newName, resourceIdString);

	/* Get DB handles */
	if (!GetDBC(wdv, &hDBC)) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_INVALID_HANDLE,
				                WDVCAPI_ERR_TEXT_INVALID_HANDLE );

		return WDV_False;
	}

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);

	/* Execute SQL */
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);

        /* Resource already exists? */
        if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_CODE_DUPLICATE_KEY)) {
            /* Shall we overwrite? */
            if (overwrite == WDV_True) {
                /* Delete existing resource to replace it */
                if (!Delete_Delete(wdv, destinationUri, NULL, WDV_False, WDV_False)) {
		            SQLFreeStmt(hStmt, SQL_DROP);

                    return WDV_False;
                } else {
                    /* Try to rename again */
	                sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	                if ( sqlRC != 0 ) {
                        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);
		                SQLFreeStmt(hStmt, SQL_DROP);

                        return WDV_False;
                    }

                    /* Destination URL already exists */
                    *destinationCreated = WDV_False;
                }
            } else {
		        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                        WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT,
				                        WDVCAPI_ERR_TEXT_OVERWRITE_CONFLICT);

		        SQLFreeStmt(hStmt, SQL_DROP);

		        return WDV_False;
            }
        } else {
		    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
        }
	}

	/* Close statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Update property diasplayname */
	sp77sprintf(sqlStmt, 1024, "UPDATE WEBDAV_Property SET Property_Short_Value = '%s' WHERE CId = X'%s' And Property_Id = X'"ID_PROPERTY_DISPLAYNAME_STRING"'", newName, resourceIdString);
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmt, sqlRC);
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
    }

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_SetState( WDVCAPI_WDV             wdv,
                            WDVCAPI_Id              resourceId,
                            WDVCAPI_ResourceState   resourceState )
{

	SQLRETURN           sqlRC = 0;
    SQLHDBC             hDBC = 0;
	SQLHSTMT            hStmt = 0;
	SQLCHAR             sqlStmt [1024] = "";
	WDVCAPI_IdString    resourceIdString = "";

    /* Get id strings */
	WDVCAPI_IdAsString(resourceId, resourceIdString);

	/* Build SQL statement */
	sp77sprintf(sqlStmt, 1024, SQL_SET_STATE, resourceState, resourceIdString);

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
		return WDV_False;
	}

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_CreateNullResource( WDVCAPI_WDV       wdv,
                                      WDVCAPI_URI       uri,
                                      WDVCAPI_Resource  newResource )
{

    WDVCAPI_URIString           parentUri = "";
    WDVCAPI_ResourceNameString  resourceName = "";
    WDVCAPI_Resource            parentResource = NULL;
    WDVCAPI_Id                  newId;

    /* Split uri into parent and name */
	SplitURI(wdv, uri, parentUri, resourceName);

    /* Create resource objects */
    if (!Resource_CreateHandle(wdv, &parentResource)) {
        return WDV_False;
    }

    /* Get parent resource */
    if (!Resource_GetByUri(wdv, parentUri, parentResource)) {
        Resource_DestroyHandle(wdv, parentResource);

        if (WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_PARENT_DOESNT_EXIST,
                                    WDVCAPI_ERR_TEXT_PARENT_DOESNT_EXIST);
        }

        return WDV_False;
    }

    /* The resource type of parent describes a collection, does it? */
    if (strcmp(parentResource->resourceType, PROPERTY_VALUE_COLLECTION) != 0) {
        /* Parent has to be a collection */
        Resource_DestroyHandle(wdv, parentResource);

        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION,
                           WDVCAPI_ERR_TEXT_PARENT_NOT_A_COLLECTION);

		return WDV_False;
    }

    /* Create inode entry with state null resource */
    if (!Resource_CreateInode(wdv, parentResource, resourceName, WDVCAPI_RESOURCE_STATE_NULL_RESOURCE, 0,
                              WDVCAPI_NO_OVERWRITE, newResource, NULL, WDV_False)) {
        return WDV_False;
    }

    /* Insert default properties */
    Resource_GetId(newResource, newId);

    /* Insert live properties */
    if (!Property_CreateLiveProperties(wdv, newId, resourceName, "null resource", "sapdbwww/nullresource", "0", WDV_False)) {
		return WDV_False;
    }
    if (!Property_CreateDefaultProperties(wdv, newId, WDV_False)) {
        return WDV_False;
    }

    Resource_DestroyHandle(wdv, parentResource);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_CreateHandleCreateInode( WDVCAPI_WDV                        wdv,
                                           WDVCAPI_ResourceHandleCreateInode *hResourceCreateInode )
{

	WDV_Bool            ok = WDV_False;

	if (!wdv || !hResourceCreateInode) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

		return WDV_False;
	}

	WDVCAPI_Allocat(sizeof(struct st_resource_handle_create_inode),
                    (WDV_UInt1**) hResourceCreateInode, &ok);
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init resource handle create inode */
	Resource_InitHandleCreateInode(wdv, *hResourceCreateInode);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_DestroyHandleCreateInode( WDVCAPI_WDV                       wdv,
                                            WDVCAPI_ResourceHandleCreateInode hResourceCreateInode )
{

	if (!wdv || !hResourceCreateInode) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Free statement handles */
    if (hResourceCreateInode->hStmt) {
        SQLFreeStmt(hResourceCreateInode->hStmt, SQL_DROP);
        hResourceCreateInode->hStmt = 0;
    }

    /* Free handle itself */
    WDVCAPI_Free((WDV_UInt1*) hResourceCreateInode);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_InitHandleCreateInode( WDVCAPI_WDV                       wdv,
                                         WDVCAPI_ResourceHandleCreateInode hResourceCreateInode )
{

    hResourceCreateInode->hStmt = 0;

    memset(hResourceCreateInode->parentId, 0, WDVCAPI_MAX_ID_LEN);
    hResourceCreateInode->parentIdIndicator = WDVCAPI_MAX_ID_LEN;

    memset(hResourceCreateInode->resourceId, 0, WDVCAPI_MAX_ID_LEN);
    hResourceCreateInode->resourceIdIndicator = WDVCAPI_MAX_ID_LEN;

    hResourceCreateInode->resourceState = WDVCAPI_RESOURCE_STATE_DEFAULT;
    hResourceCreateInode->resourceStateIndicator = sizeof(hResourceCreateInode->resourceStateIndicator);

    hResourceCreateInode->resourceName[0] = '\0';
    hResourceCreateInode->resourceNameIndicator = SQL_NTS;

    hResourceCreateInode->compressedLength = 0;
    hResourceCreateInode->compressedLengthIndicator = sizeof(hResourceCreateInode->compressedLengthIndicator);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_SetCompressedLength( WDVCAPI_WDV          wdv,
                                       WDVCAPI_Id           parentId,
                                       WDVCAPI_ResourceName name,
                                       WDV_UInt4            compressedLength )
{

    SQLRETURN   sqlRC = 0;

    /* Is statement already parsed? */
	if (!wdv->hStmtSetCompressedLength) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hStmtSetCompressedLength);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hStmtSetCompressedLength, sqlRC );

    	    SQLFreeStmt(wdv->hStmtSetCompressedLength, SQL_DROP);
            wdv->hStmtSetCompressedLength = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hStmtSetCompressedLength, (SQLCHAR *) SQL_SET_COMPRESSED_LENGTH, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hStmtSetCompressedLength, sqlRC );

    	    SQLFreeStmt(wdv->hStmtSetCompressedLength, SQL_DROP);
            wdv->hStmtSetCompressedLength = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->hStmtSetCompressedLength, 1,
                                 SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0,
                                 &wdv->compressedLength, sizeof(wdv->compressedLength),
                                 NULL);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetCompressedLength, sqlRC);

            SQLFreeStmt(wdv->hStmtSetCompressedLength, SQL_DROP);
            wdv->hStmtSetCompressedLength = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hStmtSetCompressedLength, 2, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->compressedParentId, WDVCAPI_MAX_ID_LEN,
                                 &wdv->compressedParentIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetCompressedLength, sqlRC );

		    SQLFreeStmt(wdv->hStmtSetCompressedLength, SQL_DROP);
		    wdv->hStmtSetCompressedLength = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hStmtSetCompressedLength, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 wdv->compressedName, WDV_MAX_RESOURCE_NAME_LEN,
                                 &wdv->compressedNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetCompressedLength, sqlRC );

		    SQLFreeStmt(wdv->hStmtSetCompressedLength, SQL_DROP);
		    wdv->hStmtSetCompressedLength = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
        SQLFreeStmt(wdv->hStmtSetCompressedLength, SQL_CLOSE);
    }

    /* Set value */
    wdv->compressedLength = compressedLength;
    SAPDB_memcpy(wdv->compressedParentId, parentId, WDVCAPI_MAX_ID_LEN);
    Common_StrMaxCopy(wdv->compressedName, name, WDV_MAX_RESOURCE_NAME_LEN);
    wdv->compressedNameIndicator = SQL_NTS;

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hStmtSetCompressedLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hStmtSetCompressedLength, sqlRC);
		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetCompressedLength( WDVCAPI_WDV           wdv,
                                       WDVCAPI_Id            parentId,
                                       WDVCAPI_ResourceName  name,
                                       WDV_UInt4            *compressedLength )
{

    SQLRETURN   sqlRC = 0;

    /* Is statement already parsed? */
	if (!wdv->hStmtGetCompressedLength) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hStmtGetCompressedLength);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hStmtGetCompressedLength, sqlRC );

    	    SQLFreeStmt(wdv->hStmtGetCompressedLength, SQL_DROP);
            wdv->hStmtGetCompressedLength = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(wdv->hStmtGetCompressedLength, (SQLCHAR *) SQL_GET_COMPRESSED_LENGTH, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, wdv->hStmtGetCompressedLength, sqlRC );

    	    SQLFreeStmt(wdv->hStmtGetCompressedLength, SQL_DROP);
            wdv->hStmtGetCompressedLength = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->hStmtGetCompressedLength, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 wdv->compressedParentId, WDVCAPI_MAX_ID_LEN,
                                 &wdv->compressedParentIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtGetCompressedLength, sqlRC );

		    SQLFreeStmt(wdv->hStmtGetCompressedLength, SQL_DROP);
		    wdv->hStmtGetCompressedLength = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hStmtGetCompressedLength, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 wdv->compressedName, WDV_MAX_RESOURCE_NAME_LEN,
                                 &wdv->compressedNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtGetCompressedLength, sqlRC );

		    SQLFreeStmt(wdv->hStmtGetCompressedLength, SQL_DROP);
		    wdv->hStmtGetCompressedLength = 0;

		    return WDV_False;
	    }

        /* Bind output parameter */
        sqlRC = SQLBindCol(wdv->hStmtGetCompressedLength, 1, SQL_C_ULONG,
                           &wdv->compressedLength, 0, &wdv->compressedLengthIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtGetCompressedLength, sqlRC);

            SQLFreeStmt(wdv->hStmtGetCompressedLength, SQL_DROP);
            wdv->hStmtGetCompressedLength = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
        SQLFreeStmt(wdv->hStmtGetCompressedLength, SQL_CLOSE);
    }

    /* Set value */
    SAPDB_memcpy(wdv->compressedParentId, parentId, WDVCAPI_MAX_ID_LEN);
    Common_StrMaxCopy(wdv->compressedName, name, WDV_MAX_RESOURCE_NAME_LEN);
    wdv->compressedNameIndicator = SQL_NTS;

	/* Execute SQL */
	sqlRC = SQLExecute(wdv->hStmtGetCompressedLength);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hStmtGetCompressedLength, sqlRC);
		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(wdv->hStmtGetCompressedLength);

	/* Error handling */
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hStmtGetCompressedLength, sqlRC);

		return WDV_False;
	}

    if (wdv->compressedLengthIndicator == -1) {
        /* No compression */
        *compressedLength = 0;
    } else {
        /* Get compressed length */
        *compressedLength = wdv->compressedLength;
    }

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Resource_InitHandle( WDVCAPI_WDV      wdv,
                              WDVCAPI_Resource resource )
{

    resource->hStmtRoot = 0;
    resource->hStmtParent = 0;
    resource->hStmtCollectChilds = 0;
    resource->hStmtCollectChildsWithProps = 0;

	SAPDB_memcpy(resource->parentId, ID_ROOT, WDVCAPI_MAX_ID_LEN);
    resource->parentIdIndicator = WDVCAPI_MAX_ID_LEN;

	SAPDB_memcpy(resource->id, ID_ROOT, WDVCAPI_MAX_ID_LEN);
    resource->idIndicator = WDVCAPI_MAX_ID_LEN;
	SAPDB_memcpy(resource->idWithProps, ID_ROOT, WDVCAPI_MAX_ID_LEN);
    resource->idWithPropsIndicator = WDVCAPI_MAX_ID_LEN;

	memset(resource->name, 0, WDV_MAX_RESOURCE_NAME_LEN);
    resource->nameIndicator = SQL_NTS;
	memset(resource->nameWithProps, 0, WDV_MAX_RESOURCE_NAME_LEN);
    resource->nameWithPropsIndicator = SQL_NTS;

    resource->state = WDVCAPI_RESOURCE_STATE_DEFAULT;
    resource->stateIndicator = sizeof(resource->stateIndicator);

    resource->compressedLength = 0;
    resource->compressedLengthIndicator = sizeof(resource->compressedLengthIndicator);

    memset(resource->resourceType, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
    resource->resourceTypeIndicator = WDV_MAX_PROPERTY_SHORT_VALUE_LEN;

    resource->lockDepth = WDVCAPI_DEPTH_UNDEFINED;
    resource->lockDepthIndicator = sizeof(resource->lockDepthIndicator);

	memset(resource->resourceTypeWithProps, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
    resource->resourceTypeWithPropsIndicator = WDV_MAX_PROPERTY_SHORT_VALUE_LEN;
	memset(resource->sizeWithProps, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
    resource->sizeWithPropsIndicator = WDV_MAX_PROPERTY_SHORT_VALUE_LEN;
	memset(resource->lastModifiedWithProps, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
    resource->lastModifiedWithPropsIndicator = WDV_MAX_PROPERTY_SHORT_VALUE_LEN;

    /* Create lock handle */
    if (!WDVCAPI_LockCreateHandle(wdv, &resource->currentLockDescription)) {
        return WDV_False;
    }

    /* XDB properties */
    resource->xdbDocId = 0;
    resource->xdbDocIdIndicator = 0;

    resource->nextFreeResource = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_ResetHandle( WDVCAPI_WDV      wdv,
                               WDVCAPI_Resource resource,
                               WDV_Bool         withStmtHandles )
{

    if (withStmtHandles == WDV_True) {
        if (resource->hStmtRoot) {
            SQLFreeStmt(resource->hStmtRoot, SQL_DROP);
            resource->hStmtRoot = 0;
        }

        if (resource->hStmtParent) {
            SQLFreeStmt(resource->hStmtParent, SQL_DROP);
            resource->hStmtParent = 0;
        }

        if (resource->hStmtCollectChilds) {
            SQLFreeStmt(resource->hStmtCollectChilds, SQL_DROP);
            resource->hStmtCollectChilds = 0;
        }

        if (resource->hStmtCollectChildsWithProps) {
            SQLFreeStmt(resource->hStmtCollectChildsWithProps, SQL_DROP);
            resource->hStmtCollectChildsWithProps = 0;
        }
    }

	SAPDB_memcpy(resource->parentId, ID_ROOT, WDVCAPI_MAX_ID_LEN);
	SAPDB_memcpy(resource->id, ID_ROOT, WDVCAPI_MAX_ID_LEN);
	SAPDB_memcpy(resource->idWithProps, ID_ROOT, WDVCAPI_MAX_ID_LEN);
	SAPDB_memcpy(resource->name, ID_ROOT, WDV_MAX_RESOURCE_NAME_LEN);
	SAPDB_memcpy(resource->nameWithProps, ID_ROOT, WDV_MAX_RESOURCE_NAME_LEN);

    resource->state = WDVCAPI_RESOURCE_STATE_DEFAULT;
    resource->compressedLength = 0;
    resource->lockDepth = WDVCAPI_DEPTH_UNDEFINED;

	memset(resource->resourceTypeWithProps, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
	memset(resource->sizeWithProps, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
	memset(resource->lastModifiedWithProps, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);

    /* Reset lock */
    if (!Lock_ResetHandle(wdv, resource->currentLockDescription)) {
        return WDV_False;
    }

    resource->nextFreeResource = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetByUriLocal( WDVCAPI_WDV        wdv,
				                 WDVCAPI_URI        uri,
				                 WDVCAPI_Resource   resource,
                                 WDVCAPI_LockHandle hLock )
{

	SQLRETURN                   sqlRC = 0;
	SQLHSTMT                    statementHandle = 0;
    SQLCHAR                     sqlStatement [256] = "";
	WDVCAPI_ResourceNameString  parentName = "";
	WDVCAPI_ResourceNameString  childName = "";
	WDVCAPI_Resource            parent = NULL;

	/* Checks */
	if (!wdv || !uri || !resource ) {
		if ( wdv ) {
			Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                    WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		}

		return WDV_False;
	}

    /* Is resource in the cache? */
    /*
    if (!Resource_CacheGetResource(wdv, uri, resource, hLock)) {
        return WDV_False;
    }
    */

    /* Inits */
    if (!hLock) {
        hLock = resource->currentLockDescription;
    }

	/* Is URI root? */
	if (!strcmp(uri, WDV_PATH_SEPARATOR_STR)) {
	    /* Get root resource by name */
        if (!Resource_GetByName( wdv, NULL, WDV_PATH_SEPARATOR_STR, resource )) {
            return WDV_False;
        }

		return WDV_True;
	}

	/* Split URI */
	SplitURI( wdv, uri, parentName, childName );
	if ( *parentName ) {
		/* Create parent resource */
		if (!Resource_CreateHandle(wdv, &parent)) {
			return WDV_False;
		}

		/* Get parent */
		if (!Resource_GetByUriLocal( wdv, parentName, parent, hLock )) {
			Resource_DestroyHandle(wdv, parent);
			return WDV_False;
		}

        /* Check if resource is null resource */
        if (Resource_IsNullResource(parent)) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST,
                                    WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_DOESNT_EXIST);

			Resource_DestroyHandle(wdv, parent);

			return WDV_False;
        }

        /* Check if there is an lock */
        if (Resource_IsLocked(parent, NULL)) {
            Lock_GetLockDescription(wdv, parent, hLock);
        }

		/* Get resource by name */
		if (!Resource_GetByName( wdv, parent, childName, resource )) {
			Resource_DestroyHandle(wdv, parent);
			return WDV_False;
		}

		/* Destroy parent resource */
		Resource_DestroyHandle(wdv, parent);

		return WDV_True;
	}

	/* Get root resource by name */
    if (!Resource_GetByName( wdv, NULL, childName, resource )) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_InitCache( WDVCAPI_ResourceCache  resourceCache )
{

	if (!resourceCache) {
		return WDV_False;
	}

    resourceCache->firstCacheItem = NULL;
    resourceCache->lastCacheItem = NULL;

    resourceCache->lastSearchedCacheItem = NULL;

    resourceCache->listCount = 0;

	return WDV_True;

}
/*===========================================================================*/

WDV_Bool Resource_CreateCacheItem( WDVCAPI_WDV          wdv,
                                   ResourceCacheItem   *resourceCacheItem )
{

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_DestroyCacheItem( WDVCAPI_WDV         wdv,
                                    ResourceCacheItem   resourceCacheItem )
{

	if (!resourceCacheItem) {
		return WDV_False;
	}

    /* Destroy cache item itself */
    WDVCAPI_Free((WDV_UInt1*) resourceCacheItem);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_AddCacheItem( WDVCAPI_WDV                 wdv,
                                WDVCAPI_URI                 uri,
                                WDVCAPI_Id                  parentId,
                                WDVCAPI_ResourceNameString  resourceName )
{

    WDVCAPI_ResourceCache   resourceCache = NULL;
    ResourceCacheItem       newCacheItem = NULL;

    /* Get cache handle */
    WDV_GetCacheHandle(wdv, &resourceCache);
    if (!resourceCache) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Create new cache item */
    if (!Resource_CreateCacheItem(wdv, &newCacheItem)) {
        return WDV_False;
    }

    /* Init new cache item */
    Common_StrMaxCopy(newCacheItem->uri, uri, WDV_MAX_URI_LEN);
    SAPDB_memcpy(newCacheItem->parentId, parentId, WDVCAPI_MAX_ID_LEN);
    Common_StrMaxCopy(newCacheItem->resourceName, resourceName, WDV_MAX_RESOURCE_NAME_LEN);

    newCacheItem->nextCacheItem = NULL;

    /* Append cache item to list */
    if (!resourceCache->firstCacheItem) {
        resourceCache->firstCacheItem = newCacheItem;
    }

    resourceCache->lastCacheItem->nextCacheItem = newCacheItem;
    resourceCache->lastCacheItem = newCacheItem->nextCacheItem;

    /* Increase list counter */
    resourceCache->listCount++;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_AddResourceToFreeList( WDVCAPI_WDV       wdv,
                                         WDVCAPI_Resource *resourceFreeList,
                                         WDVCAPI_Resource  resource )
{

    TRACE_OUT(">> Resource_AddResourceToFreeList()\n");

    if (!wdv || !resourceFreeList || !resource) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    resource->nextFreeResource = *resourceFreeList;
    *resourceFreeList = resource;

    TRACE_OUT(">> Resource_AddResourceToFreeList()\n");

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_GetResourceFromFreeList( WDVCAPI_WDV       wdv,
                                           WDVCAPI_Resource *resourceFreeList,
                                           WDVCAPI_Resource *resource )
{

    TRACE_OUT(">> Resource_GetResourceFromFreeList()\n");

    if (!wdv || !resourceFreeList || !resource) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    *resource = *resourceFreeList;

    if (*resourceFreeList) {
        *resourceFreeList = (*resourceFreeList)->nextFreeResource;
    }

    /* Reset resource */
    if (*resource) {
        #ifdef WDVCAPI_MEMCHECK
        cacheCount--;
        #endif

        (*resource)->nextFreeResource = NULL;
        if (!Resource_ResetHandle(wdv, *resource, WDV_False)) {
            return WDV_False;
        }
    }

    TRACE_OUT("<< Resource_GetResourceFromFreeList()\n");

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_DestroyFreeListItem( WDVCAPI_WDV      wdv,
                                       WDVCAPI_Resource resource )
{

	if ( !resource ) {
		return WDV_False;
	}

    if (resource->hStmtRoot) {
        SQLFreeStmt(resource->hStmtRoot, SQL_DROP);
    }

    if (resource->hStmtParent) {
        SQLFreeStmt(resource->hStmtParent, SQL_DROP);
    }

    if (resource->hStmtCollectChilds) {
        SQLFreeStmt(resource->hStmtCollectChilds, SQL_DROP);
    }

    if (resource->hStmtCollectChildsWithProps) {
        SQLFreeStmt(resource->hStmtCollectChildsWithProps, SQL_DROP);
    }

    /* Destroy lock handle */
    WDVCAPI_LockDestroyHandle(wdv, resource->currentLockDescription);

    WDVCAPI_Free((WDV_UInt1*) resource );

    #ifdef WDVCAPI_MEMCHECK
    allocCount--;
    #endif

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Resource_SetParent( WDVCAPI_WDV            wdv,
                             WDVCAPI_Resource       parentResource,
                             WDVCAPI_Resource       resource,
                             WDVCAPI_ResourceName   resourceName )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";
    
    /* Is statement prepared? */
    if (wdv->hStmtSetParent == 0) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(wdv->hDBC, &wdv->hStmtSetParent);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetParent, sqlRC);

		    return WDV_False;
	    }

        /* Prepare statement */
        sqlRC = SQLPrepare(wdv->hStmtSetParent, (SQLCHAR*)SQL_RESOURCE_SET_PARENT, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetParent, sqlRC);

            SQLFreeStmt(wdv->hStmtSetParent, SQL_DROP);
            wdv->hStmtSetParent = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter(wdv->hStmtSetParent, 1,
                                SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                wdv->setParentParentId, WDVCAPI_MAX_ID_LEN, &wdv->setParentParentIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetParent, sqlRC);

            SQLFreeStmt(wdv->hStmtSetParent, SQL_DROP);
            wdv->hStmtSetParent = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hStmtSetParent, 2,
                                SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                wdv->setParentResourceName, WDV_MAX_RESOURCE_NAME_LEN, &wdv->setParentResourceNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetParent, sqlRC);

            SQLFreeStmt(wdv->hStmtSetParent, SQL_DROP);
            wdv->hStmtSetParent = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(wdv->hStmtSetParent, 3,
                                SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                wdv->setParentResourceId, WDVCAPI_MAX_ID_LEN, &wdv->setParentResourceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, wdv->hStmtSetParent, sqlRC);

            SQLFreeStmt(wdv->hStmtSetParent, SQL_DROP);
            wdv->hStmtSetParent = 0;

		    return WDV_False;
	    }
    } else {
        sqlRC = SQLFreeStmt(wdv->hStmtSetParent, SQL_CLOSE);
    }

    /* Set input parameters */
    SAPDB_memcpy(wdv->setParentParentId, parentResource->id, WDVCAPI_MAX_ID_LEN);
    SAPDB_memcpy(wdv->setParentResourceId, resource->id, WDVCAPI_MAX_ID_LEN);
    Common_StrMaxCopy(wdv->setParentResourceName, resourceName, WDV_MAX_RESOURCE_NAME_LEN);
    wdv->setParentResourceNameIndicator = SQL_NTS;

    sqlRC = SQLExecute(wdv->hStmtSetParent);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, wdv->hStmtSetParent, sqlRC);

        SQLFreeStmt(wdv->hStmtSetParent, SQL_DROP);
        wdv->hStmtSetParent = 0;

        return WDV_False;
    }

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
