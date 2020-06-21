/*!
  \file    DBMSrvCmdTrc_TraceOff.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command trace_off

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

#include "SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOn.hpp"
#include "SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOff.hpp"


//-----------------------------------------------------------------------------
// members of class DBMSrvCmdTrc_TraceOff
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdTrc_TraceOff::DBMSrvCmdTrc_TraceOff
//-----------------------------------------------------------------------------
DBMSrvCmdTrc_TraceOff::DBMSrvCmdTrc_TraceOff()
    : DBMSrv_Command( DBMSrv_Command::KeyTraceOff, false, DBMSrvCmdTrc_TraceOff::m_LongHelp ) 
{
} // end DBMSrvCmdTrc_TraceOff::DBMSrvCmdTrc_TraceOff

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdTrc_TraceOff::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmdTrc_TraceOff::m_LongHelp=
"@command trace_off You determine which activities of the database instance should "
                   "no longer be logged or completely deactivate the database trace. "
                   "You specify which activities should no longer be logged using the "
                   "command option activity_list. Separate multiple activities by a "
                   "space. By specifying ALL, you deactivate the log function for all "
                   "activities, and therefore completely deactivate the database trace. "
                   "You can view the command options currently active with a description "
                   "using the command trace_show verbose."
"@preconditions You have the server authorization DBInfoRead or DBFileRead."
"@syntax trace_off <activity_list> | ALL"
"@param  <activity_list> List of activities which should no longer be logged."
"@reply OK";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdTrc_TraceOff::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceOff::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmdTrc_TraceOff oCommand;

  return oCommand.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmdTrc_TraceOff::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceOff::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  Reply.startWithOK();
  
  if (command->oArguments.getValue(1).ToUpper().Compare(DBMSrvCmdTrc_TraceOn::OPTION_ALL) == 0) {
    if (command->oArguments.getCount() > 1) {
      nFuncReturn = Reply.startWithError(ERR_TOOMANYPARAM_CN00);
    } else {
      nFuncReturn = DBMSrvCmdTrc_TraceOn::SwitchAll(vcontrol, 0, false, Reply);
    } // end if
  } else {
    nFuncReturn = DBMSrvCmdTrc_TraceOn::Switch(vcontrol, command->args, false, Reply);
  } // end if

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceOff::run
