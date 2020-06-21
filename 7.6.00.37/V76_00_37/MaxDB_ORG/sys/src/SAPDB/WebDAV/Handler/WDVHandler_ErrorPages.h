#ifndef WDVHandler_ERRORPAGES_H
#define WDVHandler_ERRORPAGES_H

/****************************************************************************

  module      : WDVHandler_ErrorPages.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-02-26  13:31
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

	
	/* Client Errors 4xx */
	#define ERROR_PAGE_400		"<HTML><HEAD><TITLE>400 - Bad Request</TITLE></HEAD><BODY><H1>400 - Bad Request</H1>"	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */
	#define ERROR_PAGE_403		"<HTML><HEAD><TITLE>403 - Forbidden</TITLE></HEAD><BODY><H1>403 - Forbidden</H1>"	/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124007 */
	#define ERROR_PAGE_404		"<HTML><HEAD><TITLE>404 - Not Found</TITLE></HEAD><BODY><H1>404 - Not Found</H1></BODY></HTML>"
	#define ERROR_PAGE_405		"<HTML><HEAD><TITLE>405 - Method Not Allowed</TITLE></HEAD><BODY><H1>405 - Method not allowed</H1></BODY></HTML>"
	#define ERROR_PAGE_409		"<HTML><HEAD><TITLE>409 - Conflict</TITLE></HEAD><BODY><H1>409 - Conflict</H1></BODY></HTML>"
	/* Begin Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124043 */
	#define ERROR_PAGE_409_OWN	"<HTML><HEAD><TITLE>409 - Conflict</TITLE></HEAD><BODY><H1>409 - Conflict</H1>"
	/* End Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1124043 */
	#define ERROR_PAGE_412		"<HTML><HEAD><TITLE>412 - Precondition Failed</TITLE></HEAD><BODY><H1>412 - Precondition Failed</H1></BODY></HTML>"
	/* Begin Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
	#define ERROR_PAGE_416		"<HTML><HEAD><TITLE>416 - Requested range not satisfiable</TITLE></HEAD><BODY><H1>416 - Requested range not satisfiable</H1></BODY></HTML>"
	/* End Correction  http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1120395 */
	#define ERROR_PAGE_423		"<HTML><HEAD><TITLE>423 - Locked</TITLE></HEAD><BODY><H1>423 - Locked</H1></BODY></HTML>"
	#define ERROR_PAGE_FOOTER	"</BODY></HTML>"		/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123371 */

	/* Server Errors 5xx */
	/* 500 (Internal Server Error) */
	#define ERROR_PAGE_500		"<HTML><HEAD><TITLE>500 - Internal Server Error</TITLE></HEAD><BODY><H1>500 - Internal Server Error</H1></BODY></HTML>"
	#define ERROR_PAGE_500_CON	"<HTML><HEAD><TITLE>500 - Internal Server Error</TITLE></HEAD><BODY><H1>500 - Internal Server Error</H1>Connect to database failed. Please contact your administrator.</BODY></HTML>"
	/* 501 (Not Implemented) */
	#define ERROR_PAGE_501		"<HTML><HEAD><TITLE>501 - Not Implemented</TITLE></HEAD><BODY><H1>501 - Not Implemented</H1>Method \"%s\" not yet implemented.</BODY></HTML>"
	#define ERROR_PAGE_502		"<HTML><HEAD><TITLE>502 - Bad Gateway</TITLE></HEAD><BODY><H1>502 - Bad Gateway</H1>Method \"%s\" not yet implemented.</BODY></HTML>"


	/* Text version for including in existing but not completed body stream */
	/* Server Errors 5xx */
	/* 500 (Internal Server Error) */
	#define ERROR_PAGE_500_TEXT		"<HTML><HEAD><TITLE>500 - Internal Server Error</TITLE></HEAD><BODY><H1>500 - Internal Server Error</H1></BODY></HTML>"
	/* 501 (Not Implemented) */
	#define ERROR_PAGE_501_TEXT		"<HTML><HEAD><TITLE>501 - Not Implemented</TITLE></HEAD><BODY><H1>501 - Not Implemented</H1>Method \"%s\" not yet implemented.</BODY></HTML>"



#endif