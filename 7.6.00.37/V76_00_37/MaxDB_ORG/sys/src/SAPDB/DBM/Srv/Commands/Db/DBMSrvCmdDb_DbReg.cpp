/*!
  \file    DBMSrvCmdDb_DbReg.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling database registration

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

// system includes
#include <sys/stat.h>

// maxdb includes
#include "hsp100.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_UserProfileContainer.hpp"
#include "ToolsCommon/Tools_Arguments.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"

// dbm include
#include "hcn50.h"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp"
#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/User/DBMSrvUsr_Password.hpp"

// this class
#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbReg.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdDb_DbReg
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdDb_DbReg::DBMSrvCmdDb_DbReg
//-----------------------------------------------------------------------------
DBMSrvCmdDb_DbReg::DBMSrvCmdDb_DbReg( bool obsolete )
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, obsolete , m_LongHelp) 
{
} // end DBMSrvCmdDb_DbReg::DBMSrvCmdDb_DbReg

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdDb_DbReg::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_DbReg                   "db_reg"

#define DBMSRV_OPT_DbReg_Root          "ROOT"
#define DBMSRV_OPT_DbReg_Group         "GROUP"
#define DBMSRV_OPT_DbReg_Auto          "AUTO"
#define DBMSRV_OPT_DbReg_User          "USER"
#define DBMSRV_OPT_DbReg_Db            "DB"

#define DBMSRV_OPT_DbReg_RootC         "-R"
#define DBMSRV_OPT_DbReg_GroupC        "-g"
#define DBMSRV_OPT_DbReg_AutoC         "-a"
#define DBMSRV_OPT_DbReg_UserC         "-u"
#define DBMSRV_OPT_DbReg_DbC           "-d"

#define DBMSRV_OPT_DbReg_Off           "OFF"

const char * const DBMSrvCmdDb_DbReg::m_LongHelp=
"@command "DBMSRV_CMD_DbReg " You update the registration of a database instance. In Microsoft Windows, the"
                            " relevant entry in the registry is updated; in UNIX, the Databases.ini file is"
                            " updated in directory <independent_data_path>/config."
                            " Also with this command you create the database registration for a database which"
                            " was unregistered with db_unreg before and its files still exists."

"@preconditions  You have the server authorization InstallMgm."
               " The operational state of the database instance is OFFLINE."

"@syntax "DBMSRV_CMD_DbReg " ["DBMSRV_OPT_DbReg_Root" <instroot>]"
                           " ["DBMSRV_OPT_DbReg_Auto" ["DBMSRV_OPT_DbReg_Off"]]" 
                           " ["DBMSRV_OPT_DbReg_Group" <usergroup>]"
                           " ["DBMSRV_OPT_DbReg_User" "DBMSRV_OPT_DbReg_Off"| "DBMSRV_OPT_DbReg_User" <osusr>,<pwd>]" 
                           " ["DBMSRV_OPT_DbReg_Db" <dbname> [<firstdbm>,<pwd>]]"

"@param  \""DBMSRV_OPT_DbReg_Root" <instroot>\""
        "The software path for the database instance. "
        "The argument for this option is:"
            "@value <instroot> \"\" The new software path"

"@param  \""DBMSRV_OPT_DbReg_Auto" ["DBMSRV_OPT_DbReg_Off"]\""
        "Activates or deactivates the automatic start of the service in Microsoft Windows. With "
        "the optional value "DBMSRV_OPT_DbReg_Off" this will deactivate the automatic start."

"@param  \""DBMSRV_OPT_DbReg_Group" <usergroup>\""
        "Sets in UNIX the usergroup which will own the database files. "
        "The argument for this option is:"
            "@value <usergroup> \"\" The name of the operating system usergroup"

"@param  \""DBMSRV_OPT_DbReg_User" <osusr>,<pwd>\""
        "Sets in Microsoft Windows the user which will assigned to the service entry. "
        "The argument for this option is:"
            "@value <osusr>,<pwd>  The name and the password of the operating system user"

"@param  \""DBMSRV_OPT_DbReg_User" "DBMSRV_OPT_DbReg_Off"\""
        "Removes in Microsoft Windows the user from the service entry."

"@param  \""DBMSRV_OPT_DbReg_Db" <dbname> <firstdbm>,<pwd>\""
        "This recreates the registration of a database instance which was unregistered before. "
        "The arguments for this option are:"
            "@value <dbname>       \"\" The name of the database instance"
            "@value <dbmusr>,<pwd> \"\" The name and the password of the first database manager operator"

"@reply OK<NL>";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbReg::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdDb_DbReg::getCommandName
    ( )
{
  return DBMSRV_CMD_DbReg;
} // end DBMSrvCmdDb_DbReg::getCommandName

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdDb_DbReg::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDb_DbReg::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmdDb_DbReg oCommand;

  return oCommand.run(vcontrol, command, oReply);
} // end DBMSrvCmdDb_DbReg::runCommand

/*
  -------------------------------------------------------------------------
  function:     DBMSrvCmdDb_DbReg::CheckVersionAndUser
  -------------------------------------------------------------------------
 */
bool DBMSrvCmdDb_DbReg::CheckVersionAndUser 
      ( tsp00_DbNamec              szDbName,
        const DBMSrvUsr_Password & oDBMUserAndPassword,
        DBMSrvMsg_Error          & oError )
{
    tsp100_VersionID1  oVersionReached;

    oVersionReached.MajorVersion_sp100       = 0;
    oVersionReached.MinorVersion_sp100       = 0;
    oVersionReached.CorrLevel_sp100          = 0;
    oVersionReached.BuildNumberPrefix_sp100  = 0;

    tcn002_XpValueString   szVersionReached;

    SAPDB_Bool                    bFileExist  = false;
    RTEConf_Parameter             oConfigFile(szDbName, 
                                              0, 
                                              RTECONF_MAXNAMELENGTH,
                                              RTECONF_MAXSTRINGLENGTH);

    // read configuration
    if (!oConfigFile.Read(bFileExist, oError)) {
        oError = DBMSrvMsg_RTEError(oError);
        // exists configuration?
        if (!bFileExist) {
            oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME,szDbName)));
            oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_PARAM_KERNELVERSION)));
        } // end if
        return false;
    } else if (!bFileExist) {
        oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME,szDbName));
        oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_PARAM_KERNELVERSION)));
        return false;
    } // end if

    if(! oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_PARAM_KERNELVERSION, (DBMSrvPar_RTEConfParameter::String) szVersionReached.asCharp(), oError)) {
        if (oError.ID() == RTEERR_CONF_NOT_FOUND_ID) {
            oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_PARAM_KERNELVERSION)));
        } else {
            oError = DBMSrvMsg_RTEError(oError);
        } // end if
        // ready with error
        return false;
    } // end if

    // convert version string to structure
    tsp00_C64c szVersion;
    tsp00_C64c szBuild;
    tsp00_C64c szTemp;
    char *     pBegin;
    char *     pEnd;
      
    // extract version information from string
    strncpy(szVersion, &szVersionReached[BEG_OF_1ST_VERS_SUBSTR_SP100], END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1);
    szVersion[END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1] = 0;

    // extract build information from string
    strncpy(szBuild, &szVersionReached[BEG_OF_2CND_VERS_SUBSTR_SP100], END_OF_2CND_VERS_SUBSTR_SP100 - BEG_OF_2CND_VERS_SUBSTR_SP100 + 1);
    szBuild[END_OF_2CND_VERS_SUBSTR_SP100 - BEG_OF_2CND_VERS_SUBSTR_SP100 + 1] = 0;

    // major version  
    pBegin = &szVersion[0];
    pEnd   = strchr(pBegin, '.');
    if (pBegin != NULL && pEnd != NULL) {
        strncpy(szTemp, pBegin, pEnd - pBegin);
        szTemp[(int) (pEnd - pBegin)] = 0;
        oVersionReached.MajorVersion_sp100 = (unsigned char) atoi(szTemp);
        pBegin = pEnd + 1;
        pEnd   = strchr(pBegin, '.');
    } else {
        // error reading the version information
        oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BADVERSION, Msg_Arg(SDBMSGTAG_DBMSRV_BADVERSION__DBMSRV_COMP,szVersionReached.asCharp())));
        return false;
    } // end if

    // minor version  
    if (pBegin != NULL && pEnd != NULL) {
        strncpy(szTemp, pBegin, pEnd - pBegin);
        szTemp[(int) (pEnd - pBegin)] = 0;
        oVersionReached.MinorVersion_sp100 = (unsigned char) atoi(szTemp);
        pBegin = pEnd + 1;
    } else {
        // error reading the version information
        oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BADVERSION, Msg_Arg(SDBMSGTAG_DBMSRV_BADVERSION__DBMSRV_COMP,szVersionReached.asCharp())));
        return false;
    } // end if

    // correction level
    if (pBegin != NULL) {
        oVersionReached.CorrLevel_sp100 = (unsigned char) atoi(pBegin);
    } else {
        // error reading the version information
        oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BADVERSION, Msg_Arg(SDBMSGTAG_DBMSRV_BADVERSION__DBMSRV_COMP,szVersionReached.asCharp())));
        return false;
    } // end if
      
    // build
    oVersionReached.BuildNumberPrefix_sp100 = (unsigned char) atoi(szBuild);

    // get my version
    if ((oVersionReached.MajorVersion_sp100       != <MAJOR_VERSION>   ) ||
        (oVersionReached.MinorVersion_sp100       != <MINOR_VERSION>   ) ||
        (oVersionReached.CorrLevel_sp100          != <CORRECTION_LEVEL>) ||
        (oVersionReached.BuildNumberPrefix_sp100  != <BUILD_PREFIX>    )    ) {
        oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_VERSION));
        return false;
    } // end if

    tcn002_XpValueString                  szUserName;
    DBMSrvPar_RTEConfParameter::CryptInfo oPassword;

    if(! oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_CONTROLID, (DBMSrvPar_RTEConfParameter::String) szUserName.asCharp(), oError)) {
        if (oError.ID() == RTEERR_CONF_NOT_FOUND_ID) {
            oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_PARAM_KERNELVERSION)));
        } else {
            oError = DBMSrvMsg_RTEError(oError);
        } // end if
        // ready with error
        return false;
    } // end if

    if(! oConfigFile.GetValue ((const SAPDB_UTF8 *) PAN_CONTROLPW, oPassword, oError)) {
        if (oError.ID() == RTEERR_CONF_NOT_FOUND_ID) {
            oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_PARAM_KERNELVERSION)));
        } else {
            oError = DBMSrvMsg_RTEError(oError);
        } // end if
        // ready with error
        return false;
    } // end if

    DBMSrvUsr_Password oControlPw(&oPassword, DBMSrvUsr_Password::CryptedBufferDB);

    if (SAPDB_strcasecmp(oDBMUserAndPassword.getUser()      , szUserName                ) != 0 || 
        SAPDB_strcasecmp(oDBMUserAndPassword.asClearString(), oControlPw.asClearString()) != 0    ) {
        oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_USRFAIL);
        return false;
    } // end if

    return true;
} // end DBMSrvCmdDb_DbReg::CheckVersionAndUser


//-----------------------------------------------------------------------------
// private member function DBMSrvCmdDb_DbReg::ReRegisterDatabase
//-----------------------------------------------------------------------------
bool DBMSrvCmdDb_DbReg::ReRegisterDatabase
  ( const tsp00_Pathc        & szDBRoot,
    const tsp00_DbNamec      & szDbName,
    const DBMSrvUsr_Password & oDBMUserAndPassword,
    DBMSrvMsg_Error          & oError )
{
    // 1. check existence of database instance
    Msg_List       errRTE;        
    RTE_Database * dbInstance = RTE_DBRegister::Instance().GetDatabase(szDbName, errRTE);
    if (dbInstance != NULL) {
        oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_DBEXIST,	Msg_Arg(SDBMSGTAG_DBMSRV_DBEXIST__DATABASE_NAME, szDbName));
        delete dbInstance;
        return false;
    } // end if
    errRTE.ClearMessageList();

    // 2. Check database version and usr/pwd
    if (!CheckVersionAndUser(szDbName, oDBMUserAndPassword, oError)) {
        return false;
    } // end if

    // 3. Initialize shared memory for this database
    // we have a valid DB name, start using shared memory
    if( !DBMSrvShM_ShmAdmin::TheAdmin().initialize(szDbName, oError) ) {
        return false;
    } // end if

    // 4. register database
    RTE_ActiveInstallation * activeInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBRoot(szDBRoot, errRTE);
    if (activeInstallation == NULL) {
        delete activeInstallation;
        oError = DBMSrvMsg_RTEError(errRTE);
        return false; 
    } // end if

    if (!activeInstallation->RegisterDatabase(szDbName, errRTE)) {
        delete activeInstallation;
        oError = DBMSrvMsg_RTEError(errRTE);
        return false; 
    } // end if

    delete activeInstallation;

    return true;
} // end member function DBMSrvCmdDb_DbReg::ReRegisterDatabase

//-----------------------------------------------------------------------------
// private member function DBMSrvCmdDb_DbReg::SetOsUser
//-----------------------------------------------------------------------------
bool DBMSrvCmdDb_DbReg::SetOsUser
  ( const tsp00_DbNamec      & dbname,
    bool                       bDeleteUser,
    const DBMSrvUsr_Password & oUser,
    DBMSrvMsg_Error          & oError )
{
#ifdef _WIN32
    Msg_List             errRTE;        
    RTE_ActiveDatabase * dbInstance = RTE_DBRegister::Instance().GetActiveDatabase(dbname, errRTE);

    if (dbInstance == NULL) {
        oError.Overrule(DBMSrvMsg_RTEError(errRTE));
        return false;
    } // end if

    tsp00_Namec szUsr;
    tsp00_Namec szPwd;
    szUsr.rawAssign(oUser.getUser());
    szPwd.rawAssign(oUser.asClearString());

    if (!dbInstance->UpdateUserAccount( bDeleteUser ? NULL : szUsr.asCharp(), bDeleteUser ? NULL : szPwd.asCharp(), errRTE)) {
        delete dbInstance;
        oError.Overrule(DBMSrvMsg_RTEError(errRTE));
        return false;
    } // end if
    delete dbInstance;
#endif

    return true;
} // end member function DBMSrvCmdDb_DbReg::SetOsUser

//-----------------------------------------------------------------------------
// private member function DBMSrvCmdDb_DbReg::SetAutostart
//-----------------------------------------------------------------------------
bool DBMSrvCmdDb_DbReg::SetAutostart
  ( const tsp00_DbNamec      & dbname,
    bool                       bValue,
    DBMSrvMsg_Error          & oError )
{
#ifdef _WIN32
    Msg_List             errRTE;        
    RTE_ActiveDatabase * dbInstance = RTE_DBRegister::Instance().GetActiveDatabase(dbname, errRTE);

    if (dbInstance == NULL) {
        oError.Overrule(DBMSrvMsg_RTEError(errRTE));
        return false;
    } // end if

    if (!dbInstance->UpdateAutoStartFlag(bValue, errRTE)) {
        delete dbInstance;
        oError.Overrule(DBMSrvMsg_RTEError(errRTE));
        return false;
    } // end if
    delete dbInstance;
#endif

    return true;
} // end member function DBMSrvCmdDb_DbReg::SetAutostart

//-----------------------------------------------------------------------------
// private member function DBMSrvCmdDb_DbReg::SetGroup
//-----------------------------------------------------------------------------
bool DBMSrvCmdDb_DbReg::SetGroup
  ( const tsp00_DbNamec      & dbname,
    const tsp00_Namec        & szGroup,
    DBMSrvMsg_Error          & oError )
{
    Msg_List                 errRTE;        
    RTE_UserProfileContainer oProfile;

    if (!oProfile.Open(dbname, true, errRTE)) {
        oError.Overrule(DBMSrvMsg_RTEError(errRTE));
        return false;
    } // end if

    if (!oProfile.SetGroupName(szGroup, errRTE)) {
        oError.Overrule(DBMSrvMsg_RTEError(errRTE));
        return false;
    } // end if
    
    return true;
} // end member function DBMSrvCmdDb_DbReg::SetGroup

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdDb_DbReg::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdDb_DbReg::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
    // read the arguments
    SAPDB_Bool          bSetAutostart     = false;
    SAPDB_Bool          bNewAutostart     = false;

    SAPDB_Bool          bSetUser          = false;
    SAPDB_Bool          bDeleteUser       = false;
    DBMSrvUsr_Password  oOsUserAndPassword;

    SAPDB_Bool          bSetGroup         = false;
    SAPDB_Bool          bDeleteGroup      = false;
    tsp00_Namec         szGroup;

    SAPDB_Bool          bSetDbRoot        = false;
    tsp00_Pathc         szDBRoot;

    SAPDB_Bool          bRegDB            = false;
    tsp00_DbNamec       szDbName;
    DBMSrvUsr_Password  oDBMUserAndPassword;

    Tools_Arguments::TokenBuffer szOption;
    Tools_Arguments::TokenBuffer szToken;
    int                          nToken = 1;

    szDBRoot = vcontrol->dbroot;

    szOption.Init();
    szToken.Init();
    while (Tools_Arguments::getTokenFromBuffer(command->args, nToken, szOption)) {

        if ((SAPDB_strcasecmp(DBMSRV_OPT_DbReg_Root, szOption) == 0) ||  
            (SAPDB_strcmp(DBMSRV_OPT_DbReg_RootC, szOption)    == 0)    ) {
            // this option only with datbase name
            if (vcontrol->dbname[0] == 0) {
                DBMSrvMsg_Error oError (SDBMSG_DBMSRV_NODBNAME);
                oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
                return Reply.startWithMessageList(oError);
            } // end if
            if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOffline) {
                DBMSrvMsg_Error oError (SDBMSG_DBMSRV_DBRUN);
                oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
                return Reply.startWithMessageList(oError);
            } // end if
            // read the next token (<dbroot>)
            if (!Tools_Arguments::getTokenFromBuffer(command->args, ++nToken, szDBRoot, szDBRoot.size())) {
                return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
            } // end if
            bSetDbRoot = true;

        } else if ((SAPDB_strcasecmp(DBMSRV_OPT_DbReg_Group, szOption) == 0) ||
                   (SAPDB_strcmp(DBMSRV_OPT_DbReg_GroupC, szOption)    == 0)    ) {
            // this option only with datbase name
            if (vcontrol->dbname[0] == 0) {
                DBMSrvMsg_Error oError (SDBMSG_DBMSRV_NODBNAME);
                oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
                return Reply.startWithMessageList(oError);
            } // end if
            // read the next token (<group>)
            if (!Tools_Arguments::getTokenFromBuffer(command->args, ++nToken, szGroup, szGroup.size())) {
                return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
            } // end if
            bSetGroup    = true;
            bDeleteGroup = (SAPDB_strcasecmp(DBMSRV_OPT_DbReg_Off, szGroup) == 0);
        
        } else if ((SAPDB_strcasecmp(DBMSRV_OPT_DbReg_Auto, szOption) == 0) ||
                   (SAPDB_strcmp(DBMSRV_OPT_DbReg_AutoC, szOption)    == 0)    ) {
            // this option only with datbase name
            if (vcontrol->dbname[0] == 0) {
                DBMSrvMsg_Error oError (SDBMSG_DBMSRV_NODBNAME);
                oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
                return Reply.startWithMessageList(oError);
            } // end if
            // read the next token ('OFF')
            bSetAutostart = true;
            bNewAutostart = true;
            if (Tools_Arguments::getTokenFromBuffer(command->args, ++nToken, szToken)) {
                if (SAPDB_strcasecmp(DBMSRV_OPT_DbReg_Off, szToken) == 0) {
                    bNewAutostart = false;
                } else {
                    --nToken;
                } // end if
            } // end if

        } else if ((SAPDB_strcasecmp(DBMSRV_OPT_DbReg_User, szOption) == 0) ||
                   (SAPDB_strcmp(DBMSRV_OPT_DbReg_UserC, szOption)    == 0)    ) {
            // this option only with datbase name
            if (vcontrol->dbname[0] == 0) {
                DBMSrvMsg_Error oError (SDBMSG_DBMSRV_NODBNAME);
                oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
                return Reply.startWithMessageList(oError);
            } // end if
            // read the next token (<osusr>,<pwd>)
            if (!Tools_Arguments::getTokenFromBuffer(command->args, ++nToken, szToken)) {
                return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
            } // end if
            bSetUser    = true;
            if (SAPDB_strcasecmp(DBMSRV_OPT_DbReg_Off, szToken) == 0) {
                bDeleteUser = true;
            } else {
                // set password object
                oOsUserAndPassword.setPassword(szToken, DBMSrvUsr_Password::UnknownStringWithUser);

                // check password object
                if (!oOsUserAndPassword.isValid()) {
                    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                    Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                    Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
                } // end if
            } // end if
        } else if ((SAPDB_strcasecmp(DBMSRV_OPT_DbReg_Db, szOption) == 0) ||
                   (SAPDB_strcmp(DBMSRV_OPT_DbReg_DbC, szOption)    == 0)    ) {
            // database option only without datbase name
            if (vcontrol->dbname[0] != 0) {
                DBMSrvMsg_Error oError (SDBMSG_DBMSRV_DBNAME);
                oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                        Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                        Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
                return Reply.startWithMessageList(oError);
            } // end if
            // read the next token (<datbase name>)
            if (!Tools_Arguments::getTokenFromBuffer(command->args, ++nToken, szDbName, szDbName.size())) {
                return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
            } // end if
            ToolsParsersUtil_IdentifierCheck::CRuntimeToupper(szDbName);
            // read the next token (<dbmusr>,<pwd>)
            if (!Tools_Arguments::getTokenFromBuffer(command->args, ++nToken, szToken)) {
                return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                  Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
            } // end if
            // set password object
            oDBMUserAndPassword.setPassword(szToken, DBMSrvUsr_Password::UnknownStringWithUser);

            // check password object
            if (!oDBMUserAndPassword.isValid()) {
                return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
            } // end if
            bRegDB = true;

        } else {
            return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_CMDOPTION,
                                                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDOPTION, szOption),
                                                                Msg_Arg(SDBMSGTAG_DBMSRV_CMDOPTION__DBMSRV_CMDNAME, DBMSRV_CMD_DbReg)));
        } // end if

        ++nToken;
    } // end while

    Reply.startWithOK();

    DBMSrvMsg_Error oError;

    // re-register database
    if (bRegDB) {
        if (!ReRegisterDatabase(szDBRoot, szDbName, oDBMUserAndPassword, oError)) {
            return Reply.startWithMessageList(oError);
        } // end if
        vcontrol->pCurrentUser->clear();
        vcontrol->pCurrentUser->setDBName   ( szDbName    )
                               .setUserName ( oDBMUserAndPassword.getUser() );
        vcontrol->pCurrentUser->reload();
        vcontrol->pCurrentUser->checkMasterPwd(oDBMUserAndPassword.asClearString());
        vcontrol->dbname.rawAssign(szDbName);
        vcontrol->pHSSNodes->SetConnectionData(szDbName.asCharp(), oDBMUserAndPassword.getUser(), oDBMUserAndPassword.asClearString());

    } else {

        // check existence of kernel executable in the new dbroot
        if (bSetDbRoot && (strcmp(szDBRoot, vcontrol->dbroot) != NULL)) {
            tsp00_Pathc szKernel;
            szKernel.rawAssign(szDBRoot);
#ifdef _WIN32
            strcat(szKernel, "\\pgm\\kernel.exe");
#else
            strcat(szKernel, "/pgm/kernel");
#endif
            struct stat oState;
            int nState = stat(szKernel, &oState);
            if (nState < 0 || (oState.st_mode & S_IFREG) == 0) {
                return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECHK,
                                                                Msg_Arg(SDBMSGTAG_DBMSRV_FILECHK__FILENAME, szKernel)));
            } // end if
        } // end if

        // refresh installation
        Msg_List errRTE;        
        RTE_ActiveInstallation * activeInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBRoot(szDBRoot, errRTE);
        if (activeInstallation == NULL) {
            return Reply.startWithMessageList(DBMSrvMsg_RTEError(errRTE));
        } else {
            if (!activeInstallation->RegisterDatabase(vcontrol->dbname, errRTE)) {
                delete activeInstallation;
                return Reply.startWithMessageList(DBMSrvMsg_RTEError(errRTE));
            } // end if
            delete activeInstallation;
        } // end if

        // set or unset os user (windows only)
        if (bSetUser) {
            if (!SetOsUser(vcontrol->dbname, bDeleteUser, oOsUserAndPassword, oError)) {
                return Reply.startWithMessageList(oError);
            } // end if
        } // end if

        // set or unset os user (windows only)
        if (bSetAutostart) {
            if (!SetAutostart(vcontrol->dbname, bNewAutostart, oError)) {
                return Reply.startWithMessageList(oError);
            } // end if
        } // end if

        // set support group
        if (bSetGroup) {
            if (!SetGroup(vcontrol->dbname, szGroup, oError)) {
                return Reply.startWithMessageList(oError);
            } // end if
        } // end if

    } // end if

    return OK_CN00;
} // end DBMSrvCmdDb_DbReg::run
