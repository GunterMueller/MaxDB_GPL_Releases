# ------------------------------------------------------------------
# contents: 	Dependencies of files in 
# 		        component 'SAPDB/SQLManager/Optimizer/'.
# author:	Dirk Thomsen
# created:	Fri Jan 17 15:27:12  2003
# updated:	Fri Jan 17 17:21:07  2003
# to do:	Reduce the dependencies by using forward declarations!
# ------------------------------------------------------------------
#
#
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

# include dependencies (do not edit)
#
QueryRewrite_Rewrite.cpp
QueryRewrite_Rewrite.hpp
QueryRewrite_Rules.cpp
QueryRewrite_Rules.hpp
QueryRewrite_RuleList.cpp
QueryRewrite_RuleList.hpp

#
# include dependencies (do not edit)
#
QueryRewrite_PascalWrapper.cpp inc=hak664wrapper.h,hak101.h,hak542.h,\
    hak40.h,hak42.h,:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeRoot.hpp,\
    :SAPDB/SQLManager/Query/Query_ManipulationContext.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rewrite.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp
QueryRewrite_Rewrite.cpp inc=\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rewrite.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeRoot.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp
QueryRewrite_Rewrite.hpp inc=\
    :SAPDB/SQLManager/Query/Query_ManipulationContext.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
QueryRewrite_RuleList.cpp inc=\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp
QueryRewrite_RuleList.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RawAllocator.hpp,\
    :SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp,\
    :SAPDB/Container/Container_Vector.hpp
QueryRewrite_Rules.cpp inc=\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp,\
    :SAPDB/SQLManager/Query/Query_Node.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSet.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp
QueryRewrite_Rules.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.hpp

