/*!
  \file    DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   

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

#include "DBM/Srv/Commands/Dbmevtdisp/Internal/DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp"


DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd::DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd()
        : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, m_LongHelp, true )
{}

tcn00_Error DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    theReply.startWithOK();

    if( atoi(command->oArguments.getValue(1).CharPtr()) == DBMSrvShM_LineAccessorEventing::INTERNAL &&
        !cn00DBMServerData::vcontrol()->bIsLocalCommunication ) {
        // the user is trying to start an internal event dispatcher remotely...
        return theReply.startWithMessageList(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_DISP_INTERNAL_NOT_REMOTE));
    }

    DBMSrvMsg_Error accessorError;
    DBMSrvShM_LineAccessorEventing newEventingLine(
            command->oArguments.getValue(1).CharPtr(),
            command->oArguments.getValue(2).CharPtr(),
            command->oArguments.getValue(3).CharPtr(),
            command->oArguments.getValue(4).CharPtr(),
            vcontrol->m_ClientMachineID.CharPtr(),
            command->oArguments.getValue(5).CharPtr(),
            command->oArguments.getValue(6).CharPtr(),
            accessorError);

    if( !accessorError.IsEmpty() )
        return theReply.startWithMessageList(accessorError);

    if( newEventingLine.surplusHidden() ) {
        // there is already an internal event dispatcher running
        return theReply.startWithMessageList(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_DISP_INTERNAL_EXISTS));
    }

    char newIndexString[512];
    sprintf(newIndexString, "%d", newEventingLine.getInstanceID());
    theReply.appendLine(newIndexString);
    return OK_CN00;
}

const char* const DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd::m_LongHelp =
        "@command dbmevtdisp_internal_add add an event dispatcher"
        "@reply OK";

const char* DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd::getCommandName() {
    return "dbmevtdisp_internal_add";
}
