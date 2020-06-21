/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_NodeInfo.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Node Info Class
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

#ifndef _DBMCLI_NODEINFO_HPP_
#define _DBMCLI_NODEINFO_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Version.hpp"

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Node;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_NodeInfo
  -----------------------------------------------------------------------------
  description: Handle node specific information.
  -----------------------------------------------------------------------------
*/
class DBMCli_NodeInfo
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_NodeInfo
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    sName     [IN]  - name of database
                    sInstRoot [IN]  - installation root
                    sVersion  [IN]  - version string
                    sKernel   [IN]  - kernel (fast,quick,slow)
                    sState    [IN]  - state (offline,running)
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_NodeInfo ( );

    DBMCli_NodeInfo ( const DBMCli_String & sVersion,
                      const DBMCli_String & sBuild,
                      const DBMCli_String & sOS,
                      const DBMCli_String & sInstRoot,
                            SAPDB_Bool      bLogon,
                      const DBMCli_String & sCode,
                      const DBMCli_String & sSwap);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_NodeInfo
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_NodeInfo ( );
      
    /*!
      -------------------------------------------------------------------------
      function:     SetNode
      -------------------------------------------------------------------------
      description:  Set the node member.
      -------------------------------------------------------------------------
    */
    void SetNode ( DBMCli_Node * p ) { m_pNode = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetNode
      -------------------------------------------------------------------------
      description:  Get the node member.
      return value: Reference to the node object.
      -------------------------------------------------------------------------
    */
    DBMCli_Node & GetNode ( ) { return * m_pNode; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the node info.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refreshes the infos of the node.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Version
      -------------------------------------------------------------------------
      description:  Retrieve version information.
      -------------------------------------------------------------------------
    */
    DBMCli_Version & Version ( ) { return m_oVersion; };

    /*!
      -------------------------------------------------------------------------
      function:     Build
      -------------------------------------------------------------------------
      description:  Retrieve the build number.
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Build ( ) { return m_sBuild; };

    /*!
      -------------------------------------------------------------------------
      function:     Os
      -------------------------------------------------------------------------
      description:  Retrieve the operating system.
      -------------------------------------------------------------------------
    */
    const DBMCli_String & OS ( ) { return m_sOS; };

    /*!
      -------------------------------------------------------------------------
      function:     InstRoot
      -------------------------------------------------------------------------
      description:  Retrieve installation directory.
      -------------------------------------------------------------------------
    */
    const DBMCli_String & InstRoot ( ) { return m_sInstRoot; };

    /*!
      -------------------------------------------------------------------------
      function:     Logon
      -------------------------------------------------------------------------
      description:  Retrieve operating system logon state.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Logon ( ) { return m_bLogon; };

    /*!
      -------------------------------------------------------------------------
      function:     Code
      -------------------------------------------------------------------------
      description:  Retrieve the used character set.
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Code ( ) { return m_sCode; };

    /*!
      -------------------------------------------------------------------------
      function:     Swap
      -------------------------------------------------------------------------
      description:  Retrieve the type of interger swapping.
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Swap ( ) { return m_sSwap; };

    /*!  
      EndChapter: Properties
    */

  private:
    void AssignProp ( const DBMCli_String & sProp,
                      const DBMCli_String & sVal );

    void AssignValBool ( const DBMCli_String & sVal,
                               SAPDB_Bool    * pVal );

  private:
    DBMCli_Node *   m_pNode;

    DBMCli_Version  m_oVersion;
    DBMCli_String   m_sBuild;
    DBMCli_String   m_sOS;
    DBMCli_String   m_sInstRoot;
    SAPDB_Bool      m_bLogon;
    DBMCli_String   m_sCode;
    DBMCli_String   m_sSwap;
}; 

/*! EndClass: DBMCli_NodeInfo */

#endif // _DBMCLI_ENUMDATABASE_HPP_

