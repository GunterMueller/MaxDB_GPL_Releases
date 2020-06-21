/*!
  @file           RTEThread_Console.cpp
  @author         Stefan Paeck
  @special area   Kernel Console Thread
  @brief          Main
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

#if (defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)) || defined (SUN) || defined (SOLARIS)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if !defined (_WIN32)
#include <pthread.h>
#endif

#include    "RunTime/RTE_CompilerFeatures.h"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Thread_Messages.hpp"
#include    "RunTime/RTE_ConsoleCommon.hpp"
#include    "RunTime/Threading/RTEThread_ConsoleWorkerThread.hpp"
#include    "RunTime/Threading/RTEThread_ConsoleRequestor.hpp"
#include    "RunTime/RTE_ConsoleRequestCommunication.hpp"
#include    "RunTime/Threading/RTEThread_Console.h"
#include    "RunTime/RTE_KGS.hpp"
#include    "RunTime/RTE_KSS.h"
#include    "RunTime/RTE_ConsoleStandardDataRequests.h"
#include    "RunTime/RTE_ConsoleDataCommunication.hpp"

#if defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)             /*&if ($OSSPEC = LINUX && $__HAS_NPTL != 1) || $MACH = SUN || $OSSPEC == SOLARIS */
#include    "RunTime/RTE_ConsoleSemaphoreTimeoutList.hpp"   /* nocheck */
#endif                                                      /*&endif*/        

#include    "RunTime/RTE_ConsoleTraceTopic.hpp"

#if defined (_WIN32) && defined (_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif

#define NUM_CONSOLE_WORKER_THREADS     2
#define WORKER_COMMAND_THRESHOLD       2
#define WORKER_HEAVY_LOAD_THRESHOLD    3

extern SAPDBTrace_Topic Console_Trace; ///< global console trace object
     
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined (_WIN32) 
# define SET_THREAD_STATE(_state)    RTE_KGS::Instance().GetKGS()->Console.ThrdCtrlHeader.ThrdState = _state;
#else
# define SET_THREAD_STATE(_state)    RTE_KGS::Instance().GetKGS()->console.state = _state;
#endif


/*===========================================================================*
 *  DECLARATIONS                                                             *
 *===========================================================================*/


static  RTEThread_ConsoleWorkerThread*       consoleWorker[NUM_CONSOLE_WORKER_THREADS];

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


static  SAPDB_Bool  InitConsoleThread           (SAPDBErr_MessageList           &messageList);
static  SAPDB_Bool  PutCommandIntoCommandQueue  (RTEThread_ConsoleCommand       &command,
                                                 SAPDBErr_MessageList           &messageList);

static  void        ConsoleMain                 (void);
static  SAPDB_UInt4 numActThread = 0;

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/

static  SAPDB_Bool InitConsoleThread ( SAPDBErr_MessageList                  &messageList )
{
    SAPDBTRACE_ROUTINE_DEBUG("InitConsoleThread", Console_Trace, 1);

    for ( SAPDB_UInt4 threadIndex = 0; threadIndex < NUM_CONSOLE_WORKER_THREADS; threadIndex++ )
    {
        consoleWorker[threadIndex] = RTEThread_ConsoleWorkerThread::BeginThread( CONSOLE_WORKER_STACK_SIZE,
                                                                                 CONSOLE_WORKER_PRIO,
                                                                                 threadIndex,
                                                                                 messageList );

        if ( 0 != consoleWorker[threadIndex] )
            ++numActThread;

        SAPDBTRACE_WRITELN(Console_Trace, 1, "Worker thread started" << threadIndex  );
    }

    if ( 0 == numActThread )
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_WORKER_THREADS);
        return false;
    }

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
    if (!RTE_ConsoleSemaphoreTimeoutList::CreateInstance(NUM_CONSOLE_WORKER_THREADS, 
                                                         RTE_KGS::Instance().GetKGS()->consoleSemTimeout/1000,
                                                         messageList))
    {
        return false;
    }
#endif

    return true;
}

/*---------------------------------------------------------------------------*/

static  SAPDB_Bool PutCommandIntoCommandQueue ( RTEThread_ConsoleCommand       &command,
                                                SAPDBErr_MessageList           &messageList )
{
    SAPDBTRACE_ROUTINE_DEBUG("PutCommandIntoCommandQueue", Console_Trace, 1);

    SAPDB_UInt4 index = 0;

    if ( consoleWorker[index]->CommandsPending() >= WORKER_COMMAND_THRESHOLD )
    {
        // Find shortest command queue
        for ( SAPDB_UInt4 threadIndex = 1; threadIndex < numActThread; threadIndex++ )
        {
            if ( consoleWorker[threadIndex]->CommandsPending() < 
                 consoleWorker[threadIndex - 1]->CommandsPending() )
            {
                index = threadIndex;
            }
        }
    }

    if ( consoleWorker[0]->CommandsPending() >= WORKER_HEAVY_LOAD_THRESHOLD )
    {
        RTE_Message (SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_THREAD_CONS_WORKER_THRD_BUSY)); 
    }

    if ( !consoleWorker[index]->SendCommand( command, messageList ))
    {
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

externC void * RTEThread_Console ( void *)
{

#if defined (_WIN32) 
    __try 
#endif
    {
        ConsoleMain ();  
    }
#if defined (_WIN32) 
    __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}
#endif
    RTE_Message (SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_KERNEL_THREAD_STOPPED, "CONSOLE" ));
#if defined (_WIN32)
    EXITPROCESS(0);
#endif
    return NULL;
}

/*---------------------------------------------------------------------------*/


static void ConsoleMain ( void )

{
    RTEThread_ConsoleCommand            command;
    SAPDBErr_MessageList                messageList;
    SAPDB_UInt4                         rc;

//UNIX:
//	en81_Set_KT_TLS ( & KGS->console );


#if defined (_WIN32) 

#   if defined(_FASTCAP)
    CAPNameThread ("Console");
#   endif

    rc = SET_THREAD_PRIO (RTE_KGS::Instance().GetKGS()->Console.ThrdCtrlHeader.lThrdPrio,
                          RTE_KGS::Instance().GetKGS()->Console.ThrdCtrlHeader.Tid,
                          RTE_KGS::Instance().GetKGS()->Console.ThrdCtrlHeader.hThrd);


    if ( rc != NO_ERROR )
    {
        RTE_Crash (SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SETTING_THRD_PRIO, 
                                         SAPDB_ToString(RTE_KGS::Instance().GetKGS()->Console.ThrdCtrlHeader.Tid)));
    }


    if (RTE_KGS::Instance().GetKGS()->XParam->fUseMemEnhancement)
    {
        pkss->consoleShmSize = max (SAPDB_ALIGN (RTE_CONSOLE_SHM_SIZE_AWE, RTE_KGS::Instance().GetKGS()->ulSystemPageSize), 
                                    SAPDB_ALIGN (SAPDB_ALIGN (2 * sizeof (RTE_ConsoleSemaphore), SAPDB_ALIGNMENT)
                                                    + SAPDB_ALIGN (RTEComm_PacketHeader::PacketHeaderBufferSize(), SAPDB_ALIGNMENT)
                                                    + sizeof (CONSOLE_DATA_REC),
                                                 RTE_KGS::Instance().GetKGS()->ulSystemPageSize));
    }
    else
    {
        pkss->consoleShmSize = max (SAPDB_ALIGN (RTE_CONSOLE_SHM_SIZE, RTE_KGS::Instance().GetKGS()->ulSystemPageSize), 
                                    SAPDB_ALIGN (SAPDB_ALIGN (2 * sizeof (RTE_ConsoleSemaphore), SAPDB_ALIGNMENT)
                                                    + SAPDB_ALIGN (RTEComm_PacketHeader::PacketHeaderBufferSize(), SAPDB_ALIGNMENT)
                                                    + sizeof (CONSOLE_DATA_REC),
                                                 RTE_KGS::Instance().GetKGS()->ulSystemPageSize));
    }

    pkss->consoleSemTimeout = RTE_CONSOLE_TIMEOUT;
#else //due to missing system page size for UNIX we may lose some space in the allocated memory
	RTE_KGS::Instance().GetKGS()->consoleShmSize    = max (SAPDB_ALIGN (RTE_CONSOLE_SHM_SIZE, SAPDB_ALIGNMENT),
                                                           SAPDB_ALIGN (2 * sizeof (RTE_ConsoleSemaphore), SAPDB_ALIGNMENT)
                                                           + SAPDB_ALIGN (RTEComm_PacketHeader::PacketHeaderBufferSize(), SAPDB_ALIGNMENT) 
                                                           + SAPDB_ALIGN (sizeof (CONSOLE_DATA_REC), SAPDB_ALIGNMENT));
	RTE_KGS::Instance().GetKGS()->consoleSemTimeout = RTE_CONSOLE_TIMEOUT;
#endif

    if (!InitTrace ("console.trc", messageList))
    {
        RTE_Message (messageList);
		messageList.ClearMessageList ();
    }


    SAPDBTRACE_ROUTINE_DEBUG("RTEThread_Console", Console_Trace, 1);

    if (!InitConsoleThread (messageList))
    {
        RTE_Crash (messageList);
    }


    RTE_Message (SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_KERNEL_THREAD_STARTED, "CONSOLE" ));

#if defined (_WIN32) 
    RTE_KGS::Instance().GetKGS()->Console.ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;
#else
	RTE_KGS::Instance().GetKGS()->console.start_time = KGS->current_time;
#endif

    for (;;)
	{
        SET_THREAD_STATE(KT_SLEEPING);

        if (!RTEThread_ConsoleRequestor::Instance ().ReadCommand ( command, messageList))
        {
            RTE_Message (messageList);
		    messageList.ClearMessageList ();
            continue;
        }
        SET_THREAD_STATE(KT_RUNNING);

        if (!PutCommandIntoCommandQueue ( command, messageList ))
        {
            RTE_Message (messageList);
		    messageList.ClearMessageList ();
            continue;
        }
    }

 }

