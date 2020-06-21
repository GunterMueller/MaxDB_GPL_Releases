# Tracing.com
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

# include dependencies (do not edit)
#
SAPDBTrace_FileTrace.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_FileTrace.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp
SAPDBTrace_FileTrace.hpp inc=
SAPDBTrace_Hex.cpp inc=:SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_OStream.hpp
SAPDBTrace_Hex.hpp inc=
SAPDBTrace_HexBuf.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_OStream.hpp
SAPDBTrace_HexBuf.hpp inc=
SAPDBTrace_MethodBrace.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_MethodBrace.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/RunTime/RTE_CurrentStack.hpp
SAPDBTrace_MethodBrace.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.h
SAPDBTrace_Stream.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp
SAPDBTrace_Stream.hpp inc=:SAPDB/SAPDBCommon/SAPDB_OStream.hpp
SAPDBTrace_Streaming-kernel.cpp -DKERNEL inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,ggg01,hgg01_3.h
SAPDBTrace_Streaming.cpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,ggg01
SAPDBTrace_Streaming.hpp inc=gsp00,ggg00,ggg04,ggg07,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_OStream.hpp
SAPDBTrace_Topic.cpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp
SAPDBTrace_Topic.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Diagnostics/SAPDBDiag_Topic.hpp
SAPDBTrace_Usage.hpp inc=\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_MethodBrace.hpp

