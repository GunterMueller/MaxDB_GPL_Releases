/*!**********************************************************************

  module: WDVCAPI_Get.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Getting resources

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
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Get.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ZLibIO.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_HandleManager.h"
#define LC_STREAMS_ENABLED
#include "livecachetypes.h" /* no check */
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define SQL_GET_CONTENT \
    "SELECT Short_Content, Content FROM WEBDAV_Container WHERE CId = ?"

#define SQL_GET_GET_PROPERTIES                                       \
    "SELECT Property_Id, Property_Short_Value FROM WEBDAV_Property WHERE CId = ? AND \
     Property_Id IN (x'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"', x'"ID_PROPERTY_GET_CONTENT_TYPE_STRING"', x'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"')"

#define SQL_GET_COLLECTION_PROPS    \
    "DECLARE COL_PROPS CURSOR FOR WITH RECURSIVE ColPropsInodes(Inode) AS \
     ( \
        SELECT I.CId \
        FROM WEBDAV_Inode I \
        WHERE I.PId = ? \
     UNION ALL \
        SELECT I.CId \
        FROM WEBDAV_Inode I, ColPropsInodes \
        WHERE I.PId = Inode \
     ) \
     SELECT count(Inode), SUM(NUM(P.Property_Short_Value)), PP.Property_Short_Value ResourceType \
     FROM ColPropsInodes C, WebDAV_Property P, WebDAV_Property PP \
     WHERE C.Inode = P.CId \
     AND P.CId = PP.CId \
     AND P.Property_Id = x'000000000000000000000000000000000000000000000004' \
     AND PP.Property_id = x'000000000000000000000000000000000000000000000002' \
     GROUP BY PP.Property_Short_Value ORDER BY 3 DESC"

#define SQL_XDB_GET     "CALL XDB_GET(?, ?)"
#define SQL_XDB_XQUERY  "CALL XDB_XQUERY(?, ?, ?)"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

struct st_wdvcapi_get_handle {
	SQLHSTMT                    statementHandle;
	WDV_Bool                    eol;
	WDVCAPI_ResourceNameString  resourceName;
};

struct st_get_handle {
	SQLHSTMT                            hStmtContainer;
	SQLHSTMT                            hStmtGetLengthAndType;
	SQLHSTMT                            hStmtGetCollectionProps;

    WDV_Bool                            firstBlockDone;

    WDVCAPI_ContentLength               contentLength;
    WDVCAPI_ContentTypeString           contentTypeString;
    WDVCAPI_PropertyShortValueBuffer    lastModified;

    WDVCAPI_Id                          propertyId;
    SQLLEN                              propertyIdIndicator;

    WDVCAPI_PropertyShortValueBuffer    propertyShortValue;
    SQLLEN                              propertyShortValueIndicator;

    WDVCAPI_PropertyShortValueBuffer    collectionPropsCount;
    SQLLEN                              collectionPropsCountIndicator;

    WDVCAPI_PropertyShortValueBuffer    collectionPropsSize;
    SQLLEN                              collectionPropsSizeIndicator;

    WDVCAPI_PropertyShortValueBuffer    collectionPropsResourceType;
    SQLLEN                              collectionPropsResourceTypeIndicator;

    WDVCAPI_Resource                    resource;
    WDVCAPI_Resource                    childResource;

    WDV_Bool                            decompressionInitialized;
    gzFile                              gzFile;
    WDV_UInt4                           compressedLength;

    WDV_Long                            rangeStartPos;
    WDV_Long                            rangeEndPos;
    WDV_Long                            readLength;
    WDV_Bool                            startNotReached;

    WDVCAPI_OutStream                    outStream;
    void                                *outStreamUserData;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Get_InitHandle( WDVCAPI_WDV        wdv,
                         WDVCAPI_GetHandle  hGet);

WDV_Bool Get_GetProperties( WDVCAPI_WDV          wdv,
                            WDVCAPI_GetHandle    hGet );

WDV_Bool Get_OpenContainer( WDVCAPI_WDV         wdv,
                            WDVCAPI_GetHandle   hGet );

WDV_Bool Get_ReadContainer( WDVCAPI_WDV           wdv,
                            WDVCAPI_GetHandle     hGet,
                            WDVCAPI_Buffer        buffer,
                            WDVCAPI_BufferLength  bufferLength,
                            WDV_Long             *bufferStartPos,
                            WDVCAPI_BufferLength *readLength );

WDV_Bool Get_ReadXDBContainer( WDVCAPI_WDV           wdv,
                               WDVCAPI_GetHandle     hGet,
                               WDVCAPI_Buffer        buffer,
                               WDVCAPI_BufferLength  bufferLength,
                               WDV_Long             *bufferStartPos,
                               WDVCAPI_BufferLength *readLength );

WDV_Bool Get_CloseContainer( WDVCAPI_WDV        wdv,
                             WDVCAPI_GetHandle  hGet );

WDV_Bool Get_GetCompressedData( WDVCAPI_WDV              wdv,
                                WDVCAPI_GetHandle        hGet,
                                WDVCAPI_Buffer           buffer,
                                WDVCAPI_BufferLength     bufferLength,
                                WDVCAPI_BufferLength    *readLength );

int WriteStream( SQL_LC_StreamParm *StreamParam, void *rgbInStream, int noOfRows, int cbStreamLen, void* pMask );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_GetCollectionProperties( WDVCAPI_WDV                   wdv,
					                      WDVCAPI_PropertyShortValue    collectionCount,
					                      WDVCAPI_PropertyShortValue    resourceCount,
					                      WDVCAPI_PropertyShortValue    size )
{
    WDVCAPI_GetHandle                   hGet = NULL;
	SQLRETURN                           sqlRC = 0;
    SQLHDBC                             hDBC = 0;

    /* Checks */
    if (!wdv || !resourceCount || !collectionCount || !size) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    collectionCount[0] = '\0';
    resourceCount[0] = '\0';
    size[0] = '\0';

    /* Get get handles */
    GetGetHandle(wdv, &hGet);

    /* Statement already parsed? */
    if (!hGet->hStmtGetCollectionProps) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hGet->hStmtGetCollectionProps);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_DROP);
            hGet->hStmtGetCollectionProps = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hGet->hStmtGetCollectionProps, (SQLCHAR*)SQL_GET_COLLECTION_PROPS, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_DROP);
            hGet->hStmtGetCollectionProps = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hGet->hStmtGetCollectionProps, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hGet->resource->id, WDVCAPI_MAX_ID_LEN,
                                  &hGet->resource->idIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hGet->hStmtGetCollectionProps, sqlRC);

            SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_DROP);
            hGet->hStmtGetCollectionProps = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(hGet->hStmtGetCollectionProps, 1, SQL_C_CHAR,
                           hGet->collectionPropsCount, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                           &hGet->collectionPropsCountIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_DROP);
            hGet->hStmtGetCollectionProps = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindCol(hGet->hStmtGetCollectionProps, 2, SQL_C_CHAR,
                           hGet->collectionPropsSize, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                           &hGet->collectionPropsSizeIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_DROP);
            hGet->hStmtGetCollectionProps = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindCol(hGet->hStmtGetCollectionProps, 3, SQL_C_CHAR,
                           hGet->collectionPropsResourceType, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                           &hGet->collectionPropsResourceTypeIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_DROP);
            hGet->hStmtGetCollectionProps = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hGet->hStmtGetCollectionProps);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );

		return WDV_False;
	}

    /* Inits */
    strcpy(collectionCount, "0");
    strcpy(resourceCount, "0");
    strcpy(size, "0");

	/* Fetch collection rowset */
    sqlRC = SQLFetch(hGet->hStmtGetCollectionProps);
	if ( sqlRC != 0 ) {
        if (sqlRC != SQL_NO_DATA) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );

    		return WDV_False;
        }
    } else {
        if (strcmp(hGet->collectionPropsResourceType, "collection") == 0) {
            strcpy(collectionCount, hGet->collectionPropsCount);

	        /* Fetch resource count and size */
            sqlRC = SQLFetch(hGet->hStmtGetCollectionProps);
	        if ( sqlRC != 0 ) {
                if (sqlRC != SQL_NO_DATA) {
                    Error_AddSQLErrorItem( wdv, hGet->hStmtGetCollectionProps, sqlRC );

    		        return WDV_False;
                }
            } else {
                strcpy(resourceCount, hGet->collectionPropsCount);
                strcpy(size, hGet->collectionPropsSize);
            }
        } else {
            strcpy(resourceCount, hGet->collectionPropsCount);
            strcpy(size, hGet->collectionPropsSize);
        }
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetCollection( WDVCAPI_WDV                 wdv,
					            WDVCAPI_URI                 childResourceName,
                                WDVCAPI_PropertyShortValue  resourceType,
                                WDVCAPI_PropertyShortValue  size,
                                WDVCAPI_PropertyShortValue  lastModified,
                                WDV_Bool                   *isXDBDoc,
				                WDV_Bool                   *childFound )
{
    WDVCAPI_GetHandle   hGet = NULL;

    /* Checks */
    if (!wdv || !childResourceName || !resourceType || !size || !lastModified || !childFound) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    resourceType[0] = '\0';
    size[0] = '\0';
    lastModified[0] = '\0';

    /* Get get handles */
    GetGetHandle(wdv, &hGet);

    if (!hGet->childResource) {
        if (!Resource_CreateHandle(wdv, &hGet->childResource)) {
            return WDV_False;
        }
    }

    if (!Resource_CollectChildsWithProps(wdv, hGet->resource, hGet->childResource, childFound)) {
		return WDV_False;
    }

    strcpy(childResourceName, hGet->childResource->nameWithProps);
    strcpy(resourceType, hGet->childResource->resourceTypeWithProps);
    strcpy(size, hGet->childResource->sizeWithProps);
    strcpy(lastModified, hGet->childResource->lastModifiedWithProps);

    if (hGet->childResource->xdbDocId) {
        *isXDBDoc = WDV_True;
    } else {
        *isXDBDoc = WDV_False;
    }

    return WDV_True;
}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetNextCollection( WDVCAPI_WDV                 wdv,
					                WDVCAPI_URI                 childResourceName,
                                    WDVCAPI_PropertyShortValue  resourceType,
                                    WDVCAPI_PropertyShortValue  size,
                                    WDVCAPI_PropertyShortValue  lastModified,
                                    WDV_Bool                   *isXDBDoc,
				                    WDV_Bool                   *childFound )
{
    WDVCAPI_GetHandle   hGet = NULL;

    /* Checks */
    if (!wdv || !childResourceName || !resourceType || !size || !lastModified || !childFound) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    /* Get get handle */
    GetGetHandle(wdv, &hGet);

    if (!Resource_GetNextFromCollectionWithProps(wdv, hGet->resource, childFound)) {
        return WDV_False;
    }

    strcpy(childResourceName, hGet->childResource->nameWithProps);
    strcpy(resourceType, hGet->childResource->resourceTypeWithProps);
    strcpy(size, hGet->childResource->sizeWithProps);
    strcpy(lastModified, hGet->childResource->lastModifiedWithProps);

    if (hGet->childResource->xdbDocId) {
        *isXDBDoc = WDV_True;
    } else {
        *isXDBDoc = WDV_False;
    }

    return WDV_True;
}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetOpen( WDVCAPI_WDV           wdv,
					      WDVCAPI_URI           uri,
                          WDVCAPI_LockIdString  lockIdString,
                          WDV_Long              rangeStartPos,
                          WDV_Long              rangeEndPos,
                          WDVCAPI_OutStream     outStream,
                          void                 *outStreamUserData,
                          WDV_Bool             *isDocInXDB )
{

    WDVCAPI_GetHandle       hGet = NULL;
    WDVCAPI_LockHandle      hLock = NULL;
    WDVCAPI_LockId          lockId;

    /* Checks */
    if (!wdv || !uri) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Get get handle */
    GetGetHandle(wdv, &hGet);

    /* Set current range */
    hGet->rangeStartPos = rangeStartPos;
    hGet->rangeEndPos = rangeEndPos;
    hGet->readLength = 0;
    hGet->startNotReached = WDV_True;

    /* Get if resource exists */
    if (!Resource_GetByUri(wdv, uri, hGet->resource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Get content length and type */
    if (!Get_GetProperties(wdv, hGet)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Is content stored in XDB? */
    if (hGet->resource->xdbDocId > 0) {
        *isDocInXDB = WDV_True;
        if (!outStream) {
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }

        hGet->outStream = outStream;
        hGet->outStreamUserData = outStreamUserData;
    } else {
        /* Range check */
        if (hGet->rangeStartPos > (WDV_Long)hGet->contentLength) {
            /* Range start pos out of bounds */
		    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_RANGE_START_OUT_OF_BOUNDS,
                            WDVCAPI_ERR_TEXT_RANGE_START_OUT_OF_BOUNDS);
		    return WDV_False;
        }

        /* Range end should not behind the content end */
        if (hGet->rangeStartPos == -1 && hGet->contentLength < hGet->rangeEndPos) {
            hGet->rangeEndPos = hGet->contentLength;
        }

        /* If only the last bytes defined to get */
        if (hGet->rangeStartPos == -1 && hGet->rangeEndPos > 0) {
            hGet->rangeStartPos = hGet->contentLength - hGet->rangeEndPos;
            hGet->rangeEndPos = hGet->contentLength;
        }

        /* Range end has to included */
        if (hGet->rangeEndPos != -1) {
            hGet->rangeEndPos++;
        }

        /* If uri is not root */
        if (strcmp(uri, WDV_PATH_SEPARATOR_STR)) {
            /* Is content compressed? */
            hGet->compressedLength = 0;
            if (!Resource_GetCompressedLength(wdv, hGet->resource->parentId,
                                            hGet->resource->name, &hGet->compressedLength)) {
                return WDV_False;
            }

            /* Open container */
            if (!Get_OpenContainer(wdv, hGet)) {
                /* Cancel transaction */
                WDV_CancelTransaction(wdv);

                return WDV_False;
            }
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_Get( WDVCAPI_WDV           wdv,
                      WDVCAPI_Buffer        buffer,
                      WDVCAPI_BufferLength  bufferLength,
                      WDV_Long             *bufferStartPos,
                      WDVCAPI_BufferLength *readLength )
{

    WDVCAPI_GetHandle   hGet = NULL;

    /* Checks */
    if (!wdv || !buffer || !readLength) {
		if ( wdv ) {
			Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		}

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

		return WDV_False;
    }

    /* Inits */
    *readLength = 0;

    /* Check buffer length: is there something to do? */
    if (bufferLength < 1) {
        return WDV_True;
    }

    /* Get get handle */
    GetGetHandle(wdv, &hGet);

    /* Read container */
    if (hGet->resource->xdbDocId > 0) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                            WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        *readLength = 0;

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    } else {
        if (!Get_ReadContainer(wdv, hGet, buffer, bufferLength, bufferStartPos, readLength)) {
            *readLength = 0;
            /* Cancel transaction */
            WDV_CancelTransaction(wdv);

            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetXDB( WDVCAPI_WDV    wdv )
{

	SQLRETURN                sqlRC = 0;
    SQLHSTMT                 hStmtXDBProcCall = 0;
    SQLStreamDesc            streamDesc;
    SQL_LC_StreamParm       *StreamParam;
    WDV_Bool                 ok = WDV_False;
    WDVCAPI_GetHandle        hGet = NULL;
    WDVCAPI_Handle           hdl = 0;

    if ( !wdv ) {
		return WDV_False;
	}

    hGet = wdv->hGet;

    /* Prepare stream descriptor */
	WDVCAPI_Allocat(sizeof(SQL_LC_StreamParm) - sizeof (OmsTypeStreamMemberDesc) * 256 + sizeof (OmsTypeStreamMemberDesc) * 1, (WDV_UInt1**) &streamDesc.StreamParam, &ok ); /* Just one col desc */
	if (ok == WDV_False) {
		return WDV_False;
	}

    StreamParam = streamDesc.StreamParam;
    if (StreamParam) {
      StreamParam->C_1.Stream.nilPointer          = (void*) 0;
      StreamParam->C_1.Stream.hStream.ABAPTabId   = 1;
      StreamParam->C_1.Stream.hStream.size        = 1;  /* One byte */
      StreamParam->C_1.Stream.hStream.memberCount = 1;
    }

    streamDesc.WriteProc = &WriteStream;
    streamDesc.ReadProc  = NULL;

    /* Allocat statement handle */
	sqlRC = SQLAllocStmt(wdv->hDBC, &hStmtXDBProcCall);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );
    	SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
        hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hStmtXDBProcCall, (SQLCHAR *) SQL_XDB_GET, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );
    	SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
        hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

		return WDV_False;
	}

    /* Bind input parameter */
    sqlRC = SQLBindParameter(hStmtXDBProcCall, 1,
                                SQL_PARAM_INPUT, SQL_C_ULONG, SQL_BIGINT, 0, 0,
                                &hGet->resource->xdbDocId, 0,
                                &hGet->resource->xdbDocIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmtXDBProcCall, sqlRC);

        SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
        hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

		return WDV_False;
	}

    /* Bind stream parameter */
    HdlMan_NewHandle(&hdl, hGet);

    StreamParam->C_1.Stream.hStream.colDesc[0].memberType = STYPE_CHAR;
    StreamParam->C_1.Stream.hStream.colDesc[0].length     = 1;
    StreamParam->C_1.Stream.hStream.colDesc[0].offset     = 0;
    StreamParam->C_1.Stream.hStream.ABAPTabId             = hdl;

    sqlRC = SQLBindParameter( hStmtXDBProcCall, 2,
                              SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_ABAPHANDLE, 0, 0,
                              &streamDesc, sizeof(streamDesc), NULL);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );

		SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
		hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
        HdlMan_FreeHandle(hdl);

		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(hStmtXDBProcCall);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );
        SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
        HdlMan_FreeHandle(hdl);

        return WDV_False;
	}

    /* Close statement handle */
    SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);

    WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
    HdlMan_FreeHandle(hdl);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetClose( WDVCAPI_WDV  wdv )
{

    WDVCAPI_GetHandle   hGet = NULL;

    /* Checks */
    if (!wdv) {
		return WDV_False;
    }

    /* Get get handle */
    GetGetHandle(wdv, &hGet);

    /* Close container */
    if (hGet->resource->xdbDocId == 0) {
        if (!Get_CloseContainer(wdv, hGet)) {
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

WDV_Bool WDVCAPI_GetContentLength( WDVCAPI_WDV            wdv,
                                   WDVCAPI_ContentLength *contentLength )
{

    WDVCAPI_GetHandle   hGet = NULL;

    if (!wdv || !contentLength) {
        return WDV_False;
    }

    /* Get get handle */
    GetGetHandle(wdv, &hGet);
    if (!hGet) {
        return WDV_False;
    }

    *contentLength = hGet->contentLength;

    return WDV_True;
}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetContentType( WDVCAPI_WDV          wdv,
                                 WDVCAPI_ContentType  contentType )
{

    WDVCAPI_GetHandle   hGet = NULL;

    if (!wdv) {
        return WDV_False;
    }

    /* Get get handle */
    GetGetHandle(wdv, &hGet);
    if (!hGet) {
        return WDV_False;
    }

    Common_StrMaxCopy(contentType, hGet->contentTypeString, WDVCAPI_MAX_CONTENT_TYPE_LEN);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetLastModified( WDVCAPI_WDV                   wdv,
                                  WDVCAPI_PropertyShortValue    lastModified )
{

    WDVCAPI_GetHandle   hGet = NULL;

    if (!wdv) {
        return WDV_False;
    }

    /* Get get handle */
    GetGetHandle(wdv, &hGet);
    if (!hGet) {
        return WDV_False;
    }

    Common_StrMaxCopy(lastModified, hGet->lastModified, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool XXWDVCAPI_Get( WDVCAPI_WDV   wdv,
                        WDVCAPI_URI   uri )
{

	SQLRETURN                   sqlRC = 0;
	SQLHDBC                     hDBC = 0;
	SQLHSTMT                    statementHandle = 0;
    SQLCHAR                     sqlStatement [256] = "";
	SQLLEN                      bufferLength = 0;
	WDVCAPI_ResourceNameString  parentName = "";
	WDVCAPI_ResourceNameString  childName = "";
	WDVCAPI_IdString            parentId = "";
	WDVCAPI_IdString            newResourceId = "";
	WDVCAPI_Resource            parent = NULL;
	WDVCAPI_Resource            newResource = NULL;
    WDVCAPI_GetHandle           hGet = NULL;

	/* Checks */
	if (!wdv || !uri) {
		return WDV_False;
	}

	/* Create temp resource */
	if (!Resource_CreateHandle(wdv, &newResource)) {
		return WDV_False;
	}

	/* Does URI exist? */
	if(!Resource_GetByUri(wdv, uri, newResource)) {
		Resource_DestroyHandle(wdv, newResource);

		return WDV_False;
	}

	Resource_GetIdAsString(newResource, newResourceId);

	/* Destroy parent resource */
	Resource_DestroyHandle(wdv, newResource);

	/* Build SQL statement */
	sp77sprintf( sqlStatement, 256, "SELECT NAME FROM WEBDAV_INODE WHERE PID = X'%s'", newResourceId );

    /* Get get handle */
	GetGetHandle(wdv, &hGet);

    /* Get ODBC handles */
	GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &statementHandle);

	/* Execute SQL */
	sqlRC = SQLExecDirect(statementHandle, sqlStatement, strlen((char*)sqlStatement));
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		SQLFreeStmt(statementHandle, SQL_DROP);

		return WDV_False;
	}

    /* Bind output columns */
/*    sqlRC = SQLBindCol( statementHandle, 1, SQL_C_CHAR, &hGet->resourceName, WDV_MAX_RESOURCE_NAME_LEN, &bufferLength );
*/	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		return WDV_False;
	}

	/* Set get handle */
/*	hGet->statementHandle = statementHandle;
	hGet->eol = WDV_False;
*/
	/* Fetch rowset */
/*    sqlRC = SQLFetch( hGet->statementHandle );
*/
	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA_FOUND) {
            Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
/*			hGet->eol = WDV_True;
*/
			return WDV_True;
        } else {
            Error_AddSQLErrorItem( wdv, statementHandle, sqlRC );
		}

/*		SQLFreeStmt(hGet->statementHandle, SQL_DROP);
		hGet->statementHandle = 0;
*/
		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_GetItem( WDVCAPI_WDV                 wdv,
						  WDVCAPI_ResourceNameString  resourceName,
						  WDV_Length                  resourceNameLen )
{

	SQLRETURN         sqlRC = 0;
    WDVCAPI_GetHandle hGet = NULL;

	/* Checks */
    if (!wdv || !resourceName) {
        return WDV_False;
    }

    /* Get handle */
    GetGetHandle(wdv, &hGet);
/*	if (!hGet || !hGet->statementHandle) {
		Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_INVALID_HANDLE,
				            WDVCAPI_ERR_TEXT_INVALID_HANDLE );

		hGet->eol = WDV_True;

		return WDV_False;
	}
*/
	/* Set resource */
/*	strncpy(resourceName, hGet->resourceName, resourceNameLen);
	resourceName[resourceNameLen] = '\0';
*/
	/* Fetch rowset */
/*    sqlRC = SQLFetch( hGet->statementHandle );
*/
	/* Error handling */
/*	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA_FOUND) {
            Error_AddSQLErrorItem( wdv, hGet->statementHandle, sqlRC );
			hGet->eol = WDV_True;

			return WDV_True;
        } else {
            Error_AddSQLErrorItem( wdv, hGet->statementHandle, sqlRC );
		}

		SQLFreeStmt(hGet->statementHandle, SQL_DROP);
		hGet->statementHandle = 0;

		return WDV_False;
	}
*/
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_XQuery( WDVCAPI_WDV           wdv,
                         WDVCAPI_URI           uri,
                         WDVCAPI_LockIdString  lockIdString,
                         unsigned char        *xdbXQuery,
                         SAPDB_Long            xdbXQueryLen,
                         WDVCAPI_OutStream     outStream,
                         void                 *outStreamUserData )
{

	SQLRETURN                sqlRC = 0;
    SQLHSTMT                 hStmtXDBProcCall = 0;
    SQLStreamDesc            streamDesc;
    SQL_LC_StreamParm       *StreamParam;
    WDV_Bool                 ok = WDV_False;
    WDVCAPI_GetHandle        hGet = wdv->hGet;
    WDVCAPI_LockHandle       hLock = NULL;
    WDVCAPI_LockId           lockId;
    WDVCAPI_Handle           hdl = 0;
    SQLLEN                   xdbXQueryIndicator = xdbXQueryLen;

    /* Get doc id */
    if (!wdv || !uri) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                           WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
		return WDV_False;
    }

    /* Start transaction */
    WDV_StartTransaction(wdv);

    /* Get if resource exists */
    if (!Resource_GetByUri(wdv, uri, hGet->resource)) {
        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Is content stored in XDB? */
    if (hGet->resource->xdbDocId == 0) {
		Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                            WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                            WDVCAPI_ERR_TEXT_INTERNAL_ERROR);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    hGet->outStream = outStream;
    hGet->outStreamUserData = outStreamUserData;

    /* Process XQuery */

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

    streamDesc.WriteProc = &WriteStream;
    streamDesc.ReadProc  = NULL;

    /* Allocat statement handle */
	sqlRC = SQLAllocStmt(wdv->hDBC, &hStmtXDBProcCall);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );
    	SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
        hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hStmtXDBProcCall, (SQLCHAR *) SQL_XDB_XQUERY, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );
    	SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
        hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

		return WDV_False;
	}

    /* Bind input parameter */
    sqlRC = SQLBindParameter(hStmtXDBProcCall, 1,
                                SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                xdbXQuery, xdbXQueryLen,
                                &xdbXQueryIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmtXDBProcCall, sqlRC);

        SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
        hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

		return WDV_False;
	}

    sqlRC = SQLBindParameter(hStmtXDBProcCall, 2,
                                SQL_PARAM_INPUT, SQL_C_ULONG, SQL_BIGINT, 0, 0,
                                &hGet->resource->xdbDocId, 0,
                                &hGet->resource->xdbDocIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem(wdv, hStmtXDBProcCall, sqlRC);

        SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
        hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);

		return WDV_False;
	}

    /* Bind stream parameter */
    HdlMan_NewHandle(&hdl, hGet);

    StreamParam->C_1.Stream.hStream.colDesc[0].memberType = STYPE_CHAR;
    StreamParam->C_1.Stream.hStream.colDesc[0].length     = 1;
    StreamParam->C_1.Stream.hStream.colDesc[0].offset     = 0;
    StreamParam->C_1.Stream.hStream.ABAPTabId             = hdl;

    sqlRC = SQLBindParameter( hStmtXDBProcCall, 3,
                              SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_ABAPHANDLE, 0, 0,
                              &streamDesc, sizeof(streamDesc), NULL);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );

		SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);
		hStmtXDBProcCall = 0;

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
        HdlMan_FreeHandle(hdl);

		return WDV_False;
	}

	/* Execute SQL */
	sqlRC = SQLExecute(hStmtXDBProcCall);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmtXDBProcCall, sqlRC );
        SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);

        WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
        HdlMan_FreeHandle(hdl);

        return WDV_False;
	}

    /* Close statement handle */
    SQLFreeStmt(hStmtXDBProcCall, SQL_DROP);

    WDVCAPI_Free((WDV_UInt1*) streamDesc.StreamParam);
    HdlMan_FreeHandle(hdl);

    return WDV_True;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

WDV_Bool CreateGetHandle( WDVCAPI_WDV  wdv )
{

	WDV_Bool           ok = WDV_False;
    WDVCAPI_GetHandle  hGet = NULL;

	if ( !wdv ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_get_handle), (WDV_UInt1**) &hGet, &ok );
	if (ok == WDV_True) {
        Get_InitHandle(wdv, hGet);
        SetGetHandle(wdv, hGet);

		return WDV_True;
	} else {
		Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_NO_MEMORY,
				            WDVCAPI_ERR_TEXT_NO_MEMORY);

        SetGetHandle(wdv, NULL);

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyGetHandle( WDVCAPI_WDV  wdv )
{

    WDVCAPI_GetHandle  hGet = NULL;
    unsigned long      compressedLength = 0;

	/* Checks */
    if (!wdv ) {
        return WDV_False;
    }

    /* Get handle */
    GetGetHandle(wdv, &hGet);
	if (!hGet) {
		Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_INVALID_HANDLE,
				            WDVCAPI_ERR_TEXT_INVALID_HANDLE );

		return WDV_False;
	}

    /* Close decompression */
    if (hGet->decompressionInitialized == WDV_True) {
        ZLib_gzclose(hGet->hStmtContainer, &compressedLength);
        hGet->gzFile = NULL;
        hGet->decompressionInitialized = WDV_False;
    }

	/* Destroy get handle */
	Resource_DestroyHandle(wdv, hGet->resource);
	Resource_DestroyHandle(wdv, hGet->childResource);

	/* Free statement handle */
	if ( hGet->hStmtGetLengthAndType) {
		SQLFreeStmt(hGet->hStmtGetLengthAndType, SQL_DROP);
        hGet->hStmtGetLengthAndType = 0;
	}

	/* Free statement handle */
	if ( hGet->hStmtContainer ) {
		SQLFreeStmt(hGet->hStmtContainer, SQL_DROP);
        hGet->hStmtGetLengthAndType = 0;
	}

	/* Free statement handle */
	if ( hGet->hStmtGetCollectionProps) {
		SQLFreeStmt(hGet->hStmtGetCollectionProps, SQL_DROP);
        hGet->hStmtGetCollectionProps = 0;
	}

    /* Free get handle */
    WDVCAPI_Free((WDV_UInt1*) hGet );

    /* Set get handle */
    SetGetHandle(wdv, NULL);

	return WDV_True;

}

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool Get_InitHandle( WDVCAPI_WDV        wdv,
                         WDVCAPI_GetHandle  hGet)
{

    /* Inits */
	hGet->hStmtContainer = 0;
	hGet->hStmtGetLengthAndType = 0;
    hGet->hStmtGetCollectionProps = 0;

    hGet->firstBlockDone = WDV_False;

    hGet->contentLength = 0;
    hGet->contentTypeString[0] = '\0';
    hGet->lastModified[0] = '\0';

    memset(hGet->propertyId, 0, WDVCAPI_MAX_ID_LEN);
    hGet->propertyIdIndicator = WDVCAPI_MAX_ID_LEN;

    hGet->propertyShortValue[0] = '\0';
    hGet->propertyShortValueIndicator = SQL_NTS;

    hGet->collectionPropsCount[0] = '\0';
    hGet->collectionPropsCountIndicator = SQL_NTS;
    hGet->collectionPropsSize[0] = '\0';
    hGet->collectionPropsSizeIndicator = SQL_NTS;
    hGet->collectionPropsResourceType[0] = '\0';
    hGet->collectionPropsResourceTypeIndicator = SQL_NTS;

    /* Create and init resource object */
     if (!Resource_CreateHandle(wdv, &hGet->resource )) {
        return WDV_False;
    }

    hGet->childResource = NULL;

    /* Init decompression */
    hGet->decompressionInitialized = WDV_False;
    hGet->gzFile = NULL;

    /* Init range values */
    hGet->rangeStartPos = -1;
    hGet->rangeEndPos = -1;
    hGet->readLength = 0;
    hGet->startNotReached = WDV_True;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Get_GetProperties( WDVCAPI_WDV          wdv,
                            WDVCAPI_GetHandle    hGet )
{

	SQLRETURN                           sqlRC = 0;
    SQLHDBC                             hDBC = 0;
    WDVCAPI_PropertyShortValueBuffer    resourceType = "";
    WDV_UInt4                           i;

    /* Statement already parsed? */
    if (!hGet->hStmtGetLengthAndType) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hGet->hStmtGetLengthAndType);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetLengthAndType, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetLengthAndType, SQL_DROP);
            hGet->hStmtGetLengthAndType = 0;

		    return WDV_False;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hGet->hStmtGetLengthAndType, (SQLCHAR*)SQL_GET_GET_PROPERTIES, SQL_NTS );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetLengthAndType, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetLengthAndType, SQL_DROP);
            hGet->hStmtGetLengthAndType = 0;

		    return WDV_False;
	    }

        /* Bind input parameter */
        sqlRC = SQLBindParameter( hGet->hStmtGetLengthAndType, 1,
                                  SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                  hGet->resource->id, WDVCAPI_MAX_ID_LEN,
                                  &hGet->resource->idIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hGet->hStmtGetLengthAndType, sqlRC);

            SQLFreeStmt(hGet->hStmtGetLengthAndType, SQL_DROP);
            hGet->hStmtGetLengthAndType = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        sqlRC = SQLBindCol(hGet->hStmtGetLengthAndType, 1, SQL_C_BINARY,
                           hGet->propertyId, WDVCAPI_MAX_ID_LEN,
                           &hGet->propertyIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetLengthAndType, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetLengthAndType, SQL_DROP);
            hGet->hStmtGetLengthAndType = 0;

		    return WDV_False;
	    }

        sqlRC = SQLBindCol(hGet->hStmtGetLengthAndType, 2, SQL_C_CHAR,
                           hGet->propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                           &hGet->propertyShortValueIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtGetLengthAndType, sqlRC );
    	    SQLFreeStmt(hGet->hStmtGetLengthAndType, SQL_DROP);
            hGet->hStmtGetLengthAndType = 0;

		    return WDV_False;
	    }
    } else {
        /* Close statement */
    	SQLFreeStmt(hGet->hStmtGetLengthAndType, SQL_CLOSE);
    }

	/* Execute SQL */
	sqlRC = SQLExecute(hGet->hStmtGetLengthAndType);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hGet->hStmtGetLengthAndType, sqlRC );

		return WDV_False;
	}

	/* Fetch rowset */
    for (i=0; i<3; i++) {
        sqlRC = SQLFetch(hGet->hStmtGetLengthAndType);
	    if ( sqlRC != 0 ) {
            if (sqlRC == SQL_NO_DATA) {
                /* Get resource type of existing resource */
                if (!Property_GetShortValue(wdv, hGet->resource->id, ID_PROPERTY_RESOURCE_TYPE, resourceType)) {
			        return WDV_False;
                }

                /* The resource type is not a collection? */
                if (strcmp(resourceType, PROPERTY_VALUE_COLLECTION) != 0) {
                    Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                    WDVCAPI_ERR_CODE_PROPERTY_DOESNT_EXIST,
                                    WDVCAPI_ERR_TEXT_PROPERTY_DOESNT_EXIST);

        		    return WDV_False;
                }
            } else {
                Error_AddSQLErrorItem( wdv, hGet->hStmtGetLengthAndType, sqlRC );

    		    return WDV_False;
            }
	    }

        if (WDVCAPI_IdsAreIdentical(hGet->propertyId, ID_PROPERTY_GET_CONTENT_LENGTH)) {
            hGet->contentLength = atol(hGet->propertyShortValue);
        } else if (WDVCAPI_IdsAreIdentical(hGet->propertyId, ID_PROPERTY_GET_CONTENT_TYPE)) {
            SAPDB_memcpy(hGet->contentTypeString, hGet->propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
        } else if (WDVCAPI_IdsAreIdentical(hGet->propertyId, ID_PROPERTY_GET_LAST_MODIFIED)) {
            SAPDB_memcpy(hGet->lastModified, hGet->propertyShortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN);
        }
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/
    
WDV_Bool Get_OpenContainer( WDVCAPI_WDV         wdv,
                            WDVCAPI_GetHandle   hGet )
{

	SQLRETURN         sqlRC = 0;
    SQLHDBC           hDBC = 0;
	SQLCHAR           sqlStmt [256] = "";
	WDVCAPI_IdString  cidString = "";

    hGet->firstBlockDone = WDV_False;

    /* Is statement already parsed? */
    if (!hGet->hStmtContainer) {
        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hGet->hStmtContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hGet->hStmtContainer, sqlRC);

		    return WDV_False;
	    }

        sqlRC = SQLPrepare(hGet->hStmtContainer, (SQLCHAR*)SQL_GET_CONTENT, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hGet->hStmtContainer, sqlRC);
            SQLFreeStmt(hGet->hStmtContainer, SQL_DROP);
            hGet->hStmtContainer = 0;

		    return WDV_False;
	    }

        /* Bind input parameters */
        sqlRC = SQLBindParameter(hGet->hStmtContainer, 1,
                                 SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 0, 0,
                                 hGet->resource->id, WDVCAPI_MAX_ID_LEN,
                                 &hGet->resource->idIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hGet->hStmtContainer, sqlRC);

            SQLFreeStmt(hGet->hStmtContainer, SQL_DROP);
            hGet->hStmtContainer = 0;

		    return WDV_False;
	    }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Get_ReadContainer( WDVCAPI_WDV           wdv,
                            WDVCAPI_GetHandle     hGet,
                            WDVCAPI_Buffer        buffer,
                            WDVCAPI_BufferLength  bufferLength,
                            WDV_Long             *bufferStartPos,
                            WDVCAPI_BufferLength *readLength )
{

	SQLRETURN               sqlRC = 0;
    WDVCAPI_ErrorItem       errorItem = NULL;
    WDVCAPI_ErrorState      errorState;
    WDVCAPI_BufferLength    firstBufferLength = 0;
    WDV_UInt4               compressedLength = 0;
    WDV_Bool                rangeStartNotReached = WDV_True;
    WDV_Bool                breakLoop = WDV_False;

    if (hGet->firstBlockDone == WDV_False) {
        /* Check buffer length                                   */
        /* The size of the first block hast to be at least 8000! */
        if (bufferLength < 8000) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_BUFFER_IS_TOO_SMALL,
                               WDVCAPI_ERR_TEXT_BUFFER_IS_TOO_SMALL);

            return WDV_False;
        }

        hGet->firstBlockDone = WDV_True;

        sqlRC = SQLBindCol(hGet->hStmtContainer, 1, SQL_C_BINARY,
                           buffer, bufferLength,
                           (SQLLEN*)&firstBufferLength);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hGet->hStmtContainer, sqlRC );
            SQLFreeStmt(hGet->hStmtContainer, SQL_DROP);
            hGet->hStmtContainer = 0;

		    return WDV_False;
	    }
    
        sqlRC = SQLExecute(hGet->hStmtContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hGet->hStmtContainer, sqlRC);

		    return WDV_False;
	    }

        sqlRC = SQLFetch(hGet->hStmtContainer);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem(wdv, hGet->hStmtContainer, sqlRC);

		    return WDV_False;
	    }
    }

    /* Is there more to read? */
    if (hGet->contentLength > firstBufferLength) {
        breakLoop = WDV_False;
        do {
            /* Decompress content */
            if (hGet->compressedLength > 0) {
                if (!Get_GetCompressedData(wdv, hGet, (char*)buffer + firstBufferLength,
                                           bufferLength - firstBufferLength, readLength)) {
                    return WDV_False;
                }

                *readLength += firstBufferLength;
            } else {
                sqlRC = SQLGetData(hGet->hStmtContainer, 2, SQL_C_BINARY, (char*)buffer + firstBufferLength,
                                   bufferLength - firstBufferLength, (SQLLEN*)readLength);
                if (sqlRC == 100) { /* No more data */
                    breakLoop = WDV_True;
                } else {
	                if ( sqlRC != 0 ) {
                        Error_AddSQLErrorItem(wdv, hGet->hStmtContainer, sqlRC);

                        if (!WDVCAPI_GetLastError(wdv, &errorItem)) {
                            return WDV_False;
                        }

                        WDVCAPI_GetErrorState(errorItem, &errorState);
                        if (strcmp(errorState, WDVCAPI_ERR_SQL_STATE_DATA_TRUNCATED)) {
                            return WDV_False;
                        }

                        /* ???Here we have to drop the last error due to memory limits */

                        *readLength = bufferLength;
                    } else {
                        *readLength += firstBufferLength;
                    }

                    /* Cummulate read length */
                    hGet->readLength += *readLength;

                    if (*readLength >= bufferLength) {
                        breakLoop = WDV_True;
                    }
                }
            }
        } while (breakLoop == WDV_False && (!hGet->startNotReached || (hGet->startNotReached && hGet->rangeStartPos > (WDV_Long)hGet->readLength)));
    } else {
        *readLength = hGet->contentLength;
        hGet->readLength = *readLength;
    }

    /* Do we have found the first? */
    if (hGet->startNotReached) {
        /* Range start in first block */
        if (hGet->rangeStartPos != -1) {
            *bufferStartPos = (WDV_ULong)*readLength - (hGet->readLength - hGet->rangeStartPos);
        } else {
            *bufferStartPos = 0;
        }

        /* Update read length according range end (start and end in this buffer) */
        if (hGet->rangeEndPos != -1 && hGet->readLength >= hGet->rangeEndPos) {
            *readLength = (WDV_ULong)(hGet->rangeEndPos - hGet->rangeStartPos);
        }

        hGet->startNotReached = WDV_False;
    } else {
        /* Update read length according range end */
        if (hGet->rangeEndPos != -1 && hGet->readLength >= hGet->rangeEndPos) {
            *readLength = *readLength - (WDV_ULong)(hGet->readLength - hGet->rangeEndPos);
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Get_CloseContainer( WDVCAPI_WDV        wdv,
                             WDVCAPI_GetHandle  hGet )
{

    unsigned long   compressedLength = 0;

    /* Close decompression */
    if (hGet->decompressionInitialized == WDV_True) {
        ZLib_gzclose(hGet->gzFile, &compressedLength);
        hGet->gzFile = NULL;
        hGet->decompressionInitialized = WDV_False;
    }

    /* Close statement handle */
    SQLFreeStmt(hGet->hStmtContainer, SQL_CLOSE);

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Get_GetCompressedData( WDVCAPI_WDV              wdv,
                                WDVCAPI_GetHandle        hGet,
                                WDVCAPI_Buffer           buffer,
                                WDVCAPI_BufferLength     bufferLength,
                                WDVCAPI_BufferLength    *readLength )
{

    int         zRc;
	SQLRETURN   sqlRC = 0;

    /* Decompression initialized? */
    if (hGet->decompressionInitialized == WDV_False) {
        hGet->gzFile = ZLib_gzopen (hGet->hStmtContainer, "rb9");

        if (hGet->gzFile) {
            hGet->decompressionInitialized = WDV_True;
        } else {
            return WDV_False;
        }
    }

    /* Read and decompress */
    zRc = ZLib_gzread(hGet->gzFile, buffer, bufferLength);
    if (zRc == -1) {
        return WDV_False;
    }

    *readLength = zRc;

    return WDV_True;

}

/*===========================================================================*/

int WriteStream( SQL_LC_StreamParm *StreamParam, void *rgbInStream, int noOfRows, int cbStreamLen, void* pMask )
{

    WDVCAPI_Handle       hdl = 0;
    WDVCAPI_GetHandle    hGet;
    SAPDB_UInt4          copyLen = 0;
    SAPDB_UInt4          currLen = 0;
    WDVCAPI_OutStream    outStream;

    hdl = StreamParam->C_1.Stream.hStream.ABAPTabId;

    HdlMan_LeaseHandleData(hdl, (void*)&hGet);
    if (!hGet) {
        HdlMan_ReleaseHandleData(hdl);
        return SQL_STREAM_ERROR;
    }
    outStream = *hGet->outStream;
    outStream(hGet->outStreamUserData, rgbInStream, cbStreamLen);

    HdlMan_ReleaseHandleData(hdl);

    return SQL_STREAM_OK;

}

/***********************************************************************

	End

 ***********************************************************************/
