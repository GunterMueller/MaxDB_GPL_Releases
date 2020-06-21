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
#ifndef XMLIDMLIB_COMMON_TEXTCONSTANTS_H
#define XMLIDMLIB_COMMON_TEXTCONSTANTS_H

/*!**********************************************************************

  module: XMLIDMLib_Common_TextConstants.h

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
	#define TEXT_CONSTANT_HTML_INITIALFRAME_EMBEDDED		"XMLIDMLib_Html_InitialFrameEmbedded.htm"
	#define TEXT_CONSTANT_HTML_INITIALFRAME_ALONE			"XMLIDMLib_Html_InitialFrameAlone.htm"

	/* Page Requests */
	#define TEXT_CONSTANT_PAGE_REQ_INITIALFRAME_EMBEDDED	"model=0&start=1"
	#define TEXT_CONSTANT_PAGE_REQ_INITIALFRAMEXIE_EMBEDDED	"xie=0&start=1"
	#define TEXT_CONSTANT_PAGE_REQ_INITIALFRAMEQUERY_EMBEDDED	"query=0&start=1"
	#define TEXT_CONSTANT_PAGE_REQ_HEADER_EMBEDDED			"model=0&header=1"
	#define TEXT_CONSTANT_PAGE_REQ_XIEHEADER_EMBEDDED		"xie=0&header=1"
	#define TEXT_CONSTANT_PAGE_REQ_QUERYHEADER_EMBEDDED		"query=0&header=1"
	#define TEXT_CONSTANT_PAGE_REQ_NAVIGATION_EMBEDDED		"model=0&navigation=1"
	#define TEXT_CONSTANT_PAGE_REQ_XIENAVIGATION_EMBEDDED	"xie=0&navigation=1"
	#define TEXT_CONSTANT_PAGE_REQ_QUERYNAVIGATION_EMBEDDED	"query=0&navigation=1"
	#define TEXT_CONSTANT_PAGE_REQ_CLIENT_XIS				"model=0&client=xis"
	#define TEXT_CONSTANT_PAGE_REQ_CLIENT_XML				"model=0&client=xml&data=empty"
	#define TEXT_CONSTANT_PAGE_REQ_CLIENT_XML_CLIENT_EMPTY	"model=0&client=xml&data=empty"
	#define TEXT_CONSTANT_PAGE_REQ_CLIENT_XML_CLIENT_NEWXML	"model=0&client=xml&data=newxml"
	#define TEXT_CONSTANT_PAGE_REQ_CLIENT_XML_CLIENT_NEWDOC	"model=0&client=xml&data=newdoc"
	#define TEXT_CONSTANT_PAGE_REQ_CLIENT_XIE				"xie=0&client=xie"
	#define TEXT_CONSTANT_PAGE_REQ_CLIENT_QUERY				"query=0&client=query"
	#define TEXT_CONSTANT_PAGE_REQ_MESSAGE					"model=0&message=1"
	#define TEXT_CONSTANT_PAGE_REQ_XML_NAVIGATION			"model=0&xmlnavi=1"
	#define TEXT_CONSTANT_PAGE_REQ_DOC_NAVIGATION			"model=0&docnavi=1"
	#define TEXT_CONSTANT_PAGE_REQ_SRV_NAVIGATION			"model=0&srvnavi=1"
	#define TEXT_CONSTANT_PAGE_REQ_XIE_NAVIGATION			"xie=0&xienavi=1"
	#define TEXT_CONSTANT_PAGE_REQ_XIE_MESSAGE				"xie=0&message=1"
	#define TEXT_CONSTANT_PAGE_REQ_QUERY_MESSAGE			"query=0&message=1"
	
	/* Commands */
	#define TEXT_CONSTANT_CMD_PREFIX						"model=0&cmd="
	#define TEXT_CONSTANT_CMD_XMLINDEX_NEW					"XmlIndexNew"
	#define TEXT_CONSTANT_CMD_XMLINDEX_CREATE				"XmlIndexCreate"
	#define TEXT_CONSTANT_CMD_XMLINDEX_SHOW					"XmlIndexShow"
	#define TEXT_CONSTANT_CMD_XMLINDEX_UPDATE				"XmlIndexUpd"
	#define TEXT_CONSTANT_CMD_XMLINDEX_DELETE				"XmlIndexDel"
	#define TEXT_CONSTANT_CMD_DOCCLASS_NEW					"DocClassNew"
	#define TEXT_CONSTANT_CMD_DOCCLASS_CREATE				"DocClassCreate"
	#define TEXT_CONSTANT_CMD_DOCCLASS_SHOW					"DocClassShow"
	#define TEXT_CONSTANT_CMD_DOCCLASS_UPDATE				"DocClassUpd"
	#define TEXT_CONSTANT_CMD_DOCCLASS_DELETE				"DocClassDel"
	#define TEXT_CONSTANT_CMD_DOCCLASS_ADDINDEX				"DcAddIndex"
	#define TEXT_CONSTANT_CMD_DOCCLASS_REMOVEALLINDEX		"DcRemoveAllIndex"
	#define TEXT_CONSTANT_CMD_DOCCLASS_REMOVEINDEX			"DcRemoveIndex"
	#define TEXT_CONSTANT_CMD_DOCCLASS_ADDINDEXNEW			"DcAddIndexNew"
	#define TEXT_CONSTANT_CMD_SESSIONPOOL_NEW				"SPNew"
	#define TEXT_CONSTANT_CMD_SESSIONPOOL_CREATE			"SPCreate"
	#define TEXT_CONSTANT_CMD_SESSIONPOOL_SHOW				"SPShow"
	#define TEXT_CONSTANT_CMD_SESSIONPOOL_UPDATE			"SPUpd"
	#define TEXT_CONSTANT_CMD_SESSIONPOOL_DELETE			"SPDel"
	#define TEXT_CONSTANT_CMD_INDEXINGSERVICE_NEW			"ISNew"
	#define TEXT_CONSTANT_CMD_INDEXINGSERVICE_CREATE		"ISCreate"
	#define TEXT_CONSTANT_CMD_INDEXINGSERVICE_SHOW			"ISShow"
	#define TEXT_CONSTANT_CMD_INDEXINGSERVICE_UPDATE		"ISUpd"
	#define TEXT_CONSTANT_CMD_INDEXINGSERVICE_DELETE		"ISDel"
	#define TEXT_CONSTANT_CMD_XIE_SHOW						"XieShow"
	#define TEXT_CONSTANT_CMD_XIE_OVERVIEW					"XieShowAll"
	#define TEXT_CONSTANT_CMD_QUERY_SHOW_FREE				"FreeQuery"
	#define TEXT_CONSTANT_CMD_QUERY_ADD_DC					"addDc"
	#define TEXT_CONSTANT_CMD_QUERY_REMOVE_DC				"rmvDc"
	#define TEXT_CONSTANT_CMD_QUERY_ADD_OR					"addOr"
	#define TEXT_CONSTANT_CMD_QUERY_MORE					"addLine"
	#define TEXT_CONSTANT_CMD_MESSAGE_CLEAR					"MsgClear"
	#define TEXT_CONSTANT_CMD_CANCEL						"cancel"
	#define TEXT_CONSTANT_CMD_CANCEL_IS						"cancelis"
	#define TEXT_CONSTANT_CMD_CANCEL_SP						"cancelsp"
	#define TEXT_CONSTANT_CMD_CANCEL_DC						"canceldc"
	#define TEXT_CONSTANT_CMD_CANCEL_XI						"cancelxi"

	#define TEXT_CONSTANT_CMD_XIE_PREFIX					"xie=0&cmd="
	#define TEXT_CONSTANT_CMD_QUERY_PREFIX					"query=0&cmd="

	/* Messages */
	#define TEXT_CONSTANT_MSG_XMLINDEX_UPDATE				"XML Index updated"
	#define TEXT_CONSTANT_MSG_XMLINDEX_DELETE				"XML Index deleted"
	#define TEXT_CONSTANT_MSG_NO_CONNECTION					"Could not connect to database."
	
	/* Parameter Names */
	#define TEXT_CONSTANT_PARAM_NAVI_TAB					"Tab"
	#define TEXT_CONSTANT_PARAM_CLIENT_XML_DATA				"data"
	#define TEXT_CONSTANT_PARAM_XMLINDEX_NAME				"XmlIndexName"
	#define TEXT_CONSTANT_PARAM_XMLINDEX_ID					"XmlIndexID"
	#define TEXT_CONSTANT_PARAM_XMLINDEX_PROPID				"XmlIndexPropID"
	#define TEXT_CONSTANT_PARAM_XMLINDEX_XPATHBASEPATH		"XpathBasePath"
	#define TEXT_CONSTANT_PARAM_XMLINDEX_XPATHVALUEPATH		"XpathValuePath"
	#define TEXT_CONSTANT_PARAM_XMLINDEX_DESCRIPTION		"XmlIndexDescription"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_ID					"DocClassID"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_NAME				"DocClassName"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_DESCRIPTION		"DocClassDescription"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES "AXIString"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNED_XML_INDEXES_CNT "AXICount"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_ASSIGNABLE			"Assignable"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_UNASSIGNED_XML_INDEXES "UXIString"
	#define TEXT_CONSTANT_PARAM_DOCCLASS_NEWLY_ADDED		"NewlyAdded"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_ID				"SPID"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_NAME			"SPName"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_DESCRIPTION		"SPDesc"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_DBNODE			"SPDbNode"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_DBNAME			"SPDbName"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_DBUSER			"SPDbUser"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_DBPWD			"SPDbPwd"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_POOLSIZE		"SPPoolSize"
	#define TEXT_CONSTANT_PARAM_SESSIONPOOL_TRACEFILE		"SPTraceFile"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_ID				"ISID"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_NAME				"ISName"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_DESCRIPTION		"ISDesc"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_DOCSTORE			"DSSP"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_IDXSTORE			"ISSP"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_DSSPNEW			"DSSPNew"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_ISSPNEW			"ISSPNew"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_STORE			"DocIdxStore"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_DOCCLASS			"ADC"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_INIT				"init"
	#define TEXT_CONSTANT_PARAM_IDXSERVICE_SYNC				"ISSync"
	#define TEXT_CONSTANT_PARAM_XMLINDEXID1					"id1"
	#define TEXT_CONSTANT_PARAM_XMLINDEXID2					"id2"
	#define TEXT_CONSTANT_PARAM_XMLINDEXID3					"id3"
	#define TEXT_CONSTANT_PARAM_XMLINDEXID4					"id4"
	#define TEXT_CONSTANT_PARAM_XMLINDEXID5					"id5"
	#define TEXT_CONSTANT_PARAM_XMLINDEXNAME1				"name1"
	#define TEXT_CONSTANT_PARAM_XMLINDEXNAME2				"name2"
	#define TEXT_CONSTANT_PARAM_XMLINDEXNAME3				"name3"
	#define TEXT_CONSTANT_PARAM_XMLINDEXNAME4				"name4"
	#define TEXT_CONSTANT_PARAM_XMLINDEXNAME5				"name5"
	#define TEXT_CONSTANT_PARAM_XMLINDEXDESC1				"desc1"
	#define TEXT_CONSTANT_PARAM_XMLINDEXDESC2				"desc2"
	#define TEXT_CONSTANT_PARAM_XMLINDEXDESC3				"desc3"
	#define TEXT_CONSTANT_PARAM_XMLINDEXDESC4				"desc4"
	#define TEXT_CONSTANT_PARAM_XMLINDEXDESC5				"desc5"
	#define TEXT_CONSTANT_PARAM_ASSIGNABLE					"Assignable"
	#define TEXT_CONSTANT_PARAM_STATE						"state"
	#define TEXT_CONSTANT_PARAM_ACOUNT						"acount"
	#define TEXT_CONSTANT_PARAM_CANCEL						"cancel"
	#define TEXT_CONSTANT_PARAM_XIE_ID						"XIEID"
	#define TEXT_CONSTANT_PARAM_QUERY_SEL_DC_STRING			"SDCS"
	#define TEXT_CONSTANT_PARAM_COMMAND						"cmd"
	#define TEXT_CONSTANT_PARAM_SUB_COMMAND					"scmd"
	#define TEXT_CONSTANT_PARAM_QUERY_IDX_LINES				"IDXLines"
	#define TEXT_CONSTANT_PARAM_LEVEL						"Level"

	/* Parameter Values */
	#define TEXT_CONSTANT_PARAM_VAL_CLNT_XML_DATA_EMPTY		"empty"
	#define TEXT_CONSTANT_PARAM_VAL_CLNT_XML_DATA_NEWDOC	"newdoc"
	#define TEXT_CONSTANT_PARAM_VAL_CLNT_XML_DATA_NEWXML	"newxml"
	
	/* HTML Fragments */
	#define TEXT_CONSTANT_HTML_CLNTXML_EMPTY				"<frame name='Data' src='/WARoot/HTML/XMLIDMLib_Html_Empty.htm' marginheight='0' marginwidth='0'>"
	#define TEXT_CONSTANT_HTML_CLNTXML_NEWDOCCLASS			"<frame name='Data' src='%s?model=0&client=xml&data=newdoc' marginheight='0' marginwidth='0'>"
	#define TEXT_CONSTANT_HTML_CLNTXML_NEWXMLINDEX			"<frame name='Data' src='%s?model=0&client=xml&data=newxml' marginheight='0' marginwidth='0'>"

	#define TEXT_CONSTANT_HTML_XMLNAVI_XMLINDEX				"<tr><td nowrap class=\"treenode\">&nbsp;<img src=\"/WARoot/Images/xml_WANav11.gif\" border=0 valign=\"absmiddle\"><a href=\"%s?model=0&cmd=XmlIndexShow&XmlIndexID=%s&XmlIndexName=%s\" class=\"treenode\" target=\"Data\" title=\"%s\">&nbsp;%s </a></td></tr>"
	#define TEXT_CONSTANT_HTML_DOCNAVI_DOCCLASS				"<tr><td nowrap class=\"treenode\">&nbsp;<img src=\"/WARoot/Images/xml_WANav11.gif\" border=0 valign=\"absmiddle\"><a href=\"%s?model=0&cmd=DocClassShow&DocClassID=%s\" class=\"treenode\" target=\"Data\" title=\"%s\">&nbsp;%s </a></td></tr>"
	#define TEXT_CONSTANT_HTML_SESSIONPOOL					"<tr><td nowrap class=\"treenode\">&nbsp;<img src=\"/WARoot/Images/xml_WANav11.gif\" border=0 valign=\"bottom\"><a href=\"%s?model=0&cmd=SPShow&SPID=%s\" class=\"treenode\" target=\"Data\" title=\"%s\">&nbsp;%s </a></td></tr>"
	#define TEXT_CONSTANT_HTML_SERVICE						"<tr><td nowrap class=\"treenode\">&nbsp;<img src=\"/WARoot/Images/xml_WANav7.gif\" border=0 valign=\"bottom\"><a href=\"%s?model=0&cmd=ISShow&ISID=%s&init=1\" class=\"treenode\" target=\"Data\" title=\"%s\">&nbsp;%s </a></td></tr>"
	#define TEXT_CONSTANT_HTML_XIENAVI_XIENODE				"<tr><td nowrap class=\"treenode\">&nbsp;<img src=\"/WARoot/Images/xml_WANav11.gif\" border=0 valign=\"bottom\" title='%s'>&nbsp;%s</td></tr><tr><td nowrap class=\"treenode\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src=\"/WARoot/Images/xml_WANav7.gif\" border=0 valign=\"bottom\"><a href=\"%s?xie=0&cmd=XieShow&XIEID=%s&init=1\" class=\"treenode\" target=\"Data\" title=\"%s\">&nbsp;%s </a></td></tr>"
	#define TEXT_CONSTANT_HTML_XIENAVI_XIENAME				"<tr><td nowrap class=\"treenode\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src=\"/WARoot/Images/xml_WANav7.gif\" border=0 valign=\"bottom\"><a href=\"%s?xie=0&cmd=XieShow&XIEID=%s&init=1\" class=\"treenode\" target=\"Data\" title=\"%s\">&nbsp;%s </a></td></tr>"
	#define TEXT_CONSTANT_HTML_SESSIONPOOL_WEBDBM			"<a href=\"/webdbm?Server=%s&Database=%s\" target=\"top\" class=\"button\">&nbsp;WebDBM&nbsp;</a>"

	#define TEXT_CONSTANT_HTML_MSGTY_ERROR_GIF				"xml_msgerrosmall.gif"
	#define TEXT_CONSTANT_HTML_MSGTY_WARNING_GIF			"xml_msgwarnsmall.gif"
	#define TEXT_CONSTANT_HTML_MSGTY_INFO_GIF				"xml_msginfo.gif"
	#define TEXT_CONSTANT_HTML_MSGTY_REQUEST_GIF			"xml_msgquessmall.gif"
	#define TEXT_CONSTANT_HTML_MSGTY_OK_GIF					"xml_msgoksmall.gif"
	#define TEXT_CONSTANT_HTML_MSGTY_CLEAR_GIF				"1x1.gif"

	/* Mime types */
	#define TEXT_CONSTANT_MIME_TYPE_TEXT_HTML				"text/html"
	
	/* charset */
	#define TEXT_CONSTANT_CHARSET_UTF8						"UTF-8"
	#define TEXT_CONSTANT_CHARSET_ASCII						"iso-8859-1"
	
#endif