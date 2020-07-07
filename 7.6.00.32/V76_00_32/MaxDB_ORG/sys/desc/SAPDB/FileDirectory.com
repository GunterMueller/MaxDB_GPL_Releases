
#
# include dependencies (do not edit)
#
FileDir_BaseEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_BaseEntry.hpp,ggg91.h
FileDir_BaseEntry.hpp inc=:SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_BucketLocks.cpp inc=\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_BucketLocks.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_BucketLocks.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Common.cpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,:SAPDB/RunTime/RTE_Crash.hpp
FileDir_Common.hpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp
FileDir_Directory.cpp inc=:SAPDB/FileDirectory/FileDir_HashBucket.hpp,\
    :SAPDB/FileDirectory/FileDir_Directory.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_GenericDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_TableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_TempTableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumnEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartitionEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVarEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixedEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVar.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixed.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/Converter/Converter_IPageNoManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_IteratorObj.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_IndexDescription.hpp,heo51.h,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg91.h,ggg00,hgg01_3.h
FileDir_Directory.hpp inc=gbd00,ggg00,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_GenericDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_PointerCache.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hkb50.h
FileDir_Entry.cpp inc=:SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg91.h,hgg06.h,hgg01_3.h,heo51.h,\
    hbd300.h
FileDir_Entry.hpp inc=:SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,ggg91.h,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Exception.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
FileDir_FileNo.cpp inc=:SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_OStream.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_FileNo.hpp inc=ggg90.h,ggg00,hsp77.h,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Generic.cpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,ggg91.h,heo51.h,\
    :SAPDB/Converter/Converter_Version.hpp
FileDir_Generic.hpp inc=:SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    ggg91.h,:SAPDB/Converter/Converter_Version.hpp
FileDir_GenericDirectory.cpp inc=\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_TableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_TempTableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumnEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartitionEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVarEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixedEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVar.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixed.hpp,\
    :SAPDB/FileDirectory/FileDir_BucketLocks.hpp,\
    :SAPDB/FileDirectory/FileDir_HashBucket.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_GenericDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,heo51.h,ggg00,hbd01_1.h,hgg01_3.h
FileDir_GenericDirectory.hpp inc=\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_BucketLocks.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp
FileDir_HashBucket.cpp inc=:SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_HashBucket.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_HashBucket.hpp inc=:SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/RunTime/RTE_Types.hpp
FileDir_HotSpinRWRegion.cpp inc=\
    :SAPDB/FileDirectory/FileDir_HotSpinRWRegion.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp
FileDir_HotSpinRWRegion.hpp inc=:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_IDirectory.hpp inc=:SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,ggg91.h,ggg00
FileDir_IIndex.hpp inc=:SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp
FileDir_IOms.hpp inc=:SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_IOmsFixed.hpp inc=:SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp
FileDir_IOmsKey.hpp inc=:SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp
FileDir_IOmsKeyPartition.hpp inc=:SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp
FileDir_IOmsVar.hpp inc=:SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp
FileDir_ISharedDirectory.hpp inc=:SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_IShortColumn.hpp inc=:SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp
FileDir_ITable.hpp inc=:SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Index.cpp inc=:SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_IndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IIndex.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Index.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_IIndex.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp
FileDir_IndexEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_IndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd300.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,hgg01_3.h
FileDir_IndexEntry.hpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_IIndex.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Iterator.cpp inc=:SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_IteratorObj.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Iterator.hpp inc=:SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_IteratorObj.cpp inc=:SAPDB/FileDirectory/FileDir_IteratorObj.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_BucketLocks.hpp,\
    :SAPDB/FileDirectory/FileDir_HashBucket.hpp,\
    :SAPDB/FileDirectory/FileDir_Directory.hpp,\
    :SAPDB/FileDirectory/FileDir_GenericDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,heo51.h
FileDir_IteratorObj.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
FileDir_Oms.cpp inc=:SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOms.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsVar.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Oms.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_IOms.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gbd900.h
FileDir_OmsEntry.hpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOms.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsFixed.cpp inc=:SAPDB/FileDirectory/FileDir_OmsFixed.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixedEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsFixed.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsFixed.hpp inc=:SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsFixed.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsFixedEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixedEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
FileDir_OmsFixedEntry.hpp inc=:SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsFixed.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,ggg00
FileDir_OmsKey.cpp inc=:SAPDB/FileDirectory/FileDir_OmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsKey.hpp inc=:SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsKeyEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,heo51.h
FileDir_OmsKeyEntry.hpp inc=:SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
FileDir_OmsKeyPartition.cpp inc=\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartitionEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsKeyPartition.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp
FileDir_OmsKeyPartitionEntry.cpp inc=\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartitionEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gbd900.h
FileDir_OmsKeyPartitionEntry.hpp inc=\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsVar.cpp inc=:SAPDB/FileDirectory/FileDir_OmsVar.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVarEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsVar.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsVar.hpp inc=:SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsVar.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_OmsVarEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVarEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,heo51.h,hbd01_1.h
FileDir_OmsVarEntry.hpp inc=:SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IOmsVar.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
FileDir_Page.cpp inc=:SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FrameControl/FrameCtrl_IFileDirectory.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,hbd01_1.h,hbd13.h,ggg00,heo51.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Page.hpp inc=:SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_PageIndexPage.cpp inc=\
    :SAPDB/FileDirectory/FileDir_PageIndexPage.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/IOManager/IOMan_IDataIO.hpp,ggg00,hbd01_1.h,hbd13.h,\
    :SAPDB/FrameControl/FrameCtrl_IFileDirectory.hpp,heo51.h,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_PageIndexPage.hpp inc=:SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,gsp00,ggg00,\
    :SAPDB/Container/Container_Hash.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_PageManager.cpp inc=:SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_PageIndexPage.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/IOManager/IOMan_IDataIO.hpp,\
    :SAPDB/Converter/Converter_IPageNoManager.hpp,\
    :SAPDB/Converter/Converter_IVerify.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_PageManager.hpp inc=:SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_PageIndexPage.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/Container/Container_Hash.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_PointerCache.cpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartition.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVar.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixed.hpp,\
    :SAPDB/FileDirectory/FileDir_PointerCache.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp
FileDir_PointerCache.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/Container/Container_Hash.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp
FileDir_RWRegionWrapper.cpp inc=\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_FileDir.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_RWRegionWrapper.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp
FileDir_SharedDirectory.cpp inc=\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_TableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumnEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_SingleIndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKeyPartitionEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVarEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixedEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_IteratorObj.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_HashBucket.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/Converter/Converter_IPageNoManager.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/KernelCommon/Kernel_IndexDescription.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,heo51.h,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hkb57.h,hbd17.h,hgg11.h,ggg00,\
    gbd00,hbd17.h
FileDir_SharedDirectory.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_SysKeyCache.hpp,\
    :SAPDB/FileDirectory/FileDir_GenericDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,gsp00
FileDir_ShortColumn.cpp inc=:SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumnEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_IShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_ShortColumn.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_IShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp
FileDir_ShortColumnEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumnEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd300.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,hgg01_3.h
FileDir_ShortColumnEntry.hpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_IShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_SingleIndexEntry.cpp inc=\
    :SAPDB/FileDirectory/FileDir_SingleIndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd300.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,hgg01_3.h
FileDir_SingleIndexEntry.hpp inc=\
    :SAPDB/FileDirectory/FileDir_IndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_SysKeyCache.cpp inc=:SAPDB/FileDirectory/FileDir_SysKeyCache.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/Container/Container_Hash.hpp,ggg00,hsp30.h
FileDir_SysKeyCache.hpp inc=ggg00,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_RWRegionWrapper.hpp,\
    :SAPDB/Container/Container_Hash.hpp
FileDir_Table.cpp inc=:SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_TableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_ITable.hpp,\
    :SAPDB/FileDirectory/FileDir_SysKeyCache.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    ggg00
FileDir_Table.hpp inc=:SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_ITable.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    ggg00
FileDir_TableEntry.cpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_TableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/FileDirectory/FileDir_Page.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,heo51.h,\
    :SAPDB/RunTime/RTE_Types.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd300.h,ggg00,gbd00
FileDir_TableEntry.hpp inc=:SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_ITable.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,ggg91.h,\
    :SAPDB/RunTime/RTE_Types.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    ggg00
FileDir_TempEntry.cpp inc=:SAPDB/FileDirectory/FileDir_TempEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg91.h,hgg06.h
FileDir_TempEntry.hpp inc=:SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,ggg91.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_TempTableEntry.cpp inc=\
    :SAPDB/FileDirectory/FileDir_TempEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_TempTableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd300.h,gbd00
FileDir_TempTableEntry.hpp inc=\
    :SAPDB/FileDirectory/FileDir_TempEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_ITable.hpp,\
    :SAPDB/FileDirectory/FileDir_Vector.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg91.h,ggg00
FileDir_Test.cpp inc=:SAPDB/FileDirectory/FileDir_OmsVar.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsKey.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsFixed.hpp,\
    :SAPDB/FileDirectory/FileDir_Oms.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_IndexEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_TableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_OmsVarEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_Directory.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_PageManager.hpp,\
    :SAPDB/KernelCommon/Kernel_BlobDescription.hpp,\
    :SAPDB/KernelCommon/Kernel_IndexDescription.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_IPageNoManager.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,heo51.h,hkb50.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/FileDirectory/FileDir_Test.hpp,ggg91.h,gsp00,ggg00
FileDir_Test.hpp inc=:SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,ggg00
FileDir_Types.hpp inc=:SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
FileDir_Vector.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/FileDirectory/FileDir_Common.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
FileDir_Wrapper.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Instance.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_SharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_FileDir.h,\
    :SAPDB/FileDirectory/FileDir_BaseEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Entry.hpp,\
    :SAPDB/FileDirectory/FileDir_TableEntry.hpp,\
    :SAPDB/FileDirectory/FileDir_Exception.hpp,\
    :SAPDB/FileDirectory/FileDir_Messages.hpp,hbd998.h,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gbd00,ggg00,gsp00,hkb50.h,hkb57.h,\
    hkb64.h,hkb900.h,hbd30.h,hbd01_1.h,hgg06.h,hgg17.h,ggg91.h,heo51.h,\
    hbd01.h,hbd13.h,hbd17.h,ggg11,hgg04.h,hgg01_3.h,hta01.h,\
    :SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp
FileDir_Wrapper.hpp inc=:SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.h,ggg00

