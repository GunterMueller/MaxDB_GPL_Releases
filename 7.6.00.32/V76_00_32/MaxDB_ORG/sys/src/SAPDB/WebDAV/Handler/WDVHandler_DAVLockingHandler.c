/****************************************************************************

  module      : WDVHandler_DAVLockingHandler.c

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

	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVLockingHandler.h"


	/*------------------------------------------------------------------------
		Function: webdavLockHandler
	--------------------------------------------------------------------------*/
	void webdavLockHandler( sapdbwa_Handle		wa,
						     sapdbwa_HttpRequestP request,
						     sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Int2				statuscode;
		SQLHDBC					odbchandle;
		SQLHENV					henvhandle;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVCAPI_Depth			depth;
		WDVH_Char				*pDepth=NULL;
		WDVH_Char				sTimeout[50];
		WDVH_Char				*pTimeout;
		WDVH_Char				sIf[50];
		WDVH_Char				*pIf;
		WDVH_Char				sLocktoken[50];
		WDVH_Char				*pLocktoken;
		WDVH_CapiUserData		capiUserData;
		WDVH_PChar				server;
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Char				errormsg[1001];				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */

		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif

		
		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavLockHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavLockHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_LOCK,"");
			return;
		}

		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);
		/* get server string for error messages */
		buildServerString(request,&server);


		/* interpret Header's */
		/* - Depth */
		pDepth = (char*)sapdbwa_GetHeader(request,"Depth");
		if (pDepth) {
			if (strcmp(pDepth,WDVH_DEPTH_0)==0)
				depth = WDVCAPI_DEPTH_0;
			else if (strcmp(pDepth,WDVH_DEPTH_INFINITY)==0)
				depth = WDVCAPI_DEPTH_INFINITE;
			else {
				sendErrorReply(ERROR_400, reply, WEBDAV_LOCK,"");
			}
		} else
			depth = WDVCAPI_DEPTH_INFINITE;

		/* - Timeout */
		pTimeout = sTimeout;
		pTimeout = (char*)sapdbwa_GetHeader(request,"Timeout");

		/* - If */
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
				sendErrorReply(ERROR_500, reply, WEBDAV_LOCK,"");
				return;
			}
			strcpy(errormsg,"");							/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* - CapiUserData for call back */
			createCapiUserData(&capiUserData);
			capiUserData->reply = reply;
			capiUserData->server = server;
			capiUserData->prefix = sapdbwa_GetServiceName(wa);
			/* TODO: calculation of timeout in seconds depending on Timeout Header*/
			capiUserData->timeout = 600;
			capiUserData->depth = depth;
			if (pIf)
				strcpy(capiUserData->lockId,pIf);

			/* interpret Body */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			statuscode = lockParseRequestBody(wa,pCapiHandle,request,capiUserData,errormsg);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124043 */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  Parsing     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			/* call CAPI function*/
			if (statuscode == ERROR_200) {
				statuscode = lockCallCapiFunc(wa,
											  pCapiHandle,
											  request,
											  requestURI,
											  capiUserData,
											  errormsg);				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
			}
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CapiLock    : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif

			/* build Reply */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			lockBuildReply(statuscode, reply, capiUserData, requestURI,errormsg);		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124007 */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  BuildReply  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
			
			/* Clean Up */
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time1 = clock();
			#endif
			destroyCapiUserData(capiUserData);
			/* finally return connection to session pool */
			closeConnection(wa,connection);
			#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
				time2 = clock();
				duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
				printf("  CleanUp     : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
			#endif		
		} else {
			sapdbwa_WriteLogMsg(wa, ERROR_PROPPATCH_DB_CONN);
			/* error handling */
			sendErrorReply(ERROR_500, reply, WEBDAV_LOCK,"");
		}

	}

	/*------------------------------------------------------------------------
		Function: webdavUnlockHandler
	--------------------------------------------------------------------------*/
	void webdavUnlockHandler( sapdbwa_Handle		wa,
						     sapdbwa_HttpRequestP request,
						     sapdbwa_HttpReplyP   reply )
	{
		WDVH_Char				requestURI[WDVH_MAX_URI_LEN+1];
		WDVH_Int2				statuscode;
		SQLHDBC					odbchandle;
		SQLHENV					henvhandle;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				pCapiHandle;
		WDVH_Char				*server;
		WDVH_Char				*host;
		WDVH_Char				*port;
		WDVH_Char				sIf[100];
		WDVH_Char				*pIf;
		WDVH_Char				sLockToken[100];
		WDVH_Char				*pLockToken;
		WDVH_Char				errormsg[1001];				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */

		#if defined(PROFILING_OVERALL) || defined(PROFILING_DETAIL)
			clock_t    time1;
			clock_t	   time2;
			double     duration;
		#endif

		if (wa==NULL)
			return;
		if (reply==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavUnlockHandler:Uninitialized Pointers\n");
			return;
		}
		if (request==NULL) {
			sapdbwa_WriteLogMsg(wa, "webdavUnlockHandler:Uninitialized Pointers\n");
			sendErrorReply(ERROR_500, reply, WEBDAV_UNLOCK,"");
			return;
		}
		
		/* extract Parameters (Host, RequestURI)*/
		getFirstHeaderLine(wa,request,&host,&port,requestURI);
		/* get server string for error messages */
		buildServerString(request,&server);

		/* interpret Header's */
		/* - If */
		getIfHeader(request,sIf,100);
		pIf = sIf;

		/* - Lock-Token */
		strcpy(errormsg,"");
		if (getLockTokenHeader(request,sLockToken,errormsg)) {		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			pLockToken = sLockToken;
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
					sendErrorReply(ERROR_500, reply, WEBDAV_UNLOCK,"");
					return;
				}
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  Connecting  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif

				/* call CAPI function*/
				statuscode = unlockCallCapiFunc(wa,pCapiHandle,request,requestURI,sLockToken);
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time2 = clock();
					duration = ((time2-time1)*1000) / CLOCKS_PER_SEC;
					printf("  CapiUnlock  : %f msec (Time1 = %d, Time2 = %d, Diff = %d)\n",duration,time1,time2,time2-time1);
				#endif

				/* build Reply */
				#if defined(PROFILING_OVERALL) && defined(PROFILING_DETAIL)
					time1 = clock();
				#endif
				unlockBuildReply(statuscode, reply);
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
				sapdbwa_WriteLogMsg(wa, ERROR_PROPPATCH_DB_CONN);
				/* error handling */
				sendErrorReply(ERROR_500, reply, WEBDAV_UNLOCK,errormsg);
			}
		} else {																		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			/* error handling */														/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
			sendErrorReply(ERROR_400, reply, WEBDAV_UNLOCK,errormsg);					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
		}																				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
	}

