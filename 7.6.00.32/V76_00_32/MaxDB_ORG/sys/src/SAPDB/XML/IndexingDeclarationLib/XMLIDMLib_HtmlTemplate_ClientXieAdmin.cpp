/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_ClientXieAdmin.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ClientXieAdmin.hpp"



	XMLIDMLib_HtmlTemplate_ClientXieAdmin::XMLIDMLib_HtmlTemplate_ClientXieAdmin (sapdbwa_WebAgent &wa,
																  						  sapdbwa_HttpRequest &req) 
		                  : _req(req), Tools_Template ( wa, (const unsigned char*)TEMPLATE_CLIENT_XIEADMIN )
	{ 
		strcpy(_serviceName,wa.GetServiceName());
	} 

	XMLIDMLib_HtmlTemplate_ClientXieAdmin::~XMLIDMLib_HtmlTemplate_ClientXieAdmin() 
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_ClientXieAdmin :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		return 1;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_ClientXieAdmin :: askForValue (const Tools_DynamicUTF8String & szName)
	{

		Tools_DynamicUTF8String __buffer;

		if (szName == TEMPLATE_KEY_CLNT_XML_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		return "";
	}

