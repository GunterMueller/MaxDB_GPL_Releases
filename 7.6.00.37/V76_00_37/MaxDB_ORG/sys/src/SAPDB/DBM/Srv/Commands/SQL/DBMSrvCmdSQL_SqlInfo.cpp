/*!
  \file    DBMSrvCmdSQL_SqlInfo.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command sql_info

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

#include "SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlInfo.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdSQL_SqlInfo
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdSQL_SqlInfo::DBMSrvCmdSQL_SqlInfo
//-----------------------------------------------------------------------------
DBMSrvCmdSQL_SqlInfo::DBMSrvCmdSQL_SqlInfo()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdSQL_SqlInfo::m_LongHelp ) 
{
} // end DBMSrvCmdSQL_SqlInfo::DBMSrvCmdSQL_SqlInfo

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdSQL_SqlInfo::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_SqlInfo       "sql_info"

const char * const DBMSrvCmdSQL_SqlInfo::m_LongHelp=
"@command "DBMSRV_CMD_SqlInfo" You transfer the specified SELECT statement to the database instance. "
                             "The system displays the column list of the result. For more "
                             "information about SQL statements, see the Reference Manual and "
                             "SQL Tutorial."
"@preconditions You have the server authorization AccessSQL."
"@syntax "DBMSRV_CMD_SqlInfo" <select statement>"
"@reply After the SQL statement has been successfully executed, the following is displayed:<NL>"
       "OK<NL>"
       "<columnlist><NL>"
       "The fields in the column list are separated by semicolons.";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlInfo::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdSQL_SqlInfo::getCommandName
    ( )
{
  return DBMSRV_CMD_SqlInfo;
} // end DBMSrvCmdSQL_SqlInfo::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlInfo::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdSQL_SqlInfo::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::info
    ( DBMSrvKnl_GlobalSessions::SessionSQL,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdSQL_SqlInfo::run
