/*!
  \file    DBMSrvCmdEvent_EventAvailable.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   command event_available

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

#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventAvailable.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

tcn00_Error DBMSrvCmdEvent_EventAvailable::run(
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

    if (!pSession->ReplyAvailable()) {
        return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_EVENTNONEAVAIL));
    }
    else {
        theReply.startWithOK();
        return OK_CN00;
    }
}


DBMSrvCmdEvent_EventAvailable::DBMSrvCmdEvent_EventAvailable()
: DBMSrv_Command( DBMSrv_Command::KeyEventAvailable, false, m_LongHelp )
{}

const char* const DBMSrvCmdEvent_EventAvailable::m_LongHelp=
        "@command event_available With this command, you check whether an event is available "
            "and can be received with the command event_receive or event_wait. If the reply is OK, there is "
            "an event available, otherwise there is no event available."
        "@preconditions You have the server authorization DBInfoRead."
        "@syntax event_available"
        "@reply OK<NL>";

