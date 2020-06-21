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

# include dependencies (do not edit)
#
Table_Exceptions.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/Table/Table_Messages.hpp
Table_IResult.hpp inc=:SAPDB/Messages/Msg_List.hpp
Table_KeyDescription.hpp inc=
Table_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Table_Table.cpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp,\
    :SAPDB/Table/Table_Exceptions.hpp,\
    :SAPDB/KernelCommon/Kernel_BlobDescription.hpp,\
    :SAPDB/KernelCommon/Kernel_IndexDescription.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/Logging/Log_ActionCreateDropFile.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/Table/Table_UnsortedResult.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Table.h,\
    :SAPDB/Table/Table_Table.hpp,ggg00,hbd01.h,ggg92.h,hkb50.h,hkb64.h
Table_Table.hpp inc=:SAPDB/Table/Table_Types.hpp,\
    :SAPDB/Table/Table_UnsortedResult.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/KernelCommon/Kernel_IndexDescription.hpp
Table_TempHashTable.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_TempFile.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_TempHashTable.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Table.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_FunnelMerge.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    vak001,hak07.h
Table_Test.cpp inc=\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_FunnelMerge.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldList.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_BlobDescription.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,:SAPDB/Table/Table_Exceptions.hpp,\
    :SAPDB/Table/Table_TempHashTable.hpp,:SAPDB/Table/Table_Table.hpp,\
    :SAPDB/Table/Table_UnsortedResult.hpp,\
    :SAPDB/Table/Table_Messages.hpp,:SAPDB/Table/Table_Test.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,hkb53.h,ggg200.h,heo52.h
Table_Test.hpp inc=
Table_Types.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp
Table_UnsortedResult.cpp inc=:SAPDB/Table/Table_UnsortedResult.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Table.h
Table_UnsortedResult.hpp inc=:SAPDB/Table/Table_IResult.hpp,\
    :SAPDB/Messages/Msg_List.hpp

