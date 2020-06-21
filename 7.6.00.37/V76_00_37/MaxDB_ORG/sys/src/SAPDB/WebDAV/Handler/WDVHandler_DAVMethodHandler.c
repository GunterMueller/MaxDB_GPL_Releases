/****************************************************************************

  module      : WDVHandler_DAVMethodHandler.c

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

	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethodHandler.h"
	#include "hwd21req.h"


	/*------------------------------------------------------------------------
		Function: webdavMkcolHandler
	--------------------------------------------------------------------------*/
	void webdavMkcolHandler( sapdbwa_Handle		wa,
						     sapdbwa_HttpRequestP request,
						     sapdbwa_HttpReplyP   reply )
	{
		WDVH_Int2				statuscode;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Handle				wdvhHandle;
/*		WDVH_Char				sIf[100];*/
		WDVH_Char				*pIf;
		WDVH_Char				errormsg[1001];				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */
		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif

		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavMkcolHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavMkcolHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_MKCOL,"");
			return;
		}

		/* first get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			/* get wdvh handle */
			wdvhHandle = getWdvhHandle(wa,connection);
			strcpy(errormsg,"");
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_MKCOL,"");
				return;
			}
			if (wdvhHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDVH Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_MKCOL,"");
				return;
			}

			/* get request uri */
			getRequestUri(wa,request,wdvhHandle->requesturi);
			wdvhHandle->request = request;
			wdvhHandle->reply = reply;
			wdvhHandle->wa = wa;

			/* interpret If-Header */
	/* ===> TODO!!!!! */
			getIfHeader(request,wdvhHandle->locktoken,WDVCAPI_MAX_ID_STRING_LEN);

			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* call CAPI function*/
/*			statuscode = mkcolCallCapiFunc(wa,pCapiHandle,request,requestURI,pIf);*/
			statuscode = mkcolCallCapiFunc(wdvhHandle,pCapiHandle,errormsg);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CapiMkCol   : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			mkcolBuildReply(wdvhHandle,statuscode,errormsg);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
			
			/* Clean Up */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_MKCOL_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_MKCOL,"");
		}
	}

	/*------------------------------------------------------------------------
		Function: webdavDeleteHandler
	--------------------------------------------------------------------------*/
	void webdavDeleteHandler( sapdbwa_Handle		wa,
						     sapdbwa_HttpRequestP request,
						     sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Char				*host;
		WDVH_Char			    *port;
		WDVH_Int2				statuscode;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		WDVH_Bool				deleteditems;
		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif

		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavDeleteHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavDeleteHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_DELETE,"");
			return;
		}

		
		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);

		/* interpret If-Header */
		getIfHeader(request,sIf,100);
		pIf = sIf;

		/* get SAPDB_DeletedItems Header */
		getDeletedItemsHeader(request,&deleteditems);
		
		/* first get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_DELETE,"");
				return;
			}
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* call CAPI function*/
			statuscode = deleteCallCapiFunc(wa,pCapiHandle,request,requestURI,pIf,deleteditems);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CapiMkCol   : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			deleteBuildReply(statuscode, reply);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
			
			/* Clean Up */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* finally return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_DELETE_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_DELETE,"");
		}

	}


	/*------------------------------------------------------------------------
		Function: webdavGetHandler
	--------------------------------------------------------------------------*/
	void webdavGetHandler( sapdbwa_Handle		wa,
				   	       sapdbwa_HttpRequestP request,
						   sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Int2				statuscode;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Handle				wdvhHandle;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Char				*crFrom,*crTo;						/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
		WDVH_Long				contentRangeFrom,contentRangeTo;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
        WDV_Bool                isDocInXDB=WDVH_False;

		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavGetHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavGetHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_GET,"");
			return;
		}

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);

		/* interpret Header's */
/* ===> TODO!!!!! */
		getIfHeader(request,sIf,100);
		pIf = sIf;
		
		/* Begin Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
		crFrom = NULL; crTo = NULL;
		if (getContentRangeHeader(request,&crFrom,&crTo)) {	
			/* test the values of the content range header */
			if (crFrom==NULL || crTo==NULL) {
				/* invalid header */
				getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
				return;
			} else if (strcmp(crFrom,"")==0 && strcmp(crTo,"")==0) {
				/* invalid header */
				getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
				return;
			} else if (strcmp(crFrom,"")==0) {
				/* no "from" is given -> we have a suffix-byte-range-spec */
				if (isNumber(crTo)) {
					contentRangeFrom = -1;
					contentRangeTo = atol(crTo);
					if (contentRangeTo <= 0) {
						getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
						return;
					}
				} else {
					/* invalid header */
					getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
					return;
				}
			} else if (strcmp(crTo,"")==0) {
				/* no "to" given -> read all up to the end beginning at "from" position */
				if (isNumber(crFrom)) {
					contentRangeFrom = atol(crFrom);
					contentRangeTo = -1;
					if (contentRangeFrom < 0) {
						getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
						return;
					}
				} else {
					/* invalid header */
					getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
					return;
				}
			} else {
				/* both values given, we have a real range */
				if (isNumber(crTo)) {
					contentRangeTo = atol(crTo);
				} else {
					/* invalid header */
					getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
					return;
				}
				if (isNumber(crFrom)) {
					contentRangeFrom = atol(crFrom);
				} else {
					/* invalid header */
					getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
					return;
				}
				if (contentRangeFrom > contentRangeTo ||
					contentRangeFrom < 0 ||
					contentRangeTo < 0) {
					getBuildReply(wa,ERROR_416,request,reply,NULL,NULL,host,port,requestURI,0,0,isDocInXDB);
					return;
				}
			}
		} else {
			contentRangeFrom = -1;
			contentRangeTo = -1;
		}
		/* End Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */


		/* first get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			wdvhHandle = getWdvhHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_GET,"");
				return;
			}
			if (wdvhHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDVH Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_GET,"");
				return;
			}

			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* call CAPI function*/
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
			statuscode = getCallCapiFunc(wa,request, reply,pCapiHandle,requestURI,pIf,contentRangeFrom,contentRangeTo, &isDocInXDB);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CapiGet     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
			getBuildReply(wa,statuscode,request, reply,pCapiHandle,wdvhHandle,host,port,requestURI,contentRangeFrom,contentRangeTo, isDocInXDB);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
			
			/* Clean Up */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* finally return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_GET_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_GET,"");
		}
	}

	/*------------------------------------------------------------------------
		Function: webdavHeadHandler
	--------------------------------------------------------------------------*/
	void webdavHeadHandler( sapdbwa_Handle		wa,
				   	       sapdbwa_HttpRequestP request,
						   sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Int2				statuscode;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavHeadHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavHeadHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_HEAD,"");
			return;
		}

		/* get request uri */
		getRequestUri(wa,request,requestURI);

		/* interpret Header's */
/* ===> TODO!!!!!*/
		getIfHeader(request,sIf,100);
		pIf = sIf;
		
		/* first get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_HEAD,"");
				return;
			}
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* call CAPI function*/
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			statuscode = headCallCapiFunc(wa,request,pCapiHandle,requestURI);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CapiGet     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			headBuildReply(statuscode, reply, pCapiHandle);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
			
			/* Clean Up */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* finally return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_HEAD_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_HEAD,"");
		}
	}

	/*------------------------------------------------------------------------
		Function: webdavOptionsHandler
	--------------------------------------------------------------------------*/
	void webdavOptionsHandler( sapdbwa_Handle		wa,
				   		       sapdbwa_HttpRequestP request,
							   sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Char				*host;
		WDVH_Char				*port;
		sapdbwa_Int2	_statuscode;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		
		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavOptionsHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavOptionsHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_OPTIONS,"");
			return;
		}

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);

		/* interpret Header's */
/* ===> TODO!!!!! */
		getIfHeader(request,sIf,100);
		pIf = sIf;
		
		/* call CAPI function*/
		_statuscode = 200;/*optionsCallCapiFunc(requestURI, wa);*/

		/* build Reply */
		optionsBuildReply(_statuscode, reply, requestURI, host, port);
		
	}

	/*------------------------------------------------------------------------
		Function: webdavPropfindHandler
	--------------------------------------------------------------------------*/
	void webdavPropfindHandler( sapdbwa_Handle		wa,
					  	        sapdbwa_HttpRequestP request,
						        sapdbwa_HttpReplyP   reply )
	{
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVCAPI_PropfindHandle  propfind=NULL;
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Int2				statuscode = ERROR_200;
		WDVCAPI_Depth			depth;
		WDVH_Char				*pDepth=NULL;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		WDVH_Char				sUserAgent[1001];
		WDVH_Char				*pUserAgent;
		WDVH_Bool				isEmptyPropertyList;	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
		WDVH_Char				errormsg[1001];			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
		WDVH_Bool				isPropnameRequest;		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130891 */
		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavPropfindHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavPropfindHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
			return;
		}

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);

		/* interpret Header's */
		pDepth = (char*)sapdbwa_GetHeader(request,"Depth");
		if (pDepth) {
			if (strcmp(pDepth,WDVH_DEPTH_0)==0)
				depth = WDVCAPI_DEPTH_0;
			else if (strcmp(pDepth,WDVH_DEPTH_1)==0)
				depth = WDVCAPI_DEPTH_1;
			else if (strcmp(pDepth,WDVH_DEPTH_INFINITY)==0)
				depth = WDVCAPI_DEPTH_INFINITE;
			else {
				sendErrorReply(ERROR_400, reply, WEBDAV_PROPFIND,"");
			}
		} else
			depth = WDVCAPI_DEPTH_INFINITE;

		/* - If */
		getIfHeader(request,sIf,100);
		pIf = sIf;

		/* - User-Agent */
		pUserAgent = (char*)sapdbwa_GetHeader(request,"User-Agent");
		if (pUserAgent==NULL) {
			strcpy(sUserAgent,"");
		} else {
			strncpy(sUserAgent,pUserAgent,1000);
			sUserAgent[1000]='\0';
		}

		/* Build Handles */
		/* - get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
				return;
			}
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* interpret Body */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			statuscode = propfindParseRequestBody(wa,pCapiHandle,&propfind,request,depth,&isPropnameRequest,&isEmptyPropertyList,errormsg); /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Parsing     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* if (statuscode == ERROR_200) { 			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
			if (statuscode == ERROR_200 &&
				!isEmptyPropertyList)	{			    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1123332 */
				/* call CAPI function*/
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif
				statuscode = propfindCallCapiFunc(pCapiHandle,wa,request,propfind);
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  CapiPropfind: %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif
			}

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			propfindBuildReply(wa,statuscode,reply,host,port,requestURI,pCapiHandle,propfind,isPropnameRequest,isEmptyPropertyList,sUserAgent,errormsg); /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124183 */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		

			/* Clean Up */
			/* - handles */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			if (propfind) {
				WDVCAPI_PropfindDestroy(pCapiHandle,propfind);
			}
			/* - return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_PROPFIND_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_PROPFIND,"");
		}
	}

	/*------------------------------------------------------------------------
		Function: webdavCopyHandler
	--------------------------------------------------------------------------*/
	WDVH_Bool webdavCopyHandler(sapdbwa_Handle		wa,
						  	    sapdbwa_HttpRequestP request,
							    sapdbwa_HttpReplyP   reply )
	{
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVCAPI_URI				destination;
		WDVH_Char				destinationURL[WDVH_MAX_URI_LEN+1];
		WDVCAPI_URI				destinationURI;
		WDVH_Bool				externURL;
		sapdbwa_Bool			allocatOK;
		sapdbwa_Int4			length;
		WDVH_Char				*server;
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Int2				statuscode = ERROR_200;
		WDVCAPI_Depth			depth;
		WDVCAPI_Overwrite		overwrite;
		WDVH_Char				*pDepth=NULL;
		WDVH_Char				*pOverwrite;
		WDVH_CapiUserData		capiUserData;
		WDVCAPI_LockIdList		lockTokenList;
		WDVH_Char				prefix[WDV_MAX_URI_LEN+1];


		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		if (wa==NULL)
			return WDVH_False;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavCopyHandler:Uninitialized Pointers\n");
			return WDVH_False;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavCopyHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_COPY,"");
			return WDVH_False;
		}

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);
		/* get server string for error messages */
		buildServerString(request,&server);

		/* interpret Header's */
		/* - Depth */
		pDepth = (char*)sapdbwa_GetHeader(request,WDVH_DEPTH_HEADER);
		if (pDepth) {
			if (strcmp(pDepth,WDVH_DEPTH_0)==0)
				depth = WDVCAPI_DEPTH_0;
			else if (strcmp(pDepth,WDVH_DEPTH_INFINITY)==0)
				depth = WDVCAPI_DEPTH_INFINITE;
			else {
				sendErrorReply(ERROR_400, reply, WEBDAV_COPY,"");
				return WDVH_False;
			}
		} else
			depth = WDVCAPI_DEPTH_INFINITE;
		
		/* - Overwrite */
		pOverwrite = (char*)sapdbwa_GetHeader(request,WDVH_OVERWRITE_HEADER);
		if (pOverwrite) {
			if (strcmp(pOverwrite,WDVH_OVERWRITE_TRUE)==0) {
				overwrite = WDVCAPI_OVERWRITE;
			} else if (strcmp(pOverwrite,WDVH_OVERWRITE_FALSE)==0) {
				overwrite = WDVCAPI_NO_OVERWRITE;
			} else {
				sendErrorReply(ERROR_400, reply, WEBDAV_COPY,"");
				return WDVH_False;
			}
		} else {
			overwrite = WDVCAPI_OVERWRITE;
		}

		/* set server prefix */
		getWebDAVPrefix(wa,prefix);
		/* - Destination */
		externURL = WDVH_False;
		destination = (char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER);
		if (destination==NULL) {
			sendErrorReply(ERROR_400, reply, WEBDAV_COPY,"");
			return WDVH_False;
		} 
		wd21_UnescapeUrl(destination,strlen(destination));
		strncpy(destinationURL,destination,WDVH_MAX_URI_LEN);
		destinationURL[WDVH_MAX_URI_LEN]='\0';
		destinationURI = destination;
		if (strncmp(server,destinationURI,strlen(server))==0) {
			destinationURI += strlen(server);
			destinationURI += strlen(prefix);
		} else {
			/* error: no URL or bad gateway or ... */
			externURL = WDVH_True;
			statuscode = ERROR_501;
		}

		/* Build Handles */
		/* - get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_COPY,"");
				return WDVH_False;
			}
			/* - CapiUserData for call back */
			createCapiUserData(&capiUserData);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif


			/* -If header (lock tokens) */
			if (!WDVCAPI_LockCreateIdList(pCapiHandle,&lockTokenList)) {
				/* error handling */
			}

			getIfHeaderList(wa,request,pCapiHandle,&lockTokenList);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1120166 */

			if (statuscode != ERROR_500) {
				capiUserData->reply = reply;
				capiUserData->server = server;
				/* call CAPI function*/
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif
				if (externURL) {
					/* statuscode = copyCallCapiFunc(wa,request,pCapiHandle,requestURI,destinationURI,depth,overwrite);*/
				} else {
					statuscode = copyCallCapiFunc(wa,request,pCapiHandle,requestURI,destinationURI,depth,overwrite,capiUserData,lockTokenList);
				}
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  CapiCopy    : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif
			}

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			copyBuildReply(statuscode,reply,requestURI,pCapiHandle,capiUserData);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		

			/* Clean Up */
			/* - handles */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			if (server) {
				WDVHandler_Free((sapdbwa_UInt1*) server);
			}
			destroyCapiUserData(capiUserData);

			WDVCAPI_LockDropIdList(pCapiHandle,&lockTokenList );
			
			/* - return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
			return WDVH_True;
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_COPY_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_COPY,"");
			return WDVH_False;
		}
	}

	/*------------------------------------------------------------------------
		Function: webdavMoveHandler
	--------------------------------------------------------------------------*/
	WDVH_Bool webdavMoveHandler(sapdbwa_Handle		wa,
						  	    sapdbwa_HttpRequestP request,
							    sapdbwa_HttpReplyP   reply )
	{
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVCAPI_URI				destination;
		WDVH_Char				destinationURL[WDVH_MAX_URI_LEN+1];
		WDVCAPI_URI				destinationURI;
		WDVH_Bool				externURL;
		sapdbwa_Bool			allocatOK;
		sapdbwa_Int4			length;
		WDVH_Char				*server;
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Int2				statuscode = ERROR_200;
		WDVCAPI_Depth			depth;
		WDVCAPI_Overwrite		overwrite;
		WDVH_Char				*pDepth=NULL;
		WDVH_Char				*pOverwrite;
		WDVH_CapiUserData		capiUserData;
		WDVCAPI_LockIdList		lockTokenList;
		WDVH_Char				prefix[WDV_MAX_URI_LEN+1];

		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		if (wa==NULL)
			return WDVH_False;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavMoveHandler:Uninitialized Pointers\n");
			return WDVH_False;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavMoveHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_MOVE,"");
			return WDVH_False;
		}

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);
		/* get server string for error messages */
		buildServerString(request,&server);

		/* interpret Header's */
		/* - Depth */
		pDepth = (char*)sapdbwa_GetHeader(request,WDVH_DEPTH_HEADER);
		if (pDepth) {
			if (strcmp(pDepth,WDVH_DEPTH_INFINITY)==0)
				depth = WDVCAPI_DEPTH_INFINITE;
			else {
				sendErrorReply(ERROR_400, reply, WEBDAV_MOVE,"");
				return WDVH_False;
			}
		} else
			depth = WDVCAPI_DEPTH_INFINITE;
		
		/* - Overwrite */
		pOverwrite = (char*)sapdbwa_GetHeader(request,WDVH_OVERWRITE_HEADER);
		if (pOverwrite) {
			if (strcmp(pOverwrite,WDVH_OVERWRITE_TRUE)==0) {
				overwrite = WDVCAPI_OVERWRITE;
			} else if (strcmp(pOverwrite,WDVH_OVERWRITE_FALSE)==0) {
				overwrite = WDVCAPI_NO_OVERWRITE;
			} else {
				sendErrorReply(ERROR_400, reply, WEBDAV_MOVE,"");
				return WDVH_False;
			}
		} else {
			overwrite = WDVCAPI_OVERWRITE;
		}

		/* set server prefix */
		getWebDAVPrefix(wa,prefix);
		/* - Destination */
		externURL = WDVH_False;
		destination = (char*)sapdbwa_GetHeader(request,WDVH_DESTINATION_HEADER);
		if (destination==NULL) {
			sendErrorReply(ERROR_400, reply, WEBDAV_MOVE,"");
			return WDVH_False;
		} 
		wd21_UnescapeUrl(destination,strlen(destination));
		strncpy(destinationURL,destination,WDVH_MAX_URI_LEN);
		destinationURL[WDVH_MAX_URI_LEN]='\0';
		destinationURI = destination;
		if (strncmp(server,destinationURI,strlen(server))==0) {
			destinationURI += strlen(server);
			destinationURI += strlen(prefix);
		} else {
			/* error: no URL or bad gateway or ... */
			externURL = WDVH_True;
			statuscode = ERROR_501;
		}

		/* Build Handles */
		/* - get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_MOVE,"");
				return WDVH_False;
			}
			/* - CapiUserData for call back */
			createCapiUserData(&capiUserData);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* -If header (lock tokens) */
			if (!WDVCAPI_LockCreateIdList(pCapiHandle,&lockTokenList)) {
				/* error handling */
				return WDVH_False;
			}

			getIfHeaderList(wa,request,pCapiHandle,&lockTokenList);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1120166 */

			if (statuscode != ERROR_500) {
				capiUserData->reply = reply;
				capiUserData->server = server;
				/* call CAPI function*/
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif
				if (externURL) {
					/* statuscode = moveCallCapiFunc(wa,request,pCapiHandle,requestURI,destinationURI,depth,overwrite);*/
				} else {
					statuscode = moveCallCapiFunc(wa,request,pCapiHandle,requestURI,destinationURI,depth,overwrite,capiUserData,lockTokenList);
				}
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  CapiMove    : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif
			}

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			moveBuildReply(statuscode,reply,requestURI,pCapiHandle,capiUserData);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		

			/* Clean Up */
			/* - handles */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			if (server) {
				WDVHandler_Free((sapdbwa_UInt1*) server);
			}
			destroyCapiUserData(capiUserData);

			WDVCAPI_LockDropIdList(pCapiHandle,&lockTokenList );
			
			/* - return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
			return WDVH_True;
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_MOVE_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_MOVE,"");
			return WDVH_False;
		}
	}


	/*------------------------------------------------------------------------
		Function: webdavPostHandler
	--------------------------------------------------------------------------*/
	void webdavPostHandler( sapdbwa_Handle		wa,
				  	        sapdbwa_HttpRequestP request,
					        sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Int2				statuscode = ERROR_200;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Handle				wdvhHandle;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		WDVH_Char				sDc[100];
		WDVH_Char				*pDc;
		WDVH_Bool				async;
		WDVCAPI_XmlDocClassId	docClassId;
		WDVH_Bool				compress;
		WDVCAPI_ContentType		contentType;				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
		WDVCAPI_ContentTypeString	putContentType;			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
		WDVH_Char				errormsg[1001];				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
		WDVH_Bool				putToXmldb;

		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavPostHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavPostHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_PUT,"");
			return;
		}

		/* get request uri */
		getRequestUri(wa,request,requestURI);

		/* interpret Header's */
/* ===> TODO!!!!! */
		getIfHeader(request,sIf,100);
		pIf = sIf;

		/* build handles */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		/* first get a connection from session pool */
		connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			wdvhHandle = getWdvhHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_PUT,"");
				return;
			}
			if (wdvhHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDVH Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_PUT,"");
				return;
			}
			compress = getCompressFlag(wa,connection);
			strcpy(errormsg,"");
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* get and check DocClassId */
			statuscode = postCheckDocumentClass(wa,request,wdvhHandle,&docClassId,&async);

			/* get the MaxDB_XmlDB header */
			if ((char*)sapdbwa_GetHeader(request,"MaxDB_XmlDB") != NULL) {
				if (strcmp((char*)sapdbwa_GetHeader(request,"MaxDB_XmlDB"),"F")==0) {
					putToXmldb = WDVH_False;
				} else if (strcmp((char*)sapdbwa_GetHeader(request,"MaxDB_XmlDB"),"T")==0) {
					putToXmldb = WDVH_True;
				} else {
					/* bad request -> wrong value of the header */
					sp77sprintf(errormsg,1000,"MaxDB_XmlDB header must have value 'F' or 'T' standing for false and true. Found '%s'.",(char*)sapdbwa_GetHeader(request,"MaxDB_XmlDB"));
					sendErrorReply(ERROR_400, reply, WEBDAV_PUT,errormsg);
					return;
				}
			} else {
				/* not given means "F" */
				putToXmldb = WDVH_False;
			}


			/* get content type of the body */
			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
			if (statuscode = ERROR_200) {
				contentType = (WDVCAPI_ContentType)sapdbwa_GetHeader(request, "Content-Type" );
				if (!contentType) 
					/* no content type given -> set default */
					strcpy(putContentType,"text/plain");
				else {
					/* check ContentType */
					strncpy(putContentType,contentType,WDVCAPI_MAX_CONTENT_TYPE_LEN);
					putContentType[WDVCAPI_MAX_CONTENT_TYPE_LEN]='\0';
					if (!WDVH_CheckContentType(putContentType)) {
						/* something is wrong with it */
						statuscode = ERROR_400;
						sp77sprintf(errormsg,1000,"Content-Type does not satisfy the syntax defined in RFC2068. ('%s')",putContentType);
					}
				}
			}

			/* call CAPI function*/
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			if (statuscode == ERROR_200) 
				if (!WDVCAPI_IdIsInitialValue(docClassId)) 
					statuscode = postCallCapiFunc(wa,request,pCapiHandle,wdvhHandle,requestURI,docClassId,async,pIf,compress,putToXmldb,putContentType,errormsg);
				else
					statuscode = postCallCapiFunc(wa,request,pCapiHandle,wdvhHandle,requestURI,NULL,WDVH_False,pIf,compress,putToXmldb,putContentType,errormsg);

			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CapiPut/Post: %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			postBuildReply(statuscode, reply, errormsg);			/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		

			/* Clean Up */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* finally return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_PUT_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_PUT,"");
		}
	}

	/*------------------------------------------------------------------------
		Function: webdavProppatchHandler
	--------------------------------------------------------------------------*/
	void webdavProppatchHandler( sapdbwa_Handle		wa,
							     sapdbwa_HttpRequestP request,
								 sapdbwa_HttpReplyP   reply )
	{
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVCAPI_ProppatchHandle proppatch = NULL;
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
		WDVH_Char				*server;
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Int2				statuscode = ERROR_200;
		WDVH_Bool				iserror;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavProppatchHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavProppatchHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
			return;
		}

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);
		/* get server string for error messages */
		buildServerString(request,&server);

		/* interpret Header's */
		getIfHeader(request,sIf,100);
		pIf = sIf;

		/* Build Handles */
		/* - get a connection from session pool */
		#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
			time1 = clock();
		#endif
		connection = getConnection(wa);
		if (connection == NULL) 
			connection = getConnection(wa);
		if (connection != NULL) {
			/* get wdv handle */
			pCapiHandle = getWdvHandle(wa,connection);
			/* check the handles */
			if (pCapiHandle==NULL) {
				sapdbwa_WriteLogMsg(wa, "Got no valid WDV Handle");
				sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
				return;
			}
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif
			/* - proppatch handle */
			if(!WDVCAPI_ProppatchCreate(pCapiHandle,requestURI,pIf,&proppatch )) {
				WDVCAPI_GetLastError(pCapiHandle,&errorItem);
				WDVCAPI_GetErrorType(errorItem,&errorType);
				switch (errorType) {
					case WDVCAPI_ERR_TYPE_UNDEFINED:
					case WDVCAPI_ERR_TYPE_SQL: 
						{
							/* internal server error */
							WDVCAPI_GetErrorText(errorItem,&errorMsg);
							statuscode = ERROR_500;
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
								case WDVCAPI_ERR_CODE_RESOURCE_OR_COLLECTION_DOESNT_EXIST:
									{
										statuscode = ERROR_404;
										WDVCAPI_GetErrorText(errorItem,&errorMsg);
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_NOTFOUND_404,server,(char*)sapdbwa_GetRequestURI(request));
										sapdbwa_WriteLogMsg(wa, errorText);
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_MESSAGE,errorMsg);
										sapdbwa_WriteLogMsg(wa, errorText);
										break;
									}
								case WDVCAPI_ERR_CODE_LOCK_EXISTS:
									{
										statuscode = ERROR_423;
										WDVCAPI_GetErrorText(errorItem,&errorMsg);
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_LOCKED_423,server,(char*)sapdbwa_GetRequestURI(request));
										sapdbwa_WriteLogMsg(wa, errorText);
										sp77sprintf(errorText,1000,ERROR_PROPPATCH_MESSAGE,errorMsg);
										sapdbwa_WriteLogMsg(wa, errorText);
										break;
									}
								default:
									{
										statuscode = ERROR_500;
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
							statuscode = ERROR_500;
							sp77sprintf(errorText,1000,ERROR_PROPPATCH_URI,(char*)sapdbwa_GetHeader(request,"HTTP_HOST"),(char*)sapdbwa_GetRequestURI(request));
							sapdbwa_WriteLogMsg(wa, errorText);
							sp77sprintf(errorText,1000,ERROR_PROPPATCH_500,errorMsg);
							sapdbwa_WriteLogMsg(wa, errorText);
							break;
						}
				}
			} else {

				/* interpret Body */
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif
				statuscode = proppatchParseRequestBody(wa,pCapiHandle,proppatch,request,&iserror);
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  Parsing     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif
			}

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			proppatchBuildReply(statuscode,wa,request,reply,host,port,requestURI,pCapiHandle,proppatch,iserror);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		

			/* Clean Up */
			/* - handles */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			if (proppatch) 
				WDVCAPI_ProppatchDestroy(pCapiHandle,proppatch);
			/* - return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_PROPPATCH_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_PROPPATCH,"");
		}
	}

	/*------------------------------------------------------------------------
		Function: webdavXqueryHandler
		ATTENTION: This is NO WebDAV method!!! 
		its an extension for xmldb. if there is more of it one time it should 
		be transfered to some file WDVHandler_XqueryHandler.cpp
		ATTENTION: This is just a QUICK'N'DIRTY approach!!! If this should be
		a lasting one, some more work has to be done!
	--------------------------------------------------------------------------*/
	void webdavXqueryHandler(sapdbwa_Handle		wa,
				  	        sapdbwa_HttpRequestP request,
					        sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Int2				statuscode = ERROR_200;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Handle				wdvhHandle;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		WDVH_Char				sDc[100];
		WDVH_Char				*pDc;
		WDVH_Bool				async;
		WDVCAPI_XmlDocClassId	docClassId;
		WDVH_Bool				compress;
		WDVH_Char				errormsg[1001];				
		WDVH_Char				*querystring;
		WDVH_ULong				contentLength;
		sapdbwa_Bool			allocatOK;
	    WDVH_Int2				done;
		WDVH_ULong				pos;
		WDVH_Bool				success=WDVH_False;
		WDVH_ULong				bytesRead=0;
		WDVCAPI_ErrorItem		errorItem;
		WDVCAPI_ErrorType		errorType;
		WDVCAPI_ErrorCode		errorCode;
		WDVCAPI_ErrorText		errorMsg;
		WDVH_Error				error;
		WDVH_Char				errorText[1000];
		WDVH_Char				*server;

		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif
		
		/* get request uri */
		getRequestUri(wa,request,requestURI);

		/* get server string for error messages */
		buildServerString(request,&server);

		/* interpret Header's */
        if (sapdbwa_GetContentLength(request)) {
			contentLength = atoi(sapdbwa_GetContentLength(request));
			WDVHandler_Allocat(sizeof(SAPDB_Char)*(contentLength+1),(sapdbwa_UInt1**) &querystring,&allocatOK);
			if (allocatOK) {
				/* read the body of the XQUERY request */
				do {
					if (contentLength > 0) {
						pos = sapdbwa_ReadBody(request,querystring+bytesRead,wdvh_max_put_buffer_len );
						bytesRead += pos;
						if (bytesRead >= contentLength && contentLength > 0)
							done = 1;
						else if (pos == 0) {
							done = 1;
							strcpy(querystring,"");
						} else
							done = 0;
					}
				} while(!done);
			}
			/* build handles */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* first get a connection from session pool */
			connection = getConnection(wa);
			if (connection != NULL) {
				/* get wdv handle */
				pCapiHandle = getWdvHandle(wa,connection);
				wdvhHandle = getWdvhHandle(wa,connection);
				strcpy(errormsg,"");
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif
				/* call CAPI function*/
		/*		if (WDVCAPI_XQueryOpen(wdv,uri,querystring,WDVCAPI_CHAR_ENCODING_ASCII))
					success = WDVH_True;
				else 
					success = WDVH_False;
*/				if (success==WDV_False) {
					WDVCAPI_GetLastError(pCapiHandle,&errorItem);
					WDVCAPI_GetErrorType(errorItem,&errorType);
					switch (errorType) {
						case WDVCAPI_ERR_TYPE_UNDEFINED:
						case WDVCAPI_ERR_TYPE_SQL: 
							{
								/* internal server error */
								WDVCAPI_GetErrorText(errorItem,&errorMsg);
								statuscode = ERROR_500;
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
											statuscode = ERROR_500;
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
								/* internal server error */
								WDVCAPI_GetErrorText(errorItem,&errorMsg);
								statuscode = ERROR_500;
								sp77sprintf(errorText,1000,ERROR_XQUERY_URI,server,(char*)sapdbwa_GetRequestURI(request));
								sapdbwa_WriteLogMsg(wa, errorText);
								sp77sprintf(errorText,1000,ERROR_XQUERY_500,errorMsg);
								sapdbwa_WriteLogMsg(wa, errorText);
								break;
							}
					}
				}
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif

				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  CapiPut/Post: %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif

				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif
				/* send query result */
				if (success == WDVH_True) {
					sapdbwa_InitHeader(reply,
									ERROR_200,
									"text/xml",
									NULL,
									NULL,								/* const char    *LastModified,*/
									NULL,								/* const char    *Expires,*/
									NULL);								/* const char    *Location );*/
					sapdbwa_SendHeader(reply);
					/* get XQuery result and send it to the requester */
/*					buffer = wdvhHandle->wdvGetBuffer;
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
*/				} else {
					sapdbwa_InitHeader(reply,
									statuscode,
									"text/html",
									NULL,
									NULL,								/* const char    *LastModified,*/
									NULL,								/* const char    *Expires,*/
									NULL);								/* const char    *Location );*/
					sapdbwa_SendHeader(reply);
					switch (statuscode) {
						case ERROR_400:
							sapdbwa_SendBody(reply,
											ERROR_PAGE_400,
											0);
							sapdbwa_SendBody(reply,
											"",
											0);
							sapdbwa_SendBody(reply,
											ERROR_PAGE_FOOTER,
											0);
							break; 
						case ERROR_409: 
							sapdbwa_SendBody(reply,
											ERROR_PAGE_409,
											strlen(ERROR_PAGE_409));
							break; 
						case ERROR_412: 
							sapdbwa_SendBody(reply,
											ERROR_PAGE_412,
											strlen(ERROR_PAGE_412));
							break; 
						case ERROR_423: 
							sapdbwa_SendBody(reply,
											ERROR_PAGE_423,
											strlen(ERROR_PAGE_423));
							break; 
						case ERROR_500:
							sapdbwa_SendBody(reply,
										ERROR_PAGE_500,
										strlen(ERROR_PAGE_500));
							break;
					}
				}
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif		

				/* Clean Up */
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif
				/* finally return connection to session pool */
				closeConnection(wa,connection);
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif		
			} else {
				sapdbwa_WriteLogMsg(wa, ERROR_XQUERY_DB_CONN);
				/* error handling */
				sendErrorReply(ERROR_500, reply, WEBDAV_XQUERY,"");
			}
        } else {
			contentLength = 0;
        }

		if (server) {
			WDVHandler_Free((sapdbwa_UInt1*) server);
		}
	}


