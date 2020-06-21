/*!
  \file    DBMSrvUsr_ChallengeResponseServer.cpp
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
#include <time.h>
#include <stdio.h>

#include "hcn20.h"
#include "hcn50.h"
#include "hcn90.h"
#include "RunTime/RTE_UserProfileContainer.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"

#include "DBM/Srv/User/DBMSrvUsr_ChallengeResponseServer.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvUsr_ChallengeResponseServer
//-----------------------------------------------------------------------------
DBMSrvUsr_ChallengeResponseServer * DBMSrvUsr_ChallengeResponseServer::m_oInstance = NULL;

//-----------------------------------------------------------------------------
// public static member DBMSrvUsr_ChallengeResponseServer::CreateInstance
//-----------------------------------------------------------------------------
DBMSrvUsr_ChallengeResponseServer * DBMSrvUsr_ChallengeResponseServer::CreateInstance
  ( const char            * szDatabase,
    const char            * szUser,
    const Tools_Array<Tools_DynamicUTF8String> & oMethods,
    const Tools_Array<Tools_DynamicUTF8String> & oRequestData )
{
  DestroyInstance();
  m_oInstance = new DBMSrvUsr_ChallengeResponseServer(szDatabase, szUser, oMethods, oRequestData);
  return m_oInstance;
} // end DBMSrvUsr_ChallengeResponseServer::CreateInstance

//-----------------------------------------------------------------------------
// public static memeber DBMSrvUsr_ChallengeResponseServer::GiveInstance
//-----------------------------------------------------------------------------
DBMSrvUsr_ChallengeResponseServer * DBMSrvUsr_ChallengeResponseServer::GiveInstance
  ( )
{
  return m_oInstance;
} // end DBMSrvUsr_ChallengeResponseServer::GiveInstance

//-----------------------------------------------------------------------------
// public static member DBMSrvUsr_ChallengeResponseServer::DestroyInstance
//-----------------------------------------------------------------------------
void DBMSrvUsr_ChallengeResponseServer::DestroyInstance
  ( )
{
  if (m_oInstance != NULL)
    delete m_oInstance;
  m_oInstance = NULL;
} // end DBMSrvUsr_ChallengeResponseServer::DestroyInstance

//-----------------------------------------------------------------------------
// private constructor DBMSrvUsr_ChallengeResponseServer::DBMSrvUsr_ChallengeResponseServer
//-----------------------------------------------------------------------------
DBMSrvUsr_ChallengeResponseServer::DBMSrvUsr_ChallengeResponseServer
  ( const char            * szDatabase,
    const char            * szUser,
    const Tools_Array<Tools_DynamicUTF8String> & oMethods,
    const Tools_Array<Tools_DynamicUTF8String> & oRequestData )
{
  m_szUsername.Erase();
  m_szDatabase.Erase();
  m_szMethod.Erase();

  if (CheckUser(szDatabase, szUser)) {
    m_szDatabase.Assign(szDatabase);
    m_szUsername.Assign(szUser);

    const RTESec_Authentication * pAuthentication = NULL;
    DBMSrvUsr_AuthenticateIterator iter(m_szDatabase.CharPtr(), oMethods);

    if  (iter.next (pAuthentication)) {
      m_szMethod.Assign(pAuthentication->Name ());

      int nIndex = 0;
      if (oMethods.size() > 0) {
        for (nIndex = 0; nIndex < oMethods.size(); ++nIndex) {
          if (m_szMethod.Compare(oMethods[nIndex]) == 0) {
            Tools_DynamicUTF8String oRequest = oRequestData[nIndex];
            m_oRequestData.SetData(oRequest.CharPtr());
          } // end if
        } // end if
      } // end if
    } // end if
  } // end if

} // end DBMSrvUsr_ChallengeResponseServer::DBMSrvUsr_ChallengeResponseServer

//-----------------------------------------------------------------------------
// private destructor DBMSrvUsr_ChallengeResponseServer::~DBMSrvUsr_ChallengeResponseServer
//-----------------------------------------------------------------------------
DBMSrvUsr_ChallengeResponseServer::~DBMSrvUsr_ChallengeResponseServer
  ( )
{
} // end DBMSrvUsr_ChallengeResponseServer::~DBMSrvUsr_ChallengeResponseServer

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_ChallengeResponseServer::GiveChallenge
//-----------------------------------------------------------------------------
const char * DBMSrvUsr_ChallengeResponseServer::GiveChallenge
  ( )
{
  m_oChallenge.SetData("");

  const RTESec_Authentication * pAuthentication
           = RTESec_Authentication::FindByName( m_szMethod.CharPtr(),
                                                m_szMethod.Length() );

  if (pAuthentication == NULL) {
    return NULL;
  } // end if

  // generate challenge
  const void * pChallenge;
  int          nChallenge;
  if (!pAuthentication->CreateChallenge(m_oBuffer, pChallenge, nChallenge, m_oRequestData.GiveBinBuffer(), (int) m_oRequestData.GiveSizeOfBinBuffer())) {
    return NULL;
  } // end if

  m_oChallenge.SetData(pChallenge, nChallenge);

  return m_oChallenge.GiveHexBuffer();
} // end DBMSrvUsr_ChallengeResponseServer::GiveChallenge

//-----------------------------------------------------------------------------
// public member DBMSrvUsr_ChallengeResponseServer::CheckResponse
//-----------------------------------------------------------------------------
bool DBMSrvUsr_ChallengeResponseServer::VerifyResponse
  ( const char * pASCIIResponse )
{
  // check input
  if (pASCIIResponse == NULL)
    return false;

  // get authentication object
  const RTESec_Authentication * pAuthentication
           = RTESec_Authentication::FindByName( m_szMethod.CharPtr(),
                                                m_szMethod.Length() );
  if (pAuthentication == NULL)
    return false;

  // load user 
  cn50DBMUser oUser(m_szDatabase.CharPtr(), m_szUsername.CharPtr());

  // convert response data
  Tools_HexBuffer oResponse(pASCIIResponse);

  // verify response data
  tsp00_Namec szPassword;

  szPassword.rawAssign(oUser.getClearMasterPwd());
  ToolsParsersUtil_IdentifierCheck::KernelToupper(szPassword.asCharp());

  bool bOk = pAuthentication->VerifyResponse(m_oBuffer,
                                             oResponse.GiveBinBuffer(), (int) oResponse.GiveSizeOfBinBuffer(),
                                             szPassword, szPassword.length());
  if (!bOk && oUser.isSecondPwd()) {
    szPassword.rawAssign(oUser.getSecondPwd(true));
    ToolsParsersUtil_IdentifierCheck::KernelToupper(szPassword.asCharp());

    bOk = pAuthentication->VerifyResponse(m_oBuffer,
                                          oResponse.GiveBinBuffer(), (int) oResponse.GiveSizeOfBinBuffer(),
                                          szPassword, szPassword.length());
  } // end if

  return bOk;
} // end DBMSrvUsr_ChallengeResponseServer::CheckResponse

//-----------------------------------------------------------------------------
// private member DBMSrvUsr_ChallengeResponseServer::CheckUser
//-----------------------------------------------------------------------------
bool DBMSrvUsr_ChallengeResponseServer::CheckUser
  ( const char * szDatabaseIn, 
    const char * szUsernameIn ) const 
{
  tsp00_DbNamec       szDatabase;
  tcn00_UserNamec     szUsername;

  szUsername.rawAssign(szUsernameIn);
  szDatabase.rawAssign(szDatabaseIn);

  // load user 
  cn50DBMUser oUser(szDatabase, szUsername);

  // try to restore upc file
  if (!oUser.lastEvent().IsEmpty()) {
      if (cn50DBMUser::isEmpty(szDatabase)) {
        cn50DBMUser::tryRestoreIfEmpty(szDatabase);
      } // end if
  } // end if

  if (oUser.reload() == ERR_USRREAD_CN00) {
    if (cn50DBMUser::isEmpty(szDatabase)) {
      // yes UPC is empty -> create user from param file

      tcn002_XpValueString szColdUser;
      tcn002_XpValueString szColdPwd;
      DBMSrvMsg_Error oParamGetMsg;
      if (cn20XParamGetValue(szDatabase, PAN_CONTROLID, szColdUser, oParamGetMsg) &&
          cn20XParamGetValue(szDatabase, PAN_CONTROLPW, szColdPwd, oParamGetMsg)     ) {

        if (stricmp(szColdUser, szUsername) != 0) {
          return false;
        } // end if

        cn90Uncrypt( szColdPwd, false);

        oUser.setUserName  (szColdUser)
             .setMasterPwd (szColdPwd  )
             .setColdUser     ( )
             .setServerRights ( DBMMaskAll_CN50, DBMUserDefault_CN50 )
             .setGUIRights    ( DBMMaskAll_CN50, DBMUserDefaultGUI_CN50 );

        if (oUser.save(true) != OK_CN00) {
          return false;
        } // end if
      } // end if 

    } else {
      // No, upc is not empty -> wrong username!
      return false;
    } // end if 
  } // end if

  return true;
} // end DBMSrvUsr_ChallengeResponseServer::CheckUser
