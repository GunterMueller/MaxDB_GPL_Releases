/*!***************************************************************************

    @file     Query_NodeDifference.cpp
    @ingroup  Query
    @author   DirkT
    @brief    
    @see  

  -------------------------------------------------------------------------

  responsible : DirkT

  special area: SQL Manager / Query
  description : 

  last changed: 2005-06-08  12:00
  first created:2004-04-29  10:00

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




*****************************************************************************/

#include "SAPDB/SQLManager/Query/Query_NodeDifference.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"

// ------------------------------------------------------------------------------------------------

Query_DifferenceNode::Query_DifferenceNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode, bool All) 
    : Query_SetNode(Context, QueryGraph, Position, APNode, All)
{
}

Query_DifferenceNode::Query_DifferenceNode( const Query_DifferenceNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : Query_SetNode(N, Father, Q, mpMap, mpQMap) 
{
}

Query_Node* Query_DifferenceNode::CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    return new(mAllocator) Query_DifferenceNode(*this, Father, Q, mpMap, mpQMap);
}

void Query_DifferenceNode::PrintToTrace( Kernel_VTrace& VTrace )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_DifferenceNode::PrintToTrace", QueryRewrite_Trace, 8);

    Query_Node::PrintToTrace(VTrace);
    if ( QueryRewrite_Trace.TracesLevel(2) )
    {
        if ( mAll )
            VTrace << "NodeType  : Difference All" << FlushLine;
        else
            VTrace << "NodeType  : Difference" << FlushLine;
    }
    if ( QueryRewrite_Trace.TracesLevel(1) && !QueryRewrite_Trace.TracesLevel(2) )
    {
        Query_Quantifier* Q1 = mQuantifier[0];
        Query_Quantifier* Q2 = mQuantifier[1];
        VTrace << "[" << mID << "] <" << Q1->NodeID() 
               << "> DIFFERENCE " << (mAll?"ALL <":"<") << Q2->NodeID() << ">" << FlushLine; 
    }
    PrintChildNodesToTrace(VTrace);
}

// ------------------------------------------------------------------------------------------------
