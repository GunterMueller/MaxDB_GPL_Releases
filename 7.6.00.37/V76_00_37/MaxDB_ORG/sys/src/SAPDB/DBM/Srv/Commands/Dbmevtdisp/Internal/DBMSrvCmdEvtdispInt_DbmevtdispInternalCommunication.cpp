/*!
  \file    DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication.cpp
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

#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp"
#include "DBM/Srv/Commands/Dbmevtdisp/Internal/DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineIteratorEventing.hpp"
#include "hcn50.h"

DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication()
        : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, m_LongHelp, true )
{}

tcn00_Error DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    theReply.startWithOK();

    DBMCliEvtDisp_SrvCommand::ID srvCommand;
    srvCommand = (DBMCliEvtDisp_SrvCommand::ID) atoi(command->oArguments.getValue(1).CharPtr());

    int instanceID(0);
    DBMSrvMsg_Error outList;

    switch( srvCommand ) {
        case DBMCliEvtDisp_SrvCommand::REQUESTCOMMAND:
            instanceID = atoi(command->oArguments.getValue(2).CharPtr());
            replyCommandToInstance(instanceID, outList, theReply);
            break;
        case DBMCliEvtDisp_SrvCommand::STATE:
            listInstances(outList, theReply);
            break;
        case DBMCliEvtDisp_SrvCommand::STOP:
            instanceID = atoi(command->oArguments.getValue(2).CharPtr());
            stopInstance(instanceID, outList);
            break;
        default:
            break;
    }

    if( !outList.IsEmpty() )
        return theReply.startWithMessageList(outList);
    else
        return OK_CN00;
}

bool DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::userHasPermissionToAccess(
        const char* lineOwner ) {
    
    bool permissionToAccess(cn00DBMServerData::vcontrol()->pCurrentUser->isServerRight(DBMRightEvtDispAdmin_CN50));
    if( !permissionToAccess ) {
        // this user is no dispatcher admin, check if she is the owner
        Tools_DynamicUTF8String instanceOwner(lineOwner);
        Tools_DynamicUTF8String currentUser(
                    cn00DBMServerData::vcontrol()->pCurrentUser->getUserName().asCharp());
        permissionToAccess = (currentUser == instanceOwner);
     }
     return permissionToAccess;
}

void DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::stopInstance(
        const int instanceID,
        DBMSrvMsg_Error& outList) {

    outList.ClearMessageList();
    
    DBMSrvShM_LineAccessorEventing instanceLine(instanceID, outList);
    if( !outList.IsEmpty() ) {
        outList.Overrule(
        DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_INTERNAL_NOACC,
        Msg_Arg("DISPID", instanceID)));
        return;
    }

    if( instanceLine.isValid() && instanceLine.existsEventing(outList) ) {

        if( !outList.IsEmpty() ) {
            outList.Overrule(
            DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_INTERNAL_NOACC,
            Msg_Arg("DISPID", instanceID)));
            return;
        }

        DBMSrvShM_LineAccessorEventing::VisibilityStatus vstat(DBMSrvShM_LineAccessorEventing::INTERNAL);
        if( !instanceLine.getVisibilityStatus(vstat, outList) || vstat == DBMSrvShM_LineAccessorEventing::INTERNAL ) {
            // no access to this instance
            outList.Overrule(
                    DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_INTERNAL_NOACC,
                    Msg_Arg("DISPID", instanceID)));
            return;
        }

        char* lineOwner(NULL);
        if( !instanceLine.getDBMUser(lineOwner, outList) ) {
            // no access to this instance
            outList.Overrule(
                    DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_INTERNAL_NOACC,
                    Msg_Arg("DISPID", instanceID)));
            if( lineOwner != NULL )
                delete[] lineOwner;
            return;
        }
        bool permissionToStop(userHasPermissionToAccess(lineOwner));
        if( lineOwner != NULL )
            delete[] lineOwner;
        if( !permissionToStop ) {
            outList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MISSRIGHT));
            return;
        }

        // check whether communication is allowed at all
        char* nodeName(NULL);
        if( !instanceLine.getNodeName(nodeName, outList) ) {
            outList.Overrule(
                    DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_INTERNAL_NOACC,
                    Msg_Arg("DISPID", instanceID)));
            if( nodeName != NULL )
                delete[] nodeName;
            return;
        }

        if( !cn00DBMServerData::vcontrol()->bIsLocalCommunication &&
            (nodeName != NULL && (0 != SAPDB_strcmp(nodeName, cn00DBMServerData::vcontrol()->m_ClientMachineID.CharPtr())))) {
            // I am serving a remote client
            DBMSrvShM_LineAccessorEventing::InternalCommStatus status;
            if( !instanceLine.getCommunicationStatus(status, outList) || status == DBMSrvShM_LineAccessorEventing::LOCAL ) {
                // the target dispatcher may only be accessed locally
                outList.Overrule(DBMSrvMsg_Error(
                        SDBMSG_DBMSRV_DISP_NO_REMOTE_ACCESS,
                        Msg_Arg("DISPID",instanceID)));
                if( nodeName != NULL )
                    delete[] nodeName;
                return;
            }
        }
        if( nodeName != NULL )
            delete[] nodeName;

        // this is the actual stopping
        if( !instanceLine.doCancel(outList) ) {
            outList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_NOT_STOPPED,
                Msg_Arg("DISPID",instanceID)));
            return;
        }

        // now wait until the line is removed...
        int waitCount(300); 
        while( waitCount > 0 && instanceLine.existsEventing(outList) ) {
            if( !outList.IsEmpty() ) {
                outList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DISP_NOT_STOPPED,
                    Msg_Arg("DISPID",instanceID)));
                return;
            }
            RTE_ISystem::DoSleep(1000);
            --waitCount;
        }

        if( instanceLine.existsEventing(outList) ) {
            outList.Overrule(DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_DISP_NOT_STOPPED,
                    Msg_Arg("DISPID",instanceID)));
        }
    }
    else {
        outList.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_DISP_NO_EXIST,
                Msg_Arg("DISPID",instanceID)));
    }
}

void DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::replyCommandToInstance(
        const int instanceID,
        DBMSrvMsg_Error& outList,
        DBMSrv_Reply& theReply) {
    
    // check if there is a command for instance instanceID
    // currently, we can only check if it must stop...

    outList.ClearMessageList();

    DBMSrvShM_LineAccessorEventing instanceLine(instanceID, outList );
    if( outList.IsEmpty() && instanceLine.isValid() && instanceLine.existsEventing(outList) ) {
        bool cancelled(false);
        if( instanceLine.isCancelled(cancelled, outList) ) {
            char replyCmd[10];
            if( cancelled ) {
                // someone wants to stop that instance, tell it to do so
                sprintf(replyCmd, "%d", (int)DBMCliEvtDisp_SrvCommand::STOP);
            }
            else {
                sprintf(replyCmd, "%d", (int)DBMCliEvtDisp_SrvCommand::NONE);
            }
            theReply.appendLine(replyCmd);
        }
        else {
            // could not find out if instance is cancelled
            outList.Overrule(DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_DISP_NO_INTCMD_FOUND,
                    Msg_Arg("DISPID",instanceID)));
        }
    }
    else {
        outList.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_DISP_NO_EXIST,
                Msg_Arg("DISPID",instanceID)));
    }
}

void DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::listInstances(
        DBMSrvMsg_Error& outList,
        DBMSrv_Reply& theReply) {

    
    DBMSrvShM_LineIteratorEventing eventingIterator(outList);
    if( eventingIterator.isValid() ){
        while ( eventingIterator.hasNext() ) {
            if( userHasPermissionToAccess(eventingIterator.getDBMUser()) ) {
                char buf[2000];
                sprintf(
                        buf,
                        "%d,%s,%s,%s,%s",
                        eventingIterator.getInstanceID(),
                        eventingIterator.getDBMUser(),
                        eventingIterator.getNodeName(),
                        eventingIterator.getConfigFilePath(),
                        eventingIterator.getLogFilePath());
                theReply.appendLine(buf);
            }
            eventingIterator.next();
        }
    }
    else {
        outList.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_DISP_NO_LIST));
    }
}

const char* const DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::m_LongHelp =
        "@command dbmevtdisp_internal_communication communication between event dispatchers"
        "@reply OK";

const char* DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication::getCommandName() {
    return "dbmevtdisp_internal_communication";
}