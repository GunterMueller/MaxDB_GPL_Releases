/*!
  -----------------------------------------------------------------------------

  module: vcn12dbm.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: Database Manager

  description:  Special local access to dbm

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "heo01.h"
#include "hsp02.h"
#include "hsp77.h"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMSrv.h"

#ifndef _WIN32
  #include "RunTime/RTE_UNIXAuthenticate.hpp" // nocheck
  #include "RunTime/RTE_saveUNIXcalls.h"      // nocheck
#endif

#define XPARAM_NAMES_ONLY
#include "geo002.h"

#ifdef _WIN32
  #define TUEDDELCHEN "\""
  #define DBMCLI      "pgm\\dbmcli"
#else
  #define TUEDDELCHEN 
  #define DBMCLI      "bin/dbmcli"
#endif

#ifdef DBMSTART
  #define DBM_COMMAND TUEDDELCHEN"%s"DBMCLI""TUEDDELCHEN" %.0s -d %s -u %s,%s db_online"
  #define DBM_LOGO    "Starter"
#endif

#ifdef DBMSTOP
  #define DBM_COMMAND TUEDDELCHEN"%s"DBMCLI""TUEDDELCHEN" %.0s -d %s -u %s,%s db_offline -nc"
  #define DBM_LOGO    "Stopper"
#endif

#ifdef DBMUNREG
  #define DBM_COMMAND TUEDDELCHEN"%s"DBMCLI""TUEDDELCHEN" %.0s -d %s -u %s,%s db_unreg"
  #define DBM_LOGO    "Unregisterer"
#endif

#ifdef DBMREG
  #define DBM_COMMAND TUEDDELCHEN"%s"DBMCLI""TUEDDELCHEN" -R "TUEDDELCHEN"%s"TUEDDELCHEN" db_reg DB %s %s,%s"
  #define DBM_LOGO    "Registerer"
#endif

void reportError(const char * pText, const Msg_List * pMsg)
{
  SAPDB_Char  szBuffer[1000+1];
  SAPDB_UInt4 nLength;

  printf("Error: %s\n", (pText != NULL) ? pText : "");
  while((pMsg != NULL) && (!pMsg->IsEmpty())) {
    pMsg->MessageWithInsertedArguments( 1000, szBuffer, nLength);
    printf("%s\n", szBuffer);
    pMsg = pMsg->NextMessage();
  } // end if
  printf("\n");
  exit(1);
} // end reportError


void logo () 
{
    printf("The <PRODUCT_NAME> Database "DBM_LOGO", Version <MAJOR_VERSION>.<MINOR_VERSION>.<FORMATED_CORRECTION_LEVEL>.<FORMATED_BUILD_PREFIX>\n");
    printf("<COPYRIGHT_STRING>\n\n");
} // end logo

void usage (const char * pProgram) 
{
    printf("usage: %s [-d] <dbname>\n", pProgram);
    exit(1);
} // end usage

int main(int argc, char * argv[]) 
{
    logo();

    if ((argc < 2) || (argc > 3)) {
        usage(argv[0]);
    } // end if

    if ((argc == 3) && (strcmp(argv[1], "-d") != 0)) {
        usage(argv[0]);
    } // end if

    if ((argc == 2) && (argv[1][0] == '-')) {
        usage(argv[0]);
    } // end if

    Msg_List                         oMsg;
    Msg_List                         oMsg2;

#ifndef _WIN32
    // Become session leader [Create own processgroup (prevents against kill(0,SIGKILL))]
    if ( getsid((pid_t)0) != getpid() )
        setsid ();
    // close all file handles except stdin, stdout and stderr
    for (int fd = 3; fd < FOPEN_MAX; ++fd)
        close(fd);
    // check for sapdd access
    if (!RTE_VerifyAsSapdbAdministrator(oMsg) && !RTE_IsSapdbOwnerOrRoot(oMsg2)) {
        oMsg2.Overrule(oMsg);
        reportError("Access denied for this operation", &oMsg2);
    } // end if

#endif

    // extract dbroot of database
    tsp00_Pathc    szDBRoot;
    tsp00_Pathc    szModule;
    tsp01_RteError oRteError;
    memset(&oRteError, 0, sizeof(oRteError));
    if (!sqlGetMyModuleFilename ( argv[0], szModule.asCharp(), szModule.size(), &oRteError)) {
        Msg_List oMsg(Msg_List::Error, SDBMSG_DBMSRV_INTERNAL_TRANSPORT_MSG, Msg_Arg(SDBMSGTAG_DBMSRV_INTERNAL_TRANSPORT_MSG__TAG, oRteError.OsErrText));
        oMsg.Overrule(Msg_List(Msg_List::Error, SDBMSG_DBMSRV_INTERNAL_TRANSPORT_MSG, Msg_Arg(SDBMSGTAG_DBMSRV_INTERNAL_TRANSPORT_MSG__TAG, oRteError.RteErrText)));
        reportError("Can not determine software path for database instance", &oMsg);
    } else {
        sqlGetInstallationPathFromExecPath ( szModule, szDBRoot, false);

#ifndef _WIN32
        // because of UNIX may use links we have to check our path against the available installations

        // stat(...) from given dbroot
        struct stat stat1;
        struct stat stat2;
        if (RTE_save_stat(szDBRoot,&stat1) != 0) {
            reportError("Can not determine software path for database instance", NULL);
        } // end if

        // iterate trough the installations
        Container_List<RTE_Installation>::Iterator  iterator = ((RTE_DBRegister::Instance()).GetInstallations())->Begin();
        while (iterator != 0) {
            // stat(...) from given installations dbroot
            if (RTE_save_stat(iterator->GetDBRoot(),&stat2) == 0) {
                // compare both
                if (S_ISDIR( stat1.st_mode ) && S_ISDIR( stat2.st_mode ) && (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino)) {
                    // yeah that's me
                    szDBRoot.rawAssign(iterator->GetDBRoot());
                    break;
                } // end if
            } // end if
            ++iterator;
        } // end while
#endif

    } // end if

    // prepare the parameter file for reading
    RTEConf_Parameter  oConfigFile(argv[argc-1], 0, RTECONF_MAXNAMELENGTH, RTECONF_MAXSTRINGLENGTH);

    RTEConf_Parameter::StringElement szUser[RTECONF_MAXSTRINGLENGTH];   
    RTEConf_Parameter::CryptInfo     oPassword;
    SAPDB_Bool                       bFileExist  = false;

    // open the parameter file
    if (!oConfigFile.Read(bFileExist, oMsg))
        reportError("Can not read parameters", &oMsg);

    // read username
    if (!oConfigFile.GetValue ((RTEConf_Parameter::Name) PAN_CONTROLID, szUser, oMsg))
        reportError("Can not read user name", &oMsg);

    // read password
    if (!oConfigFile.GetValue ((RTEConf_Parameter::Name) PAN_CONTROLPW, oPassword, oMsg))
        reportError("Can not read password", &oMsg);

    // uncrypt password
    tsp00_Name    pwClear;
    tsp00_Namec   pwClearc;
    tsp00_CryptPw pwCrypt;

    memcpy(&(pwCrypt[0]), oPassword, sizeof(pwCrypt));

    s02decrypt(pwClear, pwCrypt);
    pwClearc.p2c(pwClear);


    tsp00_Pathc         szPath;
    tsp01_RteError      RteError;
  
    // read independent program path
    if (!sqlGetIndependentProgramsPath ( szPath, TERM_WITH_DELIMITER_EO01, &RteError))
        reportError("Can not read program path", NULL);

    // generate command
    char szBuffer[1000+1];

    sp77sprintf(szBuffer, 1000,
                DBM_COMMAND, 
                szPath.asCharp(),
                szDBRoot.asCharp(),
                argv[argc-1],
                szUser, 
                pwClearc.asCharp());

    // call dbmcli
    if (Tools_PipeCall::CallProgram(szBuffer, Tools_PipeCall::CallSynchron) != 0) {
        char szOutput[1000+1];
        sp77sprintf(szOutput, 1000,
                    DBM_COMMAND, 
                    szPath.asCharp(),
                    szDBRoot.asCharp(),
                    argv[argc-1],
                    "*", 
                    "*");
        Msg_List oMsg(Msg_List::Error, SDBMSG_DBMSRV_INTERNAL_TRANSPORT_MSG, Msg_Arg(SDBMSGTAG_DBMSRV_INTERNAL_TRANSPORT_MSG__TAG, Tools_PipeCall::ErrorReason()));
        oMsg.Overrule(Msg_List(Msg_List::Error, SDBMSG_DBMSRV_INTERNAL_TRANSPORT_MSG, Msg_Arg(SDBMSGTAG_DBMSRV_INTERNAL_TRANSPORT_MSG__TAG, szOutput)));
        reportError("Error while calling dbmcli", &oMsg);
    } // end if

} // end main

