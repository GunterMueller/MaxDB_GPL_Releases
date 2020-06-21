/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Session.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  DBM-Server Session Class
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

#ifndef _DBMCLI_SESSION_HPP_
#define _DBMCLI_SESSION_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "gsp00.h"
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Result.hpp"
#include "DBM/Cli/DBMCli_Version.hpp"

/*!  -----------------------------------------------------------------------------
  Class:        DBMCli_Session
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a dbm server session.
  -----------------------------------------------------------------------------
*/
class DBMCli_Session
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Session
      -------------------------------------------------------------------------
      description:  Constructor

                    This constructor connects to the DBMServer of the
                    specified database at a specified node.

      arguments:    sServer     [IN]  - server node
                    sDatabase [IN]  - database name
                    sUser         [IN]  - user name
                    oMsgList      [OUT] - Object for indication of errors
                    bConnect      [IN]  - Connect immediately or not

      prototypes:   5
      -------------------------------------------------------------------------
    */
    DBMCli_Session ( );

    DBMCli_Session ( const DBMCli_String        & sServer,
                           Msg_List & oMsgList,
                     const SAPDB_Bool             bConnect = true);

    DBMCli_Session ( const DBMCli_String        & sServer,
                     const DBMCli_String        & sDatabase,
                           Msg_List & oMsgList,
                     const SAPDB_Bool             bConnect = true);

    DBMCli_Session ( const DBMCli_String        & sServer,
                     const DBMCli_String        & sDatabase,
                     const DBMCli_String        & sUser,
                           Msg_List & oMsgList,
                     const SAPDB_Bool             bConnect = true);

    DBMCli_Session ( const DBMCli_String        & sServer,
                     const DBMCli_String        & sDatabase,
                     const DBMCli_String        & sUser,
                     const DBMCli_String        & sInstRoot,
                           Msg_List & oMsgList,
                     const SAPDB_Bool             bConnect = true);

    /*!
      -------------------------------------------------------------------------
      function:    ~DBMCli_Session
      -------------------------------------------------------------------------
      description: Destructor Disconnect from the DBMServer.
      -------------------------------------------------------------------------
    */
    ~DBMCli_Session ( );

    /*!
      -------------------------------------------------------------------------
      function:     Connect
      -------------------------------------------------------------------------
      description:  Connect to DBMServer

                    This function creates a session to the DBMServer. Is there
                    an open session the old session will be closed an all
                    session data are lost.

      arguments:    oMsgList [OUT] - Object for indication of errors

      return value: - True, if successfull communication with DBMServer.
                    - False, if there an error in communication
                      Check the oMsgList for more information.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Connect ( Msg_List &  oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Disconnect
      -------------------------------------------------------------------------
      description:  Disconnect from DBMServer

                    This function destryos the session to the DBMServer. All
                    data of the open session are lost.
      -------------------------------------------------------------------------
    */
    void Disconnect ( );

    /*!
      -------------------------------------------------------------------------
      function:     IsConnected
      -------------------------------------------------------------------------
      description:  connection state

      return value: true, if connected
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsConnected ( );

    /*!
      -------------------------------------------------------------------------
      function:     Execute
      -------------------------------------------------------------------------
      description:  command execution

                    This function executes a DBMServer command an provide
                    the answer.

      arguments:    aCommand [IN]  - DBMServer commadn with parameters
                    oMsgList [OUT] - Object for indication of errors
                    szAnswer [OUT] - pointer to answer

      return value: - True, if successfull communication with DBMServer.
                      Check anyway the aEventList for a DBMServer error.
                    - False, if there an error in communication
                      Check the aEventList for more information.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Execute ( const DBMCli_String        & sCommand,
                               Msg_List & oMsgList );

    /*!
      ----------------------------------------------------------------------
      function:     ServerName
      -------------------------------------------------------------------------
      description:  active node name

      return value: the node name
      -------------------------------------------------------------------------
    */
    DBMCli_String & ServerName ( ) { return m_sServer; };

    /*!
      -------------------------------------------------------------------------
      function:     DatabaseName
      -------------------------------------------------------------------------
      description:  active database name

      return value: the database name
      -------------------------------------------------------------------------
    */
    DBMCli_String & DatabaseName ( ) { return m_sDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     UserName
      -------------------------------------------------------------------------
      description:  active user name

      return value: the user name
      -------------------------------------------------------------------------
    */
    DBMCli_String & UserName ( ) { return m_sUser; };

    /*!
      -------------------------------------------------------------------------
      function:     GetResult
      -------------------------------------------------------------------------
      description:  get the last result object

      return value: the result object
      -------------------------------------------------------------------------
    */
    DBMCli_Result & GetResult ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetVersion
      -------------------------------------------------------------------------
      description:  Retrieve version information.

      return value: the version object
      -------------------------------------------------------------------------
    */
    DBMCli_Version & GetVersion ( ) { return m_oVersion; };


    void SetServerName ( const DBMCli_String & sServer ) { m_sServer = sServer; };
    void SetDatabaseName ( const DBMCli_String & sDatabase ) { m_sDatabase = sDatabase; };
    void SetUserName ( const DBMCli_String & sUser ) { m_sUser = sUser; };
    void SetInstRoot ( const DBMCli_String & sInstRoot) { m_sInstRoot = sInstRoot; };

  private:
    void          * m_pSession;

    DBMCli_Result * m_pResult;

    SAPDB_Int4      m_nPacketSize;
    DBMCli_String   m_sServer;
    DBMCli_String   m_sDatabase;
    DBMCli_String   m_sUser;
    DBMCli_String   m_sInstRoot;

    DBMCli_Version  m_oVersion;

    SAPDB_Bool DBMConnect ( const DBMCli_String  & sServer,
                            const DBMCli_String  & sDatabase,
                            const DBMCli_String  & sUser,
                            const DBMCli_String  & sInstRoot,
                            Msg_List & oMsgList );

    void DBMDisconnect ( );
}; 

/*! EndClass: DBMCli_Session */

#endif // _DBMCLI_SESSION_HPP_

