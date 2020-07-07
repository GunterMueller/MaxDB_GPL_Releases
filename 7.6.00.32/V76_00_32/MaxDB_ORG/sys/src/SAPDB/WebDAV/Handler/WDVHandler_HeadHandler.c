/****************************************************************************

  module      : WDVHandler_HeadHandler.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-30  13:35
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


	#include "SAPDB/WebDAV/Handler/WDVHandler_HeadHandler.h"

	/*------------------------------------------------------------------------
		Function: headCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 headCallCapiFunc(sapdbwa_Handle			wa,
   							     sapdbwa_HttpRequestP	request,
						 		 WDVCAPI_WDV			pCapiHandle,
								 WDVCAPI_URI			requesturi)
	{
		sapdbwa_Int2			rc;
		WDV_Bool				success;
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
		WDVH_Char				*server;

		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || pCapiHandle==NULL || requesturi==NULL) {
			sapdbwa_WriteLogMsg(wa, "headCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}

		if (WDVCAPI_Head(pCapiHandle,requesturi)) 
			success = WDVH_True;
		else 
			success = WDVH_False;

		/* get server string for error messages */
		buildServerString(request,&server);
		
		if (success==WDV_False) {
			WDVCAPI_GetLastError(pCapiHandle,&errorItem);
			WDVCAPI_GetErrorType(errorItem,&errorType);
			switch (errorType) {
				case WDVCAPI_ERR_TYPE_UNDEFINED:
				case WDVCAPI_ERR_TYPE_SQL: 
					{
						/* internal server error */
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						error = ERROR_500;
						sp77sprintf(errorText,1000,ERROR_HEAD_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_HEAD_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						switch(errorCode) {
							case WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST:
								error = ERROR_404;
								break;
							default:
								{
									error = ERROR_500;
									sp77sprintf(errorText,1000,ERROR_HEAD_URI,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_HEAD_500,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);
									break;
								}
						}
						break;
					}
				default:
					{
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						error = ERROR_500;
						sp77sprintf(errorText,1000,ERROR_HEAD_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_HEAD_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			error = ERROR_200;
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	headBuildReply
	--------------------------------------------------------------------------*/
	void headBuildReply(sapdbwa_Int2 statuscode, 
					   sapdbwa_HttpReplyP reply,
					   WDVCAPI_WDV	pCapiHandle)
	{
		WDVCAPI_ContentLength		contentLength;
		WDVH_Char					strContentLength[100];
		WDVCAPI_ContentTypeString	contentType;
		WDVCAPI_PropertyShortValueBuffer  lastModified;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126839 */

		if (reply==NULL || pCapiHandle==NULL) {
			sendErrorReply(ERROR_500, reply, WEBDAV_HEAD,"");
			return;
		}

		switch(statuscode) {
			case ERROR_200:
				{
					if (!WDVCAPI_HeadGetContentType(pCapiHandle,contentType)) {
						/* error handling */
					}			
					if (!WDVCAPI_HeadGetLastModified(pCapiHandle, lastModified)) {	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126839 */
						/* error handling */									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126839 */
					}															/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126839 */

					/* initialize Response Header */
					sapdbwa_InitHeader(reply,
									   statuscode,
									   contentType,
									   NULL,
				                       NULL,					/* const char    *LastModified,*/
									   NULL,					/* const char    *Expires,*/
									   NULL);					/* const char    *Location );*/

					if (WDVCAPI_HeadGetContentLength(pCapiHandle,&contentLength)) {
						sp77sprintf(strContentLength,100,"%d",contentLength);
						sapdbwa_SetHeader( reply,
										   "Content-Length",
										   strContentLength );
					}			
					sapdbwa_SetHeader( reply,"Last-Modified",lastModified);			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126839 */

/*
					sapdbwa_SetHeader( reply,
									   "DAV",
									   "1" );
*/
					sapdbwa_SendHeader(reply);
					break;
				}
			case ERROR_404: 
				{
					/* initialize Response Header */
					sapdbwa_InitHeader(reply,
									   statuscode,
									   "text/html",
									   NULL,
									   NULL,								/* const char    *LastModified,*/
									   NULL,								/* const char    *Expires,*/
									   NULL);								/* const char    *Location );*/

					sp77sprintf(strContentLength,100,"%d",strlen(ERROR_PAGE_404));
					sapdbwa_SetHeader( reply,
									   "Content-Length",
									   strContentLength );

					sapdbwa_SendHeader(reply);

					sapdbwa_SendBody(reply,
					                 ERROR_PAGE_404,
							         strlen(ERROR_PAGE_404));
					break; 
				}
			case ERROR_501:
				{
					/* initialize Response Header */
					sapdbwa_InitHeader(reply,
									   statuscode,
									   "text/html",
									   NULL,
									   NULL,								/* const char    *LastModified,*/
									   NULL,								/* const char    *Expires,*/
									   NULL);								/* const char    *Location );*/

					sp77sprintf(strContentLength,100,"%d",strlen(ERROR_PAGE_501));
					sapdbwa_SetHeader( reply,
									   "Content-Length",
									   strContentLength );

					sapdbwa_SendHeader(reply);

					sapdbwa_SendBody(reply,
					                 ERROR_PAGE_501,
							         strlen(ERROR_PAGE_501));
					break;
				}
			default:
				{
					/* initialize Response Header */
					sapdbwa_InitHeader(reply,
									   statuscode,
									   "text/html",
									   NULL,
									   NULL,								/* const char    *LastModified,*/
									   NULL,								/* const char    *Expires,*/
									   NULL);								/* const char    *Location );*/

					sp77sprintf(strContentLength,100,"%d",strlen(ERROR_PAGE_500));
					sapdbwa_SetHeader( reply,
									   "Content-Length",
									   strContentLength );

					sapdbwa_SendHeader(reply);

					sapdbwa_SendBody(reply,
					                 ERROR_PAGE_500,
							         strlen(ERROR_PAGE_500));
				}
		}
	}