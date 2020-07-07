/*! @lastChanged: "1998-06-24  10:45"
 *
 * @file    :    vcn80.cpp
 * @purpose:     "utility and sql access"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * @Copyright (c) 1998-2005 SAP AG"
 *
 * ==================================================================
 *
 * responsible:   Martin Reinecke (D024853 - Martin.Reinecke@SAP-AG.de)
 *                Bernd Vorsprach (D025588 - Bernd.Vorsprach@SAP-AG.de)
 *
 * created:       1998-03-19 by Martin Reinecke
 *
 * purpose:       utility connect/execute/release
 *
 * ==================================================================


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#include "vsp009c.h"

#include "hcn20.h"
#include "hcn31.h"
#include "hcn51.h"
#include "hcn80.h"
#include "hcn90.h"

#include "DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"
#include "SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbConnect.hpp"
#include "SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbRelease.hpp"
#include "SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbExecuteNice.hpp"

/* ------------------------------------------------------------------
 * SPECIFICATION PRIVATE TYPES/MACROS
 * ------------------------------------------------------------------
 */
#define PARAM_RECOVER         "RECOVER"
#define DB_CREATE           "CREATE INSTANCE SYSDBA %s PASSWORD %s"

/* PUBLIC FUNCTION cn80ServiceConnect
 * ------------------------------------------------------------------
 * purpose: open a utility session with the service kernel
 *
 */
tcn00_Error cn80ServiceConnect (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp00_DbNamec       szDBName;
  tcn00_UserNamec     szUserName;
  tsp00_Namec         szPassword;
  tsp9_rte_xerror     xError;
  bool                bStarted = false; 


  DBMSrvKnl_Session * pSession = DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionService);

  if (pSession == NULL) {
    //  get parameter of service kernel
    nFuncReturn =  cn20XParamGetServiceValues ( szDBName,
                                                vcontrol->dbroot,
                                                szUserName,
                                                szPassword );

    //  start service kernel
    if (nFuncReturn == OK_CN00) {
      cn20CreateSrvParam(szDBName, vcontrol->dbname);

      sqlxstart (szDBName, csp9_fast_pgm, vcontrol->dbroot, false, 0, NULL, &xError);

      if (xError.xe_result != csp9_xrte_ok) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
      } else {
        bStarted = true;
      } // end if
    } else {
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
    } // end if

    // connect
    if (nFuncReturn == OK_CN00) {
      DBMSrvMsg_Error oError;
      pSession = DBMSrvKnl_GlobalSessions::CreateSession(DBMSrvKnl_GlobalSessions::SessionService,
                                                         szDBName,
                                                         szUserName,
                                                         szPassword,
                                                         oError);
      if (pSession == 0) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
    } // end if

    // stop kernel in case of errors
    if (nFuncReturn != OK_CN00 && bStarted) {
      sqlxstop (szDBName, vcontrol->dbroot, false, &xError);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} /* end cn80ServiceConnect */

/* PUBLIC FUNCTION cn80ServiceRelease
 * ------------------------------------------------------------------
 * purpose: release the utility session with the service kernel
 *
 */
tcn00_Error cn80ServiceRelease (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  cn90AnswerOK(replyData, replyLen, NULL);
  DBMSrvMsg_Error errOut;
  DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionService, errOut);
  if( !errOut.IsEmpty() )
      return DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(errOut);
  return OK_CN00;
} /* end cn80ServiceRelease */

static void cn80_PrepareActivate
  (tsp00_DbNamec szDbName )
{
    // delete possible existing sysdev parameters
    tsp00_XpKeyTypec szName;
    szName.rawAssign("SYSDEV_001");
    cn20XParamDelete(szDbName, szName);
    szName.rawAssign(PAN_MAXSYSDEVSPACES);
    cn20XParamDelete(szDbName, szName);

    // disable autosave
    cn51DBMConfigValue cfgAuto (szDbName, CFG_AUTOSSAVE_CN51);
    cfgAuto = "";

    // disable volume migration
    cn51DBMConfigValue oMigrVol( szDbName, CFG_CFGMIGRVOL_CN51 );
    oMigrVol = "";

    // disable database migration
    cn51DBMConfigValue oMigrDB( szDbName, CFG_CFGMIGRDB_CN51 );
    oMigrDB  = "";
} // cn80_PrepareActivate

/* PUBLIC FUNCTION cn80DBActivate
 * ------------------------------------------------------------------
 * purpose: execute actvtivate serverdb
 *
 */
tcn00_Error cn80DBActivate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	        nFuncReturn = OK_CN00;
  char                szToken    [PARAMETER_MAXLEN_CN90];
  bool                bRecover  = false;

  tcn00_UserNamec     szUser;
  tsp00_Namec         szPassword;

  /**/

  if (!cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90)) {
    cn50DBMUser  usrDBA(vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));
    if (usrDBA.existsOnDisk() ) {
      szUser     = usrDBA.getUserName();
      szPassword = usrDBA.getClearMasterPwd();
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAUNKNOWN_CN00);
    } // end if
  } else {
    if (stricmp(szToken, PARAM_RECOVER) == 0) {
      bRecover = true;
    } else {
      char * pPassword = strchr(szToken, PWD_SEPARATOR_CN00);
      if (pPassword != NULL) {
        *pPassword = CHAR_STRINGTERM_CN90;
        pPassword++;
        cn90Uncrypt(pPassword, false);
        szUser.rawAssign(szToken);
        szPassword.rawAssign(pPassword);

        if (cn90GetToken(command->args, szToken, 2, PARAMETER_MAXLEN_CN90)) {
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_TOOMANYPARAM_CN00);
        } else {
          if (!ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(szUser    .asCharp()) || 
              !ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(szPassword.asCharp())    ) {
            nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1, "Invalid database system administrator");
          } // end if
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } // end if
    } // end if
  } // end if
  
  if (nFuncReturn == OK_CN00) {
    if (bRecover) {
      cn80_PrepareActivate(vcontrol->dbname);

      command->args = command->args + strlen(PARAM_RECOVER) + 1;
      nFuncReturn =  cn31BackupRestoreCreate (vcontrol, command, replyData, replyLen, replyLenMax );
    } else {

      // check db state
      if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateAdmin) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_NOTADMIN_CN00);
      } // end if

      if (nFuncReturn == OK_CN00) {
        cn80_PrepareActivate(vcontrol->dbname);
      } // end if

      // place here the utility command
      if (nFuncReturn == OK_CN00) {
        char  szCommand[PARAMETER_MAXLEN_CN90];
        sprintf(szCommand, DB_CREATE, szUser.asCharp(), szPassword.asCharp());

        cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
        DBMSrvMsg_Error oError;
        if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                        oUser.getUserName().asCharp(),
                                        oUser.getClearMasterPwd().asCharp(),
                                        szCommand,
                                        oError)) {
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
        } // end if
      } // end if 

      if (nFuncReturn == OK_CN00) {
        // delete old sysdba-user
        cn50DBMUser         usrOldDBA(vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));
        usrOldDBA.setSDBAUser(false);
        usrOldDBA.deleteUsr();
        
        // create new sysdba-user
        cn50DBMUser         usrDBA;

        // save data of new sysdba-user
        usrDBA.setDBName(vcontrol->dbname)
              .setUserName ( szUser)
              .setSDBAUser ( )
              .setMasterPwd( szPassword)
              .setServerRights ( DBMMaskAll_CN50, DBMUserDefault_CN50 )
              .setGUIRights    ( DBMMaskAll_CN50, DBMUserDefault_CN50 );

        nFuncReturn = usrDBA.save();
        if (nFuncReturn != OK_CN00) {
          cn90AnswerIError(replyData, replyLen, nFuncReturn);
        } // end if
      } // end if

      if (nFuncReturn == OK_CN00) {
        cn90AnswerOK(replyData, replyLen, NULL);
      } // end if 

    } // end if

  } // end if

  return nFuncReturn;
} /* end cn80DBActivate */

/* PUBLIC FUNCTION cn80DBActivateRequest
 * ------------------------------------------------------------------
 * purpose: execute actvtivate serverdb
 *
 */
tcn00_Error cn80DBActivateRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  FUNCTION_DBG_MCN00_1("cn80DBActivateRequest");

  tcn00_Error	        nFuncReturn = OK_CN00;
  char                szToken    [PARAMETER_MAXLEN_CN90];

  /**/

  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  if (stricmp(szToken, PARAM_RECOVER) != 0) {
    DBMSrvMsg_Error aPARAM(SDBMSG_DBMSRV_MISSPARAM, Msg_Arg(SDBMSGTAG_DBMSRV_MISSPARAM__PARAMETER,"RECOVER"));
    aPARAM.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aPARAM);
  } // end if

  if (nFuncReturn == OK_CN00) {
      cn80_PrepareActivate(vcontrol->dbname);

      command->args = command->args + strlen(PARAM_RECOVER) + 1;
      nFuncReturn =  cn31BackupRestoreCreateRequest (vcontrol, command, replyData, replyLen, replyLenMax );
  } // end if

  return nFuncReturn;
} /* end cn80DBActivateRequest */

/* PUBLIC FUNCTION cn80UtilConnect
 * ------------------------------------------------------------------
 * purpose: open a utility session
 *
 */
tcn00_Error cn80UtilConnect (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  DBMSrvCmdDb_DbConnect oCommand;
  DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
  return oCommand.run(vcontrol, command, reply);
} /* end cn80UtilConnect */

/* PUBLIC FUNCTION cn80UtilRelease
 * ------------------------------------------------------------------
 * purpose: release the utility session
 *
 */
tcn00_Error cn80UtilRelease (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  DBMSrvCmdDb_DbRelease oCommand;
  DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
  return oCommand.run(vcontrol, command, reply);
} /* end cn80UtilRelease */

/* PUBLIC FUNCTION cn80UtilExecute
 * ------------------------------------------------------------------
 * purpose: release the utility session
 *
 */
tcn00_Error cn80UtilExecute (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  DBMSrvCmdDb_DbExecuteNice oCommand;
  DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
  return oCommand.run(vcontrol, command, reply);
} /* end cn80UtilExecute */
