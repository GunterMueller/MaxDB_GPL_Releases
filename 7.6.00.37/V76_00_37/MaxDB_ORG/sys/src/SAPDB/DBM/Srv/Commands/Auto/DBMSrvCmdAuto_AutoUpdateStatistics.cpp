/*!

  \file    DBMSrvCmdAuto_AutoUpdateStatistics.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   automatic update of optimizer statistics

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


*/

// including a kernel header to get the event name...
#include "vsp0031.h"
#include "hcn50.h"
#include "hcn51.h"
#include "DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoUpdateStatistics.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/VersionInfo/DBMVI_Strings.hpp"

DBMSrvCmdAuto_AutoUpdateStatistics::DBMSrvCmdAuto_AutoUpdateStatistics()
        : DBMSrvCmd_SilentDispatcherUsingCommand( KeyAutoUpdateStatistics ),
          m_Mode( UNDEFINED ) {

    m_longHelp=
        "@command auto_update_statistics With this command, the database instance can be configured "
            "to automatically update the optimizer statistics, when necessary."
        "@preconditions You have the server authorization AccessSQL."
        "@syntax auto_update_statistics <mode>"
        "@param <mode> New mode for the database. Possible values are:"
            "@value ON \"\" activate automatic update of optimizer statistics"
            "@value OFF \"\" deactivate automatic update of optimizer statistics"
            "@value SHOW \"\" show current status of automatic update of optimizer statistics"
        "@reply OK<NL>[ON|OFF|UNKNOWN]"
            "@replyvalue ON Automatic update of optimizer statistics is activated (only if <mode> is SHOW)"
            "@replyvalue OFF Automatic update of optimizer statistics deactivated (only if <mode> is SHOW)"
            "@replyvalue UNKNOWN Status of update of optimizer statistics is unknown (only if <mode> is SHOW)";
}

tcn00_Error DBMSrvCmdAuto_AutoUpdateStatistics::runLockProtected(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    // check whether the user may access the event dispatcher
    if( !vcontrol->pCurrentUser->isServerRight(DBMRightEvtDispAdmin_CN50, true) )
        return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_MISSRIGHT));

    tcn00_Error rc(OK_CN00);
    rc = getEventCategoryNameFromID( sp31ei_upd_stat_wanted, theReply, m_EventName);
    if( rc != OK_CN00 )
        return rc;

    theReply.startWithOK();

    rc = analyzeParameters(command->oArguments, theReply );
    if( rc != OK_CN00 ) 
        return rc;

    bool bConfigLineExists(false);

    DBMSrvMsg_Error errOut;
    bool bDispatcherUp(isDispatcherUp(errOut));
    if( !errOut.IsEmpty() )
        return theReply.startWithMessageList(errOut);

    Tools_DynamicUTF8String autoUpdStatCondition;
    Tools_DynamicUTF8String autoUpdStatHandler;
    getEventHandler(m_EventName, autoUpdStatCondition, autoUpdStatHandler);
    bConfigLineExists = autoUpdStatCondition.Length() > 0;

    switch( m_Mode ) {
        case SHOW:
            handleSHOW(theReply, bConfigLineExists, bDispatcherUp);
            break;
        case ON:
            handleON(errOut, autoUpdStatCondition, autoUpdStatHandler, bConfigLineExists);
            if( !errOut.IsEmpty() )
                return theReply.startWithMessageList(errOut);
            break;
        case OFF:
            handleOFF(errOut, autoUpdStatCondition, bConfigLineExists);
            if( m_SilentDispatcherProps.isEmpty() && !stopDispatcher(errOut) ) {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOAUTOUPDSTATOFF));
                return theReply.startWithMessageList(errOut);
            }
            break;
        default:
            // not possible
            break;
    }

    return rc;
}

void DBMSrvCmdAuto_AutoUpdateStatistics::handleOFF(
        DBMSrvMsg_Error& errOut,
        Tools_DynamicUTF8String& autoUpdStatCondition,
        bool bConfigLineExists) {

    if( bConfigLineExists ) {
        m_SilentDispatcherProps.removeProperty(autoUpdStatCondition);
        m_SilentDispatcherProps.store(errOut);
    }

    if( m_SilentDispatcherProps.getCount() == 1 )
        stopDispatcher(errOut);

    cn51DBMConfigValue updateStatisticsWhenReachingOnline(
        cn00DBMServerData::vcontrol()->dbname,
        CFG_UPDATESTATISTICSWHENREACHINGONLINE_CN51);
    tsp00_C512c emptyValue;
    emptyValue.rawAssign("");
    updateStatisticsWhenReachingOnline = emptyValue;
}

void DBMSrvCmdAuto_AutoUpdateStatistics::handleSHOW(
        DBMSrv_Reply& theReply,
        bool bConfigLineExists,
        bool bDispatcherUp) {

    const char* statusText(m_StatusTextUNKNOWN);

    if( !bConfigLineExists || !bDispatcherUp )
        // no line in dispatcher configuration, no pid for dispatcher -> OFF, okay
        statusText = m_StatusTextOFF;
    else if ( bConfigLineExists && bDispatcherUp )
        // line exists, a dispatcher process is running -> ON
        statusText = m_StatusTextON;

    bool offline(DBMSrvKnl_State::State(cn00DBMServerData::vcontrol()->dbname) == DBMSrvKnl_State::StateOffline );
    if( offline && bConfigLineExists) {
        cn51DBMConfigValue cfgRunDispatcher(
            cn00DBMServerData::vcontrol()->dbname, 
            CFG_RUNDISPATCHERFLAG_CN51);
        if( 0==strcmp("yes", cfgRunDispatcher) ) {
            // database offline, event dispatcher not running but configured to run
            statusText = m_StatusTextON;
        }
    }

    theReply.appendLine(statusText);
}

void DBMSrvCmdAuto_AutoUpdateStatistics::handleON(
        DBMSrvMsg_Error& errOut,
        Tools_DynamicUTF8String& autoUpdStatCondition,
        Tools_DynamicUTF8String& autoUpdStatHandler,
        bool bConfigLineExists) {

    DBMSrvMsg_Error errors;

    if( DBMSrvKnl_State::State(cn00DBMServerData::vcontrol()->dbname) == DBMSrvKnl_State::StateOffline ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
        return;
    }

    if( !bConfigLineExists ) {
        buildConfigLine(m_EventName, autoUpdStatCondition, autoUpdStatHandler);
        m_SilentDispatcherProps.setProperty(autoUpdStatCondition, autoUpdStatHandler);
        m_SilentDispatcherProps.setProperty(
                DBMCliEvtDisp_Properties::FileVersionKey,
                DBMVI_Strings::FormatedBuildVersion);
        m_SilentDispatcherProps.store(errors);
    }

    if( !bConfigLineExists && errors.IsEmpty() ) {
        
        if( !startDispatcher(errors) )
            errors.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOAUTOUPDSTAT));
        else if( DBMSrvKnl_State::State(cn00DBMServerData::vcontrol()->dbname) == DBMSrvKnl_State::StateOnline ) {
            // database is online, update statistics in background
            m_ExecuteUnprotectedAfter = true;
        }
        else {
            // database is not online, defer updating the statistics to next online
            cn51DBMConfigValue updateStatisticsWhenReachingOnline(
                cn00DBMServerData::vcontrol()->dbname,
                CFG_UPDATESTATISTICSWHENREACHINGONLINE_CN51);
            tsp00_C512c yes;
            yes.rawAssign("yes");
            updateStatisticsWhenReachingOnline = yes;
        }
    }

    errOut.Overrule(errors);
}

tcn00_Error DBMSrvCmdAuto_AutoUpdateStatistics::runUnProtectedAfter(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrvMsg_Error& errOut) {

    DBMSrvCmdSQL_SqlUpdatestatPerSystemtable sqlUpdatestatPerSystemtable;
    if( sqlUpdatestatPerSystemtable.updateSmartInBackground(vcontrol, errOut) )
        return OK_CN00;
    else
        return errOut.get_tcn00_Error();
}


void DBMSrvCmdAuto_AutoUpdateStatistics::buildConfigLine(
        Tools_DynamicUTF8String& eventName,
        Tools_DynamicUTF8String& autoUpdStatCondition,
        Tools_DynamicUTF8String& autoUpdStatHandler) {

    // build the condition
    autoUpdStatCondition = SAPDB_ToString(((int)UPDATESTATISTICS), 10, _T_z);
    autoUpdStatCondition += ".((Name == \"";
    autoUpdStatCondition += eventName;
    autoUpdStatCondition += "\"))";

    // build the handler
    autoUpdStatHandler = m_ConfigLine;
}

tcn00_Error DBMSrvCmdAuto_AutoUpdateStatistics::analyzeParameters(
        Tools_Arguments& args,
        DBMSrv_Reply& theReply) {

    tcn00_Error rc(OK_CN00);
    m_Mode = UNDEFINED;

    Tools_DynamicUTF8String mode(args.getValue(1).ToUpper());
    if( mode == Tools_DynamicUTF8String(m_ModeTextON) )
        m_Mode = ON;
    else if( mode == Tools_DynamicUTF8String(m_ModeTextOFF) )
        m_Mode = OFF;
    else if( mode == Tools_DynamicUTF8String(m_ModeTextSHOW) )
        m_Mode = SHOW;

    if( m_Mode == UNDEFINED )
        // error
        rc = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));

        // append usage string to reply if parameters are wrong
    if( rc != OK_CN00 )
        getUsageText(theReply);

    return rc;
}

const char* const DBMSrvCmdAuto_AutoUpdateStatistics::m_ModeTextON = "ON";
const char* const DBMSrvCmdAuto_AutoUpdateStatistics::m_ModeTextOFF = "OFF";
const char* const DBMSrvCmdAuto_AutoUpdateStatistics::m_ModeTextSHOW = "SHOW";

const char* const DBMSrvCmdAuto_AutoUpdateStatistics::m_StatusTextON = "ON";
const char* const DBMSrvCmdAuto_AutoUpdateStatistics::m_StatusTextOFF = "OFF";
const char* const DBMSrvCmdAuto_AutoUpdateStatistics::m_StatusTextUNKNOWN = "UNKNOWN";

const char* const DBMSrvCmdAuto_AutoUpdateStatistics::m_ConfigLine =
                "dbmcli -d $DBNAME$ -u $DBMUSERPW$ "DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_CMD;
