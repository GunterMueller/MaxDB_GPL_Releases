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
#ifndef XMLQCLIB_COMMON_TEXTCONSTANTS_H
#define XMLQCLIB_COMMON_TEXTCONSTANTS_H

/*!**********************************************************************

  module: XMLQCLib_Common_TextConstants.h

  -----------------------------------------------------------------------

  responsible:  Daniel Kirmse (D027470 - Daniel.Kirmse@SAP.com)

  special area: WebDAV / XML Indexing

  description:  Text constants 

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/


	/*===================================================================
	// Defines
	//===================================================================*/
	/* HTML Pages */
	#define TEXT_CONSTANT_QCL_HTML_INITIALFRAME_EMBEDDED		"XMLQCLib_Html_InitialFrameEmbedded.htm"

	/* Page Requests */
	#define TEXT_CONSTANT_QCL_PAGE_REQ_INITIALFRAME_EMBEDDED	"query=0&start=1"
	#define TEXT_CONSTANT_QCL_PAGE_REQ_HEADER_EMBEDDED			"query=0&header=1"
	#define TEXT_CONSTANT_QCL_PAGE_REQ_MESSAGE_EMBEDDED			"query=0&message=1"
	#define TEXT_CONSTANT_QCL_PAGE_REQ_NAVIGATION_EMBEDDED		"query=0&navigation=1"
	#define TEXT_CONSTANT_QCL_PAGE_REQ_CLIENT_QUERY				"query=0&client=query"
	
	/* Commands */
	#define TEXT_CONSTANT_QCL_CMD_PREFIX						"query=0&cmd="
	#define TEXT_CONSTANT_QCL_CMD_QUERY_SHOW_FREE				"FreeQuery"
	#define TEXT_CONSTANT_QCL_CMD_QUERY_SET_DC					"setDc"
	#define TEXT_CONSTANT_QCL_CMD_QUERY_ADD_OR					"addOr"
	#define TEXT_CONSTANT_QCL_CMD_QUERY_MORE					"addLine"
	#define TEXT_CONSTANT_QCL_CMD_QUERY_ADD_DC_LINE				"addDcLine"
	#define TEXT_CONSTANT_QCL_CMD_QUERY_SEARCH					"search"

	/* Messages */
	#define TEXT_CONSTANT_QCL_MSG_NO_CONNECTION					"Could not connect to database."
	
	/* Parameter Names */
	#define TEXT_CONSTANT_QCL_PARAM_NAVI_TAB					"Tab"
	#define TEXT_CONSTANT_QCL_PARAM_QUERY_SEL_DC_STRING			"SDCS"
	#define TEXT_CONSTANT_QCL_PARAM_SUB_COMMAND					"scmd"
	#define TEXT_CONSTANT_QCL_PARAM_QUERY_IDX_LINES				"IDXLines"
	
	/* Parameter Values */
	#define TEXT_CONSTANT_QCL_PARAM_VAL_CLNT_XML_DATA_EMPTY		"empty"
	
	/* HTML Fragments */
	#define TEXT_CONSTANT_QCL_HTML_MSGTY_CLEAR_GIF				"1x1.gif"

	/* Mime types */
	#define TEXT_CONSTANT_QCL_MIME_TYPE_TEXT_HTML				"text/html"
	
	/* charset */
	#define TEXT_CONSTANT_QCL_CHARSET_UTF8						"UTF-8"
	#define TEXT_CONSTANT_QCL_CHARSET_ASCII						"iso-8859-1"
	
#endif