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
FBM_BlockStateList.hpp inc=:SAPDB/FreeBlockManagement/FBM_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp
FBM_DataVolume.cpp inc=:SAPDB/FreeBlockManagement/FBM_DataVolume.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Dump.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Exception.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
FBM_DataVolume.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IDataVolume.hpp,\
    :SAPDB/FreeBlockManagement/FBM_BlockStateList.hpp
FBM_DataVolumeArray.hpp inc=:SAPDB/FreeBlockManagement/FBM_IVolume.hpp,\
    :SAPDB/Container/Container_Vector.hpp
FBM_DataVolumeIterator.hpp inc=\
    :SAPDB/FreeBlockManagement/FBM_DataVolumeArray.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IDataVolume.hpp
FBM_Dump.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
FBM_Exception.hpp inc=:SAPDB/FreeBlockManagement/FBM_Messages.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Types.hpp
FBM_IDataVolume.hpp inc=:SAPDB/FreeBlockManagement/FBM_IVolume.hpp
FBM_IManager.hpp inc=:SAPDB/RunTime/RTE_Types.h,gsp00,\
    :SAPDB/FreeBlockManagement/FBM_Types.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_IBlockAddressIterator.hpp,\
    :SAPDB/IOManager/IOMan_ReservedBlockAddress.hpp,\
    :SAPDB/IOManager/IOMan_ClusterAddress.hpp
FBM_IVolume.hpp inc=:SAPDB/FreeBlockManagement/FBM_Types.hpp,\
    :SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,:SAPDB/RunTime/RTE_Types.h
FBM_Manager.cpp inc=hgg08.h,hgg01.h,hkb57.h,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Manager.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Dump.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Types.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Exception.hpp,\
    :SAPDB/FreeBlockManagement/FBM_SimpleDataVolume.hpp,\
    :SAPDB/FreeBlockManagement/FBM_SequentialDataVolume.hpp,\
    :SAPDB/FreeBlockManagement/FBM_DataVolumeIterator.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp
FBM_Manager.hpp inc=gsp00,ggg00,heo51.h,heo55k.h,hgg08.h,\
    :SAPDB/IOManager/IOMan_ComplexClusterAddress.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/FreeBlockManagement/FBM_DataVolumeArray.hpp,\
    :SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_SynchronizedDBFullList.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
FBM_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
FBM_PascalInterface.cpp inc=gbd00,gsp03,gsp03_3,hbd11.h,\
    :SAPDB/FreeBlockManagement/FBM_Manager.hpp
FBM_SequentialDataVolume.cpp inc=\
    :SAPDB/FreeBlockManagement/FBM_SequentialDataVolume.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Dump.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Exception.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Messages.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
FBM_SequentialDataVolume.hpp inc=\
    :SAPDB/FreeBlockManagement/FBM_IVolume.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
FBM_SimpleDataVolume.cpp inc=\
    :SAPDB/FreeBlockManagement/FBM_SimpleDataVolume.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Dump.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Exception.hpp,\
    :SAPDB/FreeBlockManagement/FBM_Messages.hpp,\
    :SAPDB/Pager/Pager_Controller.hpp,\
    :SAPDB/IOManager/IOMan_ComplexClusterAddress.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,:SAPDB/RunTime/RTE_Crash.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
FBM_SimpleDataVolume.hpp inc=\
    :SAPDB/FreeBlockManagement/FBM_DataVolume.hpp
FBM_Types.hpp inc=\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp

