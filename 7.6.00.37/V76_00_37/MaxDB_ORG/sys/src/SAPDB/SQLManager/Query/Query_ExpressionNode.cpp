/*!***************************************************************************

    @file     Query_ExpressionNode.cpp
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

#include "SAPDB/SQLManager/Query/Query_ExpressionNode.hpp"
#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Column.hpp"
#include "SAPDB/SQLManager/Query/Query_Operator.hpp"
#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"

// ------------------------------------------------------------------------------------------------

Query_ExpressionNode::Query_ExpressionNode( void )
    : mSyntaxNode(cak_qrewrite_ref_init)
    , mFather(0)
    , mNext(0)
    , mPos(0)
    , mLen(0)
{
}

Query_ExpressionNode::Query_ExpressionNode( SyntaxTreeNodeNo Node )
    : mSyntaxNode(Node)
    , mFather(0)
    , mNext(0)
    , mPos( 0 )
    , mLen( 0 )
{
}

Query_ExpressionNode::Query_ExpressionNode( SyntaxTreeNodeNo Node, SAPDB_Int2 Pos, SAPDB_Int2 Len )
    : mSyntaxNode(Node)
    , mFather(0)
    , mNext(0)
    , mPos( Pos )
    , mLen( Len )
{
}


Query_ExpressionNode::Query_ExpressionNode( const Query_ExpressionNode& C )
    : mSyntaxNode(C.mSyntaxNode)
    , mFather(C.mFather)
    , mNext(C.mNext)
    , mPos(C.mPos)
    , mLen(C.mLen)
{
}

Query_ExpressionNode::Query_ExpressionNode( const Query_ExpressionNode& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap )
    : mSyntaxNode(C.mSyntaxNode)
    , mFather(C.mFather)
    , mNext(C.mNext)
    , mPos(C.mPos)
    , mLen(C.mLen)
{
}

Query_ExpressionNode::~Query_ExpressionNode( void )
{

}

void Query_ExpressionNode::Destroy( SAPDBMem_IRawAllocator& Alloc )
{
    if ( mNext )
    {
        mNext->Destroy(Alloc);
        destroy(mNext, Alloc);
        mNext = 0;
    }
}

bool Query_ExpressionNode::Parse( Query_Node* Father, Query_Position Position, SyntaxTreeNodeNo Node, bool ComplexViewReplacement )
{
    Query_SyntaxTree& SyntaxTree = Father->QueryGraph().SyntaxTree();
    Query_Node* Query = 0;
    switch ( SyntaxTree[Node].n_proc )
    {
    case no_proc:
        switch ( SyntaxTree[Node].n_symb )
        {
		case s_authid:

        case s_tablename:

        case s_columnname:

            break;
        default:

            break;
        }
        break;
    case a63:
        switch ( SyntaxTree[Node].n_subproc() )
        {
        case cak_x_start_union:
            Query = Father->AddQuery(Node, Position, ComplexViewReplacement);
            if ( Query )
                return Query->Parse(ComplexViewReplacement);
            break;
        default:
            break;
        }
        break;
    case a63query_spec:
        Query = Father->AddQuery(Node, Position, ComplexViewReplacement);
        if ( Query )
            return Query->Parse(ComplexViewReplacement);
                
        break;
    case a64:
        //switch ( SyntaxTree[Node].n_subproc() )
        //{
        //default:
        //    break;
        //}
        break;
    }
    return true;
}

// ------------------------------------------------------------------------------------------------
