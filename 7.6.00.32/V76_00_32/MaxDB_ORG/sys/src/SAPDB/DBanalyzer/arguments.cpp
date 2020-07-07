/*!
  -----------------------------------------------------------------------------
  module: arguments.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR BerndV
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  handle the commandline arguments for dbanalyzer
  version:      7.4 and higher 
  -----------------------------------------------------------------------------

  Copyright (c) 1998-2005 SAP AG


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




  -----------------------------------------------------------------------------
*/

#include "heo06.h"
#define VSP100_CPP
#include "hsp100.h"
#include "SAPDB/DBanalyzer/arguments.hpp"
#include "SAPDB/DBanalyzer/error.hpp"

extern DBA_error gError;

DBA_arguments::DBA_arguments (DBA_int nCount, DBA_char *sArgs[]) 
{
  Clear();
  m_bOK = Parse(nCount, sArgs);
}

DBA_arguments::~DBA_arguments () 
{
}

//
// Clear
//
// initialize all variables
//
DBA_void DBA_arguments::Clear() 
{
  m_sApplication = "";
  m_sServer      = "";
  m_sDatabase    = "";
  m_sUser        = "";
  m_sPassword    = "";
  m_sInterval    = "";
  m_sNumber      = "";
  m_sConsole     = "";
  m_sConfigFile  = "";
  m_sOutputDir   = "";
  m_bUsage       = false;
  m_bNoLogo      = false;
  m_bIgnore      = false;
  m_bNoEnv       = false;
  m_bStop        = false;
  m_bReconnect   = false;
  m_bState       = false;
  m_bDelete      = false;
  m_sDelete      = "";
  m_bVersion     = false;
}

//
// Parse
//
// parse the program arguments 
//
DBA_bool DBA_arguments::Parse (DBA_int nCount, DBA_char *sArgs[]) 
{
  if (nCount <= 0) {
    gError.SetError(DBA_ERR_ARGS);
    gError.SetExtText("To few arguments!");
    return false;
  } // end if

  m_sApplication = sArgs[0];

  DBA_int    nArg   = 1;
  DBA_string sArg;
  DBA_bool   bError = false;

  while (nArg < nCount) {
    sArg = sArgs[nArg];

    if (sArg == DBA_ARGTXT_SRV) {                 // -n <server>
      nArg++;
      if (nArg < nCount) {
        m_sServer = sArgs[nArg];
        nArg++;
      } else {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (node name) for option -n!");
        break;
      }
    } else if (sArg == DBA_ARGTXT_DB) {           // -d <database>
      nArg++;
      if (nArg < nCount) {
        m_sDatabase = sArgs[nArg];
        m_sDatabase.ToUpper();
        nArg++;
      } else {
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (database name) for option -d!");
        bError = true;
        break;
      }
    } else if (sArg == DBA_ARGTXT_USR) {          // -u <user,pwd>
      nArg++;
      if (nArg < nCount) {
        sArg = sArgs[nArg];
        DBA_int n = sArg.FindFirstOf(',');
        if (n >= 0) {
          m_sUser = sArg.SubStr(0, n).Trim();
          m_sPassword = sArg.SubStr(n+1).Trim();
        } else {
          m_sUser = sArg;
          m_sPassword = "";
        }
        nArg++;
      } else {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (user/password) for option -u!");
        break;
      }
    } else if (sArg == DBA_ARGTXT_INT) {          // -t <interval>,<number>
      nArg++;
      if (nArg < nCount) {
        sArg = sArgs[nArg];
        DBA_int n = sArg.FindFirstOf(',');
        if (n >= 0) {
          m_sInterval = sArg.SubStr(0, n).Trim();
          m_sNumber = sArg.SubStr(n+1).Trim();
        } else {
          m_sInterval = sArg;
          m_sNumber = "";
        }
        nArg++;
      } else {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (intervall/number) for option -t!");
        break;
      }
    } else if (sArg == DBA_ARGTXT_CON) {          // -c <outputlevel>
      nArg++;
      if (nArg < nCount) {
        m_sConsole = sArgs[nArg];
        nArg++;
      } else {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (output level) for option -c!");
        break;
      }
    } else if (sArg == DBA_ARGTXT_CFG) {          // -f <configfile>
      nArg++;
      if (nArg < nCount) {
        m_sConfigFile = sArgs[nArg];
        nArg++;
      } else {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (configuration file) for option -f!");
        break;
      }
    } else if (sArg == DBA_ARGTXT_OUT) {          // -o <ouputdir>
      nArg++;
      if (nArg < nCount) {
        m_sOutputDir = sArgs[nArg];
        nArg++;
      } else {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (output directory) for option -o!");
        break;
      }
    } else if (sArg == DBA_ARGTXT_USG) {          // -?
      m_bUsage = true;
      nArg++;
    } else if (sArg == DBA_ARGTXT_IGN) {          // -i
      m_bIgnore = true;
      nArg++;
    } else if (sArg == DBA_ARGTXT_NOL) {          // -nologo
      m_bNoLogo = true;
      nArg++;
    } else if (sArg == DBA_ARGTXT_NOE) {          // -noenv
      m_bNoEnv = true;
      nArg++;
    } else if (sArg == DBA_ARGTXT_STO) {          // -stop
      m_bStop = true;
      nArg++;
    } else if (sArg == DBA_ARGTXT_REC) {         // -reconnect
      m_bReconnect = true;
      nArg++;
    } else if (sArg == DBA_ARGTXT_STA) {         // -state
      m_bState = true;
      nArg++;
    } else if (sArg == DBA_ARGTXT_DEL) {         // -delete <date>
      m_bDelete = true;
      nArg++;
      if (nArg < nCount) {
        m_sDelete = sArgs[nArg];
        nArg++;
      } else {
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing parameter (date) for option -delete!");
        bError = true;
        break;
      }
    } else if (sArg == DBA_ARGTXT_VER) {         // -V
      m_bVersion = true;
      nArg++;
    } else {
      bError = true;
      gError.SetError(DBA_ERR_ARGS);
      gError.SetExtText("Unknown option!");
      break;
    } // end if
  } // end while

  if (!m_bNoLogo) {
    Logo();
  } // end if
  if (m_bVersion) {
    Version();
  } // end if


  if (!bError) {

    if ((m_sDatabase.Empty() ||
         m_sUser.Empty()     ||
         m_sPassword.Empty()    ) && !m_bUsage) {
      if (m_sDatabase.Empty()) {
        m_bUsage = ReadValue(m_sDatabase, "Enter database name: ", false);
      } // end if
      if (m_sUser.Empty()) {
        m_bUsage = ReadValue(m_sUser, "Enter user name: ", false);
      } // end if
      if (m_sPassword.Empty()) {
        m_bUsage = ReadValue(m_sPassword, "Enter password: ", true);
      } // end if
      printf("\n");
    } // end if

    if (!m_bUsage) {
      if (m_sDatabase.Empty() ||
          m_sUser.Empty() ||
          m_sPassword.Empty()) {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing databasename and/or user data!");
      }
    }

  } // end if

  if (!bError) {

    if (!m_bUsage && !m_bVersion) {
      if (!m_sServer.Empty() &&
          m_sOutputDir.Empty()) {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Missing output directory (option -o)!");
      }
    }

  } // end if


  if (!bError) {
    
    if (!m_bUsage && !m_bVersion) {
      if (m_bStop && m_bState) {
        bError = true;
        gError.SetError(DBA_ERR_ARGS);
        gError.SetExtText("Do not mix -stop and -state!");
      }
    }

  } // end if

  if (bError && !m_bVersion) {
    m_bVersion = false;
    m_bUsage   = true;
    return false;
  } // end if

  if (!bError) {
    m_bVersion = false;
  } // end if

  if (m_bUsage) {
    m_bVersion = false;
  } // end if

  return true;
}

//
// Version
//
// show the usage information
//
DBA_void DBA_arguments::Version() 
{
  tsp00_Versionc VersionString;
  sp100_GetVersionString  ( "Analyzer", s100buildnumber, VersionString );
  printf("%s\n\n", VersionString.asCharp());
} // DBA_arguments::Version

//
// Usage
//
// show the usage information
//
DBA_void DBA_arguments::Usage() 
{
  printf("Usage:\n\n");
  printf("  dbanalyzer -n <server> -d <database> -u <user,pwd>\n");
  printf("             -f <configfile> -t <interval>,<number>\n");
  printf("             -o <outputdir> -c <level> -i -nologo\n");
  printf("             -noenv -stop -state -reconnect\n\n");
  printf("  -n <server>       Name of the server on which the database instance\n");
  printf("                    is running\n");
  printf("                    If you specify -n, you must also specify -o.\n");
  printf("  -d <database>     Name of the database instance\n");
  printf("  -u <user,pwd>     User name and password\n");
  printf("  -f <configfile>   Name of the configuration file\n");
  printf("  -t <interval>,    Time interval (in seconds) between two evaluations and\n");
  printf("     <number>       number of desired evaluations\n");
  printf("  -o <outputdir>    Directory for the protocol files\n");
  printf("                    Default directory is the working directory of the\n");
  printf("                    database instance.\n");
  printf("  -c <level>        Specifies on a scale of 1 (low) to 4 (high) how much\n");
  printf("                    additional information should appear on the console\n");
  printf("  -i                Deletes protocol files that already exist\n");
  printf("  -nologo           Suppresses display of the logo\n");
  printf("  -noenv            Suppresses logging and display of the environment variables\n");
  printf("                    from the configuration file\n");
  printf("  -stop             Stops a running Database Analyzer\n");
  printf("  -state            Shows the state of a running Database Analayzer\n");
  printf("  -reconnect        Reconnects after a restart of the database instance\n");
  printf("  -V                Prints version information\n");
  printf("  -delete <date>    Deletes old protocols (format YYYYMMDD)\n");
}

//
// Logo
//
// show the logo lines
//
DBA_void DBA_arguments::Logo() 
{
  printf("<PRODUCT_NAME> Database Analyzer, The Performance Analysis Tool, Version <MAJOR_VERSION>.<MINOR_VERSION>.<FORMATED_CORRECTION_LEVEL>.<FORMATED_BUILD_PREFIX>\n");
  printf("<COPYRIGHT_STRING>\n\n");
}

//
// ReadDatabase
//
// read a value from stdin
//
DBA_bool DBA_arguments::ReadValue(DBA_string & sValue, DBA_string sPrompt, DBA_bool bHidden) 
{

  if (!bHidden) {
    char pBuffer[101]= "";
    fputs(sPrompt.CharPtr(), stdout);
    fgets(pBuffer, 100, stdin);
    sValue = pBuffer;
  } else {
    tsp05_RteFileError rteError;
    tsp00_Namec        sName;
    if(sqlgetpass(sPrompt.CharPtr(), sName, rteError)) {
      sValue = sName.asCharp();
    } // end if
  } // end if

  sValue.Trim();
  return sValue.Empty();
} // end ReadValue
