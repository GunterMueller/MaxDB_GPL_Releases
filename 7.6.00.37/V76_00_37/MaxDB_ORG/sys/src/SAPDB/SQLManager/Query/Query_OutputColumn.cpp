/*!***************************************************************************

    @file     Query_OutputColumn.cpp
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

#include "SAPDB/SQLManager/Query/Query_OutputColumn.hpp"
#include "SAPDB/SQLManager/Query/Query_Column.hpp"
#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_Quantifier.hpp"

// ------------------------------------------------------------------------------------------------

Query_OutputColumn::Query_OutputColumn( void )
    : Query_ColumnSpec()
    , mRefName()
    , mColumnCount(0)
    , mRefCount(0)
    , mOutColType()
{
}

Query_OutputColumn::Query_OutputColumn( SyntaxTreeNodeNo Node )
    : Query_ColumnSpec(Node)
    , mRefName()
    , mColumnCount(0)
    , mRefCount(0)
    , mOutColType()
{
}

Query_OutputColumn::Query_OutputColumn( const Query_OutputColumn& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : Query_ColumnSpec(C)
    , mRefName(C.mRefName)
    , mColumnCount(C.mColumnCount)
    , mRefCount(C.mRefCount)
    , mOutColType(C.mOutColType)
{
    //if ( mpMap )
    //    SyntaxTree.MapNode(mSyntaxNode, mpMap); 
}

Query_OutputColumn::~Query_OutputColumn( void )
{
}

void Query_OutputColumn::Destroy( SAPDBMem_IRawAllocator& Alloc )
{
    Query_ColumnSpec::Destroy(Alloc);
}

void Query_OutputColumn::RenewReferenceName( Query_QueryGraph& Graph )
{
    Query_SyntaxTree& SyntaxTree = Graph.SyntaxTree();
    SyntaxTreeNodeNo Node = SyntaxTree[mSyntaxNode].n_lo_level;
    if ( (SyntaxTree[Node].n_proc == 0) &&
        (SyntaxTree[Node].n_symb == s_reference_name) )
    {
        Graph.GetIdentifier(Node, mRefName);
    }
}


Query_OutputColumn* Query_OutputColumn::CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Query_OutputColumn* N = new(Alloc) Query_OutputColumn(*this, SyntaxTree, mpMap, mpQMap);
    if ( N )
    {
        SyntaxTree.MapNode(mSyntaxNode, mpMap);
        N->mExpression = mExpression.CreateDuplicate2(Alloc, SyntaxTree, mpMap, mpQMap);
    }
    return N;
}

bool Query_OutputColumn::AppendColumn( SAPDBMem_IRawAllocator& Alloc, Query_Column& Col )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::AppendColumn", QueryRewrite_Trace, 5);

    Query_Column* C = new(Alloc) Query_Column(Col);
    if ( C )
    {
        mExpression.AddNodes(C);
        return true;
    }
    return false;
}

bool Query_OutputColumn::AppendColumn( Query_Expression& Expr )
{
	mExpression.AddWithAnd(Expr);
	return true;
}

void Query_OutputColumn::IncRefCountsToChildColumns( Query_Node* Node )
{
    mExpression.IncRefCountsToChildColumns( Node );
}

void Query_OutputColumn::DelRefCountsToChildColumns( Query_Node* Node )
{
    mExpression.DelRefCountsToChildColumns( Node );
}

// ------------------------------------------------------------------------------------------------

bool Query_OutputColumn::Parse( Query_SelectNode* Father, Query_OutputColumn* /*OutCol*/, Query_Position /*Position*/, SyntaxTreeNodeNo Node, bool ComplexViewReplacement )
{
    Query_QueryGraph& QueryGraph = Father->QueryGraph();
    Query_SyntaxTree& SyntaxTree = QueryGraph.SyntaxTree();
        
    mSyntaxNode = Node;
    SyntaxTreeNodeNo N = SyntaxTree[Node].n_lo_level;

    // Referencename
    if ( (SyntaxTree[N].n_proc == 0) &&
        (SyntaxTree[N].n_symb == s_reference_name) )
    {
        QueryGraph.GetIdentifier(N, mRefName);
        N = SyntaxTree[N].n_sa_level;
    }

    SAPDB_Int2 ColErrPos = cak_qrewrite_ref_init;
    SQLMan_Identifier Schema(false), TableName(false), ColumnName(false);

    // Schema
    if ( (SyntaxTree[N].n_proc == 0) &&
        (SyntaxTree[N].n_symb == s_authid) )
    {
        if ( ColErrPos <= 0 )
            ColErrPos = SyntaxTree[N].n_pos();
        QueryGraph.GetSchemaName(N, Schema);
    }
    else
    {
        Schema.SetBlank();
    }
    // Tablename
    if ( (SyntaxTree[N].n_proc == 0) &&
        (SyntaxTree[N].n_symb == s_tablename) )
    {
        if ( ColErrPos <= 0 )
            ColErrPos = SyntaxTree[N].n_pos();
        QueryGraph.GetIdentifier(N, TableName);
        N = SyntaxTree[N].n_sa_level;
    }
    else
    {
        TableName.SetBlank();
    }
    // Columnname
    if ( (SyntaxTree[N].n_proc == 0) &&
        (SyntaxTree[N].n_symb == s_columnname) )
    {
        if ( ColErrPos <= 0 )
            ColErrPos = SyntaxTree[N].n_pos();
        QueryGraph.GetIdentifier(N, ColumnName);

        Query_Column C1 = Query_Column(N, ColErrPos, Schema, TableName, ColumnName);
        SetColType(Query_OutColType::OCTM_Column, /*SubType*/0, 1);
        return AppendColumn(Father->GetAllocator(), C1);
    }
    // Asterisk
    if ( (SyntaxTree[N].n_proc == 0) &&
        (SyntaxTree[N].n_symb == s_asterisk) )
    {
        if ( ColErrPos <= 0 )
            ColErrPos = SyntaxTree[N].n_pos();

        ColumnName.SetBlank();
        if ( TableName.IsBlank() )
        {   // select '*'
            Query_Column C2 = Query_Column(N, ColErrPos, Schema, TableName);
            C2.SetInfo(-1, cak_qrewrite_ref_multistar, cak_qrewrite_ref_multistar, ColumnName, cak_qrewrite_ref_multistar, cak_qrewrite_ref_multistar, /*IsNullable*/true);
            SetColType(Query_OutColType::OCTM_MultiAsterisk, /*SubType*/0, 0);
            AppendColumn(Father->GetAllocator(), C2);
        }
        else
        {   // select (s.)t.*
            Query_Column C3 = Query_Column(N, ColErrPos, Schema, TableName);
            C3.SetInfo(-1, cak_qrewrite_ref_star, cak_qrewrite_ref_star, ColumnName, cak_qrewrite_ref_star, cak_qrewrite_ref_star, /*IsNullable*/true);
            SetColType(Query_OutColType::OCTM_Asterisk, /*SubType*/0, 0);
            AppendColumn(Father->GetAllocator(), C3);
        }
        return true;
    }

    // without reference name
    if ( mRefName.IsBlank() && !Father->FatherNode()->IsPseudoInsertSelect() )
    {
		if ( !(SyntaxTree[N].n_proc == 0 && ( SyntaxTree[N].n_symb == s_default || SyntaxTree[N].n_symb == s_parameter_name )) )
			if ( !QueryGraph.AddExpressionRefName(N, Father->mExprCnt, mRefName) )
				return false;
    }

    // Constants and functions
    if ( (SyntaxTree[N].n_proc == 0) &&
         (     SyntaxTree[N].n_symb == s_unsigned_integer
            || SyntaxTree[N].n_symb == s_fixed
            || SyntaxTree[N].n_symb == s_float
            || SyntaxTree[N].n_symb == s_string_literal
            || SyntaxTree[N].n_symb == s_byte_string) 
            || SyntaxTree[N].n_symb == s_parameter_name
            || SyntaxTree[N].n_symb == s_null
            || SyntaxTree[N].n_symb == s_true
            || SyntaxTree[N].n_symb == s_false
            || SyntaxTree[N].n_symb == s_date
            || SyntaxTree[N].n_symb == s_time
            || SyntaxTree[N].n_symb == s_timestamp
            || SyntaxTree[N].n_symb == s_timezone
            || SyntaxTree[N].n_symb == s_now
            || SyntaxTree[N].n_symb == s_utcdiff
            || SyntaxTree[N].n_symb == s_utcdate
            || SyntaxTree[N].n_symb == s_user
            || SyntaxTree[N].n_symb == s_usergroup
            || SyntaxTree[N].n_symb == s_current_schema
            || SyntaxTree[N].n_symb == s_sysdba
            || SyntaxTree[N].n_symb == s_rowno
			|| SyntaxTree[N].n_symb == s_default
            )
    {
        // DDT : we can step out here already
        SetColType(Query_OutColType::OCTM_Expression, /*SubType*/0, 1);
        return true;
    }

    bool rc = Query_ColumnSpec::Parse(Father, this, P_SelectList, N, ComplexViewReplacement);
    if ( GetMainColType() == Query_OutColType::OCTM_Unknown )
    {
        SAPDBERR_ASSERT_STATE( GetMainColType() != Query_OutColType::OCTM_Unknown );
        SetColType(Query_OutColType::OCTM_Expression, /*SubType*/0, 1);
    }
    return rc;
}

bool Query_OutputColumn::Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy )
{
    switch ( mOutColType.MainType )
    {
    case Query_OutColType::OCTM_MultiAsterisk:
        return SemanticsMultiAsterisk(Father);
        break;
    case Query_OutColType::OCTM_Asterisk:
        return SemanticsAsterisk(Father);
        break;
    default:
        SAPDBERR_ASSERT_STATE( GetMainColType() != Query_OutColType::OCTM_Unknown );
        mColumnCount = 1;
        return Query_ColumnSpec::Semantics(Father, MayBeCorrelated, ForOrderBy);
        break;
    }
}

bool Query_OutputColumn::SemanticsMultiAsterisk( Query_SelectNode* Father )
{
    mColumnCount = Father->Semantics_OutputColumnCntForMultiAsterisk();
    return true;
}

bool Query_OutputColumn::SemanticsAsterisk( Query_SelectNode* Father )
{
    Query_Column* C = (Query_Column*)mExpression.GetRoot();
    if ( C )
    {
        Query_QueryGraph& QueryGraph = Father->QueryGraph();
        Query_SyntaxTree& SyntaxTree = QueryGraph.SyntaxTree();

        SQLMan_Identifier Schema(false), TableName(false), ColumnName;
        Query_Quantifier* Q;
        C->GetSchemaName(Schema);
        C->GetTableName(TableName);
        if ( Father->Semantics_FindTableInFromPart(Schema, TableName, Q) )
        {
			mColumnCount = Q->OutputColumns();
			SyntaxTreeNodeNo FromTabNode = Q->SyntaxNode();

            if ( !SyntaxTree.ChangeColumnNameToColumnId(C->GetSyntaxNode(), FromTabNode, cak_qrewrite_ref_star, C->ErrorPos()) )
                return false;

            C->SetInfo(-1, Q->NodeID(), FromTabNode, ColumnName, cak_qrewrite_ref_star, cak_qrewrite_ref_star, /*IsNullable*/true);
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------------------------------------------------
