/*!---------------------------------------------------------------------
  @file           RTESys_CommandPipe.c
  @author         JoergM
  @brief          System Calls for piping a command output, input or both
  @see            


  Currently only command output is needed and implemented...

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
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/System/RTESys_CommandPipe.h"


#if !defined(_WIN32)
#include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

#if !defined(WIN32)
/* only needed for UNIX, NT allows to create process from command line, UNIX needs arguments separation... */
static int BuildArgumentVector(const char *cmdline, char **pArgv, char *argArea);
static RTE_SystemRc UnixOpenCommandPipe( RTESys_CommandOutputPipeContext *pContext
                      , char *argArea
                      , char **cmdArgv );
#endif

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

RTE_SystemRc RTESys_OpenCommandOutputPipe(RTESys_CommandOutputPipeContext *pContext, SAPDB_Char const *command)
{
    RTE_SystemRc result = RTE_SYSTEMRC_NO_ERROR;

#if defined(WIN32)

    HANDLE callerStdout, callerStderr, inheritStdoutRead, childStdoutWrite;
    SECURITY_ATTRIBUTES doInherit;
    STARTUPINFO         startInfo; 
    PROCESS_INFORMATION processInfo;

    doInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
    doInherit.lpSecurityDescriptor = NULL;
    doInherit.bInheritHandle = TRUE;

    callerStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    callerStderr = GetStdHandle(STD_ERROR_HANDLE);

    if ( !CreatePipe(&inheritStdoutRead,
                     &childStdoutWrite,
                     &doInherit,
                     0) )
    {
        return GetLastError();
    }

    if (! SetStdHandle(STD_OUTPUT_HANDLE, childStdoutWrite)) 
    {
        result = GetLastError();
        CloseHandle(inheritStdoutRead);
        CloseHandle(childStdoutWrite);
        SetStdHandle(STD_OUTPUT_HANDLE, callerStdout);
        return result;
    }

    if (! SetStdHandle(STD_ERROR_HANDLE, childStdoutWrite))
    {
        result = GetLastError();
        CloseHandle(inheritStdoutRead);
        CloseHandle(childStdoutWrite);
        SetStdHandle(STD_OUTPUT_HANDLE, callerStdout);
        SetStdHandle(STD_ERROR_HANDLE, callerStderr);
        return result;
    }

    if ( !DuplicateHandle( GetCurrentProcess(), inheritStdoutRead,
                           GetCurrentProcess(), &pContext->cmdOutput,
                           0,
                           FALSE,
                           DUPLICATE_SAME_ACCESS) )
    {
        result = GetLastError();
        CloseHandle(inheritStdoutRead);
        CloseHandle(childStdoutWrite);
        SetStdHandle(STD_OUTPUT_HANDLE, callerStdout);
        SetStdHandle(STD_ERROR_HANDLE, callerStderr);
        return result;
    }

    CloseHandle(inheritStdoutRead);
 
    memset( &processInfo, 0, sizeof(PROCESS_INFORMATION) );
 
    memset( &startInfo, 0, sizeof(STARTUPINFO) );

    startInfo.cb = sizeof(STARTUPINFO); 
 
    // Create the child process. 
 
    if ( !CreateProcess(NULL, 
          (LPTSTR)command,       /* command line */
          NULL,          /* process security attributes */
          NULL,          /* primary thread security attributes  */
          TRUE,          /* handles are inherited  */
          0,             /* creation flags  */
          NULL,          /* use parent's environment  */
          NULL,          /* use parent's current directory  */
          &startInfo,    /* STARTUPINFO pointer */
          &processInfo)  /* receives PROCESS_INFORMATION */
       )
    {
        result = GetLastError();
        CloseHandle(pContext->cmdOutput);
        CloseHandle(childStdoutWrite);
    }
    else
    {
        pContext->childProcess = (void *)processInfo.hProcess;
        CloseHandle(childStdoutWrite);
    }

    if ( !SetStdHandle(STD_OUTPUT_HANDLE, callerStdout) )
    {
        WriteFile(callerStdout, "ReRedirOutFailed\n", 17, NULL, NULL);
        WriteFile(callerStderr, "ReRedirOutFailed\n", 17, NULL, NULL);
    }
    if ( !SetStdHandle(STD_ERROR_HANDLE, callerStderr) )
    {
        WriteFile(callerStdout, "ReRedirErrFailed\n", 17, NULL, NULL);
        WriteFile(callerStderr, "ReRedirErrFailed\n", 17, NULL, NULL);
    }

#else

    SAPDB_Int     cmdArgc;
    SAPDB_Char *  argArea;
    SAPDB_Char ** cmdArgv;
  
    /* malloc space (single malloc call...)
                     command copy as parameters (zero terminated) 
                   + padding for (char *) alignment
                   + argv[] pointer (maximum (strlen(command)+1)/2 arguments possible + NULL pointer 
     */
    argArea = (char *)RTE_save_malloc(  (strlen(command)+sizeof(char)) 
                                        + sizeof(char *)
                                        + (
                                            sizeof(char *) 
                                              * ( 1 + ( (strlen(command)+sizeof(char))/2 ) ) 
                                          )
                                       );
    if ( !argArea )
    {
        return RTE_SYSTEMRC_NOT_ENOUGH_MEMORY;
    }

    /* malloc'ed space organization: First command string copy followed by a 'char *' aligned array of argv[] */
    cmdArgv = ((char **)argArea)+((strlen(command)+1+sizeof(char *))/sizeof(char *));

    cmdArgc = BuildArgumentVector(command, cmdArgv, argArea);
    if ( cmdArgc < 1 )
    {
        return EINVAL;
    }

    result = UnixOpenCommandPipe(pContext, argArea, cmdArgv);

#endif

    return result;
}

/*------------------------------------------------------*/

RTE_SystemRc RTESys_OpenCommandVectorOutputPipe( RTESys_CommandOutputPipeContext *pContext
                                               , SAPDB_Char **cmdArgv )
{
    RTE_SystemRc result = RTE_SYSTEMRC_NO_ERROR;

#if defined(WIN32)
    {
        SAPDB_Int    iArg;
        SAPDB_Int4   totalLength = 0;
        SAPDB_Char * command = 0;

        for ( iArg = 0; cmdArgv[iArg]; ++iArg )
        {
            totalLength += (SAPDB_Int4)(strlen(cmdArgv[iArg]) + 1);
        }

        command = malloc(totalLength);
        if ( !command )
        {
            return RTE_SYSTEMRC_NOT_ENOUGH_MEMORY;
        }

        memset(command, 0, totalLength);

        for ( iArg = 0; cmdArgv[iArg]; ++iArg )
        {
            strcat(command, cmdArgv[iArg]);
            if ( cmdArgv[iArg+1] )
            {
                strcat(command, " ");
            }
        }

        result = RTESys_OpenCommandOutputPipe(pContext, command );

        free(command);
    }
#else
    if ( !cmdArgv || !cmdArgv[0] )
    {
        return EINVAL;
    }

    result = UnixOpenCommandPipe(pContext, 0, cmdArgv);

#endif

    return result;
}

/*------------------------------------------------------*/

RTE_SystemRc RTESys_CloseCommandOutputPipe(RTESys_CommandOutputPipeContext *pContext, SAPDB_Int4 *pExitStatus)
{
    RTE_SystemRc result = RTE_SYSTEMRC_NO_ERROR;
#if defined(WIN32)
    DWORD terminationStatus;
    
    CloseHandle(pContext->cmdOutput);
    do
    {
        if ( !GetExitCodeProcess((HANDLE)pContext->childProcess, &terminationStatus) )
        {
            return GetLastError();
        }

        if ( STILL_ACTIVE == terminationStatus )
        {
            WaitForSingleObject((HANDLE)pContext->childProcess, INFINITE);
        }

    } while ( STILL_ACTIVE == terminationStatus );

    if ( pExitStatus )
    {
        *pExitStatus = (SAPDB_Int4)terminationStatus;
    }
#else
    int terminationStatus;
    SAPDB_Int4 childPid;

    RTE_save_close(pContext->cmdOutput);

    if ( pContext->cmdArea )
    {
        RTE_save_free(pContext->cmdArea);
    }

    do
    {   /* Wait for exit of this child */
        childPid = RTE_save_waitpid(pContext->childPid, &terminationStatus, 0);
        if ( -1 == childPid )
        {
            result = errno;
            break;
        }
        else if ( pExitStatus )
        {
            *pExitStatus = (SAPDB_Int4)terminationStatus;
        }
    } while ( childPid != pContext->childPid );
#endif
    return result;
}

/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/

#if !defined(WIN32)
/* only needed for UNIX, NT allows to create process from command line, UNIX needs arguments separation... */
static int BuildArgumentVector(const char *cmdline, char **pArgv, char *argArea)
{
    int ArgIndex;
    const char *arg;
    char *currentArg = argArea;

    ArgIndex = 0;
    arg   = cmdline;

    while ( *arg )
    {
        while ( *arg && (*arg == ' ' || *arg == '\t') )
        {
            ++arg;
        }
        pArgv[ArgIndex] = currentArg;
        while ( *arg && (*arg != ' ' && *arg != '\t') )
        {
            *currentArg++ = *arg++;
        }
        *currentArg++ = 0;
        ++ArgIndex;
    }
    pArgv[ArgIndex] = 0;
    return ArgIndex;
}

/*---------------------------------------------------------------------------------*/

static RTE_SystemRc UnixOpenCommandPipe( RTESys_CommandOutputPipeContext *pContext
                                   , char *argArea
                                   , char **cmdArgv )
{
    extern char **environ;
    RTE_FileHandle pipeDesc[2];
    RTE_SystemRc result = RTE_SYSTEMRC_NO_ERROR;

    pContext->cmdArea = argArea;

    if ( RTE_save_access(cmdArgv[0], X_OK) != 0 )
    {
        return errno;
    }

    if ( RTE_save_pipe(pipeDesc) != 0 )
    {
        return errno;
    }

    /* tried with RTE_save_vfork, but failed on HP and DEC... ffs... */
    pContext->childPid = (SAPDB_Int4)RTE_save_fork();
    switch(pContext->childPid)
    {
    case 0:
        RTE_save_close(pipeDesc[0]);
        RTE_save_dup2(pipeDesc[1],1);
        RTE_save_dup2(pipeDesc[1],2);
        RTE_save_execve(cmdArgv[0], cmdArgv, environ);
        _exit(45);
        break;

    case -1:
        result = errno;
        break;

    default:
        RTE_save_close(pipeDesc[1]);
        pContext->cmdOutput = pipeDesc[0];
        break;
    }

    return result;
}

#endif /* !WIN32 */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/