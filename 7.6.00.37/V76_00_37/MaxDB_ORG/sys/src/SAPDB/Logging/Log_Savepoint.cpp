/*!**************************************************************************

  module      : Log_Savepoint.cpp
  special area: Logging
  responsible : UweH
  created     : 2000-09-25  12:00
  last changed: 2000-10-27  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : The savepoint manager.

    The phases of the savepoint:
    ----------------------------
    
    - Enter all TransRegions, so redo/undo file handling is locked.
    - Get IO-Sequence of all logwriters
    - Flush all logwriter until this savepoint entry and get the entry id.
    - Create the savepoint file with entries for all open transactions.
    - set the datacache to PrepareSavepoint
    - Leave all TransRegion.
    - Flush datacache and converter
    - Flush the restart record and the log info page.
    - increment the converter version


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


*****************************************************************************/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo52.h"   // vclock
#include "heo55k.h"  // vbegexcl, vendexcl
#include "ggg92.h"   // only for hkb51.h
#include "hgg01.h"   // g01svp_1_conv_flush
#include "hgg08.h"   // g08savepoint
#include "hgg11.h"   // g11kernel_version
#include "hkb38_1.h" // k38e_autosave_end
#include "hkb50.h"   // k50GetLogTransaction, k50RegisterTransaction
#include "hkb51.h"   // k51enter_all_trans_regions, kb51GetFirstWriteTrans
#include "hbd01.h"   // b01downfilesystem
#include "hbd20_4.h" // bd20SavepointCompleted
#include "hbd91.h"   // GarbCollController
#include "gkb00.h"   // tkb00_SaveptParam
#include "hkb57.h"   // k57flush_sequences
#include "hkb57_1.h" // k57restartrec

#include "RunTime/RTE_Message.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Migration.hpp"

#include "Converter/Converter_IBackUp.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_ISavePoint.hpp"

#include "IOManager/IOMan_IDataManager.hpp"

#include "Logging/Log_Types.hpp"
#include "Logging/Log_AfterImage.hpp"
#include "Logging/Log_ActionSavepoint.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_OpenTransFile.hpp"
#include "Logging/Log_History.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_Exceptions.hpp"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"

#include "FreeBlockManagement/FBM_IManager.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "Pager/Pager_ISavepoint.hpp"

#include "ServerTasks/SrvTasks_ICoordinator.hpp"
#include "ServerTasks/SrvTasks_JobSavepoint.hpp"

#include "Restart/Rst_RedoManager.hpp"

#include "SAPDBCommon/Messages/SDBMsg_Log.h"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "SAPDBCommon/Messages/SDBMsg_Savepoint.h"

/*---------------------------------------------------------------------------*/
Log_Savepoint* Log_Savepoint::m_Instance = 0;
/*---------------------------------------------------------------------------*/
Log_Savepoint& Log_Savepoint::Instance()
{
    SAPDBERR_ASSERT_STATE(m_Instance != 0);
    return *m_Instance;
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::CreateInstance (SAPDBMem_IRawAllocator &allocator)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_Savepoint::CreateInstance", LogTrans_Trace, 5);
    if ( m_Instance == 0 )
    {
        SAPDBMEM_STATIC_RAW_ALLOCATE (Space, sizeof(Log_Savepoint));

        m_Instance = new (Space) Log_Savepoint(allocator);

        if ( m_Instance == 0 )
            RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_LOG_ALLOCATION_OF_SAVEPOINT_COORDINATOR_FAILED));
    }
}
/*---------------------------------------------------------------------------*/
#define FOR_INTERNAL_USAGE true
/*---------------------------------------------------------------------------*/
bool Log_Savepoint::ReserveServerTask (RTETask_ITask &task,
                                       Msg_List      &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_Savepoint::CreateInstance", LogTrans_Trace, 5);

    BeginRegion (task);
    
    if ( m_Instance->m_ServerTask.pTask != 0 )
    {
        EndRegion (task);
        return true;
    }

    if ( SrvTasks_ICoordinator::Instance().Reserve ( task,
                                                     m_Instance->m_ServerTask,
                                                     errlist,
                                                     SrvTasks_JobTypesString[jobSavepoint],
                                                     SrvTasks_JobTypesStringShort[jobSavepoint],
                                                     FOR_INTERNAL_USAGE)
         !=
         SrvTasks_ICoordinator::reserveOK )
    {
        SrvTasks_ICoordinator::Instance().AppendContextInfo (__CONTEXT__, errlist, "No Servtask For Savepoint available");
        errlist.Overrule ( Msg_List(Msg_List::Error, SDBMSG_LOG_NO_SERVERTASK_FOR_SAVEPOINT) );
        EndRegion (task);
        return false;
    }

    RTE_Message(Msg_List(Msg_List::Info,SDBMSG_SRVTASKS_SERVERTASKS_PERMANENTLY_RESERVED,
                         Msg_Arg("TASKS_WANTED","1"),
                         Msg_Arg("DESCRIPTION", "Savepoint")));


    if ( m_Instance->m_TaskWaitingUntilServertaskIsAvailable != 0 )
    {
        task.ResumeOther(*(m_Instance->m_TaskWaitingUntilServertaskIsAvailable));
        m_Instance->m_TaskWaitingUntilServertaskIsAvailable = 0;
    }

    EndRegion (task);
    return true;
}
/*---------------------------------------------------------------------------*/
static inline void PrintReason (tsp00_TaskId        taskid,
                                Log_SavepointReason reason,
                                bool                started)
{
    if ( started )
        RTE_Message( Msg_List(Msg_List::Info,SDBMSG_SAVEPOINT_REASON_INFO,
                              Msg_Arg("REASON",Log_SavepointReasonStrings[reason]),
                              Msg_Arg("TASK_ID",SAPDB_ToString(taskid))) );
}

/*---------------------------------------------------------------------------*/
static inline bool SystemIsReadyForSavepoint(Msg_List &errlist)
{
    if ( ! b01downfilesystem
           &&
           Log_Volume::IsReadyForWriting()
           &&
           Log_History::IsInitialized() )
    {
        return true;
    }
    // PTS 1129993 UH 2004-06-16
    errlist = Msg_List(Msg_List::Warning,SDBMSG_SAVEPOINT_NOT_ALLOWED,
                       Msg_Arg("REASON","NOT SystemIsReadyForSavepoint"));
    return false;
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::BeginRegion (RTETask_ITask &task)
{
    vbegexcl ( task.ID(), g08savepoint);
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::EndRegion (RTETask_ITask &task)
{
    vendexcl ( task.ID(), g08savepoint);
}
/*---------------------------------------------------------------------------*/
#define SUSPEND_REASON_WAIT_UNTIL_SERVERTASK_AVAILABLE 245
/*---------------------------------------------------------------------------*/
bool Log_Savepoint::StartCoordinator (RTETask_ITask &task,
                                      Msg_List      &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::StartCoordinator", LogTrans_Trace, 5);
    
    if ( 0 == m_Instance->m_ServerTask.pTask )
    {
        // PTS 1138252 UH 11-10-2005
        BeginRegion(task);
        if ( 0 != m_Instance->m_ServerTask.pTask )
            EndRegion(task);
        else
        {
            if ( m_TaskWaitingUntilServertaskIsAvailable != 0 )
                Kernel_IAdminRestartShutdown::Instance().Offline("m_TaskWaitingUntilServertaskIsAvailable != 0");
            m_TaskWaitingUntilServertaskIsAvailable = &task;
            EndRegion(task);
            task.Suspend(SUSPEND_REASON_WAIT_UNTIL_SERVERTASK_AVAILABLE);
            if ( 0 == m_Instance->m_ServerTask.pTask )
                Kernel_IAdminRestartShutdown::Instance().Offline("Servertask not assigned", true);
        }
    }

    SrvTasks_JobSavepoint job (Converter_ICommon::Instance().Version());

    // PTS 1134588 UH 2005-03-24
    #define WAIT_UNTIL_TASK_IS_FREE true

    switch ( m_ServerTask.pTask->RegisterJob (task, job, errlist, WAIT_UNTIL_TASK_IS_FREE) )
    {
        case SrvTasks_ITask::cannotWaitforRegister :
            // it is assured that a savepoint is running
            // FALLTHROUGH
        case SrvTasks_ITask::registered :
            return true;
        case SrvTasks_ITask::registerError :
            // FALLTHROUGH
        default:
            return false;
    }
}
/*---------------------------------------------------------------------------*/
bool Log_Savepoint::IsDBManagerReason (Log_SavepointReason reason)
{
    return reason == Log_SVPReasonSaveData
           ||
           reason == Log_SVPReasonSavePages
           ||
           reason == Log_SVPReasonStartDropVolume
           ||
           reason == Log_SVPReasonDropVolume
           ||
           reason == Log_SVPReasonRestart
           ||
           reason == Log_SVPReasonShutdown;
}

/*---------------------------------------------------------------------------*/
bool Log_Savepoint::DenySavepoint (RTETask_ITask &task)
{
    // PTS 1121281 UH 2003-03-24 new
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::DenySavepoint", LogTrans_Trace, 5);
    BeginRegion (task);
    if ( m_IsActive )
    {
        EndRegion (task);
        return false;
    }
    m_SavepointAllowed = false;
    EndRegion (task);
    return true;
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::PermitSavepoint (RTETask_ITask &task)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::PermitSavepoint", LogTrans_Trace, 5);
    // PTS 1121281 UH 2003-03-24 new
    BeginRegion (task);
    m_SavepointAllowed = true;
    EndRegion (task);
}
/*---------------------------------------------------------------------------*/
bool Log_Savepoint::IsSavepointAllowed() const
{
    // PTS 1121281 UH 2003-03-24 new
    return m_SavepointAllowed;
}
/*---------------------------------------------------------------------------*/
Log_Savepoint::StartResult
Log_Savepoint::StartSavepointAndWait (RTETask_ITask       &task,
                                      Log_SavepointReason  reason,
                                      Msg_List            &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::StartSavepointAndWait", LogTrans_Trace, 5);

    // clear message list as it is used as an error criterion later on
    errlist.ClearMessageList();

    if ( ! SystemIsReadyForSavepoint(errlist) )
        return systemNotReady;
    
    tgg00_WaitContext waitContext;

    if ( ! Log_Volume::Instance().ReserveForEotWillNotBlock() // PTS 1130928 mb 2004-08-10
         &&
         (Log_SVPReasonSaveData  == reason ||
          Log_SVPReasonSavePages == reason) )
    {
        // PTS 1129993 UH 2004-06-16 added if-clause

        errlist = Msg_List(Msg_List::Warning,SDBMSG_SAVEPOINT_NOT_ALLOWED,
                           Msg_Arg("REASON","LOG FULL"));
        if ( Log_Volume::Instance().GetDeviceState() == Log_DeviceStateHistoryLost )
            errlist.AddDetail(SDBMSG_SAVEPOINT_DENIED_BECAUSE_LOG_FULL_AND_HISTORY_LOST);
        return logIsFull;
    }

    bool startCoordinator = false;
    
    BeginRegion (task);
    
    if ( ! m_SavepointAllowed )
    {
        errlist = Msg_List(Msg_List::Info,SDBMSG_SAVEPOINT_NOT_ALLOWED,
                           Msg_Arg("REASON","temporary not allowed (is logwriting suspended ?)"));
        EndRegion (task);
        return notAllowed;
    }
    
    StartResult result = ok;
    
    if ( ! IsDBManagerReason(reason) )
        CurrentWaitList().InsertAsFirst(task.ID(), waitContext);
    else
    {
        if ( ! m_ExclusiveRequestor.Assign(task.ID(), reason, errlist, result) )
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_SAVEPOINT_SECOND_DBM_REASON,
                               Msg_Arg("OLD_TASK_ID",SAPDB_ToString(m_ExclusiveRequestor.taskid)),
                               Msg_Arg("OLD_REASON", Log_SavepointReasonStrings[m_ExclusiveRequestor.reason]),
                               Msg_Arg("TASK_ID",    SAPDB_ToString(task.ID())),
                               Msg_Arg("REASON",     Log_SavepointReasonStrings[reason]));
            Kernel_VTrace() << "waiting task: " << m_ExclusiveRequestor.taskid
                            << ", old reason: " << Log_SavepointReasonStrings[m_ExclusiveRequestor.reason]
                            << ", new reason: " << Log_SavepointReasonStrings[reason]
                            << ", from task "   << task.ID()
                            << NewLine;
            EndRegion (task);
            return notAllowed;
        }
    }
    if ( ! m_IsActive )
    {
        startCoordinator = true;
        m_IsActive       = true;
    }
    EndRegion (task);

    PrintReason (task.ID(), reason, startCoordinator);

    if ( startCoordinator )
    {
        if ( StartCoordinator (task,errlist) )
        {
            if ( LogTrans_Trace.TracesLevel(6) )                                          
                Kernel_VTrace() << "*** SAVEPOINT STARTED AND WAIT T" << task.ID();
        }
        else
        {
            errlist.AddDetail(SDBMSG_SAVEPOINT_START_FAILED);
            Kernel_VTrace() << "New reason: '" << Log_SavepointReasonStrings[reason] << "'" << FlushLine;
            WriteToTrace ("Log_Savepoint::StartSavepointAndWait: serverStartFailed");
            #ifdef SAPDB_SLOW
            RTE_Message(errlist);
            RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Log_Savepoint::StartSavepointAndWait: serverStartFailed") );
            #endif
            return serverStartFailed;
        }
    }
    
    
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "suspended TaskId: " << task.ID());

    task.Suspend(243);

    if ( result != ok || ! errlist.IsEmpty() )
        Kernel_IAdminRestartShutdown::Instance().Offline(errlist);

    return ok;
}
/*---------------------------------------------------------------------------*/
Log_Savepoint::StartResult
Log_Savepoint::StartSavepoint (RTETask_ITask       &task,
                               Log_SavepointReason  reason,
                               Msg_List            &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::StartSavepoint", LogTrans_Trace, 5);

    if ( ! SystemIsReadyForSavepoint(errlist) )
        return systemNotReady;

    if ( ! Log_Volume::Instance().ReserveForEotWillNotBlock() // PTS 1130928 mb 2004-08-10
         &&
         (Log_SVPReasonSaveData  == reason ||
          Log_SVPReasonSavePages == reason) )
    {
        // PTS 1129993 UH 2004-06-16 added if-clause
        errlist = Msg_List(Msg_List::Warning,SDBMSG_SAVEPOINT_NOT_ALLOWED,
                           Msg_Arg("REASON","LOG FULL"));
        if ( Log_Volume::Instance().GetDeviceState() == Log_DeviceStateHistoryLost )
            errlist.AddDetail(SDBMSG_SAVEPOINT_DENIED_BECAUSE_LOG_FULL_AND_HISTORY_LOST);
        return logIsFull;
    }

    bool startCoordinator = false;
    
    BeginRegion (task);
    if ( ! m_IsActive )
    {
        if ( ! m_SavepointAllowed ) // PTS 1121281 UH 2003-03-24
        {
            errlist = Msg_List(Msg_List::Info,SDBMSG_SAVEPOINT_NOT_ALLOWED,
                               Msg_Arg("REASON","temporary not allowed (is logwriting suspended ?)"));
            EndRegion (task);
            return notAllowed;
        }
        
        if ( reason != Log_SVPReasonUnknown
             ||
             ! CurrentWaitList().IsEmpty()
             || 
             m_ExclusiveRequestor.IsAssigned() )
        {
            startCoordinator = true;
            m_IsActive       = true;
        }
    }
    EndRegion (task);

    PrintReason (task.ID(), reason, startCoordinator);
    
    if ( startCoordinator )
    {
        if ( StartCoordinator (task, errlist) )
        {
            if ( LogTrans_Trace.TracesLevel(6) )                                          
                Kernel_VTrace() << "*** SAVEPOINT STARTED T" << task.ID();
        }
        else
        {
            errlist.AddDetail(SDBMSG_SAVEPOINT_START_FAILED);
            Kernel_VTrace() << "New reason: '" << Log_SavepointReasonStrings[reason] << "'" << FlushLine;
            WriteToTrace ("Log_Savepoint::StartSavepoint: serverStartFailed");
            #ifdef SAPDB_SLOW
            RTE_Message(errlist);
            RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Log_Savepoint::StartSavepoint: serverStartFailed") );
            #endif
            return serverStartFailed;
        }
    }
    return ok;
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::WriteToTrace (char const * const title)
{
    Kernel_VTrace trace;
    if ( title != 0 )
        trace << title << FlushLine;
    trace << "IsActive: " << (m_IsActive?"true":"false") << FlushLine;
    trace << "IsEnabled: " << (m_LogWriteEnabled?"true":"false") << FlushLine;
    trace << "IsAllowed: " << (m_SavepointAllowed?"true":"false") << FlushLine;
    trace << "CurrentWaitList: " << m_CurrentWaitList << FlushLine;
    // Kernel_TaskWaitList    m_Waitlist [2];
    trace << "WaitingDBManager: T" << m_ExclusiveRequestor.taskid << FlushLine;
    trace << "CurrentReason: '" << Log_SavepointReasonStrings[m_ExclusiveRequestor.reason] << "'" << FlushLine;
    // Kernel_TaskWaitList    m_WaitingForPrepare;
    // SrvTasks_RequestedTask m_ServerTask;
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::InternalErrorHandling( ExclusiveRequestor &exlusiveRequestor,
                                           Msg_List           &errlist)
{
    WriteToTrace ("InternalErrorHandling");
    if ( exlusiveRequestor.IsAssigned() )
    {
        exlusiveRequestor.pErrorList->AppendNewMessage(errlist);
        errlist.ClearMessageList();
        vresume (exlusiveRequestor.taskid);
    }
    else
    {
        RTE_Crash(errlist);
    }
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::Coordinator (tgg00_TransContext &Trans)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::Coordinator", LogTrans_Trace, 5);

    Trans_Context &context = Trans_Context::GetContext( Trans );
    RTETask_ITask &task    = context.Task();
    Msg_List      &errlist = context.ErrorList();

    if ( b01downfilesystem )
    {
        BeginRegion (task);
        m_IsActive = false;
        EndRegion (task);

        Trans.trError_gg00 = e_shutdown;

        return;
    }

    ExclusiveRequestor  exclusiveRequestor;
    tkb00_SaveptParam   saveptParam;
    bool                loopAgain;
    teo00_Int4          starttimeSec;      // PTS 1119276 UH 2002-12-10
    teo00_Int4          starttimeMSec;     // PTS 1119276 UH 2002-12-10
    teo00_Int4          endtimeSec;        // PTS 1119276 UH 2002-12-10
    teo00_Int4          endtimeMSec;       // PTS 1119276 UH 2002-12-10
    SAPDB_UInt          numPages;          // PTS 1119276 HH 2002-12-12
    SAPDB_UInt          numPagesTotal = 0; // PTS 1119276 HH 2002-12-12
    SAPDB_UInt          numIO;             // PTS 1119276 HH 2002-12-12
    SAPDB_UInt          numIOTotal    = 0; // PTS 1119276 HH 2002-12-12

    do
    {
        vclock(&starttimeSec,&starttimeMSec); // PTS 1119276 UH 2002-12-10
        loopAgain = false;
        
        BeginRegion (task);
        
        if ( ! m_IsActive )
        {
            errlist.AppendNewMessage(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "Coordinator: active"));
            InternalErrorHandling(m_ExclusiveRequestor, errlist);
            EndRegion (task);
            return;
        }
    
        SwitchWaitList();

        exclusiveRequestor = m_ExclusiveRequestor;
        m_ExclusiveRequestor.Clear();
        
        saveptParam = k57restartrec->rstLastSavept_kb00();
        
        EndRegion (task);

        if ( ! Log_Volume::Instance().ReserveForEotWillNotBlock() // PTS 1130928 mb 2004-08-10
             &&
             Log_SVPReasonShutdown == exclusiveRequestor.reason )
        {
            // PTS 1129993 UH 2004-06-16 added if-clause
            Trans.trError_gg00 = e_log_full;
            ShutdownFilesystem (Trans);
            // never come to this line
        }

        /***************************************************************************/
        /***** now, new requests would result in another savepoint loop       ******/
        /***** now, tasks which want to wait for a savepoint wait for the next *****/
        /***************************************************************************/
        
        saveptParam.svpReason_kb00 = exclusiveRequestor.reason;
        ++saveptParam.svpId_kb00;

        /*********************************************************************/
        /***** Prepare data writer for the first flush of data cache and *****/
        /***** converter storage. Make memory structures persistent.     *****/
        /*********************************************************************/

        // Stop GC first, because it could take a long time until an undo file
        // is removed. In this time the pager will be able to flush a full 
        // data cache, which is impotant for the page allocator (frame allocator)
        Pager_ISavepoint::GetInstance().BeginSavepoint( task.ID() );

        Pager_ISavepoint::GetInstance().FlushDataCacheBeforePrepare( task.ID(), numPages, numIO );
        numPagesTotal += numPages;
        numIOTotal    += numIO;

        /*************************************************************************/
        /***** all cached sequences must be flushed now                      *****/
        /*************************************************************************/

        // PTS 1117320 UH 2002-10-28
        // PTS ? UH 2003-05-07 moved k57flush_sequences() after FlushDataCacheBeforePrepare()
        k57flush_sequences (Trans);
  
        if ( Trans.trError_gg00 != e_ok )
        {
            errlist.AppendNewMessage(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "k57flush_sequences"));
            InternalErrorHandling(exclusiveRequestor, errlist);
            return;
        }
  
        /*************************************************************************/
        /***** Prevent update operations and wait until last update is finished *****/
        /*************************************************************************/

        bd20SetPreventSvpSyncOperation( task.ID() );

        Pager_ISavepoint::GetInstance().WaitForLastSvpSyncOperation( task.ID(), numPages, numIO );
        numPagesTotal += numPages;
        numIOTotal    += numIO;

        k51enter_all_trans_regions (task.ID());

        const bool bIsRedoSavepoint = ! m_LogWriteEnabled;

        if ( ! DetermineRedoStartPositions ( context, saveptParam) )
        {
            InternalErrorHandling(exclusiveRequestor, errlist);
            k51leave_all_trans_regions (task.ID());
            k57end_flush_sequences (task.ID());
            return;
        }

        // PTS 1117320 UH 2002-10-28 - re-enable surrogates to be written to the log
        k57end_flush_sequences (task.ID());

        if ( ! CreateOpenTransFile (context, saveptParam, bIsRedoSavepoint) )
        {
            InternalErrorHandling(exclusiveRequestor, errlist);
            k51leave_all_trans_regions (task.ID());
            return;
        }

        Data_PageNo newHistRoot;
        if (! Log_History::GetInstance().Flush(Trans,newHistRoot,errlist))
        {
            errlist.Overrule(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,"Flush history-directory"));
            InternalErrorHandling(exclusiveRequestor, errlist);
            k51leave_all_trans_regions (task.ID());
            return;
        }
        saveptParam.svpHistoryRoot_kb00 = newHistRoot;
        /******************************************************************************/
        /*****    mark all svp relevant pages within data cache for final flush    ****/
        /******************************************************************************/

        Converter_Version converterVersion = Converter_ICommon::Instance().Version();
        
        FileDir_ISharedDirectory::Instance().BeginSavepoint( task.ID() );
        // Set the savepoint state in the component converter to active before the data cache 
        // is prepared to guarantee that no gap exist during the operation "FreeDataPageNo", 
        // which possible cause an inconsistency between data and converter
        Converter_ISavePoint::Instance().BeginSavepoint( task );

        bd20PrepareSavepoint( task.ID(), converterVersion );
        
        bd20UnSetPreventSvpSyncOperation( task.ID() );

        if ( Log_SVPReasonStartDropVolume  == exclusiveRequestor.reason)
        {
            // We have to ensure that no write operations into the data area is pending, 
            // because this could lead to a remaining page on the volume to be dropped
            if( ! bd20PrepareDropDataVolume( task.ID(), &errlist ))
            {
                context.OldTrans().trError_gg00 = e_system_error;
                InternalErrorHandling( exclusiveRequestor, errlist );
                k51leave_all_trans_regions( task.ID());
                return;
            }
        }

        BeginRegion(task);

        m_WaitingForPrepare.ResumeAll();

        /*********************************************/
        /***** new converter version is used now *****/
        /*********************************************/

        EndRegion(task);

        k51leave_all_trans_regions (task.ID());

        //if ( Log_SVPReasonShutdown != exclusiveRequestor.reason ) // PTS 1129812 TS 2004-05-25
        //    bd91StartOMSGarbageCollectionAfterSpecialOp( task.ID() ); // PTS 1139163

        Pager_ISavepoint::GetInstance().FlushDataCacheAfterPrepare( task.ID(), numPages, numIO );
        numPagesTotal += numPages;
        numIOTotal += numIO;

        // security check, if all savepoint relevant data pages are written
        bd20IsSavepointCompleted (task.ID());

        FileDir_ISharedDirectory::Instance().Flush( task.ID() );

        // Prepare converter for backup. Note that this will change converter pages
        // and this pages have to be flushed with the current savepoint.
        
        if ( Log_SVPReasonSaveData  == exclusiveRequestor.reason
             ||
             Log_SVPReasonSavePages == exclusiveRequestor.reason )
        {
            Converter_ReturnCode retCode;
            
            if ( Log_SVPReasonSaveData == exclusiveRequestor.reason )
                retCode = Converter_IBackUp::Instance().BeginSaveData(task, errlist);
            else
                retCode = Converter_IBackUp::Instance().BeginSavePages(task, errlist);
                
            if( Converter_Ok != retCode )
            {   
                if( Converter_MemoryAllocationFailed == retCode )
                    context.OldTrans().trError_gg00 = e_no_more_memory;
                else
                    context.OldTrans().trError_gg00 = e_system_error;

                InternalErrorHandling(exclusiveRequestor, errlist);
                return;
            }
        }
        
        IOMan_PackedBlockAddress  packedRootBlockAddr;
        Data_PageNo               maxDynamicPageNo;
        Data_PageNo               maxStaticPageNo;

        if ( Log_SVPReasonDropVolume == exclusiveRequestor.reason )
        {
            Converter_ISavePoint::Instance().PrepareForDropVolume( task );
        }

        Converter_ISavePoint::Instance().FlushParallel( task,
                                                        converterVersion, 
                                                        maxStaticPageNo,
                                                        maxDynamicPageNo,
                                                        packedRootBlockAddr,
                                                        numPages,
                                                        numIO );

        numPagesTotal += numPages;
        numIOTotal    += numIO;

        if ( IOMan_IDataManager::GetInstance().IsRestartPageOnDropVolume()
             &&
             Log_SVPReasonDropVolume == exclusiveRequestor.reason )
        {
            IOMan_IDataManager::GetInstance().MoveRestartPage(task.ID());
        }
        
        FlushRestartRecordAndLogInfoPages ( context,
                                            saveptParam,
                                            converterVersion,
                                            packedRootBlockAddr,
                                            maxDynamicPageNo,
                                            maxStaticPageNo );

        FlushCompleted( task, converterVersion );

        if ( Log_SVPReasonShutdown == exclusiveRequestor.reason )
        {
            Pager_ISavepoint::GetInstance().EndSavepointForShutdown( task.ID());
            ShutdownFilesystem (Trans);
        }
        else
            Pager_ISavepoint::GetInstance().EndSavepoint( task.ID() );

        if ( exclusiveRequestor.IsAssigned() )
            vresume (exclusiveRequestor.taskid);
                
        BeginRegion (task);

        // PTS 1123461 UH 2003-08-28 wake up of waiting tasks is now done here after savepoint is valid.
        WaitListForCurrentSavepoint().ResumeAll();

        if ( exclusiveRequestor.reason == Log_SVPReasonShutdown )
            loopAgain = false;
        else
        {
            if ( m_ExclusiveRequestor.IsAssigned() )
            {
                loopAgain = true;
                PrintReason (task.ID(), m_ExclusiveRequestor.reason, true);
            }
            else
                if ( CurrentWaitList().IsEmpty() )
                    loopAgain = false;
                else
                {
                    loopAgain = true;
                    PrintReason (task.ID(), Log_SVPReasonUnknown, true);
                }
        }
        
        if ( ! loopAgain )
            m_IsActive = false;

        EndRegion (task);
        
        // PTS 1119276 UH 2002-12-10 begin
        vclock(&endtimeSec,&endtimeMSec);
        m_Statistics.Add(Statistic(endtimeSec  - starttimeSec,
                                   endtimeMSec - starttimeMSec,
                                   numPagesTotal, numIOTotal));
        // PTS 1119276 UH 2002-12-10 end
    }
    while ( loopAgain );
}

/*---------------------------------------------------------------------------*/
bool Log_Savepoint::DetermineRedoStartPositions ( Trans_Context     &trans,
                                                  tkb00_SaveptParam &saveptParam)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::DetermineRedoStartPositions", LogTrans_Trace, 5);

    if (!m_LogWriteEnabled)                     // PTS 1115875 mb 2002-05-28
    {
        if ( Log_SVPReasonRestart == saveptParam.svpReason_kb00 )
            m_LogWriteEnabled = true; // PTS 1126112 UH 2003-12-09 
        else
        {
            Log_EntryInfo entryInfo;
            Kernel_IAdminRestartShutdown::Instance().GetRedoManager().GetLastEntryRead (entryInfo);
            if ( Log_IOSequenceNo(entryInfo.iosequence).IsValid() )
            {
                if ( Log_IOSequenceNo(saveptParam.svpIOsequence_kb00).IsInvalid() )
                    RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                                  "Savepoint IOSequence must be valid") );
                saveptParam.svpIOsequence_kb00       = entryInfo.iosequence.RawValue();
                saveptParam.svpStartOffset_kb00      = entryInfo.pageoffset.RawValue();
                saveptParam.svpStartEntryOffset_kb00 = entryInfo.entryoffset;
                saveptParam.svpStartEntryType_kb00   = entryInfo.entrytype;
                saveptParam.svpStartEntryDate_kb00   = entryInfo.entrydate;
                saveptParam.svpStartEntryTime_kb00   = entryInfo.entrytime;
            }
            return true;
        }
    }
    
    Msg_List           &errlist = trans.ErrorList();
    
    Log_AfterImage      AfterImage;
    Log_ActionSavepoint Action (Log_SavepointReason(saveptParam.svpReason_kb00));

    AfterImage.PrepareForWrite( Action.GetType(),
                                Action.GetPersistentVersion(),
                                trans.OldTrans().trWriteTransId_gg00,
                                saveptParam.svpId_kb00,
                                saveptParam.svpId_kb00 );
    
    Log_AfterImageSpace Space (trans.Allocator());

    if ( ! Space.Initialize() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "Space.Initialize()");
        return false;
    }
    
    Log_Queue           *queue = NULL;                // PTS 1135304 mb 2005-05-24
    Log_Volume::Instance().GetQueueForWriting(trans.Task().ID(), queue, LOG_USE_QUEUE_ZERO); // PTS 1135304 mb 2005-05-24
    Log_IOSequenceNo     ioSeqNo;
    Log_RawDeviceOffset  devOffset;
    tgg00_WaitContext    waitContext;

    if ( 0 == queue )
    {
        trans.ErrorList() = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "queue is assigned");
        return false;
    }

    ioSeqNo.Invalidate();
    devOffset.Invalidate();
    saveptParam.svpStartEntryOffset_kb00 = 0;

    if (!queue->ReserveForEOT ( trans.Task().ID(),
                                waitContext,
                                AfterImage.GetPersistentLength()
                                +
                                Action.GetPersistentLength(),
                                Space,
                                saveptParam.svpStartEntryOffset_kb00,
                                errlist ) )
    {
        AfterImage.AppendContextInfo(errlist);
        Action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    SAPDBERR_ASSERT_STATE ( (AfterImage.GetPersistentLength() \
                             + \
                             Action.GetPersistentLength()) == Space.Length() );

    Data_SplitSpaceWriter Writer (Space);
    
    if(!AfterImage.WritePersistentFormat (Writer, trans.ErrorList()))
    {
        AfterImage.AppendContextInfo(trans.ErrorList());
        Action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if(!Action.WritePersistentFormat (Writer, errlist))
    {
        AfterImage.AppendContextInfo(errlist);
        Action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    queue->UserTaskEOTReady ( trans.Task().ID(),
                             waitContext,
                             Space,
                             ioSeqNo,
                             devOffset );
    Log_Volume::Instance().ReleaseQueue(trans.Task().ID(), queue); //  PTS 1135304 mb 2005-05-20
                            
    saveptParam.svpIOsequence_kb00     = ioSeqNo.RawValue(); // PTS 1124724 mb 2003-10-29
    saveptParam.svpStartOffset_kb00    = devOffset.RawValue();
    saveptParam.svpStartEntryType_kb00 = AfterImage.GetActionType();
    saveptParam.svpStartEntryDate_kb00 = saveptParam.svpDate_kb00;
    saveptParam.svpStartEntryTime_kb00 = saveptParam.svpTime_kb00;
    
    if ( LogTrans_Trace.TracesLevel(7) )
    {
        Kernel_OpInfo knldiag (csp3_n_savepoint);
    
        knldiag << "SVP: Position: " << ioSeqNo
                << " @ " << devOffset
                << "."   << saveptParam.svpStartEntryOffset_kb00;
    }
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "SVP: IOSeqNo: " << saveptParam.svpIOsequence_kb00 << \
                                      ", OffsetOnDevice: " << saveptParam.svpStartOffset_kb00 << \
                                      ", OffsetInPage: "   << saveptParam.svpStartEntryOffset_kb00);
    return true;
}

/*---------------------------------------------------------------------------*/
void Log_Savepoint::FlushRestartRecordAndLogInfoPages
    (Trans_Context             &trans,
     tkb00_SaveptParam          &saveptParam,
     Converter_Version          &converterVersion,
     IOMan_PackedBlockAddress   &packedRootBlockAddr,
     Data_PageNo                &maxDynamicPageNo,
     Data_PageNo                &maxStaticPageNo)
{
    BeginRegion (trans.Task());
    
    k57restartrec->rstLastSavept_kb00() = saveptParam;

    g11kernel_version ( k57restartrec->rstReleaseVersion_kb00() );
    
    if ( Log_SVPReasonSaveData == saveptParam.svpReason_kb00 )
        k57restartrec->rstLastSaveDataSuccessful_kb00() = false;
        
    k57restartrec->rstPrevConverterVers_kb00() = k57restartrec->rstConverterVersion_kb00();
    k57restartrec->rstConverterVersion_kb00()  = converterVersion;
    k57restartrec->rstSetEndReadOnly_kb00()    = false;
    
    tkb00_ConfigRestartParam &conf = k57restartrec->rstConfigParam_kb00();

    conf.crConvRootBlockAddr_kb00 = packedRootBlockAddr;
    conf.crMaxDynamicPno_kb00     = maxDynamicPageNo;
    conf.crMaxStaticPno_kb00      = maxStaticPageNo;

    k57save_restartrecord (trans.Task().ID()); // now the new savepoint is valid

    if ( Log_SVPReasonSaveData == saveptParam.svpReason_kb00
         ||
         Log_SVPReasonSavePages == saveptParam.svpReason_kb00 )
	{
        *k57frozen_restartrec = *k57restartrec;
	}
    
    EndRegion (trans.Task());

    if ( ! Kernel_IAdminHotStandby::Instance().RoleIsStandby() )
        Log_Volume::Instance().WriteSavepointValuesAndFlushInfoPage
                    (trans.Task().ID(),
                     Log_SVPReasonRestart == saveptParam.svpReason_kb00, // PTS 1137938
                     k57restartrec->rstReleaseVersion_kb00(),
                     SAPDBFields_Field(reinterpret_cast<SAPDB_Byte*>(&saveptParam), sizeof(saveptParam)));
}

/*---------------------------------------------------------------------------*/
void Log_Savepoint::FlushCompleted (const RTETask_ITask     &task,
                                    const Converter_Version &converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::FlushCompleted", LogTrans_Trace, 5);

    // Set all blocks from state FreeAfterSavepoint to Free. Note that this
    // has to take place BEFORE the data cache AND the converter are informed 
    // about the savepoint end situation, because: 
    // 
    // First: this guarantees that blocks which are written after the data cache and 
    // converter flush initiated by the savepoint won't be set to FreeAfterSavepoint
    // resp. Free immediately. (PTS 1132144)
    // 
    // Second: this guarantees that the blocks of the pageNo's which are released at 
    // the end of the savepoint won't be set to FreeAfterSavepoint resp. Free 
    // immediately. (PTS 1137295)

    FBM_IManager::Instance().SetAllBlocksMarkedAsFreeAfterSVPToFree( task.ID());

    // Increment converter version. At this time all savepoint relevant data
    // pages and the converter pages are written to the data area.
    // The data pages written next, will get the incremented converter version!
    // At this point the converter is not in savepoint mode, which means that there
    // is no special handling for pageNo's and the corresponding blocks to be released!

    Converter_Version newConverterVersion;
    Converter_ISavePoint::Instance().EndSavepoint( task, newConverterVersion );

    // Reset savepoint state and enable data cache to write non savepoint relevant 
    // data pages to the data area

    bd20SavepointCompleted( task.ID(), converterVersion );

    FileDir_ISharedDirectory::Instance().EndSavepoint( task.ID(), newConverterVersion );
    
    // Free pnos removed during prepare phase 
    Converter_ISavePoint::Instance().FreePageNosAfterSavepoint( task );
}

/*---------------------------------------------------------------------------*/
bool Log_Savepoint::CreateOpenTransFile (Trans_Context      &trans,
                                         tkb00_SaveptParam  &saveptParam,
                                         bool                bIsRedoSavepoint)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Savepoint::CreateOpenTransFile", LogTrans_Trace, 5);

    Msg_List          &errlist = trans.ErrorList();
    tgg00_TransIndex  WriteTransIndex = cgg_nil_transindex;
    Log_Transaction  *pLogTrans       = NULL;
    Log_IOSequenceNo  oldestKnownIOSequence; // PTS 1124684 UH 2003-10-16
    Log_IOSequenceNo  oldestOpenTransIOSequence; // PTS 1124684 UH 2003-10-16
    Log_OpenTransFile File             (trans.OldTrans(), saveptParam.svpId_kb00, bIsRedoSavepoint);
    const SAPDB_Char* openTransFileWithQueueId = "OPEN TRANS FILE WITH QUEUE ID";

    Kernel_Migration::ChangeInfo&  queueIdInOpentransfile =
        Kernel_IAdminConfig::Instance().MigrationHandler().SearchForChangeInfo (openTransFileWithQueueId );
    Log_OpenTransFile::EntryLayout opentransFileEntryLayout; 
    if ( ! m_openTransfileCreated && queueIdInOpentransfile.IsEnabled() ) 
    {
        #ifdef SAPDB_SLOW
        Kernel_VTrace() << "Log_Savepoint::CreateOpenTransFile : old filelayout used" << NewLine ;
        #endif
        opentransFileEntryLayout = Log_OpenTransFile::HasNoQueueId; 
    }
    else
    {
        #ifdef SAPDB_SLOW
        Kernel_VTrace() << "Log_Savepoint::CreateOpenTransFile : new filelayout used" << NewLine ;
        #endif
        opentransFileEntryLayout = Log_OpenTransFile::HasQueueId; 
    }
    Log_OpenTransFile OldOpenTransFile (trans.OldTrans(), saveptParam.svpOpenTransRoot_kb00, opentransFileEntryLayout );
    
    if ( saveptParam.svpOpenTransRoot_kb00 == NIL_PAGE_NO_GG00 )  // PTS 1124684 UH 2003-10-16
        oldestKnownIOSequence = 0;
    else
    {
        errlist.ClearMessageList();
        oldestKnownIOSequence = OldOpenTransFile.GetOldestKnownIOSequence(errlist);
        if(!errlist.IsEmpty())
        {
            errlist.Overrule (Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"OldOpenTransFile.GetOldestKnownIOSequence"));
            return false;
        }
    }
    
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "SVP: oldestKnownIOSequence: " << oldestKnownIOSequence);

    if ( ! File.Create (errlist) ) // PTS 1121659 UH 2003-04-30
    {
        errlist.Overrule(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"new OpenTransFile.Create"));
        return false;
    }
                  
    saveptParam.svpOpenTransCnt_kb00 = 0;
    
    Log_IOSequenceNo transOldestKnownIOSequence;
    
    for ( kb51GetFirstWriteTrans (WriteTransIndex);
          WriteTransIndex != cgg_nil_transindex;
          kb51GetNextWriteTrans  (WriteTransIndex) )
    {
        SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "SVP: OpenTransNo: "  << saveptParam.svpOpenTransCnt_kb00<<\
                                               ", WriteTransIndex: " << WriteTransIndex);
        
        pLogTrans = reinterpret_cast<Log_Transaction*>(k50GetLogTransaction(WriteTransIndex));
        
        if ( NULL == pLogTrans )
        {
            Msg_List auxErrList;
            if ( ! File.Drop(auxErrList) )
                RTE_Message(auxErrList);
            errlist= Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"pLogTrans is assigned");
            return false;
        }
        
        if ( pLogTrans->IsSavepointRelevant() )
        {
            if ( saveptParam.svpReason_kb00 == Log_SVPReasonRestart )
            {
                Msg_List auxErrList;
                if ( ! File.Drop(auxErrList) )
                    RTE_Message(auxErrList);
                // PTS 1124684 UH 2003-10-16 inserted check
                pLogTrans->WriteToTrace("open trans not allowed");
                errlist.Overrule(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "open redo trans found"));
                pLogTrans->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            ++saveptParam.svpOpenTransCnt_kb00;
            if(!File.WriteOpenTrans (*pLogTrans, errlist))
            {
                errlist.Overrule(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "File.WriteOpenTrans"));
                pLogTrans->AppendContextInfo(__CONTEXT__,errlist);
                Msg_List auxErrList;
                if ( ! File.Drop(auxErrList) )
                    RTE_Message(auxErrList);
                return false;
            }
            transOldestKnownIOSequence = pLogTrans->GetOldestKnownIOSequence();
            if ( oldestOpenTransIOSequence.IsInvalid()  // PTS 1124684 UH 2003-10-16
                 ||
                 Log_IOSequenceNo::LeftIsOlder(transOldestKnownIOSequence, // PTS 1127009 UH 2004-01-09
                                               oldestOpenTransIOSequence,
                                               oldestKnownIOSequence) )
            {
                oldestOpenTransIOSequence = transOldestKnownIOSequence; // PTS 1127009 UH 2004-01-09
            }
        }
    }
    
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "SVP: new oldestEOTsequence: " << oldestOpenTransIOSequence);
    if(!File.SetOldestKnownIOSequence(oldestOpenTransIOSequence,errlist))  // PTS 1124684 UH 2003-10-16
    {
        errlist.Overrule(Log_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,"File.SetOldestKnownIOSequence"));
        Msg_List auxErrList;
        if ( ! File.Drop(auxErrList) )
            RTE_Message(auxErrList);
        return false;
    }

    if ( saveptParam.svpOpenTransRoot_kb00 != NIL_PAGE_NO_GG00 )
    {
        Msg_List auxErrList;
        if(!OldOpenTransFile.Drop(auxErrList))
            RTE_Message(auxErrList);
        SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "SVP: old OpenTransRoot: " << saveptParam.svpOpenTransRoot_kb00);
    }
    saveptParam.svpOpenTransRoot_kb00 = File.GetRootId().PageNo();
    SAPDBTRACE_WRITELN (LogTrans_Trace, 6, "SVP: new OpenTransRoot: " << saveptParam.svpOpenTransRoot_kb00);

    Kernel_DateTime dateTime;

    dateTime.DetermineTime();
    saveptParam.svpDate_kb00 = dateTime.GetDate();
    saveptParam.svpTime_kb00 = dateTime.GetTime();
    m_openTransfileCreated = true;
    return true;
}

/*---------------------------------------------------------------------------*/

void Log_Savepoint::ShutdownFilesystem (tgg00_TransContext &coordinatorTrans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_Savepoint::ShutdownFilesystem", LogTrans_Trace, 5);

    k38e_autosave_end (coordinatorTrans, false);

    Kernel_VTrace() << "\n"
                    << "\n"
                    << "############################################\n"
                    << "############### SHUTDOWN ###################\n";
    Kernel_VTrace() << "############################################\n"
                    << "\n"
                    << "\n";

    Kernel_IAdminRestartShutdown::Instance().Offline(coordinatorTrans.trError_gg00);

    RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
               "Log_Savepoint::Shutdown: kernel must not run at this moment.") );
}

/*---------------------------------------------------------------------------*/
bool Log_Savepoint::Verify (tgg00_TransContext   &trans,
                            bool                  isCold,
                            SAPDBErr_MessageList &errlist)
{
    Log_OpenTransFile file (trans, k57restartrec->rstLastSavept_kb00().svpOpenTransRoot_kb00);
    if ( isCold && file.IsCreated() )
        return file.Verify(isCold,errlist);
    else
        return true;
}

/*---------------------------------------------------------------------------*/
Log_EntrySequence Log_Savepoint::GetLastSavepointId ()
{
    return 0 == k57restartrec
           ? Log_EntrySequence()
           : Log_EntrySequence(k57restartrec->rstLastSavept_kb00().svpId_kb00);
}

/*---------------------------------------------------------------------------*/
Log_ActionType Log_Savepoint::GetLastSavepointEntryType()
{
    return 0 == k57restartrec
           ? Log_NoOp : Log_ActionType(k57restartrec->rstLastSavept_kb00().svpStartEntryType_kb00);
}

/*---------------------------------------------------------------------------*/
Log_IOSequenceNo Log_Savepoint::GetLastSavepointIOSequence()
{
    return 0 == k57restartrec
           ? Log_IOSequenceNo()
           : Log_IOSequenceNo(k57restartrec->rstLastSavept_kb00().svpIOsequence_kb00);
}

/*---------------------------------------------------------------------------*/
Log_RawDeviceOffset Log_Savepoint::GetLastSavepointOffset()
{
    return k57restartrec->rstLastSavept_kb00().svpStartOffset_kb00;
}

/*---------------------------------------------------------------------------*/
void Log_Savepoint::GetStartEntryTimestamp( Kernel_Date &date,
                                            Kernel_Time &time )
{
    date.Invalidate();
    time.Invalidate();
    
    if ( k57restartrec != 0 ) return;
    
    date = k57restartrec->rstLastSavept_kb00().svpStartEntryDate_kb00;
    time = k57restartrec->rstLastSavept_kb00().svpStartEntryTime_kb00;
}
/*---------------------------------------------------------------------------*/
void Log_Savepoint::InsertToWaitForPrepare (RTETask_ITask     &task,
                                            tgg00_WaitContext &waitcontext)
{
    BeginRegion(task);
    m_WaitingForPrepare.InsertAsLast(task.ID(), waitcontext);
    EndRegion(task);
}

/*---------------------------------------------------------------------------*/
void Log_Savepoint::GetRestartInformation ( RTETask_ITask      &task,
                                            RestartInformation &info )
{
    tkb00_SaveptParam  &saveptParam = k57restartrec->rstLastSavept_kb00();
    SAPDB_UInt          copyLength;
    
    BeginRegion(task);

    info.badIndexCount            = FileDir_ISharedDirectory::Instance().GetBadIndexCount();
    info.previousConverterVersion = k57restartrec->rstPrevConverterVers_kb00();
    info.lastSavepointId          = GetLastSavepointId();

    copyLength = sizeof(info.lastSavepointReason) > SAPDB_strlen(Log_SavepointReasonStrings[saveptParam.svpReason_kb00])+1
                 ? SAPDB_strlen(Log_SavepointReasonStrings[saveptParam.svpReason_kb00])+1
                 : sizeof(info.lastSavepointReason);
    SAPDB_strncpy_zero_terminated (info.lastSavepointReason, Log_SavepointReasonStrings[saveptParam.svpReason_kb00], copyLength);

	info.lastSavepointDate        = saveptParam.svpDate_kb00;
	info.lastSavepointTime        = saveptParam.svpTime_kb00;
    info.openTransactionCount     = saveptParam.svpOpenTransCnt_kb00;
    info.openTransactionFileRoot  = saveptParam.svpOpenTransRoot_kb00;
	info.historyDirectoryFileRoot = saveptParam.svpHistoryRoot_kb00;
	info.fileDirectoryRoot        = k57restartrec->rstConfigParam_kb00().crFdirRoot_kb00;
	info.longFileDirectoryRoot    = k57restartrec->rstConfigParam_kb00().crLongDirRoot_kb00;
	info.restartIOSequence        = GetLastSavepointIOSequence().RawValue();
    info.restartLogVolumeOffset   = GetLastSavepointOffset().RawValue();
	info.restartLogEntryOffset    = saveptParam.svpStartEntryOffset_kb00;

    copyLength = sizeof(info.restartLogEntryType) > SAPDB_strlen(Log_GetActionTypeString(GetLastSavepointEntryType()))+1
                 ? SAPDB_strlen(Log_GetActionTypeString(GetLastSavepointEntryType()))+1
                 : sizeof(info.restartLogEntryType);
    SAPDB_strncpy_zero_terminated (info.restartLogEntryType, Log_GetActionTypeString(GetLastSavepointEntryType()), copyLength);

    info.restartEntryDate        = saveptParam.svpStartEntryDate_kb00;
    info.restartEntryTime        = saveptParam.svpStartEntryTime_kb00;

    copyLength = sizeof(info.databaseIdentifier) > sizeof(k57restartrec->rstDbIdent_kb00())+1
                 ? sizeof(k57restartrec->rstDbIdent_kb00())+1
                 : sizeof(info.databaseIdentifier);
    SAPDB_strncpy_zero_terminated (info.databaseIdentifier, k57restartrec->rstDbIdent_kb00(), copyLength);

    copyLength = sizeof(info.currentSoftwareVersion) > sizeof(k57restartrec->rstReleaseVersion_kb00())+1
                 ? sizeof(k57restartrec->rstReleaseVersion_kb00())+1
                 : sizeof(info.currentSoftwareVersion);
    SAPDB_strncpy_zero_terminated (info.currentSoftwareVersion, k57restartrec->rstReleaseVersion_kb00(), copyLength);

    copyLength = sizeof(info.installationSoftwareVersion) > sizeof(k57restartrec->rstConfigParam_kb00().crReleaseVers_kb00)+1
                 ? sizeof(k57restartrec->rstConfigParam_kb00().crReleaseVers_kb00)+1
                 : sizeof(info.installationSoftwareVersion);
    SAPDB_strncpy_zero_terminated (info.installationSoftwareVersion, k57restartrec->rstConfigParam_kb00().crReleaseVers_kb00, copyLength);

    EndRegion(task);
}
