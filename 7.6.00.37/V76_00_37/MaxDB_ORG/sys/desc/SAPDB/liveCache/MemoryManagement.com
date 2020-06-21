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

*.cpp -DOMS_IMPLEMENTATION

LVCMem_Wrapper.hpp            inc=heo38.h,geo38.h,heo51.h,heo58.h,geo573.h,hak260.h

LVCMem_Allocator.hpp          inc=SAPDBMem_IRawAllocator.hpp,\
                               SAPDBMem_SynchronizedRawAllocator.hpp,\
							   LVCMem_Wrapper.hpp,\
							   SAPDBMem_CallDirectory.hpp,\
							   Container_Vector.hpp,\
							   heo670.h

LVCMem_BlockAllocator.hpp     inc=SAPDB_Types.hpp,\
                               SAPDBMem_IBlockAllocator.hpp,\
                               RTESync_NamedSpinlock.hpp
LVCMem_DefaultAllocator.hpp   inc=SAPDBMem_IRawAllocator.hpp


LVCMem_EmergencyAllocator.hpp inc=SAPDBMem_RawAllocator.hpp,\
                                  gsp00

LVCMem_Allocator.cpp -except      inc=LVCMem_Allocator.h,\
                                      LVCMem_Allocator.hpp,\
                                      LVCMem_BlockAllocator.hpp,\
							          LVCMem_EmergencyAllocator.hpp,\
									  LVC_LibOmsInterface.hpp,\
                                      SAPDBMem_NewDestroy.hpp,\
                                      SAPDBMem_Exceptions.hpp,\
							          RTESync_NamedSpinlock.hpp,\
                                      RTE_Crash.hpp,\
                                      RTE_Message.hpp,\
                                      Kernel_OpMsg.hpp,\
                                      SDBMsg_LiveCache.h,\
									  OMS_Types.hpp,\
									  SQLMan_Context.hpp,\
									  geo573.h,\
                                      heo670.h,\
                                      hsp77.h,\
                                      OMS_DbpError.hpp,\
                                      KernelMem_RawAllocatorTracer.hpp
									  
LVCMem_Allocator-t.cpp -except    -DOMSTST inc=LVCMem_Allocator.h,\
                                      LVCMem_Allocator.hpp,\
                                      LVCMem_BlockAllocator.hpp,\
							          LVCMem_EmergencyAllocator.hpp,\
									  LVC_LibOmsInterface.hpp,\
                                      SAPDBMem_NewDestroy.hpp,\
                                      SAPDBMem_Exceptions.hpp,\
							          RTESync_NamedSpinlock.hpp,\
							          RTE_Message.hpp,\
                                      RTE_Crash.hpp,\
                                      Kernel_OpMsg.hpp,\
                                      SDBMsg_LiveCache.h,\
									  OMS_Types.hpp,\
									  SQLMan_Context.hpp,\
									  geo573.h,\
                                      heo670.h,\
                                      hsp77.h,\
                                      OMS_DbpError.hpp,\
                                      KernelMem_RawAllocatorTracer.hpp
									  

LVCMem_BlockAllocator.cpp -except inc=RTE_CompilerFeatures.h,\
                                      SAPDBMem_NewDestroy.hpp,\
                                      LVCMem_BlockAllocator.hpp,\
                                      RTEMem_BlockAllocator.hpp,\
									  RTEMem_AllocatorRegister.hpp,\
									  Kernel_OpMsg.hpp,\
                                      KernelCommon/Kernel_VTrace.hpp,\
                                      SDBMsg_LiveCache.h,\
                                      RTE_Message.hpp

LVCMem_DefaultAllocator.cpp -except inc=LVCMem_DefaultAllocator.hpp,\
                                         geo573.h

LVCMem_EmergencyAllocator.cpp     inc=LVCMem_EmergencyAllocator.hpp,\
                                      LVCMem_BlockAllocator.hpp,\
									  hgg08.h,\
									  heo51.h,\
									  heo55k.h



LVCMem_Allocator.exp inc= ascii



