#
# include dependencies (do not edit)
#
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

KernelMem_DataCacheAllocator.hpp inc=\
    :SAPDB/RunTime/RTE_CompilerFeatures.h,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp,\
    :SAPDB/FrameControl/FrameCtrl_IBlockAllocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_KernelCommon.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Container/Container_AVLTree.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp
KernelMem_RawAllocatorTracer.cpp inc=heo52.h,heo66.h,\
    :SAPDB/KernelCommon/MemoryManagement/KernelMem_RawAllocatorTracer.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/KernelCommon/Kernel_OpMsg.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp
KernelMem_RawAllocatorTracer.hpp inc=\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp

