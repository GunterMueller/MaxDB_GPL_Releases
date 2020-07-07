/*!
    @file           DBMSrvScd_Main.cpp
    @author         MarcW
    @brief          Scheduler  - main function

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#include <time.h>

#if !defined (WIN32)
#include <signal.h>
extern "C" {
  #include <ucontext.h>
} 
#endif

#include "hcn42.h"
#include "heo670.h"

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Logging/DBMLog_LogFile.hpp"
#include "DBM/Logging/DBMLog_SynchronizedLogFile.hpp"
#include "DBM/Srv/Scheduler/DBMSrvScd_Scheduler.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"

/*!
    @brief output callback. Called for each text line of the callstack output
*/
static void
DBMSrvScd_Main_ExceptionOutputCallback(
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
DBMSrvScd_Main_ExceptionHandler (
    unsigned int exCode,
    struct _EXCEPTION_POINTERS *ep) {

    if( exCode == EXCEPTION_BREAKPOINT ) // allow DebugBreak()
        return EXCEPTION_CONTINUE_SEARCH;

    char              codeBuf[128];
    char              addressBuf[128];
    eo670TraceContext traceContext;
    
    sprintf(codeBuf, "0x%x", exCode);
    sprintf(addressBuf, "0x%p", ep->ExceptionRecord->ExceptionAddress);
    DBMSrvMsg_Error callStack(SDBMSG_DBMSRV_EXCEPTIONCAUGHT, Msg_Arg("DBMSRV_COMP1", addressBuf), Msg_Arg("DBMSRV_COMP2", codeBuf));

    traceContext.threadHandle   = GetCurrentThread();
    traceContext.pThreadContext = (CONTEXT*)ep->ContextRecord;

    eo670_CTraceContextStackOCB(
        &traceContext,
        DBMSrvScd_Main_ExceptionOutputCallback,
        &callStack);

    DBMLog_Logger::getInstance().addMessageList(callStack);
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
static void* DBMSrvScd_Main_InitStack(int signalCode) {
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
static void DBMSrvScd_Main_DumpStack(void* const contextInformation) {
    DBMSrvMsg_Error* callStack = (DBMSrvMsg_Error*) contextInformation;
    DBMLog_Logger::getInstance().addMessageList(*callStack);
}

/*!
    @brief signal handler for "crash" signals in unix-like systems
*/
extern "C" void
DBMSrvScd_Main_ExceptionHandler( int signalCode ) {
    ucontext_t userContext;
    void* callStack = NULL;

    // avoid calling this function recursively
    signal(signalCode, SIG_DFL);
    
    callStack = DBMSrvScd_Main_InitStack(signalCode);
    if (getcontext(&userContext) == 0)
        eo670_CTraceContextStackOCB(&userContext, DBMSrvScd_Main_ExceptionOutputCallback, callStack);
    else
        eo670_CTraceContextStackOCB(NULL, DBMSrvScd_Main_ExceptionOutputCallback, callStack);

    DBMSrvScd_Main_DumpStack(callStack);
}
#endif

int main (int argcIn, char* argvIn[])
{
    int argc(0); 
    char** argv=NULL;

    int nRC = Tools_PipeCall::ReadArguments(
            argcIn,
            argvIn,
            argc,
            argv);

    if( nRC != 0 ) {
        fprintf(stderr, "%s: could not read arguments from commandline. reason: %s\n",
            argvIn[0],
            Tools_PipeCall::ErrorReason());
        fflush(stderr);
        return 1;
    }

    if( argc == 5 ) {
        DBMSrvMsg_Error* mainErrorList(new DBMSrvMsg_Error()); // compiler warning if created on stack
        if( mainErrorList == NULL ) {
            fprintf(stderr, "out of memory\n");
            fflush(stderr);
            return 1;
        }
        mainErrorList->ClearMessageList();

        tsp00_Pathc   szLogFilePath;
        tsp00_DbNamec dbName;
        DBMLog_LogFile* normalFile(NULL);
        DBMLog_LogFile* panicFile(NULL);

        // should check arguments more thoroughly...
        // attach to shared memory
        dbName.rawAssign(argv[2]);
        bool shmAvailable(DBMSrvShM_ShmAdmin::TheAdmin().initialize(dbName.asCharp(), *mainErrorList));

        // initialize logging
        // a synched file for normal logging
        // a direct file for panic logging
        cn42GetFileName(
            dbName,
            FGET_DBMPRT_CN42,
            szLogFilePath);
        if( shmAvailable )
            normalFile = new DBMLog_SynchronizedLogFile(szLogFilePath.asCharp());
        else
            normalFile = new DBMLog_LogFile(szLogFilePath.asCharp(), false);
        panicFile = new DBMLog_LogFile(szLogFilePath.asCharp(), true);
        DBMLog_Logger::getInstance().addFile(normalFile);
        DBMLog_Logger::getInstance().addFile(panicFile);

        if( shmAvailable ) {
            int retVal(2);
#if defined (WIN32)
            __try {
#else
            signal( SIGILL, DBMSrvScd_Main_ExceptionHandler );
            signal( SIGTRAP, DBMSrvScd_Main_ExceptionHandler );
            signal( SIGABRT, DBMSrvScd_Main_ExceptionHandler );
            signal( SIGFPE, DBMSrvScd_Main_ExceptionHandler );
            signal( SIGBUS, DBMSrvScd_Main_ExceptionHandler );
            signal( SIGSEGV, DBMSrvScd_Main_ExceptionHandler );
            signal( SIGXCPU, DBMSrvScd_Main_ExceptionHandler );
            #if defined(AIX)
            signal ( SIGIOT, DBMSrvScd_Main_ExceptionHandler );
            #endif
            #if !defined(LINUX) && !defined(FREEBSD)
            signal ( SIGEMT, DBMSrvScd_Main_ExceptionHandler );
            signal ( SIGSYS, DBMSrvScd_Main_ExceptionHandler );
            #endif
#endif
            DBMSrvScd_Scheduler* theScheduler = new DBMSrvScd_Scheduler(argv[2], argv[4]);
            if( theScheduler != NULL ) {
                if( theScheduler->isValid() ) {
                    theScheduler->run();
                    retVal = 0;
                }
                delete theScheduler;
            }
#if defined (WIN32)
            }
            __except (DBMSrvScd_Main_ExceptionHandler(GetExceptionCode(), GetExceptionInformation())) {
            }
#endif

            DBMLog_Logger::getInstance().freeResources();
            // close shm
            DBMSrvShM_ShmAdmin::TheAdmin().close(*mainErrorList);
            delete mainErrorList;
            return retVal;
        }
        else {
            // shared memory not available
            // cleanup and exit
            DBMLog_Logger::getInstance().addMessageList(*mainErrorList);
            DBMLog_Logger::getInstance().freeResources();
            delete mainErrorList;
            return 1;           
        }
    }
    else {
        printf("%s: this program is not to be called interactively.\n", argv[0]);
        return 1;
    }
}
