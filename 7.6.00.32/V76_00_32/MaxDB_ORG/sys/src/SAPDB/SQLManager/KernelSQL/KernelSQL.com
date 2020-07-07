

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



KSQL_Messages.genmsg    ascii

KSQL_SQLDBC.hpp inc=:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
                    :SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h

KSQL_Connection.cpp inc=:SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Runtime.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Messages.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_SQLDBC.hpp
KSQL_Connection.hpp inc=:SAPDB/SQLManager/KernelSQL/KSQL_Statement.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_SQLDBC.hpp
KSQL_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
KSQL_PreparedStatement.cpp inc=\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_SQLDBC.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h
KSQL_PreparedStatement.hpp inc=\
    :SAPDB/SQLManager/KernelSQL/KSQL_Statement.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/Interfaces/Runtime/IFR_Types.h,\
    :SAPDB/RunTime/System/RTESys_Time.h
KSQL_PreparedStatementScope.cpp inc=\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatementScope.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_KSQL.h,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
KSQL_PreparedStatementScope.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
KSQL_ResultSet.cpp inc=:SAPDB/SQLManager/KernelSQL/KSQL_ResultSet.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Messages.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Interface.hpp
KSQL_ResultSet.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/SQLManager/KernelSQL/KSQL_SQLDBC.hpp
KSQL_Runtime.cpp inc=heo51.h,hak51.h,hak93.h,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Runtime.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/SQLMan_TempFileCollection.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/PacketInterface/PIn_Packet.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
KSQL_Runtime.hpp inc=:SAPDB/SQLManager/KernelSQL/KSQL_SQLDBC.hpp
KSQL_Statement.cpp inc=:SAPDB/SQLManager/KernelSQL/KSQL_Statement.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Messages.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
KSQL_Statement.hpp inc=:SAPDB/SQLManager/KernelSQL/KSQL_ResultSet.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_SQLDBC.hpp


