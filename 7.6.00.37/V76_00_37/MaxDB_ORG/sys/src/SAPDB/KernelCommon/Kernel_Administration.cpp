/*!**************************************************************************

  module      : Kernel_Administration.cpp
  special area: Kernel Management
  responsible : UweH
  created     : 2002-12-11
  last changed: 2002-12-11  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : This is the handler which coordinates all
                DBM commands in the kernel. (utility)

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
#include "KernelCommon/Kernel_Administration.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Admin.h"
#include "SAPDBCommon/Messages/SDBMsg_Backup.h"
#include "SAPDBCommon/Messages/SDBMsg_Recovery.h"

#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_KernelInterface.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_StandbyNodeList.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"

#include "KernelCommon/Kernel_Types.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_Messages.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_RestartPage.hpp"
#include "KernelCommon/Kernel_Task.hpp"

#include "IOManager/IOMan_IDataManager.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "IOManager/IOMan_IDataVolumeInfo.hpp"
#include "IOManager/IOMan_ILogVolumeInfo.hpp"
#include "IOManager/IOMan_IDataIO.hpp"
#include "IOManager/IOMan_ILogManager.hpp"
#include "IOManager/IOMan_ILogInfo.hpp"
#include "IOManager/IOMan_IDiagnose.hpp"
#include "IOManager/IOMan_IMigration.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"

#include "Logging/Log_Types.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_History.hpp"

#include "FrameControl/FrameCtrl_IIOManager.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"

#include "Converter/Converter_IBackUp.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_IManager.hpp"
#include "Converter/Converter_IDiagnose.hpp"

#include "DataAccess/Data_Types.hpp"

#include "SQLManager/Catalog/Catalog_Interface.hpp"
#include "SQLManager/SQLMan_Context.hpp"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"

#include "ServerTasks/SrvTasks_JobCollection.hpp"
#include "ServerTasks/SrvTasks_JobRedo.hpp"

#include "Transaction/Trans_Context.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"

#include "gsp00.h"   // tsp00_TaskId
#include "ggg92.h"   // tgg92_KernelOid
#include "ggg00.h"   // tgg00_TransContext
#include "hak10.h"   // a10expand_rec
#include "hak36_1.h" // a36PrepareCatalogForUserConnect
#include "hak93.h"   // a93reply
#include "hbd01.h"   // b01restart_filesystem
#include "hbd01_1.h" // b01niltree_id
#include "hbd02.h"   // b02next_record
#include "hbd03.h"   // bd03ExtractIndex
#include "hbd05.h"   // bd05ExtractLob
#include "hbd06.h"   // b06create_errortext_file
#include "hbd10.h"   // b10rc_restart_converter
#include "hgg01_3.h" // g01tabid
#include "hgg17.h"   // g17basis_err_to_line
#include "hkb51.h"   // k51restart_locklist
#include "hkb53.h"   // k53StartInternalSession,k53StopInternalSession
#include "hkb57.h"   // k57save_restartrecord / k57isSnapshot
#include "hkb57_1.h" // k57restartrec
#include "hkb38.h"   // k38init_restore / k38headmaster
#include "hkb38_1.h" // k38is_on_autosave
#include "hkb900.h"  // kb900AddToPrefixDestroyFile
#include "hbd17.h"   // bd17GetBlobFdirRoot
#include "hbd20.h"   // bd20FlushDataCache
#include "heo51.h"   // voffline
#include "heo60.h"   // vtime_in_seconds
#include "heo79.h"   // vxparam_save_good_config
#include "heo58.h"   // vget_uniqe_id
#include "heo51.h"   // vgetpid
#include "hsp20.h"   // s20int4_to_buf_swap
#include "heo47.h"   // sql47_ptoc
#include "hbd91.h"   // bd91StartOMSGarbageCollectionAfterSpecialOp & bd91EnableOMSHistoryRemoval
#include "heo670.h"  // vtracestack
#include "geo67.h"   // eo67InsertStandbyEvent

/* --------------------------------------------------------------------------- */
/// This suspend reason is used to wait for end of redo in case of HotStandby::TakeOver
#define KERNEL_ADMIN_WAIT_FOR_TAKEOVER 250 // suspend reasons: heo00x.h
#define KERNEL_ADMIN_WAIT_FOR_BACKUP 55            // suspend reasons: heo00x.h
/* --------------------------------------------------------------------------- */
static void PickUpAuxFilesForDestroy( tgg00_TransContext &trans )
{

    tgg00_FileId    auxFileId = b01niltree_id;

    auxFileId.fileTfn_gg00().becomes( tfnAux_egg00 );

    do
    {
        b01next_fdir_entry( trans, auxFileId );

        if( tfnAux_egg00 != auxFileId.fileTfn_gg00())
            break;

        if( e_ok == trans.trError_gg00 )
            kb900AddToPrefixDestroyFile( trans, auxFileId.fileTransId__7_gg00());
    }
    while( e_ok == trans.trError_gg00 );

    trans.trError_gg00 = e_ok; // ignore errors

    // PTS 1134924 M.Ki.
    // remove all tfnTempAux files from blob file directory
    auxFileId = b01niltree_id;
    auxFileId.fileTfn_gg00().becomes( tfnTempAux_egg00 );
    tbd_fileinfo   fi;
    tgg00_Filename fn = auxFileId.fileName_gg00();
    do
    {
        b17next_del_fdir( auxFileId.fileName_gg00(), 1, fn, fi, trans );
    }
    while( e_ok == trans.trError_gg00 );

    trans.trError_gg00 = e_ok; // ignore errors
}

/* --------------------------------------------------------------------------- */
Kernel_Administration* Kernel_Administration::m_Instance = 0;
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::CreateInstance ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_Administration::CreateInstance", Common_Trace, 5);
    SAPDBMEM_STATIC_RAW_ALLOCATE (Space, sizeof(Kernel_Administration));
    SAPDBERR_ASSERT_STATE(m_Instance == 0);
    m_Instance = new (Space) Kernel_Administration (RTEMem_Allocator::Instance());
    return m_Instance != 0;
}
/* --------------------------------------------------------------------------- */
Kernel_Administration& Kernel_Administration::Instance()
{
    SAPDBERR_ASSERT_STATE(m_Instance != 0);
    return *m_Instance;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminRestartShutdown& Kernel_IAdminRestartShutdown::Instance()
{
    return Kernel_Administration::Instance();
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby& Kernel_IAdminHotStandby::Instance()
{
    return Kernel_Administration::Instance();
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminBackupRecovery& Kernel_IAdminBackupRecovery::Instance()
{
    return Kernel_Administration::Instance();
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminInfo& Kernel_IAdminInfo::Instance()
{
    return Kernel_Administration::Instance();
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminConfig& Kernel_IAdminConfig::Instance()
{
    return Kernel_Administration::Instance();
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminDiagnose& Kernel_IAdminDiagnose::Instance()
{
    return Kernel_Administration::Instance();
}
/* --------------------------------------------------------------------------- */
Kernel_Administration::Kernel_Administration(SAPDBMem_IRawAllocator &allocator)
: m_Allocator(UTF8("Kernel_Administration"),allocator),
  m_SpinLock((SAPDB_UTF8*)"Kernel_Administration::m_SpinLock"),
  m_CommandNo(0),
  m_State(m_SpinLock),
  m_WaitingForTakeOverReady(cgg_nil_pid),
  m_DataBackupIsRunning(false),
  m_SynchronizationIsRunning(false),
  m_SynchronizationIsNeeded(false)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::Kernel_Administration", Common_Trace, 5);
    RTE_Message (Admin_Exception(__CONTEXT__, KERNEL_STATE_IS_ADMIN));
}
/* --------------------------------------------------------------------------- */
Kernel_Administration::ActivateResult
Kernel_Administration::Activate (tsp00_TaskId          taskid,
                                 tgg00_TransContext   &oldcontext,
                                 SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::Activate", Common_Trace, 5);

    Kernel_StateScope state (Kernel_State::criticalStateRedo, m_State);

    if ( ! state.EnterCriticalState() )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "Redo");
        return ActivateNotAllowed;
    }
    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(oldcontext.trAllocator_gg00));
    const RestartInternResult result =
        RestartIntern ( taskid, allocator, oldcontext, restartKindActivation,
                        Kernel_Date(), Kernel_Time(), Log_IOSequenceNo(), errlist);

    if ( result != RestartInternOk )
        return ActivateRestartInternFailed;

    return ActivateOk;
}
/* --------------------------------------------------------------------------- */
Kernel_Administration::RestartResult
Kernel_Administration::Restart (tsp00_TaskId          taskid,
                                tgg00_TransContext   &oldcontext,
                                Kernel_Date           untildate,
                                Kernel_Time           untiltime,
                                Log_IOSequenceNo      untilIOSequence,
                                SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::Restart", Common_Trace, 5);

    if ( m_State.IsOnline() )
        return RestartNotAllowed;

    Kernel_StateScope stateBackup (Kernel_State::criticalStateBackup, m_State);
    Kernel_StateScope stateRedo   (Kernel_State::criticalStateRedo,   m_State);

    if ( ! m_State.IsCriticalState(Kernel_State::criticalStateBackup)
         &&
         ! stateBackup.EnterCriticalState() )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "Backup");
        return RestartNotAllowed;
    }

    if ( ! stateRedo.EnterCriticalState() )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "Redo");
        return RestartNotAllowed;
    }

    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(oldcontext.trAllocator_gg00));
    if ( RestartInternOk != RestartIntern (taskid, allocator, oldcontext, restartKindNormal,
                                           untildate, untiltime, untilIOSequence, errlist) )
        return RestartRestartInternFailed;

    return RestartOk;
}

/* --------------------------------------------------------------------------- */
bool Kernel_Administration::IsMasterAlive(SAPDBErr_MessageList &errlist)
{
    // PTS 1127374 UH 2004-01-23 new
    RTE_Nodename                    masterNodeName;
    RTEHSS_KernelInterface::Mapping dummyOtherMapping;

    if ( ! GetMaster (masterNodeName, errlist) )
        return true; // for safety - to not destroy the log assume master is alive

    if ( RTEHSS_KernelInterface::Instance().
         SendInfoStandby(masterNodeName, dummyOtherMapping, errlist) )
        return true;
    errlist.ClearMessageList(); // its not interesting
    return false;
}

/* --------------------------------------------------------------------------- */
bool Kernel_Administration::RestartPrepare (tgg00_TransContext   &trans,
                                            RestartKind           restartkind,
                                            SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestartPrepare", Common_Trace, 5);

    RTEHSS_KernelInterface &hss = RTEHSS_KernelInterface::Instance();


    if ( Kernel_State::standbyRoleStandby == m_State.GetHotStandbyRole()
         &&
         restartKindStandby != restartkind )
    {
        // PTS 1126502 UH 2003-12-17
        // This block is moved before RestartFilesystem() so that
        // the volumes are opened writable.
        // The Master State is set here.
        // The log info page is updated at the end of the RedoLog.

        // Check if restart is allowed for a standby
        // Is the given master alive ? If so we must restart.
        // The master is alive if a harmless command is answered.
        RTE_Nodename                    masterNodeName;
        RTEHSS_KernelInterface::Mapping dummyOtherMapping;

        if ( ! GetMaster (masterNodeName, errlist) )
            return false;
        if ( RTEHSS_KernelInterface::Instance().
             SendInfoStandby(masterNodeName, dummyOtherMapping, errlist) )
        {
            errlist = errlist + Admin_Exception(__CONTEXT__, KERNEL_RESTART_FOR_STANDBY_NOT_ALLOWED);
            return false;
        }

        // This standby node is allowed to restart normally and to become master
        // because the master does not answer.

        if ( ! RTEHSS_KernelInterface::Instance().SetRoleToMaster(errlist) )
            Offline(errlist);
        m_State.SetHotStandbyRole(Kernel_State::standbyRoleMaster);
    }

    if ( ! RestartFilesystem(trans) )
    {
        errlist = errlist + Admin_Exception(__CONTEXT__, KERNEL_RESTART_FILESYSTEM_FAILED,
                                            Kernel_BasisErrorToResultString(trans.trError_gg00));
        return false;
    }

    if ( ! CheckAndUpdateActivationState (trans, restartkind, errlist) )
        return false;

    b06create_errortext_file (trans);

    if ( trans.trError_gg00 != e_ok )
    {
        RTE_Message (Admin_Exception(__CONTEXT__,KERNEL_CREATE_ERROR_FILE_FAILED));
        return false;
    }

    PickUpAuxFilesForDestroy( trans ); // PTS 1126313 TS 2003-12-15

    k51restart_locklist(trans);

    if ( trans.trError_gg00 != e_ok )
    {
        errlist = errlist + Admin_Exception(__CONTEXT__,KERNEL_RESTART_LOCKLIST_FAILED,
                                  Kernel_BasisErrorToResultString(trans.trError_gg00));
        return false;
    }

    if ( restartKindStandby != restartkind )
    {
        // Check if restart is allowed for a standby
        // Is the given master alive ? If so we must not restart.
        if ( Kernel_State::standbyRoleStandby == m_State.GetHotStandbyRole()
             &&
             IsMasterAlive(errlist) ) // PTS 1127374 UH 2004-01-23
        {
            errlist = errlist + Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Restart","master is alive");
            return false;
        }
    }
    else
    {
        // Register this stand by and prevent the master from overwriting our savepoint offset
        // It may be that our savepoint offset is already overwritten since the establishment
        // of the mirror but this will be detected later. (LogDataIncompatible)
        SAPDB_UInt4 auxOffset;
        if ( ! hss.SendRegisterStandby( k57restartrec->rstLastSavept_kb00().svpStartOffset_kb00,
                                        auxOffset, errlist ) )
            return false;

        RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_STANDBY_REGISTER, SAPDB_ToString(auxOffset)));

        m_LastSyncOffset = auxOffset;

        // db_state is now changed from ADMIN to STANDBY
        if ( ! hss.SetKernelStateToStandby(errlist) )
            return false;

        eo67InsertStandbyEvent();        
    }

    if ( ! RestartLog (trans.trTaskId_gg00, restartkind, errlist) )
        return false;

    if ( ! CheckAndResetReadOnlyState(trans) )
    {
        errlist = errlist + Admin_Exception(__CONTEXT__, KERNEL_RESET_READONLY_FAILED,
                                  Kernel_BasisErrorToResultString(trans.trError_gg00));
        return false;
    }

    if ( restartKindActivation != restartkind )
    {
        tgg00_TransContext auxTrans;
        k53StartInternalSession (trans, auxTrans, false, false);
        if ( trans.trError_gg00 == e_ok )
        {
            a36PrepareCatalogForUserConnect (trans.trAcvPtr_gg00);
            if ( trans.trError_gg00 == e_ok )
                k53StopInternalSession (trans, auxTrans, m_commit);
            else
            {
                trans.trError_gg00 = e_ok;
                k53StopInternalSession (trans, auxTrans, m_rollback);
            }
        }
        else
            trans.trError_gg00 = e_ok;
    }

    return true;
}

/* --------------------------------------------------------------------------- */
Kernel_Administration::RestartInternResult
Kernel_Administration::RestartIntern (tsp00_TaskId            taskid,
                                      SAPDBMem_IRawAllocator &allocator,
                                      tgg00_TransContext     &oldcontext,
                                      RestartKind             restartkind,
                                      Kernel_Date             untildate,
                                      Kernel_Time             untiltime,
                                      Log_IOSequenceNo        untilIOSequence,
                                      SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestartIntern", Common_Trace, 5);

    if ( restartKindStandby != restartkind )
    {
        if ( ! RestartPrepare (oldcontext, restartkind, errlist) )
            return RestartInternPrepareFailed;
    }

    /* allow garbage collectors to act on "rollforward drop container" */
    bd91StartOMSGarbageCollectionAfterSpecialOp( taskid );

    if ( restartKindRestoreLog != restartkind )
    {
        Rst_RedoKind redokind;

        switch ( restartkind )
        {
            case restartKindNormal:
            case restartKindActivation:
                redokind = Rst_RedoKindNormal;
                break;
            case restartKindRestoreLog:
                redokind = Rst_RedoKindRestoreLog;
                break;
            case restartKindStandby:
                redokind = Rst_RedoKindStandby;
                break;
            default:
                redokind = Rst_RedoKindNormal;
                break;
        }

        if ( redokind != restartKindStandby )
            m_LastSyncOffset.Invalidate();

        const Kernel_IAdminBackupRecovery::RedoLogResult result =
                RedoLog (taskid, oldcontext, redokind, m_LastSyncOffset,
                         untildate, untiltime, untilIOSequence, errlist);

        if (  result != Kernel_IAdminBackupRecovery::RedoLogOk )
        {
            bd91StopOMSGarbageCollectionForSpecialOp( taskid );
            return RestartInternRedoLogFailed;
        }
    }

    // The persistent data is now transactional consistent

    /* activate complete functionality of garbage collectors */
    bd91EnableOMSHistoryRemoval(taskid);

    if ( ! Log_History::GetInstance().CheckAndStartMigrationIfNeeded(oldcontext) )
    {
        Kernel_AppendBasisError(oldcontext.trError_gg00, errlist, "CheckAndStartMigrationIfNeeded");
        bd91StopOMSGarbageCollectionForSpecialOp( taskid );
        return RestartInternRedoLogFailed;
    }


    if ( restartKindRestoreLog != restartkind  )
    {
        SrvTasks_JobCollection job;

        if ( job.ExecuteByServer(*RTETask_ITask::Self(),errlist) != SrvTasks_Job::executeOK )
        {
            RTE_Message(errlist);
            errlist.ClearMessageList();
        }
    }

    tsp00_ErrText      xp_errortext;
    tsp00_XpReturnCode xp_return;
    vxparam_save_good_config (restartKindActivation != restartkind, xp_errortext, xp_return);
    if ( xp_return != xp_ok )
        RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_SAVE_GOOD_FAILED,
                                    SAPDB_ToString(xp_return), xp_errortext));
    
    return RestartInternOk;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::Shutdown (tgg00_TransContext &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::Shutdown", Common_Trace, 5);

    if ( ! m_State.IsOnline()  )
    {
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    trans.trError_gg00 = e_ok;

    Trans_Context &context = Trans_Context::GetContext(trans);
    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait (context.Task(), Log_SVPReasonShutdown, context.ErrorList()) )
        Offline (context.ErrorList());
    // it should not be possible to reach this code
    Offline(e_shutdown);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::Offline ( const tgg00_BasisError error,
                                            bool             forceBacktrace,
                                            bool             writeCore )
{
    if ( 0 == error )
        Offline ((const char *)0, forceBacktrace, writeCore);
    else
    {
        SAPDBErr_MessageList errlist;
        Kernel_AppendBasisError(error,errlist,"Internal errorcode");

        Offline(errlist, forceBacktrace, writeCore);
    }
}

/* --------------------------------------------------------------------------- */
void Kernel_Administration::Offline ( const char * errorText,
                                            bool   forceBacktrace,
                                            bool   writeCore )
{
    SAPDBErr_MessageList errlist;
    if (errorText != 0)
    {
        errlist = errlist + Admin_Exception (__CONTEXT__, KERNEL_EMERGENCY_SHUTDOWN,
                                   "EMERGENCY SHUTDOWN",
                                   errorText);
    }
    Offline(errlist, forceBacktrace, writeCore);
}

/* --------------------------------------------------------------------------- */
void Kernel_Administration::Offline ( const SAPDBErr_MessageList &errlist,
                                            bool                  forceBacktrace,
                                            bool                  writeCore )
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::Offline", Common_Trace, 5);

    RTETask_ITask        &task            = *RTETask_ITask::Self();
    Msg_List             &globalErrorList = Kernel_Task::Instance().GetTransContext(task)->ErrorList();
    tak_all_command_glob *pAcv            = reinterpret_cast<tak_all_command_glob*>(Kernel_Task::Instance().GetSQLManContext(task));
    tgg00_BasisError     &trError         = pAcv->a_transinf.tri_trans.trError_gg00;
    
    globalErrorList.Overrule(errlist);

    const bool anyErrorOccured = ! globalErrorList.IsEmpty();

    globalErrorList.Overrule(Msg_List( anyErrorOccured ? Msg_List::Error : Msg_List::Info, SDBMSG_ADMIN_STATE_OFFLINE_REACHED));

    if ( writeCore )
        RTE_Crash(globalErrorList);

    if ( forceBacktrace || Common_Check.ChecksLevel(9) )
        vtracestack();

    if ( pAcv != 0 )
    {
        if ( e_ok == trError && anyErrorOccured) // PTS 1136948 mb 2005-07-28
            trError = e_system_error;
        a93reply (*pAcv, trError);
    }
    else
        RTE_Message(globalErrorList);

    voffline (task.ID(), ((forceBacktrace || anyErrorOccured) ? shtShutdKill_esp00 : shtShutdNormal_esp00));
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::RestartFilesystem (tgg00_TransContext &trans,
                                               const bool         incrementConverterVersion)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestartFilesystem", Common_Trace, 5);

    b01restart_filesystem (trans, incrementConverterVersion);
    if ( trans.trError_gg00 != e_ok )
        return false;

    if ( ! Catalog_Interface::GetInstance().RestartCatalog(
             trans,
             k57restartrec->rstSys1Catalog_kb00(),
             k57restartrec->rstSys2Catalog_kb00()) ) {
        return false;
    }

    return true;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::RestartFileSystemForSaveDataCold (const tsp00_TaskId taskId,
                                                         tgg00_BasisError   &error)
{
    const bool incrementConverterVersion = true;
    if(! IOMan_IDataManager::GetInstance().OpenAllDataVolumes( taskId ))
    {
        error = e_disk_not_accessible;
        return;
    }

    b10rc_restart_converter( taskId, error,
              k57restartrec->rstConverterVersion_kb00(),
              IOMan_IDataInfo::GetInstance().TotalUsableDataPages(),
              !incrementConverterVersion,
              k57restartrec->rstLastSaveDataSuccessful_kb00(),
              k57restartrec->rstConfigParam_kb00() );
    if (error != e_ok)
    {
        IOMan_IDataManager::GetInstance().CloseAllDataVolumes( taskId );
    }
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::CheckAndUpdateActivationState (tgg00_TransContext   &trans,
                                                           RestartKind           restartkind,
                                                           SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::CheckAndUpdateActivationState", Common_Trace, 5);

    SAPDBERR_ASSERT_STATE(m_State.IsRedoLogProcessing());

    const tkb00_ConfigPhase state = k57restartrec->rstConfigPhase_kb00();

    if ( restartKindActivation == restartkind )
    {
        if ( state != cfpConfigured_ekb00 )
        {
            errlist = Admin_Exception(__CONTEXT__,KERNEL_ALREADY_ACTIVATED);
            return false;
        }

        k57restartrec->rstConfigPhase_kb00().becomes(cfpNone_ekb00);
        FlushRestartRecord (trans.trTaskId_gg00);
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_SWITCHED_TO_ACTIVATED));
        return true;
    }

    if ( restartKindRestoreLog == restartkind
         &&
         state == cfpConfigured_ekb00 )
    {
        // restore log without initial data recovery
        k57restartrec->rstConfigPhase_kb00().becomes(cfpNone_ekb00);
        FlushRestartRecord (trans.trTaskId_gg00);
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_SWITCHED_TO_ACTIVATED2));
        // if the restore log fails
        // it must be called init_config again
        return true;
    }

    if ( state == cfpNone_ekb00 )
        return true; // normal restart

    if ( state == cfpConfigured_ekb00 )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_ACTIVATE_IS_MISSING);
        return false;
    }

    errlist = errlist + Admin_Exception( __CONTEXT__,
                                KERNEL_UNKNOWN_ACTIVATION_STATE,
                                SAPDB_ToString(state));
    return false;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::RestartLog (tsp00_TaskId          taskid,
                                        RestartKind           reason,
                                        SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestartLog", Common_Trace, 5);

    Log_Volume::RestartType logRestartType;

    switch ( reason )
    {
        case restartKindNormal:
        case restartKindActivation:
            logRestartType = Log_Volume::RestartNormal;
            break;
        case restartKindRestoreLog:
            logRestartType = Log_Volume::RestartForRestore;
            break;
        case restartKindStandby:
            logRestartType = Log_Volume::RestartForStandby;
            break;
        default:
            return false;
    }

    const Log_ErrorState logRestartResult = Log_Volume::Instance().Restart (taskid, logRestartType, errlist);
    if ( logRestartResult == Log_Ok )
        return true;

    Log_Volume::Instance().WriteDeviceParametersToTrace();
    errlist = errlist + Admin_Exception(__CONTEXT__, KERNEL_INIT_LOG_FAILED,
                              (restartKindRestoreLog==reason?"restore log":"restart"),
                              Log_ErrorStateStrings[logRestartResult]);
    return false;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::CheckAndResetReadOnlyState (tgg00_TransContext &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::CheckAndResetReadOnlyState", Common_Trace, 5);
    SAPDBERR_ASSERT_STATE(m_State.IsRedoLogProcessing());

    if ( k57restartrec->rstSetEndReadOnly_kb00() != 0 )
        b01end_read_only (trans);

    return trans.trError_gg00 == e_ok;
}
/* --------------------------------------------------------------------------- */
Kernel_Administration::RedoLogResult
Kernel_Administration::RedoLog (tsp00_TaskId          taskid,
                                tgg00_TransContext   &oldcontext,
                                Rst_RedoKind          redokind,
                                Log_RawDeviceOffset   lastRedoOffset,
                                Kernel_Date           untildate,
                                Kernel_Time           untiltime,
                                Log_IOSequenceNo      untilIOSequence,
                                SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RedoLog", Common_Trace, 5);

    if ( ! m_State.IsRedoLogProcessing()  )
        return RedoLogNotAllowed;

    tkb00_SaveptParam &svp = k57restartrec->rstLastSavept_kb00();

    if ( untildate.IsValid() && untiltime.IsValid()
         &&                                     // PTS 1120507 mb 2002-02-10
         Kernel_DateTime::OlderThan (untildate, untiltime,
                                     svp.svpStartEntryDate_kb00,
                                     svp.svpStartEntryTime_kb00) )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_UNTILDATE_TOO_OLD,
                                  SAPDB_ToString(svp.svpStartEntryDate_kb00),
                                  SAPDB_ToString(svp.svpStartEntryTime_kb00));
        return RedoLogUntilTimeInvalid; // old: e_incompatible_log
    }
    if ( untilIOSequence.IsValid()
         &&
         Log_IOSequenceNo::LeftIsOlder( untilIOSequence,                  // PTS 1124727 mb 2003-10-20
                                        svp.svpIOsequence_kb00,
                                        Log_Volume::Instance().GetOldestKnownIOSeq()))
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_UNTILIOSEQ_TOO_OLD,
                                  SAPDB_ToString(svp.svpIOsequence_kb00));
        return RedoLogUntilIOSequenceInvalid; // old: e_incompatible_log;
    }

    const Rst_RedoManager::RedoLogResult result =
        m_RedoManager.RedoLog ( taskid, oldcontext,
                                lastRedoOffset,
                                untildate, untiltime, untilIOSequence,
                                redokind, errlist );

    if  ( Rst_RedoManager::RedoLogOk != result )
        return Rst_RedoManager::RedoLogCancelled == result
               ? RedoLogRedoLogCancelled
               : RedoLogRedoLogFailed;

    if ( IsHotStandbyConfigured()
         &&
         Kernel_State::standbyRoleMaster != m_State.GetHotStandbyRole() )
    {
        // The node which can restart must be the master and so the hostname is written to the log info page.
        // detach and attach again the log volume
        Log_Volume::Instance().CloseLogArea(taskid);
        ForceHotStandbyRoleToMaster();
        if ( ! Log_Volume::Instance().OpenLogArea(taskid) )
            return RedoLogReOpenLogForWritingFailed;
    }

    RTE_Nodename masternodename;
    SAPDB_MemCopyNoCheck(masternodename,RTE_ISystem::Instance().GetLocalNodeName(),sizeof(masternodename));
    Log_Volume::Instance().SetMasterNodeName( taskid, masternodename);

    Trans_Context &context = Trans_Context::GetContext(oldcontext);
    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait (context.Task(), Log_SVPReasonRestart, context.ErrorList()) )
        Offline(context.ErrorList());

    RTE_Message (Admin_Exception(__CONTEXT__, KERNEL_REDO_LOG_FINISHED));

    return RedoLogOk;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::SetNewDBIdentifierIfHistoryLost (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::SetNewDBIdentifierIfHistoryLost", Common_Trace, 5);

    if ( Kernel_State::standbyRoleStandby == m_State.GetHotStandbyRole() )
        return; // This is not allowed for standbys

    Log_Volume &log = Log_Volume::Instance();

    if ( ! log.PrepareLogInfoPageForRead(taskid) )
        return; // Error on accessing the log are ignored.

    if ( (log.GetDeviceState() == Log_DeviceStateCleared
          ||
          log.GetDeviceState() == Log_DeviceStatePartiallyCleared
          ||
          log.GetDeviceState() == Log_DeviceStateHistoryLost)
         &&
         log.IsDevspaceEnabled(taskid) )
    {
        // a new backuphistory has began
        tsp00_Line      newDBIdent;
        vget_uniqe_id  (newDBIdent);
        tkb00_SaveptParam &SaveptParam = k57restartrec->rstLastSavept_kb00();

        k57restartrec->rstDbIdent_kb00() = newDBIdent;

        if ( Log_DeviceStateCleared == log.GetDeviceState()
             &&
             m_State.IsAdmin() )
        {
            // adjust the offset if the log is empty, because the
            // first entry is written onto offset 24 in page 1
            // PTS 1116123 mb 2002-06-12
            SaveptParam.svpStartEntryType_kb00   = Log_FinishOpenTrans;
            SaveptParam.svpStartEntryOffset_kb00 = Log_Page::MinSpaceOffset();
            // PTS 1130390 mb 2004-06-29
            SaveptParam.svpIOsequence_kb00       = LOG_INITIAL_IOSEQ.RawValue();
            SaveptParam.svpStartOffset_kb00      = log.LogicalLogBeginOffset().RawValue();
        }

        FlushRestartRecord (taskid);
        log.SetLogDBIdent (taskid, newDBIdent);

        SAPDB_Char newLogDBIdent[sizeof(tsp00_Line)+1];
        sql47_ptoc(newLogDBIdent, k57restartrec->rstDbIdent_kb00(), sizeof(tsp00_Line));
        RTE_Message(Log_Exception(__CONTEXT__, LOG_NEW_DBIDENT,newLogDBIdent));
    }
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::SetKernelStateOnline()
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::SetKernelStateOnline", Common_Trace, 5);
    m_State.SetOnline();
    RTE_Message (Admin_Exception(__CONTEXT__, KERNEL_STATE_IS_ONLINE));
}
/* --------------------------------------------------------------------------- */
Kernel_Administration::RestoreLogResult
Kernel_Administration::RestoreLog (tsp00_TaskId          taskid,
                                   tgg00_TransContext   &oldcontext,
                                   tgg00_MessBlock      &restoreDescription,
                                   SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestoreLog", Common_Trace, 5);

    if ( Kernel_State::standbyRoleStandby == m_State.GetHotStandbyRole() )
        return RestoreLogWrongRole;

    if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateBackup) )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "Backup");
        return RestoreLogNotAllowed;
    }

    if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateRedo) )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "Redo");
        return RestoreLogNotAllowed;
    }

    const bool onlyCheck = IsCheckBackupConfigured();

    if ( ! onlyCheck )
    {
        // Until dat and time is given to the backup/restore component
        // which calls RedoLog later.
        SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(oldcontext.trAllocator_gg00));
        if ( RestartInternOk != RestartIntern (taskid, allocator, oldcontext, restartKindRestoreLog,
                                               Kernel_Date(), Kernel_Time(), Log_IOSequenceNo(),
                                               errlist) )
            return RestoreLogRestartInternFailed;
    }

    k38init_restore (restoreDescription, onlyCheck);
    if ( oldcontext.trError_gg00 != e_ok )
    {
        Kernel_AppendBasisError(oldcontext.trError_gg00, errlist, "k38init_restore");
        if ( ! onlyCheck ) // PTS 1133118 mb 2004-12-23
        {        
            bd91StopOMSGarbageCollectionForSpecialOp( taskid );
        }
        return RestoreLogInitError;
    }

    k38headmaster (restoreDescription);

    RestoreLogResult result;

    switch ( oldcontext.trError_gg00 )
    {
        case e_ok :
            result = RestoreLogOk;
            break;
        case e_new_hostfile_required :
            if ( onlyCheck )
            {
                result = RestoreLogOk;
            }
            else
            {
                return RestoreLogNewMediumRequired;
            }
            break;
        case e_wrong_hostfile :
        case e_hostfile_error :
        case e_incompatible_log :
        case e_invalid_label :
        case e_save_skipped :
            result = RestoreLogError;
            break;
        default:
            if ( onlyCheck )
            {
                result = RestoreLogError;
            }
            else
            {
                result = RestoreLogFatalError;
            }
            break;
    }

    m_State.LeaveCriticalState(Kernel_State::criticalStateRedo);

    if ( result != RestoreLogOk )
    {
        if ( ! onlyCheck ) // PTS 1133118 mb 2004-12-23
        {        
            bd91StopOMSGarbageCollectionForSpecialOp( taskid );
        }
        Kernel_AppendBasisError(oldcontext.trError_gg00, errlist, "k38headmaster");
        tgg00_BasisError auxError = oldcontext.trError_gg00;
        oldcontext.trError_gg00   = e_ok;
        b01shutdown_filesystem(oldcontext);
        oldcontext.trError_gg00   = auxError;
    }

    m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
    return result;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::ClearLog (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::ClearLog", Common_Trace, 5);

    Kernel_StateScope stateBackup (Kernel_State::criticalStateBackup, m_State);
    Kernel_StateScope stateRedo   (Kernel_State::criticalStateRedo, m_State);
    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);

    if ( ! m_State.IsAdmin()
         ||
         ! stateBackup.EnterCriticalState()
         ||
         ! stateRedo.EnterCriticalState()
         ||
         ! stateConfig.EnterCriticalState() )
        Offline (e_dbm_command_not_possible);

   ClearLogArea( taskid );
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::ClearLogArea (tsp00_TaskId taskid)
{
    ReadRestartRecord (taskid);
    Msg_List errlist;
    Log_ErrorState result = Log_Volume::Instance().Restart (taskid, Log_Volume::RestartForFormat, errlist);

    if (result != Log_Ok)
    {
        Log_Volume::Instance().WriteDeviceParametersToTrace();
        RTE_Crash( errlist + Admin_Exception(__CONTEXT__, KERNEL_CLEAR_LOG_ERROR, Log_ErrorStateStrings[result]) );
    }

    Log_Volume::Instance().ClearLogComplete(taskid);

    Log_Volume::Instance().CloseLogArea(taskid);
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::DenyBackupAndRestore()
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::DenyBackupAndRestore", Common_Trace, 5);
    return m_State.EnterCriticalState(Kernel_State::criticalStateBackup);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::PermitBackupAndRestore()
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::PermitBackupAndRestore", Common_Trace, 5);
    m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::BackupData (tgg00_TransContext &trans,
                                        DataBackupType      backupType,
                                        tgg00_MessBlock    &backupDescription)
{
    
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::BackupData", Common_Trace, 5);

    Trans_Context    &context = Trans_Context::GetContext(trans);
    Msg_List         &errlist = context.ErrorList();
    tgg00_BasisError &trError = context.OldTrans().trError_gg00;

    if ( ! Log_Savepoint::Instance().IsSavepointAllowed() ) // PTS 1121281 UH 2003-03-24
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Data Backup","No savepoint allowed");
        trError = e_dbm_command_not_possible;
        return;
    }
    if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateBackup) )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Backup");
        trError = e_dbm_command_not_possible;
        return;
    }

    m_DataBackupIsRunning = true;
    trError = e_ok;
    
    if ( ! BackupDataIntern( context, backupType, backupDescription ) )
    {
        if ( e_ok == trError )
            trError = e_system_error;
        if( ! m_State.IsOnline() )
        {
            Converter_IManager::Instance().Shutdown( context.Task() );
            IOMan_IDataManager::GetInstance().CloseAllDataVolumes( context.TaskId());
        }
        errlist.Overrule( Msg_List( Msg_List::Error, SDBMSG_BACKUP_DATA_FAILED ));
        m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
        WakeupDropVolume();
        return;
    }

    if ( trError == e_new_hostfile_required )
        return; // do not change critical state; do not shutdown resources


    m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
    
    WakeupDropVolume();
}
/* --------------------------------------------------------------------------- */

bool Kernel_Administration::BackupDataIntern( Trans_Context      &context,
                                              DataBackupType      backupType,
                                              tgg00_MessBlock    &backupDescription )
{
    tgg00_BasisError &trError = context.OldTrans().trError_gg00;
    
    if ( m_State.IsOnline() )
    {
        vbegexcl (context.TaskId(), g08savepoint);
        k57restartrec->rstDataBackupCnt_kb00()++;
        FlushRestartRecord (context.TaskId());
        vendexcl (context.TaskId(), g08savepoint);
        
        if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait ( context.Task(),
                                                                                    backupComplete == backupType
                                                                                    ? Log_SVPReasonSaveData
                                                                                    : Log_SVPReasonSavePages,
                                                                                    context.ErrorList() ) )
        {
            k38history_error_write ( context.TaskId(),
                                     backupDescription.mb_qual()->msave_restore().sripUtilCmdId_gg00,
                                     backupDescription.mb_type(),
                                     backupDescription.mb_type2(),
                                     false,
                                     k57restartrec->rstDataBackupCnt_kb00(),
                                     e_start_server_task_error );

            Converter_IBackUp::Instance().EndSave (context.Task(), false);
            return false;
        }
    }
    else
    {
        RestartFileSystemForSaveDataCold (context.TaskId(), trError);
        if ( trError != e_ok )
            return false;

        if ( backupComplete == backupType )
            k57restartrec->rstLastSaveDataSuccessful_kb00() = false;

        k57restartrec->rstDataBackupCnt_kb00()++;
        FlushRestartRecord (context.TaskId());

        Converter_ReturnCode retCode;
        if ( backupIncrementell == backupType )
            retCode = Converter_IBackUp::Instance().BeginSavePages(context.Task(), context.ErrorList());
        else
            retCode =  Converter_IBackUp::Instance().BeginSaveData(context.Task(), context.ErrorList());
        
        if( Converter_Ok != retCode )
        {
            if( Converter_MemoryAllocationFailed == retCode )
                trError = e_no_more_memory;
            else
                trError = e_system_error;
            return false;
        }

        if ( backupIncrementell != backupType )
        {
            IOMan_PackedBlockAddress  packedRootBlockAddr;
            
            if (! Converter_IManager::Instance().FlushSerialForColdSave ( 
                context.Task(), packedRootBlockAddr))
            {
                trError = e_shutdown;
                return false;
            }
            k57restartrec->rstConfigParam_kb00().crConvRootBlockAddr_kb00 = packedRootBlockAddr;
            FlushRestartRecord (context.TaskId());
        }
    }

    k38init_save (backupDescription, m_State.IsAdmin(), false, false);
    
    if ( trError != e_ok )
    {
        if ( m_State.IsOnline() )
            Converter_IBackUp::Instance().EndSave (context.Task(), false);
        return false;
    }

    k38headmaster (backupDescription);
    
    return trError == e_ok || trError == e_new_hostfile_required;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::BackupLog (tgg00_TransContext &trans,
                                       tgg00_MessBlock    &backupDescription)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::BackupLog", Common_Trace, 5);

    if ( Kernel_State::standbyRoleStandby == m_State.GetHotStandbyRole() )
    {
        RTE_Message (Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Log recovery","StandByRole"));
        trans.trError_gg00 = e_hotstandby_wrong_role;
        return;
    }
    if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateBackup) )
    {
        RTE_Message (Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Backup"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    Msg_List                    &errlist            = Trans_Context::GetContext(trans).ErrorList();
    tgg00_SaveRestoreInputParam &backupParameter    = backupDescription.mb_qual()->msave_restore();
    tgg00_MessBufHeader          initial_mb_head    = backupDescription.mb_header();
    tgg00_SaveRestoreInputParam  inital_user_data   = backupParameter;
    bool                         firstTimeTapeCheck = true;
    bool                         completeLog        = backupParameter.sripFileVersion_gg00 < 0
                                                      &&
                                                      backupParameter.sripHostFiletypes_gg00[0] != vf_t_tape_norew;

    if ( m_State.IsOnline() )
    {
        if ( k38is_on_autosave (trans, false) )
        {
            trans.trError_gg00 = e_autosave_running;
            m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
            return;
        }
    }
    else
    {
        const Log_ErrorState  result = Log_Volume::Instance().RestartForSaveLogCold(trans.trTaskId_gg00, errlist);

        if ( result != Log_Ok )
        {
            Log_Volume::Instance().WriteDeviceParametersToTrace();
            RTE_Message( Admin_Exception(__CONTEXT__,KERNEL_INIT_LOG_FAILED,
                                         "BackupLog in admin mode",
                                          Log_ErrorStateStrings[result]) );
            Offline ();
        }
    }

    if ( completeLog )
    {
        k38init_save ( backupDescription,
                       m_State.IsAdmin(),
                       completeLog,
                       firstTimeTapeCheck );
        if ( e_ok == trans.trError_gg00 )
            k38headmaster (backupDescription);

        if ( m_State.IsAdmin() )
            Log_Volume::Instance().CloseLogArea(trans.trTaskId_gg00);

        m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
        return;
    }

    // backup each log segment separately

    bool                        anylogWasSaved = false;
    tgg00_MessBufHeader         lastResponse_mb_head;
    tgg00_SaveRestoreInputParam lastResponse_user_data;
    do
    {
        k38init_save ( backupDescription,
                       m_State.IsAdmin(),
                       completeLog,
                       firstTimeTapeCheck );
        if ( e_ok != trans.trError_gg00 )
            break;
        if ( e_ok != trans.trError_gg00 )
            break;
        k38headmaster (backupDescription);
        if ( e_ok != trans.trError_gg00 )
            break;
        anylogWasSaved = true;

        lastResponse_mb_head   = backupDescription.mb_header();
        lastResponse_user_data = backupDescription.mb_qual()->msave_restore();

        backupDescription.mb_header()                = initial_mb_head;
        backupDescription.mb_qual()->msave_restore() = inital_user_data;
        backupDescription.mb_qual_len()              = sizeof (inital_user_data);
    }
    while ( ! Log_Volume::Instance().GetLogSaveIter().IsLastSegmentToSave() );

    if ( e_ok == trans.trError_gg00
         ||
         e_no_log_to_save == trans.trError_gg00 )
    {
        if ( anylogWasSaved )
        {
            trans.trError_gg00                           = e_ok;
            backupDescription.mb_header()                = lastResponse_mb_head;
            backupDescription.mb_qual_len()              = sizeof (lastResponse_user_data);
            backupDescription.mb_qual()->msave_restore() = lastResponse_user_data;
        }
        else
        {
            backupDescription.mb_data_len() = 0;
            backupDescription.mb_struct().becomes(mbs_buf);
            backupDescription.mb_type().becomes  (m_return_result);
        }
    }
    else 
    // the current log backup is cancelled now
    {
        tgg00_BasisError foundError = trans.trError_gg00;
        
        backupDescription.mb_header()        = initial_mb_head;
        backupDescription.mb_qual_len()      = sizeof (inital_user_data);
        backupDescription.mb_type().becomes  (m_save_parallel);
        backupDescription.mb_type2().becomes (mm_abort);
        trans.trError_gg00                   = e_ok;
        k38headmaster (backupDescription);
        if (    ( e_new_hostfile_required == foundError )
             || ( e_wrong_hostfile        == foundError ))
        {
            trans.trError_gg00 = e_cancelled;
        }
        else
        {
            trans.trError_gg00 = foundError;
        }
    }

    if ( m_State.IsAdmin() )
        Log_Volume::Instance().CloseLogArea(trans.trTaskId_gg00);

    m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::RestoreData (tgg00_TransContext &trans,
                                         tgg00_MessBlock    &backupDescription)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestoreData", Common_Trace, 5);

    if ( m_State.IsOnline() )
    {
        RTE_Message (Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Data Recovery","Online"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateBackup) )
    {
        RTE_Message (Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Backup"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    k38init_restore (backupDescription, IsCheckBackupConfigured());
    if ( e_ok == trans.trError_gg00 )
        k38headmaster (backupDescription);

    if (    e_new_hostfile_required != trans.trError_gg00
         && e_wrong_hostfile        != trans.trError_gg00
         && e_invalid_label         != trans.trError_gg00 )
    {
        m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
        if( e_ok != trans.trError_gg00 )
        {
            Trans_Context&  context = Trans_Context::GetContext(trans);
            context.ErrorList().Overrule( Msg_List( Msg_List::Error, SDBMSG_RECOVERY_DATA_FAILED ));
        }
    }
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::BackupRestoreHandling (tgg00_TransContext &trans,
                                                   tgg00_MessBlock    &handlingDescription)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::BackupRestoreHandling", Common_Trace, 5);

    if ( handlingDescription.mb_type2() == mm_fread )
    {
        // only for reading label of backup medium
        if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateBackup) )
        {
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }
        k38headmaster (handlingDescription);
        m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
        return;
    }

    if ( k38GetTaskIdOfCurrentBackup(trans.trTaskId_gg00) != trans.trTaskId_gg00 ) // PTS 1134753 UH 2005-04-11
	{
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
	}
    
    if ( ! m_State.IsCriticalState(Kernel_State::criticalStateBackup) )
	{
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
	}
    
    const bool isAbortHandling = handlingDescription.mb_type2() == mm_abort; // PTS 1133694 UH 2005-02-01
    
    k38headmaster (handlingDescription);
    
    if ( trans.trError_gg00 == e_ok )
    {
        m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
        if ( m_State.IsCriticalState(Kernel_State::criticalStateRedo) )
            m_State.LeaveCriticalState(Kernel_State::criticalStateRedo);
    }
    else
    {
        if ( isAbortHandling )                  // PTS 1133694 UH 2005-02-01 added leaveCriticalState
            m_State.LeaveCriticalState(Kernel_State::criticalStateBackup);
            
        if ( m_State.IsCriticalState(Kernel_State::criticalStateRedo) )
        // in case of an error during recovery/redo the database has to be stopped
        {
            if (    trans.trError_gg00 != e_new_hostfile_required 
                 && trans.trError_gg00 != e_wrong_hostfile
                 && trans.trError_gg00 != e_hostfile_error // PTS 1124787 mb 2003-10-23
                 && trans.trError_gg00 != e_incompatible_log 
                 && trans.trError_gg00 != e_invalid_label 
                 && trans.trError_gg00 != e_save_skipped )
            {
                Offline (trans.trError_gg00);
            }
        }
    }
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::InitConfig (tgg00_TransContext &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::InitConfig", Common_Trace, 5);

    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    if ( m_State.IsOnline() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Reinstallation of database","Online"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if ( ! stateConfig.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if ( ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if ( IsHotStandbyConfigured() )
        ForceHotStandbyRoleToMaster();

    tsp00_Line  DBIdent;

    vget_uniqe_id (DBIdent);

    b01create_filesystem (trans);
    if ( trans.trError_gg00 != e_ok )
        Offline (trans.trError_gg00);

    k57create_restartrecord (trans);
    if ( trans.trError_gg00 != e_ok )
        Offline (trans.trError_gg00);

    if ( ! Catalog_Interface::GetInstance().CreateFile (trans) ) {
        Offline (trans.trError_gg00);
    }

    bd20FlushDataCache (trans);
    if ( trans.trError_gg00 != e_ok )
        Offline (trans.trError_gg00);

    FileDir_ISharedDirectory::Instance().Flush (trans.trTaskId_gg00);

    Converter_Version           converterVersion;
    Data_PageNo                 maxStaticPageNo;
    Data_PageNo                 maxDynamicPageNo;
    IOMan_PackedBlockAddress    packedRootBlockAddr;
    
    Trans_Context &context = Trans_Context::GetContext(trans);

    if( ! Converter_IManager::Instance().FlushSerial (context.Task(), converterVersion,
            maxStaticPageNo, maxDynamicPageNo, packedRootBlockAddr))
    {
        Offline (e_shutdown);
    }

    k57restartrec->rstConfigParam_kb00().crFdirRoot_kb00          = FileDir_ISharedDirectory::Instance().GetRootPageNo();
    k57restartrec->rstConfigParam_kb00().crLongDirRoot_kb00       = bd17GetBlobFdirRoot();
    k57restartrec->rstConfigParam_kb00().crFdirEntryCount_kb00    = FileDir_ISharedDirectory::Instance().GetEntryCount();
    k57restartrec->rstConfigParam_kb00().crMaxDynamicPno_kb00     = maxDynamicPageNo;
    k57restartrec->rstConfigParam_kb00().crMaxStaticPno_kb00      = maxStaticPageNo;
    k57restartrec->rstConfigParam_kb00().crConvRootBlockAddr_kb00 = packedRootBlockAddr;
    k57restartrec->rstConverterVersion_kb00()                     = converterVersion;
    k57restartrec->rstSys1Catalog_kb00()                          = Catalog_Interface::GetInstance().GetSys1CatalogTabId();
    k57restartrec->rstSys2Catalog_kb00()                          = Catalog_Interface::GetInstance().GetSys2CatalogTabId();

    SAPDB_MemCopyNoCheck(k57restartrec->rstDbIdent_kb00(), DBIdent, sizeof(DBIdent));

    Log_Volume &log = Log_Volume::Instance();
    const Log_ErrorState result = log.CreateForNewDatabase (trans.trTaskId_gg00);

    if ( result != Log_Ok )
    {
        log.WriteDeviceParametersToTrace();
        RTE_Message( Admin_Exception(__CONTEXT__,KERNEL_INIT_LOG_FAILED,
                     "CreateForNewDatabase", Log_ErrorStateStrings[result]) );
        Offline(true);
    }

    k57restartrec->rstConfigPhase_kb00().becomes(cfpConfigured_ekb00);
    FlushRestartRecord (trans.trTaskId_gg00);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::AddLogVolume (tgg00_TransContext &trans,
                                          tsp00_VFilename    &volumeName,
                                          tsp00_VFilename    &mirrorVolumeName,
                                          SAPDB_Int4          newVolumeSize,
                                          SAPDB_Int2          volumeId)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::AddLogVolume", Common_Trace, 5);

    Kernel_StateScope state (Kernel_State::criticalStateConfiguration, m_State);

    if ( ! state.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if ( ! memcmp (volumeName, b01blankfilename, sizeof(b01blankfilename))
         ||
         (IOMan_ILogInfo::GetInstance().IsLogMirrored()
          &&
          ! memcmp (mirrorVolumeName, b01blankfilename, sizeof(b01blankfilename))) )
    {
        trans.trError_gg00 = e_devname_invalid;
        return;
    }

    if ( newVolumeSize <= 0 )
    {
        trans.trError_gg00 = e_devsize_invalid;
        return;
    }

    FrameCtrl_IIOManager  &frameControl = FrameCtrl_IIOManager::GetInstance();
    Data_PageFrame      pageFrame = frameControl.NewIOManFrame(trans.trTaskId_gg00);
    Log_Page            page  (pageFrame, Log_QueueID(), Log_IOSequenceNo());
    IOMan_LogPages      pages (*(reinterpret_cast<SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00)),UTF8("AddLogVolume"));
    IOMan_BlockCount    newLogSize = 0;
    const IOMan_BlockCount    LogIOSize = IOMan_ILogInfo::GetInstance().GetLogIOBlockCount();

    if ( ! pages.Initialize(LogIOSize) )
    {
        frameControl.FreeIOManFrame (trans.trTaskId_gg00, pageFrame);
        trans.trError_gg00 = e_sysbuf_storage_exceeded;
        return;
    }

    for( SAPDB_Int i = 0; i < LogIOSize; ++i ){
        pages.Append( page );
    }

    if ( ! IOMan_ILogManager::GetInstance().AddLogVolume(
        trans.trTaskId_gg00, volumeName, mirrorVolumeName,
        newVolumeSize, volumeId, pages, newLogSize ) )
    {
        frameControl.FreeIOManFrame (trans.trTaskId_gg00, pageFrame);
        trans.trError_gg00 = e_new_disk_not_accessible;
        return;
    }

    frameControl.FreeIOManFrame (trans.trTaskId_gg00, pageFrame);
    if ( newLogSize <= 0 )
    {
        Kernel_VTrace() << "AddLogVolume: Invalid new logsize: " << newLogSize;
        RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                      "AddLogVolume: Invalid new logsize ") );
    }
    Log_Volume::Instance().SetNewDeviceEndOffset(newLogSize - 1);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::AddDataVolume (tgg00_TransContext &trans,
                                           tsp00_VFilename     &volumeName,
                                           SAPDB_Int4           newVolumeSize,
                                           SAPDB_Int2           volumeId)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::AddDataVolume", Common_Trace, 5);

    Kernel_StateScope state (Kernel_State::criticalStateAddDataVolume, m_State);

    if ( ! state.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if ( ! memcmp (volumeName, b01blankfilename, sizeof(b01blankfilename)))
    {
        trans.trError_gg00 = e_devname_invalid;
        return;
    }

    if ( newVolumeSize <= 0 )
    {
        trans.trError_gg00 = e_devsize_invalid;
        return;
    }

    if ( ! IOMan_IDataManager::GetInstance().AddDataVolume(
        trans.trTaskId_gg00, volumeName, newVolumeSize, volumeId ))
    {
        trans.trError_gg00 = e_new_disk_not_accessible;
        return;
    }

    {   // add volume to parameters, visible in ACTIVECONFIGURATION system view
        SAPDBErr_MessageList    msgList;
        SAPDB_Bool changeOnline = FALSE;
        SAPDB_Bool permanent = TRUE;
        char dataVolumeParam[] = "DATA_VOLUME_MODE_XXXX";
        tsp00_VFilenamec volName;
        volName.p2c(volumeName);

        sprintf(dataVolumeParam,"DATA_VOLUME_NAME_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->AddString((SAPDB_UTF8*)dataVolumeParam,
                                                     (RTEConf_Parameter::String) &volName[0],
                                                     changeOnline,
                                                     permanent,
                                                     msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }

        sprintf(dataVolumeParam,"DATA_VOLUME_SIZE_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->AddInteger((SAPDB_UTF8*)dataVolumeParam,
                                                      (RTEConf_Parameter::Integer) newVolumeSize,
                                                      changeOnline,
                                                      permanent,
                                                      msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }

        sprintf(dataVolumeParam,"DATA_VOLUME_TYPE_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->AddString((SAPDB_UTF8*)dataVolumeParam,
                                                      (RTEConf_Parameter::String) "F",
                                                      changeOnline,
                                                      permanent,
                                                      msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }

        // always mode normal
        sprintf(dataVolumeParam,"DATA_VOLUME_MODE_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->AddString((SAPDB_UTF8*)dataVolumeParam,
                                                      (RTEConf_Parameter::String) "NORMAL",
                                                      changeOnline,
                                                      permanent,
                                                      msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }
    }
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::DropDataVolume (tgg00_TransContext &trans,
                                           SAPDB_Int2           volumeId)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::DropDataVolume", Common_Trace, 5);

    Kernel_StateScope state (Kernel_State::criticalStateDropDataVolume, m_State);

    if ( ! state.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"DropDataVolume"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    } 

    Trans_Context&  context = Trans_Context::GetContext(trans);
    
    IOMan_ReturnCode ret = IOMan_IDataManager::GetInstance().DropDataVolume( 
                            context, context.ErrorList(), volumeId );
    if (IOMan_Canceled == ret)
    {
            trans.trError_gg00 = e_cancelled; 
            return;
    }
    if (IOMan_Okay != ret)
    {
        trans.trError_gg00 = e_system_error; // more details in msgList
        return;
    }

    if( m_State.IsCriticalState(Kernel_State::criticalStateBackup) && 
        m_DataBackupIsRunning )
    {  
        // wait for backup to finish
        tgg00_WaitContext   waitContext;
        
        m_WaitingForBackupFinished.InsertAsLast( context.Task().ID(), waitContext);
        vsuspend( context.Task().ID(), KERNEL_ADMIN_WAIT_FOR_BACKUP );
    }

    // call savepoint and remove volume
    ret = IOMan_IDataManager::GetInstance().RemoveDroppedDataVolume( 
        context.Task(), context.ErrorList(), volumeId );

    if (IOMan_Canceled == ret)
    {
        trans.trError_gg00 = e_cancelled; 
    }
    if (IOMan_Okay != ret)
    {
        trans.trError_gg00 = e_system_error; // more details in msgList
    }

    if (e_ok == trans.trError_gg00)
    {   // remove volume from parameters, visible in ACTIVECONFIGURATION system view
        SAPDBErr_MessageList    msgList;
        char dataVolumeParam[] = "DATA_VOLUME_MODE_XXXX";

        sprintf(dataVolumeParam,"DATA_VOLUME_MODE_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->Remove((SAPDB_UTF8*)dataVolumeParam, msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }

        sprintf(dataVolumeParam,"DATA_VOLUME_NAME_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->Remove((SAPDB_UTF8*)dataVolumeParam, msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }

        sprintf(dataVolumeParam,"DATA_VOLUME_SIZE_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->Remove((SAPDB_UTF8*)dataVolumeParam, msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }

        sprintf(dataVolumeParam,"DATA_VOLUME_TYPE_%04d",volumeId);
        if (!RTEConf_ParameterAccess::Instance()->Remove((SAPDB_UTF8*)dataVolumeParam, msgList))
        {
            RTE_Message(msgList);
            msgList.ClearMessageList();
        }
    }
}

/* --------------------------------------------------------------------------- */
void Kernel_Administration::WakeupDropVolume ()
{
    m_DataBackupIsRunning = false;
    m_WaitingForBackupFinished.ResumeAll();
}

/* --------------------------------------------------------------------------- */
void Kernel_Administration::GetRestartVersion (tgg00_TransContext &trans,
                                               Log_IOSequenceNo   &firstUsedIOsequence,
                                               Log_IOSequenceNo   &firstLogIOsequence,
                                               SAPDB_Char         *dbidentRestartrecord,
                                               SAPDB_UInt          dbidentRestartrecordSize,
                                               SAPDB_Char         *dbidentLogInfo,
                                               SAPDB_UInt          dbidentLogInfoSize,
                                               RTE_Nodename       &masterNodeName,
                                               bool               &logWritingEnabled,
                                               bool               &logAutoOverwrite,
                                               SAPDB_Int4         &queueCount,
                                               bool               &restartable,
                                               SAPDB_Int4         &converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::GetRestartVersion", Common_Trace, 5);

    if ( dbidentRestartrecordSize != dbidentLogInfoSize )
    {
        trans.trError_gg00 = e_not_implemented;
        return;
    }

    Msg_List         &errlist = Trans_Context::GetContext(trans).ErrorList();
    tsp00_TaskId     &taskid  = trans.trTaskId_gg00;
    tgg00_BasisError &trError = trans.trError_gg00;

    Kernel_StateScope stateBackup    (Kernel_State::criticalStateBackup, m_State);
    Kernel_StateScope stateRedo      (Kernel_State::criticalStateRedo, m_State);
    Kernel_StateScope stateConfig    (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    Log_Volume & log      = Log_Volume::Instance();
    bool volumesAreAttached  = m_State.IsOnline() || m_State.IsRedoLogProcessing();
    
    if ( ! volumesAreAttached )
    {
        if ( ! stateBackup.EnterCriticalState() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Backup"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }

        if ( ! stateRedo.EnterCriticalState() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Redo"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }

        if ( ! stateConfig.EnterCriticalState() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }

        if ( ! stateAddVolume.EnterCriticalState() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }

        if ( 0 == k57restartrec )
        {
            trError = e_no_more_memory;
            return;
        }

        if ( ! ReadRestartRecord (taskid) )
        {
            trError = e_disk_not_accessible;
            return;
        }

        stateAddVolume.LeaveCriticalState();
        stateConfig.LeaveCriticalState();
        stateRedo.LeaveCriticalState();
        stateBackup.LeaveCriticalState();
    }

    // determine first needed iosequence to restart and dbident of data area

    const SAPDB_Char * pDbIdent = reinterpret_cast<const SAPDB_Char*>(&(k57restartrec->rstDbIdent_kb00()));

    converterVersion    = k57restartrec->rstConverterVersion_kb00();
    firstUsedIOsequence = k57restartrec->rstLastSavept_kb00().svpIOsequence_kb00;
     SAPDB_RangeMove (__FILE__,  1,
          sizeof (tsp00_Line), dbidentRestartrecordSize,
          pDbIdent, 1,
          dbidentRestartrecord, 1,
          sizeof (tsp00_Line), trError);

    // determine all log parameters

    if ( ! volumesAreAttached )
    {
        if ( ! stateBackup.EnterCriticalState() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Backup"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }

        if ( ! stateConfig.EnterCriticalState() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }

        if ( ! stateAddVolume.EnterCriticalState() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }

        const Log_ErrorState  result = log.RestartForSaveLogCold(taskid, errlist);

        if ( result != Log_Ok )
        {
            log.WriteDeviceParametersToTrace();
            RTE_Message( Admin_Exception(__CONTEXT__,KERNEL_INIT_LOG_FAILED,
                         "GetRestartVersion", Log_ErrorStateStrings[result]) );
            log.CloseLogArea (taskid);
            trError = e_redo_log_read_error;
            return;
        }
    }

    log.GetRestartVersion ( taskid,
                            firstLogIOsequence,
                            dbidentLogInfo, dbidentLogInfoSize,
                            masterNodeName );
    logWritingEnabled = log.IsDevspaceEnabled(taskid);
    logAutoOverwrite  = log.IsAutomaticOverwrite(taskid);
    if (log.LogIsEmpty()) //rewritten with 1124724 mb 2003-11-13
    {
        restartable = true;
    }
    else
    {
        restartable = firstUsedIOsequence.IsInRange ( firstLogIOsequence,log.GetLastKnownIOSeq());
        // check also the db-identifier
        restartable = restartable && ! memcmp(dbidentRestartrecord, dbidentLogInfo, dbidentRestartrecordSize);
    }

    queueCount = log.GetQueueCountFromInfoPage(taskid);

    if ( ! volumesAreAttached )
        log.CloseLogArea(taskid);
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::ReadRestartRecord (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::ReadRestartRecord", Common_Trace, 5);

    if ( 0 == k57restartrec )
        return false;

    IOMan_IDataManager &ioManData = IOMan_IDataManager::GetInstance();

    const IOMan_BlockAddress    &restartInfoAddr = ioManData.GetRestartPageAddress(taskid);
    const IOMan_DataVolumeInfo  restartInfoVolume ( restartInfoAddr.GetDeviceNo() );
          Kernel_RestartPage    restartInfoPage   (k57restartrec);

	if ( restartInfoVolume.IsVolumeOnline() )
	{
		IOMan_IDataIO::GetInstance().ReadRestartPage(taskid, restartInfoPage);
		return true;
	}
	else
	{
		if ( ! ioManData.OpenOneDataVolume(taskid, restartInfoAddr.GetDeviceNo()) )
		{
			return false;
		}
		else
		{
			IOMan_IDataIO::GetInstance().ReadRestartPage(taskid, restartInfoPage);
			ioManData.CloseAllDataVolumes(taskid);
			return true;
		}
	}
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::FlushRestartRecord (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::FlushRestartRecord", Common_Trace, 5);

    const IOMan_BlockAddress    &restartInfoAddr = IOMan_IDataManager::GetInstance().GetRestartPageAddress(taskid);
    const IOMan_DataVolumeInfo  restartInfoVolume ( restartInfoAddr.GetDeviceNo());

    if ( restartInfoVolume.IsVolumeOnline() )
    {
        k57save_restartrecord( taskid );
        return true;
    }

    IOMan_IDataManager &ioManData = IOMan_IDataManager::GetInstance();

    if ( ! ioManData.OpenOneDataVolume( taskid, restartInfoAddr.GetDeviceNo()))
        return false;

    k57save_restartrecord(taskid);

    ioManData.CloseAllDataVolumes(taskid);
    return true;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::IsCheckBackupConfigured()
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::IsCheckBackupConfigured", Common_Trace, 5);

    tsp00_VFilename           devicename;
    const IOMan_DeviceNo  firstVolumeNo = IOMan_IDataManager::GetInstance().GetFirstConfiguredDataVolumeNo();

    IOMan_IDataInfo::GetInstance().GetDataVolumeName( firstVolumeNo, devicename );
    return ! memcmp (devicename,"CHECKDEV",8);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::GetNewCommandId( tsp00_TaskId     taskid,
                                             tgg00_UtilCmdId &id )
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::GetNewCommandId", Common_Trace, 5);

    SAPDB_UInt2 commandno;
    SAPDB_Int4  timestamp;

    vtime_in_seconds (timestamp);

    if ( g01code.kernel_swap != sw_normal )
        s20int4_to_buf_swap (timestamp, g01code.kernel_swap, &timestamp, 1, sw_normal);

    // ************** SYNCH ******************
    m_SpinLock.Lock();
    m_CommandNo = (m_CommandNo+1) % 999;
    commandno   = m_CommandNo;
    m_SpinLock.Unlock();
    // ************** END SYNCH **************

    SAPDB_Byte* auxint = reinterpret_cast<SAPDB_Byte*>(&timestamp);
    SAPDB_Char* auxstr = reinterpret_cast<SAPDB_Char*>(&id);

    SAPDB_sprintf(auxstr, sizeof(id.utidId_gg00), "%02X%02X%02X%02X", auxint[0], auxint[1], auxint[2], auxint[3]);

    auxstr += 8;
    auxint  = reinterpret_cast<SAPDB_Byte*>(&commandno);

    if ( g01code.kernel_swap == sw_normal )
        SAPDB_sprintf(auxstr, sizeof(id.utidId_gg00)-7, "%02X%02X", auxint[0], auxint[1]);
    else
        SAPDB_sprintf(auxstr, sizeof(id.utidId_gg00)-7, "%02X%02X", auxint[1], auxint[0]);
    id .utidLineNo_gg00 = 0;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::ReIntegrateBadLogVolume (tgg00_TransContext &trans,
                                                     tsp00_VFilename    &devicename)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::ReIntegrateBadLogVolume", Common_Trace, 5);

    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    if ( m_State.IsOnline() ) // PTS 1123452 UH 2003-08-06 removed "!"
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Reintegrate log volume","Online"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if ( ! stateConfig.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if ( ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if( ! IOMan_ILogManager::GetInstance().ReIntegrateBadLogVolume(trans.trTaskId_gg00,devicename) )
        trans.trError_gg00 = e_disk_not_accessible;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::GetBadLogVolume (tgg00_TransContext &trans,
                                             tsp00_VFilename    &devicename)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::GetBadLogVolume", Common_Trace, 5);

    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    if ( m_State.IsOnline() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Get bad log volume","Online"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if ( ! stateConfig.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if ( ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if( ! IOMan_ILogInfo::GetInstance().GetBadLogVolume(trans.trTaskId_gg00,devicename) )
        trans.trError_gg00 = e_row_not_found; // no bad volume found
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::SetLogWriting (tsp00_TaskId taskid,
                                           bool         on)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::SetLogWriting", Common_Trace, 5);

    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    if ( ! m_State.IsAdmin() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"SetLogWriting","not Admin"));
        Offline (e_dbm_command_not_possible, true);
    }
    if ( ! stateConfig.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        Offline (e_dbm_command_not_possible, true);
    }

    if ( ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        Offline (e_dbm_command_not_possible, true);
    }

    if ( ! Log_Volume::Instance().SetLogWriting (taskid, on) )
        Offline (e_disk_not_accessible, true);
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::IsLogWritingEnabled (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::IsLogWritingEnabled", Common_Trace, 5);
    return Log_Volume::Instance().IsDevspaceEnabled(taskid);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::SetLogAutoOverwrite (tsp00_TaskId taskid,
                                                 bool         on)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::SetLogAutoOverwrite", Common_Trace, 5);

    Kernel_StateScope state (Kernel_State::criticalStateConfiguration, m_State);

    if ( ! state.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        Offline (e_dbm_command_not_possible, true);
    }

    if ( ! Log_Volume::Instance().SetAutomaticOverwrite (taskid, on) )
        Offline (e_disk_not_accessible, true);
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::IsLogAutoOverwriteEnabled (tsp00_TaskId taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::IsLogAutoOverwriteEnabled", Common_Trace, 5);
    return Log_Volume::Instance().IsAutomaticOverwrite(taskid);
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::KernelStateIsRedoLogProcessing()
{
    return m_State.IsRedoLogProcessing();
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::KernelStateIsOnline()
{
    return m_State.IsOnline();
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::KernelStateIsAdmin()
{
    return m_State.IsAdmin();
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::GetBlockAddress (
                                             const RTETask_ITask         &task,
                                             const Data_PageNo           &pageno,
                                             const Data_PageRecoveryMode &recMode,
                                             IOMan_BlockAddress          &block )
{
    SAPDBTRACE_METHOD_DEBUG( "Kernel_Administration::GetBlockAddress", Common_Trace, 5 );

    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    if (m_State.IsAdmin() && ! stateConfig.EnterCriticalState() )
    {
        RTE_Message( Admin_Exception( __CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration" ));
        block = IOMan_BlockAddress();
        return;
    }

    if (m_State.IsAdmin() && ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message( Admin_Exception( __CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume" ));
        block = IOMan_BlockAddress();
        return;
    }

    tgg00_BasisError error = e_ok;
    block.Invalidate();

    if( m_State.IsAdmin() )
        RestartFileSystemForSaveDataCold (task.ID(), error);

    if( e_ok == error )
        Converter_IDiagnose::Instance().GetBlockAddress (task, pageno, recMode, block);

    if( m_State.IsAdmin() )
    {
        Converter_IManager::Instance().Shutdown( task );
        IOMan_IDataManager::GetInstance().CloseAllDataVolumes( task.ID() );
    }
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::SetNewBlockAddress (
                                                const RTETask_ITask         &task,
                                                const Data_PageNo           &pageno,
                                                const Data_PageRecoveryMode &recMode,
                                                const IOMan_BlockAddress    &block )
{
    SAPDBTRACE_METHOD_DEBUG( "Kernel_Administration::SetNewBlockAddress", Common_Trace, 5 );

    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    if ( m_State.IsAdmin() && ! stateConfig.EnterCriticalState() )
    {
        RTE_Message( Admin_Exception( __CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration" ));
        return false;
    }

    if (m_State.IsAdmin() && ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message( Admin_Exception( __CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume" ));
        return false;
    }

    tgg00_BasisError error = e_ok;
    bool             result;

    if( m_State.IsAdmin() )
        RestartFileSystemForSaveDataCold (task.ID(), error);

    if( e_ok == error )
        result = Converter_IDiagnose::Instance().SetNewBlockAddress( task, pageno, recMode, block );
    else
        result = false;

    if ( m_State.IsAdmin() )
    {
        if( result )
        {
            IOMan_PackedBlockAddress  packedRootBlockAddr;

            if( Converter_IManager::Instance().FlushSerialForColdSave( task, packedRootBlockAddr ))
            {
                k57restartrec->rstConfigParam_kb00().crConvRootBlockAddr_kb00 = packedRootBlockAddr;

                FlushRestartRecord (task.ID());
            }
        }
        Converter_IManager::Instance().Shutdown( task );
        IOMan_IDataManager::GetInstance().CloseAllDataVolumes( task.ID() );
    }

    return result;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::DeleteBlockAddress(
                                               const RTETask_ITask         &task,
                                               const Data_PageNo           &pageno,
                                               const Data_PageRecoveryMode &recMode )
{
    SAPDBTRACE_METHOD_DEBUG( "Kernel_Administration::DeleteBlockAddress", Common_Trace, 5 );

    Kernel_StateScope stateConfig (Kernel_State::criticalStateConfiguration, m_State);
    Kernel_StateScope stateAddVolume (Kernel_State::criticalStateAddDataVolume, m_State);

    if( m_State.IsAdmin() && ! stateConfig.EnterCriticalState() )
    {
        RTE_Message( Admin_Exception( __CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration" ));
        return false;
    }

    if (m_State.IsAdmin() && ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message( Admin_Exception( __CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume" ));
        return false;
    }

    tgg00_BasisError error = e_ok;
    bool             result;

    if( m_State.IsAdmin() )
        RestartFileSystemForSaveDataCold (task.ID(), error);

    if( e_ok == error )
        result = Converter_IDiagnose::Instance().DeleteBlockAddress( task, pageno, recMode );
    else
        result = false;

    if( m_State.IsAdmin() )
    {
        if( result )
        {
            IOMan_PackedBlockAddress  packedRootBlockAddr;

            if (Converter_IManager::Instance().FlushSerialForColdSave( task, packedRootBlockAddr ))
            {
                k57restartrec->rstConfigParam_kb00().crConvRootBlockAddr_kb00 = packedRootBlockAddr;

                FlushRestartRecord (task.ID());
            }
        }
        Converter_IManager::Instance().Shutdown( task );
        IOMan_IDataManager::GetInstance().CloseAllDataVolumes( task.ID() );
    }

    return result;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::SuspendLogWriter ( tsp00_TaskId      taskid,
                                               Log_IOSequenceNo &lastWrittenIOsequence)
{
    // PTS 1114053 UH 2002-10-22 new as kb560SuspendAndGetLastWrittenIOSequence
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::SuspendLogWriter", Common_Trace, 5);

    Kernel_StateScope state (Kernel_State::criticalStateConfiguration, m_State);

    lastWrittenIOsequence = Log_IOSequenceNo();

    if ( m_State.IsAdmin()  )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"SuspendLogWriter","Admin"));
        return false;
    }
    if ( ! Log_Volume::IsReadyForWriting()  )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"SuspendLogWriter","log volume is not initialized"));
        return false;
    }
    RTETask_ITask &task = *RTETask_ITask::ByTaskID(taskid);
    if ( ! Log_Savepoint::Instance().DenySavepoint (task) )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"SuspendLogWriter","DenySavepoint failed"));
        return false;
    }
    if ( ! state.EnterCriticalState() ) // PTS 1121281 UH 2003-03-24
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        return false;
    }
    return Log_Volume::Instance().SuspendAndGetLastWrittenIOSequence(taskid, lastWrittenIOsequence);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::ResumeLogWriter (tsp00_TaskId taskid)
{
    // PTS 1114053 UH 2002-10-22 new as k560ResumeLogWriterByUser
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::ResumeLogWriter", Common_Trace, 5);

    Kernel_StateScope state (Kernel_State::criticalStateConfiguration, m_State);

    if ( m_State.IsAdmin() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"ResumeLogWriter","Admin"));
        return;
    }
    if ( ! Log_Volume::IsReadyForWriting() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"ResumeLogWriter","Log volume not initialized"));
        return;
    }
    if ( ! state.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        return;
    }
    RTETask_ITask &task = *RTETask_ITask::ByTaskID(taskid);
    Log_Savepoint::Instance().PermitSavepoint (task); // PTS 1121281 UH 2003-03-24
    Log_Volume::Instance().ResumeByUser();
}

/* --------------------------------------------------------------------------- */

void Kernel_Administration::CheckData(
                                      tgg00_TransContext  &trans,
                                      const SAPDB_Bool    bWithExtendedCheck,
                                      const SAPDB_Bool    bWithIndexes,
                                      const SAPDB_Bool    bWithDeleteUnreferencedBlobs, // PTS 1133300 UH 2005-01-11
                                      SAPDB_Int4          &badIndexCount,
                                      SAPDB_Int4          &totalBadCount )
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::CheckData", Common_Trace, 5);

    Kernel_StateScope state( Kernel_State::criticalStateCheckData, m_State );
    
    if ( bWithDeleteUnreferencedBlobs ) // PTS 1133300 UH 2005-01-11
    {
        if( ! m_State.IsAdmin() )
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"CheckData","Admin"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }
    }
    else
    {
        if( ! ( m_State.IsOnline() || m_State.IsRedoLogProcessing() )) // PTS 1127542 2004-02-03
        {
            RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"CheckData","Redo and Online"));
            trans.trError_gg00 = e_dbm_command_not_possible;
            return;
        }
    }

    if ( ! state.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"CheckData"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    SAPDB_Int4       pagesUsed      = 0;
    SAPDB_Int4       blocksReleased = 0;
    const SAPDB_Bool bWithUpdate    = false;

    b01verify_database( trans, bWithUpdate, bWithIndexes, bWithExtendedCheck, bWithDeleteUnreferencedBlobs, // PTS 1133300 UH 2005-01-11
        pagesUsed, blocksReleased, badIndexCount, totalBadCount );
}

/* --------------------------------------------------------------------------- */

void Kernel_Administration::CheckDataWithUpdate(
                                                tgg00_TransContext  &trans,
                                                const SAPDB_Bool    bWithExtendedCheck,
                                                SAPDB_Int4          &pagesUsed,
                                                SAPDB_Int4          &blocksReleased,
                                                SAPDB_Int4          &badIndexCount,
                                                SAPDB_Int4          &totalBadCount )
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::CheckDataWithUpdate", Common_Trace, 5);

    Kernel_StateScope stateBackup( Kernel_State::criticalStateBackup, m_State );
    Kernel_StateScope stateRedo( Kernel_State::criticalStateRedo, m_State );
    Kernel_StateScope stateConfig( Kernel_State::criticalStateConfiguration, m_State );
    Kernel_StateScope stateAddVolume( Kernel_State::criticalStateAddDataVolume, m_State );
    Kernel_StateScope stateCheckData( Kernel_State::criticalStateCheckData, m_State );


    // Note that "check data with update" and "save data/pages" are using the same
    // backup pno list within the converter, therefore no concurrent access is
    // possible. Configuration latch is used to hold the data area respl. converter
    // entries stable.
    // Redo state is set to guarantee that no restart is possible as long as check
    // data with update is executed and therefore no filesystem restart is made

    if( ! m_State.IsAdmin() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"CheckDataWithUpdate","not Admin"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if( ! stateBackup.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Backup"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if( ! stateRedo.EnterCriticalState() ) // PTS 1126032 TS 2003-08-12
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Redo"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if( ! stateConfig.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if( ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }
    if( ! stateCheckData.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"CheckData"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    // Do not increment converter version, because no savepoint will be generated!

    if( ! RestartFilesystem(trans, false) )
        Offline(trans.trError_gg00);

    const SAPDB_Bool bWithIndexes = true; // Check all indexes
    const SAPDB_Bool bWithUpdate  = true; // Free unreferenced blocks
    const SAPDB_Bool bWithDeleteUnreferencedBlobs = false; // PTS 1133300 UH 2005-01-11
    
    b01verify_database( trans, bWithUpdate, bWithIndexes, bWithExtendedCheck, bWithDeleteUnreferencedBlobs, // PTS 1133300 UH 2005-01-11
        pagesUsed, blocksReleased, badIndexCount, totalBadCount );

    const tgg00_BasisError  auxError = trans.trError_gg00;
    b01shutdown_filesystem( trans );
    trans.trError_gg00 = auxError;
}

/* --------------------------------------------------------------------------- */

void Kernel_Administration::Diagnose( tgg00_MessBlock &messBlock )
{
    SAPDBTRACE_METHOD_DEBUG( "Kernel_Administration::Diagnose", Common_Trace, 5 );

    const SAPDB_Int4        pageSize   = FrameCtrl_ICommon::GetInstance().GetPageSize();
    const tgg00_DiagType    &diagType  = messBlock.mb_qual()->mut_diag_type();
    const bool              bIsRestore = ( mm_device_restore == messBlock.mb_type2());

    tgg00_TransContext      &trans   = *(messBlock.mb_trns());
    tgg00_BasisError        &trError = trans.trError_gg00;

    // Check preconditions for diagnose repair

    Kernel_StateScope stateBackup   ( Kernel_State::criticalStateBackup, m_State );
    Kernel_StateScope stateRedo     ( Kernel_State::criticalStateRedo, m_State );
    Kernel_StateScope stateConfig   ( Kernel_State::criticalStateConfiguration, m_State );
    Kernel_StateScope stateAddVolume( Kernel_State::criticalStateAddDataVolume, m_State );
    Kernel_StateScope stateCheckData( Kernel_State::criticalStateCheckData, m_State );

    if( bIsRestore )
    {
        if(  ! m_State.IsAdmin())
        {
            RTE_Message( Admin_Exception( __CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,
                "Diagnose repair", "state is not Admin" ));
            trError = e_dbm_command_not_possible;
            return;
        }
        if( ! stateBackup.EnterCriticalState())
        {
            RTE_Message( Admin_Exception( __CONTEXT__, KERNEL_CRITICAL_STATE_NOT_ENTERED, "Backup" ));
            trError = e_dbm_command_not_possible;
            return;
        }
        if( ! stateRedo.EnterCriticalState())
        {
            RTE_Message( Admin_Exception( __CONTEXT__, KERNEL_CRITICAL_STATE_NOT_ENTERED, "Redo" ));
            trError = e_dbm_command_not_possible;
            return;
        }
        if( ! stateConfig.EnterCriticalState())
        {
            RTE_Message( Admin_Exception( __CONTEXT__, KERNEL_CRITICAL_STATE_NOT_ENTERED, "Configuration" ));
            trError = e_dbm_command_not_possible;
            return;
        }
        if( ! stateAddVolume.EnterCriticalState())
        {
            RTE_Message( Admin_Exception( __CONTEXT__, KERNEL_CRITICAL_STATE_NOT_ENTERED, "AddDataVolume" ));
            trError = e_dbm_command_not_possible;
            return;
        }
        if( ! stateCheckData.EnterCriticalState())
        {
            RTE_Message( Admin_Exception( __CONTEXT__, KERNEL_CRITICAL_STATE_NOT_ENTERED, "CheckData" ));
            trError = e_dbm_command_not_possible;
            return;
        }
    }

    if( diagNil_egg00 == diagType ){
        trError = e_not_implemented;
        return;
    }
    if( messBlock.mb_data_size() < pageSize ){
        trError = e_too_short_datapart;
        return;
    }
    if(( bIsRestore ) && ( pageSize != messBlock.mb_data_len()))
    {
        trError = e_too_short_datapart;
        return;
    }
    tsp00_PageAddr  pPage = reinterpret_cast< tsp00_PageAddr >
        ( RTE_ISystem::Instance().AllocSystemPages( pageSize ));

    if( NULL == pPage ){
        trError = e_no_more_memory;
        return;
    }
    if( bIsRestore )
    {  // copy page to be restored into i/o aligned buffer
        memcpy( pPage, messBlock.mb_data()->mbp_buf(), pageSize );
    }

    bool bPageWasExtracted    = false;
    bool bShutdownFileSystem  = false;
    bool bCloseVolumes        = false;

    if(
        ( diagDev_egg00         != diagType ) &&
        ( diagLoginfoPage_egg00 != diagType ) &&
        ( diagLogDev_egg00      != diagType ) &&
        ( diagMirrLogDev_egg00  != diagType ) &&
        ( diagRestart_egg00     != diagType ) &&
        ( b01downfilesystem ) // +++ get file system state
        )
    {
        RestartFilesystem( trans, false ); // do not increment converter version
        bShutdownFileSystem = true;
    }

    if( e_ok == trError )
    {
        if( ! DiagnoseIntern( trans, messBlock, pPage,
            diagType, bIsRestore, bPageWasExtracted, bCloseVolumes ))
        {
            if( e_ok == trError )
                trError = e_disk_not_accessible;
        }
    }

    if( NULL != pPage ){
        RTE_ISystem::Instance().FreeSystemPages( pPage, pageSize );
    }

    if( bShutdownFileSystem )
    {
        const tgg00_BasisError  auxError = trans.trError_gg00;
        b01shutdown_filesystem( trans );
        trans.trError_gg00 = auxError;
    }
    else if( bCloseVolumes )
    {
        IOMan_IDataManager::GetInstance().CloseAllDataVolumes( trans.trTaskId_gg00 );
        Log_Volume::Instance().CloseLogArea( trans.trTaskId_gg00 );
    }

    if(( e_ok == trError ) && bPageWasExtracted )
    {
        messBlock.mb_type().becomes( m_return_result );
        messBlock.mb_type2().becomes( mm_nil );
        messBlock.mb_struct().becomes( mbs_buf );
    }
}

/* --------------------------------------------------------------------------- */

bool Kernel_Administration::DiagnoseIntern(
                                           tgg00_TransContext     &trans,
                                           tgg00_MessBlock        &messBlock,
                                           tsp00_PageAddr         pPage,
                                           const tgg00_DiagType   diagType,
                                           const bool             bIsRestore,
                                           bool                   &bPageWasExtracted,
                                           bool                   &bCloseVolumes )
{
    const RTETask_ITask &task           = *RTETask_ITask::ByTaskID( trans.trTaskId_gg00 );
    const SAPDB_Int4    pageSize        = FrameCtrl_ICommon::GetInstance().GetPageSize();
    const bool          bFromPrimaryLog = true;
    const bool          bToPrimaryLog   = true;

    tgg00_BasisError    &trError = trans.trError_gg00;


    switch( diagType )
    {
    case diagDev_egg00:
        {
            bool                bIsPrimaryLog;
            IOMan_DeviceNo      volumeNo; // determine with volume name
            const IOMan_BlockNo blockNo     = messBlock.mb_qual()->mut_pno();
            const IOMan_BlockNo stopBockNo  = messBlock.mb_qual()->mut_pno2();

            if( blockNo != stopBockNo )
            {
                trError = e_not_implemented;
                return false;
            }
            if( IOMan_IDataInfo::GetInstance().GetDataVolumeNo( // is data page !
                messBlock.mb_qual()->mut_dev(), volumeNo ))
            {
                const IOMan_BlockAddress    dataBlock( volumeNo, blockNo );
                const IOMan_DataVolumeInfo  dataVolumeInfo( dataBlock.GetDeviceNo());

                bCloseVolumes = ! dataVolumeInfo.IsVolumeOnline();

                if( ! IOMan_IDataManager::GetInstance().OpenOneDataVolume(
                    task.ID(), dataBlock.GetDeviceNo())){
                    return false;
                }
                if( bIsRestore )
                {
                    return( IOMan_IDiagnose::GetInstance().WritePageDirectToDataVolume(
                        task.ID(), dataBlock, pPage ));
                }
                // extract data page
                if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromDataVolume(
                    task.ID(), dataBlock, pPage )){
                    return false;
                } // else copy data page into order packet
            }
            else if( IOMan_ILogInfo::GetInstance().GetLogVolumeNo(  // is log page
                messBlock.mb_qual()->mut_dev(), volumeNo, bIsPrimaryLog ))
            {
                const IOMan_BlockAddress    logBlock( volumeNo, blockNo );
                const IOMan_LogVolumeInfo   logVolumeInfo( logBlock.GetDeviceNo());

                bCloseVolumes = ! logVolumeInfo.IsVolumeOnline();

                if( ! IOMan_ILogManager::GetInstance().OpenOneLogVolume(
                    task.ID(), logBlock.GetDeviceNo())){
                    return false;
                }
                if( bIsRestore )
                {
                    if( bIsPrimaryLog ){
                        return( IOMan_IDiagnose::GetInstance().WritePageDirectToLogVolume(
                            task.ID(), logBlock, pPage, bToPrimaryLog ));
                    }
                    else{
                        return( IOMan_IDiagnose::GetInstance().WritePageDirectToLogVolume(
                            task.ID(), logBlock, pPage, ! bToPrimaryLog ));
                    }
                }
                // extract log page
                if( bIsPrimaryLog ){
                    if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromLogVolume(
                        task.ID(), logBlock, pPage, bFromPrimaryLog )){
                        return false;
                    }
                }
                else{
                    if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromLogVolume(
                        task.ID(), logBlock, pPage, ! bFromPrimaryLog )){
                        return false;
                    }
                } // else copy log page into order packet
            }
            else // unknown volume
            {
                trError = e_devname_invalid;
                return false;
            }
            memcpy( messBlock.mb_data()->mbp_buf(), pPage, pageSize );
            messBlock.mb_data_len() = pageSize;
            bPageWasExtracted       = true;
            return true;
        };
    case diagLogDev_egg00:
    case diagMirrLogDev_egg00:
        {
            if( messBlock.mb_qual()->mut_pno() != messBlock.mb_qual()->mut_pno2() )
            {
                trError = e_not_implemented;
                return false;
            }
            const IOMan_BlockAddress    logBlock( messBlock.mb_qual()->mut_count(), messBlock.mb_qual()->mut_pno() );
            const IOMan_LogVolumeInfo   logVolumeInfo( logBlock.GetDeviceNo());

            bCloseVolumes = ! logVolumeInfo.IsVolumeOnline();

            if( ! IOMan_ILogManager::GetInstance().OpenOneLogVolume(
                task.ID(), logBlock.GetDeviceNo())){
                return false;
            }
            if( bIsRestore )
            {
                if( diagLogDev_egg00 == messBlock.mb_qual()->mut_diag_type()){
                    return( IOMan_IDiagnose::GetInstance().WritePageDirectToLogVolume(
                        task.ID(), logBlock, pPage, bToPrimaryLog ));
                }
                else{
                    return( IOMan_IDiagnose::GetInstance().WritePageDirectToLogVolume(
                        task.ID(), logBlock, pPage, ! bToPrimaryLog ));
                }
            }
            // extract log page
            if( diagLogDev_egg00 == messBlock.mb_qual()->mut_diag_type())
            {
                if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromLogVolume(
                    task.ID(), logBlock, pPage, bFromPrimaryLog )){
                    return false;
                }
            }
            else
            {
                if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromLogVolume(
                    task.ID(), logBlock, pPage, ! bFromPrimaryLog )){
                    return false;
                }
            }
            memcpy( messBlock.mb_data()->mbp_buf(), pPage, pageSize );
            messBlock.mb_data_len() = pageSize;
            bPageWasExtracted       = true;
            return true;
        };
    case diagLoginfoPage_egg00:
        {
            const IOMan_BlockAddress    logInfoBlock( 1,1 ); // log info block
            const IOMan_LogVolumeInfo   logVolumeInfo( logInfoBlock.GetDeviceNo());

            bCloseVolumes = ! logVolumeInfo.IsVolumeOnline();

            if( ! IOMan_ILogManager::GetInstance().OpenOneLogVolume(
                task.ID(), logInfoBlock.GetDeviceNo())){
                return false;
            }
            if( bIsRestore )
            {
                if( ! IOMan_IDiagnose::GetInstance().WritePageDirectToLogVolume(
                    task.ID(), logInfoBlock, pPage, bToPrimaryLog )){
                    return false;
                }
                if( ! IOMan_ILogInfo::GetInstance().IsLogMirrored()){
                    return true; // ready
                }
                if( ! IOMan_IDiagnose::GetInstance().WritePageDirectToLogVolume(
                    task.ID(), logInfoBlock, pPage, ! bToPrimaryLog )){
                    return false;
                }
                return true; // ready
            }
            // extract log info page
            if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromLogVolume(
                task.ID(), logInfoBlock, pPage, bFromPrimaryLog )){
                return false;
            }
            memcpy( messBlock.mb_data()->mbp_buf(), pPage, pageSize );
            messBlock.mb_data_len() = pageSize;
            bPageWasExtracted       = true;
            return true;
        };
    case diagRestart_egg00:
        {
            // get restart record block
            const IOMan_BlockAddress    &rstBlock =
                IOMan_IDataManager::GetInstance().GetRestartPageAddress(trans.trTaskId_gg00);
            const IOMan_DataVolumeInfo  dataVolumeInfo( rstBlock.GetDeviceNo());

            bCloseVolumes = ! dataVolumeInfo.IsVolumeOnline();

            if( ! IOMan_IDataManager::GetInstance().OpenOneDataVolume(
                task.ID(), rstBlock.GetDeviceNo())){
                return false;
            }
            if( bIsRestore )
            {
                if( IOMan_IDiagnose::GetInstance().WritePageDirectToDataVolume(
                    task.ID(), rstBlock, pPage )){
                    return true;
                }
                return false;
            }
            // extract restart record
            if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromDataVolume(
                task.ID(), rstBlock, pPage )){
                return false;
            }
            memcpy( messBlock.mb_data()->mbp_buf(), pPage, pageSize );
            messBlock.mb_data_len() = pageSize;
            bPageWasExtracted       = true;
            return true;
        };
    case diagPermPages_egg00:
    case diagStaticPages_egg00:
        {
            const bool                 bIsPermanent = ( diagPermPages_egg00 == diagType );
            const Data_AddressingMode  addrMode     = ( bIsPermanent ? Data_Dynamic : Data_Static );
            IOMan_BlockAddress         block;
            tsp00_PageNo               pageNo;

            if( bIsRestore )
                memcpy( &pageNo, pPage, sizeof( tsp00_PageNo ));
            else
                pageNo = messBlock.mb_qual()->mut_pno();

            if( ! Converter_IDiagnose::Instance().GetBlockAddress(
                task, pageNo, addrMode, block ))
            {
                trError = e_no_converter_entry;
                return false;
            }
            if( bIsRestore )
            {
                if( ! IOMan_IDiagnose::GetInstance().WritePageDirectToDataVolume(
                    task.ID(), block, pPage )){
                    return false;
                }
                return true;
            }
            // extract data page
            if( ! IOMan_IDiagnose::GetInstance().ReadPageDirectFromDataVolume(
                task.ID(), block, pPage )){
                return false;
            }
            memcpy( messBlock.mb_data()->mbp_buf(), pPage, pageSize );
            messBlock.mb_data_len() = pageSize;
            bPageWasExtracted       = true;
            return true;
        }
    case diagFDir1_egg00:
        {
            FileDir_ISharedDirectory::Instance().Extract(
                task.ID(), messBlock.mb_qual()->mut_hostfn());
        }
        break;
    case diagFDir2_egg00:
        {
            const tsp00_PageNo fdir2Root = bd17GetBlobFdirRoot();
            bd01ExtractTable( trans, fdir2Root, messBlock.mb_qual()->mut_hostfn());
        }
        break;
    case diagFile_egg00:
        {
            tsp00_PageNo tableRoot = messBlock.mb_qual()->mut_pno();
            bd01ExtractTable( trans, tableRoot, messBlock.mb_qual()->mut_hostfn());
        }
        break;
    case diagInvRoot_egg00:
        {
            const tsp00_PageNo indexRoot = messBlock.mb_qual()->mut_pno();
            bd03ExtractIndex( trans, indexRoot, messBlock.mb_qual()->mut_hostfn());
        }
        break;
    case diagColRoot_egg00:
        {
            tsp00_PageNo blobRoot = messBlock.mb_qual()->mut_pno();
            bd05ExtractLob( trans, blobRoot, messBlock.mb_qual()->mut_hostfn());
        }
        break;
    default:
        trError = e_not_implemented;
    }
    return( e_ok == trError );
}

/* --------------------------------------------------------------------------- */

void Kernel_Administration::CreateSnapShot( tgg00_TransContext &trans )
{
    SAPDBTRACE_METHOD_DEBUG( "Kernel_Administration::CreateSnapShot", Common_Trace, 5 );

    Kernel_StateScope stateConfig( Kernel_State::criticalStateConfiguration, m_State );
    Kernel_StateScope stateAddVolume( Kernel_State::criticalStateAddDataVolume, m_State );

    tsp00_TaskId        &taskId  = trans.trTaskId_gg00;
    tgg00_BasisError    &trError = trans.trError_gg00;

    if ( ! stateConfig.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trError = e_dbm_command_not_possible;
        return;
    }

    if ( ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trError = e_dbm_command_not_possible;
        return;
    }

    if( ! DropSnapShot( trans.trTaskId_gg00 )){
        trError = e_disk_not_accessible;
        return;
    }

    // The filesysten is started, because an unsused position for the
    // snapShot restart page is needed and therefore the converter
    // has to be started.

    if( ! RestartFilesystem(trans, false) )
    {
        Offline( trError );
    }

    SAPDBERR_ASSERT_STATE( NULL != k57restartrec );

    IOMan_BlockAddress  restartPageAddr;
    Kernel_RestartPage  restartPage( k57restartrec );

    k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00  = 0;

    IOMan_IDataIO::GetInstance().WriteSnapShotRestartPage( taskId, restartPage, restartPageAddr );

    const SAPDB_Int                 volNoBitCount   = Converter_ICommon::Instance().GetVolumeNoBitCount();
    const IOMan_PackedBlockAddress  packedBlockAddr = restartPageAddr.Pack( volNoBitCount );

    k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 = packedBlockAddr;

    IOMan_IDataIO::GetInstance().WriteRestartPage( taskId, restartPage );

    b01shutdown_filesystem( trans );
}

/* --------------------------------------------------------------------------- */

void Kernel_Administration::ActivateSnapShot( tgg00_TransContext &trans )
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::ActivateSnapShot", Common_Trace, 5);

    tsp00_TaskId        &taskId  = trans.trTaskId_gg00;
    tgg00_BasisError    &trError = trans.trError_gg00;

    Kernel_StateScope stateBackup   ( Kernel_State::criticalStateBackup, m_State );
    Kernel_StateScope stateRedo     ( Kernel_State::criticalStateRedo, m_State );
    Kernel_StateScope stateConfig   ( Kernel_State::criticalStateConfiguration, m_State );
    Kernel_StateScope stateAddVolume( Kernel_State::criticalStateAddDataVolume, m_State );

    if( ! stateBackup.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Backup"));
        trError = e_dbm_command_not_possible;
        return;
    }
    if( ! stateRedo.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Redo"));
        trError = e_dbm_command_not_possible;
        return;
    }
    if( ! stateConfig.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trError = e_dbm_command_not_possible;
        return;
    }
    if( ! stateAddVolume.EnterCriticalState() )
    {
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trError = e_dbm_command_not_possible;
        return;
    }
    if( ! ReadRestartRecord(taskId) )
    {
        trError = e_disk_not_accessible;
        return;
    }

    if( 0 == k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 ){
        trError = e_no_snapshot_available;
        return;
    }

    {
        const IOMan_PackedBlockAddress  packedBlockAddr(
            k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 );
        const SAPDB_Int           volNoBitCount   = Converter_ICommon::Instance().GetVolumeNoBitCount();
        const IOMan_BlockAddress  restartPageAddr = packedBlockAddr.Unpack( volNoBitCount );

        IOMan_IDataManager &ioManData = IOMan_IDataManager::GetInstance();

        if ( ! ioManData.OpenOneDataVolume( taskId, restartPageAddr.GetDeviceNo()) ){
            trError = e_disk_not_accessible;
            return;
        }

        // read frozen info page into the memory of the old restart info page
        Kernel_RestartPage  snapShotRestartPage( k57restartrec );

        IOMan_IDataIO::GetInstance().ReadSnapShotRestartPage( taskId,
            snapShotRestartPage, restartPageAddr );

        // write snapshot block address into the restart page, to
        // guarantee that the snapshot is still available.

        k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 = packedBlockAddr;

        ioManData.CloseAllDataVolumes( taskId );
    }

    if( ! FlushRestartRecord( taskId )){
        trError = e_disk_not_accessible;
        return;
    }

    ClearLogArea( taskId );
}

/* --------------------------------------------------------------------------- */

void Kernel_Administration::DropSnapShot( tgg00_TransContext &trans )
{
    SAPDBTRACE_METHOD_DEBUG( "Kernel_Administration::DropSnapShot", Common_Trace, 5 );

    Kernel_StateScope stateConfig( Kernel_State::criticalStateConfiguration, m_State );
    Kernel_StateScope stateAddVolume( Kernel_State::criticalStateAddDataVolume, m_State );

    if( ! stateConfig.EnterCriticalState()){
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if( ! stateAddVolume.EnterCriticalState()){
        RTE_Message(Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"AddDataVolume"));
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if( ! DropSnapShot( trans.trTaskId_gg00 )){
        trans.trError_gg00 = e_disk_not_accessible;
        return;
    }
}

/* --------------------------------------------------------------------------- */

bool Kernel_Administration::DropSnapShot( const tsp00_TaskId    taskId )
{
    if( ! ReadRestartRecord( taskId ))
        return( false );

    if( 0 == k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 )
        return( true ); // no snapshot available

    k57restartrec->rstConfigParam_kb00().crSnapShotRestartRecordAddr_kb00 = 0;

    if( FlushRestartRecord( taskId ))
        return( true );

    return( false );
}
/* --------------------------------------------------------------------------- */

void Kernel_Administration::MigrateVolumes(
                                           tgg00_TransContext      &trans,
                                           SAPDBErr_MessageList    &msgList )
{
    Kernel_StateScope stateConfig( Kernel_State::criticalStateConfiguration, m_State );
    Kernel_StateScope stateAddVolume( Kernel_State::criticalStateAddDataVolume, m_State );

    if( ! stateConfig.EnterCriticalState()){
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }

    if( ! stateAddVolume.EnterCriticalState()){
        trans.trError_gg00 = e_dbm_command_not_possible;
        return;
    }


    if( ! IOMan_IMigration::GetInstance().MigrateVolumes( 
        *RTETask_ITask::ByTaskID(trans.trTaskId_gg00), msgList ))
    {
        trans.trError_gg00 = e_disk_not_accessible;
    }
}

/* --------------------------------------------------------------------------- */

bool Kernel_Administration::IsHotStandbyConfigured()
{
    SAPDBErr_MessageList                   errlist;
    RTEHSS_KernelInterface::Configuration  config;

    (void) RTEHSS_KernelInterface::Instance().
              CheckForValidHotStandbyConfiguration(config, errlist);

    if ( RTEHSS_KernelInterface::ConfigurationHotStandby == config )
        return true;
    else if ( RTEHSS_KernelInterface::ConfigurationNormal == config )
        return false;
    else // ConfigurationInvalid, ...
    {
        errlist = errlist + Admin_Exception(__CONTEXT__, KERNEL_HINT_HOTSTANDBY_CONFIG);
        RTE_Message(errlist);
        Offline();
    }
    return false;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::ForceHotStandbyRoleToMaster()
{
    SAPDBErr_MessageList errlist;

    if ( ! RTEHSS_KernelInterface::Instance().SetRoleToMaster(errlist) )
    {
        RTE_Message(errlist);
        Offline(true);
    }
    m_State.SetHotStandbyRole(Kernel_State::standbyRoleMaster);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::DetermineHotStandbyRole()
{
    if ( Kernel_State::standbyRoleNone != m_State.GetHotStandbyRole()
         ||
         ! IsHotStandbyConfigured() )
        return;

    // hot stand by is configured - which role does the running instance own

    RTEHSS_KernelInterface &hss = RTEHSS_KernelInterface::Instance();
    SAPDBErr_MessageList    errlist;
    SAPDB_Char const *      localNode = RTE_ISystem::Instance().GetLocalNodeName();
    RTE_Nodename            masterName;
    bool                    shouldBeMaster;

    // Try to read the master node name from log info page

    if ( GetMaster (masterName, errlist) )
        shouldBeMaster = ! strcmp (masterName, localNode);
    else
    {
        SAPDBErr_MessageList    tmperrlist;
        strcpy (masterName, "NOT_INITIALIZED");
        if ( ! hss.IsDefaultMaster (shouldBeMaster, tmperrlist) )
        {
            RTE_Message(errlist+tmperrlist);
            Offline(true);
        }
    }

    if ( shouldBeMaster )
        ForceHotStandbyRoleToMaster();
    else
    {
        if ( ! hss.SetRoleToStandby(masterName, errlist) )
        {
            RTE_Message(errlist);
            Offline(true);
        }
        m_State.SetHotStandbyRole(Kernel_State::standbyRoleStandby);
    }
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::RoleIsStandby()
{
    return Kernel_State::standbyRoleStandby == m_State.GetHotStandbyRole();
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::GetMaster (RTE_Nodename         &masterNodeName,
                                       SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::GetMaster", Common_Trace, 5);

    masterNodeName[0] = 0; // set to empty string

    tsp00_TaskId taskid;

    vgetpid(taskid);

    if ( m_State.IsAdmin() )
    {
        bool leaveCriticalState = false;

        if ( ! m_State.IsCriticalState(Kernel_State::criticalStateConfiguration) )
        {
            if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateConfiguration) )
            {
                errlist = Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration");
                return false;
            }
            leaveCriticalState = true;
        }

        if ( ! Log_Volume::Instance().PrepareLogInfoPageForRead (taskid) )
        {
            if ( leaveCriticalState )
                m_State.LeaveCriticalState(Kernel_State::criticalStateConfiguration);

            errlist = Admin_Exception(__CONTEXT__,KERNEL_PREPARE_LOGINFO_PAGE_FOR_READ_FAILED);
            return false;
        }

        if ( leaveCriticalState )
            m_State.LeaveCriticalState(Kernel_State::criticalStateConfiguration);
    }

    return Log_Volume::Instance().GetMasterNodeName(masterNodeName);
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::InitStandbyResult
Kernel_Administration::InitStandby (tsp00_TaskId          taskid,
                                    SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::InitStandby", Common_Trace, 5);

    if ( Kernel_State::standbyRoleStandby != m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Init Standby","role is not standby");
        return InitStandbyWrongRole;
    }

    Kernel_StateScope stateConfig( Kernel_State::criticalStateConfiguration, m_State );
    Kernel_StateScope stateAddVolume( Kernel_State::criticalStateAddDataVolume, m_State );

    if ( ! stateConfig.EnterCriticalState() )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "Configuration");
        return InitStandbyNotAllowed;
    }

    if ( ! stateAddVolume.EnterCriticalState() )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "AddDataVolume");
        return InitStandbyNotAllowed;
    }

    RTEHSS_KernelInterface &hss = RTEHSS_KernelInterface::Instance();

    // Notify INIT STANDBY was accepted
    hss.GotInitStandby();

    // Check if the master wants to become a standby

    RTE_Nodename masterNodeName;
    RTE_Nodename ownNodeName;

    if ( ! GetMaster (masterNodeName, errlist) )
        return InitStandbyCannotReadMaster;

    if ( ! hss.GetOwnNodeName (ownNodeName,errlist) )
        return InitStandbyCannotReadOwnName;

    if ( ! strcmp (masterNodeName, ownNodeName) )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_INIT_STANDBY_NOT_ALLOWED,
                                  ownNodeName, masterNodeName);
        return InitStandbyNotAllowed;
    }

    // Write a message to knldiag

    RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_INIT_STANDBY_START, masterNodeName));

    if( ! IOMan_IDataManager::GetInstance().CreateAllDataVolumes(taskid) )
        return InitStandbyCreateDataVolumesFailed;

    // determine the masters mapping info

    RTEHSS_KernelInterface::Mapping otherMapping;

    if ( ! hss.SendInfoStandby(masterNodeName, otherMapping, errlist) )
        return InitStandbyCommunicationError;

    // Establish the data volume mirror

    if ( ! hss.EstablishMirror(otherMapping, errlist) )
        return InitStandbyEstablishMirrorFailed;

    if ( ! hss.FreeMemory(otherMapping.mappingString, errlist) )
        return InitStandbyFreeMemoryFailed;

    // The data volumes become prepared for usage

    if ( ! hss.SendPrepareStandby(errlist) )
        return InitStandbyCommunicationError;

    IOMan_IDataManager::GetInstance().CloseAllDataVolumes(taskid);
    return InitStandbyOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::RestartStandbyResult
Kernel_Administration::RestartStandby (tsp00_TaskId          taskid,
                                       tgg00_TransContext   &serverStartContext,
                                       SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestartStandby", Common_Trace, 5);

    RTEHSS_KernelInterface &hss       = RTEHSS_KernelInterface::Instance();
    tgg00_BasisError       &trError   = serverStartContext.trError_gg00;
    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(serverStartContext.trAllocator_gg00));

    if ( Kernel_State::standbyRoleStandby != m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Restart Standby","role is not standby");
        return RestartStandbyWrongRole;
    }

    if ( m_State.IsOnline() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Restart Standby","state is online");
        return RestartStandbyNotAllowed;
    }

    if ( m_State.IsRedoLogProcessing() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Restart Standby","state is redo");
        return RestartStandbyNotAllowed;
    }

    if ( ! m_State.EnterCriticalState(Kernel_State::criticalStateRedo) )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_ENTER_CRITICAL_STATE_FAILED, "Redo");
        return RestartStandbyNotAllowed;
    }

    // Notify RESTART STANDBY was accepted
    hss.GotRestartStandby();

    if ( ! RestartPrepare (serverStartContext, restartKindStandby, errlist) )
    {
        m_State.LeaveCriticalState(Kernel_State::criticalStateRedo);
        return RestartStandbyRestartPrepareFailed;
    }

    SrvTasks_JobRedo job (jobStandbyRestartCoordinator,SrvTasks_Job::Attributes());

    if ( job.ExecuteByServer (*RTETask_ITask::Self(),errlist) != SrvTasks_Job::executeOK )
    {
        m_State.LeaveCriticalState(Kernel_State::criticalStateRedo);
        return RestartStandbyTriggerServerFailed;
    }

    return RestartStandbyOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::PrepareStandbyResult
Kernel_Administration::PrepareStandby (      tsp00_TaskId          taskid,
                                             tgg00_TransContext   &savepointStartContext,
                                             SAPDBErr_MessageList &errlist,
                                       const SAPDB_Char           *newStandbyNode)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::PrepareStandby", Common_Trace, 5);

    if ( Kernel_State::standbyRoleMaster != m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Prepare Standby","role is not master");
        return PrepareStandbyWrongRole;
    }

    // communicate with standby to split the mirror of the data volumes
    RTEHSS_KernelInterface          &hss = RTEHSS_KernelInterface::Instance();
    RTEHSS_KernelInterface::Mapping  otherMapping;
    RTE_Nodename                     othernode;

    strcpy ( othernode, newStandbyNode );

    SAPDB_Int4 nodeIndex;

    if ( ! RTEHSS_StandbyNodeList::Instance().AddNode(othernode, nodeIndex) )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_STANDBY_NODELIST_OP_FAILED,
                                  "PREPARE STANDBY", othernode);
        return PrepareStandbyAddNodeFailed;
    }

    RTEHSS_StandbyNodeList::Instance().GotPrepareStandby(nodeIndex);

    // get mapping of standby node
    if ( ! hss.SendInfoStandby(othernode, otherMapping, errlist) )
    {
        errlist.Overrule( Admin_Exception(__CONTEXT__, KERNEL_STANDBY_SEND_TO_NODE_FAILED,
                                                  "PREPARE STANDBY", othernode) );
        return PrepareStandbySendInfoToStandbyFailed;
    }

    // Trigger savepoint to increase the chance for the standby to be able to restart
    // without the error: log and data incompatible
    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait (*RTETask_ITask::ByTaskID(taskid), Log_SVPReasonSplitMirror, errlist) )
        return PrepareStandbyStartSavepointFailed;

    // finally split the mirror to the standby which can run now autonomous
    if ( ! hss.SplitMirror(otherMapping, errlist) )
        return PrepareStandbySplitMirrorFailed;

    if ( ! hss.FreeMemory(otherMapping.mappingString, errlist) )
        return PrepareStandbyFreeMemoryFailed;

    return PrepareStandbyOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::TakeOverResult
Kernel_Administration::TakeOver (tsp00_TaskId          taskid,
                                 Kernel_Date           date,
                                 Kernel_Time           time,
                                 SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::TakeOver", Common_Trace, 5);

    if ( Kernel_State::standbyRoleStandby != m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"TakeOver","role is not standby");
        return TakeOverWrongRole;
    }
    if ( m_State.IsOnline() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"TakeOver","state is already online");
        return TakeOverNotAllowed;
    }
    if ( ! m_State.IsRedoLogProcessing() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"TakeOver","state is not redo");
        return TakeOverNotAllowed;
    }
    if ( m_WaitingForTakeOverReady != cgg_nil_pid )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"TakeOver","it seems that another TakOver is running");
        return TakeOverNotAllowed;
    }
    if ( IsMasterAlive(errlist) ) // PTS 1127374 UH 2004-01-23
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"TakeOver","master is alive");
        return TakeOverNotAllowed;
    }

    // Notify TAKEOVER was accepted
    RTEHSS_KernelInterface::Instance().GotTakeover();

    // First make sure that no one is accessing the log volumes.
    m_RedoManager.SuspendRedoReader(taskid);

    // detach and attach again the log volume
    Log_Volume::Instance().CloseLogArea(taskid);

    if ( ! RTEHSS_KernelInterface::Instance().SetRoleToMaster(errlist) )
        return TakeOverSetRoleToMasterFailed;

    m_State.SetHotStandbyRole(Kernel_State::standbyRoleMaster);

    if ( ! Log_Volume::Instance().OpenLogArea(taskid) )
        return TakeOverOpenLogAreaFailed;

    // re-read log info page
    Log_Volume::Instance().ReadLogInfoPageFromLogVolume(taskid);

    // Resume the redo log reader which reads now until the end of the log
    if ( m_State.IsRedoLogProcessing() )
    {
        m_WaitingForTakeOverReady = taskid;
        m_RedoManager.ResumeRedoReader(date,time);
        vsuspend(m_WaitingForTakeOverReady, KERNEL_ADMIN_WAIT_FOR_TAKEOVER);
        m_WaitingForTakeOverReady = cgg_nil_pid;
    }
    return TakeOverOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::RegisterStandbyResult
Kernel_Administration::RegisterStandby (      tsp00_TaskId            taskid,
                                              SAPDBMem_IRawAllocator &allocator,
                                        const SAPDB_Char             *standbyNodeId,
                                              Log_RawDeviceOffset     firstNeededOffset,
                                              Log_RawDeviceOffset    &maxValidOffset,
                                              SAPDBErr_MessageList   &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RegisterStandby", Common_Trace, 5);

    if ( Kernel_State::standbyRoleMaster != m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"RegisterStandby","role is not master");
        return RegisterStandbyWrongRole;
    }

    if ( ! m_State.IsOnline() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"RegisterStandby","state is not online");
        return RegisterStandbyNotAllowed;
    }

    SAPDB_Int4   nodeIndex;
    RTE_Nodename othernode;

    strcpy ( othernode, standbyNodeId );

    if ( ! RTEHSS_StandbyNodeList::Instance().FindNode(othernode, nodeIndex) )
    {
        // The standby did not begin with Init Standby (new mirror)
        // But instead the standby tried immediately the Restart Standby
        if ( ! RTEHSS_StandbyNodeList::Instance().AddNode(othernode, nodeIndex) )
            return RegisterStandbyAddNodeFailed;
    }

    RTEHSS_StandbyNodeList::Instance().GotRegisterStandby (nodeIndex, firstNeededOffset.RawValue());

    // Tell the other standby's that there is a new one
    SAPDB_UInt propagateErrors = PropagateInfoAboutStandby (allocator, standbyNodeId, true, errlist);
    if ( propagateErrors > 0 )
        return RegisterStandbyPropagationFailed;

    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist);
        errlist.ClearMessageList();
    }

    RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_NEW_STANDBY,
                                othernode, SAPDB_ToString(firstNeededOffset)));

    // Give the new standby the last readable offset for his Redo. This is not the current writePosition,
    // but the page before this, because the last page may be written multiple times.
    // m_LastSyncOffset = (--Log_Volume::Instance().GetWriterPosition()).GetPosition();
    m_LastSyncOffset = Log_Savepoint::GetLastSavepointOffset(); // UH 2003-11-27
    maxValidOffset   = m_LastSyncOffset;

    // Add the new standby to the standby list
    RTE_Nodename newStandbyNode;
    strcpy ( newStandbyNode, standbyNodeId );

    RTEHSS_StandbyNodeList::Instance().SetWaitForSynchronize(nodeIndex);

    m_SynchronizationIsNeeded = true;

    return RegisterStandbyOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::InsertStandbyResult
Kernel_Administration::InsertStandby (      tsp00_TaskId          taskid,
                                      const SAPDB_Char           *standbyNodeId,
                                            SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::InsertStandby", Common_Trace, 5);

    if ( Kernel_State::standbyRoleNone == m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"InsertStandby","role is neither master nor standby");
        return InsertStandbyWrongRole;
    }

    // PTS 1125942 mb 2004-02-13 synchronize insert/remove-standby by using state-information
    Kernel_StateScope state (Kernel_State::criticalStateConfiguration, m_State);
    if ( ! state.EnterCriticalState() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration");
        return InsertStandbyNotAllowed;
    }

    RTE_Nodename newStandbyNode;

    strcpy ( newStandbyNode, standbyNodeId );

    SAPDB_Int4 nodeIndex;

    if ( ! RTEHSS_StandbyNodeList::Instance().AddNode(newStandbyNode, nodeIndex) )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_STANDBY_NODELIST_OP_FAILED,
                                  "INSERT STANDBY", newStandbyNode);
        return InsertStandbyAddNodeFailed;
    }

    RTEHSS_StandbyNodeList::Instance().SetWaitForSynchronize(nodeIndex);

    return InsertStandbyOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::RemoveStandbyResult
Kernel_Administration::RemoveStandby (      tsp00_TaskId          taskid,
                                      const SAPDB_Char           *standbyNodeId,
                                            SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RemoveStandby", Common_Trace, 5);

    if ( Kernel_State::standbyRoleNone == m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"RemoveStandby","role is neither master nor standby");
        return RemoveStandbyWrongRole;
    }

    // PTS 1125942 mb 2004-02-13 synchronize insert/remove-standby by using state-information
    Kernel_StateScope state (Kernel_State::criticalStateConfiguration, m_State);
    if ( ! state.EnterCriticalState() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_CRITICAL_STATE_NOT_ENTERED,"Configuration");
        return RemoveStandbyNotAllowed;
    }


    RTE_Nodename remStandbyNode;

    strcpy ( remStandbyNode, standbyNodeId );

    SAPDB_Int4 nodeIndex;

    if ( ! RTEHSS_StandbyNodeList::Instance().FindNode(remStandbyNode, nodeIndex) )
    {
        errlist = Admin_Exception(__CONTEXT__, KERNEL_STANDBY_NODELIST_OP_FAILED,
                                  "REMOVE STANDBY", remStandbyNode);
        return RemoveStandbyNodeNotFound;
    }

    RTEHSS_StandbyNodeList::Instance().RemoveNode(nodeIndex);

    // tell all the other standby nodes that current standby node is not available
    SAPDB_UInt propagateErrors = PropagateInfoAboutStandby(m_Allocator, remStandbyNode, false, errlist);

    // if neccessary switch off synchronization
    if (RTEHSS_StandbyNodeList::Instance().IsEmpty())
    {
        m_SynchronizationIsNeeded  = false;
        m_LastSyncOffset.Invalidate();
        m_LastSyncTime.Invalidate();
        RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_LAST_STANDBY_REMOVED,standbyNodeId));
    }
    else
    {
        RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_STANDBY_REMOVED,standbyNodeId));
    }

    return RemoveStandbyOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::SynchronizeResult
Kernel_Administration::Synchronize (tsp00_TaskId          taskid,
                                    Log_RawDeviceOffset   newMaxOffset,
                                    Log_RawDeviceOffset  &lastRedoOffset,
                                    SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::Synchronize", Common_Trace, 5);

    lastRedoOffset.Invalidate();

    if ( Kernel_State::standbyRoleStandby != m_State.GetHotStandbyRole() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Synchronize","role is not standby");
        return SynchronizeWrongRole;
    }
    if ( ! m_State.IsRedoLogProcessing() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Synchronize","state is not redo");
        return SynchronizeNotAllowed;
    }
    if ( m_State.IsOnline() )
    {
        errlist = Admin_Exception(__CONTEXT__,KERNEL_COMMAND_NOT_ALLOWED,"Synchronize","state is online");
        return SynchronizeNotAllowed;
    }

    RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_SYNC_RECEIVED,SAPDB_ToString(newMaxOffset)));

    m_RedoManager.SetLastRedoReadOffset(newMaxOffset, taskid);

    lastRedoOffset = m_RedoManager.GetLastRedoReadOffset();

    RTEHSS_KernelInterface::Instance().GotSynchronize(newMaxOffset.RawValue(), lastRedoOffset.RawValue());
    return SynchronizeOk;
}
/* --------------------------------------------------------------------------- */
Kernel_IAdminHotStandby::TriggerSynchronizeResult
Kernel_Administration::TriggerSynchronize (tgg00_TransContext &serverTaskStartContext)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::TriggerSynchronize", Common_Trace, 5);

    if ( ! m_SynchronizationIsNeeded || m_SynchronizationIsRunning )
        return TriggerSynchronizeOk; // fast return because called by timeout task

    // needs no critical region because is only called by timeout task
    m_SynchronizationIsRunning = true;

    Log_RawDeviceIterator currentWriterPosition = Log_Volume::Instance().GetWriterPosition();

    RTESys_ReadMemoryBarrier();

    if ( m_LastSyncOffset.IsValid()
         &&
         currentWriterPosition.GetPosition() == m_LastSyncOffset )
    {
        // Not enough log was written to sync again.
        m_SynchronizationIsRunning = false;
        return TriggerSynchronizeNotNeeded;
    }

    // decrement the position, because the page on the current write position must not been read by the standby
    --currentWriterPosition;

    if ( m_LastSyncOffset.IsValid()
         &&
         currentWriterPosition.GetDistanceFrom(m_LastSyncOffset) < 1 )
    {
        // Not enough log was written to sync again.
        m_SynchronizationIsRunning = false;
        return TriggerSynchronizeNotNeeded;
    }

    Trans_Context &context = Trans_Context::GetContext(serverTaskStartContext);

    if (m_LastSyncTime.IsValid())           // PTS 1125481 mb 2003-11-18
    {
        RTEConf_Parameter::Integer syncInterval = 0;
        if( ! RTEConf_ParameterAccess::Instance()->GetInteger(UTF8("HS_SYNC_INTERVAL"), syncInterval, context.ErrorList() ) )
            return TriggerSynchronizeStartSyncServerFailed;

        Kernel_DateTime startOfSyncInterval;
        startOfSyncInterval.DetermineDelayedTime(static_cast<SAPDB_UInt>(syncInterval));

        if (startOfSyncInterval.OlderThan(m_LastSyncTime))
        {
            // span of time since last synchronize not long enough
            m_SynchronizationIsRunning = false;
            return TriggerSynchronizeNotNeeded;
        }
    }

    SrvTasks_JobRedo job (jobStandbySynchronizeServer,SrvTasks_Job::Attributes());

    if ( job.ExecuteByServer(*RTETask_ITask::Self(),context.ErrorList()) != SrvTasks_Job::executeOK )
    {
        m_SynchronizationIsRunning = false;
        return TriggerSynchronizeStartSyncServerFailed;
    }

    return TriggerSynchronizeOk;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::SynchronizeServer ( tsp00_TaskId            taskid,
                                                SAPDBMem_IRawAllocator &allocator)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::SynchronizeServer", Common_Trace, 5);

    RTEHSS_KernelInterface &hss = RTEHSS_KernelInterface::Instance();
    SAPDBErr_MessageList    errlist;
    RTEHSS_NodeChain        nodechain (allocator);
    RTE_Nodename            sendToNodename;
    Log_RawDeviceIterator   lastValidReadPosition = Log_Volume::Instance().GetWriterPosition();
    Log_RawDeviceOffset     firstStillNeededOffset;
    Log_RawDeviceOffset     offsetFromStandby;

    // decrement the position, because the page on the current write position must not been read by the standby
    --lastValidReadPosition;

    if ( m_LastSyncOffset.IsValid() )
    {
        if ( m_LastSyncOffset == lastValidReadPosition.GetPosition() )
        {
            RTE_Message(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                        "Kernel_Administration::SynchronizeServer: m_LastSyncOffset == lastValidReadPosition") );
            return;
        }
    }

    m_LastSyncOffset = lastValidReadPosition.GetPosition();
    m_LastSyncTime.DetermineTime();

    if ( ! nodechain.Fill() )
    {
        RTE_Message(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                    "Kernel_Administration::SynchronizeServer: nodechain.Fill() failed => NO SYNC WITH STANDBY") );
        return;
    }

    if ( ! nodechain.FirstNode(sendToNodename) )
    {
        // no standby nodes available - this should not happen
        RTE_Message(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                    "Kernel_Administration::SynchronizeServer: no standby node found. Synchronization switched off!") );
        m_LastSyncTime.Invalidate();
        m_LastSyncOffset.Invalidate();
        m_SynchronizationIsNeeded  = false;

        RTESys_WriteMemoryBarrier();
        m_SynchronizationIsRunning = false;
        return;
    }

    RTEHSS_KernelInterface::LogPosition auxPosition;

    do
    {
        if ( ! hss.SendSynchronize(sendToNodename,
                                   lastValidReadPosition.GetPosition().RawValue(),
                                   auxPosition,
                                   errlist) )
        {
            if ( Common_Check.ChecksLevel(6) )
                Kernel_VTrace() << "Sync: '"   << sendToNodename
                                << "' send: " << lastValidReadPosition.GetPosition()
                                << ", received: NO ANSWER";

            if ( ! errlist.IsEmpty() )
            {
                RTE_Message(errlist);
                errlist.ClearMessageList();
            }

            // remove the not answering standby node from nodelist.
            RemoveStandby (taskid, sendToNodename, errlist); // PTS 1125942 mb 2004-01-14
            if ( ! errlist.IsEmpty() )
            {
                RTE_Message(errlist);
                errlist.ClearMessageList();
            }
        }
        else
        {
            if ( Common_Check.ChecksLevel(6) )
                Kernel_VTrace() << "Sync: '"   << sendToNodename
                                << "' send: " << lastValidReadPosition.GetPosition()
                                << ", received: " << firstStillNeededOffset;
            offsetFromStandby = auxPosition;
            RTE_Message(Admin_Exception(__CONTEXT__, KERNEL_SYNCREPLY_RECEIVED,
                                        sendToNodename, SAPDB_ToString(offsetFromStandby)));

            if ( firstStillNeededOffset.IsInvalid() )
                firstStillNeededOffset = offsetFromStandby;
            else
            {
                if ( offsetFromStandby.IsValid()
                     &&
                     lastValidReadPosition.GetDistanceFrom(offsetFromStandby)
                     >
                     lastValidReadPosition.GetDistanceFrom(firstStillNeededOffset) )
                {
                    firstStillNeededOffset = offsetFromStandby;
                }
            }
        }
    }
    while ( nodechain.NextNode(sendToNodename) );

    // Assert that the m_LastSyncOffset is flushed before the timeouttask reads it
    RTESys_WriteMemoryBarrier();
    m_SynchronizationIsRunning = false;
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::BroadcastLineToStandbys (
                            const BroadcastDestination    dest,
                                  tsp00_TaskId            taskid,
                            const char  *                 line,
                            const SAPDB_UInt              lineLength)

{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::BroadcastLineToStandbys", Common_Trace, 5);

    RTEHSS_KernelInterface &hss = RTEHSS_KernelInterface::Instance();
    SAPDBErr_MessageList    errlist;
    RTEHSS_NodeChain        nodechain (m_Allocator);
    RTE_Nodename            sendToNodename;

    if (!IsHotStandbyConfigured())
    {
        return;
    }
    if ( ! nodechain.Fill() )
    {
        RTE_Message(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                    "Kernel_Administration::BroadcastLineToStandbys: nodechain.Fill() failed => NO BackupHistory sent") );
        return;
    }

    if ( ! nodechain.FirstNode(sendToNodename) )
    {
        // no standby nodes available - this should not happen, but this is no error
        return;
    }

    // create the string, which has to be send
    char * lineToSend;
    newarray (lineToSend, lineLength + 3, m_Allocator);
    lineToSend[0] = '\'';
    SAPDB_MemCopyNoCheck(&lineToSend[1], line, lineLength);
    lineToSend[lineLength+1] =  '\'';
    lineToSend[lineLength+2] =  '\0';

    do
    {
        bool sendWasOk;
        if (dest == BackupHistoryFile)
        {
            sendWasOk = hss.SendBackupHistoryInfo(sendToNodename,
                                                  lineToSend,
                                                  errlist);
        }
        else
        {
            sendWasOk = hss.SendBackupMediumInfo(sendToNodename,
                                                 lineToSend,
                                                 errlist);
        }
        if ( ! sendWasOk )
        {
            if ( Common_Check.ChecksLevel(6) )
                Kernel_VTrace() << "BroadcastLineToStandbys: '"   << sendToNodename
                                << ", received: NO ANSWER";

            if ( ! errlist.IsEmpty() )
            {
                RTE_Message(errlist);
                errlist.ClearMessageList();
            }

            // remove the not answering standby node from nodelist.
            RemoveStandby (taskid, sendToNodename, errlist);
            if ( ! errlist.IsEmpty() )
            {
                RTE_Message(errlist);
                errlist.ClearMessageList();
            }
        }
        else
        {
            if ( Common_Check.ChecksLevel(6) )
                Kernel_VTrace() << "BroadcastLineToStandbys: '"   << sendToNodename;
        }
    }
    while ( nodechain.NextNode(sendToNodename) );

    destroyarray (lineToSend, lineLength + 3, m_Allocator);
}

/* --------------------------------------------------------------------------- */
void Kernel_Administration::BroadcastBackupHistoryEntry (
                        tsp00_TaskId            taskid,
                        const char * line,
                        const SAPDB_UInt lineLength)
{
    BroadcastLineToStandbys (BackupHistoryFile, taskid, line, lineLength);
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::BroadcastBackupMediumDefEntry (
                        tsp00_TaskId            taskid,
                        const char * line,
                        const SAPDB_UInt lineLength)
{
    BroadcastLineToStandbys (BackupMediumDefinitionFile, taskid, line, lineLength);
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::RestartStandbyServer (tsp00_TaskId          taskid,
                                                  tgg00_TransContext   &oldcontext,
                                                  SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::RestartStandbyServer", Common_Trace, 5);

    // go to permanent redo until Offline() or TakeOver() is called

    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(oldcontext.trAllocator_gg00));

    const bool result = RestartInternOk
                        ==                                                                  // PTS 1134799 mb 2005-04-08
                        RestartIntern (taskid, allocator, oldcontext, restartKindStandby,
                                       Kernel_Date(), Kernel_Time(), Log_IOSequenceNo(),
                                       errlist);

    if ( m_WaitingForTakeOverReady != cgg_nil_pid )
        vresume(m_WaitingForTakeOverReady);

    m_State.LeaveCriticalState(Kernel_State::criticalStateRedo);

    return result;
}
/* --------------------------------------------------------------------------- */
SAPDB_UInt
Kernel_Administration::PropagateInfoAboutStandby (       SAPDBMem_IRawAllocator &allocator,
                                                   const SAPDB_Char             *standbyNodename,
                                                         bool                    isActive,
                                                         SAPDBErr_MessageList   &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::PropagateInfoAboutStandby", Common_Trace, 5);

    RTEHSS_NodeChain        nodechain (allocator);
    RTEHSS_KernelInterface &hss = RTEHSS_KernelInterface::Instance();
    RTE_Nodename            nodename;
    RTE_Nodename            sendToNodename;
    SAPDB_UInt              numErrorsOccured = 0;

    if ( strlen(standbyNodename)+1 > sizeof(nodename) )
        return 1;

    if ( ! nodechain.Fill() )
        return 1;

    if ( Common_Check.ChecksLevel(6) )
        Kernel_VTrace() << "Propagate that '" << standbyNodename
                        << "' is "            << (isActive?"ACTIVE":"INACTIVE") << FlushLine;

    strcpy ( nodename, standbyNodename );

    if ( nodechain.FirstNode(sendToNodename) )
    {
        do
        {
            if ( strcmp(sendToNodename, nodename) )
            {
                if ( Common_Check.ChecksLevel(6) )
                    Kernel_VTrace() << "To: '"   << sendToNodename << "'" << FlushLine;
                if ( (isActive ? hss.SendInsertStandby (sendToNodename, nodename, errlist)
                               : hss.SendRemoveStandby (sendToNodename, nodename, errlist)) )
                {
                    ++numErrorsOccured;
                }
            }
        }
        while ( nodechain.NextNode(sendToNodename) );
    }
    return numErrorsOccured;
}

/* --------------------------------------------------------------------------- */
void Kernel_Administration::GetDelayTimeStamp (Kernel_Date &date,
                                               Kernel_Time &time)
{
    SAPDBTRACE_METHOD_DEBUG ("Kernel_Administration::GetDelayTimeStamp", Common_Trace, 5);

    SAPDBErr_MessageList errlist;
    SAPDB_Int4           delayTimeInSec; // (see cserv.pcf)

    if ( ! RTEHSS_KernelInterface::Instance().GetOwnDelayTime (delayTimeInSec, errlist) )
        Offline(true);

    Kernel_DateTime dateTime;

    dateTime.DetermineDelayedTime( delayTimeInSec );
    date = dateTime.GetDate();
    time = dateTime.GetTime();
}
/* --------------------------------------------------------------------------- */
Rst_RedoManager& Kernel_Administration::GetRedoManager()
{
    return m_RedoManager;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Administration::IsRedoLogAborted (tsp00_TaskId taskid)
{
    return m_RedoManager.IsAborted();
}
/* --------------------------------------------------------------------------- */
void Kernel_Administration::AbortRedoLogByRestoreComponent (tsp00_TaskId     taskid,
                                                            tgg00_BasisError abortReason)
{
    m_RedoManager.AbortByRestoreComponent(taskid, abortReason);
}
