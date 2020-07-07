/*!========================================================================

  @file         RTETask_LegacyTaskCtrl.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access old task control

                The following class provides methodes to access 
                old task control.

  @since        2005-11-15  16:16
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


#ifndef RTETASK_LEGACYTASKCTRL_HPP
#define RTETASK_LEGACYTASKCTRL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (KERNEL_LZU)
# error "Expecting KERNEL_LZU define"
#endif
#include    "RunTime/RTE_Types.h"
#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include    "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#include    "geo50_0.h"       // old task control

#if defined (_WIN32)
# include   "gos74.h"    /* nocheck */
# include   "gos003.h"   /* nocheck */
#else
# include   "gen71.h"    /* nocheck */
# include   "gen003.h"   /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;
struct RTETask_TaskScheduler;

/*!---------------------------------------------------------------------------
  @class   RTETask_LegacyTaskCtrl
  @brief   Access old task control.

           The following class provides methodes to access the old task
           control. The most variables of the old task control 
           structures have different writings on various
           platforms like UNIX and Windows.
           To hide this differences and to provide access to these
           legacy values in new classes the following class should be used.
 ----------------------------------------------------------------------------*/

struct RTETask_LegacyTaskCtrl
{
public:
    /*!  
    @brief          Get the current legacy task control object

                    USE ONLY IF NO ANCHOR IS AVAILABLE! 

                    If a pointer to the current task scheduler object or
                    to the UKT control is known use tis way to get the current 
                    task!

    @return         Pointer to the current legacy task object or 
                    null if we are not running in UKT context.
     */
    static RTETask_LegacyTaskCtrl* Self();


    /*!  
    @brief          Get the legacy task control object of a given taskid

    @return         Pointer to the legacy task object or 
                    null if unknown.
     */
    static RTETask_LegacyTaskCtrl* ByTaskID(  RTE_TaskId taskID );

    /*!
    @brief         Get the region ID the task is curently waiting.
    */
#   if defined (_WIN32)
     RTE_RegionId &InRegionID()
     {
         return m_pTaskCtrl->Sid;
     }
#   else
     SAPDB_UInt4 &InRegionID()
     {
         return m_pTaskCtrl->sid;
     }
#   endif

     /*!
     @brief check if cancel byte is set
      */
     SAPDB_Bool CancelPending() const
     {
#   if defined(_WIN32)
        return m_pTaskCtrl->KernelCtrlRecord.to_cancel != 0;
#   else
        return m_pTaskCtrl->rte_comm.to_cancel != 0;
#   endif
     }

    /*! 
    @brief  Set cancel request to true
    */
    virtual void Cancel()
    {
#   if defined(_WIN32)
        m_pTaskCtrl->KernelCtrlRecord.to_cancel = 1;
#   else
        m_pTaskCtrl->rte_comm.to_cancel = 1;
#   endif
    }

    /*! 
    @brief  Set cancel request to false
    */
    virtual void ResetPendingCancel()
    {
#   if defined(_WIN32)
        m_pTaskCtrl->KernelCtrlRecord.to_cancel = 0;
#   else
        m_pTaskCtrl->rte_comm.to_cancel = 0;
#   endif
    }


    /*!
    @brief         Get the state of task e.g. TSK_VRELEASE, TSK_INACTIVE ... 
    */
    volatile SAPDB_Int4 &TaskState()
    {
#       if defined (_WIN32)
         return m_pTaskCtrl->TaskState;
#       else
         return m_pTaskCtrl->state;
#       endif
    }

    /*!
    @brief         Get the current spinlock task is yielding for
    */
    SAPDB_Char * NameOfSpinlockYieldingFor()
    {
         return &m_pTaskCtrl->NameOfSpinlockYieldingFor[0];
    }

    /*!
    @brief         Get the state reason given by the kernel code
    */
    SAPDB_Int4 &TaskStateReason()
    {
#       if defined (_WIN32)
         return m_pTaskCtrl->ulStateReason;
#       else
         return m_pTaskCtrl->state_reason;
#       endif
    }

    /*!
    @brief         Next task in list of all kernel tasks
    */
    TASK_CTRL &NextTask()           
    {
#       if defined (_WIN32)
         return     m_pTaskCtrl->pNextInTaskList;
#       else
         return     m_pTaskCtrl->next_task;
#       endif
    }

    /*!
    @brief         Next task in wait queue
    */
    TASK_CTRL &NextTaskInWaitQueue()           
    {
         return     m_pTaskCtrl->nextTaskInWaitQueue;
    }
 
    /*!
    @brief          Get the number of region calls since last 
                    dispatcher/scheduler call
    */
    SAPDB_UInt8 &BeginExclusiveCount()         
    {
#       if defined (_WIN32)
         return m_pTaskCtrl->ulBegExclCnt;
#       else
         return m_pTaskCtrl->begexcl_count;
#       endif
    }

    /*!
    @brief          Nested exclusive count
    */
    SAPDB_UInt4 &ExclusiveNestingCount()    
    {
#       if defined (_WIN32)
         return (SAPDB_UInt4&)m_pTaskCtrl->lNestingExcl;
#       else
         return m_pTaskCtrl->excl_schachtel;
#       endif
    }

    /*!
    @brief          Get the task scheduling priority flag
    */
#   if defined (_WIN32)
     SAPDB_UInt1 &PrioFlag()
     {
         return m_pTaskCtrl->PrioFlag;
     }
#   else
     SAPDB_UInt4 &PrioFlag()
     {
         return m_pTaskCtrl->prio_flag;
     }
#   endif


    /*!
    @brief          Get the encryption type (e.g. SSL) of this connection
    */
    RTETask_ITaskClientConnection::EncryptionType EncryptionType() const
    {
#   ifdef _WIN32
        return (RTETask_ITaskClientConnection::EncryptionType)(m_pTaskCtrl->pUserCommCtrl->encryption);
#   else
        return (RTETask_ITaskClientConnection::EncryptionType)(m_pTaskCtrl->connection->encryption);
#   endif
    }

    /*!
    @brief          Get the connection type (e.g. SQL or liveCache)
    */
    RTETask_ITaskClientConnection::ConnectionType ConnectionType() const;

    /*!
    @brief          Get the number of times the task has done a self suspend ib venexcl
    */
    SAPDB_UInt8 &EndExclusiveSelfSuspendCount()
    {
#       if defined (_WIN32)
         return m_pTaskCtrl->ulSelfSuspCount;
#       else
         return m_pTaskCtrl->self_susp_cnt;
#       endif
    }
    
    /*!
    @brief          Get the number of region calls the task has made
    */
     SAPDB_UInt8 &BeginExclusiveCalls()
     {
#   if defined (_WIN32)
          return m_pTaskCtrl->ulBegExclCalls;
#   else
          return m_pTaskCtrl->ulBegExclCalls;
#   endif
     }

    /*!
    @brief          Region collison statistic counter  (UNIX only)
    */
    SAPDB_Int &RegionCollisionCounter(SAPDB_Int8 id)
    {
#       if defined (_WIN32)
         return m_dummy;
#       else
         return m_pTaskCtrl->reg_coll_cnt[id];
#       endif
    }


    /*!
    @brief          Task goto dispatcher call
    */
    UKT_CTRL GotoDispatcher( UKT_CTRL pUKT  )
    {
        GOTO_DISP(&pUKT);

        return pUKT;
    }


    /*!
    @brief          Get the dispatcher/scheduler queue type where the 
                    task stays
    */
#   if defined (_WIN32)
     SAPDB_UInt1 &QueueLocation()
     {
         return m_pTaskCtrl->QueLocation;
     }
#   else
     SAPDB_UInt4 &QueueLocation()
     {
         return m_pTaskCtrl->is_in_queue;
     }
#   endif

    /*!
    @brief          Get the communication queue statistic counter
    */
    SAPDB_UInt4 &CommQueueCount(SAPDB_Bool resetable=true)
    {
         if ( resetable )
         {
             return m_pTaskCtrl->QueStat.Resetable.ulCOMQueCount;
         }
         else
         {
             return m_pTaskCtrl->QueStat.Total.ulCOMQueCount;
         }
    }

    /*!
    @brief          Set a load balancing lock for task, so it's not
                    moved to another UKT
    */
    void LockLoadBalancing()
    {
#       if defined (_WIN32)
         sql74k_temp_move_lock ( m_pTaskCtrl, 1, true );
#       else
         en71TempMoveLock ( m_pTaskCtrl, 1, true );
#       endif
    }

    /*!
    @brief          Release load balancing lock
    */
    void UnlockLoadBalancing()
    {
#       if defined (_WIN32)
         sql74k_temp_move_lock ( m_pTaskCtrl, 1, false );
#       else
         en71TempMoveLock ( m_pTaskCtrl, 1, false );
#       endif
    }

    /*!
    @brief          Set a load balancing lock for task, so it's not
                    moved to another UKT
    */
    void LockLoadBalancing( SAPDB_UInt2        lockType )
    {
#       if defined (_WIN32)
          sql74k_long_term_move_lock( m_pTaskCtrl, lockType, true );
#       else
         en71LongTermMoveLock( m_pTaskCtrl, lockType, true );
#       endif
    }


    /*!
    @brief          Release load balancing lock
    */
    void UnlockLoadBalancing( SAPDB_UInt2        lockType )
    {
#       if defined (_WIN32)
          sql74k_long_term_move_lock( m_pTaskCtrl, lockType, false );
#       else
         en71LongTermMoveLock( m_pTaskCtrl, lockType, false  );
#       endif
    }


    /*!
    @brief        Is the task connectable?
    @return       Returns true if the task is connectable
    */
    SAPDB_Bool IsConnectable()
    {
#       if defined (_WIN32)
         return m_pTaskCtrl->Connectable;
#       else
         return m_pTaskCtrl->connectable;
#       endif
    }


    /*!
    @brief        Is the task connected?
    @return       Returns true if the task is connected to a client
    */
    SAPDB_Bool IsConnected()
    {
#       if defined (_WIN32)
         return  0 != m_pTaskCtrl->pUserCommCtrl->ulConnected;
#       else
         return  0 != m_pTaskCtrl->connection->ci_connect_time;
#       endif
    }

    /*!
    @brief        Is the task connected?
    @return       Returns true if the task is connected to a client
    */
    static SAPDB_Bool IsUtilityTaskConnected();

    /*!
    @brief         Set task connected state
    */
    void SetConnected()
    {
#       if defined (_WIN32)
         m_pTaskCtrl->pUserCommCtrl->ulConnected  = kgs.ulCurrTime;
#       else
         m_pTaskCtrl->connection->ci_connect_time = KGS->current_time;
#       endif
    }


    /*!
    @brief         Set task not connected state
    */
    void SetNotConnected()
    {
#       if defined (_WIN32)
         m_pTaskCtrl->pUserCommCtrl->ulConnected  = 0;
#       else
         m_pTaskCtrl->connection->ci_connect_time = 0;
#       endif
    }


    /*!
    @brief         Get pointer to a task related common 
                   queue element
    @return        Pointer to common task queue element.
    */
    DLQ_ELEMENT*           CommonQueueElement1()
    {
        return m_DLQElement[0];
    }

    /*!
    @brief         Get pointer to a task related common 
                   queue element
    @return        Pointer to common task queue element.
    */
    DLQ_ELEMENT*           CommonQueueElement2()
    {
        return m_DLQElement[1];
    }
     
    /*!
    @brief          Get direct access to task control structure
    */
    operator TASK_CTRL() const
    {
        return m_pTaskCtrl;
    }

    /*!
    @brief          The UKT the task belongs to.
    */
    UKT_CTRL UKTCtrl()
    {
#       if defined (_WIN32)
         return m_pTaskCtrl->pUKT;
#       else
         return m_pTaskCtrl->ukt;
#       endif
    }

    /*! 
    @brief          Return the Task object. 
      
                    WE SHOULD NOT STORE THE THE TASKCTRL OR TASK
                    IN MEMBER VARIABLES BECAUSE IT MIGHT CHANGE DURING
                    A SCHEDULE OPERATION (LOAD BALANCING)
     */
    RTETask_Task*   TaskObject()
    {
        return m_pTaskCtrl->pTaskObject;
    }

    /*!
    @brief          Return the TaskScheduler previously called UKT.
      
                    WE SHOULD NOT STORE THE THE UKT OR TASKSCHEDULER 
                    IN MEMBER VARIABLES BECAUSE IT MIGHT CHANGE DURING
                    A SCHEDULE OPERATION (LOAD BALANCING)
     */
    RTETask_TaskScheduler*  TaskSchedulerObject()
    {
        return UKTCtrl()->pTaskSchedulerObject;
    }


private:
    /*! @brief    The following methodes can also be used by the task class */
    friend struct RTETask_Task;

    RTETask_LegacyTaskCtrl(TASK_CTRL pTaskCtrl)
                       : m_pTaskCtrl(pTaskCtrl) 
#                      if defined (_WIN32)
                        ,m_dummy(0)
#                      endif
    {
        pTaskCtrl->pLegacyTaskCtrlObject = this;
    }


private:
    // don't allow copies
	inline   RTETask_LegacyTaskCtrl( const RTETask_LegacyTaskCtrl &c ) {;}; 

private:
    static  RTETask_ITaskClientConnection::ConnectionType              m_ConnectTypeByInstance;

  RTETask_CommonQueueElement     m_DLQElement[2];
  TASK_CTRL                      m_pTaskCtrl;              // old task control

# if defined (_WIN32)
   SAPDB_Int                     m_dummy;
# endif
};


#endif  /* RTETASK_LEGACYTASKCTRL_HPP */
