
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
Log_Types-diagnose.cpp inc=:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp -DINDIAGNOSE=1
# ------------------------------------------------------------------------------
#
# Platform dependend compilation option.... Do NOT place at end of this file...
#
# PTS 1111952 UH 2001-09-26
&if $OSSPEC == OSF1 && $__DECCXX > 600
Log_RecordColumnMap.cpp -OPT=-O1
&endif

&if $OSSPEC == LINUX && $MACH in [ SDBonPPC64 ]
Log_Volume.cpp -OPT=-O1
&endif

# PTS 1133966 mb 2005-02-16
&if $MACH == HP_IA64
Log_Volume.cpp -OPT=-O0
&endif
#
# ------------------------------------------------------------------------------
#
# include dependencies (do not edit)
#
Log_ActionBlobCopy.cpp inc=:SAPDB/Logging/Log_ActionBlobCopy.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00,hbd01.h,hbd05.h
Log_ActionBlobCopy.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionBlobCopySame.cpp inc=:SAPDB/Logging/Log_ActionBlobCopySame.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00,hbd01.h,hbd05.h
Log_ActionBlobCopySame.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionBlobExpand.cpp inc=:SAPDB/Logging/Log_ActionBlobExpand.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00,hbd01.h,hbd05.h
Log_ActionBlobExpand.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionBlobTruncate.cpp inc=:SAPDB/Logging/Log_ActionBlobTruncate.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00,hbd01.h,hbd05.h
Log_ActionBlobTruncate.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionBlobUpdate.cpp inc=:SAPDB/Logging/Log_ActionBlobUpdate.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00,hbd01.h,hbd05.h
Log_ActionBlobUpdate.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionCopyUpdBlob75.cpp inc=\
    :SAPDB/Logging/Log_ActionCopyUpdBlob75.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00,hbd01.h,hbd05.h
Log_ActionCopyUpdBlob75.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionCreateDropFile.cpp inc=\
    :SAPDB/Logging/Log_ActionCreateDropFile.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,:SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Messages/Msg_List.hpp,ggg00,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,ggg91.h,ggg00,hbd01.h,\
    hbd03.h,hkb64.h
Log_ActionCreateDropFile.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg91.h,ggg00
Log_ActionCreateIndex75.cpp inc=\
    :SAPDB/Logging/Log_ActionCreateIndex75.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_ActionCreateDropFile.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Messages/Msg_List.hpp,ggg91.h,ggg00,hbd01.h,hbd03.h,hkb64.h
Log_ActionCreateIndex75.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg91.h,ggg00
Log_ActionCreateIndexFile.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Logging/Log_ActionCreateIndexFile.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,hbd01.h,hbd03.h,ggg00,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Messages/Msg_List.hpp
Log_ActionCreateIndexFile.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionCreateShortColumnFile.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Logging/Log_ActionCreateShortColumnFile.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Messages/Msg_List.hpp,ggg91.h,ggg00,hbd01.h,hbd01_1.h
Log_ActionCreateShortColumnFile.hpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,ggg00
Log_ActionCreateTable.cpp inc=:SAPDB/Logging/Log_ActionCreateTable.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Messages/Msg_List.hpp,ggg00,ggg91.h,ggg00,hbd01.h,hbd01_1.h
Log_ActionCreateTable.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,ggg91.h,ggg00
Log_ActionCreateTable75.cpp inc=\
    :SAPDB/Logging/Log_ActionCreateTable75.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/Logging/Log_ActionCreateDropFile.hpp,\
    :SAPDB/Logging/Log_ActionCreateShortColumnFile.hpp,\
    :SAPDB/Logging/Log_ActionCreateIndexFile.hpp,\
    :SAPDB/Logging/Log_ActionCreateTable.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Messages/Msg_List.hpp,ggg91.h,ggg00
Log_ActionCreateTable75.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg91.h,ggg00
Log_ActionDropFile75.cpp inc=:SAPDB/Logging/Log_ActionDropFile75.hpp,\
    :SAPDB/Logging/Log_ActionCreateDropFile.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,ggg00
Log_ActionDropFile75.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg91.h,ggg00
Log_ActionFile.cpp inc=:SAPDB/Logging/Log_ActionFile.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,hbd01.h,hbd30.h,gbd00,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/Messages/Msg_List.hpp
Log_ActionFile.hpp inc=:SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg91.h,ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Log_ActionFileHandling.hpp inc=ggg00,hbd01_1.h
Log_ActionInsDelRec.cpp inc=:SAPDB/Logging/Log_ActionInsDelRec.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,ggg00,hbd01_1.h,hbd02.h
Log_ActionInsDelRec.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionInsDelRec75.cpp inc=:SAPDB/Logging/Log_ActionInsDelRec75.hpp,\
    :SAPDB/Logging/Log_ActionInsDelRec.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,ggg00,hbd01_1.h,hbd02.h
Log_ActionInsDelRec75.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionModifyTable.cpp inc=:SAPDB/Logging/Log_ActionModifyTable.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,ggg00,hbd01.h,hbd37.h
Log_ActionModifyTable.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionMultiple.cpp inc=:SAPDB/Logging/Log_ActionMultiple.hpp,\
    :SAPDB/Logging/Log_AfterImage.hpp,:SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp
Log_ActionMultiple.hpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_IAction.hpp
Log_ActionObject.cpp inc=:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionObject.hpp,\
    :SAPDB/Logging/Log_AfterImage.hpp,:SAPDB/Logging/Log_BeforeImage.hpp,\
    hbd04.h,hbd95.h
Log_ActionObject.hpp inc=ggg00,ggg92.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/DataAccess/Data_PageSplitSpace.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_IAction.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp
Log_ActionObjectFile.cpp inc=:SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionObjectFile.hpp,\
    :SAPDB/Logging/Log_AfterImage.hpp,:SAPDB/Logging/Log_BeforeImage.hpp,\
    hbd04.h,hbd95.h,hbd91.h
Log_ActionObjectFile.hpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_IAction.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/liveCache/LVC_Types.hpp,:SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp
Log_ActionPartialRollback.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_IAction.hpp
Log_ActionReassignSecondaryFile.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ActionReassignSecondaryFile.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Logging/Log_ActionReassignSecondaryFile.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,:SAPDB/Messages/Msg_List.hpp
Log_ActionReassignSecondaryFile.hpp inc=ggg00,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/FileDirectory/FileDir_Types.hpp
Log_ActionRefreshSurrogSeq.cpp inc=\
    :SAPDB/Logging/Log_ActionRefreshSurrogSeq.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,gsp00,ggg00,hkb57.h
Log_ActionRefreshSurrogSeq.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,gsp00,ggg00
Log_ActionRenameFile75.cpp inc=:SAPDB/Logging/Log_ActionRenameFile75.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/Logging/Log_ActionReassignSecondaryFile.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Messages/Msg_List.hpp,ggg00,ggg91.h,ggg00,hbd01.h,hbd03.h,\
    hkb64.h,hbd17.h,hbd998.h
Log_ActionRenameFile75.hpp inc=:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg91.h,ggg00
Log_ActionReorganizeFile.cpp inc=:SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/Logging/Log_ActionReorganizeFile.hpp,\
    :SAPDB/Logging/Log_ActionFileHandling.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,ggg00,hbd01.h,\
    hbd09.h
Log_ActionReorganizeFile.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_ActionSavepoint.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp,\
    :SAPDB/Logging/Log_IAction.hpp,hgg11.h
Log_ActionSetFileAttributes.cpp inc=\
    :SAPDB/Logging/Log_ActionSetFileAttributes.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_Messages.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
Log_ActionSetFileAttributes.hpp inc=\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    ggg00
Log_ActionSetFileDirCounter.cpp inc=\
    :SAPDB/Logging/Log_ActionSetFileDirCounter.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_Messages.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
Log_ActionSetFileDirCounter.hpp inc=\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    ggg00
Log_ActionUndoRedo.cpp inc=:SAPDB/Logging/Log_ActionUndoRedo.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_AfterImage.hpp,\
    :SAPDB/Logging/Log_BeforeImage.hpp,\
    :SAPDB/Logging/Log_ActionCopyUpdBlob75.hpp,\
    :SAPDB/Logging/Log_ActionBlobCopy.hpp,\
    :SAPDB/Logging/Log_ActionBlobCopySame.hpp,\
    :SAPDB/Logging/Log_ActionBlobExpand.hpp,\
    :SAPDB/Logging/Log_ActionBlobTruncate.hpp,\
    :SAPDB/Logging/Log_ActionBlobUpdate.hpp,\
    :SAPDB/Logging/Log_ActionCreateTable.hpp,\
    :SAPDB/Logging/Log_ActionCreateDropFile.hpp,\
    :SAPDB/Logging/Log_ActionDropFile75.hpp,\
    :SAPDB/Logging/Log_ActionCreateTable75.hpp,\
    :SAPDB/Logging/Log_ActionFile.hpp,\
    :SAPDB/Logging/Log_ActionCreateIndexFile.hpp,\
    :SAPDB/Logging/Log_ActionCreateIndex75.hpp,\
    :SAPDB/Logging/Log_ActionCreateShortColumnFile.hpp,\
    :SAPDB/Logging/Log_ActionInsDelRec.hpp,\
    :SAPDB/Logging/Log_ActionInsDelRec75.hpp,\
    :SAPDB/Logging/Log_ActionModifyTable.hpp,\
    :SAPDB/Logging/Log_ActionObject.hpp,\
    :SAPDB/Logging/Log_ActionObjectFile.hpp,\
    :SAPDB/Logging/Log_ActionRefreshSurrogSeq.hpp,\
    :SAPDB/Logging/Log_ActionRenameFile75.hpp,\
    :SAPDB/Logging/Log_ActionReorganizeFile.hpp,\
    :SAPDB/Logging/Log_ActionReassignSecondaryFile.hpp,\
    :SAPDB/Logging/Log_ActionSetFileAttributes.hpp,\
    :SAPDB/Logging/Log_ActionSetFileDirCounter.hpp,\
    :SAPDB/Logging/Log_ActionUpdateRec.hpp,\
    :SAPDB/Logging/Log_ActionUpdateRec75.hpp,\
    :SAPDB/Logging/Log_ActionUnqualifiedDelete.hpp,\
    :SAPDB/Logging/Log_ActionPartialRollback.hpp,\
    :SAPDB/Logging/Log_ActionSavepoint.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp,\
    :SAPDB/Restart/Rst_RedoManager.hpp,\
    :SAPDB/Restart/Rst_IRedoTrafficControl.hpp
Log_ActionUndoRedo.hpp inc=:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_UndoFile.hpp,:SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/Restart/Rst_RedoFile.hpp
Log_ActionUnqualifiedDelete.cpp inc=\
    :SAPDB/Logging/Log_ActionUnqualifiedDelete.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Cache.hpp,\
    :SAPDB/FileDirectory/FileDir_Iterator.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_Generic.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,hbd01.h,hbd30.h,hbd20_9.h,\
    hbd13.h,gbd00,:SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,:SAPDB/Messages/Msg_List.hpp
Log_ActionUnqualifiedDelete.hpp inc=\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/DataAccess/Data_FileTypes.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Log_ActionUpdateRec.cpp inc=:SAPDB/Logging/Log_ActionUpdateRec.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_RecordColumnMap.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,ggg00,hbd01_1.h,hbd02.h
Log_ActionUpdateRec.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_RecordColumnMap.hpp,ggg00
Log_ActionUpdateRec75.cpp inc=:SAPDB/Logging/Log_ActionUpdateRec75.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_RecordColumnMap.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/FileDirectory/FileDir_ShortColumn.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,ggg00,hbd01_1.h,hbd02.h
Log_ActionUpdateRec75.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_RecordColumnMap.hpp,ggg00
Log_AfterImage.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceWriter.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceReader.hpp,\
    :SAPDB/Logging/Log_AfterImageSpace.hpp,:SAPDB/Logging/Log_IImage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Log.h,ggg91.h
Log_AfterImageSpace.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp,\
    :SAPDB/Logging/Log_QueueEntry.hpp
Log_BeforeImage.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/DataAccess/Data_SplitSpace.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceWriter.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceReader.hpp,\
    :SAPDB/DataAccess/Data_PageSplitSpace.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_IImage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Log.h,ggg91.h
Log_ClusterAddress.hpp inc=:SAPDB/IOManager/IOMan_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp
Log_CommandInsDelUpd.cpp inc=\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_CommandInsDelUpd.hpp,\
    :SAPDB/Logging/Log_ActionUpdateRec.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,ggg00,hbd02.h
Log_CommandInsDelUpd.hpp inc=\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_ActionInsDelRec.hpp,\
    :SAPDB/Logging/Log_ActionUpdateRec.hpp,\
    :SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_Exceptions.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/Logging/Log_Messages.hpp
Log_History.cpp inc=heo55k.h,heo56.h,heo58.h,hgg01.h,ggg92.h,hkb51.h,\
    hkb57_1.h,hbd02.h,hbd20_11.h,hbd91.h,hkb900.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceReader.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/Logging/Log_ActionObject.hpp,\
    :SAPDB/Logging/Log_ActionCreateDropFile.hpp,\
    :SAPDB/Logging/Log_ActionFile.hpp,\
    :SAPDB/Logging/Log_ActionObjectFile.hpp,\
    :SAPDB/Logging/Log_ActionInsDelRec.hpp,\
    :SAPDB/Logging/Log_ActionUpdateRec.hpp,\
    :SAPDB/Logging/Log_ActionUnqualifiedDelete.hpp,\
    :SAPDB/Logging/Log_UndoFile.hpp,:SAPDB/Logging/Log_BeforeImage.hpp,\
    :SAPDB/Logging/Log_History.hpp,\
    :SAPDB/GarbageCollector/GC_IGCController.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp
Log_History.hpp inc=gsp00,ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/Logging/Log_HistoryFile.hpp,\
    :SAPDB/Logging/Log_HistoryDirectory.hpp,\
    :SAPDB/GarbageCollector/GC_IGarbageCollector.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp
Log_HistoryDirectory.cpp inc=ggg00,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/Logging/Log_HistoryFile.hpp,\
    :SAPDB/Logging/Log_HistoryDirectory.hpp
Log_HistoryDirectory.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ChainFixSizeSpace.hpp,\
    :SAPDB/DataAccess/Data_BaseFile.hpp
Log_HistoryFile.cpp inc=ggg00,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_UndoFile.hpp,\
    :SAPDB/Logging/Log_HistoryFile.hpp
Log_HistoryFile.hpp inc=ggg91.h,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ChainFixSizeSpace.hpp,\
    :SAPDB/DataAccess/Data_BaseFile.hpp,:SAPDB/Logging/Log_Types.hpp
Log_IAction.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Log.h,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    ggg00
Log_IDiagnose.hpp inc=:SAPDB/Logging/Log_Page.hpp
Log_IImage.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp
Log_IRecordColumnMap.hpp inc=ggg00
Log_ISequentialReader.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_RawDeviceIterator.hpp
Log_InfoPage.hpp inc=gkb00,hgg08.h,heo55k.h,:SAPDB/RunTime/RTE_Types.h,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/KernelCommon/Kernel_Page74.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/Logging/Log_Types.hpp
Log_InvDescMap.cpp inc=:SAPDB/Logging/Log_InvDescMap.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IncrementalBufferAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_IRecordColumnMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SQLManager/DBProc/DBProc_Handler.hpp,\
    :SAPDB/ConsistentRead/CnsRead_CachedRow.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/ConsistentRead/CnsRead_Manager.hpp,gsp00,ggg00,ggg01,ggg04,\
    hbd03.h,hbd06.h,hgg04.h,hkb53.h,hkb62.h,hsp30.h,heo58.h
Log_InvDescMap.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Logging/Log_IRecordColumnMap.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
Log_OpenTransFile.cpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/Logging/Log_UndoFile.hpp,:SAPDB/Restart/Rst_RedoFile.hpp,\
    :SAPDB/Logging/Log_OpenTransFile.hpp
Log_OpenTransFile.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_ChainFixSizeSpace.hpp,\
    :SAPDB/DataAccess/Data_BaseFile.hpp
Log_Page.cpp inc=:SAPDB/Logging/Log_Page.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Log.h
Log_Page.hpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_Page74.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/Logging/Log_Types.hpp
Log_PageTest.cpp inc=:SAPDB/Logging/Log_Page.hpp,\
    :SAPDB/Logging/Log_InfoPage.hpp
Log_Queue.cpp inc=heo56.h,:SAPDB/Logging/Log_Queue.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_Savepoint.hpp,\
    :SAPDB/Logging/Log_Volume.hpp
Log_Queue.hpp inc=gsp00,heo55k.h,ggg04,hgg08.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_TaskWaitList.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_RawDeviceIterator.hpp,\
    :SAPDB/Logging/Log_WriterTaskSynchronizer.hpp,\
    :SAPDB/Logging/Log_QueueRingBuffer.hpp,\
    :SAPDB/Logging/Log_AfterImageSpace.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp,\
    :SAPDB/RunTime/System/RTESys_MemoryBarrier.h,\
    :SAPDB/IOManager/IOMan_LogPages.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h
Log_QueueEntry.hpp inc=gsp00,\
    :SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_TaskWaitList.hpp,\
    :SAPDB/Container/Container_Vector.hpp,:SAPDB/Logging/Log_Page.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp
Log_QueueRingBuffer.cpp inc=:SAPDB/Logging/Log_QueueRingBuffer.hpp
Log_QueueRingBuffer.hpp inc=:SAPDB/Logging/Log_QueueEntry.hpp,\
    :SAPDB/Container/Container_CircularList.hpp,\
    :SAPDB/IOManager/IOMan_LogPages.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,hgg01_3.h,\
    :SAPDB/RunTime/RTE_ISystem.hpp
Log_QueueStatistics.cpp inc=:SAPDB/Logging/Log_QueueStatistics.hpp,\
    :SAPDB/Logging/Log_Volume.hpp,:SAPDB/Logging/Log_Queue.hpp
Log_QueueStatistics.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Logging/Log_Types.hpp
Log_QueueTest.cpp inc=:SAPDB/Logging/Log_Queue.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp
Log_RawDeviceIterator.hpp inc=:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/Logging/Log_Types.hpp
Log_ReadWriteActionImage.cpp inc=\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_BeforeImage.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,hkb560.h,ggg00
Log_ReadWriteActionImage.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceWriter.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Logging/Log_IAction.hpp,ggg00
Log_RecordColumnMap.cpp inc=:SAPDB/Logging/Log_RecordColumnMap.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,hgg04.h,gsp00,ggg00,hsp30.h
Log_RecordColumnMap.hpp inc=:SAPDB/Logging/Log_IRecordColumnMap.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ReadWriteActionImage.hpp,ggg00
Log_SaveIterator.cpp inc=:SAPDB/Logging/Log_SaveIterator.hpp,\
    :SAPDB/Logging/Log_VolumeIterator.hpp,\
    :SAPDB/Logging/Log_ClusterAddress.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,hgg01.h
Log_SaveIterator.hpp inc=gsp00,gkb03,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/IOManager/IOMan_ILogIO.hpp,\
    :SAPDB/IOManager/IOMan_LogPages.hpp,\
    :SAPDB/KernelCommon/Kernel_Page.hpp,:SAPDB/Logging/Log_Page.hpp,\
    :SAPDB/Logging/Log_RawDeviceIterator.hpp,\
    :SAPDB/Logging/Log_QueueRingBuffer.hpp
Log_Savepoint.cpp inc=heo52.h,heo55k.h,ggg92.h,hgg01.h,hgg08.h,hgg11.h,\
    hkb38_1.h,hkb50.h,hkb51.h,hbd01.h,hbd20_4.h,hbd91.h,gkb00,hkb57.h,\
    hkb57_1.h,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_OpMsg.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/Converter/Converter_IBackUp.hpp,\
    :SAPDB/Converter/Converter_ICommon.hpp,\
    :SAPDB/Converter/Converter_ISavePoint.hpp,\
    :SAPDB/IOManager/IOMan_IDataManager.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_AfterImage.hpp,\
    :SAPDB/Logging/Log_ActionSavepoint.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,\
    :SAPDB/Logging/Log_OpenTransFile.hpp,:SAPDB/Logging/Log_History.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,:SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/FileDirectory/FileDir_ISharedDirectory.hpp,\
    :SAPDB/FreeBlockManagement/FBM_IManager.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/Pager/Pager_ISavepoint.hpp,\
    :SAPDB/ServerTasks/SrvTasks_ICoordinator.hpp,\
    :SAPDB/ServerTasks/SrvTasks_JobSavepoint.hpp,\
    :SAPDB/Restart/Rst_RedoManager.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Log.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SrvTasks.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Savepoint.h
Log_Savepoint.hpp inc=gsp00,ggg00,gkb00,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_TaskWaitList.hpp,\
    :SAPDB/Logging/Log_Types.hpp,:SAPDB/Converter/Converter_Version.hpp,\
    :SAPDB/IOManager/IOMan_BlockAddress.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/ServerTasks/SrvTasks_Types.hpp
Log_SeqLogDevReader.hpp inc=:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_RawDeviceIterator.hpp,\
    :SAPDB/Logging/Log_ClusterAddress.hpp,\
    :SAPDB/IOManager/IOMan_ILogIO.hpp,\
    :SAPDB/IOManager/IOMan_ILogInfo.hpp,\
    :SAPDB/Logging/Log_ISequentialReader.hpp,\
    :SAPDB/Restart/Rst_StandbySynchronizer.hpp
Log_SeqTapeReader.hpp inc=:SAPDB/Logging/Log_ISequentialReader.hpp,\
    :SAPDB/FrameControl/FrameCtrl_ICommon.hpp,hkb39.h
Log_Statistics.cpp inc=:SAPDB/Logging/Log_Statistics.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/RunTime/System/RTESys_MicroTime.h,hkb57.h,hkb51.h
Log_Statistics.hpp inc=:SAPDB/Logging/Log_Types.hpp
Log_Test.cpp inc=ggg00,:SAPDB/Logging/Log_Test.hpp,\
    :SAPDB/Logging/Log_ActionUndoRedo.hpp,\
    :SAPDB/Logging/Log_Statistics.hpp,\
    :SAPDB/Logging/Log_QueueStatistics.hpp
Log_Test.hpp inc=ggg00,:SAPDB/Logging/Log_Types.hpp
Log_Transaction.cpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp,\
    :SAPDB/DataAccess/Data_Exceptions.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_Transaction.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_UndoFile.hpp,:SAPDB/Restart/Rst_RedoFile.hpp,\
    :SAPDB/Logging/Log_AfterImage.hpp,:SAPDB/Logging/Log_BeforeImage.hpp,\
    :SAPDB/Logging/Log_IAction.hpp,:SAPDB/Logging/Log_ActionUndoRedo.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_History.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,\
    :SAPDB/GarbageCollector/GC_IGCController.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,hkb50.h,\
    hkb54.h,hkb57.h,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_Log.h
Log_Transaction.hpp inc=ggg00,ggg92.h,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceWriter.hpp,\
    :SAPDB/DataAccess/Data_SplitSpaceReader.hpp,\
    :SAPDB/DataAccess/Data_ISplitSpace.hpp,\
    :SAPDB/Container/Container_Stack.hpp,:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_UndoFile.hpp,\
    :SAPDB/Logging/Log_ActionMultiple.hpp,\
    :SAPDB/Restart/Rst_RedoFile.hpp
Log_Types.cpp inc=:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp
Log_Types-diagnose.cpp inc=:SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp
Log_Types.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_EnhancedTypes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeIterator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_OStream.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp
Log_UndoFile.cpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_TraceTopics.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/DataAccess/Data_Types.hpp,:SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/Logging/Log_BeforeImage.hpp,:SAPDB/Logging/Log_UndoFile.hpp
Log_UndoFile.hpp inc=ggg00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DataAccess/Data_PageAccessManager.hpp,\
    :SAPDB/DataAccess/Data_ChainSplitSpaceBackwardRead.hpp,\
    :SAPDB/DataAccess/Data_BaseFile.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,:SAPDB/Logging/Log_IAction.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp
Log_Volume.cpp inc=:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_Statistics.hpp,heo47.h,hkb38.h,hkb57_1.h,\
    hgg01_3.h,:SAPDB/SAPDBCommon/Messages/SDBMsg_Log.h,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/KernelCommon/Kernel_ExtendedState.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminDiagnose.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_Version.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_DataAccess.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    :SAPDB/Logging/Log_SeqTapeReader.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,\
    :SAPDB/IOManager/IOMan_ILogManager.hpp,\
    :SAPDB/IOManager/IOMan_ILogInfo.hpp
Log_Volume.hpp inc=gsp00,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_Event.hpp,\
    :SAPDB/Logging/Log_IDiagnose.hpp,:SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/Logging/Log_Queue.hpp,:SAPDB/Logging/Log_Writer.hpp,\
    :SAPDB/Logging/Log_RawDeviceIterator.hpp,\
    :SAPDB/Logging/Log_InfoPage.hpp,\
    :SAPDB/Logging/Log_VolumeIterator.hpp,\
    :SAPDB/Logging/Log_SeqLogDevReader.hpp,\
    :SAPDB/Logging/Log_SaveIterator.hpp,\
    :SAPDB/IOManager/IOMan_ILogIO.hpp
Log_VolumeIterator.cpp inc=:SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminRestartShutdown.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminHotStandby.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/Logging/Log_VolumeIterator.hpp,\
    :SAPDB/Logging/Log_Exceptions.hpp,\
    :SAPDB/Restart/Rst_StandbySynchronizer.hpp,\
    :SAPDB/RunTime/RTE_Crash.hpp,:SAPDB/RunTime/RTE_Message.hpp
Log_VolumeIterator.hpp inc=gsp00,:SAPDB/IOManager/IOMan_ILogIO.hpp,\
    :SAPDB/Logging/Log_Types.hpp,\
    :SAPDB/Logging/Log_ISequentialReader.hpp
Log_Writer.cpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminInfo.hpp,\
    :SAPDB/KernelCommon/Kernel_DateTime.hpp,\
    :SAPDB/KernelCommon/Kernel_ExtendedState.hpp,\
    :SAPDB/Logging/Log_Writer.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    :SAPDB/Logging/Log_Savepoint.hpp,:SAPDB/Logging/Log_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,:SAPDB/Transaction/Trans_Context.hpp,\
    hkb38_1.h,hkb57_1.h,hgg01_3.h,heo52.h,heo60.h,vsp0031,hgg01_2.h
Log_Writer.hpp inc=:SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/Logging/Log_Queue.hpp,\
    :SAPDB/Logging/Log_RawDeviceIterator.hpp,\
    :SAPDB/Logging/Log_InfoPage.hpp,\
    :SAPDB/Logging/Log_WriterTaskSynchronizer.hpp,\
    :SAPDB/IOManager/IOMan_ILogIO.hpp,\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_Event.hpp,hgg01.h
Log_WriterTaskSynchronizer.hpp inc=gsp00,ggg00,heo56.h,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_ITask.hpp,\
    :SAPDB/RunTime/Tasking/RTETask_SuspendReason.h

