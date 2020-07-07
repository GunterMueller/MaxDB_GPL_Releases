/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Node.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Node Class
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

#ifndef _DBMCLI_NODE_HPP_
#define _DBMCLI_NODE_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Session.hpp"
#include "DBM/Cli/DBMCli_NodeInfo.hpp"
#include "DBM/Cli/DBMCli_EnumInstallation.hpp"
#include "DBM/Cli/DBMCli_EnumDatabase.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMCli_Node
  -----------------------------------------------------------------------------
  description:  Represents a Database Node
  -----------------------------------------------------------------------------
*/
class DBMCli_Node : public DBMCli_Session
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Node
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    sServer    [IN] - server node
                    sDatabase  [IN] - database name
                    sUser      [IN] - user name
                    oMsgList  [OUT] - Object for indication of errors
      prototypes:   3
      -------------------------------------------------------------------------
     */
     DBMCli_Node ( );

     DBMCli_Node ( const DBMCli_String        & sServer,
                         Msg_List & oMsgList,
                   const SAPDB_Bool             bConnect = true );

     DBMCli_Node ( const DBMCli_String        & sServer,
                   const DBMCli_String        & sDatabase,
                   const DBMCli_String        & sUser,
                         Msg_List & oMsgList,
                   const SAPDB_Bool             bConnect = true );
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Node
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Node ( );

    /*!
      -------------------------------------------------------------------------
      function:     NodeInfo
      -------------------------------------------------------------------------
      description:  Get the nodeinfo object.
      return value: Reference to a nodeinfo object.
      -------------------------------------------------------------------------
    */
    DBMCli_NodeInfo & NodeInfo ( ) { return m_oNodeInfo; }; 
  
    /*!
      -------------------------------------------------------------------------
      function:     EnumerateInstallations
      -------------------------------------------------------------------------
      description:  Enumerate the installations on the current node.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool EnumerateInstallations ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     EnumerateDatabases
      -------------------------------------------------------------------------
      description:  Enumerate the databases on the current node.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool EnumerateDatabases ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     DatabaseIndexByName
      -------------------------------------------------------------------------
      description:  Get a index from a database by the name.
      return value: Index of database in array.
      -------------------------------------------------------------------------
    */
    SAPDB_Int DatabaseIndexByName ( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     CreateDatabase
      -------------------------------------------------------------------------
      description:  Create a databases instance.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool CreateDatabase ( const DBMCli_String & sServerName,
                                const DBMCli_String & sDatabaseName,
                                const DBMCli_String & sUserName,
                                const DBMCli_String & sPassword,
                                const DBMCli_String & sOSUser,
                                const DBMCli_String & sOSPassword,
                                const DBMCli_String & sInstRoot,
                                Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     DropDatabase
      -------------------------------------------------------------------------
      description:  Drop a databases instance.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool DropDatabase ( const DBMCli_String & sDatabaseName,
                              const DBMCli_String & sUserName,
                              const DBMCli_String & sPassword,
                              Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     EnumInstallation
      -------------------------------------------------------------------------
      description:  Get a array of enum installaion object.
      return value: Reference to a array of enum installation objects.
      -------------------------------------------------------------------------
    */
    DBMCli_EnumInstallationArray & EnumInstallationArray ( ) { return m_aEnumInstallation; }; 
    
    /*!
      -------------------------------------------------------------------------
      function:     EnumDatabase
      -------------------------------------------------------------------------
      description:  Get a array of enum database object.
      return value: Reference to a array of enum database objects.
      -------------------------------------------------------------------------
    */
    DBMCli_EnumDatabaseArray & EnumDatabaseArray ( ) { return m_aEnumDatabase; }; 


  private:
    DBMCli_NodeInfo              m_oNodeInfo;
    DBMCli_EnumInstallationArray m_aEnumInstallation;
    DBMCli_EnumDatabaseArray     m_aEnumDatabase;
}; 

/*! EndClass: DBMCli_Node */

#endif // _DBMCLI_NODE_HPP_

