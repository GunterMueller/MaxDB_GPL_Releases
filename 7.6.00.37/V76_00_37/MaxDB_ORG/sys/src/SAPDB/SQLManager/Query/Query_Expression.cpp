/*!***************************************************************************

    @file     Query_Expression.cpp
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

#include "SAPDB/SQLManager/Query/Query_Expression.hpp"
#include "SAPDB/SQLManager/Query/Query_ExpressionNode.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_OutputColumn.hpp"

#include "SAPDB/RunTime/RTE_CurrentStack.hpp"

// ------------------------------------------------------------------------------------------------

Query_Expression::Query_Expression( void )
    : mRoot(0)
{
}

Query_Expression::~Query_Expression( void )
{
//    SAPDBERR_ASSERT_STATE( !mRoot );
}

Query_Expression* Query_Expression::CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Query_Expression* Exp = new (Alloc) Query_Expression();
    if ( Exp )
    {
        if ( mRoot )
            Exp->mRoot = mRoot->CreateDuplicate(Alloc, SyntaxTree, mpMap, mpQMap);
    }
    return Exp;
}

Query_Expression Query_Expression::CreateDuplicate2( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Query_Expression Exp = Query_Expression();
    if ( mRoot )
        Exp.mRoot = mRoot->CreateDuplicate(Alloc, SyntaxTree, mpMap, mpQMap);
    return Exp;
}

void Query_Expression::Destroy( SAPDBMem_IRawAllocator& Alloc )
{
    if ( mRoot )
    {
        mRoot->Destroy(Alloc);
        destroy(mRoot, Alloc);
        mRoot = 0;
    }
}

void Query_Expression::SetInAggr( void )
{
    Query_ExpressionNode* p = mRoot;
    while ( p )
    {
        p->SetInAggr(true);
        p = p->mNext;
    }
}

bool Query_Expression::Parse( Query_SelectNode* Father, Query_OutputColumn* OutCol, Query_Position Position, SyntaxTreeNodeNo Node, bool InAggr, bool ComplexViewReplacement )
{
    // DDT : OutCol sollte nicht übergeben werden, sondern die Attribute nach Query_Expression verschoben werden !!!

    Query_QueryGraph& QueryGraph = Father->QueryGraph();
    Query_SyntaxTree& SyntaxTree = QueryGraph.SyntaxTree();
    bool rc=true;
    bool subq=false;

    {
        SAPDB_Int4 StackFree = RTE_CurrentStackFree();

        if ( StackFree < 4096 )
        {
            QueryGraph.SetError("Stack exhausted", /*PrintToTrace=*/false);
            return false;
        }
    }

    // DDT : zunächst die dummy version, die auch nur subqueries und columns berücksichtigt !

    while ( Node>0 && rc && QueryGraph.IsOk() )
    {
        // 1. Check for aggregation
        bool Aggr = InAggr;
        if ( SyntaxTree.IsAggregationNode(Node) )
        {
            if ( InAggr )
            {
                QueryGraph.SetError("Aggregation within Aggregation");
                return false;
            }

            /**/if ( OutCol )
                {
                    if ( OutCol->GetMainColType() == Query_OutColType::OCTM_Unknown )
                        OutCol->SetColType(Query_OutColType::OCTM_Aggregate, 0, 1); 
                    else
                        if ( OutCol->GetMainColType() == Query_OutColType::OCTM_Expression )
                            OutCol->GetSubColType() |= Query_OutColType::OCTS_ExpressionContainsAggr;
                    Father->SetAggregatesInOutput();
                }
                else
                {
                    Father->SetAggregatesNotInOutput();
            /**/}
            Aggr = true;
        }

        if ( !OutCol )
        {
            // 2. Check for outer join predicate
            if ( SyntaxTree[Node].n_proc == a64 )
            {
                switch ( SyntaxTree[Node].n_subproc() )
                {
                case cak_x_left_outer_join:     
                    Father->AddJoinType(JT_LeftOuter);
			        if ( ComplexViewReplacement )
				        QueryGraph.ManipulationContext().SQLManContext().a_outer_join = true;
                    break;
                case cak_x_right_outer_join:    
                    Father->AddJoinType(JT_RightOuter);
			        if ( ComplexViewReplacement )
				        QueryGraph.ManipulationContext().SQLManContext().a_outer_join = true;
                    break;
                case cak_x_full_outer_join:     
                    Father->AddJoinType(JT_FullOuter);
			        if ( ComplexViewReplacement )
				        QueryGraph.ManipulationContext().SQLManContext().a_outer_join = true;
                    break;
                }
            }
        }
        else // if ( OutCol )
        {
            // 3. Check for expression
            if (    SyntaxTree[Node].n_proc == a64 
                 || SyntaxTree[Node].n_proc == a641
                 || SyntaxTree[Node].n_proc == no_proc )
            {
                if ( OutCol->GetMainColType() == Query_OutColType::OCTM_Unknown )
                    OutCol->SetColType(Query_OutColType::OCTM_Expression, 0, 1);   
                else
                    if ( InAggr )
                        OutCol->GetSubColType() |= Query_OutColType::OCTS_AggregationContainsExpression;
            }
        }

        // 4. Check for Subquery
	    if (   SyntaxTree[Node].n_proc == a63query_spec 
            || (SyntaxTree[Node].n_proc == a63 && SyntaxTree[Node].n_subproc() == cak_x_start_union) )
        {
            // stop at subselects as they define their own operator
            if ( OutCol )
            {
                if ( InAggr )
                {
                    OutCol->GetSubColType() |= Query_OutColType::OCTS_AggregationContainsSubquery;
                }
                else
                {
                    if ( OutCol->GetMainColType() == Query_OutColType::OCTM_Unknown )
                        OutCol->SetColType(Query_OutColType::OCTM_Subquery, 0, 1);
                    else
                        if ( OutCol->GetMainColType() == Query_OutColType::OCTM_Expression )
                            OutCol->GetSubColType() |= Query_OutColType::OCTS_ExpressionContainsSubquery;
                }
            }

            Query_Node* SubQuery = Father->AddQuery(Node, Position, ComplexViewReplacement);
            if ( SubQuery )
                rc &= SubQuery->Parse(ComplexViewReplacement);
            else
                return false;
            subq = true;
        }

        // 5.
        if (   SyntaxTree[Node].n_proc == no_proc 
            && SyntaxTree[Node].n_symb == s_rowno )
        {
            Father->SetRownoPredicate();
        }

        // 6. Check for column
        if ( SyntaxTree[Node].n_proc == no_proc 
            && ( SyntaxTree[Node].n_symb == s_authid
            ||   SyntaxTree[Node].n_symb == s_tablename 
            ||   SyntaxTree[Node].n_symb == s_columnname ) )
        {
            rc &= ParseCol(Father, OutCol, Node, InAggr);
        }

        if ( !subq && SyntaxTree[Node].n_lo_level )
            rc &= Parse(Father, OutCol, Position, SyntaxTree[Node].n_lo_level, Aggr, ComplexViewReplacement);

        Node = SyntaxTree[Node].n_sa_level;
        subq = false;
    }
    return rc && QueryGraph.IsOk();
}

bool Query_Expression::ParseCol( Query_SelectNode* Father, Query_OutputColumn* OutCol, SyntaxTreeNodeNo& Node, bool InAggr )
{
    Query_QueryGraph& QueryGraph = Father->QueryGraph();
    Query_SyntaxTree& SyntaxTree = QueryGraph.SyntaxTree();

    SyntaxTreeNodeNo SavNode = Node;
    SQLMan_Identifier Schema(false), TableName(false), ColumnName(false);
    SAPDB_Int2 ColErrPos = cak_qrewrite_ref_init;
    bool SchemaOrTableName = false;

    // Schema
    if ( SyntaxTree[Node].n_symb == s_authid )
    {
        if ( ColErrPos <= 0 )
            ColErrPos = SyntaxTree[Node].n_pos();
        QueryGraph.GetSchemaName(Node, Schema);
        SchemaOrTableName = true;
    }
    else
    {
        Schema.SetBlank();
    }
    // Tablename
    if ( SyntaxTree[Node].n_symb == s_tablename )
    {
        if ( ColErrPos <= 0 )
            ColErrPos = SyntaxTree[Node].n_pos();
        QueryGraph.GetIdentifier(Node, TableName);
        Node = SyntaxTree[Node].n_sa_level;
        SchemaOrTableName = true;
    }
    else
    {
        TableName.SetBlank();
    }
    // Columnname
    if ( SyntaxTree[Node].n_symb == s_columnname )
    {
        if ( ColErrPos <= 0 )
            ColErrPos = SyntaxTree[Node].n_pos();
        QueryGraph.GetIdentifier(Node, ColumnName);

        if ( OutCol && OutCol->GetMainColType() == Query_OutColType::OCTM_Unknown )
        {
#if defined (SAPDB_SLOW)
            QueryGraph.SetError("ParseCol unknown type", Node);
            SAPDBERR_ASSERT_STATE( 0 );
#endif
            return false;
        }

//        SyntaxTree[Node].n_symb.becomes(s_columnid); // DDT 
        Query_Column* C = new(Father->GetAllocator()) Query_Column(Node, ColErrPos, Schema, TableName, ColumnName);
        if ( !C )
            return false;
        C->SetInAggr(InAggr);
        return AddNodes(C);
    }
    if ( SyntaxTree[Node].n_symb == s_columnid )
    {
        SAPDBERR_ASSERT_STATE( 0 );
        return false;
    }
    return true;
}


bool Query_Expression::Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy )
{
    Query_ExpressionNode* p = mRoot;
    bool rc=true;
    while ( p && rc )
    {
        rc &= p->Semantics(Father, MayBeCorrelated, ForOrderBy);
        p = p->mNext;
    }
    return rc;
}

bool Query_Expression::SemanticsCorrelation( Query_SelectNode* SubQuery, bool& Unidentified )
{
    Query_ExpressionNode* p = mRoot;
    bool rc=true;
    while ( p && rc )
    {
        rc &= p->SemanticsCorrelation(SubQuery, Unidentified);
        p = p->mNext;
    }
    return rc;
}

bool Query_Expression::CheckCorrelatedColumnsForExistToIn( Query_SelectNode* Father )
{
    Query_ExpressionNode* p = mRoot;
    bool rc=true;
    while ( p && rc )
    {
        rc &= p->CheckCorrelatedColumnsForExistToIn(Father);
        p = p->mNext;
    }
    return rc;
}

void Query_Expression::DeleteEmptyColumnNodes( Query_SelectNode* InnerSelectNode )
{
	Query_ExpressionNode* Act  = mRoot;
	while ( Act )
	{
		Query_ExpressionNode* Next = Act->mNext;
		if ( Act->GetNodeID() == -1 && Act->GetQuantifierID() == -1 )
			DelNode(Act, InnerSelectNode->GetAllocator());
		Act = Next;
	}
}

// ------------------------------------------------------------------------------------------------
