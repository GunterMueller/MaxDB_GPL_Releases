/*!**********************************************************************

  module: StudioWeb_TemplateLogon.hpp

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

#ifndef STUDIOWEB_TEMPLATELOGON_HPP
#define STUDIOWEB_TEMPLATELOGON_HPP

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        StudioWeb_TemplateLogon
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                Info Table.
  -----------------------------------------------------------------------------
*/
class StudioWeb_TemplateLogon : public Tools_Template 
{
  public:

     /*!-------------------------------------------------------------------------
		  function:     StudioWeb_TemplateLogon
		  description:  Constructor
      
		  return value: / 
      
		arguments:    [in] wa,  webagent instance
      
      -------------------------------------------------------------------------*/
     StudioWeb_TemplateLogon ( sapdbwa_WebAgent & wa );

     StudioWeb_TemplateLogon ( sapdbwa_WebAgent & wa,
							   Tools_DynamicUTF8String &sServer,
							   Tools_DynamicUTF8String &sDatabase,
							   Tools_DynamicUTF8String &sUser );


  private:
    virtual SAPDB_Int2              askForWriteCount(const Tools_DynamicUTF8String &szName);
    virtual Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String &szName);
                                              
    Tools_DynamicUTF8String m_sServer;
    Tools_DynamicUTF8String m_sDatabase;
    Tools_DynamicUTF8String m_sUser;
}; 



#endif 

