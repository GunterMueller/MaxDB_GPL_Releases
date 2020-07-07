/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateDevspaces.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  devspaces
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

#ifndef _DBMWEB_TEMPLATEDEVSPACES_HPP_
#define _DBMWEB_TEMPLATEDEVSPACES_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Devspace.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateDevspaces
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                database devspaces.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateDevspaces : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateDevspaces
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateDevspaces ( sapdbwa_WebAgent & wa,
                                DBMCli_String & sClass,
                                DBMCli_Devspaces & oDevspaces,
                                DBMCli_DevspaceClass nClass);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateDevspaces
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateDevspaces ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetDevspaces
      -------------------------------------------------------------------------
      description:  Get the devspaces object.
      return value: Reference to the devspaces object.
      -------------------------------------------------------------------------
    */
    DBMCli_Devspaces & GetDevspaces ( ) { return m_oDevspaces; };


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
    virtual bool                    askForContinue   ( const Tools_DynamicUTF8String & szName );

    bool FindNextDevspace ( );

    bool FindSystemDevspace ( );

  private:
    DBMCli_String m_sClass;
    DBMCli_Devspaces & m_oDevspaces;
    DBMCli_DevspaceClass m_nClass;

    SAPDB_Bool m_bMirrored;

    SAPDB_Int2 m_nDevspace;
}; 

/*! EndClass: DBMWeb_TemplateDevspaces */

#endif // _DBMWEB_TEMPLATEDEVSPACES_HPP_

