/*!***************************************************************************

    @file     Query_SyntaxTree.cpp
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

#include "SAPDB/SQLManager/Query/Query_SyntaxTree.hpp"
#include "SAPDB/SQLManager/Query/Query_NodeSelect.hpp"

#include "SAPDB/RunTime/RTE_CurrentStack.hpp"

#include "hak01.h"
#include "hak99.h"

// ------------------------------------------------------------------------------------------------

Query_SyntaxTree::Query_SyntaxTree( SQLMan_Context& Context )
    : mContext(Context)
    , mFirstUnusedNode(cak_qrewrite_ref_init)
    , mFreeNodeCount(0)
    , mError(false)
{
    mAPTreePtr = (tak_ap_node*) mContext.a_ap_tree;
}

Query_SyntaxTree& Query_SyntaxTree::operator= ( const Query_SyntaxTree& T )
{
    SAPDB_MemCopyNoCheck(this, &T, sizeof(*this));
    return *this;
}

void Query_SyntaxTree::PrintToTrace( void )
{
#if defined(SAPDB_SLOW)
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::PrintToTrace", QueryRewrite_Trace, 8);
    if ( QueryRewrite_Trace.TracesLevel(2) )
        a99print_nodes(*mContext.a_ap_tree, mContext.a_scv_index, mContext.a_first_hint_node);
#endif
}

// -------------------

SyntaxTreeNodeNo Query_SyntaxTree::GetFreeNode( bool ForcePutNode /*=false*/ )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::GetFreeNode", QueryRewrite_Trace, 6);
	SAPDBERR_ASSERT_STATE( mContext.IsOk() );

    SyntaxTreeNodeNo NodeNo = mFirstUnusedNode;
    if ( NodeNo > 0 && !ForcePutNode )
    {   // there are unused nodes that can be used
        mFirstUnusedNode = mAPTreePtr[NodeNo].n_refback;
        mAPTreePtr[NodeNo].n_sa_level = 0;
        mAPTreePtr[NodeNo].n_lo_level = 0;
        mAPTreePtr[NodeNo].n_refback  = 0;
        --mFreeNodeCount;
    }
    else
    {
        a01_call_put_queryrewrite(mContext, NodeNo);
        if ( mContext.ReturnCode() )
        {
            NodeNo = cak_qrewrite_ref_init;
            mContext.ResetReturnCode();			// DDT : Reset in any case???
        }
        if ( (tak_ap_node*)mContext.a_ap_tree != mAPTreePtr )
            mAPTreePtr = (tak_ap_node*)mContext.a_ap_tree;
    }
    return NodeNo;
}

// -------------------

bool Query_SyntaxTree::CheckNodes( void  )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::CheckNodes", QueryRewrite_Trace, 6);

    return InternCheckNodes(0); 
}

bool Query_SyntaxTree::InternCheckNodes( SyntaxTreeNodeNo NodeNo )
{
    if ( !CheckSingleNode(NodeNo) )
        return false;

    if ( mAPTreePtr[NodeNo].n_sa_level > 0 )
    {
        if ( !InternCheckNodes(mAPTreePtr[NodeNo].n_sa_level) )
            return false;
    }
    else
    {
        if ( mAPTreePtr[NodeNo].n_sa_level < 0 )
        {
#if defined(SAPDB_SLOW)
            if ( QueryRewrite_Trace.TracesLevel(1) )
                Kernel_VTrace() << "Error 01 in Node " << NodeNo << FlushLine;
#endif
            return false;
        }
    }

    if ( mAPTreePtr[NodeNo].n_lo_level > 0 )
    {
        if ( !InternCheckNodes(mAPTreePtr[NodeNo].n_lo_level) )
            return false;
    }
    else
    {
        if ( mAPTreePtr[NodeNo].n_lo_level < 0 )
        {
#if defined(SAPDB_SLOW)
            if ( QueryRewrite_Trace.TracesLevel(1) )
                Kernel_VTrace() << "Error 02 in Node " << NodeNo << FlushLine;
#endif
            return false;
        }
    }

    return true;
}

bool Query_SyntaxTree::CheckSingleNode( SyntaxTreeNodeNo NodeNo )
{
    SAPDBERR_ASSERT_ARGUMENT( NodeNo>=0 );

    if ( NodeNo >= 0 )
    {
        if ( NodeNo != 0 && mAPTreePtr[NodeNo].n_refback < 0 )
        {
#if defined(SAPDB_SLOW)
            if ( QueryRewrite_Trace.TracesLevel(1) )
                Kernel_VTrace() << "Error 11 in Node " << NodeNo << FlushLine;
#endif
            return false;
        }

        if ( mAPTreePtr[NodeNo].n_lo_level < 0 || mAPTreePtr[NodeNo].n_lo_level < 0 )
        {
#if defined(SAPDB_SLOW)
            if ( QueryRewrite_Trace.TracesLevel(1) )
                Kernel_VTrace() << "Error 12 in Node " << NodeNo << FlushLine;
#endif
            return false;
        }

	    SyntaxTreeNodeNo Sa = mAPTreePtr[NodeNo].n_sa_level;
	    if ( Sa > 0 )
		    if ( mAPTreePtr[Sa].n_refback != NodeNo )
            {
#if defined(SAPDB_SLOW)
                if ( QueryRewrite_Trace.TracesLevel(1) )
                    Kernel_VTrace() << "Error 13 in Node " << NodeNo << FlushLine;
#endif
                return false;
            }

	    SyntaxTreeNodeNo Lo = mAPTreePtr[NodeNo].n_lo_level;
	    if ( Lo > 0 )
		    if ( mAPTreePtr[Lo].n_refback != NodeNo )
            {
#if defined(SAPDB_SLOW)
                if ( QueryRewrite_Trace.TracesLevel(1) )
                    Kernel_VTrace() << "Error 14 in Node " << NodeNo << FlushLine;
#endif
                return false;
            }
    }

    return true;
}

SyntaxTreeNodeNo Query_SyntaxTree::DoubleTreePart(  SAPDBMem_IRawAllocator* Alloc, tak_ap_node* SourceSyntaxTree, SyntaxTreeNodeNo StartNode, SAPDB_Int4 PosOffset, Query_NodeMapper* pMap, bool MapNodes, bool WithSameLevel )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::DoubleTreePart", QueryRewrite_Trace, 7);

    if ( !pMap && MapNodes )
        mpMapper = new(*Alloc) Query_NodeMapper(mContext.GetAllocator());
    else
        mpMapper = pMap;

    SyntaxTreeNodeNo DestRoot = DoubleTreePartInternal(SourceSyntaxTree, StartNode, PosOffset, WithSameLevel );
    if ( MapNodes )
        MapSubSelectNodes(mpMapper);

    if ( !pMap && mpMapper )
        destroy(mpMapper, *Alloc);

    return DestRoot;
}

void Query_SyntaxTree::MapSubSelectNodes( Query_NodeMapper* pMap )
{
    if ( pMap )
    {
        SAPDB_Int2 n = pMap->GetSize();
        for (SAPDB_Int2 i = 0; i<n; i++)
        {
            SyntaxTreeNodeNo node = pMap->GetDestNodeFromPos(i);
            if ( ( mAPTreePtr[node].n_proc == a92fromsel ) ||
                 ( mAPTreePtr[node].n_proc == a63 && mAPTreePtr[node].n_subproc() == cak_x_subquery )            ||
                 ( mAPTreePtr[node].n_proc == a63 && mAPTreePtr[node].n_subproc() == cak_x_one_val_subquery )    ||
                 ( mAPTreePtr[node].n_proc == a63 && mAPTreePtr[node].n_subproc() == cak_x_arith_with_subquery ) ||
                 ( mAPTreePtr[node].n_proc == a80 && mAPTreePtr[node].n_subproc() == cak_x_select_hint ) || 
                 ( mAPTreePtr[node].n_proc == a80 && mAPTreePtr[node].n_subproc() == cak_x_update_hint ) || 
                 ( mAPTreePtr[node].n_proc == a80 && mAPTreePtr[node].n_subproc() == cak_x_delete_hint ) ) 
				 // this if-statement must match with Query_SyntaxTree::NeedsMapping 
            {
                SyntaxTreeNodeNo Target   = pMap->GetDestNodeFromSrcNode(mAPTreePtr[node].n_pos());
                mAPTreePtr[node].n_pos()  = Target;
                if ( mAPTreePtr[node].n_proc == a92fromsel )
                    mAPTreePtr[node].n_subproc() = Target;
            }
        }
    }
}

SyntaxTreeNodeNo Query_SyntaxTree::DoubleTreePartInternal( tak_ap_node* SourceSyntaxTree, SyntaxTreeNodeNo StartNode, SAPDB_Int4 PosOffset, bool WithSameLevel )
{	// DDT : RECURSIVE
    if ( mError )
       return cak_qrewrite_ref_init;

    {
        SAPDB_Int4 StackFree = RTE_CurrentStackFree();

        if ( StackFree < 512 )
        {
            SetError();
            return cak_qrewrite_ref_init;
        }
    }

    tak_ap_node* SourceAPTreePtr = SourceSyntaxTree;

    SyntaxTreeNodeNo NewRootNode = GetFreeNode(SourceAPTreePtr[StartNode].n_special.includes(ni_info_node));
    if ( NewRootNode > 0 )
    {
        mAPTreePtr[NewRootNode] = SourceAPTreePtr[StartNode];
		if ( PosOffset != 0 && !WithOutPositionOffset(NewRootNode) )
	        mAPTreePtr[NewRootNode].n_pos() += PosOffset;

		if ( mpMapper )
        {
            if ( NeedsMapping(NewRootNode) )
                mpMapper->AddMappingInfo(StartNode, NewRootNode);
        }

        if ( SourceAPTreePtr[StartNode].n_special.includes(ni_info_node) )
        {
            SyntaxTreeNodeNo NewInfoNode = GetFreeNode(true);
            if ( NewInfoNode > 0 )
            {
                SyntaxTreeNodeNo OldInfoNode=StartNode+1;
                if ( SourceAPTreePtr[OldInfoNode].n_special.includes(ni_skip_node) )
                    OldInfoNode = SourceAPTreePtr[OldInfoNode].n_pos();
                mAPTreePtr[NewInfoNode] = SourceAPTreePtr[OldInfoNode];
            }
            else
            {
                return cak_qrewrite_ref_init;
            }
        }

        if ( SourceAPTreePtr[StartNode].n_lo_level > 0 )
        {
            SyntaxTreeNodeNo Lo = DoubleTreePartInternal(SourceSyntaxTree, SourceAPTreePtr[StartNode].n_lo_level, PosOffset, true);
            if ( Lo > 0 )
            {
                mAPTreePtr[NewRootNode].n_lo_level = Lo;
                mAPTreePtr[Lo         ].n_refback  = NewRootNode;
            }
            else
            {
                mAPTreePtr[NewRootNode].n_lo_level = 0;
                mError = true;
                return cak_qrewrite_ref_init;
            }
        }

        if ( WithSameLevel && SourceAPTreePtr[StartNode].n_sa_level > 0 )
        {
            SyntaxTreeNodeNo Sa = DoubleTreePartInternal(SourceSyntaxTree, SourceAPTreePtr[StartNode].n_sa_level, PosOffset, true);
            if ( Sa > 0 )
            {
                mAPTreePtr[NewRootNode].n_sa_level = Sa;
                mAPTreePtr[Sa         ].n_refback  = NewRootNode;
            }
            else
            {
                mAPTreePtr[NewRootNode].n_sa_level = 0;
                mError = true;
                return cak_qrewrite_ref_init;
            }
        }
        else
        {
            mAPTreePtr[NewRootNode].n_sa_level = 0;
        }

    }
    return NewRootNode;
}

// ------------------------------------------------------------------------------------------------

bool Query_SyntaxTree::ExchangeAnyAndAll( SyntaxTreeNodeNo Node, Query_SelectNode* SelNode )
{
    SAPDBERR_ASSERT_ARGUMENT( Node>0 && SelNode );
    if ( Node > 0 && SelNode )
    {
        SyntaxTreeNodeNo H1,H2;
        if ( GetFirstOperand(Node, H1) && GetSecondOperand(Node, H2) )
        {
            if ( mAPTreePtr[H1].n_proc == no_proc && mAPTreePtr[H1].n_symb==s_any )
            {
                mAPTreePtr[H1].n_symb.becomes(s_all);
                SelNode->SetSubQueryTypeForQuantifier(mAPTreePtr[H1].n_sa_level, SQT_ALL);
            }
            else
                if ( mAPTreePtr[H1].n_proc == no_proc && mAPTreePtr[H1].n_symb==s_all )
                {
                    mAPTreePtr[H1].n_symb.becomes(s_any);
                SelNode->SetSubQueryTypeForQuantifier(mAPTreePtr[H1].n_sa_level, SQT_ANY);
                }

            if ( mAPTreePtr[H2].n_proc == no_proc && mAPTreePtr[H2].n_symb==s_any )
            {
                mAPTreePtr[H2].n_symb.becomes(s_all);
                SelNode->SetSubQueryTypeForQuantifier(mAPTreePtr[H2].n_sa_level, SQT_ALL);
            }
            else
                if ( mAPTreePtr[H2].n_proc == no_proc && mAPTreePtr[H2].n_symb==s_all )
                {
                    mAPTreePtr[H2].n_symb.becomes(s_any);
                    SelNode->SetSubQueryTypeForQuantifier(mAPTreePtr[H2].n_sa_level, SQT_ANY);
                }

            return false;
        }
    }
    return false;
}

// ------------------------------------------------------------------------------------------------
