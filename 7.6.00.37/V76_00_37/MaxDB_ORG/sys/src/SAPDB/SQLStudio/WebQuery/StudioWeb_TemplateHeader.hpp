/*!**********************************************************************

  module: StudioWeb_TemplateHeader.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  logo header for WebQuery

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

#ifndef STUDIOWEB_TEMPLATEHEADER_HPP
#define STUDIOWEB_TEMPLATEHEADER_HPP

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"


class StudioWeb_TemplateHeader : public Tools_Template 
{
  public:

    /*!-------------------------------------------------------------------------
      function:     StudioWeb_TemplateHeader
      
      description:  Constructor
      arguments:    [in] wa,webagent instance
      prototypes:   2
      -------------------------------------------------------------------------*/
     StudioWeb_TemplateHeader(sapdbwa_WebAgent & wa);

     StudioWeb_TemplateHeader(sapdbwa_WebAgent        & wa,
                              SAPDB_Bool                bLogOff,
                              Tools_DynamicUTF8String & sServer,
							  Tools_DynamicUTF8String & sDatabase,
							  Tools_DynamicUTF8String & sUser);
 
    /*!
      -------------------------------------------------------------------------
      function:     ~StudioWeb_TemplateHeader
      
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~StudioWeb_TemplateHeader();


  private:
     virtual SAPDB_Int2 askForWriteCount(const Tools_DynamicUTF8String &szName);

     virtual Tools_DynamicUTF8String askForValue(const Tools_DynamicUTF8String &szName);
                                              
  private:
    SAPDB_Bool              m_bLogOff;
    Tools_DynamicUTF8String m_sServer;
    Tools_DynamicUTF8String m_sDatabase;
    Tools_DynamicUTF8String m_sUser;
}; 

/*! EndClass: StudioWeb_TemplateHeader */

#endif // _StudioWeb_TemplateHeader_HPP_

