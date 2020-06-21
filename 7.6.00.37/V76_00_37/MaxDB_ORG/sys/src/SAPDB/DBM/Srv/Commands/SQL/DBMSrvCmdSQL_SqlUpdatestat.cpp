/*!
  \file    DBMSrvCmdSQL_SqlUpdatestat.cpp
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

#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestat.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

#include "hcn50.h"

// command name
#define DBMSrvCmdSQL_SqlUpdatestat_CMD "sql_updatestat"

DBMSrvCmdSQL_SqlUpdatestat::DBMSrvCmdSQL_SqlUpdatestat()
    : DBMSrv_Command(
            DBMSrv_Command::KeyUsingCommandFactory,
            false,
            DBMSrvCmdSQL_SqlUpdatestat::m_LongHelp ) {
}

const char* const DBMSrvCmdSQL_SqlUpdatestat::m_LongHelp=
            "@command "DBMSrvCmdSQL_SqlUpdatestat_CMD" update of optimizer statistics"
            "@syntax "DBMSrvCmdSQL_SqlUpdatestat_CMD" [<table or column spec>]";

const char* DBMSrvCmdSQL_SqlUpdatestat::getCommandName() {
    return DBMSrvCmdSQL_SqlUpdatestat_CMD;
}

const char* const DBMSrvCmdSQL_SqlUpdatestat::SQL_UPDATESTAT("UPDATE STAT ");
const char* const DBMSrvCmdSQL_SqlUpdatestat::SQL_UPDATESTAT_ALL("UPDATE STAT *");

tcn00_Error DBMSrvCmdSQL_SqlUpdatestat::run(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply) {

    tcn00_Error rc(OK_CN00);
    theReply.startWithOK();

    Tools_DynamicUTF8String commandString;

    // update statistics
    if( command->oArguments.getCount() == 0 )
        commandString = SQL_UPDATESTAT_ALL;
    else {
        commandString = SQL_UPDATESTAT;
        commandString += command->args;
    }

    // build execute kernel command
    cn50DBMUser superUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    DBMSrvMsg_Error errOut;
    if( !DBMSrvKnl_Session::Execute(
            vcontrol->dbname.asCharp(),
            superUser.getUserName().asCharp(),
            superUser.getClearMasterPwd().asCharp(),
            commandString.CharPtr(),
            errOut) )
        rc = theReply.startWithMessageList(errOut);

    return rc;
}
