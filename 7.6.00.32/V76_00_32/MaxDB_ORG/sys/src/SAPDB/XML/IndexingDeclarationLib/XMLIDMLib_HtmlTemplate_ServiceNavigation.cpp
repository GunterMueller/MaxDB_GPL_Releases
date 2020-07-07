/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_ServiceNavigation.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ServiceNavigation.hpp"



	XMLIDMLib_HtmlTemplate_ServiceNavigation::XMLIDMLib_HtmlTemplate_ServiceNavigation (sapdbwa_WebAgent &wa,
																  sapdbwa_HttpRequest &req,
																  XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_SRVNAVIGATION )
	{ 
		XMLIMAPI_ServiceDescListCreate(&_services);
		_servicesCount = 0;
		_firstService = XMLIMAPI_True;

		getServices();

		strcpy(_serviceName,wa.GetServiceName());

	} 


	XMLIDMLib_HtmlTemplate_ServiceNavigation::~XMLIDMLib_HtmlTemplate_ServiceNavigation() 
	{
		XMLIMAPI_ServiceDescListDestroy(_services);
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_ServiceNavigation :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 _nReturn = 0;

		if (szName == TEMPLATE_KEY_SRV_NAVI_IS) {
			_nReturn = _servicesCount;
		} else if (szName == TEMPLATE_KEY_SRV_NAVI_SERVICE) {
			_nReturn = 1;
		} 
		return _nReturn;
	} 

	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_ServiceNavigation :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		XMLIMAPI_ServiceDesc		*__currService;
		Tools_DynamicUTF8String		__buffer;
		XMLIDMLIB_Char				__string1[4096];
		XMLIDMLIB_Char				__string2[4096];


		if (szName == TEMPLATE_VALUE_SRV_NAVI_IS) {
			if (_firstService) {
				XMLIMAPI_ServiceDescListGetFirst(_services,&__currService);
				_firstService = XMLIMAPI_False;
			}
			else
				XMLIMAPI_ServiceDescListGetNext(_services,&__currService);

			XMLIDMLIB_EscapeQuotes(__currService->ServiceDesc,strlen(__currService->ServiceDesc),__string1);
			XMLIDMLIB_EscapeQuotes(__currService->ServiceName,strlen(__currService->ServiceName),__string2);
			sp77sprintf(_string,4096,TEXT_CONSTANT_HTML_SERVICE,
						_serviceName,
						__currService->ServiceID,
						__string1,
						__string2);
			__buffer.ConvertFromASCII_Latin1(_string,_string+strlen(_string));
			return __buffer;
		} else if (szName == TEMPLATE_VALUE_SRV_NAVI_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		return "";
	}

	void XMLIDMLib_HtmlTemplate_ServiceNavigation::getServices()
	{
		XMLIMAPI_ServiceDesc		__service;
		XMLIMAPI_ErrorItem			__errorItem;
		XMLIMAPI_ErrorType			__errorType;
		XMLIMAPI_ErrorText			__errorMsg;
		XMLIDMLIB_Char				__msg[1024];
		
		if (XMLIMAPI_ServiceGetFirst(_xml,&__service)) {

			do {
				XMLIMAPI_ServiceDescListAddItem(_services,&__service);
				_servicesCount++;						
			} while (XMLIMAPI_ServiceGetNext(_xml,&__service));

		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_ServiceNavigation::getServices] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}
