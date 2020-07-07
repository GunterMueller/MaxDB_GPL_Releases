/*!
  \file    DBMSrvCmdDbmShm_DbmShmInfo.cpp
  \author  MarcW
  \ingroup DBMServer shared memory administration commands
  \brief   list information about shared memory

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

#include "DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmInfo.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmInfoReplier.hpp"

DBMSrvCmdDbmShm_DbmShmInfo::DBMSrvCmdDbmShm_DbmShmInfo()
: DBMSrv_Command( DBMSrv_Command::KeyDbmShmInfo, false )
{
    m_longHelp=
        "@command dbm_shm_info This command lists information about the DBM shared "
            "memory: version, locking status, connected processes and information about "
            "the persistence if necessary."
        "@preconditions You have the server authorization SharedMemoryAdmin."
        "@syntax dbm_shm_info"
        "@reply OK<NL>"
            "VERSION<NL>"
            "Required <req_version><NL>"
            "Found    <curr_version><NL>"
            "<NL>"
            "LOCKING<NL>"
            "Lock  <status_lock><NL>"
            "Reserve lock <status_reserve_lock><NL>"
            "Locking process <pid><NL>"
            "<NL>"
            "PROCESSES<NL>"
            "pid | status<NL>"
            "<pid_1> | <status_1><NL>"
            "...<NL>"
            "<pid_n> | <status_n><NL>"
            "<NL>"
            "[PERSISTENCE<NL>"
            "<error_message>]"
        "@replyvalue <pid_i> process id of a connected process"
        "@replyvalue <status_i> status of process <pid_i>. Status can be \"ALIVE\", \"DEAD\" "
            "or \"UNKNOWN\""
        "@replyvalue <req_version> required version of shared memory"
        "@replyvalue <curr_version> version of shared memory found on disk"
        "@replyvalue <status_lock> status of shared memory lock. Can be \"LOCKED\" "
            "or \"UNLOCKED\""
        "@replyvalue <status_reserve_lock> status of reserve shared memory lock. Can be \"LOCKED\" "
            "or \"UNLOCKED\""
        "@replyvalue <pid> process id of process that is supposed to have the lock"
        "@replyvalue <error_message> error text, if the persistence is incomplete";
}

tcn00_Error DBMSrvCmdDbmShm_DbmShmInfo::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    DBMSrvShM_ShmInfoReplier replier(vcontrol->dbname.asCharp(), theReply);
    return replier.replyInfo();
}

const char* const DBMSrvCmdDbmShm_DbmShmInfo::m_CommandName = "dbm_shm_info";
