/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_XmlNavigation.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-26  14:03
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


	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XmlNavigation.hpp"



	XMLIDMLib_HtmlTemplate_XmlNavigation::XMLIDMLib_HtmlTemplate_XmlNavigation (sapdbwa_WebAgent &wa,
																  		sapdbwa_HttpRequest &req,
																		XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_XMLNAVIGATION )
	{ 
		XMLIMAPI_XmlIndexListCreate(&_xmlIndexList);
		_xmlIndexCount = 0;
		_firstXmlIndex = XMLIMAPI_True;

		getXmlIndices();

		strcpy(_serviceName,wa.GetServiceName());
	}

	XMLIDMLib_HtmlTemplate_XmlNavigation::~XMLIDMLib_HtmlTemplate_XmlNavigation() 
	{
		/* delete all xml indices */
		XMLIMAPI_XmlIndexListDestroy(_xmlIndexList);
	}

	SAPDB_Int2 XMLIDMLib_HtmlTemplate_XmlNavigation :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_XML_NAVI_XMLINDEX) {
			__nReturn = _xmlIndexCount;
		} else if (szName == TEMPLATE_KEY_XML_NAVI_SERVICE) {
			__nReturn = 1;
		}
		
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_XmlNavigation :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		XMLIMAPI_XmlIndex	*__curr;
		Tools_DynamicUTF8String __buffer;

		if (szName == TEMPLATE_VALUE_XML_NAVI_XMLINDEX) {
			if (_firstXmlIndex) {
				XMLIMAPI_XmlIndexListGetFirst(_xmlIndexList,&__curr);
				_firstXmlIndex = XMLIMAPI_False;
			}
			else
				XMLIMAPI_XmlIndexListGetNext(_xmlIndexList,&__curr);

			sp77sprintf(_string,4096,TEXT_CONSTANT_HTML_XMLNAVI_XMLINDEX,
						_serviceName,
						__curr->IndexID,
						__curr->indexName,
						__curr->description,
						__curr->indexName);
			__buffer.ConvertFromASCII_Latin1(_string,_string+strlen(_string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_XML_NAVI_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		return "";
	}

	void XMLIDMLib_HtmlTemplate_XmlNavigation::getXmlIndices()
	{
		XMLIMAPI_XmlIndex		__xmlIndex;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__msg[1024];
		
		if (XMLIMAPI_XmlIndexGetFirst(_xml,&__xmlIndex)) {
			do {
				XMLIMAPI_XmlIndexListAddItem(_xmlIndexList,&__xmlIndex);
				_xmlIndexCount++;						
			} while (XMLIMAPI_XmlIndexGetNext(_xml,&__xmlIndex));
		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_XmlNavigation::getXmlIndices] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}
	}
