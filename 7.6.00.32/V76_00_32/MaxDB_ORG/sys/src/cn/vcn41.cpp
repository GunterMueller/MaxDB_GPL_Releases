/* 
  -----------------------------------------------------------------------------
 
  module: vcn41.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Configuration
   
  description:  DBMServer Configuration - Implementation

  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "vsp001.h"
#include "vsp009c.h"

//#include "hin101.h"

#include "hcn20.h"
#include "hcn50.h"
#include "hcn42.h"
#include "hcn51.h"
#include "hcn53.h"
#include "hcn90.h"
#include "hcn41.h"

#ifndef _WIN32
  #include "RunTime/RTE_UNIXAuthenticate.hpp"           // nocheck
#endif
#include "RunTime/RTE_UserProfileContainer.hpp"

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"


#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"

/*
  -----------------------------------------------------------------------------
  function:     cn41UserLogon
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserLogon
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  _TCHAR              szToken    [PARAMETER_MAXLEN_CN90];
  _TCHAR            * pPassword   = NULL; 
  tcn00_UserNamec     szUser;
  tsp00_Namec         szPassword;
  tsp9_rte_xerror     xError;
  tcn002_XpValueString szColdUser;
  tcn002_XpValueString szColdPwd;
  bool                bErrSysLogon = false;

  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  pPassword = _tcschr(szToken, PWD_SEPARATOR_CN00);
  if (pPassword != NULL) {
    *pPassword = CHAR_STRINGTERM_CN90;
    pPassword++;
    cn90Uncrypt(pPassword, false);
    szUser.rawAssign(szToken);
    szPassword.rawAssign(pPassword);

    vcontrol->pCurrentUser->setDBName   ( vcontrol->dbname )
                           .setUserName ( szUser );

    nFuncReturn = vcontrol->pCurrentUser->reload();

    if (nFuncReturn == ERR_USRREAD_CN00) {
      if (cn50DBMUser::isEmpty(vcontrol->dbname)) {
        if (cn50DBMUser::tryRestoreIfEmpty(vcontrol->dbname)) {
          nFuncReturn = vcontrol->pCurrentUser->reload();
        } // end if
      } // end if
    } // end if
  
    if (nFuncReturn == ERR_USRREAD_CN00) {
      if (cn50DBMUser::isEmpty(vcontrol->dbname)) {
        // Yes, upc is empty -> create user
        nFuncReturn = OK_CN00;

        // check User against param file 
        DBMSrvMsg_Error oParamGetMsg;
        if (cn20XParamGetValue(vcontrol->dbname, PAN_CONTROLID, szColdUser, oParamGetMsg) &&
            cn20XParamGetValue(vcontrol->dbname, PAN_CONTROLPW, szColdPwd, oParamGetMsg)     ) {
          cn90Uncrypt( szColdPwd, false);
          if (_tcsicmp(szColdUser, szUser)     != 0 || 
              _tcsicmp(szColdPwd,  szPassword) != 0    ) {
            nFuncReturn = ERR_USRFAIL_CN00;
          } // end if
        } // end if

        if (nFuncReturn == OK_CN00) {
          vcontrol->pCurrentUser->setMasterPwd    ( szPassword  )
                                  .setColdUser     ( )
                                  .setServerRights ( DBMMaskAll_CN50, DBMUserDefault_CN50 )
                                  .setGUIRights    ( DBMMaskAll_CN50, DBMUserDefaultGUI_CN50 );

          nFuncReturn = vcontrol->pCurrentUser->save(true);
          if (nFuncReturn != OK_CN00) {
            nFuncReturn = (nFuncReturn != ERR_USRFRM_CN00) ? ERR_USRFAIL_CN00 : nFuncReturn;
          } // end if
        } // end if 
      } else {
        // No, upc is not empty -> wrong username!
        nFuncReturn = ERR_USRFAIL_CN00;
      } // end if 
    } // end if 

    if (nFuncReturn == OK_CN00) {
      if (!vcontrol->pCurrentUser->checkMasterPwd( szPassword) &&
          !vcontrol->pCurrentUser->checkSecondPwd( szPassword)    ) {
        nFuncReturn = ERR_USRFAIL_CN00;
      } // end if
    } // end if
  } else {
    nFuncReturn = ERR_PARAM_CN00;
  } // end if

  if (nFuncReturn == OK_CN00 || bErrSysLogon) {

    if (vcontrol->dbname.length() > 0) {
      cn51DBMConfigValue  cfgDbaDtl (vcontrol->dbname, CFG_DBADTL_CN51);
      if (((int)cfgDbaDtl) != 0) {
        tcn53_DBAAction::enable ();
      } // end if
    } // end if

    // check Unicode
    if (vcontrol->dbname.length() > 0) {
      tcn002_XpValueString  szUnicode;
      DBMSrvMsg_Error       oParamGetMsg;
      if (cn20XParamGetValue(vcontrol->dbname, PAN_UNICODE_CATALOG, szUnicode, oParamGetMsg)) {
        vcontrol->bUnicode = (stricmp(szUnicode, XP_VAL_YES_CN00) == 0);
      } // end if

    } // end if

  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
    vcontrol->pHSSNodes->SetConnectionData(vcontrol->dbname.asCharp(), szUser.asCharp(), szPassword.asCharp());
  } else {
    if (bErrSysLogon) {
      _stprintf(replyData, "%s%s%s%s%s%s",
                           ANSWER_OK_CN00,
                           LINE_SEPSTRING_CN00,
                           _T("INFO: Logon to operating system failed!"),
                           LINE_SEPSTRING_CN00,
                           xError.xe_text,
                           LINE_SEPSTRING_CN00);
      *replyLen = (int)_tcslen(replyData);
    } else {
      vcontrol->pCurrentUser->clear();
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } // end if
  } // end if

  return nFuncReturn;
} // cn41UserLogon

/*
  -----------------------------------------------------------------------------
  function:     cn41UserSystem
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserSystem
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;

  _TCHAR              szToken    [PARAMETER_MAXLEN_CN90];
  _TCHAR            * pPassword   = NULL; 
  tcn00_UserNamec     szUser;
  tsp00_Namec         szPassword;

  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  pPassword = _tcschr(szToken, PWD_SEPARATOR_CN00);
  if (pPassword != NULL) {
    *pPassword = CHAR_STRINGTERM_CN90;
    pPassword++;
    cn90Uncrypt(pPassword, false);
    szUser.rawAssign(szToken);
    szPassword.rawAssign(pPassword);

#ifndef _WIN32      
      Msg_List oMsg;
      oMsg.ClearMessageList();
      if (RTE_VerifyUserPasswordCombination(szUser.asCharp(), szPassword.asCharp(), oMsg)) {
        vcontrol->bIsSAPDBAdmi = RTE_VerifyUserAsSapdbAdministrator (szUser.asCharp(), oMsg);
      } else {
        DBMSrvMsg_RTEError rteError(oMsg);
        rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NEEDADMI));
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(rteError);
      } // end if
#else
      tsp9_rte_xerror     xError;
      sqlxlogon (szUser.asCharp(), szPassword.asCharp(), &xError);
      if (xError.xe_result != 0) {
        DBMSrvMsg_RTEError rteError(xError);
        rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SYSLOGON));
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(rteError);
      } else {
        vcontrol->bIsSAPDBAdmi = true;
      } // end if
#endif

//    sqlxlogon (szUser.asCharp(), szPassword.asCharp(), &xError);
//    if (xError.xe_result != 0) {
//      nFuncReturn = cn90AnswerRTEError (replyData, replyLen, ERR_SYSLOGON_CN00, xError.xe_text, sizeof(xError.xe_text), xError.xe_result);
//    } else {
//      vcontrol->bSysLogon = true;
//    } // end if
  
  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // cn41UserSystem

/*
  -----------------------------------------------------------------------------
  function:     cn41UserSap
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserSap
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn41UserSap");

  tcn00_Error         nFuncReturn = OK_CN00;

  char                szToken    [PARAMETER_MAXLEN_CN90];
  char              * pPassword   = NULL; 
  tcn00_UserNamec     szUser;
  tsp00_Namec         szPassword;

  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  pPassword = strchr(szToken, PWD_SEPARATOR_CN00);
  if (pPassword != NULL) {
    *pPassword = CHAR_STRINGTERM_CN90;
    pPassword++;
    cn90Uncrypt(pPassword, false);
    szUser.rawAssign(szToken);
    szPassword.rawAssign(pPassword);

    // check new sap user
    cn50DBMUser checkNewSapUser( vcontrol->dbname, szUser);
    if (checkNewSapUser.existsOnDisk() && !checkNewSapUser.isSAPUser()) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_USREXISTS_CN00);
    } else {
      // delete old user
      cn50DBMUser oldSapUser(vcontrol->dbname, cn50DBMUser::getSAPUser(vcontrol->dbname));
      oldSapUser.deleteUsr();

      // create new user
      cn50DBMUser newSapUser( vcontrol->dbname );
      newSapUser.setUserName  ( szUser )
                .setMasterPwd ( szPassword )
                .setSAPUser   ( true );
    
      nFuncReturn = newSapUser.save();
      if (nFuncReturn != OK_CN00) {
        if (!newSapUser.lastEvent().IsEmpty()) {
           nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, newSapUser.lastEvent());
        } else {
          cn90AnswerIError(replyData, replyLen, nFuncReturn);
        } // end if
      } // end if
    } // end if
  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // cn41UserSap

/*
  -----------------------------------------------------------------------------
  function:     cn41UserSysdba
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserSysdba
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn41UserSysdba");

  tcn00_Error         nFuncReturn = OK_CN00;

  char                szToken    [PARAMETER_MAXLEN_CN90];
  char              * pPassword   = NULL; 
  tcn00_UserNamec     szUser;
  tsp00_Namec         szPassword;

  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  pPassword = strchr(szToken, PWD_SEPARATOR_CN00);
  if (pPassword != NULL) {
    *pPassword = CHAR_STRINGTERM_CN90;
    pPassword++;
    cn90Uncrypt(pPassword, false);
    szUser.rawAssign(szToken);
    szPassword.rawAssign(pPassword);
    
    cn50DBMUser oldUser(vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));
    oldUser.deleteUsr();

    cn50DBMUser newUser( vcontrol->dbname );
    newUser.setUserName     ( szUser )
           .setMasterPwd    ( szPassword )
           .setSDBAUser     ( true )
           .setServerRights ( DBMMaskAll_CN50, DBMUserDefault_CN50 )
           .setGUIRights    ( DBMMaskAll_CN50, DBMUserDefaultGUI_CN50 );

    nFuncReturn = newUser.save();
    if (nFuncReturn != OK_CN00) {
      if (!newUser.lastEvent().IsEmpty()) {
         nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, newUser.lastEvent());
      } else {
        cn90AnswerIError(replyData, replyLen, nFuncReturn);
      } // end if
    } // end if
      
  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // cn41UserSysdba

/*
  -----------------------------------------------------------------------------
  function:     cn41UserCreate
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserCreate
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  _TCHAR              szToken    [PARAMETER_MAXLEN_CN90];
  _TCHAR            * pPassword   = NULL; 
  tcn00_UserNamec     szUser;
  tsp00_Namec         szPassword;
  _TCHAR              szTemplate  [PARAMETER_MAXLEN_CN90];
  cn50DBMUser         dbUser;
  bool                bTemplate   = false;
    
  // read parameters
  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  if ( cn90GetToken(command->args, szTemplate, 2, PARAMETER_MAXLEN_CN90)) {
    bTemplate = true;
  } // end if

  // extract password
  pPassword = _tcschr(szToken, PWD_SEPARATOR_CN00);
  if (pPassword != NULL) {
    *pPassword = CHAR_STRINGTERM_CN90;
    pPassword++;
    szUser.rawAssign(szToken);
    cn90StripQuotes(szUser.asCharp());
    szPassword.rawAssign(pPassword);
    cn90StripQuotes(szPassword.asCharp());
  } else {
    nFuncReturn = ERR_PARAM_CN00;
  } // end if

  if (nFuncReturn == OK_CN00) {
    if (!ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(szUser    .asCharp()) || 
        !ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(szPassword.asCharp())    ) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1, "Invalid database manager operator");
    } // end if
  } // end if

  // check existence of user
  if (nFuncReturn == OK_CN00) {
    dbUser.setDBName   ( vcontrol->dbname )
          .setUserName ( szUser );

    if (dbUser.reload() == OK_CN00) {
      nFuncReturn = ERR_USREXISTS_CN00;
    } // end if
  } // end if

  // read Template
  if (nFuncReturn == OK_CN00 && bTemplate) {
    dbUser.setDBName   ( vcontrol->dbname )
          .setUserName ( *(tcn00_UserNamec *) szTemplate );

    nFuncReturn = dbUser.reload();
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90Uncrypt(pPassword, false);
    dbUser.setDBName    ( vcontrol->dbname )
          .setUserName  ( szUser )
          .setMasterPwd ( szPassword )
          .setColdUser  ( false )
          .setSDBAUser  ( false );
    
    nFuncReturn = dbUser.save();
  } // end if
  
  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if

  return nFuncReturn;
} // cn41UserCreate

/*
  -----------------------------------------------------------------------------
  function:     cn41UserGet
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserGet
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  _TCHAR       * pData;
  _TCHAR         szName      [PARAMETER_MAXLEN_CN90];

  cn90GetToken(command->args, szName, 1, PARAMETER_MAXLEN_CN90);

  cn50DBMUser  dbUser( vcontrol->dbname, 
                       *(tcn00_UserNamec *) szName );
  
  nFuncReturn = dbUser.reload();
  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
    pData = replyData + _tcslen(replyData);
    dbUser.getData(pData);
    *replyLen = (int)_tcslen(replyData);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if

  return nFuncReturn;
} // cn41UserGet

/*
  -----------------------------------------------------------------------------
  function:     cn41UserGetRights
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserGetRights
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  _TCHAR       * pData;
  _TCHAR         szName      [PARAMETER_MAXLEN_CN90];
  _TCHAR         szRights    [PARAMETER_MAXLEN_CN90];

  cn90GetToken(command->args, szName, 1, PARAMETER_MAXLEN_CN90);
  cn90GetToken(command->args, szRights, 2, PARAMETER_MAXLEN_CN90);

  cn50DBMUser  dbUser( vcontrol->dbname, 
                       *(tcn00_UserNamec *) szName );
    
  nFuncReturn = dbUser.reload();
  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
    pData = replyData + _tcslen(replyData);
    nFuncReturn = dbUser.listRights(pData, szRights);
  } // end if
  
  if (nFuncReturn == OK_CN00) {
    *replyLen = (int)_tcslen(replyData);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if

  return nFuncReturn;
} // cn41UserGetRights

/*
  -----------------------------------------------------------------------------
  function:     cn41UserGetAll
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserGetAll
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn41UserGetAll");

  tcn00_Error             nFuncReturn = OK_CN00;
  Msg_List        EventList;

  RTE_UserProfileContainer Profile;
  Profile.Open(vcontrol->dbname, true, EventList);

  tsp00_Uint4             nToken = FIRST_TOKEN;
  tcn00_UserNamec         szUserName;
  char                  * pCurrent = replyData;
  tsp00_Int4              nLength = 0;

  if (EventList.IsEmpty()) {
    // Init OK Message
    sprintf(pCurrent, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrent = pCurrent + strlen(pCurrent);

    do {
      EventList.ClearMessageList();
      Profile.ScanProfiles(nToken, nLength, NULL, szUserName, EventList);

      if(EventList.IsEmpty()) {
        if (nToken == NO_TOKEN) {
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_NOUSERS_CN00);
        } else {
          cn50DBMUser aUser(vcontrol->dbname, szUserName);
          if (!aUser.isSAPUser()) {
            sprintf(pCurrent, "%s%s", szUserName.asCharp(), LINE_SEPSTRING_CN00);
            pCurrent = pCurrent + strlen(pCurrent);
          } // end if
        } // end if
      } // end if

    } while (nToken != LAST_TOKEN && EventList.IsEmpty() && nToken != NO_TOKEN);

  } // end if

  if (!EventList.IsEmpty()) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(EventList));
  } // end if

  *replyLen = (int)_tcslen(replyData);

  return nFuncReturn;
} // cn41UserGetAll

/*
  -----------------------------------------------------------------------------
  function:     cn41UserPut
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserPut
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  _TCHAR         szName      [PARAMETER_MAXLEN_CN90];
  const _TCHAR * pData       = NULL;
  cn50DBMUser    dbUser;
  
  cn90GetToken(command->args, szName, 1, PARAMETER_MAXLEN_CN90);

  dbUser.setDBName  ( vcontrol->dbname )
        .setUserName( *(tcn00_UserNamec *) szName           );

  nFuncReturn = dbUser.reload();

  if (nFuncReturn == OK_CN00) {
    pData = _tcschr(command->args, _T(' '));
    
    nFuncReturn = dbUser.setData(pData);

    if (nFuncReturn == OK_CN00) {
      nFuncReturn = dbUser.save();
      if (nFuncReturn == OK_CN00) {
        if (vcontrol->pCurrentUser->isThis(*(tcn00_UserNamec *) szName)) {
          nFuncReturn = vcontrol->pCurrentUser->reload();
        } // end if
      } // end if
    } // end if

  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if

  return nFuncReturn;
} // cn41UserPut

/*
  -----------------------------------------------------------------------------
  function:     cn41UserDelete
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserDelete
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  _TCHAR         szName      [PARAMETER_MAXLEN_CN90];
  cn50DBMUser    dbUser;
  
  cn90GetToken(command->args, szName, 1, PARAMETER_MAXLEN_CN90);
  dbUser.setDBName  ( vcontrol->dbname )
        .setUserName( *(tcn00_UserNamec *) szName           );

  nFuncReturn = dbUser.reload();

  if (nFuncReturn == OK_CN00) {
    if (vcontrol->pCurrentUser->isThis(dbUser.getUserName())) {
      nFuncReturn = ERR_USRDELSELF_CN00;
    } else if (dbUser.isColdUser()) {
      nFuncReturn = ERR_USRDELDBM_CN00;
    } else if (dbUser.isSDBAUser()) {
      nFuncReturn = ERR_USRDELSDBA_CN00;
    } else {
      nFuncReturn = dbUser.deleteUsr();
    } // end if

  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } else {
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if

  return nFuncReturn;
} // end cn41UserDelete
