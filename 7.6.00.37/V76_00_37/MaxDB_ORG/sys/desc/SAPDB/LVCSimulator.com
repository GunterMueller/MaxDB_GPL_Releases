# @(#)   desc/SAPDB/LVCSimulator.mac    7.4   01-Jul-2002
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

*       -DLVCSIM_DLL_VMAKE_BUILD -DOMS_IMPLEMENTATION

LVCSim_Client.hpp           ->$INSTROOT/incl/livecachesimul.h \
                            dep=OMS_ClientInterface.hpp,OMS_LiveCacheVersion.hpp
LVCSim_FastHash.hpp         inc=LVCSim_Internal.hpp,LVCSim_CheckpointIO.hpp
LVCSim_Internal.hpp         inc=LVCSim_Client.hpp,RTESync_Spinlock.hpp,\
                                LVCSim_CheckpointIO.hpp,ggg00,ggg01,\
                                LVC_LibOmsInterface.hpp,geo573.h
LVCSim_KernelInterface.hpp  inc=LVC_KernelInterface.hpp,LVC_LockRequest.hpp,\
                                LVCSim_RWLockManager.hpp,SAPDBMem_DefaultRawAllocator.hpp
LVCSim_liveCacheSink.hpp    inc=LVCSim_Internal.hpp,LVCSim_FastHash.hpp,\
                                OMS_RawAllocator.hpp,LVCSim_ProcServerInterface.hpp,\
                                LVCSim_CheckpointIO.hpp,LVC_IliveCacheSink.hpp,geo07.h
LVCSim_ObjectHandle.hpp     inc=LVCSim_ObjectVerHandle.hpp,LVCSim_CheckpointIO.hpp,\
                                LVCSim_liveCacheSink.hpp
LVCSim_ObjectTree.hpp       inc=LVCSim_ObjectHandle.hpp,LVCSim_CheckpointIO.hpp,\
                            SAPDB_MemCopyMove.hpp
LVCSim_ObjectVerHandle.hpp  inc=LVCSim_Internal.hpp,LVCSim_OIDAllocator.hpp,\
                                LVCSim_CheckpointIO.hpp,SAPDB_MemCopyMove.hpp
LVCSim_OIDAllocator.hpp     inc=LVCSim_CheckpointIO.hpp
LVCSim_DebugAllocator.hpp   inc=SAPDBMem_IRawAllocator.hpp
LVCSim_DebugCallback.hpp    inc=MemDbg_Callback.hpp
LVCSim_ProcServerInterface.hpp inc=LVC_ProcServerInterface.hpp,\
                               LVCSim_Client.hpp

LVCSim_Compat.cpp           -except inc=LVCSim_Client.hpp,RTESync_RWRegion.hpp,\
                                RTESync_Spinlock.hpp,LVCSim_KernelInterface.hpp

LVCSim_GlobFnc.cpp          -except inc=LVCSim_Internal.hpp,\
                                LVCSim_liveCacheSink.hpp,\
                                LVCSim_KernelInterface.hpp,\
                                LVCSim_OIDAllocator.hpp,\
                                LVCSim_DebugAllocator.hpp,\
                                LVCSim_DebugCallback.hpp,\
                                OMS_DbpGlobals.hpp,\
                                heo07.h,heo670.h,hsp77.h,\
                                LVC_Dispatcher.hpp,\
                                MSSim_Interface.hpp
LVCSim_KernelInterface.cpp  -except inc=LVCSim_Internal.hpp,\
                                LVCSim_KernelInterface.hpp,\
                                LVCSim_DebugAllocator.hpp,\
                                OMS_StackTraceCallbackInterface.hpp,\
                                RTESys_MicroTime.h,RTE_KernelInterfaceInstance.hpp,heo670.h,hsp77.h
LVCSim_DebugAllocator.cpp   -except inc=LVCSim_DebugAllocator.hpp,\
                                RTESync_Spinlock.hpp,hsp77.h,geo573.h,\
                                LVCSim_Internal.hpp,heo670.h,\
                                LVCSim_DebugCallback.hpp,\
                                SAPDB_MemCopyMove.hpp,\
                                MemDbg_Interface.hpp,MemDbg_Config.hpp
LVCSim_DebugCallback.cpp    -except inc=LVCSim_DebugCallback.hpp,\
                                MemDbg_Interface.hpp,\
                                RTESync_Spinlock.hpp,heo670.h,hsp77.h
LVCSim_liveCacheSink.cpp    -except inc=LVCSim_liveCacheSink.hpp,\
                                LVCSim_ObjectHandle.hpp,\
                                LVCSim_ObjectTree.hpp,\
                                LVCSim_KernelInterface.hpp,OMS_Session.hpp,\
                                LVCSim_ProcServerInterface.hpp,\
                                LVC_ProcServerExecutor.hpp,\
                                LVC_LoadComponentLibrary.hpp,\
                                LVCPS_ParamHandler.hpp,\
                                LVC_Dispatcher.hpp,OMS_VarObj.hpp,\
                                heo52.h,gsp01.h,heo01.h,hsp77.h,heo02x.h,\
                                OMS_VarObjByClsIter.hpp,SAPDB_MemCopyMove.hpp
LVCSim_Main.cpp             -except inc=LVCSim_Internal.hpp
LVCSim_ObjectHandle.cpp     -except inc=LVCSim_Internal.hpp,LVCSim_ObjectHandle.hpp,\
                                LVCSim_OIDAllocator.hpp,\
                                LVCSim_liveCacheSink.hpp,\
                                LVCSim_ObjectTree.hpp
LVCSim_ObjectTree.cpp       -except inc=LVCSim_Internal.hpp,\
                                LVCSim_ObjectTree.hpp
LVCSim_ObjectVerHandle.cpp  -except inc=LVCSim_ObjectVerHandle.hpp,\
                                LVCSim_OIDAllocator.hpp
LVCSim_OIDAllocator.cpp     -except inc=LVCSim_Internal.hpp,\
                                LVCSim_OIDAllocator.hpp,\
                                LVCSim_ObjectHandle.hpp
LVCSim_CheckpointIO.cpp     -except inc=LVCSim_CheckpointIO.hpp

LVCSim_ProcServerInterface.cpp -except inc=LVCSim_ProcServerInterface.hpp,\
                               LVCPS_LiveCacheSink.hpp,LVCPS_Dispatcher.hpp

LVCSim_RWLockManager.hpp               inc=RTESync_RWRegion.hpp,RTESync_Spinlock.hpp
LVCSim_RWLockManager.cpp               inc=LVCSim_RWLockManager.hpp,LVC_LockRequest.hpp
