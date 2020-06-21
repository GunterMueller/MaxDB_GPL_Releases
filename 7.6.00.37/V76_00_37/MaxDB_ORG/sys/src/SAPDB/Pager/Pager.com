
#
# include dependencies (do not edit)
#
Pager_Controller.cpp inc=:SAPDB/Pager/Pager_Controller.hpp,\
    :SAPDB/Pager/Pager_Exception.hpp,:SAPDB/Pager/Pager_Messages.hpp,\
    :SAPDB/Pager/Pager_Lock.hpp,:SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,\
    :SAPDB/Converter/Converter_ISavePoint.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_Pager.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_SuspendReason.h,ggg00,gbd00,hgg01.h,\
    heo56.h,hgg04.h,hbd20_5.h,gbd201.h
Pager_Controller.hpp inc=gsp00,gbd00,gbd02,gbd201.h,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Pager/Pager_IRestart.hpp,:SAPDB/Pager/Pager_ISavepoint.hpp,\
    :SAPDB/Pager/Pager_IDump.hpp,:SAPDB/Pager/Pager_Dump.hpp,\
    :SAPDB/Pager/Pager_Writer.hpp,:SAPDB/Pager/Pager_Types.hpp
Pager_Dump.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Pager_Error.hpp inc=:SAPDB/Pager/Pager_Exception.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
Pager_Exception.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Pager_IDump.hpp inc=gsp00,:SAPDB/KernelCommon/Kernel_Common.hpp
Pager_IRestart.hpp inc=gsp00
Pager_ISavepoint.hpp inc=gsp00,:SAPDB/KernelCommon/Kernel_Common.hpp
Pager_Lock.cpp inc=:SAPDB/Pager/Pager_Lock.hpp,heo55k.h,hgg08.h
Pager_Lock.hpp inc=gsp00
Pager_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Pager_Types.hpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp
Pager_Wrapper.cpp inc=:SAPDB/Pager/Pager_Controller.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp
Pager_Writer.cpp inc=:SAPDB/Pager/Pager_Writer.hpp,\
    :SAPDB/Pager/Pager_Types.hpp,:SAPDB/Pager/Pager_Controller.hpp,\
    :SAPDB/Pager/Pager_Exception.hpp,:SAPDB/Pager/Pager_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/Converter/Converter_IPageIO.hpp,heo56.h,hbd20_5.h,hgg04.h,\
    gbd201.h
Pager_Writer.hpp inc=ggg00,gbd00,gbd02,hgg01.h,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_DataPages.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp

