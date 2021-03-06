#ifndef WDVHandler_DAVVersioningHandler_H
#define WDVHandler_DAVVersioningHandler_H

/****************************************************************************

  module      : WDVHandler_DAVVersioningHandler.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  10:40
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


*****************************************************************************

  This header contains all the WebDAV Method Handlers that are currently 
  supported

*****************************************************************************/

	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"

	#include "SAPDB/WebDAV/Handler/WDVHandler_VersionControlHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_CheckoutHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_CheckinHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_UncheckoutHandler.h"



	/*------------------------------------------------------------------------
		Function	: webdavVersionControlHandler
		Description	: Handler for Version-Control Requests 
		Parameters	:
		Return		:
	--------------------------------------------------------------------------*/
	void webdavVersionControlHandler(sapdbwa_Handle		  wa,
									 sapdbwa_HttpRequestP request,
									 sapdbwa_HttpReplyP   reply );

#endif