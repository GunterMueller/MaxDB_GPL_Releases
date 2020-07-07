/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateDatabases.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of Databases
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

#ifndef _DBMWEB_TEMPLATEDATABASES_HPP_
#define _DBMWEB_TEMPLATEDATABASES_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Node.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateDatabases
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                list of Databases.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateDatabases : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateDatabases
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateDatabases ( sapdbwa_WebAgent & wa,
                                DBMCli_Node & oNode);
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateDatabases
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateDatabases ( );

  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
     DBMCli_Node & m_oNode;

     SAPDB_Int2 m_nRow;
     SAPDB_Int2 m_nCol;
}; 

/*! EndClass: DBMWeb_TemplateDatabases */

#endif // _DBMWEB_TEMPLATEDatabases_HPP_

