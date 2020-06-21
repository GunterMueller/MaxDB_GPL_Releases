/*!***************************************************************************

    @file     Query_Node.cpp
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

#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"
#include "SAPDB/SQLManager/Query/Query_Node.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeUnion.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeIntersect.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeDifference.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeTable.hpp"

// ------------------------------------------------------------------------------------------------

Query_Node::Query_Node( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph,
                        Query_Position Position, SyntaxTreeNodeNo APNode,
                        bool HeadDistinct, Query_DistinctType BodyDistinctType )
    : mContext(Context)
    , mAllocator(Context.GetAllocator())
    , mQueryGraph(QueryGraph)
    , mAPNode(APNode)
    , mID(0)
    , mFQuantifierCnt(0)
    , mWQuantifierCnt(0)
    , mLQuantifierCnt(0)
    , mPosition(Position)
    , mFatherNode(0)
    , mFatherQuantifier(0)
    , mQuantifier(Context.GetAllocator())
    , mHeadDistinct(HeadDistinct)
    , mBodyDistinctType(BodyDistinctType)
    , mSemanticsDone(false)
	, mViewReplacement(false)
{
    mQuantifier.Reserve(5);
    mQueryGraph.SetNodeID(this);
}

// copy constructor
Query_Node::Query_Node( const Query_Node& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : mContext(N.mContext)
    , mAllocator(N.mAllocator)
    , mQueryGraph(N.mQueryGraph)
    , mAPNode(0)                                    //
    , mID(0)                                        //
    , mFQuantifierCnt(0)                            // will be set in AddQuantifier
    , mWQuantifierCnt(0)                            // will be set in AddQuantifier
    , mLQuantifierCnt(0)                            // will be set in AddQuantifier
    , mPosition(N.mPosition)
    , mFatherNode(0)                                //
    , mFatherQuantifier(0)                          //
    , mQuantifier(N.mAllocator)                     //
    , mHeadDistinct(N.mHeadDistinct)
    , mBodyDistinctType(N.mBodyDistinctType)
    , mSemanticsDone(N.mSemanticsDone)
{
    // Create an own id
    mQueryGraph.SetNodeID(this);

    mpQMap->AddMappingInfo(N.mID, mID);

    // Map the mAPNode
    if ( !mQueryGraph.SyntaxTree().MapNode(mAPNode, mpMap) )
        mQueryGraph.SetError("Map APNode");

    // Set the linkage to topnode and topquantifier
    mFatherNode         = Father;
    mFatherQuantifier	= Q;

    // Copy the quantifiers
    SAPDB_Int2 m = N.mQuantifier.GetSize();
    mQuantifier.Reserve(m);
    for ( SAPDB_Int2 i=0; i<m; m++ )
    {
        Query_Quantifier* Q = (mQuantifier[i])->CreateDuplicate(this, mpMap, mpQMap);
        if ( Q )
            AddQuantifier((mQuantifier[i])->CreateDuplicate(this, mpMap, mpQMap));
        else
            mQueryGraph.SetError("CreateDuplicate Quantifier");
    }
}

Query_Node::~Query_Node( void )
{
    unsigned qc = mQuantifier.GetSize();
    for ( unsigned i = 0; i < qc; i++ )
    {
        if ( mQuantifier[i] != 0 )
            destroy(mQuantifier[i], mAllocator);
        mQuantifier[i] = 0;
    }
}

// ------------------------------------------------------------------------------------------------

void Query_Node::SetPosition( Query_Position Pos ) 
{
    if ( Pos == P_FromPart && mPosition != P_FromPart && IsSelectOrSet() )
        mQueryGraph.FromSelectFound();
    if ( Pos != P_FromPart && mPosition == P_FromPart && IsSelectOrSet() )
        mQueryGraph.FromSelectReplaced();
    mPosition = Pos; 
}

// ------------------------------------------------------------------------------------------------

bool Query_Node::IllegalUseOfMethod( const char* MethodName )
{
	mQueryGraph.SetError(MethodName, /*TraceGraph*/ false);
    SAPDBERR_ASSERT_STATE(0);   // alway crash in slowknl
	return false;
}

// ------------------------------------------------------------------------------------------------

void Query_Node::PrintToTrace( Kernel_VTrace& VTrace )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_Node::PrintToTrace", QueryRewrite_Trace, 8);

    if ( QueryRewrite_Trace.TracesLevel(2) )
    {
        VTrace << "+-------------------------------" << FlushLine;
        VTrace << "NodeID     : " << mID << FlushLine;
        VTrace << "FatherID   : " << (mFatherNode  ? mFatherNode->NodeID()  : cak_qrewrite_ref_init) << FlushLine;
        for ( unsigned j=0; j < mQuantifier.GetSize(); j++ )
            VTrace << "ChildNodeID: " << mQuantifier[j]->NodeRef()->NodeID() << FlushLine;
        VTrace << "APNode     : " << mAPNode << FlushLine;

        switch ( mPosition )
        {
        case P_Start:
            VTrace << "Position   : Start" << FlushLine;
            break;
        case P_Union:
            VTrace << "Position   : Union" << FlushLine;
            break;
        case P_Difference:
            VTrace << "Position   : Difference" << FlushLine;
            break;
        case P_Intersect:
            VTrace << "Position   : Intersect" << FlushLine;
            break;
        case P_Select:
            VTrace << "Position   : Select" << FlushLine;
            break;
        case P_SelectList:
            VTrace << "Position   : SelectList" << FlushLine;
            break;
        case P_FromPart:
            VTrace << "Position   : FromPart" << FlushLine;
            break;

        case P_Where:
            VTrace << "Position   : Where" << FlushLine;
            break;
        case P_Having:
            VTrace << "Position   : Having" << FlushLine;
            break;
        case P_OrderBy:
            VTrace << "Position   : OrderBy" << FlushLine;
            break;
        case P_GroupBy:
            VTrace << "Position   : GroupBy" << FlushLine;
            break;
        case P_UpdateColumns:
            VTrace << "Position   : GroupBy" << FlushLine;
            break;
        case P_Unknown:
            VTrace << "Position   : Unkown ???" << FlushLine;
            SAPDBERR_ASSERT_STATE( 0 );
            break;
        default:
            break;
        }
    }
}

void Query_Node::PrintChildNodesToTrace( Kernel_VTrace& VTrace )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_Node::PrintChildNodesToTrace", QueryRewrite_Trace, 8);

    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; i < n; i++ )
        mQuantifier[i]->NodeRef()->PrintToTrace(VTrace);
}

// ------------------------------------------------------------------------------------------------

Query_Node* Query_Node::GetFirstChildNode( SAPDB_Int2& ChildCount )
{
	ChildCount = mQuantifier.GetSize();
	if ( ChildCount > 0 )
		return mQuantifier[0]->NodeRef();
	else
		return 0;
}

// ------------------------------------------------------------------------------------------------

bool Query_Node::Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_Node::Semantics", QueryRewrite_Trace, 6);

    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
    {
        Query_Quantifier* Q =   mQuantifier[i];
        if ( !Q->NodeRef()->Semantics(UserId, UniqueOutputColumnNames, ComplexViewReplacement) )
		{
			mQueryGraph.SetError("Semantcis of Subnode failed");
            return false;
		}
    }
    return mQueryGraph.IsOk();
}

bool Query_Node::ReplaceComplexViews( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_Node::ReplaceComplexViews", QueryRewrite_Trace, 6);

#if !defined( QRW_REPLACEVIEWS_DIRECT )
    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
    {
        if ( !mQuantifier[i]->NodeRef()->ReplaceComplexViews() )
            return false;
    }
    return mQueryGraph.IsOk();
#else
    mQueryGraph.SetError("Unreplaced ComplexViews");
    SAPDBERR_ASSERT_STATE( 0 );
    return false;
#endif
}

bool Query_Node::SetTempFileNameAndCounter( SAPDB_Int1 Level, SAPDB_Int2& Cnt, bool FatherIsUnion, SAPDB_Int2& FromSelCnt, SAPDB_Int2& ParsCnt, bool& ContainsCorrelatedSubquery, bool& ContainsFromSelectInSubquery, SAPDB_Int2& MaxSubqueryLevel )
{
    SAPDB_Int2 n = mQuantifier.GetSize();
	SAPDB_Int2 QuantCnt = n;
	if ( LQuantifierCnt() )
	{
		for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
		{
			Query_Quantifier* Q = mQuantifier[i];
			if ( Q->QuantifierType()==QT_L )
			{
				SAPDBERR_ASSERT_STATE( Q->NodeRef()->IsSelectOrSet() );
				--QuantCnt;
				if ( !Q->NodeRef()->SetTempFileNameAndCounter(Level, Cnt, FatherIsUnion/*false*/, FromSelCnt, ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel) )
					return false;
				if ( !FatherIsUnion )
					++Cnt;
			}
		}
		SAPDBERR_ASSERT_STATE( QuantCnt == n-LQuantifierCnt() );
	}

	for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
	{
		Query_Quantifier* Q = mQuantifier[i];
		if ( Q->QuantifierType()==QT_F )
		{
			--QuantCnt;
			if ( Q->NodeRef()->IsSelectOrSet() || Q->NodeRef()->IsInsertSelect() )
			{
				if ( !Q->NodeRef()->SetTempFileNameAndCounter(Level, Cnt, FatherIsUnion/*false*/, FromSelCnt, ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel) )
					return false;
				if ( !FatherIsUnion )
					++Cnt;
			}
		}
	}

	if ( QuantCnt )
	{
		for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
		{
			Query_Quantifier* Q = mQuantifier[i];
			if ( Q->QuantifierType()!=QT_L && Q->QuantifierType()!=QT_F )
			{
				SAPDBERR_ASSERT_STATE( Q->NodeRef()->IsSelectOrSet() );
				--QuantCnt;
				if ( !Q->NodeRef()->SetTempFileNameAndCounter(Level, Cnt, FatherIsUnion/*false*/, FromSelCnt, ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel) )
					return false;
				if ( !FatherIsUnion )
					++Cnt;
			}
		}
	}
	SAPDBERR_ASSERT_STATE( !QuantCnt );
    return mQueryGraph.IsOk();
}

// ------------------------------------------------------------------------------------------------

bool Query_Node::GetQuantifierNoFromSyntaxNode( SyntaxTreeNodeNo Node, SAPDB_Int2& QNo )
{
    SAPDBERR_ASSERT_ARGUMENT( Node>0 );

    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( QNo=0; QNo<n; QNo++ )
    {
        if ( mQuantifier[QNo]->NodeRef()->SyntaxNode() == Node )
            return true;
    }
	return false;
}

bool Query_Node::GetQuantifierNoFromQId( SAPDB_Int2 QID, SAPDB_Int2& QNo )
{
    SAPDBERR_ASSERT_ARGUMENT( QID>0 );

    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( QNo=0; QNo<n; QNo++ )
    {
        if ( mQuantifier[QNo]->NodeRef()->NodeID() == QID )
            return true;
    }
	return false;
}

bool Query_Node::GetQuantifierIDFromSyntaxNode( SyntaxTreeNodeNo Node, SAPDB_Int2& QID )
{
    SAPDBERR_ASSERT_ARGUMENT( Node>0 );

    SAPDB_Int2 n = mQuantifier.GetSize();

    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        if ( mQuantifier[i]->NodeRef()->SyntaxNode() == Node )
		{
			QID = mQuantifier[i]->NodeRef()->NodeID();
            return true;
		}
    }
	return false;
}

bool Query_Node::GetQuantifierIDAndNoFromSyntaxNode( SyntaxTreeNodeNo Node, SAPDB_Int2& QID, SAPDB_Int2& QNo )
{
    SAPDBERR_ASSERT_ARGUMENT( Node>0 );

    SAPDB_Int2 n = mQuantifier.GetSize();

    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        if ( mQuantifier[i]->NodeRef()->SyntaxNode() == Node )
		{
			QID = mQuantifier[i]->NodeRef()->NodeID();
			QNo = i;
            return true;
		}
    }
	return false;
}

// ------------------------------------------------------------------------------------------------

Query_Node* Query_Node::AddQuery( SyntaxTreeNodeNo NodeNo, Query_Position Position, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SAPDB_Int2 SubQueryType = 0;
    
    Query_Node* Query = 0;
    bool All=true;
    if ( SyntaxTree[NodeNo].n_proc == a63 )
    {
        switch ( SyntaxTree[NodeNo].n_subproc() )
        {
        case cak_x_start_union:
            return AddQuery(SyntaxTree[NodeNo].n_lo_level, Position, ComplexViewReplacement);    // that's correct !
            break;

        case cak_x_mass_select:
            Query = new(mAllocator) Query_SelectNode(mContext, mQueryGraph, Position, NodeNo, /*Distinct*/false, /*IsDeclareCursor*/false);
            break;
        case cak_x_decl_cursor:
            Query = new(mAllocator) Query_SelectNode(mContext, mQueryGraph, Position, NodeNo, /*Distinct*/false, /*IsDeclareCursor*/true);
            break;

        case cak_x_union:
            All=false;
        case cak_x_union_all:
            Query = new(mAllocator) Query_UnionNode(mContext, mQueryGraph, Position, NodeNo, All);
            break;
        case cak_x_except:
            All=false;
        case cak_x_except_all:
            Query = new(mAllocator) Query_DifferenceNode(mContext, mQueryGraph, Position, NodeNo, All);
            break;
        case cak_x_intersect:
            All=false;
        case cak_x_intersect_all:
            Query = new(mAllocator) Query_IntersectNode(mContext, mQueryGraph, Position, NodeNo, All);
			break;
        default:
            mQueryGraph.SetError("AddQuery 3", NodeNo);
            SAPDBERR_ASSERT_STATE( 0 );
            break;
        }
    }
    else
    {
        if ( SyntaxTree[NodeNo].n_proc == a63query_spec )
        {
            Query = new(mAllocator) Query_SelectNode(mContext, mQueryGraph, Position, NodeNo, /*Distinct*/false, /*IsDeclareCursor*/false);
        }
        else
        {
            mQueryGraph.SetError("AddQuery 5", NodeNo);
            SAPDBERR_ASSERT_STATE( 0 );
        }
    }
    if ( Query )
    {
        Query_QuantifierType QT = DetermineQuantifierType(Position, Query->GetSubQueryType());
        if ( !AddQuantifier(Query, DT_Preserve, QT) )
        {
            mQueryGraph.SetError("AddQuery 5", NodeNo);
            SAPDBERR_ASSERT_STATE( 0 );
            destroy(Query, mAllocator);
			Query = 0;
        }
    }
    else
    {
        mQueryGraph.SetError("AddQuery 6", NodeNo);
        SAPDBERR_ASSERT_STATE( 0 );
    }
    return Query;
}

Query_Node* Query_Node::AddTable( SyntaxTreeNodeNo NodeNo, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    Query_Node* Query = new(mAllocator) Query_TableNode(mContext, mQueryGraph, P_FromPart, NodeNo, ComplexViewReplacement);
    if ( Query )
    {
        if ( !AddQuantifier(Query, DT_Preserve, QT_F) )
        {
            mQueryGraph.SetError("AddTable 1");
            destroy(Query, mAllocator);
			Query = 0;
        }
    }
    else
    {
        mQueryGraph.SetError("AddTable 2");
    }  
    return Query;
}   

bool Query_Node::AddQuantifier ( Query_Quantifier* Q )
{
    SAPDBERR_ASSERT_ARGUMENT( Q );
    SAPDBTRACE_METHOD_DEBUG ("Query_Node::AddQuantifier", QueryRewrite_Trace, 5);

    SAPDB_UInt2 Position = mQuantifier.GetSize()+1;  // this will be an InsertEnd  DDT : +1 is incorrect !!!
    return AddQuantifier(Q, Position );
}

bool Query_Node::AddQuantifier ( Query_Quantifier* Q, SAPDB_UInt2 Position )
{
    SAPDBERR_ASSERT_ARGUMENT( Q );
    SAPDBTRACE_METHOD_DEBUG ("Query_Node::AddQuantifier", QueryRewrite_Trace, 5);

    Q->NodeRef()->FatherNode() = this;
    if ( !mQuantifier.Insert(Position, Q) )
        return false;

    switch ( Q->QuantifierType() )
    {
    case QT_F:
        ++mFQuantifierCnt;
        if ( Q->NodeTypeSelectOrSet() )
            mQueryGraph.SetNeedOptimization();
        break;
    case QT_A:
    case QT_E:
    case QT_S:
        ++mWQuantifierCnt;
        if ( Q->NodeTypeSelectOrSet() )
        {
            mQueryGraph.SetNeedOptimization();
        }
        else
        {
            mQueryGraph.SetError("Illegal Quantifier type 1");
            SAPDBERR_ASSERT_STATE( 0 );
        }
        break;
    case QT_L:
        ++mLQuantifierCnt;
        if ( !Q->NodeTypeSelectOrSet() )
        {
            mQueryGraph.SetError("Illegal Quantifier type 2");
            SAPDBERR_ASSERT_STATE( 0 );
        }
        break;
	case QT_Unknown:	// DDT
    default:
        mQueryGraph.SetError("Unknown Quantifier type");
        SAPDBERR_ASSERT_STATE( 0 );
        break;
    }

    return true;
}

bool Query_Node::AddQuantifier ( Query_Node* Node, Query_DistinctType DistinctType, Query_QuantifierType QuantifierType )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_Node::AddQuantifier", QueryRewrite_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Node );

    Query_Quantifier* Quantifier = new(mAllocator) Query_Quantifier(Node, DistinctType, QuantifierType);
    if ( Quantifier )
    {
        Node->FatherQuantifier() = Quantifier;
   //     if ( !AddQuantifier(Quantifier) )
   //     {
   //         mQueryGraph.SetError("AddQuantifier 1");
   //         destroy(Quantifier, mAllocator);
			//Quantifier = 0;
   //     }


        Node->FatherNode() = this;
        if ( !mQuantifier.InsertEnd(Quantifier) )
        {
            mQueryGraph.SetError("AddQuantifier 1");
            destroy(Quantifier, mAllocator);
			Quantifier = 0;
        }

        switch ( QuantifierType )
        {
        case QT_F:
            ++mFQuantifierCnt;
            if ( Quantifier->NodeTypeSelectOrSet() )
                mQueryGraph.SetNeedOptimization();
            break;
        case QT_A:
        case QT_E:
        case QT_S:
            ++mWQuantifierCnt;
            if ( Quantifier->NodeTypeSelectOrSet() )
            {
                mQueryGraph.SetNeedOptimization();
            }
            else
            {
                mQueryGraph.SetError("Illegal Quantifier type 1");
                SAPDBERR_ASSERT_STATE( 0 );
            }
            break;
        case QT_L:
            ++mLQuantifierCnt;
            if ( !Quantifier->NodeTypeSelectOrSet() )
            {
                mQueryGraph.SetError("Illegal Quantifier type 2");
                SAPDBERR_ASSERT_STATE( 0 );
            }
            break;
		case QT_Unknown:	// DDT
        default:
            mQueryGraph.SetError("Unknown Quantifier type");
            SAPDBERR_ASSERT_STATE( 0 );
            break;
        }

        return true;
    }
    mQueryGraph.SetError("AddQuantifier 2");
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_Node::ContainsCorrelatedSubquery( void )
{
	if ( WQuantifierCnt() > 0 )
	{
		SAPDB_Int2 n = mQuantifier.GetSize();
		for ( SAPDB_Int2 i=0; i<n; i++ )
		{
			Query_Node* Node = (&(*mQuantifier[i]))->NodeRef();

			if ( Node->IsSubquery() )
			{
				if ( Node->IsCorrelatedSubquery() )
					return true;
				else
					if ( Node->ContainsCorrelatedSubquery() )
						return true;
			}
		}
	}
	return false;
}

// ------------------------------------------------------------------------------------------------
