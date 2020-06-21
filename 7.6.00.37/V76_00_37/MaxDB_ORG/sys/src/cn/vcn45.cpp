/*
  -----------------------------------------------------------------------------

  module: vcn45.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer high level commands

  description:  DBMServer high level commands - Implementation

  -----------------------------------------------------------------------------


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



*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>

// this module
#include "hcn45.h"

// cn modules
#include "hcn20.h"
#include "hcn40.h"
#include "hcn50.h"
#include "hcn90.h"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

/*
  -----------------------------------------------------------------------------
  definitions
  -----------------------------------------------------------------------------
 */
#define DB_DISABLE_LOG      "SET LOG WRITER OFF"
#define DB_ENABLE_LOG       "SET LOG WRITER ON"
#define DB_MIGRATE_CATALOG  "MIGRATE TO UNICODE"
#define DB_CREATE           "CREATE INSTANCE SYSDBA %s PASSWORD %s"

/*
  -----------------------------------------------------------------------------
  public function cn45MigrateCatalog
  -----------------------------------------------------------------------------
*/
tcn00_Error cn45MigrateCatalog
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn45MigrateCatalog");

  tcn00_Error    nFuncReturn = OK_CN00;
  tcn002_XpValueString szOldPacketSize;
  bool           bParamsSet = false;
 DBMSrvMsg_Error oParamGetMsg;

  // 1. check the database state; it has to be offline
  if (nFuncReturn == OK_CN00) {
    if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOffline) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_DBRUN));
    } // end if
  } // end if

  // 2. check the param dirty flag; it has to be clear
  if (nFuncReturn == OK_CN00) {
    tsp00_Int4        nValue;
    if (cn20XParamGetValue(vcontrol->dbname, PAN_XPARAM_CHANGED, nValue, oParamGetMsg)) {
      if (nValue == 1) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_XPDIRTY));
      } // end if
    } else {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);
    } // end if
  } // end if

  // 3. check the _UNICODE parameter; it has to be "NO"
  if (nFuncReturn == OK_CN00) {
    tcn002_XpValueString     szValue;
    if (cn20XParamGetValue(vcontrol->dbname, PAN_UNICODE_CATALOG, szValue, oParamGetMsg)) {
      if (strcmp(szValue, "NO") != 0) {
        DBMSrvMsg_Error aEvent(SDBMSG_DBMSRV_XPVALUNEX,
                               Msg_Arg(SDBMSGTAG_DBMSRV_XPVALUNEX__VALUE,PAN_UNICODE_CATALOG),
	                             Msg_Arg(SDBMSGTAG_DBMSRV_XPVALUNEX__DATABASE_PARAMETER,szValue));
        aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
      } // end if
    } else {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);
    } // end if
  } // end if

  // 4. Read _PACKET_SIZE
  if (nFuncReturn == OK_CN00) {
    if (!cn20XParamGetValue(vcontrol->dbname, PAN_PACKET_SIZE, szOldPacketSize, oParamGetMsg)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);
    } // end if
  } // end if
  
  // 5. change and check parameters
  if (nFuncReturn == OK_CN00) {
    const char * szNames[]  = {PAN_PACKET_SIZE, PAN_UNICODE_CATALOG};
    const char * szValues[] = {"131072"       , "YES"};

    command->args = "";
    nFuncReturn = cn20CheckedParamPut ( vcontrol,
                                        command,
                                        replyData,
                                        replyLen,
                                        replyLenMax,
                                        2,
                                        szNames,
                                        szValues);
    bParamsSet = (nFuncReturn == OK_CN00);
  } // end if

  // 6. database to admin
  if (nFuncReturn == OK_CN00) {
    command->args = "";
    nFuncReturn = cn40DBCold( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax);
  } // end if

  // 7. disable log
  if (nFuncReturn == OK_CN00) {
    // execute kernel command
    cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    DBMSrvMsg_Error oError;
    if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                     oUser.getUserName().asCharp(),
                                     oUser.getClearMasterPwd().asCharp(),
                                     DB_DISABLE_LOG,
                                     oError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  // 8. migrate catalaog
  if (nFuncReturn == OK_CN00) {
    // execute kernel command
    cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    DBMSrvMsg_Error oError;
    if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                     oUser.getUserName().asCharp(),
                                     oUser.getClearMasterPwd().asCharp(),
                                     DB_MIGRATE_CATALOG,
                                     oError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  // 9. on error reset params
  if ((nFuncReturn != OK_CN00) && bParamsSet) {
    char         tmpReplyData[1001];
    int          tmpReplyLen;
    const char * szNames[]  = {PAN_PACKET_SIZE,           PAN_UNICODE_CATALOG};
    const char * szValues[] = {szOldPacketSize.asCharp(), "NO"};

    command->args = "";
    cn20CheckedParamPut ( vcontrol,
                          command,
                          tmpReplyData,
                          &tmpReplyLen,
                          1000,
                          2,
                          szNames,
                          szValues);
  } // end if

  // 10. check database state
  if (nFuncReturn == OK_CN00) {
    if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
    } // end if
  } // end if

  // 11. load system tables
  if (nFuncReturn == OK_CN00) {
    command->args = "-m";
    nFuncReturn = cn40LoadSysTab( vcontrol,
                                  command,
                                  replyData,
                                  replyLen,
                                  replyLenMax);
  } // end if

  // 12. database to offline
  if (nFuncReturn == OK_CN00) {
    command->args = "";
    nFuncReturn = cn40DBOffline( vcontrol,
                                 command,
                                 replyData,
                                 replyLen,
                                 replyLenMax);
  } // end if

  // 13. reset packetsize
  if (nFuncReturn == OK_CN00) {
    const char * szNames[]  = {PAN_PACKET_SIZE};
    const char * szValues[] = {szOldPacketSize.asCharp()};

    command->args = "";
    nFuncReturn = cn20CheckedParamPut ( vcontrol,
                                        command,
                                        replyData,
                                        replyLen,
                                        replyLenMax,
                                        1,
                                        szNames,
                                        szValues);
  } // end if

  // 14. database to admin
  if (nFuncReturn == OK_CN00) {
    command->args = "";
    nFuncReturn = cn40DBCold( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax);
  } // end if

  // 15. enable log
  if (nFuncReturn == OK_CN00) {
    // execute kernel command
    cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    DBMSrvMsg_Error oError;
    if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                     oUser.getUserName().asCharp(),
                                     oUser.getClearMasterPwd().asCharp(),
                                     DB_ENABLE_LOG,
                                     oError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  // 16. database to warm
  if (nFuncReturn == OK_CN00) {
    command->args = "";
    nFuncReturn = cn40DBWarm( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn45MigrateCatalog

/*
  -----------------------------------------------------------------------------
  public function cn45Reinstall
  -----------------------------------------------------------------------------
*/
tcn00_Error cn45Reinstall
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn45Reinstall");

  tcn00_Error    nFuncReturn = OK_CN00;

  // 1. check the database state; it has to be offline
  if (nFuncReturn == OK_CN00) {
    if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOffline) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_DBRUN));
    } // end if
  } // end if


  // 2. Load and check Users
  cn50DBMUser  usrDBA(vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));
  if (nFuncReturn == OK_CN00) {
    if (!usrDBA.lastEvent().IsEmpty()) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAUNKNOWN));
    } // end if
  } // end if

  // 3. database to admin
  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn40DBCold( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax);
  } // end if

  // 3. create instance
  if (nFuncReturn == OK_CN00) {
    char szCommand[1000];
    sprintf(szCommand, DB_CREATE, usrDBA.getUserName().asCharp(), usrDBA.getClearMasterPwd().asCharp());
    // execute kernel command
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

  // 4. load system tables
  if (nFuncReturn == OK_CN00) {
    const char * pArgs = command->args;
    command->args = "";
    nFuncReturn = cn40LoadSysTab( vcontrol,
                                  command,
                                  replyData,
                                  replyLen,
                                  replyLenMax);
    command->args = pArgs;
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn45Reinstall

/*
  -----------------------------------------------------------------------------
  public function cn45DBInstall
  -----------------------------------------------------------------------------
*/
/*
tcn00_Error cn45DBInstall
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn45Reinstall");

  tcn00_Error    nFuncReturn = OK_CN00;

  // expat test
  XML_Parser oParser;

  oParser = XML_ParserCreate(NULL);
  
  if (!oParser) {
    nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK(replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn45DBInstall
*/