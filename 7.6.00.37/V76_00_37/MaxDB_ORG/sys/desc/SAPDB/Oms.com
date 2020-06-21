#
#
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

&ifdebug
*.c -DOMS_DEBUG -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
*.cpp -DOMS_DEBUG -except -DOMS_IMPLEMENTATION -DSQLCLS_IMPL 
&else
*.cpp -except -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
&endif

# these are the right file descriptions for
# OMS_IliveCacheSpec.idl, OMS_LiveCacheTypes.idl as of vmake version 9.2.5
#

OMS_IliveCacheSpec.hpp   dep=OMS_LiveCacheTypes.xml unpack=no concom=yes \
                         ->$INSTROOT/incl/ilivecachesink.h extdep=$INSTROOT/incl/ilivecachesink.h

# XML schema 
OMS_CppBaseDecl.xsd             ->$INSTROOT/incl/CppBaseDecl.xsd ascii
OMS_CppApplInterface.xsd        ->$INSTROOT/incl/CppApplInterface.xsd ascii \
                                dep=OMS_CppBaseDecl.xsd
OMS_CppBaseDeclExt.xsd          ->$INSTROOT/incl/CppBaseDeclExt.xsd ascii \
                                dep=OMS_CppBaseDecl.xsd
OMS_CppClassDecl.xsd            ->$INSTROOT/incl/CppClassDecl.xsd ascii \
                                dep=OMS_CppBaseDeclExt.xsd

OMS_LiveCacheTypes.idl   ascii unpack=yes concom=yes \
&if (not defined BIT64) and ($MACH in [_IBMR2, SUN, PA20W, PA11])
                         ->$WRK/incl/livecachetypes.idl
&else
                         ->$INSTROOT/incl/livecachetypes.idl
&endif

OMS_LiveCacheTypes.xml   lang=r unpack=no concom=yes obj= -nocompile -xml2ispc \
                         dep=OMS_CppApplInterface.xsd,OMS_CppClassDecl.xsd,OMS_LiveCacheTypes.idl \
&if (not defined BIT64) and ($MACH in [_IBMR2, SUN, PA20W, PA11])
                         -header=$WRK/incl/livecachetypes.h \
                         -copy=$WRK/incl/livecachetypes.xml \
                         extdep=$WRK/incl/livecachetypes.h,$WRK/incl/livecachetypes.xml
&else
                         -header=$INSTROOT/incl/livecachetypes.h,$SAPDBSDK/incl/livecachetypes.h \
                         -copy=$INSTROOT/incl/livecachetypes.xml \
                         extdep=$INSTROOT/incl/livecachetypes.h,$SAPDBSDK/incl/livecachetypes.h,\
                                $INSTROOT/incl/livecachetypes.xml
&endif
&if ($OSSPEC!=WIN32 or defined BIT64) and ($MACH!=SUN)
OMS_LiveCacheTypes.xml -novalidate
&endif

OMS_LiveCacheVersion.hpp        ->$INSTROOT/incl/livecacheversion.h


# normal implementation files:

OMS_AbstractObject.hpp          inc=OMS_Oid.hpp,OMS_Defines.h,OMS_ObjByClsIterBase.hpp \
                                ->$INSTROOT/incl/Oms/OMS_AbstractObject.hpp
OMS_AbstractObject.cpp          inc=OMS_AbstractObject.hpp,OMS_Handle.hpp,OMS_Trace.hpp,\
                                OMS_Session.hpp,OMS_ObjectContainer.hpp,OMS_ContainerEntry.hpp,\
                                OMS_ClassEntry.hpp,OMS_KeyDesc.hpp
OMS_AbstractObject-t.cpp        -DOMSTST inc=OMS_AbstractObject.hpp,OMS_Handle.hpp,OMS_Trace.hpp,\
                                OMS_Session.hpp,OMS_ObjectContainer.hpp,OMS_ContainerEntry.hpp,\
                                OMS_ClassEntry.hpp,OMS_KeyDesc.hpp

OMS_AFX.h                       concom=yes ->$INSTROOT/incl/Oms/OMS_AFX.h

OMS_AllocatorInterface.hpp      inc=OMS_AFX.h,OMS_Defines.h ->$INSTROOT/incl/Oms/OMS_AllocatorInterface.hpp
OMS_AllocatorInterface.cpp      inc=OMS_AllocatorInterface.hpp
OMS_AllocatorTracer.hpp         inc=OMS_RawAllocator.hpp
OMS_AllocatorTracer.cpp         inc=OMS_AllocatorTracer.hpp,OMS_Globals.hpp,OMS_DbpError.hpp
OMS_AllocatorInterfaceBase.cpp  inc=OMS_AllocatorInterface.hpp

OMS_ArrayObject.hpp             inc=OMS_Object.hpp,OMS_Handle.hpp,OMS_ArrayObjectUtils.hpp \
                                ->$INSTROOT/incl/Oms/OMS_ArrayObject.hpp
OMS_ArrayObject.cpp             inc=OMS_ArrayObject.hpp,OMS_ClassEntry.hpp,OMS_ContainerEntry.hpp,\
                                OMS_GUID.hpp,hsp77.h,ggg01,OMS_Globals.hpp,OMS_Context.hpp,\
                                OMS_Session.hpp

OMS_ArrayObjectHandle.hpp       inc=OMS_ArrayObjectCompression.hpp \
                                ->$INSTROOT/incl/Oms/OMS_ArrayObjectHandle.hpp
                                
OMS_ArrayObjectPlain.hpp        inc=OMS_Object.hpp,OMS_Handle.hpp,OMS_ArrayObject.hpp,OMS_ArrayObjectUtils.hpp,OMS_AllocatorInterface.hpp \
                                ->$INSTROOT/incl/Oms/OMS_ArrayObjectPlain.hpp 
OMS_ArrayObjectPlain.cpp        inc=OMS_ArrayObjectPlain.hpp,OMS_Trace.hpp

OMS_ArrayObjectProxy.hpp        inc=OMS_ArrayObjectPlain.hpp \
                                ->$INSTROOT/incl/Oms/OMS_ArrayObjectProxy.hpp

OMS_ArrayObjectCompression.hpp  inc=OMS_ArrayObjectProxy.hpp \
                                ->$INSTROOT/incl/Oms/OMS_ArrayObjectCompression.hpp
                                
OMS_ArrayObjectCompression.cpp  inc=OMS_ArrayObjectCompression.hpp

OMS_ArrayObjectUtils.hpp        inc=OMS_Handle.hpp \
                                ->$INSTROOT/incl/Oms/OMS_ArrayObjectUtils.hpp
                                
OMS_ArrayObjectUtils.cpp        inc=OMS_ArrayObjectUtils.hpp,SAPDB_MemCopyMove.hpp

OMS_BeforeImageList.hpp         inc=
OMS_BeforeImageList.cpp         inc=OMS_BeforeImageList.hpp,OMS_Session.hpp,\
                                OMS_DumpInterface.hpp,OMS_ObjectContainer.hpp,\
                                SAPDB_MemCopyMove.hpp,OMS_ContainerHandle.hpp

OMS_CacheMissEntry.hpp          inc=OMS_Defines.h,SAPDB_MemCopyMove.hpp
OMS_CacheMissEntry.cpp          inc=OMS_Context.hpp,OMS_CacheMissEntry.hpp,\
                                SAPDB_MemCopyMove.hpp,SAPDB_MemCopyMove.hpp,\
                                OMS_Session.hpp

OMS_CallbackInterface.hpp       inc= ->$INSTROOT/incl/Oms/OMS_CallbackInterface.hpp

OMS_CheckedUserAllocator.hpp    inc=SAPDBMem_IAllocatorInfo.hpp,OMS_AllocatorInterface.hpp,\
                                RTEMem_AllocatorRegister.hpp,ggg01
OMS_CheckedUserAllocator.cpp    inc=OMS_CheckedUserAllocator.hpp,OMS_UserAllocator.hpp,OMS_CallbackInterface.hpp,\
                                OMS_Session.hpp,OMS_Globals.hpp

OMS_ClassEntry.hpp              inc=OMS_Defines.h,OMS_KeyDesc.hpp,OMS_ObjectContainerDef.hpp
OMS_ClassEntry.cpp              inc=OMS_ClassEntry.hpp,SAPDB_MemCopyMove.hpp,OMS_Session.hpp,\
                                OMS_ArrayObject.hpp

OMS_ClassDirectory.hpp          inc=OMS_IGuidHash.hpp,OMS_ClassEntry.hpp,OMS_KeyDesc.hpp,\
                                OMS_Globals.hpp,OMS_DbpError.hpp,hsp77.h
OMS_ClassDirectory.cpp          inc=OMS_ClassDirectory.hpp,OMS_ClassEntry.hpp,\
                                OMS_Session.hpp,OMS_VarObjInfo.hpp,OMS_Trace.hpp,\
                                OMS_ArrayObject.hpp

OMS_ClientInterface.hpp         inc=OMS_IterDirection.hpp,OMS_CriticalSection.hpp,\
                                OMS_SimDumpCollector.hpp,OMS_Oid.hpp,OMS_VarOid.hpp,\
                                OMS_TypedOid.hpp,OMS_Exceptions.hpp,OMS_ObjByClsIter.hpp,\
                                OMS_KeyRangeIter.hpp,OMS_CallbackInterface.hpp,\
                                OMS_AllocatorInterface.hpp,OMS_Handle.hpp,\
                                OMS_AbstractObject.hpp,OMS_LockObject.hpp,\
                                OMS_Sequence.hpp,OMS_Stream.hpp,\
                                OMS_DbpCurrency.hpp,OMS_DbpLongCurrency.hpp,OMS_Defines.h,OMS_Object.hpp,\
                                OMS_ArrayObject.hpp,OMS_LockScopes.hpp,OMS_GlobalAnchorObject.hpp,OMS_DateTime.hpp \
                                dep=OMS_ClientInterfaceWrapper.hpp,OMS_DbpGlobals.hpp,OMS_IliveCacheSpec.hpp,\
                                OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_ClientInterface.hpp
OMS_ClientInterfaceWrapper.hpp  ->$INSTROOT/incl/livecache.h

OMS_ClientInterface.hpp setbuildinfo dep=:config/Buildinfo/VERSIONS

OMS_COMMonitor.hpp              inc=OMS_AFX.h,OMS_Defines.h,ggg00,ggg01,SAPDB_MemCopyMove.hpp
OMS_COMMonitor.cpp              inc=OMS_COMMonitor.hpp,OMS_Defines.h,OMS_Handle.hpp,\
                                RTESync_Spinlock.hpp,LVC_IliveCacheSink.hpp,\
                                OMS_Session.hpp,OMS_ObjectContainer.hpp,\
                                SQL_Handle.hpp,hsp77.h,geo573.h
OMS_COMMonitor.cpp setbuildinfo dep=:config/Buildinfo/VERSIONS

OMS_ContainerDirectoryDef.hpp   inc=OMS_ContainerHandle.hpp,OMS_IGuidHash.hpp
OMS_ContainerDirectory.hpp      inc=OMS_ContainerDirectoryDef.hpp,OMS_ContainerEntry.hpp,\
                                OMS_ContextDef.hpp,OMS_ArrayObject.hpp
OMS_ContainerDirectory.cpp      inc=OMS_ContainerDirectory.hpp,OMS_Defines.h,\
                                hsp77.h,geo573.h,OMS_FreeListHeader.hpp,\
                                OMS_ObjectContainer.hpp,OMS_ArrayObject.hpp,\
                                OMS_Context.hpp,OMS_Session.hpp,OMS_Trace.hpp

OMS_ContainerEntryDef.hpp       inc=OMS_CacheMissEntry.hpp,\
                                OMS_ContainerHandle.hpp,OMS_DynamicStack.hpp,OMS_List.hpp,\
                                OMS_Oid.hpp,OMS_Defines.h,OMS_GUID.hpp,OMS_ClassEntry.hpp,\
                                ggg250.h,ggg01
#                               ,ggg00.h
#                               dep=OMS_LiveCacheTypes.idl
OMS_ContainerEntry.hpp          inc=OMS_ContainerEntryDef.hpp,OMS_ContextDef.hpp,\
                                OMS_SessionDef.hpp,OMS_Trace.hpp,OMS_FreeListHeader.hpp,\
                                OMS_VarObjInfo.hpp,OMS_DbpBase.hpp
OMS_ContainerEntry.cpp          inc=OMS_ContainerEntry.hpp,OMS_Globals.hpp,SAPDB_MemCopyMove.hpp,\
                                OMS_ObjectContainer.hpp,OMS_ClassEntry.hpp,\
                                OMS_FreeListHeader.hpp,OMS_Context.hpp,OMS_Session.hpp

OMS_ContainerHandle.hpp         inc=gsp00,SAPDB_MemCopyMove.hpp
OMS_ContainerHandle.cpp         inc=OMS_ContainerHandle.hpp

OMS_ContextDef.hpp              inc=ggg01,OMS_RawAllocator.hpp,OMS_OidHash.hpp,\
                                OMS_ContainerDirectoryDef.hpp,OMS_VarObj.hpp,OMS_ContainerHandle.hpp,\
                                LVC_IliveCacheSink.hpp,OMS_DynamicStack.hpp,OMS_AllocatorTracer.hpp,\
                                Container_Hash.hpp,OMS_StackAllocator.hpp
OMS_Context.hpp                 inc=OMS_ContainerDirectory.hpp,OMS_ContextDef.hpp,\
                                OMS_SessionDef.hpp,OMS_ClassEntry.hpp,\
                                OMS_VarObjInfo.hpp,OMS_UnknownContainerId.hpp,\
                                OMS_HResult.hpp,OMS_Exceptions.hpp,\
                                OMS_ContainerEntry.hpp,\
                                OMS_VarOid.hpp,SAPDB_MemCopyMove.hpp,OMS_ClassDirectory.hpp
OMS_Context.cpp                 inc=OMS_Context.hpp,hsp77.h,OMS_DumpInterface.hpp,\
                                OMS_Globals.hpp,OMS_VersionDictionary.hpp,\
                                OMS_ObjectContainer.hpp,OMS_Session.hpp,geo573.h

OMS_CriticalSection.hpp         inc=OMS_AFX.h \
                                ->$INSTROOT/incl/Oms/OMS_CriticalSection.hpp
OMS_CriticalSection.cpp         inc=OMS_CriticalSection.hpp,OMS_Session.hpp,\
                                OMS_Handle.hpp,OMS_ObjectContainer.hpp

OMS_DateTime.hpp                inc=OMS_AFX.h ->$INSTROOT/incl/Oms/OMS_DateTime.hpp
OMS_DateTime.cpp                inc=OMS_DateTime.hpp,OMS_Defines.h,OMS_DbpError.hpp,\
                                ggg00,ggg01,OMS_Globals.hpp,RTE_IInterface.hpp

OMS_DbpBase.hpp                 inc=OMS_DbpError.hpp,OMS_Defines.h dep=OMS_DbpBaseWrapper.hpp \
                                ->$INSTROOT/incl/Oms/OMS_DbpBase.hpp
OMS_DbpBase.cpp                 inc=OMS_DbpBase.hpp,LVC_IliveCacheSink.hpp,RTE_IInterface.hpp,hsp77.h,\
                                OMS_Globals.hpp,SAPDB_MemCopyMove.hpp,SAPDB_UTF8Basis.hpp
OMS_DbpBaseWrapper.hpp          inc= ->$INSTROOT/incl/livecachebase.h

OMS_DbpCurrency.hpp             inc=OMS_Packed.hpp dep=OMS_DbpCurrencyWrapper.hpp \
                                ->$INSTROOT/incl/Oms/OMS_DbpCurrency.hpp
OMS_DbpCurrency.cpp             inc=OMS_DbpCurrency.hpp
OMS_DbpCurrencyWrapper.hpp      inc= ->$INSTROOT/incl/livecachecurrency.h

OMS_DbpError.hpp                inc=OMS_AFX.h,OMS_Oid.hpp dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_DbpError.hpp
OMS_DbpError.cpp                inc=OMS_DbpError.hpp,OMS_Oid.hpp,hsp81.h,hsp77.h,geo573.h,OMS_Globals.hpp \
                                dep=OMS_DbpErrorWrapper.hpp
OMS_DbpError-k.cpp              -DKERNEL inc=OMS_DbpError.hpp,OMS_Oid.hpp,hsp81.h,hsp77.h,geo573.h,OMS_Globals.hpp \
                                dep=OMS_DbpErrorWrapper.hpp
OMS_DbpErrorWrapper.hpp         inc= ->$INSTROOT/incl/livecachedbperror.h

OMS_DbpGlobals.hpp              dep=OMS_LiveCacheTypes.xml,\
                                SQL_ClientInterface.hpp,\
                                SQL_SQLDBCConnection.hpp,\
                                OMS_DbpGlobalsWrapper.hpp,\
                                OMS_ClientInterfaceWrapper.hpp \
                                ->$INSTROOT/incl/Oms/OMS_DbpGlobals.hpp
OMS_DbpGlobalsWrapper.hpp       inc= ->$INSTROOT/incl/livecachedbproc.h

OMS_DbpLongCurrency.hpp         inc=OMS_LongPacked.hpp dep=OMS_DbpLongCurrencyWrapper.hpp \
                                ->$INSTROOT/incl/Oms/OMS_DbpLongCurrency.hpp
OMS_DbpLongCurrency.cpp         inc=OMS_DbpLongCurrency.hpp
OMS_DbpLongCurrencyWrapper.hpp  inc= ->$INSTROOT/incl/livecachelongcurrency.h

OMS_Defines.h                   inc=OMS_AFX.h,OMS_GUID.hpp dep=OMS_DefinesWrapper.hpp \
                                concom=yes ->$INSTROOT/incl/Oms/OMS_Defines.h
OMS_Defines.h setbuildinfo dep=:config/Buildinfo/VERSIONS

OMS_DefinesWrapper.hpp          inc= ->$INSTROOT/incl/lcbasetypes.h

OMS_DummyKernelCallback.hpp     inc=ggg01,LVC_IliveCacheSink.hpp

OMS_DynamicStack.hpp            ->$INSTROOT/incl/Oms/OMS_DynamicStack.hpp

OMS_Exceptions.hpp              inc=OMS_DbpError.hpp ->$INSTROOT/incl/Oms/OMS_Exceptions.hpp
OMS_Exceptions.cpp              inc=OMS_Exceptions.hpp,ggg00,ggg01,OMS_Defines.h

OMS_FreeListHeader.hpp          inc=OMS_Defines.h,OMS_ObjectContainerDef.hpp
OMS_FreeListHeader.cpp          inc=OMS_FreeListHeader.hpp,OMS_Context.hpp,\
                                OMS_ObjectContainer.hpp,OMS_Session.hpp

OMS_Globals.hpp                 inc=OMS_GlobalAllocatorWrapper.hpp,LVC_KernelInterface.hpp,\
                                OMS_Oid.hpp,OMS_LibOmsInterfaceInstance.hpp,ggg250.h,\
                                OMS_VersionDictionary.hpp,OMS_ContainerHandle.hpp,\
                                SAPDBMem_NewDestroy.hpp,OMS_DynamicStack.hpp,\
                                OMS_GlobalUserAllocatorDirectory.hpp,OMS_GlobalAnchorDirectory.hpp
OMS_Globals.cpp                 inc=OMS_Globals.hpp,OMS_DummyKernelCallback.hpp,\
                                OMS_DummyKernelCallback.hpp,OMS_KernelDefaultInterface.hpp,\
                                OMS_DumpInterface.hpp,\
                                OMS_VersionDictionary.hpp,OMS_Session.hpp,\
                                OMS_MonitorDirectory.hpp,RTESync_Spinlock.hpp,\
                                OMS_ObjectContainer.hpp,OMS_AFX.h,OMS_Defines.h,\
                                SAPDB_MemCopyMove.hpp,SAPDBMem_RawAllocator.hpp
OMS_Globals-t.cpp               -DOMSTST inc=OMS_Globals.hpp,OMS_DummyKernelCallback.hpp,\
                                OMS_DummyKernelCallback.hpp,OMS_KernelDefaultInterface.hpp,\
                                OMS_DumpInterface.hpp,\
                                OMS_VersionDictionary.hpp,OMS_Session.hpp,\
                                OMS_MonitorDirectory.hpp,RTESync_Spinlock.hpp,\
                                OMS_ObjectContainer.hpp,OMS_AFX.h,OMS_Defines.h,\
                                SAPDB_MemCopyMove.hpp,SAPDBMem_RawAllocator.hpp

OMS_Globals.cpp   setbuildinfo dep=:config/Buildinfo/VERSIONS
OMS_Globals-t.cpp setbuildinfo dep=:config/Buildinfo/VERSIONS

OMS_GUID.hpp                    inc= ->$INSTROOT/incl/Oms/OMS_GUID.hpp

OMS_GlobalAnchorObject.hpp      inc=OMS_AFX.h,OMS_Defines.h,OMS_AllocatorInterface.hpp \
                                ->$INSTROOT/incl/Oms/OMS_GlobalAnchorObject.hpp

OMS_GlobalAnchorObject.cpp      inc=OMS_GlobalAnchorObject.hpp,OMS_RWLock.hpp,OMS_Handle.hpp,\
                                OMS_GlobalUserAllocator.hpp,OMS_Session.hpp,OMS_GlobalAnchorItem.hpp,\
                                LVC_IliveCacheSink.hpp
                              
OMS_GlobalAnchorItem.hpp        inc=SAPDBMem_IRawAllocator.hpp,OMS_Defines.h
OMS_GlobalAnchorItem.cpp        inc=OMS_GlobalAnchorItem.hpp
OMS_GlobalAnchorDirectory.hpp	  inc=SAPDBMem_IRawAllocator.hpp,OMS_Defines.h
OMS_GlobalAnchorDirectory.cpp	  inc=OMS_GlobalAnchorDirectory.hpp,\
                                OMS_GlobalAnchorItem.hpp,hsp77.h,\
                                OMS_Globals.hpp,OMS_RWLock.hpp,\
                                OMS_GlobalUserAllocator.hpp,OMS_Handle.hpp            
                                
OMS_GlobalUserAllocator.hpp     inc=OMS_AllocatorInterface.hpp,OMS_RawAllocator.hpp,\
                                OMS_CallbackInterface.hpp,RTESync_Spinlock.hpp,OMS_AllocatorTracer.hpp
OMS_GlobalUserAllocator.cpp     inc=OMS_GlobalUserAllocator.hpp,OMS_Globals.hpp,LVC_IliveCacheSink.hpp,\
                                SAPDB_UTF8Basis.hpp,hsp77.h,hak34.h,hgg01_3.h,hgg01_1.h,gsp03_1
OMS_GlobalUserAllocatorDirectory.hpp	    inc=OMS_Defines.h,SAPDB_MemCopyMove.hpp
OMS_GlobalUserAllocatorDirectory.cpp	    inc=OMS_GlobalUserAllocatorDirectory.hpp,OMS_Handle.hpp,\
                                          OMS_GlobalUserAllocator.hpp,OMS_RWLock.hpp,\
                                          OMS_Globals.hpp,OMS_AllocatorInterface.hpp,LVC_LockRequest.hpp   
                                          
OMS_GlobalAllocatorWrapper.hpp  inc=gsp00,ggg01,SAPDBMem_IRawAllocator.hpp
OMS_GlobalAllocatorWrapper.cpp  inc=OMS_GlobalAllocatorWrapper.hpp,OMS_Globals.hpp,OMS_Malloc.hpp,geo573.h         

OMS_Handle.hpp                  inc=OMS_ObjByKeyIterBase.hpp,OMS_ObjByClsIterBase.hpp,\
                                OMS_DbpBase.hpp,OMS_MassDerefIterators.hpp,\
                                OMS_StackTraceCallbackInterface.hpp \
                                ->$INSTROOT/incl/Oms/OMS_Handle.hpp
OMS_Handle.cpp                  inc=OMS_Handle.hpp,OMS_Globals.hpp,OMS_Session.hpp,\
                                OMS_CallbackInterface.hpp,OMS_VersionDictionary.hpp,\
                                LVC_IliveCacheSink.hpp,\
                                OMS_OidArrayReader.hpp,OMS_OidArrayWriter.hpp,\
                                OMS_MonitorDirectory.hpp,OMS_ObjectContainer.hpp,\
                                OMS_VarOid.hpp,OMS_COMMonitor.hpp,SAPDB_UTF8Basis.hpp,\
                                ggg200.h,hsp77.h,hsp78_0.h,geo573.h,OMS_Defines.h,\
                                OMS_VarObjByClsIter.hpp,SAPDB_MemCopyMove.hpp,\
                                OMS_ArrayObject.hpp,RTESys_MemoryBarrier.h
OMS_Handle-t.cpp                -DOMSTST inc=OMS_Handle.hpp,OMS_Globals.hpp,OMS_Session.hpp,\
                                OMS_CallbackInterface.hpp,OMS_VersionDictionary.hpp,\
                                LVC_IliveCacheSink.hpp,\
                                OMS_OidArrayReader.hpp,OMS_OidArrayWriter.hpp,\
                                OMS_MonitorDirectory.hpp,OMS_ObjectContainer.hpp,\
                                OMS_VarOid.hpp,OMS_COMMonitor.hpp,SAPDB_UTF8Basis.hpp,\
                                ggg200.h,hsp77.h,hsp78_0.h,geo573.h,OMS_Defines.h,\
                                OMS_VarObjByClsIter.hpp,SAPDB_MemCopyMove.hpp,\
                                OMS_ArrayObject.hpp,RTESys_MemoryBarrier.h

OMS_HResult.hpp                 inc=OMS_Defines.h,OMS_DbpError.hpp

OMS_IGuidHash.hpp               inc=OMS_Defines.h,gsp00 dep=OMS_LiveCacheTypes.xml
OMS_IGuidHash.cpp               inc=OMS_IGuidHash.hpp

OMS_InMemVersionKeyIter.hpp     inc=OMS_KeyIterBase.hpp dep=OMS_LiveCacheTypes.xml
OMS_InMemVersionKeyIter.cpp     inc=OMS_InMemVersionKeyIter.hpp,SAPDB_MemCopyMove.hpp

OMS_IterDirection.hpp           inc= ->$INSTROOT/incl/Oms/OMS_IterDirection.hpp

OMS_KernelClsIter.hpp           inc=OMS_Oid.hpp,OMS_ObjectContainer.hpp,\
                                OMS_Session.hpp,\
                                OMS_Defines.h,OMS_ContainerEntryDef.hpp
OMS_KernelClsIter.cpp           inc=OMS_KernelClsIter.hpp,Oms/OMS_HResult.hpp

OMS_KernelDefaultInterface.hpp  inc=LVC_KernelInterface.hpp
OMS_KernelDefaultInterface.cpp  inc=ggg00,OMS_KernelDefaultInterface.hpp,\
                                LVCMem_DefaultAllocator.hpp,\
                                SAPDBMem_NewDestroy.hpp,\
								                RTE_IInterface.hpp

OMS_KeyIterBase.hpp             inc=OMS_IterDirection.hpp,OMS_ObjectContainer.hpp,\
                                OMS_Session.hpp,OMS_ContainerEntryDef.hpp

OMS_KernelKeyIter.hpp           inc=ggg01,OMS_KeyIterBase.hpp,OMS_Defines.h \
                                dep=OMS_LiveCacheTypes.xml
OMS_KernelKeyIter.cpp           inc=OMS_KernelKeyIter.hpp

OMS_KernelVersionMergeKeyIter.hpp  inc=OMS_KeyIterBase.hpp,OMS_VersionMergeKeyIter.hpp,\
                                   OMS_KernelKeyIter.hpp  dep=OMS_LiveCacheTypes.xml
OMS_KernelVersionMergeKeyIter.cpp  inc=OMS_KernelVersionMergeKeyIter.hpp

OMS_KeyRangeIter.hpp            inc=OMS_ObjByKeyIterBase.hpp ->$INSTROOT/incl/Oms/OMS_KeyRangeIter.hpp

OMS_LibOmsInterface.hpp         inc=gsp00,ggg01,ggg92.h,OMS_Defines.h,OMS_ContainerHandle.hpp \
								dep=OMS_ClientInterface.hpp
OMS_LibOmsInterfaceInstance.hpp inc=ggg01,OMS_LibOmsInterface.hpp \
                                dep=OMS_ClientInterface.hpp
OMS_LibOmsInterfaceInstance.cpp inc=OMS_LibOmsInterfaceInstance.hpp,OMS_Globals.hpp,\
                                OMS_ObjectContainer.hpp,OMS_SinkCriticalSection.hpp,\
                                OMS_ObjectContainer.hpp,OMS_VersionDictionary.hpp,\
                                OMS_MonitorDirectory.hpp,\
                                OMS_LockEntryHash.hpp,OMS_Session.hpp,hsp77.h,hsp78_0.h,\
                                LVC_LockRequest.hpp,SAPDB_MemCopyMove.hpp,RTE_IInterface.hpp

OMS_LockEntryHash.hpp           inc=RTESync_Spinlock.hpp,ggg00,ggg01

OMS_LockObject.hpp              inc=OMS_AFX.h dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_LockObject.hpp
OMS_LockObject.cpp              inc=OMS_LockObject.hpp,OMS_Globals.hpp,OMS_Session.hpp,\
                                OMS_ISessionLockObjects.hpp,OMS_Handle.hpp,\
                                OMS_LockEntryHash.hpp,gsp00,ggg251.h,SAPDB_MemCopyMove.hpp

OMS_LongPacked.hpp              inc=OMS_AFX.h dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_LongPacked.hpp
OMS_LongPacked.cpp              inc=OMS_LongPacked.hpp,OMS_DbpError.hpp,ggg00,\
                                ggg01,OMS_Globals.hpp

OMS_Malloc.hpp                  inc=OMS_Defines.h ->$INSTROOT/incl/Oms/OMS_Malloc.hpp
OMS_Malloc.cpp                  inc=OMS_Malloc.hpp,OMS_Globals.hpp

OMS_MassDerefIterators.hpp      inc=OMS_Oid.hpp,OMS_AFX.h,OMS_GUID.hpp \
                                ->$INSTROOT/incl/Oms/OMS_MassDerefIterators.hpp
OMS_MassDerefIterators.cpp      inc=OMS_MassDerefIterators.hpp,OMS_DbpError.hpp,OMS_Globals.hpp,\
                                SAPDB_MemCopyMove.hpp
OMS_Monitor.hpp                 inc=gsp92,ggg01
OMS_Monitor.cpp                 inc=OMS_Monitor.hpp,ggg00,ggg01

OMS_MonitorDirectory.hpp        inc=ggg01,ggg250.h,OMS_COMMonitor.hpp,\
                                SAPDB_MemCopyMove.hpp,OMS_GlobalAllocatorWrapper.hpp
OMS_MonitorDirectory.cpp        inc=OMS_MonitorDirectory.hpp,OMS_Globals.hpp,geo573.h,OMS_DbpError.hpp

OMS_ObjByClsIter.hpp            inc=OMS_ObjByClsIterBase.hpp,OMS_TypedOid.hpp \
                                ->$INSTROOT/incl/Oms/OMS_ObjByClsIter.hpp

OMS_ObjByClsIterBase.hpp        inc=OMS_Oid.hpp,OMS_AFX.h dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_ObjByClsIterBase.hpp
OMS_ObjByClsIterBase.cpp        inc=OMS_ObjByClsIterBase.hpp,OMS_ObjectContainer.hpp,\
                                OMS_Session.hpp,OMS_KernelClsIter.hpp,OMS_VersionClsIter.hpp
OMS_ObjByClsIterBase-t.cpp      -DOMSTST inc=OMS_ObjByClsIterBase.hpp,OMS_ObjectContainer.hpp,\
                                OMS_Session.hpp,OMS_KernelClsIter.hpp,OMS_VersionClsIter.hpp

OMS_ObjByKeyIterBase.hpp        inc=OMS_GUID.hpp,OMS_AFX.h,OMS_IterDirection.hpp dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_ObjByKeyIterBase.hpp
OMS_ObjByKeyIterBase.cpp        inc=OMS_ObjByKeyIterBase.hpp,OMS_ObjectContainer.hpp,\
                                OMS_Handle.hpp,OMS_Session.hpp,OMS_KernelVersionMergeKeyIter.hpp

OMS_Object.hpp                  inc=OMS_Handle.hpp,OMS_AbstractObject.hpp,OMS_TypedOid.hpp,\
                                OMS_ObjByClsIter.hpp,OMS_KeyRangeIter.hpp,OMS_DbpError.hpp \
                                ->$INSTROOT/incl/Oms/OMS_Object.hpp

OMS_ObjectContainer.hpp         inc=OMS_ObjectContainerDef.hpp,OMS_ContainerEntry.hpp,\
                                OMS_ClassEntry.hpp,OMS_VarObjInfo.hpp,SAPDB_MemCopyMove.hpp
OMS_ObjectContainerDef.hpp      inc=OMS_AbstractObject.hpp,ggg91.h,OMS_ContainerHandle.hpp

OMS_Oid.hpp                     dep=OMS_LiveCacheTypes.xml inc=OMS_AFX.h \
                                ->$INSTROOT/incl/Oms/OMS_Oid.hpp

OMS_OidArrayReader.hpp          inc=OMS_Defines.h,OMS_Oid.hpp
OMS_OidArrayReader.cpp          inc=OMS_OidArrayReader.hpp

OMS_OidArrayWriter.hpp          inc=OMS_Defines.h,OMS_Oid.hpp
OMS_OidArrayWriter.cpp          inc=OMS_OidArrayWriter.hpp

OMS_OidHash.hpp                 inc=OMS_ObjectContainerDef.hpp,ggg00
OMS_OidHash.cpp                 inc=OMS_OidHash.hpp,OMS_ContainerEntry.hpp,\
                                OMS_VarObjInfo.hpp,OMS_DumpInterface.hpp,OMS_Session.hpp,\
                                OMS_ObjectContainer.hpp,geo573.h,SAPDB_MemCopyMove.hpp,\
                                OMS_DbpError.hpp

OMS_Packed.hpp                  inc=OMS_AFX.h dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_Packed.hpp
OMS_Packed.cpp                  inc=OMS_Packed.hpp,OMS_DbpError.hpp,ggg00,\
                                ggg01,OMS_Globals.hpp

OMS_RawAllocator.hpp            inc=SAPDBMem_RawAllocator.hpp
OMS_RWLock.hpp                  dep=OMS_LiveCacheTypes.xml inc=OMS_AFX.h,ggg00,ggg251.h
OMS_RWLock.cpp                  inc=OMS_RWLock.hpp,OMS_Globals.hpp,OMS_DbpError.hpp,OMS_Session.hpp,OMS_Handle.hpp,\
                                 LVC_KernelInterface.hpp,LVC_LockRequest.hpp,OMS_Defines.h
                                 
OMS_LockScopes.hpp              dep=OMS_LiveCacheTypes.xml inc=OMS_AFX.h ->$INSTROOT/incl/Oms/OMS_LockScopes.hpp
OMS_LockScopes.cpp              inc=OMS_LockScopes.hpp,OMS_Handle.hpp,OMS_RWLock.hpp,OMS_DbpError.hpp,\
                                OMS_Defines.h,OMS_Session.hpp,OMS_Globals.hpp

OMS_Sequence.hpp                inc=OMS_Defines.h dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_Sequence.hpp
OMS_Sequence.cpp                inc=OMS_Sequence.hpp,OMS_Globals.hpp,\
                                OMS_Handle.hpp,OMS_Session.hpp,ggg00

OMS_Session.hpp                 inc=OMS_SessionDef.hpp,OMS_Context.hpp,\
                                OMS_ISessionLockObjects.hpp,OMS_ObjectContainer.hpp,\
                                SAPDB_MemCopyMove.hpp,OMS_RWLock.hpp,\
                                OMS_FreeListHeader.hpp,OMS_SinkCriticalSection.hpp
OMS_SessionDef.hpp              inc=ggg01,geo00.h,OMS_Monitor.hpp,\
                                OMS_BeforeImageList.hpp,OMS_VarObj.hpp,ggg251.h,\
                                OMS_MassDerefIterators.hpp,OMS_ContainerHandle.hpp,\
                                OMS_StreamRegistry.hpp,OMS_ClassDirectory.hpp,\
                                OMS_KeyDesc.hpp,OMS_RWLock.hpp,OMS_DynamicStack.hpp,\
                                OMS_UserAllocator.hpp,OMS_CheckedUserAllocator.hpp
OMS_Session.cpp                 inc=OMS_Session.hpp,OMS_DumpInterface.hpp,\
                                OMS_CallbackInterface.hpp,OMS_Handle.hpp,\
                                OMS_ObjectContainer.hpp,OMS_Defines.h,\
                                hsp77.h,hsp78_0.h,geo573.h,OMS_DynamicStack.hpp,OMS_Globals.hpp

OMS_SimDumpCollector.hpp        inc=OMS_AFX.h ->$INSTROOT/incl/Oms/OMS_SimDumpCollector.hpp
OMS_SimDumpCollector.cpp        inc=OMS_SimDumpCollector.hpp

OMS_SinkCriticalSection.hpp     inc=OMS_DbpBase.hpp,LVC_IliveCacheSink.hpp,OMS_Globals.hpp \
                                dep=OMS_IliveCacheSpec.hpp

OMS_StackAllocator.hpp          inc=ggg01
OMS_StackAllocator.cpp          inc=OMS_StackAllocator.hpp,OMS_Globals.hpp,OMS_DbpError.hpp,SAPDBMem_IRawAllocator.hpp

OMS_StackTraceCallbackInterface.hpp     inc=OMS_AFX.h \
                                ->$INSTROOT/incl/Oms/OMS_StackTraceCallbackInterface.hpp

OMS_Stream.hpp                  inc=OMS_AFX.h dep=OMS_LiveCacheTypes.xml \
                                ->$INSTROOT/incl/Oms/OMS_Stream.hpp
OMS_Stream.cpp                  inc=OMS_Stream.hpp,OMS_Types.hpp,OMS_Defines.h,OMS_StreamRegistry.hpp,\
                                OMS_Handle.hpp,OMS_Globals.hpp,OMS_Session.hpp,hsp20.h,\
                                SAPDB_MemCopyMove.hpp,OMS_ObjectContainer.hpp,OMS_Trace.hpp,OMS_SessionDef.hpp -MD -D_AFXDLL -c
OMS_Stream-t.cpp                -DOMSTST inc=OMS_Stream.hpp,OMS_Types.hpp,OMS_Defines.h,OMS_StreamRegistry.hpp,\
                                OMS_Handle.hpp,OMS_Globals.hpp,OMS_Session.hpp,hsp20.h,\
                                SAPDB_MemCopyMove.hpp,OMS_ObjectContainer.hpp,OMS_Trace.hpp,OMS_SessionDef.hpp -MD -D_AFXDLL -c

OMS_Trace.hpp                   inc=gsp00,OMS_DbpError.hpp,OMS_Defines.h,OMS_ContainerHandle.hpp
OMS_Trace.cpp                   inc=OMS_Trace.hpp,OMS_Oid.hpp,hsp77.h,SAPDB_MemCopyMove.hpp

OMS_TypedOid.hpp                inc=OMS_Oid.hpp,OMS_Handle.hpp,OMS_SimDumpCollector.hpp,\
                                OMS_AbstractObject.hpp ->$INSTROOT/incl/Oms/OMS_TypedOid.hpp

OMS_Types.hpp                   inc=gsp92

OMS_UnknownContainerId.hpp      inc=ggg00,ggg01,OMS_ContainerHandle.hpp

OMS_UnloadableVersionList.hpp   inc=
OMS_UnloadableVersionList.cpp   inc=OMS_UnloadableVersionList.hpp,OMS_Session.hpp,\
                                SAPDB_MemCopyMove.hpp

OMS_UnloadedVersionKeyIter.hpp  inc=OMS_KeyIterBase.hpp dep=OMS_LiveCacheTypes.xml
OMS_UnloadedVersionKeyIter.cpp  inc=OMS_UnloadedVersionKeyIter.hpp,SAPDB_MemCopyMove.hpp

OMS_UserAllocator.hpp           inc=OMS_RawAllocator.hpp,\
                                OMS_CallbackInterface.hpp,OMS_AllocatorTracer.hpp
OMS_UserAllocator.cpp           inc=OMS_UserAllocator.hpp,OMS_Globals.hpp,LVC_IliveCacheSink.hpp,\
                                SAPDB_UTF8Basis.hpp,hsp77.h,hak34.h,hgg01_3.h,hgg01_1.h,gsp03_1

OMS_VarObjByClsIter.hpp         inc=OMS_ObjByClsIterBase.hpp,OMS_TypedOid.hpp \
                                ->$INSTROOT/incl/Oms/OMS_VarObjByClsIter.hpp

OMS_VarObj.hpp                  inc=OMS_Oid.hpp,ggg00,OMS_ContainerHandle.hpp,FileDir_FileNo.hpp
OMS_VarObjInfo.hpp              inc=OMS_VarObj.hpp,geo00.h,OMS_Trace.hpp,OMS_Globals.hpp
OMS_VarObj.cpp                  inc=OMS_VarObjInfo.hpp,OMS_Context.hpp,OMS_Session.hpp

OMS_VarOid.hpp                  inc=OMS_Oid.hpp \
                                ->$INSTROOT/incl/Oms/OMS_VarOid.hpp
OMS_VarOid.cpp                  inc=OMS_VarOid.hpp,OMS_Handle.hpp,OMS_Globals.hpp

OMS_VersionClsIter.hpp          inc=OMS_Oid.hpp,OMS_ObjectContainer.hpp,\
                                OMS_Session.hpp \
                                dep=OMS_LiveCacheTypes.xml

OMS_VersionDictionary.hpp       inc=OMS_UnloadableVersionList.hpp,OMS_ContextDef.hpp,OMS_Defines.h,\
                                OMS_LibOmsInterfaceInstance.hpp
OMS_VersionDictionary.cpp       inc=OMS_VersionDictionary.hpp,OMS_SinkCriticalSection.hpp,\
                                OMS_Session.hpp,OMS_Globals.hpp,\
                                LVC_IliveCacheSink.hpp,LVC_LockRequest.hpp,hsp77.h,\
                                SAPDB_MemCopyMove.hpp,OMS_Context.hpp,OMS_RWLock.hpp

OMS_VersionMergeKeyIter.hpp     inc=OMS_KeyIterBase.hpp,OMS_UnloadedVersionKeyIter.hpp,\
                                OMS_InMemVersionKeyIter.hpp  dep=OMS_LiveCacheTypes.xml
OMS_VersionMergeKeyIter.cpp     inc=OMS_VersionMergeKeyIter.hpp

#--------

OMS_TestHeader.cpp              inc=OMS_ClientInterface.hpp

#--------

SizeOfSink.cpp              inc=geo38.h


