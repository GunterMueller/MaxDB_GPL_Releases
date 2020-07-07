# DBProc/DBProc.com
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

*-procsim -DPROCSERVER_SIMULATION

javaudeserver.jar binary ->$INSTROOT/runtime/jar/
Proc_PointerList-simple.cpp -DSIMPLE=1

#
#
# include dependencies (do not edit)
#
Proc_FactoryDirectory.cpp inc=:SAPDB/DBProc/Proc_FactoryDirectory.hpp,\
    :SAPDB/DBProc/Proc_IFactory.hpp,:SAPDB/DBProc/Proc_SystemProcs.hpp,\
    :SAPDB/DBProc/Proc_UDEDirectory.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/DBProc/Proc_SharedInternals.hpp
Proc_FactoryDirectory.hpp inc=\
    :SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp
Proc_IFactory.hpp inc=
Proc_IServerControl.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.h
Proc_ISession.hpp inc=
Proc_JavaUDE.cpp inc=gsp01.h
Proc_ParameterCopy.cpp inc=:SAPDB/DBProc/Proc_ParameterCopy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp
Proc_ParameterCopy.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp
Proc_PointerList.cpp inc=:SAPDB/DBProc/Proc_PointerList.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp
Proc_PointerList-simple.cpp inc=:SAPDB/DBProc/Proc_PointerList.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp
Proc_PointerList.hpp inc=
Proc_SQL.cpp inc=:SAPDB/DBProc/Proc_SQL.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Messages.hpp,\
    :SAPDB/DBProc/Proc_PointerList.hpp
Proc_SQL.hpp inc=
Proc_ServerControl.cpp inc=:SAPDB/DBProc/Proc_ServerControl.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITaskClientConnection.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_ServerControl.hpp,\
    :SAPDB/DBProc/Proc_ServerSession.hpp,\
    :SAPDB/DBProc/Proc_ServerSessionFactory.hpp,\
    :SAPDB/DBProc/Proc_ServerSessionIterator.hpp,\
    :SAPDB/DBProc/Proc_FactoryDirectory.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Messages.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp
Proc_ServerControl.hpp inc=:SAPDB/DBProc/Proc_IServerControl.hpp,\
    :SAPDB/DBProc/Proc_PointerList.hpp
Proc_ServerSession.cpp inc=:SAPDB/DBProc/Proc_ServerSession.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/DBProc/Proc_ParameterCopy.hpp,\
    :SAPDB/DBProc/Proc_ServerControl.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITaskClientConnection.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/PacketInterface/PIn_Packet.h,\
    :SAPDB/PacketInterface/PIn_RequestWriter.h,\
    :SAPDB/PacketInterface/PIn_Segment.h,vsp001,hsp77.h,\
    :SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    SQLManager/KernelSQL/KSQL_Runtime.hpp
Proc_ServerSessionprocsim.cpp inc=:SAPDB/DBProc/Proc_ServerSession.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/DBProc/Proc_ParameterCopy.hpp,\
    :SAPDB/DBProc/Proc_ServerControl.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITaskClientConnection.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/PacketInterface/PIn_Packet.h,\
    :SAPDB/PacketInterface/PIn_RequestWriter.h,\
    :SAPDB/PacketInterface/PIn_Segment.h,vsp001,hsp77.h,\
    :SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,heo03.h
Proc_ServerSession.hpp inc=:SAPDB/DBProc/Proc_ISession.hpp
Proc_ServerSessionFactory.cpp inc=\
    :SAPDB/DBProc/Proc_ServerSessionFactory.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,\
    :SAPDB/DBProc/Proc_ServerControl.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Messages.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/DBProc/Proc_SharedInternals.hpp
Proc_ServerSessionFactory.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.h,\
    :SAPDB/DBProc/Proc_IFactory.hpp,:SAPDB/DBProc/Proc_PointerList.hpp
Proc_ServerSessionIterator.cpp inc=\
    :SAPDB/DBProc/Proc_ServerSessionIterator.hpp,\
    :SAPDB/DBProc/Proc_IServerControl.hpp
Proc_ServerSessionIterator.hpp inc=:SAPDB/DBProc/Proc_PointerList.hpp
Proc_Session.cpp inc=:SAPDB/DBProc/Proc_Session.hpp
Proc_Session.hpp inc=:SAPDB/DBProc/Proc_ISession.hpp
Proc_SessionList.cpp inc=:SAPDB/DBProc/Proc_SessionList.hpp,\
    :SAPDB/DBProc/Proc_ISession.hpp
Proc_SessionList.hpp inc=:SAPDB/DBProc/Proc_UDEDirectory.hpp
Proc_SessionListContextInterface.cpp inc=\
    :SAPDB/DBProc/Proc_SessionListContextInterface.hpp,\
    :SAPDB/DBProc/Proc_SessionList.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
Proc_SessionListContextInterface.hpp inc=
Proc_SharedInternals.hpp inc=:SAPDB/Container/Container_Vector.hpp
Proc_SystemProcs.cpp inc=:SAPDB/DBProc/Proc_SystemProcs.hpp,hsp42.h,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Handler.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Messages.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp,\
    :SAPDB/DBProc/Proc_ParameterCopy.hpp,\
    :SAPDB/DBProc/Proc_SharedInternals.hpp,\
    :SAPDB/DBProc/Proc_IFactory.hpp,\
    :SAPDB/DBProc/Proc_IServerControl.hpp,\
    :SAPDB/DBProc/Proc_ISession.hpp,\
    :SAPDB/DBProc/Proc_FactoryDirectory.hpp,\
    :SAPDB/DBProc/Proc_ServerSessionIterator.hpp
Proc_SystemProcs.hpp inc=
Proc_UDEDirectory.cpp inc=:SAPDB/DBProc/Proc_UDEDirectory.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
Proc_UDEDirectory.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcObject.hpp

