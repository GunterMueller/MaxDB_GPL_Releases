# --------------------------------------------------------------------------
# description: SQLManagerErrorHandling
#
# author:	ElkeZ
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

# include dependencies (do not edit)
#
SQLManErr_BaseToReturnCode.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.h,\
    gsp00
SQLManErr_Instance.cpp inc=\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Instance.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_BaseToReturnCode.hpp,\
    :SAPDB/Container/Container_SortableVector.hpp,ggg00,ggg01,hak101.h
SQLManErr_Instance.hpp inc=\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_BaseToReturnCode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Singleton.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RawAllocator.hpp,\
    :SAPDB/Container/Container_SortableVector.hpp,gsp00,ggg00
SQLManErr_Interface.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.h,gsp00

