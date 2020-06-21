/*!
    @file           DBMCliEvtDisp_Main.cpp
    @author         MarcW
    @brief          Event Dispatcher  - main function

    ========== licence begin  GPL
    Copyright (c) 2003-2006 SAP AG

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

#include <stdio.h>
#include <signal.h>
#if !defined (WIN32)
extern "C" {
  #include <ucontext.h>
} 
#endif
#include "heo670.h"
#include "hni34.h"
#include "hsp100.h"

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Headers.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Main.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp"
#include "DBM/Logging/DBMLog_LogFile.hpp"
#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/VersionInfo/DBMVI_Strings.hpp"
#include "RunTime/Communication/RTEComm_URIBuilder.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"
#include "ToolsCommon/Parsers/Old/ToolsParsersOld_ProgramParameterParser.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"

static int printVersionInfo( FILE* outFile ) {
    return fprintf(
        outFile,
        "%s Event Dispatcher, Version %s\n%s\n\n",
        DBMVI_Strings::ProductName,
        DBMVI_Strings::FormatedBuildVersion,
        DBMVI_Strings::CopyRightString);
}

// migrate configuration if necessary
static bool migrateConfiguration( ToolsParsersOldSyntEl_Variable& cfgFile, Msg_List& outList ) {
    if( cfgFile.isSet() ) {
        tni34_AVirtualFile aConfigFile(
                cfgFile.getValue(),
                tni34_AVirtualFile::FromDisk,
                tni34_AVirtualFile::DoNotOpenNow);
        if( aConfigFile.IsGood() &&
            aConfigFile.Exists() &&
            !DBMCliEvtDisp_Properties::migrateFormat(outList, cfgFile.getValue())) {
            return false;
        }
    }
    return true;
}

/*!
    @brief main function

    This program is can either be started from the command line manually or the DBMServer
    starts it if DBM parameter RunEventDispatcher is set to "yes". DB connect data
    must be on the command line (with -d, -u, -n). Reads arguments from command line
    and initializes the event dispatcher class.

    This method contains a FinallyBlock, that is called before leaving the method.
*/

int main (int argcIn, char* argvIn[])
{
    int        argc            = 0; 
    char **    argv            = NULL;

    int nRC = Tools_PipeCall::ReadArguments(
            argcIn,
            argvIn,
            argc,
            argv,
            Tools_PipeCall::ReadXUser | Tools_PipeCall::PromptForUserOnRequest);

    if( nRC != 0 ) {
        fprintf(stderr, "%s: could not read arguments from commandline. reason: %s\n",
            argvIn[0],
            Tools_PipeCall::ErrorReason());
        fflush(stderr);
        exit(1);
    }

    int returnValue(0);
    Msg_List mainMsgList;
    DBMLog_LogFile* panicFile(NULL);
    DBMLog_LogFile* normalFile(NULL);
    Tools_DynamicUTF8String nodeNameString;

    // all commands
    ToolsParsersOldSyntEl_Keyword listCommand("list");
    ToolsParsersOldSyntEl_Keyword startCommand("start");
    ToolsParsersOldSyntEl_Keyword stopCommand("stop");
    ToolsParsersOldSyntEl_Keyword stateCommand("state");
    ToolsParsersOldSyntEl_Keyword deleteCommand("delete");
    ToolsParsersOldSyntEl_Keyword addCommand("add");
    ToolsParsersOldSyntEl_Keyword versionCommand("version");

    // internal option
    ToolsParsersOldSyntEl_Keyword internal("-internal");
    ToolsParsersOldSyntEl_Option internalOption(&internal);
    // remote access
    ToolsParsersOldSyntEl_Keyword remoteAccess("-remoteaccess");
    ToolsParsersOldSyntEl_Option remoteAccessOption(&remoteAccess);

    // configuration file
    ToolsParsersOldSyntEl_Variable cfgFile("cfgFile");

    // log file
    ToolsParsersOldSyntEl_Keyword logSwitch("-l");
    ToolsParsersOldSyntEl_Variable logFile("logFile");
    ToolsParsersOldSyntEl_Concatenation log;
    log.push_back(&logSwitch);
    log.push_back(&logFile);

    // event condition
    ToolsParsersOldSyntEl_Keyword nameKeyWord(DBMCliEvtCm_Event::AttribName);
    ToolsParsersOldSyntEl_Keyword nameEQ("==");
    ToolsParsersOldSyntEl_Variable nameValue("value");
    ToolsParsersOldSyntEl_Concatenation nameExpr;
    nameExpr.push_back(&nameKeyWord);
    nameExpr.push_back(&nameEQ);
    nameExpr.push_back(&nameValue);

    ToolsParsersOldSyntEl_Keyword priorityKeyWord(DBMCliEvtCm_Event::AttribPriority);
    ToolsParsersOldSyntEl_Keyword priorityEQ("==");
    ToolsParsersOldSyntEl_Keyword priorityLow(DBMSrvCmd_EventAdministrationConstants::m_TextLow, true);
    ToolsParsersOldSyntEl_Keyword priorityMedium(DBMSrvCmd_EventAdministrationConstants::m_TextMedium, true);
    ToolsParsersOldSyntEl_Keyword priorityHigh(DBMSrvCmd_EventAdministrationConstants::m_TextHigh, true);
    ToolsParsersOldSyntEl_AlternativeList priorityValue;
    priorityValue.push_back(&priorityLow);
    priorityValue.push_back(&priorityMedium);
    priorityValue.push_back(&priorityHigh);
    ToolsParsersOldSyntEl_Concatenation PriorityExpr;
    PriorityExpr.push_back(&priorityKeyWord);
    PriorityExpr.push_back(&priorityEQ);
    PriorityExpr.push_back(&priorityValue);
    ToolsParsersOldSyntEl_Option priorityOption(&PriorityExpr);

    ToolsParsersOldSyntEl_Keyword value1KeyWord(DBMCliEvtCm_Event::AttribValue1);
    ToolsParsersOldSyntEl_Keyword value1EQ("==");
    ToolsParsersOldSyntEl_Keyword value1GE(">=");
    ToolsParsersOldSyntEl_Keyword value1SE("<=");
    ToolsParsersOldSyntEl_Keyword value1GT(">");
    ToolsParsersOldSyntEl_Keyword value1ST("<");
    ToolsParsersOldSyntEl_AlternativeList value1Relation;
    value1Relation.push_back(&value1EQ);
    value1Relation.push_back(&value1GE);
    value1Relation.push_back(&value1SE);
    value1Relation.push_back(&value1GT);
    value1Relation.push_back(&value1ST);
    ToolsParsersOldSyntEl_Variable value1Value("value");
    ToolsParsersOldSyntEl_Concatenation value1Expr;
    value1Expr.push_back(&value1KeyWord);
    value1Expr.push_back(&value1Relation);
    value1Expr.push_back(&value1Value);
    ToolsParsersOldSyntEl_Option value1Option(&value1Expr);

    ToolsParsersOldSyntEl_Keyword value2KeyWord(DBMCliEvtCm_Event::AttribValue2);
    ToolsParsersOldSyntEl_Keyword value2EQ("==");
    ToolsParsersOldSyntEl_Keyword value2GE(">=");
    ToolsParsersOldSyntEl_Keyword value2SE("<=");
    ToolsParsersOldSyntEl_Keyword value2GT(">");
    ToolsParsersOldSyntEl_Keyword value2ST("<");
    ToolsParsersOldSyntEl_AlternativeList value2Relation;
    value2Relation.push_back(&value2EQ);
    value2Relation.push_back(&value2GE);
    value2Relation.push_back(&value2SE);
    value2Relation.push_back(&value2GT);
    value2Relation.push_back(&value2ST);
    ToolsParsersOldSyntEl_Variable value2Value("value");
    ToolsParsersOldSyntEl_Concatenation value2Expr;
    value2Expr.push_back(&value2KeyWord);
    value2Expr.push_back(&value2Relation);
    value2Expr.push_back(&value2Value);
    ToolsParsersOldSyntEl_Option value2Option(&value2Expr);

    ToolsParsersOldSyntEl_Set eventCondition;
    eventCondition.push_back(&nameExpr);
    eventCondition.push_back(&priorityOption);
    eventCondition.push_back(&value1Option);
    eventCondition.push_back(&value2Option);

    // finally, the command that is to be executed in case of an event that is dispatched
    ToolsParsersOldSyntEl_Keyword commandKeyWord("Command");
    ToolsParsersOldSyntEl_Keyword commandEQ("==");
    ToolsParsersOldSyntEl_Variable commandValue("command");
    ToolsParsersOldSyntEl_Concatenation command;
    command.push_back(&commandKeyWord);
    command.push_back(&commandEQ);
    command.push_back(&commandValue);

    // connection data
    ToolsParsersOldSyntEl_Keyword dbSwitch("-d");
    ToolsParsersOldSyntEl_Variable dbName("dbName");
    ToolsParsersOldSyntEl_Concatenation db;
    db.push_back(&dbSwitch);
    db.push_back(&dbName);
    ToolsParsersOldSyntEl_Keyword userSwitch("-u");
    ToolsParsersOldSyntEl_Variable userName("user,pwd");
    ToolsParsersOldSyntEl_Concatenation userpw;
    userpw.push_back(&userSwitch);
    userpw.push_back(&userName);
    ToolsParsersOldSyntEl_Keyword xuserSwitch("-U");
    ToolsParsersOldSyntEl_Variable xuserKey("userkey");
    ToolsParsersOldSyntEl_Concatenation xuser;
    xuser.push_back(&xuserSwitch);
    xuser.push_back(&xuserKey);
    ToolsParsersOldSyntEl_AlternativeList user;
    user.push_back(&userpw);
    user.push_back(&xuser);
    ToolsParsersOldSyntEl_Keyword encryptionSwitch("-e");
    ToolsParsersOldSyntEl_Keyword encryptionMethodSSL("SSL");
    ToolsParsersOldSyntEl_Concatenation encryption;
    encryption.push_back(&encryptionSwitch);
    encryption.push_back(&encryptionMethodSSL);
    ToolsParsersOldSyntEl_Option encryptionOption(&encryption);
    ToolsParsersOldSyntEl_Keyword nodeSwitch("-n");
    ToolsParsersOldSyntEl_Variable nodeName("node", "");
    ToolsParsersOldSyntEl_Concatenation node;
    node.push_back(&nodeSwitch);
    node.push_back(&nodeName);
    node.push_back(&encryptionOption);
    ToolsParsersOldSyntEl_Option nodeOption(&node);
    ToolsParsersOldSyntEl_Set connectionData;
    connectionData.push_back(&db);
    connectionData.push_back(&user);
    connectionData.push_back(&nodeOption);

    // list
    ToolsParsersOldSyntEl_Concatenation listSyntax;
    listSyntax.push_back(&listCommand);
    listSyntax.push_back(&cfgFile);

    // start
    ToolsParsersOldSyntEl_Concatenation startSyntax;
    startSyntax.push_back(&startCommand);
    ToolsParsersOldSyntEl_AlternativeList startOptions;
    startOptions.push_back(&remoteAccessOption);
    startOptions.push_back(&internalOption);
    startSyntax.push_back(&startOptions);
    startSyntax.push_back(&cfgFile);
    startSyntax.push_back(&log);
    startSyntax.push_back(&connectionData);

    // stop
    ToolsParsersOldSyntEl_Concatenation stopSyntax;
    ToolsParsersOldSyntEl_Variable instanceID("instanceID");
    stopSyntax.push_back(&stopCommand);
    stopSyntax.push_back(&internalOption);
    stopSyntax.push_back(&instanceID);
    stopSyntax.push_back(&connectionData);

    // state
    ToolsParsersOldSyntEl_Concatenation stateSyntax;
    stateSyntax.push_back(&stateCommand);
    stateSyntax.push_back(&connectionData);

    // delete
    ToolsParsersOldSyntEl_Variable entryID("entryID");
    ToolsParsersOldSyntEl_Concatenation deleteSyntax;
    deleteSyntax.push_back(&deleteCommand);
    deleteSyntax.push_back(&entryID);
    deleteSyntax.push_back(&cfgFile);

    // add
    ToolsParsersOldSyntEl_Concatenation addSyntax;
    addSyntax.push_back(&addCommand);
    addSyntax.push_back(&cfgFile);
    addSyntax.push_back(&eventCondition);
    addSyntax.push_back(&command);

    // the whole syntax
    ToolsParsersOldSyntEl_AlternativeList commandSyntax;
    
    commandSyntax.push_back(&addSyntax);
    commandSyntax.push_back(&deleteSyntax);
    commandSyntax.push_back(&listSyntax);
    commandSyntax.push_back(&startSyntax);
    commandSyntax.push_back(&stateSyntax);
    commandSyntax.push_back(&stopSyntax);
    commandSyntax.push_back(&versionCommand);

    char commandNameForErrorText[1024];
    size_t placeHolder(1023);

    // now parse the command line
    ToolsParsersOld_ProgramParameterParser commandLineParser(argc, argv, &commandSyntax);
    bool syntaxError(false);
    commandLineParser.parse(syntaxError);
    if( syntaxError ) {
        printVersionInfo(stderr);
        const char* syntaxText =
            "add <cfgFile> Name == <value> [Priority == (LOW|MEDIUM|HIGH)] [Value1 (==|>=|<=|>|<) <value>] [Value2 (==|>=|<=|>|<) <value>] Command == <command>\n\n"
            "delete <entryID> <cfgFile>\n\n"
            "list <cfgFile>\n\n"
            "start [-remoteaccess] <cfgFile> -l <logFile> -d <dbName> (-u <user,pwd>|-U <userkey>) [-n <node> [-e SSL]]\n\n"
            "state -d <dbName> (-u <user,pwd>|-U <userkey>) [-n <node> [-e SSL]]\n\n"
            "stop <instanceID> -d <dbName> (-u <user,pwd>|-U <userkey>) [-n <node> [-e SSL]]\n\n"
            "version";

        fprintf(stderr, "invalid arguments.\nusage:\n%s\n\n", syntaxText);
        returnValue = 1;
        goto FinallyBlock;
    }

    // if a logfile is defined, try to use it
    if( logFile.isSet() ) {
        panicFile = new DBMLog_LogFile(logFile.getValue(), true);
        normalFile = new DBMLog_LogFile(logFile.getValue(), false);
        DBMLog_Logger::getInstance().addFile(panicFile);
        DBMLog_Logger::getInstance().addFile(normalFile);
    }

    // calculate the URI for encryption if necessary
    if( nodeName.isSet () ) {
        nodeNameString = nodeName.getValue();
        if( encryptionMethodSSL.isSet() ) {
            RTEComm_URIBuilder myURIBuilder;
            RTEComm_URIBuilder::URIBuildRC buildRC(RTEComm_URIBuilder::Error);
            buildRC = myURIBuilder.BuildDBMURI(
                    (SAPDB_UTF8*)dbName.getValue(),
                    nodeNameString.CharPtr(),
                    mainMsgList,
                    true);
            
            if( buildRC != RTEComm_URIBuilder::NoError ) {
                returnValue = 1;
                goto FinallyBlock;
            }
            else {
                nodeNameString = myURIBuilder.GetURI();
            }
        }
    }

    // decide what to do
    if( addCommand.isSet() ) {
        bool runOkay(false);
        if( migrateConfiguration(cfgFile, mainMsgList) ) {
            DBMCliEvtDispCmd_Add cmdAdd(
                    mainMsgList,
                    cfgFile.getValue(),
                    nameValue,
                    priorityLow,
                    priorityMedium,
                    priorityHigh,
                    value1Value,
                    value1EQ,
                    value1GE,
                    value1SE,
                    value1GT,
                    value1ST,
                    value2Value,
                    value2EQ,
                    value2GE,
                    value2SE,
                    value2GT,
                    value2ST,
                    commandValue.getValue());
            runOkay = cmdAdd.run();
        }
        if( runOkay )
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Info,
                    SDBMSG_DBMED_EXECOKAY));
        else {
            commandNameForErrorText[0] = '\0';
            addCommand.printSyntax(commandNameForErrorText, placeHolder);
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_EXECERR,
                    Msg_Arg("DBMCMD", commandNameForErrorText)));
        }
        DBMCliEvtDisp_MsgDumper::dump(mainMsgList, runOkay?stdout:stderr);
        if( !runOkay ) {
            returnValue = 1;
            goto FinallyBlock;
        }
    }
    else if( deleteCommand.isSet() ) {
        bool runOkay(false);
        if( migrateConfiguration(cfgFile, mainMsgList) ) {
            DBMCliEvtDispCmd_Delete cmdDelete(
                mainMsgList,
                cfgFile.getValue(),
                entryID.getValue());
            runOkay = cmdDelete.run();
        }
        if( runOkay )
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Info,
                    SDBMSG_DBMED_EXECOKAY));
        else {
            commandNameForErrorText[0] = '\0';
            deleteCommand.printSyntax(commandNameForErrorText, placeHolder);
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_EXECERR,
                    Msg_Arg("DBMCMD", commandNameForErrorText)));
        }
        DBMCliEvtDisp_MsgDumper::dump(mainMsgList, runOkay?stdout:stderr);
        if( !runOkay ) {
            returnValue = 1;
            goto FinallyBlock;
        }
    }
    else if( listCommand.isSet() ) {
        bool runOkay(false);
        if( migrateConfiguration(cfgFile, mainMsgList) ) {
            DBMCliEvtDispCmd_List cmdList(
                mainMsgList,
                cfgFile.getValue());
            runOkay = cmdList.run();
        }
        if( runOkay )
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Info,
                    SDBMSG_DBMED_EXECOKAY));
        else {
            commandNameForErrorText[0] = '\0';
            listCommand.printSyntax(commandNameForErrorText, placeHolder);
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_EXECERR,
                    Msg_Arg("DBMCMD", commandNameForErrorText)));
        }
        DBMCliEvtDisp_MsgDumper::dump(mainMsgList, runOkay?stdout:stderr);
        if( !runOkay ) {
            returnValue = 1;
            goto FinallyBlock;
        }
    }
    else if( startCommand.isSet() ) {
        bool runOkay(false);
        commandNameForErrorText[0] = '\0';
        startCommand.printSyntax(commandNameForErrorText, placeHolder);
        if( migrateConfiguration(cfgFile, mainMsgList) ) {
            // start a new dispatcher
            DBMCliEvtDispCmd_Start cmdStart(
                    internal.isSet(),
                    remoteAccess.isSet(),
                    nodeNameString.CharPtr(),
                    dbName.getValue(),
                    userName.getValue(),
                    cfgFile.getValue(),
                    logFile.getValue(),
                    mainMsgList);

            if( !cmdStart.readyToRun() || !cmdStart.run()) {
                // something has gone wrong during dispatcher initialisation
                mainMsgList.Overrule(
                        Msg_List(Msg_List::Error,
                        SDBMSG_DBMED_EXECERR,
                        Msg_Arg("DBMCMD", commandNameForErrorText)));
                DBMCliEvtDisp_MsgDumper::dump(mainMsgList, stderr);
                returnValue = 1;
                goto FinallyBlock;
            }
        }
        else {
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_EXECERR,
                    Msg_Arg("DBMCMD", commandNameForErrorText)));
            DBMCliEvtDisp_MsgDumper::dump(mainMsgList, stderr);
        }
    } 
    else if( stateCommand.isSet() ) {
        commandNameForErrorText[0] = '\0';
        stateCommand.printSyntax(commandNameForErrorText, placeHolder);
        DBMCliEvtDispCmd_State cmdState(
                nodeNameString.CharPtr(),
                dbName.getValue(),
                userName.getValue(),
                mainMsgList);

        if( !cmdState.readyToRun() ) {
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_EXECERR,
                    Msg_Arg("DBMCMD", commandNameForErrorText)));
            return 1;
        }
        bool runOkay(cmdState.run());
        if( runOkay )
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Info,
                    SDBMSG_DBMED_EXECOKAY));
        else {
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_EXECERR,
                    Msg_Arg("DBMCMD", commandNameForErrorText)));
        }
        DBMCliEvtDisp_MsgDumper::dump(mainMsgList, runOkay?stdout:stderr);
        if( !runOkay ) {
            returnValue = 1;
            goto FinallyBlock;
        }
    } 
    else if( stopCommand.isSet() ) {
        // stop a certain dispatcher
        DBMCliEvtDispCmd_Stop cmdStop(
                //internal.isSet(),
                instanceID.getValue(),
                nodeNameString.CharPtr(),
                dbName.getValue(),
                userName.getValue(),
                mainMsgList);

        bool runOkay(cmdStop.run());
        if( runOkay )
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Info,
                    SDBMSG_DBMED_EXECOKAY));
        else {
            commandNameForErrorText[0] = '\0';
            stopCommand.printSyntax(commandNameForErrorText, placeHolder);
            mainMsgList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_EXECERR,
                    Msg_Arg("DBMCMD", commandNameForErrorText)));
        }
        DBMCliEvtDisp_MsgDumper::dump(mainMsgList, runOkay?stdout:stderr);
        if( !runOkay ) {
            returnValue = 1;
            goto FinallyBlock;
        }
    } 
    else if( versionCommand.isSet() ) {
        if( printVersionInfo(stdout)<=0 ) {
            returnValue = 1;
            goto FinallyBlock;
        }
    }
    else {
        fprintf(stderr, "this is impossible...\n");
    }

    // cleanup before leaving method
    FinallyBlock: {
        /* no need to do this, the logger does it
        if( panicFile != NULL )
            delete panicFile;
        if( normalFile != NULL )
            delete normalFile;
        */
        // now we can leave
        return returnValue;
    }
}

/*!
    @brief output callback. Called for each text line of the callstack output
*/
static void
DBMCliEvtDisp_Main_ExceptionOutputCallback(
    char const* const backtraceString, 
    void* const contextInformation ) {
    
    DBMSrvMsg_Error* callStack = (DBMSrvMsg_Error*) contextInformation;
    DBMSrvMsg_Error newBottom(DBMSrvMsg_Error(SDBMSG_DBMSRV_STE, Msg_Arg("DBMSRV_COMP", backtraceString)));
    newBottom.Overrule((*callStack));
    (*callStack) = newBottom;
}

#if defined (WIN32)
/*!
    @brief handler for structured exceptions on windows systems
*/
static int
DBMCliEvtDisp_Main_ExceptionHandler (
    unsigned int exCode,
    struct _EXCEPTION_POINTERS *ep) {

    char              codeBuf[128];
    char              addressBuf[128];
    eo670TraceContext traceContext;
    
    sprintf(codeBuf, "0x%x", exCode);
    sprintf(addressBuf, "0x%p", ep->ExceptionRecord->ExceptionAddress);
    DBMSrvMsg_Error callStack(SDBMSG_DBMSRV_EXCEPTIONCAUGHT, Msg_Arg("DBMSRV_COMP1", addressBuf), Msg_Arg("DBMSRV_COMP2", codeBuf));

    traceContext.threadHandle   = GetCurrentThread();
    traceContext.pThreadContext = ep->ContextRecord;

    eo670_CTraceContextStackOCB(
        &traceContext,
        DBMCliEvtDisp_Main_ExceptionOutputCallback,
        &callStack);

    // dump lines to stderr and logger
    DBMCliEvtDisp_MsgDumper::dump( callStack, stderr );

    return EXCEPTION_EXECUTE_HANDLER;
}
#else
/*!
    @brief unix-like: callstack is collected in this message list (nothing else)
*/
static DBMSrvMsg_Error globalExceptionMessageList;

/*!
    @brief to be able to write an extra entry before the callstack
*/
static void* DBMCliEvtDisp_Main_InitStack(int signalCode) {
    char codeBuf[128];
    char addressBuf[128];
    
    sprintf(codeBuf, "0x%x", signalCode);
    // getting the address would be much more complicated at signal handling...
    sprintf(addressBuf, "0x0");
    globalExceptionMessageList.Overrule(
            DBMSrvMsg_Error(SDBMSG_DBMSRV_EXCEPTIONCAUGHT,
            Msg_Arg("DBMSRV_COMP1", addressBuf),
            Msg_Arg("DBMSRV_COMP2", codeBuf)));
   
    return &globalExceptionMessageList;
}

/*!
    @brief writes the callstack into logger
*/
static void DBMCliEvtDisp_Main_DumpStack(void* const contextInformation) {
    DBMSrvMsg_Error* callStack = (DBMSrvMsg_Error*) contextInformation;
    
    // dump lines to stderr and logger
    DBMCliEvtDisp_MsgDumper::dump( *callStack, stderr );
}

/*!
    @brief signal handler for "crash" signals in unix-like systems
*/
extern "C" void
DBMCliEvtDisp_Main_ExceptionHandler( int signalCode ) {
    ucontext_t userContext;
    void* callStack = NULL;

    // avoid calling this function recursively
    signal(signalCode, SIG_DFL);

    callStack = DBMCliEvtDisp_Main_InitStack(signalCode);
    if (getcontext(&userContext) == 0)
        eo670_CTraceContextStackOCB(&userContext, DBMCliEvtDisp_Main_ExceptionOutputCallback, callStack);
    else
        eo670_CTraceContextStackOCB(NULL, DBMCliEvtDisp_Main_ExceptionOutputCallback, callStack);

    DBMCliEvtDisp_Main_DumpStack(callStack);
}
#endif

// compiler warnings if the try catch block is in a function, where C++ objects are defined
// on the stack, so I put the block in it's own function...
void DBMCliEvtDisp_Main_RunDispatcher(DBMCliEvtDisp_Dispatcher& aDispatcher) {
#if defined (WIN32)
        __try {
#else
        signal( SIGILL, DBMCliEvtDisp_Main_ExceptionHandler );
        signal( SIGTRAP, DBMCliEvtDisp_Main_ExceptionHandler );
        signal( SIGABRT, DBMCliEvtDisp_Main_ExceptionHandler );
        signal( SIGFPE, DBMCliEvtDisp_Main_ExceptionHandler );
        signal( SIGBUS, DBMCliEvtDisp_Main_ExceptionHandler );
        signal( SIGSEGV, DBMCliEvtDisp_Main_ExceptionHandler );
        signal( SIGXCPU, DBMCliEvtDisp_Main_ExceptionHandler );
        #if defined(AIX)
        signal ( SIGIOT, DBMCliEvtDisp_Main_ExceptionHandler );
        #endif
        #if !defined(LINUX) && !defined(FREEBSD)
        signal ( SIGEMT, DBMCliEvtDisp_Main_ExceptionHandler );
        signal ( SIGSYS, DBMCliEvtDisp_Main_ExceptionHandler );
        #endif
#endif

    aDispatcher.Run();

#if defined (WIN32)
        }
        __except (DBMCliEvtDisp_Main_ExceptionHandler(GetExceptionCode(), GetExceptionInformation())) {
        }
#endif
 }