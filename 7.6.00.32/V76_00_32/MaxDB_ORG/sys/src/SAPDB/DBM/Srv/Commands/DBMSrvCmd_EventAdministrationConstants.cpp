/*!
  \file    DBMSrvCmd_EventAdministrationConstants.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief

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

#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp"

tsp31_event_prio_Enum DBMSrvCmd_EventAdministrationConstants::getEventPriorityFromText(
        const Tools_DynamicUTF8String& eventPriorityAsText) {
    if( eventPriorityAsText == Tools_DynamicUTF8String(m_TextHigh) )
        return sp31ep_high;

    if( eventPriorityAsText == Tools_DynamicUTF8String(m_TextMedium) )
        return sp31ep_medium;

    if( eventPriorityAsText == Tools_DynamicUTF8String(m_TextLow) )
        return sp31ep_low;
    
    return sp31ep_nil;
}

Tools_DynamicUTF8String DBMSrvCmd_EventAdministrationConstants::getTextFromEventPriority(
        const tsp31_event_prio_Enum eventPriority) {
    switch( eventPriority ) {
        case sp31ep_high:
            return Tools_DynamicUTF8String(m_TextHigh);
        case sp31ep_medium:
            return Tools_DynamicUTF8String(m_TextMedium);
        case sp31ep_low:
            return Tools_DynamicUTF8String(m_TextLow);
        default:
            return Tools_DynamicUTF8String(m_TextOff);
    }
}

const char* const DBMSrvCmd_EventAdministrationConstants::m_TextOff("OFF");
const char* const DBMSrvCmd_EventAdministrationConstants::m_TextLow("LOW");
const char* const DBMSrvCmd_EventAdministrationConstants::m_TextMedium("MEDIUM");
const char* const DBMSrvCmd_EventAdministrationConstants::m_TextHigh("HIGH");
const char* const DBMSrvCmd_EventAdministrationConstants::m_EventCategoryUnknown("UNKNOWN");

