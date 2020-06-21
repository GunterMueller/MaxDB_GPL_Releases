#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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

*.hpp       compilecheck
*.h         compilecheck

#
# include dependencies (do not edit)
#
Rst_Exceptions.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/Restart/Rst_Messages.hpp
Rst_IRedoTrafficControl.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/Restart/Rst_Types.hpp,gsp00,ggg00
Rst_IoSeqList.hpp inc=:SAPDB/Restart/Rst_Exceptions.hpp
Rst_LogEntryIterator.cpp inc=hkb57_1.h,\
    :SAPDB/Restart/Rst_LogEntryIterator.hpp,\
    :SAPDB/Logging/Log_Volume.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/Messages/Msg_List.hpp
Rst_LogEntryIterator.hpp inc=:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_Page.hpp,:SAPDB/Logging/Log_VolumeIterator.hpp,\
    :SAPDB/Restart/Rst_Exceptions.hpp,:SAPDB/Restart/Rst_Types.hpp,\
    :SAPDB/Restart/Rst_StandbySynchronizer.hpp
Rst_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Rst_RedoFile.cpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceReader.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceWriter.hpp,\
    :SAPDB/Restart/Rst_Exceptions.hpp,:SAPDB/Restart/Rst_RedoFile.hpp
Rst_RedoFile.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ChainSplitSpaceForwardRead.hpp,\
    :SAPDB/DataAccess/Data_BaseFile.hpp,\
    :SAPDB/Logging/Log_AfterImageSpace.hpp,:SAPDB/Logging/Log_Types.hpp
Rst_RedoManager.cpp inc=:SAPDB/Restart/Rst_Exceptions.hpp,\
    :SAPDB/Restart/Rst_RedoManager.hpp,\
    :SAPDB/Restart/Rst_RedoTrafficControl.hpp,\
    :SAPDB/Restart/Rst_IRedoTrafficControl.hpp,\
    :SAPDB/Restart/Rst_RedoReadTask.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobRedo.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_WaitingTask.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Rst.h,\
    :SAPDB/Transaction/Trans_Context.hpp,ggg00,hgg01.h
Rst_RedoManager.hpp inc=ggg00,:SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/Restart/Rst_Types.hpp,:SAPDB/Restart/Rst_LogEntryIterator.hpp,\
    :SAPDB/Restart/Rst_IRedoTrafficControl.hpp
Rst_RedoReadTask.cpp inc=ggg92.h,heo52.h,hgg01_3.h,hkb51.h,hkb50.h,\
    hkb57.h,hkb57_1.h,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,\
    :SAPDB/Restart/Rst_RedoManager.hpp,\
    :SAPDB/Restart/Rst_RedoReadTask.hpp,\
    :SAPDB/Restart/Rst_IRedoTrafficControl.hpp,\
    :SAPDB/Restart/Rst_Exceptions.hpp,\
    :SAPDB/Restart/Rst_LogEntryIterator.hpp,\
    :SAPDB/Logging/Log_VolumeIterator.hpp,\
    :SAPDB/Logging/Log_SeqTapeReader.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,:SAPDB/Logging/Log_AfterImage.hpp,\
    :SAPDB/Logging/Log_ActionUndoRedo.hpp,\
    :SAPDB/Logging/Log_ActionPartialRollback.hpp,\
    :SAPDB/Logging/Log_ActionSavepoint.hpp,\
    :SAPDB/Logging/Log_OpenTransFile.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Rst.h
Rst_RedoReadTask.hpp inc=:SAPDB/Restart/Rst_Types.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,ggg00,gsp00
Rst_RedoTrafficControl.cpp inc=\
    :SAPDB/Restart/Rst_IRedoTrafficControl.hpp,\
    :SAPDB/Restart/Rst_RedoTrafficControl.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_TaskWaitList.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminBackupRecovery.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/Restart/Rst_Exceptions.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobDropAuxFiles.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Rst.h,gsp00,ggg00,ggg92.h,\
    heo55k.h,heo56.h,hgg01.h,hgg08.h,hkb50.h,hkb51.h,hkb53.h
Rst_RedoTrafficControl.hpp inc=\
    :SAPDB/Restart/Rst_IRedoTrafficControl.hpp,\
    :SAPDB/Restart/Rst_IoSeqList.hpp
Rst_StandbySynchronizer.hpp inc=gsp00,ggg00,heo56.h,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp
Rst_Types.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp

