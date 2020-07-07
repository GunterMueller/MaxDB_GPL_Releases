/*!
    \file    DBMSrvCmd_Explain.cpp
    \author  TiloH
    \ingroup DBMServer commands
    \brief   implementation of a class for the DBMServer command explain

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "hcn10.h"
#include "hcn36.h"
#include "hcn90.h"

#include "DBM/Srv/Commands/DBMSrvCmd_Explain.hpp"
#include "DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_Explain
//-----------------------------------------------------------------------------

DBMSrvCmd_Explain::DBMSrvCmd_Explain()
: DBMSrv_Command( DBMSrv_Command::KeyExplain, false )
{
    m_longHelp=
        "@command explain A function for explaining DBM commands."
        "@preconditions There are none."
        "@syntax explain <command> [<option> [-nolinewrap|-longinfo]]"
        "@param <command>   The DBMServer command that will be explained."
        "@param <option>    Restricts the output to the explanation of the specified command option."
        "@param -nolinewrap Suppresses line wraps normally inserted for better readability."
        "@param -longinfo   Suppresses line wraps normally inserted for better readability, "
                        "shows the descriptive names of the allowed values for option <option> and "
                        "separates allowed values, long names and explanation of the value by |."
        "@reply OK<NL>";
}

tcn00_Error DBMSrvCmd_Explain::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & reply)
{
    tcn00_Error rc=OK_CN00;

    bool CommandFound=false;
    bool ParameterFound=false;
    bool ExplainCompleteCommand=true;
    bool ShowLongNames=false;
    size_t LineSize=80;

    const char * Command=0;
    const char * CommandEnd=0;
    const char * Option=0;
    const char * OptionEnd=0;
    const char * DummyParameter=0;
    const char * DummyParameterEnd=0;

    const cn10DBMServerCommand * CommandInArray=DBMServerCommandsArray_cn10;

    reply.startWithOK();

    cn90GetStartAndEndOfToken(command->args, Command, CommandEnd, 1);

    //search the command in the array of command information
    while(0!=CommandInArray->name && !CommandFound)
    {
        if(Command!=CommandEnd && 0==cn36_StrStrNUprCmp(CommandInArray->name, Command, CommandEnd-Command)) //<command> parameter is set and matches namesC[nIndex].name
            CommandFound=true;
        else
            ++CommandInArray;
    }

    //if the command was found, explain it
    if(CommandFound)
    {
        DBMSrv_Command * pCommand=0;
        DBMSrvMsg_Error  message;

        rc=DBMSrvInfrCmd_CommandFactory::getCommand(
                CommandInArray->name,
                CommandInArray->cmdKey,
                pCommand,
                reply,
                message);

        if(0!=pCommand)
        {
            pCommand->getExplainText( command, reply );
            delete pCommand;

            rc=OK_CN00;
        }
        else
        {
            message.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOEXPLAIN, Msg_Arg("COMMAND_NAME", CommandInArray->name)));
            rc=reply.startWithMessageList(message);
        }
    }
    else
    {
        char   commandAsString[51];
        size_t len=(CommandEnd-Command>50?50:(CommandEnd-Command));

        SAPDB_strncpy(commandAsString, Command, len);
        commandAsString[len]='\0';

        reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_COMMAND, Msg_Arg("COMMAND_NAME", commandAsString)));
    }

    return rc;
}
