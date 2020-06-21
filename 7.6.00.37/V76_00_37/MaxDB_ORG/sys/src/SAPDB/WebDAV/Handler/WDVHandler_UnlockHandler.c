/****************************************************************************

  module      : WDVHandler_UnockHandler.c

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

	
	#include "SAPDB/WebDAV/Handler/WDVHandler_UnlockHandler.h"


	/*------------------------------------------------------------------------
		Function: unlockCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 unlockCallCapiFunc(sapdbwa_Handle		wa,
								   WDVCAPI_WDV			pCapiHandle,
   							       sapdbwa_HttpRequestP	request,
						 		   WDVCAPI_URI			requesturi,
								   WDVCAPI_LockIdString	lockIdString)
	{
		sapdbwa_Int2			rc;
		WDV_Bool				success;
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];

		
		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || pCapiHandle==NULL || requesturi==NULL) {
			sapdbwa_WriteLogMsg(wa, "unlockCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}

		success = WDVCAPI_Unlock(pCapiHandle, requesturi, lockIdString);


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
						sp77sprintf(errorText,1000,ERROR_UNLOCK_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_UNLOCK_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						switch(errorCode) {
							case WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST:
								error = ERROR_404;
								break;
							/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124234 */
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								error = ERROR_423;
								break;
							/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124234 */
							default:
								{
									error = ERROR_500;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_UNLOCK_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_UNLOCK_500,errorMsg);
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
						sp77sprintf(errorText,1000,ERROR_UNLOCK_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_UNLOCK_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			error = ERROR_204;
		}

		return error;

	}

	/*------------------------------------------------------------------------
		Function:	unlockBuildReply
	--------------------------------------------------------------------------*/
	void unlockBuildReply(sapdbwa_Int2 statuscode, sapdbwa_HttpReplyP reply )
	{
		WDVH_Char	contentlength[10];

		if (reply==NULL) {
			return;
		}

		switch (statuscode) {
			case ERROR_204:
				sapdbwa_InitHeader(reply,
								statuscode,
								NULL,
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/
				sapdbwa_SetHeader( reply,
								"Content-Length",
								"0");
				sapdbwa_SendHeader(reply);
				break;
			case ERROR_400: 
				sapdbwa_InitHeader(reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_400));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_400,
								 strlen(ERROR_PAGE_400)); 
				break; 
			case ERROR_404:
				sapdbwa_InitHeader(reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_404));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_404,
								 strlen(ERROR_PAGE_404));
				break;
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124234 */
			case ERROR_423:
				sapdbwa_InitHeader(reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_423));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_423,
								 strlen(ERROR_PAGE_423));
				break;
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124234 */
			case ERROR_500:
				sapdbwa_InitHeader(reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_500));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
							 ERROR_PAGE_500,
							 strlen(ERROR_PAGE_500));
				break;
		}
	}
