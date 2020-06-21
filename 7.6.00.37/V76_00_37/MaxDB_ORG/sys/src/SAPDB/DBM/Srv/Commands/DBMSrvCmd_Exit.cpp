/*!
  \file    DBMSrvCmd_Exit.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   exit the DBMServer

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

#include "DBM/Srv/Commands/DBMSrvCmd_Exit.hpp"
#include "DBM/Srv/Framework/DBMSrvFrm_Server.hpp"

DBMSrvCmd_Exit::DBMSrvCmd_Exit()
: DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false )
{
    m_longHelp=
        "@command exit You close the Database Manager CLI program."
        "@preconditions none"
        "@syntax exit"
        "@reply OK<NL>";
}

tcn00_Error DBMSrvCmd_Exit::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    DBMSrvMsg_Error errOut;
    theReply.startWithOK();

    if( !DBMSrvFrm_Server::Instance().stop(errOut) )
        return theReply.startWithMessageList(errOut);
    else
        return OK_CN00;
}
