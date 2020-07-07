/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateLogon.hpp
  -----------------------------------------------------------------------------
  responsible:  
  special area: Database Manager WebAgent Extension
  description:  a class for the logon mask of the Web DBM
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

#ifndef _DBMWEB_TEMPLATELOGON_HPP_
#define _DBMWEB_TEMPLATELOGON_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "Messages/Msg_List.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_TemplateLogon
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                Info Table.
  -----------------------------------------------------------------------------
*/
class DBMWeb_TemplateLogon : public Tools_Template 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_TemplateLogon
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    wa [IN] - webagent instance
      prototypes:   2
      -------------------------------------------------------------------------
     */
     DBMWeb_TemplateLogon ( sapdbwa_WebAgent & wa );

     DBMWeb_TemplateLogon ( sapdbwa_WebAgent & wa,
                            DBMCli_String & sServer,
                            DBMCli_String & sDatabase,
                            DBMCli_String & sUser);

     DBMWeb_TemplateLogon ( sapdbwa_WebAgent & wa,
                            DBMCli_String & sServer,
                            DBMCli_String & sDatabase,
                            DBMCli_String & sUser,
                            const Msg_List & oMsgList);


  private:
    virtual SAPDB_Int2              askForWriteCount ( const Tools_DynamicUTF8String & szName );
    virtual Tools_DynamicUTF8String askForValue      ( const Tools_DynamicUTF8String & szName );

    Msg_List    m_oMsgList;
    Msg_List *  m_pCurrentMsg;
                                              
    DBMCli_String m_sServer;
    DBMCli_String m_sDatabase;
    DBMCli_String m_sUser;
}; 

/*! EndClass: DBMWeb_TemplateLogon */

#endif // _DBMWEB_TEMPLATELOGON_HPP_

