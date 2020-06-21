/*!
  \file    DBMSrvCmdEvent_EventRelease.cpp
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

#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventRelease.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"

DBMSrvCmdEvent_EventRelease::DBMSrvCmdEvent_EventRelease()
: DBMSrv_Command( DBMSrv_Command::KeyEventRelease, false, m_LongHelp )
{}

tcn00_Error DBMSrvCmdEvent_EventRelease::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    DBMSrvMsg_Error errOut;
    DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionEvent, errOut);
    if( !errOut.IsEmpty() )
        return theReply.startWithMessageList(errOut);
    else {
        theReply.startWithOK();
        return OK_CN00;
    }
}

const char* const DBMSrvCmdEvent_EventRelease::m_LongHelp= 
    "@command event_release With this command, you release the event session, which is implicitly created "
        "by the commands event_available, event_receive and event_wait."
    "@preconditions You have the DBM operator authorization DBInfoRead."
    "@syntax event_release"
    "@reply OK<NL>";
