/*!
  \file    DBMSrvCmdSQL_SqlRelease.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command sql_release

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

#include "DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp"

#include "SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlRelease.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdSQL_SqlRelease
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdSQL_SqlRelease::DBMSrvCmdSQL_SqlRelease
//-----------------------------------------------------------------------------
DBMSrvCmdSQL_SqlRelease::DBMSrvCmdSQL_SqlRelease()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdSQL_SqlRelease::m_LongHelp ) 
{
} // end DBMSrvCmdSQL_SqlRelease::DBMSrvCmdSQL_SqlRelease

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdSQL_SqlRelease::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_SqlRelease       "sql_release"

const char * const DBMSrvCmdSQL_SqlRelease::m_LongHelp=
"@command "DBMSRV_CMD_SqlRelease" You end the active SQL session. All assigned resources are released."
"@preconditions You have the server authorization AccessSQL."
"@syntax "DBMSRV_CMD_SqlRelease
"@reply OK";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlRelease::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdSQL_SqlRelease::getCommandName
    ( )
{
  return DBMSRV_CMD_SqlRelease;
} // end DBMSrvCmdSQL_SqlRelease::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlRelease::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdSQL_SqlRelease::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::release
    ( DBMSrvKnl_GlobalSessions::SessionSQL,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdSQL_SqlRelease::run
