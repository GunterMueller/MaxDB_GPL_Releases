/*
  -----------------------------------------------------------------------------

  module: vcn40.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer

  description:  DBMServer start-stop / system / registry access

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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/stat.h>

#include "vsp001.h"
#include "hsp77.h"
#include "gsp09.h"
#include "geo43.h"

#include "gip00.h"
#include "heo02.h"
#include "heo03.h"
#include "heo06.h"
#include "heo11.h"
#include "hsp100.h"

#ifndef _WIN32
  #include "RunTime/RTE_UNIXAuthenticate.hpp"           // nocheck
#endif

#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"

#include "ToolsCommon/Tools_System.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"

#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbDrop.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp"
#include "SAPDB/DBM/Srv/Process/DBMSrvProc_EnvironmentVariable.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_Volumes.hpp"
#include "SAPDB/DBM/Srv/Process/DBMSrvProc_EnvironmentVariable.hpp"

#include "DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp"

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

#include "hcn20.h"
#include "hcn31.h"
#include "hcn36.h"
#include "hcn42.h"
#include "hcn44.h"
#include "hcn46.h"
#include "hcn50.h"
#include "hcn51.h"
#include "hcn90.h"

#include "hcn40.h"

#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_UserProfileContainer.hpp"

extern "C" int chdir(const char * szDir);

/*
  -----------------------------------------------------------------------------
  specification private macros
  -----------------------------------------------------------------------------
*/
#define SYS_SYSTEM_ERR        -1
#define SYS_MKDIR_ERR         -1
#define SYS_MKDIR_OK          0
#define CHAR_SLASH            '/'
#define CHAR_BACKSLASH        '\\'
#define CHAR_DRIVE            ':'
#define TMP_ENV               "TMP"
#define TMP_LOCDIR            "."
#define TMP_EXT               "cst"
#define REPMAN_PRT            "loader.log"
#define UTIL_PRT              "util.prt"

#define OPT_OPT_AUTO          "-auto"
#define OPTS_OPT_AUTO         "-a"

#define OPT_OPT_GROUP         "-group"
#define OPTS_OPT_GROUP        "-g"

#define OPT_OPT_USER          "-user"
#define OPTS_OPT_USER         "-u"

#define OPT_VAL_ON            "on"
#define OPT_VAL_OFF           "off"

#define OPT_SPEED_FAST        "-fast"
#define OPTS_SPEED_FAST       "-f"

#define OPT_SPEED_SLOW        "-slow"
#define OPTS_SPEED_SLOW       "-s"

#define OPT_SPEED_TEST        "-test"
#define OPTS_SPEED_TEST       "-t"

#define OPT_REGIST_ROOT       "-Root"
#define OPTS_REGIST_ROOT      "-R"

#define OPTS_STOP_DUMP        "-d"
#define OPT_STOP_DUMP         "-dump"

#define OPTS_STOP_NOCLUSTER   "-nc"
#define OPT_STOP_NOCLUSTER    "-nocluster"

#define OPT_RESTART_UNTIL     "-until"
#define OPTS_RESTART_UNTIL    "-u"

#define TXT_TAKEOVER          "TAKEOVER"
#define TXT_RESTART           "RESTART"
#define TXT_RESTART_UNTIL     "RESTART UNTIL '%s' '%s'"

#define OPT_LOAD_DBA          "-u"
#define OPT_LOAD_DOM          "-ud"
#define OPT_LOAD_MIGRATE      "-m"
#define LOAD_FILE             "lsystab.ins"
#define PYTHON_FILE           "lsystab.py"
#define LOAD_OPT              "en ENG"
#define R3_FILE               "CNR3DATA.ins"
#define PYR3_FILE             "CNR3DATA.py"
#define LCAPPS_FILE           "lapps.py"
#define TUTORIAL_FILE         "../demo/ltutorial.py"

#define LOAD_INIT             "INIT AFTER LOAD"
#define DB_MIGRATE_VOLUME     "MIGRATE VOLUME"
#define DB_MIGRATE_DATABASE   "MIGRATE DATABASE"
#define DB_SHUTDOWN           "SHUTDOWN"

#define TXT_CMD_SHOW          "SHOW"
#ifdef _WIN32
  #define TXT_CMD_XCONS         _T("\"pgm\\cons.exe\"")
#else
  #define TXT_CMD_XCONS         _T("pgm/cons")
#endif
#define TXT_CMD_HELP          "help"

#define TXTL_SPEED_FAST       "fast"
#define TXTL_SPEED_QUICK      "quick"
#define TXTL_SPEED_SLOW       "slow"
#define TXTL_SPEED_TEST       "test"
#define TXTL_SPEED_UNKNOWN    "unknown"

#define TXT_CMD_LCINITSAP     "sap%clcinit %s %s -uDBM %s,%s -uDBA %s,%s -uSQL %s,%s"
#define TXT_CMD_LCINIT        "sap%clcinit %s %s -uDBM %s,%s -uDBA %s,%s"
#define TXT_CMD_XPU           "xpu %s"
#ifdef _WIN32
  #define TXT_CMD_NIPINGTEST  "sap\\niping -t"
  #define TXT_PRT_NIPINGTEST  "sap\\niping -t"
  #define TXT_CMD_NIPING      "start /B sap\\niping -s -I %d"
  #define TXT_PRT_NIPING      "sap\\niping -s -I %d"
#else
  #define TXT_CMD_NIPINGTEST  "sap/niping -t"
  #define TXT_PRT_NIPINGTEST  "sap/niping -t"
  #define TXT_CMD_NIPING      "sap/niping -s -I %d &"
  #define TXT_PRT_NIPING      "sap/niping -s -I %d"
#endif
#define TXT_CMD_SDBINFO       "sdbinfo"
#define TXT_CMD_SDBINST       "SDBINST"

#define KEYDROP_WITHOUTFILES  "WITHOUTFILES"

#define RTE_INDPROG           "IndepProgPath"
#define RTE_INDDATA           "IndepDataPath"

#define OPTS_INST_VERSION     "-v"
#define OPTS_INST_KEY         "-k"
#define OPTS_INST_CLIENT      "-c"
#define OPTS_ENUM_SRV         "-s"

#define TEST_OPT_WAIT         "-w"

#define SQL_CHECKSYSDBA           "SELECT * FROM SYSDD.VERSION WHERE SYSDBA=USER"

#define SQL_DBANLOCK_CHECK        "EXISTS TABLE SYSDBANA.DBANLOCK"
#define SQL_DBANLOCK_DROPUSER     "DROP USER SYSDBANA"
#define SQL_DBANLOCK_CREATESCHEMA "CREATE SCHEMA SYSDBANA"
#define SQL_DBANLOCK_CREATETABLE  "CREATE TABLE SYSDBANA.DBANLOCK (FIELD1 INT)"
#define SQL_DBANLOCK_CREATEGRANT  "GRANT SELECT,INSERT ON SYSDBANA.DBANLOCK TO PUBLIC"
#define SQL_DBANLOCK_LOCK         "LOCK (WAIT) TABLE SYSDBANA.DBANLOCK IN EXCLUSIVE MODE"

#define TXT_SQLOPT            "SQLOPT=-d %s -u %s,%s"

#define MAX_DBNAME            8

#ifdef _WIN32
  #define TXT_CMD_DBMEXT        "bin\\dbmevtdisp -d %s -u %s,%s"
#else
  #define TXT_CMD_DBMEXT        "bin/dbmevtdisp -d %s -u %s,%s"
#endif
/*
  -----------------------------------------------------------------------------
  specification private functions
  -----------------------------------------------------------------------------
*/

static tcn00_Error cn40_ExecPythonLoad
      ( VControlDataT * vcontrol,
        const char    * szLoadFile,
        const char    * szUser,
        const char    * szPassword,
        const char    * szOptions,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

static tcn00_Error cn40_OfflineToCold
      ( VControlDataT       * vcontrol,
        const tsp9_pgm_kind   pgmKind,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax);

static tcn00_Error cn40_ColdOrStandbyToWarm
      ( VControlDataT       * vcontrol,
        const char          * pRestart,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax);

static tcn00_Error cn40_OnlineToOffline
      ( VControlDataT       * vcontrol,
        bool                  bCluster,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax);

static tcn00_Error cn40_ColdOrStandbyToOffline
      ( VControlDataT       * vcontrol,
        const bool            bDump,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax);

static tcn00_Error cn40_CheckVersion
      ( const tsp00_DbNamec   szDBName);

static tcn00_Error cn40_StartStopOptions
      ( const char    * szParameters,
        tsp9_pgm_kind & pgmKind,
        bool          & bDump,
        char          * pRestart,
        bool          & bCluster );

static tcn00_Error cn40_ChangeDBState
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax,
        DBMSrvKnl_State::StateValue targetState,
        bool            bStop = false);

static bool cn40_CheckInstKey
      ( const tsp09_RteInstallationInfoNew  & relInfoIn );

static bool cn40_RegisterServiceDB
      ( const tsp09_RteInstallationInfoNew  & relInfoIn); 

/*
  -----------------------------------------------------------------------------
  implementation of public functions
  -----------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBMNext
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBMNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40AutoNext");

  if (vcontrol->szNextCommand.length() > 0) {
    // print answer
    sp77sprintf(replyData, replyLenMax, "%s%s%s%s%d%s",
                ANSWER_OK_CN00,
                LINE_SEPSTRING_CN00,
                vcontrol->szNextCommand.asCharp(),
                LINE_SEPSTRING_CN00,
                vcontrol->nNextCommandSkip,
                LINE_SEPSTRING_CN00);
    *replyLen = (int) strlen(replyData);
    // clear next command
    vcontrol->szNextCommand.rawAssign("");
    vcontrol->nNextCommandSkip = 0;
  } else {
    // print only a OK (no error!!!)
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  return OK_CN00;
} /* end cn40DBMNext */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40ExecLcInit
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ExecLcInit
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40ExecLcInit");

  tcn00_Error     nFuncReturn = OK_CN00;
  char            szCommand      [PARAMETER_MAXLEN_CN90] = "";
  char            szPrCommand    [PARAMETER_MAXLEN_CN90] = "";
  bool            bSAPExists  = true;

  // collect the users
  cn50DBMUser oSAPUser   (vcontrol->dbname, cn50DBMUser::getSAPUser (vcontrol->dbname));
  if (!oSAPUser.lastEvent().IsEmpty()) {
    bSAPExists  = false;
  } // end if
  
  cn50DBMUser oDBMUser   (vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
  if (!oDBMUser.lastEvent().IsEmpty()) {
    Msg_List aEvent = oDBMUser.lastEvent();
    aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_USRREAD));
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
    return nFuncReturn;
  } // end if
  
  cn50DBMUser oDBAUser   (vcontrol->dbname, cn50DBMUser::getSYSDBA  (vcontrol->dbname));
  if (!oDBAUser.lastEvent().IsEmpty()) {
    Msg_List aEvent = oDBMUser.lastEvent();
    aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAUNKNOWN));
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
    return nFuncReturn;
  } // end if

  if (bSAPExists) {
    //#define TXT_CMD_LCINITSAP     "sap%clcinit %s %s -uDBM %s,%s -uDBA %s,%s -uSQL %s,%s -ud %s"
    sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, TXT_CMD_LCINITSAP, PATH_DELIMITER_CN90, 
                                                                     vcontrol->dbname.asCharp(), 
                                                                     command->args,
                                                                     oDBMUser.getUserName().asCharp(),
                                                                     oDBMUser.getClearMasterPwd().asCharp(),
                                                                     oDBAUser.getUserName().asCharp(),
                                                                     oDBAUser.getClearMasterPwd().asCharp(),
                                                                     oSAPUser.getUserName().asCharp(),
                                                                     oSAPUser.getClearMasterPwd().asCharp()
                                                                     );

    sp77sprintf(szPrCommand, PARAMETER_MAXLEN_CN90, TXT_CMD_LCINITSAP, PATH_DELIMITER_CN90, 
                                                                     vcontrol->dbname.asCharp(), 
                                                                     command->args,
                                                                     "*",
                                                                     "*",
                                                                     "*",
                                                                     "*",
                                                                     "*",
                                                                     "*"
                                                                     );
  } else {
    //#define TXT_CMD_LCINIT     "sap%clcinit %s %s -uDBM %s,%s -uDBA %s,%s -ud %s"
    sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, TXT_CMD_LCINIT, PATH_DELIMITER_CN90, 
                                                                  vcontrol->dbname.asCharp(), 
                                                                  command->args,
                                                                  oDBMUser.getUserName().asCharp(),
                                                                  oDBMUser.getClearMasterPwd().asCharp(),
                                                                  oDBAUser.getUserName().asCharp(),
                                                                  oDBAUser.getClearMasterPwd().asCharp()
                                                                  );

    sp77sprintf(szPrCommand, PARAMETER_MAXLEN_CN90, TXT_CMD_LCINIT, PATH_DELIMITER_CN90, 
                                                                  vcontrol->dbname.asCharp(), 
                                                                  command->args,
                                                                  "*",
                                                                  "*",
                                                                  "*",
                                                                  "*"
                                                                  );
  } // end if

  nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szPrCommand, false, true);

  if (nFuncReturn == OK_CN00) {
    // check the lcinit output
    char * pTemp  = new char[*replyLen + 10];
    char * pData  = NULL;

    if (pTemp != NULL) {
      SAPDB_strcpy(pTemp, replyData);

      // find Nutzlast
      pData = strchr (pTemp, '\n');
      if (pData != NULL) {
        ++pData;
        pData = strchr (pData, '\n');
      } // end if
      if (pData != NULL) {
        ++pData;
        if (strstr(pData, "\nERROR") != NULL) {
          // fake answer
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_EXECUTE_CN00);
          pTemp = replyData + strlen(replyData);
          SAPDB_strcpy(pTemp, pData);
          *replyLen = (int) strlen(replyData);
        } // end if

      } // end if

      delete [] pTemp;
    } // end if

  } // end if

  return nFuncReturn;

} /* end cn40ExecLcInit */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40ExecLcInit1
 * ------------------------------------------------------------------
 */
#define LCINIT_MOD_INIT             "INIT"
#define LCINIT_MOD_REGISTER         "REGISTER"
#define LCINIT_MOD_RESTART          "RESTART"
#define LCINIT_MOD_SLOW             "SLOW"
#define LCINIT_MOD_STOP             "STOP"
#define LCINIT_MOD_SHUTDOWN         "SHUTDOWN"

#define LCINIT_OPT_ENCODING         "-e"
#define LCINIT_OPT_DEBUG            "debug"

tcn00_Error cn40ExecLcInit1
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40ExecLcInit1");

  tcn00_Error     nFuncReturn = OK_CN00;
  int             nToken      = 1;
  char            szCommand[PARAMETER_MAXLEN_CN90];
  char            szParameter[PARAMETER_MAXLEN_CN90];


  // read command
  if (!cn90GetToken(command->args, szCommand, nToken, PARAMETER_MAXLEN_CN90)) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
  } // end if

  ++nToken;
  
  while ((nFuncReturn == OK_CN00) && cn90GetToken(command->args, szParameter, nToken, PARAMETER_MAXLEN_CN90)) {
    if (stricmp(szParameter,LCINIT_OPT_DEBUG) == 0) {
    } // end if

  } // end if


  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if
  

  return nFuncReturn;

} /* end cn40ExecLcInit1 */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40ExecSDBInfo
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ExecSDBInfo
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error     nFuncReturn = OK_CN00;
  char            szCommand [PARAMETER_MAXLEN_CN90];

  /* read the parameters */
  if (nFuncReturn == OK_CN00) {
    sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90-1, "%s %s", TXT_CMD_SDBINFO, command->args);
    nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szCommand, false, true);
  } /* end if */

  return nFuncReturn;
} /* end cn40ExecSDBInfo */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40ExecSDBInst
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ExecSDBInst
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error     nFuncReturn = OK_CN00;

  char            szDirectory[PARAMETER_MAXLEN_CN90];
  char         *  pCurrent;
  /**/

  // read parameters 
  cn90GetToken(command->args, szDirectory, 1, PARAMETER_MAXLEN_CN90);

  // init OK
  cn90AnswerOK (replyData, replyLen, NULL);
  pCurrent = replyData + strlen(replyData);

  // readpacket info
  if (!cn44_PackageInfo(szDirectory, pCurrent, replyLenMax - (int)(pCurrent - replyData))) {
    char * pError = new char[strlen(pCurrent) + 1];
    if (pError != NULL) {
      SAPDB_strcpy(pError, pCurrent);
      DBMSrvMsg_Error sdbinstError(
          SDBMSG_DBMSRV_SDBINST,
          Msg_Arg(SDBMSGTAG_DBMSRV_SDBINST__ERRORTEXT, pError));
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(sdbinstError);
      delete[] pError;
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
    } // end if
  } // end if

  *replyLen = (int) strlen(replyData);

  return nFuncReturn;
} /* end cn40ExecSDBInst */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40ExecNiPingSrv
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ExecNiPingSrv
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error     nFuncReturn = OK_CN00;
  char            szIdleTime  [PARAMETER_MAXLEN_CN90] = "";
  int             nIdleTime   = 300;
  char            szCommand [PARAMETER_MAXLEN_CN90];
  char            szProtocol [PARAMETER_MAXLEN_CN90];

  if (cn90GetToken(command->args, szIdleTime,  1, PARAMETER_MAXLEN_CN90) ) {
    nIdleTime = atoi(szIdleTime);
    if (nIdleTime == 0) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if
  
  // get indepprogpath and add lib
  tsp00_Pathc     szDir;
  szDir.Init();
  tsp01_RteError  aRteError;
  sqlGetDbrootPath(szDir, TERM_WITH_DELIMITER_EO01, &aRteError);
  strcat(szDir, "lib");

  // change process environment
  DBMSrvMsg_Error oError;
  DBMSrvProc_EnvironmentVariable oLibpath(LDLIBPATH_IP00);
  if (oLibpath.Manipulate(szDir.asCharp(), ENVSEP_IP00, DBMSrvProc_EnvironmentVariable::ManipulatePrependOrAdd,oError) == DBMSrvProc_EnvironmentVariable::ManipulateError) {
     oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EXECUTE));
     nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oError);
  } // end if

  if (nFuncReturn == OK_CN00) {
    sprintf(szCommand, TXT_CMD_NIPINGTEST);
    sprintf(szProtocol, TXT_PRT_NIPINGTEST);
    nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szProtocol, false, true);
  } // end if

  if (nFuncReturn == OK_CN00) {
    sprintf(szCommand, TXT_CMD_NIPING, nIdleTime);
    sprintf(szProtocol, TXT_PRT_NIPING, nIdleTime);
    nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szProtocol, false, true);
  } // end if

  // reset process environment
  DBMSrvMsg_Error oDummy;
  oLibpath.Reset(oDummy);

  return nFuncReturn;
} /* end cn40ExecNiPingSrv */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40ExecXPU
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ExecXPU
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error   nFuncReturn = OK_CN00;
  const char  * pCommand    = command->args;
  const char  * pValue;
  char          szParameter[PARAMETER_MAXLEN_CN90];
  int           nUser       = SYSDBA_CN50;

  // read parameters 
  if (nFuncReturn == OK_CN00) {
    cn90GetToken(command->args, szParameter, 1, PARAMETER_MAXLEN_CN90);
    if (strncmp(szParameter, USER_TYPE_CN50, strlen(USER_TYPE_CN50)) == 0) {
      pValue = strchr(szParameter, ASSIGN_CHAR_CN50);
      if (pValue != NULL) {
        pValue++;
        pCommand = pCommand + strlen(szParameter);
        if (strcmp(pValue, USERTYPE_DBM_CN50) == 0) {
          nUser = FIRSTDBM_CN50;
        } else if (strcmp(pValue, USERTYPE_DBA_CN50) == 0) {
          nUser = SYSDBA_CN50;
        } else if (strcmp(pValue, USERTYPE_SAP_CN50) == 0) {
          nUser = SAPUSR_CN50;
        } else {
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "unknown usertype");
        } // end if
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "missing usertype");
      } // end if
    } // end if
  } // end if

  cn50DBMUser usrUser (vcontrol->dbname, cn50DBMUser::getUser(vcontrol->dbname, nUser));

  if (usrUser.existsOnDisk()) {
    char szTemp [PARAMETER_MAXLEN_CN90] = "";

    // build SQLOPT
    sp77sprintf(szTemp, PARAMETER_MAXLEN_CN90, TXT_SQLOPT, vcontrol->dbname.asCharp(), 
                                                           usrUser.getUserName().asCharp(),
                                                           usrUser.getClearMasterPwd().asCharp());
    // set SQLOPT
    putenv(szTemp);

    // build command
    sp77sprintf(szTemp, PARAMETER_MAXLEN_CN90, TXT_CMD_XPU, pCommand);

    // execute command
    nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szTemp, szTemp, false, true);

  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_NOUSER_CN00);
  } // end if

  return nFuncReturn;

} /* end cn40ExecXPU */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40LoadSysTab
 * ------------------------------------------------------------------
 */
tcn00_Error cn40LoadSysTab
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40LoadSysTab");

  tcn00_Error          nFuncReturn = OK_CN00;
  char                 szToken   [PARAMETER_MAXLEN_CN90];
  char                 szOptions [PARAMETER_MAXLEN_CN90];
  int                  nToken = 1;
  tcn00_UserNamec      szUsername;
  tsp00_Namec          szPassword;
  char               * pPassword;
  bool                 bMigrate = false;
  tsp00_KnlIdentifierc szTmp;
  szTmp.rawAssign(vcontrol->pCurrentUser->getUserName());

  if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
  } // end if

  szUsername.Init();
  szPassword.Init();

  while (cn90GetToken(command->args, szToken, nToken, PARAMETER_MAXLEN_CN90) && nFuncReturn == OK_CN00) {

    if (strcmp(szToken, OPT_LOAD_DBA) == 0) {
      // DBA found
      nToken++;
      if (cn90GetToken(command->args, szToken, nToken, PARAMETER_MAXLEN_CN90)) {
        /* search for password */
        pPassword = strchr (szToken, PWD_SEPARATOR_CN00);
        if (pPassword != NULL) {
          *pPassword = CHAR_STRINGTERM_CN90;
          ++pPassword;
          cn90Uncrypt(pPassword, false);
          szPassword.rawAssign(pPassword);
        } /* end if */
        szUsername.rawAssign(szToken);
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } // end if

    } else if (strcmp(szToken, OPT_LOAD_DOM) == 0) {
      // DOMAIN pwd found -> ignore it
      nToken++;
    } else if (strcmp(szToken, OPT_LOAD_MIGRATE) == 0) {
      bMigrate = true;
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if

    nToken++;
  } // end while

  // dba management
  if (nFuncReturn == OK_CN00) {
    DBMSrvMsg_Error oError;

    if (szUsername.length() == 0) {
      // load dba user
      cn50DBMUser         usrDBA    (vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname) );

      szUsername = usrDBA.getUserName();
      szPassword = usrDBA.getClearMasterPwd();

      // check sysdba property
      if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                       usrDBA.getUserName().asCharp(),
                                       usrDBA.getClearMasterPwd().asCharp(),
                                       SQL_CHECKSYSDBA,
                                       oError)) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAUNKNOWN_CN00);
      } // end if

    } else {
      cn50DBMUser  usrTempDBA(vcontrol->dbname);
      
      usrTempDBA.setUserName (szUsername)
                .setMasterPwd(szPassword);

      // check sysdba property
      if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                       usrTempDBA.getUserName().asCharp(),
                                       usrTempDBA.getClearMasterPwd().asCharp(),
                                       SQL_CHECKSYSDBA,
                                       oError)) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAWRONG_CN00);
      } // end if

      if (nFuncReturn == OK_CN00) {
        // create dba user
        cn50DBMUser         usrOldDBA(vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));
        cn50DBMUser         usrNewDBA(vcontrol->dbname);

        usrNewDBA.setUserName ( szUsername)
                 .setSDBAUser ( )
                 .setMasterPwd( szPassword)
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
        nFuncReturn = usrNewDBA.save();

        if (nFuncReturn != OK_CN00) {
          cn90AnswerIError(replyData, replyLen, nFuncReturn);
        } // end if
      } // end if
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00 && szUsername.length() == 0) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBAUNKNOWN_CN00);
  } // end if

  // lock the system tables
  DBMSrvKnl_Session oLockSession;
  DBMSrvMsg_Error   oLockSessionError;
  if (nFuncReturn == OK_CN00) {
      // connect the lock session with the sysdba
      if (!oLockSession.NormalConnect(vcontrol->dbname.asCharp(),
                                      szUsername.asCharp(),
                                      szPassword.asCharp(),
                                      oLockSessionError)) {
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oLockSessionError);
      } // end if

      // drop user SYSDBANA if exists (migration from 7.5.00 - this may fail with "-4003, Unknown user name"
      if ((nFuncReturn == OK_CN00) && !oLockSession.Execute(SQL_DBANLOCK_DROPUSER, oLockSessionError)) {
          const char* sqlReturnCode = (const char*)oLockSessionError.ArgValue(SDBMSGTAG_DBMSRV_SQL__RETURNCODE);
          if ( (oLockSessionError   != DBMSrvMsg_Error::DBMSrv_SQL) ||
               (sqlReturnCode       == NULL                       ) || 
               (atoi(sqlReturnCode) != -4003 /* Unknown user name */ )    ) {
              nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oLockSessionError);
          } // end if
      } // end if

      // create the lock table
      if ((nFuncReturn == OK_CN00) && !oLockSession.Execute(SQL_DBANLOCK_CHECK, oLockSessionError)) {
          oLockSessionError.ClearMessageList();
          // create schema  - this may fail with "-4003, Unknown user name"
          if ((nFuncReturn == OK_CN00) && !oLockSession.Execute(SQL_DBANLOCK_CREATESCHEMA, oLockSessionError)) {
              const char* sqlReturnCode = (const char*)oLockSessionError.ArgValue(SDBMSGTAG_DBMSRV_SQL__RETURNCODE);
              if ( (oLockSessionError   != DBMSrvMsg_Error::DBMSrv_SQL) ||
                   (sqlReturnCode       == NULL                       ) || 
                   (atoi(sqlReturnCode) != -6006 /* duplicate name */ )    ) {
                  nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oLockSessionError);
              } // end if
          } // end if
          // create table
          if ((nFuncReturn == OK_CN00) && !oLockSession.Execute(SQL_DBANLOCK_CREATETABLE, oLockSessionError)) {
              nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oLockSessionError);
          } // end if
          if ((nFuncReturn == OK_CN00) && !oLockSession.Execute(SQL_DBANLOCK_CREATEGRANT, oLockSessionError)) {
              nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oLockSessionError);
          } // end if
      } // end if

      // lock the table
      if ((nFuncReturn == OK_CN00) && !oLockSession.ExecuteWithoutCommit(SQL_DBANLOCK_LOCK, oLockSessionError)) {
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oLockSessionError);
      } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {

    sp77sprintf(szOptions, PARAMETER_MAXLEN_CN90, "%s %s", (bMigrate?OPT_LOAD_MIGRATE:""), LOAD_OPT);
    nFuncReturn = cn40_ExecPythonLoad(vcontrol,
                                      PYTHON_FILE,
                                      szUsername.asCharp(),
                                      szPassword.asCharp(),
                                      szOptions,
                                      replyData,
                                      replyLen,
                                      replyLenMax);

    if (nFuncReturn == OK_CN00 || nFuncReturn == ERR_EXECUTE_CN00) {
      DBMSrvMsg_Error oError;
      cn42BackupFile(vcontrol->dbname, FGET_REPMAN_CN42, oError);

      cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
      DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                  oUser.getUserName().asCharp(),
                                  oUser.getClearMasterPwd().asCharp(),
                                  LOAD_INIT,
                                  oError);

      cn20SaveParamsInfoToDB(vcontrol->dbname);
    } // end if

  } // end if

  // unlock the system tables
  oLockSession.Release(oLockSessionError);

  return nFuncReturn;
} // end cn40LoadSysTab

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40LoadR3Tab
 * ------------------------------------------------------------------
 */
tcn00_Error cn40LoadR3Tab
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error          nFuncReturn = OK_CN00;
  char                 szUsername [PARAMETER_MAXLEN_CN90] = "";
  char                 szPassword [PARAMETER_MAXLEN_CN90] = "";
  tcn002_XpValueString szValue;
  char                 szOptions [PARAMETER_MAXLEN_CN90];

  // dba management
  cn50DBMUser         usrDBA    ( vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));

  if (!usrDBA.existsOnDisk()) {
    nFuncReturn = ERR_USRREAD_CN00;
  } else {
    SAPDB_strcpy(szUsername, usrDBA.getUserName());
    cn90CalculateUncrypt(usrDBA.getMasterPwd(), szPassword, false);
  } // end if

  DBMSrvMsg_Error oParamGetMsg;
  if (!cn20XParamGetValue(vcontrol->dbname, PAN_RUNDIR, szValue, oParamGetMsg)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);
  } else {
      sp77sprintf(szOptions, PARAMETER_MAXLEN_CN90, "-fdir %s", szValue.asCharp());
      nFuncReturn = cn40_ExecPythonLoad(vcontrol,
                                        PYR3_FILE,
                                        (char   *) szUsername,
                                        (char   *) szPassword,
                                        (char   *) szOptions,
                                        replyData,
                                        replyLen,
                                        replyLenMax);

      cn20SaveParamsInfoToDB(vcontrol->dbname);
  } // end if

  return nFuncReturn;
} // end cn40LoadR3Tab

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40LoadLcApps
 * ------------------------------------------------------------------
 */
tcn00_Error cn40LoadLcApps
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40LoadLcApps");

  tcn00_Error          nFuncReturn = OK_CN00;
  char                 szSAPUsername [PARAMETER_MAXLEN_CN90] = "";

  cn50DBMUser oDBAUser   (vcontrol->dbname, cn50DBMUser::getSYSDBA  (vcontrol->dbname));
  if (nFuncReturn == OK_CN00) {
    if (!oDBAUser.lastEvent().IsEmpty()) {
      Msg_List aEvent = oDBAUser.lastEvent();
      aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAUNKNOWN));
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
    } // end if
  } // end if

  char szOptions[PARAMETER_MAXLEN_CN90];

  if (nFuncReturn == OK_CN00) {
    if (cn90GetToken(command->args, szSAPUsername, 1, PARAMETER_MAXLEN_CN90)) {
      char              * pPassword   = NULL; 
      tcn00_UserNamec     szUser;
      tsp00_Namec         szPassword;

      pPassword = strchr(szSAPUsername, PWD_SEPARATOR_CN00);
      if (pPassword != NULL) {
        *pPassword = CHAR_STRINGTERM_CN90;
        pPassword++;
        cn90Uncrypt(pPassword, false);
        szUser.rawAssign(szSAPUsername);
        szPassword.rawAssign(pPassword);
        
        sp77sprintf(szOptions, PARAMETER_MAXLEN_CN90, "-usap %s,%s", szUser.asCharp(), szPassword.asCharp());
          
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "invalid user specification");
      } // end if
    } else {
      cn50DBMUser oldSapUser(vcontrol->dbname, cn50DBMUser::getSAPUser(vcontrol->dbname));
      if (!oldSapUser.lastEvent().IsEmpty()) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 0, "SAP user not found");
      } else {
        sp77sprintf(szOptions, PARAMETER_MAXLEN_CN90, "-usap %s,%s", oldSapUser.getUserName().asCharp(), oldSapUser.getClearMasterPwd().asCharp());
      } // end if
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn40_ExecPythonLoad(vcontrol,
                                      LCAPPS_FILE,
                                      oDBAUser.getUserName(),
                                      oDBAUser.getClearMasterPwd(),
                                      szOptions,
                                      replyData,
                                      replyLen,
                                      replyLenMax);
  } // end if

  return nFuncReturn;
} // end cn40LoadLcApps

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40LoadTutorial
 * ------------------------------------------------------------------
 */
tcn00_Error cn40LoadTutorial
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40LoadTutorial");

  tcn00_Error          nFuncReturn = OK_CN00;

  cn50DBMUser oDBAUser   (vcontrol->dbname, cn50DBMUser::getSYSDBA  (vcontrol->dbname));
  if (nFuncReturn == OK_CN00) {
    if (!oDBAUser.lastEvent().IsEmpty()) {
      Msg_List aEvent = oDBAUser.lastEvent();
      aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBAUNKNOWN));
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn40_ExecPythonLoad(vcontrol,
                                      TUTORIAL_FILE,
                                      oDBAUser.getUserName(),
                                      oDBAUser.getClearMasterPwd(),
                                      "",
                                      replyData,
                                      replyLen,
                                      replyLenMax);
  } // end if

  return nFuncReturn;
} // end cn40LoadTutorial

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40ExecXKernprot
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ExecXKernprot
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error     nFuncReturn = OK_CN00;
  tsp00_C256      szDir;
  char            szOptions   [PARAMETER_MAXLEN_CN90] = "";
  char            szCommand   [PARAMETER_MAXLEN_CN90] = "";

  if (!cn90GetToken(command->args, szOptions, 1, PARAMETER_MAXLEN_CN90)) {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
  }

  if (nFuncReturn == OK_CN00) {

    cn90GetVersionBinPath(szDir);

#ifdef _WIN32
    sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, "\"\"%sxkernprot\" -d %s %s\"", (char *) szDir, &vcontrol->dbname[0], szOptions);
#else
    sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, "%sxkernprot -d %s %s", (char *) szDir, &vcontrol->dbname[0], szOptions);
#endif

    // change to rundir
    tcn002_XpValueString szRunDir;
    tsp01_RteError  rteError;

    DBMSrvMsg_Error oParamGetMsg;
    if (!cn20XParamGetValue(vcontrol->dbname, PAN_RUNDIR, szRunDir, oParamGetMsg)) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);
    } else {
      if (sqlchdirc(szRunDir.asCharp(), &rteError)) {

        nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szCommand, false, true);

        sqlchdirc(vcontrol->dbroot.asCharp(), &rteError);
      } else {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(rteError));
      } // end if
    } // end if

  } // end if

  return nFuncReturn;
} /* end cn40ExecXKernprot */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40GetVersion
 * ------------------------------------------------------------------
 */
tcn00_Error cn40GetVersion
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp00_Os              osNumber;
  char                osString [100];
  char                vsString [100];
  int                 nSwap;
  union
  {
    tsp00_Int4                      i4 ;
    tsp00_C4                        c4 ;
  } uMap;

  uMap.i4 = 65536;
  for ( nSwap = 0 ; nSwap < 4 ; nSwap ++ ) {
    if ( uMap.c4[nSwap] == 1 ) {
      break;
    } /* end if */
  } /* end for */

  tsp100_VersionID0   VersionID0;

  sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &VersionID0 );

  sp77sprintf(vsString, 100, "%d.%d.%02d",
                    (int) VersionID0.MajorVersion_sp100,
                    (int) VersionID0.MinorVersion_sp100,
                    (int) VersionID0.CorrLevel_sp100);

  sqlos (&osNumber);

  switch (osNumber) {
    case os_unix:
      SAPDB_strcpy (osString, "UNIX");
      break;
    case os_vms:
      SAPDB_strcpy (osString, "VMS");
      break;
    case os_windows:
      SAPDB_strcpy (osString, "WINDOWS");
      break;
    case os_win32:
      SAPDB_strcpy (osString, "WIN32");
      break;
    case os_os2:
      SAPDB_strcpy (osString, "OS2");
      break;
    default:
      sp77sprintf (osString, 100, "<Unknown OS %d>", (int) osNumber);
      break;
  } /* end switch */

  sp77sprintf(replyData, replyLenMax, "%s%s%s%s\"%s\",\"%s\",\"%s\",%s,%s,%d%s",
                     ANSWER_OK_CN00,
                     LINE_SEPSTRING_CN00,
                     "version,os,dbroot,logon,code,swap",
                     LINE_SEPSTRING_CN00,
                     vsString,
                     osString,
                     (char *) vcontrol->dbroot,
                     BOOLSTR_CN00(vcontrol->bIsSAPDBAdmi),
                     (' ' == 32) ? "ASCII" : "EBCDIC",
                     nSwap,
                     LINE_SEPSTRING_CN00);

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end cn40GetVersion */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBMGetPath
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBMGetPath
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  char                szMode[PARAMETER_MAXLEN_CN90];
  tsp00_Pathc         szPath;
  tsp01_RteError      RteError;
  tsp00_Bool          bDBRootWarning = false;

  cn90GetToken(command->args, szMode, 1, PARAMETER_MAXLEN_CN90);

  if (stricmp(szMode, RTE_INDPROG) == 0) {
    if (!sqlGetIndependentProgramsPath ( szPath, 0, &RteError)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(RteError));
    } // end if
    bDBRootWarning = !sqlIsIndependentProgramsPathInRegistry();
  } else if (stricmp(szMode, RTE_INDDATA) == 0) {
    if (!sqlGetIndependentDataPath ( szPath, 0, &RteError)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(RteError));
    } // end if
    bDBRootWarning = !sqlIsIndependentDataPathInRegistry();
  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    sp77sprintf(replyData, replyLenMax, "%s%s%s%s%s%s", 
                        ANSWER_OK_CN00, 
                        LINE_SEPSTRING_CN00,
                        szPath.asCharp(),
                        LINE_SEPSTRING_CN00,
                        bDBRootWarning ? "INFO: Path extracted from $DBROOT" : "",
                        bDBRootWarning ? LINE_SEPSTRING_CN00 : "");

    *replyLen = (int)strlen(replyData);
  } /* end if */

  return nFuncReturn;
} /* end cn40DBMGetPath */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBMSetPath
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBMSetPath
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  char                szMode[PARAMETER_MAXLEN_CN90];
  tsp00_Pathc         szPath;
  tsp01_RteError      RteError;

  cn90GetToken(command->args, szMode, 1, PARAMETER_MAXLEN_CN90);
  cn90GetToken(command->args, szPath, 2, szPath.size()+1);
  cn90StripQuotes(szPath);

  if (stricmp(szMode, RTE_INDPROG) == 0) {
    if (!sqlSetIndependentProgramsPath ( szPath, &RteError)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(RteError));
    } // end if
  } else if (stricmp(szMode, RTE_INDDATA) == 0) {
    if (!sqlSetIndependentDataPath ( szPath, &RteError)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(RteError));
    } // end if
  } else {
    nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK (replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} /* end cn40DBMSetPath */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBCreate
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBCreate
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40DBCreate");

  tcn00_Error         nFuncReturn = OK_CN00;
  char                szToken    [PARAMETER_MAXLEN_CN90] = "";
  int                 nIndex      = 1;

  SAPDB_Bool          bRegistered  = false;

  SAPDB_Bool          bAutostart  = false;
  SAPDB_Bool          bAddUser    = false;
  SAPDB_Bool          bAddGroup   = false;
  SAPDB_Bool          bOsLogin    = false;

  tsp00_DbNamec       szDBName;

  tcn00_UserNamec     szUser;
  tsp00_Namec         szPassword;

  tcn00_UserNamec     szOsUser;
  tsp00_Namec         szOsPassword;

  tsp00_Namec         szGroup;

  // read parameters, option and values
  while (nFuncReturn == OK_CN00) {
    if (!cn90GetToken(command->args, szToken, nIndex, PARAMETER_MAXLEN_CN90)) {
      break;
    } // end if
    // autostart option    
    if (strcmp(szToken, OPT_OPT_AUTO) == 0 || strcmp(szToken, OPTS_OPT_AUTO) == 0) {
#ifdef _WIN32
      bAutostart = true;
#endif
    // group service user    
    } else if (strcmp(szToken, OPT_OPT_USER) == 0 || strcmp(szToken, OPTS_OPT_USER) == 0) {
#ifdef _WIN32
      bAddUser = true;
#endif
    // group option    
    } else if (strcmp(szToken, OPT_OPT_GROUP) == 0 || strcmp(szToken, OPTS_OPT_GROUP) == 0) {
      if (!cn90GetToken(command->args, szToken, ++nIndex, PARAMETER_MAXLEN_CN90)) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1, "Missing group name");
      } else {
        bAddGroup = true;
        szGroup.rawAssign(szToken);
      } // end if
    } else {
      // database name
      szDBName.rawAssign(szToken);
      cn90StrUpperCopy(szDBName, szDBName, false);
      break;
    } // end if
    ++nIndex;
  } // end while    

  // first database manager operator
  if (nFuncReturn == OK_CN00) {
    if (!cn90GetToken(command->args, szToken, ++nIndex, PARAMETER_MAXLEN_CN90)) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1, "Missing first Database Manager Operator");
    } else {
      char * pPassword = strchr(szToken, PWD_SEPARATOR_CN00);
      if (pPassword != NULL) {
        *pPassword = CHAR_STRINGTERM_CN90;
        pPassword++;
        szUser.rawAssign(szToken);
        szPassword.rawAssign(pPassword);
        cn90Uncrypt(szPassword, false);
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1 , "Invalid first Database Manager Operator");
      } // end if
    } // end if
  } // end if

  // operating system user
  if (nFuncReturn == OK_CN00) {
    if (cn90GetToken(command->args, szToken, ++nIndex, PARAMETER_MAXLEN_CN90)) {
      char * pPassword = strchr(szToken, PWD_SEPARATOR_CN00);
      if (pPassword != NULL) {
        *pPassword = CHAR_STRINGTERM_CN90;
        pPassword++;
        szOsUser.rawAssign(szToken);
        szOsPassword.rawAssign(pPassword);
        cn90Uncrypt(szOsPassword, false);
        bOsLogin = true;
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1 , "Invalid operating system user");
      } // end if
    } // end if
    if (bAddUser && !bOsLogin && (nFuncReturn == OK_CN00)) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1 , "Missing operating system user");
    } // end if
  } // end if

  // check dbname
  if (nFuncReturn == OK_CN00) {
    if ((strncmp(szDBName, SERVICE_PARAMFILE_ID_OLD_CN90, strlen(SERVICE_PARAMFILE_ID_OLD_CN90)) == 0) ||
        (strncmp(szDBName, SERVICE_PARAMFILE_ID_CN90,     strlen(SERVICE_PARAMFILE_ID_CN90    )) == 0) ||
        (strncmp(szDBName, "-",                           strlen("-"                          )) == 0) ||
        (strlen(szDBName) > MAX_DBNAME                                                               )    ) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_INVDBNAME, Msg_Arg(SDBMSGTAG_DBMSRV_INVDBNAME__DBMSRV_COMP,szDBName.asCharp())));
    } else {
      for (nIndex = 0; szDBName[nIndex] != 0; ++nIndex) {
        nFuncReturn = (((unsigned char) szDBName[nIndex]) < 128) ?  nFuncReturn : cn90AnswerIError(replyData, replyLen, ERR_ASCII_CN00, 1, "Invalid database name");
      } // end for
    } // end if
  } // end if

  // check existence of database
  if (nFuncReturn == OK_CN00) {
    if (cn90DBExists(szDBName)) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBEXIST_CN00);
    } // end if
  } // end if

  if( nFuncReturn == OK_CN00 ) {
    // we have a valid DB name, start using shared memory
    DBMSrvMsg_Error shmInitError;
    if( !DBMSrvShM_ShmAdmin::TheAdmin().initialize(szDBName.asCharp(), shmInitError) )
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(shmInitError);
  }

  // check first dbm operator
  if (nFuncReturn == OK_CN00) {
    if (strnicmp(szUser, INVALID_USER_CN50, strlen(INVALID_USER_CN50)) == 0) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_INVUSRNAME, Msg_Arg(SDBMSGTAG_DBMSRV_INVUSRNAME__DBMSRV_COMP,szUser.asCharp())));
    } else {
      if (!ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(szUser    .asCharp()) || 
          !ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier(szPassword.asCharp())    ) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00, 1, "Invalid first database manager operator");
      } // end if
    } // end if
  } // end if

  // os logon
  if ((nFuncReturn == OK_CN00) && bOsLogin && !vcontrol->bIsSAPDBAdmi) {
#ifndef _WIN32      
    Msg_List oMsg;
    oMsg.ClearMessageList();
    if (RTE_VerifyUserPasswordCombination(szOsUser, szOsPassword, oMsg)) {
      vcontrol->bIsSAPDBAdmi = RTE_VerifyUserAsSapdbAdministrator (szOsUser, oMsg);
    } else {
      DBMSrvMsg_RTEError rteError(oMsg);
      rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NEEDADMI));
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(rteError);
    } // end if
#else
    tsp9_rte_xerror     xError;
    sqlxlogon (szOsUser, szOsPassword, &xError);
    if (xError.xe_result != 0) {
      DBMSrvMsg_RTEError rteError(xError);
      rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SYSLOGON));
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(rteError);
    } else {
      vcontrol->bIsSAPDBAdmi = true;
    } // end if
#endif
  } // end if

  // check logon state
  if (nFuncReturn == OK_CN00 && !vcontrol->bIsSAPDBAdmi) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NEEDADMI));
  } // end if

  // register database
  if (nFuncReturn == OK_CN00) {

    Msg_List err;        

    RTE_ActiveInstallation * activeInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBRoot(vcontrol->dbroot,err);
    if (activeInstallation != NULL) {
      if (!bAddUser) {
        activeInstallation->RegisterDatabase(szDBName, bAutostart, NULL, NULL, err);
      } else {
        activeInstallation->RegisterDatabase(szDBName, bAutostart, szOsUser, szOsPassword, err);
      } // end if
      delete activeInstallation;

      if (Msg_List::Error == err.Type()) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(err));
      } else {
        bRegistered = true;
      } // end if
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if

  } // end if

  // create DBM-User
  if (nFuncReturn == OK_CN00) {
    vcontrol->pCurrentUser->clear();
    vcontrol->pCurrentUser->setDBName       ( szDBName    )
                           .setUserName     ( szUser      )
                           .setMasterPwd    ( szPassword  )
                           .setServerRights ( DBMMaskAll_CN50, DBMUserDefault_CN50 )
                           .setGUIRights    ( DBMMaskAll_CN50, DBMUserDefaultGUI_CN50 )
                           .setColdUser     ( );

    nFuncReturn = vcontrol->pCurrentUser->save(true);
    if ((nFuncReturn == OK_CN00) && bAddGroup) {
      vcontrol->pCurrentUser->setSupportGroup(szGroup);
    } // end if
    if (nFuncReturn == OK_CN00) {
      vcontrol->pCurrentUser->checkMasterPwd(szPassword);
    } else {
      cn90AnswerEvent(replyData, replyLen, replyLenMax, vcontrol->pCurrentUser->lastEvent());
    } // end if
  } // end if

  // generate answer
  if (nFuncReturn == OK_CN00) {
    vcontrol->dbname.rawAssign(szDBName);
    vcontrol->pHSSNodes->SetConnectionData(szDBName.asCharp(), szUser.asCharp(), szPassword.asCharp());
    cn90AnswerOK (replyData, replyLen, NULL);
  } else {
    if (bRegistered) {
      Msg_List err;        
      RTE_ActiveInstallation * activeInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBName(szDBName,err);
      activeInstallation->UnregisterDatabase(szDBName,err);
      delete activeInstallation;
    } // end if
    
    // something went wrong: remove shared memory files
    // this does not return errors if shared memory was not initialized at all
    DBMSrvMsg_Error shmDeleteError;
    DBMSrvShM_ShmAdmin::TheAdmin().close(shmDeleteError);
    DBMSrvShM_ShmAdmin::TheAdmin().removePersistence(shmDeleteError);
    if( !shmDeleteError.IsEmpty() )
        DBMSrv_Reply(replyData, replyLen, replyLenMax).appendMessageList(shmDeleteError); // still return the nFuncReturn of the
                                                                                          // previous error (the reason why we are here)

    vcontrol->pCurrentUser->clear();
  } // end if

  return nFuncReturn;
} /* end cn40DBCreate */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBEnum
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBEnum
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
    tcn00_Error         nFuncReturn = OK_CN00;
    char                szOption[PARAMETER_MAXLEN_CN90];
    bool                bService = false;
    Msg_List            oRTEErr;        
    char              * pCurrPos;

    if (cn90GetToken(command->args, szOption, 1, PARAMETER_MAXLEN_CN90)) {
        if (stricmp(szOption, OPTS_ENUM_SRV) == 0) {
            bService = true;
        } else {
            return cn90AnswerIError (replyData, replyLen, ERR_PARAM_CN00);
        } // end if
    } // end if

    // init OK
    cn90AnswerOK (replyData, replyLen, NULL);
    pCurrPos = replyData + strlen(replyData);

    Container_List<RTE_Installation>* pInstallations = (RTE_DBRegister::Instance()).GetInstallations();

    if (pInstallations) {
        Container_List<RTE_Installation>::Iterator oInstIterator = pInstallations->Begin();

        while (oInstIterator != pInstallations->End()) {

            // now we have a installation
            Container_List<RTE_Database> * pDatabases = (*oInstIterator).GetDatabases(oRTEErr);
            if (pDatabases) {
                Container_List<RTE_Database>::Iterator oDbIterator = pDatabases->Begin();
                while (oDbIterator != pDatabases->End()) {

                    // now we have a database
                    RTE_SpeedList & oSpeeds = (*oDbIterator).GetDBSpeeds();
                    RTE_SpeedList::Iterator oSpeedIterator = oSpeeds.Begin();

                    while (oSpeedIterator != oSpeeds.End()) {

                        // now we have a single speed
                        if ( ( ((*oDbIterator).GetDBName()[0] != '_') &&  
                               ((*oDbIterator).GetDBName()[0] != '.')     ) || bService) {

                            sp77sprintf (pCurrPos, replyLenMax - (int)(pCurrPos - replyData), "%s%s%-39s%s%d.%d.%02d.%02d%s%s%s%s%s",
                                                (char *) (*oDbIterator).GetDBName(),
                                                VALUE_SEPSTRING_CN00,
                                                (char *) (*oDbIterator).GetDBRoot(),
                                                VALUE_SEPSTRING_CN00,
                                                (int) (*oInstIterator).GetVersion().release,
                                                (int) (*oInstIterator).GetVersion().major,
                                                (int) (*oInstIterator).GetVersion().minor,
                                                (int) (*oInstIterator).GetVersion().build,
                                                VALUE_SEPSTRING_CN00,
                                                (*oSpeedIterator).Name(),
                                                VALUE_SEPSTRING_CN00,
                                                (*oSpeedIterator) == (*oDbIterator).GetActiveSpeed() ? "running" : "offline",
                                                LINE_SEPSTRING_CN00);
                            pCurrPos = pCurrPos + strlen(pCurrPos);
                        } // end if
                        

                        ++oSpeedIterator;
                    } // end while

                    ++oDbIterator;
                } // end while
                delete pDatabases;
            } // end if
            ++oInstIterator;
        } // end while

        delete pInstallations;
    } // end if

    *replyLen = (int) strlen(replyData);

    return nFuncReturn;
} // end cn40DBEnum

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBClear
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBClear
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp9_rte_xerror     xError;

  sqlxclear( vcontrol->dbname, vcontrol->dbroot, &xError );

  if (xError.xe_result != csp9_xrte_ok) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
  } else {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;

} /* end cn40DBClear */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBStart
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBStart
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  return cn40_ChangeDBState ( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax,
                              DBMSrvKnl_State::StateAdmin);
} /* end cn40DBStart */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBStop
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBStop
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  return cn40_ChangeDBState ( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax,
                              DBMSrvKnl_State::StateOffline,
                              true);

} /* end cn40DBStop */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBWarm
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBWarm
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  return cn40_ChangeDBState ( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax,
                              DBMSrvKnl_State::StateOnline);

} /* end cn40DBWarm */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBOffline
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBOffline
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  return cn40_ChangeDBState ( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax,
                              DBMSrvKnl_State::StateOffline);

} /* end cn40DBOffline */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBCold
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBCold
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error     nFuncReturn = OK_CN00;
  tsp9_pgm_kind   pgmKind;
  bool            bDump;
  bool            bCluster = false;
  char            szNewArgs [PARAMETER_MAXLEN_CN90] = "";
  char            szRestart [PARAMETER_MAXLEN_CN90] = "";

  // check current and requested speed
  if (cn40_StartStopOptions(command->args, pgmKind, bDump, szRestart, bCluster) == OK_CN00) {
    if (pgmKind == csp9_any_pgm) {
      const RTE_SpeedInfo & oSpeed = DBMSrvKnl_State::Speed(vcontrol->dbname);

      if  (oSpeed == speedSlow) {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s %s", (char   *) command->args, (char   *) OPT_SPEED_SLOW);
      } else if  (oSpeed == speedFast) {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s %s", (char   *) command->args, (char   *) OPT_SPEED_FAST);
      } else if  (oSpeed == speedTest) {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s %s", (char   *) command->args, (char   *) OPT_SPEED_TEST);
      } else {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s", (char   *) command->args);
      } // end if

      command->args = &szNewArgs[0];
    } // end if
  }

  return cn40_ChangeDBState ( vcontrol,
                              command,
                              replyData,
                              replyLen,
                              replyLenMax,
                              DBMSrvKnl_State::StateAdmin);
} /* end cn40DBCold */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBRestart
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBRestart
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error     nFuncReturn = OK_CN00;
  tsp9_pgm_kind   pgmKind;
  bool            bDump;
  bool            bCluster = false;
  char            szNewArgs [PARAMETER_MAXLEN_CN90] = "";
  char            szRestart [PARAMETER_MAXLEN_CN90] = "";

  // check current and requested speed
  if (cn40_StartStopOptions(command->args, pgmKind, bDump, szRestart, bCluster) == OK_CN00) {
    if (pgmKind == csp9_any_pgm) {
      const RTE_SpeedInfo & oSpeed = DBMSrvKnl_State::Speed(vcontrol->dbname);

      if  (oSpeed == speedSlow) {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s %s", (char   *) command->args, (char   *) OPT_SPEED_SLOW);
      } else if  (oSpeed == speedFast) {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s %s", (char   *) command->args, (char   *) OPT_SPEED_FAST);
      } else if  (oSpeed == speedTest) {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s %s", (char   *) command->args, (char   *) OPT_SPEED_TEST);
      } else {
        sp77sprintf (szNewArgs, PARAMETER_MAXLEN_CN90, "%s", (char   *) command->args);
      } // end if

      command->args = &szNewArgs[0];
    } // end if
  }

  nFuncReturn = cn40_ChangeDBState ( vcontrol,
                                     command,
                                     replyData,
                                     replyLen,
                                     replyLenMax,
                                     DBMSrvKnl_State::StateOffline);
  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn40_ChangeDBState ( vcontrol,
                                       command,
                                       replyData,
                                       replyLen,
                                       replyLenMax,
                                       DBMSrvKnl_State::StateOnline);
  } // end if

  return nFuncReturn;
} /* end cn40DBRestart */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40ShowSomething
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ShowSomething
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
FUNCTION_DBG_MCN00_1("cn40ShowSomething");
  tcn00_Error    nFuncReturn = OK_CN00;

  char         * pCurrent  = replyData;
  char         * pContinue = NULL;
  bool           bNoNewLine;
  tsp00_Bool     bOk;
  tsp00_Int4     nExitCode;
  tsp00_Int4     nSize;
  ceo43_xshow  * pObjShow = NULL;
  ceo43_xshow  * pObjTmp  = NULL;

  // check for intervall and repeat option and reject
  Tools_DynamicUTF8String szLastArg = command->oArguments.getValue(command->oArguments.getCount());
  if (szLastArg.FindFirstNotOf("0123456789") == Tools_DynamicUTF8String::NPos) {
     return DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szLastArg.CharPtr()),
                                                                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, "show")));
  } // end if

  pObjShow = new ceo43_xshow(command->args, vcontrol->dbname, bOk);

  // Init OK Message
  if (bOk && pObjShow != NULL) {

    sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), "%s%s", ANSWER_OK_CN00,  LINE_SEPSTRING_CN00);
    pCurrent = pCurrent    + strlen(pCurrent);
    sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), "%*s%s", (int) strlen(KEY_DIRCONTINUE_CN00), " ", LINE_SEPSTRING_CN00);
    pContinue = pCurrent;
    pCurrent = pCurrent    + strlen(pCurrent);

    nSize    = (tsp00_Int4)(replyLenMax - strlen(replyData));

    while (pObjShow->eo43NextLine(pCurrent, nSize, bNoNewLine, nExitCode) > 0 && nSize > 200) {
      pCurrent = pCurrent + strlen(pCurrent);
      nSize = (tsp00_Int4)(replyLenMax - strlen(replyData));
      if (bNoNewLine && nSize > 0) {
        sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), LINE_SEPSTRING_CN00);
        pCurrent = pCurrent + strlen(pCurrent);
      } // end if
      nSize = (tsp00_Int4)(replyLenMax - strlen(replyData));
    } // end while

    if (nSize <= 200) {
      cn40FreeCommandObj(vcontrol);
      vcontrol->pCommandObj = (void *) pObjShow;
      strncpy(pContinue, KEY_DIRCONTINUE_CN00, strlen(KEY_DIRCONTINUE_CN00));
      vcontrol->szNextCommand.rawAssign("show_next");
      vcontrol->nNextCommandSkip = 1;
    } else {
      if (bNoNewLine) {
        sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), LINE_SEPSTRING_CN00);
        pCurrent = pCurrent + strlen(pCurrent);
      } // end if
      delete pObjShow;
    } // end if

    *replyLen = (int)strlen(replyData);

    if (strstr(replyData, "usage: x_cons") != NULL) {
      DBMSrvMsg_Error aPARAM(SDBMSG_DBMSRV_INVPARAM, Msg_Arg(SDBMSGTAG_DBMSRV_INVPARAM__PARAMETER,command->args));
      aPARAM.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aPARAM);
    } // end if

  } else {
    nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_EXECUTE_CN00);
  } // end if

  return nFuncReturn;
} /* end cn40ShowSomething */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40ShowList
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ShowList
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  char         * pCurrent = replyData;
  char         * pShowCmd = NULL;
  char           szLine[PARAMETER_MAXLEN_CN90];
  char           szToken[PARAMETER_MAXLEN_CN90];
  bool           bNoNewLine;
  tsp00_Bool     bOk;
  tsp00_Int4     nExitCode;
  tsp00_Int4     nSize;
  ceo43_CmdPipe  pObjXCons;

  sp77sprintf(szLine, PARAMETER_MAXLEN_CN90, "%s %s %s", TXT_CMD_XCONS, (char   *) vcontrol->dbname, TXT_CMD_HELP);
  pObjXCons.eo43OpenCommand(szLine, bOk);

  // Init OK Message
  if (bOk) {

    sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), "%s%s", ANSWER_OK_CN00,  LINE_SEPSTRING_CN00);
    pCurrent = pCurrent    + strlen(pCurrent);
    nSize    = (tsp00_Int4)(replyLenMax - strlen(replyData));

    while (pObjXCons.eo43NextLine(szLine, PARAMETER_MAXLEN_CN90, bNoNewLine, nExitCode) > 0) {
      pShowCmd = strstr(szLine, TXT_CMD_SHOW);
      if (pShowCmd != NULL) {
        cn90GetToken(pShowCmd, szToken, 2, PARAMETER_MAXLEN_CN90);
        sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), "%s%s", szToken,  LINE_SEPSTRING_CN00);
        pCurrent = pCurrent + strlen(pCurrent);
      } // end if
    } // end while

      *replyLen = (int)strlen(replyData);

  } else {
    nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_EXECUTE_CN00);
  } // end if

  return nFuncReturn;
} /* end cn40ShowList */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40ShowNext
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ShowNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  char         * pCurrent  = replyData;
  char         * pContinue = NULL;
  bool           bNoNewLine;
  tsp00_Int4     nExitCode;
  tsp00_Int4     nSize;
  ceo43_xshow  * pObjShow = NULL;
  ceo43_xshow  * pObjTmp  = NULL;

  pObjShow = (ceo43_xshow *) vcontrol->pCommandObj;

  // Init OK Message
  if (pObjShow != NULL) {

  sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), "%s%s", ANSWER_OK_CN00,  LINE_SEPSTRING_CN00);
  pCurrent = pCurrent    + strlen(pCurrent);
  sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), "%*s%s", (int) strlen(KEY_DIRCONTINUE_CN00), " ", LINE_SEPSTRING_CN00);
  pContinue = pCurrent;
  pCurrent = pCurrent    + strlen(pCurrent);

  nSize    = (tsp00_Int4)(replyLenMax - strlen(replyData));

  while (pObjShow->eo43NextLine(pCurrent, nSize, bNoNewLine, nExitCode) > 0 && nSize > 200) {
    pCurrent = pCurrent + strlen(pCurrent);
    nSize = (tsp00_Int4)(replyLenMax - strlen(replyData));
    if (bNoNewLine && nSize > 0) {
      sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + strlen(pCurrent);
    } // end if
    nSize = (tsp00_Int4)(replyLenMax - strlen(replyData));
  } // end while

  if (nSize <= 200) {
    vcontrol->pCommandObj = (void *) pObjShow;
    strncpy(pContinue, KEY_DIRCONTINUE_CN00, strlen(KEY_DIRCONTINUE_CN00));
    vcontrol->szNextCommand.rawAssign("show_next");
    vcontrol->nNextCommandSkip = 1;
  } else {
    if (bNoNewLine) {
      sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + strlen(pCurrent);
    } // end if
    cn40FreeCommandObj(vcontrol);
  } // end if

  *replyLen = (int)strlen(replyData);

  } else {
    nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_EXECUTE_CN00);
  } // end if

  return nFuncReturn;
} /* end cn40ShowNext */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DBConsole
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DBConsole
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error    nFuncReturn = OK_CN00;
  char         * pCurrent = replyData;
  char           szLine[PARAMETER_MAXLEN_CN90];
  bool           bNoNewLine;
  tsp00_Bool     bOk;
  tsp00_Int4     nExitCode;
  tsp00_Int4     nSize;
  ceo43_CmdPipe  pObjXCons;


    // check for intervall and repeat option and reject if this a show command
    Tools_DynamicUTF8String szLastArg = command->oArguments.getValue(command->oArguments.getCount());
    Tools_DynamicUTF8String szFirstArg = command->oArguments.getValue(1);
    if ((szLastArg.FindFirstNotOf("0123456789") == Tools_DynamicUTF8String::NPos) &&
        (szFirstArg.ToLower().Compare((Tools_DynamicUTF8String::ConstPointer) "show", szFirstArg.Length()) == 0)) {
        return DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                                    Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szLastArg.CharPtr()),
                                                                                    Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, "db_cons")));
    } // end if

    sp77sprintf(szLine, PARAMETER_MAXLEN_CN90, "%s %s %s", TXT_CMD_XCONS, (char   *) vcontrol->dbname, command->args);

    pObjXCons.eo43OpenCommand(szLine, bOk);

    if (bOk) {

      // init message
      sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), "%70s%s", " ",  LINE_SEPSTRING_CN00);
      pCurrent = pCurrent    + strlen(pCurrent);

      nSize    = (tsp00_Int4)(replyLenMax - strlen(replyData));

      while (pObjXCons.eo43NextLine(pCurrent, nSize, bNoNewLine, nExitCode) > 0 && nSize > 100) {
        pCurrent = pCurrent + strlen(pCurrent);
        nSize = (tsp00_Int4)(replyLenMax - strlen(replyData));
        if (bNoNewLine && nSize > 0) {
          sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), LINE_SEPSTRING_CN00);
          pCurrent = pCurrent + strlen(pCurrent);
        } // end if
        nSize = (tsp00_Int4)(replyLenMax - strlen(replyData));
      } // end while

      if (bNoNewLine) {
        sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent - replyData), LINE_SEPSTRING_CN00);
        pCurrent = pCurrent + strlen(pCurrent);
      } // end if

      if (nExitCode != 0) {
        nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_EXECUTE_CN00);
      } else {
        cn90AnswerOK (replyData, replyLen, NULL);
      } // end if
      replyData[strlen(replyData) - 1] = ' ';
      replyData[strlen(replyData)] = ' ';

      *replyLen = (int)strlen(replyData);

    } else {
      nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_EXECUTE_CN00);
    } // end if

  return nFuncReturn;
} /* end cn40DBConsole */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40FreeCommandObj
 * ------------------------------------------------------------------
 */
void cn40FreeCommandObj
      ( VControlDataT * vcontrol )
{
  ceo43_xshow  * pObjShow = NULL;

  if (vcontrol != NULL) {
    pObjShow = (ceo43_xshow *) vcontrol->pCommandObj;
    if (pObjShow != NULL) {
      delete pObjShow;
    } // end if
    vcontrol->pCommandObj = NULL;
  } // end if

} // end cn40FreeCommandObj


static bool cn40_UnRegisterServiceDBAfterError( tsp00_DbNamec& szDBName, Msg_List& errOut ) {
    bool success(false);
    RTE_ActiveInstallation* activeInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBName(szDBName,errOut);
    if( activeInstallation != NULL ) {
        if( !activeInstallation->UnregisterDatabase(szDBName, errOut) ) {
//          error!
        }
        else
            success = true;
        delete activeInstallation;
    }
    return success;
}

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40_RegisterServiceDB
 * ------------------------------------------------------------------
 */
#define DBNAME_LENGTH 8
// 65 bis 90

static bool cn40_RegisterServiceDB
      ( const tsp09_RteInstallationInfoNew  & relInfoIn)
{
  FUNCTION_DBG_MCN00_1("cn40_RegisterServiceDB");

  bool             bReturn = false;
  char             szTemp[PARAMETER_MAXLEN_CN90];
  char           * pTemp = NULL;
  tsp00_DbNamec    szDBName;
  int              nPostfix = 1;
//  tsp9_rte_xerror  xError;

  cn90ServiceDB(szDBName, relInfoIn.dbroot);

  if (szDBName.length() == 0) {
    sprintf(szTemp,
            "%sM%01d%01d%02d%02d", 
            SERVICE_PARAMFILE_ID_CN90,
            relInfoIn.version.no[0],
            relInfoIn.version.no[1],
            relInfoIn.version.no[2],
            relInfoIn.version.no[3]);
    szTemp[DBNAME_LENGTH] = CHAR_STRINGTERM_CN90;
    pTemp     = &szTemp[(strlen(szTemp) > (DBNAME_LENGTH - 2)) ? (DBNAME_LENGTH - 2) : strlen(szTemp)];
    szDBName.rawAssign(szTemp);

    while (cn90DBExists(szDBName) && nPostfix < 100) {
      sp77sprintf(pTemp, PARAMETER_MAXLEN_CN90, "%02d", nPostfix);
      ++nPostfix;
      szDBName.rawAssign(szTemp);
    } // end while
  } // end if  

  if (nPostfix < 100) {
//    sqlxregisterdb (szDBName, relInfoIn.dbroot, csp9_any_pgm, csp9_no_options, &xError);

    Msg_List err;        
    RTE_ActiveInstallation * activeInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBRoot(relInfoIn.dbroot,err);

    if(!activeInstallation->RegisterDatabase(szDBName, false, NULL, NULL, err))
//    if(!activeInstallation->RegisterDatabase(szDBName,err))
    {
//        error!
    }
    delete activeInstallation;
    if(Msg_List::Error != err.Type()){
      // initialize shared memory for service database
      DBMSrvMsg_Error shmInitError;
      if( !DBMSrvShM_ShmAdmin::TheAdmin().initialize(szDBName.asCharp(), shmInitError) ) {
        DBMSrvMsg_Error unregError;
        cn40_UnRegisterServiceDBAfterError(szDBName, unregError);
        unregError.Overrule(shmInitError);
        unregError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SERVICEDB, Msg_Arg(SDBMSGTAG_DBMSRV_SERVICEDB__DBMSRV_COMP,szDBName.asCharp())));
        DBMLog_Logger::getInstance().addMessageList(unregError);
        return false;
      }
      if (cn20CreateSrvParam( szDBName ) == OK_CN00) {
        cn50DBMUser     oUser;
        tcn00_UserNamec szUser;
        tsp00_Namec     szPassword;
        szUser.rawAssign("SERVICE");
        szPassword.rawAssign("SERVICE");
        oUser.clear();
        oUser.setDBName      ( szDBName  )
             .setColdUser    (           )
             .setUserName    ( szUser    )
             .setMasterPwd   ( szPassword)
             .setServerRights( DBMMaskAll_CN50, DBMUserDefault_CN50   )
             .setGUIRights   ( DBMMaskAll_CN50, DBMUserDefaultGUI_CN50);

        if (oUser.save() == OK_CN00) {
          bReturn = true;
        } else {
//          sqlxunregisterdb (szDBName, csp9_any_pgm, &xError);

          Msg_List err;  
          cn40_UnRegisterServiceDBAfterError(szDBName, err); // this error is never reported
        } // end if
      } else {
//        sqlxunregisterdb (szDBName, csp9_any_pgm, &xError);

        Msg_List err;        
        cn40_UnRegisterServiceDBAfterError(szDBName, err); // this error is never reported
      } // end if
    } else {
      DBMLog_Logger::getInstance().addMessageList(DBMSrvMsg_RTEError(err));
    } // end if
  } else {
    DBMLog_Logger::getInstance().addMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_SERVICEDB, Msg_Arg(SDBMSGTAG_DBMSRV_SERVICEDB__DBMSRV_COMP,szDBName.asCharp())));
  } // end if
  
  // close the shared memory of the service database
  DBMSrvMsg_Error shmCloseError;
  if( !DBMSrvShM_ShmAdmin::TheAdmin().close(shmCloseError) ) {
    shmCloseError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SERVICEDB, Msg_Arg(SDBMSGTAG_DBMSRV_SERVICEDB__DBMSRV_COMP,szDBName.asCharp())));
    DBMLog_Logger::getInstance().addMessageList(shmCloseError);
    // no need to unregister the service db here
    return false;
  }
  
  return bReturn;
} // end cn40_RegisterServiceDB

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40_CheckInstKey
 * ------------------------------------------------------------------
 */
static bool cn40_CheckInstKey
      ( const tsp09_RteInstallationInfoNew  & relInfoIn )
{
  bool                           bFound = false;
  tsp9_rte_xerror                xError;
  void                         * pHandle;
  tsp09_RteInstallationInfoNew   relInfo;

  // open enum
  sqlxopen_installation_enum (&pHandle, &xError);

  if (xError.xe_result == csp9_xrte_ok) {
    // iterate trough installations
    while (xError.xe_result == csp9_xrte_ok && !bFound) {
      relInfo.RecordVersion = INSTALLATION_RECORD_VERSION_V721;
      relInfo.RecordLength  = sizeof(relInfo);
      sqlxnext_installationNew (pHandle, relInfo, &xError);
      if (xError.xe_result == csp9_xrte_ok) {
        bFound = ( strcmp ( relInfo.key,     relInfoIn.key                               ) == 0 &&
                   memcmp  ( &relInfo.version, &relInfoIn.version, sizeof(relInfo.version)) == 0 &&
#ifdef _WIN32
                   stricmp( relInfo.dbroot,  relInfoIn.dbroot                            ) != 0    );
#else
                   strcmp ( relInfo.dbroot,  relInfoIn.dbroot                            ) != 0    );
#endif
      } // end if
    } // end while

    // close enum
    sqlxclose_installation_enum(pHandle);
  } // end if

  return bFound;
} // end cn40_CheckInstKey

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40InstEnum
 * ------------------------------------------------------------------
 */
tcn00_Error cn40InstEnum
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error                    nFuncReturn = OK_CN00;
  tsp9_rte_xerror                xError;
  void                      *    pHandle;
  tsp09_RteInstallationInfoNew   relInfo;
  char                         * pCurrent    = replyData;
  char                           szMode[PARAMETER_MAXLEN_CN90];
  bool                           bKey = false;
  int                            nLength;

  if (cn90GetToken(command->args, szMode, 1, PARAMETER_MAXLEN_CN90)) {
    if (stricmp(szMode, OPTS_INST_KEY) == 0) {
      bKey = true;
    } else {
      nFuncReturn =  cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    // init message
    sp77sprintf(pCurrent, replyLenMax - (int)(pCurrent -replyData), "%s%s", ANSWER_OK_CN00,  LINE_SEPSTRING_CN00);
    pCurrent = pCurrent + strlen(pCurrent);

    // open enum
    sqlxopen_installation_enum (&pHandle, &xError);
    
    if (xError.xe_result == csp9_xrte_ok) {
      // iterate trough installations
      while (xError.xe_result == csp9_xrte_ok) {
        relInfo.RecordVersion = INSTALLATION_RECORD_VERSION_V721;
        relInfo.RecordLength  = sizeof(relInfo);
        sqlxnext_installationNew (pHandle, relInfo, &xError);
        if (xError.xe_result == csp9_xrte_ok) {
          sp77sprintf (pCurrent, replyLenMax - (int)(pCurrent -replyData), "%d.%d.%02d.%02d",
                     relInfo.version.no [0],
                     relInfo.version.no [1],
                     relInfo.version.no [2],
                     relInfo.version.no [3]);
          nLength  = (int)strlen(pCurrent);
          pCurrent = pCurrent + nLength;
          sp77sprintf (pCurrent, replyLenMax - (int)(pCurrent -replyData), "%*s", 13 - ((nLength >= 13) ? 12 : nLength), " ");
          pCurrent = pCurrent + strlen(pCurrent);

          if (bKey) {
            sp77sprintf (pCurrent, replyLenMax - (int)(pCurrent -replyData), "%-*s ",
                       18,
                       relInfo.key.asCharp());
            pCurrent = pCurrent + strlen(pCurrent);
          } // end if
          sp77sprintf (pCurrent, replyLenMax - (int)(pCurrent -replyData), "%s%s",
                     relInfo.dbroot.asCharp(),
                     LINE_SEPSTRING_CN00);
          pCurrent = pCurrent + strlen(pCurrent);
        } // end if

      } // end while

      sqlxclose_installation_enum(pHandle);

    } // end if

    if (xError.xe_result != csp9_xrte_ok && xError.xe_result != csp9_xrte_atend) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
    } else {
      *replyLen = (int)strlen(replyData);
    } // end if

  } // end if


  return nFuncReturn;
} /* end cn40InstEnum */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40InstReg
 * ------------------------------------------------------------------
 */
tcn00_Error cn40InstReg
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40InstReg");

  tcn00_Error                    nFuncReturn                      = OK_CN00;
  tsp09_RteInstallationInfoNew   relInfo;
  char                           szToken[PARAMETER_MAXLEN_CN90]   = "";
  char                           szVersion[PARAMETER_MAXLEN_CN90] = "";
  char                         * pToken;
  int                            nIndex;
  int                            nToken                           = 1;
  tsp9_rte_xerror                xError;
  tsp100_VersionID1              VersionID1;
  bool                           bClient = false;
  
  relInfo.key.Init();
  relInfo.dbroot.Init();

  // read parameters is Version or switch
  while (cn90GetToken(command->args, szToken, nToken, PARAMETER_MAXLEN_CN90)) {
    if (stricmp(szToken, OPTS_INST_VERSION) == 0) {
      ++nToken;
      if (!cn90GetToken(command->args, szVersion, nToken, PARAMETER_MAXLEN_CN90)) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } // end if
    } else if (stricmp(szToken, OPTS_INST_KEY) == 0) {
      ++nToken;
      if (!cn90GetToken(command->args, szToken, nToken, PARAMETER_MAXLEN_CN90)) {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } else {
        relInfo.key.rawAssign(szToken);
      } // end if
    } else if (stricmp(szToken, OPTS_INST_CLIENT) == 0) {
      bClient = true;
    } else {
      if (nToken == 1) {
        relInfo.dbroot.rawAssign(szToken);
      } else if (nToken == 2) {
        SAPDB_strcpy(szVersion , szToken);
      } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } // end if
    } // end if
 
    ++nToken;    
  } // end while

  // check dbroot
  if (nFuncReturn == OK_CN00) {

    if (relInfo.dbroot.length() == 0) {
      relInfo.dbroot = vcontrol->dbroot;
    } else {
      if (relInfo.dbroot[relInfo.dbroot.length() - 1] == PATH_DELIMITER_CN90) {
        relInfo.dbroot[relInfo.dbroot.length() - 1] = 0;
      } // end if
        
      if (
#ifndef _WIN32
        strcmp( relInfo.dbroot, vcontrol->dbroot )  != 0
#else
        stricmp(relInfo.dbroot, vcontrol->dbroot ) != 0
#endif
         ) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_WRONGINST,Msg_Arg(SDBMSGTAG_DBMSRV_WRONGINST__DBMSRV_COMP, vcontrol->dbroot.asCharp())));
      } // end if
    } // end if

  } // end if

  // checking parameters
  if (nFuncReturn == OK_CN00) {
    for (nIndex = 0; nIndex < csp9_version_digits; nIndex++) {
      relInfo.version.no [nIndex] = STATIC_CAST(signed char, 0);
    } // end for
    // reading verison
    if (strlen(szVersion) == 0) {
      sp100_GetVersionID ( VersionIDType1_esp100, s100buildnumberEx, &VersionID1 );

      relInfo.version.no [0] = STATIC_CAST(signed char, VersionID1.MajorVersion_sp100);
      relInfo.version.no [1] = STATIC_CAST(signed char, VersionID1.MinorVersion_sp100);
      relInfo.version.no [2] = STATIC_CAST(signed char, VersionID1.CorrLevel_sp100);
      relInfo.version.no [3] = STATIC_CAST(signed char, VersionID1.BuildNumberPrefix_sp100);
    } else {
      // Read Version from arguments
      pToken = strtok(szVersion, ".");
      nIndex = 0;
      while (pToken && nIndex < csp9_version_digits) {
        relInfo.version.no [nIndex] = STATIC_CAST(signed char, atoi(pToken));
        nIndex++;
        pToken = strtok(NULL, ".");
      } // end while
    } // end if

    // checking key
    if (cn40_CheckInstKey(relInfo)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_KEYEXISTS,Msg_Arg(SDBMSGTAG_DBMSRV_KEYEXISTS__DBMSRV_COMP,(relInfo.key.length() == 0) ? "(empty)" : relInfo.key.asCharp())));
    } // end if
  } // end if

  // register version
  if (nFuncReturn == OK_CN00) {
    relInfo.RecordVersion = INSTALLATION_RECORD_VERSION_V721;
    relInfo.RecordLength  = sizeof(relInfo);

    // remove trailing (back)slashes from inst path
    nIndex = relInfo.dbroot.length() - 1;
    while (nIndex > 0) {
      if ((relInfo.dbroot[nIndex] != CHAR_SLASH) && (relInfo.dbroot[nIndex] != CHAR_BACKSLASH)) {
        relInfo.dbroot[nIndex + 1] = CHAR_STRINGTERM_CN90;
        break;
      } // end if
      --nIndex;
    } // end while

    sqlxregisterinstNew (relInfo, &xError);

    if (xError.xe_result != csp9_xrte_ok) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
    } else {
      if (!bClient) {
        if (!cn40_RegisterServiceDB(relInfo)) {
          nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_SERVICEDB,Msg_Arg(SDBMSGTAG_DBMSRV_SERVICEDB__DBMSRV_COMP,relInfo.dbroot.asCharp())));
        } else {
          cn90AnswerOK (replyData, replyLen, NULL);
        } // end if
      } else {
        cn90AnswerOK (replyData, replyLen, NULL);
      } // end if
    } // end if

  } // end if

  return nFuncReturn;

} /* end cn40InstReg */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40InstUnreg
 * ------------------------------------------------------------------
 */
tcn00_Error cn40InstUnreg
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("cn40InstUnreg");

  tcn00_Error          nFuncReturn = OK_CN00;
  char                 szToken[PARAMETER_MAXLEN_CN90];
  tsp00_Pathc          szDbRoot;
  tsp00_DbNamec        szDbName;
  tsp9_rte_xerror      xError;

  cn90GetToken(command->args, szToken, 1, PARAMETER_MAXLEN_CN90);
  cn90StripQuotes(szToken);
  szDbRoot.rawAssign(szToken);

  // check dbroot

  if (szDbRoot.length() == 0) {
    szDbRoot = vcontrol->dbroot;
  } // end if

  if (szDbRoot[szDbRoot.length()  - 1] == PATH_DELIMITER_CN90) {
    szDbRoot[szDbRoot.length() - 1] = 0;
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90ServiceDB(szDbName, szDbRoot);
    if (szDbName.length() > 0) {
      
      // we must do this to delete the shm files of the service db...
      DBMSrvMsg_Error shmInitError;
      DBMSrv_Reply reply (replyData, replyLen, replyLenMax);
      if( !DBMSrvShM_ShmAdmin::TheAdmin().initialize(szDbName.asCharp(), shmInitError) )
        return reply.startWithMessageList(shmInitError);

      DBMSrvCmdDb_DbDrop dropCommand;
      dropCommand.doDrop(szDbName, szDbRoot, true, true, reply, true);
    } // end if
  
    sqlxunregisterinst (szDbRoot, &xError);

    if (xError.xe_result != csp9_xrte_ok) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
    } else {
      cn90AnswerOK (replyData, replyLen, NULL);
    } /* end if */

  } // end if

  return nFuncReturn;

} /* end cn40InstUnreg */

/*
 * IMPLEMENTATION PRIVATE FUNCTION cn40_ComputeEsc
 * ------------------------------------------------------------------
 */
#define ESC_MARK '#'

static void cn40_ComputeEsc
      ( tsp00_Pathc & szFile )
{
  tsp00_Int4 nLength = 0;
  tsp00_Int4 nSource = 0;
  tsp00_Int4 nTarget = 0;
  unsigned char nDig1   = 0;
  unsigned char nDig2   = 0;
  unsigned char nChar   = 0;

  nLength = szFile.length();
  while (nSource < nLength) {

    nDig1   = 0;
    nDig2   = 0;
    nChar   = 0;

    if (szFile[nSource] == ESC_MARK) {
      ++nSource;
      nDig1 = szFile[nSource];
      if (nDig1 == ESC_MARK) {
        nChar = nDig1;
      } else {
        ++nSource;
        nDig2 = szFile[nSource];

        // Konverting to upper
        nDig1 = toupper(nDig1);
        nDig2 = toupper(nDig2);
        // checking hexdigit
        nDig1 = (nDig1 >= '0' && nDig1 <= '9' ||
                 nDig1 >= 'A' && nDig1 <= 'F'    ) ? nDig1 : 0;
        nDig2 = (nDig2 >= '0' && nDig2 <= '9' ||
                 nDig2 >= 'A' && nDig2 <= 'F'    ) ? nDig2 : 0;
        // checking second digit
        nDig1 = (nDig2 == 0) ? 0 : nDig1;
        nDig2 = (nDig1 == 0) ? 0 : nDig2;

        if (nDig1 > 0 && nDig2 > 0) {
          nDig1 = (nDig1 <= '9') ? nDig1 - 0x30 : nDig1 - 0x41;
          nDig2 = (nDig2 <= '9') ? nDig2 - 0x30 : nDig2 - 0x41;

          nChar = (nDig1 * 0x10) + nDig2;
        } else {
          --nSource;
          --nSource;
          nChar = ESC_MARK;
        } // end if

      } // end if
    } else {
      nChar = szFile[nSource];
    } // end if

    if (nChar != 0) {
      szFile[nTarget] = nChar;
      ++nTarget;
    } // end if

    ++nSource;
  } // end while

   szFile[nTarget] = 0;

} // end cn40_ComputeEsc

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40DbAddDevice
 * ------------------------------------------------------------------
 */
tcn00_Error cn40DbAddDevice
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
    tcn00_Error            nFuncReturn    = OK_CN00;
    char                   szMode  [PARAMETER_MAXLEN_CN90];
    char                   szType  [PARAMETER_MAXLEN_CN90];
    char                   szMType [PARAMETER_MAXLEN_CN90];
    char                   szSize  [PARAMETER_MAXLEN_CN90];
    tsp00_Pathc            szName;
    tsp00_Pathc            szMName;
    DBMSrvKnl_State::StateValue aState;
    char                   szCommand [PARAMETER_MAXLEN_CN90];
    int                    nNumber;
    tcn00_DevspaceKeywords aKeyTable  [] = XP_DEVSPACE_KEYWORDS_CN00;
    tcn00_DevspaceParams   aParamTable[] = XP_DEVSPACE_PARAMS_CN00;
    tsp00_Int4             nKey          = 0;
    tsp00_Int4             nParam        = 0;
    tsp00_Int4             mirrKey       = 0;
    tsp00_Int4             mirrParam     = 0;
    tsp00_Int4             nSize         = 0;
    long                   nReturnSize   = 0;
    tcn002_XpValueName     szParam;
    tcn002_XpValueString   szValue;
    bool                   bParamWritten(false);
    bool                   bParamMirrorWritten=false;
    DBMSrv_Reply           myReply(replyData, replyLen, replyLenMax);
    char                   szLastVolName[RTECONF_MAXSTRINGLENGTH+1];
    char                   szLastMVolName[RTECONF_MAXSTRINGLENGTH+1];
    tsp00_Int4             nLastVolSize(0);
    tsp00_Int4             nAllVolSize=0;
    bool                   bHSS          = false;
    const char           * pArgs         = command->args;
    DBMSrvMsg_Error        oParamGetMsg;

    SAPDB_Int argCount(command->oArguments.getCount());
    bool bAuto(argCount==1 || argCount==3);

    // assign arguments
    cn90GetToken(command->args, szMode,   1, PARAMETER_MAXLEN_CN90);
    cn90GetToken(command->args, szName,   2, szName.size()+1);
    cn90StripQuotes(szName);
    cn90GetToken(command->args, szType,   3, PARAMETER_MAXLEN_CN90);
    cn90GetToken(command->args, szSize,   4, PARAMETER_MAXLEN_CN90);
    cn90GetToken(command->args, szMName,  5, szMName.size()+1);
    cn90StripQuotes(szMName);
    cn90GetToken(command->args, szMType,  6, PARAMETER_MAXLEN_CN90);

    // checking mode
    if (stricmp(szMode, XP_DEV_DAT_CN00) != 0 &&
        stricmp(szMode, XP_DEV_LOG_CN00) != 0    ) {
            return myReply.startWithError(ERR_PARAM_CN00);
    }

    // log volume mirrorred?
    szValue.Init();
    szParam.rawAssign(PAN_LOG_MIRRORED);
    if (!cn20XParamGetValue( vcontrol->dbname, szParam, szValue, oParamGetMsg)) {
      return myReply.startWithMessageList(oParamGetMsg);
    } // end if
    bool bMirror(stricmp(szMode, XP_DEV_LOG_CN00) == 0 &&
                 (stricmp(szValue, XP_VAL_DUAL_CN00) == 0 || 
                  stricmp(szValue, XP_VAL_YES_CN00) == 0)
                );

    // mirroring, no automatic volume addition and no mirror volume name
    if( bMirror && !bAuto && szMName.length() == 0)
        return myReply.startWithError(ERR_MISSMIRR_CN00);

    // checking type
    if (stricmp(szType, "A") == 0 ) {
        return myReply.startWithError(ERR_PARAM_CN00);
    } // end if

    // checking dbstate
    aState = DBMSrvKnl_State::State(vcontrol->dbname);
    if (aState != DBMSrvKnl_State::StateAdmin && aState != DBMSrvKnl_State::StateOnline && aState != DBMSrvKnl_State::StateStandby) {
        return myReply.startWithError(ERR_DBNORUN_CN00);
    }

    // checking hss 
    if (nFuncReturn == OK_CN00) {
      bHSS = vcontrol->pHSSNodes->Exists(RTE_ISystem::Instance().GetLocalNodeName()) && cn46IsMaster(vcontrol->dbname);
    } // end if

    // find number of next volume for the requested mode
    // find key for parameter table
    for (nKey = 0; stricmp(aKeyTable[nKey].szText, szMode) != 0; nKey++);

    if (aKeyTable[nKey].nDevSpace == XPDevSpaceUnknown_ecn00)
        return myReply.startWithError(ERR_PARAM_CN00);

    // search in paramtable with that key
    for (nParam = 0; aKeyTable[nKey].nDevSpace != aParamTable[nParam].nDevSpace; nParam++);

    if( bMirror ) {
        for (mirrKey = 0; !aKeyTable[mirrKey].bMirror; mirrKey++);
        for (mirrParam = 0; aKeyTable[mirrKey].nDevSpace != aParamTable[mirrParam].nDevSpace; mirrParam++);
    }

    // get maximal number of volumes
    tsp00_Int4 maxVolumeCount=0;
    szParam.rawAssign(aParamTable[nParam].szMaxCount);
    if (!cn20XParamGetValue(vcontrol->dbname, szParam, maxVolumeCount, oParamGetMsg)) {
      return myReply.startWithMessageList(oParamGetMsg);
    } // end if

    nNumber=0;
    
    for(tsp00_Int4 j=1; j<=maxVolumeCount; j++)
    {
        sp77sprintf(szParam, szParam.size(), aParamTable[nParam].szName, j);
        if (!cn20XParamGetValue( vcontrol->dbname, szParam, szValue, oParamGetMsg)) {
            if (oParamGetMsg == DBMSrvMsg_Error::DBMSrv_XPNOTFOUND) 
                oParamGetMsg.ClearMessageList();
            else
                return myReply.startWithMessageList(oParamGetMsg);
        } // end if

        if(0<strlen(szValue.asCharp()))
        {
            if( bAuto )
            {
                // sum up all sizes for potential calculation of new volume size
                sp77sprintf(szParam, szParam.size(), aParamTable[nParam].szSize, j);
                if (!cn20XParamGetValue(vcontrol->dbname, szParam, nLastVolSize, oParamGetMsg)) {
                     return myReply.startWithMessageList(oParamGetMsg);
                } // end if
                
                nAllVolSize += nLastVolSize;
            }
        }
        else
        {   //store index of first empty place in nNumber
            if(0==nNumber)
                nNumber=j;
        }
    }

    if(0==nNumber) // number still 0 means, that there was no free parameter entry for the new volume 
    {
        szParam.rawAssign(aParamTable[nParam].szMaxCount);

        return myReply.startWithMessageList(
            DBMSrvMsg_Error(SDBMSG_DBMSRV_MAXVOLCNT,
                            Msg_Arg(SDBMSGTAG_DBMSRV_MAXVOLCNT__VOLUME_TYPE, szMode),
	                        Msg_Arg(SDBMSGTAG_DBMSRV_MAXVOLCNT__VOLUME_MAXCOUNT, maxVolumeCount),
	                        Msg_Arg(SDBMSGTAG_DBMSRV_MAXVOLCNT__PARAMETER, (const char*)szParam)));
    }

    // scan names for escape sequences
    cn40_ComputeEsc(szName);
    if( bMirror )
        cn40_ComputeEsc(szMName);

    if( bAuto )
    {
        // automatic name calculation necessary
        DBMSrvMsg_Error error;
        char * newName=0;
        char * newNameMirror=0;
        DBMSrvVol_VolumeSize newSize;
        DBMSrvVol_Volume::ContentType contentType;

        DBMSrvVol_Volume::getContentType(szMode, contentType);

        bool rc=DBMSrvVol_Volumes::calculateNameAndSizeOfNewVolume(
                    nNumber,
                    contentType,
                    bMirror,
                    nAllVolSize,
                    newName,
                    newNameMirror,
                    newSize,
                    error);

		if(!rc)
			return myReply.startWithMessageList(error);

        // set type of new volume
        sprintf(szType, "%s", XP_VAL_F_CN00);

        // set name of new volume
        SAPDB_strcpy(szLastVolName, newName);
        
        // set size of new volume
        nLastVolSize=newSize.getSize();

        // set name and type of mirror volume if necessary
        if( bMirror )
        {
            sprintf(szMType, "%s", XP_VAL_F_CN00);
            SAPDB_strcpy(szLastMVolName, newNameMirror);
        }

        //free memory
        cn36_StrDealloc(newName);
        cn36_StrDealloc(newNameMirror);
    } else {
        SAPDB_strcpy(szLastVolName, szName.asCharp());
        nLastVolSize = atol(szSize);
        if( bMirror )
            SAPDB_strcpy(szLastMVolName, szMName.asCharp());
    }

    // now we have it all: nLastVolSize, szLastVolName (and szLastVolMName)

    // add params to kernel parameter file
    if (nFuncReturn == OK_CN00) {
      sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, "%d %s \"%s\" %s %d",
            nNumber,
            szMode,
            szLastVolName,
            szType,
            (int) nLastVolSize);
        command->args = szCommand;
        nFuncReturn = cn20ParamAddDevSpace(vcontrol,
            command,
            replyData,
            replyLen,
            replyLenMax,
            nReturnSize);
        myReply.adaptLength();
        if (nFuncReturn == OK_CN00) {
            bParamWritten = true;
            nLastVolSize = (nReturnSize != 0) ? nReturnSize : nLastVolSize;
        } // end if

        if (bMirror && nFuncReturn == OK_CN00) {
            sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, "%d M%s \"%s\" %s",
                nNumber,
                szMode,
                szLastMVolName,
                szMType);
            command->args = szCommand;
            nFuncReturn = cn20ParamAddDevSpace(vcontrol,
                command,
                replyData,
                replyLen,
                replyLenMax);
            myReply.adaptLength();

            if(OK_CN00==nFuncReturn)
                bParamMirrorWritten=true;
        } // end if
    } // end if

    // build kernel command
    if (nFuncReturn == OK_CN00) {
        if (!bMirror) {
            sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, "ADD %s VOLUME '%s' PAGES %d DEVICE %d",
                szMode,
                szLastVolName,
                (int) nLastVolSize,
                nNumber);
        } else {
            sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, "ADD %s VOLUME '%s' '%s' PAGES %d DEVICE %d",
                szMode,
                szLastVolName,
                szLastMVolName,
                (int) nLastVolSize,
                nNumber);
        } // end if

        // build execute kernel command
        cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
        DBMSrvMsg_Error oError;
        if (!DBMSrvKnl_Session::ExecuteWithNoTempFiles (vcontrol->dbname.asCharp(),
                                                        oUser.getUserName().asCharp(),
                                                        oUser.getClearMasterPwd().asCharp(),
                                                        szCommand,
                                                        oError)) {
            nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
            myReply.adaptLength();
        } // end if
    } // end if

    // "restore" old volume parameters, if needed
    if(OK_CN00!=nFuncReturn && bParamWritten)
    {
        CommandT newCommand;

        sprintf(szCommand, "%d %s", (int)nNumber, (const char *)szMode);

        newCommand.nIndex=CommandT::InvalidIndex;
        newCommand.setArgs(szCommand);
        
        DBMSrv_Reply dummyReply;

        //remove parameters for that volume, ignore errors and error output
        cn20ParamDelDevSpace(vcontrol,
                             &newCommand, // must do this , because it's changed...
                             dummyReply.giveData(),
                             dummyReply.giveLength(),
                             dummyReply.giveMaximalLength());
    }

    // "restore" old volume parameters of mirror, if needed
    if(OK_CN00!=nFuncReturn && bParamMirrorWritten)
    {
        CommandT newCommand;

        sprintf(szCommand, "%d M%s", (int)nNumber, (const char *)szMode);

        newCommand.nIndex=CommandT::InvalidIndex;
        newCommand.setArgs(szCommand);
        
        DBMSrv_Reply dummyReply;

        //remove parameters for that volume, ignore errors and error output
        cn20ParamDelDevSpace(vcontrol,
                             &newCommand, // must do this , because it's changed...
                             dummyReply.giveData(),
                             dummyReply.giveLength(),
                             dummyReply.giveMaximalLength());
    }

    if (nFuncReturn == OK_CN00) {
        myReply.startWithOK();
    } // end if

    // hss handling
    if ((nFuncReturn == OK_CN00) && bHSS) {
        DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
        reply.adaptLength(); //just to be sure we append correctly
        vcontrol->pHSSNodes->AddVolumeToCluster(pArgs, reply);
    } // end if

    return nFuncReturn;
} /* end cn40DbAddDevice */

/*
 * IMPLEMENTATION PUBLIC FUNCTION cn40Test
 * ------------------------------------------------------------------
 */
tcn00_Error cn40Test
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
//  tsp00_Namec szUser = cn50DBMUser::getColdUser(vcontrol->dbname);
 
  Msg_List   oEvent;
  oEvent.ClearMessageList();
  RTE_UserProfileContainer  oProfile;
  oProfile.Open(vcontrol->dbname, true, oEvent);
  tsp00_C18c              szUserName;
  SAPDB_UInt4             nToken = FIRST_TOKEN;
  char                    oBuffer[1000];
  SAPDB_Int4              nLength = 1000;

  oProfile.ScanProfiles(nToken, nLength, &oBuffer, szUserName, oEvent);

  return cn90AnswerOK (replyData, replyLen, NULL);
} /* end cn40Test */

/* ------------------------------------------------------------------
 * START OF IMPLEMENTATION PRIVATE
 * ------------------------------------------------------------------
 */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40ExecCommand
 * ------------------------------------------------------------------
 */
tcn00_Error cn40ExecCommand
      ( char          * replyData,
        int           * replyLen,
        const int       replyLenMax,
        const char    * szProgram,
        const char    * szOutput,
        bool            bPipe,
        bool            bSynchron )
{
  tcn00_Error     nFuncReturn = OK_CN00;
  int             nSysReturn  = 0;
  char            szTmpFile[PARAMETER_MAXLEN_CN90] = "";
  char            szCommand[PARAMETER_MAXLEN_CN90] = "";

  const char * pTempFile = ToolsSys_tmpname();
  if (pTempFile != NULL) {
    SAPDB_strcpy(szTmpFile, pTempFile);
  } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_EXECUTE_CN00, 1, "can not generate temporary file name");
  } // end if 

  /* execute command */
  if (nFuncReturn == OK_CN00) {
    if (strlen(szProgram) + strlen(szTmpFile) + 10 < PARAMETER_MAXLEN_CN90) {

      if (bPipe) {
        nSysReturn = Tools_PipeCall::CallProgram(szProgram, bSynchron ? Tools_PipeCall::CallSynchron : Tools_PipeCall::CallAsynchron, szTmpFile, szTmpFile);
      } else {
        sp77sprintf(szCommand, PARAMETER_MAXLEN_CN90, "%s 1>%s 2>&1", szProgram, szTmpFile);
        nSysReturn = system(cn90Strip(szCommand));
      } // end if

      if (nSysReturn == SYS_SYSTEM_ERR) {
        if (bPipe) {
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_SYSTEM_CN00, nSysReturn, Tools_PipeCall::ErrorReason());
        } else {
          nFuncReturn = cn90AnswerErrnoError(replyData, replyLen, ERR_SYSTEM_CN00, errno, szCommand);
        } // end if
      } else if (nSysReturn != 0) {
        nFuncReturn = cn90AnswerExecute(replyData, replyLen, replyLenMax, ERR_EXECUTE_CN00, nSysReturn, szOutput, szTmpFile);
      } /* end if */

    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
    } /* end if */
  } /* end if */

  /* check result */
  if (nFuncReturn == OK_CN00) {
    cn90AnswerExecute(replyData, replyLen, replyLenMax, nFuncReturn, nSysReturn, szOutput, szTmpFile);
  } /* end if */

  /* remove temporary files */
  remove(szTmpFile);

  return nFuncReturn;
} /* end cn40ExecCommand */

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_ExecPythonLoad
 * ------------------------------------------------------------------
 */
static tcn00_Error cn40_ExecPythonLoad
      ( VControlDataT * vcontrol,
        const char    * szLoadFile,
        const char    * szUser,
        const char    * szPassword,
        const char    * szOptions,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error     nFuncReturn = OK_CN00;
  int             nIndex      = 0;
  FILE        *   hFile       = NULL;
  char            szCommand   [PARAMETER_MAXLEN_CN90] = "";
  char            szProtocol  [PARAMETER_MAXLEN_CN90] = "";
  char            szLine      [PARAMETER_MAXLEN_CN90] = "";
  tsp01_RteError  aRteError;
  tsp00_Pathc     szBinDir;
  tsp00_Pathc     szEnvDir;
  tsp00_Pathc     szScript;
  tsp00_Pathc     szProt;

  /* create command filename */
  szScript.rawAssign(szLoadFile);

  if (nFuncReturn == OK_CN00) {
    // remove backslashes
    for (nIndex = 0; nIndex < szScript.length(); nIndex++) {
      szScript[nIndex] = (szScript[nIndex] == CHAR_BACKSLASH) ? CHAR_SLASH : szScript[nIndex];
    } /* end for */

    // if no absolute path for scriptfile use "env" directory
    szEnvDir.Init();
    if ( !(isalpha(szScript[0]) && szScript[1] == CHAR_DRIVE && szScript[2] == CHAR_SLASH) &&
         !(szScript[0] == CHAR_SLASH) ) {
      sqlGetDbrootEnvPath(szEnvDir, TERM_WITH_DELIMITER_EO01, &aRteError);
    } /* end if */

    // look for "bin" directory
    szBinDir.Init();
    // x_python is now in the depended bin path
    // sqlGetIndependentBinPath(szBinDir, TERM_WITH_DELIMITER_EO01, &aRteError);
    sqlGetDbrootBinPath(szBinDir, TERM_WITH_DELIMITER_EO01, &aRteError);
    
#ifdef _WIN32
    char Tueddelchen[2] = "\"";
#else
    char Tueddelchen[2] = "";
#endif

    sp77sprintf(szCommand,  PARAMETER_MAXLEN_CN90, "%s%s%sx_python%s %s%s%s%s -R %s%s%s -d %s -u %s,%s %s%s", 
                                                                           Tueddelchen,
                                                                           Tueddelchen,
                                                                           szBinDir.asCharp(),
                                                                           Tueddelchen,
                                                                           Tueddelchen,
                                                                           szEnvDir.asCharp(),
                                                                           szScript.asCharp(),
                                                                           Tueddelchen,
                                                                           Tueddelchen,
                                                                           vcontrol->dbroot.asCharp(),
                                                                           Tueddelchen,
                                                                           vcontrol->dbname.asCharp(),
                                                                           szUser,
                                                                           szPassword,
                                                                           szOptions,
                                                                           Tueddelchen);
    sp77sprintf(szProtocol, PARAMETER_MAXLEN_CN90, "%s%s%sx_python%s %s%s%s%s -R %s%s%s -d %s -u %s,%s%s", Tueddelchen,
                                                                           Tueddelchen,
                                                                           szBinDir.asCharp(),
                                                                           Tueddelchen,
                                                                           Tueddelchen,
                                                                           szEnvDir.asCharp(),
                                                                           szScript.asCharp(),
                                                                           Tueddelchen,
                                                                           Tueddelchen,
                                                                           vcontrol->dbroot.asCharp(),
                                                                           Tueddelchen,
                                                                           vcontrol->dbname.asCharp(),
                                                                           szUser,
                                                                           "*",
                                                                           Tueddelchen);
    // remove old protocol
    cn42GetFileName(vcontrol->dbname, FGET_REPMAN_CN42, szProt);
    remove(szProt);

    // ... and GO!
    nFuncReturn = cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szProtocol, false, true);

  } //

  // read protocol file
#ifdef ELCH
  if (nFuncReturn == OK_CN00 || nFuncReturn == ERR_EXECUTE_CN00) {
    hFile = NULL;
    hFile = fopen(szProt, "r");

    if (hFile) {
      while (fgets(szLine, PARAMETER_MAXLEN_CN90, hFile) != NULL && ((int) (strlen(replyData) + strlen(szLine) + 5) < replyLenMax)) {
        if (cn90CheckKeyword(szLine, "/ M") ||
            cn90CheckKeyword(szLine, "/ E")    ) {
          strcat(replyData, cn90StripTrailingChar(szLine, '\n'));
          strcat(replyData, LINE_SEPSTRING_CN00);
        } /* end if */
      } /* end while */
      fclose(hFile);
      *replyLen = strlen(replyData);
    } /* end if */
  } /* end if */
#endif

  return nFuncReturn;
} /* end cn40_ExecPythonLoad */

#define KnlSetenv_CN40   "KnlSetenv"
#define KnlUnsetenv_CN40 "KnlUnsetenv"


/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_PrepareEnvironment
 * ------------------------------------------------------------------
 */
static tcn00_Error cn40_PrepareEnvironment
  ( const char   * szDBName,
    DBMSrv_Reply & reply)
{
  tcn00_Error nFuncReturn = OK_CN00;

  size_t LengthOfSetPrefix   = strlen(KnlSetenv_CN40);
  size_t LengthOfUnsetPrefix = strlen(KnlUnsetenv_CN40);
  int    cn36rc=1;
  
  cn51DBMConfigValueIterator oIterator(szDBName);
  while (!oIterator.IsEnd() && (nFuncReturn == OK_CN00)) {
    if (strnicmp(oIterator.GetCurrentName(), KnlSetenv_CN40, LengthOfSetPrefix) == 0) {
      if (!cn36_PutEnv(oIterator.GetCurrentValueAsString())) {
        nFuncReturn = reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_SETENV, Msg_Arg(SDBMSGTAG_DBMSRV_SETENV__VALUE,oIterator.GetCurrentValueAsString())));
      } // end if
    } else if (strncmp(oIterator.GetCurrentName(), KnlUnsetenv_CN40, LengthOfUnsetPrefix) == 0) {
      cn36_UnsetEnv(oIterator.GetCurrentValueAsString());
    } // end if
    oIterator.Next();
  } // end while

  return nFuncReturn;
} // end cn40_PrepareEnvironment


/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_OfflineToCold
 * ------------------------------------------------------------------
 */
static tcn00_Error cn40_OfflineToCold
      ( VControlDataT       * vcontrol,
        const tsp9_pgm_kind   pgmKind,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp9_rte_xerror     xError;
  tcn002_XpValueString paramValueRunDirectory;
  tsp00_DbNamec       szDBNameL = vcontrol->dbname;

  // always try to create rundir
  DBMSrvMsg_Error oParamGetMsg;
  if (cn20XParamGetValue(szDBNameL, PAN_RUNDIR, paramValueRunDirectory, oParamGetMsg)) {
    cn90DirCreate(paramValueRunDirectory, NULL);
  } // end if

  // start event dispatcher if necessary
  DBMSrv_Reply reply(replyData, replyLen, replyLenMax);

  if (nFuncReturn == OK_CN00) {
    cn20CalcDevices( vcontrol, replyData, replyLen, replyLenMax);

    // prepare environment for database
    nFuncReturn = cn40_PrepareEnvironment(vcontrol->dbname, reply);
  } // end if

  if (nFuncReturn == OK_CN00) {
    // clear database
    sqlxclear( vcontrol->dbname, vcontrol->dbroot, &xError );

    // start database
    sqlxstart (vcontrol->dbname, pgmKind, vcontrol->dbroot, false, 0, NULL, &xError);

    if (xError.xe_result != csp9_xrte_ok) {
      // check Versions
      nFuncReturn = cn90AnswerIError(replyData, replyLen, cn40_CheckVersion(vcontrol->dbname));
      if (nFuncReturn == OK_CN00) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
      } // end if
    } else {
      nFuncReturn = cn90AnswerOK (replyData, replyLen, NULL);
    } /* end if */
  } /* end if */

  return nFuncReturn;
} // cn40_OfflineToCold

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_ColdOrStandbyToWarm
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn40_ColdOrStandbyToWarm"
static tcn00_Error cn40_ColdOrStandbyToWarm
      ( VControlDataT       * vcontrol,
        const char          * pRestart,
        char                * replyData,
        int                 * replyLen,
        int                 replyLenMax)
{
  CONSOLE_TRACE_ENTER_CN00;
  
  tcn00_Error nFuncReturn = OK_CN00;

  DBMSrvMsg_Error errOut;
  DBMSrvKnl_GlobalSessions::ReleaseAll(errOut, true);
  errOut.ClearMessageList(); // sessions may invalid meanwhile so we can ignore the error


  // volume migration
  if (nFuncReturn == OK_CN00) {
    cn51DBMConfigValue oMigrVol( vcontrol->dbname, CFG_CFGMIGRVOL_CN51 );
    cn51DBMConfigValue oMigrDB( vcontrol->dbname, CFG_CFGMIGRDB_CN51 );
    if (oMigrDB == 1) {
      // execute kernel command
      cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
      DBMSrvMsg_Error oError;
      if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                       oUser.getUserName().asCharp(),
                                       oUser.getClearMasterPwd().asCharp(),
                                       DB_MIGRATE_DATABASE,
                                       oError)) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
      if (nFuncReturn == OK_CN00) {
        oMigrVol = "";
        oMigrDB  = "";
      } // end if
    } else if (oMigrVol == 1) {
      // execute kernel command
      cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
      DBMSrvMsg_Error oError;
      if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                       oUser.getUserName().asCharp(),
                                       oUser.getClearMasterPwd().asCharp(),
                                       DB_MIGRATE_VOLUME,
                                       oError)) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
      if (nFuncReturn == OK_CN00) {
        oMigrVol = "";
        oMigrDB  = "";
      } // end if
    } // end if
  } // end if

  // execute kernel command
  cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
  DBMSrvMsg_Error oError;
  if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                    oUser.getUserName().asCharp(),
                                    oUser.getClearMasterPwd().asCharp(),
                                    pRestart,
                                    oError)) {
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } // end if

    // check SYSUPDATECOUNTERWANTED and execute check table if necessary
    if (nFuncReturn == OK_CN00) {
        cn50DBMUser     oSYSDBA(vcontrol->dbname, cn50DBMUser::getSYSDBA(vcontrol->dbname));

        // do not check for errors because of this is a secondary action of the db_online
        DBMSrvMsg_Error oErrorToIgnore;

        if (DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                        oSYSDBA.getUserName().asCharp(),
                                        oSYSDBA.getClearMasterPwd().asCharp(),
                                        "select * from SYSUPDATECOUNTERWANTED",
                                        oErrorToIgnore)) {
            // yeah, there are counters to update

            // build command line
            DBMSrvProc_ArgumentList theArgList;
            char upwdBuffer[256];

            theArgList.AddArgument("-d");
            theArgList.AddArgument(vcontrol->dbname.asCharp());
            theArgList.AddArgument("-u");
            sprintf(upwdBuffer, "%s,%s", oUser.getUserName().asCharp(), oUser.getClearMasterPwd().asCharp());
            theArgList.AddArgument(upwdBuffer);
            theArgList.AddArgument("db_execute Check Table As Per System Table");

            DBMSrvProc_ServerProcess oProcess( DBMSrvProc_ServerProcessCommandLineConverter::KeyDbmcli,
                                               theArgList );

//            do not check for errors because of this is a secondary action of the db_online
//            if(!oProcess.started() ) {
//                Msg_List aMsgList;
//                oProcess.getMessagesInto( aMsgList );
//                aMsgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EXECUTE));
//                nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aMsgList);
//            } // end if

        } // end if

    } // end if

  if (nFuncReturn == OK_CN00) {
    cn51DBMConfigValue  cfgAuto    (vcontrol->dbname, CFG_AUTOSSAVE_CN51);

    if (cfgAuto == 1) {
      cn31AutosaveOn(vcontrol, "", replyData, replyLen, replyLenMax);
    } // end if

  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // cn40_ColdOrStandbyToWarm

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_OnlineToOffline
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn40_OnlineToOffline"
static tcn00_Error cn40_OnlineToOffline
      ( VControlDataT       * vcontrol,
        bool                  bCluster,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax)
{
  CONSOLE_TRACE_ENTER_CN00;

  tcn00_Error     nFuncReturn = OK_CN00;

  cn51DBMConfigValue  cfgAuto    (vcontrol->dbname, CFG_AUTOSSAVE_CN51);
  if (cfgAuto == 1) {
    cn31AutosaveEnd(vcontrol, replyData, replyLen, replyLenMax);
  } // end if

  // release all global sessions
  DBMSrvMsg_Error errOut;
  DBMSrvKnl_GlobalSessions::ReleaseAll(errOut, true);
  if( !errOut.IsEmpty() )
      return DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(errOut);

#  if defined CLUSTER_SERVER_VERSION
  if (bCluster) {
    RTE_DBName dbname;
      
    memset(&dbname[0], 0, sizeof(RTE_DBName));
    if ( vcontrol->dbname.length() < sizeof(RTE_DBName) ) {
        SAPDB_strcpy(&dbname[0], vcontrol->dbname.asCharp());

        RTE_ActiveDatabase * dbInstance;

        Msg_List errList;

        dbInstance = RTE_DBRegister::Instance().GetActiveDatabase(dbname, errList);
        if ( !dbInstance ) {
            nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(errList));
        } else if ( !dbInstance->Stop(errList, 90, false, true) ) /* Stop without dump but gracefully */ {
            if ( errList.ID() == RTEERR_DBREG_SERVERDB_NOT_RESPONDING ) {
                nFuncReturn = DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_SHUTDOWN);
            } else {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(errList));
            } // end if
        } else {
            return nFuncReturn;
        } // end if
    } else {
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBNAME_CN00);
    } // end if
  } else {
    // execute kernel command
    cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    DBMSrvMsg_Error oError;
    if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                    oUser.getUserName().asCharp(),
                                    oUser.getClearMasterPwd().asCharp(),
                                    DB_SHUTDOWN,
                                    oError)) {
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if
#else
  // execute kernel command
  cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
  DBMSrvMsg_Error oError;
  if (!DBMSrvKnl_Session::Execute (vcontrol->dbname.asCharp(),
                                   oUser.getUserName().asCharp(),
                                   oUser.getClearMasterPwd().asCharp(),
                                   DB_SHUTDOWN,
                                   oError)) {
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } // end if
#endif

  DBMSrvKnl_State::StateValue DBState = DBMSrvKnl_State::StateOffline;

  if (nFuncReturn == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_SHUTDOWN)) {
    int nTimeout = 0;
    do {
      ToolsSys_sleep(1);
      ++nTimeout;
      DBState = DBMSrvKnl_State::State(vcontrol->dbname);
    } while ( (DBState != DBMSrvKnl_State::StateOffline) || (nTimeout == 300) );

    if (DBState != DBMSrvKnl_State::StateOffline) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_DBRUN_CN00);
    } // end if
  } else if (nFuncReturn == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_DATAFULL)) {
    tsp9_rte_xerror     xError;
    sqlxstop (vcontrol->dbname, vcontrol->dbroot, false, &xError);
    DBState = DBMSrvKnl_State::State(vcontrol->dbname);
    if ((xError.xe_result != csp9_xrte_ok) &&  (DBState != DBMSrvKnl_State::StateOffline)) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
    } /* end if */
  } // end if

  nFuncReturn = ((nFuncReturn == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_SHUTDOWN))||
                 (nFuncReturn == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_DATAFULL))) ? OK_CN00 : nFuncReturn;

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // cn40_OnlineToOffline

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_ColdOrStandbyToOffline
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn40_ColdOrStandbyToOffline"
static tcn00_Error cn40_ColdOrStandbyToOffline
      ( VControlDataT       * vcontrol,
        const bool            bDump,
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax)
{
  CONSOLE_TRACE_ENTER_CN00;

  tcn00_Error         nFuncReturn = OK_CN00;
  tsp9_rte_xerror     xError;

  // release all global sessions
  DBMSrvMsg_Error errOut;
  DBMSrvKnl_GlobalSessions::ReleaseAll(errOut, true);
  if( !errOut.IsEmpty() )
      return DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(errOut);

  sqlxstop (vcontrol->dbname, vcontrol->dbroot, bDump, &xError);

  DBMSrvKnl_State::StateValue dbState(DBMSrvKnl_State::State(vcontrol->dbname));

  if ((xError.xe_result != csp9_xrte_ok) &&  (dbState != DBMSrvKnl_State::StateOffline)) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(xError));
  } else {
    cn90AnswerOK (replyData, replyLen, NULL);
  } /* end if */

  return nFuncReturn;
} // cn40_ColdOrStandbyToOffline

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_CheckVersion
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn40_CheckVersion"
static tcn00_Error cn40_CheckVersion
      ( const tsp00_DbNamec   szDBName)
{
  CONSOLE_TRACE_ENTER_CN00;

  tcn00_Error     nFuncReturn = OK_CN00;
  tsp00_Versionc  szMyVersion;
  tcn002_XpValueString szParamVersion;
  tsp00_DbNamec   szLocDBName = szDBName;
  DBMSrvMsg_Error oParamGetMsg;

  if (cn20XParamGetValue(szLocDBName, PAN_PARAM_KERNELVERSION, szParamVersion, oParamGetMsg)) {
    sp100_GetVersionString(COMP_NAME_KERNEL_SP100, s100buildnumber, szMyVersion);

    if ((strncmp ( szParamVersion + BEG_OF_1ST_VERS_SUBSTR_SP100,
                    szMyVersion    + BEG_OF_1ST_VERS_SUBSTR_SP100,
                    END_OF_1ST_VERS_SUBSTR_SP100  - BEG_OF_1ST_VERS_SUBSTR_SP100   + 1 ) != 0 ) ||
        (strncmp ( szParamVersion + BEG_OF_2CND_VERS_SUBSTR_SP100,
                    szMyVersion    + BEG_OF_2CND_VERS_SUBSTR_SP100,
                    END_OF_2CND_VERS_SUBSTR_SP100 - BEG_OF_2CND_VERS_SUBSTR_SP100  + 1 ) != 0 )     ) {
      nFuncReturn = ERR_VERSION_CN00;
    } // end if
  } // end if

  return nFuncReturn;
} // cn40_CheckVersion

/* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_StartStopOptions
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn40_StartStopOptions"
static tcn00_Error cn40_StartStopOptions
      ( const char    * szParameters,
        tsp9_pgm_kind & pgmKind,
        bool          & bDump,
        char          * pRestart,
        bool          & bCluster )
{
  CONSOLE_TRACE_ENTER_CN00;

  tcn00_Error     nFuncReturn = OK_CN00;
  char            szToken   [PARAMETER_MAXLEN_CN90];
  char            szToken1  [PARAMETER_MAXLEN_CN90];
  int             nToken = 1;

  // init Options
  pgmKind    = csp9_any_pgm;
  bDump      = false;
  bCluster   = true;
  SAPDB_strcpy(pRestart, TXT_RESTART);

  while (cn90GetToken(szParameters, szToken, nToken, PARAMETER_MAXLEN_CN90) && nFuncReturn == OK_CN00) {

    if (stricmp(szToken, OPT_SPEED_FAST) == 0 || stricmp(szToken, OPTS_SPEED_FAST) == 0) {
      pgmKind = csp9_fast_pgm;
    } else if (stricmp(szToken, OPT_SPEED_SLOW) == 0 || stricmp(szToken, OPTS_SPEED_SLOW) == 0) {
      pgmKind = csp9_slow_pgm;
    } else if (stricmp(szToken, OPT_SPEED_TEST) == 0 || stricmp(szToken, OPTS_SPEED_TEST) == 0) {
      pgmKind = csp9_test_pgm;
    } else if (stricmp(szToken, OPT_STOP_DUMP) == 0 || stricmp(szToken, OPTS_STOP_DUMP) == 0) {
      bDump = true;
    } else if (stricmp(szToken, OPT_STOP_NOCLUSTER) == 0 || stricmp(szToken, OPTS_STOP_NOCLUSTER) == 0) {
      bCluster = false;
    } else if (stricmp(szToken, OPT_RESTART_UNTIL) == 0 || stricmp(szToken, OPTS_RESTART_UNTIL) == 0) {
      ++nToken;
      cn90GetToken(szParameters, szToken,  nToken, PARAMETER_MAXLEN_CN90);
      ++nToken;
      cn90GetToken(szParameters, szToken1, nToken, PARAMETER_MAXLEN_CN90);
      sp77sprintf(pRestart, PARAMETER_MAXLEN_CN90, TXT_RESTART_UNTIL, szToken, szToken1);
    } else {
      nFuncReturn = ERR_PARAM_CN00;
    } // end if

    ++nToken;
  } // end while

  return nFuncReturn;
} // cn40_StartStopOptions

  /* ------------------------------------------------------------------
 * IMPLEMENTATION PRIVATE FUNCTION cn40_ChangeDBState
 * ------------------------------------------------------------------
 */
#undef  CN_FUNCTION
#define CN_FUNCTION "cn40_ChangeDBState"
static tcn00_Error cn40_ChangeDBState
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax,
        DBMSrvKnl_State::StateValue targetState,
        bool            bStop)
{
  FUNCTION_DBG_MCN00_1("cn40_ChangeDBState");

  CONSOLE_TRACE_ENTER_CN00;

  tcn00_Error     nFuncReturn = OK_CN00;
  tsp9_pgm_kind   pgmKind;
  bool            bDump;
  bool            bCluster = false;
  char            szRestart [PARAMETER_MAXLEN_CN90] = "";
  DBMSrvKnl_State::StateValue currentState;

  // read Options
  nFuncReturn = cn90AnswerIError(replyData, replyLen, cn40_StartStopOptions(command->args, pgmKind, bDump, szRestart, bCluster));

  if (nFuncReturn == OK_CN00) {
    currentState = DBMSrvKnl_State::State(vcontrol->dbname);

    currentState = (bStop && targetState == DBMSrvKnl_State::StateOffline)
                   ? DBMSrvKnl_State::StateAdmin : currentState;

    if( targetState!=DBMSrvKnl_State::StateOffline ) {
        // in any case, start the event dispatcher, even if the database is not offline already
        DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
        DBMSrvMsg_Error errOut;

        DBMSrvCmd_SilentDispatcherUsingCommand dispatcherStart(DBMSrv_Command::KeyUnknownCommand);
        bool mustStartDispatcher(dispatcherStart.isConfiguredToRun(errOut));
        if( !errOut.IsEmpty() )
            return reply.startWithMessageList(errOut);
        if( mustStartDispatcher ) {
            CommandT cmdLineStartDispatcher;
            cmdLineStartDispatcher.setArgs(DBMSrvCmd_SilentDispatcherUsingCommand::Start);
            cmdLineStartDispatcher.nIndex = 1;
            nFuncReturn = dispatcherStart.run(vcontrol, &cmdLineStartDispatcher, reply);
        }
    }

    switch (currentState) {

      case DBMSrvKnl_State::StateOffline:  // Ist

        switch (targetState) {
          case DBMSrvKnl_State::StateAdmin:  // Soll
            nFuncReturn = cn40_OfflineToCold(vcontrol, pgmKind, replyData, replyLen, replyLenMax);
            break;

          case DBMSrvKnl_State::StateOnline:  // Soll
            nFuncReturn = cn40_OfflineToCold(vcontrol, pgmKind, replyData, replyLen, replyLenMax);
            if (nFuncReturn == OK_CN00) {
              nFuncReturn = cn40_ColdOrStandbyToWarm(vcontrol, szRestart, replyData, replyLen, replyLenMax);
            } // end if
            break;

        } // end switch
        break;

      case DBMSrvKnl_State::StateAdmin: // Ist

        switch (targetState) {
          case DBMSrvKnl_State::StateOffline: // Soll
            nFuncReturn = cn40_ColdOrStandbyToOffline(vcontrol, bDump, replyData, replyLen, replyLenMax);
            break;

          case DBMSrvKnl_State::StateOnline:    // Soll
            nFuncReturn = cn40_ColdOrStandbyToWarm(vcontrol, szRestart, replyData, replyLen, replyLenMax);
            break;

        } // end switch
        break;

      case DBMSrvKnl_State::StateStandby: // Ist

        switch (targetState) {
          case DBMSrvKnl_State::StateOffline: // Soll
            nFuncReturn = cn40_ColdOrStandbyToOffline(vcontrol, bDump, replyData, replyLen, replyLenMax);
            break;

          case DBMSrvKnl_State::StateOnline:    // Soll
            nFuncReturn = cn40_ColdOrStandbyToWarm(vcontrol, TXT_TAKEOVER, replyData, replyLen, replyLenMax);
            break;

        } // end switch
        break;

      case DBMSrvKnl_State::StateOnline: // Ist

        switch (targetState) {
          case DBMSrvKnl_State::StateOffline: // Soll
            nFuncReturn = cn40_OnlineToOffline(vcontrol, bCluster, replyData, replyLen, replyLenMax);
            break;

          case DBMSrvKnl_State::StateAdmin:    // Soll
            nFuncReturn = cn40_OnlineToOffline(vcontrol, bCluster, replyData, replyLen, replyLenMax);
            if (nFuncReturn == OK_CN00) {
              nFuncReturn = cn40_OfflineToCold(vcontrol, pgmKind, replyData, replyLen, replyLenMax);
            } // end if
            break;

        } // end switch
        break;

      default:
        nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_STATE_CN00);
        break;
    } // end switch

  } // end if

  if( nFuncReturn==OK_CN00 && targetState==DBMSrvKnl_State::StateOffline ) {
      // in any case, stop the event dispatcher, even if the database is offline already
      DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
      CommandT cmdLineStopDispatcher;
      cmdLineStopDispatcher.setArgs(DBMSrvCmd_SilentDispatcherUsingCommand::StopAll);
      cmdLineStopDispatcher.nIndex = 1;
      DBMSrvCmd_SilentDispatcherUsingCommand dispatcherStopAll(DBMSrv_Command::KeyUnknownCommand);
      nFuncReturn = dispatcherStopAll.run( vcontrol, &cmdLineStopDispatcher, reply);
  }

  if( nFuncReturn==OK_CN00 && targetState==DBMSrvKnl_State::StateOnline ) {
      DBMSrvMsg_Error configError;
      cn51DBMConfigValue updateStatisticsWhenReachingOnline(
          vcontrol->dbname,
          CFG_UPDATESTATISTICSWHENREACHINGONLINE_CN51);
      if( stricmp(updateStatisticsWhenReachingOnline, "yes")== 0 ) {
          tsp00_C512c emptyValue;
          emptyValue.rawAssign("");
          updateStatisticsWhenReachingOnline = emptyValue;
          DBMSrvCmdSQL_SqlUpdatestatPerSystemtable sqlUpdatestatPerSystemtable;
          if( !sqlUpdatestatPerSystemtable.updateSmartInBackground(vcontrol, configError) ) {
              DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
              nFuncReturn = reply.startWithMessageList(configError);
          }
      }
  }


  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn40_ChangeDBState
