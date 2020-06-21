/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_DocNavigation.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_DocNavigation.hpp"



	XMLIDMLib_HtmlTemplate_DocNavigation::XMLIDMLib_HtmlTemplate_DocNavigation (sapdbwa_WebAgent &wa,
																  				sapdbwa_HttpRequest &req,
																				XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_DOCNAVIGATION )
	{ 
		XMLIMAPI_DocClassListCreate(&_docClassList);
		_docClassCount = 0;
		_firstDocClass = XMLIMAPI_True;

		getDocClasses();
		strcpy(_serviceName,wa.GetServiceName());
	} 


	XMLIDMLib_HtmlTemplate_DocNavigation::~XMLIDMLib_HtmlTemplate_DocNavigation() 
	{
		/* delete all document classes */
		XMLIMAPI_DocClassListDestroy(_docClassList);
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_DocNavigation :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_DOC_NAVI_DOCCLASS) {
			__nReturn = _docClassCount;
		} else if (szName == TEMPLATE_KEY_DOC_NAVI_SERVICE) {
			__nReturn = 1;
		}
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_DocNavigation :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		XMLIMAPI_DocClass	*__curr;
		Tools_DynamicUTF8String __buffer;

		if (szName == TEMPLATE_VALUE_DOC_NAVI_DOCCLASS) {
			if (_firstDocClass) {
				XMLIMAPI_DocClassListGetFirst(_docClassList,&__curr);
				_firstDocClass = XMLIMAPI_False;
			}
			else
				XMLIMAPI_DocClassListGetNext(_docClassList,&__curr);

			sp77sprintf(_string,4096,TEXT_CONSTANT_HTML_DOCNAVI_DOCCLASS,
						_serviceName,
						__curr->docClassID,
						__curr->description,
						__curr->docClassName);
			__buffer.ConvertFromASCII_Latin1(_string,_string+strlen(_string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_DOC_NAVI_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		return "";
	}

	void XMLIDMLib_HtmlTemplate_DocNavigation::getDocClasses()
	{
		XMLIMAPI_DocClass			__docClass;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLIDMLIB_Char				__msg[1024];
		
		if (XMLIMAPI_DocClassGetFirst(_xml,&__docClass)) {

			do {
				XMLIMAPI_DocClassListAddItem(_docClassList,&__docClass);
				_docClassCount++;						
			} while (XMLIMAPI_DocClassGetNext(_xml,&__docClass));


		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_DocNavigation::getDocClasses] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}
