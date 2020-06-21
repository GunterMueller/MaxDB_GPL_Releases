/*!
  @file           vbd500.cpp
  @ingroup        Tree
  @author         UweH
  @see            example.html ...
  @brief          methods for navigating through the tree structure

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
#include <string.h>

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd500.h"
#include "gbd501.h"
#include "gsp03.h"
#include "gsp03_3.h"


#include "hbd20_2.h" // bd20IsPageRequested
#include "hbd13_3.h" // bd13LoadNodes
#include "heo51.h"   // vgetpid
#include "heo56.h"   // vsleep
#include "hgg01_1.h" // g01abort,g01opmsg
#include "hgg01_3.h" // g01vtrace
#include "hsp77.h"   // sp77sprintf

#include "Trace/Trace_Entry.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "Converter/Converter_PageCollector.hpp"
#include "DataAccess/Data_IOBuffer.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

#define NIL_REC_INDEX -1

/*---------------------------------------------------------------------------*/

inline bool
cbd500_Tree::bd500_BeyondStopKey (bool FromNextNode) const
{
    ROUTINE_DBG_MEO00 ("bd500_BeyondStopKey");

    if (NULL == m_pStopKey)
        return false;

    tsp00_KeyPtr      pKey;
    tsp00_Int2        KeyLength;
    tsp00_LcompResult CompResult;

    m_Node.bd600GetKey ( FromNextNode ? FIRST_REC_INDEX_BD00 : m_Node.bd600MaxRecIndex(),
                         pKey, KeyLength );

    s30cmp (pKey,       POS_OFF_DIFF_BD00, KeyLength,
            m_pStopKey, POS_OFF_DIFF_BD00, m_StopKeyLen, 
            CompResult);

    return FromNextNode ? l_greater == CompResult : l_less == CompResult;
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500FindBranch (cbd600_Node  &Node,
                              tsp00_KeyPtr  pSepKey,
                              tsp00_Int4    SepKeyLen,
                              tsp00_Int4   &SepRecIndex)
{
    ROUTINE_DBG_MEO00 ("bd500FindBranch");

    tbd_searchresult SearchResult;

    SearchResult.becomes (nonefound);

    Node.bd600SearchRecord (pSepKey, SepKeyLen, SepRecIndex, SearchResult);
    if ( e_ok != Node.m_TrError ) return;

    switch (SearchResult)
    {
    case thisfound: break;         // separator found
    case nextfound: --SepRecIndex; // separator found
                    break;
    case lastfound: break;         // separator found
    case nonefound: break;         // impossible
    default:        break;         // impossible
    }

    if (SepRecIndex < FIRST_REC_INDEX_BD00)
    {
        Node.m_TrError = e_invalid_index_structure;
        return;
    }
                        
    if (Node.bd600SepPtr(SepRecIndex)->recLen_gg00() > sizeof (tbd00_Separator))
    {
        Node.m_TrError = e_illegal_branchlength;
    }
        return;
}


/*---------------------------------------------------------------------------*/

void cbd500_Tree::bd500_NextSubNode (tsp00_Int4       RecIndex,
                                     tsp00_Int4       TargetLevel,
                                     tbd_node_request NodeRequest)
{
    ROUTINE_DBG_MEO00 ("bd500_NextSubNode");

    if ( LEAF_LEVEL_BD00 != m_CurrLevel )
    {
        /* invalidate the next value, so NextNode is not functioning wrong */
        m_LeftMostIdNextLowerLevel = NIL_PAGE_NO_GG00;

        if (g01glob.treecheck)
        {
            m_Node.bd600CheckIndex();
            if ( m_TrError != e_ok )
                return;
        }

        tbd500_PathInfoElem & PathInfo   = m_PathInfo [m_CurrLevel];

        PathInfo.piId_bd500              = m_Node.m_pNode->nd_id();
        PathInfo.piRightId_bd500         = m_Node.m_pNode->nd_right();
        PathInfo.piSubNodeRecIndex_bd500 = RecIndex;
        PathInfo.piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
        PathInfo.piLeafCount_bd500       = m_Node.bd600LeafCount(RecIndex, RecIndex);

        --m_CurrLevel;

        tbd500_PathInfoElem & SubNodePathInfo = m_PathInfo [m_CurrLevel];

        SubNodePathInfo.piId_bd500 = m_Node.bd600SepPtr(RecIndex)->recPno_gg00().gg90GetInt4();

        if ( FIRST_REC_INDEX_BD00 == RecIndex )
            SubNodePathInfo.piLeftId_bd500 = NIL_PAGE_NO_GG00;
        else
            SubNodePathInfo.piLeftId_bd500 = m_Node.bd600SepPtr (RecIndex-1)->recPno_gg00().gg90GetInt4();

        if ( m_ReadClustered
             &&
             m_CurrLevel == LEAF_LEVEL_BD00 + 1
             &&
             m_CurrLevel > TargetLevel )
        {
            m_ReadClustered = false;
            bd500_ReadClustered(PathInfo.piSubNodeRecIndex_bd500);
            m_TrError = e_ok;
        }

        m_Node.bd600Release ( nr_for_update == m_Node.m_RequestKind );
   
        if ( m_CurrLevel > TargetLevel )
            m_Node.bd600GetNode (SubNodePathInfo.piId_bd500, tbd_node_request::fromConst(nr_for_read));
        else
            m_Node.bd600GetNode (SubNodePathInfo.piId_bd500, NodeRequest);

        if ( e_ok == m_TrError && m_Node.bd600IsAccessable() )
            SubNodePathInfo.piRightId_bd500 = m_Node.m_pNode->nd_right();
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01int4 (bd_index, "SubNodeId:  ", (m_Node.m_pNode!=NULL)?m_Node.m_pNode->nd_id():NIL_PAGE_NO_GG00);
#   endif
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500FindNode (tsp00_KeyPtr pKey,
                            tsp00_Int4   KeyLen,
                            tsp00_Int4   Level = LEAF_LEVEL_BD00)
{
    /* Preconditions:
       - The Tree is locked correctly.
       - The internal Node (m_Node) contains the NodeRequest
         for the Node/Leaf, which is searched.
       Description:
       - The Search begins always with the root
    */
    ROUTINE_DBG_MEO00 ("bd500FindNode");

    tsp00_Int4       RecIndex;
    tbd_node_request TargetRequest = m_Node.m_RequestKind;

    bd500SetToRoot(); 
    if ( e_ok != m_TrError ) return;

    while ( m_CurrLevel > Level)
    {
        bd500_FindBranch (pKey, KeyLen, RecIndex);
        if ( e_ok != m_TrError ) break;

        bd500_NextSubNode (RecIndex, Level, TargetRequest);
        if ( e_ok != m_TrError ) break;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500Continue ( const tsp00_KeyPtr rePositionKey,
                             const tsp00_Int2   rePositionKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd500Continue");
    m_Node.m_Current.bd300LockTree();
    if ( e_ok == m_TrError )
        bd500FindNode (rePositionKey, rePositionKeyLen, LEAF_LEVEL_BD00);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500_CheckStopKeyAndInterrupt (bool bAscending)
{
    if ( m_Node.bd600IsLeaf() )
    {
        if ( bd500_BeyondStopKey(bAscending) )
        {
#           if COMPILEMODE_MEO00 >= SLOW_MEO00 
            t01int4 (bd_index, "BeyondStp ID", m_Node.m_pNode->nd_id());
#           endif
            m_Node.bd600Release (IS_CHANGED_BD600);
        }
        else
        {
            if ( m_Interruptible &&  // PTS 1121770 TS 2003-04-25
                m_Node.m_Current.bd300RootPtrs().np_ptr() != 0 ) // PTS 1121589 UH 2003-04-11 (Drop has not)
            {
                
                if ( (nr_for_read == m_Node.m_Current.bd300RootRequestKind())
                    &&
                    bd20IsPageRequested (m_Node.m_Current.bd300RootPtrs().np_cbptr()) )
                {
                    // PTS 1121589 UH 2003-04-11 local rePositionKey added
                    tsp00_Key    rePositionKeyValue;
                    tsp00_KeyPtr rePositionKey = reinterpret_cast<tsp00_KeyPtr>(&rePositionKeyValue);
                    tsp00_Int2   rePositionKeyLen; 
                    
                    const tsp00_Int4 recIndex = bAscending ? FIRST_REC_INDEX_BD00 : m_Node.bd600MaxRecIndex();
                    bd500Break( recIndex, rePositionKey, rePositionKeyLen );
                    
                    // TorstenS 2002-07-17: reduce data cache priority problems by rescheduling the task
                    // tsp00_TaskId taskId;  PTS 1121589 UH 2003-04-11 exchanged with trTaskId
                    // vgetpid( taskId );    PTS 1121589 UH 2003-04-11 exchanged with trTaskId
                    vsleep( m_Node.m_Current.curr_trans->trTaskId_gg00, 0 ); 
                    
                    bd500Continue ( rePositionKey, rePositionKeyLen);
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500NextNode ()
{
    ROUTINE_DBG_MEO00 ("bd500NextNode");

    if ( m_Node.m_Current.curr_trans->trRteCommPtr_gg00->to_cancel
         &&    
         (m_select == m_Node.m_Current.curr_action || m_verify == m_Node.m_Current.curr_action) )                     
    {
        m_TrError = e_cancelled;
        g01opmsg (sp3p_knldiag, sp3m_info, csp3_bd_msg, csp3_n_btree,
              "BD500: Next cancelled   " , m_Node.m_Current.curr_trans->trTaskId_gg00);
        return;
    }

    if ( ! m_Node.bd600IsAccessable() )
        return;

    if ( m_Node.bd600IsCached() && m_pIOBuffer == 0 ) // PTS 1136998 UH 2006-01-23
    {
        // determine right neighbor
        tsp00_KeyPtr pKey;
        tsp00_Int2   keyLength;
        tbd_nodeptr  pPageCache = 0;
        
        // +++++ TODO get leaf directly and test if key can be found there
        // instead of searching the leaf through the tree again.
        
        m_Node.bd600GetKey(m_Node.bd600MaxRecIndex(), pKey, keyLength);
        m_Node.bd600DisableCaching(pPageCache);
        bd500Continue(pKey, keyLength);
        if ( ! m_Node.bd600IsAccessable() || e_ok != m_TrError )
            return;
        m_Node.bd600EnableCaching(pPageCache);
    }
    
    tbd500_PathInfoElem &piIndexElem = m_PathInfo[m_CurrLevel-1];

    if ( NIL_PAGE_NO_GG00 != m_Node.bd600RightNeighbor() )
    {
        //#ifdef SAPDB_SLOW
        if ( m_PathInfo[m_CurrLevel].piRightId_bd500 != m_Node.bd600RightNeighbor()  )
            bd500_InvalidIndexStructure (csp3_bd_msg, "right neighbor is wrong ",
                                         m_Node.bd600GetId());
        //#endif

        m_PathInfo[m_CurrLevel].piLeftId_bd500 = m_Node.bd600GetId();

        m_Node.bd600Release(true);
        if ( e_ok != m_TrError )
            return;
        if ( m_pIOBuffer != 0
             &&
             m_CurrLevel == LEAF_LEVEL_BD00
             &&
             m_CurrLevel < m_RootLevel 
             &&
             piIndexElem.piMaxRecIndex_bd500 == piIndexElem.piSubNodeRecIndex_bd500
             &&
             piIndexElem.piRightId_bd500 != NIL_PAGE_NO_GG00 )
        {
            m_Node.bd600_GetPageFromDataCache (piIndexElem.piRightId_bd500);
            if ( e_ok == m_TrError )
            {
                if ( m_Node.bd600IsAccessable() )
                {
                    piIndexElem.piId_bd500              = m_Node.bd600GetId();
                    piIndexElem.piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
                    piIndexElem.piRightId_bd500         = m_Node.bd600RightNeighbor();
                    piIndexElem.piSubNodeRecIndex_bd500 = FIRST_REC_INDEX_BD00;
                    piIndexElem.piLeafCount_bd500       = m_Node.bd600LeafCount(FIRST_REC_INDEX_BD00,FIRST_REC_INDEX_BD00);
                }  
                bd500_ReadClustered(FIRST_REC_INDEX_BD00);
                m_TrError = e_ok;
            }
            m_Node.bd600Release(true);
            m_TrError = e_ok;
        }
        m_Node.bd600_GetPageFromDataCache (m_PathInfo[m_CurrLevel].piRightId_bd500);
        if ( e_ok != m_TrError )
            return;
        
        m_PathInfo[m_CurrLevel].piId_bd500              = m_Node.bd600GetId();
        m_PathInfo[m_CurrLevel].piRightId_bd500         = m_Node.bd600RightNeighbor();
        m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 = FIRST_REC_INDEX_BD00;
        m_PathInfo[m_CurrLevel].piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
        m_PathInfo[m_CurrLevel].piLeafCount_bd500       = m_Node.bd600LeafCount(FIRST_REC_INDEX_BD00,FIRST_REC_INDEX_BD00);
        
        ++(m_PathInfo[m_CurrLevel-1].piSubNodeRecIndex_bd500);
    }
    else
    {
        if ( m_Node.bd600IsLeaf() )
        {
            m_Node.bd600Release (IS_CHANGED_BD600);
            return;
        }
        else
            m_Node.bd600Release (IS_CHANGED_BD600);

        if ( NIL_PAGE_NO_GG00 != m_LeftMostIdNextLowerLevel )
        {
            m_Node.bd600GetNode (m_LeftMostIdNextLowerLevel);
            if ( e_ok != m_TrError )
                return;
                
            #if COMPILEMODE_MEO00 >= SLOW_MEO00 
            t01int4 (bd_index, "ToLeftMostId", m_LeftMostIdNextLowerLevel);
            #endif

            m_CurrLevel = m_Node.bd600Level();

            bd500_SetLeftMostIdForLowerLevel (m_Node.m_pNode);

            m_PathInfo[m_CurrLevel].piId_bd500              = m_Node.bd600GetId();
            m_PathInfo[m_CurrLevel].piLeftId_bd500          = NIL_PAGE_NO_GG00;
            m_PathInfo[m_CurrLevel].piRightId_bd500         = m_Node.bd600RightNeighbor();
            m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 = FIRST_REC_INDEX_BD00;
            m_PathInfo[m_CurrLevel].piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
            m_PathInfo[m_CurrLevel].piLeafCount_bd500       = m_Node.bd600LeafCount(FIRST_REC_INDEX_BD00,FIRST_REC_INDEX_BD00);
        }
    }

    if ( ! m_Node.bd600IsAccessable() || e_ok != m_TrError )
        return;

    bd500_CheckStopKeyAndInterrupt (ASCENDING_GBD500);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500PrevNode ()
{
    ROUTINE_DBG_MEO00 ("bd500PrevNode");

    if ( // PTS 1103703 AK 30-08-1999 
        (m_Node.m_Current.curr_trans->trRteCommPtr_gg00->to_cancel) &&    
        (
        (m_select == m_Node.m_Current.curr_action) ||
        (m_verify == m_Node.m_Current.curr_action)
        )
       )                     
    {
        m_TrError = e_cancelled;
        g01opmsg (sp3p_knldiag, sp3m_info, csp3_bd_msg, csp3_n_btree,
              "BD500: Prev cancelled   " , m_Node.m_Current.curr_trans->trTaskId_gg00);
        return;
    }

    // PTS 1122998 UH 2003-07-09 completely rewritten (was: bd500_SetPathInfoToLeftNeighbor)

    if ( g01vtrace.vtrBdIndex_gg00 )
        bd500Print("bd500PrevNode::BEGIN");

    const tsp00_Int4       TargetLevel    = m_CurrLevel;
    const tbd_node_request NodeRequest    = m_Node.m_RequestKind;
    const tsp00_PageNo     checkId        = m_Node.bd600IsAccessable() ? m_Node.bd600GetId() : NIL_PAGE_NO_GG00;
          bool             bSearchSubNode = false;
          
    if ( m_Node.bd600IsAccessable()
         &&
         m_Node.bd600Level() != m_CurrLevel ) // only check
    {
        m_Node.bd600Release (true);
        m_TrError = e_invalid_index_structure;
        bd500Print("bd500PrevNode: m_CurrLevel is wrong");
        return;
    }

    m_Node.bd600Release (true); // if m_Node is not accessable the iterator has to stop

    // position node to left leaf or to index node to go down again
    while ( m_CurrLevel < m_RootLevel )
    {
        // 1. is left neighbour known ?
        if ( m_PathInfo[m_CurrLevel].piLeftId_bd500 != NIL_PAGE_NO_GG00 )
        {
            if ( m_PathInfo[m_CurrLevel+1].piSubNodeRecIndex_bd500 == FIRST_REC_INDEX_BD00 )
            {
                m_TrError = e_invalid_index_structure;
                bd500Print("bd500PrevNode: Wrong PathInfo1");
                return;
            }
            m_Node.bd600Move(m_PathInfo[m_CurrLevel].piLeftId_bd500);
            if ( e_ok != m_TrError )
            {
                bd500Print("bd500PrevNode::no access to left neighbor");
                return;
            }
            m_PathInfo[m_CurrLevel].piRightId_bd500         = m_PathInfo[m_CurrLevel].piId_bd500;
            m_PathInfo[m_CurrLevel].piId_bd500              = m_PathInfo[m_CurrLevel].piLeftId_bd500;
            m_PathInfo[m_CurrLevel].piLeftId_bd500          = NIL_PAGE_NO_GG00;
            m_PathInfo[m_CurrLevel].piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
            m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 = m_Node.bd600MaxRecIndex();
            m_PathInfo[m_CurrLevel].piLeafCount_bd500       = m_Node.bd600LeafCount(m_Node.bd600MaxRecIndex(),m_Node.bd600MaxRecIndex());
            --(m_PathInfo[m_CurrLevel+1].piSubNodeRecIndex_bd500); // update index above
            if ( TargetLevel == m_CurrLevel )
            {
                // the simplest way - the left neighbor node is known
                if ( checkId != NIL_PAGE_NO_GG00
                     &&
                     checkId != m_Node.bd600RightNeighbor() )
                {
                    m_TrError = e_invalid_index_structure;
                    bd500Print("bd500PrevNode: Wrong PathInfo2");
                    return;
                }
                if ( g01vtrace.vtrBdIndex_gg00 )
                    bd500Print("bd500PrevNode: FOUND simple");
                return;
            }
            bSearchSubNode = true;
            break;
        }

        // invalidate the pathinfo for this level because we must search for a new way
        m_PathInfo[m_CurrLevel].piId_bd500              = NIL_PAGE_NO_GG00;
        m_PathInfo[m_CurrLevel].piRightId_bd500         = NIL_PAGE_NO_GG00;
        m_PathInfo[m_CurrLevel].piLeftId_bd500          = NIL_PAGE_NO_GG00;
        m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 = NIL_RECINDEX_BD00;
        m_PathInfo[m_CurrLevel].piMaxRecIndex_bd500     = NIL_RECINDEX_BD00;
        m_PathInfo[m_CurrLevel].piLeafCount_bd500       = -1;

        // 2. try to use the next higher index node

        ++m_CurrLevel;

        if ( m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500 > FIRST_REC_INDEX_BD00 )
        {
            // This index node can be used to find the leaf again.
            --(m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500);
            m_Node.bd600Move(m_PathInfo[m_CurrLevel].piId_bd500);
            bSearchSubNode = true;
            break;
        }
    }//endwhile

    if ( ! bSearchSubNode )
    {
        if ( g01vtrace.vtrBdIndex_gg00 )
            bd500Print("bd500PrevNode: NOT FOUND");
        return; // the loop above did not find any left neighbor
    }

    /* Find leaf node again from found index node */

    // Start with determined Separator
    bd500_NextSubNode (m_PathInfo[m_CurrLevel].piSubNodeRecIndex_bd500, TargetLevel, NodeRequest);
    if ( e_ok != m_TrError )
    {
        bd500Print("bd500PrevNode:no access to subnode1");
        return;
    }
    
    while ( m_CurrLevel > TargetLevel )
    {
        bd500_NextSubNode (m_Node.bd600MaxRecIndex(), TargetLevel, NodeRequest);
        if ( e_ok != m_TrError )
        {
            bd500Print("bd500PrevNode:no access to subnode2");
            return;
        }
    }
    if ( m_CurrLevel != TargetLevel )
    {
        m_TrError = e_invalid_index_structure;
        bd500Print("bd500PrevNode: target level not reached");
        return;
    }
    
    if ( checkId != NIL_PAGE_NO_GG00
         &&
         checkId != m_Node.bd600RightNeighbor() )
    {
        m_TrError = e_invalid_index_structure;
        bd500Print("bd500PrevNode: Wrong PathInfo3");
        return;
    }
    
    if ( g01vtrace.vtrBdIndex_gg00 )
        bd500Print("bd500PrevNode::FOUND complex");

    if ( m_select == m_Node.m_Current.curr_action
         &&
         m_Node.bd600IsAccessable() )
        bd500_CheckStopKeyAndInterrupt (! ASCENDING_GBD500);
}

/*---------------------------------------------------------------------------*/

tsp00_PageNo
cbd500_Tree::bd500LeftNeighborId (cbd600_Node &auxNode) const
{
    // This uses the m_PathInfo, but does not change it.
    
    ROUTINE_DBG_MEO00 ("bd500LeftNeighborId");

    // PTS 1122998 UH 2003-07-09 new

    if ( g01vtrace.vtrBdIndex_gg00 )
        bd500Print("bd500LeftNeighborId::BEGIN");

    if ( auxNode.bd600IsAccessable() )
    {
        auxNode.bd600TrError() = e_not_implemented;
        return NIL_PAGE_NO_GG00;
    }
    
    if ( m_CurrLevel >= m_RootLevel )
        return NIL_PAGE_NO_GG00;

    const tsp00_Int4   TargetLevel = m_CurrLevel;
    const tsp00_PageNo checkId     = m_PathInfo[m_CurrLevel].piId_bd500;

	tsp00_Int4   CurrentLevel        = m_CurrLevel;
    tsp00_Int4   CurrentSubNodeIndex = NIL_REC_INDEX;
    tsp00_PageNo IndexNodeId         = NIL_PAGE_NO_GG00;

          
    // position node to left leaf or to index node to go down again

    while ( CurrentLevel < m_RootLevel
            &&
            IndexNodeId == NIL_PAGE_NO_GG00 )
    {
        if ( m_PathInfo[CurrentLevel].piLeftId_bd500 != NIL_PAGE_NO_GG00 )
        {
            // left neighbor is known so take this one
            // the rightmost separator must be used
            // this is indeicated by NIL_REC_INDEX
            IndexNodeId = m_PathInfo[CurrentLevel].piLeftId_bd500;

            if ( nr_for_read == m_Node.m_RequestKind
                 &&
                 m_PathInfo[CurrentLevel+1].piSubNodeRecIndex_bd500 == FIRST_REC_INDEX_BD00 )
            {
                // if for_update this may not be valid because of overflow or underflow handling
                auxNode.bd600TrError() = e_invalid_index_structure;
                bd500Print("bd500LeftNeighborId: Wrong PathInfo1");
                return NIL_PAGE_NO_GG00;
            }
            break;
        }
        ++CurrentLevel;
        if ( m_PathInfo[CurrentLevel].piSubNodeRecIndex_bd500 > FIRST_REC_INDEX_BD00 )
        {
            // on the next higher level the index node can be used to go down again
            // the left subnode is used
            IndexNodeId         = m_PathInfo[CurrentLevel].piId_bd500;
            CurrentSubNodeIndex = m_PathInfo[CurrentLevel].piSubNodeRecIndex_bd500-1;
            break;
        }
    }

    if ( NIL_PAGE_NO_GG00 == IndexNodeId )
    {
        if ( g01vtrace.vtrBdIndex_gg00 )
            bd500Print("bd500LeftNeighborId: NOT FOUND");
        return NIL_PAGE_NO_GG00; // the loop above did not find any left neighbor
    }

    if ( CurrentLevel == TargetLevel )
    {
        if ( g01vtrace.vtrBdIndex_gg00 )
            bd500Print("bd500LeftNeighborId: FOUND simple");
        return IndexNodeId;
    }
    
    // The CurrentLevel is above the TargetLevel so go down over the rightmost separator.

    auxNode.bd600Move(IndexNodeId);
    if ( e_ok != auxNode.bd600TrError() )
    {
        auxNode.bd600Release (false); // release aux node if necessary
        bd500Print("bd500LeftNeighborId: no access to index node");
        return NIL_PAGE_NO_GG00;
    }
    
    if ( CurrentSubNodeIndex == NIL_REC_INDEX )
        CurrentSubNodeIndex = auxNode.bd600MaxRecIndex();
        
    /* Find leaf node again from found index node */

    while ( CurrentLevel > TargetLevel )
    {
        auxNode.bd600Move(auxNode.bd600SepPtr(CurrentSubNodeIndex)->recPno_gg00().gg90GetInt4());
        if ( e_ok != auxNode.bd600TrError() )
        {
            auxNode.bd600Release (false); // release aux node
            bd500Print("bd500LeftNeighborId:no access to subnode1");
            return NIL_PAGE_NO_GG00;
        }
        --CurrentLevel;
        CurrentSubNodeIndex = auxNode.bd600MaxRecIndex();
    }
    if ( CurrentLevel != TargetLevel )
    {
        auxNode.bd600Release (false); // release aux node
        auxNode.bd600TrError() = e_invalid_index_structure;
        bd500Print("bd500LeftNeighborId: target level not reached");
        return NIL_PAGE_NO_GG00;
    }
    
    if ( checkId != NIL_PAGE_NO_GG00
         &&
         auxNode.bd600IsAccessable()
         &&
         checkId != auxNode.bd600RightNeighbor() )
    {
        auxNode.bd600TrError() = e_invalid_index_structure;
        bd500Print("bd500LeftNeighborId: Wrong PathInfo3");
        return NIL_PAGE_NO_GG00;
    }
    
    if ( g01vtrace.vtrBdIndex_gg00 )
        bd500Print("bd500LeftNeighborId::FOUND complex");

    const tsp00_PageNo leftId = auxNode.bd600GetId();
    auxNode.bd600Release (false); // release aux node
    return leftId;
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500Break (    
    const tsp00_Int4    recIndex,
    const tsp00_KeyPtr  rePositionKey,      
    tsp00_Int2          &rePositionKeyLen ) 
{
    ROUTINE_DBG_MEO00 ("bd500Break");

    tsp00_KeyPtr pKey;
    tsp00_Int2   keyLength;

    m_Node.bd600GetKey (recIndex, pKey, keyLength);
    
    SAPDB_RangeMove( __FILE__, 1,
        sizeof( tsp00_Key ), sizeof( tsp00_Key ),
        pKey, POS_OFF_DIFF_BD00, rePositionKey, POS_OFF_DIFF_BD00,  
        keyLength, m_TrError );

    if ( e_ok != m_TrError ) 
        return;

     rePositionKeyLen = keyLength;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    g01opmsg (sp3p_knldiag, sp3m_info, csp3_bd_msg, csp3_n_btree,
          "BD500: Interrupting  Id:" , m_Node.m_pNode->nd_id() );
#   endif

    m_Node.bd600Release(IS_CHANGED_BD600);
    m_Node.m_Current.bd300UnlockTree();
}

/*---------------------------------------------------------------------------*/

void cbd500_Tree::bd500_WriteTrace (const char      *Line,
                                    tsp00_Int4       LineLen) const
{
    Trace_CommonEntry entry( tgg00_Debug::fromConst(ta), tgg00_VtraceType::fromConst(td_none), 
                       reinterpret_cast< const tsp00_Byte*> ( Line ), LineLen );
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500Print (const char* Title) const
{
    const tsp00_Int4 LineSize = sizeof(tsp00_Line)+1;
    char             Line [LineSize ];
//  tsp00_Int4       LineLen;

    if ( NULL != Title )
        bd500_WriteTrace (Title, (tsp00_Int4)strlen(Title));

    if ( m_Node.m_pNode != NULL )
    {
        if ( pt2InvSubtree_egg00 == m_Node.m_pNode->nd_pt2() )
            bd500_WriteTrace ("*** InvTree (Sub) ***", 21);
        else
            bd500_WriteTrace ("*** InvTree       ***", 21);
    }
    else
        bd500_WriteTrace ("*** Tree          ***", 21);

    sp77sprintf (Line, LineSize, "* CurrLevel %d, CurrNode %d, LeftMostIdNLL %d, Action %d",
                 m_CurrLevel, (m_Node.m_pNode!=NULL)?m_Node.m_pNode->nd_id():NIL_PAGE_NO_GG00,
                 m_LeftMostIdNextLowerLevel, (int)(m_Node.m_Current.curr_action)); // PTS 1110051 UH 2001-05-09
    bd500_WriteTrace (Line, (tsp00_Int4)strlen(Line));

    bd500_WriteTrace ("*-------+--------------+------------+----------+----------+------------+", 72);
    bd500_WriteTrace ("* Level | LeftNeighbor |       Node | RecIndex | MxRecIdx |  LeafCount |", 72);
    for (tsp00_Int4 Level = m_RootLevel; Level >= LEAF_LEVEL_BD00; --Level)
    {
        sp77sprintf (Line, LineSize, "* %5d | %10d   | %10d | %8d | %8d | %10d |",
                     Level, m_PathInfo [Level].piLeftId_bd500, m_PathInfo [Level].piId_bd500,
                     m_PathInfo [Level].piSubNodeRecIndex_bd500,
                     m_PathInfo [Level].piMaxRecIndex_bd500, m_PathInfo [Level].piLeafCount_bd500);
        bd500_WriteTrace (Line, (tsp00_Int4)strlen(Line));
    }
    bd500_WriteTrace ("*-------+--------------+------------+----------+----------+------------+", 72);
}


/*---------------------------------------------------------------------------*/

tsp00_PageNo
cbd500_Tree::bd500LeftNeighbor ()
{
    ROUTINE_DBG_MEO00 ("bd500LeftNeighbor");

    if ( ! m_Node.bd600IsAccessable() )
        return NIL_PAGE_NO_GG00;

    cbd600_Node auxNode(m_Node.m_Current, m_Node.m_RequestKind);

    return m_Node.bd600LeftNeighbor() != NIL_PAGE_NO_GG00
           ? m_Node.bd600LeftNeighbor()
           : bd500LeftNeighborId(auxNode);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500SetToNodeOnEdge (bool leftEdge, tsp00_Int2 level)
{
    ROUTINE_DBG_MEO00 ("bd500SetToNodeOnEdge");

    const tsp00_Int2 targetLevel = level < 0 || level > MAX_LEVEL_BD00
                                   ? LEAF_LEVEL_BD00 : level;

    bd500SetToRoot();
    if ( e_ok != m_TrError ) return;

    tbd_node_request TargetRequest = m_Node.m_RequestKind;

    while ( m_CurrLevel > targetLevel)
    {
        if ( leftEdge )
            bd500_NextSubNode (FIRST_REC_INDEX_BD00, targetLevel, TargetRequest);
        else
            bd500_NextSubNode (m_Node.bd600MaxRecIndex(), targetLevel, TargetRequest);
        if ( e_ok != m_TrError ) break;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500_InvalidIndexStructure (tsp00_Int4     MsgNo,
                                          const char*  MsgText,
                                          tsp00_Int4     BadValue)
{
    ROUTINE_DBG_MEO00 ("bd500_InvalidIndexStructure");
    m_TrError = e_invalid_index_structure;
    bd500Print ("bd500_InvalidIndexStructure");
    g01abort (MsgNo, csp3_n_btree, MsgText, BadValue);
}


/*---------------------------------------------------------------------------*/
/// The current node is an index node. Beginning with given RecIndex all referenced leafpages are read in a cluster
void cbd500_Tree::bd500_ReadClustered(tsp00_Int4 RecIndex)
{
    ROUTINE_DBG_MEO00 ("bd500_ReadClustered");
    // If the page collection throws any error it is ignored and routine is left. 

    if( m_Node.bd600Level() != LEAF_LEVEL_BD00 + 1 || m_pIOBuffer == 0 )
        return;

    Trans_Context           &trans = Trans_Context::GetContext(*m_Node.m_Current.curr_trans);
    Converter_PageCollector  pageCollection (trans.Allocator(), Data_PageRecoveryMode());
    cbd520_Separator         sep;
    Kernel_VTrace            trace;

    if( ! pageCollection.Initialize() )
        return;

    if ( DataIndex_Trace.TracesLevel(6) )
    {
        bd500Print("cbd500_Tree::bd500_ReadClustered");
        m_Node.bd600Print();
    }

    for ( cbd600_Node::Iterator recIter = m_Node.bd600GetFirstRecord();
          recIter.IsValid();
          ++recIter )
    {
        sep.Assign(*recIter);
        
        if( ! pageCollection.AppendPageNo(sep.bd520SubNodeId()) )
            return;
            
        if ( DataIndex_Trace.TracesLevel(6) )
            trace << "SepRecIndex: " << recIter.Index() << " -> " << sep.bd520SubNodeId() << FlushLine;
    }
    
    pageCollection.DetermineCluster( trans.Task() );
    
    Data_PageNoList pageNoList( trans.Allocator());

    if( ! pageNoList.Reserve(pageCollection.GetMaxClusterSize()) )
        return;

    IOMan_ClusterAddress cluster;

    for( SAPDB_UInt clusterID = Converter_PageCollector::FirstClusterId();
         clusterID <= pageCollection.GetClusterCount();
         clusterID++ )
    {
        pageCollection.GetCluster( clusterID, pageNoList, cluster );
        bd13LoadNodes( trans,
                       m_Node.m_Current.curr_tree_id,
                       pageCollection.GetRecoveryMode(),
                       pageNoList,
                       cluster,
                       m_pIOBuffer->GetBuffer() );
        if( e_ok != m_TrError)
            return;
    }
}

    
/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd500CountClusteredPages (tsp00_Int4            &  LeafPageCount,
                                       tsp00_Int4            &  ClusteredBlockCount)
{
    ROUTINE_DBG_MEO00 ("bd500CountClusteredPages");
    
    Trans_Context           &trans = Trans_Context::GetContext(*m_Node.m_Current.curr_trans);
    Converter_PageCollector  pageCollection (trans.Allocator(), Data_PageRecoveryMode());
    Kernel_VTrace            trace;

    if( ! pageCollection.Initialize() )
        return;

    Data_PageNoList pageNoList( trans.Allocator());

    if( ! pageNoList.Reserve(pageCollection.GetMaxClusterSize()) )
        return;

    bd500SetToNodeOnEdge (true, LEAF_LEVEL_BD00+1);
    
    while ( e_ok == m_TrError
            &&
            m_Node.bd600IsAccessable() )
    {
        m_Node.bd600CountClusteredPages (*this, pageCollection, pageNoList, LeafPageCount, ClusteredBlockCount);

        if ( e_ok == m_TrError )
        {
            if ( m_Node.bd600RightNeighbor() == NIL_PAGE_NO_GG00 )
                m_Node.bd600Release(false);
            else
                bd500NextNode();
        }
    }
    
    if ( DataIndex_Trace.TracesLevel(6) )
    {
        trace << "bd500CountClusteredPages"
              << ", ClusteredBlockCount: " << ClusteredBlockCount
              << ", LeafPageCount: "       << LeafPageCount
              << FlushLine;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd500_SubTree::bd500_CheckStopKeyAndInterrupt (bool bAscending)
{
    if ( m_Node.bd600IsLeaf() )
    {
        if ( bd500_BeyondStopKey(bAscending) )
        {
#           if COMPILEMODE_MEO00 >= SLOW_MEO00 
            t01int4 (bd_index, "BeyondStp ID", m_Node.m_pNode->nd_id());
#           endif
            m_Node.bd600Release (IS_CHANGED_BD600);
        }
        else
        {
            if( ! m_Interruptible ) // PTS 1129329 TS 2004-04-26
                return;
            
            if ( NULL == m_pTreeIsRequested )
                return;

            /* check if another task wants to access the tree */
            *m_pTreeIsRequested =
                ((nr_for_read == m_Node.m_Current.bd300RootRequestKind())                       &&
                (bd20IsPageRequested (m_Node.m_Current.bd300RootPtrs().np_cbptr())              ||
                bd20IsPageRequested (m_MotherTree.m_Node.m_Current.bd300RootPtrs().np_cbptr()) ||
                m_MotherTree.m_Node.bd600NodeIsRequested()));
            
            /* check if this order was canceled */
            if ( m_Node.m_Current.curr_trans->trRteCommPtr_gg00->to_cancel             && 
                ! m_Node.m_Current.curr_trans->trState_gg00.includes(tsRollback_egg00))
            {
                m_TrError = e_cancelled;
                g01opmsg (sp3p_knldiag, sp3m_info, csp3_bd_msg, csp3_n_btree,
                    "bd500_Inter: canceled   " , m_Node.m_Current.curr_trans->trTaskId_gg00);
            }
        }
    }
}

/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd500.h (CODE)                        * 
 *===========================================================================*/

inline void
bd500_WriteTrace (tgg00_TransContext &Trans,
                  const char         *Line,
                  tsp00_Int4          LineLen)
{
    Trace_CommonEntry entry( tgg00_Debug::fromConst(ta), tgg00_VtraceType::fromConst(td_none), 
                reinterpret_cast< const tsp00_Byte*> ( Line ), LineLen );
}

/*---------------------------------------------------------------------------*/

void
cbd501_IndexOrders::bd501Trace (tgg00_TransContext &Trans)
{
    const tsp00_Int4   LineSize = sizeof(tsp00_Line)+1;
    const char        *OrderKindChar = "NCAD";
    char               Line [LineSize ];
    char              *AuxLine = Line;

    sp77sprintf (Line, LineSize, "IndexOrders [%d]", iosOrderCount_bd501);
    bd500_WriteTrace (Trans, Line, (tsp00_Int4)strlen(Line));
    bd500_WriteTrace (Trans, "------------------------------", 30);
    for (tsp00_Int4 OrderNo = 0; OrderNo < iosOrderCount_bd501; ++OrderNo)
    {
        /* PTS 1108839 UH 2000-12-27 */
        cbd501_IndexOrder &CurrOrder = iosOrders_bd501[OrderNo];
        sp77sprintf (AuxLine, LineSize, "%c%d#%d%s,",
            OrderKindChar[CurrOrder.ioKind_bd501],
            CurrOrder.ioSubNodeId_bd501(),
            CurrOrder.ioLeafCount_bd501(),
            CurrOrder.ioKeyIsInvalid()?(""):("K"));
        AuxLine = Line + strlen(Line);
        // CurrOrder->ioSepKey_bd501(), CurrOrder->ioSepKeyLen_bd501
    }
    bd500_WriteTrace (Trans, Line, (tsp00_Int4)strlen(Line));
    bd500_WriteTrace (Trans, "------------------------------", 30);
}

