# ------------------------------------------------------------------------------
#
# LiveCache ProcServer Descriptions
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

regex(.*\.cpp$)    -except

LVCPS_Dispatcher.hpp          inc=LVC_Dispatcher.hpp
LVCPS_Dispatcher.cpp          inc=LVCPS_Dispatcher.hpp,LVC_ProcServerInterface.hpp,\
                              SAPDBMem_DefaultRawAllocator.hpp,OMS_Defines.h

LVCPS_KernelInterface.hpp     inc=LVC_KernelInterface.hpp
LVCPS_KernelInterface.cpp     inc=LVCPS_KernelInterface.hpp,LVC_ProcServerInterface.hpp,\
                              LVCMem_DefaultAllocator.hpp

LVCPS_LiveCacheSink.hpp       inc=LVC_IliveCacheSink.hpp
LVCPS_LiveCacheSink.cpp       inc=LVCPS_LiveCacheSink.hpp,LVC_ProcServerInterface.hpp,\
                              LVCPS_PacketConsoleMsg.hpp,LVC_ProcServerExecutor.hpp,\
                              LVCPS_PacketSingleObj.hpp,LVCPS_PacketContainer.hpp,\
                              LVCPS_PacketVersion.hpp,LVCPS_PacketSchema.hpp,\
                              LVCPS_PacketRegion.hpp,LVCPS_PacketSubtrans.hpp,\
                              LVCPS_PacketConsistentView.hpp,LVCPS_PacketMassObj.hpp,\
                              LVCPS_PacketIterator.hpp,LVCPS_PacketSequence.hpp,\
                              LVCPS_PacketFile.hpp,LVCPS_PacketABAPStream.hpp,\
                              LVCPS_PacketTasking.hpp,LVCPS_PacketSQL.hpp,\
                              LVCPS_PacketVersionDict.hpp,\
                              OMS_DbpError.hpp,\
                              OMS_CallbackInterface.hpp,SAPDBMem_RawAllocator.hpp,\
                              SAPDBMem_NewDestroy.hpp,OMS_Defines.h\

LVCPS_ParamHandler.hpp        inc=LVC_LoadComponentLibrary.hpp,LVC_DispatcherParams.hpp,vak001
LVCPS_ParamHandler.cpp        inc=LVCPS_ParamHandler.hpp,geo00d.h

LVCPS_PacketConsoleMsg.hpp    inc=LVC_ProcServerInterface.hpp
LVCPS_PacketConsoleMsg.cpp    inc=LVCPS_PacketConsoleMsg.hpp,LVC_IliveCacheSink.hpp

LVCPS_PacketSingleObj.hpp     inc=LVC_ProcServerInterface.hpp
LVCPS_PacketSingleObj.cpp     inc=LVCPS_PacketSingleObj.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketContainer.hpp     inc=LVC_ProcServerInterface.hpp,OMS_ContainerHandle.hpp
LVCPS_PacketContainer.cpp     inc=LVCPS_PacketContainer.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketVersion.hpp       inc=LVC_ProcServerInterface.hpp
LVCPS_PacketVersion.cpp       inc=LVCPS_PacketVersion.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketSchema.hpp        inc=LVC_ProcServerInterface.hpp,OMS_Defines.h
LVCPS_PacketSchema.cpp        inc=LVCPS_PacketSchema.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketRegion.hpp        inc=LVC_ProcServerInterface.hpp
LVCPS_PacketRegion.cpp        inc=LVCPS_PacketRegion.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketSubtrans.hpp      inc=LVC_ProcServerInterface.hpp
LVCPS_PacketSubtrans.cpp      inc=LVCPS_PacketSubtrans.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketConsistentView.hpp inc=LVC_ProcServerInterface.hpp
LVCPS_PacketConsistentView.cpp inc=LVCPS_PacketConsistentView.hpp,LVC_IliveCacheSink.hpp,\
                               OMS_DbpError.hpp

LVCPS_PacketMassObj.hpp       inc=LVC_ProcServerInterface.hpp,OMS_Defines.h
LVCPS_PacketMassObj.cpp       inc=LVCPS_PacketMassObj.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketIterator.hpp      inc=LVC_ProcServerInterface.hpp,OMS_Defines.h,OMS_IterDirection.hpp
LVCPS_PacketIterator.cpp      inc=LVCPS_PacketIterator.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketSequence.hpp      inc=LVC_ProcServerInterface.hpp
LVCPS_PacketSequence.cpp      inc=LVCPS_PacketSequence.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketFile.hpp          inc=LVC_ProcServerInterface.hpp
LVCPS_PacketFile.cpp          inc=LVCPS_PacketFile.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketABAPStream.hpp    inc=LVC_ProcServerInterface.hpp
LVCPS_PacketABAPStream.cpp    inc=LVCPS_PacketABAPStream.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketTasking.hpp       inc=LVC_ProcServerInterface.hpp
LVCPS_PacketTasking.cpp       inc=LVCPS_PacketTasking.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketSQL.hpp           inc=LVC_ProcServerInterface.hpp
LVCPS_PacketSQL.cpp           inc=LVCPS_PacketSQL.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp

LVCPS_PacketVersionDict.hpp   inc=LVC_ProcServerInterface.hpp,OMS_Defines.h \
                              dep=OMS_ClientInterface.hpp
LVCPS_PacketVersionDict.cpp   inc=LVCPS_PacketVersionDict.hpp,LVC_IliveCacheSink.hpp,\
                              OMS_DbpError.hpp,LVC_LibOmsInterface.hpp



# UDE-Server

#*-noex    -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING

LVCPS_ProcServerMain.cpp    inc=heo01.h,heo02.h,heo78.h,\
                            LVC_LoadLibrary.hpp,\
                            LVCPS_Dispatcher.hpp,\
                            LVCPS_KernelInterface.hpp,\
                            LVCPS_LiveCacheSink.hpp,\
                            LVCPS_ProcServerInterfaceInstance.hpp,\
                            OMS_LibOmsInterface.hpp,\
                            RTE_Message.hpp,\
                            RTECKC_RequestChannel.hpp,\
                            RTECKC_GlobalClientInstance.hpp,\
                            RTEIPC_Mailslot.hpp,\
                            RTEMem_Allocator.hpp \
                            dep=gos00.h

LVCPS_ProcServerInterfaceInstance.hpp  inc=LVC_ProcServerInterface.hpp,\
                                       RTE_Types.h
LVCPS_ProcServerInterfaceInstance.cpp  inc=LVCPS_ProcServerInterfaceInstance.hpp,\
                                       RTECKC_GlobalClientInstance.hpp,\
                                       RTE_Message.hpp

