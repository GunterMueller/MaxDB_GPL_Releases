#ifndef XMLIMAPI_ERROR_H
#define XMLIMAPI_ERROR_H
/****************************************************************************

  module      : XMLIMAPI_Error.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-03  12:48
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/

	#include <XMLIMAPI_Definition.h>

	#define XMLIMAPI_ERR_SQL_TEXT_INVALID_HANDLE                             "Invalid handle"
	#define XMLIMAPI_ERR_SQL_TEXT_NO_DATA_FOUND                              "No data"
	#define XMLIMAPI_ERR_SQL_TEXT_SUCCESS                                    ""

	#define XMLIMAPI_ERR_SQL_CODE_DUPLICATE_KEY				              200

	#define XMLIMAPI_ERR_SQL_STATE_DATA_TRUNCATED                            "01004"
	#define XMLIMAPI_ERR_SQL_STATE_MORE_THAN_ONE_ROW_UPDATE                  "01S04"
	#define XMLIMAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED                "01S03"

	#define XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX_ASSIGNED				"No XML Index assigned"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_INDICES					"No more XML Indices found"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_XMLINDEX					    "No XML Index found"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_DOCCLASS				        "No Document Class found"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_DOCCLASSES				"No more Document Classes found"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_SESSIONPOOL					"No Session Pool found"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_SESSIONPOOLS				"No more Session Pools found"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_XIE							"No XML Index Engine"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_XIES						"No more XML Index Engines found"
	#define XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_USED					"Session Pool is still used. Deletion forbidden."
	#define XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_INTERNAL				"Internal error occured. Session Pool not deleted."
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_SERVICE				        "No Service Description found"
	#define XMLIMAPI_ERR_MAPI_TEXT_NO_MORE_SERVICES					"No more Service Descriptions found"
	#define XMLIMAPI_ERR_MAPI_TEXT_XMLINDEX_DOES_NOT_EXIST			"XML Index does not exist"
	#define XMLIMAPI_ERR_MAPI_TEXT_XMLINDEX_ALLREADY_EXISTS			"XML Index allready exists"
	#define XMLIMAPI_ERR_MAPI_TEXT_DOCCLASS_DOES_NOT_EXIST			"Document Class does not exist"
	#define XMLIMAPI_ERR_MAPI_TEXT_DOCCLASS_ALLREADY_EXISTS			"Document Class allready exists"
	#define XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_DOES_NOT_EXIST		"Session Pool does not exist"
	#define XMLIMAPI_ERR_MAPI_TEXT_SESSIONPOOL_ALLREADY_EXISTS		"Session Pool allready exists"
	#define XMLIMAPI_ERR_MAPI_TEXT_INDEXINGSERVICE_DOES_NOT_EXIST	"Indexing Service does not exist"
	#define XMLIMAPI_ERR_MAPI_TEXT_INDEXINGSERVICE_ALLREADY_EXISTS	"Indexing Service allready exists"
	#define XMLIMAPI_ERR_MAPI_TEXT_XIE_DOES_NOT_EXIST				"XML Index Engine does not exist"
	#define XMLIMAPI_ERR_MAPI_TEXT_XIE_ALLREADY_EXISTS				"XML Index Engine allready exists"
	#define XMLIMAPI_ERR_TEXT_COULD_NOT_GET_NEXT_UNIQUE_ID			"Could get no unique ID"
	#define XMLIMAPI_ERR_MAPI_TEXT_XMLINDEX_ASSIGNED_TO_DOCCLASS	"%s"

	#define XMLIMAPI_ERR_TEXT_UNDEFINED                                      "Undefined error"
	#define XMLIMAPI_ERR_TEXT_UNKNOWN_ODBC_RETURN_CODE                       "Unknown ODBC return code"
	
	#define XMLIMAPI_ERR_STATE_UNDEFINED                                     "Undefined state"

	/***********************************************************************

		Types

	 ***********************************************************************/

/*	typedef struct st_xmlimapi_error_list  *XMLIMAPI_ErrorList;
*/
	/***********************************************************************

		Prototypes

	 ***********************************************************************/

	XMLIMAPI_Bool createErrorList( XMLIMAPI_Handle handle );

	XMLIMAPI_Bool destroyErrorList( XMLIMAPI_Handle  handle);

	XMLIMAPI_Bool addErrorItem( XMLIMAPI_Handle           handle,
						   XMLIMAPI_ErrorType        type,
						   XMLIMAPI_ErrorCode        code,
						   XMLIMAPI_ErrorTextString  text );

	XMLIMAPI_Bool addSQLErrorItem( XMLIMAPI_Handle handle,
							  SQLHSTMT     hStmt,
							  SQLRETURN    sqlRC );



#endif