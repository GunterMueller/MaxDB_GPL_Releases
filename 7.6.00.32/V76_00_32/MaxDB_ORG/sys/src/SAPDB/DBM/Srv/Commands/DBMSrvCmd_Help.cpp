/*!
    \file    DBMSrvCmd_Help.cpp
    \author  TiloH
    \ingroup DBMServer commands
    \brief   implementation of a class for the DBMServer command help

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

#include <limits.h>

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_Help.hpp"
#include "DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp"
#include "DBM/Srv/ExplainHelp/DBMSrvExpHlp_SyntaxPart.hpp"
#include "DBM/Srv/ExplainHelp/DBMSrvExpHlp_FuzzyStringDifference.hpp"
#include "hcn10.h"
#include "hcn36.h"
#include "hcn90.h"


//-----------------------------------------------------------------------------
// member of class DBMSrvCmd_Help
//-----------------------------------------------------------------------------

const char * DBMSrvCmd_Help::m_LongHelp=
    "@command help You request the list of all DBM commands. In addition, "
        "you receive a short syntax description for all commands."
    "@preconditions You do not need to log on to the Database Manager to execute this DBM command."
    "@syntax help [-obsolete] [-k] [<command>]"
    "@param -obsolete Specify the option -obsolete, to display exclusively the "
        "obsolete commands. The system displays a reference to the current name "
        "for each of these commands. If -obsolete is omitted, no obsolete DBM commands "
        "are displayed."
    "@param -k Use option -k, to do a fuzzy search for commands. If -k is set, all commands are displayed, that "
        "contain the string <command>, or something similar to that string."
    "@param <command> A parameter restricting the list of displayed commands. Unless option -k is specified, "
        "only those commands are displayed which start with the string <command>."
    "@reply OK<NL>"
        "<command_name> <description><NL>"
        "<command_name> <description><NL>"
        "..."
        "@replyvalue <command_name> The name of a DBM command."
        "@replyvalue <description> The syntax description of the command, if the option -obsolete is "
            "not used. Otherwise description contains a reference to the current name of the command. "
            "The description contains line breaks, if it is too long.";

DBMSrvCmd_Help::DBMSrvCmd_Help()
: DBMSrv_Command( DBMSrv_Command::KeyHelp, false, DBMSrvCmd_Help::m_LongHelp )
{
}

tcn00_Error DBMSrvCmd_Help::allocateCopyWithoutC(char * & copy, const char * string, size_t length, char c, DBMSrv_Reply  & Reply)
{
    tcn00_Error rc=OK_CN00;
    size_t i;
    size_t lengthWithoutC=0;

    //compute length without all occurrences of c
    for(i=0; i<length; i++)
    {
        if(string[i]!=c)
            lengthWithoutC++;
    }

    //allocate memory
    if(!cn36_StrAlloc(copy, lengthWithoutC))
        rc=Reply.startWithError(ERR_MEM_CN00);
    else
    {   //copy everything except c
        size_t j=0;

        for(i=0; i<length; i++)
        {
            if(string[i]!=c)
                copy[j++]=string[i];
        }

        copy[lengthWithoutC]='\0';
    }

    return rc;
}

tcn00_Error DBMSrvCmd_Help::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & reply)
{
    bool ShowObsolete=false;
    
    bool useInexactStringCompare=false;
    bool CommandFound=false;

    size_t LineSize=80;

    const char * Command=0;
    const char * CommandEnd=0;

    // analyze parameters
    int          i=1;
    const char * parameterStart=0;
    const char * parameterEnd=0;

    cn90GetStartAndEndOfToken(command->args, parameterStart, parameterEnd, i);

    while(0!=parameterStart && parameterStart!=parameterEnd)
    {
        if(0==cn36_StrStrNUprCmp("-OBSOLETE", parameterStart, parameterEnd-parameterStart)) // we have a parameter and it is a "-obsolete"
            ShowObsolete=true;
        else
        {
            if(0==cn36_StrStrNUprCmp("-K", parameterStart, parameterEnd-parameterStart)) // we have a parameter and it is a "-k"
                useInexactStringCompare=true;
            else                                                                         //every other parameter is a command name, start of a command name, or a command name particle
            {
                //ShowAllCommands=false;
                Command=parameterStart;
                CommandEnd=parameterEnd;
            }
        }

        cn90GetStartAndEndOfToken(command->args, parameterStart, parameterEnd, ++i);
    }

    return run(ShowObsolete, useInexactStringCompare, Command, CommandEnd-Command, reply);
}

tcn00_Error DBMSrvCmd_Help::run(
    bool            showObsolete,
    bool            searchSimilar,
    const char    * command,
    size_t          commandLength,
    DBMSrv_Reply  & reply)
{
    tcn00_Error rc=OK_CN00;

    bool ShowAllCommands=(commandLength==0)?true:false;

    char       * commandWithoutUnderline=0;
    size_t       lengthCommandWithoutUnderline=0;

    const cn10DBMServerCommand * CommandInArray=DBMServerCommandsArray_cn10;

    reply.startWithOK();

    if(ShowAllCommands)
        searchSimilar=false;

    if(searchSimilar)
    {
        rc=allocateCopyWithoutC(commandWithoutUnderline, command, commandLength, '_', reply);

        if(OK_CN00==rc)
            lengthCommandWithoutUnderline=strlen(commandWithoutUnderline);
    }

    //search the commands in the array of command information
    while(OK_CN00==rc && 0!=CommandInArray->name)
    {
        bool matchingInexact=false;

        if(searchSimilar)
        {
            char * commandInArrayWithoutUnderline=0;

            rc=allocateCopyWithoutC(commandInArrayWithoutUnderline, CommandInArray->name, strlen(CommandInArray->name), '_', reply);

            if(OK_CN00==rc)
            {
                size_t diff=0;

                if(!DBMSrvExpHlp_FuzzyStringDifference::calculateMinimalDifferenceToAllSubstrings(commandWithoutUnderline, commandInArrayWithoutUnderline, diff))
                    rc=reply.startWithError(ERR_MEM_CN00);
                else
                    matchingInexact=(diff<=((lengthCommandWithoutUnderline/5)*1)); //for every 5 characters of the particle two missing character are allowed
            }

            cn36_StrDealloc(commandInArrayWithoutUnderline);
        }

        if(OK_CN00==rc)
        {
            if(ShowAllCommands ||
               (!searchSimilar && 0==strnicmp(command, CommandInArray->name, commandLength)) || //the command matches by name
               matchingInexact)                                                                 // -k is set and command matches the given command name particle
            {
                if(CommandInArray->cmdKey == DBMSrv_Command::KeyUnknownCommand)
                {
                    // take syntax directly from array, old fashion
                    const char * Syntax=CommandInArray->shortHelp;

                    if(NULL!=Syntax)
                    {
                        size_t SyntaxLength=cn36_TerminatingZeroOf(Syntax)-Syntax;
                        if((showObsolete && strlen(OBSOLETE_MARK_CN10)<=SyntaxLength && 0==strnicmp(OBSOLETE_MARK_CN10, Syntax, strlen(OBSOLETE_MARK_CN10))) ||    //if asked for obsolete functions, show obsolete functions only
                            (!showObsolete && (strlen(OBSOLETE_MARK_CN10)>SyntaxLength || 0!=strnicmp(OBSOLETE_MARK_CN10, Syntax, strlen(OBSOLETE_MARK_CN10)))) ||  //if asked for non obsolete functions, show non obsolete functions
                            (!ShowAllCommands && 0==cn36_StrStrNUprCmp(CommandInArray->name, command, commandLength))                                        ) //but always show exact matches
                        {
                            if(0!=CommandInArray->shortHelp)
                            {
                                reply.appendStringWithMinWidth(CommandInArray->name, 25);
                                reply.appendLine(CommandInArray->shortHelp);
                            }
                        }
                    }
                }
                else
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
                        if((!pCommand->isHidden() && showObsolete == pCommand->isObsolete()) ||                       //either the command is not hidden and of the requested type of obsoleteness
                           (!ShowAllCommands && 0==cn36_StrStrNUprCmp(CommandInArray->name, command, commandLength))) //or we match the name exactly to the last character
                        {
                            pCommand->getSyntaxText(reply);
                        }

                        delete pCommand;
                    }
                }
            }
        }

        ++CommandInArray;
    }

    cn36_StrDealloc(commandWithoutUnderline);

    return rc;
}
