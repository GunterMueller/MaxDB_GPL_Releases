/*!
  @file           RTESync_RWRegion.cpp
  @author         RobinW
  @ingroup        Runtime
  @brief          critical regions that support exclusive ("write") and
                  non-exclusive ("read") access - implementation of the
                  RTESync_IRWRegion interface


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

#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_CallDebugger.h"
#include "RunTime/System/RTESys_Spinlock.h"
#include "SAPDB/SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"

#define KERNEL_LZU
#include "RunTime/Tasking/RTETask_Task.hpp"

#define USE_KGS
#include "geo50_0.h"    // for things like KGS, TASK_CTRL...
#include "geo002.h"     // for XPARAM()

extern SAPDBTrace_Topic RWRegion_Trace; ///< global RWRregion trace object

#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"


#ifdef _WIN32
static void RTESync_RWRegionLeaveRemoteFunction ( PPTASK_CTRL_REC ppCTask,
                              PTASK_CTRL_REC  pFromTask,
                              PPDLQ_REC       ppCurrReq,
                              void*           pDummyParam )
{
    if((*ppCTask)->pTaskObject->PrepareEnqueueOfInterruptibleRequest())
    {
        DLQ_ELEMENT* pSpecDLQElem1 = GetTaskDLQElement1( *ppCTask );

        *ppCurrReq               = pSpecDLQElem1;
        pSpecDLQElem1->ulReqType = RTETask_CommonQueueElement::LeaveRWRegion;
    }
    else
    {
        *ppCTask = NULL;
    }
    return;
}
#else
extern "C" void RTESync_RWRegionLeaveRemoteFunction (   void *param ,
                                                    struct TASK_TYPE *FromTask)
{
    ten50_UKT_Control               * this_ukt = THIS_UKT_CTRL;
    if( this_ukt->curr_task->pTaskObject->PrepareEnqueueOfInterruptibleRequest() )
    {

        struct DOUBLY_LINKED*   request = GetTaskDLQElement1( this_ukt->curr_task );
        request->req_type   = RTETask_CommonQueueElement::LeaveRWRegion;
        request->fromtaskid = FromTask;
    }
    else
    {
        this_ukt->curr_task = 0;
    }
}

#endif
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define DEBUG_BREAK

#ifdef DEBUG_BREAK
#define DoDebugBreak RTE_DebugBreak
#else
#define DoDebugBreak()
#endif  // DEBUG_BREAK

#define USE_SEMALIST    RTESync_IRWRegion::WithSemalist

#define MAX_TAS_COUNT SAPDB_MAX_UINT8   // currently the biggest possible number, may be adjusted if necessary

/* the supporters thougt they were interested in a list of waiting tasks in the systemview READERWRITERLOCKSTATISTICS */
/* The implementation was started but not completely finished when they decided they do not really need it.           */
//#define ADD_WAITERS_
/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class RTESync_RWRegion ;
class RTESync_WaitQueueForRWRegion;

SAPDB_Bool RTESync_RWRegionRegister::GetStatisticInfo(StatisticInfo * &pInfoArray,RTE_TaskId * &pWaitingTasks,SAPDB_Int4 &itemCount,RTESync_IRWRegionReader::Mode mode,RTESync_IRWRegionReader::Selection  selection)
{
    SAPDB_Int4 currentCount;
    StatisticInfo *pSnapshotInfoArray = 0;

    while ( 0 == pSnapshotInfoArray )
    {
        currentCount = m_Count; // dirty read... this leads to the necessity of 'while()' loop. See below
        if ( 0 == currentCount )
        {
            return false;
        }

        pSnapshotInfoArray = reinterpret_cast<StatisticInfo *>(RTEMem_RteAllocator::Instance().Allocate(currentCount * sizeof(StatisticInfo)));
        if ( 0 == pSnapshotInfoArray )
        {
            return false;
        }

        memset( pSnapshotInfoArray, 0, currentCount * sizeof(StatisticInfo));

        pWaitingTasks = (RTE_TaskId *)(RTEMem_RteAllocator::Instance()).Allocate( KGS->ulNumOfTasks * sizeof(RTE_TaskId));
        if ( 0 == pWaitingTasks )
        {
            return false;
        }

        memset( pWaitingTasks, 0, KGS->ulNumOfTasks * sizeof(RTE_TaskId));
        /* This is using a locked scope... */
        if ( !FillStatisticInfo(pSnapshotInfoArray,pWaitingTasks, currentCount, itemCount,mode,selection) )
        {
            /* FillInfoArray() only reason to fail, is that the most unlikely case happened and */
            /* in the meantime more allocators were registered. This is handled with forcing a loop here... */
            RTEMem_RteAllocator::Instance().Deallocate(pSnapshotInfoArray);
            pSnapshotInfoArray = 0;
        }
    }

    pInfoArray = pSnapshotInfoArray;

    return true;
}

void RTESync_RWRegionRegister::FreeStatisticInfo(StatisticInfo *pInfoArray,RTE_TaskId *pWaitingTasks)
{
    RTEMem_RteAllocator::Instance().Deallocate(pInfoArray);
    RTEMem_RteAllocator::Instance().Deallocate(pWaitingTasks);
}

/*!
   @class RTESync_RWRegion
   @brief class containing methods common to all flavours of RWRegions
    RWRegions can be accessed from outside ONLY by using the interface RTESync_IRWRegion.
    The "flavours" of RWRegions (currently NoSemalist and WithSemalist) are implemented
    in different classes. The methods that are common to all flavours are implemented
    in the class RTESync_RWRegion.
*/
class RTESync_RWRegion : public RTESync_IRWRegion
{
public:
    /*!
      @brief ctor

      @param id [in] region id
      @param pool [in] pool region is associated with
      @param enterCollisionCounterInTcb [in] flag to indicate collision count should be visible in TCB
     */
    RTESync_RWRegion(SAPDB_Int8 id,RTESync_SpinlockPool &pool,SAPDB_Bool enterCollisionCounterInTcb=false);

    /*!
      @brief dtor
     */
    ~RTESync_RWRegion();

    /*!
      @brief enter non interruptable

      @param exclusive [in] exclusive access wanted
      @param pid [in] if known, current task id (default RTE_UnknownTaskId)
     */
    void enter(SAPDB_Bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId);

    /*!
      @brief enter non interruptable

      @param exclusive [in] exclusive access wanted
      @param pTaskObject [in] current task object
     */
    void enter(SAPDB_Bool exclusive,RTETask_ITask *pTaskObject);

    /*!
      @brief enter interruptible

      @param exclusive [in] exclusive access wanted
      @param cancelable [in] if set, entering region can be aborted
      @param timeout [in] timeout in (ask Robin)
      @param pid [in] if known, current task id (default RTE_UnknownTaskId)
     */
    void enterInterruptible(SAPDB_Bool exclusive,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Bool &hasBeenInterrupted,RTE_TaskId pid=RTE_UnknownTaskId);

    /*!
      @brief                            enter the region either exclusively or non-exclusively
                                        the blocking wait may be canceled from outside!

      @param exclusive          [in]    true if exclusive access is desired, false if non-exclusive access is desired
      @param hasBeenCanceled    [out]   true if the wait has been canceled. Do not enter the critical section in this case!!!
      @param pTaskObject        [in]    the current task object. As there are means to calculate the current task object, it can be 
                                        zero. But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    void enterInterruptible(SAPDB_Bool exclusive,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Bool &hasBeenInterrupted,RTETask_ITask *pTaskObject);

    /*!
      @brief try to enter region

      @param exclusive   [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pid [in] if known, current task id (default RTE_UnknownTaskId)
      @return true region entered, false if region currently blocked
     */
    bool tryEnter(SAPDB_Bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId);

    /*!
      @brief try to enter region

      @param exclusive   [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pTaskObject [in] current task object
      @return true region entered, false if region currently blocked
     */
    bool tryEnter(SAPDB_Bool exclusive,RTETask_ITask *pTaskObject);

    /*!
      @brief leave region

      @param exclusive   [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pid [in] if known, current task id (default RTE_UnknownTaskId)
     */
    void leave(SAPDB_Bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId);

    /*!
      @brief leave region

      @param exclusive   [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pTaskObject [in] current task object
     */
    void leave(SAPDB_Bool exclusive,RTETask_ITask *pTaskObject);

    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    void SwitchToNonExclusiveAccess(RTE_TaskId pid=RTE_UnknownTaskId);

    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    void SwitchToNonExclusiveAccess(RTETask_ITask *pTaskObject);

    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    SAPDB_Int8 getId(void)
    {
        return m_id;
    };

    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    RTE_TaskId getLastEntered(void)
    {
        return m_lastEntered;
    };

    virtual RTESync_WaitQueueForRWRegion *GetWaitQueue(void)
    {
        return NULL;
    }

    virtual SAPDB_UInt4 GetCollisionCounter(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetExclusiveAccessCounter(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetNonExclusiveAccessCounter(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetWaitCounter(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetTasCounter(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetCollisionCounterAtReset(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetExclusiveAccessCounterAtReset(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetNonExclusiveAccessCounterAtReset(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetWaitCounterAtReset(void)
    {
        return 0;
    }

    virtual SAPDB_UInt4 GetTasCounterAtReset(void)
    {
        return 0;
    }

    virtual void Reset(void)
    {
    }

    virtual RTESync_SpinlockPool *GetSpinlockPool(void)
	{
		return &m_pool;
	}

protected:
    SAPDB_Int8      m_id;
    RTE_TaskId      m_lastEntered;
    volatile int    m_nonExclusive;
    SAPDB_UInt      m_exclusiveRequests;

    void get_lock(RTETask_Task *pTask, SAPDB_Bool busyWaiting,SAPDB_ULong &tasCounter);

    void free_lock();

private:
    SAPDB_Int4                                  m_poolLockIndex;
    SAPDB_Bool                                  m_enterCollisionCounterInTcb;   // only for vbegexcl()/vendexcl() emulation
    RTE_ItemRegister<RTESync_IRWRegion>::Info   m_info;
    virtual SAPDB_Bool enterRegionExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)=0;
    virtual SAPDB_Bool enterRegionNonExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)=0;
    virtual void leaveRegionExclusive(RTETask_Task *pTaskObject)=0;
    virtual void leaveRegionNonExclusive(RTETask_Task *pTaskObject)=0;
    virtual void SwitchNonExclusive(RTETask_Task *pTaskObject)=0;
    virtual SAPDB_Bool tryEnterRegionExclusiveNonBlocking(RTETask_Task *pTaskObject)=0;
    virtual SAPDB_Bool tryEnterRegionNonExclusiveNonBlocking(RTETask_Task *pTaskObject)=0;

    void check_indefinite_loop(SAPDB_ULong &tasCount,RTETask_Task *pTaskObject);

    RTESync_SpinlockPool  & m_pool;
friend class RTESync_RWRegionRegister;
};

/*!
   @class RTESync_WaitQueueForRWRegion
   @brief class implementing the wait queue in which tasks waiting for access to a RWRegion are stored
 */
class RTESync_WaitQueueForRWRegion
{
public:
    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    SAPDB_Bool empty(void);

    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    void insert(SAPDB_Bool exclusive,RTETask_Task *pTask);
    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    RTETask_Task *getFirst(void);

    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    void removeFirst(void);

    /*!
      @brief remove a given task from the wait queue

      @param pTask [in] the task to remove
      @return [SAPDB_Bool] true if pTask was found in the wait queue, false else
     */
    SAPDB_Bool remove(RTETask_Task *pTask);

    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    RTESync_WaitQueueForRWRegion();

private:
    RTETask_Task *m_pfirstEntry;
    RTETask_Task *m_plastEntry;
};

/*!
   @class RTESync_RWRegionWithSemalist
   @brief the implementation class for the "WithSemalist" flavour of RWRegions
 */
class RTESync_RWRegionWithSemalist : public RTESync_RWRegion
{
public:
    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    RTESync_RWRegionWithSemalist(SAPDB_Int8 id,RTESync_SpinlockPool &pool);

    virtual RTESync_WaitQueueForRWRegion *GetWaitQueue(void)
    {
        return &m_waitQueue;
    }

    virtual SAPDB_UInt4 GetCollisionCounter(void)
    {
        return m_collisionCounter;
    }

    virtual SAPDB_UInt4 GetExclusiveAccessCounter(void)
    {
        return m_exclusiveAccessCounter;
    }

    virtual SAPDB_UInt4 GetNonExclusiveAccessCounter(void)
    {
        return m_nonExclusiveAccessCounter;
    }

    virtual SAPDB_UInt4 GetWaitCounter(void)
    {
        return m_waitCounter;
    }

    virtual SAPDB_UInt4 GetTasCounter(void)
    {
        return m_tasCounter;
    }

    virtual SAPDB_UInt4 GetCollisionCounterAtReset(void)
    {
        return m_collisionCounterAtReset;
    }

    virtual SAPDB_UInt4 GetExclusiveAccessCounterAtReset(void)
    {
        return m_exclusiveAccessCounterAtReset;
    }

    virtual SAPDB_UInt4 GetNonExclusiveAccessCounterAtReset(void)
    {
        return m_nonExclusiveAccessCounterAtReset;
    }

    virtual SAPDB_UInt4 GetWaitCounterAtReset(void)
    {
        return m_waitCounterAtReset;
    }

    virtual SAPDB_UInt4 GetTasCounterAtReset(void)
    {
        return m_tasCounterAtReset;
    }

    virtual void Reset(void)
    {
        m_exclusiveAccessCounterAtReset       = m_exclusiveAccessCounter;
        m_nonExclusiveAccessCounterAtReset    = m_nonExclusiveAccessCounter;
        m_collisionCounterAtReset             = m_collisionCounter;
        m_waitCounterAtReset                  = m_waitCounter;
        m_tasCounterAtReset                   = m_tasCounter;
    }

private:
    SAPDB_UInt4 m_exclusiveAccessCounter;
    SAPDB_UInt4 m_nonExclusiveAccessCounter;
    SAPDB_UInt4 m_collisionCounter;
    SAPDB_UInt4     m_waitCounter;
    SAPDB_ULong     m_tasCounter;

    SAPDB_UInt4 m_exclusiveAccessCounterAtReset;
    SAPDB_UInt4 m_nonExclusiveAccessCounterAtReset;
    SAPDB_UInt4 m_collisionCounterAtReset;
    SAPDB_UInt4 m_waitCounterAtReset;
    SAPDB_ULong m_tasCounterAtReset;

    SAPDB_Bool enterRegionExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid);
    SAPDB_Bool enterRegionNonExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid);
    void leaveRegionExclusive(RTETask_Task *pTaskObject);
    void activateTaskInMyUKT(RTETask_Task *pTaskObject,RTETask_Task *pNextTaskObject,SAPDB_Bool &taskFoundWithHigherPriority);
    void leaveRegionNonExclusive(RTETask_Task *pTaskObject);
    void SwitchNonExclusive(RTETask_Task *pTaskObject);

    SAPDB_Bool tryEnterRegionExclusiveNonBlocking(RTETask_Task *pTaskObject);
    SAPDB_Bool tryEnterRegionNonExclusiveNonBlocking(RTETask_Task *pTaskObject);

    void activateTaskInOtherUKT(RTETask_Task *pTaskObject,RTETask_Task *pTaskNextObject);
    RTESync_WaitQueueForRWRegion m_waitQueue;

};

/*!
   @class RTESync_RWRegionNoSemalist
   @brief the implementation class for the "NoSemalist" flavour of RWRegions
 */
class RTESync_RWRegionNoSemalist : public RTESync_RWRegion
{
public:
    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    RTESync_RWRegionNoSemalist(SAPDB_Int8 id,RTESync_SpinlockPool &pool);
private:
    SAPDB_Bool enterRegionExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid);
    SAPDB_Bool enterRegionNonExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid);
    void leaveRegionExclusive(RTETask_Task *pTaskObject);
    void leaveRegionNonExclusive(RTETask_Task *pTaskObject);
    void SwitchNonExclusive(RTETask_Task *pTaskObject);
    SAPDB_Bool tryEnterRegionExclusiveNonBlocking(RTETask_Task *pTaskObject);
    SAPDB_Bool tryEnterRegionNonExclusiveNonBlocking(RTETask_Task *pTaskObject);
};

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/


RTESync_RWRegion::RTESync_RWRegion(SAPDB_Int8 id,RTESync_SpinlockPool &pool,SAPDB_Bool enterCollisionCounterInTcb)
    : m_id(id)
    , m_nonExclusive(0)
    , m_pool(pool)
    , m_poolLockIndex(pool.GetLockIndex(id))
    , m_exclusiveRequests(0)
    , m_lastEntered(-1)
    , m_enterCollisionCounterInTcb(enterCollisionCounterInTcb)
    , m_info(NULL,NULL)
{
    if(0 == id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,"","constructor"));
    }
    new ( ((void *)&m_info) ) RTE_ItemRegister<RTESync_IRWRegion>::Info ( (const SAPDB_UTF8 *)"", this );
    m_pool.Register(m_info);
}

/*---------------------------------------------------------------------------*/

RTESync_RWRegion::~RTESync_RWRegion()
{
    m_pool.Deregister(m_info);
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::enter(SAPDB_Bool exclusive,RTE_TaskId pid)
{

#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pid),"enter"));
    }
#endif
    RTETask_Task *pTaskObject;
    if(pid)
    {
        pTaskObject = RTETask_Task::ByTaskID(pid);
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pid),SAPDB_ToString(RTETask_Task::Self()->ID()),"enter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
        pid = pTaskObject->ID();
    }

    pTaskObject->LegacyTaskCtrl().BeginExclusiveCalls()++;
    SAPDB_Long maxCollisions = XPARAM(ulRegionCollisionLoop);
    SAPDB_Long collisionCounter = 0;
    pTaskObject->LegacyTaskCtrl().InRegionID() = (RTE_RegionId)m_id;
    pTaskObject->RWRegion() = this;
    pTaskObject->LegacyTaskCtrl().BeginExclusiveCount()++;
    pTaskObject->State() = RTETask_Task::TaskEnteringRWRegion;
    pTaskObject->TaskIsWaitingForExclusiveAccess() = exclusive;
    if(exclusive)
    {
#ifdef SAPDB_SLOW
        if(pid == m_lastEntered)
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ALREADY_ENTERED,SAPDB_ToString(pid),SAPDB_ToString(m_id)));
        }
#endif
        enterRegionExclusive(pTaskObject,false/*not cancelable*/,0/*no timeout*/,maxCollisions,collisionCounter,pid);
    }
    else
    {
        enterRegionNonExclusive(pTaskObject,false/*not cancelable*/,0/*no timeout*/,maxCollisions,collisionCounter,pid);
    }
    if(m_enterCollisionCounterInTcb)
    {
        pTaskObject->LegacyTaskCtrl().RegionCollisionCounter(m_id-1) += collisionCounter;
    }
    pTaskObject->State() = RTETask_Task::TaskIsRunning;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::enter(SAPDB_Bool exclusive,RTETask_ITask *pITaskObject)
{
    RTETask_Task *pTaskObject;
    if(pITaskObject)
    {
        pTaskObject = (RTETask_Task *)pITaskObject;
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(RTETask_Task::Self()->ID()),"enter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
    }
#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pTaskObject->ID()),"enter"));
    }
#endif
    RTETask_ScopeRelatedTaskState scopeRelatedTaskState( pTaskObject, RTETask_Task::TaskEnteringRWRegion );

    pTaskObject->LegacyTaskCtrl().BeginExclusiveCalls()++;
    SAPDB_Long maxCollisions = XPARAM(ulRegionCollisionLoop);
    SAPDB_Long collisionCounter = 0;
    pTaskObject->LegacyTaskCtrl().InRegionID() = (RTE_RegionId)m_id;
    pTaskObject->RWRegion() = this;
    pTaskObject->LegacyTaskCtrl().BeginExclusiveCount()++;
    pTaskObject->TaskIsWaitingForExclusiveAccess() = exclusive;
    if(exclusive)
    {
#ifdef SAPDB_SLOW
        if(pTaskObject->ID() == m_lastEntered)
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ALREADY_ENTERED,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(m_id)));
        }
#endif
        enterRegionExclusive(pTaskObject,false/*not cancelable*/,0/*no timeout*/,maxCollisions,collisionCounter,pTaskObject->ID());
    }
    else
    {
        enterRegionNonExclusive(pTaskObject,false/*not cancelable*/,0/*no timeout*/,maxCollisions,collisionCounter,pTaskObject->ID());
    }
    if(m_enterCollisionCounterInTcb)
    {
        pTaskObject->LegacyTaskCtrl().RegionCollisionCounter(m_id-1) += collisionCounter;
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::enterInterruptible(SAPDB_Bool exclusive,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Bool &hasBeenInterrupted,RTE_TaskId pid)
{

#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pid),"enter"));
    }
#endif
    RTETask_Task *pTaskObject;
    if(pid)
    {
        pTaskObject = RTETask_Task::ByTaskID(pid);
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pid),SAPDB_ToString(RTETask_Task::Self()->ID()),"enter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
        pid = pTaskObject->ID();
    }

    pTaskObject->LegacyTaskCtrl().BeginExclusiveCalls()++;
    SAPDB_Long maxCollisions = XPARAM(ulRegionCollisionLoop);
    SAPDB_Long collisionCounter = 0;
    pTaskObject->LegacyTaskCtrl().InRegionID() = (RTE_RegionId)m_id;
    pTaskObject->RWRegion() = this;
    pTaskObject->LegacyTaskCtrl().BeginExclusiveCount()++;
    pTaskObject->State() = RTETask_Task::TaskEnteringRWRegion;
    pTaskObject->TaskIsWaitingForExclusiveAccess() = exclusive;
    if(exclusive)
    {
#ifdef SAPDB_SLOW
        if(pid == m_lastEntered)
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ALREADY_ENTERED,SAPDB_ToString(pid),SAPDB_ToString(m_id)));
        }
#endif
        hasBeenInterrupted = !enterRegionExclusive(pTaskObject,cancelable,timeout,maxCollisions,collisionCounter,pid);
    }
    else
    {
        hasBeenInterrupted = !enterRegionNonExclusive(pTaskObject,cancelable,timeout,maxCollisions,collisionCounter,pid);
    }
    if(!hasBeenInterrupted && m_enterCollisionCounterInTcb)
    {
        pTaskObject->LegacyTaskCtrl().RegionCollisionCounter(m_id-1) += collisionCounter;
    }
    if(hasBeenInterrupted)
    {
        pTaskObject->LegacyTaskCtrl().InRegionID() = 0;
        pTaskObject->RWRegion() = NULL;
    }
    pTaskObject->State() = RTETask_Task::TaskIsRunning;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::enterInterruptible(SAPDB_Bool exclusive,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Bool &hasBeenInterrupted,RTETask_ITask *pITaskObject)
{
    RTETask_Task *pTaskObject;
    if(pITaskObject)
    {
        pTaskObject = (RTETask_Task *)pITaskObject;
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(RTETask_Task::Self()->ID()),"enter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
    }
#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pTaskObject->ID()),"enter"));
    }
#endif

    RTETask_ScopeRelatedTaskState scopeRelatedTaskState( pTaskObject, RTETask_Task::TaskEnteringRWRegion );

    pTaskObject->LegacyTaskCtrl().BeginExclusiveCalls()++;
    SAPDB_Long maxCollisions = XPARAM(ulRegionCollisionLoop);
    SAPDB_Long collisionCounter = 0;
    pTaskObject->LegacyTaskCtrl().InRegionID() = (RTE_RegionId)m_id;
    pTaskObject->RWRegion() = this;
    pTaskObject->LegacyTaskCtrl().BeginExclusiveCount()++;
    pTaskObject->TaskIsWaitingForExclusiveAccess() = exclusive;
    if(exclusive)
    {
#ifdef SAPDB_SLOW
        if(pTaskObject->ID() == m_lastEntered)
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ALREADY_ENTERED,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(m_id)));
        }
#endif
        hasBeenInterrupted = !enterRegionExclusive(pTaskObject,cancelable,timeout,maxCollisions,collisionCounter,pTaskObject->ID());
    }
    else
    {
        hasBeenInterrupted = !enterRegionNonExclusive(pTaskObject,cancelable,timeout,maxCollisions,collisionCounter,pTaskObject->ID());
    }
    if(!hasBeenInterrupted && m_enterCollisionCounterInTcb)
    {
        pTaskObject->LegacyTaskCtrl().RegionCollisionCounter(m_id-1) += collisionCounter;
    }
    if(hasBeenInterrupted)
    {
        pTaskObject->LegacyTaskCtrl().InRegionID() = 0;
        pTaskObject->RWRegion() = NULL;
    }

}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::leave(SAPDB_Bool exclusive,RTE_TaskId pid)
{
#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pid),"leave"));
    }
#endif
    RTETask_Task *pTaskObject;
    if(pid)
    {
        pTaskObject = RTETask_Task::ByTaskID(pid);
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pid),SAPDB_ToString(RTETask_Task::Self()->ID()),"leave"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
        pid = pTaskObject->ID();
    }

    if(exclusive)
    {
#ifdef SAPDB_SLOW
        if(pid != m_lastEntered)
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_LEAVE_WITHOUT_ENTER,SAPDB_ToString(pid),SAPDB_ToString(m_id)));
        }
#endif
        leaveRegionExclusive(pTaskObject);
    }
    else
    {
        leaveRegionNonExclusive(pTaskObject);
    }

    if (( pTaskObject->LegacyTaskCtrl().BeginExclusiveCount() > (SAPDB_Long)XPARAM(ulRegLockSlice) ||
            pTaskObject->TaskScheduler().LegacyUKTCtrl().prioritizedTasksInRunqueue() ) &&
            0 == pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount() && !pTaskObject->LegacyTaskCtrl().PrioFlag() )
    {
        if(0 != pTaskObject->TaskScheduler().LegacyUKTCtrl().prioritizedTasksInRunqueue())
            pTaskObject->TaskScheduler().LegacyUKTCtrl().prioritizedTasksInRunqueue()--;
        pTaskObject->State() = RTETask_Task::TaskRunnable;
        pTaskObject->LegacyTaskCtrl().EndExclusiveSelfSuspendCount()++;
        pTaskObject->YieldTask();
    }
    pTaskObject->LegacyTaskCtrl().InRegionID() = 0;
    pTaskObject->RWRegion() = NULL;
    pTaskObject->State() = RTETask_Task::TaskIsRunning;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::leave(SAPDB_Bool exclusive,RTETask_ITask *pITaskObject)
{
    RTETask_Task *pTaskObject;
    if(pITaskObject)
    {
        pTaskObject = (RTETask_Task *)pITaskObject;
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(RTETask_Task::Self()->ID()),"enter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
    }

    RTETask_ScopeRelatedTaskState scopeRelatedTaskState( pTaskObject, RTETask_Task::TaskLeavingRWRegion );
    if(exclusive)
    {
#ifdef SAPDB_SLOW
        if(pTaskObject->ID() != m_lastEntered)
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_LEAVE_WITHOUT_ENTER,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(m_id)));
        }
#endif
        leaveRegionExclusive(pTaskObject);
    }
    else
    {
        leaveRegionNonExclusive(pTaskObject);
    }

    if (( pTaskObject->LegacyTaskCtrl().BeginExclusiveCount() > (SAPDB_Long)XPARAM(ulRegLockSlice) ||
            pTaskObject->TaskScheduler().LegacyUKTCtrl().prioritizedTasksInRunqueue() ) &&
            0 == pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount() && !pTaskObject->LegacyTaskCtrl().PrioFlag() )
    {
        if(0 != pTaskObject->TaskScheduler().LegacyUKTCtrl().prioritizedTasksInRunqueue())
            pTaskObject->TaskScheduler().LegacyUKTCtrl().prioritizedTasksInRunqueue()--;
        pTaskObject->State() = RTETask_Task::TaskRunnable;
        pTaskObject->LegacyTaskCtrl().EndExclusiveSelfSuspendCount()++;
        pTaskObject->YieldTask();
    }
    pTaskObject->LegacyTaskCtrl().InRegionID() = 0;
    pTaskObject->RWRegion() = NULL;
    pTaskObject->State() = RTETask_Task::TaskIsRunning;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegion::tryEnter(SAPDB_Bool exclusive,RTE_TaskId pid)
{
    RTETask_Task *pTaskObject;
    if(pid)
    {
        pTaskObject = RTETask_Task::ByTaskID(pid);
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pid),SAPDB_ToString(RTETask_Task::Self()->ID()),"tryEnter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
        pid = pTaskObject->ID();
    }

#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pid),"tryEnter"));
    }
#endif
    pTaskObject->LegacyTaskCtrl().InRegionID() = (RTE_RegionId)m_id;
    pTaskObject->State() = RTETask_Task::TaskEnteringRWRegion;

    SAPDB_Bool canEnter;
    if(exclusive)
        canEnter = tryEnterRegionExclusiveNonBlocking(pTaskObject);
    else
        canEnter = tryEnterRegionNonExclusiveNonBlocking(pTaskObject);

    if(canEnter)
    {
        pTaskObject->LegacyTaskCtrl().BeginExclusiveCalls()++;
        pTaskObject->LegacyTaskCtrl().BeginExclusiveCount()++;
    }
    pTaskObject->State() = RTETask_Task::TaskIsRunning;
    return canEnter;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegion::tryEnter(SAPDB_Bool exclusive,RTETask_ITask *pITaskObject)
{
    RTETask_Task *pTaskObject;
    if(pITaskObject)
    {
        pTaskObject = (RTETask_Task *)pITaskObject;
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(RTETask_Task::Self()->ID()),"enter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
    }
#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pTaskObject->ID()),"tryEnter"));
    }
#endif
    pTaskObject->LegacyTaskCtrl().InRegionID() = (RTE_RegionId)m_id;

    RTETask_ScopeRelatedTaskState scopeRelatedTaskState( pTaskObject, RTETask_Task::TaskEnteringRWRegion );

    SAPDB_Bool canEnter;
    if(exclusive)
        canEnter = tryEnterRegionExclusiveNonBlocking(pTaskObject);
    else
        canEnter = tryEnterRegionNonExclusiveNonBlocking(pTaskObject);

    if(canEnter)
    {
        pTaskObject->LegacyTaskCtrl().BeginExclusiveCalls()++;
        pTaskObject->LegacyTaskCtrl().BeginExclusiveCount()++;
    }
    return canEnter;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::SwitchToNonExclusiveAccess(RTE_TaskId pid)
{
    RTETask_Task *pTaskObject;
    if(pid)
    {
        pTaskObject = RTETask_Task::ByTaskID(pid);
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pid),SAPDB_ToString(RTETask_Task::Self()->ID()),"SwitchToNonExclusive"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
        pid = pTaskObject->ID();
    }
#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pTaskObject->ID()),"SwitchToNonExclusive"));
    }
#endif
    SwitchNonExclusive( pTaskObject );
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::SwitchToNonExclusiveAccess(RTETask_ITask *pITaskObject)
{
    RTETask_Task *pTaskObject;
    if(pITaskObject)
    {
        pTaskObject = (RTETask_Task *)pITaskObject;
#ifdef SAPDB_SLOW
        if( pTaskObject != RTETask_Task::Self() )
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_TASKID_MISMATCH,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(RTETask_Task::Self()->ID()),"enter"));
        }
#endif
    }
    else
    {
        pTaskObject = RTETask_Task::Self();
    }
#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pTaskObject->ID()),"tryEnter"));
    }
#endif
#ifdef SAPDB_SLOW
    if(0 == m_id)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_ID_ZERO,SAPDB_ToString(pTaskObject->ID()),"SwitchToNonExclusive"));
    }
#endif
    SwitchNonExclusive( pTaskObject );
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::check_indefinite_loop(SAPDB_ULong &tasCount,RTETask_Task *pTaskObject)
{
    if ( MAX_TAS_COUNT == tasCount )
    {
        if ( pTaskObject->Type() == TT_TW_EO00 && pTaskObject->TaskScheduler().LegacyUKTCtrl().activeTasks() == 1 )
        {
            RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT,RTEINFO_SYNC_RWREGION_TW_SLEEPING));
            RTETask_ITask::ByTaskID( pTaskObject->ID()+1 )->Sleep(1);
            tasCount = 0;
        }
        else
        {
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INDEFINITE_LOOP,SAPDB_ToString(m_id)));
        }
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::get_lock(RTETask_Task *pTaskObject,SAPDB_Bool busyWaiting,SAPDB_ULong &tasCount)
{
    if ( m_pool.TryLock(m_poolLockIndex) )
    {
        while ( m_pool.TryLock(m_poolLockIndex) )
        {
            tasCount++;
            check_indefinite_loop(tasCount, pTaskObject );
            if ( KGS->fBusyWaitingForbidden )
            {
                RTE_ISystem::DoSleep( 0 ); // reschedule ukt
            }
            else if ( !busyWaiting )
            {
            /* --- reschedule myself (task) via rex queue  */
                pTaskObject->YieldTask(/*false*/);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegion::free_lock()
{
    m_pool.Unlock(m_poolLockIndex);
}

/*---------------------------------------------------------------------------*/

RTESync_RWRegionNoSemalist::RTESync_RWRegionNoSemalist(SAPDB_Int8 id,RTESync_SpinlockPool &pool)
    :
    RTESync_RWRegion(id,pool)
{
}

/*---------------------------------------------------------------------------*/
SAPDB_Bool RTESync_RWRegionNoSemalist::enterRegionExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)
{
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()++;
    SAPDB_Bool fRegionFree;
    SAPDB_ULong tasCounter;
    if(XPARAM(ulRegionCollisionLoop)  &&
       ( !pTaskObject->LegacyTaskCtrl().PrioFlag() || !XPARAM(fBegExclQueWhenPrio)))
    {
        get_lock(pTaskObject,false,tasCounter);
    }
    else
    {
        do
        {
            fRegionFree = (0==m_nonExclusive); /* dirty read */
            if ( fRegionFree )
            {
                get_lock(pTaskObject,false,tasCounter);
                fRegionFree = (0==m_nonExclusive); /* real read */
                if ( !fRegionFree )
                    free_lock();
            }

            if ( !fRegionFree )
            {
                collisionCounter++;

                pTaskObject->YieldTask(/*false*/);
            }
        }
        while ( !fRegionFree ) ;
    }

    m_nonExclusive = -1;
    m_lastEntered = pTaskObject->ID();
    free_lock();

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionNoSemalist::enterRegionNonExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)
{
    SAPDB_ULong tasCounter;
    do
    {
        get_lock(pTaskObject,false,tasCounter);
        if(m_nonExclusive < 0)
        {
            free_lock();
            collisionCounter++;

            pTaskObject->YieldTask(/*false*/);
        }
        else
        {
            break;
        }
    }
    while(1);

    /*
    * we are alone now
    */

    m_lastEntered = pTaskObject->ID();
    m_nonExclusive++;
    if(0 == m_nonExclusive)
    {
        SAPDB_Char msg[256];
        SAPDB_sprintf(msg,256," m_nonExclusive is 0 after enter (non-Exclusive)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
    }
    free_lock();
    return true;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionNoSemalist::leaveRegionExclusive(RTETask_Task *pTaskObject)
{
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()--;
    m_nonExclusive = 0;   /* the no-sema-list case works without a lock */
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionNoSemalist::leaveRegionNonExclusive(RTETask_Task *pTaskObject)
{
    m_nonExclusive--;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionNoSemalist::SwitchNonExclusive(RTETask_Task *pTaskObject)
{
    SAPDB_ULong tasCounter;
    get_lock(pTaskObject,false,tasCounter);
    if(-1 != m_nonExclusive)
    {
        SAPDB_Char msg[256];
        SAPDB_sprintf(msg,256,"SwitchToNonExclusiveAccess() called but m_nonExclusive is != -1 (%d)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
    }
    m_nonExclusive = 1;
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()--;
    free_lock();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionNoSemalist::tryEnterRegionExclusiveNonBlocking(RTETask_Task *pTaskObject)
{
    SAPDB_ULong tasCounter;
    get_lock(pTaskObject,false,tasCounter);
    if(m_nonExclusive != 0) // the region has been locked ("exclusive" or "non-exclusive") -> can't enter
    {
        free_lock();
        return false;
    }
    else
    {
        m_exclusiveRequests++;
        m_nonExclusive = -1;
        m_lastEntered = pTaskObject->ID();
        pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()++;
        free_lock();
        return true;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionNoSemalist::tryEnterRegionNonExclusiveNonBlocking(RTETask_Task *pTaskObject)
{
    SAPDB_ULong tasCounter;
    get_lock(pTaskObject,false,tasCounter);
    if(m_nonExclusive < 0)  // the region has been locked "exclusive" -> can't enter
    {
        free_lock();
        return false;
    }
    else
    {
        m_nonExclusive++;
        m_lastEntered = pTaskObject->ID();
        free_lock();
        return true;
    }
}

/*!
   @class RTESync_RWRegionWithSemalistNoStatistics
   @brief the implementation class for the "WithSemalist" flavour of RWRegions
 */
class RTESync_RWRegionWithSemalistNoStatistics : public RTESync_RWRegion
{
public:
    /*!
      @brief

      @param <name> [in]
      @return [<type>]
     */
    RTESync_RWRegionWithSemalistNoStatistics(SAPDB_Int8 id,RTESync_SpinlockPool &pool);

    virtual RTESync_WaitQueueForRWRegion *GetWaitQueue(void)
    {
        return &m_waitQueue;
    }
private:
    SAPDB_Bool enterRegionExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid);
    SAPDB_Bool enterRegionNonExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid);
    void leaveRegionExclusive(RTETask_Task *pTaskObject);
    void activateTaskInMyUKT(RTETask_Task *pTaskObject,RTETask_Task *pNextTaskObject,SAPDB_Bool &taskFoundWithHigherPriority );
    void leaveRegionNonExclusive(RTETask_Task *pTaskObject);
    void SwitchNonExclusive(RTETask_Task *pTaskObject);

    SAPDB_Bool tryEnterRegionExclusiveNonBlocking(RTETask_Task *pTaskObject);
    SAPDB_Bool tryEnterRegionNonExclusiveNonBlocking(RTETask_Task *pTaskObject);

    void activateTaskInOtherUKT(RTETask_Task *pTaskObject,RTETask_Task *pTaskNextObject);
    RTESync_WaitQueueForRWRegion m_waitQueue;

};
/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_WaitQueueForRWRegion::empty(void)
{
    return (NULL == m_pfirstEntry);
}

/*---------------------------------------------------------------------------*/

void RTESync_WaitQueueForRWRegion::insert(SAPDB_Bool exclusive,RTETask_Task *pTask)
{
    pTask->NextTaskInWaitQueue()               = NULL;
    if(empty()) // empty list?
    {           // then this one is the first and last entry
        m_pfirstEntry = m_plastEntry = pTask;
    }
    else
    {           // else add this tcb after the last entry
        m_plastEntry->NextTaskInWaitQueue()  = pTask;
        m_plastEntry                         = pTask;
    }
}

/*---------------------------------------------------------------------------*/

RTETask_Task *RTESync_WaitQueueForRWRegion::getFirst(void)
{
    return m_pfirstEntry;
}

/*---------------------------------------------------------------------------*/

void RTESync_WaitQueueForRWRegion::removeFirst(void)
{
    RTETask_Task *pTask          = m_pfirstEntry;
    m_pfirstEntry                = pTask->NextTaskInWaitQueue();
    pTask->NextTaskInWaitQueue() = NULL;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_WaitQueueForRWRegion::remove(RTETask_Task *pTask)
{
    RTETask_Task *pCurrentTask = m_pfirstEntry,*pPredecessor=NULL;
    
    while( pCurrentTask )
    {
        if ( pTask == pCurrentTask )
        {
            if( pPredecessor )
            {
                pPredecessor->NextTaskInWaitQueue() = pCurrentTask->NextTaskInWaitQueue();
            }
            else
            {
                m_pfirstEntry = pCurrentTask->NextTaskInWaitQueue();
            }
            if( m_plastEntry == pCurrentTask )
            {
                m_plastEntry = pPredecessor;
            }
            pCurrentTask->NextTaskInWaitQueue() = NULL;
            return true;
        }
        pPredecessor = pCurrentTask;
        pCurrentTask = pCurrentTask->NextTaskInWaitQueue();
    }
    return false;
}

/*---------------------------------------------------------------------------*/

RTESync_WaitQueueForRWRegion::RTESync_WaitQueueForRWRegion()
    : m_pfirstEntry(NULL)
    , m_plastEntry(NULL)
{
}

/*---------------------------------------------------------------------------*/

RTESync_RWRegionWithSemalist::RTESync_RWRegionWithSemalist(SAPDB_Int8 id,RTESync_SpinlockPool &pool)
    : RTESync_RWRegion(id,pool)
    , m_collisionCounter(0)
    , m_exclusiveAccessCounter(0)
    , m_nonExclusiveAccessCounter(0)
    , m_waitCounter(0)
    , m_tasCounter(0)
    , m_collisionCounterAtReset(0)
    , m_exclusiveAccessCounterAtReset(0)
    , m_nonExclusiveAccessCounterAtReset(0)
    , m_waitCounterAtReset(0)
    , m_tasCounterAtReset(0)
{
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalist::enterRegionExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)
{
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()++;
    do
    {
        get_lock(pTaskObject,false,m_tasCounter);
        if(m_nonExclusive != 0)   // not free?
        {
            collisionCounter++;
            if(collisionCounter >= maxCollisions)
                break;
            free_lock();

            pTaskObject->YieldTask(/*false*/);
        }
        else
        {
            break;
        }
    }
    while(true);
    m_exclusiveRequests++;
    if(m_nonExclusive != 0)
    {
        m_waitCounter++;
        m_waitQueue.insert(true,pTaskObject);
        free_lock();
        RTETask_CommonQueueElement &returnElement = pTaskObject->WaitForReschedule(cancelable,timeout);

        switch( returnElement.GetType())
        {
        case RTETask_CommonQueueElement::CancelTask : // task was cancelled:
        case RTETask_CommonQueueElement::RequestTimedOut : // task was cancelled:
            returnElement.Release();
            get_lock(pTaskObject,false,m_tasCounter);
            m_waitQueue.remove(pTaskObject);    // remove task from the wait queue
            pTaskObject->ResetPendingCancel();         // reset the "cancel pending" flag
            pTaskObject->EndOfInterruptibleRequest();
            free_lock();
            return false;
            break;
        case RTETask_CommonQueueElement::LeaveRWRegion:  // the expected one: go ahead!
            break;
        default:    // any other requests are badly wrong
            RTE_Crash(Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"enterRegionExclusive got wrong request:",SAPDB_ToString(returnElement.GetType())));
        }
        returnElement.Release();
    }
    else
    {
        m_nonExclusive = -1;
        m_lastEntered = pid;
        free_lock();
    }
    m_exclusiveAccessCounter++;
    m_collisionCounter += collisionCounter;

    pTaskObject->EndOfInterruptibleRequest();
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalist::enterRegionNonExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)
{
    do
    {
        get_lock(pTaskObject,false,m_tasCounter);
        if(m_nonExclusive < 0)   // not free?
        {
            collisionCounter++;
            if(collisionCounter >= maxCollisions)
                break;
            free_lock();

            pTaskObject->YieldTask(/*false*/);
        }
        else
        {
            break;
        }
    }
    while(true);
    if(m_nonExclusive < 0 || m_exclusiveRequests > 0)
    {
        m_waitCounter++;
        m_waitQueue.insert(false,pTaskObject);
        free_lock();

        RTETask_CommonQueueElement &returnElement = pTaskObject->WaitForReschedule(cancelable,timeout);

        switch( returnElement.GetType())
        {
        case RTETask_CommonQueueElement::CancelTask : // task was cancelled:
        case RTETask_CommonQueueElement::RequestTimedOut : // task was cancelled:
            returnElement.Release();
            get_lock(pTaskObject,false,m_tasCounter);
            m_waitQueue.remove(pTaskObject);    // remove task from the wait queue
            pTaskObject->ResetPendingCancel();         // reset the "cancel pending" flag
            pTaskObject->EndOfInterruptibleRequest();
            free_lock();
            return false;
            break;
        case RTETask_CommonQueueElement::LeaveRWRegion:  // the expected one: go ahead!
            break;
        default:    // any other requests are badly wrong
            RTE_Crash(Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"enterRegionExclusive got wrong request:",SAPDB_ToString(returnElement.GetType())));
        }
        returnElement.Release();
    }
    else
    {
        m_nonExclusive++;
        m_lastEntered = pid;
        if(0 == m_nonExclusive)
        {
            SAPDB_Char msg[256];
            SAPDB_sprintf(msg,256," m_nonExclusive is 0 after enter (non-Exclusive)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
        }
        free_lock();
    }
    m_nonExclusiveAccessCounter++;
    m_collisionCounter += collisionCounter;

    pTaskObject->EndOfInterruptibleRequest();
    return true;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalist::leaveRegionExclusive(RTETask_Task *pTaskObject)
{
    SAPDB_Bool readerActivated = false;   // indicate that at least one reader has been activated. In this case, no writer must be activated
    get_lock(pTaskObject,false,m_tasCounter);
    if(0 <= m_nonExclusive)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_LEAVE_WITHOUT_ENTER,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(m_id)));
    }
    m_nonExclusive = 0;
    m_exclusiveRequests--;
    m_lastEntered = -1;     // at first assume that no task can be activated

    SAPDB_Bool taskFoundWithHigherPriority = false;
    do
    {
        if(m_waitQueue.empty()) // no tasks that could be activated -> leave
        {
            free_lock();
            break;
        }
        else
        {
            RTETask_Task *pNextTaskObject = m_waitQueue.getFirst();
            if (readerActivated && pNextTaskObject->TaskIsWaitingForExclusiveAccess())   // already readers activated and the next one would be a writer -> leave
            {
                free_lock();
                break;
            }
            else
            {
                m_waitQueue.removeFirst();
                m_waitCounter--;
                if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                {
                    m_nonExclusive = -1;
                }
                else
                {
                    m_nonExclusive++;
                    if(0 == m_nonExclusive)
                    {
                        SAPDB_Char msg[256];
                        SAPDB_sprintf(msg,256," m_nonExclusive is 0 after enter-after-leave (Exclusive)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
                        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
                    }
                }
                m_lastEntered = pNextTaskObject->ID();
                if(&(pNextTaskObject->TaskScheduler()) == &(pTaskObject->TaskScheduler()))   // the waiting task is in my UKT -> activate it (incl. a goto_disp()) and leave
                {
                    activateTaskInMyUKT(pTaskObject,pNextTaskObject,taskFoundWithHigherPriority);
                    if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        readerActivated = true;
                    }
                }
                else
                {
                    SAPDB_Bool nextTaskIsWaitingForExclusiveAccess = pNextTaskObject->TaskIsWaitingForExclusiveAccess();
                    activateTaskInOtherUKT(pTaskObject,pNextTaskObject);
                    if(nextTaskIsWaitingForExclusiveAccess)
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        // in fact, this is the only case in which the loop is continued: a reader task has been activated in another UKT. In this case, other reader tasks can also be activated
                        readerActivated = true;
                    }
                }
            }
        }
    }
    while(1);
    if ( taskFoundWithHigherPriority )
    {
        pTaskObject->YieldTask();
    }
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()--;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalist::leaveRegionNonExclusive(RTETask_Task *pTaskObject)
{
    SAPDB_Bool readerActivated = false;   // indicate that at least one reader has been activated. In this case, no writer must be activated
    get_lock(pTaskObject,false,m_tasCounter);
    if(0 >= m_nonExclusive)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_LEAVE_WITHOUT_ENTER,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(m_id)));
    }
    m_nonExclusive--;
    m_lastEntered = -1;     // at first assume that no task can be activated

    SAPDB_Bool taskFoundWithHigherPriority = false;
    do
    {
        if( m_waitQueue.empty()) // no tasks that could be activated -> leave
        {
            free_lock();
            break;
        }
        else
        {
            RTETask_Task *pNextTaskObject = m_waitQueue.getFirst();
            if(( ( m_nonExclusive != 0 ) && pNextTaskObject->TaskIsWaitingForExclusiveAccess() )   // there is still someone in the region and the next one would be a writer -> leave
               ||( readerActivated && pNextTaskObject->TaskIsWaitingForExclusiveAccess()))   // already readers activated and the next one would be a writer -> leave
            {
                free_lock();
                break;
            }
            else
            {
                m_waitQueue.removeFirst();
                m_waitCounter--;
                if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                    m_nonExclusive = -1;
                else
                {
                    m_nonExclusive++;
                    if(0 == m_nonExclusive)
                    {
                        SAPDB_Char msg[256];
                        SAPDB_sprintf(msg,256," m_nonExclusive is 0 after enter-after-leave (non-Exclusive)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
                        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
                    }
                }
                m_lastEntered = pNextTaskObject->ID();
                if(&(pNextTaskObject->TaskScheduler()) == &(pTaskObject->TaskScheduler()))   // the waiting task is in my UKT -> activate it (incl. a goto_disp()) and leave
                {
                    activateTaskInMyUKT(pTaskObject,pNextTaskObject,taskFoundWithHigherPriority);
                    if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        readerActivated = true;
                    }
                }
                else
                {
                    SAPDB_Bool nextTaskIsWaitingForExclusiveAccess = pNextTaskObject->TaskIsWaitingForExclusiveAccess();
                    activateTaskInOtherUKT(pTaskObject,pNextTaskObject);
                    if(nextTaskIsWaitingForExclusiveAccess)
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        // in fact, this is the only case in which the loop is continued: a reader task has been activated in another UKT. In this case, other reader tasks can also be activated
                        readerActivated = true;
                    }
                }
            }
        }
    }
    while(1);
    if ( taskFoundWithHigherPriority )
    {
        pTaskObject->YieldTask();
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalist::SwitchNonExclusive(RTETask_Task *pTaskObject)
{
    get_lock(pTaskObject,false,m_tasCounter);
    if(-1 != m_nonExclusive)
    {
        SAPDB_Char msg[256];
        SAPDB_sprintf(msg,256,"SwitchToNonExclusiveAccess() called but m_nonExclusive is != -1 (%d)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
    }
    m_nonExclusive = 1;
    m_exclusiveRequests--;
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()--;

    SAPDB_Bool taskFoundWithHigherPriority = false;
    do
    {
        if(m_waitQueue.empty()) // no tasks that could be activated -> leave
        {
            free_lock();
            break;
        }
        else
        {
            RTETask_Task *pNextTaskObject = m_waitQueue.getFirst();
            if (pNextTaskObject->TaskIsWaitingForExclusiveAccess())   // next one would be a writer -> leave
            {
                free_lock();
                break;
            }
            else
            {
                m_waitQueue.removeFirst();
                m_waitCounter--;
                m_nonExclusive++;
                if(&(pNextTaskObject->TaskScheduler()) != &(pTaskObject->TaskScheduler()))
                {
                    activateTaskInOtherUKT(pTaskObject,pNextTaskObject);
                }
                else
                {
//http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129805:
// activate tasks in my own task scheduler, too
                    activateTaskInMyUKT(pTaskObject,pNextTaskObject,taskFoundWithHigherPriority);
                }
            }
        }
    }
    while(1);
    if ( taskFoundWithHigherPriority )
    {
        pTaskObject->YieldTask();
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalist::tryEnterRegionExclusiveNonBlocking(RTETask_Task *pTaskObject)
{
    get_lock(pTaskObject,false,m_tasCounter);
    if(m_nonExclusive != 0) // the region has been locked ("exclusive" or "non-exclusive") -> can't enter
    {
        free_lock();
        return false;
    }
    else
    {
        m_exclusiveRequests++;
        m_nonExclusive = -1;
        m_lastEntered = pTaskObject->ID();
        pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()++;
        free_lock();
        return true;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalist::tryEnterRegionNonExclusiveNonBlocking(RTETask_Task *pTaskObject)
{
    get_lock(pTaskObject,false,m_tasCounter);
    if(m_nonExclusive < 0)  // the region has been locked "exclusive" -> can't enter
    {
        free_lock();
        return false;
    }
    else
    {
        m_nonExclusive++;
        m_lastEntered = pTaskObject->ID();
        free_lock();
        return true;
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalist::activateTaskInMyUKT(RTETask_Task *pTaskObject,RTETask_Task *pNextTaskObject,SAPDB_Bool &taskFoundWithHigherPriority)
{
    if(pNextTaskObject->PrepareEnqueueOfInterruptibleRequest())
    {
        RTETask_CommonQueueElement &element = pNextTaskObject->CommonQueueElementFromFreeList();
        element.Init(RTETask_CommonQueueElement::LeaveRWRegion);

        pNextTaskObject->EnqueueIntoReadyToRunQueue(element,*pNextTaskObject);

        if( pNextTaskObject->LegacyTaskCtrl().PrioFlag() > pTaskObject->LegacyTaskCtrl().PrioFlag())
        {
            taskFoundWithHigherPriority = true;
        }
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalist::activateTaskInOtherUKT(RTETask_Task *pTaskObject,RTETask_Task *pNextTaskObject)
{
#ifdef _WIN32
    sql74k_UToU_enqu_and_wake (
        (pTaskObject->TaskScheduler()).LegacyUKTCtrl(),
        pTaskObject->LegacyTaskCtrl(),
        (pNextTaskObject->TaskScheduler()).LegacyUKTCtrl(),
        pNextTaskObject->LegacyTaskCtrl(),
        RTESync_RWRegionLeaveRemoteFunction, 0 );
#else
    en71EnqueueUkt2UktAndWake (
        (pTaskObject->TaskScheduler()).LegacyUKTCtrl(),
        pNextTaskObject->LegacyTaskCtrl(),
        pTaskObject->LegacyTaskCtrl(),
        RTESync_RWRegionLeaveRemoteFunction,
        (void *)pNextTaskObject->LegacyTaskCtrl().InRegionID() );
#endif
    if ( KGS->fBusyWaitingForbidden )
    {
        RTE_ISystem::DoSleep( 0 ); // reschedule
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTESync_IRWRegion::sizeOf(RTESync_IRWRegion::flavours flavour)
{
    switch(flavour)
    {
    case RTESync_IRWRegion::WithSemalist:
        return sizeof(RTESync_RWRegionWithSemalist);
    case RTESync_IRWRegion::WithSemalistNoStatistics:
        return sizeof(RTESync_RWRegionWithSemalistNoStatistics);
    case RTESync_IRWRegion::WithoutSemalist:
        return sizeof(RTESync_RWRegionNoSemalist);
    default:
        return 0;
    }
}


RTESync_RWRegionWithSemalistNoStatistics::RTESync_RWRegionWithSemalistNoStatistics(SAPDB_Int8 id,RTESync_SpinlockPool &pool)
    : RTESync_RWRegion(id,pool)
{
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalistNoStatistics::enterRegionExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)
{
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()++;
    SAPDB_ULong tasCounter;
    do
    {
        get_lock(pTaskObject,false,tasCounter);
        if(m_nonExclusive != 0)   // not free?
        {
            collisionCounter++;
            if(collisionCounter >= maxCollisions)
                break;
            free_lock();

            pTaskObject->YieldTask(/*false*/);
        }
        else
        {
            break;
        }
    }
    while(true);
    m_exclusiveRequests++;
    if(m_nonExclusive != 0)
    {
        m_waitQueue.insert(true,pTaskObject);
        free_lock();
        RTETask_CommonQueueElement &returnElement = pTaskObject->WaitForReschedule(cancelable,timeout);

        switch( returnElement.GetType())
        {
        case RTETask_CommonQueueElement::CancelTask : // task was cancelled:
        case RTETask_CommonQueueElement::RequestTimedOut : // task was cancelled:
            returnElement.Release();
            get_lock(pTaskObject,false,tasCounter);
            m_waitQueue.remove(pTaskObject);    // remove task from the wait queue
            pTaskObject->ResetPendingCancel();         // reset the "cancel pending" flag
            pTaskObject->EndOfInterruptibleRequest();
            free_lock();
            return false;
            break;
        case RTETask_CommonQueueElement::LeaveRWRegion:  // the expected one: go ahead!
            break;
        default:    // any other requests are badly wrong
            RTE_Crash(Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"enterRegionExclusive got wrong request:",SAPDB_ToString(returnElement.GetType())));
        }
        returnElement.Release();
    }
    else
    {
        m_nonExclusive = -1;
        m_lastEntered = pTaskObject->ID();
        free_lock();
    }

    pTaskObject->EndOfInterruptibleRequest();
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalistNoStatistics::enterRegionNonExclusive(RTETask_Task *pTaskObject,SAPDB_Bool cancelable,SAPDB_ULong timeout,SAPDB_Long maxCollisions,SAPDB_Long &collisionCounter,RTE_TaskId pid)
{
    SAPDB_ULong tasCounter;
    do
    {
        get_lock(pTaskObject,false,tasCounter);
        if(m_nonExclusive < 0)   // not free?
        {
            collisionCounter++;
            if(collisionCounter >= maxCollisions)
                break;
            free_lock();

            pTaskObject->YieldTask(/*false*/);
        }
        else
        {
            break;
        }
    }
    while(true);
    if(m_nonExclusive < 0 || m_exclusiveRequests > 0)
    {
        m_waitQueue.insert(false,pTaskObject);
        free_lock();

        RTETask_CommonQueueElement &returnElement = pTaskObject->WaitForReschedule(cancelable,timeout);

        switch( returnElement.GetType())
        {
        case RTETask_CommonQueueElement::CancelTask : // task was cancelled:
        case RTETask_CommonQueueElement::RequestTimedOut : // task was cancelled:
            returnElement.Release();
            get_lock(pTaskObject,false,tasCounter);
            m_waitQueue.remove(pTaskObject);    // remove task from the wait queue
            pTaskObject->ResetPendingCancel();         // reset the "cancel pending" flag
            pTaskObject->EndOfInterruptibleRequest();
            free_lock();
            return false;
            break;
        case RTETask_CommonQueueElement::LeaveRWRegion:  // the expected one: go ahead!
            break;
        default:    // any other requests are badly wrong
            RTE_Crash(Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"enterRegionExclusive got wrong request:",SAPDB_ToString(returnElement.GetType())));
        }
        returnElement.Release();
    }
    else
    {
        m_nonExclusive++;
        m_lastEntered = pTaskObject->ID();
        if(0 == m_nonExclusive)
        {
            SAPDB_Char msg[256];
            SAPDB_sprintf(msg,256," m_nonExclusive is 0 after enter (non-Exclusive)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
            RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
        }
        free_lock();
    }

    pTaskObject->EndOfInterruptibleRequest();
    return true;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalistNoStatistics::leaveRegionExclusive(RTETask_Task *pTaskObject)
{
    SAPDB_ULong tasCounter;
    SAPDB_Bool readerActivated = false;   // indicate that at least one reader has been activated. In this case, no writer must be activated
    get_lock(pTaskObject,false,tasCounter);
    if(0 <= m_nonExclusive)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_LEAVE_WITHOUT_ENTER,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(m_id)));
    }
    m_nonExclusive = 0;
    m_exclusiveRequests--;
    m_lastEntered = -1;     // at first assume that no task can be activated

    SAPDB_Bool taskFoundWithHigherPriority = false;
    do
    {
        if(m_waitQueue.empty()) // no tasks that could be activated -> leave
        {
            free_lock();
            break;
        }
        else
        {
            RTETask_Task *pNextTaskObject = m_waitQueue.getFirst();
            if (readerActivated && pNextTaskObject->TaskIsWaitingForExclusiveAccess())   // already readers activated and the next one would be a writer -> leave
            {
                free_lock();
                break;
            }
            else
            {
                m_waitQueue.removeFirst();
                if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                {
                    m_nonExclusive = -1;
                }
                else
                {
                    m_nonExclusive++;
                    if(0 == m_nonExclusive)
                    {
                        SAPDB_Char msg[256];
                        SAPDB_sprintf(msg,256," m_nonExclusive is 0 after enter-after-leave (Exclusive)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
                        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
                    }
                }
                m_lastEntered = pNextTaskObject->ID();
                if(&(pNextTaskObject->TaskScheduler()) == &(pTaskObject->TaskScheduler()))   // the waiting task is in my UKT -> activate it (incl. a goto_disp()) and leave
                {
                    activateTaskInMyUKT(pTaskObject,pNextTaskObject,taskFoundWithHigherPriority);
                    if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        readerActivated = true;
                    }
                }
                else
                {
                    SAPDB_Bool nextTaskIsWaitingForExclusiveAccess = pNextTaskObject->TaskIsWaitingForExclusiveAccess();
                    activateTaskInOtherUKT(pTaskObject,pNextTaskObject);
                    if(nextTaskIsWaitingForExclusiveAccess)
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        // in fact, this is the only case in which the loop is continued: a reader task has been activated in another UKT. In this case, other reader tasks can also be activated
                        readerActivated = true;
                    }
                }
            }
        }
    }
    while(1);
    if ( taskFoundWithHigherPriority )
    {
        pTaskObject->YieldTask();
    }
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()--;
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalistNoStatistics::leaveRegionNonExclusive(RTETask_Task *pTaskObject)
{
    SAPDB_Bool readerActivated = false;   // indicate that at least one reader has been activated. In this case, no writer must be activated
    SAPDB_ULong tasCounter;
    get_lock(pTaskObject,false,tasCounter);
    if(0 >= m_nonExclusive)
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_LEAVE_WITHOUT_ENTER,SAPDB_ToString(pTaskObject->ID()),SAPDB_ToString(m_id)));
    }
    m_nonExclusive--;
    m_lastEntered = -1;     // at first assume that no task can be activated

    SAPDB_Bool taskFoundWithHigherPriority = false;
    do
    {
        if( m_waitQueue.empty()) // no tasks that could be activated -> leave
        {
            free_lock();
            break;
        }
        else
        {
            RTETask_Task *pNextTaskObject = m_waitQueue.getFirst();
            if(( ( m_nonExclusive != 0 ) && pNextTaskObject->TaskIsWaitingForExclusiveAccess() )   // there is still someone in the region and the next one would be a writer -> leave
               ||( readerActivated && pNextTaskObject->TaskIsWaitingForExclusiveAccess()))   // already readers activated and the next one would be a writer -> leave
            {
                free_lock();
                break;
            }
            else
            {
                m_waitQueue.removeFirst();
                if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                    m_nonExclusive = -1;
                else
                {
                    m_nonExclusive++;
                    if(0 == m_nonExclusive)
                    {
                        SAPDB_Char msg[256];
                        SAPDB_sprintf(msg,256," m_nonExclusive is 0 after enter-after-leave (non-Exclusive)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
                        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
                    }
                }
                m_lastEntered = pNextTaskObject->ID();
                if(&(pNextTaskObject->TaskScheduler()) == &(pTaskObject->TaskScheduler()))   // the waiting task is in my UKT -> activate it (incl. a goto_disp()) and leave
                {
                    activateTaskInMyUKT(pTaskObject,pNextTaskObject,taskFoundWithHigherPriority);
                    if(pNextTaskObject->TaskIsWaitingForExclusiveAccess())
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        readerActivated = true;
                    }
                }
                else
                {
                    SAPDB_Bool nextTaskIsWaitingForExclusiveAccess = pNextTaskObject->TaskIsWaitingForExclusiveAccess();
                    activateTaskInOtherUKT(pTaskObject,pNextTaskObject);
                    if(nextTaskIsWaitingForExclusiveAccess)
                    {
                        free_lock();
                        break;
                    }
                    else
                    {
                        // in fact, this is the only case in which the loop is continued: a reader task has been activated in another UKT. In this case, other reader tasks can also be activated
                        readerActivated = true;
                    }
                }
            }
        }
    }
    while(1);
    if ( taskFoundWithHigherPriority )
    {
        pTaskObject->YieldTask();
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalistNoStatistics::SwitchNonExclusive(RTETask_Task *pTaskObject)
{
    SAPDB_ULong tasCounter;
    get_lock(pTaskObject,false,tasCounter);
    if(-1 != m_nonExclusive)
    {
        SAPDB_Char msg[256];
        SAPDB_sprintf(msg,256,"SwitchToNonExclusiveAccess() called but m_nonExclusive is != -1 (%d)!!(pid is %d, region id is %d)\n",pTaskObject->ID(),m_id);
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_RWREGION_INCONSISTENCY,msg));
    }
    m_nonExclusive = 1;
    m_exclusiveRequests--;
    pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()--;

    SAPDB_Bool taskFoundWithHigherPriority = false;
    do
    {
        if(m_waitQueue.empty()) // no tasks that could be activated -> leave
        {
            free_lock();
            break;
        }
        else
        {
            RTETask_Task *pNextTaskObject = m_waitQueue.getFirst();
            if (pNextTaskObject->TaskIsWaitingForExclusiveAccess())   // next one would be a writer -> leave
            {
                free_lock();
                break;
            }
            else
            {
                m_waitQueue.removeFirst();
                m_nonExclusive++;
                if(&(pNextTaskObject->TaskScheduler()) != &(pTaskObject->TaskScheduler()))
                {
                    activateTaskInOtherUKT(pTaskObject,pNextTaskObject);
                }
                else
                {
                    free_lock();
//http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1129805:
// activate tasks in my own task scheduler, too
                    activateTaskInMyUKT(pTaskObject,pNextTaskObject,taskFoundWithHigherPriority);

                    break;
                }
            }
        }
    }
    while(1);
    if ( taskFoundWithHigherPriority )
    {
        pTaskObject->YieldTask();
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalistNoStatistics::tryEnterRegionExclusiveNonBlocking(RTETask_Task *pTaskObject)
{
    SAPDB_ULong tasCounter;
    get_lock(pTaskObject,false,tasCounter);
    if(m_nonExclusive != 0) // the region has been locked ("exclusive" or "non-exclusive") -> can't enter
    {
        free_lock();
        return false;
    }
    else
    {
        m_exclusiveRequests++;
        m_nonExclusive = -1;
        m_lastEntered = pTaskObject->ID();
        pTaskObject->LegacyTaskCtrl().ExclusiveNestingCount()++;
        free_lock();
        return true;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionWithSemalistNoStatistics::tryEnterRegionNonExclusiveNonBlocking(RTETask_Task *pTaskObject)
{
    SAPDB_ULong tasCounter;
    get_lock(pTaskObject,false,tasCounter);
    if(m_nonExclusive < 0)  // the region has been locked "exclusive" -> can't enter
    {
        free_lock();
        return false;
    }
    else
    {
        m_nonExclusive++;
        m_lastEntered = pTaskObject->ID();
        free_lock();
        return true;
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalistNoStatistics::activateTaskInMyUKT(RTETask_Task *pTaskObject,RTETask_Task *pNextTaskObject,SAPDB_Bool &taskFoundWithHigherPriority)
{
    if(pNextTaskObject->PrepareEnqueueOfInterruptibleRequest())
    {
        RTETask_CommonQueueElement &element = pNextTaskObject->CommonQueueElementFromFreeList();
        element.Init(RTETask_CommonQueueElement::LeaveRWRegion);

        pNextTaskObject->EnqueueIntoReadyToRunQueue(element,*pNextTaskObject);

        if( pNextTaskObject->LegacyTaskCtrl().PrioFlag() > pTaskObject->LegacyTaskCtrl().PrioFlag())
        {
            taskFoundWithHigherPriority = true;
        }
    }
}

/*---------------------------------------------------------------------------*/

void RTESync_RWRegionWithSemalistNoStatistics::activateTaskInOtherUKT(RTETask_Task *pTaskObject,RTETask_Task *pNextTaskObject)
{
#ifdef _WIN32
    sql74k_UToU_enqu_and_wake (
        (pTaskObject->TaskScheduler()).LegacyUKTCtrl(),
        pTaskObject->LegacyTaskCtrl(),
        (pNextTaskObject->TaskScheduler()).LegacyUKTCtrl(),
        pNextTaskObject->LegacyTaskCtrl(),
        RTESync_RWRegionLeaveRemoteFunction/*sql55k_remote_vendexcl*/, 0 );
#else
    en71EnqueueUkt2UktAndWake (
        (pTaskObject->TaskScheduler()).LegacyUKTCtrl(),
        pNextTaskObject->LegacyTaskCtrl(),
        pTaskObject->LegacyTaskCtrl(),
        RTESync_RWRegionLeaveRemoteFunction/*e55_remactive*/,
        (void *)pNextTaskObject->LegacyTaskCtrl().InRegionID() );
#endif
    if ( KGS->fBusyWaitingForbidden )
    {
        RTE_ISystem::DoSleep( 0 ); // reschedule
    }
}

/*---------------------------------------------------------------------------*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

/*!
    the factory method used to obtain a RWRegion
*/

RTESync_IRWRegion *RTESync_CreateRWRegion(SAPDB_Int8 id,RTESync_SpinlockPool &pool,SAPDBMem_IRawAllocator &allocator,RTESync_IRWRegion::flavours flavour)
{
    RTESync_RWRegion *newRWRegion;
    if(RTESync_IRWRegion::WithSemalist == flavour)
        newRWRegion = new (allocator) RTESync_RWRegionWithSemalist (id,pool);
    else if(RTESync_IRWRegion::WithSemalistNoStatistics == flavour)
        newRWRegion = new (allocator) RTESync_RWRegionWithSemalistNoStatistics (id,pool);
    else
        newRWRegion = new (allocator) RTESync_RWRegionNoSemalist   (id,pool);
    return newRWRegion;
}

/*!
    and the counterpart to destroy a RWRegion created by the routine above
*/

void RTESync_DestroyRWRegion(RTESync_IRWRegion *region,SAPDBMem_IRawAllocator &allocator)
{
    ((RTESync_RWRegion *)region)->~RTESync_RWRegion();
    allocator.Deallocate(region);
}
/* -----------------------------------------------------------------------*/
/*! chapter: Constructors and initialization                              */
/* -----------------------------------------------------------------------*/

SAPDB_Bool RTESync_RWRegionRegister::FillStatisticInfo(StatisticInfo * &pInfoArray, RTE_TaskId *pWaitingTasks,SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount,RTESync_IRWRegionReader::Mode mode,RTESync_IRWRegionReader::Selection  selection)
{
    RTESync_LockedScope lock(Spinlock());
    SAPDB_UInt4 currentWaitingTaskIndex=0;

    if ( this->m_Count > maxCount )
    {
        return false;
    }
    itemCount = 0;
    StatisticInfo *pFill = pInfoArray;

    for ( RTE_ItemRegister<RTESync_IRWRegion>::Info * pInfo = this->m_First; 0 != pInfo; pInfo = pInfo->GetSuccessor() )
    {
        RTESync_RWRegion *pCurrent = (RTESync_RWRegion *)&(pInfo->GetItem());
        if( (selection == RTESync_IRWRegionReader::all) || (pCurrent->m_nonExclusive != 0)/*(pCurrent->m_lastEntered != -1)*/ )
        {
            pFill->ExclusiveAccessCount     = pCurrent->GetExclusiveAccessCounter();
            pFill->NonExclusiveAccessCount  = pCurrent->GetNonExclusiveAccessCounter();
            pFill->CollisionCount           = pCurrent->GetCollisionCounter();
            pFill->WaitCount                = pCurrent->GetWaitCounter();
            pFill->TasCount                 = pCurrent->GetTasCounter();
            pFill->CurrentAccesses          = pCurrent->m_nonExclusive;
            if(RTESync_IRWRegionReader::reset == mode)
            {
                pFill->ExclusiveAccessCount     -= pCurrent->GetExclusiveAccessCounterAtReset();
                pFill->NonExclusiveAccessCount  -= pCurrent->GetNonExclusiveAccessCounterAtReset();
                pFill->CollisionCount           -= pCurrent->GetCollisionCounterAtReset();
                pFill->WaitCount                -= pCurrent->GetWaitCounterAtReset();
                pFill->CollisionCount           -= pCurrent->GetTasCounterAtReset();
            }
            strcpy((SAPDB_Char *)pFill->PoolName, (SAPDB_Char *)pCurrent->m_pool.Name());
            pFill->Index = pCurrent->getId();
            pFill->TaskId = pCurrent->m_lastEntered;
            pFill->numberOfWaitingTasks=0;
            if(pCurrent->GetWaitQueue())
            {
                RTETask_Task *pCurrenWaitQueueEntry = pCurrent->GetWaitQueue()->getFirst();
                if(pCurrenWaitQueueEntry)
                {
                    pFill->indexOfFirstWaitingTask = currentWaitingTaskIndex;
                    while(pCurrenWaitQueueEntry)
                    {
                        if( currentWaitingTaskIndex < KGS->ulNumOfTasks )
                        {
                            pWaitingTasks[currentWaitingTaskIndex] = pCurrenWaitQueueEntry->ID();
                        }
                        currentWaitingTaskIndex++;
                        pFill->numberOfWaitingTasks++;
                        pCurrenWaitQueueEntry = pCurrenWaitQueueEntry->NextTaskInWaitQueue();
                    }
                }
            }
            pFill->Address = (SAPDB_ULong)pCurrent;
            pFill++;
            itemCount++;
        }
    }
    return true;

}

void RTESync_RWRegionRegister::ResetAllRegisteredElements(void)
{
    RTESync_LockedScope lock(Spinlock());
    SAPDB_UInt4 currentWaitingTaskIndex=0;


    for ( RTE_ItemRegister<RTESync_IRWRegion>::Info * pInfo = this->m_First; 0 != pInfo; pInfo = pInfo->GetSuccessor() )
    {
        RTESync_IRWRegion *pCurrent = &(pInfo->GetItem());
        pCurrent->Reset();
        
    }
}


class RTESync_RWRegionReaderForSpecificRWRegionRegister : public RTESync_IRWRegionReader
{

public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    RTESync_RWRegionReaderForSpecificRWRegionRegister ( RTESync_RWRegionRegister &rWRegionRegister,SAPDBMem_IRawAllocator &allocator )
        : m_RWRegionRegister(rWRegionRegister)
        , m_allocator (allocator)
    {
    }

    ~RTESync_RWRegionReaderForSpecificRWRegionRegister (void)
    {
        if(m_pWaitingTasks)
            m_allocator.Deallocate(m_pWaitingTasks);
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Iteration                                                    */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool First (
        RTESync_IRWRegionReader::Mode       mode        = RTESync_IRWRegionReader::normal,
        RTESync_IRWRegionReader::Selection  selection   = RTESync_IRWRegionReader::all
    )
    {
        m_selection = selection;
        m_mode      = mode;
        if ( m_RWRegionRegister.GetStatisticInfo(m_pInfoArray, m_pWaitingTasks,m_Elements,mode,selection) )
        {
            m_Current = -1;
            return (true);
        }
        return (false);
    }

    SAPDB_Bool Next (void)
    {
        m_Current ++;

        if ((!m_pInfoArray) || (m_Current >= m_Elements))
        {
            FreeArray();
            return (false);
        }
        else
        {
            return (true);
        }
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Iteration                                                 */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Data Access                                                  */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool SpinlockPoolName         (SAPDB_UTF8 name[])
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            strcpy((SAPDB_Char *)name, (SAPDB_Char *)m_pInfoArray[m_Current].PoolName);
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool Address                  (SAPDB_UInt8 &address)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            address = m_pInfoArray[m_Current].Address;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool Index                   (SAPDB_UInt8 &index)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            index = m_pInfoArray[m_Current].Index;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool CollisionCount           (SAPDB_UInt8 &collisioncount)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            collisioncount = m_pInfoArray[m_Current].CollisionCount;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool WaitCount           (SAPDB_UInt8 &waitcount)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            waitcount = m_pInfoArray[m_Current].WaitCount;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool SpinLockCollisionCount   (SAPDB_UInt8 &tascount)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            tascount = m_pInfoArray[m_Current].TasCount;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool ExclusiveAccessCount     (SAPDB_UInt8 &exclusiveaccesscount)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            exclusiveaccesscount = m_pInfoArray[m_Current].ExclusiveAccessCount;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool NonExclusiveAccessCount  (SAPDB_UInt8 &nonexclusiveaccesscount)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            nonexclusiveaccesscount = m_pInfoArray[m_Current].NonExclusiveAccessCount;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool LastEnteredTask          (SAPDB_Int4 &taskId)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            taskId = m_pInfoArray[m_Current].TaskId;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool CurrentAccesses          (SAPDB_Int4 &currentAccesses)
    {
        if ((m_pInfoArray) && (m_Current < m_Elements))
        {
            currentAccesses = m_pInfoArray[m_Current].CurrentAccesses;
            return (true);
        }
        else
        {
            return (true);
        }
    }

    SAPDB_Bool GetNextWaitingTask       (SAPDB_Int4 &taskId)
    {
        if( m_pInfoArray[m_Current].numberOfWaitingTasks > 0 )
        {
            taskId = m_pWaitingTasks[m_pInfoArray[m_Current].indexOfFirstWaitingTask];
            m_pInfoArray[m_Current].indexOfFirstWaitingTask++;
            --m_pInfoArray[m_Current].numberOfWaitingTasks;
            return (true);
        }
        else
        {
            return (false);
        }
    }


    /* -----------------------------------------------------------------------*/
    /*! endchapter: Data Access                                               */
    /* -----------------------------------------------------------------------*/


private:

    void FreeArray (void)
    {
        if (m_pInfoArray)
        {
            m_RWRegionRegister.FreeStatisticInfo(m_pInfoArray,m_pWaitingTasks);
            m_pInfoArray  = NULL;
            m_Elements    = 0;
            m_Current     = 0;
        }
    };

    RTESync_RWRegionRegister::StatisticInfo *m_pInfoArray;
    SAPDB_Int4                               m_Elements;
    SAPDB_Int4                               m_Current;
private:
    RTESync_IRWRegionReader::Mode            m_mode;
    RTESync_IRWRegionReader::Selection       m_selection;
    RTESync_RWRegionRegister                &m_RWRegionRegister;
    SAPDBMem_IRawAllocator                  &m_allocator;
    RTE_TaskId                              *m_pWaitingTasks;
};

RTESync_IRWRegionReader *RTESync_GetRWRegionReader(RTESync_RWRegionRegister &rWRegionRegister,SAPDBMem_IRawAllocator &allocator)
{
    return new (allocator) RTESync_RWRegionReaderForSpecificRWRegionRegister( rWRegionRegister, allocator );
}

/*---------------------------------------------------------------------------*/
/*!
    The following code can be used to redirect calls to vbegexcl()/vendexcl()/visexcl()
    to RWRegions for testing purposes. To use it, replace the vbegexcl() code with:

     void RTESync_RWRegionvbegexcl ( tsp00_TaskId pid, tsp00_RegionId sid  );
     void vbegexcl ( tsp00_TaskId pid, tsp00_RegionId sid  )
     {
        RTESync_RWRegionvbegexcl ( pid, sid  );
     }

     and the code for vendexcl() and visexcl() the same way.
*/
class RTESync_RegionSimulator
{
public:
    static RTESync_RegionSimulator &Instance()
    {
        if(NULL == m_Instance)
        {
            m_Instance = new (RTEMem_RteAllocator::Instance()) RTESync_RegionSimulator;
        }
        return *m_Instance;
    }
    RTESync_IRWRegion    **m_Regions;
private:
    RTESync_RegionSimulator()
    {
        m_Spinlocks = new (RTEMem_RteAllocator::Instance()) RTESync_SpinlockPool((SAPDB_UTF8 *)"RegionSimulationPool",XPARAM(ulNoOfRegions));
        m_Regions = (RTESync_IRWRegion **)(RTEMem_RteAllocator::Instance()).Allocate( sizeof(RTESync_RWRegion*) * XPARAM(ulNoOfRegions) );
        for(SAPDB_UInt4 i=0 ; i<XPARAM(ulNoOfRegions); i++)
        {
            m_Regions[i] = RTESync_CreateRWRegion(i+1, *m_Spinlocks, RTEMem_RteAllocator::Instance(),USE_SEMALIST);
        }
    }
    static RTESync_RegionSimulator *m_Instance;
    RTESync_SpinlockPool    *m_Spinlocks;
};
RTESync_RegionSimulator *RTESync_RegionSimulator::m_Instance;

/*---------------------------------------------------------------------------*/

extern "C" void RTESync_RWRegionvbegexcl ( tsp00_TaskId pid, tsp00_RegionId sid  )
{
    (RTESync_RegionSimulator::Instance()).m_Regions[sid-1]->enter(true,pid);
}

/*---------------------------------------------------------------------------*/

extern "C" void RTESync_RWRegionvendexcl ( tsp00_TaskId pid, tsp00_RegionId sid )
{
    (RTESync_RegionSimulator::Instance()).m_Regions[sid-1]->leave(true,pid);
}

/*---------------------------------------------------------------------------*/

extern "C" BOOLEAN RTESync_RWRegionvisexcl ( tsp00_TaskId pid, tsp00_RegionId  sid )
{
    if(-1 == sid)   /* dummy region */
        return true;

    if ( sid < 1 || sid > (tsp00_RegionId)XPARAM(ulNoOfRegions) )
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_SYNC_VISEXCL_INVALID_REGION,SAPDB_ToString(pid),SAPDB_ToString(sid)));
    }
    if(pid == (RTESync_RegionSimulator::Instance()).m_Regions[sid-1]->getLastEntered())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*---------------------------------------------------------------------------*/

