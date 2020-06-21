#ifndef WDVHANDLER_ADMINUTILS_H
#define WDVHANDLER_ADMINUTILS_H
/****************************************************************************

  module      : WDVHandler_AdminUtils.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-28  16:57
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Connection.h"

	/*------------------------------------------------------------------------
	Function	:	sendAdminPingReply
	Description	:	sends an empty page
	Parameters	:	[in] reply
	Return		:	
	--------------------------------------------------------------------------*/
	void sendAdminPingReply(sapdbwa_HttpReplyP   reply);

	/*------------------------------------------------------------------------
	Function	:	sendAdminStateReply
	Description	:	sends an xml page with state information
	Parameters	:	[in] reply
	Return		:	
	--------------------------------------------------------------------------*/
	void sendAdminStateReply(sapdbwa_Handle wa,sapdbwa_HttpRequestP request, sapdbwa_HttpReplyP reply, WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
	Function	:	sendAdminResetReply
	Description	:	sends an empty page 
	Parameters	:	[in] reply
	Return		:	
	--------------------------------------------------------------------------*/
	void sendAdminResetReply(sapdbwa_HttpReplyP reply, WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
	Function	:	createAdminState
	Description	:	creates the AdminState
	Parameters	:	[in] request
					[out] adminState
	Return		:	
	--------------------------------------------------------------------------*/
    WDVH_Bool createAdminState(sapdbwa_Handle wa, WDVH_AdminState *adminState); 

	/*------------------------------------------------------------------------
	Function	:	destroyAdminState
	Description	:	destroys the AdminState
	Parameters	:	[in] adminState
	Return		:	
	--------------------------------------------------------------------------*/
    WDVH_Bool destroyAdminState(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
	Function	:	resetAdminState
	Description	:	resets the AdminState
	Parameters	:	[in] adminState
	Return		:	
	--------------------------------------------------------------------------*/
    WDVH_Bool resetAdminState(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: setAdminStateName
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateName(WDVH_AdminState adminState, WDVH_Char *name);

	/*------------------------------------------------------------------------
		Function: setAdminStateLocation
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateLocation(WDVH_AdminState adminState, WDVH_Char *location, WDVH_Char *port);

	/*------------------------------------------------------------------------
		Function: setAdminStateRunnigSince
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateRunningSince(WDVH_AdminState adminState, WDVH_Char *running);

	/*------------------------------------------------------------------------
		Function: setAdminStateIdleStart
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateIdleStart(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: setAdminStateIdleStop
	--------------------------------------------------------------------------*/
    WDVH_Bool setAdminStateIdleStop(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: adminStateLogMethod
	--------------------------------------------------------------------------*/
    WDVH_Bool adminStateLogMethod(WDVH_AdminState adminState, WDVH_UInt1 method);

	/*------------------------------------------------------------------------
		Function: getAdminStateName
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateName(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: getAdminStateLocation
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateLocation(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: getAdminStateRunnigSince
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateRunningSince(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: getAdminStateRequestCount
	--------------------------------------------------------------------------*/
    WDVH_UInt4 getAdminStateRequestCount(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: getAdminStateVersion
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateVersion(WDVH_AdminState adminState); 

	/*------------------------------------------------------------------------
		Function: getAdminStateBuild
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateBuild(WDVH_AdminState adminState); 

	/*------------------------------------------------------------------------
		Function: getAdminStateWWWName
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateWWWName(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: getAdminStateWWWLocation
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateWWWLocation(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: getAdminStateWWWVersion
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateWWWVersion(WDVH_AdminState adminState); 

	/*------------------------------------------------------------------------
		Function: getAdminStateWWWBuild
	--------------------------------------------------------------------------*/
    WDVH_Char* getAdminStateWWWBuild(WDVH_AdminState adminState); 

	/*------------------------------------------------------------------------
		Function: getAdminStateMethodLog
	--------------------------------------------------------------------------*/
    WDVH_Bool getAdminStateMethodLog(WDVH_AdminState adminState,WDVH_UInt4 *methodpercent); 

	/*------------------------------------------------------------------------
		Function: getAdminStateRecentOverall
	--------------------------------------------------------------------------*/
    WDVH_Double getAdminStateRecentOverall(WDVH_AdminState adminState);

	/*------------------------------------------------------------------------
		Function: getAdminStateRecentBusy
	--------------------------------------------------------------------------*/
    WDVH_Double getAdminStateRecentBusy(WDVH_AdminState adminState);

#endif