/*!
  @file           geo50_0.h
  @author         JoergM
  @special area   OS dependend include file for LZU kernel specific structures
  @brief          Exists only to prevent vmake from complaining too often
  @see            example.html ...

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
*/



#ifndef GEO50_0_H
#define GEO50_0_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if defined(_WIN32)
#ifdef KERNEL_LZU
#ifndef KERNEL
#define KERNEL
#endif /* KERNEL */
#endif /* KERNEL_LZU */
#include "gos00k.h" /* nocheck */
#else
#ifdef KERNEL_LZU
#ifndef USE_KGS
#define USE_KGS
#endif /* KGS */
#endif /* KERNEL_LZU */
#include "gen50.h" /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32) || defined(OS2)
#define KGS                             (&kgs)
#define UKT_CTRL                        PUKT_CTRL_REC
#define TASK_CTRL                       PTASK_CTRL_REC
#define TASK_TYPE(_pTaskCtrl)           (_pTaskCtrl)->TaskType
#define TASK_STATE(_pTaskCtrl)          (_pTaskCtrl)->TaskState
#define ACTIVE_TASK(_pUKT)              (_pUKT)->pCTask
#define UKT(_pTaskCtrl)                 (_pTaskCtrl)->pUKT
#define CURR_TASK                       (THIS_UKT_CTRL)->pCTask
#define CURR_TASK_STATE                 (THIS_UKT_CTRL)->pCTask->TaskState

/*---------------------------------------------------------------------------------
 *  Macro for retrieving a task control block from a task index
 */
#if (COMPILEMODE_MEO00 >= SLOW_MEO00) || defined(DEVELOP_CHECKS)

#define PID_TCB(pid_,tcb_) \
  {\
    PUKT_CTRL_REC  this_ukt = THIS_UKT_CTRL; \
    if ( this_ukt->pCTask->ulTaskIndex != (ULONG)pid_ ) \
    { \
      MSGALL (( ERR_VXXXX_WRONG_TASK, "PID_TCB_Mapping", pid_ )); \
      ABORT() ; \
    } \
    if ( pid_ < 1 || pid_ > (SAPDB_Long)kgs.ulNumOfTasks )\
    {\
      MSGCD (( ERR_KERN_PARAM_PID_OUT_OF_RANGE, pid_ ));\
      ABORT ();\
    }\
    tcb_ = &(kgs.pFirstTaskCtrl[pid_ - 1]) ;\
  }

#else /* COMPILEMODE_MEO00 < SLOW_MEO00 */

/*
 * Since in slow mode pid is checked, we accept it without check in fast mode...
 */
#define PID_TCB(pid_,tcb_) tcb_ = (&kgs.pFirstTaskCtrl[pid_ - 1])

#endif /* COMPILEMODE_MEO00 < SLOW_MEO00 */

#else

#define KGS                             kgs
#define UKT_CTRL                        ten50_UKT_Control *
#define TASK_CTRL                       struct TASK_TYPE *
#define TASK_TYPE(_pTaskCtrl)           (_pTaskCtrl)->type
#define TASK_STATE(_pTaskCtrl)          (_pTaskCtrl)->state
#define ACTIVE_TASK(_pTaskCtrl)         (_pTaskCtrl)->curr_task
#define UKT(_pTaskCtrl)                 (_pTaskCtrl)->ukt
#define CURR_TASK                       (THIS_UKT_CTRL)->curr_task
#define CURR_TASK_STATE                 (THIS_UKT_CTRL)->curr_task->state

typedef struct DDB4_SEMA    *PSEM_CTRL_REC ;
typedef struct N_QUEUE      *PSEM_QUEUE_REC;

/*---------------------------------------------------------------------------------
 *  Macro for retrieving a task control block from a task index
 */
#if COMPILEMODE_MEO00 >= SLOW_MEO00

#define PID_TCB(pid_,tcb_) \
  {\
    ten50_UKT_Control * this_ukt = THIS_UKT_CTRL; \
    if ( this_ukt->curr_task->index != pid_ ) \
    { \
      MSGALL (( ERR_VXXXX_WRONG_TASK, "PID_TCB_Mapping", pid_ )); \
      vabort( WRITE_CORE ) ; \
    } \
    if ( pid_ < 1 || pid_ > kgs->ulNumOfTasks )\
    {\
      MSGCD (( ERR_KERN_PARAM_PID_OUT_OF_RANGE, pid_ ));\
      vabort ( WRITE_CORE );\
    }\
    tcb_ = kgs->pFirstTaskCtrl + pid_ - 1 ;\
  }

#else /* COMPILEMODE_MEO00 < SLOW_MEO00 */

/*
 * Since in slow mode pid is checked, we accept it without check in fast mode...
 */
#define PID_TCB(pid_,tcb_) tcb_ = kgs->pFirstTaskCtrl + (pid_) - 1

#endif /* COMPILEMODE_MEO00 < SLOW_MEO00 */

#endif

#ifdef _WIN32
#define FIRST_REGION                        KGS->pSemaphores
#define TASK_INDEX(_pTaskCtrl)              (_pTaskCtrl)->ulTaskIndex
#define OWNER_TASK(_pSemCtrl)               (_pSemCtrl)->pOwnerTaskCtrl
#define REGION_NAME(_pSemCtrl)              (_pSemCtrl)->szSemaName
#define WAIT_LIST(_pSemCtrl)                (_pSemCtrl)->SemaList
#define WAIT_TASK(_pWaitElem)               (_pWaitElem)->pTaskCtrl
#define NEXT_ELEM(_pWaitElem)               (_pWaitElem)->pNextElem
#define LAST_ELEM(_pWaitList)               (_pWaitList).pRemovePos
#define REGION_ACCESS(_pSemCtrl)            (_pSemCtrl->lGetCount)
#define REGION_COLLISION(_pSemCtrl)         (_pSemCtrl->lCollisionCount)
#define REGION_WAITS(_pSemCtrl)             (_pSemCtrl->lWaitCount)
#define REGION_BEGEXCL_TAS_COLL(_pSemCtrl)  (_pSemCtrl->lBegExclTasCount)
#define REGION_ENDEXCL_TAS_COLL(_pSemCtrl)  (_pSemCtrl->lEndExclTasCount)
#else
#define FIRST_REGION                        (KGS->semaphore)
#define TASK_INDEX(_pTaskCtrl)              (_pTaskCtrl)->index
#define OWNER_TASK(_pSemCtrl)               (TASK_TYPE *)(_pSemCtrl)->owner
#define REGION_NAME(_pSemCtrl)              (_pSemCtrl)->semaname
#define WAIT_LIST(_pSemCtrl)                (_pSemCtrl)->semaliste
#define WAIT_TASK(_pWaitElem)               (_pWaitElem)->taskid
#define NEXT_ELEM(_pWaitElem)               (_pWaitElem)->next
#define LAST_ELEM(_pWaitList)               (_pWaitList).last
#define REGION_ACCESS(_pSemCtrl)            (_pSemCtrl->get_count)
#define REGION_COLLISION(_pSemCtrl)         (_pSemCtrl->collision_count)
#define REGION_WAITS(_pSemCtrl)             (_pSemCtrl->wait_count)
#define REGION_BEGEXCL_TAS_COLL(_pSemCtrl)  (_pSemCtrl->begexcl_tas_count)
#define REGION_ENDEXCL_TAS_COLL(_pSemCtrl)  (_pSemCtrl->endexcl_tas_count)
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO50_0_H */
