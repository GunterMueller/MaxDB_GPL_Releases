/*!
  @file           gen71.h
  @author         JoergM
  @special area   dispatcher
  @brief          Dispatcher and Queue Management
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



#ifndef GEN71_H
#define GEN71_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gen50.h"


/* ---- long term move task locks */
#define NEVER_MOVE_TASK                  0x0001
#define CLIENT_NOT_TASK_MOVE_ENABLED     0x0002
#define WAITING_FOR_NEW_COMMAND          0x0004
#define TASK_IS_INACTIVE                 0x0008


/* - used by 'en71AlterTaskCnt' */
#define ALL_COUNTER                0x00FF
#define ACTIVE_COUNTER             0x0001
#define TOTAL_COUNTER              0x0002

#define COUNTER_ADD                0x0100
#define COUNTER_SUB                0x0200

#define ADD_ALL_COUNTER            ALL_COUNTER | COUNTER_ADD
#define SUB_ALL_COUNTER            ALL_COUNTER | COUNTER_SUB
#define ADD_ACTIVE_COUNTER         ACTIVE_COUNTER | COUNTER_ADD
#define SUB_ACTIVE_COUNTER         ACTIVE_COUNTER | COUNTER_SUB
#define ADD_TOTAL_COUNTER          TOTAL_COUNTER | COUNTER_ADD
#define SUB_TOTAL_COUNTER          TOTAL_COUNTER | COUNTER_SUB


#define BASE_PRIO_U2U (XPARAM(lPrioBaseU2U))
#define BASE_PRIO_RAV (XPARAM(lPrioBaseRAV))
#define BASE_PRIO_IOC (XPARAM(lPrioBaseIOC))
#define BASE_PRIO_COM (XPARAM(lPrioBaseCOM))
#define BASE_PRIO_REX (XPARAM(lPrioBaseREX))
#define PRIO_FACTOR   (XPARAM(lPrioFactor))
#define BASE_PRIO_UKT  BASE_PRIO_IOC
#define BASE_PRIO_DEBUGTASK (SAPDB_UInt4)-1

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
/*! @name Dispatcher */
 /*@(*/
/*!
   @brief          Select the next running task in a UKT
   @param          this_ukt [in] Pointer to current UKT Control structure
   @return         none


   This function is called within the RTE code, whenever a new task selection
   is performed. It therefore scans the UKT specific queues for candidates and
   moves them into the running queue if they are allowed to run.
   It begins scanning the queue that queues request from other UKTs (XXX)
   Second scan is done on the queue of outstanding asynchronous IO (AIOC)
   Third scan is done on the queue of outstanding communication request (COM)
   Forth scan is done on the queue of internal requests to UKT (IOC)
   Then the top element of the run queue is selected.
   If run queue was empty after all these scans UKT starts sleeping on its semaphore.

 */

externC void en71Dispatcher (struct ten50_UKT_Control *this_ukt );


/*!
   @brief          Set temporary lock to prevent moving task from one UKT to another.
   @param          tcb [in] Pointer to current Task Control structure
   @param          cnt [in] temporary lock count
   @param          setLock [in] true if the lock should be set, else false
   @return         none

 */

externC void en71TempMoveLock( struct TASK_TYPE   *tcb,
                               SAPDB_UInt2        cnt,
                               SAPDB_Bool         setLock );


/*!
   @brief          Set long term lock to prevent moving task from one UKT to another.
   @param          tcb [in] Pointer to current Task Control structure
   @param          lockType [in] type of lock (e.g. client is not able to handle task moves)
   @param          setLock [in] true if the lock should be set, else false
   @return         none

 */

externC void en71LongTermMoveLock( struct TASK_TYPE   *tcb,
                                   SAPDB_UInt2        lockType,
                                   SAPDB_Bool         setLock );

/*@)*/
/*! @name Queuing */
 /*@(*/
/*!
   @brief          Enqueue element in IOC queue
   @param          this_ukt [in] Pointer to current UKT Control structure
   @param          tid [in] Pointer to current Task Control structure
   @param          quu [in] Pointer to request element to be enqueued
   @return         none

 */

externC void en71_ioc ( ten50_UKT_Control    * this_ukt,
                        struct TASK_TYPE     * tid ,
                        struct DOUBLY_LINKED * quu );
 
/*!
   @brief          Enqueue element in IOC queue
   @param          this_ukt [in] Pointer to current UKT Control structure
   @param          quu [in] Pointer to request element to be enqueued
   @return         none

 */

externC void en71_iocNoTask ( ten50_UKT_Control    * this_ukt,
                              struct DOUBLY_LINKED * quu );

 /*!
    @brief          Enqueue element in IOC queue and wakeup UKT
    @param          this_ukt [in] Pointer to current UKT Control structure
    @param          tid [in] Pointer to current Task Control structure
    @param          quu [in] Pointer to request element to be enqueued
    @return         none
 
  */
 
 externC void en71_iocEnquAndWake ( ten50_UKT_Control    * this_ukt,
                                    struct TASK_TYPE     * tid ,
                                    struct DOUBLY_LINKED * quu );

/*!
   @brief          Enqueue task in RUN queue
   @param          tcb [in]       - Pointer to receiving Task Control Block
   @param          fromtcb [in]   - Pointer to current Task Control Block
   @param          this_ukt [in]  - Pointer to current UKT Control structure
   @param          quu [in]       - Pointer to request element to be enqueued
   @param          BasePrio [in]  - Set priority like BASE_PRIO_RAV or 
                                    BASE_PRIO_REX.
   @return         none
 */

externC void e71_EnquRun ( struct ten50_UKT_Control * ukt,
                           struct TASK_TYPE         * tcb,
                           struct TASK_TYPE         * fromtcb,
                           struct DOUBLY_LINKED     * quu,
                           SAPDB_UInt4                basePrio );


/*!
   @brief          Enqueue yielding task in RUN queue
   @param          tcb [in]       - Pointer to current Task Control Block
   @param          this_ukt [in]  - Pointer to current UKT Control structure
   @param          quu [in]       - Pointer to request element to be enqueued
   @param          BasePrio [in]  - Set priority like BASE_PRIO_RAV or 
                                    BASE_PRIO_REX.
   @return         none
 */
externC void e71_YieldTaskRunEnqu ( struct ten50_UKT_Control * ukt,
                                    struct TASK_TYPE         * tcb,
                                    struct DOUBLY_LINKED     * quu,
                                    SAPDB_UInt4                basePrio );

/*!
   @brief          Enqueue function request from UKT to UKT
   @param          SourceUkt [in] Pointer to source User Kernel Thread
   @param          TargetTcb [in] Pointer to target Task Control Block
   @param          FromTcb   [in] Pointer to source Task Control Block
   @param          Func [in] Function to execute in target UKT
   @param          FuncParam [in] Function func parameter
   @return         none


   Enqueue a function call to be executed on other UKT.
   Function is called with two argument
     void Func(void *FuncParam, struct TASK_TYPE *FromTask)

 */

externC void en71EnqueueUkt2UktAndWake ( struct ten50_UKT_Control * SourceUkt ,
                                         struct TASK_TYPE     * TargetTcb ,
                                         struct TASK_TYPE     * FromTcb ,
                                         PFNREMOTEFUNC          Func ,
                                         void                 * FuncParam );

/*!
   @brief          Alter UKTs task counter values
   @param          tid [in] Pointer to current Task Control structure
   @param          alterType [in] Use one of the following defines:
                  ADD_ALL_COUNTER 
                  SUB_ALL_COUNTER
                  ADD_ACTIVE_COUNTER
                  SUB_ACTIVE_COUNTER
                  ADD_TOTAL_COUNTER 
                  SUB_TOTAL_COUNTER
   @return         none

 */

externC void  en71AlterTaskCnt( struct TASK_TYPE   *tcb,
                                SAPDB_UInt2        alterType );


/*@)*/


/*!
   @brief          Check load balancing
   @param          none
   @return         next check wait time

 */
externC time_t  en71LoadBalancing();


#endif  /* GEN71_H */
