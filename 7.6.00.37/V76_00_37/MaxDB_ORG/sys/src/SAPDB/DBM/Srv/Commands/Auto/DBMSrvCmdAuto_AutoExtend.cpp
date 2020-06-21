/*!
  \file    DBMSrvCmdAuto_AutoExtend.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   definition of classes for handling DBMServer
           dealing with the autoextender

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

#include "DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoExtend.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/VersionInfo/DBMVI_Strings.hpp"
#include "hcn50.h"
#include "hcn51.h"

DBMSrvCmdAuto_AutoExtend::DBMSrvCmdAuto_AutoExtend()
        : DBMSrvCmd_SilentDispatcherUsingCommand( KeyAutoExtend ),
          m_Mode( UNDEFINED ),
          m_Percentage( m_PercentageDefault ) {

    m_longHelp=
        "@command auto_extend With this command, the database instance can be configured "
            "to automatically create new data volumes, when the degree of filling reaches "
            "a certain limit. Size and location of the new data volume are determined as "
            "explained in DBMServer command db_addvolume."
        "@preconditions You have the server authorization ParamFull or AccessUtility."
        "@syntax auto_extend <mode> [<percentage>]"
        "@param <mode> New mode for the database. Possible values are:"
            "@value ON \"\" activate automatic database extension"
            "@value OFF \"\" deactivate automatic database extension"
            "@value SHOW \"\" show current status of automatic database extension"
        "@param <percentage> Limit of degree of filling of the database instance, at which a "
            "new data volume should be added. 1 <= <percentage> <= 100 must be true. Default is 90. "
            "Use only with <mode> ON."
            "@reply OK<NL>[ON <percentage>|OFF[, <reason>]|UNKNOWN]"
            "@replyvalue ON Automatic database extension is activated (only if <mode> is SHOW). <percentage> "
                "is the actual degree of filling of the database instance, at which a new data volume is "
                "added in the current configuration."
            "@replyvalue OFF Automatic database extension is deactivated (only if <mode> is SHOW)"
            "@replyvalue UNKNOWN Status of automatic database extension is unknown (only if <mode> is SHOW)";

}

tcn00_Error DBMSrvCmdAuto_AutoExtend::runLockProtected(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    // check whether the user may access the event dispatcher
    if( !vcontrol->pCurrentUser->isServerRight(DBMRightEvtDispAdmin_CN50, true) )
        return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_MISSRIGHT));
    
    tcn00_Error rc(OK_CN00);
    rc = getEventCategoryNameFromID( sp31ei_db_filling_above_limit, theReply, m_EventName);
    if( rc != OK_CN00 )
        return rc;

    theReply.startWithOK();
        
    rc = analyzeParameters(command->oArguments, theReply );
    if( rc != OK_CN00 ) 
        return rc;

    bool bConfigLineExists(false);
    int iPercent(-1);
    
    DBMSrvMsg_Error errOut;
    bool bDispatcherUp(isDispatcherUp(errOut));
    if( !errOut.IsEmpty() )
        return theReply.startWithMessageList(errOut);

    Tools_DynamicUTF8String autoExtendCondition;
    Tools_DynamicUTF8String autoExtendHandler;
    getEventHandler(m_EventName, autoExtendCondition, autoExtendHandler);
    analyzeConfigLine( autoExtendCondition, autoExtendHandler, bConfigLineExists, iPercent );

    switch( m_Mode ) {
        case SHOW:
            handleSHOW(theReply, iPercent, bConfigLineExists, bDispatcherUp);
            break;
        case ON:
            handleON(errOut, autoExtendCondition, autoExtendHandler, bConfigLineExists, iPercent);
            if( !errOut.IsEmpty() )
                return theReply.startWithMessageList(errOut);
            break;
        case OFF:
            handleOFF(errOut, autoExtendCondition, bConfigLineExists);
            if( m_SilentDispatcherProps.isEmpty() && !stopDispatcher(errOut) ) {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOAUTOEXTENDOFF));
                return theReply.startWithMessageList(errOut);
            }
            break;
        default:
            // not possible
            break;
    }

    return rc;
}

void DBMSrvCmdAuto_AutoExtend::handleSHOW(
        DBMSrv_Reply& theReply,
        int iPercent,
        bool bConfigLineExists,
        bool bDispatcherUp) {

    bool lineAppended(false);

    if( !bConfigLineExists || !bDispatcherUp ) {
        // no line in dispatcher configuration, no pid for dispatcher -> OFF, okay
        theReply.appendLine(m_StatusTextOFF);
        lineAppended = true;
    }
    else if ( bConfigLineExists && bDispatcherUp ) {
        // line exists, a dispatcher process is running -> ON
        theReply.appendString(m_StatusTextON);
        theReply.appendChar(' ');
        theReply.appendLine(SAPDB_ToString(iPercent));
        lineAppended = true;
    }

    bool offline(DBMSrvKnl_State::State(cn00DBMServerData::vcontrol()->dbname) == DBMSrvKnl_State::StateOffline );
    if( offline && bConfigLineExists) {
        cn51DBMConfigValue cfgRunDispatcher(
            cn00DBMServerData::vcontrol()->dbname, 
            CFG_RUNDISPATCHERFLAG_CN51);
        if( 0==strcmp("yes", cfgRunDispatcher) ) {
            // database offline, event dispatcher not running but configured to run
            theReply.appendString(m_StatusTextON);
            theReply.appendChar(' ');
            theReply.appendLine(SAPDB_ToString(iPercent));
            lineAppended = true;
        }
    }

    if( !lineAppended )
        theReply.appendLine(m_StatusTextUNKNOWN);
}

void DBMSrvCmdAuto_AutoExtend::handleOFF(
        DBMSrvMsg_Error& errOut,
        Tools_DynamicUTF8String& autoExtendCondition,
        bool bConfigLineExists) {

    if( bConfigLineExists ) {
        m_SilentDispatcherProps.removeProperty(autoExtendCondition);
        m_SilentDispatcherProps.store(errOut);
    }

    if( m_SilentDispatcherProps.getCount() == 1 ) // only the version information is there
        stopDispatcher(errOut);
}

void DBMSrvCmdAuto_AutoExtend::handleON(
        DBMSrvMsg_Error& errOut,
        Tools_DynamicUTF8String& autoExtendCondition,
        Tools_DynamicUTF8String& autoExtendHandler,
        bool bConfigLineExists,
        int iPercent) {

    if( DBMSrvKnl_State::State(cn00DBMServerData::vcontrol()->dbname) == DBMSrvKnl_State::StateOffline ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
        return;
    }

    if( !bConfigLineExists || iPercent != m_Percentage ) {
        if( bConfigLineExists )
            m_SilentDispatcherProps.removeProperty(autoExtendCondition);

        buildConfigLine( m_EventName, m_Percentage, autoExtendCondition, autoExtendHandler);

        m_SilentDispatcherProps.setProperty(autoExtendCondition, autoExtendHandler);
        m_SilentDispatcherProps.setProperty(
                DBMCliEvtDisp_Properties::FileVersionKey,
                DBMVI_Strings::FormatedBuildVersion);
        m_SilentDispatcherProps.store(errOut);
    }
    if( !startDispatcher(errOut) )
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOAUTOEXTEND));
}

void DBMSrvCmdAuto_AutoExtend::buildConfigLine(
        Tools_DynamicUTF8String& eventName,
        int percentage,
        Tools_DynamicUTF8String& autoExtendCondition,
        Tools_DynamicUTF8String& autoExtendHandler) {

    // build the condition
    autoExtendCondition = SAPDB_ToString(((int)AUTOEXTEND), 10, _T_z);
    autoExtendCondition += ".((Name == \"";
    autoExtendCondition += eventName;
    autoExtendCondition += "\") && (Value2 >= ";
    autoExtendCondition += SAPDB_ToString(percentage);
    autoExtendCondition += "))";

    // build the handler
    autoExtendHandler = m_ConfigLineTemplate;
    autoExtendHandler += SAPDB_ToString(percentage);
}

void DBMSrvCmdAuto_AutoExtend::analyzeConfigLine(
                Tools_DynamicUTF8String& condition,
                Tools_DynamicUTF8String& handler,
                bool& bLineExists,
                int& iPercent ) {

    bLineExists = ( condition.Length() > 0 );

    if( !bLineExists )
        return;
    
    Tools_DynamicUTF8String valCondition("Value2 >=");
    Tools_DynamicUTF8String::BasisElementIndex value2(condition.Find(Tools_DynamicUTF8String(valCondition)));

    if( value2 != Tools_DynamicUTF8String::NPos ) {
        value2 += valCondition.Length();
        iPercent = atoi(condition.SubStr(value2).CharPtr());
    }
}

tcn00_Error DBMSrvCmdAuto_AutoExtend::analyzeParameters(Tools_Arguments& args, DBMSrv_Reply& theReply) {
    tcn00_Error rc(OK_CN00);
    m_Mode = UNDEFINED;

    Tools_DynamicUTF8String mode(args.getValue(1).ToUpper());
    if( mode == Tools_DynamicUTF8String(m_ModeTextON) )
        m_Mode = ON;
    else if( mode == Tools_DynamicUTF8String(m_ModeTextOFF) )
        m_Mode = OFF;
    else if( mode == Tools_DynamicUTF8String(m_ModeTextSHOW) )
        m_Mode = SHOW;

    if( m_Mode == ON ) {
        // get 2nd parameter
        if( args.getCount() == 2 ) {
            // there is a 2nd parameter
            int tempPerc(atoi(args.getValue(2).CharPtr()));
            if( tempPerc < 1 || tempPerc > 100 ) {
                // error
                DBMSrvMsg_Error errPercent(DBMSrvMsg_Error(
                        SDBMSG_DBMSRV_PERCENTVAL_ILLEGAL,
                        Msg_Arg("PERCENTVALUE", args.getValue(2).CharPtr())));
                rc = theReply.startWithMessageList(errPercent);
            }
            else
                m_Percentage = tempPerc;
        }
    }
    else if ( (m_Mode == OFF || m_Mode == SHOW) && (args.getCount() == 2) ) {
        // there is a second parameter, error in this case
        rc = theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
    }
    else if( m_Mode == UNDEFINED ) {
        // error
        DBMSrvMsg_Error errMode(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_AUTOEXT_ILLMODE,
                Msg_Arg("DBMSRV_COMP", args.getValue(1).CharPtr())));
        rc = theReply.startWithMessageList(errMode);
    }

    // append usage string to reply if parameters are wrong
    if( rc != OK_CN00 )
        getUsageText(theReply);

    return rc;
}

const int DBMSrvCmdAuto_AutoExtend::m_PercentageDefault = 90;

const char* const DBMSrvCmdAuto_AutoExtend::m_ModeTextON = "ON";
const char* const DBMSrvCmdAuto_AutoExtend::m_ModeTextOFF = "OFF";
const char* const DBMSrvCmdAuto_AutoExtend::m_ModeTextSHOW = "SHOW";

const char* const DBMSrvCmdAuto_AutoExtend::m_StatusTextON = "ON";
const char* const DBMSrvCmdAuto_AutoExtend::m_StatusTextOFF = "OFF";
const char* const DBMSrvCmdAuto_AutoExtend::m_StatusTextUNKNOWN = "UNKNOWN";

const char* const DBMSrvCmdAuto_AutoExtend::m_ConfigLineTemplate =
                "dbmcli -d $DBNAME$ -u $DBMUSERPW$ db_addvolume DATA -fd ";
