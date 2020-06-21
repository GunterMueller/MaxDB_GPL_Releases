/*!***************************************************************************

    @file     Query_Column.hpp
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

#ifndef Query_Column_H
#define Query_Column_H

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
    @class          Query_Column
    @brief          ...
    @copydoc        Query_ExpressionNode
 */
class Query_Column : public Query_ExpressionNode
{
private:    // private copyconstuctor used by CreateDuplicate
    //Query_Column( const Query_Column& C, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap );
public:
    /// ...
	enum ColumnState  { cUnidentified, cMayBeCorrelated, cIdentified };

    //Query_Column( void )
    //    : Query_ExpressionNode(cak_qrewrite_ref_init, Exp_Unknown)
    //    , mColName()
    //    , mNodeID(cak_qrewrite_ref_init)
    //    , mQuantifierID(cak_qrewrite_ref_init)
    //    , mLogicalColumnIndex(0)
    //    , mQuantifierNode(cak_qrewrite_ref_init)
	   // , mExtColNo(0)
	   // , mStatus(cUnidentified)
    //    , mIsNullable(true)
    //    , mInAggr(false)
    //{
    //}

    /// ...
    Query_Column( SyntaxTreeNodeNo Node )
        : Query_ExpressionNode(Node)
        , mType(Exp_Column)
        , mSchemaName(false)
        , mTableName(false)
        , mColName(false)
        , mNodeID(cak_qrewrite_ref_init)
        , mQuantifierID(cak_qrewrite_ref_init)
        , mLogicalColumnIndex(0)
        , mQuantifierNode(cak_qrewrite_ref_init)
	    , mExtColNo(0)
	    , mStatus(cUnidentified)
        , mIsNullable(true)
        , mInAggr(false)
        , mIsAsterisk(false)
    {
    }

    /// ...
    Query_Column( SyntaxTreeNodeNo Node, SAPDB_Int2 ErrPos, SQLMan_Identifier& S, SQLMan_Identifier&T )
        : Query_ExpressionNode(Node)
        , mType(Exp_Column)
        , mSchemaName(S)
        , mTableName(T)
        , mColName()
        , mErrPos(ErrPos)
        , mNodeID(cak_qrewrite_ref_init)
        , mQuantifierID(cak_qrewrite_ref_init)
        , mLogicalColumnIndex(0)
        , mQuantifierNode(cak_qrewrite_ref_init)
	    , mExtColNo(0)
	    , mStatus(cUnidentified)
        , mIsNullable(true)
        , mInAggr(false)
        , mIsAsterisk(true)
    {
    }

    /// ...
    Query_Column( SyntaxTreeNodeNo Node, SAPDB_Int2 ErrPos, SQLMan_Identifier& S, SQLMan_Identifier&T, SQLMan_Identifier& C )
        : Query_ExpressionNode(Node)
        , mType(Exp_Column)
        , mSchemaName(S)
        , mTableName(T)
        , mColName(C)
        , mErrPos(ErrPos)
        , mNodeID(cak_qrewrite_ref_init)
        , mQuantifierID(cak_qrewrite_ref_init)
        , mLogicalColumnIndex(0)
        , mQuantifierNode(cak_qrewrite_ref_init)
	    , mExtColNo(0)
	    , mStatus(cUnidentified)
        , mIsNullable(true)
        , mInAggr(false)
        , mIsAsterisk(false)
    {
    }

    /// ...
    Query_Column( const Query_Column& C )
        : Query_ExpressionNode(C) 
        , mType(C.mType)
        , mSchemaName(C.mSchemaName)
        , mTableName(C.mTableName)
        , mColName(C.mColName)
		, mErrPos(C.mErrPos)
        , mNodeID(C.mNodeID)
        , mQuantifierID(C.mQuantifierID)
	    , mLogicalColumnIndex(C.mLogicalColumnIndex)
        , mQuantifierNode(C.mQuantifierNode)
	    , mExtColNo(C.mExtColNo)
	    , mStatus(C.mStatus)
        , mIsNullable(C.mIsNullable)
        , mInAggr(C.mInAggr)
        , mIsAsterisk(C.mIsAsterisk)
    {
    }

    virtual bool Semantics( Query_SelectNode* Father, bool MayBeCorrelated, bool ForOrderBy );
    virtual bool SemanticsCorrelation( Query_SelectNode* SubQuery, bool& Unidentified );
	virtual bool CheckCorrelatedColumnsForExistToIn( Query_SelectNode* Father );

    virtual Query_ExpressionNode* CreateDuplicate( SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
    virtual bool ExchangeQuantifierID( SAPDB_Int2 OldID, SAPDB_Int2 NewID );
    virtual bool ExchangeNodeID( SAPDB_Int2 OldID, SAPDB_Int2 NewID, SAPDB_Int2& Cnt );
    virtual bool MappFromOuterToInnerSelect( Query_SelectNode* InnerSelectNode, Query_MappingInfo* MappingInfo );
    virtual bool RenewSemanticsDueToColumnPrivs( Query_Node* NewNode );
    virtual bool RenewSemanticsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Query_Position Pos );
    virtual void IncRefCountsToChildColumns( Query_Node* Node );
    virtual void DelRefCountsToChildColumns( Query_Node* Node );
    virtual SAPDB_Int2 GetNodeID( void ) const           { return mNodeID; }
    virtual SAPDB_Int2 GetQuantifierID( void ) const     { return mQuantifierID; }
    virtual void SetInAggr( bool InAggr )                { mInAggr = InAggr; } 

	/// ...
    inline  bool InAggr( void ) const                    { return mInAggr;   }

    /// ...
    inline  const SQLMan_Identifier& GetColumnName( void ) const { return mColName; }
    /// ...
    inline  void GetColumnName( SQLMan_Identifier& C ) const { C = mColName; }
    /// ...
    inline  void GetSchemaName( SQLMan_Identifier& S ) const { S = mSchemaName; }
    /// ...
    inline  void GetTableName( SQLMan_Identifier& T ) const { T = mTableName; }

    /// ...
    inline SyntaxTreeNodeNo GetQuantifierSyntaxNode( void ) const   { return mQuantifierNode;   }


    /// ...
    inline SAPDB_Int2 GetLogicalColumnIndex( void ) const { return mLogicalColumnIndex;  }
    /// ...
    inline SAPDB_Int2 GetExtColNo( void ) const			{ return mExtColNo; }

    /// ...
	inline ColumnState GetStatus( void ) const          { return mStatus; }
    /// ...
    inline bool        IsNullable( void ) const         { return mIsNullable; }
    /// ...
    inline void        SetNullable( bool n )            { mIsNullable = n; }

    /// ...
	inline bool IsUnidentified( void )	const			{ return mStatus != cIdentified; }
    /// ...
	inline bool MayBeCorrelated( void ) const			{ return mStatus == cMayBeCorrelated; }

    virtual Query_ExpType    GetExpType( void ) const   { return mType;         }

    /// ...
	inline void SetStatus( ColumnState Status )
	{
		mStatus = Status;
	}

    /// ...
    inline void SetInfo( SAPDB_Int2 NodeID, SAPDB_Int2 QID, SyntaxTreeNodeNo QNode, const SQLMan_Identifier& CName, SAPDB_Int2 LogColIdx, SAPDB_Int2 ExtColNo, bool IsNullable )
    {
        mColName            = CName;
        mNodeID             = NodeID;	// QueryNode->mID
        mQuantifierID       = QID;
        mQuantifierNode     = QNode;    // QueryNode->mAPNode
        ChangeIndex(LogColIdx, ExtColNo);
		SetStatus(cIdentified);
        mIsNullable         = IsNullable;
    }

    /// ...
	inline void ChangeIndex( SAPDB_Int2 NewLogColumnIndex, SAPDB_Int2 NewExtColNo )
	{
		mLogicalColumnIndex = NewLogColumnIndex;
		mExtColNo           = NewExtColNo;
	}

	/// ...
    inline SAPDB_Int2&   ErrorPos( void )    { return mErrPos; }

private:

    /// ...
    SAPDB_Int2          mNodeID;        /// QueryNode->mID. indicates if it is a correlation
                                        // -1 : the same node contains quantifier and column
                                        // >0 : ID of that node that contains the quantifier (correlated columns only)
    /// QueryNode->mID
    SAPDB_Int2          mQuantifierID;  
    /// QueryNode->mAPNode
    SyntaxTreeNodeNo    mQuantifierNode;
    /// Column number within the Quantifier           
    SAPDB_Int2          mLogicalColumnIndex;   
    /// ExtColNo for Pascal           
    SAPDB_Int2          mExtColNo;             
    /// ...
    SQLMan_Identifier   mSchemaName;  
    SQLMan_Identifier   mTableName;  
    SQLMan_Identifier   mColName;       
    SAPDB_Int2          mErrPos;
    /// ...
	ColumnState			mStatus;	    
    /// Column may be NULL
    bool                mIsNullable;    
    /// Column is argument of some aggregate function
    bool                mInAggr;       
    bool                mIsAsterisk;

    Query_ExpType       mType;   
};

// ------------------------------------------------------------------------------------------------

#endif