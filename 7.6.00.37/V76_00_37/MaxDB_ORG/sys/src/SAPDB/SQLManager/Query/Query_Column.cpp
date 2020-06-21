/*!***************************************************************************

    @file     Query_Column.cpp
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

#include "SAPDB/SQLManager/Query/Query_Column.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"

// ------------------------------------------------------------------------------------------------

Query_ExpressionNode* Query_Column::CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Query_Column* N = new(Alloc) Query_Column(*this); // Query_Column(*this, SyntaxTree, mpMap);
    if ( N )
    {
        if ( mpMap )
        {
            SyntaxTree.MapNode(N->mSyntaxNode, mpMap);
            SyntaxTree.MapNode(N->mQuantifierNode, mpMap);
            SyntaxTree[N->mSyntaxNode].n_fromtabnode() = N->mQuantifierNode;
        }
        if ( mpQMap )
        {
            SyntaxTree.MapQuantifierID(N->mNodeID, mpQMap);
            SyntaxTree.MapQuantifierID(N->mQuantifierID, mpQMap);
        }
        if ( mNext )
            N->mNext = mNext->CreateDuplicate(Alloc, SyntaxTree, mpMap, mpQMap);
    }
    return N;
}

// ------------------------------------------------------------------------------------------------

bool Query_Column::ExchangeQuantifierID( SAPDB_Int2 OldID, SAPDB_Int2 NewID )
{
    if ( mQuantifierID == OldID )
    {
        mQuantifierID = NewID;
    }
    if ( mNext )
        return mNext->ExchangeQuantifierID(OldID, NewID);
    return true;  
}

bool Query_Column::ExchangeNodeID( SAPDB_Int2 OldID, SAPDB_Int2 NewID, SAPDB_Int2& Cnt )
{
    if ( mNodeID == OldID )
    {
        mNodeID = NewID;
        ++Cnt;
    }
    if ( mNext )
        return mNext->ExchangeNodeID(OldID, NewID, Cnt);
    return true;  
}

bool Query_Column::MappFromOuterToInnerSelect( Query_SelectNode* InnerSelectNode, Query_MappingInfo* MappingInfo )
{
    SAPDBERR_ASSERT_ARGUMENT( InnerSelectNode );

    if ( InnerSelectNode )
    {
        bool rc = InnerSelectNode->MappColumnFromOuterSelect(this, MappingInfo);
        if ( rc && mNext )
            rc &= mNext->MappFromOuterToInnerSelect(InnerSelectNode, MappingInfo);
        return rc;
    }
    return false;
}

bool Query_Column::Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy )
{
    SAPDB_Int2  FromTabNode=cak_qrewrite_ref_init, 
                QuantifierID=cak_qrewrite_ref_init, 
                LogColumnIndex=cak_qrewrite_ref_init, 
                ExtColNo=cak_qrewrite_ref_init;
    bool        IsNullable = true;
	SAPDB_Int2  found = 0;

    SAPDBERR_ASSERT_STATE( !mIsAsterisk );

    if ( ForOrderBy && mSchemaName.IsBlank() && mTableName.IsBlank() )
	{
        SAPDB_Int2 OutCol;
		found = Father->Semantics_FindOrderColumnInSelectList(mColName, FromTabNode, QuantifierID, LogColumnIndex, ExtColNo, OutCol, IsNullable, Father->FatherNode()->GetNodeType() == Query_Node::Top);
	}
    if ( !found && !(ForOrderBy && Father->FatherNode()->GetNodeType()!=Query_Node::Top) )      // DDT : ???
	{
		found += Father->Semantics_FindColumnInFromPart(mSchemaName, mTableName, mColName, FromTabNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
		if ( !found && MayBeCorrelated )
		{
			Father->SetUnidentifiedColumns();
			SetStatus(Query_Column::cMayBeCorrelated);
			return true;
		}
	}
	if ( found!=1 )
		return false;

	if ( FromTabNode>0 )
	{
        Query_SyntaxTree& SyntaxTree = Father->QueryGraph().SyntaxTree();
		if ( !SyntaxTree.ChangeColumnNameToColumnId(mSyntaxNode, FromTabNode, ExtColNo, mErrPos) )
			return false;
	}
    SetInfo(-1, QuantifierID, (FromTabNode>=0?FromTabNode:cak_qrewrite_ref_init), mColName, LogColumnIndex, ExtColNo, IsNullable);
    return true;
}

bool Query_Column::SemanticsCorrelation( Query_SelectNode* SubQuery, bool& Unidentified )
{
    Query_Node* FNode = SubQuery;
    while ( mStatus == cMayBeCorrelated && FNode->Position()!=P_FromPart)
    {
        FNode = FNode->FatherNode();
        if ( FNode->GetNodeType() != Query_Node::Select )
			break;
		Query_SelectNode* FSelNode = (Query_SelectNode*)FNode; 

		SAPDB_Int2  FromTabNode=cak_qrewrite_ref_init, 
					QuantifierID=cak_qrewrite_ref_init, 
					LogColumnIndex=cak_qrewrite_ref_init, 
					ExtColNo=cak_qrewrite_ref_init;
		bool        IsNullable = true;

		SAPDB_Int2 found = FSelNode->Semantics_FindColumnInFromPart(mSchemaName, mTableName, mColName, FromTabNode, QuantifierID, LogColumnIndex, ExtColNo, IsNullable);
		if ( found>1 )
		{
			Unidentified = true;
			return false;
		}
		if ( found==1 )
		{
			if ( FromTabNode>0 )
			{
				Query_SyntaxTree& SyntaxTree = SubQuery->QueryGraph().SyntaxTree();
				if ( !SyntaxTree.ChangeColumnNameToColumnId(mSyntaxNode, FromTabNode, ExtColNo, mErrPos) )
					return false;
			}
			SetInfo(FSelNode->NodeID(), QuantifierID, (FromTabNode>=0?FromTabNode:cak_qrewrite_ref_init), mColName, LogColumnIndex, ExtColNo, IsNullable);

			if ( InAggr() )
				FSelNode->ChangeCorrelatedAndAggregatedColumnsRefersToMe(+1);
  			FSelNode->ChangeCorrelatedColumnsRefersToMe(+1);  // DDT : else ?

			SubQuery->ChangeCorrelatedColumns(+1);
		}
    }
    Unidentified |= (mStatus == cMayBeCorrelated); 
    return true;
}

bool Query_Column::CheckCorrelatedColumnsForExistToIn( Query_SelectNode* Father )
{
	if ( mNodeID > 0 ) // Correlated Column
	{
		if ( mNodeID != Father->NodeID() ) 
			return false;					// but another father select

		Query_SyntaxTree& SyntaxTree = Father->QueryGraph().SyntaxTree();
		SyntaxTreeNodeNo node = SyntaxTree[mSyntaxNode].n_refback;
		if (   SyntaxTree[node].n_proc      == no_proc 
			&& SyntaxTree[node].n_subproc() == s_equal )
		{
			return SyntaxTree.IsBooleanFactorInCondition(node);
		}
		return false;
	}
	return true;
}

bool Query_Column::RenewSemanticsDueToColumnPrivs( Query_Node* NewNode )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );
    if ( NewNode )
    {
        bool rc = true;
        if ( mQuantifierID == NewNode->NodeID() )
            rc &= NewNode->RenewSemanticsForColumnDueToColumnPrivs(this);
        if ( rc && mNext )
            rc &= mNext->RenewSemanticsDueToColumnPrivs(NewNode);
        return rc;
    }
    return true;
}

bool Query_Column::RenewSemanticsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Query_Position Pos )
{
    SAPDBERR_ASSERT_ARGUMENT( NewNode );
    if ( NewNode )
    {
        bool rc = true;
        if ( mQuantifierID == OldQID )
            rc &= NewNode->RenewSemanticsForColumnOnMergeForCorrCols(this, Pos);
        if ( rc && mNext )
            rc &= mNext->RenewSemanticsOnMergeForCorrCols(OldQID, NewNode, Pos);
        return rc;
    }
    return true;
}

void Query_Column::IncRefCountsToChildColumns( Query_Node* Node )
{
    SAPDBERR_ASSERT_ARGUMENT( Node );
    if ( Node )
    {
        Node->IncRefCountForOutputColumn(mQuantifierID, mLogicalColumnIndex);
        if ( mNext )
            mNext->IncRefCountsToChildColumns(Node);
    }
}

void Query_Column::DelRefCountsToChildColumns( Query_Node* Node )
{
    SAPDBERR_ASSERT_ARGUMENT( Node );
    if ( Node )
    {
        Node->DecRefCountForOutputColumn(mQuantifierID, mLogicalColumnIndex);
        if ( mNext )
            mNext->DelRefCountsToChildColumns(Node);
    }
}

// ------------------------------------------------------------------------------------------------
