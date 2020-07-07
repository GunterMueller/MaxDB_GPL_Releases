#
# include dependencies (do not edit)
#
CnsRead_CachedLeafPage.cpp inc=\
    :SAPDB/ConsistentRead/CnsRead_CachedLeafPage.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/ConsistentRead/CnsRead_MarshaledTransHistoryInfo.hpp
CnsRead_CachedLeafPage.hpp inc=gbd00
CnsRead_CachedRow.cpp inc=:SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/DataAccess/Data_Row.hpp,:SAPDB/Logging/Log_History.hpp,\
    hkb50.h,hkb04.h,hkb71.h
CnsRead_CachedRow.hpp inc=\
    :SAPDB/ConsistentRead/CnsRead_SwizzledPointer.hpp,\
    :SAPDB/ConsistentRead/CnsRead_TransHistoryTab.hpp,\
    :SAPDB/ConsistentRead/CnsRead_SeparatedRowHeader.hpp
CnsRead_Manager.cpp inc=:SAPDB/ConsistentRead/CnsRead_Manager.hpp,\
    hgg01_3.h,hbd01.h,hbd02.h,hgg04.h,hbd998.h,hkb50.h,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Messages.hpp,\
    :SAPDB/ConsistentRead/CnsRead_TransHistoryTab.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp
CnsRead_Manager.hpp inc=ggg00
CnsRead_MarshaledTransHistoryInfo.cpp inc=\
    :SAPDB/ConsistentRead/CnsRead_MarshaledTransHistoryInfo.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Messages.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
CnsRead_MarshaledTransHistoryInfo.hpp inc=ggg00,\
    :SAPDB/ConsistentRead/CnsRead_TransHistoryInfo.hpp
CnsRead_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
CnsRead_PascalWrapper.cpp inc=hbd999_1.h,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/ConsistentRead/CnsRead_CachedLeafPage.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp
CnsRead_SeparatedRowHeader.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
CnsRead_SwizzledPointer.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
CnsRead_Test.cpp inc=:SAPDB/ConsistentRead/CnsRead_Test.hpp,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/KernelCommon/MemoryManagement/KernelMem_DataCacheAllocator.hpp
CnsRead_Test.hpp inc=
CnsRead_TransHistoryInfo.cpp inc=\
    :SAPDB/ConsistentRead/CnsRead_TransHistoryInfo.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp
CnsRead_TransHistoryInfo.hpp inc=ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
CnsRead_TransHistoryTab.cpp inc=\
    :SAPDB/ConsistentRead/CnsRead_TransHistoryTab.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/KernelCommon/MemoryManagement/KernelMem_DataCacheAllocator.hpp,\
    hkb50.h
CnsRead_TransHistoryTab.hpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ConsistentRead/CnsRead_SeparatedRowHeader.hpp,\
    :SAPDB/ConsistentRead/CnsRead_TransHistoryInfo.hpp

