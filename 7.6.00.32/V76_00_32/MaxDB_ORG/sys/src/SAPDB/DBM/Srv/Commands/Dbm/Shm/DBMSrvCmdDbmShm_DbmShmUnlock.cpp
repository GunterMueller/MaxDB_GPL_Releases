/*!
  \file    DBMSrvCmdDbmShm_DbmShmUnlock.cpp
  \author  MarcW
  \ingroup DBMServer commands for administration of shared memory
  \brief   unlock shared memory

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

#include "SAPDB/DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmUnlock.hpp"

DBMSrvCmdDbmShm_DbmShmUnlock::DBMSrvCmdDbmShm_DbmShmUnlock()
    : DBMSrv_Command( DBMSrv_Command::KeyDbmShmUnlock, true ) 
{
    m_longHelp =
        "@command dbm_shm_unlock This command is obsolete."
        "@syntax dbm_shm_unlock"
        "@reply OK";
}

tcn00_Error DBMSrvCmdDbmShm_DbmShmUnlock::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & theReply) {

    theReply.startWithOK();
    return OK_CN00;
}
