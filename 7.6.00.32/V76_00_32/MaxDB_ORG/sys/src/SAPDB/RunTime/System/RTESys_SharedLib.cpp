/*!---------------------------------------------------------------------
  @file           RTESys_SharedLib.cpp
  @author         JoergM
  @brief          System: Base class for loading DLL during runtime
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#include "RunTime/System/RTESys_SharedLib.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

#if defined AIX
#  include <errno.h> 
#  include <sys/ldr.h> 
#  include "RunTime/System/RTESys_IO.h" /* nocheck */
#  include "RunTime/System/RTESys_CommandPipe.h" /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MAX_ERROR_TEXT_SIZE 4096

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*
  Since dlerror() on AIX is not working as wanted the detailed error information
  is collected using 'loadquery()' + execerror. The latter is a pretty print program
  for the cryptic strings returned by loadquery().
 */
#ifdef AIX
static void AppendLoadQueryInformation( SAPDB_Char const *sharedLibName,
                                        SAPDBErr_MessageList &errList )
{
    int iMsg;
    char **cmdArgv = 0;
    SAPDB_Int4 bufferSize = 8192;

    /* try again until either buffer exceeds mem available or loadquery is satisfied...*/
    do
    {
        cmdArgv = (char **)malloc(bufferSize);
        if ( cmdArgv )
        {
            cmdArgv[0] = (char *)"/usr/sbin/execerror";
            cmdArgv[1] = (char *)sharedLibName;
            iMsg = loadquery( L_GETMESSAGES
                            , &cmdArgv[2]
                            , bufferSize - ( 2 * sizeof(char *) ) );
            if ( (-1 == iMsg) && (ENOMEM == errno) )
            {
                free(cmdArgv);
                bufferSize *= 2;
                errno = ENOMEM;
            }
        }
        else
        {
            errList.AppendNewMessage( SAPDBErr_MessageList( RTE_CONTEXT
                                                          , RTEERR_SYS_LOADQUERY
                                                          , "cant allocate loadquery buffer" ) );
            return;
        }
    } while ( (-1 == iMsg) && (ENOMEM == errno) );

    /* if loadquery got a severe problem give up */
    if ( iMsg == -1 )
    {
        free(cmdArgv);
        errList.AppendNewMessage( SAPDBErr_MessageList( RTE_CONTEXT
                                                      , RTEERR_SYS_LOADQUERY
                                                      , "error from loadquery call" ) );
        return;
    }

    /* now use 'execerror' to get the pretty print */
    RTESys_CommandOutputPipeContext cmdContext;

    if ( RTESys_OpenCommandVectorOutputPipe( cmdContext, &cmdArgv[0]) != RTE_SYSTEMRC_NO_ERROR )
    {
        free(cmdArgv);
        errList.AppendNewMessage( SAPDBErr_MessageList( RTE_CONTEXT
                                                      , RTEERR_SYS_LOADQUERY
                                                      , "error calling /usr/sbin/execerror" ) );
        return;
    }

    /* pipe waits to be emptied... */
    RTE_SystemRc rc;
    SAPDB_Int4 exitStatus;
#define MESSAGE_BUFFER_LIMIT 8191
    SAPDB_Char *MessageBuffer = (SAPDB_Char *)malloc(MESSAGE_BUFFER_LIMIT + 1);
    SAPDB_Char * nextInput;
    tsp00_VfReturn_Param returnValue;
    RTE_FileOffset bytesReadIntoBuffer;
    RTE_FileOffset remainingBufferSpace;
    
    SAPDB_MemFillNoCheck(&MessageBuffer[0], 0, MESSAGE_BUFFER_LIMIT+1);
    bytesReadIntoBuffer = 0;

    /* do not exceed MESSAGE_BUFFER_LIMIT on input... */
    do
    {
        RTE_FileOffset bytesReadFromPipe;

        remainingBufferSpace = MESSAGE_BUFFER_LIMIT - bytesReadIntoBuffer;
        nextInput = &MessageBuffer[bytesReadIntoBuffer];
        bytesReadFromPipe = 0;
        RTESys_IORead( cmdContext.cmdOutput
                     , nextInput
                     , remainingBufferSpace
                     , bytesReadFromPipe
                     , returnValue);
        if ( vf_ok == returnValue && bytesReadFromPipe > 0 )
        {
            SAPDB_Char *gotNewLine;

            bytesReadIntoBuffer += bytesReadFromPipe;
            gotNewLine = strchr(&MessageBuffer[0], '\n');
            if ( gotNewLine )
            {
                while ( gotNewLine )
                {
                    *gotNewLine = 0;
                    errList.AppendNewMessage( SAPDBErr_MessageList( RTE_CONTEXT
                                                                  , RTEERR_SYS_LOADQUERY
                                                                  , &MessageBuffer[0] ) );
                    bytesReadIntoBuffer = (&MessageBuffer[bytesReadIntoBuffer] - (gotNewLine+1));
                    SAPDB_MemMoveNoCheck(&MessageBuffer[0]
                                       , gotNewLine + 1
                                       , bytesReadIntoBuffer);
                    SAPDB_MemFillNoCheck(&MessageBuffer[bytesReadIntoBuffer]
                         , 0
                         , MESSAGE_BUFFER_LIMIT + 1 - bytesReadIntoBuffer);
                    gotNewLine = strchr(&MessageBuffer[0], '\n');
                }
            }
            else if ( bytesReadIntoBuffer == MESSAGE_BUFFER_LIMIT )
            {
                errList.AppendNewMessage( SAPDBErr_MessageList( RTE_CONTEXT
                                                              , RTEERR_SYS_LOADQUERY
                                                              , &MessageBuffer[0] ) );
                SAPDB_MemFillNoCheck(&MessageBuffer[0]
                     , 0
                     , MESSAGE_BUFFER_LIMIT+1);
                bytesReadIntoBuffer = 0;
            }
        }
    } while ( vf_ok == returnValue );

    /* finally done. Release buffer and close pipe. */
    free(MessageBuffer);
    free(cmdArgv);

    rc = RTESys_CloseCommandOutputPipe(cmdContext, exitStatus);
    if ( RTE_SYSTEMRC_NO_ERROR != rc )
    {
        errList.AppendNewMessage( SAPDBErr_MessageList( RTE_CONTEXT
                                                      , RTEERR_SYS_LOAD_DLL
                                                      , "closing command output pipe" ) );
        return;
    }
}
#endif /* AIX */

SAPDB_Bool RTESys_SharedLib::LoadSharedLib( 
  SAPDB_Char const *sharedLibName,
  RTE_LoadMode loadMode,
  SAPDBErr_MessageList &errList)
{
    SAPDB_Bool callResult;
    SAPDB_Char errorText[MAX_ERROR_TEXT_SIZE];

    callResult = RTESys_LoadSharedLibrary( sharedLibName, 
                                           loadMode,
                                           &errorText[0],
                                           (SAPDB_Int4)sizeof(errorText),
                                           m_FullSharedLibName,
                                           m_LibHandle);

    if ( false == callResult )
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_LOAD_SHARED_LIB, sharedLibName, &errorText[0]);
#ifdef AIX
        AppendLoadQueryInformation(sharedLibName, errList);
#endif
    }

    return callResult;
}

/*---------------------------------*/

SAPDB_Bool RTESys_SharedLib::UnloadSharedLib(SAPDBErr_MessageList *pErrList)
{
    if ( 0 == m_LibHandle )
    {
        if ( 0 != pErrList )
        {
           *pErrList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_SYS_CALL_DLL_FUNC, "Unload: library never loaded");
        }
        return false;
    }

    SAPDB_Bool callResult;
    SAPDB_Char errorText[MAX_ERROR_TEXT_SIZE];

    callResult = RTESys_UnloadSharedLibrary( m_LibHandle, 
                                             &errorText[0],
                                             (SAPDB_Int4)sizeof(errorText) );

    if ( false == callResult )
    {
        if ( 0 != pErrList )
        {
            *pErrList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_UNLOAD_SHARED_LIB, m_FullSharedLibName, &errorText[0]);
        }
    }
    else
    {
        m_LibHandle = 0;
    }

    return callResult;
}

/*---------------------------------*/

SAPDB_Bool RTESys_SharedLib::GetProcAddress( 
  SAPDB_Char const * symbolicName,
  RTE_SharedLibProcAddress & procAddress,
  SAPDBErr_MessageList & errList)
{
    if ( 0 == m_LibHandle )
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_SYS_CALL_DLL_FUNC, "GetProcAddress: library never loaded");
        return false;
    }

    SAPDB_Bool callResult;
    SAPDB_Char errorText[MAX_ERROR_TEXT_SIZE];

    callResult = RTESys_GetProcAddressFromSharedLibrary( m_LibHandle,
                                                         symbolicName,
                                                         &errorText[0],
                                                         (SAPDB_Int4)sizeof(errorText),
                                                         procAddress );

    if ( false == callResult )
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_GET_PROC_ADDRESS, symbolicName, m_FullSharedLibName, &errorText[0]);
    }

    return callResult;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
