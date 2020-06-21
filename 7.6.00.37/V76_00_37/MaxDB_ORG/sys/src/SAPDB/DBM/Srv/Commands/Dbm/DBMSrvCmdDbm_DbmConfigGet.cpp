/*!
  \file    DBMSrvCmdDbm_DbmConfigGet.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command dbm_configget

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

#include "hcn51.h"
#include "hcn90.h"

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#include "SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmConfigGet.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdDbm_DbmConfigGet
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdDbm_DbmConfigGet::DBMSrvCmdDbm_DbmConfigGet
//-----------------------------------------------------------------------------
DBMSrvCmdDbm_DbmConfigGet::DBMSrvCmdDbm_DbmConfigGet()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdDbm_DbmConfigGet::m_LongHelp ) 
{
} // end DBMSrvCmdDbm_DbmConfigGet::DBMSrvCmdDbm_DbmConfigGet

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdDbm_DbmConfigGet::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbmConfigGet              "dbm_configget"
#define DBMSRV_OPT_DbmConfigGet_Raw          "-raw"
#define DBMSRV_OPT_DbmConfigGet_ALL          "ALL"
#define DBMSRV_OPT_DbmConfigGet_INSTANCE     "INSTANCE"
#define DBMSRV_OPT_DbmConfigGet_INSTALLATION "INSTALLATION"
#define DBMSRV_OPT_DbmConfigGet_GLOBAL       "GLOBAL"


const char * const DBMSrvCmdDbm_DbmConfigGet::m_LongHelp=
"@command "DBMSRV_CMD_DbmConfigGet" You display the values of one ore more configuration parameters of the Database Manager Server."
                                  " If you do not specify a name of a parameter all parameters from the requested area will"
                                  " be listed with name and value."
                                  " The parameter values are stored in configuration files. There is one file specific"
                                  " for each database instance. A second file is specific for each installed software version."
                                  " A third file will be valid for the whole database server. Parameters in the instance specific"
                                  " file overrule parameters from the installation specific file and parameters in the installation"
                                  " specific file overrule parameters in the file valid for the whole database server."

"@preconditions You have the server authorization InstallMgm."

"@syntax "DBMSRV_CMD_DbmConfigGet" ["DBMSRV_OPT_DbmConfigGet_ALL"|"DBMSRV_OPT_DbmConfigGet_INSTANCE"|"DBMSRV_OPT_DbmConfigGet_INSTALLATION"|"DBMSRV_OPT_DbmConfigGet_GLOBAL"] [<name>]"

"@param "DBMSRV_OPT_DbmConfigGet_ALL"           With option "DBMSRV_OPT_DbmConfigGet_ALL" the parameter(s) will be searched in all three configuration files. This is also the default."
"@param "DBMSRV_OPT_DbmConfigGet_INSTANCE"      With option "DBMSRV_OPT_DbmConfigGet_INSTANCE" the parameter(s) will be searched only in the file specific for the database instance."
"@param "DBMSRV_OPT_DbmConfigGet_INSTALLATION"  With option "DBMSRV_OPT_DbmConfigGet_INSTALLATION" the parameter(s) will be searched only in the file specific for the installation."
"@param "DBMSRV_OPT_DbmConfigGet_GLOBAL"        With option "DBMSRV_OPT_DbmConfigGet_GLOBAL" the parameter(s) will be searched only in the file valid for the whole datbase server."
"@param <name>                                  The name of the requested parameter."

"@reply OK<NL>"
       "<value><NL>"
       "  or<NL>"
       "OK<NL>"
       "<name> = <value><NL>"
       "<name> = <value><NL>"
       "...";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDbm_DbmConfigGet::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdDbm_DbmConfigGet::getCommandName
    ( )
{
  return DBMSRV_CMD_DbmConfigGet;
} // end DBMSrvCmdDbm_DbmConfigGet::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDbm_DbmConfigGet::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDbm_DbmConfigGet::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error         nFuncReturn = OK_CN00;
  
  cn51DBMConfigValue::ConfigurationMode oMode = cn51DBMConfigValue::ModeAll;
  
  char  szOption  [PARAMETER_MAXLEN_CN90] = "";
  char  szName    [PARAMETER_MAXLEN_CN90] = "";

  boolean bRaw = false;
  
  cn90GetToken(command->args, szOption, 1, PARAMETER_MAXLEN_CN90);
  if (stricmp(szOption, DBMSRV_OPT_DbmConfigGet_Raw) == 0) {
    // ignore it, it is obsolete
  } else if (stricmp(szOption, DBMSRV_OPT_DbmConfigGet_ALL) == 0) {
    // ignore it, it is the default
  } else if (stricmp(szOption, DBMSRV_OPT_DbmConfigGet_INSTANCE) == 0) {
    oMode = cn51DBMConfigValue::ModeInstance;
  } else if (stricmp(szOption, DBMSRV_OPT_DbmConfigGet_INSTALLATION) == 0) {
    oMode = cn51DBMConfigValue::ModeInstallation;
  } else if (stricmp(szOption, DBMSRV_OPT_DbmConfigGet_GLOBAL) == 0) {
    oMode = cn51DBMConfigValue::ModeGlobal;
  } else {
    SAPDB_strcpy(szName, szOption);
  } // end if

  // read the name
  if (SAPDB_strlen(szName) == 0) {
    cn90GetToken(command->args, szName, 2, PARAMETER_MAXLEN_CN90);
  } // end if

  if (SAPDB_strlen(szName) == 0) {
    cn51DBMConfigValueIterator oIterator(vcontrol->dbname, oMode);

    Reply.startWithOK();

    while (!oIterator.IsEnd()) {
      Reply.appendStringWithMinWidth(oIterator.GetCurrentName(), 20);
      Reply.appendString("= ");
      Reply.appendLine(oIterator.GetCurrentValueAsString());
      oIterator.Next();
    } // end while

  } else {
    Reply.startWithOK();

    // read the value from file
    cn51DBMConfigValue cfgValue(vcontrol->dbname, szName, oMode);
    Reply.appendLine(cfgValue);
  } // end if

  return OK_CN00;
} // end DBMSrvCmdDbm_DbmConfigGet::run
