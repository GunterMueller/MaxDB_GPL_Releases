/****************************************************************************

  module      : WDVHandler_Explorer.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  10:41
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_Explorer.h"
	#include "hwd21req.h"
	#include "SAPDBCommon/SAPDB_string.h"

	#define  WDVH_DELETED_ITEMS_FOLDER		"/Deleted Items"
	#define  WDVH_DELETED_ITEMS_FOLDER_ESC	"/Deleted%20Items"
	#define  WDVH_PUT_TO_XMLDB				"xmldb"
	#define  WDVH_IMPORT					"import"
	#define  WDVH_FILE_CONTENT_TYPE			"Content-Type"

    void inStreamHandlerExplorer( void           *userData,
                                  void           *buffer,
                                  SAPDB_Int4      bufferSize,
                                  SAPDB_Int4     *readLen );

    typedef struct st_wdvh_stream_desc {
		sapdbwa_HttpRequestP	request; 
		WDV_Bool				moreData;
		const WDVH_Char        *boundary;
		WDVH_Bool              *moreParts;
    } WDVHStreamDescExplorer;

	void outStreamHandlerExplorer( void *userData, void *buffer, SAPDB_Int4 writeLen );

	/*------------------------------------------------------------------------
		Function: doServiceExplorer
	--------------------------------------------------------------------------*/
	void doServiceExplorer(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,WDVCAPI_WDV pCapiHandle, WDVH_Handle wdvhHandle, XMLIMAPI_Handle xmlHandle, WDVH_Bool compress)
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Int2				statuscode;
		WDVCAPI_PropfindHandle  propfind=NULL;
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Char				*server;
		WDVH_Char				__command[100];
		WDVH_Char				__location[WDVH_MAX_URI_LEN+1];
		WDVH_Char				__resource[WDVH_MAX_URI_LEN+1];
		WDVH_Char				__destination[WDVH_MAX_URI_LEN+1];
		WDVH_Char				__docclass[129];
		WDVH_Char				*__pDocClass;
		WDVH_Char				__docclassold[129];
		WDVCAPI_Overwrite		__overwrite;
		WDVH_Char				__reindex[3];
		WDVCAPI_ErrorItem		__errorItem;
		WDVCAPI_ErrorType		__errorType;
		WDVCAPI_ErrorCode		__errorCode;
		WDVCAPI_ErrorText		__errorMsg;
		WDVH_Error				__error;
		WDVH_Char				__errorText[1000];
		WDVH_Bool				__errorsoccurred = WDVH_False;
		WDVH_Char				__locktoken[100];
		WDVCAPI_LockHandle		__lock;
		WDVH_CapiUserData		__capiUserData;
		WDVCAPI_LockIdList		__lockTokenList;
		WDVH_StringBuffer		__tempname;
		const WDVH_Char			*__collection;
        WDV_Bool                isDocInXDB = WDV_False;

		getParameterValue("cmd",req,__command);
		getParameterValue("location",req,__location);
		WDVH_UnEscapeUri(__location);
		getParameterValue("res",req,__resource);
		WDVH_UnEscapeUri(__resource);
		getRequestUriFromString(wa,req,__resource);
		if (strcmp(__command,"Del")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* delete the given resource */
			statuscode = deleteCallCapiFunc(wa,pCapiHandle,req,__resource,"",WDVH_True);
			if (statuscode==204) {
				/* just show the "new" collection */
				statuscode = getCallCapiFunc(wa,req,rep,pCapiHandle,__location,"",-1,-1,&isDocInXDB);
				getBuildReply(wa,statuscode,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
			} else {
				deleteBuildReply(statuscode, rep);
			}
		} else if (strcmp(__command,"Discard")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* delete the given resource */
			statuscode = deleteCallCapiFunc(wa,pCapiHandle,req,__resource,"",WDVH_False);
			if (statuscode==204) {
				/* just show the "new" collection */
				statuscode = getCallCapiFunc(wa,req,rep,pCapiHandle,__location,"",-1,-1,&isDocInXDB);
				getBuildReply(wa,statuscode,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
			} else {
				deleteBuildReply(statuscode, rep);
			}
		} else if (strcmp(__command,"RestoreScreen")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* build Reply */
			restoreScreen(wa,req,rep,__resource,__location,pCapiHandle,WDVH_False,"");
		} else if (strcmp(__command,"uploadFileScreen")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* build Reply */
			uploadFileScreen(wa,req,rep,__resource,__location,pCapiHandle,WDVH_False,"");
		} else if (strcmp(__command,"mkColScreen")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			__tempname = createStrBuffer(500);
			if (__tempname!=NULL) {
				/* extract Parameters (Host, RequestURI)*/
				appendStrBuffer(__tempname,"");
				/* build Reply */
				mkColScreen(wa,req,rep,"",__location,pCapiHandle,WDVH_False,"");
				destroyStrBuffer(__tempname);
			} else {
				/* get server string for error messages */
				buildServerString(req,&server);
				sp77sprintf(__errorText,1000,ERROR_MKCOL_URI,server,wdvhHandle->requesturi);
				sapdbwa_WriteLogMsg(wdvhHandle->wa, __errorText);
				sp77sprintf(__errorText,1000,ERROR_MKCOL_500,"WebDAV Explorer. Create Collection. Out of memory. Could not create StringBuffer.");
				sapdbwa_WriteLogMsg(wdvhHandle->wa, __errorText);
				statuscode = ERROR_500;
			}
		} else if (strcmp(__command,"ShowXQuery")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* build Reply */
			showXQueryScreen(wa,req,rep,__resource,__location,pCapiHandle,wdvhHandle,WDVH_False,"");
		} else if (strcmp(__command,"Restore")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			if (getParameterValue("Overwrite",req,__destination)) 
				__overwrite = WDVCAPI_DELETE_BEFORE_OVERWRITE;	/* no merge!!! */
			else 
				__overwrite = WDVCAPI_NO_OVERWRITE;
			getParameterValue("DestinationURI",req,__destination);
			/* get server string for error messages */
			buildServerString(req,&server);
			/* -If header (lock tokens) */
			if (!WDVCAPI_LockCreateIdList(pCapiHandle,&__lockTokenList)) {
				__lockTokenList = NULL;
			}
			/* - CapiUserData for call back */
			createCapiUserData(&__capiUserData);
			__capiUserData->reply = rep;
			__capiUserData->server = server;
			/* call CAPI function*/
			statuscode = moveCallCapiFunc(wa,req,pCapiHandle,__resource,__destination,WDVCAPI_DEPTH_INFINITE,__overwrite,__capiUserData,__lockTokenList);
			/* build Reply */
			if (statuscode==201 || statuscode==204) {
				/* o.k. */
				statuscode = getCallCapiFunc(wa,req,rep,pCapiHandle,__location,"",-1,-1,&isDocInXDB);
				getBuildReply(wa,statuscode,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
			} else {
				WDVCAPI_GetLastError(pCapiHandle,&__errorItem);
				WDVCAPI_GetErrorType(__errorItem,&__errorType);
				switch (__errorType) {
					case WDVCAPI_ERR_TYPE_UNDEFINED:
					case WDVCAPI_ERR_TYPE_SQL: 
						{
							/* internal server error */
							WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
							sp77sprintf(__errorText,1000,ERROR_MOVE_500,__errorMsg);
							break;
						}
					case WDVCAPI_ERR_TYPE_CAPI: 
						{
							WDVCAPI_GetErrorCode(__errorItem,&__errorCode);
							switch(__errorCode) {
								/* WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION 
								= Resource Uri and Destination Uri MUST NOT be equal */
								case WDVCAPI_ERR_CODE_EQUAL_RESOURCE_AND_DESTINATION:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,"ERROR: 403 (Forbidden). Restore location is the same as the current location.");
										break;
									}
								case WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,"ERROR: 404 (Not Found). Resource to restore does not exist.");
										break;
									}
									/* WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST 
										= the destination Uri contains at least one collection 
										that does not exist rigth now */
								case WDVCAPI_ERR_CODE_DESTINATION_URI_DOESNT_EXIST:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,"ERROR: 409 (Conflict). Destination does not exist. At least one part of the destination path is not valid.");
										break;
									}
									/*     WDVCAPI_ERR_CODE_DESTINATION_IS_CHILD_OF_SOURCE
										= the destination Uri is child of the source uri ->
										recursion occurs */
								case WDVCAPI_ERR_CODE_DESTINATION_IS_CHILD_OF_SOURCE:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,"ERROR: 409 (Conflict). Destination is child of Source.");
										break;
									}
								/* WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT 
									= overwrite was verbidden and destination allready exists */
								/*case WDVCAPI_ERR_CODE_OVERWRITE_CONFLICT:*/
								case WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_ALREADY_EXIST:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,"ERROR: 412 (Precondition Failed). Resource at destination already exists and overwriting was not allowed.");
										break;
									}
								case WDVCAPI_ERR_CODE_LOCK_EXISTS:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,"ERROR: 423 (Locked). Resource at destination is locked.");
										break;
									}
								/* WDVCAPI_ERR_CODE_NO_MEMORY
								= lack of memory */
								case WDVCAPI_ERR_CODE_NO_MEMORY:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,ERROR_MOVE_STORAGE_507,server,__resource,__destination);
										sapdbwa_WriteLogMsg(wa, __errorText);
										sp77sprintf(__errorText,1000,ERROR_MOVE_MESSAGE,__errorMsg);
										sapdbwa_WriteLogMsg(wa, __errorText);
										break;
									}
								default:
									{
										WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
										sp77sprintf(__errorText,1000,ERROR_MOVE_URI,server,__resource,__destination);
										sapdbwa_WriteLogMsg(wa, __errorText);
										sp77sprintf(__errorText,1000,ERROR_MOVE_500,__errorMsg);
										sapdbwa_WriteLogMsg(wa, __errorText);
										break;
									}
							}
							break;
						}
					default:
						{
							WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
							sp77sprintf(__errorText,1000,ERROR_MOVE_URI,server,__resource,__destination);
							sapdbwa_WriteLogMsg(wa, __errorText);
							sp77sprintf(__errorText,1000,ERROR_MOVE_500,__errorMsg);
							sapdbwa_WriteLogMsg(wa, __errorText);
							break;
						}
				}
				restoreScreen(wa,req,rep,__resource,__location,pCapiHandle,WDVH_True,__errorText);
			}
		} else if (strcmp(__command,"ShowProps")==0) {
			/* show properties of the given resource */
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* create Propfind handle */
			WDVCAPI_PropfindCreate(pCapiHandle,__resource,WDVCAPI_DEPTH_0,WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL,&propfind);
			/* call CAPI function*/
			statuscode = propfindCallCapiFunc(pCapiHandle,wa,req,propfind);
			if (statuscode==207) {
				/* build Reply */
				propfindBuildExplorerReply(wa,req,rep,__resource,__location,pCapiHandle,propfind);
			}
			/* Clean Up */
			/* - handles */
			if (propfind) {
				WDVCAPI_PropfindDestroy(pCapiHandle,propfind);
			}
		} else if (strcmp(__command,"ShowColProps")==0) {
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* create Propfind handle */
			WDVCAPI_PropfindCreate(pCapiHandle,__resource,WDVCAPI_DEPTH_0,WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL,&propfind);
			/* call CAPI function*/
			statuscode = propfindCallCapiFunc(pCapiHandle,wa,req,propfind);
			if (statuscode==207) {
				/* build Reply */
				collectionLockProps(wa,__resource,__location,pCapiHandle,propfind,__locktoken,&__lock);
			}
			/* Clean Up */
			/* - handles */
			if (propfind) {
				WDVCAPI_PropfindDestroy(pCapiHandle,propfind);
			}
			/* call CAPI function*/
			statuscode = getCallCapiFunc(wa,req,rep,pCapiHandle,__resource,"",-1,-1,&isDocInXDB);
			if (statuscode==200) {
				/* build Reply */
				getBuildExplorerReply(wa,req,rep,pCapiHandle,wdvhHandle,__resource,__location,__locktoken,__lock);
			}
		} else if (strcmp(__command,"ShowXmlProps")==0) {
			/* show properties of the given resource */
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* build Reply */
			showXmlProperties(wa,req,rep,__resource,__location,pCapiHandle,xmlHandle,WDVH_False,"");
		} else if (strcmp(__command,"Upload")==0) {
			/* get document from multipart request and put it to the webdav */
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* build Reply */
			if (uploadFile(wa,req,rep,__location,pCapiHandle,wdvhHandle,compress,&__errorsoccurred,__errorText))
				getBuildReply(wa,ERROR_200,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
			else
				uploadFileScreen(wa,req,rep,__resource,__location,pCapiHandle,__errorsoccurred,__errorText);
		} else if (strcmp(__command,"MkCol")==0) {
			__tempname = createStrBuffer(500);
			if (__tempname!=NULL) {
				/* extract Parameters (Host, RequestURI)*/
				getFirstHeaderLine(wa,req,&host,&port,requestURI);
				if (__location[strlen(__location)-1] == '/' || __location[strlen(__location)-1] == '\\')
					__location[strlen(__location)-1] = '\0';
				appendStrBuffer(__tempname,__location);
				appendStrBuffer(__tempname,"/");
				__collection = getParameterValueWithoutCopy("collection",req);
				if (__collection != NULL) {
					if (__collection[0] != '/' && __collection[0] != '\\')
						appendStrBuffer(__tempname,(WDVH_Char*)__collection);
					else
						appendStrBuffer(__tempname,(WDVH_Char*)__collection+1);
				}
				strcpy(wdvhHandle->requesturi,getStrBufferContent(__tempname));
				wdvhHandle->request = req;
				wdvhHandle->reply = rep;
				wdvhHandle->wa = wa;
				/* interpret If-Header */
				WDVCAPI_IdStringInitValue(wdvhHandle->locktoken);
				/* call CAPI function*/
				statuscode = mkcolCallCapiFunc(wdvhHandle,pCapiHandle,__errorText);
			} else {
				/* get server string for error messages */
				buildServerString(req,&server);
				sp77sprintf(__errorText,1000,ERROR_MKCOL_URI,server,wdvhHandle->requesturi);
				sapdbwa_WriteLogMsg(wdvhHandle->wa, __errorText);
				sp77sprintf(__errorText,1000,ERROR_MKCOL_500,"WebDAV Explorer. Create Collection. Out of memory. Could not create StringBuffer.");
				sapdbwa_WriteLogMsg(wdvhHandle->wa, __errorText);
				statuscode = ERROR_500;
			}
			/* build Reply */
			if (statuscode==201) {
				/* build Reply */
				getBuildReply(wa,ERROR_200,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
			} else {
				__errorsoccurred = WDVH_True;
				mkColScreen(wa,req,rep,(WDVH_Char*)__collection,__location,pCapiHandle,__errorsoccurred,__errorText);
			}
			if (__tempname !=  NULL)
				destroyStrBuffer(__tempname);
		} else if (strcmp(__command,"SetXmlProps")==0) {
			/* change xml properties of the given resource */
			/* get needed parameters from request */
			getParameterValue("DocClass",req,__docclass);
			getParameterValue("DocClassOld",req,__docclassold);
			getParameterValue("ReIndex",req,__reindex);
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* perform the change */
			if (strcmp(__docclass,__docclassold)!=0 || getParameterValue("ReIndex",req,__reindex)) {
				/* docclass changed */
				if (strcmp(__docclass,"")!=0)
					__pDocClass = __docclass;
				else
					__pDocClass = NULL;
				if (!WDVCAPI_ProppatchSetDocClass(pCapiHandle,__resource,__pDocClass)) {
					/* get server string for error messages */
					buildServerString(req,&server);
					/* something went wrong */
					WDVCAPI_GetLastError(pCapiHandle,&__errorItem);
					WDVCAPI_GetErrorType(__errorItem,&__errorType);
					switch (__errorType) {
						case WDVCAPI_ERR_TYPE_UNDEFINED:
						case WDVCAPI_ERR_TYPE_SQL: 
							{
								/* internal server error */
								WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
								__error = ERROR_500;
								sp77sprintf(__errorText,1000,ERROR_PROPPATCH_URI,server,__resource);
								sapdbwa_WriteLogMsg(wa, __errorText);
								sp77sprintf(__errorText,1000,ERROR_PROPPATCH_500,__errorMsg);
								sapdbwa_WriteLogMsg(wa, __errorText);
								break;
							}
						case WDVCAPI_ERR_TYPE_CAPI: 
							{
								WDVCAPI_GetErrorCode(__errorItem,&__errorCode);
								WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
								switch(__errorCode) {
									case WDVCAPI_ERR_CODE_LOCK_EXISTS:
											__error = ERROR_423;
											sp77sprintf(__errorText,1000,"423 Resource %s locked. Change of properties forbidden.",__resource);
											sapdbwa_WriteLogMsg(wa, __errorText);
											break;
									default:
										{
											__error = ERROR_500;
											sp77sprintf(__errorText,1000,ERROR_PROPPATCH_URI,server,__resource);
											sapdbwa_WriteLogMsg(wa, __errorText);
											sp77sprintf(__errorText,1000,ERROR_PROPPATCH_500,__errorMsg);
											sapdbwa_WriteLogMsg(wa, __errorText);
											break;
										}
								}
								break;
							}
						default:
							{
								WDVCAPI_GetErrorText(__errorItem,&__errorMsg);
								__error = ERROR_500;
								sp77sprintf(__errorText,1000,ERROR_PROPPATCH_URI,server,__resource);
								sapdbwa_WriteLogMsg(wa, __errorText);
								sp77sprintf(__errorText,1000,ERROR_PROPPATCH_500,__errorMsg);
								sapdbwa_WriteLogMsg(wa, __errorText);
								break;
							}
					}
					if (server) {
						WDVHandler_Free((sapdbwa_UInt1*) server);
					}
					showXmlProperties(wa,req,rep,__resource,__location,pCapiHandle,xmlHandle,WDVH_True,__errorText);
				} else {					
					/* success */
					/* call CAPI function*/
					statuscode = getCallCapiFunc(wa,req,rep,pCapiHandle,__location,"",-1,-1,&isDocInXDB);
					if (statuscode==200)
						getBuildReply(wa,statuscode,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
				}
			} else {
				/* nothing to do -> go back to list */
				/* call CAPI function*/
				statuscode = getCallCapiFunc(wa,req,rep,pCapiHandle,__location,"",-1,-1,&isDocInXDB);
				if (statuscode==200) 
					getBuildReply(wa,statuscode,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
			}
		} else {
			/* unknown command -> just show the collection */
			/* extract Parameters (Host, RequestURI)*/
			getFirstHeaderLine(wa,req,&host,&port,requestURI);
			/* call CAPI function*/
			statuscode = getCallCapiFunc(wa,req,rep,pCapiHandle,__location,"",-1,-1,&isDocInXDB);
			if (statuscode==200)
				getBuildReply(wa,statuscode,req,rep,pCapiHandle,wdvhHandle,host,port,__location,-1,-1,isDocInXDB);
		}
	}

	void showCollection(sapdbwa_Handle		wa,
						sapdbwa_HttpRequestP request,
						sapdbwa_HttpReplyP	reply, 
						WDVCAPI_WDV			pCapiHandle,
						WDVH_Handle			wdvhHandle,
						WDVH_Char			*host,
						WDVH_Char			*port,
						WDVCAPI_URI			resourceURI)
	{
		/* build a page with links to the collection members */
		WDVH_Char					prefix[WDVH_MAX_URI_LEN+1];
		WDVCAPI_URIString			childResource;
		WDVH_Bool					childFound;
		WDVH_Char					temp[WDVH_MAX_URI_LEN+1];
		WDVH_Char					url[WDVH_MAX_URI_LEN+1];
		WDVH_Char					urlenc[WDVH_MAX_URI_LEN+1];
		WDVH_Char					url_up[WDVH_MAX_URI_LEN+1];
		WDVH_Char					resourceURIenc[WDVH_MAX_URI_LEN+1];
		WDVH_Char					href[WDVH_MAX_URI_LEN+1];
		WDVH_Char					uri[WDVH_MAX_URI_LEN+1];
		WDVH_Char					garbage[WDVH_MAX_URI_LEN+1];
		WDVH_Int2					toggle = 1;
        WDVCAPI_PropertyShortValueBuffer  resourceType;
        WDVCAPI_PropertyShortValueBuffer  filesize;
        WDVCAPI_PropertyShortValueBuffer  lastModified;
        WDVH_Bool						  isXmlDb;
		WDVH_Char							size[50];
		WDVH_Char							*server;

		/* set server prefix */
		getWebDAVPrefix(wa,prefix);
		buildServerStringLong(wa,request,&server);

		strcpy(url,"");
		sp77sprintf(url,WDVH_MAX_URI_LEN,"http://%s:%s%s%s",host,port,prefix,resourceURI);
		WDVH_EscapeQuotes(url, strlen(url), urlenc,WDVH_MAX_URI_LEN);
		strcpy(temp,"");
		sp77sprintf(temp,WDVH_MAX_URI_LEN,"http://%s:%s%s%s",host,port,prefix,resourceURI);
		WDVH_EscapeQuotes(temp, strlen(temp), url_up,WDVH_MAX_URI_LEN);
		WDVH_EscapeQuotes(resourceURI, strlen(resourceURI), resourceURIenc,WDVH_MAX_URI_LEN);
		strcpy(temp,"");
		sp77sprintf(temp,WDVH_MAX_URI_LEN,"http://%s:%s%s%s",host,port,prefix,WDVH_DELETED_ITEMS_FOLDER_ESC);
		WDVH_EscapeQuotes(temp, strlen(temp), garbage,WDVH_MAX_URI_LEN);

		sapdbwa_SendBody(reply,
							"<html><head>\
							<meta http-equiv=\"expires\" content=\"Sat, 01 Dec 2001 00:00:00 GMT\">\
							<link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
							<script>\
							function Del(file) \
							{\
								question = \"Do you really want to delete \"+file+\" ?\";\
								agree = confirm(question);\
								if (agree) {\
									document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
									"\"+escape(file);\
									document.explorer.action = \"",
							0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=Del\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
								}\
							}",
							0);
		sapdbwa_SendBody(reply,
						 "function Discard(file) \
							{\
								if (file == '/Deleted Items')\
									question = \"Do you really want to discard all deleted items?\";\
								else \
									question = \"Do you really want to discard \" + file + \" ?\";\
								agree = confirm(question);\
								if (agree) {\
									document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
									"\"+escape(file);\
									document.explorer.action = \"",
							0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=Discard\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
								}\
							}",
							0);
		sapdbwa_SendBody(reply,
						 "function Restore(file) \
							{\
								document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
								"\"+escape(file);\
								document.explorer.action = \"",
							0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=RestoreScreen\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
							}",
							0);
		sapdbwa_SendBody(reply,
						 "function upload(file) \
							{\
								document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
								"\"+escape(file);\
								document.explorer.action = \"",
							0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=uploadFileScreen\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
							}",
							0);
		sapdbwa_SendBody(reply,
						 "function mkcol(file) \
							{\
								document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
								"\"+escape(file);\
								document.explorer.action = \"",
							0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=mkColScreen\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
							}",
							0);
		sapdbwa_SendBody(reply,
						"function showProps(file) {\
							document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
						"\"+escape(file);\
							document.explorer.action = \"",
						0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=ShowProps\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
							}",
							0);
		sapdbwa_SendBody(reply,
						"function showXQuery(file) {\
							document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
						"\"+escape(file);\
							document.explorer.action = \"",
						0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=ShowXQuery\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
							}",
							0);
		sapdbwa_SendBody(reply,
						"function showColProps(file) {\
							document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
						"\"+escape(file);\
							document.explorer.action = \"",
						0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=ShowColProps\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
							}",
							0);
		sapdbwa_SendBody(reply,
						"function showXmlProps(file) {\
							document.explorer.res.value=\"",
						0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,
						"\"+escape(file);\
							document.explorer.action = \"",
						0);
		sapdbwa_SendBody(reply,
							sapdbwa_GetServiceName(wa),
							strlen(sapdbwa_GetServiceName(wa)));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=ShowXmlProps\";\
									if (top.frames[\"Client\"]) {\
										document.explorer.target = \"Client\";\
									}\
									document.explorer.submit();\
							}",
							0);
		sapdbwa_SendBody(reply,							
						"</script>\
						</head>\
						<body>\
						<form action=\"\" method=\"post\" name=\"explorer\">\
						<input type='hidden' name='location' value='",
						0);
		sapdbwa_SendBody(reply,
							resourceURIenc,
							strlen(resourceURIenc));
		sapdbwa_SendBody(reply,
						"'><input type='hidden' name='res' value=''>",
						0);
		sapdbwa_SendBody(reply,
					"<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr><td width=1%></td>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr>",0);
		sapdbwa_SendBody(reply,
							"<td><img src='/WARoot/Images/1x1.gif' width='1'></td>\
							<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='",0);
		sapdbwa_SendBody(reply,
						server,
						strlen(server));
		sapdbwa_SendBody(reply,
						"' class='buttonemph'><img src=\"/WARoot/Images/xml_home.gif\" valign=absmiddle border=0>&nbsp;Home&nbsp;</a>\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
										<td><img src='/WARoot/Images/1x1.gif' width='10'></td>",0);
		sapdbwa_SendBody(reply,
							"<td><img src='/WARoot/Images/1x1.gif' width='1'></td>\
							<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='javascript:upload(\"",0);
		sapdbwa_SendBody(reply,
						uri,
						strlen(uri));
		sapdbwa_SendBody(reply,
						"\")' class='buttonemph'><img src=\"/WARoot/Images/xml_file.gif\" height=12 width=12 valign=absmiddle border=0>&nbsp;Upload File&nbsp;</a>\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
										<td><img src='/WARoot/Images/1x1.gif' width='10'></td>",0);
		sapdbwa_SendBody(reply,
							"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='javascript:mkcol(\"",0);
		sapdbwa_SendBody(reply,
						uri,
						strlen(uri));
		sapdbwa_SendBody(reply,
						"\")' class='buttonemph'><img src=\"/WARoot/Images/xml_folder.gif\" height=12 width=12 valign=absmiddle border=0>&nbsp;Create Collection&nbsp;</a>\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
							</tr><tr><td width=1%>&nbsp</td></tr>\
					</table>",
					0);
		sapdbwa_SendBody(reply,
						"<table border=\"0\" width=\"100%\"><tr><td width=1%></td><td valign=center class='tableheader' colspan=8>&nbsp;Content of&nbsp;",
						0);

		sapdbwa_SendBody(reply,
							url,
							strlen(url));

		sapdbwa_SendBody(reply,
							"</td><td width=1%></td></tr>",
							0);

		/* table header */
		sapdbwa_SendBody(reply,
							"<tr><td width=1%></td><td class='tablecolheader'>&nbsp;</td><td class='tablecolheader'width=\"100%\">&nbsp;Name&nbsp;</td>",
							0);
		sapdbwa_SendBody(reply,
						"<td class='tablecolheader' nowrap>&nbsp;Size&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tablecolheader' nowrap>&nbsp;Last&nbsp;Modified&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tablecolheader'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tablecolheader'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tablecolheader'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td width=1%></td></tr>",
						0);
		/* this folder "." */
		sapdbwa_SendBody(reply,
						"<tr><td width=1%></td><td class='tableline'><a href=\"",
						0);
		getUpHref(urlenc,url_up);
		sapdbwa_SendBody(reply,
						urlenc,
						strlen(urlenc));
		sapdbwa_SendBody(reply,
							"\"><img src='/WARoot/Images/xml_folder_refresh.gif' valign=absmiddle border=0></a></td><td class='tableline'width=\"100%\"><a class='linkhoverti' href=\"",
							0);
		sapdbwa_SendBody(reply,
							urlenc,
							strlen(urlenc));
		sapdbwa_SendBody(reply,
						"\">&nbsp;",
						0);
		sapdbwa_SendBody(reply,
							"<font class='text'><b>.</b></font>",
							0);
		sapdbwa_SendBody(reply,
						"</a></td>",
						strlen("</a></td>"));
		sapdbwa_SendBody(reply,
						"<td class='tableline'>&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tableline'>&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tableline'>&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tableline'>&nbsp;</td>",
						0);
		sapdbwa_SendBody(reply,
						"<td class='tableline'>&nbsp;</td><td width=1%></td></tr>",
						0);
		toggle = toggle * (-1);

		if (strcmp(resourceURI,"/")!=0) {
			/* this folder ".." */
			sapdbwa_SendBody(reply,
								"<tr><td width=1%></td><td class='tableline2'><a href=\"",
								0);
			getUpHref(urlenc,url_up);
			sapdbwa_SendBody(reply,
								url_up,
								strlen(url_up));
			sapdbwa_SendBody(reply,
								"\"><img src='/WARoot/Images/xml_folder_up.gif' valign=absmiddle border=0></a></td><td class='tableline2'width=\"100%\"><a class='linkhoverti' href=\"",
								0);
			getUpHref(urlenc,url_up);
			sapdbwa_SendBody(reply,
								url_up,
								strlen(url_up));
			sapdbwa_SendBody(reply,
							"\">&nbsp;",
							0);
			sapdbwa_SendBody(reply,
								"<font class='text'><b>..</b></font>",
								0);
			sapdbwa_SendBody(reply,
							"</a></td>",
							0);
			sapdbwa_SendBody(reply,
							"<td class='tableline2' nowrap>&nbsp;</td>",
							0);
			sapdbwa_SendBody(reply,
							"<td class='tableline2' nowrap>&nbsp;</td>",
							0);
			sapdbwa_SendBody(reply,
							"<td class='tableline2'>&nbsp;</td>",
							0);
			sapdbwa_SendBody(reply,
							"<td class='tableline2'>&nbsp;</td>",
							0);
			sapdbwa_SendBody(reply,
							"<td class='tableline2'>&nbsp;</td><td width=1%></td></tr>",
							0);
			toggle = toggle * (-1);
		}
		if (strcmp(resourceURI,"/")==0) {
			/* this folder "Deleted Items" */
			if (toggle == 1) {
				sapdbwa_SendBody(reply,
									"<tr><td width=1%></td><td class='tableline'><a href=\"",
									0);
				sapdbwa_SendBody(reply,
								garbage,
								strlen(garbage));
				sapdbwa_SendBody(reply,
								"\"><img src='/WARoot/Images/xml_folder_trash.gif' valign=absmiddle border=0></a></td><td class='tableline'width=\"100%\"><a class='linkhoverti' href=\"",
								0);
				sapdbwa_SendBody(reply,
								garbage,
								strlen(garbage));
				sapdbwa_SendBody(reply,
								"\">&nbsp;",
								0);
				sapdbwa_SendBody(reply,
									"<font class='text'>Deleted Items</font>",
									0);
				sapdbwa_SendBody(reply,
								"</a></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline' align=right nowrap><font class='text'>&nbsp;&nbsp;&nbsp;</font></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline' nowrap><font class='text'>&nbsp;&nbsp;</font></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline'><a title='Show Properties' href=javascript:showColProps(\"",
								0);
				sapdbwa_SendBody(reply,
								WDVH_DELETED_ITEMS_FOLDER_ESC,
								strlen(WDVH_DELETED_ITEMS_FOLDER_ESC));
				sapdbwa_SendBody(reply,
								"\")><img src='/WARoot/Images/xml_props.gif' border=0 valign=absmiddle></a></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline' align=right nowrap><font class='text'>&nbsp;</font></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline'><a title='Empty Deleted Items Folder' href=javascript:Discard(\"",
								0);
				sapdbwa_SendBody(reply,
								WDVH_DELETED_ITEMS_FOLDER_ESC,
								strlen(WDVH_DELETED_ITEMS_FOLDER_ESC));
				sapdbwa_SendBody(reply,
								"\")><img src='/WARoot/Images/xml_trash_empty.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
								0);
			} else {
				sapdbwa_SendBody(reply,
									"<tr><td width=1%></td><td class='tableline2'><a href=\"",
									0);
				sapdbwa_SendBody(reply,
								garbage,
								strlen(garbage));
				sapdbwa_SendBody(reply,
								"\"><img src='/WARoot/Images/xml_folder_trash.gif' valign=absmiddle border=0></a></td><td class='tableline2'width=\"100%\"><a class='linkhoverti' href=\"",
								0);
				sapdbwa_SendBody(reply,
								garbage,
								strlen(garbage));
				sapdbwa_SendBody(reply,
								"\">&nbsp;",
								0);
				sapdbwa_SendBody(reply,
									"<font class='text'>Deleted Items</font>",
									0);
				sapdbwa_SendBody(reply,
								"</a></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline2' align=right nowrap><font class='text'>&nbsp;&nbsp;&nbsp;</font></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline2' nowrap><font class='text'>&nbsp;&nbsp;</font></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline2'><a title='Show Properties' href=javascript:showColProps(\"",
								0);
				sapdbwa_SendBody(reply,
								WDVH_DELETED_ITEMS_FOLDER_ESC,
								strlen(WDVH_DELETED_ITEMS_FOLDER_ESC));
				sapdbwa_SendBody(reply,
								"\")><img src='/WARoot/Images/xml_props.gif' border=0 valign=absmiddle></a></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline2' align=right nowrap><font class='text'>&nbsp;</font></td>",
								0);
				sapdbwa_SendBody(reply,
								"<td class='tableline2'><a title='Empty Deleted Items Folder' href=javascript:Discard(\"",
								0);
				sapdbwa_SendBody(reply,
								WDVH_DELETED_ITEMS_FOLDER_ESC,
								strlen(WDVH_DELETED_ITEMS_FOLDER_ESC));
				sapdbwa_SendBody(reply,
								"\")><img src='/WARoot/Images/xml_trash_empty.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
								0);
			}
			toggle = toggle * (-1);
		}

		/* get the child collections */
/*		WDVCAPI_GetCollection(pCapiHandle,childResource,resourceType,filesize,lastModified,&childFound);*/
		WDVCAPI_GetCollection(pCapiHandle,childResource,resourceType,filesize,lastModified,&isXmlDb,&childFound);
		while(childFound) {
			/* get some properties */
			if (toggle == 1) {
				getResourceHref(urlenc,childResource,temp);
				WDVH_EscapeQuotes(temp, strlen(temp), href, WDVH_MAX_URI_LEN);
				strcpy(uri,temp);
				getRequestUriFromString(wa,request,temp);
				if (strcmp(temp,WDVH_DELETED_ITEMS_FOLDER)!=0) {
					WDVH_EscapeQuotes(temp, strlen(temp), uri, WDVH_MAX_URI_LEN);
					if (strcmp(resourceType,"collection")==0) {
						sapdbwa_SendBody(reply,
										"<tr><td width=1%></td><td class='tableline'><a href=\"",
										0);
						sapdbwa_SendBody(reply,
										href,
										strlen(href));
						sapdbwa_SendBody(reply,
										"\"><img src='/WARoot/Images/xml_folder.gif' valign=absmiddle border=0></a></td><td class='tableline'width=\"100%\"><a class='linkhoverti' href=\"",
										0);
					} else {
						sapdbwa_SendBody(reply,
										"<tr><td width=1%></td><td class='tableline'><a href=\"",
										0);
						sapdbwa_SendBody(reply,
										href,
										strlen(href));
						sapdbwa_SendBody(reply,
										"\"><img src='/WARoot/Images/xml_file.gif' valign=absmiddle border=0></a></td><td class='tableline'width=\"100%\"><a class='linkhoverti' target=top href=\"",
										0);
					}
					sapdbwa_SendBody(reply,
									href,
									strlen(href));
					sapdbwa_SendBody(reply,
									"\">&nbsp;<font class='text'>",
									0);
					sapdbwa_SendBody(reply,
									childResource,
									strlen(childResource));
					sapdbwa_SendBody(reply,
									"</font></a></td>",
									0);
					if (strcmp(resourceType,"collection")!=0) {
						sapdbwa_SendBody(reply,
										"<td class='tableline' align=right nowrap><font class='text'>&nbsp;",
										0);
						transformFileSize(filesize,size,WDVH_False);
						sapdbwa_SendBody(reply,
										size,
										strlen(size));
						sapdbwa_SendBody(reply,
										"&nbsp;&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline' nowrap><font class='text'>&nbsp;",
										0);
						sapdbwa_SendBody(reply,
										lastModified,
										strlen(lastModified));
						sapdbwa_SendBody(reply,
										"&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline'><a title='Show Properties' href=javascript:showProps(\"",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"\")><img src='/WARoot/Images/xml_props.gif' border=0 valign=absmiddle></a></td>",
										0);
/*						sapdbwa_SendBody(reply,
										"<td class='tableline'><a title='Show XML Indexing Properties' href=javascript:showXmlProps('",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"')><img src='/WARoot/Images/xml_xml.gif' border=0 valign=absmiddle></a></td>",
										0);
*/						if (strncmp(resourceURI,WDVH_DELETED_ITEMS_FOLDER,strlen(WDVH_DELETED_ITEMS_FOLDER))!=0) {
							if (isXmlDb == WDVH_False) {
								sapdbwa_SendBody(reply,
												"<td class='tableline'><a title='Show XML Indexing Properties' href=javascript:showXmlProps('",
												0);
								sapdbwa_SendBody(reply,
												uri,
												strlen(uri));
								sapdbwa_SendBody(reply,
												"')><img src='/WARoot/Images/xml_xml.gif' border=0 valign=absmiddle></a></td>",
												0);
							} else {
								sapdbwa_SendBody(reply,
												"<td class='tableline'><a title='Show XML Indexing Properties' href=javascript:showXQuery('",
												0);
								sapdbwa_SendBody(reply,
												uri,
												strlen(uri));
								sapdbwa_SendBody(reply,
												"')><img src='/WARoot/Images/xml_xquery.gif' border=0 valign=absmiddle></a></td>",
												0);
							}
						} else {
							sapdbwa_SendBody(reply,
											"<td class='tableline'><a title='Restore' href=javascript:Restore('",
											0);
							sapdbwa_SendBody(reply,
											uri,
											strlen(uri));
							sapdbwa_SendBody(reply,
											"')><img src='/WARoot/Images/xml_trash_recycle.gif' border=0 valign=absmiddle></a></td>",
											0);
						}
					} else {
						sapdbwa_SendBody(reply,
										"<td class='tableline' align=right nowrap><font class='text'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline' nowrap><font class='text'>&nbsp;",
										0);
						sapdbwa_SendBody(reply,
										lastModified,
										strlen(lastModified));
						sapdbwa_SendBody(reply,
										"&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline'><a title='Show Properties' href=javascript:showColProps('",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"')><img src='/WARoot/Images/xml_props.gif' border=0 valign=absmiddle></a></td>",
										0);
/*						sapdbwa_SendBody(reply,
										"<td class='tableline' align=right nowrap><font class='text'>&nbsp;&nbsp;&nbsp;</font></td>",
										0);
*/
						if (strncmp(resourceURI,WDVH_DELETED_ITEMS_FOLDER,strlen(WDVH_DELETED_ITEMS_FOLDER))!=0) {
							sapdbwa_SendBody(reply,
											"<td class='tableline'><a title='Restore' href=javascript:showXQuery('",
											0);
							sapdbwa_SendBody(reply,
											uri,
											strlen(uri));
							sapdbwa_SendBody(reply,
											"')><img src='/WARoot/Images/xml_xquery.gif' border=0 valign=absmiddle></a></td>",
											0);
						} else {
							sapdbwa_SendBody(reply,
											"<td class='tableline'><a title='Restore' href=javascript:Restore('",
											0);
							sapdbwa_SendBody(reply,
											uri,
											strlen(uri));
							sapdbwa_SendBody(reply,
											"')><img src='/WARoot/Images/xml_trash_recycle.gif' border=0 valign=absmiddle></a></td>",
											0);
						}
					}
/*					sapdbwa_SendBody(reply,
									"<td class='tableline'><a title='Delete' href=javascript:Del(\"",
									0);
					sapdbwa_SendBody(reply,
									uri,
									strlen(uri));
					sapdbwa_SendBody(reply,
									"\")><img src='/WARoot/Images/xml_trash.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
									0);
*/					if (strncmp(resourceURI,WDVH_DELETED_ITEMS_FOLDER,strlen(WDVH_DELETED_ITEMS_FOLDER))!=0) {
						sapdbwa_SendBody(reply,
										"<td class='tableline'><a title='Delete' href=javascript:Del(\"",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"\")><img src='/WARoot/Images/xml_trash.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
										0);
					} else {
						sapdbwa_SendBody(reply,
										"<td class='tableline'><a title='Discard' href=javascript:Discard(\"",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"\")><img src='/WARoot/Images/xml_trash_empty.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
										0);
					}
					toggle = toggle*(-1);
				}
			} else {
				getResourceHref(urlenc,childResource,temp);
				WDVH_EscapeQuotes(temp, strlen(temp), href, WDVH_MAX_URI_LEN);
				strcpy(uri,temp);
				getRequestUriFromString(wa,request,temp);
				if (strcmp(temp,WDVH_DELETED_ITEMS_FOLDER)!=0) {
					WDVH_EscapeQuotes(temp, strlen(temp), uri, WDVH_MAX_URI_LEN);
					if (strcmp(resourceType,"collection")==0) {
						sapdbwa_SendBody(reply,
										"<tr><td width=1%></td><td class='tableline2'><a href=\"",
										0);
						sapdbwa_SendBody(reply,
										href,
										strlen(href));
						sapdbwa_SendBody(reply,
										"\"><img src='/WARoot/Images/xml_folder.gif' valign=absmiddle border=0></a></td><td class='tableline2'width=\"100%\"><a class='linkhoverti' href=\"",
										0);
					} else {
						sapdbwa_SendBody(reply,
										"<tr><td width=1%></td><td class='tableline2'><a href=\"",
										0);
						sapdbwa_SendBody(reply,
										href,
										strlen(href));
						sapdbwa_SendBody(reply,
										"\"><img src='/WARoot/Images/xml_file.gif' valign=absmiddle border=0></a></td><td class='tableline2'width=\"100%\"><a class='linkhoverti' target=top href=\"",
										0);
					}
					sapdbwa_SendBody(reply,
									href,
									strlen(href));
					sapdbwa_SendBody(reply,
									"\">&nbsp;",
									0);
					sapdbwa_SendBody(reply,
									childResource,
									strlen(childResource));
					sapdbwa_SendBody(reply,
									"</a></td>",
									0);
					if (strcmp(resourceType,"collection")!=0) {
						sapdbwa_SendBody(reply,
										"<td class='tableline2' align=right nowrap><font class='text'>&nbsp;",
										0);
						transformFileSize(filesize,size,WDVH_False);
						sapdbwa_SendBody(reply,
										size,
										strlen(size));
						sapdbwa_SendBody(reply,
										"&nbsp;&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline2' nowrap><font class='text'>&nbsp;",
										0);
						sapdbwa_SendBody(reply,
										lastModified,
										strlen(lastModified));
						sapdbwa_SendBody(reply,
										"&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline2'><a title='Show Properties' href=javascript:showProps(\"",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"\")><img src='/WARoot/Images/xml_props.gif' border=0 valign=absmiddle></a></td>",
										0);
/*						sapdbwa_SendBody(reply,
										"<td class='tableline2'><a title='Show XML Indexing Properties' href=javascript:showXmlProps('",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"')><img src='/WARoot/Images/xml_xml.gif' border=0 valign=absmiddle></a></td>",
										0);
*/
						if (strncmp(resourceURI,WDVH_DELETED_ITEMS_FOLDER,strlen(WDVH_DELETED_ITEMS_FOLDER))!=0) {
							if (isXmlDb == WDVH_False) {
								sapdbwa_SendBody(reply,
												"<td class='tableline2'><a title='Show XML Indexing Properties' href=javascript:showXmlProps('",
												0);
								sapdbwa_SendBody(reply,
												uri,
												strlen(uri));
								sapdbwa_SendBody(reply,
												"')><img src='/WARoot/Images/xml_xml.gif' border=0 valign=absmiddle></a></td>",
												0);
							} else {
								sapdbwa_SendBody(reply,
												"<td class='tableline2'><a title='Show XML Indexing Properties' href=javascript:showXQuery('",
												0);
								sapdbwa_SendBody(reply,
												uri,
												strlen(uri));
								sapdbwa_SendBody(reply,
												"')><img src='/WARoot/Images/xml_xquery.gif' border=0 valign=absmiddle></a></td>",
												0);
							}
						} else {
							sapdbwa_SendBody(reply,
											"<td class='tableline2'><a title='Restore' href=javascript:Restore('",
											0);
							sapdbwa_SendBody(reply,
											uri,
											strlen(uri));
							sapdbwa_SendBody(reply,
											"')><img src='/WARoot/Images/xml_trash_recycle.gif' border=0 valign=absmiddle></a></td>",
											0);
						}
					} else {
						sapdbwa_SendBody(reply,
										"<td class='tableline2' align=right nowrap><font class='text'>&nbsp;&nbsp;&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline2' nowrap><font class='text'>&nbsp;",
										0);
						sapdbwa_SendBody(reply,
										lastModified,
										strlen(lastModified));
						sapdbwa_SendBody(reply,
										"&nbsp;</font></td>",
										0);
						sapdbwa_SendBody(reply,
										"<td class='tableline2'><a title='Show Properties' href=javascript:showColProps('",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"')><img src='/WARoot/Images/xml_props.gif' border=0 valign=absmiddle></a></td>",
										0);
/*						sapdbwa_SendBody(reply,
										"<td class='tableline2' align=right nowrap><font class='text'>&nbsp;&nbsp;&nbsp;</font></td>",
										0);
*/
						if (strncmp(resourceURI,WDVH_DELETED_ITEMS_FOLDER,strlen(WDVH_DELETED_ITEMS_FOLDER))!=0) {
							sapdbwa_SendBody(reply,
											"<td class='tableline2'><a title='Restore' href=javascript:showXQuery('",
											0);
							sapdbwa_SendBody(reply,
											uri,
											strlen(uri));
							sapdbwa_SendBody(reply,
											"')><img src='/WARoot/Images/xml_xquery.gif' border=0 valign=absmiddle></a></td>",
											0);
						} else {
							sapdbwa_SendBody(reply,
											"<td class='tableline2'><a title='Restore' href=javascript:Restore('",
											0);
							sapdbwa_SendBody(reply,
											uri,
											strlen(uri));
							sapdbwa_SendBody(reply,
											"')><img src='/WARoot/Images/xml_trash_recycle.gif' border=0 valign=absmiddle></a></td>",
											0);
						}
					}
/*					sapdbwa_SendBody(reply,
									"<td class='tableline2'><a title='Delete' href=javascript:Del(\"",
									0);
					sapdbwa_SendBody(reply,
									uri,
									strlen(uri));
					sapdbwa_SendBody(reply,
									"\")><img src='/WARoot/Images/xml_trash.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
									0);
*/					if (strncmp(resourceURI,WDVH_DELETED_ITEMS_FOLDER,strlen(WDVH_DELETED_ITEMS_FOLDER))!=0) {
						sapdbwa_SendBody(reply,
										"<td class='tableline2'><a title='Delete' href=javascript:Del(\"",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"\")><img src='/WARoot/Images/xml_trash.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
										0);
					} else {
						sapdbwa_SendBody(reply,
										"<td class='tableline2'><a title='Discard' href=javascript:Discard(\"",
										0);
						sapdbwa_SendBody(reply,
										uri,
										strlen(uri));
						sapdbwa_SendBody(reply,
										"\")><img src='/WARoot/Images/xml_trash_empty.gif' border=0 valign=absmiddle></a></td><td width=1%></td></tr>",
										0);
					}
					toggle = toggle*(-1);
				}
			}
			WDVCAPI_GetNextCollection(pCapiHandle,childResource,resourceType,filesize,lastModified,&isXmlDb,&childFound);

		}

		sapdbwa_SendBody(reply,
							"</table></form></body></html>",
							strlen("</table></form></body></html>"));

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	void propfindBuildExplorerReply(sapdbwa_Handle			wa,
									sapdbwa_HttpRequestP	request,
									sapdbwa_HttpReplyP		reply,
									WDVH_Char				*uri,
									WDVH_Char				*location,
									WDVCAPI_WDV				wdv,
									WDVCAPI_PropfindHandle  propfind)
	{
		WDVCAPI_PropfindPropertyName				propertyName;
		WDVCAPI_PropfindPropertyShortValue			propertyShortValue;
		WDVCAPI_PropfindNameSpace					propfindNamespace;
		WDVCAPI_PropfindNameSpaceShortcut			propertyNamespaceShortcut;
		WDVCAPI_PropfindNameSpaceShortcut			davPropertyNamespaceShortcut = NULL;
		WDVH_XmlNamespaceList						namespaceList;
		WDVH_XmlNamespace							ns;
		WDVH_Char									resourcetype[WDV_MAX_PROPERTY_SHORT_VALUE_LEN+1];
		WDVH_Char									getcontenttype[WDV_MAX_PROPERTY_SHORT_VALUE_LEN+1];
		WDVH_Char									getcontentlength[WDV_MAX_PROPERTY_SHORT_VALUE_LEN+1];
		WDVH_Char									getlastmodified[WDV_MAX_PROPERTY_SHORT_VALUE_LEN+1];
		WDVCAPI_LockHandle							lock;
		WDVH_Char									timeout[50];
		WDVH_Char									locktoken[100];
		WDVH_Char									size[50];
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									*server;

		sapdbwa_InitHeader(reply,
						   ERROR_200,
						   "text/html",
						   NULL,
						   NULL,								/* const char    *LastModified,*/
						   NULL,								/* const char    *Expires,*/
						   NULL);								/* const char    *Location );*/
		sapdbwa_SendHeader(reply);

		/* get server string for error messages */
		buildServerString(request,&server);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		/* build namespace list */
		if (!xmlParserCreateNamespaceList(NULL, &namespaceList)) {
		}
		if (WDVCAPI_PropfindGetFirstNameSpace(wdv,propfind,&propertyNamespaceShortcut,&propfindNamespace)) {
			do {
                /* Find out wich shortcut belongs to DAV: */
                if (!davPropertyNamespaceShortcut && strcmp(propfindNamespace, "DAV:") == 0) {
                    /* Shortcut found, so we have to memorize shortcut for later use */
                    davPropertyNamespaceShortcut = propertyNamespaceShortcut;
                }
				strcpy(ns.prefix,propertyNamespaceShortcut);
				strcpy(ns.uri,propfindNamespace);
				xmlParserAddNamespaceToList(NULL,ns,namespaceList);
			} while(WDVCAPI_PropfindGetNextNameSpace(wdv,propfind,&propertyNamespaceShortcut,&propfindNamespace));
		} else {
		}

		/* loop at properties */
		if (WDVCAPI_PropfindGetFirstProperty(wdv,propfind)) {
			do {
				if (WDVCAPI_PropfindGetPropertyName(wdv,propfind,&propertyName )) {
					WDVCAPI_PropfindGetPropertyNameSpaceShortcut(wdv,propfind,&propertyNamespaceShortcut);
					WDVCAPI_PropfindGetPropertyShortValue(wdv,propfind,&propertyShortValue);
					if (strcmp(propertyName,"resourcetype")==0 && strcmp(propertyNamespaceShortcut,davPropertyNamespaceShortcut)==0)
						strcpy(resourcetype,propertyShortValue);
					else if (strcmp(propertyName,"getcontenttype")==0 && strcmp(propertyNamespaceShortcut,davPropertyNamespaceShortcut)==0)
						strcpy(getcontenttype,propertyShortValue);
					else if (strcmp(propertyName,"getcontentlength")==0 && strcmp(propertyNamespaceShortcut,davPropertyNamespaceShortcut)==0)
						strcpy(getcontentlength,propertyShortValue);
					else if (strcmp(propertyName,"getlastmodified")==0 && strcmp(propertyNamespaceShortcut,davPropertyNamespaceShortcut)==0)
						strcpy(getlastmodified,propertyShortValue);
					/* property lockdiscovery has to collect additional data */
					else if (strcmp(propertyName,WDVH_XML_LOCKDISCOVERY)==0) {
						if (WDVCAPI_IdStringIsInitialValue(propertyShortValue)) {
							sp77sprintf(locktoken,100,"");
						} else {
							sp77sprintf(locktoken,100,"saplocktoken:%s",propertyShortValue);
							WDVCAPI_PropfindGetLockDescription(wdv,propfind,propertyShortValue,&lock);
						}
					}
				}
			} while(WDVCAPI_PropfindGetNextProperty(wdv,propfind));
		}

		/* send the properties page */
		sapdbwa_SendBody(reply,
						"<head><meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'><link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'></head><body topmargin='0' leftmargin='0'><form action='' method='post' name='showXie' ID='Form1'><table border='0' cellpadding='0' cellspacing='0' ID='Table1'><tr><td>&nbsp;&nbsp;&nbsp;</td><td class='text'><h1>Properties of ",
						strlen("<head><meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'><link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'></head><body topmargin='0' leftmargin='0'><form action='' method='post' name='showXie' ID='Form1'><table border='0' cellpadding='0' cellspacing='0' ID='Table1'><tr><td>&nbsp;&nbsp;&nbsp;</td><td class='text'><h1>Properties of "));
		sapdbwa_SendBody(reply,
						 uri,
						 strlen(uri));
		sapdbwa_SendBody(reply,
						"</h1></td>\			
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;WebDAV Properties&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
								</table>",
							strlen("</h1></td>\			
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;WebDAV Properties&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
								</table>"));
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr><td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td></tr>",
						0);
		sapdbwa_SendBody(reply,
						"									<!-- Serer Name -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Content Type</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table13'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																				&nbsp;&nbsp;",
						0);
		sapdbwa_SendBody(reply,
						 getcontenttype,
						 strlen(getcontenttype));
		sapdbwa_SendBody(reply,
						"&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- Description  -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Size</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table14'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																				&nbsp;&nbsp;",
						0);
		transformFileSize(getcontentlength,size,WDVH_True);
		sapdbwa_SendBody(reply,
						 size,
						 strlen(size));
		sapdbwa_SendBody(reply,
						"&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- InstRoot  -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Last Modified</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='5'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table15'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																				&nbsp;&nbsp;",
						0);
		sapdbwa_SendBody(reply,
						 getlastmodified,
						 strlen(getlastmodified));
		sapdbwa_SendBody(reply,
						"&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>",
						0);
		if (strcmp(locktoken,"")!=0) {
			sapdbwa_SendBody(reply,
							"<tr>\
								<td>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table16'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
										</tr>\
									</table>\
									<!-- GroupboxTitle -->\
									<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table17'>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
										<tr>\
											<td nowrap class='groupheader'>&nbsp;&nbsp;WebDAV Lock Properties&nbsp;</td>\
										</tr>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
									</table>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table18'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
										</tr>\
									</table>",
								0);
			sapdbwa_SendBody(reply,
							"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table19'>\
										<tr>\
											<td>\
												<!-- Separator -->\
												<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
													<tr>\
														<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
													</tr>\
												</table>\
												<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table21'>\
													<tr>\
														<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
														<td class='text'>\
															<table border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table23'>\
																<tr><td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td></tr>\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Lock Type</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table24'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			if (lock->type == WDVCAPI_LOCK_TYPE_WRITE) 
				sapdbwa_SendBody(reply,
								"write",
								strlen("write"));
			else
				sapdbwa_SendBody(reply,
								" ",
								strlen(" "));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- Serer Name -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Scope</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table25'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			if (lock->scope == WDVCAPI_LOCK_SCOPE_EXCLUSIVE) 
				sapdbwa_SendBody(reply,
								"exclusive",
								strlen("exclusive"));
			else
				sapdbwa_SendBody(reply,
								" ",
								strlen(" "));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- Description  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Owner</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table26'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			sapdbwa_SendBody(reply,
							lock->owner,
							strlen(lock->owner));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- LockDepth  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Depth</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='5'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table29'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			if (lock->depth == WDVCAPI_DEPTH_0)
				sapdbwa_SendBody(reply,
								"0",
								strlen("0"));
			else if (lock->depth == WDVCAPI_DEPTH_1)
				sapdbwa_SendBody(reply,
								"1",
								strlen("1"));
			else if (lock->depth == WDVCAPI_DEPTH_INFINITE)
				sapdbwa_SendBody(reply,
								"Infinite",
								strlen("Infinite"));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- LockTimeout  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Timeout</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='5'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table27'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			sp77sprintf(timeout,49,"%l",lock->timeout);
			sapdbwa_SendBody(reply,timeout,strlen(timeout));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- LockToken  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Lock Token</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='5'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table28'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			sapdbwa_SendBody(reply,locktoken,strlen(locktoken));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
																</tr>\
															</table>\
														</td>\
													</tr>\
												</table>\
											</td>\
										</tr>\
									</table>\
								</td>",
							strlen("&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
																</tr>\
															</table>\
														</td>\
													</tr>\
												</table>\
											</td>\
										</tr>\
									</table>\
								</td>"));
		}
		sapdbwa_SendBody(reply,
						"</tr>\
						</table>\
						<!-- Separator -->\
						<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
							<tr>\
								<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
							</tr>\
						</table>\
					<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' width='1'></td>\
										<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='",
						strlen("</tr>\
						</table>\
						<!-- Separator -->\
						<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
							<tr>\
								<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
							</tr>\
						</table>\
					<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' width='1'></td>\
										<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='"));
		sapdbwa_SendBody(reply,
						server,
						strlen(server));
		sapdbwa_SendBody(reply,
						prefix,
						strlen(prefix));
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"' class='buttonemph'>Back</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",
						strlen("' class='buttonemph'>Back</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>"));

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	/*------------------------------------------------------------------------
		Function:	GetBuildReply
	--------------------------------------------------------------------------*/
	void getBuildExplorerReply(sapdbwa_Handle wa,
							   sapdbwa_HttpRequestP request, 
							   sapdbwa_HttpReplyP reply, 
							   WDVCAPI_WDV	pCapiHandle,
							   WDVH_Handle	wdvhHandle,
							   WDVH_Char	*resource,
							   WDVH_Char    *location,
							   WDVH_Char	*locktoken,
							   WDVCAPI_LockHandle lock)
	{
		
		WDVH_Char					prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char					*server;
		WDVCAPI_PropertyShortValueBuffer	childcols;
		WDVCAPI_PropertyShortValueBuffer	childres;
		WDVCAPI_PropertyShortValueBuffer	treesize;
		WDVH_Char							size[30];
		WDVH_Char									timeout[50];

		/* get server string for error messages */
		buildServerString(request,&server);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		/* initialize Response Header */
		sapdbwa_InitHeader(reply,
                           ERROR_200,
                           "text/html",
                           NULL,
	                       NULL,								/* const char    *LastModified,*/
                           NULL,								/* const char    *Expires,*/
                           NULL);								/* const char    *Location );*/

		sapdbwa_SetHeader ( reply, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader(reply);

		/* build Body */
		WDVCAPI_GetCollectionProperties(pCapiHandle,childcols,childres,treesize);
		if (strcmp(childcols,"")==0)
			strcpy(childcols,"0");
		if (strcmp(childres,"")==0)
			strcpy(childres,"0");
		if (strcmp(treesize,"")==0)
			strcpy(treesize,"0");

		sapdbwa_SendBody(reply,
						"<head><meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'><link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'></head><body topmargin='0' leftmargin='0'><form action='' method='post' name='showXie' ID='Form1'><table border='0' cellpadding='0' cellspacing='0' ID='Table1'><tr><td>&nbsp;&nbsp;&nbsp;</td><td class='text'><h1>Properties of ",
						strlen("<head><meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'><link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'></head><body topmargin='0' leftmargin='0'><form action='' method='post' name='showXie' ID='Form1'><table border='0' cellpadding='0' cellspacing='0' ID='Table1'><tr><td>&nbsp;&nbsp;&nbsp;</td><td class='text'><h1>Properties of "));
		sapdbwa_SendBody(reply,
						resource,
						strlen(resource));
		sapdbwa_SendBody(reply,
						"</h1></td>		\	
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>",
						strlen("</h1></td>		\	
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>"));
		sapdbwa_SendBody(reply,
						"<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Collection Properties&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
								</table>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>",0);
		sapdbwa_SendBody(reply,
						"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr><td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td></tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Contained Files</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table12'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																				&nbsp;&nbsp;",0);
		sapdbwa_SendBody(reply,
						childres,
						strlen(childres));
		sapdbwa_SendBody(reply,
						"&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- Serer Name -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Contained Folders</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table13'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='100%' nowrap>\
																				&nbsp;&nbsp;",0);
		sapdbwa_SendBody(reply,
						childcols,
						strlen(childcols));
		sapdbwa_SendBody(reply,
						"&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- Description  -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Size</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table14'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='100%' nowrap>\
																				&nbsp;&nbsp;",0);
		transformFileSize(treesize,size,WDVH_True);
		sapdbwa_SendBody(reply,
						size,
						strlen(size));
		sapdbwa_SendBody(reply,
						"&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>",0);
		if (strcmp(locktoken,"")!=0) {
			sapdbwa_SendBody(reply,
							"<tr>\
								<td>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table16'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
										</tr>\
									</table>\
									<!-- GroupboxTitle -->\
									<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table17'>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
										<tr>\
											<td nowrap class='groupheader'>&nbsp;&nbsp;WebDAV Lock Properties&nbsp;</td>\
										</tr>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
									</table>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table18'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
										</tr>\
									</table>",
								0);
			sapdbwa_SendBody(reply,
							"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table19'>\
										<tr>\
											<td>\
												<!-- Separator -->\
												<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
													<tr>\
														<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
													</tr>\
												</table>\
												<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table21'>\
													<tr>\
														<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
														<td class='text'>\
															<table border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table23'>\
																<tr><td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td></tr>\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Lock Type</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table24'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			if (lock->type == WDVCAPI_LOCK_TYPE_WRITE) 
				sapdbwa_SendBody(reply,
								"write",
								strlen("write"));
			else
				sapdbwa_SendBody(reply,
								" ",
								strlen(" "));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- Serer Name -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Scope</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table25'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			if (lock->scope == WDVCAPI_LOCK_SCOPE_EXCLUSIVE) 
				sapdbwa_SendBody(reply,
								"exclusive",
								strlen("exclusive"));
			else
				sapdbwa_SendBody(reply,
								" ",
								strlen(" "));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- Description  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Owner</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table26'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			sapdbwa_SendBody(reply,
							lock->owner,
							strlen(lock->owner));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- LockDepth  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Depth</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='5'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table29'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			if (lock->depth == WDVCAPI_DEPTH_0)
				sapdbwa_SendBody(reply,
								"0",
								strlen("0"));
			else if (lock->depth == WDVCAPI_DEPTH_1)
				sapdbwa_SendBody(reply,
								"1",
								strlen("1"));
			else if (lock->depth == WDVCAPI_DEPTH_INFINITE)
				sapdbwa_SendBody(reply,
								"Infinite",
								strlen("Infinite"));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- LockTimeout  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Timeout</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='5'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table27'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			sp77sprintf(timeout,49,"%l",lock->timeout);
			sapdbwa_SendBody(reply,timeout,strlen(timeout));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
																</tr>\
																<!-- LockToken  -->\
																<tr>\
																	<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Lock Token</td>\
																	<td><img src='/WARoot/Images/1x1.gif' width='5'></td>\
																	<td>\
																		<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table28'>\
																			<tr>\
																				<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='480' nowrap>\
																					&nbsp;&nbsp;",
							0);
			sapdbwa_SendBody(reply,locktoken,strlen(locktoken));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;\
																				</td>\
																			</tr>\
																		</table>\
																	</td>\
																</tr>\
																<tr>\
																	<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif'></td>\
																</tr>\
																<!-- Separator -->\
																<tr>\
																	<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
																</tr>\
															</table>\
														</td>\
													</tr>\
												</table>\
											</td>\
										</tr>\
									</table>\
								</td>",0);
		}
		sapdbwa_SendBody(reply,
						"</tr>\
						</table>",0);
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
						<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
							<tr>\
								<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
							</tr>\
						</table>\
						<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' width='1'></td>\
										<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='",
						strlen("<!-- Separator -->\
						<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
							<tr>\
								<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
							</tr>\
						</table>\
						<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' width='1'></td>\
										<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='"));
		sapdbwa_SendBody(reply,
						server,
						strlen(server));
		sapdbwa_SendBody(reply,
						prefix,
						strlen(prefix));
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"' class='buttonemph'>Back</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",
						strlen("' class='buttonemph'>Back</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>"));
		
		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	void showXmlProperties(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   XMLIMAPI_Handle			xmlHandle,
						   WDVH_Bool				erroroccured,
						   WDVH_Char				*errormsg)
	{
		WDVCAPI_PropertyShortValueBuffer			docclass;
		WDVCAPI_PropertyShortValueBuffer			indexstate;
		XMLIMAPI_Id									docclassid;
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									*server;
		XMLIMAPI_DocClass							docClass;
		WDVCAPI_ErrorItem							errorItem;
		WDVCAPI_ErrorType							errorType;
		WDVCAPI_ErrorCode							errorCode;
		WDVCAPI_ErrorText							errorMsg;
		WDVH_Error									error;
		WDVH_Char									errorText[1000];

		sapdbwa_InitHeader(reply,
						   ERROR_200,
						   "text/html",
						   NULL,
						   NULL,								/* const char    *LastModified,*/
						   NULL,								/* const char    *Expires,*/
						   NULL);								/* const char    *Location );*/
		sapdbwa_SendHeader(reply);

		/* get server string for error messages */
		buildServerString(request,&server);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		strcpy(docclass,"");
		strcpy(indexstate,"");
		strcpy(errorText,"");
		if (erroroccured)
			strcpy(errorText,errormsg);
		/* get Xml Properties */
		if (!WDVCAPI_ProppatchGetDocClassProperties(wdv,uri,docclass,indexstate)) {
			if (!erroroccured) {
				/* something went wrong */
				WDVCAPI_GetLastError(wdv,&errorItem);
				WDVCAPI_GetErrorType(errorItem,&errorType);
				switch (errorType) {
					case WDVCAPI_ERR_TYPE_UNDEFINED:
					case WDVCAPI_ERR_TYPE_SQL: 
						{
							erroroccured = WDVH_True;
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
								case WDVCAPI_ERR_CODE_NO_DOC_CLASS_PROPERTY:
									strcpy(docclass,"");
									strcpy(indexstate,"");
									break;
								default:
									{
										erroroccured = WDVH_True;
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
							erroroccured = WDVH_True;
							WDVCAPI_GetErrorText(errorItem,&errorMsg);
							error = ERROR_500;
							sp77sprintf(errorText,1000,ERROR_PROPFIND_URI,server,(char*)sapdbwa_GetRequestURI(request));
							sapdbwa_WriteLogMsg(wa, errorText);
							sp77sprintf(errorText,1000,ERROR_PROPFIND_500,errorMsg);
							sapdbwa_WriteLogMsg(wa, errorText);
							break;
						}
				}
			}
		} else {
			error = ERROR_200;
		}

		/* build reply body */
		sapdbwa_SendBody(reply,
						"<head>\
	<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'>\
	<link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
		<script language='javascript'>\
		function update()\
		{\
			document.showXmlProps.action = '",
						strlen("<head>\
	<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'>\
	<link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
		<script language='javascript'>\
		function update()\
		{\
			document.showXmlProps.action = '"));
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=SetXmlProps';\
			document.showXmlProps.target = 'Client';\
			document.showXmlProps.submit();\
		}\
		</script>\
</head>\
<body topmargin='0' leftmargin='0'>\
	<form action='' method='post' name='showXmlProps' ID='Form1'>\
		<input type='hidden' name='DocClassOld' value='",
						strlen("?explorer=0&cmd=SetXmlProps';\
			document.showXmlProps.target = 'Client';\
			document.showXmlProps.submit();\
		}\
		</script>\
</head>\
<body topmargin='0' leftmargin='0'>\
	<form action='' method='post' name='showXmlProps' ID='Form1'>\
		<input type='hidden' name='DocClassOld' value='"));
		sapdbwa_SendBody(reply,docclass,strlen(docclass));
		sapdbwa_SendBody(reply,
						"'>\
		<input type='hidden' name='res' value='",
						strlen("'>\
		<input type='hidden' name='res' value='"));
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,uri,strlen(uri));
		sapdbwa_SendBody(reply,
						"'>\
		<input type='hidden' name='location' value='",
						strlen("'>\
		<input type='hidden' name='location' value='"));
		sapdbwa_SendBody(reply,location,strlen(location));
		sapdbwa_SendBody(reply,
						"'>\
		<table border='0' cellpadding='0' cellspacing='0' ID='Table1'>\
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td class='text'><h1>Properties of ",
						strlen("'>\
		<table border='0' cellpadding='0' cellspacing='0' ID='Table1'>\
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td class='text'><h1>Properties of "));
		sapdbwa_SendBody(reply,
						uri,
						strlen(uri));
		sapdbwa_SendBody(reply,
						"</h1>",
						strlen("</h1>"));
		if (strcmp(indexstate,"")==0 || strcmp(indexstate,XMLINDEXING_STATE_NEW_AS_STRING)==0 || strcmp(indexstate,XMLINDEXING_STATE_FINISHED_AS_STRING)==0) {
			sapdbwa_SendBody(reply,
							"</td>",
							strlen("</td>"));
		} else {
			sapdbwa_SendBody(reply,
							"<h4><font class='inputlabelreq'>Resource is beeing indexed. No change of Document Class or triggering of re-indexing allowed. Please wait until the resource is unlocked.</font></h4></td>",
							strlen("<h4><font class='inputlabelreq'>Resource is beeing indexed. No change of Document Class or triggering of re-indexing allowed. Please wait until the resource is unlocked.</font></h4></td>"));
		}
		sapdbwa_SendBody(reply,
						"<tr>\
					<td>&nbsp;&nbsp;&nbsp;</td>\
					<td>\
						<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
							<tr>\
								<td>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
										</tr>\
									</table>\
									<!-- GroupboxTitle -->\
									<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
										<tr>\
											<td nowrap class='groupheader'>&nbsp;&nbsp;XML Indexing Properties&nbsp;</td>\
										</tr>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
									</table>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
										</tr>\
									</table>",
							strlen("<tr>\
					<td>&nbsp;&nbsp;&nbsp;</td>\
					<td>\
						<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
							<tr>\
								<td>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
										</tr>\
									</table>\
									<!-- GroupboxTitle -->\
									<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
										<tr>\
											<td nowrap class='groupheader'>&nbsp;&nbsp;XML Indexing Properties&nbsp;</td>\
										</tr>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
									</table>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
										</tr>\
									</table>"));
		sapdbwa_SendBody(reply,
						"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Document \
																	Class</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>",
						strlen("<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Document \
																	Class</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>"));
		if (strcmp(indexstate,"")==0 || strcmp(indexstate,XMLINDEXING_STATE_NEW_AS_STRING)==0 || strcmp(indexstate,XMLINDEXING_STATE_FINISHED_AS_STRING)==0) {
			sapdbwa_SendBody(reply,
							"<select name='DocClass' value='' class='text'>\
																		<option value=''>&lt;No Document \
																			Class assigned&gt;</option>",
							strlen("<select name='DocClass' value='' class='text'>\
																		<option value=''>&lt;No Document \
																			Class assigned&gt;</option>"));
			if (XMLIMAPI_DocClassGetFirst(xmlHandle,&docClass)) {
				do {
					sapdbwa_SendBody(reply,"<option value='",strlen("<option value='"));
					sapdbwa_SendBody(reply,docClass.docClassID,strlen(docClass.docClassID));
					if (strcmp(docclass,docClass.docClassID)==0) {
						sapdbwa_SendBody(reply,"' selected >",strlen("' selected >"));
					} else {
						sapdbwa_SendBody(reply,"'>",strlen("'>"));
					}
					sapdbwa_SendBody(reply,docClass.docClassName,strlen(docClass.docClassName));
					sapdbwa_SendBody(reply,"</option>",strlen("</option>"));
				} while (XMLIMAPI_DocClassGetNext(xmlHandle,&docClass));
			}
			sapdbwa_SendBody(reply,"</select>",strlen("</select>"));
		} else {
			sapdbwa_SendBody(reply,
							"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='450' nowrap>\
																				&nbsp;&nbsp;",
							strlen("<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='450' nowrap>\
																				&nbsp;&nbsp;"));
			XMLIMAPI_IdStringAsId(docclass,docclassid);
			XMLIMAPI_DocClassGetName(xmlHandle,docclassid,docclass);
			sapdbwa_SendBody(reply,
							docclass,
							strlen(docclass));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;</td></tr></table>",
							strlen("&nbsp;&nbsp;</td></tr></table>"));
		}
		sapdbwa_SendBody(reply,
						"</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- Serer Name -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Indexing \
																	Status</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table13'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='450' nowrap>\
																				&nbsp;&nbsp;",
						strlen("</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- Serer Name -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Indexing \
																	Status</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<table border='0' cellpadding='0' cellspacing='1' bgcolor='#666666' width='100%' ID='Table13'>\
																		<tr>\
																			<td colspan='6' valign='bottom' height='16' class='inputreadonly' width='450' nowrap>\
																				&nbsp;&nbsp;"));
		sapdbwa_SendBody(reply,
						indexstate,
						strlen(indexstate));
		sapdbwa_SendBody(reply,
						"&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>",
						strlen("&nbsp;&nbsp;\
																			</td>\
																		</tr>\
																	</table>\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>"));
		if ((strcmp(indexstate,"")==0 || strcmp(indexstate,XMLINDEXING_STATE_NEW_AS_STRING)==0 || strcmp(indexstate,XMLINDEXING_STATE_FINISHED_AS_STRING)==0)&&(strcmp(docclass,"")!=0)) {
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- Serer Name -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Trigger \
																	Reindexing</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<input type='checkbox' name='ReIndex'>\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>",
							strlen("<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<!-- Serer Name -->\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Trigger \
																	Reindexing</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																	<input type='checkbox' name='ReIndex'>\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>"));
		}
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
						</tr>\
					</table>\					
					<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr><td><img src='/WARoot/Images/1x1.gif' width='1'></td>",
						strlen("<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
						</tr>\
					</table>\					
					<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr><td><img src='/WARoot/Images/1x1.gif' width='1'></td>"));
		if (strcmp(indexstate,"")==0 || strcmp(indexstate,XMLINDEXING_STATE_NEW_AS_STRING)==0 || strcmp(indexstate,XMLINDEXING_STATE_FINISHED_AS_STRING)==0) {
			sapdbwa_SendBody(reply,
							"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='javascript:update()' class='buttonemph'>Update</a>&nbsp;&nbsp;\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
											<td><img src='/WARoot/Images/1x1.gif' width='10'></td>",
							strlen("<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='javascript:update()' class='buttonemph'>Update</a>&nbsp;&nbsp;\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
											<td><img src='/WARoot/Images/1x1.gif' width='10'></td>"));
		}
		sapdbwa_SendBody(reply,
						"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='",
						strlen("<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='"));
		sapdbwa_SendBody(reply,
						server,
						strlen(server));
		sapdbwa_SendBody(reply,
						prefix,
						strlen(prefix));
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"' class='buttonemph'>Cancel</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",
					0);
		if (erroroccured) {
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='25'></td>\
						</tr>\
					</table>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Error&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
								</table>",
							strlen("<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='25'></td>\
						</tr>\
					</table>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Error&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
								</table>"));
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
												<tr>\
													<td class='text'>&nbsp;&nbsp;<img src='/WARoot/Images/xml_msgerrosmall.gif' border='0'>&nbsp;",
							strlen("<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
												<tr>\
													<td class='text'>&nbsp;&nbsp;<img src='/WARoot/Images/xml_msgerrosmall.gif' border='0'>&nbsp;"));
			sapdbwa_SendBody(reply,
							errorText,
							strlen(errorText));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;</td>\
												</tr>\
											</table>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",
							strlen("&nbsp;&nbsp;</td>\
												</tr>\
											</table>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>"));
		}
		sapdbwa_SendBody(reply,
					"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",
						strlen("<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>"));

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	void showXQueryScreen(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   WDVH_Handle				wdvhHandle,
						   WDVH_Bool				erroroccured,
						   WDVH_Char				*errormsg)
	{
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									*server;
		WDVCAPI_ErrorItem							errorItem;
		WDVCAPI_ErrorType							errorType;
		WDVCAPI_ErrorCode							errorCode;
		WDVCAPI_ErrorText							errorMsg;
		WDVH_Error									error;
		WDVH_Char									errorText[1000];
		const WDVH_Char								*querystring=NULL;
		WDVCAPI_CharEncoding						encoding;
		WDVH_Bool									success=WDVH_True;
		WDVH_GetBuffer								buffer;
        WDVCAPI_ContentLength						readLength;
        WDVH_Char                                   bodyData[8000] = "";

		sapdbwa_InitHeader(reply,
						   ERROR_200,
						   "text/html",
						   NULL,
						   NULL,								/* const char    *LastModified,*/
						   NULL,								/* const char    *Expires,*/
						   NULL);								/* const char    *Location );*/
		sapdbwa_SetHeader(reply,
						"Transfer-Encoding",
						"chunked");
		sapdbwa_SendHeader(reply);

		/* get server string for error messages */
		buildServerString(request,&server);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		querystring = getParameterValueWithoutCopy("xquery",request);

		encoding = WDVCAPI_CHAR_ENCODING_ASCII;

/*		if (WDVCAPI_XQueryOpen(wdv,uri,querystring,encoding))
			success = WDVH_True;
		else 
			success = WDVH_False;
*/
		if (success==WDV_False) {
			WDVCAPI_GetLastError(wdv,&errorItem);
			WDVCAPI_GetErrorType(errorItem,&errorType);
			switch (errorType) {
				case WDVCAPI_ERR_TYPE_UNDEFINED:
				case WDVCAPI_ERR_TYPE_SQL: 
					{
						erroroccured = WDVH_True;
						/* internal server error */
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						error = ERROR_500;
						sp77sprintf(errorText,1000,ERROR_XQUERY_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_XQUERY_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
				case WDVCAPI_ERR_TYPE_CAPI: 
					{
						WDVCAPI_GetErrorCode(errorItem,&errorCode);
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						switch(errorCode) {
							default:
								{
									erroroccured = WDVH_True;
									error = ERROR_500;
									sp77sprintf(errorText,1000,ERROR_XQUERY_URI,server,(char*)sapdbwa_GetRequestURI(request));
									sapdbwa_WriteLogMsg(wa, errorText);
									sp77sprintf(errorText,1000,ERROR_XQUERY_500,errorMsg);
									sapdbwa_WriteLogMsg(wa, errorText);
									break;
								}
						}
						break;
					}
				default:
					{
						erroroccured = WDVH_True;
						/* internal server error */
						WDVCAPI_GetErrorText(errorItem,&errorMsg);
						error = ERROR_500;
						sp77sprintf(errorText,1000,ERROR_XQUERY_URI,server,(char*)sapdbwa_GetRequestURI(request));
						sapdbwa_WriteLogMsg(wa, errorText);
						sp77sprintf(errorText,1000,ERROR_XQUERY_500,errorMsg);
						sapdbwa_WriteLogMsg(wa, errorText);
						break;
					}
			}
		}

		strcpy(errorText,"");
		if (erroroccured)
			strcpy(errorText,errormsg);

		/* build reply body */
        sp77sprintf(bodyData, 8000, "%s", 
            "<head>\
	<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'>\
	<link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sp77sprintf(bodyData, 8000, "%s", 
		"<script language='javascript'>\
		function Search()\
		{\
			document.showXQuery.action = '",
					 0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sapdbwa_SendBodyChunk(reply, server, strlen(server));
        sapdbwa_SendBodyChunk(reply, prefix, strlen(prefix));
        sapdbwa_SendBodyChunk(reply, uri, strlen(uri));

        sp77sprintf(bodyData, 8000, "%s", 
            "?explorer=0&cmd=ShowXQuery';\
			document.showXQuery.target = 'Client';\
			document.showXQuery.submit();\
		}\
		function Clear()\
		{\
			document.showXQuery.xquery.value = '';\
		}\
		</script>\
</head>\
<body topmargin='0' leftmargin='0'>\
	<form action='' method='post' name='showXQuery' ID='Form1'>\
		<input type='hidden' name='res' value='",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sapdbwa_SendBodyChunk(reply, server, strlen(server));
        sapdbwa_SendBodyChunk(reply, prefix, strlen(prefix));
        sapdbwa_SendBodyChunk(reply, uri, strlen(uri));

        sp77sprintf(bodyData, 8000, "%s", 
			"'>\
			<table border='0' cellpadding='0' cellspacing='0' ID='Table1'>\
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td class='text'><h1>XQuery on ",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sapdbwa_SendBodyChunk(reply, uri, strlen(uri));

        sp77sprintf(bodyData, 8000, "%s", 
						"</h1>\
				</td>\
			</tr>\
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#cbd5e1' width='100%' ID='Table3'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>\
											&nbsp;&nbsp;Query&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
								</table>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#dce3ec' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>",0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sp77sprintf(bodyData, 8000, "%s", 
						 "<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
															<td width='207'><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<!-- Query String -->\
															<tr>\
																<td colspan='6' valign='center' height='12' class='text' width='200' nowrap>\
																	<!-- TemplateKey Result -->\
																	<textarea name='xquery' cols='81' rows='5'>",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        if (querystring!=NULL && strcmp(querystring,"")!=0) {
            sapdbwa_SendBodyChunk(reply, querystring, strlen(querystring));
		} else {
            /*
            sapdbwa_SendBodyChunk(reply, "", 0);
            */
		}

        sp77sprintf(bodyData, 8000, "%s", 
						"</textarea>\
																	<!-- EndTemplateKey QueryString -->\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td colspan='8'><img src='/WARoot/Images/1x1.gif' height='5'></td>\
															</tr>\
															<tr>\
																<td colspan='8' valign='bottom' nowrap height='25' class='text' align='absmiddle'>\
																	<font class='inputlabelreq'>&nbsp;*</font>&nbsp;By now only abbreviated forward\ 
																	axis steps w/o predicates and namespaces are supported.\
																</td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td colspan='8'><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
														<!-- Toolbar -->\
														<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' ID='Table20'>\
															<tr>\
																<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
																<td background='/WARoot/Images/xml_btnb1.gif' align='center' class='button3' nowrap>\
																	&nbsp;&nbsp;<a href='javascript:Search()' class='buttonemph'>Search</a>&nbsp;&nbsp;\
																</td>\
																<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																\
																<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
																<td background='/WARoot/Images/xml_btnb1.gif' align='center' class='button3' nowrap>\
																	&nbsp;&nbsp;<a href='javascript:Clear()' class='buttonemph'>Clear</a>&nbsp;&nbsp;\
																</td>\
																<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>",
						 0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sp77sprintf(bodyData, 8000, "%s", 
						 "										<td><img src='/WARoot/Images/1x1.gif' width='1'></td>\
																<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
																<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
																	&nbsp;&nbsp;<a href='",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sapdbwa_SendBodyChunk(reply, server, strlen(server));
        sapdbwa_SendBodyChunk(reply, prefix, strlen(prefix));
        sapdbwa_SendBodyChunk(reply, location, strlen(location));

        sp77sprintf(bodyData, 8000, "%s", 
						"' class='buttonemph'>Back</a>&nbsp;&nbsp;\
																</td>\
																<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td colspan='8'><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table16'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#cbd5e1' width='100%' ID='Table17'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>\
											&nbsp;&nbsp;Result&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
								</table>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0' ID='Table18'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sp77sprintf(bodyData, 8000, "%s", 
						 "<table border='0' cellpadding='0' cellspacing='1' bgcolor='#dce3ec' ID='Table13'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table14'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='15'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table15'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table19'>\
															<tr>\
															<td width='207'><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<!-- Query String -->\
															<tr>\
																<td colspan='6' valign='center' height='12' class='text' width='200' nowrap>\
																	<!-- TemplateKey Result -->\
																	<textarea cols='81' rows='20'>",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        /* get the result stream from the capi */
		if (querystring!=NULL && strcmp(querystring,"")!=0) {
/*
			sapdbwa_SendBody(reply,
						"<bib>\n \
	<book year='1994'>\n \
		<title>TCP/IP Illustrated</title>\n \
		<author>\n \
			<last>Stevens</last>\n \
			<first>W.</first>\n \
		</author>\n \
		<publisher>Addison-Wesley</publisher>\n \
		<price>65.95</price>\n \
	</book>\n \
	<book year='1992'>\n \
		<title>Advanced Programming in the Unix environment</title>\n \
		<author>\n \
			<last>Stevens</last>\n \
			<first>W.</first>\n \
		</author>\n \
		<publisher>Addison-Wesley</publisher>\n \
		<price>65.95</price>\n \
	</book>\n \
</bib> \
",
						0);
*/
			/* get XQuery result and send it to the requester */
            WDVCAPI_XQuery(wdv, uri, "", (unsigned char*) querystring, strlen(querystring), &outStreamHandlerExplorer, reply);
            
/*			buffer = wdvhHandle->wdvGetBuffer;
			while(WDVCAPI_XQuery(wdv,
							(WDVCAPI_Buffer)buffer,
							wdvh_max_get_buffer_len,
							&readLength)) {
				sapdbwa_SendBody(reply,buffer,readLength);
				if (readLength != wdvh_max_get_buffer_len) 
					break;
			}
			if (WDVCAPI_XQueryClose(wdv)) {
			}
*/		}

        sp77sprintf(bodyData, 8000, "%s", 
						"</textarea>\
																	<!-- EndTemplateKey QueryString -->\
																</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<!-- Separator -->\
															<tr>\
																<td colspan='8'><img src='/WARoot/Images/1x1.gif' height='15'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table9'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",
						0);
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));


		if (erroroccured) {
            sp77sprintf(bodyData, 8000, "%s", 
							"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='25'></td>\
						</tr>\
					</table>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Error&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
								</table>",
							strlen("<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='25'></td>\
						</tr>\
					</table>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Error&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
									</tr>\
								</table>"));
            sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));
			
            sp77sprintf(bodyData, 8000, "%s", 
							"<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
												<tr>\
													<td class='text'>&nbsp;&nbsp;<img src='/WARoot/Images/xml_msgerrosmall.gif' border='0'>&nbsp;",
							strlen("<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
												<tr>\
													<td class='text'>&nbsp;&nbsp;<img src='/WARoot/Images/xml_msgerrosmall.gif' border='0'>&nbsp;"));
            sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));
	
            sapdbwa_SendBodyChunk(reply, errorText, strlen(errorText));
			
            sp77sprintf(bodyData, 8000, "%s", 
							"&nbsp;&nbsp;</td>\
												</tr>\
											</table>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",
							strlen("&nbsp;&nbsp;</td>\
												</tr>\
											</table>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>"));
            sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));
		}
	
        sp77sprintf(bodyData, 8000, "%s", 
					"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",
						strlen("<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>"));
        sapdbwa_SendBodyChunk(reply, bodyData, strlen(bodyData));

        sapdbwa_SendBodyChunkEnd(reply);  /* Send empty chunk as eof */

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	void collectionLockProps(sapdbwa_Handle			wa,
							 WDVH_Char				*uri,
							 WDVH_Char				*location,
							 WDVCAPI_WDV			wdv,
							 WDVCAPI_PropfindHandle propfind,
							 WDVH_Char				*locktoken,
					 		 WDVCAPI_LockHandle		*lock)
	{
		WDVCAPI_PropfindPropertyName				propertyName;
		WDVCAPI_PropfindPropertyShortValue			propertyShortValue;
		WDVCAPI_PropfindNameSpace					propfindNamespace;
		WDVCAPI_PropfindNameSpaceShortcut			propertyNamespaceShortcut;
		WDVCAPI_PropfindNameSpaceShortcut			davPropertyNamespaceShortcut = NULL;
		WDVH_XmlNamespaceList						namespaceList;
		WDVH_XmlNamespace							ns;

		/* build namespace list */
		if (!xmlParserCreateNamespaceList(NULL, &namespaceList)) {
		}
		if (WDVCAPI_PropfindGetFirstNameSpace(wdv,propfind,&propertyNamespaceShortcut,&propfindNamespace)) {
			do {
                /* Find out wich shortcut belongs to DAV: */
                if (!davPropertyNamespaceShortcut && strcmp(propfindNamespace, "DAV:") == 0) {
                    /* Shortcut found, so we have to memorize shortcut for later use */
                    davPropertyNamespaceShortcut = propertyNamespaceShortcut;
                }
				strcpy(ns.prefix,propertyNamespaceShortcut);
				strcpy(ns.uri,propfindNamespace);
				xmlParserAddNamespaceToList(NULL,ns,namespaceList);
			} while(WDVCAPI_PropfindGetNextNameSpace(wdv,propfind,&propertyNamespaceShortcut,&propfindNamespace));
		} else {
		}

		/* loop at properties */
		if (WDVCAPI_PropfindGetFirstProperty(wdv,propfind)) {
			do {
				if (WDVCAPI_PropfindGetPropertyName(wdv,propfind,&propertyName )) {
					WDVCAPI_PropfindGetPropertyNameSpaceShortcut(wdv,propfind,&propertyNamespaceShortcut);
					WDVCAPI_PropfindGetPropertyShortValue(wdv,propfind,&propertyShortValue);
					/* property lockdiscovery has to collect additional data */
					if (strcmp(propertyName,WDVH_XML_LOCKDISCOVERY)==0) {
						if (WDVCAPI_IdStringIsInitialValue(propertyShortValue)) {
							sp77sprintf(locktoken,100,"");
						} else {
							sp77sprintf(locktoken,100,"saplocktoken:%s",propertyShortValue);
							WDVCAPI_PropfindGetLockDescription(wdv,propfind,propertyShortValue,lock);
						}
					}
				}
			} while(WDVCAPI_PropfindGetNextProperty(wdv,propfind));
		}
	}

	void restoreScreen(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   WDVH_Bool				erroroccured,
						   WDVH_Char				*errormsg)
	{
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									__destination[WDVH_MAX_URI_LEN+1];
		WDVH_Char									*server;
		WDVH_Char									__temp[WDVH_MAX_URI_LEN+1];
		WDVCAPI_ErrorItem							__errorItem;
		WDVCAPI_ErrorType							__errorType;
		WDVCAPI_ErrorCode							__errorCode;

		sapdbwa_InitHeader(reply,
						   ERROR_200,
						   "text/html",
						   NULL,
						   NULL,								/* const char    *LastModified,*/
						   NULL,								/* const char    *Expires,*/
						   NULL);								/* const char    *Location );*/
		sapdbwa_SendHeader(reply);

		/* get server string for error messages */
		buildServerString(request,&server);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		if (!getParameterValue("DestinationURI",request,__destination))
			strcpy(__destination,"");
			
		/* build reply body */
		sapdbwa_SendBody(reply,
						"<head>\
	<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'>\
	<link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
		<script language='javascript'>\
		function restore()\
		{\
			document.showXmlProps.action = '",0);
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=Restore';\
			document.showXmlProps.target = '_self';\
			document.showXmlProps.submit();\
		}\
		</script>\
</head>\
<body topmargin='0' leftmargin='0'>\
	<form action='' method='post' name='showXmlProps' ID='Form1'>\
		<input type='hidden' name='res' value='",0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,uri,strlen(uri));
		sapdbwa_SendBody(reply,
						"'>\
		<input type='hidden' name='location' value='",0);
		sapdbwa_SendBody(reply,location,strlen(location));
		sapdbwa_SendBody(reply,
						"'>\
		<table border='0' cellpadding='0' cellspacing='0' ID='Table1'>\
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td class='text'><h1>Restore ",0);
		sapdbwa_SendBody(reply,
						uri,
						strlen(uri));
		sapdbwa_SendBody(reply,
						"</h1>",
						strlen("</h1>"));
		sapdbwa_SendBody(reply,
						"<tr>\
					<td>&nbsp;&nbsp;&nbsp;</td>\
					<td>\
						<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
							<tr>\
								<td>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
										</tr>\
									</table>\
									<!-- GroupboxTitle -->\
									<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
										<tr>\
											<td nowrap class='groupheader'>&nbsp;&nbsp;Location&nbsp;</td>\
										</tr>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
									</table>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
										</tr>\
									</table>",0);
		sapdbwa_SendBody(reply,
						"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Restore to</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																<input name=\"DestinationURI\" type=\"text\" size=\"75\" maxlength=\"512\" value=\"",0);
		if (strcmp(__destination,"")==0) {
			/* use the original location */
			if (!WDVCAPI_PropfindOriginalLocation(wdv,uri,__temp)) {
				WDVCAPI_GetLastError(wdv,&__errorItem);
				WDVCAPI_GetErrorType(__errorItem,&__errorType);
				switch (__errorType) {
					case WDVCAPI_ERR_TYPE_UNDEFINED:
					case WDVCAPI_ERR_TYPE_SQL: 
					case WDVCAPI_ERR_TYPE_CAPI: 
						sp77sprintf(__destination,WDVH_MAX_URI_LEN,"<Enter the path of the location this resource is moved to>");
				}
			} else {
				if (strcmp(__temp,"/")==0)
					sp77sprintf(__destination,WDVH_MAX_URI_LEN,"%s",strrchr(uri,'/'));
				else
					sp77sprintf(__destination,WDVH_MAX_URI_LEN,"%s%s",__temp,strrchr(uri,'/'));
			}
		}
		sapdbwa_SendBody(reply,__destination,0);
		sapdbwa_SendBody(reply,
						"\"></td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>",0);
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Overwrite</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																<input name=\"Overwrite\" type=\"checkbox\">",0);
		sapdbwa_SendBody(reply,
						"</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>",0);
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
						</tr>\
					</table>\					
					<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr><td><img src='/WARoot/Images/1x1.gif' width='1'></td>",0);
			sapdbwa_SendBody(reply,
							"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='javascript:restore()' class='buttonemph'>Restore</a>&nbsp;&nbsp;\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
											<td><img src='/WARoot/Images/1x1.gif' width='10'></td>",0);
		sapdbwa_SendBody(reply,
						"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='",0);
		sapdbwa_SendBody(reply,
						server,
						strlen(server));
		sapdbwa_SendBody(reply,
						prefix,
						strlen(prefix));
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"' class='buttonemph'>Cancel</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",
					0);
		if (erroroccured) {
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='25'></td>\
						</tr>\
					</table>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Error&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
								</table>",0);
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
												<tr>\
													<td class='text'>&nbsp;&nbsp;<img src='/WARoot/Images/xml_msgerrosmall.gif' border='0'>&nbsp;",0);
			sapdbwa_SendBody(reply,
							errormsg,
							strlen(errormsg));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;</td>\
												</tr>\
											</table>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",0);
		}
		sapdbwa_SendBody(reply,
					"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",0);

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	void mkColScreen(sapdbwa_Handle			wa,
				     sapdbwa_HttpRequestP	request,
				     sapdbwa_HttpReplyP		reply,
				     WDVH_Char				*uri,
					 WDVH_Char				*location,
					 WDVCAPI_WDV			wdv,
					 WDVH_Bool				erroroccured,
					 WDVH_Char				*errormsg)
	{
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									*server;
		WDVH_Char									__temp[WDVH_MAX_URI_LEN+1];
		WDVCAPI_ErrorItem							__errorItem;
		WDVCAPI_ErrorType							__errorType;
		WDVCAPI_ErrorCode							__errorCode;

		sapdbwa_InitHeader(reply,
						   ERROR_200,
						   "text/html",
						   NULL,
						   NULL,								/* const char    *LastModified,*/
						   NULL,								/* const char    *Expires,*/
						   NULL);								/* const char    *Location );*/
		sapdbwa_SendHeader(reply);

		/* get server string for error messages */
		buildServerString(request,&server);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		/* build reply body */
		sapdbwa_SendBody(reply,
						"<head>\
	<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'>\
	<link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
		<script language='javascript'>\
		function mkcol()\
		{\
			if (document.mkcol.collection.value != '') {\
				document.mkcol.action = '",0);
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,
						"?explorer=0&cmd=MkCol';\
				document.mkcol.target = '_self';\
				document.mkcol.submit();\
				}\
		}\
		</script>\
</head>\
<body topmargin='0' leftmargin='0'>\
	<form action='' method='post' name='mkcol' ID='Form1'>\
		<input type='hidden' name='res' value='",0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,uri,strlen(uri));
		sapdbwa_SendBody(reply,
						"'>\
		<input type='hidden' name='location' value='",0);
		sapdbwa_SendBody(reply,location,strlen(location));
		sapdbwa_SendBody(reply,
						"'>\
		<table border='0' cellpadding='0' cellspacing='0' ID='Table1'>\
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td class='text'><h1>Create collection in collection ",0);
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"</h1>",
						strlen("</h1>"));
		sapdbwa_SendBody(reply,
						"<tr>\
					<td>&nbsp;&nbsp;&nbsp;</td>\
					<td>\
						<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
							<tr>\
								<td>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
										</tr>\
									</table>\
									<!-- GroupboxTitle -->\
									<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
										<tr>\
											<td nowrap class='groupheader'>&nbsp;&nbsp;Location&nbsp;</td>\
										</tr>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
									</table>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
										</tr>\
									</table>",0);
		sapdbwa_SendBody(reply,
						"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Collection name</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td><input class='input_field' type='text' name='collection' size='75%' value='",0);
		if (uri[0] != '/' && uri[0] != '\\')
			sapdbwa_SendBody(reply,
							uri,
							strlen(uri));
		else
			sapdbwa_SendBody(reply,
							uri+1,
							strlen(uri+1));
		sapdbwa_SendBody(reply,
						 "'></input></td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>",0);
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
						</tr>\
					</table>\					
					<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr><td><img src='/WARoot/Images/1x1.gif' width='1'></td>",0);
			sapdbwa_SendBody(reply,
							"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='javascript:mkcol()' class='buttonemph'>Create</a>&nbsp;&nbsp;\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
											<td><img src='/WARoot/Images/1x1.gif' width='10'></td>",0);
		sapdbwa_SendBody(reply,
						"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='",0);
		sapdbwa_SendBody(reply,
						server,
						strlen(server));
		sapdbwa_SendBody(reply,
						prefix,
						strlen(prefix));
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"' class='buttonemph'>Cancel</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",
					0);
		if (erroroccured) {
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='25'></td>\
						</tr>\
					</table>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='700'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Error&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='700'></td>\
									</tr>\
								</table>",0);
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
												<tr>\
													<td class='text'>&nbsp;&nbsp;<img src='/WARoot/Images/xml_msgerrosmall.gif' border='0'>&nbsp;",0);
			sapdbwa_SendBody(reply,
							errormsg,
							strlen(errormsg));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;</td>\
												</tr>\
											</table>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",0);
		}
		sapdbwa_SendBody(reply,
					"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",0);

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	void uploadFileScreen(sapdbwa_Handle			wa,
						   sapdbwa_HttpRequestP		request,
						   sapdbwa_HttpReplyP		reply,
						   WDVH_Char				*uri,
						   WDVH_Char				*location,
						   WDVCAPI_WDV				wdv,
						   WDVH_Bool				erroroccured,
						   WDVH_Char				*errormsg)
	{
		WDVH_Char									prefix[WDVH_MAX_URI_LEN+1];
		WDVH_Char									*server;
		WDVH_Char									__temp[WDVH_MAX_URI_LEN+1];
		WDVCAPI_ErrorItem							__errorItem;
		WDVCAPI_ErrorType							__errorType;
		WDVCAPI_ErrorCode							__errorCode;

		sapdbwa_InitHeader(reply,
						   ERROR_200,
						   "text/html",
						   NULL,
						   NULL,								/* const char    *LastModified,*/
						   NULL,								/* const char    *Expires,*/
						   NULL);								/* const char    *Location );*/
		sapdbwa_SendHeader(reply);

		/* get server string for error messages */
		buildServerString(request,&server);
		/* set server prefix */
		getWebDAVPrefix(wa,prefix);

		/* build reply body */
		sapdbwa_SendBody(reply,
						"<head>\
	<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'>\
	<link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
		<script language='javascript'>\
		function upload()\
		{ \
			if (document.uploadFile.xmldb.checked == false) \
				querystring = \"?explorer=0&cmd=Upload&isxmldb=0&location=\"+document.uploadFile.location.value; \
			else \
				querystring = \"?explorer=0&cmd=Upload&isxmldb=1&location=\"+document.uploadFile.location.value; \
			document.uploadFile.action = '",0);
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,
						"' + querystring;\
			document.uploadFile.target = '_self';\
			document.uploadFile.submit();\
		}\
		</script>\
</head>\
<body topmargin='0' leftmargin='0'>\
	<form name='uploadFile' METHOD='POST' action='' ENCTYPE='multipart/form-data'>	<form action='' method='post' name='uploadFile' ID='Form1'>\
		<input type='hidden' name='res' value='",0);
		sapdbwa_SendBody(reply,server,strlen(server));
		sapdbwa_SendBody(reply,prefix,strlen(prefix));
		sapdbwa_SendBody(reply,uri,strlen(uri));
		sapdbwa_SendBody(reply,
						"'>\
		<input type='hidden' name='location' value='",0);
		sapdbwa_SendBody(reply,location,strlen(location));
		sapdbwa_SendBody(reply,
						"'>\
		<table border='0' cellpadding='0' cellspacing='0' ID='Table1'>\
			<tr>\
				<td>&nbsp;&nbsp;&nbsp;</td>\
				<td class='text'><h1>Upload File to collection ",0);
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"</h1>",
						strlen("</h1>"));
		sapdbwa_SendBody(reply,
						"<tr>\
					<td>&nbsp;&nbsp;&nbsp;</td>\
					<td>\
						<table border='0' cellpadding='0' cellspacing='0' width='10%' ID='Table2'>\
							<tr>\
								<td>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table7'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
										</tr>\
									</table>\
									<!-- GroupboxTitle -->\
									<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%' ID='Table3'>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
										<tr>\
											<td nowrap class='groupheader'>&nbsp;&nbsp;Location&nbsp;</td>\
										</tr>\
										<tr>\
											<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='500'></td>\
										</tr>\
									</table>\
									<!-- Separator -->\
									<table border='0' cellpadding='0' cellspacing='0' ID='Table4'>\
										<tr>\
											<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
										</tr>\
									</table>",0);
		sapdbwa_SendBody(reply,
						"<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' ID='Table8'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table11'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table5'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>File name</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td><input class='input_field' type='file' name='import' size='65%'></input></td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>",0);
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
													<td class='text'>\
														<table border='0' cellpadding='0' cellspacing='0' ID='Table6'>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' width='150' height='1'></td>\
															</tr>\
															<tr>\
																<td valign='bottom' nowrap height='25' class='inputlabel' align='absmiddle'><img src='/WARoot/Images/xml_labelnotch.gif'>Import into XMLDB</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='10'></td>\
																<td>\
																<input name=\"xmldb\" type=\"checkbox\">",0);
		sapdbwa_SendBody(reply,
						"</td>\
																<td><img src='/WARoot/Images/1x1.gif' width='20'></td>\
															</tr>\
															<tr>\
																<td colspan='2' bgcolor='#ffffff'><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif'></td>\
															</tr>",0);
		sapdbwa_SendBody(reply,
						"<!-- Separator -->\
															<tr>\
																<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
															</tr>\
														</table>\
													</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>\
					<!-- Separator -->\
					<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%' ID='Table20'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='20'></td>\
						</tr>\
					</table>\					
					<!-- Toolbar -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#FFFFFF' width='100%' ID='Table10'>\
						<tr>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' ID='Table9'>\
									<tr><td><img src='/WARoot/Images/1x1.gif' width='1'></td>",0);
			sapdbwa_SendBody(reply,
							"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
											<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
												&nbsp;&nbsp;<a href='javascript:upload()' class='buttonemph'>Upload</a>&nbsp;&nbsp;\
											</td>\
											<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
											<td><img src='/WARoot/Images/1x1.gif' width='10'></td>",0);
		sapdbwa_SendBody(reply,
						"<td align='right'><img src='/WARoot/Images/xml_btnl1.gif'></td>\
										<td background='/WARoot/Images/xml_btnb1.gif' align='middle' class='button3' nowrap>\
											&nbsp;&nbsp;<a href='",0);
		sapdbwa_SendBody(reply,
						server,
						strlen(server));
		sapdbwa_SendBody(reply,
						prefix,
						strlen(prefix));
		sapdbwa_SendBody(reply,
						location,
						strlen(location));
		sapdbwa_SendBody(reply,
						"' class='buttonemph'>Cancel</a>&nbsp;&nbsp;\
										</td>\
										<td><img src='/WARoot/Images/xml_btnr1.gif'></td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",
					0);
		if (erroroccured) {
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='25'></td>\
						</tr>\
					</table>\
					<table border='0' cellpadding='0' cellspacing='0' width='10%'>\
						<tr>\
							<td>\
								<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='10'></td>\
									</tr>\
								</table>\
								<!-- GroupboxTitle -->\
								<table border='0' cellpadding='0' cellspacing='0' bgcolor='#CBD5E1' width='100%'>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
									<tr>\
										<td nowrap class='groupheader'>&nbsp;&nbsp;Error&nbsp;</td>\
									</tr>\
									<tr>\
										<td class='groupheader'><img src='/WARoot/Images/1x1.gif' height='5' width='660'></td>\
									</tr>\
								</table>",0);
			sapdbwa_SendBody(reply,
							"<!-- Separator -->\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>\
										<td><img src='/WARoot/Images/1x1.gif' height='1'></td>\
									</tr>\
								</table>\
								<table border='0' cellpadding='0' cellspacing='1' bgcolor='#DCE3EC' width='100%'>\
									<tr>\
										<td>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
											<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%'>\
												<tr>\
													<td class='text'>&nbsp;&nbsp;<img src='/WARoot/Images/xml_msgerrosmall.gif' border='0'>&nbsp;",0);
			sapdbwa_SendBody(reply,
							errormsg,
							strlen(errormsg));
			sapdbwa_SendBody(reply,
							"&nbsp;&nbsp;</td>\
												</tr>\
											</table>\
											<!-- Separator -->\
											<table bgcolor='#ffffff' border='0' cellpadding='0' cellspacing='0' width='100%'>\
												<tr>\
													<td><img src='/WARoot/Images/1x1.gif' height='5'</td>\
												</tr>\
											</table>\
										</td>\
									</tr>\
								</table>\
							</td>\
						</tr>\
					</table>",0);
		}
		sapdbwa_SendBody(reply,
					"<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0' bgcolor='#ffffff' width='100%' ID='Table22'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='5'></td>\
						</tr>\
					</table>\
				</td>\
			</tr>\
		</table>\
	</form>\
</body>",0);

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}

	SAPDB_Bool uploadFile(sapdbwa_Handle		wa,
						  sapdbwa_HttpRequestP  req,
						  sapdbwa_HttpReplyP    rep,
						  WDVH_Char				*location,
						  WDVCAPI_WDV			wdv,
						  WDVH_Handle			wdvh,
						  WDVH_Bool				compress,
						  WDVH_Bool				*errorsoccured,
						  WDVH_Char				*errormsg)
	{

		WDVH_Bool               bRet = WDVH_True;
		WDVH_Bool				haveBoundary = WDVH_False;
		WDVH_ULong             boundaryStart = 0;
		WDVH_ULong             boundaryLen = 0;
		const char              *contentType = NULL;
		WDVH_Char               boundary[100];


		contentType = sapdbwa_GetContentType(req);
		haveBoundary = sapdbwa_ExtractValue(contentType, 
											strlen(contentType), 
											"BOUNDARY=", 
											"boundary=", 
											&boundaryStart, 
											&boundaryLen);
		if(haveBoundary == WDVH_True && boundaryLen < 100) {
			SAPDB_memcpy(boundary, contentType + boundaryStart, boundaryLen);
			boundary[boundaryLen] = 0;
			bRet = storeMimeParts(wa, req, location, wdv, wdvh, compress, errorsoccured, errormsg, boundary);
			if(bRet == WDVH_False) {
			}
		}
		return bRet;
	}

	/*
	* HTML file upload is generally done using the encoding type 'multipart/form-data' for all of the form data.
	* For example, if one has the following HTML form :
	*
	* <form name='frmImport' METHOD='POST' action='SOMEURL' ENCTYPE='multipart/form-data'>
	*          <input class='input_field' type='file' name='import'></input>
	*          <input type='hidden' name='actiontotake' value='/'></input>   
	*  </form>
	*
	* Then the Content-Type HTTP header is (e.g.) of the form 
	*
	* content type=multipart/form-data; boundary=---------------------------7d3541c073e
	* 
	* and the request body looks like this (where a "/r/n" sequence is denoted by <CR><LF> ) :
	* 
	* -----------------------------7d3541c073e<CR><LF>
	* Content-Disposition: form-data; name="import"; filename="D:\SAPDevelop\VTools\src\SQLStudio\Main\SQLStudio\Independent\Local Folder\cnt"<CR><LF>
	* Content-Type: application/octet-stream<CR><LF>
	* <CR><LF>
	* 13?1?select count(*) from "REPMAN"."REPMAN_SENDORDER"<CR>?????1?1?2?72200?<CR><LF>
	* -----------------------------7d3541c073e<CR><LF>
	* Content-Disposition: form-data; name="actiontotake"<CR><LF>
	* <CR><LF>
	* /<CR><LF>
	* -----------------------------7d3541c073e--<CR><LF>
	* 
	* In RFC speak this is a 'multipart entity consisting of two parts'
	* (according to the input elements in the HTML form).
	* The method 'storeMimeParts' does not rely on the web browser preserving the 
	* order of the HTML input elements in the multipart entity.
	*/
	WDVH_Bool storeMimeParts(sapdbwa_Handle			wa,
							 sapdbwa_HttpRequestP	req,
							 WDVH_Char				*location,
							 WDVCAPI_WDV			wdv,
							 WDVH_Handle			wdvh,
							 WDVH_Bool				compress,
						     WDVH_Bool				*errorsoccured,
							 WDVH_Char				*errormsg,
							 const char				*boundary)
	{
		WDVH_Char				buf[wdvh_max_put_buffer_len];
		WDVH_ULong				read_len            = 0;

		WDVH_Char				hdr_buf[wdvh_max_put_buffer_len];
		WDVH_ULong				hdr_read_len        = 0;

		WDVH_Char				contenttype[1001];

		WDVH_Bool				retval                 = WDVH_True;
		WDVH_Bool				Ok                     = WDVH_False;
		WDVH_Bool				moreParts              = WDVH_True;
		WDVH_Bool				moreData               = WDVH_False;
		WDVH_Bool				moreHeaderFields       = WDVH_False;

		WDVH_Bool				found                = WDVH_False;
		WDVH_Bool				fileInputFound       = WDVH_False;
		WDVH_ULong				filePathStart        = 0;
		WDVH_ULong				filePathLen          = 0;
		WDVH_ULong				fileNameStart        = 0;
		WDVH_ULong				fileNameLen          = 0;
		WDVH_ULong				contentTypeStart     = 0;
		WDVH_ULong				contentTypeLen       = 0;

		WDVH_Char               subBoundary[100];
		WDVH_Bool				isMultipartFound  = WDVH_False;
		WDVH_Bool				fileContentTypeFound = WDVH_False;

		WDVH_Char				tempname[1001];
		WDVH_StringBuffer		filename;


		if ((filename = createStrBuffer(wdvh_max_put_buffer_len)) == NULL) {
			return WDVH_False;
		}
		if (location[strlen(location)-1] == '/' || location[strlen(location)-1] == '\\')
			location[strlen(location)-1] = '\0';
		appendStrBuffer(filename,location);

		Ok = sapdbwa_MimeMultipartBody_NextPart(req,boundary, &moreParts);
		while(Ok == WDVH_True && moreParts == WDVH_True) {
			do {
				Ok = sapdbwa_MimeMultipartBody_PartHeaderField(req,
																hdr_buf, 
																wdvh_max_put_buffer_len, 
																&hdr_read_len, 
																&moreData, 
																&moreHeaderFields);
				if(Ok != WDVH_True) break;

				Ok = getFileInput(hdr_buf, 
								hdr_read_len, 
								&found,
								&filePathStart,
								&filePathLen,
								&fileNameStart,
								&fileNameLen);
				if(Ok != WDVH_True) break;
				if(found == WDVH_True && fileNameLen > 0) {
					fileInputFound = WDVH_True;
					if (fileNameLen <= 1000) {
						if (*(hdr_buf+fileNameStart) != '/' && *(hdr_buf+fileNameStart) != '\\') {
							SAPDB_memcpy(tempname,hdr_buf+fileNameStart,fileNameLen);
							tempname[fileNameLen] = '\0';
						} else {
							SAPDB_memcpy(tempname,hdr_buf+fileNameStart+1,fileNameLen-1);
							tempname[fileNameLen-1] = '\0';
						}
					} else {
						if (*(hdr_buf+fileNameStart) != '/' && *(hdr_buf+fileNameStart) != '\\') {
							SAPDB_memcpy(tempname,hdr_buf+fileNameStart,1000);
							tempname[1000] = '\0';
						} else {
							SAPDB_memcpy(tempname,hdr_buf+fileNameStart+1,1000);
							tempname[1000] = '\0';
						}
					}
					appendStrBuffer(filename,"/");
					appendStrBuffer(filename,tempname);
				}

				Ok = getIsMultipart(hdr_buf, 
									hdr_read_len, 
									&found,
									subBoundary);
				if(Ok != WDVH_True) break;
				isMultipartFound = (found == WDVH_True || isMultipartFound == WDVH_True) ? WDVH_True : WDVH_False;

				if (fileInputFound == WDVH_True) {
					fileContentTypeFound = sapdbwa_ExtractValue(hdr_buf, 
																hdr_read_len, 
																"CONTENT-TYPE: ", 
																"content-type: ", 
																&contentTypeStart, 
																&contentTypeLen);
					if (fileContentTypeFound == WDVH_True) {
						if (contentTypeLen <= 1000) {
							SAPDB_memcpy(contenttype,hdr_buf+contentTypeStart,contentTypeLen);
							contenttype[contentTypeLen] = '\0';
						} else {
							SAPDB_memcpy(contenttype,hdr_buf+contentTypeStart,1000);
							contenttype[1000] = '\0';
						}
					}
					fileContentTypeFound = (found == WDVH_True || fileContentTypeFound == WDVH_True) ? WDVH_True : WDVH_False;
				}

			} while(moreHeaderFields == WDVH_True && Ok == WDVH_True);

			if(Ok != WDVH_True) break;

			if(fileInputFound  == WDVH_True) {
				/* read the file and stream it to the CAPI */
				putFile(wa,req,wdv,wdvh,getStrBufferContent(filename),compress,boundary,&moreParts,contenttype,errorsoccured,errormsg);
			} else {
				Ok = sapdbwa_MimeMultipartBody_NextPart(req, boundary, &moreParts);
				if(Ok != WDVH_True) break;
			}
			fileInputFound     = WDVH_False;
		} /* while(Ok == WDVH_True && moreParts == WDVH_True) { ... */
		if(Ok != WDVH_True) {
			retval = WDVH_False;
		}
		destroyStrBuffer(filename);
		return retval;
	}

	/*
	* scans a part header line for 
	* "content-type: form-data; name="import" boundary="<mime boundary>" .
	* Returns true, or false if an error occurred.
	*/
	WDVH_Bool getIsMultipart(const WDVH_Char		*buf, 
							 WDVH_ULong				buflen, 
							 WDVH_Bool				*found,
							 WDVH_Char				*subBoundary) 
	{
		WDVH_Bool		retval                  = WDVH_True;
		WDVH_Bool		haveContentType         = WDVH_False;
		WDVH_ULong		contentTypeStart        = 0;
		WDVH_ULong		contentTypeLen          = 0;
		WDVH_Bool		haveBoundary            = WDVH_False;
		WDVH_ULong		boundaryStart           = 0;
		WDVH_ULong		boundaryLen             = 0;


		*found = WDVH_False;
	                            
		haveContentType = sapdbwa_ExtractValue(buf, buflen, "CONTENT-TYPE: ", "content-type: ", &contentTypeStart, &contentTypeLen);
		if(haveContentType == WDVH_True &&
		compareBuf(buf + contentTypeStart, "FORM-DATA", "form-data", contentTypeLen)) {
			haveBoundary = sapdbwa_ExtractValue(buf, buflen, "BOUNDARY=", "boundary=", &boundaryStart, &boundaryLen);
			if(haveBoundary == WDVH_True) {
				/*printbuf(buf+boundaryStart, boundaryStart);*/
				*found = WDVH_True;
			}
			else {
				retval = WDVH_False;
			}
		}
		return retval;
	}

	/*
	* scans a part header line for 
	* "Content-Disposition: form-data; name="import" filename="<path to a file>" .
	* Returns true, or false if an error occurred.
	*/
	WDVH_Bool getFileInput(const WDVH_Char		*buf, 
						   WDVH_ULong			buflen, 
						   WDVH_Bool			*found,
						   WDVH_ULong			*filePathStart,
						   WDVH_ULong			*filePathLen,
						   WDVH_ULong			*fileNameStart,
						   WDVH_ULong			*fileNameLen) 
	{
		WDVH_Bool		retval                  = WDVH_True;
		WDVH_Bool		haveContentDisposition  = WDVH_False;
		WDVH_ULong		contentDispositionStart = 0;
		WDVH_ULong		contentDispositionLen   = 0;
		WDVH_Bool		haveName                = WDVH_False;
		WDVH_ULong		nameStart               = 0;
		WDVH_ULong		nameLen                 = 0;
		WDVH_Bool		haveFilePath            = WDVH_False;


		*found = WDVH_False;
	                            
		haveContentDisposition = sapdbwa_ExtractValue(buf, buflen, "CONTENT-DISPOSITION: ", "content-disposition: ", 
																&contentDispositionStart, &contentDispositionLen);
		if(haveContentDisposition == WDVH_True &&
		compareBuf(buf + contentDispositionStart, "FORM-DATA", "form-data", contentDispositionLen)) {
			haveName = sapdbwa_ExtractValue(buf, buflen, "NAME=", "name=", &nameStart, &nameLen);
			if(haveName == WDVH_True) {
				if(compareBuf(buf + nameStart, WDVH_IMPORT, WDVH_IMPORT, nameLen)) {
					haveFilePath = sapdbwa_ExtractValue(buf, buflen, "FILENAME=", "filename=", filePathStart, filePathLen);
					if(haveFilePath == WDVH_True) {
						trimFileName(buf, filePathStart, filePathLen, fileNameStart, fileNameLen);
						/*printbuf(buf+fileNameStart, fileNameLen);*/
						*found = WDVH_True;
					}
					else {
						retval = WDVH_False;
					}
				}
			}
		}
		return retval;
	}

	/*
	* returns offset and length of the basename of a file or directory in a full path.
	* If the path ends with '/'es or '\'es , these are ignored.
	* The character sequence of the full path is given in 'buf' with offset 'filePathStart'
	* and length 'filePathLen'.
	* 
	*/
	void trimFileName(const WDVH_Char	*buf, 
					  WDVH_ULong		*filePathStart, 
					  WDVH_ULong		*filePathLen, 
					  WDVH_ULong		*fileNameStart, 
					  WDVH_ULong		*fileNameLen)
	{

		WDVH_ULong idx;
		WDVH_ULong len;

		for(len = *filePathLen; len > 0; len--) {
			if(buf[*filePathStart+len-1] == '/' || buf[*filePathStart+len-1] == '\\') {
				len--;
			}
			else {
				break;
			}
		}
		for(idx = 0; idx < len; idx++) {
			if(buf[*filePathStart+len-idx-1] == '/' || buf[*filePathStart+len-idx-1] == '\\') {
				break;
			}
		}
		*fileNameStart = *filePathStart+len-idx;
		*fileNameLen = idx;
	}

	/*
	* Compares the character sequence in 'buf' against the sequences given in 'upper' resp. 'lower'.
	* a character at a certain position in 'buf' matches, if if matches the character in 'lower'
	* _or_ in 'upper' at that position.
	* This allows e.g. case insensitive comparisons.
	* Parameter 'upper' and 'lower' must have the same length of data but need not be 
	* null terminated.
	* Care must be taken of that parameter len does not exceed the data length of
	* 'buf', 'lower', or 'upper'.
	* Comparison is done byte-wise, encodings are not respected.
	* True is returned if a matching can be achieved, false otherwise.
	*/
	WDVH_Bool compareBuf(const WDVH_Char	*buf,
						 const WDVH_Char	*lower,
						 const WDVH_Char	*upper,
						 WDVH_ULong			len) 
	{
		WDVH_ULong idx;
		WDVH_Bool retval = WDVH_True;

		for(idx = 0; idx < len; idx++) {
			if(buf[idx] != lower[idx] && buf[idx] != upper[idx]) {
				retval = WDVH_False;
				break;
			}
		}
		return retval;
	}

	/*
	* scans a part header line for 
	* "Content-Disposition: form-data; name="<inputElementName>" .
	* Returns true, or false if an error occurred.
	*/
	WDVH_Bool getInputElement(const WDVH_Char	*buf,
							  WDVH_ULong		buflen,
							  const WDVH_Char	*inputElementName,
							  WDVH_Bool			*found) 
	{
		WDVH_Bool		retval                  = WDVH_True;
		WDVH_Bool		haveContentDisposition  = WDVH_False;
		WDVH_ULong		contentDispositionStart = 0;
		WDVH_ULong		contentDispositionLen   = 0;
		WDVH_Bool		haveName                = WDVH_False;
		WDVH_ULong		nameStart               = 0;
		WDVH_ULong		nameLen                 = 0;


		*found = WDVH_False;
	                            
		haveContentDisposition = sapdbwa_ExtractValue(buf, buflen, "CONTENT-DISPOSITION: ", "content-disposition: ", 
																&contentDispositionStart, &contentDispositionLen);
		if(haveContentDisposition == WDVH_True &&
		compareBuf(buf + contentDispositionStart, "FORM-DATA", "form-data", contentDispositionLen)) {
			haveName = sapdbwa_ExtractValue(buf, buflen, "NAME=", "name=", &nameStart, &nameLen);
			if(haveName == WDVH_True) {
				if(compareBuf(buf+nameStart, inputElementName, inputElementName, nameLen)) {
					/*printbuf(buf+nameStart, nameLen);*/
					*found = WDVH_True;
				}
			}
		}
		return retval;
	}

	sapdbwa_Int2 putFile(sapdbwa_Handle			wa, 
						 sapdbwa_HttpRequestP	request, 
						 WDVCAPI_WDV			pCapiHandle, 
						 WDVH_Handle			wdvh,
						 WDVCAPI_URI			requesturi,
						 WDVH_Bool				compress,
						 const WDVH_Char		*boundary,
						 WDVH_Bool				*moreParts,
						 WDVH_Char				*putContentType,
						 WDVH_Bool				*errorsoccured,
						 WDVH_Char				*errormsg)
	{
		sapdbwa_Int2			rc;
		WDVH_PutBuffer			buffer;
		WDVH_ULong				read_len            = 0;
	    WDVH_Int2				done;
		WDVH_ULong				pos;
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
		const WDVH_Char			*__isxmldb;
		WDVH_Bool				__xmldb;
		WDVH_Char				*server;
		WDVCAPI_XmlDocClassId	docClassId;
		WDVH_Bool				async = WDVH_False;
		WDVCAPI_LockIdString	locktoken;
		WDVH_Bool				moreData               = WDVH_False;
		WDVH_Bool				Ok                     = WDVH_False;
        WDVHStreamDescExplorer  streamDesc;

		buffer = wdvh->wdvPutBuffer;
		WDVCAPI_IdInitValue(docClassId);
		WDVCAPI_IdStringInitValue(locktoken);

		/* get the flag whether to store the doc in classic style or in xmldb */
		__isxmldb = getParameterValueWithoutCopy("isxmldb",request);
		if (strcmp(__isxmldb,"0")==0) 
			__xmldb = WDVH_False;
		else
			__xmldb = WDVH_True;

		if (WDVCAPI_PutOpen(pCapiHandle,
							requesturi,
							(WDVCAPI_ContentType)putContentType,
							docClassId,
							async,
							locktoken,
							compress,
                            __xmldb,
                            &inStreamHandlerExplorer,
                            &streamDesc,
							&resourceCreated)) {

            if (__xmldb == WDVH_True) {
                streamDesc.request = request;
                streamDesc.moreData = WDV_True;
		        streamDesc.boundary = boundary;
		        streamDesc.moreParts = moreParts;

                if (!WDVCAPI_PutXDB(pCapiHandle)) {
					/* error handling */
					success = 0;
				} else {
					success = 1;
			    }
            } else {
			    do {
				    /* get another chunk */
				    Ok = sapdbwa_MimeMultipartBody_PartBody(request,
														    boundary, 
														    buffer, 
														    wdvh_max_put_buffer_len, 
														    &read_len, 
														    &moreData, 
														    moreParts);
				    if (Ok != WDVH_True) break;
				    /* put another chunk */
				    if (!WDVCAPI_Put(pCapiHandle,(WDVCAPI_Buffer)buffer,read_len)) {
					    /* error handling */
					    success = 0;
				    } else {
					    success = 1;
				    }
			    } while(moreData == WDVH_True && Ok == WDVH_True);
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
						*errorsoccured = WDVH_True;
						sp77sprintf(errormsg,1000,"Internal server error. Please contact your System Administrator.");
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
									*errorsoccured = WDVH_True;
									error = ERROR_409;
									sp77sprintf(errormsg,1000,"At least one parent does not exist.");
									break;
								}
							/* Correction Begin http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							case WDVCAPI_ERR_CODE_PARENT_NOT_A_COLLECTION:
								{
									*errorsoccured = WDVH_True;
									error = ERROR_403;
									sp77sprintf(errormsg,1000,"At least one parent is no collection.");
									break;
								}
							/* Correction End http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
							case WDVCAPI_ERR_CODE_NO_PUT_ON_COLLECTION:
								{
									*errorsoccured = WDVH_True;
									sp77sprintf(errormsg,1000,"The filename given is the name of a collection. Please change the name of the file.");
									error = ERROR_405;
									break;
								}
							case WDVCAPI_ERR_CODE_LOCK_EXISTS:
								{
									*errorsoccured = WDVH_True;
									sp77sprintf(errormsg,1000,"The document is locked.");
									error = ERROR_423;
									break;
								}
							default:
								{
									*errorsoccured = WDVH_True;
									sp77sprintf(errormsg,1000,"Internal server error. Please contact your System Administrator.");
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
						*errorsoccured = WDVH_True;
						sp77sprintf(errormsg,1000,"Internal server error. Please contact your System Administrator.");
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
			error = ERROR_201;
		}

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
		return error;
	}

	/*------------------------------------------------------------------------
		Function:	inStreamHandlerExplorer
	--------------------------------------------------------------------------*/
    void inStreamHandlerExplorer( void           *userData,
                                  void           *buffer,
                                  SAPDB_Int4      bufferSize,
                                  SAPDB_Int4     *readLen )
    {

        SAPDB_ULong waReadLen = 0;
        WDVHStreamDescExplorer  *streamDesc = (WDVHStreamDescExplorer*)userData;

        *readLen = 0;

        if (!streamDesc) {
            return;
        }

        if (streamDesc->moreData == WDV_False) {
            return;
        }

		/* get another chunk */
		sapdbwa_MimeMultipartBody_PartBody(streamDesc->request,
                                           streamDesc->boundary,
                                           buffer, bufferSize, &waReadLen,
                                           &streamDesc->moreData,
                                           streamDesc->moreParts);

        *readLen = (int)waReadLen;

}

	/*------------------------------------------------------------------------
		Function:	outStreamHandlerExplorer - Callback function for stream handling
	--------------------------------------------------------------------------*/
	void outStreamHandlerExplorer( void *userData, void *buffer, SAPDB_Int4 writeLen )
    {

		sapdbwa_HttpReplyP reply = (sapdbwa_HttpReplyP) userData;

        sapdbwa_SendBodyChunk(reply, buffer, writeLen);

    }
