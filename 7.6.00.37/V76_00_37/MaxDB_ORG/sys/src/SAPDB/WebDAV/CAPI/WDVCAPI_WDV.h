/*!**********************************************************************

  module: WDVCAPI_WDV.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV handling

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

#ifndef WDVCAPI_WDV_H
#define WDVCAPI_WDV_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"

#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Get.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Put.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Delete.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Head.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_MkCol.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Property.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_XMLIndexing.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Container.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_wdvcapi_wdv {

    SQLHENV                                 hEnv;
    SQLHDBC                                 hDBC;

    WDVCAPI_AdminHandle                     hAdmin;

    WDVCAPI_Resource                        resourceFreeList;
    WDVCAPI_LockHandle                      lockFreeList;
    WDVCAPI_ContainerHandle                 hContainerFreeList;

    XMLIMAPI_Handle                        hXmlModelling;
    WDVCAPI_XmlDocClass                     hXmlDocClassList;

    WDVCAPI_GetHandle                       hGet;
    WDVCAPI_PutHandle                       hPut;
    WDVCAPI_DeleteHandle                    hDelete;
    WDVCAPI_HeadHandle                      hHead;

    WDVCAPI_MkColHandle                     hMkCol;

    WDVCAPI_PropertyHandleSetXmlIndex       hPropertySetXmlIndex;
    WDVCAPI_PropertyHandleSetShortValue     hPropertySetShortValue;
    WDVCAPI_PropertyHandleGetShortValue     hPropertyGetShortValue;
    WDVCAPI_PropertyHandleSetLastModified   hPropertySetLastModified;
    WDVCAPI_PropertyHandleUpdateShortValue  hPropertyUpdateShortValue;
    WDVCAPI_PropertyHandleSetCreationDate   hPropertySetCreationDate;

	WDVCAPI_ResourceHandleCreateInode       hResourceCreateInode;

    SQLHSTMT                                hEndTransaction;
    SQLHSTMT                                hCancelTransaction;

    SQLHSTMT                                hEndSubTransaction;
    SQLHSTMT                                hStartSubTransaction;
    SQLHSTMT                                hCancelSubTransaction;

    SQLHSTMT                                hStmtPropertyDeleteAllIndexValues;
    WDVCAPI_Id                              propertyDeleteAllIndexValuesResourceId;
    SQLLEN                                  propertyDeleteAllIndexValuesResourceIdIndicator;

    SQLHSTMT                                hStmtSetCompressedLength;
    SQLHSTMT                                hStmtGetCompressedLength;
    WDV_UInt4                               compressedLength;
    SQLLEN                                  compressedLengthIndicator;
    WDVCAPI_Id                              compressedParentId;
    SQLLEN                                  compressedParentIdIndicator;
    WDVCAPI_ResourceNameString              compressedName;
    SQLLEN                                  compressedNameIndicator;

    SQLHSTMT                                dcMgtStmtGetDocClassProperties;
    SQLHSTMT                                dcMgtStmtUpdateDocClass;
    SQLHSTMT                                dcMgtStmtInsertDocClass;
    SQLHSTMT                                dcMgtStmtUpdateIndexingState;
    SQLHSTMT                                dcMgtStmtInsertIndexingState;
    SQLHSTMT                                dcMgtStmtDeleteIndexValues;
    WDVCAPI_Id                              dcMgtResourceId;
    SQLLEN                                  dcMgtResourceIdIndicator;
    WDVCAPI_PropertyShortValueBuffer        dcMgtDocClassId;
    SQLLEN                                  dcMgtDocClassIdIndicator;
    WDVCAPI_PropertyShortValueBuffer        dcMgtIndexingState;
    SQLLEN                                  dcMgtIndexingStateIndicator;

    SQLHSTMT                                hStmtSetParent;
    WDVCAPI_Id                              setParentParentId;
    SQLLEN                                  setParentParentIdIndicator;
    WDVCAPI_Id                              setParentResourceId;
    SQLLEN                                  setParentResourceIdIndicator;
    WDVCAPI_ResourceNameString              setParentResourceName;
    SQLLEN                                  setParentResourceNameIndicator;

    SQLHSTMT                                propertyDeleteStmt;
    WDVCAPI_Id                              propertyDeleteResourceId;
    SQLLEN                                  propertyDeleteResourceIdIndicator;
    WDVCAPI_Id                              propertyDeletePropertyId;
    SQLLEN                                  propertyDeletePropertyIdIndicator;

	WDVCAPI_ErrorList                       errorList;

};

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool WDV_SetDBEnv  ( WDVCAPI_WDV  wdv,
                         SQLHENV      hEnv );

WDV_Bool WDV_SetDBC    ( WDVCAPI_WDV  wdv,
                         SQLHDBC      hDBC );

WDV_Bool GetDBEnv( WDVCAPI_WDV  wdv,
                   SQLHENV     *hEnv );

WDV_Bool GetDBC( WDVCAPI_WDV  wdv,
                 SQLHDBC     *hDBC );

WDV_Bool SetPutHandle( WDVCAPI_WDV       wdv,
                       WDVCAPI_PutHandle hPut );

WDV_Bool GetPutHandle( WDVCAPI_WDV        wdv,
                       WDVCAPI_PutHandle *hPut );

WDV_Bool SetDeleteHandle( WDVCAPI_WDV           wdv,
                          WDVCAPI_DeleteHandle  hDelete );

WDV_Bool GetDeleteHandle( WDVCAPI_WDV           wdv,
                          WDVCAPI_DeleteHandle *hDelete );

WDV_Bool WDV_SetHeadHandle( WDVCAPI_WDV         wdv,
                            WDVCAPI_HeadHandle  hHead );

WDV_Bool WDV_GetHeadHandle( WDVCAPI_WDV         wdv,
                            WDVCAPI_HeadHandle *hHead );

void SetErrorList( WDVCAPI_WDV        wdv,
                   WDVCAPI_ErrorList  errorList );

void GetErrorList( WDVCAPI_WDV        wdv,
                   WDVCAPI_ErrorList *errorList );

void SetGetHandle( WDVCAPI_WDV        wdv,
                   WDVCAPI_GetHandle  hGet );

void GetGetHandle( WDVCAPI_WDV        wdv,
                   WDVCAPI_GetHandle *hGet );

/* Transaction management */
WDV_Bool WDV_EndTransaction( WDVCAPI_WDV  wdv );
WDV_Bool WDV_StartTransaction( WDVCAPI_WDV  wdv );
#define WDV_CancelTransaction(p)    WDV_StartTransaction(p)

WDV_Bool WDV_EndSubTransaction( WDVCAPI_WDV  wdv );
WDV_Bool WDV_StartSubTransaction( WDVCAPI_WDV  wdv );
WDV_Bool WDV_CancelSubTransaction( WDVCAPI_WDV  wdv );

/* MkCol handles */
void WDV_GetHandleMkCol( WDVCAPI_WDV          wdv,
                         WDVCAPI_MkColHandle *hMkCol );

/* Property handles */
void WDV_GetHandlePropertySetShortValue( WDVCAPI_WDV                          wdv,
                                         WDVCAPI_PropertyHandleSetShortValue *hPropertySetShortValue );

void WDV_GetHandlePropertyGetShortValue( WDVCAPI_WDV                          wdv,
                                         WDVCAPI_PropertyHandleGetShortValue *hPropertyGetShortValue );

void WDV_GetHandlePropertySetLastModified( WDVCAPI_WDV                            wdv,
                                           WDVCAPI_PropertyHandleSetLastModified *hPropertySetLastModified );

void WDV_GetHandlePropertyUpdateShortValue( WDVCAPI_WDV                             wdv,
                                            WDVCAPI_PropertyHandleUpdateShortValue *hPropertyUpdateShortValue );

/* Resource handles */
void WDV_GetResourceFreeList( WDVCAPI_WDV        wdv,
                              WDVCAPI_Resource **resourceFreeList );

void WDV_GetCacheHandle( WDVCAPI_WDV            wdv, 
                         WDVCAPI_ResourceCache *resourceCache );

/* Lock handles */
void WDV_GetLockFreeList( WDVCAPI_WDV          wdv,
                          WDVCAPI_LockHandle **lockFreeList );

/***********************************************************************

	End

 ***********************************************************************/

#endif
