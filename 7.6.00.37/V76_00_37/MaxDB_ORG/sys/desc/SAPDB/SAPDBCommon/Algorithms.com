# --------------------------------------------------------------------------
# contents: Dependencies of files in component 'SAPDB/SAPDBCommon/Algorithms/'.
# to do:    Reduce the dependencies by using forward declarations!
# author:   Mirko.Luedde@SAP.Com
# created:  Wed Jan  5 17:04:41  2000
# updated:  Mon Feb 28 16:22:18  2000
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
SAPDBAlgo_Comparator.hpp inc=
SAPDBAlgo_FNVHash.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
SAPDBAlgo_FunnelMerge.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SAPDBAlgo.h,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp
SAPDBAlgo_MD5File.c inc=:SAPDB/RunTime/System/RTESys_IO.h
SAPDBAlgo_MD5File.h inc=
SAPDBAlgo_QuickSort.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp
SAPDBAlgo_Test.cpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_Test.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SAPDBAlgo.h,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_FunnelMerge.hpp,\
    :SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,:SAPDB/RunTime/RTE_Message.hpp,\
    ggg00,ggg200.h,gsp00
SAPDBAlgo_Test.hpp inc=

