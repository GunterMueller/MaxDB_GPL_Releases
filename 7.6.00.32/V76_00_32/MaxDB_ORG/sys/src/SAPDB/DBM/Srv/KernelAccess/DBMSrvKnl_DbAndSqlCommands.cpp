/*!
  @file           DBMSrvKnl_DbAndSqlCommands.cpp
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
#include "hcn50.h"
#include "hcn53.h"
#include "hcn90.h"

#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"

#include "DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp"

/*
  -----------------------------------------------------------------------------
  definitions
  -----------------------------------------------------------------------------
 */
#define STRINGMARK_CHAR   '\''
#define FIELDSEP_CHAR     ';'

#define CONTINUE_TXT      "CONTINUE"
#define END_TXT           "END     "
#define EMPTY_TXT         "        "
#define ASSIGN_TXT       " = "

#define MIN_WIDTH        30

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::connect
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvKnl_DbAndSqlCommands::connect
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply)
{
  if (DBMSrvKnl_State::State(vcontrol->dbname.asCharp()) == DBMSrvKnl_State::StateOffline) {
    return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
  } // end if

  tcn00_Error     nFuncReturn = OK_CN00; 
  DBMSrvMsg_Error oError;

  if (DBMSrvKnl_GlobalSessions::CreateSession(oType,
                                              vcontrol->dbname.asCharp(),
                                              command->args,
                                              oError) == NULL) {
    theReply.startWithMessageList(oError);
    nFuncReturn = oError.ID();
  } else {
    theReply.startWithOK();
  } // end if
  return nFuncReturn;
} // end DBMSrvKnl_DbAndSqlCommands::connect

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::release
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvKnl_DbAndSqlCommands::release
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply)
{
  DBMSrvMsg_Error errOut;
  DBMSrvKnl_GlobalSessions::ReleaseSession(oType, errOut);
  if( !errOut.IsEmpty() )
    return theReply.startWithMessageList(errOut);
  else {
    theReply.startWithOK();
    return OK_CN00;
  }
} // end DBMSrvKnl_DbAndSqlCommands::release

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::execute
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvKnl_DbAndSqlCommands::execute
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        bool                                    bNice,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply)
{
  tcn00_Error nFuncReturn = OK_CN00;
  bool        bTemporary  = false;
  DBMSrvKnl_Session::ResultType oResultType = DBMSrvKnl_Session::resultUnknown;
  DBMSrvKnl_Session * pSession =  NULL;
  DBMSrvMsg_Error     oSessionError;

  // DBA history begin
  const char *     pAction = "";
  bool             bDBAAction     = false;
  tsp00_Timestampc szTimeStamp;
  if (checkForDBAAction(command->args, pAction)) {
    bDBAAction = true;
    cn90Timestamp(szTimeStamp);
    tcn53_DBAAction::start(vcontrol->dbname, szTimeStamp, pAction, DBA_OBJ_DB_CN53);
    tcn53_DBAAction::writeDetail(DBADTL_CDBCMD_CN53, command->args);
  } // end if
  // DBA history end

  if (DBMSrvKnl_State::State(vcontrol->dbname.asCharp()) == DBMSrvKnl_State::StateOffline) {
    nFuncReturn = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
  } // end if

  if (nFuncReturn == OK_CN00) {
    char        szToken   [PARAMETER_MAXLEN_CN90];
    cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
    if (strncmp(szToken, USER_TYPE_CN50, strlen(USER_TYPE_CN50)) == 0) {
      DBMSrvMsg_Error oError;
      pSession = DBMSrvKnl_GlobalSessions::CreateSession(oType,
                                                        vcontrol->dbname.asCharp(),
                                                        szToken,
                                                        oSessionError);
      if (pSession == NULL) {
        nFuncReturn = theReply.startWithMessageList(oSessionError);
      } else {
        bTemporary  = true;
        command->setArgs(command->args + strlen(szToken));
      } // end if
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    pSession = DBMSrvKnl_GlobalSessions::GetSession(oType);
    if (pSession == NULL) {
      pSession = DBMSrvKnl_GlobalSessions::CreateSession(oType,
                                                        vcontrol->dbname.asCharp(),
                                                        "",
                                                        oSessionError);
      if (pSession == NULL) {
        nFuncReturn = theReply.startWithMessageList(oSessionError);
      } else {
        bTemporary  = true;
      } // end if
    } // end if
  } // end if
  
  if (nFuncReturn == OK_CN00) {
    if(oType==DBMSrvKnl_GlobalSessions::SessionDatabase)
      pSession->allowReconnectAtNextExecute(DBMSrvKnl_Session::dbExecute);
    if (!pSession->Execute(command->args, oResultType, oSessionError)) {
      nFuncReturn = theReply.startWithMessageList(oSessionError);
    } else {
      switch (oResultType) {
        case DBMSrvKnl_Session::resultNothing:
          theReply.startWithOK();
          break;
        case DBMSrvKnl_Session::resultMessage:
          {
            theReply.startWithOK();
            theReply.appendMessageList(pSession->KernelMessage());
          } // end block
          break;
        case DBMSrvKnl_Session::resultSQL:
          if (bNice) {
            nFuncReturn = fetchnice(oType, vcontrol, command, theReply);
          } else {
            nFuncReturn = fetch(oType, vcontrol, command, theReply);
          } // end if
          break;
        case DBMSrvKnl_Session::resultUtility:
          {
            DBMSrvKnl_SQLRecord & oRecord = pSession->GetRecordObject();
            tsp00_Int4 i = 0;
            theReply.startWithOK();

            for (i = 1; i <= oRecord.fields(); ++i) {
              theReply.appendStringWithMinWidth(oRecord.fieldName(i).CharPtr(), MIN_WIDTH);
              theReply.appendString(ASSIGN_TXT);
              theReply.appendLine(oRecord.fieldAsChar(i, 0).CharPtr());
            } // end if
          } // end block
          break;
        default:
          nFuncReturn = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_KNLUNKNOWN));
          break;
      } // end switch
    } // end if

    if(oType==DBMSrvKnl_GlobalSessions::SessionDatabase)
      pSession->disallowReconnectAtNextExecute();
  } // end if

  // SYSDBA password handling
  if (nFuncReturn == OK_CN00) {
    Tools_DynamicUTF8String szUser;
    Tools_DynamicUTF8String szNewPassword;
    if (checkForPasswordChange(command->args, szUser, szNewPassword)) {
      if (((szUser.Length() == 0) && pSession->isSYSDBA()) || (pSession->isSYSDBA(szUser.CharPtr())) ) {
        // create dba user
        cn50DBMUser         usrOldDBA(vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));
        cn50DBMUser         usrNewDBA(vcontrol->dbname);

        usrNewDBA.setUserName ( pSession->getSYSDBA())
                 .setSDBAUser ( )
                 .setMasterPwd( szNewPassword.CharPtr())
                 .setServerRights ( DBMMaskAll_CN50, DBMUserDefault_CN50 )
                 .setGUIRights    ( DBMMaskAll_CN50, DBMUserDefault_CN50 );

        if (usrOldDBA.existsOnDisk()) {
          usrNewDBA.setServerRights ( DBMMaskAll_CN50, usrOldDBA.getServerRights() )
                   .setGUIRights    ( DBMMaskAll_CN50, usrOldDBA.getGUIRights() );
        } // end if

        // delete old dba user
        usrOldDBA.setSDBAUser (false);
        usrOldDBA.deleteUsr();

        // save new dba user
        usrNewDBA.save();

      } // end if
    } // end if
  } // end if

  if (bTemporary) {
    DBMSrvKnl_GlobalSessions::ReleaseSession(oType, oSessionError);
  } // end if

  // DBA history begin
  if (bDBAAction) {
    tcn53_DBAAction::writeDetail(DBADTL_CDBRESULT_CN53, theReply.giveData());
    tcn53_DBAAction::finish(cn90Timestamp(szTimeStamp), (nFuncReturn == OK_CN00));
  } // end if
  // DBA history end

  return nFuncReturn;
} // end DBMSrvKnl_DbAndSqlCommands::execute

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::fetch
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvKnl_DbAndSqlCommands::fetch
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply)
{
  if (DBMSrvKnl_State::State(vcontrol->dbname.asCharp()) == DBMSrvKnl_State::StateOffline) {
    return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
  } // end if

  tcn00_Error nFuncReturn = OK_CN00;

  DBMSrvKnl_Session * pSession =   DBMSrvKnl_GlobalSessions::GetSession(oType);
  DBMSrvMsg_Error     oSessionError;

  if (pSession == NULL) {
    nFuncReturn = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION));
  } // end if
  else
    if(oType==DBMSrvKnl_GlobalSessions::SessionDatabase)
    {
      DBMSrvMsg_Info info;

      if(pSession->hasInterruptedSubsession(DBMSrvKnl_Session::dbExecute, info))
      {
          nFuncReturn=theReply.startWithMessageList(
                                    DBMSrvMsg_Error(
                                        info,
                                        SDBMSG_DBMSRV_RESULTLOST,
                                        Msg_Arg(SDBMSGTAG_DBMSRV_RESULTLOST__COMMAND_NAME, "db_execute")));
      }
    }

  if (nFuncReturn == OK_CN00) {
    DBMSrvKnl_SQLRecord & oRecord   = pSession->GetRecordObject();
    bool               bEnd      = false;
    bool               bContinue = false;

    theReply.startWithOK();
    DBMSrv_Reply::Position posContinue      = theReply.giveCurrentEnd();
    theReply.appendLine(CONTINUE_TXT);

    DBMSrv_Reply::Position posStartOfRecord = theReply.giveCurrentEnd();
    Tools_DynamicUTF8String sField;

    tsp00_Int4 i = 0;

    do {
      if (bEnd || !oSessionError.IsEmpty()) {
        break; 
      } // end if
      
      posStartOfRecord  = theReply.giveCurrentEnd();

      // read record
      for (i = 1; i <= oRecord.fields(); ++i) {
        sField = oRecord.fieldAsChar(i, STRINGMARK_CHAR);
        if (theReply.giveAvailableSpace() >  (int)(sField.Length() + 1)) {
          theReply.appendString(sField.CharPtr());
          theReply.appendChar(FIELDSEP_CHAR);
        } else {
          bContinue = true;
          break;
        } // end if
      } // end for

      if (bContinue) {
        theReply.skipAt(posStartOfRecord);
        break;
      } else {
        theReply.skipAt(theReply.giveCurrentEnd()-1);
        theReply.appendLine();
      } // end if

    } while (oRecord.getNextRecord(bEnd, oSessionError));

    if (!oSessionError.IsEmpty()) {
      theReply.startWithMessageList(oSessionError);
    } else {
      if (bEnd) {
        theReply.insertString(posContinue, END_TXT);
      } else {
        // prepare next command
        if (oType == DBMSrvKnl_GlobalSessions::SessionSQL) {
          vcontrol->szNextCommand.rawAssign("sql_fetch");
          vcontrol->nNextCommandSkip = 1;
        } else if (oType == DBMSrvKnl_GlobalSessions::SessionDatabase) {
          vcontrol->szNextCommand.rawAssign("db_fetch");
          vcontrol->nNextCommandSkip = 1;
        } // end if
      } // end if
    } // end if

  } // end if

  return nFuncReturn;
} // end DBMSrvKnl_DbAndSqlCommands::fetch

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::fetchnice
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvKnl_DbAndSqlCommands::fetchnice
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply)
{
  if (DBMSrvKnl_State::State(vcontrol->dbname.asCharp()) == DBMSrvKnl_State::StateOffline) {
    return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
  } // end if

  tcn00_Error nFuncReturn = OK_CN00;

  DBMSrvKnl_Session * pSession =   DBMSrvKnl_GlobalSessions::GetSession(oType);

  if (pSession == NULL) {
    nFuncReturn = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION));
  }
  else
    if(oType==DBMSrvKnl_GlobalSessions::SessionDatabase)
    {
      DBMSrvMsg_Info info;

      if(pSession->hasInterruptedSubsession(DBMSrvKnl_Session::dbExecute, info))
      {
          nFuncReturn=theReply.startWithMessageList(
                                    DBMSrvMsg_Error(
                                        info,
                                        SDBMSG_DBMSRV_RESULTLOST,
                                        Msg_Arg(SDBMSGTAG_DBMSRV_RESULTLOST__COMMAND_NAME, "db_execute")));
      }
    }

  if (nFuncReturn == OK_CN00) {

    DBMSrvKnl_SQLRecord & oRecord = pSession->GetRecordObject();
    bool               bEnd    = false;

    theReply.startWithOK();
    DBMSrv_Reply::Position posContinue      = theReply.giveCurrentEnd();
    theReply.appendLine(CONTINUE_TXT);

    tsp00_Int4 i = 0;

    for (i = 1; i <= oRecord.fields(); ++i) {
      theReply.appendStringWithMinWidth(oRecord.fieldName(i).CharPtr(), MIN_WIDTH);
      theReply.appendString(ASSIGN_TXT);
      theReply.appendLine(oRecord.fieldAsChar(i, STRINGMARK_CHAR).CharPtr());
    } // end if

    DBMSrvMsg_Error oSessionError;
    oRecord.getNextRecord(bEnd, oSessionError);

    if (!oSessionError.IsEmpty()) {
      theReply.startWithMessageList(oSessionError);
    } else {
      if (bEnd) {
        theReply.insertString(posContinue, EMPTY_TXT);
      } else {
        // prepare next command
        if (oType == DBMSrvKnl_GlobalSessions::SessionSQL) {
          vcontrol->szNextCommand.rawAssign("sql_fetchnice");
          vcontrol->nNextCommandSkip = 1;
        } else if (oType == DBMSrvKnl_GlobalSessions::SessionDatabase) {
          vcontrol->szNextCommand.rawAssign("db_fetchnice");
          vcontrol->nNextCommandSkip = 1;
        } // end if
      } // end if
    } // end if

  } // end if

  return nFuncReturn;
} // end DBMSrvKnl_DbAndSqlCommands::fetchnice

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::info
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvKnl_DbAndSqlCommands::info
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply)
{
  if (DBMSrvKnl_State::State(vcontrol->dbname.asCharp()) == DBMSrvKnl_State::StateOffline) {
    return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
  } // end if

  tcn00_Error nFuncReturn = OK_CN00;
  bool        bTemporary  = false;
  DBMSrvKnl_Session::ResultType oResultType = DBMSrvKnl_Session::resultUnknown;
  DBMSrvKnl_Session * pSession =  NULL;
  DBMSrvMsg_Error     oSessionError;

  char        szToken   [PARAMETER_MAXLEN_CN90];
  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  if (strncmp(szToken, USER_TYPE_CN50, strlen(USER_TYPE_CN50)) == 0) {
    DBMSrvMsg_Error oError;
    pSession = DBMSrvKnl_GlobalSessions::CreateSession(oType,
                                                       vcontrol->dbname.asCharp(),
                                                       command->args,
                                                       oSessionError);
    if (pSession == NULL) {
      nFuncReturn = theReply.startWithMessageList(oSessionError);
    } else {
      bTemporary  = true;
      command->setArgs(command->args + strlen(szToken));
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    pSession = DBMSrvKnl_GlobalSessions::GetSession(oType);
    if (pSession == NULL) {
      pSession = DBMSrvKnl_GlobalSessions::CreateSession(oType,
                                                        vcontrol->dbname.asCharp(),
                                                        "",
                                                        oSessionError);
      if (pSession == NULL) {
        nFuncReturn = theReply.startWithMessageList(oSessionError);
      } else {
        bTemporary  = true;
      } // end if
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {

    cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
    if (stricmp(szToken, "SELECT") == 0) {

      if (!pSession->Execute(command->args, oResultType, oSessionError)) {
        nFuncReturn = theReply.startWithMessageList(oSessionError);
      } else {
        switch (oResultType) {
          case DBMSrvKnl_Session::resultSQL:
            {
              theReply.startWithOK();

              DBMSrvKnl_SQLRecord & oRecord = pSession->GetRecordObject();
              tsp00_Int4 i = 0;
              for (i = 1; i <= oRecord.fields(); ++i) {
                theReply.appendString(oRecord.fieldName(i).CharPtr());
                theReply.appendChar(FIELDSEP_CHAR);
              } // end if
              theReply.skipAt(theReply.giveCurrentEnd() - 1);
              theReply.appendLine();
            } // end block
            break;
          default:
            nFuncReturn = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_KNLUNKNOWN));
            break;
        } // end switch
      } // end if
    } else {
      nFuncReturn = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, Msg_Arg("DBMSRV_CMDOPTION", szToken), Msg_Arg("DBMSRV_CMDNAME","Info")));
    } // end if

  } // end if

  if (bTemporary) {
    DBMSrvKnl_GlobalSessions::ReleaseSession(oType, oSessionError);
  } // end if

  return nFuncReturn;
} // end DBMSrvKnl_DbAndSqlCommands::info

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::checkForDBAAction
  -----------------------------------------------------------------------------
 */
bool DBMSrvKnl_DbAndSqlCommands::checkForDBAAction 
      ( const char *   szStatement, 
        const char * & pAction ) 
{
  bool      bDBAAction = false;
  char      szToken    [PARAMETER_MAXLEN_CN90];

  if (cn90GetToken(szStatement, szToken, 1, PARAMETER_MAXLEN_CN90)) {
    if (stricmp(szToken, "VERIFY") == 0) {
      pAction = DBAOP_CHECKDB_CN53;
      bDBAAction = true;
    } else if (stricmp(szToken, "CHECK") == 0) {
      if (cn90GetToken(szStatement, szToken, 2, PARAMETER_MAXLEN_CN90)) {
        if (stricmp(szToken, "DATA") == 0) {
          pAction = DBAOP_CHECKDB_CN53;
          bDBAAction = true;
          if (cn90GetToken(szStatement, szToken, 3, PARAMETER_MAXLEN_CN90)) {
            if (stricmp(szToken, "EXCEPT") == 0) {
              pAction = DBAOP_CHECKDBE_CN53;
              bDBAAction = true;
            } else if (stricmp(szToken, "EXTENDED") == 0) {
              if (cn90GetToken(szStatement, szToken, 4, PARAMETER_MAXLEN_CN90)) {
                if (stricmp(szToken, "EXCEPT") == 0) {
                  pAction = DBAOP_CHECKDBE_CN53;
                  bDBAAction = true;
                } // end if
              } // end if
            } // end if
          } // end if
        } else if (stricmp(szToken, "TABLE") == 0) {
          pAction = DBAOP_TABLECHECK_CN53;
          bDBAAction = true;
        } // end if
      } // end if
    } // end if
  } // end if

  return bDBAAction;
} // end DBMSrvKnl_DbAndSqlCommands::checkForDBAAction

/*
  -----------------------------------------------------------------------------
  static member DBMSrvKnl_DbAndSqlCommands::checkForPasswordChange
  -----------------------------------------------------------------------------
 */
bool DBMSrvKnl_DbAndSqlCommands::checkForPasswordChange 
      ( const char              * szStatement,
        Tools_DynamicUTF8String & szUser,
        Tools_DynamicUTF8String & szNewPassword )
{
  char szToken    [PARAMETER_MAXLEN_CN90];
  bool bReturn = false;

  if (cn90GetToken(szStatement, szToken, 1, PARAMETER_MAXLEN_CN90)) {
    if (stricmp(szToken, "ALTER") == 0) {
      if (cn90GetToken(szStatement, szToken, 2, PARAMETER_MAXLEN_CN90)) {
        if (stricmp(szToken, "PASSWORD") == 0) {
          if (cn90GetToken(szStatement, szToken, 3, PARAMETER_MAXLEN_CN90)) {
            szUser = szToken;
            if (cn90GetToken(szStatement, szToken, 4, PARAMETER_MAXLEN_CN90)) {
              if (stricmp(szToken, "TO") == 0) {
                szUser.Erase();
                if (cn90GetToken(szStatement, szToken, 5, PARAMETER_MAXLEN_CN90)) {
                  szNewPassword = szToken;
                  bReturn = true;
                } // end if
              } else {
                szNewPassword = szToken;
                bReturn = true;
              } // end if
            } // end if
          } // end if
        } // end if
      } // end if
    } // end if
  } // end if

  return bReturn;
} // end DBMSrvKnl_DbAndSqlCommands::checkForPasswordChange
