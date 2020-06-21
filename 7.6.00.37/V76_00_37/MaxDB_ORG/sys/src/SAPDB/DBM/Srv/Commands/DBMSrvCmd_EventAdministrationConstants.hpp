/*!
  \file    DBMSrvCmd_EventAdministrationConstants.hpp
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

#if !defined(DBMSrvCmd_EventAdministrationConstants_hpp)
#define DBMSrvCmd_EventAdministrationConstants_hpp

#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "vsp0031.h"

class DBMSrvCmd_EventAdministrationConstants {
public:
    DBMSrvCmd_EventAdministrationConstants() {}
    virtual ~DBMSrvCmd_EventAdministrationConstants() {}

    static tsp31_event_prio_Enum getEventPriorityFromText(const Tools_DynamicUTF8String& eventPriorityAsText);
    static Tools_DynamicUTF8String getTextFromEventPriority(const tsp31_event_prio_Enum eventPriority); 

    static const char* const m_EventCategoryUnknown;
    static const char* const m_TextOff;
    static const char* const m_TextLow;
    static const char* const m_TextMedium;
    static const char* const m_TextHigh;
};

#endif //DBMSrvCmd_EventAdministrationConstants_hpp
