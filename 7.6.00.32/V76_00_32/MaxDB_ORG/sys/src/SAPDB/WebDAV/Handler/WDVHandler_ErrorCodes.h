#ifndef WDVHandler_ERRORCODES_H
#define WDVHandler_ERRORCODES_H

/****************************************************************************

  module      : WDVHandler_ErrorCodes.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-26  13:31
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

	
	/* Client Errors 2xx */
	#define ERROR_200			200
	#define ERROR_200_STR		"200"
	#define ERROR_200_TEXT		" OK"

	#define ERROR_201			201
	#define ERROR_201_STR		"201"
	#define ERROR_201_TEXT		" Created"

	#define ERROR_204			204
	#define ERROR_204_STR		"204"
	#define ERROR_204_TEXT		" No Content"

	/* Begin Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395*/
	#define ERROR_206			206
	#define ERROR_206_STR		"206"
	#define ERROR_206_TEXT		" Partial Content"
	/* End Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */

	#define ERROR_207			207
	#define ERROR_207_STR		"207"
	#define ERROR_207_TEXT		" Multi-Status"

	/* Client Errors 4xx */
	#define ERROR_400			400
	#define ERROR_400_STR		"400"
	#define ERROR_400_TEXT		" Bad Request"

	#define ERROR_403			403
	#define ERROR_403_STR		"403"
	#define ERROR_403_TEXT		" Forbidden"

	#define ERROR_404			404
	#define ERROR_404_STR		"404"
	#define ERROR_404_TEXT		" Not Found"

	#define ERROR_405			405
	#define ERROR_405_STR		"405"
	#define ERROR_405_TEXT		" Method Not Allowed"

	#define ERROR_409			409
	#define ERROR_409_OWNER		4091
	#define ERROR_409_STR		"409"
	#define ERROR_409_TEXT		" Conflict"

	#define ERROR_412			412
	#define ERROR_412_STR		"412"
	#define ERROR_412_TEXT		" Precondition Failed"

	#define ERROR_415			415
	#define ERROR_415_STR		"415"
	#define ERROR_415_TEXT		" Unsupported Media Type"

	/* Begin Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
	#define ERROR_416			416
	#define ERROR_416_STR		"416"
	#define ERROR_416_TEXT		" Requested range not satisfiable"
	/* End Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */

	#define ERROR_423			423
	#define ERROR_423_STR		"423"
	#define ERROR_423_TEXT		" Locked"

	#define ERROR_424			424
	#define ERROR_424_STR		"424"
	#define ERROR_424_TEXT		" Failed Dependency"

	/* Server Errors 5xx */
	#define ERROR_500			500
	#define ERROR_500_STR		"500"
	#define ERROR_500_TEXT		" Internal Server Error"

	#define ERROR_500_CON		599

	#define ERROR_501			501
	#define ERROR_501_STR		"501"
	#define ERROR_501_TEXT		" Not Implemented"

	#define ERROR_502			502
	#define ERROR_502_STR		"502"
	#define ERROR_502_TEXT		" Bad Gateway"

	#define ERROR_507			507
	#define ERROR_507_STR		"507"
	#define ERROR_507_TEXT		" Insufficient Storage"

	/*------------------------------------------------------------------------
	 |   ERROR MESSAGES                                                      |
	 -------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------
		Error Messages PROPFIND
	--------------------------------------------------------------------------*/
	#define ERROR_PROPFIND_DB_CONN		"PROPFIND: Could get no connection to database\n"
	#define ERROR_PROPFIND_PARSE_1		"PROPFIND: Error parsing request for %s%s\n"
	#define ERROR_PROPFIND_PARSE_2		"PROPFIND: XML parse error (line %d): %s\n"
	#define ERROR_PROPFIND_URI			"PROPFIND: Error on request for %s%s\n"
	#define ERROR_PROPFIND_500			"PROPFIND: Internal server error: %s\n"
	#define ERROR_PROPFIND_MISSING_BODY	"PROPFIND: Request body missing\n"

	/*------------------------------------------------------------------------
		Error Messages PROPPATCH
	--------------------------------------------------------------------------*/
	#define ERROR_PROPPATCH_DB_CONN			"PROPPATCH: Could get no connection to database\n"
	#define ERROR_PROPPATCH_PARSE_1			"PROPPATCH: Error parsing request for %s%s\n"
	#define ERROR_PROPPATCH_PARSE_2			"PROPPATCH: XML parse error (line %d): %s\n"
	#define ERROR_PROPPATCH_URI				"PROPPATCH: Error on request for %s%s\n"
	#define ERROR_PROPPATCH_500				"PROPPATCH: Internal server error: %s\n"
	#define ERROR_PROPPATCH_MISSING_BODY	"PROPPATCH: Request body missing\n"
	#define ERROR_PROPPATCH_NOTFOUND_404	"PROPPATCH: Error 404 (Not Found) on request for %s%s\n"
	#define ERROR_PROPPATCH_LOCKED_423		"PROPPATCH: Error 423 (Locked) on request for %s%s\n"
	#define ERROR_PROPPATCH_MESSAGE			"PROPPATCH: Internal error message: %s\n"

	/*------------------------------------------------------------------------
		Error Messages GET
	--------------------------------------------------------------------------*/
	#define ERROR_GET_DB_CONN		"GET: Could get no connection to database\n"
	#define ERROR_GET_URI			"GET: Error on request for %s%s\n"
	#define ERROR_GET_500			"GET: Internal server error: %s\n"

	/*------------------------------------------------------------------------
		Error Messages HEAD
	--------------------------------------------------------------------------*/
	#define ERROR_HEAD_DB_CONN		"HEAD: Could get no connection to database\n"
	#define ERROR_HEAD_URI			"HEAD: Error on request for %s%s\n"
	#define ERROR_HEAD_500			"HEAD: Internal server error: %s\n"

	/*------------------------------------------------------------------------
		Error Messages PUT/POST
	--------------------------------------------------------------------------*/
	#define ERROR_PUT_DB_CONN			"PUT/POST: Could get no connection to database\n"
	#define ERROR_PUT_URI				"PUT/POST: Error on request for %s%s\n"
	#define ERROR_PUT_500				"PUT/POST: Internal server error: %s\n"
	#define ERROR_PUT_COLLECTION_405	"PUT/POST: Error 405 (Method not allowed) on request for %s%s\n"
	#define ERROR_PUT_PARENT_409		"PUT/POST: Error 409 (Conflict) on request for %s%s\n"
	#define ERROR_PUT_LOCKED_423		"PUT/POST: Error 423 (Locked) on request for %s%s\n"
	#define ERROR_PUT_PRECONDITION_412	"PUT/POST: Error 412 (Precondition Failed) on request for %s%s\n"
	#define ERROR_PUT_DOCCLASS_NOT_EXIST	"PUT/POST: Error 412 (Precondition Failed) on request for %s%s. Document Class %s does not exist.\n"
	#define ERROR_PUT_MESSAGE			"PUT/POST: Internal error message: %s\n"

	/*------------------------------------------------------------------------
		Error Messages MKCOL
	--------------------------------------------------------------------------*/
	#define ERROR_MKCOL_DB_CONN		"MKCOL: Could get no connection to database\n"
	#define ERROR_MKCOL_URI			"MKCOL: Error on request for %s%s\n"
	#define ERROR_MKCOL_500			"MKCOL: Internal server error: %s\n"
	#define ERROR_MKCOL_MESSAGE		"MKCOL: Internal error message: %s\n"
	#define ERROR_MKCOL_EXISTS_405	"MKCOL: Error 405 (Method Not Allowed) on request for %s%s\n"
	#define ERROR_MKCOL_PARENT_409	"MKCOL: Error 409 (Conflict) on request for %s%s\n"
	#define ERROR_MKCOL_LOCKED_423	"MKCOL: Error 423 (Locked) on request for %s%s\n"

	/*------------------------------------------------------------------------
		Error Messages DELETE
	--------------------------------------------------------------------------*/
	#define ERROR_DELETE_DB_CONN		"DELETE: Could get no connection to database\n"
	#define ERROR_DELETE_URI			"DELETE: Error on request for %s%s\n"
	#define ERROR_DELETE_500			"DELETE: Internal server error: %s\n"
	#define ERROR_DELETE_MESSAGE		"DELETE: Internal error message: %s\n"
	#define ERROR_DELETE_NOTFOUND_404	"DELETE: Error 404 (Not Found) on request for %s%s\n"
	#define ERROR_DELETE_LOCKED_423		"DELETE: Error 423 (Locked) on request for %s%s\n"

	/*------------------------------------------------------------------------
		Error Messages COPY
	--------------------------------------------------------------------------*/
	#define ERROR_COPY_DB_CONN			"COPY: Could get no connection to database\n"
	#define ERROR_COPY_URI				"COPY: Error on request for copy %s%s to %s\n"
	#define ERROR_COPY_500				"COPY: Internal server error: %s\n"
	#define ERROR_COPY_MESSAGE			"COPY: Internal error message: %s\n"
	#define ERROR_COPY_FORBIDDEN_403	"COPY: Error 403 (Forbidden) on request for copy %s%s to %s\n"
	#define ERROR_COPY_NOTFOUND_404		"COPY: Error 404 (Not Found) on request for copy %s%s to %s\n"		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1119947 */
	#define ERROR_COPY_PARENT_409		"COPY: Error 409 (Conflict) on request for copy %s%s to %s\n"
	#define ERROR_COPY_PRECONDITION_412	"COPY: Error 412 (Precondition Failed) on request for copy %s%s to %s\n"
	#define ERROR_COPY_LOCKED_423		"COPY: Error 423 (Locked) on request for copy %s%s to %s\n"
	#define ERROR_COPY_BAD_GATEWAY_502	"COPY: Error 502 (Bad Gateway) on request for copy %s%s to %s\n"
	#define ERROR_COPY_STORAGE_507		"COPY: Error 507 (Insufficient Storage) on request for copy %s%s to %s\n"

	/*------------------------------------------------------------------------
		Error Messages MOVE
	--------------------------------------------------------------------------*/
	#define ERROR_MOVE_DB_CONN			"MOVE: Could get no connection to database\n"
	#define ERROR_MOVE_URI				"MOVE: Error on request for move %s%s to %s\n"
	#define ERROR_MOVE_500				"MOVE: Internal server error: %s\n"
	#define ERROR_MOVE_MESSAGE			"MOVE: Internal error message: %s\n"
	#define ERROR_MOVE_FORBIDDEN_403	"MOVE: Error 403 (Forbidden) on request for move %s%s to %s\n"
	#define ERROR_MOVE_NOTFOUND_404		"MOVE: Error 404 (Not Found) on request for move %s%s to %s\n"		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1119947 */
	#define ERROR_MOVE_PARENT_409		"MOVE: Error 409 (Conflict) on request for move %s%s to %s\n"
	#define ERROR_MOVE_CHILD_409		"MOVE: Error 409 (Conflict) on request for move %s%s to %s\n"
	#define ERROR_MOVE_PRECONDITION_412	"MOVE: Error 412 (Precondition Failed) on request for move %s%s to %s\n"
	#define ERROR_MOVE_LOCKED_423		"MOVE: Error 423 (Locked) on request for move %s%s to %s\n"
	#define ERROR_MOVE_BAD_GATEWAY_502	"MOVE: Error 502 (Bad Gateway) on request for move %s%s to %s\n"
	#define ERROR_MOVE_STORAGE_507		"MOVE: Error 507 (Insufficient Storage) on request for move %s%s to %s\n"
	
	/*------------------------------------------------------------------------
		Error Messages LOCK
	--------------------------------------------------------------------------*/
	#define ERROR_LOCK_DB_CONN		"LOCK: Could get no connection to database\n"
	#define ERROR_LOCK_PARSE_1		"LOCK: Error parsing request for %s%s\n"
	#define ERROR_LOCK_PARSE_2		"LOCK: XML parse error (line %d): %s\n"
	#define ERROR_LOCK_URI			"LOCK: Error on request for %s%s\n"
	#define ERROR_LOCK_500			"LOCK: Internal server error: %s\n"
	#define ERROR_LOCK_MESSAGE		"LOCK: Internal error message: %s\n"
	#define ERROR_LOCK_PARENT_405	"LOCK: Error 405 (Method Not Allowed) on request for %s%s\n"
	#define ERROR_LOCK_EXISTS_409	"LOCK: Error 409 (Conflict) on request for %s%s\n"
	#define ERROR_LOCK_MISSING_BODY	"LOCK: Request body missing\n"

	/*------------------------------------------------------------------------
		Error Messages UNLOCK
	--------------------------------------------------------------------------*/
	#define ERROR_UNLOCK_DB_CONN	"UNLOCK: Could get no connection to database\n"
	#define ERROR_UNLOCK_URI		"UNLOCK: Error on request for %s%s\n"
	#define ERROR_UNLOCK_500		"UNLOCK: Internal server error: %s\n"
	#define ERROR_UNLOCK_MESSAGE	"UNLOCK: Internal error message: %s\n"
	#define ERROR_UNLOCK_PARENT_405	"UNLOCK: Error 405 (Method Not Allowed) on request for %s%s\n"
	#define ERROR_UNLOCK_EXISTS_409	"UNLOCK: Error 409 (Conflict) on request for %s%s\n"

	/*------------------------------------------------------------------------
		Error Messages OPTIONS
	--------------------------------------------------------------------------*/
	#define ERROR_OPTIONS_DB_CONN	"OPTIONS: Could get no connection to database\n"

	/*------------------------------------------------------------------------
		Error Messages PROPPATCH
	--------------------------------------------------------------------------*/
	enum en_wdvh_error_codes {
		WDVH_ERR_CODE_WRONG_CONTENTYPE_SYNTAX = WDVCAPI_ERR_CODE_WDVHANDLER,
		WDVH_ERR_CODE_SET_LIVEPROPERTY_FORBIDDEN,
		WDVH_ERR_CODE_REMOVE_LIVEPROPERTY_FORBIDDEN,
		WDVH_ERR_CODE_PROPERTY_TOO_LONG,
		WDVH_ERR_CODE_PROPERTY_NAME_TOO_LONG,
		WDVH_ERR_CODE_NAMESPACE_NAME_TOO_LONG,
		WDVH_ERR_CODE_PROPERTY_NS_NAME_TOO_LONG
	};

	/*------------------------------------------------------------------------
		Error Messages XQUERY
	--------------------------------------------------------------------------*/
	#define ERROR_XQUERY_DB_CONN	"XQUERY: Could get no connection to database\n"
	#define ERROR_XQUERY_URI		"XQUERY: Error on request for %s%s\n"
	#define ERROR_XQUERY_500		"XQUERY: Internal server error: %s\n"

	#define WDVH_ERR_TEXT_WRONG_CONTENTYPE_SYNTAX		"Content Type does not match the syntax defined in RFC2518 section 13.5."
	#define WDVH_ERR_TEXT_SET_LIVEPROPERTY_FORBIDDEN	"The property to change is a live property. Updating live properties is forbidden."
	#define WDVH_ERR_TEXT_REMOVE_LIVEPROPERTY_FORBIDDEN	"The property to remove is a live property. Removing live properties is forbidden."
	#define WDVH_ERR_TEXT_PROPERTY_TOO_LONG				"The property value exceeds the supported length of %d characters. Please shorten the value and try again."
	#define WDVH_ERR_TEXT_PROPERTY_NAME_TOO_LONG		"The property name exceeds the supported length of %d characters. Please shorten the name and try again."
	#define WDVH_ERR_TEXT_NAMESPACE_NAME_TOO_LONG		"The namespace name exceeds the supported length of %d characters. Please shorten the name and try again."
	#define WDVH_ERR_TEXT_PROPERTY_NS_NAME_TOO_LONG		"The property name exceeds the supported length of %d characters. The namespace name exceeds the supported length of %d characters. Please shorten the names and try again."

	#define WDVH_ERR_TEXT_UNLOCK_MISSING_LOCKTOKEN		"Lock Token missing"
	#define WDVH_ERR_TEXT_UNLOCK_MISSING_RIGHT_BRACKET	"Malformed lock token. Missing opening '<'-bracket. Lock token MUST fit this pattern '<saplocktoken:[GUID]>'. With [GUID] beeing a hex number of 48 figures."
	#define WDVH_ERR_TEXT_UNLOCK_MISSING_LEFT_BRACKET	"Malformed lock token. Missing closing '>'-bracket. Lock token MUST fit this pattern '<saplocktoken:[GUID]>'. With [GUID] beeing a hex number of 48 figures."
	#define WDVH_ERR_TEXT_UNLOCK_MISSING_BRACKETS		"Malformed lock token. Missing opening '<'-bracket and closing '>'-bracket. Lock token MUST fit this pattern '<saplocktoken:[GUID]>'. With [GUID] beeing a hex number of 48 figures."
	#define WDVH_ERR_TEXT_UNLOCK_MISSING_SAPLOCKTOKEN	"Malformed lock token. Missing 'saplocktoken:' part. Lock token MUST fit this pattern '&lt;saplocktoken:[GUID]&gt;'. With [GUID] beeing a hex number of 48 figures."
	#define WDVH_ERR_TEXT_UNLOCK_WRONG_GUID				"Malformed lock token. The GUID does not fit the pattern. Lock token MUST fit this pattern '&lt;saplocktoken:[GUID]&gt;'. With [GUID] beeing a hex number of 48 figures."

#endif	