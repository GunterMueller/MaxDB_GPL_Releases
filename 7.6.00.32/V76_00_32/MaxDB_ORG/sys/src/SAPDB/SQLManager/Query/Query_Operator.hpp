/*!***************************************************************************

    @file     Query_Operator.hpp
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

#ifndef Query_Operator_H
#define Query_Operator_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/SQLMan_Types.hpp"

#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"
#include "SAPDB/SQLManager/Query/Query_ExpressionNode.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

class Query_Node;
class Query_OutColType;

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_Operator
    @brief          ...
    @copydoc        Query_ExpressionNode
 */
class Query_Operator : public Query_ExpressionNode
{
public:
    /// ...
    Query_Operator( tak_procs Proc, tsp00_Int2 Subproc, tak_sc_symbol Symb, SAPDB_Int2 Pos, SAPDB_Int2 Len )
        : Query_ExpressionNode(0, Pos, Len) // DDT 
        , mProc(Proc)
        , mSubproc(Subproc)
        , mSymb(Symb)
        , mSame(0)
        , mLow(0)
    {
    }

    /// ...
    Query_Operator( const Query_Operator& C )
        : Query_ExpressionNode(C) 
        , mProc(C.mProc)
        , mSubproc(C.mSubproc)
        , mSymb(C.mSymb)
        , mSame(C.mSame)
        , mLow(C.mLow)
    {
    }

    virtual Query_Operator* CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
    virtual bool ExchangeQuantifierID( SAPDB_Int2 OldID, SAPDB_Int2 NewID );
    virtual bool ExchangeNodeID( SAPDB_Int2 OldID, SAPDB_Int2 NewID, SAPDB_Int2& Cnt );
    virtual bool MappFromOuterToInnerSelect( Query_SelectNode* InnerSelectNode, Query_MappingInfo* MappingInfo );
    virtual bool IdentityCorrelatedColumn( Query_SelectNode* TopNode, Query_SelectNode* SubQuery, bool& Unidentified );
    virtual bool RenewSemanticsDueToColumnPrivs( Query_Node* NewNode );
    virtual bool RenewSemanticsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Query_Position Pos );
    virtual void IncRefCountsToChildColumns( Query_Node* Node );
    virtual void DelRefCountsToChildColumns( Query_Node* Node );

private:
    Query_ExpressionNode*   mSame;              // ...
    Query_ExpressionNode*   mLow;               // ...
    tak_procs               mProc;              // ...
    tak_sc_symbol           mSymb;              // ...
    tsp00_Int2              mSubproc;           // ...
};


// ------------------------------------------------------------------------------------------------

#endif