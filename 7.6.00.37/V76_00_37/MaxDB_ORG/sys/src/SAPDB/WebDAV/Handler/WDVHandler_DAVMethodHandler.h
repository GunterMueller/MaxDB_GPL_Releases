#ifndef WDVHandler_DAVMETHODHANDLER_H
#define WDVHandler_DAVMETHODHANDLER_H

/****************************************************************************

  module      : WDVHandler_DAVMethodHandler.h

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

	#include "SAPDB/WebDAV/Handler/WDVHandler_MKCOLHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DeleteHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_GetHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_HeadHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_OptionsHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_PropfindHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_PostHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_CopyHandler.h"



	/*------------------------------------------------------------------------
		Function	: webdavMkcolHandler
		Description	: Handler for MKCOL Requests (creating of a collection)
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavMkcolHandler( sapdbwa_Handle		  wa,
						     sapdbwa_HttpRequestP request,
						     sapdbwa_HttpReplyP   reply );

	/*------------------------------------------------------------------------
		Function	: webdavDeleteHandler
		Description	: Handler for DELETE Requests (creating of a collection)
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavDeleteHandler( sapdbwa_Handle		wa,
					 	      sapdbwa_HttpRequestP  request,
						      sapdbwa_HttpReplyP    reply );

	/*------------------------------------------------------------------------
		Function	: webdavGetHandler
		Description	: Handler for GET Requests 
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavGetHandler( sapdbwa_Handle		wa,
				   	       sapdbwa_HttpRequestP  request,
					       sapdbwa_HttpReplyP    reply );

	/*------------------------------------------------------------------------
		Function	: webdavCopyHandler
		Description	: Handler for COPY Requests 
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	WDVH_Bool webdavCopyHandler(sapdbwa_Handle		 wa,
					   	       sapdbwa_HttpRequestP  request,
						       sapdbwa_HttpReplyP    reply );

	/*------------------------------------------------------------------------
		Function	: webdavMoveHandler
		Description	: Handler for MOVE Requests 
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	WDVH_Bool webdavMoveHandler(sapdbwa_Handle		 wa,
					   	       sapdbwa_HttpRequestP  request,
						       sapdbwa_HttpReplyP    reply );

	/*------------------------------------------------------------------------
		Function	: webdavHeadHandler
		Description	: Handler for HEAD Requests 
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavHeadHandler( sapdbwa_Handle		wa,
				   	       sapdbwa_HttpRequestP  request,
					       sapdbwa_HttpReplyP    reply );

	/*------------------------------------------------------------------------
		Function	: webdavOptionsHandler
		Description	: Handler for OPTIONS Requests 
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavOptionsHandler( sapdbwa_Handle		wa,
				   		       sapdbwa_HttpRequestP  request,
							   sapdbwa_HttpReplyP    reply );

	/*------------------------------------------------------------------------
		Function	: webdavPropfindHandler
		Description	: Handler for PROPFIND Requests (reading of properties for
					  given Resource(s))
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavPropfindHandler( sapdbwa_Handle		wa,
						        sapdbwa_HttpRequestP request,
						        sapdbwa_HttpReplyP   reply );

	/*------------------------------------------------------------------------
		Function	: webdavProppatchHandler
		Description	: Handler for PROPPATCH Requests (changing of properties 
					  for a given Resource)
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavProppatchHandler( sapdbwa_Handle		wa,
							     sapdbwa_HttpRequestP request,
								 sapdbwa_HttpReplyP   reply );

	/*------------------------------------------------------------------------
		Function	: webdavPostHandler
		Description	: Handler for POST Requests (reading of properties for
					  given Resource(s))
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavPostHandler( sapdbwa_Handle		wa,
					        sapdbwa_HttpRequestP request,
					        sapdbwa_HttpReplyP   reply );

	/*------------------------------------------------------------------------
		Function: webdavXqueryHandler
		ATTENTION: This is NO WebDAV method!!! 
		its an extension for xmldb. if there is more of it one time it should 
		be transfered to some file WDVHandler_XqueryHandler.cpp
	--------------------------------------------------------------------------*/
	void webdavXqueryHandler(sapdbwa_Handle		wa,
				  	        sapdbwa_HttpRequestP request,
					        sapdbwa_HttpReplyP   reply );

#endif