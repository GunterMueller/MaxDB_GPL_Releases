#ifndef WDVHANDLER_PROPFINDHANDLER_H
#define WDVHANDLER_PROPFINDHANDLER_H

/****************************************************************************

  module      : WDVHandler_PropfindHandler.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  13:29
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
/*	#define sapdbwa_MEMCHECK
*/
	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Connection.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_StringBuffer.h"
/*	#include "SAPDB/WebDAV/Handler/WDVHandler_BufferList.h"
*/
  
	/*------------------------------------------------------------------------
	Function	:	propfindCallCapiFunc
	Description	:	Call WebDAV CAPI Function to create a collection
	Parameters	:	[in]	requesturi	-	HTTP RequestURI
	Return		:	statuscode	-	HTTP Status code
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 propfindCallCapiFunc(WDVCAPI_WDV				wdv,
									  sapdbwa_Handle		wa,
									  sapdbwa_HttpRequestP request,
									  WDVCAPI_PropfindHandle	propfind);

	/*------------------------------------------------------------------------
	Function	:	propfindParseRequestBody
	Description	:	parse request body and collect input data for CAPI
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 propfindParseRequestBody(sapdbwa_Handle			wa,
										  WDVCAPI_WDV				wdv,
										  WDVCAPI_PropfindHandle*	propfind,
										  sapdbwa_HttpRequestP		request,
										  WDVCAPI_Depth				depth,
									      WDVH_Bool					*isPropnameRequest,		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
										  WDVH_Bool					*isEmptyPropertyList,
										  WDVH_Char					*errormsg);				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */

	/*------------------------------------------------------------------------
	Function	:	propfindBuildReply
	Description	:	build the HTTP response
	Parameters	:	[in] statuscode	-	HTTP Status code
	Return		:	---
	--------------------------------------------------------------------------*/
	WDVH_Bool propfindBuildReply(sapdbwa_Handle		wa,
						     sapdbwa_Int2 statuscode, 
								sapdbwa_HttpReplyP reply, 
								WDVH_Char *host,
								WDVH_Char *port,
								WDVH_Char *uri,
								WDVCAPI_WDV wdv,
								WDVCAPI_PropfindHandle  propfind,
							    WDVH_Bool isPropnameRequest,	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
								WDVH_Bool isEmptyPropertyList,  /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
								WDVH_Char *userAgent,
								WDVH_Char	*errormsg);			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */

	/*------------------------------------------------------------------------
	Function	:	propfindXmlTagStartHandler
	Description	:	call back function for expat to deal with XML start tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
					[in]	 attr	-	array with attributes of the XML tag
	Return		:	---
	--------------------------------------------------------------------------*/
	void propfindXmlTagStartHandler(void *data, const char *el, const char **attr);

	/*------------------------------------------------------------------------
	Function	:	propfindXmlTagEndHandler
	Description	:	call back function for expat to deal with XML end tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
	Return		:	---
	--------------------------------------------------------------------------*/
	void propfindXmlTagEndHandler(void *data, const char *el);

	/*------------------------------------------------------------------------
	Function	:	propfindXmlStartNamespaceHandler
	Description	:	call back function for expat to deal with Namespace 
					declarations in start tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 prefix	-	namespace abbreviation
					[in]	 uri	-	namespace name/uri
	Return		:	---
	--------------------------------------------------------------------------*/
	WDVH_Bool propfindXmlStartNamespaceHandler(void *data, const XML_Char *prefix, const XML_Char *uri);

	/*------------------------------------------------------------------------
	Function	:	propfindXmlEndNamespaceHandler
	Description	:	call back function for expat to deal with end of a Namespace 
					scope
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 prefix	-	namespace abbreviation
	Return		:	---
	--------------------------------------------------------------------------*/
	void propfindXmlEndNamespaceHandler(void *data, const XML_Char *prefix);

	#define FAKE_ALLPROP "<?xml version=\"1.0\" encoding=\"utf-8\" ?><D:propfind xmlns:D=\"DAV:\"><D:prop><D:displayname/><D:getcontenttype/><D:getcontentlength/><D:getlastmodified/><D:resourcetype/></D:prop></D:propfind>"

#endif
