#ifndef XMLQCLIB_HTMLTEMPLATE_INITIALFRAMEEMBEDDED_HPP
#define XMLQCLIB_HTMLTEMPLATE_INITIALFRAMEEMBEDDED_HPP
/****************************************************************************

  module      : XMLQCLib_HtmlTemplate_InitialFrameEmbedded.hpp

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
	#define TEMPLATE_EMBEDDED_INITIAL_FRAME					"XMLQCLib_Html_InitialFrameEmbedded.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_EMB_IFQ_SERVICE					"Service*"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_EMB_IFQ_SERVICE					"Service"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp"
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLQCLib_HtmlTemplate_InitialFrameEmbedded
	//===================================================================*/
	class XMLQCLib_HtmlTemplate_InitialFrameEmbedded : public Tools_Template 
	{
		public:

			XMLQCLib_HtmlTemplate_InitialFrameEmbedded(sapdbwa_WebAgent &wa,
										   				sapdbwa_HttpRequest &req);
			
			~XMLQCLib_HtmlTemplate_InitialFrameEmbedded();

		private:

			XMLQCLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

			XMLQCLIB_Char	_serviceName[512];
	}; 




#endif