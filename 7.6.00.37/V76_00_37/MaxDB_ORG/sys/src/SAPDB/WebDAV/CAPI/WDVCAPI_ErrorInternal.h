/*!**********************************************************************

  module: WDVCAPI_ErrorInternal.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV internal used error codes 

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

#ifndef WDVCAPI_ERROR_INTERNAL_H
#define WDVCAPI_ERROR_INTERNAL_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

/***********************************************************************

	Defines

 ***********************************************************************/

#define WDVCAPI_ERR_SQL_TEXT_INVALID_HANDLE                             "Invalid handle"
#define WDVCAPI_ERR_SQL_TEXT_NO_DATA_FOUND                              "No data"
#define WDVCAPI_ERR_SQL_TEXT_SUCCESS                                    ""

#define WDVCAPI_ERR_SQL_STATE_DATA_TRUNCATED                            "01004"

#define WDVCAPI_ERR_TEXT_UNDEFINED                                      "Undefined error"
#define WDVCAPI_ERR_TEXT_PARENT_DOESNT_EXIST                            "Parent doesn't exist"
#define WDVCAPI_ERR_TEXT_COLLECTION_ALREADY_EXIST                       "Collection already exist"
#define WDVCAPI_ERR_TEXT_INVALID_HANDLE                                 "Invalid handle"
#define WDVCAPI_ERR_TEXT_ROOT_ALREADY_EXIST                             "Root already exist"
#define WDVCAPI_ERR_TEXT_CANNOT_FIND_NEW_RESOURCE                       "Cannot find new resource"
#define WDVCAPI_ERR_TEXT_NO_MEMORY                                      "No more memory"
#define WDVCAPI_ERR_TEXT_COLLECTION_DOESNT_EXIST                        "Collection doesn't exist"
#define WDVCAPI_ERR_TEXT_RESOURCE_NAME_TOO_LONG                         "Resource name too long"
#define WDVCAPI_ERR_TEXT_INTERNAL_ERROR                                 "Internal error"
#define WDVCAPI_ERR_TEXT_UNKNOWN_ODBC_RETURN_CODE                       "Unknown ODBC return code"
#define WDVCAPI_ERR_TEXT_WRONG_DEPTH                                    "Wrong depth value"
#define WDVCAPI_ERR_TEXT_NO_MORE_SIBLING_NODES                          "No more sibling nodes"
#define WDVCAPI_ERR_TEXT_NO_MORE_CHILD_NODES                            "No more child nodes"
#define WDVCAPI_ERR_TEXT_NO_MORE_RESOURCES                              "No more resources"
#define WDVCAPI_ERR_TEXT_UNKNOWN_PROPFIND_DEPTH                         "Unknown propfind depth value"
#define WDVCAPI_ERR_TEXT_URI_IS_CORRUPTED                               "URI is corrupted"
#define WDVCAPI_ERR_TEXT_RESOURCE_DOESNT_EXIST                          "Resource doesn't exist"
#define WDVCAPI_ERR_TEXT_NO_MORE_TO_DELETE                              "No more to delete"
#define WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_DOESNT_EXIST            "Resource or collection doesn't exist"
#define WDVCAPI_ERR_TEXT_RESOURCE_OR_COLLECTION_ALREADY_EXIST           "Resource or collection already exist"
#define WDVCAPI_ERR_TEXT_DEPTH_1_IS_NOT_ALLOWED                         "Depth 1 is not allowed"
#define WDVCAPI_ERR_TEXT_EQUAL_RESOURCE_AND_DESTINATION                 "Source and destination are equal"
#define WDVCAPI_ERR_TEXT_DESTINATION_URI_DOESNT_EXIST                   "Destination path doesn't exist"
#define WDVCAPI_ERR_TEXT_OVERWRITE_CONFLICT                             "Overwrite is not allowed"
#define WDVCAPI_ERR_TEXT_COULD_NOT_DELETE_MOVED_RESOURCE_OR_COLLECTION  "Could not delete moved resource or collection"
#define WDVCAPI_ERR_TEXT_UNDEFINED_OVERWRITE                            "Undefined overwrite"
#define WDVCAPI_ERR_TEXT_COULD_NOT_SET_PROPERTY                         "Could not set property"
#define WDVCAPI_ERR_TEXT_COULD_NOT_REMOVE_PROPERTY                      "Could not remove property"
#define WDVCAPI_ERR_TEXT_UNKNOWN_PROPFIND_TYPE                          "Unknown propfind type"
#define WDVCAPI_ERR_TEXT_EMPTY_PROPERTY_LIST                            "Empty property list"
#define WDVCAPI_ERR_TEXT_UNKNOWN_NAME_SPACE                             "Unknown name space"
#define WDVCAPI_ERR_TEXT_COULD_NOT_GET_NEXT_UNIQUE_ID                   "Could not get next unique id"
#define WDVCAPI_ERR_TEXT_LOCK_EXISTS                                    "Lock exists"
#define WDVCAPI_ERR_TEXT_PROPERTY_DOESNT_EXIST                          "Property doesn't exist"
#define WDVCAPI_ERR_TEXT_COULD_NOT_SET_DEFAULT_PROPERTY                 "Could not set default property"
#define WDVCAPI_ERR_TEXT_DESTINATION_IS_CHILD_OF_SOURCE                 "Destination is child of source"
#define WDVCAPI_ERR_TEXT_LOCK_FOUND_IN_SUBTREE                          "Lock found in sub tree"
#define WDVCAPI_ERR_TEXT_BUFFER_IS_TOO_SMALL                            "Buffer is too small"
#define WDVCAPI_ERR_TEXT_NO_PUT_ON_COLLECTION                           "Put on a collection is not allowed"
#define WDVCAPI_ERR_TEXT_NO_ACCESS_TO_XML_MODELLING_MANAGEMENT          "No access to XML modelling management"
#define WDVCAPI_ERR_TEXT_DOC_CLASS_NOT_FOUND                            "Document class not found"
#define WDVCAPI_ERR_TEXT_NO_ACCESS_TO_XPATH_ENGINE                      "No access to XPath engine"
#define WDVCAPI_ERR_TEXT_ACCESS_TO_XPATH_ENGINE_FAILED                  "Access to XPath engine failed"
#define WDVCAPI_ERR_TEXT_XPATH_SYNTAX_ERROR                             "XPath syntax error"
#define WDVCAPI_ERR_TEXT_RANGE_START_OUT_OF_BOUNDS                      "Requested range begins behind end of file"
#define WDVCAPI_ERR_TEXT_INDEXING_STATE_NOT_APPROPRIATE                 "Indexing state not appropriate for an update"
#define WDVCAPI_ERR_TEXT_NO_DOC_CLASS_PROPERTY                          "No document class property found"
#define WDVCAPI_ERR_TEXT_INDEXING_FAILED                                "Indexing failed"
#define WDVCAPI_ERR_TEXT_NOT_ALLOWED_ON_DELETED_ITEMS_FOLDER            "Not allowed on folder 'Deleted Items'"
#define WDVCAPI_ERR_TEXT_COULD_NOT_SET_LIVE_PROPERTY                    "Could not set live property"
#define WDVCAPI_ERR_TEXT_PARENT_NOT_A_COLLECTION                        "Parent resource is not a collection"

#define WDVCAPI_ERR_STATE_UNDEFINED                                     "Undefined state"

/* Extended error description */
#define WDV_COMPREHENSIVE_ERROR_DESCRIPTION     WDVCAPI_COMPREHENSIVE_ERROR_DESCRIPTION

#ifdef WDV_COMPREHENSIVE_ERROR_DESCRIPTION
    #define Error_AddErrorItem(a,b,c,d)     AddErrorItem(a,b,c,d,__FILE__,__LINE__)
    #define Error_AddSQLErrorItem(a,b,c)    AddSQLErrorItem(a,b,c,__FILE__,__LINE__)
#else
    #define Error_AddErrorItem(a,b,c,d)     AddErrorItem(a,b,c,d,NULL,NULL)
    #define Error_AddSQLErrorItem(a,b,c)    AddSQLErrorItem(a,b,c,NULL,NULL)
#endif

#ifndef strcasecmp
    #ifdef WIN32
        #define strcasecmp(s1, s2)	stricmp(s1, s2)
    #endif
#endif

#ifndef strncasecmp
    #ifdef WIN32
        #define strncasecmp(s1, s2, n)	strnicmp(s1, s2, n)
    #endif
#endif

/***********************************************************************

	Types

 ***********************************************************************/

typedef struct st_wdvcapi_error_list  *WDVCAPI_ErrorList;

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool CreateErrorList( WDVCAPI_WDV wdv );

WDV_Bool DestroyErrorList( WDVCAPI_WDV  wdv );

WDV_Bool AddErrorItemToList( WDVCAPI_WDV       wdv,
				             WDVCAPI_ErrorItem errorItem );

WDV_Bool AddErrorItem( WDVCAPI_WDV              wdv,
				       WDVCAPI_ErrorType        type,
				       WDVCAPI_ErrorCode        code,
				       WDVCAPI_ErrorText        text,
                       char                    *file,
                       const int                line );

WDV_Bool AddSQLErrorItem( WDVCAPI_WDV       wdv,
				          SQLHSTMT          hStmt,
						  SQLRETURN         sqlRC,
                          char             *file,
                          const int         line );

/***********************************************************************

	End

 ***********************************************************************/

#endif
