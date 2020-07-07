/*!
  @file           Log_WriterTaskSynchronizer.hpp
  @author         UweH
  @author         TillL
  @ingroup        Logging
  @brief          
  @see            

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



#ifndef LOG_WRITERTASKSYNCHRONIZER_HPP
#define LOG_WRITERTASKSYNCHRONIZER_HPP


#include "gsp00.h"
#include "ggg00.h"
#include "heo56.h"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_SuspendReason.h"


/// The Log_Writer is suspended with this reason because of an empty queue
#define LOG_QUEUE_EMPTY             TSK_VSUSPEND_REASON_NO_WORK

/// The Log_Writer is initially suspended with this suspend reason
#define LOG_WRITER_NOT_INITIALIZED  TSK_VSUSPEND_REASON_NO_WORK


/*!
  @class          Log_WriterTaskSynchronizer
  @brief          Corodinates the suspend/resume of the Log_Writer
 */

class Log_WriterTaskSynchronizer
{
public:

	/// identifier of kernel task
    typedef tsp00_TaskId                                        TaskID;

    /// suspend reason id
    typedef SAPDB_Int2                                          SuspendReason;

    /// constructor
    Log_WriterTaskSynchronizer
        (TaskID                         taskid)
        :
         m_taskID    (taskid),
         m_suspended (false),
         m_resumeCalled (false),
         m_spinlock  ((SAPDB_UTF8*)"Log_WriterTaskSynchronizer::m_spinlock")
    {}

    /// Suspends writer-task if necessary
    void Suspend (SuspendReason reason)
    {
        m_spinlock.Lock();
        m_suspended = true;
        m_spinlock.Unlock();
        vsuspend(m_taskID, reason);
    }
    
    /// Initializes member variable to detect, if a complete cycle has been done
    /// without a call to Resume()
    void InitCycle()
    {
        RTESync_LockedScope Lock(m_spinlock);
        m_resumeCalled = false;
    }
    
    /// Suspends writer only if NO resume-call has been issued after the last call to
    /// InitCycle()
    void SuspendIfNoResumeCalled ()
    {
        m_spinlock.Lock();
        bool doSuspend = false;
        if ( ! m_resumeCalled )
        {
            m_suspended = true;
            doSuspend = true;
        }
        m_spinlock.Unlock();
        if (doSuspend)
        {
            vsuspend(m_taskID, LOG_QUEUE_EMPTY);
        }
    }
    


    /// Returns the task id of the writer task
    TaskID GetTaskID() const
    {
        return m_taskID;
    }

    /// Resumes task if necessary (if still running).
    void Resume()
    {
        SAPDB_Bool do_resume = false;
        {
            RTESync_LockedScope Lock(m_spinlock);

            if (m_suspended)
            {
                do_resume   = true;
                m_suspended = false;
            }
            m_resumeCalled = true;
        }
        if (do_resume)
        {
            // Kernel_VTrace() << "LWSync Resume" << NewLine;
            vresume(m_taskID);
        }
    }
               
private:

    /// task id of the Log_Writer
    TaskID                          m_taskID;

    /// task suspend state
    SAPDB_Bool                      m_suspended;
    
    /// this state is neccessary to detect, whether a Log_Queue has a outstanding IO,
    /// but this Log_Queue has already tested in the cycle of GetOutput. 
    SAPDB_Bool                      m_resumeCalled;
    
    /// lock for synchronized code parts
    mutable RTESync_NamedSpinlock   m_spinlock;

};




#endif // LOG_WRITERTASKSYNCHRONIZER_HPP

