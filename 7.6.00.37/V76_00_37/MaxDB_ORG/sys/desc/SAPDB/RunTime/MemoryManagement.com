#
# RunTime Environment / Memory Handling Descriptions
#
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

*         -except 
#
# No exception handling code variant
#
*-noex    -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
#
# Singleton RawAllocator (synchronized + minimum overhead)
RTEMem_RawAllocator.hpp inc=SAPDBMem_IRawAllocator.hpp,RTESync_InterlockedCounter.hpp
RTEMem_RawAllocator*.cpp inc=SAPDBMem_NewDestroy.hpp,SAPDB_Types.hpp,RTE_ISystem.hpp,RTEMem_RawAllocator.hpp,RTEMem_BlockAllocator.hpp,RTEMem_AllocatorRegister.hpp,SAPDBMem_Exceptions.hpp,SAPDBErr_Assertions.hpp
#
# Singleton BlockAllocator (synchronized + zero overhead)
RTEMem_BlockAllocator.hpp inc=SAPDBMem_IBlockAllocator.hpp,RTESync_InterlockedCounter.hpp,RTEMem_AllocatorRegister.hpp
RTEMem_BlockAllocator*.cpp inc=SAPDBMem_NewDestroy.hpp,RTE_ISystem.hpp,RTEMem_BlockAllocator.hpp,\
                               SAPDBErr_Assertions.hpp,SAPDBMem_Exceptions.hpp,RTEMem_AllocatorRegister.hpp
#
# Singleton PageAllocator (NON synchronized + zero overhead)
RTEMem_PageAllocator.h   inc=SAPDB_Types.h
RTEMem_PageAllocator.hpp inc=SAPDBMem_IRawAllocator.hpp,SAPDB_Singleton.hpp,RTESync_NamedSpinlock.hpp
&if $OS in [ WIN32 ]
RTEMem_PageAllocator*.cpp inc=SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,RTEMem_PageAllocator.hpp,RTEMem_BlockAllocator.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_Message.hpp
&else
RTEMem_PageAllocator*.cpp inc=SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,RTEMem_PageAllocator.hpp,RTEMem_BlockAllocator.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_Message.hpp,\
    RTEConf_ParameterAccessKernelInterface.hpp,RTE_saveUNIXcalls.h,RTE_SystemUNIX.hpp
&endif
#
# Singleton Allocator (synchronized and nonsynchronized version of SAPDBMem_RawAllocator)
RTEMem_Allocator.hpp inc=SAPDBMem_IRawAllocator.hpp,SAPDBMem_SynchronizedRawAllocator.hpp,RTESync_NamedSpinlock.hpp
RTEMem_Allocator*.cpp inc=SAPDBMem_NewDestroy.hpp,RTEMem_Allocator.hpp,RTEMem_BlockAllocator.hpp,SAPDBErr_Assertions.hpp,RTEMem_AllocatorRegister.hpp
RTEMem_AllocatorSimpleProcess.hpp inc=SAPDBMem_IRawAllocator.hpp,SAPDBMem_DougLeaAllocator.hpp
RTEMem_AllocatorSimpleProcess*.cpp inc=SAPDBMem_NewDestroy.hpp,RTEMem_AllocatorSimpleProcess.hpp,RTEMem_RawAllocator.hpp,SAPDBErr_Assertions.hpp
#
# RTE only used Singleton Allocator (synchronized version of DougLea Allocator)
RTEMem_RteAllocator.hpp inc=SAPDBMem_IRawAllocator.hpp,SAPDBMem_SynchronizedRawAllocator.hpp,RTESync_NamedSpinlock.hpp
RTEMem_RteAllocator*.cpp inc=SAPDBMem_NewDestroy.hpp,RTEMem_RteAllocator.hpp,\
    RTEMem_BlockAllocator.hpp,SAPDBErr_Assertions.hpp,\
    RTEMem_AllocatorRegister.hpp dep=heo07.h
#
RTEMem_AllocatorRegister.hpp inc=SAPDBMem_IAllocatorInfo.hpp,RTE_ItemRegister.hpp
RTEMem_AllocatorRegister*.cpp inc=RTEMem_AllocatorRegister.hpp,SAPDBMem_NewDestroy.hpp,RTEMem_RteAllocator.hpp,SAPDB_string.h
#
RTEMem_AllocatorInfoReader.hpp inc=RTEMem_AllocatorRegister.hpp,SAPDBMem_IRawAllocator.hpp,SAPDBMem_NewDestroy.hpp,Container_List.hpp
RTEMem_AllocatorInfoReader.cpp inc=ggg00,RTEMem_AllocatorRegister.hpp,RTEMem_AllocatorInfoReader.hpp,SAPDBMem_IRawAllocator.hpp,SAPDBMem_NewDestroy.hpp,Container_List.hpp
#
# Self registering 'Allocate only' wrapper allocator for identifying memory using components
RTEMem_AllocatorWrapper.hpp inc=SAPDBMem_IRawAllocator.hpp,RTEMem_AllocatorRegister.hpp,RTESync_InterlockedCounter.hpp,SAPDBErr_Assertions.hpp
#
# Self registering BuddyAllocator
RTEMem_BuddyAllocator.hpp inc=SAPDBMem_BuddyAllocator.hpp,RTEMem_AllocatorRegister.hpp
#
# Self registering IncrementalRawAllocator
RTEMem_IncrementalRawAllocator.hpp inc=SAPDBMem_IncrementalRawAllocator.hpp,RTEMem_AllocatorRegister.hpp
#
# Testing Wrapper Allocator and Register
RTEMem_TestAllocatorRegister.cpp inc=RTEMem_AllocatorWrapper.hpp,RTEMem_DougLeaAllocator.hpp,RTEMem_BuddyAllocator.hpp,RTEMem_IncrementalRawAllocator.hpp,RTEMem_Allocator.hpp,SAPDBMem_NewDestroy.hpp

#
# System page caching
RTEMem_SystemPageCache.hpp inc=SAPDB_Types.hpp,SAPDB_Singleton.hpp,SAPDBMem_IBlockAllocator.hpp,RTESync_NamedSpinlock.hpp,RTESync_InterlockedCounter.hpp
RTEMem_SystemPageCache.cpp inc=SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,RTEMem_SystemPageCache.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTESys_SystemInfo.h,RTEMem_AllocatorRegister.hpp,RTE_Message.hpp,RTE_SystemPageCache_Messages.hpp

# Allocator for special cases where registering must be avoided (for example within RTE_ItemRegister...)
RTEMem_UnregisteredAllocator.hpp inc=SAPDBMem_IRawAllocator.hpp,RTESync_Spinlock.hpp
RTEMem_UnregisteredAllocator.cpp inc=RTEMem_UnregisteredAllocator.hpp,RTEMem_RteAllocator.hpp

RTEMem_ProtectingAllocator.h inc=SAPDB_Types.h
RTEMem_ProtectingAllocator.hpp inc=heo07.h,SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,APDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp,RunTime/Synchronisation/RTESync_InterlockedCounter.hpp
RTEMem_ProtectingAllocator.cpp inc=RTEMem_ProtectingAllocator.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Messages.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp\
&if $OS not in [ WIN32 ]
                               ,gen41.h
&endif

#
# AWE
&if $OS in [ WIN32 ]
RTEMem_AWEAllocator.hpp inc=SAPDB_Singleton.hpp,SAPDBTrace_Usage.hpp dep=RTESys_NTAWE.hpp
RTEMem_AWEAllocator.h inc=SAPDB_Types.h
&else
RTEMem_AWEAllocator.hpp inc=SAPDB_Singleton.hpp,SAPDBTrace_Usage.hpp
&endif

RTEMem_AWEAllocator.cpp inc=SAPDBMem_NewDestroy.hpp,SAPDBMem_Exceptions.hpp,RTEMem_AWEAllocator.hpp,RTE_Crash.hpp,\
                            RTE_MessageList.hpp,RTE_Message.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp

RTEMem_GlobalSharedMemoryAllocator.hpp inc=SAPDBMem_IBlockAllocator.hpp,RTEIPC_NamedSharedMemory.hpp,Container_Vector.hpp
RTEMem_GlobalSharedMemoryAllocator.cpp inc=RTEMem_GlobalSharedMemoryAllocator.hpp,SAPDBMem_IBlockAllocator.hpp,\
	RTESync_InterlockedCounter.hpp,SAPDB_sprintf.h,RTEMem_RteAllocator.hpp,SAPDBMem_IBlockAllocator.hpp,\
	RTE_Crash.hpp,RTESys_SecurityAttributes.hpp,RTEMem_SharedMemoryChunkAllocator.hpp,\
	RTESync_InterlockedCounter.hpp,RTEIPC_NamedSharedMemory.hpp

RTEMem_LocalSharedMemoryAllocator.hpp inc=RTEIPC_NamedSharedMemory.hpp,SAPDBMem_RawAllocator.hpp,SAPDBMem_IRawAllocator.hpp,RTEMem_GlobalSharedMemoryAllocator.hpp,SAPDBMem_IBlockAllocator.hpp,RTESync_InterlockedCounter.hpp,RTESync_NamedSpinlock.hpp,SAPDBMem_RawAllocator.hpp
RTEMem_LocalSharedMemoryAllocator.cpp inc=RTEMem_LocalSharedMemoryAllocator.hpp,SAPDBMem_IBlockAllocator.hpp,RTESync_InterlockedCounter.hpp,SAPDB_sprintf.h,RTEIPC_NamedSharedMemory.hpp,RTESync_NamedSpinlock.hpp

RTEMem_LocalSharedAddressMapper.hpp inc=RTEMem_SharedMemoryCommonControls.hpp,RTEMem_GlobalSharedMemoryAllocator.hpp,SAPDBMem_IBlockAllocator.hpp,RTESync_InterlockedCounter.hpp,RTESync_NamedSpinlock.hpp
RTEMem_LocalSharedAddressMapper.cpp inc=RTEMem_LocalSharedAddressMapper.hpp,SAPDBMem_IBlockAllocator.hpp,RTESync_InterlockedCounter.hpp,SAPDB_sprintf.h,RTEIPC_NamedSharedMemory.hpp,RTESync_NamedSpinlock.hpp,RTEMem_RteAllocator.hpp,RTESys_SecurityAttributes.hpp

RTEMem_SharedMemoryCommonControls.hpp inc=RTEIPC_NamedSharedMemory.hpp,SAPDBMem_RawAllocator.hpp,SAPDBMem_IRawAllocator.hpp,SAPDBMem_IBlockAllocator.hpp,RTESync_InterlockedCounter.hpp,SAPDBMem_RawAllocator.hpp,RTESys_SystemInfo.h,RTEMem_SharedMemoryChunkAllocator.hpp

RTEMem_SharedMemoryChunkAllocator.hpp inc=SAPDBMem_IBlockAllocator.hpp,RTESync_InterlockedCounter.hpp,RTESys_SystemInfo.h
RTEMem_RemoteSharedMemoryChunkAllocator.hpp inc=RTEMem_SharedMemoryChunkAllocator.hpp
RTEMem_KernelSharedMemoryChunkAllocator.hpp inc=RTEMem_SharedMemoryChunkAllocator.hpp

RTEMem_RemoteSharedMemoryChunkAllocator.cpp inc=RTEMem_SharedMemoryCommonControls.hpp,SAPDB_sprintf.h,RTEIPC_NamedSharedMemory.hpp,RTE_Crash.hpp,RTEMem_RteAllocator.hpp,RTESys_SecurityAttributes.hpp,SAPDB_string.h,RTEMem_RemoteSharedMemoryChunkAllocator.hpp
RTEMem_KernelSharedMemoryChunkAllocator.cpp inc=SAPDB_sprintf.h,RTEIPC_NamedSharedMemory.hpp,RTE_Crash.hpp,RTEMem_RteAllocator.hpp,RTESys_SecurityAttributes.hpp,SAPDB_string.h,RTEMem_KernelSharedMemoryChunkAllocator.hpp,RTEMem_SharedMemoryCommonControls.hpp

&if $OS in [ WIN32 ]
RTEMem_GSMJobSegment.hpp inc=RTE_Types.hpp
&else
RTEMem_GSMJobSegment.hpp inc=RTE_Types.hpp,RTESync_BinarySemaphore.hpp
&endif
RTEMem_KernelGSMJobSegment.hpp inc=RTEMem_GSMJobSegment.hpp
RTEMem_GSMSegment.hpp inc=SAPDBMem_DoublyLinked.hpp,RTEIPC_NamedSharedMemory.hpp
RTEMem_GSMSegment.cpp inc=RTEMem_GSMSegment.hpp,SAPDBMem_VirtualAddressSpace.hpp,RTE_Message.hpp,SAPDB_string.h,Msg_List.hpp
RTEMem_KernelGSMWorker.hpp inc=RTEMem_KernelGSMJobSegment.hpp,RTEThread_Thread.hpp,SAPDBMem_RawAllocator.hpp,RTEMem_BlockAllocator.hpp,Msg_List.hpp
RTEMem_KernelGSMWorker-k.cpp -DKERNEL \
	inc=RTEMem_KernelGSMWorker.hpp,RTEMem_UDEServerGSMJobSegment.hpp,RTE_IInterface.hpp,RTEIPC_NamedSharedMemory.hpp,SAPDB_string.h,SAPDBMem_NewDestroy.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTESync_BinarySemaphore.hpp,RTESys_SecurityAttributes.hpp,RTE_Message.hpp,RTESync_NamedSpinlock.hpp,RTESys_ProcessState.hpp,SAPDBMem_DoublyLinked.hpp,SAPDBMem_VirtualAddressSpace.hpp,SDBMsg_SAPDBMem.h,RTEMem_GSMSegment.hpp
RTEMem_KernelGSMWorker.cpp \
    inc=RTEMem_KernelGSMWorker.hpp,RTEMem_UDEServerGSMJobSegment.hpp,RTE_IInterface.hpp,RTEIPC_NamedSharedMemory.hpp,SAPDB_string.h,SAPDBMem_NewDestroy.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTESync_BinarySemaphore.hpp,RTESys_SecurityAttributes.hpp,RTE_Message.hpp,SAPDBMem_DoublyLinked.hpp,SDBMsg_SAPDBMem.h

RTEMem_UDEServerGSMJobSegment.hpp inc=RTEMem_GSMJobSegment.hpp
RTEMem_UDEServerGSMWorker.hpp inc=RTEMem_UDEServerGSMJobSegment.hpp,RTE_IInterface.hpp,RTEThread_Thread.hpp,SAPDBMem_RawAllocator.hpp
RTEMem_UDEServerGSMWorker-k.cpp -DKERNEL \
	inc=RTEMem_UDEServerGSMWorker.hpp,RTEMem_KernelGSMWorker.hpp,RTEIPC_NamedSharedMemory.hpp,SAPDB_string.h,SAPDBMem_NewDestroy.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTESync_BinarySemaphore.hpp,RTESys_SecurityAttributes.hpp,RTE_Message.hpp,RTESync_NamedSpinlock.hpp
RTEMem_UDEServerGSMWorker.cpp \
	inc=RTEMem_UDEServerGSMWorker.hpp,RTEMem_KernelGSMWorker.hpp,RTEIPC_NamedSharedMemory.hpp,SAPDB_string.h,SAPDBMem_NewDestroy.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTESync_BinarySemaphore.hpp,RTESys_SecurityAttributes.hpp,RTE_Message.hpp,RTESync_NamedSpinlock.hpp,SDBMsg_SAPDBMem.h,SDBMsg_RTEMem.h,SAPDBMem_DoublyLinked.hpp,RTEMem_GSMSegment.hpp

RTEMem_AddressIndepSHMAllocatorHashTab.hpp  inc=RTESync_NamedSpinlock.hpp,SAPDBMem_NewDestroy.hpp,SAPDB_Types.h,RTEIPC_NamedSharedMemory.hpp,RTE_Mem_Messages.hpp
RTEMem_AddressIndepSHMAllocatorHashTab.cpp  inc=RTEMem_AddressIndepSHMAllocatorHashTab.hpp,SAPDBMath_PrimeNumber.hpp,RTE_Mem_Messages.hpp,SAPDBErr_Assertions.hpp,RTEMem_RteAllocator.hpp,SAPDBTrace_Usage.hpp

RTEMem_AddressIndepSHMAllocator.hpp			inc=SAPDBMem_IRawAllocator.hpp,SAPDBMem_SynchronizedRawAllocator.hpp,RTESys_SecurityAttributes.hpp
RTEMem_AddressIndepSHMAllocator.cpp			inc=SAPDBMem_NewDestroy.hpp,RTEMem_AllocatorRegister.hpp,SAPDBErr_Assertions.hpp,SAPDBMem_Exceptions.hpp,RTE_ISystem.hpp,\
                                                RTEIPC_NamedSharedMemory.hpp,RTE_Message.hpp,RTEMem_AddressIndepSHMAllocator.hpp,\
                                                RTEMem_AddressIndepSHMAllocatorHashTab.hpp,RTE_Mem_Messages.hpp,RTESync_InterlockedCounter.hpp,RTEMem_RteAllocator.hpp,SAPDB_ToString.hpp

RTEMem_AddressIndepSHMAllocatorMapper.hpp   inc=RTEMem_AddressIndepSHMAllocator.hpp,RTEMem_AddressIndepSHMAllocatorHashTab.hpp
RTEMem_AddressIndepSHMAllocatorMapper.cpp   inc=SAPDBMem_NewDestroy.hpp,RTE_ISystem.hpp,RTE_Mem_Messages.hpp,RTEIPC_NamedSharedMemory.hpp,RTESys_SecurityAttributes.hpp,\
                                                RTEMem_AddressIndepSHMAllocatorMapper.hpp,RTEMem_AddressIndepSHMAllocatorHashTab.hpp,SAPDB_ToString.hpp,\
                                                RTEMem_RteAllocator.hpp
RTEMem_PseudoStackAllocator.hpp inc=SAPDBMem_PseudoAllocator.hpp
RTEMem_PseudoStackAllocator.cpp inc=RTEMem_PseudoStackAllocator.hpp,SAPDBMem_NewDestroy.hpp
