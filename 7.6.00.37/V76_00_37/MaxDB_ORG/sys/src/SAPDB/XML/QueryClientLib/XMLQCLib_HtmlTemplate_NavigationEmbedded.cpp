/****************************************************************************

  module      : XMLQCLib_HtmlTemplate_NavigationEmbedded.cpp

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
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_HtmlTemplate_NavigationEmbedded.hpp"



	XMLQCLib_HtmlTemplate_NavigationEmbedded::XMLQCLib_HtmlTemplate_NavigationEmbedded (sapdbwa_WebAgent		&wa,
																  sapdbwa_HttpRequest	&req) 
		                  : _req(req), Tools_Template ( wa, (const unsigned char*)TEMPLATE_EMBEDDED_NAVIGATION )
	{ 
		strcpy(_serviceName,wa.GetServiceName());
	} 


	XMLQCLib_HtmlTemplate_NavigationEmbedded::~XMLQCLib_HtmlTemplate_NavigationEmbedded() 
	{
	}

	XMLQCLIB_Int2 XMLQCLib_HtmlTemplate_NavigationEmbedded :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		return 1;
	} 


	Tools_DynamicUTF8String XMLQCLib_HtmlTemplate_NavigationEmbedded :: askForValue (const Tools_DynamicUTF8String & szName)
	{

		XMLQCLIB_Char			value[513];
		XMLQCLIB_GetParameterValue(TEXT_CONSTANT_QCL_PARAM_NAVI_TAB,_req,value);

		if (szName == TEMPLATE_VALUE_EMB_NAV_TAB) {
			return (Tools_DynamicUTF8String::ConstPointer)value;
		} else if (szName == TEMPLATE_VALUE_EMB_NAV_SERVICE) {
			return (Tools_DynamicUTF8String::ConstPointer)_serviceName;
		}
		return "";
	}

