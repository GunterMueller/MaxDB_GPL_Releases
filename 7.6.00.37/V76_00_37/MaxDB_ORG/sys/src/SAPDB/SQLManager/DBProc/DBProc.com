# --------------------------------------------------------------------------
# description: DBProc
#
# author:	thomasA
# --------------------------------------------------------------------------
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

DBProc_Messages.genmsg unpack=no noversion header=DBProc_Messages.hpp \
    execscript=$OWN/tool/bin/genmsg.pl

DBProc_Handler-80.cpp -DKERNEL80

#
# include dependencies (do not edit)
#
DBProc_DBProcDescriptor.cpp inc=\
    :SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcFileObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcInfoObject.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ISessionCache.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,hsp77.h
DBProc_DBProcDescriptor.hpp inc=\
    :SAPDB/SQLManager/Catalog/Catalog_DBProcObject.hpp
DBProc_Debugger.cpp inc=:SAPDB/SQLManager/DBProc/DBProc_Debugger.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,heo56.h,hkb71.h
DBProc_Debugger.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/Container/Container_AVLTree.hpp,\
    :SAPDB/Container/Container_Stack.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp
DBProc_Handler.cpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_MessBlockObject.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Handler.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Messages.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/DBProc/Proc_ServerSessionFactory.hpp,\
    :SAPDB/DBProc/Proc_IServerControl.hpp,\
    :SAPDB/DBProc/Proc_ISession.hpp,:SAPDB/DBProc/Proc_SessionList.hpp,\
    :SAPDB/DBProc/Proc_FactoryDirectory.hpp,\
    :SAPDB/DBProc/Proc_ParameterCopy.hpp,:SAPDB/DBProc/Proc_SQL.hpp,\
    :SAPDB/DBProc/Proc_IFactory.hpp,:SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBProc.h,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,hak07.h,hak071.h,\
    hak06.h,hak10.h,hak260.h,hak262.h,ggg01,hgg01_3.h,hgg04.h,hkb71.h,\
    hak663.h,hak651.h,hsp51.h,hsp77.h,hsp81.h
DBProc_Handler-80.cpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp,\
    :SAPDB/SQLManager/SQLMan_MessBlock.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_MessBlockObject.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Handler.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Messages.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Object.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/DBProc/Proc_ServerSessionFactory.hpp,\
    :SAPDB/DBProc/Proc_IServerControl.hpp,\
    :SAPDB/DBProc/Proc_ISession.hpp,:SAPDB/DBProc/Proc_SessionList.hpp,\
    :SAPDB/DBProc/Proc_FactoryDirectory.hpp,\
    :SAPDB/DBProc/Proc_ParameterCopy.hpp,:SAPDB/DBProc/Proc_SQL.hpp,\
    :SAPDB/DBProc/Proc_IFactory.hpp,:SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBProc.h,\
    :SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp,hak07.h,hak071.h,\
    hak06.h,hak10.h,hak260.h,hak262.h,ggg01,hgg01_3.h,hgg04.h,hkb71.h,\
    hak663.h,hak651.h,hsp51.h,hsp77.h,hsp81.h
DBProc_Handler.hpp inc=
DBProc_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
DBProc_PascalWrapper.cpp inc=hak101.h,\
    :SAPDB/SQLManager/DBProc/DBProc_Handler.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp

