/*!
  \file    DBMSrvCmdSQL_SqlFetchNice.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command sql_fetchnice

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

#include "SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetchNice.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdSQL_SqlFetchNice
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdSQL_SqlFetchNice::DBMSrvCmdSQL_SqlFetchNice
//-----------------------------------------------------------------------------
DBMSrvCmdSQL_SqlFetchNice::DBMSrvCmdSQL_SqlFetchNice()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdSQL_SqlFetchNice::m_LongHelp ) 
{
} // end DBMSrvCmdSQL_SqlFetchNice::DBMSrvCmdSQL_SqlFetchNice

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdSQL_SqlFetchNice::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_SqlFetchNice       "sql_fetchnice"

const char * const DBMSrvCmdSQL_SqlFetchNice::m_LongHelp=
"@command "DBMSRV_CMD_SqlFetchNice" You have executed a SQL statement. The keyword CONTINUE at the end "
                                  "of the reply shows that additional result data is available. You display "
                                  "this additional data in the same database session."
"@preconditions You have the server authorization AccessSQL. You have executed a SQL statement "
"@syntax "DBMSRV_CMD_SqlFetchNice
"@reply After the SQL statement has been successfully executed, the following is displayed:<NL>"
       "OK<NL>"
       "[CONTINUE]<NL>"
       "<column>=<value><NL>"
       "<column>=<value><NL>"
       "...<NL>"
       "The answer contains only one record. The fields with name of column and value are separated by newlines. "
       "Character strings are displayed in single quotation marks.";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlFetchNice::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdSQL_SqlFetchNice::getCommandName
    ( )
{
  return DBMSRV_CMD_SqlFetchNice;
} // end DBMSrvCmdSQL_SqlFetchNice::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlFetchNice::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdSQL_SqlFetchNice::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::fetchnice
    ( DBMSrvKnl_GlobalSessions::SessionSQL,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdSQL_SqlFetchNice::run
