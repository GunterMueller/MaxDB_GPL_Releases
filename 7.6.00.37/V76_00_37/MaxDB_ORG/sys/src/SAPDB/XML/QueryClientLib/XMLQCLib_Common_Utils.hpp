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
#ifndef XMLQCLIB_COMMON_UTILS_H
#define XMLQCLIB_COMMON_UTILS_H

/*!**********************************************************************

  module: XMLQCLib_Common_Utils.h

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
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Common_TextConstants.h"
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp"
	

	/*===================================================================
	// Defines
	//===================================================================*/


	/*===================================================================
	// Functions
	//===================================================================*/
	XMLQCLIB_Bool XMLQCLIB_IsQueryStringValid (sapdbwa_HttpRequestP req);

	void XMLQCLIB_SendHTMLFile(sapdbwa_Handle wa,sapdbwa_HttpReplyP   rep,const Tools_DynamicUTF8String& sFile);
	XMLQCLIB_Bool XMLQCLIB_IsPageRequest (sapdbwa_HttpRequestP  req, XMLQCLIB_Char *page);
	XMLQCLIB_Bool XMLQCLIB_IsCommandRequest (sapdbwa_HttpRequestP  req);
	XMLQCLIB_Bool XMLQCLIB_IsCommand(sapdbwa_HttpRequestP req, XMLQCLIB_Char *cmdstring);
	XMLQCLIB_Bool XMLQCLIB_IsSubCommand(sapdbwa_HttpRequest &req, XMLQCLIB_Char *cmdstring);
	XMLQCLIB_Bool XMLQCLIB_GetParameterValue(const XMLQCLIB_Char *pParamName,sapdbwa_HttpRequest &req,XMLQCLIB_Char *sValue);
	XMLQCLIB_Bool XMLQCLIB_GetParameterValueC(const XMLQCLIB_Char *pParamName,sapdbwa_HttpRequestP req,XMLQCLIB_Char *sValue);
	XMLQCLIB_Bool XMLQCLIB_CreateConnUserData(XMLQCLIB_ConnUserData *userdata);
	XMLQCLIB_Bool XMLQCLIB_DestroyConnUserData(void *userdata);
	void XMLQCLIB_SendPageHeaders(sapdbwa_HttpReplyP rep, XMLQCLIB_Char *mimetype, XMLQCLIB_Char *expires, XMLQCLIB_Char *charset);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListCreate(XMLQCLIB_QueryParamsIdxList *list);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListDestroy(XMLQCLIB_QueryParamsIdxList list);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListInit(XMLQCLIB_QueryParamsIdxList list);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListAddItem(XMLQCLIB_QueryParamsIdxList list,XMLQCLIB_QueryParamsIdx *param);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListGetFirst(XMLQCLIB_QueryParamsIdxList list,XMLQCLIB_QueryParamsIdx **param);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxListGetNext(XMLQCLIB_QueryParamsIdxList list,XMLQCLIB_QueryParamsIdx **param);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsIdxHasValidLine(XMLQCLIB_QueryParamsIdxList	list);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsListCreate(XMLQCLIB_QueryParamsList *list);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsListDestroy(XMLQCLIB_QueryParamsList list);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsListInit(XMLQCLIB_QueryParamsList list);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsListAddItem(XMLQCLIB_QueryParamsList list,XMLQCLIB_QueryParams *param);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsListGetFirst(XMLQCLIB_QueryParamsList list,XMLQCLIB_QueryParams **param);
	XMLQCLIB_Bool XMLQCLIB_QueryParamsListGetNext(XMLQCLIB_QueryParamsList list,XMLQCLIB_QueryParams **param);
	XMLQCLIB_UInt4 XMLQCLIB_QueryParamsListGetLength(XMLQCLIB_QueryParamsList	list);
	XMLQCLIB_Bool XMLQCLIB_BuildWhereClause(XMLQCLIB_QueryParamsList params, XMLQSLib_WhereClause *where);
	void XMLQCLIB_strMaxCopy(char *destination, char *source, size_t sizeToCopy );
	void XMLQCLIB_EscapeQuotes(XMLQCLIB_Char *oldstring, XMLQCLIB_Int4 len, XMLQCLIB_Char *newstring);
	void XMLQCLIB_BuildServerStringLong(sapdbwa_WebAgent &wa,sapdbwa_HttpRequest &request,XMLQCLIB_Char *server,XMLQCLIB_Int2 length);
								 	 
#endif