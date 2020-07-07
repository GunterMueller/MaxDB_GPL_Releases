# ------------------------------------------------------------------
# contents: 	Dependencies of files in 
# 		        component 'SAPDB/SQLManager/Query/'.
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
Query_Column.cpp
Query_Column.hpp
Query_Graph.cpp
Query_Graph.hpp
Query_ManipulationContext.cpp
Query_ManipulationContext.hpp
Query_Node.cpp
Query_Node.hpp
Query_NodeCursor.cpp
Query_NodeCursor.hpp
Query_NodeDelete.cpp
Query_NodeDelete.hpp
Query_NodeDifference.cpp
Query_NodeDifference.hpp
Query_NodeInsert.cpp
Query_NodeInsert.hpp
Query_NodeIntersect.cpp
Query_NodeIntersect.hpp
Query_NodeRoot.cpp
Query_NodeRoot.hpp
Query_NodeSelect.cpp
Query_NodeSelect.hpp
Query_NodeSet.cpp
Query_NodeSet.hpp
Query_NodeTable.cpp
Query_NodeTable.hpp
Query_NodeUnion.cpp
Query_NodeUnion.hpp
Query_NodeUpdate.cpp
Query_NodeUpdate.hpp
Query_Quantifier.cpp
Query_Quantifier.hpp
Query_SyntaxTree.cpp
Query_SyntaxTree.hpp
Query_Types.hpp

#
# include dependencies (do not edit)
#
Query_Column.cpp inc=:SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp
Query_Column.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,\
    :SAPDB/SQLManager/Query/Query_ExpressionNode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Query_ColumnSpec.cpp inc=:SAPDB/SQLManager/Query/Query_OutputColumn.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp
Query_ColumnSpec.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_ExpressionNode.hpp,\
    :SAPDB/SQLManager/Query/Query_Expression.hpp,\
    :SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Query_Expression.cpp inc=:SAPDB/SQLManager/Query/Query_Expression.hpp,\
    :SAPDB/SQLManager/Query/Query_ExpressionNode.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_OutputColumn.hpp,\
    :SAPDB/RunTime/RTE_CurrentStack.hpp
Query_Expression.hpp inc=:SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_ExpressionNode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Query_ExpressionNode.cpp inc=\
    :SAPDB/SQLManager/Query/Query_ExpressionNode.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/SQLManager/Query/Query_Operator.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp
Query_ExpressionNode.hpp inc=:SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Query_Graph.cpp inc=:SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Node.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeRoot.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeTable.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeCursor.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeUnion.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeIntersect.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeDifference.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ViewDescriptor.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rewrite.hpp,\
    :SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp,\
    :SAPDB/RunTime/RTE_CurrentStack.hpp,hak05.h,hak06.h,hak35.h,hak542.h,\
    hak61.h
Query_Graph.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_ManipulationContext.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,\
    :SAPDB/SQLManager/Query/Query_Node.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeRoot.hpp,\
    :SAPDB/SQLManager/Query/Query_OutputColumn.hpp,\
    :SAPDB/SQLManager/Query/Query_ColumnSpec.hpp,\
    :SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/SQLManager/Query/Query_Operator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hak07.h
Query_ManipulationContext.cpp inc=\
    :SAPDB/SQLManager/Query/Query_ManipulationContext.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,hak542.h,hak99.h,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp
Query_ManipulationContext.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,hak01.h,hgg01_3.h,hsp30.h
Query_Node.cpp inc=:SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp,\
    :SAPDB/SQLManager/Query/Query_Node.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeUnion.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeIntersect.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeDifference.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeTable.hpp
Query_Node.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_OutputColumn.hpp,\
    :SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp
Query_NodeCursor.cpp inc=:SAPDB/SQLManager/Query/Query_NodeCursor.hpp
Query_NodeCursor.hpp inc=
Query_NodeDelete.cpp inc=:SAPDB/SQLManager/Query/Query_NodeDelete.hpp
Query_NodeDelete.hpp inc=:SAPDB/SQLManager/Query/Query_Node.hpp
Query_NodeDifference.cpp inc=\
    :SAPDB/SQLManager/Query/Query_NodeDifference.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp
Query_NodeDifference.hpp inc=:SAPDB/SQLManager/Query/Query_NodeSet.hpp
Query_NodeInsert.cpp inc=:SAPDB/SQLManager/Query/Query_NodeInsert.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp
Query_NodeInsert.hpp inc=:SAPDB/SQLManager/Query/Query_Node.hpp
Query_NodeIntersect.cpp inc=\
    :SAPDB/SQLManager/Query/Query_NodeIntersect.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp
Query_NodeIntersect.hpp inc=:SAPDB/SQLManager/Query/Query_NodeSet.hpp
Query_NodeRoot.cpp inc=:SAPDB/SQLManager/Query/Query_NodeRoot.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeInsert.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeUpdate.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeDelete.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp
Query_NodeRoot.hpp inc=:SAPDB/SQLManager/Query/Query_Node.hpp
Query_NodeSelect.cpp inc=:SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSet.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeTable.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp,\
    :SAPDB/SQLManager/Query/Query_OutputColumn.hpp,\
    :SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/RunTime/RTE_CurrentStack.hpp
Query_NodeSelect.hpp inc=:SAPDB/SQLManager/Query/Query_Node.hpp,\
    :SAPDB/SQLManager/Query/Query_Expression.hpp
Query_NodeSet.cpp inc=:SAPDB/SQLManager/Query/Query_NodeSet.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Expression.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp
Query_NodeSet.hpp inc=:SAPDB/SQLManager/Query/Query_Node.hpp
Query_NodeTable.cpp inc=:SAPDB/SQLManager/Query/Query_NodeTable.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp,\
    :SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp
Query_NodeTable.hpp inc=:SAPDB/SQLManager/Query/Query_Node.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp
Query_NodeUnion.cpp inc=:SAPDB/SQLManager/Query/Query_NodeUnion.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp
Query_NodeUnion.hpp inc=:SAPDB/SQLManager/Query/Query_NodeSet.hpp
Query_NodeUpdate.cpp inc=:SAPDB/SQLManager/Query/Query_NodeUpdate.hpp
Query_NodeUpdate.hpp inc=:SAPDB/SQLManager/Query/Query_Node.hpp
Query_Operator.cpp inc=:SAPDB/SQLManager/Query/Query_Operator.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp
Query_Operator.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,\
    :SAPDB/SQLManager/Query/Query_ExpressionNode.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Query_OutputColumn.cpp inc=\
    :SAPDB/SQLManager/Query/Query_OutputColumn.hpp,\
    :SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,\
    :SAPDB/SQLManager/Query/Query_Graph.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/SQLManager/Query/Query_Quantifier.hpp
Query_OutputColumn.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_ExpressionNode.hpp,\
    :SAPDB/SQLManager/Query/Query_Expression.hpp,\
    :SAPDB/SQLManager/Query/Query_Column.hpp,\
    :SAPDB/SQLManager/Query/Query_ColumnSpec.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
Query_Quantifier.cpp inc=:SAPDB/SQLManager/Query/Query_Quantifier.hpp,\
    :SAPDB/SQLManager/Query/Query_Node.hpp
Query_Quantifier.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Node.hpp,\
    :SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp
Query_SyntaxTree.cpp inc=:SAPDB/SQLManager/Query/Query_SyntaxTree.hpp,\
    :SAPDB/SQLManager/Query/Query_NodeSelect.hpp,\
    :SAPDB/RunTime/RTE_CurrentStack.hpp,hak01.h,hak99.h
Query_SyntaxTree.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SQLManager/Query/Query_Types.hpp,\
    :SAPDB/Container/Container_Vector.hpp,vak001
Query_Types.hpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp,\
    :SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp,\
    :SAPDB/KernelCommon/Kernel_VTrace.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_OStream.hpp,vak001

