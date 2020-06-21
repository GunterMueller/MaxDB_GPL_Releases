/*!
  \file    DBMSrvCmdUsr_UserResponse.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command user_response

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
#include <time.h>

#include "hcn20.h"
#include "hcn50.h"
#include "hcn51.h"
#include "hcn53.h"

#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#include "DBM/Srv/User/DBMSrvUsr_ChallengeResponseServer.hpp"

#include "DBM/Srv/Commands/User/DBMSrvCmdUsr_UserResponse.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdUsr_UserResponse
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdUsr_UserResponse::DBMSrvCmdUsr_UserResponse
//-----------------------------------------------------------------------------
DBMSrvCmdUsr_UserResponse::DBMSrvCmdUsr_UserResponse()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdUsr_UserResponse::m_LongHelp ) 
{
} // end DBMSrvCmdUsr_UserResponse::DBMSrvCmdUsr_UserResponse

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdUsr_UserResponse::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_UserResponse         "user_response"

const char * const DBMSrvCmdUsr_UserResponse::m_LongHelp=
"@command "DBMSRV_CMD_UserResponse" A client submits with this command the response data for a challenge"
                                  " which has the client requested before from the server."
"@preconditions The client has requested before  from the server the challenge data for a challenge response authentication."
"@syntax "DBMSRV_CMD_UserResponse" <response>"
"@param  <response>  The response from the client. This binary data must be specified in hexadecimal"
                   " ASCII representation (e.g. '27F37D95')." 
"@reply OK<NL>";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdUsr_UserResponse::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdUsr_UserResponse::getCommandName
    ( )
{
  return DBMSRV_CMD_UserResponse;
} // end DBMSrvCmdUsr_UserResponse::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdUsr_UserResponse::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdUsr_UserResponse::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply ) 
{
  tcn00_Error nFuncReturn = OK_CN00;

  // read user 
  Tools_DynamicUTF8String szResponse;
  szResponse  = command->oArguments.getValue(1);

  DBMSrvUsr_ChallengeResponseServer * pServer = 
    DBMSrvUsr_ChallengeResponseServer::GiveInstance();

  if (pServer == NULL) {
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFAIL));
  } // end if

  if (!pServer->VerifyResponse(szResponse.CharPtr())) {
    pServer->DestroyInstance();
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFAIL));
  } // end if

  // load the user
  tcn00_UserNamec szUser;
  szUser.rawAssign(pServer->GetUser());
  pServer->DestroyInstance();
  vcontrol->pCurrentUser->setDBName   ( vcontrol->dbname )
                         .setUserName ( szUser );
  nFuncReturn = vcontrol->pCurrentUser->reload();
  if (nFuncReturn != OK_CN00) {
    return Reply.startWithError(nFuncReturn);
  } // end if

  // set to logged on
  vcontrol->pCurrentUser->setLoggedOn();

  // enable DBA logging if configured
  cn51DBMConfigValue  cfgDbaDtl (vcontrol->dbname, CFG_DBADTL_CN51);
  if (((int)cfgDbaDtl) != 0) {
    tcn53_DBAAction::enable ();
  } // end if

  // read the unicode setting
  tcn002_XpValueString  szUnicode;
  DBMSrvMsg_Error oParamGetMsg;
  if (cn20XParamGetValue(vcontrol->dbname, PAN_UNICODE_CATALOG, szUnicode, oParamGetMsg)) {
    vcontrol->bUnicode = (stricmp(szUnicode, XP_VAL_YES_CN00) == 0);
  } // end if

  // set hss connection data
  vcontrol->pHSSNodes->SetConnectionData(vcontrol->dbname.asCharp(), 
                                         vcontrol->pCurrentUser->getUserName().asCharp(),
                                         vcontrol->pCurrentUser->getClearMasterPwd().asCharp());

  Reply.startWithOK();

  return OK_CN00;
} // end DBMSrvCmdUsr_UserResponse::run
