/*!***************************************************************************

    @file     Query_NodeSet.cpp
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

#include "SAPDB/SQLManager/Query/Query_NodeSet.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Expression.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"

// ------------------------------------------------------------------------------------------------

Query_SetNode::Query_SetNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode, bool All )
    : Query_Node(Context, QueryGraph, Position, APNode, (All?false:true), (All?DT_Preserve:DT_Enforce))
    , mInternAPNode(APNode)
    , mAll(All)
    , mFirstFlg(false)
    , mReferenceName(false)
    , mProjectionPulledDown(false)
    , mSubQueryType(SQT_None)
{
    DetermineAPNode();

    if ( mPosition == P_FromPart )
        mQueryGraph.FromSelectFound();
}

void Query_SetNode::DetermineAPNode( void )
{ 
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    SyntaxTreeNodeNo H = SyntaxTree[mInternAPNode].n_refback;
    bool RefName = false;
    if ( SyntaxTree[H].n_proc == a63 && SyntaxTree[H].n_subproc() == cak_x_start_union ) 
    {
	    SyntaxTreeNodeNo Sa = SyntaxTree[H].n_sa_level;
	    if ( Sa > 0 )
	    {
		    if ( SyntaxTree[Sa].n_symb == s_identifier )
		    {
                RefName = true;
			    mQueryGraph.GetIdentifier(Sa, mReferenceName);
		    }
	    }
    	mFirstFlg = true;
        H = SyntaxTree[H].n_refback;
        if ( SyntaxTree[H].n_proc == a66 && SyntaxTree[H].n_subproc() == cak_x_select_in_from_part )
            mAPNode = H;

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
    if ( !RefName )
        mReferenceName.SetBlank();
}

// copy constructor
Query_SetNode::Query_SetNode( const Query_SetNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : Query_Node(N, Father, Q, mpMap, mpQMap)
    , mInternAPNode(N.mInternAPNode)
    , mReferenceName(N.mReferenceName)
    , mAll(N.mAll)
    , mFirstFlg(N.mFirstFlg)
    , mProjectionPulledDown(N.mProjectionPulledDown)
    , mSubQueryType(N.mSubQueryType)
{
    mQueryGraph.SyntaxTree().MapNode(mInternAPNode, mpMap);
    if ( mPosition == P_FromPart )
        mQueryGraph.FromSelectFound();
}

// ------------------------------------------------------------------------------------------------

void Query_SetNode::AddJoinType( Query_JoinType JT )
{
    Query_Quantifier*& Q1 = mQuantifier[0];
    Query_Quantifier*& Q2 = mQuantifier[1];

    Q1->NodeRef()->AddJoinType(JT);
    Q2->NodeRef()->AddJoinType(JT);
}

bool Query_SetNode::OuterJoin( void )
{
    Query_Quantifier*& Q1 = mQuantifier[0];
    Query_Quantifier*& Q2 = mQuantifier[1];

    return (Q1->OuterJoin() || Q2->OuterJoin());
}

bool Query_SetNode::Aggregates( void )
{
    Query_Quantifier*& Q1 = mQuantifier[0];
    Query_Quantifier*& Q2 = mQuantifier[1];

    return (Q1->NodeRef()->Aggregates() || Q2->NodeRef()->Aggregates());
}

bool Query_SetNode::AggregatesInOutput( void )
{
    Query_Quantifier*& Q1 = mQuantifier[0];
    Query_Quantifier*& Q2 = mQuantifier[1];

    return (Q1->NodeRef()->AggregatesInOutput() || Q2->NodeRef()->AggregatesInOutput());
}

bool Query_SetNode::HasHaving( void )
{
    Query_Quantifier*& Q1 = mQuantifier[0];
    Query_Quantifier*& Q2 = mQuantifier[1];

    return (Q1->NodeRef()->HasHaving() || Q2->NodeRef()->HasHaving());
}

bool Query_SetNode::HasGroupBy( void )
{
    Query_Quantifier*& Q1 = mQuantifier[0];
    Query_Quantifier*& Q2 = mQuantifier[1];

    return (Q1->NodeRef()->HasGroupBy() || Q2->NodeRef()->HasGroupBy());
}

bool Query_SetNode::IsDeclareCursor( void )
{
    return mQuantifier[0]->NodeRef()->IsDeclareCursor();
}

void Query_SetNode::SetDeclareCursor( bool Decl )
{
    mQuantifier[0]->NodeRef()->SetDeclareCursor(Decl);
}

void Query_SetNode::SetA631( void )
{
    mQuantifier[0]->NodeRef()->SetA631();
}

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::PullDownForMerge( 
        Container_Vector<Query_OutputColumn*>*& pOutputColumns, SyntaxTreeNodeNo SelectList_AP,
                                                                SyntaxTreeNodeNo SelectInto_AP,
        Query_Expression& SearchCondition,                      SyntaxTreeNodeNo SearchCond_AP,
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
    SAPDBERR_ASSERT_ARGUMENT( !MustCreateCopy || (OrderBy_AP <= 0 && LimitStart <=0 && !ForReuse ) );

    if ( 
//           Having_AP == cak_qrewrite_ref_init 
//        && GroupBy_AP == cak_qrewrite_ref_init 
        /*&&*/ Update_AP == cak_qrewrite_ref_init   // DDT
        && !MustCreateCopy || (SelectInto_AP <= 0 && OrderBy_AP <= 0 && !ForReuse )
       )
    {
        // Quantifier[0] is the left side of the union which is connected in the aptree on low level
        // Quantifier[1] is the right side of the union which is connected in the aptree on same level
        // As "for reuse" and "order by" are connected which the left side we have to push down to the
        //  right side first (with MustCreateCopy=true) without pushing ForReuse and OrderBy
        //  and then push down everything to the left side 
        // If there really is ForReuse or OrderyBy then MustCreateCopy have to be false or this method would
        //  have been called incorrectly.

        Query_Expression EmptyCols;
        Container_Vector<Query_ColumnSpec*>* EmptyColSpec = 0;

        if ( mQuantifier[1]->NodeRef()->PullDownForMerge(
                                                pOutputColumns, SelectList_AP, 
                                                cak_qrewrite_ref_init,
                                                SearchCondition, SearchCond_AP, 
                                                HavingColumns, Having_AP,
                                                pGroupByColumns, GroupBy_AP,
                                                EmptyColSpec, cak_qrewrite_ref_init,
                                                UpdateColumns, Update_AP,
                                                Quantifier, FromPart_AP, 
												/*LimitStart*/cak_qrewrite_ref_init,
                                                /*LockOptionStart*/cak_qrewrite_ref_init,
                                                /*ForReuse*/ false, OuterJoinPredicates,
                                                QID,/* NID,*/ MergeType, AggrInSelectList, /*MustCreateCopy*/ true ) )
            if ( mQuantifier[0]->NodeRef()->PullDownForMerge(
                                                    pOutputColumns, SelectList_AP, 
                                                    SelectInto_AP,
                                                    SearchCondition, SearchCond_AP, 
                                                    HavingColumns, Having_AP,
                                                    pGroupByColumns, GroupBy_AP,
                                                    pOrderByColumns, OrderBy_AP,
                                                    UpdateColumns, Update_AP,
                                                    Quantifier, FromPart_AP, 
													LimitStart,
                                                    LockOptionStart,
                                                    ForReuse, OuterJoinPredicates,
                                                    QID, /*NID,*/ MergeType, AggrInSelectList, MustCreateCopy ) )
                return true;
    }
    return false;
}

void Query_SetNode::RemoveAllFromSet( void ) 
{ 
    mAll = false;

    switch ( mQueryGraph.SyntaxTree()[mInternAPNode].n_subproc() )
    {
    case cak_x_union_all:
        mQueryGraph.SyntaxTree()[mInternAPNode].n_subproc() = cak_x_union;
        break;
    case cak_x_except_all:
        mQueryGraph.SyntaxTree()[mInternAPNode].n_subproc() = cak_x_except;
        break;
    case cak_x_intersect_all:
        mQueryGraph.SyntaxTree()[mInternAPNode].n_subproc() = cak_x_intersect;
        break;
    }

    mQuantifier[0]->NodeRef()->RemoveAllFromSet();
    mQuantifier[1]->NodeRef()->RemoveAllFromSet();
};

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::PullDownSearchCond( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, SAPDB_Int2 QID,bool MustCreateCopy )
{
    // pull down further
    bool rc = true;
    rc &= mQuantifier[0]->PullDownSearchCond(SearchCondition, SearchCond_AP, QID, true);
    rc &= mQuantifier[1]->PullDownSearchCond(SearchCondition, SearchCond_AP, QID, MustCreateCopy);
    return rc;
}

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::PullDownProjectionPossible( void )
{
	if ( GetNodeType()==Query_Node::Union
		&& BodyDistinctType() != DT_Enforce 
		&& (mSubQueryType == SQT_None || mSubQueryType == SQT_EXISTS) )
	{
		bool rc = true;
		rc &= mQuantifier[0]->NodeRef()->PullDownProjectionPossible();
		rc &= mQuantifier[1]->NodeRef()->PullDownProjectionPossible();
		return rc;
	}
	return false;
}

bool Query_SetNode::PullDownProjection( void )
{
    // pull down further
	SAPDBERR_ASSERT_STATE( PullDownProjectionPossible() );
	bool rc = true;
	rc &= mQuantifier[0]->NodeRef()->PullDownProjection();
	rc &= mQuantifier[1]->NodeRef()->PullDownProjection();
	mProjectionPulledDown = true;
	return rc;
}

bool Query_SetNode::ProjectionPulledDown( void )
{
    return mProjectionPulledDown;
}

bool Query_SetNode::RenewSemanticsForColumnsDueToColumnPrivs( Query_Node* NewNode )
{
	if ( mQuantifier[0]->NodeRef() == NewNode )
	{
		return this->FatherNode()->RenewSemanticsForColumnsDueToColumnPrivs(this);
	}
	else
	{
		if ( mQuantifier[1]->NodeRef() == NewNode )
			return true;
		return false;
	}
}

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::AddGivenSequenceNode( void )
{
    // pull down further
    bool rc = true;
    rc &= mQuantifier[0]->NodeRef()->AddGivenSequenceNode();
    rc &= mQuantifier[1]->NodeRef()->AddGivenSequenceNode();
    return rc;
}

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::CreateColRefNamesAndResolveAsterisk( SyntaxTreeNodeNo& RefNameNodes )
{
    if ( !mQueryGraph.IsOk() )
        return false;

	Query_Quantifier* Q1 = mQuantifier[0];
	return Q1->NodeRef()->CreateColRefNamesAndResolveAsterisk(RefNameNodes);
}

bool Query_SetNode::RenewSemanticsForColumnDueToColumnPrivs( Query_Column* C )
{
	SAPDBERR_ASSERT_ARGUMENT( C );

	// DDT : falsch ???
	//Query_Quantifier* Q1 = mQuantifier[0];
	//return Q1->NodeRef()->RenewSemanticsForColumnDueToColumnPrivs(C);

    if ( C )
    {
        SAPDB_Int2 NewQSyntaxNode=0, NewQID=0, NewLogColumnIndex=0, NewExtColNo=0, NewSelColNo=0, NewNodeID=0;
        bool IsNullable=true;

		Query_Quantifier* Q1 = mQuantifier[0];
        if ( !Q1->CheckForColumn(C->GetColumnName(), NewQSyntaxNode, NewQID, NewLogColumnIndex, NewExtColNo, IsNullable) )
        {
            mQueryGraph.SetError("RenewSemanticsForColumnDueToColumnPrivs 1");
            SAPDBERR_ASSERT_STATE(0);
            return false;
        }
		Q1->NodeRef()->DecRefCountForOutputColumn(NewLogColumnIndex); // CheckForColumn incremented the RefCount so we have to revert that
		NewQID          = mID;
		NewQSyntaxNode	= mAPNode;
		NewNodeID		= C->GetNodeID();

		if ( mPosition == P_FromPart )
			++NewExtColNo;		// as there will be a syskey

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

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::SetTempFileNameAndCounter( SAPDB_Int1 Level, SAPDB_Int2& Cnt, bool FatherIsUnion, SAPDB_Int2& FromSelCnt, SAPDB_Int2& ParsCnt, bool& ContainsCorrelatedSubquery, bool& ContainsFromSelectInSubquery, SAPDB_Int2& MaxSubqueryLevel )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    // DDT : Do we need to increment ParsCnt ???

    // Add a92fromsel or other hint node
    if ( mFirstFlg )
    {
        SyntaxTreeNodeNo H = mQueryGraph.SyntaxTree()[mInternAPNode].n_refback;
        mQueryGraph.AddSubSelectNode(H, mPosition);
    }

    if ( Cnt < Query_MaxSubqueriesPerLevel && Level < Query_MaxSubqueriesPerLevel )
    {
        SyntaxTreeNodeNo H = mQueryGraph.SyntaxTree()[mInternAPNode].n_refback;
        if ( mFirstFlg && Level > 0 )
        {
            SAPDB_Int2 Name = 2 + Cnt + (Level<<8);
            mQueryGraph.SyntaxTree()[H].n_length() = Name;
        }

        if ( mPosition == P_FromPart )
        {
            H = mQueryGraph.SyntaxTree()[H].n_refback;
            if ( FromSelCnt < Query_MaxFromSelects )
                mQueryGraph.SyntaxTree()[H].n_pos() = ++FromSelCnt;
            else
                return false;
        }

	    //Query_Quantifier* Q1 = mQuantifier[0];
	    //Query_Quantifier* Q2 = mQuantifier[1];
        //SAPDB_Int2 n = (mFirstFlg ? 0 : Cnt);
//	    if ( Q1 && Q2 )
	    {
			bool rc = Query_Node::SetTempFileNameAndCounter(Level, Cnt, true, FromSelCnt,ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel);
		    //bool rc = Q1->NodeRef()->SetTempFileNameAndCounter(Level, n, true, FromSelCnt, ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel);
      //      rc     &= Q2->NodeRef()->SetTempFileNameAndCounter(Level, n, true, FromSelCnt, ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel);
//            Cnt = n;
            return rc;
	    }
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::Parse( bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();
    
//    DetermineAPNode();

    Query_Node* L = AddQuery(SyntaxTree[mInternAPNode].n_lo_level, PositionForParse(), ComplexViewReplacement);
    Query_Node* R = AddQuery(SyntaxTree[mInternAPNode].n_sa_level, PositionForParse(), ComplexViewReplacement);
    if ( R && L )
        return L->Parse(ComplexViewReplacement) && R->Parse(ComplexViewReplacement);
    return false;
}

bool Query_SetNode::Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement )
{
    if ( !mQueryGraph.IsOk() )
        return false;

    Query_Quantifier* Q1 = mQuantifier[0];
    if ( !Q1->NodeRef()->Semantics(UserId, UniqueOutputColumnNames, ComplexViewReplacement) )
    {
        mQueryGraph.SetError("Semantics for left child failed");
        return false;
    }
	SAPDB_Int2 OutCols1 = Q1->OutputColumns();

    Query_Quantifier* Q2 = mQuantifier[1];
    if ( !Q2->NodeRef()->Semantics(UserId, false, ComplexViewReplacement) )
    {
        mQueryGraph.SetError("Semantics for right child failed");
        return false;
    }
	SAPDB_Int2 OutCols2 = Q2->OutputColumns();

    Query_SyntaxTree& SyntaxTree = mQueryGraph.SyntaxTree();

	if ( OutCols1 != OutCols2 )     // DDT: different number of output columns within the two selects !!!
    {
        mQueryGraph.SetError("Semantics for SetNode failed");
        return false;
    }
    mSemanticsDone = true;
    return mSemanticsDone;
}

// ------------------------------------------------------------------------------------------------

SAPDB_Int2 Query_SetNode::OutputColumns( void )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    Query_Quantifier* Q1 = mQuantifier[0];
    return Q1->OutputColumns();
}

bool Query_SetNode::GetOutputColumnInternType( SAPDB_Int2 LogColIndex, Query_OutColType& Type, SyntaxTreeNodeNo& SyntaxNode )
{
    SyntaxTreeNodeNo N;
    Query_OutColType Type1, Type2;
    if ( mQuantifier[0]->NodeRef()->GetOutputColumnInternType(LogColIndex, Type1, N) )
    {
        if ( mQuantifier[0]->NodeRef()->GetOutputColumnInternType(LogColIndex, Type2, N) )
        {
            Type = Type1;
            Type.SubType |= Type2.SubType;
            switch ( Type1.MainType )
            {
            case Query_OutColType::OCTM_Column:
            case Query_OutColType::OCTM_Asterisk:
            case Query_OutColType::OCTM_MultiAsterisk:
                break;
            case Query_OutColType::OCTM_Expression:
                switch( Type2.MainType )
                {
                case Query_OutColType::OCTM_Subquery:
                    Type.SubType |= Query_OutColType::OCTS_ExpressionContainsSubquery;
                    break;
                case Query_OutColType::OCTM_Aggregate:
                    Type.SubType |= Query_OutColType::OCTS_ExpressionContainsAggr;
                    break;
                }
            case Query_OutColType::OCTM_Subquery:
                switch( Type2.MainType )
                {
                case Query_OutColType::OCTM_Expression:
                    Type.MainType = Query_OutColType::OCTM_Expression;
                    Type.SubType |= Query_OutColType::OCTS_ExpressionContainsSubquery;
                    break;
                case Query_OutColType::OCTM_Aggregate:
                    Type.MainType = Query_OutColType::OCTM_Aggregate;
                    Type.SubType |= Query_OutColType::OCTS_AggregationContainsSubquery;
                    break;
                }
                break;
            case Query_OutColType::OCTM_Aggregate:
                switch( Type2.MainType )
                {
                case Query_OutColType::OCTM_Expression:
                    Type.SubType |= Query_OutColType::OCTS_AggregationContainsExpression;
                    break;
                case Query_OutColType::OCTM_Subquery:
                    Type.SubType |= Query_OutColType::OCTS_AggregationContainsSubquery;
                    break;
                }
                break;
            }
            return true;
        }
    }
    return false;
}

SAPDB_Int2 Query_SetNode::GetOutputColumnSize( SAPDB_Int2 LogColIndex )
{
	SAPDBERR_ASSERT_STATE( mSemanticsDone );

	SAPDB_Int2 L1 = mQuantifier[0]->NodeRef()->GetOutputColumnSize(LogColIndex);
	SAPDB_Int2 L2 = mQuantifier[1]->NodeRef()->GetOutputColumnSize(LogColIndex);
	if ( L1>=0 && L2>=0 )
		return (L1>L2?L1:L2);
	return cak_qrewrite_ref_init;
}

bool Query_SetNode::GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    Query_Quantifier* Q1 = mQuantifier[0];
    return Q1->GetOutputColumnName(LogColIndex, ColName, IsNullable);
}

bool Query_SetNode::GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ExtColNo )
{
    Query_Quantifier* Q1 = mQuantifier[0];
	int syskey = mFatherQuantifier->FromTableSpec() ? 1:0;
    ExtColNo =  LogColIndex + syskey;
    return ( LogColIndex>0 && LogColIndex<=Q1->OutputColumns()+syskey );
}

bool Query_SetNode::CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    QuantifierID = mID;
    QuantifierSyntaxNode = mAPNode;
    Query_Quantifier* Q1 = mQuantifier[0];
    SAPDB_Int2 Node, ID;
    bool rc = Q1->CheckForColumn(Column, Node, ID, LogColumnIndex, ExtColNo, IsNullable);
	if ( rc )
	{
		// we have to inc the refcount for the second quantifier also. for the first one this was done by Q1->CheckForColumn implicietly
		Query_Quantifier* Q2 = mQuantifier[1];
		rc = Q2->NodeRef()->IncRefCountForOutputColumn(LogColumnIndex);
	}
	return rc;
}

bool Query_SetNode::CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable )
{
    if ( CheckQuantifierName(Schema, Table) )
		return CheckForColumn(Column, QuantifierSyntaxNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
    return false;
}

bool Query_SetNode::CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table )
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

void Query_SetNode::GetIdentificationNames( SQLMan_Identifier& Schema, SQLMan_Identifier& Table )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    Schema.SetBlank();
    Table  = mReferenceName;
}

bool Query_SetNode::HasReferenceName( void )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
	return ( !mReferenceName.IsBlank() );
}

bool Query_SetNode::DecRefCountForOutputColumn( SAPDB_Int2 i )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    bool rc = true;
    rc &= mQuantifier[0]->NodeRef()->DecRefCountForOutputColumn(i);
    rc &= mQuantifier[1]->NodeRef()->DecRefCountForOutputColumn(i);
    return rc;
}

bool Query_SetNode::IncRefCountForOutputColumn( SAPDB_Int2 i )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    bool rc = true;
    rc &= mQuantifier[0]->NodeRef()->IncRefCountForOutputColumn(i);
    rc &= mQuantifier[1]->NodeRef()->IncRefCountForOutputColumn(i);
    return rc;
}

bool Query_SetNode::AddRefCountForOutputColumn( SAPDB_Int2 i, SAPDB_Int2 Count )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );
    bool rc = true;
    rc &= mQuantifier[0]->NodeRef()->AddRefCountForOutputColumn(i, Count);
    rc &= mQuantifier[1]->NodeRef()->AddRefCountForOutputColumn(i, Count);
    return rc;
}

// ------------------------------------------------------------------------------------------------

bool Query_SetNode::CheckForIntOrExcept2Exist( void )
{
    SAPDBERR_ASSERT_STATE( mSemanticsDone );

    if ( All() || OuterJoin() || Aggregates() )
        return false;

    if (   mQuantifier[0]->NodeTypeSelect() 
        && mQuantifier[1]->NodeTypeSelect() )
    {
        Query_SelectNode* LeftSelNode   = (Query_SelectNode*)(mQuantifier[0]->NodeRef());
        Query_SelectNode* RightSelNode  = (Query_SelectNode*)(mQuantifier[1]->NodeRef());

        if (   RightSelNode->WQuantifierCnt() != 0
            || RightSelNode->LQuantifierCnt() != 0
            || RightSelNode->HasFromSelects() )
            return false;

        SAPDB_Int2 n1 = LeftSelNode->OutputColumns();
#if defined ( SAPDB_SLOW )
        SAPDB_Int2 n2 = RightSelNode->OutputColumns();
        SAPDBERR_ASSERT_STATE( n1==n2 );
#endif
        for( SAPDB_Int2 i=1; i<=n1; i++ )
        {
            if ( LeftSelNode->OutputColumnIsNullable(i) && RightSelNode->OutputColumnIsNullable(i) )
                return false;
        }
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------
