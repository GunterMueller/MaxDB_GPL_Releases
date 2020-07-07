/*!**********************************************************************

  module: WDVCAPI_Put.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Put

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

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

#include "SAPDB/WebDAV/CAPI/WDVCAPI_Put.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Property.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_HandleManager.h"
#define LC_STREAMS_ENABLED
#include "livecachetypes.h" /* no check */
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_INSERT_CONTAINER    "INSERT INTO WEBDAV_Container SET CId = ?, Short_Content = ?, Content = ? UPDATE DUPLICATES"
#define SQL_INSERT_RESOURCE     "INSERT INTO WEBDAV_Inode SET PId = ?, CId = ?, Name = ?"
#define SQL_SELECT_BY_NAME      "SELECT CId FROM WEBDAV_Inode WHERE PId = ? AND Name = ?"
#define SQL_INSERT_INODE        "INSERT INTO WEBDAV_Inode SET PId = ?, Name = ?"

#define SQL_CALL_DBPROC_XML_PUT "CALL XDB_PUT(?,?)"
#define SQL_UPDATE_XDB_DOCID    "UPDATE WEBDAV_Inode SET XDBDocId = ? WHERE PId = ? AND CId = ?"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool InitPutHandle( WDVCAPI_WDV         wdv,
                        WDVCAPI_PutHandle   hput );

WDV_Bool ResourceCreatePath( WDVCAPI_WDV        wdv,
                             WDVCAPI_PutHandle  hPut,
                             WDVCAPI_URI        uri,
                             WDVCAPI_Id         id );

WDV_Bool ResourceInsertCollection( WDVCAPI_WDV                  wdv,
                                   WDVCAPI_PutHandle            hPut,
                                   WDVCAPI_Id                   pid,
					               WDVCAPI_ResourceNameString   name,
                                   WDVCAPI_Id                   cid );

WDV_Bool ResourceInsertResource( WDVCAPI_WDV                wdv,
                                 WDVCAPI_PutHandle          hPut );

WDV_Bool ResourceGetByName( WDVCAPI_WDV                 wdv,
                            WDVCAPI_PutHandle           hPut,
				            WDVCAPI_Id                  pid,
				            WDVCAPI_ResourceNameString  name,
				            WDVCAPI_Id                  cid );
    
WDV_Bool OpenContainer( WDVCAPI_WDV         wdv,
                        WDVCAPI_PutHandle   hPut );

WDV_Bool WriteContainer( WDVCAPI_WDV            wdv,
                         WDVCAPI_PutHandle      hPut,
                         WDVCAPI_Buffer         buffer,
                         WDVCAPI_BufferLength   bufferLength );

WDV_Bool CloseContainer( WDVCAPI_WDV        wdv,
                         WDVCAPI_PutHandle  hPut );

WDV_Bool CloseXMLContainer( WDVCAPI_WDV        wdv,
                            WDVCAPI_PutHandle  hPut );

WDV_Bool UpdateResourceProperties( WDVCAPI_WDV           wdv,
                                   WDVCAPI_PutHandle     hPut );

WDV_Bool Put_OpenIndexing( WDVCAPI_WDV           wdv,
                           WDVCAPI_PutHandle     hPut,
                           WDVCAPI_XmlDocClassId xmlDocClassId );

WDV_Bool Put_Indexing( WDVCAPI_WDV          wdv,
                       WDVCAPI_PutHandle    hPut,
                       WDVCAPI_Buffer       buffer,
                       WDVCAPI_BufferLength bufferLength );

WDV_Bool Put_CloseIndexing( WDVCAPI_WDV       wdv,
                            WDVCAPI_PutHandle hPut );

WDV_Bool Put_WriteCompressedData( WDVCAPI_WDV             wdv,
                                  WDVCAPI_PutHandle       hPut,
                                  WDVCAPI_Buffer          buffer,
                                  WDVCAPI_BufferLength    bufferLength );

int ReadStream( SQL_LC_StreamParm *StreamParam, void *rgbOutStream, int cbStreamMax, int *pcbStreamLen, int *pcbNoOfRowsRead, void* pMask );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_PutOpen( WDVCAPI_WDV           wdv,
                          WDVCAPI_URI           uri,
                          WDVCAPI_ContentType   contentType,
                          WDVCAPI_XmlDocClassId docClassId,
                          WDV_Bool              asyncIndexing,
                          WDVCAPI_LockIdString  lockIdString,
                          WDV_Bool              compress,
                          WDV_Bool              isDocForXDB,
                          WDVCAPI_InStream      inStream,
                          void                 *inStreamUserData,
                          WDV_Bool             *uriCreated )
{

    WDVCAPI_PutHandle                   hPut = NULL;
    WDVCAPI_URIString                   path = "";
    WDVCAPI_ResourceNameString          name = "";
    WDVCAPI_LockId                      lockId;
    WDVCAPI_LockHandle                  hLock = NULL;
	WDVCAPI_Resource                    newResource = NULL;
	WDVCAPI_Resource                    parentResource = NULL;
    WDV_Bool                            resourceNotFound = WDV_False;

    /* Checks */
    if (!wdv || !uri) {
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

    /* Inits */
    if (!lockIdString) {
        WDVCAPI_IdInitValue(lockId);
    } else {
        WDVCAPI_IdStringAsId(lockIdString, lockId);
    }

    /* Get put handle */
    if (!GetPutHandle(wdv, &hPut)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Init XML indexing description in handle hPut */
    hPut->hXmlDocClass = NULL;

    /* Split path from name */
	SplitURI(wdv, uri, path, name);

    /* Store name for later use */
    strncpy(hPut->name, name, WDV_MAX_RESOURCE_NAME_LEN);
    hPut->name[WDV_MAX_RESOURCE_NAME_LEN] = '\0';

    /* Store content type for later use */
    strncpy(hPut->contentTypeString, contentType, WDVCAPI_MAX_CONTENT_TYPE_LEN);
    hPut->contentTypeString[WDVCAPI_MAX_CONTENT_TYPE_LEN] = '\0';

	/* Get resource from URI */
	if (!Resource_CreateHandle(wdv, &newResource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

	if(!Resource_GetByUri( wdv, uri, newResource )) {
        /* Is there an error or could we just not found the resource? */
		if(!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
			Resource_DestroyHandle(wdv, newResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

			return WDV_False;
		}

        resourceNotFound = WDV_True;
    } else {
        /* The resource type describes a collection, does it? */
        if (strcmp(newResource->resourceType, PROPERTY_VALUE_COLLECTION) == 0) {
			Resource_DestroyHandle(wdv, newResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_NO_PUT_ON_COLLECTION,
                               WDVCAPI_ERR_TEXT_NO_PUT_ON_COLLECTION);

			return WDV_False;
        }
    }

    /* Is new resource or something in the uri already locked? */
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

    /* Determine parent resource */
	if (!Resource_CreateHandle(wdv, &parentResource)) {
		Resource_DestroyHandle(wdv, newResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
	}

    if(!Resource_GetByUri( wdv, path, parentResource)) {
		if(WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
			Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_PARENT_DOESNT_EXIST,
				                WDVCAPI_ERR_TEXT_PARENT_DOESNT_EXIST );
		}

        Resource_DestroyHandle(wdv, parentResource);
		Resource_DestroyHandle(wdv, newResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* The resource type describes a collection, does it? */
    if (strcmp(parentResource->resourceType, PROPERTY_VALUE_COLLECTION) != 0) {
        /* Parent has to be a collection */
        Resource_DestroyHandle(wdv, parentResource);
		Resource_DestroyHandle(wdv, newResource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION,
                           WDVCAPI_ERR_TEXT_PARENT_NOT_A_COLLECTION);

		return WDV_False;
    }

    /* Get id of the resource */
    Resource_GetId(parentResource, hPut->pid);

    /* Have we create the path and the resource itself? */
    if (resourceNotFound == WDV_True) {
        /* Get cid if resource already exists, otherwise insert resource */
        if (!ResourceInsertResource(wdv, hPut)) {
            Resource_DestroyHandle(wdv, parentResource);
			Resource_DestroyHandle(wdv, newResource);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    } else {
        /* Get id of the resource */
        Resource_GetId(newResource, hPut->cid);

        if (Resource_IsNullResource(newResource)) {
            /* Set resource state to default. The resource state could be set */
            /* to null resource, if the resource is created by a lock         */
            if (!Resource_SetState(wdv, hPut->cid, WDVCAPI_RESOURCE_STATE_DEFAULT)) {
                Resource_DestroyHandle(wdv, parentResource);
			    Resource_DestroyHandle(wdv, newResource);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }

            /* Update property resource type */
            if (!Property_UpdateShortValue(wdv, hPut->cid, ID_PROPERTY_RESOURCE_TYPE, "")) {
                Resource_DestroyHandle(wdv, parentResource);
			    Resource_DestroyHandle(wdv, newResource);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }

            /* There is no need to update getcontenttype property at this place */
            /* because the getcontentlength and getcontenttype will be set with */
            /* WDVCAPI_PutClose                                                 */
        }
    }

    /* Drop the resource handle */
    Resource_DestroyHandle(wdv, parentResource);
	Resource_DestroyHandle(wdv, newResource);

    /* Open container */
    if (isDocForXDB == WDV_False) {
        /* Document is not an XML document */
        hPut->isDocForXDB = WDV_False;

        if (!OpenContainer(wdv, hPut)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* XML indexing requested? */
        if (docClassId && !WDVCAPI_IdIsInitialValue(docClassId)) {
            /* Delete all existing index entries */
            if (!Property_DeleteAllIndexValues(wdv, newResource->id)) {
                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }

            /* Open indexing */
            hPut->asyncIndexing = asyncIndexing;
            if (!Put_OpenIndexing(wdv, hPut, docClassId)) {
                /* Close container */
                CloseContainer(wdv, hPut);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        }

        /* Store compress info */
        hPut->compress = compress;
    } else {
        /* Document is an XML document */
        hPut->isDocForXDB = WDV_True;

        if (!inStream) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

		    return WDV_False;
        }

        hPut->inStream = inStream;
        hPut->inStreamUserData = inStreamUserData;
    }

    /* Set return value */
    *uriCreated = resourceNotFound;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_Put( WDVCAPI_WDV           wdv,
                      WDVCAPI_Buffer        buffer,
                      WDVCAPI_BufferLength  bufferLength )
{

    WDVCAPI_PutHandle   hPut = NULL;

    /* Checks */
    if (!wdv || !buffer) {
		if ( wdv ) {
			Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                    WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		}

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
    }

/*    if (bufferLength < 1) {
        return WDV_True;
    }
*/
    /* Get put handle */
    if (!GetPutHandle(wdv, &hPut)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Write container */
    if (hPut->isDocForXDB == WDV_True) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        WDV_CancelTransaction(wdv);

        return WDV_False;
    } else {
        if (!WriteContainer(wdv, hPut, buffer, bufferLength)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Is there something to index */
        if (hPut->hXmlDocClass && hPut->asyncIndexing == WDV_False) {
            /* XML indexing */
            if (!Put_Indexing(wdv, hPut, buffer, bufferLength)) {
			    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_INDEXING_FAILED,
				                WDVCAPI_ERR_TEXT_INDEXING_FAILED);

                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PutXDB( WDVCAPI_WDV    wdv )
{

	SQLRETURN                sqlRC = 0;
    SQLLEN                   indicator = 0;
    SQLStreamDesc            streamDesc;
    SQL_LC_StreamParm       *StreamParam;
    WDV_Bool                 ok = WDV_False;
    WDVCAPI_PutHandle        hPut = wdv->hPut;
    WDVCAPI_Handle           hdl = 0;

    if (!wdv) {
        return WDV_False;
    }

    /* Prepare stream descriptor */
	WDVCAPI_Allocat(sizeof(SQL_LC_StreamParm) - sizeof (OmsTypeStreamMemberDesc) * 256 + sizeof (OmsTypeStreamMemberDesc) * 1, (WDV_UInt1**) &streamDesc.StreamParam, &ok ); /* Just one col desc */
	if (ok == WDV_False) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
	}

    StreamParam = streamDesc.StreamParam;
    if (StreamParam) {
      StreamParam->C_1.Stream.nilPointer          = (void*) 0;
      StreamParam->C_1.Stream.hStream.ABAPTabId   = 1;
      StreamParam->C_1.Stream.hStream.size        = 1;  /* One byte */
      StreamParam->C_1.Stream.hStream.memberCount = 1;
    }

    streamDesc.WriteProc = NULL;
    streamDesc.ReadProc  = &ReadStream;

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(wdv->hDBC, &hPut->hStmtXMLPut);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);
        hPut->hStmtXMLPut = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

    sqlRC = SQLPrepare(hPut->hStmtXMLPut, (SQLCHAR*)SQL_CALL_DBPROC_XML_PUT, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);
        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

    /* Bind stream parameter */
    HdlMan_NewHandle(&hdl, hPut);

    StreamParam->C_1.Stream.hStream.colDesc[0].memberType = STYPE_CHAR;
    StreamParam->C_1.Stream.hStream.colDesc[0].length     =  1;
    StreamParam->C_1.Stream.hStream.colDesc[0].offset     = 0;
    StreamParam->C_1.Stream.hStream.ABAPTabId             = hdl;

    sqlRC = SQLBindParameter (hPut->hStmtXMLPut, 1,
                              SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_ABAPHANDLE, 0, 0,
                              &streamDesc, sizeof(streamDesc), NULL);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);

        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
        HdlMan_FreeHandle(hdl);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

    /* Bind output parameters */
    sqlRC = SQLBindParameter(hPut->hStmtXMLPut, 2,
                                SQL_PARAM_OUTPUT, SQL_C_ULONG, SQL_BIGINT, 0, 0,
                                &hPut->xdbDocId, 0,
                                &indicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);

        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
        HdlMan_FreeHandle(hdl);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

    sqlRC = SQLExecute(hPut->hStmtXMLPut);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);

        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
        HdlMan_FreeHandle(hdl);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
	}

    /* Free statment handle */
    SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
    hPut->hStmtXMLPut = 0;

    WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
    HdlMan_FreeHandle(hdl);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PutClose( WDVCAPI_WDV  wdv )
{

    WDVCAPI_PutHandle   hPut = NULL;

    /* Checks */
    if (!wdv) {
		return WDV_False;
    }

    /* Get put handle */
    if (!GetPutHandle(wdv, &hPut)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Close container */
    if (hPut->isDocForXDB == WDV_True) {
        if (!CloseXMLContainer(wdv, hPut)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    } else {
        if (!CloseContainer(wdv, hPut)) {
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        /* Close indexing */
        if (!Put_CloseIndexing(wdv, hPut)) {
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

WDV_Bool PutCreateHandle( WDVCAPI_WDV  wdv)
{

	WDV_Bool            ok = WDV_False;
    WDVCAPI_PutHandle   hPut = NULL;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_put_handle), (WDV_UInt1**) &hPut, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init put handle */
	InitPutHandle(wdv, hPut);

    /* Set put handle in wdv */
    SetPutHandle(wdv, hPut);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool PutDestroyHandle( WDVCAPI_WDV  wdv)
{

    WDVCAPI_PutHandle   hPut = NULL;
    unsigned long       compressedLength = 0;

    /* Get put handle from wdv handle */
    if (!GetPutHandle(wdv, &hPut)) {
        return WDV_False;
    }

    /* Close open compression */
    if (hPut->compressionInitialized) {
        ZLib_gzclose(hPut->gzFile, &compressedLength);
        hPut->gzFile = NULL;
        hPut->compressionInitialized = WDV_False;
    }

    /* Free statement handles */
    if (hPut->hStmtContainer) {
        SQLFreeStmt(hPut->hStmtContainer, SQL_DROP);
        hPut->hStmtContainer = 0;
    }

    if (hPut->hStmtInode) {
        SQLFreeStmt(hPut->hStmtInode, SQL_DROP);
        hPut->hStmtInode = 0;
    }

    if (hPut->hStmtXMLPut) {
        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;
    }

    if (hPut->hStmtSelectByName) {
        SQLFreeStmt(hPut->hStmtSelectByName, SQL_DROP);
        hPut->hStmtSelectByName = 0;
    }

    if (hPut->hStmtInsertResource) {
        SQLFreeStmt(hPut->hStmtInsertResource, SQL_DROP);
        hPut->hStmtInsertResource = 0;
    }

    hPut->hXmlDocClass = NULL;

    /* Free put handle itself */
    WDVCAPI_Free((WDV_UInt1*) hPut );

    return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool InitPutHandle( WDVCAPI_WDV         wdv,
                        WDVCAPI_PutHandle   hPut )
{

    WDV_Bool    ok;

    hPut->hStmtInode = 0;

    hPut->hStmtContainer = 0;
    hPut->hStmtInsertResource = 0;

    memset(hPut->pid, 0, WDVCAPI_MAX_ID_LEN);
    hPut->pIdIndicator = WDVCAPI_MAX_ID_LEN;

    memset(hPut->cid, 0, WDVCAPI_MAX_ID_LEN);
    hPut->cIdIndicator = WDVCAPI_MAX_ID_LEN;

    memset(hPut->firstBlock, 0, 8000);
    hPut->firstBlockLength = 0;
    hPut->firstBlockDone = WDV_False;
    hPut->shortContentIndicator = 0;

    hPut->name[0] = '\0';
    hPut->nameIndicator = SQL_NTS;

    hPut->contentTypeString[0] = '\0';

    hPut->contentLength = 0;

    hPut->bufferLength = SQL_NULL_DATA;

    hPut->hStmtSelectByName = 0;
    memset(hPut->selectByNameParentId, 0, WDVCAPI_MAX_ID_LEN);
    hPut->selectByNameParentIdIndicator = WDVCAPI_MAX_ID_LEN;
    memset(hPut->selectByNameChildId, 0, WDVCAPI_MAX_ID_LEN);
    hPut->selectByNameChildIdIndicator = WDVCAPI_MAX_ID_LEN;
    hPut->selectByNameName[0] = '\0';
    hPut->selectByNameNameIndicator = SQL_NTS;

    hPut->compressionInitialized = WDV_False;
    hPut->gzFile = NULL;
    hPut->compress = WDV_False;

    hPut->hStmtXMLPut = 0;
    hPut->xdbDocId = 0;
    hPut->isDocForXDB = WDV_False;

    hPut->hXmlDocClass = NULL;
    hPut->asyncIndexing = WDV_True;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool ResourceCreatePath( WDVCAPI_WDV        wdv,
                             WDVCAPI_PutHandle  hPut,
                             WDVCAPI_URI        uri,
                             WDVCAPI_Id         id )
{

	WDVCAPI_URIString           parentName = "";
	WDVCAPI_ResourceNameString  childName = "";
    WDVCAPI_Id                  pid;

	/* Checks */
	if (!wdv || !uri) {
		if ( wdv ) {
			Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                    WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		}

		return WDV_False;
	}

	/* Is URI root? */
	if (!strcmp(uri, WDV_PATH_SEPARATOR_STR)) {
        /* Nothing to do! */
        WDVCAPI_IdInitValue(id);
		return WDV_True;
	}

	/* Split URI */
	SplitURI( wdv, uri, parentName, childName );
	if ( *parentName ) {
        /* Create needed parent path */
        if (!ResourceCreatePath(wdv, hPut, parentName, pid)) {
            return WDV_False;
        }
    } else {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_URI_IS_CORRUPTED,
                                WDVCAPI_ERR_TEXT_URI_IS_CORRUPTED);

        return WDV_False;
    }

	/* Create URI child collection */
    if (!ResourceInsertCollection(wdv, hPut, pid, childName, id )) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool ResourceInsertCollection( WDVCAPI_WDV                  wdv,
                                   WDVCAPI_PutHandle            hPut,
                                   WDVCAPI_Id                   pid,
					               WDVCAPI_ResourceNameString   name,
                                   WDVCAPI_Id                   cid )
{

	SQLRETURN             sqlRC = 0;
	SQLHDBC               hDBC = 0;
	SQLHSTMT              hStmt = 0;
    SQLCHAR               sqlStmt [256] = "";
	WDVCAPI_IdString      pidString = "";
	WDVCAPI_IdString      cidString = "";

	/* Checks */
	if (!wdv || !hPut || !pid || !name || !cid) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
	}

    /* Get cid if collection already exists */
    if (ResourceGetByName(wdv, hPut, pid, name, cid)) {
        return WDV_True;
    } else {
        /* Collection doesn't exist and has to be created */
		if(!WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST)) {
            /* Another unexpected error */
            return WDV_False;
        }
    }

    /* Get new id */
    if (!WDVCAPI_IdGetNext(wdv, cid)) {
        return WDV_False;
    }

	/* Build SQL statement */
	WDVCAPI_IdAsString(pid, pidString);
	WDVCAPI_IdAsString(cid, cidString);
	sp77sprintf(sqlStmt, 256,
               "INSERT INTO WEBDAV_INODE SET PID = X'%s', CID = X'%s', NAME = '%s'",
                pidString, cidString, name);

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
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}
	SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Insert displayname */
	sp77sprintf(sqlStmt, 256,
               "INSERT INTO WEBDAV_PROPERTY SET CId = X'%s', Property_Id = X'%s', Property_Short_Value = '%s'",
                cidString, ID_PROPERTY_DISPLAYNAME_STRING, name );

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}
	SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Insert resource type */
	sp77sprintf(sqlStmt, 256,
               "INSERT INTO WEBDAV_PROPERTY SET CId = X'%s', Property_Id = X'%s', Property_Short_Value = '%s'",
                cidString, ID_PROPERTY_RESOURCE_TYPE_STRING, PROPERTY_VALUE_COLLECTION);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}
	SQLFreeStmt(hStmt, SQL_CLOSE);

    /* Insert getlastmodified */
	sp77sprintf(sqlStmt, 256,
               "INSERT INTO WEBDAV_PROPERTY SET CId = X'%s', Property_Id = X'%s', Property_Short_Value = TIMESTAMP",
                cidString, ID_PROPERTY_GET_LAST_MODIFIED_STRING);

	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}
	SQLFreeStmt(hStmt, SQL_CLOSE);

	/* Free statement handle */
	sqlRC = SQLFreeStmt(hStmt, SQL_DROP);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool ResourceInsertResource( WDVCAPI_WDV                wdv,
                                 WDVCAPI_PutHandle          hPut )
{

	SQLRETURN                           sqlRC = 0;
	SQLHDBC                             hDBC = 0;
    WDVCAPI_PropertyShortValueBuffer    propertyShortValueBuffer = "";

	/* Checks */
	if (!wdv || !hPut) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
	}

    /* Get new id */
    if (!WDVCAPI_IdGetNext(wdv, hPut->cid)) {
        return WDV_False;
    }

    /* Statement already prepared */
    if (!hPut->hStmtInsertResource) {
	    /* Get DB handles */
	    if (!GetDBC(wdv, &hDBC)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
				                    WDVCAPI_ERR_CODE_INVALID_HANDLE,
				                    WDVCAPI_ERR_TEXT_INVALID_HANDLE );

		    return WDV_False;
	    }

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hPut->hStmtInsertResource);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtInsertResource, sqlRC);
            hPut->hStmtInsertResource = 0;

		    return WDV_False;
	    }

        sqlRC = SQLPrepare(hPut->hStmtInsertResource, (SQLCHAR*)SQL_INSERT_RESOURCE, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtInsertResource, sqlRC);
            SQLFreeStmt(hPut->hStmtInsertResource, SQL_DROP);
            hPut->hStmtInsertResource = 0;

		    return WDV_False;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hPut->hStmtInsertResource, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hPut->pid, WDVCAPI_MAX_ID_LEN,
                                 &hPut->pIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtInsertResource, sqlRC);

            SQLFreeStmt(hPut->hStmtInsertResource, SQL_DROP);
            hPut->hStmtInsertResource = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hPut->hStmtInsertResource, 2,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hPut->cid, WDVCAPI_MAX_ID_LEN,
                                 &hPut->cIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtInsertResource, sqlRC);

            SQLFreeStmt(hPut->hStmtInsertResource, SQL_DROP);
            hPut->hStmtInsertResource = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hPut->hStmtInsertResource, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hPut->name, WDV_MAX_RESOURCE_NAME_LEN,
                                 &hPut->nameIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPut->hStmtInsertResource, sqlRC );
		    SQLFreeStmt(hPut->hStmtInsertResource, SQL_DROP);
		    hPut->hStmtInsertResource = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
		SQLFreeStmt(hPut->hStmtInsertResource, SQL_CLOSE);
    }

    hPut->nameIndicator = SQL_NTS;

	/* Execute SQL */
	sqlRC = SQLExecute(hPut->hStmtInsertResource);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtInsertResource, sqlRC);

		return WDV_False;
	}

    /* Insert live properties */
    if (!Property_CreateLiveProperties(wdv, hPut->cid, hPut->name, "", "", "0", WDV_False)) {
		return WDV_False;
    }

    /*
    * Insert displayname *
    if (!Property_SetShortValue(wdv, hPut->cid, ID_PROPERTY_DISPLAYNAME, hPut->name)) {
		return WDV_False;
    }

    * Insert resource type *
    if (!Property_SetShortValue(wdv, hPut->cid, ID_PROPERTY_RESOURCE_TYPE, "")) {
		return WDV_False;
    }

    * Insert getcontenttype type *
    if (!Property_SetShortValue(wdv, hPut->cid, ID_PROPERTY_GET_CONTENT_TYPE, "")) {
		return WDV_False;
    }

    * Insert getcontentlength type *
    if (!Property_SetShortValue(wdv, hPut->cid, ID_PROPERTY_GET_CONTENT_LENGTH, "0")) {
		return WDV_False;
    }

    * Insert getlastmodified *
    if (!Property_SetLastModified(wdv, hPut->cid)) {
		return WDV_False;
    }
    */

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool ResourceGetByName( WDVCAPI_WDV                 wdv,
                            WDVCAPI_PutHandle           hPut,
				            WDVCAPI_Id                  pid,
				            WDVCAPI_ResourceNameString  name,
				            WDVCAPI_Id                  cid )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;

    /* Set input values */
    SAPDB_memcpy(hPut->selectByNameParentId, pid, WDVCAPI_MAX_ID_LEN);
    Common_StrMaxCopy(hPut->selectByNameName, name, WDV_MAX_RESOURCE_NAME_LEN);

    /* Statement already prepared */
    if (!hPut->hStmtSelectByName) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hPut->hStmtSelectByName);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPut->hStmtSelectByName, sqlRC );
            hPut->hStmtSelectByName = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare( hPut->hStmtSelectByName, (SQLCHAR*)SQL_SELECT_BY_NAME, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPut->hStmtSelectByName, sqlRC );
		    SQLFreeStmt(hPut->hStmtSelectByName, SQL_DROP);
            hPut->hStmtSelectByName = 0;

            return WDV_False;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hPut->hStmtSelectByName, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hPut->selectByNameParentId, WDVCAPI_MAX_ID_LEN,
                                 &hPut->selectByNameParentIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtSelectByName, sqlRC);
            SQLFreeStmt(hPut->hStmtSelectByName, SQL_DROP);
            hPut->hStmtSelectByName = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hPut->hStmtSelectByName, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                                 hPut->selectByNameName, WDV_MAX_RESOURCE_NAME_LEN,
                                 &hPut->selectByNameNameIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPut->hStmtSelectByName, sqlRC );
		    SQLFreeStmt(hPut->hStmtSelectByName, SQL_DROP);
		    hPut->hStmtSelectByName = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(hPut->hStmtSelectByName, 1, SQL_C_BINARY,
                           hPut->selectByNameChildId, WDVCAPI_MAX_ID_LEN,
                           &hPut->selectByNameChildIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPut->hStmtSelectByName, sqlRC );
		    SQLFreeStmt(hPut->hStmtSelectByName, SQL_DROP);
            hPut->hStmtSelectByName = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
		SQLFreeStmt(hPut->hStmtSelectByName, SQL_CLOSE);
    }

    hPut->selectByNameNameIndicator = SQL_NTS;

	/* Execute SQL */
	sqlRC = SQLExecute(hPut->hStmtSelectByName);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPut->hStmtSelectByName, sqlRC );

		return WDV_False;
	}

	/* Fetch rowset */
    sqlRC = SQLFetch(hPut->hStmtSelectByName);

	/* Error handling */
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPut->hStmtSelectByName, sqlRC );

		if(WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_SQL, WDVCAPI_ERR_SQL_CODE_NO_DATA)) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST,
                                    WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_DOESNT_EXIST);
        }

		return WDV_False;
	}

    /* Set return value */
    SAPDB_memcpy(cid, hPut->selectByNameChildId, WDVCAPI_MAX_ID_LEN);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool OpenContainer( WDVCAPI_WDV         wdv,
                        WDVCAPI_PutHandle   hPut )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;

    /* Inits */
    hPut->contentLength = 0;
    hPut->firstBlockDone = WDV_False;
    hPut->firstBlockLength = 0;

    /* Statement already prepared? */
    if (!hPut->hStmtContainer) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hPut->hStmtContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);
            hPut->hStmtContainer = 0;

		    return WDV_False;
	    }

        sqlRC = SQLPrepare(hPut->hStmtContainer, (SQLCHAR*)SQL_INSERT_CONTAINER, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);
            SQLFreeStmt(hPut->hStmtContainer, SQL_DROP);
            hPut->hStmtContainer = 0;

		    return WDV_False;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hPut->hStmtContainer, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hPut->cid, WDVCAPI_MAX_ID_LEN,
                                 &hPut->cIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);

            SQLFreeStmt(hPut->hStmtContainer, SQL_DROP);
            hPut->hStmtContainer = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindParameter(hPut->hStmtContainer, 3,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 (PTR) 1, 0, &hPut->bufferLength );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);
            SQLFreeStmt(hPut->hStmtContainer, SQL_DROP);
            hPut->hStmtContainer = 0;

		    return WDV_False;
	    }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WriteContainer( WDVCAPI_WDV            wdv,
                         WDVCAPI_PutHandle      hPut,
                         WDVCAPI_Buffer         buffer,
                         WDVCAPI_BufferLength   bufferLength )
{

	SQLRETURN               sqlRC = 0;
    SQLPOINTER              valuePointer;
    WDVCAPI_ContentLength   bufferOffset = 0;

    /* First block has been already wrote? */
    if (hPut->firstBlockDone == WDV_False) {
        /* Set first block */
        if (hPut->firstBlockLength + bufferLength < 8000) {
            SAPDB_memcpy(hPut->firstBlock + hPut->firstBlockLength, buffer, bufferLength);
            hPut->firstBlockLength += bufferLength;

            return WDV_True;
        } else {
            bufferOffset = 8000 - hPut->firstBlockLength;
            SAPDB_memcpy(hPut->firstBlock + hPut->firstBlockLength, buffer, bufferOffset);
            hPut->firstBlockLength = 8000;

            /*(char*)buffer += bufferOffset;*/
			buffer = (char*)buffer + bufferOffset;
            bufferLength -= bufferOffset;
        }

        hPut->firstBlockDone = WDV_True;
        hPut->bufferLength = SQL_LEN_DATA_AT_EXEC(0);
        hPut->shortContentIndicator = hPut->firstBlockLength;

        sqlRC = SQLBindParameter(hPut->hStmtContainer, 2,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hPut->firstBlock, hPut->firstBlockLength,
                                 &hPut->shortContentIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);
            SQLFreeStmt(hPut->hStmtContainer, SQL_DROP);
            hPut->hStmtContainer = 0;

		    return WDV_False;
	    }
    
        sqlRC = SQLExecute(hPut->hStmtContainer);
	    if ( sqlRC != SQL_NEED_DATA ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);

		    return WDV_False;
	    }

        hPut->contentLength = hPut->firstBlockLength;

        sqlRC = SQLParamData(hPut->hStmtContainer, &valuePointer);
	    if ( sqlRC != SQL_NEED_DATA ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);

		    return WDV_False;
	    }
    }
    
    /* Write buffer into content BLOB */
    if (hPut->compress == WDV_True) {
        if (!Put_WriteCompressedData(wdv, hPut, buffer, bufferLength)) {
            return WDV_False;
        }
    } else {
        sqlRC = SQLPutData(hPut->hStmtContainer, buffer, bufferLength);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);

		    return WDV_False;
	    }
    }

    /* Increment content length */
    hPut->contentLength += bufferLength;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool CloseContainer( WDVCAPI_WDV        wdv,
                         WDVCAPI_PutHandle  hPut )
{

	SQLRETURN                           sqlRC = 0;
    SQLPOINTER                          valuePointer;
    unsigned long                       compressedLength = 0;

    /* Close compression */
    if (hPut->compressionInitialized) {
        ZLib_gzclose(hPut->gzFile, &compressedLength);
        hPut->gzFile = NULL;
        hPut->compressionInitialized = WDV_False;

        if (!Resource_SetCompressedLength(wdv, hPut->pid, hPut->name, compressedLength)) {
            return WDV_False;
        }
    } else {
        if (!Resource_SetCompressedLength(wdv, hPut->pid, hPut->name, 0)) {
            return WDV_False;
        }
    }

    /* Flush first block, if not done */
    if (hPut->firstBlockLength >= 0 && hPut->firstBlockDone == WDV_False) {
        hPut->bufferLength = SQL_NULL_DATA;
        hPut->shortContentIndicator = hPut->firstBlockLength;

        sqlRC = SQLBindParameter(hPut->hStmtContainer, 2,
                                SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                hPut->firstBlock, hPut->firstBlockLength,
                                &hPut->shortContentIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);
            SQLFreeStmt(hPut->hStmtContainer, SQL_DROP);
            hPut->hStmtContainer = 0;

		    return WDV_False;
	    }
    
        sqlRC = SQLExecute(hPut->hStmtContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);

		    return WDV_False;
	    }

        hPut->contentLength = hPut->firstBlockLength;
    }

    /* If we just filled the short content the BLOB is null, */
    /* so we must not param any more data                    */
    if (hPut->bufferLength != SQL_NULL_DATA) {
        /* The BLOB was filled at least with one byte */
        if (hPut->contentLength == 0) {
            /* Call SQLPutData to prevent function sequence error */
            sqlRC = SQLPutData(hPut->hStmtContainer, NULL, 0 );
	        if ( sqlRC != 0 ) {
                Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);

		        return WDV_False;
	        }
        };

        sqlRC = SQLParamData(hPut->hStmtContainer, &valuePointer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hPut->hStmtContainer, sqlRC);

		    return WDV_False;
	    }
    }

    SQLFreeStmt(hPut->hStmtContainer, SQL_CLOSE);

    /* Now we have the content length and can update the resource entry */
    /* We also set the content type here                                */
    if (!UpdateResourceProperties(wdv, hPut)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool CloseXMLContainer( WDVCAPI_WDV        wdv,
                            WDVCAPI_PutHandle  hPut )
{

	SQLRETURN               sqlRC = 0;
    SAPDB_UInt8             docId = 0;
    SQLLEN                  indicator = 0;

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(wdv->hDBC, &hPut->hStmtXMLPut);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);
        hPut->hStmtXMLPut = 0;

		return WDV_False;
	}

    sqlRC = SQLPrepare(hPut->hStmtXMLPut, (SQLCHAR*)SQL_UPDATE_XDB_DOCID, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);
        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

		return WDV_False;
	}

    /* Bind input parameters */
    sqlRC = SQLBindParameter(hPut->hStmtXMLPut, 1,
                                SQL_PARAM_INPUT, SQL_C_ULONG, SQL_BIGINT, 0, 0,
                                &hPut->xdbDocId, 0,
                                &indicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);

        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

		return WDV_False;
	}

    sqlRC = SQLBindParameter(hPut->hStmtXMLPut, 2,
                                SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                hPut->pid, WDVCAPI_MAX_ID_LEN   ,
                                &hPut->pIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);

        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

		return WDV_False;
	}

    sqlRC = SQLBindParameter(hPut->hStmtXMLPut, 3,
                                SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                hPut->cid, WDVCAPI_MAX_ID_LEN,
                                &hPut->cIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);

        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

		return WDV_False;
	}

    sqlRC = SQLExecute(hPut->hStmtXMLPut);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hPut->hStmtXMLPut, sqlRC);

        SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
        hPut->hStmtXMLPut = 0;

		return WDV_False;
	}

    /* Free statment handle */
    SQLFreeStmt(hPut->hStmtXMLPut, SQL_DROP);
    hPut->hStmtXMLPut = 0;

    /* Now we have the content length and can update the resource entry */
    /* We also set the content type here                                */
    if (!UpdateResourceProperties(wdv, hPut)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool UpdateResourceProperties( WDVCAPI_WDV           wdv,
                                   WDVCAPI_PutHandle     hPut )
{

    WDVCAPI_PropertyShortValueBuffer    propertyShortValueBuffer;

    sp77sprintf(propertyShortValueBuffer, WDV_MAX_PROPERTY_SHORT_VALUE_LEN, "%lu", hPut->contentLength);
    if (!Property_UpdateShortValue(wdv, hPut->cid, ID_PROPERTY_GET_CONTENT_LENGTH, propertyShortValueBuffer)) {
        return WDV_False;
    }

    if (!Property_UpdateShortValue(wdv, hPut->cid, ID_PROPERTY_GET_CONTENT_TYPE, hPut->contentTypeString)) {
        return WDV_False;
    }

    /* Update last modification date */
    if (!Property_UpdateLastModified(wdv, hPut->cid)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Put_OpenIndexing( WDVCAPI_WDV           wdv,
                           WDVCAPI_PutHandle     hPut,
                           WDVCAPI_XmlDocClassId xmlDocClassId )
{

    /* Inits */
    hPut->hXmlDocClass = NULL;

    /* Get handles */
    if (!XMLIndexing_GetXmlDocClass(wdv, xmlDocClassId, &hPut->hXmlDocClass)) {
        return WDV_False;
    }

    /* Do we have something to index? */
    if (!hPut->hXmlDocClass) {
        return WDV_True;
    }

    if (hPut->asyncIndexing == WDV_False && !hPut->hXmlDocClass->hXPath) {
	    if (XMLXPath_Idx_Initialize(&hPut->hXmlDocClass->hXPath)) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_NO_ACCESS_TO_XPATH_ENGINE,
                                    WDVCAPI_ERR_TEXT_NO_ACCESS_TO_XPATH_ENGINE);

            return WDV_False;
	    }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Put_Indexing( WDVCAPI_WDV          wdv,
                       WDVCAPI_PutHandle    hPut,
                       WDVCAPI_Buffer       buffer,
                       WDVCAPI_BufferLength bufferLength )
{

    /* Checks */
    if (!hPut) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Something to index? */
    if (!hPut->hXmlDocClass) {
        return WDV_True;
    }

    /* Index buffer */
    if (XMLXPath_Idx_ParseBuf(hPut->hXmlDocClass->hXPath, buffer, bufferLength, NULL, 0)) {
        return WDV_False;
    } else {
        return WDV_True;
    }

}

/*===========================================================================*/

WDV_Bool Put_CloseIndexing( WDVCAPI_WDV       wdv,
                            WDVCAPI_PutHandle hPut )
{

    WDVCAPI_IdString    docClassIdString;

    /* Checks */
    if (!hPut) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        return WDV_False;
    }

    /* Something to index? */
    if (hPut->hXmlDocClass) {
        /* Call final parsing */
	    XMLXPath_Idx_ParseBuf(hPut->hXmlDocClass->hXPath, (unsigned char*)"", 0, NULL, 1);

        /* Close XPath engine */
        /*
        if (XMLXPath_Idx_Terminate(hPut->hXmlDocClass->hXPath)) {
            return WDV_False;
        }
        */

        /* This has to be changed!                                 */
        /* If we could reset hXPath, we don't have to terminate it */
        /*
        hPut->hXmlDocClass->hXPath = NULL;
        */
    }

    /* Set doc class and indexing state */
    if (hPut->hXmlDocClass->docClassId && !WDVCAPI_IdIsInitialValue(hPut->hXmlDocClass->docClassId)) {
        WDVCAPI_IdAsString(hPut->hXmlDocClass->docClassId, docClassIdString);
        if (!Property_SetShortValue(wdv, hPut->cid, ID_PROPERTY_DOC_CLASS_ID, docClassIdString)) {
            return WDV_False;
        }

        if (hPut->asyncIndexing == WDV_True) {
            if (!Property_SetShortValue(wdv, hPut->cid, ID_PROPERTY_PARSE_STATE, XMLINDEXING_STATE_NEW)) {
                return WDV_False;
            }
        } else {
            if (!Property_SetShortValue(wdv, hPut->cid, ID_PROPERTY_PARSE_STATE, XMLINDEXING_STATE_FINISHED)) {
                return WDV_False;
            }
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Put_WriteCompressedData( WDVCAPI_WDV             wdv,
                                  WDVCAPI_PutHandle       hPut,
                                  WDVCAPI_Buffer          buffer,
                                  WDVCAPI_BufferLength    bufferLength )
{

    int         zRc;
	SQLRETURN   sqlRC = 0;

    /* Compression initialized? */
    if (hPut->compressionInitialized == WDV_False) {
        hPut->gzFile = ZLib_gzopen (hPut->hStmtContainer, "wb9");

        if (hPut->gzFile) {
            hPut->compressionInitialized = WDV_True;
        } else {
            return WDV_False;
        }
    }

    /* Compress and write */
    zRc = ZLib_gzwrite(hPut->gzFile, buffer, bufferLength);
    if (zRc != bufferLength) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

int ReadStream( SQL_LC_StreamParm *StreamParam, void *rgbOutStream, int cbStreamMax, int *pcbStreamLen, int *pcbNoOfRowsRead, void* pMask )
{

    WDVCAPI_Handle   hdl = 0;
    SAPDB_UInt4      pieces = 0;
    SAPDB_UInt4      currMaxLen = 0;
    int              rc = SQL_STREAM_OK;
    WDVCAPI_InStream    inStream;
    WDVCAPI_PutHandle   hPut;

    *pcbStreamLen = 0;
    *pcbNoOfRowsRead = 0;

    hdl = StreamParam->C_1.Stream.hStream.ABAPTabId;

    HdlMan_LeaseHandleData(hdl, (void*)&hPut);
    if (!hPut) {
        HdlMan_ReleaseHandleData(hdl);
        return SQL_STREAM_NO_MORE_DATA;
    }

    /*
    pieces = cbStreamMax / 128;
    */
    currMaxLen = cbStreamMax; /*(cbStreamMax > 32767) ? 32767 : cbStreamMax;*/
    inStream = *hPut->inStream;
    inStream(hPut->inStreamUserData, rgbOutStream, currMaxLen, pcbStreamLen);
    *pcbNoOfRowsRead = 1;/* *pcbStreamLen; */

    /* Increment content length */
    hPut->contentLength += *pcbStreamLen;

    if (*pcbStreamLen < currMaxLen) {
        rc = SQL_STREAM_NO_MORE_DATA;
    } else {
        rc = SQL_STREAM_OK;
    }

    HdlMan_ReleaseHandleData(hdl);

    return rc;

}

/***********************************************************************

	End

 ***********************************************************************/
