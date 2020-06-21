/*!
  \file    DBMSrvCmdEvent_EventReceive.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   command event_receive
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

#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventWait.hpp"
#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventReceive.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"

tcn00_Error DBMSrvCmdEvent_EventReceive::run(
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
                (requestError == DBMSrvMsg_Error::DBMSrv_RTE     )    ) {
                DBMSrvMsg_Error releaseError;
                DBMSrvKnl_GlobalSessions::ReleaseSession(DBMSrvKnl_GlobalSessions::SessionEvent, releaseError);
                releaseError.Overrule(requestError);
                requestError = releaseError;
             } // end if
            return theReply.startWithMessageList(requestError);
        } // end if
    } // end if

    if (!pSession->ReplyAvailable()) {
        return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_EVENTNONEAVAIL));
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

DBMSrvCmdEvent_EventReceive::DBMSrvCmdEvent_EventReceive()
: DBMSrv_Command( DBMSrv_Command::KeyEventReceive, false, m_LongHelp )
{}

const char* const DBMSrvCmdEvent_EventReceive::m_LongHelp =
        "@command event_receive With this command, an event is received, if one is available. "
            "If no event is available, this command returns with an error. "
            "Whether an event is available can be checked with the command event_available."
        "@preconditions You have the server authorization DBInfoRead."
        "@syntax event_receive"
        DBMSrvCmdEvent_LongHelpReplyPart;
