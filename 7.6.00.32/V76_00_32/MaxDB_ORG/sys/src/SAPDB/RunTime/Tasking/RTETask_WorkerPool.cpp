/*!========================================================================

  @file         RTETask_WorkerPool.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Pool of System Independend Task Worker Threads.

                System Independend Worker Threads For Time Consuming Or Blocking Task Work
                Needed i.e. during initialization of HotStandby 

  @since        2005-10-20  18:06
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
  \endif
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Tasking/RTETask_WorkerPool.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Threading/RTEThread_KernelThread.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"

/*! define KERNEL_LZU */
#define KERNEL_LZU
#include "geo50_0.h"
#include "heo56.h"
#include "heo07.h"



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if !defined(OSF1)
/*! Cast for OSF1 */
# define  LLE_CAST(_val) const_cast<RTETask_WorkerQueueEntry * const>(_val)
#else
/*! Dummy cast */
# define  LLE_CAST(_val) _val
#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class   RTETask_WorkerQueue;

/*!---------------------------------------------------------------------------
  @class   RTETask_WorkerQueueEntry
  @brief   Entry in worker queue.
 ----------------------------------------------------------------------------*/
class RTETask_WorkerQueueEntry
{
public:
    /*!
    @brief      Constructor
    @param      job [in] job to execute
    */
    RTETask_WorkerQueueEntry(RTETask_IWorkerJob &job) : m_Prev(0), m_Next(0), m_Job(job) {}


   /*!
    @brief      Get the job to be executed
    @param      none
    @return     The job
    */
    RTETask_IWorkerJob      &Job() const { return m_Job; }

private:
    friend class RTETask_WorkerQueue;
    class  RTETask_WorkerQueueEntry*  m_Next; ///< next entry
    class  RTETask_WorkerQueueEntry*  m_Prev; ///< previous entry
    RTETask_IWorkerJob&               m_Job;  ///< the job to do
};

/*!---------------------------------------------------------------------------
  @class   RTETask_WorkerQueue
  @brief   Lifo queue for the wóker jobs.
 ----------------------------------------------------------------------------*/

class RTETask_WorkerQueue
{
public:
    /*!
       @brief          Constructor
     */
    RTETask_WorkerQueue() : m_Tail(0), m_SingleConsumerList(0),
                           m_ElemCount(0), m_SingleConsumerElemCount(0) {}

    /*!
    @brief          Returns the actual element count of the list
    @return         Element count
    */
    SAPDB_UInt4 ListLen() const
    { 
        return m_ElemCount + m_SingleConsumerElemCount; 
    }

    /*!
       @brief          Inserts a new element at the end of the list
       @param          element [in]  - Element to be inserted
       @return         Inserted element;
     */
    inline RTETask_WorkerQueueEntry*  Insert ( RTETask_WorkerQueueEntry  &element );

    /*!
       @brief          Remove a element or all elements from the list
       @return         Pointer to the removed element, 
                       null if no element available
     */
    inline RTETask_WorkerQueueEntry* Remove();

private:
    RTETask_WorkerQueueEntry *   m_SingleConsumerList;       ///< Single consumer list pointer
    RTETask_WorkerQueueEntry *   m_Tail;                     ///< List pointer

    SAPDB_UInt4                  m_SingleConsumerElemCount;  ///< Actual element count in single consumer list
    SAPDB_UInt4                  m_ElemCount;                ///< Actual element count in the list
};


/*!---------------------------------------------------------------------------
  @class   RTETask_Worker
  @brief   Task worker thread.

  The worker thread is listening on its worker queue and does blocking calls
  instead of tasks.
 ----------------------------------------------------------------------------*/

class RTETask_Worker : public RTEThread_KernelThread
{
public:
    /*!
    @brief    Startup a new task worker

                    This call creates the worker thread, that is listening on its
                    worker queue and does blocking calls instead of tasks. This 
                    routine is called once during kernel initialization, where no 
                    task is running.

    @param          stackSize   [in] - Stack size in bytes.
    @param          messageList [out] - message list
    @return value   if no error an instance pointer else null
    */
    static RTETask_Worker*  BeginThread( SAPDB_UInt4            stackSize,
                                         SAPDBErr_MessageList   &messageList );

    /*!
    @brief    Enqueue worker job and wait for completion

                    This call enqueues the task in a waiting queue and calls 
                    the dispatcher to allow the next task in the task group 
                    to execute. Only if no more tasks are runnable, the task
                    will suspend itself. It will do so by internally calling 
                    a 'vstop()'. The worker thread will issue the corresponding
                    'vcontinue()'. If the worker thread is running, will find 
                    the job in the queue, otherwise it will be awoken while
                    sleeping on its internal semaphore.

                    @param job [in] the job to enqueue
    */
   void AddJob( RTETask_IWorkerJob &job );

    /*!
      @brief          Get number of pending commands
      @return         Number of pending commands
      */
    SAPDB_UInt4    CommandsPending() const  { return m_WorkerQueue.ListLen(); }

private:
    /*!
       @brief          Constructor
     */
    RTETask_Worker() 
         : RTEThread_KernelThread("TaskWorker"), 
           m_LoopForever(true) {}

    /*!
    @brief    The created thread runs the code of the methode 'ThreadMain'
    */
    virtual SAPDB_Int4  KernelThreadMain();

  /*!
    @brief    Destroy object
    */
   virtual void  DestroyObject() { this->~RTETask_Worker();
                                   RTEMem_RteAllocator::Instance().Deallocate(this); }

private:
    RTETask_WorkerQueue                     m_WorkerQueue;
    teo07_ThreadSemaphore                   m_QueueSemaphore;
    SAPDB_Bool                              m_LoopForever;
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class   RTETask_WorkerQueue
  @brief   Lifo queue for the wóker jobs.
 ----------------------------------------------------------------------------*/

inline RTETask_WorkerQueueEntry* RTETask_WorkerQueue::Remove ()
{
    RTETask_WorkerQueueEntry*  pElement;
    RTETask_WorkerQueueEntry*  pDummy;
    SAPDB_Int4         deltaElemCount;

    if ( 0 == m_SingleConsumerList )
    {
        do
        {
            pElement  = m_Tail;

            if ( 0 == pElement ) 
                return 0;
        } 
        while ( !RTESys_CompareAndExchange<RTETask_WorkerQueueEntry>( m_Tail,           // pointer to exchange
                                                              LLE_CAST(pElement),// expected element
                                                              0,                 // null 
                                                              pDummy ) );        // previous value

        RTESys_ReadMemoryBarrier();  // now we 've got a complete element chain

        deltaElemCount = 1;

        while ( pElement->m_Prev )
        {
            pElement->m_Prev->m_Next = pElement;
            pElement                 = pElement->m_Prev;
            pElement->m_Next->m_Prev = 0;
            ++deltaElemCount;
        }

        m_SingleConsumerList      = pElement->m_Next;
        pElement->m_Next          = 0;
        m_SingleConsumerElemCount = deltaElemCount - 1;
   }
    else
    {
        deltaElemCount = 0;

        pElement                  = m_SingleConsumerList;
        m_SingleConsumerList      = m_SingleConsumerList->m_Next;
        pElement->m_Next          = 0;
        --m_SingleConsumerElemCount;
    }

    RTESys_AtomicModify( m_ElemCount, -deltaElemCount );

    return pElement;
}

/*---------------------------------------------------------------------------*/

inline RTETask_WorkerQueueEntry* RTETask_WorkerQueue::Insert ( RTETask_WorkerQueueEntry  &element )
{
    RTETask_WorkerQueueEntry*  pDummy;

    do
    {
        element.m_Prev = m_Tail;
        RTESys_WriteMemoryBarrier();  // Needed for 'element.m_Prev' value
    } 
    while ( !RTESys_CompareAndExchange<RTETask_WorkerQueueEntry>( m_Tail,                     // pointer to exchange
                                                          LLE_CAST(element.m_Prev),   // expected element
                                                          LLE_CAST(&element),         // new element
                                                          pDummy ) );                 // previous value

    RTESys_AtomicModify( m_ElemCount, 1 );

    return &element;
}

/*!---------------------------------------------------------------------------
  @class   RTETask_Worker
  @brief   Task worker thread.

  The worker thread is listening on its worker queue and does blocking calls
  instead of tasks.
 ----------------------------------------------------------------------------*/

void RTETask_Worker::AddJob(RTETask_IWorkerJob &job)
{
    RTETask_WorkerQueueEntry newEntry(job);

    m_WorkerQueue.Insert ( newEntry );

    //----- signal new entry
    sqlsignalsem(m_QueueSemaphore);

    // stop until worker thread has done its job
    vstop(job.GetTaskId());
}


//-----------------------------------------------------------------
    
SAPDB_Int4  RTETask_Worker::KernelThreadMain()
{
    for ( ; m_LoopForever ; )
    {
        RTETask_WorkerQueueEntry* pQueueEntry;

        while ( 0 == ( pQueueEntry = m_WorkerQueue.Remove()) )
        {
            sqlwaitsem(m_QueueSemaphore);
        }

        pQueueEntry->Job().DoJob();

        vcontinue(pQueueEntry->Job().GetTaskId());

    }
    return 0;
}

//-----------------------------------------------------------------

RTETask_Worker* RTETask_Worker::BeginThread( SAPDB_UInt4            stackSize,
                                             SAPDBErr_MessageList   &messageList )
{
    tsp00_ErrTextc           errText;
    teo07_ThreadErr          errCode;
    RTETask_Worker*          pInstance;

    pInstance  = new (RTEMem_RteAllocator::Instance()) RTETask_Worker();

    if ( 0 == pInstance )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_MALLOC_FAILED_FOR, "RTETask_Worker" );
        return 0;
    }

    sqlcreatesem( &pInstance->m_QueueSemaphore, 0, errText, &errCode);

    if ( THR_OK_EO07 != errCode )
    {
        pInstance->DestroyObject();
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_ALLOCATION_FAILED, "worker thread semaphore");
        return 0;
    }

    if ( pInstance->Create( RTETask_Worker::Normal, stackSize,
                            messageList ) != RTETask_Worker::NoError )
    {
        pInstance->DestroyObject();
        return 0;
    }

    return pInstance;
}


/*!---------------------------------------------------------------------------
  @class   RTETask_WorkerPool
  @brief   pool of worker threads
 ----------------------------------------------------------------------------*/

RTETask_WorkerPool * RTETask_WorkerPool::m_pInstance = NULL;


RTETask_WorkerPool & RTETask_WorkerPool::Instance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTETask_WorkerPool));

  if ( m_pInstance == NULL )
    m_pInstance = new (Space) RTETask_WorkerPool();

  return *m_pInstance;
}

//-----------------------------------------------------------------

SAPDB_UInt4 RTETask_WorkerPool::CreateAllWorker ( SAPDB_UInt4            numOfWorker,
                                                  SAPDB_UInt4            workerStackSize,
                                                  SAPDB_UInt4            workerJobThreshold,
                                                  SAPDBErr_MessageList   &messageList )
{
    m_NumOfWorker        = numOfWorker;
    m_WorkerJobThreshold = workerJobThreshold;
    m_pWorkerPool        = (RTETask_Worker**)RTEMem_RteAllocator::Instance().Allocate(numOfWorker * sizeof(RTETask_Worker*));

    if ( 0 == m_pWorkerPool )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_MALLOC_FAILED_FOR, "TaskWorkerPool" );
        return false;
    }

    SAPDB_UInt4 worker;

    for ( worker = 0; worker < numOfWorker; worker++ )
    {
        m_pWorkerPool[worker] = RTETask_Worker::BeginThread( workerStackSize, messageList );

        if ( 0 == m_pWorkerPool[worker] )
        {
            m_NumOfWorker = worker;
            break;
        }
    }

    return worker;
}

//-----------------------------------------------------------------

void RTETask_WorkerPool::AddJob( RTETask_IWorkerJob &job )
{
    SAPDB_UInt4 worker = 0;

    if ( m_pWorkerPool[worker]->CommandsPending() >= m_WorkerJobThreshold )
    {
        // Find shortest command queue
        for ( SAPDB_UInt4 workerIdx = 1; workerIdx < m_NumOfWorker; workerIdx++ )
        {
            if ( m_pWorkerPool[workerIdx]->CommandsPending() < 
                 m_pWorkerPool[workerIdx-1]->CommandsPending() )
            {
                worker = workerIdx;

                if ( m_pWorkerPool[worker]->CommandsPending() < m_WorkerJobThreshold )
                   break;
            }
        }
    }

    m_pWorkerPool[worker]->AddJob(job);
}


//-----------------------------------------------------------------

//--------- to be removed if ven81.c and vos81k.c are rewritten in C++
extern "C" void RTETask_CWrapperForCreateWorkerThread( SAPDB_Int4 workerStackSize )
{
    SAPDBErr_MessageList   messageList;
    SAPDB_Int4             numOfWorker = 8;
    SAPDB_Int4             numOfWorkerStarted;

    numOfWorkerStarted = RTETask_WorkerPool::Instance().CreateAllWorker ( numOfWorker, workerStackSize, 
                                                                          2, messageList );

    if ( 0 == numOfWorkerStarted )
        RTE_Crash( messageList );
    else if ( numOfWorkerStarted < numOfWorker )
        RTE_Message ( messageList, MSG_DIAG_CONSOLE );
}

//--------------------------------------------------------

extern "C" void RTETask_CallWorker( RTETask_IWorkerJob &job )
{
    RTETask_WorkerPool::Instance().AddJob(job);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/