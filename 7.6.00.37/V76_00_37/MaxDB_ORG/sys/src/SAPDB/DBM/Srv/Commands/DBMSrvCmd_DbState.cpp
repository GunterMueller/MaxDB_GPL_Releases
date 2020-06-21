/*!
  \file    DBMSrvCmd_DbState.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command db_state

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
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"

#include "SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbState.hpp"

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmd_DbState::DBMSrvCmd_DbState
//-----------------------------------------------------------------------------
DBMSrvCmd_DbState::DBMSrvCmd_DbState()
    : DBMSrv_Command( DBMSrv_Command::KeyDbState, false, m_LongHelp ) 
{
} // end DBMSrvCmd_DbState::DBMSrvCmd_DbState

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmd_DbState::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbState        "db_state"
#define DBMSRV_OPTION_VERBOSE     "v"
#define DBMSRV_TXT_State          "State"
#define DBMSRV_TXT_LogFull        "Log Full      = "
#define DBMSRV_TXT_DatabaseFull   "Database Full = "
#define DBMSRV_TXT_Yes            "Yes"
#define DBMSRV_TXT_No             "No"

const char * const DBMSrvCmd_DbState::m_LongHelp=
    "@command db_state You display the operational state of the database instance."
    "@preconditions You have the DBM operator authorization DBInfoRead."
    "@syntax "DBMSRV_CMD_DbState" [-"DBMSRV_OPTION_VERBOSE"]"
    "@param  -"DBMSRV_OPTION_VERBOSE" This options displays in addition the extended operational state of the database"
    "@reply OK<NL>"
            DBMSRV_TXT_State"<NL>"
           "<state><NL>"
           "["DBMSRV_TXT_LogFull"("DBMSRV_TXT_Yes"|"DBMSRV_TXT_No")]<NL>"
           "["DBMSRV_TXT_DatabaseFull"("DBMSRV_TXT_Yes"|"DBMSRV_TXT_No")]<NL>"
           "@replyvalue <state>         operational state the database (e.g. OFFLINE, ADMIN or ONLINE)";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmd_DbState::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_DbState::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmd_DbState oVersion;

  return oVersion.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmd_DbState::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_DbState::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  Tools_DynamicUTF8String szOption;
  bool                    bVerbose = false;

  if (command->oArguments.getCount() > 0) {
    szOption = command->oArguments.getName(1);
    szOption.ToLower();
    if (szOption.Compare(DBMSRV_OPTION_VERBOSE) != 0) {
      if (szOption.Length() == 0) {
        szOption = command->oArguments.getValue(1);
      } // end if
      return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, Msg_Arg("DBMSRV_CMDOPTION", szOption.CharPtr()), Msg_Arg("DBMSRV_CMDNAME",DBMSRV_CMD_DbState)));
    } else {
      bVerbose = true;
    } // end if
  } // end if

  DBMSrvKnl_State oState(vcontrol->dbname);
  if (!oState.DBMMsg().IsEmpty()) {
    return Reply.startWithMessageList(oState.DBMMsg());
  } // end if

  Tools_DynamicUTF8String     szState;
  bool                        bLogIsFull  = false;
  bool                        bDataIsFull = false;
  
  oState.State(szState, bLogIsFull, bDataIsFull);
  if (!oState.DBMMsg().IsEmpty()) {
    return Reply.startWithMessageList(oState.DBMMsg());
  } // end if

  Reply.startWithOK();

  Reply.appendLines(DBMSRV_TXT_State, szState.CharPtr());
  if (bVerbose) {
    Reply.appendStringAndLine(DBMSRV_TXT_LogFull,      bLogIsFull ?DBMSRV_TXT_Yes:DBMSRV_TXT_No);
    Reply.appendStringAndLine(DBMSRV_TXT_DatabaseFull, bDataIsFull?DBMSRV_TXT_Yes:DBMSRV_TXT_No);
  } // end if

  return Reply.ReturnCode();
} // end DBMSrvCmd_DbState::run
