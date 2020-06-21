#ifndef WDVHANDLER_XMLFRAGMENTS_H
#define WDVHANDLER_XMLFRAGMENTS_H

/****************************************************************************

  module      : WDVHandler_XmlFragments.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-05  12:40
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

	#define	WDVH_XML_HEADER				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"

	#define WDVH_XML_OPEN_TAG			"<"
	#define WDVH_XML_CLOSE_TAG			">"
	#define WDVH_XML_EQUAL_SIGN			"="
	#define WDVH_XML_QUOTE				"\""
	#define WDVH_XML_SPACE				" "
	#define WDVH_XML_SLASH				"/"
	#define WDVH_XML_COLON				":"

	#define WDVH_HTTP_PREFIX			"http://"

	#define WDVH_MULTI_STATUS_OPEN		"<D:multistatus xmlns:D=\"DAV:\">"
	#define WDVH_MULTI_STATUS_CLOSE		"</D:multistatus>"

	#define WDVH_RESPONSE_OPEN			"<D:response"
	#define WDVH_RESPONSE_CLOSE			"</D:response>"

	#define WDVH_PROP_OPEN				"<D:prop>"
	#define WDVH_PROP_OPEN_XMLNS		"<D:prop xmlns:D=\"DAV:\">"
	#define WDVH_PROP_CLOSE				"</D:prop>"

	#define WDVH_PROPSTAT_OPEN			"<D:propstat>"
	#define WDVH_PROPSTAT_CLOSE			"</D:propstat>"

	#define WDVH_HREF_OPEN				"<D:href>"
	#define WDVH_HREF_CLOSE				"</D:href>"

	#define WDVH_STATUS_OPEN			"<D:status>"
	#define WDVH_STATUS_CLOSE			"</D:status>"
	#define WDVH_RESPONSEDESC_OPEN		"<D:responsedescription>"
	#define WDVH_RESPONSEDESC_CLOSE		"</D:responsedescription>"
	#define WDVH_RESPONSEDESC_424		"Attempted operation could not be completed due to other errors."
	#define WDVH_STATUS_OK				"HTTP/1.1 200 OK"
	#define WDVH_STATUS_409				"HTTP/1.1 409 Conflict"
	#define WDVH_STATUS_424				"HTTP/1.1 424 Failed Dependency"
	#define WDVH_STATUS_500				"HTTP/1.1 500 Internal Server Error"
	#define WDVH_STATUS_FAILED			"HTTP/1.1 404 Not Found"
	#define WDVH_HTTP_1_1				"HTTP/1.1 "

	#define WDVH_XMLNS_DEFAULT			" xmlns="
	#define WDVH_XMLNS					" xmlns:"
	#define WDVH_XML_NS_PREFIX			"s"

	#define WDVH_DEFAULT_NS				"D:"
	
	#define WDVH_XML_RESOURCETYPE		"resourcetype"
	#define WDVH_XML_LOCKDISCOVERY      "lockdiscovery"
	#define WDVH_XML_SUPPORTEDLOCK      "supportedlock"
    #define WDVH_XML_GETLASTMODIFIED    "getlastmodified"
    #define WDVH_XML_GETLASTMODIFIED_ATTR    "dt:dt='dateTime.rfc1123'"
    #define WDVH_XML_CREATIONDATE       "creationdate"
    #define WDVH_XML_CREATIONDATE_ATTR    "dt:dt='dateTime.rfc1123'"
	/*------------------------------------------------------------------------
		Live Properties
	--------------------------------------------------------------------------*/
	#define WDVH_PROP_CONTENT_TYPE			"getcontenttype"

	/*------------------------------------------------------------------------
		LOCKING
	--------------------------------------------------------------------------*/
	#define WDVH_LOCKDISCOVERY_OPEN			"<D:lockdiscovery>"
	#define WDVH_LOCKDISCOVERY_CLOSE		"</D:lockdiscovery>"
	#define WDVH_LOCKENTRY_OPEN				"<D:lockentry>"
	#define WDVH_LOCKENTRY_CLOSE			"</D:lockentry>"
	#define WDVH_LOCKSCOPE_OPEN				"<D:lockscope>"
	#define WDVH_LOCKSCOPE_CLOSE			"</D:lockscope>"
	#define WDVH_LOCKTYPE_OPEN				"<D:locktype>"
	#define WDVH_LOCKTYPE_CLOSE				"</D:locktype>"
	#define WDVH_OWNER_OPEN					"<D:owner>"
	#define WDVH_OWNER_CLOSE				"</D:owner>"
	#define WDVH_LOCKDISCOVERY_EMPTY		"<D:lockdiscovery/>"
	#define WDVH_EXCLUSIVE_EMPTY			"<D:exclusive/>"
	#define WDVH_SHARED_EMPTY				"<D:shared/>"
	#define WDVH_WRITE_EMPTY				"<D:write/>"
	#define WDVH_ACTIVELOCK_OPEN			"<D:activelock>"
	#define WDVH_ACTIVELOCK_CLOSE			"</D:activelock>"
	#define WDVH_TIMEOUT_OPEN				"<D:timeout>"
	#define WDVH_TIMEOUT_CLOSE				"</D:timeout>"
	#define WDVH_LOCKTOKEN_OPEN				"<D:locktoken>"
	#define WDVH_LOCKTOKEN_CLOSE			"</D:locktoken>"
	#define WDVH_DEPTH_OPEN					"<D:depth>"
	#define WDVH_DEPTH_CLOSE				"</D:depth>"
	#define WDVH_DEPTH_0					"0"
	#define WDVH_DEPTH_INFINITY				"infinity"

	/*------------------------------------------------------------------------
		ADMINISTRATION
	--------------------------------------------------------------------------*/
	#define WDVH_XML_ADMIN_STATE_OPEN						"<ADM:state>"
	#define WDVH_XML_ADMIN_STATE_CLOSE						"</ADM:state>"
	#define WDVH_XML_ADMIN_SERVER_OPEN						"<ADM:server>"
	#define WDVH_XML_ADMIN_SERVER_CLOSE						"</ADM:server>"
	#define WDVH_XML_ADMIN_WEBSERVER_OPEN					"<ADM:webserver>"
	#define WDVH_XML_ADMIN_WEBSERVER_CLOSE					"</ADM:webserver>"
	#define WDVH_XML_ADMIN_WEBDAVSERVER_OPEN				"<ADM:webdavserver>"
	#define WDVH_XML_ADMIN_WEBDAVSERVER_CLOSE				"</ADM:webdavserver>"
	#define WDVH_XML_ADMIN_SERVER_VERSION_OPEN				"<ADM:version>"
	#define WDVH_XML_ADMIN_SERVER_VERSION_CLOSE				"</ADM:version>"
	#define WDVH_XML_ADMIN_SERVER_BUILD_OPEN				"<ADM:build>"
	#define WDVH_XML_ADMIN_SERVER_BUILD_CLOSE				"</ADM:build>"
	#define WDVH_XML_ADMIN_SERVER_NAME_OPEN					"<ADM:name>"
	#define WDVH_XML_ADMIN_SERVER_NAME_CLOSE				"</ADM:name>"
	#define WDVH_XML_ADMIN_SERVER_LOCATION_OPEN				"<ADM:location>"
	#define WDVH_XML_ADMIN_SERVER_LOCATION_CLOSE			"</ADM:location>"
	#define WDVH_XML_ADMIN_STATISTICS_OPEN					"<ADM:statistics>"
	#define WDVH_XML_ADMIN_STATISTICS_CLOSE					"</ADM:statistics>"
	#define WDVH_XML_ADMIN_STATISTICS_RUNNING_OPEN			"<ADM:running>"
	#define WDVH_XML_ADMIN_STATISTICS_RUNNING_CLOSE			"</ADM:running>"
	#define WDVH_XML_ADMIN_STATISTICS_REQCOUNT_OPEN			"<ADM:requestcount>"
	#define WDVH_XML_ADMIN_STATISTICS_REQCOUNT_CLOSE		"</ADM:requestcount>"
	#define WDVH_XML_ADMIN_STATISTICS_REQPERSEC_OPEN		"<ADM:requestpersec>"
	#define WDVH_XML_ADMIN_STATISTICS_REQPERSEC_CLOSE		"</ADM:requestpersec>"
	#define WDVH_XML_ADMIN_STATISTICS_REQPERSECBUSY_OPEN	"<ADM:requestpersecbusy>"
	#define WDVH_XML_ADMIN_STATISTICS_REQPERSECBUSY_CLOSE	"</ADM:requestpersecbusy>"
	#define WDVH_XML_ADMIN_STATISTICS_IDLE_OPEN				"<ADM:idle>"
	#define WDVH_XML_ADMIN_STATISTICS_IDLE_CLOSE			"</ADM:idle>"
	#define WDVH_XML_ADMIN_STATISTICS_BUSY_OPEN				"<ADM:busy>"
	#define WDVH_XML_ADMIN_STATISTICS_BUSY_CLOSE			"</ADM:busy>"
	#define WDVH_XML_ADMIN_STATISTICS_RECENT_IDLE_OPEN		"<ADM:recentidle>"
	#define WDVH_XML_ADMIN_STATISTICS_RECENT_IDLE_CLOSE		"</ADM:recentidle>"
	#define WDVH_XML_ADMIN_STATISTICS_RECENT_BUSY_OPEN		"<ADM:recentbusy>"
	#define WDVH_XML_ADMIN_STATISTICS_RECENT_BUSY_CLOSE		"</ADM:recentbusy>"
	#define WDVH_XML_ADMIN_STATISTICS_METHODLOG_OPEN		"<ADM:methodlog>"
	#define WDVH_XML_ADMIN_STATISTICS_METHODLOG_CLOSE		"</ADM:methodlog>"

	#define WDVH_XML_ADMIN_STATISTICS_CONTENTLEN_OPEN		"<ADM:logicalcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_CONTENTLEN_CLOSE		"</ADM:logicalcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_COMPCONTENTLEN_OPEN	"<ADM:compressedcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_COMPCONTENTLEN_CLOSE	"</ADM:compressedcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_PHYCONTENTLEN_OPEN	"<ADM:physicalcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_PHYCONTENTLEN_CLOSE	"</ADM:physicalcontentlength>"

	#define WDVH_XML_ADMIN_STATISTICS_LOGCOMPCONTENTLEN_OPEN	"<ADM:logcompcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_LOGCOMPCONTENTLEN_CLOSE	"</ADM:logcompcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_PHYCOMPCONTENTLEN_OPEN	"<ADM:phycompcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_PHYCOMPCONTENTLEN_CLOSE	"</ADM:phycompcontentlength>"
	#define WDVH_XML_ADMIN_STATISTICS_COMPRESSIONGRADE_OPEN		"<ADM:compressiongrade>"
	#define WDVH_XML_ADMIN_STATISTICS_COMPRESSIONGRADE_CLOSE	"</ADM:compressiongrade>"

	#define WDVH_XML_ADMIN_STATISTICS_RESOURCES_OPEN		"<ADM:resources>"
	#define WDVH_XML_ADMIN_STATISTICS_RESOURCES_CLOSE		"</ADM:resources>"

	#define WDVH_XML_ADMIN_STATISTICS_COLLECTIONS_OPEN		"<ADM:collections>"
	#define WDVH_XML_ADMIN_STATISTICS_COLLECTIONS_CLOSE		"</ADM:collections>"
	#define WDVH_XML_ADMIN_METHODLOG_TOTAL_LOGGED_OPEN		"<ADM:totallogged>"
	#define WDVH_XML_ADMIN_METHODLOG_TOTAL_LOGGED_CLOSE		"</ADM:totallogged>"
	#define WDVH_XML_ADMIN_METHODLOG_GET_OPEN				"<ADM:get>"
	#define WDVH_XML_ADMIN_METHODLOG_GET_CLOSE				"</ADM:get>"
	#define WDVH_XML_ADMIN_METHODLOG_HEAD_OPEN				"<ADM:head>"
	#define WDVH_XML_ADMIN_METHODLOG_HEAD_CLOSE				"</ADM:head>"
	#define WDVH_XML_ADMIN_METHODLOG_PUT_OPEN				"<ADM:put>"
	#define WDVH_XML_ADMIN_METHODLOG_PUT_CLOSE				"</ADM:put>"
	#define WDVH_XML_ADMIN_METHODLOG_POST_OPEN				"<ADM:post>"
	#define WDVH_XML_ADMIN_METHODLOG_POST_CLOSE				"</ADM:post>"
	#define WDVH_XML_ADMIN_METHODLOG_DELETE_OPEN			"<ADM:delete>"
	#define WDVH_XML_ADMIN_METHODLOG_DELETE_CLOSE			"</ADM:delete>"
	#define WDVH_XML_ADMIN_METHODLOG_COPY_OPEN				"<ADM:copy>"
	#define WDVH_XML_ADMIN_METHODLOG_COPY_CLOSE				"</ADM:copy>"
	#define WDVH_XML_ADMIN_METHODLOG_MOVE_OPEN				"<ADM:move>"
	#define WDVH_XML_ADMIN_METHODLOG_MOVE_CLOSE				"</ADM:move>"
	#define WDVH_XML_ADMIN_METHODLOG_PROPFIND_OPEN			"<ADM:propfind>"
	#define WDVH_XML_ADMIN_METHODLOG_PROPFIND_CLOSE			"</ADM:propfind>"
	#define WDVH_XML_ADMIN_METHODLOG_PROPPATCH_OPEN			"<ADM:proppatch>"
	#define WDVH_XML_ADMIN_METHODLOG_PROPPATCH_CLOSE		"</ADM:proppatch>"
	#define WDVH_XML_ADMIN_METHODLOG_MKCOL_OPEN				"<ADM:mkcol>"
	#define WDVH_XML_ADMIN_METHODLOG_MKCOL_CLOSE			"</ADM:mkcol>"
	#define WDVH_XML_ADMIN_METHODLOG_LOCK_OPEN				"<ADM:lock>"
	#define WDVH_XML_ADMIN_METHODLOG_LOCK_CLOSE				"</ADM:lock>"
	#define WDVH_XML_ADMIN_METHODLOG_UNLOCK_OPEN			"<ADM:unlock>"
	#define WDVH_XML_ADMIN_METHODLOG_UNLOCK_CLOSE			"</ADM:unlock>"
	#define WDVH_XML_ADMIN_METHODLOG_OPTIONS_OPEN			"<ADM:options>"
	#define WDVH_XML_ADMIN_METHODLOG_OPTIONS_CLOSE			"</ADM:options>"
	#define WDVH_XML_ADMIN_METHODLOG_TRACE_OPEN				"<ADM:trace>"
	#define WDVH_XML_ADMIN_METHODLOG_TRACE_CLOSE			"</ADM:trace>"
	#define WDVH_XML_ADMIN_METHODLOG_CONNECT_OPEN			"<ADM:connect>"
	#define WDVH_XML_ADMIN_METHODLOG_CONNECT_CLOSE			"</ADM:connect>"
	#define WDVH_XML_ADMIN_METHODLOG_PING_OPEN				"<ADM:ping>"
	#define WDVH_XML_ADMIN_METHODLOG_PING_CLOSE				"</ADM:ping>"
	#define WDVH_XML_ADMIN_METHODLOG_STATE_OPEN				"<ADM:state>"
	#define WDVH_XML_ADMIN_METHODLOG_STATE_CLOSE			"</ADM:state>"
	#define WDVH_XML_ADMIN_METHODLOG_RESET_OPEN				"<ADM:reset>"
	#define WDVH_XML_ADMIN_METHODLOG_RESET_CLOSE			"</ADM:reset>"
	#define WDVH_XML_ADMIN_STATE_NAMESPACE				" xmlns:D=\"DAV:\" xmlns:ADM=\"http://www.sapdb.org/webdav\" "

	#define	WDVH_XML_HEADER_LEN			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"

	#define WDVH_XML_OPEN_TAG_LEN		2
	#define WDVH_XML_CLOSE_TAG_LEN		2
	#define WDVH_XML_EQUAL_SIGN_LEN		2
	#define WDVH_XML_QUOTE_LEN			2
	#define WDVH_XML_SPACE_LEN			2
	#define WDVH_XML_SLASH_LEN			2
	#define WDVH_XML_COLON_LEN			2

	#define WDVH_MULTI_STATUS_OPEN_LEN	31
	#define WDVH_MULTI_STATUS_CLOSE_LEN	17

	#define WDVH_RESPONSE_OPEN_LEN		12
	#define WDVH_RESPONSE_CLOSE_LEN		14

	#define WDVH_PROP_OPEN_LEN			9
	#define WDVH_PROP_CLOSE_LEN			10

	#define WDVH_PROPSTAT_OPEN_LEN		13
	#define WDVH_PROPSTAT_CLOSE_LEN		14

	#define WDVH_HREF_OPEN_LEN			17
	#define WDVH_HREF_CLOSE_LEN			10

	#define WDVH_STATUS_OPEN_LEN		11
	#define WDVH_STATUS_CLOSE_LEN		12
	#define WDVH_STATUS_OK_LEN			16
	#define WDVH_STATUS_FAILED_LEN		23

	#define WDVH_XMLNS_DEFAULT_LEN		7
	#define WDVH_XMLNS_LEN				7
	#define WDVH_XML_NS_PREFIX_LEN		2

	#define WDVH_DEFAULT_NS_LEN			3
	
	/*------------------------------------------------------------------------
		PROPFIND XML TAGS
	--------------------------------------------------------------------------*/
	#define WDVH_XML_PROPFIND_OPEN		"propfind"
	#define WDVH_XML_PROPFIND_CLOSE		"propfind"
	#define WDVH_XML_ALLPROP			"allprop"
	#define WDVH_XML_PROPNAME			"propname"
	#define WDVH_XML_PROP_OPEN			"prop"
	#define WDVH_XML_PROP_CLOSE			"prop"
	#define WDVH_XML_ALLPROP_BODY		"<?xml version=\"1.0\" encoding=\"utf-8\" ?> \
										 <D:propfind xmlns:D=\"DAV:\"> \
											 <D:allprop/> \
										 </D:propfind>"


	/*------------------------------------------------------------------------
		PROPFIND EMPTYTAGS
	--------------------------------------------------------------------------*/
	#define WDVH_EMPTY_TAG_ALLPROP		"allprop/>"
	#define	WDVH_EMPTY_TAG_PROPNAME		"propname/>"

	/*------------------------------------------------------------------------
		PROPPATCH XML TAGS
	--------------------------------------------------------------------------*/
	#define WDVH_XML_PROPERTYUPDATE_OPEN		"propertyupdate"
	#define WDVH_XML_PROPERTYUPDATE_CLOSE		"propertyupdate"
	#define WDVH_XML_SET_OPEN					"set"
	#define WDVH_XML_SET_CLOSE					"set"
	#define WDVH_XML_REMOVE_OPEN				"remove"
	#define WDVH_XML_REMOVE_CLOSE				"remove"

	/*------------------------------------------------------------------------
		LOCK XML TAGS
	--------------------------------------------------------------------------*/
	#define WDVH_XML_LOCKDISCOVERY_OPEN			"lockdiscovery"
	#define WDVH_XML_LOCKDISCOVERY_CLOSE		"lockdiscovery"
	#define WDVH_XML_LOCKINFO_OPEN				"lockinfo"
	#define WDVH_XML_LOCKINFO_CLOSE				"lockinfo"
	#define WDVH_XML_LOCKSCOPE_OPEN				"lockscope"
	#define WDVH_XML_LOCKSCOPE_CLOSE			"lockscope"
	#define WDVH_XML_LOCKTYPE_OPEN				"locktype"
	#define WDVH_XML_LOCKTYPE_CLOSE				"locktype"
	#define WDVH_XML_OWNER_OPEN					"owner"
	#define WDVH_XML_OWNER_CLOSE				"owner"
	#define WDVH_XML_EXCLUSIVE_OPEN				"exclusive"
	#define WDVH_XML_EXCLUSIVE_CLOSE			"exclusive"
	#define WDVH_XML_SHARED_OPEN				"shared"
	#define WDVH_XML_SHARED_CLOSE				"shared"
	#define WDVH_XML_WRITE_OPEN					"write"
	#define WDVH_XML_WRITE_CLOSE				"write"
	#define WDVH_XML_ACTIVELOCK_OPEN			"activelock"
	#define WDVH_XML_ACTIVELOCK_CLOSE			"activelock"
	#define WDVH_XML_TIMEOUT_OPEN				"timeout"
	#define WDVH_XML_TIMEOUT_CLOSE				"timeout"
	#define WDVH_XML_LOCKTOKEN_OPEN				"locktoken"
	#define WDVH_XML_LOCKTOKEN_CLOSE			"locktoken"

	/*------------------------------------------------------------------------
		LOCK EMPTYTAGS
	--------------------------------------------------------------------------*/
	#define WDVH_EMPTY_TAG_EXCLUSIVE			"exclusive/>"
	#define	WDVH_EMPTY_TAG_SHARED				"shared/>"
	#define	WDVH_EMPTY_TAG_WRITE				"write/>"

	/*------------------------------------------------------------------------
		WEBDAV 1.0 DTD
	--------------------------------------------------------------------------*/
	#define WDVH_XML_WEBDAV10_DTD		"<!DOCTYPE webdav-1.0 [ \
			 <!--============ XML Elements from Section 12 ==================--> \
			   <!ELEMENT activelock (lockscope, locktype, depth, owner?, timeout?, \
				locktoken?) > \
				\
			   <!ELEMENT lockentry (lockscope, locktype) > \
			   <!ELEMENT lockinfo (lockscope, locktype, owner?) > \
				\
			   <!ELEMENT locktype (write) > \
			   <!ELEMENT write EMPTY > \
				\
			   <!ELEMENT lockscope (exclusive | shared) > \
			   <!ELEMENT exclusive EMPTY > \
			   <!ELEMENT shared EMPTY > \
				\
			   <!ELEMENT depth (#PCDATA) > \
				\
			   <!ELEMENT owner ANY > \
				\
			   <!ELEMENT timeout (#PCDATA) > \
				\
			   <!ELEMENT locktoken (href+) > \
				\
			   <!ELEMENT href (#PCDATA) > \
				\
			   <!ELEMENT link (src+, dst+) > \
			   <!ELEMENT dst (#PCDATA) > \
			   <!ELEMENT src (#PCDATA) > \
				\
			   <!ELEMENT multistatus (response+, responsedescription?) > \
				\
			   <!ELEMENT response (href, ((href*, status)|(propstat+)), \
			    responsedescription?) > \
			   <!ELEMENT status (#PCDATA) > \
			   <!ELEMENT propstat (prop, status, responsedescription?) > \
			   <!ELEMENT responsedescription (#PCDATA) > \
				\
			   <!ELEMENT prop ANY > \
				\
			   <!ELEMENT propertybehavior (omit | keepalive) > \
			   <!ELEMENT omit EMPTY > \
				\
			   <!ELEMENT keepalive (#PCDATA | href+) > \
				\
			   <!ELEMENT propertyupdate (remove | set)+ > \
			   <!ELEMENT remove (prop) > \
			   <!ELEMENT set (prop) > \
				\
			   <!ELEMENT propfind (allprop | propname | prop) > \
			   <!ELEMENT allprop EMPTY > \
			   <!ELEMENT propname EMPTY > \
				\
			   <!ELEMENT collection EMPTY > \
				\
			   <!--=========== Property Elements from Section 13 ===============--> \
			   <!ELEMENT creationdate (#PCDATA) > \
			   <!ELEMENT displayname (#PCDATA) > \
			   <!ELEMENT getcontentlanguage (#PCDATA) > \
			   <!ELEMENT getcontentlength (#PCDATA) > \
			   <!ELEMENT getcontenttype (#PCDATA) > \
			   <!ELEMENT getetag (#PCDATA) > \
			   <!ELEMENT getlastmodified (#PCDATA) > \
			   <!ELEMENT lockdiscovery (activelock)* > \
			   <!ELEMENT resourcetype ANY > \
			   <!ELEMENT source (link)* > \
			   <!ELEMENT supportedlock (lockentry)* > \
			   ]>"
	/*----------------------------------------------------------------
	/ Embedded HTML GUI 
	/-----------------------------------------------------------------*/
	#define WDVH_HTML_EMBEDDED_GUI				"gui"
	#define WDVH_HTML_EMBEDDED_GUI_NAVI			"gui=0&navigation=1"
	#define WDVH_HTML_EMBEDDED_GUI_NAVI_TAB1	"gui=0&navigation=1&Tab=1"
	#define WDVH_HTML_EMBEDDED_GUI_NAVI_TAB2	"gui=0&navigation=1&Tab=2"
	#define WDVH_HTML_EMBEDDED_GUI_NAVI_TAB3	"gui=0&navigation=1&Tab=3"
	#define WDVH_HTML_EMBEDDED_GUI_NAVI_TAB4	"gui=0&navigation=1&Tab=4"
	#define WDVH_HTML_EMBEDDED_MODELLING		"model"
	#define WDVH_HTML_EMBEDDED_XIEADMIN			"xie"
	#define WDVH_HTML_EMBEDDED_QUERY			"query"
	#define WDVH_HTML_EMBEDDED_EXPLORER			"explorer"
	
	/* Embedded Html GUI start page */
	#define WDVH_HTML_EMBEDDED_START_PAGE_PART_01 \
			"<html><head><link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
 			 <title>SAP DB WebDAV XML Indexing Administration and Modelling - ("
 			 
 	#define WDVH_HTML_EMBEDDED_START_PAGE_PART_02 \
 			")</title></head><body topmargin='0' leftmargin='0' marginwidth='0' marginheight='0' bgcolor='#DEDEC8'>\
			<table border='0' cellspacing='0' cellpadding='0' width='100%'>\
		   <tr><td bgcolor='#102973'><img src='/WARoot/Images/1x1.gif' width='10'></td>\
		   <td width='100%'>\
      		<table border='0' cellspacing='0' cellpadding='0' width='100%'>\
	        <tr><td bgcolor='#102973' height='31' width='100%'><img src='/WARoot/Images/1x1.gif'></td>\
	        </tr></table></td><td rowspan='6'><img src='/WARoot/Images/sapdblogo.gif' height='64' width='356'></td></tr>\
		    <tr><td colspan='2' bgcolor='#102973'><img src='/WARoot/Images/1x1.gif' height='2'></td></tr>\
  			<tr><td colspan='2' bgcolor='#687AA7'><img src='/WARoot/Images/1x1.gif' height='1'></td></tr>\
  			<tr><td colspan='2' bgcolor='#E6D2E7'><img src='/WARoot/Images/1x1.gif' height='1'></td></tr>\
  			<tr><td bgcolor='#FFFFFF'><img src='/WARoot/Images/1x1.gif' width='10'></td>\
    		<td class='appdoctitle' nowrap bgcolor='#FFFFFF' height='28'><i><b>WebDAV Administration and Indexing - ("
    		
    #define WDVH_HTML_EMBEDDED_START_PAGE_PART_03 \
    		")</b></i></td></tr>\
  			 <tr><td colspan='2' bgcolor='#000000'><img src='/WARoot/Images/1x1.gif' height='1'></td></tr>\
			 </table>\
			 <table border='0' cellspacing='0' cellpadding='0' width='100%'>\
			 <tr><td><img src='/WARoot/Images/1x1.gif' height='100'></td></tr></table>\
			 <table border='0' cellspacing='0' cellpadding='0' width='100%'>\
 			 <tr><td><img src='/WARoot/Images/1x1.gif' width='50' height='1'></td>\
   			 <td class='text'><h1>Welcome to SAP DB WebDAV XML Indexing Administration and Modelling</h1><br><br>\
   			 This tool provides you with facilities to administrate your WebDAV XML Indexing environment.<br>\
   			 For administration of your XML Index Engines please follow this link:<br><br>\
   			 <a href='"
   			 
   	#define WDVH_HTML_EMBEDDED_START_PAGE_PART_04 \
   			"?xie=0&start=1'><h3>XML Index Engine Administration</h3></a><br><br><br>\
             For definition of your DocumentClasses and your XML Indexes please follow this link:<br><br>\
   			 <a href='"
   			  
   	#define WDVH_HTML_EMBEDDED_START_PAGE_PART_05 \
   			"?model=0&start=1'><h3>XML Modelling</h3></a><br><br><br>\
   			 For querying your indexed Documents please follow this link:<br><br>\
   			 <a href='"
   			 
   	#define WDVH_HTML_EMBEDDED_START_PAGE_PART_06 \
   			"?query=0&start=1'><h3>XML Document Query</h3></a><br><br><br>\
			 For browsing through the WebDAV Filesystem please follow this link:<br><br>\
   			 <a href='"

   	#define WDVH_HTML_EMBEDDED_START_PAGE_PART_07 \
   			"'><h3>WebDAV Document Root</h3></a><br><br><br></tr></table></body>"

	#define WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_01 \
			"<html><head><link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'><title>SAP DB WebDAV XML Indexing (Service "
	
	#define WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_02 \
			")</title></head><frameset rows='40,42,*,32' framespacing='0' frameborder='0' border='0'><frame name='Header' src='/WARoot/HTML/WDVHandler_Html_Header.htm' marginheight='0' marginwidth='0' scrolling='no' noresize><frame name='Navigation' src='"

	#define	WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_03 \
			"?gui=0&navigation=1&Tab=4' marginheight='0' marginwidth='0' scrolling='no' noresize> \
			 <frame name='Client' src='"

	#define WDVH_HTML_EMBEDDED_INITIAL_FRAME_PART_04 \
			"' marginheight='0' marginwidth='0'> \
			 <frame name='Message' src='/WARoot/HTML/WDVHandler_Html_Message.htm' marginheight='0' marginwidth='0' scrolling='no' noresize>\
			<noframes><body><p>Please use a browser supporting frames !</p></body></noframes></frameset></html>"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_01 \
			"<head><link rel='stylesheet' href='/WARoot/Styles/WDVHandler_Stylesheet.css' type='text/css'>\
			 <script language='JavaScript'>\
				function go(tab) {\
					if (tab==1) {\
						top.frames['Client'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_02 \
			"?xie=0&client=xie';top.frames['Navigation'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_03 \
			"?gui=0&navigation=1&Tab=1';top.frames['Message'].location.href='/WARoot/HTML/XMLIDMLib_Html_MessageEmpty.htm';"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_04 \
			" } else if (tab==2) {\
				top.frames['Client'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_05 \
			"?model=0&client=xis';\
			 top.frames['Navigation'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_06 \
			"?gui=0&navigation=1&Tab=2';\
			 top.frames['Message'].location.href='/WARoot/HTML/XMLIDMLib_Html_MessageEmpty.htm';"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_07 \
			" } else if (tab==3) {\
				top.frames['Client'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_08 \
			"?query=0&client=query';top.frames['Navigation'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_09 \
			"?gui=0&navigation=1&Tab=3';top.frames['Message'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_10 \
			"?query=0&message=1';\
			 } else if (tab==4) {\
				top.frames['Client'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_11 \
			"';top.frames['Navigation'].location.href='"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_12 \
			"?gui=0&navigation=1&Tab=4';top.frames['Message'].location.href='/WARoot/HTML/WDVHandler_Html_Message.htm';"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_13 \
			" }}</script></head>\
			<body topmargin='0' leftmargin='0' onload=''>\
				<form action='' method='post' name='navi'>\
					<!-- Separator -->\
					<table border='0' cellpadding='0' cellspacing='0'>\
						<tr>\
							<td><img src='/WARoot/Images/1x1.gif' height='8' width='0'></td>\
						</tr>\
						<tr>\
							<td></td>\
							<td>\
								<table border='0' cellpadding='0' cellspacing='0'>\
									<tr>"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_14 \
			"<td ><img src='/WARoot/Images/xml_tabfron.gif' height=22></td>\
			 <td  height=22 background='/WARoot/Images/xml_tabon.gif' class='text' nowrap><a href='javascript:go(4)'  class='linkhoverta'>&nbsp;WebDAV Explorer&nbsp;</a></td>\
			 <td ><img src='/WARoot/Images/xml_tabonoff.gif' height=22></td>\
			 <td  height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(3)'  class='linkhoverti'>&nbsp;Document Retrieval&nbsp;</a></td>\
			 <td ><img src='/WARoot/Images/xml_taboffoff.gif' height=22></td>\
			 <td  height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(1)'  class='linkhoverti'>&nbsp;Index Engine Monitoring&nbsp;</a></td>\
			 <td ><img src='/WARoot/Images/xml_taboffoff.gif' height=22></td>\
			 <td  height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(2)'  class='linkhoverti'>&nbsp;Indexing Service Settings&nbsp;</a></td>\
			 <td ><img src='/WARoot/Images/xml_tabbkoff.gif' height=22></td>"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_15 \
			"<td rowspan='1'><img src='/WARoot/Images/xml_tabfroff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(4)'  class='linkhoverti'>&nbsp;WebDAV Explorer&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_taboffon.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_tabon.gif' class='text' nowrap><a href='javascript:go(3)'  class='linkhoverta'>&nbsp;Document Retrieval&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_tabonoff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(1)'  class='linkhoverti'>&nbsp;Index Engine Monitoring&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_taboffoff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(2)'  class='linkhoverti'>&nbsp;Indexing Service Settings&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_tabbkoff.gif' height=22></td>"			

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_16 \
			"<td rowspan='1'><img src='/WARoot/Images/xml_tabfroff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(4)'  class='linkhoverti'>&nbsp;WebDAV Explorer&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_taboffoff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(3)'  class='linkhoverti'>&nbsp;Document Retrieval&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_taboffon.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_tabon.gif' class='text' nowrap><a href='javascript:go(1)'  class='linkhoverta'>&nbsp;Index Engine Monitoring&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_tabonoff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(2)'  class='linkhoverti'>&nbsp;Indexing Service Settings&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_tabbkoff.gif' height=22></td>"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_17 \
			"<td rowspan='1'><img src='/WARoot/Images/xml_tabfroff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(4)'  class='linkhoverti'>&nbsp;WebDAV Explorer&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_taboffoff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(3)'  class='linkhoverti'>&nbsp;Document Retrieval&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_taboffoff.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_taboff.gif' class='text' nowrap><a href='javascript:go(1)'  class='linkhoverti'>&nbsp;Index Engine Monitoring&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_taboffon.gif' height=22></td>\
			<td rowspan='1' height=22 background='/WARoot/Images/xml_tabon.gif' class='text' nowrap><a href='javascript:go(2)'  class='linkhoverta'>&nbsp;Indexing Service Settings&nbsp;</a></td>\
			<td rowspan='1'><img src='/WARoot/Images/xml_tabbkon.gif' height=22></td>"

	#define WDVH_HTML_EMBEDDED_NAVI_PAGE_PART_18 \
			"</tr></table></td></tr></table>\
			 <table border='0' cellpadding='0' cellspacing='0'>\
			 <tr><td colspan=8 width='2000' height=2 bgcolor='#225A8D'  align='bottom'><img src='/WARoot/Images/1x1.gif' height='2'></td></tr>\
			 <tr><td bgcolor='#FFFFFF'>&nbsp;</td></tr>\
			 </table></form></body>"

#endif