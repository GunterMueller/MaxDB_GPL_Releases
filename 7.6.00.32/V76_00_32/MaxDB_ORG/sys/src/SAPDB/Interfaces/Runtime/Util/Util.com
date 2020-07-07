# -----------------------------------------------------------------------------
# 
# PURPOSE
# Handling Description for Runtime Util layer.
# 
# RELEASE:  SAP DB 8.0
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

* -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME 
*-nomt -I$OWN/sys/wrk/sdk/incl -DBUILD_INTERFACE_RUNTIME -DIFR_SINGLE_THREADED
*-kernel -DBUILD_INTERFACE_RUNTIME -DSQLDBC_FOR_KERNEL

IFRUtil_VDNNumber*.cpp inc=IFRUtil_VDNNumber.h,IFR_Common.h,SAPDBMem_Alloca.h,hsp40.h,hsp41.h

IFRUtil_SQLNumeric*.cpp inc=IFRUtil_SQLNumeric.h,IFR_Common.h
IFRUtil_SQLNumeric.h inc=IFR_Common.h,IFR_Datatypes.h

IFRUtil_Vector.h inc=\
    IFR_Types.h,\
    SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    IFRUtil_Algo.h,\
    IFRUtil_CopyTraits.h


IFRUtil_DefaultRawAllocator.h inc=SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp
IFRUtil_DefaultRawAllocator*.cpp inc=IFRUtil_DefaultRawAllocator.h,SAPDBCommon/SAPDB_ReuseLib.h,RTE_CallDebugger.h,IFR_Common.h

IFRUtil_New.h inc=\
    SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    IFRUtil_CopyTraits.h,\
    IFRUtil_StackAllocator.h

IFRUtil_VDNNumber.h inc=IFR_ErrorHndl.h

IFRUtil_StackAllocator.h inc=IFR_Types.h,SAPDBMem_IRawAllocator.hpp

IFRUtil_UniqueID.h inc=\
	IFR_String.h,\
	IFRUtil_RuntimeItem.h
	
IFRUtil_UniqueID*.cpp inc=IFRUtil_UniqueID.h

IFRUtil_Pair.h inc=IFR_Types.h
IFRUtil_Hashtable.h inc=\
    IFR_Types.h,\
    IFRUtil_Algo.h,\
    SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    IFRUtil_Pair.h,IFRUtil_Vector.h

IFRUtil_Algo.h inc=IFR_Types.h
IFRUtil_HashMap.h inc=IFRUtil_Hashtable.h,IFRUtil_Pair.h

IFRUtil_Tls.h inc=SAPDBCommon/SAPDB_ReuseLib.h,heo07.h
IFRUtil_Tls*.cpp inc=IFR_Types.h,SAPDBCommon/SAPDB_ReuseLib.h,IFRUtil_Tls.h

IFRUtil_Time.h inc=IFR_Types.h,SAPDBCommon/SAPDB_ReuseLib.h
IFRUtil_Time*.cpp inc=IFRUtil_Time.h

IFRUtil_TraceSharedMemory.h inc=\
    SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h,\
    IFR_ErrorHndl.h,\
    SAPDBCommon/SAPDB_ReuseLib.h,\
    IFR_Common.h,\
    SAPDB/Runtime/Synchronisation/RTESync_Spinlock.hpp

IFRUtil_TraceSharedMemory*.cpp inc=\
    IFRUtil_TraceSharedMemory.h,\
    IFRUtil_Configuration.h,\
    SAPDBMem_Alloca.h,\
    RTE_IniFileHandling.h

IFRUtil_RuntimeItem.h inc=SQLDBC_IRuntime.h

IFRUtil_Stream.h inc=\
    IFR_ErrorHndl.h
IFRUtil_Stream*.cpp inc=\
    IFRUtil_Stream.h,\
    IFR_Common.h

IFRUtil_Tls_Reliant*.cpp inc=IFR_Types.h,SAPDBCommon/SAPDB_ReuseLib.h


IFRUtil_Configuration.h inc=\
    IFR_Types.h
 
IFRUtil_Configuration*.cpp inc=\
    IFRUtil_Configuration.h,\
    RTE_IniFileHandling.h,\
    SAPDBMem_Alloca.h,\
    SAPDB_Names.h

IFRUtil_CopyTraits.h inc=IFR_Types.h,SAPDBMem_IRawAllocator.hpp

IFRUtil_LinkedList.h inc=\
	SAPDB/Interfaces/Runtime/IFR_Types.h,\
	SAPDB/Interfaces/SQLDBC/SQLDBC_Internals.h,\
	SAPDB/Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h

IFRUtil_BinaryToHex.h inc=hsp78_0.h
IFRUtil_BinaryToHex*.cpp inc=IFRUtil_BinaryToHex.h,hsp77.h,IFR_Common.h

IFRUtil_AsyncOperation*.cpp inc=IFRUtil_AsyncOperation.h
IFRUtil_AsyncOperation.h inc=IFR_Types.h

IFRUtil_DateTime.h inc=IFR_Types.h