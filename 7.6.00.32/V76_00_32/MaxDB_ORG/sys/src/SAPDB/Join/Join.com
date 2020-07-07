# --------------------------------------------------------------------------
# description: 
#
# author:	
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

# decrease optimization because of segmentation fault 
# in gcc compiler with higher optimization

&if $OSSPEC == LINUX && $MACH == IA64
Join_JoinOperator2.cpp -OPT=-O1
&endif

#
# include dependencies (do not edit)
#
Join_AccessDesc.cpp inc=:SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Join.h,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/Join/Join_AccessDesc.hpp,:SAPDB/Join/Join_Trace.hpp,gsp00,\
    gsp03_1,hak06.h,hta01.h,hta01_3.h,ggg07,hgg04.h,hgg04_1.h,hbd01.h,\
    hbd01_1.h,hkb720.h,hgg01_1.h,hak725.h,hbd998.h
Join_AccessDesc.hpp inc=\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/Join/Join_Types.hpp,\
    ggg00,ggg07,hak70.h,hak101.h
Join_AccessOperator.cpp inc=ggg00,hgg01_3.h,ggg11,gsp00,hak101.h,hta01.h,\
    hta01_3.h,hgg04.h,:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Join/Join_IAccessOperator.hpp,:SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Join/Join_AccessOperator.hpp
Join_AccessOperator.hpp inc=:SAPDB/Join/Join_IAccessOperator.hpp,\
    :SAPDB/Join/Join_AccessDesc.hpp,:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gsp00,ggg00
Join_Aggregation.cpp inc=:SAPDB/Join/Join_Aggregation.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/Join/Join_Trace.hpp,ggg00,hta01.h,hkb721.h,hbd01.h,hbd07.h,\
    hsp41.h,hsp51.h,hsp20.h,hsp30.h,hsp40.h,hak101.h,hgg01_3.h,hbd998.h
Join_Aggregation.hpp inc=:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,ggg00,ggg07
Join_Aggregator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/Table/Table_TempHashTable.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/Join/Join_Trace.hpp,:SAPDB/Join/Join_Aggregation.hpp,ggg00,\
    gsp00
Join_Execute.cpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Join/Join_Types.hpp,:SAPDB/Join/Join_JoinOperator2.hpp,\
    :SAPDB/Join/Join_IResultSetOperator.hpp,\
    :SAPDB/Join/Join_ResultSetInserter.hpp,\
    :SAPDB/Join/Join_ResultSetHashInserter.hpp,\
    :SAPDB/Join/Join_ResultSetAppenderEx.hpp,\
    :SAPDB/Join/Join_ResultSetAppender.hpp,\
    :SAPDB/Join/Join_ResultSetOneResult.hpp,\
    :SAPDB/Join/Join_ResultSetDistinctOneResult.hpp,\
    :SAPDB/Join/Join_ResultSetDistinct.hpp,\
    :SAPDB/Join/Join_ResultSetAggregator.hpp,\
    :SAPDB/Join/Join_ResultSetAggregatorOneGroup.hpp,\
    :SAPDB/Join/Join_ResultSetAggregator2.hpp,\
    :SAPDB/Join/Join_ResultSetDummy.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SQLManager/SQLMan_Strategy.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Join.h,\
    :SAPDB/Table/Table_TempHashTable.hpp,gsp03,gsp03_1,ggg00,ggg04,\
    vak001,gak68,hak06.h,hak101.h,hak682.h,hak686.h,hta01.h,heo51.h,\
    hkb720.h,hkb05.h,hak071.h,hsp41.h,hak680.h,hak502.h
Join_HashAccessAllocator.cpp inc=\
    :SAPDB/Join/Join_HashAccessAllocator.hpp,:SAPDB/Join/Join_Trace.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_Join.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Join.h
Join_HashAccessAllocator.hpp inc=\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Join_HashAccessOperator.cpp inc=:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/Join/Join_HashAccessOperator.hpp,\
    :SAPDB/Join/Join_HashAccessAllocator.hpp,\
    :SAPDB/Join/Join_IAccessOperator.hpp,:SAPDB/Join/Join_AccessDesc.hpp,\
    :SAPDB/Join/Join_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,hbd73.h,hta01.h,hta01_3.h,\
    hsp30.h
Join_HashAccessOperator.hpp inc=:SAPDB/Join/Join_IAccessOperator.hpp,\
    :SAPDB/Join/Join_AccessDesc.hpp,:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gsp00,ggg00
Join_IAccessOperator.cpp inc=:SAPDB/Join/Join_IAccessOperator.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,:SAPDB/Trace/Trace_MessBlockEntry.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,gsp00,ggg00,\
    hgg01_3.h,ggg11,hgg04.h,hkb71.h,hta01.h
Join_IAccessOperator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Join/Join_IOperator.hpp,:SAPDB/Join/Join_AccessDesc.hpp,ggg00
Join_IOperator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Join/Join_Types.hpp,:SAPDB/Join/Join_Trace.hpp,ggg00
Join_IResultSetAggregator.cpp inc=\
    :SAPDB/Join/Join_IResultSetAggregator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Join/Join_Aggregation.hpp,hak101.h,hbd07.h,hkb721.h,hbd01.h,\
    hgg01_3.h,hta01.h,hsp51.h,hbd998.h
Join_IResultSetAggregator.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/Join/Join_IOperator.hpp,\
    :SAPDB/Join/Join_IResultSetOperator.hpp,\
    :SAPDB/Join/Join_Aggregation.hpp,hgg04.h,gsp00,hsp30.h,hsp40.h,\
    hsp41.h,hsp51.h,hta01.h,hbd73.h
Join_IResultSetAppender.hpp inc=:SAPDB/Join/Join_IResultSetOperator.hpp,\
    :SAPDB/Container/Container_Vector.hpp,hbd07.h,hsp41.h
Join_IResultSetInserter.hpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Join/Join_IResultSetOperator.hpp,hak101.h,hbd07.h,hbd73.h
Join_IResultSetOperator.hpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Join/Join_Trace.hpp,:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,ggg00,hgg01_3.h,hsp20.h,\
    hta01.h,hkb721.h
Join_IndexAccessOperator.hpp inc=:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/Join/Join_AccessOperator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,hgg04.h,hbd01.h
Join_InvAccessOperator.hpp inc=:SAPDB/Join/Join_IndexAccessOperator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,hak01.h,hbd03.h,hbd21.h,\
    hta01.h,hkb720.h
Join_InvAccessOperatorEx.cpp inc=\
    :SAPDB/Join/Join_InvAccessOperatorEx.hpp,\
    :SAPDB/Join/Join_IndexAccessOperator.hpp,\
    :SAPDB/Join/Join_InvSelectIterator.hpp,:SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,ggg07,hta01.h
Join_InvAccessOperatorEx.hpp inc=:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/Join/Join_IndexAccessOperator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00,hgg01_3.h
Join_InvSelectIterator.cpp inc=heo00x.h,heo56.h,heo670.h,heo922.h,\
    hbd01.h,hbd02.h,hbd03.h,hbd21.h,hbd73.h,hgg01.h,hgg01_3.h,hgg04.h,\
    hkb742.h,hak101.h,hta01.h,hta01_3.h,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Join/Join_InvSelectIterator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_SuspendReason.h
Join_InvSelectIterator.hpp inc=\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobList.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobJoinInvSelector.hpp
Join_JoinOperator2.cpp inc=gsp00,ggg00,ggg07,gkb07,\
    :SAPDB/RunTime/RTE_CurrentStack.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Join/Join_JoinOperator2.hpp,\
    :SAPDB/Join/Join_TableAccessOperator.hpp,\
    :SAPDB/Join/Join_InvAccessOperator.hpp,\
    :SAPDB/Join/Join_InvAccessOperatorEx.hpp,\
    :SAPDB/Join/Join_InvSelectIterator.hpp,\
    :SAPDB/Join/Join_HashAccessAllocator.hpp,\
    :SAPDB/Join/Join_HashAccessOperator.hpp,\
    :SAPDB/Join/Join_NopAccessOperator.hpp,\
    :SAPDB/Join/Join_SortOperator.hpp,\
    :SAPDB/Join/Join_LegacyAccessOperator.hpp,hak06.h,hak682.h,hak683.h,\
    hak101.h,hak70.h,hkb74.h,hsp30.h,hgg01.h,hgg02.h,hgg20.h,hta01.h,\
    hta01_3.h,hsp49.h,hkb71.h,hgg04.h,hbd01.h,hbd01_1.h,hbd07.h,hbd73.h,\
    hbd998.h,hkb720.h
Join_JoinOperator2.hpp inc=gsp00,ggg00,ggg07,gkb07,vak001,gak68,\
    :SAPDB/Join/Join_Types.hpp,:SAPDB/Join/Join_IOperator.hpp
Join_LegacyAccessOperator.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/Join/Join_LegacyAccessOperator.hpp,\
    :SAPDB/Join/Join_InvAccessOperator.hpp,\
    :SAPDB/Join/Join_TableAccessOperator.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,hbd01.h,hbd07.h,hak41.h,hgg04.h,heo51.h,\
    hgg01_3.h,ggg11,hkb05.h,hak101.h,hbd998.h
Join_LegacyAccessOperator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Join/Join_Types.hpp,:SAPDB/Join/Join_Trace.hpp,\
    :SAPDB/Join/Join_AccessOperator.hpp,ggg00,hbd07.h
Join_NopAccessOperator.hpp inc=:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/Join/Join_AccessOperator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Join_ResultSetAggregator.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Join/Join_IResultSetAggregator.hpp,\
    :SAPDB/Join/Join_ResultSetInserter.hpp,hbd07.h
Join_ResultSetAggregator2.cpp inc=\
    :SAPDB/Join/Join_ResultSetAggregator2.hpp,\
    :SAPDB/Join/Join_Aggregation.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,hak101.h,hbd07.h,hkb721.h,\
    hgg01_3.h,hta01.h,hsp51.h,hsp41.h,hbd998.h
Join_ResultSetAggregator2.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/Join/Join_IOperator.hpp,\
    :SAPDB/Join/Join_IResultSetOperator.hpp,\
    :SAPDB/Table/Table_TempHashTable.hpp,\
    :SAPDB/Join/Join_Aggregation.hpp,:SAPDB/Join/Join_Aggregator.hpp,\
    hgg04.h,gsp00,hbd73.h,hbd01.h,hbd07.h,hak101.h
Join_ResultSetAggregatorOneGroup.hpp inc=\
    :SAPDB/Join/Join_IResultSetAggregator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,hgg01_3.h,hsp41.h
Join_ResultSetAppender.hpp inc=:SAPDB/Join/Join_IResultSetAppender.hpp
Join_ResultSetAppenderEx.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Join/Join_IResultSetAppender.hpp
Join_ResultSetDistinct.cpp inc=:SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Join/Join_ResultSetDistinct.hpp,hbd07.h,hkb50_2.h,hsp30.h
Join_ResultSetDistinct.hpp inc=:SAPDB/Join/Join_IResultSetInserter.hpp
Join_ResultSetDistinctOneResult.cpp inc=\
    :SAPDB/Join/Join_ResultSetDistinctOneResult.hpp,hkb50_2.h
Join_ResultSetDistinctOneResult.hpp inc=\
    :SAPDB/Join/Join_ResultSetOneResult.hpp
Join_ResultSetDummy.hpp inc=:SAPDB/Join/Join_IResultSetOperator.hpp
Join_ResultSetHashInserter.hpp inc=\
    :SAPDB/Join/Join_ResultSetInserter.hpp,hkb50_2.h
Join_ResultSetInserter.cpp inc=:SAPDB/Join/Join_ResultSetInserter.hpp,\
    hbd07.h,hsp41.h
Join_ResultSetInserter.hpp inc=:SAPDB/Join/Join_IResultSetInserter.hpp,\
    :SAPDB/Container/Container_Vector.hpp
Join_ResultSetOneResult.hpp inc=:SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/Join/Join_IResultSetOperator.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/Join/Join_IOperator.hpp,\
    hsp41.h
Join_SortOperator.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/Join/Join_SortOperator.hpp,\
    :SAPDB/Join/Join_InvAccessOperator.hpp,\
    :SAPDB/Join/Join_TableAccessOperator.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp,hbd07.h,hgg04.h,\
    heo51.h,hgg01_3.h,ggg11
Join_SortOperator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Join/Join_Types.hpp,:SAPDB/Join/Join_Trace.hpp,\
    :SAPDB/Join/Join_AccessOperator.hpp,ggg00,hbd07.h
Join_TableAccessOperator.cpp inc=:SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Join/Join_TableAccessOperator.hpp,\
    :SAPDB/Join/Join_AccessOperator.hpp,ggg00,hgg04.h,ggg07,hbd02.h,\
    hbd21.h,hta01.h,hta01_3.h,hak70.h,hak01.h,hkb720.h,hak101.h,hbd998.h,\
    hak01.h
Join_TableAccessOperator.hpp inc=:SAPDB/Join/Join_Types.hpp,\
    :SAPDB/Join/Join_AccessOperator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,gsp00
Join_Trace.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp
Join_Types.hpp inc=gsp00,ggg00,gak68,hbd01_1.h,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Table/Table_TempHashTable.hpp

