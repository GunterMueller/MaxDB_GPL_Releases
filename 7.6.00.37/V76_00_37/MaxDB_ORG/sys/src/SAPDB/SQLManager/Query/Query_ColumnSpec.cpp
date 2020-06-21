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
#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"

// ------------------------------------------------------------------------------------------------

Query_ColumnSpec::Query_ColumnSpec( void )
    : mExpression()
    , mSyntaxNode(cak_qrewrite_ref_init)
{
}

Query_ColumnSpec::Query_ColumnSpec( SyntaxTreeNodeNo Node )
    : mExpression()
    , mSyntaxNode(Node)
{
}

Query_ColumnSpec::Query_ColumnSpec( const Query_ColumnSpec& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
    : mExpression(C.mExpression)
    , mSyntaxNode(C.mSyntaxNode)
{
    if ( mpMap )
        SyntaxTree.MapNode(mSyntaxNode, mpMap); 
}

Query_ColumnSpec::~Query_ColumnSpec( void )
{
}

void Query_ColumnSpec::Destroy( SAPDBMem_IRawAllocator& Alloc )
{
    mExpression.Destroy(Alloc);
}

Query_ColumnSpec* Query_ColumnSpec::CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap )
{
    Query_ColumnSpec* N = new(Alloc) Query_ColumnSpec(*this, SyntaxTree, mpMap, mpQMap);
    if ( N )
    {
        SyntaxTree.MapNode(mSyntaxNode, mpMap);
        N->mExpression = mExpression.CreateDuplicate2(Alloc, SyntaxTree, mpMap, mpQMap);
    }
    return N;
}

// ------------------------------------------------------------------------------------------------

bool Query_ColumnSpec::Parse( Query_SelectNode* Father, Query_OutputColumn* OutCol, Query_Position Position, SyntaxTreeNodeNo Node, bool ComplexViewReplacement )
{
    return mExpression.Parse(Father, OutCol, Position, Node, false, ComplexViewReplacement);
}

bool Query_ColumnSpec::Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy )
{
    return mExpression.Semantics(Father, MayBeCorrelated, ForOrderBy);
}

bool Query_ColumnSpec::CheckCorrelatedColumnsForExistToIn( Query_SelectNode* Father )
{
    return mExpression.CheckCorrelatedColumnsForExistToIn(Father);
}

// ------------------------------------------------------------------------------------------------
