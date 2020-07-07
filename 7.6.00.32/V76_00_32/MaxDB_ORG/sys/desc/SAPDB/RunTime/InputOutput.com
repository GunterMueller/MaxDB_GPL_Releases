# ------------------------------------------------------------------------------
#
# RunTime Environment / InputOutput Descriptions
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

# -------------------------------------------------------------------------------------
# Interfaces and basic types:
# -------------------------------------------------------------------------------------
#
* -ansi
*
#
# include dependencies (do not edit)
#
RTEIO_DAFS.h inc=:SAPDB/RunTime/RTE_Types.h
RTEIO_DAFS.hpp inc=:SAPDB/RunTime/RTE_Types.h,\
    :SAPDB/RunTime/InputOutput/RTEIO_VolumeInfo.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
RTEIO_DirectoryIterator.cpp inc=\
    :SAPDB/RunTime/InputOutput/RTEIO_DirectoryIterator.hpp,heo06.h
RTEIO_DirectoryIterator.hpp inc=
RTEIO_IKernel.hpp inc=:SAPDB/RunTime/RTE_Types.h,\
    :SAPDB/RunTime/InputOutput/RTEIO_VolumeInfo.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
RTEIO_IMedium.cpp inc=:SAPDB/RunTime/InputOutput/RTEIO_IVolume.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_Medium.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_VolumeConfiguration.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_MediumConfiguration.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp
RTEIO_IMedium.hpp inc=:SAPDB/RunTime/RTE_Types.hpp
RTEIO_IVolume.cpp inc=:SAPDB/RunTime/InputOutput/RTEIO_IVolume.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_Volume.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_VolumeConfiguration.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_VolumeInfo.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
RTEIO_IVolume.hpp inc=:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_IMedium.hpp,\
    :SAPDB/Container/Container_Vector.hpp
RTEIO_Info.hpp inc=:SAPDB/RunTime/RTE_Types.h
RTEIO_Medium.cpp inc=:SAPDB/RunTime/InputOutput/RTEIO_Medium.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
RTEIO_Medium.hpp inc=:SAPDB/RunTime/InputOutput/RTEIO_IMedium.hpp
RTEIO_ShellUtil.cpp inc=:SAPDB/RunTime/InputOutput/RTEIO_ShellUtil.hpp,\
    heo06.h,:SAPDB/RunTime/InputOutput/RTEIO_DirectoryIterator.hpp,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/Messages/Msg_Arg.hpp
RTEIO_ShellUtil.hpp inc=:SAPDB/Messages/Msg_List.hpp
RTEIO_Volume.cpp inc=:SAPDB/RunTime/InputOutput/RTEIO_Volume.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
RTEIO_Volume.hpp inc=:SAPDB/RunTime/InputOutput/RTEIO_IVolume.hpp
RTEIO_VolumeInfo.cpp inc=:SAPDB/RunTime/InputOutput/RTEIO_VolumeInfo.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,heo07.h,\
    RTEThread_KernelThread.hpp,RTEIO_VolumeInfo.h,\
    heo58.h,heo47.h,:SAPDB/RunTime/RTE_MessageList.hpp,RTETask_IWorkerJob.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,geo50_0.h \
&if $OSSPEC not in [ WIN32 ]
    dep=hen40.h     
&endif

RTEIO_VolumeInfo.h inc=geo002.h
RTEIO_VolumeInfo.hpp inc=:SAPDB/RunTime/RTE_Types.h,\
    :SAPDB/RunTime/InputOutput/RTEIO_Info.hpp,\
    :SAPDB/Container/Container_Vector.hpp,vsp002
RTEIO_VolumeWrapper.cpp inc=:SAPDB/RunTime/InputOutput/RTEIO_IVolume.hpp

