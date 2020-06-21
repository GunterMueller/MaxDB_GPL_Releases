/*!
  @file           DBMSrvKnl_GlobalSessions.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Implementation

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "hcn31.h"
#include "hcn50.h"
#include "hcn90.h"
#include "vsp0031.h"

#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"
#include "RunTime/RTE_DBRegister.hpp"

/*
  -----------------------------------------------------------------------------
  static members variables
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_GlobalSessions::KernelSessionPointer DBMSrvKnl_GlobalSessions::m_oSessions[];

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::GetSession
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_Session * DBMSrvKnl_GlobalSessions::GetSession
    ( SessionType oType )
{
  DBMSrvKnl_Session * pSession = NULL;

  if ((oType > SessionMinValue) && (oType < SessionMaxValue)) {
    pSession = m_oSessions[oType].m_pPointer;
  } // end if

  return pSession;
} // end DBMSrvKnl_GlobalSessions::GetSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::CreateSession
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_Session * DBMSrvKnl_GlobalSessions::CreateSession
    ( SessionType       oType,
      const char      * szDatabase,
      DBMSrvMsg_Error & oError )
{
  return DBMSrvKnl_GlobalSessions::CreateSession(oType, szDatabase, "", oError);
} // end DBMSrvKnl_GlobalSessions::CreateSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::CreateSession
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_Session * DBMSrvKnl_GlobalSessions::CreateSession
    ( SessionType       oType,
      const char      * szDatabase,
      const char      * szConnect,
      DBMSrvMsg_Error & oError )
{
  cn50DBMUser oUser(szDatabase);
  const char * pValue = NULL;
  int          nUser  = 0;
  Tools_DynamicUTF8String strUser;
  Tools_DynamicUTF8String strPassword;

  oError.ClearMessageList();

  // read parameters 
  if (SAPDB_strlen(szConnect) > 0) {
    if (SAPDB_strncmp(szConnect, USER_TYPE_CN50, SAPDB_strlen(USER_TYPE_CN50)) == 0) {
      pValue = SAPDB_strchr(szConnect, ASSIGN_CHAR_CN50);
      if (pValue != NULL) {
        pValue++;
        if ( (SAPDB_strcmp (pValue, USERTYPE_DBM_CN50)                                        == 0 ) ||
             (SAPDB_strncmp(pValue, USERTYPE_DBM_CN50" ", SAPDB_strlen(USERTYPE_DBM_CN50" ")) == 0 )    ) {
          nUser = FIRSTDBM_CN50;
        } else if ( (SAPDB_strcmp (pValue, USERTYPE_DBA_CN50)                                        == 0 ) ||
                    (SAPDB_strncmp(pValue, USERTYPE_DBA_CN50" ", SAPDB_strlen(USERTYPE_DBA_CN50" ")) == 0 )    ) {
          nUser = SYSDBA_CN50;
        } else if ( (SAPDB_strcmp (pValue, USERTYPE_SAP_CN50)                                        == 0 ) ||
                    (SAPDB_strncmp(pValue, USERTYPE_SAP_CN50" ", SAPDB_strlen(USERTYPE_SAP_CN50" ")) == 0 )    ) {
          nUser = SAPUSR_CN50;
        } else {
          oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, Msg_Arg("DBMSRV_CMDOPTION", pValue), Msg_Arg("DBMSRV_CMDNAME","Connect"));
          return NULL;
        } // end if
      } else {
        oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, Msg_Arg("DBMSRV_CMDOPTION", USER_TYPE_CN50), Msg_Arg("DBMSRV_CMDNAME","Connect"));
        return NULL;
      } // end if

      oUser.setUserName( cn50DBMUser::getUser(szDatabase, nUser));
      if (oUser.reload() != OK_CN00) {
        oError.Overrule(oUser.lastEvent());
        oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
        return NULL;
      } // end if
    
      strUser = _Tools_UTF8Ptr(oUser.getUserName().asCharp());
      strPassword = _Tools_UTF8Ptr(oUser.getClearMasterPwd().asCharp());

    } else {
      char * tmpPassword;
      char   szBuffer[101];
  
      SAPDB_strncpy(szBuffer, szConnect, 100);

      // split username and password
      tmpPassword = SAPDB_strchr(szBuffer, PWD_SEPARATOR_CN00);
      if (tmpPassword != NULL) {
        *tmpPassword = CHAR_STRINGTERM_CN90;
        tmpPassword++;
        cn90Uncrypt(tmpPassword, false);
      } else {
        oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, Msg_Arg("DBMSRV_CMDOPTION", "<usr>,<pwd>"), Msg_Arg("DBMSRV_CMDNAME","Connect"));
        return NULL;
      } // end if
      strUser = _Tools_UTF8Ptr(szBuffer);
      strPassword = _Tools_UTF8Ptr(tmpPassword);
    } // end if
  } else {
    oUser.setUserName( oUser.getColdUser());
    if (oUser.reload() != OK_CN00) {
      oError.Overrule(oUser.lastEvent());
      oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
      return NULL;
    } // end if

    strUser = _Tools_UTF8Ptr(oUser.getUserName().asCharp());
    strPassword = _Tools_UTF8Ptr(oUser.getClearMasterPwd().asCharp());
  } // end if

  strUser.Trim();
  strPassword.Trim();
  return DBMSrvKnl_GlobalSessions::CreateSession(oType, szDatabase, strUser.CharPtr(), strPassword.CharPtr(), oError);
} // end DBMSrvKnl_GlobalSessions::CreateSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::CreateSession
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_Session * DBMSrvKnl_GlobalSessions::CreateSession
    ( SessionType       oType,
      const char      * szDatabase,
      const char      * szUser,
      const char      * szPassword,
      DBMSrvMsg_Error & oError )
{
  DBMSrvKnl_Session * pSession = NULL;

  if ((oType > SessionMinValue) && (oType < SessionMaxValue)) {

    // The error of the implicit release does not concern because of this session
    // may be corrupt (e.g. by a "kill <task>" or a "db_restart" via another dbmserver)
    // and so on. The caller/user wishes a new session and want not to know what happen 
    // with the old session. If the caller wants to know what happen with the old 
    // session he should use an explicit ReleaseSession().
    DBMSrvMsg_Error oReleaseError;
    ReleaseSession(oType, oReleaseError);

    pSession = new DBMSrvKnl_Session;
    if (pSession != NULL) {
      switch (oType) {
        case SessionEvent:
          if (!pSession->EventConnect(szDatabase, oError)) {
            delete pSession;
            pSession = NULL;
          } else {
            m_oSessions[oType].m_pPointer = pSession;
          } // end if
          break;
        default:
          if (!pSession->NormalConnect(szDatabase, szUser, szPassword, oError)) {
            delete pSession;
            pSession = NULL;
          } else {
            m_oSessions[oType].m_pPointer = pSession;
          } // end if
          break;
      } // end switch
    } else {
      oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_MEMORY);
    } // end if
  } // end if

  return pSession;
} // end DBMSrvKnl_GlobalSessions::CreateSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::GetOrCreateSession
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_Session * DBMSrvKnl_GlobalSessions::GetOrCreateSession
    ( SessionType       oType,
      const char      * szDatabase,
      DBMSrvMsg_Error & oError )
{
  DBMSrvKnl_Session * pSession = GetSession(oType);
  if (pSession == NULL) {
    pSession = CreateSession(oType, szDatabase, oError);
  } // end if
  return pSession;
} // end DBMSrvKnl_GlobalSessions::GetOrCreateSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::GetOrCreateSession
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_Session * DBMSrvKnl_GlobalSessions::GetOrCreateSession
    ( SessionType       oType,
      const char      * szDatabase,
      const char      * szConnect,
      DBMSrvMsg_Error & oError )
{
  DBMSrvKnl_Session * pSession = GetSession(oType);
  if (pSession == NULL) {
    pSession = CreateSession(oType, szDatabase, szConnect, oError);
  } // end if
  return pSession;
} // end DBMSrvKnl_GlobalSessions::GetOrCreateSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::GetOrCreateSession
  -----------------------------------------------------------------------------
 */
DBMSrvKnl_Session * DBMSrvKnl_GlobalSessions::GetOrCreateSession
    ( SessionType       oType,
      const char      * szDatabase,
      const char      * szUser,
      const char      * szPassword,
      DBMSrvMsg_Error & oError )
{
  DBMSrvKnl_Session * pSession = GetSession(oType);
  if (pSession == NULL) {
    pSession = CreateSession(oType, szDatabase, szUser, szPassword, oError);
  } // end if
  return pSession;
} // end DBMSrvKnl_GlobalSessions::GetOrCreateSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::ReleaseSession
  -----------------------------------------------------------------------------
 */
void DBMSrvKnl_GlobalSessions::ReleaseSession
    ( SessionType       oType,
      DBMSrvMsg_Error & oError,
      bool              keepSessionObject )
{
  // special handling for service sesssion because if we have this session
  // we must stop the service database
  if (oType == SessionService) {
    if (GetSession(SessionService) != NULL && GetSession(SessionService)->isConnected()) {
      Msg_List oMsg;
      RTE_ActiveDatabase * pDatabase = (RTE_DBRegister::Instance()).GetActiveDatabase( GetSession(SessionService)->GetDatabase(), oMsg);
      if (oMsg.IsEmpty()) {
        pDatabase->Stop(oMsg, 90);
      } // end if
    } // end if
  } // end if

  // check session value, get pointer and destroy the session object
  if ((oType > SessionMinValue) && (oType < SessionMaxValue)) {
    if (m_oSessions[oType].m_pPointer != NULL) {
/*
      DBMSrvMsg_Error oErrorToIgnore;

      if( m_oSessions[oType].m_pPointer->IsRequestPending() ) {
        // there is a request pending and we did not ask for the answer yet
        // so cancel it
        m_oSessions[oType].m_pPointer->Cancel();
        // to avoid unbalanced request/reply error, try to receive the answer and ignore the error
        if( oType == SessionEvent ) {
            tsp31_event_description eventDescription;
            m_oSessions[oType].m_pPointer->EventReceive(&eventDescription, sizeof(eventDescription), oErrorToIgnore);
        } else {
            DBMSrvKnl_Session::ResultType resultType;
            m_oSessions[oType].m_pPointer->Receive(resultType, oErrorToIgnore);
        }
      }
*/
      m_oSessions[oType].m_pPointer->Release(oError, keepSessionObject);

      if(!keepSessionObject)
      {
        delete m_oSessions[oType].m_pPointer;
        m_oSessions[oType].m_pPointer = NULL;
      }
    } // end if
  } // end if
} // end DBMSrvKnl_GlobalSessions::ReleaseSession

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_GlobalSessions::ReleaseAll
  -----------------------------------------------------------------------------
 */
void DBMSrvKnl_GlobalSessions::ReleaseAll
    ( DBMSrvMsg_Error & oError,
      bool              keepDBSessionObjectAndServiceSession)
{
  cn31FreeBackupSession(cn00DBMServerData::vcontrol());

  ReleaseSession(SessionDatabase, oError, keepDBSessionObjectAndServiceSession);
  ReleaseSession(SessionSQL, oError);
  ReleaseSession(SessionInfo, oError);
  ReleaseSession(SessionBackup, oError);
  ReleaseSession(SessionEvent, oError);

  if(!keepDBSessionObjectAndServiceSession)
    ReleaseSession(SessionService, oError);
} // end DBMSrvKnl_GlobalSessions::ReleaseAll
