/*!
  \file    DBMSrvUsr_ChallengeResponseClient.cpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   definition  of class to handle challenge/response

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
#include "hcn90c.h"
#include "hsp02.h"
#include "RunTime/Security/RTESec_Authentication.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"
#include "DBM/Srv/User/DBMSrvUsr_Authentications.hpp"

#include "DBM/Srv/User/DBMSrvUsr_ChallengeResponseClient.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvUsr_ChallengeResponseClient
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvUsr_ChallengeResponseClient::DBMSrvUsr_ChallengeResponseClient
//-----------------------------------------------------------------------------
DBMSrvUsr_ChallengeResponseClient::DBMSrvUsr_ChallengeResponseClient
  ( const char            * szUsrPwd )
{
  m_szUsername.Init();
  m_szPassword.Init();

  m_szGetChallengeCommand.Erase();
  m_szResponseCommand.Erase();

  m_szUsername.rawAssign(szUsrPwd);
  m_szPassword.Init();

  const char * pComma = strchr(szUsrPwd, ',');
  if (pComma != NULL) {
    m_szUsername[(int)(pComma-szUsrPwd)] = 0;
    ++pComma;
    m_szPassword.rawAssign(pComma);
    cn90Uncrypt(m_szPassword, false);
    ToolsParsersUtil_IdentifierCheck::KernelToupper(m_szPassword.asCharp());
  } // end if

} // end DBMSrvUsr_ChallengeResponseClient::DBMSrvUsr_ChallengeResponseClient

//-----------------------------------------------------------------------------
// Destructor DBMSrvUsr_ChallengeResponseClient::~DBMSrvUsr_ChallengeResponseClient
//-----------------------------------------------------------------------------
DBMSrvUsr_ChallengeResponseClient::~DBMSrvUsr_ChallengeResponseClient
  ( )
{
} // end DBMSrvUsr_ChallengeResponseClient::~DBMSrvUsr_ChallengeResponseClient

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_ChallengeResponseClient::GiveGetChallengeCommand
//-----------------------------------------------------------------------------
const char * DBMSrvUsr_ChallengeResponseClient::GiveGetChallengeCommand
  ( )
{
  m_szGetChallengeCommand.Erase();

  RTESec_Authentication::Iterator iter;
  const RTESec_Authentication * pAuthentication = NULL;

  if (!iter.next(pAuthentication)) {
    return NULL;
  } // end if

  m_szGetChallengeCommand.Assign("user_getchallenge")
                         .Append(" ")
                         .Append(m_szUsername.asCharp());

  int                       nRequestLen;
  const void *              pRequest;
  Tools_HexBuffer           oHexBuffer;

  do {
    RTESec_ClientAuthenticationBuffer & oClientBuffer = m_oAuthentications.GiveClientBuffer(pAuthentication->Name());

    if (!pAuthentication->DataForChallengeRequest(oClientBuffer,
                                                  m_szUsername.asCharp(), m_szUsername.length(),
                                                  pRequest, nRequestLen)) {
      return NULL;
    } // end if

    oHexBuffer.SetData(pRequest, nRequestLen);

    m_szGetChallengeCommand.Append(" ")
                           .Append(pAuthentication->Name())
                           .Append(" ")
                           .Append(oHexBuffer.GiveHexBuffer());
    
  } while (iter.next(pAuthentication));

  return m_szGetChallengeCommand.CharPtr();
} // end DBMSrvUsr_ChallengeResponseClient::GiveGetChallengeCommand

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_ChallengeResponseClient::SetGetChallengeAnswer
//-----------------------------------------------------------------------------
bool DBMSrvUsr_ChallengeResponseClient::SetGetChallengeAnswer
  ( const char * pAnswer,
    int          nLen)
{
  m_szMethod.Erase();
  m_oChallenge.SetData("");

  if (pAnswer == NULL) 
    return false;

  const char * pData = strchr(pAnswer, '\n');

  if (pData == NULL) 
    return false;

  // copy method
  m_szMethod.Assign((Tools_DynamicUTF8String::ConstPointer)pAnswer, (Tools_DynamicUTF8String::ElementCount)(pData-pAnswer));

  // search challenge data
  ++pData;
  m_oChallenge.SetData(pData, nLen - (pData-pAnswer));

  return true;
} // end DBMSrvUsr_ChallengeResponseClient::SetGetChallengeAnswer

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_ChallengeResponseClient::GiveResponseCommand
//-----------------------------------------------------------------------------
const char * DBMSrvUsr_ChallengeResponseClient::GiveResponseCommand
  ( )
{
  m_szResponseCommand.Erase();

  const RTESec_Authentication * pAuthentication 
       = RTESec_Authentication::FindByName(m_szMethod.CharPtr(),
                                           m_szMethod.Length());
  if (pAuthentication == NULL) {
    return NULL;
  } // end if

  const void * pResponse;
  int          nResponseLength;

  RTESec_ClientAuthenticationBuffer & oBuffer = m_oAuthentications.GiveClientBuffer(pAuthentication->Name());

  pAuthentication->CreateResponse(oBuffer,
                                  pResponse, nResponseLength,
                                  m_oChallenge.GiveBinBuffer(), m_oChallenge.GiveSizeOfBinBuffer(),
                                  m_szPassword.asCharp(), m_szPassword.length());

  // prepare answer
  Tools_HexBuffer oResponse(pResponse, nResponseLength);
  m_szResponseCommand.Assign("user_response")
                     .Append(" ")
                     .Append(oResponse.GiveHexBuffer());

  return m_szResponseCommand.CharPtr();
} // end DBMSrvUsr_ChallengeResponseClient::GiveResponseCommand
