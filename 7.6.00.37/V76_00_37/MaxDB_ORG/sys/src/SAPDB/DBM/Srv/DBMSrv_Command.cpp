/*!
    \file    DBMSrvCmd_Command.cpp
    \author  MarcW
    \ingroup common classes for the DBMServer
    \brief   definition of base class for DBMServer commands

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

#include <limits.h>

#include "hcn10.h"
#include "hcn20.h"
#include "hcn36.h"
#include "hcn90.h"

#include "DBM/Srv/DBMSrv_Command.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_CommandPart.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_DefaultValuePart.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ParameterPart.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_PreconditionsPart.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ReplyPart.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ReplyValuePart.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_SyntaxPart.hpp"
#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ValuePart.hpp"

#include "DBM/Srv/Commands/Auto/DBMSrvCmdAuto_Headers.hpp"

#include "DBM/Srv/Commands/BackupHistory/DBMSrvCmdBHist_BackupHistoryAppend.hpp"
#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAddVolume.hpp"
#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmExecuteServerprocess.hpp"
#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetpid.hpp"
#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmListMemoryerror.hpp"
#include "DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmReset.hpp"
#include "DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmInfo.hpp"
#include "DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmUnlock.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_Apropos.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_BackupExtIds.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_BackupHistory.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_DbmVersion.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_DbSpeed.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_DbState.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_Explain.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_GetDefault.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_Help.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_HssCopyFile.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_InstInfo.hpp"
#include "DBM/Srv/Commands/Event/DBMSrvCmdEvent_Header.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_Header.hpp"
#include "DBM/Srv/Commands/Trace/DBMSrvCmdTrc_Header.hpp"

const char * const DBMSrv_Command::defaultSentencePart1="The default value for this option is ";
const char * const DBMSrv_Command::defaultSentencePart2=".";
const char * const DBMSrv_Command::allowedValuesSentencePart1="Permitted values for option ";
const char * const DBMSrv_Command::allowedValuesSentencePart2=" are:";


DBMSrv_Command* DBMSrv_Command::getCommand(
    CommandKey     aCommandKey,
    const char   * commandName,
    DBMSrv_Reply & reply)
{
    DBMSrv_Command * rc=0;
    bool             keyFound=true;

    switch( aCommandKey ) {
        case KeyApropos:                  rc=new DBMSrvCmd_Apropos(); break;
        case KeyAutoExtend:               rc=new DBMSrvCmdAuto_AutoExtend(); break;
        case KeyAutoUpdateStatistics:     rc=new DBMSrvCmdAuto_AutoUpdateStatistics(); break;
        case KeyBackupExtIdsGet:          rc=new DBMSrvCmd_BackupExtIdsGet(); break;
        case KeyBackupExtIdsGetFromTool:  rc=new DBMSrvCmd_BackupExtIdsGetFromTool(); break;
        case KeyBackupHistoryAppend:      rc=new DBMSrvCmdBHist_BackupHistoryAppend(); break;
        case KeyBackupHistoryClose:       rc=new DBMSrvCmd_BackupHistoryClose(); break;
        case KeyBackupHistoryDate:        rc=new DBMSrvCmd_BackupHistoryDate(); break;
        case KeyBackupHistoryList:        rc=new DBMSrvCmd_BackupHistoryList(); break; 
        case KeyBackupHistoryListNext:    rc=new DBMSrvCmd_BackupHistoryListNext(); break;
        case KeyBackupHistoryOpen:        rc=new DBMSrvCmd_BackupHistoryOpen(); break;
        case KeyDbmExecuteServerprocess:  rc=new DBMSrvCmdDbm_DbmExecuteServerprocess(); break;
        case KeyDbmGetpid:                rc=new DBMSrvCmdDbm_DbmGetpid(); break;
        case KeyDbmListMemoryerror:       rc=new DBMSrvCmdDbm_DbmListMemoryerror(); break;
        case KeyDbmShmInfo:               rc=new DBMSrvCmdDbmShm_DbmShmInfo(); break;
        case KeyDbmShmReset:              rc=new DBMSrvCmdDbmShm_DbmShmReset(); break;
        case KeyDbmShmUnlock:             rc=new DBMSrvCmdDbmShm_DbmShmUnlock(); break;
        case KeyDbmVersion:               rc=new DBMSrvCmd_DbmVersion(); break;
        case KeyDbSpeed:                  rc=new DBMSrvCmd_DbSpeed(); break;
        case KeyDbState:                  rc=new DBMSrvCmd_DbState(); break;
        case KeyExplain:                  rc=new DBMSrvCmd_Explain(); break;
        case KeyEventAvailable:           rc=new DBMSrvCmdEvent_EventAvailable(); break;
        case KeyEventDelete:              rc=new DBMSrvCmdEvent_EventDelete(); break;
        case KeyEventList:                rc=new DBMSrvCmdEvent_EventList(); break;
        case KeyEventListCategories:      rc=new DBMSrvCmdEvent_EventListCategories(); break;
        case KeyEventReceive:             rc=new DBMSrvCmdEvent_EventReceive(); break;
        case KeyEventRelease:             rc=new DBMSrvCmdEvent_EventRelease(); break;
        case KeyEventSet:                 rc=new DBMSrvCmdEvent_EventSet(); break;
        case KeyEventWait:                rc=new DBMSrvCmdEvent_EventWait(); break;
        case KeyGetDefault:               rc=new DBMSrvCmd_GetDefault(); break;
        case KeyHelp:                     rc=new DBMSrvCmd_Help(); break;
        case KeyHssCopyFile:              rc=new DBMSrvCmd_HssCopyFile(); break;
        case KeyInstInfo:                 rc=new DBMSrvCmd_InstInfo(); break;
        case KeyMediumDelete:             rc=new DBMSrvCmdMedium_MediumDelete(); break;
        case KeyMediumDeleteNoDistribute: rc=new DBMSrvCmdMedium_MediumDeleteNoDistribute(); break;
        case KeyMediumPut:                rc=new DBMSrvCmdMedium_MediumPut(); break;
        case KeyMediumPutNoDistribute:    rc=new DBMSrvCmdMedium_MediumPutNoDistribute(); break;
        case KeySchedulerActivateJob:     rc=new DBMSrvCmdScd_SchedulerActivateJob(); break;
        case KeySchedulerDeactivateJob:   rc=new DBMSrvCmdScd_SchedulerDeactivateJob(); break;
        case KeySchedulerDeleteJob:       rc=new DBMSrvCmdScd_SchedulerDeleteJob(); break;
        case KeySchedulerState:           rc=new DBMSrvCmdScd_SchedulerState(); break;
        case KeySchedulerListJobs:        rc=new DBMSrvCmdScd_SchedulerListJobs(); break;
        case KeySchedulerCreateJob:       rc=new DBMSrvCmdScd_SchedulerCreateJob(); break;
        case KeySchedulerStart:           rc=new DBMSrvCmdScd_SchedulerStart(); break;
        case KeySchedulerStop:            rc=new DBMSrvCmdScd_SchedulerStop(); break;
        case KeySchedulerShowJoblog:      rc=new DBMSrvCmdScd_SchedulerShowJoblog(); break;
        case KeySchedulerDeleteJoblog:    rc=new DBMSrvCmdScd_SchedulerDeleteJoblog(); break;
        case KeyTraceClear:               rc=new DBMSrvCmdTrc_TraceClear(); break;
        case KeyTraceFlush:               rc=new DBMSrvCmdTrc_TraceFlush(); break;
        case KeyTraceOff:                 rc=new DBMSrvCmdTrc_TraceOff(); break;
        case KeyTraceOn:                  rc=new DBMSrvCmdTrc_TraceOn(); break;
        case KeyTraceProt:                rc=new DBMSrvCmdTrc_TraceProt(); break;
        case KeyTraceProtOpt:             rc=new DBMSrvCmdTrc_TraceProtOpt(); break;
        case KeyTraceShow:                rc=new DBMSrvCmdTrc_TraceShow(); break;

        /* stop inserting new commands here, command keys are dead, use the command factory class
           DBMSrvCmdInfr_CommandFactory instead, start deleting case statements from above */

        default:                          keyFound=false; break;
    }

    if(0==rc)
    {
        if(keyFound)                                //key found, but new returned 0
            reply.startWithError(ERR_MEM_CN00);
        else                                        //known key not found in the switch statement
            reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOTIMPL, Msg_Arg("COMMAND_NAME", commandName)));
    }

    return rc;
}

// public member function DBMSrv_Command::getRundirectory
const char* DBMSrv_Command::getRundirectory(bool ReadAgain) {
    if( m_RundirectoryNotCalculated || ReadAgain) {
      m_RundirectoryNotCalculated = !getParameter(PAN_RUNDIR, m_Rundirectory);
    } // end if
    return m_Rundirectory;
} // end DBMSrv_Command::getRundirectory

// public member function DBMSrv_Command::getUnicode
const char* DBMSrv_Command::getUnicode(bool ReadAgain) {
    if( m_UnicodeNotCalculated || ReadAgain) {
      m_UnicodeNotCalculated = !getParameter(PAN_UNICODE_CATALOG, m_Unicode);
    } // end if
    return m_Unicode;
} // end DBMSrv_Command::getUnicode

// public member function DBMSrv_Command::InstanceType
const char* DBMSrv_Command::getInstanceType(bool ReadAgain) {
    if( m_InstanceTypeNotCalculated || ReadAgain) {
      m_InstanceTypeNotCalculated = !getParameter(PAN_INSTANCE_TYPE, m_InstanceType);
    } // end if
    return m_InstanceType;
} // end DBMSrv_Command::getInstanceType

// private member function DBMSrv_Command::getParameter
bool DBMSrv_Command::getParameter
    ( const char           * szName,
      tcn002_XpValueString & aValue )
{
  DBMSrvMsg_Error oParamGetMsg;
  if( strlen((const char*)cn00DBMServerData::vcontrol()->dbname) > 0 ) {
    return (cn20XParamGetValue(cn00DBMServerData::vcontrol()->dbname, szName, aValue, oParamGetMsg));
  } // end if

  return false;
} // end DBMSrv_Command::getUnicode

void DBMSrv_Command::formatExplainText(CommandT* command, DBMSrv_Reply& theReply, const char* longHelp) {
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

    theReply.startWithOK();

    // analyze parameters
    cn90GetStartAndEndOfToken(command->args, Command, CommandEnd, 1);
    cn90GetStartAndEndOfToken(command->args, Option, OptionEnd, 2);
    cn90GetStartAndEndOfToken(command->args, DummyParameter, DummyParameterEnd, 3);

    char cmdName[80];
    size_t len(CommandEnd-Command>79?79:CommandEnd-Command);
    strncpy(cmdName, Command, len);
    cmdName[len] = '\0';

    if(DummyParameter!=DummyParameterEnd)
    {
        if(0==cn36_StrStrNUprCmp("-NOLINEWRAP", DummyParameter, DummyParameterEnd-DummyParameter)) // we have a third parameter and it is a "-nolinewrap"
            LineSize=INT_MAX;

        if(0==cn36_StrStrNUprCmp("-LONGINFO", DummyParameter, DummyParameterEnd-DummyParameter))
        {
            LineSize=INT_MAX;
            ShowLongNames=true;
        }     
    }

    if(Option!=OptionEnd)
    {
        if(Option[0]=='"' && 2<=OptionEnd-Option && '"'==*(OptionEnd-1)) {
            // if option is in "..."
            Option++;
            OptionEnd--;
        }
        ExplainCompleteCommand=false;
    }

    if(longHelp == NULL)
        theReply.appendStringsAndLine("No explanation available for command ", cmdName, ".");
    else
    {
        DBMSrvExpHlp_CommandPart       CommandInfo(longHelp);       //search for first "@command" part
        DBMSrvExpHlp_PreconditionsPart PreconditionsInfo(longHelp); //search for the first "@preconditions" part
        DBMSrvExpHlp_SyntaxPart        SyntaxInfo(longHelp);        //search for first "@syntax" part
        DBMSrvExpHlp_ParameterPart     ParameterInfo(longHelp);     //search for first "@param" part
        DBMSrvExpHlp_ReplyPart         ReplyPart(longHelp);         //search for first "@reply" part

        if(CommandInfo.wasFound())
        {
            if(ExplainCompleteCommand) // the complete explanation of a command was requested
            {
                //print command info
                if(CommandInfo.hasExplanation())
                    theReply.appendIndentedWrappedLineN(CommandInfo.giveExplanationStart(), CommandInfo.giveExplanationLength(), 0, LineSize, "<NL>", 0);
                else
                    theReply.appendStringsAndLine("No explanation available for command ", cmdName, ".");

                //print preconditions
                if(PreconditionsInfo.wasFound())
                {
                    theReply.appendLines("", "Preconditions:");
                    theReply.appendIndentedWrappedLineN(PreconditionsInfo.giveExplanationStart(), PreconditionsInfo.giveExplanationLength(), 4, LineSize, 0);
                }

                //print syntax
                theReply.appendLines("", "Syntax:");
                theReply.appendStrings("    ", cmdName);

                if(SyntaxInfo.hasExplanation())
                {
                    size_t LengthBeforeWrappedPart=4+strlen(cmdName)+1;

                    theReply.appendString(" ");
                    theReply.appendIndentedWrappedLineN(SyntaxInfo.giveExplanationStart(), SyntaxInfo.giveExplanationLength(), LengthBeforeWrappedPart, LineSize, LengthBeforeWrappedPart);
                }
                else //if the command has no syntax info we assume it has no parameters
                    theReply.appendLine();
            }

            //explain parameters
            if(ParameterInfo.wasFound())
            {
                bool   IsFirstOption=true;
                size_t MaxParameterLength=ParameterInfo.LongestIDOfSameType();

                if(ExplainCompleteCommand)
                    theReply.appendLines("", "Options:");

                while(ParameterInfo.wasFound() && !ParameterFound)
                {
                    //explain the parameter
                    if(ExplainCompleteCommand)
                    {
                        if(IsFirstOption)
                            IsFirstOption=false;
                        else
                            theReply.appendLine();

                        theReply.appendStringAndLineN("    ", ParameterInfo.giveIDStart(), ParameterInfo.giveIDLength());
                    }
                    else
                    {
                        if(parameterHasName(ParameterInfo.giveIDStart(), ParameterInfo.giveIDLength(), Option, OptionEnd-Option))
                            ParameterFound=true;
                    }

                    if(ExplainCompleteCommand || ParameterFound)
                    {
                        size_t                 ParameterIndent=ParameterFound?0:8;
                        DBMSrvExpHlp_ValuePart ValueInfo(ParameterInfo.giveExplanationEnd());
                        size_t                 postionInOutputLine=0;

                        theReply.appendIndentedWrappedStringN(
                            ParameterInfo.giveExplanationStart(),
                            ParameterInfo.giveExplanationLength(),
                            ParameterIndent,
                            LineSize,
                            "<NL>",
                            postionInOutputLine,
                            true);

                        //default handling
                        DBMSrvExpHlp_DefaultValuePart defaultValue(ParameterInfo.giveExplanationEnd());

                        if(defaultValue.wasFound())
                        {
                            if(OK_CN00!=appendWrappedText(
                                            theReply,
                                            defaultSentencePart1,                   //"The default value for this option is "
                                            defaultValue.giveExplanationStart(),    //<default>
                                            defaultValue.giveExplanationLength(),   //"."
                                            defaultSentencePart2,
                                            ParameterIndent,
                                            LineSize,
                                            postionInOutputLine))
                            {
                                return;
                            }
                        }

                        //permitted values handling
                        if(ValueInfo.wasFound() &&                                                              //parameter has special values and
                           (0>=ParameterInfo.giveExplanationLength() ||                                         //parameter has either no explanantion or
                           ':'!=ParameterInfo.giveExplanationStart()[ParameterInfo.giveExplanationLength()-1])) //explanation ends not already with a ':'
                        {
                            if(OK_CN00!=appendWrappedText(
                                            theReply,
                                            allowedValuesSentencePart1,     //"Permitted values for option "
                                            ParameterInfo.giveIDStart(),    //<option>
                                            ParameterInfo.giveIDLength(),
                                            allowedValuesSentencePart2,     //" are:"
                                            ParameterIndent,
                                            LineSize,
                                            postionInOutputLine))
                            {
                                return;
                            }
                        }

                        theReply.appendLine();

                        //explain special values for currently explained parameter
                        if(ValueInfo.wasFound())
                        {
                            size_t MaxValueLength=ValueInfo.LongestIDOfSameType();
                            size_t MaxLongNameLength=ValueInfo.LongestLongNameOfSameType();
                            size_t Ident=ShowLongNames?0:ParameterIndent+4;

                            while(ValueInfo.wasFound())
                            {
                                theReply.appendCharacterRepeatedly(' ', Ident);
                                theReply.appendStringNWithMinWidth(
                                    ValueInfo.giveIDStart(),
                                    ValueInfo.giveIDLength(),
                                    ShowLongNames?MaxValueLength:MaxValueLength+1);

                                if(ShowLongNames)
                                {
                                    theReply.appendString("|");
                                    theReply.appendStringNWithMinWidth(ValueInfo.giveLongNameStart(), ValueInfo.giveLongNameLength(), MaxLongNameLength);
                                    theReply.appendString("|");
                                }

                                theReply.appendIndentedWrappedLineN(ValueInfo.giveExplanationStart(), ValueInfo.giveExplanationLength(), Ident+MaxValueLength+1, LineSize, Ident+MaxValueLength+1);

                                ValueInfo.setToNextParameter();
                            }
                        }
                    }

                    //switch to the next parameter
                    ParameterInfo.setToNextParameter();
                }

            }

            if(!ExplainCompleteCommand && !ParameterFound) //one parameter should be explained, but no explanation was found in longHelp
                theReply.appendStringsStringNAndLine("Command ", cmdName, " has no option ", Option, OptionEnd-Option, ".");

            //explain reply format
            if(ExplainCompleteCommand && ReplyPart.wasFound() && ReplyPart.hasExplanation())
            {
                theReply.appendLines("", "Reply:");
                theReply.appendIndentedWrappedLineN(ReplyPart.giveExplanationStart(), ReplyPart.giveExplanationLength(), 4, LineSize, "<NL>", 0);

                DBMSrvExpHlp_ReplyValuePart ReplyValue(ReplyPart.giveExplanationEnd());

                if(ReplyValue.wasFound())
                {
                    theReply.appendLine();
                    theReply.appendStringAndLine("    ", "Meaning of the individual fields of the reply:");

                    size_t MaxValueLength=ReplyValue.LongestIDOfSameType();
                    bool IsFirstReplyValue=true;

                    while(ReplyValue.wasFound())
                    {
                        if(IsFirstReplyValue)
                            IsFirstReplyValue=false;
                        else
                            theReply.appendLine();

                        theReply.appendStringAndLineN("        ", ReplyValue.giveIDStart(), ReplyValue.giveIDLength());
                        theReply.appendIndentedWrappedLineN(ReplyValue.giveExplanationStart(), ReplyValue.giveExplanationLength(), 12, LineSize, 0);

                        ReplyValue.setToNextParameter();
                    }
                }
            }
        }
        else
            theReply.appendLine(longHelp); //there is a longHelp, but it does not have even a "@command" -> simply print the longHelp string
    }
}

void DBMSrv_Command::formatSyntaxText( DBMSrv_Reply& Reply, const char* longHelp ) {

    size_t LineSize(80);
    size_t IndentSize(25);

    if( longHelp != NULL ) {
        DBMSrvExpHlp_SyntaxPart SyntaxInfo(longHelp);
        if(SyntaxInfo.wasFound())
        {
            size_t LengthBeforeWrappedPart(SyntaxInfo.giveIDLength());

            if(LengthBeforeWrappedPart<IndentSize)
                LengthBeforeWrappedPart=IndentSize;

            Reply.appendStringNWithMinWidth(SyntaxInfo.giveIDStart(), SyntaxInfo.giveIDLength(), IndentSize-1);
            Reply.appendString(" ");
            Reply.appendIndentedWrappedLineN(SyntaxInfo.giveExplanationStart(), SyntaxInfo.giveExplanationLength(), LengthBeforeWrappedPart, LineSize, LengthBeforeWrappedPart);
        }
    }
}

bool DBMSrv_Command::getExplainText(CommandT* command, DBMSrv_Reply& theReply)
{
    if(0!=m_longHelp)
        formatExplainText(command, theReply, m_longHelp);

    return (0!=m_longHelp);
}

bool DBMSrv_Command::getSyntaxText(DBMSrv_Reply& theReply)
{
    if(0!=m_longHelp)
        formatSyntaxText(theReply, m_longHelp);

    return (0!=m_longHelp);
}

bool DBMSrv_Command::getUsageText(DBMSrv_Reply& theReply) {
    theReply.appendLine("usage:");
    return getSyntaxText( theReply );
}

bool DBMSrv_Command::hasOption(const char *option, size_t optionLength)
{
    DBMSrvExpHlp_ParameterPart parameterInfo(m_longHelp);     //search for first "@param" part in own longhelp member

    while(parameterInfo.wasFound())
    {
        if(parameterHasName(parameterInfo.giveIDStart() ,parameterInfo.giveIDLength(), option, optionLength))
            return true;
        else //switch to the next parameter
            parameterInfo.setToNextParameter();
    }

    return false;
}

bool DBMSrv_Command::getDefaultForOption(
    const char  * option,
    size_t        optionLength,
    const char *& defaultStart,
    const char *& defaultEnd)
{
    bool foundParameter=false;

    DBMSrvExpHlp_ParameterPart parameterInfo(m_longHelp);

    while(parameterInfo.wasFound() && !foundParameter)
    {
        if(parameterHasName(parameterInfo.giveIDStart() ,parameterInfo.giveIDLength(), option, optionLength))
            foundParameter=true;
        else //switch to the next parameter
            parameterInfo.setToNextParameter();
    }

    if(foundParameter)
    {
        DBMSrvExpHlp_DefaultValuePart defaultValue(parameterInfo.giveExplanationEnd());
        
        if(defaultValue.wasFound())
        {
            defaultStart=defaultValue.giveExplanationStart();
            defaultEnd  =defaultValue.giveExplanationEnd();
        }
        else
            defaultStart=defaultEnd=0;

        return true;
    }
    else
        return false;
}

bool DBMSrv_Command::parameterHasName(const char * parameterName, size_t parameterNameLength, const char * name, size_t nameLength)
{
    return((parameterNameLength==nameLength &&                         //name equals parameter's ID
            0==strnicmp(name, parameterName, parameterNameLength)) ||  //or
           (parameterNameLength==nameLength+2 &&                       //<name> equals parameter's ID
            parameterName[0]=='<' &&
            parameterName[parameterNameLength-1]=='>' &&
            0==strnicmp(name, parameterName+1, parameterNameLength-2)));
}

tcn00_Error DBMSrv_Command::appendWrappedText(
    DBMSrv_Reply   reply,
    const char   * string1,
    const char   * string2,
    size_t         lengthString2,
    const char   * string3,
    size_t         indent,
    size_t         lineSize,
    size_t       & postionInOutputLine)
{
    char * s=0;
    
    if(cn36_StrAlloc(s, strlen(string1)+lengthString2+strlen(string2)))
    {
        SAPDB_strcpy(s, string1);
        SAPDB_strncpy(s+strlen(string1), string2, lengthString2);
        SAPDB_strcpy(s+strlen(string1)+lengthString2, string3);

        reply.appendIndentedWrappedStringN(      //"The default value for this option is "
            s,
            strlen(s),
            indent,
            lineSize,
            0,
            postionInOutputLine,
            false);

        cn36_StrDealloc(s);

        return OK_CN00;
    }
    else
        return reply.startWithError(ERR_MEM_CN00);  
}

