/****************************************************************************

  module      : WDVHandler_Error.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-28  13:45
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_Error.h"

	/*------------------------------------------------------------------------
		Function: sendErrorReply
	--------------------------------------------------------------------------*/
	void sendErrorReply( sapdbwa_Int2 statuscode,
			             sapdbwa_HttpReplyP   reply,
						 const char* method,
						 const char* errormsg)					/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
	{
		char contentlength[10];
		char buf[4096];
		strcpy(buf,"");

		switch (statuscode) {
			case ERROR_400: 
				sp77sprintf(buf,4095,"%s%s%s",ERROR_PAGE_400,errormsg,ERROR_PAGE_FOOTER);	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1124197 */
				break; 
			case ERROR_500_CON:
				sp77sprintf(buf,4095,ERROR_PAGE_500_CON,method);
				break;
			case ERROR_501:
				sp77sprintf(buf,4095,ERROR_PAGE_501,method);
				break;
		}

		sapdbwa_InitHeader(reply,
                           statuscode,
                           "text/html",
                           NULL,
	                       NULL,								/* const char    *LastModified,*/
                           NULL,								/* const char    *Expires,*/
                           NULL);								/* const char    *Location );*/

		sp77sprintf(contentlength,9,"%d",strlen(buf));
		sapdbwa_SetHeader( reply,
						"Content-Length",
						contentlength);

		sapdbwa_SendHeader(reply);

		sapdbwa_SendBody(reply,
                         buf,
                         strlen(buf));
		
	}


