# --------------------------------------------------------------------------
# description: Memory Management within SAPDB Kernel
#              Definition of several allocators
#
# author:	Stefan Baier
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

# Compile all modules with exception handling enabled
regex(.*\.cpp$)    -except

# define variant <devtest>
*-devtest          -DDEV_TEST

#
#-------------------------------------------------------------------------------
# Utility files
#
SAPDBMem_Messages.genmsg ascii

SAPDBMem_Messages.hpp    inc=SAPDBErr_MessageList.hpp

SAPDBMem_Exceptions.hpp  inc=SAPDB_ToString.hpp,SAPDBErr_MessageList.hpp \
                         dep=SAPDBMem_Messages.hpp

#
#-------------------------------------------------------------------------------
# Block memory allocators
#
SAPDBMem_IAllocatorInfo.hpp               inc=SAPDB_Types.hpp

SAPDBMem_IBlockAllocator.hpp              inc=SAPDBMem_IAllocatorInfo.hpp

SAPDBMem_DefaultBlockAllocator.hpp        inc=SAPDBMem_IBlockAllocator.hpp,\
                                              SAPDBMem_DefaultRawAllocator.hpp,\
                                              SAPDBMem_Exceptions.hpp,\
                                              SAPDBErr_Assertions.hpp

#
#-------------------------------------------------------------------------------
# Raw memory allocators
#
SAPDBMem_IRawAllocator.hpp                inc=SAPDBMem_IAllocatorInfo.hpp 

SAPDBMem_CallDirectory.hpp                inc=SAPDB_Types.hpp,\
                                              RTESync_InterlockedCounter.hpp,\
											  RTESync_Spinlock.hpp

SAPDBMem_RawAllocator.hpp                   inc=SAPDBMem_IRawAllocator.hpp,\
                                                SAPDBMem_IBlockAllocator.hpp,\
                                                RTE_CompilerFeatures.h,\
					                           	RTEMem_AllocatorRegister.hpp,\
                                                ggg250.h

SAPDBMem_CallDirectory.cpp                inc=SAPDBMem_CallDirectory.hpp,\
                                              RTE_CompilerFeatures.h,\
											  SAPDBMem_NewDestroy.hpp,\
											  heo670.h

SAPDBMem_DefaultRawAllocator.hpp          inc=SAPDBMem_IRawAllocator.hpp,SAPDBMem_Exceptions.hpp,\
                                              SAPDBErr_Assertions.hpp
SAPDBMem_DefaultRawAllocator.cpp          inc=SAPDBMem_DefaultRawAllocator.hpp,\
                                              SAPDBMem_NewDestroy.hpp

SAPDBMem_IncrementalBufferAllocator.hpp   inc=SAPDBMem_IRawAllocator.hpp
SAPDBMem_IncrementalBufferAllocator.cpp   inc=SAPDBMem_IncrementalBufferAllocator.hpp,SAPDBMem_Exceptions.hpp,\
                                              SAPDBErr_Assertions.hpp

SAPDBMem_IncrementalRawAllocator.hpp      inc=SAPDBMem_IRawAllocator.hpp,\
                                              SAPDBMem_IncrementalBufferAllocator.hpp
SAPDBMem_IncrementalRawAllocator.cpp      inc=SAPDBMem_IncrementalRawAllocator.hpp,\
                                              SAPDBErr_Assertions.hpp

SAPDBMem_BuddyHeader.hpp
SAPDBMem_BuddyBlock.hpp                   inc=SAPDBMem_IRawAllocator.hpp,\
                                              SAPDBErr_Assertions.hpp

SAPDBMem_BuddyAllocator.hpp               inc=SAPDBMem_BuddyHeader.hpp,SAPDBMem_BuddyBlock.hpp,\
                                              SAPDBMem_IRawAllocator.hpp,SAPDBMem_IBlockAllocator.hpp,\
                                              SAPDB_IntegerArithmetic.hpp
SAPDBMem_BuddyAllocator.cpp               inc=SAPDBMem_BuddyAllocator.hpp

SAPDBMem_PageAllocator.hpp                inc=SAPDBMem_IRawAllocator.hpp
SAPDBMem_PageAllocator.cpp                inc=SAPDBMem_PageAllocator.hpp,SAPDBErr_Assertions.hpp

SAPDBMem_RawAllocator-k.cpp               -DKERNEL -DOMS_IMPLEMENTATION \
                                          inc=SAPDBMem_RawAllocator.hpp,\
                                          SAPDBErr_Assertions.hpp,\
                                          SAPDBMem_Exceptions.hpp,\
                                          SAPDBTrace_Topic.hpp,\
                                          SAPDBTrace_Usage.hpp,\
                                          RTESync_Spinlock.hpp,\
						                  RTE_Crash.hpp,\
						                  RTEMem_AllocatorRegister.hpp,\
                                          RTE_IInterface.hpp,\
						                  SAPDBMem_CallDirectory.hpp,\
                                          Kernel_OpMsg.hpp,\
                                          RTEConf_ParameterAccessKernelInterface.hpp,\
                                          SAPDB_MemCopyMove.hpp,\
                                          OMS_DbpError.hpp,\
                                          RTETask_ITask.hpp,\
                                          SAPDB_OStream.hpp,\
						                  heo670.h,\
                                          geo573.h,\
                                          heo52.h,\
                                          heo66.h,\
                                          hsp77.h,\
                                          ggg01,\
                                          hxt12.h dep=heo07.h

SAPDBMem_RawAllocator.cpp                 inc=SAPDBMem_RawAllocator.hpp,\
                                          SAPDBErr_Assertions.hpp,\
                                          SAPDBMem_Exceptions.hpp,\
                                          SAPDBTrace_Topic.hpp,\
                                          SAPDBTrace_Usage.hpp,\
                                          RTESync_Spinlock.hpp,\
						                  RTE_Crash.hpp,\
						                  RTEMem_AllocatorRegister.hpp,\
                                          RTE_IInterface.hpp,\
						                  SAPDBMem_CallDirectory.hpp,\
                                          Kernel_OpMsg.hpp,\
                                          RTEConf_ParameterAccessKernelInterface.hpp,\
                                          SAPDB_MemCopyMove.hpp,\
                                          OMS_DbpError.hpp,\
                                          RTETask_ITask.hpp,\
                                          SAPDB_OStream.hpp,\
						                  heo670.h,\
                                          geo573.h,\
                                          heo52.h,\
                                          heo66.h,\
                                          hsp77.h,\
                                          ggg01,\
                                          hxt12.h dep=heo07.h


SAPDBMem_RawAllocator-o.cpp               -DSAPDBMEM_RAWALLOCATOR_NAMESPACE=OMS_Namespace \
                                          inc=SAPDBMem_RawAllocator.hpp,\
                                          SAPDBErr_Assertions.hpp,\
                                          SAPDBMem_Exceptions.hpp,\
                                          SAPDBTrace_Topic.hpp,\
                                          SAPDBTrace_Usage.hpp,\
                                          RTESync_Spinlock.hpp,\
						                  RTE_Crash.hpp,\
						                  RTEMem_AllocatorRegister.hpp,\
                                          RTE_IInterface.hpp,\
						                  SAPDBMem_CallDirectory.hpp,\
                                          Kernel_OpMsg.hpp,\
                                          RTEConf_ParameterAccessKernelInterface.hpp,\
                                          SAPDB_MemCopyMove.hpp,\
                                          OMS_DbpError.hpp,\
                                          RTETask_ITask.hpp,\
                                          SAPDB_OStream.hpp,\
						                  heo670.h,\
                                          geo573.h,\
                                          heo52.h,\
                                          heo66.h,\
                                          hsp77.h,\
                                          ggg01,\
                                          hxt12.h dep=heo07.h



SAPDBMem_SynchronizedRawAllocator.hpp     inc=SAPDBMem_RawAllocator.hpp,RTESync_NamedSpinlock.hpp 

#
#-------------------------------------------------------------------------------
# New and Destroy
#
SAPDBMem_NewDestroy.hpp                   inc=SAPDBMem_IRawAllocator.hpp,\
                                              SAPDBErr_Assertions.hpp,\
                                              SAPDB_AutoPtr.hpp

#
#-------------------------------------------------------------------------------
# Typed allocators
#
SAPDBMem_Allocator.hpp               inc=SAPDB_IntegerArithmetic.hpp,SAPDBMem_Exceptions.hpp
SAPDBMem_RawToTypedAllocator.hpp     inc=SAPDB_IntegerArithmetic.hpp,SAPDBMem_IRawAllocator.hpp

#
#-------------------------------------------------------------------------------
# Test program
#
SAPDBMem_Test.cpp  inc=SAPDBMem_RawAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTEMem_Allocator.hpp,SAPDBMem_Test.hpp

SAPDBMem_PseudoAllocator.hpp inc=SAPDBMem_IRawAllocator.hpp,RTESync_NamedSpinlock.hpp
SAPDBMem_PseudoAllocator.cpp inc=SAPDBMem_PseudoAllocator.hpp,RTEMem_AllocatorRegister.hpp,\
						RTEMem_Allocator.hpp,SAPDB_string.h

SAPDBMem_DoublyLinked.hpp inc=
SAPDBMem_VirtualAddressSpace.hpp inc=SAPDB_Types.hpp,SAPDBMem_DoublyLinked.hpp
SAPDBMem_VirtualAddressSpace.cpp inc=SAPDBMem_VirtualAddressSpace.hpp,SAPDBMem_IRawAllocator.hpp,\
						SDBMsg_SAPDBMem.h,SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,SAPDB_sprintf.h
