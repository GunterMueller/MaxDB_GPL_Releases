#ifndef WDVHandler_CopyHandler_H
#define WDVHandler_CopyHandler_H

/****************************************************************************

  module      : WDVHandler_CopyHandler.h

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
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	
  
	/*------------------------------------------------------------------------
	Function	:	copyCallCapiFunc
	Description	:	Call WebDAV CAPI Function to create a collection
	Parameters	:	[in]	requesturi	-	HTTP RequestURI
	Return		:	statuscode	-	HTTP Status code
	--------------------------------------------------------------------------*/
    sapdbwa_Int2 copyCallCapiFunc(sapdbwa_Handle		wa, 
								  sapdbwa_HttpRequestP	request, 
								  WDVCAPI_WDV			pCapiHandle,
								  WDVCAPI_URI			sourceUri,
								  WDVCAPI_URI			destinationUri,
								  WDVCAPI_Depth			depth,
								  WDVCAPI_Overwrite		overwrite,
								  WDVH_CapiUserData		userData,
								  WDVCAPI_LockIdList	lockTokenList);

	/*------------------------------------------------------------------------
	Function	:	copyBuildReply
	Description	:	build the HTTP response
	Parameters	:	[in] statuscode	-	HTTP Status code
	Return		:	---
	--------------------------------------------------------------------------*/
	void copyBuildReply(sapdbwa_Int2		statuscode, 
		                sapdbwa_HttpReplyP	reply, 
						WDVCAPI_URI			resourceURI, 
						WDVCAPI_WDV			pCapiHandle,
						WDVH_CapiUserData	userData);

	/*------------------------------------------------------------------------
	Function	:	copyErrorCallBack
	Description	:	call back function for error handling (sending method reply)
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	void copyErrorCallBack(void *clientData, 
                           WDVCAPI_URI        uri,
						   WDVCAPI_ErrorItem error);

#endif