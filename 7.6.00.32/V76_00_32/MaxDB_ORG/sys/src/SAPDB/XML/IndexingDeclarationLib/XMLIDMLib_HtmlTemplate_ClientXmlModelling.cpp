/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_ClientXmlModelling.cpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-26  16:19
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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ClientXmlModelling.hpp"



	XMLIDMLib_HtmlTemplate_ClientXmlModelling::XMLIDMLib_HtmlTemplate_ClientXmlModelling (sapdbwa_WebAgent &wa,
																  						  sapdbwa_HttpRequest &req) 
		                  : _req(req), Tools_Template ( wa, (const unsigned char*)TEMPLATE_CLIENT_XMLMODELLING )
	{ 
		strcpy(_serviceName,wa.GetServiceName());
	} 

	XMLIDMLib_HtmlTemplate_ClientXmlModelling::~XMLIDMLib_HtmlTemplate_ClientXmlModelling() 
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_ClientXmlModelling :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		return 1;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_ClientXmlModelling :: askForValue (const Tools_DynamicUTF8String & szName)
	{

		XMLIDMLIB_Char	__value[513];

		if (szName == TEMPLATE_KEY_CLNT_XML_DATA) {
			XMLIDMLIB_GetParameterValue(TEXT_CONSTANT_PARAM_CLIENT_XML_DATA,_req,__value);
			if (strcmp(__value,TEXT_CONSTANT_PARAM_VAL_CLNT_XML_DATA_EMPTY)==0) {
				sp77sprintf(__value,512,TEXT_CONSTANT_HTML_CLNTXML_EMPTY,_serviceName);
				return (Tools_DynamicUTF8String::ConstPointer)__value;
			} else if (strcmp(__value,TEXT_CONSTANT_PARAM_VAL_CLNT_XML_DATA_NEWDOC)==0) {
				sp77sprintf(__value,512,TEXT_CONSTANT_HTML_CLNTXML_NEWDOCCLASS,_serviceName);
				return (Tools_DynamicUTF8String::ConstPointer)__value;
			} else if (strcmp(__value,TEXT_CONSTANT_PARAM_VAL_CLNT_XML_DATA_NEWXML)==0) {
				sp77sprintf(__value,512,TEXT_CONSTANT_HTML_CLNTXML_NEWXMLINDEX,_serviceName);
				return (Tools_DynamicUTF8String::ConstPointer)__value;
			}
			return "";
		} else if (szName == TEMPLATE_KEY_CLNT_XML_SERVICE) {
			return (Tools_DynamicUTF8String::ConstPointer)_serviceName;
		}
		return "";
	}

