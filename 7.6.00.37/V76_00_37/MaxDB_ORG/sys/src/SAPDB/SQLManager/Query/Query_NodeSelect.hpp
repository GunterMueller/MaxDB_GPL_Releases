/*!***************************************************************************

    @file     Query_NodeSelect.hpp
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

#ifndef Query_NodeSelect_H
#define Query_NodeSelect_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/Query/Query_Node.hpp"
#include "SAPDB/SQLManager/Query/Query_Expression.hpp"

// ------------------------------------------------------------------------------------------------

class Query_OutputColumn;
class Query_SetNode;

// ------------------------------------------------------------------------------------------------
/*!
    @class          Query_SelectNode
    @brief          Node that represents a SELECT
    @copydoc        Query_Node
 */
class Query_SelectNode : public Query_Node
{
public:

    // DDT : TODO !!!

    //class Query_SelectList
    //{
    //public:
    //    Query_SelectList( )
    //        
    //private:
    //    Container_Vector<Query_OutputColumn*>*  mpOutputColumns;            
    //    SAPDB_Int2                              mOutputColumnCnt;           

    //};

    //class Query_ColumnSpecList
    //{
    //public:

    //private:
    //    Container_Vector<Query_ColumnSpec*>*    mpOrderByColumns;           
    //};


protected:    // private copyconstuctor used by CreateDuplicate
    /*!
        @brief  Protected constructor called by CreateDuplicates
        @param  N       [in]
        @param  Father  [in]
        @param  Q       [in]
        @param  mpMap   [in]
        @param  mpQMap  [in]
    */
    Query_SelectNode( const Query_SelectNode& N, Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
    /// ...
    Container_Vector<Query_OutputColumn*>* CreateDuplicateOutputColumns( Container_Vector<Query_OutputColumn*>* OldList, SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
    /// ...
    Container_Vector<Query_ColumnSpec*>* CreateDuplicateColumnSpec( Container_Vector<Query_ColumnSpec*>* OldList, SAPDBMem_IRawAllocator& Alloc, Query_SyntaxTree& SyntaxTree, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );
    
    /// ...
	void ExchangeNodeIDInOutputColumns( Container_Vector<Query_OutputColumn*>*& pOutCols, SAPDB_Int2 OldID, SAPDB_Int2 NewID );
    /// ...
    void ExchangeQuantifierIDInOutputColumns( Container_Vector<Query_OutputColumn*>*& pOutCols, SAPDB_Int2 OldID, SAPDB_Int2 NewID );
    /// ...
    void ExchangeQuantifierIDInColumnSpec( Container_Vector<Query_ColumnSpec*>*& pColSpec, SAPDB_Int2 OldID, SAPDB_Int2 NewID );
    
    /// ...
    void DestroyOutputColumns( void );
    /// ...
    void ClearOutputColumns( void );
	/// ...
	void DestroyOneOutputColumn( Query_OutputColumn* Col, SAPDB_Int2 n, bool DecRefCount );
    /// ...
    void DestroyColumnSpec( Container_Vector<Query_ColumnSpec*>*& pColSpec );

public:
    virtual Query_Node* CreateDuplicate( Query_Node* Father, Query_Quantifier* Q, Query_NodeMapper* mpMap, Query_QuantifierMapper* mpQMap );

    /*!
        @brief  Constructor
        @param  Context    [in]
        @param  QueryGraph [in]
        @param  Position   [in]
        @param  APNode     [in]
        @param  All        [in]
        @param  IsDeclareCursor [in]
    */     

    Query_SelectNode( Query_ManipulationContext& Context, Query_QueryGraph& QueryGraph, 
                      Query_Position Position, SyntaxTreeNodeNo APNode, bool All, bool IsDeclareCursor ); 

    /// ...
    virtual ~Query_SelectNode( void );

    virtual NodeType GetNodeType( void )	        { return Select; }
    virtual Query_JoinType GetJoinType( void )		{ return mJoinType; }
    virtual bool OuterJoin( void )					{ return  ((mJoinType==JT_Unknown) || (mJoinType&JT_Outer)); }
    virtual void AddJoinType( Query_JoinType JT )	
    { 
        mJoinType |= JT; 
        if ( mJoinType!=JT_NoJoin )
            mJoinType &= ~JT_NoJoin;
    }
    virtual void SetForReuse( void )				{ mForReuse = true; }
	virtual void SetGivenSequence( void )			{ mGivenSequence = true; }
	virtual bool HasGivenSequence( void )			{ return mGivenSequence; }
    virtual bool AddGivenSequenceNode( void );

    virtual bool Aggregates( void )					{ return mAggregatesInOutput ||  mAggregatesNotInOutput || mCorrelatedAndAggregatedColumnsRefersToThis>0; }
    virtual bool AggregatesInOutput( void )         { return mAggregatesInOutput; }
    virtual bool AggregatesInCorrSubQuery( void )	{ return mCorrelatedAndAggregatedColumnsRefersToThis>0;	} 
    /// ...
    void SetAggregatesInOutput( void )              { mAggregatesInOutput = true; }
    /// ...
    void SetAggregatesNotInOutput( void )           { mAggregatesNotInOutput = true; }

    virtual void PrintToTrace( Kernel_VTrace& VTrace );
    virtual bool Parse( bool ComplexViewReplacement );

    /// ...
    virtual bool Semantics( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool ComplexViewReplacement );  
    /// ...
    SAPDB_Int2 Semantics_FindColumnInFromPart( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    /// ...
    bool       Semantics_FindTableInFromPart(const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, Query_Quantifier*& Q );
    /// ...
    SAPDB_Int2 Semantics_OutputColumnCntForMultiAsterisk( void );
    /// ...
	SAPDB_Int2 Semantics_FindOrderColumnInSelectList( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, SAPDB_Int2& OutputCol, bool& IsNullable, bool TopSelect );
    /// ...
    bool Semantics_Correlation( void );

    virtual bool AppendColumn( Query_Expression& pExpression, Query_Column Col );
    virtual bool AppendOutputColumn ( SyntaxTreeNodeNo Node );
    virtual SAPDB_Int2 OutputColumns( void );
    /// ...
            bool OutputColumnIsNullable( SAPDB_Int2 LogColIndex );
			bool OutputContainsNullableColumns( void );

	Query_OutputColumn* GetOutputColumnPointer( SAPDB_Int2 LogColIndex );
	virtual bool GetOutputColumnInternType( SAPDB_Int2 LogColIndex, Query_OutColType& Type, SyntaxTreeNodeNo& SyntaxNode );
	virtual SAPDB_Int2 GetOutputColumnSize( SAPDB_Int2 LogColIndex );
    virtual bool GetOutputColumnName( SAPDB_Int2 LogColIndex, SQLMan_Identifier& ColName, bool& IsNullable );
    virtual bool GetOutputColumnExtNo( SAPDB_Int2 LogColIndex, SAPDB_Int2& ColumnIndex );
    virtual bool CheckForColumn( const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    virtual bool CheckForColumn( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    /// ...
            bool CheckForColumnSpecialJoinFromSelect( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table, const SQLMan_Identifier& Column, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, bool& IsNullable );
    virtual bool CheckQuantifierName( const SQLMan_Identifier& Schema, const SQLMan_Identifier& Table );
    virtual void GetIdentificationNames(SQLMan_Identifier& Schema, SQLMan_Identifier& Table);
	virtual bool HasReferenceName( void );
    virtual bool IncRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo );
    virtual bool DecRefCountForOutputColumn( SAPDB_Int2 QId, SAPDB_Int2 ColNo );
    virtual bool AddRefCountForOutputColumn( SAPDB_Int2 i, SAPDB_Int2 Count );
    virtual bool IncRefCountForOutputColumn( SAPDB_Int2 i );
    virtual bool DecRefCountForOutputColumn( SAPDB_Int2 i );

    /// ...
	bool GetOutputColumn( SAPDB_Int2 i, SQLMan_Identifier& ColName, SQLMan_Identifier& RefName, SyntaxTreeNodeNo& QuantifierSyntaxNode, SAPDB_Int2& QuantifierID, SAPDB_Int2& LogColumnIndex, SAPDB_Int2& ExtColNo, SAPDB_Int2& SelColNo, bool& IsNullable );

    virtual bool IsSubquery( void ) { return mSubQueryType != SQT_None; }
    virtual bool IsCorrelatedSubquery( void ) { return (mCorrelatedColumns>0); }
	/// ... 
	        SAPDB_Int2 CorrelatedColumns( void ) { return mCorrelatedColumns; }
    /// ...
            void ChangeCorrelatedColumns( SAPDB_Int2 D ) 
            { 
                mCorrelatedColumns += D; 
                SAPDBERR_ASSERT_STATE( mCorrelatedColumns>=0 );
            }
    /// ...
    bool HasUnidentifiedColumns( void ) { return mUnidentifiedColumns; }
    /// ...
    void SetUnidentifiedColumns( void ) { mUnidentifiedColumns=true; }
    /// ...
	  SAPDB_Int2 CorrelatedColumnsRefersToMe( void ) { return mCorrelatedColumnsRefersToThis; }
    /// ...
            void ChangeCorrelatedColumnsRefersToMe( SAPDB_Int2 D ) 
            { 
                mCorrelatedColumnsRefersToThis += D; 
                SAPDBERR_ASSERT_STATE( mCorrelatedColumnsRefersToThis>=0 );
            }

    /// ...
	  SAPDB_Int2 CorrelatedAndAggregatedColumnsRefersToMe( void ) { return mCorrelatedAndAggregatedColumnsRefersToThis; }
    /// ...
            void ChangeCorrelatedAndAggregatedColumnsRefersToMe( SAPDB_Int2 D ) 
            { 
                mCorrelatedAndAggregatedColumnsRefersToThis += D; 
                SAPDBERR_ASSERT_STATE( mCorrelatedAndAggregatedColumnsRefersToThis>=0 );
            }

    virtual bool CompatibleAggregation( Query_Node* TargetNode );
    /// ...
            bool ContainsColumnFromQuantifierWhichIsAnAggregation( Query_Expression& Predicates, Query_Node* Node );
    /// ...
            bool ContainsColumnFromQuantifierWhichIsAnAggregation( Container_Vector<Query_ColumnSpec*>* pColSpec, Query_Node* Node );
    /// ...
            bool PushDownForMerge( Query_Node* TargetNode );
    /// ...
            bool PushDownQuantifier( Query_Node* TargetNode );
    virtual bool PullDownForMerge( 
                    Container_Vector<Query_OutputColumn*>*& pOutputColumns, SyntaxTreeNodeNo SelectList_AP,
                                                                           SyntaxTreeNodeNo SelectInto_AP,
                    Query_Expression& SearchCondColumns,                   SyntaxTreeNodeNo SearchCond_AP,
                    Query_Expression& HavingColumns,                       SyntaxTreeNodeNo Having_AP,
                    Container_Vector<Query_ColumnSpec*>*& pGroupByColumns, SyntaxTreeNodeNo GroupBy_AP,
                    Container_Vector<Query_ColumnSpec*>*& pOrderByColumns, SyntaxTreeNodeNo OrderBy_AP,
                    Query_Expression& UpdateColumns,                       SyntaxTreeNodeNo Update_AP,
                    Container_Vector<Query_Quantifier*>& Quantifier,       SyntaxTreeNodeNo FromPart_AP,
                    SyntaxTreeNodeNo LimitStart, 
					SyntaxTreeNodeNo LockOptionStart,
                    bool ForReuse, bool OuterJoinPredicates,
                    SAPDB_Int2 QID,
//                    SAPDB_Int2 NID,
                    Query_MergeType MergeType,
					bool AggrInSelectList,
                    bool MustCreateCopy
                    );

    /// ...
            bool MergeSubquery( Query_SelectNode* SubQuery );
    /// ...
			bool ExistSubqueryToIn( Query_SelectNode* FatherQuery );
    /// ...
			bool CheckColumnsForExistToIn( Query_SelectNode* FatherQuery );
protected:
    /// ...
			bool CheckColumnsForExistToIn( SyntaxTreeNodeNo Start, bool CheckSame, Query_SelectNode* Query1, Query_SelectNode* Query2, SAPDB_UInt2& QID, Container_Vector<SAPDB_Int2>* Cols, SAPDB_Int2& Len );
public:

	virtual bool AddMinOrMaxToAllOutputColumn ( bool Max );

    virtual bool OutputContainsAllColumnsFromQuantifier( Query_Quantifier* Q );
    virtual bool OutputContainsAllColumnsFromOrderBy( void );
    /// ...
            bool OutputContainsAllColumnsFromGroupBy( void );
    /// ...
            bool GroupByContainsAllOrderByColumnsFromLeft( bool Identity );

    virtual bool PullDownSearchCond( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, SAPDB_Int2 QID, bool MustCreateCopy );
    /// ...
    bool         PushDownSearchCond( void );
    virtual bool QualificationPushedDown( void ) { return mQualificationPushedDown; }
    /// ...
    bool         SplitAndAddSearchCondition( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, bool MapColumns, Query_MergeType MergeType, bool AsWhere, bool OuterJoinPredicates );
    /// ...
    bool         AddSearchCondition( Query_Expression& SearchCondition, SyntaxTreeNodeNo SearchCond_AP, bool MapColumns, Query_MergeType MergeType, bool AsWhere );
    /// ...
    bool         AddQualification( SyntaxTreeNodeNo AddSearchCond, bool AsWhere );
    /// ...
    bool         AddGroupBy( Container_Vector<Query_ColumnSpec*>*& pGroupBySpec, SyntaxTreeNodeNo GroupBy_AP, bool MapColumns, Query_MergeType MergeType );
    /// ...
    bool         AddGroupBy( SyntaxTreeNodeNo AddGroupBy );
    /// ...
    bool         AddOrderBy( Container_Vector<Query_ColumnSpec*>*& pOrderBySpec, SyntaxTreeNodeNo OrderBy_AP, bool MapColumns, Query_MergeType MergeType );
    /// ...
    bool         AddOrderBy( SyntaxTreeNodeNo AddGroupBy );
    /// ...
    bool         AddLockOptions( SyntaxTreeNodeNo AddLockOptions );
    /// ...
    SyntaxTreeNodeNo NextAndTerm(SyntaxTreeNodeNo StartNode);
    /// ...
    bool IsLocalPredicate(SyntaxTreeNodeNo CurrentTerm, SyntaxTreeNodeNo& QNo, Container_Vector<SAPDB_Int2>& Cols, Container_Vector<SyntaxTreeNodeNo>& SubQueries, bool& OuterJoinFlag, bool& RownoPredicate, bool FillColAndSubQInfo );
    /// ...
    bool CheckForLocalPredicateAndSubqueries(SyntaxTreeNodeNo CurrentTerm, SyntaxTreeNodeNo& QNo, Container_Vector<SAPDB_Int2>& Cols, Container_Vector<SyntaxTreeNodeNo>& SubQueries, bool& OuterJoinFlag, bool& RownoPredicate, bool CheckSame, bool FillColAndSubQInfo );
    /// ...
    SyntaxTreeNodeNo ExtractPredicate(SyntaxTreeNodeNo& CurrentTerm);
    /// ...
    SyntaxTreeNodeNo ExtractPredicate(Query_Expression& OldPredicates, Query_Expression& Predicate, Container_Vector<SAPDB_Int2>& Cols);
    /// ...
    SyntaxTreeNodeNo ExtractPredicate(SyntaxTreeNodeNo& CurrentTerm, Query_Expression& OldPredicates, Query_Expression& Predicate, Container_Vector<SAPDB_Int2>& Cols);
    /// ...
    void RemoveOuterJoinFlag( SyntaxTreeNodeNo Node );

    /// ...
    bool         ResolveAllMultiAsteriskIntoSingleAsterisk( void );
    /// ...
    bool         ReplaceAsteriskReferToLowerNodeForMerge( Query_Node* LowerNode );
    /// ...
    bool         MappColumnFromOuterSelect( Query_Column* Column, Query_MappingInfo* MappingInfo );
    /// ...
    bool         SetColTypeOnMapping( bool MappedColumnInAggr, Query_OutColType InnerColType, Query_OutColType& ResultingColType, Query_Position Pos );

    virtual bool SimplifyPredicates( void );
	void         RemoveDeletedColumns( Query_Expression& Predicates );
	void         RemoveDeletedSubqueries( void );
    /// ...
    bool         SimplifyPredicates( Query_Expression& Predicates, SyntaxTreeNodeNo& PredicateStart_AP );
    virtual bool PredicatesSimplified( void );
	///
	void         SetPredicatesNotSimplified( void ) { mSimplifiedPredicates = false; }
    virtual bool HasRownoPredicate( void )  { return mRownoPredicate; }
    virtual void SetRownoPredicate( void )  { mRownoPredicate = true; }

	virtual	bool SubqueryOptimized( void );
	virtual	void SetSubqueryOptimized( void );

    virtual bool EliminateOrderBy( void );
    virtual bool OrderByEliminated( void );

    virtual bool PullDownProjectionPossible( void );
    virtual bool PullDownProjection( void );
    virtual bool ProjectionPulledDown( void );

    virtual bool OneTupleCondition( void );
    virtual bool OneTupleAggregation( void );
    virtual bool NoDuplicateCondition( void );
    
    virtual bool DefineUniqueOutput(Container_Vector<SAPDB_Int2>& LogOutColNr);

	virtual bool CreateColRefNamesAndResolveAsterisk( SyntaxTreeNodeNo& RefNameNodes );

    //--

    /// ...
			bool ExchangeNodeIDForCorrColumnsOnMerge( SAPDB_UInt2 OldNodeID, SAPDB_UInt2 NewNodeID );

    virtual bool RenewSemanticsForColumnsDueToColumnPrivs( Query_Node* NewNode );
    virtual bool RenewSemanticsForCorrelatedColumnsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode );

    virtual bool RenewSemanticsForMyColumnsDueToColumnPrivs( Query_Node* NewNode );
    virtual bool RenewSemanticsForMyColumnsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode );

    /// ...
    bool         RenewSemanticsForMySelectListDueToColumnPrivs( Query_Node* NewNode );
    /// ...
    bool         RenewSemanticsForMySelectListOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode );

    /// ...
    bool         RenewSemanticsForColumnSpecDueToColumnPrivs( Query_Node* NewNode, Container_Vector<Query_ColumnSpec*>* pColSpec );
    /// ...
    bool         RenewSemanticsForColumnSpecOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Container_Vector<Query_ColumnSpec*>* pColSpec, Query_Position Pos );

    /// ...
    bool         RenewSemanticsForColumnsDueToColumnPrivs( Query_Node* NewNode, Query_Expression Expression );
    /// ...
    bool         RenewSemanticsForColumnsOnMergeForCorrCols( SAPDB_UInt2 OldQID, Query_Node* NewNode, Query_Expression Expression, Query_Position Pos );

    virtual bool RenewSemanticsForColumnDueToColumnPrivs( Query_Column* C );
    virtual bool RenewSemanticsForColumnOnMergeForCorrCols( Query_Column* C, Query_Position Pos );

	/// ...
	bool         DetermineCountOrSumInSelListNode( void );

    //--

    virtual bool SetTempFileNameAndCounter( SAPDB_Int1 Level, SAPDB_Int2& Cnt, bool FatherIsUnion, SAPDB_Int2& FromSelCnt, SAPDB_Int2& ParsCnt, bool& ContainsCorrelatedSubquery, bool& ContainsFromSelectInSubquery, SAPDB_Int2& MaxSubqueryLevel );

    virtual bool RemoveSubSelectNode( SyntaxTreeNodeNo SubSelNode, Query_Position PosType, bool ReplaceByChildren );
    virtual bool AddSubSelectNode( SyntaxTreeNodeNo SubSelNode, SyntaxTreeNodeNo FatherNode, Query_Position FatherPosType );
    /// ...
    bool RemoveGroupBy( void );
    /// ...
    bool RemoveDistinctNode( void );
    /// ...
    bool AddDistinctNode( void );

    virtual SyntaxTreeNodeNo	SelectListStart( void )							{ return mSelectListStartNo; }
    virtual void				SetSelectListStart( SyntaxTreeNodeNo Start )	{ mSelectListStartNo = Start; }

    virtual SyntaxTreeNodeNo	SelectIntoStart( void )							{ return mSelectIntoStartNo; }
    virtual void				SetSelectIntoStart( SyntaxTreeNodeNo Start )	{ mSelectIntoStartNo = Start; }
    virtual bool				HasSelectInto( void )					     	{ return (mSelectIntoStartNo > 0); }


    virtual SyntaxTreeNodeNo	FromPartStart( void )							{ return mFromPartStartNo; }
    virtual void				SetFromPartStart( SyntaxTreeNodeNo Start )		{ mFromPartStartNo = Start; }

    virtual SyntaxTreeNodeNo	SearchConditionStart( void )					{ return mSearchConditionStartNo; }
    virtual void				SetSearchConditionStart( SyntaxTreeNodeNo Start ){ mSearchConditionStartNo = Start; }
    virtual bool				HasSearchCondition( void )						{ return (mSearchConditionStartNo > 0); }
    virtual SyntaxTreeNodeNo	OrderByStart( void )							{ return mOrderByStartNo; }
    virtual void				SetOrderByStart( SyntaxTreeNodeNo Start )       { mOrderByStartNo = Start; }
    virtual bool				HasOrderBy( void )								{ return (mOrderByStartNo > 0); }
    virtual SyntaxTreeNodeNo	GroupByStart( void )							{ return mGroupByStartNo; }
    virtual void				SetGroupByStart( SyntaxTreeNodeNo Start )       { mGroupByStartNo = Start; }
    virtual bool				HasGroupBy( void )								{ return (mGroupByStartNo > 0); }
    virtual SyntaxTreeNodeNo	HavingStart( void )								{ return mHavingStartNo; }
	virtual void				SetHavingStart( SyntaxTreeNodeNo Start )        { mHavingStartNo = Start; }
    virtual bool				HasHaving( void )								{ return (mHavingStartNo > 0); }

    virtual SyntaxTreeNodeNo	LimitStart( void )								{ return mLimitStartNo; }
	virtual void				SetLimitStart( SyntaxTreeNodeNo Start )         { mLimitStartNo = Start; }
	virtual bool				HasLimit( void )								{ return (mLimitStartNo > 0); }

	virtual SyntaxTreeNodeNo	UpdateColumnsStart( void )						{ return mForUpdateColumnStartNo; }
    virtual void				SetUpdateColumnsStart( SyntaxTreeNodeNo Start ) { mForUpdateColumnStartNo = Start; }
    virtual bool				HasForUpdate( void )							{ return (mForUpdateColumnStartNo > 0); }
    virtual bool				HasForReuse( void )							    { return mForReuse; }
	virtual bool				IsFromSelect( void );
	virtual bool                IsSelectOrSet( void )                           { return true; }
    virtual bool                HasFromSelects( void );
    virtual bool                IsSpecialJoinFromSelect( void )                 { return mSpecialJoinFromSelect; }
    /// ...
	bool IsFromSelectOrPartOfSet( void );


    virtual SyntaxTreeNodeNo	LockOptionStart( void )						    { return mLockOptionStart; }
    virtual void				SetLockOptionStart( SyntaxTreeNodeNo Start )    { mLockOptionStart = Start; }
    virtual bool				HasLockOption( void )							{ return (mLockOptionStart > 0); }

    virtual bool                IsDeclareCursor( void )                         { return mIsDeclareCursor; }
    virtual void                SetDeclareCursor( bool Decl )                   { mIsDeclareCursor = Decl; }
    virtual void                SetA631( void );

    /// ...
    bool& Distinct( void )           { return mDistinct; }

    /// ...
    Container_Vector<Query_OutputColumn*>*& GetOutputColumns( void )
    {
        return mpOutputColumns;
    }

    /// ...
    Query_Expression& GetSearchCondColumns( void )
    {
        return mSearchConditionColumns;
    }

    virtual void DetermineAPNode( void );

    virtual SyntaxTreeNodeNo GetInternAPNode( void )
    {
        return mInternAPNode;
    }

    virtual void SetInternAPNode( SyntaxTreeNodeNo NodeNo )
    {
        mInternAPNode = NodeNo;
    }

    virtual void SetSubQueryType( Query_SubQueryType SubQueryType )
    {
        mSubQueryType = SubQueryType;
    }

    virtual Query_SubQueryType GetSubQueryType( void )
    {
        return mSubQueryType;
    }

    /// ...
    bool SetSubQueryTypeForQuantifier( SyntaxTreeNodeNo QNodeNo, Query_SubQueryType SubQueryType );


protected:

    /// ...
    bool ParseSelectList( SyntaxTreeNodeNo Start, bool ComplexViewReplacement );
    /// ...
    bool ParseSelectInto( SyntaxTreeNodeNo Start );
    /// ...
    bool ParseFromPart( SyntaxTreeNodeNo Start, bool ComplexViewReplacement );
    /// ...
    bool ParseWhere( SyntaxTreeNodeNo Start, bool ComplexViewReplacement );
    /// ...
    bool ParseHaving( SyntaxTreeNodeNo Start, bool ComplexViewReplacement );
    /// ...
    bool ParseGroupBy( SyntaxTreeNodeNo Start, bool ComplexViewReplacement );
    /// ...
    bool ParseOrderBy( SyntaxTreeNodeNo Start, bool ComplexViewReplacement );
    /// ...
    bool ParseForUpdate( SyntaxTreeNodeNo Start );
    /// ...
    bool ParseLockOption( SyntaxTreeNodeNo Start );

    /// ...
	bool Semantics_FAndLQuantifier( SQLMan_Surrogate UserId, bool& SelectPrivilegesForAllColumns,  bool ComplexViewReplacement );
    /// ...
	bool Semantics_Quantifier( SQLMan_Surrogate UserId, bool UniqueOutputColumnNames, bool& SelectPrivilegesForAllColumns,  bool ComplexViewReplacement, Query_QuantifierType QuantifierType );
    /// ...
    bool Semantics_OtherQuantifier( SQLMan_Surrogate UserId, bool ComplexViewReplacement );
    /// ...
    bool Semantics_SelectList( bool ComplexViewReplacement, bool& HasAsterisk );
    /// ...
    bool Semantics_WherePart( void );
    /// ...
    bool Semantics_Having( void );
    /// ...
    bool Semantics_GroupBy( void );
    /// ...
    bool Semantics_OrderBy( void );
    /// ...
	bool Semantics_UpdateColumns( void );
    /// ...
    bool Semantics_CheckUniqueOutputColumns( void );
    /// ...
    bool Semantics_CheckUniqueOutputColumns( Query_OutputColumn* Col1, Query_OutputColumn* Col2 );
    /// ...
    bool Semantics_CheckUniqueOutputColumns( const SQLMan_Identifier& ColName, Query_Quantifier* Q );
    /// ...
    bool Semantics_CheckAgainstGroupBy( void );
    /// ...
    bool Semantics_CheckAgainstGroupBy( Container_Vector<Query_ColumnSpec*>*& pColSpec );
    /// ...
    bool Semantics_CheckAgainstGroupBy( Query_Expression& Expr );
    /// ...
    bool Semantics_CheckCheckIfGroupByColumn( Query_Column* C );

    /// ...
	bool IsColumnOfTypeLong( SAPDB_Int2 QuantifierSyntaxNode, SAPDB_Int2 ExtColNo );

    /// ...
    bool MappSelectListFromOuterSelect( Container_Vector<Query_OutputColumn*>* OutputColumns, Query_MergeType MergeType );

    /// ...
    bool ResolveAllAsteriskForAllQuantifiers( bool IncRefCounts );
    /// ...
    bool ResolveAllAsteriskForQuantifiersWithoutAllSelectPriv( void );
    /// ...
    bool ResolveSingleAsterisk( SAPDB_Int2 SelectListColNo, bool IncRefCounts );
    /// ...
    bool ResolveOneMultiAsteriskIntoSingleAsterisk( SAPDB_Int2 SelectListColNo );
    /// ...
    bool CreateColRefNames( SyntaxTreeNodeNo& RefNameNodes );
    /// ...
	bool SubstituteOneColForAsterisk( SyntaxTreeNodeNo& SelColNo, SAPDB_Int2 QID, SyntaxTreeNodeNo ColQuantNode, const SQLMan_Identifier& ColName, SAPDB_Int2 ColLogIndexQuant, SAPDB_Int2 ColExtColNoQuant, SAPDB_Int2 ColLogIndexOwn, SAPDB_Int2 ColCnt, SAPDB_Int2 ErrPos, bool IsNullable, bool CreateNewNode );

    /// ...
    SAPDB_Int2 CountOutputColumns( void );

    // --------------------------------------------------------------------------------------------

    /// ...
    SQLMan_Identifier                       mReferenceName;             /// ...

    Container_Vector<Query_OutputColumn*>*  mpOutputColumns;            /// ...
    Container_Vector<Query_ColumnSpec*>*    mpGroupByColumns;           /// ...
    Container_Vector<Query_ColumnSpec*>*    mpOrderByColumns;           /// ...
    Query_Expression                        mSearchConditionColumns;    /// ...
    Query_Expression                        mHavingColumns;             /// ...
	Query_Expression                        mUpdateColumns;             /// ...
    SAPDB_Int2                              mOutputColumnCnt;           /// ...

	SyntaxTreeNodeNo    mInternAPNode;                                  /// ...
    SyntaxTreeNodeNo    mCursorNameNo;                                  /// ...
    SyntaxTreeNodeNo    mRefNameNode;                                   /// ...
    SyntaxTreeNodeNo    mSelectListStartNo;                             /// ...
    SyntaxTreeNodeNo    mSelectIntoStartNo;                             /// ...
    SyntaxTreeNodeNo    mFromPartStartNo;                               /// ...
    SyntaxTreeNodeNo    mSearchConditionStartNo;                        /// ...
    SyntaxTreeNodeNo    mOrderByStartNo;                                /// ...
    SyntaxTreeNodeNo    mGroupByStartNo;                                /// ...
    SyntaxTreeNodeNo    mHavingStartNo;                                 /// ...
	SyntaxTreeNodeNo    mLimitStartNo;                                  /// ...
	SyntaxTreeNodeNo    mForUpdateColumnStartNo;                        /// ...
    SyntaxTreeNodeNo    mLockOptionStart;                               /// ...

public:
    /// ...
	SAPDB_Int2 mExprCnt;                                                /// ...

private:
	Query_JoinType  mJoinType;                                          /// ...
    Query_SubQueryType mSubQueryType;                                   /// ...    
	bool      mDistinct;                                                /// ...
	bool      mForReuse;                                                /// ...
	bool      mGivenSequence;                                           /// ...
    bool      mIsDeclareCursor;                                         /// ...
    Query_TrueFalseNull mResDelete;                                     /// ...

    SAPDB_Int2 mCorrelatedColumns;                                      /// ...
    SAPDB_Int2 mCorrelatedColumnsRefersToThis;                          /// ...
    SAPDB_Int2 mCorrelatedAndAggregatedColumnsRefersToThis;             /// ...
	bool      mUnidentifiedColumns;                                     /// ...
    bool      mQualificationPushedDown;                                 /// ...
    bool      mSimplifiedPredicates;                                    /// ...
    bool      mSubqueryOptimized;                                       /// ...
    bool      mRownoPredicate;                                          /// ...
    bool      mOrderByEliminated;                                       /// ...
    bool      mProjectionPulledDown;                                    /// ...

    bool      mAggregatesInOutput;                                      /// ...
    bool      mAggregatesNotInOutput;                                   /// ...
    bool      mOneTupleCondition;                                       /// ...
    bool      mSpecialJoinFromSelect;                                   /// ...
    Query_TrueFalseNull  mNoDuplicateCondition;                         /// ...
};

// ------------------------------------------------------------------------------------------------

#endif