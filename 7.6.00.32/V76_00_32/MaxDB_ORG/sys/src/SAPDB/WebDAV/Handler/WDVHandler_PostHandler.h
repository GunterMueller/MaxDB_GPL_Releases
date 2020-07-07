#ifndef WDVHandler_PostHandler_H
#define WDVHandler_PostHandler_H

/****************************************************************************

  module      : WDVHandler_PostHandler.h

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
	
  
	/*------------------------------------------------------------------------
	Function	:	postCallCapiFunc
	Description	:	Call WebDAV CAPI Function to create a collection
	Parameters	:	[in]	requesturi	-	HTTP RequestURI
	Return		:	statuscode	-	HTTP Status code
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 postCallCapiFunc(sapdbwa_Handle		wa, 
								  sapdbwa_HttpRequestP	request, 
								  WDVCAPI_WDV			pCapiHandle, 
								  WDVH_Handle			wdvh,
								  WDVCAPI_URI			requesturi,
								  WDVCAPI_XmlDocClassId	docClassId,
								  WDVH_Bool				async,
								  WDVCAPI_LockIdString	locktoken,
								  WDVH_Bool				compress,
								  WDVH_Bool				putToXmldb,
								  WDVH_Char				*putContentType,		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
								  WDVH_Char				*errmsg);				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */

	/*------------------------------------------------------------------------
	Function	:	postBuildReply
	Description	:	build the HTTP response
	Parameters	:	[in] statuscode	-	HTTP Status code
	Return		:	---
	--------------------------------------------------------------------------*/
	void postBuildReply(sapdbwa_Int2 statuscode, 
						sapdbwa_HttpReplyP reply,
						WDVH_Char* msg);				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */

	/*------------------------------------------------------------------------
	Function	:	postCheckDocumentClass
	Description	:	get Document Class Id using SAPDB_DocumentClass Header and
					modelling API
	Parameters	:	[in] statuscode	-	HTTP Status code
	Return		:	---
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 postCheckDocumentClass(sapdbwa_Handle			wa, 
										sapdbwa_HttpRequestP	request, 
										WDVH_Handle				wdvh,
										XMLIMAPI_Id				*id,
										WDVH_Bool				*async);

#endif