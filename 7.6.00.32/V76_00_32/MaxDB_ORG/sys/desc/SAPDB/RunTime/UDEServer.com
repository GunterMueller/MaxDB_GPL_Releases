#
# RunTime Environment / User Defined Extension Server Descriptions
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

RTEUDE_ServerControl.hpp    inc=SAPDB_Types.hpp
RTEUDE_ServerControl.cpp    -DKERNEL_LZU \
                             inc=RTEUDE_ServerControl.hpp,RTE_Types.hpp,RTETask_Task.hpp,RTE_MessageList.hpp
#
# include dependencies (do not edit)
#
RTEUDE_CommandInterpreter.cpp inc=\
    :SAPDB/RunTime/UDEServer/RTEUDE_CommandInterpreter.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_IPC.hpp
RTEUDE_CommandInterpreter.hpp inc=
RTEUDE_IPC.cpp inc=:SAPDB/RunTime/UDEServer/RTEUDE_IPC.hpp
RTEUDE_IPC.hpp inc=
RTEUDE_Message.cpp inc=:SAPDB/RunTime/UDEServer/RTEUDE_Message.hpp
RTEUDE_Message.hpp inc=
RTEUDE_MessagePort.cpp inc=\
    :SAPDB/RunTime/UDEServer/RTEUDE_MessagePort.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_Message.hpp
RTEUDE_MessagePort.hpp inc=
RTEUDE_PipeIPC.cpp inc=:SAPDB/RunTime/UDEServer/RTEUDE_PipeIPC.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp\
&if $OSSPEC == WIN32
&else
,:SAPDB/RunTime/RTE_saveUNIXcalls.h\
&endif

RTEUDE_PipeIPC.hpp inc=:SAPDB/RunTime/UDEServer/RTEUDE_IPC.hpp
RTEUDE_RelayTestMain.cpp inc=\
    :SAPDB/RunTime/UDEServer/RTEUDE_StarterRelay.hpp
RTEUDE_RelayUnixMain.cpp inc=\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_CommandInterpreter.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_StarterRelay.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_PipeIPC.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_SocketIPC.hpp
RTEUDE_RelayWindowsMain.cpp inc=\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_Win32ServiceControl.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_CommandInterpreter.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_StarterRelay.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_PipeIPC.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_SocketIPC.hpp
RTEUDE_ServerControl.cpp inc=:SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_ServerControl.hpp
RTEUDE_ServerControl.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
RTEUDE_SocketIPC.cpp inc=:SAPDB/RunTime/UDEServer/RTEUDE_SocketIPC.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
RTEUDE_SocketIPC.hpp inc=:SAPDB/RunTime/UDEServer/RTEUDE_IPC.hpp
RTEUDE_StarterRelay.cpp inc=\
    :SAPDB/RunTime/UDEServer/RTEUDE_StarterRelay.hpp,gsp00,heo06.h,\
    hsp77.h,:SAPDB/RunTime/InputOutput/RTEIO_ShellUtil.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_IPC.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_CommandInterpreter.hpp
RTEUDE_StarterRelay.hpp inc=
RTEUDE_Win32ServiceControl.cpp inc=\
    :SAPDB/RunTime/UDEServer/RTEUDE_Win32ServiceControl.hpp
RTEUDE_Win32ServiceControl.hpp inc=

