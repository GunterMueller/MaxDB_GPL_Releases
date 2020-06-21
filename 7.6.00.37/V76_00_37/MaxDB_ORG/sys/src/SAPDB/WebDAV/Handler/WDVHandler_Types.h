#ifndef WDVHANDLER_TYPES_H
#define WDVHAMDLER_TYPES_H

/****************************************************************************

  module      : WDVHandler_Types.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-01  9:37
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

	#include "SAPDB/WebDAV/CAPI/WDVCAPI_Definition.h"
	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"
	#include "SAPDB/XML/QueryServerLib/XMLQSLib_Definition.h"
	#include "idl/expat/expat.h"
	#include <time.h>

	/*------------------------------------------------------------------------
	 |   DEFINES                                                             |
	 -------------------------------------------------------------------------*/
	/* Prefix is variable - it is adjustable via waadmin.wa Parameter URLprefix */	
	/* #define WDVH_WEBDAV_SERVER_PREFIX					"/webdav"*/
	#define WDVH_WEBDAV_SERVER_NAME						"SAP DB WWW WebDAV"

	#define WDVH_PATH_SEPARATOR							'/'
	#define WDVH_PATH_SEPARATOR_STR						"/"
	#define WDVH_NS_SEPARATOR							'&'

	/*---------------------------------------------------------------------------*/

	#define WDVH_MAX_SYSKEY_LEN                          WDV_MAX_SYSKEY_LEN

	#define WDVH_MAX_ERROR_TEXT_LEN                      WDV_MAX_ERROR_TEXT_LEN
	#define WDVH_MAX_STATE_TEXT_LEN                      WDV_MAX_STATE_TEXT_LEN

	#define WDVH_MAX_URI_LEN                             WDV_MAX_URI_LEN

	#define WDVH_MAX_RESOURCE_NAME_LEN                   WDV_MAX_RESOURCE_NAME_LEN

	#define WDVH_MAX_NAME_SPACE_LEN			             WDV_MAX_NAME_SPACE_LEN
	#define WDVH_MAX_NAME_SPACE_PREFIX_LEN			     WDV_MAX_NAME_SPACE_LEN
	#define WDVH_MAX_PROPERTY_NAME_PREFIX_LEN			 WDV_MAX_PROPERTY_NAME_PREFIX_LEN
	#define WDVH_MAX_PROPERTY_SHORT_VALUE_LEN			 WDV_MAX_PROPERTY_SHORT_VALUE_LEN
	#define WDVH_MAX_PROPERTY_LONG_VALUE_LEN			 WDV_MAX_PROPERTY_LONG_VALUE_LEN

	#define WDVH_MAX_IF_HEADER_LEN						 1000

	#define WDVH_MAX_XML_BUFFER_LEN		                 (WDVH_MAX_PROPERTY_LONG_VALUE_LEN / 2) + 1
	#define WDVH_MAX_XML_TAG_LEN						 WDVH_MAX_PROPERTY_NAME_PREFIX_LEN + WDVH_MAX_NAME_SPACE_LEN + 4
	#define WDVH_MAX_STR_BUFFER_LEN		                 20000
	#define wdvh_max_put_buffer_len						 128000
	#define wdvh_max_get_buffer_len						 128000

    #define WDVH_MAX_HTTP_DATE_LEN                       50

	/*------------------------------------------------------------------------*/

	#define WDVH_DEPTH_0									"0"
	#define WDVH_DEPTH_1									"1"
	#define WDVH_DEPTH_INFINITY								"infinity"

	/*------------------------------------------------------------------------*/

	#define WDVH_OVERWRITE_TRUE								"T"
	#define WDVH_OVERWRITE_FALSE							"F"

	/*------------------------------------------------------------------------*/

	#define WDVH_DEPTH_HEADER								"Depth"
	#define WDVH_DESTINATION_HEADER							"Destination"
	#define WDVH_OVERWRITE_HEADER							"Overwrite"

	/*------------------------------------------------------------------------*/

	/*------------------------------------------------------------------------
	 |   TYPES                                                               |
	 -------------------------------------------------------------------------*/
	/* Basic types */
	typedef unsigned int		WDVH_UInt4;
	typedef unsigned short		WDVH_UInt2;
	typedef unsigned char		WDVH_UInt1;
	typedef short				WDVH_Int2;
	typedef int			        WDVH_Int4;
	typedef unsigned char		WDVH_Bool;
	typedef size_t				WDVH_Length;
	typedef char		        WDVH_Char;
	typedef char		       *WDVH_PChar;
	typedef double				WDVH_Double;

	/*--------------------------------------------------------------------------
	declaration: sapdbwa_{ULong | Long}
	description: basic long data types for fastest execution
				 (machine-dependent size)
	--------------------------------------------------------------------------*/
	#if defined(WIN32) && defined(_M_IA64)
	typedef unsigned __int64    WDVH_ULong;      /* long (fast, unsigned) */
	typedef signed __int64      WDVH_Long;       /* long (fast, signed)   */
	#else
	typedef unsigned long       WDVH_ULong;      /* long (fast, unsigned) */
	typedef signed long         WDVH_Long;       /* long (fast, signed)   */
	#endif

	#define WDVH_True			((WDVH_Bool)1)
	#define WDVH_False			((WDVH_Bool)0)

	/* WDVHandler */
	typedef char				*WDVH_URI;
	typedef char                WDVH_URIString[WDVH_MAX_URI_LEN+1];
	typedef char                WDVH_ResourceName[WDVH_MAX_RESOURCE_NAME_LEN+1];
	typedef char				WDVH_XmlBuffer[WDVH_MAX_XML_BUFFER_LEN+1];
	typedef char*				WDVH_PutBuffer;
	typedef char*				WDVH_GetBuffer;
	typedef char				*WDVH_XmlBufferRef;
	typedef const char			*WDVH_XmlEmptyTag;
	/* Parsing */
	typedef void				*WDVH_XmlParserState;
	typedef struct strBuffer	*WDVH_DynamicString;

    /* Time/Date */
    typedef char                *WDVH_HTTPDate;
    typedef char                 WDVH_HTTPDateString[WDVH_MAX_HTTP_DATE_LEN+1];

	/*------------------------------------------------------------------------
		ERROR         
	--------------------------------------------------------------------------*/
	typedef WDVH_UInt2			WDVH_Error;
	
	#define WDVH_WEBDAV_SERVER_PREFIX_DEFAULT				"/webdav"
	#define WDVH_WEBDAV_SERVER_PREFIX_PARAM					"URLprefix"

	/*------------------------------------------------------------------------
		ADMINISTRATIV INFORMATION         
	--------------------------------------------------------------------------*/
	#define WDVH_ADMIN_PING									"ping"
	#define WDVH_ADMIN_STATE								"state"
	#define WDVH_ADMIN_RESET								"reset"
	#define WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE				1000

	#define WDVH_ADMIN_METHODS								18
	#define WDVH_ADMIN_METHOD_COUNT							0
	#define WDVH_ADMIN_METHOD_GET							1
	#define WDVH_ADMIN_METHOD_HEAD							2
	#define WDVH_ADMIN_METHOD_PUT							3
	#define WDVH_ADMIN_METHOD_POST							4
	#define WDVH_ADMIN_METHOD_DELETE						5
	#define WDVH_ADMIN_METHOD_COPY							6
	#define WDVH_ADMIN_METHOD_MOVE							7
	#define WDVH_ADMIN_METHOD_PROPFIND						8
	#define WDVH_ADMIN_METHOD_PROPPATCH						9
	#define WDVH_ADMIN_METHOD_MKCOL							10
	#define WDVH_ADMIN_METHOD_LOCK							11
	#define WDVH_ADMIN_METHOD_UNLOCK						12
	#define WDVH_ADMIN_METHOD_OPTIONS						13
	#define WDVH_ADMIN_METHOD_TRACE							14
	#define WDVH_ADMIN_METHOD_CONNECT						15
	#define WDVH_ADMIN_METHOD_PING							16
	#define WDVH_ADMIN_METHOD_STATE							17
	#define WDVH_ADMIN_METHOD_RESET							18

	struct st_admin_method {
		WDVH_UInt4				method;
		clock_t					start;
		clock_t					end;
	};
	typedef struct st_admin_method		WDVH_AdminMethodLog;

	struct st_admin_state {
		WDVH_Char				name[100];
		WDVH_Char				location[250];
		WDVH_Char				version[20];
		WDVH_Char				build[50];
		WDVH_Char				runningSince[100];
		WDVH_UInt4				runningSinceUTC;
		WDVH_UInt4				requestCount;
		WDVH_UInt4				idleTimeUTC;
		WDVH_UInt4				lastIdleStartUTC;
		WDVH_AdminMethodLog		lastCalledMethods[WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE];
		WDVH_UInt4				currentMethodLogPos;
	};

	typedef struct st_admin_state		*WDVH_AdminState;

	
	/*------------------------------------------------------------------------
		NAMESPACE LIST
	--------------------------------------------------------------------------*/
	struct st_open_ns {
		WDVH_Char	prefix[WDVH_MAX_NAME_SPACE_LEN+1];
		WDVH_Char	uri[WDVH_MAX_NAME_SPACE_LEN+1];
	};

	typedef struct st_open_ns		WDVH_XmlNamespace;

	struct st_ns_list_entry {
		WDVH_XmlNamespace		ns;
		struct st_ns_list_entry	*next;
		struct st_ns_list_entry	*prev;
	};

	typedef struct st_ns_list_entry	*WDVH_XmlNamespaceListItem;

	struct st_ns_list {
		WDVH_XmlNamespaceListItem	first;
		WDVH_XmlNamespaceListItem	last;
		WDVH_XmlNamespaceListItem	current;
		WDVH_UInt4					length;
	};

	typedef struct st_ns_list		*WDVH_XmlNamespaceList;

	/*------------------------------------------------------------------------
		XML BUFFER LIST
	--------------------------------------------------------------------------*/
	struct st_buff_list_entry {
		WDVH_XmlBuffer				buff;
		struct st_buff_list_entry	*next;
		struct st_buff_list_entry	*prev;
	};

	typedef struct st_buff_list_entry	*WDVH_XmlBufferListItem;

	struct st_buff_list {
		WDVH_XmlBufferListItem	first;
		WDVH_XmlBufferListItem	last;
		WDVH_XmlBufferListItem	current;
		WDVH_UInt4				length;
	};

	typedef struct st_buff_list		*WDVH_XmlBufferList;

	/*------------------------------------------------------------------------
		PROPFIND PARSING
	--------------------------------------------------------------------------*/
	enum enum_propfind_DFA_states {
		WDVH_PROPFIND_DFA_STATE_1 = 1,
		WDVH_PROPFIND_DFA_STATE_2,
		WDVH_PROPFIND_DFA_STATE_3,
		WDVH_PROPFIND_DFA_STATE_4,
		WDVH_PROPFIND_DFA_STATE_5,
		WDVH_PROPFIND_DFA_STATE_6,
		WDVH_PROPFIND_DFA_STATE_7,
		WDVH_PROPFIND_DFA_STATE_8		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123337 */
	};
	typedef enum enum_propfind_DFA_states	WDVH_XmlPropfindDFAState;

	enum enum_parser_errors {
		WDVH_XML_PARSER_OK				= 0,
		WDVH_XML_PARSER_SYNTAX_ERROR,
		WDVH_XML_PARSER_PROPPATCH_ERROR,
		WDVH_XML_PARSER_MEMORY_ERROR	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
	};
	typedef enum enum_parser_errors			WDVH_XmlParserError;

	struct st_propfind_parser_state {
		WDVH_UInt2					depth;
		WDVH_XmlPropfindDFAState	state;
		WDVH_XmlParserError			error;
	};
	typedef struct st_propfind_parser_state	*WDVH_XmlPropfindParserState;

	/*------------------------------------------------------------------------
		PROPPATCH PARSING
	--------------------------------------------------------------------------*/
	enum enum_proppatch_DFA_states {
		WDVH_PROPPATCH_DFA_STATE_1 = 1,     /* START state */
		WDVH_PROPPATCH_DFA_STATE_2,
		WDVH_PROPPATCH_DFA_STATE_3,
		WDVH_PROPPATCH_DFA_STATE_4,
		WDVH_PROPPATCH_DFA_STATE_5,
		WDVH_PROPPATCH_DFA_STATE_6,
		WDVH_PROPPATCH_DFA_STATE_7,
		WDVH_PROPPATCH_DFA_STATE_8,
		WDVH_PROPPATCH_DFA_STATE_9,
		WDVH_PROPPATCH_DFA_STATE_10,
		WDVH_PROPPATCH_DFA_STATE_11,
		WDVH_PROPPATCH_DFA_STATE_12,
		WDVH_PROPPATCH_DFA_STATE_13,
		WDVH_PROPPATCH_DFA_STATE_14,
		WDVH_PROPPATCH_DFA_STATE_15,
		WDVH_PROPPATCH_DFA_STATE_16,			/* FINAL state */
		WDVH_PROPPATCH_DFA_STATE_17,			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360 */
		WDVH_PROPPATCH_DFA_STATE_18,			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360 */
		WDVH_PROPPATCH_DFA_STATE_19,			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
		WDVH_PROPPATCH_DFA_STATE_20				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
	};
	typedef enum enum_proppatch_DFA_states	WDVH_XmlProppatchDFAState;

	struct st_proppatch_parser_state {
		WDVH_UInt2					depth;
		WDVH_XmlProppatchDFAState	state;
		WDVH_XmlParserError			error;
	};
	typedef struct st_proppatch_parser_state	*WDVH_XmlProppatchParserState;

	/*------------------------------------------------------------------------
		LOCK PARSING
	--------------------------------------------------------------------------*/
	enum enum_lock_DFA_states {
		WDVH_LOCK_DFA_STATE_1 = 1,     /* START state */
		WDVH_LOCK_DFA_STATE_2,
		WDVH_LOCK_DFA_STATE_3,
		WDVH_LOCK_DFA_STATE_4,
		WDVH_LOCK_DFA_STATE_5,
		WDVH_LOCK_DFA_STATE_6,
		WDVH_LOCK_DFA_STATE_7,
		WDVH_LOCK_DFA_STATE_8,
		WDVH_LOCK_DFA_STATE_9,
		WDVH_LOCK_DFA_STATE_10,
		WDVH_LOCK_DFA_STATE_11,			/* FINAL state */
		WDVH_LOCK_DFA_STATE_12,
		WDVH_LOCK_DFA_STATE_13,
		WDVH_LOCK_DFA_STATE_14
	};
	typedef enum enum_lock_DFA_states	WDVH_XmlLockDFAState;

	struct st_lock_parser_state {
		WDVH_UInt2					depth;
		WDVH_XmlLockDFAState		state;
		WDVH_XmlParserError			error;
		WDVH_Bool					lockscope;
		WDVH_Bool					locktype;
		WDVH_Bool					owner;
	};
	typedef struct st_lock_parser_state	*WDVH_XmlLockParserState;

	typedef WDVH_Char				WDVH_LockOwner[WDVH_MAX_PROPERTY_SHORT_VALUE_LEN+1];

	enum enum_locktypes {
		WDVH_LOCKTYPE_WRITE = 1
	};
	typedef enum enum_locktypes		WDVH_LockType;

	enum enum_lockscopes {
		WDVH_LOCKSCOPE_EXCLUSIVE = 1,
		WDVH_LOCKSCOPE_SHARED	 = 2
	};
	typedef enum enum_lockscopes		WDVH_LockScope;

	struct st_docclass_list_item {
		WDVH_Char						docClassName[513];
		WDVCAPI_XmlDocClassId			docClassId;
		struct st_docclass_list_item	*nextItem;
	};
	typedef struct st_docclass_list_item	*WDVH_DocClassListItem;

	struct st_docclass_list {
		struct st_docclass_list_item	*first;
		struct st_docclass_list_item	*last;
	};
	typedef struct st_docclass_list			*WDVH_DocClassList;

	/*------------------------------------------------------------------------
		USER DATA
	--------------------------------------------------------------------------*/
	struct st_user_data {
		XML_Parser				parser;
		WDVH_XmlBufferRef		currentBuff;
		WDVH_XmlBufferRef		previousBuff;
		WDVH_UInt4				bufferChunks;
		WDVH_UInt4				bufferStart;				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
		WDVH_UInt4				bufferEnd;					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
		WDVH_XmlNamespaceList	nslist;
		WDVCAPI_WDV				wdv;
		sapdbwa_Handle			wa;
		WDVCAPI_PropfindHandle	*propfindHandle;
		WDVH_Bool				isPropnameRequest;			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
		sapdbwa_HttpRequestP	request;
		WDVCAPI_URI				requestUri;
		WDVCAPI_Depth			depth;
		WDVCAPI_LockType		locktype;
		WDVCAPI_LockScope		lockscope;
		WDVH_Bool				propfindEmptyPropertyList; /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
		WDVCAPI_ProppatchHandle	proppatchHandle;
		WDVH_DynamicString		propertyShortValue;			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_Int4				propertyValueStartPos;		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_Int4				propertyValueEndPos;		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_Bool				propertyShortValueStarted;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVCAPI_ErrorItem		error;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_XmlParserState		parserState;
	};
	typedef struct st_user_data	*WDVH_XmlUserData;


	/*------------------------------------------------------------------------
		SESSION DATA (WebAgent Handle specific)
	--------------------------------------------------------------------------*/
	struct st_capi_session {
		WDVH_Char			*server;
		const WDVH_Char		*prefix;
		WDVH_Bool			copyReplyHeaderSent;
		WDVH_Bool			moveReplyHeaderSent;
		WDVH_Bool			lockReplyHeaderSent;
		WDVCAPI_Depth		depth;
		WDVCAPI_LockTimeout timeout;
		WDVCAPI_LockScope	lockScope;
		WDVCAPI_LockType	lockType;
		WDVCAPI_LockOwnerString	lockOwner;
		WDVCAPI_LockIdString      lockId;
		WDVH_Bool			errorsOccured;
		sapdbwa_HttpReplyP	reply;
	};

	typedef struct st_capi_session		*WDVH_CapiUserData;

	/*------------------------------------------------------------------------
		WDVHANDLER HANDLE
	--------------------------------------------------------------------------*/
	struct st_wdvh_handle {
		sapdbwa_Handle			wa;
   		sapdbwa_HttpRequestP	request;
   		sapdbwa_HttpReplyP		reply;
		WDVH_Char				requesturi[WDVH_MAX_URI_LEN+1];
		WDVCAPI_LockIdString	locktoken;
		WDVH_Char*				wdvPutBuffer;
		WDVH_Char*				wdvGetBuffer;
		WDVH_DocClassList		docClassList;
		XMLIMAPI_Handle			xmlHandle;
		sapdbwa_DbConnectionP	xmlConn;
	};
	typedef struct st_wdvh_handle	*WDVH_Handle;

	/*------------------------------------------------------------------------
		ERROR HANDLE
	--------------------------------------------------------------------------*/
	struct st_error_handle {
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
	};
	typedef struct st_error_handle	*WDVH_ErrorHandle;

	/*------------------------------------------------------------------------
		CONNECTION USER DATA 
	--------------------------------------------------------------------------*/
	struct st_conn_user_data {
		WDVCAPI_WDV		wdvHandle;
		WDVH_Handle		wdvhHandle;
		WDVH_Bool		compress;
		XMLQSLib_Handle qsHandle;
	};
	typedef struct st_conn_user_data	*WDVH_ConnUserData;

#endif
