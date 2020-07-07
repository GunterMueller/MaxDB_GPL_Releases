&if $OSSPEC == LINUX && $MACH in [ SDBonPPC64 ]
IOMan_Manager.cpp -OPT=-O1
&endif


#
# include dependencies (do not edit)
#
IOMan_AsynIOHandle.hpp inc=:SAPDB/KernelCommon/Kernel_Common.hpp,gkb03
IOMan_BlockAddress.hpp inc=:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp
IOMan_ClusterAddress.hpp inc=:SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp
IOMan_ComplexClusterAddress.hpp inc=\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,gbd00
IOMan_ConverterPages.hpp inc=:SAPDB/Converter/Converter_Page.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp
IOMan_DataArea.cpp inc=hkb57.h,hgg01_3.h,hgg04.h,\
    :SAPDB/IOManager/IOMan_DataArea.hpp,\
    :SAPDB/IOManager/IOMan_AsynIOHandle.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,\
    :SAPDB/IOManager/IOMan_MessageList.hpp,\
    :SAPDB/IOManager/IOMan_ComplexClusterAddress.hpp,\
    :SAPDB/Converter/Converter_IManager.hpp,\
    :SAPDB/Converter/Converter_IBackUp.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/DataAccess/Data_IOBuffer.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/Messages/Msg_IOutput.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_VolumeInfo.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_IOMan.h,\
    :SAPDB/Trace/Trace_DevSpaceIOEntry.hpp
IOMan_DataArea.hpp inc=gsp00,:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_DataPages.hpp,\
    :SAPDB/IOManager/IOMan_DataVolume.hpp,\
    :SAPDB/IOManager/IOMan_PageFrameAllocator.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp
IOMan_DataPages.cpp inc=:SAPDB/IOManager/IOMan_DataPages.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
IOMan_DataPages.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_PageNoList.hpp,\
    :SAPDB/DataAccess/Data_BasePage.hpp,:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp
IOMan_DataVolume.cpp inc=:SAPDB/IOManager/IOMan_DataVolume.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/IOManager/IOMan_ComplexClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_Exception.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,:SAPDB/Messages/Msg_IOutput.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_IOMan.h
IOMan_DataVolume.hpp inc=gsp00,vsp002,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/IOManager/IOMan_Volume.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,:SAPDB/IOManager/IOMan_IPage.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
IOMan_DualLogDevice.cpp inc=vsp002,heo54k.h,\
    :SAPDB/IOManager/IOMan_Exception.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,\
    :SAPDB/IOManager/IOMan_DualLogDevice.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_IOMan.h
IOMan_DualLogDevice.hpp inc=gsp00,:SAPDB/IOManager/IOMan_LogDevice.hpp,\
    :SAPDB/IOManager/IOMan_LogVolume.hpp,\
    :SAPDB/IOManager/IOMan_MirrorLogVolume.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_Exception.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
IOMan_IBlockAddressIterator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp
IOMan_IConverterIO.hpp inc=
IOMan_IDataIO.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_PageNoList.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/IOManager/IOMan_DataPages.hpp
IOMan_IDataInfo.hpp inc=:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_IDataManager.hpp inc=:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/IOManager/IOMan_ReservedBlockAddress.hpp
IOMan_IDataVolumeInfo.cpp inc=:SAPDB/IOManager/IOMan_IDataVolumeInfo.hpp,\
    :SAPDB/IOManager/IOMan_Manager.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp
IOMan_IDataVolumeInfo.hpp inc=gsp00,:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_IDiagnose.hpp inc=gsp00,:SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp
IOMan_ILogIO.hpp inc=:SAPDB/IOManager/IOMan_LogPages.hpp
IOMan_ILogInfo.hpp inc=:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_ILogManager.hpp inc=:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/IOManager/IOMan_LogPages.hpp
IOMan_ILogVolumeInfo.cpp inc=:SAPDB/IOManager/IOMan_ILogVolumeInfo.hpp,\
    :SAPDB/IOManager/IOMan_Manager.hpp
IOMan_ILogVolumeInfo.hpp inc=gsp00,:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_IMigration.hpp inc=
IOMan_IPage.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
IOMan_InfoPage.hpp inc=:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_Page74.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
IOMan_KernelPages.hpp inc=gsp00,:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/IOManager/IOMan_IPage.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
IOMan_LogArea.cpp inc=:SAPDB/IOManager/IOMan_LogArea.hpp,\
    :SAPDB/IOManager/IOMan_MessageList.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,\
    :SAPDB/IOManager/IOMan_ReservedBlockAddress.hpp,\
    :SAPDB/Logging/Log_Page.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_VolumeInfo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
IOMan_LogArea.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_LogDevice.hpp,\
    :SAPDB/IOManager/IOMan_LogPages.hpp,\
    :SAPDB/IOManager/IOMan_LogDevices.hpp,\
    :SAPDB/IOManager/IOMan_PageFrameAllocator.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,:SAPDB/Logging/Log_Types.hpp
IOMan_LogDevice.hpp inc=gsp00,:SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_InfoPage.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp,\
    :SAPDB/IOManager/IOMan_LogVolume.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,:SAPDB/Logging/Log_Types.hpp
IOMan_LogDevices.cpp inc=:SAPDB/IOManager/IOMan_LogDevices.hpp,\
    :SAPDB/IOManager/IOMan_SingleLogDevice.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
IOMan_LogDevices.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/IOManager/IOMan_LogDevice.hpp,\
    :SAPDB/IOManager/IOMan_DualLogDevice.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
IOMan_LogPages.hpp inc=:SAPDB/Logging/Log_Page.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp
IOMan_LogVolume.cpp inc=heo56.h,:SAPDB/IOManager/IOMan_LogVolume.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/IOManager/IOMan_Exception.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
IOMan_LogVolume.hpp inc=gsp00,vsp002,\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_Volume.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/IOManager/IOMan_IPage.hpp,:SAPDB/Logging/Log_Types.hpp
IOMan_Manager.cpp inc=ggg00,ggg01,hbd73.h,hkb57.h,hgg01_3.h,vsp002,\
    heo58.h,heo79_1.h,hbd01_1.h,hbd13_3.h,\
    :SAPDB/FreeBlockManagement/FBM_Types.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/Converter/Converter_IPageNoMapping.hpp,\
    :SAPDB/Converter/Converter_ISavePoint.hpp,\
    :SAPDB/Converter/Converter_IDropVolume.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/DataAccess/Data_PageNoList.hpp,\
    :SAPDB/IOManager/IOMan_MessageList.hpp,\
    :SAPDB/IOManager/IOMan_Manager.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,\
    :SAPDB/IOManager/IOMan_Migration.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/IOManager/IOMan_ComplexClusterAddress.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_IOManager.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_RunTime.h,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobDropVolume.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/Trace/Trace_DevSpaceIOEntry.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
IOMan_Manager.hpp inc=gsp00,:SAPDB/IOManager/IOMan_IConverterIO.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,\
    :SAPDB/IOManager/IOMan_IDataManager.hpp,\
    :SAPDB/IOManager/IOMan_IDiagnose.hpp,\
    :SAPDB/IOManager/IOMan_ILogInfo.hpp,\
    :SAPDB/IOManager/IOMan_ILogManager.hpp,\
    :SAPDB/IOManager/IOMan_ILogIO.hpp,\
    :SAPDB/IOManager/IOMan_IMigration.hpp,\
    :SAPDB/Converter/Converter_Page.hpp,\
    :SAPDB/DataAccess/Data_BasePage.hpp,:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_DataArea.hpp,\
    :SAPDB/IOManager/IOMan_ConverterPages.hpp,\
    :SAPDB/IOManager/IOMan_DataPages.hpp,\
    :SAPDB/IOManager/IOMan_LogArea.hpp,\
    :SAPDB/IOManager/IOMan_LogPages.hpp,\
    :SAPDB/IOManager/IOMan_PageFrameAllocator.hpp,\
    :SAPDB/IOManager/IOMan_ReservedBlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_RestartPage.hpp,\
    :SAPDB/Logging/Log_ClusterAddress.hpp,\
    :SAPDB/Logging/Log_InfoPage.hpp,:SAPDB/Logging/Log_Page.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
IOMan_MessageList.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
IOMan_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
IOMan_Migration.cpp inc=ggg00,vsp002,hkb57.h,hgg08.h,heo54k.h,heo58.h,\
    heo79_1.h,:SAPDB/Converter/Converter_IManager.hpp,\
    :SAPDB/Converter/Converter_IMigration.hpp,\
    :SAPDB/Converter/Converter_IPageNoMapping.hpp,\
    :SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/IOManager/IOMan_Exception.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,\
    :SAPDB/IOManager/IOMan_Migration.hpp,\
    :SAPDB/IOManager/IOMan_ReservedBlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_DataArea.hpp,\
    :SAPDB/IOManager/IOMan_LogArea.hpp,\
    :SAPDB/IOManager/IOMan_Manager.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_RestartPage.hpp,\
    :SAPDB/Logging/Log_Volume.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
IOMan_Migration.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Converter/Converter_Page72.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_PageFrameAllocator.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/KernelCommon/Kernel_Page72.hpp
IOMan_MirrorLogVolume.cpp inc=:SAPDB/IOManager/IOMan_MirrorLogVolume.hpp,\
    :SAPDB/IOManager/IOMan_ReservedBlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_MessageList.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
IOMan_MirrorLogVolume.hpp inc=gsp00,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp,\
    :SAPDB/IOManager/IOMan_LogVolume.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_PageFrameAllocator.hpp inc=gsp00,\
    :SAPDB/FrameControl/FrameCtrl_IIOManager.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_ReservedBlockAddress.hpp inc=\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
IOMan_SingleLogDevice.cpp inc=:SAPDB/IOManager/IOMan_Exception.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,\
    :SAPDB/IOManager/IOMan_SingleLogDevice.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
IOMan_SingleLogDevice.hpp inc=:SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_LogDevice.hpp,\
    :SAPDB/IOManager/IOMan_LogVolume.hpp,\
    :SAPDB/IOManager/IOMan_InfoPage.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,:SAPDB/Logging/Log_Types.hpp
IOMan_Types.hpp inc=:SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp
IOMan_Volume.cpp inc=hgg08.h,heo54k.h,heo55k.h,heo56.h,heo58.h,\
    :SAPDB/IOManager/IOMan_Volume.hpp,\
    :SAPDB/IOManager/IOMan_MessageList.hpp,\
    :SAPDB/IOManager/IOMan_Messages.hpp,\
    :SAPDB/IOManager/IOMan_ReservedBlockAddress.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_IOMan.h
IOMan_Volume.hpp inc=gsp00,vsp002,\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,:SAPDB/IOManager/IOMan_InfoPage.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_VolumeInfo.hpp
IOMan_Wrapper.cpp inc=gsp00,ggg00,gbd00,gsp03,hgg04.h,gkb03,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/DataAccess/Data_BasePage.hpp,:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ILog.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,\
    :SAPDB/IOManager/IOMan_IDataManager.hpp,\
    :SAPDB/IOManager/IOMan_IDiagnose.hpp,\
    :SAPDB/IOManager/IOMan_ILogInfo.hpp,\
    :SAPDB/IOManager/IOMan_ILogManager.hpp,\
    :SAPDB/IOManager/IOMan_IMigration.hpp,\
    :SAPDB/IOManager/IOMan_AsynIOHandle.hpp,\
    :SAPDB/IOManager/IOMan_KernelPages.hpp,\
    :SAPDB/IOManager/IOMan_Manager.hpp,:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/IOManager/IOMan_MessageList.hpp,\
    :SAPDB/IOManager/IOMan_IDataVolumeInfo.hpp,\
    :SAPDB/Logging/Log_Page.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_Volume.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp

