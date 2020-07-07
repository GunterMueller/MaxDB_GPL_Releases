/*!***************************************************************************

    @file     Query_Graph.cpp
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

#include "SAPDB/SQLManager/Query/Query_Graph.hpp"
#include "SAPDB/SQLManager/Query/Query_Node.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeRoot.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeTable.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeCursor.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeUnion.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeIntersect.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeDifference.hpp"

#include "SAPDB/SQLManager/Catalog/Catalog_ViewDescriptor.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"

#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_Rewrite.hpp"
#include "SAPDB/SQLManager/QueryRewrite/QueryRewrite_RuleList.hpp"

#include "SAPDB/RunTime/RTE_CurrentStack.hpp"

#include "hak05.h"
#include "hak06.h"
#include "hak35.h"
#include "hak542.h"
#include "hak61.h"

// ------------------------------------------------------------------------------------------------

bool Query_QueryGraph::GetIdentifier( SyntaxTreeNodeNo NodeNo, SQLMan_Identifier& Identifier )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::GetIdentifier", QueryRewrite_Trace, 6);
    SAPDBERR_ASSERT_ARGUMENT( NodeNo > 0 );

    if ( NodeNo > 0 )
    {
		a05_identifier_get_from_buffer(mContext.SQLManContext(),
			mContext.GetSQLStatementPtr(), mContext.GetSQLStatementSize(),
			mSyntaxTree[NodeNo].n_pos(), mSyntaxTree[NodeNo].n_length(), mSyntaxTree[NodeNo].n_special.includes(ni_complex_string_id),
			sizeof(SQLMan_Identifier), &Identifier[0]);
        if ( mContext.SQLManContext().a_returncode == 0 )
            return true;
        mContext.SQLManContext().a_returncode = 0;
        SetError("MissingIdentifier");
    }
    Identifier.SetBlank();
    return false;
}

bool Query_QueryGraph::GetSchemaName( SyntaxTreeNodeNo& NodeNo, SQLMan_Identifier& Identifier )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::GetSchemaName", QueryRewrite_Trace, 6);
    SAPDBERR_ASSERT_ARGUMENT( NodeNo > 0 );

    if ( NodeNo > 0 )
    {
        int N = NodeNo;
		a06get_username_from_buffer(mContext.SQLManContext(), N,
			mContext.GetSQLStatementPtr(), mContext.GetSQLStatementSize(),
			Identifier);
        NodeNo = N;
        if ( mContext.SQLManContext().a_returncode == 0 )
            return true;
        mContext.SQLManContext().a_returncode = 0;
        SetError("MissingIdentifier");
    }
    Identifier.SetBlank();
    return false;
}

// ------------------------------------------------------------------------------------------------

void Query_QueryGraph::TransferApplyCounts( void )
{
    QueryRewrite_GlobalRuleList* RuleList = QueryRewrite_GlobalRuleList::Instance();
    for ( SAPDB_Int2 i=0; i<QUERY_RULECOUNT; i++ )
    {
        if ( mApplyCnt[i] )
        {
            QueryRewrite_Rule* R = (*RuleList)[i];
            R->ChangeApplyCount(mApplyCnt[i]);
        }
    }
}

// ------------------------------------------------------------------------------------------------

bool Query_QueryGraph::Parse( void )
{
    if ( IsOk() )
    {
        char* StatementPtr = (char*) mContext.SQLManContext().GetSQLStatementPtr(); // just for debugging

        if ( mRootNode )
            destroy(mRootNode, mAllocator);

        if ( mRootNode = new(mAllocator) Query_RootNode(mContext, *this) )
		{
            if ( !mRootNode->Parse(/*ComplexViewReplacement*/false) )
				SetError("Parsing");
		}
        else
            SetError("Initialize");
    }
    return IsOk();
}

bool Query_QueryGraph::Semantic( void )
{
    if ( IsOk() )
    {
        SQLMan_Surrogate UserId = mContext.SQLManContext().GetCurrentUserId();

        if ( mRootNode->Semantics(UserId, /*UniqueOutputColumns=*/false, /*ComplexViewReplacement=*/false) )
        {
            SetProgress(PG_Semantics);

            if ( mNeedViewReplacement )
            {
#if !defined( QRW_REPLACEVIEWS_DIRECT )
                if ( !mRootNode->ReplaceComplexViews() )
                    SetError("ComplexViewReplacements failed");
#else
                SetError("Unreplaced ComplexViews");
                SAPDBERR_ASSERT_STATE( 0 );
#endif
            }
        }
        else
        {
            SetError("Semantics failed");
        }
    }
    return IsOk();
}

bool Query_QueryGraph::Optimize( bool force )
{
    if ( IsOk() )
	{
	    if ( mNeedOptimization || force )
	    {
            QueryRewrite_GlobalRuleList& Rules = *(QueryRewrite_GlobalRuleList::Instance());

		    Opti_QueryRewriter QueryRewriter(mContext, *this);
		    QueryRewriter.Optimize(Rules);
        }
	}
    return IsOk();
}

bool Query_QueryGraph::GenerateAPTree( void )
{
    if ( IsOk() )
    {
        bool    ContainsCorrelatedSubquery   = false, 
                ContainsFromSelectInSubquery = false; 
        bool    FatherIsUnion                = false;

        SAPDB_Int2  MaxSubqueryLevel = 0,
                    FromSelCnt = 0,
                    ParsCnt = 0,
                    Cnt=0;

        if ( mRootNode->SetTempFileNameAndCounter(0, Cnt, FatherIsUnion, FromSelCnt, ParsCnt, ContainsCorrelatedSubquery, ContainsFromSelectInSubquery, MaxSubqueryLevel) )
        {
            if ( FromSelectsCnt() != FromSelCnt )
            {
                SAPDBERR_ASSERT_STATE( FromSelectsCnt() == FromSelCnt );
                SetError("FromSelectsCnt() != FromSelCnt");
                return false;
            }

            if ( ContainsCorrelatedSubquery )
            {
                if ( MaxSubqueryLevel >= Query_MaxCorrLevel )
                {
                    SetNotYetImplemented("Too complicated SQL statement (too many subqueries) -1102");
                    return false;
                }
                if ( ContainsFromSelectInSubquery )
                {
                    SetNotYetImplemented("Correlated Subquery with Select in From Part -7058");
                    return false;
                }
            }

            if ( FromSelectsCnt() > 0 )
                mContext.SQLManContext().a_from_select = true;
            else
                mContext.SQLManContext().a_from_select = false;
            if ( QueryRewrite_Trace.TracesLevel(2) )
                mVTrace << "Number of FromSelects left: " << FromSelectsCnt() << FlushLine;
			return true;
        }
		SetError("GenerateAPTree - SetTempFileNameAndCounter");
    }
    return false;
}

bool Query_QueryGraph::GenerateStackCode( void )
{
    return false;
}

bool Query_QueryGraph::GenerateSQLStatement( void )
{
    return false;
}

bool Query_QueryGraph::OrigSQLStatement( void )
{
    return false;
}

// ------------------------------------------------------------------------------------------------

void Query_QueryGraph::PrintToTrace( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_QueryGraph::PrintToTrace", QueryRewrite_Trace, 5);
    SAPDBERR_ASSERT_STATE(  ((mProgress == PG_Error) &&  mError ) ||
                            ((mProgress != PG_Error) && !mError ) );

    if ( QueryRewrite_Trace.TracesLevel(1) )
    {
        int Check = false;
        switch ( mProgress )
        {
        case PG_Init:
            mVTrace << "-- Query: Initial State -------------------------" << FlushLine;
            break;
        case PG_Nodes:
            mVTrace << "-- Query: QueryNodes created --------------------" << FlushLine;
            Check = true;
            break;
        case PG_SubstituteViews:
            mVTrace << "-- Query: Views substituted ---------------------" << FlushLine;
            Check = true;
            break;
        case PG_Semantics:
            mVTrace << "-- Query: SemanticInfo added --------------------" << FlushLine;
            Check = true;
            break;
        case PG_Normalized:
            mVTrace << "-- Query: SearchCond normalized -----------------" << FlushLine;
            Check = true;
            break;
        case PG_Optimized:
            mVTrace << "-- Query: Query optimized -----------------------" << FlushLine;
            Check = true;
            break;
        case PG_Error:
            mVTrace << "-- Query: ERRORS !!! ----------------------------" << FlushLine;
            break;
        default:
            mVTrace << "-- Query: Unknown state !!! ---------------------" << FlushLine;
            break;
        }

	    if ( QueryRewrite_Trace.TracesLevel(2) )
		{
			mSyntaxTree.PrintToTrace();
			PrintQueryToTrace();
		}

#if defined(SAPDB_SLOW)
        if ( Check )
            if ( !CheckGraph() )
            {
                SAPDBERR_ASSERT_STATE( 0 ); // DDT: Never fullfilled ! slowknl will alway break
            }
#endif
    }
}

void Query_QueryGraph::PrintQueryToTrace( void )
{
    if ( mRootNode )
    {
        if ( QueryRewrite_Trace.TracesLevel(1) )
        {
            mRootNode->PrintToTrace(mVTrace);
            if ( QueryRewrite_Trace.TracesLevel(2) )
            mVTrace << "+-------------------------------" << FlushLine;
        }
    }
}

// ------------------------------------------------------------------------------------------------

bool Query_QueryGraph::CheckGraph( void )
{
#if defined(SAPDB_SLOW)
    if ( !mSyntaxTree.CheckNodes() )
    {
        mError = true;
        mProgress = PG_Error;
        //if ( !QueryRewrite_Trace.TracesLevel(2) )
        //    SyntaxTree().PrintToTrace();
    }
#endif
    return !mError;
}

// ------------------------------------------------------------------------------------------------

bool Query_QueryGraph::AddSubSelectNode( SyntaxTreeNodeNo SelNode, Query_Position Position )
{
	if ( mSyntaxTree[SelNode].n_querycheck.includes(qcn_SubQInfo) )
	{
		SyntaxTreeNodeNo N = SelNode;
        SAPDB_Int2 SubQueryType = mSyntaxTree[SelNode].n_length();
        mSyntaxTree[SelNode].n_length() = 0;

        if ( !AddSubSelectNode(N, Position, SubQueryType, false) )
		{
			SetError("AddSubSelectNode 1");
			return false;
		}
	}
    else
    {
        if ( Position == P_FromPart )
        {
            if ( !AddSubSelectNode(mSyntaxTree[SelNode].n_refback, Position, 0, false) )
		    {
			    SetError("AddSubSelectNode 2");
			    return false;
		    }
        }
    }
    return true;
}

bool Query_QueryGraph::AddSubSelectNode( SyntaxTreeNodeNo StartNode, Query_Position Position, SAPDB_Int2 SubProc, bool ComplexViewReplacement )
{
	SAPDBERR_ASSERT_ARGUMENT( Position == P_FromPart ||
		SubProc == cak_x_subquery ||
		SubProc == cak_x_one_val_subquery ||
		SubProc == cak_x_arith_with_subquery ||
        SubProc == cak_x_corr_subquery ||
        SubProc == cak_x_one_val_corr_sub );

    // Add
	//		a92fromsel		for the new subselect	if Position == P_FromPart
	//      a63/n select							if Position == P_SelectList
    //		a63/n where								if Position == P_Where/P_WhereA/P_WhereE
	//		a63/n having							if Position == P_Having/P_HavinA/P_HavingE

	// -> see ak60put_new_sub_select_node - but that routine wouldn't work for our purpose ;-((

    if ( ComplexViewReplacement )
        if ( mSyntaxTree.FindSubSelectNode(StartNode) > 0 )     // node exists already 
        {
//            SAPDBERR_ASSERT_STATE( 0 );
            return true;
        }

	SyntaxTreeNodeNo ToBeInsert = mSyntaxTree.GetFreeNode();
    if ( ToBeInsert > 0 )
    {
        SAPDB_Int2 HPos = -1, Pos = -1;
        SyntaxTreeNodeNo FatherSelNode = cak_qrewrite_ref_init;
        SyntaxTreeNodeNo SelNode = cak_qrewrite_ref_init;

/**/	SyntaxTreeNodeNo Node = mSyntaxTree[StartNode].n_refback;
        while ( Node > 0 )
        {
            if ( ( mSyntaxTree[Node].n_proc == a63 &&
                  (mSyntaxTree[Node].n_subproc() == cak_x_mass_select ||
				   mSyntaxTree[Node].n_subproc() == cak_x_decl_cursor ||
                   mSyntaxTree[Node].n_subproc() == cak_x_start_union)) ||
                 (mSyntaxTree[Node].n_proc == a63query_spec) )
            {
                if ( FatherSelNode == cak_qrewrite_ref_init )
				{
					SyntaxTreeNodeNo R = mSyntaxTree[Node].n_refback;
					if ( (mSyntaxTree[Node].n_proc == a63query_spec) &&
						 (mSyntaxTree[R   ].n_proc == a66)           &&
						 (mSyntaxTree[R   ].n_subproc() == cak_x_select_in_from_part) )
						FatherSelNode = mSyntaxTree[Node].n_refback;
					else
	                    FatherSelNode = Node;
				}
                SelNode = Node;
            }
            if ( mSyntaxTree[Node].n_proc == a63 )
                if ( (mSyntaxTree[Node].n_subproc() == cak_x_union)      ||
                     (mSyntaxTree[Node].n_subproc() == cak_x_union_all)  ||
                     (mSyntaxTree[Node].n_subproc() == cak_x_except)     ||
                     (mSyntaxTree[Node].n_subproc() == cak_x_except_all) ||
                     (mSyntaxTree[Node].n_subproc() == cak_x_intersect)  ||
                     (mSyntaxTree[Node].n_subproc() == cak_x_intersect_all) )
                {
                    break;
                }

            Node = mSyntaxTree[Node].n_refback;
        }

		if ( Position == P_FromPart )
		{
			mSyntaxTree[ToBeInsert].n_proc.becomes(a92fromsel);
			mSyntaxTree[ToBeInsert].n_subproc()    = StartNode;
			mSyntaxTree[ToBeInsert].n_pos()        = StartNode;
		}
		else
		{
			mSyntaxTree[ToBeInsert].n_proc.becomes(a63);
			mSyntaxTree[ToBeInsert].n_subproc() = SubProc;
			mSyntaxTree[ToBeInsert].n_pos()     = StartNode;
		}
		switch ( Position )
		{
		case P_FromPart:
			mSyntaxTree[ToBeInsert].n_symb.becomes(s_unknown);
			break;
		case P_SelectList:
			mSyntaxTree[ToBeInsert].n_symb.becomes(s_select);
			break;
		case P_Where:
			mSyntaxTree[ToBeInsert].n_symb.becomes(s_where);
			break;
		case P_Having:
			mSyntaxTree[ToBeInsert].n_symb.becomes(s_having);
			break;
		//
		case P_Start:
		case P_Select:
		case P_Union:
		case P_Intersect:
		case P_Difference:
		case P_OrderBy:
		case P_GroupBy:
		case P_UpdateColumns:
		case P_Unknown:
		default:
            SAPDBERR_ASSERT_ARGUMENT( 0 );
			SetError("AddSubSelectNode");
			return false;
			break;
		}

        return mSyntaxTree.AddSubSelectNode(SelNode, ToBeInsert, FatherSelNode);
    }
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_QueryGraph::SCVCreateInternalPacket(SQLMan_Context& ViewContext, SAPDB_Int4 ViewStmtLength)
{
    a542internal_packet (ViewContext, /*release_internal_packet*/false, ViewStmtLength);
    if ( ViewContext.IsOk() )
    {
        ViewContext.a_cmd_part->sp1p_buf_len() = ViewStmtLength;
        ViewContext.a_cmd_segm->sp1c_prepare() = ViewContext.a_initial_segment_header.sp1c_prepare();
        ViewContext.a_cmd_segm->sp1c_producer().becomes(sp1pr_complex_view_handling);
        ViewContext.a_cmd_segment_header       = ViewContext.a_cmd_segm->sp1s_segm_header();
        return true;
    }
    return false;
}

bool Query_QueryGraph::SCVReleaseInternalPacket(SQLMan_Context& ViewContext)
{
    a542pop_packet(ViewContext);	// release the internal packet AND restore the previous one
    return ViewContext.IsOk();
}


void* Query_QueryGraph::SCVSaveContextStmtPart( SQLMan_Context& ViewContext )
{
	int Size = ViewContext.GetStatementPartSize();
    void* SavedPart = mContext.GetAllocator().Allocate(Size);
    if ( SavedPart )
	    SAPDB_MemCopyNoCheck (SavedPart, ViewContext.GetStatementPartPtr(), Size);
	return SavedPart;
}

bool Query_QueryGraph::SCVRestoreContextStmtPart( SQLMan_Context& ViewContext, void* SavedPart )
{
    if ( SavedPart )
    {
        SAPDB_MemCopyNoCheck (ViewContext.GetStatementPartPtr(), SavedPart, ViewContext.GetStatementPartSize());
        mContext.GetAllocator().Deallocate(SavedPart);
		return true;
    }
	return false;
}

bool Query_QueryGraph::SCVParseViewStatement(SQLMan_Context& ViewContext, Catalog_ViewDescriptor& ViewDescr )
{
	// Save old stmtpart of the context
	void* SavedContextStmtPart = SCVSaveContextStmtPart(ViewContext);
	tgg00_DateTimeFormat SavedDTFormat = ViewContext.a_dt_format; // a_dt_format is not part of the statement context
	if ( SavedContextStmtPart )
	{
		// Parse View Definition...
		ViewContext.a_ex_kind.becomes(only_syntax);
		ViewContext.a_sqlmode.becomes(ViewDescr.GetSqlMode());
		ViewContext.a_dt_format.becomes(ViewDescr.GetDateTimeFormat());
		a35_asql_statement(ViewContext);
		SAPDB_Int2 AddLitCount = ViewContext.a_count_literals;

		//  Restore old values...
		SQLMan_SyntaxTreeMembers OrgSTM = ViewContext.GetSyntaxTreeMembers();
		SCVRestoreContextStmtPart(ViewContext, SavedContextStmtPart);
		ViewContext.a_dt_format.becomes(SavedDTFormat);
		ViewContext.a_count_literals += AddLitCount;
		ViewContext.SetSyntaxTreeMembers(OrgSTM);

	    return ViewContext.IsOk();
	}
	return false;
}

bool Query_QueryGraph::SCVCheckIfViewCanBeSubstituted(Query_SyntaxTree& NewSyntaxTree, SyntaxTreeNodeNo& ViewNameNode, SyntaxTreeNodeNo& ViewSelectNode, SyntaxTreeNodeNo& ColNameNodes )
{
    // So far either number of columns given by a16/3 must match number of columns given by a60/1 or there is no a16/3 node.

    SyntaxTreeNodeNo Node = NewSyntaxTree[0].n_lo_level;
    if ( (NewSyntaxTree[Node].n_proc == a16) && (NewSyntaxTree[Node].n_subproc() == cak_i_view) )
    {
        ViewNameNode = Node = NewSyntaxTree[Node].n_lo_level;    // points to the Symb tabl witch gives the viewname
		if ( (NewSyntaxTree[Node].n_proc == no_proc) &&
		     (NewSyntaxTree[Node].n_symb == s_authid) )
			ViewNameNode = Node = NewSyntaxTree[Node].n_sa_level;

        Node = NewSyntaxTree[Node].n_lo_level;
        if ( (NewSyntaxTree[Node].n_proc == a16) &&
            (NewSyntaxTree[Node].n_subproc() == cak_x_column_list) )
        {
			SyntaxTreeNodeNo Help = Node;
            Node = NewSyntaxTree[Help].n_lo_level;
            NewSyntaxTree[Help].n_lo_level = 0;

			ColNameNodes = NewSyntaxTree[Help].n_sa_level;
			NewSyntaxTree[Help].n_sa_level = 0;
        }
        // Node now points to a631/-2 or a63/cak_x_start_union

        SCVMoveSubqueryInformation(NewSyntaxTree, NewSyntaxTree[Node].n_sa_level);

        // set ViewName Node as alias for the "subquery"
        ViewSelectNode = Node;
        NewSyntaxTree[ViewSelectNode].n_sa_level = ViewNameNode;
        NewSyntaxTree[ViewNameNode  ].n_proc.becomes(no_proc);
        NewSyntaxTree[ViewNameNode  ].n_subproc() = 0;
        NewSyntaxTree[ViewNameNode  ].n_symb.becomes(s_identifier);
        NewSyntaxTree[ViewNameNode  ].n_sa_level = 0;
        NewSyntaxTree[ViewNameNode  ].n_lo_level = 0;
        NewSyntaxTree[ViewNameNode  ].n_refback  = ViewSelectNode;

		return true;
    }

	SetError("CheckIfViewCanBeSubstituted");
//	NewSyntaxTree.PrintToTrace();
	return false;
}

bool Query_QueryGraph::SCVMoveSubqueryInformation(Query_SyntaxTree& NewSyntaxTree, SyntaxTreeNodeNo StartNode )
{
    if ( StartNode > 0 )
    {
        SyntaxTreeNodeNo N = NewSyntaxTree[StartNode].n_pos();
        SAPDBERR_ASSERT_STATE( N > 0 );

        bool x = NewSyntaxTree[StartNode].n_proc == a92fromsel    ||  
                (NewSyntaxTree[StartNode].n_proc == a63 && 
                 ( NewSyntaxTree[StartNode].n_subproc() == cak_x_subquery || 
                   NewSyntaxTree[StartNode].n_subproc() == cak_x_one_val_subquery || 
                   NewSyntaxTree[StartNode].n_subproc() == cak_x_arith_with_subquery ) );

        if ( !x || !(N>0) )
            return false;

        if ( !(NewSyntaxTree[N].n_proc == a66 && NewSyntaxTree[N].n_subproc() == cak_x_select_in_from_part) )
        {
            NewSyntaxTree[N].n_querycheck.addElement(qcn_SubQInfo);
            NewSyntaxTree[N].n_length() = NewSyntaxTree[StartNode].n_subproc();
        }
        else
        {
            SAPDBERR_ASSERT_STATE( NewSyntaxTree[StartNode].n_proc == a92fromsel );
        }

        SCVMoveSubqueryInformation(NewSyntaxTree, NewSyntaxTree[StartNode].n_sa_level);
        SCVMoveSubqueryInformation(NewSyntaxTree, NewSyntaxTree[StartNode].n_lo_level);
    }
    return true;
}

bool Query_QueryGraph::SCVLinkSyntaxTrees(SyntaxTreeNodeNo FromTabNode, SyntaxTreeNodeNo ViewSyntaxTreeNode, tak_ap_node* OrgViewNameNode, bool ReferencedViaSynonym)
{
    SyntaxTreeNodeNo Node  = mSyntaxTree[FromTabNode].n_lo_level;
    SyntaxTreeNodeNo ToBeRemoved = Node;

    while ( Node > 0 )
    {
		if ( ReferencedViaSynonym )
		{
			if ( (mSyntaxTree[Node].n_proc == no_proc) &&
				(mSyntaxTree[Node].n_symb == s_tablename) ) // we have to use the synonym name
			{
				mSyntaxTree[Node].n_pos()    = OrgViewNameNode->n_pos();
				mSyntaxTree[Node].n_length() = OrgViewNameNode->n_length();
			}
		}
        Node = mSyntaxTree[Node].n_sa_level;
    }

    mSyntaxTree.SetUnusedNode(ToBeRemoved, true);
    mSyntaxTree[FromTabNode        ].n_lo_level = ViewSyntaxTreeNode;
    mSyntaxTree[ViewSyntaxTreeNode ].n_refback  = FromTabNode;

    // Change a63/cak_x_mass_select to a63query_spec/-2 if necessary
    if ( (mSyntaxTree[ViewSyntaxTreeNode].n_proc == a63) &&
        (mSyntaxTree[ViewSyntaxTreeNode].n_subproc() == cak_x_mass_select) )
    {
        mSyntaxTree[ViewSyntaxTreeNode].n_proc.becomes(a63query_spec);
        mSyntaxTree[ViewSyntaxTreeNode].n_subproc()  = -2;
    }

    // Change type ComplexView.SyntaxNode from a66/cak_x_table_spec to a66/cak_x_select_in_from_part
    mSyntaxTree[FromTabNode].n_subproc() = cak_x_select_in_from_part;
//    mSyntaxTree[FromTabNode].n_pos()     = FromSelectsCnt(); // muß per nachbrenner erledigt werden !

    return true;
}

bool Query_QueryGraph::SubstituteComplexView( Query_ComplexViewInfo& ComplexView, SyntaxTreeNodeNo& ColNameNodes )
{
    //SetError("Cannot substitute ComplexView");
    //return false;

    // Note: This method is called _before_ CreateNodes !!!
    //       Therefore anything has to be done on Syntax(AP)Tree-Level only !!!
    ///

    SAPDBTRACE_METHOD_DEBUG ("Query_QueryGraph::SubstituteComplexView", QueryRewrite_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( ComplexView.SyntaxNode != 0 );
	SAPDBERR_ASSERT_STATE( mContext.SQLManContext().IsOk() );

    SQLMan_Context& ViewContext = mContext.SQLManContext();

	// Create ViewDescriptor
	Catalog_ObjectId ViewID = ComplexView.TabID;
	Catalog_ViewDescriptor ViewDescr(ViewContext, ViewID);
	if ( ViewDescr )
	{
		SAPDB_Int4 ViewStmtLength = ViewDescr.GetDefinitionLength(false);
		if ( SCVCreateInternalPacket(ViewContext, ViewStmtLength) )				// Create InternalPacket for parsing the view
		{
			void* Dest = (void*)ViewContext.a_cmd_part->sp1p_buf();
			if ( ViewDescr.CopyDefinition (false, true, Dest, ViewStmtLength) )	// Copy the ViewStatement into the InternalPacket
			{
				// Save old syntax tree and create a new one
				SQLMan_SyntaxTreeMembers OrgSTM = ViewContext.GetSyntaxTreeMembers();
				ViewContext.a_ap_tree = 0; // to insure NewSyntaxTree works...
				ViewContext.NewSyntaxTree(8192); // DDT : should be smaller but due to old pascal coding
												 // we have to insure that no reallocate occurs during parsing...

				if ( SCVParseViewStatement(ViewContext, ViewDescr ) )	// Parse ViewStatement
				{
					SQLMan_SyntaxTreeMembers ViewSTM = ViewContext.GetSyntaxTreeMembers();

					// Check if replacement is possible
					Query_SyntaxTree ViewSyntaxTree(ViewContext);
					SyntaxTreeNodeNo ViewSelectNode, ViewNameNode;
					if ( SCVCheckIfViewCanBeSubstituted(ViewSyntaxTree, ViewNameNode, ViewSelectNode, ColNameNodes) )
					{
						// Length of the original Statement
						SAPDB_Int4 PosOffset = mContext.GetSQLStatementSize();

						tak_ap_node* OrgViewNameNode = 0;
						if ( ViewNameNode>0 )
							OrgViewNameNode = &(ViewSyntaxTree[ViewNameNode]);

						// Copy SyntaxTree into own SyntaxTree array...
						ViewContext.SetSyntaxTreeMembers(OrgSTM);

						SyntaxTreeNodeNo ViewSyntaxTreeNode = mSyntaxTree.DoubleTreePart(&(mContext.GetAllocator()), (tak_ap_node*)ViewSTM.SyntaxTreePtr, ViewSelectNode, PosOffset, 0, /*MapNodes*/true, /*WithSameLevel*/true);
						if ( ColNameNodes > 0 )
							ColNameNodes = mSyntaxTree.DoubleTreePart(&(mContext.GetAllocator()), (tak_ap_node*)ViewSTM.SyntaxTreePtr, ColNameNodes, PosOffset, 0, /*MapNodes*/false, /*WithSameLevel*/true);
						ViewContext.GetSyntaxTreeMembers(OrgSTM); // Save the modified syntax tree as original one
						if ( ViewSyntaxTreeNode > 0 )
						{
							// Append View Statement to SQLStatement...
							void* ToAdd             = ViewContext.GetSQLStatementPtr();
							SAPDB_Int4  AddLength   = ViewContext.GetSQLStatementSize();
							if ( mContext.AppendToSQLStatement(ToAdd, AddLength) )
							{
								ViewContext.SetSyntaxTreeMembers(OrgSTM);
#if defined(SAPDB_SLOW)
								Dest = (void*)ViewContext.GetSQLStatementPtr();	// For debugging...
#endif
								// Link both SyntaxTrees
								if ( SCVLinkSyntaxTrees(ComplexView.SyntaxNode, ViewSyntaxTreeNode, OrgViewNameNode, ComplexView.ReferencedViaSynonym) )
								{
									// Set backwards references for the new syntaxtree part
									mSyntaxTree.SetRefBack(ComplexView.SyntaxNode);

//                                    if ( AddSubSelectNode(ComplexView.SyntaxNode, P_FromPart, cak_x_subquery, false) )    // Add FromSelectNode
  									{
  										// Delete the new syntax tree and restore the modified new one...
  										ViewContext.SetSyntaxTreeMembers(ViewSTM);
  										ViewContext.DeleteSyntaxTree();
  										ViewContext.SetSyntaxTreeMembers(OrgSTM);
  										//Release the packet that we don't need any longer
  										SCVReleaseInternalPacket(ViewContext);
  										ComplexView.SyntaxNode = 0;
  										ViewContext.a_from_select = true;
  										return true;
  									} // AddFromSelectNode
								} // LinkSyntaxTrees
							} // AppendSQLStatement
						} // DuplicateTreePart
						ViewContext.SetSyntaxTreeMembers(ViewSTM);
					} // Check if view can be substituted
				} // Parse ViewStatement
				// Restore old syntax tree and statement part of the context
				ViewContext.DeleteSyntaxTree();
				ViewContext.SetSyntaxTreeMembers(OrgSTM);
			} // if ( ViewDescr.CopyDefinition )
			SCVReleaseInternalPacket(ViewContext); //Release the packet that we don't need any longer
		} // Create Internal Packet
	} // if ( ViewDescr )

	SetError("SubsituteComplexView");
    mContext.SQLManContext().ResetReturnCode(); // DDT : Reset in any case???
    return false;
}

// ------------------------------------------------------------------------------------------------

bool Query_QueryGraph::AddExpressionRefName( SyntaxTreeNodeNo Node, SAPDB_Int2& ExprCount, SQLMan_Identifier& ExprName )
{
	if ( mSyntaxTree[Node].n_symb != s_rowno )
	{
		SyntaxTreeNodeNo RefNode = mSyntaxTree.GetFreeNode();

		if ( RefNode > 0 )
		{
			tak00_columninfo akColInfo;
			a61_new_expr_no(akColInfo, ExprCount);
			ExprName = SQLMan_Identifier(akColInfo.ccolumnn()); 
			SAPDB_Int4 Pos = mContext.AppendToSQLStatement((void*)akColInfo.ccolumnn(), akColInfo.ccolumnn_len());
			if ( Pos > 0 )
			{
				// fill the new node
				mSyntaxTree[RefNode].n_symb.becomes(s_reference_name);
				mSyntaxTree[RefNode].n_pos() = Pos + 1; // +1 due to the pascal coding...
				mSyntaxTree[RefNode].n_length() = akColInfo.ccolumnn_len();

				// link nodes
				SyntaxTreeNodeNo PrevNode = mSyntaxTree[Node].n_refback;
				mSyntaxTree[PrevNode].n_lo_level = RefNode;
				mSyntaxTree[RefNode ].n_sa_level = Node;
				mSyntaxTree[RefNode ].n_refback  = PrevNode;
				mSyntaxTree[Node    ].n_refback  = RefNode;

				return true;
			}
			mSyntaxTree.SetUnusedNode(RefNode);
		}
		return false;
	}
	return true;
}

// ------------------------------------------------------------------------------------------------
