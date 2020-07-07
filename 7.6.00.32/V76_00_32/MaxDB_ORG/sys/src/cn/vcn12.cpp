/*!
  -----------------------------------------------------------------------------

  module: vcn12.h

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: dbmcli

  description:  implementation of command line DBMServer access

  version:      7.2. / 6.2

  -----------------------------------------------------------------------------

                          Copyright (c) 1998-2005 SAP AG

  -----------------------------------------------------------------------------


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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

#include "gsp09.h"
#include "heo02.h"
#include "heo13.h"
#include "heo01.h"
#include "heo06.h"
#include "geo43.h"
#include "hsp100.h"

#include "hcn10.h"
#include "hcn13.h"
#include "hcn14.hpp"
#include "hcn90.h"

#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "RunTime/RTE_GetDBRootOfDB.h"
#ifndef _WIN32
  #include "RunTime/RTE_UNIXAuthenticate.hpp"
#endif
#include "RunTime/System/RTESys_ProcessID.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"

#include "ToolsCommon/Tools_PipeCall.hpp"

/***** Memory Debug BEGIN *****/
#ifdef BVMEMDEBUG
#pragma message ("Memory Debug ON")
#include <crtdbg.h>
#endif
/***** Memory Debug END *****/

/*
  -----------------------------------------------------------------------------
  specification
  -----------------------------------------------------------------------------
*/
#ifndef MAX_CRYPT
  #define MAX_CRYPT csp_cryptname
#endif

#define EndOfCommandC    "\n---\n"

static tsp00_CompName  c_component;
#define C_COMPONENT "dbmcli                                                          "

static tsp00_DbNamec g_szDbName;
static tsp00_NodeIdc g_szNode;

#define CMD_USER_LOGON   "user_logon"
#define CMD_USER_SAP     "user_sap"
#define CMD_SYSTEM_LOGON "user_system"
#define CMD_UTIL_CONNECT "util_connect"
#define CMD_SQL_CONNECT  "sql_connect"
#define CMD_SRV_CONNECT  "service_connect"
#define CMD_RELEASE      "release"
#define CMD_BYE          "bye"
#define CMD_EXIT         "exit"
#define CMD_QUIT         "quit"
#define CMD_RELEASE      "release"
#define CMD_DROP         "db_drop"
#define CMD_VERSION      "version"
#define CMD_NEXT         "dbm_next"

#define CTRL_ERR         "ERR"
#define CTRL_OK          "OK"

#define NO_ERROR_CN12    0
#define DBM_ERROR        1
#define LOCAL_ERROR      2
#define COMM_ERROR       3

#define XUK_DBMUSR       "DBMUSR"
#define XUK_DEFAULT      "DEFAULT"
#define XUK_COLD         "c"

#define C_SWITCH         '-'
#define SW_NODE          "n"
#define SW_STATIC        "s"
#define SW_DATABASE      "d"
#define SW_INPUT         "i"
#define SW_INPUTCONT     "ic"
#define SW_INPUTSESS     "is"
#define SW_INPUT_C_S_1   "ics"
#define SW_INPUT_C_S_2   "isc"
#define SW_OUTPUT        "o"
#define SW_VERSION       "V"
#define SW_DBROOT        "R"
#define SW_COMMAND       "c"
#define SW_PROTOCOL      "t"
#define SW_USER          "u"
#define SW_USERSAVE      "us"
#define SW_USERXUSER     "ux"
#define SW_USERLIST      "ul"
#define SW_USERDEL       "ud"
#define SW_USERKEY       "uk"
#define SW_USERPARAM     "up"
#define SW_XUSER         "U"
#define SW_USRUTL        "uUTL"
#define SW_XUSRUTL       "UUTL"
#define SW_USRSQL        "uSQL"
#define SW_XUSRSQL       "USQL"
#define SW_USRSRV        "uSRV"
#define SW_TPSYSID       "tpi"
#define SW_TPCONN        "tpc"
#define SW_TPPROF        "tpp"
#define SW_USRSAP        "uSAP"
#define SW_NOHOLD        "nohold"
#define SW_INTERVALL     "int"
#define SW_ENCRYPT       "e"
#define SW_KEY           "key"

#define ENCRYPT_TEXT_SSL      "SSL"

enum tcn12EncryptionMethod {
    EncryptionMethodNone,
    EncryptionMethodSSL
};

typedef struct tcn12Options {
    const char    *   application;
    const char    *   dbname;
    const char    *   dbroot;
    const char    *   node;
    FILE          *   infile;
    FILE          *   outfile;
    FILE          *   hProtocol;
    BOOLEAN           showServerVersion;
    int               cmdArgStart;
    void          *   session;
    bool              bStaticCServer;
    const char    *   szUser;
    const char    *   szSysId;
    const char    *   szConn;
    const char    *   szProfile;
    const char    *   szSaveUser;
    const char    *   szXUserUser;
    const char    *   szUserKey;
    const char    *   szUserParam;
    bool              bDelUser;
    bool              bListUser;
    const char    *   szUserUTL;
    const char    *   szUserSQL;
    bool              bService;
    tsp00_Int4        nMaxData;
    bool              bContinue;
    bool              bSession;
    bool              bNohold;
    int               nIntervall;
    int               nRepeat;
    bool              bSAP;
    bool              bSelfDBRoot;
    bool              bKey;
    tcn12EncryptionMethod encMethod;
} tcn12Options;

tcn12Options * g_pOptions = NULL;

#define TXT_ERR_WRONGUSRPWD   "Error! wrong username or password!"
#define TXT_ERR_MISSDBNAME    "Error! database name missing!"
#define TXT_ERR_XUSERNFOUND   "Error! XUser not found!"
#define TXT_ERR_MISSTPDATA    "Error! Incomplete TP data (-tpi/-tpc/-tpp)!"
#define TXT_ERR_MISSPROFILE   "Error! Missing R/3 tp profile!"
#define TXT_ERR_MISSSYSID     "Error! Missing R/3 system id!"
#define TXT_ERR_MISSCONN      "Error! Missing R/3 conn id!"
#define TXT_ERR_MISSNODE      "Error! Missing node name!"
#define TXT_ERR_MISSINPUT     "Error! Missing input file name!"
#define TXT_ERR_MISSOUTPUT    "Error! Missing output file name!"
#define TXT_ERR_MISSPROTOCOL  "Error! Missing protocol file name!"
#define TXT_ERR_OPENINPUT     "Error! Cannot open inputfile!"
#define TXT_ERR_OPENOUTPUT    "Error! Cannot open output file!"
#define TXT_ERR_OPENPROTOCOL  "Error! Cannot open protocol file!"
#define TXT_ERR_MISSINSTROOT  "Error! Missing name of installation root!"
#define TXT_ERR_UPWITHOUTUK   "Error! Use -up only with -uk!"
#define TXT_ERR_MIXSTATICNODE "Error! Don't mix -s and -n option!"
#define TXT_ERR_MULTIXUSER    "Error! Don't mix more than one XUser operation!"
#define TXT_ERR_MIXXUSER      "Error! Don't mix XUser operations with DBMServer command!"
#define TXT_ERR_MISSUSER      "Error! Missing user data!"
#define TXT_ERR_MEMORY        "Error! Cannot allocate memory!"
#define TXT_ERR_MISSDBMUSR    "Error! missing username or password for dbmuser!\n"
#define TXT_ERR_XUSERKEY      "Error! XUser key too long!"
#define TXT_ERR_XUSERAPI      "Error! XUser API error!"
#define TXT_ERR_READPWD       "Error! Read password error!"
#define TXT_ERR_MIXTP         "Error! Don't mix username with R/3 tp data!"
#define TXT_ERR_DBNOTFOUND    "Error! database not found"
#define TXT_ERR_MISSINTERVALL "Error! Missing intervall value!"
#define TXT_ERR_MIXROOTNAME   "Error! Do not mix installation root and instance name!"
#define TXT_ERR_CALLDBMCLI    "Error! Can not call matching dbmcli!"
#define TXT_ERR_ENCMETHOD     "Error! Unrecognized encryption method!"
#define TXT_ERR_ENCNOHOST     "Error! Option -"SW_NODE" required for encryption!"

#define TXT_MSG_ENTERPWD      "Enter password: "
#define TXT_MSG_RENTERPWD     "Re-enter password: "
#define TXT_MSG_PWDNOTSAME    "Passwords aren't identical\n"
#define TXT_MSG_OK            "OK\n"
#define TXT_MSG_ENTERDBMUSR   "Enter user for authorization: "
#define TXT_MSG_ENTERDBNAME   "Enter database name: "
#define TXT_MSG_ENTERSRVNAME  "Enter server name: "
#define TXT_MSG_ENTERSAVEUSR  "Enter user to save: "
#define TXT_MSG_ENTEROSUSR    "Enter operating system user: "
#define TXT_MSG_ENTERXUSR     "Enter user for XUser authorization: "

#define IS_UNICODE(x)         ( ((x)->bStaticCServer) ? false : cn14IsUnicode((x)->session))

/*
  -----------------------------------------------------------------------------
  start of implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function: cn12PrintProtocol
  -----------------------------------------------------------------------------
*/
static void cn12PrintProtocol
      ( tcn12Options * options,
        const char   * pCommand,
        const char   * pAnswer,
        const int      nLength = -1   )
{
  time_t          aTime;

  if (options->hProtocol != NULL) {
    time(&aTime);
    cn13fputs(options->hProtocol, "*** ", IS_UNICODE(options));
    cn13fputs(options->hProtocol, ctime(&aTime), IS_UNICODE(options));
    cn13fputs(options->hProtocol, pCommand, IS_UNICODE(options));
    cn13fputs(options->hProtocol, EndOfCommandC, IS_UNICODE(options));
    cn13fputs(options->hProtocol, pAnswer, IS_UNICODE(options), nLength);
  } // end if

} /* end cn12PrintProtocol */
static void cn12PrintProtocol
      ( tcn12Options * options,
        const char   * pString )
{
  time_t          aTime;

  if (options->hProtocol != NULL) {
    time(&aTime);
    cn13fputs(options->hProtocol, "+++ ", IS_UNICODE(options));
    cn13fputs(options->hProtocol, ctime(&aTime), IS_UNICODE(options));
    cn13fputs(options->hProtocol, pString, IS_UNICODE(options));
    cn13fputs(options->hProtocol, "\n", IS_UNICODE(options));
  } // end if

} /* end cn12PrintProtocol */

/*
  -----------------------------------------------------------------------------
  function: cn12DumpError
  -----------------------------------------------------------------------------
*/
static void cn12DumpError
      ( tcn12Options * options,
        const char   * errmsg )
{
    cn12PrintProtocol(options, errmsg);

    cn13fputs(options->outfile, errmsg, IS_UNICODE(options));
    cn13fputs(options->outfile, "\n", IS_UNICODE(options));
} /* end cn12DumpError */

/*
  -----------------------------------------------------------------------------
  function: cn12FatalError
  -----------------------------------------------------------------------------
*/
static void cn12FatalError
      ( tcn12Options * options,
        const char   * errmsg )
{
    cn12DumpError(options, errmsg);
    if (options->bKey && options->infile == stdin) {
        printf("Press any key to continue . . .");
        getchar();
    } // end if

    exit (LOCAL_ERROR);
} /* end cn12FatalError */


/*
  -----------------------------------------------------------------------------
  function: cn12ConvertArguments
  -----------------------------------------------------------------------------
*/
#ifdef _WIN32
  static char ** cn12ConvertArguments
    ( tcn12Options * options,
      int            argc,
      wchar_t      * argU[])
  {
    char **    argv = new char *[argc];
    int nIndex      = 0;

    if (argv == NULL) {
      cn12FatalError (options, TXT_ERR_MEMORY);
    } // end if

    for (nIndex = 0; nIndex < argc; nIndex++) {
      // UCS2 to UTF8 needs max. 4 bytes per character
      argv[nIndex] = new char[(wcslen(argU[nIndex]) * 4) + 1];
      if (argv[nIndex] == NULL) {
        cn12FatalError (options, TXT_ERR_MEMORY);
      } // end if
      cn13MemcpyUCS2ToUTF8(argv[nIndex], (char *) argU[nIndex], (int)wcslen(argU[nIndex]) * 2);
    } // end for

    return argv;
  } // end cn12ConvertArguments
#else
  #define cn12ConvertArguments(a, b, c)  c
#endif

/*
  -----------------------------------------------------------------------------
  function: cn12Reconvert
  -----------------------------------------------------------------------------
*/
#ifdef _WIN32
  static void cn12Reconvert
    ( tcn12Options * options,
      int            argc,
      char         * argv[])
  {

//    options->application;
//    options->dbname;
//    options->dbroot;
//    options->node;
//    options->infile;
//    options->outfile;
//    options->hProtocol;
//    options->showServerVersion;
// !! options->cmdArgStart;
//    options->session;
//    options->bStaticCServer;
// !! options->szUser;
//    options->szSysId;
//    options->szConn;
//    options->szProfile;
//    options->szSaveUser;
//    options->szXUserUser;
//    options->bDelUser;
//    options->bListUser;
// !! options->szUserUTL;
// !! options->szUserSQL;
//    options->bService;
//    options->nMaxData;

    static char   szUser[100];
    if (options->szUser != NULL) {
      cn13MemcpyUTF8ToASCIIAcp(&szUser[0], options->szUser);
      options->szUser = &szUser[0];
    } // end if

    static char   szUserUTL[100];
    if (options->szUserUTL != NULL) {
      cn13MemcpyUTF8ToASCIIAcp(&szUserUTL[0], options->szUserUTL);
      options->szUserUTL = &szUserUTL[0];
    } // end if

    static char   szUserSQL[100];
    if (options->szUserSQL != NULL) {
      cn13MemcpyUTF8ToASCIIAcp(&szUserSQL[0], options->szUserSQL);
      options->szUserSQL = &szUserSQL[0];
    } // end if


    if (options->cmdArgStart != undef_ceo00) {
      int nIndex  = 0;
      char * pNew = NULL;

      for (nIndex = options->cmdArgStart; nIndex < argc; ++nIndex) {
        pNew = new char [(strlen(argv[nIndex]) * 4) + 1];

        if (pNew == NULL) {
          cn12FatalError (options, TXT_ERR_MEMORY);
        } // end if
        cn13MemcpyUTF8ToASCIIAcp(pNew, argv[nIndex]);
        argv[nIndex] = pNew;

      } // end for
    } // end if

  } // end cn12Reconvert
#else
  #define cn12Reconvert(a, b, c)
#endif

/*
  -----------------------------------------------------------------------------
  function: cn12ReadValue
  -----------------------------------------------------------------------------
*/
static char  * cn12ReadValue
      ( tcn12Options * options,
        int            nTries,
        int            nReadPassword,
        const char   * szMessage,
        char         * szBuffer)
{
    char  *  pReturn = NULL;
    char     szLocalBuffer[100];

    tsp00_Namec     szPwd1;
    tsp00_Namec     szPwd2;

    tsp05_RteFileError rteError;

    int    nLoop = 0;

    if (options->infile == stdin) {
        while (pReturn == NULL && nTries > 0) {
            --nTries;
            cn13fputs(stdout, szMessage, IS_UNICODE(options));
            cn13fgets(szLocalBuffer, 99, options->infile, IS_UNICODE(options));
            cn90StripTrailingChar(szLocalBuffer, '\n');
            cn90StripTrailingChar(szLocalBuffer, '\r');
            cn90StripTrailingChar(szLocalBuffer, '\n');

            if ((strlen(szLocalBuffer) > 0) && (nReadPassword > 0)) {
                if (!sqlgetpass(TXT_MSG_ENTERPWD, szPwd1, rteError)) {
                    cn12FatalError(options, TXT_ERR_READPWD);
                } // end if
                if (strlen(szPwd1) > 0) {
                    sprintf(szBuffer, "%s,%s", szLocalBuffer, szPwd1.asCharp());
                    cn90StrUpperCopy(szBuffer, szBuffer, false);

                    if (nReadPassword > 1) {
                        if (!sqlgetpass(TXT_MSG_RENTERPWD, szPwd2, rteError)) {
                            cn12FatalError(options, TXT_ERR_READPWD);
                        } // end if

                        if (strcmp(szPwd1, szPwd2) == 0) {
                            pReturn = &szBuffer[0];
                        } else {
                            cn13fputs(stdout, TXT_MSG_PWDNOTSAME, IS_UNICODE(options));
                        } // end if
                    } else {
                        pReturn = &szBuffer[0];
                    } // end if
                } // end if
            } else if (strlen(szLocalBuffer) > 0) {
                sprintf(szBuffer, "%s", szLocalBuffer);
                pReturn = &szBuffer[0];
            } // end if
        } // end while
    } // end if

    return pReturn;
} /* end cn12ReadValue */

/*
  -----------------------------------------------------------------------------
  function: cn12ReadNextArgument
  -----------------------------------------------------------------------------
*/
#define RA_SWITCH  1
#define RA_UCASE   2

static char * cn12ReadNextArgument
      ( char * szArgs[],
        int  & nCurArg,
        int    nMaxArgs,
        int    nMode = 0)
{
  int    nCurrent  = nCurArg + 1;
  char * pArgument = NULL;

  if (nCurrent < nMaxArgs) {
    if ((nMode & RA_SWITCH) == RA_SWITCH && szArgs[nCurrent][0] == C_SWITCH) {
      pArgument = &(szArgs[nCurrent][1]);
      nCurArg   = nCurrent;
    } else if ((nMode & RA_SWITCH) != RA_SWITCH && szArgs[nCurrent][0] != C_SWITCH) {
      pArgument = szArgs[nCurrent];
      nCurArg   = nCurrent;
    } // end if
  } // end if

  if (pArgument != NULL && (nMode & RA_UCASE) == RA_UCASE) {
    cn90StrUpperCopy(pArgument, pArgument, false);
  } // end if

  return pArgument;
} // end cn12ReadNextArgument

/*
  -----------------------------------------------------------------------------
  function: cn12SaveUsers
  -----------------------------------------------------------------------------
*/
static void cn12SaveUsers(tcn12Options * options)
{
  int  nCheckReturn = DBMAPI_USR_FALSE_CN14;

  if (strlen(options->dbname) == 0 && options->szUserKey == NULL) {
    cn12FatalError(options, TXT_ERR_MISSDBNAME);
  } // end if

  if ((options->szUser != NULL)) {
    if (options->szUserKey != NULL) {
      nCheckReturn = cn14checkUserByKey(options->szUserKey,
                                        options->szUser);
    } else {
      nCheckReturn = cn14checkUser(options->node,
                                   options->dbname,
                                   options->szUser);
    } // end if
  } else if ((options->szXUserUser != NULL)) {
    nCheckReturn = cn14checkDefaultUser(options->szXUserUser);
  } else {
    nCheckReturn = cn14checkUser(options->node,
                                 options->dbname,
                                 "");
  } // end if

  if (nCheckReturn != DBMAPI_OK_CN14 && nCheckReturn != DBMAPI_USR_UNKNOWN_CN14) {
    // terminate program
    cn12FatalError(options, TXT_ERR_WRONGUSRPWD);
  } // end if

  if (options->szUserKey != NULL) {
    nCheckReturn = cn14saveUserByKey(options->szUserKey,
                                     options->node,
                                     options->dbname,
                                     options->szSaveUser,
                                     options->szUserParam);
  } else {
    nCheckReturn = cn14saveUser(options->node,
                                options->dbname,
                                options->szSaveUser);
  } // end if

  if (nCheckReturn == DBMAPI_OK_CN14) {
    cn13fputs(options->outfile, TXT_MSG_OK, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_MSG_OK);
  } else if (nCheckReturn == DBMAPI_XUSERKEY_CN14) {
    cn13fputs(options->outfile, TXT_ERR_XUSERKEY, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_ERR_XUSERKEY);
    exit(LOCAL_ERROR);
  } else {
    cn13fputs(options->outfile, TXT_ERR_XUSERAPI, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_ERR_XUSERAPI);
    exit(LOCAL_ERROR);
  } // end if
  exit(NO_ERROR_CN12);

} // end cn12SaveUsers

/*
  -----------------------------------------------------------------------------
  function: cn12DeleteUsers
  -----------------------------------------------------------------------------
*/
static void cn12DeleteUsers(tcn12Options * options)
{
  int  nCheckReturn = DBMAPI_USR_FALSE_CN14;

  if (strlen(options->dbname) == 0 && options->szUserKey == NULL) {
    cn12FatalError(options, TXT_ERR_MISSDBNAME);
  } // end if

  if ((options->szUser != NULL)) {
    if (options->szUserKey != NULL) {
      nCheckReturn = cn14checkUserByKey(options->szUserKey,
                                        options->szUser);
    } else {
      nCheckReturn = cn14checkUser(options->node,
                                   options->dbname,
                                   options->szUser);
    } // end if
  } else if ((options->szXUserUser != NULL)) {
    nCheckReturn = cn14checkDefaultUser(options->szXUserUser);
  } else {
    nCheckReturn = cn14checkUser(options->node,
                                 options->dbname,
                                 "");
  } // end if

  if (nCheckReturn != DBMAPI_OK_CN14) {
    // terminate program
    cn12FatalError(options, TXT_ERR_WRONGUSRPWD);
  } // end if

  if (options->szUserKey != NULL) {
    nCheckReturn = cn14deleteUserByKey(options->szUserKey);
  } else {
    nCheckReturn = cn14deleteUser(options->node,
                                  options->dbname);
  } // end if

  if (nCheckReturn == DBMAPI_OK_CN14) {
    cn13fputs(options->outfile, TXT_MSG_OK, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_MSG_OK);
  } else if (nCheckReturn == DBMAPI_XUSERKEY_CN14) {
    cn13fputs(options->outfile, TXT_ERR_XUSERKEY, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_ERR_XUSERKEY);
    exit(LOCAL_ERROR);
  } else if (nCheckReturn == DBMAPI_USR_UNKNOWN_CN14) {
    cn13fputs(options->outfile, TXT_ERR_XUSERNFOUND, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_ERR_XUSERNFOUND);
    exit(LOCAL_ERROR);
  } else {
    cn13fputs(options->outfile, TXT_ERR_XUSERAPI, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_ERR_XUSERAPI);
    exit(LOCAL_ERROR);
  } // end if

  exit(NO_ERROR_CN12);

} // end cn12DeleteUsers

/*
  -----------------------------------------------------------------------------
  function: cn12ListUsers
  -----------------------------------------------------------------------------
*/
static void cn12ListUsers(tcn12Options * options)
{

  int  nCheckReturn = DBMAPI_USR_FALSE_CN14;

  if (options->szXUserUser != NULL) {
    nCheckReturn = cn14checkDefaultUser(options->szXUserUser);
  } // end if

  if (nCheckReturn == DBMAPI_OK_CN14) {

    char  pBuffer[64 * 32];

    cn14listUsers(pBuffer, 64 * 32);

    cn13fputs(options->outfile, TXT_MSG_OK, IS_UNICODE(options));
    cn12PrintProtocol(options, TXT_MSG_OK);

    cn13fputs(options->outfile, pBuffer, IS_UNICODE(options));
    cn12PrintProtocol(options, pBuffer);
  } else {
    // terminate program
    cn12FatalError(options, TXT_ERR_WRONGUSRPWD);
  } // end if

  exit(NO_ERROR_CN12);

} // end cn12ListUsers

/*
  -----------------------------------------------------------------------------
  function: cn12GetXUser
  -----------------------------------------------------------------------------
*/
static char * cn12GetXUser(tcn12Options * options, const char * szKey, bool bWithDB = false)
{
  char              * szReturn = NULL;
  tsp4_xuser_record   recUser;
  tsp00_ErrText         szError;
  tsp00_Bool          bOk;
  tsp00_CryptName       pwCrypt;
  static char         szDBMUsr[2 * (sizeof(recUser.xu_user) + 1)];
  char                szUser    [sizeof(recUser.xu_user) + 1];
  char                szPassword[sizeof(recUser.xu_user) + 1];

  memset(recUser.xu_key, ' ', sizeof(recUser.xu_key));

  if (szKey == NULL) {
    strncpy(recUser.xu_key, XUK_DBMUSR, strlen(XUK_DBMUSR));
  } else {
    strncpy(recUser.xu_key, szKey, strlen(szKey));
  } /* end if */

  sqlgetuser (&recUser, NULL, szError, &bOk);

  if (bOk) {

    SAPDB_memcpy (&pwCrypt[0], &(recUser.xu_password[0]), sizeof(pwCrypt));
    cn90CalculateUncrypt(pwCrypt, szPassword, false);
    cn90StringPtoC(szUser, recUser.xu_user, sizeof(recUser.xu_user));
    sprintf(szDBMUsr, "%s,%s", szUser, szPassword);
    szReturn = new char[2 * (sizeof(recUser.xu_user) + 1)];
    if (szReturn == NULL) {
      // terminate program
      cn12FatalError(options, TXT_ERR_MEMORY);
    } // end if
    SAPDB_strcpy(szReturn, szDBMUsr);
    if (bWithDB && strlen(options->dbname) == 0) {
      g_szDbName.p2c(recUser.xu_serverdb);
      options->dbname = g_szDbName.asCharp();
    } // end if
    if (bWithDB && strlen(options->node) == 0) {
      g_szNode.p2c(recUser.xu_servernode);
      options->node = g_szNode.asCharp();
    } // end if
  } /* end if */

  if (szReturn == NULL) {
    // terminate program
    cn12FatalError(options, TXT_ERR_XUSERNFOUND);
  } // end if

  return szReturn;

} /* end cn12GetXUser */

/*
  -----------------------------------------------------------------------------
  function: cn12GetVersion
  -----------------------------------------------------------------------------
*/
static void cn12GetVersion
      ( int & nVMajor,
        int & nVMinor,
        int & nVCorr )
{
  tsp100_VersionID0           VersionID0;

  sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &VersionID0 );

  nVMajor = (int) VersionID0.MajorVersion_sp100;
  nVMinor = (int) VersionID0.MinorVersion_sp100;
  nVCorr  = (int) VersionID0.CorrLevel_sp100;
} /* end cn12GetVersion */

/*
  -----------------------------------------------------------------------------
  function: cn12FindDBRoot
  -----------------------------------------------------------------------------
*/
char * cn12FindDBRoot
      ( tcn12Options       * options )
{
    static char                 locDBRoot[512];
  void                      * hEnum;
  tsp09_rte_xerror            xError;
  tsp09_rte_installationinfo  relInfo;
  int                         nVMajor = InvalidVersion_csp09;
  int                         nVMinor = InvalidVersion_csp09;
  int                         nVCorr  = InvalidVersion_csp09;

  locDBRoot[0] = 0;

  if (options->dbname[0] == 0) {

    cn12GetVersion(nVMajor, nVMinor, nVCorr);
    sqlxopen_installation_enum (&hEnum, &xError);
    while (xError.xe_result == CtrlCmdOk_csp09) {
      sqlxnext_installation (hEnum, &relInfo, &xError);
      if ( xError.xe_result == CtrlCmdOk_csp09 &&
           relInfo.version.no [0] == nVMajor &&
           relInfo.version.no [1] == nVMinor &&
           relInfo.version.no [2] == nVCorr     ) {
        SAPDB_strcpy(locDBRoot, relInfo.dbroot);
      } /* end if */
    } /* end while */
    sqlxclose_installation_enum (hEnum);

  } else {
    RTE_GetDBRootOfDB((SAPDB_Char *)options->dbname,locDBRoot,512);
    if (locDBRoot[0] == 0) {
      cn12FatalError(options, TXT_ERR_DBNOTFOUND);
    } /* end if */
  } // end if

  return locDBRoot;
} /* cn12FindDBRoot */

/*
  -----------------------------------------------------------------------------
  function: cn12Usage
  -----------------------------------------------------------------------------
*/
static void cn12Usage
      (tcn12Options * options)
{
  cn13fputs (options->outfile, "usage: dbmcli [<options>] [[-c] <DBMServer-Command>]     \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "    <options>:                                           \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-u    username,passwd   (DBM user)                     \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-U    [userkey]         (default DBMUSR)               \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-uUTL [username,passwd] (UTILITY session)              \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-uSQL [username,passwd] (SQL session)                  \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-uSRV                   (Service session)              \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-V                      (show version of server)       \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-d    dbname            (set dbname)                   \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-R    dbroot            (set dbroot)                   \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-n    node              (name of servernode)           \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-e    encryption        (encryption method)            \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t                        (use only with -n)             \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t                        (valid methods: '"ENCRYPT_TEXT_SSL"')\n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-i    inputfile         (Default stdin)                \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-ic   inputfile         (Default stdin)                \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-o    outputfile        (Default stdout)               \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-t    protocolfile      (writes subsequent protocol)   \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-s                      (local mode, don't use with -n)\n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\t-key                    (wait for any key on exit)     \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "    <DBMServer-Command>:                                 \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\tEverything after the options will be sent to DBMServer.\n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\tFor more information about the DBMServer-Commands use  \n", IS_UNICODE(options));
  cn13fputs (options->outfile, "\tthe DBMServer-Command help.                            \n", IS_UNICODE(options));

  cn12FatalError(options, "");
} /* end cn12Usage */

/*
  -----------------------------------------------------------------------------
  function: cn12CheckStatic
  -----------------------------------------------------------------------------
*/
static void cn12CheckStatic
      ( tcn12Options       * options,
        char               * argv[] )
{

  if (options->bStaticCServer) {
    if (options->cmdArgStart == undef_ceo00) {
      options->bStaticCServer = false;
    } else {
      bool bNeedsDatabase   = false;
      bool bStaticAvailable = false;
      bool bNeedsUser       = false;

      if (cn10CheckStatic(argv[options->cmdArgStart], bStaticAvailable, bNeedsDatabase, bNeedsUser)) {
        if (!bStaticAvailable) {
          options->bStaticCServer = false;
        } else {
          if ( (!bNeedsUser      && (options->szUser         != NULL)) ||
               (!bNeedsDatabase  && (strlen(options->dbname) != 0   )) ||
               (options->szUserSQL != NULL)                            ||
               (options->szUserUTL != NULL)                            ||
               options->bService                                       ||
               options->bSAP                                              ) {
            options->bStaticCServer = false;
          } // end if
        } // end if
      } else {
        options->bStaticCServer = false;
      } // end if
    } // end if
  } // end if

#ifdef STATIC_DBMCLI
  #pragma message ("*** Making STATIC dbmcli ***")
  options->bStaticCServer = true;
#endif
} // end cn12CheckStatic

/*
  -----------------------------------------------------------------------------
  function: cn12CallRightDBMCli
  -----------------------------------------------------------------------------
*/
static void cn12CallRightDBMCli
      ( tcn12Options       * options,
        int                  argc,
        char               * argv[] )
{
  if (options->bStaticCServer && (!options->bSelfDBRoot || (options->dbname[0] != 0))) {
    char            szPath  [1024 + 1];
    char            szTmp   [1024 + 1];
    tsp01_RteError  aRteError;
    tsp00_Pathc     szExecPath;
    tsp00_Pathc     szDbroot;
    const char *    pTueddelchen = NULL;

    if (sqlGetMyModuleFilename ( argv[0], szPath, 1024, &aRteError)) {
      if (strstr(szPath, "dbmcli_HelpInst") != NULL) {
        return;
      } // end if
      szExecPath.rawAssign(szPath);
      sqlGetDBrootFromExecPath(szExecPath, szDbroot, 0);

      szPath[0] = 0;

#ifdef _WIN32
      if (options->dbroot[0] != 0 && stricmp(options->dbroot, szDbroot) != 0) {
        sprintf(szPath, "\"\"%s\\pgm\\dbmcli.exe\"", options->dbroot);
      } // end if
      pTueddelchen = "\"";
      for (size_t i = 0; i < strlen(szPath); ++i) szPath[i] = szPath[i] == '/' ? '\\' : szPath[i];
#else
      if (options->dbroot[0] != 0 && strcmp(options->dbroot, szDbroot) != 0) {
        sprintf(szPath, "%s/bin/dbmcli", options->dbroot);
      } // end if
      pTueddelchen = "";
#endif

      struct stat sBuf;
      SAPDB_strcpy(szTmp, szPath);
      cn90StripQuotes(szTmp);
      if (stat(szTmp, &sBuf) == 0) {


        // check version of target dbmcli
        SAPDB_strcpy(szTmp, szPath);
        strcat(szTmp, " -R ");
        strcat(szTmp, pTueddelchen);
        strcat(szTmp, options->dbroot);
        strcat(szTmp, pTueddelchen);
        strcat(szTmp, " dbm_version VERSION");
        strcat(szTmp, pTueddelchen);

        ceo43_CmdPipe  pObjDBMCli;
        bool           bNoNewLine;
        tsp00_Bool     bOk;
        tsp00_Int4     nExitCode;

        pObjDBMCli.eo43OpenCommand(szTmp, bOk);

        if (bOk) {
          if (pObjDBMCli.eo43NextLine(szTmp, 1024, bNoNewLine, nExitCode) > 0) {
            if (strncmp(szTmp, "OK", 2) == 0) {
              if (pObjDBMCli.eo43NextLine(szTmp, 1024, bNoNewLine, nExitCode) > 0) {
                if ((strlen(szTmp) > 2) && (szTmp[0] <= '7') && (szTmp[2] <= '4')) {

                  // old version found -> call the dbmcli
                  pObjDBMCli.eo43CloseCommand(nExitCode);
                  for (int i = 1; i < argc; ++i) {
                    strcat(szPath, " ");
                    if (strchr(argv[i], ' ') != NULL) strcat(szPath, "\"");
                    strcat(szPath, argv[i]);
                    if (strchr(argv[i], ' ') != NULL) strcat(szPath, "\"");
                  } // end for
                  strcat(szPath, pTueddelchen);

                  int nExit = system(szPath);
                  if (nExit == -1) {
                    cn12FatalError (options, TXT_ERR_CALLDBMCLI);
                  } else {
                    exit(nExit);
                  } // end if

                } // end if
              } // end if
            } // end if

          } // end if

          pObjDBMCli.eo43CloseCommand(nExitCode);
        } // end if

      } // end if

    } // end if

  } // end if

} // end cn12CallRightDBMCli

/*
  -----------------------------------------------------------------------------
  function: cn12ParseOptions
  -----------------------------------------------------------------------------
*/
static void cn12ParseOptions
      ( tcn12Options       * options,
        int                  argc,
        char               * argv[] )
{
  int          argI           = 0;
  char       * pSwitch        = NULL;
  char       * pValue         = NULL;
  const char * pTemp          = NULL;
  bool         bReadUser      = false;
  bool         bReadDatabase  = false;
  bool         bReadServer    = false;
  bool         bReadSaveUser  = false;
  bool         bReadXUserUser = false;

  static char        szUser[100];
  static char        szServer[100];
  static char        szDatabase[100];
  static char        szSaveUser[100];
  static char        szXUserUser[100];

  /* init options struct */
  options->application       = argv[0];
  options->dbname            = "";
  options->dbroot            = "";
  options->node              = "";
  options->infile            = stdin;
  options->outfile           = stdout;
  options->hProtocol         = NULL;
  options->showServerVersion = false;
  options->cmdArgStart       = undef_ceo00;
  options->session           = NULL;
  options->bStaticCServer    = false;
  options->szUser            = NULL;
  options->szSysId           = NULL;
  options->szConn            = NULL;
  options->szProfile         = NULL;
  options->szSaveUser        = NULL;
  options->szXUserUser       = NULL;
  options->szUserKey         = NULL;
  options->szUserParam       = NULL;
  options->bDelUser          = false;
  options->bListUser         = false;
  options->szUserUTL         = NULL;
  options->szUserSQL         = NULL;
  options->bService          = false;
  options->nMaxData          = 0;
  options->bContinue         = false;
  options->bSession          = false;
  options->bNohold           = false;
  options->nIntervall        = 0;
  options->nRepeat           = 0;
  options->bSAP              = false;
  options->bSelfDBRoot       = false;
  options->bKey              = false;
  options->encMethod         = EncryptionMethodNone;

  /* read options form command line into struct */
  pSwitch = cn12ReadNextArgument(argv, argI, argc, RA_SWITCH);

  while (pSwitch != NULL) {

    // * Node -n ******************************
    if (strcmp(pSwitch, SW_NODE) == 0) {
      options->node = cn12ReadNextArgument(argv, argI, argc);
      if (options->node == NULL) {
          options->node = "";
          bReadServer = true;
      } // end if

    // * Static -s ****************************
    } else if (strcmp(pSwitch, SW_STATIC) == 0) {
      options->bStaticCServer = true;

    // * Key -key *****************************
    } else if (strcmp(pSwitch, SW_KEY) == 0) {
      options->bKey = true;

    // * Encryption -e ************************
    } else if (SAPDB_strcmp(pSwitch, SW_ENCRYPT) == 0) {
        const char* encryptMethod(cn12ReadNextArgument(argv, argI, argc)); // we dont delete it
        if (encryptMethod == NULL || SAPDB_strcasecmp(ENCRYPT_TEXT_SSL,encryptMethod) != 0) {
            cn12FatalError (options, TXT_ERR_ENCMETHOD);
        }
        else {
            // the only method that is supported currently
            options->encMethod = EncryptionMethodSSL;
        }

    // * Database -d **************************
    } else if (strcmp(pSwitch, SW_DATABASE) == 0) {
      options->dbname = cn12ReadNextArgument(argv, argI, argc);
      if (options->dbname == NULL) {
          options->dbname = "";
          bReadDatabase = true;
      } // end if

    // * Inputfile -i *************************
    } else if (strcmp(pSwitch, SW_INPUT) == 0) {
      pValue = cn12ReadNextArgument(argv, argI, argc);
      if (pValue == NULL) {
        cn12FatalError (options, TXT_ERR_MISSINPUT);
      } // end if
      options->infile = cn13fopen (pValue, "r");
      if (options->infile == NULL) {
        options->infile = stdin;
        cn12FatalError (options, TXT_ERR_OPENINPUT);
      } // end if

    // * Inputfile with continue -ic **********
    } else if (strcmp(pSwitch, SW_INPUTCONT) == 0) {
      pValue = cn12ReadNextArgument(argv, argI, argc);
      if (pValue == NULL) {
        cn12FatalError (options, TXT_ERR_MISSINPUT);
      } // end if
      options->infile = cn13fopen (pValue, "r");
      if (options->infile == NULL) {
        options->infile = stdin;
        cn12FatalError (options, TXT_ERR_OPENINPUT);
      } // end if
      options->bContinue = true;

    // * Inputfile with session -is ***********
    } else if (strcmp(pSwitch, SW_INPUTSESS) == 0) {
      pValue = cn12ReadNextArgument(argv, argI, argc);
      if (pValue == NULL) {
        cn12FatalError (options, TXT_ERR_MISSINPUT);
      } // end if
      options->infile = cn13fopen (pValue, "r");
      if (options->infile == NULL) {
        options->infile = stdin;
        cn12FatalError (options, TXT_ERR_OPENINPUT);
      } // end if
      options->bSession = true;

    // * Inputfile with continue and session -ics ***
    } else if ((strcmp(pSwitch, SW_INPUT_C_S_1) == 0) ||
               (strcmp(pSwitch, SW_INPUT_C_S_2) == 0)    ) {
      pValue = cn12ReadNextArgument(argv, argI, argc);
      if (pValue == NULL) {
        cn12FatalError (options, TXT_ERR_MISSINPUT);
      } // end if
      options->infile = cn13fopen (pValue, "r");
      if (options->infile == NULL) {
        options->infile = stdin;
        cn12FatalError (options, TXT_ERR_OPENINPUT);
      } // end if
      options->bSession  = true;
      options->bContinue = true;

    // * Outputfile -o ************************
    } else if (strcmp(pSwitch, SW_OUTPUT) == 0) {
      pValue = cn12ReadNextArgument(argv, argI, argc);
      if (pValue == NULL) {
        cn12FatalError (options, TXT_ERR_MISSOUTPUT);
      } // end if
      options->outfile = cn13fopen (pValue, "w");
      if (options->outfile == NULL) {
        options->outfile = stdout;
        cn12FatalError (options, TXT_ERR_OPENOUTPUT);
      } // end if

    // * Protocolfile -t **********************
    } else if (strcmp(pSwitch, SW_PROTOCOL) == 0) {
      pValue = cn12ReadNextArgument(argv, argI, argc);
      if (pValue == NULL) {
        cn12FatalError (options, TXT_ERR_MISSPROTOCOL);
      } // end if
      options->hProtocol = cn13fopen (pValue, "a+");
      if (options->hProtocol == NULL) {
        cn12FatalError (options, TXT_ERR_OPENPROTOCOL);
      } else {
        // write options to protocol
        if (options->hProtocol != NULL) {
          char  pBuffer[1000];
          int    nIndex = 0;
          pBuffer[0] = 0;
          for (nIndex = 0; nIndex < argc; nIndex++) {
            if ((1000 - strlen(pBuffer)) > (strlen(argv[nIndex]) + 2)) {
              strcat(pBuffer, argv[nIndex]);
              strcat(pBuffer, " ");
            } // end if
          } // end if
          cn12PrintProtocol(options, pBuffer);
        } // end if
      } // end if

    // * Version -V ***************************
    } else if (strcmp(pSwitch, SW_VERSION) == 0) {
      options->showServerVersion = true;

    // * DBRoot -R ****************************
    } else if (strcmp(pSwitch, SW_DBROOT) == 0) {
      options->dbroot = cn12ReadNextArgument(argv, argI, argc);
      if (options->dbroot == NULL) {
        cn12FatalError (options, TXT_ERR_MISSINSTROOT);
      } else {
        cn90StripQuotes((char *) options->dbroot);
      } // end if

    // * DBMServer Command -c *****************
    } else if (strcmp(pSwitch, SW_COMMAND) == 0) {
      // do nothing

    // * User -u ******************************
    } else if (strcmp(pSwitch, SW_USER) == 0) {
      options->szUser = cn12ReadNextArgument(argv, argI, argc, RA_UCASE);
      if (options->szUser == NULL) {
        bReadUser = true;
      } // end if

    // * TP sysid -tpi ************************
    } else if (strcmp(pSwitch, SW_TPSYSID) == 0) {
      options->szSysId = cn12ReadNextArgument(argv, argI, argc);
      if (options->szSysId == NULL) {
        cn12FatalError (options, TXT_ERR_MISSSYSID);
      } // end if

    // * TP conn -tpc *************************
    } else if (strcmp(pSwitch, SW_TPCONN) == 0) {
      options->szConn = cn12ReadNextArgument(argv, argI, argc);
      if (options->szConn == NULL) {
        cn12FatalError (options, TXT_ERR_MISSCONN);
      } // end if

    // * TP profile -tpp **********************
    } else if (strcmp(pSwitch, SW_TPPROF) == 0) {
      options->szProfile = cn12ReadNextArgument(argv, argI, argc);
      if (options->szProfile == NULL) {
        cn12FatalError (options, TXT_ERR_MISSPROFILE);
      } // end if

    // * SAP User -uSAP ***********************
    } else if (strcmp(pSwitch, SW_USRSAP) == 0) {
      options->bSAP = true;

    // * no hold -nohold **********************
    } else if (strcmp(pSwitch, SW_NOHOLD) == 0) {
      options->bNohold = true;

    // * intervall -int ***********************
    } else if (strcmp(pSwitch, SW_INTERVALL) == 0) {
      pTemp = cn12ReadNextArgument(argv, argI, argc);
      if (pTemp == NULL) {
        cn12FatalError (options, TXT_ERR_MISSINTERVALL);
      } // end if
      options->nIntervall = atoi(pTemp);
      if (options->nIntervall == 0) {
        cn12FatalError (options, TXT_ERR_MISSINTERVALL);
      } // end if
      pTemp = cn12ReadNextArgument(argv, argI, argc);
      if (pTemp != NULL) {
        options->nRepeat = atoi(pTemp);
      } // end if

    // * Save User -us ************************
    } else if (strcmp(pSwitch, SW_USERSAVE) == 0) {
      options->szSaveUser = cn12ReadNextArgument(argv, argI, argc, RA_UCASE);
      if (options->szSaveUser == NULL) {
        bReadSaveUser = true;
      } // end if

    // * XUserUser -ux ************************
    } else if (strcmp(pSwitch, SW_USERXUSER) == 0) {
      options->szXUserUser = cn12ReadNextArgument(argv, argI, argc, RA_UCASE);
      if (options->szUser == NULL) {
        bReadXUserUser = true;
      } // end if

    // * Delete User -ud **********************
    } else if (strcmp(pSwitch, SW_USERDEL) == 0) {
      options->bDelUser = true;

    // * List Users -ul ***********************
    } else if (strcmp(pSwitch, SW_USERLIST) == 0) {
      options->bListUser = true;

    // * User with Userkey -U *****************
    } else if (strcmp(pSwitch, SW_XUSER) == 0) {
      options->szUser = cn12GetXUser(options, cn12ReadNextArgument(argv, argI, argc), true);

    // * Userkey -uk **************************
    } else if (strcmp(pSwitch, SW_USERKEY) == 0) {
      options->szUserKey = cn12ReadNextArgument(argv, argI, argc);

    // * User parameters -up ******************
    } else if (strcmp(pSwitch, SW_USERPARAM) == 0) {
      options->szUserParam = cn12ReadNextArgument(argv, argI, argc);

    // * szUserUTL -uUTL ********************
    } else if (strcmp(pSwitch, SW_USRUTL) == 0) {
      options->szUserUTL = cn12ReadNextArgument(argv, argI, argc);
      options->szUserUTL = options->szUserUTL == NULL ? "" : options->szUserUTL;

    // * szUserUTL -UUTL ********************
    } else if (strcmp(pSwitch, SW_XUSRUTL) == 0) {
      pTemp = cn12ReadNextArgument(argv, argI, argc);
      options->szUserUTL = cn12GetXUser(options, pTemp == NULL ? XUK_COLD : pTemp);

    // * szUserSQL -uSQL ************************
    } else if (strcmp(pSwitch, SW_USRSQL) == 0) {
      options->szUserSQL = cn12ReadNextArgument(argv, argI, argc);
      options->szUserSQL = options->szUserSQL == NULL ? "" : options->szUserSQL;

    // * szUserSQL -USQL ************************
    } else if (strcmp(pSwitch, SW_XUSRSQL) == 0) {
      pTemp = cn12ReadNextArgument(argv, argI, argc);
      options->szUserSQL = cn12GetXUser(options, pTemp == NULL ? XUK_DEFAULT : pTemp);

    // * bService -uSRV *************************
    } else if (strcmp(pSwitch, SW_USRSRV) == 0) {
      options->bService = true;

    // ****************************************
    } else {
      cn12Usage (options);
    } // end if

    pSwitch = cn12ReadNextArgument(argv, argI, argc, RA_SWITCH);
  } /* end while */

  if (options->bStaticCServer && strlen(options->node) > 0) {
    cn12FatalError (options, TXT_ERR_MIXSTATICNODE);
  } /* end if */

  if ( options->dbname[0] != 0 && options->dbroot[0] != 0) {
    cn12FatalError (options, TXT_ERR_MIXROOTNAME);
  } /* end if */

  // check that if encryption method is there, node is there
  if( options->encMethod != EncryptionMethodNone && options->node[0] == 0 ) {
      cn12FatalError (options, TXT_ERR_ENCNOHOST);
  }

  /* dbroot check */
  if ( options->node[0] == 0 && options->dbroot[0] == 0 && options->bStaticCServer) {
    options->dbroot = cn12FindDBRoot(options);
    if (options->dbroot[0] == 0) {
      cn13fputs(options->outfile, "Warning! Can't find Installation! \n  (use \"", IS_UNICODE(options));
      cn13fputs(options->outfile, options->application, IS_UNICODE(options));
      cn13fputs(options->outfile, " -s -R <instroot> inst_reg [-c]\" to avoid this message)\n", IS_UNICODE(options));
    } else {
      options->bSelfDBRoot = true;
    } /* end if */
  } /* end if */

  // exclude check
  if (((options->bDelUser                ) &&
       (options->bListUser          ||
        options->szSaveUser != NULL ||
        bReadSaveUser                  )       ) &&
      ((options->bListUser               ) &&
       (options->bDelUser           ||
        options->szSaveUser != NULL ||
        bReadSaveUser                  )       )    ) {
    cn12FatalError (options, TXT_ERR_MULTIXUSER);
  } // end if

  if ((options->szUserParam != NULL) && (options->szUserKey == NULL)) {
    cn12FatalError (options, TXT_ERR_UPWITHOUTUK);
  } // end if

  if (cn12ReadNextArgument(argv, argI, argc) != NULL) {
    // exclude check
    options->cmdArgStart = argI;
    if ( ( options->bDelUser                   ||
           options->bListUser                  ||
           options->szSaveUser != NULL         ||
           options->szUserKey  != NULL         ||
           options->szUserParam!= NULL         ||
           bReadSaveUser                          ) &&
         ( options->szUserUTL  != NULL         ||
           options->szUserSQL  != NULL         ||
           options->bService                   ||
           options->showServerVersion          ||
           options->cmdArgStart != undef_ceo00    )    ) {
      cn12FatalError (options, TXT_ERR_MIXXUSER);
    } // end if
  } /* end if */

  // read option values
  if (bReadServer && strlen(options->node) == 0) {
    options->node = cn12ReadValue(options, 1, 0, TXT_MSG_ENTERSRVNAME, szServer);
    if (options->node == NULL) {
      options->node = "";
    } // end if
  } // end if
  if (bReadDatabase && strlen(options->dbname) == 0) {
    options->dbname = cn12ReadValue(options, 1, 0, TXT_MSG_ENTERDBNAME, szDatabase);
    if (options->dbname == NULL) {
      options->dbname = "";
      bReadUser = false;
    } // end if
  } // end if
  // read users
  if (bReadUser && options->szUser == NULL) {
    options->szUser = cn12ReadValue(options, 3, 1, TXT_MSG_ENTERDBMUSR, szUser);
    if (options->szUser == NULL) {
      cn12FatalError (options, TXT_ERR_MISSUSER);
    } // end if
  } // end if
  if (bReadSaveUser && options->szSaveUser == NULL) {
    options->szSaveUser = cn12ReadValue(options, 3,  2, TXT_MSG_ENTERSAVEUSR, szSaveUser);
    if (options->szSaveUser == NULL) {
      cn12FatalError (options, TXT_ERR_MISSUSER);
    } // end if
  } // end if
  if (bReadXUserUser && options->szXUserUser == NULL) {
    options->szXUserUser = cn12ReadValue(options, 3, 1, TXT_MSG_ENTERXUSR, szXUserUser);
    if (options->szXUserUser == NULL) {
      cn12FatalError (options, TXT_ERR_MISSUSER);
    } // end if
  } // end if

  if (options->szUser != NULL && (options->bSAP || options->szSysId != NULL || options->szConn != NULL || options->szProfile != NULL)) {
    cn12FatalError (options, TXT_ERR_MIXTP);
  } // end if

  if ( ( options->szConn != NULL || options->szSysId != NULL || options->szProfile != NULL ) &&
       ( options->szConn == NULL || options->szSysId == NULL || options->szProfile == NULL )    ) {
    cn12FatalError (options, TXT_ERR_MISSTPDATA);
  } // end if

  if ( options->bSAP && ( options->szConn == NULL || options->szSysId == NULL || options->szProfile == NULL )    ) {
    cn12FatalError (options, TXT_ERR_MISSTPDATA);
  } // end if

  cn12CallRightDBMCli(options, argc, argv);
  cn12CheckStatic(options, argv);

} /* end cn12ParseOptions */

/*
  -----------------------------------------------------------------------------
  function: cn12ReadHereArg
  -----------------------------------------------------------------------------
*/
static tsp00_Int4 cn12ReadHereArg
      ( tcn12Options  * options,
        char         * data,
        tsp00_Int4        linelen)
{
  char       stopArg [100];
  tsp00_Int4   stopArgLen;
  tsp00_Int4   cmdLen;
  tsp00_Int4   linepos;
  char     * cmdPos;
  char     * packetEnd;
  tsp00_Int4   inData;

  linepos = 1;
  /* get stop arg */
  while (!isspace (data [linepos])) {
    ++linepos;
  }
  stopArgLen = linepos - 1;
  SAPDB_memcpy (stopArg, data + 1, stopArgLen);

  /* move command */
  while (isspace (data [linepos])) {
    ++linepos;
  }
  cmdLen = linelen - linepos;
  SAPDB_memcpy (data, data + linepos, cmdLen);
  data [cmdLen] = '\n';
  cmdPos = data + cmdLen + 1;
  packetEnd = data + options->nMaxData;

  /* read argument */
  do {
    if (cn13fgets (cmdPos, (int)(packetEnd - cmdPos), options->infile, IS_UNICODE(options))) {
      inData = (memcmp (cmdPos, stopArg, stopArgLen) != 0);
      if (inData) {
        cmdPos += strlen (cmdPos);
      }
    } else {
     inData = false;
    }
  } while (inData);

  *cmdPos = '\0';
  return ((tsp00_Int4)(cmdPos - data));

} /* end cn12ReadHereArg */

/*
  -----------------------------------------------------------------------------
  function: cn12CheckAnswer
  -----------------------------------------------------------------------------
*/
static tsp00_Int4 cn12CheckAnswer
      ( char  * pData )
{
  tsp00_Int4   i       = 0;
  tsp00_Int4   nNumber = 0;

  if (strncmp(pData, CTRL_ERR, strlen(CTRL_ERR)) == 0) {

    while (pData[i] != '\n' && pData[i] != '\0') {
      i++;
    } /* end while */
    if (pData[i] == '\n') {
      i++;
    } /* end if */
    nNumber = atol(&pData[i]);
    nNumber = (nNumber == 0) ? LOCAL_ERROR : nNumber;
  } /* end if */

  return nNumber;
} /* end cn12CheckAnswer */

/*
  -----------------------------------------------------------------------------
  function: cn12CServerConnect
  -----------------------------------------------------------------------------
*/
static void cn12CServerConnect
      ( tcn12Options  *  options )
{
  tsp00_ErrTextc            errtext;
  const char              * pError  = errtext.asCharp();
  Tools_DynamicUTF8String   oError;
  tsp00_Int4                commErr(DBMAPI_OK_CN14);
  static BOOLEAN            fCancel = false;

  if (options->bStaticCServer) {
    // static DBMServer
    sqlxinit("","");

    SAPDB_Int4 userId  = 0; /* obsolete */
    SAPDB_Bool bVerifiedLocalCommunication;
#ifdef _WIN32
    bVerifiedLocalCommunication = true;
#else
    Msg_List errList;
    bVerifiedLocalCommunication = RTE_VerifyAsSapdbAdministrator (errList);
#endif
    ctrlservopen (
            &(options->session),
            options->dbname,
            options->dbroot,
            &fCancel,
            bVerifiedLocalCommunication,
            true,
            userId,
            true,
            RTESys_Hostname(),
            RTESys_ProcessID());
    if ( !options->session )
    {
      char  cBuffer[200];
      sprintf (cBuffer, "Error! Connection failed to node %s for database %s",
                (char *) options->node[0]   ? options->node   : "(local)",
                (char *) options->dbname[0] ? options->dbname : "(no dbname)" );
      cn12FatalError (options, cBuffer);
    }
    options->nMaxData = CtrlPacketSize_csp09;
  } else {
    // real communication
    char* dbmDynamicURI(NULL);
    const char* theURI(NULL);
    bool usingDynamicURI(false);
    if( options->encMethod != EncryptionMethodNone ) {
      // create URI for encryption
      tsp00_Int4 lenURI = cn14buildDBMURI(options->node, options->dbname, DBMAPI_USE_OUTURI_CN14_NO, dbmDynamicURI, errtext);
      if( lenURI <= 0)
        commErr = DBMAPI_NOT_OK_CN14;
      else {
        dbmDynamicURI = new char[lenURI];
        if( dbmDynamicURI == NULL ) {
          commErr = DBMAPI_NOT_OK_CN14;
          errtext.rawAssign(TXT_ERR_MEMORY);
        }
        else {
          lenURI = cn14buildDBMURI(options->node, options->dbname, DBMAPI_USE_OUTURI_CN14_YES, dbmDynamicURI, errtext);
          if( lenURI <=0 )
            commErr = DBMAPI_NOT_OK_CN14;
          else {
            // here we make sure we pass the URI of we have no error
            theURI = dbmDynamicURI;
          }
        }
      }
   }
    else
      theURI = options->node;

    if( commErr == DBMAPI_OK_CN14 ) {
      if (options->szUser != NULL) {
        if (strlen(options->dbname) != 0) {
          cn90StrUpperCopy((char *)options->szUser, options->szUser, false);
        } // end if
        commErr = cn14connectDBMUsr (theURI,
                                    options->dbname,
                                    options->dbroot,
                                    options->szUser,
                                    &(options->session),
                                    oError);
      } else if (options->szSysId != NULL) {
        commErr = cn14connectDBMTp  (theURI,
                                    options->dbname,
                                    options->dbroot,
                                    options->szSysId,
                                    options->szConn,
                                    options->szProfile,
                                    &(options->session),
                                    oError);
      } else {
        commErr = cn14connectDBM (theURI,
                                    options->dbname,
                                    options->dbroot,
                                    &(options->session),
                                    oError);
      } // end if
      pError = oError.CharPtr();
    }

    if( dbmDynamicURI != NULL )
      delete[] dbmDynamicURI;


    DBMCli_String outMsg;
    if (commErr != DBMAPI_OK_CN14) {
      outMsg += "Error! Connection failed to node ";
    } else {
      outMsg += "Connection established to node ";
    } // end if
      
    outMsg += options->node[0]==0?"(local)":options->node;
    outMsg += " for database ";
    outMsg += options->dbname[0]==0?"(no dbname)":options->dbname;
      
    if(commErr != DBMAPI_OK_CN14) {
      	outMsg += ":\n";
  		outMsg += pError;
    	cn12FatalError (options, outMsg);
    } // end if
    
    cn12PrintProtocol(options, outMsg);
    tsp00_Int4        nDummy1;
    tsp00_Int4        nDummy2;
    cn14packetstats(options->session, &(options->nMaxData), &nDummy1, &nDummy2);
  } /* end if */

} /* end cn12CServerConnect */

/*
  -----------------------------------------------------------------------------
  function: cn12CServerCommand
  -----------------------------------------------------------------------------
*/
static int cn12CServerCommand
      ( tcn12Options       * options,
        char               * data,
        char              ** reply,
        bool                 bQuiet,
        bool                 bRepeat,
        bool               & bRelease)
{
  tsp00_ErrTextc errtext;
  tsp00_Int4       commErr      = DBMAPI_OK_CN14;
  tsp00_Int4       nCmdLen      = 0;
  tsp00_Int4       nReplyLen    = 0;
  tsp00_Int4       nMaxReply    = 0;
  int            nRepeat      = 1;

  if (bRepeat) {
    if (options->nIntervall > 0 && options->nRepeat == 0) {
      nRepeat = -1;
    } else {
      nRepeat = options->nRepeat;
    } // end if
  } // end if

  do {

    // calc packet info
    nCmdLen   = (tsp00_Int4)strlen (data) + 1; /* null byte!!! */
    nMaxReply = options->nMaxData - nCmdLen;
    *reply    = data + nCmdLen;

    // check for release command
    bRelease = stricmp (data, CMD_DROP   ) == 0 ||
               stricmp (data, CMD_RELEASE) == 0 ||
               stricmp (data, CMD_BYE    ) == 0 ||
               stricmp (data, CMD_EXIT   ) == 0 ||
               stricmp (data, CMD_QUIT   ) == 0 ;

    // exit client even we got an error on the exit commands
    bool releaseOnError( bRelease && (stricmp(data, CMD_DROP)!=0) );

    if (options->bStaticCServer) {
      // static server
      bRelease = ctrlservcommand (options->session, data, nCmdLen, *reply, (int *) &nReplyLen, nMaxReply) != CtrlCmdOk_csp09;
    } else {
      // Kommunikation
      commErr   = cn14cmdExecute (options->session, data, nCmdLen, *reply, &nMaxReply, errtext);
      nReplyLen = nMaxReply;
    } // end if

    if (commErr == DBMAPI_OK_CN14) {
      bRelease = bRelease && (releaseOnError || (cn12CheckAnswer(*reply) == 0));
      if (!bQuiet || cn12CheckAnswer(*reply) != 0) {
        // we received a reply
        cn13fputs(options->outfile, *reply, IS_UNICODE(options), nReplyLen);
        cn12PrintProtocol(options, data, *reply, nReplyLen);
      } // end if
    } else {
      if (!bRelease) {
        // no release command, a communication error is really an error
        char  Buffer[100];
        sprintf (Buffer, "DBM-Api error ( %d'%.40s')\n", commErr, (char *) errtext);
        cn13fputs (options->outfile, Buffer, IS_UNICODE(options));
        cn12PrintProtocol(options, data, Buffer);
      } else {
          // release command, we received a communication error, which we consider as okay...
          cn13fputs (options->outfile, CTRL_OK"\n", IS_UNICODE(options));
          commErr = DBMAPI_OK_CN14;
      } // end if
    } // end if

    if (nRepeat > 0) {
      --nRepeat;
    } // end if
    if ((options->nIntervall > 0) && bRepeat && (nRepeat != 0) && !bRelease) {
      sqlsleep(options->nIntervall);
    } // end if
  } while (nRepeat != 0);

  return commErr;

} /* end cn12CServerCommand */

/*
  -----------------------------------------------------------------------------
  function: cn12CServerDisconnect
  -----------------------------------------------------------------------------
*/
static void cn12CServerDisconnect
      ( tcn12Options       * options)
{
  if (options->bStaticCServer) {
    ctrlservclose (options->session);
  } else {
    cn14release (&(options->session));
  } /* end if */
} /* cn12CServerDisconnect */

/*
  -----------------------------------------------------------------------------
  function cn12Aborthandler
  -----------------------------------------------------------------------------
 */
extern "C" void cn12Aborthandler(int sig) {

  switch (sig) {
    case SIGINT:
    case SIGTERM:
    case SIGABRT:
#if defined (_WIN32)
    case SIGBREAK:
#endif
      // disconnect
      if (g_pOptions != NULL) {
        if (g_pOptions->session != NULL) {
          cn12CServerDisconnect(g_pOptions);
        } // end if
      } // end if
      exit(0);
      break;
    default:
      // do nothing
      break;
  } // end switch

} // end cn12Aborthandler

/*
  -----------------------------------------------------------------------------
  function: cn12Next
  -----------------------------------------------------------------------------
*/
static void cn12Next
      ( tcn12Options       * options )
{
  char          * data      = NULL;
  char          * replyData = NULL;
  int             rc        = DBMAPI_OK_CN14;
  const char    * pPayLoad  = NULL;
  tsp00_Int4        nReplyLen = 0;
  tsp00_ErrTextc  szError;
  tsp00_Int4        nError    = 0;
  int             nIgnore   = 0;
  const char    * pCurrent  = NULL;
  bool            bRelease  = false;

  data = new char[options->nMaxData];

  if (data == NULL) {
    cn12FatalError (options, TXT_ERR_MEMORY);
  } // end if

  do {
    SAPDB_strcpy(data, CMD_NEXT);
    memset(data + strlen(data), 0, options->nMaxData - strlen(data));
    rc = cn12CServerCommand(options, data, &replyData, true, false, bRelease);

    if (rc == DBMAPI_OK_CN14) {
      rc = cn14analyzeDbmData(replyData, (int) strlen(replyData) +1, (const void **) &pPayLoad,
                              &nReplyLen, &nError, szError);
    } // end if

    if ((rc == DBMAPI_OK_CN14) && (pPayLoad != NULL)) {

      // read the next command
      pCurrent = strchr(pPayLoad, '\n');
      if (pCurrent != NULL) {

        strncpy(data, pPayLoad, pCurrent - pPayLoad);
        data[pCurrent - pPayLoad] = '\0';

        // read the lines to ignore
        ++pCurrent;
        nIgnore = atoi(pCurrent);

        // send the next command (don't print the answer)
        memset(data + strlen(data), 0, options->nMaxData - strlen(data));
        rc = cn12CServerCommand(options, data, &replyData, true, false, bRelease);
        if (rc == DBMAPI_OK_CN14) {
          rc = cn14analyzeDbmData(replyData, (int) strlen(replyData) +1, (const void **) &pPayLoad,
                                  &nReplyLen, &nError, szError);
        } // end if

        if ((rc == DBMAPI_OK_CN14) && (pPayLoad != NULL)) {
          // skip the first lines
          pCurrent = pPayLoad;
          while ((pCurrent != NULL) && (nIgnore > 0)) {
            pCurrent = strchr(pCurrent, '\n');
            if (pCurrent != NULL) {
              ++pCurrent;
              --nIgnore;
            } // end if
          } // end while

          // print the answer
          if (pCurrent != NULL) {
            cn13fputs(options->outfile, pCurrent, IS_UNICODE(options), nReplyLen - (int) (pCurrent - pPayLoad) );
            cn12PrintProtocol(options, data, pCurrent, nReplyLen - (int) (pCurrent - pPayLoad) );
          } // end if

        } // end if
      } // end if

    } // end if

  } while ((rc == DBMAPI_OK_CN14) && (pCurrent != NULL));

} /* end cn12Next */

/*
  -----------------------------------------------------------------------------
  function: cn12Logon
  -----------------------------------------------------------------------------
*/
static int cn12Logon
      ( tcn12Options       * options,
        char               * data,
        const char         * szUserOS,
        const char         * command,
        char              ** replyData )
{
  tsp00_Namec   pwClear;
  tsp00_C64c    pwCrypt;
  const char  * pPassword;
  int           nLen;
  bool          bRelease  = false;

  if (szUserOS != NULL && strlen(szUserOS) > 0) {
    pPassword = strchr (szUserOS, ',');
    if (pPassword != NULL) {
        nLen = (int)(pPassword - szUserOS);
        pPassword++;
    } else {
        nLen = (int)strlen (szUserOS);
        pPassword = "";
    } /* end if */

    cn90ClearStringToCryptString(pwClear.rawAssign(pPassword), pwCrypt);
    sprintf (data, "%s %.*s,%s", command, nLen, szUserOS, (char *) pwCrypt);

  } else {
    /* autologon */
    sprintf (data, "%s", command);
  } /* end if */

  /* send logon command */
  return cn12CServerCommand (options, data, replyData, true, false, bRelease);
} /* end cn12Logon */

/*
  -----------------------------------------------------------------------------
  function: main
  -----------------------------------------------------------------------------
*/
#ifdef _WIN32
  int wmain (int givenArgc, wchar_t * argU[])
#else
  int main (int givenArgc, char     * argU[])
#endif
{
  static tcn12Options        options;
  int                 rc           = DBMAPI_OK_CN14;
  int                 nSysRc       = 0;
  int                 linelen      = 0;
  int                 concat       = true;
  int                 i            = 0;
  char *              data         = NULL;
  char *              replyData    = NULL;
  char *              bufpos       = NULL;
  char *              tmp          = NULL;
  long                nExit        = 0;
  bool                bRelease  = false;

  char **             givenArgv    = cn12ConvertArguments(&options, givenArgc, argU);
  int                 argc;
  char **             argv;

  nExit = Tools_PipeCall::ReadArguments(givenArgc, givenArgv, argc, argv);

/***** Memory Debug BEGIN *****/
#ifdef BVMEMDEBUG
  _CrtMemState s1, s2, s3;

  _CrtMemCheckpoint( &s1 );

  _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
  _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
  _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
  _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
  _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
  _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
#endif
/***** Memory Debug END *****/

  g_pOptions = &options;

  SAPDB_strcpy(c_component, C_COMPONENT);
  sqlinit (c_component, NULL);

  cn12ParseOptions (&options, argc, argv);

  // save user to xuser
  if (options.szSaveUser != NULL ) {
    cn12SaveUsers(&options);
  } // end if

  // delete user from xuser
  if (options.bDelUser ) {
    cn12DeleteUsers(&options);
  } // end if

  // list users from xuser
  if (options.bListUser ) {
    cn12ListUsers(&options);
  } // end if

  cn12CServerConnect (&options);

  if (!IS_UNICODE(&options)) {
    cn12Reconvert(&options, argc, argv);
  } // end if

  data = new char[options.nMaxData];

  if (data == NULL) {
      cn12FatalError (&options, TXT_ERR_MEMORY);
  } /* end if */

  // show version
  if (options.showServerVersion) {
      SAPDB_strcpy (data, CMD_VERSION);
      rc = cn12CServerCommand (&options, data, &replyData, false, false, bRelease);
      nExit = cn12CheckAnswer(replyData);
  } /* end if */

  // user logon
  if (options.szUser != NULL && rc == DBMAPI_OK_CN14 && nExit == 0 && options.bStaticCServer) {
    if (strlen(options.dbname) != 0) {
       cn90StrUpperCopy((char *)options.szUser, options.szUser, false);
       rc = cn12Logon (&options, data, options.szUser, CMD_USER_LOGON, &replyData);
    } else {
       rc = cn12Logon (&options, data, options.szUser, CMD_SYSTEM_LOGON, &replyData);
    } // end if
    nExit = cn12CheckAnswer(replyData);
  } /* end if */

  // utility connect
  if (options.szUserUTL != NULL && rc == DBMAPI_OK_CN14 && nExit == 0) {
      rc = cn12Logon (&options, data, options.szUserUTL, CMD_UTIL_CONNECT, &replyData);
      nExit = cn12CheckAnswer(replyData);
  } /* end if */

  // sql connect
  if (options.szUserSQL != NULL && rc == DBMAPI_OK_CN14 && nExit == 0) {
      rc = cn12Logon (&options, data, options.szUserSQL, CMD_SQL_CONNECT, &replyData);
      nExit = cn12CheckAnswer(replyData);
  } /* end if */

  // service connect
  if (options.bService != 0 && rc == DBMAPI_OK_CN14 && nExit == 0) {
      rc = cn12Logon (&options, data, "", CMD_SRV_CONNECT, &replyData);
      nExit = cn12CheckAnswer(replyData);
  } /* end if */

  // SAP User
  if (options.bSAP != 0 && rc == DBMAPI_OK_CN14 && nExit == 0) {
      tsp00_C64c     szUser;
      tsp00_ErrTextc szErrtext;
      tsp00_C8c      szSysId;
      tsp00_C30c     szConn;
      tsp00_Pathc    szProfile;

      rc = cn14GetTPUser(TP_USERMODE_SAP_CN14,
                         szSysId.rawAssign(options.szSysId),
                         szConn.rawAssign(options.szConn),
                         szProfile.rawAssign(options.szProfile),
                         szUser, szErrtext);
      if (rc == DBMAPI_OK_CN14) {
        rc = cn12Logon (&options, data, szUser, CMD_USER_SAP, &replyData);
        nExit = cn12CheckAnswer(replyData);
      } else {
        cn12FatalError (&options, szErrtext);
      } // end if
  } /* end if */


  // setup signal managment
  signal(SIGINT,  cn12Aborthandler);
  signal(SIGTERM, cn12Aborthandler);
  signal(SIGABRT, cn12Aborthandler);
#if defined (_WIN32)
  signal(SIGBREAK, cn12Aborthandler);
#endif

  if (options.cmdArgStart != undef_ceo00 && rc == DBMAPI_OK_CN14 && nExit == 0) {

    /* read command from command line */
    tmp = data;
    for (i = options.cmdArgStart; i < argc; ++i) {
      sprintf (tmp, "%s ", argv [i]);
      tmp += strlen (tmp);
    } /* end for */

    rc = cn12CServerCommand (&options, data, &replyData, false, true, bRelease);
    nExit = cn12CheckAnswer(replyData);
    if (options.bNohold && (rc == DBMAPI_OK_CN14) && (nExit == 0)) {
      cn12Next(&options);
    } // end if

  } else if ( rc == DBMAPI_OK_CN14 && nExit == 0) {

    // Prompt
    if (options.infile == stdin) {
      cn13fputs(options.outfile, options.application, IS_UNICODE(&options));
      cn13fputs(options.outfile, options.node[0] != 0  || options.dbname[0] != 0 ? " on " : "", IS_UNICODE(&options));
      cn13fputs(options.outfile, options.node, IS_UNICODE(&options));
      cn13fputs(options.outfile, options.node[0] != 0  && options.dbname[0] != 0 ? " : " : "", IS_UNICODE(&options));
      cn13fputs(options.outfile, options.dbname, IS_UNICODE(&options));
      cn13fputs(options.outfile, ">", IS_UNICODE(&options));
    } // end if

    // read command(s) from file (e.g. stdin)
    while (!bRelease                                                 &&
           (rc == DBMAPI_OK_CN14)                                    &&
           (nExit == 0)                                                 ) {

      if (cn13fgets (data, options.nMaxData, options.infile, IS_UNICODE(&options)) == NULL) {
        if ((options.infile != stdin) && options.bSession) {
          options.infile = stdin;
          // Prompt
          cn13fputs(options.outfile, options.application, IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node[0] != 0  || options.dbname[0] != 0 ? " on " : "", IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node, IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node[0] != 0  && options.dbname[0] != 0 ? " : " : "", IS_UNICODE(&options));
          cn13fputs(options.outfile, options.dbname, IS_UNICODE(&options));
          cn13fputs(options.outfile, ">", IS_UNICODE(&options));
          if (cn13fgets (data, options.nMaxData, options.infile, IS_UNICODE(&options)) == NULL) {
            break;
          } // end if
        } else {
          break;
        } // end if
      } // end if

      linelen = (int)strlen (data);

      concat = ((linelen > 1) && (data [linelen-2] == '/'));
      while  (concat) {
        --linelen;               /* throw away '/' */
        data [linelen-1] = '\n'; /* keep new line character */
        bufpos = data + linelen;
        if ((cn13fgets (bufpos, options.nMaxData, options.infile, IS_UNICODE(&options)) != NULL)) {
          linelen = (int)strlen (data);
          concat = ((linelen > 1) && (data [linelen-2] == '/'));
        } else {
          concat = false;
        } /* end if */
      } /* end while */

      if (data [linelen - 1] == '\n') {
        --linelen; /* remove new line character */
        data [linelen] = '\0';
      } /* end if */

      if ((options.infile != stdin) || (options.outfile != stdout)) {
        // write line to protocol file
        cn13fputs(options.outfile, ">", IS_UNICODE(&options));
        cn13fputs(options.outfile, data, IS_UNICODE(&options));
        cn13fputs(options.outfile, "\n", IS_UNICODE(&options));
      } /* end if */

      // comment or empty line
      if ((linelen == 0) || (data [0] == '#')) {
        continue;
      } /* end if */

      if (data [0] == '!') {
        /* execute command in local shell */
        nSysRc = system (data + 1); /* ignore ! */
        char szBuffer[20];
        sprintf(szBuffer, "%d", nSysRc);
        cn13fputs(options.outfile, "'", IS_UNICODE(&options));
        cn13fputs(options.outfile, data + 1, IS_UNICODE(&options));
        cn13fputs(options.outfile, "' returned with ", IS_UNICODE(&options));
        cn13fputs(options.outfile, szBuffer, IS_UNICODE(&options));
        cn13fputs(options.outfile, EndOfCommandC, IS_UNICODE(&options));

        // Prompt
        if (options.infile == stdin) {
          cn13fputs(options.outfile, options.application, IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node[0] != 0  || options.dbname[0] != 0 ? " on " : "", IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node, IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node[0] != 0  && options.dbname[0] != 0 ? " : " : "", IS_UNICODE(&options));
          cn13fputs(options.outfile, options.dbname, IS_UNICODE(&options));
          cn13fputs(options.outfile, ">", IS_UNICODE(&options));
        } /* end if */
      } else {

        if (data [0] == '<') {
          linelen = cn12ReadHereArg (&options, data, linelen);
        } /* end if */

        rc = cn12CServerCommand (&options, data, &replyData, false, true, bRelease);
        nExit = cn12CheckAnswer(replyData);
        if (options.bNohold && (rc == DBMAPI_OK_CN14) && (nExit == 0) && !bRelease) {
          cn12Next(&options);
        } // end if
        nExit = 0;

        cn13fputs(options.outfile, EndOfCommandC, IS_UNICODE(&options));
        if (rc == DBMAPI_OK_CN14 && options.infile == stdin && !bRelease) {
          // Prompt
          cn13fputs(options.outfile, options.application, IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node[0] != 0  || options.dbname[0] != 0 ? " on " : "", IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node, IS_UNICODE(&options));
          cn13fputs(options.outfile, options.node[0] != 0  && options.dbname[0] != 0 ? " : " : "", IS_UNICODE(&options));
          cn13fputs(options.outfile, options.dbname, IS_UNICODE(&options));
          cn13fputs(options.outfile, ">", IS_UNICODE(&options));
        } // end if
        if (rc == DBMAPI_OK_CN14 && options.infile != stdin && !options.bContinue && !bRelease) {
          nExit = cn12CheckAnswer(replyData);
        } /* end if */
      } /* end if */
    } /* end while */
  } /* end if */

  // disconnect
  cn12CServerDisconnect (&options);

  // close open files
  if (options.infile != stdin) {
    fclose (options.infile);
  } /* end if */
  if (options.outfile != stdout) {
    fclose (options.outfile);
  } /* end if */
  if (options.hProtocol != NULL) {
    fclose (options.hProtocol);
  } /* end if */

  delete data;

  nExit = (nExit != 0) ? DBM_ERROR : nExit;
  nExit = (rc != 0 && nExit == 0) ? COMM_ERROR : nExit;

    if (!bRelease && options.bKey && options.infile == stdin) {
        printf("Press any key to continue . . .");
        getchar();
    } // end if

/***** Memory Debug BEGIN *****/
#ifdef BVMEMDEBUG
    _CrtMemCheckpoint( &s2 );

    if ( _CrtMemDifference( &s3, &s1, &s2 ) ) {
      printf("\n***\n");
      _CrtMemDumpAllObjectsSince( &s1 );
      printf("\n***\n");
      _CrtMemDumpStatistics( &s3 );
      printf("\n***\n");
    }

#endif
/***** Memory Debug END *****/

  return nExit;
} /* end main */
