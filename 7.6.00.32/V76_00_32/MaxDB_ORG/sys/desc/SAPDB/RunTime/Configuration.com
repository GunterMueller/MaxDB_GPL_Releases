#
#	RunTime Tasking
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



RTEConf_ParameterAccess.cpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,gsp01.h,\
    :SAPDB/RunTime/Configuration/RTEConf_BuildParamFileName.h,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,heo01.h


RTEConf_ParameterAccess.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,:SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/RTE_Messages.hpp,:SAPDB/RunTime/System/RTESys_IO.h,\
    :SAPDB/Container/Container_List.hpp

RTEConf_ParameterAccessKernelInterface.cpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h,\
    gsp11.h,heo11.h,hsp100.h,\
    :SAPDB/RunTime/Configuration/RTEConf_BuildParamFileName.h,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,geo50_0.h,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h,heo00.h,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_SystemPageCache.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_IOManager.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_DataAccess.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_Converter.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_RunTime.h,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_Task.hpp,\
    :SAPDB/RunTime/RTE_Version.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_RTEConf.h,\
    :SAPDB/RunTime/Threading/RTEThread_ConsoleStandardWorker.hpp
# New IO Interface
#RTEConf_ParameterAccessKernelInterface.cpp  -DKERNEL_LZU -DSAPDB_NEW_IO_INTERFACE inc=\
#    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
#    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h,\
#    :SAPDB/RunTime/RTE_Message.hpp,\
#    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,:SAPDB/SAPDBCommon/SAPDB_sprintf.h,\
#    geo50_0.h,\
#    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
#    :SAPDB/RunTime/Configuration/RTEConf_VolumeConfiguration.hpp,\
#    :SAPDB/RunTime/MemoryManagement/RTEMem_SystemPageCache.hpp,\
#    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
#    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
#    :SAPDB/RunTime/Configuration/RTEConf_BuildParamFileName.h,\
#    KernelParam_IOManager.hpp,KernelParam_Converter.hpp,KernelParam_RunTime.h,\
#    gsp11.h,heo11.h,heo00.h,\
#    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
#    :SAPDB/RunTime/Tasking/RTETask_Task.hpp,\
#    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
#    heo51.h
#RTEConf_ParameterAccessKernelInterface.cpp  -DKERNEL_LZU -DSAPDB_NEW_IO_INTERFACE inc=\
# this line is needed otherwise last line transforms next line into comment...

RTEConf_ParameterAccessKernelInterface.h inc=vsp0011,geo002.h

RTEConf_ParameterAccessKernelInterface.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/RunTime/RTE_ISystem.hpp,:SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/RunTime/RTE_Messages.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,\
    :SAPDB/Container/Container_List.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp,\
    geo002.h

RTEConf_ParameterAccessSimple.cpp inc=\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessSimple.h,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp

RTEConf_ParameterAccessSimple.h inc=gsp00

RTEConf_BuildParamFileName.c inc=gsp01.h,gsp11.h,heo01.h,heo47.h,geo60.h,\
    :SAPDB/RunTime/Configuration/RTEConf_BuildParamFileName.h

RTEConf_BuildParamFileName.h inc=

RTEConf_MediumConfiguration.cpp inc=\
    :SAPDB/RunTime/Configuration/RTEConf_MediumConfiguration.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_IMedium.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_Medium.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp

RTEConf_MediumConfiguration.hpp inc=:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp

RTEConf_VolumeConfiguration.cpp inc=\
    :SAPDB/RunTime/Configuration/RTEConf_VolumeConfiguration.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_IVolume.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_Volume.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp

RTEConf_VolumeConfiguration.hpp inc=:SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp

x.cpp inc=:SAPDB/RunTime/Configuration/RTEConf_VolumeConfiguration.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_IVolume.hpp,\
    :SAPDB/RunTime/InputOutput/RTEIO_Volume.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp

