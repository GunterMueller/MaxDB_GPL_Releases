#ifndef WDVHANDLER_PROPPATCHHANDLER_H
#define WDVHANDLER_PROPPATCHHANDLER_H

/****************************************************************************

  module      : WDVHandler_ProppatchHandler.h

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

  
	/*------------------------------------------------------------------------
	Function	:	proppatchCallCapiFunc
	Description	:	Call WebDAV CAPI Function to create a collection
	Parameters	:	[in]	requesturi	-	HTTP RequestURI
	Return		:	statuscode	-	HTTP Status code
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 proppatchCallCapiFunc(WDVCAPI_WDV				wdv,
									  sapdbwa_Handle			wa,
									  sapdbwa_HttpRequestP		request,
									  WDVCAPI_ProppatchHandle	proppatch); 

	/*------------------------------------------------------------------------
	Function	:	proppatchParseRequestBody
	Description	:	parse request body and collect input data for CAPI
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 proppatchParseRequestBody(sapdbwa_Handle		wa,
										  WDVCAPI_WDV wdv,
										  WDVCAPI_ProppatchHandle proppatch,
										  sapdbwa_HttpRequestP request,
										  WDVH_Bool					*iserror);

	/*------------------------------------------------------------------------
	Function	:	proppatchBuildReply
	Description	:	build the HTTP response
	Parameters	:	[in] statuscode	-	HTTP Status code
	Return		:	---
	--------------------------------------------------------------------------*/
	WDVH_Bool proppatchBuildReply(sapdbwa_Int2 statuscode,
							sapdbwa_Handle			wa,
							sapdbwa_HttpRequestP request,
							sapdbwa_HttpReplyP reply, 
							WDVH_Char *host,
							WDVH_Char *port,
							WDVH_Char *uri,
							WDVCAPI_WDV wdv,
							WDVCAPI_ProppatchHandle proppatch,
							WDVH_Bool				iserror); 

	/*------------------------------------------------------------------------
	Function	:	proppatchXmlTagStartHandler
	Description	:	call back function for expat to deal with XML start tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
					[in]	 attr	-	array with attributes of the XML tag
	Return		:	---
	--------------------------------------------------------------------------*/
	void proppatchXmlTagStartHandler(void *data, const char *el, const char **attr);

	/*------------------------------------------------------------------------
	Function	:	proppatchXmlTagEndHandler
	Description	:	call back function for expat to deal with XML end tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
	Return		:	---
	--------------------------------------------------------------------------*/
	void proppatchXmlTagEndHandler(void *data, const char *el);

	/*------------------------------------------------------------------------
	Function	:	proppatchXmlStartNamespaceHandler
	Description	:	call back function for expat to deal with Namespace 
					declarations in start tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 prefix	-	namespace abbreviation
					[in]	 uri	-	namespace name/uri
	Return		:	---
	--------------------------------------------------------------------------*/
	void proppatchXmlStartNamespaceHandler(void *data, const XML_Char *prefix, const XML_Char *uri);

	/*------------------------------------------------------------------------
	Function	:	proppatchXmlEndNamespaceHandler
	Description	:	call back function for expat to deal with end of a Namespace 
					scope
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 prefix	-	namespace abbreviation
	Return		:	---
	--------------------------------------------------------------------------*/
	void proppatchXmlEndNamespaceHandler(void *data, const XML_Char *prefix);

	/*------------------------------------------------------------------------
		Function:	proppatchReadShortValue
	--------------------------------------------------------------------------*/
	void proppatchReadShortValue(WDVH_XmlUserData data, WDVH_Char *el);

	/*------------------------------------------------------------------------
		Function:	getPropertyShortValueStartPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800
	--------------------------------------------------------------------------*/
	void getPropertyShortValueStartPos(WDVH_XmlUserData data, WDVH_Char *el);

	/*------------------------------------------------------------------------
		Function:	getPropertyShortValueEndPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800
	--------------------------------------------------------------------------*/
	void getPropertyShortValueEndPos(WDVH_XmlUserData data, WDVH_Char *el); 


#endif