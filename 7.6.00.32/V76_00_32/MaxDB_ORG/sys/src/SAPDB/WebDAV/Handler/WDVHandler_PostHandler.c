/****************************************************************************

  module      : WDVHandler_PostHandler.c

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


	#include "SAPDB/WebDAV/Handler/WDVHandler_PostHandler.h"
	#include "SAPDBCommon/SAPDB_string.h"

    void inStreamHandler( void           *userData,
                          void           *buffer,
                          SAPDB_Int4      bufferSize,
                          SAPDB_Int4     *readLen );

    typedef struct st_wdvh_stream_desc {
		sapdbwa_HttpRequestP	request; 
		WDVH_ULong				contentLength;
		WDVH_ULong				bytesRead;
    } WDVHStreamDesc;

	/*------------------------------------------------------------------------
		Function: postCallCapiFunc
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
								  WDVH_Char				*errmsg)				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */
	{
		sapdbwa_Int2			rc;
		WDVH_PutBuffer			buffer;
	    WDVH_Int2				done;
		WDVH_ULong				pos=0;
		WDVH_Bool				success;
		WDVH_Bool				resourceCreated;
		WDVH_ULong				contentLength;
		WDVH_ULong				bytesRead=0;
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
		WDVH_Char				*server;
        WDVHStreamDesc          streamDesc;

		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || pCapiHandle==NULL || wdvh==NULL || requesturi==NULL || locktoken==NULL || putContentType==NULL || errmsg==NULL) {
			sapdbwa_WriteLogMsg(wa, "postCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}

		buffer = wdvh->wdvPutBuffer;

		if (WDVCAPI_PutOpen(pCapiHandle,
							requesturi,
							(WDVCAPI_ContentType)putContentType,
							docClassId,
							async,
							locktoken,
							compress,
                            putToXmldb,
                            &inStreamHandler,
                            &streamDesc,
							&resourceCreated)) {

			/* get content length to determine end of read loop
			   the recv in sapdbwa_ReadBody is a blocking one, so it has to be avoided
			   that one single bytes is read beyond the real existing content length of
			   the body */
            if (sapdbwa_GetContentLength(request)) {
			    contentLength = atoi(sapdbwa_GetContentLength(request));
            } else {
			    contentLength = 0;
            }

            if (putToXmldb == WDVH_True) {
                streamDesc.request = request;
                streamDesc.contentLength = contentLength;
                streamDesc.bytesRead = 0;

                if (!WDVCAPI_PutXDB(pCapiHandle)) {
					/* error handling */
					success = 0;
				} else {
					success = 1;
			    }
				done = 1;
            } else {
			    do {
				    if (contentLength > 0 || sapdbwa_GetContentChunked(request)) {
					    pos = sapdbwa_ReadBody(request,buffer,wdvh_max_put_buffer_len );
                    }
					bytesRead += pos;
					if (bytesRead >= contentLength && contentLength > 0)
						done = 1;
					else if (pos == 0) {
						done = 1;
						strcpy(buffer,"");
					} else
						done = 0;
					if (!WDVCAPI_Put(pCapiHandle,(WDVCAPI_Buffer)buffer,pos)) {
						/* error handling */
						success = 0;
						done = 1;
					} else {
						success = 1;
					}
			    } while(!done);
            }

			if (success == 1) {
				if (!WDVCAPI_PutClose(pCapiHandle)) {
					success = 0;
				}
			}
		} else {
			success = 0;
		}

		/* get server string for error messages */
		buildServerString(request,&server);
		
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
						sp77sprintf(errorText,1000,ERROR_PUT_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_PUT_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						switch(errorCode) {
							case WDVCAPI_ERR_CODE_PARENT_DOESNT_EXIST:
								{
									error = ERROR_409;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_PUT_PARENT_409,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_PUT_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
							/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							case WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION:
								{
									error = ERROR_403;
									sp77sprintf(errmsg,1000,"At least one parent is no collection.");
									break;
								}
							/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							case WDVCAPI_ERR_CODE_NO_PUT_ON_COLLECTION:
								{
									error = ERROR_405;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_PUT_COLLECTION_405,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_PUT_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								{
									error = ERROR_423;
									/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_PUT_LOCKED_423,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_PUT_MESSAGE,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);*/
									/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121446 */
									break;
								}
							default:
								{
									error = ERROR_500;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_PUT_URI,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_PUT_500,errorMsg);
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
						sp77sprintf(errorText,1000,ERROR_MKCOL_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_MKCOL_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			if (resourceCreated) {
				error = ERROR_201;
			} else {
				error = ERROR_204;
			}
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	postBuildReply
	--------------------------------------------------------------------------*/
	void postBuildReply(sapdbwa_Int2 statuscode, sapdbwa_HttpReplyP reply, WDVH_Char* msg)
	{
		char *buf;
		char *url;
		sapdbwa_Bool allocatOK;
		sapdbwa_Int4 length;
		WDVH_Char				contentlength[10];

		if (reply==NULL || msg==NULL) {
			return;
		}

		switch (statuscode) {
			case ERROR_201:
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
			case ERROR_400:								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
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

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_400)+strlen(msg)+strlen(ERROR_PAGE_FOOTER));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_400,
								 0);
				sapdbwa_SendBody(reply,
								 msg,
								 0);
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_FOOTER,
								 0);
				break; 
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */
			case ERROR_403:								
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

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_403)+strlen(msg)+strlen(ERROR_PAGE_FOOTER));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_403,
								 0);
				sapdbwa_SendBody(reply,
								 msg,
								 0);
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_FOOTER,
								 0);
				break; 
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */
			case ERROR_409: 
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

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_409));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_409,
								 strlen(ERROR_PAGE_409));
				break; 
			case ERROR_412: 
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

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_412));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);
				
				sapdbwa_SendBody(reply,
								 ERROR_PAGE_412,
								 strlen(ERROR_PAGE_412));
				break; 
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

	/*------------------------------------------------------------------------
		Function:	postCheckDocumentClass
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 postCheckDocumentClass(sapdbwa_Handle			wa, 
										sapdbwa_HttpRequestP	request, 
										WDVH_Handle				wdvh,
										XMLIMAPI_Id				*id,
										WDVH_Bool				*async)
	{
		sapdbwa_UInt2			statuscode = ERROR_200;
		WDVH_Char				*pDc;
		WDVH_Char				sDcIgnore[2];
		WDVH_Char				*pDcIgnore;
		XMLIMAPI_Id			docClassId;
		XMLIMAPI_Handle		handle;
		XMLIMAPI_ErrorItem		errorItem;
		XMLIMAPI_ErrorType		errorType;
		XMLIMAPI_ErrorCode		errorCode;
		XMLIMAPI_ErrorText		errorMsg;
		WDVH_Char				errorText[1000];
		WDVH_Char				*server;

		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || wdvh==NULL || async == NULL || id==NULL) {
			sapdbwa_WriteLogMsg(wa, "postCheckDocumentClass:Unitialized Parameters");
			return ERROR_500;
		}
		/* get server string for error messages */
		buildServerString(request,&server);

		/* get Document Class Header */ 
		/* (no part of RFC2518) */
		pDc = (char*)sapdbwa_GetHeader(request,"SAPDB_DocumentClass");
		
		if (pDc != NULL) {
			if(strcmp(pDc,"")!=0) {
				/* get Async Indexing Header */ 
				/* (no part of RFC2518) */
				getAsyncHeader(request,async);
				/* first lookup cache */
				if (findDocClassList(wdvh->docClassList,pDc,&docClassId)==WDVH_False) {
					/* not cached yet -> look up modelling info */
					if (!XMLIMAPI_DocClassGetId(wdvh->xmlHandle,pDc,&docClassId)) {
						XMLIMAPI_GetLastError(wdvh->xmlHandle,&errorItem);
						XMLIMAPI_GetErrorType(errorItem,&errorType);
						XMLIMAPI_GetErrorCode(errorItem,&errorCode);
						XMLIMAPI_GetErrorText(errorItem,&errorMsg);
						if (errorCode == XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS) {
							statuscode = ERROR_412;
							sp77sprintf(errorText,1000,ERROR_PUT_DOCCLASS_NOT_EXIST,server,(char*)sapdbwa_GetRequestURI(request),pDc);
							sapdbwa_WriteLogMsg(wa, errorText);
							sp77sprintf(errorText,1000,ERROR_PUT_MESSAGE,errorMsg);
							sapdbwa_WriteLogMsg(wa, errorText);
						}
					} else {
						/* add doc class id to cache */
						addItemDocClassList(wdvh->docClassList,pDc,docClassId);
						SAPDB_memcpy(*id,docClassId,XMLIMAPI_MAX_ID_LEN);
						statuscode = ERROR_200;
					}
				} else {
					SAPDB_memcpy(*id,docClassId,XMLIMAPI_MAX_ID_LEN);
					statuscode = ERROR_200;
				}
			} else {
				WDVCAPI_IdInitValue(docClassId);
				SAPDB_memcpy(*id,docClassId,XMLIMAPI_MAX_ID_LEN);
				*async = WDVH_True;
			}
		} else {
			WDVCAPI_IdInitValue(docClassId);
			SAPDB_memcpy(*id,docClassId,XMLIMAPI_MAX_ID_LEN);
			*async = WDVH_True;
		}
		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return statuscode;
	}

	/*------------------------------------------------------------------------
		Function:	inStreamHandler
	--------------------------------------------------------------------------*/
    void inStreamHandler( void           *userData,
                          void           *buffer,
                          SAPDB_Int4      bufferSize,
                          SAPDB_Int4     *readLen )
    {

        WDVHStreamDesc  *streamDesc = (WDVHStreamDesc*)userData;
        SAPDB_UInt4      copyLen = 0;
        SAPDB_UInt4      currLen = 0;

        *readLen = 0;

        if (!streamDesc) {
            return;
        }

        currLen = streamDesc->contentLength - streamDesc->bytesRead;
        copyLen = (bufferSize > currLen) ? currLen : bufferSize;

        if (copyLen > 0) {
		    *readLen = sapdbwa_ReadBody(streamDesc->request, buffer, copyLen);
		} else {
			*readLen = 0;
		}

        streamDesc->bytesRead += *readLen;

    }
