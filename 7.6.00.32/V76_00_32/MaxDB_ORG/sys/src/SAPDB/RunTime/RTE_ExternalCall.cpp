/*!
  @file           RTE_ExternalCall.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          RTE Implementation calling external program interface

    This interface tries to make usage of external programs easier.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_IExternalCall.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/Threading/RTEThread_KernelThread.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/System/RTESys_CommandPipe.h"
#include "RunTime/System/RTESys_IO.h"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "heo01.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "RunTime/RTE_DiagPipe.h" 
#if defined(WIN32)  /*&if $OSSPEC = WIN32 */
#include "gos00k.h" /* nocheck but_imkdep */
#else               /*&else */
#include "gen41.h"  /* nocheck but_imkdep */
#endif              /*&endif */

#if defined(WIN32)
#define RTE_DBMSTART_PROGRAM "dbmstart.exe"
#define RTE_DBMSTOP_PROGRAM "dbmstop.exe"
#else
#define RTE_DBMSTART_PROGRAM "dbmstart"
#define RTE_DBMSTOP_PROGRAM "dbmstop"
#endif

class RTE_AutoRestartShutdownKernelThread : public RTEThread_KernelThread
{
public:
    /*!
    @description    Startup a new restart shutdown thread
    @param          stackSize   [in] - Stack size in bytes.
    @param          startOrStop [in] - True: restart database, false stop database
    @return value   true:   Thread could be started
                    false:  Start failed
    */
    static SAPDB_Bool BeginThread( SAPDB_UInt4   stackSize,
                                   SAPDB_Bool    startOrStop );
private:
    /*!
       @brief          Constructor
     */
    RTE_AutoRestartShutdownKernelThread( SAPDB_Bool startOrStop ) 
                            : RTEThread_KernelThread("AutoRestartShutdownKernelThread"),
                              m_StartOrStop(startOrStop) {}

    /*!
    @description    The created thread runs the code of the methode 'ThreadMain'
    */
    virtual SAPDB_Int4  KernelThreadMain();

  /*!
    @description    Destroy object
    */
   virtual void  DestroyObject() { this->~RTE_AutoRestartShutdownKernelThread();
                                   RTEMem_RteAllocator::Instance().Deallocate(this); }

    SAPDB_Bool   RestartShutdownKernel( SAPDB_Bool doRestart );

private:
    SAPDB_Bool         m_StartOrStop;
};


/*---------------------------- RTE_ExternalCall -----------------------------*/

class RTE_ExternalCall : public RTE_IExternalCall
{
public:
    RTE_ExternalCall(SAPDBMem_IRawAllocator& ownAllocator,
                     SAPDB_UInt4 maxReplySize)
        : m_Allocator(ownAllocator)
        , m_MaxReplySize(maxReplySize)
        , m_ReplyBuffer(0)
        , m_ReplyBufferLength(0)
#if defined(WIN32)
        , m_PipeName(0)
#endif
    {
    }

    virtual ~RTE_ExternalCall()
    {
        if ( m_ReplyBuffer )
        {
            m_ReplyBufferLength = 0;
            Deallocate(m_ReplyBuffer);
        }
#if defined(WIN32)
        if ( m_PipeName )
        {
            Deallocate(m_PipeName);
        }
#endif
    }

    virtual SAPDB_Bool ExecuteCommand( SAPDB_Char const     * command,
                                       RTE_IExternalCallReplyHandler   replyHandler,
                                       RTE_IExternalCallReplyContext * replyContext,
                                       SAPDBErr_MessageList & errList);

    virtual SAPDBMem_IRawAllocator &GetAllocator() { return m_Allocator; }

    static void TrimReplyForKnldiag(SAPDB_Char const *reply);

private:
    /*!
      @brief allocate memory and return it set to zero
      @param what [in] descriptive text for memory region
      @param bytes [in] number of bytes requested
      @param errList [out] errorlist filled if memory not allocated
      @return 0 if call failed, pointer to allocated memory else
     */
    SAPDB_Char *AllocateAndClear( SAPDB_Char const *what,
                                  SAPDB_ULong bytes,
                                  SAPDBErr_MessageList &errList)
    {
        SAPDB_Char *ptr = (SAPDB_Char *)m_Allocator.Allocate(bytes);
        if ( ptr )
        {
            SAPDB_MemFillNoCheck(ptr, 0, bytes);
        }
        else
        {
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_MALLOC_FAILED_FOR,
                                            what );
        }
        return ptr;
    }

    void Deallocate(SAPDB_Char * &ptr)
    {
        m_Allocator.Deallocate(ptr);
        ptr = 0;
    }

#if !defined(WIN32)
    /*!
      @brief fill reply buffer with answer

      Reply buffer is allocated for each command individually and is extended by this function if too small
      @param cmdOutput [in] the file handle for the result pipe
      @return true if call succeeded, false else (errList filled)
     */
    SAPDB_Bool FillReplyBuffer( RTE_FileHandle & cmdOutput,
                                SAPDBErr_MessageList &errList );
#else /* WIN32 only */
    /*!
      @brief fill reply buffer with answer from named pipe

      Reply buffer is allocated for each command individually and is extended by this function if too small.
      The process is constantly monitored during reply handling to abort unwanted waiting if process is already dead.

      @param processHandle [in] the handle for the started process
      @param pipeHandle [in] the handle for the result named pipe
      @return true if call succeeded, false else (errList filled)
     */
    SAPDB_Bool FillReplyBufferFromNamedPipe( HANDLE           processHandle,
                                             HANDLE           pipeHandle,
                                             SAPDBErr_MessageList &errList );
#endif

    SAPDBMem_IRawAllocator& m_Allocator;
    SAPDB_UInt4 const       m_MaxReplySize;
    SAPDB_Char             *m_ReplyBuffer;
    SAPDB_UInt4             m_ReplyBufferLength;
#if defined(WIN32)
    SAPDB_Char             *m_PipeName;
#endif
};

// some space for an answer, that will not produce to much allocations for reply buffer content...
SAPDB_UInt4 RTE_IExternalCall::defaultReplySize=(64*1024);

RTE_IExternalCall *RTE_IExternalCall::CreateExternalCall( SAPDBErr_MessageList & errList
                                                        , SAPDB_UInt4 initialReplySize )
{
    return CreateExternalCall( errList
                              ,RTEMem_RteAllocator::Instance()
                              ,initialReplySize );
}

RTE_IExternalCall *RTE_IExternalCall::CreateExternalCall( SAPDBErr_MessageList & errList
                                                        , SAPDBMem_IRawAllocator & allocator
                                                        , SAPDB_UInt4 initialReplySize )
{
    RTE_IExternalCall *pExternalCall = new ( allocator ) RTE_ExternalCall( allocator
                                                                          ,initialReplySize );
    if ( !pExternalCall )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_MALLOC_FAILED_FOR,
                                        "RTE_ExternalCall object" );
    }
    return pExternalCall;
}

void RTE_IExternalCall::DestroyExternalCall( RTE_IExternalCall *pExternalCall )
{
    if ( pExternalCall )
    {
        destroy ( pExternalCall, pExternalCall->GetAllocator());
    }
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
  @brief Interpret output expecting 'OK'
  @param command [in] used command string
  @param replyContext [in] context for the call
  @param errList [out] errorlist to be filled if parsing failes
  @return true if call succeeded, false else (errList filled)
 */
SAPDB_Bool RTE_ExternalCallReplyOk( SAPDB_Char const *command,
                                    SAPDB_Char const *reply,
                                    RTE_IExternalCallReplyContext *, // reply context not needed
                                    SAPDBErr_MessageList &errList)
{
    SAPDB_Char const *pReply = reply;

    while ( isspace(*pReply) )
        ++pReply;

    if ( pReply[0] != 'O'
      || pReply[1] != 'K' )
    {
        RTE_ExternalCall::TrimReplyForKnldiag(reply);
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_UNEXPECTED_REPLY,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(reply) );
        return false;
    }

    return true;
}

//--------------------------------------------------------------

SAPDB_Bool RTE_ExternalCallIgnoreReply( SAPDB_Char const *,
                                    SAPDB_Char const *,
                                    RTE_IExternalCallReplyContext *,
                                    SAPDBErr_MessageList &)
{
    return true;
}

//---------------------------------------------
//   RTE_ExternalCall
//---------------------------------------------

/*!
  @brief   Trim reply from external command, to allow display in knldiag without corrupting this file

  Especially new lines are dangerous.... Leading spaces are trimmed away and any groups of whitespaces
  are replaced by a single space char. The last space is removed at last.

  @param [inout] the reply to trim
 */
void RTE_ExternalCall::TrimReplyForKnldiag(SAPDB_Char const *reply)
{
    SAPDB_Char const *pReply = reply;
    SAPDB_Char *pTrim = const_cast<SAPDB_Char *>(reply);

    // skip leading white spaces
    while ( pReply && isspace(*pReply) )
    {
        ++pReply;
    }

    // compress spaces and convert newline to spaces
    SAPDB_Bool gotSpace = false;
    while ( pReply && *pReply )
    {
        SAPDB_Char const singleChar = *pReply++;

        if ( isspace(singleChar) )
        {
            if ( !gotSpace )
            {
                *pTrim++ = ' ';
            }
            gotSpace = true;
        }
        else
        {
            gotSpace = false;
            *pTrim++ = singleChar;
        }
    }

     // reomve trailing space if it exists
    if ( pTrim != const_cast<SAPDB_Char *>(reply) )
    {
        if ( *--pTrim == ' ' )
        {
            *pTrim = 0;
        }
    }
}

//-----------------------------

SAPDB_Bool RTE_ExternalCall::ExecuteCommand( SAPDB_Char const *command,
                                             RTE_IExternalCallReplyHandler  replyHandler,
                                             RTE_IExternalCallReplyContext *replyContext,
                                             SAPDBErr_MessageList &errList)
{
    SAPDB_Bool callResult;

    if ( replyHandler != RTE_ExternalCallIgnoreReply )
    {
        if ( !m_ReplyBuffer )
        {
            m_ReplyBuffer = AllocateAndClear("ReplyBuffer", m_MaxReplySize, errList);
            if ( !m_ReplyBuffer )
            {
                return false;
            }
            m_ReplyBufferLength = m_MaxReplySize;
        }
    }

#if defined(WIN32)
#define RTE_WINDOWS_COMMAND_OUTPUT_PIPE "\\\\.\\pipe\\SAPDB_%d"

    m_PipeName = AllocateAndClear("PipeName", strlen(RTE_WINDOWS_COMMAND_OUTPUT_PIPE)+20, errList);
    if ( !m_PipeName )
    {
        return false;
    }
    SAPDB_sprintf(m_PipeName, strlen(RTE_WINDOWS_COMMAND_OUTPUT_PIPE)+20, RTE_WINDOWS_COMMAND_OUTPUT_PIPE, (int)GetCurrentProcessId() );

    HANDLE pipeHandle = CreateNamedPipe( m_PipeName,
                                         PIPE_ACCESS_INBOUND,
                                         PIPE_TYPE_BYTE     |
                                         PIPE_READMODE_BYTE |
                                         PIPE_NOWAIT,
                                         1,
                                         m_MaxReplySize,
                                         m_MaxReplySize,
                                         INFINITE,
                                         NULL );
    if ( INVALID_HANDLE_VALUE == pipeHandle )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_OPEN_OUTPUT_PIPE,
                                        m_PipeName,
                                        SAPDB_ToString(GetLastError()) );
        return false;
    }

    (void)ConnectNamedPipe(pipeHandle, NULL); // More error handling takes place during pipe read

    PROCESS_INFORMATION processInfo;
    STARTUPINFO         startInfo; 
    RTE_SystemRc rc;

    SAPDB_MemFillNoCheck( &processInfo, 0, sizeof(PROCESS_INFORMATION) );
 
    SAPDB_MemFillNoCheck( &startInfo, 0, sizeof(STARTUPINFO) );

    startInfo.cb = sizeof(STARTUPINFO); 
 
    // Create the child process. 
    if ( !CreateProcess(NULL, 
          (LPTSTR)command, /* command line */
          NULL,          /* process security attributes */
          NULL,          /* primary thread security attributes  */
          false,         /* handles are NOT inherited  */
          CREATE_NO_WINDOW,
          NULL,          /* use parent's environment  */
          NULL,          /* use parent's current directory  */
          &startInfo,    /* STARTUPINFO pointer */
          &processInfo)  /* receives PROCESS_INFORMATION */
       )
    {
        rc = GetLastError();
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(rc)
                                       );
        DisconnectNamedPipe(pipeHandle);
        CloseHandle(pipeHandle);
        return false;
    }
    
    if ( replyHandler != RTE_ExternalCallIgnoreReply )
    {
        if ( !FillReplyBufferFromNamedPipe( processInfo.hProcess,
                                            pipeHandle,
                                            errList) )
        {
            CloseHandle(processInfo.hProcess);
            DisconnectNamedPipe(pipeHandle);
            CloseHandle(pipeHandle);
            return false;        
        }
    }

    RTE_SystemRc result = RTE_SYSTEMRC_NO_ERROR;
    DWORD terminationStatus;
    
    do
    {
        if ( !GetExitCodeProcess(processInfo.hProcess, &terminationStatus) )
        {
            rc = GetLastError();
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_COMMAND_PIPE_CLOSE,
                                            SAPDB_ToString(command),
                                            SAPDB_ToString(rc)
                                           );
            CloseHandle(processInfo.hProcess);
            DisconnectNamedPipe(pipeHandle);
            CloseHandle(pipeHandle);
            return false;
        }

        if ( STILL_ACTIVE == terminationStatus )
        {
            WaitForSingleObject(processInfo.hProcess, INFINITE);
        }

    } while ( STILL_ACTIVE == terminationStatus );

    CloseHandle(processInfo.hProcess);
    DisconnectNamedPipe(pipeHandle);
    CloseHandle(pipeHandle);

    callResult = ( 0 == terminationStatus );
#else
    RTESys_CommandOutputPipeContext cmdContext;
    RTE_SystemRc rc;
    rc = RTESys_OpenCommandOutputPipe(cmdContext, command);
    if ( RTE_SYSTEMRC_NO_ERROR != rc )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_OPEN,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(rc)
                                       );
        return false;        
    }

    if ( !FillReplyBuffer( cmdContext.cmdOutput,
                           errList) )
    {
        return false;        
    }

    SAPDB_Int4 exitStatus;
    rc = RTESys_CloseCommandOutputPipe(cmdContext, exitStatus);
    if ( RTE_SYSTEMRC_NO_ERROR != rc )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEERR_COMMAND_PIPE_CLOSE,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(rc)
                                       );
        return false;        
    }

    callResult = ( exitStatus == 0 );
#endif

    if ( replyHandler != RTE_ExternalCallIgnoreReply )
    {
        callResult = (*replyHandler)(command, m_ReplyBuffer, replyContext, errList);

        // free memory if we got an oversized reply...
        if ( m_ReplyBufferLength > m_MaxReplySize )
        {
            m_ReplyBufferLength = 0;
            Deallocate(m_ReplyBuffer);
        }
    }

    return callResult;
} // RTE_ExternalCall::ExecuteCommand

//---------------------------------------------

#if !defined(WIN32)

SAPDB_Bool RTE_ExternalCall::FillReplyBuffer( RTE_FileHandle & cmdOutput,
                                              SAPDBErr_MessageList &errList )
{
    SAPDB_Bool callResult = true;
    SAPDB_Char * nextInput;
    tsp00_VfReturn_Param returnValue;
    RTE_FileOffset bytesReadFromPipe;
    RTE_FileOffset remainingBufferSpace = m_ReplyBufferLength;

    SAPDB_MemFillNoCheck(m_ReplyBuffer, 0, m_ReplyBufferLength);
    nextInput = m_ReplyBuffer;

    do
    {
        bytesReadFromPipe = 0;
        RTESys_IORead( cmdOutput, nextInput, remainingBufferSpace, bytesReadFromPipe, returnValue);
        if ( vf_ok == returnValue && bytesReadFromPipe > 0 )
        {
            remainingBufferSpace -= bytesReadFromPipe;
            nextInput += bytesReadFromPipe;

            SAPDBERR_ASSERT_STATE(remainingBufferSpace > 0);

            if ( 0 == remainingBufferSpace ) // reallocate extended buffer...
            {
                SAPDB_Char *extendedReplyBuffer = AllocateAndClear("ExtendedReplyBuffer", 
                                                            m_ReplyBufferLength + m_MaxReplySize, 
                                                            errList);
                if ( !extendedReplyBuffer )
                {
                    callResult = false;
                    break;
                }

                SAPDB_MemCopyNoCheck(extendedReplyBuffer, m_ReplyBuffer, m_ReplyBufferLength);
                nextInput = (extendedReplyBuffer + (nextInput - m_ReplyBuffer));

                m_ReplyBufferLength += m_MaxReplySize;
                Deallocate(m_ReplyBuffer);
                m_ReplyBuffer = extendedReplyBuffer;

                remainingBufferSpace += m_MaxReplySize;
            }
        }
    } while ( vf_ok == returnValue );

    return callResult;
} // RTE_ExternalCall::FillReplyBuffer

#else /* WIN32 does not support redirection... */

//---------------------------------------------

SAPDB_Bool RTE_ExternalCall::FillReplyBufferFromNamedPipe( HANDLE processHandle,
                                                           HANDLE pipeHandle,
                                                           SAPDBErr_MessageList &errList )
{
    SAPDB_Bool callResult = true;
    SAPDB_Char * nextInput;
    tsp00_VfReturn_Param returnValue;
    RTE_FileOffset bytesReadFromPipe;
    RTE_FileOffset remainingBufferSpace = m_ReplyBufferLength;

    SAPDB_MemFillNoCheck(m_ReplyBuffer, 0, m_ReplyBufferLength);
    nextInput = m_ReplyBuffer;


    do
    {
        bytesReadFromPipe = 0;
        // Asynchronous opened Output pipe synchronous read....
#define RTE_EXTERNAL_CALL_PIPE_READ_TIMEOUT 100
        {
            SAPDBErr_MessageList localErrList;
            SAPDB_Bool readPipeDone = false;
            while ( !readPipeDone ) 
            {
                switch( WaitForSingleObject(pipeHandle, RTE_EXTERNAL_CALL_PIPE_READ_TIMEOUT) )
                {
                    case WAIT_OBJECT_0:
                        localErrList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                             RTEERR_OUTPUT_PIPE_READ,
                                                             "Pipe handle signals"
                                                           );
                        RTE_Message(localErrList);
                        break;

                    case WAIT_ABANDONED:
                        readPipeDone = true;
                        localErrList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                             RTEERR_OUTPUT_PIPE_READ,
                                                             "Wait for handle abandoned"
                                                           );
                        RTE_Message(localErrList);
                        returnValue = vf_notok;
                        break;

                    case WAIT_TIMEOUT:
                        break;

                    default:
                        break;
                }

                DWORD winBytesReadFromPipe;
                SAPDB_Bool lastTry = false;
                DWORD terminationStatus;

                if ( !ReadFile(pipeHandle, nextInput, (DWORD)remainingBufferSpace, &winBytesReadFromPipe, NULL) )
                {
                    if ( lastTry )
                    {
                        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                        RTEERR_COMMAND_PIPE_CLOSE,
                                                        "Kernel command no longer active",
                                                        SAPDB_ToString(terminationStatus)
                                                      );
                        callResult = false;
                        break;
                    }

                    DWORD rc = GetLastError();

                    if ( ERROR_PIPE_LISTENING != rc && ERROR_NO_DATA != rc )
                    {
                        readPipeDone = true;
                        if ( ERROR_BROKEN_PIPE == rc || ERROR_HANDLE_EOF == rc )
                        {
                            returnValue = vf_eof;
                        }
                        else
                        {
                            localErrList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                                 RTEERR_OUTPUT_PIPE_READ,
                                                                 SAPDB_ToString(rc)
                                                               );
                            RTE_Message(localErrList);
                            returnValue = vf_notok;
                        }
                    }
                    else
                    {
                        if ( !GetExitCodeProcess(processHandle, &terminationStatus) )
                        {
                            rc = GetLastError();
                            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                            RTEERR_COMMAND_PIPE_CLOSE,
                                                            "Cannot get exit status from kernel command",
                                                            SAPDB_ToString(rc)
                                                          );
                            callResult = false;
                            break;
                        }

                        if ( 0 == terminationStatus )
                        {
                            readPipeDone = true;
                            returnValue = vf_eof;
                        }
                        else if ( STILL_ACTIVE != terminationStatus )
                        {
                            lastTry = true;
                            localErrList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                            RTEERR_COMMAND_PIPE_CLOSE,
                                                            "Kernel command first no longer active ignored",
                                                            SAPDB_ToString(terminationStatus)
                                                          );
                            RTE_Message(localErrList);
                            if ( !ReadFile(pipeHandle, nextInput, (DWORD)remainingBufferSpace, &winBytesReadFromPipe, NULL) )
                            readPipeDone = false;
                        }
                    }
                }
                else
                {
                    bytesReadFromPipe = winBytesReadFromPipe;
                    readPipeDone = true;
                }
            }
        }

        if ( vf_ok == returnValue && bytesReadFromPipe > 0 )
        {
            remainingBufferSpace -= bytesReadFromPipe;
            nextInput += bytesReadFromPipe;

            SAPDBERR_ASSERT_STATE(remainingBufferSpace > 0);

            if ( 0 == remainingBufferSpace ) // reallocate extended buffer...
            {
                SAPDB_Char *extendedReplyBuffer = AllocateAndClear("ExtendedReplyBuffer", 
                                                            m_ReplyBufferLength + m_MaxReplySize, 
                                                            errList);
                if ( !extendedReplyBuffer )
                {
                    callResult = false;
                    break;
                }

                SAPDB_MemCopyNoCheck(extendedReplyBuffer, m_ReplyBuffer, m_ReplyBufferLength);
                nextInput = (extendedReplyBuffer + (nextInput - m_ReplyBuffer));

                m_ReplyBufferLength += m_MaxReplySize;
                Deallocate(m_ReplyBuffer);
                m_ReplyBuffer = extendedReplyBuffer;

                remainingBufferSpace += m_MaxReplySize;
            }
        }
    } while ( vf_ok == returnValue );

    return callResult;
} // RTE_ExternalCall::FillReplyBufferFromNamedPipe
#endif

//-----------------------------------------------------------------

SAPDB_Bool RTE_SpawnDbmstartOrStop( SAPDB_Bool startOrStop )
{
    if ( RTE_AutoRestartShutdownKernelThread::BeginThread( 64*1024, startOrStop ) )
    {
        if ( startOrStop )
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT
                       , RTEINFO_AUTOMATIC_RESTART_INITIATED ) );
        }
        else
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT
                       , RTEINFO_AUTOMATIC_SHUTDOWN_INITIATED ) );
        }
        return true;
    }

    if ( startOrStop )
    {
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT
                                         , RTEERR_AUTOMATIC_RESTART_FAILED ) );
    }
    else
    {
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT
                                         , RTEERR_AUTOMATIC_SHUTDOWN_FAILED ) );
    }

    return false;
}

//-----------------------------------------------------------------
    
extern "C" SAPDB_Bool RTE_SpawnRestartShutdown( SAPDB_Bool startOrStop )
{
    return RTE_SpawnDbmstartOrStop(startOrStop);
}

//-----------------------------------------------------------------
    
SAPDB_Bool RTE_AutoRestartShutdownKernelThread::RestartShutdownKernel( SAPDB_Bool doRestart )
{
    tsp00_Pathc    pgmPath;
    tsp01_RteError rteError;
    SAPDB_MemFillNoCheck(&rteError, 0, sizeof(tsp01_RteError));

    if ( !sqlGetDbrootPgmPath ( pgmPath
                              , TERM_WITH_DELIMITER_EO01
                              , &rteError ) )
    {
        RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT
                                        , RTEERR_DEPENDEND_PROGRAMPATH_NOT_FOUND
                                        , SAPDB_ToString(rteError.RteErrCode)
                                        , rteError.RteErrText
                                        , SAPDB_ToString(rteError.OsErrCode)
                                        , rteError.OsErrText ));
        return false;
    }

    SAPDB_Char *serverdb;

#if defined(WIN32)
    serverdb = (SAPDB_Char *)RTE_KGS::Instance().GetKGS()->szServerDB;
#else
    serverdb = (SAPDB_Char *)RTE_KGS::Instance().GetKGS()->serverdb;
#endif
    SAPDB_Char const *startOrStop = ( doRestart ? RTE_DBMSTART_PROGRAM : RTE_DBMSTOP_PROGRAM );

    SAPDB_Char *command = (SAPDB_Char *)RTEMem_RteAllocator::Instance().Allocate( strlen(pgmPath.asCharp())
                                                                                + strlen(startOrStop)
                                                                                + strlen(" -d ")
                                                                                + strlen(serverdb)
                                                                                + 1);
    if ( !command )
    {
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                           RTEERR_MALLOC_FAILED_FOR,
                                           startOrStop ) );
        return false;
    }

    strcpy( command, pgmPath.asCharp() );
    strcat( command, startOrStop );
    strcat( command, " -d ");
    strcat( command, serverdb);

    SAPDBErr_MessageList errList;
    
    RTE_IExternalCall *pCall = RTE_IExternalCall::CreateExternalCall(errList, 1024);
    if ( !pCall )
    {
        RTE_Message(errList);
        return false;
    }

    if ( !pCall->ExecuteCommand( command
                               , RTE_ExternalCallIgnoreReply
                               , 0
                               , errList ) )
    {
        RTE_Message(errList);
        RTE_IExternalCall::DestroyExternalCall(pCall);
        return false;
    }

    RTE_IExternalCall::DestroyExternalCall(pCall);
    return true;
}

//-----------------------------------------------------------------
    
SAPDB_Int4  RTE_AutoRestartShutdownKernelThread::KernelThreadMain()
{
    if ( RestartShutdownKernel( m_StartOrStop ) )
    {
        if ( m_StartOrStop )
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT
                                             , RTEINFO_AUTOMATIC_RESTART_DONE ) );
        }
        else
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT
                                             , RTEINFO_AUTOMATIC_SHUTDOWN_DONE ) );
        }
    }
    else
    {
        if ( m_StartOrStop )
        {
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT
                                             , RTEERR_AUTOMATIC_RESTART_FAILED ) );
            /* tidy up... */
            RTE_CloseDiagPipe();

#if defined(_WIN32)
            sql61k_post_xstart_sem ( kgs.szServerDB, false );
#else
            (void)en41RemovePipeFDFile(&kgs->serverdb[0]);
#endif
        }
        else
        {
            RTE_Crash( SAPDBErr_MessageList( RTE_CONTEXT
                                           , RTEERR_AUTOMATIC_SHUTDOWN_FAILED ) );
        }
    }

    return 0;
}

//-----------------------------------------------------------------

SAPDB_Bool RTE_AutoRestartShutdownKernelThread::BeginThread( SAPDB_UInt4   stackSize,
                                                             SAPDB_Bool    startOrStop )
{
    RTE_AutoRestartShutdownKernelThread*   pInstance;
    SAPDBErr_MessageList                   messageList;

    pInstance  = new (RTEMem_RteAllocator::Instance()) RTE_AutoRestartShutdownKernelThread( startOrStop );

    if ( 0 == pInstance )
    {
        RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_MALLOC_FAILED_FOR, "RTE_AutoRestartShutdownKernelThread" ));
        return false;
    }

    if ( pInstance->Create( RTE_AutoRestartShutdownKernelThread::Normal, stackSize,
                            messageList ) != RTE_AutoRestartShutdownKernelThread::NoError )
    {
        pInstance->DestroyObject();
        RTE_Message( messageList );
        return false;
    }

    pInstance->Forget();

    return true;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/