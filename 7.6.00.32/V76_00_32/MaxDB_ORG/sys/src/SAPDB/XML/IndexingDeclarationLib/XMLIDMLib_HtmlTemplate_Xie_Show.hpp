#ifndef XMLIDMLIB_HTMLTEMPLATE_XIE_SHOW_HPP
#define XMLIDMLIB_HTMLTEMPLATE_XIE_SHOW_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_Xie_Show.hpp

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
	#define TEMPLATE_XIE_SHOW								"XMLIDMLib_Html_Xie_Show.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_XIE_SHOW_SERVICE					"Service*"
	#define TEMPLATE_KEY_XIE_SHOW_XIE						"Xie*"
	#define TEMPLATE_KEY_XIE_SHOW_XIEID						"XIEID"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_XIE_SHOW_SERVICE					"Service"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEID					"XIEID"
	#define TEMPLATE_VALUE_XIE_SHOW_XIENAME					"XieName"
	#define TEMPLATE_VALUE_XIE_SHOW_XIENODE					"XieNode"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEINSTROOT				"XieInstRoot"
	#define TEMPLATE_VALUE_XIE_SHOW_XIELOGFILE				"XieLogFile"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEDESC					"XieDesc"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEISNAME				"XieISName"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEISDESC				"XieISDesc"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEISDOCSTORE			"XieISDocStore"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEISIDXSTORE			"XieISIdxStore"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEILNAME				"XieILName"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEILVERSION			"XieILVersion"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEILDESC				"XieILDesc"
	#define TEMPLATE_VALUE_XIE_SHOW_XIEALIVE				"XieAlive"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_SessionPool_Show
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_Xie_Show : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_Xie_Show(sapdbwa_WebAgent & wa, sapdbwa_HttpRequest &req,XMLIMAPI_Handle &xml);

			~XMLIDMLib_HtmlTemplate_Xie_Show();

		private:

			XMLIMAPI_Handle				&_xml;
			sapdbwa_HttpRequest			&_req;
			sapdbwa_WebAgent			&_wa;

			XMLIMAPI_XieAdmin			_xie;
			XMLIDMLIB_Char				_serviceName[512];

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

			void getXie();
	}; 




#endif