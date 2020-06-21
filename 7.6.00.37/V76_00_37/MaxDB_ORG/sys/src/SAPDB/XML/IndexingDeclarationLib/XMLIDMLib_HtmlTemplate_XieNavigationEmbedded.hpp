#ifndef XMLIDMLIB_HTMLTEMPLATE_XIENAVIGATIONEMBEDDED_HPP
#define XMLIDMLIB_HTMLTEMPLATE_XIENAVIGATIONEMBEDDED_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_XieNavigationEmbedded.hpp

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
	// Defines
	//===================================================================*/
	#define TEMPLATE_EMBEDDED_XIENAVIGATION			"XMLIDMLib_Html_XieNavigationEmbedded.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_EMB_NAV_SERVICE			"Service*"
	#define TEMPLATE_KEY_EMB_NAV_TAB				"Tab"
	
	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_EMB_NAV_SERVICE			"Service"
	#define TEMPLATE_VALUE_EMB_NAV_TAB				"Tab"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// class XMLIDMLib_HtmlTemplate_XieNavigationEmbedded
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_XieNavigationEmbedded : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_XieNavigationEmbedded(sapdbwa_WebAgent & wa,
												   	     sapdbwa_HttpRequest &req);
			
			~XMLIDMLib_HtmlTemplate_XieNavigationEmbedded();

		private:

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

			XMLIDMLIB_Char		_serviceName[512];
			sapdbwa_HttpRequest	&_req;
	}; 




#endif