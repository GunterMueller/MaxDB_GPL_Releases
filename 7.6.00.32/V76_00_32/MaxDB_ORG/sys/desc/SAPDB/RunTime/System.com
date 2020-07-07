#
#	RunTime System Call Wrapper
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

*.c -ansi
*.cpp -ansi

#
# include dependencies (do not edit)
#
RTESys_ProcessState.hpp  inc=:SAPDB/RunTime/RTE_Types.h,SAPDB_ToString.hpp
RTESys_ProcessState.cpp  inc=:SAPDB/RunTime/System/RTESys_ProcessState.hpp

RTESys_NTLoadDLL.hpp inc=:SAPDB/RunTime/RTE_MessageList.hpp\
,:SAPDB/RunTime/RTE_Message.hpp
RTESys_NTLoadDLLFunc.cpp inc=\
:SAPDB/RunTime/System/RTESys_NTLoadKernelDLL.hpp\
,:SAPDB/RunTime/System/RTESys_NTLoadDLLFunc.h
RTESys_NTLoadDLLFunc.h inc=:SAPDBCommon/SAPDB_C_Cplusplus.h
RTESys_NTLoadKernelDLL.cpp inc=\
:SAPDB/RunTime/System/RTESys_NTLoadKernelDLL.hpp\
,:SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp\
,:SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_MessageList.hpp\
,:SAPDB/RunTime/RTE_Message.hpp
RTESys_NTLoadKernelDLL.hpp inc=:SAPDB/RunTime/RTE_MessageList.hpp\
,:SAPDB/RunTime/RTE_Message.hpp\
,:SAPDB/RunTime/System/RTESys_NTLoadDLL.hpp
RTESys_Time.c inc=:SAPDB/RunTime/System/RTESys_Time.h,SAPDB_string.h
RTESys_Time.h inc=:SAPDB/RunTime/RTE_Types.h,RTE_CompilerFeatures.h
RTESys_Spinlock.h    inc=RTE_Types.h,RTE_CompilerFeatures.h
RTESys_Spinlock.c    inc=RTESys_Spinlock.h,RTESys_SystemInfo.h,RTESys_AtomicOperation.h
RTESys_Spinlock-k.c -DKERNEL_LZU inc=RTESys_Spinlock.h,RTESys_SystemInfo.h,RTESys_AtomicOperation.h,RTETask_Yield.h
RTESys_LockArea.hpp  inc=RTE_Types.h,RTESys_Spinlock.h,SAPDBErr_Assertions.hpp
#
# Assembler spinlock coding
#
&if $OS not in [ WIN32 ] && $MACH not in [ _IBMR2 ]
#
# For LINUX + IA64 + __INTEL_COMPILER this is also not needed, but i do not know how to set that here... (jrg)
RTESys_AsmSpinlock.s inc=
&endif

RTESys_Cmpxchg.h 
RTESys_Cmpxchg.c inc=RTESys_Cmpxchg.h

RTESys_NTAWE.hpp inc=SAPDB_Types.h,SAPDBErr_Assertions.hpp,RTESys_NTLoadDLLFunc.h,SAPDBTrace_Usage.hpp,SAPDBErr_MessageList.hpp,SAPDBMem_IAllocatorInfo.hpp,RTEMem_AllocatorRegister.hpp,RTESync_Spinlock.hpp
RTESys_NTAWE.cpp inc=RTE_Types.h,RTESys_NTAWE.hpp,RTEMem_Allocator.hpp,RTE_Crash.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTESys_NTLoadDLLFunc.h,RTE_AWE_Messages.hpp,RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp

RTESys_SystemInfo.h inc=RTE_Types.h,RTE_CompilerFeatures.h
RTESys_SystemInfo.c inc=RTESys_SystemInfo.h,geo60.h,geo007_1.h,SAPDB_sprintf.h,RTESys_CommandPipe.h,heo01.h\
&if $OS in [ WIN32 ]
,RTE_WindowsReleaseDependend.h,RTESys_NTLoadDLLFunc.h
&else
,RTE_saveUNIXcalls.h
&endif

# reduced variant for liboms
RTESys_SystemInfo-liboms.c -DLIBOMS inc=RTESys_SystemInfo.h,geo60.h,geo007_1.h,SAPDB_sprintf.h,heo01.h,RTESys_CommandPipe.h

# reduced variant for libhsscopy
RTESys_SystemInfo-libhsscopy.c -DLIBHSSCOPY inc=RTESys_SystemInfo.h,geo60.h,geo007_1.h,SAPDB_sprintf.h,heo01.h,RTESys_CommandPipe.h

RTESys_SharedLib.h     inc=RTE_CompilerFeatures.h,RTESys_SharedLib.hpp,RTE_Types.h
RTESys_SharedLib.hpp   inc=RTE_CompilerFeatures.h,RTE_MessageList.hpp
&if $OSSPEC in [ AIX ]
RTESys_SharedLib.cpp   inc=RTESys_SharedLib.h,RTESys_IO.h,RTESys_CommandPipe.h,SAPDB_MemCopyMoveNoCheck.hpp
&else
RTESys_SharedLib.cpp   inc=RTESys_SharedLib.h,SAPDB_MemCopyMoveNoCheck.hpp
&endif

RTESys_SharedLibCode.c inc=RTESys_SharedLib.h,SAPDB_sprintf.h

# &gar: need dynamic loader lib under linux and sun
&if $OSSPEC in [ LINUX, SOLARIS ] || $MACH == SUN
RTESys_SharedLibCode*.c parentlinkoption=-append=-ldl
&endif

RTESys_NTHighResTimer.h   inc=SAPDB_Types.h,RTE_CompilerFeatures.h
RTESys_NTHighResTimer.hpp inc=SAPDB_Types.h,SAPDB_Singleton.hpp
RTESys_NTHighResTimer.cpp inc=RTESys_NTHighResTimer.h,RTESys_NTHighResTimer.hpp,SAPDBMem_NewDestroy.hpp,RTESync_Spinlock.hpp,RTE_MessageList.hpp,RTE_Messages.hpp,RTE_Message.hpp,RTESys_MemoryBarrier.h,RTESys_AtomicOperation.hpp

RTESys_IO.h	inc=gsp00,SAPDB_C_Cplusplus.h,RTE_Types.h
&if $OS in [ WIN32 ]
RTESys_IO.c	inc=RTESys_IO.h,geo00_0.h,geo007_1.h,hsp77.h
&else
RTESys_IO.c	inc=RTESys_IO.h,geo00_0.h,geo007_1.h,hsp77.h,RTE_saveUNIXcalls.h
&endif

&if $OS in [ WIN32 ]
RTESys_MicroTime.c inc=:SAPDB/RunTime/System/RTESys_MicroTime.h dep=RTESys_NTHighResTimer.h
&else
RTESys_MicroTime.c inc=:SAPDB/RunTime/System/RTESys_MicroTime.h
&endif
RTESys_MicroTime.h inc=:SAPDB/RunTime/RTE_Types.h,RTE_CompilerFeatures.h
RTESys_MicroTimer.hpp inc=RTESys_MicroTime.h
RTESys_MicroTimer.cpp inc=RTESys_MicroTimer.hpp

RTESys_MemProtect.h inc=
RTESys_MemProtect.c inc=:SAPDB/RunTime/RTE_Types.h,:SAPDB/RunTime/System/RTESys_MemProtect.h

RTESys_CommandPipe.h inc=RTE_Types.h,SAPDB_C_Cplusplus.h
&if $OS in [ WIN32 ]
RTESys_CommandPipe.c inc=RTESys_CommandPipe.h
&else
RTESys_CommandPipe.c inc=RTESys_CommandPipe.h,RTE_saveUNIXcalls.h
&endif

RTESys_AtomicOperation.h inc=RTE_CompilerFeatures.h,SAPDB_Types.h dep=RTE_Configure.pl
RTESys_AtomicOperation.hpp inc=RTESys_AtomicOperation.h
RTESys_AtomicOperation*.c               inc=RTESys_AtomicOperation.h,RTE_Types.h,RTESys_Spinlock.h,RTESys_SystemInfo.h
RTESys_AtomicOperation-k.c -DKERNEL_LZU inc=RTESys_AtomicOperation.h,RTE_Types.h,RTESys_Spinlock.h,RTESys_SystemInfo.h

RTESys_MemoryInfo.cpp    inc=RTESys_MemoryInfo.hpp,RTESys_SystemInfo.h
RTESys_MemoryInfo.hpp    inc=SAPDB_Types.h

RTESys_MemoryBarrier.h   inc=RTE_CompilerFeatures.h dep=RTE_Configure.pl
RTESys_MemoryBarrier.s   inc=

RTESys_ProcessID.hpp     inc=RTE_Types.h\
&if $OSSPEC not in [ WIN32 ]
,RTE_saveUNIXcalls.h
&endif

# Security Attributes
RTESys_SecurityAttributes.h  		inc=RTE_Types.h
RTESys_SecurityAttributes.hpp		inc=RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_Types.h
RTESys_SecurityAttributes.cpp		inc=RTESys_SecurityAttributes.h,RTESys_SecurityAttributes.hpp,RTEMem_Allocator.hpp,RTE_Messages.hpp\
&if $OS in [ WIN32 ]
,gos00.h
&endif

RTESys_SecurityAttributes-k.cpp		-DKERNEL  \
                                    inc=RTESys_SecurityAttributes.h,RTESys_SecurityAttributes.hpp,RTEMem_Allocator.hpp,RTE_Messages.hpp\
&if $OS in [ WIN32 ]
,gos00.h,gos00k.h
&endif

RTESys_TestUserPaths.cpp	inc=RTE_IniFileHandling.h
