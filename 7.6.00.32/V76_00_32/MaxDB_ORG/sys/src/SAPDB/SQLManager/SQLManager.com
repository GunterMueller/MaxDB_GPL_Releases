# --------------------------------------------------------------------------
# description: SQL Manager
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
SQLMan_ActiveStatementDirectory.cpp inc=\
    :SAPDB/SQLManager/SQLMan_ActiveStatementDirectory.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp
SQLMan_ActiveStatementDirectory.hpp inc=\
    :SAPDB/Container/Container_AVLTree.hpp
SQLMan_ChangedUserScope.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_ChangedUserScope.hpp,hak51.h
SQLMan_ChangedUserScope.hpp inc=
SQLMan_Connect.cpp inc=:SAPDB/SQLManager/SQLMan_Connect.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_VarData.hpp,\
    :SAPDB/RunTime/Security/RTESec_Authentication.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/KernelCommon/Kernel_Random.hpp
SQLMan_Connect.hpp inc=gsp00,\
    :SAPDB/RunTime/Security/RTESec_Authentication.hpp,\
    :SAPDB/Messages/Msg_List.hpp
SQLMan_Context.cpp inc=hak06.h,hak07.h,hak10.h,hak101.h,hak262.h,heo58.h,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/KernelCommon/Kernel_Initialization.hpp,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/Messages/Msg_Arg.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h
SQLMan_Context.hpp inc=vak001,:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp
SQLMan_ContextSessionMembers.cpp inc=\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Debugger.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/DBProc/Proc_SessionListContextInterface.hpp,\
    :SAPDB/SQLManager/SQLMan_SchemaMap.hpp,hak101.h
SQLMan_ContextSessionMembers.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SQLMan_ActiveStatementDirectory.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/SQLManager/SQLMan_SessionInfo.hpp,\
    :SAPDB/SystemViews/SysView_PseudoLongHandler.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Runtime.hpp,\
    :SAPDB/Messages/Msg_List.hpp
SQLMan_DiagnoseDropFile.cpp inc=\
    :SAPDB/SQLManager/SQLMan_DiagnoseDropFile.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_IndexDescriptor.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.h,hbd01.h
SQLMan_DiagnoseDropFile.hpp inc=
SQLMan_ErrPascalWrapper.cpp inc=hgg171.h,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Instance.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
SQLMan_InternalTrans.cpp inc=:SAPDB/SQLManager/SQLMan_InternalTrans.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,hak51.h
SQLMan_InternalTrans.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
SQLMan_LogicalFileNameGenerator.cpp inc=\
    :SAPDB/SQLManager/SQLMan_LogicalFileNameGenerator.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,gsp00
SQLMan_LogicalFileNameGenerator.hpp inc=\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SQLMan_MessBlock.cpp inc=:SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_SequenceMemoryCopy.hpp
SQLMan_MessBlock.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp
SQLMan_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
SQLMan_PascalWrapper.cpp inc=hak01.h,hak06.h,hak07.h,hak071.h,hak10.h,\
    hak101.h,hsp77.h,hsp80.h,hsp81.h,hbd998.h,hgg01_3.h,hgg06.h,gsp00,\
    geo200.h,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SQLManager/SQLMan_UpdateStatistics.hpp,\
    :SAPDB/SQLManager/SQLMan_UpdateCounter.hpp,\
    :SAPDB/SQLManager/SQLMan_Connect.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_DiagnoseDropFile.hpp,\
    :SAPDB/SQLManager/SQLMan_SessionInfo.hpp,\
    :SAPDB/SQLManager/SQLMan_SessionList.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SessionCache.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_SharedSqlObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Debugger.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Instance.hpp,\
    :SAPDB/SQLManager/SQLMan_SchemaMap.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_MySQLProxy.h,\
    :SAPDB/Join/Join_HashAccessAllocator.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,hbd998.h,hbd01.h,\
    :SAPDB/SQLManager/SQLMan_LogicalFileNameGenerator.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    hbd998.h,:SAPDB/PacketInterface/PacketCheck/PkgCheck_Checker.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_VarData.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_PropertyList.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysCheckTableLog.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpgradeHistory.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdStatLog.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdStatWanted.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdateCounterWanted.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h
SQLMan_SQLStatementContext.cpp inc=hak10.h,hak93.h,hak101.h,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h
SQLMan_SQLStatementContext.hpp inc=ggg00,gsp00
SQLMan_SavedErrorScope.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_SavedErrorScope.hpp
SQLMan_SavedErrorScope.hpp inc=:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SQLMan_SchemaMap.cpp inc=:SAPDB/SQLManager/SQLMan_SchemaMap.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,hak05.h
SQLMan_SchemaMap.hpp inc=:SAPDB/Container/Container_AVLTree.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp
SQLMan_SessionInfo.cpp inc=:SAPDB/SQLManager/SQLMan_SessionInfo.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_LegacyConnectionInterface.hpp,\
    hkb51_2.h,ggg00,geo003.h\
&if $OSSPEC == WIN32 
,gos003.h\
&else                
,hen53.h\
&endif               

SQLMan_SessionInfo.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITaskClientConnection.hpp,ggg91.h
SQLMan_SessionIterator.cpp inc=\
    :SAPDB/SQLManager/SQLMan_SessionIterator.hpp,\
    :SAPDB/SQLManager/SQLMan_SessionInfo.hpp,\
    :SAPDB/SQLManager/SQLMan_SessionList.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,hak10.h
SQLMan_SessionIterator.hpp inc=:SAPDB/SQLManager/SQLMan_SessionList.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITaskClientConnection.hpp,gsp00
SQLMan_SessionList.cpp inc=:SAPDB/SQLManager/SQLMan_SessionList.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/Container/Container_List.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPool.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp
SQLMan_SessionList.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/Container/Container_List.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_SpinlockPool.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp
SQLMan_Strategy.hpp inc=ggg07
SQLMan_TempFileCollection.cpp inc=\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Wrapper.hpp,\
    :SAPDB/SQLManager/SQLMan_LogicalFileNameGenerator.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd998.h,hgg01_3.h,hgg04.h,ggg00
SQLMan_TempFileCollection.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
SQLMan_Test.cpp inc=:SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Test.hpp
SQLMan_Test.hpp inc=
SQLMan_Tracing.hpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,hta01.h,hta01_1.h
SQLMan_Types.cpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Instance.hpp,hak01.h,hak061.h,\
    hsp83.h
SQLMan_Types.hpp inc=vak001,:SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
SQLMan_UpdateCounter.cpp inc=:SAPDB/SQLManager/SQLMan_UpdateCounter.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdateCounterWanted.hpp,\
    :SAPDB/SQLManager/SQLMan_InternalTrans.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/DataAccess/Data_UpdateCounterSrvTask.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Logging/Log_ActionSetFileDirCounter.hpp,\
    :SAPDB/RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.h,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,vak001,hak06.h,hkb50.h,hkb51.h,\
    hbd01.h,hgg06.h,heo56.h
SQLMan_UpdateCounter.hpp inc=:SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SQLMan_UpdateStatistics.cpp inc=\
    :SAPDB/SQLManager/SQLMan_UpdateStatistics.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ColumnDescriptor.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdStatWanted.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdStatLog.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/DataAccess/Data_PrimTreeStatisticsCoordinator.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_DataAccess.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    hak01.h,hak05.h,hak06.h,hak10.h,hak28.h,hak51.h,hak52.h,hkb64.h,\
    hgg06.h,heo52.h
SQLMan_UpdateStatistics.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/DataAccess/Data_UpdStatRequestFile.hpp,\
    :SAPDB/DataAccess/Data_PrimTreeStatisticsCoordinator.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,gsp00

