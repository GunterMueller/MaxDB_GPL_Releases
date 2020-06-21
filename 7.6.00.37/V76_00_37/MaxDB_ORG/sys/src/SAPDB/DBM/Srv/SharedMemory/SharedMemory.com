#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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

DBMSrvShM_Base.cpp setbuildinfo

#
# include dependencies (do not edit)
#
DBMSrvShM_AlignmentHelper.hpp inc=
DBMSrvShM_Base.cpp inc=heo06.h,heo41.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_AlignmentHelper.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InitialisationSynchroniser.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp,\
    :SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/RunTime/System/RTESys_LockArea.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp
DBMSrvShM_Base.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Types.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_HalfSwitcher.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ParticipantSet.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessID.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp
DBMSrvShM_BaseEnumerator.cpp inc=heo41.h,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_AlignmentHelper.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_BaseEnumerator.hpp,\
    :SAPDB/RunTime/System/RTESys_LockArea.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h
DBMSrvShM_BaseEnumerator.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp
DBMSrvShM_BaseUnlocker.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_BaseUnlocker.hpp
DBMSrvShM_BaseUnlocker.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp
DBMSrvShM_HalfSwitcher.hpp inc=
DBMSrvShM_InitialisationSynchroniser.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InitialisationSynchroniser.hpp
DBMSrvShM_InitialisationSynchroniser.hpp inc=
DBMSrvShM_InternalResource.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp
DBMSrvShM_InternalResource.hpp inc=
DBMSrvShM_LineAccessorBase.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp
DBMSrvShM_LineAccessorEventing.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp
DBMSrvShM_LineAccessorEventing.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorBase.hpp
DBMSrvShM_LineAccessorResourceLock.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutResourceLock.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
DBMSrvShM_LineAccessorResourceLock.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorBase.hpp
DBMSrvShM_LineAccessorScheduler.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/RunTime/RTE_ISystem.hpp,:SAPDB/SAPDBCommon/SAPDB_string.h
DBMSrvShM_LineAccessorScheduler.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorBase.hpp
DBMSrvShM_LineIteratorEventing.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineIteratorEventing.hpp
DBMSrvShM_LineIteratorEventing.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutEventing.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp
DBMSrvShM_LineLayoutBase.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp
DBMSrvShM_LineLayoutBase.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessID.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
DBMSrvShM_LineLayoutEventing.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutEventing.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp
DBMSrvShM_LineLayoutEventing.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp
DBMSrvShM_LineLayoutResourceLock.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutResourceLock.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp
DBMSrvShM_LineLayoutResourceLock.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp
DBMSrvShM_LineLayoutScheduler.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutScheduler.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp
DBMSrvShM_LineLayoutScheduler.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp
DBMSrvShM_ParticipantSet.cpp inc=\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ParticipantSet.hpp
DBMSrvShM_ParticipantSet.hpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessID.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp
DBMSrvShM_Persistence.cpp inc=hcn42.h,heo06.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvShM_Persistence.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvShM_ShmAdmin.cpp inc=gsp01.h,heo01.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp,\
    :SAPDB/ToolsCommon/Tools_HexBuffer.hpp
DBMSrvShM_ShmAdmin.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutEventing.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutResourceLock.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutScheduler.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp
DBMSrvShM_ShmInfo.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmInfo.hpp
DBMSrvShM_ShmInfo.hpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_BaseEnumerator.hpp
DBMSrvShM_ShmInfoReplier.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmInfoReplier.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp
DBMSrvShM_ShmInfoReplier.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmInfo.hpp
DBMSrvShM_ShmMigrator.cpp inc=gsp05,heo01.h,heo06.h,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmMigrator.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvShM_ShmMigrator.hpp inc=
DBMSrvShM_Types.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h

