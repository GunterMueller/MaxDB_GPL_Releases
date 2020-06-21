/*!**********************************************************************

  module: XMLIDMLib_SessionEmbedded.c

  -----------------------------------------------------------------------


  responsible:  Daniel Kirmse (D027470 - Daniel.Kirmse@SAP.com)


  special area: WebDAV / XML Indexing

  description:  Point of entry for WebDAV embedded XML Html Modelling GUI

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


\endif
************************************************************************/

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_SessionEmbedded.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Definition.h"
	#include "sapdbwa.hpp" //nocheck


	SAPDB_Bool XMLIDMLIB_doServiceEmbedded(sapdbwa_Handle    wa,
								  		   sapdbwa_HttpRequestP req,
								 		   sapdbwa_HttpReplyP   rep,
										   XMLIMAPI_Handle	xml)
	{ 
		XMLIDMLIB_Char		__message[1024];
		XMLIDMLIB_Bool		__keepdata = XMLIDMLIB_False;
		XMLIDMLIB_Bool		__refreshXML = XMLIDMLIB_False;
		XMLIDMLIB_Bool		__refreshDOC = XMLIDMLIB_False;
		XMLIDMLIB_Bool		__refreshSERV = XMLIDMLIB_False;
		XMLIDMLIB_Char		__idxServiceID[50];
		XMLIDMLIB_Char		__idxServiceName[130];
		XMLIDMLIB_Char		__idxServiceDesc[520];
		XMLIDMLIB_Char		__spName[130];
		XMLIDMLIB_Char		__store[10];
		XMLIDMLIB_Char		__docstore[130];
		XMLIDMLIB_Char		__idxstore[130];
		XMLIDMLIB_Char		__docstorenew[3];
		XMLIDMLIB_Char		__idxstorenew[3];
		XMLIDMLIB_Char		__docclassid[50];
		XMLIDMLIB_Char		__docclassname[129];
		XMLIDMLIB_Char		__docclassdesc[513];
		XMLIDMLIB_Char		__assignedXmlIndexes[5001];
		XMLIDMLIB_Char		__assignedXmlIndexesCount[5];
		XMLIDMLIB_Char      __newlyAdded[5001];
		XMLIDMLIB_Char		__adc[129];
		XMLIDMLIB_Char		__emptymsg[2];

		strcpy(__emptymsg,"");

		if (!XMLIDMLIB_IsQueryStringValid(req)) {
			// send the web page
			sendInitialFrame(wa,req,rep);
			return XMLIDMLIB_True;
		} else {
			if (XMLIDMLIB_IsCommandRequest(req)) {
				/*-------------------------------------------------------------
				/* Commands
				/*-------------------------------------------------------------*/
				/*-------------------------------------------------------------
				/* XML Index
				/*-------------------------------------------------------------*/
				if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_XMLINDEX_NEW)) {
					sendXmlIndexNew(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_XMLINDEX_CREATE)) {
					if (!createNewXmlIndex(wa,req,xml,__message,&__keepdata,&__refreshXML,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_False);
					else {
						// XML Index created -> depending on the value of docclassid a blank screen, DC New or DC Show has to be called
						if (strcmp(__docclassid,"TMP")==0) {
							// XML Index New/Update was called from DC New
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_OK,(char*)"XML Index created",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
						} else if (strcmp(__docclassid,"")!=0) {
							// XML Index New/Update was called from DC Show
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_OK,(char*)"XML Index created",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
						} else {
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						}
					}
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_XMLINDEX_SHOW)) {
					sendXmlIndexShow(wa,req,rep,xml);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_XMLINDEX_UPDATE)) {
					if (!updateXmlIndex(wa,req,xml,__message,&__keepdata,&__refreshXML,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_False);
					else {
						// XML Index update -> depending on the value of docclassid a blank screen, DC New or DC Show has to be called
						if (strcmp(__docclassid,"TMP")==0) {
							// XML Index New/Update was called from DC New
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_OK,(char*)"XML Index updated",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
						} else if (strcmp(__docclassid,"")!=0) {
							// XML Index New/Update was called from DC Show
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_OK,(char*)"XML Index updated",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
						} else {
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						}
					}
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_XMLINDEX_DELETE)) {
					if (!deleteXmlIndex(wa,req,xml,__message,&__keepdata,&__refreshXML)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else {
						// XML Index deleted -> depending on the value of docclassid a blank screen, DC New or DC Show has to be called
						if (strcmp(__docclassid,"TMP")==0) {
							// XML Index New/Update was called from DC New
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
						} else if (strcmp(__docclassid,"")!=0) {
							// XML Index New/Update was called from DC Show
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
						} else {
							sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						}
					}
					return XMLIDMLIB_True;
				}
				//------------------------------------------------------------------------
				// Document Class
				//------------------------------------------------------------------------
				else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_DOCCLASS_NEW)) {
					// send data page
					sendDocClassNew(wa,req,rep,xml);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_DOCCLASS_SHOW)) {
					// send data page
					sendDocClassShow(wa,req,rep,xml);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_DOCCLASS_CREATE)) {
					if (!createNewDocClass(wa,req,xml,__message,&__keepdata,&__refreshDOC,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Class created",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_DOCCLASS_UPDATE)) {
					if (!updateDocClass(wa,req,xml,__message,&__keepdata,&__refreshDOC,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Class updated",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_DOCCLASS_DELETE)) {
					if (!deleteDocClass(wa,req,xml,__message,&__keepdata,&__refreshDOC,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid)) 
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Class deleted",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				//------------------------------------------------------------------------
				// Session Pool
				//------------------------------------------------------------------------
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_SESSIONPOOL_NEW)) {
					sendSessionPoolNew(wa,req,rep);
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_SESSIONPOOL_SHOW)) {
					sendSessionPoolShow(wa,req,rep,xml);
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_SESSIONPOOL_CREATE)) {
					if (!createNewSessionPool(wa,req,xml,__message,&__keepdata,&__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else {
						// Session Pool created -> depending on the value of idxServiceID a blank screen, IS New or IS Show has to be called
						if (strcmp(__idxServiceID,"TMP")==0) {
							// Session Pool New was called from IS New
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Store created",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						} else if (strcmp(__idxServiceID,"")!=0) {
							// Session Pool New was called from IS Show
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Store created",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						} else {
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Store created",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						}
					}
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_SESSIONPOOL_UPDATE)) {
					if (!updateSessionPool(wa,req,xml,__message,&__keepdata,&__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						// Session Pool created -> depending on the value of idxServiceID a blank screen, IS New or IS Show has to be called
						if (strcmp(__idxServiceID,"TMP")==0) {
							// Session Pool New was called from IS New
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Store changed",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						} else if (strcmp(__idxServiceID,"")!=0) {
							// Session Pool New was called from IS Show
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Store changed",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						} else {
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_OK,(char*)"Document Store changed",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						}
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_SESSIONPOOL_DELETE)) {
					if (!deleteSessionPool(wa,req,xml,(char*)"",__message,&__keepdata,&__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc)) 
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						// Session Pool created -> depending on the value of idxServiceID a blank screen, IS New or IS Show has to be called
						if (strcmp(__idxServiceID,"TMP")==0) {
							// Session Pool New was called from IS New
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						} else if (strcmp(__idxServiceID,"")!=0) {
							// Session Pool New was called from IS Show
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						} else {
							sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
						}
				//------------------------------------------------------------------------
				// Indexing Service
				//------------------------------------------------------------------------
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_INDEXINGSERVICE_NEW)) {
					sendIndexingServiceNew(wa,req,rep,xml);
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_INDEXINGSERVICE_SHOW)) {
					sendIndexingServiceShow(wa,req,rep,xml);
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_INDEXINGSERVICE_CREATE)) {
					if (!createNewIndexingService(wa,req,xml,__message,&__keepdata,&__refreshSERV)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_OK,(char*)"Indexing Service created",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_INDEXINGSERVICE_UPDATE)) {
					if (!updateIndexingService(wa,req,xml,__message,&__keepdata,&__refreshSERV)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_OK,(char*)"Indexing Service changed",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_INDEXINGSERVICE_DELETE)) {
					if (!deleteIndexingService(wa,req,xml,__message,&__keepdata,&__refreshSERV)) 
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,__message,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					else
						sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_OK,(char*)"Indexing Service deleted",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
				/*-------------------------------------------------------------
				/* Message Frame
				/*-------------------------------------------------------------*/
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_CANCEL)) {
					sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_CANCEL_IS)) {
					cancelIndexingService(wa,req,xml,__message,&__keepdata,&__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc);
					sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_CANCEL_SP)) {
					cancelSessionPool(wa,req,__message,&__keepdata,&__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc);
					if (strcmp(__idxServiceID,"TMP")==0) {
						// Session Pool New was called from IS New
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_True);
					} else if (strcmp(__idxServiceID,"")!=0) {
						// Session Pool New was called from IS Show
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_True);
					} else {
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					}
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_CANCEL_XI)) {
					cancelXmlIndex(wa,req,__message,&__keepdata,&__refreshXML,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded);
					if (strcmp(__docclassid,"TMP")==0) {
						// XML Index New/Update was called from DC New
						sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
					} else if (strcmp(__docclassid,"")!=0) {
						// XML Index New/Update was called from DC Show
						sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
					} else {
						sendMessagePage(wa,req,rep,(char*)"2",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					}
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_CANCEL_DC)) {
					cancelDocClass(wa,req,xml,__message,&__keepdata,&__refreshXML,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded);
					if (strcmp(__idxServiceID,"TMP")==0) {
						// XML Index New/Update was called from DC New
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
					} else if (strcmp(__idxServiceID,"")!=0) {
						// XML Index New/Update was called from DC Show
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_CLEAR,__emptymsg,__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__idxServiceID,__idxServiceName,__idxServiceDesc,__spName,__store,__docstore,__idxstore,__docstorenew,__idxstorenew,__adc,__docclassid,__docclassname,__docclassdesc,__assignedXmlIndexes,__assignedXmlIndexesCount,__newlyAdded,XMLIDMLIB_True);
					} else {
						sendMessagePage(wa,req,rep,(char*)"1",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					}
					return XMLIDMLIB_True;
				}
				else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_MESSAGE_CLEAR)) {
					sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_CLEAR,(char*)" ",XMLIDMLIB_True,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				}
			} else if (XMLIDMLIB_IsCommandRequestXie(req)) {
				//------------------------------------------------------------------------
				// XML Index Engine
				//------------------------------------------------------------------------
				if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_XIE_SHOW)) {
					sendXieShow(wa,req,rep,xml);
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_XIE_OVERVIEW)) {
					sendXieShowAll(wa,req,rep,xml);
				/*-------------------------------------------------------------
				/* Message Frame
				/*-------------------------------------------------------------*/
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_CANCEL)) {
					sendMessagePage(wa,req,rep,(char*)"-1",XMLIDMLIB_MSGTY_ERROR,(char*)"Action canceled",XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,XMLIDMLIB_False,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsCommand(req,(char*)TEXT_CONSTANT_CMD_MESSAGE_CLEAR)) {
					sendMessagePage(wa,req,rep,(char*)"-1",XMLIDMLIB_MSGTY_CLEAR,(char*)" ",XMLIDMLIB_True,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				}
			} else {
				/*-------------------------------------------------------------
				/* HTML Pages called w/o user action
				/*-------------------------------------------------------------*/
				if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_INITIALFRAME_EMBEDDED)) {
					sendInitialFrame(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_INITIALFRAMEXIE_EMBEDDED)) {
					sendInitialFrameXie(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_HEADER_EMBEDDED)) {
					sendHeader(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_XIEHEADER_EMBEDDED)) {
					sendXieHeader(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_NAVIGATION_EMBEDDED)) {
					sendNavigation(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_XIENAVIGATION_EMBEDDED)) {
					sendXieNavigationEmbedded(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_CLIENT_XML)) {
					sendClientXmlModelling(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_CLIENT_XIS)) {
					sendClientService(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_CLIENT_XIE)) {
					sendClientXieAdmin(wa,req,rep);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_XML_NAVIGATION)) {
					sendXmlNavigation(wa,req,rep,xml);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_DOC_NAVIGATION)) {
					sendDocNavigation(wa,req,rep,xml);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_SRV_NAVIGATION)) {
					sendServiceNavigation(wa,req,rep,xml);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_XIE_NAVIGATION)) {
					sendXieNavigation(wa,req,rep,xml);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_MESSAGE)) {
					sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_CLEAR,(char*)" ",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				} else if (XMLIDMLIB_IsPageRequest(req,(char*)TEXT_CONSTANT_PAGE_REQ_XIE_MESSAGE)) {
					sendMessagePage(wa,req,rep,(char*)"0",XMLIDMLIB_MSGTY_CLEAR,(char*)" ",__keepdata,__refreshXML,__refreshDOC,__refreshSERV,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,__emptymsg,XMLIDMLIB_False);
					return XMLIDMLIB_True;
				}
			}
			return XMLIDMLIB_False;
		}
	}
	
	
	void sendInitialFrame(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_InitialFrameEmbedded oIFE(oWa, oReq);
		oIFE.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendInitialFrameXie(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_InitialFrameXieEmbedded oIFE(oWa, oReq);
		oIFE.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendHeader(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_HeaderEmbedded oHeader(oWa, oReq);
		oHeader.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendXieHeader(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_XieHeaderEmbedded oHeader(oWa, oReq);
		oHeader.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_NavigationEmbedded oNavi(oWa, oReq);
		oNavi.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendXieNavigationEmbedded(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_XieNavigationEmbedded oNavi(oWa, oReq);
		oNavi.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendClientXmlModelling(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_ClientXmlModelling oClientXml(oWa, oReq);
		oClientXml.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendClientService(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_ClientService oClientSrv(oWa, oReq);
		oClientSrv.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendClientXieAdmin(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_ClientXieAdmin oClientXie(oWa, oReq);
		oClientXie.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendXmlNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep, XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_XmlNavigation oXmlNavi(oWa, oReq, xml);
		oXmlNavi.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendDocNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_DocNavigation oDocNavi(oWa, oReq,xml);
		oDocNavi.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendServiceNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_ServiceNavigation oSrvNavi(oWa, oReq,xml);
		oSrvNavi.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendXieNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_XieNavigation oXieNavi(oWa, oReq,xml);
		oXieNavi.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendXieShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_Xie_Show oXieShow(oWa, oReq, xml);
		oXieShow.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendXieShowAll(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_Xie_ShowAll oXieShowAll(oWa, oReq, xml);
		oXieShowAll.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendMessagePage(sapdbwa_Handle			wa, 
					 	 sapdbwa_HttpRequestP	req,
						 sapdbwa_HttpReplyP		rep,
						 XMLIDMLIB_Char			*level,
						 XMLIDMLIB_Int2			msgty,
						 XMLIDMLIB_Char			*msg,
						 XMLIDMLIB_Bool			keepdata,
						 XMLIDMLIB_Bool			refreshXml,
						 XMLIDMLIB_Bool			refreshDoc,
						 XMLIDMLIB_Bool			refreshServ,
						 XMLIDMLIB_Char			*idxServiceID,
						 XMLIDMLIB_Char			*idxServiceName,
						 XMLIDMLIB_Char			*idxServiceDesc,
						 XMLIDMLIB_Char			*spName,
						 XMLIDMLIB_Char			*store,
						 XMLIDMLIB_Char			*docstore,
						 XMLIDMLIB_Char			*idxstore,
						 XMLIDMLIB_Char			*docstorenew,
						 XMLIDMLIB_Char			*idxstorenew,
						 XMLIDMLIB_Char			*adc,
						 XMLIDMLIB_Char			*docclassid,
						 XMLIDMLIB_Char			*docclassname,
						 XMLIDMLIB_Char			*docclassdesc,
						 XMLIDMLIB_Char			*assignedXmlIndexes,
						 XMLIDMLIB_Char			*assignedXmlIndexesCount,
						 XMLIDMLIB_Char			*newlyadded,
						 XMLIDMLIB_Bool			cancel)
	{
		Tools_DynamicUTF8String  message;
		message = msg;

		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_Message oMessage(oWa, oReq);
		oMessage.setMessage(msgty,message,keepdata,refreshXml,refreshDoc,refreshServ,idxServiceID,idxServiceName,idxServiceDesc,spName,store,docstore,idxstore,docstorenew,idxstorenew,adc,docclassid,docclassname,docclassdesc,assignedXmlIndexes,assignedXmlIndexesCount,newlyadded,cancel,level);
		oMessage.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}

	void sendXmlIndexNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_XmlIndex_New oXmlNew(oWa, oReq);
		oXmlNew.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendXmlIndexShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_XmlIndex_Show oXmlShow(oWa, oReq, xml);
		oXmlShow.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendDocClassNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_DocClass_New oDocNew(oWa, oReq, xml);
		oDocNew.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendDocClassShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_DocClass_Show oDocShow(oWa, oReq, xml);
		oDocShow.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendSessionPoolNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_SessionPool_New oSPNew(oWa, oReq);
		oSPNew.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendSessionPoolShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_SessionPool_Show oSPShow(oWa, oReq, xml);
		oSPShow.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendIndexingServiceNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_IndexingService_New oISNew(oWa,oReq,xml);
		oISNew.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	void sendIndexingServiceShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLIDMLIB_SendPageHeaders(rep,
								  (char*)TEXT_CONSTANT_MIME_TYPE_TEXT_HTML, 
								  (char*)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char*)TEXT_CONSTANT_CHARSET_ASCII);
		XMLIDMLib_HtmlTemplate_IndexingService_Show oISShow(oWa, oReq, xml);
		oISShow.writePage(Tools_TemplateWriterWA(oRep),XMLIDMLIB_False);	
	}
	
	/*--------------------------------------------------------------------------
	/* Data Work
	/*--------------------------------------------------------------------------*/
	/* XML Index */
	XMLIDMLIB_Bool createNewXmlIndex(sapdbwa_Handle wa, 
									 sapdbwa_HttpRequestP req,
									 XMLIMAPI_Handle	xml,
									 XMLIDMLIB_Char *msg,
									 XMLIDMLIB_Bool *keepdata,
									 XMLIDMLIB_Bool *refresh,
									 XMLIDMLIB_Char *idxServiceID,
									XMLIDMLIB_Char *idxServiceName,
									XMLIDMLIB_Char *idxServiceDesc,
									XMLIDMLIB_Char *spName,
									XMLIDMLIB_Char *store,
									XMLIDMLIB_Char *docstore,
									XMLIDMLIB_Char *idxstore,
									XMLIDMLIB_Char *docstorenew,
									XMLIDMLIB_Char *idxstorenew,
									XMLIDMLIB_Char *adc,
									XMLIDMLIB_Char		*docclassid,
									XMLIDMLIB_Char		*docclassname,
									XMLIDMLIB_Char		*docclassdesc,
									XMLIDMLIB_Char		*assignedXmlIndexes,
									XMLIDMLIB_Char		*assignedXmlIndexesCount,
									XMLIDMLIB_Char		*newlyadded)
	{
		XMLIMAPI_XmlIndex		__xmlIndex;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];
		XMLIDMLIB_Char			__temp[5001];
		
		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_NAME,req,__value);
  		strcpy(__xmlIndex.indexName,__value);
		if (strcmp(__xmlIndex.indexName,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Name for XML Index missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_XPATHBASEPATH,req,__value);
  		strcpy(__xmlIndex.xpathBasePath,__value);
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_XPATHVALUEPATH,req,__value);
  		strcpy(__xmlIndex.xpathValuePath,__value);
		if (strcmp(__xmlIndex.xpathValuePath,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"XPATH value path for XML Index missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_DESCRIPTION,req,__value);
  		strcpy(__xmlIndex.description,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_DESCRIPTION,req,__value);
  		strcpy(__xmlIndex.description,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strcpy(docclassid,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NAME,req,__value);
		strcpy(docclassname,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,req,__value);
		strcpy(docclassdesc,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,req,__value);
		strcpy(assignedXmlIndexes,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES_CNT,req,__value);
		strcpy(assignedXmlIndexesCount,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NEWLY_ADDED,req,__value);
		strcpy(newlyadded,__value);

		// 1. create xml index with propID produced in (1)
		//	  (will eventually be performed by xml modelling api)
		if(!XMLIMAPI_XmlIndexCreate(xml,&__xmlIndex)) {
			if (XMLIMAPI_IsError(xml,
							  	 XMLIMAPI_ERR_TYPE_MAPI,
							  	 XMLIMAPI_ERR_MAPI_CODE_XMLINDEX_ALLREADY_EXISTS))
				sp77sprintf(msg,1024,"XML Index '%s' allready exists.",__xmlIndex.indexName);
			else {
				XMLIMAPI_GetLastError(xml,&__errorItem);
				XMLIMAPI_GetErrorType(__errorItem,&__errorType);
				XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
				sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::createNewXmlIndex::create] %s\n",__errorMsg);
				sapdbwa_WriteLogMsg(wa,__logmsg);
				strcpy(msg,"Internal error occured. XML Index not created.");
			}
			strcpy(idxServiceID,"");
			strcpy(idxServiceName,"");
			strcpy(idxServiceDesc,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			strcpy(docstorenew,"");
			strcpy(idxstorenew,"");
			strcpy(docclassid,"");
			strcpy(docclassname,"");
			strcpy(docclassdesc,"");
			strcpy(assignedXmlIndexes,"");
			strcpy(assignedXmlIndexesCount,"");
			strcpy(newlyadded,"");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			/* XML Index Created -> add it to the assigned Xml Indexes String and set assignable to this xml index (in order to have
			   it selected */
			if (strcmp(assignedXmlIndexes,"")==0) {
				strcpy(assignedXmlIndexes,__xmlIndex.IndexID);
			} else {
				sp77sprintf(__temp,5000,"%s,%s",assignedXmlIndexes,__xmlIndex.IndexID);
				strcpy(assignedXmlIndexes,__temp);
			}
			if (strcmp(newlyadded,"")==0) {
				strcpy(newlyadded,__xmlIndex.IndexID);
			} else {
				sp77sprintf(__temp,5000,"%s,%s",newlyadded,__xmlIndex.IndexID);
				strcpy(newlyadded,__temp);
			}
			if (strcmp(docclassid,"")==0)
				*keepdata = XMLIDMLIB_False;
			else
				*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool updateXmlIndex(sapdbwa_Handle		wa, 
					 		   	  sapdbwa_HttpRequestP	req,
 								 XMLIMAPI_Handle	xml,
								  XMLIDMLIB_Char		*msg,
								  XMLIDMLIB_Bool		*keepdata,
								  XMLIDMLIB_Bool		*refresh,
									 XMLIDMLIB_Char *idxServiceID,
									XMLIDMLIB_Char *idxServiceName,
									XMLIDMLIB_Char *idxServiceDesc,
									XMLIDMLIB_Char *spName,
									XMLIDMLIB_Char *store,
									XMLIDMLIB_Char *docstore,
									XMLIDMLIB_Char *idxstore,
									XMLIDMLIB_Char *docstorenew,
									XMLIDMLIB_Char *idxstorenew,
									XMLIDMLIB_Char *adc,
								  XMLIDMLIB_Char		*docclassid,
								  XMLIDMLIB_Char		*docclassname,
								  XMLIDMLIB_Char		*docclassdesc,
								  XMLIDMLIB_Char		*assignedXmlIndexes,
								  XMLIDMLIB_Char		*assignedXmlIndexesCount,
								  XMLIDMLIB_Char		*newlyadded)
	{
		XMLIMAPI_XmlIndex		__xmlIndex;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_ID,req,__value);
  		strcpy(__xmlIndex.IndexID,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_XPATHBASEPATH,req,__value);
  		strcpy(__xmlIndex.xpathBasePath,__value);
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_XPATHVALUEPATH,req,__value);
  		strcpy(__xmlIndex.xpathValuePath,__value);
		if (strcmp(__xmlIndex.xpathValuePath,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"XPATH value path for XML Index missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_DESCRIPTION,req,__value);
  		strcpy(__xmlIndex.description,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strcpy(docclassid,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NAME,req,__value);
		strcpy(docclassname,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,req,__value);
		strcpy(docclassdesc,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,req,__value);
		strcpy(assignedXmlIndexes,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES_CNT,req,__value);
		strcpy(assignedXmlIndexesCount,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NEWLY_ADDED,req,__value);
		strcpy(newlyadded,__value);

		// get values from querystring
		// 1. update xml index 
		//	  (will eventually be performed by xml modelling api)
		if(!XMLIMAPI_XmlIndexUpdate(xml,&__xmlIndex)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::updateXmlIndex::update] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. XML Index not changed.");
			strcpy(idxServiceID,"");
			strcpy(idxServiceName,"");
			strcpy(idxServiceDesc,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			strcpy(docstorenew,"");
			strcpy(idxstorenew,"");
			strcpy(docclassid,"");
			strcpy(docclassname,"");
			strcpy(docclassdesc,"");
			strcpy(assignedXmlIndexes,"");
			strcpy(assignedXmlIndexesCount,"");
			strcpy(newlyadded,"");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			if (strcmp(docclassid,"")==0)
				*keepdata = XMLIDMLIB_False;
			else
				*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool deleteXmlIndex(sapdbwa_Handle &wa, 
								  sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
								  XMLIDMLIB_Char *msg,
								  XMLIDMLIB_Bool *keepdata,
								  XMLIDMLIB_Bool *refresh)
	{
		XMLIDMLib_XmlIndex		__xmlIndex;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_Id				__xmlIndexId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorCode		__errorCode;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_ID,req,__value);
  		strcpy(__xmlIndex.IndexID,__value);

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_XMLINDEX_PROPID,req,__value);
  		strcpy(__xmlIndex.PropertyID,__value);

		// 1. check wether the index is assigned to documentclasses 
		//	  (will eventually be performed by xml modelling api)
		/* perform a check wether the index can be deleted or not */
		/* maybe call back function ??? */
		XMLIMAPI_IdStringAsId(__xmlIndex.IndexID,__xmlIndexId);

		if(!XMLIMAPI_XmlIndexCheckAssign(xml,__xmlIndexId)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			if (__errorType == XMLIMAPI_ERR_TYPE_MAPI) {
				XMLIMAPI_GetErrorCode(__errorItem,&__errorCode);
				if (__errorCode == XMLIMAPI_ERR_MAPI_CODE_XMLINDEX_ASSIGNED_TO_DOCCLASS) {
					XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
					strcpy(msg,__errorMsg);
				} else {
					XMLIMAPI_GetLastError(xml,&__errorItem);
					XMLIMAPI_GetErrorType(__errorItem,&__errorType);
					XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
					sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::deleteXmlIndex::checkAssign1] %s\n",__errorMsg);
					sapdbwa_WriteLogMsg(wa,__logmsg);
					strcpy(msg,"Internal error occured. XML Index not deleted.");
				}
			} else {
				XMLIMAPI_GetLastError(xml,&__errorItem);
				XMLIMAPI_GetErrorType(__errorItem,&__errorType);
				XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
				sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::deleteXmlIndex::checkAssign2] %s\n",__errorMsg);
				sapdbwa_WriteLogMsg(wa,__logmsg);
				strcpy(msg,"Internal error occured. XML Index not deleted.");
			}
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		}

		// 2. if not delete xml index 
		if(!XMLIMAPI_XmlIndexDelete(xml,__xmlIndexId,XMLIDMLIB_True)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::deleteXmlIndex::delete] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. XML Index not deleted.");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			*keepdata = XMLIDMLIB_False;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool createNewDocClass(sapdbwa_Handle	wa, 
									 sapdbwa_HttpRequestP req,
									  XMLIMAPI_Handle xml,
									 XMLIDMLIB_Char	*msg,
								     XMLIDMLIB_Bool	*keepdata,
								     XMLIDMLIB_Bool	*refresh,
									 XMLIDMLIB_Char *idxServiceID,
									XMLIDMLIB_Char *idxServiceName,
									XMLIDMLIB_Char *idxServiceDesc,
									XMLIDMLIB_Char *spName,
									XMLIDMLIB_Char *store,
									XMLIDMLIB_Char *docstore,
									XMLIDMLIB_Char *idxstore,
									XMLIDMLIB_Char *docstorenew,
									XMLIDMLIB_Char *idxstorenew,
									XMLIDMLIB_Char *adc,
									XMLIDMLIB_Char *docclassid)
	{
		XMLIDMLib_DocClass		__docClass;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_XmlIndex		__xmlIndex;
		XMLIMAPI_XmlIndexList	__assigned;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];
		XMLIDMLIB_Char			*__temp1,*__temp2;
		XMLIDMLIB_Char			__assignedXmlIndexesString[5001];

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NAME,req,__value);
  		strcpy(__docClass.docClassName,__value);
		if (strcmp(__docClass.docClassName,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Name for Document Class missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,req,__value);
  		strcpy(__docClass.description,__value);

		// get assigned xml indices
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,req,__value);
		strncpy(__assignedXmlIndexesString,__value,5001);
		__assignedXmlIndexesString[5000]='\0';

		XMLIMAPI_XmlIndexListCreate(&__assigned);
		if (strcmp(__assignedXmlIndexesString,"")!=0) {
			__temp1 = __assignedXmlIndexesString;
			while(__temp2 = strchr(__temp1,',')) {
				if (__temp2-__temp1 == 48) {
					strncpy(__xmlIndex.IndexID,__temp1,__temp2-__temp1);
					__xmlIndex.IndexID[48] = '\0';
					XMLIMAPI_XmlIndexListAddItem(__assigned,&__xmlIndex);
				}
				__temp1 = __temp2+1;
			}
			if (strlen(__temp1)==48) {
				strcpy(__xmlIndex.IndexID,__temp1);
				XMLIMAPI_XmlIndexListAddItem(__assigned,&__xmlIndex);
			}
		}

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strncpy(docclassid,__value,48);
		docclassid[48]='\0';

		// create document class
		// (will eventually be performed by xml modelling api)
		if(!XMLIMAPI_DocClassCreate(xml,
									 __docClass.docClassName,
									 __docClass.description,
									 __assigned)) {
			if (XMLIMAPI_IsError(xml,
							  	 XMLIMAPI_ERR_TYPE_MAPI,
							  	 XMLIMAPI_ERR_MAPI_CODE_DOCCLASS_ALLREADY_EXISTS))
				sp77sprintf(msg,1024,"Document Class '%s' allready exists.",__docClass.docClassName);
			else {
				XMLIMAPI_GetLastError(xml,&__errorItem);
				XMLIMAPI_GetErrorType(__errorItem,&__errorType);
				XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
				sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::createNewDocClass::create] %s\n",__errorMsg);
				sapdbwa_WriteLogMsg(wa,__logmsg);
				strcpy(msg,"Internal error occured. Document Class not created.");
			}
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			*keepdata = XMLIDMLIB_False;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool updateDocClass(sapdbwa_Handle wa, 
					  		      sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
								  XMLIDMLIB_Char *msg,
								  XMLIDMLIB_Bool *keepdata,
								  XMLIDMLIB_Bool *refresh,
									 XMLIDMLIB_Char *idxServiceID,
									XMLIDMLIB_Char *idxServiceName,
									XMLIDMLIB_Char *idxServiceDesc,
									XMLIDMLIB_Char *spName,
									XMLIDMLIB_Char *store,
									XMLIDMLIB_Char *docstore,
									XMLIDMLIB_Char *idxstore,
									XMLIDMLIB_Char *docstorenew,
									XMLIDMLIB_Char *idxstorenew,
									XMLIDMLIB_Char *adc,
									XMLIDMLIB_Char *docclassid)
	{
		XMLIDMLib_DocClass		__docClass;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_XmlIndex		__xmlIndex;
		XMLIMAPI_XmlIndexList	__assigned;
		XMLIMAPI_Id				__docClassId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];
		XMLIDMLIB_Char			*__temp1,*__temp2;
		XMLIDMLIB_Char			__assignedXmlIndexesString[5001];


		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
  		strcpy(__docClass.docClassID,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,req,__value);
  		strcpy(__docClass.description,__value);

		// get assigned xml indices
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,req,__value);
		strncpy(__assignedXmlIndexesString,__value,5001);
		__assignedXmlIndexesString[5000]='\0';

		XMLIMAPI_XmlIndexListCreate(&__assigned);
		if (strcmp(__assignedXmlIndexesString,"")!=0) {
			__temp1 = __assignedXmlIndexesString;
			while(__temp2 = strchr(__temp1,',')) {
				if (__temp2-__temp1 == 48) {
					strncpy(__xmlIndex.IndexID,__temp1,__temp2-__temp1);
					__xmlIndex.IndexID[48] = '\0';
					XMLIMAPI_XmlIndexListAddItem(__assigned,&__xmlIndex);
				}
				__temp1 = __temp2+1;
			}
			if (strlen(__temp1)==48) {
				strcpy(__xmlIndex.IndexID,__temp1);
				XMLIMAPI_XmlIndexListAddItem(__assigned,&__xmlIndex);
			}
		}

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strncpy(docclassid,__value,48);
		docclassid[48]='\0';

		// 1. update document class 
		XMLIMAPI_IdStringAsId(__docClass.docClassID,__docClassId);
		if(!XMLIMAPI_DocClassUpdate(xml,
									 __docClassId,
									 __docClass.description,
									 __assigned)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::updateDocClass::update] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. Document Class not changed.");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			*keepdata = XMLIDMLIB_False;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool deleteDocClass(sapdbwa_Handle wa, 
								  sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
								  XMLIDMLIB_Char *msg,
								  XMLIDMLIB_Bool *keepdata,
								  XMLIDMLIB_Bool *refresh,
									 XMLIDMLIB_Char *idxServiceID,
									XMLIDMLIB_Char *idxServiceName,
									XMLIDMLIB_Char *idxServiceDesc,
									XMLIDMLIB_Char *spName,
									XMLIDMLIB_Char *store,
									XMLIDMLIB_Char *docstore,
									XMLIDMLIB_Char *idxstore,
									XMLIDMLIB_Char *docstorenew,
									XMLIDMLIB_Char *idxstorenew,
									XMLIDMLIB_Char *adc,
									XMLIDMLIB_Char *docclassid)
	{
		XMLIDMLib_DocClass		__docClass;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_Id				__docClassId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
  		strcpy(__docClass.docClassID,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strncpy(docclassid,__value,48);
		docclassid[48]='\0';

		// 1. check wether there are documents of this class
/* TODO: callback function for check must be provided */
/*		__handle = __conn->getXmlHandle();
		XMLIMAPI_IdStringAsId(__docClass.docClassID,__docClassId);
		if (!XMLIMAPI_DocClass_CheckDocs(__handle,
										 __docClassId))
		{
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		}

*/		// 2. if not delete document class 
		XMLIMAPI_IdStringAsId(__docClass.docClassID,__docClassId);
		if(!XMLIMAPI_DocClassDelete(xml,__docClassId)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::deleteDocClass::delete] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. Document Class not deleted.");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			*keepdata = XMLIDMLIB_False;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool createNewSessionPool(sapdbwa_Handle wa, 
										sapdbwa_HttpRequestP req,
									  XMLIMAPI_Handle xml,
										XMLIDMLIB_Char *msg,
										XMLIDMLIB_Bool *keepdata,
										XMLIDMLIB_Bool *refresh,
										XMLIDMLIB_Char *idxServiceID,
										XMLIDMLIB_Char *idxServiceName,
										XMLIDMLIB_Char *idxServiceDesc,
										XMLIDMLIB_Char *spName,
										XMLIDMLIB_Char *store,
										XMLIDMLIB_Char *docstore,
										XMLIDMLIB_Char *idxstore,
										XMLIDMLIB_Char *docstorenew,
										XMLIDMLIB_Char *idxstorenew,
										XMLIDMLIB_Char *adc)
	{
		XMLIMAPI_SessionPool	__sessionPool;
		XMLIDMLIB_Char			__value[513];
		XMLIDMLIB_Char			__idxServiceID[50];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_NAME,req,__value);
  		strcpy(__sessionPool.PoolName,__value);
		if (strcmp(__sessionPool.PoolName,"")==0) {
			strcpy(idxServiceID,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  			strncpy(docstorenew,__value,2);
			docstorenew[2]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  			strncpy(idxstorenew,__value,2);
			idxstorenew[2]='\0';
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Name for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DESCRIPTION,req,__value);
  		strcpy(__sessionPool.description,__value);
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBNODE,req,__value);
  		strcpy(__sessionPool.DbNode,__value);
		if (strcmp(__sessionPool.DbNode,"")==0) {
			strcpy(idxServiceID,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  			strncpy(docstorenew,__value,2);
			docstorenew[2]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  			strncpy(idxstorenew,__value,2);
			idxstorenew[2]='\0';
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Database Server for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBNAME,req,__value);
  		strcpy(__sessionPool.DbName,__value);
		if (strcmp(__sessionPool.DbName,"")==0) {
			strcpy(idxServiceID,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  			strncpy(docstorenew,__value,2);
			docstorenew[2]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  			strncpy(idxstorenew,__value,2);
			idxstorenew[2]='\0';
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Database Name for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBUSER,req,__value);
  		strcpy(__sessionPool.DbUser,__value);
		if (strcmp(__sessionPool.DbUser,"")==0) {
			strcpy(idxServiceID,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  			strncpy(docstorenew,__value,2);
			docstorenew[2]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  			strncpy(idxstorenew,__value,2);
			idxstorenew[2]='\0';
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Database User for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBPWD,req,__value);
  		strcpy(__sessionPool.DbPassword,__value);
		if (strcmp(__sessionPool.DbPassword,"")==0) {
			strcpy(idxServiceID,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  			strncpy(docstorenew,__value,2);
			docstorenew[2]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  			strncpy(idxstorenew,__value,2);
			idxstorenew[2]='\0';
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Password for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_TRACEFILE,req,__value);
  		strcpy(__sessionPool.TraceFile,__value);

		

		// 1. create session pool
		if(!XMLIMAPI_SessionPoolCreate(xml,&__sessionPool)) {
			if (XMLIMAPI_IsError(xml,
							  	 XMLIMAPI_ERR_TYPE_MAPI,
							  	 XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_ALLREADY_EXISTS))
				sp77sprintf(msg,1024,"Document Store '%s' allready exists.",__sessionPool.PoolName);
			else {
				XMLIMAPI_GetLastError(xml,&__errorItem);
				XMLIMAPI_GetErrorType(__errorItem,&__errorType);
				XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
				sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::createSessionPool::create] %s\n",__errorMsg);
				sapdbwa_WriteLogMsg(wa,__logmsg);
				strcpy(msg,"Internal error occured. Document Store not created.");
			}
			strcpy(idxServiceID,"");
			strcpy(spName,"");
			strcpy(store,"");
			strcpy(docstore,"");
			strcpy(idxstore,"");
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  			strncpy(docstorenew,__value,2);
			docstorenew[2]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  			strncpy(idxstorenew,__value,2);
			idxstorenew[2]='\0';
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			// depending on how this was called some action has to be taken
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  			strncpy(__idxServiceID,__value,49);
			__idxServiceID[49]='\0';
			strcpy(idxServiceID,__idxServiceID);
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  			strncpy(idxServiceName,__value,129);
			idxServiceName[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  			strncpy(idxServiceDesc,__value,519);
			idxServiceDesc[519]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  			strncpy(store,__value,9);
			store[9]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			if (strcmp(store,"1")==0) {
				strcpy(docstore,__sessionPool.PoolID);
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  				strncpy(docstore,__value,129);
				docstore[129]='\0';
			}
			if (strcmp(store,"2")==0) {
				strcpy(idxstore,__sessionPool.PoolID);
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  				strncpy(idxstore,__value,129);
				idxstore[129]='\0';
			}
			/* ok the new session pool was created. now delete a session pool that was created in this session before */
			if (strcmp(store,"1")==0 && strcmp(docstorenew,"1")==0) {
				/* the currently assigned session pool was newly created itself before assigning it -> remove it */
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
				if (strcmp(__value,"")!=0) 
					deleteSessionPool(wa,req,xml,__value,msg,keepdata,refresh,idxServiceID,idxServiceName,idxServiceDesc,spName,store,docstore,idxstore,docstorenew,idxstorenew,adc);
			}
			if (strcmp(store,"2")==0 && strcmp(idxstorenew,"1")==0) {
				/* the currently assigned session pool was newly created itself before assigning it -> remove it */
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
				if (strcmp(__value,"")!=0) 
					deleteSessionPool(wa,req,xml,__value,msg,keepdata,refresh,idxServiceID,idxServiceName,idxServiceDesc,spName,store,docstore,idxstore,docstorenew,idxstorenew,adc);
			}
			if (strcmp(store,"1")==0) {
				strcpy(docstorenew,"1");
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  				strncpy(docstorenew,__value,2);
				docstorenew[2]='\0';
			}
			if (strcmp(store,"2")==0) {
				strcpy(idxstorenew,"1");
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  				strncpy(idxstorenew,__value,2);
				idxstorenew[2]='\0';
			}
			if (strcmp(idxServiceID,"")==0)
				*keepdata = XMLIDMLIB_False;
			else
				*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(spName,__sessionPool.PoolName);
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool updateSessionPool(sapdbwa_Handle wa, 
				  					sapdbwa_HttpRequestP req,
									  XMLIMAPI_Handle xml,
									XMLIDMLIB_Char *msg,
									XMLIDMLIB_Bool *keepdata,
									XMLIDMLIB_Bool *refresh,
									XMLIDMLIB_Char *idxServiceID,
									XMLIDMLIB_Char *idxServiceName,
									XMLIDMLIB_Char *idxServiceDesc,
									XMLIDMLIB_Char *spName,
									XMLIDMLIB_Char *store,
									XMLIDMLIB_Char *docstore,
									XMLIDMLIB_Char *idxstore,
									XMLIDMLIB_Char *docstorenew,
									XMLIDMLIB_Char *idxstorenew,
									XMLIDMLIB_Char *adc)
	{
		XMLIMAPI_SessionPool	__sessionPool;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];
		XMLIDMLIB_Char			__idxServiceID[50];

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_ID,req,__value);
  		strcpy(__sessionPool.PoolID,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_NAME,req,__value);
  		strcpy(__sessionPool.PoolName,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DESCRIPTION,req,__value);
  		strcpy(__sessionPool.description,__value);
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBNODE,req,__value);
  		strcpy(__sessionPool.DbNode,__value);
		if (strcmp(__sessionPool.DbNode,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Database Server for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBNAME,req,__value);
  		strcpy(__sessionPool.DbName,__value);
		if (strcmp(__sessionPool.DbName,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Database Name for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBUSER,req,__value);
  		strcpy(__sessionPool.DbUser,__value);
		if (strcmp(__sessionPool.DbUser,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Database User for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_DBPWD,req,__value);
  		strcpy(__sessionPool.DbPassword,__value);
		if (strcmp(__sessionPool.DbPassword,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Password for Document Store missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_TRACEFILE,req,__value);
  		strcpy(__sessionPool.TraceFile,__value);

		// 1. update session pool
		if(!XMLIMAPI_SessionPoolUpdate(xml,&__sessionPool)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::updateSessionPool::update] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. Document Store not changed.");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  			strncpy(__idxServiceID,__value,49);
			__idxServiceID[49]='\0';
			strcpy(idxServiceID,__idxServiceID);
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  			strncpy(idxServiceName,__value,129);
			idxServiceName[129]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  			strncpy(idxServiceDesc,__value,519);
			idxServiceDesc[519]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  			strncpy(store,__value,9);
			store[9]='\0';
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  			strncpy(adc,__value,129);
			adc[129]='\0';
			if (strcmp(store,"1")==0) {
				strcpy(docstore,__sessionPool.PoolID);
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  				strncpy(docstore,__value,129);
				docstore[129]='\0';
			}
			if (strcmp(store,"2")==0) {
				strcpy(idxstore,__sessionPool.PoolID);
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  				strncpy(idxstore,__value,129);
				idxstore[129]='\0';
			}
			/* ok the new session pool was created. now delete a session pool that was created in this session before */
			if (strcmp(store,"1")==0 && strcmp(docstorenew,"1")==0) {
				/* the currently assigned session pool was newly created itself before assigning it -> remove it */
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
				if (strcmp(__value,"")!=0) 
					deleteSessionPool(wa,req,xml,__value,msg,keepdata,refresh,idxServiceID,idxServiceName,idxServiceDesc,spName,store,docstore,idxstore,docstorenew,idxstorenew,adc);
			} 
			if (strcmp(store,"2")==0 && strcmp(idxstorenew,"1")==0) {
				/* the currently assigned session pool was newly created itself before assigning it -> remove it */
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
				if (strcmp(__value,"")!=0) 
					deleteSessionPool(wa,req,xml,__value,msg,keepdata,refresh,idxServiceID,idxServiceName,idxServiceDesc,spName,store,docstore,idxstore,docstorenew,idxstorenew,adc);
			}
			if (strcmp(store,"1")==0) {
				strcpy(docstorenew,"1");
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  				strncpy(docstorenew,__value,2);
				docstorenew[2]='\0';
			}
			if (strcmp(store,"2")==0) {
				strcpy(idxstorenew,"1");
			} else {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  				strncpy(idxstorenew,__value,2);
				idxstorenew[2]='\0';
			}
			if (strcmp(idxServiceID,"")==0)
				*keepdata = XMLIDMLIB_False;
			else
				*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(spName,__sessionPool.PoolName);
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool deleteSessionPool(sapdbwa_Handle wa, 
									sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
									XMLIDMLIB_Char *id,
									XMLIDMLIB_Char *msg,
									XMLIDMLIB_Bool *keepdata,
									XMLIDMLIB_Bool *refresh,
									XMLIDMLIB_Char *idxServiceID,
									XMLIDMLIB_Char *idxServiceName,
									XMLIDMLIB_Char *idxServiceDesc,
									XMLIDMLIB_Char *spName,
									XMLIDMLIB_Char *store,
									XMLIDMLIB_Char *docstore,
									XMLIDMLIB_Char *idxstore,
									XMLIDMLIB_Char *docstorenew,
									XMLIDMLIB_Char *idxstorenew,
									XMLIDMLIB_Char *adc)
	{
		XMLIMAPI_SessionPool	__sessionPool;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];
		XMLIDMLIB_Char			__idxServiceID[50];


		// get values from querystring
		if (strcmp(id,"")==0) {
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_SESSIONPOOL_ID,req,__value);
  			strcpy(__sessionPool.PoolID,__value);
		} else {
			strcpy(__sessionPool.PoolID,id);
		}

		// 1. Check wether Document Store is used anywhere (Indexing Service)
		if(!XMLIMAPI_SessionPoolCheckUsage(xml,&__sessionPool)) {
			/* if it is assigned no implicit delete is necessary */
			return XMLIDMLIB_True;
		}

		// 2. If not used anymore -> remove it
		if(!XMLIMAPI_SessionPoolDelete(xml,&__sessionPool)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::deleteSessionPool::delete] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. Document Store not deleted.");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			if (strcmp(id,"")==0) {
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  				strncpy(__idxServiceID,__value,49);
				__idxServiceID[49]='\0';
				strcpy(idxServiceID,__idxServiceID);
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  				strncpy(idxServiceName,__value,129);
				idxServiceName[129]='\0';
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  				strncpy(idxServiceDesc,__value,519);
				idxServiceDesc[519]='\0';
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  				strncpy(store,__value,9);
				store[9]='\0';
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  				strncpy(docstore,__value,129);
				docstore[129]='\0';
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  				strncpy(idxstore,__value,129);
				idxstore[129]='\0';
				XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  				strncpy(adc,__value,129);
				adc[129]='\0';
				if (strcmp(store,"1")==0) {
					strcpy(docstorenew,"0");
				} else {
					XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  					strncpy(docstorenew,__value,2);
					docstorenew[2]='\0';
				}
				if (strcmp(store,"2")==0) {
					strcpy(idxstorenew,"0");
				} else {
					XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  					strncpy(idxstorenew,__value,2);
					idxstorenew[2]='\0';
				}
				if (strcmp(idxServiceID,"")==0)
					*keepdata = XMLIDMLIB_False;
				else
					*keepdata = XMLIDMLIB_True;
				*refresh = XMLIDMLIB_False;
				strcpy(spName,__sessionPool.PoolName);
				return XMLIDMLIB_True;
			} else {
				return XMLIDMLIB_True;
			}
		}
	}

	XMLIDMLIB_Bool createNewIndexingService(sapdbwa_Handle wa, 
											sapdbwa_HttpRequestP req,
										  XMLIMAPI_Handle xml,
											XMLIDMLIB_Char *msg,
											XMLIDMLIB_Bool *keepdata,
											XMLIDMLIB_Bool *refresh)
	{
		XMLIMAPI_ServiceDesc	__service;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];


		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strcpy(__service.ServiceName,__value);
		if (strcmp(__service.ServiceName,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Name for Indexing Service missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strcpy(__service.ServiceDesc,__value);
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strcpy(__service.DocStoreID,__value);
		if (strcmp(__value,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Document Store for Indexing Service missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
/*  		strcpy(__service.IdxStoreID,__value);*/
		strcpy(__service.IdxStoreID,__service.DocStoreID);
		if (strcmp(__service.IdxStoreID,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Index Store for Indexing Service missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		if(!XMLIMAPI_ServiceCreate(xml,&__service,XMLIDMLIB_True)) {
			if (XMLIMAPI_IsError(xml,
							  	 XMLIMAPI_ERR_TYPE_MAPI,
							  	 XMLIMAPI_ERR_MAPI_CODE_INDEXINGSERVICE_ALLREADY_EXISTS))
				 sp77sprintf(msg,1024,"Indexing Service '%s' allready exists.",__service.ServiceName);
			else {
				XMLIMAPI_GetLastError(xml,&__errorItem);
				XMLIMAPI_GetErrorType(__errorItem,&__errorType);
				XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
				sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::createNewIndexingService::create] %s\n",__errorMsg);
				sapdbwa_WriteLogMsg(wa,__logmsg);
				strcpy(msg,"Internal error occured. Indexing Service not created.");
			}
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			*keepdata = XMLIDMLIB_False;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}

	}

	XMLIDMLIB_Bool updateIndexingService(sapdbwa_Handle wa, 
										sapdbwa_HttpRequestP req,
										  XMLIMAPI_Handle xml,
										XMLIDMLIB_Char *msg,
										XMLIDMLIB_Bool *keepdata,
										XMLIDMLIB_Bool *refresh)
	{
		XMLIMAPI_ServiceDesc	__service;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];


		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strcpy(__service.ServiceID,__value);
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strcpy(__service.ServiceDesc,__value);
		
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strcpy(__service.DocStoreID,__value);
		if (strcmp(__value,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Document Store for Indexing Service missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
/*  		strcpy(__service.IdxStoreID,__value);*/
		strcpy(__service.IdxStoreID,__service.DocStoreID);
		if (strcmp(__service.IdxStoreID,"")==0) {
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			strcpy(msg,"Index Store for Indexing Service missing. Action cancelled.");
			return XMLIDMLIB_False;
		}		

		if(!XMLIMAPI_ServiceUpdate(xml,&__service)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::updateIndexingService::update] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. Indexing Service not changed.");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			*keepdata = XMLIDMLIB_False;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}

	}

	XMLIDMLIB_Bool deleteIndexingService(sapdbwa_Handle wa, 
										sapdbwa_HttpRequestP req,
									  XMLIMAPI_Handle xml,
										XMLIDMLIB_Char *msg,
										XMLIDMLIB_Bool *keepdata,
										XMLIDMLIB_Bool *refresh)
	{
		XMLIMAPI_ServiceDesc	__service;
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__logmsg[1025];

		// get values from querystring
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strcpy(__service.ServiceID,__value);

		if(!XMLIMAPI_ServiceDelete(xml,&__service)) {
			XMLIMAPI_GetLastError(xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__logmsg,1024,"[XMLIDMLIB::XMLIDMLib_SessionEmbedded::deleteIndexingService::delete] %s\n",__errorMsg);
			sapdbwa_WriteLogMsg(wa,__logmsg);
			strcpy(msg,"Internal error occured. Indexing Service not deleted.");
			*keepdata = XMLIDMLIB_True;
			*refresh = XMLIDMLIB_False;
			return XMLIDMLIB_False;
		} else {
			*keepdata = XMLIDMLIB_False;
			*refresh = XMLIDMLIB_True;
			return XMLIDMLIB_True;
		}
	}

	XMLIDMLIB_Bool cancelIndexingService(sapdbwa_Handle wa, 
										sapdbwa_HttpRequestP req,
									  XMLIMAPI_Handle xml,
										XMLIDMLIB_Char *msg,
										XMLIDMLIB_Bool *keepdata,
										XMLIDMLIB_Bool *refresh,
										XMLIDMLIB_Char *idxServiceID,
										XMLIDMLIB_Char *idxServiceName,
										XMLIDMLIB_Char *idxServiceDesc,
										XMLIDMLIB_Char *spName,
										XMLIDMLIB_Char *store,
										XMLIDMLIB_Char *docstore,
										XMLIDMLIB_Char *idxstore,
										XMLIDMLIB_Char *docstorenew,
										XMLIDMLIB_Char *idxstorenew,
										XMLIDMLIB_Char *adc)
	{
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_SessionPool	__sessionpool;

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';
		/* the assigned DocStore and/or IdxStore could be just created -> delete them */
		if (strcmp(docstorenew,"1")==0) {
			strcpy(__sessionpool.PoolID,docstore);
			/* the currently assigned DocStore is new created -> delete it */
			if (XMLIMAPI_SessionPoolCheckUsage(xml,&__sessionpool)) {
				XMLIMAPI_SessionPoolDelete(xml,&__sessionpool);
			}
		}
		if (strcmp(idxstorenew,"1")==0) {
			strcpy(__sessionpool.PoolID,idxstore);
			/* the currently assigned IdxStore is new created -> delete it */
			if (XMLIMAPI_SessionPoolCheckUsage(xml,&__sessionpool)) {
				XMLIMAPI_SessionPoolDelete(xml,&__sessionpool);
			}
		}
		*keepdata = XMLIDMLIB_False;
		*refresh = XMLIDMLIB_True;
		return XMLIDMLIB_True;
	}

	XMLIDMLIB_Bool cancelSessionPool(sapdbwa_Handle wa, 
										sapdbwa_HttpRequestP req,
										XMLIDMLIB_Char *msg,
										XMLIDMLIB_Bool *keepdata,
										XMLIDMLIB_Bool *refresh,
										XMLIDMLIB_Char *idxServiceID,
										XMLIDMLIB_Char *idxServiceName,
										XMLIDMLIB_Char *idxServiceDesc,
										XMLIDMLIB_Char *spName,
										XMLIDMLIB_Char *store,
										XMLIDMLIB_Char *docstore,
										XMLIDMLIB_Char *idxstore,
										XMLIDMLIB_Char *docstorenew,
										XMLIDMLIB_Char *idxstorenew,
										XMLIDMLIB_Char *adc)
	{
		XMLIDMLIB_Char			__value[513];

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		if (strcmp(store,"1")==0) {
			strcpy(docstorenew,"0");
		} else {
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  			strncpy(docstorenew,__value,2);
			docstorenew[2]='\0';
		}
		if (strcmp(store,"2")==0) {
			strcpy(idxstorenew,"0");
		} else {
			XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  			strncpy(idxstorenew,__value,2);
			idxstorenew[2]='\0';
		}
		*keepdata = XMLIDMLIB_False;
		*refresh = XMLIDMLIB_True;
		return XMLIDMLIB_True;
	}

	XMLIDMLIB_Bool cancelXmlIndex(sapdbwa_Handle wa, 
								  sapdbwa_HttpRequestP req,
								  XMLIDMLIB_Char *msg,
								  XMLIDMLIB_Bool *keepdata,
								  XMLIDMLIB_Bool *refresh,
								XMLIDMLIB_Char *idxServiceID,
								XMLIDMLIB_Char *idxServiceName,
								XMLIDMLIB_Char *idxServiceDesc,
								XMLIDMLIB_Char *spName,
								XMLIDMLIB_Char *store,
								XMLIDMLIB_Char *docstore,
								XMLIDMLIB_Char *idxstore,
								XMLIDMLIB_Char *docstorenew,
								XMLIDMLIB_Char *idxstorenew,
								XMLIDMLIB_Char *adc,
								  XMLIDMLIB_Char *docclassid,
								  XMLIDMLIB_Char *docclassname,
								  XMLIDMLIB_Char *docclassdesc,
								  XMLIDMLIB_Char *assignedXmlIndexes,
								  XMLIDMLIB_Char *assignedXmlIndexesCount,
								  XMLIDMLIB_Char *newlyAdded)
	{
		XMLIDMLIB_Char			__value[513];

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strcpy(docclassid,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NAME,req,__value);
		strcpy(docclassname,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,req,__value);
		strcpy(docclassdesc,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,req,__value);
		strcpy(assignedXmlIndexes,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES_CNT,req,__value);
		strcpy(assignedXmlIndexesCount,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NEWLY_ADDED,req,__value);
		strcpy(newlyAdded,__value);

		*keepdata = XMLIDMLIB_False;
		*refresh = XMLIDMLIB_True;
		return XMLIDMLIB_True;
	}

	XMLIDMLIB_Bool cancelDocClass(sapdbwa_Handle wa, 
								  sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
								  XMLIDMLIB_Char *msg,
								  XMLIDMLIB_Bool *keepdata,
								  XMLIDMLIB_Bool *refresh,
								XMLIDMLIB_Char *idxServiceID,
								XMLIDMLIB_Char *idxServiceName,
								XMLIDMLIB_Char *idxServiceDesc,
								XMLIDMLIB_Char *spName,
								XMLIDMLIB_Char *store,
								XMLIDMLIB_Char *docstore,
								XMLIDMLIB_Char *idxstore,
								XMLIDMLIB_Char *docstorenew,
								XMLIDMLIB_Char *idxstorenew,
								XMLIDMLIB_Char *adc,
								  XMLIDMLIB_Char *docclassid,
								  XMLIDMLIB_Char *docclassname,
								  XMLIDMLIB_Char *docclassdesc,
								  XMLIDMLIB_Char *assignedXmlIndexes,
								  XMLIDMLIB_Char *assignedXmlIndexesCount,
								  XMLIDMLIB_Char *newlyAdded)
	{
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_XmlIndex		__xmlIndex;
		XMLIMAPI_Id				__xmlIndexId;
		XMLIDMLIB_Char			*__temp1,*__temp2;

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ID,req,__value);
  		strncpy(idxServiceID,__value,49);
		idxServiceID[49]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_NAME,req,__value);
  		strncpy(idxServiceName,__value,129);
		idxServiceName[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION,req,__value);
  		strncpy(idxServiceDesc,__value,519);
		idxServiceDesc[519]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_STORE,req,__value);
  		strncpy(store,__value,9);
		store[9]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE,req,__value);
  		strncpy(docstore,__value,129);
		docstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE,req,__value);
  		strncpy(idxstore,__value,129);
		idxstore[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS,req,__value);
  		strncpy(adc,__value,129);
		adc[129]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW,req,__value);
  		strncpy(docstorenew,__value,2);
		docstorenew[2]='\0';
		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW,req,__value);
  		strncpy(idxstorenew,__value,2);
		idxstorenew[2]='\0';

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ID,req,__value);
		strcpy(docclassid,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NAME,req,__value);
		strcpy(docclassname,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION,req,__value);
		strcpy(docclassdesc,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES,req,__value);
		strcpy(assignedXmlIndexes,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES_CNT,req,__value);
		strcpy(assignedXmlIndexesCount,__value);

		XMLIDMLIB_GetParameterValueC(TEXT_CONSTANT_PARAM_DOCCLASS_NEWLY_ADDED,req,__value);
		strcpy(newlyAdded,__value);

		/* delete all newly created XML Indexes of that session */
		if (strcmp(newlyAdded,"")!=0) {
			__temp1 = newlyAdded;
			while(__temp2 = strchr(__temp1,',')) {
				if (__temp2-__temp1 == 48) {
					strncpy(__xmlIndex.IndexID,__temp1,__temp2-__temp1);
					__xmlIndex.IndexID[48] = '\0';
					XMLIMAPI_IdStringAsId(__xmlIndex.IndexID,__xmlIndexId);
					if (XMLIMAPI_XmlIndexCheckAssign(xml,__xmlIndexId)) {
						/* no more assignments -> delete the xml index */
						XMLIMAPI_XmlIndexDelete(xml,__xmlIndexId,XMLIDMLIB_True);
					}
				}
				__temp1 = __temp2+1;
			}
			if (strlen(__temp1)==48) {
				strcpy(__xmlIndex.IndexID,__temp1);
				__xmlIndex.IndexID[48] = '\0';
				XMLIMAPI_IdStringAsId(__xmlIndex.IndexID,__xmlIndexId);
				if (XMLIMAPI_XmlIndexCheckAssign(xml,__xmlIndexId)) {
					/* no more assignments -> delete the xml index */
					XMLIMAPI_XmlIndexDelete(xml,__xmlIndexId,XMLIDMLIB_True);
				}
			}
		}

		*keepdata = XMLIDMLIB_False;
		*refresh = XMLIDMLIB_True;
		return XMLIDMLIB_True;
	}


