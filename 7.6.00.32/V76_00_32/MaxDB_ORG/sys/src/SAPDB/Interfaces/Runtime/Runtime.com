# -----------------------------------------------------------------------------
#
# PURPOSE
# Handling Description for Runtime layer.
#
#
# -----------------------------------------------------------------------------
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
&if $OSSPEC in [ HPUX ]
* -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME +W1039
&else
* -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME
&endif
*-nomt -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME -DIFR_SINGLE_THREADED
*-kernel -DBUILD_INTERFACE_RUNTIME -DSQLDBC_FOR_KERNEL

IFR_Common*.h inc=\
    IFR_TraceContext.h,\
    IFR_Trace.h,\
    IFR_Types.h,\
    IFRUtil_New.h

IFR_Types.h inc=:SAPDB/Interfaces/SQLDBC/SQLDBC_Types.h setbuildinfo

IFR_Connection.h inc=IFR_ErrorHndl.h,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    IFRPacket_RequestPacket.h,\
    IFRPacket_ReplyPacket.h,\
    IFRPacket_LongDescriptor.h,\
    IFR_ConnectionItem.h,\
    IFR_Environment.h,\
    IFR_ConnectProperties.h,\
    IFR_Common.h,\
    IFR_Datatypes.h,\
    IFRUtil_UniqueID.h,\
    IFRUtil_AsyncOperation.h

IFR_Connection*.cpp inc=IFR_Common.h,\
    :SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_Connection.h,\
    IFR_Environment.h,\
    IFR_Statement.h,\
    IFR_PreparedStmt.h,\
    IFR_SQLWarning.h,\
    IFRPacket_Part.h,\
    IFR_ParseInfoCache.h,\
    SAPDBMem_IRawAllocator.hpp,\
    IFRPacket_Lock.h,\
    IFRUtil_StackAllocator.h,\
    IFRConversion_Factory.h

IFR_Environment.h inc=IFR_Common.h,IFR_String.h,IFRUtil_RuntimeItem.h,IFR_Profile.h,\
                      SAPDB/Interfaces/Runtime/Util/IFRUtil_LinkedList.h,\
                      SAPDB/Interfaces/SQLDBC/SQLDBC_StorageItems.h
  

IFR_Environment*.cpp inc=\
        IFR_Common.h,\
        IFR_Environment.h,\
        IFR_Connection.h,\
        SAPDBCommon/SAPDB_ReuseLib.h,\
        Interfaces/SQLDBC/SQLDBC_IRuntime.h,\
        gsp100.h, \
        setbuildinfo

IFR_ErrorHndl.h inc=IFR_String.h,\
    IFR_ErrorCode.h,\
    Interfaces/SQLDBC/SQLDBC_IRuntime.h,\
    IFR_Types.h

IFR_ErrorHndl*.cpp inc=IFR_ErrorHndl.h,\
    :SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_Trace.h,\
    IFR_TraceContext.h \
    concom=yes

IFR_ErrorHndl-doxygen.cpp concom=yes ascii DOXYGEN ->$WRK/fast/obj/SAPDB/Interfaces/Runtime/IFR_ErrorHndl.dxy

IFR_ErrorHndl-msg.cpp ascii \
    dep=:SAPDB/Interfaces/tools/generrmsg.pl \
    execscript=$OWN/tool/bin/generrmsg.pl \
    ->$INSTROOT/env/en/SQLDBCM.en

IFR_ResultSet.h inc=IFR_ErrorHndl.h,IFR_ParseInfo.h,IFR_Connection.h,IFR_FetchInfo.h,IFR_FetchChunk.h
IFR_ResultSet*.cpp inc=IFR_Common.h,IFR_ResultSet.h,IFR_ErrorHndl.h,IFR_Statement.h,IFRConversion_Converter.h,IFR_UpdatableRowSet.h

IFR_RowSet.h inc=IFR_ErrorHndl.h,IFR_Connection.h,IFR_ResultSet.h,IFRConversion_Getval.h,IFRPacket_RequestSegment.h,IFR_LOBHost.h
IFR_RowSet*.cpp inc=IFR_RowSet.h,IFR_Statement.h"

IFR_UpdatableRowSet.h inc=IFR_RowSet.h
IFR_UpdatableRowSet*.cpp inc=IFR_UpdatableRowSet.h,IFR_PreparedStmt.h

IFR_ParseInfo.h inc=\
    IFR_Types.h,\
    IFR_ResultSetMetaData.h,\
    IFR_ParameterMetaData.h,\
    IFR_Connection.h,\
    IFRPacket_FunctionCode.h,\
    IFRUtil_RuntimeItem.h

IFR_ParseInfo*.cpp inc=\
    IFR_ParseInfo.h,\
    IFR_Connection.h,\
    IFRPacket_FunctionCode.h,\
    IFRPacket_RequestPacket.h,\
    IFRPacket_PartEnum.h,\
    IFRPacket_ReplySegment.h,\
    IFRPacket_Part.h,\
    IFRPacket_RequestSegment.h,\
    IFR_ParseInfoCache.h

IFR_Statement.h inc=\
    :SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
    IFR_String.h,\
    IFR_ErrorHndl.h,\
    IFR_ResultSet.h,\
    IFRUtil_Vector.h,\
    IFR_Parameter.h,\
    IFRPacket_ReplyPacket.h,\
    IFR_Types.h,\
    IFR_FetchInfo.h,\
    IFR_ConnectionItem.h,\
    IFR_SQLWarning.h,\
    IFRUtil_RuntimeItem.h


IFR_Statement*.cpp inc=\
    IFR_Common.h,\
    SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_Statement.h,IFR_ErrorHndl.h,\
    IFR_Connection.h,\
    IFR_ResultSet.h,\
    IFRPacket_FunctionCode.h,\
    IFRPacket_PartEnum.h,\
    IFRPacket_Part.h,\
    IFRPacket_RequestSegment.h,\
    IFR_PreparedStmt.h

IFR_PreparedStmt.h inc=\
        IFR_Statement.h,\
        IFRUtil_DefaultRawAllocator.h,\
        :SAPDB/Runtime/Packet/IFRPacket_RequestPacket.h,\
        :SAPDB/Runtime/Packet/IFRPacket_ReplyPacket.h,\
        :SAPDB/Runtime/Packet/IFRPacket_RequestSegment.h,\
        :SAPDB/Runtime/IFR_LOBHost.h

IFR_PreparedStmt*.cpp inc=\
        IFR_PreparedStmt.h,\
        :SAPDB/Runtime/Packet/IFRPacket_PartEnum.h,\
        :SAPDB/Runtime/Packet/IFRPacket_Part.h,\
        IFR_Common.h,\
        :SAPDB/Runtime/Packet/IFRPacket_ReplySegment.h,\
        IFRPacket_FunctionCode.h,\
        IFRUtil_AsyncOperation.h,\
        IFR_ParseInfoCache.h

IFR_String.h inc=:SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_Types.h,\
    SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    IFRUtil_CopyTraits.h

IFR_String*.cpp inc=IFR_String.h,IFR_Trace.h,SAPDBMem_Alloca.h

IFR_Parameter.h inc=\
    IFR_Common.h,\
    IFR_Datatypes.h,\
    IFR_String.h,\
    IFR_LOB.h

IFR_FetchInfo.h inc=\
        IFRUtil_Vector.h,\
        IFRConversion_Converter.h,\
        IFRConversion_Getval.h,\
        IFR_ResultSetMetaData.h,\
        IFRUtil_RuntimeItem.h,\
        IFRPacket_RequestPacket.h,\
        IFRPacket_RequestSegment.h

IFR_FetchInfo*.cpp inc=\
        IFR_FetchInfo.h,\
        IFR_Statement.h,\
        IFRPacket_PartEnum.h,\
        IFRPacket_ReplySegment.h,\
        IFRPacket_Part.h,\
        IFRUtil_StackAllocator.h,\
        IFR_Common.h

IFR_FetchChunk.h inc=IFR_Common.h,IFRPacket_ReplySegment.h
IFR_FetchChunk*.cpp inc=IFR_FetchChunk.h,IFRPacket_Part.h


IFR_ConnectionItem*.cpp inc=IFR_ConnectionItem.h,IFR_ErrorHndl.h,IFR_SQLWarning.h,IFR_Connection.h
IFR_ConnectionItem.h inc=\
    Interfaces/SQLDBC/SQLDBC_IRuntime.h,\
    IFR_SQLWarning.h,\
    IFR_ErrorHndl.h,\
    SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp

IFR_Shortinfo.h inc=\
    :SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_Common.h,\
    IFR_String.h,\
    IFR_Datatypes.h,\
    IFRUtil_Vector.h

IFR_Shortinfo*.cpp inc=\
    IFR_ShortInfo.h

IFR_Datatypes.h inc=:SAPDBCommon/SAPDB_ReuseLib.h,IFR_Types.h
IFR_Datatypes*.cpp inc=IFR_Datatypes.h

IFR_ErrorCode.h
IFR_Parameter*.cpp \
            dep=OMS_LiveCacheTypes.xml \
            inc=\
                ,IFR_Parameter.h\
                ,IFRConversion_Converter.h

IFR_SQLWarning.h inc=IFR_Types.h,IFR_String.h
IFR_SQLWarning*.cpp inc=IFR_SQLWarning.h

IFR_ConnectProperties.h inc=IFR_Common.h,IFR_Types.h,IFR_String.h,IFRUtil_Vector.h
IFR_ConnectProperties*.cpp inc=IFR_ConnectProperties.h,RTE_IInterface.hpp

IFR_ParseInfoCache.h inc=IFR_ParseInfo.h
IFR_ParseInfoCache*.cpp inc=IFR_ParseInfoCache.h,IFRUtil_Vector.h,IFRUtil_HashMap.h

IFR_Trace.h inc=\
    :SAPDBCommon/SAPDB_ReuseLib.h,\
    IFRUtil_New.h,\
	IFRUtil_RuntimeItem.h,\
	IFR_String.h,\
    IFR_Datatypes.h,\
    :SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h


IFR_Trace*.cpp inc=\
    IFR_Types.h,\
    IFR_Trace.h,\
    IFRUtil_Time.h

IFR_Trace-notrace.cpp inc=\
        IFR_Types.h,\
        IFR_Trace.h,\
        IFRUtil_Time.h \
        -DIFR_NO_TRACE

IFR_ParameterMetaData.h inc=\
        IFRConversion_Converter.h,\
        :SAPDB/Interfaces/SQLDBC/SQLDBC.h

IFR_ResultSetMetaData.h inc=\
        IFRConversion_Converter.h,\
        :SAPDB/Interfaces/SQLDBC/SQLDBC.h

IFR_TraceContext.h inc=IFR_Trace.h

IFR_TraceContext*.cpp inc=\
    IFR_Common.h,\
    IFR_TraceContext.h,\
    IFR_ConnectionItem.h,\
    IFRUtil_RuntimeItem.h,\
    IFR_Environment.h,\
    IFR_Connection.h,\
    IFR_ResultSet.h,\
    IFR_Statement.h,\
    IFR_RowSet.h,\
    IFR_FetchInfo.h,\
    IFR_Statement.h

IFR_LOB.h inc=IFR_Types.h,IFR_ConnectionItem.h,IFR_Datatypes.h

IFR_LOB*.cpp inc=IFR_LOB.h,IFR_LOBHost.h,IFR_Common.h

IFR_LOBHost.h inc=IFR_Common.h,IFRUtil_Vector.h,IFRConversion_Getval.h,IFRConversion_Putval.h,IFRPacket_ReplySegment.h,IFRPacket_RequestPacket.h,IFRPacket_RequestSegment.h,IFR_Connection.h
IFR_LOBHost*.cpp inc=IFR_LOBHost.h

# Special file for OMS SQLDBC Version
IFR_liboms_Unresolved*.cpp inc=gsp03

IFR_Profile.h inc=IFR_Types.h,IFRUtil_LinkedList.h
IFR_Profile*.cpp inc=IFR_Profile.h,IFR_Environment.h
