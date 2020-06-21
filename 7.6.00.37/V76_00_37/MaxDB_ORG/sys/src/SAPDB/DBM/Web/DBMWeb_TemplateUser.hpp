/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateUser.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  User
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

#ifndef _DBMWEB_TEMPLATEUSER_HPP_
#define _DBMWEB_TEMPLATEUSER_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_User.hpp"
/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum  
{ 
	DBMWEB_TEMPLUSER_NEW  = 0,
	DBMWEB_TEMPLUSER_EDIT = 1,
	DBMWEB_TEMPLUSER_CHANGEPWD = 2,
	DBMWEB_TEMPLUSER_CHANGEPWD2 = 3
} DBMWeb_TemplUser_Mode;


/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateUser
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with  
                a user.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateUser : public Tools_Template
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateUser
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN]  - webagent instance
      -------------------------------------------------------------------------
    */

    DBMWeb_TemplateUser ( sapdbwa_WebAgent & wa,
                          DBMCli_User & oUser,
                          DBMWeb_TemplUser_Mode nMode,
                          const DBMCli_String & sBackURL );
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_TemplateUser
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_TemplateUser ( );

  private:
     virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
     virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );
                                              
  private:
    DBMCli_User & m_oUser;
    DBMWeb_TemplUser_Mode m_nMode; 
    DBMCli_String m_sBackURL;

    SAPDB_Int     m_nRight;
};

/*! EndClass: DBMWeb_TemplateUser */

#endif // _DBMWEB_TEMPLATEUSER_HPP_

