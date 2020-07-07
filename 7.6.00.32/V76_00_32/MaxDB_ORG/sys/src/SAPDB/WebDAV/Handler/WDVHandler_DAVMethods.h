#ifndef WDVHandler_METHODS_H
#define WDVHandler_METHODS_H

/****************************************************************************

  module      : WDVHandler_Methods.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-26  13:22
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

	/* WebDAV Core */
	#define WEBDAV_GET			"GET"
	#define WEBDAV_HEAD			"HEAD"
	#define WEBDAV_PUT			"PUT"
	#define WEBDAV_DELETE		"DELETE"
	#define WEBDAV_POST			"POST"
	#define WEBDAV_PROPFIND		"PROPFIND"
	#define WEBDAV_PROPPATCH	"PROPPATCH"
	#define WEBDAV_COPY			"COPY"
	#define WEBDAV_MOVE			"MOVE"
	#define WEBDAV_MKCOL		"MKCOL"
	#define WEBDAV_OPTIONS		"OPTIONS"
	#define WEBDAV_TRACE		"TRACE"
	#define WEBDAV_CONNECT		"CONNECT"

	/* Locking */
	#define WEBDAV_LOCK			"LOCK"
	#define WEBDAV_UNLOCK		"UNLOCK"

	/* Versioning Core Package */
	#define WEBDAV_VERSION_CONTROL	"VERSION-CONTROL"
	#define WEBDAV_CHECKOUT			"CHECKOUT"
	#define WEBDAV_CHECKIN			"CHECKIN"
	#define WEBDAV_UNCHECKOUT		"UNCHECKOUT"
	#define WEBDAV_REPORT			"REPORT"

	/* XMLDB Extension */
	#define WEBDAV_XQUERY			"XQUERY"
#endif