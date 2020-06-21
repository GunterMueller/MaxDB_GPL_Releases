/*!
    @file           Kernel_TaskWaitList.hpp
    @author         UweH
    @ingroup        KernelCommon
    @brief          This class can be used to collect tasks to suspend or resume them

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
#ifndef KERNEL_TASKWAITLIST_H
#define KERNEL_TASKWAITLIST_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "heo51.h"   // vgetpid
#include "heo56.h"   // vresume
/*!
    @class Kernel_TaskWaitList
    @brief Defines a list of waiting tasks.
 */
class Kernel_TaskWaitList
{
public:
    /// only rename
    typedef tgg00_WaitContext WaitContext;
    /// An empty list is created.
    Kernel_TaskWaitList()
    :m_NumberOfWaitingTasks(0),
     m_First(0),
     m_Last(0)
	{}
    /// Returns true if the lis is empty
    bool IsEmpty() const
    {
        return 0 == m_First;
    }
    /// Returns number of waiting tasks.
    SAPDB_UInt NumberOfWaitingTasks() const
    {
        return m_NumberOfWaitingTasks;
    }
    /// Inserts a task id at the top of the list. The taskid must be set.
    void InsertAsFirst (tsp00_TaskId  taskid,
                        WaitContext  &context)
    {
        if ( PrepareInsert (taskid, context) )
        {
            context.wcNextWaiting_gg00 = 0;
            return;
        }

        context.wcNextWaiting_gg00 = m_First;
        m_First                    = &context;
    }
    /// Inserts a task id at the bottom of the list. The taskid must be set.
    void InsertAsLast (tsp00_TaskId  taskid,
                       WaitContext  &context)
    {
        if ( PrepareInsert (taskid, context) )
            return; // list was empty

        m_Last->wcNextWaiting_gg00 = &context;
        m_Last                     =  m_Last->wcNextWaiting_gg00;
    }
    /// The first task in the list is resumed and removed from the list.
    void ResumeFirst (bool removeonly = false)
    {
        WaitContext *nextWaitContext     = m_First;
        WaitContext *resumeAndRemoveWaitContext;
        
        if ( nextWaitContext != 0 )
        {
            resumeAndRemoveWaitContext = nextWaitContext;
            nextWaitContext            = nextWaitContext->wcNextWaiting_gg00;

            RemoveContextFromList (resumeAndRemoveWaitContext);

            if ( cgg_nil_pid == resumeAndRemoveWaitContext->wcWaitingTaskId_gg00 )
                RTE_Crash(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"waiting taskid not nil"));

            if ( ! removeonly )
                vresume (resumeAndRemoveWaitContext->wcWaitingTaskId_gg00);
        }
    }
    /// All tasks in the list are resumed and removed from the list. POST: IsEmpty() returns true.
    void ResumeAll (bool removeonly = false)
    {
        while ( ! IsEmpty() )
            ResumeFirst(removeonly);
    }
private:
    /// check the input and initialize the waiting context. Returns true if first insert.
    bool PrepareInsert (tsp00_TaskId  taskid,
                        WaitContext  &context)
    {
        #ifdef SAPDB_QUICK
        vgetpid(context.wcWaitingTaskId_gg00);
        if ( context.wcWaitingTaskId_gg00 != taskid )
            RTE_Crash(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"wrong taskid"));
        if ( cgg_nil_pid == taskid )
            RTE_Crash(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"waiting taskid not nil"));
        #endif
        context.wcWaitingTaskId_gg00 = taskid;
        context.wcNextWaiting_gg00   = 0;

        ++m_NumberOfWaitingTasks;
        if ( IsEmpty() )
        {
            m_First = &context;
            m_Last  =  m_First;
            return true;
        }
        return false;
    }
    /// de-chain given context from waiting list
    void RemoveContextFromList (WaitContext *context)
    {
        if ( context == m_First )
            m_First = context->wcNextWaiting_gg00;
        if ( 0 == m_First )
            m_Last = 0;
        context->wcNextWaiting_gg00   = 0;
        if ( m_NumberOfWaitingTasks > 0 )
            --m_NumberOfWaitingTasks; // PTS 1130813 UH 2007-07-28 added decrement
    }
private:
    /// waiting task counter
    SAPDB_UInt   m_NumberOfWaitingTasks;
    /// first task waiting
    WaitContext *m_First;
    /// last task waiting
    WaitContext *m_Last;
};
#endif  /* KERNEL_TASKWAITLIST_H */
