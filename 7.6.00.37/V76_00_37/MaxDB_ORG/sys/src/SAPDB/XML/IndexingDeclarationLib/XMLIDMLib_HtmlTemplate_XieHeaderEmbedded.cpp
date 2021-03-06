/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_XieHeaderEmbedded.cpp

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
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XieHeaderEmbedded.hpp"



	XMLIDMLib_HtmlTemplate_XieHeaderEmbedded::XMLIDMLib_HtmlTemplate_XieHeaderEmbedded(sapdbwa_WebAgent &wa,
																 				 sapdbwa_HttpRequest &req) 
		                  : Tools_Template ( wa, (const unsigned char*)TEMPLATE_EMBEDDED_XIEHEADER )
	{ 
		strcpy(_serviceName,wa.GetServiceName());
	} 


	XMLIDMLib_HtmlTemplate_XieHeaderEmbedded::~XMLIDMLib_HtmlTemplate_XieHeaderEmbedded()
	{
	}

	XMLIDMLIB_Int2 XMLIDMLib_HtmlTemplate_XieHeaderEmbedded::askForWriteCount(const Tools_DynamicUTF8String & szName)
	{
		return 1;
	} 


	Tools_DynamicUTF8String XMLIDMLib_HtmlTemplate_XieHeaderEmbedded::askForValue (const Tools_DynamicUTF8String & szName)
	{
		return (Tools_DynamicUTF8String::ConstPointer)_serviceName;
	}

