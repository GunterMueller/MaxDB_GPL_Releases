/*!
  \file    DBMSrvCmd_DbSpeed.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command db_speed

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"

#include "SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbSpeed.hpp"

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmd_DbSpeed::DBMSrvCmd_DbSpeed
//-----------------------------------------------------------------------------
DBMSrvCmd_DbSpeed::DBMSrvCmd_DbSpeed()
    : DBMSrv_Command( DBMSrv_Command::KeyDbSpeed, false, m_LongHelp ) 
{
} // end DBMSrvCmd_DbSpeed::DBMSrvCmd_DbSpeed

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmd_DbSpeed::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DBSpeed        "db_speed"
#define DBMSRV_TXT_Speed          "Speed"

const char * const DBMSrvCmd_DbSpeed::m_LongHelp=
    "@command db_speed You display the variant of the database kernel of the current database instance."
    "@preconditions Prerequisites You have the server authorization DBInfoRead."
                                 "The database instance is in operational state ONLINE or ADMIN."

    "@syntax "DBMSRV_CMD_DBSpeed
    "@reply OK<NL>"
            DBMSRV_TXT_Speed"<NL>"
           "<speed><NL>"
           "@replyvalue <speed>         variant of the database kernel (e.g. FAST, SLOW, TEST)";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmd_DbSpeed::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_DbSpeed::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmd_DbSpeed oVersion;

  return oVersion.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmd_DbSpeed::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_DbSpeed::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  Tools_DynamicUTF8String szOption;

  DBMSrvKnl_State oState(vcontrol->dbname);
  if (!oState.DBMMsg().IsEmpty()) {
    return Reply.startWithMessageList(oState.DBMMsg());
  } // end if

  if (oState.State() == DBMSrvKnl_State::StateOffline) {
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
  } // end if

  Reply.startWithOK();
  Reply.appendLines(DBMSRV_TXT_Speed, DBMSrvKnl_State::getSpeedAsString(oState.Speed()));

  return Reply.ReturnCode();
} // end DBMSrvCmd_DbSpeed::run
