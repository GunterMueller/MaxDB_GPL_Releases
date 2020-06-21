/*!
  \file    DBMSrvCmdEvent_EventList.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   list all event

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

#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventList.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp"

DBMSrvCmdEvent_EventList::DBMSrvCmdEvent_EventList()
        : DBMSrvCmd_EventAdministrationCommand( DBMSrv_Command::KeyEventList, false, m_LongHelp ) {
}

tcn00_Error DBMSrvCmdEvent_EventList::runEventAdministrationCommand(
        VControlDataT* vcontrol,
        DBMSrvPar_Parameters* kernelParameters,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    DBMSrvMsg_Error localErrList;
            
    theReply.startWithOK();
    DBMSrvPar_ClassIterator classEventsIterator(kernelParameters, DBMSrvPar_Parameter::XPClassValueEvents);

    size_t maxNameWidth(0);
    size_t maxValueWidth(0);
    size_t maxPrioWidth(0);

    char* eventCategoryHelpText(NULL);
    bool calculatingLoop(true);
    do {
        if( calculatingLoop ) {
            size_t currPrioWidth(getTextFromEventPriority(sp31ep_nil).Length());
            if( currPrioWidth > maxPrioWidth )
                maxPrioWidth = currPrioWidth;
            currPrioWidth = getTextFromEventPriority(sp31ep_low).Length();
            if( currPrioWidth > maxPrioWidth )
                maxPrioWidth = currPrioWidth;
            currPrioWidth = getTextFromEventPriority(sp31ep_medium).Length();
            if( currPrioWidth > maxPrioWidth )
                maxPrioWidth = currPrioWidth;
            currPrioWidth = getTextFromEventPriority(sp31ep_high).Length();
            if( currPrioWidth > maxPrioWidth )
                maxPrioWidth = currPrioWidth;
        }

        classEventsIterator.reset();
        while( *classEventsIterator != NULL ) {
            // there is one more parameter to list        
            if( calculatingLoop ) {
                size_t currNameWith(SAPDB_strlen((*classEventsIterator)->szParameterName.asCharp()));
                if( currNameWith > maxNameWidth )
                    maxNameWidth = currNameWith;
            }

            if( !calculatingLoop ) {
                // compile parameter's help text
                DBMSrvPar_XPLine* helpLine((*classEventsIterator)->pHelp);
                size_t helpLength(0);
                // ignore line containing the event index
                if( helpLine != NULL )
                    helpLine = helpLine->Next();

                while (helpLine != NULL) {
                    helpLength += SAPDB_strlen(helpLine->Line());
                    if( helpLine->Line()[0] == ' ' )
                        ++helpLength;
                    helpLine = helpLine->Next();
                    if( helpLine!= NULL && helpLine->Line()[0] != ' ' )
                        ++helpLength;
                }
                
                eventCategoryHelpText = new char[helpLength+1];
                if( eventCategoryHelpText != NULL ) {
                    eventCategoryHelpText[0] = '\0';
                    helpLine = (*classEventsIterator)->pHelp;
                    // ignore line containing the event index
                    if( helpLine != NULL )
                        helpLine = helpLine->Next();

                    while (helpLine != NULL) {
                        SAPDB_strcat(eventCategoryHelpText, helpLine->Line());
                        helpLine = helpLine->Next();
                        if( helpLine!= NULL && helpLine->Line()[0] != ' ' )
                            SAPDB_strcat(eventCategoryHelpText, " ");
                    }
                }
            }

            if( (*classEventsIterator)->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_String ) {
                // it's a complex event category
                SAPDBDBE_EventCategory eventCategory(
                        localErrList,
                        (*classEventsIterator)->szActiveValue.asCharp());

                if( !localErrList.IsEmpty() ) {
                    // could not create category object
                    return theReply.startWithMessageList(localErrList);
                }

                while( eventCategory.hasNext() ) {
                    const SAPDBDBE_EventDefinition& eventDef(eventCategory.getNext());
                    if( calculatingLoop ) {
                        size_t currValueWith(SAPDB_strlen((const char*)SAPDB_ToString(eventDef.getValue())));
                        if( currValueWith > maxValueWidth )
                            maxValueWidth = currValueWith;
                    }
                    else {
                        // event name
                        theReply.appendStringWithMinWidth(
                                (*classEventsIterator)->szParameterName.asCharp(),
                                maxNameWidth);
                        
                        // event priority
                        theReply.appendStringWithMinWidth(
                                getTextFromEventPriority(eventDef.getPriority()).CharPtr(),
                                maxPrioWidth);

                        // event value
                        theReply.appendStringWithMinWidth(
                                (const char*)SAPDB_ToString(eventDef.getValue()),
                                maxValueWidth);

                        // help text
                        if( eventCategoryHelpText != NULL )
                            theReply.appendString(eventCategoryHelpText);

                        // newline
                        theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
                    }
                }
            }
            else if( (*classEventsIterator)->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Integer ) {
                // it's a simple event category, no value available
                if( !calculatingLoop ) {
                    // event name
                    theReply.appendStringWithMinWidth(
                            (*classEventsIterator)->szParameterName.asCharp(),
                            maxNameWidth);
                    
                    // event priority
                    tsp31_event_prio_Enum eventPrio =
                            (tsp31_event_prio_Enum)(atoi((*classEventsIterator)->szActiveValue.asCharp()));
                    theReply.appendStringWithMinWidth(
                            getTextFromEventPriority(eventPrio).CharPtr(),
                            maxPrioWidth);
                    
                    // spaces at the value column
                    theReply.appendCharacterRepeatedly(' ', maxValueWidth);

                    // help text
                    if( eventCategoryHelpText != NULL )
                        theReply.appendString(eventCategoryHelpText);

                    // newline
                    theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);

                }
            }
            else {
                // in this case we have a severe problem, not the user's error...
                return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, "CLASS EVENTS")));
            }
            if( eventCategoryHelpText != NULL ) {
                delete[] eventCategoryHelpText;
                eventCategoryHelpText = NULL;
            }
            ++classEventsIterator;
        }
        
        if( calculatingLoop ) {
            if( maxNameWidth > 0 ) {
                // there are events...
                if( SAPDB_strlen(m_TextName) > maxNameWidth ) {
                    maxNameWidth = SAPDB_strlen(m_TextName);
                }
                ++maxNameWidth;
                theReply.appendStringWithMinWidth(m_TextName, maxNameWidth);
            }

            if( maxPrioWidth > 0 ) {
                if( SAPDB_strlen(m_TextPrio) > maxPrioWidth ) {
                    maxPrioWidth = SAPDB_strlen(m_TextPrio);
                }
                ++maxPrioWidth;
                theReply.appendStringWithMinWidth(m_TextPrio, maxPrioWidth);
            }

            if( maxValueWidth > 0 ) {
                if( SAPDB_strlen(m_TextValue) > maxValueWidth ) {
                    maxValueWidth = SAPDB_strlen(m_TextValue);
                }
                ++maxValueWidth;
                theReply.appendStringWithMinWidth(m_TextValue, maxValueWidth);
            }

            if( maxNameWidth > 0 ) {
                theReply.appendString(m_TextHelp);
                theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
            }

            calculatingLoop = false;
        }
        else
            break;

    } while (true);

    return OK_CN00;
}

const char* const DBMSrvCmdEvent_EventList::m_TextName("Name");
const char* const DBMSrvCmdEvent_EventList::m_TextValue("Value");
const char* const DBMSrvCmdEvent_EventList::m_TextPrio("Priority");
const char* const DBMSrvCmdEvent_EventList::m_TextHelp("Description");

const char* const DBMSrvCmdEvent_EventList::m_LongHelp= 
    "@command event_list With this command, you list all active events."
    "@preconditions You have the DBM operator authorization DBInfoRead."
    "@syntax event_list"
    "@reply OK<NL>"
        "Name Priority Value Description<NL>"
        "<ec> <ev_pri> [<v>] <desc><NL>"
        "...<NL>"
    "@replyvalue <ec> event category"
    "@replyvalue <ev_pri> event priority"
    "@replyvalue <v> event value. Meaning depends on value of ec"
    "@replyvalue <desc> descriptive event text. Depends on value of ec";

