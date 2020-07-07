#
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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

*-k.cpp -DSAPDB_KERNEL

ProcRTE_Runtime-k.cpp  -DSAPDB_KERNEL

#
# include dependencies (do not edit)
#
ProcRTE_LocalCommunication.c inc=\
    :SAPDB/RunTime/ProcServer/ProcRTE_LocalCommunication.h,heo46.h,\
    geo007_2.h
ProcRTE_LocalCommunication.cpp inc=\
    :SAPDB/RunTime/ProcServer/ProcRTE_LocalCommunication.h,gos003.h,\
    heo46.h,geo007_2.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
ProcRTE_LocalCommunication.h inc=gsp00\
&if $OSSPEC == WIN32 
,gos003.h\
&endif               

ProcRTE_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
ProcRTE_Runtime.cpp inc=:SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp,\
    hsp77.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp
ProcRTE_Runtime-k.cpp inc=:SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp,\
    hsp77.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp
ProcRTE_Runtime.hpp inc=
ProcRTE_ServerControl.cpp inc=\
    :SAPDB/RunTime/ProcServer/ProcRTE_ServerControl.hpp,hsp77.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_ServerSession.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Messages.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITaskClientConnection.hpp,\
    :SAPDB/RunTime/UDEServer/RTEUDE_SocketIPC.hpp
ProcRTE_ServerControl.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,gsp00
ProcRTE_ServerSession.cpp inc=\
    :SAPDB/RunTime/ProcServer/ProcRTE_ServerSession.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_ServerControl.hpp
ProcRTE_ServerSession.hpp inc=gsp00,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Socket.hpp
ProcRTE_Socket.cpp inc=:SAPDB/RunTime/ProcServer/ProcRTE_Socket.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp
ProcRTE_Socket.hpp inc=:SAPDB/RunTime/ProcServer/ProcRTE_StdTypes.hpp
ProcRTE_StdTypes.hpp inc=

