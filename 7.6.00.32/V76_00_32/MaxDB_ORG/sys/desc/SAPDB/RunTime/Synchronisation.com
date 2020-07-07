#
# RunTime Synchronization Modules
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

* -except
#
# No exception handling code variant
#
*-noex -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
#
#
# RTESync_Spinlock,RTESync_LockedScope class
RTESync_Spinlock.hpp inc=RTE_Types.hpp,RTESys_Spinlock.h,RTE_IInterface.hpp,SAPDB_MemCopyMoveNoCheck.hpp
RTESync_NamedSpinlock.hpp inc=SAPDBMem_NewDestroy.hpp,RTESync_Spinlock.hpp,RTESync_SpinlockRegister.hpp
RTESync_SpinlockRegister.hpp inc=RTE_ItemRegister.hpp
RTESync_SpinlockRegister.cpp inc=RTESync_SpinlockRegister.hpp,SAPDBMem_NewDestroy.hpp,RTEMem_RteAllocator.hpp,SAPDB_MemCopyMoveNoCheck.hpp
#
# RTESync_ExclusiveRegion and RTESync_Exclusive class
RTESync_IExclusiveRegion.hpp inc=RTE_Types.hpp,RTETask_IContext.hpp
RTESync_ExclusiveRegion.hpp inc=gsp00,RTESync_IExclusiveRegion.hpp
RTESync_ExclusiveRegion*.cpp inc=RTESync_ExclusiveRegion.hpp,SAPDBErr_Assertions.hpp
#
# RTESync_SharedRegion and RTESync_Shared class
RTESync_ISharedRegion.hpp inc=RTE_Types.hpp
RTESync_SharedRegion.hpp inc=RTESync_ISharedRegion.hpp
RTESync_SharedRegion*.cpp inc=RTESync_SharedRegion.hpp,SAPDBErr_Assertions.hpp
#

RTESync_RWRegion.hpp inc=SAPDB_Types.hpp,RTE_Types.hpp,RTESync_NamedSpinlock.hpp,RTESync_SpinlockPool.hpp

&if $OSSPEC in [ WIN32 ]
RTESync_RWRegion.cpp -Ow
&endif

RTESync_RWRegion.cpp inc=RTESync_RWRegion.hpp,RTEMem_RteAllocator.hpp,RTE_CallDebugger.h,RTESys_Spinlock.h,RTESync_SpinlockPoolRegister.hpp,SAPDB_sprintf.h,SAPDBMem_NewDestroy.hpp,RTE_Message.hpp,RTE_MessageList.hpp,SAPDBTrace_Usage.hpp,SAPDBTrace_Topic.hpp,geo002.h,geo50_0.h,RTETask_Task.hpp,RTETask_ITask.hpp,RTETask_LegacyTaskCtrl.h


# RTESync_Factory class
RTESync_Factory.hpp inc=RTE_Types.hpp
RTESync_Factory*.cpp inc=SAPDBMem_NewDestroy.hpp,RTESync_Factory.hpp,RTESync_ExclusiveRegion.hpp,RTESync_SharedRegion.hpp,RTEMem_Allocator.hpp
#
# RTESync_InterlockedCounter class
RTESync_InterlockedCounter.hpp inc=SAPDB_Types.hpp,RTESync_Spinlock.hpp

RTESync_SpinlockPool.hpp        inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_ItemRegister.hpp,RTEMem_RteAllocator.hpp,\
                                    RTESync_RWRegionRegister.hpp
RTESync_SpinlockPool.cpp        inc=RTESync_SpinlockPool.hpp,\
                                    RTESync_NamedSpinlock.hpp,\
                                    RTESync_SpinlockPoolRegister.hpp,\
                                    RTE_Message.hpp,\
                                    RTE_MessageList.hpp,\
                                    RTEMem_RteAllocator.hpp,\
                                    SAPDB_sprintf.h,\
                                    SAPDBMem_NewDestroy.hpp,\
                                    SAPDBMem_IRawAllocator.hpp
# (* PTS 115043 U.J. *)
RTESync_SpinlockReader.hpp      inc=RTESync_SpinlockRegister.hpp

RTESync_SpinlockReader.cpp      inc=ggg00,gsp00,\
                                    RTESync_SpinlockRegister.hpp,\
                                    RTESync_SpinlockReader.hpp
                                    
# Binary Semaphore class
RTESync_BinarySemaphore.hpp		inc=RTE_MessageList.hpp,SAPDB_ToString.hpp,RTE_TagFileHandlingUNIX.hpp
RTESync_BinarySemaphore.cpp		inc=RTESys_SecurityAttributes.hpp,RTESync_BinarySemaphore.hpp,RTE_GlobalNameSpaceNT.h,RTE_Message.hpp,RTE_Sync_Messages.hpp,\
									RTE_Messages.hpp,RTEMem_RteAllocator.hpp,SAPDB_sprintf.h,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp\
&if $OSSPEC not in [ WIN32 ]
								,RTE_saveUNIXcalls.h,RTE_UNIXAuthenticate.hpp	
&endif

RTESync_TestSemaphore.cpp		inc=

RTESync_SpinlockPoolRegister.hpp inc=SAPDB_Types.hpp,RTE_Types.hpp,RTESync_NamedSpinlock.hpp
RTESync_SpinlockPoolRegister.cpp inc=RTESync_SpinlockPoolRegister.hpp,RTESync_RWRegion.hpp,RTEMem_RteAllocator.hpp
   
