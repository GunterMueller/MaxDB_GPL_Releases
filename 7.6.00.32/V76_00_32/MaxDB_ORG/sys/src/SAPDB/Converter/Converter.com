
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
# ------------------------------------------------------------------------------
#
# Platform dependend compilation option.... Do NOT place at end of this file...
#
#

#PTS 1114591 performance improvement ...
&if  $MACH in [PA20W, PA11]
Converter_PermMapControl.cpp                +inline_level0
&endif


&if $OSSPEC == LINUX && $MACH in [ SDBonPPC64 ]
Converter_Converter.cpp      -OPT=-O1 #PTS 1133972 TS 2005-02-16
Converter_PermMapControl.cpp -OPT=-O1 #PTS 1133972 TS 2005-02-16
Converter_Map.cpp            -OPT=-O1 #PTS 1133972 TS 2005-02-16
&endif

&if $OSSPEC = WIN32 and not defined BIT64 and $_MSC_VER = 12
Converter_PermMapControl.cpp    -IGNORE=C4786
&endif


# ------------------------------------------------------------------------------
#
# include dependencies (do not edit)
#
Converter_ArchiveContainer.cpp inc=\
    :SAPDB/Converter/Converter_ArchiveContainer.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp
Converter_ArchiveContainer.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp
Converter_BitMapPage.hpp inc=:SAPDB/SAPDBCommon/SAPDB_BitOperations.hpp,\
    :SAPDB/KernelCommon/Kernel_Page74.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Converter_BlockAddressIterator.cpp inc=\
    :SAPDB/Converter/Converter_BlockAddressIterator.hpp
Converter_BlockAddressIterator.hpp inc=\
    :SAPDB/IOManager/IOMan_IBlockAddressIterator.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp
Converter_Chain.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Converter_Converter.cpp inc=hgg01.h,hgg08.h,hbd20_6.h,hkb57.h,\
    :SAPDB/Converter/Converter_Converter.hpp,\
    :SAPDB/Converter/Converter_Dump.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_MapError.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp,\
    :SAPDB/Converter/Converter_Pager.hpp,\
    :SAPDB/Converter/Converter_BlockAddressIterator.hpp,\
    :SAPDB/Converter/Converter_DBSpaceStatistic.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/GarbageCollector/GC_IGCController.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/IOManager/IOMan_IDataManager.hpp,\
    :SAPDB/Pager/Pager_IRestart.hpp,:SAPDB/Pager/Pager_ISavepoint.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_Converter.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_DBFull.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h,:SAPDB/RunTime/RTE_KGS.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Converter.h
Converter_Converter.hpp inc=:SAPDB/Converter/Converter_IBackUp.hpp,\
    :SAPDB/Converter/Converter_IDiagnose.hpp,\
    :SAPDB/Converter/Converter_IDump.hpp,\
    :SAPDB/Converter/Converter_IPageIO.hpp,\
    :SAPDB/Converter/Converter_ISavePoint.hpp,\
    :SAPDB/Converter/Converter_IDropVolume.hpp,\
    :SAPDB/Converter/Converter_IManager.hpp,\
    :SAPDB/Converter/Converter_IMigration.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/Converter/Converter_IPageNoMapping.hpp,\
    :SAPDB/Converter/Converter_IPageNoManager.hpp,\
    :SAPDB/Converter/Converter_IVerify.hpp,\
    :SAPDB/Converter/Converter_IEvent.hpp,\
    :SAPDB/Converter/Converter_Index.hpp,\
    :SAPDB/Converter/Converter_MapControl.hpp,\
    :SAPDB/Converter/Converter_TempMapControl.hpp,\
    :SAPDB/Converter/Converter_PermMapControl.hpp,\
    :SAPDB/Converter/Converter_PermStaticMapControl.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_Event.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_SynchronizedTaskWaitList.hpp,\
    :SAPDB/KernelCommon/Kernel_SynchronizedDBFullList.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp
Converter_DBSpaceStatistic.hpp inc=ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Converter_DiagnoseWrapper.cpp inc=gsp00,\
    :SAPDB/Converter/Converter_Page.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_IndexPage.hpp,\
    :SAPDB/Converter/Converter_ParentReference.hpp,\
    :SAPDB/KernelCommon/Kernel_Page.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Converter_Dump.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Converter_Entry.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp
Converter_Exception.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Converter_FlagInfo.cpp inc=:SAPDB/Converter/Converter_FlagInfo.hpp
Converter_FlagInfo.hpp inc=:SAPDB/DataAccess/Data_Types.hpp
Converter_FreePageList.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Converter_IBackUp.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Types.hpp
Converter_ICommon.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Converter_IDiagnose.hpp inc=:SAPDB/DataAccess/Data_Types.hpp
Converter_IDropVolume.hpp inc=:SAPDB/IOManager/IOMan_BlockAddress.hpp
Converter_IDump.hpp inc=
Converter_IEvent.hpp inc=\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_Event.hpp
Converter_IManager.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Converter_IMigration.hpp inc=:SAPDB/DataAccess/Data_Types.hpp
Converter_IPageIO.hpp inc=
Converter_IPageNoManager.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Converter_IPageNoMapping.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Converter_ISavePoint.hpp inc=:SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp
Converter_IVerify.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Types.hpp
Converter_Index.cpp inc=:SAPDB/Converter/Converter_Dump.hpp,\
    :SAPDB/Converter/Converter_Index.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/Converter/Converter_Lock.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp,\
    :SAPDB/Converter/Converter_Pager.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp
Converter_Index.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Container/Container_SortableVector.hpp,\
    :SAPDB/Converter/Converter_Chain.hpp,\
    :SAPDB/Converter/Converter_IndexEntry.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_LeafReference.hpp
Converter_IndexEntry.hpp inc=:SAPDB/Converter/Converter_Entry.hpp,\
    :SAPDB/Converter/Converter_IndexPage.hpp
Converter_IndexPage.cpp inc=:SAPDB/Converter/Converter_IndexPage.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Converter_IndexPage.hpp inc=:SAPDB/Converter/Converter_Page.hpp
Converter_LeafPage.cpp inc=:SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_BlockAddressIterator.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Converter_LeafPage.hpp inc=:SAPDB/Converter/Converter_Page.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp
Converter_LeafReference.hpp inc=:SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/Converter/Converter_ParentReference.hpp
Converter_Lock.cpp inc=:SAPDB/Converter/Converter_Lock.hpp,heo55k.h,\
    hgg08.h
Converter_Lock.hpp inc=:SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Converter_Map.cpp inc=:SAPDB/Converter/Converter_Dump.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_FlagInfo.hpp,\
    :SAPDB/Converter/Converter_Map.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/Converter/Converter_MapSection.hpp,\
    :SAPDB/Converter/Converter_PermMapSection.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_Crash.hpp
Converter_Map.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Converter/Converter_BitMapPage.hpp,\
    :SAPDB/Converter/Converter_Chain.hpp,\
    :SAPDB/Converter/Converter_MapEntry.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp,\
    :SAPDB/Converter/Converter_PermMapSection.hpp
Converter_MapClusterIterator.hpp inc=:SAPDB/Converter/Converter_Page.hpp,\
    :SAPDB/Converter/Converter_Map.hpp
Converter_MapControl.cpp inc=:SAPDB/Converter/Converter_MapControl.hpp,\
    :SAPDB/Converter/Converter_MapSection.hpp,\
    :SAPDB/Converter/Converter_Dump.hpp,\
    :SAPDB/Converter/Converter_MapPeriodIterator.hpp,\
    :SAPDB/Converter/Converter_MapClusterIterator.hpp,\
    :SAPDB/Converter/Converter_MapSectionIterator.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Converter_MapControl.hpp inc=:SAPDB/Converter/Converter_Map.hpp,\
    :SAPDB/Converter/Converter_MapSectionData.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
Converter_MapEntry.hpp inc=:SAPDB/Converter/Converter_Chain.hpp,\
    :SAPDB/Converter/Converter_Entry.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp
Converter_MapError.hpp inc=:SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Converter_MapPageNoIterator.hpp inc=\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Converter/Converter_MapEntry.hpp
Converter_MapPeriodIterator.hpp inc=\
    :SAPDB/Converter/Converter_MapClusterIterator.hpp,\
    :SAPDB/Converter/Converter_MapControl.hpp
Converter_MapSection.cpp inc=:SAPDB/Converter/Converter_MapSection.hpp,\
    :SAPDB/Converter/Converter_Map.hpp,heo55k.h
Converter_MapSection.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_MapSectionData.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Converter_MapSectionData.cpp inc=\
    :SAPDB/Converter/Converter_MapSectionData.hpp,\
    :SAPDB/Converter/Converter_Map.hpp
Converter_MapSectionData.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_Chain.hpp,\
    :SAPDB/Converter/Converter_Dump.hpp,\
    :SAPDB/Converter/Converter_MapEntry.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/RunTime/RTE_Types.hpp
Converter_MapSectionIterator.hpp inc=\
    :SAPDB/Converter/Converter_MapPeriodIterator.hpp
Converter_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Converter_Page.cpp inc=:SAPDB/Converter/Converter_Page.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/RunTime/RTE_Crash.hpp
Converter_Page.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/KernelCommon/Kernel_Page74.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_ParentReference.hpp,\
    :SAPDB/Converter/Converter_FlagInfo.hpp
Converter_Page72.cpp inc=ggg00,hgg01_3.h,\
    :SAPDB/Converter/Converter_Page72.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/SAPDBCommon/SAPDB_ToString.hpp
Converter_Page72.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Page72.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp
Converter_PageCollector.cpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_PageCollector.hpp,\
    :SAPDB/Converter/Converter_IPageNoMapping.hpp,\
    :SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp
Converter_PageCollector.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_PageNoList.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/IOManager/IOMan_DataPages.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp
Converter_PageFrameAllocator.cpp inc=\
    :SAPDB/Converter/Converter_Converter.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp,\
    :SAPDB/FrameControl/FrameCtrl_IConverter.hpp
Converter_PageFrameAllocator.hpp inc=\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Converter_PageNoContainer.cpp inc=\
    :SAPDB/Converter/Converter_PageNoContainer.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Converter.h
Converter_PageNoContainer.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Converter/Converter_FlagInfo.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp
Converter_PageNoIterator.cpp inc=\
    :SAPDB/Converter/Converter_PageNoIterator.hpp,\
    :SAPDB/Converter/Converter_Converter.hpp
Converter_PageNoIterator.hpp inc=\
    :SAPDB/Converter/Converter_MapSectionIterator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp
Converter_Pager.cpp inc=:SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/Converter/Converter_Converter.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/Converter/Converter_Page.hpp,\
    :SAPDB/Converter/Converter_Pager.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Types.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_IConverterIO.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp
Converter_Pager.hpp inc=\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp,\
    :SAPDB/Converter/Converter_Version.hpp
Converter_ParentReference.hpp inc=:SAPDB/Converter/Converter_Types.hpp
Converter_PermMapControl.cpp inc=\
    :SAPDB/Converter/Converter_PermMapControl.hpp,\
    :SAPDB/Converter/Converter_BitMapPage.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_Index.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_MapError.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/Converter/Converter_Pager.hpp,\
    :SAPDB/Converter/Converter_PermMapSection.hpp,\
    :SAPDB/Converter/Converter_BlockAddressIterator.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/KernelCommon/Kernel_SynchronizedTaskWaitList.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Converter.h
Converter_PermMapControl.hpp inc=\
    :SAPDB/Converter/Converter_MapControl.hpp,\
    :SAPDB/Converter/Converter_UsedPageNoContainer.hpp,\
    :SAPDB/Converter/Converter_PageNoContainer.hpp,\
    :SAPDB/Converter/Converter_Map.hpp,\
    :SAPDB/Converter/Converter_PermMapSectionData.hpp,\
    :SAPDB/Converter/Converter_ArchiveContainer.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_SynchronizedTaskWaitList.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp
Converter_PermMapSection.cpp inc=\
    :SAPDB/Converter/Converter_PermMapSection.hpp,\
    :SAPDB/Converter/Converter_Map.hpp
Converter_PermMapSection.hpp inc=:SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_Chain.hpp,\
    :SAPDB/Converter/Converter_MapSection.hpp,\
    :SAPDB/Converter/Converter_MapSectionData.hpp,\
    :SAPDB/Converter/Converter_PermMapSectionData.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Converter_PermMapSectionData.cpp inc=\
    :SAPDB/Converter/Converter_PermMapSectionData.hpp,\
    :SAPDB/Converter/Converter_Map.hpp
Converter_PermMapSectionData.hpp inc=\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_Chain.hpp,\
    :SAPDB/Converter/Converter_Dump.hpp,\
    :SAPDB/Converter/Converter_FreePageList.hpp,\
    :SAPDB/Converter/Converter_ParentReference.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp
Converter_PermStaticMapControl.cpp inc=\
    :SAPDB/Converter/Converter_PermStaticMapControl.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_MapError.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/Converter/Converter_Map.hpp,\
    :SAPDB/Converter/Converter_PermMapSection.hpp,\
    :SAPDB/Converter/Converter_Pager.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Converter_PermStaticMapControl.hpp inc=\
    :SAPDB/Converter/Converter_PermMapControl.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_SynchronizedTaskWaitList.hpp
Converter_TempMapControl.cpp inc=\
    :SAPDB/Converter/Converter_TempMapControl.hpp,\
    :SAPDB/Converter/Converter_Map.hpp,\
    :SAPDB/Converter/Converter_MapError.hpp,\
    :SAPDB/Converter/Converter_MapSection.hpp,\
    :SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Converter_TempMapControl.hpp inc=\
    :SAPDB/Converter/Converter_MapControl.hpp
Converter_Types.hpp inc=:SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp
Converter_UsedPageNoContainer.cpp inc=\
    :SAPDB/Converter/Converter_UsedPageNoContainer.hpp
Converter_UsedPageNoContainer.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Converter/Converter_Types.hpp,\
    :SAPDB/Converter/Converter_LeafPage.hpp,\
    :SAPDB/Converter/Converter_BitMapPage.hpp,\
    :SAPDB/Converter/Converter_PageFrameAllocator.hpp
Converter_Version.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp
Converter_Wrapper.cpp inc=ggg00,gbd00,gkb00,hbd10.h,\
    :SAPDB/Converter/Converter_Converter.hpp,\
    :SAPDB/Converter/Converter_DBSpaceStatistic.hpp,\
    :SAPDB/Converter/Converter_Exception.hpp,\
    :SAPDB/Converter/Converter_Messages.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_SnapShot.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp

