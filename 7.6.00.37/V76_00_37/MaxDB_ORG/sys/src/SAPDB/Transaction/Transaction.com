
#
# include dependencies (do not edit)
#
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

Trans_Context.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Messages/Msg_List.hpp,ggg90.h,ggg00
Trans_ExtendedMonitoring.cpp inc=hgg01.h,hgg08.h,heo55k.h,gkb05,hkb52.h,\
    hkb51_2.h,:SAPDB/Transaction/Trans_ExtendedMonitoring.hpp,\
    :SAPDB/Transaction/Trans_ExtendedMonitoringIter.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp
Trans_ExtendedMonitoring.hpp inc=ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Transaction/Trans_ExtendedMonitoringContext.hpp
Trans_ExtendedMonitoringContext.cpp inc=\
    :SAPDB/Transaction/Trans_ExtendedMonitoringContext.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h
Trans_ExtendedMonitoringContext.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg91.h,\
    :SAPDB/RunTime/System/RTESys_Time.h
Trans_ExtendedMonitoringIter.cpp inc=\
    :SAPDB/Transaction/Trans_ExtendedMonitoringIter.hpp
Trans_ExtendedMonitoringIter.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Transaction/Trans_ExtendedMonitoring.hpp,\
    :SAPDB/Transaction/Trans_ExtendedMonitoringContext.hpp
Trans_GlobalContext.cpp inc=hkb52.h,\
    :SAPDB/Transaction/Trans_GlobalContext.hpp
Trans_GlobalContext.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,gkb05
Trans_Iterator.cpp inc=:SAPDB/Transaction/Trans_Iterator.hpp,ggg00,gkb05,\
    hkb52.h,hgg08.h,hkb51_2.h,hkb03_1.h,heo55k.h,\
    :SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/Transaction/Trans_ExtendedMonitoringContext.hpp,\
    :SAPDB/Transaction/Trans_GlobalContext.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Transaction.h
Trans_Iterator.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h,gsp00,gkb05
Trans_Wrapper.cpp inc=:SAPDB/Transaction/Trans_GlobalContext.hpp,\
    hkb509.h

