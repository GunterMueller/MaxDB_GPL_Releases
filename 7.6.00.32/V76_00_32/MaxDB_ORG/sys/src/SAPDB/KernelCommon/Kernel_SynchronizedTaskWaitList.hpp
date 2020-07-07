/*!
  @file     Kernel_SynchronizedTaskWaitList.hpp
  @author   TorstenS
  @ingroup  KernelCommon
  @brief    This class can be used to collect tasks to suspend or resume them
*/

/*
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

*/


#ifndef KERNEL_SYNCHRONIZEDTASKWAITLIST_HPP
#define KERNEL_SYNCHRONIZEDTASKWAITLIST_HPP


#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"


/*!
   @class   Kernel_SynchronizedTaskWaitList
   @brief   wait queue class used to suspend and resume tasks.
            The g08KnlWaitQueue latch is used to synchronize concurrent
            accesses.
 */

class Kernel_SynchronizedTaskWaitList
{
public:

    /*!
        @class   Kernel_IWaitListAction
        @brief   this class offers the possibility to implement a
                 callback mechanism, which is called before the proper
                 Kernel_SynchronizedTaskWaitList methods.
    */

    class Kernel_IWaitListAction
    {
    public:

        /*!
            @brief   executed before a task is suspended
            @param   bIsEmpty [in] true if no task is suspended yet
            @return  none
        */

        virtual void ExecuteBeforeSuspend( const bool bIsEmpty  ) const = 0;

        /*!
            @brief   executed before a task is resumed
            @return  none
        */

        virtual void ExecuteBeforeResume() const = 0;

        /// destructor
        virtual ~Kernel_IWaitListAction(){};
    };

    /// only for convenience
    typedef Kernel_TaskWaitList::WaitContext WaitContext;

    /*!
       @brief   constructor for a synchronized wait queue
       @param   suspendReason  [in] reason used if a task should be suspended
       @return  none
     */

    Kernel_SynchronizedTaskWaitList(
        const SAPDB_Int2                suspendReason,
        const Kernel_IWaitListAction*   pAction = NULL);

    /*!
       @brief   This method is used to resume all task stored within the
                wait queue. Note that a region is used to protect the
                internal structure of the wait queue.
       @return  none
     */

    void ResumeAllWaitingTasks( const RTE_TaskId taskId );

    /*!
       @brief   This method is used to insert a task into the wait queue and
                to suspend the task. The task will leave the method after it
                is resume by another task. Note that a region is used to
                protect the internal structure of the wait queue.
       @param   taskId [in] calling task identification
       @return  none
     */

    void InsertTaskAndWait( const RTE_TaskId taskId )
    {
        WaitContext waitContext;

        InsertTask( taskId, waitContext );

        Wait( taskId );
    }

    /*!
       @brief   This method is used to insert a task into the wait queue.
                Note that the task is not suspended by this method!
                It will return immediately. Note that a region is used to
                protect the internal structure of the wait queue.
       @param   taskId      [in] calling task identification
       @param   waitContext [in] task specific wait context used to manage
                                 the wait situation
       @return  none
     */

    void InsertTask( const RTE_TaskId   taskId,
                     WaitContext        &waitContext );

    /*!
       @brief   This method is used to suspend the calling task. Note
                that the task won't be inserted into a wait queue. This
                has to be done before with InsertTask.
       @param   taskId [in] calling task identification
       @return  none
     */

    void Wait( const RTE_TaskId taskId ) const;

    /*!
       @brief   This method is used to provide the filling state of the
                wait queue. Note that NO region is used to protect the
                internal structure of the wait queue.
       @return  (SAPDB_Bool) true if the wait queue is empty else false.
     */

    bool IsEmpty() const
    {
        return( m_WaitList.IsEmpty() );
    }

private:

    /// suspend reason
    const SAPDB_Int2 m_SuspendReason;

    /// identifier for the latch
    const  RTE_RegionId  m_RegionId;

    /// possible callback
    const Kernel_IWaitListAction*   m_pAction;

    /// the list of waiting tasks
    Kernel_TaskWaitList m_WaitList;
};

#endif  /* KERNEL_SYNCHRONIZEDTASKWAITLIST_HPP */
