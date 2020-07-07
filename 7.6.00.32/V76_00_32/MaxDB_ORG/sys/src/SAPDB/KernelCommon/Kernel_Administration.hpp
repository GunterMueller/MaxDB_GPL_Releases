/*! @defgroup  KernelAdministration Kernel Administration */
/*! @ingroup   KernelAdministration
    @file      Kernel_Administration.hpp
    @author    UweH
    @brief     This is the handler which coordinates all DBM commands in the kernel. (utility)
*/
/*!
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
#ifndef Kernel_Administration_H
#define Kernel_Administration_H

#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "KernelCommon/Kernel_IAdminBackupRecovery.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_IAdminDiagnose.hpp"

#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_State.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "Restart/Rst_RedoManager.hpp"
#include "Restart/Rst_Types.hpp"

class Trans_Context;

/*!
    @class Kernel_Administration
    @brief This class handles all dbm commands.
 */
class Kernel_Administration : public SAPDB_Singleton,
                              public Kernel_IAdminRestartShutdown,
                              public Kernel_IAdminHotStandby,
                              public Kernel_IAdminBackupRecovery,
                              public Kernel_IAdminInfo,
                              public Kernel_IAdminConfig,
                              public Kernel_IAdminDiagnose
{
public:
    /// Creates the singleton and may be called only once.
    static bool CreateInstance ();
    /// Access for interfaces
    static Kernel_Administration& Instance();
private: // interfaces
    typedef Kernel_IAdminBackupRecovery::DataBackupType DataBackupType;
	virtual void InitConfig (tgg00_TransContext &trans);
    virtual ActivateResult Activate (tsp00_TaskId          taskid,
                                     tgg00_TransContext   &oldcontext,
                                     SAPDBErr_MessageList &errlist);
    virtual void AddDataVolume (tgg00_TransContext &trans,
                                tsp00_VFilename    &volumeName,
                                SAPDB_Int4          newVolumeSize,
                                SAPDB_Int2          volumeId);
    virtual void AddLogVolume (tgg00_TransContext &trans,
                               tsp00_VFilename    &volumeName,
                               tsp00_VFilename    &mirrorVolumeName,
                               SAPDB_Int4          newVolumeSize,
                               SAPDB_Int2          volumeId);

    /*!
       @brief          drop a volume from the set of data volumes.
       @param          trans [in/out] 
       @param          volumeName [in] 
       @returns        none
     */
    virtual void DropDataVolume (tgg00_TransContext &trans,
                                SAPDB_Int2          volumeId);

	virtual void ReIntegrateBadLogVolume (tgg00_TransContext &trans,
                                          tsp00_VFilename    &devicename);
    virtual void GetBadLogVolume (tgg00_TransContext &trans,
                                  tsp00_VFilename    &devicename);
    virtual void ClearLog (tsp00_TaskId taskid);
    virtual void SetLogWriting (tsp00_TaskId taskid,
                                bool         on);
    virtual void SetLogAutoOverwrite (tsp00_TaskId taskid,
                                      bool         on);
    virtual bool SuspendLogWriter (tsp00_TaskId      taskid,
                                   Log_IOSequenceNo &lastWrittenIOsequence);

    virtual void CheckData( 
        tgg00_TransContext  &trans,
        const SAPDB_Bool    bWithExtendedCheck,
        const SAPDB_Bool    bWithIndexes,
        const SAPDB_Bool    bWithDeleteUnreferencedBlobs, // PTS 1133300 UH 2005-01-11
        SAPDB_Int4          &badIndexCount,
        SAPDB_Int4          &totalBadCount );
    
    virtual void CheckDataWithUpdate( 
        tgg00_TransContext  &trans,
        const SAPDB_Bool    bWithExtendedCheck,
        SAPDB_Int4          &pagesUsed,
        SAPDB_Int4          &blocksReleased,
        SAPDB_Int4          &badIndexCount,
        SAPDB_Int4          &totalBadCount );
    
    virtual void Diagnose( tgg00_MessBlock  &messBlock );

    virtual void ResumeLogWriter (tsp00_TaskId taskid);
    virtual void CreateSnapShot (tgg00_TransContext &trans);
    virtual void ActivateSnapShot (tgg00_TransContext &trans);
    virtual void DropSnapShot (tgg00_TransContext &trans);

    virtual void MigrateVolumes(
        tgg00_TransContext      &trans,
        SAPDBErr_MessageList    &msgList );

    virtual Kernel_Migration& MigrationHandler()
    {
        return m_Migration;
    }
    virtual bool KernelStateIsRedoLogProcessing();
    virtual bool KernelStateIsOnline();
    virtual bool KernelStateIsAdmin();
    virtual void GetNewCommandId( tsp00_TaskId     taskid,
                                  tgg00_UtilCmdId &id );
    virtual void GetRestartVersion
                           (tgg00_TransContext &trans,
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
                            SAPDB_Int4         &converterVersion);
    virtual bool IsCheckBackupConfigured();
    virtual bool IsLogWritingEnabled(tsp00_TaskId taskid);
    virtual bool IsLogAutoOverwriteEnabled(tsp00_TaskId taskid);
    virtual RestartResult Restart (tsp00_TaskId          taskid,
                                   tgg00_TransContext   &oldcontext,
                                   Kernel_Date           untildate,
                                   Kernel_Time           untiltime,
                                   Log_IOSequenceNo      untilIOSequence,
                                   SAPDBErr_MessageList &errlist);
    virtual void Shutdown (tgg00_TransContext &trans);
    virtual void Offline ( const tgg00_BasisError error,
                                 bool             forceBacktrace = false,
                                 bool             writeCore      = false );
    virtual void Offline ( const char * errorText = 0,
                                 bool   forceBacktrace = false,
                                 bool   writeCore      = false );
    virtual void Offline ( const SAPDBErr_MessageList &errlist,
                                 bool                  forceBacktrace = false,
                                 bool                  writeCore      = false );
    virtual void SetKernelStateOnline();
    virtual Rst_RedoManager& GetRedoManager();
    virtual void BackupData (tgg00_TransContext &trans,
                             DataBackupType      backupType,
                             tgg00_MessBlock    &backupDescription);
    virtual void RestoreData (tgg00_TransContext &trans,
                              tgg00_MessBlock    &backupDescription);
    virtual void BackupLog (tgg00_TransContext &trans,
                            tgg00_MessBlock    &backupDescription);
    virtual RestoreLogResult RestoreLog (tsp00_TaskId          taskid,
                                         tgg00_TransContext   &oldcontext,
                                         tgg00_MessBlock      &restoreDescription,
                                         SAPDBErr_MessageList &errlist);
    virtual void BackupRestoreHandling (tgg00_TransContext &trans,
                                        tgg00_MessBlock    &handlingDescription);
    virtual RedoLogResult RedoLog (tsp00_TaskId          taskid,
                                   tgg00_TransContext   &oldcontext,
                                   Rst_RedoKind          redokind,
                                   Log_RawDeviceOffset   lastRedoOffset,
                                   Kernel_Date           untildate,
                                   Kernel_Time           untiltime,
                                   Log_IOSequenceNo      untilIOSequence,
                                   SAPDBErr_MessageList &errlist);
    /// returns true if redo was aborted by any redo execution task.
    virtual bool IsRedoLogAborted (tsp00_TaskId taskid);
    virtual void AbortRedoLogByRestoreComponent (tsp00_TaskId     taskid,
                                                 tgg00_BasisError abortReason);
    virtual void SetNewDBIdentifierIfHistoryLost (tsp00_TaskId taskid);
    virtual bool DenyBackupAndRestore();
    virtual void PermitBackupAndRestore();

    virtual void GetBlockAddress ( 
         const RTETask_ITask         &task,
        const Data_PageNo            &pageno,
        const Data_PageRecoveryMode  &recMode,
        IOMan_BlockAddress           &block );
    
    virtual bool SetNewBlockAddress ( 
        const RTETask_ITask         &task,
        const Data_PageNo           &pageno,
        const Data_PageRecoveryMode &recMode,
        const IOMan_BlockAddress    &block );
    
    virtual bool DeleteBlockAddress( 
        const RTETask_ITask            &task,
        const Data_PageNo              &pageno,
        const Data_PageRecoveryMode    &recMode );
    
    virtual bool ReadRestartRecord ( tsp00_TaskId taskid);
    virtual bool FlushRestartRecord ( tsp00_TaskId taskid);
    virtual void DetermineHotStandbyRole();
    virtual bool RoleIsStandby();
    virtual bool GetMaster( RTE_Nodename         &masterNodeName,
                            SAPDBErr_MessageList &errlist);
    virtual InitStandbyResult InitStandby (tsp00_TaskId          taskid,
                                           SAPDBErr_MessageList &errlist);
    virtual RestartStandbyResult RestartStandby (tsp00_TaskId          taskid,
                                                 tgg00_TransContext   &serverStartContext,
                                                 SAPDBErr_MessageList &errlist);
    virtual PrepareStandbyResult PrepareStandby (      tsp00_TaskId          taskid,
                                                       tgg00_TransContext   &savepointStartContext,
                                                       SAPDBErr_MessageList &errlist,
                                                 const SAPDB_Char           *newStandbyNode);
    virtual TakeOverResult TakeOver (tsp00_TaskId          taskid,
                                     Kernel_Date           date,
                                     Kernel_Time           time,
                                     SAPDBErr_MessageList &errlist);
	virtual RegisterStandbyResult RegisterStandby (      tsp00_TaskId            taskid,
                                                         SAPDBMem_IRawAllocator &allocator,
                                                   const SAPDB_Char             *standbyNodeId,
                                                         Log_RawDeviceOffset     firstNeededOffset,
                                                         Log_RawDeviceOffset    &maxValidOffset,
                                                         SAPDBErr_MessageList   &errlist );
    virtual InsertStandbyResult InsertStandby (      tsp00_TaskId          taskid,
                                               const SAPDB_Char           *standbyNodeId,
                                                     SAPDBErr_MessageList &errlist);
    virtual RemoveStandbyResult RemoveStandby (      tsp00_TaskId          taskid,
                                               const SAPDB_Char           *standbyNodeId,
                                                     SAPDBErr_MessageList &errlist);
	virtual SynchronizeResult Synchronize ( tsp00_TaskId          taskid,
                                            Log_RawDeviceOffset   newMaxOffset,
                                            Log_RawDeviceOffset  &lastRedoOffset,
                                            SAPDBErr_MessageList &errlist );
    virtual TriggerSynchronizeResult TriggerSynchronize (tgg00_TransContext &serverTaskStartContext);
    virtual void SynchronizeServer ( tsp00_TaskId            taskid,
                                     SAPDBMem_IRawAllocator &allocator);
    virtual void BroadcastBackupHistoryEntry (
                            tsp00_TaskId            taskid,
                            const char * line,
                            const SAPDB_UInt lineLength);
    virtual void BroadcastBackupMediumDefEntry (
                            tsp00_TaskId            taskid,
                            const char * line,
                            const SAPDB_UInt lineLength);
                                     
	virtual bool RestartStandbyServer (tsp00_TaskId          taskid,
                                       tgg00_TransContext   &oldcontext,
                                       SAPDBErr_MessageList &errlist);
    virtual void GetDelayTimeStamp (Kernel_Date &date,
                                    Kernel_Time &time);
private:
    /// There are several internal kinds to scan the log.
    enum RestartKind
    {
        restartKindNormal     = 0,
        restartKindActivation = 1,
        restartKindRestoreLog = 2,
        restartKindStandby    = 3
	};
	/// constructor
    Kernel_Administration                 (SAPDBMem_IRawAllocator &allocator);

    /// Attach all data and log volumes. This is needed for switching to online mode.
    bool RestartFilesystem                (tgg00_TransContext &trans,
                                           const bool         incrementConverterVersion = true);

    void RestartFileSystemForSaveDataCold (const tsp00_TaskId taskid,
                                      tgg00_BasisError   &error);

    /// This is called before RedoLog can be called.
    bool RestartPrepare                   (tgg00_TransContext   &trans,
                                           RestartKind           restartkind,
                                           SAPDBErr_MessageList &errlist);
    /// result for RestartIntern()
    enum RestartInternResult
    {
        RestartInternOk,
        RestartInternPrepareFailed,
        RestartInternOmsContainerMigrationFailed,
        RestartInternRedoLogFailed
    };
    /// This is the internal used method to bring the kernel to online mode.
    RestartInternResult RestartIntern     (tsp00_TaskId            taskid,
                                           SAPDBMem_IRawAllocator &allocator,
                                           tgg00_TransContext     &oldcontext,
                                           RestartKind             restartkind,
                                           Kernel_Date             untildate,
                                           Kernel_Time             untiltime,
                                           Log_IOSequenceNo        untilIOSequence,
                                           SAPDBErr_MessageList   &errlist);
	/// This is used to control the steps INIT CONFIG and ACTIVATE SERVERDB
    bool CheckAndUpdateActivationState    (tgg00_TransContext   &trans,
                                           RestartKind           restartkind,
                                           SAPDBErr_MessageList &errlist);
	/// After data loading the database is set to readonly. A data backup resets this flag.
    /// If the backup was done in admin mode the next restart will reset the flag.
    bool CheckAndResetReadOnlyState       (tgg00_TransContext &trans);
    /// If an until timestamp is given it is check with this.
    /// If the current db state is younger then the given date then false is returned.
    bool CheckUntilTimeStamp              (Kernel_Date         untildate,
                                           Kernel_Time         untiltime);
    /// This scans the log volume and determines all parameters. firstKnown, lastKnown, ...
    bool RestartLog                          (tsp00_TaskId          taskid,
                                              RestartKind           reason,
                                              SAPDBErr_MessageList &errlist);
    /// propagate to all known standbys if a standby is active or inactive
    SAPDB_UInt PropagateInfoAboutStandby (       SAPDBMem_IRawAllocator &allocator,
                                           const SAPDB_Char             *standbyNodename,
                                                 bool                    isActive,
                                                 SAPDBErr_MessageList   &errlist );
    void PropagateInfoAboutStandby ( tgg00_TransContext &trans,
                                     const SAPDB_Char   *standbyNodename,
                                     bool                isActive );
	/// This returns true if the parameter configuration is set for hot standby
    bool IsHotStandbyConfigured();
    /// This is called by methods like InitConfig
    void ForceHotStandbyRoleToMaster();
    /// Remove the snapshot information from the restart info page, if available
    bool DropSnapShot( const tsp00_TaskId    taskId );
    /// format log area
    void ClearLogArea( tsp00_TaskId taskid );
    /// returns true if the master database can be accessed
    bool IsMasterAlive(SAPDBErr_MessageList &errlist); // PTS 1127374 UH 2004-01-23 new
    
    /// defines where the call to BroadcastLineToStandbys shall append the given line
    enum BroadcastDestination {
        BackupHistoryFile,			                    ///< destination is dbm.knl
        BackupMediumDefinitionFile                      ///< destination is dbm.mdf
    } ;
    /// sends the given Line to all standbys. Depending on dest, the line will be
    /// appended to the backup history file or the backup medium definietion file
    void BroadcastLineToStandbys (
                            const BroadcastDestination    dest,
                                  tsp00_TaskId            taskid,
                            const char *                  line,
                            const SAPDB_UInt              lineLength);

    /// in and outcopy of pages in case of diagnose repair
    bool DiagnoseIntern(
        tgg00_TransContext     &trans, 
        tgg00_MessBlock        &messBlock,
        tsp00_PageAddr         pPage,
        const tgg00_DiagType   diagType,
        const bool             bIsRestore,
        bool                   &bPageWasExtracted,
        bool                   &bCloseVolumes );

	/// resume drop volume after backup data
	void WakeupDropVolume ();

    /// achieve database backup
    bool BackupDataIntern( Trans_Context      &trans,
                           DataBackupType      backupType,
                           tgg00_MessBlock    &backupDescription );

private:
    /// pointer to singleton instance
    static Kernel_Administration* m_Instance;
    /// Its statistic can be found in ALLOCATORSTATISTICS with the label "Kernel_Administration".
    RTEMem_AllocatorWrapper m_Allocator;
    /// The redo manager which is used during restart.
    Rst_RedoManager m_RedoManager;
    /// This spinlock guards critical internal sections
    RTESync_NamedSpinlock m_SpinLock;
    /// The administration command counter.
    /// It is guarded by m_SpinLock.
    SAPDB_UInt2 m_CommandNo;
    /// This defines State it is used internally to check the semantik
    /// and the context of the the commands.
    /// This is the kernel state from admin sight.
    Kernel_State m_State;
    /// This is a waiting TakeOver task.
    tsp00_TaskId m_WaitingForTakeOverReady;
    /// This is a waiting DropVolume task.
    Kernel_TaskWaitList    m_WaitingForBackupFinished;
    /// This is true if DataBackup is running
    bool         m_DataBackupIsRunning;
    /// This is true if synchronization is really needed (if at least one standby was added)
    bool         m_SynchronizationIsNeeded;
    /// This is true if synchronization is running
    bool         m_SynchronizationIsRunning;
    /// writer position of last sync
    Log_RawDeviceOffset m_LastSyncOffset;
    /// timestamp of the last sync (PTS 1125481 mb 2003-11-17)
    Kernel_DateTime m_LastSyncTime;
    /// This is the migration handler
    Kernel_Migration    m_Migration;
};
#endif  /* Kernel_Administration_H */
