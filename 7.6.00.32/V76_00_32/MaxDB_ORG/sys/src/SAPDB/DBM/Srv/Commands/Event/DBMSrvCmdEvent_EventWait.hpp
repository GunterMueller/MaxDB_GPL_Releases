/*!
  \file    DBMSrvCmdEvent_Event.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   declaration of classes for handling DBMServer
           commands dealing with events

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

#include "vsp0031.h"

#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "ToolsCommon/Tools_Vector.hpp"
#include "DBM/Srv/DatabaseEvents/DBMSrvDBE_EventMapping.hpp"

#if !defined(DBMSrvCmdEvent_EventWait_hpp)
#define DBMSrvCmdEvent_EventWait_hpp

/*! 
    @brief Class for handling the DBMServer command event_wait
*/
class DBMSrvCmdEvent_EventWait: public DBMSrv_Command {
public:
    /*! @brief constructor */
    DBMSrvCmdEvent_EventWait();

    /*! 
        @brief the actual implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error run(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply& theReply);

    static void replyEvent(tsp31_event_description& eventDescription, DBMSrv_Reply& theReply);

private:
    static void intializeEventCategories(const char* dbName, DBMSrv_Reply& theReply);
    
    static bool m_EventCategoriesRead;
    static Tools_Vector<DBMSrvDBE_EventMapping*> m_CategoryMapping;

    static const char* const m_LabelName;
    static const char* const m_LabelPriority;
    static const char* const m_LabelValue1;
    static const char* const m_LabelValue2;
    static const char* const m_LabelDate;
    static const char* const m_LabelTime;
    static const char* const m_LabelCount;
    static const char* const m_LabelText;
    static const char m_AssignChar;
};

/*! @brief reused in event_wait and event_receive */
#define DBMSrvCmdEvent_LongHelpReplyPart \
        "@reply OK<NL>" \
            "NAME=<event_category><NL>" \
            "PRIORITY=<priority><NL>" \
            "VALUE1=<value1><NL>" \
            "VALUE2=<value2><NL>" \
            "DATE=<yyyymmdd><NL>" \
            "TIME=<HHmmss><NL>" \
            "COUNT=<count><NL>" \
            "TEXT=<text><NL>" \
            "@replyvalue <event_category> name of the event's category" \
            "@replyvalue <priority> event's priority (LOW, MEDIUM or HIGH)" \
            "@replyvalue <value1> event's value1 (meaning depends on value of event_category)" \
            "@replyvalue <value2> event's value2 (meaning depends on value of event_category)" \
            "@replyvalue <yyyymmdd> Date of event creation (year, month, day)" \
            "@replyvalue <HHmmss> Time of event creation (hour, minutes, seconds)" \
            "@replyvalue <count> count of event (numbering starts when database status " \
                "is set to ONLINE)" \
            "@replyvalue <text> can be empty, meaning depends on value of event_category"

#endif //DBMSrvCmdEvent_EventWait_hpp
