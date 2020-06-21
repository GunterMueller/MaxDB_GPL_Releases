/*!***************************************************************************

    @file     Query_NodeSet.hpp
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

#ifndef Query_NodeSet_H
#define Query_NodeSet_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/Query/Query_Node.hpp"

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_SetNode
    @brief          Node that represents a SET; Base class of Query_UnionNode, Query_IntersectNode or Query_DifferenceNode
    @copydoc        Query_Node
 */
class Query_SetNode : public Query_Node
{
protected:    // private copyconstuctor used by CreateDuplicate
    /*!
        @brief  Protected constructor called by CreateDuplicates
        @param  N       [in]
        @param  Father  [in]
        @param  Q       [in]
        @param  mpMap   [in]
        @param  mpQMap  [in]
    */
    Query_SetNode( const Query_SetNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
public:
    /*!
        @brief  Constructor
        @param  Context    [in]
        @param  QueryGraph [in]
        @param  Position   [in]
        @param  APNode     [in]
        @param  All        [in]
    */    
    Query_SetNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, Query_Position Position, SyntaxTreeNodeNo APNode, bool All );

    virtual bool Parse( bool ComplexViewReplacement );
    virtual bool Semantics ( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement );
    virtual bool PullDownForMerge( 
        Container_Vector<Query_OutputColumn*>*& pOutputColumns, SyntaxTreeNodeNo SelectList_AP,
                                                                SyntaxTreeNodeNo SelectInto_AP,
        Query_Expression& SearchCondition,                      SyntaxTreeNodeNo SearchCond_AP,
        Query_Expression& HavingColumns,                        SyntaxTreeNodeNo Having_AP,
        Container_Vector<Query_ColumnSpec*>*& pGroupByColumns,  SyntaxTreeNodeNo GroupBy_AP,
        Container_Vector<Query_ColumnSpec*>*& pOrderByColumns,  SyntaxTreeNodeNo OrderBy_AP,
        Query_Expression& UpdateColumns,                        SyntaxTreeNodeNo Update_AP,
        Container_Vector<Query_Quantifier*>& Quantifier,        SyntaxTreeNodeNo FromPart_AP,
        SyntaxTreeNodeNo LimitStart, 
		SyntaxTreeNodeNo LockOptionStart,
        bool ForReuse, bool OuterJoinPredicates,
        SAPDB_Int2 QID,
//        SAPDB_Int2 NID,
        Query_MergeType MergeType,
		bool AggrInSelectList,
        bool MustCreateCopy
        );
    virtual bool PullDownSearchCond( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, SAPDB_Int2 QID, bool MustCreateCopy );
    virtual void RemoveAllFromSet( void );

	virtual	bool PullDownProjectionPossible( void );
    virtual bool PullDownProjection( void );
    virtual bool ProjectionPulledDown( void );

	virtual bool RenewSemanticsForColumnsDueToColumnPrivs( Query_Node* NewNode );

	virtual bool CreateColRefNamesAndResolveAsterisk( SyntaxTreeNodeNo& RefNameNodes ) ;
    virtual bool RenewSemanticsForColumnDueToColumnPrivs( Query_Column* C );
//    virtual bool RenewSemanticsForColumnOnMergeForCorrCols( Query_Column* C );
    virtual bool SetTempFileNameAndCounter( SAPDB_Int1 Level, SAPDB_Int2& Cnt, bool FatherIsUnion, SAPDB_Int2& FromSelCnt, SAPDB_Int2& ParsCnt, bool& ContainsCorrelatedSubquery, bool& ContainsFromSelectInSubquery, SAPDB_Int2& MaxSubqueryLevel );

    virtual bool IsSelectOrSet( void ) { return true; }
    virtual bool IsSet( void )         { return true; }

    virtual SAPDB_Int2 OutputColumns( void );
    virtual bool GetOutputColumnInternType( SAPDB_Int2 LogColIndex, Query_OutColType& Type, SyntaxTreeNodeNo& SyntaxNode );
	virtual SAPDB_Int2 GetOutputColumnSize( SAPDB_Int2 LogColIndex );
    virtual bool GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable );
    virtual bool GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ColumnIndex );
    virtual bool CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    virtual bool CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    virtual bool CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table );
    virtual void GetIdentificationNames(SQLMan_Identifier& Schema, SQLMan_Identifier& Table);
	virtual bool HasReferenceName( void );

    virtual bool IncRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo ) { return true; } // just to make some compilers happy...
    virtual bool DecRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo ) { return true; } // just to make some compilers happy...
    virtual bool AddRefCountForOutputColumn( SAPDB_Int2 i, SAPDB_Int2 Count );
    virtual bool IncRefCountForOutputColumn( SAPDB_Int2 i );
    virtual bool DecRefCountForOutputColumn( SAPDB_Int2 i );

    virtual bool AddGivenSequenceNode( void );

    virtual bool OuterJoin( void );
    virtual bool Aggregates( void );
	virtual bool AggregatesInOutput( void );
    virtual bool HasHaving( void );
    virtual bool HasGroupBy( void );
    virtual void AddJoinType( Query_JoinType JT );

    virtual bool                IsDeclareCursor( void );
    virtual void                SetDeclareCursor( bool Decl );
    virtual void                SetA631( void );

    /// ...
    bool& All( void )     { return mAll; }

    /// ...
    bool CheckForIntOrExcept2Exist( void );

    virtual void DetermineAPNode( void );

    virtual SyntaxTreeNodeNo GetInternAPNode( void )
    {
        return mInternAPNode;
    }

    virtual void SetInternAPNode( SyntaxTreeNodeNo NodeNo )
    {
        mInternAPNode = NodeNo;
    }

    virtual void SetSubqueryType( Query_SubQueryType SubQueryType ) 
    { 
        mSubQueryType = SubQueryType;
    }

    virtual Query_SubQueryType GetSubQueryType( void )
    {
        return mSubQueryType;
    }

	/// ...
    virtual Query_Position PositionForParse( void ) = 0;

protected:

    /// ...
    SQLMan_Identifier   mReferenceName; /// ...
    Query_SubQueryType  mSubQueryType;  /// ...    
    SyntaxTreeNodeNo    mInternAPNode;  /// ...
    bool				mAll;           /// ...
    bool                mFirstFlg;      /// ...
    bool                mProjectionPulledDown; /// ...
};

// ------------------------------------------------------------------------------------------------

#endif