/****************************************************************************

  module      : WDVHandler_PropfindHandler.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  13:32
  see also    : 

  -------------------------------------------------------------------------


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


	#include "SAPDB/WebDAV/Handler/WDVHandler_PropfindHandler.h"


	/*------------------------------------------------------------------------
		Function: propfindCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 propfindCallCapiFunc(WDVCAPI_WDV				wdv,
									  sapdbwa_Handle			wa,
									  sapdbwa_HttpRequestP		request,
									  WDVCAPI_PropfindHandle	propfind)
	{
		sapdbwa_Int2			rc;
		WDVH_Bool				success;
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
		WDVH_Char				*server;

		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || wdv==NULL || propfind==NULL) {
			sapdbwa_WriteLogMsg(wa, "propfindCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}

		/* then call CAPI Function */
		success = WDVCAPI_Propfind(wdv,propfind);

		/* get server string for error messages */
		buildServerString(request,&server);
		
		if (success==WDV_False) {
			WDVCAPI_GetLastError(wdv,&errorItem);
			WDVCAPI_GetErrorType(errorItem,&errorType);
			switch (errorType) {
				case WDVCAPI_ERR_TYPE_UNDEFINED:
				case WDVCAPI_ERR_TYPE_SQL: 
					{
						/* internal server error */
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						error = ERROR_500;
						sp77sprintf(errorText,1000,ERROR_PROPFIND_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_PROPFIND_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						switch(errorCode) {
							case WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST:
								{
									error = ERROR_404;
									break;
								}
							default:
								{
									error = ERROR_500;
									sp77sprintf(errorText,1000,ERROR_PROPFIND_URI,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_PROPFIND_500,errorMsg);
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
						sp77sprintf(errorText,1000,ERROR_PROPFIND_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_PROPFIND_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			error = ERROR_207;
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	propfindBuildReply
	--------------------------------------------------------------------------*/
	WDVH_Bool propfindBuildReply(sapdbwa_Handle		wa,
						     sapdbwa_Int2 statuscode, 
							sapdbwa_HttpReplyP reply, 
							WDVH_Char *host,
							WDVH_Char *port,
							WDVH_Char *uri,
							WDVCAPI_WDV wdv,
							WDVCAPI_PropfindHandle  propfind,
							WDVH_Bool isPropnameRequest,		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
							WDVH_Bool isEmptyPropertyList,     /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
							WDVH_Char *userAgent,
							WDVH_Char *errormsg)			   /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
	{
		char *buf;
		char *url;
		sapdbwa_Bool								allocatOK;
		sapdbwa_Int4								length;
		WDVCAPI_PropfindNameSpace					propfindNamespace;
		WDVCAPI_PropfindNameSpaceShortcut			propertyNamespaceShortcut;
		WDVCAPI_PropfindPropertyName				propertyName;
		WDVCAPI_PropfindPropertyShortValue			propertyShortValue;
		WDVCAPI_PropertyStatus						propertyStatus;
		WDVH_DynamicString							namespaceList;
		WDVH_DynamicString							rootResource;
		WDVH_Bool									requestedResource = WDVH_True;
		WDVH_Char									*server=NULL;
		WDVCAPI_URI									resourceUri;
		WDVH_Char									uri_enc[WDVH_MAX_URI_LEN+1];
		WDVCAPI_LockHandle							lock;
		WDVH_Char									timeout[50];
		WDVH_Char									locktoken[100];
		WDVCAPI_PropfindNameSpaceShortcut			davPropertyNamespaceShortcut = NULL;
        WDVH_HTTPDateString                         httpDate;
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									contentlength[10];
		WDVH_Bool									isMSWebFolder = WDVH_False;

		if (wa==NULL)
			return WDVH_False;
		if (reply==NULL || wdv==NULL || host==NULL || port==NULL || uri==NULL || propfind==NULL || userAgent==NULL || errormsg==NULL) {
			sapdbwa_WriteLogMsg(wa, "propfindBuildReply:Uninitialized Pointers\n");
			return WDVH_False;
		}

		switch (statuscode) {
			case ERROR_400: {
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
			}
			case ERROR_404: {
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
			}
			case ERROR_409: {										
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

				sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_409_OWN)+strlen(errormsg)+strlen(ERROR_PAGE_FOOTER));
				sapdbwa_SetHeader( reply,
								"Content-Length",
								contentlength);

				sapdbwa_SendHeader(reply);

				sapdbwa_SendBody(reply,ERROR_PAGE_409_OWN,0);		
				sapdbwa_SendBody(reply,errormsg,0);					
				sapdbwa_SendBody(reply,ERROR_PAGE_FOOTER,0);		
				break;												
			}														
			case ERROR_500: {
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
			case ERROR_207: {
				if (strstr(userAgent,"Microsoft Data Access Internet Publishing Provider DAV")!=NULL) {
					/* User-Agent is MS WebFolder -> do not use transfer-encoding chunked! */
					isMSWebFolder = WDVH_True;
				} else {
					isMSWebFolder = WDVH_False;
				}
				
				sapdbwa_InitHeader(reply,
								statuscode,
								"text/xml",
								NULL,
								NULL,								/* const char    *LastModified,*/
								NULL,								/* const char    *Expires,*/
								NULL);								/* const char    *Location );*/

				if (isMSWebFolder == WDVH_False) {
					sapdbwa_SetHeader( reply,
						"Transfer-Encoding",
						"chunked" );
				}

				sapdbwa_SendHeader(reply);

				/* send body header */
				WDVH_sendBodyChunkPropfind(reply,WDVH_XML_HEADER,isMSWebFolder);

				WDVH_sendBodyChunkPropfind(reply,WDVH_MULTI_STATUS_OPEN,isMSWebFolder);

				/* set server prefix */
				getWebDAVPrefix(wa,prefix);

				length = strlen(prefix) + 1;
				WDVHandler_Allocat(sizeof(SAPDB_Char)*length,(sapdbwa_UInt1**) &server,&allocatOK);
				if (allocatOK) {
					sp77sprintf(server,length,"%s",prefix);
				}
				/* build namespace list */
				if (WDVCAPI_PropfindGetFirstNameSpace(wdv,propfind,&propertyNamespaceShortcut,&propfindNamespace)) {
					namespaceList = createStrBuffer(WDVH_MAX_STR_BUFFER_LEN);

					do {
                        /* Find out wich shortcut belongs to DAV: */
                        if (!davPropertyNamespaceShortcut && strcmp(propfindNamespace, "DAV:") == 0) {
                            /* Shortcut found, so we have to memorize shortcut for later use */
                            davPropertyNamespaceShortcut = propertyNamespaceShortcut;
                        }

						if (!appendStrBuffer(namespaceList,WDVH_XMLNS)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_NS_PREFIX)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,propertyNamespaceShortcut)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_EQUAL_SIGN)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_QUOTE)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,propfindNamespace)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_QUOTE)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_SPACE)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
					} while(WDVCAPI_PropfindGetNextNameSpace(wdv,propfind,&propertyNamespaceShortcut,&propfindNamespace));
					/* special namespace for date/time recognition by WebFolder */
					if (!appendStrBuffer(namespaceList,WDVH_XMLNS)) {
						/* error handling */
						sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
						return WDVH_True;
					}
					if (!appendStrBuffer(namespaceList,"dt")) {
						/* error handling */
						sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
						return WDVH_True;
					}
					if (!appendStrBuffer(namespaceList,WDVH_XML_EQUAL_SIGN)) {
						/* error handling */
						sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
						return WDVH_True;
					}
					if (!appendStrBuffer(namespaceList,"'urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/'")) {
						/* error handling */
						sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
						return WDVH_True;
					}
					if (!appendStrBuffer(namespaceList,WDVH_XML_SPACE)) {
						/* error handling */
						sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
						return WDVH_True;
					}
				} else {
					/* error handling */
					sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
					return WDVH_True;
				}
				/* loop at the resultlist provided by propfind function */
				do {
					/* response */
					WDVH_sendBodyChunkPropfind(reply,WDVH_RESPONSE_OPEN,isMSWebFolder);
					WDVH_sendBodyChunkPropfind(reply,getStrBufferContent(namespaceList),isMSWebFolder);
					WDVH_sendBodyChunkPropfind(reply,WDVH_XML_CLOSE_TAG,isMSWebFolder);
								
					/*		href */
					WDVH_sendBodyChunkPropfind(reply,WDVH_HREF_OPEN,isMSWebFolder);
					WDVH_sendBodyChunkPropfind(reply,server,isMSWebFolder);
					if (requestedResource) {
						URLencode(uri,uri_enc,WDVH_MAX_URI_LEN);
						WDVH_sendBodyChunkPropfind(reply,uri_enc,isMSWebFolder);
						requestedResource = WDVH_False;
					} else {
						URLencode(resourceUri,uri_enc,WDVH_MAX_URI_LEN);
						WDVH_sendBodyChunkPropfind(reply,uri_enc,isMSWebFolder);
					}
					WDVH_sendBodyChunkPropfind(reply,WDVH_HREF_CLOSE,isMSWebFolder);

					/* loop at properties */
					if (!isEmptyPropertyList) {
						if (WDVCAPI_PropfindGetFirstProperty(wdv,propfind)) {
						} else {
							/* clean up */
							destroyStrBuffer(namespaceList);
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
							return WDVH_True;
						}
					}
					do {
						/*		propstat */
						WDVH_sendBodyChunkPropfind(reply,WDVH_PROPSTAT_OPEN,isMSWebFolder);
						if (!isEmptyPropertyList) {
							/*			prop */
							WDVH_sendBodyChunkPropfind(reply,WDVH_PROP_OPEN,isMSWebFolder);

							/*				property name */
							if (WDVCAPI_PropfindGetPropertyName(wdv,propfind,&propertyName )) {
								WDVH_sendBodyChunkPropfind(reply,WDVH_XML_OPEN_TAG,isMSWebFolder);
								WDVH_sendBodyChunkPropfind(reply,WDVH_XML_NS_PREFIX,isMSWebFolder);
								if (WDVCAPI_PropfindGetPropertyNameSpaceShortcut(wdv,propfind,&propertyNamespaceShortcut)) {
									WDVH_sendBodyChunkPropfind(reply,propertyNamespaceShortcut,isMSWebFolder);
								} else {
									/* clean up */
									destroyStrBuffer(namespaceList);
									/* error handling */
									sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
									return WDVH_True;
								}
								WDVH_sendBodyChunkPropfind(reply,WDVH_XML_COLON,isMSWebFolder);
								/* Property getlastmodified has to be handled especially */
								if (strcmp(propertyName,WDVH_XML_GETLASTMODIFIED) == 0 &&
									strcmp(propertyNamespaceShortcut, davPropertyNamespaceShortcut) == 0) {
									/* give a special attribute for the date/time format in use in order for
									MS WebFolder to recognize the getlastmodified date */
									WDVH_sendBodyChunkPropfind(reply,propertyName,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply," ",isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_GETLASTMODIFIED_ATTR,isMSWebFolder);
								/* Property creationdate has to be handled especially */
								} else if (strcmp(propertyName,WDVH_XML_CREATIONDATE) == 0 &&
									strcmp(propertyNamespaceShortcut, davPropertyNamespaceShortcut) == 0) {
									/* give a special attribute for the date/time format in use in order for
									MS WebFolder to recognize the getlastmodified date */
									WDVH_sendBodyChunkPropfind(reply,propertyName,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply," ",isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_CREATIONDATE_ATTR,isMSWebFolder);
								} else {
									WDVH_sendBodyChunkPropfind(reply,propertyName,isMSWebFolder);
								}
								if (isPropnameRequest == WDVH_False) {					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_CLOSE_TAG,isMSWebFolder);
									/*					property value */
									if (WDVCAPI_PropfindGetPropertyShortValue(wdv,propfind,&propertyShortValue)) {
										/* Property getlastmodified has to be handled especially */
										if (strcmp(propertyName,WDVH_XML_GETLASTMODIFIED) == 0 &&
											strcmp(propertyNamespaceShortcut, davPropertyNamespaceShortcut) == 0) {

											/* Convert timestamp to HTTP date RFC 2616 Ch: 3.3.1 */
											/* Date format is RFC 822, updated by RFC 1123       */
											propertyToHTTPDate(propertyShortValue, httpDate);

											/* Send converted property */
											WDVH_sendBodyChunkPropfind(reply, httpDate,isMSWebFolder);

										} else if (strcmp(propertyName,WDVH_XML_CREATIONDATE) == 0 &&
											strcmp(propertyNamespaceShortcut, davPropertyNamespaceShortcut) == 0) {

											/* Convert timestamp to HTTP date RFC 2616 Ch: 3.3.1 */
											/* Date format is RFC 822, updated by RFC 1123       */
											propertyToHTTPDate(propertyShortValue, httpDate);

											/* Send converted property */
											WDVH_sendBodyChunkPropfind(reply, httpDate,isMSWebFolder);

										/* property resourcetype has to written as xml tag */
										} else if (strcmp(propertyName,WDVH_XML_RESOURCETYPE)==0) {
											if (strcmp(propertyShortValue,"collection")==0) {
												WDVH_sendBodyChunkPropfind(reply,WDVH_XML_OPEN_TAG,isMSWebFolder);
												WDVH_sendBodyChunkPropfind(reply,WDVH_DEFAULT_NS,isMSWebFolder);
												WDVH_sendBodyChunkPropfind(reply,propertyShortValue,isMSWebFolder);
												WDVH_sendBodyChunkPropfind(reply,WDVH_XML_SLASH,isMSWebFolder);
												WDVH_sendBodyChunkPropfind(reply,WDVH_XML_CLOSE_TAG,isMSWebFolder);
											} else if (strcmp(propertyShortValue,"null resource")==0) {
												WDVH_sendBodyChunkPropfind(reply,propertyShortValue,isMSWebFolder);
											}
										/* property lockdiscovery has to collect additional data */
										} else if (strcmp(propertyName,WDVH_XML_LOCKDISCOVERY)==0) {
											if (WDVCAPI_IdStringIsInitialValue(propertyShortValue)) {
		/*										WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKDISCOVERY_EMPTY);
		*/									} else {
												WDVCAPI_PropfindGetLockDescription(wdv,
																				propfind,
																				propertyShortValue,
																				&lock);
												/* there is a lock on this resource -> get info and
												merge data into the reply */
												/*			<D:activelock> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_ACTIVELOCK_OPEN,isMSWebFolder);
												/*				<D:locktype> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKTYPE_OPEN,isMSWebFolder);
												/*					value (get locktype from CAPI) */
												if (lock->type == WDVCAPI_LOCK_TYPE_WRITE) {
													WDVH_sendBodyChunkPropfind(reply,WDVH_WRITE_EMPTY,isMSWebFolder);
												}
												/*				</D:locktype> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKTYPE_CLOSE,isMSWebFolder);
												/*				<D:lockscope> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKSCOPE_OPEN,isMSWebFolder);
												/*					value (get lockscope from CAPI) */
												if (lock->scope == WDVCAPI_LOCK_SCOPE_EXCLUSIVE) {
													WDVH_sendBodyChunkPropfind(reply,WDVH_EXCLUSIVE_EMPTY,isMSWebFolder);
												} else if (lock->scope == WDVH_LOCKSCOPE_SHARED) {
													WDVH_sendBodyChunkPropfind(reply,WDVH_SHARED_EMPTY,isMSWebFolder);
												}
												/*				</D:lockscope> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKSCOPE_CLOSE,isMSWebFolder);
												/*				<D:depth> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_DEPTH_OPEN,isMSWebFolder);
												/*					value */
												if (lock->depth == WDVCAPI_DEPTH_0) {
													WDVH_sendBodyChunkPropfind(reply,WDVH_DEPTH_0,isMSWebFolder);
												} else if (lock->depth == WDVCAPI_DEPTH_INFINITE) {
													WDVH_sendBodyChunkPropfind(reply,WDVH_DEPTH_INFINITY,isMSWebFolder);
												}
												/*				</D:depth> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_DEPTH_CLOSE,isMSWebFolder);
												/*				<D:owner> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_OWNER_OPEN,isMSWebFolder);
												/*					value */
												if (strlen(lock->owner)>0) 
													WDVH_sendBodyChunkPropfind(reply,lock->owner,isMSWebFolder);
												/*				</D:owner> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_OWNER_CLOSE,isMSWebFolder);
												/*				<D:timeout> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_TIMEOUT_OPEN,isMSWebFolder);
												/*					value */
												sp77sprintf(timeout,50,"%ld",lock->timeout);
												WDVH_sendBodyChunkPropfind(reply,timeout,isMSWebFolder);
												/*				</D:timeout> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_TIMEOUT_CLOSE,isMSWebFolder);
												/*				<D:locktoken> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKTOKEN_OPEN,isMSWebFolder);
												/*					<D:href> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_HREF_OPEN,isMSWebFolder);
												/*						value */
												sp77sprintf(locktoken,100,"saplocktoken:%s",propertyShortValue);
												WDVH_sendBodyChunkPropfind(reply,locktoken,isMSWebFolder);
												/*					</D:href> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_HREF_CLOSE,isMSWebFolder);
												/*				</D:locktoken> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKTOKEN_CLOSE,isMSWebFolder);
												/*			</D:activelock> */
												WDVH_sendBodyChunkPropfind(reply,WDVH_ACTIVELOCK_CLOSE,isMSWebFolder);
											}
										/* property supportedlock has to build a xml structure */
										} else if (strcmp(propertyName,WDVH_XML_SUPPORTEDLOCK)==0) {
											/*			<D:lockentry> */
											WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKENTRY_OPEN,isMSWebFolder);
											/*				<D:locktype> */
											WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKTYPE_OPEN,isMSWebFolder);
											/*					value (get locktype from CAPI) */
											WDVH_sendBodyChunkPropfind(reply,WDVH_WRITE_EMPTY,isMSWebFolder);
											/*				</D:locktype> */
											WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKTYPE_CLOSE,isMSWebFolder);
											/*				<D:lockscope> */
											WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKSCOPE_OPEN,isMSWebFolder);
											/*					value (get lockscope from CAPI) */
											WDVH_sendBodyChunkPropfind(reply,WDVH_EXCLUSIVE_EMPTY,isMSWebFolder);
											/*				</D:lockscope> */
											WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKSCOPE_CLOSE,isMSWebFolder);
											/*			</D:lockentry> */
											WDVH_sendBodyChunkPropfind(reply,WDVH_LOCKENTRY_CLOSE,isMSWebFolder);
										} else {
											WDVH_sendBodyChunkPropfind(reply,propertyShortValue,isMSWebFolder);
										}
									} else {
										/* clean up */
										destroyStrBuffer(namespaceList);
										/* error handling */
										sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
										return WDVH_True;
									}
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_OPEN_TAG,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_SLASH,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_NS_PREFIX,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,propertyNamespaceShortcut,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_COLON,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,propertyName,isMSWebFolder);
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_CLOSE_TAG,isMSWebFolder);
								} else {														/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_SLASH,isMSWebFolder);					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
									WDVH_sendBodyChunkPropfind(reply,WDVH_XML_CLOSE_TAG,isMSWebFolder);				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
								}																/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
								/*				/property name */
							} else {
								/* clean up */
								destroyStrBuffer(namespaceList);
								/* error handling */
								sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
								return WDVH_True;
							}
							/*			/prop */
							WDVH_sendBodyChunkPropfind(reply,WDVH_PROP_CLOSE,isMSWebFolder);

							/*			status */
							WDVH_sendBodyChunkPropfind(reply,WDVH_STATUS_OPEN,isMSWebFolder);
							if (WDVCAPI_PropfindGetPropertyStatus(wdv,propfind,&propertyStatus)) {
								if (propertyStatus == WDVCAPI_PROPERTY_STATUS_OK) {
									WDVH_sendBodyChunkPropfind(reply,WDVH_STATUS_OK,isMSWebFolder);
								} else if (propertyStatus == WDVCAPI_PROPERTY_STATUS_FAILED) {
									WDVH_sendBodyChunkPropfind(reply,WDVH_STATUS_FAILED,isMSWebFolder);
								}
							} else {
								/* error handling */
								sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
								return WDVH_True;
							}

							/*			/status */
							WDVH_sendBodyChunkPropfind(reply,WDVH_STATUS_CLOSE,isMSWebFolder);
						}
						/*		/propstat */
						WDVH_sendBodyChunkPropfind(reply,WDVH_PROPSTAT_CLOSE,isMSWebFolder);
					} while(WDVCAPI_PropfindGetNextProperty(wdv,propfind));

					/*	/response */
					WDVH_sendBodyChunkPropfind(reply,WDVH_RESPONSE_CLOSE,isMSWebFolder);


				} while(WDVCAPI_PropfindGetNextURI(wdv,propfind,&resourceUri));

				WDVH_sendBodyChunkPropfind(reply,WDVH_MULTI_STATUS_CLOSE,isMSWebFolder);
                /* Send empty chunk as eof */
				if (isMSWebFolder == WDVH_False)
					WDVH_sendBodyChunkEnd(reply);

				/* clean up */
				destroyStrBuffer(namespaceList);
			}
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	propfindParseRequestBody
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 propfindParseRequestBody(sapdbwa_Handle		wa,
										  WDVCAPI_WDV wdv,
										  WDVCAPI_PropfindHandle *propfind,
										  sapdbwa_HttpRequestP request,
										  WDVCAPI_Depth				depth,
										  WDVH_Bool *isPropnameRequest,				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
										  WDVH_Bool *isEmptyPropertyList,			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
										  WDVH_Char					*errormsg)		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
	{

	    WDVH_Int2					done;
		WDVH_Int2					pos;
		WDVH_XmlNamespaceList		defaultNamespaceList;
		WDVH_XmlPropfindParserState	parserState;
		WDVH_XmlUserData			newUserData;
		WDVH_Error					error = ERROR_200;
		WDVH_UInt4					contentLength = 0;
		WDVH_UInt4					bytesRead=0;
		WDVH_Char					errorText[1000];
		WDVH_Char					requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Char					*host;
		WDVH_Char					*port;
/*		WDVH_XmlBuffer				buff1;									http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
/*		WDVH_XmlBuffer				buff2;									http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
		WDVH_XmlBuffer				currentBuffer;						/*	http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
/*		WDVH_XmlBufferRef			previousBuffer;							http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
/*		WDVH_XmlBufferRef			tempBuffer;								http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
/*		WDVH_UInt4					bufferChunks;							http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
		XML_Parser					p;
		WDVH_Char					*server;
		WDVCAPI_ErrorText			errorMsg;							/* 	http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */ 	

		if (wa==NULL)
			return ERROR_500;
		if (wdv==NULL || propfind==NULL || request==NULL || isPropnameRequest==NULL || isEmptyPropertyList==NULL || errormsg==NULL) {
			sapdbwa_WriteLogMsg(wa, "propfindParseRequestBody:Uninitialized Pointers\n");
			return ERROR_500;
		}

		/* get server string for error messages */
		buildServerString(request,&server);
		
		/* Initialization */
		p =  XML_ParserCreateNS("UTF-8",WDVH_NS_SEPARATOR);
		if (!p) {
			return ERROR_500;
		}
/*		bufferChunks = 0;																	http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*		currentBuffer = buff2;																http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*		previousBuffer = buff1;																http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		strcpy(currentBuffer,"");
/*		strcpy(previousBuffer,"");															http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);

		xmlParserCreateNamespaceList(p,&defaultNamespaceList); 
		/* Attention: previousBuffer and currentBuffer are swaped later on, so give 
		              previousBuffer as currentBuffer to user data! */
		xmlParserCreateUserData(p,currentBuffer,defaultNamespaceList,wdv,&newUserData);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		xmlParserUserDataSetDepth(newUserData,depth);
		xmlParserUserDataSetWaHandle(newUserData,wa);
		xmlParserUserDataSetRequestUri(newUserData,requestURI);
		xmlParserUserDataSetRequest(newUserData,request);
		xmlParserUserDataSetPropfindHandle(newUserData,propfind);
		xmlParserCreatePropfindParserState(&parserState);
		xmlParserUserDataSetParserState(newUserData,(WDVH_XmlParserState)parserState);
		XML_SetUserData(p,(void*)newUserData);
		XML_SetElementHandler(p, 
							  propfindXmlTagStartHandler,
							  propfindXmlTagEndHandler);
        /*
		XML_SetNamespaceDeclHandler(p,
									propfindXmlStartNamespaceHandler,
									propfindXmlEndNamespaceHandler);
        */
		XML_SetParamEntityParsing(p,XML_PARAM_ENTITY_PARSING_ALWAYS);
		
		/* loop at all chunks of the incomming XML document and parse the XML */
		do {
/*			bufferChunks++;																			http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*			tempBuffer = previousBuffer;															http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*			previousBuffer = currentBuffer;															http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*			currentBuffer = tempBuffer;																http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
/*			xmlParserUserDataSetParseBuffer(newUserData,currentBuffer,previousBuffer,bufferChunks);	http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* get content length to determine end of read loop
			   the recv in sapdbwa_ReadBody is a blocking one, so it has to be avoided
			   that one single bytes is read beyond the real existing content length of
			   the body */
			if (sapdbwa_GetContentLength(request)) {
				contentLength = atoi(sapdbwa_GetContentLength(request));
			}
			if (contentLength > 0) {
				pos = sapdbwa_ReadBody( request, currentBuffer, WDVH_MAX_XML_BUFFER_LEN );
				if (strcmp(currentBuffer,"")==0) {
					strcpy(currentBuffer,WDVH_XML_ALLPROP_BODY);
					pos = strlen(WDVH_XML_ALLPROP_BODY);
	/*				sp77sprintf(errorText,1000,ERROR_PROPFIND_URI,server,(char*)sapdbwa_GetRequestURI(request));
					sapdbwa_WriteLogMsg(wa, errorText);
					sp77sprintf(errorText,1000,ERROR_PROPFIND_MISSING_BODY);
					sapdbwa_WriteLogMsg(wa, errorText);
					error = ERROR_400;
					done = 1;
	*/			}
				bytesRead += pos;
				if (bytesRead >= contentLength)
					done = 1;
				else if (pos == 0) {
					done = 1;
					strcpy(currentBuffer,"");
				} else
					done = 0;
			} else {
				strcpy(currentBuffer,WDVH_XML_ALLPROP_BODY);
				pos = strlen(WDVH_XML_ALLPROP_BODY);
				done = 1;
			}
			if (strcmp(currentBuffer,"")!=0) {
				if (! XML_Parse(p, currentBuffer, pos, done)) {
					pos = XML_GetCurrentByteIndex(p);
					sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_1,server,(char*)sapdbwa_GetRequestURI(request));
					sapdbwa_WriteLogMsg(wa, errorText);
					sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_2,XML_GetCurrentLineNumber(p),XML_ErrorString(XML_GetErrorCode(p)));
					sapdbwa_WriteLogMsg(wa, errorText);
					error = ERROR_400;
					done = 1;
				}
			} else 
				done = 1;
		} while(!done);

		if (error != ERROR_400) {
			parserState = xmlParserUserDataGetPropfindParserState(newUserData);
			if (parserState->error == WDVH_XML_PARSER_OK)
				error = ERROR_200;
			else if (parserState->error == ERROR_409) {					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
				error = ERROR_409;										/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
				WDVCAPI_GetErrorText(newUserData->error,&errorMsg);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
				strcpy(errormsg,errorMsg);								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
			}
			else {
				error = ERROR_400;
				sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_1,server,(char*)sapdbwa_GetRequestURI(request));
				sapdbwa_WriteLogMsg(wa, errorText);
			}
			*isPropnameRequest = newUserData->isPropnameRequest;
		}

		
		/* Clean Up */
		xmlParserDestroyNamespaceList(p,newUserData->nslist);
		xmlParserDestroyPropfindParserState((WDVH_XmlPropfindParserState)newUserData->parserState);
		*isEmptyPropertyList = newUserData->propfindEmptyPropertyList; /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
		xmlParserDestroyUserData(newUserData);
		XML_ParserFree(p);
		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	propfindXmlTagStartHandler
	--------------------------------------------------------------------------*/
	void propfindXmlTagStartHandler(void *data, const char *el, const char **attr) {

		/* DTD Propfind Body
			<!ELEMENT propfind (allprop | propname | prop) >
			<!ELEMENT allprop EMPTY >
			<!ELEMENT propname EMPTY >
			<!ELEMENT prop ANY>

			- allprop and propname must only appear as <allprop/> and <propname/>
			<allprop></allprop> and <propname></propname> are syntax errors! -> bad request
		*/
		WDVH_Char						*ns;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		WDVH_Char						*tag;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		WDVH_Bool						allocatOK;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		WDVH_XmlPropfindParserState		parserState;
		WDVCAPI_WDV						capiWdv;
		WDVH_Char						errorText[1000];
		WDVH_XmlUserData				userData;
		XML_Parser						p;
		WDVH_Char						*server;

		if (data==NULL || el==NULL || attr==NULL)
			return;

		userData = (WDVH_XmlUserData)data;
		p = xmlParserUserDataGetParser(userData);
		parserState = xmlParserUserDataGetPropfindParserState(userData);
		capiWdv = xmlParserUserDataGetWDV(userData);

		/* get server string for error messages */
		buildServerString(userData->request,&server);

		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124138 */
		/* allocate tag and ns depending on the length of el */
		WDVHandler_Allocat(sizeof(SAPDB_Char)*strlen(el),(sapdbwa_UInt1**) &tag,&allocatOK);
		if (!allocatOK) {
			parserState->error = WDVH_XML_PARSER_MEMORY_ERROR;
			return;
		}
		WDVHandler_Allocat(sizeof(SAPDB_Char)*strlen(el),(sapdbwa_UInt1**) &ns,&allocatOK);
		if (!allocatOK) {
			parserState->error = WDVH_XML_PARSER_MEMORY_ERROR;
			return;
		}
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124138 */

		xmlParserCutNamespace(el,ns,tag,WDVH_NS_SEPARATOR);
		/* check wether or not the tag contains a namespace abbrev. if so the request must
		   be rejected due to the missing declaration of the namespace for the discoverd
		   namespace abbrev. */
		if(xmlParserCheckNamespaceAbbrev(tag)) {
			parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
		}

		/* parsing using kind of finite automata. please refer to technical documentation
		   for further information */

		if (parserState->error == WDVH_XML_PARSER_OK) {

			if (parserState->depth == 0 &&
				parserState->state == WDVH_PROPFIND_DFA_STATE_1) {
				if (strcmp(tag,WDVH_XML_PROPFIND_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPFIND_DFA_STATE_2;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			} else {

				if (parserState->depth == 1 &&
					parserState->state == WDVH_PROPFIND_DFA_STATE_2) {
					if (strcmp(tag,WDVH_XML_ALLPROP)==0) {
						/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 
						if(xmlParserCheckEmptyTag(p,userData,WDVH_EMPTY_TAG_ALLPROP)) {
						
						if(!WDVCAPI_PropfindCreate(capiWdv,
													userData->requestUri,
													userData->depth,
													WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL,
													xmlParserUserDataGetPropfindHandle(userData))) {
							sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_1,(char*)sapdbwa_GetHeader(userData->request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(userData->request));
							sapdbwa_WriteLogMsg(userData->wa, errorText);
							sp77sprintf(errorText,1000,ERROR_PROPFIND_500,"Invalid Handle");
							sapdbwa_WriteLogMsg(userData->wa, errorText);
							parserState->error = ERROR_500;
						}*/
						parserState->depth++;
						parserState->state = WDVH_PROPFIND_DFA_STATE_6;
						/*} else {
							parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
						}*/
						/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
						userData->propfindEmptyPropertyList = WDVH_False; /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
					} else if (strcmp(tag,WDVH_XML_PROPNAME)==0) {
						/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 
						if(xmlParserCheckEmptyTag(p,userData,WDVH_EMPTY_TAG_PROPNAME)) {
						if(!WDVCAPI_PropfindCreate(capiWdv,
													userData->requestUri,
													userData->depth,
													WDVCAPI_PROPFIND_TYPE_PROPFIND_NAME,
													xmlParserUserDataGetPropfindHandle(userData))) {
							sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_1,(char*)sapdbwa_GetHeader(userData->request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(userData->request));
							sapdbwa_WriteLogMsg(userData->wa, errorText);
							sp77sprintf(errorText,1000,ERROR_PROPFIND_500,"Invalid Handle");
							sapdbwa_WriteLogMsg(userData->wa, errorText);
							parserState->error = ERROR_500;
						}*/
						parserState->depth++;
						parserState->state = WDVH_PROPFIND_DFA_STATE_7;
						/*} else {
							parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
						}*/
						/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
						userData->propfindEmptyPropertyList = WDVH_False; /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
					} else if (strcmp(tag,WDVH_XML_PROP_OPEN)==0) {
						if(!WDVCAPI_PropfindCreate(capiWdv,
												   userData->requestUri,
												   userData->depth,
												   WDVCAPI_PROPFIND_TYPE_PROPFIND,
												   xmlParserUserDataGetPropfindHandle(userData))) {
							sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_1,(char*)sapdbwa_GetHeader(userData->request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(userData->request));
							sapdbwa_WriteLogMsg(userData->wa, errorText);
							sp77sprintf(errorText,1000,ERROR_PROPFIND_500,"Invalid Handle");
							sapdbwa_WriteLogMsg(userData->wa, errorText);
							parserState->error = ERROR_500;
						}
						parserState->depth++;
						parserState->state = WDVH_PROPFIND_DFA_STATE_3;
					} else {
						/* unknown tags */
					}
				} else {

					if (parserState->depth == 2 &&
						parserState->state == WDVH_PROPFIND_DFA_STATE_3) {
						/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123337 
						sp77sprintf(cmptag,strlen(tag)+3,"%s/>",tag);
						if(xmlParserCheckEmptyTag(p,userData,cmptag)) {*/
							/* property begin 
							strcpy(capiNamespace,ns);
							strcpy(capiPropertyName,tag);
							capiWdv = xmlParserUserDataGetWDV(userData);
							WDVCAPI_PropfindAddPropertyToPropertyList(capiWdv,
																	  *(userData->propfindHandle),
																	  capiNamespace,
																	  capiPropertyName );
							userData->propfindEmptyPropertyList = WDVH_False;  http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 
						} else {
							parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
						}*/
						parserState->depth++;
						parserState->state = WDVH_PROPFIND_DFA_STATE_8;
						/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123337 */
					} else {

						if (parserState->depth == 1 &&
							parserState->state == WDVH_PROPFIND_DFA_STATE_4) {
							if (strcmp(tag,WDVH_XML_ALLPROP)==0 ||
								strcmp(tag,WDVH_XML_PROPNAME)==0 ||
								strcmp(tag,WDVH_XML_PROP_OPEN)==0) {
								parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
							} else {
								/* unknown tags */
							}
						} else {

							if (parserState->depth == 0 &&
								parserState->state == WDVH_PROPFIND_DFA_STATE_5) {
								parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
							}
						}
					}
				}
			}
		}
		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}

	}  /* End of start handler */

	/*------------------------------------------------------------------------
		Function:	propfindXmlTagEndHandler
	--------------------------------------------------------------------------*/
	void propfindXmlTagEndHandler(void *data, const char *el) {
		
		WDVH_Char						*ns;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		WDVH_Char						*tag;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		WDVH_Bool						allocatOK;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		WDVH_XmlPropfindParserState		parserState;
		WDVH_Char						capiNamespace[WDV_MAX_NAME_SPACE_LEN+1];				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123337 */
        WDVH_Char						capiPropertyName[WDV_MAX_PROPERTY_NAME_PREFIX_LEN+1];	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123337 */
		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
		WDVCAPI_WDV						capiWdv;
		WDVH_Char						errorText[1000];
		WDVH_XmlUserData				userData;
		XML_Parser						p;
		WDVH_Char						*server;

		if (data==NULL || el==NULL)
			return;

		userData = (WDVH_XmlUserData)data;
		capiWdv = xmlParserUserDataGetWDV(userData);
		parserState = xmlParserUserDataGetPropfindParserState(userData);
		/* get server string for error messages */
		buildServerString(userData->request,&server);
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */

		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		/* allocate tag and ns depending on the length of el */
		WDVHandler_Allocat(sizeof(SAPDB_Char)*strlen(el),(sapdbwa_UInt1**) &tag,&allocatOK);
		if (!allocatOK) {
			parserState->error = WDVH_XML_PARSER_MEMORY_ERROR;
			return;
		}
		WDVHandler_Allocat(sizeof(SAPDB_Char)*strlen(el),(sapdbwa_UInt1**) &ns,&allocatOK);
		if (!allocatOK) {
			parserState->error = WDVH_XML_PARSER_MEMORY_ERROR;
			return;
		}
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */

		xmlParserCutNamespace(el,ns,tag,WDVH_NS_SEPARATOR);

		if (parserState->depth == 2 &&
			parserState->state == WDVH_PROPFIND_DFA_STATE_3) {
			if (strcmp(tag,WDVH_XML_PROP_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPFIND_DFA_STATE_4;
			} else {
				/* property end */
			}
		} else {

			if (parserState->depth == 1 &&
				parserState->state == WDVH_PROPFIND_DFA_STATE_4) {
				if (strcmp(tag,WDVH_XML_PROPFIND_CLOSE)==0) {
					parserState->depth--;
					parserState->state = WDVH_PROPFIND_DFA_STATE_5;
				}
			} else {

				if (parserState->state != WDVH_PROPFIND_DFA_STATE_4 &&
					strcmp(tag,WDVH_XML_PROPFIND_CLOSE)==0) {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				} else {

					if (parserState->state != WDVH_PROPFIND_DFA_STATE_3 &&
						strcmp(tag,WDVH_XML_PROP_CLOSE)==0) {
						parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
					}
				}
			}
		}
		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
		if (parserState->depth == 2 &&
			parserState->state == WDVH_PROPFIND_DFA_STATE_6) {
			if (strcmp(tag,WDVH_XML_ALLPROP)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPFIND_DFA_STATE_4;
				if(!WDVCAPI_PropfindCreate(capiWdv,
											userData->requestUri,
											userData->depth,
											WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL,
											xmlParserUserDataGetPropfindHandle(userData))) {
					sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_1,(char*)sapdbwa_GetHeader(userData->request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(userData->request));
					sapdbwa_WriteLogMsg(userData->wa, errorText);
					sp77sprintf(errorText,1000,ERROR_PROPFIND_500,"Invalid Handle");
					sapdbwa_WriteLogMsg(userData->wa, errorText);
					parserState->error = ERROR_500;
				}
			}
		}

		if (parserState->depth == 2 &&
			parserState->state == WDVH_PROPFIND_DFA_STATE_7) {
			if (strcmp(tag,WDVH_XML_PROPNAME)==0) {
				if(!WDVCAPI_PropfindCreate(capiWdv,
											userData->requestUri,
											userData->depth,
											WDVCAPI_PROPFIND_TYPE_PROPFIND_NAME,
											xmlParserUserDataGetPropfindHandle(userData))) {
					sp77sprintf(errorText,1000,ERROR_PROPFIND_PARSE_1,(char*)sapdbwa_GetHeader(userData->request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(userData->request));
					sapdbwa_WriteLogMsg(userData->wa, errorText);
					sp77sprintf(errorText,1000,ERROR_PROPFIND_500,"Invalid Handle");
					sapdbwa_WriteLogMsg(userData->wa, errorText);
					parserState->error = ERROR_500;
				}
				userData->isPropnameRequest = WDVH_True;
				parserState->depth--;
				parserState->state = WDVH_PROPFIND_DFA_STATE_4;
			}
		}
		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123337 */
		if (parserState->depth == 3 &&
			parserState->state == WDVH_PROPFIND_DFA_STATE_8) {
			parserState->depth--;
			parserState->state = WDVH_PROPFIND_DFA_STATE_3;
			/* property end */
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
			/* check whether the property name is within the limits of  */
			if (strlen(tag)>WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)<=WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				tag[WDVH_MAX_PROPERTY_NAME_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errorText,1000,WDVH_ERR_TEXT_PROPERTY_NAME_TOO_LONG,WDVH_MAX_PROPERTY_NAME_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_NAME_TOO_LONG,errorText,NULL,0);
				parserState->error = ERROR_409;
			}
			/* check whether the namespace name is within the limits of  */
			if (strlen(tag)<=WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)>WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				ns[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errorText,1000,WDVH_ERR_TEXT_NAMESPACE_NAME_TOO_LONG,WDVH_MAX_NAME_SPACE_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_NAMESPACE_NAME_TOO_LONG,errorText,NULL,0);
				parserState->error = ERROR_409;
			}
			/* check whether the namespace and property name is within the limits of  */
			if (strlen(tag)>WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)>WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				ns[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				tag[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errorText,1000,WDVH_ERR_TEXT_PROPERTY_NS_NAME_TOO_LONG,WDVH_MAX_PROPERTY_NAME_PREFIX_LEN,WDVH_MAX_NAME_SPACE_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_NS_NAME_TOO_LONG,errorText,NULL,0);
				parserState->error = ERROR_409;
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
			strcpy(capiNamespace,ns);
			strcpy(capiPropertyName,tag);
			capiWdv = xmlParserUserDataGetWDV(userData);
			WDVCAPI_PropfindAddPropertyToPropertyList(capiWdv,
 													  *(userData->propfindHandle),
													  capiNamespace,
													  capiPropertyName );
			userData->propfindEmptyPropertyList = WDVH_False; /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
		}
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123337 */

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
	}  /* End of end handler */

	/*------------------------------------------------------------------------
		Function:	propfindXmlStartNamespaceHandler
	--------------------------------------------------------------------------*/
	WDVH_Bool propfindXmlStartNamespaceHandler(void *data, 
											   const XML_Char *prefix, 
											   const XML_Char *uri)
	{
		WDVH_XmlNamespace				xmlNamespace;
		WDVH_XmlUserData				xmlUserData;
		XML_Parser						p;

		if (data==NULL || prefix==NULL || uri==NULL)
			return WDVH_False;

		if (prefix)
			strcpy(xmlNamespace.prefix,prefix);
		strcpy(xmlNamespace.uri,uri);
		xmlUserData = (WDVH_XmlUserData)data;
		p = xmlParserUserDataGetParser(xmlUserData);

		if(!xmlParserIsInNamespaceList(p,xmlNamespace,xmlUserData->nslist))
			return xmlParserAddNamespaceToList(p,xmlNamespace,xmlUserData->nslist);
		else
			return WDVH_True;

	}

	/*------------------------------------------------------------------------
		Function:	propfindXmlEndNamespaceHandler
	--------------------------------------------------------------------------*/
	void propfindXmlEndNamespaceHandler(void *data, 
										  const XML_Char *prefix)
	{
		WDVH_XmlNamespace				xmlNamespace;
		WDVH_XmlUserData				xmlUserData;
		XML_Parser						p;

		if (data==NULL || prefix==NULL)
			return;

		if (prefix)
			strcpy(xmlNamespace.prefix,prefix);
		xmlUserData = (WDVH_XmlUserData)data;
		p = xmlParserUserDataGetParser(xmlUserData);

		if(xmlParserIsPrefixInNamespaceList(p,xmlNamespace,xmlUserData->nslist))
			xmlParserRemoveNamespaceFromList(p,xmlNamespace,xmlUserData->nslist);

	}

