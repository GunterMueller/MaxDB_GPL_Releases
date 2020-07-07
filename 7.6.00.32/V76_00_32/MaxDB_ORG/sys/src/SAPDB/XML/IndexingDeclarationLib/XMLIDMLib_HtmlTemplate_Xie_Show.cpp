/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_Xie_Show.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-22  14:32
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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_Xie_Show.hpp"


	XMLIDMLib_HtmlTemplate_Xie_Show::XMLIDMLib_HtmlTemplate_Xie_Show (sapdbwa_WebAgent &wa,
																  sapdbwa_HttpRequest &req,
																  XMLIMAPI_Handle	&xml) 
		                  : _wa(wa), _req(req), _xml(xml), Tools_Template ( wa, (const unsigned char*)TEMPLATE_XIE_SHOW )
	{ 
		getXie();

		strcpy(_serviceName,wa.GetServiceName());
	} 


	XMLIDMLib_HtmlTemplate_Xie_Show::~XMLIDMLib_HtmlTemplate_Xie_Show() 
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_Xie_Show :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		XMLIDMLIB_UInt4 __nReturn = 0;

		if (szName == TEMPLATE_KEY_XIE_SHOW_SERVICE) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XIE_SHOW_XIEID) {
			__nReturn = 1;
		}
		else if (szName == TEMPLATE_KEY_XIE_SHOW_XIE) {
			__nReturn = 1;
		}
		return __nReturn;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_Xie_Show :: askForValue (const Tools_DynamicUTF8String & szName)
	{
		Tools_DynamicUTF8String __buffer;

		if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEID) {
			__buffer.ConvertFromASCII_Latin1(_xie.ID,_xie.ID+strlen(_xie.ID));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIENAME) {
			__buffer.ConvertFromASCII_Latin1(_xie.XieName,_xie.XieName+strlen(_xie.XieName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIENODE) {
			__buffer.ConvertFromASCII_Latin1(_xie.Node,_xie.Node+strlen(_xie.Node));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEDESC) {
			__buffer.ConvertFromASCII_Latin1(_xie.description,_xie.description+strlen(_xie.description));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEINSTROOT) {
			__buffer.ConvertFromASCII_Latin1(_xie.InstRoot,_xie.InstRoot+strlen(_xie.InstRoot));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIELOGFILE) {
			__buffer.ConvertFromASCII_Latin1(_xie.LogFile,_xie.LogFile+strlen(_xie.LogFile));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEISNAME) {
			__buffer.ConvertFromASCII_Latin1(_xie.IndexingServiceName,_xie.IndexingServiceName+strlen(_xie.IndexingServiceName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEISDESC) {
			__buffer.ConvertFromASCII_Latin1(_xie.IndexingServiceDesc,_xie.IndexingServiceDesc+strlen(_xie.IndexingServiceDesc));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEISDOCSTORE) {
			__buffer.ConvertFromASCII_Latin1(_xie.IndexingServiceDocStoreName,_xie.IndexingServiceDocStoreName+strlen(_xie.IndexingServiceDocStoreName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEISIDXSTORE) {
			__buffer.ConvertFromASCII_Latin1(_xie.IndexingServiceIdxStoreName,_xie.IndexingServiceIdxStoreName+strlen(_xie.IndexingServiceIdxStoreName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEILNAME) {
			__buffer.ConvertFromASCII_Latin1(_xie.IndexingLibName,_xie.IndexingLibName+strlen(_xie.IndexingLibName));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEILDESC) {
			__buffer.ConvertFromASCII_Latin1(_xie.IndexingLibDesc,_xie.IndexingLibDesc+strlen(_xie.IndexingLibDesc));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEILVERSION) {
			__buffer.ConvertFromASCII_Latin1(_xie.IndexingLibVersion,_xie.IndexingLibVersion+strlen(_xie.IndexingLibVersion));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_XIEALIVE) {
			__buffer.ConvertFromASCII_Latin1(_xie.Alive,_xie.Alive+strlen(_xie.Alive));
			return __buffer;
		}
		else if (szName == TEMPLATE_VALUE_XIE_SHOW_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}

		return "";
	}

	void XMLIDMLib_HtmlTemplate_Xie_Show::getXie()
	{
		XMLIDMLIB_Char			__value[513];
		XMLIMAPI_Id				__xieId;
		XMLIMAPI_ErrorItem		__errorItem;
		XMLIMAPI_ErrorType		__errorType;
		XMLIMAPI_ErrorText		__errorMsg;
		XMLIDMLIB_Char			__msg[1024];
		
		// get values from querystring
		XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_XIE_ID,_req,__value);

		XMLIMAPI_IdStringAsId(__value,__xieId);
		
		if (XMLIMAPI_XieAdminGet(_xml,__xieId,&_xie)) {
		} else {
			XMLIMAPI_GetLastError(_xml,&__errorItem);
			XMLIMAPI_GetErrorType(__errorItem,&__errorType);
			XMLIMAPI_GetErrorText(__errorItem,&__errorMsg);
			sp77sprintf(__msg,1023,"[XMLIDMLIB::XMLIDMLib_HtmlTemplate_Xie_Show::getXie] %s\n",__errorMsg);
			_wa.WriteLogMsg(__msg);
		}

	}

