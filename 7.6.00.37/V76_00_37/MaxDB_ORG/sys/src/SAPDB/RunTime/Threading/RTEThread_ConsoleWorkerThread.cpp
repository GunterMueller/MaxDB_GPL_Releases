/*!========================================================================

  @file         RTEThread_ConsoleWorkerThread.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Console Worker Threads (Header File)

                The worker threads listen to the command queue and process
                the requests from Console

  @since        2005-01-10  14:03
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_CompilerFeatures.h"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Thread_Messages.hpp"
#include    "RunTime/RTE_ConsoleCommon.hpp"
#include    "RunTime/Threading/RTEThread_ConsoleWorkerThread.hpp"

extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTECont_DoublyLinkedList<RTESync_LockedScope,RTESync_Spinlock>   RTEThread_ConsoleWorkerThread::commandFreeList;


RTEThread_ConsoleWorkerThread* RTEThread_ConsoleWorkerThread::BeginThread( SAPDB_UInt4            stackSize,
                                                                           SAPDB_Int4             priority,
                                                                           SAPDB_UInt4            threadIndex,
                                                                           SAPDBErr_MessageList   &messageList )
{
    RTEThread_ConsoleWorkerThread*        pInstance;

    pInstance  = new (RTEMem_RteAllocator::Instance()) RTEThread_ConsoleWorkerThread( threadIndex );

    if ( 0 == pInstance )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_MALLOC_FAILED_FOR, 
                                            "RTEThread_ConsoleWorkerThread" );
        return 0;
    }

    ThrdRet ret = pInstance->Create( RTEThread_ConsoleWorkerThread::Normal, stackSize, priority,
                                     messageList );

    if ( RTEThread_ConsoleWorkerThread::NoError != ret )
    {
        pInstance->DestroyObject();
        return 0;
    }

    return pInstance;
}


void RTEThread_ConsoleWorkerThread::ExecuteNextCommand ( RTEThread_ConsoleCommand  &command )
{
    SAPDBErr_MessageList                    messageList;
    SAPDB_UInt4                             rc = RTE_CONS_NO_ERROR;

    SAPDBTRACE_ROUTINE_DEBUG("ExecuteConsoleCommand", Console_Trace, 1);

    switch ( command.ReqType() )
    {
    case  OPEN_REQ:

        rc = m_Connections.OpenConnection( command, m_ThreadIndex, messageList );

        if (( RTE_CONS_NO_ERROR != rc ) && ( RTE_CONS_TIMEOUT != rc ))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_CONNECT);
            RTE_Message (messageList);
        }

        SAPDBTRACE_WRITELN(Console_Trace, 5, "Worker thread " 
                            << m_ThreadIndex
                            << ", open request of client PID " 
                            <<  (SAPDB_UInt8)command.Data().connectData.pid
                            << (rc ? " succeeded" : " failed"));
        break;

    case CLOSE_REQ:
        rc = m_Connections.CloseConnection( command, m_ThreadIndex, messageList );

        if (( RTE_CONS_NO_ERROR != rc ) && ( RTE_CONS_TIMEOUT != rc ))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_RELEASE);
            RTE_Message (messageList);
        }

        SAPDBTRACE_WRITELN(Console_Trace, 5, "Worker thread " << m_ThreadIndex 
                                       << ", release request for connection handle " 
                                       <<  command.Data().hConnect
                                       << (rc ? " succeeded" : " failed"));
        break;

    default:
        rc = m_Connections.RequestProcessing( command, m_ThreadIndex, messageList );

        if (( RTE_CONS_NO_ERROR != rc ) && ( RTE_CONS_TIMEOUT != rc ))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_THREAD_CONS_RELEASE);
            RTE_Message (messageList);
        }

        SAPDBTRACE_WRITELN(Console_Trace, 5, "Worker thread " << m_ThreadIndex << ", data request: "
                                        << " Client Ref.: "       << command.SenderRef()
                                        << ", Connection slot.: " << command.ReceiverRef()
                                        << ", Conn. handle: "     << command.Data().hConnect);

        break;
    }

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4  RTEThread_ConsoleWorkerThread::KernelThreadMain()
{
    SAPDBTRACE_ROUTINE_DEBUG("ConsoleWorker", Console_Trace, 1);

    SAPDBTRACE_WRITELN(Console_Trace, 7, "Worker thread " << m_ThreadIndex );

    SAPDBErr_MessageList       messageList;
    RTEThread_ConsoleCommand   command;
    SAPDB_Bool                 found;

    for ( ; true ; )
    {
        do 
        {
           if ( false == ( found = GetNextCommandFromCommandQueue ( command ) ) ) // no new command?
           {
               ThrdRet ret = Suspend( messageList );

               if ( RTEThread_ConsoleWorkerThread::NoError != ret )
                   RTE_Crash( messageList );

           }
        }
        while ( false == found );

        SAPDBTRACE_WRITELN(Console_Trace, 7, "Worker thread " << m_ThreadIndex 
                                                              << ", state: new command received" );
        ExecuteNextCommand ( command );
    }

    return 0;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/