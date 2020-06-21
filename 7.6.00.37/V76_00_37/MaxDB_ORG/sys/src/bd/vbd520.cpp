/*!
  @file           vbd520.cpp
  @ingroup        Index
  @author         UweH
  @brief          OverflowHandling and UnderflowHandling on Tree Level

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
*/
/* PTS 1107674 UH 2000-09-13 m_create_index => m_create_index_parallel */
#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd500.h"
#include "gbd501.h"
#include "gbd600.h"

#include "heo60.h"   // vksprintf
#include "hbd102.h"  // PTS 1103980 JA 1999-09-15
#include "hgg01_1.h" // g01optextmsg
#include "hgg01_3.h" // g01glob, g01vtrace
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "DataAccess/Data_Types.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif
 
#define RIGHT_DISTRIBUTION_BD520    true
#define LEFT_DISTRIBUTION_BD520   ! RIGHT_DISTRIBUTION_BD520
#define SEPKEY_IN_NODE_BD520        true
#define WITH_LEFT_NODE_BD520        true
#define RECORD_IN_RIGHT_NODE        true
#define UPDATE_KEY                  true

/*---------------------------------------------------------------------------*/

inline void
bd520_CreateChangeKeyOrder (cbd600_Node         &Node,
                            tsp00_PageNo         neighborId,    // PTS 1112713 UH 2001-12-07
                            tbd501_NeighborKind  neighborKind,  // PTS 1112713 UH 2001-12-07
                            cbd501_IndexOrders  &NewOrders,
                            tsp00_Int4           KeyLen = -1)   // PTS 1103597 TS 1999-08-11
{
    tsp00_KeyPtr pKey;
    tsp00_Int2   KeyLength;
    Node.bd600GetKey (FIRST_REC_INDEX_BD00, pKey, KeyLength);
    NewOrders.bd501CreateNew (iokChange_ebd501, Node.bd600GetNodePtr(),
                              neighborId,   // PTS 1112713 UH 2001-12-07
                              neighborKind, // PTS 1112713 UH 2001-12-07
                              SEPKEY_IN_NODE_BD520,
                              pKey,
                              (-1 == KeyLen) ? KeyLength : KeyLen); // PTS 1103597 TS 1999-08-11
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520LeafCountPropagation (tsp00_KeyPtr  pKey,
                                        tsp00_Int4    KeyLen)
{
    ROUTINE_DBG_MEO00 ("bd520LeafCountPropagation");

    cbd501_IndexOrders NewOrders;

    if ( NULL != pKey )
    {
        m_Node.bd600Release (nr_for_update == m_Node.m_RequestKind);
        if ( m_Node.m_Current.bd300LockTreeExclusive() )
            if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                bd500_WriteTrace("bd520LeafCountPropagation",25);
        if (m_TrError != e_ok) return;
        bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
        if (m_TrError != e_ok) return;
    }
    else
    {
        /* if pKey is NULL m_Node is the correct LeafNode       */
        /* and the tree is locked correctly                     */
        /* but it has to be checked, if any action is necessary */

        if ( ! bd500PropagationNeeded() ) return;
    }

    bd520_Propagate (! UPDATE_KEY, NewOrders);
    if ( NewOrders.iosOrderCount_bd501 > 0 )
    {
        tsp00_Int4 DummyNumNodeDiff;
        bd510_ExcuteOrdersOnOneLevel (NewOrders, DummyNumNodeDiff);
    }
}


/*---------------------------------------------------------------------------*/

bool
cbd500_Tree::bd520LeafUnderFlow (tsp00_KeyPtr pKey,
                                 tsp00_Int4   KeyLen)
{
    ROUTINE_DBG_MEO00 ("bd520LeafUnderFlow");

    /* PTS 1106992 UH 2000-06-21 */
    /* the return value was added */

    if ( m_Node.bd600IsRoot()
         ||
         m_Node.bd600Covering() >= UNDER_FLOW_LIMIT_BD600 )
        return false;

    /* convert current tree-lock from leaf-exclusive to tree-exclusive */

    m_Node.bd600Release (IS_CHANGED_BD600);
    if (e_ok != m_Node.m_TrError) return false;

    if ( m_Node.m_Current.bd300LockTreeExclusive() )
        if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
            bd500_WriteTrace("bd520LeafUnderFlow",18);
    if (e_ok != m_Node.m_TrError) return false;

    /* Because there were intermediately no locks on the tree.   */
    /* The leaf must be searched again, which potentially        */
    /* can have an underflow.                                    */

    bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
    if (e_ok != m_Node.m_TrError) return false;

    cbd501_IndexOrders NewOrders; 
    tsp00_Int4         NumNodeDiff = 0;
    cbd600_Node        Neighbor (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
    cbd600_Node        AuxNode  (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update)); // PTS 1109619 UH 2001-03-13
    bool               bOrdersExisting; /* PTS 1106992 UH 2000-06-21 */

    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
        bd500Print ("bd520LeafUnderFlow"); // PTS 1112713 UH 2001-12-07

    bd520_UnderFlow (Neighbor, AuxNode, false, NumNodeDiff, NewOrders);

    bOrdersExisting = NewOrders.iosOrderCount_bd501 > 0;

    if (e_ok != m_Node.m_TrError) return bOrdersExisting;

    m_Node.m_Current.currNumLeaves_bd00 += NumNodeDiff;
    
    if ( bOrdersExisting )
        bd510_ExcuteOrdersOnOneLevel (NewOrders, NumNodeDiff);

    m_CurrLevel = LEAF_LEVEL_BD00;

    return bOrdersExisting;
}

/*---------------------------------------------------------------------------*/

bool
cbd500_Tree::bd520LeafOverflow (tsp00_KeyPtr  pKey,
                                tsp00_Int4    KeyLen,
                                bool          bInsNewRecord,
                                tsp00_Int4    RequiredLen,
                                tsp00_Int4   &RecIndex)
{
    ROUTINE_DBG_MEO00 ("bd520LeafOverflow");
    
    cbd501_IndexOrders NewOrders; 
    tsp00_Int4         NumNodeDiff = 0;
    cbd600_Node        Neighbor (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
    cbd600_Node        NextNeighbor (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
    bool               bOrdersExisting; /* PTS 1106992 UH 2000-06-21 */

    /* PTS 1106992 UH 2000-06-21 */
    /* the return value was added */
    
    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
        bd500Print ("bd520LeafOverFlow"); // PTS 1112713 UH 2001-12-07

    bd520_Overflow (Neighbor, NextNeighbor, pKey, KeyLen, 0,
        bInsNewRecord, false, RequiredLen, RecIndex, NumNodeDiff, NewOrders);

    bOrdersExisting = NewOrders.iosOrderCount_bd501 > 0;

    if (e_ok != m_TrError) return bOrdersExisting;
    
    m_Node.m_Current.currNumLeaves_bd00 += NumNodeDiff;
    
    if ( bOrdersExisting )
        bd510_ExcuteOrdersOnOneLevel (NewOrders, NumNodeDiff);
    
    m_CurrLevel = LEAF_LEVEL_BD00;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if ( e_ok != m_TrError )
        t01basis_error (bd_index, "m_TrError   ", m_TrError);
#   endif
    return bOrdersExisting;
}


/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

inline void
bd520_PrintBalancing (tbd501_BalancingType  BalancingType,
                      const cbd600_Node    &Node,
                      const cbd600_Node    &Neighbor)
{
    const tsp00_Int4   Level      = Node.bd600Level();
    const tsp00_PageNo NodeId     = Node.bd600GetId();
    const tsp00_PageNo NeighborId = Neighbor.bd600GetId();

    char s [sizeof(tsp00_C40)+1];
    // ........................................
    // 0000000000 Node --> 0000000000 Neighbor            
    switch (BalancingType)
    {
    case btDistToRight_ebd501:
        vksprintf( s, "%1d %10d Node -> %10d Neighbor", Level, NodeId, NeighborId );
        break;
    case btDistFromRight_ebd501:
        vksprintf( s, "%1d %10d Node <- %10d Neighbor", Level, NodeId, NeighborId );
        break;
    case btDistToLeft_ebd501:
        vksprintf( s, "%1d %10d Neighbor <- %10d Node", Level, NeighborId, NodeId );
        break;
    case btDistFromLeft_ebd501:
        vksprintf( s, "%1d %10d Neighbor -> %10d Node", Level, NeighborId, NodeId );
        break;
    case btDistToNextRight_ebd501:
        vksprintf( s, "%1d %10d Node -->%10d Neighbor", Level, NodeId, NeighborId );
        break;
    case btMergeFromRight_ebd501:
        vksprintf( s, "%1d %10d Node <= %10d Neighbor", Level, NodeId, NeighborId );
        break;
    case btMergeFromLeft_ebd501:
        vksprintf( s, "%1d %10d Neighbor => %10d Node", Level, NeighborId, NodeId );
        break;
    case btAppend_ebd501:
        vksprintf( s, "%1d append to %10d Neighbor         ", Level, NeighborId );
        break;
    case btRemoveEmpty_ebd501:
        vksprintf( s, "%1d delete empty Node %10d          ", Level, NodeId );
        break;
    case btPropagate_ebd501:
        vksprintf( s, "%1d propagate from Node %10d        ", Level, NodeId );
        break;
    case btPropagateWithKey_ebd501:
        vksprintf( s, "%1d propagate from Node %10d (wkey) ", Level, NodeId );
        break;
    case btNewLevel_ebd501:
        vksprintf( s, "%1d %10d Root +> Neighbor %10d", Level, NodeId, NeighborId );
        break;
    case btRemoveLevel_ebd501:
        vksprintf( s, "%1d %10d Root remove level          ", Level, NodeId);
        break;
    default: 
        vksprintf( s, "Unknown balancing type                  " );
        break;
    }
    g01optextmsg (sp3p_nil, sp3m_info, csp3_bd_msg, csp3_n_btree, s);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_Overflow (cbd600_Node        &Neighbor,
                             cbd600_Node        &NextNeighbor,
                             tsp00_KeyPtr        pKey,
                             tsp00_Int4          KeyLen,
                             tsp00_Int4          LeafCount,     
                             bool                bInsNewRecord,
                             bool                bFirstRecUpdated,
                             tsp00_Int4          RequiredLen,
                             tsp00_Int4         &RecIndex,
                             tsp00_Int4         &NumNodeDiff,
                             cbd501_IndexOrders &NewOrders)
{
    /* POST: m_Node holds the page, where the new record has to be inserted */
    /*       Neighbor is necessary to hold, in the case a new separator-key */
    /*       is needed for that page                                        */
    ROUTINE_DBG_MEO00 ("bd520_Overflow");

    tsp00_PageNo RightNeighbor    = m_Node.m_pNode->nd_right();
    bool         bUpdateNptrNode  = false;
    bool         bNewNodeInserted = false;
    tsp00_KeyPtr pSepKey;
    tsp00_Int2   SepKeyLen;
    
    /* check whether the record should be appended */

    if ( NIL_PAGE_NO_GG00 == RightNeighbor )
    {
        if ( (RecIndex > m_Node.bd600MaxRecIndex()) && bInsNewRecord )
        {
            /* append optimization, i.e. introduce a new right neighbour page where the new */
            /* record has to be inserted without balancing the new and the current page     */  
            if ( Neighbor.bd600IsAccessable() ) // PTS 1130325 UH 2004-06-25
            {
                // PTS 1109033 UH 2000-01-19 if-block added
                NextNeighbor.bd600Exchange(Neighbor);
            }
            Neighbor.bd600New (m_Node.m_pNode->nd_level());
            if (e_ok != m_TrError)
            {
				// PTS 1130325 UH 2004-06-25
                bd500Print            (     "Neighbor.bd600New"       );
                m_Node.bd600Dump      (4711,"Neighbor.bd600New...node");
                Neighbor.bd600Dump    (4711,"Neighbor.bd600New...neig");
                NextNeighbor.bd600Dump(4711,"Neighbor.bd600New...next");
                g01abort (csp3_bd_msg, csp3_n_btree,"bd600New failed         ",4711);
                return;
            }
            
            ++NumNodeDiff;
            Neighbor.m_pNode->nd_right()         =  RightNeighbor;
            m_Node.m_pNode->nd_right()           =  Neighbor.m_pNode->nd_id();
            if ( bd500IsClustered() )
                Neighbor.m_pNode->nd_left()      =  m_Node.m_pNode->nd_id();
            Neighbor.m_pNode->ndLeafCount_bd00() += LeafCount;
            bNewNodeInserted                     =  true;

            /* create separator between new and current page  */

            m_Node.bd600BuildSeparatorKey (IS_LEFT_NODE_GBD600, pKey, KeyLen, pSepKey, SepKeyLen);
            if (e_ok != m_TrError)
            {
				// PTS 1130325 UH 2004-06-25
                bd500Print            (     "m_Node.bd600BuildSeparatorKey");
                m_Node.bd600Dump      (4712,"m_Node                  "     );
                Neighbor.bd600Dump    (4712,"Neigbor                 "     );
                NextNeighbor.bd600Dump(4712,"NextNeighbor            "     );
                g01abort (csp3_bd_msg, csp3_n_btree,"bd600BuildSep failed    ",4712);
                return;
            }

            if ( m_Node.bd600IsRoot() )
            {
                cbd600_Node NewLeftSubNode (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));

                bd510_InsertNewLevel (Neighbor, pSepKey, SepKeyLen,
                    FIRST_REC_INDEX_BD00+1, RECORD_IN_RIGHT_NODE, NewLeftSubNode, NumNodeDiff);
                if (e_ok != m_TrError)
                {
					// PTS 1130325 UH 2004-06-25
                    bd500Print            (     "bd510_InsertNewLevel");
                    m_Node.bd600Dump      (4713,"m_Node                  "     );
                    Neighbor.bd600Dump    (4713,"Neigbor                 "     );
                    NextNeighbor.bd600Dump(4713,"NextNeighbor            "     );
                    g01abort (csp3_bd_msg, csp3_n_btree,"bd510_InsertNew failed  ",4713);
                    return;
                }

                if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                    bd520_PrintBalancing (btNewLevel_ebd501, m_Node, Neighbor);
            }
            else
            {
                if ( m_create_index_parallel == m_Node.m_Current.curr_action ) /* PTS 1104650 UH 21-11-1999 */
                {
                    /* especially for CreateIndex: cbd400:InvTree::bd400AppendPrimKey() */
                    NewOrders.bd501CreateNew ( iokChange_ebd501, m_Node.m_pNode,
                                               Neighbor.bd600GetId(), rightNeighbor_ebd501); // PTS 1112713 UH 2001-12-07
                }

                NewOrders.bd501CreateNew ( iokAdd_ebd501, Neighbor.m_pNode,
                                           m_Node.bd600GetId(), leftNeighbor_ebd501, // PTS 1112713 UH 2001-12-07
                                           ! SEPKEY_IN_NODE_BD520, pSepKey, SepKeyLen);

                if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                    bd520_PrintBalancing (btAppend_ebd501, m_Node, Neighbor);
            }
            /* set Node and RecIndex to the page and the Index */
            /* where the record will be inserted               */
            m_Node.bd600Exchange (Neighbor);
            RecIndex = FIRST_REC_INDEX_BD00;
            // PTS 1112713 UH 2001-12-07 update especially the leftId for following orders
            m_PathInfo[m_CurrLevel].piLeftId_bd500 = NIL_PAGE_NO_GG00;


//            PTS 1112713 UH 2001-12-07 not neccessary anymore
//
//            if ( 1 == NewOrders.iosOrderCount_bd501 )
//            {
//                /* there is no order before this append order, so the PathInfo is set */
//                /* to the current node for this level                                 */
//                /* In bd510_ExcuteOrdersOnOneLevel is this used to determine, if the  */
//                /* orders are starting with the left separator                        */
//                m_PathInfo[m_CurrLevel].piId_bd500 = m_Node.m_pNode->nd_id();
//            }
            return;
        }
        else
        {
            m_TrError = e_no_more_space;
        }
    }

    /* try to balance the current page with ist right neighbor page */
    if ( NIL_PAGE_NO_GG00 != RightNeighbor )
    {
        // PTS 1109033 UH 2000-01-19 begin
        if ( Neighbor.bd600IsAccessable() ) // PTS 1130325 UH 2004-06-25
        {
            if ( RightNeighbor != Neighbor.bd600GetId() )
            {
                NextNeighbor.bd600Exchange(Neighbor);
                Neighbor.bd600GetNode (RightNeighbor); /* PTS 1109619 UH 2001-03-13 */
            }
        }
        else
            Neighbor.bd600GetNode (RightNeighbor);
        // PTS 1109033 UH 2000-01-19 end

        if (e_ok != m_TrError)
        {
			// PTS 1130325 UH 2004-06-25
            bd500Print            (     "RightNeigbor access failed");
            m_Node.bd600Dump      (4714,"m_Node                  "     );
            Neighbor.bd600Dump    (4714,"Neigbor                 "     );
            NextNeighbor.bd600Dump(4714,"NextNeighbor            "     );
            g01abort (csp3_bd_msg, csp3_n_btree,"right distrib error     ",4714);
            return;
        }

        if ( (m_Node.bd600Covering() + Neighbor.bd600Covering() + RequiredLen) > (2 * FULLCOVERING_BD00) )
        {
            Neighbor.bd600Release (! IS_CHANGED_BD600);
            m_TrError = e_no_more_space;
        }
        else
        {
            tgg00_MessType_Enum DistribReason;

            if (bInsNewRecord)
                DistribReason = m_insert;
            else
                DistribReason = m_update;

            bd520_DistributeRight (Neighbor, NextNeighbor, bNewNodeInserted, bFirstRecUpdated,
                DistribReason, RequiredLen, pKey, KeyLen, LeafCount, RecIndex, NumNodeDiff,
                NewOrders);
            
            if (e_ok != m_TrError)
            {
                Neighbor.bd600Release (! IS_CHANGED_BD600);
                if (e_no_more_space != m_TrError) return;
            }
        }
    }

    /* try balancing with the left neighbor page */

    if ( (e_no_more_space == m_TrError)
         &&
         (m_create_index_parallel != m_Node.m_Current.curr_action) ) /* PTS 1105471 UH 28-01-2000 */
    {
        m_TrError = e_ok;

        tsp00_PageNo LeftNeighbor = bd500LeftNeighbor();
        if ( e_ok != m_TrError )
        {
			// PTS 1130325 UH 2004-06-25
            bd500Print            (     "bd500LeftNeighbor failed");
            m_Node.bd600Dump      (4715,"m_Node                  "     );
            Neighbor.bd600Dump    (4715,"Neigbor                 "     );
            NextNeighbor.bd600Dump(4715,"NextNeighbor            "     );
            g01abort (csp3_bd_msg, csp3_n_btree,"left distrib error      ",4715);
            return;
        }

        if ( NIL_PAGE_NO_GG00 == LeftNeighbor )
            m_TrError = e_no_more_space;
        else
        {
            // PTS 1109033 UH 2001-01-19 begin
            if ( Neighbor.bd600IsAccessable() ) // PTS 1130325 UH 2004-06-25
            {
                if ( LeftNeighbor != Neighbor.bd600GetId() )
                {
                    NextNeighbor.bd600Exchange(Neighbor);
                    Neighbor.bd600GetNode (LeftNeighbor); /* PTS 1109619 UH 2001-03-13 */
                }
            }
            else
				// PTS 1130325 UH 2004-06-25 in addition check NextNeighbor too
                if ( NextNeighbor.bd600IsAccessable() )
                {
                    if ( LeftNeighbor == NextNeighbor.bd600GetId() )
                        Neighbor.bd600Exchange(NextNeighbor);
                    else
                    {
                        bd500Print            (     "NextNeighbor unexpected occupied");
                        m_Node.bd600Dump      (4718,"m_Node                  "     );
                        Neighbor.bd600Dump    (4718,"Neigbor                 "     );
                        NextNeighbor.bd600Dump(4718,"NextNeighbor            "     );
                        g01abort (csp3_bd_msg, csp3_n_btree,"NextNeighbor unexpected ",4718);
                    }
                }
                else
                    Neighbor.bd600GetNode (LeftNeighbor);
            // PTS 1109033 UH 2001-01-19 end

            if ( e_ok != m_TrError )
            {
				// PTS 1130325 UH 2004-06-25
                bd500Print            (     "LeftNeighbor access failed");
                m_Node.bd600Dump      (4716,"m_Node                  "     );
                Neighbor.bd600Dump    (4716,"Neigbor                 "     );
                NextNeighbor.bd600Dump(4716,"NextNeighbor            "     );
                g01abort (csp3_bd_msg, csp3_n_btree,"LeftNeighbor access fail",4716);
            }
            else
            {
                if (((m_Node.bd600Covering() + Neighbor.bd600Covering() + RequiredLen) >
                    2 * FULLCOVERING_BD00) || 
                    (0 == Neighbor.bd600NumRecords ()) // circumvention, until new branchhandling is introduced
                    )
                {
                    Neighbor.bd600Release (! IS_CHANGED_BD600);
                    m_TrError = e_no_more_space;
                }
                else
                {
                    tgg00_MessType_Enum DistribReason;

                    if (bInsNewRecord)
                        DistribReason = m_insert;
                    else
                        DistribReason = m_update;

                    bd520_DistributeLeft (Neighbor, DistribReason, RequiredLen, 
                        pKey, KeyLen, LeafCount, RecIndex, NewOrders);
            
                    if (e_ok != m_TrError)
                    {
                        Neighbor.bd600Release (! IS_CHANGED_BD600);
                        if (e_no_more_space != m_TrError) return;
                    }
                }
            }
        }
    }

    /* introduce a new page and balance with this new page */ 
    if (e_no_more_space == m_TrError)
    {
        m_TrError = e_ok;

        if ( Neighbor.bd600IsAccessable() ) // PTS 1130325 UH 2004-06-25
        {
            // PTS 1109033 UH 2000-01-19 if-block added
            NextNeighbor.bd600Exchange(Neighbor);
        }

        Neighbor.bd600New (m_Node.m_pNode->nd_level());
        if (e_ok != m_TrError)
        {
			// PTS 1130325 UH 2004-06-25
            bd500Print            (     "New right neighbor failed");
            m_Node.bd600Dump      (4717,"m_Node                  "     );
            Neighbor.bd600Dump    (4717,"Neigbor                 "     );
            NextNeighbor.bd600Dump(4717,"NextNeighbor            "     );
            g01abort (csp3_bd_msg, csp3_n_btree,"New right neighbor faile",4717);
            return;
        }

        ++NumNodeDiff;
        bNewNodeInserted             = true;
        Neighbor.m_pNode->nd_right() = RightNeighbor;
        m_Node.m_pNode->nd_right()   = Neighbor.m_pNode->nd_id();
        if ( bd500IsClustered() )
        {
            Neighbor.m_pNode->nd_left()  = m_Node.m_pNode->nd_id();
            if ( NextNeighbor.bd600IsAccessable() )
            {
                if ( NextNeighbor.bd600GetId() == RightNeighbor )
                    NextNeighbor.m_pNode->nd_left() = Neighbor.bd600GetId();
                else
                {
                    cbd600_Node RightNode (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
                    if (e_ok != m_TrError) return;
                    RightNode.m_pNode->nd_left() = Neighbor.bd600GetId();
                }
            }
            else
            {
                if ( Neighbor.m_pNode->nd_right() != NIL_PAGE_NO_GG00 )
                {
                    NextNeighbor.bd600GetNode(Neighbor.m_pNode->nd_right());
                    if (e_ok != m_TrError) return;
                    NextNeighbor.m_pNode->nd_left() = Neighbor.bd600GetId();
                }
            }
        }

        tgg00_MessType_Enum DistribReason;

        if (bInsNewRecord)
            DistribReason = m_insert;
        else
            DistribReason = m_update;

        bd520_DistributeRight (Neighbor, NextNeighbor, bNewNodeInserted, bFirstRecUpdated,
            DistribReason, RequiredLen, pKey, KeyLen, LeafCount, RecIndex, NumNodeDiff,
            NewOrders);
        
        if (e_ok != m_TrError)
        {
            // this variant (New Node) must be possible
            // if an error occures it is serious
            tgg00_BasisError AuxTrError = m_TrError;
            m_TrError = e_ok;
            Neighbor.bd600Free ();
            m_TrError = AuxTrError;
            return;
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if ( e_ok != m_TrError )
        t01basis_error (bd_index, "m_TrError   ", m_TrError);
#   endif
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_DistributeRight (cbd600_Node         &Neighbor,
                                    cbd600_Node         &NextNeighbor,
                                    bool                 bNewNodeInserted,
                                    bool                 bFirstRecUpdated,
                                    tgg00_MessType_Enum  DistribReason,
                                    tsp00_Int4           RequiredLen,
                                    tsp00_KeyPtr         pKey,
                                    tsp00_Int4           KeyLen,
                                    tsp00_Int4           LeafCount,
                                    tsp00_Int4          &RecIndex,
                                    tsp00_Int4          &NumNodeDiff,
                                    cbd501_IndexOrders  &NewOrders)
{

    ROUTINE_DBG_MEO00 ("bd520_DistributeRight");

    const tsp00_Int4 NodeLeafCount           = m_Node.bd600LeafCount();   /* PTS 1104320 UH 27-10-1999 */
    const tsp00_Int4 NeighborLeafCount       = Neighbor.bd600LeafCount(); /* PTS 1104320 UH 27-10-1999 */
    bool             bDistributionFound      = false;
    bool             bMoveRecordIntoNeighbor = false;
    tsp00_Int4       OptimRecIndex           = 0;              
    tsp00_Int4       AuxRecIndex;   
    tsp00_KeyPtr     pSepKey;
    tsp00_Int2       SepKeyLen;

    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        m_Node.bd600Print();
        Neighbor.bd600Print();
    }
    #endif

    if ( (m_create_index_parallel == m_Node.m_Current.curr_action)
         &&
         bNewNodeInserted
         &&
         (NIL_PAGE_NO_GG00 == Neighbor.m_pNode->nd_right())
         &&
         (RecIndex         == m_Node.bd600MaxRecIndex()) )
    {
        /* UH 28-10-1999 */

        /* Update optimization especially for CreateIndex */
        OptimRecIndex           = RecIndex;
        bDistributionFound      = true;
        bMoveRecordIntoNeighbor = true;
        /* In bd510_ExcuteOrdersOnOneLevel is this used to determine, if the  */
        /* orders are starting with the left separator                        */
        /* This is not an append, it MOVES the last record into the new page  */
        /* PTS 1104700 UH 18-11-1999
        m_PathInfo[m_CurrLevel].piId_bd500 = Neighbor.m_pNode->nd_id(); */
    }
    else
    {
        /* find the optimum distribution of the new record     */ 
        /* and those of pNode and pNeighbor at pNode and pNeighbor */
        if (m_insert == DistribReason)
        {
            m_Node.bd620_FindOptimDistribForInsert (RIGHT_DISTRIBUTION_BD520, 
              RequiredLen, Neighbor.bd600Covering(), RecIndex, bDistributionFound,
              OptimRecIndex, bMoveRecordIntoNeighbor);
        }
        else // update record or delete record
        {
            m_Node.bd620_FindOptimDistribForUpdate (RIGHT_DISTRIBUTION_BD520, 
              RequiredLen, Neighbor.bd600Covering(), RecIndex, bDistributionFound,
              OptimRecIndex, bMoveRecordIntoNeighbor);
        }
        if (e_ok != m_TrError) return;
    }

    if (!bDistributionFound)
    {
        if (Neighbor.bd600NumRecords() > 0)
        {
            /* balancing with this neighbor impossible */
            m_TrError = e_no_more_space;
            return;
        }
        else
        {
            /* although neighbor is empty balancing is impossible => get second empty page */
            bd520_ThreeNodeDistribution (Neighbor, NextNeighbor, DistribReason, bNewNodeInserted,
               bFirstRecUpdated, pKey, KeyLen, LeafCount, RecIndex, NewOrders);

            /* increase counter of nodes which were introduced to get the space of RequiredLen */
            ++NumNodeDiff;
        }
    }
    else
    {
        /* distribute the new record and those of pNode and pNeighbor at pNode and pNeighbor */
        tsp00_Int4    RecLen;
        bool          withConsistentReadInfo;
        tsp00_BytePtr pSourceRec;
        tsp00_BytePtr pTargetRec;
        bool          bSepKeyNotInNode = (OptimRecIndex == RecIndex);
        tsp00_Int4    RecLeafCount; /* PTS 1104320 UH 27-10-1999 */

        /* move all records with Indexes <= OptimRecIndex from pNode to pNeighbor */
        for (AuxRecIndex = OptimRecIndex; AuxRecIndex <= m_Node.bd600MaxRecIndex(); ++AuxRecIndex)
        {
            RecLeafCount = m_Node.bd600LeafCount(AuxRecIndex, AuxRecIndex); /* PTS 1104320 UH 27-10-1999 */

            m_Node.m_RecordList.GetRecord (AuxRecIndex, withConsistentReadInfo, RecLen, pSourceRec);

            Neighbor.bd600AddRecordSpace ( RecLen + POINTERSIZE_BD00,
                                           AuxRecIndex - OptimRecIndex,
                                           pTargetRec,
                                           false );
            if (e_ok != m_TrError)
                return;

            SAPDB_RangeMove( __FILE__, 1,
                MAX_RECLEN_GG00,     sizeof(tbd_node),
                pSourceRec,          POS_OFF_DIFF_BD00,
                pTargetRec,          POS_OFF_DIFF_BD00,
                RecLen,              m_TrError);
            if (e_ok != m_TrError)
                return;
            
            /* PTS 1104320 UH 27-10-1999 */
            m_Node.bd600LeafCount()   -= RecLeafCount;
            Neighbor.bd600LeafCount() += RecLeafCount;
        }

        /* delete spaces of moved records on pNode */
        m_Node.bd620_DeleteRecordsFromPage (RIGHT_DISTRIBUTION_BD520, OptimRecIndex);
        if (e_ok != m_TrError) return;

        if (g01glob.treecheck)
        {
            m_Node.bd600CheckWithErrorHandling();
            Neighbor.bd600CheckWithErrorHandling();
        }

        /* add the additional LeafCount */
        if ( bMoveRecordIntoNeighbor )
            Neighbor.m_pNode->ndLeafCount_bd00() += LeafCount;
        else
            m_Node.m_pNode->ndLeafCount_bd00() += LeafCount;

        #ifdef SAPDB_SLOW 
        if ( DataIndex_Trace.TracesLevel(6) )
        {
            t01sname (bd_oflw, "After Distri");
            m_Node.bd600Print();
            Neighbor.bd600Print();
        }
        #endif

        /* build orders to restructure and update the index */

        if ( ! m_Node.bd600IsRoot() )
        {
            if ( bFirstRecUpdated )
                bd520_CreateChangeKeyOrder ( m_Node,
                                             Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                             rightNeighbor_ebd501,  // PTS 1112713 UH 2001-12-07
                                             NewOrders, KeyLen);    // PTS 1103597 TS 1999-08-11
            else
            {
                if ( (NodeLeafCount  != m_Node.bd600LeafCount()      ) ||
                     (m_create_index_parallel == m_Node.m_Current.curr_action )  )
                {
                    NewOrders.bd501CreateNew ( iokChange_ebd501,
                                               m_Node.m_pNode,
                                               Neighbor.bd600GetId(),  // PTS 1112713 UH 2001-12-07
                                               rightNeighbor_ebd501 ); // PTS 1112713 UH 2001-12-07
                }
            }
        }

        if ( bSepKeyNotInNode )
        {
            if ( bMoveRecordIntoNeighbor )
                m_Node.bd600BuildSeparatorKey (IS_LEFT_NODE_GBD600,
                    pKey, KeyLen, pSepKey, SepKeyLen);
            else
                Neighbor.bd600BuildSeparatorKey (IS_RIGHT_NODE_GBD600,
                    pKey, KeyLen, pSepKey, SepKeyLen);
        }
        else
        {
            m_Node.bd600BuildSeparatorKey (Neighbor, pSepKey, SepKeyLen);
        }

        if (e_ok != m_TrError) return; 

        if ( m_Node.bd600IsRoot() )
        {
            cbd600_Node NewLeftSubNode (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));

            if ( bMoveRecordIntoNeighbor )
                RecIndex -= OptimRecIndex;

            bd510_InsertNewLevel (Neighbor, pSepKey, SepKeyLen,
                RecIndex, bMoveRecordIntoNeighbor, NewLeftSubNode, NumNodeDiff);
            if (e_ok != m_TrError) return; 

            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd520_PrintBalancing (btNewLevel_ebd501, m_Node, Neighbor);

            if ( bMoveRecordIntoNeighbor )
                m_Node.bd600Exchange(Neighbor);
            else
                m_Node.bd600Exchange(NewLeftSubNode);

            /* a following order may use Neighbor */
            Neighbor.bd600Release(IS_CHANGED_BD600);
        }
        else
        {
            NewOrders.bd501CreateNew ((bNewNodeInserted)?(iokAdd_ebd501):(iokChange_ebd501),
                                      Neighbor.m_pNode,
									  m_Node.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                      leftNeighbor_ebd501, // PTS 1112713 UH 2001-12-07
									  ! bSepKeyNotInNode, pSepKey, SepKeyLen);

            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd520_PrintBalancing (btDistToRight_ebd501, m_Node, Neighbor);

            /*  update record index and TargetPage if the new record has to be inserted into the neighbor page */
            if (bMoveRecordIntoNeighbor)
            {
                m_Node.bd600Exchange (Neighbor);
                RecIndex -= OptimRecIndex;
                // if ( bNewNodeInserted )
                // PTS 1112713 UH 2001-12-07
                // inserted block to allow succeeding order to work properly
                {
					m_PathInfo[m_CurrLevel].piId_bd500      = m_Node.bd600GetId();
                    m_PathInfo[m_CurrLevel].piRightId_bd500 = m_Node.bd600RightNeighbor();
					m_PathInfo[m_CurrLevel].piLeftId_bd500  = Neighbor.bd600GetId();
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_DistributeLeft (cbd600_Node         &Neighbor,
                                   tgg00_MessType_Enum  DistribReason,
                                   tsp00_Int4           RequiredLen,
                                   tsp00_KeyPtr         pKey,
                                   tsp00_Int4           KeyLen,
                                   tsp00_Int4           LeafCount,
                                   tsp00_Int4          &RecIndex,
                                   cbd501_IndexOrders  &NewOrders)
{
    ROUTINE_DBG_MEO00 ("bd520_DistributeLeft");
        
    const tsp00_Int4 NodeLeafCount           = m_Node.bd600LeafCount();   /* PTS 1104320 UH 27-10-1999 */
    const tsp00_Int4 NeighborLeafCount       = Neighbor.bd600LeafCount(); /* PTS 1104320 UH 27-10-1999 */
    bool             bDistributionFound      = false;
    bool             bMoveRecordIntoNeighbor = false;
    tsp00_Int4       OptimRecIndex           = 0;
    tsp00_KeyPtr     pSepKey;
    tsp00_Int2       SepKeyLen;

    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        t01sname (bd_oflw, "before distr");
        m_Node.bd600Print();
        Neighbor.bd600Print();
    }
    #endif
    
    /* find the optimum distribution of the the new record  */ 
    /* and those of Node and Neighbor at Node and Neighbor  */
    if (m_insert == DistribReason)
    {
        m_Node.bd620_FindOptimDistribForInsert (LEFT_DISTRIBUTION_BD520,
            RequiredLen, Neighbor.bd600Covering(), RecIndex, bDistributionFound,
            OptimRecIndex, bMoveRecordIntoNeighbor);
    }
    else // update record and delete record
    {
        m_Node.bd620_FindOptimDistribForUpdate (LEFT_DISTRIBUTION_BD520, 
            RequiredLen, Neighbor.bd600Covering(), RecIndex, bDistributionFound, 
            OptimRecIndex, bMoveRecordIntoNeighbor);
    }
    
    if (e_ok != m_TrError) return;
    
    if (!bDistributionFound)
    {       
        m_TrError = e_no_more_space;
        return;
    }
    else
    {
        tsp00_Int4    RecLen;
        bool          withConsistentReadInfo;
        tsp00_BytePtr pSourceRec;
        tsp00_BytePtr pTargetRec;
        tsp00_Int4    AuxRecIndex; 
        tsp00_Int4    RecLeafCount; /* PTS 1104320 UH 27-10-1999 */
        const tsp00_Int4   NeighborOldMaxRecIndex = Neighbor.bd600MaxRecIndex();;
        const bool         bSepKeyNotInNode = ((OptimRecIndex + 1) == RecIndex);

        /* distribute the new record and those of pNode and pNeighbor at pNode and pNeighbor */
        /* move all records with Indexes <= OptimRecIndex from pNode to pNeighbor            */
        for (AuxRecIndex = FIRST_REC_INDEX_BD00; AuxRecIndex <= OptimRecIndex; ++AuxRecIndex)
        {
            RecLeafCount = m_Node.bd600LeafCount(AuxRecIndex, AuxRecIndex); /* PTS 1104320 UH 27-10-1999 */

            m_Node.m_RecordList.GetRecord (AuxRecIndex, withConsistentReadInfo, RecLen, pSourceRec);

            Neighbor.bd600AddRecordSpace ( RecLen + POINTERSIZE_BD00,
                                           NeighborOldMaxRecIndex + AuxRecIndex + 1,
                                           pTargetRec,
                                           false );
            if (e_ok != m_TrError)
                return;
            
            SAPDB_RangeMove( __FILE__, 2,
                MAX_RECLEN_GG00, sizeof(tbd_node),
                pSourceRec,      POS_OFF_DIFF_BD00,
                pTargetRec,      POS_OFF_DIFF_BD00,
                RecLen, m_TrError);

            if (e_ok != m_TrError)
                return;
            
            /* PTS 1104320 UH 27-10-1999 */
            m_Node.bd600LeafCount()   -= RecLeafCount;
            Neighbor.bd600LeafCount() += RecLeafCount;
        }
        
        /* delete spaces of moved records on pNode */
        m_Node.bd620_DeleteRecordsFromPage (LEFT_DISTRIBUTION_BD520, OptimRecIndex);
        if (e_ok != m_TrError) return; 
        
        if (g01glob.treecheck)
        {
            m_Node.bd600CheckWithErrorHandling();
            Neighbor.bd600CheckWithErrorHandling();
        }

        if ( bMoveRecordIntoNeighbor )
            Neighbor.m_pNode->ndLeafCount_bd00() += LeafCount;
        else
            m_Node.m_pNode->ndLeafCount_bd00() += LeafCount;

        #ifdef SAPDB_SLOW 
        if ( DataIndex_Trace.TracesLevel(6) )
        {
            t01sname (bd_oflw, "after distr ");
            m_Node.bd600Print();
            Neighbor.bd600Print();
            t01int4  (bd_oflw, "New RecIndex", RecIndex);
        }
        #endif

        /* build orders to restructure and update the index */

        // NewOrders.iosOrderCount_bd501 = 0; // PTS 1112713 UH 2001-12-07

        if ( NeighborLeafCount != Neighbor.bd600LeafCount() ) /* PTS 1104320 UH 27-10-1999 */
            NewOrders.bd501CreateNew ( iokChange_ebd501,
                                       Neighbor.m_pNode,
			                           m_Node.bd600GetId(),    // PTS 1112713 UH 2001-12-07
                                       rightNeighbor_ebd501 ); // PTS 1112713 UH 2001-12-07

        if ( bSepKeyNotInNode )
        {
            if ( bMoveRecordIntoNeighbor )
                m_Node.bd600BuildSeparatorKey (IS_RIGHT_NODE_GBD600,
                    pKey, KeyLen, pSepKey, SepKeyLen);
            else
                Neighbor.bd600BuildSeparatorKey (IS_LEFT_NODE_GBD600,
                    pKey, KeyLen, pSepKey, SepKeyLen);
        }
        else
        {
            Neighbor.bd600BuildSeparatorKey (m_Node, pSepKey, SepKeyLen);
        }

        if (e_ok != m_TrError) return; 
        
        NewOrders.bd501CreateNew ( iokChange_ebd501, m_Node.m_pNode,
			                       Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                   leftNeighbor_ebd501,   // PTS 1112713 UH 2001-12-07
                                   ! bSepKeyNotInNode, pSepKey, SepKeyLen);

        if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
            bd520_PrintBalancing (btDistToLeft_ebd501, m_Node, Neighbor);

        /* update Index of the record and TargetPage                   */
        /* if the new record has to be inserted into the neighbor page */

        if (bMoveRecordIntoNeighbor)
        {
            m_Node.bd600Exchange (Neighbor);
            RecIndex = NeighborOldMaxRecIndex + RecIndex + 1;
            
            m_PathInfo[m_CurrLevel].piId_bd500      = m_Node.bd600GetId();
            m_PathInfo[m_CurrLevel].piRightId_bd500 = m_Node.bd600RightNeighbor();
            m_PathInfo[m_CurrLevel].piLeftId_bd500  = NIL_PAGE_NO_GG00;
        }
        else
            RecIndex = RecIndex - (OptimRecIndex  + 1);
    }
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_ThreeNodeDistribution (cbd600_Node         &Neighbor,
                                          cbd600_Node         &NextNeighbor,
                                          tgg00_MessType_Enum  DistribReason,
                                          bool                 bNewNodeInserted,
                                          bool                 bFirstRecUpdated,
                                          tsp00_KeyPtr         pKey,
                                          tsp00_Int4           KeyLen,
                                          tsp00_Int4           LeafCount,
                                          tsp00_Int4          &RecIndex,
                                          cbd501_IndexOrders  &NewOrders)
{
    ROUTINE_DBG_MEO00 ("bd520_ThreeNodeDistribution");

                                        ////////////////////////////////////////////////////////
    m_TrError = e_not_implemented;      // UH 2005-11-15, would only be needed by primary trees 
    return;                             ////////////////////////////////////////////////////////
    
    /* a new page is introduced wich becomes the next neighbor page of NptrsNode    */
    /* case DistribReason == m_update:                                              */
    /* all records with indexes < RecIndex are moved to this next neighbor page the */
    /* record with the index == RecIndex is moved to the neighbor page              */
    /* case DistribReason == m_insert:                                              */
    /* all  records with indexes < RecIndex are moved to the next neighbor page     */
    
    const tsp00_Int4 NodeLeafCount = m_Node.bd600LeafCount(); /* PTS 1104320 UH 27-10-1999 */

    tsp00_Int4    RecLen;
    bool          withConsistentReadInfo;
    tsp00_BytePtr pSourceRec;
    tsp00_BytePtr pTargetRec;
    tsp00_Int4    StartIndex    = RecIndex;
    tsp00_Int4    AuxRecIndex;
    tsp00_KeyPtr  pSepKey;
    tsp00_Int2    SepKeyLen;
    tsp00_Int4    RecLeafCount; /* PTS 1104320 UH 27-10-1999 */


    NextNeighbor.bd600New (m_Node.m_pNode->nd_level());
    if (e_ok != m_TrError) return;
    
    if (m_update == DistribReason) 
    {
        StartIndex = RecIndex+1;
        
        RecLeafCount = m_Node.bd600LeafCount(RecIndex, RecIndex); /* PTS 1104320 UH 27-10-1999 */

        /* move record which is to update into the NeighborPage */
        m_Node.m_RecordList.GetRecord (RecIndex, withConsistentReadInfo, RecLen, pSourceRec);
        
        Neighbor.bd600AddRecordSpace ( RecLen + POINTERSIZE_BD00,
                                       FIRST_REC_INDEX_BD00,
                                       pTargetRec,
                                       false );
        if (e_ok != m_TrError)
            return;

        SAPDB_RangeMove( __FILE__, 3,
            MAX_RECLEN_GG00, sizeof(tbd_node),
            pSourceRec,      POS_OFF_DIFF_BD00,
            pTargetRec,      POS_OFF_DIFF_BD00,
            RecLen,          m_TrError);

        if (e_ok != m_TrError)
            return;

        /* PTS 1104320 UH 27-10-1999 */
        m_Node.bd600LeafCount()   -= RecLeafCount;
        Neighbor.bd600LeafCount() += RecLeafCount;
    }

    /* move all records with Indexes <= StartIndex into the NextNeighborPage */
    for ( AuxRecIndex = StartIndex;
         (AuxRecIndex <= m_Node.bd600MaxRecIndex()) && (e_ok == m_TrError);
         ++AuxRecIndex)
    {
        RecLeafCount = m_Node.bd600LeafCount(AuxRecIndex, AuxRecIndex); /* PTS 1104320 UH 27-10-1999 */

        m_Node.m_RecordList.GetRecord (AuxRecIndex, withConsistentReadInfo, RecLen, pSourceRec);
        
        NextNeighbor.bd600AddRecordSpace ( RecLen + POINTERSIZE_BD00,
                                           AuxRecIndex - StartIndex,
                                           pTargetRec,
                                           false );
        if (e_ok != m_TrError)
            return;

        SAPDB_RangeMove( __FILE__, 4,
            MAX_RECLEN_GG00, sizeof(tbd_node),
            pSourceRec,      POS_OFF_DIFF_BD00,
            pTargetRec,      POS_OFF_DIFF_BD00,
            RecLen,          m_TrError);
        if (e_ok != m_TrError)
            break;
        
        /* PTS 1104320 UH 27-10-1999 */
        m_Node.bd600LeafCount()       -= RecLeafCount;
        NextNeighbor.bd600LeafCount() += RecLeafCount;
    }
    
    /* delete spaces of moved records on pNode */
    m_Node.bd620_DeleteRecordsFromPage (RIGHT_DISTRIBUTION_BD520, RecIndex);
    if (e_ok != m_TrError) return; 

    if (e_ok != m_TrError) 
    {
        tgg00_BasisError AuxTrError = m_TrError;
        m_TrError = e_ok;
        NextNeighbor.bd600Free();
        if (e_ok == m_TrError) m_TrError = AuxTrError;
        return;
    }

    NextNeighbor.m_pNode->nd_right() = Neighbor.m_pNode->nd_right(); /* note: in case of temp files the left pointer has to be updated too */   
    Neighbor.m_pNode->nd_right()     = NextNeighbor.m_pNode->nd_id(); /* note: in case of temp files the left pointer has to be updated too */  

    /* Determine how many leaves are under the nodes now */

    Neighbor.m_pNode->ndLeafCount_bd00() += LeafCount;

    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        t01sname (bd_oflw, "Node        ");
        m_Node.bd600Print();
        t01sname (bd_oflw, "Neighbor    ");
        Neighbor.bd600Print();
        t01sname (bd_oflw, "NextNeighbor");
        NextNeighbor.bd600Print();
        t01int4  (bd_oflw, "New RecIndex", RecIndex);        
    }
    #endif

    /* build the new orders for adding separators for both new nodes */

    if ( bFirstRecUpdated )
        bd520_CreateChangeKeyOrder ( m_Node,
                                     Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                     rightNeighbor_ebd501,  // PTS 1112713 UH 2001-12-07
                                     NewOrders, KeyLen);
    else
    {
        if ( NodeLeafCount != m_Node.bd600LeafCount() ) /* PTS 1104320 UH 27-10-1999 */
            NewOrders.bd501CreateNew ( iokChange_ebd501,
                                       m_Node.m_pNode,
                                       Neighbor.bd600GetId(),  // PTS 1112713 UH 2001-12-07
                                       rightNeighbor_ebd501 ); // PTS 1112713 UH 2001-12-07
    }

    m_Node.bd600BuildSeparatorKey (IS_LEFT_NODE_GBD600, pKey, KeyLen, pSepKey, SepKeyLen);
    if (e_ok != m_TrError) return;
    NewOrders.bd501CreateNew ((bNewNodeInserted)?(iokAdd_ebd501):(iokChange_ebd501),
                              Neighbor.m_pNode,
							  m_Node.bd600GetId(), // PTS 1112713 UH 2001-12-07
                              leftNeighbor_ebd501, // PTS 1112713 UH 2001-12-07
							  ! SEPKEY_IN_NODE_BD520, pSepKey, SepKeyLen);

    NextNeighbor.bd600BuildSeparatorKey (IS_RIGHT_NODE_GBD600, pKey, KeyLen, pSepKey, SepKeyLen);
    if (e_ok != m_TrError) return;
    NewOrders.bd501CreateNew (iokAdd_ebd501, NextNeighbor.m_pNode,
		                      Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                              leftNeighbor_ebd501,   // PTS 1112713 UH 2001-12-07
                              SEPKEY_IN_NODE_BD520, pSepKey, SepKeyLen);

    // PTS 1112713 UH 2001-12-07 update info for succeeding orders
    m_PathInfo[m_CurrLevel].piLeftId_bd500 = Neighbor.bd600GetId();

    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
        bd520_PrintBalancing (btDistToNextRight_ebd501, m_Node, NextNeighbor);

    /* set index where the new record has to be inserted */
    RecIndex = FIRST_REC_INDEX_BD00;

    /* return only the node where the record is to include   */
    m_Node.bd600Exchange (Neighbor);
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_UnderFlow (cbd600_Node        &Neighbor,
                              cbd600_Node        &AuxNode, // PTS 1109619 UH 2001-03-13
                              bool                bFirstRecUpdated,
                              tsp00_Int4         &NumNodeDiff,
                              cbd501_IndexOrders &NewOrders)
{
    ROUTINE_DBG_MEO00 ("bd520_Underflow");

    if ( bd500GetTheCurrent().curr_action == m_create_index_parallel ) // PTS 1109967 UH 2001-04-09
        g01abort (csp3_bd_msg, csp3_n_btree,
                  "CreIdxPar: b520_Underflo",
                  bd500GetTheCurrent().curr_action);

    tsp00_Int4    Covering;
    tsp00_Int4    NeighborCovering  = 0;
    cbd600_Node   RightNeighbor (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
    cbd600_Node   LeftNeighbor  (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));

    /* PTS 1103723 UH 1999-09-06 */
    if ( m_Node.bd600IsRoot  () ||
         m_Node.bd600Covering() >= UNDER_FLOW_LIMIT_BD600 ) return;

    Covering = m_Node.bd600Covering();

    if ( 0 == m_Node.bd600NumRecords() )
    {
        /* delete separator to current page  */

        tsp00_PageNo LeftNeighbor = bd500LeftNeighbor();
        if ( e_ok != m_TrError ) return;

        if ( NIL_PAGE_NO_GG00 == LeftNeighbor )
        {
            NewOrders.bd501CreateNew ( iokDelete_ebd501, m_Node.m_pNode,
                                       m_Node.bd600RightNeighbor(),      // PTS 1112713 UH 2001-12-07
                                       rightNeighbor_ebd501 ); // PTS 1112713 UH 2001-12-07
            m_Node.bd600Free();
            if ( e_ok != m_TrError ) return;
            --NumNodeDiff;
        }
        else
        {
            /* PTS 1109619 UH 2001-03-13 begin */
            if ( Neighbor.bd600IsAccessable() ) // PTS 1130325 UH 2004-06-25
            {
                if ( LeftNeighbor != Neighbor.bd600GetId() )
                {
                    AuxNode.bd600Exchange(Neighbor);
                    Neighbor.bd600GetNode (LeftNeighbor);
                }
            }
            else
                Neighbor.bd600GetNode (LeftNeighbor);
            /* PTS 1109619 UH 2001-03-13 end */
            
            if ( e_ok != m_TrError ) return;

            if ( bd510_TryToRemoveLevel(Neighbor) )
            {
                /* PTS 1103562 UH 1999-08-08 */
                if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                    bd520_PrintBalancing (btRemoveLevel_ebd501, m_Node, m_Node);
                NumNodeDiff -= 2;
            }
            else
            {
                NewOrders.bd501CreateNew ( iokDelete_ebd501, m_Node.m_pNode,
                                           Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                           leftNeighbor_ebd501); // PTS 1112713 UH 2001-12-07
                Neighbor.m_pNode->nd_right() = m_Node.m_pNode->nd_right();
                if ( bd500IsClustered() )
                {
                    if ( m_Node.m_pNode->nd_right() != NIL_PAGE_NO_GG00 )
                    {
                        RightNeighbor.bd600GetNode(m_Node.m_pNode->nd_right());
                        if ( e_ok != m_TrError ) return;
                        RightNeighbor.m_pNode->nd_left() = Neighbor.m_pNode->nd_id();
                    }
                }

                if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                    bd520_PrintBalancing (btRemoveEmpty_ebd501, m_Node, m_Node);

                m_Node.bd600Free();
                --NumNodeDiff;
            }
        }
        return;
    }

    /* try to merge or balance with right neighbor node */
    if (NIL_PAGE_NO_GG00 == m_Node.m_pNode->nd_right())
        m_TrError = e_no_more_space;
    else
    {
        /* PTS 1109619 UH 2001-03-13 begin */
        if ( Neighbor.bd600IsAccessable() ) // PTS 1130325 UH 2004-06-25
        {
            if ( m_Node.m_pNode->nd_right() != Neighbor.bd600GetId() )
            {
                AuxNode.bd600Exchange(Neighbor);
                Neighbor.bd600GetNode (m_Node.m_pNode->nd_right());
            }
        }
        else
            Neighbor.bd600GetNode (m_Node.m_pNode->nd_right());
        /* PTS 1109619 UH 2001-03-13 end */
        
        if (e_ok == m_TrError)
        {
            if ( 0 == Neighbor.bd600NumRecords() )
            {
                m_Node.m_pNode->nd_right() = Neighbor.m_pNode->nd_right();

                if ( bd510_TryToRemoveLevel(Neighbor) )
                {
                    /* PTS 1103562 UH 1999-08-08 */
                    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                        bd520_PrintBalancing (btRemoveLevel_ebd501, m_Node, m_Node);
                    NumNodeDiff -= 2;
                }
                else
                {
                    if ( bFirstRecUpdated )
                        bd520_CreateChangeKeyOrder ( m_Node,
                                                     Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                                     rightNeighbor_ebd501,  // PTS 1112713 UH 2001-12-07
                                                     NewOrders );
                    NewOrders.bd501CreateNew ( iokDelete_ebd501, Neighbor.m_pNode,
                                               m_Node.bd600GetId(),  // PTS 1112713 UH 2001-12-07
                                               leftNeighbor_ebd501); // PTS 1112713 UH 2001-12-07

                    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                        bd520_PrintBalancing (btRemoveEmpty_ebd501, Neighbor, Neighbor);

                    Neighbor.bd600Free();
                    if (e_ok != m_TrError) return;
                    --NumNodeDiff;
                    if ( bd500IsClustered() )
                    {
                        if ( m_Node.m_pNode->nd_right() != NIL_PAGE_NO_GG00 )
                        {
                            RightNeighbor.bd600GetNode(m_Node.m_pNode->nd_right());
                            if ( e_ok != m_TrError ) return;
                            RightNeighbor.m_pNode->nd_left() = m_Node.m_pNode->nd_id();
                        }
                    }
                }
                return;
            }

            NeighborCovering = Neighbor.bd600Covering();

            /* try to merge the node with its right neighbor */
            if ( (Covering + NeighborCovering) <= FULLCOVERING_BD00 )
            {

                const tsp00_Int4 NodeLeafCount = m_Node.bd600LeafCount(); /* PTS 1104320 UH 27-10-1999 */

                Neighbor.bd620_MergeNodes (m_Node);

                if (e_ok != m_TrError)
                {
                    Neighbor.bd600Release (! IS_CHANGED_BD600);
                }
                else
                {
                    if ( bd510_TryToRemoveLevel(Neighbor) )
                    {
                        /* PTS 1103562 UH 1999-08-08 */
                        if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                            bd520_PrintBalancing (btRemoveLevel_ebd501, m_Node, m_Node);
                        NumNodeDiff -= 2;
                    }
                    else
                    {
                        if ( bFirstRecUpdated )
                            bd520_CreateChangeKeyOrder ( m_Node,
                                                         Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                                         rightNeighbor_ebd501,  // PTS 1112713 UH 2001-12-07
                                                         NewOrders );
                        else
                        {
                            if ( NodeLeafCount != m_Node.bd600LeafCount() ) /* PTS 1104320 UH 27-10-1999 */
                                NewOrders.bd501CreateNew ( iokChange_ebd501, m_Node.m_pNode,
                                                           Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                                           rightNeighbor_ebd501); // PTS 1112713 UH 2001-12-07
                        }

                        NewOrders.bd501CreateNew ( iokDelete_ebd501, Neighbor.m_pNode,
                                                   m_Node.bd600GetId(),  // PTS 1112713 UH 2001-12-07
                                                   leftNeighbor_ebd501); // PTS 1112713 UH 2001-12-07

                        if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                            bd520_PrintBalancing (btMergeFromRight_ebd501, m_Node, Neighbor);

                        /* update the right pointer */
                        m_Node.m_pNode->nd_right() = Neighbor.m_pNode->nd_right() ;

                        if ( bd500IsClustered() )
                        {
                            if ( m_Node.m_pNode->nd_right() != NIL_PAGE_NO_GG00 )
                            {
                                RightNeighbor.bd600GetNode(m_Node.m_pNode->nd_right());
                                if ( e_ok != m_TrError ) return;
                                RightNeighbor.m_pNode->nd_left() = m_Node.m_pNode->nd_id();
                            }
                        }

                        Neighbor.bd600Free();
                        --NumNodeDiff;
                    }
                }
                return;
            }
            else
            {
                /* balance the node with its right neighbor */
                bd520_DistributeFromRight (Neighbor, Covering, NeighborCovering, bFirstRecUpdated, NewOrders);

                if ( e_ok != m_TrError )
                {
                    Neighbor.bd600Release (! IS_CHANGED_BD600);
                    if ( e_no_more_space != m_TrError ) return;
                }
            }
        }
    }
    
    /* try to merge or balance with left neighbor node */
    if ( e_no_more_space == m_TrError )
    {
        m_TrError = e_ok;

        tsp00_PageNo LeftNeighbor = bd500LeftNeighbor();
        if ( e_ok != m_TrError ) return;

        if ( NIL_PAGE_NO_GG00 == LeftNeighbor )
            m_TrError = e_no_more_space;
        else
        {
            /* PTS 1109619 UH 2001-03-13 begin */
            if ( Neighbor.bd600IsAccessable() ) // PTS 1130325 UH 2004-06-25
            {
                if ( LeftNeighbor != Neighbor.bd600GetId() )
                {
                    AuxNode.bd600Exchange(Neighbor);
                    Neighbor.bd600GetNode (LeftNeighbor);
                }
            }
            else
                Neighbor.bd600GetNode (LeftNeighbor);
            /* PTS 1109619 UH 2001-03-13 end */

            if ( e_ok == m_TrError )
            {
                if ( 0 == Neighbor.bd600NumRecords () )
                {
                    /* It is too expensive to look for the previous node  */
                    /* of the left neighbor. That is why the distribution */
                    /* is aborted here.                                   */
                    Neighbor.bd600Release (! IS_CHANGED_BD600);
                    return;
                }

                NeighborCovering = Neighbor.bd600Covering();

                if ( (Covering + NeighborCovering) <= FULLCOVERING_BD00)
                {
                    /* PTS 1103562 UH 1999-08-08 */
                    /* update the SubNodeRecIndex in the PathInfo so the following orders can be executed */
                    m_PathInfo [m_CurrLevel].piSubNodeRecIndex_bd500 += (1 + Neighbor.bd600MaxRecIndex());

                    const tsp00_Int4 NeighborLeafCount = Neighbor.bd600LeafCount(); /* PTS 1104320 UH 27-10-1999 */

                    m_Node.bd620_MergeNodes (Neighbor); // Move from Node to (Left)Neighbor

                    if (e_ok == m_TrError)
                    {
                        if ( bd510_TryToRemoveLevel(Neighbor) )
                        {
                            /* PTS 1103562 UH 1999-08-08 */
                            NumNodeDiff -= 2;
                            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                                bd520_PrintBalancing (btRemoveLevel_ebd501, m_Node, m_Node);
                        }
                        else
                        {
                            // NewOrders.iosOrderCount_bd501 = 0; PTS 1131880 UH 2004-10-13
                            if ( NeighborLeafCount != Neighbor.bd600LeafCount() ) /* PTS 1104320 UH 27-10-1999 */
                            {
                                NewOrders.bd501CreateNew ( iokChange_ebd501,
                                                           Neighbor.m_pNode,
                                                           m_Node.bd600GetId(),   // PTS 1112713 UH 2001-12-07
                                                           rightNeighbor_ebd501); // PTS 1112713 UH 2001-12-07
                            }
                            NewOrders.bd501CreateNew ( iokDelete_ebd501,
                                                       m_Node.m_pNode,
                                                       Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                                       leftNeighbor_ebd501);  // PTS 1112713 UH 2001-12-07

                            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                                bd520_PrintBalancing (btMergeFromLeft_ebd501, m_Node, Neighbor);

                            /* update the right pointer */
                            Neighbor.m_pNode->nd_right() = m_Node.m_pNode->nd_right();
                            if ( bd500IsClustered() )
                            {
                                if ( Neighbor.m_pNode->nd_right() != NIL_PAGE_NO_GG00 )
                                {
                                    RightNeighbor.bd600GetNode(Neighbor.m_pNode->nd_right());
                                    if ( e_ok != m_TrError ) return;
                                    RightNeighbor.m_pNode->nd_left() = Neighbor.m_pNode->nd_id();
                                }
                            }
                            m_Node.bd600Free();
                            --NumNodeDiff;
                            m_Node.bd600Exchange (Neighbor);
                        }
                    }
                    else
                    {
                        Neighbor.bd600Release (! IS_CHANGED_BD600);
                        return;
                    }
                }
                else
                {
                    bd520_DistributeFromLeft (Neighbor, Covering, NeighborCovering, NewOrders);

                    if ( (e_ok != m_TrError) && (e_no_more_space != m_TrError) ) return;
                }
            }
        }
    }

    if (e_no_more_space == m_TrError)
    {
        /* nothing was necessary to do */
        m_TrError = e_ok;
        if ( m_Node.m_pNode->nd_level() > LEAF_LEVEL_BD00 )
        {
            if ( bFirstRecUpdated )
                bd520_CreateChangeKeyOrder ( m_Node,
                                             m_Node.bd600RightNeighbor(),     // PTS 1112713 UH 2001-12-07
                                             rightNeighbor_ebd501, // PTS 1112713 UH 2001-12-07
                                             NewOrders);
            else
                /* after deleting a separator, the LeafCount must be changed */
                NewOrders.bd501CreateNew ( iokChange_ebd501, m_Node.m_pNode,
                                           m_Node.bd600RightNeighbor(),      // PTS 1112713 UH 2001-12-07
                                           rightNeighbor_ebd501); // PTS 1112713 UH 2001-12-07
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if ( e_ok != m_TrError )
        t01basis_error (bd_index, "m_TrError   ", m_TrError);
#   endif
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_DistributeFromRight (cbd600_Node         &Neighbor,
                                        tsp00_Int4           Covering,
                                        tsp00_Int4           NeighborCovering,
                                        bool                 bFirstRecUpdated,
                                        cbd501_IndexOrders  &NewOrders)
{
    /* Check and Do Distribution from right node to (current) node */

    ROUTINE_DBG_MEO00 ("bd520_DistributeFromRight");

    const tsp00_Int4 NodeLeafCount      = m_Node.bd600LeafCount(); /* PTS 1104320 UH 27-10-1999 */
    tsp00_Int4       OptimRecIndex      = 0;              
    tsp00_Int4       AuxRecIndex;   
    bool             bDistributionFound = false;
    tsp00_Int4       RecLen;
    bool             withConsistentReadInfo;
    tsp00_BytePtr    pSourceRec;
    tsp00_BytePtr    pTargetRec;
    tsp00_KeyPtr     pSepKey;
    tsp00_Int2       SepKeyLen;
    tsp00_Int4       RecLeafCount; /* PTS 1104320 UH 27-10-1999 */

    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        t01sname (bd_oflw, "before distr");
        m_Node.bd600Print();
        Neighbor.bd600Print();
    }
    #endif
    
    /* find the optimum distribution */
    m_Node.bd620_FindOptimDistribForDelete (Neighbor, RIGHT_DISTRIBUTION_BD520,
        Covering, NeighborCovering, bDistributionFound, OptimRecIndex);
    if (e_ok != m_TrError)
        return;
    
    if (!bDistributionFound)
    {
        m_TrError = e_no_more_space;
        return;
    }

    /* move all records with Indexes >= OptimRecIndex from pNeighbor to pNode */
    for (AuxRecIndex = FIRST_REC_INDEX_BD00; AuxRecIndex <= OptimRecIndex; ++AuxRecIndex)
    {
        RecLeafCount = Neighbor.bd600LeafCount (AuxRecIndex, AuxRecIndex); /* PTS 1104320 UH 27-10-1999 */
        
        Neighbor.m_RecordList.GetRecord (AuxRecIndex, withConsistentReadInfo, RecLen, pSourceRec);
        
        m_Node.bd600AddRecordSpace ( RecLen + POINTERSIZE_BD00,
                                     m_Node.bd600MaxRecIndex() + 1,
                                     pTargetRec,
                                     false );
        if (e_ok != m_TrError)
            return;
        
        SAPDB_RangeMove( __FILE__, 5,
            MAX_RECLEN_GG00, sizeof(tbd_node), 
            pSourceRec,      POS_OFF_DIFF_BD00,
            pTargetRec,      POS_OFF_DIFF_BD00,
            RecLen,          m_TrError);
            
        if (e_ok != m_TrError)
            return;

        /* PTS 1104320 UH 27-10-1999 */
        Neighbor.bd600LeafCount() -= RecLeafCount;
        m_Node.bd600LeafCount()   += RecLeafCount;
    }
    
    /* delete spaces of moved records on pNeighbor */
    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        Neighbor.m_RecordList.Print();
    }
    #endif

    Neighbor.bd620_DeleteRecordsFromPage (! RIGHT_DISTRIBUTION_BD520, OptimRecIndex);

    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        Neighbor.m_RecordList.Print();
        t01sname (bd_oflw, "after distr ");
        m_Node.bd600Print();
        Neighbor.bd600Print();
    }
    #endif

    if (g01glob.treecheck)
    {
        m_Node.bd600CheckWithErrorHandling();
        Neighbor.bd600CheckWithErrorHandling();
    }

    if (e_ok != m_TrError) return;

    /* update the index nodes above */

    if ( bFirstRecUpdated )
        bd520_CreateChangeKeyOrder ( m_Node,
                                     Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                     rightNeighbor_ebd501,  // PTS 1112713 UH 2001-12-07
                                     NewOrders);
    else
    {
        if ( NodeLeafCount != m_Node.bd600LeafCount() ) /* PTS 1104320 UH 27-10-1999 */
            NewOrders.bd501CreateNew ( iokChange_ebd501,
                                       m_Node.m_pNode,
                                       Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                                       rightNeighbor_ebd501); // PTS 1112713 UH 2001-12-07
    }

    m_Node.bd600BuildSeparatorKey (Neighbor, pSepKey, SepKeyLen);
    if (e_ok != m_TrError) return; 
    NewOrders.bd501CreateNew ( iokChange_ebd501, Neighbor.m_pNode,
                               m_Node.bd600GetId(), // PTS 1112713 UH 2001-12-07
                               leftNeighbor_ebd501, // PTS 1112713 UH 2001-12-07
                               SEPKEY_IN_NODE_BD520, pSepKey, SepKeyLen);

    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
        bd520_PrintBalancing (btDistFromRight_ebd501, m_Node, Neighbor);
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_DistributeFromLeft (cbd600_Node         &Neighbor,
                                       tsp00_Int4           Covering,
                                       tsp00_Int4           NeighborCovering,
                                       cbd501_IndexOrders  &NewOrders)
{
    /* Check and Do Distribution from left node to (current) node */

    ROUTINE_DBG_MEO00 ("bd520_DistributeFromLeft");

    const tsp00_Int4 NeighborLeafCount  = Neighbor.bd600LeafCount(); /* PTS 1104320 UH 27-10-1999 */
    tsp00_Int4       OptimRecIndex      = 0;              
    tsp00_Int4       AuxRecIndex;   
    bool             bDistributionFound = false;
    tsp00_Int4       RecLen;
    bool             withConsistentReadInfo;
    tsp00_BytePtr    pSourceRec;
    tsp00_BytePtr    pTargetRec;
    tsp00_KeyPtr     pSepKey;
    tsp00_Int2       SepKeyLen;
    tsp00_Int4       RecLeafCount; /* PTS 1104320 UH 27-10-1999 */
    
    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        t01sname (bd_oflw, "before distr");
        m_Node.bd600Print();
        Neighbor.bd600Print();
    }
    #endif
    
    /* find the optimum distribution */
    m_Node.bd620_FindOptimDistribForDelete (Neighbor, ! RIGHT_DISTRIBUTION_BD520,
        Covering, NeighborCovering, bDistributionFound, OptimRecIndex);
    if (e_ok != m_TrError)
        return;
    
    if (!bDistributionFound)
    {
        m_TrError = e_no_more_space;
        return;
    }

    /* PTS 1103562 UH 1999-08-08 */
    /* update the SubNodeRecIndex in the PathInfo so the following orders can be executed */
    m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 += (1 + Neighbor.bd600MaxRecIndex());

    /* move all records with Indexes <= OptimRecIndex from pNeighbor to pNode */
    for (AuxRecIndex = OptimRecIndex; AuxRecIndex <= Neighbor.bd600MaxRecIndex(); ++AuxRecIndex)
    {
        RecLeafCount = Neighbor.bd600LeafCount (AuxRecIndex, AuxRecIndex); /* PTS 1104320 UH 27-10-1999 */

        Neighbor.m_RecordList.GetRecord (AuxRecIndex, withConsistentReadInfo, RecLen, pSourceRec);

        m_Node.bd600AddRecordSpace ( RecLen + POINTERSIZE_BD00,
                                     AuxRecIndex - OptimRecIndex,
                                     pTargetRec,
                                     false );
        if (e_ok != m_TrError)
            return;
        
        SAPDB_RangeMove( __FILE__, 6,
            MAX_RECLEN_GG00, sizeof(tbd_node), 
            pSourceRec,      POS_OFF_DIFF_BD00, 
            pTargetRec,      POS_OFF_DIFF_BD00,
            RecLen,          m_TrError);
        if (e_ok != m_TrError)
            return;

        /* PTS 1104320 UH 27-10-1999 */
        Neighbor.bd600LeafCount() -= RecLeafCount;
        m_Node.bd600LeafCount()   += RecLeafCount;
    }
    
    /* delete spaces of moved records on pNode */
    Neighbor.bd620_DeleteRecordsFromPage (RIGHT_DISTRIBUTION_BD520, OptimRecIndex);

    #ifdef SAPDB_SLOW 
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        t01sname (bd_oflw, "after distr ");
        m_Node.bd600Print();
        Neighbor.bd600Print();
    }
    #endif

    if (g01glob.treecheck)
    {
        m_Node.bd600CheckWithErrorHandling();
        Neighbor.bd600CheckWithErrorHandling();
    }

    if (e_ok != m_TrError) return; 

    /* update the index nodes above */

    // NewOrders.iosOrderCount_bd501 = 0;  PTS 1131880 UH 2004-10-13
    if ( NeighborLeafCount != Neighbor.bd600LeafCount() ) /* PTS 1104320 UH 27-10-1999 */
        NewOrders.bd501CreateNew ( iokChange_ebd501, Neighbor.m_pNode,
                                   m_Node.bd600GetId(),   // PTS 1112713 UH 2001-12-07
                                   rightNeighbor_ebd501); // PTS 1112713 UH 2001-12-07

    Neighbor.bd600BuildSeparatorKey (m_Node, pSepKey, SepKeyLen);
    if (e_ok != m_TrError) return; 
    NewOrders.bd501CreateNew ( iokChange_ebd501, m_Node.m_pNode,
                               Neighbor.bd600GetId(), // PTS 1112713 UH 2001-12-07
                               leftNeighbor_ebd501,   // PTS 1112713 UH 2001-12-07
                               SEPKEY_IN_NODE_BD520, pSepKey, SepKeyLen);

    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
        bd520_PrintBalancing (btDistFromLeft_ebd501, m_Node, Neighbor);
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd520_Propagate (bool                bWithKey,
                              cbd501_IndexOrders &NewOrders)
{
    /* PRE: ndLeafCount_bd00 must be updated correctly */
    /* PRE: if bWithKey is true then RecKeyLen is set correct */

    ROUTINE_DBG_MEO00 ("bd520_Propagate");

    if ( ! m_Node.bd600IsRoot() )
    {
        if ( bWithKey && (m_Node.m_pNode->nd_level() > LEAF_LEVEL_BD00) )
        {
            bd520_CreateChangeKeyOrder ( m_Node,
                                         m_Node.bd600RightNeighbor(), // PTS 1112713 UH 2001-12-07
                                         rightNeighbor_ebd501, // PTS 1112713 UH 2001-12-07
                                         NewOrders);
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd520_PrintBalancing (btPropagateWithKey_ebd501, m_Node, m_Node);
        }
        else
        {
            NewOrders.bd501CreateNew ( iokChange_ebd501,
                                       m_Node.m_pNode,
                                       m_Node.bd600RightNeighbor(), // PTS 1112713 UH 2001-12-07
                                       rightNeighbor_ebd501); // PTS 1112713 UH 2001-12-07
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd520_PrintBalancing (btPropagate_ebd501, m_Node, m_Node);
        }
    }
}
