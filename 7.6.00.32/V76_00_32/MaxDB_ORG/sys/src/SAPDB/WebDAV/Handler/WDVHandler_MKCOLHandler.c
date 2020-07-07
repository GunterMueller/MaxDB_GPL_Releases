/****************************************************************************

  module      : WDVHandler_MKCOLHandler.c

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

	
	#include "SAPDB/WebDAV/Handler/WDVHandler_MKCOLHandler.h"


	/*------------------------------------------------------------------------
		Function: mkcolCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 mkcolCallCapiFunc(WDVH_Handle		wdvhHandle,
								   WDVCAPI_WDV		pCapiHandle,
								   WDVH_Char		*errmsg)			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
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
		
		if (wdvhHandle==NULL || pCapiHandle==NULL || errmsg==NULL) {
			if (wdvhHandle->wa!=NULL)
				sapdbwa_WriteLogMsg(wdvhHandle->wa, "mkcolCallCapiFunc:Uninitialized Pointers\n");
			if (wdvhHandle->reply!=NULL)
				sendErrorReply(ERROR_500, wdvhHandle->reply, WEBDAV_MKCOL,"");
			return ERROR_500;
		}

		/* get server string for error messages */
		buildServerString(wdvhHandle->request,&server);
		
		success = WDVCAPI_MkCol( pCapiHandle, wdvhHandle->requesturi, wdvhHandle->locktoken);

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
						sp77sprintf(errorText,1000,ERROR_MKCOL_URI,server,(char*)sapdbwa_GetRequestURI(wdvhHandle->request));
						sapdbwa_WriteLogMsg(wdvhHandle->wa, errorText);
						sp77sprintf(errorText,1000,ERROR_MKCOL_500,errorMsg);
						sapdbwa_WriteLogMsg(wdvhHandle->wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						switch(errorCode) {
							case WDVCAPI_ERR_CODE_PARENT_DOESNT_EXIST:
								{
									error = ERROR_409;
									if (errmsg != NULL)
										sp77sprintf(errmsg,1000,"At least one parent does not exist.");	
									break;
								}
							/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							case WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION:
								{
									error = ERROR_403;
									if (errmsg != NULL)
										sp77sprintf(errmsg,1000,"Parent is no collection.");	
									break;
								}
							/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							case WDVCAPI_ERR_CODE_COLLECTION_ALREADY_EXIST:
								{
									error = ERROR_405;
									if (errmsg != NULL)
										sp77sprintf(errmsg,1000,"Collection already exists.");	
									break;
								}
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								{
									error = ERROR_423;
									if (errmsg != NULL)
										sp77sprintf(errmsg,1000,"A parent is locked.");	
									break;
								}
							default:
								{
									error = ERROR_500;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_MKCOL_URI,server,(char*)sapdbwa_GetRequestURI(wdvhHandle->request));
									sapdbwa_WriteLogMsg(wdvhHandle->wa, errorText);
									sp77sprintf(errorText,1000,ERROR_MKCOL_500,errorMsg);
									sapdbwa_WriteLogMsg(wdvhHandle->wa, errorText);
									if (errmsg != NULL)
										sp77sprintf(errmsg,1000,"Internal server error. Please contact your System Adminstrator.");	
									break;
								}
						}
						break;
					}
				default:
					{
						error = ERROR_500;
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						sp77sprintf(errorText,1000,ERROR_MKCOL_URI,server,(char*)sapdbwa_GetRequestURI(wdvhHandle->request));
						sapdbwa_WriteLogMsg(wdvhHandle->wa, errorText);
						sp77sprintf(errorText,1000,ERROR_MKCOL_500,errorMsg);
						sapdbwa_WriteLogMsg(wdvhHandle->wa, errorText);
						if (errmsg != NULL)
							sp77sprintf(errmsg,1000,"Internal server error. Please contact your System Adminstrator.");	
						break;
					}
			}
		} else {
			error = ERROR_201;
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;

	}

	/*------------------------------------------------------------------------
		Function:	mkcolBuildReply
	--------------------------------------------------------------------------*/
	void mkcolBuildReply(WDVH_Handle	wdvhHandle, 
						 sapdbwa_Int2	statuscode,
						 WDVH_Char		*errmsg)			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
	{
		WDVH_Char	contentlength[10];

		if (wdvhHandle==NULL)
			return;
		if (errmsg==NULL) {
			if (wdvhHandle->wa!=NULL)
				sapdbwa_WriteLogMsg(wdvhHandle->wa, "mkcolBuildReply:Uninitialized Pointers\n");
			if (wdvhHandle->reply!=NULL)
				sendErrorReply(ERROR_500, wdvhHandle->reply, WEBDAV_MKCOL,"");
			return;
		}

		switch (statuscode) {
			case ERROR_201:
				sapdbwa_InitHeader(wdvhHandle->reply,
								statuscode,
								NULL,
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/
				sapdbwa_SetHeader( wdvhHandle->reply,
								"Content-Length",
								"0" );
				sapdbwa_SendHeader(wdvhHandle->reply);
				
				break;
			case ERROR_409: 
				sapdbwa_InitHeader(wdvhHandle->reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( wdvhHandle->reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_409));
				sapdbwa_SetHeader( wdvhHandle->reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(wdvhHandle->reply);
				
				sapdbwa_SendBody(wdvhHandle->reply,
								 ERROR_PAGE_409,
								 strlen(ERROR_PAGE_409));
				break; 
			case ERROR_403:								
				sapdbwa_InitHeader(wdvhHandle->reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( wdvhHandle->reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_403)+strlen(errmsg)+strlen(ERROR_PAGE_FOOTER));
				sapdbwa_SetHeader( wdvhHandle->reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(wdvhHandle->reply);
				
				sapdbwa_SendBody(wdvhHandle->reply,
								 ERROR_PAGE_403,
								 0);
				sapdbwa_SendBody(wdvhHandle->reply,
								 errmsg,
								 0);
				sapdbwa_SendBody(wdvhHandle->reply,
								 ERROR_PAGE_FOOTER,
								 0);
				break; 
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */
			case ERROR_405:
				sapdbwa_InitHeader(wdvhHandle->reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( wdvhHandle->reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_405));
				sapdbwa_SetHeader( wdvhHandle->reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(wdvhHandle->reply);
				
				sapdbwa_SendBody(wdvhHandle->reply,
								 ERROR_PAGE_405,
								 strlen(ERROR_PAGE_405));
				break;
			case ERROR_423:
				sapdbwa_InitHeader(wdvhHandle->reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( wdvhHandle->reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_423));
				sapdbwa_SetHeader( wdvhHandle->reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(wdvhHandle->reply);
				
				sapdbwa_SendBody(wdvhHandle->reply,
								 ERROR_PAGE_423,
								 strlen(ERROR_PAGE_423));
				break;
			case ERROR_500:
				sapdbwa_InitHeader(wdvhHandle->reply,
									statuscode,
									"text/html",
									NULL,
									NULL,					/* const char    *LastModified,*/
									NULL,					/* const char    *Expires,*/
									NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( wdvhHandle->reply,
									"DAV",
									"1" );

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_500));
				sapdbwa_SetHeader( wdvhHandle->reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(wdvhHandle->reply);
				
				sapdbwa_SendBody(wdvhHandle->reply,
							 ERROR_PAGE_500,
							 strlen(ERROR_PAGE_500));
				break;
			default:
				sapdbwa_InitHeader(wdvhHandle->reply,
								statuscode,
								NULL,
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/
				sapdbwa_SetHeader( wdvhHandle->reply,
								"Content-Length",
								"0" );
				sapdbwa_SendHeader(wdvhHandle->reply);
				break;
		}
	}
