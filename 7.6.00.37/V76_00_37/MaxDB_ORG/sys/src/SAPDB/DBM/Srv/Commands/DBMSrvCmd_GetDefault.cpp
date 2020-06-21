/*!
    \file    DBMSrvCmd_GetDefault.cpp
    \author  TiloH
    \ingroup DBMServer commands
    \brief   implementation of a class for the DBMServer command getdefault

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#include "DBM/Srv/Commands/DBMSrvCmd_GetDefault.hpp"
#include "DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_GetDefault
//-----------------------------------------------------------------------------

DBMSrvCmd_GetDefault::DBMSrvCmd_GetDefault()
: DBMSrv_Command(DBMSrv_Command::KeyGetDefault, false, 0, true) //a non obsolete but hidden command
{
    m_longHelp=
        "@command get_default A function for determining the default value of a DBM command options."
        "@preconditions There are none."
        "@syntax get_default <command> <option>"
        "@param <command>   The DBM Server command that you are interested in."
        "@param <option>    The option of the DBM Server command <command>, whose default value you are asking for."
        "@reply OK<NL>"
                "<default_value><NL>"
                "@replyvalue <default_value> The default value of the option. <default_value> is empty, "
                                             "if the option has no default value.";
}

tcn00_Error DBMSrvCmd_GetDefault::run(
    VControlDataT * vcontrol,
    CommandT      * com,
    DBMSrv_Reply  & reply)
{
    tcn00_Error rc=OK_CN00;

    bool commandFound=false;

    const char * command=0;
    const char * commandEnd=0;
    const char * option=0;
    const char * optionEnd=0;

    const cn10DBMServerCommand * commandInArray=DBMServerCommandsArray_cn10;

    reply.startWithOK();

    cn90GetStartAndEndOfToken(com->args, command, commandEnd, 1);
    cn90GetStartAndEndOfToken(com->args, option, optionEnd, 2);

    //search the command in the array of command information
    while(0!=commandInArray->name && !commandFound)
    {
        if(0==cn36_StrStrNUprCmp(commandInArray->name, command, commandEnd-command)) //<command> parameter is set and matches namesC[nIndex].name
            commandFound=true;
        else
            ++commandInArray;
    }

    //if the command was found, explain it
    if(commandFound)
    {
        DBMSrv_Command * commandObject=0;
        DBMSrvMsg_Error  message;

        rc=DBMSrvInfrCmd_CommandFactory::getCommand(
                commandInArray->name,
                commandInArray->cmdKey,
                commandObject,
                reply,
                message);

        if(0!=commandObject)
        {
            const char * defaultStart=0; 
            const char * defaultEnd=0;

            bool parameterFound=commandObject->getDefaultForOption(option, optionEnd-option, defaultStart, defaultEnd);

            if(parameterFound)
            {
                if(0!=defaultStart && 0!=defaultEnd)
                    reply.appendLineN(defaultStart, defaultEnd-defaultStart);
                else
                    reply.appendLine();
            }
            else
            {
                char commandCopy[51];
                char optionCopy[51];

                cn36_StrNCpy(commandCopy, command, (commandEnd-command>50)?50:commandEnd-command);
                cn36_StrNCpy(optionCopy, option, (optionEnd-option>50)?50:optionEnd-option);

                rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_IS_NO_OPTION, Msg_Arg("DBMSRV_COMP1", commandCopy), Msg_Arg("DBMSRV_COMP2", optionCopy)));
            }

            delete commandObject;
        }
    }
    else
    {
        char commandAsString[51];
        size_t len(commandEnd-command>50?50:(commandEnd-command));

        strncpy(commandAsString, command, len);
        commandAsString[len]='\0';

        rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_COMMAND, Msg_Arg("COMMAND_NAME", commandAsString)));
    }

    return rc;
}
