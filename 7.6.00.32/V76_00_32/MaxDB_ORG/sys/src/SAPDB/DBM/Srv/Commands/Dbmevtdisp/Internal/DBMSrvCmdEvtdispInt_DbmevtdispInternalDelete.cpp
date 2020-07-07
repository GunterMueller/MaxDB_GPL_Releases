/*!
  \file    DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete.cpp
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

#include "DBM/Srv/Commands/Dbmevtdisp/Internal/DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication.hpp"
#include "DBM/Srv/Commands/Dbmevtdisp/Internal/DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete.hpp"
#include "SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp"

DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete::DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete()
        : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, m_LongHelp, true )
{}

tcn00_Error DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    theReply.startWithOK();

    // the client has already checked that this is numeric
    int instanceID(atoi(command->oArguments.getValue(1).CharPtr()));
    
    DBMSrvMsg_Error errOut;
    DBMSrvShM_LineAccessorEventing instanceEventingLine(instanceID, errOut);

    if( instanceEventingLine.existsEventing(errOut) && instanceEventingLine.isValid() ) {
        // check if the user has sufficient permissions
        char* lineOwner(NULL);
        if( !instanceEventingLine.getDBMUser(lineOwner, errOut) )
            return theReply.startWithMessageList(errOut);

        bool permissionToDelete(DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::userHasPermissionToAccess(lineOwner));
        delete[] lineOwner;

        if( permissionToDelete ) {
            // check cancel flag
            bool isCancelled(false);
            if( !instanceEventingLine.isCancelled(isCancelled, errOut) )
                return theReply.startWithMessageList(errOut);

            if( !isCancelled && !cn00DBMServerData::vcontrol()->bIsLocalCommunication ) {
                // trying to remove a line that is not cancelled remotely
                return theReply.startWithMessageList(DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_DISP_NO_DELETE,
                    Msg_Arg("DISPID",instanceID)));
            }
            else {
                // here it happens (only if the dispatcher is stopping or the client of
                // this DBM Server is local)
                if( !instanceEventingLine.removeLineEventing(errOut) )
                    return theReply.startWithMessageList(errOut);
            }
        }
        else {
            return theReply.startWithMessageList((DBMSrvMsg_Error(SDBMSG_DBMSRV_MISSRIGHT)));
        }
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_NO_EXIST,
            Msg_Arg("DISPID",instanceID)));
        return theReply.startWithMessageList(errOut);
    }
    
    return OK_CN00;
}

const char* const DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete::m_LongHelp =
        "@command dbmevtdisp_internal_delete delete an event dispatcher"
        "@reply OK";

const char* DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete::getCommandName() {
    return "dbmevtdisp_internal_delete";
}