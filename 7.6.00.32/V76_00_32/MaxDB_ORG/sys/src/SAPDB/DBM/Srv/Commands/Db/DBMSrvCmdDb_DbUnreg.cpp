/*!
  \file    DBMSrvCmdDb_DbUnreg.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling database registration

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

// maxdb includes
#include "RunTime/RTE_DBRegister.hpp"

// dbm include
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbDrop.hpp"

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbUnreg.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdDb_DbUnreg
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdDb_DbUnreg::DBMSrvCmdDb_DbUnreg
//-----------------------------------------------------------------------------
DBMSrvCmdDb_DbUnreg::DBMSrvCmdDb_DbUnreg()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdDb_DbUnreg::m_LongHelp ) 
{
} // end DBMSrvCmdDb_DbUnreg::DBMSrvCmdDb_DbUnreg

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdDb_DbUnreg::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbUnreg       "db_unreg"

const char * const DBMSrvCmdDb_DbUnreg::m_LongHelp=
"@command "DBMSRV_CMD_DbUnreg " You removes the registration of a database instance. In Microsoft Windows, the"
                              " relevant entry in the registry is removed; in UNIX, the Databases.ini file is"
                              " changed in directory <independent_data_path>/config."

"@preconditions  You have the server authorization InstallMgm."
               " The operational state of the database instance is OFFLINE."

"@syntax "DBMSRV_CMD_DbUnreg
"@reply OK<NL>";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbUnreg::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdDb_DbUnreg::getCommandName
    ( )
{
  return DBMSRV_CMD_DbUnreg;
} // end DBMSrvCmdDb_DbUnreg::getCommandName

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdDb_DbUnreg::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDb_DbUnreg::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmdDb_DbUnreg oCommand;

  return oCommand.run(vcontrol, command, oReply);
} // end DBMSrvCmdDb_DbUnreg::runCommand

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbUnreg::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDb_DbUnreg::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
    if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOffline) {
        return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBRUN));
    } // end if

    DBMSrvCmdDb_DbDrop dropCommand;
    return dropCommand.doDrop(vcontrol->dbname, vcontrol->dbroot, false, false, Reply, false);
} // end DBMSrvCmdDb_DbUnreg::run
