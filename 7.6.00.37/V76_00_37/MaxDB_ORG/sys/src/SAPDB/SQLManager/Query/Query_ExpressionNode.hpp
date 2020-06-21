/*!***************************************************************************

    @file     Query_ExpressionNode.hpp
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

#ifndef Query_ExpressionNode_H
#define Query_ExpressionNode_H

// ------------------------------------------------------------------------------------------------

//#include "SAPDB/SQLManager/SQLMan_Types.hpp"

#include "SAPDB/SQLManager/Query/Query_Types.hpp"
//#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

class Query_SyntaxTree;
class Query_NodeMapper;
class Query_QuantifierMapper;
class Query_Node;
class Query_SelectNode;
class Query_Quantifier;
class Query_OutColType;

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_ExpressionNode
    @brief          ...
 */
class Query_ExpressionNode
{
protected:    // private copyconstuctor used by CreateDuplicate
    /// ...
    Query_ExpressionNode( const Query_ExpressionNode& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap );
public:
    /// ...
    Query_ExpressionNode( void );
    /// ...
    Query_ExpressionNode( SyntaxTreeNodeNo Node );
    /// ...
    Query_ExpressionNode( SyntaxTreeNodeNo Node, SAPDB_Int2 Pos, SAPDB_Int2 Len );
    /// ...
	Query_ExpressionNode( const Query_ExpressionNode& C );

    /// ...
    virtual ~Query_ExpressionNode( void );
    /// ...
    virtual void Destroy( SAPDBMem_IRawAllocator& Alloc );

    /// ...
    virtual bool Parse( Query_Node* Father, Query_Position Position, SyntaxTreeNodeNo Node, bool ComplexViewReplacement );
    /// ...
    virtual bool Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy ) = 0;
    /// ...
    virtual bool SemanticsCorrelation( Query_SelectNode* SubQuery, bool& Unidentified ) = 0;
    /// ...
	virtual bool CheckCorrelatedColumnsForExistToIn( Query_SelectNode* Father ) = 0;

    /// ...
    virtual Query_ExpressionNode* CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap ) = 0;
    /// ...
    virtual bool ExchangeQuantifierID( SAPDB_Int2 OldID, SAPDB_Int2 NewID ) = 0;
    /// ...
    virtual bool ExchangeNodeID( SAPDB_Int2 OldID, SAPDB_Int2 NewID, SAPDB_Int2& Cnt ) = 0;
    /// ...
    virtual bool MappFromOuterToInnerSelect( Query_SelectNode* InnerSelectNode, Query_MappingInfo* MappingInfo ) = 0;
    /// ...
    virtual bool RenewSemanticsDueToColumnPrivs( Query_Node* NewNode ) = 0;
    /// ...
    virtual bool RenewSemanticsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Query_Position Pos ) = 0;
    /// ...
    virtual void IncRefCountsToChildColumns( Query_Node* Node ) = 0;
    /// ...
    virtual void DelRefCountsToChildColumns( Query_Node* Node ) = 0;
    /// ...
    virtual SAPDB_Int2 GetNodeID( void ) const = 0;
    /// ...
	virtual SAPDB_Int2 GetQuantifierID( void ) const = 0;

    /// ...
    virtual void SetInAggr( bool InAggr ) = 0;

    /// ...
    inline SyntaxTreeNodeNo GetSyntaxNode( void ) const         { return mSyntaxNode;   }
    /// ...
    inline void SetSyntaxNode( SyntaxTreeNodeNo SyntaxNode )    { mSyntaxNode = SyntaxNode; }

    /// ...
    virtual Query_ExpType    GetExpType( void ) const            { return Exp_Unknown;         }

    /// ...
    Query_ExpressionNode*   mNext;

    /// syntax node : a60/2 for OutputColumns, else 0/s_columnname /s_columnid
    SyntaxTreeNodeNo    mSyntaxNode;    
    

protected:
	/// ...
    Query_ExpressionNode*   mFather;  
	/// ...
    SAPDB_Int2              mPos;     
	/// ...
    SAPDB_Int2              mLen;     


};

// ------------------------------------------------------------------------------------------------

#endif