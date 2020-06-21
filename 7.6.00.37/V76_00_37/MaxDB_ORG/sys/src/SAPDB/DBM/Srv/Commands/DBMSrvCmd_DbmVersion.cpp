/*!
  \file    DBMSrvCmd_DbmVersion.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command dbm_version

\if EMIT_LICENCE

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


\endif
*/

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "hsp100.h"

#include "ToolsCommon/Tools_System.hpp"

#include "SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbmVersion.hpp"


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_DbmVersion
//-----------------------------------------------------------------------------

#define KEYVER_VERSION          "VERSION"
#define KEYVER_BUILD            "BUILD"
#define KEYVER_OS               "OS"
#define KEYVER_DBROOT           "INSTROOT"
#define KEYVER_LOGON            "LOGON"
#define KEYVER_CODE             "CODE"
#define KEYVER_SWAP             "SWAP"
#define KEYVER_UNICODE          "UNICODE"
#define KEYVER_INSTANCE         "INSTANCE"
#define KEYVER_SYSNAME          "SYSNAME"
#define KEYVER_LICENSE          "LICENSE"

#define VER_SEPARATOR           " = "
#define VER_MAXKEYLEN           10

#define VERVAL_SWAP_NORMAL      "normal"
#define VERVAL_SWAP_FULL        "full"
#define VERVAL_SWAP_PART        "part"

#define VERVAL_CODE_ASCII       "ASCII"
#define VERVAL_CODE_EBCDIC      "EBCDIC"
#define VERVAL_CODE_UTF8        "UTF8"

#define VERVAL_YES              "YES"
#define VERVAL_NO               "NO"
#define VERVAL_UNKNOWN          "(unknown)"

#define VERVAL_WIN32            "WIN32"
#define VERVAL_WIN64            "WIN64"
#define VERVAL_UNIX             "UNIX"
#define VERVAL_UTF8             "UTF8"
#define VERVAL_ASCII            "ASCII"
#define VERVAL_COMMERCIAL       "Commercial"
#define VERVAL_FREE             "Free"

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmd_DbmVersion::DBMSrvCmd_DbmVersion
//-----------------------------------------------------------------------------
DBMSrvCmd_DbmVersion::DBMSrvCmd_DbmVersion()
    : DBMSrv_Command( DBMSrv_Command::KeyDbmVersion, false, m_LongHelp ) 
{
} // end DBMSrvCmd_DbmVersion::DBMSrvCmd_DbmVersion

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmd_DbmVersion::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmd_DbmVersion::m_LongHelp=
    "@command dbm_version You display the version information of the Database Manager. Since the version "
             "of the DBM Server is always the same as the version of the database software, this is how "
             "you can also determine the version of the database software." 
    "@preconditions You do not need to log on to the Database Manager to execute this DBM command. "
                   "Nor do you require any server authorization."
    "@syntax dbm_version [<value>]"
    "@param  <value> You display only the requested information. Possible values are:"
        "@value VERSION    \"\"  Version number of the Database Manager"
        "@value BUILD      \"\"  Identification number of the Database Manager" 
        "@value OS         \"\"  Name of the operating system"
        "@value INSTROOT   \"\"  Installation directory of the version-dependent components"
        "@value LOGON      \"\"  Displays whether a logon to the operating system has been performed: "
                                "True (logged on) or False (not logged on)"
        "@value CODE       \"\"  Display of the character set used in the Database Manager: ASCII or UTF8"
        "@value SWAP       \"\"  Displays of the swap type (internal representation of numerical values) used "
                                "in the Database Manager: no (hihi-hilo-lohi-lolo), full (lolo-lohi-hilo-hihi) "
                                "or half (lohi-lolo-hihi-hilo)"
        "@value UNICODE    \"\"  Configuration of the current database instance as a UNICODE database"
        "@value INSTANCE   \"\"  Database instance type: OLTP, LVC or ARCHIVE"
        "@value SYSNAME    \"\"  Name of the operating system"
    "@reply OK<NL>"
           "VERSION    = <version><NL>"
           "BUILD      = <build_number><NL>"
           "OS         = <os><NL>"
           "INSTROOT   = <dependent_path><NL>"
           "LOGON      = <logon_state><NL>"
           "CODE       = <code><NL>"
           "SWAP       = <swap><NL>"
           "UNICODE    = (YES|NO)<NL>"
           "INSTANCE   = (OLTP|LVC|ARCHIVE)<NL>"
           "SYSNAME    = <os><NL>"
        "@replyvalue VERSION     Version number of the Database Manager"
        "@replyvalue BUILD       Identification number of the Database Manager" 
        "@replyvalue OS          Name of the operating system"
        "@replyvalue INSTROOT    Installation directory of the version-dependent components"
        "@replyvalue LOGON       Displays whether a logon to the operating system has been performed: "
                                "True (logged on) or False (not logged on)"
        "@replyvalue CODE        Display of the character set used in the Database Manager: ASCII or UTF8"
        "@replyvalue SWAP        Displays of the swap type (internal representation of numerical values) used "
                                "in the Database Manager: no (hihi-hilo-lohi-lolo), full (lolo-lohi-hilo-hihi) "
                                "or half (lohi-lolo-hihi-hilo)"
        "@replyvalue UNICODE     Configuration of the current database instance as a UNICODE database"
        "@replyvalue INSTANCE    Database instance type: OLTP, LVC or ARCHIVE"
        "@replyvalue SYSNAME     Name of the operating system";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmd_DbmVersion::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_DbmVersion::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmd_DbmVersion oVersion;

  return oVersion.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmd_DbmVersion::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_DbmVersion::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 
  bool                    bAll        = false;
  Tools_DynamicUTF8String szOption;

  bAll = command->oArguments.getCount() == 0;

  if (!bAll) {
    szOption = command->oArguments.getValue(1);
    szOption.ToUpper();
  } // end if

  Reply.startWithOK();

  // VERSION
  if (szOption.Compare(KEYVER_VERSION) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_VERSION, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    tsp100_VersionID0   VersionID0;
    char                szVersion[20];

    sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &VersionID0 );
    sprintf(szVersion, "%d.%d.%02d",
                      (int) VersionID0.MajorVersion_sp100,
                      (int) VersionID0.MinorVersion_sp100,
                      (int) VersionID0.CorrLevel_sp100);

    Reply.appendLine(szVersion);
  } // end if

  //  BUILD
  if (szOption.Compare(KEYVER_BUILD) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_BUILD, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    tsp00_Versionc VersionString;

    sp100_GetVersionString  ( COMP_NAME_DBMSERVER_SP100, s100buildnumber, VersionString );

    Reply.appendLine(VersionString);
  } // end if

  // OS
  if (szOption.Compare(KEYVER_OS) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_OS, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

#ifdef _WIN32
  #ifdef BIT64
      Reply.appendLine(VERVAL_WIN64);
  #else
      Reply.appendLine(VERVAL_WIN32);
  #endif
#else
      Reply.appendLine(VERVAL_UNIX);
#endif
  } // end if

  //  DBROOT/INSTROOT
  if (szOption.Compare(KEYVER_DBROOT) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_DBROOT, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    Reply.appendLine((char *) vcontrol->dbroot);
  } // end if

  //  LOGON
  if (szOption.Compare(KEYVER_LOGON) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_LOGON, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    Reply.appendLine(BOOLSTR_CN00(vcontrol->bIsSAPDBAdmi));
  } // end if

  //  CODE
  if (szOption.Compare(KEYVER_CODE) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_CODE, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    if (stricmp(getUnicode(), VERVAL_YES) == 0) {
      Reply.appendLine(VERVAL_CODE_UTF8);
    } else {
      Reply.appendLine(VERVAL_CODE_ASCII);
    } // end if
  } // end if

  //  SWAP
  if (szOption.Compare(KEYVER_SWAP) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_SWAP, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    tsp00_Int4     i234   = 1;
    char         * c234   = (char *) &i234;;
    const char   * szSwap = VERVAL_SWAP_PART;

    if (c234[3] == 1) {
        szSwap = VERVAL_SWAP_NORMAL;
    } else if (c234[0] == 1) {
        szSwap = VERVAL_SWAP_FULL;
    } // end if

    Reply.appendLine(szSwap);
  } // end if

  //  UNICODE
  if (szOption.Compare(KEYVER_UNICODE) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_UNICODE, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    if (strlen(getUnicode()) > 0) {
      Reply.appendLine(getUnicode());
    } else {
      Reply.appendLine(VERVAL_UNKNOWN);
    } // end if
  } // end if

  //  INSTANCE
  if (szOption.Compare(KEYVER_INSTANCE) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_INSTANCE, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    if (strlen(getInstanceType()) > 0) {
      Reply.appendLine(getInstanceType());
    } else {
      Reply.appendLine(VERVAL_UNKNOWN);
    } // end if
  } // end if

  //  SYSNAME
  if (szOption.Compare(KEYVER_SYSNAME) == 0 || bAll) {
    if (bAll) {
      Reply.appendStringWithMinWidth(KEYVER_SYSNAME, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
    } // end if

    Reply.appendLine(ToolsSys_sysname());
  } // end if

  //  LICENSE
  if (szOption.Compare(KEYVER_LICENSE) == 0 || bAll) {
    if (bAll && m_bCommercial) {
      Reply.appendStringWithMinWidth(KEYVER_LICENSE, VER_MAXKEYLEN);
      Reply.appendString(VER_SEPARATOR);
      Reply.appendLine(VERVAL_COMMERCIAL);
    } else if (!bAll) {
      Reply.appendLine(m_bCommercial? VERVAL_COMMERCIAL : VERVAL_FREE);
    } // end if
  } // end if


  return nFuncReturn;
} // end DBMSrvCmd_DbmVersion::run
