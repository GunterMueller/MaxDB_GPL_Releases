/*!
  \file    DBMSrvFrm_ExceptionHandler.cpp
  \author  MarcW
  \ingroup DBMServer framework
  \brief   handle exceptions

\if EMIT_LICENCE

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


\endif
*/
#if defined( _WIN32 )
#else
extern "C" {
#include <ucontext.h>
}
#endif
#include <time.h>

#include "hcn42.h"
#include "heo670.h"

#include "DBM/Srv/Framework/DBMSrvFrm_ExceptionHandler.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp"
#include "RunTime/System/RTESys_SecurityAttributes.h"
#include "RunTime/System/RTESys_AtomicOperation.h"
#include "RunTime/System/RTESys_ProcessID.hpp"

void DBMSrvFrm_ExceptionHandler_StrackWritingCallback( const char* const backtraceString, void* const contextInformation ) {
    DBMSrvFrm_ExceptionHandler* theExceptionHandler = (DBMSrvFrm_ExceptionHandler*) contextInformation;
    theExceptionHandler->writeExceptionLine( backtraceString );
}

void DBMSrvFrm_ExceptionHandler_DumpWritingCallback( const char* const messageString, void* const contextInformation ) {
    DBMSrvFrm_ExceptionHandler* theExceptionHandler = (DBMSrvFrm_ExceptionHandler*) contextInformation;
    theExceptionHandler->writeDumpStatusMessage( messageString );
}

DBMSrvFrm_ExceptionHandler::DBMSrvFrm_ExceptionHandler()
        : m_IsInitialized(false),
#if defined( _WIN32 )  
          m_DumperThread(NULL),
#else
          m_StackTraceSemaphore(0),
#endif
          m_StackTraceFile(NULL) {
    m_DumpFileName.Init();
}

DBMSrvFrm_ExceptionHandler::~DBMSrvFrm_ExceptionHandler() {
    if( m_StackTraceFile != NULL )
        m_StackTraceFile->Close();
}

bool DBMSrvFrm_ExceptionHandler::initialize( DBMSrvMsg_Error& errOut ) {
    
    if( m_IsInitialized )
        return true;

    m_IsInitialized = true;

    bool initOkay(true);
    tsp00_DbNamec emptyDbName;
    emptyDbName.Init();
    
    if( initOkay ) {
        // open file that we write stacktrace into in case of crash
        tsp00_Pathc stackTraceFileName;
        DBMSrvMsg_Error stackTraceFileError;
        if( !cn42GetFileName( emptyDbName, FGET_DBMSRVSTKTRC_CN42, stackTraceFileName, stackTraceFileError) ) {
            // we could not calculate the file name
            errOut.Overrule(stackTraceFileError);
            initOkay = false;
        }

        if( initOkay ) {
            // open the file
            DBMSrvMsg_ErrorHandler errorHandler;
            errorHandler.activate();
            m_StackTraceFile = new DBMSrvIO_SharedTextFile(stackTraceFileName.asCharp(), tni34_AFile::ToDiskAppend, 1, 1); // open now, create if doesn't exist
            if( m_StackTraceFile == NULL ) {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
                initOkay = false;
            }
            else if( !m_StackTraceFile->IsGood() ) {
                    DBMSrvMsg_Error fileOpenError;
                    errorHandler.getLastError(fileOpenError);
                    errOut.Overrule(fileOpenError);
                    initOkay = false;
            }
            errorHandler.deactivate();
        }

        if( !initOkay ) {
            // something went wrong during file opening
            errOut.Overrule(
                DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, 
	            Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME, stackTraceFileName.asCharp())));
        }
    }

#if defined( _WIN32 )        
    if( initOkay ) {
        // get filename of the file, into which we will write dump in case we crash (windows only)
        DBMSrvMsg_Error dumpFileError;
        if( !cn42GetFileName( emptyDbName, FGET_DBMSRVMINIDUMP_CN42, m_DumpFileName, dumpFileError) ) {
            errOut.Overrule(dumpFileError);
            m_DumpFileName.Init();
            initOkay = false;
        }
    }
#endif
    if( !initOkay ) {
        // something went wrong during initialisation
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EXHNDLR));
    }
    return initOkay;
}

#if defined( _WIN32)
void DBMSrvFrm_ExceptionHandler::writeExceptionHeader(unsigned int exCode, void* exAddress) {
#else
void DBMSrvFrm_ExceptionHandler::writeExceptionHeader(int signalCode) {
#endif
    if( m_StackTraceFile != NULL ) {
        // one day, if the database name is a property of this class, we should also write it to the file
        time_t now;
        time(&now);
        sprintf(
            m_WriteBuffer,
#if defined( _WIN32)
            "PID %d: exception caught (address 0x%p, id 0x%x) at %.24s GMT", (int)RTESys_ProcessID(), exAddress, exCode,
#else
            "PID %d: exception caught (id %d) at %.24s GMT", (int)RTESys_ProcessID(), signalCode,
#endif
            asctime(gmtime(&now))); // only the first 24 chars are printed; field is guaranteed to have 26 chars
        long bytesWritten(0L);
        m_StackTraceFile->WriteLine(m_WriteBuffer, bytesWritten);
        m_StackTraceFile->flush();
    }
}

void DBMSrvFrm_ExceptionHandler::writeExceptionLine( const char* const backtraceString ) {
    if( m_StackTraceFile != NULL ) {
        long bytesWritten(0L);
        
        sprintf(m_WriteBuffer, "PID %d: ", (int)RTESys_ProcessID());
        m_StackTraceFile->Write(m_WriteBuffer, (long)strlen(m_WriteBuffer), bytesWritten);
        m_StackTraceFile->WriteLine(backtraceString, bytesWritten);
        m_StackTraceFile->flush();
    }
}

void DBMSrvFrm_ExceptionHandler::writeDumpStatusMessage( const char* const messageString ) {
    if( m_StackTraceFile != NULL ) {
        long bytesWritten(0L);

        sprintf(m_WriteBuffer, "PID %d: ", (int)RTESys_ProcessID());
        m_StackTraceFile->Write(m_WriteBuffer, (long)strlen(m_WriteBuffer), bytesWritten);
        m_StackTraceFile->WriteLine(messageString, bytesWritten);
        m_StackTraceFile->flush();
    }
}

#if defined( _WIN32 )
int DBMSrvFrm_ExceptionHandler::dumpInformation( 
    unsigned int exCode, struct _EXCEPTION_POINTERS *ep) {

    // necessary for thread safety during crash...
    
    // if we are here because API function DebugBreak() was called, we do nothing
    if( exCode == EXCEPTION_BREAKPOINT )
        return EXCEPTION_CONTINUE_SEARCH;

    /*
        only the first thread may write a stacktrace and a memory dump!
        other threads will wait for it.

        check and exchange in an atomic operation:
        check if dumperThread is NULL:
            if true  -> put newDumperThread into dumperThread variable, return true
            if false -> put value of dumperThread into oldDumperThread, return false
    */
    HANDLE newDumperThread(GetCurrentThread());
    HANDLE oldDumperThread(NULL);
    if( RTESys_CmpxchgPointer(m_DumperThread, NULL, newDumperThread, oldDumperThread) ) {
        
        writeExceptionHeader(exCode, ep->ExceptionRecord->ExceptionAddress);

        eo670TraceContext traceContext;
        traceContext.threadHandle   = m_DumperThread;
        traceContext.pThreadContext = (CONTEXT*)ep->ContextRecord;
        eo670_CTraceContextStackOCB(
            &traceContext,
            DBMSrvFrm_ExceptionHandler_StrackWritingCallback,
            this);

        // write mini dump on windows: "normal" dump including all memory segments
        eo670_WriteMiniDumpOCB(
            GetCurrentThreadId(),
            ep,
            m_DumpFileName.asCharp(),
            true,
            false,
            DBMSrvFrm_ExceptionHandler_DumpWritingCallback,
            this,
            GetOperatorAdminSA() );
    }
    else {
        // all other threads must wait until dumperThread is done
        WaitForSingleObject(oldDumperThread, INFINITE);
    }    
    return EXCEPTION_EXECUTE_HANDLER;
}
#else
void DBMSrvFrm_ExceptionHandler::dumpInformation( int signalCode ) {

    /*
        only the first thread may write a stacktrace!
        what to do with the other threads?

        check and exchange in an atomic operation:
        check if traceWriterSemaphore is 0:
            if true  -> put 1 into dumperThread variable, return true
            if false -> put value of traceWriterSemaphore into neverRead, return false
    */
    SAPDB_Int4 neverRead(0);
    if( RTESys_CmpxchgInt4( m_StackTraceSemaphore, 0, 1, neverRead) ) {

        // other threads must not get here
        signal(signalCode, SIG_DFL);

        // getting the address would be much more complicated at signal handling, so we pass 0
        writeExceptionHeader(signalCode);
        
        ucontext_t userContext;
        if (getcontext(&userContext) == 0)
            eo670_CTraceContextStackOCB(&userContext, DBMSrvFrm_ExceptionHandler_StrackWritingCallback, this);
        else
            eo670_CTraceContextStackOCB(NULL, DBMSrvFrm_ExceptionHandler_StrackWritingCallback, this);
    }
    else {
        // some other thread is currenty writing a stacktrace. what can we do?
    }
}
#endif
