
#
# include dependencies (do not edit)
#
SrvTasks_Coordinator.cpp inc=:SAPDB/ServerTasks/SrvTasks_Coordinator.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Task.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,hkb38.h,hkb900.h,heo55k.h,hgg08.h,\
    hgg01.h
SrvTasks_Coordinator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Task.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
SrvTasks_ICoordinator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
SrvTasks_ITask.hpp inc=
SrvTasks_ITaskForJob.hpp inc=:SAPDB/RunTime/Tasking/RTETask_ITask.hpp
SrvTasks_Job.cpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,ggg00,hkb53.h,heo55k.h,hgg08.h,\
    vsp002
SrvTasks_Job.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobTypes.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
SrvTasks_JobBackupRestore.cpp inc=\
    :SAPDB/ServerTasks/SrvTasks_JobBackupRestore.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Recovery.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,heo55k.h,hgg08.h,hkb03_1.h,\
    hkb38.h,hkb39.h,ggg00,hkb900.h
SrvTasks_JobBackupRestore.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    ggg00
SrvTasks_JobCheckData.cpp inc=\
    :SAPDB/ServerTasks/SrvTasks_JobCheckData.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,hbd01.h,heo56.h
SrvTasks_JobCheckData.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,ggg00
SrvTasks_JobCollectTableStatistics.cpp inc=\
    :SAPDB/SQLManager/SQLMan_UpdateStatistics.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobCollectTableStatistics.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobTypes.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/DataAccess/Data_PrimTreeStatisticsCoordinator.hpp,\
    :SAPDB/DataAccess/Data_RandomTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_ScanTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_KeyBoundedPageAccess.hpp,\
    :SAPDB/DataAccess/Data_PrimTreeStatistic.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd30.h,hbd01.h,gbd00,gsp00,ggg00
SrvTasks_JobCollectTableStatistics.hpp inc=\
    :SAPDB/DataAccess/Data_PrimTreeStatisticsCoordinator.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Job.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gsp00
SrvTasks_JobCollection.cpp inc=\
    :SAPDB/ServerTasks/SrvTasks_JobCollection.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobPrefetchObjects.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,hbd01.h,hbd02.h,hbd91.h,hbd05.h,\
    hkb53.h,ggg00
SrvTasks_JobCollection.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,ggg00
SrvTasks_JobCreateIndex.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,ggg00,hbd03.h
SrvTasks_JobDropAuxFiles.cpp inc=\
    :SAPDB/ServerTasks/SrvTasks_JobDropAuxFiles.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Task.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,ggg00,hkb64.h,hbd01.h,hkb900.h,\
    heo55k.h,hgg08.h,hbd02.h,hbd01_1.h,hbd998.h
SrvTasks_JobDropAuxFiles.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    ggg00
SrvTasks_JobDropVolume.cpp inc=\
    :SAPDB/ServerTasks/SrvTasks_JobDropVolume.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/IOManager/IOMan_IDataManager.hpp
SrvTasks_JobDropVolume.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/Converter/Converter_PageNoIterator.hpp,ggg00
SrvTasks_JobJoinInvSelector.cpp inc=\
    :SAPDB/ServerTasks/SrvTasks_JobJoinInvSelector.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
SrvTasks_JobJoinInvSelector.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp
SrvTasks_JobList.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Job.hpp,heo55k.h,hgg08.h
SrvTasks_JobPrefetchObjects.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,ggg00,hkb53.h,hbd04.h
SrvTasks_JobRedo.cpp inc=:SAPDB/ServerTasks/SrvTasks_JobRedo.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,ggg00,\
    :SAPDB/Restart/Rst_RedoManager.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp
SrvTasks_JobRedo.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp
SrvTasks_JobSavepoint.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,ggg00,\
    :SAPDB/Logging/Log_Transaction.hpp,:SAPDB/Logging/Log_Savepoint.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
SrvTasks_JobTypes.cpp inc=:SAPDB/ServerTasks/SrvTasks_JobTypes.hpp
SrvTasks_JobTypes.hpp inc=:SAPDB/RunTime/Tasking/RTETask_ITask.hpp
SrvTasks_Task.cpp inc=:SAPDB/ServerTasks/SrvTasks_Task.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Coordinator.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_SuspendReason.h,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,heo55k.h,hgg08.h
SrvTasks_Task.hpp inc=:SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITask.hpp
SrvTasks_Test.cpp inc=:SAPDB/ServerTasks/SrvTasks_Test.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/Logging/Log_Savepoint.hpp
SrvTasks_Test.hpp inc=
SrvTasks_Types.hpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp
SrvTasks_WaitingTask.cpp inc=:SAPDB/ServerTasks/SrvTasks_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_SuspendReason.h
SrvTasks_WaitingTask.hpp inc=\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp

