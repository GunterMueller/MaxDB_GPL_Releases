# ------------------------------------------------------------------------------
#
# Kernel Descriptions
#
# ------------------------------------------------------------------------------
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

regex(.*\.cpp$)    -except

# -------------------------------------------------------------------------------------
# Errors and messages:
# -------------------------------------------------------------------------------------
#
Kernel_Messages.genmsg     ascii
Kernel_MigrationMatrix.hpp dep=:config/Buildinfo/VERSIONS,\
	:config/Persinfo/VERSIONS.persinfo setbuildinfo
#
# include dependencies (do not edit)
#
Kernel_Administration.cpp inc=\
    :SAPDB/KernelCommon/Kernel_Administration.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Admin.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Backup.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Recovery.h,\
    :SAPDB/RunTime/RTE_ISystem.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/HotStandbyStorage/RTEHSS_KernelInterface.hpp,\
    :SAPDB/RunTime/HotStandbyStorage/RTEHSS_StandbyNodeList.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/KernelCommon/Kernel_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_RestartPage.hpp,\
    :SAPDB/KernelCommon/Kernel_Task.hpp,\
    :SAPDB/IOManager/IOMan_IDataManager.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/IOManager/IOMan_IDataVolumeInfo.hpp,\
    :SAPDB/IOManager/IOMan_ILogVolumeInfo.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,\
    :SAPDB/IOManager/IOMan_ILogManager.hpp,\
    :SAPDB/IOManager/IOMan_ILogInfo.hpp,\
    :SAPDB/IOManager/IOMan_IDiagnose.hpp,\
    :SAPDB/IOManager/IOMan_IMigration.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_History.hpp,\
    :SAPDB/FrameControl/FrameCtrl_IIOManager.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/Converter/Converter_IBackUp.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/Converter/Converter_IManager.hpp,\
    :SAPDB/Converter/Converter_IDiagnose.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobCollection.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobRedo.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,gsp00,ggg92.h,ggg00,\
    hak10.h,hak36_1.h,hak93.h,hbd01.h,hbd01_1.h,hbd02.h,hbd03.h,hbd05.h,\
    hbd06.h,hbd10.h,hgg01_3.h,hgg17.h,hkb51.h,hkb53.h,hkb57.h,hkb57_1.h,\
    hkb38.h,hkb38_1.h,hkb900.h,hbd17.h,hbd20.h,heo51.h,heo60.h,heo79.h,\
    heo58.h,heo51.h,hsp20.h,heo47.h,hbd91.h,heo670.h,geo67.h
Kernel_Administration.hpp inc=\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminBackupRecovery.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminDiagnose.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_State.hpp,\
    :SAPDB/KernelCommon/Kernel_TaskWaitList.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/Restart/Rst_RedoManager.hpp,:SAPDB/Restart/Rst_Types.hpp
Kernel_BlobDescription.cpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_BlobDescription.hpp
Kernel_BlobDescription.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/Container/Container_Vector.hpp
Kernel_Common.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp
Kernel_DateTime.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h
Kernel_Dump.cpp inc=heo58.h,:SAPDB/KernelCommon/Kernel_Dump.hpp
Kernel_Dump.hpp inc=gsp00,:SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_DumpPage.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp
Kernel_DumpPage.hpp inc=:SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_Page.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp
Kernel_Exception.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp
Kernel_ExtendedState.cpp inc=\
    :SAPDB/KernelCommon/Kernel_ExtendedState.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h
Kernel_ExtendedState.hpp inc=:SAPDB/KernelCommon/Kernel_SharedState.h
Kernel_FileIO.cpp inc=heo51.h,heo55k.h,heo66.h,hsp77.h,hgg08.h,\
    :SAPDB/IOManager/IOMan_IPage.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Kernel_FileIO.hpp inc=gsp00,:SAPDB/KernelCommon/Kernel_Common.hpp
Kernel_IAdminBackupRecovery.hpp inc=gsp00,ggg00,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,:SAPDB/Restart/Rst_Types.hpp
Kernel_IAdminConfig.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,gsp00
Kernel_IAdminDiagnose.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Kernel_IAdminHotStandby.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,:SAPDB/Logging/Log_Types.hpp,\
    gsp00
Kernel_IAdminInfo.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp
Kernel_IAdminRestartShutdown.hpp inc=\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,:SAPDB/Logging/Log_Types.hpp,\
    gsp00,ggg00
Kernel_IPersistenceStream.hpp inc=
Kernel_IndexDescription.cpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/KernelCommon/Kernel_IndexDescription.hpp
Kernel_IndexDescription.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/Container/Container_Vector.hpp
Kernel_Initialization.cpp inc=\
    :SAPDB/KernelCommon/Kernel_Initialization.hpp,\
    :SAPDB/KernelCommon/Kernel_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Task.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_KernelCommon.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/Logging/Log_Volume.hpp,:SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,:SAPDB/Trace/Trace_Writer.hpp,\
    :SAPDB/Trace/Trace_Manager.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Coordinator.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Task.hpp,:SAPDB/Pager/Pager_Writer.hpp,\
    :SAPDB/KernelCommon/Kernel_Timeout.hpp,\
    :SAPDB/KernelCommon/Kernel_Administration.hpp,\
    :SAPDB/FrameControl/FrameCtrl_Manager.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Manager.hpp,\
    :SAPDB/IOManager/IOMan_Manager.hpp,\
    :SAPDB/Converter/Converter_Converter.hpp,\
    :SAPDB/Pager/Pager_Controller.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/liveCache/LVC_InProcDispatcher.hpp,\
    :SAPDB/SQLManager/SQLMan_UpdateStatistics.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,ggg00,hgg01.h,\
    hgg01_3.h,hgg02.h,hgg03.h,hgg08.h,hgg011.h,hgg941.h,hak01.h,hak101.h,\
    hak05.h,hak10.h,hak260.h,hak362.h,hak40.h,hak41.h,hak506.h,hak509.h,\
    hak51.h,hak54.h,hak545.h,hak642.h,hak728.h,hak71.h,hak720.h,hak92.h,\
    hak96.h,hak99.h,hkb05.h,hkb71.h,gbd490.h,gbd493.h,gbd930.h,hbd01.h,\
    hbd01_1.h,hbd06.h,hbd91.h,hkb38.h,hkb391.h,hkb57.h,hkb571.h,hta01.h,\
    heo51.h
Kernel_Initialization.hpp inc=ggg00
Kernel_Main.cpp inc=hak102.h,hak93.h,heo51.h,heo58.h,vak001,vsp001,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Main.hpp,\
    :SAPDB/KernelCommon/Kernel_Task.hpp,\
    :SAPDB/KernelCommon/Kernel_Initialization.hpp,\
    :SAPDB/RunTime/RTE_OldKernelCommunication.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp
Kernel_Main.hpp inc=
Kernel_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Kernel_Migration.cpp inc=:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/KernelCommon/Kernel_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_MigrationMatrix.hpp,\
    :SAPDB/Container/Container_List.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpgradeHistory.hpp,heo52.h
Kernel_Migration.hpp inc=:SAPDB/KernelCommon/Kernel_Version.hpp
Kernel_MigrationMatrix.hpp inc=
Kernel_OpMsg.cpp inc=heo60.h,:SAPDB/KernelCommon/Kernel_OpMsg.hpp
Kernel_OpMsg.hpp inc=ggg00,gsp03,:SAPDB/SAPDBCommon/SAPDB_OStream.hpp
Kernel_Page.hpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_PersistentTypes.hpp,\
    :SAPDB/IOManager/IOMan_IPage.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp
Kernel_Page72.cpp inc=:SAPDB/KernelCommon/Kernel_Page72.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    hgg17.h
Kernel_Page72.hpp inc=:SAPDB/KernelCommon/Kernel_Page.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_PersistentTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp
Kernel_Page74.cpp inc=:SAPDB/KernelCommon/Kernel_Page74.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Kernel_Page74.hpp inc=:SAPDB/KernelCommon/Kernel_Page.hpp
Kernel_Random.cpp inc=:SAPDB/KernelCommon/Kernel_Random.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_IWorkerJob.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,heo55k.h,\
    hgg08.h
Kernel_Random.hpp inc=:SAPDB/RunTime/Security/Crypto/RTECrypto_RNG.hpp
Kernel_RestartPage.cpp inc=ggg00,hgg01_3.h,\
    :SAPDB/KernelCommon/Kernel_Page.hpp,\
    :SAPDB/KernelCommon/Kernel_RestartPage.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Kernel_RestartPage.hpp inc=gkb00,:SAPDB/KernelCommon/Kernel_Page72.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Kernel_SharedState.h inc=:SAPDB/SAPDBCommon/SAPDB_Types.h
Kernel_SnapShot.cpp inc=:SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FrameControl/FrameCtrl_IRestartPage.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_RestartPage.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/KernelCommon/Kernel_SnapShot.hpp
Kernel_SnapShot.hpp inc=:SAPDB/Converter/Converter_Version.hpp,gkb00,\
    hkb57_1.h
Kernel_State.hpp inc=\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp
Kernel_SynchronizedDBFullList.hpp inc=\
    :SAPDB/KernelCommon/Kernel_SynchronizedTaskWaitList.hpp,\
    :SAPDB/KernelCommon/Kernel_ExtendedState.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Kernel_SynchronizedTaskWaitList.cpp inc=heo56.h,heo55k.h,hgg08.h,\
    :SAPDB/KernelCommon/Kernel_SynchronizedTaskWaitList.hpp
Kernel_SynchronizedTaskWaitList.hpp inc=\
    :SAPDB/KernelCommon/Kernel_TaskWaitList.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Kernel_TableStatistic.hpp inc=ggg00,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
Kernel_Task.hpp inc=:SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_KernelCommon.h,\
    :SAPDB/KernelCommon/Kernel_Types.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
Kernel_TaskWaitList.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,gsp00,ggg00,heo51.h,heo56.h
Kernel_Test.cpp inc=:SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_Test.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Messages/Msg_List.hpp
Kernel_Test.hpp inc=
Kernel_Timeout.cpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Timeout.hpp,ggg00,heo52.h,ggg92.h,hkb51.h,\
    hak341.h,hbd91.h,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobDropAuxFiles.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp
Kernel_Timeout.hpp inc=
Kernel_Trace.cpp inc=:SAPDB/KernelCommon/Kernel_Trace.hpp,gsp00,ggg00,\
    hgg17.h,:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_KernelCommon.h
Kernel_Trace.hpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Kernel_TraceTopics.cpp inc=gsp00,ggg00,hgg941.h,heo51.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/KernelCommon/Kernel_TraceTopics.hpp
Kernel_TraceTopics.hpp inc=
Kernel_Types.hpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp
Kernel_VTrace.cpp inc=heo51.h,:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp
Kernel_VTrace.hpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_OStream.hpp
Kernel_Version.cpp inc=hgg11.h,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_KernelCommon.h,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,gsp100.h
Kernel_Version.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,gsp00

