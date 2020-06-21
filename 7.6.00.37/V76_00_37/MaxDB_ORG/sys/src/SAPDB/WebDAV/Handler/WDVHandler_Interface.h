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
#ifndef WDVHandler_INTERFACE_H
#define WDVHandler_INTERFACE_H

/*!**********************************************************************

  module: PTSWeb_WAInterface.hpp

  -----------------------------------------------------------------------

  responsible:  Daniel Kirmse (D027470 - Daniel.Kirmse@SAP.com)

  special area: WDVHandler

  description:  functions for WDVHandler

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_AdminUtils.h"

	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethodHandler.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVLockingHandler.h"

	/*!-------------------------------------------------------------------------
		function:     WDVHandlerInit
		description:  called by webagent 
		arguments:	  [in] wa, webagent object	
					  
		return value: true if success
		--------------------------------------------------------------------------*/
	extern sapdbwa_Int2 WDVHandlerInit( sapdbwa_Handle wa );


	/*!-------------------------------------------------------------------------
		function:     WDVHandlerExit
		description:  called by webagent 
		arguments:	  [in] wa, webagent object	
					  
		return value: true if success
		--------------------------------------------------------------------------*/
	extern sapdbwa_Int2 WDVHandlerExit( sapdbwa_Handle wa );


	/*!-------------------------------------------------------------------------
		function:     WDVHandlerService
		description:  called by webagent 
		arguments:	  [in] wa, webagent object	
					  [in] req, webagent http request object
					  [in] rep, webagent http reply object
					  
		return value: true if success
		--------------------------------------------------------------------------*/
	extern sapdbwa_Int2 WDVHandlerService( sapdbwa_Handle		wa,
										   sapdbwa_HttpRequestP request,
										   sapdbwa_HttpReplyP   reply );

#endif