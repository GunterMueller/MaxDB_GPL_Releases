/*!
  @file           DBMSrvHSS_Nodes.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to DBMServer Communication - Specification

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



\endif
*/

#ifndef _DBMSrvHSS_Nodes_HPP_
#define _DBMSrvHSS_Nodes_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Node.hpp"

/*!
  @brief     Hot Standby Object with all nodes

  One object of this class represents a hot standby cluster

 */
class DBMSrvHSS_Nodes {

public:
  /*!
    @brief   Constructor 

    Constructor to create a the object
  */
  DBMSrvHSS_Nodes 
    (  );

  /*!
    @brief   Destructor

    Destructor for the object
  */
  ~DBMSrvHSS_Nodes 
    (  );

  /*!
    @brief  Sets the data for a connection.

    This function sets the dtabase instance name and the user/password 
    combination for all standby nodes

    @param  sDatabase [IN] - database name
    @param  sUser     [IN] - user name
    @param  sPassword [IN] - password
  */
  void SetConnectionData
    ( const Tools_DynamicUTF8String & sDatabase,
      const Tools_DynamicUTF8String & sUser,
      const Tools_DynamicUTF8String & sPassword );

  /*!
    @brief Create a node

    This function should be used to add a node where no database
    exists. This function executes a db_create at the node an copies
    the parameter sto the node.

    @param sNode         [IN] - name of the node
    @param sOsUser       [IN] - name of the operating system user
    @param sOsPwd        [IN] - password of the operating system user
    @param sInstallation [IN] - path to installation
    @param nDelayTime    [IN] - delay time of standby
    @param bUser         [IN] - add the user account to the service
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  bool Create
    (       Tools_DynamicUTF8String & sNode,
            Tools_DynamicUTF8String & sOsUser,
            Tools_DynamicUTF8String & sOsPwd,
            Tools_DynamicUTF8String & sInstallation,
            SAPDB_Int8                nDelayTime,
            SAPDB_Bool                bUser, 
            DBMSrvMsg_Error         & oMessageList );

  /*!
    @brief Remove a node

    This function should be used to remove a node. This function
    the drop of the database on the node.

    @param sNode         [IN] - name of the node
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  bool Remove
    ( const Tools_DynamicUTF8String & sNode,
            DBMSrvMsg_Error         & oMessageList );

  /*!
    @brief Execute a DBMServer command a node

    This function executes a DBMServer command at the specified node.

    @param sNode    [IN] name of the node
    @param sCommand [IN] command to execute
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  bool Execute
    ( const Tools_DynamicUTF8String & sNode,
      const Tools_DynamicUTF8String & sCommand,
            DBMSrvMsg_Error         & oMessageList );

  /*!
    @brief Execute a DBMServer command at a node

    This function executes a DBMServer command at the specified node.

    @param sNode    [IN]  name of the node
    @param sCommand [IN]  command to execute
    @param sAnswer  [OUT] answer of command
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  bool Execute
    ( const Tools_DynamicUTF8String & sNode,
      const Tools_DynamicUTF8String & sCommand,
            Tools_DynamicUTF8String & sAnswer,
            DBMSrvMsg_Error         & oMessageList  );
  /*!
    @brief Execute a DBMServer command at a node

    This function executes a DBMServer command at the specified node.

    @param nNode    [IN]  number of node
    @param sCommand [IN]  command to execute
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  bool Execute
    ( SAPDB_Int                       nNode,
      const Tools_DynamicUTF8String & sCommand,
            DBMSrvMsg_Error         & oMessageList  );

  /*!
    @brief Execute a DBMServer command at a node

    This function executes a DBMServer command at the specified node.

    @param nNode    [IN]  number of node
    @param sCommand [IN]  command to execute
    @param sAnswer  [OUT] answer of command
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  bool Execute
    ( SAPDB_Int                       nNode,
      const Tools_DynamicUTF8String & sCommand,
            Tools_DynamicUTF8String & sAnswer,
            DBMSrvMsg_Error         & oMessageList  );

  /*! \brief Execute a DBM Server command at all other nodes of the HSS

      This function executes a DBM Server command at all other nodes of a HSS, but not on the current
      node. The replies are added into a reply buffer. If the current database is not part of a HSS,
      nothing is executed and true is returned.

      \param command [IN]  command string to execute
      \param reply   [OUT] answer of all nodes are added to this reply
      \return true if all commands were executed successfully on all other HSS nodes, false otherwise */
  bool ExecuteOnAllOtherNodes(const char * command, DBMSrv_Reply & reply);

  /*!
    @brief check existence of node entry
    @param sNode    [IN]  name of the node
   */
  bool Exists
    ( const Tools_DynamicUTF8String & sNode )
  {DBMSrvMsg_Error oMsg; return (this->GetNode(sNode, oMsg) != NULL);}

  /*!
    @brief check existence of node entry
    @param sNode    [IN]  name of the node
   */
  bool Exists
    ( SAPDB_Int                       nNode )
  {DBMSrvMsg_Error oMsg; return (this->GetNode(nNode, oMsg) != NULL);}

  
  /*!
    @brief returns the node pointer
    @param sNode    [IN]  name of the node
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  DBMSrvHSS_Node * GetNode
    ( const Tools_DynamicUTF8String & sNode,
      DBMSrvMsg_Error               & oMessageList );

  /*!
    @brief returns the node pointer
    @param sNode    [IN]  name of the node
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  DBMSrvHSS_Node * GetNode
    ( SAPDB_Int                       nNode,
      DBMSrvMsg_Error               & oMessageList );

  /*!
    @brief bring node in mode standby
    @param sNode         [IN] - name of the node
    @param oMessageList  [OUT]- object for error messages, any content will be cleared
   */
  bool Standby
    ( const Tools_DynamicUTF8String & sNode,
            DBMSrvMsg_Error         & oMessageList );

  /*!
    @brief disribute a add volume to all standbys
    @param sAddVolume [IN]     add volume command parameters
    \param reply      [IN/OUT] reply buffer for the DBM reply, the function will just append */
  bool AddVolumeToCluster
    ( const Tools_DynamicUTF8String & sAddVolume,
            DBMSrv_Reply            & reply      );

  /*!
    @brief returns the HSS information

    @param sInfo         [OUT] - object for the info string
    @param oMessageList  [OUT] - object for error messages, any content will be cleared
   */
  bool ListHSSInfo
      ( Tools_DynamicUTF8String & sInfo,
        DBMSrvMsg_Error         & oMessageList );

    /*! \brief function for determining, if we are in a HSS or not
        \return true if database is part of a HSS, false otherwise */
    static bool isHSS();

private:
  typedef Tools_List<DBMSrvHSS_Node *> NodeList;

  /*! @brief load the nodes from configuration */
  bool LoadNodes( DBMSrvMsg_Error & oMessageList );
  /*! @brief clears the internal structures of this object */
  static void UnloadNodes
    ( NodeList                & oNodes);
  /*! @brief create the HSS ErrorMessage */
  bool HSSError(const Msg_List & oMsg);
  /*! @brief find a node by number */
  static DBMSrvHSS_Node * FindNode
    ( SAPDB_Int                 nNumber,
      const NodeList          & oNodes );
  /*! @brief find a node by name */
  static DBMSrvHSS_Node * FindNode
    ( Tools_DynamicUTF8String   sName,
      const NodeList          & oNodes );
  /*! @brief find and delete a node by name */
  static void DelNode
    ( Tools_DynamicUTF8String   sName,
      bool                      bDelete,
      NodeList                & oNodes );
  /*! @brief execute command at given node */
  bool Execute
    (       DBMSrvHSS_Node          * pNode,
      const Tools_DynamicUTF8String & sCommand,
            Tools_DynamicUTF8String & sAnswer,
            DBMSrvMsg_Error         & oMessageList  );
  /*! @brief copy parameters to specified node */
  bool CopyParams
    ( DBMSrvHSS_Node  * pNode,
      DBMSrvMsg_Error & oMessageList );


  /*! @brief Messagelist object */
//  DBMSrvMsg_Error         m_oMessageList;
  /*! @brief Name of the database */
  Tools_DynamicUTF8String m_sDatabase;
  /*! @brief Name of the DBM operator */
  Tools_DynamicUTF8String m_sUser;
  /*! @brief Password of the DBM operator */
  Tools_DynamicUTF8String m_sPassword;
  /*! @brief the node list */
  NodeList                m_oNodes;
  /*! @brief Name of the storage DLL */
  Tools_DynamicUTF8String m_sStorageDLL;
  /*! @brief Name of the official node */
  Tools_DynamicUTF8String m_sOfficialNode;

}; // end class DBMSrvHSS_Nodes


#endif /* _DBMSrvHSS_Nodes_HPP_ */
