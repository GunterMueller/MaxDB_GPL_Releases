#ifndef XMLIDMLIB_HTMLTEMPLATE_XMLNAVIGATION_HPP
#define XMLIDMLIB_HTMLTEMPLATE_XMLNAVIGATION_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_XmlNavigation.hpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-26  14:03
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
	#define TEMPLATE_XMLNAVIGATION		"XMLIDMLib_Html_XmlNavigation.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_XML_NAVI_SERVICE					"Service"
	#define TEMPLATE_KEY_XML_NAVI_XMLINDEX					"XMLIndex"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_XML_NAVI_SERVICE					"Service"
	#define TEMPLATE_VALUE_XML_NAVI_XMLINDEX				"XMLIndex"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_XmlNavigation
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_XmlNavigation : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_XmlNavigation(sapdbwa_WebAgent & wa,
										   		 sapdbwa_HttpRequest &req,
												 XMLIMAPI_Handle	&xml);

			~XMLIDMLib_HtmlTemplate_XmlNavigation();

		private:

			XMLIMAPI_Handle			&_xml;
			sapdbwa_WebAgent		&_wa;

			XMLIMAPI_XmlIndexList	_xmlIndexList;
			XMLIDMLIB_UInt4			_xmlIndexCount;
			XMLIMAPI_Bool			_firstXmlIndex;

			XMLIDMLIB_Char			_string[4096];
			XMLIDMLIB_Char			_serviceName[512];

			void getXmlIndices();

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

	}; 






#endif