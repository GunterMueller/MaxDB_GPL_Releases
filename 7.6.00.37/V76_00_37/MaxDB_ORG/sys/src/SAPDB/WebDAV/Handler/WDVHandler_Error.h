#ifndef WDVHandler_ERROR_H
#define WDVHandler_ERROR_H

/****************************************************************************

  module      : WDVHandler_Error.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  10:35
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
/*	#define sapdbwa_MEMCHECK
*/
	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorCodes.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorPages.h"


 	/*------------------------------------------------------------------------
		Function	: sendErrorReply
		Description	: send an Error HTTP response
		Parameters	: [in] statuscode	-	HTTP status code
					  [in] request		-	HTTP request
					  [in] reply		-	HTTP reply
					  [in] method		-	HTTP method that caused the error
		Return		: ---
	--------------------------------------------------------------------------*/
	void sendErrorReply( sapdbwa_Int2 statuscode,
			             sapdbwa_HttpReplyP   reply,
						 const char* method,
						 const char* errormsg);				/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */



#endif