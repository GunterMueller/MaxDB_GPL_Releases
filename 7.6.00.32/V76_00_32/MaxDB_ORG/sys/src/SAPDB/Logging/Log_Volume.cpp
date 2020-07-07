/*!
  @file           Log_Volume.cpp 
  @author         UweH
  @author         MartinB
  @ingroup        LogVolume

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


#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Statistics.hpp"
#include "heo47.h"   // sql47_ptoc
#include "hkb38.h"   // k38history_lost_write
#include "hkb57_1.h" // k57restartrec
#include "hgg01_3.h" // g01restart_time
#include "SAPDBCommon/Messages/SDBMsg_Log.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "Container/Container_Vector.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_ExtendedState.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "KernelCommon/Kernel_IAdminDiagnose.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/ParameterNames/KernelParam_DataAccess.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "Logging/Log_SeqTapeReader.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "IOManager/IOMan_ILogManager.hpp"
#include "IOManager/IOMan_ILogInfo.hpp"

/// forward declaration
class Rst_StandbySynchronizer;

/// the instance-pointer of the singleton is initially initialized
Log_Volume* Log_Volume::m_Instance = 0;

// -----------------------------------------------------------------------------
void Log_Volume::CreateInstance(RTETask_ITask          &task,
                                SAPDBMem_IRawAllocator &allocator,
                                FrameAllocator         &framealloc,
                                QueueSyncMode           syncmode)
{
    SAPDBMEM_STATIC_RAW_ALLOCATE (Space, sizeof(Log_Volume));
    
    if ( task.Type() != RTETask_ITask::Logwriter )
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"must be called by writertask") );

    if ( m_Instance != 0 )
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"may be called only once") );

    m_Instance = new (Space) Log_Volume ( task.ID(),
                                          allocator,
                                          framealloc,
                                          syncmode);
     
    if ( m_Instance == 0 )
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_Instance != 0") );

    Kernel_IAdminHotStandby::Instance().DetermineHotStandbyRole();
}
// -----------------------------------------------------------------------------
void Log_Volume::Delete()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::Delete", LogVolume_Trace, 5);

    // Close all log volumes if necassary
    tsp00_TaskId taskid;
    vgetpid(taskid);
    CloseLogArea(taskid);
    
    // reset all states
    m_State = State();
    
    if (m_LogInfoPage.IsAssigned())
    {
        Kernel_Page::PageFrame frame = m_LogInfoPage.GetFrame();
        m_FrameAlloc.Free(frame);
        m_LogInfoPage.ReleaseFrame();
    }

    if (m_BufferedPage.IsAssigned())
    {
        Kernel_Page::PageFrame frame = m_BufferedPage.GetFrame();
        m_FrameAlloc.Free(frame);
        m_BufferedPage.ReleaseFrame();
    }

    if (m_LogQueues.GetSize() > 0)
    {
        for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize(); qid++)
        {
            m_LogQueues[qid]->Delete();
            destroy(m_LogQueues[qid], m_Allocator); // PTS 11135304 mb 2005-05-26
        }
    }

    m_IOPages.Delete();
    m_LogQueues.Delete();
    m_LogWriter.Delete();
}
// -----------------------------------------------------------------------------
Log_ErrorState Log_Volume::CreateForNewDatabase(TaskID taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CreateForNewDatabase", LogVolume_Trace, 5);

    // log volumes were just created and so they are open an accessible
    m_State.SetLogAreaAccessible();

    const SAPDB_UInt queuesize    = QueueSize();
    const SAPDB_UInt ioblockcount = IOMan_ILogInfo::GetInstance().GetLogIOBlockCount();
    const SAPDB_UInt queuecount   = g01usedmaxlogqueuecount();
    
    m_RawDeviceBegin   = LOG_BEGIN_OFFSET;
    m_RawDeviceEnd     = IOMan_ILogInfo::GetInstance().TotalUsableLogPages() - 1;
    m_autosaveDistance = g01autosave_distance;

    // -------------------------------------------------
    // allocate frame for info page and for buffered page:
    // -------------------------------------------------
    if ( ! AllocateAllAndReadInfoPage(taskid, ioblockcount, queuesize, queuecount) )
    {
        Delete();
        return Log_PageAllocationFailed;
    }

    if ( m_RawDeviceEnd.RawValue() < m_RawDeviceBegin.RawValue() + m_autosaveDistance) 
    {
		RTE_Message( Log_Exception(__CONTEXT__, LOG_SAVE_DISTANCE_TOO_SMALL,
                                   SAPDB_ToString(m_RawDeviceEnd),
                                   SAPDB_ToString(m_RawDeviceBegin),
                                   SAPDB_ToString(m_autosaveDistance) ) );
        Delete();
        return Log_LogSaveDistanceTooLarge;
    }

    // -------------------------------------------------
    // create info log page
    // -------------------------------------------------
    CreateInfoPage(taskid, queuecount);

    // -------------------------------------------------
    // clear entire raw log device:
    // -------------------------------------------------
    const SAPDB_Bool formatLog = SAPDB_FALSE;
    ClearLogIntern( taskid, 
                    LogicalLogBeginOffset(),
                    LogicalLogEndOffset(),
                    Log_RawDeviceOffset(),
                    Log_RawDeviceOffset(),
                    m_LogInfoPage.ReadLastKnownIOSequenceNo(),
                    Log_DeviceStateClearing,
                    Log_DeviceStateCleared,
                    formatLog); 
    
    // -------------------------------------------------
    // initialize log writer, ioseqno is invalid, write position is at first device block:
    // -------------------------------------------------
    if ( m_State.IsReadyForWriting() )
    {
        ResetLogWriter (taskid);
    }
    else
    {
        if ( ! InitLogWriter( LogicalLogBeginOffset(), 
                              m_LogInfoPage.ReadLastKnownIOSequenceNo(),
                              ioblockcount) )
        {
            Delete();
            return Log_RawAllocationFailed;
        }
    }

    // The size check must be done after logwriter is initialized
    // because only the logwriter gives the correct log size information.
    if (m_RawDeviceEnd.RawValue() < m_RawDeviceBegin.RawValue() + m_LogWriter.CalculateReservedPages(queuecount*queuesize))
    {
		RTE_Message( Log_Exception(__CONTEXT__, LOG_VOLUME_TOO_SMALL,
                                   SAPDB_ToString(m_RawDeviceEnd),
                                   SAPDB_ToString(m_RawDeviceBegin),
                                   SAPDB_ToString(m_LogWriter.CalculateReservedPages(m_LogInfoPage.ReadQueueCount()*queuesize)),
                                   SAPDB_ToString(m_LogInfoPage.ReadQueueCount()),
                                   SAPDB_ToString(queuesize) ) );
        Delete();
        return Log_LogDeviceTooSmall;
    }

    // resume writer task after that archive log has been allocated
    m_WriterTaskSync.Resume();

    if ( LogVolume_Trace.TracesLevel(6) )
        WriteDeviceParametersToTrace ();

    m_State.SetReadyForWriting();
    
    // -------------------------------------------------
    // mark dbm.knl so the recovery manager can work
    // -------------------------------------------------
    k38history_lost_write(taskid);
 
    return Log_Ok;
}
// -----------------------------------------------------------------------------
Log_ErrorState Log_Volume::Restart ( TaskID       taskid,
                                     RestartType  restartType,
                                     Msg_List    &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::Restart", LogVolume_Trace, 5);

    // Open all log volumes if necassary
    if ( ! OpenLogArea(taskid) )
        return Log_AreaOpenError;
    
          Log_ErrorState  result             = Log_Ok;
    const SAPDB_UInt      queuesize          = QueueSize();
    const SAPDB_UInt      ioblockcount       = IOMan_ILogInfo::GetInstance().GetLogIOBlockCount();
    const Log_RawDeviceOffset maxOffsetFromIoMan = IOMan_ILogInfo::GetInstance().TotalUsableLogPages() - 1;  // PTS 1124854 UH 2003-10-20

    
    m_RawDeviceBegin   = LOG_BEGIN_OFFSET;
    m_RawDeviceEnd     = maxOffsetFromIoMan;
    m_autosaveDistance = g01autosave_distance;
    
    if ( ! AllocateAllAndReadInfoPage(taskid, ioblockcount, queuesize, 0) )
    {
        Delete();
        return Log_PageAllocationFailed;
    }

    bool addLogDevSpaceIsPending = maxOffsetFromIoMan.RawValue() > m_LogInfoPage.ReadCycleRelatedEndOffset().RawValue();

    if ( addLogDevSpaceIsPending )
        m_RawDeviceEnd = m_LogInfoPage.ReadCycleRelatedEndOffset();

    if ( m_RawDeviceEnd.RawValue() < m_RawDeviceBegin.RawValue() + m_autosaveDistance) 
    {
        errlist = Log_Exception(__CONTEXT__, LOG_SAVE_DISTANCE_TOO_SMALL,
                                   SAPDB_ToString(m_RawDeviceEnd),
                                   SAPDB_ToString(m_RawDeviceBegin),
                                   SAPDB_ToString(m_autosaveDistance) );
        Delete();
        return Log_LogSaveDistanceTooLarge;
    }

    if ( m_RawDeviceEnd.RawValue() < m_LogInfoPage.ReadCycleRelatedEndOffset().RawValue() )
    {
        Delete();
        return Log_LogSizeHasDecreased;
    }

	// PTS 1125372 UH 2003-11-11 removed db version check

    m_LogInfoPage.EnterRegion(taskid);

    if ( RestartForFormat == restartType )
    {
        tkb00_SaveptParam &SaveptParam = k57restartrec->rstLastSavept_kb00();
        IOSequenceNo       SVPIOSeq = SaveptParam.svpIOsequence_kb00;
        RawDeviceOffset    SVPOffset;      // PTS 1119223 mb 2002-12-10 

        m_LogInfoPage.WriteOldestNotSavedIOSequenceNo ( SVPIOSeq  );
        m_LogInfoPage.WriteOldestNotSavedOffset       ( SVPOffset );

        if ( SVPIOSeq.IsValid() )
            --SVPIOSeq;
        m_LogInfoPage.WriteLastKnownIOSequenceNo      ( SVPIOSeq  );
        m_LogInfoPage.WriteLastKnownOffset            ( SVPOffset );
        m_LogInfoPage.ResetSavepointPart              (SVPIOSeq, SVPOffset);
    }

    if ( RestartForFormat != restartType )
    {
        tkb00_SaveptParam &SaveptParam = k57restartrec->rstLastSavept_kb00();
        IOSequenceNo       firstKnownSeqno;
        RawDeviceOffset    firstKnownOffset;

        if ( RestartForStandby != restartType )
        {
            // for a normal restart/restore check if a CLEAR LOG is pending and finish it
            switch ( m_LogInfoPage.ReadDeviceState() )
            {
                case Log_DeviceStateClearing:
                    FormatLog(taskid, LogicalLogBeginOffset(), LogicalLogEndOffset());
                    m_LogInfoPage.WriteClearUntilLowerOffset ( Log_RawDeviceOffset());     // set to invalid
                    m_LogInfoPage.WriteClearUntilUpperOffset ( Log_RawDeviceOffset());     // set to invalid
                    
                    m_LogInfoPage.WriteLastKnownIOSequenceNo(SaveptParam.svpIOsequence_kb00);
                    m_LogInfoPage.WriteLastKnownOffset(Log_RawDeviceOffset());
                    m_LogInfoPage.WriteOldestNotSavedOffset(Log_RawDeviceOffset());
                    m_LogInfoPage.WriteOldestNotSavedIOSequenceNo(Log_IOSequenceNo());
                    m_LogInfoPage.WriteOldestKnownIOSequenceNo(Log_IOSequenceNo());
                    m_LogInfoPage.WriteDeviceState ( Log_DeviceStateCleared );
                    k38history_lost_write(taskid);
                    break;
                case Log_DeviceStatePartiallyClearing:
                    FormatLog ( taskid, m_LogInfoPage.ReadClearUntilLowerOffset(),
                                        m_LogInfoPage.ReadClearUntilUpperOffset());
                    m_LogInfoPage.WriteClearUntilLowerOffset ( Log_RawDeviceOffset());     // set to invalid
                    m_LogInfoPage.WriteClearUntilUpperOffset ( Log_RawDeviceOffset());     // set to invalid
                    m_LogInfoPage.WriteLastKnownIOSequenceNo(SaveptParam.svpIOsequence_kb00);
                    m_LogInfoPage.WriteLastKnownOffset(SaveptParam.svpStartOffset_kb00);
                    m_LogInfoPage.WriteDeviceState ( Log_DeviceStatePartiallyCleared );
                    k38history_lost_write(taskid);
                    break;
                case Log_DeviceStateHistoryLost:
                    // if HistoryLost no restore log is allowed, because no DB-Ident is set
                    // and therefore no compatibility check can be done
                    if (RestartForRestore == restartType)
                    {
                        Delete();
                        m_LogInfoPage.LeaveRegion(taskid);
                        errlist.AppendNewMessage(Log_Exception(__CONTEXT__, LOG_RESTORE_NEEDS_CLEARED_LOG));
                        errlist.AddDetail       (SDBMSG_LOG_LOG_AND_DATA_INCOMPATIBLE,
                                                 Msg_Arg(SDBMSGTAG_LOG_LOG_AND_DATA_INCOMPATIBLE__REASON,"Restore Log with Log State 'HistoryLost' is not allowed"));
                        return Log_LogAndDataIncompatible;
                    }
                    break;
                default:
                    break;
            } // end switch DeviceState
        } // end not RestartForStandby

        result = UpdateLogInfoPage ( taskid, errlist );

        if (result != Log_Ok)
        {
            Delete();
            m_LogInfoPage.LeaveRegion(taskid);
            return result;
        }
        // PTS 1128080 mb 2004-02-26
        ScanLogForFirstKnown(taskid);

        if ( RestartNormal == restartType 
             ||
             RestartForStandby == restartType )
        {
            result = CheckLogAndDataCompatibility ( taskid, errlist );
            
            if (result != Log_Ok)
            {
                Delete();
                m_LogInfoPage.LeaveRegion(taskid);
                return result;
            }

            if ( RestartNormal == restartType
                 &&
                 LogIsEmpty() )
            {
                // PTS 1130780 UH 2004-07-27
                // Reset IOSequence only if cleared log and normal restart is requested
                tkb00_SaveptParam &SaveptParam = k57restartrec->rstLastSavept_kb00();
                SaveptParam.svpIOsequence_kb00  = Log_IOSequenceNo().RawValue();
                SaveptParam.svpStartOffset_kb00 = Log_RawDeviceOffset().RawValue();
            }
        }
    }
    
    FlushLogInfoPage (taskid);
    m_LogInfoPage.LeaveRegion(taskid);

    if ( m_State.IsReadyForWriting() )
        ResetLogWriter (taskid);
    else
    {
        if ( ! InitLogWriter( m_LogInfoPage.ReadLastKnownOffset(),
                              m_LogInfoPage.ReadLastKnownIOSequenceNo(),
                              ioblockcount) )
        {
            Delete();
            return Log_RawAllocationFailed;
        }

        // resume writer task after that archive log has been allocated
        m_WriterTaskSync.Resume();
    }

    Log_StatisticManager.Initialize(taskid);
    if ( addLogDevSpaceIsPending )
    {
        m_LogWriter.SetDeviceEnd (maxOffsetFromIoMan); // PTS 1124854 UH 2003-10-20
        RTE_Message(Log_Exception(__CONTEXT__, LOG_ADD_DEVSPACE_PENDING,
                                  SAPDB_ToString(m_LogInfoPage.ReadCycleRelatedEndOffset()),
                                  SAPDB_ToString(maxOffsetFromIoMan) ));
    }

    // The size check must be done after logwriter is initialized
    // because only the logwriter gives the correct log size information.
    if ( m_RawDeviceEnd.RawValue() < m_RawDeviceBegin.RawValue() + m_LogWriter.CalculateReservedPages(m_LogInfoPage.ReadQueueCount()*queuesize) )
    {
		errlist = Log_Exception(__CONTEXT__, LOG_VOLUME_TOO_SMALL,
                                   SAPDB_ToString(m_RawDeviceEnd),
                                   SAPDB_ToString(m_RawDeviceBegin),
                                   SAPDB_ToString(m_LogWriter.CalculateReservedPages(m_LogInfoPage.ReadQueueCount()*queuesize)),
                                   SAPDB_ToString(m_LogInfoPage.ReadQueueCount()),
                                   SAPDB_ToString(queuesize) );
        Delete();
        return Log_LogDeviceTooSmall;
    }
    
    if ( LogVolume_Trace.TracesLevel(6) )
        WriteDeviceParametersToTrace ();

    RTE_Message(Log_Exception(__CONTEXT__, LOG_DEVICE_INFO_BASIC,
                              SAPDB_ToString(m_LogInfoPage.ReadQueueCount(),_T_d),
                              Log_FlushModeStrings[m_LogInfoPage.ReadFlushMode()],
                              Log_DeviceStateStrings[m_LogInfoPage.ReadDeviceState()]));
    RTE_Message(Log_Exception(__CONTEXT__, LOG_DEVICE_INFO_OLDEST_NOT_SAVED,
                              SAPDB_ToString(m_LogInfoPage.ReadOldestNotSavedIOSequenceNo(),_T_d),
                              SAPDB_ToString(m_LogInfoPage.ReadOldestNotSavedOffset(),_T_d)));
    if (m_FirstKnownIOSeq.IsValid())
    {
        RTE_Message(Log_Exception(__CONTEXT__, LOG_DEVICE_INFO_FIRST_KNOWN,
                                  SAPDB_ToString(m_FirstKnownIOSeq),
                                  SAPDB_ToString(m_FirstKnownOffset)));
    }
    else
    {
        RTE_Message(Log_Exception(__CONTEXT__, LOG_DEVICE_INFO_UNCOMPLETE_CYCLE));
    }

    RTE_Message(Log_Exception(__CONTEXT__, LOG_RESTART_INFO,
                              SAPDB_ToString(k57restartrec->rstLastSavept_kb00().svpIOsequence_kb00,_T_d),
                              SAPDB_ToString(k57restartrec->rstLastSavept_kb00().svpStartOffset_kb00,_T_d),
                              SAPDB_ToString(m_LogInfoPage.ReadLastKnownIOSequenceNo(),_T_d),
                              SAPDB_ToString(m_LogInfoPage.ReadLastKnownOffset(),_T_d)));
    RTE_Message(Log_Exception(__CONTEXT__, LOG_CHECK_RESULT,
                              Log_ErrorStateStrings[result]));

    if ( result == Log_Ok )
        m_State.SetReadyForWriting();

    return result;
}


// -----------------------------------------------------------------------------

Log_ErrorState Log_Volume::RestartForSaveLogCold ( TaskID     taskid,
                                                   Msg_List  &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::RestartForSaveLogCold", LogVolume_Trace, 5);

    // Open all log volumes if necassary
    if ( ! OpenLogArea(taskid) )
        return Log_AreaOpenError;
    
    const SAPDB_UInt      queuesize    = QueueSize();
    const SAPDB_UInt      ioblockcount = IOMan_ILogInfo::GetInstance().GetLogIOBlockCount();
          Log_ErrorState  result       = Log_Ok;
          IOSequenceNo    firstKnownSeqno;
          RawDeviceOffset firstKnownOffset;

    m_RawDeviceBegin   = LOG_BEGIN_OFFSET;
    m_RawDeviceEnd     = IOMan_ILogInfo::GetInstance().TotalUsableLogPages() - 1;
    m_autosaveDistance = g01autosave_distance; // PTS 1113361 mb 2002-01-11

    if ( ! AllocateAllAndReadInfoPage(taskid, ioblockcount, queuesize, 0) )
    {
        Delete();
        return Log_PageAllocationFailed;
    }

    if ( m_LogInfoPage.ReadDeviceState() == Log_DeviceStateClearing
         ||
         m_LogInfoPage.ReadDeviceState() == Log_DeviceStatePartiallyClearing ) //PTS 1111525 mb 2002-01-29
    {
        errlist = Log_Exception(__CONTEXT__,LOG_SAVE_FOR_ABORTED_CLEAR_LOG);
        return Log_SaveForAbortedClearLog;
    }

    if ( m_RawDeviceEnd.RawValue() < m_RawDeviceBegin.RawValue() + m_autosaveDistance ) 
    {
		errlist = Log_Exception(__CONTEXT__, LOG_SAVE_DISTANCE_TOO_SMALL,
                                   SAPDB_ToString(m_RawDeviceEnd),
                                   SAPDB_ToString(m_RawDeviceBegin),
                                   SAPDB_ToString(m_autosaveDistance) );
        Delete();
        return Log_LogSaveDistanceTooLarge;
    }

    if ( m_RawDeviceEnd < m_LogInfoPage.ReadCycleRelatedEndOffset() )
    {
        Delete();
        return Log_LogSizeHasDecreased;
    }

    m_LogInfoPage.EnterRegion(taskid);
    
    result = UpdateLogInfoPage ( taskid, errlist );

    if (result != Log_Ok)
    {
        Delete();
        m_LogInfoPage.LeaveRegion(taskid);
        return result;
    }

    ScanLogForFirstKnown( taskid );    
    
    FlushLogInfoPage (taskid);
    
    m_LogInfoPage.LeaveRegion(taskid);

    return result;
}


// -----------------------------------------------------------------------------

Log_ErrorState Log_Volume::CheckLogAndDataCompatibility    (  TaskID     taskid,
                                                              Msg_List  &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CheckLogAndDataCompatibility", LogVolume_Trace, 5);

    if ( LogIsEmpty() ) //PTS 1113550 mb 2002-03-27
        return Log_Ok;

    // ----------------------------------------------------------------------
    // check if the DB_Identifier of Log and Data are compatible (PTS1113550 mb 2002-05-02)
    // ----------------------------------------------------------------------
    tsp00_Line &rstDBIdent = k57restartrec->rstDbIdent_kb00();

    if (m_LogInfoPage.ReadDBIdentifier()[0] != '\0' &&              //PTS 1113550 mb 2002-03-27
        rstDBIdent[0] != '\0')
    {
        if (memcmp(m_LogInfoPage.ReadDBIdentifier(), &rstDBIdent, sizeof(tsp00_Line)))
        {
            SAPDB_Char dataDBIdent[sizeof(tsp00_Line) + 1];
            SAPDB_Char logDBIdent[sizeof(tsp00_Line) + 1];
            
            sql47_ptoc(logDBIdent, (SAPDB_Char*)m_LogInfoPage.ReadDBIdentifier(), sizeof(tsp00_Line));
            sql47_ptoc(dataDBIdent, &rstDBIdent, sizeof(tsp00_Line));
            errlist.AppendNewMessage(Log_Exception(__CONTEXT__,LOG_DBIDENT_MISMATCH, dataDBIdent, logDBIdent));
            errlist.AddDetail       (SDBMSG_LOG_LOG_AND_DATA_INCOMPATIBLE,
                                     Msg_Arg(SDBMSGTAG_LOG_LOG_AND_DATA_INCOMPATIBLE__REASON,"The DB Identifiers are not equal"));
            return Log_LogAndDataIncompatible;
        }
    }
    else if (m_LogInfoPage.ReadDBIdentifier()[0] == '\0' &&
             rstDBIdent[0] != '\0')
    {
        m_LogInfoPage.WriteDBIdentifier(rstDBIdent);
    }

    // ----------------------------------------------------------------------
    // check if the restart record savepoint points within the log device.
    // ----------------------------------------------------------------------
    tkb00_SaveptParam &SaveptParam    = k57restartrec->rstLastSavept_kb00();
    Log_IOSequenceNo   lastKnownSeqNo = m_LogInfoPage.ReadLastKnownIOSequenceNo();
    Log_IOSequenceNo   savepointIOSeq   (SaveptParam.svpIOsequence_kb00);

    if ( (static_cast<IOSequenceNo>(SaveptParam.svpIOsequence_kb00)).IsValid() )
    {
        if ( lastKnownSeqNo.IsValid()
             &&
             !savepointIOSeq.IsInRange(m_FirstKnownIOSeq, lastKnownSeqNo)) // PTS 1128080 mb 2004-02-26
        {
            errlist = Msg_List(Msg_List::Error,	SDBMSG_LOG_LOG_AND_DATA_INCOMPATIBLE,
                               Msg_Arg(SDBMSGTAG_LOG_LOG_AND_DATA_INCOMPATIBLE__REASON,"Restart position not found"));
            errlist.AddDetail (SDBMSG_LOG_IOSEQUENCE_OUT_OF_RANGE,
                               Msg_Arg(SDBMSGTAG_LOG_IOSEQUENCE_OUT_OF_RANGE__CURRENT_IOSEQUENCE, SAPDB_ToString(savepointIOSeq)),
                               Msg_Arg(SDBMSGTAG_LOG_IOSEQUENCE_OUT_OF_RANGE__FIRST_IOSEQUENCE,   SAPDB_ToString(m_FirstKnownIOSeq)),
                               Msg_Arg(SDBMSGTAG_LOG_IOSEQUENCE_OUT_OF_RANGE__LAST_IOSEQUENCE,    SAPDB_ToString(lastKnownSeqNo)) );
            return Log_LogAndDataIncompatible;
        }

        if ( Log_RawDeviceOffset(SaveptParam.svpStartOffset_kb00).IsInvalid())
        {
            SaveptParam.svpStartOffset_kb00 = SearchSavepointOffset(taskid, savepointIOSeq).RawValue();
        }
        if ( ! CheckOffsetAndIOsequence ( taskid,
                                          SaveptParam.svpStartOffset_kb00,
                                          savepointIOSeq,
                                          errlist ) )
        {
            errlist = errlist + Log_Exception(__CONTEXT__,LOG_SAVEPOINT_NOT_FOUND);
            return Log_SavepointPositionNotFound;
        }
    }

    return Log_Ok;
}

// -----------------------------------------------------------------------------

bool Log_Volume::RepairYoungestPage(
          TaskID                         taskid,
    const Log_IOSequenceNo               lastKnownQueueSeqNo,
          RawDeviceIterator              iter,
          Log_Page                      &corruptPage,
          Log_IOSequenceNo              &lastKnownSeqNo,
          Log_RawDeviceOffset           &lastKnownOffset,
          Msg_List                      &errlist ) // UH 2005-10-18 added errlist and return bool
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::RepairYoungestPage", LogVolume_Trace, 5);

    const Log_IOSequenceNo firstIOSeqOfNextPage = corruptPage.ReadLastWriterIOSequenceNo();
    // remove information from last (incomplete) IO
    corruptPage.Truncate(--corruptPage.ReadLastWriterIOSequenceNo());

    Kernel_Page::PageFrame frame = m_FrameAlloc.New();
    if (!frame.IsAssigned())
    {
        errlist.AppendNewMessage(Log_Exception(__CONTEXT__,LOG_NOT_ENOUGH_PAGEFRAMES));
        return false;
    }

    // read the next page
    Log_Page            successorPage;
    Log_RawDeviceOffset offsetOfBadPage = iter.GetPosition();

    successorPage.SetFrame(frame);

    ++iter;
    m_PageReader.ReadLogPage( taskid, successorPage, Log_ClusterAddress(iter.GetPosition(),1));

    if (corruptPage.IsEmpty())
    {
        if (!successorPage.IsEmpty())
        {
            if (firstIOSeqOfNextPage == successorPage.ReadFirstWriterIOSequenceNo())
            {
                errlist.AppendNewMessage(Log_Exception(__CONTEXT__, LOG_REPAIR_PAGE_FAILED,
                                          SAPDB_ToString(offsetOfBadPage),
                                          SAPDB_ToString(lastKnownSeqNo),
                                          SAPDB_ToString(successorPage.ReadFirstWriterIOSequenceNo()) ));
                return false;
            }
        }
    }
    else
    {
        if (Log_VolumeIterator::consistent
            !=
            Log_VolumeIterator::CheckLogDeviceConsistency ( corruptPage,
                                                            offsetOfBadPage,
                                                            lastKnownSeqNo,
                                                            lastKnownQueueSeqNo,
                                                            errlist))
        {
            errlist.Overrule(Log_Exception(__CONTEXT__,LOG_CONSISTENT_EXPECTED));
            return false;
        }

        if (Log_VolumeIterator::endOfLogDevice
            !=
            Log_VolumeIterator::CheckLogDeviceConsistency ( successorPage,
                                                            iter.GetPosition(),
                                                            corruptPage.ReadLastWriterIOSequenceNo(),
                                                            corruptPage.ReadQueueIOSequenceNo(),
                                                            errlist))
        {
            errlist.Overrule(Log_Exception(__CONTEXT__,LOG_END_OF_LOG_EXPECTED));
            return false;
        }

        lastKnownSeqNo  = corruptPage.ReadLastWriterIOSequenceNo();
        lastKnownOffset = offsetOfBadPage;
    }
    
    // write the repaired page back onto the LogVolume
    if ( ! corruptPage.PrepareForWrite() )
        return false;

    m_PageWriter.WriteLogPage(taskid, corruptPage, Log_ClusterAddress(offsetOfBadPage, 1));

    RTE_Message( Log_Exception(__CONTEXT__, LOG_PAGE_REPAIRED,
                               SAPDB_ToString(offsetOfBadPage),
                               SAPDB_ToString(corruptPage.ReadLastWriterIOSequenceNo())));

    m_FrameAlloc.Free(frame);
    successorPage.ReleaseFrame();

    return true;
}

// -----------------------------------------------------------------------------
Log_ErrorState Log_Volume::UpdateLogInfoPage (TaskID    taskid,
                                              Msg_List &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::UpdateLogInfoPage", LogVolume_Trace, 5);
    
    m_LogInfoPage.WriteLogFlushMode(m_FlushMode);
    m_LogInfoPage.WriteQueueCount(g01logqueuecount());

    if ( (Log_DeviceStateOkay        != m_LogInfoPage.ReadDeviceState() &&
          Log_DeviceStateHistoryLost != m_LogInfoPage.ReadDeviceState())
          ||  
          LogIsEmpty()  )
    {
        errlist = Log_Exception(__CONTEXT__, LOG_NO_INFOPAGE_UPD,
                                  Log_DeviceStateStrings[m_LogInfoPage.ReadDeviceState()],
                                  SAPDB_ToString(LogIsEmpty(),_T_b) );
        return Log_Ok;
    }

    // ----------------------------------------------------------------------
    // check software version first
    // ----------------------------------------------------------------------
    // PTS 1125372 UH 2003-11-11 check moved here to be executed only if log was not cleared
    if ( m_LogInfoPage.ReadDBVersion()[0] != '\0'
         &&
         ! Kernel_IAdminConfig::Instance().MigrationHandler().
             CheckCompatibility( Kernel_Migration::log,
                                 Kernel_Version(m_LogInfoPage.ReadDBVersion()),
                                 errlist) )
    {
        Delete();
        return Log_LogAndSoftwareIncompatible;
    }
    

    // ----------------------------------------------------------------------
    // check consistency of info page and log device
    // ----------------------------------------------------------------------
    if ( ! CheckOffsetAndIOsequence ( taskid,
                                      m_LogInfoPage.ReadLastKnownOffset(),
                                      m_LogInfoPage.ReadLastKnownIOSequenceNo(),
                                      errlist ) )
    {
        errlist = errlist + Log_Exception(__CONTEXT__,LOG_LASTKNOWN_NOT_FOUND);
        return Log_InconsistentLogInfoPage;
    }

    // ----------------------------------------------------------------------
    // find youngest log page and perhaps the oldest log page (cycle first)
    // ----------------------------------------------------------------------

    const SAPDB_UInt   maxloops           = m_LogInfoPage.ReadCycleRelatedEndOffset().RawValue() -
                                            LogicalLogBeginOffset().RawValue();
    Log_VolumeIterator::CheckLogDeviceResult checkresult;
    SAPDB_UInt         loopcount          = 0;
    
    // imagine values of previous page for first check
    RawDeviceIterator  iter                 ( LogicalLogBeginOffset(),
                                              m_LogInfoPage.ReadLastKnownOffset(),
                                              m_LogInfoPage.ReadCycleRelatedEndOffset() );
    --iter;
    Log_RawDeviceOffset lastKnownOffset    = iter.GetPosition();
    Log_IOSequenceNo    lastKnownIOSeqNo   = --(m_BufferedPage.ReadFirstWriterIOSequenceNo());
    Log_IOSequenceNo    lastKnownQueueSeq  = --(m_BufferedPage.ReadQueueIOSequenceNo());
    ++iter;

    do
    {
        Log_ClusterAddress ioaddress(lastKnownOffset,1);

        if (m_BufferedPage.CheckAfterRead())
        {
            checkresult = Log_VolumeIterator::CheckLogDeviceConsistency
                                ( m_BufferedPage,
                                  ioaddress.GetOffset(),
                                  lastKnownIOSeqNo,
                                  lastKnownQueueSeq,
                                  errlist);
                                  
            switch ( checkresult )
            {
                case Log_VolumeIterator::inconsistent:
                    return Log_InconsistentLogDeviceIOSequence;
                case Log_VolumeIterator::endOfLogDevice:
                    if ( m_BufferedPage.ReadLastWriterIOSequenceNo().IsValid() )
                    {
                        // PTS 1119223 mb 2002-12-10 save values direct in own members 
                        m_FirstKnownIOSeq  = m_BufferedPage.ReadFirstWriterIOSequenceNo(); // PTS 1115226 mb 2002-04-15
                        m_FirstKnownOffset = iter.GetPosition();
                    }
                    break;
                default: // case Log_VolumeIterator::consistent:
                    // save values of current page
                    lastKnownIOSeqNo    = m_BufferedPage.ReadLastWriterIOSequenceNo();
                    lastKnownQueueSeq = m_BufferedPage.ReadQueueIOSequenceNo();
                    lastKnownOffset   = iter.GetPosition();
                    
                    // read next page
                    ++iter; 
                    ++loopcount;
                    ioaddress.SetOffset(iter.GetPosition());
                    m_PageReader.ReadLogPageWithoutCheck( taskid, m_BufferedPage, ioaddress );
                    break;
            }
        }
        else
        // repair page if possible (PTS 1125456 mb 2003-09-29)
        {
            if ( ! RepairYoungestPage(taskid,
                               lastKnownQueueSeq,
                               iter,
                               m_BufferedPage,
                               lastKnownIOSeqNo,
                               lastKnownOffset,
                               errlist) )
            {
                Kernel_IAdminRestartShutdown::Instance().Offline(errlist);
            }
            checkresult = Log_VolumeIterator::endOfLogDevice;
        }
    }
    while ( Log_VolumeIterator::consistent == checkresult
            &&
            loopcount < maxloops );

    if ( loopcount >= maxloops )
    {
        errlist = Log_Exception(__CONTEXT__,LOG_CYCLE_NOT_FOUND);
        return Log_InconsistentLogDeviceIOSequence;
    }

    // last known is found now
    m_LogInfoPage.WriteLastKnownIOSequenceNo(lastKnownIOSeqNo);
    m_LogInfoPage.WriteLastKnownOffset(lastKnownOffset);

    if ( LogVolume_Trace.TracesLevel(6) )
        WriteDeviceParametersToTrace ();

    // ----------------------------------------------------------------------
    // check if the log save history is consistent
    // ----------------------------------------------------------------------

    if ( m_LogInfoPage.ReadOldestNotSavedIOSequenceNo().IsValid()
         &&
         m_LogInfoPage.ReadOldestNotSavedOffset().IsValid()
         &&
         ! CheckOffsetAndIOsequence ( taskid,
                                      m_LogInfoPage.ReadOldestNotSavedOffset(),
                                      m_LogInfoPage.ReadOldestNotSavedIOSequenceNo(),
                                      errlist ) )
    {
        // additional check if a cold log save was done
        // then it is allowed, that oldestNotSaved is exactly the next offset after
        // last known
        iter.SetPosition (m_LogInfoPage.ReadLastKnownOffset());
        ++iter;
        if ( m_LogInfoPage.ReadOldestNotSavedOffset()
             !=
             iter.GetPosition() )
        {
            errlist = errlist + Log_Exception(__CONTEXT__,LOG_OLDEST_NOT_SAVED_NOT_FOUND);
            return Log_InconsistentLogInfoPage;
        }
        errlist.ClearMessageList();
    }

    return Log_Ok;
}

// -----------------------------------------------------------------------------

void Log_Volume::ScanLogForFirstKnown(TaskID           taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ScanLogForFirstKnown", LogVolume_Trace, 5);
    // PTS 1119223 mb 2002-12-10 SetFirstKnownBeforeRestoreLog renamed and rewritten

    if ( Log_DeviceStateCleared  == m_LogInfoPage.ReadDeviceState()
         ||  
         LogIsEmpty() )
    {
        // an empty log need not to be scanned
        m_FirstKnownIOSeq.Invalidate();
        m_FirstKnownOffset.Invalidate();
        m_LogInfoPage.WriteOldestKnownIOSequenceNo(m_FirstKnownIOSeq);
        return;
    }

    // FirstKnown can already be set by UpdateLogInfoPage, if it has  been found by the way. 

    if ( m_FirstKnownIOSeq.IsInvalid() )
    {
        // first, look at first offset of the device
        RawDeviceIterator  iter( LogicalLogBeginOffset(),
                                 LogicalLogBeginOffset(),
                                 m_LogInfoPage.ReadCycleRelatedEndOffset() );
        m_PageReader.ReadLogPage ( taskid, m_BufferedPage,
                                   Log_ClusterAddress(iter.GetPosition(),1) );
        IOSequenceNo IOSequenceOfFirstOffset = m_BufferedPage.ReadFirstWriterIOSequenceNo();

        // second, look at last offset and then at last offset of the device
        --iter;
        m_PageReader.ReadLogPage ( taskid, m_BufferedPage,
                                   Log_ClusterAddress(iter.GetPosition(),1) );
        IOSequenceNo IOSequenceOfLastOffset = m_BufferedPage.ReadFirstWriterIOSequenceNo();

        if ( IOSequenceOfLastOffset.IsInvalid() && IOSequenceOfFirstOffset.IsValid() ) // 2002-02-15 mb pts1111525 partially cleared log
        {
            // In this case the log is not written around
            m_FirstKnownIOSeq  = IOSequenceOfFirstOffset;
            m_FirstKnownOffset = LogicalLogBeginOffset();
        }
        else
        {
            // the log has been written around, but may also be partially cleared, so
            // search from lastknown forward to first valid -> this must be the smallest io-seq
            iter.SetPosition (m_LogInfoPage.ReadLastKnownOffset());
            ++iter;
            m_PageReader.ReadLogPage ( taskid, m_BufferedPage,
                                       Log_ClusterAddress(iter.GetPosition(),1) );
            while ( m_BufferedPage.ReadFirstWriterIOSequenceNo().IsInvalid() )
            {
                ++iter;
                m_PageReader.ReadLogPage ( taskid, m_BufferedPage,
                                           Log_ClusterAddress(iter.GetPosition(),1) );
            }
            m_FirstKnownIOSeq  = m_BufferedPage.ReadFirstWriterIOSequenceNo();
            m_FirstKnownOffset = iter.GetPosition();
        }
    }
    m_LogInfoPage.WriteOldestKnownIOSequenceNo(m_FirstKnownIOSeq);
}
// -----------------------------------------------------------------------------

Log_RawDeviceOffset 
Log_Volume::SearchSavepointOffset(      TaskID              taskid,
                                  const Log_IOSequenceNo    savepointIOSequence)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SearchSavepointOffset", LogVolume_Trace, 5);

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "searching savepointIOSequence: " << savepointIOSequence );

    Log_RawDeviceOffset   foundSavepointOffset;

    if (savepointIOSequence.IsValid()
        &&
        ! LogIsEmpty()
        &&
        savepointIOSequence.IsInRange(m_FirstKnownIOSeq, GetLastKnownIOSeq()))
    {
        m_PageReader.ReadLogPage ( taskid, m_BufferedPage, Log_ClusterAddress(LogicalLogBeginOffset(),1) );
        IOSequenceNo ioSequenceOfFirstOffset = m_BufferedPage.ReadFirstWriterIOSequenceNo();
        
        Log_RawDeviceIterator leftBound (LogicalLogBeginOffset(),LogicalLogBeginOffset(),LogicalLogEndOffset() );
        Log_RawDeviceIterator rightBound(LogicalLogBeginOffset(),LogicalLogBeginOffset(),LogicalLogEndOffset() );
    
        if (Log_IOSequenceNo::LeftIsOlder(ioSequenceOfFirstOffset, savepointIOSequence, GetOldestKnownIOSeq()))
        {
            leftBound.SetPosition ( LogicalLogBeginOffset() );
            rightBound.SetPosition( m_LogInfoPage.ReadLastKnownOffset() );
        }
        else
        {
            leftBound.SetPosition ( m_FirstKnownOffset );
            rightBound.SetPosition( LogicalLogEndOffset() );
        }
    
        Log_RawDeviceOffset middle;
        while (leftBound.GetDistanceTo(rightBound.GetPosition()) > 1)
        {
            middle = (leftBound + (leftBound.GetDistanceTo(rightBound.GetPosition()) / 2)).GetPosition();
            m_PageReader.ReadLogPage ( taskid, m_BufferedPage, Log_ClusterAddress(middle,1) );
            if (Log_IOSequenceNo::LeftIsOlder(savepointIOSequence, m_BufferedPage.ReadFirstWriterIOSequenceNo(), GetOldestKnownIOSeq()))
            {
                rightBound.SetPosition(middle);
            }
            else
            {
                leftBound.SetPosition(middle);
            }
        }
        
        m_PageReader.ReadLogPage ( taskid, m_BufferedPage, Log_ClusterAddress(leftBound.GetPosition(),1) );
        if (savepointIOSequence.IsInRange(m_BufferedPage.ReadFirstWriterIOSequenceNo(), 
                                          m_BufferedPage.ReadLastWriterIOSequenceNo()))
        {
            foundSavepointOffset = leftBound.GetPosition();
        }
        else
        {
            m_PageReader.ReadLogPage ( taskid, m_BufferedPage, Log_ClusterAddress(rightBound.GetPosition(),1) );
            if (savepointIOSequence.IsInRange(m_BufferedPage.ReadFirstWriterIOSequenceNo(), 
                                              m_BufferedPage.ReadLastWriterIOSequenceNo()))
            {
                foundSavepointOffset = rightBound.GetPosition();
            }
        }
    }


    if (foundSavepointOffset.IsValid())
    {
        RTE_Message(Log_Exception(__CONTEXT__,LOG_SAVEPOINT_FOUND,        
                                  SAPDB_ToString(savepointIOSequence,_T_d),
                                  SAPDB_ToString(foundSavepointOffset,_T_d)));
    }
    else
    {
        RTE_Message(Log_Exception(__CONTEXT__,LOG_SAVEPOINT_IOSEQ_NOT_FOUND,
                                  SAPDB_ToString(savepointIOSequence,_T_d)));
    }
    
    return foundSavepointOffset;

}

// -----------------------------------------------------------------------------

SAPDB_Bool
Log_Volume::CheckOffsetAndIOsequence (TaskID          taskid,
                                      RawDeviceOffset offset,
                                      IOSequenceNo    seqno,
                                      Msg_List        &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CheckOffsetAndIOsequence", LogVolume_Trace, 5);

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "offset: " << offset << ", sequence: " << seqno);
    SAPDB_Bool sequenceFound;
    if ( offset.IsInvalid() )
        sequenceFound = false;
    else
    {
        if (offset <= LogicalLogEndOffset())                    // PTS 1115870 mb 2002-05-23
        {
            m_PageReader.ReadLogPageWithoutCheck (taskid, m_BufferedPage,Log_ClusterAddress(offset,1)); // PTS 1124456 mb 2003-10-02
            if (!m_BufferedPage.CheckAfterRead()) 
            {
                errlist = Log_Exception(__CONTEXT__, LOG_PAGECHECK_FAILED,
                                          SAPDB_ToString(offset),
                                          SAPDB_ToString(m_BufferedPage.ReadFirstWriterIOSequenceNo(),_T_d),
                                          SAPDB_ToString(m_BufferedPage.ReadLastWriterIOSequenceNo(),_T_d) );
            }
            sequenceFound = m_BufferedPage.IOSequenceIsOnPage(seqno); // PTS 1124727 mb 2003-10-16
        }
        else
        {
            sequenceFound = false;
        }
    }
    if ( ! sequenceFound )
        errlist = Log_Exception(__CONTEXT__, LOG_SEQUENCE_NOT_FOUND,
                     SAPDB_ToString(seqno,_T_d),
                     SAPDB_ToString(m_BufferedPage.ReadFirstWriterIOSequenceNo(),_T_d),
                     SAPDB_ToString(m_BufferedPage.ReadLastWriterIOSequenceNo(),_T_d),
                     SAPDB_ToString(offset,_T_d) );
    
    return sequenceFound;
}
// -----------------------------------------------------------------------------
SAPDB_Bool Log_Volume::AllocateAndReadInfoPage( tsp00_TaskId taskid,
                                                bool         readPage )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::AllocateAndReadInfoPage", LogVolume_Trace, 5);

    if ( ! m_State.IsLogAreaAccessible() )
        return false;
        
    if ( ! m_LogInfoPage.IsAssigned() )
    {
        Kernel_Page::PageFrame frame = m_FrameAlloc.New();
        if (!frame.IsAssigned())
            return false;
        m_LogInfoPage.SetFrame(frame);
    }
    if ( readPage )
    {
        m_LogInfoPage.EnterRegion(taskid);
        
        m_PageReader.ReadLogInfoPage ( taskid, m_LogInfoPage,
                                       Log_ClusterAddress(LogInfoPageOffset(),1) );
        
        m_LogInfoPage.LeaveRegion(taskid);
        
        m_State.SetLogInfoPageReadable();
    }
    return true;
}
// -----------------------------------------------------------------------------
SAPDB_Bool Log_Volume::AllocateAllAndReadInfoPage ( tsp00_TaskId taskid,
                                                    SAPDB_UInt   ioblockCount,
                                                    SAPDB_UInt   queueSize,
                                                    SAPDB_UInt   queueCount )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::AllocateAllAndReadInfoPage", LogVolume_Trace, 5);

    const bool isForLogCreation = queueCount != 0;
    
    if ( ! AllocateAndReadInfoPage(taskid, ! isForLogCreation) )
        return false;
        
    if ( ! m_BufferedPage.IsAssigned() )
    {
        Kernel_Page::PageFrame frame = m_FrameAlloc.New();
        if (!frame.IsAssigned())
            return false;
        m_BufferedPage.SetFrame(frame);
    }

    if (! m_IOPages.Initialize(ioblockCount) )
        return false;

    if ( ! isForLogCreation )
        queueCount = g01usedmaxlogqueuecount();

    m_CurrentQueueCount = g01logqueuecount();
    m_FlushMode = Log_DeviceMaximizeSafety;
    if ( ! CreateLogQueues( queueCount, queueSize) )
        return false;
        
    return true;
}
// -----------------------------------------------------------------------------
void Log_Volume::CreateInfoPage (tsp00_TaskId taskid,
                                 SAPDB_UInt queuecount)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CreateInfoPage", LogVolume_Trace, 5);

    Kernel_DateTime dateTime;
    dateTime.DetermineTime();

    m_LogInfoPage.EnterRegion(taskid);

    m_LogInfoPage.InitializeFrame( (Kernel_Page::Id)m_WriterTaskSync.GetTaskID() );

    m_LogInfoPage.WriteLogDate(dateTime.GetDate());
    m_LogInfoPage.WriteLogTime(dateTime.GetTime());
    m_LogInfoPage.WriteLogFlushMode(m_FlushMode);
    m_LogInfoPage.WriteDeviceState(Log_DeviceStateOkay);
    m_LogInfoPage.WriteQueueCount(queuecount);
    m_LogInfoPage.WriteCycleRelatedEndOffset(m_RawDeviceEnd);

    // invalid last known ioseqno indicates that log has never been written
    m_LogInfoPage.WriteLastKnownIOSequenceNo(IOSequenceNo());           // == invalid
    m_LogInfoPage.WriteLastKnownOffset      (RawDeviceOffset());        // == invalid

    m_LogInfoPage.WriteDevspaceEnabled(true); // PTS 1113190 UH 2001-12-19

    m_LogInfoPage.WriteOldestNotSavedOffset(LogicalLogBeginOffset());

    // invalid oldest not saved ioseqno indicates that log has never been saved
    m_LogInfoPage.WriteOldestNotSavedIOSequenceNo(IOSequenceNo());      // == invalid

    m_LogInfoPage.WriteOldestKnownIOSequenceNo(IOSequenceNo());      // == invalid
    m_LogInfoPage.WriteLogBackupCount(0);
    m_LogInfoPage.WriteRedoUntilDate(0);
    m_LogInfoPage.WriteRedoUntilTime(0);
    m_LogInfoPage.WriteClearUntilLowerOffset(RawDeviceOffset());        // inv.
    m_LogInfoPage.WriteClearUntilUpperOffset(RawDeviceOffset());        // inv.
    m_LogInfoPage.WriteIOSequenceNoValidAfterRedoUntil(IOSequenceNo()); // inv.

    tkb00_SaveptParam &SaveptParam = k57restartrec->rstLastSavept_kb00();
    m_LogInfoPage.WriteSavepointParameters(
        SAPDBFields_Field(
            reinterpret_cast<SAPDB_Byte*>(&SaveptParam), sizeof(SaveptParam) ) );

    // 2003-11-11 added initialization
    tsp00_Line dbIdent;
    dbIdent.SetZero();
    m_LogInfoPage.WriteDBIdentifier(dbIdent);

    // 2003-11-11 added initialization
    tsp00_Version dbVersion;
    dbVersion.SetZero();
    m_LogInfoPage.WriteDBVersion (dbVersion);

    m_LogInfoPage.WriteDevspaceEnabled(true);
    m_LogInfoPage.WriteAutoOverwrite  (false);
    
    RTE_Nodename unknownMasterNodeName;
    SAPDB_MemFillNoCheck (unknownMasterNodeName, 0, sizeof(unknownMasterNodeName));
    if ( ! m_LogInfoPage.WriteMasterNodeName (unknownMasterNodeName) )
        RTE_Crash(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                     "m_LogInfoPage.WriteMasterNodeName"));

    m_LogInfoPage.LeaveRegion(taskid);

    m_State.SetLogInfoPageReadable();
}
// -----------------------------------------------------------------------------
SAPDB_Bool Log_Volume::CreateLogQueues ( SAPDB_UInt queuecount,
                                         SAPDB_UInt queuesize)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CreateLogQueues", LogVolume_Trace, 5);

    if ( m_LogQueues.GetSize() == queuecount )
        return true; // If the correct queue size is allocated then nothing is to do
    if ( m_LogQueues.GetSize() != 0 )
        return false; // something is allocated but not the correct size is used - it is an error.
    
    // changed to vector-based log-queues with PTS 111135304 mb 2005-05-25)
    for (SAPDB_UInt qid=0; qid < queuecount; qid++)
    {
        Log_Queue* queue = new(m_Allocator)
            Log_Queue(qid, m_QueueSyncMode, m_FlushMode, m_WriterTaskSync,
                      m_Allocator, m_FrameAlloc);
        if (!m_LogQueues.InsertEnd(queue))
        {
            return false;
        }
        if (!m_LogQueues[qid]->Initialize(queuesize))
        {
            return false;
        }
    }
    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "queuecount: " << queuecount << ", queuesize: " << queuesize);
    return true;
    
}
// -----------------------------------------------------------------------------
void Log_Volume::ResetLogWriter (TaskID taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ResetLogWriter", LogVolume_Trace, 5);

    if ( m_LogInfoPage.ReadLastKnownOffset().IsValid() )
    {
        RawDeviceIterator iter ( LogicalLogBeginOffset(),
                                 m_LogInfoPage.ReadLastKnownOffset(),
                                 LogicalLogEndOffset());

        ++iter; // use next offset on device to begin writing
        m_LogWriter.SetState ( taskid,
                               m_LogInfoPage.ReadLastKnownIOSequenceNo(),
                               iter.GetPosition(),
                               LogicalLogEndOffset());
    }
    else
        // log was never been written
        m_LogWriter.SetState ( taskid,
                               m_LogInfoPage.ReadLastKnownIOSequenceNo(),
                               LogicalLogBeginOffset(),
                               LogicalLogEndOffset());
}
// -----------------------------------------------------------------------------
SAPDB_Bool Log_Volume::InitLogWriter (RawDeviceOffset offset,
                                      IOSequenceNo    seqno,
                                      SAPDB_UInt      ioblockcount)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::InitLogWriter", LogVolume_Trace, 5);

    RawDeviceIterator iter ( LogicalLogBeginOffset(),
                             LogicalLogBeginOffset(),
                             LogicalLogEndOffset());

    if ( offset.IsValid() )
    {
        iter.SetPosition(offset);
        ++iter; // use next offset on device to begin writing
    }

    return m_LogWriter.Initialize (seqno,
                                   iter,
                                   this,
                                   m_LogInfoPage,
                                   LogInfoPageOffset(),
                                   ioblockcount);
}

// -----------------------------------------------------------------------------

void Log_Volume::FormatLog                // new 2002-02-15 mb PTS 1111525
    (const TaskID                         taskid,
     const RawDeviceOffset                beginOffset,
     const RawDeviceOffset                endOffset)
{
    if (beginOffset <= endOffset)
    {
        FormatLogLinear(taskid, beginOffset, endOffset);
    }
    else
    {
        FormatLogLinear(taskid, beginOffset, LogicalLogEndOffset());
        FormatLogLinear(taskid, LogicalLogBeginOffset(), endOffset);
    }
    m_FirstKnownIOSeq.Invalidate();
    m_FirstKnownOffset.Invalidate();
}

// -----------------------------------------------------------------------------

void Log_Volume::FormatLogLinear
    (const TaskID                         taskid,
     const RawDeviceOffset                beginoffs,
     const RawDeviceOffset                endoffs)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::FormatLogLinear", LogVolume_Trace, 5);

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "beginoffs: " << beginoffs << ", endoffs: " << endoffs);

    SAPDBERR_ASSERT_RANGE(beginoffs, LogicalLogBeginOffset(), LogicalLogEndOffset());
    SAPDBERR_ASSERT_RANGE(endoffs, LogicalLogBeginOffset(), LogicalLogEndOffset());

    m_BufferedPage.InitializeFrame(Log_QueueID(), IOSequenceNo());

    RawDeviceIterator iter(LogicalLogBeginOffset(), beginoffs, LogicalLogEndOffset());
    RawDeviceIterator end (LogicalLogBeginOffset(), endoffs, LogicalLogEndOffset());


    RTE_Message( Log_Exception(__CONTEXT__, LOG_FORMATTING,
                               SAPDB_ToString(beginoffs,_T_d),
                               SAPDB_ToString(endoffs,_T_d) ));

    // fill i/o cluster with initalized pages:
    m_IOPages.Clear();
    while (!m_IOPages.IsFull())
        m_IOPages.Append(m_BufferedPage);

    // while blocks clusterwise as far as possible:
    while (iter.GetDistanceTo(end.GetPosition()) >= m_IOPages.GetPageCount())
    {
        m_PageWriter.WriteLogPages
            (taskid, m_IOPages, Log_ClusterAddress(iter.GetPosition(), m_IOPages.GetPageCount()));
        iter += m_IOPages.GetPageCount();
    }

    // write ultimate pages with single page i/o's:
    m_IOPages.Clear();
    m_IOPages.Append(m_BufferedPage);
    while (iter != end)
    {
        m_PageWriter.WriteLogPages
            (taskid, m_IOPages, Log_ClusterAddress(iter.GetPosition(), 1));
        ++iter;
    }
    m_PageWriter.WriteLogPages
        (taskid, m_IOPages, Log_ClusterAddress(iter.GetPosition(), 1));
}

// -----------------------------------------------------------------------------

void Log_Volume::ClearLogPartially
    (const TaskID                         taskid,
     const Log_RawDeviceOffset            firstOffsetToClear, 
     const Log_RawDeviceOffset            lastOffsetToClear,
     const Log_IOSequenceNo               lastIOSequenceOnLog,
           bool                          &firstPageIsEmpty)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ClearLogPartially", LogVolume_Trace, 5);

    if (firstOffsetToClear == lastOffsetToClear)
    // the until had been reached at the youngest page on the log
    {
        TruncateLogPage (taskid,
                 lastIOSequenceOnLog,
                 firstOffsetToClear,
                 firstPageIsEmpty);

        Log_RawDeviceOffset newOldestNotSavedOffset;
        if (!firstPageIsEmpty)
        {
            newOldestNotSavedOffset = firstOffsetToClear;
        }
        else
        {
            newOldestNotSavedOffset = (--Log_RawDeviceIterator(LogicalLogBeginOffset(),
                                                  firstOffsetToClear, 
                                                  LogicalLogEndOffset())).GetPosition();
        }
        ReadLogPage(taskid,m_BufferedPage,newOldestNotSavedOffset);
        m_LogInfoPage.WriteOldestNotSavedOffset(newOldestNotSavedOffset);
        m_LogInfoPage.WriteOldestNotSavedIOSequenceNo(m_BufferedPage.ReadFirstWriterIOSequenceNo());
    }
    else
    {
        TruncateLogPage (taskid,
                         lastIOSequenceOnLog,
                         firstOffsetToClear,
                         firstPageIsEmpty);
        Log_RawDeviceOffset realFirstClearedOffset;
        if (!firstPageIsEmpty)
        // the last read page is totally cleared
        {
            realFirstClearedOffset = (++Log_RawDeviceIterator(LogicalLogBeginOffset(),
                                                              firstOffsetToClear, 
                                                              LogicalLogEndOffset())).GetPosition();
        }
        else
        {
            realFirstClearedOffset = firstOffsetToClear;
        }

        Log_RawDeviceIterator    predOfFirst = --Log_RawDeviceIterator(LogicalLogBeginOffset(), 
                                                                      realFirstClearedOffset,
                                                                      LogicalLogEndOffset());
        if (predOfFirst.GetPosition() == lastOffsetToClear)
        {
            ClearLogComplete(taskid,lastIOSequenceOnLog);
            firstPageIsEmpty = true;
        }
        else
        {
            Log_RawDeviceOffset newOldestNotSavedOffset;
            Log_IOSequenceNo    newOldestNotSavedIOSeq;
            Log_IOSequenceNo    incrementedLastKnownIOSeq;
            
            if (m_LogInfoPage.ReadLastKnownIOSequenceNo().IsValid()) // PTS 1132069 mb 2004-10-21
            {
                incrementedLastKnownIOSeq = m_LogInfoPage.ReadLastKnownIOSequenceNo();
                ++incrementedLastKnownIOSeq ;
            }
            else
            {
                incrementedLastKnownIOSeq.Invalidate();
            }

            // check if oldestNotSavedOffset had been cleared
            // changed with PTS 1124727 mb 2003-10-30
            if (    // 1. Situation: realFirstOffsetToClear ... oldestNotSaved ... lastOffsetToClear
                    (    (realFirstClearedOffset < lastOffsetToClear)
                      && (m_LogInfoPage.ReadOldestNotSavedOffset() >= realFirstClearedOffset)
                      && (m_LogInfoPage.ReadOldestNotSavedOffset() <=      lastOffsetToClear))
                    ||
                    // 2. Situation:  lastOffsetToClear ... realFirstOffsetToClear ... oldestNotSaved 
                    (    (realFirstClearedOffset > lastOffsetToClear)
                      && (m_LogInfoPage.ReadOldestNotSavedOffset() >= realFirstClearedOffset)
                      && (m_LogInfoPage.ReadOldestNotSavedOffset() <= LogicalLogEndOffset()))
                    ||
                    // 3. Situation:  oldestNotSaved ... lastOffsetToClear ... realFirstOffsetToClear 
                    (    (realFirstClearedOffset > lastOffsetToClear)
                      && (m_LogInfoPage.ReadOldestNotSavedOffset() <= lastOffsetToClear))
                    ||
                    // 4. situation: oldestNotSaved is the only cleared page
                    (    (m_LogInfoPage.ReadOldestNotSavedOffset() == realFirstClearedOffset)
                      && (m_LogInfoPage.ReadOldestNotSavedOffset() ==      lastOffsetToClear))
                    ||
                    // 5. Situation: everything saved, oldestNotSaved point to never written page
                    //               PTS 1132069 mb 2004-10-21
                    (     incrementedLastKnownIOSeq.IsValid()
                      && (m_LogInfoPage.ReadOldestNotSavedIOSequenceNo() == incrementedLastKnownIOSeq) )
               )
            {
                newOldestNotSavedOffset = (--Log_RawDeviceIterator(LogicalLogBeginOffset(), 
                                                                   realFirstClearedOffset,  
                                                                   LogicalLogEndOffset())).GetPosition();
                ReadLogPage(taskid,m_BufferedPage,newOldestNotSavedOffset);
                newOldestNotSavedIOSeq = m_BufferedPage.ReadFirstWriterIOSequenceNo();
            }
            else
            {
                newOldestNotSavedOffset = m_LogInfoPage.ReadOldestNotSavedOffset();
                newOldestNotSavedIOSeq = m_LogInfoPage.ReadOldestNotSavedIOSequenceNo();
            }

            const SAPDB_Bool formatLog = SAPDB_TRUE;
            ClearLogIntern(taskid,
                           realFirstClearedOffset, 
                           lastOffsetToClear,
                           (--Log_RawDeviceIterator(LogicalLogBeginOffset(),    // LastKnownOffset is the predecessor
                                                    realFirstClearedOffset,     // of the first cleared page
                                                    LogicalLogEndOffset())).GetPosition(),
                           newOldestNotSavedOffset,
                           newOldestNotSavedIOSeq,
                           Log_DeviceStatePartiallyClearing, Log_DeviceStatePartiallyCleared,
                           formatLog);
        }
    }
}

// ----------------------------------------------------------------------------- // new PTS1113550 mb 2002-05-15
void Log_Volume::ClearLogComplete   (const TaskID                         taskid,
                                     const Log_IOSequenceNo               lastKnownIOSeqAfterClear)
{
    const SAPDB_Bool formatLog = SAPDB_TRUE;

    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ClearLogComplete", LogVolume_Trace, 5);
    ClearLogIntern(taskid, 
                   LogicalLogBeginOffset(), LogicalLogEndOffset(),
                   Log_RawDeviceOffset(),                          // LastKnownOffset = invalid
                   Log_RawDeviceOffset(),                          // OldestNotSavedOffset = invalid
                   lastKnownIOSeqAfterClear,                       // OldestNotSavedIOSeq is next new one
                   Log_DeviceStateClearing, Log_DeviceStateCleared,
                   formatLog);
}

// -----------------------------------------------------------------------------
void Log_Volume::ClearLogComplete   (const TaskID                         taskid)
{
    const SAPDB_Bool formatLog = SAPDB_TRUE;

    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ClearLogComplete", LogVolume_Trace, 5);
    ClearLogIntern(taskid, 
                   LogicalLogBeginOffset(), LogicalLogEndOffset(),
                   Log_RawDeviceOffset(),                          // LastKnownOffset = invalid
                   Log_RawDeviceOffset(),                          // OldestNotSavedOffset = invalid
                   ++(m_LogInfoPage.ReadLastKnownIOSequenceNo()),                             // OldestNotSavedIOSeq is next new one
                   Log_DeviceStateClearing, Log_DeviceStateCleared,
                   formatLog);
}

// -----------------------------------------------------------------------------

void Log_Volume::ClearLogIntern (                      // PTS 1111525 mb 2002-01-29 new
        const TaskID                          taskid,
        const Log_RawDeviceOffset             firstOffsetToClear, 
        const Log_RawDeviceOffset             lastOffsetToClear,
        const Log_RawDeviceOffset             lastKnownOffset,
        const Log_RawDeviceOffset             oldestNotSavedOffset,
        const Log_IOSequenceNo                oldestNotSavedIOSeq,
        const Log_DeviceState                 stateBeforeFormat,
        const Log_DeviceState                 stateAfterFormat,
        const SAPDB_Bool                      doFormatLog)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ClearLogIntern", LogVolume_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT (stateAfterFormat != Log_DeviceStateOkay);
    
	m_LogInfoPage.EnterRegion(taskid);
	m_LogInfoPage.WriteClearUntilLowerOffset      ( firstOffsetToClear );
	m_LogInfoPage.WriteClearUntilUpperOffset      ( lastOffsetToClear   );
	m_LogInfoPage.WriteDeviceState                ( stateBeforeFormat );
	FlushLogInfoPage (taskid);
	m_LogInfoPage.LeaveRegion(taskid);

    if (doFormatLog)
    {
        FormatLog(taskid, firstOffsetToClear, lastOffsetToClear);
    }

    m_LogInfoPage.EnterRegion(taskid);
    // PTS 1118120 UH 2002-10-01 begin
    const bool clearCompleteLog = firstOffsetToClear == LogicalLogBeginOffset()
                                  &&
                                  lastOffsetToClear == LogicalLogEndOffset();

    if ( clearCompleteLog )
    {
        // complete log is cleared => reset iosequence and log backup no
        m_LogInfoPage.WriteLastKnownIOSequenceNo      ( Log_IOSequenceNo()      );
        m_LogInfoPage.WriteLastKnownOffset            ( Log_RawDeviceOffset()   );
        m_LogInfoPage.WriteLogBackupCount             ( 0                       );
        m_LogInfoPage.WriteOldestNotSavedOffset       ( LogicalLogBeginOffset() );
        m_LogInfoPage.WriteOldestNotSavedIOSequenceNo ( Log_IOSequenceNo()      );
        m_LogInfoPage.WriteOldestKnownIOSequenceNo    ( Log_IOSequenceNo()      );
// PTS 1130780 UH 2004-07-27 moved to Restart()
//        k57restartrec->rstLastSavept_kb00().svpIOsequence_kb00  = Log_IOSequenceNo().RawValue(); // invalid-value PTS 1124727 mb 2003-30-10
//        k57restartrec->rstLastSavept_kb00().svpStartOffset_kb00 = Log_RawDeviceOffset().RawValue();
    }
    else
    {
        m_LogInfoPage.WriteLastKnownOffset            ( lastKnownOffset );
        m_LogInfoPage.WriteOldestNotSavedOffset       ( oldestNotSavedOffset );
        m_LogInfoPage.WriteOldestNotSavedIOSequenceNo ( oldestNotSavedIOSeq );
        m_LogInfoPage.WriteOldestKnownIOSequenceNo    ( oldestNotSavedIOSeq );
    }
    // PTS 1118120 UH 2002-10-01 end
    m_LogInfoPage.WriteClearUntilLowerOffset ( Log_RawDeviceOffset());     // set to invalid
    m_LogInfoPage.WriteClearUntilUpperOffset ( Log_RawDeviceOffset());     // set to invalid
    m_LogInfoPage.WriteDeviceState ( stateAfterFormat );
    FlushLogInfoPage (taskid);
    m_LogInfoPage.LeaveRegion(taskid);

    if ( clearCompleteLog ) // PTS 1118120 UH 2002-10-01
        Kernel_IAdminDiagnose::Instance().FlushRestartRecord(taskid);

    if ( m_State.IsReadyForWriting() )
        ResetLogWriter(taskid);
    k38history_lost_write(taskid);
}

// -----------------------------------------------------------------------------
void
Log_Volume::WriteSavepointValuesAndFlushInfoPage
    (TaskID                   taskid,
     const bool               isRestartSavepoint, // PTS 1137938 mb 2005-09-19
     tsp00_Version            kernelVersion,
     const SAPDBFields_Field& rrpart)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::WriteSavepointValuesAndFlushInfoPage", LogVolume_Trace, 5);
 
    m_LogInfoPage.EnterRegion(taskid);
    // PTS 1113190 UH 2001-12-19 do not reset history lost here
    m_LogInfoPage.WriteSavepointParameters (rrpart);
    m_LogInfoPage.WriteDBVersion (kernelVersion);
    if ( m_LogInfoPage.ReadAutoOverwrite() ) // PTS 1120106 UH 2003-01-20
    {
        if ( isRestartSavepoint || Kernel_IAdminInfo::Instance().KernelStateIsOnline())
        // Redosavepoints do not release log (PTS 1137938 mb 2005-09-19
        // Restart should as first savepoint release log and any online-savpoint
        // also
        {
            ReadLogPage(taskid, m_BufferedPage, m_LogInfoPage.ReadSavepointEntryOffset());
            m_LogInfoPage.WriteOldestNotSavedIOSequenceNo(m_BufferedPage.ReadFirstWriterIOSequenceNo());
            m_LogInfoPage.WriteOldestNotSavedOffset(m_LogInfoPage.ReadSavepointEntryOffset());
        }
    }
    FlushLogInfoPage (taskid);
 
    m_LogInfoPage.LeaveRegion(taskid);
    
    // The Log_Writer is in LOG_FULL, because no savepoint had been written
    // => wake him and all waiting user-tasks up
    m_WriterTaskSync.Resume();
    ClearLogFullQueueState(taskid);
    
    #ifdef SAPDB_SLOW
    if ( LogVolume_Trace.TracesLevel(6) )
        m_LogInfoPage.WriteToTrace();
    #endif
}

// -----------------------------------------------------------------------------
void
Log_Volume::WriteDeviceParametersToTrace ()
{
    Kernel_VTrace() << "LogicalLogOffsets: " << LogicalLogBeginOffset()
                    << " -> " << LogicalLogEndOffset();
    Kernel_VTrace() << "FirstKnownSeq: " << m_FirstKnownIOSeq
                    << "@" << m_FirstKnownOffset ;
    m_LogInfoPage.WriteToTrace();
    m_SaveIterator.WriteToTrace();
    
    for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize(); qid++)
    {
        m_LogQueues[qid]->WriteToTrace();
    }
}
// -----------------------------------------------------------------------------
void
Log_Volume::SetToLastSavepointEntry (tsp00_TaskId  taskId,Log_VolumeIterator &iter)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetToLastSavepointEntry", LogVolume_Trace, 5);

          tkb00_SaveptParam   & SaveptParam = k57restartrec->rstLastSavept_kb00();
    const Log_IOSequenceNo      svpSeq = SaveptParam.svpIOsequence_kb00;
     
    if (    RawDeviceOffset(SaveptParam.svpStartOffset_kb00).IsInvalid()                                               // PTS 1129184 mb 2004-04-21
        && (Log_VolumeIterator::LogVolumeIterator == iter.GetSourceType()))
    // for redo-savepoints, no offset is known, but for shadow-instances
    // it may be possible, that the LogVolume has been copied, so that a 
    // restart from a logvolume is possible
    {
        SaveptParam.svpStartOffset_kb00 = SearchSavepointOffset(taskId, svpSeq).RawValue();
    }
            
    if ( svpSeq.IsValid()
         &&
         ((Log_VolumeIterator::TapeIterator == iter.GetSourceType())           // PTS 1129154 mb 2004-04-20
          ||
          (   (Log_VolumeIterator::LogVolumeIterator == iter.GetSourceType())    // PTS 1129154 mb 2004-04-20
           &&  RawDeviceOffset(SaveptParam.svpStartOffset_kb00).IsValid()
           &&  SaveptParam.svpStartOffset_kb00 <= LogicalLogEndOffset().RawValue())))      // PTS 1115091 UH 2002-03-27
    {
        iter.SetPosition ( SaveptParam.svpStartOffset_kb00,
                           SaveptParam.svpStartEntryOffset_kb00 );
    }
    else
    {
        if ( svpSeq.IsValid() ) // PTS 1115870 MB 2002-05-23
        {
            // there has been a savepoint written, but the offset is incorrect
            RTE_Message(Log_Exception(__CONTEXT__,LOG_SAVEPOINT_NOT_FOUND));
            Kernel_IAdminRestartShutdown::Instance().Offline();
        }


        // for an empty log with no savepoint: start with IOSeq ZERO
        iter.SetLastKnownIOSeq(MIN_IOSEQUENCE);
        iter.SetPosition(LogicalLogBeginOffset());
    }
}
// -----------------------------------------------------------------------------
Log_IOSequenceNo Log_Volume::GetSavepointIOSeq()
{
    tkb00_SaveptParam& SaveptParam = k57restartrec->rstLastSavept_kb00();
    return SaveptParam.svpIOsequence_kb00;
}
// -----------------------------------------------------------------------------
void
Log_Volume::SetToBeginOfPage (Log_VolumeIterator    &iter,
                              Log_RawDeviceOffset    offset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetToBeginOfPage ", LogVolume_Trace, 5);
    iter.SetPosition (offset);
}
// -----------------------------------------------------------------------------
Log_VolumeIterator* 
Log_Volume::GetLogVolumeIterator (
                      SAPDBMem_IRawAllocator    &alloc,
                      tsp00_TaskId               taskId,
                      boolean                   *RTECancelFlag,
                      Kernel_Date               &untilDate,         // PTS 1111525 mb 2001-01-15
                      Kernel_Time               &untilTime,         // PTS 1111525 mb 2001-01-15
                      Log_IOSequenceNo          &untilIOSequence,   // PTS 1121539 mb 2003-02-06
                      Rst_StandbySynchronizer   &standbySynchronizer)
    const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetLogVolumeIterator", LogVolume_Trace, 5);
    Log_SeqLogDevReader    *logDeviceReader = new(alloc) 
                                              Log_SeqLogDevReader(  LogicalLogBeginOffset(),
                                                                    LogicalLogEndOffset(),
                                                                    m_PageReader,
                                                                    standbySynchronizer,
                                                                    alloc);
    logDeviceReader->Initialize();
    return new (alloc) Log_VolumeIterator
                        (taskId,
                         RTECancelFlag,
                         logDeviceReader,
                         Log_IOSequenceNo(),
                         alloc,
                         untilDate,untilTime,untilIOSequence,
                         Log_VolumeIterator::LogVolumeIterator);
}

// -----------------------------------------------------------------------------
Log_VolumeIterator* 
Log_Volume::GetTapeIterator (
                      SAPDBMem_IRawAllocator     &alloc,
                      tsp00_TaskId                taskId,
                      boolean                    *RTECancelFlag,
                      Log_IOSequenceNo            lastKnownOnTape,
                      Kernel_Date                &untilDate,         // PTS 1111525 mb 2001-01-15
                      Kernel_Time                &untilTime,         // PTS 1111525 mb 2001-01-15
                      Log_IOSequenceNo           &untilIOSequence)   // PTS 1121539 mb 2003-02-06
          const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetTapeIterator", LogVolume_Trace, 5);
    tkb00_SaveptParam& SaveptParam = k57restartrec->rstLastSavept_kb00();
    IOSequenceNo lastIOSequence = SaveptParam.svpIOsequence_kb00;
	if (lastIOSequence.IsInvalid())
	// no savepoint known => restore log from the first possible IOSequence
	{
		lastIOSequence = MIN_IOSEQUENCE;
	}
    Log_SeqTapeReader    *tapeReader = new(alloc) 
                                              Log_SeqTapeReader(lastIOSequence);
    return new(alloc) Log_VolumeIterator
                        (taskId,
                         RTECancelFlag,
                         tapeReader,
                         lastKnownOnTape,
                         alloc,
                         untilDate,untilTime,untilIOSequence,
                         Log_VolumeIterator::TapeIterator); 
}

// -----------------------------------------------------------------------------
Log_Volume::DeviceBlockCount 
Log_Volume::GetNumberOfUnsavedPages() const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetNumberOfUnsavedPages", LogVolume_Trace, 5);

    if ( m_State.IsReadyForWriting() )
    // warm operating mode => m_LogWriter is allocated and active
    // this depends on the assumption that it is not possible
    // to change directly from WARM to COLD
    {
        RawDeviceIterator iter ( LogicalLogBeginOffset(),
                                 m_LogWriter.GetWriteOffset(),
                                 LogicalLogEndOffset());
        if ( m_LogInfoPage.ReadOldestNotSavedOffset().IsValid() )
        {
            if (iter.GetPosition() != m_LogInfoPage.ReadOldestNotSavedOffset())
            // the currently written page can never saved in warm mode
            {
                --iter;
                return iter.GetDistanceFrom(m_LogInfoPage.ReadOldestNotSavedOffset());
            }
            else
            // everything saved except the currently written page
            {
                return 0;
            }
        }
        else
        {
            return iter.GetDistanceFrom(LogicalLogBeginOffset());
        }
    }
    else
    // cold operating mode
    {
        if (m_LogInfoPage.ReadLastKnownOffset().IsValid())
        {
            RawDeviceIterator iter ( LogicalLogBeginOffset(),
                                     m_LogInfoPage.ReadLastKnownOffset(),
                                     LogicalLogEndOffset());
    
            if ( m_LogInfoPage.ReadOldestNotSavedOffset().IsValid() )                             
                return iter.GetDistanceFrom(m_LogInfoPage.ReadOldestNotSavedOffset());
            else
                return iter.GetDistanceFrom(LogicalLogBeginOffset());
        }
        else
        {
            return 0;
        }
    }
}

// -----------------------------------------------------------------------------
const Log_Volume::InitLogSaveResult 
Log_Volume::InitLogSaveIterator(const tsp00_TaskId            taskId,
                                      SAPDBMem_IRawAllocator &allocator,
                                const Log_Volume::SaveRange   rangeToBeSaved, // PTS 1128703       
                                const SAPDB_Bool              repeatable,                      
                                const Log_DeviceBlockCount    blockSize)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::InitLogSaveIterator", LogVolume_Trace, 5);

    const bool isOnline = Kernel_IAdminInfo::Instance().KernelStateIsOnline();
    
    if ( m_LogInfoPage.ReadDeviceState() != Log_DeviceStateOkay ) 
    {
        RTE_Message( Log_Exception(__CONTEXT__, LOG_HISTORY_LOST_NO_LOGSAVE_ALLOWED,
                                   Log_DeviceStateStrings[m_LogInfoPage.ReadDeviceState()]));
        return HistoryIsLost;
    }

    // determine first and last page to save
    Log_RawDeviceOffset startPoint, endPoint;
    Log_RawDeviceIterator startIter, endIter;
    
    if (!m_SaveIterator.IsValid()) // PTS 1128703 mb 2004-05-07
    {
        bool firstSaveOfLog;

        // determine first and last page to save
        startPoint = m_LogInfoPage.ReadOldestNotSavedOffset();

        if (startPoint.IsInvalid() || m_LogInfoPage.ReadOldestNotSavedIOSequenceNo().IsInvalid())
        {
            startPoint = LogicalLogBeginOffset();
            firstSaveOfLog = true;
        }
        else
        {
            firstSaveOfLog = false;
        }
        
        // PTS 1123761 mb 2003-09-03 online flush all last pages in the queues for complete save-log
        if (isOnline)
        {
            Log_RawDeviceOffset writerPosition = m_LogWriter.GetWriteOffset();

            if (Log_DeviceMinimizeSpace == m_FlushMode)
            // PTS 1133966 mb 2005-02-16 compiler-bug HPIA64
            {
                for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize() ; qid++) // PTS 1135304
                {
                    m_LogQueues[qid]->ForceNewPage(taskId, endPoint);
                }
            }

            // check if writerposition points to the only page to be saved
            if (endPoint.IsInvalid())
            {
                endPoint = writerPosition;
                endIter = Log_RawDeviceIterator( LogicalLogBeginOffset(),
                                                 endPoint,
                                                 LogicalLogEndOffset());
            }
            else
            {
                endIter = Log_RawDeviceIterator( LogicalLogBeginOffset(),
                                                 endPoint,
                                                 LogicalLogEndOffset());
                ++endIter;
            }
            
            
            if (endIter.GetPosition() == startPoint)
            {
                return NoLogToSave;
            }
            
            // decrement enditer because this page is in use by the writer
            --endIter;
            endPoint = endIter.GetPosition();
        }
        else
        // database is in admin-mode
        {
            endPoint=m_LogInfoPage.ReadLastKnownOffset();
            if ( endPoint.IsInvalid() )
            // exit if the log has no endpoint for save
            {
                SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "Log_Volume::GetSaveLogIter: No End-Offset found");
                return NoLogToSave;
            }
            endIter = Log_RawDeviceIterator( LogicalLogBeginOffset(),
                                             endPoint,
                                             LogicalLogEndOffset());
        }
        
        // check if something has to be saved
        startIter = Log_RawDeviceIterator( LogicalLogBeginOffset(),
                                           startPoint,
                                           LogicalLogEndOffset());
        // if startPoint denotes the page after endpoint, then nothing has to be saved
        --startIter;    // startIter is reset later
        if (startIter.GetPosition() == endPoint)
        {
            if (!firstSaveOfLog)
            // when the log is saved first it may occur that every page is written
            // so startPoint denotes the first page on the log end endpoint the 
            // last page on the log
            {
                return NoLogToSave;
            }
        }
        ++startIter;   
    }
    else
    // SaveIterator is repeatable and valid 
    {
        m_SaveIterator.GetNextSegmentRange(startPoint, endPoint);
        endIter = Log_RawDeviceIterator( LogicalLogBeginOffset(),
                                         endPoint,
                                         LogicalLogEndOffset());
        startIter = Log_RawDeviceIterator( LogicalLogBeginOffset(),
                                         startPoint,
                                         LogicalLogEndOffset());
    }
            
    Log_DeviceBlockCount max_pages_to_save;
    if (AllAtOnce == rangeToBeSaved)                            // PTS1111985 mb 2001-11-16
    // save as much as possible (i.e. for save into pipes)
    {
        max_pages_to_save = LogicalLogEndOffset().RawValue()-LogicalLogBeginOffset().RawValue()+1;
    }
    else
    // save maximal plogSegmentSize_gg00 pages in one run
    {
        max_pages_to_save = GetAutosaveDistance();  // PTS 1113361 mb 2002-01-11
    }
    if (max_pages_to_save == 0)
    {
          RTE_Crash(Log_Exception(__CONTEXT__,LOG_SAVEITER_INV_SEG_SIZE));
    }

    bool isLastSegmentToSave;                     // PTS1111987 mb 2001-10-12
    if (startIter.GetDistanceTo(endIter.GetPosition())+1 > max_pages_to_save)
    {
        isLastSegmentToSave = false;
        endIter = startIter + (max_pages_to_save - 1); // +++ added () UH 2001-06-27
    }
    else
    {
        // For autosave only complete segments are allowed
        // so exit if not enough has to be saved
        if ( OnlyCompleteLogSegment == rangeToBeSaved)
        {
            m_SaveIterator.Invalidate();
            return IncompleteSegment;
        }
        
        // now we have the case AllIncompleteSegmentsAllowed == rangeToBeSaved
        isLastSegmentToSave = true;
    }

    if (!m_SaveIterator.Initialize(  taskId,
                                     GetNextBackupCount(taskId),
                                     startPoint,
                                     endPoint,
                                     startIter,
                                     endIter,
                                     blockSize,
                                     isLastSegmentToSave,
                                     repeatable))
    {
        RTE_Message( Log_Exception(__CONTEXT__,
                      SAPDBERR_ASSERT_STATE_FAILED,
                      "GetSaveLogIter:m_SaveIterator.Initialize()"));
        return InitializeFailed;
    }

    SAPDBERR_ASSERT_STATE(m_SaveIterator.IsValid());
    SAPDBTRACE_WRITELN (LogVolume_Trace, 6,"Log_Volume::GetSaveLogIter save from page " << m_SaveIterator.GetStartPageOffset() << " to " << m_SaveIterator.GetEndPageOffset());
    
    return Okay;

}
// -----------------------------------------------------------------------------

SAPDB_UInt4 Log_Volume::GetNextBackupCount ( tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetNextBackupCount", LogVolume_Trace, 5);

    m_LogInfoPage.EnterRegion(taskid);

    SAPDB_UInt backupcount = m_LogInfoPage.ReadLogBackupCount();
    backupcount++;
    m_LogInfoPage.WriteLogBackupCount(backupcount);
    FlushLogInfoPage(taskid);

    m_LogInfoPage.LeaveRegion(taskid);

    return backupcount;
}
// -----------------------------------------------------------------------------
Log_Volume::FreeLogResult Log_Volume::FreeLog (const tsp00_TaskId        taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::FreeLog", LogVolume_Trace, 5);
    
    if (m_SaveIterator.IsInvalid())
    {
        RTE_Message( Log_Exception(__CONTEXT__,LOG_FREE_FAILED_PROT ));
        return NoSaveLogExecutedBefore;
    }
    
    if (m_SaveIterator.IsRepeatable())
    {
        SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "Log_Volume::FreeLog nothing freed:m_SaveIterator.IsRepeatable");
        return FreeLogOk;
    }

    // check IOSequences
    Log_IOSequenceNo infoPageOldestNotSavedIOSeq = m_LogInfoPage.ReadOldestNotSavedIOSequenceNo();
    if (infoPageOldestNotSavedIOSeq.IsValid())
    {
        if (infoPageOldestNotSavedIOSeq != m_SaveIterator.GetStartIOSequence())
        {
            RTE_Message( Log_Exception(__CONTEXT__, LOG_FREE_FAILED_IOSEQ,
                   SAPDB_ToString(m_SaveIterator.GetStartIOSequence(),_T_d),
                   SAPDB_ToString(m_SaveIterator.GetEndIOSequence(),_T_d),
                   SAPDB_ToString(infoPageOldestNotSavedIOSeq,_T_d) ));
            return IOSequenceDoesNotMatch;
        }
    }
    
    m_LogInfoPage.EnterRegion(taskid);
    
    // Set oldestNotSaved-values
    Log_RawDeviceIterator newOldestNotSavedOffset (LogicalLogBeginOffset(), m_SaveIterator.GetEndPageOffset(), LogicalLogEndOffset());
    ++newOldestNotSavedOffset;
    m_LogInfoPage.WriteOldestNotSavedIOSequenceNo(m_SaveIterator.GetEndIOSequence()+1);
    m_LogInfoPage.WriteOldestNotSavedOffset      (newOldestNotSavedOffset.GetPosition());
    
    FlushLogInfoPage (taskid);
    
    m_LogInfoPage.LeaveRegion(taskid);
    
    if ( m_State.IsReadyForWriting() )
    {
        ClearLogFullQueueState(taskid);
    }
    if ( m_State.IsReadyForWriting() )
    // Resume the WriterTask only if really neccessary 
    // (i.e. everything is running and not for save-cold)
    {
        m_WriterTaskSync.Resume();
    }

    // PTS 1114791 mb 2002-08-28
    m_SaveIterator.InvalidateNoLongerRequired();

    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "Log_Volume::FreeLog succeeded : last saved IOSeq =" << m_SaveIterator.GetEndIOSequence()
                                            << "at offset " << m_SaveIterator.GetEndPageOffset());
    
    return FreeLogOk;
}
// -----------------------------------------------------------------------------
Log_Volume::FreeLogResult Log_Volume::FreeLogForPipe(
                const tsp00_TaskId         taskid,
                const Log_IOSequenceNo     startIOSeq,
                const Log_IOSequenceNo     endIOSeq)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::FreeLogForPipe", LogVolume_Trace, 5);

    if (m_SaveIterator.IsInvalid())
    {
        RTE_Message( Log_Exception(__CONTEXT__,LOG_FREE_FAILED_PROT ));
        return NoSaveLogExecutedBefore;
    }

    if (   m_SaveIterator.GetStartIOSequence() != startIOSeq
        || m_SaveIterator.GetEndIOSequence() != endIOSeq)
    {
        RTE_Message( Log_Exception(__CONTEXT__, LOG_FREEPIPE_FAILED_IOSEQ,
               SAPDB_ToString(m_SaveIterator.GetStartIOSequence(),_T_d),
               SAPDB_ToString(startIOSeq,_T_d),
               SAPDB_ToString(m_SaveIterator.GetEndIOSequence(),_T_d),
               SAPDB_ToString(endIOSeq,_T_d) ));
        return IOSequenceDoesNotMatch;
    }
    else
    {
        if ( ! PrepareLogInfoPageForRead(taskid) )
            return LogVolumeNotAccessible;
        
        return FreeLog(taskid);
    }
}     
// -----------------------------------------------------------------------------
void Log_Volume::ChangeEndOffsetAfterAddDevSpace
        (tsp00_TaskId        writertaskid,
         Log_RawDeviceOffset newEndOffset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ChangeEndOffsetAfterAddDevSpace", LogVolume_Trace, 5);
    
    m_RawDeviceEnd = newEndOffset;

    m_LogInfoPage.EnterRegion(writertaskid);

    m_LogInfoPage.WriteCycleRelatedEndOffset(newEndOffset);

    FlushLogInfoPage (writertaskid);

    m_LogInfoPage.LeaveRegion(writertaskid);
}
// -----------------------------------------------------------------------------
void Log_Volume::SetNewDeviceEndOffset ( SAPDB_UInt4 newEndOffset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetNewDeviceEndOffset", LogVolume_Trace, 5);
    m_LogWriter.SetDeviceEnd(newEndOffset);
}
// -----------------------------------------------------------------------------
void Log_Volume::FlushLogInfoPage (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::FlushLogInfoPage", LogVolume_Trace, 5);
 
    SAPDBERR_ASSERT_STATE(m_LogInfoPage.InRegion(taskid));
    SAPDBERR_ASSERT_STATE(m_State.IsLogInfoPageReadable());

    Kernel_DateTime dateTime;
    dateTime.DetermineTime();
 
    m_LogInfoPage.WriteLogDate(dateTime.GetDate());
    m_LogInfoPage.WriteLogTime(dateTime.GetTime());

    const bool openLogAreaTemporary = ! m_State.IsLogAreaAccessible();
    if ( openLogAreaTemporary )
        OpenLogArea(taskid);
        
    if (!Kernel_IAdminHotStandby::Instance().RoleIsStandby())
    {
        m_PageWriter.WriteLogInfoPage ( taskid, m_LogInfoPage, Log_ClusterAddress(LogInfoPageOffset(),1) );
    }
 
    if ( openLogAreaTemporary )
        CloseLogArea(taskid);

    #ifdef SAPDB_SLOW
    if ( LogVolume_Trace.TracesLevel(6) )
        m_LogInfoPage.WriteToTrace();
    #endif
}
// -----------------------------------------------------------------------------
void Log_Volume::UpdateInfoPageFromWriter (tsp00_TaskId        writertaskid,
                                               Log_IOSequenceNo    lastKnownSequence,
                                               Log_RawDeviceOffset lastKnownOffset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::UpdateInfoPageFromWriter", LogVolume_Trace, 5);

    m_LogInfoPage.EnterRegion(writertaskid);
    if (    m_LogInfoPage.ReadDeviceState() == Log_DeviceStateCleared
         || m_LogInfoPage.ReadDeviceState() == Log_DeviceStatePartiallyCleared )   // PTS 1111525 mb 2002-01-29
    {
        m_LogInfoPage.WriteDeviceState ( Log_DeviceStateHistoryLost );
        m_LogInfoPage.WriteDBIdentifier(k57restartrec->rstDbIdent_kb00());  //PTS1113550 mb 2002-05-08
    }
    
    m_LogInfoPage.WriteLastKnownIOSequenceNo(lastKnownSequence);
    m_LogInfoPage.WriteLastKnownOffset      (lastKnownOffset);
    FlushLogInfoPage (writertaskid);

    m_LogInfoPage.LeaveRegion(writertaskid);
}
 
// -----------------------------------------------------------------------------
void Log_Volume::GetDeviceUsage ( SAPDB_UInt &deviceSize,
                                  SAPDB_UInt &pagesUsed,
                                  SAPDB_UInt &pagesNotSaved,
                                  SAPDB_UInt &pendingLogSize )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetDeviceUsage", LogVolume_Trace, 5);

    deviceSize     = 0;
    pagesUsed      = 0;
    pagesNotSaved  = 0;
    pendingLogSize = 0;
    if ( ! m_State.IsReadyForWriting() )
        return;
        
    deviceSize     = GetDeviceSize();         // PTS 1124876 UH 2003-10-23
    pagesUsed      = GetWriter().UsedPages(); // PTS 1124876 UH 2003-10-23
    pendingLogSize = GetWriter().PendingSize();

    RawDeviceIterator currentWriterPosition = GetWriter().GetPosition();
    
    if ( m_LogInfoPage.ReadOldestNotSavedOffset().IsValid() )                             
        pagesNotSaved = currentWriterPosition.GetDistanceFrom(m_LogInfoPage.ReadOldestNotSavedOffset());
    else
        pagesNotSaved = currentWriterPosition.GetDistanceFrom(LogicalLogBeginOffset());
	
    if ( pagesNotSaved > deviceSize ) // PTS 1124876 UH 2003-10-23
        pagesNotSaved = deviceSize;
}

// -----------------------------------------------------------------------------
SAPDB_UInt Log_Volume::QueueSize()
{
    const SAPDB_UTF8 *         paramName    = UTF8("LOG_IO_QUEUE");
    RTEConf_Parameter::Integer size = 0;
    SAPDBErr_MessageList       error;

    if( RTEConf_ParameterAccess::Instance() == 0 )
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"RTEConf_ParameterAccess::Instance() != 0"));

    if( ! RTEConf_ParameterAccess::Instance()->GetInteger(paramName, size, error) )
    {
        RTE_Message (error);
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"GetParameter: LOG_IO_QUEUE"));
    }
    
    if ( size > SAPDB_MAX_UINT4 )
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"size <= SAPDB_MAX_UINT4"));

    return static_cast<SAPDB_UInt>(size);
}



// -----------------------------------------------------------------------------
void Log_Volume::GetQueueUsage ( SAPDB_UInt4    &queueSize,
                                 SAPDB_UInt4    &maxPagesUsedInQueue,
                                 SAPDB_UInt4    &countReserveRequests,
                                 SAPDB_UInt4    &countWaitForIO,
                                 SAPDB_UInt4    &countGroupCommit,
                                 SAPDB_UInt4    &countWaitForQueueFree,
                                 SAPDB_UInt4    &maxWaitForPageIO )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetQueueUsage", LogVolume_Trace, 5);

    queueSize             = 0;
    maxPagesUsedInQueue   = 0;
    countReserveRequests  = 0;
    countWaitForIO        = 0;
    countGroupCommit      = 0;
    countWaitForQueueFree = 0;
    maxWaitForPageIO      = 0;

    if ( ! m_State.IsReadyForWriting() )
        return;

    // this value is equal for all queues
    queueSize = m_LogQueues[0]->GetSize();

    for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize(); qid++)
    {
        Log_Queue & currentQueue = *m_LogQueues[qid];
    
        if (maxPagesUsedInQueue < currentQueue.GetMaxPagesUsed(Log_Queue::MonitorLog_OBSOLETE))
        {
            maxPagesUsedInQueue = currentQueue.GetMaxPagesUsed(Log_Queue::MonitorLog_OBSOLETE);
        }
        countReserveRequests  += currentQueue.GetReserveCount(Log_Queue::MonitorLog_OBSOLETE);
        countWaitForIO        += currentQueue.GetWaitCount(Log_Queue::MonitorLog_OBSOLETE);
        countGroupCommit      += currentQueue.GetGroupCommitCount(Log_Queue::MonitorLog_OBSOLETE);
        countWaitForQueueFree += currentQueue.GetQueueFullSuspendCount(Log_Queue::MonitorLog_OBSOLETE);
    
        if (maxWaitForPageIO  < currentQueue.GetMaxCountOfWaitingTasksPerPage(Log_Queue::MonitorLog_OBSOLETE))
        {
            maxWaitForPageIO = currentQueue.GetMaxCountOfWaitingTasksPerPage(Log_Queue::MonitorLog_OBSOLETE);
        }
    } 
}

// -----------------------------------------------------------------------------
void Log_Volume::ResetStatistics (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ResetStatistics", LogVolume_Trace, 5);

    for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize(); qid++)
    {
       m_LogQueues[qid]->ResetStatistics (taskid, Log_Queue::ResetMonitorLog_OBSOLETE);
    } 
}

// -----------------------------------------------------------------------------
void Log_Volume::GetRestartVersion ( tsp00_TaskId      taskid,
                                     Log_IOSequenceNo &firstiosequence,
                                     SAPDB_Char       *dbident,
                                     SAPDB_UInt        dbidentSize,
                                     RTE_Nodename     &masterNodeName )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetRestartVersion", LogVolume_Trace, 5);

    RawDeviceOffset firstKnownOffset;
    
    GetFirstKnownBeforeRestoreLog (firstKnownOffset, firstiosequence);

    SAPDB_MemCopyNoCheck (dbident, m_LogInfoPage.ReadDBIdentifier(), dbidentSize);
    if ( ! GetMasterNodeName (masterNodeName) )
        strcpy (masterNodeName, "*error*");
}

// -----------------------------------------------------------------------------
void  Log_Volume::GetLogDBIdent ( SAPDB_Char       *dbident )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetLogDBIdent", LogVolume_Trace, 5);

    SAPDB_MemCopyNoCheck (dbident, m_LogInfoPage.ReadDBIdentifier(), sizeof(tsp00_Line));
}

// -----------------------------------------------------------------------------
bool Log_Volume::GetMasterNodeName ( RTE_Nodename &masterNodename )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetMasterNodeName", LogVolume_Trace, 5);

    return m_LogInfoPage.ReadMasterNodeName(masterNodename);
}

// -----------------------------------------------------------------------------
const Log_DeviceState Log_Volume::GetDeviceState () const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetDeviceState", LogVolume_Trace, 5);

    return m_LogInfoPage.ReadDeviceState();
}

// -----------------------------------------------------------------------------
const Log_FlushMode Log_Volume::GetFlushMode() const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetFlushMode", LogVolume_Trace, 5);

    return m_FlushMode;
}

// -----------------------------------------------------------------------------
bool Log_Volume::GetLogInfo ( tsp00_TaskId              taskid,
                                  Kernel_Date              &date,
                                  Kernel_Time              &time,
                                  Log_FlushMode            &flushmode,
                                  Log_DeviceState          &devicestate,
                                  SAPDB_UInt2              &queuecount,
                                  Log_RawDeviceOffset      &cycleendoffset,
                                  Log_RawDeviceOffset      &oldestnotsavedoffset,
                                  Log_IOSequenceNo         &oldestnotsavedsequence,
                                  Log_RawDeviceOffset      &lastknownoffset,
                                  Log_IOSequenceNo         &lastknownsequence,
                                  SAPDB_UInt4              &backupcount,
                                  Kernel_Date              &untildate,
                                  Kernel_Time              &untiltime,
                                  Log_RawDeviceOffset      &untilclearloweroffset,
                                  Log_RawDeviceOffset      &untilclearupperoffset,
                                  Log_IOSequenceNo         &untilsequenceafterredo,
                                  Log_IOSequenceNo         &savepointsequence,
                                  Log_RawDeviceOffset      &savepointdevoffset,
                                  Data_PageFrameOffsetType &savepointoffset )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetLogInfo", LogVolume_Trace, 5);

    if ( ! m_State.IsLogInfoPageReadable() )
        return false;
        
    m_LogInfoPage.EnterRegion(taskid);

    date                    = m_LogInfoPage.ReadLogDate();
    time                    = m_LogInfoPage.ReadLogTime();
    flushmode               = m_LogInfoPage.ReadFlushMode();
    devicestate             = m_LogInfoPage.ReadDeviceState();
    queuecount              = m_LogInfoPage.ReadQueueCount();
    cycleendoffset          = m_LogInfoPage.ReadCycleRelatedEndOffset();
    oldestnotsavedoffset    = m_LogInfoPage.ReadOldestNotSavedOffset();
    oldestnotsavedsequence  = m_LogInfoPage.ReadOldestNotSavedIOSequenceNo();
    lastknownoffset         = m_LogInfoPage.ReadLastKnownOffset();
    lastknownsequence       = m_LogInfoPage.ReadLastKnownIOSequenceNo();
    backupcount             = m_LogInfoPage.ReadLogBackupCount();
    untildate               = m_LogInfoPage.ReadRedoUntilDate();
    untiltime               = m_LogInfoPage.ReadRedoUntilTime();
    untilclearloweroffset   = m_LogInfoPage.ReadClearUntilLowerOffset();
    untilclearupperoffset   = m_LogInfoPage.ReadClearUntilUpperOffset();
    untilsequenceafterredo  = m_LogInfoPage.ReadIOSequenceNoValidAfterRedoUntil();
    savepointsequence       = m_LogInfoPage.ReadSavepointIOSequenceNo();
    savepointdevoffset      = m_LogInfoPage.ReadSavepointEntryOffset();
    savepointoffset         = m_LogInfoPage.ReadSavepointEntryOffsetInPage();

    m_LogInfoPage.LeaveRegion(taskid);
    
    return true;
}
 
// -----------------------------------------------------------------------------
void Log_Volume::ClearLogFullQueueState(TaskID      taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ClearLogFullQueueState", LogVolume_Trace, 5);

    if ( ! m_LogWriter.LogIsFull() )
    {
        for (SAPDB_UInt qid = 0; qid < m_LogQueues.GetSize(); qid++)
        {
            m_LogQueues[qid]->SetLogFullState(taskid,false);
        } 
        Kernel_ExtendedState::UpdateLogvolumeFullState(0);
    }
}

// -----------------------------------------------------------------------------
bool  Log_Volume::OpenLogArea ( TaskID taskid,
                                bool   forWrite )
{
    // PTS 1113190 UH 2001-12-19 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::OpenLogArea", LogVolume_Trace, 5);

    if ( m_State.IsLogAreaAccessible() )
        return true;
    if ( IOMan_ILogManager::GetInstance().OpenAllLogVolumes(taskid, forWrite) )
    {
        m_State.SetLogAreaAccessible();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
void  Log_Volume::CloseLogArea (TaskID taskid)
{
    // PTS 1113190 UH 2001-12-19 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CloseLogArea", LogVolume_Trace, 5);

    if ( ! m_State.IsLogAreaAccessible() )
        return;
    m_State.SetLogAreaNotAccessible();
    IOMan_ILogManager::GetInstance().CloseAllLogVolumes(taskid);
}

// -----------------------------------------------------------------------------
bool  Log_Volume::PrepareLogInfoPageForRead (TaskID taskid)
{
    // PTS 1113190 UH 2001-12-19 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::PrepareLogInfoPageForRead", LogVolume_Trace, 5);

    if ( m_State.IsLogInfoPageReadable() )
        return true;
        
    if ( ! OpenLogArea(taskid) )
        return false;

    const SAPDB_Int4 LogCapacity = IOMan_ILogInfo::GetInstance().TotalUsableLogPages();

    m_RawDeviceBegin = LOG_BEGIN_OFFSET;
    m_RawDeviceEnd   = LogCapacity-1;

    if ( ! AllocateAndReadInfoPage(taskid, true) )
    {
        Delete();
        CloseLogArea(taskid);
        return false;
    }
    CloseLogArea(taskid);
    return true;
}

// -----------------------------------------------------------------------------
   
void Log_Volume::SetLogDBIdent ( tsp00_TaskId    taskid, // PTS 1113550 mb 2002-04-30 new
                                 tsp00_Line      dbIdent)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetLogDBIdent", LogVolume_Trace, 5);

    m_LogInfoPage.EnterRegion(taskid);
    m_LogInfoPage.WriteDBIdentifier (dbIdent);
    FlushLogInfoPage (taskid);
    m_LogInfoPage.LeaveRegion(taskid);
}

// -----------------------------------------------------------------------------
   
void Log_Volume::SetMasterNodeName ( tsp00_TaskId        taskid,
                                     const RTE_Nodename &masterNodename )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetMasterNodeName", LogVolume_Trace, 5);

    m_LogInfoPage.EnterRegion(taskid);
    if ( ! m_LogInfoPage.WriteMasterNodeName (masterNodename) )
        RTE_Crash(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                     "m_LogInfoPage.WriteMasterNodeName"));
    FlushLogInfoPage (taskid);
    m_LogInfoPage.LeaveRegion(taskid);
}

// -----------------------------------------------------------------------------
bool Log_Volume::SetVolumeState (tsp00_TaskId    taskid,
                                 Log_DeviceState state)
{
    // PTS 1113190 UH 2001-12-19 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetVolumeState", LogVolume_Trace, 5);

    if ( ! PrepareLogInfoPageForRead(taskid) )
        return false;
        
    if ( Log_DeviceStateOkay == state )
    {
        if ( ! m_LogInfoPage.ReadDevspaceEnabled() )
            return true;
        if ( m_LogInfoPage.ReadAutoOverwrite() )// PTS 1120106 UH 2003-01-20
            state = Log_DeviceStateHistoryLost;
    }

    m_LogInfoPage.EnterRegion(taskid);
    
    m_LogInfoPage.WriteDeviceState ( state );
    
    FlushLogInfoPage (taskid);
    
    m_LogInfoPage.LeaveRegion(taskid);
    
    if ( state != Log_DeviceStateOkay )
        k38history_lost_write(taskid);

    return true;
}

// -----------------------------------------------------------------------------
bool Log_Volume::SetLogWriting (tsp00_TaskId taskid,
                                bool         enabled)
{
    // PTS 1113190 UH 2001-12-19 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetLogWriting", LogVolume_Trace, 5);
    if ( ! PrepareLogInfoPageForRead(taskid) )
        return false;
        
    m_LogInfoPage.EnterRegion(taskid);
    m_LogInfoPage.WriteDevspaceEnabled ( enabled );
    FlushLogInfoPage (taskid);
    m_LogInfoPage.LeaveRegion(taskid);

    if ( ! enabled )
        SetVolumeState (taskid, Log_DeviceStateHistoryLost);
    return true;
}

// -----------------------------------------------------------------------------
SAPDB_UInt2 Log_Volume::GetQueueCountFromInfoPage (tsp00_TaskId taskid)
{
    // PTS 1130336 mb 2004-07-08 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetQueueCountFromInfoPage", LogVolume_Trace, 5);

    if ( ! PrepareLogInfoPageForRead(taskid) )
        return false;
    return m_LogInfoPage.ReadQueueCount();
}
// -----------------------------------------------------------------------------
SAPDB_UInt2 Log_Volume::GetNumQueuesAllocated ()
{
    // PTS 1130336 mb 2004-07-08 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetNumQueuesAllocated", LogVolume_Trace, 5);

    return m_LogQueues.GetSize();
}
// -----------------------------------------------------------------------------
bool Log_Volume::IsDevspaceEnabled (tsp00_TaskId taskid)
{
    // PTS 1113190 UH 2001-12-19 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::IsDevspaceEnabled", LogVolume_Trace, 5);

    if ( ! PrepareLogInfoPageForRead(taskid) )
        return false;

    return m_LogInfoPage.ReadDevspaceEnabled();
}

// -----------------------------------------------------------------------------
bool Log_Volume::SetAutomaticOverwrite (tsp00_TaskId  taskid,
                                        bool         enabled)
{
    // PTS 1120106 UH 2003-01-20 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SetAutomaticOverwrite", LogVolume_Trace, 5);

    if ( ! PrepareLogInfoPageForRead(taskid) )
        return false;
        
    m_LogInfoPage.EnterRegion(taskid);

    m_LogInfoPage.WriteAutoOverwrite(enabled);

    FlushLogInfoPage (taskid);

    m_LogInfoPage.LeaveRegion(taskid);

    if ( enabled )
        SetVolumeState (taskid, Log_DeviceStateHistoryLost);
    
    return true;
}

// -----------------------------------------------------------------------------
bool Log_Volume::IsAutomaticOverwrite (tsp00_TaskId taskid)

{
    // PTS 1120106 UH 2003-01-20 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::IsAutomaticOverwrite", LogVolume_Trace, 5);
    if ( ! PrepareLogInfoPageForRead(taskid) )
        return false;
    return m_LogInfoPage.ReadAutoOverwrite();
}

// -----------------------------------------------------------------------------
void Log_Volume::UpdateIOSequenceAndOffset (           TaskID                taskid, //PTS 1113550 mb 2002-05-15
                                                       IOSequenceNo          lastProcessedIOSequence,
                                                 const Log_RawDeviceIterator nextWritePosition)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::UpdateIOSequenceNoAfterRedoLog", LogVolume_Trace, 5);

    if (LogIsEmpty())
    {
        if (lastProcessedIOSequence.IsValid())
        {
            m_LogInfoPage.EnterRegion(taskid);
            m_LogInfoPage.WriteOldestNotSavedIOSequenceNo(lastProcessedIOSequence);
            m_LogInfoPage.WriteLastKnownIOSequenceNo(lastProcessedIOSequence);
            FlushLogInfoPage (taskid);
            m_LogInfoPage.LeaveRegion(taskid);
        }
    }
    
    // PTS 1117079 UH 2002-08-06
    // lastProcessedIOSequence is only invalid between InitConfig and ActivateServerDb.
    // But then the first logpage should not be written with an invalid ioseq.
    if ( lastProcessedIOSequence.IsValid() )
    {
        m_LogWriter.SetWritePositionAndIOSeq(lastProcessedIOSequence, nextWritePosition);
    }
}
// -----------------------------------------------------------------------------
void Log_Volume::CheckForLogSaveAndSavepoint (Trans_Context &context)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CheckForLogSaveAndSavepoint", LogVolume_Trace, 5);
    m_LogWriter.CheckForLogSaveAndSavepoint(context);
}

// -----------------------------------------------------------------------------
bool Log_Volume::SuspendAndGetLastWrittenIOSequence ( TaskID            taskid,
                                                          Log_IOSequenceNo &iosequence )
{
    // PTS 1114053 UH 2002-10-22 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::SuspendAndGetLastWrittenIOSequence", LogVolume_Trace, 5);
    return m_LogWriter.SuspendAndGetLastWrittenIOSequence(taskid, iosequence);
}

// -----------------------------------------------------------------------------
void Log_Volume::ResumeByUser()
{
    // PTS 1114053 UH 2002-10-22 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ResumeByUser", LogVolume_Trace, 5);
    m_LogWriter.ResumeByUser();
}

// -----------------------------------------------------------------------------

void Log_Volume::ReadLogInfoPageFromLogVolume ( TaskID taskid )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ReadLogInfoPageFromLogVolume", LogVolume_Trace, 5);

    m_LogInfoPage.EnterRegion(taskid);
    m_PageReader.ReadLogInfoPage ( taskid, m_LogInfoPage,
                                   Log_ClusterAddress(LogInfoPageOffset(),1) );
    m_State.SetLogInfoPageReadable();
    m_LogInfoPage.LeaveRegion(taskid);
}

// -----------------------------------------------------------------------------

Log_RawDeviceIterator Log_Volume::GetWriterPosition() const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetWriterPosition", LogVolume_Trace, 5);
    SAPDBTRACE_WRITELN (LogVolume_Trace, 6, "current writer position: "<<m_LogWriter.GetPosition().GetPosition());

    if ( Log_DeviceMinimizeSpace == m_FlushMode )
    {
        Log_RawDeviceIterator currentPosition = m_LogWriter.GetPosition();
        -- currentPosition;
        return currentPosition;
    }
    else
    {
        return m_LogWriter.GetPosition();
    }
}

// -----------------------------------------------------------------------------

bool Log_Volume::AllocateLogPageframe(Log_Page & logPage)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::AllocateLogPageframe", LogVolume_Trace, 5);

    Kernel_Page::PageFrame frame = m_FrameAlloc.New();
    if (!frame.IsAssigned())
        return false;
    logPage.SetFrame(frame);
    return true;    
}

// -----------------------------------------------------------------------------

void Log_Volume::ReleaseLogPageframe(Log_Page & logPage)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ReleaseLogPageframe", LogVolume_Trace, 5);

    Kernel_Page::PageFrame frame = m_BufferedPage.GetFrame();
    m_FrameAlloc.Free(frame);
    m_BufferedPage.ReleaseFrame();
}

// -----------------------------------------------------------------------------

void Log_Volume::ReadLogPage(const tsp00_TaskId          taskid,
                                   Log_Page            & logPage,
                             const Log_RawDeviceOffset   offset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ReadLogPage", LogVolume_Trace, 5);

    const bool openLogAreaTemporary = ! m_State.IsLogAreaAccessible();
    if ( openLogAreaTemporary )
        OpenLogArea(taskid);

    Log_ClusterAddress ioaddress (offset, 1);
    m_PageReader.ReadLogPage( taskid, logPage, ioaddress );

    if ( openLogAreaTemporary )
        CloseLogArea(taskid);
}

// -----------------------------------------------------------------------------

void Log_Volume::WriteLogPage(const tsp00_TaskId          taskid,
                                    Log_Page            & logPage,
                              const Log_RawDeviceOffset   offset)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::WriteLogPage", LogVolume_Trace, 5);

    const bool openLogAreaTemporary = ! m_State.IsLogAreaAccessible();
    if ( openLogAreaTemporary )
        OpenLogArea(taskid);

    Log_ClusterAddress ioaddress (offset, 1);
    m_PageWriter.WriteLogPage( taskid, logPage, ioaddress );

    if ( openLogAreaTemporary )
        CloseLogArea(taskid);
}

// -----------------------------------------------------------------------------

Log_IDiagnose& Log_IDiagnose::Instance()
{
    return Log_Volume::Instance();
}

// -----------------------------------------------------------------------------
void Log_Volume::GetQueueForWriting (tsp00_TaskId  taskid,
                                     Log_Queue*   &logQueue,
                                     const bool    useQueueZero)

{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetQueueForWriting", LogVolume_Trace, 5);

    ReleaseQueue (taskid, logQueue);                           // PTS 11135304 mb 2005-05-24 

    // PTS 11338762 mb 2005-02-17 reassign log_queue 
    if (m_CurrentQueueCount > m_NumQueuesActive) // PTS 1135304 mb 2005-05-20
    {
        SAPDB_UInt4 quid;
        if ( ! useQueueZero )
        {
            SAPDB_Int4  myUKT;
            
            vgetuktid(myUKT);
            quid = myUKT % m_CurrentQueueCount;
            logQueue = m_LogQueues[quid];
        }
        else
        {
            quid = 0;
            logQueue = m_LogQueues[0];
        }
        
        if (m_NumQueuesActive < (quid+1)) // there are more queues active than known up to now
        {
            SAPDB_UInt newQueueCount=0;
            
            m_LogQueues[0]->EnterRegion(taskid);

            if (m_NumQueuesActive < (quid+1))
            {
                if ( 1 == m_NumQueuesActive)
                {
                    m_LogQueues[0]->ChangeFlushMode(Log_DeviceMaximizeSafety);
                    m_FlushMode = Log_DeviceMaximizeSafety;
                }
                else
                {
                    if ( 0 == m_NumQueuesActive)
                    // this is the first operation on the queues
                    {
                        if (0 != quid)
                        {
                            m_LogQueues[0]->ChangeFlushMode(Log_DeviceMaximizeSafety);
                            m_FlushMode = Log_DeviceMaximizeSafety;
                            m_LogInfoPage.WriteLogFlushMode(Log_DeviceMaximizeSafety);
                        }
                        else
                        {
                            m_LogQueues[0]->ChangeFlushMode(Log_DeviceMinimizeSpace);
                            m_FlushMode = Log_DeviceMinimizeSpace;
                            m_LogInfoPage.WriteLogFlushMode(Log_DeviceMinimizeSpace);
                        }
                    }
                }
                m_NumQueuesActive = quid + 1;
                newQueueCount = m_NumQueuesActive;
            }
            
            m_LogQueues[0]->LeaveRegion(taskid);
	    
            if (0 != newQueueCount)
            {
                RTE_Message(
                    Msg_List(
                        Msg_List::Info,
                        SDBMSG_LOG_NUM_ACTIVE_QUEUES_INCREASED,
                        Msg_Arg(SDBMSGTAG_LOG_NUM_ACTIVE_QUEUES_INCREASED__LOG_QUEUE_COUNT,newQueueCount)));
            }
        }
    }
    else
    {
        if (! useQueueZero && (m_CurrentQueueCount > 1))
        {
            SAPDB_Int4  myUKT;

            vgetuktid(myUKT);
            SAPDB_UInt4 quid = myUKT % m_CurrentQueueCount;
            logQueue = m_LogQueues[quid];
        }
        else
        {
            logQueue = m_LogQueues[0];
        }
    }
    logQueue->IncrementCounter(Log_Queue::queueUsage);  // PTS 11135304 mb 2005-05-24 
}

void Log_Volume::ReleaseQueue (tsp00_TaskId  taskid,
                               Log_Queue* &logQueue) // PTS 11135304 mb 2005-05-24
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::ReleaseQueue", LogVolume_Trace, 5);

    if (NULL != logQueue)                              
    {
        logQueue->DecrementCounter(Log_Queue::queueUsage);
        logQueue = NULL;
        if (m_CurrentQueueCount < m_NumQueuesActive)
        {
            if (0 == m_LogQueues[m_NumQueuesActive-1]->GetCounterValue(Log_Queue::queueUsage))
            {
                // this queue can safely removed now
                SAPDB_UInt newQueueCount=0;
                
                m_LogQueues[0]->EnterRegion(taskid);
		
                while (   (0 == m_LogQueues[m_NumQueuesActive-1]->GetCounterValue(Log_Queue::queueUsage))
                       && (m_NumQueuesActive > m_CurrentQueueCount))
                {
                    --m_NumQueuesActive;
                }
                if (1 == m_NumQueuesActive)
                {
                    m_LogQueues[0]->ChangeFlushMode(Log_DeviceMinimizeSpace);
                    m_FlushMode = Log_DeviceMinimizeSpace;
                    m_LogInfoPage.WriteLogFlushMode(Log_DeviceMinimizeSpace);
                }
                newQueueCount = m_NumQueuesActive;

                m_LogQueues[0]->LeaveRegion(taskid);
                
                RTE_Message(
                    Msg_List(
                        Msg_List::Info,
                        SDBMSG_LOG_NUM_ACTIVE_QUEUES_DECREASED,
                        Msg_Arg(SDBMSGTAG_LOG_NUM_ACTIVE_QUEUES_DECREASED__LOG_QUEUE_COUNT,newQueueCount)
                    )
                );                
                
            }
        }
    }
}

// -----------------------------------------------------------------------------
Log_Queue& Log_Volume::GetQueueForStatistic (const Log_QueueID queueId)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::GetQueueForStatistic", LogVolume_Trace, 5);

    return *m_LogQueues[queueId];
}

// -----------------------------------------------------------------------------
//
void Log_Volume::TruncateLogPage (      TaskID               taskid,
                                  const Log_IOSequenceNo     newLastIOSequence,
                                  const Log_RawDeviceOffset  pageOffset,
                                        bool                &isEmpty)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::TruncateLogPage", LogTrans_Trace, 5);
    SAPDBTRACE_WRITELN (LogVolume_Trace, 5, "TruncateLogPage until IOSequence:  " << newLastIOSequence);
    SAPDBTRACE_WRITELN (LogVolume_Trace, 5, "TruncateLogPage at Offset:  " << pageOffset);

    m_PageReader.ReadLogPage (taskid, m_BufferedPage,Log_ClusterAddress(pageOffset,1));
    m_BufferedPage.Truncate(newLastIOSequence);
    isEmpty = m_BufferedPage.IsEmpty();
    m_PageWriter.WriteLogPage(taskid, m_BufferedPage, Log_ClusterAddress(pageOffset, 1));
}

void Log_Volume::TruncateLogPage (      TaskID               taskid,
                          const Log_IOSequenceNo     newLastIOSequence,
                          const Log_RawDeviceOffset  pageOffset)
{
    bool dummy;
    TruncateLogPage(taskid, newLastIOSequence, pageOffset, dummy);
}

// -----------------------------------------------------------------------------

/// During execution of Log_Volume::CheckLogBackup the state for each Log_Queue
/// has to be remembered, because Log_Pages from different Log_Queues may be
/// interleaved
struct LogPageState
{
    /// Amount of bytes which has to b read to complete this entry of the associated Log_Queue
    Log_Page::EntryLength       expectedRestLength; 

    /// CTOR
    LogPageState()
    : expectedRestLength(0) {}
};

SAPDB_Bool Log_Volume::CheckLogBackup(TaskID                  taskId,
                                      pasbool                &rteCancelFlag,
                                      const Log_IOSequenceNo  firstIOSeq,
                                      const Log_IOSequenceNo  lastIOSeq)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Volume::CheckLogBackup", LogTrans_Trace, 5);
    tkb00_PagePtr framePtr = NULL;

    Log_IOSequenceNo wantedIOSeq = firstIOSeq;
    tgg00_BasisError trError=e_ok;
    bool lastPageRead=false;
    Container_Vector<LogPageState> pageState (m_Allocator);
    do 
    {
        if ( LogVolume_Trace.TracesLevel(6) )
            Kernel_VTrace() << "CheckLogBackup: IOSeq =  " << wantedIOSeq << NewLine;
        k39locate_logentries_in_page(   taskId,
                                        rteCancelFlag,
                                        wantedIOSeq.RawValue(),
                                        framePtr,
                                        trError);

        switch (trError)
        {
            case e_ok:
            {
                Log_Page::PageFrame frame(framePtr,FrameCtrl_ICommon::GetInstance().GetPageSize());
                Log_Page currentPage(frame);

                // resize the container if neccessary
                if ((Log_QueueID)pageState.GetSize() < (currentPage.ReadQueueID()+1))
                {
                    pageState.Resize(currentPage.ReadQueueID()+1);
                }
                LogPageState &currentState = pageState[currentPage.ReadQueueID()];

                Log_Page::Iterator entryIter = currentPage.GetIterator();
                currentPage.Begin(entryIter);

                if (currentPage.FirstEntryIncomplete())
                {
                    if (currentState.expectedRestLength != 0)
                    {
                        if (entryIter.RestLength() != currentState.expectedRestLength)
                        {
                            Kernel_VTrace() << "expectedRestLength: " << currentState.expectedRestLength;
                            currentPage.WriteToTrace();
                            RTE_Crash(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                                                         "entryIter.RestLength() != currentState.expectedRestLength"));
                        }
                    }
                }
                
                Log_Page::EntryLength lastRestLength,
                                      lastLength;
                
                // check entries
                while (entryIter.IsValid())
                {
                    lastRestLength = entryIter.RestLength();
                    lastLength = entryIter.Length();
                    ++entryIter;
                }

                if (lastRestLength != lastLength)
                {
                    currentState.expectedRestLength = lastRestLength - lastLength;
                }
                else
                {
                    currentState.expectedRestLength = 0;
                }

                // prepare everything for next loop
                wantedIOSeq = currentPage.ReadLastWriterIOSequenceNo();
                if (wantedIOSeq == lastIOSeq)
                {
                    lastPageRead = true;
                }
                ++wantedIOSeq;
                break;
            }
            case e_end_of_backup:
                return true;
            case e_logpage_not_found:
            default:
            {
                Kernel_VTrace() << "CheckLogBackup: wantedIOSeq " << wantedIOSeq << " not found" << NewLine;
                return false;
            }
        }
    } while ((e_ok == trError) && ( ! lastPageRead));
    return (e_ok == trError);
}

bool Log_Volume::SetQueueCount(SAPDB_UInt queueCount, Msg_List & errlist)
{
    const SAPDB_UTF8 *         paramName    = UTF8("LOCAL_REDO_LOG_BUFFER_SIZE");
    RTEConf_Parameter::Integer localRedoBufferSize = 0;
    SAPDBErr_MessageList       error;

    if ( RTEConf_ParameterAccess::Instance() == 0 )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"no access to RTEConf_ParameterAccess::Instance()");
        return false;
    }

    if ( ! RTEConf_ParameterAccess::Instance()->GetInteger(paramName, localRedoBufferSize, error) )
    {
        errlist = error;
        return false;
    }
    
    if (    (0 == localRedoBufferSize)
         || (1 == queueCount ))
    {
        m_CurrentQueueCount = queueCount;
        return true;
    }
    else
    // A local redologbuffer is not supported for multiple logqueues
    {
        errlist = Msg_List(
            Msg_List::Error,
            SDBMSG_LOG_MULTILOGQUEUE_NOT_ALLOWED,
            Msg_OptArg(SDBMSGTAG_LOG_MULTILOGQUEUE_NOT_ALLOWED__LOG_QUEUE_COUNT,queueCount),
            Msg_OptArg(SDBMSGTAG_LOG_MULTILOGQUEUE_NOT_ALLOWED__LOG_REDOBUFFERSIZE,localRedoBufferSize)
        );
        return false;
    }
}


