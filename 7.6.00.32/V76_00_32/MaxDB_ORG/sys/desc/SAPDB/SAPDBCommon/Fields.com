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

# Compile all modules with exception handling enabled
regex(.*\.cpp$)    -except
#
#-------------------------------------------------------------------------------
# Fields and field sequences
#
SAPDBFields_Field-diagnose.cpp  -DINDIAGNOSE=1

#
# include dependencies (do not edit)
#
SAPDBFields_AttributedField.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp
SAPDBFields_AttributedField.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp
SAPDBFields_Exceptions.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Messages.hpp
SAPDBFields_Field.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp
SAPDBFields_Field-diagnose.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/KernelCommon/Kernel_Trace.hpp
SAPDBFields_Field.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp
SAPDBFields_FieldCollection.hpp inc=\
    :SAPDB/Container/Container_Vector.hpp
SAPDBFields_FieldList.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldList.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp
SAPDBFields_FieldList.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp
SAPDBFields_FieldListOffsetList.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldListOffsetList.hpp
SAPDBFields_FieldListOffsetList.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp
SAPDBFields_FieldMap.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Exceptions.hpp
SAPDBFields_FieldMap.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp,\
    :SAPDB/Container/Container_Vector.hpp
SAPDBFields_IFieldSequences.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp
SAPDBFields_IncrementalMemorySequence.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp
SAPDBFields_IncrementalMemorySequence.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp
SAPDBFields_MemorySequence.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,hgg01_1.h,gsp03_1,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp
SAPDBFields_MemorySequence.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Field.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp
SAPDBFields_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp
SAPDBFields_Persistence.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_Persistence.hpp,gsp00,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_IPersistenceStream.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_SequenceMemoryCopy.hpp
SAPDBFields_Persistence.hpp inc=:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp
SAPDBFields_PropertyList.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_PropertyList.hpp
SAPDBFields_PropertyList.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_VarData.hpp
SAPDBFields_SequenceMemoryCopy.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_SequenceMemoryCopy.hpp,gsp00,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_IPersistenceStream.hpp
SAPDBFields_SequenceMemoryCopy.hpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_IPersistenceStream.hpp,\
    :SAPDB/Messages/Msg_List.hpp
SAPDBFields_VarData.cpp inc=\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_VarData.hpp,vsp001
SAPDBFields_VarData.hpp inc=

