/*!
    \file    DBMSrvCmdDb_DbOnline.cpp
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

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOnline.hpp"

DBMSrvCmdDb_DbOnline::DBMSrvCmdDb_DbOnline(bool obsolete)
//: DBMSrvCmd_ShmUsingCommand( DBMSrv_Command::KeyUsingCommandFactory, obsolete )
: DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, obsolete )
{
    m_longHelp=
        "@command db_online Put the database in operational state ONLINE. This command "
            "requires a database name."
        "@preconditions You have the server authorization DBStart."
        "@syntax db_online [-f|-s|-t] [-u <yyyymmdd> <hhmmss>]"
        "@reply OK<NL>";
}

tcn00_Error DBMSrvCmdDb_DbOnline::run(
//tcn00_Error DBMSrvCmdDb_DbOnline::runUsingSharedMemory(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {
    

    tcn00_Error out(OK_CN00);
    
    out = cn40DBWarm( vcontrol, command, theReply.giveData(), theReply.giveLength(), theReply.giveMaximalLength());
    theReply.adaptLength();

    return out;
}
