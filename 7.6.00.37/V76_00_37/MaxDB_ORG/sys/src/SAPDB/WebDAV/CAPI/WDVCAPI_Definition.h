/*!**********************************************************************

  module: WDVCAPI_Definition.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV API Definition

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

#ifndef WDVCAPI_DEFINITION_H
#define WDVCAPI_DEFINITION_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WINDOWS.H>

#ifndef _WIN32
#include <string.h>
#endif

#include "vpa00sql.h"
#include "vpa00sqlext.h"

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"
#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define WDV_PATH_SEPARATOR	    '/'
#define WDV_PATH_SEPARATOR_STR  "/"

/*---------------------------------------------------------------------------*/

#define WDV_MAX_ERROR_TEXT_LEN                      1000
#define WDV_MAX_STATE_TEXT_LEN                       100

#define WDV_MAX_URI_LEN                             1000

#define WDV_MAX_RESOURCE_NAME_LEN                    499

#define WDV_MAX_NAME_SPACE_SHORTCUT_LEN               16
#define WDV_MAX_NAME_SPACE_LEN                       499

#define WDV_MAX_PROPERTY_NAME_PREFIX_LEN             300
#define WDV_MAX_PROPERTY_SHORT_VALUE_LEN             450
#define WDV_MAX_PROPERTY_LONG_VALUE_LEN             8000

#define WDVCAPI_MAX_CONTENT_TYPE_LEN                WDV_MAX_PROPERTY_SHORT_VALUE_LEN

#define WDV_MAX_LOCK_TIMEOUT_LEN                      19
#define WDV_MAX_LOCK_OWNER_LEN                       450

/***********************************************************************

	Types

 ***********************************************************************/

/* Basic types */
typedef unsigned int   WDV_UInt4;
typedef unsigned short WDV_UInt2;
typedef unsigned char  WDV_UInt1;
typedef short          WDV_Int2;
typedef int            WDV_Int4;
typedef unsigned char  WDV_Bool;
typedef size_t         WDV_Length;

/*--------------------------------------------------------------------------
declaration: sapdbwa_{ULong | Long}
description: basic long data types for fastest execution
             (machine-dependent size)
--------------------------------------------------------------------------*/
#if defined(WIN32) && defined(BIT64)
typedef unsigned __int64    WDV_ULong;      /* long (fast, unsigned) */
typedef signed __int64      WDV_Long;       /* long (fast, signed)   */
#else
typedef unsigned long       WDV_ULong;      /* long (fast, unsigned) */
typedef signed long         WDV_Long;       /* long (fast, signed)   */
#endif

#define WDV_True	((WDV_Bool)1)
#define WDV_False	((WDV_Bool)0)

/* WDVCAPI */
typedef struct st_wdvcapi_wdv             *WDVCAPI_WDV;
typedef char                              *WDVCAPI_URI;
typedef char                               WDVCAPI_URIString[WDV_MAX_URI_LEN+1];

/*---------------------------------------------------------------------------*/

#define WDV_MAX_BIG_LENGTH_LENGTH  64

#define WDV_NAME_SPACE_DAV          "DAV:"

#define WDV_PROPERTY_NAME_DISPLAYNAME       "displayname"
#define WDV_PROPERTY_NAME_GETCONTENTTYPE    "getcontenttype"

typedef char                        WDVCAPI_BigLengthString[WDV_MAX_BIG_LENGTH_LENGTH+1];
typedef char                       *WDVCAPI_BigLength;

typedef struct st_wdvcapi_admin    *WDVCAPI_AdminHandle;


WDV_Bool WDVCAPI_AdminTotalSumContentLength( WDVCAPI_WDV        wdv,
                                             WDVCAPI_BigLength  sumContentLength );

WDV_Bool WDVCAPI_AdminTotalSumCompressedContentLength( WDVCAPI_WDV        wdv,
                                                       WDVCAPI_BigLength  sumCompressedContentLength );

WDV_Bool WDVCAPI_AdminTotalSumPhysicalLength( WDVCAPI_WDV        wdv,
                                              WDVCAPI_BigLength  sumPhysicalLength );

WDV_Bool WDVCAPI_AdminTotalCompressedRatio( WDVCAPI_WDV        wdv,
                                            WDVCAPI_BigLength  sumLogicalCompressedLength,
                                            WDVCAPI_BigLength  sumPhysicalCompressedLength,
                                            WDVCAPI_BigLength  sumRatio );

WDV_Bool WDVCAPI_AdminTotalSumOfResources( WDVCAPI_WDV        wdv,
                                           WDVCAPI_BigLength  sumOfResources );

WDV_Bool WDVCAPI_AdminTotalSumOfCollections( WDVCAPI_WDV        wdv,
                                             WDVCAPI_BigLength  sumOfCollections );

/*---------------------------------------------------------------------------*/

#define WDVCAPI_MAX_ID_LEN              24
#define WDVCAPI_MAX_ID_STRING_LEN       2*WDVCAPI_MAX_ID_LEN
#define WDVCAPI_MAX_ID_DEBUG_STRING_LEN 52  /* See also RTE_UniqueIdBuffer in RTE_UniqueId.h */

typedef char                            WDVCAPI_Id[WDVCAPI_MAX_ID_LEN];
typedef char                            WDVCAPI_IdString[WDVCAPI_MAX_ID_STRING_LEN+1];
typedef char                            WDVCAPI_IdDebugString[WDVCAPI_MAX_ID_DEBUG_STRING_LEN+1];

/*---------------------------------------------------------------------------*/

typedef char                              *WDVCAPI_ResourceName;
typedef char                               WDVCAPI_ResourceNameString[WDV_MAX_RESOURCE_NAME_LEN+1];

typedef enum en_wdvcapi_resource_state {
    WDVCAPI_RESOURCE_STATE_DEFAULT = 0,
    WDVCAPI_RESOURCE_STATE_NULL_RESOURCE
} WDVCAPI_ResourceState;

/*---------------------------------------------------------------------------*/

typedef void                              *WDVCAPI_Buffer;
typedef WDV_ULong                          WDVCAPI_BufferLength;

/*---------------------------------------------------------------------------*/

typedef WDV_ULong                          WDVCAPI_ContentLength;
typedef WDV_UInt2                          WDVCAPI_ContentTypeId;
typedef char                              *WDVCAPI_ContentType;
typedef char                               WDVCAPI_ContentTypeString[WDVCAPI_MAX_CONTENT_TYPE_LEN+1];

/*---------------------------------------------------------------------------*/

typedef enum en_wdvcapi_overwrite {
    WDVCAPI_NO_OVERWRITE = 0,           /* No overwrite             */
    WDVCAPI_OVERWRITE,                  /* Overwrite if we copy     */
                                        /* Merge if we move         */
    WDVCAPI_DELETE_BEFORE_OVERWRITE     /* Overwrite if we move     */
} WDVCAPI_Overwrite;

/*---------------------------------------------------------------------------*/

typedef struct st_container_handle        *WDVCAPI_ContainerHandle;

/*---------------------------------------------------------------------------*/

typedef struct st_property_handle         *WDVCAPI_PropertyHandle;

/*---------------------------------------------------------------------------*/

typedef struct st_wdvcapi_propfind        *WDVCAPI_PropfindHandle;
typedef char                              *WDVCAPI_PropfindNameSpaceShortcut;
typedef char                              *WDVCAPI_PropfindNameSpace;
typedef char                              *WDVCAPI_PropfindPropertyNameSpace;
typedef char                              *WDVCAPI_PropfindPropertyName;
typedef char                              *WDVCAPI_PropfindPropertyShortValue;

typedef enum en_wdvcapi_depth {
    WDVCAPI_DEPTH_UNDEFINED = 0,
    WDVCAPI_DEPTH_0,
    WDVCAPI_DEPTH_1,
    WDVCAPI_DEPTH_INFINITE
} WDVCAPI_Depth;

typedef enum en_wdvcapi_property_status {
    WDVCAPI_PROPERTY_STATUS_UNDEFINED = 0,
    WDVCAPI_PROPERTY_STATUS_OK,
    WDVCAPI_PROPERTY_STATUS_FAILED
} WDVCAPI_PropertyStatus;

typedef enum en_wdvcapi_propfind_type {
    WDVCAPI_PROPFIND_TYPE_UNDEFINED = 0,
    WDVCAPI_PROPFIND_TYPE_PROPFIND,
    WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL,
    WDVCAPI_PROPFIND_TYPE_PROPFIND_NAME
} WDVCAPI_PropfindType;

/*---------------------------------------------------------------------------*/

typedef struct st_wdvcapi_proppatch_handle  *WDVCAPI_ProppatchHandle;

typedef enum en_wdvcapi_proppatch_access_type {
    WDVCAPI_PROPPATCH_ACCESS_TYPE_UNDEFINED = 0,
    WDVCAPI_PROPPATCH_ACCESS_TYPE_REMOVE,
    WDVCAPI_PROPPATCH_ACCESS_TYPE_SET
} WDVCAPI_ProppatchAccessType;

typedef char                                *WDVCAPI_NameSpaceShortcut;
typedef char                                 WDVCAPI_NameSpaceShortcutString[WDV_MAX_NAME_SPACE_SHORTCUT_LEN+1];

typedef char                                *WDVCAPI_NameSpace;
typedef char                                 WDVCAPI_NameSpaceString[WDV_MAX_NAME_SPACE_LEN+1];

typedef char                                *WDVCAPI_PropertyName;
typedef char                                 WDVCAPI_PropertyNameString[WDV_MAX_PROPERTY_NAME_PREFIX_LEN+1];

typedef char                                *WDVCAPI_PropertyShortValue;
typedef char                                 WDVCAPI_PropertyShortValueBuffer[WDV_MAX_PROPERTY_SHORT_VALUE_LEN+1];

/*---------------------------------------------------------------------------*/

typedef WDV_ULong           WDVCAPI_LockTimeout;

typedef enum en_wdvcapi_lock_scope {
    WDVCAPI_LOCK_SCOPE_UNDEFINED = 0,
    WDVCAPI_LOCK_SCOPE_EXCLUSIVE
} WDVCAPI_LockScope;

typedef enum en_wdvcapi_lock_type {
    WDVCAPI_LOCK_TYPE_UNDEFINED = 0,
    WDVCAPI_LOCK_TYPE_WRITE
} WDVCAPI_LockType;

typedef enum en_wdvcapi_supported_lock {
    WDVCAPI_SUPPORTED_LOCK_UNDEFINED = 0,
    WDVCAPI_SUPPORTED_LOCK_EXCLUSIVE_WRITE
} WDVCAPI_SupportedLock;

typedef char               *WDVCAPI_LockOwner;
typedef char                WDVCAPI_LockOwnerString[WDV_MAX_LOCK_OWNER_LEN+1];

typedef WDVCAPI_Id          WDVCAPI_LockId;
typedef WDVCAPI_IdString    WDVCAPI_LockIdString;


typedef struct st_wdvcapi_lock_description  *WDVCAPI_LockHandle;
typedef struct st_wdvcapi_lock_id_list      *WDVCAPI_LockIdList;

struct st_wdvcapi_lock_description {

    WDVCAPI_Id              resourceId;
    WDVCAPI_LockId          id;
    WDVCAPI_Depth           depth;
    WDVCAPI_LockTimeout     timeout;
    WDVCAPI_LockScope       scope;
    WDVCAPI_LockType        type;
    WDVCAPI_LockOwnerString owner;

    WDVCAPI_LockHandle      nextFreeLock;

};

/*---------------------------------------------------------------------------*/

typedef enum en_wdvcapi_char_encoding {
    WDVCAPI_CHAR_ENCODING_UNDEFINED = 0,
    WDVCAPI_CHAR_ENCODING_ASCII,
    WDVCAPI_CHAR_ENCODING_UTF8
} WDVCAPI_CharEncoding;


/*---------------------------------------------------------------------------*/

#define WDVCAPI_MAX_DOC_CLASS_ID_LEN    WDVCAPI_MAX_ID_LEN

#define XMLINDEXING_STATE_NEW           "1"
#define XMLINDEXING_STATE_IN_PROCESS    "2"
#define XMLINDEXING_STATE_FINISHED      "3"

#define XMLINDEXING_STATE_NEW_AS_STRING         "Document not indexed"
#define XMLINDEXING_STATE_IN_PROCESS_AS_STRING  "Indexing in process"
#define XMLINDEXING_STATE_FINISHED_AS_STRING    "Indexing finished"

typedef WDVCAPI_Id                      WDVCAPI_XmlDocClassId;

/*---------------------------------------------------------------------------*/

#define WDVCAPI_DELETED_ITEMS_FOLDER        "/Deleted Items"
#define WDVCAPI_DELETED_ITEMS_FOLDER_LEN    14

/*---------------------------------------------------------------------------*/

/* Extended error description */
#define WDVCAPI_COMPREHENSIVE_ERROR_DESCRIPTION     1

typedef struct st_wdvcapi_error_item      *WDVCAPI_ErrorItem;
typedef char                               WDVCAPI_ErrorTextString[WDV_MAX_ERROR_TEXT_LEN+1];
typedef char                               WDVCAPI_ErrorStateString[WDV_MAX_STATE_TEXT_LEN+1];
typedef char                              *WDVCAPI_ErrorText;
typedef char                              *WDVCAPI_ErrorState;
typedef SQLINTEGER                         WDVCAPI_ErrorCode;

typedef void (*WDVCAPI_ClientErrorHandling)( void              *clientData,
                                             WDVCAPI_URI        uri,
                                             WDVCAPI_ErrorItem  error );

/* Error types */
typedef enum en_wdvcapi_error_type {
	WDVCAPI_ERR_TYPE_UNDEFINED,
	WDVCAPI_ERR_TYPE_CAPI,
	WDVCAPI_ERR_TYPE_SQL
} WDVCAPI_ErrorType;

/* C API error codes */
typedef enum en_wdvcapi_error_code {
    WDVCAPI_ERR_CODE_UNDEFINED,
    WDVCAPI_ERR_CODE_PARENT_DOESNT_EXIST,
    WDVCAPI_ERR_CODE_COLLECTION_ALREADY_EXIST,
    WDVCAPI_ERR_CODE_INVALID_HANDLE,
    WDVCAPI_ERR_CODE_ROOT_ALREADY_EXIST,
    WDVCAPI_ERR_CODE_CANNOT_FIND_NEW_RESOURCE,
	WDVCAPI_ERR_CODE_NO_MEMORY,
	WDVCAPI_ERR_CODE_COLLECTION_DOESNT_EXIST,
	WDVCAPI_ERR_CODE_RESOURCE_NAME_TOO_LONG,
	WDVCAPI_ERR_CODE_INTERNAL_ERROR,
	WDVCAPI_ERR_CODE_UNKNOWN_ODBC_RETURN_CODE,
    WDVCAPI_ERR_CODE_WRONG_DEPTH,
    WDVCAPI_ERR_CODE_NO_MORE_SIBLING_NODES,
    WDVCAPI_ERR_CODE_NO_MORE_CHILD_NODES,
    WDVCAPI_ERR_CODE_NO_MORE_RESOURCES,
    WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_DEPTH,
    WDVCAPI_ERR_CODE_URI_IS_CORRUPTED,
    WDVCAPI_ERR_CODE_RESOURCE_DOESNT_EXIST,
    WDVCAPI_ERR_CODE_NO_MORE_TO_DELETE,
    WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST,
    WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_ALREADY_EXIST,
    WDVCAPI_ERR_CODE_DEPTH_1_IS_NOT_ALLOWED,
    WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION,
    WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST,
    WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT,
    WDVCAPI_ERR_CODE_COULD_NOT_DELETE_MOVED_RESOURCE_OR_COLLECTION,
    WDVCAPI_ERR_CODE_UNDEFINED_OVERWRITE,
    WDVCAPI_ERR_CODE_COULD_NOT_SET_PROPERTY,
    WDVCAPI_ERR_CODE_COULD_NOT_REMOVE_PROPERTY,
    WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_TYPE,
    WDVCAPI_ERR_CODE_EMPTY_PROPERTY_LIST,
    WDVCAPI_ERR_CODE_UNKNOWN_NAME_SPACE,
    WDVCAPI_ERR_CODE_COULD_NOT_GET_NEXT_UNIQUE_ID,
    WDVCAPI_ERR_CODE_LOCK_EXISTS,
    WDVCAPI_ERR_CODE_PROPERTY_DOESNT_EXIST,
    WDVCAPI_ERR_CODE_COULD_NOT_SET_DEFAULT_PROPERTY,
    WDVCAPI_ERR_CODE_DESTINATION_IS_CHILD_OF_SOURCE,
    WDVCAPI_ERR_CODE_LOCK_FOUND_IN_SUBTREE,
    WDVCAPI_ERR_CODE_BUFFER_IS_TOO_SMALL,
    WDVCAPI_ERR_CODE_NO_PUT_ON_COLLECTION,
    WDVCAPI_ERR_CODE_NO_ACCESS_TO_XML_MODELLING_MANAGEMENT,
    WDVCAPI_ERR_CODE_DOC_CLASS_NOT_FOUND,
    WDVCAPI_ERR_CODE_NO_ACCESS_TO_XPATH_ENGINE,
    WDVCAPI_ERR_CODE_ACCESS_TO_XPATH_ENGINE_FAILED,
    WDVCAPI_ERR_CODE_XPATH_SYNTAX_ERROR,
    WDVCAPI_ERR_CODE_RANGE_START_OUT_OF_BOUNDS,
    WDVCAPI_ERR_CODE_INDEXING_STATE_NOT_APPROPRIATE,
    WDVCAPI_ERR_CODE_NO_DOC_CLASS_PROPERTY,
    WDVCAPI_ERR_CODE_INDEXING_FAILED,
    WDVCAPI_ERR_CODE_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER,
    WDVCAPI_ERR_CODE_COULD_NOT_SET_LIVE_PROPERTY,
    WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION
} WDVCAPI_ErrorCodeList;

/* SQL error codes */
#define WDVCAPI_ERR_SQL_CODE_NO_DATA                        SQL_NO_DATA
#define WDVCAPI_ERR_SQL_CODE_INVALID_HANDLE                 SQL_INVALID_HANDLE
#define WDVCAPI_ERR_SQL_CODE_DUPLICATE_KEY                  200

/* SQL states */
#define WDVCAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED            "01S03" /* No rows updated or deleted           */
#define WDVCAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED  "01S04" /* More then one row updated or deleted */

/* User defined error types and codes */
#define WDVCAPI_ERR_TYPE_USER_DEFINED   10      /* Step by 1    */
#define WDVCAPI_ERR_CODE_USER_DEFINED   1000    /* Step by 1000 */

#define WDVCAPI_ERR_TYPE_WDVHANDLER     WDVCAPI_ERR_TYPE_USER_DEFINED + 1
#define WDVCAPI_ERR_CODE_WDVHANDLER     WDVCAPI_ERR_CODE_USER_DEFINED + 1

/*---------------------------------------------------------------------------*/

typedef void (*WDVCAPI_OutStream)( void          *userData,
                                   void          *buffer,
                                   SAPDB_Int4     writeLen );

typedef void (*WDVCAPI_InStream)( void           *userData,
                                  void           *buffer,
                                  SAPDB_Int4      bufferSize,
                                  SAPDB_Int4     *readLen );

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool WDVCAPI_CreateWDV ( WDVCAPI_WDV       *wdv,
                             SQLHENV            hEnv,
                             SQLHDBC            hDBC,
                             XMLIMAPI_Handle   hXmlModelling );

WDV_Bool WDVCAPI_DestroyWDV( WDVCAPI_WDV  wdv  );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_ClearErrorList( WDVCAPI_WDV  wdv );

WDV_Bool WDVCAPI_GetLastError    ( WDVCAPI_WDV        wdv,
							       WDVCAPI_ErrorItem *errorItem  );

WDV_Bool WDVCAPI_GetNextError    ( WDVCAPI_ErrorItem  currentItem,
							       WDVCAPI_ErrorItem *errorItem  );

WDV_Bool WDVCAPI_FindError       ( WDVCAPI_WDV        wdv,
						           WDVCAPI_ErrorType  errorType,
						           WDVCAPI_ErrorCode  errorCode,
							       WDVCAPI_ErrorItem *errorItem  );

WDV_Bool WDVCAPI_IsError         ( WDVCAPI_WDV        wdv,
                                   WDVCAPI_ErrorType  errorType,
                                   WDVCAPI_ErrorCode  errorCode  );

WDV_Bool WDVCAPI_IsState         ( WDVCAPI_WDV        wdv,
                                   WDVCAPI_ErrorType  errorType,
                                   WDVCAPI_ErrorState errorState );

void  WDVCAPI_GetErrorType( WDVCAPI_ErrorItem   error,
                            WDVCAPI_ErrorType  *errorType );

void  WDVCAPI_GetErrorCode( WDVCAPI_ErrorItem   error,
                            WDVCAPI_ErrorCode   *errorCode );

void  WDVCAPI_GetErrorText( WDVCAPI_ErrorItem   error,
                            WDVCAPI_ErrorText  *errorText );

void  WDVCAPI_GetErrorState( WDVCAPI_ErrorItem    error,
                             WDVCAPI_ErrorState  *errorState );

WDV_Bool WDVCAPI_SetErrorItemEx( WDVCAPI_ErrorItem   *errorItem,
                                 WDVCAPI_ErrorType    type,
                                 WDVCAPI_ErrorCode    code,
                                 WDVCAPI_ErrorText    text,
                                 char                *file,
                                 const int            line );

void WDVCAPI_UnsetErrorItem( WDVCAPI_ErrorItem    errorItem );

#ifdef WDVCAPI_COMPREHENSIVE_ERROR_DESCRIPTION
    #define WDVCAPI_SetErrorItem(a,b,c,d)   WDVCAPI_SetErrorItemEx(a,b,c,d,__FILE__,__LINE__)
#else
    #define WDVCAPI_SetErrorItem(a,b,c,d)   WDVCAPI_SetErrorItemEx(a,b,c,d,NULL,0)
#endif

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_IdGetNext( WDVCAPI_WDV   wdv,
                            WDVCAPI_Id    id );

void WDVCAPI_IdInitValue( WDVCAPI_Id    id );

void WDVCAPI_IdStringInitValue( WDVCAPI_IdString    idString );

void WDVCAPI_IdAsString( WDVCAPI_Id         id,
                         WDVCAPI_IdString   idString );

void WDVCAPI_IdAsDebugString( WDVCAPI_Id             id,
                              WDVCAPI_IdDebugString  idDebugString );

void WDVCAPI_IdStringAsId( WDVCAPI_IdString idString,
                           WDVCAPI_Id       id );

WDV_Bool WDVCAPI_IdIsInitialValue( WDVCAPI_Id id );

WDV_Bool WDVCAPI_IdStringIsInitialValue( WDVCAPI_IdString idString );

WDV_Bool WDVCAPI_IdsAreIdentical( WDVCAPI_Id id1,
                                  WDVCAPI_Id id2 );

WDV_Bool WDVCAPI_IdStringssAreIdentical( WDVCAPI_IdString idString1,
                                         WDVCAPI_IdString idString2 );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_MkCol( WDVCAPI_WDV             wdv,
					    WDVCAPI_URI             uri,
                        WDVCAPI_LockIdString    lockIdString );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_GetOpen( WDVCAPI_WDV           wdv,
					      WDVCAPI_URI           uri,
                          WDVCAPI_LockIdString  lockIdString,
                          WDV_Long              rangeStartPos,
                          WDV_Long              rangeEndPos,
                          WDVCAPI_OutStream     outStream,
                          void                 *outStreamUserData,
                          WDV_Bool             *isDocInXDB );

WDV_Bool WDVCAPI_Get( WDVCAPI_WDV           wdv,
                      WDVCAPI_Buffer        buffer,
                      WDVCAPI_BufferLength  bufferLength,
                      WDV_Long             *bufferStartPos,
                      WDVCAPI_BufferLength *readLength );

WDV_Bool WDVCAPI_GetXDB( WDVCAPI_WDV    wdv );

WDV_Bool WDVCAPI_GetClose( WDVCAPI_WDV  wdv );

WDV_Bool WDVCAPI_GetContentLength( WDVCAPI_WDV            wdv,
                                   WDVCAPI_ContentLength *contentLength );

WDV_Bool WDVCAPI_GetContentType( WDVCAPI_WDV          wdv,
                                 WDVCAPI_ContentType  contentType );

WDV_Bool WDVCAPI_GetLastModified( WDVCAPI_WDV                   wdv,
                                  WDVCAPI_PropertyShortValue    lastModified );

WDV_Bool WDVCAPI_GetCollectionProperties( WDVCAPI_WDV                   wdv,
					                      WDVCAPI_PropertyShortValue    collectionCount,
					                      WDVCAPI_PropertyShortValue    resourceCount,
					                      WDVCAPI_PropertyShortValue    size );

WDV_Bool WDVCAPI_GetCollection( WDVCAPI_WDV                 wdv,
					            WDVCAPI_URI                 childResourceName,
                                WDVCAPI_PropertyShortValue  resourceType,
                                WDVCAPI_PropertyShortValue  size,
                                WDVCAPI_PropertyShortValue  lastModified,
                                WDV_Bool                   *isXDBDoc,
				                WDV_Bool                   *childFound );

WDV_Bool WDVCAPI_GetNextCollection( WDVCAPI_WDV                 wdv,
					                WDVCAPI_URI                 childResourceName,
                                    WDVCAPI_PropertyShortValue  resourceType,
                                    WDVCAPI_PropertyShortValue  size,
                                    WDVCAPI_PropertyShortValue  lastModified,
                                    WDV_Bool                   *isXDBDoc,
				                    WDV_Bool                   *childFound );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_PropfindCreate( WDVCAPI_WDV             wdv,
                                 WDVCAPI_URI             uri,
                                 WDVCAPI_Depth           depth,
                                 WDVCAPI_PropfindType    type,
                                 WDVCAPI_PropfindHandle *hPropfind );

WDV_Bool WDVCAPI_PropfindDestroy( WDVCAPI_WDV            wdv,
                                  WDVCAPI_PropfindHandle propfind );

WDV_Bool WDVCAPI_PropfindAddPropertyToPropertyList ( WDVCAPI_WDV                    wdv,
                                                     WDVCAPI_PropfindHandle         propfind,
                                                     WDVCAPI_PropfindNameSpace      nameSpace,
                                                     WDVCAPI_PropfindPropertyName   propertyName );

WDV_Bool WDVCAPI_Propfind( WDVCAPI_WDV             wdv,
                           WDVCAPI_PropfindHandle  propfind );

WDV_Bool WDVCAPI_PropfindGetFirstNameSpace( WDVCAPI_WDV                        wdv,
                                            WDVCAPI_PropfindHandle             propfind,
                                            WDVCAPI_PropfindNameSpaceShortcut *nameSpaceShortcut,
                                            WDVCAPI_PropfindNameSpace         *nameSpace );

WDV_Bool WDVCAPI_PropfindGetNextNameSpace( WDVCAPI_WDV                        wdv,
                                           WDVCAPI_PropfindHandle             propfind,
                                           WDVCAPI_PropfindNameSpaceShortcut *nameSpaceShortcut,
                                           WDVCAPI_PropfindNameSpace         *nameSpace );

WDV_Bool WDVCAPI_PropfindGetNextURI( WDVCAPI_WDV             wdv,
                                     WDVCAPI_PropfindHandle  propfind,
                                     WDVCAPI_URI            *uri );

WDV_Bool WDVCAPI_PropfindGetFirstProperty( WDVCAPI_WDV              wdv,
                                           WDVCAPI_PropfindHandle   propfind );

WDV_Bool WDVCAPI_PropfindGetNextProperty( WDVCAPI_WDV              wdv,
                                          WDVCAPI_PropfindHandle   propfind );

WDV_Bool WDVCAPI_PropfindGetPropertyStatus( WDVCAPI_WDV              wdv,
                                            WDVCAPI_PropfindHandle   propfind,
                                            WDVCAPI_PropertyStatus  *propertyStatus );

WDV_Bool WDVCAPI_PropfindGetPropertyNameSpaceShortcut( WDVCAPI_WDV                        wdv,
                                                       WDVCAPI_PropfindHandle             propfind,
                                                       WDVCAPI_PropfindNameSpaceShortcut *nameSpaceShortcut );

WDV_Bool WDVCAPI_PropfindGetPropertyNameSpace( WDVCAPI_WDV                        wdv,
                                               WDVCAPI_PropfindHandle             propfind,
                                               WDVCAPI_PropfindPropertyNameSpace *propertyNameSpace );

WDV_Bool WDVCAPI_PropfindGetPropertyName( WDVCAPI_WDV                    wdv,
                                          WDVCAPI_PropfindHandle         propfind,
                                          WDVCAPI_PropfindPropertyName  *propertyName );

WDV_Bool WDVCAPI_PropfindGetPropertyShortValue( WDVCAPI_WDV                          wdv,
                                                WDVCAPI_PropfindHandle               propfind,
                                                WDVCAPI_PropfindPropertyShortValue  *propertyShortValue );

WDV_Bool WDVCAPI_PropfindGetLockDescription( WDVCAPI_WDV             wdv,
                                             WDVCAPI_PropfindHandle  hPropfind,
                                             WDVCAPI_LockIdString    lockIdString,
                                             WDVCAPI_LockHandle     *hLock );

WDV_Bool WDVCAPI_PropfindOriginalLocation( WDVCAPI_WDV                          wdv,
                                           WDVCAPI_URI                          uri,
                                           WDVCAPI_PropfindPropertyShortValue   propertyShortValue  );

/*---------------------------------------------------------------------------*/

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
                          WDV_Bool             *uriCreated );

WDV_Bool WDVCAPI_Put( WDVCAPI_WDV          wdv,
                      WDVCAPI_Buffer       buffer,
                      WDVCAPI_BufferLength bufferLength );

WDV_Bool WDVCAPI_PutXDB( WDVCAPI_WDV    wdv );

WDV_Bool WDVCAPI_PutClose( WDVCAPI_WDV  wdv );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_Delete( WDVCAPI_WDV            wdv,
                         WDVCAPI_URI            uri,
                         WDVCAPI_LockIdString   lockIdString,
                         WDV_Bool               deleteIntoDeletedItems );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_Head( WDVCAPI_WDV   wdv,
                       WDVCAPI_URI   uri );

WDV_Bool WDVCAPI_HeadGetContentLength( WDVCAPI_WDV            wdv,
                                       WDVCAPI_ContentLength *contentLength );

WDV_Bool WDVCAPI_HeadGetContentType( WDVCAPI_WDV          wdv,
                                     WDVCAPI_ContentType  contentType );

WDV_Bool WDVCAPI_HeadGetLastModified( WDVCAPI_WDV                 wdv,
                                      WDVCAPI_PropertyShortValue  lastModified );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_Copy( WDVCAPI_WDV			        wdv,
                       WDVCAPI_URI			        sourceUri,
					   WDVCAPI_URI			        destinationUri,
					   WDVCAPI_Depth		        depth,
					   WDVCAPI_Overwrite	        overwrite,
                       WDVCAPI_LockIdList           destinationLockIdList,
                       WDVCAPI_ClientErrorHandling  clientErrorHandling,
                       void                        *clientData,
                       WDV_Bool                    *destinationCreated );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_Move( WDVCAPI_WDV			        wdv,
                       WDVCAPI_URI			        sourceUri,
					   WDVCAPI_URI			        destinationUri,
					   WDVCAPI_Overwrite	        overwrite,
                       WDVCAPI_LockIdList           destinationLockIdList,
                       WDVCAPI_ClientErrorHandling  clientErrorHandling,
                       void                        *clientData,
                       WDV_Bool                    *destinationCreated );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_ProppatchCreate( WDVCAPI_WDV			    wdv,
                                  WDVCAPI_URI               uri,
                                  WDVCAPI_LockIdString      lockIdString,
                                  WDVCAPI_ProppatchHandle  *hProppatch );

WDV_Bool WDVCAPI_ProppatchDestroy( WDVCAPI_WDV              wdv,
                                   WDVCAPI_ProppatchHandle  hProppatch );

WDV_Bool WDVCAPI_ProppatchRemove( WDVCAPI_WDV               wdv,
                                  WDVCAPI_ProppatchHandle   hProppatch,
                                  WDVCAPI_NameSpace         nameSpace,
                                  WDVCAPI_PropertyName      propertyName,
                                  WDVCAPI_ErrorItem         errorItem );

WDV_Bool WDVCAPI_ProppatchSetShortValue( WDVCAPI_WDV                wdv,
                                         WDVCAPI_ProppatchHandle    hProppatch,
                                         WDVCAPI_NameSpace          nameSpace,
                                         WDVCAPI_PropertyName       propertyName,
                                         WDVCAPI_PropertyShortValue propertyShortValue,
                                         WDVCAPI_ErrorItem          errorItem );

WDV_Bool WDVCAPI_ProppatchSetDocClass( WDVCAPI_WDV                  wdv,
                                       WDVCAPI_URI                  uri,
                                       WDVCAPI_PropertyShortValue   docClassId );

WDV_Bool WDVCAPI_ProppatchGetDocClassProperties( WDVCAPI_WDV                wdv,
                                                 WDVCAPI_URI                uri,
                                                 WDVCAPI_PropertyShortValue docClassId,
                                                 WDVCAPI_PropertyShortValue indexingState );

WDV_Bool WDVCAPI_ProppatchGetFirstProcessedNameSpace( WDVCAPI_WDV                wdv,
                                                      WDVCAPI_ProppatchHandle    hProppatch,
                                                      WDVCAPI_NameSpaceShortcut *nameSpaceShortcut,
                                                      WDVCAPI_NameSpace         *nameSpace );

WDV_Bool WDVCAPI_ProppatchGetNextProcessedNameSpace( WDVCAPI_WDV                wdv,
                                                     WDVCAPI_ProppatchHandle    hProppatch,
                                                     WDVCAPI_NameSpaceShortcut *nameSpaceShortcut,
                                                     WDVCAPI_NameSpace         *nameSpace );

WDV_Bool WDVCAPI_ProppatchGetFirstProcessedProperty( WDVCAPI_WDV                     wdv,
                                                     WDVCAPI_ProppatchHandle         hProppatch,
                                                     WDVCAPI_NameSpaceShortcut      *nameSpaceShortcut,
                                                     WDVCAPI_PropertyName           *propertyName,
                                                     WDVCAPI_ProppatchAccessType    *proppatchAccessType,
                                                     WDVCAPI_PropertyStatus         *propertyStatus,
                                                     WDVCAPI_ErrorItem              *errorItem );

WDV_Bool WDVCAPI_ProppatchGetNextProcessedProperty( WDVCAPI_WDV                     wdv,
                                                    WDVCAPI_ProppatchHandle         hProppatch,
                                                    WDVCAPI_NameSpaceShortcut      *nameSpaceShortcut,
                                                    WDVCAPI_PropertyName           *propertyName,
                                                    WDVCAPI_ProppatchAccessType    *proppatchAccessType,
                                                    WDVCAPI_PropertyStatus         *propertyStatus,
                                                    WDVCAPI_ErrorItem              *errorItem );

/*---------------------------------------------------------------------------*/

WDV_Bool WDVCAPI_Lock( WDVCAPI_WDV			        wdv,
                       WDVCAPI_URI			        lockUri,
					   WDVCAPI_Depth				lockDepth,
					   WDVCAPI_LockTimeout          lockTimeout,
					   WDVCAPI_LockScope			lockScope,
					   WDVCAPI_LockType				lockType,
					   WDVCAPI_LockOwner			lockOwner,
					   WDVCAPI_LockIdString         lockIdString,
                       WDVCAPI_ClientErrorHandling  clientErrorHandling,
                       void                        *clientData );

WDV_Bool WDVCAPI_Unlock( WDVCAPI_WDV			wdv,
                         WDVCAPI_URI			lockUri,
                         WDVCAPI_LockIdString   lockIdString );

WDV_Bool WDVCAPI_LockCreateIdList( WDVCAPI_WDV         wdv,
                                   WDVCAPI_LockIdList *lockIdList );

WDV_Bool WDVCAPI_LockDropIdList( WDVCAPI_WDV         wdv,
                                 WDVCAPI_LockIdList *lockIdList );

WDV_Bool WDVCAPI_LockAddIdToList( WDVCAPI_WDV           wdv,
                                  WDVCAPI_LockIdList   *lockIdList,
                                  WDVCAPI_URI           lockUri,
                                  WDVCAPI_LockIdString  lockIdString );

/*---------------------------------------------------------------------------*/

/*
WDV_Bool WDVCAPI_XQueryOpen( WDVCAPI_WDV			wdv,
							 WDVCAPI_URI			uri,
							 const char				*querystring,
							 WDVCAPI_CharEncoding	encoding);

WDV_Bool WDVCAPI_XQuery( WDVCAPI_WDV           wdv,
						 WDVCAPI_Buffer        buffer,
						 WDVCAPI_BufferLength  bufferLength,
						 WDVCAPI_BufferLength *readLength );

WDV_Bool WDVCAPI_XQueryClose( WDVCAPI_WDV  wdv );
*/
WDV_Bool WDVCAPI_XQuery( WDVCAPI_WDV           wdv,
                         WDVCAPI_URI           uri,
                         WDVCAPI_LockIdString  lockIdString,
                         unsigned char        *xdbXQuery,
                         SAPDB_Long            xdbXQueryLen,
                         WDVCAPI_OutStream     outStream,
                         void                 *outStreamUserData );

/*---------------------------------------------------------------------------*/

#if defined(__cplusplus)
extern "C" {
#endif

    typedef SAPDB_UInt4 WDVCAPI_Handle;

    void WDVCAPI_InitHandleManager();

#if defined(__cplusplus)
}
#endif

/***********************************************************************

	End

 ***********************************************************************/

#endif
