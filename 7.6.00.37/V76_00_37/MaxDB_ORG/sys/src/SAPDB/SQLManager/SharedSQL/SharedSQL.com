# ------------------------------------------------------------------
# contents: 	Dependencies of files in 
# 		        component 'SAPDB/SQLManager/SharedSQL/'.
# author:	Dirk Thomsen
# created:	Fri Jan 17 15:27:12  2003
# updated:	Fri Jan 17 17:21:07  2003
# to do:	Reduce the dependencies by using forward declarations!
# ------------------------------------------------------------------
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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

# include dependencies (do not edit)
#
SharedSQL_Command.cpp
SharedSQL_Command.hpp
SharedSQL_CommandCache.cpp
SharedSQL_CommandCache.hpp
SharedSQL_CommandCacheIterator.cpp
SharedSQL_CommandCacheIterator.hpp
SharedSQL_CommandCacheStatistics.hpp
SharedSQL_CommandCache_Wrapper.cpp
SharedSQL_CommandInfo.hpp
SharedSQL_CommandStatistics.hpp
SharedSQL_Handles.cpp
SharedSQL_Handles.hpp
SharedSQL_Messages.hpp
SharedSQL_ParseIDCache.hpp
SharedSQL_Plan.hpp
SharedSQL_SQLCache.cpp
SharedSQL_SQLCache.hpp
SharedSQL_Types.hpp

#
# include dependencies (do not edit)
#
SharedSQL_Command.cpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp
SharedSQL_Command.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandInfo.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Plan.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,heo58.h
SharedSQL_CommandCache.cpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Plan.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_SharedSQL.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RawAllocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h,ggg00,hgg04.h,hbd01.h,\
    hbd02.h,heo58.h,:SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h
SharedSQL_CommandCache.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h,ggg00
SharedSQL_CommandCacheIterator.cpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp
SharedSQL_CommandCacheIterator.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SharedSQL_CommandCacheStatistics.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
SharedSQL_CommandCache_Wrapper.cpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_SharedSQL.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    hak101.h
SharedSQL_CommandInfo.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp
SharedSQL_CommandStatistics.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp
SharedSQL_Handles.cpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Plan.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,heo58.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h
SharedSQL_Handles.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Plan.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SharedSQL_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
SharedSQL_ParseIDCache.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Command.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/Container/Container_AVLTree.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h
SharedSQL_Plan.hpp inc=:SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ChangedUserScope.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ISharedSQLInterface.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h,\
    :SAPDB/Container/Container_AVLTree.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp
SharedSQL_SQLCache.cpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h
SharedSQL_SQLCache.hpp inc=\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_ParseIDCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
SharedSQL_Types.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/Container/Container_AVLTree.hpp,vak001

