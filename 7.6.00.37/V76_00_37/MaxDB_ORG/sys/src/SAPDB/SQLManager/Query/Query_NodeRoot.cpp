/*!***************************************************************************

    @file     Query_NodeRoot.cpp
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

#include "SAPDB/SQLManager/Query/Query_NodeRoot.hpp"
//#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
//#include "SAPDB/SQLManager/Query/Query_NodeUnion.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeInsert.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeUpdate.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeDelete.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"

// ------------------------------------------------------------------------------------------------
    
Query_RootNode::Query_RootNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph ) 
    : Query_Node(Context, QueryGraph, P_Start, 0, false, DT_Preserve)
    , mIsExplain(false)
{
}

Query_RootNode::Query_RootNode( Query_Node* AnyOtherNode )
	: Query_Node(AnyOtherNode->Context()
                , AnyOtherNode->QueryGraph()
                , AnyOtherNode->Position()
                , AnyOtherNode->SyntaxNode(), 
                false, DT_Preserve)
    , mIsExplain(false)
{
}

Query_RootNode::Query_RootNode( const Query_RootNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : Query_Node(N, Father, Q, mpMap, mpQMap) 
    , mIsExplain(false)
{
}

Query_Node* Query_RootNode::CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Query_RootNode* R = new(mAllocator) Query_RootNode(*this, Father, Q, mpMap, mpQMap);
    R->mIsExplain = mIsExplain;
    return R;
}

void Query_RootNode::PrintToTrace( Kernel_VTrace& VTrace )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_RootNode::PrintToTrace", QueryRewrite_Trace, 8);

    Query_Node::PrintToTrace(VTrace);
    if ( QueryRewrite_Trace.TracesLevel(2) )
    {
        VTrace << "NodeType  : Top" << FlushLine;
    }
    PrintChildNodesToTrace(VTrace);
}

bool Query_RootNode::Parse( bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTree.SetRefBack(mAPNode);   // DDT - sollte man das nicht "on the fly" machen ?

    SyntaxTreeNodeNo N = (mAPNode==0 ? SyntaxTree[mAPNode].n_lo_level : mAPNode);


	switch ( SyntaxTree[N].n_proc )
	{
	case a43:
		if ( SyntaxTree[N].n_subproc() == cak_x_explain )
		{
			mIsExplain = true;
			N = SyntaxTree[N].n_lo_level;
			return AddQuery(N, ComplexViewReplacement);
		}
		else
		{
			return false;
		}
		break;
	case a63:
		switch ( SyntaxTree[N].n_subproc() )
		{
		case cak_x_decl_cursor:
		case cak_x_mass_select:
		case cak_x_start_union:
			return AddQuery(N, ComplexViewReplacement);
			break;
		case cak_x_recursive_select:
			mQueryGraph.SetNotYetImplemented("cak_x_recursive_select", false);  // DDT : recursive select 
			return false;
			break;
		}
		break;
	case a63query_spec:
		return AddQuery(N, ComplexViewReplacement);
		break;
	case a56:
		switch ( SyntaxTree[N].n_subproc() )
		{
		case cak_x_normal_insert:
		case cak_x_minsert:
		case cak_x_insert_select:
			return AddInsert(N, ComplexViewReplacement);
			break;			
		}
		break;
	default:
		mQueryGraph.SetNotYetImplemented("cak_x_recursive_select", false); 
		break;
	}

    return false;
}

bool Query_RootNode::AddQuery( SyntaxTreeNodeNo N, bool ComplexViewReplacement )
{
	Query_Node* Query = Query_Node::AddQuery(N, mPosition, ComplexViewReplacement);
    if ( Query )
	{
        if ( Query->Parse(ComplexViewReplacement) )
		{
            mQueryGraph.SetProgress(PG_Nodes);
            return true;
		}
	}
    return false;
}

bool Query_RootNode::AddInsert( SyntaxTreeNodeNo N, bool ComplexViewReplacement )
{
	SAPDBERR_ASSERT_ARGUMENT( !ComplexViewReplacement);

    Query_Node* NInsert = new(mAllocator) Query_InsertNode(mContext, mQueryGraph, mPosition, N);
    if ( NInsert )
	{
        if ( !AddQuantifier(NInsert, DT_Preserve, QT_F) )
        {
            mQueryGraph.SetError("AddInsert");
            SAPDBERR_ASSERT_STATE( 0 );
            destroy(NInsert, mAllocator);
			return false;
        }

        if ( NInsert->Parse(ComplexViewReplacement) )
		{
            mQueryGraph.SetProgress(PG_Nodes);
            return true;
		}
	}
    return false;
}

bool Query_RootNode::AddUpdate( SyntaxTreeNodeNo N, bool ComplexViewReplacement )
{
	SAPDBERR_ASSERT_ARGUMENT( !ComplexViewReplacement);

    Query_Node* NUpdate = new(mAllocator) Query_UpdateNode(mContext, mQueryGraph, mPosition, N);
    if ( NUpdate )
	{
        if ( !AddQuantifier(NUpdate, DT_Preserve, QT_F) )
        {
            mQueryGraph.SetError("AddUpdate");
            SAPDBERR_ASSERT_STATE( 0 );
            destroy(NUpdate, mAllocator);
			return false;
        }

        if ( NUpdate->Parse(ComplexViewReplacement) )
		{
            mQueryGraph.SetProgress(PG_Nodes);
            return true;
		}
	}
    return false;
}

bool Query_RootNode::AddDelete( SyntaxTreeNodeNo N, bool ComplexViewReplacement )
{
	SAPDBERR_ASSERT_ARGUMENT( !ComplexViewReplacement);

    Query_Node* NDelete = new(mAllocator) Query_DeleteNode(mContext, mQueryGraph, mPosition, N);
    if ( NDelete )
	{
        if ( !AddQuantifier(NDelete, DT_Preserve, QT_F) )
        {
            mQueryGraph.SetError("AddDelete");
            SAPDBERR_ASSERT_STATE( 0 );
            destroy(NDelete, mAllocator);
			return false;
        }
     
		if ( NDelete->Parse(ComplexViewReplacement) )
		{
            mQueryGraph.SetProgress(PG_Nodes);
            return true;
		}
	}
    return false;
}

// ------------------------------------------------------------------------------------------------
