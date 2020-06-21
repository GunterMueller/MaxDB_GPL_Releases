/*!
  \file    DBMSrvCmdEvent_EventDelete.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   set an event

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

#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventDelete.hpp"
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp"

DBMSrvCmdEvent_EventDelete::DBMSrvCmdEvent_EventDelete()
: DBMSrvCmd_EventAdministrationCommand( DBMSrv_Command::KeyEventDelete, false, m_LongHelp )
{}

tcn00_Error DBMSrvCmdEvent_EventDelete::runEventAdministrationCommand(
        VControlDataT* vcontrol,
        DBMSrvPar_Parameters* kernelParameters,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    DBMSrvMsg_Error localErrList;

    // must not delete that pointer later, even though its possible
    DBMSrvPar_Parameter* eventCategoryPar(kernelParameters->Get(command->oArguments.getValue(1).ToUpper().CharPtr()));

    if( eventCategoryPar == NULL ) {
        // the parameter name is invalid
        localErrList.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_EVENTCAT_UNKNOWN,
                Msg_Arg("EVENTCATEGORY", command->oArguments.getValue(1).CharPtr())));
        return theReply.startWithMessageList(localErrList);
    }

    if( !(eventCategoryPar->nClasses & (SAPDB_UInt4)DBMSrvPar_Parameter::XPClassValueEvents) ) {
        // it is not only exactly in parameter class EVENTS
        localErrList.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_EVENTCAT_UNKNOWN,
                Msg_Arg("EVENTCATEGORY", command->oArguments.getValue(1).CharPtr())));
        return theReply.startWithMessageList(localErrList);
    }

    bool isMultiple(eventCategoryPar->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_String);
    if( !isMultiple ) {
        if( !(eventCategoryPar->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Integer) ) {
            // parameter is neither a string nor an integer
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
            return theReply.startWithMessageList(localErrList);
        }
    }

    int expectedParamCount(isMultiple?2:1);
    if( command->oArguments.getCount() < expectedParamCount ) {
        // too few parameters
        localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_TOOFEWPARAM));
        return theReply.startWithMessageList(localErrList);
    }
    else if( command->oArguments.getCount() > expectedParamCount ) {
        // too many parameters
        localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_TOOMANYPARAM));
        return theReply.startWithMessageList(localErrList);
    }

    if( isMultiple ) {
        // working on an event category that can hold more than one event definition
        if( command->oArguments.getValue(2).FindFirstNotOf("-0123456789") != Tools_DynamicUTF8String::NPos ) {
            // parameter is not numeric
            DBMSrvMsg_Error errPercent(DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_PERCENTVAL_ILLEGAL,
                    Msg_Arg("PERCENTVALUE", command->oArguments.getValue(2).CharPtr())));
            localErrList.Overrule(errPercent);
            return theReply.startWithMessageList(localErrList);
        }
        SAPDB_Int4 eventValue(atoi(command->oArguments.getValue(2).CharPtr()));
        if( eventValue < 0 || eventValue > 100 ) {
            // not a valid percent number
            DBMSrvMsg_Error errPercent(DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_PERCENTVAL_ILLEGAL,
                    Msg_Arg("PERCENTVALUE", command->oArguments.getValue(2).CharPtr())));
            localErrList.Overrule(errPercent);
            return theReply.startWithMessageList(localErrList);
        }

        SAPDBDBE_EventCategory eventCategory(
                localErrList,
                eventCategoryPar->szActiveValue.asCharp());
        if( !localErrList.IsEmpty() ) {
            // could not create event category
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NO_EVENTCONFIG_CHANGE));
            return theReply.startWithMessageList(localErrList);
        }

        SAPDBDBE_EventDefinition oldEvent(
                SAPDBDBE_EventDefinition::EDS_Removed,
                sp31ep_low,
                eventValue);
        eventCategory.remove(oldEvent);

        oldEvent.setPriority(sp31ep_medium);
        eventCategory.remove(oldEvent);

        oldEvent.setPriority(sp31ep_high);
        eventCategory.remove(oldEvent);

        size_t reqSize(eventCategory.getUpdatedCategoryValueSize());
        char* categoryString(new char[reqSize]);
        if( categoryString == NULL ) {
            // memory problem
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NO_EVENTCONFIG_CHANGE));
            return theReply.startWithMessageList(localErrList);
        }
        
        if( !eventCategory.getUpdatedCategoryValue(categoryString, reqSize, reqSize) ) {
            // could not create string describing new category
            delete[] categoryString;
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NO_EVENTCONFIG_CHANGE));
            return theReply.startWithMessageList(localErrList);
        }

        if( !kernelParameters->SetValueWithCheck(eventCategoryPar->szParameterName.asCharp(), 
                                                 categoryString,
                                                 true,
                                                 DBMSrvPar_Parameter::XPWriteModeAll,
                                                 localErrList) ) {
            delete[] categoryString;
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NO_EVENTCONFIG_CHANGE));
            return theReply.startWithMessageList(localErrList);
        }

    }
    else {
        // working on a single event definition event category
        if( !kernelParameters->SetValueWithCheck(eventCategoryPar->szParameterName.asCharp(), 
                                                 SAPDB_ToString((DBMSrvPar_RTEConfParameter::Integer) sp31ep_nil), // necessary since it must be a string...
                                                 true,
                                                 DBMSrvPar_Parameter::XPWriteModeAll,
                                                 localErrList) ) {
            localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NO_EVENTCONFIG_CHANGE));
            return theReply.startWithMessageList(localErrList);
        }
    }

    if( !kernelParameters->CheckAll(localErrList) ) {
        // checking failed...
        // my interpretation
        localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NO_EVENTCONFIG_CHANGE));
        return theReply.startWithMessageList(localErrList);
    }

    if( !kernelParameters->Commit(localErrList) ) {
        // committing failed...
        // my interpretation
        localErrList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NO_EVENTCONFIG_CHANGE));
        return theReply.startWithMessageList(localErrList);
    }

    theReply.startWithOK();
    return OK_CN00;
}

const char* const DBMSrvCmdEvent_EventDelete::m_LongHelp= 
    "@command event_delete With this command, you instruct the database kernel to stop creating "
        "an event if a specific situation occurs. This command works in all operational states "
        "of the database. There are event categories, that contain only one event. For these "
        "event categories, a value cannot be specified"
    "@preconditions You have the DBM operator authorization DBInfoRead."
    "@syntax event_delete <event_category> [<value>]"
    "@param <event_category> category of the event that is to be deleted."
    "@param <value> numeric value. It's meaning depends on the value of event_category."
    "@reply OK<NL>";
