/*!***************************************************************************

    @file     Query_Expression.hpp
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

#ifndef Query_Expression_H
#define Query_Expression_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_ExpressionNode.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

class Query_NodeMapper;
class Query_SyntaxTree;
class SAPDBMem_IRawAllocator;
class Query_SelectNode;
class Query_Node;
class Query_OutputColumn;

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_Expression
    @brief          ...
 */
class Query_Expression
{
private:    // private copyconstuctor used by CreateDuplicate
//    Query_Expression( const Query_Expression& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap );
public:
    /// ...
    Query_Expression( void );
    /// ...
    ~Query_Expression( void );

    /// ...
    void Destroy( SAPDBMem_IRawAllocator& Alloc );
    
    /// ...
    bool DelNode( Query_ExpressionNode*& N, SAPDBMem_IRawAllocator& Alloc )
    {
        if ( N == mRoot )
        {
            mRoot = N->mNext;
            N->mNext = 0;
            destroy(N, Alloc);
            N = 0;
        }
        else
        {
            Query_ExpressionNode* Act = mRoot;
            Query_ExpressionNode* Last = 0;

            while ( Act && Act != N )
            {
                Last = Act;
                Act  = Act->mNext;
            }
            if ( Act == N )
            {
                if ( Last )
                {
                    Last->mNext = Act->mNext;
                    Act->mNext = 0;
                    destroy(Act, Alloc);
                    N = 0;
                }
                else
                {
                    SAPDBERR_ASSERT_STATE( 0 );
                    return false;
                }
            }
        }
        return true;
    }

    /// ...
    bool AddNodes( Query_ExpressionNode* N )
    {
        Query_ExpressionNode* L = mRoot;
        if ( L )
        {
            while ( L->mNext )
            {
                L = L->mNext;
            }
            L->mNext = N;
        }
        else
        {
            mRoot = N;
        }
        return true;
    }

    /// ...
    bool AddWithAnd( Query_Expression& Expr )
    {
        AddNodes(Expr.mRoot);
        Expr.mRoot = 0;
        return true;
    }

    /// ...
    bool IsEmpty( void ) const
    {
        return mRoot==0;
    }

    /// ...
    void SetRoot( Query_ExpressionNode* N )
    {
        mRoot = N;
    }

    /// ...
    Query_ExpressionNode* GetRoot( void ) const
    {
        return mRoot;
    }

    /// ...
    void SetInAggr( void );

    /// ...
    Query_Expression* CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
    /// ...
    Query_Expression  CreateDuplicate2( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /// ...
    bool ExchangeQuantifierID( SAPDB_Int2 OldID, SAPDB_Int2 NewID )
    {
        if ( mRoot && OldID != NewID )
            return mRoot->ExchangeQuantifierID(OldID, NewID);
        return true;
    }

    /// ...
    bool ExchangeNodeID( SAPDB_Int2 OldID, SAPDB_Int2 NewID, SAPDB_Int2& Cnt )
    {
        if ( mRoot && OldID != NewID )
            return mRoot->ExchangeNodeID(OldID, NewID, Cnt);
        return true;
    }

    /// ...
    bool MappFromOuterToInnerSelect( Query_SelectNode* InnerSelectNode, Query_MappingInfo* MappingInfo )
    {
        if ( mRoot )
		{
            if ( !mRoot->MappFromOuterToInnerSelect(InnerSelectNode, MappingInfo) )
				return false;
			DeleteEmptyColumnNodes(InnerSelectNode);
		}
        return true;
    }

    /// ...
	void DeleteEmptyColumnNodes( Query_SelectNode* InnerSelectNode );

    /// ...
    void IncRefCountsToChildColumns( Query_Node* Node )
    {
        if ( mRoot )
            mRoot->IncRefCountsToChildColumns(Node);
    }

    /// ...
    void DelRefCountsToChildColumns( Query_Node* Node )
    {
        if ( mRoot )
            mRoot->DelRefCountsToChildColumns(Node);
    }

    /// ...
    bool Parse( Query_SelectNode* Father, Query_OutputColumn* OutCol, Query_Position Position, SyntaxTreeNodeNo Node, bool InAggr, bool ComplexViewReplacement );
    /// ...
    bool ParseCol( Query_SelectNode* Father, Query_OutputColumn* OutCol, SyntaxTreeNodeNo& Node, bool InAggr );
    /// ...
    bool Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy );
    /// ...
    bool SemanticsCorrelation( Query_SelectNode* SubQuery, bool& Unidentified );
    /// ...
    bool CheckCorrelatedColumnsForExistToIn( Query_SelectNode* Father );

private:
    /// ...
    Query_ExpressionNode*   mRoot;
};

// ------------------------------------------------------------------------------------------------

#endif