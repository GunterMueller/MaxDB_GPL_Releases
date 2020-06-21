/****************************************************************************

  module      : WDVHandler_GetHandler.c

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


	#include "SAPDB/WebDAV/Handler/WDVHandler_GetHandler.h"
	/*#include "SAPDB/SAPDBCommon/SAPDB_Types.h"*/

	void outStreamHandler( void *userData, void *buffer, SAPDB_Int4 writeLen );

	void getResourceHref(WDVH_Char *url, WDVH_Char *res, WDVH_Char *result)
	{
		WDVH_Char *pos;

		if (result==NULL)
			return;
		if (url==NULL || res==NULL)
			sp77sprintf(result,WDV_MAX_URI_LEN,"");

		/* concatenate the two strings; 
		   consider trailing and leading slashes */
		if (url[strlen(url)-1] == '/' && res[0] == '/') {
			sp77sprintf(result,WDV_MAX_URI_LEN,"%s",url);
			pos = &result[strlen(url)-2];
			pos = res;
		} else if (url[strlen(url)-1] != '/' && res[0] != '/') {
			sp77sprintf(result,WDV_MAX_URI_LEN,"%s/%s",url,res);
		} else {
			sp77sprintf(result,WDV_MAX_URI_LEN,"%s%s", url, res);
		}
	}

	void getUpHref(WDVH_Char *url, WDVH_Char *result)
	{
		WDVH_Char *pos;
		WDVH_Char temp[WDV_MAX_URI_LEN+1];

		if (result==NULL)
			return;
		if (url==NULL)
			strcpy(result,"");

		if (url[strlen(url)-1] == '/') {
			/* the last '/' to find ist the trailing '/' of the url
			   look for the second last */
			pos = strrchr(url,'/');
			strncpy(temp,url,pos-url);
			pos = strrchr(temp,'/');
			strcpy(result,temp);
			result[pos-temp] = '\0';
		} else {
			/* the last '/' to find ist the trailing '/' of the parent */
			pos = strrchr(url,'/');
			strcpy(result,url);
			result[pos-url] = '\0';
		}
	}

	/*------------------------------------------------------------------------
		Function: GetCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 getCallCapiFunc(sapdbwa_Handle			wa,
   							     sapdbwa_HttpRequestP	request,
   							     sapdbwa_HttpReplyP	    reply,
						 		 WDVCAPI_WDV			pCapiHandle,
								 WDVCAPI_URI			requesturi,
								 WDVCAPI_LockIdString	lockIdString,
								 WDVH_Long				crFrom,				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
								 WDVH_Long				crTo,				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395*/
                                 WDV_Bool              *isDocInXDB)
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
		if (request==NULL || pCapiHandle==NULL || requesturi==NULL || lockIdString==NULL) {
			sapdbwa_WriteLogMsg(wa, "getCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}

        	*isDocInXDB = WDV_False;
  		if (WDVCAPI_GetOpen(pCapiHandle,requesturi,lockIdString,crFrom,crTo, &outStreamHandler, reply, isDocInXDB)) /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
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
						sp77sprintf(errorText,1000,ERROR_GET_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_GET_500,errorMsg);
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
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								error = ERROR_423;
								break;
							case WDVCAPI_ERR_CODE_RANGE_START_OUT_OF_BOUNDS:		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
								error = ERROR_416;									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
								break;												/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
							default:
								{
									error = ERROR_500;
									sp77sprintf(errorText,1000,ERROR_GET_URI,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_GET_500,errorMsg);
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
						sp77sprintf(errorText,1000,ERROR_GET_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_GET_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			if (crFrom != -1 || crTo != -1)		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
				error = ERROR_206;				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
			else								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
				error = ERROR_200;
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	GetBuildReply
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
					   WDVH_Long	crFrom,
					   WDVH_Long	crTo,
                       WDV_Bool     isDocInXDB )
	{
		
		WDVH_GetBuffer				buffer;
        WDVCAPI_ContentLength		readLength;
        WDVH_Long					bufferStartPos;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
		WDVCAPI_ContentLength		contentLength;
		WDVH_Char					strContentLength[100];
		WDVCAPI_ContentTypeString	contentType;
		WDVCAPI_PropertyShortValueBuffer  lastModified;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126839 */
		WDVH_Char					locktoken[100];
		WDVH_Bool					collection;
		WDVH_Char					prefix[WDVH_MAX_URI_LEN+1];
		WDVCAPI_URIString			childResource;
		WDVH_Bool					childFound;
		WDVH_Char					strContentRange[WDVH_MAX_URI_LEN+1];	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
		WDVH_Char					contentlengthstr[10];
		WDVH_Bool					transferEncodingChunked = WDVH_False;
		WDVH_Char					*te_chunked;
		WDVH_Char					chunk[101];

		if (wa==NULL)
			return;
		if (statuscode!=ERROR_416) {
			if (request==NULL || pCapiHandle==NULL || reply==NULL || wdvhHandle==NULL || host==NULL || port==NULL || resourceURI==NULL) {
				sapdbwa_WriteLogMsg(wa, "getBuildReply:Uninitialized Pointers\n");
				sendErrorReply(ERROR_500, reply, WEBDAV_GET,"");
				return;
			}
		} else {
			if (request==NULL || reply==NULL || host==NULL || port==NULL || resourceURI==NULL) {
				sapdbwa_WriteLogMsg(wa, "getBuildReply:Uninitialized Pointers\n");
				sendErrorReply(ERROR_500, reply, WEBDAV_GET,"");
				return;
			}
		}

		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		/* build Body */
		switch (statuscode) {
			case ERROR_200:
			case ERROR_206:
				buffer = wdvhHandle->wdvGetBuffer;							
				if (!WDVCAPI_GetContentType(pCapiHandle,contentType)) {
					/* error handling */
				}
				if (strcmp(contentType,"sapdbwww/directory")==0) {
					/* the resource is a directory -> different output! */
					collection = WDVH_True;
					strcpy(contentType,"text/html");
				} else {
					collection = WDVH_False;
				}
				if (!WDVCAPI_GetLastModified(pCapiHandle, lastModified)) {	
					/* error handling */									
				}															

				/* initialize Response Header */
				sapdbwa_InitHeader(reply,
								statuscode,
								contentType,
								NULL,
								NULL,					/* const char    *LastModified,*/
								NULL,					/* const char    *Expires,*/
								NULL);					/* const char    *Location );*/

				sapdbwa_SetHeader( reply,"Last-Modified",lastModified);			

				sapdbwa_SetHeader ( reply, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");

				if (collection == WDVH_False) {
		            if (isDocInXDB == WDVH_False) {
						if (WDVCAPI_GetContentLength(pCapiHandle,&contentLength)) {
							/* check whether the reply was requested to be sent as Transfer-Encoding chunked or not (independent from the contentlength) */
							te_chunked = (char*)sapdbwa_GetHeader(request,"SAPDB_Chunked");
							if (contentLength >= 0 && (te_chunked == NULL || strcmp(te_chunked,"F")==0)) {
								/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
								if (crFrom == -1 && crTo == -1) {
									sp77sprintf(strContentLength,100,"%ld",contentLength);
								} else {
									/* Content Range is given */
									if (crTo == -1) {
										/* only from is given */
										sp77sprintf(strContentLength,100,"%ld",contentLength-crFrom);
									} else if (crTo >= contentLength) {
										if (crFrom != -1) {
											/* content was read up to th end */
											sp77sprintf(strContentLength,100,"%ld",contentLength-crFrom);
										} else {
											/* only the last "crTo" bytes are read -> crTo > contentLength -> the whole doc was read*/
											sp77sprintf(strContentLength,100,"%ld",contentLength);
										}
									} else {
										if (crFrom != -1) {
											/* only the range between from and to was read */
											sp77sprintf(strContentLength,100,"%ld",crTo-crFrom+1);
										} else {
											/* only the last "crTo" bytes are read */
											sp77sprintf(strContentLength,100,"%ld",crTo);
										}
									}
								}
								/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
								sapdbwa_SetHeader( reply,
												"Content-Length",
												strContentLength );
							} else {
								sapdbwa_SetHeader(reply,
												"Transfer-Encoding",
												"chunked");
								transferEncodingChunked = WDVH_True;
							}
						}			
						/* ContentRange Header */
						if (crFrom != -1 || crTo != -1) {
							if (crFrom == -1) {
								/* has the meaning of: return the last crFrom bytes of the document */
								if (crTo <= contentLength) {
									sp77sprintf(strContentRange,WDVH_MAX_URI_LEN+1,"%ld-%ld/%ld",(WDVH_ULong)contentLength-crTo,contentLength-1,contentLength);
								} else {
									sp77sprintf(strContentRange,WDVH_MAX_URI_LEN+1,"%ld-%ld/%ld",0,contentLength-1,contentLength);
								}
							} else if (crTo == -1) {
								/* has the meaning of: return the document from crFrom up to the end of the document */
								sp77sprintf(strContentRange,WDVH_MAX_URI_LEN+1,"%ld-%ld/%ld",crFrom,contentLength-1,contentLength);
							} else {
								/* has the meaning of: return the document from crFrom-th byte up to the crTo-th byte of the document */
								if (crTo < contentLength) {
									sp77sprintf(strContentRange,WDVH_MAX_URI_LEN+1,"%ld-%ld/%ld",crFrom,crTo,contentLength);
								} else {
									sp77sprintf(strContentRange,WDVH_MAX_URI_LEN+1,"%ld-%ld/%ld",crFrom,contentLength-1,contentLength);
								}
							}
							sapdbwa_SetHeader( reply,
												"Content-Range",
												strContentRange );
						}
		            } else {
						sapdbwa_SetHeader(reply, "Transfer-Encoding",
										"chunked");
						transferEncodingChunked = WDVH_True;
            		}
				}

				sapdbwa_SetHeader( reply,
								"ETag",
								"" );

				sapdbwa_SendHeader(reply);

				if (collection == WDVH_False) {
					/* get content of the resource and send it to the requester */
                    if (isDocInXDB == WDVH_True) {
					    WDVCAPI_GetXDB(pCapiHandle);
                        sapdbwa_SendBodyChunkEnd(reply);  /* Send empty chunk as eof */
                    } else {
						while(WDVCAPI_Get(pCapiHandle,
									(WDVCAPI_Buffer)buffer,
									wdvh_max_get_buffer_len,
									&bufferStartPos,
									&readLength)) {
							if (transferEncodingChunked == WDVH_True) {
								sp77sprintf(chunk,100,"%x%c%c",readLength,0x0D,0x0A);
								sapdbwa_SendBody(reply,chunk,strlen(chunk));
							}
							sapdbwa_SendBody(reply,buffer+bufferStartPos,readLength);
							if (transferEncodingChunked == WDVH_True) {
								sp77sprintf(chunk,100,"%c%c",0x0D,0x0A);
								sapdbwa_SendBody(reply,chunk,strlen(chunk));
							}
							if (readLength != wdvh_max_get_buffer_len) {
								/* eof */
								if (transferEncodingChunked == WDVH_True) {
				                    WDVH_sendBodyChunkEnd(reply);  /* Send empty chunk as eof */
								}
								break;
							}
						}
					}
				} else {
					showCollection(wa,request,reply,pCapiHandle,wdvhHandle,host,port,resourceURI);
				}

				if (WDVCAPI_GetClose(pCapiHandle)) {
				}
				break;
			case ERROR_404: 
				/* initialize Response Header */
				sapdbwa_InitHeader(reply,
								statuscode,
								"text/html",
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/
				sp77sprintf(contentlengthstr,9,"%d",strlen(ERROR_PAGE_404));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlengthstr );
				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
				                 ERROR_PAGE_404,
						         strlen(ERROR_PAGE_404));
				break; 
			/* Begin Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
			case ERROR_416: 
				/* initialize Response Header */
				sapdbwa_InitHeader(reply,
								statuscode,
								"text/html",
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/
				sp77sprintf(contentlengthstr,9,"%d",strlen(ERROR_PAGE_416));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlengthstr );
				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
				                 ERROR_PAGE_416,
						         strlen(ERROR_PAGE_416));
				break; 
			/* End Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
			case ERROR_423: 
				/* initialize Response Header */
				sapdbwa_InitHeader(reply,
								statuscode,
								"text/html",
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/
				sp77sprintf(contentlengthstr,9,"%d",strlen(ERROR_PAGE_423));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlengthstr );
				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
				                 ERROR_PAGE_423,
						         strlen(ERROR_PAGE_423));
				break; 
			case ERROR_501:
				/* initialize Response Header */
				sapdbwa_InitHeader(reply,
								statuscode,
								"text/html",
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/
				sp77sprintf(contentlengthstr,9,"%d",strlen(ERROR_PAGE_501));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlengthstr );
				sapdbwa_SendHeader(reply);
				sapdbwa_SendBody(reply,
				                 ERROR_PAGE_501,
						         strlen(ERROR_PAGE_501));
				break;
		}

	}


	/*------------------------------------------------------------------------
		Function:	outStreamHandler - Callback function for stream handling
	--------------------------------------------------------------------------*/
	void outStreamHandler( void *userData, void *buffer, SAPDB_Int4 writeLen )
    {

		sapdbwa_HttpReplyP reply = (sapdbwa_HttpReplyP) userData;

        sapdbwa_SendBodyChunk(reply, buffer, writeLen);

    }
