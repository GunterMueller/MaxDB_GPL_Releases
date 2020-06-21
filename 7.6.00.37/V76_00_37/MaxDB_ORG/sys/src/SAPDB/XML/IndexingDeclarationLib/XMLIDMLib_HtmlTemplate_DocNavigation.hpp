#ifndef XMLIDMLIB_HTMLTEMPLATE_DOCNAVIGATION_HPP
#define XMLIDMLIB_HTMLTEMPLATE_DOCNAVIGATION_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_DocNavigation.hpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-21  17:10
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
	#define TEMPLATE_DOCNAVIGATION							"XMLIDMLib_Html_DocNavigation.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_DOC_NAVI_SERVICE					"Service"
	#define TEMPLATE_KEY_DOC_NAVI_DOCCLASS					"DocClass"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_DOC_NAVI_SERVICE					"Service"
	#define TEMPLATE_VALUE_DOC_NAVI_DOCCLASS				"DocClass"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_DocNavigation
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_DocNavigation : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_DocNavigation(sapdbwa_WebAgent & wa,
										   		 sapdbwa_HttpRequest &req,
												 XMLIMAPI_Handle	&xml);

			~XMLIDMLib_HtmlTemplate_DocNavigation();

		private:


			XMLIMAPI_Handle			&_xml;
			sapdbwa_WebAgent		&_wa;

			XMLIDMLIB_Char			_string[4096];
			XMLIDMLIB_Char			_serviceName[512];

			XMLIMAPI_DocClassList	_docClassList;
			XMLIDMLIB_UInt4			_docClassCount;
			XMLIMAPI_Bool			_firstDocClass;

			void getDocClasses();

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

	}; 


#endif