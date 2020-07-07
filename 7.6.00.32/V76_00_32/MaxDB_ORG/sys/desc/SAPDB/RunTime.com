#
# RunTime Environment Descriptions
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
*-k    -DKERNEL_LZU
#
# Variant without UNIX password verification (used inside kernel)
#
*-nocrypt -DNO_CRYPT
#
RTE_Types.h     inc=SAPDB_Types.h
RTE_Types.hpp   inc=RTE_Types.h,SAPDB_ToString.hpp
#
# Header for 'TheRTE interface'
RTE_IGlobal.hpp inc=RTE_Types.hpp
#
# TheRTE singleton with support for internal tasking and multithreading (i.e. kernel....)
RTE_Global*.cpp  inc=SAPDBMem_NewDestroy.hpp,RTE_IGlobal.hpp,SAPDBErr_Assertions.hpp,RTETask_IContext.hpp,RTEThread_IContext.hpp,RTE_MessageList.hpp
#
# TheRTE singleton without support for internal tasking, but support for multithreading
# Usefull for multi threaded processes without internal tasking only
RTE_GlobalMultiThread*.cpp  inc=SAPDBMem_NewDestroy.hpp,RTE_IGlobal.hpp,SAPDBErr_Assertions.hpp,RTEThread_IContext.hpp,RTE_MessageList.hpp
#
# TheRTE singleton without support for internal tasking, but support for multithreading
# Usefull for single threaded processes only
RTE_GlobalSimpleProcess*.cpp  inc=SAPDBMem_NewDestroy.hpp,RTE_IGlobal.hpp,SAPDBErr_Assertions.hpp,RTE_MessageList.hpp
#
# Header for 'TheRTE system functions interface'
RTE_ISystem.hpp inc=RTE_Types.hpp,RTESys_MemProtect.h
#

RTE_Version.cpp	inc=hgg11.h,geo002.h,hsp100.h,SAPDB_Types.h

&if $OS = WIN32

RTE_SystemNT*.cpp   inc=SAPDBMem_NewDestroy.hpp,RTE_ISystem.hpp,SAPDBErr_Assertions.hpp,RTESys_Spinlock.h,RTESys_SystemInfo.h,RTEMem_SystemPageCache.hpp,RTEMem_PseudoStackAllocator.hpp
#RTE_SystemNT*.cpp   inc=SAPDBMem_NewDestroy.hpp,RTE_ISystem.hpp,SAPDBErr_Assertions.hpp,RTESys_Spinlock.h,RTESys_SystemInfo.h

&else

RTE_SystemUNIX*.cpp inc=SAPDBMem_NewDestroy.hpp,RTE_ISystem.hpp,SAPDBErr_Assertions.hpp,RTESys_Spinlock.h,RTESys_SystemInfo.h,RTEMem_SystemPageCache.hpp,RTEMem_PseudoStackAllocator.hpp,SDBMsg_RTEMem.h,Msg_List.hpp,SAPDB_sprintf.h
#RTE_SystemUNIX*.cpp inc=SAPDBMem_NewDestroy.hpp,RTE_ISystem.hpp,SAPDBErr_Assertions.hpp,RTESys_Spinlock.h,RTESys_SystemInfo.h
&  if $MACH in [ PA11, PA20W ]
RTE_SystemPA.h inc=
&  endif

&endif
#
# Basic Locking Testprogram
RTE_LockTest*.cpp inc=RTE_ISystem.hpp
#
# Basic Locked Counter Testprogram
RTE_CounterTest*.cpp inc=RTE_IGlobal.hpp,RTE_ISystem.hpp,RTESync_InterlockedCounter.hpp
#
# RTE Kernel Initialization
#
RTE_KernelMain.h   inc=SAPDB_Types.h
RTE_KernelMain*.cpp inc=RTE_KernelMain.h,RTE_KGS.hpp,RTE_UniqueId.h,heo670.h,RTETst_Default.hpp,RTETst_RWRegionCode.hpp,RTE_KernelInterfaceInstance.hpp,RTESys_AtomicOperation.hpp,:SAPDB/RunTime/Security/Crypto/RTECrypto_EntropyPool.hpp
#
# RTE_StackBacktrace code
#
RTE_StackBacktrace.h inc=SAPDB_C_Cplusplus.h
RTE_StackBacktrace.c inc=RTE_StackBacktrace.h,heo670.h
#
# RTE_Debugger calling code
#
RTE_CallDebugger.h           inc=SAPDB_Types.h,SAPDB_C_Cplusplus.h
RTE_CallDebugger.cpp         inc=RTE_CallDebugger.h,RTEMem_Allocator.hpp
#
# RTE_Message SAPDBErr_MessageList output in knldiag
#
RTE_Message.cpp              inc=RTE_Message.hpp,SAPDB_ToString.hpp,heo60_2.h,SAPDB_string.h,Msg_IOutput.hpp
RTE_Message-n.cpp            inc=RTE_Message.hpp,SAPDB_ToString.hpp,heo60_2.h,SAPDB_string.h,Msg_IOutput.hpp -DNEW_DIAG_FILES
RTE_Message.hpp              inc=SAPDBErr_MessageList.hpp
#
# RTE_Crash   Abort() including SAPDBErr_MessageList output
#
RTE_Crash.cpp                inc=RTE_Crash.hpp,RTE_Message.hpp,Msg_List.hpp,geo007_1.h,geo60.h
RTE_NonKernelCrash.cpp       inc=RTE_Crash.hpp,RTE_Message.hpp
RTE_Crash.hpp                inc=
#
# RTE_ItemRegister Singleton template class to register items
#
RTE_ItemRegister.hpp         inc=RTESync_Spinlock.hpp,RTE_Message.hpp,RTE_Messages.hpp,RTE_MessageList.hpp,SAPDBErr_MessageList.hpp,RTEMem_UnregisteredAllocator.hpp

#
RTE_EmergencyChannel.hpp     inc=SAPDBErr_Diagnostic.hpp
RTE_EmergencyChannel.cpp     inc=RTE_EmergencyChannel.hpp,SAPDBMem_NewDestroy.hpp
RTE_TestEmergencyChannel.cpp inc=SAPDBErr_Diagnostic.hpp,SAPDBErr_MessageList.hpp,RTE_ISystem.hpp
#
# RTE Messages
#
RTE_Messages.genmsg          ascii

RTE_Messages.hpp                 inc=SAPDBErr_MessageList.hpp
RTE_AWE_Messages.hpp             inc=SAPDBErr_MessageList.hpp
RTE_Comm_Messages.hpp            inc=SAPDBErr_MessageList.hpp
RTE_XCons_Messages.hpp           inc=SAPDBErr_MessageList.hpp
RTE_Console_Thread_Messages.hpp  inc=SAPDBErr_MessageList.hpp
RTE_Console_Messages.hpp         inc=SAPDBErr_MessageList.hpp
RTE_HSS_Messages.hpp             inc=SAPDBErr_MessageList.hpp
RTE_CKC_Messages.hpp             inc=SAPDBErr_MessageList.hpp
RTE_SystemPageCache_Messages.hpp inc=SAPDBErr_MessageList.hpp
RTE_IPC_Messages.hpp             inc=SAPDBErr_MessageList.hpp
RTE_Sync_Messages.hpp            inc=SAPDBErr_MessageList.hpp
RTE_Task_Messages.hpp            inc=SAPDBErr_MessageList.hpp
RTE_Thread_Messages.hpp          inc=SAPDBErr_MessageList.hpp
RTE_Requestor_Messages.hpp       inc=SAPDBErr_MessageList.hpp
RTE_Test_Messages.hpp            inc=SAPDBErr_MessageList.hpp
RTE_IO_Messages.hpp              inc=SAPDBErr_MessageList.hpp
RTE_Mem_Messages.hpp             inc=SAPDBErr_MessageList.hpp

RTE_Message.hpp              inc=SAPDB_Types.hpp
RTE_MessageList.hpp          inc=RTE_Messages.hpp
#
# RTE_KGS
#
RTE_KGS.hpp                  inc=SAPDB_Singleton.hpp,geo50_0.h,SAPDB_ToString.hpp
RTE_KGS.cpp                  inc=SAPDBMem_NewDestroy.hpp,RTE_KGS.hpp,RTE_ISystem.hpp,RTEMem_Allocator.hpp,RTEMem_PageAllocator.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTESys_MicroTime.h,RTESys_SystemInfo.h,RTE_MySQLProxy.hpp,SDBMsg_RunTime.h,SAPDB_MemCopyMoveNoCheck.hpp\
&if $OSSPEC = WIN32
	,geo002.h
&else
	,geo002.h,RTE_UNIXAuthenticate.hpp,RTESys_IO.h
&endif
#
# RTE_KSS
#
RTE_KSS.h                    inc=gsp100.h,RTE_Types.h,Kernel_SharedState.h

#
RTE_UniqueId.h               inc=SAPDB_C_Cplusplus.h,RTE_Types.h
RTE_UniqueId.c               inc=RTE_UniqueId.h,RTESys_Spinlock.h,SAPDB_string.h

#
# RTE_Speed
#
RTE_Speed.cpp  			            inc=SAPDB_C_Cplusplus.h,RTE_Types.h
RTE_Speed-omstst.cpp -DSAPDB_OMSTST inc=SAPDB_C_Cplusplus.h,RTE_Types.h

#
# RTE_CurrentStack
#
RTE_CurrentStack.hpp         inc=RTE_Types.h
#
# The -NO is not needed for __INTEL_COMPILER due to special coding in GetCurrentStack()... PTS 1140067
#
&if $COMPILER != ICC
RTE_CurrentStack.cpp -NO     inc=RTE_CurrentStack.hpp,geo50_0.h\
&else
RTE_CurrentStack.cpp         inc=RTE_CurrentStack.hpp,geo50_0.h\
&endif
&if ($MACH = IA64) || ($MACH = HP_IA64) || ($MACHINE = IA64) # PTS 1128113
                             ,RTE_IA64GetBsp.h
&endif

#
# RTE_Test
#
RTE_Test.hpp inc=ggg00
RTE_Test.cpp inc=RTE_Test.hpp,RTETst_IParser.hpp,RTE_Message.hpp,SAPDB_ToString.hpp,RTETask_ITask.hpp,SQLMan_Context.hpp,RTE_Test_Messages.hpp,RTEConf_ParameterAccessKernelInterface.hpp

#
# Common Routines for XCons and Console Thread
#
&if ($OSSPEC == LINUX && $__HAS_NPTL != 1) || $MACH == SUN || $OSSPEC == SOLARIS
RTE_ConsoleDataCommunication.hpp    inc=RTE_UniqueId.h,RTEComm_PacketHeader.h,geo900.h,RTE_ConsoleCommon.hpp,RTESys_Spinlock.h
&else
RTE_ConsoleDataCommunication.hpp    inc=RTE_UniqueId.h,RTEComm_PacketHeader.h,geo900.h,RTE_ConsoleCommon.hpp
&endif

RTE_ConsoleDataCommunication*.cpp   inc=heo41.h,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Crash.hpp,RTE_Console_Messages.hpp,RTE_ConsoleCommon.hpp\
                                        ,RTE_ConsoleDataCommunication.hpp,RTE_ConsoleStandardDataRequests.h,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp\
                                        ,SAPDB_sprintf.h,SAPDB_MemCopyMoveNoCheck.hpp\
&if $OSSPEC = WIN32
                                        ,gos41.h
&else
&if ($OSSPEC == LINUX && $__HAS_NPTL != 1) || $MACH == SUN || $OSSPEC == SOLARIS
                                        ,gen41.h,RTE_UNIXAuthenticate.hpp,RTE_saveUNIXcalls.h,RTE_ConsoleSemaphoreTimeoutList.hpp
&else
                                        ,gen41.h,RTE_UNIXAuthenticate.hpp
&endif

&endif

RTE_ConsoleRequestCommunication.hpp  inc=

RTE_ConsoleRequestCommunication*.cpp inc=SAPDB_C_Cplusplus.h,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_MessageList.hpp,RTE_Message.hpp\
                                         ,RTE_Console_Messages.hpp,RTE_ConsoleCommon.hpp,RTE_ConsoleRequestCommunication.hpp,RTEMem_RteAllocator.hpp\
                                         ,SAPDB_sprintf.h,SAPDB_MemCopyMoveNoCheck.hpp\
&if $OSSPEC = WIN32
                                        ,gos41.h
&else
                                        ,gen41.h,hen40.h,RTE_saveUNIXcalls.h
&endif

RTE_ConsoleTraceTopic.hpp           inc=heo07.h
RTE_ConsoleTraceTopic*.cpp          inc=gsp01.h,heo01.h,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Console_Messages.hpp,RTEMem_RteAllocator.hpp\
                                        ,SAPDBTrace_Stream.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,SAPDB_sprintf.h,RTE_ConsoleTraceTopic.hpp\
                                        ,RTESys_Time.h,RTESys_IO.h,SAPDB_MemCopyMoveNoCheck.hpp\
&if $OSSPEC = WIN32
                                        ,gos44.h
&else
                                        ,hen40.h
&endif

&if $OSSPEC == WIN32 || (($OSSPEC == LINUX && $__HAS_NPTL != 1) || $MACH == SUN || $OSSPEC == SOLARIS)
RTE_ConsoleCommon.hpp               inc=geo001.h,RTE_UniqueId.h,RTEComm_PacketHeader.h
&else
RTE_ConsoleCommon.hpp               inc=geo001.h,RTE_UniqueId.h,RTEComm_PacketHeader.h,geo07.h
&endif

RTE_ConsoleCommon.cpp               inc=RTE_ConsoleCommon.hpp

&if $OSSPEC = WIN32
RTE_ConsoleStandardDataRequests.h   inc=heo46.h,heo00.h,geo52.h,geo002.h,gos00k.h,RTETask_Task.h
&else
RTE_ConsoleStandardDataRequests.h   inc=heo46.h,heo00.h,geo52.h,geo002.h,RTETask_Task.h
&endif

&if ($OSSPEC == LINUX && $__HAS_NPTL != 1) || $MACH == SUN || $OSSPEC == SOLARIS
RTE_ConsoleSemaphoreTimeoutList.hpp inc=heo07.h,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTEMem_RteAllocator.hpp
RTE_ConsoleSemaphoreTimeoutList*.cpp inc=gen00.h,RTESys_Time.h,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Crash.hpp\
                                        ,RTE_Console_Messages.hpp,RTE_ConsoleSemaphoreTimeoutList.hpp,RTE_saveUNIXcalls.h,RTE_ConsoleDataCommunication.hpp\
                                        ,RTEThread_KernelThread.hpp,RTEThread_Thread.hpp
&endif

#
# End Common Routines for XCons and Console Thread
#


RTE_DBRegister.hpp \
    inc=RTE_Types.h,RTEMem_Allocator.hpp,Container_List.hpp,RTE_Installation.hpp,gsp09.h

RTE_SpeedInfo.hpp \
    inc=RTE_Types.h,Container_List.hpp

RTE_Database.hpp \
    inc=RTE_Types.h,RTEMem_Allocator.hpp,RTE_SpeedInfo.hpp,RTE_Installation.hpp

RTE_Database.cpp \
&if $OS in [ WIN32 ]
    inc=RTE_NTService.hpp,RTE_Database.hpp,gos00.h
&endif

RTE_ActiveDatabase.hpp \
    inc=RTE_Types.h,RTEMem_Allocator.hpp,Container_List.hpp,RTE_Installation.hpp,RTE_ActiveInstallation.hpp,RTE_DiagPipe.h,RTE_KSS.h,RTE_Database.hpp,RTESync_BinarySemaphore.hpp,RTEIPC_NamedSharedMemory.hpp,Kernel_SharedState.h\
&if $OSSPEC in [ WIN32 ]
,gos00.h
&endif

RTE_Installation.hpp \
    inc=RTE_Types.h,RTEMem_Allocator.hpp,Container_List.hpp,RTE_SpeedInfo.hpp,SAPDB_sprintf.h,RTE_PidListForExecutable.hpp\
&if $OS in [ WIN32 ]
,gos44.h
&endif

RTE_ActiveInstallation.hpp \
    inc=RTE_Types.h,Container_List.hpp,RTE_Installation.hpp

RTE_GetDBRootOfDB.h \
    inc=SAPDB_Types.h

RTE_GetDBRootOfDB.c \
    inc=RTE_Types.h,SAPDB_sprintf.h,RTE_IniFileHandling.h\
&if $OS in [ WIN32 ]
,gos00.h
&else
,RTE_saveUNIXcalls.h,SAPDB_Names.h,heo01.h
&endif

RTE_DiagPipe.hpp inc=RTESys_IO.h
RTE_DiagPipe.h   inc=RTE_DiagPipe.hpp

RTE_DiagPipe.cpp \
    inc=RTE_Types.h,RTESys_IO.h,SAPDB_sprintf.h,RTEComm_PacketHeader.h,SAPDBMem_NewDestroy.hpp,RTE_DiagPipe.h,SAPDB_MemCopyMoveNoCheck.hpp,heo07.h\
&if $OS in [ WIN32 ]
&else
        ,gen41.h
&endif

RTE_UserProfileContainer.hpp    inc=gsp00,RTE_MessageList.hpp,RTE_Types.h
RTE_UserProfileContainer.cpp    inc=gsp01.h,heo01.h,geo49.h,SAPDB_MemCopyMoveNoCheck.hpp,RTE_Message.hpp,SAPDB_string.h,RTEMem_RteAllocator.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_UserProfileContainer.hpp,SAPDBMem_NewDestroy.hpp

RTE_NTService.hpp inc=RTE_SpeedInfo.hpp

RTE_NTService.cpp \
    inc=RTE_MessageList.hpp,RTE_SpeedInfo.hpp,geo002.h,RTE_Messages.hpp,RTE_DBRegister.hpp,gos44.h,geo007_1.h,heo01.h,hsp100.h,heo11.h,RTEConf_ParameterAccess.hpp,RTEConf_ParameterAccessSimple.h,RTE_NTService.hpp

RTE_NTService-w.cpp  -DCLUSTER_SERVER_VERSION \
    inc=RTE_MessageList.hpp,RTE_SpeedInfo.hpp,geo002.h,RTE_Messages.hpp,RTE_DBRegister.hpp,gos44.h,geo007_1.h,heo01.h,hsp100.h,heo11.h,RTEConf_ParameterAccess.hpp,RTEConf_ParameterAccessSimple.h,gos212.h,RTE_NTService.hpp

RTE_SpeedInfo.cpp \
    inc=RTE_SpeedInfo.hpp\
&if $OS in [ WIN32 ]
        ,gos00.h
&else

&endif

RTE_DBRegister.cpp \
    inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Messages.hpp,RTE_SpeedInfo.hpp,RTE_Installation.hpp,RTE_ActiveInstallation.hpp,RTE_Database.hpp,RTE_ActiveDatabase.hpp,RTE_DBRegister.hpp,SAPDB_sprintf.h,\
        RTE_GetDBRootOfDB.h,heo05.h,heo44.h,gsp00,gsp09.h,SAPDB_Names.h,SDBMsg_RTEDBState.h,RTE_IniFileHandling.h,heo01.h\
&if $OS in [ WIN32 ]
        ,gos00.h,gos00k.h,gos94.h
&else
        ,gen41.h
&endif

RTE_ActiveDatabase-w.cpp -DCLUSTER_SERVER_VERSION  \
    inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Messages.hpp,RTE_DBRegister.hpp,RTE_Database.hpp,RTE_ActiveDatabase.hpp,SAPDB_sprintf.h,RTEComm_PacketHeader.h,\
        heo01.h,heo05.h,heo44.h,RTEConf_ParameterAccess.hpp,geo002.h,hsp100.h,gsp09.h,RTESys_SecurityAttributes.hpp,RTESys_MemoryBarrier.h,RTESys_AtomicOperation.hpp,Msg_List.hpp,SAPDB_string.h\
&if $OS in [ WIN32 ]
        ,gos00k.h,gos212.h,RTE_NTService.hpp
&else
        ,RTE_saveUNIXcalls.h,gen41.h,hen40.h,RTE_IniFileHandling.h,gen34.h,RTE_PidListForExecutable.hpp\
&  if $MACH in [ PA11, PA20W, HP_IA64 ]
        ,hin100.h
&  endif
&endif

RTE_ActiveDatabase.cpp \
    inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Messages.hpp,RTE_DBRegister.hpp,RTE_Database.hpp,RTE_ActiveDatabase.hpp,SAPDB_sprintf.h,RTEComm_PacketHeader.h,\
        heo01.h,heo44.h,RTEConf_ParameterAccess.hpp,geo002.h,hsp100.h,gsp09.h,heo05.h,RTESys_SecurityAttributes.hpp,RTESys_MemoryBarrier.h,RTESys_AtomicOperation.hpp,Msg_List.hpp,SAPDB_string.h\
&if $OS in [ WIN32 ]
        ,gos00k.h,RTE_NTService.hpp
&else
        ,RTE_saveUNIXcalls.h,gen41.h,hen40.h,RTE_IniFileHandling.h,gen34.h,RTE_PidListForExecutable.hpp\
&  if $MACH in [ PA11, PA20W, HP_IA64 ]
        ,hin100.h
&  endif
&endif

RTE_Installation.cpp \
    inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Messages.hpp,RTE_Database.hpp,RTE_Installation.hpp,RTE_DBRegister.hpp,SAPDB_sprintf.h,\
        RTE_GetDBRootOfDB.h,RTE_NTService.hpp,SAPDB_Names.h\
&if $OS in [ WIN32 ]
        ,gos00.h,gos41.h
&else
        ,gen41.h,RTE_IniFileHandling.h,heo01.h,RTE_PidListForExecutable.hpp
&endif

RTE_ActiveInstallation.cpp \
    inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_Messages.hpp,RTE_ActiveInstallation.hpp,RTE_Database.hpp,RTE_DBRegister.hpp,SAPDB_sprintf.h,\
        SAPDB_Names.h,RTE_IniFileHandling.h\
&if $OS in [ WIN32 ]
        ,RTE_NTService.hpp,gos00.h
&else
        ,heo01.h
&endif

RTE_PidListForExecutable.hpp \
    inc=RTE_Types.h,RTEMem_RteAllocator.hpp

RTE_PidListForExecutable.cpp \
    inc=RTE_PidListForExecutable.hpp,SAPDB_Types.hpp,RTE_Types.hpp,SAPDB_sprintf.h,SAPDB_Names.h,RTE_saveUNIXcalls.h,RTESys_CommandPipe.h,RTESys_IO.h,RTEMem_RteAllocator.hpp,RTE_PidListForExecutable.hpp

RTE_FileUser.cpp \
	inc=RTE_FileUser.hpp,SAPDB_MemCopyMoveNoCheck.hpp

RTE_HandleManager.hpp \
    inc=SAPDB_Types.hpp,SAPDB_ToString.hpp,RTESync_Spinlock.hpp
RTE_HandleManager.cpp \
    inc=SAPDB_Types.hpp,RTEMem_RteAllocator.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTESync_Spinlock.hpp,RTE_HandleManager.hpp



#
# Testprogram used to check timing of shm_open compared with shmget
#
RTE_CommTest.h inc=
RTE_CommTest.c inc=RTE_CommTest.h
RTE_CommProgram.c inc=RTE_CommTest.h,RTESys_MemoryBarrier.h,SAPDB_string.h


#
# Signal save wrapper for UNIX system calls
#


RTE_saveUNIXcalls.h inc=gen00.h,RTE_Types.h
&if $MACH in [ SUN,ALPHA ] || $OSSPEC in [ LINUX,SOLARIS ]
RTE_saveUNIXcalls.c parentlinkoption=-append=-lrt inc=RTE_saveUNIXcalls.h,SAPDBErr_MessageOutput.h
&else
RTE_saveUNIXcalls.c inc=RTE_saveUNIXcalls.h,SAPDBErr_MessageOutput.h
&endif

#
# Configure
#
RTE_Configure.pl ascii exec \
    execscript=./RTE_Configure.pl \
    extdep=$WRK/incl/SAPDB/RunTime/RTE_Configure.h
&if defined XCOMP
&if defined BIT64
RTE_Configure.pl dep=RTE_Configure_${MACH}_${OSSPEC}_64.fixed
&else
RTE_Configure.pl dep=RTE_Configure_${MACH}_${OSSPEC}_32.fixed
&endif
regex(RTE_Configure.*[.]fixed) ascii ->$WRK/obj/SAPDB/RunTime/RTE_Configure.fixed
&endif

#
# Client Trace Topics
#

RTE_ClientTraceTopics*.cpp          inc=SAPDBTrace_Stream.hpp,SAPDBTrace_Topic.hpp

&if $MACH in [ PA11, PA20W ]
RTE_Coroutine.h                     inc=SAPDB_Types.h,RTE_Types.h,RTE_CompilerFeatures.h,RTE_SystemPA.h,RTE_SystemLinux.h
&else
RTE_Coroutine.h                     inc=SAPDB_Types.h,RTE_Types.h,RTE_CompilerFeatures.h,RTE_SystemLinux.h
&endif
RTE_Coroutine.cpp                   inc=geo002.h,geo50_0.h,SAPDBMem_IRawAllocator.hpp,RTE_Coroutine.h,RTESys_AtomicOperation.hpp
RTE_IExternalCall.hpp inc=SAPDB_Types.hpp

&if $OSSPEC == WIN32
RTE_ExternalCall.cpp inc=RTE_IExternalCall.hpp,SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,RTEMem_RteAllocator.hpp,SAPDB_sprintf.h,RTE_MessageList.hpp,RTESys_CommandPipe.h,RTESys_IO.h,RTE_Message.hpp,heo01.h,RTE_KGS.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTE_DiagPipe.h,gos00k.h,RTEThread_KernelThread.hpp
RTE_StartDatabase.cpp inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,geo00_2.h,RTE_ActiveDatabase.hpp,RTE_DBRegister.hpp,RTE_GetDBRootOfDB.h,heo01.h,geo00_2.h
RTE_StopDatabase.cpp  inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_ActiveDatabase.hpp,RTE_DBRegister.hpp,RTE_GetDBRootOfDB.h,heo01.h
&else
RTE_ExternalCall.cpp inc=RTE_IExternalCall.hpp,SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,RTEMem_RteAllocator.hpp,SAPDB_sprintf.h,RTE_MessageList.hpp,RTESys_CommandPipe.h,RTESys_IO.h,RTE_Message.hpp,heo01.h,RTE_KGS.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTE_DiagPipe.h,gen41.h,RTEThread_KernelThread.hpp
RTE_StartDatabase.cpp inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,geo00_2.h,RTE_ActiveDatabase.hpp,RTE_DBRegister.hpp,RTE_GetDBRootOfDB.h,heo01.h dep=RTE_UNIXAuthenticate.hpp,RTE_saveUNIXcalls.h,geo00_2.h
RTE_StopDatabase.cpp  inc=SAPDB_Types.hpp,RTE_Types.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_ActiveDatabase.hpp,RTE_DBRegister.hpp,RTE_GetDBRootOfDB.h,heo01.h dep=RTE_UNIXAuthenticate.hpp
&endif

&if $OSSPEC == WIN32
RTE_WindowsReleaseDependend.c       inc=RTE_WindowsReleaseDependend.h
RTE_WindowsReleaseDependend.h       inc=SAPDB_Types.h,SAPDB_C_Cplusplus.h
&endif

RTE_IniFileHandling.h               inc=RTE_Types.h,SAPDB_C_Cplusplus.h,gsp00
RTE_IniFileHandling.c               inc=SAPDB_Names.h,RTE_IniFileHandling.h,SAPDB_string.h,RTE_UNIXAuthenticate.h,geo007_2.h,heo46.h,heo01.h,SAPDB_sprintf.h \
&if $OSSPEC in [ WIN32 ]
                                    dep=gos00.h
&else
									dep=heo07.h
&endif
RTE_IniFileHandling-k.c   -DKERNEL  inc=SAPDB_Names.h,RTE_IniFileHandling.h,SAPDB_string.h,RTE_UNIXAuthenticate.h,geo007_2.h,heo46.h,heo01.h,SAPDB_sprintf.h \
&if $OSSPEC in [ WIN32 ]
                                    dep=gos00.h
&else
									dep=heo07.h
&endif
RTE_IniFileHandling-multithread.c -DSDB_MULTI_THREAD inc=SAPDB_Names.h,RTE_IniFileHandling.h,SAPDB_string.h,RTE_UNIXAuthenticate.h \
&if $OSSPEC in [ WIN32 ]
                                    dep=gos00.h,geo007_2.h
&else
									dep=
&endif

RTE_GlobalConfig.cpp                inc=RTE_IniFileHandling.h,SAPDB_Names.h

RTE_UNIXAuthenticate.h              inc=RTE_Types.h,SAPDB_C_Cplusplus.h
RTE_UNIXAuthenticate.hpp            inc=RTE_Types.hpp
RTE_UNIXAuthenticate*.cpp           inc=RTE_UNIXAuthenticate.hpp,RTE_IniFileHandling.h,\
    RTE_saveUNIXcalls.h,RTESys_IO.h,RTE_MessageList.hpp,SAPDBErr_Assertions.hpp,SAPDB_Names.h,\
    heo01.h,SAPDB_sprintf.h,geo28.h,RTESync_Spinlock.hpp,RTE_UNIXAuthenticate.h,\
    SDBMsg_RunTime.h,RTEMem_Allocator.hpp,RTESys_SharedLib.hpp dep=RTE_Configure.pl
RTE_VerifyPassword.cpp              inc=RTE_UNIXAuthenticate.hpp,RTE_saveUNIXcalls.h,Msg_List.hpp
RTE_VerifyPassword-check.cpp  -DSDB_PASSWORD_TEST inc=RTE_UNIXAuthenticate.hpp,RTE_saveUNIXcalls.h,Msg_List.hpp
RTE_GetDbroot.cpp                   inc=RTE_IniFileHandling.h,SAPDB_Names.h,RTE_SdbStarter.h
&if $OSSPEC not in [ WIN32 ]
RTE_SdbStarter.h                    inc=RTE_CompilerFeatures.h
RTE_SdbStarter-program.cpp -DCOMPILE_SDBSTARTER_PROGRAM \
                                    inc=heo01.h,RTE_SdbStarter.h,RTE_IniFileHandling.h,SAPDB_Names.h,RTE_Message.hpp,RTE_saveUNIXcalls.h,RTE_UNIXAuthenticate.hpp,RTESys_IO.h,Msg_List.hpp
RTE_SdbStarter-call.cpp             inc=heo01.h,RTE_SdbStarter.h,RTE_IniFileHandling.h,SAPDB_Names.h,RTE_Message.hpp,RTE_saveUNIXcalls.h,RTE_UNIXAuthenticate.hpp,RTESys_IO.h,Msg_List.hpp
RTE_SdbStarter-testcode.cpp -DCOMPILE_SDBTEST_PROGRAM \
                                    inc=heo01.h,RTE_SdbStarter.h,RTE_IniFileHandling.h,SAPDB_Names.h,RTE_Message.hpp,RTE_saveUNIXcalls.h,RTE_UNIXAuthenticate.hpp,RTESys_IO.h,Msg_List.hpp
&endif

#---------------------------------------------------------------
# Kernel internal MySQL-Proxy and direct MySQL-Proxy support
#
# Pascal Interface
RTE_MySQLProxy.h inc=gsp00,RTE_CompilerFeatures.h
# C++ classes
RTE_MySQLProxy.hpp inc=SAPDB_Types.hpp,SAPDB_Singleton.hpp
# Extern "C" implementation for kernel
RTE_MySQLProxy.cpp inc=SAPDBMem_NewDestroy.hpp,RTE_MySQLProxy.hpp,RTE_MySQLProxy.h,RTESys_Time.h,SAPDB_MemCopyMoveNoCheck.hpp

RTE_MySQLTest.cpp inc=RTE_MySQLProxy.hpp,RTE_MySQLProxy.h
#
# Kernel internal MySQL-Proxy and direct MySQL-Proxy support
#---------------------------------------------------------------

#NT Building Global Namespace
RTE_GlobalNameSpaceNT.h				inc=RTE_CompilerFeatures.h
RTE_GlobalNameSpaceNT.c				inc=gos00.h

#UNIX Tag File Handling
RTE_TagFileHandlingUNIX.hpp			inc=SAPDB_ToString.hpp,RTE_MessageList.hpp
RTE_TagFileHandlingUNIX.cpp			inc=RTE_saveUNIXcalls.h,RTE_TagFileHandlingUNIX.hpp,SAPDB_sprintf.h,RTE_IniFileHandling.h,SAPDB_Names.h,RTE_Messages.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp

RTE_ITaskStackControl.hpp inc=RTE_Types.hpp,SAPDB_Singleton.hpp
RTE_TaskStackControl.cpp -DKERNEL_LZU inc=RTE_ITaskStackControl.hpp,RTEMem_RteAllocator.hpp,RTE_Crash.hpp,RTESys_AtomicOperation.hpp,RTESys_MemProtect.h,geo007_1.h,geo50_0.h,geo002.h,geo70k.h,RTESys_MemoryBarrier.h,heo07.h,RTEMem_PseudoStackAllocator.hpp

RTE_IInterface.hpp inc=RTE_ISystem.hpp
RTE_CommonInterfaceInstance.hpp inc=RTE_IInterface.hpp
RTE_CommonInterfaceInstance.cpp inc=RTE_CommonInterfaceInstance.hpp,\
									RTESys_ProcessID.hpp,\
									RTESys_Spinlock.h,\
									RTESys_Time.h,\
									RTESys_AtomicOperation.hpp,\
									RTESys_MemoryBarrier.h,\
									RTEMem_RteAllocator.hpp,\
									Msg_Registry.hpp,\
									RTE_Message.hpp,\
									RTE_Crash.hpp
RTE_KernelInterfaceInstance.hpp inc=RTE_CommonInterfaceInstance.hpp
RTE_KernelInterfaceInstance.cpp inc=RTE_KernelInterfaceInstance.hpp,\
									SAPDBMem_NewDestroy.hpp,\
									heo02x.h,\
									RTETask_Task.hpp
RTE_KernelInterfaceInstance-simul.cpp -DTESTSIMUL_SMALL inc=RTE_KernelInterfaceInstance.hpp,\
									SAPDBMem_NewDestroy.hpp,\
									heo02x.h
RTE_ClientInterfaceInstance.hpp inc=RTE_CommonInterfaceInstance.hpp
RTE_ClientInterfaceInstance.cpp inc=RTE_ClientInterfaceInstance.hpp,\
									SAPDBMem_NewDestroy.hpp,\
									heo02x.h,\
									RTE_Message.hpp

RTE_PendingEventHandler.hpp inc=gsp00,vsp0031,SAPDB_Types.h,RTEThread_KernelThread.hpp,RTEMem_RteAllocator.hpp
RTE_PendingEventHandler.h inc=gsp00,vsp0031,SAPDB_Types.h,RTE_PendingEventHandler.hpp
RTE_PendingEventHandler.cpp -DKERNEL_LZU inc=RTE_Types.h,heo07.h,heo60.h,geo60.h,geo007_1.h,RTE_PendingEventHandler.h,RTESync_NamedSpinlock.hpp,SAPDBMem_NewDestroy.hpp,RTEMem_RteAllocator.hpp,RTESys_AtomicOperation.hpp,geo50_0.h

RTE_EnvironmentVariables.h    inc=
RTE_EnvironmentVariables.cpp  inc=RTE_EnvironmentVariables.h,SAPDB_string.h,SAPDB_Types.hpp,SAPDBMem_Alloca.h

RTE_Random.hpp     inc=RTE_IRandom.hpp
RTE_Random.cpp     inc=RTE_Random.hpp
RTE_OldKernelCommunication.hpp inc=gsp00,gsp01.h,RTE_Types.hpp

