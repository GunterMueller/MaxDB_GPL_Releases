/*!***************************************************************************

    @file     QueryRewrite_Rules.cpp
    @ingroup  QueryRewrite
    @author   DirkT
    @brief    
    @see    

  -------------------------------------------------------------------------

  responsible : DirkT

  special area: SQL Manager / QueryRewrite
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

#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rules.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"
#include "SAPDB/SQLManager/Query/Query_Node.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSet.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"

// ------------------------------------------------------------------------------------------------

/// Write output (rulename + node the rule was applied to) to vtrace
inline void RuleTrace( SQLMan_Identifier RuleName, Query_Node* NodePtr )
{
    if ( QueryRewrite_Trace.TracesLevel(2) )
    {
        Kernel_VTrace()
            << "    > Apply rule "
            << RuleName
            << " for node "
            << NodePtr->NodeID() << FlushLine;
    }
}

/// Write output (rulename + nodes the rule was applied to) to vtrace
inline void RuleTrace( SQLMan_Identifier RuleName, Query_Node* NodePtr, Query_Quantifier* Quantifier )
{
    if ( QueryRewrite_Trace.TracesLevel(2) )
    {
        Kernel_VTrace()
            << "    > Apply rule "
            << RuleName
            << " for node "
            << NodePtr->NodeID()
            << " childnode "
            << Quantifier->NodeRef()->NodeID() << FlushLine;
    }
}

// ------------------------------------------------------------------------------------------------

// NOTE: If the output parameter Optimized in method Apply will be set to TRUE this will cause "Statement has been optimized" in SUTs
//       Therefore this should only be set if the APTree was modified

// ------------------------------------------------------------------------------------------------
// ok

bool Rule_SimplifyPredicates::IsApplicable( Query_Node* Node ) 
{
	if ( Node->GetNodeType() == Query_Node::Select )
		if (   !Node->OuterJoin()				// DDT : that's not necessary but if we allow outerjoins predicates like "col is NULL" where col is not nullable have to be treated in aa special way!
//			&& !Node->IsCorrelatedSubquery()	// DDT : just temporaryly until problems with mapping of corrcolumns while merging are solved
		    )
		    return !Node->PredicatesSimplified();
	return false;
}

bool Rule_SimplifyPredicates::Apply( Query_Node*& Node, bool& Optimized ) 
{
    if ( Node->SimplifyPredicates() )
    {
        Query_QueryGraph& Graph = Node->QueryGraph();
        RuleTrace(GetAsciiRuleName(), Node);
//		Optimized = true;
        Graph.SetRuleApplied(RuleID(), qrwSimplifyPredicates);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Rule_EliminateOrderBy::IsApplicable( Query_Node* Node ) 
{
    // group by a,b,c order by a,b  > order by is obsolete due to our implementation !

    if ( Node->GetNodeType() == Query_Node::Select )
    {
		Query_SelectNode* S = (Query_SelectNode*) Node;

		Query_SubQueryType SQT = S->GetSubQueryType();

		if (   !S->IsCorrelatedSubquery() 
            && !S->OrderByEliminated() 
			&&  S->HasOrderBy()
			&&  S->HasGroupBy()
            && !S->HasHaving() 
			&&  S->GroupByContainsAllOrderByColumnsFromLeft( SQT!=SQT_EXISTS && SQT!=SQT_None )
			&&  (!S->Distinct() 
			     || S->OneTupleAggregation()
			     || S->OutputContainsAllColumnsFromGroupBy() )// we need this to avoid an extra phase for DISTINCT 
			)
			return true;

    }
    return false;
}

bool Rule_EliminateOrderBy::Apply( Query_Node*& Node, bool& Optimized ) 
{
    if ( Node->EliminateOrderBy() )
    {
        Query_QueryGraph& Graph = Node->QueryGraph();
		RuleTrace(GetAsciiRuleName(), Node);
		if ( ((Query_SelectNode*)Node)->Distinct() )
		{
			SAPDBERR_ASSERT_STATE( ((Query_SelectNode*)Node)->OneTupleAggregation() 
				                || ((Query_SelectNode*)Node)->OutputContainsAllColumnsFromGroupBy() );
			((Query_SelectNode*)Node)->RemoveDistinctNode();
			Node->BodyDistinctType() = DT_Preserve;
			Graph.SetRuleApplied(RuleID(), qrwEliminateGroupByOrDistinct);
		}
//		Optimized = true;
		Graph.SetRuleApplied(RuleID(), qrwEliminateOrderBy);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Rule_EliminateGroupByOrDistinct::IsApplicable( Query_Node* Node ) 
{
    if ( Node->GetNodeType() == Query_Node::Select )
    {
        if ( Node->HasGroupBy() && ((Query_SelectNode*)Node)->Distinct() )
            return ((Query_SelectNode*)Node)->OutputContainsAllColumnsFromGroupBy();	// distinctness is ensured by the groupby
    }
    return false;
}

bool Rule_EliminateGroupByOrDistinct::Apply( Query_Node*& Node, bool& Optimized ) 
{
    RuleTrace(GetAsciiRuleName(), Node);

    Query_QueryGraph& Graph = Node->QueryGraph();
    if ( Node->Aggregates() || Node->OrderByEliminated() )
	{
        ((Query_SelectNode*)Node)->RemoveDistinctNode();
		Node->BodyDistinctType() = DT_Preserve;
	}
    else
	{
        ((Query_SelectNode*)Node)->RemoveGroupBy();
		Node->BodyDistinctType() = DT_Enforce;
	}
//	Optimized = true;
    Graph.SetRuleApplied(RuleID(), qrwEliminateGroupByOrDistinct);

    Node->HeadDistinct() = true;
    Node->FatherQuantifier()->DistinctType() = DT_Preserve; 
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Rule_MergeF::IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) 
{
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];

    if (    Node->GetNodeType() == Query_Node::Select
         && Quantifier->QuantifierType() == QT_F 
         && Quantifier->NodeTypeSelectOrSet()
         && !Quantifier->NodeRef()->LQuantifierCnt()     // DDT: No selects within the select list for now
               // OutputColumnsWithSelectsReferencedNoMoreThanOnce() but be sufficient to avoid doubling of Quantifier
       )
    {
        bool UpperAggregates    =  Node->Aggregates() 
                                || Node->HasGroupBy();

        bool LowerAggregates    =  Quantifier->NodeRef()->Aggregates() 
                                || Quantifier->NodeRef()->HasGroupBy();

        if ( Quantifier->NodeType() == Query_Node::Select 
             && ( ( Node->HeadDistinct() == true ) ||
                  ( Node->BodyDistinctType() == DT_Permit ) ||
//                  ( Node->OutputContainsAllColumnsFromQuantifier(Quantifier) && Node->GetJoinType()==JT_NoJoin ) || // DDT
                  ( Quantifier->NodeRef()->BodyDistinctType() != DT_Enforce ) 
                )
           )
        {   // Merge : SELECT + SELECT 

            if ( (Node->FQuantifierCnt() + Quantifier->NodeRef()->FQuantifierCnt() <= Query_MaxFQuantifier)
                && (!Node->OuterJoin() || Quantifier->NodeRef()->FQuantifierCnt()==1 )
				&& (!Node->OuterJoin() || !Quantifier->NodeRef()->ContainsCorrelatedSubquery() )             
				&& (!Quantifier->OuterJoin() || Node->FQuantifierCnt()==1)
               )
            {
                if ( UpperAggregates && !LowerAggregates )
                {
                    if ( Quantifier->NodeRef()->BodyDistinctType() != DT_Enforce )
                        return true; 
                    return false;
                }
                else
                {
                    if ( !UpperAggregates && LowerAggregates )
                    {
                        if ( Quantifier->NodeRef()->BodyDistinctType() != DT_Enforce )
                            return Node->GetJoinType()==JT_NoJoin;
                        return false;
                    }
                    else
                    {
                        if ( UpperAggregates && LowerAggregates )
                        {
                            if ( Quantifier->NodeRef()->BodyDistinctType() != DT_Enforce )
                                return Node->GetJoinType()==JT_NoJoin // DDT: unnecessary, but for performance reasons
									&& Node->CompatibleAggregation(Quantifier->NodeRef());
                            return false;
                        }
                    }
                }
                return true;
            }
        }

        if ( Quantifier->NodeTypeSet() )
        {   // Merge : SELECT + UNION/../.. 

            Query_SetNode* SetNode = (Query_SetNode*)Quantifier->NodeRef();

            if (   !Node->OuterJoin() 
                && !SetNode->OuterJoin()     
                && !UpperAggregates             
                && (Node->GetJoinType()==JT_NoJoin || !LowerAggregates)    // DDT
                && !Node->HasOrderBy()          // DDT: No OrderBy - SelectListContainsAllOrderByColumns müßte richtiges Kriterium sein !!!
                && !Node->HasRownoPredicate()
                )
            {
				bool AllCols = Node->OutputContainsAllColumnsFromQuantifier(Quantifier);

				if (	AllCols
					 || Node->BodyDistinctType()==DT_Enforce 
					 || (SetNode->GetNodeType()==Query_Node::Union && !SetNode->All() && AllCols) 
					)
				{
					Query_Node::NodeType FBT  = Node->FatherNode()->GetNodeType();
					bool FatherIsSet =  Node->FatherNode()->IsSet(); /*( FBT == Query_Node::Union )     ||
						     			( FBT == Query_Node::Intersect ) ||
										( FBT == Query_Node::Difference );*/

					if ( FatherIsSet )
						if ( ((Query_SetNode*)(Node->FatherNode()))->All() != SetNode->All() )
						{
							if (   Node->FatherNode()->GetNodeType() == /*Query_Node::Union 
								&& */SetNode->GetNodeType() /*== Query_Node::Union 
								&& !((Query_SetNode*)(Node->FatherNode()))->All()*/ )
								return true;
							else
								return false;	// AK can't handle this and therefore generates that fromsel ...
						}

					SAPDB_Int2 fq = 0;
					SAPDB_Int2 n = Node->GetQuantifier().GetSize();
					for ( SAPDB_Int2 i=0; i<n; i++ )
					{
						if ( Node->GetQuantifier()[i]->NodeTypeSelectOrSet() )
							if ( ++fq>1 )  
								return false;               // DDT : there is more than one fromselect
					}
					return true;
				}
            }
        }
    }
    return false;
}

bool Rule_MergeF::Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) 
{
    Query_QueryGraph& Graph = Node->QueryGraph();
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
    RuleTrace(GetAsciiRuleName(), Node, Quantifier);

    if ( Merge(Node, QuantifierNo) )
    {
        Optimized = true;
        Graph.SetRuleApplied(RuleID(), qrwMergeF);
        return true;
    }
    SAPDBERR_ASSERT_STATE( !Graph.IsOk() );
    return false;
}

bool Rule_MergeF::Merge( Query_Node*& Node, SAPDB_UInt2 QuantifierNo ) 
{
    Query_QueryGraph& Graph = Node->QueryGraph();
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
    Query_SelectNode* UpperNode = (Query_SelectNode*)Node;
    Query_Node* LowerNode = Quantifier->NodeRef();

    if ( UpperNode->ReplaceAsteriskReferToLowerNodeForMerge(LowerNode) )
    {
        bool rc = false;
        if ( UpperNode->PushDownForMerge(LowerNode) )
        {
            if ( DeleteUpperNode(UpperNode, QuantifierNo, LowerNode) )
            {
                Node = LowerNode;
                return true;
            }
        }
    }
    Node = 0;
    Graph.SetError("Merge failed");
    return false;
}

bool Rule_MergeF::SetJoinType( Query_SelectNode* UpperNode, Query_Node* LowerNode ) 
{
    LowerNode->AddJoinType(UpperNode->GetJoinType());
    return true;
}

bool Rule_MergeF::SetDistinctType( Query_SelectNode* UpperNode, Query_Node* LowerNode ) 
{
    if ( LowerNode->GetNodeType() == Query_Node::Select )
    {
        bool SetTargetEnforce = ( UpperNode->BodyDistinctType() != DT_Permit  ) &&
                                ( LowerNode->BodyDistinctType() == DT_Enforce );
        
        if ( SetTargetEnforce )    
        {
            if ( !((Query_SelectNode*)LowerNode)->AddDistinctNode() )
                return false;
            LowerNode->BodyDistinctType() = DT_Enforce;
        }
        else
        {
            // Copy distinct type from upper node ...
            if ( UpperNode->BodyDistinctType() == DT_Enforce )
            {
                if ( !((Query_SelectNode*)LowerNode)->AddDistinctNode() )
                    return false;
            }
            else
            {
                if ( !((Query_SelectNode*)LowerNode)->RemoveDistinctNode() )
                    return false;
            }

            LowerNode->BodyDistinctType() = UpperNode->BodyDistinctType();
            ((Query_SelectNode*)LowerNode)->Distinct() = ((Query_SelectNode*)UpperNode)->Distinct();
        }
    }
    else
    {
        if ( UpperNode->BodyDistinctType() == DT_Enforce )
        {
			if ( LowerNode->GetNodeType()==Query_Node::Union )
				LowerNode->RemoveAllFromSet();
			else
	            return false;
        }
    }
    return true;
}

bool  Rule_MergeF::DeleteUpperNode( Query_SelectNode* UpperNode, SAPDB_UInt2 QuantifierNo, Query_Node* LowerNode ) 
{
    // 1. adjust syntaxtree
    Query_QueryGraph& Graph      = UpperNode->QueryGraph();
    Query_SyntaxTree& SyntaxTree = Graph.SyntaxTree();

    SyntaxTreeNodeNo Upper_A63      = UpperNode->GetInternAPNode();
    SyntaxTreeNodeNo NewInternAPNode= Upper_A63;
    bool First =  ((SyntaxTree[Upper_A63].n_proc == a63 && SyntaxTree[Upper_A63].n_subproc() == cak_x_mass_select) 
                || (SyntaxTree[Upper_A63].n_proc == a43 && SyntaxTree[Upper_A63].n_subproc() == cak_x_explain));

    SyntaxTreeNodeNo Lower_A63      = LowerNode->GetInternAPNode();

    bool IsDeclCursor   = ( SyntaxTree[Upper_A63].n_proc == a63 
                         && SyntaxTree[Upper_A63].n_subproc() == cak_x_decl_cursor );
    bool LowerIsSelect  = ( SyntaxTree[Lower_A63].n_proc == a63query_spec ); 

    SAPDBERR_ASSERT_STATE ( IsDeclCursor == UpperNode->IsDeclareCursor() );
    SAPDBERR_ASSERT_STATE ( LowerIsSelect == (LowerNode->GetNodeType() == Query_Node::Select) );

    bool FatherIsSet    = UpperNode->FatherNode()->IsSet();

    if ( !FatherIsSet && !LowerIsSelect )	// we must copy the a63/cak_x_start_union node !
        Lower_A63		= SyntaxTree[Lower_A63].n_refback;

    bool CopySelListStart = false;

    if ( IsDeclCursor )
    {
		if ( LowerIsSelect )
		{
			SyntaxTreeNodeNo Target = SyntaxTree[Upper_A63].n_lo_level;
			if ( (SyntaxTree[Target].n_proc == a63) &&
				(SyntaxTree[Target].n_subproc() == cak_x_distinct) )
				Target = SyntaxTree[Target].n_lo_level;
			Target = SyntaxTree[Target].n_sa_level;

			SyntaxTreeNodeNo ToBeRemoved = SyntaxTree[Target].n_sa_level;

			SyntaxTreeNodeNo Source = Lower_A63;
			Source = SyntaxTree[Source].n_lo_level;
			if ( (SyntaxTree[Source].n_proc == a63) &&
				(SyntaxTree[Source].n_subproc() == cak_x_distinct) )
				Target = SyntaxTree[Source].n_lo_level;
			CopySelListStart = true;

			if (   !SyntaxTree[Source].n_special.includes(ni_info_node) 
				&&  SyntaxTree[Target].n_special.includes(ni_info_node) )
			{
				SyntaxTree[Target].n_special.delElement(ni_info_node);
				SyntaxTree.SetUnusedNode(Target+1, /*Recursive*/ false );
			}
			if (    SyntaxTree[Source].n_special.includes(ni_info_node) 
				&& !SyntaxTree[Target].n_special.includes(ni_info_node) )
			{
				SAPDBERR_ASSERT_STATE( 0 );
				return false;
			}

			tak_special_node_info OldSpecial = SyntaxTree[Source].n_special;
			SyntaxTree[Source].n_special  = SyntaxTree[Target].n_special;     // DDT ???
			SyntaxTree[Source].n_refback  = SyntaxTree[Target].n_refback;
			SyntaxTreeNodeNo Lower_Same   = SyntaxTree[Source].n_sa_level;
			if ( Lower_Same )
			{
				if ( SyntaxTree[Lower_Same].n_proc == no_proc && SyntaxTree[Lower_Same].n_symb == s_identifier )
				{
					SyntaxTree.UnlinkNodeWithSameLevel(Lower_Same, /*SetUnused=*/true);
					Lower_Same = 0;
				}
			}
			SyntaxTreeNodeNo Lower_Low    = SyntaxTree[Source].n_lo_level;
			SyntaxTree[Target]            = SyntaxTree[Source];
			SyntaxTree[Source].n_sa_level = 0;
			SyntaxTree[Source].n_lo_level = 0;
			SyntaxTree[Source].n_special  = OldSpecial;

			SyntaxTree.SetUnusedNode(ToBeRemoved, /*Recursive*/ true );
			if ( Lower_Low > 0 )
				SyntaxTree[Lower_Low ].n_refback = Target;
			if ( Lower_Same > 0 )
				SyntaxTree[Lower_Same].n_refback = Target;
		}
		else	// !LowerIsSelect
		{	
			SyntaxTreeNodeNo Target = Upper_A63;
			SyntaxTreeNodeNo CName = SyntaxTree[Upper_A63].n_lo_level;
			if ( (SyntaxTree[CName].n_proc == a63) &&
				(SyntaxTree[CName].n_subproc() == cak_x_distinct) )
				CName = SyntaxTree[CName].n_lo_level;


			SyntaxTree.UnlinkNode(CName, /*SetUnused*/ false);

			SyntaxTreeNodeNo ToBeRemoved = SyntaxTree[Target].n_lo_level;

			SyntaxTreeNodeNo Source = Lower_A63;

			SyntaxTree.UnlinkNodeWithSameLevel(Source, /*SetUnused*/false);
			SyntaxTreeNodeNo Lower_Same   = SyntaxTree[Source].n_sa_level;
			SyntaxTreeNodeNo Lower_Low    = SyntaxTree[Source].n_lo_level;
			SyntaxTree[Source].n_refback  = SyntaxTree[Target].n_refback;
			SyntaxTree[Target]            = SyntaxTree[Source];

			if ( Lower_Low > 0 )
				SyntaxTree[Lower_Low ].n_refback = Target;
			if ( Lower_Same > 0 )
				SyntaxTree[Lower_Same].n_refback = Target;

			SyntaxTree.SetUnusedNode(ToBeRemoved, /*Recursive*/ true );

			SyntaxTreeNodeNo FirstSel = Target;
			while ( FirstSel > 0 )
			{
				if ( SyntaxTree[FirstSel].n_proc != a63query_spec )
					FirstSel = SyntaxTree[FirstSel].n_lo_level;
				else
					break;
			}
			if ( SyntaxTree[FirstSel].n_special.includes(ni_info_node) )
			{
				SyntaxTree[FirstSel].n_special.delElement(ni_info_node);
				SyntaxTree.SetUnusedNode(FirstSel+1, /*Recursive*/ false );
			}

			SyntaxTree[FirstSel].n_proc.becomes(a63);
			SyntaxTree[FirstSel].n_subproc() = cak_x_decl_cursor;

			SyntaxTreeNodeNo H2 = SyntaxTree[FirstSel].n_lo_level;
			if ( SyntaxTree[H2].n_proc == a63 && SyntaxTree[H2].n_subproc() == cak_x_distinct )
			{
				FirstSel = H2;
				H2 = SyntaxTree[FirstSel].n_lo_level;
			}

			SyntaxTree[FirstSel].n_lo_level = CName;
			SyntaxTree[CName   ].n_refback  = FirstSel;
			SyntaxTree[CName   ].n_sa_level = H2;
			SyntaxTree[H2      ].n_refback  = CName;
		}
	}
    else    // !IsDeclCursor
    {
        SyntaxTreeNodeNo Target = Upper_A63;
        SyntaxTreeNodeNo Source = Lower_A63;
        SyntaxTreeNodeNo ToBeRemoved = SyntaxTree[Target].n_lo_level;
        SyntaxTreeNodeNo Upper_Same  = SyntaxTree[Target].n_sa_level;

        if (   !SyntaxTree[Source].n_special.includes(ni_info_node) 
            &&  SyntaxTree[Target].n_special.includes(ni_info_node) )
        {
            SyntaxTree[Target].n_special.delElement(ni_info_node);
            SyntaxTree.SetUnusedNode(Target+1, /*Recursive*/ false );
        }
        if (    SyntaxTree[Source].n_special.includes(ni_info_node) 
            && !SyntaxTree[Target].n_special.includes(ni_info_node) 
            && !(First && LowerIsSelect) )
        {
            SAPDBERR_ASSERT_STATE( 0 );
			return false;
        }

        tak_special_node_info OldSpecial = SyntaxTree[Source].n_special;
        SyntaxTree[Source].n_special  = SyntaxTree[Target].n_special;
        SyntaxTree[Source].n_refback  = SyntaxTree[Target].n_refback;
        SyntaxTreeNodeNo Lower_Same   = SyntaxTree[Source].n_sa_level;
        if ( Lower_Same && LowerIsSelect )
        {
            if ( SyntaxTree[Lower_Same].n_proc == no_proc && SyntaxTree[Lower_Same].n_symb == s_identifier )
            {
                SyntaxTree.UnlinkNodeWithSameLevel(Lower_Same, /*SetUnused=*/true);
                Lower_Same = 0;
            }
        }
        SyntaxTreeNodeNo Lower_Low    = SyntaxTree[Source].n_lo_level;
        bool SubQInfo                 = SyntaxTree[Target].n_querycheck.includes(qcn_SubQInfo);
        SAPDB_Int2 Len                = SyntaxTree[Target].n_length();
        SyntaxTree[Target]            = SyntaxTree[Source];
        if ( SubQInfo )
        {
            SyntaxTree[Target].n_querycheck.addElement(qcn_SubQInfo);
            SyntaxTree[Target].n_length() = Len;
        }
        SyntaxTree[Source].n_sa_level = 0;
        SyntaxTree[Source].n_lo_level = 0;
        SyntaxTree[Source].n_special  = OldSpecial;

        if ( LowerIsSelect )
            SyntaxTree[Target].n_sa_level = Upper_Same;
        else
            SyntaxTree.SetUnusedNode(Upper_Same, /*Recursive*/ true );

        if ( First && LowerIsSelect )
        {
            SyntaxTree[Upper_A63].n_proc.becomes(a63);
            SyntaxTree[Upper_A63].n_subproc() = cak_x_mass_select;
        }

        SyntaxTree.SetUnusedNode(ToBeRemoved, /*Recursive*/ true );
        if ( Lower_Low > 0 )
            SyntaxTree[Lower_Low ].n_refback = Target;
        if ( Lower_Same > 0 )
            SyntaxTree[Lower_Same].n_refback = Target;
    }

    // ------------------------------------------------------

    // 2. set the correct values for the remaining lower node
    LowerNode->SetNodeID(UpperNode->NodeID());
    LowerNode->SetPosition(UpperNode->Position());
    LowerNode->SetDeclareCursor(UpperNode->IsDeclareCursor());
    if ( UpperNode->HasGivenSequence() )
        LowerNode->AddGivenSequenceNode();
    if ( !FatherIsSet && !LowerIsSelect )
        NewInternAPNode = SyntaxTree[NewInternAPNode].n_lo_level;
    LowerNode->SetInternAPNode(NewInternAPNode);           
    LowerNode->DetermineAPNode();                             
    if ( !IsDeclCursor && LowerNode->IsSet() && LowerNode->Position() == P_Start )
        LowerNode->SetA631();

	if ( CopySelListStart )		// DDT : ?????
        LowerNode->SetSelectListStart(UpperNode->SelectListStart());

	if ( !SetDistinctType(UpperNode, LowerNode) )
        return false;
    SetJoinType(UpperNode, LowerNode);

    // 3. link the nodes
    LowerNode->FatherNode() = UpperNode->FatherNode();
    LowerNode->FatherQuantifier() = UpperNode->FatherQuantifier();
    LowerNode->FatherQuantifier()->SetNodeRef(LowerNode);

    // 4. delete the upper node
	Container_Vector<Query_Quantifier*>& Quantifier = UpperNode->GetQuantifier();
	SAPDB_Int2 n=Quantifier.GetSize();
	for ( SAPDB_Int2 i=0; i<n; i++ )
	{
		// We must delete the remaining quantifier that points 
		// to the lower node before we can destroy the upper node !!!
		if ( Quantifier[i]->NodeRef() == LowerNode )
		{
			Quantifier[i]->SetNodeRef(0);
			destroy(Quantifier[i], UpperNode->GetAllocator());
			break;
		}
	}
    Quantifier.Delete();  

	UpperNode->SetSyntaxNode(-1);
    destroy(UpperNode, UpperNode->GetAllocator()); 

	return true;
}

// ------------------------------------------------------------------------------------------------

bool Rule_MergeE::IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) 
{
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];

    if (	Node->GetNodeType() == Query_Node::Select 
        &&  Quantifier->QuantifierType() == QT_E 
        &&  Quantifier->NodeType() == Query_Node::Select 
        &&  Quantifier->NodeRef()->Position() == P_Where   // DDT : P_Having ?
        &&  (   Quantifier->NodeRef()->GetSubQueryType() == SQT_EXISTS
		     /*|| Quantifier->NodeRef()->GetSubQueryType() == SQT_IN*/)
		// --
		&& !Node->Aggregates()
        && !Quantifier->NodeRef()->HasHaving() 
		)
    {
        if ( (Node->FQuantifierCnt() + Quantifier->NodeRef()->FQuantifierCnt() <= Query_MaxFQuantifier)
            && (!Node->OuterJoin() || Quantifier->NodeRef()->FQuantifierCnt()==1 )
            && (!Quantifier->OuterJoin() || Node->FQuantifierCnt()==1) )
        {
			if (      Node->HeadDistinct() == true 
					||  Node->BodyDistinctType() == DT_Permit 
    				|| (Node->NoDuplicateCondition() )	// Apply(..) must be changed if this is changed !
					)
				{ 
//				if ( Quantifier->NodeRef()->GetSubQueryType() == SQT_EXISTS )
//				{
					Query_QueryGraph& Graph      = Node->QueryGraph();
					Query_SyntaxTree& SyntaxTree = Graph.SyntaxTree();

					SyntaxTreeNodeNo S = Quantifier->NodeRef()->GetInternAPNode();
					S = SyntaxTree[S].n_refback;
					SAPDBERR_ASSERT_STATE( SyntaxTree[S].n_proc == a64
										&& SyntaxTree[S].n_subproc() == cak_x_predicate
										&& SyntaxTree[S].n_symb == s_exists);

					SyntaxTreeNodeNo H = SyntaxTree[S].n_refback;
					if ( SyntaxTree[H].n_proc == no_proc && SyntaxTree[H].n_symb == s_not )
						return false;

					// check if the exist quantifier froms a boolean factor
					return SyntaxTree.IsBooleanFactorInCondition(S);
//				}
//				else
//				{
//					SAPDBERR_ASSERT_STATE( Quantifier->NodeRef()->GetSubQueryType() == SQT_IN );
//					return true;
//				}
            }
        }
    }
    return false;
}

bool Rule_MergeE::Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) 
{
    Query_QueryGraph& Graph = Node->QueryGraph();
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];

    RuleTrace(GetAsciiRuleName(), Node, Quantifier);

    if (   Node->HeadDistinct() != true 
    	&& Node->BodyDistinctType() != DT_Permit )  // NoDuplicateCondition() must be true  
		Node->HeadDistinct() = true;

    if ( ((Query_SelectNode*)Node)->MergeSubquery((Query_SelectNode*)(Quantifier->NodeRef())) )
    {
        Optimized = true;
        Graph.SetRuleApplied(RuleID(), qrwMergeE);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

// CONVERT "EXISTS" to "IN"

bool Rule_ConvertExist::IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) 
{
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];

    if ( Node->GetNodeType() == Query_Node::Select )
    {
        if (   Quantifier->QuantifierType() == QT_E 
            && Quantifier->NodeType() == Query_Node::Select 
            && Quantifier->NodeRef()->Position() == P_Where	// DDT : what about P_Having ???
            && Quantifier->NodeRef()->GetSubQueryType() == SQT_EXISTS  )								
        {
			Query_SelectNode* UpperNode = (Query_SelectNode*)Node;
			Query_SelectNode* LowerNode = (Query_SelectNode*)Quantifier->NodeRef();

			bool LowerAggregates	=  LowerNode->Aggregates()	// SELECT * FROM X WHERE EXISTS (SELECT MIN(A1) FROM X1 WHERE A1 = X.A)
									|| LowerNode->HasHaving() 
									|| LowerNode->HasGroupBy()
									;

			bool UpperAggregates    =  UpperNode->Aggregates() 
//									|| UpperNode->HasHaving() 
//									|| UpperNode->HasGroupBy()
									;

			if (  !LowerAggregates /*&& !UpperAggregates*/	// DDT 
				&& LowerNode->IsCorrelatedSubquery() 
				&& LowerNode->CheckColumnsForExistToIn((Query_SelectNode*)Node) )
			{
                // check if the quantifier froms a boolean factor
				Query_SyntaxTree& SyntaxTree = Node->QueryGraph().SyntaxTree();

                SyntaxTreeNodeNo S = LowerNode->GetInternAPNode();
				S = SyntaxTree[S].n_refback;
				SAPDBERR_ASSERT_STATE( SyntaxTree[S].n_proc == a64
									&& SyntaxTree[S].n_subproc() == cak_x_predicate
									&& SyntaxTree[S].n_symb == s_exists);

				SyntaxTreeNodeNo H = SyntaxTree[S].n_refback;
				//if ( SyntaxTree[H].n_proc == no_proc && SyntaxTree[H].n_symb == s_not )  // this would allow conversion of NOT EXISTS
				//	S = H;

                // check if the exist quantifier froms a boolean factor
                return SyntaxTree.IsBooleanFactorInCondition(S);
			}
        }
    }
    return false;
}

bool Rule_ConvertExist::Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) 
{
    Query_QueryGraph& Graph = Node->QueryGraph();
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
 
    RuleTrace(GetAsciiRuleName(), Node, Quantifier);

    if ( ((Query_SelectNode*)Quantifier->NodeRef())->ExistSubqueryToIn((Query_SelectNode*)Node) )
    {
        Optimized = true;
        Graph.SetRuleApplied(RuleID(), qrwConvertExist);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Rule_PushDownProjection::IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) 
{
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
    Query_Node* LowerNode = Quantifier->NodeRef();

    if (   (Node->GetNodeType() == Query_Node::Select)
        && (Node->BodyDistinctType()!=DT_Preserve || Quantifier->DistinctType()==DT_Permit || Quantifier->QuantifierType() != QT_F)
        && !LowerNode->ProjectionPulledDown()
        )
    {
        return LowerNode->PullDownProjectionPossible();
    }
    return false;
}

bool Rule_PushDownProjection::Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) 
{
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
    Query_Node* LowerNode = Quantifier->NodeRef();
    
    if ( LowerNode->PullDownProjection() )
    {
        Query_QueryGraph& Graph = Node->QueryGraph();
        RuleTrace(GetAsciiRuleName(), Node, Quantifier);
//		Optimized = true;
        Graph.SetRuleApplied(RuleID(), qrwPushDownProjection);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Rule_PushDownPredicates::IsApplicable( Query_Node* Node ) 
{
    if ( Node->GetNodeType() == Query_Node::Select )
    {
        if (   Node->HasSearchCondition() 
            && !Node->QualificationPushedDown()
            &&  Node->HasFromSelects()
            && !Node->WQuantifierCnt()      // DDT: No selects within the where part for now
//			&& !Node->ContainsCorrelatedSubquery() // DDT: only correlated subqueries would be not allowed. but we would have to move 'normal' subqueries
//            && !Node->IsCorrelatedSubquery() // Correlation predicates won't be pushed down anyway
            )     
        {
			return true;
        }
    }
    return false;
}

bool Rule_PushDownPredicates::Apply( Query_Node*& Node, bool& Optimized ) 
{
    Query_QueryGraph& Graph = Node->QueryGraph();
    if ( ((Query_SelectNode*)Node)->PushDownSearchCond() )
    {
        Optimized = true;
        RuleTrace(GetAsciiRuleName(), Node);
//		Optimized = true;
        Graph.SetRuleApplied(RuleID(), qrwPushDownPredicates);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Rule_PushDownQuantifier::IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) 
{
    return false; // DDT

//    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
//
//    if (    Node->GetNodeType() == Query_Node::Select
//         && Node->FQuantifierCnt() > 1
//         && Quantifier->QuantifierType() == QT_F 
//         && Quantifier->NodeTypeSelectOrSet()
//         && !Node->HasForUpdate() 
////         && !Node->ContainsCorrelatedSubqueryRefersToMe() /// DDT: not possible until correct mapping for the corr_cols 
////         && !Node->WQuantifierCnt()     // DDT: No selects within the where part for now
////         && !Node->LQuantifierCnt()     // DDT: No selects within the select list for now
//       )
//    {
//        bool UpperAggregates    =  Node->Aggregates() 
//                                || Node->HasHaving() 
//                                || Node->HasGroupBy();
//
//        bool LowerAggregates    =  Quantifier->NodeRef()->Aggregates() 
//                                || Quantifier->NodeRef()->HasHaving() 
//                                || Quantifier->NodeRef()->HasGroupBy();
//
//        if ( Quantifier->NodeType() == Query_Node::Select 
//             && ( ( Node->HeadDistinct() == true ) ||
//                  ( Node->BodyDistinctType() == DT_Permit ) ||
//                  ( Quantifier->NodeRef()->BodyDistinctType() != DT_Enforce ) 
//                )
//           )
//        {   // Merge : SELECT + SELECT 
//
//            if ( (Node->FQuantifierCnt() + Quantifier->NodeRef()->FQuantifierCnt() <= Query_MaxFQuantifier)
//                && (!Node->OuterJoin() /*|| Quantifier->NodeRef()->FQuantifierCnt()==1*/)
//                && (Node->GetJoinType()==JT_NoJoin || !LowerAggregates)    // DDT
//                && !Node->HasOrderBy()          // DDT: No order by for now
//                && (!Quantifier->OuterJoin() || Node->FQuantifierCnt()==1)
//                && (!( UpperAggregates && LowerAggregates ) /*|| ( UpperAggregates && Quantifier->NodeRef()->OneTupleCondtion() )*/)
//                && !Node->HasForReuse()         // DDT
//            )
//            {
//                return true;
//            }
//        }
//
//        if ( Quantifier->NodeTypeSet() )
//        {   // Merge : SELECT + UNION/../.. 
//
//            if (   (Node->BodyDistinctType()==DT_Enforce || Node->OutputContainsAllColumnsFromQuantifier(Quantifier) )
//                && (!Node->OuterJoin() /*|| Quantifier->NodeRef()->FQuantifierCnt()==1*/)
//                && (Node->GetJoinType()==JT_NoJoin || !LowerAggregates)    // DDT
//                && !Node->HasOrderBy()          // DDT: No order by for now
//                && !Quantifier->OuterJoin()     // DDT: keiner der Quantifier darf ein OuterJoin sein!? Jedenfalls nicht die "benachbarten"
//                && !(UpperAggregates && LowerAggregates)
//                && !Node->HasForReuse()         // DDT
//                )
//            {
//
//                // DDT : for the first test
//    //            return ( Node->FQuantifierCnt() == 1 );   
//
//                // DDT : for the second test :
//                SAPDB_Int2 fq = 0;
//                SAPDB_Int2 n = Node->GetQuantifier().GetSize();
//                for ( SAPDB_Int2 i=0; i<n; i++ )
//                {
//                    if ( Node->GetQuantifier()[i]->NodeTypeSelectOrSet() )
//                        if ( ++fq>1 )  
//                            return false;               // DDT : there is more than one fromselect
//                }
//                return true;
//            }
//        }
//    }
//    return false;
}

bool Rule_PushDownQuantifier::Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) 
{
    return false; // DDT

//    Query_QueryGraph& Graph = Node->QueryGraph();
//    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
//    RuleTrace(GetAsciiRuleName(), Node, Quantifier);
//
//    if ( PushDown(Node, QuantifierNo) )
//    {
//        Optimized = true;
//#if defined(SAPDB_SLOW)
//        if ( QueryRewrite_Trace.TracesLevel(3) )
//        {
//            SAPDBERR_ASSERT_STATE( Graph.SyntaxTree().CheckNodes() );
//        }
//#endif
//        return true;
//    }
//    SAPDBERR_ASSERT_STATE( !Graph.IsOk() );
//    return false;
}

bool Rule_PushDownQuantifier::PushDown( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) 
{
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
    Query_SelectNode* UpperNode = (Query_SelectNode*)Node;
    Query_Node* LowerNode = Quantifier->NodeRef();

    if ( UpperNode->ReplaceAsteriskReferToLowerNodeForMerge(LowerNode) )
        if ( UpperNode->PushDownQuantifier(LowerNode) )
            return true;
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Rule_OptimizeSubquery::IsApplicable( Query_Node* Node ) 
{
	if ( Node->GetNodeType() == Query_Node::Select )
	{
		Query_SelectNode* S = (Query_SelectNode*)Node;
		if ( S->IsSubquery() && !S->SubqueryOptimized() )
		{
			Query_SubQueryType SQT = S->GetSubQueryType();
			bool Aggregates        =  S->Aggregates() 
								   || S->HasHaving() 
								   || S->HasGroupBy();
		    
			if ( !Aggregates && (/*SQT == SQT_ALL ||*/ SQT == SQT_ANY) && !Node->LQuantifierCnt() ) 
			{
				// RULE cannot be applied for ALL as it would be incorrect if the subquery is empty !
				// S is empty -> ALL : TRUE / ANY : FALSE   Who defines such mad logic ???

				if ( S->OutputContainsNullableColumns() )
					return false;

				Query_QueryGraph& Graph      = S->QueryGraph();
				Query_SyntaxTree& SyntaxTree = Graph.SyntaxTree();
				SyntaxTreeNodeNo H = SyntaxTree[Node->GetInternAPNode()].n_refback;
				SAPDBERR_ASSERT_STATE(   SyntaxTree[H].n_proc == no_proc 
									&& (  (SyntaxTree[H].n_symb == s_all && SQT_ALL)
										||(SyntaxTree[H].n_symb == s_any && SQT_ANY) ) );

				SyntaxTreeNodeNo R = SyntaxTree[H].n_refback;
				while ( SyntaxTree[R].n_lo_level != H )
				{
					H = R;
					R = SyntaxTree[H].n_refback;
				}

				if ( SyntaxTree[R].n_proc == a64 && SyntaxTree[R].n_subproc() == cak_x_predicate )
				{
					switch ( SyntaxTree[R].n_symb )
					{
					case s_greater:
					case s_greater_or_eq:
						return true;
						break;
					case s_less:
					case s_less_or_eq:
						return true;
						break;
					case s_equal:
					case s_unequal:
					default:
						return false;
						break;
					}
				}
			}
			else
			{
/*DDT*/
				if ( S->OneTupleAggregation() )	// there will be only one result row
					return true;
/*DDT*/
			}
		}
    }
    return false;
}

bool Rule_OptimizeSubquery::Apply( Query_Node*& Node, bool& Optimized ) 
{
    Query_QueryGraph& Graph = Node->QueryGraph();
    RuleTrace(GetAsciiRuleName(), Node);

	// conversions like:
	//		a < ALL  (select x from t)   ->   a < (select min(x) from t)
	//		a < SOME (select x from t)   ->   a < (select max(x) from t)
	//
	//		a > ALL  (select x from t)   ->   a > (select max(x) from t)
	//		a > SOME (select x from t)   ->   a > (select min(x) from t)
	//
	//      ALL/ANY/IN ( select aggr(x) from t) -> (select aggr(x) from t)

	Query_SyntaxTree& SyntaxTree = Graph.SyntaxTree();

	bool Aggregates        =  Node->Aggregates() 
							|| Node->HasHaving() 
							|| Node->HasGroupBy();

	Query_SubQueryType SQT = Node->GetSubQueryType();

	if ( !Aggregates )
	{

		SAPDBERR_ASSERT_STATE( SQT == SQT_ALL || SQT == SQT_ANY ); 
		SAPDBERR_ASSERT_STATE( Node->GetNodeType() == Query_Node::Select );

		SyntaxTreeNodeNo H = SyntaxTree[Node->GetInternAPNode()].n_refback;
		SAPDBERR_ASSERT_STATE(   SyntaxTree[H].n_proc == no_proc 
								&& (  (SyntaxTree[H].n_symb == s_all && SQT_ALL)
									||(SyntaxTree[H].n_symb == s_any && SQT_ANY) ) );

		SyntaxTreeNodeNo R = SyntaxTree[H].n_refback;
		while ( SyntaxTree[R].n_lo_level != H )
		{
			H = R;
			R = SyntaxTree[H].n_refback;
		}

		if ( SyntaxTree[R].n_proc == a64 && SyntaxTree[R].n_subproc() == cak_x_predicate )
		{
			switch ( SyntaxTree[R].n_symb )
			{
			case s_greater:
			case s_greater_or_eq:
				if ( Node->AddMinOrMaxToAllOutputColumn(/*Max*/ SQT == SQT_ALL ) )
				{
					R = Node->GetInternAPNode();
					SAPDBERR_ASSERT_STATE( SyntaxTree[R].n_querycheck.includes(qcn_SubQInfo) );
					SyntaxTree[R].n_length() = cak_x_one_val_subquery;

					H = SyntaxTree[R].n_refback;
					SyntaxTree.UnlinkNode(H, /*SetUnused*/true);
					Node->EliminateOrderBy();
					Node->SetSubqueryOptimized();
					Node->SetSubQueryType(SQT_OTHER);

//					Optimized = true;
					Graph.SetRuleApplied(RuleID(), qrwOptimizeSubquery);
					return true;
				}
				return false;
				break;
			case s_less:
			case s_less_or_eq:
				if ( Node->AddMinOrMaxToAllOutputColumn(/*Max*/ SQT == SQT_ANY ) )
				{
					R = Node->GetInternAPNode();
					SAPDBERR_ASSERT_STATE( SyntaxTree[R].n_querycheck.includes(qcn_SubQInfo) );
					SyntaxTree[R].n_length() = cak_x_one_val_subquery;

					H = SyntaxTree[R].n_refback;
					SyntaxTree.UnlinkNode(H, /*SetUnused*/true);
					Node->EliminateOrderBy();
					Node->SetSubqueryOptimized();
					Node->SetSubQueryType(SQT_OTHER);

//					Optimized = true;
					Graph.SetRuleApplied(RuleID(), qrwOptimizeSubquery);
					return true;
				}
				return false;
				break;
			case s_equal:
			case s_unequal:
			default:
				break;
			}
		}
	}
	else
	{
/*DDT*/
		SAPDBERR_ASSERT_STATE ( Node->OneTupleAggregation() );

		SyntaxTreeNodeNo R = Node->GetInternAPNode();
		SAPDBERR_ASSERT_STATE( SyntaxTree[R].n_querycheck.includes(qcn_SubQInfo) );

		if ( SQT == SQT_ALL || SQT == SQT_ANY )
		{
			SyntaxTreeNodeNo H = SyntaxTree[R].n_refback;
			SyntaxTree.UnlinkNode(H, /*SetUnused*/true);
		}
		else
		{
			if ( SQT == SQT_IN )	// DDT : we have to convert IN to = 
			{
				SyntaxTreeNodeNo H = R;
				SyntaxTreeNodeNo R = SyntaxTree[H].n_refback;
				while ( SyntaxTree[R].n_lo_level != H )
				{
					H = R;
					R = SyntaxTree[H].n_refback;
				}
				SAPDBERR_ASSERT_STATE( SyntaxTree[R].n_proc == a64 && SyntaxTree[R].n_subproc() == cak_x_in_pred );
				SyntaxTree[R].n_subproc() = cak_x_predicate;
				SyntaxTree[R].n_symb.becomes(s_equal);
			}
			else
			{
				if ( SQT == SQT_EXISTS		// OneTupleAggregation allways gives 1 resultrecord, even if the fromtable is empty
					&& !Node->HasHaving()   // unless there is HAVING or GROUP BY
					&& !Node->HasGroupBy() )// therefore "EXISTS ( OneTupleSubquery )" is always TRUE if there is no having
				{
					if ( !SyntaxTree.ContainsParameter(R, /*ConsiderSame=*/true) )
					{

						SyntaxTreeNodeNo N1 = SyntaxTree.GetFreeNode();
						SyntaxTreeNodeNo N2 = SyntaxTree.GetFreeNode();
						
						if ( N1>0 && N2>0 )
						{
							SyntaxTreeNodeNo H = SyntaxTree[R].n_refback;
							SyntaxTree.UnlinkNodeWithSameLevel(R,  /*SetUnused=*/true);
							SyntaxTree[H].n_proc.becomes(a64);
							SyntaxTree[H].n_subproc() = 0;
							SyntaxTree[H].n_symb.becomes(s_is_true);

							SyntaxTree[N1].n_proc.becomes(no_proc);
							SyntaxTree[N1].n_subproc() = 0;
							SyntaxTree[N1].n_symb.becomes(s_true);

							SyntaxTree[N2].n_proc.becomes(no_proc);
							SyntaxTree[N2].n_subproc() = 0;
							SyntaxTree[N2].n_symb.becomes(s_true);

							SyntaxTree[H ].n_lo_level = N1;
							SyntaxTree[N1].n_refback  = H;
							SyntaxTree[N1].n_sa_level = N2;
							SyntaxTree[N2].n_refback  = N1;

//							Node->Context().SQLManContext().a_count_literals += 1;

							SAPDBERR_ASSERT_STATE( Node->FatherNode()->GetNodeType() == Query_Node::Select );
							Query_SelectNode* HNode = (Query_SelectNode*)(Node->FatherNode());

							HNode->RemoveDeletedSubqueries();		// this will destroy Node  !!!
							HNode->SetPredicatesNotSimplified();
							Node = HNode;

							Optimized = true;
//							Graph.SetRuleApplied(RuleID(), qrwSimplifyPredicates);
							Graph.SetRuleApplied(RuleID(), qrwOptimizeSubquery);
							return true;
						}
						else
						{
							Graph.SetError("No more AP Node");
							return false;
						}
					}
				}
			}
		}
		Node->EliminateOrderBy();
		Node->SetSubqueryOptimized();
		Node->SetSubQueryType((SQT==SQT_EXISTS?SQT_EXISTS:SQT_OTHER));

//		Optimized = true;
		Graph.SetRuleApplied(RuleID(), qrwOptimizeSubquery);
		return true;
/*DDT*/
	}
	SAPDBERR_ASSERT_STATE( 0 );
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Rule_ConvertToExist::IsApplicable( Query_Node* Node ) 
{
    if (   Node->GetNodeType() == Query_Node::Intersect 
//        || Node->GetNodeType() == Query_Node::Difference // DDT : we can't handle MergeE for NOT EXISTS so far !!!
        )     
    {
        Query_SetNode* SetNode = (Query_SetNode*)Node;
        return SetNode->CheckForIntOrExcept2Exist();
    }
    return false;
}

bool Rule_ConvertToExist::Apply( Query_Node*& Node, bool& Optimized ) 
{
    RuleTrace(GetAsciiRuleName(), Node);

    Query_QueryGraph& Graph      = Node->QueryGraph();
    Query_SyntaxTree& SyntaxTree = Graph.SyntaxTree();

    SAPDBMem_IRawAllocator& Alloc = Graph.Allocator();

    Container_Vector<Query_Quantifier*>& Quantifier = Node->GetQuantifier();
    Query_SetNode*    SetNode       = (Query_SetNode*) Node;
    Query_SelectNode* LeftSelNode   = (Query_SelectNode*)(Quantifier[0]->NodeRef());
    Query_SelectNode* RightSelNode  = (Query_SelectNode*)(Quantifier[1]->NodeRef());

    // 1. adjust syntaxtree
    SyntaxTreeNodeNo IntN    = SetNode->GetInternAPNode();
    SyntaxTreeNodeNo H = SyntaxTree[IntN].n_refback;
    bool first = ( SyntaxTree[H].n_proc == a63 && SyntaxTree[H].n_subproc() == cak_x_start_union );
    SyntaxTreeNodeNo SLeft  = LeftSelNode->GetInternAPNode();
    SyntaxTreeNodeNo SRight = RightSelNode->GetInternAPNode();
        
    SyntaxTree.UnlinkNodeWithSameLevel(SLeft,  /*SetUnused=*/false );
    SyntaxTree.UnlinkNodeWithSameLevel(SRight, /*SetUnused=*/false );
    SyntaxTreeNodeNo Target = (first ? H : IntN);
    if ( first )
        SyntaxTree.UnlinkNode(IntN, /*SetUnused=*/true );

    SyntaxTreeNodeNo Ba = SyntaxTree[Target].n_refback;
    SyntaxTree[Target] = SyntaxTree[SLeft];
    SyntaxTree[Target].n_refback = Ba;
    if ( SyntaxTree[Target].n_lo_level > 0 )
        SyntaxTree[SyntaxTree[Target].n_lo_level].n_refback = Target;
    if ( SyntaxTree[Target].n_sa_level > 0 )
        SyntaxTree[SyntaxTree[Target].n_sa_level].n_refback = Target;
    SyntaxTree.SetUnusedNode(SLeft, /*Recursive=*/false );

    SyntaxTreeNodeNo Exist = SyntaxTree.GetFreeNode();
    //SyntaxTreeNodeNo O1   = SyntaxTree.GetFreeNode();
    //SyntaxTreeNodeNo O2   = SyntaxTree.GetFreeNode();
    //SyntaxTreeNodeNo O3   = SyntaxTree.GetFreeNode();
    if ( Exist<=0 /*|| O1<=0 || O2<=0 || O3<=0*/ )
    {
        Graph.SetError("INT2EXIST::Apply 1");
        return false;
    }

    SyntaxTree[Exist].n_proc.becomes(a64);
    SyntaxTree[Exist].n_subproc() = cak_x_predicate;
    SyntaxTree[Exist].n_symb.becomes(s_exists);
    SyntaxTree[Exist].n_lo_level = SRight;
    SyntaxTree[SRight].n_refback = Exist;
    if ( SetNode->GetNodeType() == Query_Node::Difference )
    {
        SyntaxTreeNodeNo Not = SyntaxTree.GetFreeNode();
        if ( Not<=0 )
        {
            Graph.SetError("INT2EXIST::Apply 2");
            return false;
        }
        SyntaxTree[Not].n_proc.becomes(no_proc);
        SyntaxTree[Not].n_symb.becomes(s_not);

        SyntaxTree[Not  ].n_sa_level = Exist;
        SyntaxTree[Exist].n_refback  = Not;
        Exist = Not;
    }

    LeftSelNode->AddQualification(Exist, /*AsWhere*/true);

    SyntaxTreeNodeNo Qual = -1;
    SAPDB_Int2 n = LeftSelNode->OutputColumns();
    for ( SAPDB_Int2 i=1; i<=n; i++ )
    {
        SyntaxTreeNodeNo Eq = SyntaxTree.GetFreeNode();
        SyntaxTreeNodeNo ColR = SyntaxTree.GetFreeNode();
        SyntaxTreeNodeNo ColL = SyntaxTree.GetFreeNode();
        if ( Eq>0 && ColR>=0 && ColL>=0 )
        {
            SQLMan_Identifier ColName(false), RefName(false);
            SAPDB_Int2 QuantifierSyntaxNode=0, QuantifierID=0;
            SAPDB_Int2 ColumnIndex=0, ExtColNo=0, SelColNo=cak_qrewrite_ref_star;
            bool IsNullable=true;

            SyntaxTree[Eq].n_proc.becomes(a64);
            SyntaxTree[Eq].n_subproc() = cak_x_predicate;
            SyntaxTree[Eq].n_symb.becomes(s_equal);

            SyntaxTree[Eq  ].n_lo_level = ColR;
            SyntaxTree[ColR].n_refback  = Eq;
            SyntaxTree[ColR].n_sa_level = ColL;
            SyntaxTree[ColL].n_refback  = ColR;

		    SyntaxTree[ColR].n_proc.becomes(no_proc);
            SyntaxTree[ColR].n_symb.becomes(s_columnid);
		    SyntaxTree[ColR].n_fromtabnode() = -1;
		    SyntaxTree[ColR].n_columnindex() = -1;

		    SyntaxTree[ColL].n_proc.becomes(no_proc);
            SyntaxTree[ColL].n_symb.becomes(s_columnid);
		    SyntaxTree[ColL].n_fromtabnode() = -1;
		    SyntaxTree[ColL].n_columnindex() = -1;

            Query_OutColType ColType;
            Query_MappingInfo MappingInfo( ColType, MT_Normal, P_Where, false);

            Query_Column Col = Query_Column(ColR);
            Col.SetInfo(-1, RightSelNode->NodeID(), QuantifierSyntaxNode, ColName, i, ExtColNo, IsNullable);
            RightSelNode->MappColumnFromOuterSelect(&Col, &MappingInfo );
            RightSelNode->AppendColumn(RightSelNode->GetSearchCondColumns(), Col);

            Col = Query_Column(ColL);
            Col.SetInfo(LeftSelNode->NodeID(), LeftSelNode->NodeID(), QuantifierSyntaxNode, ColName, i, ExtColNo, IsNullable);
            LeftSelNode->MappColumnFromOuterSelect(&Col, &MappingInfo );
            RightSelNode->AppendColumn(RightSelNode->GetSearchCondColumns(), Col);

            LeftSelNode->ChangeCorrelatedColumnsRefersToMe(+1);
            RightSelNode->ChangeCorrelatedColumns(+1);

            if ( Qual > 0 )
            {
                SyntaxTreeNodeNo And = SyntaxTree.GetFreeNode();
                if ( And > 0 )
                {
                    SyntaxTree[And ].n_proc.becomes(a64);
                    SyntaxTree[And ].n_symb.becomes(s_and);

                    SyntaxTree[And ].n_lo_level = Eq;
                    SyntaxTree[Eq  ].n_refback  = And;
                    SyntaxTree[Eq  ].n_sa_level = Qual;
                    SyntaxTree[Qual].n_refback  = Eq;
                    Qual = And;
                }
                else
                {
                    Graph.SetError("INT2EXIST::Apply 3");
                    return false;
                }
            }
            else
            {
                Qual = Eq;
            }
        }
        else
        {
            Graph.SetError("INT2EXIST::Apply 4");
            return false;
        }
    }
    RightSelNode->AddQualification(Qual, /*AsWhere*/true);

	// DDT : we don't need an order by with exist subqueries
	//SyntaxTree[O1].n_proc.becomes(a63);
 //   SyntaxTree[O1].n_subproc() = cak_x_order;
	//SyntaxTree[O2].n_proc.becomes(a63);
 //   SyntaxTree[O2].n_subproc() = cak_x_sort_spec;
	//SyntaxTree[O3].n_proc.becomes(no_proc);
 //   SyntaxTree[O3].n_symb.becomes(s_equal);
 //   SyntaxTree[O3].n_pos() = 1;

 //   SyntaxTree[O1].n_lo_level = O2;
 //   SyntaxTree[O2].n_refback  = O1;
 //   SyntaxTree[O2].n_lo_level = O3;
 //   SyntaxTree[O3].n_refback  = O2;
 //   RightSelNode->AddOrderBy(O1);

    if ( SetNode->Position() == P_Start )
        Graph.SetASelectNode(Target);

    // 2. set the correct values for the remaining select nodes
    LeftSelNode->SetNodeID(SetNode->NodeID());
    LeftSelNode->SetPosition(SetNode->Position());
    LeftSelNode->SetInternAPNode(Target);           
    LeftSelNode->DetermineAPNode(); 
    RightSelNode->SetPosition(P_Where);
    RightSelNode->DetermineAPNode(); 

    // 3. link the nodes
    LeftSelNode->FatherNode() = SetNode->FatherNode();
    LeftSelNode->FatherQuantifier() = SetNode->FatherQuantifier();
    LeftSelNode->FatherQuantifier()->SetNodeRef(LeftSelNode);
    LeftSelNode->AddQuantifier(RightSelNode, DT_Permit, QT_E);

    // 4. Add DISTINCT
    LeftSelNode->AddDistinctNode();
    LeftSelNode->BodyDistinctType() = DT_Enforce;
    LeftSelNode->HeadDistinct() = true;
    LeftSelNode->FatherQuantifier()->DistinctType() = DT_Preserve; 
    SAPDBERR_ASSERT_STATE( RightSelNode->FatherNode() == LeftSelNode );

    // 5. delete the intersect node
    SetNode->GetQuantifier().Delete();  // We must delete the remaining quantifier that points 
                                        // to the lower node before we can destroy the set node !!!

    SetNode->SetSyntaxNode(-1);
    destroy(SetNode, SetNode->GetAllocator()); 
    Node = LeftSelNode;

//		Optimized = true;
    Graph.SetRuleApplied(RuleID(), qrwConvertToExist);
    return true;
}

// ------------------------------------------------------------------------------------------------

bool Rule_DistinctPullUp::IsApplicable( Query_Node* Node ) 
{
    if ( Node->GetNodeType() == Query_Node::Select && !Node->HasGroupBy() 
		 && (Node->BodyDistinctType() == DT_Enforce || !Node->HeadDistinct()) )
    {
		if ( Node->NoDuplicateCondition() || Node->OneTupleCondition() )
			return true;
    }
    return false;
}

bool Rule_DistinctPullUp::Apply( Query_Node*& Node, bool& Optimized ) 
{
    RuleTrace(GetAsciiRuleName(), Node);

    Query_QueryGraph& Graph = Node->QueryGraph();
    if ( ((Query_SelectNode*)Node)->Distinct() )
    {
        if ( !((Query_SelectNode*)Node)->RemoveDistinctNode() )
        {
            SAPDBERR_ASSERT_STATE( !Graph.IsOk() );
            return false;
        }
		Optimized = true;
    }

    Node->HeadDistinct()        = true;
    Node->BodyDistinctType()    = DT_Preserve;

    Graph.SetRuleApplied(RuleID(), qrwDistinctPullUp);
    return true;
}

// ------------------------------------------------------------------------------------------------
// ok

bool Rule_DistinctForSubqueries::IsApplicable( Query_Node* Node, SAPDB_UInt2 QuantifierNo ) 
{
    // DDT : for E/A quantifier it doesn't matter if they are distinct or not!
    //       BUT is it faster to remove the DISTINCT if any ???

    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];

    if ( Node->GetNodeType() == Query_Node::Select && Quantifier->DistinctType() != DT_Permit )
    {
        if ( ( Quantifier->QuantifierType() == QT_E ) ||
             ( Quantifier->QuantifierType() == QT_A ) )
            return true;
    }
    return false;
}

bool Rule_DistinctForSubqueries::Apply( Query_Node*& Node, SAPDB_UInt2 QuantifierNo, bool& Optimized ) 
{
    Query_Quantifier* Quantifier = Node->GetQuantifier()[QuantifierNo];
    Query_QueryGraph& Graph = Node->QueryGraph();
    RuleTrace(GetAsciiRuleName(), Node, Quantifier);

    // we will set the DistinctType to allow E2F but we won't remove the DISTINCT (if any) for the case E2F cannot be applied
    Quantifier->DistinctType() = DT_Permit;

    Graph.SetRuleApplied(RuleID(), qrwDistinctForSubqueries);
    return true;
}

// ------------------------------------------------------------------------------------------------
// ok

bool Rule_DISTPDFR::IsApplicable( Query_Node* Node ) 
{
    Query_Node::NodeType BT = Node->GetNodeType();

    if  (   BT == Query_Node::Select 
         || BT == Query_Node::Union        
         || BT == Query_Node::Intersect    
         || BT == Query_Node::Difference   
         )
    {
        if ( Node->FatherQuantifier()->DistinctType() == DT_Permit )
           if ( Node->BodyDistinctType() != DT_Permit )
                return true;
    }
    return false;
}

bool Rule_DISTPDFR::Apply( Query_Node*& Node, bool& Optimized ) 
{
    Query_QueryGraph& Graph = Node->QueryGraph();
    RuleTrace(GetAsciiRuleName(), Node);

    Node->BodyDistinctType() = DT_Permit;

    Graph.SetRuleApplied(RuleID(), qrwDistinctPushDownFrom);
    return true;
}

// ------------------------------------------------------------------------------------------------
// ok

bool Rule_DISTPDTO::IsApplicable( Query_Node* Node ) 
{
    Query_Node::NodeType    BT  = Node->GetNodeType();
    Query_DistinctType      DT  = Node->BodyDistinctType();

    if (   (( BT == Query_Node::Select ) ||
            ( BT == Query_Node::Union )  ||
            ( BT == Query_Node::Intersect) ||
            ( BT == Query_Node::Difference)) &&
           ((DT == DT_Permit) ||
            (DT == DT_Enforce)))
    {
        if ( !Node->Aggregates() )
        {
            // check if application of this rule is necessary at all...
            for ( unsigned i=0; i < Node->GetQuantifier().GetSize(); i++ )
            {
                Query_Quantifier* Q = Node->GetQuantifier()[i];
                if (( Q->QuantifierType() == QT_F ) && ( Q->DistinctType() != DT_Permit ))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Rule_DISTPDTO::Apply( Query_Node*& Node, bool& Optimized ) 
{
	Query_Node::NodeType    BT  = Node->GetNodeType();
    bool RemoveAll = ( BT == Query_Node::Union )     ||
                     ( BT == Query_Node::Intersect ) ||
                     ( BT == Query_Node::Difference );

    bool applied = false;
	for ( unsigned i=0; i < Node->GetQuantifier().GetSize(); i++ )
    {
        Query_Quantifier* Q = Node->GetQuantifier()[i];
        if (( Q->QuantifierType() == QT_F ) && ( Q->DistinctType() != DT_Permit ))
        {
			if ( RemoveAll )
				Q->NodeRef()->RemoveAllFromSet();
            Q->DistinctType() = DT_Permit;
            applied |= true;
        }
    }
    if ( applied )
    {
        Query_QueryGraph& Graph = Node->QueryGraph();
        RuleTrace(GetAsciiRuleName(), Node);
        Graph.SetRuleApplied(RuleID(), qrwDistinctPushDownTo);
    }
    return applied;
}

// ------------------------------------------------------------------------------------------------
