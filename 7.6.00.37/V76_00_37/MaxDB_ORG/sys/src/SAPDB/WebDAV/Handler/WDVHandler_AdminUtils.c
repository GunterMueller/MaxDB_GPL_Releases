/****************************************************************************

  module      : WDVHandler_AdminUtils.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-28  16:58
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_AdminUtils.h"
	#include "SAPDB/WebAgent/HTTP/WAHTTP_IniPar.h"
/*	#define RESOURCE_FILE
	#include "gsp100.h"
	#include "hsp100.h"
*/
	/*------------------------------------------------------------------------
		Function: sendAdminPingReply
	--------------------------------------------------------------------------*/
	void sendAdminPingReply(sapdbwa_HttpReplyP   reply)
	{
		char *buf;
		sapdbwa_Bool allocatOK;

		sapdbwa_InitHeader(reply,
                           ERROR_200,
                           "text/html",
                           NULL,
	                       NULL,								/* const char    *LastModified,*/
                           NULL,								/* const char    *Expires,*/
                           NULL);								/* const char    *Location );*/

		sapdbwa_SetHeader( reply,
                           "Server",
                           "SAPDBWWW WebDAV Server" );

		sapdbwa_SendHeader(reply);

		sapdbwa_SendBody(reply,
						 WDVH_WEBDAV_SERVER_NAME,
						 strlen(WDVH_WEBDAV_SERVER_NAME));
	}

	/*------------------------------------------------------------------------
		Function: sendAdminStateReply
	--------------------------------------------------------------------------*/
	void sendAdminStateReply(sapdbwa_Handle			wa,
							 sapdbwa_HttpRequestP	request, 
							 sapdbwa_HttpReplyP		reply, 
							 WDVH_AdminState		adminState)
	{
		WDVH_Char	reqcount[100];
		WDVH_Char	reqpersec[100];
		WDVH_Char	reqpersecbusy[100];
		WDVH_Char	idlepercentage[100];
		WDVH_Char	busypercentage[100];
		WDVH_Char	recentidlepercentage[100];
		WDVH_Char	recentbusypercentage[100];
		WDVH_Char	methodlog[100];
		WDVH_UInt4	methodpercentage[WDVH_ADMIN_METHODS+1];
		WDVH_Double	idlepercent;
		WDVH_Double	busypercent;
		WDVH_Double	recentidlepercent;
		WDVH_Double	recentbusypercent;
	    time_t		ltime;
		WDVH_Double	overall,busy,now;
		sapdbwa_DbConnectionP	connection = NULL;
		WDVCAPI_WDV				wdv;
		WDVH_CapiUserData		capiUserData;
		WDVCAPI_BigLengthString  sumLogicalCompressedLength;
        WDVCAPI_BigLengthString  sumPhysicalCompressedLength;
        WDVCAPI_BigLengthString  sumRatio;
		WDVCAPI_BigLengthString  sumContentLength;
		WDVCAPI_BigLengthString  sumCompressedContentLength;
		WDVCAPI_BigLengthString  sumOfResources;
		WDVCAPI_BigLengthString  sumOfCollections;
		WDVCAPI_BigLengthString  sumPhysicalLength;

		sapdbwa_InitHeader(reply,
                           ERROR_200,
                           "text/xml",
                           NULL,
	                       NULL,								/* const char    *LastModified,*/
                           NULL,								/* const char    *Expires,*/
                           NULL);								/* const char    *Location );*/

		sapdbwa_SetHeader( reply,
                           "Server",
                           "SAPDBWWW WebDAV Server" );

		sapdbwa_SendHeader(reply);

		/* build xml document out of state information */
		if (adminState) {
			if (strcmp(getAdminStateLocation(adminState),"")==0) 
				setAdminStateLocation(adminState,
									  (char*)sapdbwa_GetHeader(request,"HTTP_HOST"),
									  (char*)sapdbwa_GetHeader(request,"HTTP_PORT"));

			/* XML Header */
			sapdbwa_SendBody(reply,
							 WDVH_XML_HEADER,
							 strlen(WDVH_XML_HEADER));
			/* Response */
			sapdbwa_SendBody(reply,
							 WDVH_RESPONSE_OPEN,
							 strlen(WDVH_RESPONSE_OPEN));
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATE_NAMESPACE,
							 strlen(WDVH_XML_ADMIN_STATE_NAMESPACE));
			sapdbwa_SendBody(reply,
							 WDVH_XML_CLOSE_TAG,
							 strlen(WDVH_XML_CLOSE_TAG));
			/*     State */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATE_OPEN,
							 strlen(WDVH_XML_ADMIN_STATE_OPEN));
			/*         server */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_OPEN,
							 strlen(WDVH_XML_ADMIN_SERVER_OPEN));
			/*		       webdavserver */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_WEBDAVSERVER_OPEN,
							 strlen(WDVH_XML_ADMIN_WEBDAVSERVER_OPEN));
			/*                 location */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_LOCATION_OPEN,
							 strlen(WDVH_XML_ADMIN_SERVER_LOCATION_OPEN));
			/*                     value */
			sapdbwa_SendBody(reply,
							 getAdminStateLocation(adminState),
							 strlen(getAdminStateLocation(adminState)));
			/*                 /location */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_LOCATION_CLOSE,
							 strlen(WDVH_XML_ADMIN_SERVER_LOCATION_CLOSE));
			/*                 name */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_NAME_OPEN,
							 strlen(WDVH_XML_ADMIN_SERVER_NAME_OPEN));
			/*                     value */
			sapdbwa_SendBody(reply,
							 getAdminStateName(adminState),
							 strlen(getAdminStateName(adminState)));
			/*                 /name */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_NAME_CLOSE,
							 strlen(WDVH_XML_ADMIN_SERVER_NAME_CLOSE));
			/*                 version */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_VERSION_OPEN,
							 strlen(WDVH_XML_ADMIN_SERVER_VERSION_OPEN));
			/*                     value */
			sapdbwa_SendBody(reply,
							 getAdminStateVersion(adminState),
							 strlen(getAdminStateVersion(adminState)));
			/*                 /version */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_VERSION_CLOSE,
							 strlen(WDVH_XML_ADMIN_SERVER_VERSION_CLOSE));
			/*                 build */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_BUILD_OPEN,
							 strlen(WDVH_XML_ADMIN_SERVER_BUILD_OPEN));
			/*                     value */
			sapdbwa_SendBody(reply,
							 getAdminStateBuild(adminState),
							 strlen(getAdminStateBuild(adminState)));
			/*                 /build */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_BUILD_CLOSE,
							 strlen(WDVH_XML_ADMIN_SERVER_BUILD_CLOSE));
			/*             /webdavserver */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_WEBDAVSERVER_CLOSE,
							 strlen(WDVH_XML_ADMIN_WEBDAVSERVER_CLOSE));
			/*         /server */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_SERVER_CLOSE,
							 strlen(WDVH_XML_ADMIN_SERVER_CLOSE));
			/*         statistics */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_OPEN));
			/*             running */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_RUNNING_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_RUNNING_OPEN));
			/*                 value */
			sapdbwa_SendBody(reply,
							 getAdminStateRunningSince(adminState),
							 strlen(getAdminStateRunningSince(adminState)));
			/*             /running */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_RUNNING_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_RUNNING_CLOSE));
			/*             request count */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_REQCOUNT_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_REQCOUNT_OPEN));
			/*                 value */
			sp77sprintf(reqcount,100,"%ld",getAdminStateRequestCount(adminState));
			sapdbwa_SendBody(reply,
							 reqcount,
							 strlen(reqcount));
			/*             /request count */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_REQCOUNT_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_REQCOUNT_CLOSE));
			/*             request per sec */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_REQPERSEC_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_REQPERSEC_OPEN));
			/*                 value */
			time(&ltime);
			if ((ltime - adminState->runningSinceUTC) != 0)
				sp77sprintf(reqpersec,100,"%.4f",((double)adminState->requestCount/(double)(ltime - adminState->runningSinceUTC)));
			else
				sp77sprintf(reqpersec,100,"%.4f",0);
			sapdbwa_SendBody(reply,
							 reqpersec,
							 strlen(reqpersec));
			/*             /request per sec */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_REQPERSEC_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_REQPERSEC_CLOSE));
			/*             request per sec busy*/
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_REQPERSECBUSY_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_REQPERSECBUSY_OPEN));
			/*                 value */
			if (((ltime - adminState->runningSinceUTC) - adminState->idleTimeUTC) != 0)
				sp77sprintf(reqpersec,100,"%.4f",((double)adminState->requestCount/(double)((ltime - adminState->runningSinceUTC) - adminState->idleTimeUTC)));
			else
				sp77sprintf(reqpersec,100,"%.4f",0);
			sapdbwa_SendBody(reply,
							 reqpersec,
							 strlen(reqpersec));
			/*             /request per sec busy*/
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_REQPERSECBUSY_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_REQPERSECBUSY_CLOSE));
			/*             idle percentage */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_IDLE_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_IDLE_OPEN));
			/*                 value */
			if ((ltime - adminState->runningSinceUTC)!=0)
				idlepercent = ((double)adminState->idleTimeUTC/(double)(ltime - adminState->runningSinceUTC))*100;
			else
				idlepercent = 100;
			if (idlepercent > 100)
				idlepercent = 100;
			sp77sprintf(idlepercentage,100,"%.4f",idlepercent);
			sapdbwa_SendBody(reply,
							 idlepercentage,
							 strlen(idlepercentage));
			/*             /idle percentage */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_IDLE_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_IDLE_CLOSE));
			/*             busy percentage */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_BUSY_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_BUSY_OPEN));
			/*                 value */
			busypercent = 100 - idlepercent;
			sp77sprintf(busypercentage,100,"%.4f",busypercent);
			sapdbwa_SendBody(reply,
							 busypercentage,
							 strlen(busypercentage));
			/*             /busy percentage */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_BUSY_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_BUSY_CLOSE));
			/*             recent idle percentage
			busy = getAdminStateRecentBusy(adminState);
			overall = getAdminStateRecentOverall(adminState);
			now = (double)clock();
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_RECENT_IDLE_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_RECENT_IDLE_OPEN));
*/			/*                 value 
			if ((ltime - adminState->runningSinceUTC)!=0)
				recentidlepercent = ((overall - busy)/ overall) * 100;
			else
				recentidlepercent = 100;
			if (recentidlepercent > 100)
				recentidlepercent = 100;
			sp77sprintf(recentidlepercentage,100,"%.4f",recentidlepercent);
			sapdbwa_SendBody(reply,
							 recentidlepercentage,
							 strlen(recentidlepercentage));
*/			/*             /recentidle percentage 
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_RECENT_IDLE_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_RECENT_IDLE_CLOSE));
*/			/*             recent busy percentage 
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_RECENT_BUSY_OPEN,
							 strlen(WDVH_XML_ADMIN_STATISTICS_RECENT_BUSY_OPEN));
*/			/*                 value 
			recentbusypercent = 100 - recentidlepercent;
			sp77sprintf(recentbusypercentage,100,"%.4f",recentbusypercent);
			sapdbwa_SendBody(reply,
							 recentbusypercentage,
							 strlen(recentbusypercentage));
*/			/*             /recent busy percentage 
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_RECENT_BUSY_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_RECENT_BUSY_CLOSE));
*/			connection = getConnection(wa);
			if (connection != NULL) {
				wdv = getWdvHandle(wa,connection);
				createCapiUserData(&capiUserData);
				WDVCAPI_AdminTotalCompressedRatio(wdv,sumLogicalCompressedLength,
												  sumPhysicalCompressedLength,
												  sumRatio );
				WDVCAPI_AdminTotalSumContentLength(wdv,sumContentLength);
				WDVCAPI_AdminTotalSumCompressedContentLength(wdv,sumCompressedContentLength);
				WDVCAPI_AdminTotalSumPhysicalLength(wdv,sumPhysicalLength);
				WDVCAPI_AdminTotalSumOfResources(wdv,sumOfResources);
				WDVCAPI_AdminTotalSumOfCollections(wdv,sumOfCollections);
				/*             content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_CONTENTLEN_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_CONTENTLEN_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumContentLength,
								 strlen(sumContentLength));
				/*             /content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_CONTENTLEN_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_CONTENTLEN_CLOSE));
				/*             compressed content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_COMPCONTENTLEN_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_COMPCONTENTLEN_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumCompressedContentLength,
								 strlen(sumCompressedContentLength));
				/*             /compressed content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_COMPCONTENTLEN_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_COMPCONTENTLEN_CLOSE));
				/*             physical content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_PHYCONTENTLEN_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_PHYCONTENTLEN_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumPhysicalLength,
								 strlen(sumPhysicalLength));
				/*             /physical content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_PHYCONTENTLEN_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_PHYCONTENTLEN_CLOSE));
				/*             logical compressed content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_LOGCOMPCONTENTLEN_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_LOGCOMPCONTENTLEN_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumLogicalCompressedLength,
								 strlen(sumLogicalCompressedLength));
				/*             /logical compressed content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_LOGCOMPCONTENTLEN_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_LOGCOMPCONTENTLEN_CLOSE));
				/*             physical compressed content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_PHYCOMPCONTENTLEN_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_PHYCOMPCONTENTLEN_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumPhysicalCompressedLength,
								 strlen(sumPhysicalCompressedLength));
				/*             /physical content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_PHYCOMPCONTENTLEN_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_PHYCOMPCONTENTLEN_CLOSE));
				/*             compression ratio */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_COMPRESSIONGRADE_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_COMPRESSIONGRADE_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumRatio,
								 strlen(sumRatio));
				/*             /physical content length */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_COMPRESSIONGRADE_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_COMPRESSIONGRADE_CLOSE));
				/*             resources */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_RESOURCES_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_RESOURCES_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumOfResources,
								 strlen(sumOfResources));
				/*             /resources */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_RESOURCES_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_RESOURCES_CLOSE));
				/*             collections */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_COLLECTIONS_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_COLLECTIONS_OPEN));
				/*                 value */
				sapdbwa_SendBody(reply,
								 sumOfCollections,
								 strlen(sumOfCollections));
				/*             /resources */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_COLLECTIONS_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_COLLECTIONS_CLOSE));

				/* - return connection to session pool */
				destroyCapiUserData(capiUserData);
				closeConnection(wa,connection);
			}
			if (getAdminStateMethodLog(adminState,methodpercentage)) {
				/*             method log */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_METHODLOG_OPEN,
								 strlen(WDVH_XML_ADMIN_STATISTICS_METHODLOG_OPEN));
				/*					total logged */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_METHODLOG_TOTAL_LOGGED_OPEN,
								 strlen(WDVH_XML_ADMIN_METHODLOG_TOTAL_LOGGED_OPEN));
				/*						value */
				sp77sprintf(methodlog,100,"%ld",methodpercentage[WDVH_ADMIN_METHOD_COUNT]);
				sapdbwa_SendBody(reply,
								 methodlog,
								 strlen(methodlog));
				/*					/total logged */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_METHODLOG_TOTAL_LOGGED_CLOSE,
								 strlen(WDVH_XML_ADMIN_METHODLOG_TOTAL_LOGGED_CLOSE));
				if (methodpercentage[WDVH_ADMIN_METHOD_COUNT]!=0) {
					if (methodpercentage[WDVH_ADMIN_METHOD_GET]!=0) {
						/*			method get */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_GET_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_GET_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_GET])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method get */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_GET_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_GET_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_HEAD]!=0) {
						/*			method head */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_HEAD_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_HEAD_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_HEAD])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method head */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_HEAD_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_HEAD_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_PUT]!=0) {
						/*			method put */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PUT_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PUT_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_PUT])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method put */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PUT_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PUT_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_POST]!=0) {
						/*			method post */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_POST_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_POST_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_POST])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method post */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_POST_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_POST_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_DELETE]!=0) {
						/*			method delete */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_DELETE_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_DELETE_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_DELETE])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method delete */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_DELETE_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_DELETE_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_COPY]!=0) {
						/*			method copy */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_COPY_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_COPY_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_COPY])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method copy */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_COPY_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_COPY_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_MOVE]!=0) {
						/*			method move */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_MOVE_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_MOVE_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_MOVE])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method move */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_MOVE_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_MOVE_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_PROPFIND]!=0) {
						/*			method propfind */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PROPFIND_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PROPFIND_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_PROPFIND])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method propfind */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PROPFIND_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PROPFIND_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_PROPPATCH]!=0) {
						/*			method proppatch */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PROPPATCH_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PROPPATCH_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_PROPPATCH])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method proppatch */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PROPPATCH_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PROPPATCH_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_MKCOL]!=0) {
						/*			method mkcol */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_MKCOL_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_MKCOL_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_MKCOL])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method mkcol */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_MKCOL_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_MKCOL_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_LOCK]!=0) {
						/*			method lock */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_LOCK_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_LOCK_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_LOCK])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method lock */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_LOCK_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_LOCK_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_UNLOCK]!=0) {
						/*			method unlock */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_UNLOCK_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_UNLOCK_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_UNLOCK])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method unlock */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_UNLOCK_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_UNLOCK_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_OPTIONS]!=0) {
						/*			method options */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_OPTIONS_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_OPTIONS_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_OPTIONS])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method options */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_OPTIONS_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_OPTIONS_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_TRACE]!=0) {
						/*			method trace */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_TRACE_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_TRACE_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_TRACE])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method trace */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_TRACE_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_TRACE_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_CONNECT]!=0) {
						/*			method connect */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_CONNECT_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_CONNECT_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_CONNECT])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method connect */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_CONNECT_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_CONNECT_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_PING]!=0) {
						/*			method ping */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PING_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PING_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_PING])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method ping */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_PING_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_PING_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_STATE]!=0) {
						/*			method state */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_STATE_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_STATE_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_STATE])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method state */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_STATE_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_STATE_CLOSE));
					}
					if (methodpercentage[WDVH_ADMIN_METHOD_RESET]!=0) {
						/*			method reset */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_RESET_OPEN,
										 strlen(WDVH_XML_ADMIN_METHODLOG_RESET_OPEN));
						/*				value */
						sp77sprintf(methodlog,100,"%.2f",((double)(methodpercentage[WDVH_ADMIN_METHOD_RESET])/(double)(methodpercentage[WDVH_ADMIN_METHOD_COUNT]))*100);
						sapdbwa_SendBody(reply,
										 methodlog,
										 strlen(methodlog));
						/*			/method reset */
						sapdbwa_SendBody(reply,
										 WDVH_XML_ADMIN_METHODLOG_RESET_CLOSE,
										 strlen(WDVH_XML_ADMIN_METHODLOG_RESET_CLOSE));
					}
				}
				/*             /method log */
				sapdbwa_SendBody(reply,
								 WDVH_XML_ADMIN_STATISTICS_METHODLOG_CLOSE,
								 strlen(WDVH_XML_ADMIN_STATISTICS_METHODLOG_CLOSE));
			}
			/*         /statistics */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATISTICS_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATISTICS_CLOSE));
			/*     /State */
			sapdbwa_SendBody(reply,
							 WDVH_XML_ADMIN_STATE_CLOSE,
							 strlen(WDVH_XML_ADMIN_STATE_CLOSE));
			/* /response */
			sapdbwa_SendBody(reply,
							 WDVH_RESPONSE_CLOSE,
							 strlen(WDVH_RESPONSE_CLOSE));

		}

	}

	/*------------------------------------------------------------------------
		Function: sendAdminResetReply
	--------------------------------------------------------------------------*/
	void sendAdminResetReply(sapdbwa_HttpReplyP   reply, 
							 WDVH_AdminState adminState)
	{
		char *buf;
		sapdbwa_Bool allocatOK;

		resetAdminState(adminState);

		sapdbwa_InitHeader(reply,
                           ERROR_200,
                           "text/html",
                           NULL,
	                       NULL,								/* const char    *LastModified,*/
                           NULL,								/* const char    *Expires,*/
                           NULL);								/* const char    *Location );*/

		sapdbwa_SetHeader( reply,
                           "Server",
                           "SAPDBWWW WebDAV Server" );

		sapdbwa_SendHeader(reply);

		sapdbwa_SendBody(reply,
						 "SAP DB WWW WebDAV Server: Server statistics reseted",
						 strlen("SAP DB WWW WebDAV Server: Server statistics reseted"));
	}

	/*------------------------------------------------------------------------
		Function: createAdminState
	--------------------------------------------------------------------------*/
    WDVH_Bool createAdminState(sapdbwa_Handle wa, WDVH_AdminState *adminState) 
	{
		WDVH_Bool		ok = WDVH_False;
		WDVH_AdminState	newAdminState = NULL;
		WDVH_UInt2		i;
	    time_t ltime;
	    tsp100_VersionID2   VersionID2;
    
		if ( !adminState ) {
			return WDVH_False;
		}
    
		WDVHandler_Allocat( sizeof(struct st_admin_state),
					(WDVH_UInt1**) &newAdminState,
					&ok );
		if (ok == WDVH_True) {
			strcpy(newAdminState->name,"SAP DB WebDAV Service");
			strcpy(newAdminState->location,"");
			#ifdef WIN32
			sp100_GetVersionID (VersionIDType2_esp100,s100buildnumberEx,&VersionID2);
		    sp77sprintf(newAdminState->version,100,"%d.%d.%d",
						(int) VersionID2.MajorVersion_sp100,
						(int) VersionID2.MinorVersion_sp100,
						(int) VersionID2.CorrLevel_sp100);
			sp77sprintf(newAdminState->build,100,"%d", (int) VersionID2.BuildNumber_sp100);
			#endif 
			sp77sprintf(newAdminState->version,50,"%s.%s.%s",
						MAJOR_VERSION_NO_STR_SP100,
						MINOR_VERSION_NO_STR_SP100,
						CORRECTION_LEVEL_STR_SP100);
			strcpy(newAdminState->build,s100buildnumber());
			/* Get UNIX-style time and store as string. */
			time(&ltime);
			newAdminState->runningSinceUTC = ltime;
			sp77sprintf(newAdminState->runningSince,100,"%s",ctime(&ltime));
			newAdminState->requestCount = 0;
			newAdminState->idleTimeUTC = 1;
			newAdminState->lastIdleStartUTC = 0;

			for (i=0;i<WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE;i++) {
				newAdminState->lastCalledMethods[i].method = 0;
				newAdminState->lastCalledMethods[i].start = 0;
				newAdminState->lastCalledMethods[i].end = 0;
			}
			newAdminState->currentMethodLogPos = 0;

			*adminState = newAdminState;

			return WDVH_True;
		} else {
			*adminState = NULL;

			return WDVH_False;
		}

	}			   

	/*------------------------------------------------------------------------
		Function: destroyAdminState
	--------------------------------------------------------------------------*/
    WDVH_Bool destroyAdminState(WDVH_AdminState adminState) 
	{
		if ( !adminState ) {
			return WDVH_False;
		}
    
		WDVHandler_Free((WDVH_UInt1*)adminState);
		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: resetAdminState
	--------------------------------------------------------------------------*/
    WDVH_Bool resetAdminState(WDVH_AdminState adminState) 
	{
		if ( !adminState ) {
			return WDVH_False;
		}
    
		adminState->requestCount = 0;
		adminState->idleTimeUTC = 0;
		adminState->lastIdleStartUTC = 0;
		memset(adminState->lastCalledMethods,0,WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE);
		adminState->currentMethodLogPos = 0;

		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: setAdminStateName
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateName(WDVH_AdminState adminState, WDVH_Char *name) 
	{
		if ( !adminState ) {
			return WDVH_False;
		}
		strcpy(adminState->name,name);
		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: setAdminStateLocation
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateLocation(WDVH_AdminState adminState, WDVH_Char *location, WDVH_Char *port) 
	{
		if ( !adminState ) {
			return WDVH_False;
		}
		sp77sprintf(adminState->location,249,"%s:%s",location,port);
		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: setAdminStateRunnigSince
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateRunningSince(WDVH_AdminState adminState, WDVH_Char *running) 
	{
		if ( !adminState ) {
			return WDVH_False;
		}
		strcpy(adminState->runningSince,running);
		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: setAdminStateIdleStart
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateIdleStart(WDVH_AdminState adminState) 
	{
	    time_t ltime;

		if ( !adminState ) {
			return WDVH_False;
		}
		time(&ltime);
		adminState->lastIdleStartUTC = ltime;

		adminState->lastCalledMethods[adminState->currentMethodLogPos].end = clock();

		if (adminState->currentMethodLogPos == WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE-1) {
			adminState->currentMethodLogPos = 0;
		} else {
			adminState->currentMethodLogPos++;
		}

		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: setAdminStateIdleStop
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateIdleStop(WDVH_AdminState adminState) 
	{
	    time_t ltime;

		if ( !adminState ) {
			return WDVH_False;
		}
		time(&ltime);
		if (adminState->lastIdleStartUTC == 0)
			adminState->idleTimeUTC += ltime - adminState->runningSinceUTC;
		else
			adminState->idleTimeUTC += ltime - adminState->lastIdleStartUTC;

		adminState->lastCalledMethods[adminState->currentMethodLogPos].start = clock();

		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: adminStateLogMethod
	--------------------------------------------------------------------------*/
    WDVH_Bool adminStateLogMethod(WDVH_AdminState adminState, WDVH_UInt1 method) 
	{
		if ( !adminState ) {
			return WDVH_False;
		}

		adminState->lastCalledMethods[adminState->currentMethodLogPos].method = method;

		return WDVH_True;
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateName
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateName(WDVH_AdminState adminState) 
	{
		if ( adminState ) 
			return adminState->name;
		else
			return "";
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateLocation
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateLocation(WDVH_AdminState adminState) 
	{
		if ( adminState ) 
			return adminState->location;
		else
			return "";
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateRunnigSince
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateRunningSince(WDVH_AdminState adminState) 
	{
		if ( adminState ) 
			return adminState->runningSince;
		else
			return "";
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateRequestCount
	--------------------------------------------------------------------------*/
    WDVH_UInt4 getAdminStateRequestCount(WDVH_AdminState adminState) 
	{
		if ( adminState ) {
			return adminState->requestCount;
		} else 
			return 0;
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateVersion
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateVersion(WDVH_AdminState adminState) 
	{
		if ( adminState ) 
			return adminState->version;
		else
			return "";
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateBuild
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateBuild(WDVH_AdminState adminState) 
	{
		if ( adminState ) 
			return adminState->build;
		else
			return "";
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateMethodLog
	--------------------------------------------------------------------------*/
    WDVH_Bool getAdminStateMethodLog(WDVH_AdminState adminState,WDVH_UInt4 *methodpercent) 
	{
		WDVH_UInt4	i;

		if ( !adminState ) {
			return WDVH_False;
		} else {
			/* initalize result array */
			for(i=0;i<=WDVH_ADMIN_METHODS;i++) 
				methodpercent[i] = 0;
			for(i=0;i<WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE;i++) {
				if (adminState->lastCalledMethods[i].method!=0) {
					methodpercent[WDVH_ADMIN_METHOD_COUNT]++;
					methodpercent[adminState->lastCalledMethods[i].method]++;
				} 
			}
			return WDVH_True;
		}
	}			    

	/*------------------------------------------------------------------------
		Function: getAdminStateRecentOverall
	--------------------------------------------------------------------------*/
    WDVH_Double getAdminStateRecentOverall(WDVH_AdminState adminState) 
	{
		WDVH_Double	start,end;
		WDVH_UInt4	i;

		if ( !adminState ) {
			return WDVH_False;
		} else {

			start = (double)clock();
			for (i=0;i<WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE;i++) {
				if (start > (double)(adminState->lastCalledMethods[i].start))
					start = (double)(adminState->lastCalledMethods[i].start);
			}

			end = (double)clock();

			return end-start;
		}
	}			   

	/*------------------------------------------------------------------------
		Function: getAdminStateRecentBusy
	--------------------------------------------------------------------------*/
    WDVH_Double getAdminStateRecentBusy(WDVH_AdminState adminState) 
	{
		WDVH_Double	busy,start,end;
		WDVH_UInt4	i;


		if ( !adminState ) {
			return WDVH_False;
		} else {

			busy = 0;
			for (i=0;i<WDVH_ADMIN_MAX_METHOD_CALL_LOG_SIZE;i++) {
				start = (double)(adminState->lastCalledMethods[i].start);
				end = (double)(adminState->lastCalledMethods[i].end);
				if (start != 0 && end != 0) 
					if (start <= end)
						busy = busy + end - start;
			}
			
			return busy;
		}
	}			   

