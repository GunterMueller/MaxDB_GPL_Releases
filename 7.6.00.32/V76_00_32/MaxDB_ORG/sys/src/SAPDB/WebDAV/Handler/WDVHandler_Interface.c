/****************************************************************************

  module      : WDVHandler_Interface.c

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



/* =====================================================================
   |  Includes                                                         |
   =====================================================================*/
	#include "SAPDB/WebDAV/CAPI/WDVCAPI_Definition.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Interface.h"

/* =====================================================================
   |  Globals                                                          |
   =====================================================================*/
	WDVH_AdminState	adminState;
/* =====================================================================
   |  Functions                                                        |
   =====================================================================*/


/* ---------------------------------------------------------------------
   WebAgent UserInit Function
   ---------------------------------------------------------------------*/
	sapdbwa_Int2  WDVHandlerInit( sapdbwa_Handle wa )
	{
		WDVH_Char		message[WDVH_MAX_URI_LEN+50];
		WDVH_Char		tempPrefix[WDVH_MAX_URI_LEN+1];

		if (wa==NULL)
			return -1;

		sapdbwa_WriteLogMsg(wa,"WDVHandlerInit\n");

		adminState = NULL;
		createAdminState(wa,&adminState);

		/* set server prefix */
		strcpy(tempPrefix,sapdbwa_GetServiceName(wa));
		sp77sprintf(message,WDVH_MAX_URI_LEN+1,"Use URL prefix /%s \n",tempPrefix);
		sapdbwa_WriteLogMsg(wa,message);

		/* set document root */
/* strcpy(WDVH_WEBDAV_DOCUMENT_ROOT,"");
		if (!sapdbwa_GetDocumentRoot(wa,WDVH_WEBDAV_DOCUMENT_ROOT,1024))
			strcpy(WDVH_WEBDAV_DOCUMENT_ROOT,"");
*/
		/* set compress flag from registry */
/*		if (sapdbwa_GetParam(wa,"compress",message,10))
			if (strncmp(message,"1",1)==0)
				WDVH_WEBDAV_COMPRESS = WDVH_True;
			else
				WDVH_WEBDAV_COMPRESS = WDVH_False;
		else
			WDVH_WEBDAV_COMPRESS = WDVH_False;
*/

        /* Handle Manager */
        WDVCAPI_InitHandleManager();

        return 1;
	}


/* ---------------------------------------------------------------------
   WebAgent UserExit Function
   ---------------------------------------------------------------------*/
	sapdbwa_Int2  WDVHandlerExit( sapdbwa_Handle wa )
	{

		if (wa==NULL)
			return -1;

		sapdbwa_WriteLogMsg(wa,"WDVHandlerExit\n");

		if (adminState)
			destroyAdminState(adminState);

		return 1;
	}


/* ---------------------------------------------------------------------
   WebAgent UserService Function
   ---------------------------------------------------------------------*/
	sapdbwa_Int2  WDVHandlerService( sapdbwa_Handle    wa,
								     sapdbwa_HttpRequestP request,
									 sapdbwa_HttpReplyP   reply )
	{

		const char				*_pMethod;
		int						Return;
		const WDVH_Char			*querystring;
		WDVH_Char				server[512];
		WDVH_PChar				pserver;
		sapdbwa_DbConnectionP	connection = NULL;
		#ifdef PROFILING_OVERALL
			clock_t    time_1;
			clock_t	   time_2;
			double     duration;
		#endif
		
		if (wa==NULL || request==NULL || reply==NULL)
			return sapdbwa_RTC_Error;
			
		/* create a user data record and attach it to the WebAgent handle */
		
		setAdminStateIdleStop(adminState);
		pserver = server;

		/* check wether there are administrative requests */
		/* - ping */
		querystring = sapdbwa_GetQueryString(request);
		if (querystring) {

			/* - ping */
			if (strcmp(WDVH_ADMIN_PING,querystring)==0) {
				/* send ping reply */
				if (adminState)
					adminState->requestCount++;
				sendAdminPingReply(reply);
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_PING);
				Return = sapdbwa_RTC_Accepted;
			}
			/* - state */
			if (strcmp(WDVH_ADMIN_STATE,querystring)==0) {
				if (adminState)
					adminState->requestCount++;
				/* send ping reply */
				sendAdminStateReply(wa,request, reply, adminState);
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_STATE);
				Return = sapdbwa_RTC_Accepted;
			}
			/* - reset */
			if (strcmp(WDVH_ADMIN_RESET,querystring)==0) {
				/* reset statistics and send 200 OK reply */
				sendAdminResetReply(reply, adminState);
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_RESET);
				Return = sapdbwa_RTC_Accepted;
			}
			/* - call embedded gui navigation page */
			if (strncmp(WDVH_HTML_EMBEDDED_GUI_NAVI,querystring,strlen(WDVH_HTML_EMBEDDED_GUI_NAVI))==0) {
				/* send the embedded GUI start page */
				buildServerString(request,&pserver);
				sendGuiNaviPage(wa,reply,querystring,pserver,sapdbwa_GetServiceName(wa));
				Return = sapdbwa_RTC_Accepted;
			}
			/* - call embedded gui start page */
			if (strcmp(WDVH_HTML_EMBEDDED_GUI,querystring)==0) {
				/* send the embedded GUI start page */
				buildServerString(request,&pserver);
				sendGuiStartPage(wa,reply,pserver,sapdbwa_GetServiceName(wa));
				Return = sapdbwa_RTC_Accepted;
			}
			/* - call embedded webdav explorer */
			if (strncmp(WDVH_HTML_EMBEDDED_EXPLORER,querystring,strlen(WDVH_HTML_EMBEDDED_EXPLORER))==0) {
				/* call the service function for the embedded XML Modelling HTML GUI */
				connection = getConnection(wa);
				if (connection != NULL) {
					doServiceExplorer(wa,request,reply,getWdvHandle(wa,connection),getWdvhHandle(wa,connection),getXmlHandle(wa,connection),getCompressFlag(wa,connection));
					Return = sapdbwa_RTC_Accepted;
					closeConnection(wa,connection);
				} else {
					sapdbwa_WriteLogMsg(wa, ERROR_PUT_DB_CONN);
					sendErrorReply(ERROR_500_CON,reply,"","");
				}
				Return = sapdbwa_RTC_Accepted;
			}
			/* - call embedded xmlmodelling html gui */
			if (strncmp(WDVH_HTML_EMBEDDED_MODELLING,querystring,strlen(WDVH_HTML_EMBEDDED_MODELLING))==0) {
				/* call the service function for the embedded XML Modelling HTML GUI */
				connection = getConnection(wa);
				if (connection != NULL) {
					XMLIDMLIB_doServiceEmbedded(wa,request,reply,getXmlHandle(wa,connection));
					Return = sapdbwa_RTC_Accepted;
					closeConnection(wa,connection);
				} else {
					sapdbwa_WriteLogMsg(wa, ERROR_PUT_DB_CONN);
					sendErrorReply(ERROR_500_CON,reply,"","");
				}
				Return = sapdbwa_RTC_Accepted;
			}
			/* - call embedded XML Index Engine admin html gui */
			if (strncmp(WDVH_HTML_EMBEDDED_XIEADMIN,querystring,strlen(WDVH_HTML_EMBEDDED_XIEADMIN))==0) {
				/* call the service function for the embedded XML Index Engine HTML GUI */
				connection = getConnection(wa);
				if (connection != NULL) {
					XMLIDMLIB_doServiceEmbedded(wa,request,reply,getXmlHandle(wa,connection));
					Return = sapdbwa_RTC_Accepted;
					closeConnection(wa,connection);
				} else {
					sapdbwa_WriteLogMsg(wa, ERROR_PUT_DB_CONN);
					sendErrorReply(ERROR_500_CON,reply,"","");
				}
				Return = sapdbwa_RTC_Accepted;
			}
			/* - call embedded XML Index Query html gui */
			if (strncmp(WDVH_HTML_EMBEDDED_QUERY,querystring,strlen(WDVH_HTML_EMBEDDED_QUERY))==0) {
				/* call the service function for the embedded XML Index Query HTML GUI */
				connection = getConnection(wa);
				if (connection != NULL) {
					XMLQCLIB_doQueryEmbedded(wa,request,reply,getQsHandle(wa,connection),getXmlHandle(wa,connection));
					Return = sapdbwa_RTC_Accepted;
					closeConnection(wa,connection);
				} else {
					sapdbwa_WriteLogMsg(wa, ERROR_PUT_DB_CONN);
					sendErrorReply(ERROR_500_CON,reply,"","");
				}
				Return = sapdbwa_RTC_Accepted;
			}
		} else {
			if (adminState)
				adminState->requestCount++;
			/* no admin request */
			/* the request has got to be handled depending on the WebDAV-method given in the header */
			_pMethod = sapdbwa_GetMethod(request);
			if (_pMethod==NULL)
				return sapdbwa_RTC_Error;

			#ifdef PROFILING_OVERALL
				printf("%s: \n",_pMethod);
				time_1 = clock();
			#endif

			if (strcmp(_pMethod,WEBDAV_GET)==0) {
				webdavGetHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_GET);
				Return = sapdbwa_RTC_Accepted;

			} else if (strcmp(_pMethod,WEBDAV_HEAD)==0) {
				webdavHeadHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_HEAD);
				Return = sapdbwa_RTC_Accepted;

			} else if (strcmp(_pMethod,WEBDAV_PUT)==0) {
				webdavPostHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_PUT);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_DELETE)==0) {
				webdavDeleteHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_DELETE);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_POST)==0) {
				webdavPostHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_POST);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_PROPFIND)==0) {
				webdavPropfindHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_PROPFIND);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_PROPPATCH)==0) {
				webdavProppatchHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_PROPPATCH);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_COPY)==0) {
				webdavCopyHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_COPY);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_MOVE)==0) {
				webdavMoveHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_MOVE);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_MKCOL)==0) {
				webdavMkcolHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_MKCOL);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_OPTIONS)==0) {
				webdavOptionsHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_OPTIONS);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_TRACE)==0) {
				sendErrorReply(ERROR_501, reply, WEBDAV_TRACE,"");
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_TRACE);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_CONNECT)==0) {
				sendErrorReply(ERROR_501, reply, WEBDAV_CONNECT,"");
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_CONNECT);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_LOCK)==0) {
				webdavLockHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_LOCK);
				Return = sapdbwa_RTC_Accepted;
			
			} else if (strcmp(_pMethod,WEBDAV_UNLOCK)==0) {
				webdavUnlockHandler(wa, request, reply );
				adminStateLogMethod(adminState,WDVH_ADMIN_METHOD_UNLOCK);
				Return = sapdbwa_RTC_Accepted;
			/* Versioning Core Package */
			#ifdef VERSIONING_CORE
			} else if (strcmp(_pMethod,WEBDAV_VERSION_CONTROL)==0) {
				sendErrorReply(ERROR_501, reply, WEBDAV_VERSION_CONTROL,"");
				Return = sapdbwa_RTC_Accepted;

			} else if (strcmp(_pMethod,WEBDAV_CHECKOUT)==0) {
				sendErrorReply(ERROR_501, reply, WEBDAV_CHECKOUT,"");
				Return = sapdbwa_RTC_Accepted;

			} else if (strcmp(_pMethod,WEBDAV_CHECKIN)==0) {
				sendErrorReply(ERROR_501, reply, WEBDAV_CHECKIN,"");
				Return = sapdbwa_RTC_Accepted;

			} else if (strcmp(_pMethod,WEBDAV_UNCHECKOUT)==0) {
				sendErrorReply(ERROR_501, reply, WEBDAV_UNCHECKOUT,"");
				Return = sapdbwa_RTC_Accepted;

			} else if (strcmp(_pMethod,WEBDAV_REPORT)==0) {
				sendErrorReply(ERROR_501, reply, WEBDAV_REPORT,"");
				Return = sapdbwa_RTC_Accepted;

			#endif
			/* XQuery Extension */
			#ifdef XQUERY_EXTENSION
			} else if (strcmp(_pMethod,WEBDAV_XQUERY)==0) {
				webdavXqueryHandler(wa, request, reply );
				Return = sapdbwa_RTC_Accepted;
			#endif
			} else {		
				sendErrorReply(ERROR_400, reply, WEBDAV_GET,"");
				Return = sapdbwa_RTC_Accepted;
			}
			#ifdef PROFILING_OVERALL
				time_2 = clock();
				duration = ((time_2-time_1)*1000) / CLOCKS_PER_SEC;
				printf("  overall     : %f msec (Time1 = %d, Time2 = %d, Diff = %d, CLOCKS_PER_SEC = %d\n",duration,time_1,time_2,time_2-time_1,CLOCKS_PER_SEC);
			#endif
		}

		setAdminStateIdleStart(adminState);
		return Return;
	}


/* --- EOF -------------------------------------------------------------*/
