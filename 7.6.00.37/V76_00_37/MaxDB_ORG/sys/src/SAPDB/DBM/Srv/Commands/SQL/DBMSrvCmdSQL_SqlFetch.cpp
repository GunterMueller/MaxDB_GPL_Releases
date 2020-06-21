/*!
  \file    DBMSrvCmdSQL_SqlFetch.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command sql_fetch

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

#include "SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetch.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdSQL_SqlFetch
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdSQL_SqlFetch::DBMSrvCmdSQL_SqlFetch
//-----------------------------------------------------------------------------
DBMSrvCmdSQL_SqlFetch::DBMSrvCmdSQL_SqlFetch()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdSQL_SqlFetch::m_LongHelp ) 
{
} // end DBMSrvCmdSQL_SqlFetch::DBMSrvCmdSQL_SqlFetch

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdSQL_SqlFetch::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_SqlFetch       "sql_fetch"

const char * const DBMSrvCmdSQL_SqlFetch::m_LongHelp=
"@command "DBMSRV_CMD_SqlFetch" You have executed a SQL statement. The keyword CONTINUE at the end "
                              "of the reply shows that additional result data is available. You display "
                              "this additional data in the same database session."
"@preconditions You have the server authorization AccessSQL. You have executed a SQL statement "
"@syntax "DBMSRV_CMD_SqlFetch
"@reply After the SQL statement has been successfully executed, the following is displayed:<NL>"
       "OK<NL>"
       "(END|CONTINUE)<NL>"
       "<record><NL>"
       "...<NL>"
       "The fields in an reply record are separated by semicolons. Character strings are displayed in "
       "single quotation marks.";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlFetch::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdSQL_SqlFetch::getCommandName
    ( )
{
  return DBMSRV_CMD_SqlFetch;
} // end DBMSrvCmdSQL_SqlFetch::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdSQL_SqlFetch::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdSQL_SqlFetch::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::fetch
    ( DBMSrvKnl_GlobalSessions::SessionSQL,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdSQL_SqlFetch::run
