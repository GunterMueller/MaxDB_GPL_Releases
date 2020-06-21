#ifndef WDVHandler_GetHandler_H
#define WDVHandler_GetHandler_H

/****************************************************************************

  module      : WDVHandler_GetHandler.h

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
	Function	:	GetCallCapiFunc
	Description	:	Call WebDAV CAPI Function to create a collection
	Parameters	:	[in]	requesturi	-	HTTP RequestURI
	Return		:	statuscode	-	HTTP Status code
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 getCallCapiFunc(sapdbwa_Handle			wa,
   							     sapdbwa_HttpRequestP	request,
   							     sapdbwa_HttpReplyP	    reply,
						 		 WDVCAPI_WDV			pCapiHandle,
								 WDVCAPI_URI			requesturi,
								 WDVCAPI_LockIdString	lockIdString,
								 WDVH_Long				crFrom,				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
								 WDVH_Long				crTo,				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
                                 WDV_Bool              *isDocInXDB);

	/*------------------------------------------------------------------------
	Function	:	GetBuildReply
	Description	:	build the HTTP response
	Parameters	:	[in] statuscode	-	HTTP Status code
	Return		:	---
	--------------------------------------------------------------------------*/
	void getBuildReply(sapdbwa_Handle wa,
					   sapdbwa_Int2 statuscode, 
					   sapdbwa_HttpRequestP request,
					   sapdbwa_HttpReplyP reply, 
					   WDVCAPI_WDV	pCapiHandle,
					   WDVH_Handle	wdvhHandle,
					   WDVH_Char    *host,
					   WDVH_Char    *port,
					   WDVCAPI_URI  resourceURI,
 					   WDVH_Long	crFrom,									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
					   WDVH_Long	crTo,									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
                       WDV_Bool     isDocInXDB);

	/*------------------------------------------------------------------------
	Description	:	Defines for GET on collections result page
	--------------------------------------------------------------------------*/
/*	#define WDVH_GET_PAGE_BEGIN_PART1	"<HTML><HEAD><TITLE>Content of "
	#define WDVH_GET_PAGE_BEGIN_PART2	"</TITLE></HEAD><BODY>"
	#define WDVH_GET_PAGE_END			"</BODY></HTML>"
	#define	WDVH_GET_HEADING_OPEN		"<H1>Content of <I>"
	#define WDVH_GET_HEADING_CLOSE		"</I></H1>"
	#define WDVH_GET_THIS_FOLDER_OPEN	"<p><a href=\"http://"
	#define WDVH_GET_THIS_FOLDER_CLOSE	"\">.</a></p>"
	#define WDVH_GET_FOLDER_UP_OPEN		"<p><a href=\"http://"
	#define WDVH_GET_FOLDER_UP_CLOSE	"\">..</a></p>"
	#define WDVH_GET_COLLECTION_OPEN	"<p><a href=\"http://"
	#define WDVH_GET_COLLECTION_MIDDLE	"/\">"
	#define WDVH_GET_COLLECTION_CLOSE	"</a></p>"
	#define WDVH_GET_RESOURCE			"<p><a href=\"http://%s\">%s</a></p>"

	#define WDVH_GET_XML_BEGIN			"<?xml version=\"1.0\" encoding=\"utf-8\" ?> \n \
											<A:multistatus xmlns:A=\"DAV:\"> \n \
												<A:response>\n"
	#define WDVH_GET_XML_HREF_BEGIN		"			<A:href>http://"
	#define WDVH_GET_XML_HREF_END		"</A:href>\n"
	#define	WDVH_GET_XML_END			"		</A:response> \n \
											</A:multistatus>\n"

*/
#endif