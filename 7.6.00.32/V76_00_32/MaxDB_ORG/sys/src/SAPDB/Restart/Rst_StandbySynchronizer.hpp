/*!
  @file           Rst_StandbySynchronizer.hpp
  @author         MartinB
  @ingroup        Restart
  @brief          synchronizes the RedoReader with the standby-administration commands

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



#ifndef RST_STANDBYSYNCHRONIZER_HPP
#define RST_STANDBYSYNCHRONIZER_HPP


#include "gsp00.h"
#include "ggg00.h"
#include "heo56.h"
#include "Logging/Log_Types.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"

/// The RedoReader is suspended with this reason if it has to wait for a synchronize from the
/// standby-master
#define RST_WAIT_SYNC_MASTER        249

/// The administration task is suspended with this reason, when it is waiting for
/// the completion of SuspendRedoReader() 
#define RST_WAIT_SUSPEND_REDOREADER 251

/// The administration task is suspended with this reason, when it is waiting for
/// the completion of SuspendRedoReader() 
#define RST_WAIT_FINISH_INIT 252

/// This is the invalid value of the Rst_SynchronizeValue
#define RST_INVALID_SYNC_OFFSET ((SAPDB_UInt4)SAPDB_MAX_UINT4)

/*!
  @class          Rst_SynchronizeOffset
  @brief          contains all neccessary offset methods but for a volatile instance
                  (i.e. no const-methods as in SABDB_Invalidatable)
 */
class Rst_SynchronizeOffset
{
public:
    /// constructs invalid instance
    Rst_SynchronizeOffset() : m_Value(RST_INVALID_SYNC_OFFSET) {} ; 
    /// constructs instance with given value
    Rst_SynchronizeOffset(SAPDB_UInt4 initValue) : m_Value(initValue) {} ; 
    /// returns the internal value
    SAPDB_UInt4 operator()() {  return m_Value;  };
    /// return true, if the value is valid
    SAPDB_Bool IsValid() { return m_Value != RST_INVALID_SYNC_OFFSET; };
    /// assignment of an RawDeviceOffset
    SAPDB_UInt4 operator = (const Log_RawDeviceOffset newValue) 
        { return m_Value = newValue.RawValue(); }
    /// check for equality with an RawDeviceValue
    SAPDB_Bool operator == (const Log_RawDeviceOffset operand) 
        { return operand.RawValue() == m_Value; }

private:

    volatile SAPDB_UInt4   m_Value;
};


/*!
  @class          Rst_StandbySynchronizer
  @brief          synchronizes the RedoReader with the standby-administration commands
 */

class Rst_StandbySynchronizer
{
public:

    /// identifier of RedoReader-task
    typedef tsp00_TaskId        TaskID;

    /// ctor
    Rst_StandbySynchronizer ( const Log_RawDeviceOffset      lastValidOffset )
        :
         m_ReaderTaskID (cgg_nil_pid),
         m_AdminTaskID (cgg_nil_pid),
         m_SynchronizeTaskId(cgg_nil_pid),
         m_Suspended (false),
         m_Spinlock  ((SAPDB_UTF8*)"Rst_StandbySynchronizer::m_Spinlock"),
         m_lastValidOffset(lastValidOffset.RawValue()),
         m_suspendSynchronizerUntilInitalizeComplete(true),
         m_forceSuspend(false)
    {
    }

    /// The calling task is suspended. The Log-Writer may still be writing on
    /// the page which contains the SplitMirrorSavepoint. For this case the restart must be
    /// suspended until the page had been completely written.
    /// \param ownTaskId id of the task which will be suspended until the first synchronize
    /// \param syncTaskId id of the task which continues the initialization by resuming the
    ///                   current task. This task has to be suspended until the initialization
    ///                   has finished and has to be resumed then.
    void InitialSuspend(const TaskID                  ownTaskId,
                              TaskID                & syncTaskId )
    {
        SAPDBTRACE_METHOD_DEBUG ("Rst_StandbySynchronizer::InitialSuspend", LogTrans_Trace, 5);

        /// suspend/resume for synchronize with hotstandby-master is active
        {   // begin synchronized section
            RTESync_LockedScope Lock(m_Spinlock);

            m_ReaderTaskID = ownTaskId;
            m_Suspended = true;
        }   // end synchronized section
        vsuspend(m_ReaderTaskID, RST_WAIT_SYNC_MASTER);
        syncTaskId = m_SynchronizeTaskId;
    }


    /// Suspends writer-task if necessary. This has to be called after processing
    /// the whole page
    /// \param taskId The taskid, which will be suspended
    /// \param currOffset The offset of the just processed page
    /// \param pageDate The date, when this page had been written
    /// \param pageTime The time, when this page had been written
    /// \sa CheckSuspendForStandby
    void CheckSuspendAfterReadLogPage (const TaskID              taskId,
                                       const Log_RawDeviceOffset currOffset,
                                       const Kernel_Date         pageDate,
                                       const Kernel_Time         pageTime)
    {
        SAPDBTRACE_METHOD_DEBUG ("Rst_StandbySynchronizer::CheckSuspendAfterReadLogPage", LogTrans_Trace, 5);
        if (m_lastValidOffset.IsValid())
        {
            bool doSuspend = false;
            bool doResumeAdmin = false;
            Kernel_Date HSS_untilDate;
            Kernel_Time HSS_untilTime;

            Kernel_IAdminHotStandby::Instance().GetDelayTimeStamp(HSS_untilDate, HSS_untilTime);
            
            {   // begin synchronized section
                RTESync_LockedScope Lock(m_Spinlock);
                
                if (( GetLastReadableOffset() == currOffset  )
                      ||
                    ( m_forceSuspend )
                      ||
                    ( ! Kernel_DateTime::OlderThan(pageDate,pageTime, HSS_untilDate, HSS_untilTime) ))
                {
                    m_ReaderTaskID = taskId;
                    m_Suspended = true;
                    doSuspend = true;
                    if (m_forceSuspend)
                    {
                        doResumeAdmin = true;
                    }
                }
            }   // end synchronized section
            if (doSuspend)
            {
                if (doResumeAdmin)
                {
                    vresume(m_AdminTaskID);
                    m_AdminTaskID = cgg_nil_pid;
                    m_forceSuspend = false;
                }
                vsuspend(m_ReaderTaskID, RST_WAIT_SYNC_MASTER);
            }
        }
    }

    
    /// Suspends writer-task if necessary. This has to be called after processing
    /// the whole page
    /// \param taskId The taskid, which will be suspended
    /// \param currReadPosition The poistion of the last read page
    /// \param numPagesToRead the number of pages which has to be readable starting at currOffset
    /// \sa CheckSuspendAfterReadLogPage
    void CheckSuspendForStandby (const TaskID                 taskId,
                                 const Log_RawDeviceIterator  currReadPosition,
                                 const Log_DeviceBlockCount   numPagesToRead)
    {
        SAPDBTRACE_METHOD_DEBUG ("Rst_StandbySynchronizer::CheckSuspendForStandby", LogTrans_Trace, 5);

        bool doSuspend = false;
        bool doResumeAdmin = false;

        {   // begin synchronized section
            RTESync_LockedScope Lock(m_Spinlock);
            
            if (( currReadPosition.GetPosition().IsValid() &&
                  (currReadPosition.GetDistanceTo(GetLastReadableOffset()) <= numPagesToRead) )
                  ||
                ( m_forceSuspend ))
            {
                m_ReaderTaskID = taskId;
                m_Suspended = true;
                doSuspend = true;
                if (m_forceSuspend)
                {
                    doResumeAdmin = true;
                }
            }
        }   // end synchronized section
        if (doSuspend)
        {
            if (doResumeAdmin)
            {
                vresume(m_AdminTaskID);
                m_AdminTaskID = cgg_nil_pid;
                m_forceSuspend = false;
            }
            vsuspend(m_ReaderTaskID, RST_WAIT_SYNC_MASTER);
        }
    }
        
    /// At the next call to CheckSuspendForStandby the task will be suspended
    void SuspendRedoReader(TaskID  adminTaskID)
    {
        SAPDBTRACE_METHOD_DEBUG ("Rst_StandbySynchronizer::SuspendRedoReader", LogTrans_Trace, 5);
        
        bool doSuspend = false;
        
        {   // begin synchronized section
            RTESync_LockedScope Lock(m_Spinlock);
            
            if (!m_Suspended)
            {
                m_forceSuspend = true;
                m_AdminTaskID = adminTaskID;
                doSuspend = true;
            }
        }   // end synchronized section

        if (doSuspend)
        {        
            vsuspend(m_AdminTaskID,RST_WAIT_SUSPEND_REDOREADER); 
        }
    }
    
    /// resumes the reader for reading everything left on the LogVolume
    void ResumeReaderForRestart()
    {
        SAPDBTRACE_METHOD_DEBUG ("Rst_StandbySynchronizer::ResumeReaderForRestart", LogTrans_Trace, 5);
        SetLastValidOffsetAndResume(Log_RawDeviceOffset(), cgg_nil_pid);
    }

    /// returns the last readable offset (PTS 1126157)
    Log_RawDeviceOffset GetLastReadableOffset()
    {
        if (m_lastValidOffset.IsValid())
        {
            return Log_RawDeviceOffset(m_lastValidOffset());
        }
        else
        {
            return Log_RawDeviceOffset();
        }
    }

    /// Sets a new value of m_lastValidOffset and resumes the RedoReader if necessary
    void SetLastValidOffsetAndResume(const Log_RawDeviceOffset newValidOffset,
                                     const TaskID              synchronizerTaskId )
    {
        SAPDB_Bool do_resume = false;
        TaskID     taskToBeResumed = cgg_nil_pid;
        

        SAPDBTRACE_METHOD_DEBUG ("Rst_StandbySynchronizer::SetLastValidOffsetAndResume", LogTrans_Trace, 5);
        
        {   // begin synchronized section
            RTESync_LockedScope Lock(m_Spinlock);

            m_lastValidOffset = newValidOffset;
            if (m_Suspended)
            {
                taskToBeResumed = m_ReaderTaskID;
                m_ReaderTaskID = cgg_nil_pid;
                do_resume   = true;
                m_Suspended = false;
                m_forceSuspend = false;
                if (m_suspendSynchronizerUntilInitalizeComplete && synchronizerTaskId != cgg_nil_pid)
                {
                    m_SynchronizeTaskId = synchronizerTaskId;
                }
            }
        }   // end synchronized section
        
        if (do_resume)
        {
            vresume(taskToBeResumed);
            if (m_suspendSynchronizerUntilInitalizeComplete && synchronizerTaskId != cgg_nil_pid)
            {
                m_suspendSynchronizerUntilInitalizeComplete = false;
                vsuspend(synchronizerTaskId, RST_WAIT_FINISH_INIT);
            }
        }
    }

private:

    /// task id of the task which will be suspended/resumed (the RedoReader)
    volatile TaskID                          m_ReaderTaskID;

    /// task id of the adminstrator-task, which will be suspended/resumed
    volatile TaskID                          m_AdminTaskID;

    /// task id of the synchronizer-task, which has to be suspended if the initialization
    /// has not been finished completely
    
    volatile TaskID                          m_SynchronizeTaskId;
    /// true, if the RedoReader is suspended
    volatile SAPDB_Bool                      m_Suspended;
    
    /// offset of the last page, which may be processed before suspending.
    /// If is Invalid, then everything up to the untilTimestamp is read
             Rst_SynchronizeOffset           m_lastValidOffset;

    /// When true, then the next call to CheckSuspendForStandby will suspend the reader
    volatile bool                            m_forceSuspend;
    
    /// When true, then the SynchronizerTask had been suspended, because the Reader has yet
    /// not been completely initialized due to CheckForInitialSuspend()
    volatile bool                            m_suspendSynchronizerUntilInitalizeComplete;

    /// lock for synchronizing access to variables
    RTESync_NamedSpinlock                    m_Spinlock;
};

#endif // RST_STANDBYSYNCHRONIZER_HPP

