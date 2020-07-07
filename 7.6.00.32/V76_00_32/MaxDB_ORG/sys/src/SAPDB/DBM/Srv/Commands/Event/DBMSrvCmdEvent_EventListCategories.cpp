/*!
  \file    DBMSrvCmdEvent_EventListCategories.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   list all event categories

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

#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventListCategories.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp"

DBMSrvCmdEvent_EventListCategories::DBMSrvCmdEvent_EventListCategories()
        : DBMSrvCmd_EventAdministrationCommand(
                DBMSrv_Command::KeyEventListCategories,
                false,
                m_LongHelp )
{}

tcn00_Error DBMSrvCmdEvent_EventListCategories::runEventAdministrationCommand(
        VControlDataT* vcontrol,
        DBMSrvPar_Parameters* kernelParameters,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    theReply.startWithOK();
    DBMSrvPar_ClassIterator classEventsIterator(kernelParameters, DBMSrvPar_Parameter::XPClassValueEvents);

    classEventsIterator.reset();
    while( *classEventsIterator != NULL ) {
        // there is one more parameter to list        

        DBMSrvPar_XPLine* explainLine=(*classEventsIterator)->pExplain;
        const char *      notYetImplementedFlag="THIS FEATURE IS NOT YET IMPLEMENTED.";

        if(0!=strnicmp(notYetImplementedFlag, explainLine->Line(), strlen(notYetImplementedFlag))) //explain text does not mark the event category as not yet implemented
        {
            // event category name
            theReply.appendLine((*classEventsIterator)->szParameterName.asCharp());

            // parameter's explain text line by line
            for( ; explainLine != NULL; explainLine = explainLine->Next() )
                theReply.appendLine(explainLine->Line());

            // empty line
            theReply.appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
        }

        ++classEventsIterator;
    }
    return OK_CN00; // cool, this command never returns an error...
}

const char* const DBMSrvCmdEvent_EventListCategories::m_LongHelp= 
    "@command event_list_categories With this command, you list all event categories including those "
        "categories, that contain no active events. For each event category a descriptive text is "
        "displayed."
    "@preconditions You have the server authorization DBInfoRead."
    "@syntax event_list_categories"
    "@reply OK<NL>"
        "<event_category_i><NL>"
        "<description_i><NL>"
        "<NL>"
        "<event_category_j><NL>"
        "<description_j><NL>"
        "..."
    "@replyvalue <event_category_i> name of event category i"
    "@replyvalue <description_i> descriptive text for event category i"
    "@replyvalue <event_category_j> name of event category j"
    "@replyvalue <description_j> descriptive text for event category j";
