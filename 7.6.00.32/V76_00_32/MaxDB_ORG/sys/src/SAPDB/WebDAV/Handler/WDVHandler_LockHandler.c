/****************************************************************************

  module      : WDVHandler_LockHandler.c

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

	
	#include "SAPDB/WebDAV/Handler/WDVHandler_LockHandler.h"
	#include "SAPDBCommon/SAPDB_string.h"


	/*------------------------------------------------------------------------
		Function: lockCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 lockCallCapiFunc(sapdbwa_Handle		wa,
   								  WDVCAPI_WDV			pCapiHandle,
								  sapdbwa_HttpRequestP	request,
						 		  WDVCAPI_URI			requesturi,
								  WDVH_CapiUserData		userData,
								  WDVH_Char				*errmsg)		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
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
		if (request==NULL || pCapiHandle==NULL || requesturi==NULL || userData==NULL || errmsg==NULL) {
			sapdbwa_WriteLogMsg(wa, "lockCallCapiFunc:Uninitialized Pointers\n");
			return ERROR_500;
		}

		/* get server string for error messages */
		buildServerString(request,&server);
		
		success = WDVCAPI_Lock(pCapiHandle,
				 			   requesturi,
							   userData->depth,
							   userData->timeout,
							   userData->lockScope,
							   userData->lockType,
							   userData->lockOwner,
							   userData->lockId,
							   lockErrorCallBack,
							   (void*)userData );

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
						sp77sprintf(errorText,1000,ERROR_LOCK_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_LOCK_500,errorMsg);
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
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								error = ERROR_423;
								break;
							/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							case WDVCAPI_ERR_CODE_PARENT_DOESNT_EXIST:
								error = ERROR_409;
								break;
							case WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION:
								{
									error = ERROR_403;
									sp77sprintf(errmsg,1000,"At least one parent is no collection.");
									break;
								}
							/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							default:
								{
									error = ERROR_500;
									WDVCAPI_GetErrorText(errorItem,&errorMsg);
									sp77sprintf(errorText,1000,ERROR_LOCK_URI,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_LOCK_500,errorMsg);
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
						sp77sprintf(errorText,1000,ERROR_LOCK_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_LOCK_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		} else {
			error = ERROR_200;
		}

		return error;

	}

	/*------------------------------------------------------------------------
		Function:	lockBuildReply
	--------------------------------------------------------------------------*/
	void lockBuildReply(sapdbwa_Int2		statuscode, 
						sapdbwa_HttpReplyP	reply,
						WDVH_CapiUserData	userData,
						WDVCAPI_URI			requesturi,
						WDVH_Char			*errmsg)				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
	{

		WDVH_Char		timeout[50];
		WDVH_Char		locktoken[100];
		WDVH_Char		contentlength[10];

		if (reply==NULL || requesturi==NULL || userData==NULL || errmsg==NULL) {
			sendErrorReply(ERROR_500, reply, WEBDAV_LOCK,"");
			return;
		}

		/* if there are errors at the subordinated resources a header was asent allready. 
		   in this case only send the footer of the body.
		   otherwise the error occured at the root resource and no body has to be sent */
		if (userData->errorsOccured == WDVH_False) {
			switch (statuscode) {
				case ERROR_200:
					{
						/* initialize Response Header */
						sapdbwa_InitHeader(reply,
										statuscode,
										"text/xml; charset=\"utf-8\"",
										NULL,
										NULL,								/* const char    *LastModified,*/
										NULL,								/* const char    *Expires,*/
										NULL);								/* const char    *Location );*/

						sp77sprintf(locktoken,100,"<saplocktoken:%s>",userData->lockId);
						sapdbwa_SetHeader( reply,
										"Lock-Token",
										locktoken );

						sapdbwa_SetHeader( reply,
							"Transfer-Encoding",
							"chunked" );

						sapdbwa_SendHeader(reply);

						/* send lockdiscovery property as body for the reply */
						/* applies to successfull LOCK and lock refresh */
						/* XML Header */
						WDVH_sendBodyChunk(reply,
										 WDVH_XML_HEADER);
						/* <D:prop xmlns:D="DAV:"> */
						WDVH_sendBodyChunk(reply,
										 WDVH_PROP_OPEN_XMLNS);
						/*		<D:lockdiscovery> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKDISCOVERY_OPEN);
						/*			<D:activelock> */
						WDVH_sendBodyChunk(reply,
										 WDVH_ACTIVELOCK_OPEN);
						/*				<D:locktype> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKTYPE_OPEN);
						/*					value (get locktype from CAPI) */
						if (userData->lockType == WDVH_LOCKTYPE_WRITE)
							WDVH_sendBodyChunk(reply,
											 WDVH_WRITE_EMPTY);
						/*				</D:locktype> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKTYPE_CLOSE);
						/*				<D:lockscope> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKSCOPE_OPEN);
						/*					value (get lockscope from CAPI) */
						if (userData->lockScope == WDVCAPI_LOCK_SCOPE_EXCLUSIVE)
							WDVH_sendBodyChunk(reply,
											 WDVH_EXCLUSIVE_EMPTY);
						else if (userData->lockScope == WDVH_LOCKSCOPE_SHARED)
							WDVH_sendBodyChunk(reply,
											 WDVH_SHARED_EMPTY);
						/*				</D:lockscope> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKSCOPE_CLOSE);
						/*				<D:depth> */
						WDVH_sendBodyChunk(reply,
										 WDVH_DEPTH_OPEN);
						/*					value */
						if (userData->depth == WDVCAPI_DEPTH_0)
							WDVH_sendBodyChunk(reply,
											 WDVH_DEPTH_0);
						else if (userData->depth == WDVCAPI_DEPTH_INFINITE)
							WDVH_sendBodyChunk(reply,
											 WDVH_DEPTH_INFINITY);
						/*				</D:depth> */
						WDVH_sendBodyChunk(reply,
										 WDVH_DEPTH_CLOSE);
						/*				<D:owner> */
						WDVH_sendBodyChunk(reply,
										 WDVH_OWNER_OPEN);
						/*					value */
						WDVH_sendBodyChunk(reply,
										 userData->lockOwner);
						/*				</D:owner> */
						WDVH_sendBodyChunk(reply,
										 WDVH_OWNER_CLOSE);
						/*				<D:timeout> */
						WDVH_sendBodyChunk(reply,
										 WDVH_TIMEOUT_OPEN);
						/*					value */
						/* if (userData->timeout == */
						sp77sprintf(timeout,50,"Second-%d",userData->timeout);
						WDVH_sendBodyChunk(reply,
										 timeout);
						/*				</D:timeout> */
						WDVH_sendBodyChunk(reply,
										 WDVH_TIMEOUT_CLOSE);
						/*				<D:locktoken> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKTOKEN_OPEN);
						/*					<D:href> */
						WDVH_sendBodyChunk(reply,
										 WDVH_HREF_OPEN);
						/*						value */
						sp77sprintf(locktoken,100,"saplocktoken:%s",userData->lockId);
						WDVH_sendBodyChunk(reply,
										 locktoken);
						/*					</D:href> */
						WDVH_sendBodyChunk(reply,
										 WDVH_HREF_CLOSE);
						/*				</D:locktoken> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKTOKEN_CLOSE);
						/*			</D:activelock> */
						WDVH_sendBodyChunk(reply,
										 WDVH_ACTIVELOCK_CLOSE);
						/*		</D:lockdiscovery> */
						WDVH_sendBodyChunk(reply,
										 WDVH_LOCKDISCOVERY_CLOSE);
						/*	</D:prop> */
						WDVH_sendBodyChunk(reply,WDVH_PROP_CLOSE);
						/* Send empty chunk as eof */
						WDVH_sendBodyChunkEnd(reply);
						break;
					}
				case ERROR_400: 
					{
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
				case ERROR_409: 
					{
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
					}
				/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124043 */
				case ERROR_409_OWNER:								
						sapdbwa_InitHeader(reply,
										   ERROR_409,
										   "text/html",
										   NULL,
										   NULL,					/* const char    *LastModified,*/
										   NULL,					/* const char    *Expires,*/
										   NULL);					/* const char    *Location );*/

						sapdbwa_SetHeader( reply,
										   "DAV",
										   "1" );

						sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_409_OWN)+strlen(errmsg)+strlen(ERROR_PAGE_FOOTER));
						sapdbwa_SetHeader( reply,
										"Content-Length",
										contentlength);

						sapdbwa_SendHeader(reply);
						
						sapdbwa_SendBody(reply,
										ERROR_PAGE_409_OWN,
										0);
						sapdbwa_SendBody(reply,
										errmsg,
										0);
						sapdbwa_SendBody(reply,
										ERROR_PAGE_FOOTER,
										0);
						break; 
				/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124043 */
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

						sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_403));
						sapdbwa_SetHeader( reply,
										"Content-Length",
										contentlength);

						sapdbwa_SendHeader(reply);
						
						sapdbwa_SendBody(reply,
										ERROR_PAGE_403,
										0);
						sapdbwa_SendBody(reply,
										errmsg,
										0);
						sapdbwa_SendBody(reply,
										ERROR_PAGE_FOOTER,
										0);
						break; 
				/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */
				case ERROR_405:
					{
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

						sp77sprintf(contentlength,9,"%d",strlen(ERROR_PAGE_405));
						sapdbwa_SetHeader( reply,
										"Content-Length",
										contentlength);

						sapdbwa_SendHeader(reply);
						
						sapdbwa_SendBody(reply,
										 ERROR_PAGE_405,
										 strlen(ERROR_PAGE_405));
						break; 
					}
				case ERROR_423: 
					{
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
				case ERROR_500:
					{
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
		} else {
			/* there is at least one error occured on an resource different from the root 
			   resource. In this case the error call back function sent the reply. Only the
			   closing tag for multistatus is missing now */
			WDVH_sendBodyChunk(reply,WDVH_MULTI_STATUS_CLOSE);
			WDVH_sendBodyChunkEnd(reply);
		}
	}

	/*------------------------------------------------------------------------
		Function:	lockParseRequestBody
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 lockParseRequestBody(sapdbwa_Handle		wa,
									  WDVCAPI_WDV			wdv,
									  sapdbwa_HttpRequestP	request,
									  WDVH_CapiUserData		capiUserData,
									  WDVH_Char				*errmsg)	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
	{

	    WDVH_Int2					done;
		WDVH_Int2					pos;
		WDVH_XmlLockParserState		parserState;
		WDVH_XmlUserData			newUserData;
		WDVH_Error					error = ERROR_200;
		WDVH_UInt4					contentLength=0;
		WDVH_UInt4					bytesRead=0;
		WDVH_Char					errorText[1000];
		WDVH_Char					requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Char					*host;
		WDVH_Char					*port;
		/* WDVH_XmlBuffer				buff1;						http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		/* WDVH_XmlBuffer				buff2;						http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		WDVH_XmlBuffer				currentBuffer;
		/* WDVH_XmlBufferRef			previousBuffer;				http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		/* WDVH_XmlBufferRef			tempBuffer;					http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		/* WDVH_UInt4					bufferChunks;				http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		XML_Parser					p;
		WDVH_Char					*server;

		if (wa==NULL)
			return ERROR_500;
		if (wdv==NULL || request==NULL || capiUserData==NULL || errmsg==NULL) {
			sapdbwa_WriteLogMsg(wa, "lockParseRequestBody:Uninitialized Pointers\n");
			return ERROR_500;
		}

		/* get server string for error messages */
		buildServerString(request,&server);
		
		/* Initialization */
		p =  XML_ParserCreateNS("UTF-8",WDVH_NS_SEPARATOR);
		if (!p) {
			return ERROR_500;
		}
		/* bufferChunks = 0;										http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		/* currentBuffer = buff2;									http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		/* previousBuffer = buff1;									http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		strcpy(currentBuffer,"");
		/* strcpy(previousBuffer,"");								http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);

		/* Attention: previousBuffer and currentBuffer are swaped later on, so give 
		              previousBuffer as currentBuffer to user data! */
		xmlParserCreateUserData(p,currentBuffer,NULL,wdv,&newUserData);			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123800 */
		xmlParserCreateLockParserState(&parserState);
		xmlParserUserDataSetParserState(newUserData,(WDVH_XmlParserState)parserState);
		XML_SetUserData(p,(void*)newUserData);
		XML_SetElementHandler(p, 
							  lockXmlTagStartHandler,
							  lockXmlTagEndHandler);
		XML_SetParamEntityParsing(p,XML_PARAM_ENTITY_PARSING_ALWAYS);
		
		/* loop at all chunks of the incomming XML document and parse the XML */
		do {
			/* bufferChunks++;										http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			/* tempBuffer = previousBuffer;							http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			/* previousBuffer = currentBuffer;						http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			/* currentBuffer = tempBuffer;							http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			/* xmlParserUserDataSetParseBuffer(newUserData,currentBuffer,previousBuffer,bufferChunks);		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			/* get content length to determine end of read loop
			   the recv in sapdbwa_ReadBody is a blocking one, so it has to be avoided
			   that one single bytes is read beyond the real existing content length of
			   the body */
			if (sapdbwa_GetContentLength(request)) {
				contentLength = atoi(sapdbwa_GetContentLength(request));
			}
			if (contentLength > 0) {
				pos = sapdbwa_ReadBody( request, currentBuffer, WDVH_MAX_XML_BUFFER_LEN-1 );
				if (strcmp(currentBuffer,"")==0) {
					/* its only a bad request if there is no locktoken given */
					/* ----> TODO: Abfrage nach lock token */
					sp77sprintf(errorText,1000,ERROR_LOCK_URI,server,(char*)sapdbwa_GetRequestURI(request));
					sapdbwa_WriteLogMsg(wa, errorText);
					sp77sprintf(errorText,1000,ERROR_LOCK_MISSING_BODY);
					sapdbwa_WriteLogMsg(wa, errorText);
					error = ERROR_400;
					done = 1;
				}
			} else {
				strcpy(currentBuffer,"");
				/* its only a bad request if there is no locktoken given */
				/* ----> TODO: Abfrage nach lock token */
				sp77sprintf(errorText,1000,ERROR_LOCK_URI,server,(char*)sapdbwa_GetRequestURI(request));
				sapdbwa_WriteLogMsg(wa, errorText);
				sp77sprintf(errorText,1000,ERROR_LOCK_MISSING_BODY);
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
				if (newUserData->bufferEnd > 0)							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					newUserData->bufferStart = newUserData->bufferEnd+1;/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				else													/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					newUserData->bufferStart = 0;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				newUserData->bufferEnd = bytesRead-1;					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				newUserData->propertyValueStartPos = 0;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				newUserData->propertyValueEndPos = pos;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				if (! XML_Parse(p, currentBuffer, pos, done)) {
					pos = XML_GetCurrentByteIndex(p);
					sp77sprintf(errorText,1000,ERROR_LOCK_PARSE_1,server,(char*)sapdbwa_GetRequestURI(request));
					sapdbwa_WriteLogMsg(wa, errorText);
					sp77sprintf(errorText,1000,ERROR_LOCK_PARSE_2,XML_GetCurrentLineNumber(p),XML_ErrorString(XML_GetErrorCode(p)));
					sapdbwa_WriteLogMsg(wa, errorText);
					error = ERROR_400;
					done = 1;
				}
				if (newUserData->propertyShortValueStarted) {															/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					if (newUserData->propertyValueEndPos != WDVH_MAX_XML_BUFFER_LEN)									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
						SAPDB_memcpy(currentBuffer+newUserData->propertyValueEndPos,"\0",1);									/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					if (newUserData->propertyValueEndPos+newUserData->bufferStart < newUserData->bufferEnd)				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
						newUserData->propertyShortValueStarted = WDVH_False;											/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					if ((getStrBufferLength(newUserData->propertyShortValue) +
						newUserData->propertyValueEndPos - newUserData->propertyValueStartPos + 1) >WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
						done = 1;																					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
						error = ERROR_409_OWNER;																	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
						sp77sprintf(errmsg,1000,"Lock owner exceeds	supported length of %d characters.",WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					} else {																						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
						appendStrBuffer(newUserData->propertyShortValue,currentBuffer+newUserData->propertyValueStartPos);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					}																								/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				}
			} else 
				done = 1;
		} while(!done);

		if (error != ERROR_400 && error != ERROR_409_OWNER) {						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
			parserState = xmlParserUserDataGetLockParserState(newUserData);
			if (parserState->error == WDVH_XML_PARSER_OK)
				error = ERROR_200;
			else {
				error = ERROR_400;
					sp77sprintf(errorText,1000,ERROR_LOCK_PARSE_1,server,(char*)sapdbwa_GetRequestURI(request));
				sapdbwa_WriteLogMsg(wa, errorText);
			}
		}

		
		/* Clean Up */
		xmlParserDestroyLockParserState((WDVH_XmlLockParserState)newUserData->parserState);
		if (error == ERROR_200) {											/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
			strcpy(capiUserData->lockOwner,getStrBufferContent(newUserData->propertyShortValue));
			capiUserData->lockScope = newUserData->lockscope;
			capiUserData->lockType = newUserData->locktype;
		}																	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
		xmlParserDestroyUserData(newUserData);
		XML_ParserFree(p);
		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	lockXmlTagStartHandler
	--------------------------------------------------------------------------*/
	void lockXmlTagStartHandler(void *data, const char *el, const char **attr) {

		/* DTD Lock Body
			<!ELEMENT lockinfo (lockscope | locktype | owner?) >
			<!ELEMENT lockscope (exclusive | shared ) >
			<!ELEMENT locktype ( write ) >
			<!ELEMENT owner ANY>

		*/
		WDVH_Char						ns[WDVH_MAX_NAME_SPACE_LEN+1];
		WDVH_Char						tag[WDVH_MAX_PROPERTY_NAME_PREFIX_LEN+1];
		WDVH_Char						cmptag[WDVH_MAX_PROPERTY_NAME_PREFIX_LEN+3];
		WDVH_XmlLockParserState			parserState;
		WDVCAPI_WDV						capiWdv;
		WDVH_Char						errorText[1000];
		WDVH_XmlUserData				userData;
		XML_Parser						p;

		if (data==NULL || el==NULL || attr==NULL) 
			return;

		userData = (WDVH_XmlUserData)data;
		p = xmlParserUserDataGetParser(userData);
		parserState = xmlParserUserDataGetLockParserState(userData);
		capiWdv = xmlParserUserDataGetWDV(userData);

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

			if (parserState->depth == 2 &&
				parserState->state == WDVH_LOCK_DFA_STATE_6) {
				/* here comes a lockscope value tag (<exclusive/> or <shared/>) */
				if (strcmp(tag,WDVH_XML_EXCLUSIVE_OPEN)==0) {
					/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
					/*if (xmlParserCheckEmptyTag(p,userData,WDVH_EMPTY_TAG_EXCLUSIVE)) {*/
						/* do something about the exclusive lock scope */
						/*userData->lockscope = WDVCAPI_LOCK_SCOPE_EXCLUSIVE;*/
						/* and go to next parser state*/
						/*parserState->state = WDVH_LOCK_DFA_STATE_7;*/
						parserState->state = WDVH_LOCK_DFA_STATE_13;
						parserState->depth++;
					/*} else {
						parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
					}*/
					/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
				} else if (strcmp(tag,WDVH_XML_SHARED_OPEN)==0) {
					/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
					/*if (xmlParserCheckEmptyTag(p,userData,WDVH_EMPTY_TAG_SHARED)) {*/
						/* do something about the shared lock scope */
						/*userData->lockscope = WDVCAPI_LOCK_SCOPE_UNDEFINED;*/
						/* and go to next parser state*/
						/*parserState->state = WDVH_LOCK_DFA_STATE_7;*/
						parserState->state = WDVH_LOCK_DFA_STATE_14;
						parserState->depth++;
					/*} else {
						parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
					}*/
					/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 2 &&
				parserState->state == WDVH_LOCK_DFA_STATE_3) {
				/* here comes a locktype empty tag. right now there is only <write/> */
				if (strcmp(tag,WDVH_XML_WRITE_OPEN)==0) {
					/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
					/*if (xmlParserCheckEmptyTag(p,userData,WDVH_EMPTY_TAG_WRITE)) {
						userData->locktype = WDVCAPI_LOCK_TYPE_WRITE;
						parserState->state = WDVH_LOCK_DFA_STATE_4;
					} else {*/
						/*parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;*/
						userData->locktype = WDVCAPI_LOCK_TYPE_WRITE;
						parserState->state = WDVH_LOCK_DFA_STATE_12;
						parserState->depth++;
					/*}*/
					/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 1 &&
				parserState->state == WDVH_LOCK_DFA_STATE_10) {
				/* if there comes a start tag it could be locktype or lockscope */
				if (strcmp(tag,WDVH_XML_LOCKSCOPE_OPEN)==0 &&
					!parserState->lockscope) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_6;
					parserState->lockscope = WDVH_True;
				} else if (strcmp(tag,WDVH_XML_LOCKTYPE_OPEN)==0 &&
					!parserState->locktype) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_3;
					parserState->locktype = WDVH_True;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 1 &&
				parserState->state == WDVH_LOCK_DFA_STATE_8) {
				/* if there comes a start tag it could be locktype or owner */
				if (strcmp(tag,WDVH_XML_OWNER_OPEN)==0 &&
					!parserState->owner) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_9;
					parserState->owner = WDVH_True;
					/* collect value of the property */
					userData->propertyShortValueStarted = WDVH_True;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					lockOwnerShortValueStartPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				} else if (strcmp(tag,WDVH_XML_LOCKTYPE_OPEN)==0 &&
					!parserState->locktype) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_3;
					parserState->locktype = WDVH_True;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 1 &&
				parserState->state == WDVH_LOCK_DFA_STATE_5) {
				/* if there comes a start tag it is a lockscope start tag */
				if (strcmp(tag,WDVH_XML_LOCKSCOPE_OPEN)==0 &&
					!parserState->lockscope) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_6;
					parserState->lockscope = WDVH_True;
				} else if (strcmp(tag,WDVH_XML_OWNER_OPEN)==0 &&
					!parserState->owner) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_9;
					parserState->owner = WDVH_True;
					/* collect value of the property */
					userData->propertyShortValueStarted = WDVH_True;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					lockOwnerShortValueStartPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 1 &&
				parserState->state == WDVH_LOCK_DFA_STATE_2) {
				/* if there comes a start tag it is a locktype start tag */
				if (strcmp(tag,WDVH_XML_LOCKTYPE_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_3;
					parserState->locktype = WDVH_True;
				} else if (strcmp(tag,WDVH_XML_LOCKSCOPE_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_6;
					parserState->lockscope = WDVH_True;
				} else if (strcmp(tag,WDVH_XML_OWNER_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_9;
					parserState->owner = WDVH_True;
					/* collect value of the property */
					userData->propertyShortValueStarted = WDVH_True;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					lockOwnerShortValueStartPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

			if (parserState->depth == 0 &&
				parserState->state == WDVH_LOCK_DFA_STATE_1) {
				/* if there comes a start tag it is a lockinfo start tag */
				if (strcmp(tag,WDVH_XML_LOCKINFO_OPEN)==0) {
					parserState->depth++;
					parserState->state = WDVH_LOCK_DFA_STATE_2;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			}

		}

	}  /* End of start handler */

	/*------------------------------------------------------------------------
		Function:	lockXmlTagEndHandler
	--------------------------------------------------------------------------*/
	void lockXmlTagEndHandler(void *data, const char *el) {
		
		WDVH_Char					ns[WDVH_MAX_NAME_SPACE_LEN+1];
		WDVH_Char					tag[WDVH_MAX_PROPERTY_NAME_PREFIX_LEN+1];
		WDVH_XmlLockParserState		parserState;
		WDVH_XmlUserData			userData;

		if (data==NULL || el==NULL)
			return;

		userData = (WDVH_XmlUserData)data;
		parserState = xmlParserUserDataGetLockParserState(userData);

		xmlParserCutNamespace(el,ns,tag,WDVH_NS_SEPARATOR);

		if (parserState->error == WDVH_XML_PARSER_OK) {

			if (parserState->depth == 1 &&
				parserState->state == WDVH_LOCK_DFA_STATE_8) {
				if (strcmp(tag,WDVH_XML_LOCKINFO_CLOSE)==0 &&
					parserState->lockscope && parserState->locktype && parserState->owner) {
					parserState->depth--;
					parserState->state = WDVH_LOCK_DFA_STATE_11;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			} 

			if (parserState->depth == 1 &&
				parserState->state == WDVH_LOCK_DFA_STATE_10) {
				if (strcmp(tag,WDVH_XML_LOCKINFO_CLOSE)==0 &&
					parserState->lockscope && parserState->locktype && parserState->owner) {
					parserState->depth--;
					parserState->state = WDVH_LOCK_DFA_STATE_11;
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			} 

			if (parserState->depth == 1 &&
				parserState->state == WDVH_LOCK_DFA_STATE_5) {
				if (strcmp(tag,WDVH_XML_LOCKINFO_CLOSE)==0 &&
					parserState->lockscope && parserState->locktype && parserState->owner) {
					parserState->depth--;
					/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
					/*parserState->state = WDVH_LOCK_DFA_STATE_13;*/
					parserState->state = WDVH_LOCK_DFA_STATE_11;
					/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
				} else {
					parserState->error = WDVH_XML_PARSER_SYNTAX_ERROR;
				}
			} 

			if (parserState->depth == 2 &&
				parserState->state == WDVH_LOCK_DFA_STATE_9) {
				if (strcmp(tag,WDVH_XML_OWNER_CLOSE)==0) {
					/* collect value of owner tag */
					lockOwnerShortValueEndPos(userData,tag);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
					/* reset user data for value collection */
					/* userData->propertyShortValueStarted = WDVH_False; 
					userData->propertyValueStartPos = 0;
					userData->propertyValueEndPos = 0; */
					parserState->depth--;
					parserState->state = WDVH_LOCK_DFA_STATE_10;
				}
			} 

			if (parserState->depth == 2 &&
				parserState->state == WDVH_LOCK_DFA_STATE_7) {
				if (strcmp(tag,WDVH_XML_LOCKSCOPE_CLOSE)==0) {
					parserState->depth--;
					parserState->state = WDVH_LOCK_DFA_STATE_8;
				}
			} 

			if (parserState->depth == 2 &&
				parserState->state == WDVH_LOCK_DFA_STATE_4) {
				if (strcmp(tag,WDVH_XML_LOCKTYPE_CLOSE)==0) {
					parserState->depth--;
					parserState->state = WDVH_LOCK_DFA_STATE_5;
				}
			} 

			/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
			if (parserState->depth == 3 &&
				parserState->state == WDVH_LOCK_DFA_STATE_12) {
				if (strcmp(tag,WDVH_XML_WRITE_CLOSE)==0) {
					/* do something about the write lock type */
					userData->locktype = WDVCAPI_LOCK_TYPE_WRITE;
					parserState->state = WDVH_LOCK_DFA_STATE_4;
					parserState->depth--;
				}
			} 

			if (parserState->depth == 3 &&
				parserState->state == WDVH_LOCK_DFA_STATE_13) {
				if (strcmp(tag,WDVH_XML_EXCLUSIVE_CLOSE)==0) {
					/* do something about the exclusive lock scope */
					userData->lockscope = WDVCAPI_LOCK_SCOPE_EXCLUSIVE;
					parserState->state = WDVH_LOCK_DFA_STATE_7;
					parserState->depth--;
				}
			} 
			
			if (parserState->depth == 3 &&
				parserState->state == WDVH_LOCK_DFA_STATE_14) {
				if (strcmp(tag,WDVH_XML_SHARED_CLOSE)==0) {
					/* do something about the shared lock scope */
					userData->lockscope = WDVCAPI_LOCK_SCOPE_UNDEFINED;
					parserState->state = WDVH_LOCK_DFA_STATE_7;
					parserState->depth--;
				}
			} 
			/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122129 */
		}
	}  /* End of end handler */

	/*------------------------------------------------------------------------
		Function:	lockReadOwnerShortValue
		commented
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043
	--------------------------------------------------------------------------*/
	void lockReadOwnerShortValue(WDVH_XmlUserData data, WDVH_Char *el) {

/*		WDVH_Char			tag[WDVH_MAX_PROPERTY_NAME_PREFIX_LEN+WDVH_MAX_NAME_SPACE_LEN+2+1];
		WDVH_Char			*pTag;
		WDVH_Char			*temp1,*temp2;
		WDVH_Int4			pos0,pos1,pos2,pos3,taglen;
		WDVH_Int4			startPosPrevBuff,startPosCurrBuff;
		XML_Parser			p;

		p = xmlParserUserDataGetParser(data);

		if (!data->propertyShortValueStarted) {
*/			/* called from start tag handler after discovery of a owner start tag */
			/* check wether end tag is in current parse buffer */
			/* pointer to start of current tag */
/*			pos0 = XML_GetCurrentByteIndex(p);
*/			/* if start of current tag is located in the previous handled buffer chunk,
			   the tag has to be build of two chunks one located in previous and one located in
			   current buffer */
/*			pos1 = pos0 - ((data->bufferChunks - 1) * WDVH_MAX_XML_BUFFER_LEN);
			if (pos1 < 0) {
*/				/* get the remaining part from current buffer */
				/* - find closing ">" */
/*				temp1 = data->currentBuff;
				if (temp1) {
					temp2 = strchr(temp1,'>');
					if (temp2) {
						data->propertyValueStartPos = pos0 + (-pos1) + temp2 - temp1;
						data->propertyShortValueStarted = WDVH_True;
					}
				}
			} else {
*/				/* get namespace abbrev of current tag from current buffer */
/*				temp1 = data->currentBuff+pos1+1;
				if (temp1) {
					temp2 = strchr(temp1,'>');
					if (temp2) {
			            data->propertyValueStartPos = pos0 + temp2 - temp1 + 1;
			            data->propertyShortValueStarted = WDVH_True;
					}
				}
			}
		} else {
*/			/* called from end tag handler after discovery of a property end tag */
			/* check wether end tag is in current parse buffer */
			/* pointer to start of current tag */
/*			pos0 = XML_GetCurrentByteIndex(p);
*/			/* end position of property value is one before start of end tag */
/*			data->propertyValueEndPos = pos0 - 1;
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
/*				pos3 = data->propertyValueStartPos - ((data->bufferChunks - 1) * WDVH_MAX_XML_BUFFER_LEN);
*/				/* for this case read til property value start pos or til buffer start */
				/* check wether property value start pos is in previuos buffer */
/*				if (data->propertyValueStartPos >= startPosPrevBuff) {
*/					/* read til property value start pos */
/*					if(-pos3+pos1-1 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						SAPDB_memcpy(data->lockShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
						SAPDB_memcpy(data->lockShortValue-pos3+pos1,"\0",1);
					} else {
						SAPDB_memcpy(data->lockShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,-pos3+pos1-1);
						SAPDB_memcpy(data->lockShortValue-pos3+pos1,"\0",1);
					}
				} else {
*/					/* read til buffer start */
/*					pos2 = pos0 - ((data->bufferChunks - 2)* WDVH_MAX_XML_BUFFER_LEN);
					if (pos2 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						SAPDB_memcpy(data->lockShortValue,data->previousBuff,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
						SAPDB_memcpy(data->lockShortValue+WDVH_MAX_PROPERTY_SHORT_VALUE_LEN-1,"\0",1);
					} else {
						SAPDB_memcpy(data->lockShortValue,data->previousBuff,pos2);
						SAPDB_memcpy(data->lockShortValue+pos2,"\0",1);
					}
				}
			} else {
*/				/* end tag starts in current buffer -> read property value from there */
				/* TODO: for long property values the two existing buffer chunks are not
				         enough -> do something about it!!! */
				/* for this case read til property value start pos or til buffer start of
				   previous buffer */
				/* start pos of property value in previous buffer */
/*				pos3 = data->propertyValueStartPos - ((data->bufferChunks - 1) * WDVH_MAX_XML_BUFFER_LEN);
*/				/* check wether property value start pos is in current buffer */
/*				if (data->propertyValueStartPos >= startPosCurrBuff) {
*/					/* read til property value start pos */
/*					if (pos1-pos3 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						SAPDB_memcpy(data->lockShortValue,data->currentBuff+pos3+1,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
						SAPDB_memcpy(data->lockShortValue+WDVH_MAX_PROPERTY_SHORT_VALUE_LEN-1,"\0",1);
					} else {
						SAPDB_memcpy(data->lockShortValue,data->currentBuff+pos3+1,pos1-pos3);
						SAPDB_memcpy(data->lockShortValue+pos1-pos3,"\0",1);
					}
				} else {
*/					/* read first part of property value from previous buffer */
/*					if (-pos3+pos1 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
						if (-pos3 > WDVH_MAX_PROPERTY_SHORT_VALUE_LEN) {
							SAPDB_memcpy(data->lockShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,WDVH_MAX_PROPERTY_SHORT_VALUE_LEN);
							SAPDB_memcpy(data->lockShortValue+WDVH_MAX_PROPERTY_SHORT_VALUE_LEN-1,"\0",1);
						} else {
							SAPDB_memcpy(data->lockShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,-pos3);
*/							/* read til buffer start */
/*							SAPDB_memcpy(data->lockShortValue-pos3-1,
								   data->currentBuff,
								   WDVH_MAX_PROPERTY_SHORT_VALUE_LEN+pos3);
							SAPDB_memcpy(data->lockShortValue+WDVH_MAX_PROPERTY_SHORT_VALUE_LEN-1,"\0",1);
						}
					} else {
						SAPDB_memcpy(data->lockShortValue,data->previousBuff+WDVH_MAX_XML_BUFFER_LEN+pos3+1,-pos3);
*/						/* read til buffer start */
/*						SAPDB_memcpy(data->lockShortValue-pos3-1,data->currentBuff,pos1);
						SAPDB_memcpy(data->lockShortValue+pos1-pos3,"\0",1);
					}
				}
			}
		}
*/	}

	/*------------------------------------------------------------------------
		Function:	lockOwnerShortValueStartPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043
	--------------------------------------------------------------------------*/
	void lockOwnerShortValueStartPos(WDVH_XmlUserData data, WDVH_Char *el) 
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
		Function:	lockOwnerShortValueEndPos
		http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043
	--------------------------------------------------------------------------*/
	void lockOwnerShortValueEndPos(WDVH_XmlUserData data, WDVH_Char *el) 
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

	/*------------------------------------------------------------------------
		Function:	lockErrorCallBack
	--------------------------------------------------------------------------*/
	void lockErrorCallBack(void					*clientData, 
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

		if (clientData==NULL || uri==NULL || errorItem==NULL)
			return;

		userData = (WDVH_CapiUserData)clientData;

		/* at first call a header has to be sent */
		if (userData->lockReplyHeaderSent == WDVH_False) {
			sapdbwa_InitHeader(userData->reply,
			                   ERROR_207,
				               "text/xml",
					           NULL,
						       NULL,					/* const char    *LastModified,*/
							   NULL,					/* const char    *Expires,*/
							   NULL);					/* const char    *Location );*/

			sapdbwa_SetHeader( userData->reply,
		                       "DAV",
			                   "1" );

			sapdbwa_SetHeader( userData->reply,
				   "Transfer-Encoding",
				   "chunked" );

			sapdbwa_SendHeader(userData->reply);
			
			/* and the beginning of the xml body has to be sent too */
			/* send body header */
			WDVH_sendBodyChunk(userData->reply,WDVH_XML_HEADER);
			WDVH_sendBodyChunk(userData->reply,WDVH_MULTI_STATUS_OPEN);
			

			/* and error occured has to be set */
			userData->lockReplyHeaderSent = WDVH_True;
			userData->errorsOccured = WDVH_True;
		} 
		if (userData->lockReplyHeaderSent == WDVH_True) {
			/* it's the second, third, ... turn around */
			/* send the response part for this special uri */
			/* response */
			WDVH_sendBodyChunk(userData->reply,WDVH_RESPONSE_OPEN);
			WDVH_sendBodyChunk(userData->reply,WDVH_XML_CLOSE_TAG);
			/*		href */
			WDVH_sendBodyChunk(userData->reply,WDVH_HREF_OPEN);
			/*			server */
			WDVH_sendBodyChunk(userData->reply,userData->server);
			/*			slash */
			WDVH_sendBodyChunk(userData->reply, "/");
			/*			prefix */
			WDVH_sendBodyChunk(userData->reply, (WDVH_Char*)userData->prefix);
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
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								{
									WDVH_sendBodyChunk(userData->reply,WDVH_HTTP_1_1);
									WDVH_sendBodyChunk(userData->reply,ERROR_423_STR);
									WDVH_sendBodyChunk(userData->reply,ERROR_423_TEXT);
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
