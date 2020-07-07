# --------------------------------------------------------------------------
# description: Catalog
#
# author:	thomasA
# --------------------------------------------------------------------------
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

#
# include dependencies (do not edit)
#
Catalog_Authorization.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Authorization.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SchemaObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_UserObject.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,hak01.h
Catalog_Authorization.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp
Catalog_AuthorizationDescriptor.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,hsp40.h
Catalog_AuthorizationDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_CatalogTableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp
Catalog_AuthorizationObject.cpp inc=:SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_RequestScope.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ICallBack.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IsoLevelScope.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_LockedSecondaryKeyScope.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,hak01.h,hkb53.h
Catalog_AuthorizationObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp
Catalog_AuthorizationRefObject.cpp inc=\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationRefObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
Catalog_AuthorizationRefObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_Cache.cpp inc=:SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_CacheImplementation.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp
Catalog_Cache.hpp inc=:SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_CacheImplementation.cpp inc=heo670.h,\
    :SAPDB/SQLManager/Catalog/Catalog_CacheImplementation.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationRefObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ContainerDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IncrementalBufferAllocator.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,\
    :SAPDB/FrameControl/FrameCtrl_IBlockAllocator.hpp
Catalog_CacheImplementation.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPool.hpp
Catalog_CatalogTableDescriptor.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_CatalogTableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp
Catalog_CatalogTableDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp
Catalog_ColumnDescriptor.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_ColumnDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,hgg04.h,vak001
Catalog_ColumnDescriptor.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp
Catalog_ContainerDescriptor.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ContainerDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ContainerObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/liveCache/LVC_ContainerManager.hpp,\
    :SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,hsp40.h
Catalog_ContainerDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_CatalogTableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp
Catalog_ContainerObject.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ContainerObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IsoLevelScope.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_LockedSecondaryKeyScope.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_RequestScope.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp
Catalog_ContainerObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp
Catalog_DBProcDescriptor.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcInfoObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h
Catalog_DBProcDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_ReturnCodes.hpp
Catalog_DBProcFileObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcFileObject.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_DBProcFileObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_DBProcInfoObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcInfoObject.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_DBProcInfoObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_DBProcObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcObject.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_DBProcObject.hpp inc=:SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_FieldSequenceMarshaler.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_FieldSequenceMarshaler.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCache.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_FieldSequenceMarshaler.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_IPersistenceStream.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_IAuthorization.hpp inc=
Catalog_ICallBack.hpp inc=
Catalog_ISessionCache.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp
Catalog_ISharedSQLInterface.cpp inc=hsp30.h,\
    :SAPDB/SQLManager/Catalog/Catalog_ISharedSQLInterface.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCacheInline.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp
Catalog_ISharedSQLInterface.hpp inc=
Catalog_IndexDescriptor.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_IndexDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,hak24.h,hbd03.h
Catalog_IndexDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_ColumnDescriptor.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp
Catalog_IndexIterator.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IndexIterator.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,hak10.h,hak24.h
Catalog_IndexIterator.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_IndexDescriptor.hpp
Catalog_Instance.cpp inc=hak01.h,hak103.h,hak51.h,hbd998.h,\
    :SAPDB/SQLManager/SQLMan_ChangedUserScope.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RawAllocator.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,hgg01_3.h,ggg00,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,hbd01.h,hbd01_1.h
Catalog_Instance.hpp inc=:SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Authorization.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,ggg00
Catalog_Interface.hpp inc=:SAPDB/RunTime/RTE_Types.hpp,ggg00
Catalog_IsoLevelScope.hpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp
Catalog_LockedSecondaryKeyScope.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_LockedSecondaryKeyScope.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Instance.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,hkb53.h
Catalog_LockedSecondaryKeyScope.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp
Catalog_MessBlockObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_MessBlockObject.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_MessBlockObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp
Catalog_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Catalog_Object.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationRefObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ContainerObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_MessBlockObject.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_Object.hpp inc=:SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_PascalWrapper.cpp inc=hak103.h,hak362.h,hak42.h,hak43.h,hak10.h,\
    :SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ICallBack.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ContainerObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_FieldSequenceMarshaler.hpp,\
    :SAPDB/liveCache/LVC_ContainerManager.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Persistence.hpp
Catalog_RequestScope.hpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,hgg01.h,hgg01_3.h,hta01.h
Catalog_ReturnCodes.hpp inc=
Catalog_SchemaObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp
Catalog_SessionCache.cpp inc=:SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCacheInline.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ContainerObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcFileObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcInfoObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_MessBlockObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SharedSqlObject.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,hak01.h,hak10.h,\
    hak260.h,hak51.h
Catalog_SessionCache.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ISessionCache.hpp,\
    :SAPDB/Container/Container_Hash.hpp
Catalog_SessionCacheInline.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCache.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,hak10.h
Catalog_SharedSqlObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_SharedSqlObject.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_SharedSqlObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_TableDescriptor.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,hak06.h,hak10.h,hak24.h,\
    hak663.h,hbd01.h,ggg00
Catalog_TableDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ColumnDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IndexIterator.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IndexDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Container/Container_Vector.hpp
Catalog_TableObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h,hak10.h
Catalog_TableObject.hpp inc=:SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp
Catalog_TableStatisticsDescriptor.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableStatisticsObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,hak10.h,\
    hak28.h
Catalog_TableStatisticsDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp
Catalog_TableStatisticsObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_TableStatisticsObject.hpp
Catalog_TableStatisticsObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp
Catalog_Test.cpp inc=:SAPDB/SQLManager/Catalog/Catalog_Test.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ViewDescriptor.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,hak06.h,hak10.h
Catalog_Test.hpp inc=
Catalog_Types.cpp inc=:SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    hsp77.h,heo52.h
Catalog_Types.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Catalog_UserObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp
Catalog_ViewDescriptor.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ViewDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ViewtextObject.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
Catalog_ViewDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp
Catalog_ViewtextObject.cpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_ViewtextObject.hpp
Catalog_ViewtextObject.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp

