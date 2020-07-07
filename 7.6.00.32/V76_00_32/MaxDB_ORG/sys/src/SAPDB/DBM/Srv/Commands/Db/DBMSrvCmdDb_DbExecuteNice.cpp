/*!
  \file    DBMSrvCmdDb_DbExecuteNice.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command db_executenice

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

#include "SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbExecuteNice.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdDb_DbExecuteNice
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdDb_DbExecuteNice::DBMSrvCmdDb_DbExecuteNice
//-----------------------------------------------------------------------------
DBMSrvCmdDb_DbExecuteNice::DBMSrvCmdDb_DbExecuteNice()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdDb_DbExecuteNice::m_LongHelp ) 
{
} // end DBMSrvCmdDb_DbExecuteNice::DBMSrvCmdDb_DbExecuteNice

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdDb_DbExecuteNice::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbExecuteNice       "db_executenice"

const char * const DBMSrvCmdDb_DbExecuteNice::m_LongHelp=
"@command "DBMSRV_CMD_DbExecuteNice" You transfer the specified administration command or SQL statement to the database instance. "
                                    "When you enter a SELECT statement, the system displays the results. "
                                    "The system outputs an OK message for other statements. For more "
                                    "information about SQL statements, see the Reference Manual and "
                                    "SQL Tutorial."
"@preconditions You have the server authorization AccessAdmin or AccessSQL."
"@syntax "DBMSRV_CMD_DbExecuteNice" <statement>"
"@reply After the SQL statement has been successfully executed, the following is displayed:<NL>"
       "OK<NL>"
       "[CONTINUE]<NL>"
       "<column>=<value><NL>"
       "<column>=<value><NL>"
       "...<NL>"
       "The answer contains only one record. The fields with name of column and value are separated by newlines. "
       "Character strings are displayed in single quotation marks.";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbExecuteNice::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdDb_DbExecuteNice::getCommandName
    ( )
{
  return DBMSRV_CMD_DbExecuteNice;
} // end DBMSrvCmdDb_DbExecuteNice::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbExecuteNice::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDb_DbExecuteNice::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::execute
    ( DBMSrvKnl_GlobalSessions::SessionDatabase,
      true,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdDb_DbExecuteNice::run
