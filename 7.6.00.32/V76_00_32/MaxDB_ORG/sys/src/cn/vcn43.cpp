/*
  -----------------------------------------------------------------------------

  module: vcn43.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer Dbanalyzer Access

  description:  DBMServer Dbanalyzer Access - Implementation

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
#include "heo01.h"
#include "hcn43.h"
#include "hcn40.h"
#include "hcn50.h"
#include "hcn90.h"

#include "ToolsCommon/Tools_System.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"

#ifdef _WIN32
  #define DBA_STATE  "\"%sdbanalyzer\" -d %s -u %s,%s -state"
  #define DBA_STOP   "\"%sdbanalyzer\" -d %s -u %s,%s -stop"
  #define DBA_DELETE "\"%sdbanalyzer\" -d %s -u %s,%s -delete %s"
  #define DBA_START  "\"%sdbanalyzer\" -nologo -d %s -u %s,%s %s"
#else
  #define DBA_STATE  "%sdbanalyzer -d %s -u %s,%s -state"
  #define DBA_STOP   "%sdbanalyzer -d %s -u %s,%s -stop"
  #define DBA_DELETE "%sdbanalyzer -d %s -u %s,%s -delete %s"
  #define DBA_START  "%sdbanalyzer -nologo -d %s -u %s,%s %s"
#endif

/*
  -----------------------------------------------------------------------------
  public function cn43DBanalyzerStart
  -----------------------------------------------------------------------------
*/
tcn00_Error cn43DBanalyzerStart
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  char           szCommand [PARAMETER_MAXLEN_CN90];
  char           szDummy   [PARAMETER_MAXLEN_CN90];
  int            nCounter = 0;
  tsp00_Pathc    szBinDir;
  tsp01_RteError aRteError;

  if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn50DBMUser    oUser (vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));

    if (!oUser.lastEvent().IsEmpty()) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAUNKNOWN_CN00);
    } // end if

    if (nFuncReturn == OK_CN00) {
      if(!sqlGetIndependentBinPath(szBinDir, TERM_WITH_DELIMITER_EO01, &aRteError)) {
        szBinDir.Init();
      } // end if
      sprintf(szCommand, DBA_STATE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), oUser.getUserName().asCharp(), oUser.getClearMasterPwd().asCharp());
      sprintf(szDummy,   DBA_STATE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), "*",  "*");
      nFuncReturn =  cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szDummy, true, true);
      if (nFuncReturn == OK_CN00 && (strstr(replyData, "INFO 20:") != NULL)) {

        DBMSrvProc_ArgumentList theArgList;
        char upwdBuffer[256];

        // build command line
        theArgList.AddArgument("-nologo");
        theArgList.AddArgument("-d");
        theArgList.AddArgument(vcontrol->dbname.asCharp());
        theArgList.AddArgument("-u");
        sprintf(upwdBuffer, "%s,%s", oUser.getUserName().asCharp(), oUser.getClearMasterPwd().asCharp());
        theArgList.AddArgument(upwdBuffer);

        char  szToken[PARAMETER_MAXLEN_CN90];
        int   nToken = 1;
        while (cn90GetToken(command->args, szToken, nToken, PARAMETER_MAXLEN_CN90)) {
          theArgList.AddArgument(szToken);
          ++nToken;
        } // end while

        // svae timestamp for error analysis
        tsp00_Timestampc szTimestamp;
        cn90Timestamp( szTimestamp );

        DBMSrvProc_ServerProcess oProcess( DBMSrvProc_ServerProcessCommandLineConverter::KeyDbanalyzer,
                                           theArgList );

        if(!oProcess.started() ) {
          Msg_List aMsgList;
          oProcess.getMessagesInto( aMsgList );
          aMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EXECUTE));
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aMsgList);
        } // end if

        if (nFuncReturn == OK_CN00) {
          char szBufferState[5000];

          sprintf(szCommand, DBA_STATE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), oUser.getUserName().asCharp(), oUser.getClearMasterPwd().asCharp());
          sprintf(szDummy,   DBA_STATE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), "*",  "*");
          ToolsSys_sleep(2);
          nFuncReturn =  cn40ExecCommand(szBufferState, replyLen, 5000, szCommand, szDummy, true, true);
          while ((nFuncReturn == OK_CN00) && (strstr(replyData, "INFO 20:") != NULL) && (nCounter < 3)) {
            ++nCounter;
            ToolsSys_sleep(2);
            nFuncReturn =  cn40ExecCommand(szBufferState, replyLen, 5000, szCommand, szDummy, true, true);
          } // end while
          if ((nFuncReturn == OK_CN00) && (strstr(szBufferState, "INFO 20:") != NULL)) {
            // dbanalyzer does not run 
            // get error protokoll
            const char * pContent = &szBufferState[0];
            const char * pTmp    = strchr(pContent, '\n');

            if (pTmp != NULL) {
              pContent = pTmp + 1;
              pTmp = strchr(pContent, '\n');
              if (pTmp != NULL) {
                pContent = pTmp + 1;
              } // end if
            } // end if

            DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
            nFuncReturn = oReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_EXECUTE));
            oReply.appendString(pContent);

          } else {
            strcpy(replyData, szBufferState);
          } // end if
        } // end if
      } // end if
    } // end if
  } // end if

  return nFuncReturn;
} // end cn43DBanalyzerStart

/*
  -----------------------------------------------------------------------------
  public function cn43DBanalyzerStop
  -----------------------------------------------------------------------------
*/
tcn00_Error cn43DBanalyzerStop
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  char           szCommand [PARAMETER_MAXLEN_CN90];
  char           szDummy   [PARAMETER_MAXLEN_CN90];
  tsp00_Pathc    szBinDir;
  tsp01_RteError aRteError;

  if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn50DBMUser    oUser (vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));

    if (!oUser.lastEvent().IsEmpty()) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAUNKNOWN_CN00);
    } // end if

    if (nFuncReturn == OK_CN00) {
      if(!sqlGetIndependentBinPath(szBinDir, TERM_WITH_DELIMITER_EO01, &aRteError)) {
        szBinDir.Init();
      } // end if
      sprintf(szCommand, DBA_STOP, szBinDir.asCharp(), vcontrol->dbname.asCharp(), oUser.getUserName().asCharp(), oUser.getClearMasterPwd().asCharp());
      sprintf(szDummy,   DBA_STOP, szBinDir.asCharp(), vcontrol->dbname.asCharp(), "*",  "*");
      nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szDummy, true, true);
    } // end if
  } // end if

  return nFuncReturn;
} // end cn43DBanalyzerStop

/*
  -----------------------------------------------------------------------------
  public function cn43DBanalyzerState
  -----------------------------------------------------------------------------
*/
tcn00_Error cn43DBanalyzerState
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  char           szCommand [PARAMETER_MAXLEN_CN90];
  char           szDummy   [PARAMETER_MAXLEN_CN90];
  tsp00_Pathc    szBinDir;
  tsp01_RteError aRteError;

  if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
  cn50DBMUser    oUser (vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));

    if (!oUser.lastEvent().IsEmpty()) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAUNKNOWN_CN00);
    } // end if

    if (nFuncReturn == OK_CN00) {
      if(!sqlGetIndependentBinPath(szBinDir, TERM_WITH_DELIMITER_EO01, &aRteError)) {
        szBinDir.Init();
      } // end if
      sprintf(szCommand, DBA_STATE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), oUser.getUserName().asCharp(), oUser.getClearMasterPwd().asCharp());
      sprintf(szDummy,   DBA_STATE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), "*",  "*");
      nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szDummy, true, true);
    } // end if
  } // end if

  return nFuncReturn;
} // end cn43DBanalyzerState

/*
  -----------------------------------------------------------------------------
  public function cn43DBanalyzerDelete
  -----------------------------------------------------------------------------
*/
tcn00_Error cn43DBanalyzerDelete
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  char           szCommand [PARAMETER_MAXLEN_CN90];
  char           szDummy   [PARAMETER_MAXLEN_CN90];
  tsp00_Pathc    szBinDir;
  tsp01_RteError aRteError;

  if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn50DBMUser    oUser (vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));

    if (!oUser.lastEvent().IsEmpty()) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAUNKNOWN_CN00);
    } // end if

    if (nFuncReturn == OK_CN00) {
      if(!sqlGetIndependentBinPath(szBinDir, TERM_WITH_DELIMITER_EO01, &aRteError)) {
        szBinDir.Init();
      } // end if
      sprintf(szCommand, DBA_DELETE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), oUser.getUserName().asCharp(), oUser.getClearMasterPwd().asCharp(), command->args);
      sprintf(szDummy,   DBA_DELETE, szBinDir.asCharp(), vcontrol->dbname.asCharp(), "*",  "*", command->args);
      nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szDummy, true, true);
    } // end if
  } // end if

  return nFuncReturn;
} // end cn43DBanalyzerDelete
