/*!========================================================================

  @file         RTETask_TCB.hpp
  @ingroup      Runtime
  @author       RobinW

  @brief        Task and User Kernel Thread control block access classes

                Hiding historic naming conventions in Windows and UNIX Runtime
                structures...

  @since        2005-01-10  18:33
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

#ifndef RTE_TASK_TCB_HPP
#define RTE_TASK_TCB_HPP

#if defined (_WIN32)
#include "gos00k.h" /* nocheck */
#else
#define USE_KGS
#include "gen50.h" /* nocheck */
#include "gen71.h" /* nocheck */
#include "geo002.h" /* nocheck */
#endif

#include "RunTime/RTE_Types.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"

#ifdef WIN32
/*!
  @class RTE_TCB
  @brief Task control block wrapper class
 */
class RTE_TCB
{
public:
    RTE_RegionId &inRegionID(void)
    {
        return (RTE_RegionId &)(m_pTaskCtrlRec->Sid);
    }

    SAPDB_UInt1 &taskType(void)           
    {
        return m_pTaskCtrlRec->TaskType;
    }

    volatile SAPDB_Int4 &taskState(void)          
    {
        return m_pTaskCtrlRec->TaskState;
    }

    PTASK_CTRL_REC &nextTask(void)           
    {
        return     m_pTaskCtrlRec->pNextInTaskList;
    }

    PTASK_CTRL_REC &nextTaskInWaitQueue(void)           
    {
        return     m_pTaskCtrlRec->nextTaskInWaitQueue;
    }
 
    PUKT_CTRL_REC &ukt(void)                 
    {
        return m_pTaskCtrlRec->pUKT;
    }

    SAPDB_UInt4 &taskIndex(void)          
    {
        return (SAPDB_UInt4 &)m_pTaskCtrlRec->ulTaskIndex;
    }

    SAPDB_UInt8 &beginExclusiveCount(void)         
    {
        return m_pTaskCtrlRec->ulBegExclCnt;
    }

    SAPDB_ULong &exclusiveNestingCount(void)    
    {
        return m_pTaskCtrlRec->lNestingExcl;
    }

    SAPDB_UInt1 &prioFlag(void)           
    {
        return m_pTaskCtrlRec->PrioFlag;
    }

    SAPDB_UInt8 &selfSuspendCount(void)
    {
        return m_pTaskCtrlRec->ulSelfSuspCount;
    }
    
    SAPDB_UInt8 &beginExclusiveCalls(void)
    {
        return m_pTaskCtrlRec->ulBegExclCalls;
    }

    SAPDB_Long &regionCollisionCounter(SAPDB_Int8 id)
    {
        return m_dummy;
    }

    ULONG &requestType(void)
    {
        return GetTaskDLQElement1(m_pTaskCtrlRec)->ulReqType;
    }

    DLQ_REC &queueElem(void)
    {
        return *GetTaskDLQElement1(m_pTaskCtrlRec);
    }

    RTE_TCB(PTASK_CTRL_REC pTaskCtrlRec = NULL)
        : m_pTaskCtrlRec(pTaskCtrlRec)
    {
    }

    operator PTASK_CTRL_REC() const
    {
        return m_pTaskCtrlRec;
    }

    operator TASK_CTRL_REC&() const
    {
        return *m_pTaskCtrlRec;
    }

private:
    PTASK_CTRL_REC m_pTaskCtrlRec;
    static SAPDB_Long m_dummy;
};

/*!
  @class RTE_UKT
  @brief User kernel thread control block wrapper class
 */
class RTE_UKT
{
public:
    PTASK_CTRL_REC activeTask(void)
    {
        return (m_pUKTCtrlRec)->pCTask;
    }

    ULONG &selfDispatchCounter(void)
    {
        return (m_pUKTCtrlRec)->ulImmediateDispatch;
    }

    ULONG &activeTasks(void)
    {
        return (m_pUKTCtrlRec)->ulActiveTasks;
    }

    ULONG &numRegWait(void)
    {
        return (m_pUKTCtrlRec)->ulNumRegWait;
    }

    operator PUKT_CTRL_REC()
    {
        return m_pUKTCtrlRec;
    }

    operator UKT_CTRL_REC&() const
    {
        return *m_pUKTCtrlRec;
    }

    RTE_UKT(PUKT_CTRL_REC pUKTCtrlRec = NULL)
        : m_pUKTCtrlRec(pUKTCtrlRec)
    {
    }
private:
    PUKT_CTRL_REC m_pUKTCtrlRec;
};

#else
/*!
  @class RTE_TCB
  @brief Task control block wrapper class
 */
class RTE_TCB
{
public:

    void AddDynPrio(void)
    {
        m_pTaskCtrlRec->ulDynPrio += PRIO_FACTOR;
    }
    
    SAPDB_UInt4 &inRegionID(void)
    {
        return m_pTaskCtrlRec->sid;
    }

    volatile SAPDB_Int4 &taskType(void)           
    {
        return m_pTaskCtrlRec->type;
    }

    volatile SAPDB_Int4 &taskState(void)          
    {
        return m_pTaskCtrlRec->state;
    }

    TASK_TYPE *&nextTask(void)           
    {
        return     m_pTaskCtrlRec->next_task;
    }

    TASK_TYPE *&nextTaskInWaitQueue(void)           
    {
        return     m_pTaskCtrlRec->nextTaskInWaitQueue;
    }
 
    ten50_UKT_Control *&ukt(void)                 
    {
        return m_pTaskCtrlRec->ukt;
    }

    SAPDB_Int4 &taskIndex(void)          
    {
        return m_pTaskCtrlRec->index;
    }

    SAPDB_UInt8 &beginExclusiveCount(void)         
    {
        return m_pTaskCtrlRec->begexcl_count;
    }

    SAPDB_UInt4 &exclusiveNestingCount(void)    
    {
        return m_pTaskCtrlRec->excl_schachtel;
    }

    SAPDB_UInt4 &prioFlag(void)           
    {
        return m_pTaskCtrlRec->prio_flag;
    }

    SAPDB_UInt8 &selfSuspendCount(void)     
    {
        return m_pTaskCtrlRec->self_susp_cnt;
    }

    SAPDB_UInt8 &beginExclusiveCalls(void)
    {
        return m_pTaskCtrlRec->ulBegExclCalls;
    }
    
    SAPDB_Bool &taskIsWaitingForExclusiveAccess(void)
    {
        return m_pTaskCtrlRec->fTaskIsWaitingForExclusiveAccess;
    }

    SAPDB_UInt4 &requestType(void)
    {
        return GetTaskDLQElement1(m_pTaskCtrlRec)->req_type;
    }

    struct DOUBLY_LINKED  &queueElem(void)
    {
        return *GetTaskDLQElement1(m_pTaskCtrlRec);
    }

    SAPDB_Int &regionCollisionCounter(SAPDB_Int8 id)
    {
        return m_pTaskCtrlRec->reg_coll_cnt[id];
    }

    operator TASK_TYPE*()
    {
        return m_pTaskCtrlRec;
    }

    operator TASK_TYPE&() const
    {
        return *m_pTaskCtrlRec;
    }

    RTE_TCB(TASK_TYPE *pTaskCtrlRec = NULL)
        : m_pTaskCtrlRec(pTaskCtrlRec)
    {
    }
private:
    TASK_TYPE *m_pTaskCtrlRec;
};

/*!
  @class RTE_UKT
  @brief User kernel thread control block wrapper class
 */
class RTE_UKT
{
public:
    SAPDB_UInt4 &activeTasks(void)
    {
        return (m_pUKTCtrlRec)->activeTasks;
    }

    TASK_TYPE *activeTask(void)
    {
        return (m_pUKTCtrlRec)->curr_task;
    }

    SAPDB_UInt4  &selfDispatchCounter(void)
    {
        return (m_pUKTCtrlRec)->SelfDispatch;
    }

    operator ten50_UKT_Control *()
    {
        return m_pUKTCtrlRec;
    }

    RTE_UKT(ten50_UKT_Control *pUKTCtrlRec = NULL)
        : m_pUKTCtrlRec(pUKTCtrlRec)
    {
    }
private:
    ten50_UKT_Control *m_pUKTCtrlRec;
};
#endif

/*!
  @brief get wrapper class references for RTE_TCB and RTE_UKT and task id
 */
void RTETask_getTCBandUKTCB( RTE_TCB &TCB, RTE_UKT &UKTCB, RTE_TaskId &pid );


#endif // RTE_TASK_TCB_HPP
