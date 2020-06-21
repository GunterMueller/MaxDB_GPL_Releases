# ------------------------------------------------------------------------------
#
# DataAccess Descriptions
#
# ------------------------------------------------------------------------------
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

&if $MACH = SUN
# Sun compiler bug in optimization -xO4: PTS 1133843
Data_RandomTreeAccess.cpp -OPT=-xO3
&endif

&if $OSSPEC = LINUX && $MACH == SDBonPPC64
# sporadic wrong values for long page count in table TABLESTATISTICS (M.Ki.)
# detected during comparison with file directory counters during mut
# gcc (GCC) 3.3.3 (SuSE Linux)
Data_PrimTreeStatistic.cpp -OPT=-O0
&endif

#
# include dependencies (do not edit)
#
Data_BaseFile.cpp inc=:SAPDB/DataAccess/Data_BaseFile.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp
Data_BaseFile.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_IFile.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp
Data_BasePage.cpp inc=ggg00,hgg01_3.h,\
    :SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_Page.hpp,\
    :SAPDB/KernelCommon/Kernel_Exception.hpp,\
    :SAPDB/KernelCommon/Kernel_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Data_BasePage.hpp inc=gbd00,:SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Page72.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_PersistentTypes.hpp
Data_Chain.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_ChainPage.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Data_ChainFixSizeSpace.hpp inc=:SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_PageFixSizeSpace.hpp,\
    :SAPDB/DataAccess/Data_Chain.hpp
Data_ChainPage.cpp inc=:SAPDB/DataAccess/Data_ChainPage.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
Data_ChainPage.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/DataAccess/Data_Page.hpp
Data_ChainSplitSpaceBackwardRead.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Chain.hpp,\
    :SAPDB/DataAccess/Data_PageSplitSpace.hpp,\
    :SAPDB/DataAccess/Data_SplitSpace.hpp
Data_ChainSplitSpaceForwardRead.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Chain.hpp,\
    :SAPDB/DataAccess/Data_PageSplitSpace.hpp,\
    :SAPDB/DataAccess/Data_SplitSpace.hpp
Data_ChainVarSizeSpace.hpp inc=:SAPDB/DataAccess/Data_Chain.hpp
Data_Exceptions.hpp inc=:SAPDB/DataAccess/Data_Messages.hpp
Data_FileTypes.hpp inc=
Data_IFile.hpp inc=
Data_IOBuffer.cpp inc=:SAPDB/DataAccess/Data_IOBuffer.hpp,\
    :SAPDB/IOManager/IOMan_IDataInfo.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp
Data_IOBuffer.hpp inc=:SAPDB/DataAccess/Data_PageFrame.hpp
Data_ISplitSpace.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp
Data_ITreeAccess.hpp inc=:SAPDB/DataAccess/Data_PrimPage.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp
Data_IndexOrderList.cpp inc=:SAPDB/DataAccess/Data_IndexOrderList.hpp,\
    :SAPDB/Container/Container_Stack.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hgg01_3.h
Data_IndexOrderList.hpp inc=:SAPDB/Container/Container_Stack.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gbd00
Data_IndexScanTreeAccess.cpp inc=hbd01_1.h,hbd30.h,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_IndexScanTreeAccess.hpp
Data_IndexScanTreeAccess.hpp inc=ggg00,gbd00,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_PrimPage.hpp,\
    :SAPDB/DataAccess/Data_ITreeAccess.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
Data_KeyBoundedPageAccess.cpp inc=\
    :SAPDB/DataAccess/Data_KeyBoundedPageAccess.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_PrimPage.hpp,:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_ITreeAccess.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,hsp30.h,hbd13.h,hbd30.h,ggg00,gbd00,\
    gsp00,hta99.h
Data_KeyBoundedPageAccess.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_PrimPage.hpp,\
    :SAPDB/DataAccess/Data_ITreeAccess.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,ggg00,gbd00,gsp00
Data_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Data_Page.cpp inc=:SAPDB/DataAccess/Data_Page.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    hbd06.h,heo51.h
Data_Page.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/DataAccess/Data_BasePage.hpp,:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,gbd00
Data_PageAccessManager.cpp inc=ggg00,hbd01_1.h,hbd06.h,hbd13.h,hgg04.h,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp
Data_PageAccessManager.hpp inc=ggg00,gbd00,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/DataAccess/Data_BasePage.hpp,\
    :SAPDB/Converter/Converter_IVerify.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Data_PageBasic.cpp inc=:SAPDB/DataAccess/Data_PageBasic.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,:SAPDB/RunTime/RTE_Message.hpp
Data_PageBasic.hpp inc=:SAPDB/IOManager/IOMan_IPage.hpp,\
    :SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,ggg00
Data_PageFixSizeSpace.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/DataAccess/Data_ChainPage.hpp
Data_PageFrame.hpp inc=:SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp
Data_PageNoList.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Data_PagePrimaryFile.cpp inc=:SAPDB/DataAccess/Data_PagePrimaryFile.hpp
Data_PagePrimaryFile.hpp inc=:SAPDB/DataAccess/Data_Page.hpp
Data_PageSplitSpace.cpp inc=:SAPDB/DataAccess/Data_PageSplitSpace.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,:SAPDB/RunTime/RTE_Message.hpp
Data_PageSplitSpace.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/DataAccess/Data_ChainPage.hpp
Data_PrimPage.hpp inc=:SAPDB/DataAccess/Data_Page.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
Data_PrimPageAccess.hpp inc=:SAPDB/DataAccess/Data_BasePage.hpp
Data_PrimTreeStatistic.cpp inc=hbd01_1.h,hbd02.h,hbd05.h,hbd30.h,\
    hbd998.h,hgg01_3.h,hgg04.h,ggg90.h,heo50.h,hsp30.h,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/Converter/Converter_PageCollector.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_PrimTreeStatistic.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_ScanTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_IndexScanTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_ITreeAccess.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,:SAPDB/DataAccess/Data_Row.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SQLManager/SQLMan_UpdateStatistics.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp
Data_PrimTreeStatistic.hpp inc=ggg00,gbd00,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/DataAccess/Data_PrimPage.hpp,\
    :SAPDB/KernelCommon/Kernel_TableStatistic.hpp
Data_PrimTreeStatisticsCoordinator.cpp inc=\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SQLManager/SQLMan_UpdateStatistics.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp,\
    :SAPDB/DataAccess/Data_PrimTreeStatisticsCoordinator.hpp,\
    :SAPDB/DataAccess/Data_UpdStatRequestFile.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Data.h,\
    :SAPDB/ServerTasks/SrvTasks_JobCollectTableStatistics.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Container/Container_Vector.hpp,hbd998.h,hbd01.h,hbd50.h,\
    hbd30.h,gbd00,:SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,:SAPDB/Messages/Msg_List.hpp,\
    hgg04.h,heo52.h,heo56.h,:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp
Data_PrimTreeStatisticsCoordinator.hpp inc=\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,:SAPDB/RunTime/RTE_Types.hpp,\
    ggg00
Data_PrimaryFile.cpp inc=:SAPDB/DataAccess/Data_PagePrimaryFile.hpp,\
    :SAPDB/DataAccess/Data_PrimaryFile.hpp
Data_PrimaryFile.hpp inc=:SAPDB/DataAccess/Data_BaseFile.hpp
Data_RandomTreeAccess.cpp inc=hbd30.h,ggg90.h,\
    :SAPDB/DataAccess/Data_RandomTreeAccess.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Data_RandomTreeAccess.hpp inc=gbd00,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_ITreeAccess.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_PrimPage.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp
Data_RecordHeaderAccess.cpp inc=ggg00,\
    :SAPDB/DataAccess/Data_RecordHeaderAccess.h,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp
Data_RecordHeaderAccess.h inc=
Data_Row.cpp inc=:SAPDB/DataAccess/Data_Row.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp
Data_Row.hpp inc=ggg00
Data_ScanTreeAccess.cpp inc=hbd13.h,hbd20_2.h,hbd30.h,hbd50.h,heo56.h,\
    :SAPDB/DataAccess/Data_ScanTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_FileIO.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp
Data_ScanTreeAccess.hpp inc=gsp00,ggg00,gbd00,\
    :SAPDB/DataAccess/Data_ITreeAccess.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_PrimPage.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
Data_SplitSpace.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Data_SplitSpaceReader.cpp inc=\
    :SAPDB/DataAccess/Data_SplitSpaceReader.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp
Data_SplitSpaceReader.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp
Data_SplitSpaceWriter.cpp inc=\
    :SAPDB/DataAccess/Data_SplitSpaceWriter.hpp,\
    :SAPDB/DataAccess/Data_Messages.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp
Data_SplitSpaceWriter.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp
Data_TempFile.cpp inc=:SAPDB/DataAccess/Data_TempFile.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Data_TempFile.hpp inc=:SAPDB/DataAccess/Data_BaseFile.hpp,\
    :SAPDB/DataAccess/Data_ChainFixSizeSpace.hpp
Data_TempFileTypes.cpp inc=:SAPDB/DataAccess/Data_TempFileTypes.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
Data_TempFileTypes.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
Data_Test.cpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_Chain.hpp,\
    :SAPDB/DataAccess/Data_PageSplitSpace.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceWriter.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceReader.hpp,\
    :SAPDB/DataAccess/Data_ChainSplitSpaceBackwardRead.hpp,\
    :SAPDB/DataAccess/Data_ChainSplitSpaceForwardRead.hpp,\
    :SAPDB/DataAccess/Data_ChainFixSizeSpace.hpp,\
    :SAPDB/DataAccess/Data_PrimaryFile.hpp,\
    :SAPDB/DataAccess/Data_TempFile.hpp,\
    :SAPDB/DataAccess/Data_PageBasic.hpp,:SAPDB/DataAccess/Data_Test.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h,\
    :SAPDB/Transaction/Trans_Context.hpp,hbd01.h,hbd01_1.h,hbd02.h
Data_Test.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.h
Data_TraverseTreeAccess.cpp inc=ggg90.h,hbd30.h,\
    :SAPDB/DataAccess/Data_TraverseTreeAccess.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Data_TraverseTreeAccess.hpp inc=ggg00,gbd00,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_ITreeAccess.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_PrimPage.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
Data_Types.cpp inc=:SAPDB/DataAccess/Data_Types.hpp
Data_Types.hpp inc=ggg91.h,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
Data_UpdStatRequestFile.cpp inc=\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/DataAccess/Data_UpdStatRequestFile.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Data.h,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd02.h,hbd01.h,hbd998.h,hgg04.h,\
    ggg00,heo52.h,hsp41.h
Data_UpdStatRequestFile.hpp inc=:SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    ggg00
Data_UpdateCounterSrvTask.cpp inc=\
    :SAPDB/DataAccess/Data_UpdateCounterSrvTask.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ITaskForJob.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobTypes.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Data.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd01.h,hbd03.h,gbd00
Data_UpdateCounterSrvTask.hpp inc=:SAPDB/ServerTasks/SrvTasks_Job.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Data_Wrapper.cpp inc=gsp00,ggg00,gbd00,hbd998.h,\
    :SAPDB/DataAccess/Data_RandomTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_ScanTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_TraverseTreeAccess.hpp,\
    :SAPDB/DataAccess/Data_PrimTreeStatistic.hpp,\
    :SAPDB/DataAccess/Data_IndexOrderList.hpp,\
    :SAPDB/DataAccess/Data_Row.hpp,\
    :SAPDB/KernelCommon/Kernel_TableStatistic.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp

