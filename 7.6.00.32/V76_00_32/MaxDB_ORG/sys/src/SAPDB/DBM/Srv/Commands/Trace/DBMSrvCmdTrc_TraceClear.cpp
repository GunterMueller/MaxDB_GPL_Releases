/*!
  \file    DBMSrvCmdTrc_TraceClear.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command trace_clear

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

#include "hcn50.h" // cn50DBMUser
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

#include "SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceClear.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdTrc_TraceClear
//-----------------------------------------------------------------------------
#define SQL_DIAGNOSE_VTRACE_CLEAR "DIAGNOSE VTRACE CLEAR"

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdTrc_TraceClear::DBMSrvCmdTrc_TraceClear
//-----------------------------------------------------------------------------
DBMSrvCmdTrc_TraceClear::DBMSrvCmdTrc_TraceClear()
    : DBMSrv_Command( DBMSrv_Command::KeyTraceClear, false, DBMSrvCmdTrc_TraceClear::m_LongHelp ) 
{
} // end DBMSrvCmdTrc_TraceClear::DBMSrvCmdTrc_TraceClear

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdTrc_TraceClear::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmdTrc_TraceClear::m_LongHelp=
"@command trace_clear You delete the current contents of the database trace. "
                     "Use this DBM command to delete data that you no longer require "
                     "from the database trace before you perform targeted logging of "
                     "the database instance activities by activating the database trace."
"@preconditions You have the server authorization DBInfoRead or DBFileRead."
"@syntax trace_clear"
"@reply OK";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdTrc_TraceClear::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceClear::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmdTrc_TraceClear oCommand;

  return oCommand.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmdTrc_TraceClear::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceClear::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  // execute kernel command
  cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
  DBMSrvMsg_Error oError;

  if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                   oUser.getUserName().asCharp(),
                                   oUser.getClearMasterPwd().asCharp(),
                                   SQL_DIAGNOSE_VTRACE_CLEAR,
                                   oError)) {
    nFuncReturn = Reply.startWithMessageList(oError);
  } else {
    Reply.startWithOK();
  } // end if

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceClear::run
