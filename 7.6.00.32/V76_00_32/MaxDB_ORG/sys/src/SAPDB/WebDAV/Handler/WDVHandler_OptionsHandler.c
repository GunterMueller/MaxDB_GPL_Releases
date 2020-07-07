/****************************************************************************

  module      : WDVHandler_OptionsHandler.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  13:32
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


	#include "SAPDB/WebDAV/Handler/WDVHandler_OptionsHandler.h"


	/*------------------------------------------------------------------------
		Function: optionsCallCapiFunc
	--------------------------------------------------------------------------*/
	sapdbwa_Int2 optionsCallCapiFunc(const char* requesturi, sapdbwa_Handle wa)
	{
		return ERROR_200;
	}

	/*------------------------------------------------------------------------
		Function:	optionsBuildReply
	--------------------------------------------------------------------------*/
	void optionsBuildReply(sapdbwa_Int2 statuscode, sapdbwa_HttpReplyP reply, const char* resourceURI, const char* host, const char* port )
	{
		char *buf;
		char *url;
		sapdbwa_Bool allocatOK;
		sapdbwa_Int4 length;

		if (reply==NULL || resourceURI==NULL || host==NULL || port==NULL) {
			return;
		}

		/* initialize Response Header */
		sapdbwa_InitHeader(reply,
                           statuscode,
                           "text/html",
                           NULL,
	                       NULL,								/* const char    *LastModified,*/
                           NULL,								/* const char    *Expires,*/
                           NULL);								/* const char    *Location );*/

		sapdbwa_SetHeader( reply,
                           "Content-Length",
                           "0" );

		sapdbwa_SetHeader( reply,
                           "MS-Author-Via",
                           "DAV:" );

		sapdbwa_SetHeader( reply,
                           "Allow",
                           "OPTIONS, GET, HEAD, POST, PUT, DELETE, PROPFIND, PROPPATCH, MOVE, COPY, LOCK, UNLOCK" );
		sapdbwa_SetHeader( reply,
                           "DAV",
                           "1,2" );

		sapdbwa_SendHeader(reply);

	}

