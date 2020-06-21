#ifndef XMLIDMLIB_HTMLTEMPLATE_XIE_SHOWALL_HPP
#define XMLIDMLIB_HTMLTEMPLATE_XIE_SHOWALL_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_Xie_ShowAll.hpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-07-12  13:45
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
	#define TEMPLATE_XIE_SHOWALL							"XMLIDMLib_Html_Xie_ShowAll.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_XIE_SHOWALL_SERVICE					"Service*"
	#define TEMPLATE_KEY_XIE_SHOWALL_XIE						"Xie*"
	#define TEMPLATE_KEY_XIE_SHOWALL_XIE_NODES					"XieNodes*"
	#define TEMPLATE_KEY_XIE_SHOWALL_XIE_NODE					"XieNode"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_XIE_SHOWALL_SERVICE					"Service"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIENAME					"XieName"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIENODE					"XieNode"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIEDESC					"XieDesc"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIEISNAME				"XieISName"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIEISDOCSTORE			"XieISDocStore"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIEISIDXSTORE			"XieISIdxStore"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIEILNAME				"XieILName"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIEILVERSION				"XieILVersion"
	#define TEMPLATE_VALUE_XIE_SHOWALL_XIEALIVE					"XieAlive"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_Xie_ShowAll
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_Xie_ShowAll : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_Xie_ShowAll(sapdbwa_WebAgent & wa, sapdbwa_HttpRequest &req, XMLIMAPI_Handle &xml);

			~XMLIDMLib_HtmlTemplate_Xie_ShowAll();

		private:

			XMLIMAPI_Handle			&_xml;
			sapdbwa_WebAgent		&_wa;

			XMLIDMLIB_Char			_string[4096];
			XMLIDMLIB_Char			_serviceName[512];

			XMLIMAPI_XieAdminList	_xieList;
			XMLIDMLIB_UInt4			_xieCount;
			XMLIMAPI_XieAdmin		*_currXie;
			XMLIMAPI_Bool			_firstXie;
			XMLIMAPI_Bool			_endOfList;
			XMLIMAPI_Bool			_groupChange;
			XMLIDMLIB_Char			_lastXieNode[512];

			void getXieList();

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			bool askForContinue ( const Tools_DynamicUTF8String & szName );

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);
	}; 




#endif