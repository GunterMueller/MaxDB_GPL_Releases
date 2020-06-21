/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_Xie_ShowAll.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_Xie_ShowAll.hpp"



	XMLIDMLib_HtmlTemplate_Xie_ShowAll::XMLIDMLib_HtmlTemplate_Xie_ShowAll (sapdbwa_WebAgent &wa,
																  				sapdbwa_HttpRequest &req,
																				XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_XIE_SHOWALL )
	{ 
		XMLIMAPI_XieAdminListCreate(&_xieList);
		_xieCount = 0;

		getXieList();

		if (XMLIMAPI_XieAdminListGetFirst(_xieList,&_currXie)) {
			_endOfList = XMLIDMLIB_False;
			_firstXie = XMLIDMLIB_True;
		} else {
			_firstXie = XMLIDMLIB_False;
			_endOfList = XMLIDMLIB_True;
		}
		strcpy(_serviceName,wa.GetServiceName());
		strcpy(_lastXieNode,"");
	} 


	XMLIDMLib_HtmlTemplate_Xie_ShowAll::~XMLIDMLib_HtmlTemplate_Xie_ShowAll() 
	{
		/* delete all document classes */
		XMLIMAPI_XieAdminListDestroy(_xieList);
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_Xie_ShowAll :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_XIE_SHOWALL_XIE) {
			__nReturn = -1;
		} else if (szName == TEMPLATE_KEY_XIE_SHOWALL_XIE_NODES) {
			__nReturn = -1;
		} else if (szName == TEMPLATE_KEY_XIE_SHOWALL_XIE_NODE) {
			__nReturn = 1;
		} else if (szName == TEMPLATE_KEY_XIE_SHOWALL_SERVICE) {
			__nReturn = 1;
		}
		return __nReturn;
	} 

	bool XMLIDMLib_HtmlTemplate_Xie_ShowAll :: askForContinue ( const Tools_DynamicUTF8String & szName )
	{
		if (szName == TEMPLATE_KEY_XIE_SHOWALL_XIE) {
			// another XIE for this node?
			if (_firstXie) {
				_firstXie = XMLIDMLIB_False;
				_groupChange = XMLIDMLIB_False;
				return XMLIDMLIB_True;
			} else {
				if (!_groupChange) {
					if (XMLIMAPI_XieAdminListGetNext(_xieList,&_currXie)) {
						if (strcmp(_lastXieNode,_currXie->Node)==0) {
							return XMLIDMLIB_True;
						} else {
							return XMLIDMLIB_False;
						}
					} else {
						_endOfList = XMLIDMLIB_True;
						return XMLIDMLIB_False;
					}
				} else {
					_groupChange = XMLIDMLIB_False;
					return XMLIDMLIB_True;
				}
			}
		} else if (szName == TEMPLATE_KEY_XIE_SHOWALL_XIE_NODES) {
			if (!_endOfList) {
				strcpy(_lastXieNode,_currXie->Node);
				_groupChange = XMLIDMLIB_True;
				return XMLIDMLIB_True;
			} else {
				return XMLIDMLIB_False;
			}
		}
		return XMLIDMLIB_False;
	}


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_Xie_ShowAll :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		Tools_DynamicUTF8String __buffer;

		if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIENAME) {
			__buffer.ConvertFromASCII_Latin1(_currXie->XieName,_currXie->XieName+strlen(_currXie->XieName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIENODE) {
			__buffer.ConvertFromASCII_Latin1(_currXie->Node,_currXie->Node+strlen(_currXie->Node));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIEDESC) {
			__buffer.ConvertFromASCII_Latin1(_currXie->description,_currXie->description+strlen(_currXie->description));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIEISNAME) {
			__buffer.ConvertFromASCII_Latin1(_currXie->IndexingServiceName,_currXie->IndexingServiceName+strlen(_currXie->IndexingServiceName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIEISDOCSTORE) {
			__buffer.ConvertFromASCII_Latin1(_currXie->IndexingServiceDocStoreName,_currXie->IndexingServiceDocStoreName+strlen(_currXie->IndexingServiceDocStoreName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIEISIDXSTORE) {
			__buffer.ConvertFromASCII_Latin1(_currXie->IndexingServiceIdxStoreName,_currXie->IndexingServiceIdxStoreName+strlen(_currXie->IndexingServiceIdxStoreName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIEILNAME) {
			__buffer.ConvertFromASCII_Latin1(_currXie->IndexingLibName,_currXie->IndexingLibName+strlen(_currXie->IndexingLibName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIEILVERSION) {
			__buffer.ConvertFromASCII_Latin1(_currXie->IndexingLibVersion,_currXie->IndexingLibVersion+strlen(_currXie->IndexingLibVersion));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_XIEALIVE) {
			__buffer.ConvertFromASCII_Latin1(_currXie->Alive,_currXie->Alive+strlen(_currXie->Alive));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOWALL_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}

		return "";
	}

	void XMLIDMLib_HtmlTemplate_Xie_ShowAll::getXieList()
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
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_Xie_ShowAll::getXieList] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}
