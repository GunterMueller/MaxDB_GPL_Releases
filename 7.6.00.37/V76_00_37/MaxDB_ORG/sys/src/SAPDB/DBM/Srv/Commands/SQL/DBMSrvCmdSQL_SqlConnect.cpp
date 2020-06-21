/*!
  \file    DBMSrvCmdSQL_SqlConnect.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command sql_connect

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

#include "SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlConnect.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdSQL_SqlConnect
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdSQL_SqlConnect::DBMSrvCmdSQL_SqlConnect
//-----------------------------------------------------------------------------
DBMSrvCmdSQL_SqlConnect::DBMSrvCmdSQL_SqlConnect()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdSQL_SqlConnect::m_LongHelp ) 
{
} // end DBMSrvCmdSQL_SqlConnect::DBMSrvCmdSQL_SqlConnect

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdSQL_SqlConnect::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_SqlConnect       "sql_connect"

const char * const DBMSrvCmdSQL_SqlConnect::m_LongHelp=
"@command "DBMSRV_CMD_SqlConnect" You open a SQL session with the database instance as a database user "
                                 "in which you can transfer SQL statements to the database instance. The "
                                 "SQL database session is linked with a user task of the database instance. "
                                 "Log on to the database by using your user name and passwort or your user "
                                 "type. If you do not specify a database user, the first DBM operator "
                                 "registered on the database server will be logged on. "
"@preconditions You have the server authorization AccessSQL. "
               "You have the server authorizations required to execute the commands and statements. "
               "The operational state of the database instance is ONLINE. "
               "You work with the Database Manager in script mode or in session mode. "
"@syntax "DBMSRV_CMD_SqlConnect" [user-type=DBM|user-type=DBA|user-type=SAP|<user>,<pwd>]"
"@reply OK";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlConnect::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdSQL_SqlConnect::getCommandName
    ( )
{
  return DBMSRV_CMD_SqlConnect;
} // end DBMSrvCmdSQL_SqlConnect::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlConnect::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdSQL_SqlConnect::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::connect
    ( DBMSrvKnl_GlobalSessions::SessionSQL,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdSQL_SqlConnect::run
