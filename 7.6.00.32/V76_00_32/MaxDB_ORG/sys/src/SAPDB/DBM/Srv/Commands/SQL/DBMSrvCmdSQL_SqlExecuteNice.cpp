/*!
  \file    DBMSrvCmdSQL_SqlExecuteNice.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command sql_executenice

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

#include "SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlExecuteNice.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdSQL_SqlExecuteNice
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdSQL_SqlExecuteNice::DBMSrvCmdSQL_SqlExecuteNice
//-----------------------------------------------------------------------------
DBMSrvCmdSQL_SqlExecuteNice::DBMSrvCmdSQL_SqlExecuteNice()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdSQL_SqlExecuteNice::m_LongHelp ) 
{
} // end DBMSrvCmdSQL_SqlExecuteNice::DBMSrvCmdSQL_SqlExecuteNice

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdSQL_SqlExecuteNice::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_SqlExecuteNice       "sql_executenice"

const char * const DBMSrvCmdSQL_SqlExecuteNice::m_LongHelp=
"@command "DBMSRV_CMD_SqlExecuteNice" You transfer the specified SQL statement to the database instance. "
                                     "When you enter a SELECT statement, the system displays the results. "
                                     "The system outputs an OK message for other statements. For more "
                                     "information about SQL statements, see the Reference Manual and "
                                     "SQL Tutorial."
"@preconditions You have the server authorization AccessSQL."
"@syntax "DBMSRV_CMD_SqlExecuteNice" <statement>"
"@reply After the SQL statement has been successfully executed, the following is displayed:<NL>"
       "OK<NL>"
       "[CONTINUE]<NL>"
       "<column>=<value><NL>"
       "<column>=<value><NL>"
       "...<NL>"
       "The answer contains only one record. The fields with name of column and value are separated by newlines. "
       "Character strings are displayed in single quotation marks.";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlExecuteNice::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdSQL_SqlExecuteNice::getCommandName
    ( )
{
  return DBMSRV_CMD_SqlExecuteNice;
} // end DBMSrvCmdSQL_SqlExecuteNice::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlExecuteNice::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdSQL_SqlExecuteNice::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::execute
    ( DBMSrvKnl_GlobalSessions::SessionSQL,
      true,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdSQL_SqlExecuteNice::run
