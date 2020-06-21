/*!
  \file    DBMSrvCmdDb_DbFetch.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command db_fetch

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

#include "SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbFetch.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdDb_DbFetch
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdDb_DbFetch::DBMSrvCmdDb_DbFetch
//-----------------------------------------------------------------------------
DBMSrvCmdDb_DbFetch::DBMSrvCmdDb_DbFetch()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdDb_DbFetch::m_LongHelp ) 
{
} // end DBMSrvCmdDb_DbFetch::DBMSrvCmdDb_DbFetch

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdDb_DbFetch::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbFetch       "db_fetch"

const char * const DBMSrvCmdDb_DbFetch::m_LongHelp=
"@command "DBMSRV_CMD_DbFetch" You have executed a database statement. The keyword CONTINUE at the end "
                             "of the reply shows that additional result data is available. You display "
                             "this additional data in the same database session."
"@preconditions You have the server authorization AccessSQL. You have executed a database statement "
"@syntax "DBMSRV_CMD_DbFetch
"@reply After the command has been successfully executed, the following is displayed:<NL>"
       "OK<NL>"
       "(END|CONTINUE)<NL>"
       "<record><NL>"
       "...<NL>"
       "The fields in an reply record are separated by semicolons. Character strings are displayed in "
       "single quotation marks.";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbFetch::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdDb_DbFetch::getCommandName
    ( )
{
  return DBMSRV_CMD_DbFetch;
} // end DBMSrvCmdDb_DbFetch::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbFetch::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDb_DbFetch::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  return DBMSrvKnl_DbAndSqlCommands::fetch
    ( DBMSrvKnl_GlobalSessions::SessionDatabase,
      vcontrol, 
      command,
      Reply );
} // end DBMSrvCmdDb_DbFetch::run
