/*!**********************************************************************

  module: XMLQCLib_SessionEmbedded.c

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
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_SessionEmbedded.hpp"
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Definition.h"
	#include "sapdbwa.hpp" //nocheck


	SAPDB_Bool XMLQCLIB_doQueryEmbedded(sapdbwa_Handle    wa,
							  		   sapdbwa_HttpRequestP req,
							 		   sapdbwa_HttpReplyP   rep,
									   XMLQSLib_Handle	qs,
									   XMLIMAPI_Handle xml  )
	{ 
		if (!XMLQCLIB_IsQueryStringValid(req)) {
			// send the web page
			//sendInitialFrame(wa,req,rep);
			return XMLQCLIB_True;
		} else {
			if (XMLQCLIB_IsCommandRequest(req)) {
				/*-------------------------------------------------------------
				/* Commands
				/*-------------------------------------------------------------*/
				//------------------------------------------------------------------------
				// Free Query
				//------------------------------------------------------------------------
				if (XMLQCLIB_IsCommand(req,(char *)TEXT_CONSTANT_QCL_CMD_QUERY_SHOW_FREE)) {
					sendFreeQuery(wa,req,rep,qs,xml);
				}
			} else {
				/*-------------------------------------------------------------
				/* HTML Pages called w/o user action
				/*-------------------------------------------------------------*/
				if (XMLQCLIB_IsPageRequest(req,(char *)TEXT_CONSTANT_QCL_PAGE_REQ_INITIALFRAME_EMBEDDED)) {
					sendInitialFrameQuery(wa,req,rep);
					return XMLQCLIB_True;
				} else if (XMLQCLIB_IsPageRequest(req,(char *)TEXT_CONSTANT_QCL_PAGE_REQ_HEADER_EMBEDDED)) {
					sendHeaderQuery(wa,req,rep);
					return XMLQCLIB_True;
				} else if (XMLQCLIB_IsPageRequest(req,(char *)TEXT_CONSTANT_QCL_PAGE_REQ_MESSAGE_EMBEDDED)) {
					sendMessageQuery(wa,req,rep);
					return XMLQCLIB_True;
				} else if (XMLQCLIB_IsPageRequest(req,(char *)TEXT_CONSTANT_QCL_PAGE_REQ_NAVIGATION_EMBEDDED)) {
					sendNavigationQuery(wa,req,rep);
					return XMLQCLIB_True;
				} else if (XMLQCLIB_IsPageRequest(req,(char *)TEXT_CONSTANT_QCL_PAGE_REQ_CLIENT_QUERY)) {
					sendClientQuery(wa,req,rep);
					return XMLQCLIB_True;
				}
			}
			return XMLQCLIB_False;
		}
	}
	
	void sendInitialFrameQuery(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLQCLIB_SendPageHeaders(rep,
								  (char *)TEXT_CONSTANT_QCL_MIME_TYPE_TEXT_HTML, 
								  (char *)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char *)TEXT_CONSTANT_QCL_CHARSET_UTF8);
		XMLQCLib_HtmlTemplate_InitialFrameEmbedded oIFE(oWa, oReq);
		oIFE.writePage(Tools_TemplateWriterWA(oRep),XMLQCLIB_False);	
	}
	
	void sendHeaderQuery(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLQCLIB_SendPageHeaders(rep,
								  (char *)TEXT_CONSTANT_QCL_MIME_TYPE_TEXT_HTML, 
								  (char *)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char *)TEXT_CONSTANT_QCL_CHARSET_UTF8);
		XMLQCLib_HtmlTemplate_HeaderEmbedded oHeader(oWa, oReq);
		oHeader.writePage(Tools_TemplateWriterWA(oRep),XMLQCLIB_False);	
	}
	
	void sendMessageQuery(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		XMLQCLIB_SendHTMLFile(wa, rep, "XMLQCLib_Html_Message.htm");
	}
	
	void sendNavigationQuery(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLQCLIB_SendPageHeaders(rep,
								  (char *)TEXT_CONSTANT_QCL_MIME_TYPE_TEXT_HTML, 
								  (char *)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char *)TEXT_CONSTANT_QCL_CHARSET_UTF8);
		XMLQCLib_HtmlTemplate_NavigationEmbedded oNavi(oWa, oReq);
		oNavi.writePage(Tools_TemplateWriterWA(oRep),XMLQCLIB_False);	
	}
	
	void sendClientQuery(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLQCLIB_SendPageHeaders(rep,
								  (char *)TEXT_CONSTANT_QCL_MIME_TYPE_TEXT_HTML, 
								  (char *)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char *)TEXT_CONSTANT_QCL_CHARSET_UTF8);
		XMLQCLib_HtmlTemplate_ClientQuery oClientQuery(oWa, oReq);
		oClientQuery.writePage(Tools_TemplateWriterWA(oRep),XMLQCLIB_False);	
	}
	
	void sendFreeQuery(sapdbwa_Handle wa, sapdbwa_HttpRequestP req, sapdbwa_HttpReplyP rep, XMLQSLib_Handle qs, XMLIMAPI_Handle xml)
	{
		// convert C-style Handle and reply into c++ objects
		sapdbwa_WebAgent oWa(wa);
		sapdbwa_HttpRequest oReq(req);
		sapdbwa_HttpReply oRep(rep);
		
		XMLQCLIB_SendPageHeaders(rep,
								  (char *)TEXT_CONSTANT_QCL_MIME_TYPE_TEXT_HTML, 
								  (char *)"Sat, 01 Jan 2000 00:00:00 GMT", 
								  (char *)TEXT_CONSTANT_QCL_CHARSET_UTF8);
		XMLQCLib_HtmlTemplate_FreeQuery oFQ(oWa, oReq, qs, xml);
		oFQ.writePage(Tools_TemplateWriterWA(oRep),XMLQCLIB_False);	
	}
	
	
