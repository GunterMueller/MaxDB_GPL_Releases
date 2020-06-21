/*!***************************************************************************

    @file     Query_SyntaxTree.hpp
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

#ifndef QUERY_SYNTAXTREE_H
#define QUERY_SYNTAXTREE_H

// ------------------------------------------------------------------------------------------------

#include "SAPDB/SQLManager/SQLMan_Types.hpp"
#include "SAPDB/SQLManager/Query/Query_Types.hpp"

#include "SAPDB/Container/Container_Vector.hpp"

#include "vak001.h"

// ------------------------------------------------------------------------------------------------

class SubQueryMapper;
class SQLMan_Context;
class Query_SelectNode;
class SAPDBMem_IRawAllocator;

// ------------------------------------------------------------------------------------------------

/*!
    @class          NodeMap
    @brief          ...
 */
class NodeMap
{
public:
    /// ...
    NodeMap( void ) 
        : SrcNode(cak_qrewrite_ref_init)
        , DestNode(cak_qrewrite_ref_init)
    {}

    /// ...
    NodeMap( SyntaxTreeNodeNo SNode, SyntaxTreeNodeNo DNode ) 
        : SrcNode(SNode)
        , DestNode(DNode)
    {}

    /// ...
    SyntaxTreeNodeNo SrcNode;   
    /// ...
    SyntaxTreeNodeNo DestNode;  
};

/*!
    @class          Query_NodeMapper
    @brief          ...
 */
class Query_NodeMapper
{
public:
    /// ...
    Query_NodeMapper( SAPDBMem_IRawAllocator& Alloc, SAPDB_Int2 InitSize = 10 )
        : mMapVector(Alloc)
    {
        mMapVector.Reserve(InitSize);
    }

    /// ...
    ~Query_NodeMapper( void )
    {
        mMapVector.Delete();
    }

    /// ...
    inline SAPDB_Int2 GetSize( void )
    {
        return mMapVector.GetSize();
    }

    /// ...
    inline void AddMappingInfo( SyntaxTreeNodeNo SrcNode, SyntaxTreeNodeNo DestNode )
    {
        NodeMap M(SrcNode, DestNode);
        mMapVector.InsertEnd(M);
    }

    /// ...
    inline SyntaxTreeNodeNo GetDestNodeFromPos( SAPDB_Int2 i )
    {
        return mMapVector[i].DestNode;
    }

    /// ...
    inline SyntaxTreeNodeNo GetDestNodeFromSrcNode( SyntaxTreeNodeNo SrcNode )
    {
        SyntaxTreeNodeNo n = mMapVector.GetSize();
        for (SAPDB_Int2 i=0; i<n; i++)
        {
            if ( mMapVector[i].SrcNode == SrcNode )
                return mMapVector[i].DestNode;
        }
        return cak_qrewrite_ref_init;    
    }

private:
    /// ...
    Container_Vector<NodeMap>   mMapVector; 
};

// ------------------------------------------------------------------------------------------------

/*!
    @class          QuantifierMap
    @brief          ...
 */
class QuantifierMap
{
public:
    /// ...
    QuantifierMap( void ) 
        : SrcQNode(cak_qrewrite_ref_init)
        , DestQNode(cak_qrewrite_ref_init)
    {}

    /// ...
    QuantifierMap( SyntaxTreeNodeNo SQNode, SyntaxTreeNodeNo DQNode ) 
        : SrcQNode(SQNode)
        , DestQNode(DQNode)
    {}

    /// ...
    SyntaxTreeNodeNo SrcQNode;  
    /// ...
    SyntaxTreeNodeNo DestQNode;
};

/*!
    @class          Query_QuantifierMapper
    @brief          ...
 */
class Query_QuantifierMapper
{
public:
    /// ...
    Query_QuantifierMapper( SAPDBMem_IRawAllocator& Alloc, SAPDB_Int2 InitSize = 10 )
        : mMapVector(Alloc)
    {
        mMapVector.Reserve(InitSize);
    }

    /// ...
    ~Query_QuantifierMapper( void )
    {
        mMapVector.Delete();
    }

    /// ...
    inline SAPDB_Int2 GetSize( void )
    {
        return mMapVector.GetSize();
    }

    /// ...
    inline void AddMappingInfo( SyntaxTreeNodeNo SrcQNode, SyntaxTreeNodeNo DestQNode )
    {
        QuantifierMap Q(SrcQNode, DestQNode);
        mMapVector.InsertEnd(Q);
    }

    /// ...
    inline SyntaxTreeNodeNo GetDestQNodeFromPos( SAPDB_Int2 i )
    {
        return mMapVector[i].DestQNode;
    }

    /// ...
    inline SyntaxTreeNodeNo GetDestQNodeFromSrcQNode( SyntaxTreeNodeNo SrcQNode )
    {
        SyntaxTreeNodeNo n = mMapVector.GetSize();
        for (SAPDB_Int2 i=0; i<n; i++)
        {
            if ( mMapVector[i].SrcQNode == SrcQNode )
                return mMapVector[i].DestQNode;
        }
        return cak_qrewrite_ref_init;    
    }

private:
    /// ...
    Container_Vector<QuantifierMap>   mMapVector;  
};

// ------------------------------------------------------------------------------------------------

///*!
//    @class          Query_SyntaxTreeNode
//    @brief          ...
// */
//class Query_SyntaxTreeNode : public tak_ap_node
//{
//public:
//
//};

/*!
    @class          Query_SyntaxTree
    @brief          ...
 */
class Query_SyntaxTree 
{
public:
    /// ...
    Query_SyntaxTree( SQLMan_Context& Context );

    /// ...
    Query_SyntaxTree& operator= ( const Query_SyntaxTree& T );

    /// ...
    void PrintToTrace( void );

    // -------------------

    /// ...
    inline /*Query_SyntaxTreeNode&*/ tak_ap_node& operator[](SAPDB_Int2 n)
    {
        return *(mAPTreePtr + n);
    }

    /// ...
    inline const /*Query_SyntaxTreeNode&*/ tak_ap_node& operator[](SAPDB_Int2 n) const
    {
        return *(mAPTreePtr + n);
    }

    /// ...
    inline tak_ap_node* GetAPTree( void )
    {
        return mAPTreePtr;
    }

    // -------------------

    /// ...
    inline bool IsOk( void )
    {
        return !mError;
    }

    /// ...
    void SetError( void )
    {
        mError = true;
    }

    // -------------------

    /// ...
    inline bool NeedsMapping( SyntaxTreeNodeNo Node ) const
    {
        return (/* mAPTreePtr[Node].n_querycheck.includes(qcn_NeedAdjustment) || */
                 (mAPTreePtr[Node].n_proc == a60 && mAPTreePtr[Node].n_subproc() == cak_x_select_column ) ||
                 (mAPTreePtr[Node].n_proc == a66 && mAPTreePtr[Node].n_subproc() == cak_x_table_spec ) ||
                 (mAPTreePtr[Node].n_proc == a66 && mAPTreePtr[Node].n_subproc() == cak_x_select_in_from_part ) ||

                 (mAPTreePtr[Node].n_proc == a92fromsel ) ||
                 (mAPTreePtr[Node].n_proc == a66 && mAPTreePtr[Node].n_subproc() == cak_x_select_in_from_part ) ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_mass_select )         ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_subquery )            ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_one_val_subquery )    ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_arith_with_subquery ) ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_start_union )			||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_decl_cursor )			||
                 (mAPTreePtr[Node].n_proc == a63query_spec ) ||
                 //
                 (mAPTreePtr[Node].n_proc == a80 && mAPTreePtr[Node].n_subproc() == cak_x_select_hint )			||
                 (mAPTreePtr[Node].n_proc == a80 && mAPTreePtr[Node].n_subproc() == cak_x_update_hint )			||
                 (mAPTreePtr[Node].n_proc == a80 && mAPTreePtr[Node].n_subproc() == cak_x_delete_hint )			||
                 //
                 (mAPTreePtr[Node].n_proc == no_proc && mAPTreePtr[Node].n_symb == s_columnid)                  
                 );
				// this statement must match with Query_SyntaxTree::MapSubSelectNodes 
				// all nodes listed in MapSubSelectNodes must occur here too ! but not vice versa !
    }

    /// ...
    inline bool WithOutPositionOffset( SyntaxTreeNodeNo Node ) const
    {
        return ( mAPTreePtr[Node].n_querycheck.includes(qcn_FixedPos)       ||
			     /*mAPTreePtr[Node].n_querycheck.includes(qcn_NeedAdjustment) || */
                 (mAPTreePtr[Node].n_proc == a92fromsel ) ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_subquery )            ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_one_val_subquery )    ||
                 (mAPTreePtr[Node].n_proc == a63 && mAPTreePtr[Node].n_subproc() == cak_x_arith_with_subquery ) ||
                 (mAPTreePtr[Node].n_proc == a80 && mAPTreePtr[Node].n_subproc() == cak_x_select_hint )			||
                 (mAPTreePtr[Node].n_proc == a80 && mAPTreePtr[Node].n_subproc() == cak_x_update_hint )			||
                 (mAPTreePtr[Node].n_proc == a80 && mAPTreePtr[Node].n_subproc() == cak_x_delete_hint )
                );
				// this statement must match with Query_SyntaxTree::MapSubSelectNodes
    }

    /// ...
    bool MapNode( SyntaxTreeNodeNo& Node, Query_NodeMapper* mpMap )
    {
        if ( Node > 0 )
        {
            SyntaxTreeNodeNo Dest = mpMap->GetDestNodeFromSrcNode(Node);  // mpMapper ?????
            if ( Dest > 0 )
                Node = Dest;
            return ( Dest > 0 );
        }
        return true;
    }

    /// ...
    bool MapQuantifierID( SAPDB_Int2& QID, Query_QuantifierMapper* mpQMap )
    {
        if ( QID > 0 )
        {
            SyntaxTreeNodeNo Dest = mpQMap->GetDestQNodeFromSrcQNode(QID);
            if ( Dest > 0 )
                QID = Dest;
            return ( Dest > 0 );
        }
        return true;
    }

    // -------------------

    /// ...
    inline void SetRefBack( SyntaxTreeNodeNo StartNodeNo )
    {
        if ( mAPTreePtr[StartNodeNo].n_lo_level > 0 )
        {
            mAPTreePtr[mAPTreePtr[StartNodeNo].n_lo_level ].n_refback = StartNodeNo;
            SetRefBack(mAPTreePtr[StartNodeNo].n_lo_level );
        }
        if ( mAPTreePtr[StartNodeNo].n_sa_level > 0 )
        {
            mAPTreePtr[mAPTreePtr[StartNodeNo].n_sa_level].n_refback = StartNodeNo;
            SetRefBack(mAPTreePtr[StartNodeNo].n_sa_level );
        }
        return;
    }

    /// ...
    inline bool GetFirstTreeNode( SyntaxTreeNodeNo& Node )
    {
#if defined(SAPDB_SLOW)
        SyntaxTreeNodeNo OrgNode = Node;    
#endif

        SyntaxTreeNodeNo Act  = Node;
        if ( Act>=0 )
        {
            while ( mAPTreePtr[Act].n_lo_level > 0 )
            {
                Act = mAPTreePtr[Act].n_lo_level;
            }
            Node = Act;
            SAPDBERR_ASSERT_STATE( Node >= 0 );
            return true;
        }
	    Node = cak_qrewrite_ref_init;
        return false;
    }

    /// ...
    inline bool GetNextTreeNode( SyntaxTreeNodeNo& Node )
    {
#if defined(SAPDB_SLOW)
        SyntaxTreeNodeNo OrgNode = Node; 
#endif

        if ( Node >= 0 )
        {
            SyntaxTreeNodeNo Next = mAPTreePtr[Node].n_sa_level;
            if ( Next > 0 )
            {
                Node = Next;
                return GetFirstTreeNode(Node);
            }
            else
            {
                do
                {
                    Next = mAPTreePtr[Node].n_refback;
                    if ( Next == Node || Next == cak_qrewrite_ref_init ) //
				    {
					    Node = cak_qrewrite_ref_init;
                        return false;
				    }
                    if ( mAPTreePtr[Next].n_lo_level == Node )
                    {
                        Node = Next;
                        SAPDBERR_ASSERT_STATE( Node >= 0 );
                        return true;
                    }
                    Node = Next;
                } while ( Node>=0 );
            }
        }
	    Node = cak_qrewrite_ref_init;
        return false;
    }

    /// ...    
    inline bool GetFirstArrayNode( SyntaxTreeNodeNo& Node )
    {
        Node = 0;
        return true;
    }

    /// ...
    inline bool GetNextArrayNode( SyntaxTreeNodeNo& Node )
    {
        if ( Node >= 0 && Node<cak_max_node )
        {
            if ( mAPTreePtr[Node].n_special.includes(ni_info_node) )
                ++Node;

            if ( ! mAPTreePtr[Node].n_special.includes(ni_skip_node) )
            {
                ++Node;
            }
            else
            {
                if ( !(Node = mAPTreePtr[Node].n_pos()) )
                    return false;
            }
            if ( Node <= mContext.a_scv_index )
                return true;
        }
        return false;
    }

    // -------------------

    /// ...
    SyntaxTreeNodeNo GetFreeNode( bool ForcePutNode=false );

    /// ...
    inline SAPDB_Int2 GetFreeNodeCount( void )
    {
        return mFreeNodeCount;
    }

    /// ...
    inline void SetUnusedNode( SyntaxTreeNodeNo NodeNo, bool Recursive=false )
    {
        if ( NodeNo > 0 )
        {
            if ( Recursive )
            {	// DDT : RECURSIVE
                SetUnusedNode(mAPTreePtr[NodeNo].n_lo_level, true);
                SetUnusedNode(mAPTreePtr[NodeNo].n_sa_level, true);
            }

            if ( mAPTreePtr[NodeNo].n_special.includes(ni_info_node) )
                SetUnusedNode(NodeNo+1, false);

            mAPTreePtr[NodeNo].n_sa_level = cak_qrewrite_ref_init;
            mAPTreePtr[NodeNo].n_lo_level = cak_qrewrite_ref_init;
            mAPTreePtr[NodeNo].n_refback  = mFirstUnusedNode;

            mAPTreePtr[NodeNo].n_proc.becomes(no_proc);
            mAPTreePtr[NodeNo].n_special.clear();
            mAPTreePtr[NodeNo].n_symb.becomes(s_unknown);
            mAPTreePtr[NodeNo].n_datatype.becomes(dunknown);
            mAPTreePtr[NodeNo].n_querycheck.clear();
            mAPTreePtr[NodeNo].n_dt_format = mContext.a_dt_format;
            mAPTreePtr[NodeNo].n_subproc() = 0;
            mAPTreePtr[NodeNo].n_length() = 0;
            mAPTreePtr[NodeNo].n_pos() = 0;

            mFirstUnusedNode = NodeNo;
            ++mFreeNodeCount;
        }
    }

    // -------------------

    /// ...
    bool CheckNodes( void );

    // -------------------

    /// ...
    inline bool SubtreeContainsNode( SyntaxTreeNodeNo SubtreeStart, SyntaxTreeNodeNo Node );

    // -------------------

    /// ...
    inline SyntaxTreeNodeNo DoubleTreePart( SyntaxTreeNodeNo StartNode, Query_NodeMapper* pMap, bool MapNodes, bool WithSameLevel )
    {
        SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::DoubleTreePart", QueryRewrite_Trace, 7);
        return DoubleTreePart(0, mAPTreePtr, StartNode, 0, pMap, MapNodes, WithSameLevel);
    }

    // DDT: Duplicate the part of the ap_tree given by Context / StartNo 
    // the duplicate will be within the OWN nodearray !!!
    /// ...
    SyntaxTreeNodeNo DoubleTreePart( SAPDBMem_IRawAllocator* Alloc, tak_ap_node* SourceSyntaxTree, SyntaxTreeNodeNo StartNode, SAPDB_Int4 PosOffset, Query_NodeMapper* pMap, bool MapNodes, bool WithSameLevel );

    // -------------------

    /// ...
    inline void UnlinkNode( SyntaxTreeNodeNo NodeNo, bool SetUnused );
    /// ...
    inline void UnlinkNodeWithSameLevel( SyntaxTreeNodeNo NodeNo, bool SetUnused );
    /// ...
    inline bool SubstituteNodeBySubtree( SyntaxTreeNodeNo OldNodeNo, SyntaxTreeNodeNo StartSubtreeNodeNo, bool SetUnused );
    /// ...
    inline void SetSucessorOnSameLevel( SyntaxTreeNodeNo NodeNo, SyntaxTreeNodeNo NewSuccessor );
    /// ...
    inline void SetSucessorOnLowLevel( SyntaxTreeNodeNo NodeNo, SyntaxTreeNodeNo NewSuccessor );

    // -------------------

    /// ...
    inline bool AddSubSelectNode( SyntaxTreeNodeNo SelNode, SyntaxTreeNodeNo SubSelNode, SyntaxTreeNodeNo FatherOfSubSelNode );
    /// ...
    inline bool RemoveSubSelectNode( SyntaxTreeNodeNo FromSelNodeNo, bool WithChildren );
    /// ...
    inline SyntaxTreeNodeNo FindSubSelectNode( SyntaxTreeNodeNo Node );

    /// ...
    inline bool ChangeColumnNameToColumnId( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo FromTabNode, SAPDB_Int2 ColumnIndex, SAPDB_Int2 NewColPos );

    /// ...
    void MapSubSelectNodes( Query_NodeMapper* pMap );

    // -------------------

    /// ...
    inline bool SimplifyPredicates( SyntaxTreeNodeNo Node, bool& simpl, Query_SelectNode* SelNode );
    /// ...
    inline void RemoveNot( SyntaxTreeNodeNo Node );
    /// ...
    inline bool AddNotBeforeNode( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo& Not, Query_SelectNode* SelNode );
    /// ...
    inline bool ConvertNotOPToNOP( SyntaxTreeNodeNo& Node, bool& simpl, Query_SelectNode* SelNode );
    /// ...
    inline bool BoolTermTRUE( SyntaxTreeNodeNo Node );
    /// ...
    inline bool BoolTermFALSE( SyntaxTreeNodeNo Node );
    /// ...
    inline bool BoolTermTRUEorFALSE( SyntaxTreeNodeNo Node, bool& True );
    /// ...
    inline bool GetFirstOperand( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo& Op );
    /// ...
    inline bool GetSecondOperand( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo& Op );
    /// ...
    inline bool ContainsParameter( SyntaxTreeNodeNo Node, bool ConsiderSame=false );
    /// ...
    inline bool IsAggregationNode( SyntaxTreeNodeNo Node );
    /// ...
    inline bool IsBooleanFactorInCondition( SyntaxTreeNodeNo Node );

    /// ...
    inline void SetCountFlag( SyntaxTreeNodeNo Node )
    {
        SAPDBERR_ASSERT_ARGUMENT( Node > 0 );
        if ( mAPTreePtr[Node].n_symb != s_sum )
            mAPTreePtr[Node].n_symb.becomes(s_count);
    }

    // -------------------

protected:

    /// ...
    bool ExchangeAnyAndAll( SyntaxTreeNodeNo Node, Query_SelectNode* SelNode );
    /// ...
    inline bool HasOperandAnyOrAll( SyntaxTreeNodeNo Node );
    /// ...
    bool CheckSingleNode( SyntaxTreeNodeNo NodeNo );
    /// ...
    bool InternCheckNodes( SyntaxTreeNodeNo NodeNo );

    /// ...
    SyntaxTreeNodeNo DoubleTreePartInternal( tak_ap_node* SourceSyntaxTree, SyntaxTreeNodeNo StartNode, SAPDB_Int4 PosOffset, bool WithSameLevel );

private:

    /// ...
    SQLMan_Context&     mContext;           
    /// ...
    tak_ap_node*        mAPTreePtr;         
    /// ...
    Query_NodeMapper*   mpMapper;           
    /// ...
    SyntaxTreeNodeNo    mFirstUnusedNode;   
    /// ...
    SyntaxTreeNodeNo    mFreeNodeCount;     
    /// ...
    bool                mError;             
};

// ------------------------------------------------------------------------------------------------

void Query_SyntaxTree::UnlinkNode( SyntaxTreeNodeNo NodeNo, bool SetUnused )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::UnlinkNode", QueryRewrite_Trace, 6);

    if ( NodeNo > 0 )
    {
        SyntaxTreeNodeNo predecessor    = mAPTreePtr[NodeNo].n_refback;
        SyntaxTreeNodeNo successor      = mAPTreePtr[NodeNo].n_sa_level;

        if ( predecessor >= 0 )
        {
            if ( mAPTreePtr[predecessor].n_sa_level == NodeNo )
                mAPTreePtr[predecessor].n_sa_level = successor;
            else
                mAPTreePtr[predecessor].n_lo_level = successor;
        }
        if ( successor > 0 )
            mAPTreePtr[successor].n_refback = predecessor;

        mAPTreePtr[NodeNo].n_refback  = cak_qrewrite_ref_init;
        mAPTreePtr[NodeNo].n_sa_level = 0;

        if ( SetUnused )
            SetUnusedNode(NodeNo, true);
    }
}

void Query_SyntaxTree::UnlinkNodeWithSameLevel( SyntaxTreeNodeNo NodeNo, bool SetUnused )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::UnlinkNode", QueryRewrite_Trace, 6);

    if ( NodeNo > 0 )
    {
        SyntaxTreeNodeNo predecessor    = mAPTreePtr[NodeNo].n_refback;

        if ( predecessor >= 0 )
        {
            if ( mAPTreePtr[predecessor].n_sa_level == NodeNo )
                mAPTreePtr[predecessor].n_sa_level = 0;
            else
                mAPTreePtr[predecessor].n_lo_level = 0;
        }
        mAPTreePtr[NodeNo].n_refback  = cak_qrewrite_ref_init;

        if ( SetUnused )
            SetUnusedNode(NodeNo, true);
    }
}

bool Query_SyntaxTree::SubstituteNodeBySubtree( SyntaxTreeNodeNo OldNodeNo, SyntaxTreeNodeNo StartSubtreeNodeNo, bool SetUnused )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::SubstituteNodeBySubtree", QueryRewrite_Trace, 6);

    if ( OldNodeNo > 0 )
    {
        // DDT: was, wenn StarSubTreeNodeNO.n_refback =OldNodeNo ???????????????

        SyntaxTreeNodeNo OldSuccessor   = mAPTreePtr[OldNodeNo].n_sa_level;
        SyntaxTreeNodeNo predecessor    = mAPTreePtr[OldNodeNo].n_refback;

        if ( predecessor >= 0 )
            if ( mAPTreePtr[predecessor].n_sa_level == OldNodeNo )
            {
                if ( StartSubtreeNodeNo > 0 )
                    mAPTreePtr[predecessor].n_sa_level = StartSubtreeNodeNo;
                else
                {
                    mAPTreePtr[predecessor].n_sa_level = OldSuccessor;
                    if ( OldSuccessor )
                        mAPTreePtr[OldSuccessor].n_refback = predecessor;
                }
            }
            else
            {
                if ( StartSubtreeNodeNo > 0 )
                    mAPTreePtr[predecessor].n_lo_level = StartSubtreeNodeNo;
                else
                {
                    mAPTreePtr[predecessor].n_lo_level = OldSuccessor;
                    if ( OldSuccessor )
                        mAPTreePtr[OldSuccessor].n_refback = predecessor;                }
            }

        if ( StartSubtreeNodeNo > 0 )
        {
            mAPTreePtr[StartSubtreeNodeNo].n_refback = predecessor;

            if ( OldSuccessor > 0 )
            {
                SyntaxTreeNodeNo i = StartSubtreeNodeNo;
                while ( mAPTreePtr[i].n_sa_level > 0 )
                {
                    i = mAPTreePtr[i].n_sa_level;
                }
                mAPTreePtr[i].n_sa_level = OldSuccessor;
                mAPTreePtr[OldSuccessor].n_refback = i;
            }
        }
        mAPTreePtr[OldNodeNo].n_refback     = cak_qrewrite_ref_init;
        mAPTreePtr[OldNodeNo].n_sa_level    = 0;
        if ( SetUnused )
        {
            mAPTreePtr[OldNodeNo].n_lo_level    = 0;
            SetUnusedNode(OldNodeNo);
        }
    }
    return true;
}

void Query_SyntaxTree::SetSucessorOnSameLevel( SyntaxTreeNodeNo NodeNo, SyntaxTreeNodeNo NewSuccessor )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::SetSucessorOnSameLevel", QueryRewrite_Trace, 6);

    if ( NodeNo >= 0 )
    {
        SyntaxTreeNodeNo OldSuccessor = mAPTreePtr[NodeNo].n_sa_level;

        mAPTreePtr[NewSuccessor].n_sa_level = OldSuccessor;
        mAPTreePtr[NewSuccessor].n_refback  = NodeNo;
        mAPTreePtr[NodeNo      ].n_sa_level = NewSuccessor;

        if ( OldSuccessor > 0 )
            mAPTreePtr[OldSuccessor].n_refback = NewSuccessor;
    }
}

void Query_SyntaxTree::SetSucessorOnLowLevel( SyntaxTreeNodeNo NodeNo, SyntaxTreeNodeNo NewSuccessor )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::SetSucessorOnLowLevel", QueryRewrite_Trace, 6);

    if ( NodeNo >= 0 )
    {
        SyntaxTreeNodeNo OldSuccessor = mAPTreePtr[NodeNo].n_lo_level;

        mAPTreePtr[NewSuccessor].n_refback  = NodeNo;
        mAPTreePtr[NodeNo      ].n_lo_level = NewSuccessor;

        SyntaxTreeNodeNo H = NewSuccessor;
        while ( mAPTreePtr[H].n_sa_level > 0 )
        {
            H = mAPTreePtr[H].n_sa_level;
        }
        mAPTreePtr[H].n_sa_level = OldSuccessor;

		if ( OldSuccessor > 0 )
            mAPTreePtr[OldSuccessor].n_refback = H;
    }
}

bool Query_SyntaxTree::SubtreeContainsNode( SyntaxTreeNodeNo SubtreeStart, SyntaxTreeNodeNo Node )
{	// DDT : RECURSIVE
    if ( SubtreeStart > 0 && Node > 0 )
    {
        if ( SubtreeStart == Node )
            return true;
        if ( SubtreeContainsNode(mAPTreePtr[SubtreeStart].n_lo_level, Node) )
            return true;
        return SubtreeContainsNode(mAPTreePtr[SubtreeStart].n_sa_level, Node);
    }
    return false;
}

SyntaxTreeNodeNo Query_SyntaxTree::FindSubSelectNode( SyntaxTreeNodeNo Node )
{
    if ( Node >=0 )
    {
        SyntaxTreeNodeNo i=0;
        if ( GetFirstTreeNode(i) )
        {
            do
            {
                if ( mAPTreePtr[i].n_pos()  == Node )
                {
                    if ( ( mAPTreePtr[i].n_proc == a92fromsel ) ||

                        (( mAPTreePtr[i].n_proc == a63)
				        &&
                        ( mAPTreePtr[i].n_subproc() == cak_x_subquery  ||
				        mAPTreePtr[i].n_subproc() == cak_x_one_val_subquery ||
				        mAPTreePtr[i].n_subproc() == cak_x_arith_with_subquery ||
				        mAPTreePtr[i].n_subproc() == cak_x_corr_subquery ||
				        mAPTreePtr[i].n_subproc() == cak_x_one_val_corr_sub )
				        &&
                        ( mAPTreePtr[i].n_symb == s_select ||
				        mAPTreePtr[i].n_symb == s_where ||
				        mAPTreePtr[i].n_symb == s_having )
				        ) )
                    {
                        return i;
                    }
                }
            }while ( GetNextTreeNode(i) );
        }
    }
    return cak_qrewrite_ref_init;
}

bool Query_SyntaxTree::AddSubSelectNode( SyntaxTreeNodeNo SelNode, SyntaxTreeNodeNo SubSelNode, SyntaxTreeNodeNo FatherOfSubSelNode )
{
    // Adds an already existing SubSelect node identified by SubSelNode 

    SAPDBTRACE_METHOD_DEBUG ("Query_SelectNode::AddSubSelectNode", QueryRewrite_Trace, 6);

    if ( SelNode == FatherOfSubSelNode )
    {
        SyntaxTreeNodeNo H1 = SelNode;
        while ( mAPTreePtr[H1].n_sa_level > 0 )
        {
            H1 = mAPTreePtr[H1].n_sa_level;
        }
        SetSucessorOnSameLevel(H1, SubSelNode); 
        return true;
    }
    else
    {
        SyntaxTreeNodeNo TopNode = FindSubSelectNode(FatherOfSubSelNode);
        if ( TopNode > 0 )
        {
            if ( mAPTreePtr[TopNode].n_lo_level > 0 )
            {
                SyntaxTreeNodeNo H2 = mAPTreePtr[TopNode].n_lo_level;
                while ( mAPTreePtr[H2].n_sa_level > 0 )
                {
                    H2 = mAPTreePtr[H2].n_sa_level;
                }
                SetSucessorOnSameLevel(H2, SubSelNode); 
            }
            else
            {
                SetSucessorOnLowLevel(TopNode, SubSelNode);
            }
            return true;
        }
    }
    return false;
}

bool Query_SyntaxTree::RemoveSubSelectNode( SyntaxTreeNodeNo NodeNo, bool WithChildren )
{
    SAPDBTRACE_METHOD_DEBUG ("Query_SyntaxTree::RemoveFromSelNode", QueryRewrite_Trace, 6);
    SAPDBERR_ASSERT_ARGUMENT( NodeNo > 0 );

    SyntaxTreeNodeNo ToBeRemoved = FindSubSelectNode(NodeNo);
    if ( ToBeRemoved > 0 )
    {
        if ( !WithChildren )
            return SubstituteNodeBySubtree(ToBeRemoved, mAPTreePtr[ToBeRemoved].n_lo_level, /*SetUnused*/true);
        else
        {
            UnlinkNode(ToBeRemoved, /*SetUnused*/true);
            return true;
        }
    }
    return false;
}

bool Query_SyntaxTree::ChangeColumnNameToColumnId( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo FromTabNode, SAPDB_Int2 ColumnIndex, SAPDB_Int2 NewColPos )
{
	SAPDBERR_ASSERT_ARGUMENT( (FromTabNode>0 && ColumnIndex>0) || ColumnIndex==cak_qrewrite_ref_star || ColumnIndex==cak_qrewrite_ref_multistar );

	if ( (FromTabNode>0 && ColumnIndex>0) || ColumnIndex==cak_qrewrite_ref_star || ColumnIndex==cak_qrewrite_ref_multistar )
	{
		if ( (mAPTreePtr[Node].n_proc == 0) &&
				((mAPTreePtr[Node].n_symb == s_columnname) ||
				 (mAPTreePtr[Node].n_symb == s_asterisk) )  )
		{
			mAPTreePtr[Node].n_symb.becomes(s_columnid);
			mAPTreePtr[Node].n_fromtabnode() = FromTabNode;
			mAPTreePtr[Node].n_columnindex() = ColumnIndex;
            mAPTreePtr[Node].n_pos()         = NewColPos;

			Node = mAPTreePtr[Node].n_refback;
			if ( (mAPTreePtr[Node].n_proc == 0) &&
				(mAPTreePtr[Node].n_symb == s_tablename) )
			{
				SyntaxTreeNodeNo Help = Node;
				Node = mAPTreePtr[Node].n_refback;
				UnlinkNode(Help, /*SetUnused*/ true);
			}

			if ( (mAPTreePtr[Node].n_proc == 0) &&
				(mAPTreePtr[Node].n_symb == s_authid) )
			{
				UnlinkNode(Node, /*SetUnused*/ true);
			}
			return true;
		}
	}
    Kernel_VTrace() << "ChangeColumnNameToColumnId: " << Node << FlushLine;
    SAPDBERR_ASSERT_STATE( 0 );
    return false;
}

bool Query_SyntaxTree::SimplifyPredicates( SyntaxTreeNodeNo Node, bool& simpl, Query_SelectNode* SelNode )
{
    if ( mAPTreePtr[Node].n_proc == a63query_spec )
        return true;

    SyntaxTreeNodeNo Sa = mAPTreePtr[Node].n_sa_level;
    SyntaxTreeNodeNo Lo = mAPTreePtr[Node].n_lo_level;
    bool rc = true;

    if ( Sa ) 
        rc &= SimplifyPredicates(Sa, simpl, SelNode);
    if ( Lo ) 
        rc &= SimplifyPredicates(Lo, simpl, SelNode);

    if ( mAPTreePtr[Node].n_proc == no_proc && mAPTreePtr[Node].n_symb == s_not )
    {
        rc = ConvertNotOPToNOP(Node, simpl, SelNode);
    }
    else
    {   
        if ( mAPTreePtr[Node].n_proc == a64 )
        {
            SyntaxTreeNodeNo Op1, Op2;
            bool True;

            switch ( mAPTreePtr[Node].n_symb )
            {
            case s_and:
                if ( GetFirstOperand(Node, Op1) && GetSecondOperand(Node, Op2) )
                {
                    if ( BoolTermTRUEorFALSE(Op1, True) )
                    {
                        if ( True )
                        {
                            //   TRUE AND Op2 > Op2
                            UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ false);
                            UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                            rc &= SubstituteNodeBySubtree(Node, Op2, /*SetUnused*/true);
                            simpl = true;
                        }
                        else
                        {
                            if ( !ContainsParameter(Op2) )
                            {
                                //   FALSE AND Op2 > FALSE(=Op1)
                                UnlinkNodeWithSameLevel(Op1, /*SetUnused*/ false);
                                UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ true);
                                UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                                rc &= SubstituteNodeBySubtree(Node, Op1, /*SetUnused*/true);
                                simpl = true;
                            }
                        }
                    }
                    else
                    {
                        if ( BoolTermTRUEorFALSE(Op2, True) )
                        {
                            if ( True )
                            {
                                //   Op1 AND TRUE > Op1
                                UnlinkNodeWithSameLevel(Op1, /*SetUnused*/ false);
                                UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ true);
                                UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                                rc &= SubstituteNodeBySubtree(Node, Op1, /*SetUnused*/true);
                                simpl = true;
                            }
                            else
                            {
                                if ( !ContainsParameter(Op1) )
                                {
                                    //   Op1 and FALSE > FALSE(=Op2)
                                    UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ false);
                                    UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                                    rc &= SubstituteNodeBySubtree(Node, Op2, /*SetUnused*/true);
                                    simpl = true;
                                }
                            }
                        }
                    }
                }
                else
                {
                    rc = false;
                }
                break;
            case s_or:
                if ( GetFirstOperand(Node, Op1) && GetSecondOperand(Node, Op2) )
                {
                    if ( BoolTermTRUEorFALSE(Op1, True) )
                    {
                        if ( True )
                        {
                            if ( !ContainsParameter(Op2) )
                            {
                                //   TRUE OR Op2 > TRUE(=Op1)
                                UnlinkNodeWithSameLevel(Op1, /*SetUnused*/ false);
                                UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ true);
                                UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                                rc &= SubstituteNodeBySubtree(Node, Op1, /*SetUnused*/true);
                                simpl = true;
                            }
                        }
                        else
                        {
                            //   FALSE OR Op2 > Op2
                            UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ false);
                            UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                            rc &= SubstituteNodeBySubtree(Node, Op2, /*SetUnused*/true);
                            simpl = true;
                        }
                    }
                    else
                    {
                        if ( BoolTermTRUEorFALSE(Op2, True) )
                        {
                            if ( True )
                            {
                                if ( !ContainsParameter(Op1) )
                                {
                                    //   Op1 OR TRUE > TRUE(=Op2)
                                    UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ false);
                                    UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                                    rc &= SubstituteNodeBySubtree(Node, Op2, /*SetUnused*/true);
                                    simpl = true;
                                }
                            }
                            else
                            {
                                //   Op1 OR FALSE > Op1
                                UnlinkNodeWithSameLevel(Op1, /*SetUnused*/ false);
                                UnlinkNodeWithSameLevel(Op2, /*SetUnused*/ true);
                                UnlinkNode(mAPTreePtr[Node].n_lo_level, /*SetUnused*/true);
                                rc &= SubstituteNodeBySubtree(Node, Op1, /*SetUnused*/true);
                                simpl = true;
                            }
                        }
                    }                
                }
                else
                {
                    rc = false;
                }
                break;
            }
        }
    }
    return rc;
}

bool Query_SyntaxTree::ConvertNotOPToNOP( SyntaxTreeNodeNo& Node, bool& simpl, Query_SelectNode* SelNode )
{
    SyntaxTreeNodeNo Op = mAPTreePtr[Node].n_sa_level;

    if ( Node<=0 || !SelNode )
        return false;

    bool rc = true;

    // 1. replace NOT OP by NOP, e.g. 'not <' by '>='
    if ( mAPTreePtr[Op].n_proc == no_proc )
    {
        switch ( mAPTreePtr[Op].n_symb )
        {
        case s_true:
            mAPTreePtr[Op].n_symb.becomes(s_false);
            RemoveNot(Node);
            simpl = true;
            break;
        case s_false:
            mAPTreePtr[Op].n_symb.becomes(s_true);
            RemoveNot(Node);
            simpl = true;
            break;

        case s_not:
            RemoveNot(Op);
            RemoveNot(Node);
            simpl = true;
            break;
        }
    }
    else
    {
        if ( mAPTreePtr[Op].n_proc == a64 )
        {
            switch ( mAPTreePtr[Op].n_symb )
            {
            case s_equal:
                mAPTreePtr[Op].n_symb.becomes(s_unequal);
                RemoveNot(Node);
                rc = ExchangeAnyAndAll(Op, SelNode);
                break;
            case s_unequal:
                mAPTreePtr[Op].n_symb.becomes(s_equal);
                RemoveNot(Node);
                rc = ExchangeAnyAndAll(Op, SelNode);
                break;
            case s_greater:
                mAPTreePtr[Op].n_symb.becomes(s_less_or_eq);
                RemoveNot(Node);
                rc = ExchangeAnyAndAll(Op, SelNode);
                break;
            case s_less_or_eq:
                mAPTreePtr[Op].n_symb.becomes(s_greater);
                RemoveNot(Node);
                rc = ExchangeAnyAndAll(Op, SelNode);
                break;
            case s_less:
                mAPTreePtr[Op].n_symb.becomes(s_greater_or_eq);
                RemoveNot(Node);
                rc = ExchangeAnyAndAll(Op, SelNode);
                break;
            case s_greater_or_eq:
                mAPTreePtr[Op].n_symb.becomes(s_less);
                RemoveNot(Node);
                rc = ExchangeAnyAndAll(Op, SelNode);
                break;

            case s_in:
                mAPTreePtr[Op].n_symb.becomes(s_notin);
                RemoveNot(Node);
                break;
            case s_notin:
                mAPTreePtr[Op].n_symb.becomes(s_in);
                RemoveNot(Node);
                break;
            case s_between:
                mAPTreePtr[Op].n_symb.becomes(s_notbetween);
                RemoveNot(Node);
                break;
            case s_notbetween:
                mAPTreePtr[Op].n_symb.becomes(s_between);
                RemoveNot(Node);
                break;


            case s_like:
                mAPTreePtr[Op].n_symb.becomes(s_notlike);
                RemoveNot(Node);
                break;
            case s_notlike:
                mAPTreePtr[Op].n_symb.becomes(s_like);
                RemoveNot(Node);
                break;
            case s_sounds:
                mAPTreePtr[Op].n_symb.becomes(s_notsounds);
                RemoveNot(Node);
                break;
            case s_notsounds:
                mAPTreePtr[Op].n_symb.becomes(s_sounds);
                RemoveNot(Node);
                break;

            case s_is_true:
                mAPTreePtr[Op].n_symb.becomes(s_is_not_true);
                RemoveNot(Node);
                break;
            case s_is_not_true:
                mAPTreePtr[Op].n_symb.becomes(s_is_true);
                RemoveNot(Node);
                break;
            case s_is_false:
                mAPTreePtr[Op].n_symb.becomes(s_is_not_false);
                RemoveNot(Node);
                break;
            case s_is_not_false:
                mAPTreePtr[Op].n_symb.becomes(s_is_false);
                RemoveNot(Node);
                break;

            case s_is_not_null:
                mAPTreePtr[Op].n_symb.becomes(s_is_null);
                RemoveNot(Node);
                break;
            case s_is_null:
                mAPTreePtr[Op].n_symb.becomes(s_is_not_null);
                RemoveNot(Node);
                break;
            case s_or: // NOT (a OR b)  >  NOT a AND NOT b
                { 
                    SyntaxTreeNodeNo Not1 = GetFreeNode();
                    SyntaxTreeNodeNo Not2 = GetFreeNode();
                    SyntaxTreeNodeNo H1,H2;
                    if ( Not1>0 && Not2>0 && GetFirstOperand(Op, H1) && GetSecondOperand(Op, H2) )
                    {
                        RemoveNot(Node);
                        mAPTreePtr[Op].n_symb.becomes(s_and);
                        rc &= AddNotBeforeNode(H1, Not1, SelNode); 
                        rc &= AddNotBeforeNode(H2, Not2, SelNode); 
                    }
                    else
                    {
                        return false;
                    }
                }
                break;
            case s_and: // NOT (a AND b)  >  NOT a OR NOT b
                { // DDT 
                    SyntaxTreeNodeNo Not1 = GetFreeNode();
                    SyntaxTreeNodeNo Not2 = GetFreeNode();
                    SyntaxTreeNodeNo H1,H2;
                    if ( Not1>0 && Not2>0 && GetFirstOperand(Op, H1) && GetSecondOperand(Op, H2) )
                    {
                        RemoveNot(Node);
                        mAPTreePtr[Op].n_symb.becomes(s_or);
                        rc &= AddNotBeforeNode(H1, Not1, SelNode); 
                        rc &= AddNotBeforeNode(H2, Not2, SelNode); 
                    }
                    else
                    {
                        return false;
                    }
                }
                break;
            }
        }
    }
    Node = Op;
    return rc;
}

bool Query_SyntaxTree::HasOperandAnyOrAll( SyntaxTreeNodeNo Node )
{
    if ( Node > 0 )
    {
        SyntaxTreeNodeNo H1,H2;
        if ( GetFirstOperand(Node, H1) && GetSecondOperand(Node, H2) )
        {
            if (   mAPTreePtr[H1].n_proc == no_proc && mAPTreePtr[H1].n_symb==s_any 
                || mAPTreePtr[H1].n_proc == no_proc && mAPTreePtr[H1].n_symb==s_all 
                || mAPTreePtr[H2].n_proc == no_proc && mAPTreePtr[H2].n_symb==s_any 
                || mAPTreePtr[H2].n_proc == no_proc && mAPTreePtr[H2].n_symb==s_all )
                return true;
            return false;
        }
        return true; // in case of doubt ...
    }
    return false;
}

bool Query_SyntaxTree::AddNotBeforeNode( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo& Not, Query_SelectNode* SelNode )
{
    if ( Not<=0 )
        Not = GetFreeNode();
    if ( Not>0 )
    {
        mAPTreePtr[Not].n_proc.becomes(no_proc);
        mAPTreePtr[Not].n_symb.becomes(s_not);

        SyntaxTreeNodeNo R  = mAPTreePtr[Node].n_refback;
        if ( R>0 )
        {
            if ( mAPTreePtr[R].n_lo_level == Node ) 
                mAPTreePtr[R].n_lo_level = Not;
            else
                mAPTreePtr[R].n_sa_level = Not;
        }
        mAPTreePtr[Not ].n_refback  = R;
        mAPTreePtr[Not ].n_sa_level = Node;
        mAPTreePtr[Node].n_refback  = Not;

        if ( SelNode )
        {
            bool si;
            return ConvertNotOPToNOP(Not, si, SelNode);
        }
        return true;
    }
    return false;
}

void Query_SyntaxTree::RemoveNot( SyntaxTreeNodeNo Node )
{
    SyntaxTreeNodeNo R  = mAPTreePtr[Node].n_refback;
    SyntaxTreeNodeNo Sa = mAPTreePtr[Node].n_sa_level;

    mAPTreePtr[Node].n_sa_level = 0 ;
    mAPTreePtr[Sa].n_refback = R;

    if ( mAPTreePtr[R].n_lo_level == Node )
        mAPTreePtr[R].n_lo_level = Sa;
    else
        mAPTreePtr[R].n_sa_level = Sa;
    SetUnusedNode(Node);
}

bool Query_SyntaxTree::BoolTermTRUEorFALSE( SyntaxTreeNodeNo Node, bool& True )
{
    bool Not = false;
    while ( mAPTreePtr[Node].n_proc == no_proc && mAPTreePtr[Node].n_symb==s_not )
    {
        Not = !Not;
        Node = mAPTreePtr[Node].n_sa_level;
    }

#if defined( SAPDB_SLOW )
    if ( Node <=0 )
    {
        SAPDBERR_ASSERT_STATE( 0 );
    }
#endif

    if ( mAPTreePtr[Node].n_proc == a64 )
    {
        SyntaxTreeNodeNo H1, H2;
    
        switch ( mAPTreePtr[Node].n_symb )
        {
        case s_is_true:
            H1 = mAPTreePtr[Node].n_lo_level;
            H2 = mAPTreePtr[H1  ].n_sa_level;
            if ( mAPTreePtr[H1].n_proc==no_proc && mAPTreePtr[H2].n_proc==no_proc )
            {
                if ( mAPTreePtr[H1].n_symb==s_true && mAPTreePtr[H2].n_symb==s_true )
                {
                    True = (true && !Not);
                    return true;
                }
                if ( mAPTreePtr[H1].n_symb==s_true && mAPTreePtr[H2].n_symb==s_false )
                {
                    True = (false && !Not);
                    return true;
                }
                if ( mAPTreePtr[H1].n_symb==s_false && mAPTreePtr[H2].n_symb==s_true )
                {
                    True = (false && !Not);
                    return true;
                }
                if ( mAPTreePtr[H1].n_symb==s_false && mAPTreePtr[H2].n_symb==s_false )
                {
                    True = (false && !Not);
                    return true;
                }
            }
            break;
        case s_is_false:
            H1 = mAPTreePtr[Node].n_lo_level;
            H2 = mAPTreePtr[H1  ].n_sa_level;
            if ( mAPTreePtr[H1].n_proc==no_proc && mAPTreePtr[H2].n_proc==no_proc )
            {
                if ( mAPTreePtr[H1].n_symb==s_true && mAPTreePtr[H2].n_symb==s_true )
                {
                    True = (false && !Not);
                    return true;
                }
                if ( mAPTreePtr[H1].n_symb==s_true && mAPTreePtr[H2].n_symb==s_false )
                {
                    True = (true && !Not);
                    return true;
                }
                if ( mAPTreePtr[H1].n_symb==s_false && mAPTreePtr[H2].n_symb==s_true )
                {
                    True = (true && !Not);
                    return true;
                }
                if ( mAPTreePtr[H1].n_symb==s_false && mAPTreePtr[H2].n_symb==s_false )
                {
                    True = (true && !Not);
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

bool Query_SyntaxTree::BoolTermTRUE( SyntaxTreeNodeNo Node )
{
    bool True = true;
    bool rc = BoolTermTRUEorFALSE(Node, True);

    return ( rc && True );
}

bool Query_SyntaxTree::BoolTermFALSE( SyntaxTreeNodeNo Node )
{
    bool True = true;
    bool rc = BoolTermTRUEorFALSE(Node, True);

    return ( rc && !True );
}

bool Query_SyntaxTree::GetFirstOperand( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo& Op )
{
    if ( mAPTreePtr[Node].n_proc == a64 ) 
    {
        Op = mAPTreePtr[Node].n_lo_level;
        return ( Op > 0 );
    }
    return false;
}

bool Query_SyntaxTree::GetSecondOperand( SyntaxTreeNodeNo Node, SyntaxTreeNodeNo& Op )
{
    if ( GetFirstOperand(Node, Op) )
    {
        while ( mAPTreePtr[Op].n_proc==no_proc && mAPTreePtr[Op].n_symb==s_not )
        {
            Op = mAPTreePtr[Op].n_sa_level;
        }
        Op = mAPTreePtr[Op].n_sa_level;
        return ( Op > 0 );
    }
    return false;
}

bool Query_SyntaxTree::ContainsParameter( SyntaxTreeNodeNo Node, bool ConsiderSame  )
{
    // Don't stop recursion at subqueries!

    while ( mAPTreePtr[Node].n_proc == no_proc && mAPTreePtr[Node].n_symb==s_not )
    {
        Node = mAPTreePtr[Node].n_sa_level;
    }

#if defined( SAPDB_SLOW )
    if ( Node <=0 )
    {
        SAPDBERR_ASSERT_STATE( 0 );
    }
#endif

    if ( mAPTreePtr[Node].n_proc == no_proc && mAPTreePtr[Node].n_symb==s_parameter_name )
    {
        return true;
    }
    else
    {
        bool par = false;
        SyntaxTreeNodeNo N = mAPTreePtr[Node].n_lo_level;
        if ( N>0 )
            par |= ContainsParameter(N, true);
        N = mAPTreePtr[Node].n_sa_level;
        if ( N>0 && ConsiderSame && !par )
            par |= ContainsParameter(N, true);

        return par;
    }
}

bool Query_SyntaxTree::IsAggregationNode( SyntaxTreeNodeNo Node )
{
    if ( mAPTreePtr[Node].n_proc == no_proc )
    {
        switch ( mAPTreePtr[Node].n_symb )
        {
        case s_all_count:
        case s_count:
        case s_dis_count:
        case s_sum:
        case s_dis_sum:
        case s_avg:
        case s_dis_avg:
        case s_stddev:
        case s_dis_stddev:
        case s_variance:
        case s_dis_variance:
        case s_min:
        case s_max:
            return true;
            break;
        }
    }
    return false;
}

bool Query_SyntaxTree::IsBooleanFactorInCondition( SyntaxTreeNodeNo Node )
{

    SyntaxTreeNodeNo H = Node;
    SyntaxTreeNodeNo H2 = mAPTreePtr[H].n_refback;
    if ( H2>0 )
    {
		if ( mAPTreePtr[H2].n_proc == no_proc && mAPTreePtr[H2].n_symb == s_not )
			return false;

		while (H2 > 0)
		{
			if ( mAPTreePtr[H2].n_lo_level == H )
			{
				if (   mAPTreePtr[H2].n_proc == a63
					&& (   mAPTreePtr[H2].n_subproc() == cak_x_search_condition  
						|| mAPTreePtr[H2].n_subproc() == cak_x_having ) )
				{
					return true;
				}
				if ( !(mAPTreePtr[H2].n_proc == a64
					&& mAPTreePtr[H2].n_subproc() == cak_x_and_operator) )
				{
					return false;
				}
			}
			H  = H2;
			H2 = mAPTreePtr[H].n_refback;
		}

		// DDT : falsch ! nur refbacks , die auf lo_level zurückgehen müssen auf AND oder SERACH/HAVING stoßen !
        //do
        //{
        //    if (   mAPTreePtr[H].n_proc == a63
        //        && (   mAPTreePtr[H].n_subproc() == cak_x_search_condition  
        //            || mAPTreePtr[H].n_subproc() == cak_x_having ) )
        //    {
        //        return true;
        //    }
        //    if ( !(mAPTreePtr[H].n_proc == a64
        //        && mAPTreePtr[H].n_subproc() == cak_x_and_operator) )
        //    {
        //        return false;
        //    }
        //    H = mAPTreePtr[H].n_refback;
        //}while (H>0);
    }
    return false;
}

#endif