/*


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



*/
#ifndef XMLIDMLIB_SESSION_EMBEDDED_H
#define XMLIDMLIB_SESSION_EMBEDDED_H

/*!**********************************************************************

  module: XMLIDMLib_SessionEmbedded.h

  -----------------------------------------------------------------------

  responsible:  Daniel Kirmse (D027470 - Daniel.Kirmse@SAP.com)

  special area: WebDAV / XML Indexing

  description:  Point of entry for WebDAV embedded XML Html Modelling GUI

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/


	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_TextConstants.h"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_InitialFrameEmbedded.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_InitialFrameXieEmbedded.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_HeaderEmbedded.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XieHeaderEmbedded.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_NavigationEmbedded.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XieNavigationEmbedded.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ClientXmlModelling.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ClientService.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ClientXieAdmin.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XmlNavigation.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_DocNavigation.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ServiceNavigation.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XieNavigation.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_Message.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XmlIndex_New.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XmlIndex_Show.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_DocClass_New.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_DocClass_Show.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_SessionPool_New.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_SessionPool_Show.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_IndexingService_New.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_IndexingService_Show.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_Xie_Show.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_Xie_ShowAll.hpp"
	

	/*===================================================================
	// Defines
	//===================================================================*/

	void sendInitialFrame(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendInitialFrameXie(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendHeader(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendXieHeader(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendXieNavigationEmbedded(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendClientXmlModelling(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendClientService(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendClientXieAdmin(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendXmlNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendDocNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendXieNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendServiceNavigation(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendMessagePage(sapdbwa_Handle	wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep, XMLIDMLIB_Char *level, XMLIDMLIB_Int2 msgty, XMLIDMLIB_Char *msg, XMLIDMLIB_Bool keepdata, XMLIDMLIB_Bool refreshXml, XMLIDMLIB_Bool refreshDoc, XMLIDMLIB_Bool refreshServ, XMLIDMLIB_Char *idxServiceID,XMLIDMLIB_Char *idxServiceName,XMLIDMLIB_Char *idxServiceDesc, XMLIDMLIB_Char *spName, XMLIDMLIB_Char *store, XMLIDMLIB_Char *docstore, XMLIDMLIB_Char *idxstore, XMLIDMLIB_Char *docstorenew, XMLIDMLIB_Char *idxstorenew, XMLIDMLIB_Char *adc,XMLIDMLIB_Char *docclassid,XMLIDMLIB_Char *docclassname,XMLIDMLIB_Char *docclassdesc,XMLIDMLIB_Char *assignedXmlIndexes,XMLIDMLIB_Char *assignedXmlIndexesCount,XMLIDMLIB_Char *newlyadded,XMLIDMLIB_Bool cancel);
	void sendXmlIndexNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendXmlIndexShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendDocClassNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendDocClassShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendSessionPoolNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep);
	void sendSessionPoolShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendIndexingServiceNew(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendIndexingServiceShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendXieShow(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);
	void sendXieShowAll(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep,XMLIMAPI_Handle xml);

	/*--------------------------------------------------------------------------
	/* Data Work
	/*--------------------------------------------------------------------------*/
	/* XML Index */
	XMLIDMLIB_Bool createNewXmlIndex(sapdbwa_Handle wa, 
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
									XMLIDMLIB_Char		*docclassid,
									XMLIDMLIB_Char		*docclassname,
									XMLIDMLIB_Char		*docclassdesc,
									XMLIDMLIB_Char		*assignedXmlIndexes,
									XMLIDMLIB_Char		*assignedXmlIndexesCount,
									XMLIDMLIB_Char		*newlyadded);
	XMLIDMLIB_Bool updateXmlIndex(sapdbwa_Handle		wa, 
					 		   	  sapdbwa_HttpRequestP	req,
								  XMLIMAPI_Handle xml,
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
								  XMLIDMLIB_Char		*newlyadded);
	XMLIDMLIB_Bool deleteXmlIndex(sapdbwa_Handle &wa, 
								  sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
								  XMLIDMLIB_Char *msg,
								  XMLIDMLIB_Bool *keepdata,
								  XMLIDMLIB_Bool *refresh);
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
									XMLIDMLIB_Char *docclass);
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
									XMLIDMLIB_Char *docclass);
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
									XMLIDMLIB_Char *docclassid);
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
										XMLIDMLIB_Char *adc);

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
									XMLIDMLIB_Char *adc);

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
									XMLIDMLIB_Char *adc);

	XMLIDMLIB_Bool createNewIndexingService(sapdbwa_Handle wa, 
											sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
											XMLIDMLIB_Char *msg,
											XMLIDMLIB_Bool *keepdata,
											XMLIDMLIB_Bool *refresh);

	XMLIDMLIB_Bool updateIndexingService(sapdbwa_Handle wa, 
										sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
										XMLIDMLIB_Char *msg,
										XMLIDMLIB_Bool *keepdata,
										XMLIDMLIB_Bool *refresh);

	XMLIDMLIB_Bool deleteIndexingService(sapdbwa_Handle wa, 
										sapdbwa_HttpRequestP req,
								  XMLIMAPI_Handle xml,
										XMLIDMLIB_Char *msg,
										XMLIDMLIB_Bool *keepdata,
										XMLIDMLIB_Bool *refresh);
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
										XMLIDMLIB_Char *adc);
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
										XMLIDMLIB_Char *adc);
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
								  XMLIDMLIB_Char *newlyAdded);
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
								  XMLIDMLIB_Char *newlyAdded);
#endif