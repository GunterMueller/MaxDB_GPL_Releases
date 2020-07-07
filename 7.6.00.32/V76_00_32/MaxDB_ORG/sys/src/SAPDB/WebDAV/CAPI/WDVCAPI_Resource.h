/*!**********************************************************************

  module: WDVCAPI_Resource.h

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

#ifndef WDVCAPI_RESOURCE_H
#define WDVCAPI_RESOURCE_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_resource_handle_create_inode  *WDVCAPI_ResourceHandleCreateInode;

typedef struct st_resource                      *WDVCAPI_Resource;

struct st_resource {

    SQLHSTMT                    hStmtRoot;
    SQLHSTMT                    hStmtParent;

    SQLHSTMT                    hStmtCollectChilds;
    SQLHSTMT                    hStmtCollectChildsWithProps;

    WDVCAPI_Id                  parentId;
    SQLLEN                      parentIdIndicator;

    WDVCAPI_Id                  id;
    SQLLEN                      idIndicator;
    WDVCAPI_Id                  idWithProps;
    SQLLEN                      idWithPropsIndicator;

    WDVCAPI_ResourceNameString  name;
    SQLLEN                      nameIndicator;
    WDVCAPI_ResourceNameString  nameWithProps;
    SQLLEN                      nameWithPropsIndicator;

    WDVCAPI_ResourceState       state;
    SQLLEN                      stateIndicator;

    WDV_UInt4                   compressedLength;
    SQLLEN                      compressedLengthIndicator;

    WDVCAPI_PropertyShortValueBuffer    resourceType;
    SQLLEN                              resourceTypeIndicator;

    WDVCAPI_Depth               lockDepth;
    SQLLEN                      lockDepthIndicator;

    WDVCAPI_PropertyShortValueBuffer    resourceTypeWithProps;
    SQLLEN                              resourceTypeWithPropsIndicator;
    WDVCAPI_PropertyShortValueBuffer    sizeWithProps;
    SQLLEN                              sizeWithPropsIndicator;
    WDVCAPI_PropertyShortValueBuffer    lastModifiedWithProps;
    SQLLEN                              lastModifiedWithPropsIndicator;

    WDVCAPI_LockHandle          currentLockDescription;

    SAPDB_UInt8                 xdbDocId;
    SQLLEN                      xdbDocIdIndicator;

    WDVCAPI_Resource            nextFreeResource;
};

/*---------------------------------------------------------------------------*/

typedef struct st_resource_cache    *WDVCAPI_ResourceCache;

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Resource_CreateHandle( WDVCAPI_WDV        wdv,
                                WDVCAPI_Resource  *resource );

WDV_Bool Resource_DestroyHandle( WDVCAPI_WDV        wdv,
                                 WDVCAPI_Resource   resource );

WDV_Bool Resource_DestroyFreeList( WDVCAPI_WDV      wdv,
                                   WDVCAPI_Resource resource );

WDV_Bool Resource_GetByUri( WDVCAPI_WDV       wdv,
				            WDVCAPI_URI       uri,
				            WDVCAPI_Resource  resource );

WDV_Bool Resource_GetByName( WDVCAPI_WDV         wdv,
				             WDVCAPI_Resource    parent,
				             WDVCAPI_URI         name,
				             WDVCAPI_Resource    resource );

WDV_Bool Resource_GetIdAsString( WDVCAPI_Resource resource,
                                 WDVCAPI_IdString idString );

WDV_Bool Resource_GetId( WDVCAPI_Resource  resource,
                         WDVCAPI_Id        id );

WDV_Bool Resource_IsRoot( WDVCAPI_Resource  resource );

WDV_Bool Resource_IdIsRoot( WDVCAPI_Id  id );
WDV_Bool Resource_IdIsDeletedItems( WDVCAPI_Id  id );

WDV_Bool Resource_GetName( WDVCAPI_Resource      resource,
                           WDVCAPI_ResourceName *name );

WDV_Bool Resource_CollectChildsWithProps( WDVCAPI_WDV                 wdv,
                                          WDVCAPI_Resource            parentResource,
                                          WDVCAPI_Resource            childResource,
                                          WDV_Bool                   *isXDBDoc );

WDV_Bool Resource_GetNextFromCollectionWithProps( WDVCAPI_WDV                 wdv,
                                                  WDVCAPI_Resource            parentResource,
                                                  WDV_Bool                   *childFound );

WDV_Bool Resource_CollectChilds( WDVCAPI_WDV                 wdv,
                                 WDVCAPI_Resource            parentResource,
                                 WDVCAPI_Resource            childResource,
                                 WDV_Bool                   *childFound );

WDV_Bool Resource_GetNextFromCollection( WDVCAPI_WDV                 wdv,
                                         WDVCAPI_Resource            parentResource,
                                         WDVCAPI_Resource            childResource,
                                         WDV_Bool                   *childFound );

WDV_Bool Resource_CreateInode( WDVCAPI_WDV            wdv,
                               WDVCAPI_Resource       parent,
                               WDVCAPI_ResourceName   resourceName,
                               WDVCAPI_ResourceState  resourceState,
                               WDV_UInt4              compressedLength,
                               WDVCAPI_Overwrite      overwrite,
                               WDVCAPI_Resource       newResource,
                               WDV_Bool              *resourceAlreadyExists,
                               WDV_Bool               withDisplayname );

WDV_Bool Resource_CopyContainer( WDVCAPI_WDV        wdv,
                                 WDVCAPI_Resource   source,
                                 WDVCAPI_Resource   destination );

WDV_Bool Resource_CopyProperties( WDVCAPI_WDV       wdv,
                                  WDVCAPI_Resource  source,
                                  WDVCAPI_Resource  destination,
                                  WDV_Bool          withDisplayName );

WDV_Bool Resource_GetUriByChild( WDVCAPI_WDV       wdv,
                                 WDVCAPI_Resource  child,
				                 WDVCAPI_URI       uri );

WDV_Bool Resource_GetParentByCId( WDVCAPI_WDV         wdv,
                                  WDVCAPI_Id          cId,
				                  WDVCAPI_Resource    parent );

WDV_Bool Resource_GetByCId( WDVCAPI_WDV         wdv,
                            WDVCAPI_Id          cId,
				            WDVCAPI_Resource    resource );

WDV_Bool Resource_Rename( WDVCAPI_WDV           wdv,
                          WDVCAPI_Resource      resource,
                          WDVCAPI_URI           destinationUri,
                          WDVCAPI_ResourceName  newName,
                          WDVCAPI_Overwrite	    overwrite,
                          WDV_Bool             *destinationCreated );

WDV_Bool Resource_IsLocked( WDVCAPI_Resource    resource,
                            WDVCAPI_Depth      *lockDepth );

WDV_Bool Resource_ClearLockDescription( WDVCAPI_WDV         wdv,
				                        WDVCAPI_Resource    resource );

WDV_Bool Resource_CreateNullResource( WDVCAPI_WDV       wdv,
                                      WDVCAPI_URI       uri,
                                      WDVCAPI_Resource  resource );

WDV_Bool Resource_IsNullResource( WDVCAPI_Resource    resource );

WDV_Bool Resource_SetState( WDVCAPI_WDV             wdv,
                            WDVCAPI_Id              resourceId,
                            WDVCAPI_ResourceState   resourceState );

WDV_Bool Resource_IsLockInURI( WDVCAPI_WDV          wdv,
				               WDVCAPI_Resource     resource,
                               WDVCAPI_LockHandle   hLock );

WDV_Bool Resource_CreateHandleCreateInode( WDVCAPI_WDV                        wdv,
                                           WDVCAPI_ResourceHandleCreateInode *hResourceCreateInode );

WDV_Bool Resource_DestroyHandleCreateInode( WDVCAPI_WDV                       wdv,
                                            WDVCAPI_ResourceHandleCreateInode hResourceCreateInode );

WDV_Bool Resource_SetCompressedLength( WDVCAPI_WDV          wdv,
                                       WDVCAPI_Id           parentId,
                                       WDVCAPI_ResourceName name,
                                       WDV_UInt4            compressedLength );

WDV_Bool Resource_GetCompressedLength( WDVCAPI_WDV           wdv,
                                       WDVCAPI_Id            parentId,
                                       WDVCAPI_ResourceName  name,
                                       WDV_UInt4            *compressedLength );

WDV_Bool Resource_SetParent( WDVCAPI_WDV            wdv,
                             WDVCAPI_Resource       parentResource,
                             WDVCAPI_Resource       resource,
                             WDVCAPI_ResourceName   resourceName );

/***********************************************************************

	End

 ***********************************************************************/

#endif
