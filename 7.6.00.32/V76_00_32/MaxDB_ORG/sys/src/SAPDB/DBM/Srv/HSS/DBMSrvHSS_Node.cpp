/*!
  @file           DBMSrvHSS_Node.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to DBMServer Communication - Implementation

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Node.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#define DBMSRV_CMD_DbExecute          "db_execute"
#define DBMSRV_CMD_DbAddVolume        "db_addvolume"

#define CMD_DB_RESTARTSTANDBY    DBMSRV_CMD_DbExecute" restart standby"

/*! @brief   constructor */
DBMSrvHSS_Node :: DBMSrvHSS_Node
    (       SAPDB_Int                 nNumber,
      const Tools_DynamicUTF8String & sServer,
      const Tools_DynamicUTF8String & sDatabase,
      const Tools_DynamicUTF8String & sUser,
              Msg_List  & oMsgList )
    : DBMCli_Database(sServer, sDatabase, sUser, oMsgList, false),
      m_nNumber(nNumber),
      m_nDelayTime(0)
{
} // end DBMSrvHSS_Node :: DBMSrvHSS_Node

/*! @brief   destructor */
DBMSrvHSS_Node :: ~DBMSrvHSS_Node
    ( )
{
  if (DBMCli_Session::IsConnected()) {
    DBMCli_Session::Disconnect();
  } // end if
} // end DBMSrvHSS_Node :: ~DBMSrvHSS_Node

/*! @brief   Sets delay time */
DBMSrvHSS_Node & DBMSrvHSS_Node :: SetDelayTime
  ( SAPDB_Int8 nDelayTime )
{
  m_nDelayTime = nDelayTime;
  return *this;
} // end DBMSrvHSS_Node :: SetDelayTime


/*! @brief execute a DBMServer command */
bool DBMSrvHSS_Node::Execute
  ( const Tools_DynamicUTF8String & sCommand,
          Tools_DynamicUTF8String & sAnswer,
          DBMSrvMsg_Error         & oMessageList )
{
  bool bRc = true;
  oMessageList.ClearMessageList();

  bRc = DBMCli_Session::Execute(sCommand, oMessageList);

  if (bRc || (((int) oMessageList.ID()) < -100)) {
    sAnswer = _Tools_UTF8Ptr((const char *) DBMCli_Session::GetResult());
  } // end if

  return bRc;
} // end DBMSrvHSS_Node::Execute

bool DBMSrvHSS_Node::Execute
  ( const char      * command,
    DBMSrvMsg_Error & oMessageList )
{
    return DBMCli_Session::Execute(command, oMessageList); //execute the command and store errors in the proper member variable
}

/*! @brief start node in standby mode */
bool DBMSrvHSS_Node::DBStandby
  ( DBMSrvMsg_Error & oMessageList )
{
  // check state of database
  DBMCli_State & oState = this->GetState();
  if (!oState.Refresh(oMessageList)) {
    return false;
  } // end if
  if (oState.Value() != DBMCLI_DBSTATE_OFFLINE && oState.Value() != DBMCLI_DBSTATE_COLD) {
    oMessageList = DBMSrvMsg_Error(SDBMSG_DBMSRV_DBRUN);
    return false;
  } // end if

  // start database to cold
  if (oState.Value() == DBMCLI_DBSTATE_OFFLINE) {
    this->Start(oMessageList);
  } // end if

  // send RESTART STANDBY to standby
  if (!DBMCli_Session::Execute(CMD_DB_RESTARTSTANDBY, oMessageList)) {
    return false;
  } // end if

  return true;
} // end DBMSrvHSS_Node::DBStandby

/*! @brief add volume to node */
bool DBMSrvHSS_Node::AddVolume 
  ( const Tools_DynamicUTF8String & sAddVolume,
          DBMSrvMsg_Error         & oMessageList )
{
  Tools_DynamicUTF8String sCommand;

  sCommand.Assign(DBMSRV_CMD_DbAddVolume)
          .Append(" ")
          .Append(sAddVolume);

  // send add_volume Command to node
  if (!DBMCli_Session::Execute(sCommand, oMessageList)) {
    // send db_stop in errors
    Msg_List oDummy;
    DBMCli_Database::Stop(oDummy);
    return false;
  } // end if

  return true;
} // end DBMSrvHSS_Node::AddVolume 

  /*! @brief copy file from current node to the traget node */
bool DBMSrvHSS_Node::CopyFile
  ( const Tools_DynamicUTF8String & sFile,
          DBMSrvMsg_Error         & oMessageList )
{
  Tools_DynamicUTF8String sCommand;

  sCommand.Assign(DBMSrvCmd_HssCopyFile::m_Command)
          .Append(" ")
          .Append(DBMSrvCmd_HssCopyFile::m_ParamNode)
          .Append("=")
          .Append(RTE_ISystem::Instance().GetLocalNodeName())
          .Append(" ")
          .Append(DBMSrvCmd_HssCopyFile::m_ParamName)
          .Append("=")
          .Append(sFile);

  // send command to node
  return DBMCli_Session::Execute(sCommand, oMessageList);
} // end DBMSrvHSS_Node::CopyFile

