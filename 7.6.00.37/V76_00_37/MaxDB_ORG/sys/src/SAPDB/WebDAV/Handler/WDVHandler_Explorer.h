#ifndef WDVHandler_EXPLORER_H
#define WDVHandler_EXPLORER_H

/****************************************************************************

  module      : WDVHandler_Explorer.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  10:40
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


*****************************************************************************

  This header contains all the WebDAV Method Handlers that are currently 
  supported

*****************************************************************************/

	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"

	/*------------------------------------------------------------------------
		Function: doServiceExplorer
	--------------------------------------------------------------------------*/
	void doServiceExplorer(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,WDVCAPI_WDV pCapiHandle, WDVH_Handle wdvhHandle, XMLIMAPI_Handle xmlHandle, WDVH_Bool compress);
	void showCollection(sapdbwa_Handle		wa, 
  					   sapdbwa_HttpRequestP request,
					   sapdbwa_HttpReplyP	reply, 
					   WDVCAPI_WDV			pCapiHandle,
					   WDVH_Handle			wdvhHandle,
					   WDVH_Char			*host,
					   WDVH_Char			*port,
					   WDVCAPI_URI			resourceURI);
	void propfindBuildExplorerReply(sapdbwa_Handle			wa,
									sapdbwa_HttpRequestP	request,
									sapdbwa_HttpReplyP		reply,
									WDVH_Char				*uri,
									WDVH_Char				*location,
									WDVCAPI_WDV				wdv,
									WDVCAPI_PropfindHandle  propfind);
	void getBuildExplorerReply(sapdbwa_Handle wa,
							   sapdbwa_HttpRequestP request, 
							   sapdbwa_HttpReplyP reply, 
							   WDVCAPI_WDV	pCapiHandle,
							   WDVH_Handle	wdvhHandle,
							   WDVH_Char	*resource,
							   WDVH_Char    *location,
							 WDVH_Char				*locktoken,
					 		 WDVCAPI_LockHandle		lock);
	void showXmlProperties(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   XMLIMAPI_Handle			xmlHandle,
						   WDVH_Bool				errorsoccured,
						   WDVH_Char				*errormsg);
	void showXQueryScreen(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   WDVH_Handle				wdvhHandle,
						   WDVH_Bool				erroroccured,
						   WDVH_Char				*errormsg);
	void collectionLockProps(sapdbwa_Handle			wa,
							 WDVH_Char				*uri,
							 WDVH_Char				*location,
							 WDVCAPI_WDV			wdv,
							 WDVCAPI_PropfindHandle propfind,
							 WDVH_Char				*locktoken,
					 		 WDVCAPI_LockHandle		*lock);
	void restoreScreen(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   WDVH_Bool				erroroccured,
						   WDVH_Char				*errormsg);
	void mkColScreen(sapdbwa_Handle			wa,
				     sapdbwa_HttpRequestP	request,
				     sapdbwa_HttpReplyP		reply,
				     WDVH_Char				*uri,
					 WDVH_Char				*location,
					 WDVCAPI_WDV			wdv,
					 WDVH_Bool				erroroccured,
					 WDVH_Char				*errormsg);
	void uploadFileScreen(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   WDVH_Bool				erroroccured,
						   WDVH_Char				*errormsg);
	SAPDB_Bool uploadFile(sapdbwa_Handle		wa,
						  sapdbwa_HttpRequestP  req,
						  sapdbwa_HttpReplyP    rep,
						  WDVH_Char				*location,
						  WDVCAPI_WDV			wdv,
						  WDVH_Handle			wdvh,
						  WDVH_Bool				compress,
						  WDVH_Bool				*errorsoccured,
						  WDVH_Char				*errormsg);
	WDVH_Bool storeMimeParts(sapdbwa_Handle			wa,
							 sapdbwa_HttpRequestP	req,
							 WDVH_Char				*location,
							 WDVCAPI_WDV			wdv,
							 WDVH_Handle			wdvh,
							 WDVH_Bool				compress,
						     WDVH_Bool				*errorsoccured,
							 WDVH_Char				*errormsg,
							 const char				*boundary);
	void trimFileName(const WDVH_Char	*buf, 
					  WDVH_ULong		*filePathStart, 
					  WDVH_ULong		*filePathLen, 
					  WDVH_ULong		*fileNameStart, 
					  WDVH_ULong		*fileNameLen);
	WDVH_Bool getFileInput(const WDVH_Char		*buf, 
						   WDVH_ULong			buflen, 
						   WDVH_Bool			*found,
						   WDVH_ULong			*filePathStart,
						   WDVH_ULong			*filePathLen,
						   WDVH_ULong			*fileNameStart,
						   WDVH_ULong			*fileNameLen);
	WDVH_Bool getIsMultipart(const WDVH_Char	*buf, 
							 WDVH_ULong			buflen, 
							 WDVH_Bool			*found,
							 WDVH_Char			*subBoundary);
	WDVH_Bool getInputElement(const WDVH_Char	*buf,
							  WDVH_ULong		buflen,
							  const WDVH_Char	*inputElementName,
							  WDVH_Bool			*found);
	WDVH_Bool compareBuf(const WDVH_Char	*buf,
						 const WDVH_Char	*lower,
						 const WDVH_Char	*upper,
						 WDVH_ULong			len);
	sapdbwa_Int2 putFile(sapdbwa_Handle			wa, 
						 sapdbwa_HttpRequestP	request, 
						 WDVCAPI_WDV			pCapiHandle, 
						 WDVH_Handle			wdvh,
						 WDVCAPI_URI			requesturi,
						 WDVH_Bool				compress,
						 const WDVH_Char		*boundary,
						 WDVH_Bool				*moreParts,
						 WDVH_Char				*putContentType,
						 WDVH_Bool				*errorsoccured,
						 WDVH_Char				*errormsg);


#endif