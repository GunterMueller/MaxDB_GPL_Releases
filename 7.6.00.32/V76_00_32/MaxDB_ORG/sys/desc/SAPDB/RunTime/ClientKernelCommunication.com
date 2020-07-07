#
# RunTime Environment / Client Kernel Communication Descriptions
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
RTECKC_ClientCommSequenceCheck.hpp	inc=SAPDB_Types.hpp,RTESync_Spinlock.hpp,RTE_MessageList.hpp
RTECKC_ClientCommSequenceCheck.cpp	inc=RTECKC_ClientCommSequenceCheck.hpp,SAPDB_Types.hpp,RTESync_Spinlock.hpp,SAPDB_ToString.hpp,RTEMem_Allocator.hpp,\
										RTE_CKC_Messages.hpp
										
RTECKC_KernelCommSequenceCheck.hpp	inc=SAPDB_Types.hpp,RTE_MessageList.hpp
RTECKC_KernelCommSequenceCheck.cpp	inc=RTECKC_KernelCommSequenceCheck.hpp,SAPDB_Types.hpp,SAPDB_ToString.hpp,RTEMem_Allocator.hpp,RTE_CKC_Messages.hpp
	
RTECKC_Request.hpp					inc=RTESync_BinarySemaphore.hpp,RTECKC_ClientCommunication.hpp,RTEIPC_CommunicationObject.hpp,SAPDB_ToString.hpp,\
										RTEComm_ParseURI.hpp,RTEComm_URIBuilder.hpp,RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,\
										SAPDBTrace_Usage.hpp
RTECKC_Request.cpp					inc=RTECKC_Request.hpp,RTE_CKC_Messages.hpp		

RTECKC_Connect.hpp					inc=RTESync_BinarySemaphore.hpp,RTEComm_ParseURI.hpp,RTE_CKC_Messages.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp

RTECKC_RequestChannel.hpp			inc=RTEIPC_Mailslot.hpp,RTECKC_ClientCommunication.hpp,RTECKC_Request.hpp,RTE_MessageList.hpp
RTECKC_RequestChannel.cpp			inc=RTECKC_RequestChannel.hpp,RTE_Message.hpp,RTE_CKC_Messages.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_TagFileHandlingUNIX.hpp

RTECKC_PacketHeader.hpp				inc=RTEComm_PacketHeader.h,RTE_MessageList.hpp,RTE_CKC_Messages.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTECKC_PacketHeader.cpp				inc=RTECKC_PacketHeader.hpp,SAPDBErr_MessageEventConverter.hpp

RTECKC_ICommunication.hpp			inc=RTECKC_ClientCommunication.hpp,RTE_MessageList.hpp

RTECKC_GlobalClientInstance.hpp		inc=RTECKC_ClientCommunication.hpp,RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTECKC_GlobalClientInstance.cpp		inc=RTE_HandleManager.hpp,RTECKC_GlobalClientInstance.hpp,RTECKC_ICommunication.hpp,RTE_UniqueId.h,\
										RTEMem_Allocator.hpp,RTE_Message.hpp,RTE_CKC_Messages.hpp,SAPDBErr_MessageOutput.h \
&if $OSSPEC not in [ WIN32 ]
                                    dep=gen22.h,SAPDBErr_MessageOutput.h
&else
									dep=                                        
&endif

RTECKC_ClientCommunication.hpp      inc=RTE_HandleManager.hpp,RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTECKC_ClientCommunication.cpp	    inc=RTECKC_ClientCommunication.hpp,RTECKC_LocalCommunication.hpp,RTECKC_ICommunication.hpp,\
                                        RTECKC_GlobalClientInstance.hpp,RTEComm_ParseURI.hpp,RTEMem_Allocator.hpp,RTE_ISystem.hpp,RTE_Message.hpp,RTE_CKC_Messages.hpp								

RTECKC_LocalCommunication.hpp       inc=RTESys_ProcessState.hpp,RTECKC_RequestChannel.hpp,RTECKC_ICommunication.hpp,\
                                        RTECKC_ClientCommSequenceCheck.hpp,RTECKC_ClientCommunicationSegment.hpp,RTECKC_ClientCommunication.hpp,\
                                        RTECKC_PacketHeader.hpp,RTEComm_ParseURI.hpp,RTESync_BinarySemaphore.hpp,RTE_UniqueId.h,RTE_MessageList.hpp,\
                                        SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp \
&if $OSSPEC in [ WIN32 ]
                                    dep=gos00.h
&else
									dep=
&endif

RTECKC_LocalCommunication.cpp       inc=RTESys_SecurityAttributes.hpp,RTEIPC_NamedSharedMemory.hpp,RTECKC_LocalCommunication.hpp,RTECKC_Connect.hpp,SAPDBErr_MessageEventConverter.hpp,RTEMem_Allocator.hpp,\
                                        RTEMem_RteAllocator.hpp,RTE_TagFileHandlingUNIX.hpp,RTE_Message.hpp,RTE_CKC_Messages.hpp,SAPDBMem_Alloca.h,RTESys_ProcessID.hpp

RTECKC_CommunicationSegmentHeader.hpp   inc=RTECKC_CommunicationSegment.hpp,SAPDB_ToString.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp \
&if $OSSPEC not in [WIN32]
									dep=RTESync_BinarySemaphore.hpp	
&else
									dep=
&endif

RTECKC_CommunicationSegment.hpp     inc=RTEIPC_NamedSharedMemory.hpp,SAPDB_ToString.hpp,RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp \
&if $OSSPEC not in [WIN32]
									dep=RTESync_BinarySemaphore.hpp	
&else
									dep=
&endif
                                        
RTECKC_CommunicationSegment.cpp     inc=RTECKC_CommunicationSegmentHeader.hpp,RTECKC_CommunicationSegment.hpp,RTEMem_Allocator.hpp,SAPDBMem_NewDestroy.hpp,RTE_Message.hpp,RTE_CKC_Messages.hpp

RTECKC_ClientCommunicationSegment.hpp   inc=RTECKC_CommunicationSegment.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTECKC_ClientCommunicationSegment.cpp   inc=RTECKC_CommunicationSegmentHeader.hpp,RTECKC_ClientCommunicationSegment.hpp,RTE_CKC_Messages.hpp

RTECKC_KernelCommunicationSegment.hpp   inc=RTECKC_CommunicationSegment.hpp,RTECKC_CommunicationSegmentHeader.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTECKC_KernelCommunicationSegment.cpp   inc=RTECKC_KernelCommunicationSegment.hpp,RTE_CKC_Messages.hpp


RTECKC_GlobalCommSegHashTable.hpp  inc=RTESync_NamedSpinlock.hpp,SAPDBMem_NewDestroy.hpp,SAPDB_Types.h,RTEIPC_NamedSharedMemory.hpp
RTECKC_GlobalCommSegHashTable.cpp  inc=RTECKC_GlobalCommSegHashTable.hpp,SAPDBMath_PrimeNumber.hpp,RTE_CKC_Messages.hpp,SAPDBErr_Assertions.hpp,RTEMem_RteAllocator.hpp

RTECKC_GlobalCommunicationSegmentAllocator.hpp  inc=SAPDBMem_IRawAllocator.hpp,SAPDBMem_SynchronizedRawAllocator.hpp
RTECKC_GlobalCommunicationSegmentAllocator.cpp  inc=SAPDBMem_NewDestroy.hpp,RTEMem_AllocatorRegister.hpp,SAPDBErr_Assertions.hpp,SAPDBMem_Exceptions.hpp,RTE_ISystem.hpp,\
                                                   RTEIPC_NamedSharedMemory.hpp,RTESys_SecurityAttributes.hpp,RTE_Message.hpp,RTECKC_GlobalCommunicationSegmentAllocator.hpp,\
                                                   RTECKC_GlobalCommSegHashTable.hpp,RTE_CKC_Messages.hpp,RTESync_InterlockedCounter.hpp,RTEMem_RteAllocator.hpp,SAPDB_ToString.hpp

RTECKC_KernelCommunication.hpp      inc=RTETask_UnconnectedTaskPool.hpp,RTE_KGS.hpp,RTESync_InterlockedCounter.hpp
RTECKC_KernelCommunication.h        inc=RTECKC_KernelCommunication.hpp

#Only for tests:
RTECKC_TestClient.cpp               inc=RTEMem_Allocator.hpp,RTEIPC_Mailslot.hpp,heo02.h,RTECKC_RequestChannel.hpp,RTECKC_GlobalClientInstance.hpp,RTE_Message.hpp,heo07.h \
                                    dep=gos00.h

RTECKC_TestRequestor.cpp           inc=RTESys_SecurityAttributes.hpp,RTECKC_KernelCommunicationSegment.hpp,RTECKC_PacketHeader.hpp,RTECKC_Connect.hpp,RTECKC_RequestChannel.hpp,\
                                        RTESync_BinarySemaphore.hpp,RTEComm_ParseURI.hpp,RTEMem_Allocator.hpp,RTE_Message.hpp,RTE_CKC_Messages.hpp \
                                   dep=gos00.h     
                                   
RTECKC_TestKernelXFer.cpp          -DKERNEL_LZU \
                                   inc=RTETask_ITask.hpp,RTETask_ITaskClientConnection.hpp,RTE_Message.hpp,RTETask_Task.hpp,RTEUDE_ServerControl.hpp

RTECKC_TestRemoteClient.cpp			inc=RTECKC_TestSocket.hpp,RTECKC_TestSocketClientWorker.hpp,RTE_Types.h,RTE_ISystem.hpp,RTESync_BinarySemaphore.hpp,\
										RTESys_SecurityAttributes.hpp,RTEThread_Thread.hpp,SAPDBMem_Alloca.h

RTECKC_TestRemoteServer.cpp			inc=RTECKC_TestSocket.hpp,RTECKC_TestSocketServerWorker.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTESync_BinarySemaphore.hpp,\
										RTESys_SecurityAttributes.hpp,RTE_HandleManager.hpp,RTE_ISystem.hpp,SAPDBMem_Alloca.h

RTECKC_TestSocket.hpp				inc=RTE_Types.hpp

RTECKC_TestSocket.cpp				inc=RTECKC_TestSocket.hpp

RTECKC_TestSocketClientWorker.hpp	inc=RTECKC_TestSocket.hpp,RTEThread_Thread.hpp

RTECKC_TestSocketClientWorker.cpp	inc=RTECKC_TestSocketClientWorker.hpp

RTECKC_TestSocketServerWorker.hpp	inc=RTECKC_TestSocket.hpp,RTECKC_TestServerRequestQueue.hpp,RTEThread_Thread.hpp,RTE_HandleManager.hpp

RTECKC_TestSocketServerWorker.cpp	inc=RTECKC_TestSocketServerWorker.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp

RTECKC_TestServerRequestQueue.hpp	inc=RTETask_DoublyLinkedList.hpp,RTESync_Spinlock.hpp

