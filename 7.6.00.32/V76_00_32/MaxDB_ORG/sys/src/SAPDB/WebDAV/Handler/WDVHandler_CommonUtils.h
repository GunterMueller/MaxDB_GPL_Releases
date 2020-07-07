#ifndef WDVHANDLER_COMMONUTILS_H
#define WDVHANDLER_COMMONUTILS_H
/****************************************************************************

  module      : WDVHandler_CommonUtils.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-28  16:57
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Connection.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_StringBuffer.h"

	/****************************************************************************
	/
	/ Registry Sections
	/
	*****************************************************************************/

	#ifdef REG_WIN32

		#define WDVH_WEBDAV_REGISTRY			"SOFTWARE\\SAP\\SAP DBTech"

	#else

		#define WDVH_WEBDAV_REGISTRY			"/usr/spool/sql/ini/xie.ini"

	#endif

	/*------------------------------------------------------------------------
	Function	:	propertyToHTTPDate
	Description	:	Converts a timestamp property (getlastmodified)
                    into an HTTP timestamp format (RFC 822, updated by RFC 1123)
	Parameters	:	[in]    property
					[out]   httpDate
	Return		:	
	--------------------------------------------------------------------------*/
	void propertyToHTTPDate( WDVCAPI_PropfindPropertyName    property,
		                     WDVH_HTTPDate                   httpDate );

	/*------------------------------------------------------------------------
	Function	:	getFirstHeaderLine
	Description	:	gets the values of host, port and requestURI from the first
					line of the HTTP header
	Parameters	:	[in/out] host
					[in/out] port
					[in/out] uri
	Return		:	
	--------------------------------------------------------------------------*/
	void getFirstHeaderLine(sapdbwa_Handle		wa,
						    sapdbwa_HttpRequestP request,
		                    WDVH_PChar *host,
							WDVH_PChar *port, 
							WDVH_PChar uri);

	void getWebDAVPrefix(sapdbwa_Handle		wa,
						 WDVH_Char *prefix);

	/*------------------------------------------------------------------------
		Function:	getRequestUri
	--------------------------------------------------------------------------*/
	void getRequestUri(sapdbwa_Handle		wa,
					   sapdbwa_HttpRequestP request,
					   WDVH_PChar uri);
	void getRequestUriFromString(sapdbwa_Handle			wa,
								 sapdbwa_HttpRequestP	request,
								 WDVH_PChar				uri); 

	/*------------------------------------------------------------------------
	Function	:	buildServerString
	Description	:	returns the server as string (host + port)
	Parameters	:	[in/out] server
	Return		:	
	--------------------------------------------------------------------------*/
	void buildServerString(sapdbwa_HttpRequestP request,
		                   WDVH_PChar *server);

	/*------------------------------------------------------------------------
		Function:	buildServerStringLong
	--------------------------------------------------------------------------*/
	void buildServerStringLong(sapdbwa_Handle		wa,
							   sapdbwa_HttpRequestP request,
							   WDVH_PChar *server);
	
	/*------------------------------------------------------------------------
		Function:	getIfHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getIfHeader(sapdbwa_HttpRequestP request,
		                  WDVH_Char *sIf,
						  WDVH_Int4 len);

	/*------------------------------------------------------------------------
		Function:	getIfHeaderList
	--------------------------------------------------------------------------*/
	WDVH_Bool getIfHeaderList(sapdbwa_Handle		wa,
							  sapdbwa_HttpRequestP request,
							  WDVCAPI_WDV wdv,
							  WDVCAPI_LockIdList *lockTokenList);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1120166 */

	/* Begin Correction: http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
	/*------------------------------------------------------------------------
		Function:	getUserAgentHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getUserAgentHeader(sapdbwa_HttpRequestP request,
		                     WDVH_Char *sUA,
				     WDVH_Int4 len);
	/*------------------------------------------------------------------------
		Function:	getContentRangeHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getContentRangeHeader(sapdbwa_HttpRequestP request,
									WDVH_Char **from, WDVH_Char **to);


	/*------------------------------------------------------------------------
	    Function: isNumber
	--------------------------------------------------------------------------*/
	WDVH_Bool isNumber(WDVH_Char *str);
	/* End Correction: http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */

	/*------------------------------------------------------------------------
		Function:	getLockTokenHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getLockTokenHeader(sapdbwa_HttpRequestP request,
								 WDVH_Char *sLockToken,
								 WDVH_Char *errormsg);			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */

	/*------------------------------------------------------------------------
		Function:	getDcHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getDcHeader(sapdbwa_HttpRequestP request,
		                  WDVH_Char *sDc);

	/*------------------------------------------------------------------------
		Function:	getAsyncHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getAsyncHeader(sapdbwa_HttpRequestP request,
							 WDVH_Bool *async);

	/*------------------------------------------------------------------------
		Function:	getDeletedItemsHeader
	--------------------------------------------------------------------------*/
	WDVH_Bool getDeletedItemsHeader(sapdbwa_HttpRequestP request,
									WDVH_Bool *delitems);

	/*------------------------------------------------------------------------
		Function:	createCapiUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool createCapiUserData(WDVH_CapiUserData *userdata);

	/*------------------------------------------------------------------------
		Function:	destroyCapiUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyCapiUserData(WDVH_CapiUserData userdata);

	/*------------------------------------------------------------------------
		Function:	createWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool createWdvhHandle(sapdbwa_Handle wa,sapdbwa_DbConnectionP conn,WDVH_Handle *handle);

	/*------------------------------------------------------------------------
		Function:	resetWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool resetWdvhHandle(WDVH_Handle handle);

	/*------------------------------------------------------------------------
		Function:	destroyWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyWdvhHandle(WDVH_Handle handle);

	/*------------------------------------------------------------------------
		Function:	createErrorhHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool createErrorHandle(WDVH_ErrorHandle *error);

	/*------------------------------------------------------------------------
		Function:	resetErrorHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool resetErrorHandle(WDVH_ErrorHandle error);

	/*------------------------------------------------------------------------
		Function:	destroyErrorHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyErrorHandle(void *error);

	/*------------------------------------------------------------------------
		Function:	createConnUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool createConnUserData(WDVH_ConnUserData *userdata);

	/*------------------------------------------------------------------------
		Function:	destroyConnUserData
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyConnUserData(void *userdata);

	/*------------------------------------------------------------------------
		Function:	createDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool createDocClassList(WDVH_DocClassList *list);

	/*------------------------------------------------------------------------
		Function:	destroyDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyDocClassList(WDVH_DocClassList list);

	/*------------------------------------------------------------------------
		Function:	initDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool initDocClassList(WDVH_DocClassList list);

	/*------------------------------------------------------------------------
		Function:	addItemDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool addItemDocClassList(WDVH_DocClassList		list, 
								  WDVH_Char				*docClassName, 
								  WDVCAPI_XmlDocClassId docClassId);

	/*------------------------------------------------------------------------
		Function:	findDocClassList
	--------------------------------------------------------------------------*/
	WDVH_Bool findDocClassList(WDVH_DocClassList		list,
							   WDVH_Char				*docClassName,
							   WDVCAPI_XmlDocClassId	*docClassId);

	/*------------------------------------------------------------------------
		Function:	getXmlmdlapiHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool getXmlmdlapiHandle(sapdbwa_Handle wa,sapdbwa_DbConnectionP conn,XMLIMAPI_Handle *handle);

	/*------------------------------------------------------------------------
		Function:	destroyXmlmdlapiHandle
	--------------------------------------------------------------------------*/
	WDVH_Bool destroyXmlmdlapiHandle(XMLIMAPI_Handle handle);

	/*------------------------------------------------------------------------
		Function:	send initial GUI Html Page
	--------------------------------------------------------------------------*/
	void sendGuiStartPage(sapdbwa_Handle    wa,
						  sapdbwa_HttpReplyP   rep,
						  const SAPDB_Char  *server,
						  const SAPDB_Char	*servicename);

	/*------------------------------------------------------------------------
		Function:	send GUI Navigation Html Page
	--------------------------------------------------------------------------*/
	void sendGuiNaviPage(sapdbwa_Handle    wa,
						  sapdbwa_HttpReplyP   rep,
						  const SAPDB_Char *querystring,
						  const SAPDB_Char  *server,
					  	  const SAPDB_Char *sFile);

	WDVH_Bool getParameterValue(const WDVH_Char *pParamName,
				 	 		    sapdbwa_HttpRequestP req,
				 	 		    WDVH_Char *sValue);
	const WDVH_Char* getParameterValueWithoutCopy(const WDVH_Char *pParamName,
				 	 					sapdbwa_HttpRequestP req);

	void transformFileSize(WDVH_Char *in, WDVH_Char *out, WDVH_Bool complete);
	void WDVH_EscapeQuotes(WDVH_Char *oldstring, WDVH_Int4 len, WDVH_Char *newstring, WDVH_Int4 len_new);
	void WDVH_UnEscapeUri(WDVH_PChar uri);

	/*------------------------------------------------------------------------
		Function:	WDVH_CheckContentType
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371
	--------------------------------------------------------------------------*/
	WDVH_Bool WDVH_CheckContentType(WDVH_PChar contenttype);

	/*------------------------------------------------------------------------
		Function:	WDVH_CheckPropertySet
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441
	--------------------------------------------------------------------------*/
	WDVH_Bool WDVH_CheckPropertySet(WDVH_PChar tag);

	/*------------------------------------------------------------------------
		Function:	WDVH_CheckPropertyRemove
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441
	--------------------------------------------------------------------------*/
	WDVH_Bool WDVH_CheckPropertyRemove(WDVH_PChar tag);

	/*------------------------------------------------------------------------
		Function:	WDVH_sendBodyChunk
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1129986
	--------------------------------------------------------------------------*/
	void WDVH_sendBodyChunk(sapdbwa_HttpReplyP reply, WDVH_Char *data);
	void WDVH_sendBodyChunkPropfind(sapdbwa_HttpReplyP reply, WDVH_Char *data, WDVH_Bool isMSWebFolder);
	void WDVH_sendBodyChunkEnd(sapdbwa_HttpReplyP reply);

	WDVH_Char bin2hex (WDVH_Char ch);
	WDVH_Int4 AlphaNumeric (WDVH_Char ch);
	WDVH_Int4 URLencode (const WDVH_Char * plain, WDVH_Char * encode, WDVH_Int4 maxlen);



#endif
