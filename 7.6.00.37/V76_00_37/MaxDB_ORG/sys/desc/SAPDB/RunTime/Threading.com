#
#   RunTime Threading
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

# No exception handling code variant
#
*-noex -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
*-CAP  -D_FASTCAP


#
RTEThread_IContext.hpp                  inc=RTE_Types.hpp
RTEThread_Context.hpp                   inc=RTEThread_IContext.hpp,SAPDBErr_MessageList.hpp
RTEThread_Context*.cpp                  inc=RTE_IGlobal.hpp,RTEThread_Context.hpp,RTETask_IContext.hpp,SAPDBErr_Assertions.hpp,RTE_MessageList.hpp

# Console Thread Main
RTEThread_Console.h                     inc=
RTEThread_Console*.cpp                  inc=RTE_CompilerFeatures.h,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_MessageList.hpp\
                                            ,RTE_Message.hpp,RTE_Console_Thread_Messages.hpp,RTE_ConsoleCommon.hpp\
                                            ,RTEThread_ConsoleWorkerThread.hpp,RTEThread_ConsoleRequestor.hpp,RTE_ConsoleRequestCommunication.hpp\
                                            ,RTEThread_Console.h,RTE_KGS.hpp,RTE_KSS.h,RTE_ConsoleStandardDataRequests.h,RTE_ConsoleTraceTopic.hpp\
                                            ,RTE_ConsoleDataCommunication.hpp\
&if ($OSSPEC == LINUX && $__HAS_NPTL != 1) || $MACH == SUN || $OSSPEC == SOLARIS
                                            ,RTE_ConsoleSemaphoreTimeoutList.hpp
&endif  


RTEThread_ConsoleCommand.hpp            inc=RTE_Types.h,RTE_ConsoleCommon.hpp,RTECont_DoublyLinkedListElement.hpp

#
# Console Command Worker
RTEThread_ConsoleWorkerThread.hpp       inc=RTE_Types.h,RTECont_DoublyLinkedList.hpp,RTESync_Spinlock.hpp,RTEThread_ConsoleCommand.hpp\
                                            ,RTEThread_KernelThread.hpp,RTEMem_RteAllocator.hpp,RTEThread_ConsoleConnections.hpp
RTEThread_ConsoleWorkerThread*.cpp      inc=RTE_CompilerFeatures.h,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_MessageList.hpp,RTE_Message.hpp\
                                            ,RTE_Console_Thread_Messages.hpp,RTE_ConsoleCommon.hpp,RTEThread_ConsoleWorkerThread.hpp

#
# Console Connections
RTEThread_ConsoleConnections.h          inc=
RTEThread_ConsoleConnections.hpp        inc=RTEMem_Allocator.hpp,RTEThread_ConsoleWorkerBase.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp\
                                            ,SAPDB_Singleton.hpp,RTE_HandleManager.hpp,RTE_MessageList.hpp,RTESync_InterlockedCounter.hpp\
                                            ,RTEThread_ConsoleCommand.hpp
RTEThread_ConsoleConnections*.cpp       inc=RTEThread_ConsoleConnections.hpp,RTEThread_ConsoleConnections.h,RTE_CompilerFeatures.h\
                                            ,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,SAPDBMem_NewDestroy.hpp,RTEThread_ConsoleStandardWorker.hpp\
                                            ,RTE_Console_Thread_Messages.hpp,RTESys_ProcessState.hpp


#
# Console Requestor
RTEThread_ConsoleRequestor.hpp          inc=RTE_ConsoleRequestCommunication.hpp,RTEThread_ConsoleCommand.hpp
RTEThread_ConsoleRequestor*.cpp         inc=RTE_KGS.hpp,RTE_CompilerFeatures.h,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Console_Thread_Messages.hpp\
                                            ,RTE_ConsoleCommon.hpp,RTEComm_PacketHeader.h,RTEThread_ConsoleRequestor.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp\
                                            ,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Console_Thread_Messages.hpp,RTEMem_Allocator.hpp

#
# Console Standard Worker
RTEThread_ConsoleStandardWorker.hpp     inc=RTEThread_ConsoleWorkerBase.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTEThread_ConsoleStandardWorker*.cpp    inc=heo670.h,heo92.h,geo007_1.h,heo54k.h,SAPDBErr_MessageEventConverter.hpp,RTE_CompilerFeatures.h,RTE_MessageList.hpp\
                                            ,RTE_Message.hpp,RTE_Console_Thread_Messages.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp\
                                            ,RTEThread_ConsoleStandardWorker.hpp,geo00_1.h,RTEMem_RteAllocator.hpp,RTE_KGS.hpp,RTE_KSS.h\
                                            ,RTEMem_AWEAllocator.hpp,RTEMem_SystemPageCache.hpp,RTESys_MicroTime.h,RTESync_RWRegion.hpp\
                                            ,RTETask_CommunicationQueue.hpp,RTETask_Task.hpp,SAPDB_MemCopyMoveNoCheck.hpp\
                                            ,RTESync_SpinlockPoolRegister.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTEDiag_SymbolResolutionLock.h,SAPDB_Names.h\
&if $OSSPEC = WIN32
                                            ,gos003.h,gos00k.h
&else
                                            ,gen81.h,gen71.h,gen72.h,gen73.h
&endif

#
# Console Base Worker
RTEThread_ConsoleWorkerBase.hpp         inc=geo003.h,heo07.h,RTE_ConsoleCommon.hpp,RTE_ConsoleStandardDataRequests.h,RTE_ConsoleDataCommunication.hpp\
                                            ,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTESys_Time.h
RTEThread_ConsoleWorkerBase*.cpp        inc=RTE_KGS.hpp,RTE_CompilerFeatures.h,RTE_MessageList.hpp,RTEThread_ConsoleWorkerBase.hpp,RTE_Message.hpp\
                                            ,RTE_Console_Thread_Messages.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_KSS.h


#
# Thread Class
RTEThread_Thread.hpp                    -DKERNEL_LZU \
                                        inc=geo07.h,SAPDB_Types.hpp,SAPDB_ToString.hpp,RTE_MessageList.hpp,RTESync_Spinlock.hpp
RTEThread_Thread.cpp                    -DKERNEL_LZU \
                                        inc=heo07.h,RTE_Message.hpp,SAPDB_Types.hpp,RTE_Thread_Messages.hpp,RTEThread_Thread.hpp
RTEThread_KernelThread.hpp             -DKERNEL_LZU \
                                        inc=RTEThread_Thread.hpp

RTEThread_KernelThread.cpp             -DKERNEL_LZU \
                                        inc=RTEThread_KernelThread.hpp,geo50_0.h

#
# Client Kernel Communication
RTEThread_Requestor.hpp                 -DKERNEL_LZU \
                                        inc=RTE_KGS.hpp,SAPDB_Types.hpp,RTE_Thread_Messages.hpp,RTEThread_KernelThread.hpp,RTECKC_RequestChannel.hpp,SAPDB_Singleton.hpp
RTEThread_Requestor.cpp                 -DKERNEL_LZU \
                                        inc=RTE_KGS.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_CKC_Messages.hpp,RTEThread_Requestor.hpp,SAPDB_StrUtils.hpp,RTETask_TaskClientConnections.hpp,\
                                            RTE_Crash.hpp,RTETask_Task.hpp,RTETask_TaskClientConnection.hpp,RTECKC_KernelCommunication.hpp,RTE_ISystem.hpp,RTESys_SecurityAttributes.hpp

RTEThread_WorkerQueue.hpp				inc=RTESync_NamedSpinlock.hpp
RTEThread_WorkerThread.hpp				inc=RTEThread_KernelThread.hpp,RTEThread_IJob.hpp,RTEThread_IWorkerPool.hpp,SAPDBErr_Assertions.hpp
RTEThread_WorkerPool.hpp				inc=RTESync_NamedSpinlock.hpp,RTEThread_WorkerThread.hpp,RTEThread_IWorkerPool.hpp,SAPDBErr_Assertions.hpp,SAPDB_string.h,RTE_Message.hpp,RTE_Thread_Messages.hpp,RTEMem_RteAllocator.hpp
RTEThread_SingleWorkerPool.hpp		    inc=RTESync_NamedSpinlock.hpp,RTEThread_WorkerThread.hpp,RTEThread_IWorkerPool.hpp,SAPDBErr_Assertions.hpp,SAPDB_string.h,RTE_Message.hpp,RTE_Thread_Messages.hpp,RTEMem_RteAllocator.hpp
RTEThread_IdleWorkerPool.hpp		    inc=RTEThread_WorkerPool.hpp
RTEThread_IdleWorkerPool.cpp		    inc=RTEThread_IdleWorkerPool.hpp,SAPDBMem_NewDestroy.hpp,RTE_Message.hpp
RTEThread_IWorkerPool.hpp				inc=SAPDB_Types.hpp
RTEThread_IJob.hpp                      inc=SAPDB_Types.hpp

RTEThread_IORedirection.hpp             inc=RTEThread_KernelThread.hpp,Msg_IOutput.hpp,heo60_2.h,gsp03
