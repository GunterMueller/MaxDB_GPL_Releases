/*!
    \file    DBMSrvCmdDb_DbOffline.cpp
    \author  MarcW
    \ingroup DBMServer commands
    \brief   definition of class for handling DBMServer
             command db_online

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

#include "hcn40.h"

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOffline.hpp"

DBMSrvCmdDb_DbOffline::DBMSrvCmdDb_DbOffline(bool obsolete)
//: DBMSrvCmd_ShmUsingCommand( DBMSrv_Command::KeyUsingCommandFactory, obsolete )
: DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, obsolete )
{
    m_longHelp=    
        "@command db_offline Put the database in operational state OFFLINE. This command "
            "requires a database name."
        "@preconditions You have the server authorization DBStop."
        "@syntax db_offline [-dump]"
        "@reply OK<NL>";
}

tcn00_Error DBMSrvCmdDb_DbOffline::run(
//tcn00_Error DBMSrvCmdDb_DbOffline::runUsingSharedMemory(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {
        
    tcn00_Error out(OK_CN00);

    out = cn40DBOffline( vcontrol, command, theReply.giveData(), theReply.giveLength(), theReply.giveMaximalLength());
    theReply.adaptLength();

    return out;
}
