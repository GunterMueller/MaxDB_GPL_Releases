/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateUsers.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of users
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

#ifndef _DBMWEB_TEMPLATEUSERS_HPP_
#define _DBMWEB_TEMPLATEUSERS_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_User.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateUsers
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                list of Users.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateUsers : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateUsers
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateUsers ( sapdbwa_WebAgent & wa,
                            DBMCli_Users & oUsers );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateUsers
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
     ~DBMWeb_TemplateUsers ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetUsers
      -------------------------------------------------------------------------
      description:  Get the Users object.
      return value: Reference to the iUsers object.
      -------------------------------------------------------------------------
    */
    DBMCli_Users & GetUsers ( ) { return m_oUsers; };


  private:
     virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
     virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
     DBMCli_Users & m_oUsers;

     SAPDB_Int2 m_nRow;
     SAPDB_Int2 m_nCol;
}; 

/*! EndClass: DBMWeb_TemplateUsers */

#endif // _DBMWEB_TEMPLATEUSERS_HPP_

