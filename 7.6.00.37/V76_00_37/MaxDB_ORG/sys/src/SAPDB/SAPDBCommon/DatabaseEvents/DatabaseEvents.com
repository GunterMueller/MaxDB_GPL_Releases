
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

SAPDBDBE_EventCategory.cpp inc=\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SAPDBMem.h,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
SAPDBDBE_EventCategory.hpp inc=\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategoryIteratorNode.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventDefinition.hpp
SAPDBDBE_EventCategoryIteratorNode.hpp inc=\
    :SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventDefinition.hpp
SAPDBDBE_EventDefinition.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    vsp0031
SAPDBDBE_KernelParameterNames.hpp inc=

