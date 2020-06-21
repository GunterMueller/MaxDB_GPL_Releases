#
#   Trace
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

* -I$WRK/incl/SAPDB

regex(*\.hpp$)              inc=gsp00,ggg00,SAPDBCommon/SAPDB_Types.hpp

Trace_DevSpace.hpp

Trace_DevSpace.cpp

Trace_Manager.hpp

Trace_Manager.h

Trace_Manager.cpp

Trace_Page.hpp

Trace_InfoPage.hpp
                               

Trace_InfoPage.h

Trace_InfoPage.cpp -DKERNEL_LZU

Trace_Slot.hpp

Trace_Slot.cpp

Trace_SlotPool.hpp

#
# TraceSort tool (exception handling allowed...)
#
Trace_Sort.cpp -except

Trace_Types.hpp         

Trace_Types.cpp

Trace_Wrapper.cpp

Trace_Writer.hpp

Trace_Writer.cpp

############################################
## Modules containing trace entry layouts ##
############################################

Trace_DevSpaceIOEntry.hpp

Trace_Entry.hpp

Trace_Entry.cpp

Trace_MessBlockEntry.hpp

Trace_MessBlockEntry.cpp

Trace_PacketEntry.hpp

Trace_PageNoEntry.hpp


#
# include dependencies (do not edit)
#
Trace_DevSpace.cpp inc=gsp03,gsp03_3,heo54k.h,heo58.h,hgg01_1.h,vsp002,\
    :SAPDB/Trace/Trace_DevSpace.hpp,:SAPDB/Trace/Trace_InfoPage.hpp
Trace_DevSpace.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Trace_DevSpaceIOEntry.hpp inc=ggg00,ggg11,gsp00,vsp002,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp
Trace_Entry.cpp inc=ggg91.h,:SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/Trace/Trace_Manager.hpp,:SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp
Trace_Entry.hpp inc=gsp00,ggg00,ggg11,:SAPDB/Trace/Trace_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Trace_InfoPage.cpp inc=:SAPDB/Trace/Trace_InfoPage.h,\
    :SAPDB/Trace/Trace_InfoPage.hpp,geo50_0.h,geo002.h
Trace_InfoPage.h inc=gsp00
Trace_InfoPage.hpp inc=gsp00
Trace_Manager.cpp inc=heo55k.h,heo56.h,hgg01.h,hgg01_3.h,hgg08.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITaskScheduler.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/RTE_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Trace/Trace_Manager.hpp,:SAPDB/Trace/Trace_Writer.hpp,\
    :SAPDB/KernelCommon/Kernel_Task.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp
Trace_Manager.h inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Trace_Manager.hpp inc=gsp00,ggg00,:SAPDB/KernelCommon/Kernel_Common.hpp,\
    :SAPDB/KernelCommon/Kernel_TaskWaitList.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,\
    :SAPDB/Trace/Trace_DevSpace.hpp,:SAPDB/Trace/Trace_Types.hpp,\
    :SAPDB/Trace/Trace_SlotPool.hpp
Trace_MessBlockEntry.cpp inc=:SAPDB/Trace/Trace_MessBlockEntry.hpp,\
    :SAPDB/Trace/Trace_Manager.hpp
Trace_MessBlockEntry.hpp inc=gsp00,ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/Trace/Trace_Types.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp
Trace_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Trace_PacketEntry.hpp inc=gsp00,ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,:SAPDB/Trace/Trace_Manager.hpp
Trace_Page.hpp inc=gsp00,ggg11,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Trace/Trace_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Trace_PageNoEntry.hpp inc=ggg00,ggg11,gsp00,:SAPDB/Trace/Trace_Entry.hpp
Trace_Slot.cpp inc=ggg90.h,hgg01.h,hsp77.h,\
    :SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,:SAPDB/Trace/Trace_Slot.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,:SAPDB/Trace/Trace_InfoPage.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp
Trace_Slot.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/Trace/Trace_DevSpace.hpp,:SAPDB/Trace/Trace_Page.hpp,\
    :SAPDB/Trace/Trace_Types.hpp
Trace_SlotPool.hpp inc=gsp00,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/Trace/Trace_Slot.hpp
Trace_Sort.cpp inc=:SAPDB/Trace/Trace_Types.hpp,\
    :SAPDB/Trace/Trace_InfoPage.hpp,:SAPDB/Trace/Trace_Page.hpp,\
    :SAPDB/Trace/Trace_Entry.hpp,geo54.h
Trace_Types.hpp inc=ggg11,gsp00,ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Trace_Wrapper.cpp inc=gsp00,ggg00,gsp03,hgg01_1.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/Trace/Trace_Entry.hpp,\
    :SAPDB/Trace/Trace_Manager.hpp,:SAPDB/Trace/Trace_MessBlockEntry.hpp,\
    :SAPDB/Trace/Trace_PacketEntry.hpp,:SAPDB/Trace/Trace_Writer.hpp
Trace_Writer.cpp inc=hkb38.h,hkb52_1.h,hkb57.h,hbd90.h,hbd91.h,hbd20_7.h,\
    hgg01.h,hgg02.h,hgg08.h,heo51.h,heo56.h,heo58.h,heo55k.h,\
    :SAPDB/Converter/Converter_IDump.hpp,:SAPDB/Pager/Pager_IDump.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/KernelCommon/Kernel_Dump.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_SuspendReason.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/Trace/Trace_Manager.hpp,:SAPDB/Trace/Trace_Writer.hpp,\
    :SAPDB/Trace/Trace_Messages.hpp
Trace_Writer.hpp inc=gsp00,ggg00,:SAPDB/DataAccess/Data_PageFrame.hpp,\
    :SAPDB/IOManager/IOMan_IPage.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp

