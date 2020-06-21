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
#ifndef StudioWeb_WEBREQUEST_HPP
#define StudioWeb_WEBREQUEST_HPP

/*!**********************************************************************

  module: StudioWeb_WebRequest.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebSQL

  description:  functions for WebSQL

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

//#include <WINDOWS.H>


#include "sapdbwa.hpp" //nocheck

class sapdbwa_WebAgent;
class sapdbwa_HttpRequest;
class sapdbwa_HttpReply;


/*!-------------------------------------------------------------------------
    function:     WebSQLInit
    description:  called by webagent 
    arguments:	  [in] wa, webagent object	
				  
    return value: true if success
    --------------------------------------------------------------------------*/
extern "C" sapdbwa_Int2 WebSQLInit( sapdbwa_WebAgent &wa );

/*!-------------------------------------------------------------------------
    function:     WebSQLExit
    description:  called by webagent 
    arguments:	  [in] wa, webagent object	
				  
    return value: true if success
    --------------------------------------------------------------------------*/
extern "C" sapdbwa_Int2 WebSQLExit( sapdbwa_WebAgent &wa );

/*!-------------------------------------------------------------------------
    function:     WebSQLService
    description:  called by webagent 
    arguments:	  [in] wa, webagent object	
				  [in] req, webagent http request object
				  [in] rep, webagent http reply object
				  
    return value: true if success
    --------------------------------------------------------------------------*/
extern "C" sapdbwa_Int2 WebSQLService( sapdbwa_WebAgent &wa,sapdbwa_HttpRequest &req,sapdbwa_HttpReply &rep );



#endif
