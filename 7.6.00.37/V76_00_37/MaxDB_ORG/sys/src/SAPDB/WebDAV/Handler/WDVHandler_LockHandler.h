#ifndef WDVHandler_LockHandler_H
#define WDVHandler_LockHandler_H

/****************************************************************************

  module      : WDVHandler_LockHandler.h

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


	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Connection.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	
  
	/*------------------------------------------------------------------------
	Function	:	lockCallCapiFunc
	Description	:	Call WebDAV CAPI Function to create a collection
	Parameters	:	[in]	requesturi	-	HTTP RequestURI
	Return		:	statuscode	-	HTTP Status code
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 lockCallCapiFunc(sapdbwa_Handle		wa,
   								  WDVCAPI_WDV			pCapiHandle,
								  sapdbwa_HttpRequestP	request,
						 		  WDVCAPI_URI			requesturi,
								  WDVH_CapiUserData		userData,
								  WDVH_Char				*errmsg);			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */

	/*------------------------------------------------------------------------
	Function	:	lockBuildReply
	Description	:	build the HTTP response
	Parameters	:	[in] statuscode	-	HTTP Status code
	Return		:	---
	--------------------------------------------------------------------------*/
	void lockBuildReply(sapdbwa_Int2		statuscode, 
						sapdbwa_HttpReplyP	reply,
						WDVH_CapiUserData	userData,
						WDVCAPI_URI			requesturi,
						WDVH_Char			*errmsg);						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */

	/*------------------------------------------------------------------------
	Function	:	lockParseRequestBody
	Description	:	parse request body and collect input data for CAPI
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 lockParseRequestBody(sapdbwa_Handle		wa,
									  WDVCAPI_WDV			wdv,
									  sapdbwa_HttpRequestP	request,
									  WDVH_CapiUserData		userData,
									  WDVH_Char				*errmsg);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */

	/*------------------------------------------------------------------------
	Function	:	lockXmlTagStartHandler
	Description	:	call back function for expat to deal with XML start tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
					[in]	 attr	-	array with attributes of the XML tag
	Return		:	---
	--------------------------------------------------------------------------*/
	void lockXmlTagStartHandler(void *data, const char *el, const char **attr);

	/*------------------------------------------------------------------------
	Function	:	lockXmlTagEndHandler
	Description	:	call back function for expat to deal with XML end tags
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
	Return		:	---
	--------------------------------------------------------------------------*/
	void lockXmlTagEndHandler(void *data, const char *el);

	/*------------------------------------------------------------------------
	Function	:	lockReadOwnerShortValue
	Description	:	read the short value of the owner tag
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
	Return		:	---
	--------------------------------------------------------------------------*/
	void lockReadOwnerShortValue(WDVH_XmlUserData data, WDVH_Char *el);

	/*------------------------------------------------------------------------
		Function:	lockOwnerShortValueStartPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 
	--------------------------------------------------------------------------*/
	void lockOwnerShortValueStartPos(WDVH_XmlUserData data, WDVH_Char *el);

	/*------------------------------------------------------------------------
		Function:	lockOwnerShortValueEndPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043
	--------------------------------------------------------------------------*/
	void lockOwnerShortValueEndPos(WDVH_XmlUserData data, WDVH_Char *el);

	/*------------------------------------------------------------------------
	Function	:	lockErrorCallBack
	Description	:	call back function for building the reply in case of occuring errors
	Parameters	:	[in/out] data	-	user data for communication between handlers
					[in]	 el		-	XML Tag to handle
	Return		:	---
	--------------------------------------------------------------------------*/
	void lockErrorCallBack(void					*clientData, 
						   WDVCAPI_URI			uri, 
						   WDVCAPI_ErrorItem	errorItem); 
#endif
