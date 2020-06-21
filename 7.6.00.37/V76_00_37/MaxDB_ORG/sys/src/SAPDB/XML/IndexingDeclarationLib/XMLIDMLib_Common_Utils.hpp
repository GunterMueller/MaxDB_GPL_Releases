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
#ifndef XMLIDMLIB_COMMON_UTILS_H
#define XMLIDMLIB_COMMON_UTILS_H

/*!**********************************************************************

  module: XMLIDMLib_Common_Utils.h

  -----------------------------------------------------------------------

  responsible:  Daniel Kirmse (D027470 - Daniel.Kirmse@SAP.com)

  special area: WebDAV / XML Indexing

  description:  Helper functions

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/


	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_TextConstants.h"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	

	/*===================================================================
	// Defines
	//===================================================================*/


	/*===================================================================
	// Functions
	//===================================================================*/
	XMLIDMLIB_Bool XMLIDMLIB_IsQueryStringValid (sapdbwa_HttpRequestP req);

	void XMLIDMLIB_SendHTMLFile(sapdbwa_Handle wa,sapdbwa_HttpReplyP   rep,const Tools_DynamicUTF8String& sFile);
	XMLIDMLIB_Bool XMLIDMLIB_IsPageRequest (sapdbwa_HttpRequestP  req, XMLIDMLIB_Char *page);
	XMLIDMLIB_Bool XMLIDMLIB_IsCommandRequest (sapdbwa_HttpRequestP  req);
	XMLIDMLIB_Bool XMLIDMLIB_IsCommandRequestXie (sapdbwa_HttpRequestP  req);
	XMLIDMLIB_Bool XMLIDMLIB_IsCommandRequestQuery (sapdbwa_HttpRequestP  req);
	XMLIDMLIB_Bool XMLIDMLIB_IsCommand(sapdbwa_HttpRequestP req, XMLIDMLIB_Char *cmdstring);
	XMLIDMLIB_Bool XMLIDMLIB_IsSubCommand(sapdbwa_HttpRequest &req, XMLIDMLIB_Char *cmdstring);
	XMLIDMLIB_Bool XMLIDMLIB_GetParameterValue(const XMLIDMLIB_Char *pParamName,sapdbwa_HttpRequest &req,XMLIDMLIB_Char *sValue);
	XMLIDMLIB_Bool XMLIDMLIB_GetParameterValueC(const XMLIDMLIB_Char *pParamName,sapdbwa_HttpRequestP req,XMLIDMLIB_Char *sValue);
	XMLIDMLIB_Bool XMLIDMLIB_CreateConnUserData(XMLIDMLIB_ConnUserData *userdata);
	XMLIDMLIB_Bool XMLIDMLIB_DestroyConnUserData(void *userdata);
	void XMLIDMLIB_SendPageHeaders(sapdbwa_HttpReplyP rep, XMLIDMLIB_Char *mimetype, XMLIDMLIB_Char *expires, XMLIDMLIB_Char *charset);
	void XMLIDMLIB_strMaxCopy(char *destination, char *source, size_t sizeToCopy );
	void XMLIDMLIB_EscapeQuotes(XMLIDMLIB_Char *oldstring, XMLIDMLIB_Int4 len, XMLIDMLIB_Char *newstring);
								 	 
#endif