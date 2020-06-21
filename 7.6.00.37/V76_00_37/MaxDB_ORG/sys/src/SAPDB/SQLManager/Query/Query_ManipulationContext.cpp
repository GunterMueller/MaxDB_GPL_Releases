/*!***************************************************************************

    @file     Query_ManipulationContext.cpp
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

#include "SAPDB/SQLManager/Query/Query_ManipulationContext.hpp"
#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"

#include "hak542.h"
#include "hak99.h"

#include "SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"

// ------------------------------------------------------------------------------------------------

Query_ManipulationContext::Query_ManipulationContext( SQLMan_Context& SQLManContext, SharedSQL_PrepareHandle* prepareHandle )
    : mSQLManContext(SQLManContext)
#if defined( SAPDB_SLOW )
	, mRawAllocator(0)
#endif
	, mPrepareHandle(prepareHandle)
    , mVTrace()
	, mOldContextStmtPart(0)
    , mSQLStatementBuffer(0)
    , mSQLStatementBufferMaxSize(0)
    , mSQLStatementBufferUsedSize(0)
    , mSQLStatementBufferChanges(false)
    , mDecision(false)
    , mError(false)
	, mNotYetImplemented(false)
{
    SAPDBERR_ASSERT_STATE(  SQLManContext.a_ap_tree == SQLManContext.a_session_ap_tree );

#if defined( SAPDB_SLOW )
	mRawAllocator = new (mSQLManContext.GetAllocator()) 
							SAPDBMem_RawAllocator( (const SAPDB_UTF8 *) "QueryRewriteAllocator",
								                    mSQLManContext.GetAllocator(),
									                128*1024,             // FirstBlockSize
										             64*1024,             // SupplementBlockSize
											        SAPDBMem_RawAllocator::NOT_FREE_RAW_EXTENDS
												 );
	SAPDBERR_ASSERT_STATE( mRawAllocator );
	if ( mRawAllocator != 0 )
	{
		SAPDB_ULong BytesUsed, MaxBytesUsed, BytesControlled;
		mRawAllocator->CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
		mSavedBytesUsed = BytesUsed;
	}
#endif 

    CreateNewSyntaxTreeAndStatementBuffer();
}

Query_ManipulationContext::~Query_ManipulationContext( void )
{
//    SAPDBERR_ASSERT_STATE( mDecision );

    if ( !mDecision )
        RestoreOldSyntaxTreeAndStatementBuffer();

#if defined( SAPDB_SLOW )
	if ( mRawAllocator )
	{
		mRawAllocator->CheckConsistency();
		SAPDB_ULong BytesUsed, MaxBytesUsed, BytesControlled;
		mRawAllocator->CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
		if ( mSavedBytesUsed != BytesUsed )     // just for debugging
		{
			SAPDBERR_ASSERT_STATE( mSavedBytesUsed == BytesUsed );
		}
		destroy(mRawAllocator, mSQLManContext.GetAllocator());
	}
	mSQLManContext.GetAllocator().CheckConsistency();
#endif
}

SAPDBMem_RawAllocator& Query_ManipulationContext::GetAllocator( void ) const
{
#if defined( SAPDB_SLOW )
	if ( mRawAllocator )
		return *mRawAllocator;
#endif
	return mSQLManContext.GetAllocator();
}

void* Query_ManipulationContext::GetSQLStatementPtr( void ) const
{
    if ( mSQLStatementBuffer )
        return mSQLStatementBuffer;
    else
        return mSQLManContext.GetSQLStatementPtr();
}

int Query_ManipulationContext::GetSQLStatementSize( void ) const
{
    if ( mSQLStatementBuffer )
        return mSQLStatementBufferUsedSize;
    else
        return mSQLManContext.GetSQLStatementSize();
}

bool Query_ManipulationContext::AppendBlankToSQLStatement( void )
{
    if ( g01unicode )
        return (AppendToSQLStatement((char*)csp_unicode_blank, 2)>=0);
    else
        return (AppendToSQLStatement((char*)csp_ascii_blank, 1)>=0);
}

SAPDB_Int4 Query_ManipulationContext::AppendToSQLStatement( char* ToAdd )
{
    SAPDB_Int4 Len = (SAPDB_Int4) strlen(ToAdd);
    return AppendToSQLStatement(ToAdd, Len);
}

SAPDB_Int4 Query_ManipulationContext::AppendToSQLStatement( void* ToAdd, SAPDB_Int4 AddSize )
{
    SAPDBERR_ASSERT_ARGUMENT( ToAdd && (AddSize>0) );

    SAPDB_Int4 RequiredSize = mSQLStatementBufferUsedSize + AddSize;
    if ( RequiredSize > mSQLStatementBufferMaxSize )
    {
        if ( ! ExtendSQLStatementBuffer(RequiredSize) )
            return -1;
    }
    SAPDB_Int4 pos = mSQLStatementBufferUsedSize;
    SAPDB_Byte* Position = mSQLStatementBuffer + pos;
    SAPDB_MemCopyNoCheck(Position, ToAdd, AddSize);
    mSQLStatementBufferUsedSize += AddSize;
    mSQLStatementBufferChanges = true;
    return pos;
}

bool Query_ManipulationContext::ExtendSQLStatementBuffer( SAPDB_Int4 RequiredSize )
{
    SAPDBERR_ASSERT_ARGUMENT( RequiredSize>0 );

    SAPDB_Int2 BlockSize    = 8192;
    SAPDB_Int4 Blocks       = (RequiredSize/BlockSize)
                            + (RequiredSize%BlockSize?1:0);
    SAPDB_Int4 NewSize      = Blocks * BlockSize;

    if ( NewSize > mSQLStatementBufferMaxSize )
    {
        SAPDB_Byte* NewBuffer = (SAPDB_Byte*) GetAllocator().Reallocate(NewSize, mSQLStatementBuffer);
        if ( NewBuffer )
        {
            mSQLStatementBuffer         = NewBuffer;
            mSQLStatementBufferMaxSize  = NewSize;
        }
    }
    return ( mSQLStatementBuffer!=0 );
}

bool Query_ManipulationContext::CreateNewSyntaxTreeAndStatementBuffer( void )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_ManipulationContext::CreateNewSyntaxTreeAndStatementBuffer", QueryRewrite_Trace, 5);

#if defined(SAPDB_SLOW)
    if ( QueryRewrite_Trace.TracesLevel(1) )
    {
        mVTrace << "Query_ManipulationContext : Original SyntaxTree" << FlushLine;
        a99print_nodes(*mSQLManContext.a_ap_tree, mSQLManContext.a_scv_index, mSQLManContext.a_first_hint_node);
    }
#endif

    // Save the old statement context
	int Size = mSQLManContext.GetStatementPartSize();
	mOldContextStmtPart = GetAllocator().Allocate(Size);
	if ( mOldContextStmtPart )
		SAPDB_MemCopyNoCheck (mOldContextStmtPart, mSQLManContext.GetStatementPartPtr(), Size);
	else
	{
		mError = true;
		return false;
	}

	tak_ap_node* OldAPTree = (tak_ap_node*) mSQLManContext.a_ap_tree;
	SyntaxTreeNodeNo OldFirstHintNode = mSQLManContext.a_first_hint_node;

    // Create new syntax tree
	mSQLManContext.a_ap_tree			= 0; // to insure NewSyntaxTree works...
    mSQLManContext.NewSyntaxTree(256);
    mSQLManContext.a_scv_index          = 0;
    mSQLManContext.a_maxnode            = 0;
    mSQLManContext.a_ap_max_node        = 0;
    mSQLManContext.a_first_hint_node    = csp_minint2;

    Query_SyntaxTree ActSyntaxTree = Query_SyntaxTree(mSQLManContext);

    // Copy old syntax tree
    Query_NodeMapper Mapper = Query_NodeMapper(GetAllocator());
    ActSyntaxTree[0] = OldAPTree[0];
    SyntaxTreeNodeNo NewFirstNode = ActSyntaxTree.DoubleTreePart(&GetAllocator(), OldAPTree, OldAPTree[0].n_lo_level, 0, &Mapper, /*MapNodes*/false, /*WithSameLevel*/true);
    ActSyntaxTree[0].n_lo_level = NewFirstNode;

    if ( ActSyntaxTree[0].n_lo_level > 0 )
        ActSyntaxTree[ActSyntaxTree[0].n_lo_level].n_refback = 0;
    if ( OldAPTree[0].n_sa_level != 0 )
    {
        NewFirstNode = ActSyntaxTree.DoubleTreePart(&GetAllocator(), OldAPTree, OldAPTree[0].n_sa_level, 0, &Mapper, /*MapNodes*/false, /*WithSameLevel*/true);
        ActSyntaxTree[0].n_sa_level = NewFirstNode;
        if ( ActSyntaxTree[0].n_sa_level > 0 )
            ActSyntaxTree[ActSyntaxTree[0].n_sa_level].n_refback = 0;
        SAPDBERR_ASSERT_STATE( !ActSyntaxTree[0].n_sa_level );
    }
    if ( OldFirstHintNode != csp_minint2 )
    {
        SyntaxTreeNodeNo HintNodes = ActSyntaxTree.DoubleTreePart(&GetAllocator(), OldAPTree, OldFirstHintNode, 0, &Mapper, /*MapNodes*/false, /*WithSameLevel*/true);
        mSQLManContext.a_first_hint_node = HintNodes;
    }

    ActSyntaxTree.MapSubSelectNodes(&Mapper);
    if ( mSQLManContext.a_select_node > 0 )
    {
        NewFirstNode = Mapper.GetDestNodeFromSrcNode(mSQLManContext.a_select_node);
        mSQLManContext.a_select_node = NewFirstNode;
    }

    // Create a temporary statement buffer
    void* ToAdd             = mSQLManContext.GetSQLStatementPtr();
    SAPDB_Int4  AddLength   = mSQLManContext.GetSQLStatementSize();
    if ( AppendToSQLStatement(ToAdd, AddLength) < 0 )
        mError = true;
    mSQLStatementBufferChanges = false;
    mSQLManContext.a_queryrewrite_modified = true;
	return true;
}

bool Query_ManipulationContext::RestoreOldSyntaxTreeAndStatementBuffer( void )
{
	SAPDBTRACE_METHOD_DEBUG ("Query_ManipulationContext::RestoreOldSyntaxTreeAndStatementBuffer", QueryRewrite_Trace, 5);

	if ( !mOldContextStmtPart )
		return true;

    // Delete new syntax tree
    mSQLManContext.DeleteSyntaxTree();
    // ... and restore complete saved statementpart
	SAPDB_Int2 oldReturnCode = mSQLManContext.a_returncode;
    SAPDB_MemCopyNoCheck (mSQLManContext.GetStatementPartPtr(), mOldContextStmtPart, mSQLManContext.GetStatementPartSize());
    GetAllocator().Deallocate(mOldContextStmtPart);
	mSQLManContext.a_returncode = oldReturnCode;
    mOldContextStmtPart = 0;

    // Restore old StatementBuffer
    // ... there is nothing to do but to delete the new one
    if ( mSQLStatementBuffer )
    {
        GetAllocator().Deallocate(mSQLStatementBuffer);
        mSQLStatementBuffer = 0;
    }

    if ( QueryRewrite_Trace.TracesLevel(1) )
    {
        mVTrace << "Query_ManipulationContext : Restore old context" << FlushLine;
#if defined(SAPDB_SLOW)
        if ( QueryRewrite_Trace.TracesLevel(2) )
            a99print_nodes(*mSQLManContext.a_ap_tree, mSQLManContext.a_scv_index, mSQLManContext.a_first_hint_node);
#endif
    }

	// in the end we have to clear the tableinfo added within Query_TableNode::Semantics
	if ( mPrepareHandle )
		mPrepareHandle->ClearTables();

	mSQLManContext.a_queryrewrite_modified = false;
    mSQLManContext.a_queryrewrite_exec.clear();
	mSQLManContext.a_queryrewrite_done.becomes(No_Rewrite);
    if ( QueryRewrite_Trace.TracesLevel(2) )
	{
		// MessageListe ausgeben... !
	}
	mSQLManContext.SetTransInfoWarning(warn9_stmt_optimized,	/*on=*/false);
	mSQLManContext.SetTransInfoWarning(warn10_views_substituted,/*on=*/false);
#if defined(SAPDB_SLOW)
#if defined(QRW_FIND_ROLLBACK_BY_ERROR)
	if ( mError )				// only in case of error not in case of NotYetImplemented !
		mSQLManContext.SetTransInfoWarning(warn15_user_defined_code,/*on=*/true );
#endif
#if defined(QRW_FIND_ROLLBACK_BY_NOTYETIMPLEMENTED)
	if ( mNotYetImplemented )	// the other way around ...
		mSQLManContext.SetTransInfoWarning(warn15_user_defined_code,/*on=*/true );
#endif
#endif
// DDT ???	mSQLManContext.ResetReturnCode();
    return true;
}

bool Query_ManipulationContext::KeepNewSyntaxTreeAndStatementBuffer( void )
{
#if defined(QRW_FIND_ROLLBACK_BY_ERROR) || defined(QRW_FIND_ROLLBACK_BY_NOTYETIMPLEMENTED)
	// restore the old context instead of keeping it BUT reset the warning
	// therefore SUTs will run without modified APTree but there will be a warning
	// if there was a REAL rollback do to QRW
	RestoreOldSyntaxTreeAndStatementBuffer();
	mSQLManContext.a_queryrewrite_mode.becomes(No_Rewrite);
    return false;
#else

	SAPDBERR_ASSERT_STATE( mSQLManContext.IsOk() );

    // Keep new syntax tree
    // ... there is nothing to do here

    // Keep new statement buffer if necessary

    if (   mSQLStatementBufferChanges 
		&& mSQLManContext.IsOk() )
    {
        // ... create a new internal packet and copy the temporary statement buffer

		SAPDB_Int4 AddSize = 0;
		if ( mSQLManContext.a_return_segm->sp1r_function_code() == csp1_explain_fc )
			AddSize = (mSQLManContext.a_cmd_part->sp1p_buf_size() - mSQLManContext.a_cmd_part->sp1p_buf_len());

        a542internal_packet(mSQLManContext, /*release_internal_packet*/false, mSQLStatementBufferUsedSize+AddSize);
        if ( mSQLManContext.IsOk() ) // DDT
        {
            void* Dest = mSQLManContext.a_cmd_part->sp1p_buf();
            SAPDB_MemCopyNoCheck(Dest, mSQLStatementBuffer, mSQLStatementBufferUsedSize);
            mSQLManContext.a_cmd_part->sp1p_buf_len() = mSQLStatementBufferUsedSize;
            mSQLManContext.a_cmd_segm->sp1c_prepare() = mSQLManContext.a_initial_segment_header.sp1c_prepare();
            mSQLManContext.a_cmd_segm->sp1c_producer().becomes(sp1pr_queryrewrite);
			mSQLManContext.a_cmd_segment_header       = mSQLManContext.a_cmd_segm->sp1s_segm_header();
            mSQLManContext.a_cmd_segment_header.sp1c_producer().becomes(sp1pr_queryrewrite);
        }
        else
        {
            //mSQLManContext.ResetReturnCode();
            mError = true;
            RestoreOldSyntaxTreeAndStatementBuffer();
        }
    }
    if ( mSQLStatementBuffer )	// we have to delete the tempary buffer in any case
        GetAllocator().Deallocate(mSQLStatementBuffer);

    if ( mOldContextStmtPart )
        GetAllocator().Deallocate(mOldContextStmtPart); // DDT ?

#if defined(SAPDB_SLOW)
    if ( !mError )
    {
        if ( QueryRewrite_Trace.TracesLevel(1) )
        {
            mVTrace << "Query_ManipulationContext : Keep new Context" << FlushLine;
            if ( QueryRewrite_Trace.TracesLevel(1) )
                a99print_nodes(*mSQLManContext.a_ap_tree, mSQLManContext.a_scv_index, mSQLManContext.a_first_hint_node);
        }
    }
#endif
#endif
// DDT ???	mSQLManContext.ResetReturnCode();
    return true;
}


