/*!
  \file    DBMSrvCmdDbm_ListMemoryerror.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   list memory errors if exist

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

#include "hsp77.h"
#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmListMemoryerror.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Info.hpp"

DBMSrvCmdDbm_DbmListMemoryerror::DBMSrvCmdDbm_DbmListMemoryerror()
: DBMSrv_Command( DBMSrv_Command::KeyDbmListMemoryerror, false, 0, true )
{
    m_longHelp=
        "@command dbm_list_memoryerror This commands list memory manipulation errors. "
            "Note that even if this command does not list any errors, they might have "
            "occurred... "
        "@preconditions none"
        "@syntax dbm_list_memoryerror"
        "@reply OK";
}

tcn00_Error DBMSrvCmdDbm_DbmListMemoryerror::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    if( ToolsMeMa_GuardedOperation::errorFound() ) {
        const Msg_List* memErrors(&ToolsMeMa_GuardedOperation::getErrorList());
        char outMessage[2048];
        Msg_List outList;

        while(memErrors != NULL){
            sp77sprintf(
                outMessage,
                2047,
                "%s (at file: %s, line %d)",
                memErrors->Message(),
                memErrors->FileName(),
                memErrors->LineNumber());

            outList.Overrule(Msg_List(memErrors->Component(),
                                      "",
                                      0,
                                      memErrors->SystemRC(),
                                      memErrors->Type(),
                                      memErrors->ID(),
                                      outMessage,
                                      (SAPDB_UInt4) 0));

            memErrors = memErrors->NextMessage();
        }
        theReply.startWithMessageList(outList);
    }
    else {
        theReply.startWithOK();
    }
    return OK_CN00;
}
