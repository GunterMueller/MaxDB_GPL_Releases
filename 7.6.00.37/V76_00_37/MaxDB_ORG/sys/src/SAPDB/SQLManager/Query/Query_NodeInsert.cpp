/*!***************************************************************************

    @file     Query_NodeInsert.cpp
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

#include "SAPDB/SQLManager/Query/Query_NodeInsert.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"

// ------------------------------------------------------------------------------------------------

Query_InsertNode::Query_InsertNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode ) 
    : Query_Node(Context, QueryGraph, Position, APNode, false, DT_Unknown)
	, mIsInsertSelect(false)
	, mIsPseudoInsertSelect(false)
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	SAPDBERR_ASSERT_ARGUMENT( SyntaxTree[mAPNode].n_proc == a56
		                     && (SyntaxTree[mAPNode].n_subproc() == cak_x_normal_insert
		                      || SyntaxTree[mAPNode].n_subproc() == cak_x_minsert
							  || SyntaxTree[mAPNode].n_subproc() == cak_x_insert_select) );

	if ( SyntaxTree[mAPNode].n_subproc() == cak_x_insert_select )
	{
		mIsInsertSelect = true;
		if ( SyntaxTree[mAPNode].n_length() == cak_x_pseudo_ins_select ||
			 SyntaxTree[mAPNode].n_length() == cak_x_pseudo_ins_select + cak_i_ignore )
			mIsPseudoInsertSelect = true;
		mQueryGraph.SetCommandIsInsertSelect();
	}
}

Query_InsertNode::Query_InsertNode( const Query_InsertNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : Query_Node(N, Father, Q, mpMap, mpQMap) 
	, mIsInsertSelect(N.mIsInsertSelect)
	, mIsPseudoInsertSelect(N.mIsPseudoInsertSelect)
{
}

Query_Node* Query_InsertNode::CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    return new(mAllocator) Query_InsertNode(*this, Father, Q, mpMap, mpQMap);
}

void Query_InsertNode::PrintToTrace( Kernel_VTrace& VTrace )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_InsertNode::PrintToTrace", QueryRewrite_Trace, 8);

    Query_Node::PrintToTrace(VTrace);
    VTrace << "NodeType  : Insert" << FlushLine;
    PrintChildNodesToTrace(VTrace);
}

bool Query_InsertNode::Parse( bool ComplexViewReplacement )
{
	SAPDBERR_ASSERT_ARGUMENT( !ComplexViewReplacement);

	if ( mIsInsertSelect )	
	{
	    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
		SyntaxTreeNodeNo N = SyntaxTree[mAPNode].n_sa_level;
		SAPDBERR_ASSERT_STATE( N );
		if ( SyntaxTree[N].n_proc == a63 && SyntaxTree[N].n_subproc() == cak_x_subquery )
			N = SyntaxTree[N].n_pos();
		else
		{
			SAPDBERR_ASSERT_STATE( 0 );
			return false;
		}

	    Query_Node* Query = AddQuery(N, P_Insert, ComplexViewReplacement);
		if ( Query )
		{
			if ( Query->Parse(ComplexViewReplacement) )
				return true;
		}
		return false;
	}
	else
	{
		// DDT : Do nothing so far
		return true;
	}
}

bool Query_InsertNode::Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement )
{
	SAPDBERR_ASSERT_ARGUMENT( !ComplexViewReplacement);
	SAPDBERR_ASSERT_STATE( (mIsPseudoInsertSelect ? mIsInsertSelect : true) );
	SAPDBERR_ASSERT_STATE( (mIsInsertSelect ? true : !mIsPseudoInsertSelect) );

	// DDT : Do nothing so far but call Semantics for the SelectNode

	if ( mIsInsertSelect )	
	{
		SAPDBERR_ASSERT_STATE( mQuantifier.GetSize()==1 );
		return Query_Node::Semantics(UserId, UniqueOutputColumnNames, ComplexViewReplacement);
	}
	else
	{
		SAPDBERR_ASSERT_STATE( mQuantifier.GetSize()==0 );
		return true;
	}
}


// ------------------------------------------------------------------------------------------------
