/****************************************************************************

  module      : WDVHandler_ProppatchHandler.c

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


	#include "SAPDB/WebDAV/Handler/WDVHandler_ProppatchHandler.h"
	#include "SAPDBCommon/SAPDB_string.h"

	/*------------------------------------------------------------------------
		Function: proppatchCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 proppatchCallCapiFunc(WDVCAPI_WDV				wdv,
									  sapdbwa_Handle			wa,
									  sapdbwa_HttpRequestP		request,
									  WDVCAPI_ProppatchHandle	proppatch) 
	{
		sapdbwa_Int2			rc;
		WDVH_Bool				success = WDV_False;
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];

		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || wdv==NULL || proppatch==NULL) {
			sapdbwa_WriteLogMsg(wa, "proppatchCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}

		/* then call CAPI Function */
/*		success = WDVCAPI_Proppatch(wdv,proppatch);
*/
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
						sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						switch(errorCode) {
							case WDVCAPI_ERR_CODE_COLLECTION_DOESNT_EXIST:
								{
									error = ERROR_404;
									break;
								}
							default:
								{
									error = ERROR_500;
									sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
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
						sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			error = ERROR_207;
		}

		return error;
	}

	/*------------------------------------------------------------------------
		Function:	proppatchBuildReply
	--------------------------------------------------------------------------*/
	WDVH_Bool proppatchBuildReply(sapdbwa_Int2 statuscode, 
							sapdbwa_Handle			wa,
							sapdbwa_HttpRequestP request,
							sapdbwa_HttpReplyP reply, 
							WDVH_Char *host,
							WDVH_Char *port,
							WDVH_Char *uri,
							WDVCAPI_WDV wdv,
							WDVCAPI_ProppatchHandle  proppatch,
							WDVH_Bool				iserror)
	{
		char *buf;
		char *url;
		sapdbwa_Bool								allocatOK;
		sapdbwa_Int4								length;
		WDVCAPI_NameSpace							propertyNamespace;
		WDVCAPI_NameSpaceShortcut					propertyNamespaceShortcut;
		WDVCAPI_PropertyName						propertyName;
		WDVCAPI_PropertyShortValue					propertyShortValue;
		WDVCAPI_PropertyStatus						propertyStatus;
        WDVCAPI_ProppatchAccessType                 propertyAccessType;
		WDVCAPI_ErrorItem							errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
		WDVH_DynamicString							namespaceList;
		WDVH_DynamicString							rootResource;
		WDVH_Bool									requestedResource = WDVH_True;
		WDVH_Char									*server=NULL;
		WDVCAPI_URI									resourceUri;
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									contentlength[10];
		
		if (wa==NULL)
			return WDVH_False;
		if (request==NULL || wdv==NULL || reply==NULL || host==NULL || port==NULL || uri==NULL || proppatch==NULL) {
			sapdbwa_WriteLogMsg(wa, "proppatchCallCapiFunc:Uninitialized Pointers\n");
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
			case ERROR_423: {
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
				sapdbwa_InitHeader(reply,
									statuscode,
									"text/xml",
									NULL,
									NULL,								/* const char    *LastModified,*/
									NULL,								/* const char    *Expires,*/
									NULL);								/* const char    *Location );*/
				sapdbwa_SetHeader( reply,
					"Transfer-Encoding",
					"chunked" );
				sapdbwa_SendHeader(reply);

				/* send body header */
				WDVH_sendBodyChunk(reply,WDVH_XML_HEADER);
				WDVH_sendBodyChunk(reply,WDVH_MULTI_STATUS_OPEN);

				/* set server prefix */
				getWebDAVPrefix(wa,prefix);

				length = strlen(host) + strlen(port) + strlen(prefix) + 3;
				WDVHandler_Allocat(sizeof(SAPDB_Char)*length,(sapdbwa_UInt1**) &server,&allocatOK);
				if (allocatOK) {
					sp77sprintf(server,length,"%s",/*host,port,*/prefix);
				}
				/* build namespace list */
				if (WDVCAPI_ProppatchGetFirstProcessedNameSpace(wdv,proppatch,&propertyNamespaceShortcut,&propertyNamespace)) {
					namespaceList = createStrBuffer(WDVH_MAX_STR_BUFFER_LEN);
                    while(propertyNamespaceShortcut) {
						if (!appendStrBuffer(namespaceList,WDVH_XMLNS)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_NS_PREFIX)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,propertyNamespaceShortcut)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_EQUAL_SIGN)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_QUOTE)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,propertyNamespace)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_QUOTE)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}
						if (!appendStrBuffer(namespaceList,WDVH_XML_SPACE)) {
							/* error handling */
							sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
							return WDVH_True;
						}

                        if (!WDVCAPI_ProppatchGetNextProcessedNameSpace(wdv,proppatch,&propertyNamespaceShortcut,&propertyNamespace)) {
					        /* error handling */
					        sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
					        return WDVH_True;
                        }
					};
				} else {
					/* error handling */
					sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
					return WDVH_True;
				}

				/* response */
				WDVH_sendBodyChunk(reply,WDVH_RESPONSE_OPEN);
				WDVH_sendBodyChunk(reply,getStrBufferContent(namespaceList));
				WDVH_sendBodyChunk(reply,WDVH_XML_CLOSE_TAG);
							
				/*		href */
				WDVH_sendBodyChunk(reply,WDVH_HREF_OPEN);
				WDVH_sendBodyChunk(reply,server);
				WDVH_sendBodyChunk(reply,uri);
				WDVH_sendBodyChunk(reply,WDVH_HREF_CLOSE);

				/* loop at properties */
				if (WDVCAPI_ProppatchGetFirstProcessedProperty(wdv,
															   proppatch,
															   &propertyNamespaceShortcut,
															   &propertyName,
                                                               &propertyAccessType,
															   &propertyStatus,
															   &errorItem)) {
				} else {
					/* clean up */
					destroyStrBuffer(namespaceList);
					/* error handling */
					sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
					return WDVH_True;
				}
				while(propertyName) {
					/*		propstat */
					WDVH_sendBodyChunk(reply,WDVH_PROPSTAT_OPEN);

					/*			prop */
					WDVH_sendBodyChunk(reply,WDVH_PROP_OPEN);

					/*				property name */
					WDVH_sendBodyChunk(reply,WDVH_XML_OPEN_TAG);
                    /*?? MaO: Name space shortcut prefix? */
					WDVH_sendBodyChunk(reply, "s");
					WDVH_sendBodyChunk(reply,propertyNamespaceShortcut);
					WDVH_sendBodyChunk(reply,WDVH_XML_COLON);
					WDVH_sendBodyChunk(reply,propertyName);
					WDVH_sendBodyChunk(reply,WDVH_XML_SLASH);
					WDVH_sendBodyChunk(reply,WDVH_XML_CLOSE_TAG);
					/*			/prop */
					WDVH_sendBodyChunk(reply,WDVH_PROP_CLOSE);

					/*			status */
					WDVH_sendBodyChunk(reply,WDVH_STATUS_OPEN);
					/*  if there was an error during proppatch all would be ok property
						updates are "Failed Dependency" errors. All failed property
						updates appear with the real error code */
					if (iserror == WDVH_True) {
						if (propertyStatus == WDVCAPI_PROPERTY_STATUS_OK) {
							WDVH_sendBodyChunk(reply,WDVH_STATUS_424);
							/*			/status */
							WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
							/*			responsedescription */
							WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_OPEN);
							WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_424);
							WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_CLOSE);
						} else if (propertyStatus == WDVCAPI_PROPERTY_STATUS_FAILED) {
							/*  what error occured? */
							WDVCAPI_GetErrorType(errorItem,&errorType);
							switch (errorType) {
								case WDVCAPI_ERR_TYPE_UNDEFINED:
								case WDVCAPI_ERR_TYPE_SQL: 
									{
										/* internal server error */
										WDVCAPI_GetErrorText(errorItem,&errorMsg);
										error = ERROR_500;
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
										sapdbwa_WriteLogMsg(wa, errorText);
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
										sapdbwa_WriteLogMsg(wa, errorText);
										WDVH_sendBodyChunk(reply,WDVH_STATUS_500);
										/*			/status */
										WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
										/*			responsedescription */
										WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_OPEN);
										WDVH_sendBodyChunk(reply,errorMsg);
										WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_CLOSE);
										break;
									}
								case WDVCAPI_ERR_TYPE_CAPI: 
									{
										WDVCAPI_GetErrorCode(errorItem,&errorCode);
										WDVCAPI_GetErrorText(errorItem,&errorMsg);
										switch(errorCode) {
											case WDVCAPI_ERR_CODE_COULD_NOT_SET_PROPERTY:
											case WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_TYPE:
											case WDVCAPI_ERR_CODE_EMPTY_PROPERTY_LIST:
												{
													error = ERROR_500;
													sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
													sapdbwa_WriteLogMsg(wa, errorText);
													sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
													sapdbwa_WriteLogMsg(wa, errorText);
													WDVH_sendBodyChunk(reply,WDVH_STATUS_500);
													/*			/status */
													WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
													/*			responsedescription */
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_OPEN);
													WDVH_sendBodyChunk(reply,errorMsg);
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_CLOSE);
													break;
												}
											case WDVCAPI_ERR_CODE_COULD_NOT_REMOVE_PROPERTY:
												{
													WDVH_sendBodyChunk(reply,WDVH_STATUS_409);
													/*			/status */
													WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
													/*			responsedescription */
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_OPEN);
													WDVH_sendBodyChunk(reply,errorMsg);
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_CLOSE);
													break;
												}
											default:
												{
													error = ERROR_500;
													sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
													sapdbwa_WriteLogMsg(wa, errorText);
													sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
													sapdbwa_WriteLogMsg(wa, errorText);
													WDVH_sendBodyChunk(reply,WDVH_STATUS_500);
													/*			/status */
													WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
													/*			responsedescription */
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_OPEN);
													WDVH_sendBodyChunk(reply,errorMsg);
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_CLOSE);
													break;
												}
										}
										break;
									}
								/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123369 */
								case WDVCAPI_ERR_TYPE_WDVHANDLER: 
									{
										WDVCAPI_GetErrorCode(errorItem,&errorCode);
										WDVCAPI_GetErrorText(errorItem,&errorMsg);
										switch(errorCode) {
											case WDVH_ERR_CODE_WRONG_CONTENTYPE_SYNTAX:
											case WDVH_ERR_CODE_SET_LIVEPROPERTY_FORBIDDEN:
											case WDVH_ERR_CODE_REMOVE_LIVEPROPERTY_FORBIDDEN:
											case WDVH_ERR_CODE_PROPERTY_TOO_LONG:				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123800 */
											case WDVH_ERR_CODE_PROPERTY_NAME_TOO_LONG:			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123800 */
											case WDVH_ERR_CODE_NAMESPACE_NAME_TOO_LONG:			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123800 */
											case WDVH_ERR_CODE_PROPERTY_NS_NAME_TOO_LONG:		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123800 */
												{
													WDVH_sendBodyChunk(reply,WDVH_STATUS_409);
													/*			/status */
													WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
													/*			responsedescription */
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_OPEN);
													WDVH_sendBodyChunk(reply,errorMsg);
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_CLOSE);
													break;
												}
											default:
												{
													error = ERROR_500;
													sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
													sapdbwa_WriteLogMsg(wa, errorText);
													sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
													sapdbwa_WriteLogMsg(wa, errorText);
													WDVH_sendBodyChunk(reply,WDVH_STATUS_500);
													/*			/status */
													WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
													/*			responsedescription */
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_OPEN);
													WDVH_sendBodyChunk(reply,errorMsg);
													WDVH_sendBodyChunk(reply,WDVH_RESPONSEDESC_CLOSE);
													break;
												}
										}
										break;
									}
								/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123369 */
								default:
									{
										WDVCAPI_GetErrorText(errorItem,&errorMsg);
										error = ERROR_500;
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
										sapdbwa_WriteLogMsg(wa, errorText);
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
										sapdbwa_WriteLogMsg(wa, errorText);
										break;
									}
							}
						}
					} else {
						WDVH_sendBodyChunk(reply,WDVH_STATUS_OK);
						/*			/status */
						WDVH_sendBodyChunk(reply,WDVH_STATUS_CLOSE);
					}

					/*		/propstat */
					WDVH_sendBodyChunk(reply,WDVH_PROPSTAT_CLOSE);
					WDVCAPI_ProppatchGetNextProcessedProperty(wdv,
															  proppatch,
															  &propertyNamespaceShortcut,
															  &propertyName,
                                                              &propertyAccessType,
															  &propertyStatus,
															  &errorItem);
				}

				/*	/response */
				WDVH_sendBodyChunk(reply,WDVH_RESPONSE_CLOSE);


				WDVH_sendBodyChunk(reply,WDVH_MULTI_STATUS_CLOSE);
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
		Function:	proppatchParseRequestBody
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 proppatchParseRequestBody(sapdbwa_Handle		wa,
										  WDVCAPI_WDV wdv,
										  WDVCAPI_ProppatchHandle proppatch,
										  sapdbwa_HttpRequestP request,
										  WDVH_Bool				*iserror)
	{

	    WDVH_Int2						done;
		WDVH_Int2						pos;
		WDVH_XmlNamespaceList			defaultNamespaceList;
		WDVH_XmlProppatchParserState	parserState;
		WDVH_XmlUserData				newUserData;
		WDVH_Error						error = ERROR_200;
		WDVH_UInt4						contentLength=0;
		WDVH_UInt4						bytesRead=0;
		WDVH_Char						errorText[1000];
		/*WDVH_XmlBuffer					buff1;			http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		/*WDVH_XmlBuffer					buff2;			http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_XmlBuffer					currentBuffer;
		/*WDVH_XmlBufferRef				previousBuffer;		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		/*WDVH_XmlBufferRef				tempBuffer;			http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		/*WDVH_UInt4					bufferChunks;		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		XML_Parser						p;


		if (wa==NULL)
			return ERROR_500;
		if (request==NULL || wdv==NULL || proppatch==NULL || iserror==NULL) {
			sapdbwa_WriteLogMsg(wa, "proppatchParseRequestBody:Uninitialized Pointers\n");
			return ERROR_500;
		}

		/* Initialization */
		p =  XML_ParserCreateNS("UTF-8",WDVH_NS_SEPARATOR);
		if (!p) {
			return ERROR_500;
		}
		/*bufferChunks = 0;									http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		/*currentBuffer = buff2;							http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		/*previousBuffer = buff1;							http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		strcpy(currentBuffer,"");
		/*strcpy(previousBuffer,"");						http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */

		xmlParserCreateNamespaceList(p,&defaultNamespaceList); 
		/* Attention: previousBuffer and currentBuffer are swaped later on, so give 
		              previousBuffer as currentBuffer to user data! */
		xmlParserCreateUserData(p,currentBuffer,defaultNamespaceList,wdv,&newUserData);
		xmlParserUserDataSetProppatchHandle(newUserData,proppatch);
		xmlParserCreateProppatchParserState(&parserState);
		xmlParserUserDataSetParserState(newUserData,(WDVH_XmlParserState)parserState);
		XML_SetUserData(p,(void*)newUserData);
		XML_SetElementHandler(p, 
							  proppatchXmlTagStartHandler,
							  proppatchXmlTagEndHandler);
/*		XML_SetNamespaceDeclHandler(p,											http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 
									proppatchXmlStartNamespaceHandler,			http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 
									proppatchXmlEndNamespaceHandler);			http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		XML_SetParamEntityParsing(p,XML_PARAM_ENTITY_PARSING_ALWAYS);

		
		/* loop at all chunks of the incomming XML document and parse the XML */
		do {
			/*bufferChunks++;								http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/*tempBuffer = previousBuffer;					http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/*previousBuffer = currentBuffer;				http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/*currentBuffer = tempBuffer;					http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/*xmlParserUserDataSetParseBuffer(newUserData,currentBuffer,previousBuffer,bufferChunks);	http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* get content length to determine end of read loop
			   the recv in sapdbwa_ReadBody is a blocking one, so it has to be avoided
			   that one single bytes is read beyond the real existing content length of
			   the body */
			if (sapdbwa_GetContentLength(request)) {
				contentLength = atoi(sapdbwa_GetContentLength(request));
			}
			if (contentLength > 0) {
				pos = sapdbwa_ReadBody( request, currentBuffer, WDVH_MAX_XML_BUFFER_LEN-1);
				if (strcmp(currentBuffer,"")==0) {
					sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
					sapdbwa_WriteLogMsg(wa, errorText);
					sp77sprintf(errorText,1000,ERROR_PROPPATCH_MISSING_BODY);
					sapdbwa_WriteLogMsg(wa, errorText);
					error = ERROR_400;
					done = 1;
				}
				bytesRead += pos;
				if (bytesRead >= contentLength)
					done = 1;
				else if (pos == 0) {
					done = 1;
					strcpy(currentBuffer,"");
				} else
					done = 0;
				if (strcmp(currentBuffer,"")!=0) {
					if (newUserData->bufferEnd > 0)							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
						newUserData->bufferStart = newUserData->bufferEnd+1;/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
					else													/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
						newUserData->bufferStart = 0;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
					newUserData->bufferEnd = bytesRead-1;					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
					newUserData->propertyValueStartPos = 0;					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
					newUserData->propertyValueEndPos = pos;					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
					if (! XML_Parse(p, currentBuffer, pos, done)) {
						pos = XML_GetCurrentByteIndex(p);
						sp77sprintf(errorText,1000,ERROR_PROPPATCH_PARSE_1,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_PROPPATCH_PARSE_2,XML_GetCurrentLineNumber(p),XML_ErrorString(XML_GetErrorCode(p)));
						sapdbwa_WriteLogMsg(wa, errorText);
						error = ERROR_400;
						done = 1;
					}
					if (newUserData->propertyShortValueStarted) {															/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
						if (newUserData->propertyValueEndPos != WDVH_MAX_XML_BUFFER_LEN)									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
							SAPDB_memcpy(currentBuffer+newUserData->propertyValueEndPos,"\0",1);									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
						if (newUserData->propertyValueEndPos+newUserData->bufferStart < newUserData->bufferEnd)				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
							newUserData->propertyShortValueStarted = WDVH_False;											/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
						if ((getStrBufferLength(newUserData->propertyShortValue) +
							newUserData->propertyValueEndPos - newUserData->propertyValueStartPos + 1) >WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
							WDVCAPI_SetErrorItemEx(&newUserData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_TOO_LONG,WDVH_ERR_TEXT_PROPERTY_TOO_LONG,NULL,0); /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
							/*sp77sprintf(errmsg,1000,"Lock owner exceeds	supported length of %d characters.",WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);*/	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
						} else {																						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
							appendStrBuffer(newUserData->propertyShortValue,currentBuffer+newUserData->propertyValueStartPos);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
						}																								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
					}
				} else {
					done = 1;
				}
			} else {
				sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
				sapdbwa_WriteLogMsg(wa, errorText);
				sp77sprintf(errorText,1000,ERROR_PROPPATCH_MISSING_BODY);
				sapdbwa_WriteLogMsg(wa, errorText);
				error = ERROR_400;
				done = 1;
			}
		} while(!done);

		if (error == ERROR_400) {
			sp77sprintf(errorText,1000,ERROR_PROPPATCH_PARSE_1,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
			sapdbwa_WriteLogMsg(wa, errorText);
		} else {
			parserState = xmlParserUserDataGetProppatchParserState(newUserData);
			if (parserState->error == WDVH_XML_PARSER_OK) {
				error = ERROR_207;
				*iserror = WDVH_False;
			} else if (parserState->error == WDVH_XML_PARSER_PROPPATCH_ERROR) {
				error = ERROR_207;
				*iserror = WDVH_True;
			} else if (parserState->error == WDVH_XML_PARSER_MEMORY_ERROR) {
				error = ERROR_500;
				sapdbwa_WriteLogMsg(wa, "PROPPATCH Parsing: Could not allocate memory.");
				*iserror = WDVH_False;
			} else {
				error = ERROR_400;
				sp77sprintf(errorText,1000,ERROR_PROPPATCH_PARSE_1,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
				sapdbwa_WriteLogMsg(wa, errorText);
			}
		}

		/* Clean Up */
		xmlParserDestroyNamespaceList(p,newUserData->nslist);
		xmlParserDestroyProppatchParserState((WDVH_XmlProppatchParserState)newUserData->parserState);
		xmlParserDestroyUserData(newUserData);
		XML_ParserFree(p);
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	proppatchXmlTagStartHandler
	--------------------------------------------------------------------------*/
	void proppatchXmlTagStartHandler(void *data, const char *el, const char **attr) {

		/* DTD proppatch Body
			   <!ELEMENT propertyupdate (remove | set)+ > 
			   <!ELEMENT remove (prop) > 
			   <!ELEMENT set (prop) > 
			   <!ELEMENT prop ANY > 
		*/
		WDVH_Char						*ns;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_Char						*tag;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_Bool						allocatOK;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_XmlProppatchParserState	parserState;
		WDVH_XmlUserData				userData;
		XML_Parser						p;

		if (data==NULL || el==NULL || attr==NULL)
			return;

		userData = (WDVH_XmlUserData)data;
		p = xmlParserUserDataGetParser(userData);
		parserState = xmlParserUserDataGetProppatchParserState(userData);

		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
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
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */

		xmlParserCutNamespace(el,ns,tag,WDVH_NS_SEPARATOR);
		/* check wether or not the tag contains a namespace abbrev. if so the request must
		   be rejected due to the missing declaration of the namespace for the discoverd
		   namespace abbrev. */
		if(xmlParserCheckNamespaceAbbrev(tag)) {
			parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
		}

		/* parsing using kind of finite automata. please refer to technical documentation
		   for further information */

		if (parserState->error == WDVH_XML_PARSER_OK ||
			parserState->error == WDVH_XML_PARSER_PROPPATCH_ERROR) {

			if (parserState->depth == 3 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_14) {
				/* if there comes a start tag it is a property start tag and it is a
				   property to remove
				   --> check wether property is correct EMPTY tag otherwise syntax error */ 
				/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360
				if (xmlParserCheckEmptyTag(p,userData,tag)) {*/
					/* add property to list for removal 
					if(!WDVCAPI_ProppatchRemove(userData->wdv,
												userData->proppatchHandle,
												ns,
												tag)) {
						parserState->error = WDVH_XML_PARSER_PROPPATCH_ERROR;
					}
					*//* stay at this state 
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}*/
				parserState->depth++;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_18;
				/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360 */
			}

			if (parserState->depth == 2 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_13) {
				/* only <prop> may appear otherwise syntax error */
				if (strcmp(tag,WDVH_XML_PROP_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_14;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 3 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_11) {
				/* if there comes a start tag it is a property start tag and it is a
				   property to set
				   --> collect property and value and set it on the list */ 
				/* collect value of the property */
				userData->propertyShortValueStarted = WDVH_True;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				getPropertyShortValueStartPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				/*proppatchReadShortValue(userData,tag);*/			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				/* Correction Start http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
				parserState->depth++;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_20;
				/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
			}

			if (parserState->depth == 2 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_10) {
				/* only <prop> may appear otherwise syntax error */
				if (strcmp(tag,WDVH_XML_PROP_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_11;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 1 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_9) {
				/* <set> or <remove> may appear as start tag otherwise syntax error */
				/* </propertyupdate> is handled by end tag handler! */
				if (strcmp(tag,WDVH_XML_SET_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_10;
				} else if (strcmp(tag,WDVH_XML_REMOVE_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_13;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 3 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_7) {
				/* if there comes a start tag it is a property start tag and it is a
				   property to remove
				   --> check wether property is correct EMPTY tag otherwise syntax error */ 
				/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360
				if (xmlParserCheckEmptyTag(p,userData,tag)) {*/
					/* add property to list for removal 
					if(!WDVCAPI_ProppatchRemove(userData->wdv,
												userData->proppatchHandle,
												ns,
												tag)) {
						parserState->error = WDVH_XML_PARSER_PROPPATCH_ERROR;
					}*/
					/* stay at this state 
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				} */
				parserState->depth++;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_17;
				/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360 */
			}

			if (parserState->depth == 2 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_6) {
				/* only <prop> may appear otherwise syntax error */
				if (strcmp(tag,WDVH_XML_PROP_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_7;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 3 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_4) {
				/* if there comes a start tag it is a property start tag and it is a
				   property to set
				   --> collect property and value and set it on the list */ 
				/* collect value of the property */
				userData->propertyShortValueStarted = WDVH_True;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				getPropertyShortValueStartPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				/*proppatchReadShortValue(userData,tag);*/			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				/* Correction Start http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
				parserState->depth++;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_19;
				/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
			}

			if (parserState->depth == 2 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_3) {
				/* only <prop> may appear otherwise syntax error */
				if (strcmp(tag,WDVH_XML_PROP_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_4;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 1 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_2) {
				/* <set> or <remove> may appear as start tag otherwise syntax error */
				/* </propertyupdate> is handled by end tag handler! */
				if (strcmp(tag,WDVH_XML_SET_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_3;
				} else if (strcmp(tag,WDVH_XML_REMOVE_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_6;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 0 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_1) {
				/* <set> or <remove> may appear as start tag otherwise syntax error */
				/* </propertyupdate> is handled by end tag handler! */
				if (strcmp(tag,WDVH_XML_PROPERTYUPDATE_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_PROPPATCH_DFA_STATE_2;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 2 &&
				parserState->state == WDVH_PROPPATCH_DFA_STATE_5) {
				if (strcmp(tag,WDVH_XML_PROP_CLOSE)!=0) {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				} 
				
			} 
		}
		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		if (tag) {
			WDVHandler_Free((sapdbwa_UInt1*) tag);
		}
		if (ns) {
			WDVHandler_Free((sapdbwa_UInt1*) ns);
		}
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */

	}  /* End of start handler */

	/*------------------------------------------------------------------------
		Function:	proppatchXmlTagEndHandler
	--------------------------------------------------------------------------*/
	void proppatchXmlTagEndHandler(void *data, const char *el) {
		
		WDVH_Char						*ns;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_Char						*tag;							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_Bool						allocatOK;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		WDVH_XmlProppatchParserState	parserState;
		WDVH_XmlUserData				userData;
		WDVH_Char						errmsg[1001];


		if (data==NULL || el==NULL)
			return;

		userData = (WDVH_XmlUserData)data;
		parserState = xmlParserUserDataGetProppatchParserState(userData);

		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
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
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */

		xmlParserCutNamespace(el,ns,tag,WDVH_NS_SEPARATOR);

		if (parserState->depth == 1 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_9) {
			if (strcmp(tag,WDVH_XML_PROPERTYUPDATE_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_16;
			}
		} 

		if (parserState->depth == 2 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_15) {
			if (strcmp(tag,WDVH_XML_REMOVE_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_9;
			}
		} 

		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
		if (parserState->depth == 3 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_4) {
			if (strcmp(tag,WDVH_XML_PROP_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_5;
			}
		} 

		if (parserState->depth == 3 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_11) {
			if (strcmp(tag,WDVH_XML_PROP_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_12;
			}
		} 
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */

		if (parserState->depth == 2 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_12) {
			if (strcmp(tag,WDVH_XML_SET_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_9;
			}
		} 

		if (parserState->depth == 4 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_20) {
			/* collect value of the property */
			getPropertyShortValueEndPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* proppatchReadShortValue(userData,tag);*/		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* append this last snippet to the string buffer */
			if (userData->propertyValueEndPos != WDVH_MAX_XML_BUFFER_LEN)									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				SAPDB_memcpy(userData->currentBuff+userData->propertyValueEndPos,"\0",1);									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			if (userData->propertyValueEndPos+userData->bufferStart < userData->bufferEnd)					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				userData->propertyShortValueStarted = WDVH_False;											/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			if ((getStrBufferLength(userData->propertyShortValue) +
				userData->propertyValueEndPos - userData->propertyValueStartPos + 1) >WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_PROPERTY_TOO_LONG,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_TOO_LONG,errmsg,NULL,0);
			} else {																						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				appendStrBuffer(userData->propertyShortValue,userData->currentBuff+userData->propertyValueStartPos);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			}																								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123369 */
			/* if the property to change is "content-type" check the synatx of it according to RFC 2518 Section 4.1 */
			if (strcmp(tag,WDVH_PROP_CONTENT_TYPE)==0) {
				if (!WDVH_CheckContentType(getStrBufferContent(userData->propertyShortValue))) {
					/* create an WDVCAPI_ErrorItem and set the error code and message text */
					WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_WRONG_CONTENTYPE_SYNTAX,WDVH_ERR_TEXT_WRONG_CONTENTYPE_SYNTAX,NULL,0);
				}
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123369 */
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* check whether the property might be set or not. live properties are forbidden to set */
			if (!WDVH_CheckPropertySet(tag)) {
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_SET_LIVEPROPERTY_FORBIDDEN,WDVH_ERR_TEXT_SET_LIVEPROPERTY_FORBIDDEN,NULL,0);
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* check whether the property name is within the limits of  */
			if (strlen(tag)>WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)<=WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				tag[WDVH_MAX_PROPERTY_NAME_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_PROPERTY_NAME_TOO_LONG,WDVH_MAX_PROPERTY_NAME_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_NAME_TOO_LONG,errmsg,NULL,0);
			}
			/* check whether the namespace name is within the limits of  */
			if (strlen(tag)<=WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)>WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				ns[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_NAMESPACE_NAME_TOO_LONG,WDVH_MAX_NAME_SPACE_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_NAMESPACE_NAME_TOO_LONG,errmsg,NULL,0);
			}
			/* check whether the namespace and property name is within the limits of  */
			if (strlen(tag)>WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)>WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				ns[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				tag[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_PROPERTY_NS_NAME_TOO_LONG,WDVH_MAX_PROPERTY_NAME_PREFIX_LEN,WDVH_MAX_NAME_SPACE_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_NS_NAME_TOO_LONG,errmsg,NULL,0);
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* add property to list for change */
			if(!WDVCAPI_ProppatchSetShortValue(userData->wdv,
											   userData->proppatchHandle,
												ns,
												tag,
												getStrBufferContent(userData->propertyShortValue),
												userData->error)) {
				parserState->error = WDVH_XML_PARSER_PROPPATCH_ERROR;
			}
			/* reset user data for value collection */
			resetStrBuffer(userData->propertyShortValue);
			userData->propertyValueStartPos = 0;
			userData->propertyValueEndPos = userData->bufferEnd - userData->bufferStart + 1;
			userData->error = NULL;								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			parserState->depth--;
			parserState->state = WDVH_PROPPATCH_DFA_STATE_11;
		} 

		if (parserState->depth == 2 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_8) {
			if (strcmp(tag,WDVH_XML_REMOVE_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_9;
			}
		} 

		if (parserState->depth == 2 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_5) {
			if (strcmp(tag,WDVH_XML_SET_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_9;
			} 
			
		} 

		if (parserState->depth == 4 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_19) {
			/* collect value of the property */
			getPropertyShortValueEndPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* proppatchReadShortValue(userData,tag);*/		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* append this last snippet to the string buffer */
			if (userData->propertyValueEndPos != WDVH_MAX_XML_BUFFER_LEN)									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				SAPDB_memcpy(userData->currentBuff+userData->propertyValueEndPos,"\0",1);									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			if (userData->propertyValueEndPos+userData->bufferStart < userData->bufferEnd)					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				userData->propertyShortValueStarted = WDVH_False;											/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			if ((getStrBufferLength(userData->propertyShortValue) +
				userData->propertyValueEndPos - userData->propertyValueStartPos + 1) >WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_PROPERTY_TOO_LONG,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_TOO_LONG,errmsg,NULL,0);
			} else {																						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
				appendStrBuffer(userData->propertyShortValue,userData->currentBuff+userData->propertyValueStartPos);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			}																								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123369 */
			/* if the property to change is "content-type" check the synatx of it according to RFC 2518 Section 4.1 */
			if (strcmp(tag,WDVH_PROP_CONTENT_TYPE)==0) {
				if (!WDVH_CheckContentType(getStrBufferContent(userData->propertyShortValue))) {
					/* create an WDVCAPI_ErrorItem and set the error code and message text */
					WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_WRONG_CONTENTYPE_SYNTAX,WDVH_ERR_TEXT_WRONG_CONTENTYPE_SYNTAX,NULL,0);
				}
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123369 */
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* check whether the property might be set or not. live properties are forbidden to set */
			if (!WDVH_CheckPropertySet(tag)) {
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_SET_LIVEPROPERTY_FORBIDDEN,WDVH_ERR_TEXT_SET_LIVEPROPERTY_FORBIDDEN,NULL,0);
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* check whether the property name is within the limits of  */
			if (strlen(tag)>WDVH_MAX_PROPERTY_NAME_PREFIX_LEN) {
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_PROPERTY_NAME_TOO_LONG,WDVH_MAX_PROPERTY_NAME_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_NAME_TOO_LONG,errmsg,NULL,0);
			}
			/* check whether the namespace name is within the limits of  */
			if (strlen(tag)<=WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)>WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				ns[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_NAMESPACE_NAME_TOO_LONG,WDVH_MAX_NAME_SPACE_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_NAMESPACE_NAME_TOO_LONG,errmsg,NULL,0);
			}
			/* check whether the namespace and property name is within the limits of  */
			if (strlen(tag)>WDVH_MAX_PROPERTY_NAME_PREFIX_LEN && strlen(ns)>WDVH_MAX_NAME_SPACE_PREFIX_LEN) {
				/* avoid writing memory that was not allocated */
				ns[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				tag[WDVH_MAX_NAME_SPACE_PREFIX_LEN] = '\0';
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				sp77sprintf(errmsg,1000,WDVH_ERR_TEXT_PROPERTY_NS_NAME_TOO_LONG,WDVH_MAX_PROPERTY_NAME_PREFIX_LEN,WDVH_MAX_NAME_SPACE_PREFIX_LEN);
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_PROPERTY_NS_NAME_TOO_LONG,errmsg,NULL,0);
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			/* add property to list for change */
			if(!WDVCAPI_ProppatchSetShortValue(userData->wdv,
											   userData->proppatchHandle,
												ns,
												tag,
												getStrBufferContent(userData->propertyShortValue),
												userData->error)) {
				parserState->error = WDVH_XML_PARSER_PROPPATCH_ERROR;
			}
			/* reset user data for value collection */
			resetStrBuffer(userData->propertyShortValue);
			userData->propertyValueStartPos = 0;
			userData->propertyValueEndPos = userData->bufferEnd - userData->bufferStart + 1;
			userData->error = NULL;								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			parserState->depth--;
			parserState->state = WDVH_PROPPATCH_DFA_STATE_4;
		} 

		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
		if (parserState->depth == 3 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_7) {
			if (strcmp(tag,WDVH_XML_PROP_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_8;
			}
		} 

		if (parserState->depth == 3 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_14) {
			if (strcmp(tag,WDVH_XML_PROP_CLOSE)==0) {
				parserState->depth--;
				parserState->state = WDVH_PROPPATCH_DFA_STATE_15;
			}
		} 
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123906 */
		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360 */
		if (parserState->depth == 4 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_17) {
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* check whether the property might be removed or not. live properties are forbidden to remove */
			if (!WDVH_CheckPropertyRemove(tag)) {
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_REMOVE_LIVEPROPERTY_FORBIDDEN,WDVH_ERR_TEXT_REMOVE_LIVEPROPERTY_FORBIDDEN,NULL,0);
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* add property to list for removal */
			if(!WDVCAPI_ProppatchRemove(userData->wdv,
										userData->proppatchHandle,
										ns,
										tag,
										userData->error)) {
				parserState->error = WDVH_XML_PARSER_PROPPATCH_ERROR;
			}
			userData->error = NULL;								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			parserState->depth--;
			parserState->state = WDVH_PROPPATCH_DFA_STATE_7;
		} 

		if (parserState->depth == 4 &&
			parserState->state == WDVH_PROPPATCH_DFA_STATE_18) {
			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* check whether the property might be removed or not. live properties are forbidden to remove */
			if (!WDVH_CheckPropertyRemove(tag)) {
				/* create an WDVCAPI_ErrorItem and set the error code and message text */
				WDVCAPI_SetErrorItemEx(&userData->error,WDVCAPI_ERR_TYPE_WDVHANDLER,WDVH_ERR_CODE_REMOVE_LIVEPROPERTY_FORBIDDEN,WDVH_ERR_TEXT_REMOVE_LIVEPROPERTY_FORBIDDEN,NULL,0);
			}
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123441 */
			/* add property to list for removal */
			if(!WDVCAPI_ProppatchRemove(userData->wdv,
										userData->proppatchHandle,
										ns,
										tag,
										userData->error)) {
				parserState->error = WDVH_XML_PARSER_PROPPATCH_ERROR;
			}
			userData->error = NULL;								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
			parserState->depth--;
			parserState->state = WDVH_PROPPATCH_DFA_STATE_14;
		} 
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123360 */

		/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		if (tag) {
			WDVHandler_Free((sapdbwa_UInt1*) tag);
		}
		if (ns) {
			WDVHandler_Free((sapdbwa_UInt1*) ns);
		}
		/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
	}  /* End of end handler */

	/*------------------------------------------------------------------------
		Function:	proppatchXmlStartNamespaceHandler
	--------------------------------------------------------------------------*/
	void proppatchXmlStartNamespaceHandler(void *data, 
											   const XML_Char *prefix, 
											   const XML_Char *uri)
	{
		WDVH_XmlNamespace	xmlNamespace;
		WDVH_XmlUserData	xmlUserData;
		XML_Parser			p;

		if (data==NULL || prefix==NULL || uri==NULL)
			return;

		if (prefix)
			strcpy(xmlNamespace.prefix,prefix);
		strcpy(xmlNamespace.uri,uri);
		xmlUserData = (WDVH_XmlUserData)data;
		p = xmlParserUserDataGetParser(xmlUserData);

		if(!xmlParserIsInNamespaceList(p,xmlNamespace,xmlUserData->nslist))
			xmlParserAddNamespaceToList(p,xmlNamespace,xmlUserData->nslist);
        /*
			return xmlParserAddNamespaceToList(p,xmlNamespace,xmlUserData->nslist);
		else
			return WDVH_True;
        */

	}

	/*------------------------------------------------------------------------
		Function:	proppatchXmlEndNamespaceHandler
	--------------------------------------------------------------------------*/
	void proppatchXmlEndNamespaceHandler(void *data, 
										  const XML_Char *prefix)
	{
		WDVH_XmlNamespace	xmlNamespace;
		WDVH_XmlUserData	xmlUserData;
		XML_Parser			p;

		if (data==NULL || prefix==NULL)
			return;

		if (prefix)
			strcpy(xmlNamespace.prefix,prefix);
		xmlUserData = (WDVH_XmlUserData)data;
		p = xmlParserUserDataGetParser(xmlUserData);


		if(xmlParserIsPrefixInNamespaceList(p,xmlNamespace,xmlUserData->nslist))
			xmlParserRemoveNamespaceFromList(p,xmlNamespace,xmlUserData->nslist);

	}

	/*------------------------------------------------------------------------
		Function:	proppatchReadShortValue
		commented out: http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800
	--------------------------------------------------------------------------*/
	void proppatchReadShortValue(WDVH_XmlUserData data, WDVH_Char *el) {

/*		WDVH_Char			tag[WDVH_MAX_PROPERTY_NAME_PREFIX_LEN+WDVH_MAX_NAME_SPACE_LEN+2+1];
		WDVH_Char			*pTag;
		WDVH_Char			*temp1,*temp2;
		WDVH_Int4			pos0,pos1,pos2,pos3;
		WDVH_Int4			startPosPrevBuff,startPosCurrBuff;
		XML_Parser			p;

		p = xmlParserUserDataGetParser(data);

		if (!data->proppatchShortValueStarted) {
*/			/* called from start tag handler after discovery of a property start tag */
			/* check wether end tag is in current parse buffer */
			/* pointer to start of current tag */
/*			pos0 = XML_GetCurrentByteIndex(p);
*/			/* if start of current tag is located in the previous handled buffer chunk,
			   the tag has to be build of two chunks one located in previous and one located in
			   current buffer */
/*			pos1 = pos0 - ((data->bufferChunks - 1) * WDVH_MAX_XML_BUFFER_LEN);
			if (pos1 < 0) {
*/				/* get chunk of current tag from previous buffer */
				/* temp1 = data->previousBuff + (WDVH_MAX_XML_BUFFER_LEN + pos1); */
				/* read til buffer end */
				/* get the remaining part from current buffer */
				/* - find closing ">" */
/*				temp1 = data->currentBuff;
				if (temp1) {
					temp2 = strchr(temp1,'>');
					if (temp2) {
						data->proppatchValueStartPos = pos0 + (-pos1) + temp2 - temp1;
						data->proppatchShortValueStarted = WDVH_True;
					}
				}
			} else {
*/              /* Get short value start pos */
/*				temp1 = data->currentBuff+pos1+1;
                if (temp1) {
				    temp2 = strchr(temp1,'>');
                    if (temp2) {
			            data->proppatchValueStartPos = pos0 + temp2 - temp1 + 1;
			            data->proppatchShortValueStarted = WDVH_True;
                    }
                }
			}
		} else {
*/			/* called from end tag handler after discovery of a property end tag */
			/* check wether end tag is in current parse buffer */
			/* pointer to start of current tag */
/*			pos0 = XML_GetCurrentByteIndex(p);
*/			/* end position of property value is one before start of end tag */
/*			data->proppatchValueEndPos = pos0 - 1;
*/			/* if start of current tag is located in the previous handled buffer chunk,
			   the tag has to be build of two chunks one located in previous and one located in
			   current buffer */
/*			pos1 = pos0 - ((data->bufferChunks -1)* WDVH_MAX_XML_BUFFER_LEN);
*/			/* start pos of previous buffer */
/*			startPosPrevBuff = ((data->bufferChunks - 2)* WDVH_MAX_XML_BUFFER_LEN);
*/			/* start pos of current buffer */
/*			startPosCurrBuff = ((data->bufferChunks - 1)* WDVH_MAX_XML_BUFFER_LEN);
			if (pos1 < 0) {
*/				/* end tag starts in previuos buffer -> read property value from there */
				/* TODO: for long property values the two existing buffer chunks are not
				         enough -> do something about it!!! */
				/* start pos of property value in previous buffer */
/*				pos3 = data->proppatchValueStartPos - ((data->bufferChunks - 1) * WDVH_MAX_XML_BUFFER_LEN);
*/				/* for this case read til property value start pos or til buffer start */
				/* check wether property value start pos is in previuos buffer */
/*				if (data->proppatchValueStartPos >= startPosPrevBuff) {
*/					/* read til property value start pos */
/*					if(-pos3+pos1-1 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						SAPDB_memcpy(data->proppatchShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
						SAPDB_memcpy(data->proppatchShortValue-pos3+pos1-1,"\0",1);
					} else {
						SAPDB_memcpy(data->proppatchShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,-pos3+pos1-1);
						SAPDB_memcpy(data->proppatchShortValue-pos3+pos1-1,"\0",1);
					}
				} else {
*/					/* read til buffer start */
/*					pos2 = pos0 - ((data->bufferChunks - 2)* WDVH_MAX_XML_BUFFER_LEN);
					if (pos2 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						SAPDB_memcpy(data->proppatchShortValue,data->previousBuff,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
						SAPDB_memcpy(data->proppatchShortValue+WDVH_MAX_PROPERTY_SHORT_VALUE_LEN,"\0",1);
					} else {
						SAPDB_memcpy(data->proppatchShortValue,data->previousBuff,pos2);
						SAPDB_memcpy(data->proppatchShortValue+pos2,"\0",1);
					}
				}
			} else {
*/				/* end tag starts in current buffer -> read property value from there */
				/* TODO: for long property values the two existing buffer chunks are not
				         enough -> do something about it!!! */
				/* for this case read til property value start pos or til buffer start of
				   previous buffer */
				/* start pos of property value in previous buffer */
/*				pos3 = data->proppatchValueStartPos - ((data->bufferChunks - 1) * WDVH_MAX_XML_BUFFER_LEN);
*/				/* check wether property value start pos is in current buffer */
/*				if (data->proppatchValueStartPos >= startPosCurrBuff) {
*/					/* read til property value start pos */
/*					if (pos1-pos3 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						SAPDB_memcpy(data->proppatchShortValue,data->currentBuff+pos3+1,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
						SAPDB_memcpy(data->proppatchShortValue+pos1-pos3,"\0",1);
					} else {
						SAPDB_memcpy(data->proppatchShortValue,data->currentBuff+pos3+1,pos1-pos3);
						SAPDB_memcpy(data->proppatchShortValue+pos1-pos3,"\0",1);
					}
				} else {
*/					/* read first part of property value from previous buffer */
/*					if (-pos3+pos1 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						if (-pos3 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
							SAPDB_memcpy(data->proppatchShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
							SAPDB_memcpy(data->proppatchShortValue+WDVH_MAX_PROPERTY_SHORT_VALUE_LEN,"\0",1);
						} else {
							memcpy(data->proppatchShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,-pos3);
*/							/* read til buffer start */
/*							SAPDB_memcpy(data->proppatchShortValue-pos3-1,
								   data->currentBuff,
								   WDVH_MAX_PROPERTY_SHORT_VALUE_LEN+pos3);
							SAPDB_memcpy(data->proppatchShortValue+WDVH_MAX_PROPERTY_SHORT_VALUE_LEN,"\0",1);
						}
					} else {
						SAPDB_memcpy(data->proppatchShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,-pos3);
*/						/* read til buffer start */
/*						SAPDB_memcpy(data->proppatchShortValue-pos3-1,data->currentBuff,pos1);
						SAPDB_memcpy(data->proppatchShortValue+pos1-pos3,"\0",1);
					}
				}
			}
		}
*/	}

	/*------------------------------------------------------------------------
		Function:	getPropertyShortValueStartPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800
	--------------------------------------------------------------------------*/
	void getPropertyShortValueStartPos(WDVH_XmlUserData data, WDVH_Char *el) 
	{
		WDVH_Char			*temp1,*temp2,*temp3;
		WDVH_Int4			pos0,pos1;
		XML_Parser			p;

		if (data==NULL || el==NULL)
			return;

		p = xmlParserUserDataGetParser(data);
		/* called from start tag handler after discovery of a owner start tag */
		/* check wether end tag is in current parse buffer */
		/* pointer to start of current tag */
		pos0 = XML_GetCurrentByteIndex(p);
		/* if start of current tag is located in the previous handled buffer chunk,
		   the tag has to be build of two chunks one located in previous and one located in
		   current buffer */
		pos1 = pos0 - data->bufferStart;
		if (pos1 < 0) {
			/* - find closing ">" */
			temp1 = data->currentBuff;
			if (temp1) {
				temp2 = strchr(temp1,'>');
				if (temp2) {
					data->propertyValueStartPos = pos0 + (-pos1) + temp2 - temp1 - data->bufferStart + 1;
				}
			}
		} else {
			/* - find closing ">" of tag owner */
			temp1 = data->currentBuff+pos1+1;
			if (temp1) {
				temp2 = strchr(temp1,'>');
				if (temp2) {
		            data->propertyValueStartPos = pos0 + (temp2 - temp1 + 1) - data->bufferStart + 1;
				}
			}
		}
		/* check whether the closing tag is completely in this buffer */
		temp1 = data->currentBuff+data->propertyValueStartPos;
		temp2 = strchr(temp1,'<');
		if (temp2) {
			/* closing tag starts in this buffer */
			temp3 = strchr(temp2,'>');
			if (!temp3) {
				/* but it does not finish in this buffer -> set the end pos here, it wouldn't be set in the end tag handler */
				data->propertyValueEndPos = temp2 - data->currentBuff;
			}
		}
			
	}

	/*------------------------------------------------------------------------
		Function:	getPropertyShortValueEndPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800
	--------------------------------------------------------------------------*/
	void getPropertyShortValueEndPos(WDVH_XmlUserData data, WDVH_Char *el) 
	{
		WDVH_Int4			pos0;
		XML_Parser			p;

		if (data==NULL || el==NULL)
			return;

		p = xmlParserUserDataGetParser(data);
		/* called from start tag handler after discovery of a owner start tag */
		/* check wether end tag is in current parse buffer */
		/* pointer to start of current tag */
		pos0 = XML_GetCurrentByteIndex(p);
		if (pos0 > data->bufferStart)
			data->propertyValueEndPos = pos0 - data->bufferStart;
		else
			data->propertyValueEndPos = 0;
	}

