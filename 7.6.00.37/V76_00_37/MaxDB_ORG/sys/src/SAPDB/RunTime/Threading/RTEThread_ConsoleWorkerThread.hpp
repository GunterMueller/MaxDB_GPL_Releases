/*!========================================================================

  @file         RTEThread_ConsoleWorkerThread.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Console Worker Threads (Header File)

                The worker threads listen to the command queue and process
                the requests from Console

  @since        2004-12-22  13:50
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


#ifndef RTETHREAD_CONSOLECOMMANDWORKER_H
#define RTETHREAD_CONSOLECOMMANDWORKER_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "RunTime/Container/RTECont_DoublyLinkedList.hpp"
#include "RunTime/Threading/RTEThread_ConsoleCommand.hpp"
#include "RunTime/Threading/RTEThread_KernelThread.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/Threading/RTEThread_ConsoleConnections.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define CONSOLE_WORKER_STACK_SIZE               (512*1024)
#define MAX_FREE_LIST_LEN                       15

#if defined (_WIN32)
# define CONSOLE_WORKER_PRIO                     THREAD_PRIORITY_NORMAL
#else
# define CONSOLE_WORKER_PRIO                     ((tsp00_Int4)1)   // Idle
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class   RTEThread_ConsoleWorkerThread
  @brief   Console worker thread class

           The worker threads listen to the command queue and process
           the requests from Console
 ----------------------------------------------------------------------------*/

class RTEThread_ConsoleWorkerThread : public RTEThread_KernelThread
{
public:
    /*!
    @description    Create Startup a new console worker thread
    @param          stackSize   [in] - Stack size in bytes.
    @param          threadIndex [in] - Index of the thread to be started
    @return value   Thread pointer, 0 if start failed
    */
    static RTEThread_ConsoleWorkerThread* BeginThread( SAPDB_UInt4            stackSize,
                                                       SAPDB_Int4             priority,
                                                       SAPDB_UInt4            threadIndex,
                                                       SAPDBErr_MessageList   &messageList );

    /*!
    @description    Get number of pending 

                    Returns the number of pendig command which have not been executed 

    @return value   Number of pendig commands
    */
    SAPDB_UInt4 CommandsPending()
    {
        return m_CommandQueue.ListLen();
    }

    /*!
    @description    Send command to console worker

                    Send a command to the console worker and wakeup the worker thread

    @param          command   [in] - command to be queued.

    @return value   true if no error
    */
    inline SAPDB_Bool SendCommand( RTEThread_ConsoleCommand                    &command,
                                   SAPDBErr_MessageList                        &messageList );

private:
    /*!
       @brief          Constructor
       @param          threadIndex [in] - Thread index used by error message output
     */
    RTEThread_ConsoleWorkerThread( SAPDB_UInt4      threadIndex ) 
                            : RTEThread_KernelThread("ConsoleWorkerThread"),
                              m_ThreadIndex(threadIndex),
                              m_Connections(RTEThread_ConsoleConnections::Instance()) {}

    /*!
    @description    The created thread runs the code of the methode 'ThreadMain'

                    Reads the command from the command queue and hand it
                    over to the request processing

    @return value   0 if no error 
    */
    virtual SAPDB_Int4  KernelThreadMain();


    /*!
    @description    Get next command

                    Reads the command from the command queue

    @param          command   [out] - command read from queue.

    @return value   true if command available
    */
    inline SAPDB_Bool GetNextCommandFromCommandQueue( RTEThread_ConsoleCommand &command );

    /*!
    @description    Execute next command

                    Execute the next command read from the command queue
    */
   void ExecuteNextCommand ( RTEThread_ConsoleCommand  &nextCommand );


  /*!
    @description    Destroy object, remove he instance from heap
    */
   virtual void  DestroyObject() { this->~RTEThread_ConsoleWorkerThread();
                                   RTEMem_RteAllocator::Instance().Deallocate(this); }

private:
    SAPDB_UInt4                                                             m_ThreadIndex;
    RTECont_DoublyLinkedList<RTESync_LockedScope,RTESync_Spinlock>          m_CommandQueue;
    RTEThread_ConsoleConnections&                                           m_Connections;

    static RTECont_DoublyLinkedList<RTESync_LockedScope,RTESync_Spinlock>   commandFreeList;
};

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool RTEThread_ConsoleWorkerThread::SendCommand( RTEThread_ConsoleCommand &command,
                                                              SAPDBErr_MessageList     &messageList )
{
    RTEThread_ConsoleCommand* pCommand;

    pCommand = reinterpret_cast<RTEThread_ConsoleCommand*>(commandFreeList.RemoveHead());

    if ( 0 == pCommand )
    {
        pCommand = new (RTEMem_RteAllocator::Instance()) RTEThread_ConsoleCommand();

        if ( 0 == pCommand )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_MALLOC_FAILED_FOR, 
                                                "RTEThread_ConsoleWorkerThread" );
            return false;
        }
    }

    *pCommand = command;

     m_CommandQueue.InsertTail(*pCommand);

    return Resume(messageList) == RTEThread_ConsoleWorkerThread::NoError;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool RTEThread_ConsoleWorkerThread::GetNextCommandFromCommandQueue( RTEThread_ConsoleCommand &command )
{
    RTEThread_ConsoleCommand* pCommand = reinterpret_cast<RTEThread_ConsoleCommand*>(m_CommandQueue.RemoveHead());

    if ( 0 == pCommand )
        return false;

    command = *pCommand;

    if ( commandFreeList.ListLen() < MAX_FREE_LIST_LEN )
        commandFreeList.InsertTail( *pCommand );
    else
        destroy (pCommand, RTEMem_RteAllocator::Instance());

    return true;
}

#endif  /* RTETHREAD_CONSOLECOMMANDWORKER_H */
