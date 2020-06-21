/*!
  @file     RTE_ConsoleSemaphoreTimeoutList.cpp
  @author   StefanP,Raymond
  @ingroup  Console
  @brief    Timeout support for Semaphores between different Processes (System Scope).
            This Module provides a Class for a List of Semaphores to be observed for a Timeout.         

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
#if !defined (_WIN32)

#include    "gen00.h"
#include    "RunTime/System/RTESys_Time.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Crash.hpp"
#include    "RunTime/RTE_Console_Messages.hpp"
#include    "RunTime/RTE_ConsoleDataCommunication.hpp"
#include    "RunTime/RTE_ConsoleSemaphoreTimeoutList.hpp"
#include    "RunTime/RTE_saveUNIXcalls.h"
#include    "RunTime/Threading/RTEThread_KernelThread.hpp"
#include    "RunTime/Threading/RTEThread_Thread.hpp"


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

class RTE_SemaphoreTimeoutThread : 
#if defined(KERNEL_LZU)
    public RTEThread_KernelThread
#else
    public RTEThread_Thread
#endif

{
public:
    /*!
    @description    Startup a new semaphore timout thread
    @param          stackSize   [in] - Stack size in bytes.
    @param          maxSleeptime[in] - The maximum sleep time until the next semaphore check occurs
    @return value   true:   Thread could be started
                    false:  Start failed
    */
    static SAPDB_Bool BeginThread( SAPDB_UInt4              stackSize,
                                   SAPDB_UInt8              maxSleeptime,
                                   SAPDBErr_MessageList     &messageList );
private:
    /*!
       @brief          Constructor
     */
    RTE_SemaphoreTimeoutThread( SAPDB_UInt8   maxSleeptime ) 
#if defined(KERNEL_LZU)
                              : RTEThread_KernelThread("RTE_SemaphoreTimeoutThread"),
#else
                              : RTEThread_Thread("RTE_SemaphoreTimeoutThread"),
#endif
                                m_MaxSleeptime(maxSleeptime) {}

    /*!
    @description    The created thread runs the code of the methode 'ThreadMain'
    */
#if defined(KERNEL_LZU)
    virtual SAPDB_Int4  KernelThreadMain() 
    {
        return SemaphoreTimeoutThreadMain(); 
    }
#else
    virtual SAPDB_Int4 ThreadMain()
    {
        return SemaphoreTimeoutThreadMain(); 
    }
#endif

    /*!
    @description    The created thread runs the code of the methode 'ThreadMain'
    */
    virtual SAPDB_Int4  SemaphoreTimeoutThreadMain();

  /*!
    @description    Destroy object
    */
   virtual void  DestroyObject() { this->~RTE_SemaphoreTimeoutThread();
                                   RTEMem_RteAllocator::Instance().Deallocate(this); }


private:
    SAPDB_UInt8         m_MaxSleeptime;
};

/*---------------------------------------------------------------------------*/

struct TimeoutListElement
{
    RTE_ConsoleSemaphore *  pSem;
    SAPDB_UInt8             timeWait;
    SAPDB_UInt8             timeoutValue;
    SAPDB_Bool              timedOut; 
    SAPDB_Bool              finalWait;
};



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/
RTE_ConsoleSemaphoreTimeoutList   *RTE_ConsoleSemaphoreTimeoutList::m_Instance = NULL;


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


SAPDB_Bool  RTE_ConsoleSemaphoreTimeoutList::PostTimedOutSemaphores
(
    SAPDB_UInt8 &                               nextSleeptime,
    SAPDBErr_MessageList &                      messageList 
)  
{
    SAPDB_UInt4                 idx;
    SAPDB_UInt8                 exhaustedTime; 
    SAPDB_UInt8                 timeoutValue;

    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::PostTimedOutSemaphores", 
                             Console_Trace, 1);
    
    for (idx = 0; idx < m_MaxSemaphores; ++idx)
    {
        Lock ();

        if (NULL != m_pTimeoutList[idx].pSem)
        {
            exhaustedTime = RTESys_Time () - m_pTimeoutList[idx].timeWait;
            if (exhaustedTime >= m_pTimeoutList[idx].timeoutValue)
            {
                SAPDB_Bool      post = false;

                while (RTESys_TestAndLock(&m_pTimeoutList[idx].pSem->lock))
                {
                    ;
                }

                if (RTE_CONSOLE_SEM_WAIT == m_pTimeoutList[idx].pSem->state)
                {
                    m_pTimeoutList[idx].timedOut = true;
                    post = true;
                    if (m_pTimeoutList[idx].finalWait)
                    {
                        m_pTimeoutList[idx].pSem->state = RTE_CONSOLE_SEM_TIMEOUT;
                    }
                }

                RTESys_Unlock (&m_pTimeoutList[idx].pSem->lock);

                if (post)
                {
                    union semun     arg;
                    SAPDB_Int       rc;

                    arg.val = 1 ;

                    rc = RTE_save_semctl (m_pTimeoutList[idx].pSem->handle, 0 , SETVAL , SEMCTL_SETVAL(arg));
                    if ( rc == -1 )
                    {
                        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_POST_SEM_UNIX, 
                            SAPDB_ToString (m_pTimeoutList[idx].pSem->handle), 
                            RTE_ConsoleUNIXError (errno));
                        Unlock ();
                        return false;
                    }
                }
                SAPDBTRACE_WRITELN(Console_Trace, 7, "Timeout for semaphore " << m_pTimeoutList[idx].pSem->handle 
                                                                              << " on index " 
                                                                              << idx
                                                                              << " occurred");
            }
            else
            {
                nextSleeptime = min (nextSleeptime, m_pTimeoutList[idx].timeoutValue - exhaustedTime);
            }
        }

        Unlock ();
    }

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTE_ConsoleSemaphoreTimeoutList::CreateInstance 
(
    SAPDB_UInt4 const          maxSemaphores,
    SAPDB_UInt8                maxSleeptime,
    SAPDBErr_MessageList &     messageList
)
{
    SAPDB_UInt4     idx;
    SAPDBTRACE_ROUTINE_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::CreateInstance", 
                              Console_Trace, 1);

    if (NULL == m_Instance)
    {
        m_Instance = new (RTEMem_RteAllocator::Instance()) RTE_ConsoleSemaphoreTimeoutList(maxSemaphores);

        if (NULL == m_Instance)
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
                                                             "RTE_ConsoleSemaphoreTimeoutList");
            return SAPDB_FALSE;
        }

        if (! m_Instance->Initialize (messageList)) 
        {
            m_Instance->~RTE_ConsoleSemaphoreTimeoutList();
            RTEMem_RteAllocator::Instance().Deallocate(m_Instance);
            m_Instance = 0;
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_INIT_SEMAPHORE_TIMEOUT);
            return SAPDB_FALSE;
        }

        if ( !RTE_SemaphoreTimeoutThread::BeginThread( CONSOLE_SEMAPHORE_TIMEOUT_WORKER_STACK_SIZE, 
                                                       maxSleeptime, messageList ) )
        {
            m_Instance->~RTE_ConsoleSemaphoreTimeoutList();
            RTEMem_RteAllocator::Instance().Deallocate(m_Instance);
            m_Instance = 0;
            return SAPDB_FALSE;
        }
    }
 
    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool      RTE_ConsoleSemaphoreTimeoutList::Initialize              
(
    SAPDBErr_MessageList &     messageList
)
{
    SAPDB_UInt4     idx;

    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::Initialize", 
                              Console_Trace, 1);

    newarray (m_pTimeoutList, m_MaxSemaphores, RTEMem_RteAllocator::Instance());
    if (NULL == m_pTimeoutList)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYS_ALLOC_ARRAY_INST, 
                                                          SAPDB_ToString (m_MaxSemaphores),
                                                          "RTE_ConsoleSemaphoreTimeoutList");
        return SAPDB_FALSE;
    }

    for (idx = 0; idx < m_MaxSemaphores; ++idx)
    {
        m_pTimeoutList[idx].pSem = NULL;
        m_pTimeoutList[idx].timedOut = false;
        m_pTimeoutList[idx].finalWait = false;
    }
 
    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

RTE_ConsoleSemaphoreTimeoutList::~RTE_ConsoleSemaphoreTimeoutList()
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::~RTE_ConsoleSemaphoreTimeoutList", 
                             Console_Trace, 9);
    if (NULL != m_pTimeoutList)
    {
        destroyarray (m_pTimeoutList, m_MaxSemaphores, RTEMem_RteAllocator::Instance());       
    }

    sqldestroymutex(&m_TimeoutListMutex);
}


/*---------------------------------------------------------------------------*/

SAPDB_Bool      RTE_ConsoleSemaphoreTimeoutList::EnqueueSemaphore            
(
    RTE_ConsoleSemaphore * const            pSemaphore,
    SAPDB_UInt8 const                       timeout,
    SAPDB_UInt4 const                       index,
    SAPDB_Bool const                        finalWait,
    SAPDBErr_MessageList &                  messageList
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::EnqueueSemaphore", 
                              Console_Trace, 1);

    
    if (index >= m_MaxSemaphores)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_NUM_SEMAPHORE_TIMEOUT, 
                                                          SAPDB_ToString (m_MaxSemaphores));
        return SAPDB_FALSE;
    }


    m_pTimeoutList[index].timeWait = RTESys_Time ();
    m_pTimeoutList[index].timeoutValue = timeout;
    m_pTimeoutList[index].timedOut  = false;
    m_pTimeoutList[index].finalWait = finalWait;
    //Has to be written at last:
    m_pTimeoutList[index].pSem       = pSemaphore;
    SAPDBTRACE_WRITELN(Console_Trace, 7, "Enqueue semaphore " << pSemaphore->handle 
                                                              << " on index " 
                                                              << index
                                                              << " with timeout value "
                                                              << timeout);
    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool      RTE_ConsoleSemaphoreTimeoutList::DequeueSemaphore            
(
    SAPDB_UInt4 const                       index,
    SAPDB_Bool &                            timedOut,
    SAPDBErr_MessageList &                  messageList
)
{
    SAPDB_UInt4     idx;
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::DequeueSemaphore", 
                              Console_Trace, 1);

    Lock ();

    timedOut = m_pTimeoutList[index].timedOut;
    SAPDBTRACE_WRITELN(Console_Trace, 7, "Dequeue semaphore " << m_pTimeoutList[index].pSem->handle 
                                                              << " on index " 
                                                              << index);

    m_pTimeoutList[index].pSem = NULL;
    Unlock ();


    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

    
SAPDB_Int4  RTE_SemaphoreTimeoutThread::SemaphoreTimeoutThreadMain()
{
    SAPDBErr_MessageList           messageList;
    SAPDB_UInt8                    sleeptime = m_MaxSleeptime;

    SAPDBTRACE_ROUTINE_DEBUG ("SemaphoreTimeoutThreadMain", Console_Trace, 1);

    for (;;)
    {

        SAPDBTRACE_WRITELN(Console_Trace, 9, "SemaphoreTimeoutThread sleeps for " << sleeptime << " seconds");
        sleep (min (m_MaxSleeptime, sleeptime));
        //new initialization of sleeptime
        sleeptime  = m_MaxSleeptime;
        if (!RTE_ConsoleSemaphoreTimeoutList::GetInstance().PostTimedOutSemaphores (sleeptime, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_POST_SEMAPHORE_TIMEOUT);
            RTE_Message (messageList);
        }
    }
    return 0;
}

//-----------------------------------------------------------------

SAPDB_Bool RTE_SemaphoreTimeoutThread::BeginThread( SAPDB_UInt4               stackSize,
                                                    SAPDB_UInt8               maxSleeptime,
                                                    SAPDBErr_MessageList     &messageList )
{
    RTE_SemaphoreTimeoutThread*    pInstance;

    pInstance  = new (RTEMem_RteAllocator::Instance()) RTE_SemaphoreTimeoutThread( maxSleeptime );

    if ( 0 == pInstance )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_MALLOC_FAILED_FOR, "RTE_SemaphoreTimeoutThread" );
        return false;
    }

    if ( pInstance->Create( RTE_SemaphoreTimeoutThread::Normal, stackSize,
                            CONSOLE_SEMAPHORE_TIMEOUT_WORKER_PRIO,
                            messageList ) != RTE_SemaphoreTimeoutThread::NoError )
    {
        pInstance->DestroyObject();
        return false;
    }

    pInstance->Forget();

    return true;
}
#endif
