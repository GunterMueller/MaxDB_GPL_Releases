/*!
  @file           DBMSrvHSS_Node.hpp
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

#ifndef _DBMSrvHSS_Node_HPP_
#define _DBMSrvHSS_Node_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/DBM/Cli/DBMCli_Database.hpp"
#include "SAPDB/DBM/Srv/Commands/DBMSrvCmd_HssCopyFile.hpp"

/*!
  @brief     Node Object

  One object of this class represents a hot standby node
 */
class DBMSrvHSS_Node : public DBMCli_Database
{
private:

  /*!
    @brief   Constructor 
    Constructor to create a the object
    @param nNumber   [IN]  number of node
    @param sServer   [IN]  name of node
    @param sDatabase [IN]  name of database instance
    @param sUser     [IN]  user/password combination
    @param oMsgList  [OUT] messagelist object for any messages
  */
  DBMSrvHSS_Node 
    (       SAPDB_Int                 nNumber,
      const Tools_DynamicUTF8String & sServer,
      const Tools_DynamicUTF8String & sDatabase,
      const Tools_DynamicUTF8String & sUser,
              Msg_List  & oMsgList );
  
  /*!
    @brief   destructor 
  */
  ~DBMSrvHSS_Node
    (  );

  /*! @brief Sets delay time */
  DBMSrvHSS_Node & SetDelayTime
    ( SAPDB_Int8 nDelayTime );

  /*! @brief Gets delay time */
  SAPDB_Int8 GetDelayTime
    (  ) {return m_nDelayTime;}

  /*! @brief execute the command */
  bool Execute
    ( const Tools_DynamicUTF8String & sCommand,
            Tools_DynamicUTF8String & sAnswer,
            DBMSrvMsg_Error         & oMessageList );

  /*! \brief execute a command
      \param command [IN]
      \return true, if command was executed successfully, false otherwise */
  bool Execute
    ( const char      * command,
      DBMSrvMsg_Error & oMessageList );

  /*! @brief start node in standby mode */
  bool DBStandby 
    ( DBMSrvMsg_Error & oMessageList );

  /*! @brief add volume to node */
  bool AddVolume 
    ( const Tools_DynamicUTF8String & sAddVolume,
            DBMSrvMsg_Error         & oMessageList );

  /*! @brief copy file from current node to the traget node */
  bool CopyFile
    ( const Tools_DynamicUTF8String & sFile,
            DBMSrvMsg_Error         & oMessageList );

  /*! @brief get node number */
  SAPDB_Int GetNumber() {return m_nNumber;}

  /*! @brief get node name */
  const char * GetName() {return DBMCli_Session::ServerName();}

  /*! @brief Delay time for node */
  SAPDB_Int8   m_nDelayTime;
  /*! @brief Number of node */
  SAPDB_Int    m_nNumber;

  friend class DBMSrvHSS_Nodes;

}; // end DBMSrvHSS_Node

#endif /* _DBMSrvHSS_Node_HPP_ */
