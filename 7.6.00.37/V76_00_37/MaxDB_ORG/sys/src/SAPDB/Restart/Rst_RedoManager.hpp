/*!
    @defgroup       Restart Restart (Redo)
*/
/*!
    @ingroup        Restart
    @file           Rst_RedoManager.hpp
    @author         UweH
    @brief          parallel undo/redo of transactions in order to restart the database


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



 */
#ifndef RST_REDO_MANAGER_H
#define RST_REDO_MANAGER_H

#include "ggg00.h"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "Restart/Rst_Types.hpp"
#include "Restart/Rst_LogEntryIterator.hpp"
#include "Restart/Rst_IRedoTrafficControl.hpp"

class Rst_RedoReadTask;
class Rst_RedoTrafficControl;
class RTETask_ITask;

/*!
    @class Rst_RedoManager
    @brief parallel undo/redo of transactions in order to restart the database
 */
class Rst_RedoManager
{
public:
    /// default constructor
    Rst_RedoManager ()
    : m_redoIsAborted(false),
      m_abortReasonFromRestoreComponent(e_ok),
      m_transread    (0),
      m_transredone  (0),
      m_pRedoReader  (0),
      m_pRedoReadIterator(0),
      m_pRedoTrafficControl(0)
    {}
	/// result for RedoLog()
    enum RedoLogResult
    {
        RedoLogOk,
        RedoLogAllocateFailed,
        RedoLogExecuteTaskError,
        RedoLogCancelled,
        RedoLogSavepointNotStarted,
        RedoLogRedoError
    };
	/// parallel undo/redo of transactions
    RedoLogResult RedoLog (      tsp00_TaskId          taskid,
                                 tgg00_TransContext   &oldcontext,
                                 Log_RawDeviceOffset   lastRedoOffset,
                           const Kernel_Date           untilDate,     // PTS 1111525 mb 2001-01-15
                           const Kernel_Time           untilTime,     // PTS 1111525 mb 2001-01-15
                           const Log_IOSequenceNo      untilIOSeq,    // PTS 1121539 mb 2003-06-02
                                 Rst_RedoKind          redokind,
                                 SAPDBErr_MessageList &errlist);
    /// returns true if redo is aborted
    bool IsAborted() const
    {
        return m_redoIsAborted;
    }
	/// Sets the abort flag and an corresponding error-code for documentation
    void AbortByRestoreComponent (tsp00_TaskId     taskid,
                                  tgg00_BasisError abortReason)
    {
        m_abortReasonFromRestoreComponent = abortReason;
        Abort(taskid);
    }
	/// returns some information about the progress of the redo
    void GetProgressInfo( SAPDB_UInt &transread,
                          SAPDB_UInt &transredone );
	/// this contains the last entry read during redo
    void GetLastEntryRead(Log_EntryInfo &entryInfo);
    /// This is called by the locklist at the end of each transaction
    void TryToResumeRedoReader(tsp00_TaskId taskid);
    /// This is used by HotStandby::Synchronize
    void SetLastRedoReadOffset( Log_RawDeviceOffset lastRedoOffset, tsp00_TaskId taskId );
	/// This is used by HotStandby::Synchronize
    Log_RawDeviceOffset GetLastRedoReadOffset();
	/// This is used by HotStandby::TakeOver
    void SuspendRedoReader(tsp00_TaskId          taskid);
	/// This is used by HotStandby::TakeOver - finish the redo up to a given date/time
    void ResumeRedoReader(const Kernel_Date         untilDate,
                          const Kernel_Time         untilTime);
    /// this is called by the redo reader server task, returns false on any error
    bool ReadLogAndCreateRedoFiles(tsp00_TaskId          taskid,
                                   tgg00_TransContext   &oldcontext,
                                   SAPDBErr_MessageList &errlist);
    /// get the reference to the redo traffic control
    Rst_IRedoTrafficControl* GetRedoTrafficControl()
    {
        return reinterpret_cast<Rst_IRedoTrafficControl*>(m_pRedoTrafficControl);
    }

	/// Aborts the RedoManager and all started tasks
    void Abort (tsp00_TaskId taskid);
private:

    /// Result from StopTasks()
    enum StopTasksResult
    {
        StopTaskOk,
        StopTaskReceiveError,
        StopTaskRedoErrorOccured,
        StopTaskRedoTasksWereAborted
    };
    /// receives all started redo read and execute server tasks.
    StopTasksResult StopTasks (tsp00_TaskId          taskid,
                               tgg00_TransContext   &oldcontext,
                               SAPDB_Int            &RunningTasks,
                               SAPDBErr_MessageList &errlist);
    /*!
        @brief This routine checks if something has to be cleared
               on the log and then starts the clearing.
        @param taskid               taskid for writing on the log-volume
        @param iter                 The iterator, which was used for reading
        @param lastPageCleared      true if the last read page had been cleared also
    */
    void CheckForClearLog(        tsp00_TaskId          taskid,
                            const Rst_LogEntryIterator &iter,
                                  bool                 &lastPageCleared);
private:

    /// flag which denotes a abort of the redo (exported by reference and set externally)
    volatile bool m_redoIsAborted; 
    /// changed when the redoTask is aborted
    volatile tgg00_BasisError   m_abortReasonFromRestoreComponent;
    /// number of transactions read
    SAPDB_UInt    m_transread;     
    /// number of transactions redone
    SAPDB_UInt    m_transredone;
    /// this is set to the last read position
    Log_EntryInfo m_LastEntryRead;
    /// This pointer is set if redo is running.
    Rst_RedoReadTask *m_pRedoReader;
    /// This pointer is set if redo is running.
    Rst_LogEntryIterator *m_pRedoReadIterator;
    /// This pointer is set if redo is running.
    Rst_RedoTrafficControl *m_pRedoTrafficControl;
    /// when reaching this m_untilDate,m_untilTime, the redoreader will stop. This member is used by 
    /// reference by Log_VolumeIterator::operator++() and may be changed by ResumeRedoReader()
    Kernel_Date         m_untilDate;
    /// when reaching this m_untilDate,m_untilTime, the redoreader will stop. This member is used by 
    /// reference by Log_VolumeIterator::operator++() and may be changed by ResumeRedoReader()
    Kernel_Time         m_untilTime;
    /// when reaching this m_untilIOSequence the redoreader will stop. This member is used by 
    /// reference by Log_VolumeIterator::operator++() 
    Log_IOSequenceNo    m_untilIOSequence;
};
#endif  /* RST_REDO_MANAGER_H */
