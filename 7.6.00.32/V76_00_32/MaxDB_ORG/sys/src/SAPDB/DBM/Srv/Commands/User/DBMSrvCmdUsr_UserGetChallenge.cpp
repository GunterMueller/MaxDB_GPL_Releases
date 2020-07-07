/*!
  \file    DBMSrvCmdUsr_UserGetChallenge.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command user_getchallenge

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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
#include "DBM/Srv/User/DBMSrvUsr_ChallengeResponseServer.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#include "DBM/Srv/Commands/User/DBMSrvCmdUsr_UserGetChallenge.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdUsr_UserGetChallenge
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdUsr_UserGetChallenge::DBMSrvCmdUsr_UserGetChallenge
//-----------------------------------------------------------------------------
DBMSrvCmdUsr_UserGetChallenge::DBMSrvCmdUsr_UserGetChallenge()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdUsr_UserGetChallenge::m_LongHelp ) 
{
} // end DBMSrvCmdUsr_UserGetChallenge::DBMSrvCmdUsr_UserGetChallenge

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdUsr_UserGetChallenge::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_UserGetChallenge         "user_getchallenge"
#define DBMSRV_OPT_UserGetChallenge_Methods "METHODS"

const char * const DBMSrvCmdUsr_UserGetChallenge::m_LongHelp=
"@command "DBMSRV_CMD_UserGetChallenge" This command request the challenge for for the specified"
                                      " user. The client may also specify one or more authentication"
                                      " methods with challenge request data for each method:"
"@syntax "DBMSRV_CMD_UserGetChallenge" <username> [["DBMSRV_OPT_UserGetChallenge_Methods"] {<method> <requestdata>}]"
"@param  <username>    The name of the user which challenge the client does request."
"@param  <method>      The name of a authentication method."
"@param  <requestdata> The request data to calculate the challenge for the preceeding method. This method specific"
                     " binary data must be specified in hexadecimal ASCII representation (e.g. '27F37D95')."
"@reply OK<NL>"
       "<method><NL>"
       "<challengedata><NL>"
       "@replyvalue <method>         The authentication method which the server has selected."
       "@replyvalue <challengedata>  The challenge data for the selected method"
                                   " in hexadecimal ASCII representation (e.g. '27F37D95').";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdUsr_UserGetChallenge::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdUsr_UserGetChallenge::getCommandName
    ( )
{
  return DBMSRV_CMD_UserGetChallenge;
} // end DBMSrvCmdUsr_UserGetChallenge::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdUsr_UserGetChallenge::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdUsr_UserGetChallenge::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  Reply.startWithOK();

  // read user 
  Tools_DynamicUTF8String szUser;
  szUser  = command->oArguments.getValue(1);

  // read authentication methods
  int nMethod      = 0;
  Tools_DynamicUTF8String szToken;
  Tools_Array<Tools_DynamicUTF8String> oMethods;
  Tools_Array<Tools_DynamicUTF8String> oRequestData;
  int nToken = 2;
  while (nToken <= command->oArguments.getCount()) {
    szToken = command->oArguments.getValue(nToken);
    if (szToken.ToUpper().Compare(DBMSRV_OPT_UserGetChallenge_Methods) == 0) {
      if (nToken == command->oArguments.getCount()) {
        return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, 
                                                          Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, DBMSRV_OPT_UserGetChallenge_Methods), 
                                                          Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME,DBMSRV_CMD_UserGetChallenge)));
      } // end if
    } else {
      oMethods[nMethod] = command->oArguments.getValue(nToken);
      if (nToken == command->oArguments.getCount()) {
        return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, 
                                                          Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, oMethods[nMethod].CharPtr()), 
                                                          Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME,DBMSRV_CMD_UserGetChallenge)));
      } // end if
      ++nToken;
      oRequestData[nMethod] = command->oArguments.getValue(nToken);
      ++nMethod;
    } // end if
    ++nToken;
  } // end for

  DBMSrvUsr_ChallengeResponseServer * pServer = 
    DBMSrvUsr_ChallengeResponseServer::CreateInstance(vcontrol->dbname, szUser.CharPtr(), oMethods, oRequestData);

  if (pServer == NULL) {
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
  } // end if

  if  (!pServer->IsUserAvailable()) {
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFAIL));
  } // end while

  if  (!pServer->IsMethodAvailable()) {
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOAUTHENTICATION));
  } // end while

  // now we have a authentication method
  Reply.appendLine(pServer->GetMethod());

  const char * pChallenge = pServer->GiveChallenge();

  if (pChallenge == NULL) {
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));
  } // end if
  
  Reply.appendLine(pChallenge);

  return OK_CN00;
} // end DBMSrvCmdUsr_UserGetChallenge::run
