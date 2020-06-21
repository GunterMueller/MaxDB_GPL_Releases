/*!**************************************************************************

  module      : vbd510.cpp
  special area: cbd500_Tree
  responsible : UweH
  last changed: 1999-09-02  17:50
  see also    : example.html ...
  copyright:    Copyright (c) 1998-2005 SAP AG
  description : Restructure TreeIndex



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


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd500.h"
#include "gbd501.h"
#include "gbd520.h"
#include "gbd600.h"
#include "hbd102.h"

#include "hgg01_3.h" // g01vtrace, g01glob
#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define UPDATE_KEY true

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

inline void
bd510_ChangeLeafCountInNode (cbd600_Node       &Node,
                             cbd520_Separator  &Separator,
                             tsp00_Int4         NewLeafCount)
{
    Node.bd600LeafCount() -= Separator.bd520LeafCount ();
    Node.bd600LeafCount() += NewLeafCount;
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd510_ExcuteOrdersOnOneLevel(cbd501_IndexOrders &Orders,
                                          tsp00_Int4         &NumNodeDiff)
{
    ROUTINE_DBG_MEO00 ("bd510_ExcuteOrdersOnOneLevel");

    /* PRE:                                                                */
    /* at least one order must be given                                    */
    /* !!! the orders must be ordered: leftmost node comes first           */
    /* m_Node is recovered at the end of the method,                       */
    /* because in the case of add/change, m_Node holds the record          */
    /* POST:                                                               */
    /* The PathInfo is only changed, if it originaly does not point to the */
    /* leftmost node on the current TargetLevel                            */
    /* The m_PathInfo for lower levels is invalid !!                       */

    if ( e_ok != m_TrError ) return;

    cbd600_Node         AuxNode  (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
    tsp00_Int4          CurrOrderNo = 0;
    cbd501_IndexOrder*  CurrOrder   = Orders.bd501GetOrder (CurrOrderNo);
    tsp00_PageNo        RootId      = m_Node.m_Current.bd300GetRootId();
    cbd520_Separator    Separator;
    cbd600_Node         Neighbor     (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
    cbd600_Node         NextNeighbor (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));
    cbd501_IndexOrders  NewOrders;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01p2int4 (bd_index, "SourceNode  ", CurrOrder->ioSubNodeId_bd501(), "from Level  ", m_CurrLevel);
#   endif
    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
    {
        Orders.bd501Trace(*m_Node.m_Current.curr_trans);
        bd500Print ("bd510_ExcuteOrdersOnOneLevel");
    }

    /* prevent senseless function of bd500NextNode */
    m_LeftMostIdNextLowerLevel = NIL_PAGE_NO_GG00;
    
    /* update the PathInfo only for tracing !!! */
    if ( m_Node.bd600IsAccessable() )
    {
        m_PathInfo[m_CurrLevel].piId_bd500          = m_Node.bd600GetId();
        m_PathInfo[m_CurrLevel].piRightId_bd500     = m_Node.bd600RightNeighbor();
        m_PathInfo[m_CurrLevel].piLeftId_bd500      = NIL_PAGE_NO_GG00;
        m_PathInfo[m_CurrLevel].piMaxRecIndex_bd500 = m_Node.bd600MaxRecIndex();
        m_PathInfo[m_CurrLevel].piLeafCount_bd500   = m_Node.bd600LeafCount(m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500,
                                                                            m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500);

        AuxNode.bd600Exchange (m_Node); // Save current node
    }
    else
    {
        m_PathInfo[m_CurrLevel].piId_bd500          = NIL_PAGE_NO_GG00;
        m_PathInfo[m_CurrLevel].piRightId_bd500     = NIL_PAGE_NO_GG00;
        m_PathInfo[m_CurrLevel].piLeftId_bd500      = NIL_PAGE_NO_GG00;
        m_PathInfo[m_CurrLevel].piMaxRecIndex_bd500 = NIL_RECINDEX_BD00;
        m_PathInfo[m_CurrLevel].piLeafCount_bd500   = -1;
    }

    /* PTS 1103624 AK 1999-08-13, order already */
    /* executed implicetly by a previous order  */
    if ( (CurrOrder->ioKeyIsInvalid() /* PTS 1108839 UH 2000-12-27 */ )
         &&
         (iokChange_ebd501 == CurrOrder->ioKind_bd501)
         && 
         (m_CurrLevel == m_RootLevel) )
        return;

    bd510_PrepareOrderExecutionOnNextHigherLevel(Orders); // PTS 1112713 UH 2001-12-07
    if ( e_ok != m_TrError ) return;                      // PTS 1112713 UH 2001-12-07
    
    /* Now m_Node is set to the left most index node */
    /* of all orders to execute on this level.       */
    /* SepRecIndex points to the correct separator.  */

    tsp00_Int4 &SepRecIndex              = m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500;
    bool        bLastOrderForCurrentNode = true;
    bool        bFirstKeyIsChanged;

    do
    {
        /* determine the current Separator to work on */

        Separator.Assign( m_Node.bd600SepPtr(SepRecIndex) );

#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        t01p2int4 (bd_index, "IndexNode   ", m_Node.bd600GetId(),
                             "RecIndex    ", SepRecIndex);
        CurrOrder->bd501Print();
#       endif

        if ( (iokAdd_ebd501                  != CurrOrder->ioKind_bd501)   &&
             (CurrOrder->ioSubNodeId_bd501() != Separator.bd520SubNodeId()) )
        {
            Orders.bd501Trace(*m_Node.m_Current.curr_trans);
            bd500_InvalidIndexStructure (csp3_bd_msg, "IdxOrder.SubNode invalid",
                                         CurrOrder->ioSubNodeId_bd501());
            return;
        }

        /* PTS 1103693 AK 19-08-1999 */
        /* check if the first key of the current node has changed */ 
        // if (bLastOrderForCurrentNode) // PTS 1109426 UH 2000-02-19
        bFirstKeyIsChanged = (FIRST_REC_INDEX_BD00 == SepRecIndex)
                             &&
                             (iokAdd_ebd501 != CurrOrder->ioKind_bd501) /* PTS 1104601 UH 11-11-1999 */
                             &&
                             ( ! CurrOrder->ioKeyIsInvalid() /* PTS 1108839 UH 2000-12-27 */
                               ||
                               /* PTS 1103925 UH 1999-09-09 */
                               (iokDelete_ebd501 == CurrOrder->ioKind_bd501) );

        bLastOrderForCurrentNode = SepRecIndex == m_Node.bd600MaxRecIndex();
        
        switch ( CurrOrder->ioKind_bd501 )
        {
        case iokChange_ebd501:
            bd510_ExecuteChangeOrder (*CurrOrder,
                bLastOrderForCurrentNode, bFirstKeyIsChanged,  // PTS 1103693 AK 19-08-1999 
                CurrOrderNo == Orders.iosOrderCount_bd501-1,   // +++ PTS 1109619 UH 2001-03-13
                Separator, SepRecIndex,
                Neighbor, NextNeighbor, NewOrders, NumNodeDiff);
            break;
        case iokAdd_ebd501:
            bd510_ExecuteAddOrder    (*CurrOrder,
                bLastOrderForCurrentNode, bFirstKeyIsChanged,  // PTS 1103693 AK 19-08-1999 
                Separator, SepRecIndex,
                Neighbor, NextNeighbor, NewOrders, NumNodeDiff);
            break;
        case iokDelete_ebd501:
            bd510_ExecuteDeleteOrder (*CurrOrder,
                bLastOrderForCurrentNode, bFirstKeyIsChanged,  // PTS 1103693 AK 19-08-1999 
                CurrOrderNo == Orders.iosOrderCount_bd501-1,   // +++ PTS 1109619 UH 2001-03-13
                Separator, SepRecIndex,
                Neighbor, NextNeighbor, NewOrders, NumNodeDiff);
            break;
        default:
            m_TrError = e_invalid_indexorder;
            return;
        }

#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        if ( e_ok != m_TrError ) t01basis_error (bd_index, "m_TrError   ", m_TrError);
#       endif

        if ( e_ok != m_TrError ) return;

        /* get the next order from the order list */
        ++CurrOrderNo;
        if ( CurrOrderNo >= Orders.iosOrderCount_bd501 ) break;

        CurrOrder = Orders.bd501GetOrder (CurrOrderNo);

        /* Set PathInfo to the next Separator */
        if ( iokAdd_ebd501 != CurrOrder->ioKind_bd501 )
        {
            /* add must increment the piSubNodeRecIndex_bd500 by itself (for an append) */
            if ( Separator.bd520SubNodeId() != CurrOrder->ioSubNodeId_bd501() )
            {
                /* the current order needs the next separator */
                if ( SepRecIndex < m_Node.bd600MaxRecIndex() )
                    ++SepRecIndex;
                else
                {
                    if ( g01glob.treecheck && m_Node.bd600IsAccessable() )
                        m_Node.bd600CheckIndex();
                    
                    // PTS 1109429 UH 2000-02-19 begin
                    if ( Neighbor.bd600IsAccessable()
                         &&
                         m_Node.bd600RightNeighbor() == Neighbor.bd600GetId() )
                    {
                        m_PathInfo[m_CurrLevel].piLeftId_bd500 = m_Node.m_pNode->nd_id();
                        m_Node.bd600Exchange(Neighbor);
                        m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 = FIRST_REC_INDEX_BD00;
                        m_PathInfo[m_CurrLevel].piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
                        m_PathInfo[m_CurrLevel].piLeafCount_bd500       = m_Node.bd600LeafCount(FIRST_REC_INDEX_BD00,FIRST_REC_INDEX_BD00);
                    }
                    else
                        bd500NextNode();
                    // PTS 1109429 UH 2000-02-19 end
                    if ( e_ok != m_TrError ) return;
               }
            }
        }
    }
    while ( CurrOrderNo < Orders.iosOrderCount_bd501 );
    
    if ( g01glob.treecheck && m_Node.bd600IsAccessable() )
        m_Node.bd600CheckIndex();
    
    Orders.iosOrderCount_bd501 = 0;

    /* go to the next level and reorganize it                  */
    /* this is recursive, but there is a maximum Level about 5 */
    if ( (NewOrders.iosOrderCount_bd501 > 0)
         &&
         (m_CurrLevel < m_RootLevel) ) // PTS 1109276 UH 2001-02-05
        bd510_ExcuteOrdersOnOneLevel (NewOrders, NumNodeDiff);

    if (AuxNode.bd600IsAccessable() )
        m_Node.bd600Exchange (AuxNode); // restore the saved node
}


/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd510_PrepareOrderExecutionOnNextHigherLevel(cbd501_IndexOrders &Orders)
{
    // PTS 1112713 UH 2001-12-07 added this methode
    ROUTINE_DBG_MEO00 ("bd510_PrepareOrderExecutionOnNextHigherLevel");

    /*
    The PathInfo has to be updated, so the info for the above index level points to the separator
    which points to the subnode belonging to the first order of the order list.
    */
    
    const tsp00_Int4   indexLevel      = m_CurrLevel+1;
    cbd501_IndexOrder &FirstOrder      = *(Orders.bd501GetOrder (0));
    tsp00_Int4         currSepRecIndex = m_PathInfo[indexLevel].piSubNodeRecIndex_bd500;
    cbd520_Separator   separator;
    
    if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
        bd500Print ("bd510_Prepare:1");

    // Set the internal tree node to the index node above. ---------------
    
    bd500_PrevIndexNode();
    
    // Check the consistency ---------------
    
    if ( ! m_Node.bd600IsAccessable() )
    {
        Orders.bd501Trace(*m_Node.m_Current.curr_trans);
        bd500_InvalidIndexStructure (csp3_bd_msg,"IndexNode not found  SN:",FirstOrder.ioSubNodeId_bd501());
        return;
    }

    if ( e_ok != m_TrError ) return;

    separator.Assign(m_Node.bd600SepPtr(currSepRecIndex));

    if ( NULL == separator.bd520GetRecPtr() )
    {
        Orders.bd501Trace(*m_Node.m_Current.curr_trans);
        bd500_InvalidIndexStructure (csp3_bd_msg, "Separator not found #   ", currSepRecIndex);
        return;
    }
    else if ( m_PathInfo[indexLevel].piId_bd500 != m_Node.bd600GetId() )
    {
        Orders.bd501Trace(*m_Node.m_Current.curr_trans);
        bd500_InvalidIndexStructure (csp3_bd_msg, "PathInfo invalid    #   ", currSepRecIndex);
        return;
    }

    // Search for Separator matching the FirstOrder -------------
    
    // Check current separator

    if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() 
         ||
         iokAdd_ebd501 == FirstOrder.ioKind_bd501 )
        return; // the pathInfo doesn't need to be updated

    // Check first right separator in this indexnode

    if ( currSepRecIndex+1 <= m_Node.bd600MaxRecIndex() )
    {
        separator.Assign(m_Node.bd600SepPtr(currSepRecIndex+1));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = currSepRecIndex+1;
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:2");
            return;
        }
    }
    
    // Check first left separator in this indexnode

    if ( currSepRecIndex-1 >= FIRST_REC_INDEX_BD00 )
    {
        separator.Assign(m_Node.bd600SepPtr(currSepRecIndex-1));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = currSepRecIndex-1;
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:3");
            return;
        }
    }

    // Check second right separator in this indexnode

    if ( currSepRecIndex+2 <= m_Node.bd600MaxRecIndex() )
    {
        separator.Assign(m_Node.bd600SepPtr(currSepRecIndex+2));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = currSepRecIndex+2;
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:4");
            return;
        }
    }

    // Check second left separator in this indexnode

    if ( currSepRecIndex-2 >= FIRST_REC_INDEX_BD00 )
    {
        separator.Assign(m_Node.bd600SepPtr(currSepRecIndex-2));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = currSepRecIndex-2;
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:5");
            return;
        }
    }
    
    // The orders are not beginning in the current indexnode.
    // First look into the right neighbor node and then look into the left neighbor node.
    
    // Check first and second right separator in next node

    if ( currSepRecIndex+2 > m_Node.bd600MaxRecIndex()
         &&
         m_Node.bd600RightNeighbor() != NIL_PAGE_NO_GG00 )
    {
        m_Node.bd600Move ( m_Node.bd600RightNeighbor() );
        if ( ! m_Node.bd600IsAccessable() )
        {
            Orders.bd501Trace(*m_Node.m_Current.curr_trans);
            bd500_InvalidIndexStructure (csp3_bd_msg, "right IndexNode not fnd:", m_PathInfo[indexLevel].piId_bd500);
            return;
        }
        
        separator.Assign(m_Node.bd600SepPtr(FIRST_REC_INDEX_BD00));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = FIRST_REC_INDEX_BD00;
            m_PathInfo[indexLevel].piLeftId_bd500          = m_PathInfo[indexLevel].piId_bd500;
            m_PathInfo[indexLevel].piRightId_bd500         = m_Node.bd600RightNeighbor();
            m_PathInfo[indexLevel].piId_bd500              = m_Node.bd600GetId();
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:6");
            return;
        }

        separator.Assign(m_Node.bd600SepPtr(FIRST_REC_INDEX_BD00+1));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = FIRST_REC_INDEX_BD00+1;
            m_PathInfo[indexLevel].piLeftId_bd500          = m_PathInfo[indexLevel].piId_bd500;
            m_PathInfo[indexLevel].piRightId_bd500         = m_Node.bd600RightNeighbor();
            m_PathInfo[indexLevel].piId_bd500              = m_Node.bd600GetId();
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:7");
            return;
        }
    }
    
    // Check first and second left separator in prev node

    if ( currSepRecIndex-2 < FIRST_REC_INDEX_BD00
         &&
         bd500LeftNeighbor() != NIL_PAGE_NO_GG00 )
    {
        m_Node.bd600Move ( bd500LeftNeighbor() );
        
        if ( ! m_Node.bd600IsAccessable() )
        {
            Orders.bd501Trace(*m_Node.m_Current.curr_trans);
            bd500_InvalidIndexStructure (csp3_bd_msg, "left IndexNode not found", m_PathInfo[indexLevel].piId_bd500);
            return;
        }
        
        separator.Assign(m_Node.bd600SepPtr(m_Node.bd600MaxRecIndex()));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = m_Node.bd600MaxRecIndex();
            m_PathInfo[indexLevel].piLeftId_bd500          = NIL_PAGE_NO_GG00;
            m_PathInfo[indexLevel].piId_bd500              = m_Node.bd600GetId();
            m_PathInfo[indexLevel].piRightId_bd500         = m_Node.bd600RightNeighbor();
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:8");
            return;
        }

        separator.Assign(m_Node.bd600SepPtr(m_Node.bd600MaxRecIndex()-1));
        if ( FirstOrder.ioSubNodeId_bd501() == separator.bd520SubNodeId() )
        {
            m_PathInfo[indexLevel].piSubNodeRecIndex_bd500 = m_Node.bd600MaxRecIndex()-1;
            m_PathInfo[indexLevel].piLeftId_bd500          = NIL_PAGE_NO_GG00;
            m_PathInfo[indexLevel].piId_bd500              = m_Node.bd600GetId();
            m_PathInfo[indexLevel].piRightId_bd500         = m_Node.bd600RightNeighbor();
            if ( g01vtrace.vtrAll_gg00 && g01glob.treecheck )
                bd500Print ("bd510_Prepare:9");
            return;
        }
        
        m_Node.bd600Move (m_PathInfo[indexLevel].piId_bd500);
        
        if ( ! m_Node.bd600IsAccessable() )
        {
            Orders.bd501Trace(*m_Node.m_Current.curr_trans);
            bd500_InvalidIndexStructure (csp3_bd_msg, "orig IndexNode not found", m_PathInfo[indexLevel].piId_bd500);
            return;
        }
    }

    Orders.bd501Trace(*m_Node.m_Current.curr_trans);
    bd500_InvalidIndexStructure (csp3_bd_msg, "No separator found #    ", currSepRecIndex);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd510_ExecuteDeleteOrder (cbd501_IndexOrder  &Order,
                                       bool                bLastOrderForCurrentNode,
                                       bool                bFirstKeyIsChanged,// PTS 1103693 AK 19-08-1999 
                                       bool                bUnderflowAllowed, // PTS 1109619 UH 2001-03-13
                                       cbd520_Separator   &Separator,
                                       tsp00_Int4         &SepRecIndex,
                                       cbd600_Node        &Neighbor,
                                       cbd600_Node        &AuxNode,           // PTS 1109619 UH 2001-03-13
                                       cbd501_IndexOrders &NewOrders,
                                       tsp00_Int4         &NumNodeDiff)
{
    ROUTINE_DBG_MEO00 ("bd510_ExecuteDeleteOrder");

    /* PURPOSE:                                   */
    /* delete Separator of deleted subnode        */
    /* PRE:                                       */
    /* m_PathInfo points to the correct Separator */
    /* and m_Node is the IndexNode to change now  */

    /* reduce the LeafCount for the current Node */
    m_Node.m_pNode->ndLeafCount_bd00() -= Separator.bd520LeafCount();

    if ( 0 == Separator.bd520KeyLen() )
    {
        /* the next separator which becomes the first one must change its key to 0 */
        Separator.Assign(m_Node.bd600SepPtr(SepRecIndex+1));
        tsp00_Int4 SepLenDelta; /* PTS 1104481 UH 04-11-1999 SepLenDelta built in */
        Separator.bd520ConvertToFirstSepOnLevel(SepLenDelta);
        m_Node.bd600UpdRecordSpace (SepLenDelta, SepRecIndex+1, Separator.bd520GetRecPtr());
        if ( e_ok != m_TrError ) return;
    }

    /* remove the separator from the node */
    m_Node.bd600DelRecordSpace (SepRecIndex);
    if ( e_ok != m_TrError ) return;

    tsp00_Int4 NumOrders = NewOrders.iosOrderCount_bd501;

    if ( bUnderflowAllowed ) // PTS 1109619 UH 2001-03-13 /* PTS 1103564 UH 1999-08-08 */
        bd520_UnderFlow (Neighbor, AuxNode, // PTS 1109619 UH 2001-03-13
                         FIRST_REC_INDEX_BD00 == SepRecIndex,
                         NumNodeDiff, NewOrders);

    /* if no Orders are necessary propagate the LeafCount */
    if ( NumOrders == NewOrders.iosOrderCount_bd501 )
        bd520_Propagate (bFirstKeyIsChanged, NewOrders);  // PTS 1103693 AK 19-08-1999 
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd510_ExecuteAddOrder (cbd501_IndexOrder  &Order,
                                    bool                bLastOrderForCurrentNode,
                                    bool                bFirstKeyIsChanged,          // PTS 1103693 AK 19-08-1999 
                                    cbd520_Separator   &Separator,
                                    tsp00_Int4         &SepRecIndex,
                                    cbd600_Node        &Neighbor,
                                    cbd600_Node        &NextNeighbor,
                                    cbd501_IndexOrders &NewOrders,
                                    tsp00_Int4         &NumNodeDiff)
{
    ROUTINE_DBG_MEO00 ("bd510_ExecuteAddOrder");

    /* PURPOSE:                                   */
    /* add Separator                              */
    /* PRE:                                       */
    /* m_PathInfo points to the correct Separator */
    /* and m_Node is the IndexNode to change now  */

    tsp00_Int4    NewSepLen = Separator.bd520RequiredLen(Order.ioSepKeyLen_bd501)+POINTERSIZE_BD00;
    tsp00_BytePtr pSepSpace;
    /* add new separator key */

    ++SepRecIndex;

    if ( ! m_Node.bd600SpaceIsAvailable (NewSepLen) )
    {
        bd520_Overflow (Neighbor, NextNeighbor,
            Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501, Order.ioLeafCount_bd501(),
            FOR_INSERT_GBD500, bFirstKeyIsChanged,                                    // PTS 1103693 AK 19-08-1999 
            NewSepLen, SepRecIndex, NumNodeDiff, NewOrders);
        if ( e_ok != m_TrError ) return;
        
        m_Node.bd600AddRecordSpace (NewSepLen, SepRecIndex, pSepSpace, false);
        if ( e_ok != m_TrError ) return;
        
        Separator.Assign(reinterpret_cast<tgg00_RecPtr>(pSepSpace));
        
        Separator.bd520CreateNew (Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501,
            Order.ioSubNodeId_bd501(), Order.ioLeafCount_bd501());
        
#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        if (g01glob.treecheck)
            Separator.bd520Check(m_Node.m_Current);
#       endif
    }
    else
    {
        m_Node.m_pNode->ndLeafCount_bd00() += Order.ioLeafCount_bd501();
        
        m_Node.bd600AddRecordSpace (NewSepLen, SepRecIndex, pSepSpace, false);
        if ( e_ok != m_TrError ) return;
        
        Separator.Assign(reinterpret_cast<tgg00_RecPtr>(pSepSpace));
        
        Separator.bd520CreateNew (Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501,
            Order.ioSubNodeId_bd501(), Order.ioLeafCount_bd501());
        
#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        if (g01glob.treecheck)
            Separator.bd520Check(m_Node.m_Current);
#       endif       
        
        //if ( (FIRST_REC_INDEX_BD00 == SepRecIndex) || (0 == NewOrders.iosOrderCount_bd501))  // PTS 1103693 AK 19-08-1999 
        //if (bLastOrderForCurrentNode) PTS 1109033 UH 2001-1-17 commented                     // PTS 1103693 AK 19-08-1999 
        bd520_Propagate (bFirstKeyIsChanged, NewOrders);                                       // PTS 1103693 AK 19-08-1999 
    }
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd510_ExecuteChangeOrder (cbd501_IndexOrder  &Order,
                                       bool                bLastOrderForCurrentNode,
                                       bool                bFirstKeyIsChanged,       // PTS 1103693 AK 19-08-1999 
                                       bool                bUnderflowAllowed,        // PTS 1109619 UH 2001-03-13
                                       cbd520_Separator   &Separator,
                                       tsp00_Int4         &SepRecIndex,
                                       cbd600_Node        &Neighbor,
                                       cbd600_Node        &NextNeighbor,
                                       cbd501_IndexOrders &NewOrders,
                                       tsp00_Int4         &NumNodeDiff)
{
    ROUTINE_DBG_MEO00 ("bd510_ExecuteChangeOrder");

    /* PURPOSE:                                   */
    /* change count and/or Key                    */
    /* PRE:                                       */
    /* m_PathInfo points to the correct Separator */
    /* and m_Node is the IndexNode to change now  */
    
    if ( Order.ioKeyIsInvalid() /* PTS 1108839 UH 2000-12-27 */ )
    {
        // only change LeafCount
        if (Order.ioLeafCount_bd501() != Separator.bd520LeafCount())  // CRS 1103724 AK 19-08-1999
        {
            bd510_ChangeLeafCountInNode (m_Node, Separator, Order.ioLeafCount_bd501());
            Separator.bd520SetLeafCount (Order.ioLeafCount_bd501());
            //if ( bLastOrderForCurrentNode ) PTS 1109033 UH 2001-1-17 commented
            bd520_Propagate (! UPDATE_KEY, NewOrders);
        }
        return;
    }
    else
    {
        /* change complete separator key */
        tsp00_Int4 RecSizeDelta = Separator.bd520RequiredLen (Order.ioSepKeyLen_bd501)
                                  -
                                  Separator.bd520Length();

        if ( RecSizeDelta > 0 )
        {
            if ( ! m_Node.bd600SpaceIsAvailable (RecSizeDelta) )
            {
                const tsp00_Int4 NumAddLeaves =  Order.ioLeafCount_bd501() - Separator.bd520LeafCount(); // PTS 1103849 AK 01-09-1999 
                bd520_Overflow (Neighbor, NextNeighbor,
                    Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501, NumAddLeaves, // PTS 1103849 AK 01-09-1999
                    ! FOR_INSERT_GBD500, bFirstKeyIsChanged,                       // PTS 1103693 AK 19-08-1999 
                    RecSizeDelta, SepRecIndex, NumNodeDiff, NewOrders);
                if ( e_ok != m_TrError ) return;

                Separator.Assign(m_Node.bd600SepPtr (SepRecIndex));

                m_Node.bd600UpdRecordSpace (RecSizeDelta, SepRecIndex, Separator.bd520GetRecPtr());
                if ( e_ok != m_TrError ) return;

                Separator.bd520CreateNew (Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501,
                                          Order.ioSubNodeId_bd501(), Order.ioLeafCount_bd501());
                
#               if COMPILEMODE_MEO00 >= SLOW_MEO00
                if (g01glob.treecheck)
                    Separator.bd520Check(m_Node.m_Current);
#               endif

                return;
            }
            else
            {
                bd510_ChangeLeafCountInNode (m_Node, Separator, Order.ioLeafCount_bd501());

                m_Node.bd600UpdRecordSpace (RecSizeDelta, SepRecIndex, Separator.bd520GetRecPtr());
                if ( e_ok != m_TrError ) return;

                Separator.bd520CreateNew (Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501,
                                          Order.ioSubNodeId_bd501(), Order.ioLeafCount_bd501());
                
#               if COMPILEMODE_MEO00 >= SLOW_MEO00
                if (g01glob.treecheck)
                    Separator.bd520Check(m_Node.m_Current);
#               endif

                // if ( bLastOrderForCurrentNode )  PTS 1109033 UH 2001-1-17 commented
                bd520_Propagate (bFirstKeyIsChanged, NewOrders);              // PTS 1103693 AK 19-08-1999 
                return;
            }
        }

        if ( RecSizeDelta < 0 )
        {
            bd510_ChangeLeafCountInNode (m_Node, Separator, Order.ioLeafCount_bd501());

            Separator.bd520CreateNew (Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501,
                                      Order.ioSubNodeId_bd501(), Order.ioLeafCount_bd501());
            
#           if COMPILEMODE_MEO00 >= SLOW_MEO00
            if (g01glob.treecheck)
                Separator.bd520Check(m_Node.m_Current);
#           endif

            m_Node.bd600UpdRecordSpace (RecSizeDelta, SepRecIndex, Separator.bd520GetRecPtr());
            if ( e_ok != m_TrError ) return;

            tsp00_Int4 NumOrders = NewOrders.iosOrderCount_bd501;

            if ( bUnderflowAllowed ) // PTS 1109619 UH 2001-03-13 /* PTS 1103564 UH 1999-08-08 */
                bd520_UnderFlow (Neighbor, NextNeighbor, // PTS 1109619 UH 2001-03-13
                                 bFirstKeyIsChanged, NumNodeDiff, NewOrders);

            /* if no Orders are necessary propagate the LeafCount */
            if ( NumOrders == NewOrders.iosOrderCount_bd501 )
                bd520_Propagate (bFirstKeyIsChanged, NewOrders);

            return;
        }

        /* RecSizeDelta == 0 */
        bd510_ChangeLeafCountInNode (m_Node, Separator, Order.ioLeafCount_bd501());

        Separator.bd520CreateNew (Order.ioSepKey_bd501(), Order.ioSepKeyLen_bd501,
            Order.ioSubNodeId_bd501(), Order.ioLeafCount_bd501());

#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        if (g01glob.treecheck)
            Separator.bd520Check(m_Node.m_Current);
#       endif

        //if ( (FIRST_REC_INDEX_BD00 == SepRecIndex) || bLastOrderForCurrentNode )    // PTS 1103693 AK 19-08-1999 
        //if (bLastOrderForCurrentNode) PTS 1109033 UH 2001-1-17 commented            // PTS 1103693 AK 19-08-1999 
        bd520_Propagate (bFirstKeyIsChanged, NewOrders);                              // PTS 1103693 AK 19-08-1999 
        return;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd510_InsertNewLevel (cbd600_Node  &RightNeighbor,
                                   tsp00_KeyPtr  pSepKey,
                                   tsp00_Int4    SepKeyLen,
                                   tsp00_Int4    RecIndex,
                                   bool          bRecordIsInRightNode,
                                   cbd600_Node  &NewLeftSubNode,
                                   tsp00_Int4   &NumNodeDiff)
{
    ROUTINE_DBG_MEO00 ("bd510_InsertNewLevel");

    NewLeftSubNode.bd600New (m_CurrLevel);
    if ( e_ok != m_TrError ) return;

    NewLeftSubNode.bd600CopyBody (m_Node);
    if ( e_ok != m_TrError ) return;
    NewLeftSubNode.m_pNode->nd_right()         = m_Node.m_pNode->nd_right();
    if ( bd500IsClustered() )
        RightNeighbor.m_pNode->nd_left()       = NewLeftSubNode.m_pNode->nd_id();
    NewLeftSubNode.m_pNode->ndLeafCount_bd00() = m_Node.m_pNode->ndLeafCount_bd00();

    if( LEAF_LEVEL_BD00 == m_Node.m_pNode->nd_level() )
    {
        --m_Node.m_Current.currLeafPageCountDelta_bd00;
        ++m_Node.m_Current.currIndexPageCountDelta_bd00;
    }

    m_Node.m_pNode->nd_id()                   = m_Node.m_Current.curr_tree_id.fileRoot_gg00();
    m_Node.m_pNode->nd_checktype().becomes    (chckChecksumData_egg00);
    m_Node.m_pNode->nd_sorted()               = false;
    m_Node.m_pNode->nd_write_cnt()            = 0;
    m_Node.m_pNode->nd_checksum()             = 0;
    m_Node.m_pNode->nd_level()                += 1;
    m_Node.m_pNode->nd_right()                = NIL_PAGE_NO_GG00;
    m_Node.m_pNode->nd_record_cnt()           = 0;
    m_Node.m_pNode->nd_bottom()               = BODY_BEG_BD00;
    m_Node.m_pNode->nd_trailer()              = m_Node.m_pNode->nd_header();
    m_Node.m_pNode->ndPageVersion_bd00()      = 0;

    /* add left separator record */
    cbd520_Separator NewSeparator;
    tsp00_BytePtr    pSepSpace;
    tsp00_Int4       RequiredLen    = NewSeparator.bd520RequiredLen(0)+POINTERSIZE_BD00;
    tsp00_Int4       NewSepRecIndex = FIRST_REC_INDEX_BD00;

    m_Node.bd600AddRecordSpace (RequiredLen, NewSepRecIndex, pSepSpace, false);
    if ( e_ok != m_TrError )
    {
        bd500Print              ("bd510_InsertNewLevel::Error 1");
    	m_Node.bd600Dump        (m_TrError, "bd510_InsertNewLevelERR1");
    	RightNeighbor.bd600Dump (m_TrError, "bd510_InsertNewLevelERR1");
    	NewLeftSubNode.bd600Dump(m_TrError, "bd510_InsertNewLevelERR1");
    	return;
    }

    NewSeparator.Assign(reinterpret_cast<tgg00_RecPtr>(pSepSpace));

    NewSeparator.bd520CreateNew (NULL, 0, NewLeftSubNode.m_pNode->nd_id(),
                                 NewLeftSubNode.m_pNode->ndLeafCount_bd00());

    RequiredLen = NewSeparator.bd520RequiredLen(SepKeyLen)+POINTERSIZE_BD00;
    ++NewSepRecIndex;

    /* there must be enough space, because of MAX_KEY_LEN_GG00 == 1024 */
    m_Node.bd600AddRecordSpace (RequiredLen, NewSepRecIndex, pSepSpace, false);
    if ( e_ok != m_TrError )
    {
        bd500Print              ("bd510_InsertNewLevel::Error 2");
    	m_Node.bd600Dump        (m_TrError, "bd510_InsertNewLevelERR2");
    	RightNeighbor.bd600Dump (m_TrError, "bd510_InsertNewLevelERR2");
    	NewLeftSubNode.bd600Dump(m_TrError, "bd510_InsertNewLevelERR2");
    	return;
    }

    NewSeparator.Assign(reinterpret_cast<tgg00_RecPtr>(pSepSpace));

    NewSeparator.bd520CreateNew (pSepKey, SepKeyLen, RightNeighbor.m_pNode->nd_id(),
                                 RightNeighbor.m_pNode->ndLeafCount_bd00());

    m_Node.m_pNode->ndLeafCount_bd00() += RightNeighbor.m_pNode->ndLeafCount_bd00();

    /* update the PathInfo */

    m_RootLevel = m_Node.m_pNode->nd_level();
    
    tbd500_PathInfoElem* pathInfoElem = m_PathInfo + m_CurrLevel;
        
    if (bRecordIsInRightNode)
    {
        pathInfoElem->piId_bd500              = RightNeighbor.bd600GetId();
        pathInfoElem->piRightId_bd500         = RightNeighbor.bd600RightNeighbor();
        pathInfoElem->piLeftId_bd500          = NewLeftSubNode.bd600GetId();
        pathInfoElem->piMaxRecIndex_bd500     = RightNeighbor.bd600MaxRecIndex();
        pathInfoElem->piLeafCount_bd500       = RightNeighbor.bd600LeafCount(RecIndex,RecIndex);
        pathInfoElem->piSubNodeRecIndex_bd500 = RecIndex;
    }
    else
    {
        pathInfoElem->piId_bd500              = NewLeftSubNode.bd600GetId();
        pathInfoElem->piRightId_bd500         = NewLeftSubNode.bd600RightNeighbor();
        pathInfoElem->piLeftId_bd500          = NIL_PAGE_NO_GG00;
        pathInfoElem->piMaxRecIndex_bd500     = NewLeftSubNode.bd600MaxRecIndex();
        pathInfoElem->piLeafCount_bd500       = NewLeftSubNode.bd600LeafCount(RecIndex,RecIndex);
        pathInfoElem->piSubNodeRecIndex_bd500 = RecIndex;
    }
    
    pathInfoElem = m_PathInfo + m_CurrLevel + 1;
    
    pathInfoElem->piId_bd500              = m_Node.bd600GetId();
    pathInfoElem->piRightId_bd500         = m_Node.bd600RightNeighbor();
    pathInfoElem->piLeftId_bd500          = NIL_PAGE_NO_GG00;
    pathInfoElem->piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
    pathInfoElem->piSubNodeRecIndex_bd500 =(bRecordIsInRightNode)?(FIRST_REC_INDEX_BD00+1):(FIRST_REC_INDEX_BD00);
    pathInfoElem->piLeafCount_bd500       = m_Node.bd600LeafCount(pathInfoElem->piSubNodeRecIndex_bd500,
                                                                  pathInfoElem->piSubNodeRecIndex_bd500);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    NewSeparator.bd520Print();
    t01p4int4 (bd_index, "Root/Lef/Rig", m_Node.m_pNode->ndLeafCount_bd00(),
                                         NewLeftSubNode.m_pNode->ndLeafCount_bd00(),
                                         RightNeighbor.m_pNode->ndLeafCount_bd00(),
                                         0);
#   endif
}


/*---------------------------------------------------------------------------*/

bool
cbd500_Tree::bd510_TryToRemoveLevel (cbd600_Node &Neighbor)
{
    ROUTINE_DBG_MEO00 ("bd510_TryToRemoveLevel");

    tbd_nodeptr pRoot = m_Node.m_Current.bd300RootPtrs().np_ptr();
    cbd600_Node RootNode (m_Node.m_Current, tbd_node_request::fromConst (nr_for_update));

    if ( (m_CurrLevel+1 != m_RootLevel) || (pRoot->nd_record_cnt() > 2) )
        return false;

    RootNode.bd600GetNode(pRoot->nd_id());
    if ( e_ok != m_TrError ) return true;

    // PTS 1117802 UH 2002-10-17 begin
    cbd520_Separator Separator1;
    cbd520_Separator Separator2;
    Separator1.Assign(RootNode.bd600SepPtr(FIRST_REC_INDEX_BD00));
    Separator2.Assign(RootNode.bd600SepPtr(FIRST_REC_INDEX_BD00+1));

    if ( (m_Node.bd600GetId() != Separator1.bd520SubNodeId()
          &&
          m_Node.bd600GetId() != Separator2.bd520SubNodeId())
         ||
         (Neighbor.bd600GetId() != Separator1.bd520SubNodeId()
          &&
          Neighbor.bd600GetId() != Separator2.bd520SubNodeId()) )
        // one of the nodes was added previously by an AddOrder
        return false;
    // PTS 1117802 UH 2002-10-17 end

    /* really remove the level */

    /* delete the subnodes and fill the root with the correct subnode body */
    cbd600_Node* NodeToDelete;
    cbd600_Node* SubNode;

    if ( 0 == m_Node.m_pNode->nd_record_cnt() )
    {
        NodeToDelete = &m_Node;
        SubNode      = &Neighbor;
    }
    else
    {
        NodeToDelete = &Neighbor;
        SubNode      = &m_Node;
    }

    /* update the SubNodeRecIndex in the PathInfo  */
    /* so the following orders can be executed     */
    /* PTS 1103562 UH 1999-08-08                   */
    /* Note: the NodeToDelete is always the right  */
    /* node. Therefore whenever the NodeToDelete   */
    /* is in the path the recindex in the pathinfo */
    /* has to be updated                           */
    /* PTS 1103617 TS 1999-08-13                   */

    if (NodeToDelete->bd600GetId() == m_PathInfo[m_CurrLevel].piId_bd500) 
        m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 += (1 + SubNode->bd600MaxRecIndex());
    m_PathInfo[m_CurrLevel].piId_bd500 = RootNode.bd600GetId();
        
    NodeToDelete->bd600Free();
    if ( e_ok != m_TrError ) return true;

    --RootNode.m_pNode->nd_level();

    RootNode.bd600CopyBody (*SubNode);
    if ( e_ok != m_TrError ) return true;

    SubNode->bd600Free();
    if ( e_ok != m_TrError ) return true;

    if( LEAF_LEVEL_BD00 == RootNode.m_pNode->nd_level() )
    {
        ++m_Node.m_Current.currLeafPageCountDelta_bd00;
        --m_Node.m_Current.currIndexPageCountDelta_bd00;
    }

    /* update the Tree Internal Members */

    m_PathInfo[m_CurrLevel+1].piId_bd500        = NIL_PAGE_NO_GG00;
    m_PathInfo[m_CurrLevel  ].piId_bd500        = RootNode.bd600GetId();
    m_PathInfo[m_CurrLevel  ].piRightId_bd500   = NIL_PAGE_NO_GG00;
    m_PathInfo[m_CurrLevel  ].piLeftId_bd500    = NIL_PAGE_NO_GG00;
    m_PathInfo[m_CurrLevel  ].piLeafCount_bd500 = RootNode.bd600LeafCount(m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500,
                                                                          m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500);
        
    m_RootLevel = RootNode.m_pNode->nd_level();

    /* set the current node to the root node */
    m_Node.bd600Exchange (RootNode);

    return true;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
