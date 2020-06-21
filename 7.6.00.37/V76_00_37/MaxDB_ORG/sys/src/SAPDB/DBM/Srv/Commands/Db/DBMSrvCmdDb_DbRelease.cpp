/*!
  \file    DBMSrvCmdDb_DbRelease.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command db_release

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

#include "hcn31.h"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp"

#include "SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbRelease.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdDb_DbRelease
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdDb_DbRelease::DBMSrvCmdDb_DbRelease
//-----------------------------------------------------------------------------
DBMSrvCmdDb_DbRelease::DBMSrvCmdDb_DbRelease()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdDb_DbRelease::m_LongHelp ) 
{
} // end DBMSrvCmdDb_DbRelease::DBMSrvCmdDb_DbRelease

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdDb_DbRelease::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbRelease       "db_release"

const char * const DBMSrvCmdDb_DbRelease::m_LongHelp=
"@command "DBMSRV_CMD_DbRelease" You end the active database session. All assigned resources are released."
"@preconditions You have the server authorizations AccessAdmin, AccessSQL, or DBInfoRead."
"@syntax "DBMSRV_CMD_DbRelease
"@reply OK";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbRelease::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdDb_DbRelease::getCommandName
    ( )
{
  return DBMSRV_CMD_DbRelease;
} // end DBMSrvCmdDb_DbRelease::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbRelease::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDb_DbRelease::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  cn31FreeBackupSession(vcontrol);
  return DBMSrvKnl_DbAndSqlCommands::release
    ( DBMSrvKnl_GlobalSessions::SessionDatabase,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdDb_DbRelease::run
