/*!***************************************************************************

    @file     Query_Graph.hpp
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

#ifndef QUERY_GRAPH_H
#define QUERY_GRAPH_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/SQLMan_Types.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Types.hpp"

#include "SAPDB/SQLManager/Query/Query_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_ManipulationContext.hpp"
#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"
#include "SAPDB/SQLManager/Query/Query_Node.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeRoot.hpp"

#include "SAPDB/SQLManager/Query/Query_OutputColumn.hpp"
#include "SAPDB/SQLManager/Query/Query_ColumnSpec.hpp"
#include "SAPDB/SQLManager/Query/Query_Column.hpp"
#include "SAPDB/SQLManager/Query/Query_Operator.hpp"


#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

#include "hak07.h"

// ------------------------------------------------------------------------------------------------

class Catalog_ViewDescriptor;
class SAPDBMem_IRawAllocator;

// ------------------------------------------------------------------------------------------------
/*!
    @struct         Query_ComplexViewInfo
    @brief          ...
 */
struct Query_ComplexViewInfo
{
    /// ...
    SyntaxTreeNodeNo    SyntaxNode;             
    /// ...
    Catalog_ObjectId    TabID;                  
    /// ...
	bool                ReferencedViaSynonym;   
};



/*!
    @class          Query_QueryGraph
    @brief          ...
 */
class Query_QueryGraph
{

public:
    /// ...
	Query_QueryGraph( Query_ManipulationContext& Context )
		: mContext(Context)
		, mAllocator(Context.GetAllocator())
		, mSyntaxTree(Context.SQLManContext())
		, mProgress(PG_Init)
		, mRootNode(0)
		, mCurrID(0)
		, mInitialSQLStatement(true)
		, mNeedOptimization(false)
        , mNeedViewReplacement(false)
		, mVTrace()
		, mError(false)
		, mNotYetImplemented(false)
        , mContainsSpecialJoinFromSelect(false)
		, mFromSelects(0)
		, mInsertSelect(false)
	{
		PrintToTrace();
        for ( SAPDB_Int2 i=0; i<QUERY_RULECOUNT; i++ )
        {
            mApplyCnt[i] = 0;
        }
	}

    /// ...
	inline ~Query_QueryGraph( void )
    {
        if ( mRootNode )
            destroy(mRootNode, mAllocator);
    }

    /// ...
	inline Query_Node* RootNode( void )
	{
		return mRootNode;
	}

    /// ...
	inline SAPDB_UInt2 FromSelectsCnt( void )
	{
		return mFromSelects;
	}

    /// ...
	inline void FromSelectFound( void )
	{
		++mFromSelects;
	}

    /// ...
	inline void FromSelectReplaced( void )
	{
        SAPDBERR_ASSERT_STATE( mFromSelects>0 );
		--mFromSelects;
	}

    /// ...
    inline void ContainsSpecialJoinFromSelect( bool sfs )
    {
        mContainsSpecialJoinFromSelect = sfs;
    }

    /// ...
    inline bool ContainsSpecialJoinFromSelect( void )
    {
        return mContainsSpecialJoinFromSelect;
    }

    /// ...
    inline bool CommandIsInsertSelect( void )
    {
        return mInsertSelect;
    }

    /// ...
    inline void SetCommandIsInsertSelect( void )
    {
        mInsertSelect = true;
    }

    /// ...
    bool Parse( void );
    /// ...
    bool Semantic( void );
    /// ... 
    bool Optimize( bool force=false );
    /// ...
    bool GenerateAPTree( void );
    /// ...
    bool GenerateStackCode( void );
    /// ...
    bool GenerateSQLStatement( void );
    /// ...
    bool OrigSQLStatement( void );

    /// ...
	inline void SetNeedOptimization( void ) { mNeedOptimization = true; }
    /// ...
	inline void SetNeedViewReplacement( void ) { mNeedViewReplacement = true; }

    /// ...
    bool CheckGraph( void );

    /// ...
    inline bool IsOk( void )
    {
        return (!mError && !mNotYetImplemented) && mContext.IsOk();
    }

    /// ...
    inline void SetRuleApplied( SAPDB_Int2 RuleNo, tak_queryrewrite_exec__Enum Rule )
    {
        (mApplyCnt[RuleNo])++;
        mContext.SQLManContext().a_queryrewrite_exec.addElement(Rule);
        mContext.SQLManContext().a_queryrewrite_done.becomes(Operator_Rewrite);
    }

    /// ...
    void TransferApplyCounts( void );

    /// ...
	inline void SetSyntaxError( SAPDB_Int2 BasisError, SAPDB_Int4 Code )
	{
		a07_b_put_error (mContext.SQLManContext(), BasisError, Code);

        if (QueryRewrite_Trace.TracesLevel(2))
            mVTrace << "Graph - SyntaxError: " << mContext.SQLManContext().a_returncode << FlushLine;        
        mError = true;
		mContext.SetError();
        SetProgress(PG_Error, /*TraceGraph=*/ false);
	}

    /// ...
	inline void SetSemanticError( SAPDB_Int2 BasisError, SAPDB_Int4 Code )
	{
		a07_b_put_error (mContext.SQLManContext(), BasisError, Code);
			
        if (QueryRewrite_Trace.TracesLevel(2))
            mVTrace << "Graph - SemanticError: " << mContext.SQLManContext().a_returncode << FlushLine;        
		mError = true;
		mContext.SetError();
        SetProgress(PG_Error, /*TraceGraph=*/ false);
	}

    /// ...
    inline void SetError( const char* ErrText, bool TraceGraph=true )
    {
        bool OldErr = mError;
        if ( QueryRewrite_Trace.TracesLevel(2))
            mVTrace << "Graph - Error: " << ErrText << FlushLine;        
        mError = true;
		if ( !mNotYetImplemented )
			mContext.SetError();
        SetProgress(PG_Error, TraceGraph && !OldErr);
    }

    /// ...
    inline void SetError( const char* ErrText, SAPDB_Int2 NodeNo, bool TraceGraph=true )
    {
        bool OldErr = mError;
        if ( QueryRewrite_Trace.TracesLevel(2))
            mVTrace << "Graph - Error '" << ErrText << "' in Node " << NodeNo << FlushLine;        
        mError = true;
		if ( !mNotYetImplemented )
			mContext.SetError();
        SetProgress(PG_Error, TraceGraph && !OldErr);
    }

    /// ...
    inline void SetNotYetImplemented( const char* ErrText, bool TreatAsError=true )
    {
        if (QueryRewrite_Trace.TracesLevel(2))
            mVTrace << "Graph - Not yet implemented: " << ErrText << FlushLine;        
        mNotYetImplemented = true;
		mContext.SetNotYetImplemented();
		if ( TreatAsError )
        {
			mError = true;
            SetProgress(PG_Error, true);
        }
    }

    /// ...
    inline void SetProgress ( Query_Progress P, bool TraceGraph=true )
    {
        if ( mProgress != P || PG_SubstituteViews )
        {
            mProgress = P;
			if ( TraceGraph )
				PrintToTrace();
        }
    }

    /// ...
    void PrintToTrace( void );
    /// ...
    void PrintQueryToTrace( void );

    /// ...
    inline SAPDBMem_IRawAllocator& Allocator( void )
    {
        return mAllocator; 
    }

    /// ...
    inline Query_ManipulationContext& ManipulationContext( void ) 
    { 
        return mContext; 
    }

    /// ...
    inline Query_SyntaxTree& SyntaxTree( void )
    {
        return mSyntaxTree;
    }

    /// ...
    bool GetIdentifier( SyntaxTreeNodeNo NodeNo, SQLMan_Identifier& Identifier );
    /// ...
    bool GetSchemaName( SyntaxTreeNodeNo& NodeNo, SQLMan_Identifier& Identifier );

    /// ...
    bool SubstituteComplexView( Query_ComplexViewInfo& ComplexView, SyntaxTreeNodeNo& ColNameNodes );

    /// ...
	bool AddExpressionRefName( SyntaxTreeNodeNo Node, SAPDB_Int2& ExprCount, SQLMan_Identifier& ExprName );

    /// ...
	inline void SetNodeID( Query_Node* B )
    {
        B->SetNodeID(mCurrID++);
    }

    /// ... 
    inline void SetASelectNode( SyntaxTreeNodeNo Node )
    {
        mContext.SQLManContext().a_select_node = Node;
    }

    /// ...
	bool AddSubSelectNode( SyntaxTreeNodeNo SelNode, Query_Position Position );

    /// ...
	bool AddSubSelectNode( SyntaxTreeNodeNo StartNode, Query_Position Position, SAPDB_Int2 SubQueryType, bool ComplexViewReplacement );
    /// ...
    bool SCVMoveSubqueryInformation( Query_SyntaxTree& NewSyntaxTree, SyntaxTreeNodeNo StartNode );

private:

 //   /// ...
	//inline bool SubSelectNodeMustBeAdded( Query_Position Position );


    // Functions used by SubstituteComplexView
    /// ...
    bool SCVCreateInternalPacket( SQLMan_Context& ViewContext, SAPDB_Int4 ViewStmtLength );
    /// ...
    bool SCVReleaseInternalPacket( SQLMan_Context& ViewContext );
    /// ...
	void* SCVSaveContextStmtPart( SQLMan_Context& ViewContext );
    /// ...
	bool SCVRestoreContextStmtPart( SQLMan_Context& ViewContext, void* SavedPart );
    /// ...
    bool SCVParseViewStatement( SQLMan_Context& ViewContext, Catalog_ViewDescriptor& ViewDescr );
    /// ...
    bool SCVCheckIfViewCanBeSubstituted( Query_SyntaxTree& NewSyntaxTree, SyntaxTreeNodeNo& ViewNameNode, SyntaxTreeNodeNo& ViewSelectNode, SyntaxTreeNodeNo& ColNameNodes );
    /// ...
    bool SCVLinkSyntaxTrees( SyntaxTreeNodeNo FromTabNode, SyntaxTreeNodeNo ViewSyntaxTreeNode, tak_ap_node* OrgViewNameNode, bool ReferencedViaSynonym );

private:

    /// ...
    Query_ManipulationContext&  mContext;               
    /// ...
    SAPDBMem_IRawAllocator&     mAllocator;             

    /// ...
    Query_SyntaxTree            mSyntaxTree;            

    /// ...
    Query_RootNode*				mRootNode;              

    /// ...
    Kernel_VTrace               mVTrace;               

    /// ...
    SAPDB_UInt2                 mApplyCnt[QUERY_RULECOUNT]; 
    /// ...
    SAPDB_UInt2                 mCurrID;                
    /// ...
    Query_Progress              mProgress;              

    /// ...
    bool                        mInitialSQLStatement;   
    /// ...
    bool                        mNeedOptimization;      
    /// ...
    bool                        mNeedViewReplacement;   
    /// ...
    bool                        mError;                 
    /// ...
    bool                        mNotYetImplemented;     
    /// ...
    bool                        mContainsSpecialJoinFromSelect;
	/// ...
	bool                        mInsertSelect;
    /// ...
	SAPDB_UInt2					mFromSelects;           
};

// ------------------------------------------------------------------------------------------------

//inline bool Query_QueryGraph::SubSelectNodeMustBeAdded( Query_Position Position )
//{
//	if ( Position == P_SelectList ||
//		 Position == P_FromPart  ||
//		 Position == P_Where ||
//		 Position == P_WhereA ||
//		 Position == P_WhereE ||
//		 Position == P_Having ||
//		 Position == P_HavingA ||
//		 Position == P_HavingE 
//		)
//		return true;
//	return false;
//}

//inline Query_QuantifierType Query_QueryGraph::DetermineQuantifierType( Query_Position Position )
//{
//    switch ( Position ) 
//    {
//	case P_Start: 
//    case P_FromPart: 
//	case P_Union: 
//	case P_Difference: 
//	case P_Intersect:
//        return QT_F;
//        break;
//    case P_Where: 
//    case P_Having: 
//        return QT_S;
//        break;
//    case P_WhereE: 
//    case P_HavingE: 
//        return QT_E;
//        break;
//    case P_WhereA: 
//    case P_HavingA: 
//        return QT_A;
//        break;
//    case P_SelectList:
//        return QT_L;
//        break;
//    default:
//        SAPDBERR_ASSERT_STATE( 0 );
//        return QT_Unknown;
//        break;
//    }
//    return QT_Unknown;
//}

// ------------------------------------------------------------------------------------------------

#endif