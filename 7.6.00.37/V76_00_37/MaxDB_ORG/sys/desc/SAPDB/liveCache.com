# ------------------------------------------------------------------------------
#
# LiveCache Kernel-side Library Descriptions
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

regex(.*\.cpp$)    -except -DOMS_IMPLEMENTATION

# -------------------------------------------------------------------------------------
# auxiliary structures:
# -------------------------------------------------------------------------------------

LVC_ContainerManager.hpp            inc=SAPDB_Types.hpp

LVC_ErrorTraceObject.hpp            inc=ggg00,ggg12,ggg92.h,hgg01.h,gbd900.h,Trace_Entry.hpp,\
										FileDir_Types.hpp

LVC_HistoryNode.hpp                 inc=ggg00,gbd00,hbd13.h 

LVC_Types.hpp                       inc=gbd900.h,SAPDBErr_MessageList.hpp

LVC_KeyOIdQueue.hpp                 inc=ggg92.h

LVC_ObjKeyIterator.hpp              inc=gbd300.h,gbd500.h,gbd510.h,gbd600.h,\
                                        ggg00,ggg92.h

LVC_AbstractObjKeyIterator.hpp      inc=SAPDBMem_IRawAllocator.hpp

LVC_SingleFileObjKeyIterator.hpp    inc=LVC_AbstractObjKeyIterator.hpp,\
                                        SAPDBMem_IRawAllocator.hpp,\
                                        LVC_KeyOIdQueue.hpp
                                        
LVC_MultiFileObjKeyIterator.hpp     inc=ggg200.h,ggg92.h,\
                                        LVC_AbstractObjKeyIterator.hpp,\
                                        LVC_BufferedObjKeyIterator.hpp,\
                                        SAPDBMem_NewDestroy.hpp

LVC_WaitForDelObjFileQueue.hpp      inc=ggg00,\
                                        hgg08.h,\
                                        heo55k.h,\
 										heo56.h,\
                                        SAPDB_Types.hpp,\
 										RTEMem_Allocator.hpp

LVC_BufferedObjKeyIterator.hpp      inc=SAPDBMem_IRawAllocator.hpp,\
                                        LVC_KeyOIdQueue.hpp

LVC_LibOmsInterface.hpp             inc=OMS_LibOmsInterface.hpp

LVC_KernelInterface.hpp             inc=gsp00,RTEMem_AllocatorRegister.hpp

LVC_KernelInterfaceInstance.hpp     inc=LVC_KernelInterface.hpp,LVC_LockRequest.hpp,LVC_RWLockManager.hpp

LVC_ContainerManager.cpp            inc=LVC_ContainerManager.hpp,\
                                        FileDir_IDirectory.hpp,\
                                        FileDir_Oms.hpp,\
                                        SQLMan_Types.hpp,\
                                        SQLMan_Context.hpp,\
                                        Catalog_ISessionCache.hpp,\
                                        Catalog_IAuthorization.hpp,\
                                        Catalog_AuthorizationObject.hpp,\
                                        Catalog_ContainerObject.hpp,\
                                        Catalog_Cache.hpp,\
                                        SAPDB_MemCopyMove.hpp,\
                                        Kernel_VTrace.hpp,\
                                        Msg_List.hpp,\
                                        OMS_Defines.h,\
                                        SDBMsg_LiveCache.h,\
                                        hak51.h,\
                                        ggg01,\
                                        hkb04.h,\
                                        hbd01_1.h

LVC_RWLockManager.hpp               inc=RTESync_RWRegion.hpp,RTESync_Spinlock.hpp
LVC_RWLockManager.cpp               inc=LVC_RWLockManager.hpp,LVC_LockRequest.hpp,hsp77.h

LVC_LockRequest.hpp                 inc=ggg00

LVC_SingleFileObjKeyIterator.cpp inc=gbd900.h,hbd03.h,ggg01,\
                                        RTE_Message.hpp,\
                                        LVC_SingleFileObjKeyIterator.hpp,\
                                        LVC_ObjKeyIterator.hpp,\
                                        LVC_Types.hpp,\
                                        FileDir_IDirectory.hpp,\
                                        FileDir_OmsKeyPartition.hpp,\
                                        FileDir_OmsKey.hpp,\
                                        Msg_List.hpp,\
                                        SDBMsg_LiveCache.h
                                        
LVC_BufferedObjKeyIterator.cpp inc=gbd900.h,hbd03.h,ggg01,\
                                        RTE_Message.hpp,\
                                        LVC_BufferedObjKeyIterator.hpp,\
                                        LVC_ObjKeyIterator.hpp,\
                                        LVC_Types.hpp,\
                                        FileDir_IDirectory.hpp,\
                                        FileDir_OmsKeyPartition.hpp,\
                                        FileDir_OmsKey.hpp,\
                                        Msg_List.hpp,\
                                        SDBMsg_LiveCache.h

LVC_KernelInterfaceInstance.cpp     inc=LVC_KernelInterfaceInstance.hpp,\
                                        LVCMem_Wrapper.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        LVCMem_Allocator.hpp,\
                                        OMS_StackTraceCallbackInterface.hpp,OMS_DbpError.hpp,\
                                        RTEConf_ParameterAccessKernelInterface.hpp,KernelMem_RawAllocatorTracer.hpp,\
                                        RTESys_MicroTime.h,RTE_IInterface.hpp,Kernel_VTrace.hpp,hsp77.h,\
                                        vak001,hgg11.h,heo52.h,heo56.h,heo58.h,hgg01.h,hgg01_3.h,geo573.h,\
                                        heo670.h,Kernel_OpMsg.hpp,SAPDBMem_Exceptions.hpp,heo51.h


LVC_LibOmsInterface.cpp             inc=LVC_LibOmsInterface.hpp

LVC_NewObjectChecker.hpp            inc=ggg250.h

LVC_IliveCacheSink.hpp              inc=LVC_MethodNames.hpp,OMS_Defines.h,geo00.h,ggg00,ggg01,\
                                    OMS_IterDirection.hpp,OMS_ContainerHandle.hpp \
                                    dep=OMS_LiveCacheTypes.xml


LVC_DispatcherHashEntry.hpp         inc=OMS_Defines.h,gsp00
LVC_DispatcherHashEntry.cpp         inc=LVC_DispatcherHashEntry.hpp

LVC_DispatcherCOMClassHash.hpp      inc=LVC_DispatcherHashEntry.hpp
LVC_DispatcherCOMClassHash.cpp      inc=LVC_DispatcherCOMClassHash.hpp

LVC_Dispatcher.hpp                  inc=Container_Hash.hpp,LVC_DispatcherCOMClassHash.hpp,\
&if $OSSPEC = WIN32
                                    gos00.h,\
&else
                                    gen00.h,\
&endif
                                    RTESync_Spinlock.hpp,vak001 dep=OMS_IliveCacheSpec.hpp
LVC_Dispatcher.cpp                  inc=LVC_Dispatcher.hpp,RTESys_SharedLib.h,RTE_Message.hpp,\
                                    LVC_DispatcherVTable.hpp,SAPDBMem_IRawAllocator.hpp,\
                                    OMS_CallbackInterface.hpp,OMS_DbpError.hpp,hsp77.h,\
                                    LVC_LoadComponentLibrary.hpp,hgg01_3.h,\
                                    geo00d.h,geo007_1.h,heo38.h,\
                                    heo01.h,vak001,geo38.h,geo573.h,hak263.h,hak341.h,\
                                    LVC_ProcServerExtender.hpp,OMS_Defines.h

LVC_InProcDispatcher.hpp            inc=LVC_Dispatcher.hpp,ggg91.h,heo52.h
LVC_InProcDispatcher.cpp            inc=LVC_InProcDispatcher.hpp,LVC_ProcServerExtender.hpp,\
                                    LVCMem_Allocator.hpp,LVC_DispatcherParams.hpp,\
                                    RTESync_Spinlock.hpp,OMS_DbpError.hpp,\
                                    LVC_LoadComponentLibrary.hpp,LVC_LoadLibrary.hpp,\
                                    SAPDB_UTF8Basis.hpp,hgg01_3.h,\
                                    heo38.h,geo60.h,hsp77.h,\
                                    geo00d.h,geo50_0.h,geo007_1.h,hgg08.h,\
                                    heo55k.h,heo52.h,heo58.h,heo15.h,heo01.h,\
                                    gsp92,heo922.h,hgg01.h,heo52.h,hgg01_3.h,\
                                    RTEConf_ParameterAccessKernelInterface.hpp,OMS_Defines.h,\
&if $OSSPEC = WIN32
                                    hos38.h
&else
                                    hen50.h
&endif
LVC_DispatcherParams.hpp            inc=OMS_Defines.h,gsp00,hsp26.h
LVC_DispatcherParams.cpp            inc=LVC_DispatcherParams.hpp,LVC_LoadComponentLibrary.hpp,\
                                    geo00d.h,vak001,geo60.h,geo007_1.h

LVC_ProcServerInterface.hpp         inc=OMS_Defines.h,OMS_DbpError.hpp,gsp00,ggg00,ggg01,ggg91.h
LVC_ProcServerInterface.cpp         inc=LVC_ProcServerInterface.hpp,LVC_ProcServerExecutor.hpp,\
                                    SAPDBMem_RawAllocator.hpp

LVC_ProcServerInterfaceInstance.hpp inc=LVC_ProcServerInterface.hpp
LVC_ProcServerInterfaceInstance.cpp inc=LVC_ProcServerInterfaceInstance.hpp,RTETask_ITask.hpp,\
                                    RTETask_ITaskClientConnection.hpp,RTE_Message.hpp,\
                                    RTEUDE_ServerControl.hpp,RTETask_Task.hpp

LVC_ProcServerExecutor.hpp          inc=LVC_ProcServerInterface.hpp
LVC_ProcServerExecutor.cpp          inc=LVC_ProcServerExecutor.hpp,vak001,LVCPS_PacketConsoleMsg.hpp,\
                                    LVCPS_PacketSingleObj.hpp,LVCPS_PacketContainer.hpp,\
                                    LVCPS_PacketVersion.hpp,LVCPS_PacketSchema.hpp,\
                                    LVCPS_PacketRegion.hpp,LVCPS_PacketSubtrans.hpp,\
                                    LVCPS_PacketConsistentView.hpp,LVCPS_PacketMassObj.hpp,\
                                    LVCPS_PacketIterator.hpp,LVCPS_PacketSequence.hpp,\
                                    LVCPS_PacketFile.hpp,LVCPS_PacketFile.hpp,\
                                    LVCPS_PacketABAPStream.hpp,LVCPS_PacketTasking.hpp,\
                                    LVCPS_PacketSQL.hpp,LVCPS_PacketVersionDict.hpp \
                                    -DKERNEL_LZU

LVC_ProcServerExtender.hpp          inc=LVC_ProcServerInterfaceInstance.hpp,LVC_Dispatcher.hpp,\
                                    LVC_ProcServerExecutor.hpp,OMS_DbpBase.hpp,\
                                    OMS_DbpGlobals.hpp,geo38.h
LVC_ProcServerExtender.cpp          inc=LVC_ProcServerExtender.hpp,hsp77.h

LVC_LoadComponentLibrary.hpp        inc=gsp00,OMS_Defines.h
LVC_LoadComponentLibrary.cpp        inc=LVC_LoadComponentLibrary.hpp,geo00d.h,gip00.h,hgg11.h,\
                                    LVC_LoadLibrary.hpp,SAPDBAlgo_MD5File.h,heo02x.h,geo573.h,\
                                    OMS_DbpError.hpp,LVC_ILoadComponentLibraryCache.hpp

LVC_LoadLibrary.hpp                 inc=heo02x.h
LVC_LoadLibrary.cpp                 inc=LVC_LoadLibrary.hpp,LVC_Dispatcher.hpp,\
                                    OMS_DbpError.hpp,ggg251.h,ggg00,ggg01,geo007_1.h,\
                                    geo60.h,hsp77.h,heo670.h

LVC_FileDirCache.hpp                inc=LVC_IFileDirCache.hpp,\
									SAPDBTrace_Topic.hpp,\
									SAPDBTrace_Usage.hpp,\
								    Container_Hash.hpp,\
								    FileDir_Oms.hpp
									
LVC_IFileDirCache.hpp               inc=FileDir_IDirectory.hpp,FileDir_Oms.hpp,\
									FileDir_FileNo.hpp,\
								    SAPDB_RefCountingSmartPtr.hpp

LVC_FileDirCache.cpp                inc=hkb50.h,hgg06.h,\
									LVC_FileDirCache.hpp,\
									FileDir_Iterator.hpp,\
									FileDir_IDirectory.hpp,\
								    Kernel_IAdminInfo.hpp,\
								    RTEMem_Allocator.hpp,\
								    SAPDBMem_NewDestroy.hpp \
                                    -DFDIR_DEV

LVC_ILoadComponentLibraryCache.hpp  inc=gsp00,OMS_Defines.h
LVC_LoadComponentLibraryCache.hpp   inc=LVC_ILoadComponentLibraryCache.hpp,SAPDB_Singleton.hpp,\
                                    Container_Hash.hpp,OMS_Defines.h
LVC_LoadComponentLibraryCache.cpp   inc=LVC_LoadComponentLibraryCache.hpp,\
                                    RTESync_Spinlock.hpp,\
                                    SAPDBMem_NewDestroy.hpp,\
                                    RTEMem_Allocator.hpp
