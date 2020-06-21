/*!
  \file    DBMSrvCmdDbm_DbmConfigSet.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command dbm_configset

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

#include "SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmConfigSet.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdDbm_DbmConfigSet
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdDbm_DbmConfigSet::DBMSrvCmdDbm_DbmConfigSet
//-----------------------------------------------------------------------------
DBMSrvCmdDbm_DbmConfigSet::DBMSrvCmdDbm_DbmConfigSet()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdDbm_DbmConfigSet::m_LongHelp ) 
{
} // end DBMSrvCmdDbm_DbmConfigSet::DBMSrvCmdDbm_DbmConfigSet

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdDbm_DbmConfigSet::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbmConfigSet              "dbm_configset"
#define DBMSRV_OPT_DbmConfigSet_Raw          "-raw"
#define DBMSRV_OPT_DbmConfigSet_INSTANCE     "INSTANCE"
#define DBMSRV_OPT_DbmConfigSet_INSTALLATION "INSTALLATION"
#define DBMSRV_OPT_DbmConfigSet_GLOBAL       "GLOBAL"
#define DBMSRV_OPT_DbmConfigSet_name         "<name>"
#define DBMSRV_OPT_DbmConfigSet_value        "<value>"


const char * const DBMSrvCmdDbm_DbmConfigSet::m_LongHelp=
"@command "DBMSRV_CMD_DbmConfigSet" You set the value of a configuration parameter of the Database Manager Server."
                                  " The parameter values are stored in configuration files. There is one file specific"
                                  " for each database instance. A second file is specific for each installed software version."
                                  " A third file will be valid for the whole database server. Parameters in the instance specific"
                                  " file overrule parameters from the installation specific file and parameters in the installation"
                                  " specific file overrule parameters in the file valid for the whole database server."

"@preconditions You have the server authorization InstallMgm."

"@syntax "DBMSRV_CMD_DbmConfigSet" ["DBMSRV_OPT_DbmConfigSet_INSTANCE"|"DBMSRV_OPT_DbmConfigSet_INSTALLATION"|"DBMSRV_OPT_DbmConfigSet_GLOBAL"] "DBMSRV_OPT_DbmConfigSet_name" ["DBMSRV_OPT_DbmConfigSet_value"]"

"@param "DBMSRV_OPT_DbmConfigSet_INSTANCE"      With option "DBMSRV_OPT_DbmConfigSet_INSTANCE" the parameter(s) will be stored only in the file specific for the database instance. This is also the default."
"@param "DBMSRV_OPT_DbmConfigSet_INSTALLATION"  With option "DBMSRV_OPT_DbmConfigSet_INSTALLATION" the parameter(s) will be stored only in the file specific for the installation."
"@param "DBMSRV_OPT_DbmConfigSet_GLOBAL"        With option "DBMSRV_OPT_DbmConfigSet_GLOBAL" the parameter(s) will be storedonly in the file valid for the whole datbase server."
"@param "DBMSRV_OPT_DbmConfigSet_name"          The name of the parameter to save."
"@param "DBMSRV_OPT_DbmConfigSet_value"         The value for the parameters. Without value the of the parameter will be cleared."

"@reply OK<NL>";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDbm_DbmConfigSet::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdDbm_DbmConfigSet::getCommandName
    ( )
{
  return DBMSRV_CMD_DbmConfigSet;
} // end DBMSrvCmdDbm_DbmConfigSet::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDbm_DbmConfigSet::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDbm_DbmConfigSet::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error         nFuncReturn = OK_CN00;
  
  cn51DBMConfigValue::ConfigurationMode oMode = cn51DBMConfigValue::ModeInstance;
  
  char  szOption  [PARAMETER_MAXLEN_CN90] = "";
  char  szName    [PARAMETER_MAXLEN_CN90] = "";
  char  szValue   [PARAMETER_MAXLEN_CN90] = "";

  int nToken = 0;

  boolean bRaw = false;
  
  // reading first token and check for option
  cn90GetToken(command->args, szOption, ++nToken, PARAMETER_MAXLEN_CN90);
  if (stricmp(szOption, DBMSRV_OPT_DbmConfigSet_Raw) == 0) {
    // ignore it, it is obsolete
  } else if (stricmp(szOption, DBMSRV_OPT_DbmConfigSet_INSTANCE) == 0) {
    // ignore it, it is the default
  } else if (stricmp(szOption, DBMSRV_OPT_DbmConfigSet_INSTALLATION) == 0) {
    oMode = cn51DBMConfigValue::ModeInstallation;
  } else if (stricmp(szOption, DBMSRV_OPT_DbmConfigSet_GLOBAL) == 0) {
    oMode = cn51DBMConfigValue::ModeGlobal;
  } else {
    SAPDB_strcpy(szName, szOption);
  } // end if

  // reading name
  if (SAPDB_strlen(szName) == 0) {
    if (!cn90GetToken(command->args, szName, ++nToken, PARAMETER_MAXLEN_CN90)) {
      return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION, Msg_Arg("DBMSRV_CMDOPTION", DBMSRV_OPT_DbmConfigSet_name), Msg_Arg("DBMSRV_CMDNAME",DBMSRV_CMD_DbmConfigSet)));
    } // end if
  } // end if

  // reading value
  cn90GetToken(command->args, szValue, ++nToken, PARAMETER_MAXLEN_CN90);

  // save the value
  cn51DBMConfigValue cfgValue(vcontrol->dbname, szName, oMode);
  cfgValue = szValue;
  if (!cfgValue.LastEvent().IsEmpty()) {
    return Reply.startWithMessageList(cfgValue.LastEvent());
  } // end if

  Reply.startWithOK();

  return OK_CN00;
} // end DBMSrvCmdDbm_DbmConfigSet::run
