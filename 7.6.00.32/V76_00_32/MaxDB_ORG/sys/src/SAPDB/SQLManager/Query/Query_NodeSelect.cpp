/*!***************************************************************************

    @file     Query_NodeSelect.cpp
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

#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSet.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeTable.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"
#include "SAPDB/SQLManager/Query/Query_OutputColumn.hpp"
#include "SAPDB/SQLManager/Query/Query_Column.hpp"

#include "SAPDB/RunTime/RTE_CurrentStack.hpp"

// ------------------------------------------------------------------------------------------------

Query_SelectNode::Query_SelectNode(  Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode, bool Distinct, bool IsDeclareCursor )
    : Query_Node(Context, QueryGraph, Position, APNode, /*HeadDistinct*/(Distinct?true:false), (Distinct?DT_Enforce:DT_Preserve))
    , mReferenceName()
    , mpOutputColumns(0)
    , mSearchConditionColumns()
    , mHavingColumns()
    , mpGroupByColumns(0)
    , mpOrderByColumns(0)
	, mUpdateColumns()
    , mOutputColumnCnt(0)
    , mInternAPNode(APNode)
    , mRefNameNode(cak_qrewrite_ref_init)
    , mSelectListStartNo(cak_qrewrite_ref_init)
    , mSelectIntoStartNo(cak_qrewrite_ref_init)
    , mFromPartStartNo(cak_qrewrite_ref_init)
    , mSearchConditionStartNo(cak_qrewrite_ref_init)
    , mOrderByStartNo(cak_qrewrite_ref_init)
    , mGroupByStartNo(cak_qrewrite_ref_init)
    , mHavingStartNo(cak_qrewrite_ref_init)
	, mLimitStartNo(cak_qrewrite_ref_init)
    , mForUpdateColumnStartNo(cak_qrewrite_ref_init)
    , mLockOptionStart(cak_qrewrite_ref_init)
    , mCursorNameNo(cak_qrewrite_ref_init)
    , mJoinType(JT_Unknown)
    , mSubQueryType(SQT_None)
	, mExprCnt(0)
    , mDistinct(Distinct)
    , mIsDeclareCursor(IsDeclareCursor)
    , mResDelete(QNULL)
	, mForReuse(false)
	, mGivenSequence(false)
    , mCorrelatedColumns(0)
    , mCorrelatedColumnsRefersToThis(0)
    , mCorrelatedAndAggregatedColumnsRefersToThis(0)
	, mUnidentifiedColumns(false)
    , mQualificationPushedDown(false)
    , mSimplifiedPredicates(false)
	, mSubqueryOptimized(false)
    , mRownoPredicate(false)
    , mOrderByEliminated(false)
    , mProjectionPulledDown(false)
    , mAggregatesInOutput(false)
    , mAggregatesNotInOutput(false)
    , mOneTupleCondition(false)
    , mNoDuplicateCondition(QNULL)
    , mSpecialJoinFromSelect(false)
{
    DetermineAPNode();  // DDT : - > PARSE

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo H = SyntaxTree[mInternAPNode].n_sa_level;
    if ( H > 0 )
    {
        if (   SyntaxTree[H].n_proc != no_proc
            && SyntaxTree[H].n_symb != s_identifier )
        {
            mQueryGraph.SCVMoveSubqueryInformation(mQueryGraph.SyntaxTree(), H);
            SyntaxTree.UnlinkNodeWithSameLevel(H, /*SetUnsused*/ true);
        }
    }

    if ( mPosition == P_FromPart )
        mQueryGraph.FromSelectFound();
}

void Query_SelectNode::DetermineAPNode( void )
{ 
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    if (   SyntaxTree[mInternAPNode].n_proc == a63query_spec  
        && SyntaxTree[mInternAPNode].n_querycheck.includes(qcn_SpecialJoinFromSelect) )
    {
        mSpecialJoinFromSelect = true;
        mQueryGraph.ContainsSpecialJoinFromSelect(true);
    }

    SyntaxTreeNodeNo H = SyntaxTree[mInternAPNode].n_refback;
    if ( SyntaxTree[H].n_proc == a66 && SyntaxTree[H].n_subproc() == cak_x_select_in_from_part ) 
        mAPNode = H;
    else
        mAPNode = mInternAPNode;

    switch ( mPosition )
    {
        case P_SelectList:
            mSubQueryType = SQT_SELLIST;
            break;
        case P_GroupBy:
        case P_OrderBy:
            mSubQueryType = SQT_OTHER;
            break;
        case P_Where:
        case P_Having:
            mSubQueryType = SQT_OTHER;
			if ( SyntaxTree[H].n_proc == a64 && SyntaxTree[H].n_subproc() == cak_x_predicate )
			{
				switch ( SyntaxTree[H].n_symb )
				{
				case s_equal:
					mSubQueryType = SQT_EQ;
					break;
				case s_exists:
					mSubQueryType = SQT_EXISTS;
					break;
				}
			}
			else
			{
				if ( SyntaxTree[H].n_proc == no_proc && SyntaxTree[H].n_symb == s_all )
				{
					mSubQueryType = SQT_ALL;
				}
				else
				{
					if ( SyntaxTree[H].n_proc == no_proc && SyntaxTree[H].n_symb == s_any )
					{
						mSubQueryType = SQT_ANY;
					}
					else
					{
						H = mInternAPNode;
						SyntaxTreeNodeNo R = SyntaxTree[H].n_refback;
						while ( SyntaxTree[R].n_lo_level != H )
						{
							H = R;
							R = SyntaxTree[H].n_refback;
						}
						if ( SyntaxTree[R].n_proc == a64 && SyntaxTree[R].n_subproc() == cak_x_in_pred )
						{
							mSubQueryType = SQT_IN;
						}
					}
				}
			}
            break;
    }
}

void Query_SelectNode::SetA631( void )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    if ( SyntaxTree[mInternAPNode].n_proc == a63query_spec ) 
    {
        SyntaxTree[mInternAPNode].n_proc.becomes(a63);
        SyntaxTree[mInternAPNode].n_subproc() = cak_x_mass_select; 
    }
#if defined (SAPDB_SLOW)
    if ( SyntaxTree[mInternAPNode].n_proc != a63 || SyntaxTree[mInternAPNode].n_subproc() != cak_x_mass_select )
       SAPDBERR_ASSERT_STATE( 0 );
#endif
}

// copy constructor
Query_SelectNode::Query_SelectNode( const Query_SelectNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : Query_Node(N, Father, Q, mpMap, mpQMap)
    , mpOutputColumns(0)
    , mSearchConditionColumns(N.mSearchConditionColumns)
    , mHavingColumns(N.mHavingColumns)
    , mpGroupByColumns(N.mpGroupByColumns)
    , mpOrderByColumns(N.mpOrderByColumns)
	, mUpdateColumns(N.mUpdateColumns)
    , mReferenceName(N.mReferenceName)
    , mOutputColumnCnt(N.mOutputColumnCnt)
    , mInternAPNode(N.mInternAPNode)
    , mRefNameNode(N.mRefNameNode)
    , mSelectListStartNo(N.mSelectListStartNo)
    , mSelectIntoStartNo(N.mSelectIntoStartNo)
    , mFromPartStartNo(N.mFromPartStartNo)
    , mSearchConditionStartNo(N.mSearchConditionStartNo)
    , mOrderByStartNo(N.mOrderByStartNo)
    , mGroupByStartNo(N.mGroupByStartNo)
    , mHavingStartNo(N.mHavingStartNo)
	, mLimitStartNo(N.mLimitStartNo)
	, mForUpdateColumnStartNo(N.mForUpdateColumnStartNo)
    , mLockOptionStart(N.mLockOptionStart)
    , mCursorNameNo(N.mCursorNameNo)
    , mJoinType(N.mJoinType)
    , mSubQueryType(N.mSubQueryType)
	, mExprCnt(N.mExprCnt)
	, mDistinct(N.mDistinct)
    , mIsDeclareCursor(N.mIsDeclareCursor)
    , mResDelete(N.mResDelete)
	, mForReuse(N.mForReuse)
	, mGivenSequence(N.mGivenSequence)
    , mCorrelatedColumns(N.mCorrelatedColumns)
    , mCorrelatedColumnsRefersToThis(N.mCorrelatedColumnsRefersToThis)
    , mCorrelatedAndAggregatedColumnsRefersToThis(N.mCorrelatedAndAggregatedColumnsRefersToThis)
	, mUnidentifiedColumns(N.mUnidentifiedColumns)
    , mQualificationPushedDown(N.mQualificationPushedDown)
    , mSimplifiedPredicates(N.mSimplifiedPredicates)
	, mSubqueryOptimized(N.mSubqueryOptimized)
    , mRownoPredicate(N.mRownoPredicate)
    , mOrderByEliminated(N.mOrderByEliminated)
    , mProjectionPulledDown(N.mProjectionPulledDown)
    , mAggregatesInOutput(N.mAggregatesInOutput)
    , mAggregatesNotInOutput(N.mAggregatesNotInOutput)
    , mOneTupleCondition(N.mOneTupleCondition)
    , mNoDuplicateCondition(N.mNoDuplicateCondition)
    , mSpecialJoinFromSelect(N.mSpecialJoinFromSelect)
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    SyntaxTree.MapNode(mInternAPNode,           mpMap);
    SyntaxTree.MapNode(mRefNameNode,            mpMap);
    SyntaxTree.MapNode(mSelectListStartNo,      mpMap);
    SyntaxTree.MapNode(mSelectIntoStartNo,      mpMap);
    SyntaxTree.MapNode(mFromPartStartNo,        mpMap);
    SyntaxTree.MapNode(mSearchConditionStartNo, mpMap);
    SyntaxTree.MapNode(mOrderByStartNo,         mpMap);
    SyntaxTree.MapNode(mGroupByStartNo,         mpMap);
    SyntaxTree.MapNode(mHavingStartNo,          mpMap);
	SyntaxTree.MapNode(mLimitStartNo,           mpMap);
	SyntaxTree.MapNode(mForUpdateColumnStartNo, mpMap);
    SyntaxTree.MapNode(mLockOptionStart,        mpMap);
    SyntaxTree.MapNode(mCursorNameNo,           mpMap);

    mpOutputColumns         = CreateDuplicateOutputColumns(N.mpOutputColumns, mAllocator, SyntaxTree, mpMap, mpQMap);  
    mSearchConditionColumns = mSearchConditionColumns.CreateDuplicate2(mAllocator, SyntaxTree, mpMap, mpQMap); 
    mHavingColumns          = mHavingColumns.CreateDuplicate2(mAllocator, SyntaxTree, mpMap, mpQMap);  
    mpGroupByColumns        = CreateDuplicateColumnSpec(N.mpGroupByColumns, mAllocator, SyntaxTree, mpMap, mpQMap);  
    mpOrderByColumns        = CreateDuplicateColumnSpec(N.mpOrderByColumns, mAllocator, SyntaxTree, mpMap, mpQMap);  
	mUpdateColumns          = mUpdateColumns.CreateDuplicate2(mAllocator, SyntaxTree, mpMap, mpQMap); 
    if ( mPosition == P_FromPart )
        mQueryGraph.FromSelectFound();
}

Query_SelectNode::~Query_SelectNode( void )
{
    if ( mPosition == P_FromPart )
        mQueryGraph.FromSelectReplaced();

    DestroyOutputColumns();
    mSearchConditionColumns.Destroy(mAllocator);
    mHavingColumns.Destroy(mAllocator);
    DestroyColumnSpec(mpGroupByColumns);
    DestroyColumnSpec(mpOrderByColumns);
    mUpdateColumns.Destroy(mAllocator);
}

Container_Vector<Query_OutputColumn*>* Query_SelectNode::CreateDuplicateOutputColumns( Container_Vector<Query_OutputColumn*>* OldList, SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Container_Vector<Query_OutputColumn*>* S = new(mAllocator)  Container_Vector<Query_OutputColumn*>(Alloc);
    if ( S )
    {
        SAPDB_Int2 n = OldList->GetSize();
        S->Reserve(n);
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            Query_OutputColumn* C = (*OldList)[i]->CreateDuplicate(Alloc, SyntaxTree, mpMap, mpQMap);
            if ( C )
                S->InsertEnd(C);
            else
                mQueryGraph.SetError("CreateDuplicate OutputColumn failed");
        }
        return S;
    }
    mQueryGraph.SetError("CreateDuplicateOutputColumns failed");
    return 0;
}

Container_Vector<Query_ColumnSpec*>* Query_SelectNode::CreateDuplicateColumnSpec( Container_Vector<Query_ColumnSpec*>* OldList, SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Container_Vector<Query_ColumnSpec*>* S = new(mAllocator)  Container_Vector<Query_ColumnSpec*>(Alloc);
    if ( S )
    {
        SAPDB_Int2 n = OldList->GetSize();
        S->Reserve(n);
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            Query_ColumnSpec* C = (*OldList)[i]->CreateDuplicate(Alloc, SyntaxTree, mpMap, mpQMap);
            if ( C )
                S->InsertEnd(C);
            else
                mQueryGraph.SetError("CreateDuplicate ColumnSpec failed");
        }
        return S;
    }
    mQueryGraph.SetError("CreateDuplicateOutputColumns failed");
    return 0;
}

void Query_SelectNode::DestroyOutputColumns( void )
{
    if ( mpOutputColumns )
    {
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            (*mpOutputColumns)[i]->Destroy(mAllocator);
            destroy((*mpOutputColumns)[i], mAllocator);
			(*mpOutputColumns)[i] = 0;
        }
        destroy(mpOutputColumns, mAllocator);
        mpOutputColumns = 0;
    }
}

void Query_SelectNode::DestroyOneOutputColumn( Query_OutputColumn* Col, SAPDB_Int2 n, bool DecRefCount )
{
	if ( DecRefCount )
		(*mpOutputColumns)[n]->DelRefCountsToChildColumns(this);
    (*mpOutputColumns)[n]->Destroy(mAllocator);
	destroy((*mpOutputColumns)[n], mAllocator);
	mpOutputColumns->Delete(n);
}

void Query_SelectNode::ClearOutputColumns( void )
{
    if ( mpOutputColumns )
    {
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 i1=n-1; i1>=0; i1-- )
			DestroyOneOutputColumn((*mpOutputColumns)[i1], i1, true);
		
		SAPDB_Int2 nq = mQuantifier.GetSize();
		for ( SAPDB_Int2 i2=nq-1; i2>=0; i2-- )
		{
			Query_Quantifier* Q = mQuantifier[i2];
			if ( Q->QuantifierType() == QT_L )
			{
				SyntaxTreeNodeNo QNode = Q->NodeRef()->GetInternAPNode();
				if ( QNode > 0 )
				{
					// DDT : if a correlated subquery is to be deleted their correlation info must be deleted too !!!

					destroy(Q, mAllocator);
					mQuantifier.Delete(i2); 
				}
				else
				{
					mQueryGraph.SetError("Invalid SyntaxNode");
					SAPDBERR_ASSERT_STATE( 0 );
				}
			}
		}
    }
}

void Query_SelectNode::DestroyColumnSpec( Container_Vector<Query_ColumnSpec*>*& pColSpec )
{
    if ( pColSpec )
    {
        SAPDB_Int2 n = pColSpec->GetSize();
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            Query_ColumnSpec* C = (*pColSpec)[i];
            if ( C )
            {
                (*pColSpec)[i]->Destroy(mAllocator);
                destroy((*pColSpec)[i], mAllocator);
            }
        }
        destroy(pColSpec, mAllocator);
        pColSpec = 0;
    }
}

void Query_SelectNode::ExchangeQuantifierIDInOutputColumns( Container_Vector<Query_OutputColumn*>*& pOutCols, SAPDB_Int2 OldID, SAPDB_Int2 NewID )
{
    if ( pOutCols && OldID!=NewID )
    {
        SAPDB_Int2 n = pOutCols->GetSize();
        for ( SAPDB_Int2 i=0; i<n; i++ )
            (*pOutCols)[i]->GetExpression().ExchangeQuantifierID(OldID, NewID);
    }
}

void Query_SelectNode::ExchangeQuantifierIDInColumnSpec( Container_Vector<Query_ColumnSpec*>*& pColSpec, SAPDB_Int2 OldID, SAPDB_Int2 NewID )
{
    if ( pColSpec && OldID!=NewID )
    {
        SAPDB_Int2 n = pColSpec->GetSize();
        for ( SAPDB_Int2 i=0; i<n; i++ )
            (*pColSpec)[i]->GetExpression().ExchangeQuantifierID(OldID, NewID);
    }
}

void Query_SelectNode::ExchangeNodeIDInOutputColumns( Container_Vector<Query_OutputColumn*>*& pOutCols, SAPDB_Int2 OldID, SAPDB_Int2 NewID )
{
    if ( pOutCols && OldID!=NewID )
    {
        SAPDB_Int2 n = pOutCols->GetSize();
		SAPDB_Int2 ex= 0;
        for ( SAPDB_Int2 i=0; i<n; i++ )
            (*pOutCols)[i]->GetExpression().ExchangeNodeID(OldID, NewID, ex);
    }
}

Query_Node* Query_SelectNode::CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    return new(mAllocator) Query_SelectNode(*this, Father, Q, mpMap, mpQMap);
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::SetSubQueryTypeForQuantifier( SyntaxTreeNodeNo QNodeNo, Query_SubQueryType SubQueryType )
{
  	SAPDB_Int2 QNo=0;
  	if ( GetQuantifierNoFromSyntaxNode(QNodeNo, QNo) )
  	{
  		Query_Quantifier* Q = mQuantifier[QNo];
        Q->NodeRef()->SetSubQueryType(SubQueryType);
        return true;
    }
    mQueryGraph.SetError("SetSubqueryTypeForQuantifier");
    SAPDBERR_ASSERT_ARGUMENT( 0 );
    return false;
}

// ------------------------------------------------------------------------------------------------

void Query_SelectNode::PrintToTrace( Kernel_VTrace& VTrace )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::PrintToTrace", QueryRewrite_Trace, 8);

    Query_Node::PrintToTrace(VTrace);
    if ( QueryRewrite_Trace.TracesLevel(2) )
    {
        VTrace << "NodeType   : Select" << FlushLine;
        VTrace << "SelectList : " << mSelectListStartNo << FlushLine;
        VTrace << "FromPart   : " << mFromPartStartNo << FlushLine;
        VTrace << "SearchCond : " << mSearchConditionStartNo << FlushLine;
        VTrace << "OrderBy    : " << mOrderByStartNo << FlushLine;
        VTrace << "GroupBy    : " << mGroupByStartNo << FlushLine;
        VTrace << "Having     : " << mHavingStartNo << FlushLine;
        VTrace << "Limit      : " << mLimitStartNo << FlushLine;
        VTrace << "ForUpdate  : " << mForUpdateColumnStartNo << FlushLine;

        VTrace << "IsCorrSub  : " << (mCorrelatedColumns>0?"yes":"no") << FlushLine;
        VTrace << "CorrColumns: " << mCorrelatedColumnsRefersToThis << FlushLine;
        VTrace << "ContainAggr: " << (mAggregatesInOutput||mAggregatesNotInOutput?"yes":"no") << FlushLine;

        VTrace << "InnerJoin  : " << ((mFQuantifierCnt && !OuterJoin())?"yes":"no") << FlushLine;
        VTrace << "FullOuter  : " << ((mJoinType & JT_FullOuter )?"yes":"no") << FlushLine;
        VTrace << "RightOuter : " << ((mJoinType & JT_RightOuter)?"yes":"no") << FlushLine;
        VTrace << "LeftOuter  : " << ((mJoinType & JT_LeftOuter )?"yes":"no") << FlushLine;

        VTrace << "Quantifier : " << mQuantifier.GetSize() << FlushLine;
        VTrace << "FQuantifier: " << mFQuantifierCnt << FlushLine;
        if ( mSemanticsDone && QueryRewrite_Trace.TracesLevel(3) )
        {
            VTrace << "FQuantifier: " << mFQuantifierCnt << FlushLine;
            SAPDB_Int2 nq = mQuantifier.GetSize();
            SAPDBERR_ASSERT_STATE( nq >= mFQuantifierCnt);
            for ( SAPDB_UInt2 f=0; f<nq; f++ )
            {
                if ( mQuantifier[f]->QuantifierType() == QT_F )
                    VTrace  << "FQ" << f << " Node    : "
                            << mQuantifier[f]->SyntaxNode() << FlushLine;
            }

            VTrace << "OutColumns : " << mOutputColumnCnt << FlushLine;
            if ( mpOutputColumns )
            {
                for ( SAPDB_UInt2 c=0; c<mpOutputColumns->GetSize(); c++ )
                {
                    if ( (*mpOutputColumns)[c]->IsColumnOrSingleAsterisk() )
                    {
                        SAPDB_Int2 FromTabNode = (*mpOutputColumns)[c]->GetQuantifierSyntaxNode();
                        if ( FromTabNode > 0 )
					    {
						    SAPDB_Int2 QNo=0;
						    GetQuantifierNoFromSyntaxNode(FromTabNode, QNo);
                            VTrace  << "Col" << c << " Node   : "
                                    << (*mpOutputColumns)[c]->GetSyntaxNode()
                                    << " Quantifier Node "
                                    << mQuantifier[QNo]->SyntaxNode() << FlushLine;
					    }
                    }
                }
            }
        }
    }
    if ( QueryRewrite_Trace.TracesLevel(1) && !QueryRewrite_Trace.TracesLevel(2) )
    {
        SAPDB_Int2 n = mQuantifier.GetSize();

        VTrace << "[" << mID << "] SELECT ";

        bool found = false;
        SAPDB_Int2 i;
        for (i=0; i<n; i++)
        {
            Query_Quantifier* Q = mQuantifier[i];
            if ( Q->QuantifierType() == QT_L )
            {
                if ( !found )
                {
                    VTrace << "<" << Q->NodeID() << ">";
                    found = true;
                }
                else
                    VTrace << ",<" << Q->NodeID() << ">";
            }
        }
        if (!found)
            VTrace << "...";
        VTrace << FlushLine;

        VTrace << "               FROM ";
        found = false;
        for (i=0; i<n; i++)
        {
            Query_Quantifier* Q = mQuantifier[i];
            if ( Q->QuantifierType() == QT_F )
            {
                if ( !found )
                {
                    VTrace << "<" << Q->NodeID() << ">";
                    found = true;
                }
                else
                    VTrace << ",<" << Q->NodeID() << ">";
            }
        }
        VTrace << FlushLine;

        found = false;
        for (i=0; i<n; i++)
        {
            Query_Quantifier* Q = mQuantifier[i];
            if (   Q->QuantifierType() == QT_A 
                || Q->QuantifierType() == QT_E 
                || Q->QuantifierType() == QT_S )
            {
                if ( !found )
                {
                    VTrace << "               WHERE <" << Q->NodeID() << ">";
                    found = true;
                }
                else
                    VTrace << ",<" << Q->NodeID() << ">";
            }
        }
        VTrace << FlushLine;
    }
    PrintChildNodesToTrace(VTrace);
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::IsFromSelect( void )
{
	if ( mFatherNode && mFatherQuantifier )
	{
		if ( mFatherNode->GetNodeType() == Select &&
			mFatherQuantifier->QuantifierType() == QT_F )
			return true;
	}
	return false;
}

bool Query_SelectNode::IsFromSelectOrPartOfSet( void )
{
	if ( mFatherNode && mFatherQuantifier )
	{
		if ( mFatherNode->IsSelectOrSet() &&
			mFatherQuantifier->QuantifierType() == QT_F )
			return true;
	}
	return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::Parse( bool ComplexViewReplacement )
{
    {
        SAPDB_Int4 StackFree = RTE_CurrentStackFree();

        if ( StackFree < 4096 )
        {
            mQueryGraph.SetError("Stack exhausted", /*PrintToTrace=*/false);
            return false;
        }
    }

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo N = mInternAPNode;

    if ( SyntaxTree[N].n_proc == a63query_spec )
    {
        //if ( SyntaxTree[mInternAPNode].n_querycheck.includes(qcn_SpecialJoinFromSelect) )
        //    mSpecialJoinFromSelect = true;

	    SyntaxTreeNodeNo H = SyntaxTree[N].n_sa_level;
    	if ( H > 0 )
	    	if (    SyntaxTree[H].n_proc == no_proc
                 && SyntaxTree[H].n_symb == s_identifier )
            {
                mRefNameNode = H;
                mQueryGraph.GetIdentifier(mRefNameNode, mReferenceName);
            }
    }

    N = SyntaxTree[N].n_lo_level; 
    if ( SyntaxTree[N].n_proc == a63 && SyntaxTree[N].n_subproc() == cak_x_distinct )
    {
        mDistinct         = true;
        mHeadDistinct     = true;       // DDT ???
        mBodyDistinctType = DT_Enforce; // DDT ???
        N = SyntaxTree[N].n_lo_level; 
    }

    if ( mIsDeclareCursor )
    {
        mCursorNameNo = N;
        N = SyntaxTree[N].n_sa_level; 
    }

    bool rc = true;
    while (N>0 && rc)  // DDT : kein Loop -> schrittweise !
    {
        switch ( SyntaxTree[N].n_proc )
        {
        case a51:
            switch ( SyntaxTree[N].n_subproc() )
            {
            case cak_i_isolation:
                rc &= ParseLockOption(N);  
                break;
            default: 
                mQueryGraph.SetError("Parse - unknown subproc(a51)", N); 
                SAPDBERR_ASSERT_STATE( 0 );
                return false; break;
            }
            break;
        case a60:
            switch ( SyntaxTree[N].n_subproc() )
            {
            case cak_x_select_list:         
                rc &= ParseSelectList(N, ComplexViewReplacement); 
                break;
            case cak_x_single_select: 
                rc &= ParseSelectInto(N);
                break;
		    case cak_x_lock_nowait:         
		    case cak_x_lock_exclusive:      
		    case cak_x_lock_share:          
		    case cak_x_lock_optimistic:    
		    case cak_x_lock_ignore:     
                rc &= ParseLockOption(N); 
			    break;
			case cak_x_limit:
			case cak_x_top_percent:
			case cak_x_top_ties:
				mLimitStartNo = N;
				mQueryGraph.SetNotYetImplemented("cak_x_limit");
				return false;
				break;
            default: 
                mQueryGraph.SetError("Parse - unknown subproc(a60)", N); 
                SAPDBERR_ASSERT_STATE( 0 );
                return false; break;
            }
            break;
        case a63:
            switch ( SyntaxTree[N].n_subproc() )
            {
            case cak_x_from_part:           
                rc &= ParseFromPart(N, ComplexViewReplacement);   
                break;
            case cak_x_search_condition:    
                rc &= ParseWhere(N, ComplexViewReplacement);      
                break;
            case cak_x_having:              
                 rc &= ParseHaving(N, ComplexViewReplacement);     
                break;
            case cak_x_group_by:            
                 rc &= ParseGroupBy(N, ComplexViewReplacement);    
                break;
            case cak_x_order:               
                 rc &= ParseOrderBy(N, ComplexViewReplacement);    
                break;
            case cak_x_for_update:          
                 rc &= ParseForUpdate(N);  
                break;

            case cak_x_for_fetch_only:
                SAPDBERR_ASSERT_STATE( 0 );
                break;
            case cak_x_for_reuse: 
                mForReuse = true;
                break;

            case cak_x_res_delete:   
                SAPDBERR_ASSERT_STATE( mIsDeclareCursor );
                mResDelete = QTRUE;
                break;
            case cak_x_res_no_delete:     
                SAPDBERR_ASSERT_STATE( mIsDeclareCursor );
                mResDelete = QFALSE;
                break;
            default: 
                mQueryGraph.SetError("Parse - unknown subproc(a63)", N); 
                SAPDBERR_ASSERT_STATE( 0 );
                return false; break;
            }
            break;
        case a66:
            switch ( SyntaxTree[N].n_subproc() )
            {
            case cak_x_given_sequence:
                mGivenSequence=true; 
                break;
            default: 
                mQueryGraph.SetError("Parse - unknown subproc(a66)", N); 
                SAPDBERR_ASSERT_STATE( 0 );
                return false; break;
            }
            break;
        default: 
            mQueryGraph.SetError("Parse - unknown proc", N); 
            SAPDBERR_ASSERT_STATE( 0 );
            return false; break;
        }
        N = SyntaxTree[N].n_sa_level;
    }
    return rc && mQueryGraph.IsOk();
}

bool Query_SelectNode::ParseSelectList( SyntaxTreeNodeNo Start, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mSelectListStartNo = Start;

    mpOutputColumns = new(mAllocator)  Container_Vector<Query_OutputColumn*>(mAllocator);
    if ( !mpOutputColumns )
        return false;
    mpOutputColumns->Reserve(10);

    SyntaxTreeNodeNo N = SyntaxTree[Start].n_lo_level;
    while ( N>0 && mQueryGraph.IsOk() )
    {
        Query_OutputColumn* C = new(mAllocator) Query_OutputColumn();
        if ( C )
        {
            C->Parse(this, 0, P_SelectList, N, ComplexViewReplacement);
            mpOutputColumns->InsertEnd(C);
        }
        else
            return false;
        N = SyntaxTree[N].n_sa_level;
    }
    return true;
}

bool Query_SelectNode::ParseSelectInto( SyntaxTreeNodeNo Start )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mSelectIntoStartNo = Start;

    // DDT : TODO 

    return true;
}

bool Query_SelectNode::ParseFromPart( SyntaxTreeNodeNo Start, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mFromPartStartNo = Start;

    bool rc = true;
    SyntaxTreeNodeNo N = SyntaxTree[Start].n_lo_level;
    while ( N>0 && rc && mQueryGraph.IsOk() )
    {
        Query_Node* SubNode=0;
        if ( SyntaxTree[N].n_proc == a66 )
        {
            switch ( SyntaxTree[N].n_subproc() )
            {
            case cak_x_table_spec:
                if ( SubNode=AddTable(N, ComplexViewReplacement) )
                {
                    rc &= SubNode->Parse(ComplexViewReplacement);
                }
                else
                {
                    mQueryGraph.SetError("ParseFromPart1");
                    return false;
                }
                break;
            case cak_x_select_in_from_part:
                if ( SubNode=AddQuery(SyntaxTree[N].n_lo_level, P_FromPart, ComplexViewReplacement) )
                {
                    rc &= SubNode->Parse(ComplexViewReplacement);
                }
                else
                {
                    mQueryGraph.SetError("ParseFromPart2");
                    return false;
                }
                break;
            default: 
                mQueryGraph.SetError("ParseFromPart3");
                return false;
                break;
            }
        }
        else
        {
            mQueryGraph.SetError("ParseFromPart");
            return false;
        }
        N = SyntaxTree[N].n_sa_level;
    }
    return rc;
}

bool Query_SelectNode::ParseWhere( SyntaxTreeNodeNo Start, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mSearchConditionStartNo = Start;
    return mSearchConditionColumns.Parse(this, 0, P_Where, SyntaxTree[Start].n_lo_level, false, ComplexViewReplacement);
}

bool Query_SelectNode::ParseHaving( SyntaxTreeNodeNo Start, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mHavingStartNo = Start;
    return mHavingColumns.Parse(this, 0, P_Having, SyntaxTree[Start].n_lo_level, false, ComplexViewReplacement);
}

bool Query_SelectNode::ParseGroupBy( SyntaxTreeNodeNo Start, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mGroupByStartNo = Start;

    mpGroupByColumns = new(mAllocator)  Container_Vector<Query_ColumnSpec*>(mAllocator);
    if ( !mpGroupByColumns )
        return false;
    mpGroupByColumns->Reserve(10);

	SAPDB_Int2 Cnt = 0;
    SyntaxTreeNodeNo N = SyntaxTree[Start].n_lo_level;
    while ( N>0 && mQueryGraph.IsOk() )
    {
        Query_ColumnSpec* C = new(mAllocator) Query_ColumnSpec(N);
        if ( C )
        {
			SyntaxTreeNodeNo X = SyntaxTree[N].n_lo_level;
            C->Parse(this, 0, P_GroupBy, X, ComplexViewReplacement);
			if (   C->GetExpression().GetRoot() 
				|| (SyntaxTree[X].n_proc == no_proc && SyntaxTree[X].n_symb == s_unsigned_integer)
//				|| (SyntaxTree[X].n_proc == no_proc && SyntaxTree[X].n_symb == s_equal)
				)
			{
				++Cnt;
	            mpGroupByColumns->InsertEnd(C);
		        N = SyntaxTree[N].n_sa_level;
			}
			else
			{
				SyntaxTreeNodeNo H = N;
				N = SyntaxTree[N].n_sa_level;
				SyntaxTree.UnlinkNode(H, /*SetUnused*/true);
				destroy(C, mAllocator);
			}
        }
        else
		{
            return false;
		}
    }
	if ( Cnt )
	{
	    mQueryGraph.SetNeedOptimization();
	}
	else
	{
        DestroyColumnSpec(mpGroupByColumns);
        SyntaxTree.UnlinkNode(GroupByStart(), /*SetUnused*/true);
        SetGroupByStart(cak_qrewrite_ref_init);
	}
    return true;
}

bool Query_SelectNode::ParseOrderBy( SyntaxTreeNodeNo Start, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mOrderByStartNo = Start;

	if ( mSubQueryType == SQT_EXISTS )
	{
		// we don't need an order by !
        SyntaxTree.UnlinkNode(OrderByStart(), /*SetUnused*/true);
        SetOrderByStart(cak_qrewrite_ref_init);
		return true;
	}

    mpOrderByColumns = new(mAllocator)  Container_Vector<Query_ColumnSpec*>(mAllocator);
    if ( !mpOrderByColumns )
        return false;
    mpOrderByColumns->Reserve(10);

	SAPDB_Int2 Cnt = 0;
    SyntaxTreeNodeNo N = SyntaxTree[Start].n_lo_level;
    while ( N>0 && mQueryGraph.IsOk() )
    {
        Query_ColumnSpec* C = new(mAllocator) Query_ColumnSpec(N);
        if ( C )
        {
			SyntaxTreeNodeNo X = SyntaxTree[N].n_lo_level;
            C->Parse(this, 0, P_OrderBy, X, ComplexViewReplacement);
			if (   C->GetExpression().GetRoot() 
				|| (SyntaxTree[X].n_proc == no_proc && SyntaxTree[X].n_symb == s_unsigned_integer)
				|| (SyntaxTree[X].n_proc == no_proc && SyntaxTree[X].n_symb == s_equal)
				)
			{
				++Cnt;
	            mpOrderByColumns->InsertEnd(C);
		        N = SyntaxTree[N].n_sa_level;
			}
			else
			{
				SyntaxTreeNodeNo H = N;
				N = SyntaxTree[N].n_sa_level;
				SyntaxTree.UnlinkNode(H, /*SetUnused*/true);
				destroy(C, mAllocator);
			}
        }
        else
		{
            return false;
		}
    }
	if ( Cnt )
	{
	    mQueryGraph.SetNeedOptimization();
	}
	else
	{
        DestroyColumnSpec(mpOrderByColumns);
        SyntaxTree.UnlinkNode(OrderByStart(), /*SetUnused*/true);
        SetOrderByStart(cak_qrewrite_ref_init);
	}
    return true;
}

bool Query_SelectNode::ParseForUpdate( SyntaxTreeNodeNo Start )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    mForUpdateColumnStartNo = Start;

    SyntaxTreeNodeNo N = SyntaxTree[Start].n_lo_level;
    while ( N>0 && mQueryGraph.IsOk() )
    {
        // DDT 

        N = SyntaxTree[N].n_sa_level;
    }

    mQueryGraph.SetError("ParseForUpdate");
    return false;
}

bool Query_SelectNode::ParseLockOption( SyntaxTreeNodeNo Start )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
#if defined( SAPDB_SLOW )
    if ( mLockOptionStart>0 )
    {
        mQueryGraph.SetError("ParseLockOption");     
        SAPDBERR_ASSERT_STATE( 0 );
    }
#endif

    mLockOptionStart = Start;
    
    // DDT : TODO

    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics", QueryRewrite_Trace, 5);

    bool SelectPrivilegesForAllColumns  = true;
    bool HasAsteriskInSelectList        = false;

    if ( !mQueryGraph.IsOk() )
	{
		mQueryGraph.SetError("Semantics : !IsOk");
        return false;
	}

	if ( !Semantics_FAndLQuantifier(UserId, SelectPrivilegesForAllColumns, ComplexViewReplacement) )
	{
		mQueryGraph.SetError("Semantics_FAndLQuantifier failed");
        return false;
	}

    if ( !Semantics_SelectList(ComplexViewReplacement, HasAsteriskInSelectList) )
	{
		mQueryGraph.SetError("Semantics_SelectList failed");
        return false;
	}

//    if ( mPosition != P_Start )
        if ( !ResolveAllAsteriskForAllQuantifiers(/*IncRefCounts=*/true) )
        {
		    mQueryGraph.SetError("Resolve Asterisk failed");
            return false;
        }

	if (   UniqueOutputColumnNames 
        && !mSpecialJoinFromSelect
        && !ComplexViewReplacement )    // the CREATE VIEW statements insures that there are no duplicate column names !
    {
        ResolveAllMultiAsteriskIntoSingleAsterisk();
        if ( !Semantics_CheckUniqueOutputColumns() )
        {
            mQueryGraph.SetError("Semantics_CheckUniqueOutputColumns failed");
            return false;
        }
    }

    if ( !Semantics_WherePart() )
	{
		mQueryGraph.SetError("Semantics_WherePart failed");
        return false;
	}
    if ( !Semantics_Having() )
	{
		mQueryGraph.SetError("Semantics_Having failed");
        return false;
	}
    if ( !Semantics_GroupBy() )
	{
		mQueryGraph.SetError("Semantics_GroupBy failed");
        return false;
	}
    if ( !Semantics_OrderBy() )
	{
		mQueryGraph.SetError("Semantics_OrderBy failed");
        return false;
	}
	if ( !Semantics_UpdateColumns() )
	{
		mQueryGraph.SetError("Semantics_UpdateColumns failed");
        return false;
	}

    if ( !Semantics_CheckAgainstGroupBy() )
    {
		mQueryGraph.SetError("Semantics_CheckAgainstGroupBy failed");
        return false;
    }

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    if ( mJoinType == JT_Unknown )
    {
        if ( mFQuantifierCnt == 1 )
            mJoinType = JT_NoJoin;
        else
            mJoinType = JT_Inner;
    }

	mSemanticsDone = true;

    // check for scalar subquery
    if ( mPosition == P_SelectList || mPosition == P_OrderBy || mPosition == P_GroupBy )
    {
        if ( mOutputColumnCnt != 1 )
        {
            mQueryGraph.SetError("Number of Outputcolumns varies from 1");
            return false;
        }
    }

    if ( !Semantics_OtherQuantifier(UserId, ComplexViewReplacement) )
    {
		mQueryGraph.SetError("Semantics_OtherQuantifier failed");
        return false;
    }

	if ( mUnidentifiedColumns )      
	{
  		if ( Semantics_Correlation() )
		{
			if ( mUnidentifiedColumns )
			{
				mQueryGraph.SetError("UnidentiedColumns");
				return false;
			}
		}
		else
		{
			mQueryGraph.SetError("Semantics_Correlation failed");
			return false;
		}
	}

    if ( !SelectPrivilegesForAllColumns && HasAsteriskInSelectList )
    {
        ResolveAllAsteriskForQuantifiersWithoutAllSelectPriv();
    }

    if ( mPosition==P_Having && mAggregatesNotInOutput )
        mQueryGraph.SetNotYetImplemented("HAVING+SubQuery");    // DDT

    mSimplifiedPredicates = !(HasSearchCondition() || HasHaving());
	mSubqueryOptimized    = ! IsSubquery();
    mOrderByEliminated    = !(HasOrderBy() && HasGroupBy());
    mProjectionPulledDown   =  false;
    if ( !mOrderByEliminated )
        mQueryGraph.SetNeedOptimization();

	return (true);
}

bool Query_SelectNode::Semantics_FAndLQuantifier( SQLMan_Surrogate UserId, bool& SelectPrivilegesForAllColumns,  bool ComplexViewReplacement )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_FromPart", QueryRewrite_Trace, 5);

    bool Dummy = true;
    if ( Semantics_Quantifier(UserId, true, SelectPrivilegesForAllColumns, ComplexViewReplacement, QT_F) )
        if ( Semantics_Quantifier(UserId, false, Dummy, ComplexViewReplacement, QT_L) )
            return true;
    return false;
}

bool Query_SelectNode::Semantics_Quantifier( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool& SelectPrivilegesForAllColumns,  bool ComplexViewReplacement, Query_QuantifierType QuantifierType )
{
    SelectPrivilegesForAllColumns = true;
    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
    {
        Query_Quantifier* Q = mQuantifier[i];
        if ( !Q->IsDisabled() && Q->QuantifierType() == QuantifierType )
        {
            if ( Q->NodeRef()->Semantics(UserId, UniqueOutputColumnNames, ComplexViewReplacement) )
            {
                SelectPrivilegesForAllColumns &= Q->NodeRef()->SelectPrivilegesForAllColumns();
            	// check for duplicate names
                if ( Q->QuantifierType() == QT_F )
                {
                    for ( SAPDB_Int2 k=0; k<i; k++ )
                    {
                        Query_Quantifier* Q2 = mQuantifier[k];
                        if ( !Q2->IsDisabled() && Q2->QuantifierType() == QT_F )
                        {
                            if ( Q->IqualIdentificationNames(Q2) )
							{
								mQueryGraph.SetError("Semantics_Quantifier :  IqualIdentificationNames");
                                return false;
							}
                        }
                    }
                }
            }
            else
            {
				mQueryGraph.SetError("Semantics_Quantifier : Semantics of Subnode failed");
                return false;
		    }
        }
    }
    return true;
}

bool Query_SelectNode::Semantics_OtherQuantifier( SQLMan_Surrogate UserId, bool ComplexViewReplacement )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_EAndAQuantifier", QueryRewrite_Trace, 5);

    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
    {
        Query_Quantifier* Q = mQuantifier[i];
        if ( !Q->IsDisabled() && !Q->NodeRef()->SemanticsDone() )
        {
            if ( !Q->NodeRef()->Semantics(UserId, /*Unique=*/false, ComplexViewReplacement) )
            {
			    mQueryGraph.SetError("Semantics of Subnode failed");
                return false;
            }
        }
    }
    return true;
}

bool Query_SelectNode::Semantics_SelectList( bool ComplexViewReplacement, bool& HasAsterisk )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_SelectList", QueryRewrite_Trace, 5);

	// 1. check if each select-colum occurs in exactly one quantifier
    // 2. check if the output column names are unique if it is a fromselect
    //    or the left select of an union/intersect/difference
    // exactly one time within one of the from-quantifiers and add
    // information about the quantifier to the column-node

    HasAsterisk = false;
    mOutputColumnCnt = 0;

    bool rc=true;
    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Query_OutputColumn* Col = (*mpOutputColumns)[i];
        rc &= Col->Semantics(this, /*MayBeCorrelated*/true, /*ForOrderBy*/false);
        mOutputColumnCnt += Col->GetColumnCount();
        HasAsterisk      |= Col->IsAsterisk();
    }
	return rc && (mOutputColumnCnt > 0);
}

bool Query_SelectNode::Semantics_WherePart( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_WherePart", QueryRewrite_Trace, 5);

	// determine to which quantifier the column belongs
    // only those columns may be changes where no informations has been determiend before
    // in case of correlation for some of the columns the information will
    // be added when handling the outer select

    return mSearchConditionColumns.Semantics(this, /*MayBeCorrelated*/true, /*ForOrderBy*/false);
}

bool Query_SelectNode::Semantics_Having( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_Having", QueryRewrite_Trace, 5);

    return mHavingColumns.Semantics(this, /*MayBeCorrelated*/true, /*ForOrderBy*/false);
}

bool Query_SelectNode::Semantics_Correlation( void )
{
    bool ok = true;
    Query_Node* FNode = this->FatherNode();
    if ( mUnidentifiedColumns && Position()!=P_FromPart && Position()!=P_Start && FNode->GetNodeType() == Query_Node::Select )
    {
        mUnidentifiedColumns = false;
	    if ( !mSearchConditionColumns.IsEmpty() )
		    ok &= mSearchConditionColumns.SemanticsCorrelation(this, mUnidentifiedColumns);
	    if ( !mHavingColumns.IsEmpty() )
		    ok &= mHavingColumns.SemanticsCorrelation(this, mUnidentifiedColumns);
    }
	return ok;
}

bool Query_SelectNode::Semantics_GroupBy( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_GroupBy", QueryRewrite_Trace, 5);

    bool rc=true;
    if ( mpGroupByColumns )
    {
        SAPDB_Int2 n = mpGroupByColumns->GetSize();
        for ( SAPDB_Int2 i=0; (i<n) && rc; i++ )
            rc &= (*mpGroupByColumns)[i]->Semantics(this, /*MayBeCorrelated*/false, /*ForOrderBy*/false);
    }
    return rc;
}
 
bool Query_SelectNode::Semantics_CheckAgainstGroupBy( void )
{
    if ( mAggregatesInOutput || mAggregatesNotInOutput ) // DDT: mCorrelatedAndAggregatedColumnsRefersToThis ????
    {
		if ( HasForUpdate() )	// group by with update ? is that possible ??
			return false;

        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* OC = (*mpOutputColumns)[k];
            if ( !Semantics_CheckAgainstGroupBy( OC->GetExpression() ) )
                return false;
        }

		//if ( !Semantics_CheckAgainstGroupBy( mSearchConditionColumns ) )
		//    return false;
		if ( !Semantics_CheckAgainstGroupBy( mHavingColumns ) )
            return false;
        if ( !Semantics_CheckAgainstGroupBy( mpOrderByColumns ) )
            return false;
        //if ( !Semantics_CheckAgainstGroupBy( mUpdateColumns ) )
        //    return false;
	}
    return true;
}

bool Query_SelectNode::Semantics_CheckAgainstGroupBy( Container_Vector<Query_ColumnSpec*>*& pColSpec )
{
    bool rc=true;
    if ( pColSpec )
    {
        SAPDB_Int2 n=pColSpec->GetSize();
        for ( SAPDB_Int2 i=0; (i<n) && rc; i++ )
            rc &= Semantics_CheckAgainstGroupBy((*pColSpec)[i]->GetExpression());
    }
    return rc;
}


bool Query_SelectNode::Semantics_CheckAgainstGroupBy( Query_Expression& Expr )
{
    Query_ExpressionNode* pCol = Expr.GetRoot();
    while ( pCol )
    {
        if ( pCol->GetExpType() == Exp_Column )
        {
            Query_Column* C = (Query_Column*)pCol;
            if ( !C->InAggr() )
            {
                if ( !Semantics_CheckCheckIfGroupByColumn(C) )
                    return false;
            }
        }
        pCol = pCol->mNext;
    }
    return true;
}

bool Query_SelectNode::Semantics_CheckCheckIfGroupByColumn( Query_Column* C )
{
    if ( mpGroupByColumns )
    {
        SAPDB_Int2 n = mpGroupByColumns->GetSize();
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            Query_ExpressionNode* pCol = (*mpGroupByColumns)[i]->GetExpression().GetRoot();
            while ( pCol )
            {
                if ( pCol->GetExpType() == Exp_Column )
                {
                    Query_Column* C2 = (Query_Column*)pCol;
                    if (   C2->GetQuantifierID()  == C->GetQuantifierID()
                        && C2->GetLogicalColumnIndex() == C->GetLogicalColumnIndex() )
                        return true;
                }
                pCol = pCol->mNext;
            }
        }
    }
    return false;
}

bool Query_SelectNode::Semantics_OrderBy( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_OrderBy", QueryRewrite_Trace, 5);

    bool rc=true;
    if ( mpOrderByColumns )
    {
        SAPDB_Int2 n = mpOrderByColumns->GetSize();
        for ( SAPDB_Int2 i=0; (i<n) && rc; i++ )
            rc &= (*mpOrderByColumns)[i]->Semantics(this, /*MayBeCorrelated*/false, /*ForOrderBy*/true);
    }
    return rc;
}

bool Query_SelectNode::Semantics_UpdateColumns( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_UpdateColumns", QueryRewrite_Trace, 5);

    return mUpdateColumns.Semantics(this, /*MayBeCorrelated*/false, /*ForOrderBy*/true);
}

// ------------------------------------------------------------------------------------------------

SAPDB_Int2 Query_SelectNode::Semantics_FindOrderColumnInSelectList( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, SAPDB_Int2& OutputCol, bool& IsNullable, bool TopNode )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_FindOrderColumnInSelectList", QueryRewrite_Trace, 5);

    if ( mpOutputColumns )
    {
        bool AllowOrgCols = !HeadDistinct() && TopNode;

    	SAPDB_Int2 found = 0;
        SAPDB_Int2 cn = mpOutputColumns->GetSize();
        SQLMan_Identifier BlankIdentifier(true);
        for ( SAPDB_Int2 i=0; i<cn; i++ )
        {
            Query_OutputColumn* Col = (*mpOutputColumns)[i];
            switch ( Col->GetMainColType() )
            {
            case Query_OutColType::OCTM_Column:
                {
                SQLMan_Identifier Name(false);
                Col->GetColumnName(Name);

 				if ( Col->GetReferenceName()  == Column 
 					 ||	( AllowOrgCols && Col->GetReferenceName().IsBlank()
 					                   && Name == Column ) )          // DDT
				{
                    QuantifierID         = Col->GetQuantifierID();
                    QuantifierSyntaxNode = ( AllowOrgCols ? Col->GetQuantifierSyntaxNode() : cak_qrewrite_ref_init);
                    LogColumnIndex       = ( AllowOrgCols ? Col->GetLogicalColumnIndex()   : i+1);
                    ExtColNo             = ( AllowOrgCols ? Col->GetExtColNo() : cak_qrewrite_ref_init);
                    IsNullable           = Col->IsNullable();
					++found;
				}
                }
                break;
            case Query_OutColType::OCTM_Aggregate:
            case Query_OutColType::OCTM_Subquery:
            case Query_OutColType::OCTM_Expression:
                if ( Col->GetReferenceName() == Column )
                {
                    QuantifierID            = cak_qrewrite_ref_init;
  					QuantifierSyntaxNode    = cak_qrewrite_ref_init;
					LogColumnIndex          = cak_qrewrite_ref_init;
					ExtColNo                = cak_qrewrite_ref_init;
                    IsNullable              = true;
					++found;
                }
                break;

            case Query_OutColType::OCTM_Asterisk:
                if ( AllowOrgCols )
                {
      		        SAPDB_Int2 FromTabNode = Col->GetQuantifierSyntaxNode();
  			        SAPDB_Int2 QNo=0;
  			        if ( GetQuantifierNoFromSyntaxNode(FromTabNode, QNo) )
  			        {
  				        Query_Quantifier* Q = mQuantifier[QNo];
  				        if ( Q->QuantifierType() == QT_F )
  				        {
  					        if ( Q->CheckForColumn(Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable) )
  						        ++found;
  				        }
  				        else
  				        {
  					        mQueryGraph.SetError("");
  				        }
  			        }
                    else
                    {
                        mQueryGraph.SetError("Semantics_FindOrderColumnInSelectList");
                    }
                }
                break;
            case Query_OutColType::OCTM_MultiAsterisk:
                if ( AllowOrgCols )
                {
//                    BlankIdentifier.SetBlank();
      		        found += Semantics_FindColumnInFromPart(BlankIdentifier, BlankIdentifier, Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
                }
                break;
			case Query_OutColType::OCTM_Unknown:
            default:
                mQueryGraph.SetError("Semantics_FindOrderColumnInSelectList");
                break;
            }
		}
		if ( found>1 )
		{
			// DDT : Column not unique
			QuantifierSyntaxNode = LogColumnIndex = ExtColNo = cak_qrewrite_ref_init;
			mQueryGraph.SetError("Order by : Duplicate Columname");
            return 8;
		}
        if ( !TopNode && found==0 )
        {
            // DDT : Column not in selectlist
			QuantifierSyntaxNode = LogColumnIndex = ExtColNo = cak_qrewrite_ref_init;
			mQueryGraph.SetError("Order by : Order column must be number or reference name");
            return 8;
        }
    	return found;
	}
    mQueryGraph.SetError("Order by : Order column must be number or reference name");
    SAPDBERR_ASSERT_STATE( 0 );
    return 0;
}

SAPDB_Int2 Query_SelectNode::Semantics_OutputColumnCntForMultiAsterisk( void )
{
    SAPDB_Int2 cnt = 0;
    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
		Query_Quantifier* Q = mQuantifier[i];
		if (  !Q->IsDisabled()
            && Q->QuantifierType() == QT_F )
		{
			cnt += Q->OutputColumns();
		}
    }
    return cnt;
}


bool Query_SelectNode::Semantics_FindTableInFromPart(const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, Query_Quantifier*& Q )
{
    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Q = mQuantifier[i];
        if (  !Q->IsDisabled()
            && Q->QuantifierType() == QT_F 
            && Q->CheckQuantifierName(Schema, Table) )
            return true;
    }
    Q = 0;
    return false;
}

SAPDB_Int2 Query_SelectNode::Semantics_FindColumnInFromPart( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_FindColumnInFromPart", QueryRewrite_Trace, 5);

	SAPDB_Int2 QSyntaxNode = cak_qrewrite_ref_init, QID = cak_qrewrite_ref_init, _ColIndex = cak_qrewrite_ref_init, _ExtColNo = cak_qrewrite_ref_init;
	SAPDB_Int2 found = 0;
    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Query_Quantifier* Q = mQuantifier[i];
        if (  !Q->IsDisabled() 
            && Q->QuantifierType() == QT_F )
        {
            if ( !Q->SpecialJoinFromSelect() )
            {
                if ( Q->CheckForColumn(Schema, Table, Column, QSyntaxNode, QID, _ColIndex, _ExtColNo, IsNullable) )
                {
                    QuantifierID = QID;
				    QuantifierSyntaxNode = QSyntaxNode;
				    LogColumnIndex = _ColIndex;
				    ExtColNo = _ExtColNo;
                    ++found;
                }
            }
            else
            {
                Query_SelectNode* S = (Query_SelectNode*)Q->NodeRef();

                if ( S->CheckForColumnSpecialJoinFromSelect(Schema, Table, Column, QSyntaxNode, QID, _ColIndex, _ExtColNo, IsNullable) )
                {
                    QuantifierID = QID;
				    QuantifierSyntaxNode = QSyntaxNode;
				    LogColumnIndex = _ColIndex;
				    ExtColNo = _ExtColNo;
                    ++found;
                }
            }
        }
    }
    if ( found>1 )
    {
        // DDT : Column not unique
		QuantifierSyntaxNode = LogColumnIndex = ExtColNo = cak_qrewrite_ref_init;
        IsNullable = true;
    }
	return found;
}

bool Query_SelectNode::Semantics_CheckUniqueOutputColumns( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::Semantics_CheckUniqueOutputColumns", QueryRewrite_Trace, 5);

    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Query_OutputColumn* Col1 = (*mpOutputColumns)[i];
        for ( SAPDB_Int2 k=i+1; k<n; k++ )
        {
            Query_OutputColumn* Col2 = (*mpOutputColumns)[k];
            if ( !Semantics_CheckUniqueOutputColumns(Col1, Col2) )
                return false;
        }
    }
    return true;
}

bool Query_SelectNode::Semantics_CheckUniqueOutputColumns( Query_OutputColumn* Col1, Query_OutputColumn* Col2 )
{
    SAPDBERR_ASSERT_ARGUMENT( Col1->GetMainColType()!=Query_OutColType::OCTM_MultiAsterisk 
                           && Col2->GetMainColType()!=Query_OutColType::OCTM_MultiAsterisk )
    bool IsNullable=true;

    SyntaxTreeNodeNo FromTabNode1 = Col1->GetQuantifierSyntaxNode();
    SyntaxTreeNodeNo FromTabNode2 = Col2->GetQuantifierSyntaxNode();

    if ( !Col1->IsAsterisk() && !Col2->IsAsterisk() )
    {
        if ( FromTabNode1==FromTabNode2 )   // Output from FQuantifier was already checked for uniqueness
            return true;
        return Col1->GetIdentificationName() != Col2->GetIdentificationName();
    }

    // At least one of Col1, Col2 is an asterisk! 

    if ( Col1->IsAsterisk() && Col2->IsAsterisk() ) 
    {
        if ( FromTabNode1==FromTabNode2 )
            return false;

		SAPDB_Int2 QNo1=0, QNo2=0;
		if (   GetQuantifierNoFromSyntaxNode(FromTabNode1, QNo1) 
            && GetQuantifierNoFromSyntaxNode(FromTabNode2, QNo2) )
		{
			Query_Quantifier* Q1     = mQuantifier[QNo1];
			Query_Quantifier* Q2     = mQuantifier[QNo2];
            SAPDB_UInt2 n1 = Q1->OutputColumns();

            for ( SAPDB_UInt2 i=1; i<=n1; i++ )
            {
                SQLMan_Identifier ColName1(false);
			    if ( Q1->GetOutputColumnName(i, ColName1, IsNullable) )
			    {
                    if ( !Semantics_CheckUniqueOutputColumns(ColName1, Q2) )
                        return false;
                }
			}
            return true;
        }
        return false;
    }

    if ( Col1->IsAsterisk() )
    {
        Query_OutputColumn* H = Col2;
        Col2 = Col1;
        Col1 = H;
        FromTabNode2 = FromTabNode1;
    } // Col1 refers to one output column now, Col2 to the *

    SQLMan_Identifier ColName1 = Col1->GetIdentificationName();
    SAPDB_Int2 QNo2=0;
	if ( GetQuantifierNoFromSyntaxNode(FromTabNode2, QNo2) )
	{
    	Query_Quantifier* Q2 = mQuantifier[QNo2];
        return Semantics_CheckUniqueOutputColumns(ColName1, Q2);
    }
    return false;
}

bool Query_SelectNode::Semantics_CheckUniqueOutputColumns( const SQLMan_Identifier& ColName, Query_Quantifier* Q )
{
    bool IsNullable=true;

    SAPDB_UInt2 n = Q->OutputColumns();
    for ( SAPDB_UInt2 i=1; i<=n; i++ )
    {
        SQLMan_Identifier ColName2(false);
		if ( Q->GetOutputColumnName(i, ColName2, IsNullable) )
		{
            if ( ColName == ColName2 )
                return false;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::AppendColumn( Query_Expression& pExpression, Query_Column Col )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::AppendColumn", QueryRewrite_Trace, 5);

    Query_Column* C = new(mAllocator) Query_Column(Col);
    if ( C )
    {
        pExpression.AddNodes(C);
        return true;
    }
    else
    {
        mQueryGraph.SetError("Query_SelectNode::AppendColumn");
        return false;
    }
}

bool Query_SelectNode::AppendOutputColumn ( SyntaxTreeNodeNo Node )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::AppendColumn", QueryRewrite_Trace, 5);

	if ( !mpOutputColumns )
    {
        mpOutputColumns = new(mAllocator)  Container_Vector<Query_OutputColumn*>(mAllocator);
        mpOutputColumns->Reserve(10);
    }
    if ( mpOutputColumns )
    {
        Query_OutputColumn* C = new(mAllocator) Query_OutputColumn(Node);
        if ( C )
        {
            mpOutputColumns->InsertEnd(C);
            return true;
        }
    }
    mQueryGraph.SetError("Query_SelectNode::AppendColumn");
    return false;
}

SAPDB_Int2 Query_SelectNode::OutputColumns( void )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    // mOutputColumnCnt may vary from mpOutputColumns->GetSize() when there are '*' within the select list!
    return mOutputColumnCnt;
}

bool Query_SelectNode::OutputColumnIsNullable( SAPDB_Int2 LogColIndex )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    SQLMan_Identifier _ColName(false), _RefName(false);
    SAPDB_Int2 _QuantifierSyntaxNode=0, _QuantifierID=0;
    SAPDB_Int2 _LogColumnIndex=0, _ExtColNo=0, _SelColNo=0;
    bool _IsNullable=false;

    if ( GetOutputColumn(LogColIndex, _ColName, _RefName, _QuantifierSyntaxNode, _QuantifierID, _LogColumnIndex, _ExtColNo, _SelColNo, _IsNullable) )
        return _IsNullable;

	SAPDBERR_ASSERT_STATE( 0 );
    return true; // in case of doubt it's nullable
}

bool Query_SelectNode::OutputContainsNullableColumns( void )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

	for ( SAPDB_Int2 i=1; i<=mOutputColumnCnt; i++ )
	{
		if ( OutputColumnIsNullable(i) )
			return true;
	}
	return false;
}

Query_OutputColumn* Query_SelectNode::GetOutputColumnPointer( SAPDB_Int2 LogColIndex )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone && mpOutputColumns );

    if ( LogColIndex>0 && LogColIndex<=mOutputColumnCnt && mpOutputColumns && mSemanticsDone )
    {
        SAPDB_Int2 pos = 1;
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            SAPDB_Int2 ColCnt       = C->GetColumnCount();
            if ( pos+ColCnt > LogColIndex )
                return C;
            pos += ColCnt;
        }
    }
    mQueryGraph.SetError("GetOutputColumnPointer");
    SAPDBERR_ASSERT_STATE( 0 );
    return 0;
}

bool Query_SelectNode::GetOutputColumnInternType( SAPDB_Int2 LogColIndex, Query_OutColType& Type, SyntaxTreeNodeNo& SyntaxNode )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone && mpOutputColumns );

    if ( LogColIndex>0 && LogColIndex<=mOutputColumnCnt && mpOutputColumns && mSemanticsDone )
    {
        SAPDB_Int2 pos = 1;
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            SAPDB_Int2 ColCnt       = C->GetColumnCount();
            if ( pos+ColCnt > LogColIndex )
            {
                Type = C->GetColType();
                SyntaxNode = C->GetSyntaxNode();
                return true;
            }
            pos += ColCnt;
        }
    }
    mQueryGraph.SetError("GetOutputColumnInternType");
    SAPDBERR_ASSERT_STATE( 0 );
    return false;
}

SAPDB_Int2 Query_SelectNode::GetOutputColumnSize( SAPDB_Int2 LogColIndex )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone && mpOutputColumns );

	SAPDB_Int2 i = LogColIndex;
    if ( i>0 && i<=mOutputColumnCnt && mpOutputColumns && mSemanticsDone )
    {
        SAPDB_Int2 pos = 1;
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            SAPDB_Int2 ColCnt = C->GetColumnCount();
            if ( pos+ColCnt > i )
            {
                switch ( C->GetMainColType() )
                {
                case Query_OutColType::OCTM_Column:
                case Query_OutColType::OCTM_Asterisk:
					{
                        SyntaxTreeNodeNo FromTabNode = C->GetQuantifierSyntaxNode();
						SAPDB_Int2 QNo=0;
						if ( GetQuantifierNoFromSyntaxNode(FromTabNode, QNo) )
						{
							Query_Quantifier* Q     = mQuantifier[QNo];
							SAPDB_Int2 LogColumnIndex = i-pos+1;
							return Q->GetOutputColumnSize(LogColumnIndex);
						}
						else
						{
							mQueryGraph.SetError("GetOutputColumnSize");
							SAPDBERR_ASSERT_STATE( 0 );
							return cak_qrewrite_ref_init;
						}
					}
                    break;
                case Query_OutColType::OCTM_Subquery:
                case Query_OutColType::OCTM_Expression:
                case Query_OutColType::OCTM_Aggregate: 
                    return cak_qrewrite_ref_init;
                    break;
                case Query_OutColType::OCTM_MultiAsterisk:
					{
						SAPDB_Int2 n = mQuantifier.GetSize();
						for ( SAPDB_Int2 i2=0; i2<n; i2++ )
						{
							Query_Quantifier* Q = mQuantifier[i2];
							if (  !Q->IsDisabled()
                                && Q->QuantifierType() == QT_F )
							{
								SAPDB_Int2 cn = Q->OutputColumns();
								if ( pos + cn > i )
								{
									SAPDB_Int2 LogColumnIndex = i-pos+1;
									return Q->GetOutputColumnSize(LogColumnIndex);
								}
								pos += cn;
							}
						}
                        SAPDBERR_ASSERT_STATE( 0 );
						return false;
					}            
                    break;
				case Query_OutColType::OCTM_Unknown:
                default:
                    mQueryGraph.SetError("GetOutputColumnSize");
                    SAPDBERR_ASSERT_STATE( 0 );
					return cak_qrewrite_ref_init;
                    break;
                }
            }
            pos += ColCnt;
        }
    }
    SAPDBERR_ASSERT_STATE( 0 );
    mQueryGraph.SetError("GetOutputColumnSize");
    return cak_qrewrite_ref_init;

}

bool Query_SelectNode::GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    SQLMan_Identifier RefName(false);
    SAPDB_Int2 _QuantifierSyntaxNode=0, _QuantifierID=0;
    SAPDB_Int2 _LogColumnIndex=0, _ExtColNo=0, _SelColNo=0;

    if ( GetOutputColumn(LogColIndex, ColName, RefName, _QuantifierSyntaxNode, _QuantifierID, _LogColumnIndex, _ExtColNo, _SelColNo, IsNullable) )
    {
        if ( !RefName.IsBlank() )
            ColName = RefName;
        return true;
    }
    SAPDBERR_ASSERT_STATE( 0 );
    return false;
}

bool Query_SelectNode::GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ExtColNo )
{
	int syskey = mFatherQuantifier->FromTableSpec() ? 1:0;
    ExtColNo = LogColIndex + syskey;
    return ( ExtColNo>0 && ExtColNo<=mOutputColumnCnt+syskey );
}

bool Query_SelectNode::GetOutputColumn( SAPDB_Int2 i, SQLMan_Identifier& ColName, SQLMan_Identifier& RefName, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, SAPDB_Int2& SelColNo, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone && mpOutputColumns );

    SelColNo = cak_qrewrite_ref_init; 
    if ( i>0 && i<=mOutputColumnCnt && mpOutputColumns && mSemanticsDone )
    {
        SAPDB_Int2 pos = 1;
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            SAPDB_Int2 ColCnt = C->GetColumnCount();
            if ( pos+ColCnt > i )
            {

                switch ( C->GetMainColType() )
                {
                case Query_OutColType::OCTM_Column:
                    QuantifierID            = C->GetQuantifierID();
					QuantifierSyntaxNode    = C->GetQuantifierSyntaxNode();
					LogColumnIndex          = C->GetLogicalColumnIndex();
					ExtColNo                = C->GetExtColNo();
                    SelColNo                = k;
					C->GetColumnName(ColName);
					RefName = C->GetReferenceName();
                    IsNullable              = C->IsNullable();
					return true;
                    break;
                case Query_OutColType::OCTM_Subquery:
                case Query_OutColType::OCTM_Expression:
                case Query_OutColType::OCTM_Aggregate: 
                    QuantifierID			= cak_qrewrite_ref_init;
                    QuantifierSyntaxNode    = cak_qrewrite_ref_function;
					LogColumnIndex          = cak_qrewrite_ref_init;
					ExtColNo                = cak_qrewrite_ref_init;
                    SelColNo                = k;
					ColName.SetBlank();
					RefName					= C->GetReferenceName();
                    IsNullable              = true;
					return true;
                    break;
                case Query_OutColType::OCTM_Asterisk:
					{
                        SyntaxTreeNodeNo FromTabNode = C->GetQuantifierSyntaxNode();
						SAPDB_Int2 QNo=0;
						if ( GetQuantifierNoFromSyntaxNode(FromTabNode, QNo) )
						{
							Query_Quantifier* Q     = mQuantifier[QNo];
                            QuantifierID            = Q->NodeID();
							QuantifierSyntaxNode    = Q->SyntaxNode();
							RefName.SetBlank();
                            SelColNo                = k;
							if ( Q->GetOutputColumnName(i-pos+1, ColName, IsNullable) )
							{
								LogColumnIndex = i-pos+1;
								return Q->GetOutputColumnExtNo(LogColumnIndex, ExtColNo);
							}
                            SAPDBERR_ASSERT_STATE( 0 );   // DDT : i was incorrect
							return false;
						}
						else
						{
							return false;
						}
					}
                    break;
                case Query_OutColType::OCTM_MultiAsterisk:
					{
						SAPDB_Int2 n = mQuantifier.GetSize();
						for ( SAPDB_Int2 i2=0; i2<n; i2++ )
						{
							Query_Quantifier* Q = mQuantifier[i2];
							if (  !Q->IsDisabled()
                                && Q->QuantifierType() == QT_F )
							{
								SAPDB_Int2 cn = Q->OutputColumns();
								if ( pos + cn > i )
								{
                                    QuantifierID            = Q->NodeID();
									QuantifierSyntaxNode    = Q->SyntaxNode();
									RefName.SetBlank();
                                    SelColNo                = k;
									if ( Q->GetOutputColumnName(i-pos+1, ColName, IsNullable) )
									{
										LogColumnIndex = i-pos+1;
										return Q->GetOutputColumnExtNo(LogColumnIndex, ExtColNo);
									}
								}
								pos += cn;
							}
						}
                        SAPDBERR_ASSERT_STATE( 0 );
						return false;
					}            
                    break;
				case Query_OutColType::OCTM_Unknown:
                default:
                    mQueryGraph.SetError("GetOutputColumn");
                    SAPDBERR_ASSERT_STATE( 0 );
                    break;
                }
            }
            pos += ColCnt;
        }
    }
    SAPDBERR_ASSERT_STATE( 0 );
    mQueryGraph.SetError("GetOutputColumn");
    return false;
}

bool Query_SelectNode::CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    // check if this select has an output column "Column"

	// DDT: TODO : only if the result must have unique output column names there may be only one matching column !
	// select * from t1,t2 may have duplicate column names and therefore
	// select * from t1, t2 where a=7 must through an error if t1 AND t2 contain a column 'a'
	// ERROR !!!

    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    QuantifierID = mID;
    QuantifierSyntaxNode = mAPNode;
    LogColumnIndex = 0;
	ExtColNo = mFatherQuantifier->FromTableSpec() ? 1:0;
    IsNullable = true;

    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Query_OutputColumn* Col = (*mpOutputColumns)[i];
        switch ( Col->GetMainColType() )
        {
        case Query_OutColType::OCTM_Column:
        case Query_OutColType::OCTM_Aggregate: 
        case Query_OutColType::OCTM_Subquery:
        case Query_OutColType::OCTM_Expression:
            ++LogColumnIndex;
			++ExtColNo;
            if ( Column == Col->GetIdentificationName() )
            {
                IsNullable = Col->IsNullable();
                Col->IncRefCount();
                return true;
            }
            break;
        case Query_OutColType::OCTM_Asterisk:
            {
                SAPDB_Int2 FromTabNode = Col->GetQuantifierSyntaxNode();
                SAPDB_Int2 N=0, QId=0, Idx=0, Ext=0;
				SAPDB_Int2 QNo=0;
				if ( GetQuantifierNoFromSyntaxNode(FromTabNode, QNo) )
				{
					if ( mQuantifier[QNo]->CheckForColumn(Column, N, QId, Idx, Ext, IsNullable) )
					{
						LogColumnIndex += Idx;
						ExtColNo += Idx;
						return true;
					}
					else
					{
						Idx = mQuantifier[QNo]->OutputColumns();
						LogColumnIndex += Idx;
						ExtColNo += Idx;
					}            
                }
                else
                {
                    return false;
                }
            }
            break;
        case Query_OutColType::OCTM_MultiAsterisk:
            {
                SAPDB_Int2 m = mQuantifier.GetSize();
                for ( SAPDB_Int2 k=0; k<m; k++ )
                {
                    if (  !mQuantifier[k]->IsDisabled()
                        && mQuantifier[k]->QuantifierType() == QT_F )
                    {
                        SAPDB_Int2 N=0, QId=0, Idx=0, Ext=0;
                        if ( mQuantifier[k]->CheckForColumn(Column, N, QId, Idx, Ext, IsNullable) )
                        {
                            LogColumnIndex += Idx;
						    ExtColNo += Idx;
                            return true;
                        }
                        else
                        {
						    Idx = mQuantifier[k]->OutputColumns();
                            LogColumnIndex += Idx;
						    ExtColNo += Idx;
                        }
                    }
                }
            }
            break;
		case Query_OutColType::OCTM_Unknown:
        default:
            mQueryGraph.SetError("CheckForColumn");
            break;
        }
    }
    return false;
}

bool Query_SelectNode::CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    // checks if this is described by (Schema,Table) and contains Column

    if ( CheckQuantifierName(Schema, Table) )
        return CheckForColumn(Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
    return false;
}

bool Query_SelectNode::CheckForColumnSpecialJoinFromSelect( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSpecialJoinFromSelect );
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    // checks if this contains a F-Quantifier that is described by (Schema,Table) and that contains Column

    SAPDB_Int2 n = mQuantifier.GetSize();
    SAPDB_Int2 cnt=0;
    SAPDB_Int2 LogColNo=0;
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Query_Quantifier* Q = mQuantifier[i];
        if (  !Q->IsDisabled() 
            && Q->QuantifierType() == QT_F )
        {
            if (   Q->CheckQuantifierName(Schema, Table)
                && Q->CheckForColumn(Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable) )
            {
                QuantifierID            = mID;
                QuantifierSyntaxNode    = mAPNode;
                LogColumnIndex         += LogColNo;
                GetOutputColumnExtNo(LogColumnIndex, ExtColNo);
                ++cnt;
            }
            else
            {
                LogColNo += Q->OutputColumns();
            }
        }
    }
    return (cnt==1);
}

bool Query_SelectNode::CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( Schema.IsBlank() )
    {
		if ( !Table.IsBlank() )
		{
			if ( mReferenceName == Table )
				return true;
			return false;
		}
		return true;
    }
    return false;
}

void Query_SelectNode::GetIdentificationNames( SQLMan_Identifier& Schema, SQLMan_Identifier& Table )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    Schema.SetBlank();
    Table  = mReferenceName;
}

bool Query_SelectNode::HasReferenceName( void )
{
	return ( !mReferenceName.IsBlank() );
}

bool Query_SelectNode::IncRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo )
{
	SAPDB_Int2 n = 0;

    if ( GetQuantifierNoFromQId(QId, n) )
    {
        mQuantifier[n]->NodeRef()->IncRefCountForOutputColumn(ColNo);
        return true;
    }
    return false;
}

bool Query_SelectNode::DecRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo )
{
	SAPDB_Int2 n = 0;

    if ( GetQuantifierNoFromQId(QId, n) )
    {
        mQuantifier[n]->NodeRef()->DecRefCountForOutputColumn(ColNo);
        return true;
    }
    return false;
}

bool Query_SelectNode::AddRefCountForOutputColumn( SAPDB_Int2 i, SAPDB_Int2 Count )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone && mpOutputColumns );

    if ( i>0 && i<=mOutputColumnCnt && mpOutputColumns && mSemanticsDone )
    {
        SAPDB_Int2 pos = 1;
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            SAPDB_Int2 ColCnt = C->GetColumnCount();
            if ( pos+ColCnt > i )
            {
                switch ( C->GetMainColType() )
                {
                case Query_OutColType::OCTM_Column:
                case Query_OutColType::OCTM_Aggregate: 
                case Query_OutColType::OCTM_Expression:
                case Query_OutColType::OCTM_Subquery:
                    C->AddRefCount(Count);
					return true;
                    break;
                case Query_OutColType::OCTM_MultiAsterisk:
                case Query_OutColType::OCTM_Asterisk:
    				return false;
                    break;
				case Query_OutColType::OCTM_Unknown:
                default:
                    mQueryGraph.SetError("DecRefCountForOutputColumn");
                    SAPDBERR_ASSERT_STATE( 0 );
                    break;
                }
            }
            pos += ColCnt;
        }
    }
    SAPDBERR_ASSERT_STATE( 0 );
    mQueryGraph.SetError("DecRefCountForOutputColumn");
    return false;
}

bool Query_SelectNode::IncRefCountForOutputColumn( SAPDB_Int2 i )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone && mpOutputColumns );

    if ( i>0 && i<=mOutputColumnCnt && mpOutputColumns && mSemanticsDone )
    {
        SAPDB_Int2 pos = 1;
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            SAPDB_Int2 ColCnt = C->GetColumnCount();
            if ( pos+ColCnt > i )
            {
                switch ( C->GetMainColType() )
                {
                case Query_OutColType::OCTM_Column:
                case Query_OutColType::OCTM_Aggregate: 
                case Query_OutColType::OCTM_Expression:
                case Query_OutColType::OCTM_Subquery:
                    C->IncRefCount();
					return true;
                    break;
                case Query_OutColType::OCTM_MultiAsterisk:
                case Query_OutColType::OCTM_Asterisk:
    				return false;
                    break;
				case Query_OutColType::OCTM_Unknown:
                default:
                    mQueryGraph.SetError("DecRefCountForOutputColumn");
                    break;
                }
            }
            pos += ColCnt;
        }
    }
    SAPDBERR_ASSERT_STATE( 0 );
    mQueryGraph.SetError("DecRefCountForOutputColumn");
    return false;
}

bool Query_SelectNode::DecRefCountForOutputColumn( SAPDB_Int2 i )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone && mpOutputColumns );

    if ( i>0 && i<=mOutputColumnCnt && mpOutputColumns && mSemanticsDone )
    {
        SAPDB_Int2 pos = 1;
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            SAPDB_Int2 ColCnt = C->GetColumnCount();
            if ( pos+ColCnt > i )
            {
                switch ( C->GetMainColType() )
                {
                case Query_OutColType::OCTM_Column:
                case Query_OutColType::OCTM_Aggregate: 
                case Query_OutColType::OCTM_Expression:
                case Query_OutColType::OCTM_Subquery:
                    C->DecRefCount();
					return true;
                    break;
                case Query_OutColType::OCTM_MultiAsterisk:
                case Query_OutColType::OCTM_Asterisk:
    				return false;
                    break;
				case Query_OutColType::OCTM_Unknown:
                default:
                    mQueryGraph.SetError("DecRefCountForOutputColumn");
                    break;
                }
            }
            pos += ColCnt;
        }
    }
    SAPDBERR_ASSERT_STATE( 0 );
    mQueryGraph.SetError("DecRefCountForOutputColumn");
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::SubstituteOneColForAsterisk(
	SyntaxTreeNodeNo& SelColNo,
    SAPDB_Int2 QID, 
	SyntaxTreeNodeNo  ColQuantNode,
    const SQLMan_Identifier& ColName,
	SAPDB_Int2 ColLogIndexQuant,
	SAPDB_Int2 ColExtColNoQuant,
	SAPDB_Int2 ColLogIndexOwn,
    SAPDB_Int2 ColCnt,
    SAPDB_Int2 ErrPos,
    bool IsNullable,
	bool CreateNewNode )
{
	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	SyntaxTreeNodeNo NewColIDNode = cak_qrewrite_ref_init;
	SyntaxTreeNodeNo NewA60Node   = cak_qrewrite_ref_init;
	if ( CreateNewNode ) // if not the first one
	{
		NewA60Node	 = SyntaxTree.GetFreeNode();
		NewColIDNode = SyntaxTree.GetFreeNode();
		if ( NewA60Node > 0 )
		{
			SyntaxTree[NewA60Node].n_refback  = SelColNo;
			SyntaxTree[NewA60Node].n_sa_level = SyntaxTree[SelColNo].n_sa_level;

			if ( SyntaxTree[NewA60Node].n_sa_level > 0 )
				SyntaxTree[SyntaxTree[NewA60Node].n_sa_level].n_refback = NewA60Node;
			SyntaxTree[SelColNo].n_sa_level = NewA60Node;
			SelColNo = NewA60Node;
		}
	}
	else
	{
		NewA60Node   = SelColNo;
		NewColIDNode = SyntaxTree[SelColNo].n_lo_level;
	}

	if ( (NewA60Node > 0) && (NewColIDNode > 0 ) )
	{
		SyntaxTree[NewA60Node  ].n_lo_level  = NewColIDNode;
		SyntaxTree[NewColIDNode].n_refback   = NewA60Node;

		SyntaxTree[NewA60Node].n_proc.becomes(a60);
		SyntaxTree[NewA60Node].n_subproc() = cak_x_select_column;
		SyntaxTree[NewA60Node].n_symb.becomes(s_unknown);
        SyntaxTree[NewA60Node].n_pos() = ErrPos;      // we need the errorpos !

//        SyntaxTree[NewColIDNode].n_querycheck.addElement(qcn_NeedAdjustment);
		SyntaxTree[NewColIDNode].n_proc.becomes(no_proc);
        SyntaxTree[NewColIDNode].n_symb.becomes(s_columnid);
		SyntaxTree[NewColIDNode].n_fromtabnode() = ColQuantNode;
		SyntaxTree[NewColIDNode].n_columnindex() = ColExtColNoQuant;
        SyntaxTree[NewColIDNode].n_pos() = ErrPos;      // we need the errorpos !
	}
	else
	{
		mQueryGraph.SetError("GetFreeNode");
		return false;
	}

    if ( ColExtColNoQuant == cak_qrewrite_ref_star )
    {
        ColLogIndexQuant = cak_qrewrite_ref_star;
    }

    Query_Column C = Query_Column(NewColIDNode);
//    C.SetInfo(mID, QID, ColQuantNode, ColName, ColLogIndexQuant, ColExtColNoQuant, IsNullable);
    C.SetInfo(-1, QID, ColQuantNode, ColName, ColLogIndexQuant, ColExtColNoQuant, IsNullable);
	C.ErrorPos() = ErrPos;
    C.SetInAggr(false);

	Query_OutputColumn* NewOutCol = new(mAllocator) Query_OutputColumn(NewA60Node);
    if ( NewOutCol )
    {
        NewOutCol->SetColType((ColExtColNoQuant!=cak_qrewrite_ref_star?Query_OutColType::OCTM_Column:Query_OutColType::OCTM_Asterisk), /*SubType*/0, ColCnt);
        NewOutCol->AppendColumn(mAllocator, C);

	    mpOutputColumns->Insert(ColLogIndexOwn, NewOutCol);

	    return true;
    }
    return false;
}

bool Query_SelectNode::ResolveAllAsteriskForQuantifiersWithoutAllSelectPriv( void )
{
    // note: this method is called only if there are not allselectpriv for at least one quantifier !!!
    //       if called otherwise a '*' referring to more than one table will always be resolved !!!

    if ( !ResolveAllMultiAsteriskIntoSingleAsterisk() )
        return false;

    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 i=n-1; i>=0; i-- )
    {
        SAPDBERR_ASSERT_STATE( (*mpOutputColumns)[i]->GetMainColType() != Query_OutColType::OCTM_MultiAsterisk )
        if ( (*mpOutputColumns)[i]->GetMainColType() == Query_OutColType::OCTM_MultiAsterisk )
            return false;

        if ( (*mpOutputColumns)[i]->GetMainColType() == Query_OutColType::OCTM_Asterisk )
        {
    		SyntaxTreeNodeNo ColQuantNode = (*mpOutputColumns)[i]->GetQuantifierSyntaxNode();
			SAPDB_Int2 QNo=0;
			if ( GetQuantifierNoFromSyntaxNode(ColQuantNode, QNo) )
			{
				Query_Quantifier* Q = mQuantifier[QNo];
                if ( !Q->NodeRef()->SelectPrivilegesForAllColumns() )
                {
                    if ( !ResolveSingleAsterisk(i,/*IncRefCounts=*/false) )
                        return false;                   
                }
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

bool Query_SelectNode::ResolveAllAsteriskForAllQuantifiers( bool IncRefCounts )
{
    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 i=n-1; i>=0; i-- )
    {
        if ( (*mpOutputColumns)[i]->IsAsterisk() )
        {
            if ( !ResolveSingleAsterisk(i, IncRefCounts) )
                return false;
        }
    }
    return true;
}

bool Query_SelectNode::ResolveSingleAsterisk( SAPDB_Int2 SelectListColNo, bool IncRefCounts )
{
	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	SAPDB_Int2 k = SelectListColNo;
    SQLMan_Identifier ColName(false);
    Query_OutputColumn* Col = (*mpOutputColumns)[k];
    SyntaxTreeNodeNo SelColNo = Col->GetSyntaxNode();
    SAPDB_Int2 AsteriskColCnt = Col->GetColumnCount();
    SAPDB_Int2 ErrPos = SyntaxTree[SelColNo].n_pos();
    bool IsNullable=true;

    switch ( Col->GetMainColType() )
    {
    case Query_OutColType::OCTM_Asterisk:
		{
            SyntaxTreeNodeNo ColQuantNode = Col->GetQuantifierSyntaxNode();
			SAPDB_Int2 QNo=0;
			if ( GetQuantifierNoFromSyntaxNode(ColQuantNode, QNo) )
			{
				DestroyOneOutputColumn(Col, k, false);

				Query_Quantifier* Q2 = mQuantifier[QNo];
				for ( SAPDB_Int2 i=0; i<AsteriskColCnt; i++ )
				{
					SAPDB_Int2 LogColIdx = i+1;
					SAPDB_Int2 ExtColNo = Q2->LogicalColNo2ExtColNo(LogColIdx);
                    SAPDB_Int2 QId = Q2->NodeID();
                    Q2->GetOutputColumnName(LogColIdx, ColName, IsNullable);
                    if ( IncRefCounts )
                        Q2->NodeRef()->IncRefCountForOutputColumn(LogColIdx);
					if ( !SubstituteOneColForAsterisk( SelColNo,
                                                QId,
												ColQuantNode,
                                                ColName,
												LogColIdx,
												ExtColNo+(Q2->NodeTypeSelectOrSet()?1:0), // DDT
												k+i,
                                                1,
                                                ErrPos,
                                                IsNullable,
												(i?true:false)) )
					{
						return false;
					}
				}
			}
			else
			{
				return false;
			}
		}
        break;
    case Query_OutColType::OCTM_MultiAsterisk:
		{
			DestroyOneOutputColumn(Col, k, false);

			SAPDB_Int2 c = 0;
		    SAPDB_Int2 n = mQuantifier.GetSize();

			for ( SAPDB_Int2 q=0; q<n; q++ )
			{
		        Query_Quantifier* Q = mQuantifier[q];
				if (  !Q->IsDisabled()
                    && Q->QuantifierType() == QT_F )
		        {
					SyntaxTreeNodeNo ColQuantNode	= Q->SyntaxNode();
					SAPDB_Int2 qc = Q->OutputColumns();
					for ( SAPDB_Int2 j=0; j<qc; j++ )
					{
						SAPDB_Int2 LogColIdx = j+1;
						SAPDB_Int2 ExtColNo = Q->LogicalColNo2ExtColNo(LogColIdx);
                        SAPDB_Int2 QId = Q->NodeID();
                        SQLMan_Identifier ColName;
                        Q->GetOutputColumnName(LogColIdx, ColName, IsNullable);
                        if ( IncRefCounts )
                            Q->NodeRef()->IncRefCountForOutputColumn(LogColIdx);
						if ( SubstituteOneColForAsterisk( SelColNo,
                                                    QId,
													ColQuantNode,
                                                    ColName,
													LogColIdx,
                                                    ExtColNo+(Q->NodeTypeSelectOrSet()?1:0), //DDT
													k+c,
                                                    1,
                                                    ErrPos,
                                                    IsNullable,
													(c?true:false)) )
						{
							++c;
						}
						else
						{
							return false;
						}
					}
				}
			}

			if ( c != AsteriskColCnt )
			{
				mQueryGraph.SetError("ColumnMismatch");
				return false;
			}
		}
        break;
	case Query_OutColType::OCTM_Column:
	case Query_OutColType::OCTM_Expression:
	case Query_OutColType::OCTM_Aggregate:
	case Query_OutColType::OCTM_Subquery:
	case Query_OutColType::OCTM_Unknown:
	default:
        SAPDBERR_ASSERT_STATE( 0 );
        break;
    }
    return true;
}

bool Query_SelectNode::ResolveAllMultiAsteriskIntoSingleAsterisk( void )
{
    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 i=n-1; i>=0; i-- )
    {
        if ( (*mpOutputColumns)[i]->GetMainColType() == Query_OutColType::OCTM_MultiAsterisk )
            if ( !ResolveOneMultiAsteriskIntoSingleAsterisk(i) )
                return false;
    }
    return true;
}

bool Query_SelectNode::ResolveOneMultiAsteriskIntoSingleAsterisk( SAPDB_Int2 SelectListColNo )
{
	SAPDB_Int2 k = SelectListColNo;

	if ( (*mpOutputColumns)[k]->GetMainColType() == Query_OutColType::OCTM_MultiAsterisk ) 
	{
		// the Asterisk stands for all columns from MORE THAN ONE quantifier

    	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
		SyntaxTreeNodeNo SelColNo = (*mpOutputColumns)[k]->GetSyntaxNode();
        SAPDB_Int2 ErrPos = SyntaxTree[SelColNo].n_pos();
		DestroyOneOutputColumn((*mpOutputColumns)[k], k, false);

		SAPDB_Int2 c = 0;
		SAPDB_Int2 n = mQuantifier.GetSize();

        SQLMan_Identifier BlankIdentifier;
//        BlankIdentifier.SetBlank();
		for ( SAPDB_Int2 q=0; q<n; q++ )
		{
		    Query_Quantifier* Q = mQuantifier[q];
			if (  !Q->IsDisabled() 
                && Q->QuantifierType() == QT_F )
		    {
				SyntaxTreeNodeNo ColQuantNode	= Q->SyntaxNode();
                SAPDB_Int2 QId = Q->NodeID();
                SAPDB_Int2 ColCnt = Q->OutputColumns();
				if ( SubstituteOneColForAsterisk( SelColNo,
                                                  QId,
											      ColQuantNode,
                                                  BlankIdentifier,
											      cak_qrewrite_ref_star,
                                                  cak_qrewrite_ref_star,
											      k+c,
                                                  ColCnt,
                                                  ErrPos,
                                                  /*IsNullable*/true,
											      (c?true:false)) )
				{
					++c;
				}
				else
				{
					return false;
				}
			}
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::CreateColRefNames( SyntaxTreeNodeNo& RefNameNodes )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

	if ( RefNameNodes <= 0 )
		return true;

	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	SAPDB_Int2 k = 0;

	while ( RefNameNodes > 0 )
	{
		SAPDBERR_ASSERT_ARGUMENT( k<mOutputColumnCnt );
		if ( k==mOutputColumnCnt )
		{
			// ERROR: more columns within RefNameNodes than OutputColumns ...
			return false;
		}

		if ( !(*mpOutputColumns)[k]->IsAsterisk() && (*mpOutputColumns)[k]->IsSingleColumn() )
		{
//            SyntaxTreeNodeNo ColQuantNode	= (*mpOutputColumns)[k].GetQuantifierSyntaxNode();
	        SyntaxTreeNodeNo SelColNo		= (*mpOutputColumns)[k]->GetSyntaxNode(); // DDT : GetColumnSyntaxNode

	        SyntaxTreeNodeNo Help			= SyntaxTree[SelColNo].n_lo_level;

            SQLMan_Identifier NewRefName(false);
	        if ( (SyntaxTree[Help].n_proc == no_proc) &&
			        (SyntaxTree[Help].n_symb == s_reference_name) )
	        {	// we already have a reference name

		        // just change the name
		        SyntaxTree[Help].n_pos()    = SyntaxTree[RefNameNodes].n_pos();
		        SyntaxTree[Help].n_length() = SyntaxTree[RefNameNodes].n_length();

                mQueryGraph.GetIdentifier(RefNameNodes, NewRefName);
		        (*mpOutputColumns)[k]->SetReferenceName(NewRefName);

                // and discard the other node
		        Help = RefNameNodes;
		        RefNameNodes = SyntaxTree[RefNameNodes].n_sa_level;
		        mQueryGraph.SyntaxTree().SetUnusedNode(Help, /*Recursive=*/false);
	        }
	        else	// were is no reference name yet
	        {
		        SyntaxTreeNodeNo Next = SyntaxTree[RefNameNodes].n_sa_level;

		        // link the RefNameNode
		        SyntaxTree[RefNameNodes].n_proc.becomes(no_proc);
		        SyntaxTree[RefNameNodes].n_symb.becomes(s_reference_name);

		        SyntaxTree[SelColNo    ].n_lo_level = RefNameNodes;
		        SyntaxTree[RefNameNodes].n_refback  = SelColNo;
		        SyntaxTree[RefNameNodes].n_sa_level = Help;
		        SyntaxTree[Help        ].n_refback  = RefNameNodes;

		        mQueryGraph.GetIdentifier(RefNameNodes, NewRefName);
		        (*mpOutputColumns)[k]->SetReferenceName(NewRefName);

		        RefNameNodes = Next;
	        }
	        ++k;
        }
        else
        {
            return false;
        }
	}  // while

	if ( (k!=mOutputColumnCnt) || (RefNameNodes>0) )
	{
		// ERROR
		return false;
	}

	return true;
}

bool Query_SelectNode::CreateColRefNamesAndResolveAsterisk( SyntaxTreeNodeNo& RefNameNodes )
{
    if ( RefNameNodes > 0 )
    {
        if ( ResolveAllAsteriskForAllQuantifiers(/*IncRefCounts=*/false) )
        {
            if ( CreateColRefNames(RefNameNodes) )
                return true;
        }
        return false;
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::ExchangeNodeIDForCorrColumnsOnMerge( SAPDB_UInt2 OldNodeID, SAPDB_UInt2 NewNodeID )
{
	if ( OldNodeID != NodeID() )
	{
		SAPDB_Int2 n=0;
		ExchangeNodeIDInOutputColumns(mpOutputColumns, OldNodeID, NewNodeID);
		
		mSearchConditionColumns.ExchangeNodeID(OldNodeID, NewNodeID, n);
		mHavingColumns.ExchangeNodeID(OldNodeID, NewNodeID, n);
	}

    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
    {
        Query_Quantifier* Q = mQuantifier[i];
        if ( Q->NodeRef()->GetNodeType() == Query_Node::Select )
        {
			Query_SelectNode* SNode = (Query_SelectNode*)(Q->NodeRef());
            if ( !SNode->ExchangeNodeIDForCorrColumnsOnMerge(OldNodeID, NewNodeID) )
			    mQueryGraph.SetError("ExchangeNodeIDForCorrColumnsOnMerge");
        }
    }
	return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::RenewSemanticsForColumnsDueToColumnPrivs( Query_Node* NewNode )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::RenewSemanticsForCorrelatedColumns", QueryRewrite_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    if ( NewNode )
    {
        if ( NewNode!=this )
            if ( !RenewSemanticsForMyColumnsDueToColumnPrivs(NewNode) )
                mQueryGraph.SetError("RenewSemantics failed");

        if ( mCorrelatedColumnsRefersToThis>0 )
        {
            SAPDB_Int2 n = mQuantifier.GetSize();
            for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
            {
                Query_Quantifier* Q2 = mQuantifier[i];
                if ( !Q2->IsDisabled() && Q2->QuantifierType() != QT_F )
                {
                    if ( !Q2->NodeRef()->RenewSemanticsForColumnsDueToColumnPrivs(NewNode) )
			            mQueryGraph.SetError("RenewSemantics of Subnode failed");
                }
            }
        }
    }
    return true;
}

bool Query_SelectNode::RenewSemanticsForCorrelatedColumnsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::RenewSemanticsForCorrelatedColumns", QueryRewrite_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    if ( NewNode )
    {
        if ( mCorrelatedColumns>0 && NewNode!=this )
            if ( !RenewSemanticsForMyColumnsOnMergeForCorrCols(OldQID, NewNode) )
                mQueryGraph.SetError("RenewSemantics failed");

        SAPDB_Int2 n = mQuantifier.GetSize();
        for ( SAPDB_Int2 i=0; (i<n) && mQueryGraph.IsOk(); i++ )
        {
            Query_Quantifier* Q2 = mQuantifier[i];
            if ( !Q2->IsDisabled() && Q2->QuantifierType() != QT_F )
            {
                if ( !Q2->NodeRef()->RenewSemanticsForCorrelatedColumnsOnMergeForCorrCols(OldQID, NewNode) )
			        mQueryGraph.SetError("RenewSemantics of Subnode failed");
            }
        }
    }
    return true;
}

// -----

bool Query_SelectNode::RenewSemanticsForMyColumnsDueToColumnPrivs( Query_Node* NewNode )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    if ( RenewSemanticsForMySelectListDueToColumnPrivs(NewNode) )
        if ( RenewSemanticsForColumnsDueToColumnPrivs(NewNode, mSearchConditionColumns) )
            if ( RenewSemanticsForColumnsDueToColumnPrivs(NewNode, mHavingColumns) )
                if ( RenewSemanticsForColumnSpecDueToColumnPrivs(NewNode, mpGroupByColumns) )  
                    if ( RenewSemanticsForColumnSpecDueToColumnPrivs(NewNode, mpOrderByColumns) ) 
                    	if ( RenewSemanticsForColumnsDueToColumnPrivs(NewNode, mUpdateColumns) )
                            return true;
    return false;
}

bool Query_SelectNode::RenewSemanticsForMyColumnsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    if ( RenewSemanticsForMySelectListOnMergeForCorrCols(OldQID, NewNode) )
        if ( RenewSemanticsForColumnsOnMergeForCorrCols(OldQID, NewNode, mSearchConditionColumns, P_Where) )
            if ( RenewSemanticsForColumnsOnMergeForCorrCols(OldQID, NewNode, mHavingColumns, P_Having) )
                return true;
    return false;
}

// -----

bool Query_SelectNode::RenewSemanticsForMySelectListDueToColumnPrivs( Query_Node* NewNode )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    bool rc = true;
    if ( mpOutputColumns )
    {
        SAPDB_Int2 cn = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 i=0; (i<cn) && rc; i++ )
        {
            Query_OutputColumn* Col = (*mpOutputColumns)[i];
            rc &= RenewSemanticsForColumnsDueToColumnPrivs(NewNode, Col->GetExpression());
        }
    }
    return rc;
}

bool Query_SelectNode::RenewSemanticsForMySelectListOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    bool rc = true;
    if ( mpOutputColumns )
    {
        SAPDB_Int2 cn = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 i=0; (i<cn) && rc; i++ )
        {
            Query_OutputColumn* Col = (*mpOutputColumns)[i];
            rc &= RenewSemanticsForColumnsOnMergeForCorrCols(OldQID, NewNode, Col->GetExpression(), P_SelectList);
        }
    }
    return rc;
}

// -----

bool Query_SelectNode::RenewSemanticsForColumnSpecDueToColumnPrivs( Query_Node* NewNode, Container_Vector<Query_ColumnSpec*>* pColSpec )
{
    bool rc = true;
    if ( pColSpec )
    {
        SAPDB_Int2 cn = pColSpec->GetSize();
        for ( SAPDB_Int2 i=0; (i<cn) && rc; i++ )
        {
            Query_ColumnSpec* Col = (*pColSpec)[i];
            rc &= RenewSemanticsForColumnsDueToColumnPrivs(NewNode, Col->GetExpression());
        }
    }
    return rc;
}

bool Query_SelectNode::RenewSemanticsForColumnSpecOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Container_Vector<Query_ColumnSpec*>* pColSpec, Query_Position Pos )
{
    bool rc = true;
    if ( pColSpec )
    {
        SAPDB_Int2 cn = pColSpec->GetSize();
        for ( SAPDB_Int2 i=0; (i<cn) && rc; i++ )
        {
            Query_ColumnSpec* Col = (*pColSpec)[i];
            rc &= RenewSemanticsForColumnsOnMergeForCorrCols(OldQID, NewNode, Col->GetExpression(), P_SelectList);
        }
    }
    return rc;
}

// -----

bool Query_SelectNode::RenewSemanticsForColumnsDueToColumnPrivs( Query_Node* NewNode, Query_Expression Expression )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    if ( Expression.GetRoot() )
        return Expression.GetRoot()->RenewSemanticsDueToColumnPrivs(NewNode);
    return true;
}

bool Query_SelectNode::RenewSemanticsForColumnsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Query_Expression Expression, Query_Position Pos )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );

    if ( Expression.GetRoot() )
        return Expression.GetRoot()->RenewSemanticsOnMergeForCorrCols(OldQID, NewNode, Pos);
    return true;
}

// -----

bool Query_SelectNode::RenewSemanticsForColumnDueToColumnPrivs( Query_Column* C )
{
    SAPDBERR_ASSERT_ARGUMENT( C );

    if ( C )
    {
        SAPDB_Int2 NewQSyntaxNode=0, NewQID=0, NewLogColumnIndex=0, NewExtColNo=0, NewSelColNo=0, NewNodeID=0;
        bool IsNullable=true;

        if ( !CheckForColumn(C->GetColumnName(), NewQSyntaxNode, NewQID, NewLogColumnIndex, NewExtColNo, IsNullable) )
        {
            mQueryGraph.SetError("RenewSemanticsForColumn 1");
            SAPDBERR_ASSERT_STATE(0);
            return false;
        }
		NewNodeID = C->GetNodeID();

        C->SetInfo(NewNodeID, NewQID, NewQSyntaxNode, C->GetColumnName(), NewLogColumnIndex, NewExtColNo, IsNullable);
    	if ( NewQSyntaxNode > 0 )
		{
    		// adjust the SyntaxNode attributes
            Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
            SyntaxTreeNodeNo H = C->GetSyntaxNode();
			SyntaxTree[H].n_fromtabnode()   = NewQSyntaxNode;
			SyntaxTree[H].n_columnindex()   = NewExtColNo;
        }
    }
    return true;
}

bool Query_SelectNode::RenewSemanticsForColumnOnMergeForCorrCols( Query_Column* C, Query_Position Pos )
{
    SAPDBERR_ASSERT_ARGUMENT( C );

    if ( C )
    {
//        SAPDB_Int2 NewQSyntaxNode=0, NewQID=0, NewLogColumnIndex=0, NewExtColNo=0, NewSelColNo=0;
//        SQLMan_Identifier ColName, RefName;
//        bool IsNullable=true;

//CorrCol        SAPDB_Int2 OldNodeID = C->GetNodeID();

        Query_OutColType ColType;
        Query_MappingInfo MappingInfo( ColType, MT_Normal, Pos, false);

        if ( !C->MappFromOuterToInnerSelect(this, &MappingInfo) )
            return false;
//CorrCol        C->SetNodeID(OldNodeID);

        if ( ColType.ContainsAggregate() && (Pos==P_Where || Pos==P_Having) )
        {
            // DDT 
            mQueryGraph.SetNotYetImplemented("Mapped to Aggregate for Correlated SearchColumn");
            mAggregatesNotInOutput = true;
            return false;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

//SyntaxTreeNodeNo Query_SelectNode::GetRefColNo( SyntaxTreeNodeNo ColNodeStart, SAPDB_Int2 ColIdx )
//{
//	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
//	SAPDB_Int2 cnt = 0;
//    while ( (ColNodeStart = SyntaxTree[ColNodeStart].n_sa_level) > 0 )
//    {
//        if ( ++cnt == ColIdx )
//			return ColNodeStart;
//    }
//	return cak_qrewrite_ref_init;
//}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::SetTempFileNameAndCounter( SAPDB_Int1 Level, SAPDB_Int2& Cnt, bool FatherIsUnion, SAPDB_Int2& FromSelCnt, SAPDB_Int2& ParsCnt, bool& ContainsCorrelatedSubquery, bool& ContainsFromSelectInSubquery, SAPDB_Int2& MaxSubqueryLevel )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( !(mFQuantifierCnt < Query_MaxFQuantifier) )    // DDT : ???
        return false;

    if ( Level > MaxSubqueryLevel && FatherQuantifier()->QuantifierType() != QT_F )
        MaxSubqueryLevel = Level;

    if ( IsCorrelatedSubquery() )
        ContainsCorrelatedSubquery = true;
    if ( mPosition == P_FromPart && mFatherNode->FatherQuantifier()->QuantifierType() != QT_F  )
        ContainsFromSelectInSubquery = true;

//    if ( mPosition != P_Start )
    {
        if ( ParsCnt < Query_MaxParsCnt )
            ++ParsCnt;
        else
            return false;
    }

    // Add a92fromsel or other hint node
    mQueryGraph.AddSubSelectNode(mInternAPNode, mPosition);

    if ( Cnt < Query_MaxSubqueriesPerLevel && Level < Query_MaxSubqueriesPerLevel )
    {
        if ( Level > 0 )
        {
            SAPDB_Int2 Name = 2 + Cnt + (Level<<8);
            mQueryGraph.SyntaxTree()[mInternAPNode].n_length() = Name;
        }

        if ( mPosition == P_FromPart )
        {
            if ( FromSelCnt < Query_MaxFromSelects )
                mQueryGraph.SyntaxTree()[mAPNode].n_pos() = ++FromSelCnt;
            else
                return false;
        }

//        SAPDB_Int2 n = (FatherIsUnion?Cnt:0);
		SAPDB_Int2 n = 0;
        bool rc = Query_Node::SetTempFileNameAndCounter(Level+1, n, false,FromSelCnt,ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel);
        //if ( FatherIsUnion )
        //    Cnt = n;
        return rc;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::RemoveSubSelectNode( SyntaxTreeNodeNo SubSelNode, Query_Position PosType, bool ReplaceByChildren )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::RemoveSubSelectNode", QueryRewrite_Trace, 6);

    return mQueryGraph.SyntaxTree().RemoveSubSelectNode(SubSelNode, /*WithChildren*/ false);
}

bool Query_SelectNode::AddSubSelectNode( SyntaxTreeNodeNo SelNode, SyntaxTreeNodeNo FatherNode, Query_Position FatherPosType )
{
    // Adds the already existing SubSelect node identified by SubSelNode as a subselect to FatherNode as children number Pos !

    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::AddSubSelectNode", QueryRewrite_Trace, 6);

    if ( mFatherNode->GetNodeType() == Select )
        return mFatherNode->AddSubSelectNode(SelNode, FatherNode, FatherPosType);

    return mQueryGraph.SyntaxTree().AddSubSelectNode(mInternAPNode, SelNode, FatherNode);
}

bool Query_SelectNode::RemoveGroupBy( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::RemoveGroupBy", QueryRewrite_Trace, 6);

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    // DDT : we must Decrement the column references !!!
    if ( mpGroupByColumns )
    {
        SAPDB_Int2 n = mpGroupByColumns->GetSize();
        for ( SAPDB_Int2 i=0; i<n; i++ )
        {
            Query_ExpressionNode* pCol = (*mpGroupByColumns)[i]->GetExpression().GetRoot();
            while ( pCol )
            {
                if ( pCol->GetExpType() == Exp_Column )
                {
                    Query_Column* C = (Query_Column*)pCol;
                    DecRefCountForOutputColumn(C->GetQuantifierID(), C->GetExtColNo());
                }
                pCol = pCol->mNext;
            }
        }

        DestroyColumnSpec(mpGroupByColumns);
        SyntaxTree.UnlinkNode(GroupByStart(), /*SetUnused*/true);
        SetGroupByStart(cak_qrewrite_ref_init);
    }
    return true;
}

bool Query_SelectNode::RemoveDistinctNode( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::RemoveDistinctNode", QueryRewrite_Trace, 6);

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo Dist = SyntaxTree[mInternAPNode].n_lo_level;
    if ( (SyntaxTree[Dist].n_proc == a63) &&
         (SyntaxTree[Dist].n_subproc() == cak_x_distinct) )
    {
        SyntaxTree[SyntaxTree[Dist].n_lo_level].n_refback = mInternAPNode;
        SyntaxTree[mInternAPNode].n_lo_level = SyntaxTree[Dist].n_lo_level;
        SyntaxTree[Dist].n_sa_level = 0;
        SyntaxTree[Dist].n_lo_level = 0;
        SyntaxTree[Dist].n_refback = 0;
        SyntaxTree.SetUnusedNode(Dist);
    }
    mDistinct = false;
    return true;
}

bool Query_SelectNode::AddDistinctNode( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::AddDistinctNode", QueryRewrite_Trace, 6);

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo Dist = SyntaxTree[mInternAPNode].n_lo_level;
    if ( ! (SyntaxTree[Dist].n_proc == a63 &&
            SyntaxTree[Dist].n_subproc() == cak_x_distinct) )
    {
        Dist = SyntaxTree.GetFreeNode();
        if ( Dist > 0 )
        {
            SyntaxTree[Dist].n_proc.becomes(a63);
            SyntaxTree[Dist].n_subproc()    = cak_x_distinct;

            SyntaxTree[Dist].n_lo_level     = SyntaxTree[mInternAPNode].n_lo_level;
            SyntaxTree[SyntaxTree[Dist].n_lo_level].n_refback = Dist;
            SyntaxTree[mInternAPNode].n_lo_level  = Dist;
            SyntaxTree[Dist].n_refback      = mInternAPNode;

            mDistinct = true;
            return true;
        }
        return false;
    }
    return true;
}

bool Query_SelectNode::AddGivenSequenceNode( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::AddGivenSequenceNode", QueryRewrite_Trace, 6);

    // DDT : select * from <t1, ( select * from t2,t3 )> 
    //       will be merged to select * from t1,t2,t3 that means that the geiven sequence <> will be missing after merging
    //       to have select * from <t1,t2,t3> after merging this method must be implemented

    //if ( !mGivenSequence )
    //{
    //    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    //    // add a66/cak_x_given_sequence between a60/cak_x_select_list and a63/cak_x_from_part

    //    SyntaxTreeNodeNo F = FromPartStart();
    //    SyntaxTreeNodeNo H = SyntaxTree[F].nrefback;
    //    if ( 

    //    todo !


    //    mGivenSequence = true;
    //    return true;
    //}
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::CompatibleAggregation( Query_Node* TargetNode )
{
    // check if outer and inner select have compatibe aggregations that they can be merged!

    // main rules if there are aggregates in both selects:
    //      - nothing must be done with the intermediate result 
    //      - the outer aggregation must be an "on going" aggregation of the inner one

    if ( TargetNode->GetNodeType() == Select )
    {

#if defined( SAPDB_SLOW )
        bool ThisAggregates =  Aggregates() 
                            || HasHaving() 
                            || HasGroupBy();
        bool TargetAggregates   =  TargetNode->Aggregates() 
                                || TargetNode->HasHaving() 
                                || TargetNode->HasGroupBy();
        SAPDBERR_ASSERT_STATE( ThisAggregates && TargetAggregates );
#endif

        // 1. inner select must not have a having clause
        if ( TargetNode->HasHaving() )
            return false;

        // 2. outer select must not have a qualification that refers to one of the columns from the inner select 
        if ( HasSearchCondition() )
        {
            if ( ContainsColumnFromQuantifierWhichIsAnAggregation(mSearchConditionColumns, TargetNode) )
                return false;
        }
        if ( HasHaving() )
        {
            if ( ContainsColumnFromQuantifierWhichIsAnAggregation(mHavingColumns, TargetNode) )
                return false;
            // DDT : too restricted
            // select max(x) from ( select max(b) x, max(b) y from tab ) having max(x) > 4  > allowed
            // select max(x) from ( select max(b) x, max(b) y from tab ) having max(x+1) > 4  > NOT allowed
            // select max(x) from ( select max(b) x, max(b) y from tab ) having count(*) > y  > (NOT) allowed
            // Columns from TargetNode are allowed as argument of aggregation only !
        }
        // 2a. Check OrderBy and UpdateColumns as well
        if ( HasOrderBy() )
        {
            if ( ContainsColumnFromQuantifierWhichIsAnAggregation(mpOrderByColumns, TargetNode) )
                return false;
        }
        if ( HasForUpdate() ) // for update with aggregation not allowed anyway
        {
            return false;
        }

        // 3. check if the target is a one-tuple-aggregation and if this is a join select 
        bool OneTupleAggregation = TargetNode->OneTupleAggregation();
        bool Joins = GetJoinType()!=JT_NoJoin;

		if ( !TargetNode->AggregatesInOutput() && TargetNode->HasGroupBy() )
		{
			// the following statement MUST NOT be merged !!!
			//SELECT a1, SUM(a3) FROM ( SELECT a1, a2, a3 FROM T8 GROUP BY a1, a2, a3 ) GROUP BY a1
			//gruppierungsspalten aus dem inneren select
			// - müssen außen aggregiert werden
			// - müssen außen gruppiert werden
			// - dürfen nicht vorkommen
			return false;
		}

        // 4. all group by columns from the outer select must be group by columns in the inner select
        //    this is equivalent to: all group by columns from the outer select must be normal output columns in
        //    the inner select DUE to the prio checked AggregatesInOutput() !
        if ( HasGroupBy() )
        {
            SAPDB_Int2 m=mpGroupByColumns->GetSize();
            for ( SAPDB_Int2 j=0; j<m; j++ )
            {
                Query_ExpressionNode* pCol = (*mpGroupByColumns)[j]->GetExpression().GetRoot();
                while ( pCol )
                {
                    if ( pCol->GetExpType() == Exp_Column )
                    {
                        Query_Column* C = (Query_Column*)pCol;
                        if ( C->GetQuantifierID() == TargetNode->NodeID() )
                        {
                            Query_OutColType ColType;
                            SyntaxTreeNodeNo H;
                            if ( TargetNode->GetOutputColumnInternType(C->GetLogicalColumnIndex(), ColType, H) )
                            {
                                switch ( ColType.MainType )
                                {
                                case Query_OutColType::OCTM_Column:
                                case Query_OutColType::OCTM_Asterisk:
                                case Query_OutColType::OCTM_MultiAsterisk:
                                case Query_OutColType::OCTM_Expression:
                                case Query_OutColType::OCTM_Subquery:
                                    // ok
                                    break;
                                case Query_OutColType::OCTM_Aggregate:
                                    return false;
                                    break;
                                case Query_OutColType::OCTM_Unknown:
                                    mQueryGraph.SetError("xyz");
                                    SAPDBERR_ASSERT_STATE(0);
                                    return false;
                                    break;
                                }
                            }
                            else
                            {
                                mQueryGraph.SetError("xyz");
                                SAPDBERR_ASSERT_STATE(0);
                                return false;
                            }
                        }
                    }
                    pCol = pCol->mNext;
                }
            }
        }
        
        // 5. the aggregate functions and their expressions within the select list must match: 
        //    sum/sum, min/min, max/max, min/col, max/col, (Sum/col)
        SAPDB_Int2 n = mpOutputColumns->GetSize();
        for ( SAPDB_Int2 k=0; k<n; k++ )
        {
            Query_OutputColumn* OC = (*mpOutputColumns)[k];
            Query_OutColType ColType = OC->GetColType();
            switch ( ColType.MainType )
            {
                case Query_OutColType::OCTM_Column:
                case Query_OutColType::OCTM_Asterisk:
                case Query_OutColType::OCTM_MultiAsterisk:
                case Query_OutColType::OCTM_Subquery:
                    // ok
                    break;
                case Query_OutColType::OCTM_Expression:
                case Query_OutColType::OCTM_Aggregate:
                    {
                        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
                        Query_ExpressionNode* pCol = OC->GetExpression().GetRoot();
                        if ( pCol )
                        {
                            while ( pCol )
                            {
                                if ( pCol->GetExpType() == Exp_Column )
                                {
                                    Query_Column* C = (Query_Column*)pCol;
                                    if ( C->GetQuantifierID() == TargetNode->NodeID() )
                                    {
                                        SyntaxTreeNodeNo H1 = SyntaxTree[C->GetSyntaxNode()].n_refback;
                                        SyntaxTreeNodeNo H2;
                                        Query_OutColType ColType2;
                                        if ( TargetNode->GetOutputColumnInternType(C->GetLogicalColumnIndex(), ColType2, H2) )
                                        {
                                            // compare C.InAggr(), upper and lower OutColType
                                            H2 = SyntaxTree[H2].n_lo_level;
                                            if ( SyntaxTree[H2].n_proc == no_proc && SyntaxTree[H2].n_symb == s_reference_name )
                                                H2 = SyntaxTree[H2].n_sa_level;

                                            if ( C->InAggr() )
                                            {
                                                if ( ColType2.MainType == Query_OutColType::OCTM_Aggregate )
                                                {
                                                    SAPDBERR_ASSERT_STATE( SyntaxTree.IsAggregationNode(H2) )

                                                    // only MAX(x) allowed in the outer select 
                                                    // BUT NOT MAX(x+1) 
                                                    if ( !SyntaxTree.IsAggregationNode(H1) )
                                                        return false;

                                                    // must be the same aggregations
                                                    if ( (int)SyntaxTree[H1].n_symb != (int)SyntaxTree[H2].n_symb )
                                                        return false;
                                                    switch ( SyntaxTree[H1].n_symb )
                                                    {
                                                    case s_all_count: 
                                                    case s_count:
                                                    case s_dis_count:
                                                        // DDT : ??? 
                                                        return false;
                                                        break;
                                                    case s_dis_sum:
                                                    case s_avg:
                                                    case s_dis_avg:
                                                    case s_stddev:
                                                    case s_dis_stddev:
                                                    case s_variance:
                                                    case s_dis_variance:
                                                        // DDT : ??? 
                                                        return false;
                                                        break;
                                                    case s_sum:
                                                    case s_min:
                                                    case s_max:
                                                        break;
                                                    }
                                                }
                                                else // inner outcol is no aggregation
                                                {
                                                    // only MAX(x+1), MAX(x) allowed in the inner select
                                                    // BUT NOT 1+Max(x)
                                                    if ( ColType2.MainType == Query_OutColType::OCTM_Expression 
                                                        && (ColType2.SubType & Query_OutColType::OCTS_ExpressionContainsAggr) )
                                                        return false;

                                                    switch ( SyntaxTree[H1].n_symb )
                                                    {
                                                    case s_all_count:   
                                                    case s_count:
                                                    case s_dis_count:
                                                        // DDT : ??? 
                                                        return false;
                                                        break;
                                                    case s_dis_sum:
                                                    case s_avg:
                                                    case s_dis_avg:
                                                    case s_stddev:
                                                    case s_dis_stddev:
                                                    case s_variance:
                                                    case s_dis_variance:
                                                        // DDT : ??? 
                                                        return false;
                                                        break;
                                                    case s_sum:
                                                    case s_min:
                                                    case s_max:
                                                        if ( Joins && !OneTupleAggregation )
                                                            return false;
                                                        break;
                                                    }


                                                }
                                            }
                                        }
                                        else
                                        {
                                            mQueryGraph.SetError("xyz");
                                            SAPDBERR_ASSERT_STATE(0);
                                            return false;
                                        }
                                    }
                                    else // C->GetQuantifierID() != TargetNode->NodeID()
                                    {
                                        // Columns from other quantifier MUST NOT be part of some aggregate function !
                                        // DDT : too strict! in same cases it could be possible !!!
                                        if ( C->InAggr() )
                                            return false;
                                    }
                                }
                                pCol = pCol->mNext;
                            }
                        }
                        else    
                        {
                            // !pCol : must be COUNT(*)
                            return false;
                        }
                    }
                    break;
                case Query_OutColType::OCTM_Unknown:
                    mQueryGraph.SetError("xyz");
                    SAPDBERR_ASSERT_STATE(0);
                    return false;
                    break;
            }
        }
        return mQueryGraph.IsOk();
    }
    return false;
}

bool Query_SelectNode::ContainsColumnFromQuantifierWhichIsAnAggregation( Container_Vector<Query_ColumnSpec*>* pColSpec, Query_Node* Node )
{
    bool rc=false;
    SAPDB_Int2 n=pColSpec->GetSize();
    for ( SAPDB_Int2 i=0; (i<n) && rc; i++ )
        rc |= ContainsColumnFromQuantifierWhichIsAnAggregation((*pColSpec)[i]->GetExpression(), Node);
    return rc;
}


bool Query_SelectNode::ContainsColumnFromQuantifierWhichIsAnAggregation( Query_Expression& Predicates, Query_Node* Node )
{
    SAPDB_Int2 Id = Node->NodeID();
    Query_ExpressionNode* pCol = Predicates.GetRoot();
    while ( pCol )
    {
        if ( pCol->GetExpType() == Exp_Column )
        {
            Query_Column* C = (Query_Column*)pCol;
            if ( C->GetQuantifierID() == Id )
            {
                Query_OutColType ColType;
                SyntaxTreeNodeNo H;
                if ( Node->GetOutputColumnInternType(C->GetLogicalColumnIndex(), ColType, H) )
                {
                    if (   ColType.MainType == Query_OutColType::OCTM_Aggregate
                        || (ColType.SubType & Query_OutColType::OCTS_ExpressionContainsAggr) )
                    {
                        return false;
                    }
                }
                else
                {
                    mQueryGraph.SetError("xyz");
                    SAPDBERR_ASSERT_STATE(0);
                    return false;
                }
            }
        }
        pCol = pCol->mNext;
    }
    return false;
}


// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::PushDownForMerge( Query_Node* TargetNode )
{
    SAPDBERR_ASSERT_ARGUMENT( TargetNode );
    SAPDBERR_ASSERT_ARGUMENT( TargetNode->IsSelectOrSet() );

    Query_MergeType MergeType = ((Aggregates() || HasGroupBy())&& (TargetNode->Aggregates() || TargetNode->HasGroupBy()) ? MT_Aggregates : MT_Normal );

    if ( TargetNode->GetNodeType() == Select )
    {
        if ( CorrelatedColumnsRefersToMe() > 0 )
        {
            if ( !RenewSemanticsForCorrelatedColumnsOnMergeForCorrCols( TargetNode->NodeID(), TargetNode ) )
                return false;
        }
        ((Query_SelectNode*)TargetNode)->ChangeCorrelatedColumnsRefersToMe(CorrelatedColumnsRefersToMe());
        ((Query_SelectNode*)TargetNode)->ChangeCorrelatedAndAggregatedColumnsRefersToMe(CorrelatedAndAggregatedColumnsRefersToMe());
        ((Query_SelectNode*)TargetNode)->ChangeCorrelatedColumns(CorrelatedColumns());

		if ( ((Query_SelectNode*)TargetNode)->CorrelatedColumnsRefersToMe() > 0 )
		{
			if ( !((Query_SelectNode*)TargetNode)->ExchangeNodeIDForCorrColumnsOnMerge(TargetNode->NodeID(), NodeID()) )
				return false;
		}
    }

	bool AggrInSelectList = AggregatesInOutput();

    bool rc = TargetNode->PullDownForMerge( mpOutputColumns,    mSelectListStartNo,
                                                                mSelectIntoStartNo,
                                    mSearchConditionColumns,    mSearchConditionStartNo, 
                                    mHavingColumns,             mHavingStartNo,
                                    mpGroupByColumns,           mGroupByStartNo,
                                    mpOrderByColumns,           mOrderByStartNo,
                                    mUpdateColumns,             mForUpdateColumnStartNo,
                                    mQuantifier,                mFromPartStartNo, 
									mLimitStartNo,
                                    mLockOptionStart,
                                    mForReuse,                  OuterJoin(),
                                    TargetNode->NodeID(),   // we need that for the case that the Target is a SetNode !!!
                                    MergeType,
									AggrInSelectList,
                                    /*MustCreateCopy*/false);

    mOutputColumnCnt = 0;
    return rc;
}

bool Query_SelectNode::PushDownQuantifier( Query_Node* TargetNode )
{
    SAPDBERR_ASSERT_ARGUMENT( TargetNode );
    SAPDBERR_ASSERT_ARGUMENT( TargetNode->IsSelectOrSet() );

    Container_Vector<Query_OutputColumn*>* EmptyOutputCols = 0;
    Container_Vector<Query_ColumnSpec*>*   EmptyColSpec = 0;
    Query_Expression EmptyCols;

// DDT : Quantifier müssen in "PullDownForMerge" richtig behandelt werden !!!

    bool rc = TargetNode->PullDownForMerge(  EmptyOutputCols,   cak_qrewrite_ref_init,
                                                                cak_qrewrite_ref_init,
                                    mSearchConditionColumns,    mSearchConditionStartNo, 
                                    mHavingColumns,             mHavingStartNo,
                                    EmptyColSpec,               cak_qrewrite_ref_init,
                                    EmptyColSpec,               cak_qrewrite_ref_init,
                                    EmptyCols,                  cak_qrewrite_ref_init,
                                    mQuantifier,                mFromPartStartNo, 
									mLimitStartNo,
                                    mLockOptionStart,
                                    /*ForReuse*/ false,         OuterJoin(),
                                    TargetNode->NodeID(),   // we need that for the case that the Target is a SetNode !!!
//                                    TargetNode->NodeID(),
                                    MT_NoMerge,
									/*AggrInSelectList*/false,
                                    /*MustCreateCopy*/false);

// DDT : Alle verbleibenden Columns müssen auf den einzig verbleibenden Quantifier gemapped werden !!!

    //if ( TargetNode->GetNodeType() == Select )
    //{
    //    ((Query_SelectNode*)LowerNode)->SetCorrelatedSubqueryRefersToMe(
    //                                            LowerNode->ContainsCorrelatedSubqueryRefersToMe() 
    //                                            || UpperNode->ContainsCorrelatedSubqueryRefersToMe() );
    //    ((Query_SelectNode*)LowerNode)->SetCorrelatedSubquery(
    //                                            UpperNode->IsCorrelatedSubquery());

    //    if ( ContainsCorrelatedSubqueryRefersToMe() )
    //    {
    //        SAPDB_UInt2 OldID = TargetNode->NodeID();
    //        TargetNode->SetNodeID(NodeID());
    //        rc &= TargetNode->RenewSemanticsForCorrelatedColumns( TargetNode->FatherQuantifier() );
    //        TargetNode->SetNodeID(OldID);
    //    }
    //}

    return rc;
}

bool Query_SelectNode::PullDownForMerge( 
        Container_Vector<Query_OutputColumn*>*& pOutputColumns, SyntaxTreeNodeNo SelectList_AP,
                                                                SyntaxTreeNodeNo SelectInto_AP,
        Query_Expression& SearchCondColumns,                    SyntaxTreeNodeNo SearchCond_AP,
        Query_Expression& HavingColumns,                        SyntaxTreeNodeNo Having_AP,
        Container_Vector<Query_ColumnSpec*>*& pGroupByColumns,  SyntaxTreeNodeNo GroupBy_AP,
        Container_Vector<Query_ColumnSpec*>*& pOrderByColumns,  SyntaxTreeNodeNo OrderBy_AP,
        Query_Expression& UpdateColumns,                        SyntaxTreeNodeNo Update_AP,

        Container_Vector<Query_Quantifier*>& Quantifier,        SyntaxTreeNodeNo FromPart_AP,
		SyntaxTreeNodeNo LimitStart, 
        SyntaxTreeNodeNo LockOptionStart,
        bool ForReuse, bool OuterJoinPredicates,
        SAPDB_Int2 QID,
//        SAPDB_Int2 NID,
        Query_MergeType MergeType,
		bool AggrInSelectList,
        bool MustCreateCopy
        ) 
{
    bool rc = true;

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    Query_NodeMapper NodeMapper   = Query_NodeMapper(mAllocator);
    Query_QuantifierMapper QMapper = Query_QuantifierMapper(mAllocator);
    QMapper.AddMappingInfo(QID, mID);
 
// DDT : Quantifier müssen richtig behandelt werden !!!
//       für PushDownQuantifier ist es notwendig, daß im Original der eine Quantifier übrig bleibt !!!


    // 1. add the quantifier 
    bool InsertFront = true;
    bool First = true;
    SyntaxTreeNodeNo A66Node = SyntaxTree[mFromPartStartNo].n_lo_level;
    while ( SyntaxTree[A66Node].n_sa_level > 0 )
    {
        A66Node = SyntaxTree[A66Node].n_sa_level;
    }

    SAPDB_Int2 nq = Quantifier.GetSize();
    for ( SAPDB_Int2 iq=0; iq<nq; iq++ )
    {
        Query_Quantifier* Q = &(*Quantifier[iq]);
        if (  !Q->IsDisabled()
            && Q->NodeID() != QID ) // the quantifier that belongs to 'me' !!!
        {
            Query_Quantifier* Q2 = Q;
            SyntaxTreeNodeNo SyntaxNode2 = Q->SyntaxNode();
            if ( MustCreateCopy )
            {
                SyntaxNode2 = SyntaxTree.DoubleTreePart(SyntaxNode2, &NodeMapper, /*MapNodes*/true, /*WithSameLevel*/false);
                if ( SyntaxNode2 > 0 )
                    Q2 = Q->CreateDuplicate( this, &NodeMapper, &QMapper);
                else
                    rc = false;
            }
            else
            {
                if ( Q2->QuantifierType() == QT_F )
                    SyntaxTree.UnlinkNode(SyntaxNode2, /*SetUnused*/ false);
            }

            Q2->NodeRef()->FatherNode() = this;
            Q2->Disable();
            if ( rc )
            {
                if ( Q2->QuantifierType() == QT_F )
                {
                    if ( InsertFront && First )
                    {
                        First = false;
                        SyntaxTree.SetSucessorOnLowLevel(mFromPartStartNo, SyntaxNode2);
                    }
                    else
                    {
                        SyntaxTree.SetSucessorOnSameLevel(A66Node, SyntaxNode2);
                    }
                    A66Node = SyntaxNode2;
                }
                if ( InsertFront )
                    rc &= mQuantifier.Insert(iq, Q2);
                else
                    rc &= mQuantifier.InsertEnd(Q2);
            }
            switch ( Q2->QuantifierType() )
            {
            case QT_F:
                ++mFQuantifierCnt;
                break;
            case QT_A:
            case QT_E:
            case QT_S:
                ++mWQuantifierCnt;
                break;
            case QT_L:
                ++mLQuantifierCnt;
                break;
			case QT_Unknown:
            default:
                mQueryGraph.SetError("Unknown Quantifier type");
                SAPDBERR_ASSERT_STATE( 0 );
                break;
            }
        }
        else
        {
            if ( Q->NodeID() == QID )
            {
                while ( SyntaxTree[A66Node].n_sa_level > 0 )
                {
                    A66Node = SyntaxTree[A66Node].n_sa_level;
                }
                InsertFront = false;
            }
        }
    }

    // 2. Double the SearchCondition if necessary and add it
    if ( rc && SearchCond_AP > 0 )
    {
        if ( MustCreateCopy )
        {
            SyntaxTreeNodeNo H = SyntaxTree[SearchCond_AP].n_lo_level;
            H = SyntaxTree.DoubleTreePart(H, &NodeMapper, /*MapNodes*/true, /*WithSameLevel*/true);
            if ( H > 0 )
            {
                Query_Expression SearchCondColumns2 = SearchCondColumns.CreateDuplicate2(mAllocator, SyntaxTree, &NodeMapper, &QMapper);
                SearchCondColumns2.ExchangeQuantifierID(QID, mID);
                rc &= SplitAndAddSearchCondition(SearchCondColumns2, H, /*MapColumns=*/true, MergeType, /*AsWhere=*/true, OuterJoinPredicates); 
            }
            else
            {
                rc = false;
            }
        }
        else
        {
            SearchCondColumns.ExchangeQuantifierID(QID, mID);
            SyntaxTreeNodeNo H = SyntaxTree[SearchCond_AP].n_lo_level;
            SyntaxTree.UnlinkNodeWithSameLevel(H, /*SetUnsused*/ false);
            rc &= SplitAndAddSearchCondition(SearchCondColumns, H, /*MapColumns=*/true, MergeType, /*AsWhere=*/true, OuterJoinPredicates); 
        }
    }

    // 3. Add GroupBy 
    // if the inner select as well as the outer have a group by the result have to be the outer group by
    if ( HasGroupBy() && (MergeType==MT_Aggregates) )
    {
        RemoveGroupBy();
    }
    if ( rc && GroupBy_AP > 0 )
    {
        if ( MustCreateCopy )
        {
            rc = false;
        }
        else
        {
            // add the outer group by
            ExchangeQuantifierIDInColumnSpec(pGroupByColumns, QID, mID);
            SyntaxTreeNodeNo H = SyntaxTree[GroupBy_AP].n_lo_level;
            SyntaxTree.UnlinkNodeWithSameLevel(H, /*SetUnsused*/ false);
            rc &= AddGroupBy(pGroupByColumns, H, /*MapColumns=*/true, MergeType);
        }
    }

    // 4. Double the Having if necessary and add it
    if ( rc && Having_AP > 0 )
    {
        if ( MustCreateCopy )
        {
            //SyntaxTreeNodeNo Having_AP2 = SyntaxTree[Having_AP].n_lo_level;
            //Having_AP2 = SyntaxTree.DoubleTreePart(Having_AP2, &NodeMapper, /*MapNodes*/true, /*WithSameLevel*/true);
            //if ( Having_AP2 > 0 )
            //{
            //    Query_Expression HavingColumns2 = SearchCondColumns.CreateDuplicate2(mAllocator, SyntaxTree, &NodeMapper, &QMapper);
            //    rc &= AddSearchCondition(HavingColumns2, Having_AP2, /*MapColumns=*/true, /*AsWhere=*/false); 
            //}
            //else
            {
                rc = false;
            }
        }
        else
        {
            HavingColumns.ExchangeQuantifierID(QID, mID);
            SyntaxTreeNodeNo H = SyntaxTree[Having_AP].n_lo_level;
            SyntaxTree.UnlinkNodeWithSameLevel(H, /*SetUnsused*/ false);
            rc &= AddSearchCondition(HavingColumns, H, /*MapColumns=*/true, MergeType, /*AsWhere=*/false); 
        }
    }

    // 5. Add the OrderBy 
    if ( rc && OrderBy_AP > 0 )
    {
        if ( MustCreateCopy )
        {
            rc = false;
        }
        else
        {
            ExchangeQuantifierIDInColumnSpec(pOrderByColumns, QID, mID);
            SyntaxTree.UnlinkNode(OrderBy_AP, /*SetUnsused*/ false);
            rc &= AddOrderBy(pOrderByColumns, OrderBy_AP, /*MapColumns=*/true, MergeType); 
        }
    }

    // 6. There must be no Merge if there are 'for update' columns
    if ( rc && Update_AP > 0 )
    {
        rc = false;
    }

    // 7. Add ForReuse
    if ( rc && ForReuse )
    {
        mForReuse = ForReuse;
        SyntaxTreeNodeNo H = mFromPartStartNo;
        while ( SyntaxTree[H].n_sa_level > 0 )
        {   
            H = SyntaxTree[H].n_sa_level;
        }
        SyntaxTreeNodeNo NewForReuse = SyntaxTree.GetFreeNode();
        if ( NewForReuse > 0 )
        {
		    SyntaxTree[NewForReuse].n_proc.becomes(a63);
            SyntaxTree[NewForReuse].n_subproc() = cak_x_for_reuse;
            SyntaxTree[NewForReuse].n_refback = H;
            SyntaxTree[H].n_sa_level = NewForReuse;

        }
        else
        {
            rc = false;
        }
    }

    // 8. Add the LockOption
    if ( rc && LockOptionStart > 0 )
    {
        if ( MustCreateCopy )
        {
            rc = false;
        }
        else
        {
            SyntaxTree.UnlinkNode(LockOptionStart, /*SetUnsused*/ false);
            rc &= AddLockOptions(LockOptionStart); 
        }
    }

    // 9. combine the select lists : the old one must be replaced !
//    ExchangeNodeIDInOutputColumns(mpOutputColumns, mID, NID);
    if ( rc && SelectList_AP > 0 )
    {
        mNoDuplicateCondition = QNULL;
        if ( MustCreateCopy )
        {
            SyntaxTreeNodeNo SelectList_AP2 = SyntaxTree.DoubleTreePart(SyntaxTree[SelectList_AP].n_lo_level, &NodeMapper, /*MapNodes*/true, /*WithSameLevel*/true);
            if ( SelectList_AP2 > 0 )
            {
                Container_Vector<Query_OutputColumn*>* pOutputColumns2 = CreateDuplicateOutputColumns(pOutputColumns, mAllocator, SyntaxTree, &NodeMapper, &QMapper);
                ExchangeQuantifierIDInOutputColumns(pOutputColumns2, QID, mID);
                rc &= MappSelectListFromOuterSelect(pOutputColumns2, MergeType);
                DestroyOutputColumns();
                mpOutputColumns = pOutputColumns2; 
                mOutputColumnCnt = CountOutputColumns();

                SyntaxTree.SetUnusedNode(SyntaxTree[mSelectListStartNo].n_lo_level, /*Recursive*/ true);
                SyntaxTree[mSelectListStartNo].n_lo_level = SelectList_AP2;
                SyntaxTree[SelectList_AP2].n_refback = mSelectListStartNo;
            }
            else
            {
                rc = false;
            }
        }
        else
        {
            ExchangeQuantifierIDInOutputColumns(pOutputColumns, QID, mID);
            rc &= MappSelectListFromOuterSelect(pOutputColumns, MergeType);
            DestroyOutputColumns();
            mpOutputColumns = pOutputColumns; 
            mOutputColumnCnt = CountOutputColumns();
            pOutputColumns = 0; 

            SyntaxTree.SetUnusedNode(SyntaxTree[mSelectListStartNo].n_lo_level, /*Recursive*/ true);
            SyntaxTreeNodeNo H = SyntaxTree[SelectList_AP].n_lo_level;
            if ( H > 0 )
            {
                SyntaxTree[mSelectListStartNo].n_lo_level = H;
                SyntaxTree[H].n_refback = mSelectListStartNo;
            }
            else
            {
                rc = false;
            }
            SyntaxTree[SelectList_AP].n_lo_level = 0;
        }

		if ( AggrInSelectList || mAggregatesInOutput )
			DetermineCountOrSumInSelListNode();
	}

    // 10. Add selectinto 
    if ( rc && SelectInto_AP > 0 )
    {
        if ( MustCreateCopy || HasSelectInto() )
        {
            rc = false;
        }
        else
        {
            SyntaxTree.UnlinkNode(SelectInto_AP, false);
            SyntaxTree.SetSucessorOnSameLevel(mSelectListStartNo, SelectInto_AP);
            SetSelectIntoStart(SelectInto_AP);
        }
    }

    // 11. Add limit 
    if ( rc && LimitStart > 0 )
    {
        if ( MustCreateCopy )
        {
            rc = false;
        }
        else
        {
            SyntaxTree.UnlinkNode(LimitStart, false);
            SyntaxTree.SetSucessorOnSameLevel(mSelectListStartNo, LimitStart);
            SetSelectIntoStart(LimitStart);
        }
    }

    // 12. Enable all new quantifier
    SAPDB_Int2 nq2 = mQuantifier.GetSize();
    for ( SAPDB_Int2 iq2=nq2-1; iq2>=0; iq2-- )
    {
        Query_Quantifier* Q = &(*mQuantifier[iq2]);
        Q->Enable();
    }

    // 13. Delete the old quantifiers
    if ( !MustCreateCopy )
    {
        SAPDB_Int2 nq3 = Quantifier.GetSize();
        for ( SAPDB_Int2 iq3=nq3-1; iq3>=0; iq3-- )
        {
            Query_Quantifier* Q = &(*Quantifier[iq3]);
            if ( Q->NodeID() != QID )
            {
                // destroy(Q, mAllocator);  DO NOT DESTROY THE QUANTIFIER!!!
                Quantifier.Delete(iq3);
            }
        }
    }

    return rc;
}

bool Query_SelectNode::DetermineCountOrSumInSelListNode( void )
{
	// Aggregates in the select list? set s_sum/s_count in the a60/1 node
	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTree[mSelectListStartNo].n_symb.becomes(s_unknown);
    bool ssum = false;
	mAggregatesInOutput = false;
    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 io=0; io<n && !ssum; io++ )
    {
        Query_OutputColumn* C = (*mpOutputColumns)[io];
        switch ( C->GetMainColType() )
        {
        case Query_OutColType::OCTM_Aggregate: 
			mAggregatesInOutput = true;
            if ( !ssum )
                SyntaxTree[mSelectListStartNo].n_symb.becomes(s_count);
            break;
        case Query_OutColType::OCTM_Expression:
			mAggregatesInOutput = true;
            if ( C->GetSubColType() & Query_OutColType::OCTS_ExpressionContainsAggr )
            {
                SyntaxTree[mSelectListStartNo].n_symb.becomes(s_sum);
                ssum = true;
            }
            break;
        }
    }
	return true;
}

bool Query_SelectNode::MergeSubquery( Query_SelectNode* SubQuery )
{
	SAPDBERR_ASSERT_ARGUMENT( SubQuery );
    SAPDBERR_ASSERT_ARGUMENT( SubQuery->Position() == P_Where /*|| SubQuery->Position() ==P_HavingE*/ );     // DDT
    SAPDBERR_ASSERT_ARGUMENT( SubQuery->FatherNode() == this );

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    bool rc = true;

    // 1. FromPart and Subqueries
    Container_Vector<Query_Quantifier*>& Quantifier = SubQuery->GetQuantifier();
    SyntaxTreeNodeNo A66Node = SyntaxTree[mFromPartStartNo].n_lo_level;
    while ( SyntaxTree[A66Node].n_sa_level > 0 )
    {
        A66Node = SyntaxTree[A66Node].n_sa_level;
    }

    SAPDB_Int2 nq = Quantifier.GetSize();
    SAPDB_Int2 iq = 0;
    while ( iq < nq && rc )
    {
        Query_Quantifier* Q = &(*Quantifier[iq]);
        if ( !Q->IsDisabled() && Q->QuantifierType()!=QT_L )
        {
            SyntaxTreeNodeNo SyntaxNode = Q->SyntaxNode();
            if ( Q->QuantifierType() == QT_F )
                SyntaxTree.UnlinkNode(SyntaxNode, /*SetUnused*/ false);

            Q->NodeRef()->FatherNode() = this;
            if ( Q->QuantifierType() == QT_F )
            {
                SyntaxTree.SetSucessorOnSameLevel(A66Node, SyntaxNode);
                A66Node = SyntaxNode;
            }
            rc &= mQuantifier.InsertEnd(Q);
            switch ( Q->QuantifierType() )
            {
            case QT_F:
                ++mFQuantifierCnt;
                break;
            case QT_A:
            case QT_E:
            case QT_S:
                ++mWQuantifierCnt;
                break;
            case QT_L:
			case QT_Unknown:
            default:
                SAPDBERR_ASSERT_STATE( 0 );
                break;
            }
            ++iq;
        }
        else
        {
            destroy(Q, mAllocator);
            Quantifier.Delete(iq); 
            --nq;
        }
    }
    SubQuery->GetQuantifier().Delete();

    // 2. Remove subquery hint and determine wheather it's EXIST or NOT EXISTS
    bool NotExists = false;    
    SyntaxTreeNodeNo S = SubQuery->GetInternAPNode();
    SyntaxTreeNodeNo SubStart = SyntaxTree[S].n_refback;
    SyntaxTreeNodeNo H2 = SyntaxTree[SubStart].n_refback;
    if ( SyntaxTree[H2].n_proc == no_proc && SyntaxTree[H2].n_symb == s_not )
    {
        NotExists = true;
        SubStart  = H2;
    }

    // 3. Where
    SyntaxTreeNodeNo Where = SubQuery->SearchConditionStart();
    Query_Expression& SearchCondColumns = SubQuery->GetSearchCondColumns();
    if ( Where>0 )
    {
        SyntaxTreeNodeNo H = SyntaxTree[Where].n_lo_level;
        SyntaxTree.UnlinkNodeWithSameLevel(H, /*SetUnsused*/ false);
        if ( NotExists )
        {
            SyntaxTreeNodeNo Not = cak_qrewrite_ref_init;
            SyntaxTree.AddNotBeforeNode(H, Not, 0);
            H = Not;
        }

        SAPDB_Int2 ExCnt=0;
        SubQuery->GetSearchCondColumns().ExchangeNodeID(mID, cak_qrewrite_ref_init, ExCnt);
        ChangeCorrelatedColumnsRefersToMe(-ExCnt);
        rc &= AddSearchCondition(SearchCondColumns, H, /*MapColumns=*/false, /*MergeType*/MT_Normal, /*AsWhere=*/true);
        // DDT : for P_HavingE we had to move the correlated terms to Having and the "local" ones to Where ...
                        //    if ( Col->InAggr() )
                        //ChangeCorrelatedAndAggregatedColumnsRefersToMe(+1);
    }

    // 4. HavingColumns : nothing to do as there must not be a Having !
    SAPDBERR_ASSERT_ARGUMENT( !SubQuery->HasHaving() );

    // 5. Delete Subquery
    S = ExtractPredicate(SubStart);
    SyntaxTree.SetUnusedNode(S, /*recursive*/true);
    if ( SyntaxTree[SearchConditionStart()].n_lo_level <= 0 )
    {
        SyntaxTree.UnlinkNode(mSearchConditionStartNo, true);
        mSearchConditionStartNo = cak_qrewrite_ref_init;
    }

    nq = mQuantifier.GetSize();
    for ( SAPDB_Int2 iq2=0; iq2<nq; iq2++ )
    {
        Query_Quantifier* Q = &(*mQuantifier[iq2]);
        if ( Q->NodeRef() == SubQuery )
        {
            destroy(Q, mAllocator);
            mQuantifier.Delete(iq2); 
            break;
        }
    }

	if ( HeadDistinct() == true )
	{
		// we have to make the result distinct again
        if ( !AddDistinctNode() )
            rc = false;
        BodyDistinctType() = DT_Enforce;		
	}
	mNoDuplicateCondition = QNULL;
	
	return rc;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::ExistSubqueryToIn( Query_SelectNode* FatherQuery )
{
	SAPDBERR_ASSERT_STATE( IsCorrelatedSubquery() );
	SAPDBERR_ASSERT_STATE( mSubQueryType == SQT_EXISTS );
	if ( IsCorrelatedSubquery() && mSubQueryType == SQT_EXISTS ) 
	{
		SyntaxTreeNodeNo InCond = cak_qrewrite_ref_init;
		bool OneColumn = (CorrelatedColumns() == 1);
		bool first=true;
		SAPDB_Int2 OrgColCnt = mpOutputColumns->GetSize();
		SAPDB_Int2 ColCnt    = 0;

		Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

        Container_Vector<SAPDB_Int2> Cols1(mAllocator);
        Container_Vector<SAPDB_Int2> Cols2(mAllocator);
        Container_Vector<SyntaxTreeNodeNo> Subqueries(mAllocator);
        Cols1.Reserve(10);
        Cols2.Reserve(10);
        //Subqueries.Reserve(5);
        SyntaxTreeNodeNo CurrentTerm = NextAndTerm(mSearchConditionStartNo); 

        do 
        {

			if (    SyntaxTree[CurrentTerm].n_proc == a64 
				 && SyntaxTree[CurrentTerm].n_subproc() == cak_x_predicate
				 && SyntaxTree[CurrentTerm].n_symb == s_equal )
			{
				// check both arguments of the '=' 
				SyntaxTreeNodeNo H1 = SyntaxTree[CurrentTerm].n_lo_level;
				SyntaxTreeNodeNo H2 = SyntaxTree[H1].n_sa_level;
				if ( SyntaxTree[H1].n_proc == no_proc && SyntaxTree[H1].n_symb == s_not )
					H2 = SyntaxTree[H2].n_sa_level;

	            SAPDB_UInt2 QID1 = 0, QID2 = 0;
				SAPDB_Int2  Dummy;
				Cols1.Clear(); 
				Cols2.Clear();
				bool local1 = CheckColumnsForExistToIn(H1, false, this, FatherQuery, QID1, &Cols1, Dummy);
				bool local2 = CheckColumnsForExistToIn(H2, false, this, FatherQuery, QID2, &Cols2, Dummy);

				if ( local1 && local2 )
				{
					if ( QID1!=QID2 )
					{
						if ( ( (QID1==NodeID() && QID2==FatherQuery->NodeID()) 
							|| (QID2==NodeID() && QID1==FatherQuery->NodeID())
							|| (QID1==FatherQuery->NodeID() && QID2==0)
							|| (QID2==FatherQuery->NodeID() && QID1==0)
							) )
						{
							++ColCnt;
							Query_Expression Predicate1;
							Query_Expression Predicate2;
							SyntaxTreeNodeNo Y = CurrentTerm;
							ExtractPredicate(CurrentTerm);
							ExtractPredicate(mSearchConditionColumns, Predicate1, Cols1); 
							ExtractPredicate(mSearchConditionColumns, Predicate2, Cols2);
							SyntaxTree.UnlinkNode(H1, /*SetUnused=*/false);
							SyntaxTree.UnlinkNode(H2, /*SetUnused=*/false);
							SyntaxTree.SetUnusedNode(Y, /*Recursive=*/true);

							if ( first )
							{
								// delete original select list !!!
								ClearOutputColumns();
								SyntaxTreeNodeNo S = SyntaxTree[mSelectListStartNo].n_lo_level;
								SyntaxTree.UnlinkNodeWithSameLevel(S, true);
								SyntaxTree[mSelectListStartNo].n_symb.becomes(s_unknown);
							}
							else
							{
								SAPDBERR_ASSERT_STATE( !OneColumn );
								if ( OneColumn )
								{
									mQueryGraph.SetError("ExistSubqueryToIn 1");
									return false;
								}
							}
							// extract the both arguments of the = and put one as a select column to the subquery 
							// and the other as an in term to the outer search condition, change the qid of the column!
							// keep the counter for correlation correct!!! 
							if ( QID1==NodeID() )	// left of = belongs to me, right of = belongs to the father
							{
								// move left  of '=' to my select list
						        Query_OutputColumn* C = new(mAllocator) Query_OutputColumn();
						        if ( C )
						        {
									C->IncRefCount();
									C->AppendColumn(Predicate1);
									mpOutputColumns->InsertEnd(C);

									SyntaxTreeNodeNo X = SyntaxTree.GetFreeNode();
									SyntaxTree[X].n_proc.becomes(a60);
									SyntaxTree[X].n_subproc() = cak_x_select_column;
									SyntaxTree.SetSucessorOnLowLevel(X, H1);
									SyntaxTree.SetSucessorOnLowLevel(mSelectListStartNo, X);
								}
								else
								{
									mQueryGraph.SetError("ExistSubqueryToIn 2");
									return false;
								}

								// move right of '=' to fathers search condition
								SAPDB_Int2 ExCnt=0;
								Predicate2.ExchangeNodeID(QID2, -1, ExCnt);
								ChangeCorrelatedColumns(-ExCnt);
						        FatherQuery->ChangeCorrelatedColumnsRefersToMe(-ExCnt);
								FatherQuery->GetSearchCondColumns().AddWithAnd(Predicate2);  
								if ( OneColumn )
								{
									InCond = H2;
								}
								else
								{
									if ( first )
									{
										InCond = SyntaxTree.GetFreeNode();
										SyntaxTree[InCond].n_proc.becomes(a56);
										SyntaxTree[InCond].n_subproc() = cak_x_value_list;
									}
									SyntaxTreeNodeNo X = SyntaxTree.GetFreeNode();
									SyntaxTree[X].n_proc.becomes(a64);
									SyntaxTree[X].n_subproc() = cak_x_value_expression;
									SyntaxTree.SetSucessorOnLowLevel(X, H2);
									SyntaxTree.SetSucessorOnLowLevel(InCond, X);
								}
							}
							else
							{
								// move right of '=' to my select list
						        Query_OutputColumn* C = new(mAllocator) Query_OutputColumn();
						        if ( C )
						        {
									C->IncRefCount();
									C->AppendColumn(Predicate2);
									mpOutputColumns->InsertEnd(C);

									SyntaxTreeNodeNo X = SyntaxTree.GetFreeNode();
									SyntaxTree[X].n_proc.becomes(a60);
									SyntaxTree[X].n_subproc() = cak_x_select_column;
									SyntaxTree.SetSucessorOnLowLevel(X, H2);
									SyntaxTree.SetSucessorOnLowLevel(mSelectListStartNo, X);
								}
								else
								{
									mQueryGraph.SetError("ExistSubqueryToIn 3");
									return false;
								}

								// move left  of '=' to fathers search condition
								SAPDB_Int2 ExCnt=0;
								Predicate1.ExchangeNodeID(QID1, -1, ExCnt);
								ChangeCorrelatedColumns(-ExCnt);
						        FatherQuery->ChangeCorrelatedColumnsRefersToMe(-ExCnt);
								FatherQuery->GetSearchCondColumns().AddWithAnd(Predicate1); 

								if ( OneColumn )
								{
									InCond = H1;
								}
								else
								{
									if ( first )
									{
										InCond = SyntaxTree.GetFreeNode();
										SyntaxTree[InCond].n_proc.becomes(a56);
										SyntaxTree[InCond].n_subproc() = cak_x_value_list;
									}
									SyntaxTreeNodeNo X = SyntaxTree.GetFreeNode();
									SyntaxTree[X].n_proc.becomes(a64);
									SyntaxTree[X].n_subproc() = cak_x_value_expression;
									SyntaxTree.SetSucessorOnLowLevel(X, H1);
									SyntaxTree.SetSucessorOnLowLevel(InCond, X);
								}
							}
							first = false;
						}
					}
				}
			}

	// SyntaxTree.PrintToTrace();		
		
		}
        while ( (CurrentTerm = NextAndTerm(CurrentTerm)) > 0 );

		if ( SyntaxTree[mSearchConditionStartNo].n_lo_level == 0 )
		{
			SyntaxTree.UnlinkNode(mSearchConditionStartNo, /*SetUnused=*/true);
			mSearchConditionStartNo = cak_qrewrite_ref_init;
		}

		if ( HasOrderBy() )
		{
			SAPDBERR_ASSERT_STATE( 0 );
			EliminateOrderBy();
		}

		SyntaxTreeNodeNo OrderBy = SyntaxTree.GetFreeNode(); 
		if ( OrderBy>0 )
		{
			SyntaxTree[OrderBy].n_proc.becomes(a63);
			SyntaxTree[OrderBy].n_subproc() = cak_x_order;
			for ( SAPDB_Int2 o=0; o<ColCnt; o++ )
			{
				SyntaxTreeNodeNo N1 = SyntaxTree.GetFreeNode();
				SyntaxTreeNodeNo N2 = SyntaxTree.GetFreeNode();
				
				if ( N1>0 && N2>0 )
				{
					SyntaxTree[N1].n_proc.becomes(a63);
					SyntaxTree[N1].n_subproc() = cak_x_sort_spec;
					SyntaxTree[N1].n_lo_level = N2;

					SyntaxTree[N2].n_proc.becomes(no_proc);
					SyntaxTree[N2].n_symb.becomes(s_equal);
					SyntaxTree[N2].n_pos() = ColCnt-o;
					SyntaxTree[N2].n_refback = N1;

					SyntaxTree.SetSucessorOnLowLevel(OrderBy, N1);
				}
				else
				{
					mQueryGraph.SetError("ExistSubqueryToIn 4");
					return false;
				}
			}
			AddOrderBy(OrderBy);
		}
		else
		{
			mQueryGraph.SetError("ExistSubqueryToIn 5");
			return false;
		}

		//SyntaxTreeNodeNo N1 = SyntaxTree.GetFreeNode();
		//SyntaxTreeNodeNo N2 = SyntaxTree.GetFreeNode(); 

		//if ( N1>0 && N2>0 && N3>0 )
		//{

		//	SyntaxTree[N2].n_proc.becomes(a63);
		//	SyntaxTree[N2].n_subproc() = cak_x_sort_spec;
		//	SyntaxTree[N2].n_refback  = N1;
		//	SyntaxTree[N2].n_lo_level = N3;
		//	SyntaxTree[N3].n_proc.becomes(no_proc);
		//	SyntaxTree[N3].n_symb.becomes(s_equal);
		//	SyntaxTree[N3].n_refback  = N2;
		//	AddOrderBy(N1);
		//}

		SAPDBERR_ASSERT_STATE( ColCnt>0 );
		if ( !OneColumn )
		{
			// there could have been more than one column to create...
			if ( ColCnt > 1 )
			{
				SyntaxTree[InCond].n_length() = ColCnt;
			}
			else
			{
				SyntaxTreeNodeNo X = InCond;
				InCond = SyntaxTree[InCond].n_lo_level;
				InCond = SyntaxTree[InCond].n_lo_level;
				SyntaxTree.UnlinkNode(InCond, /*SetUnused=*/false);
				SyntaxTree.SetUnusedNode(X, /*Recursive=*/true);
			}
		}
		else
		{
			SAPDBERR_ASSERT_STATE( ColCnt==1 );
		}

        // add the in-condition correctly and change the type of the subquery
		SyntaxTreeNodeNo N = GetInternAPNode();
		SyntaxTreeNodeNo ExistNode = SyntaxTree[N].n_refback;

		SyntaxTree.UnlinkNode(N, /*SetUnused=*/false); 

		SyntaxTree[ExistNode].n_proc.becomes(a64);
		SyntaxTree[ExistNode].n_subproc() = cak_x_in_pred;
		SyntaxTree[ExistNode].n_symb.becomes(s_in);

		SyntaxTree.SetSucessorOnLowLevel(ExistNode, N);
		SyntaxTree.SetSucessorOnLowLevel(ExistNode, InCond);
		mSubQueryType = SQT_IN;

		return true;
	}
	mQueryGraph.SetError("ExistSubqueryToIn 6");
	return false;
}

bool Query_SelectNode::CheckColumnsForExistToIn( Query_SelectNode* FatherQuery )
{
	if ( IsCorrelatedSubquery() ) 
	{
		SAPDB_Int2 ColCnt=0;
		SAPDB_Int2 SelListLength=0;
        SyntaxTreeNodeNo CurrentTerm = NextAndTerm(mSearchConditionStartNo);
        do 
        {
			Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
			SyntaxTreeNodeNo H1 = CurrentTerm, H2=0;
			
			if (    SyntaxTree[H1].n_proc == a64 
				 && SyntaxTree[H1].n_subproc() == cak_x_predicate
				 && SyntaxTree[H1].n_symb == s_equal )
			{
				// check both arguments of the '=' 
				H1 = SyntaxTree[H1].n_lo_level;
				H2 = SyntaxTree[H1].n_sa_level;
				if ( SyntaxTree[H1].n_proc == no_proc && SyntaxTree[H1].n_symb == s_not )
					H2 = SyntaxTree[H2].n_sa_level;

	            SAPDB_UInt2 QID1 = 0, QID2 = 0;
				SAPDB_Int2 Len1=0, Len2=0;
				bool local1 = CheckColumnsForExistToIn(H1, false, this, FatherQuery, QID1, 0, Len1);
				bool local2 = CheckColumnsForExistToIn(H2, false, this, FatherQuery, QID2, 0, Len2);

				if ( !local1 || !local2 )
					return false;
	
				if ( QID1==NodeID() )
				{
					if ( Len1>0 )
						SelListLength += Len1;
					else
						return false;
				}
				else
				{
					if ( QID2==NodeID() )
					{
						if ( Len2>0 )
							SelListLength += Len2;
						else
							return false;
					}
				}

				if ( QID1!=QID2 )
				{
					if ( !( (QID1==NodeID() && QID2==FatherQuery->NodeID()) 
						 || (QID2==NodeID() && QID1==FatherQuery->NodeID())
						 || (QID1==NodeID() && QID2==0)
						 || (QID2==NodeID() && QID1==0)
						 || (QID1==FatherQuery->NodeID() && QID2==0)
						 || (QID2==FatherQuery->NodeID() && QID1==0)
					    ) )
						return false;
				}
				else
				{
					if ( QID1!=NodeID() && QID1!=0 )	// DDT : ???
						return false;
				}
				++ColCnt;
			}
        }
        while ( (CurrentTerm = NextAndTerm(CurrentTerm)) > 0 && (SelListLength<=QRW_MaxTempKeyLen) );
		SAPDBERR_ASSERT_STATE( (ColCnt>0) == (SelListLength>0) );
		return (ColCnt>0) && (SelListLength>0) && (SelListLength<=QRW_MaxTempKeyLen); // DDT : what exactly is the correct constant ? 1024-n ?
	}
	return false;
}

bool Query_SelectNode::CheckColumnsForExistToIn( SyntaxTreeNodeNo Start, bool CheckSame, Query_SelectNode* Query1, Query_SelectNode* Query2, SAPDB_UInt2& QID, Container_Vector<SAPDB_Int2>* Cols, SAPDB_Int2& Len )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	if ( SyntaxTree[Start].n_proc == a63query_spec )
        return false;   // DDT 

    if ( SyntaxTree[Start].n_proc == a64 
         && (   SyntaxTree[Start].n_subproc() == cak_x_left_outer_join 
              ||SyntaxTree[Start].n_subproc() == cak_x_right_outer_join 
              ||SyntaxTree[Start].n_subproc() == cak_x_full_outer_join ) )
		return false;

    if ( SyntaxTree[Start].n_proc == no_proc && SyntaxTree[Start].n_symb == s_rowno )
        return false;

    if ( SyntaxTree[Start].n_proc == no_proc && SyntaxTree[Start].n_symb == s_columnid )
    {
		SAPDB_Int2 QNode = SyntaxTree[Start].n_fromtabnode();
		SAPDB_Int2 ExtColNo = SyntaxTree[Start].n_columnindex();

		// check wheater the quantifier identified by QNode belongs to Query1 or Query2 
		SAPDB_Int2 X=0, N;
		if ( Query1->GetQuantifierIDAndNoFromSyntaxNode(QNode, X, N) )
		{
			if ( Len == 0 )
			{
				SAPDB_Int2 LogColNo = Query1->GetQuantifier()[N]->NodeRef()->ExtColNo2LogicalColNo(ExtColNo);
				Len = Query1->GetQuantifier()[N]->NodeRef()->GetOutputColumnSize(LogColNo);
			}
			X = Query1->NodeID();
			if ( Cols )
				Cols->InsertEnd(Start);
		}
		else
		{
			if ( Query2->GetQuantifierIDAndNoFromSyntaxNode(QNode, X, N) )
			{
				if ( Len == 0 )
				{
					SAPDB_Int2 LogColNo = Query2->GetQuantifier()[N]->NodeRef()->ExtColNo2LogicalColNo(ExtColNo);
					Len = Query2->GetQuantifier()[N]->NodeRef()->GetOutputColumnSize(LogColNo);
				}
				X = Query2->NodeID();
				if ( Cols )
					Cols->InsertEnd(Start);
			}
			else
			{
				return false;
			}
		}
		if ( X>0 )
		{
			if ( QID > 0 ) 
			{
				if ( QID != X )
					return false;
			}
			else
			{
				QID = X;
			}
		}
    }
	else
	{
		Len = cak_qrewrite_ref_init;
	}

    SyntaxTreeNodeNo H = SyntaxTree[Start].n_lo_level;
    if ( H > 0 )
        if ( !CheckColumnsForExistToIn( H, true, Query1, Query2, QID, Cols, Len) )
			return false;

    H = SyntaxTree[Start].n_sa_level;
    if ( H > 0 && CheckSame )
        if ( !CheckColumnsForExistToIn( H, true, Query1, Query2, QID, Cols, Len) )
			return false;

    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::AddMinOrMaxToAllOutputColumn ( bool Max )
{
	SAPDBERR_ASSERT_STATE( !mAggregatesInOutput );

	if ( mAggregatesInOutput )
		return false;

	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	SAPDB_Int2 n = mpOutputColumns->GetSize();
	for ( SAPDB_Int2 i=0; i<n; i++ )
    {
		Query_OutputColumn* C = (*mpOutputColumns)[i];
        switch ( C->GetMainColType() )
        {
        case Query_OutColType::OCTM_Column:
        case Query_OutColType::OCTM_Expression:
			{
				Query_ExpressionNode* R = C->GetExpression().GetRoot();
				if ( R )
				{
					SyntaxTreeNodeNo NC = C->GetSyntaxNode();
					SyntaxTreeNodeNo H  = SyntaxTree[NC].n_lo_level;

					SyntaxTreeNodeNo AggrNo = SyntaxTree.GetFreeNode();
					if ( AggrNo > 0 )
					{
						SyntaxTree[AggrNo].n_proc.becomes(no_proc);
						if ( Max )
							SyntaxTree[AggrNo].n_symb.becomes(s_max);
						else
							SyntaxTree[AggrNo].n_symb.becomes(s_min);

						if ( (SyntaxTree[H].n_proc == 0) &&
							(SyntaxTree[H].n_symb == s_reference_name) )
						{
							NC = H;
							H  = SyntaxTree[NC].n_sa_level; 

							SyntaxTree[NC    ].n_sa_level = AggrNo;
							SyntaxTree[AggrNo].n_refback  = NC;
							SyntaxTree[AggrNo].n_lo_level = H;
							SyntaxTree[H     ].n_refback  = AggrNo;
						}
						else
						{
							SyntaxTree[NC    ].n_lo_level = AggrNo;
							SyntaxTree[AggrNo].n_refback  = NC;
							SyntaxTree[AggrNo].n_lo_level = H;
							SyntaxTree[H     ].n_refback  = AggrNo;
						}
						R->SetInAggr(true);
					}
					else
					{
						mQueryGraph.SetError("AddMinOrMaxToAllOutputColumn");
						return false;
					}
				}
				else
				{
					// if no columns are in the output we have nothing to do
					// it's something like "select 1+2 from tab"
				}
			}
			break;
		case Query_OutColType::OCTM_Asterisk:
        case Query_OutColType::OCTM_MultiAsterisk:
        case Query_OutColType::OCTM_Subquery:
        case Query_OutColType::OCTM_Aggregate: 
        case Query_OutColType::OCTM_Unknown: 
        default:
			mQueryGraph.SetError("AddMinOrMaxToAllOutputColumn : Wrong OutColType");
			SAPDBERR_ASSERT_STATE( 0 );
			return false;
			break;
        }
	}
	mAggregatesInOutput = true;
	SyntaxTree[mSelectListStartNo].n_symb.becomes(s_count);
	return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::OutputContainsAllColumnsFromQuantifier( Query_Quantifier* Q ) 
{ 
    SAPDB_Int2 QId      = Q->NodeID();
    SAPDB_Int2 n_lo     = Q->NodeRef()->OutputColumns();
    SAPDB_Int2 n_this   = mpOutputColumns->GetSize();

    for ( SAPDB_Int2 i=1; i<=n_lo; i++ )
    {
        bool found = false;
        for ( SAPDB_Int2 k=0; k<n_this; k++ )
        {
            Query_OutputColumn* C = (*mpOutputColumns)[k];
            switch ( C->GetMainColType() )
            {
                case Query_OutColType::OCTM_Column:
                    if ( QId == C->GetQuantifierID() && i == C->GetLogicalColumnIndex() )
                        found = true;
                    break;
                case Query_OutColType::OCTM_Asterisk:
					if ( QId == C->GetQuantifierID() )
                        return true;
                    break;
                case Query_OutColType::OCTM_MultiAsterisk:
                    return true;
                    break;
				case Query_OutColType::OCTM_Expression:
				case Query_OutColType::OCTM_Aggregate:
				case Query_OutColType::OCTM_Subquery:
				case Query_OutColType::OCTM_Unknown:
				default:
                    // DDT ??
                    break;
            }
        }
        if ( !found )
            return false;
    }
    return true; 
}

bool Query_SelectNode::OutputContainsAllColumnsFromGroupBy( void )
{
    // DDT : does not work for "GROUP BY 1" as there is no correspondig ExpressionNode for "1" 
    // therefore we have to use the APNodes for now.

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo SortSpecN = GroupByStart();
    if ( SortSpecN > 0 )
    {
        SortSpecN = SyntaxTree[SortSpecN].n_lo_level;
        while ( SortSpecN > 0 )
        {
            bool found = false;
            SyntaxTreeNodeNo H = SyntaxTree[SortSpecN].n_lo_level;
            if ( SyntaxTree[H].n_proc == no_proc &&  SyntaxTree[H].n_symb == s_columnid )
            {
                SAPDB_Int2 QNode = SyntaxTree[H].n_fromtabnode(); 
                SAPDB_Int2 ColNo = SyntaxTree[H].n_columnindex();

                SAPDB_Int2 n = mpOutputColumns->GetSize();
                for ( SAPDB_Int2 i=0; i<n; i++ )
                {
                    Query_OutputColumn* C = (*mpOutputColumns)[i];
                    switch ( C->GetMainColType() )
                    {
                        case Query_OutColType::OCTM_Column:
                            if ( QNode == C->GetQuantifierSyntaxNode() && ColNo == C->GetExtColNo() )
                                found = true;
                            break;
                        case Query_OutColType::OCTM_Asterisk:
					        if ( QNode == C->GetQuantifierSyntaxNode() )
                                found = true;
                            break;
                        case Query_OutColType::OCTM_MultiAsterisk:
                            found = true;
                            break;
						case Query_OutColType::OCTM_Expression:
						case Query_OutColType::OCTM_Aggregate:
						case Query_OutColType::OCTM_Subquery:
						case Query_OutColType::OCTM_Unknown:
						default:
                            // DDT ??
                            break;
                    }            
                }
            }
            else
            {
                if ( SyntaxTree[H].n_proc == no_proc &&  SyntaxTree[H].n_symb == s_equal )  // DDT : internal version only !!
                {
                    // group by N
                    if ( SyntaxTree[H].n_pos() <= mOutputColumnCnt )
                        found = true;
                }
            }
            if ( !found )
                return false;
            SortSpecN = SyntaxTree[SortSpecN].n_sa_level;
        }
    }
    return false; // DDT : return true/false if there is no groupby ???
}

bool Query_SelectNode::OutputContainsAllColumnsFromOrderBy( void )
{
    // DDT : does not work for "ORDER BY 1" as there is no correspondig ExpressionNode for "1" 
    // therefore we have to use the APNodes for now.

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo SortSpecN = OrderByStart();
    while ( SortSpecN > 0 )
    {
        bool found = false;
        SyntaxTreeNodeNo H = SyntaxTree[SortSpecN].n_lo_level;
        if ( SyntaxTree[H].n_proc == no_proc &&  SyntaxTree[H].n_symb == s_columnid )
        {
            SAPDB_Int2 QNode = SyntaxTree[H].n_fromtabnode(); 
            SAPDB_Int2 ColNo = SyntaxTree[H].n_columnindex();

            SAPDB_Int2 n = mpOutputColumns->GetSize();
            for ( SAPDB_Int2 i=0; i<n; i++ )
            {
                Query_OutputColumn* C = (*mpOutputColumns)[i];
                switch ( C->GetMainColType() )
                {
                    case Query_OutColType::OCTM_Column:
                        if ( QNode == C->GetQuantifierSyntaxNode() && ColNo == C->GetExtColNo() )  // ExtColNo / GetLogicalColumnIndex ?
                            found = true;
                        break;
                    case Query_OutColType::OCTM_Asterisk:
					    if ( QNode == C->GetQuantifierSyntaxNode() )
                            found = true;
                        break;
                    case Query_OutColType::OCTM_MultiAsterisk:
                        found = true;
                        break;
					case Query_OutColType::OCTM_Expression:
					case Query_OutColType::OCTM_Aggregate:
					case Query_OutColType::OCTM_Subquery:
					case Query_OutColType::OCTM_Unknown:
                    default:
                        // DDT ??
                        break;
                }            
            }
        }
        else
        {
            if ( SyntaxTree[H].n_proc == no_proc &&  SyntaxTree[H].n_symb == s_equal )  // DDT : internal version only !!
            {
                // order by N
                if ( SyntaxTree[H].n_pos() <= mOutputColumnCnt )
                    found = true;
            }
        }
        if ( !found )
            return false;
        SortSpecN = SyntaxTree[SortSpecN].n_sa_level;
    }
    return true;
}

SAPDB_Int2 Query_SelectNode::CountOutputColumns( void )
{
    SAPDB_Int2 c = 0;
    SAPDB_Int2 n = mpOutputColumns->GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
        c+= (*mpOutputColumns)[i]->GetColumnCount();
    return c;
}

bool Query_SelectNode::GroupByContainsAllOrderByColumnsFromLeft( bool Identity )
{
    // DDT : does not work for "ORDER BY 1" as there is no correspondig ExpressionNode for "1" 
    //       GROUP BY a,b ORDER BY a+b -> return true       ERROR !
    //       GROUP BY a,b ORDER BY b+a -> return false      ERROR !
    // therefore we have to use the APNodes for now.

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    SyntaxTreeNodeNo GroupByN = GroupByStart();
    SyntaxTreeNodeNo OrderByN = OrderByStart();

    if ( GroupByN>0 && OrderByN>0 )
    {
        GroupByN = SyntaxTree[GroupByN].n_lo_level;
        OrderByN = SyntaxTree[OrderByN].n_lo_level;

        while ( GroupByN>0 && OrderByN>0 )
        {
            SyntaxTreeNodeNo HG = SyntaxTree[GroupByN].n_lo_level;
            SyntaxTreeNodeNo HO = SyntaxTree[OrderByN].n_lo_level;

            if (   SyntaxTree[HG].n_proc == no_proc &&  SyntaxTree[HG].n_symb == s_columnid 
                && SyntaxTree[HO].n_proc == no_proc &&  SyntaxTree[HO].n_symb == s_columnid )
            {
                SyntaxTreeNodeNo X = SyntaxTree[HO].n_sa_level;
                if ( X>0 )
                {
                    if (   SyntaxTree[X].n_proc == no_proc
                        && SyntaxTree[X].n_symb == s_desc )
                        return false;
                }

                SAPDB_Int2 GroupQNode = SyntaxTree[HG].n_fromtabnode();
                SAPDB_Int2 GroupColNo = SyntaxTree[HG].n_columnindex();

                SAPDB_Int2 OrderQNode = SyntaxTree[HO].n_fromtabnode();
                SAPDB_Int2 OrderColNo = SyntaxTree[HO].n_columnindex();

                if ( !(OrderQNode == GroupQNode && OrderColNo == GroupColNo) )
                    return false;
            }
            else
            {
                if (   SyntaxTree[HG].n_proc == no_proc &&  SyntaxTree[HG].n_symb == s_columnid 
                    && SyntaxTree[HO].n_proc == no_proc &&  SyntaxTree[HO].n_symb == s_equal )
                {
                    SyntaxTreeNodeNo X = SyntaxTree[HO].n_sa_level;
                    if ( X>0 )
                    {
                        if (   SyntaxTree[X].n_proc == no_proc
                            && SyntaxTree[X].n_symb == s_desc )
                            return false;
                    }

                    SAPDB_Int2 GroupQNode = SyntaxTree[HG].n_fromtabnode();
                    SAPDB_Int2 GroupColNo = SyntaxTree[HG].n_columnindex();

                    SAPDB_Int2 N = SyntaxTree[HO].n_pos(), D1;
                    SAPDB_Int2 OrderQNode,OrderColNo;
                    SQLMan_Identifier D2(false), D3(false);
                    bool D4;

                    if ( !GetOutputColumn( N, D2, D3, OrderQNode, D1, D1, OrderColNo, D1, D4 ) )
                        return false;

                    if ( !(OrderQNode == GroupQNode && OrderColNo == GroupColNo) )
                        return false;
                }
                else
                {
                    return false;
                }
            }
            GroupByN = SyntaxTree[GroupByN].n_sa_level;
            OrderByN = SyntaxTree[OrderByN].n_sa_level;
        }
    }
	if ( Identity )
		return !(OrderByN>0) && !(GroupByN>0) && (mOutputColumnCnt==1);
	else
	    return !(OrderByN>0);

    //Query_ExpressionNode* oc = mOrderByColumns.GetRoot();
    //Query_ExpressionNode* gc = mGroupByColumns.GetRoot();
    //while ( oc && gc )
    //{
    //    if (   oc->GetExpType() == Exp_Column 
    //        && gc->GetExpType() == Exp_Column )
    //    {
    //        SyntaxTreeNodeNo ColNode = ((Query_Column*)oc)->GetSyntaxNode();
    //        if ( (ColNode = SyntaxTree[ColNode].n_sa_level) > 0 )
    //        {
    //            if (   SyntaxTree[ColNode].n_proc == no_proc
    //                && SyntaxTree[ColNode].n_symb == s_desc )
    //                return false;
    //        }

    //        SAPDB_Int2 OrderQId   = ((Query_Column*)oc)->GetQuantifierID(); 
    //        SAPDB_Int2 OrderColNo = ((Query_Column*)oc)->GetLogicalColumnIndex();

    //        SAPDB_Int2 GroupQId   = ((Query_Column*)gc)->GetQuantifierID(); 
    //        SAPDB_Int2 GroupColNo = ((Query_Column*)gc)->GetLogicalColumnIndex();


    //        if ( !(OrderQId == GroupQId && OrderColNo == GroupColNo) )
    //            return false;
    //    }
    //    else
    //    {
    //        return false;
    //    }
    //    oc = oc->mNext;
    //    gc = gc->mNext;
    //}
    //return ( oc == 0 );
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::HasFromSelects( void ) 
{
    SAPDB_Int2 n = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Query_Quantifier* Q = mQuantifier[i];
        if (  !Q->IsDisabled() 
            && Q->QuantifierType() == QT_F 
            && Q->NodeTypeSelectOrSet() )
            return true;
    }
    return false;
}

SyntaxTreeNodeNo Query_SelectNode::NextAndTerm(SyntaxTreeNodeNo StartNode)
{
    if ( StartNode > 0 )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
        if ( ! (SyntaxTree[StartNode].n_proc == a63 && SyntaxTree[StartNode].n_subproc() == cak_x_search_condition ) )
        {
			if ( (SyntaxTree[StartNode].n_proc == a64 && SyntaxTree[StartNode].n_symb == s_and ) )
				StartNode = SyntaxTree[StartNode].n_lo_level;
			else
			{
				if ( (SyntaxTree[StartNode].n_proc == no_proc && SyntaxTree[StartNode].n_symb == s_not ) )
					StartNode = SyntaxTree[StartNode].n_sa_level;
	            StartNode = SyntaxTree[StartNode].n_sa_level;
				if ( (SyntaxTree[StartNode].n_proc == a64 && SyntaxTree[StartNode].n_symb == s_and ) )
					StartNode = SyntaxTree[StartNode].n_lo_level;
			}
        }
        else
        {
            StartNode = SyntaxTree[StartNode].n_lo_level;
			if ( (SyntaxTree[StartNode].n_proc == a64 && SyntaxTree[StartNode].n_symb == s_and ) )
				StartNode = SyntaxTree[StartNode].n_lo_level;
        }
    }
    return StartNode;
}

bool Query_SelectNode::IsLocalPredicate(SyntaxTreeNodeNo CurrentTerm, SyntaxTreeNodeNo& QNo, Container_Vector<SAPDB_Int2>& Cols, Container_Vector<SyntaxTreeNodeNo>& SubQueries, bool& OuterJoinFlag, bool& RownoPredicate, bool FillColAndSubQInfo )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    if ( (SyntaxTree[CurrentTerm].n_proc == no_proc && SyntaxTree[CurrentTerm].n_symb == s_not ) )
            CurrentTerm = SyntaxTree[CurrentTerm].n_sa_level;
    return CheckForLocalPredicateAndSubqueries(CurrentTerm, QNo, Cols, SubQueries, OuterJoinFlag, RownoPredicate, /*CheckSame*/false, FillColAndSubQInfo);
}

bool Query_SelectNode::CheckForLocalPredicateAndSubqueries(SyntaxTreeNodeNo CurrentTerm, SyntaxTreeNodeNo& QNo, Container_Vector<SAPDB_Int2>& Cols, Container_Vector<SyntaxTreeNodeNo>& SubQueries, bool& OuterJoinFlag, bool& RownoPredicate, bool CheckSame, bool FillColAndSubQInfo )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    if ( SyntaxTree[CurrentTerm].n_proc == a63query_spec )
    {
		if ( FillColAndSubQInfo )
	        SubQueries.InsertEnd(CurrentTerm);
        return false;   // DDT : how can we handle this situation correctly? In case of correlated subqueries ??
    }

    bool rc = true;

    if ( SyntaxTree[CurrentTerm].n_proc == a64 
         && (   SyntaxTree[CurrentTerm].n_subproc() == cak_x_left_outer_join 
              ||SyntaxTree[CurrentTerm].n_subproc() == cak_x_right_outer_join 
              ||SyntaxTree[CurrentTerm].n_subproc() == cak_x_full_outer_join ) )
        OuterJoinFlag = true;

    if ( SyntaxTree[CurrentTerm].n_proc == no_proc && SyntaxTree[CurrentTerm].n_symb == s_rowno )
        RownoPredicate = true;

    if ( SyntaxTree[CurrentTerm].n_proc == no_proc && SyntaxTree[CurrentTerm].n_symb == s_columnid )
    {
        if ( QNo >=0 ) 
        {
            if ( QNo != SyntaxTree[CurrentTerm].n_fromtabnode() )
            {
                rc = false;
            }
        }
        else
        {
            QNo = SyntaxTree[CurrentTerm].n_fromtabnode();
        }
		if ( FillColAndSubQInfo )
	        Cols.InsertEnd(CurrentTerm);
    }

    SyntaxTreeNodeNo H = SyntaxTree[CurrentTerm].n_lo_level;
    if ( H > 0 )
        rc &= CheckForLocalPredicateAndSubqueries(H, QNo, Cols, SubQueries, OuterJoinFlag, RownoPredicate, /*CheckSame*/true, FillColAndSubQInfo);

    H = SyntaxTree[CurrentTerm].n_sa_level;
    if ( H > 0 && CheckSame )
        rc &= CheckForLocalPredicateAndSubqueries(H, QNo, Cols, SubQueries, OuterJoinFlag, RownoPredicate, /*CheckSame*/true, FillColAndSubQInfo);
    
    return rc;
}

void Query_SelectNode::RemoveOuterJoinFlag( SyntaxTreeNodeNo Node )
{
    if ( Node > 0 )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
        if ( SyntaxTree[Node].n_proc != a63query_spec )
        {
            if ( SyntaxTree[Node].n_proc == a64 
                && (   SyntaxTree[Node].n_subproc() == cak_x_left_outer_join 
                    || SyntaxTree[Node].n_subproc() == cak_x_right_outer_join 
                    || SyntaxTree[Node].n_subproc() == cak_x_full_outer_join ) )
                SyntaxTree[Node].n_subproc() = 0;

            RemoveOuterJoinFlag(SyntaxTree[Node].n_lo_level);
            RemoveOuterJoinFlag(SyntaxTree[Node].n_lo_level);
        }
    }
}

SyntaxTreeNodeNo Query_SelectNode::ExtractPredicate(SyntaxTreeNodeNo& CurrentTerm)
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    
    // extract from the APTree 
    SyntaxTreeNodeNo ReturnNode = CurrentTerm;
    SyntaxTreeNodeNo H = SyntaxTree[CurrentTerm].n_refback;
    if ( SyntaxTree[H].n_lo_level == CurrentTerm || H <= 0 )
    {
        // first term
        if ( H > 0 && SyntaxTree[H].n_proc == a64 && SyntaxTree[H].n_symb == s_and )
        {
            SyntaxTreeNodeNo Next = CurrentTerm;

            if ( (SyntaxTree[Next].n_proc == no_proc && SyntaxTree[Next].n_symb == s_not ) )
                Next = SyntaxTree[Next].n_sa_level;
            SyntaxTreeNodeNo CurrentLastNode = Next;
            Next = SyntaxTree[Next].n_sa_level;

            SyntaxTree[CurrentLastNode].n_sa_level = 0;

            SyntaxTreeNodeNo H2 = SyntaxTree[H].n_refback;

            SyntaxTree[H].n_lo_level = 0;
            SyntaxTree[H].n_refback  = 0;
            SyntaxTree.SetUnusedNode(H, false);

            if ( SyntaxTree[H2].n_sa_level == H )
            {
                SyntaxTree[H2].n_sa_level = Next;
            }
            else
            {
                // H2 points to a63/10 
//                SAPDBERR_ASSERT_STATE( H2 == SearchConditionStart() );
                SyntaxTree[H2].n_lo_level = Next;
            }
            if ( Next > 0 )
                SyntaxTree[Next].n_refback = H2;
            CurrentTerm = H2;
        }
        else
        {
            // H points to a63/10 or is eq cak_qrewrite_ref_init
            SAPDBERR_ASSERT_STATE( H == SearchConditionStart() || H == cak_qrewrite_ref_init);
            SyntaxTree[CurrentTerm].n_refback = 0;
            if ( H > 0 )
                SyntaxTree[H].n_lo_level = 0;
            CurrentTerm = cak_qrewrite_ref_init;
        }
    }
    else
    {
        // second term of the last AND!
        SyntaxTree[H].n_sa_level = 0;
        SyntaxTree[CurrentTerm].n_refback = 0;
        do
        { 
            H = SyntaxTree[H].n_refback;
        }while ( ! (SyntaxTree[H].n_proc == a64 && SyntaxTree[H].n_symb == s_and) );
        // H points to the and node

        SyntaxTreeNodeNo Next = SyntaxTree[H].n_lo_level;
        SyntaxTreeNodeNo H2 = SyntaxTree[H].n_refback;

        SyntaxTree[H].n_lo_level = 0;
        SyntaxTree[H].n_refback  = 0;
        SyntaxTree.SetUnusedNode(H, false);

        if ( SyntaxTree[H2].n_sa_level == H )
        {
            SyntaxTree[H2].n_sa_level = Next;
        }
        else
        {
            // H2 points to a63/10 
//            SAPDBERR_ASSERT_STATE( H2 == SearchConditionStart() );   
            SyntaxTree[H2].n_lo_level = Next;
        }
        if ( Next > 0 ) 
            SyntaxTree[Next].n_refback = H2;
        CurrentTerm = 0;
    }
    return ReturnNode;
}

SyntaxTreeNodeNo Query_SelectNode::ExtractPredicate(SyntaxTreeNodeNo& CurrentTerm, Query_Expression& OldPredicates, Query_Expression& Predicate, Container_Vector<SAPDB_Int2>& Cols)
{
    // 1. extract from the APTree 
    SyntaxTreeNodeNo ReturnNode = ExtractPredicate(CurrentTerm);

	// 2. extract from mSearchConditionColumns and add them to Predicate
	ExtractPredicate(OldPredicates, Predicate, Cols);

    return ReturnNode;
}

SyntaxTreeNodeNo Query_SelectNode::ExtractPredicate(Query_Expression& OldPredicates, Query_Expression& Predicate, Container_Vector<SAPDB_Int2>& Cols)
{
    SAPDB_Int2 n = Cols.GetSize();

    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        Query_ExpressionNode* N = OldPredicates.GetRoot(), *S=0;
        SAPDB_Int2 QNode = Cols[i];
        while ( N && N->GetSyntaxNode() != QNode )
        {
            S = N;
            N = N->mNext;
        }
        if ( N )
        {
            if ( S )
                S->mNext = N->mNext;
            else
                OldPredicates.SetRoot(N->mNext);
            N->mNext = 0;
            Predicate.AddNodes(N);
        }
        else
        {
            mQueryGraph.SetError("ExtractPredicate");
        }
    }
	return 0;
}


bool Query_SelectNode::PushDownSearchCond( void )
{   
    bool rc = true, pushed = false;
    Container_Vector<SAPDB_Int2> Cols(mAllocator);
    Container_Vector<SyntaxTreeNodeNo> Subqueries(mAllocator);
    Cols.Reserve(10);
    Subqueries.Reserve(5);
    SyntaxTreeNodeNo CurrentTerm = NextAndTerm(SearchConditionStart());
    do 
    {
        SyntaxTreeNodeNo QNode = cak_qrewrite_ref_init;
        Cols.Clear();
        bool OuterJoinFlag = false, RownoPredicate = false;
        if ( IsLocalPredicate(CurrentTerm, QNode, Cols, Subqueries, OuterJoinFlag, RownoPredicate, true) )
        {
            if ( QNode>0 && !RownoPredicate )
            {
                if ( !OuterJoinFlag || mFQuantifierCnt==1 )  
                {
                    SAPDB_Int2 k=-1;
                    if ( GetQuantifierNoFromSyntaxNode(QNode, k) )  
                    {
                        if ( mQuantifier[k]->NodeTypeSelectOrSet() )
                        {
                            Query_Expression Predicate;
                            SyntaxTreeNodeNo SearchCond = ExtractPredicate(CurrentTerm, mSearchConditionColumns, Predicate, Cols);
                            if ( OuterJoinFlag )
                                RemoveOuterJoinFlag(SearchCond);
                            rc = mQuantifier[k]->PullDownSearchCond(Predicate, SearchCond, mQuantifier[k]->NodeID(), false);
                            pushed = true;
                        }
                    }
                    //else
                    //{ // his may happen if there is a correlation and it is no error !
                    //}
                }
            }
        }
    }
    while ( rc && (CurrentTerm = NextAndTerm(CurrentTerm)) > 0 );

    if ( rc )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
        if ( SyntaxTree[SearchConditionStart()].n_lo_level <= 0 )
        {
            SyntaxTree.UnlinkNode(mSearchConditionStartNo, true);
            mSearchConditionStartNo = cak_qrewrite_ref_init;
        }
    }
    else
    {
        mQueryGraph.SetError("PushDownSearchCond");
    }
    mQualificationPushedDown = true;
    return pushed;
}

bool Query_SelectNode::PullDownSearchCond( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, SAPDB_Int2 QID, bool MustCreateCopy )
{   
    if ( !MustCreateCopy )
    {
        SearchCondition.ExchangeQuantifierID(QID, mID);
        return AddSearchCondition(SearchCondition, SearchCond_AP, /*MapColumns=*/true, MT_NoMerge, /*AsWhere=*/true);
    }
    else
    {
   		Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

		Query_NodeMapper NodeMapper = Query_NodeMapper(mAllocator);
        SyntaxTreeNodeNo SearchCond_AP2 = SyntaxTree.DoubleTreePart(SearchCond_AP, &NodeMapper, /*MapNodes*/true, /*WithSameLevel*/true);
        if ( SearchCond_AP2 > 0 )
        {
            Query_QuantifierMapper QMapper = Query_QuantifierMapper(mAllocator);
            QMapper.AddMappingInfo(QID, mID);

            Query_Expression SearchCondition2 = SearchCondition.CreateDuplicate2(mAllocator, SyntaxTree, &NodeMapper, &QMapper);
            return AddSearchCondition(SearchCondition2, SearchCond_AP2, /*MapColumns=*/true, MT_NoMerge, /*AsWhere=*/true);
        }
        else
        {
            return false;
        }
    }
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::SplitAndAddSearchCondition( Query_Expression& SearchCondition, SyntaxTreeNodeNo AddSearchCond, bool MapColumns, Query_MergeType MergeType, bool AsWhere, bool OuterJoinPredicates )
{
    bool rc = true;
    if ( !Aggregates() && !OuterJoinPredicates )
    {
        rc = AddSearchCondition(SearchCondition, AddSearchCond, MapColumns, MergeType, AsWhere);
    }
    else
    {
        Container_Vector<SAPDB_Int2> Cols(mAllocator);
        Container_Vector<SyntaxTreeNodeNo> Subqueries(mAllocator);
        Cols.Reserve(10);
        Subqueries.Reserve(5);
        SyntaxTreeNodeNo CurrentTerm = AddSearchCond; // NextAndTerm(AddSearchCond);

        do 
        {
            SyntaxTreeNodeNo QNode = cak_qrewrite_ref_init;
            Cols.Clear();
            bool OuterJoinFlag = false, RownoPredicate = false;
            bool local = IsLocalPredicate(CurrentTerm, QNode, Cols, Subqueries, OuterJoinFlag, RownoPredicate, true); 
            // DDT : Method should be renamed
            // as we don't care if its a local predicate... 
            Query_Expression Predicate;
            SyntaxTreeNodeNo SearchCond = ExtractPredicate(CurrentTerm, SearchCondition, Predicate, Cols);
            if ( local && OuterJoinFlag ) // DDT : ups ... we do care !!????
                RemoveOuterJoinFlag(SearchCond);

            rc = AddSearchCondition(Predicate, SearchCond, MapColumns, MergeType, AsWhere);
        }
        while ( rc && (CurrentTerm = NextAndTerm(CurrentTerm)) > 0 );
    }
    return rc;
}


bool Query_SelectNode::AddSearchCondition( Query_Expression& SearchCondition, SyntaxTreeNodeNo AddSearchCond, bool MapColumns, Query_MergeType MergeType, bool AsWhere )
{
    mQualificationPushedDown = false; 
    mSimplifiedPredicates = false;
    if ( AddSearchCond > 0 )
    {
        Query_OutColType ColType;

        if ( MapColumns )
        {
            Query_MappingInfo MappingInfo( ColType, MergeType, P_Where, false);
            if ( !SearchCondition.MappFromOuterToInnerSelect(this, &MappingInfo) )
            {
				mQueryGraph.SetError("MapColumns");
                return false;
            }
        }

        if ( AsWhere && !ColType.ContainsAggregate() )
        {
            if ( mSearchConditionColumns.AddWithAnd(SearchCondition) )
                return AddQualification(AddSearchCond, /*AsWhere=*/true);
        }
        else
        {
            if ( ColType.ContainsAggregate() )
                mAggregatesNotInOutput = true;
            if ( mHavingColumns.AddWithAnd(SearchCondition) )
                return AddQualification(AddSearchCond, /*AsWhere=*/false);
        }
        return false;
    }
    return true;
}

bool Query_SelectNode::AddQualification( SyntaxTreeNodeNo AddSearchCond, bool AsWhere )
{
    if ( AddSearchCond > 0 )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

        SyntaxTreeNodeNo SearchCond = ( AsWhere ? SearchConditionStart() : HavingStart() );
        if ( SearchCond > 0 )
        {
            // create AND-Node
            SyntaxTreeNodeNo AndNode = SyntaxTree.GetFreeNode();
            if ( AndNode > 0 )
            {
                SyntaxTree[AndNode].n_proc.becomes(a64);
                SyntaxTree[AndNode].n_symb.becomes(s_and);

                SyntaxTreeNodeNo AddPredicates = AddSearchCond; 

				// find correct predicate node to insert be new ones 
                // 'and' is the problem as those nodes must be in the correct arrangement !!!
				SyntaxTreeNodeNo Father     = SearchCond;
                SyntaxTreeNodeNo Predicates = SyntaxTree[SearchCond].n_lo_level;
                SyntaxTreeNodeNo PNo        = Predicates;

				if ( SyntaxTree[PNo].n_proc == a64 && SyntaxTree[PNo].n_symb == s_and )
				{
					while ( SyntaxTree[PNo].n_proc == a64 && SyntaxTree[PNo].n_symb == s_and )
					{
						PNo = SyntaxTree[PNo].n_lo_level;
						while ( SyntaxTree[PNo].n_sa_level > 0 )
						{
    						PNo = SyntaxTree[PNo].n_sa_level;
                        }
					}
                
					Father = SyntaxTree[PNo].n_refback;
                    if ( SyntaxTree[Father].n_proc == no_proc && SyntaxTree[Father].n_symb == s_not )
                    {
    					PNo = Father;
                        Father = SyntaxTree[PNo].n_refback;
                    }
				}
				SyntaxTree.UnlinkNodeWithSameLevel(PNo, /*SetUnused*/ false);
                if ( Father == SearchCond )
    				SyntaxTree.SetSucessorOnLowLevel(Father, AndNode);
                else
    				SyntaxTree.SetSucessorOnSameLevel(Father, AndNode);
				SyntaxTree.SetSucessorOnLowLevel(AndNode, AddPredicates);
				SyntaxTree.SetSucessorOnLowLevel(AndNode, PNo);
                return true;
            }
            else
            {
				mQueryGraph.SetError("Missing FreeNode");
                return false;
            }
        }
        else
        {
            SyntaxTreeNodeNo NewNo = SyntaxTree.GetFreeNode();            
            if ( NewNo > 0 )
            {
		        SyntaxTree[NewNo].n_proc.becomes(a63);
                SyntaxTree[NewNo].n_subproc() = ( AsWhere ? cak_x_search_condition : cak_x_having );
                SyntaxTree[NewNo].n_lo_level = AddSearchCond;
                SyntaxTree[AddSearchCond].n_refback = NewNo;

                SyntaxTreeNodeNo H;
                if ( AsWhere )
                {
                    H = FromPartStart();
                }
                else
                {
                    H = GroupByStart();
                    if ( H<=0 )
                        H = SearchConditionStart();
                    if ( H<=0 )
                        H = FromPartStart();
                }
                if ( H<=0 )
                    return false;

                //while ( SyntaxTree[H].n_sa_level > 0 )
                //{   
                //    H = SyntaxTree[H].n_sa_level;
                //}

                SyntaxTree.SetSucessorOnSameLevel(H, NewNo);
                if ( AsWhere )
                    SetSearchConditionStart(NewNo);
                else
                    SetHavingStart(NewNo);
                return true;
            }
            return false;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::AddGroupBy( Container_Vector<Query_ColumnSpec*>*& pGroupBySpec, SyntaxTreeNodeNo GroupBy_AP, bool MapColumns, Query_MergeType MergeType )
{
    Query_OutColType ColType;

    Query_MappingInfo MappingInfo( ColType, MergeType, P_GroupBy, false);
    SAPDB_Int2 n= pGroupBySpec->GetSize();

    if ( n>0 && !mpGroupByColumns )
        mpGroupByColumns = new(mAllocator)  Container_Vector<Query_ColumnSpec*>(mAllocator);
    if ( !mpGroupByColumns )
        return false;

  //  for ( SAPDB_Int2 i=0; i<n; i++ )
  //  {
  //      if ( MapColumns )
  //          if ( !(*pGroupBySpec)[i]->GetExpression().MappFromOuterToInnerSelect(this, &MappingInfo) )
  //          {
		//	    mQueryGraph.SetError("MapColumns");
  //              return false;
  //          }

		//// DDT : add only whose that still have a Root as with AddOrderBy ?

  //      mpGroupByColumns->InsertEnd((*pGroupBySpec)[i]);
  //      (*pGroupBySpec)[i]=0;
  //  }

	SAPDB_Int2 Cnt=0;
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        if ( MapColumns )
		{
			if ( (*pGroupBySpec)[i]->GetExpression().GetRoot() )
			{
				if ( !(*pGroupBySpec)[i]->GetExpression().MappFromOuterToInnerSelect(this, &MappingInfo) )
				{
					mQueryGraph.SetError("MapColumns");
					return false;
				}
				if ( (*pGroupBySpec)[i]->GetExpression().GetRoot() )
				{
					mpGroupByColumns->InsertEnd((*pGroupBySpec)[i]);
					++Cnt;
				}
				else
				{
					Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
					SyntaxTreeNodeNo Help = (*pGroupBySpec)[i]->GetSyntaxNode();
					SyntaxTree.UnlinkNode(Help, /*SetUnused*/ true);
					(*pGroupBySpec)[i]->Destroy(mAllocator);
					destroy((*pGroupBySpec)[i], mAllocator);
				}
			}
			else
			{
				mpGroupByColumns->InsertEnd((*pGroupBySpec)[i]);
				++Cnt;
			}
		}
		else
		{
	        mpGroupByColumns->InsertEnd((*pGroupBySpec)[i]);
			++Cnt;
		}
        (*pGroupBySpec)[i]=0;
    }

	if ( Cnt )
	{
		if ( ColType.ContainsAggregate() )
			mAggregatesNotInOutput = true;

		return AddGroupBy(GroupBy_AP);
	}
	return true;
}

bool Query_SelectNode::AddGroupBy( SyntaxTreeNodeNo AddGroupBy )
{
    if ( AddGroupBy > 0 )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

        SyntaxTreeNodeNo GroupBy = GroupByStart();
        if ( GroupBy > 0 )
        {
            GroupBy = SyntaxTree[GroupBy].n_lo_level;
            while ( SyntaxTree[GroupBy].n_sa_level )
            {
                GroupBy = SyntaxTree[GroupBy].n_sa_level;
            }
    		SyntaxTree.SetSucessorOnSameLevel(GroupBy, AddGroupBy);
            mOrderByEliminated = !(HasOrderBy() && HasGroupBy());
            return true;
        }
        else
        {
            SyntaxTreeNodeNo NewNo = SyntaxTree.GetFreeNode();            
            if ( NewNo > 0 )
            {
		        SyntaxTree[NewNo].n_proc.becomes(a63);
                SyntaxTree[NewNo].n_subproc() = cak_x_group_by;
                SyntaxTree[NewNo].n_lo_level = AddGroupBy;
                SyntaxTree[AddGroupBy].n_refback = NewNo;

                SyntaxTreeNodeNo H = SearchConditionStart();
                if ( H<=0 )
                    H = FromPartStart();

                SyntaxTree.SetSucessorOnSameLevel(H, NewNo);
                SetGroupByStart(NewNo);
                mOrderByEliminated = !(HasOrderBy() && HasGroupBy());
                return true;
            }
            return false;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::AddOrderBy( Container_Vector<Query_ColumnSpec*>*& pOrderBySpec, SyntaxTreeNodeNo OrderBy_AP, bool MapColumns, Query_MergeType MergeType )
{
    Query_OutColType ColType;

    Query_MappingInfo MappingInfo( ColType, MergeType, P_OrderBy, false);
    SAPDB_Int2 n= pOrderBySpec->GetSize();

    if ( n>0 && !mpOrderByColumns )
        mpOrderByColumns = new(mAllocator)  Container_Vector<Query_ColumnSpec*>(mAllocator);
    if ( !mpOrderByColumns )
        return false;

	SAPDB_Int2 Cnt=0;
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        if ( MapColumns )
		{
			if ( (*pOrderBySpec)[i]->GetExpression().GetRoot() )
			{
				if ( !(*pOrderBySpec)[i]->GetExpression().MappFromOuterToInnerSelect(this, &MappingInfo) )
				{
					mQueryGraph.SetError("MapColumns");
					return false;
				}
				if ( (*pOrderBySpec)[i]->GetExpression().GetRoot() )
				{
					mpOrderByColumns->InsertEnd((*pOrderBySpec)[i]);
					++Cnt;
				}
				else
				{
					Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
					SyntaxTreeNodeNo Help = (*pOrderBySpec)[i]->GetSyntaxNode();
					SyntaxTree.UnlinkNode(Help, /*SetUnused*/ true);
					(*pOrderBySpec)[i]->Destroy(mAllocator);
					destroy((*pOrderBySpec)[i], mAllocator);
				}
			}
			else
			{
				mpOrderByColumns->InsertEnd((*pOrderBySpec)[i]);
				++Cnt;
			}
		}
		else
		{
	        mpOrderByColumns->InsertEnd((*pOrderBySpec)[i]);
			++Cnt;
		}
        (*pOrderBySpec)[i]=0;
    }

	if ( Cnt )
	{
		if ( ColType.ContainsAggregate() )
			mAggregatesNotInOutput = true;

		bool rc = AddOrderBy(OrderBy_AP);
		if ( ColType.ContainsAggregate() && rc )
		{
			Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
			SyntaxTree[mOrderByStartNo].n_symb.becomes(s_count);
		}
		return rc;
	}
	return true;
}

bool Query_SelectNode::AddOrderBy( SyntaxTreeNodeNo AddOrderBy )
{
    if ( AddOrderBy > 0 )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

        SyntaxTreeNodeNo OrderBy = OrderByStart();
        if ( OrderBy > 0 )
        {
            mQueryGraph.SetError("AddOrderBy"); // there must be no OrderBy before
            return false;
        }
        else
        {
            SyntaxTreeNodeNo H = HavingStart();
            if ( H<=0 )
                H = GroupByStart();
            if ( H<=0 )
                H = SearchConditionStart();
            if ( H<=0 )
                H = FromPartStart();

            SyntaxTree.SetSucessorOnSameLevel(H, AddOrderBy);
            SetOrderByStart(AddOrderBy);
            mOrderByEliminated = !(HasOrderBy() && HasGroupBy());
            return true;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::AddLockOptions( SyntaxTreeNodeNo AddLockOptions )
{
    if ( AddLockOptions > 0 )
    {
        Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

        SyntaxTreeNodeNo LockOptions = LockOptionStart();
        if ( LockOptions > 0 )
        {
            mQueryGraph.SetError("AddLockOptions"); // there must be no LockOptions before
            return false;
        }
        else
        {
            SyntaxTreeNodeNo H = FromPartStart();
            while ( SyntaxTree[H].n_sa_level > 0 )
            {   
                H = SyntaxTree[H].n_sa_level;
            }

            SyntaxTree.SetSucessorOnSameLevel(H, AddLockOptions);
            SetLockOptionStart(AddLockOptions);
            return true;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::OneTupleAggregation( void )
{
	return ( mAggregatesInOutput && !HasGroupBy() ); // this is ok, if the semantic analysis was done correctly !
}

bool Query_SelectNode::OneTupleCondition( void )
{
    if ( mAggregatesInOutput )
        if ( OneTupleAggregation() )
            return true;

    // check if equal conditions define a onetuple condition
    // DDT : TODO

    return false;
}

bool Query_SelectNode::NoDuplicateCondition( void )
{
    // NOTE: we will check the output list only! 
    // therefore NoDuplicateCOndition may be FALSE even if OneTupleCondition is TRUE !!!
    if ( mNoDuplicateCondition == QNULL )
    {
        mNoDuplicateCondition = QTRUE;

        SAPDB_Int2 nq = mQuantifier.GetSize();
        SAPDB_Int2 nc = mpOutputColumns->GetSize();

        Container_Vector<SAPDB_Int2> LogOutColNr(mAllocator);
        LogOutColNr.Reserve(nc);

        // check if output contains full key or unique index of all quantifier
        for ( SAPDB_Int2 q=0; q<nq && mNoDuplicateCondition==QTRUE; q++ )
        {
            Query_Quantifier* Q = mQuantifier[q];
            if ( Q->QuantifierType() == QT_F )
            {
                LogOutColNr.Clear();
                for ( SAPDB_Int2 c=0; c<nc; c++ )
                {
                    Query_OutputColumn* Col = (*mpOutputColumns)[c];
                    if ( Col->GetMainColType() == Query_OutColType::OCTM_Column )
                    {
                        if ( Col->GetQuantifierID() == Q->NodeID() )
                        {
                            LogOutColNr.InsertEnd(Col->GetLogicalColumnIndex());
                        }
                    }
                }
                if ( !Q->NodeRef()->DefineUniqueOutput(LogOutColNr) )
                {
                    mNoDuplicateCondition=QFALSE;
                    break;
                }
            }
        }
    }

    return ( mNoDuplicateCondition==QTRUE );
}

bool Query_SelectNode::DefineUniqueOutput(Container_Vector<SAPDB_Int2>& LogOutColNr)
{
    // NOTE: we will check the output list only! 
    // therefore DefineUniqueOutput may be FALSE even if OneTupleCondition is TRUE !!!

    SAPDB_Int2 nc = LogOutColNr.GetSize();
    if ( nc > 0 )
    {
        SAPDB_Int2 nq = mQuantifier.GetSize();
        Container_Vector<SAPDB_Int2> LogOutColNr2(mAllocator);
        LogOutColNr2.Reserve(nc);

        // check if output contains full key or unique index of all quantifier
        for ( SAPDB_Int2 q=0; q<nq; q++ )
        {
            Query_Quantifier* Q = mQuantifier[q];
            if ( Q->QuantifierType() == QT_F )
            {
                LogOutColNr2.Clear();
                for ( SAPDB_Int2 c=0; c<nc; c++ )
                {
                    // Query_OutputColumn* Col = GetOutputColumnPointer(LogOutColNr[c]);  // this is always correct
					Query_OutputColumn* Col = (*mpOutputColumns)[LogOutColNr[c]-1];		// this is correct only if there are no asterisk left !!!
					if ( Col )
					{
						if ( Col->GetMainColType() == Query_OutColType::OCTM_Column )
						{
							if ( Col->GetQuantifierID() == Q->NodeID() )
							{
								LogOutColNr2.InsertEnd(Col->GetLogicalColumnIndex());
							}
						}
					}
					else
					{
						return false;
					}
                }
                if ( !Q->NodeRef()->DefineUniqueOutput(LogOutColNr2) )
                {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::ReplaceAsteriskReferToLowerNodeForMerge( Query_Node* LowerNode )
{
    // First replace all asterisk that refer to more than one quantifier
    ResolveAllMultiAsteriskIntoSingleAsterisk();

    SAPDB_Int2 n = mpOutputColumns->GetSize();
    SyntaxTreeNodeNo LowerNodeNo = LowerNode->SyntaxNode();
    for ( SAPDB_Int2 i=0; i<n; i++ )
    {
        SAPDBERR_ASSERT_STATE( (*mpOutputColumns)[i]->GetMainColType() != Query_OutColType::OCTM_MultiAsterisk );
        if ( (*mpOutputColumns)[i]->GetMainColType() == Query_OutColType::OCTM_MultiAsterisk )
            return false;

        if ( (*mpOutputColumns)[i]->GetMainColType() == Query_OutColType::OCTM_Asterisk 
             && LowerNodeNo == (*mpOutputColumns)[i]->GetQuantifierSyntaxNode() )
        {
            // resolvesingleasterisk may result in to much colums if the inner selectlist contains a '*'
            if ( !ResolveSingleAsterisk(i,/*IncRefCounts=*/false) )
            {
				mQueryGraph.SetError("ResolveSingleAsterisk");
                return false;
            }
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::MappSelectListFromOuterSelect( Container_Vector<Query_OutputColumn*>* OutputColumns, Query_MergeType MergeType )
{
    SAPDBERR_ASSERT_ARGUMENT( OutputColumns );
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( OutputColumns )
    {
        bool rc = true;
        SAPDB_Int2 n = OutputColumns->GetSize();
        for ( SAPDB_Int2 i=0; (i<n) && rc; i++ )
        {
            Query_OutputColumn* C = (*OutputColumns)[i];
            bool AdjustRefname  = (C->GetMainColType()==Query_OutColType::OCTM_Column) && !C->HasReferenceName(); // DDT

            Query_MappingInfo MappingInfo( C->GetColType(), MergeType, P_SelectList, AdjustRefname);
            rc &= C->GetExpression().MappFromOuterToInnerSelect(this, &MappingInfo);
            if ( rc && AdjustRefname )
                C->RenewReferenceName(mQueryGraph);
        }
        return rc;
    }
    return false;
}

bool Query_SelectNode::MappColumnFromOuterSelect( Query_Column* Column, Query_MappingInfo* MappingInfo )
{
    SAPDBERR_ASSERT_ARGUMENT( Column );
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( Column )
    {
        // map Column if it refers to this SelectNode only !!!
        if ( Column->GetQuantifierID() != NodeID() )
            return true;    // nothing to do...

        // Column comes from the outer select and is pushed down to 'this'
        // Column.ColumnIndex  > No of outputcolumn of 'this'

        SQLMan_Identifier ColName(false), RefName(false);
        SAPDB_Int2 QuantifierSyntaxNode=0, QuantifierID=0;
        SAPDB_Int2 ColumnIndex=0, ExtColNo=0, SelColNo=cak_qrewrite_ref_star;
        bool IsNullable=true;

	    SAPDB_Int2 OuterColIndex = Column->GetLogicalColumnIndex();	

        if ( GetOutputColumn(OuterColIndex, ColName, RefName, QuantifierSyntaxNode, QuantifierID, ColumnIndex, ExtColNo, SelColNo, IsNullable) )
        {
            Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

            // adjust the Column attributes
//            SAPDB_Int2 NewNodeID = (MappingInfo->mMergeType==MT_NoMerge ? mID : Column->GetNodeID() ); // NoMerge = PushDown
            SAPDB_Int2 NewNodeID = (Column->GetNodeID()>0 ? Column->GetNodeID() : -1 ); 
            Column->SetInfo(NewNodeID, QuantifierID, QuantifierSyntaxNode, ColName, ColumnIndex, ExtColNo, IsNullable);
            Query_OutputColumn* C = (*mpOutputColumns)[SelColNo];

            if ( MappingInfo->mMergeType == MT_NoMerge )
            {
                C->DecRefCount();
                if ( QuantifierID > 0 )
                    IncRefCountForOutputColumn(QuantifierID, ColumnIndex);
                else
                    C->IncRefCountsToChildColumns(this);
            }
//            mProjectionPulledDown = false;

            // we have to set the ColType according to what we mapp the column to
            if ( QuantifierSyntaxNode > 0 )
		    {
    		    // adjust the SyntaxNode attributes
                SyntaxTreeNodeNo H = Column->GetSyntaxNode();
			    SyntaxTree[H].n_fromtabnode()   = QuantifierSyntaxNode;
			    SyntaxTree[H].n_columnindex()   = ExtColNo;

                if ( MappingInfo->mAdjustRefname && !RefName.IsBlank() )
                {
                    SAPDBERR_ASSERT_STATE( SelColNo>=0 );
                    // add a reference node !
                    SyntaxTreeNodeNo NewRefNo = SyntaxTree.GetFreeNode();
            		if ( NewRefNo > 0 )
                    {
                        SyntaxTreeNodeNo OldRefNo = SyntaxTree[(*mpOutputColumns)[SelColNo]->GetSyntaxNode()].n_lo_level;
                        SyntaxTree[NewRefNo] = SyntaxTree[OldRefNo];
                        SyntaxTreeNodeNo A66 = SyntaxTree[H].n_refback;

                        SyntaxTree[A66      ].n_lo_level = NewRefNo;
                        SyntaxTree[NewRefNo ].n_refback  = A66;
                        SyntaxTree[NewRefNo ].n_sa_level = H;
                        SyntaxTree[H        ].n_refback  = NewRefNo;
                    }
                    else
                    {
                        return false;
                    }
                }

                if ( (MappingInfo->mMergeType==MT_Aggregates) && Column->InAggr() )
                {
                    // if columns is argument of SUM we have to change SUM to SUM( distinct )
                    SyntaxTreeNodeNo H = Column->GetSyntaxNode();
                    while ( H>0 )
                    {
                        if ( SyntaxTree.IsAggregationNode(H) )
                        {
                            if (SyntaxTree[H].n_symb == s_sum )
                                SyntaxTree[H].n_symb.becomes(s_dis_sum);
                            break;
                        }
                        H = SyntaxTree[H].n_refback;
                        SAPDBERR_ASSERT_STATE( H );
                    }
                }

                return true;
		    }
		    else
		    {
                switch ( C->GetMainColType() )
                {
                case Query_OutColType::OCTM_Aggregate: 
                    {
                        SyntaxTreeNodeNo HC = Column->GetSyntaxNode();
                        SyntaxTreeNodeNo HA = SyntaxTree[HC].n_refback;
                        if (    SyntaxTree.IsAggregationNode(HA) 
                             && SyntaxTree[HA].n_lo_level == HC )
                        {
                            // remove SUM in syntaxtree
                            Column->SetInAggr(false);
                            Column->SetSyntaxNode(HA);
                            SyntaxTreeNodeNo Sa = SyntaxTree[HA].n_sa_level;
                            SyntaxTreeNodeNo Rb = SyntaxTree[HA].n_refback;
                            SyntaxTree[HA] = SyntaxTree[HC];
                            SyntaxTree[HA].n_sa_level = Sa;
                            SyntaxTree[HA].n_refback  = Rb;
                            SyntaxTree.SetUnusedNode(HC, /*Recursive*/false);
                        }
                    }
                    // DO NOT BREAK HERE !!!
                case Query_OutColType::OCTM_Expression:
                    {
                        if ( !SetColTypeOnMapping(Column->InAggr(), C->GetColType(), MappingInfo->mColumnType, MappingInfo->mPosition) )
                            return false;

                        Query_NodeMapper mMapper(mAllocator);
                        SyntaxTreeNodeNo R = SyntaxTree[C->GetSyntaxNode()].n_lo_level;
                        SyntaxTreeNodeNo OldRefNo = R;
                        if ( SyntaxTree[R].n_proc == no_proc && SyntaxTree[R].n_symb == s_reference_name )
                            R = SyntaxTree[R].n_sa_level;
                        R = SyntaxTree.DoubleTreePart(R, &mMapper, /*MapNodes*/false, /*WithSameLevel*/true);
                        SyntaxTree.SubstituteNodeBySubtree(Column->GetSyntaxNode(), R, true);
                        Column->SetSyntaxNode(R);
                        //
                        if ( C->GetExpression().GetRoot() )
                        {
                            Query_Expression Help;
                            Help.AddNodes(C->GetExpression().GetRoot()->CreateDuplicate(mAllocator, SyntaxTree, &mMapper, 0));
                            if ( Column->InAggr() )
                                Help.SetInAggr();
                            Help.AddNodes(Column->mNext);
							Query_ExpressionNode* X = Help.GetRoot();
                            *Column = *((Query_Column*)(X));
							if ( X )
							{
								X->mNext = 0;
								Help.Destroy(mAllocator);
							}
                        }
                        else
                        {
                            NewNodeID = (Column->GetNodeID()>0 ? Column->GetNodeID() : -1 ); 
                            Column->SetInfo(NewNodeID, QuantifierID, QuantifierSyntaxNode, RefName, ColumnIndex, ExtColNo, /*IsNullable*/true);
                        }

                        if ( MappingInfo->mAdjustRefname && !RefName.IsBlank() )
                        {
                            // add a reference node !
                            SyntaxTreeNodeNo NewRefNo = SyntaxTree.GetFreeNode();
            		        if ( NewRefNo > 0 )
                            {
                                SyntaxTree[NewRefNo] = SyntaxTree[OldRefNo];
                                SyntaxTreeNodeNo A66 = SyntaxTree[R].n_refback;

                                SyntaxTree[A66      ].n_lo_level = NewRefNo;
                                SyntaxTree[NewRefNo ].n_refback  = A66;
                                SyntaxTree[NewRefNo ].n_sa_level = R;
                                SyntaxTree[R        ].n_refback  = NewRefNo;
                            }
                            else
                            {
                                return false;
                            }
                        }

                    }
                    return true; 
                    break;
                case Query_OutColType::OCTM_Subquery:
                    SAPDBERR_ASSERT_STATE( 0 );
    /*DDT*/		    mQueryGraph.SetNotYetImplemented("MappFromOuterToInnerSelect 2", false);
                    break;
                }
		    }
        }
	    else
	    {
		    mQueryGraph.SetError("MappFromOuterToInnerSelect 3", false);
	    }
    }
    return false;
}

bool Query_SelectNode::SetColTypeOnMapping( bool MappedColumnInAggr, Query_OutColType InnerColType, Query_OutColType& ResultingColType, Query_Position Pos )
{
    // set the ColType. We have to consider 
    // 1. MappedColumnInAggr : the column to be mapped is an argument of some aggregate function
    // 2. InnerColType       : the outcolumn type of the column the outer column is mapped to
    // 3. ColType            : the original outcolumntype of the outer outputcolumn

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    switch ( ResultingColType.MainType )
    {
    case Query_OutColType::OCTM_Column:
    case Query_OutColType::OCTM_Unknown:    // column that is mapped is not in the select list.
        ResultingColType = InnerColType;
        if ( MappedColumnInAggr )
        {
            if ( Pos==P_SelectList) mAggregatesInOutput=true; else mAggregatesNotInOutput=true;  // DDT : mCorrelatedAndAggregatedColumnsRefersToThis
            if ( InnerColType.ContainsAggregate() )
            {
                mQueryGraph.SetError("SetColTypeOnMapping 1");
                SAPDBERR_ASSERT_STATE( 0 );
                return false;
            }
        }
        break;
    case Query_OutColType::OCTM_Aggregate:
        if ( Pos==P_SelectList) mAggregatesInOutput=true; else mAggregatesNotInOutput=true;  // DDT : mCorrelatedAndAggregatedColumnsRefersToThis
        if ( InnerColType.MainType == Query_OutColType::OCTM_Expression )
        {
            ResultingColType.SubType |= Query_OutColType::OCTS_AggregationContainsExpression;
            switch ( Pos )
            {
            case P_SelectList:
                SyntaxTree.SetCountFlag(mSelectListStartNo);
                break;
            case P_Where:
            case P_Having:
                SyntaxTree.SetCountFlag(mHavingStartNo);
                break;
            }
        }
        if ( InnerColType.MainType == Query_OutColType::OCTM_Subquery )
        {
            ResultingColType.SubType |= Query_OutColType::OCTS_AggregationContainsSubquery;
        }
        ResultingColType.SubType |= InnerColType.SubType;
        break;
    case Query_OutColType::OCTM_Expression:
        if ( InnerColType.MainType == Query_OutColType::OCTM_Aggregate )
        {
            if ( Pos==P_SelectList) mAggregatesInOutput=true; else mAggregatesNotInOutput=true;  // DDT : mCorrelatedAndAggregatedColumnsRefersToThis
            ResultingColType.SubType |= Query_OutColType::OCTS_ExpressionContainsAggr;
            switch( Pos )
            {
            case P_SelectList:
                SyntaxTree[mSelectListStartNo].n_symb.becomes(s_sum);
                break;
            case P_Where:
            case P_Having:
                SyntaxTree[mHavingStartNo].n_symb.becomes(s_sum);
                break;
            }
        }
        if ( InnerColType.MainType == Query_OutColType::OCTM_Subquery )
        {
            ResultingColType.SubType |= Query_OutColType::OCTS_ExpressionContainsSubquery;
        }
        ResultingColType.SubType |= InnerColType.SubType;
        break;
    case Query_OutColType::OCTM_Asterisk:
    case Query_OutColType::OCTM_MultiAsterisk:
    case Query_OutColType::OCTM_Subquery:
    default:
        mQueryGraph.SetError("SetColTypeOnMapping 1");
        SAPDBERR_ASSERT_STATE( 0 );
        return false;
        break;
    }
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::SimplifyPredicates( void )
{
    mSimplifiedPredicates = true;
    bool applied = false;
    if ( HasSearchCondition() )
        applied |= SimplifyPredicates(mSearchConditionColumns, mSearchConditionStartNo);
    if ( HasHaving() )
        applied |= SimplifyPredicates(mHavingColumns, mHavingStartNo);
    return applied;
}

bool Query_SelectNode::SimplifyPredicates( Query_Expression& Predicates, SyntaxTreeNodeNo& PredicateStart_AP )
{
    bool simpl = false;
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

    // 1. eliminate A IS NULL for all columns where A IS NOT NULLABLE
    Query_ExpressionNode* pCol = Predicates.GetRoot();
    while ( pCol )
    {
        Query_ExpressionNode* pNext = pCol->mNext;
        if ( pCol->GetExpType() == Exp_Column && !((Query_Column*)pCol)->IsNullable() )
        {
            // col IS NOT NULLABLE !
            SyntaxTreeNodeNo ColNode = ((Query_Column*)pCol)->GetSyntaxNode();
            SyntaxTreeNodeNo Ba = SyntaxTree[ColNode].n_refback;
            SyntaxTreeNodeNo Sa = SyntaxTree[ColNode].n_sa_level;

            if ( Sa>0 && Ba>0 )
            {
                if (   SyntaxTree[Ba].n_lo_level == ColNode 
                    && SyntaxTree[Sa].n_proc == no_proc
                    && SyntaxTree[Sa].n_symb == s_null )
                {
                    if (   SyntaxTree[Ba].n_proc == a64 
                        && SyntaxTree[Ba].n_symb == s_is_null )
                    {
                        // col IS NULL -> FALSE
                        SyntaxTree[Ba].n_symb.becomes(s_is_true);
                        SyntaxTree[Sa].n_symb.becomes(s_true);
                        SyntaxTree[ColNode].n_proc.becomes(no_proc);
                        SyntaxTree[ColNode].n_subproc() = 0;
                        SyntaxTree[ColNode].n_symb.becomes(s_false);
                        // delete pCol
                        // Predicates.DelNode(pCol, mAllocator);
                        simpl = true;
                    }
                    else
                    {
                        if (   SyntaxTree[Ba].n_proc == a64 
                            && SyntaxTree[Ba].n_symb == s_is_not_null )
                        {
                            // col IS NOT NULL -> TRUE
                            SyntaxTree[Ba].n_symb.becomes(s_is_true);
                            SyntaxTree[Sa].n_symb.becomes(s_true);
                            SyntaxTree[ColNode].n_proc.becomes(no_proc);
                            SyntaxTree[ColNode].n_subproc() = 0;
                            SyntaxTree[ColNode].n_symb.becomes(s_true);
                            // delete pCol
                            // Predicates.DelNode(pCol, mAllocator);
                            simpl = true;
                        }
                    }
                }
            }
        }
        pCol = pNext;
    }

    // 2. replace boolean expressions 
    if ( !SyntaxTree.SimplifyPredicates(PredicateStart_AP, simpl, this) )
        mQueryGraph.SetError("SimplifyPredicates");

    // 3. remove duplicate factors 
    //      X and X         -> X
    //      X and Y and X   -> X and Y 
    //      X or X          -> X
    //      X or Y or X     -> X or Y

        // DDT: TODO  easy if on same level of one or/and node
        //            otherwise ...

    // 4. Check if there is only WHERE/HAVING TRUE left
    SyntaxTreeNodeNo H = SyntaxTree[PredicateStart_AP].n_lo_level;
    if ( SyntaxTree.BoolTermTRUE(H) )
    {
        SyntaxTree.UnlinkNode(PredicateStart_AP, /*SetUnused*/ true);
        PredicateStart_AP = cak_qrewrite_ref_init;
		RemoveDeletedColumns(Predicates);
        Predicates.Destroy(mAllocator);
    }
    else
    {
		RemoveDeletedColumns(Predicates); // Delete all Columns from Predicates that were eliminated by step 1 or 2 !
    }

    // 5. Check if subqueries were deleted too
	RemoveDeletedSubqueries();

    return simpl;
}

bool Query_SelectNode::PredicatesSimplified( void )
{
    return mSimplifiedPredicates;
}

bool Query_SelectNode::SubqueryOptimized( void )
{
    return mSubqueryOptimized;
}

void Query_SelectNode::SetSubqueryOptimized( void )
{
    mSubqueryOptimized = true;
}

// ------------------------------------------------------------------------------------------------

void Query_SelectNode::RemoveDeletedColumns( Query_Expression& Predicates )
{
	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	Query_ExpressionNode* pCol = Predicates.GetRoot();
    while ( pCol )
    {
        Query_ExpressionNode* pNext = pCol->mNext;
        if ( pCol->GetExpType() == Exp_Column  )
        {
            SyntaxTreeNodeNo ColNode = pCol->GetSyntaxNode();
            if ( !(SyntaxTree[ColNode].n_proc == no_proc && SyntaxTree[ColNode].n_symb == s_columnid) )
            {
                bool found = false;
                SAPDB_Int2 NID = pCol->GetNodeID();
                if ( NID > 0 )
                { // this is a correlated column !
                    Query_Node* FNode = FatherNode();
                    while ( FNode != 0 && !found)
                    {
                        if ( FNode->NodeID() == NID )
                        {
                            FNode->DecRefCountForOutputColumn(((Query_Column*)pCol)->GetQuantifierID(), ((Query_Column*)pCol)->GetLogicalColumnIndex());
                            if ( FNode->GetNodeType() == Query_Node::Select)
                            {
                                if ( ((Query_Column*)pCol)->InAggr() )
                                    ((Query_SelectNode*)FNode)->ChangeCorrelatedAndAggregatedColumnsRefersToMe(-1);
                                ((Query_SelectNode*)FNode)->ChangeCorrelatedColumnsRefersToMe(-1); // DDT : ELSE ?

                                ChangeCorrelatedColumns(-1);
                                found = true;
                                break;
                            }
                            else
                            {
                                mQueryGraph.SetError("Remove Correlation Info 1");
                                SAPDBERR_ASSERT_STATE( 0 );
                            }
                        }
                        FNode = FNode->FatherNode();
                    }
                    if ( !found )
                    {
                        mQueryGraph.SetError("Remove Correlation Info 2");
						return;
                    }
                }
				else
				{
	                DecRefCountForOutputColumn(((Query_Column*)pCol)->GetQuantifierID(), ((Query_Column*)pCol)->GetLogicalColumnIndex());
				}
                // delete pCol
                Predicates.DelNode(pCol, mAllocator);        
            }
        }
        pCol = pNext;
    }
}

void Query_SelectNode::RemoveDeletedSubqueries( void )
{
	Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SAPDB_Int2 nq = mQuantifier.GetSize();
    for ( SAPDB_Int2 i=nq-1; i>=0; i-- )
    {
        Query_Quantifier* Q = mQuantifier[i];
        if ( Q->QuantifierType() != QT_F )
        {
            SyntaxTreeNodeNo QNode = Q->NodeRef()->GetInternAPNode();
            if ( QNode > 0 )
            {
                if (   SyntaxTree[QNode].n_proc == no_proc 
                    && SyntaxTree[QNode].n_subproc() == 0
                    && SyntaxTree[QNode].n_symb == s_unknown )
                {
                    // DDT : if a correlated subquery is to be deleted their correlation info must be deleted too !!!

					switch ( Q->QuantifierType() )
					{
					case QT_F:
						--mFQuantifierCnt;
						break;
					case QT_A:
					case QT_E:
					case QT_S:
						--mWQuantifierCnt;
						break;
					case QT_L:
						--mLQuantifierCnt;
						break;
					case QT_Unknown:
					default:
						mQueryGraph.SetError("Unknown Quantifier type");
						SAPDBERR_ASSERT_STATE( 0 );
						break;
					}
                    destroy(Q, mAllocator);
                    mQuantifier.Delete(i); 

                }
            }
            else
            {
                mQueryGraph.SetError("Invalid SyntaxNode");
                SAPDBERR_ASSERT_STATE( 0 );
            }
        }
    }
}


// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::PullDownProjectionPossible( void )
{
	return (BodyDistinctType() != DT_Enforce);
}

bool Query_SelectNode::PullDownProjection( void )
{
    // Attention: If the select was "select DISTINCT" in the beginning (which may have changed inbetween due to nodup-condition) 
    // we may have to add the "DISTINCT" again !!!

    mProjectionPulledDown = true;
    bool removed = false;
    if ( mSubQueryType == SQT_None || mSubQueryType == SQT_EXISTS )
	{
        switch ( mPosition )
        {
        case P_Difference: 
        case P_Intersect:
            // DDT : we are allowed to purge only what is not needed by the set function 
            // don't do anything so far
            break;

        case P_Union: 
			SAPDBERR_ASSERT_STATE( BodyDistinctType() != DT_Enforce );

        case P_Where:
        case P_Having: 
            // these are exist quantifiers: WHERE EXISTS ( SELECT ... ) 
            // the select list doesn't matter, so we can purge any column 
        case P_FromPart:
            {
                SAPDB_Int2 n = mpOutputColumns->GetSize();
                SAPDB_Int2 c = 0;
                mAggregatesInOutput = false;
                for ( SAPDB_Int2 k=n-1; k>=0; k-- )
                {
					Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

					Query_OutputColumn* C = (*mpOutputColumns)[k];
					SAPDBERR_ASSERT_STATE( !C->IsAsterisk() );
                    if (   !C->GetRefCount() 
						&& !C->IsAsterisk() 
//						&& (!C->GetColType().ContainsAggregate() || mPosition==P_FromPart)
						&& !SyntaxTree.ContainsParameter(C->GetSyntaxNode(), /*ConsiderSame*/false)
						)
                    {   // no reference: output column can be removed !

						if ( k>0 || c>0 ) 
						{
							SyntaxTree.UnlinkNode(C->GetSyntaxNode(), /*SetUnused*/ true);                    

							DestroyOneOutputColumn(C, k, true);
							if ( C->GetColType().ContainsSubquery() )
								RemoveDeletedSubqueries();

							--mOutputColumnCnt;
							removed = true;
						}
						else
						{
							if ( !OneTupleAggregation() )
							{
								// we can't remove the last remaining output column but we may change it to a literal

								SyntaxTreeNodeNo X = C->GetSyntaxNode();
								X = SyntaxTree[X].n_lo_level;
								SyntaxTree.UnlinkNodeWithSameLevel(X, /*SetUnused*/ true);                    

								(*mpOutputColumns)[k]->DelRefCountsToChildColumns(this);
								C->GetExpression().Destroy(mAllocator);
								if ( C->GetColType().ContainsSubquery() )
									RemoveDeletedSubqueries();
								C->SetColType( Query_OutColType::OCTM_Expression, 0, 1);

								// add dummy column to syntax tree
								SyntaxTreeNodeNo Lit = SyntaxTree.GetFreeNode();
								if ( Lit > 0 )
								{
									SyntaxTree[Lit].n_proc.becomes(no_proc);
									SyntaxTree[Lit].n_symb.becomes(s_string_literal);
									SyntaxTree[Lit].n_pos()		= 1;
									SyntaxTree[Lit].n_length()	= 2;
		
									X = C->GetSyntaxNode();
									SyntaxTree[X].n_lo_level = Lit;
									SyntaxTree[Lit].n_refback= X;
								}
								else
								{
									mQueryGraph.SetError("Add dummy literal");
									return false;
								}
								mContext.SQLManContext().a_count_literals += 1;

								removed = true;
							}
							else
							{
								SAPDBERR_ASSERT_STATE( C->GetColType().ContainsAggregate() );
								if ( C->GetColType().ContainsAggregate() )
									mAggregatesInOutput = true;
								++c;								
							}
						}
                    }
                    else
                    {
                        if ( C->GetColType().ContainsAggregate() )
                            mAggregatesInOutput = true;
                        ++c;
                    }
                }
                if ( removed )
                {
                    // as outputcolumns were deleted all columns that refere to THIS must be changed!
                    if ( !this->FatherNode()->RenewSemanticsForColumnsDueToColumnPrivs(this) )
                        mQueryGraph.SetError("RenewSemanticsOnPullDownProjection"); 
					if ( mAggregatesInOutput )
						DetermineCountOrSumInSelListNode();

                    mNoDuplicateCondition = QNULL;
                }
            }
            break;
        }
	}
    return removed;
}

bool Query_SelectNode::ProjectionPulledDown( void )
{
    return mProjectionPulledDown;
}

// ------------------------------------------------------------------------------------------------

bool Query_SelectNode::EliminateOrderBy( void )
{
    mOrderByEliminated = true;

	// OrderBy is unnecessary as the correct order is insured by the group by!
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo OrderBy = OrderByStart();
    SyntaxTree.UnlinkNode(OrderBy, /*SetUnused*/ true);
    SetOrderByStart(cak_qrewrite_ref_init);
    DestroyColumnSpec(mpOrderByColumns);
    return true;
}

bool Query_SelectNode::OrderByEliminated( void )
{
    return mOrderByEliminated;
}

// ------------------------------------------------------------------------------------------------
