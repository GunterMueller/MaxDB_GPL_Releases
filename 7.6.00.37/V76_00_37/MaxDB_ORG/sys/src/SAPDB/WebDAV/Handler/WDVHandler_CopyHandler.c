/****************************************************************************

  module      : WDVHandler_CopyHandler.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  13:32
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


	#include "SAPDB/WebDAV/Handler/WDVHandler_CopyHandler.h"


	/*------------------------------------------------------------------------
		Function: copyCallCapiFunc
	--------------------------------------------------------------------------*/
    sapdbwa_Int2 copyCallCapiFunc(sapdbwa_Handle		wa, 
								  sapdbwa_HttpRequestP	request, 
								  WDVCAPI_WDV			pCapiHandle,
								  WDVCAPI_URI			sourceUri,
								  WDVCAPI_URI			destinationUri,
								  WDVCAPI_Depth			depth,
								  WDVCAPI_Overwrite		overwrite,
								  WDVH_CapiUserData		userData,
								  WDVCAPI_LockIdList	lockTokenList)
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
		WDVH_Bool				resourceCreated;

		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || pCapiHandle==NULL || sourceUri==NULL || destinationUri==NULL || userData==NULL) {
			sapdbwa_WriteLogMsg(wa, "copyCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}


		/* get server string for error messages */
		buildServerString(request,&server);


		success = WDVCAPI_Copy(pCapiHandle,
							   sourceUri,
							   destinationUri,
							   depth,
							   overwrite,
							   lockTokenList,
							   &copyErrorCallBack,
							   (void*)userData,
							   &resourceCreated);

		/* error handling */
		if (!success) {
			WDVCAPI_GetLastError(pCapiHandle,&errorItem);
			WDVCAPI_GetErrorType(errorItem,&errorType);
			switch (errorType) {
				case WDVCAPI_ERR_TYPE_UNDEFINED:
				case WDVCAPI_ERR_TYPE_SQL: 
					{
						/* internal server error */
						error = ERROR_500;
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						sp77sprintf(errorText,1000,ERROR_COPY_URI,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_COPY_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						switch(errorCode) {
							/* WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION 
							   = Resource Uri and Destination Uri MUST NOT be equal */
							case WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION:
								{
									error = ERROR_403;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_FORBIDDEN_403,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
							/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1119947 */
							case WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST:
								{
									error = ERROR_404;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_NOTFOUND_404,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
							/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1119947 */
								/* WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST 
									= the destination Uri contains at least one collection 
									  that does not exist rigth now */
							case WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST:
								{
									error = ERROR_409;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_PARENT_409,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
/* ---> CAPI: WDVCAPI_ERR_CODE_PROPERTY_BEHAVIOR_CONFLICT = die Properties konnten nicht gem‰ﬂ
								                            <propertybehavior>-Befehl kopiert
															werden 
							case WDVCAPI_ERR_CODE_PROPERTY_BEHAVIOR_CONFLICT:
								{
									error = ERROR_412;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_PARENT_412,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);
									break;
								}
*/
							/* WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT 
								= overwrite was verbidden and destination allready exists */
							case WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT:
								{
									error = ERROR_412;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_PRECONDITION_412,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
/* ---> CAPI: WDVCAPI_ERR_CODE_RESOURCE_LOCKED = destination ist gesperrt, kein copy */
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								{
									error = ERROR_423;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_LOCKED_423,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
/* ---> CAPI: WDVCAPI_ERR_CODE_BAD_GATEWAY = destination liegt auf anderem server und dieser 
								             verweigert die Annahme der Resource, kein copy
							case WDVCAPI_ERR_CODE_BAD_GATEWAY:
								{
									error = ERROR_502;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_BAD_GATEWAY_502,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);
									break;
								}
*/
							/* WDVCAPI_ERR_CODE_NO_MEMORY
							   = lack of memory */
							case WDVCAPI_ERR_CODE_NO_MEMORY:
								{
									error = ERROR_507;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_STORAGE_507,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);
									break;
								}
							default:
								{
									error = ERROR_500;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_COPY_URI,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_COPY_500,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);
									break;
								}
						}
						break;
					}
				default:
					{
						error = ERROR_500;
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						sp77sprintf(errorText,1000,ERROR_COPY_URI,server,(char*)sapdbwa_GetRequestURI(request),(char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_COPY_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			if (resourceCreated)
				/* a) destination existierte noch nicht -> 201 Created */
				error = ERROR_201;
			else
				/* b) destination existierte und overwrite war erlaubt -> 204 No Content */
				error = ERROR_204;
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}

		return error;

	}

	/*------------------------------------------------------------------------
		Function:	copyBuildReply
	--------------------------------------------------------------------------*/
	void copyBuildReply(sapdbwa_Int2		statuscode, 
		                sapdbwa_HttpReplyP	reply, 
						WDVCAPI_URI			resourceURI, 
						WDVCAPI_WDV			pCapiHandle,
						WDVH_CapiUserData   userData)
	{
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];

		if (reply==NULL || pCapiHandle==NULL || resourceURI==NULL || userData==NULL) {
			sendErrorReply(ERROR_500, reply, WEBDAV_COPY,"");
			return;
		}


		/* if there are errors at the subordinated resources a header was asent allready. 
		   in this case only send the footer of the body.
		   otherwise the error occured at the root resource and no body has to be sent */
		if (userData->errorsOccured == WDVH_False) {
			/* if there was an error it occured on the resource named by the request URI 
			   --> take the statuscode as it is */
			/* initialize Response Header */
			sapdbwa_InitHeader(reply,
			                   statuscode,
				               NULL,
					           NULL,
						       NULL,					/* const char    *LastModified,*/
							   NULL,					/* const char    *Expires,*/
							   NULL);					/* const char    *Location );*/
			sapdbwa_SetHeader( reply,
							   "Content-Length",
							   "0" );

			sapdbwa_SendHeader(reply);
		} else {
			/* there is at least one error occured on an resource different from the root 
			   resource. In this case the error call back function sent the reply. Only the
			   closing tag for multistatus is missing now */
			WDVH_sendBodyChunk(reply,WDVH_MULTI_STATUS_CLOSE);
			WDVH_sendBodyChunkEnd(reply);
		}


	}

	/*------------------------------------------------------------------------
		Function:	copyErrorCallBack
	--------------------------------------------------------------------------*/
	void copyErrorCallBack(void					*clientData, 
						   WDVCAPI_URI			uri, 
						   WDVCAPI_ErrorItem	errorItem) 
	{
		/* this one's called only if there is an error occured at a resource not
		   equal with the root resource of the copied tree */
		WDVH_CapiUserData		userData;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
		WDVH_Char				uri_enc[WDVH_MAX_URI_LEN+1];


		if (clientData == NULL || uri==NULL || errorItem==NULL) {
			return;
		}

		userData = (WDVH_CapiUserData)clientData;

		/* at first call a header has to be sent */
		if (userData->copyReplyHeaderSent == WDVH_False) {
			sapdbwa_InitHeader(userData->reply,
			                   ERROR_207,
				               "text/xml",
					           NULL,
						       NULL,					/* const char    *LastModified,*/
							   NULL,					/* const char    *Expires,*/
							   NULL);					/* const char    *Location );*/

			sapdbwa_SetHeader( userData->reply,
				   "Transfer-Encoding",
				   "chunked" );

			sapdbwa_SendHeader(userData->reply);
			
			/* and the beginning of the xml body has to be sent too */
			/* send body header */
			WDVH_sendBodyChunk(userData->reply,WDVH_XML_HEADER);
			WDVH_sendBodyChunk(userData->reply,WDVH_MULTI_STATUS_OPEN);
			

			/* and error occured has to be set */
			userData->copyReplyHeaderSent = WDVH_True;
			userData->errorsOccured = WDVH_True;
		} 
		if (userData->copyReplyHeaderSent == WDVH_True) {
			/* it's the second, third, ... turn around */
			/* send the response part for this special uri */
			/* response */
			WDVH_sendBodyChunk(userData->reply,WDVH_RESPONSE_OPEN);
			WDVH_sendBodyChunk(userData->reply,WDVH_XML_CLOSE_TAG);
			/*		href */
			WDVH_sendBodyChunk(userData->reply,WDVH_HREF_OPEN);
			/*			server */
			WDVH_sendBodyChunk(userData->reply,userData->server);
			/*          + uri */
			URLencode(uri,uri_enc,WDVH_MAX_URI_LEN);
			WDVH_sendBodyChunk(userData->reply,uri_enc);
			/*		/href */
			WDVH_sendBodyChunk(userData->reply,WDVH_HREF_CLOSE);
			/*		status */
			WDVH_sendBodyChunk(userData->reply,WDVH_STATUS_OPEN);

			/* get the CAPI error code and translate it to the appropriate http status code */
			WDVCAPI_GetErrorType(errorItem,&errorType);
			switch (errorType) {
				case WDVCAPI_ERR_TYPE_UNDEFINED:
				case WDVCAPI_ERR_TYPE_SQL: 
					{
						/* internal server error */
						WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
						WDVH_sendBodyChunk(userData->reply,ERROR_500_STR);
						WDVH_sendBodyChunk(userData->reply,ERROR_500_TEXT);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						switch(errorCode) {
							/* WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION 
							   = Resource Uri and Destination Uri MUST NOT be equal */
							case WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_403_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_403_TEXT);
									break;
								}
								/* WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST 
									= the destination Uri contains at least one collection 
									  that does not exist rigth now */
							case WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_409_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_409_TEXT);
									break;
								}
/* ---> CAPI: WDVCAPI_ERR_CODE_PROPERTY_BEHAVIOR_CONFLICT = die Properties konnten nicht gem‰ﬂ
								                            <propertybehavior>-Befehl kopiert
															werden 
							case WDVCAPI_ERR_CODE_PROPERTY_BEHAVIOR_CONFLICT:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_412_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_412_TEXT);
									break;
								}
*/
							/* WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT 
								= overwrite was verbidden and destination allready exists */
							case WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_412_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_412_TEXT);
									break;
								}
/* ---> CAPI: WDVCAPI_ERR_CODE_RESOURCE_LOCKED = destination ist gesperrt, kein copy
							case WDVCAPI_ERR_CODE_RESOURCE_LOCKED:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_423_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_423_TEXT);
									break;
								}
*/
/* ---> CAPI: WDVCAPI_ERR_CODE_BAD_GATEWAY = destination liegt auf anderem server und dieser 
								             verweigert die Annahme der Resource, kein copy
							case WDVCAPI_ERR_CODE_BAD_GATEWAY:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_502_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_502_TEXT);
									break;
								}
*/
							/* WDVCAPI_ERR_CODE_NO_MEMORY
							   = lack of memory */
							case WDVCAPI_ERR_CODE_NO_MEMORY:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_507_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_507_TEXT);
									break;
								}
							default:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_500_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_500_TEXT);
									break;
								}
						}
						break;
					}
				default:
					{
						WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
						WDVH_sendBodyChunk(userData->reply,ERROR_500_STR);
						WDVH_sendBodyChunk(userData->reply,ERROR_500_TEXT);
						break;
					}
			}
			/*			/status */
			WDVH_sendBodyChunk(userData->reply,WDVH_STATUS_CLOSE);
			/*	/response */
			WDVH_sendBodyChunk(userData->reply,WDVH_RESPONSE_CLOSE);
		}


	}
