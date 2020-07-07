/*!**********************************************************************

  module: StudioWeb_Template.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  logon mask class for WebQuery

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




************************************************************************/

#ifndef STUDIOWEB_TEMPLATEPARAMQUERY_HPP
#define STUDIOWEB_TEMPLATEPARAMQUERY_HPP

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include "ToolsCommon/Tools_Array.hpp"
#include "ToolsCommon/Tools_Template.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"


/*!
  -----------------------------------------------------------------------------
  Class:        StudioWeb_TemplateParamQuery
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                Info Table.
  -----------------------------------------------------------------------------
*/
class StudioWeb_TemplateParamQuery : public Tools_Template 
{
  public:

     /*!-------------------------------------------------------------------------
		  function:     StudioWeb_WebQuery
		  description:  Constructor
      
		  return value: / 
      
		  arguments:    [in] wa,  webagent instance
      
      -------------------------------------------------------------------------*/
     StudioWeb_TemplateParamQuery (sapdbwa_WebAgent & wa);

	 void setParam(const Tools_DynamicUTF8String &sParamText);

	 void setSQLStatement(const Tools_DynamicUTF8String &sStatement){m_sStatement = sStatement;}

	 /*!-------------------------------------------------------------------------
		function:     resetStartValues
    
		description:  function to reset start values of the template
    -------------------------------------------------------------------------*/
	void resetStartValues ();


  private:
    virtual SAPDB_Int2            askForWriteCount(const Tools_DynamicUTF8String &szName);
    virtual Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String &szName);
                                              
                                                  
    SAPDB_Int2 m_nParams;
	SAPDB_Int2 m_nCurrentParamText;
	SAPDB_Int2 m_nCurrentParamInputName;
	SAPDB_Int2 m_nCurrentParam;
	

    Tools_DynamicUTF8String  m_sStatement;

	typedef Tools_Array<Tools_DynamicUTF8String> TextMap;     

    TextMap  m_ParamText;
	TextMap  m_ParamInputName;

}; 



#endif 