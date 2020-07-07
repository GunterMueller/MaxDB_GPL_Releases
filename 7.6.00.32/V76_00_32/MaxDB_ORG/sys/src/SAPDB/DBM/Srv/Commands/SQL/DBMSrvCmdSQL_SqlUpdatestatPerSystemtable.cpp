/*!
  \file    DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.cpp
  \author  marcW
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

#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

#include "hcn50.h"

DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::DBMSrvCmdSQL_SqlUpdatestatPerSystemtable()
    : DBMSrv_Command(
            DBMSrv_Command::KeyUsingCommandFactory,
            false,
            DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::m_LongHelp ) {
}

const char* const DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::m_LongHelp=
            "@command "DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_CMD" Update optimizer statistics "
                "for those tables, that need an update of their statistics"
            "@preconditions You have the DBM operator authorizations AccessSQL."
            "@syntax "DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_CMD;

const char* DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::getCommandName() {
    return DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_CMD;
}

const char* const DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::SQL_UPDATESTAT_SMART("UPDATE STAT AS PER SYSTEM TABLE");

tcn00_Error DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    tcn00_Error rc(OK_CN00);
    theReply.startWithOK();

    DBMSrvMsg_Error errOut;
    if( !updateSmart(vcontrol, errOut) )
        rc = theReply.startWithMessageList(errOut);

    return rc;
}

bool DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::updateSmart(
        VControlDataT* vcontrol,
        DBMSrvMsg_Error& errOut) {
    // build execute kernel command
    cn50DBMUser superUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    return DBMSrvKnl_Session::Execute(
            vcontrol->dbname.asCharp(),
            superUser.getUserName().asCharp(),
            superUser.getClearMasterPwd().asCharp(),
            SQL_UPDATESTAT_SMART,
            errOut);
}

bool DBMSrvCmdSQL_SqlUpdatestatPerSystemtable::updateSmartInBackground(
        VControlDataT* vcontrol,
        DBMSrvMsg_Error& errOut) {

    DBMSrvMsg_Error errors;
    char upwdBuffer[256];
    DBMSrvProc_ArgumentList theArgList;

    // find the dbm super user
    cn50DBMUser superUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));

    // build command line
    theArgList.AddArgument("-d");
    theArgList.AddArgument(vcontrol->dbname.asCharp());
    theArgList.AddArgument("-u");
    sprintf(upwdBuffer, "%s,%s", superUser.getUserName().asCharp(), superUser.getClearMasterPwd().asCharp());
    theArgList.AddArgument(upwdBuffer);
    theArgList.AddArgument(getCommandName());

    DBMSrvProc_ServerProcess updateStatNowInBackground(
        DBMSrvProc_ServerProcessCommandLineConverter::KeyDbmcli,
        theArgList);

    // we return errors only about problems of the start of the background process,
    // it logs errors that occur during its execution in DBMServer log by itself
    if( !updateStatNowInBackground.started() )
        updateStatNowInBackground.getMessagesInto(errors);
    
    errOut.Overrule(errors);
    return errors.IsEmpty();
}
