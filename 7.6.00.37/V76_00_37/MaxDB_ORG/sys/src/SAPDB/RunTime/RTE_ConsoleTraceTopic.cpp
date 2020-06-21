/*!
  @file           RTE_ConsoleTraceTopic.cpp
  @author         StefanP
  @special area   Tracing
  @brief          Tracing Support for XCons and Console Thread
  @see            

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "gsp01.h"
#include    "heo01.h"
//#include    "heo12.h"

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/ 
#include    "gos44.h"   /* nocheck */
#else                   /*&else*/
#include    "hen40.h"   /* nocheck */
#endif                  /*&endif*/


#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Messages.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "SAPDBCommon/SAPDB_sprintf.h"
#include    "RunTime/RTE_ConsoleTraceTopic.hpp"
#include    "RunTime/System/RTESys_Time.h"
#include    "RunTime/System/RTESys_IO.h"
#include    "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"




/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define RTE_CONSOLE_TRACE_SEM_NAME	        "SAP DBTech-CONSOLE-TRACE-SEM"
#define	RTE_CONSOLE_TRACE_MAX_HEADER_LEN    80
#define	TRACE_MUTEX_TIMEOUT                 5000

/*! global console trace object */
SAPDBTrace_Topic Console_Trace              ("CONSOLE", "Topic trace for console");

static	ConsoleTracePrinter   consoleTracePrinter; 
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


bool ConsoleTracePrinter::Initialize
(
    SAPDB_Char const * const    nameFile,
	SAPDBErr_MessageList &      messageList
)
{
    RTE_Path        traceFileLogPath;

    sqlcreatemutex(&m_ConsoleTraceMutex);
    strcpy (traceFileLogPath, (char *) nameFile);
#if defined (_WIN32) 
    // --- substitute logical path name parts
    sql44c_subst_log_parts ( m_TraceFilePath, traceFileLogPath );
#else
    strcpy (m_TraceFilePath, traceFileLogPath);
#endif


   return true;
}

/*---------------------------------------------------------------------------*/

void ConsoleTracePrinter::PrintLn
(
    const SAPDB_Char * buf, 
    int buflen
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/

    RTE_FileOffset          writtenBytes;
    RTE_FileHandle          handle = RTE_UNDEF_HANDLE;
    RTE_FileOffset          newPosition;
	RTESys_Timestamp	    timeBuffer;
    SAPDB_Byte *            pTraceBuffer = NULL;
	RTE_FileOffset          traceLen;
    tsp00_VfReturn_Param    rc;
    SAPDB_Int4              pid;

    
   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/

    sqlbeginmutex(&m_ConsoleTraceMutex);

    RTESys_IOOpen (handle,
        m_TraceFilePath,
        RTESys_IOReadWrite,
#if defined (_WIN32) 
        false,
#else
        true,
#endif
        NULL,
        rc);

#if defined (_WIN32)
    if (vf_notok == rc)
    {
        RTE_SystemRc        retCode;
        if (ERROR_FILE_NOT_FOUND == (retCode = GetLastError ()))
        {
            RTESys_IOOpen (handle,
                           m_TraceFilePath,
                           RTESys_IOReadWrite,
                           true,
                           NULL,
                           rc);
        }
    }
#endif

    if (vf_notok == rc)
    {
        RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_TRACE_OPEN_FILE,
#if defined (_WIN32) 
                     SAPDB_ToString (GetLastError ())));
#else
                     sqlerrs ()));
#endif
    }
    else
    {
        RTESys_IOSeek (handle,
            0,
            RTESys_IOSeekEnd,
            newPosition,
            rc);


        if (vf_noseek == rc)
        {
            RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_TRACE_SET_FILE_POINTER,
#if defined (_WIN32) 
                         SAPDB_ToString (GetLastError ())));
#else
                         sqlerrs ()));
#endif

        }
        else
        {
            pTraceBuffer = (SAPDB_Byte *)(RTEMem_RteAllocator::Instance().Allocate 
                (RTE_CONSOLE_TRACE_MAX_HEADER_LEN + buflen + 1));
            if (NULL == pTraceBuffer)
            {
                RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_ALLOC_TRACE_BUFFER, 
                             SAPDB_ToString (RTE_CONSOLE_TRACE_MAX_HEADER_LEN + buflen + 1)));       
            }
            else
            {
#if defined (_WIN32) 
                pid = GetCurrentProcessId();
#else
                pid = (SAPDB_Int4)getpid();
#endif

                SAPDB_sprintf ((char *)pTraceBuffer, RTE_CONSOLE_TRACE_MAX_HEADER_LEN, 
                    "Timestamp %s, Process ID %u\n", 
                    RTESys_FillProtocolTimeStamp(timeBuffer), pid);

                traceLen = strlen ((char *)pTraceBuffer);
                SAPDB_MemCopyNoCheck (pTraceBuffer + traceLen, buf, buflen);
                traceLen += buflen;
                memset (pTraceBuffer + traceLen, '\n', 1);
                ++traceLen;


                RTESys_IOWrite (handle,
                    pTraceBuffer,
                    traceLen,
                    writtenBytes,
                    rc);
                if (vf_notok == rc)
                {
                    RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_TRACE_WRITE_LEN,
                        SAPDB_ToString ((SAPDB_Int8)traceLen), 
                        SAPDB_ToString ((SAPDB_Int8)writtenBytes)));
                }
            }
        }
    }

    if (RTE_UNDEF_HANDLE != handle)
    {
        RTESys_IOClose (handle, rc);
        if (vf_notok == rc)
        {
            RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_TRACE_CLOSE_FILE,
#if defined (_WIN32) 
                     SAPDB_ToString (GetLastError ())));
#else
                     sqlerrs ()));
#endif
        }
    }

    if (NULL != pTraceBuffer)
    {
        RTEMem_RteAllocator::Instance().Deallocate(pTraceBuffer);
    }

    sqlendmutex(&m_ConsoleTraceMutex);
    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool      InitTrace 
(
    SAPDB_Char const * const            nameFile,
    SAPDBErr_MessageList &				messageList
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/

   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/

    if (!consoleTracePrinter.Initialize (nameFile, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_INIT_TRACE_PRINTER);
		return false;
    }
	else
	{
		Console_Trace.SetTracePrinter (&consoleTracePrinter);
        //Since we use tracing only in slow mode it does not matter that we set the highest level
		Console_Trace.SetTraceLevel (9);
	}

    return true;
}

