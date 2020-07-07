#
#   RunTime Tasking
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

RTETask_Yield.h                             inc=SAPDB_Types.h

&if $OS in [ WIN32 ]
RTETask_TCB.cpp                             inc=RTETask_Yield.h,RTETask_TCB.hpp,RTESys_Spinlock.h,SAPDB_Types.h,geo007_1.h,geo50_0.h,heo51.h,heo56.h
RTETask_TCB.hpp                             inc=RTE_Types.h,RTETask_LegacyTaskCtrl.h dep=gos00k.h
&else
RTETask_TCB.cpp                             inc=RTETask_Yield.h,RTETask_TCB.hpp,RTESys_Spinlock.h,SAPDB_Types.h,geo007_1.h,geo50_0.h,hen50.h,heo51.h,heo56.h
RTETask_TCB.hpp                             inc=RTE_Types.h,RTETask_LegacyTaskCtrl.h dep=gen50.h,gen71.h,geo002.h
&endif

RTETask_IContext.hpp                        inc=RTE_Types.hpp
RTETask_IWorkerJob.hpp                      inc=RTE_Types.h
RTETask_WorkerPool.hpp                      inc=RTE_Types.h,RTETask_IWorkerJob.hpp
RTETask_WorkerPool.cpp                      inc=RTETask_WorkerPool.hpp,RTESys_AtomicOperation.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTESys_MemoryBarrier.h\
                                                ,RTEThread_KernelThread.hpp,RTE_Crash.hpp,RTE_MessageList.hpp,RTESys_AtomicOperation.hpp,geo50_0.h,heo56.h,heo07.h
RTETask_DoublyLinkedListElement.hpp         inc=
RTETask_DoublyLinkedList.hpp                inc=RTETask_DoublyLinkedListElement.hpp,RTE_Types.h,SAPDBErr_Assertions.hpp,RTESys_MemoryBarrier.h

RTETask_CommunicationQueue.hpp              inc=RTE_KGS.hpp,SAPDB_Types.hpp,RTETask_DoublyLinkedList.hpp,RTETask_CommonQueueElement.hpp
RTETask_CommunicationQueue.cpp              -DKERNEL_LZU \
                                            inc=RTETask_CommunicationQueue.hpp,SAPDB_Types.hpp,RTE_MessageList.hpp,RTETask_Task.hpp,RTE_KGS.hpp

RTETask_LegacyQueueElement.h                inc=RTE_Types.h,RTETask_CQE_CommunicationReceiveWaitPart.h,RTETask_CQE_ConnectRequest.h,RTETask_CQE_TaskSleep.h
RTETask_LegacyQueueElementFreeList.h        inc=
RTETask_LegacyQueueElementFreeList.cpp      inc=RTETask_LegacyQueueElementFreeList.h,RTE_KGS.hpp,RTETask_CommonQueue.hpp,geo50_0.h,RTETask_CommonQueueElement.hpp
                                      
RTETask_LegacyTaskCtrl.h                    inc=geo50_0.h
RTETask_LegacyTaskCtrl.hpp                  inc=geo50_0.h,RTE_Types.h,RTETask_CommonQueueElement.hpp,RTETask_ITaskClientConnection.hpp \
&if $OS in [ WIN32 ]
                                            dep=gos74.h,gos003.h
&else                                  
                                            dep=gen71.h,gen003.h
&endif                                   

RTETask_LoadBalancingInfo.hpp               inc=geo50_0.h,RTE_Types.h
RTETask_LoadBalancingInfo.cpp               inc=RTEMem_RteAllocator.hpp,RTE_KGS.hpp,geo007_1.h,geo002.h,RTE_MessageList.hpp,RTE_Message.hpp,RTETask_LoadBalancingInfo.hpp
                                   
RTETask_LegacyTaskCtrl.cpp                  -DKERNEL_LZU \
                                            inc=RTETask_LegacyTaskCtrl.h,RTETask_LegacyTaskCtrl.hpp,RTETask_Task.hpp,geo50_0.h,heo670.h,SDBMsg_RTETask.h,RTE_Message.hpp,RTEConf_ParameterAccess.hpp,RTEConf_ParameterAccessKernelInterface.hpp\
&if $OS in [ WIN32 ]
&else
                                            ,gen88.h
&endif

RTETask_LegacyUKTCtrl.h                     inc=geo50_0.h
RTETask_LegacyUKTCtrl.hpp                   inc=geo50_0.h,RTE_Types.h \
&if $OS in [ WIN32 ]
                                            dep=gos74.h
&else                                  
                                            dep=gen71.h,gen72.h
&endif                                   
                                   
RTETask_LegacyUKTCtrl.cpp                   -DKERNEL_LZU \
                                            inc=RTETask_LegacyUKTCtrl.h,RTETask_LegacyUKTCtrl.hpp,RTETask_TaskScheduler.hpp,geo50_0.h\
&if $OS in [ WIN32 ]
&else
                                            ,gen88.h
&endif

RTETask_ITask.hpp                           inc=geo00_2.h,RTE_Types.hpp,RTETask_Task.h
RTETask_Task.h                              inc=SAPDB_Types.h
RTETask_SuspendReason.h						inc=
RTETask_Task.hpp                            inc=SAPDB_Types.hpp,RTETask_ITask.hpp,RTETask_LegacyTaskCtrl.hpp,RTETask_TaskScheduler.hpp,SAPDB_ToString.hpp,RTETask_Task.h,RTE_KGS.hpp,RTETask_CommonQueue.hpp,RTETask_CommonQueueElement.hpp,RTETask_TaskClientConnections.hpp,heo56.h,RTETask_LocalStorage.hpp\
&if $OS in [ WIN32 ]
&else
                                            ,gen88.h
&endif
                                   
RTETask_Task.cpp                            -DKERNEL_LZU \
                                            inc=RTETask_Task.hpp,SAPDB_Types.hpp,RTETask_LegacyTaskCtrl.hpp,heo00x.h,RTE_ConsoleStandardOutput.h,RTE_MessageList.hpp,RTE_Task_Messages.hpp,RTECKC_KernelCommunication.hpp,Kernel_Main.hpp,Kernel_Initialization.hpp

RTETask_ITaskScheduler.hpp                  inc=RTE_Types.hpp,RTE_MessageList.hpp

RTETask_TaskScheduler.hpp                   inc=SAPDB_Types.hpp,RTETask_ITaskScheduler.hpp,RTETask_CommunicationQueue.hpp,RTETask_LegacyUKTCtrl.hpp,RTETask_ReadyToRunQueue.hpp,\
                                                RTETask_ExternalRequestsQueue.hpp,RTETask_SchedulerToSchedulerQueue.hpp,RTETask_UnconnectedTasksQueue.hpp,RTETask_ITask.hpp
                                   
RTETask_TaskScheduler.cpp                   -DKERNEL_LZU \
                                            inc=RTETask_TaskScheduler.hpp,SAPDB_Types.hpp,RTETask_LegacyUKTCtrl.hpp,RTETask_Task.hpp

RTETask_CommonQueueElement.hpp              -DKERNEL_LZU \
                                            inc=RTE_Types.h,geo50_0.h,SAPDB_ToString.hpp,RTETask_LegacyQueueElement.h,RTETask_DoublyLinkedListElement.hpp,RTETask_ICommonQueue.hpp
            
RTETask_CQE_TaskSleep.h                     -DKERNEL_LZU \
                                            inc=RTE_Types.h

RTETask_CQE_CommunicationReceiveWaitPart.h  -DKERNEL_LZU \
                                            inc=RTE_Types.h

RTETask_CQE_ConnectRequest.h                -DKERNEL_LZU \
                                            inc=RTE_Types.h
                                            
RTETask_ICommonQueue.hpp                    -DKERNEL_LZU \
                                            inc=RTE_Types.hpp
                                        
RTETask_CommonQueue.hpp                     -DKERNEL_LZU \
                                            inc=RTE_Types.hpp,RTETask_DoublyLinkedList.hpp,RTESync_Spinlock.hpp,RTETask_ICommonQueue.hpp
                                     
RTETask_CommonQueue.cpp                     -DKERNEL_LZU \
                                            inc=RTETask_CommonQueue.hpp,RTE_Crash.hpp,RTE_Task_Messages.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTETask_CommonQueueElement.hpp
                                
RTETask_ReadyToRunQueue.hpp                 -DKERNEL_LZU \
                                            inc=SAPDB_Types.hpp,RTETask_CommonQueueElement.hpp,SAPDB_ToString.hpp
                                
RTETask_ReadyToRunQueue.cpp                 -DKERNEL_LZU \
                                            inc=geo002.h,RTETask_ReadyToRunQueue.hpp,RTETask_Task.hpp
                                
RTETask_ExternalRequestsQueue.hpp           -DKERNEL_LZU \
                                            inc=SAPDB_Types.hpp,RTETask_CommonQueueElement.hpp
                                
RTETask_ExternalRequestsQueue.cpp           -DKERNEL_LZU \
                                             inc=RTETask_ExternalRequestsQueue.hpp,RTETask_Task.hpp
                                   
RTETask_SchedulerToSchedulerQueue.hpp       -DKERNEL_LZU \
                                             inc=SAPDB_Types.hpp

RTETask_SchedulerToSchedulerQueue.cpp       -DKERNEL_LZU \
                                             inc=RTETask_SchedulerToSchedulerQueue.hpp,RTETask_Task.hpp
RTETask_UnconnectedTasksQueue.hpp           -DKERNEL_LZU \
                                             inc=RTETask_CommonQueue.hpp
RTETask_UnconnectedTasksQueue.cpp           -DKERNEL_LZU \
                                             inc=RTETask_UnconnectedTasksQueue.hpp,RTETask_Task.hpp
                                             
                                             
RTETask_UnconnectedTaskPool.hpp             -DKERNEL_LZU \
                                             inc=RTE_KGS.hpp,SAPDB_Types.hpp,RTETask_ITask.hpp,RTETask_CommonQueueElement.hpp,RTESync_NamedSpinlock.hpp
RTETask_UnconnectedTaskPool.cpp             -DKERNEL_LZU \
                                             inc=RTETask_UnconnectedTaskPool.hpp,RTETask_TaskScheduler.hpp,RTE_PendingEventHandler.h

RTETask_ITaskClientConnection.hpp           inc=geo003.h,SAPDBErr_MessageList.hpp,SAPDB_Types.hpp
RTETask_TaskClientConnection.hpp            -DKERNEL_LZU \
                                             inc=RTE_KGS.hpp,SAPDB_Types.hpp,RTE_MessageList.hpp,RTETask_DoublyLinkedListElement.hpp,RTETask_ITaskClientConnection.hpp,RTECKC_KernelCommunicationSegment.hpp,RTESync_BinarySemaphore.hpp,RTEComm_ParseURI.hpp,RTESys_ProcessState.hpp,RTECKC_PacketHeader.hpp,RTECKC_KernelCommSequenceCheck.hpp
                                             
RTETask_TaskClientConnection.cpp            -DKERNEL_LZU \
                                             inc=RTE_CKC_Messages.hpp,RTETask_TaskClientConnection.hpp,RTECKC_Connect.hpp,RTETask_Task.hpp,RTESys_ProcessID.hpp,RTE_Message.hpp,RTESys_SecurityAttributes.hpp,RTECKC_KernelCommunication.hpp
                                             
RTETask_TaskClientConnections.hpp           -DKERNEL_LZU \
                                             inc=RTETask_CommonQueueElement.hpp,RTE_Types.hpp,RTETask_DoublyLinkedList.hpp,RTESync_Spinlock.hpp,RTE_MessageList.hpp,RTETask_CommonQueue.hpp,RTE_KGS.hpp
                                             

RTETask_TaskClientConnections.cpp           -DKERNEL_LZU \
                                             inc=RTETask_TaskClientConnections.hpp,RTE_Message.hpp,RTEMem_Allocator.hpp,RTETask_TaskClientConnection.hpp,SAPDB_sprintf.h,RTE_CKC_Messages.hpp,SAPDB_StrUtils.hpp,RTETask_Task.hpp,RTE_Comm_Messages.hpp,RTE_Crash.hpp

RTETask_LocalStorage.hpp                    -DKERNEL_LZU \
                                             inc=SAPDB_Types.hpp,RTESync_Spinlock.hpp

RTETask_LocalStorage.cpp                    -DKERNEL_LZU \
                                             inc=RTETask_LocalStorage.hpp
                                             
RTETask_LegacyConnectionInterface.hpp		inc=RTETask_ITaskClientConnection.hpp
RTETask_LegacyConnectionInterface.cpp		-DKERNEL_LZU \
											 inc=RTETask_Task.hpp
