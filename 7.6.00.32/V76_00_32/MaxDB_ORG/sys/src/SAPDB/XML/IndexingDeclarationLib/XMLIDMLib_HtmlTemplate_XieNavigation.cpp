/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_XieNavigation.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-21  17:11
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


	/*------------------------------------------------------------------------
		Includes
	--------------------------------------------------------------------------*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XieNavigation.hpp"
	#include "SAPDBCommon/SAPDB_string.h"



	XMLIDMLib_HtmlTemplate_XieNavigation::XMLIDMLib_HtmlTemplate_XieNavigation (sapdbwa_WebAgent &wa,
																  				sapdbwa_HttpRequest &req,
																				XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_XIENAVIGATION )
	{ 
		XMLIMAPI_XieAdminListCreate(&_xieList);
		_xieCount = 0;
		_firstXie = XMLIMAPI_True;

		getXieList();

		strcpy(_serviceName,wa.GetServiceName());
		strcpy(_lastXieNode,"");
	} 


	XMLIDMLib_HtmlTemplate_XieNavigation::~XMLIDMLib_HtmlTemplate_XieNavigation() 
	{
		/* delete all document classes */
		XMLIMAPI_XieAdminListDestroy(_xieList);
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_XieNavigation :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_XIE_NAVI_XIE) {
			__nReturn = _xieCount;
		} else if (szName == TEMPLATE_KEY_XIE_NAVI_SERVICE) {
			__nReturn = 1;
		}
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_XieNavigation :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		XMLIMAPI_XieAdmin		*__curr;
		Tools_DynamicUTF8String __buffer;
		XMLIDMLIB_Char			__node[26];

		if (szName == TEMPLATE_VALUE_XIE_NAVI_XIE) {
			if (_firstXie) {
				XMLIMAPI_XieAdminListGetFirst(_xieList,&__curr);
				_firstXie = XMLIMAPI_False;
			}
			else
				XMLIMAPI_XieAdminListGetNext(_xieList,&__curr);

			if (strcmp(__curr->Node,_lastXieNode)==0) {
				sp77sprintf(_string,4096,TEXT_CONSTANT_HTML_XIENAVI_XIENAME,
							_serviceName,
							__curr->ID,
							__curr->description,
							__curr->XieName);
			} else {
				/* Group change */
				if (strlen(__curr->Node)>=26) {
					strncpy(__node,__curr->Node,12);
					__node[12] = '.';
					__node[13] = '.';
					__node[14] = '.';
					SAPDB_memcpy(__node+15,__curr->Node+(strlen(__curr->Node)-10),10);
					__node[25] = '\0';
				} else {
					strcpy(__node,__curr->Node);
				}
				sp77sprintf(_string,4096,TEXT_CONSTANT_HTML_XIENAVI_XIENODE,
							__curr->Node,
							__node,
							_serviceName,
							__curr->ID,
							__curr->description,
							__curr->XieName);
				strcpy(_lastXieNode,__curr->Node);
			}
			__buffer.ConvertFromASCII_Latin1(_string,_string+strlen(_string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XIE_NAVI_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		return "";
	}

	void XMLIDMLib_HtmlTemplate_XieNavigation::getXieList()
	{
		XMLIMAPI_XieAdmin			__xie;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLIDMLIB_Char				__msg[1024];
		
		if (XMLIMAPI_XieAdminGetFirst(_xml,&__xie)) {

			do {
				XMLIMAPI_XieAdminListAddItem(_xieList,&__xie);
				_xieCount++;						
			} while (XMLIMAPI_XieAdminGetNext(_xml,&__xie));


		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_XieNavigation::getXieList] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}
