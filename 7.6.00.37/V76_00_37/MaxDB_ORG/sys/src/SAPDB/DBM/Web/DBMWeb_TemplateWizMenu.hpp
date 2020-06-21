/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateWizMenu.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Wizard Menu
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------


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




*/

#ifndef _DBMWEB_TEMPLATEWIZMENU_HPP_
#define _DBMWEB_TEMPLATEWIZMENU_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "DBM/Cli/DBMCli_Wizard.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateWizMenu
  -----------------------------------------------------------------------------
  description:  Wizard Menu.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateWizMenu : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateWizMenu
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateWizMenu ( sapdbwa_WebAgent & wa,
                              SAPDB_Int nStep,
                              SAPDB_Int nState );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateWizMenu
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateWizMenu ( );


  private:
     virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
     virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );

  private:
     void GetStateValue( SAPDB_Int nState );
     
  private:

    SAPDB_Int m_nStep;
    SAPDB_Int m_nState;

    DBMCli_String m_sValue;
}; 

/*! EndClass: DBMWeb_TemplateWizMenu */

#endif // _DBMWEB_TEMPLATEWIZMENU_HPP_

