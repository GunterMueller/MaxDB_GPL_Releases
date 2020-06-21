/*!
  \file    DBMSrvCmd_Apropos.cpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   definition of a class for handling DBM Server
           command apropos

\if EMIT_LICENCE

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


\endif
*/

#include "hcn90.h"

#include "DBM/Srv/Commands/DBMSrvCmd_Apropos.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_Help.hpp"


DBMSrvCmd_Apropos::DBMSrvCmd_Apropos()
    :DBMSrv_Command(KeyApropos, false) //not obsolete
{
    m_longHelp=
        "@command apropos The command apropos lists all DBM Server commands that contain "
            "a certain character string or something similar to a certain character string "
            "in their name. Any underlines in command names are ignored by the apropos command."
        "@preconditions None."
        "@syntax apropos <command_name_part>"
        "@param <command_name_part> A string used to restrict the listed DBM Server commands."
        "@reply OK<NL>"
            "<command_name> <description><NL>"
            "<command_name> <description><NL>"
            "..."
            "@replyvalue <command_name> The name of a DBM command."
            "@replyvalue <description> The syntax description of the command. "
                "The description contains line breaks, if it is too long.";
}

tcn00_Error DBMSrvCmd_Apropos::run(
        VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply  & reply)
{
    int          i=1;
    const char * parameterStart=0;
    const char * parameterEnd=0;

    cn90GetStartAndEndOfToken(command->args, parameterStart, parameterEnd, 1); //get the first parameter

    if(0!=parameterStart && parameterStart!=parameterEnd) //if it is not empty, use help command with option -k
    {
        DBMSrvCmd_Help helpCommand;

        return helpCommand.run(false, true, parameterStart, parameterEnd-parameterStart, reply); //no obsolete commands, inexact search
    }
    else
    {
        reply.startWithOK();
        return OK_CN00;
    }
}
