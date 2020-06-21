#ifndef WDVHANDLER_CONNECTION_H
#define WDVHANDLER_CONNECTION_H

/****************************************************************************

  module      : WDVHandler_Connection.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-01  9:35
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonInclude.h"
	#include "sapdbwa_session.h" /*nocheck*/

	/*------------------------------------------------------------------------
	Function	:	getConnection
	Description	:	open an DB Connection via WebAgengt Session Pooling
	Parameters	:	[in] wa	-	WebAgent handler
	Return		:	conn
	--------------------------------------------------------------------------*/
	sapdbwa_DbConnectionP getConnection(sapdbwa_Handle wa);

	/*------------------------------------------------------------------------
	Function	:	closeConnection
	Description	:	close an WebAgent Session Pooling DB Connection
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	sapdbwa_Bool closeConnection(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);

	/*------------------------------------------------------------------------
	Function	:	getOdbcHandle
	Description	:	get the ODBC Handle for a given connection
	Parameters	:	[in] conn	-	connection
	Return		:	odbcHandle
	--------------------------------------------------------------------------*/
	SQLHDBC getOdbcHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);

	/*------------------------------------------------------------------------
	Function	:	getHenvHandle
	Description	:	get the Henv Handle for a given connection
	Parameters	:	[in] conn	-	connection
	Return		:	henvHandle
	--------------------------------------------------------------------------*/
	SQLHENV getHenvHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);

	/*------------------------------------------------------------------------
		Function:	getWdvHandle
	--------------------------------------------------------------------------*/
	WDVCAPI_WDV getWdvHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);

	/*------------------------------------------------------------------------
		Function:	getWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Handle getWdvhHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);

	/*------------------------------------------------------------------------
		Function:	getXmlHandle
	--------------------------------------------------------------------------*/
	XMLIMAPI_Handle getXmlHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);

	/*------------------------------------------------------------------------
		Function:	getQsHandle
	--------------------------------------------------------------------------*/
	XMLQSLib_Handle getQsHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);

	WDVH_Bool getCompressFlag(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn);
#endif