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

#include "RunTime/System/RTESys_ProcessID.hpp"
#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetpid.hpp"

DBMSrvCmdDbm_DbmGetpid::DBMSrvCmdDbm_DbmGetpid()
: DBMSrv_Command( DBMSrv_Command::KeyDbmGetpid, false, 0, true )
{
    m_longHelp=
        "@command dbm_getpid This commands returns the process id of the DBM server, "
            "that this client is connected to."
        "@preconditions none"
        "@syntax dbm_getpid"
        "@reply OK<NL>"
            "<pid>"
            "@replyvalue <pid> process ID of the DBM server";
}

tcn00_Error DBMSrvCmdDbm_DbmGetpid::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    theReply.startWithOK();
    
    char buf[32];
    sprintf(buf, "%d", (int)RTESys_ProcessID());
    theReply.appendLine(buf);

    return OK_CN00;
}
