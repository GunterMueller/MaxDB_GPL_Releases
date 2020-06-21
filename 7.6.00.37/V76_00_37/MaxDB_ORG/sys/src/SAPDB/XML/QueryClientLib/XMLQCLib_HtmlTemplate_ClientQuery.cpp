/****************************************************************************

  module      : XMLQCLib_HtmlTemplate_ClientQuery.cpp

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
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_HtmlTemplate_ClientQuery.hpp"



	XMLQCLib_HtmlTemplate_ClientQuery::XMLQCLib_HtmlTemplate_ClientQuery (sapdbwa_WebAgent &wa,
																  						  sapdbwa_HttpRequest &req) 
		                  : _req(req), Tools_Template ( wa, (const unsigned char*)TEMPLATE_CLIENT_QUERY )
	{ 
		strcpy(_serviceName,wa.GetServiceName());
	} 

	XMLQCLib_HtmlTemplate_ClientQuery::~XMLQCLib_HtmlTemplate_ClientQuery() 
	{
	}

	XMLQCLIB_Int2 XMLQCLib_HtmlTemplate_ClientQuery :: askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		return 1;
	} 


	Tools_DynamicUTF8String XMLQCLib_HtmlTemplate_ClientQuery :: askForValue (const Tools_DynamicUTF8String & szName)
	{

		Tools_DynamicUTF8String __buffer;

		if (szName == TEMPLATE_KEY_CLNT_XML_SERVICE) {
			__buffer.ConvertFromASCII_Latin1(_serviceName,_serviceName+strlen(_serviceName));
			return __buffer;
		}
		return "";
	}

