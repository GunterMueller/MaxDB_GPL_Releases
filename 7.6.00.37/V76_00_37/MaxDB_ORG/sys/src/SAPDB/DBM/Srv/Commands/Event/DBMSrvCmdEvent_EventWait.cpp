/*!
  \file    DBMSrvCmdEvent_EventWait.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   command event_wait

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

#include <stdlib.h>

#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventWait.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp"

tcn00_Error DBMSrvCmdEvent_EventWait::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    // database is offline, return an error
    if (DBMSrvKnl_State::State(vcontrol->dbname.asCharp()) == DBMSrvKnl_State::StateOffline) {
      // stop session if exists
      if (NULL!=DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionEvent)) {
        DBMSrvMsg_Error dummyErrList;
        DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionEvent, dummyErrList);
      } // end if
      return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBNORUN));
    } // end if

    DBMSrvMsg_Error localErrList;
    DBMSrvKnl_Session * pSession = DBMSrvKnl_GlobalSessions::GetOrCreateSession
                                                 ( DBMSrvKnl_GlobalSessions::SessionEvent,
                                                   vcontrol->dbname,
                                                   localErrList );

    if( pSession == NULL ) {
        // report an error and return
        return theReply.startWithMessageList(localErrList);
    } // end if

    if (!pSession->IsRequestPending()) {
        DBMSrvMsg_Error requestError;
        if (!pSession->EventRequest(requestError)) {
            if ((requestError == DBMSrvMsg_Error::DBMSrv_SHUTDOWN) ||
                (requestError == DBMSrvMsg_Error::DBMSrv_RTE)         ) {
                DBMSrvMsg_Error releaseError;
                DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionEvent, releaseError);
                releaseError.Overrule(requestError);
                requestError = releaseError;
            } // end if
            return theReply.startWithMessageList(requestError);
        } // end if
    } // end if

    tsp31_event_description   eventDescription;

    DBMSrvMsg_Error receiveError;
    if (!pSession->EventReceive(&eventDescription, sizeof(eventDescription), receiveError)) {
          if ((receiveError == DBMSrvMsg_Error::DBMSrv_SHUTDOWN) ||
              (receiveError == DBMSrvMsg_Error::DBMSrv_RTE     )    ) {
            DBMSrvMsg_Error releaseError;
            DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionEvent, releaseError);
            releaseError.Overrule(receiveError);
            receiveError = releaseError;
          } // end if
          return theReply.startWithMessageList(receiveError);
    } // end if

    theReply.startWithOK();
    DBMSrvCmdEvent_EventWait::replyEvent( eventDescription, theReply );
    return OK_CN00;
}

void DBMSrvCmdEvent_EventWait::replyEvent( tsp31_event_description& eventDescription, DBMSrv_Reply& theReply ) {

    // does nothing if initialized already
    intializeEventCategories( cn00DBMServerData::vcontrol()->dbname.asCharp(), theReply );

    if( !DBMSrvCmdEvent_EventWait::m_EventCategoriesRead )
        return;

    // look for the name
    Tools_DynamicUTF8String eventCategoryName(DBMSrvCmd_EventAdministrationCommand::m_EventCategoryUnknown);
    Tools_Vector<DBMSrvDBE_EventMapping*>::iterator it(m_CategoryMapping.begin());
    while( it != m_CategoryMapping.end() ) {
        if( (*it)->id == eventDescription.sp31ed_ident ) {
            eventCategoryName = (*it)->name;
            break;
        }
        ++it;
    }

    // print name
    theReply.appendString(m_LabelName);
    theReply.appendChar(m_AssignChar);
    theReply.appendString(eventCategoryName.CharPtr());
    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
    
    // print priority
    theReply.appendString(m_LabelPriority);
    theReply.appendChar(m_AssignChar);
    theReply.appendString(
                DBMSrvCmd_EventAdministrationCommand::getTextFromEventPriority(
                        eventDescription.sp31ed_priority).CharPtr());
    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);

    // Date
    theReply.appendString(m_LabelDate);
    theReply.appendChar(m_AssignChar);
    theReply.appendStringN((const char*)eventDescription.sp31ed_date, sizeof(eventDescription.sp31ed_date));
    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);

    // Time
    theReply.appendString(m_LabelTime);
    theReply.appendChar(m_AssignChar);
    theReply.appendStringN(((const char*)eventDescription.sp31ed_time)+2, sizeof(eventDescription.sp31ed_time)-2);
                                                                   // ^^ this is ugly...
    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);

    // Value 1
    theReply.appendString(m_LabelValue1);
    theReply.appendChar(m_AssignChar);
    theReply.appendString(SAPDB_ToString((int)eventDescription.sp31ed_value_1));
    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);

    // Value 2
    theReply.appendString(m_LabelValue2);
    theReply.appendChar(m_AssignChar);
    theReply.appendString(SAPDB_ToString((int)eventDescription.sp31ed_value_2));
    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);

    // Count
    theReply.appendString(m_LabelCount);
    theReply.appendChar(m_AssignChar);
    theReply.appendString(SAPDB_ToString((int)eventDescription.sp31ed_eventcnt));
    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);

    // Text
    theReply.appendString(m_LabelText);
    theReply.appendChar(m_AssignChar);

    if( eventDescription.sp31ed_text_len > 0 )
        theReply.appendStringN((const char*)eventDescription.sp31ed_text_value, (size_t)eventDescription.sp31ed_text_len);

    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
}

bool DBMSrvCmdEvent_EventWait::m_EventCategoriesRead(false);
Tools_Vector<DBMSrvDBE_EventMapping*> DBMSrvCmdEvent_EventWait::m_CategoryMapping;

void DBMSrvCmdEvent_EventWait::intializeEventCategories( const char* dbName, DBMSrv_Reply& theReply ) {

    // read the event categories if not done yet
    if( DBMSrvCmdEvent_EventWait::m_EventCategoriesRead )
        return;

    tsp00_DbNamec dbNameC;
    dbNameC.rawAssign(dbName);
    DBMSrvMsg_Error oParamError;
    DBMSrvPar_Parameters* kernelParameters = DBMSrvPar_Parameters::GetInstance(dbNameC, true, oParamError);
    if( kernelParameters == NULL ) {
        theReply.startWithMessageList(oParamError);
        return;
    }

    DBMSrvPar_ClassIterator classEventsIterator(kernelParameters, DBMSrvPar_Parameter::XPClassValueEvents);
    while( *classEventsIterator != NULL ) {
        DBMSrvDBE_EventMapping* newMappingEntry = new DBMSrvDBE_EventMapping;
        newMappingEntry->name = (*classEventsIterator)->szParameterName.asCharp();
        if( (*classEventsIterator)->pHelp != NULL ) {
            newMappingEntry->id =
                    (atoi((*classEventsIterator)->pHelp->Line()))==0?
                            sp31ei_nil:
                            (tsp31_event_ident_Enum)(atoi((*classEventsIterator)->pHelp->Line()));
        }
        else
            newMappingEntry->id = sp31ei_nil;

        m_CategoryMapping.push_back(newMappingEntry);
        ++classEventsIterator;
    }
        
    delete kernelParameters;
    DBMSrvCmdEvent_EventWait::m_EventCategoriesRead = true;
}

DBMSrvCmdEvent_EventWait::DBMSrvCmdEvent_EventWait()
        : DBMSrv_Command( DBMSrv_Command::KeyEventWait, false ) {
    m_longHelp=
        "@command event_wait With this command, you wait until an event is available. As "
            "soon as one is available, this command returns and the event data is displayed. This command "
            "implicitly creates an event session in the kernel."
        "@preconditions You have the server authorization DBInfoRead."
        "@syntax event_wait"
        DBMSrvCmdEvent_LongHelpReplyPart;
}

const char* const DBMSrvCmdEvent_EventWait::m_LabelName("NAME");
const char* const DBMSrvCmdEvent_EventWait::m_LabelPriority("PRIORITY");
const char* const DBMSrvCmdEvent_EventWait::m_LabelValue1("VALUE1");
const char* const DBMSrvCmdEvent_EventWait::m_LabelValue2("VALUE2");
const char* const DBMSrvCmdEvent_EventWait::m_LabelDate("DATE");
const char* const DBMSrvCmdEvent_EventWait::m_LabelTime("TIME");
const char* const DBMSrvCmdEvent_EventWait::m_LabelCount("COUNT");
const char* const DBMSrvCmdEvent_EventWait::m_LabelText("TEXT");
const char DBMSrvCmdEvent_EventWait::m_AssignChar('=');
