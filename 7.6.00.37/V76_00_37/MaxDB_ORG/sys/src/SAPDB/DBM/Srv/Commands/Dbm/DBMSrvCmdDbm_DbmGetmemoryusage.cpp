/*!
  \file    DBMSrvCmdDbm_DbmGetmemoryusage.cpp
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

#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetmemoryusage.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ProcessData.hpp"
#include "RunTime/System/RTESys_ProcessID.hpp"

DBMSrvCmdDbm_DbmGetmemoryusage::DBMSrvCmdDbm_DbmGetmemoryusage()
        : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, m_LongHelp, true ) {
}

tcn00_Error DBMSrvCmdDbm_DbmGetmemoryusage::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    theReply.startWithOK();

    DBMSrvMsg_Error errOut;

    DBMSrvProc_ProcessData myData(RTESys_ProcessID(), errOut);
    if( !errOut.IsEmpty() )
        return theReply.startWithMessageList(errOut);

    size_t myCurrentMemoryConsumtion = myData.getMemorySize(errOut);
    if( !errOut.IsEmpty() ) {
        if( errOut == DBMSrvMsg_Error::DBMSrv_PROCESSDATA_SIZE_NOTSUPPORTED )
            errOut.Overrule(DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_NOTIMPL,
                    Msg_Arg("COMMAND_NAME",getCommandName())));
        return theReply.startWithMessageList(errOut);
    }

    theReply.appendLine( ToStr(myCurrentMemoryConsumtion, _T_d ) );
    return OK_CN00;
}

const char* const DBMSrvCmdDbm_DbmGetmemoryusage::m_LongHelp =
    "@command dbm_getmemoryusage With this command, you display the current virtual memory consumption of "
        "the DBM server in bytes."
    "@reply OK<NL>"
        "<bytes>"
        "@replyvalue <bytes> amount of used virtual memory in bytes";

const char* DBMSrvCmdDbm_DbmGetmemoryusage::getCommandName() {
    return "dbm_getmemoryusage";
}