/*!
  @file           gbd500.h
  @ingroup        Tree Handling
  @author         UweH
  @brief          This module offers classes to handle B*-Trees.

                Implementation modules are:
                vbd500.cpp - operations for read access and navigation.
                vbd510.cpp - operations for restructuring the index-levels.
                vbd520.cpp - operations for doing overflow or underflow between nodes.
                vbd530.cpp - convienence functions to add/del/upd space for records on leaf level.

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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
#ifndef GBD500_H
#define GBD500_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd300.h"
#include "gbd600.h"
#include "gbd601.h"
#include "gbd520.h"
#include "hgg01_3.h"  // g01glob

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

#define FOR_INSERT_GBD500         true
#define ASCENDING_GBD500          true
#define MAX_LEAFCOUNT_DIFF_GBD500 20

class cbd500_SubTree;
class cbd510_RecordIterator;
class cbd501_IndexOrder;
class cbd501_IndexOrders;
class Data_IOBuffer;

struct tbd500_PathInfoElem
{
    tsp00_PageNo piLeftId_bd500;
    tsp00_PageNo piRightId_bd500;
    tsp00_PageNo piId_bd500;
    tsp00_PageNo piLeafCount_bd500;
    tsp00_Int4   piSubNodeRecIndex_bd500;
    tsp00_Int4   piMaxRecIndex_bd500;
};

/*---------------------------------------------------------------------------*/

typedef tbd500_PathInfoElem tbd500_PathInfo [MAX_LEVEL_BD00+1];

/*---------------------------------------------------------------------------*/
/*!
@class          cbd500_Tree
@brief          This class offers methods to handle B*-Trees.

                Navigating through, finding special nodes,
                restructure the index-levels of a B*-tree, if leaves are inserted or removes.
 */
class cbd500_Tree
{
    friend class cbd460_InvListSubtreeIteratorNoBreak;
    friend class cbd460_InvListSubtreeIterator;
    friend class cbd500_SubTree;
public:

    /*!
       @brief          This is the constructor, which is most often used.
       @param          StartNode [in/out] This node is navigated through the Tree.
       @param          bInterruptible [in] This must be set, if the operation on the tree should be 
                                     implicit interruptible by a concurrent running updater.
       @param          bIsClustered [in] if this is true all read-right operations read all leaves accessable by the current index page.                                     
       @param          StopKey [in] This must be given, if an abort criteria is given.
       @param          StopKeyLen [in] The length of StopKey must be given, if a StopKey is given

     */
    cbd500_Tree (cbd600_Node    &StartNode, // PTS 1121589 UH 2003-04-11 reEntrantKey removed
                 bool           bInterruptible    = false, // PTS 1121770 TS 2003-04-25
                 Data_IOBuffer *ioBuffer = 0,
                 tsp00_KeyPtr   pStopKey          = NULL,
                 tsp00_Int4     StopKeyLen        = 0);
    /*!
       @brief          This constructor, creates a copy of SrcTree.
       @param          SrcTree [in] Used as basis for the current tree.
       @param          StartNode [in/out] This node is navigated through the Tree.
       @param          StartId [in] This must be given, if an abort criteria is given.

     */
    cbd500_Tree (const cbd500_Tree   &SrcTree,
                       cbd600_Node   &StartNode,
                       tsp00_PageNo   StartId);
    /*!
       @brief          Current scan over nodes is interrupted.
       @param          recIndex [in] current record position
       @param          rePositionKey [out] current key at recIndex position
       @param          rePositionKeyLen [out] current key length


                    The first key in orientation of the current node is copied into rePositionKey.
                    There after all nodes of the tree are released.
     */
    void         bd500Break ( const tsp00_Int4    recIndex,
                              const tsp00_KeyPtr  rePositionKey,       // PTS 1121589 UH 2003-04-11 added
                              tsp00_Int2          &rePositionKeyLen ); // PTS 1121589 UH 2003-04-11 added
    // void         bd500Continue         (); // PTS 1121589 UH 2003-04-11 removed
    /*!
       @brief          Continue after bd500Break().
       @param          rePositionKey [in] 
       @param          rePositionKeyLen [in] 

       The Tree is locked again and the LeafNode which is addressed by rePositionKey is searched.
     */
    void         bd500Continue         (const tsp00_KeyPtr rePositionKey,
                                        const tsp00_Int2   rePositionKeyLen);
    /*!
       @brief          Current Tree is a copy of SrcTree.
       @param          SrcTree [in] 
     */
    void         bd500Copy             (const cbd500_Tree  &SrcTree);
    /*!
       @brief          This gives access to the internal used node.
       @returns        A reference to the internal node.
     */
    cbd600_Node &bd500CurrentNode      () const;
    /*!
       @brief          Determine the correct Separator in an index node with a given Key.
       @param          Node [in] 
       @param          SepKey [in] 
       @param          SepRecIndex [out] 
     */
    static void  bd500FindBranch       (cbd600_Node  &Node,
                                        tsp00_KeyPtr  pSepKey,
                                        tsp00_Int4    SepKeyLen,
                                        tsp00_Int4   &SepRecIndex);
    /*!
       @brief          This gives access to the Current.
       @returns        A reference to the the Current.
     */
    cbd300_InvCurrentBasis & bd500GetTheCurrent () const;
    /*!
       @brief          The internal node is set to the right neighbor node.

                    The PathInfo of current level is updated and of the above levels, if possible.
                    If right neighbor is nil and current node is an index-node and
                    LeftMostIdNextLowerLevel is not nil, then the next node is set to this node.
                    If the stop condition (StopKey) is reached, then the internal node is released
                    and no further Next-Operation is possible.
     */

    void         bd500NextNode         ();
    /*!
       @brief          The internal node is set to the left neighbor node.

                    The PathInfo is updated. If left neighbor is nil, no further Prev-Operation is possible.
                    If the stop condition (StopKey) is reached, then the internal node is released
                    and no further Prev-Operation is possible.
     */
    void         bd500PrevNode         ();
    /*!
       @brief          The internal node is set to a special node.
       @param          Key [in] 
       @param          Level [in] 

                    The node is found with the specified key and the given level.
                    The Search begins always with the root
                    The internal Node (m_Node.m_RequestKind) tells how Leafs are locked.
                    The Tree must be locked correctly.
     */
    void         bd500FindNode         (tsp00_KeyPtr  pKey, tsp00_Int4 KeyLen, tsp00_Int4 Level);
    /*!
       @brief          The difference of LeafCounts between a LeafNode and its IndexNode is calculated.
       @returns        Number of Leaves in the tree.
     */
    tsp00_Int4   bd500LeafCount() const;
    /*!
       @brief          Determines the left neighbor.
       @returns        page_no of left neighbor node.

                    This can be an expensive operation,
                    if the left neighbor node is not already known,
                    because of a previous operation.
                    In worst case, a CopyTree is created and there the PrevNode is searched.
     */
    tsp00_PageNo bd500LeftNeighbor     ();
    /*!
       @brief          Determines the left neighbor without changeing the current tree.
       @returns        pageno of left neighbor node
     */
    tsp00_PageNo bd500LeftNeighborId (cbd600_Node &auxNode) const;
    /*!
       @brief          The PathInfo is written into the KernelTrace.
       @param          Title [in] 
     */
    void         bd500Print            (const char* Title = NULL) const;
    /*!
       @brief          The difference of LeafCounts between a LeafNode and its IndexNode is calculated.
       @returns        true, if difference exceeds MAX_LEAFCOUNT_DIFF_GBD500
     */
    bool         bd500PropagationNeeded() const;
    /*!
       @brief          This releases the internal node and unlocks the Tree.
       @param          bCurrNodeIsChanged [in] 
     */
    void         bd500ReleaseTree      (bool bCurrNodeIsChanged = true) const; 
    /// The internal node is set to the specified index or leaf node.
    void         bd500SetToNodeOnEdge (bool leftEdge, tsp00_Int2 level);
    /// The internal node is set to the leftmost LeafNode.
    void         bd500SetToFirstLeaf   ();
    /// The internal node is set to the rightmost LeafNode.
    void         bd500SetToLastLeaf    ();
    /// The internal node is set to the LeftMostPageOnLowerLevel.
    void         bd500SetToLeftMostPageOnSameLevel();
    /// The internal node is set to the root node.
    /// The PathInfo and the other MemberVariables are initialized.
    void         bd500SetToRoot        (bool bCheckLocks = true);
    /// returns true if this tree should be written in clustered format
    /// for this feature  the nd_left id in the nodes must be set
    bool         bd500IsClustered() const;
    /// determines all leave pages and how many of them are clustered
    void         bd500CountClusteredPages (tsp00_Int4            &  LeafPageCount,
                                           tsp00_Int4            &  ClusteredBlockCount);
    /*!
       @brief          This creates space for a new or existing record.
       @return         bool - true, if anything was done
       @param          Key [in] of the new or updated record
       @param          InsNewRecord [in] wether the record is new or updated
       @param          RequiredLen [in] the total(new) or additional(update) length
       @param          RecIndex [in/out] 

        Records might be moved to right or left neighbor nodes. New nodes
        are inserted if necessary.
        In addition the reorganization of the above IndexNodes is triggered.
        POST: The internal node includes the new ore updated record-space
              and RecIndex points to this place.
              The PathInfo is invalid.
     */
    bool         bd520LeafOverflow     (tsp00_KeyPtr  pKey,
                                        tsp00_Int4    KeyLen,
                                        bool          bInsNewRecord,
                                        tsp00_Int4    RequiredLen,
                                        tsp00_Int4   &RecIndex);
    /* PTS 1106992 UH 2000-06-21 */
    /*!
       @brief          This propagates the LeafCount from the LeafNode to the root.
       @param          Key [in] specifies the Leaf

       The LeafNode is specified by the Key.
     */
    void         bd520LeafCountPropagation (tsp00_KeyPtr  pKey   = NULL,
                                            tsp00_Int4    KeyLen = 0);
    /*!
       @brief          This removes record space from a record, which is deleted.
       @return         bool - true, if anything was done
       @param          Key [in] specifies the Leaf

        The recordspace is specified by the Key. After the space is removed
        from the node it is calculated, if nodes might be deleted. Therefore
        Records are moved. The IndexNodes are restructured.
        POST: The PathInfo is invalid.
     */
    /* PTS 1106992 UH 2000-06-21 */
    bool         bd520LeafUnderFlow    (tsp00_KeyPtr  pKey,
                                        tsp00_Int4    KeyLen);
    /*!
       @brief          This adds record space at a specified place.
       @param          Key [in] specifies the Leaf
       @param          RequiredLen [in] the record length
       @param          pRec [out] points to a byte-string for the new record

       POST: The PathInfo might be invalid.
     */
    void         bd530AddRecordSpace    (tsp00_KeyPtr    pKey,
                                         tsp00_Int4      KeyLen,
                                         tsp00_Int4      RequiredRecLen,
                                         tgg00_RecPtr   &pRec);
    /*!
       @brief          This appends record space after the greatest record in the tree.
       @param          Key [in] specifies the record
       @param          RequiredLen [in] the record length
       @param          pSpace [out] points to a byte-string for the new record

       POST: The PathInfo might be invalid.
     */
    void         bd530AppendRecordSpace (tsp00_KeyPtr    pKey,
                                         tsp00_Int4      KeyLen,
                                         tsp00_Int4      RequiredRecLen,
                                         tsp00_BytePtr  &pSpace);
    /*!
       @brief          This updates record space in the tree.
       @param          Key [in] specifies the record
       @param          RequiredLen [in] the additional record length
       @param          pRec [out] points to a byte-string for the updated record

       POST: The PathInfo might be invalid.
     */
    void         bd530UpdRecordSpace    (tsp00_KeyPtr    pKey,
                                         tsp00_Int4      KeyLen,
                                         tsp00_Int4      RecSizeDelta,
                                         tgg00_RecPtr   &pRec);
    /*!
       @brief          Deletes a record, removes the space and can do an underflow.
       @param          Key [in] specifies the record

       POST: The PathInfo might be invalid.
     */
    void         bd530DelRecord         (tsp00_KeyPtr    pKey,
                                         tsp00_Int4      KeyLen);
    /*!
       @brief          points to a specified record.
       @param          Key [in] specifies the record
       @param          pRec [out] points to a byte-string for the updated record
     */
    void         bd530GetRecord         (tsp00_KeyPtr    pKey,
                                         tsp00_Int4      KeyLen,
                                         tgg00_RecPtr   &pRec);
    // made public to print
    void bd500_WriteTrace                 (const char      *Line,
                                           tsp00_Int4       LineLen) const;
protected:
    cbd600_Node      &m_Node;
    tgg00_BasisError &m_TrError;
    tsp00_PageNo      m_LeftMostIdNextLowerLevel;
    // tsp00_KeyPtr      m_pRePositionKey;    // PTS 1121589 UH 2003-04-11 removed
    // tsp00_Int2       *m_pRePositionKeyLen; // PTS 1121589 UH 2003-04-11 removed
    tsp00_KeyPtr      m_pStopKey;
    tsp00_Int4        m_StopKeyLen;
    tsp00_Int4        m_RootLevel;
    tsp00_Int4        m_CurrLevel;
    tbd500_PathInfo   m_PathInfo;

    const bool        m_Interruptible; // PTS 1121770 TS 2003-04-25
    Data_IOBuffer    *m_pIOBuffer;     // this is not 0 if clustered read is enabled
    bool              m_ReadClustered; // next time on level1 a cluster should be read


    /// The current node is an index node. Beginning with given RecIndex all referenced leafpages are read in a cluster
    void bd500_ReadClustered(tsp00_Int4 RecIndex);

    bool bd500_BeyondStopKey            (bool          bAscending) const;
    virtual void bd500_CheckStopKeyAndInterrupt (bool bAscending);
    void bd500_FindBranch               (tsp00_KeyPtr  pSepKey,
                                         tsp00_Int4    SepKeyLen,
                                         tsp00_Int4   &SepRecIndex) const;
    void bd500_ResetForNewSearch                     ();
    void bd500_InvalidIndexStructure    (tsp00_Int4          MsgNo,
                                         const char*       MsgText,
                                         tsp00_Int4          BadValue);
    void bd500_NextSubNode              (tsp00_Int4       RecIndex,
                                         tsp00_Int4        TargetLevel,
                                         tbd_node_request NodeRequest);
    void bd500_PrevIndexNode              ();
    
    void bd510_ExecuteAddOrder          (cbd501_IndexOrder  &Order,
                                         bool                bLastOrderForCurrentNode,
                                         bool                bFirstKeyIsChanged,       // PTS 1103693 AK 19-08-1999 
                                         cbd520_Separator   &Separator,
                                         tsp00_Int4         &SepRecIndex,
                                         cbd600_Node        &Neighbor, 
                                         cbd600_Node        &NextNeighbor, 
                                         cbd501_IndexOrders &NewOrders,
                                         tsp00_Int4         &NumNodeDiff);
    void bd510_ExecuteDeleteOrder       (cbd501_IndexOrder  &Order,
                                         bool                bLastOrderForCurrentNode,
                                         bool                bFirstKeyIsChanged,       // PTS 1103693 AK 19-08-1999 
                                         bool                bUnderflowAllowed,        // PTS 1109619 UH 2001-03-13
                                         cbd520_Separator   &Separator,
                                         tsp00_Int4         &SepRecIndex,
                                         cbd600_Node        &Neighbor, 
                                         cbd600_Node        &AuxNode,                  // PTS 1109619 UH 2001-03-13
                                         cbd501_IndexOrders &NewOrders,
                                         tsp00_Int4         &NumNodeDiff);
    void bd510_ExecuteChangeOrder       (cbd501_IndexOrder  &Order,
                                         bool                bLastOrderForCurrentNode,
                                         bool                bFirstKeyIsChanged,       // PTS 1103693 AK 19-08-1999 
                                         bool                bUnderflowAllowed,        // PTS 1109619 UH 2001-03-13
                                         cbd520_Separator   &Separator,
                                         tsp00_Int4         &SepRecIndex,
                                         cbd600_Node        &Neighbor, 
                                         cbd600_Node        &NextNeighbor, 
                                         cbd501_IndexOrders &NewOrders,
                                         tsp00_Int4         &NumNodeDiff);
    void bd510_ExcuteOrdersOnOneLevel   (cbd501_IndexOrders &Orders,
                                         tsp00_Int4         &NumNodeDiff);
    void bd510_PrepareOrderExecutionOnNextHigherLevel(cbd501_IndexOrders &Orders); // PTS 1112713 UH 2001-12-07
    void bd510_InsertNewLevel           (cbd600_Node        &RightNeighbor,
                                         tsp00_KeyPtr        pSepKey,
                                         tsp00_Int4          SepKeyLen,
                                         tsp00_Int4          RecIndex,
                                         bool                bRecordIsInRightNode,
                                         cbd600_Node        &NewLeftSubNode,
                                         tsp00_Int4         &NumAddedNodes);
    void bd500_SetLeftMostIdForLowerLevel (tbd_nodeptr pNode);
    bool bd510_TryToRemoveLevel         (cbd600_Node        &Neighbor);

    void bd520_Overflow                 (cbd600_Node         &Neighbor,
                                         cbd600_Node         &NextNeighbor,
                                         tsp00_KeyPtr         pKey,
                                         tsp00_Int4           KeyLen,
                                         tsp00_Int4           LeafCount,
                                         bool                 bInsNewRecord,
                                         bool                 bFirstRecUpdated,
                                         tsp00_Int4           RequiredLen,
                                         tsp00_Int4          &RecIndex,
                                         tsp00_Int4          &NumNodeDiff,
                                         cbd501_IndexOrders  &NewOrders);
    void bd520_DistributeLeft           (cbd600_Node         &Neighbor,
                                         tgg00_MessType_Enum  DistribReason,
                                         tsp00_Int4           RequiredLen,
                                         tsp00_KeyPtr         pKey,
                                         tsp00_Int4           KeyLen,
                                         tsp00_Int4           LeafCount,
                                         tsp00_Int4          &RecIndex,
                                         cbd501_IndexOrders  &NewOrders);
    void bd520_DistributeRight          (cbd600_Node         &Neighbor,
                                         cbd600_Node         &NextNeighbor,
                                         bool                 bNewNodeInserted,
                                         bool                 bFirstRecUpdated,
                                         tgg00_MessType_Enum  DistribReason,
                                         tsp00_Int4           RequiredLen,
                                         tsp00_KeyPtr         pKey,
                                         tsp00_Int4           KeyLen,
                                         tsp00_Int4           LeafCount,
                                         tsp00_Int4          &RecIndex,
                                         tsp00_Int4          &NumAddedNodes,
                                         cbd501_IndexOrders  &NewOrders);
    void bd520_ThreeNodeDistribution    (cbd600_Node         &Neighbor,
                                         cbd600_Node         &NextNeighbor,
                                         tgg00_MessType_Enum  DistribReason,
                                         bool                 bNewNodeInserted,
                                         bool                 bFirstRecUpdated,
                                         tsp00_KeyPtr         pKey,
                                         tsp00_Int4           KeyLen,
                                         tsp00_Int4           LeafCount,
                                         tsp00_Int4          &RecIndex,
                                         cbd501_IndexOrders  &NewOrders);
    void bd520_UnderFlow                (cbd600_Node         &Neighbor,
                                         cbd600_Node         &AuxNode,                 // PTS 1109619 UH 2001-03-13
                                         bool                 bFirstRecUpdated,
                                         tsp00_Int4          &NumNodeDiff,
                                         cbd501_IndexOrders  &NewOrders);
    void bd520_DistributeFromLeft       (cbd600_Node         &Neighbor,
                                         tsp00_Int4           Covering,
                                         tsp00_Int4           NeighborCovering,
                                         cbd501_IndexOrders  &NewOrders);
    void bd520_DistributeFromRight      (cbd600_Node         &Neighbor,
                                         tsp00_Int4           Covering,
                                         tsp00_Int4           NeighborCovering,
                                         bool                 bFirstRecUpdated,
                                         cbd501_IndexOrders  &NewOrders);
    void bd520_Propagate                (bool                 bWithKey,
                                         cbd501_IndexOrders  &NewOrders);
};

/*---------------------------------------------------------------------------*/
/*!
  @class          cbd500_SubTree
 */
class cbd500_SubTree : public cbd500_Tree
{   
    friend class cbd460_InvListSubtreeIterator;
    friend class cbd460_InvListSubtreeIteratorB;
    friend class cbd460_InvListIterator;

private:
    cbd500_Tree  &m_MotherTree;
    bool         *m_pTreeIsRequested;
    virtual void bd500_CheckStopKeyAndInterrupt (bool bAscending);
    
public:
    /// standard constructor
    cbd500_SubTree (cbd500_Tree        &MotherTree, 
                    cbd601_SubTreeNode &StartNode, 
                    bool               *pTreeIsRequested);
};


/*---------------------------------------------------------------------------*/
inline
void cbd500_Tree::bd500_FindBranch (tsp00_KeyPtr  pSepKey,
                                    tsp00_Int4    SepKeyLen,
                                    tsp00_Int4   &SepRecIndex) const
{
    bd500FindBranch (m_Node, pSepKey, SepKeyLen, SepRecIndex);
}

/*---------------------------------------------------------------------------*/

inline void
cbd500_Tree::bd500_SetLeftMostIdForLowerLevel (tbd_nodeptr pNode)
{

    if ( m_CurrLevel == LEAF_LEVEL_BD00 )
        return;
        
    cbd520_Separator Separator;

    Separator.Assign(cbd600_Node::bd600FirstRecPtr(pNode));

    if ( 0 == Separator.bd520KeyLen() )
        m_LeftMostIdNextLowerLevel = Separator.bd520SubNodeId();
}

/*---------------------------------------------------------------------------*/
inline void
cbd500_Tree::bd500_ResetForNewSearch ()
{

    m_LeftMostIdNextLowerLevel = NIL_PAGE_NO_GG00;
    m_RootLevel                = m_Node.m_Current.bd300GetRootLevel();
    m_CurrLevel                = m_Node.bd600IsAccessable() ? m_Node.bd600Level() : m_RootLevel;
    bd500_SetLeftMostIdForLowerLevel (m_Node.m_Current.bd300RootPtrs().np_ptr());

    for (tsp00_Int4 Level = LEAF_LEVEL_BD00; Level <= m_RootLevel; ++Level)
    {
        m_PathInfo [Level].piLeftId_bd500          = NIL_PAGE_NO_GG00;
        m_PathInfo [Level].piRightId_bd500         = NIL_PAGE_NO_GG00;
        m_PathInfo [Level].piId_bd500              = NIL_PAGE_NO_GG00;
        m_PathInfo [Level].piLeafCount_bd500       = -1;
        m_PathInfo [Level].piSubNodeRecIndex_bd500 = NIL_RECINDEX_BD00;
        m_PathInfo [Level].piMaxRecIndex_bd500     = NIL_RECINDEX_BD00;
    }
}
/*---------------------------------------------------------------------------*/
inline void
cbd500_Tree::bd500SetToRoot (bool bCheckLocks)
{
    ROUTINE_DBG_MEO00 ("bd500SetToRoot");

    bd500_ResetForNewSearch(); 

    if ( bCheckLocks )
    {
        if ( nr_for_update   == m_Node.m_RequestKind
             &&
             LEAF_LEVEL_BD00 == m_Node.m_Current.bd300RootPtrs().np_ptr()->nd_level()
             &&
             nr_for_read     == m_Node.m_Current.bd300RootRequestKind() )
        {
            m_Node.m_Current.bd300LockTreeExclusive();
            if ( e_ok != m_TrError ){
                return;
            }
            bd500_ResetForNewSearch();
        }
    }

    if ( m_Node.bd600IsAccessable() )
        m_Node.bd600Release ( nr_for_update == m_Node.m_RequestKind );

    m_Node.bd600GetNode(m_Node.m_Current.bd300RootPtrs().np_ptr()->nd_id(),
                        m_Node.m_Current.bd300RootRequestKind());
}
/*---------------------------------------------------------------------------*/
inline bool
cbd500_Tree::bd500PropagationNeeded() const
{
    ROUTINE_DBG_MEO00 ("bd500PropagationNeeded");

    bool Result;

    if ( m_RootLevel == LEAF_LEVEL_BD00 || ! m_Node.bd600IsAccessable() || ! m_Node.bd600IsLeaf() )
    {
        /* PTS 1104578 UH 10-11-1999 */
        Result = false;
    }
    else
    {
        tsp00_Int4 IndexLeafCount = m_PathInfo[LEAF_LEVEL_BD00+1].piLeafCount_bd500;
        tsp00_Int4 LeafCount      = m_Node.bd600LeafCount();

#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        t01p2int4 (bd_inv, "IdxLeafCount", IndexLeafCount, "LeaveCount  ", LeafCount);
#       endif

        if ( LeafCount > IndexLeafCount )
            Result = LeafCount - IndexLeafCount > 3
                     &&
                     ((100*(LeafCount-IndexLeafCount))/LeafCount) > MAX_LEAFCOUNT_DIFF_GBD500;
        else
            Result = IndexLeafCount - LeafCount > 3
                     &&
                     ((100*(IndexLeafCount-LeafCount))/IndexLeafCount) > MAX_LEAFCOUNT_DIFF_GBD500;
    }


#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01bool (bd_inv, "Result      ", Result);
#   endif

    return Result;
}
/*---------------------------------------------------------------------------*/
inline void
cbd500_Tree::bd500Copy (const cbd500_Tree &SrcTree)
{
    ROUTINE_DBG_MEO00 ("bd500Copy");

    m_LeftMostIdNextLowerLevel = SrcTree.m_LeftMostIdNextLowerLevel;
    m_pStopKey                 = SrcTree.m_pStopKey;
    m_StopKeyLen               = SrcTree.m_StopKeyLen;
    m_RootLevel                = SrcTree.m_RootLevel;
    m_CurrLevel                = SrcTree.m_CurrLevel;
    m_ReadClustered            = SrcTree.m_ReadClustered;
    m_pIOBuffer                = SrcTree.m_pIOBuffer;
    for (tsp00_Int4 Level = LEAF_LEVEL_BD00; Level < m_RootLevel+1; ++Level)
    {
        m_PathInfo[Level].piLeftId_bd500          = SrcTree.m_PathInfo[Level].piLeftId_bd500;
        m_PathInfo[Level].piRightId_bd500         = SrcTree.m_PathInfo[Level].piRightId_bd500;
        m_PathInfo[Level].piId_bd500              = SrcTree.m_PathInfo[Level].piId_bd500;
        m_PathInfo[Level].piLeafCount_bd500       = SrcTree.m_PathInfo[Level].piLeafCount_bd500;
        m_PathInfo[Level].piSubNodeRecIndex_bd500 = SrcTree.m_PathInfo[Level].piSubNodeRecIndex_bd500;
        m_PathInfo[Level].piMaxRecIndex_bd500     = SrcTree.m_PathInfo[Level].piMaxRecIndex_bd500;
    }
}
/*---------------------------------------------------------------------------*/
inline
cbd500_Tree::cbd500_Tree (cbd600_Node    &StartNode,
                          bool           bInterruptible, // PTS 1121770 TS 2003-04-25
                          Data_IOBuffer *ioBuffer,
                          tsp00_KeyPtr   pStopKey,
                          tsp00_Int4     StopKeyLen)
: m_Node    (StartNode),
  m_TrError (m_Node.m_TrError),
  m_Interruptible (bInterruptible), // PTS 1121770 TS 2003-04-25
  m_pIOBuffer     (ioBuffer),
  m_ReadClustered (ioBuffer!=0)
{
    ROUTINE_DBG_MEO00 ("cbd500_Tree");

    if (e_ok != m_TrError) return; // PTS 1104721 TS 1999-11-23

    tbd_nodeptr &pRootNode = m_Node.m_Current.bd300RootPtrs().np_ptr();

    if ( pRootNode != NULL )
        bd500_ResetForNewSearch ();
    m_pStopKey          = pStopKey;
    m_StopKeyLen        = StopKeyLen;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    if ( m_Node.bd600IsAccessable() )
        t01int4 (bd_inv, "CurrentNode ", m_Node.bd600GetId());
    if (NULL != pStopKey)
    {
        t01sname   (bd_inv, "StopKey     ");
        t01moveobj (bd_inv, m_pStopKey, POS_OFF_DIFF_BD00, m_StopKeyLen);
    }
    if ( pRootNode != NULL )
        t01p2int4 (bd_index, "RootNode    ", pRootNode->nd_id(),
                             "LeafCount   ", pRootNode->ndLeafCount_bd00());
#   endif
}
/*---------------------------------------------------------------------------*/
inline
cbd500_Tree::cbd500_Tree (const cbd500_Tree   &SrcTree,
                                cbd600_Node   &StartNode,
                                tsp00_PageNo   StartId)
: m_Node          (StartNode),
  m_TrError       (m_Node.m_TrError),
  m_Interruptible (SrcTree.m_Interruptible) // PTS 1121770 TS 2003-04-25
{
    ROUTINE_DBG_MEO00 ("cbd500_Tree2");

    if (e_ok != m_TrError) return; // PTS 1104721 TS 1999-11-23

    bd500Copy (SrcTree);
    m_Node.bd600Move (StartId);
    if ( e_ok != m_TrError ) return;
    m_CurrLevel = m_Node.m_pNode->nd_level();
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01p2int4 (bd_index, "RootNode    ", m_Node.m_Current.bd300RootPtrs().np_ptr()->nd_id(),
                         "LeafCount   ", m_Node.m_Current.bd300RootPtrs().np_ptr()->ndLeafCount_bd00());
#   endif
}
/*---------------------------------------------------------------------------*/
inline void
cbd500_Tree::bd500SetToLeftMostPageOnSameLevel()
{
    ROUTINE_DBG_MEO00 ("bd500SetToLeftMostPageOnSameLevel");

    if ( NIL_PAGE_NO_GG00 == m_LeftMostIdNextLowerLevel )
        m_Node.bd600Release ( nr_for_update == m_Node.m_RequestKind );

    const tsp00_PageNo leftMostPageNo = m_LeftMostIdNextLowerLevel;
    
    bd500_ResetForNewSearch();
    
    if ( leftMostPageNo != NIL_PAGE_NO_GG00 )
    {
        m_Node.bd600GetNode (leftMostPageNo);
        if ( m_TrError != e_ok || ! m_Node.bd600IsAccessable() )
            return;
        
        m_LeftMostIdNextLowerLevel                       = leftMostPageNo;
        m_CurrLevel                                      = m_Node.bd600Level();
        m_PathInfo [m_CurrLevel].piLeftId_bd500          = NIL_PAGE_NO_GG00;
        m_PathInfo [m_CurrLevel].piRightId_bd500         = m_Node.bd600RightNeighbor();
        m_PathInfo [m_CurrLevel].piId_bd500              = m_Node.bd600GetId();
        m_PathInfo [m_CurrLevel].piLeafCount_bd500       = m_Node.bd600LeafCount();
        m_PathInfo [m_CurrLevel].piSubNodeRecIndex_bd500 = FIRST_REC_INDEX_BD00;
        m_PathInfo [m_CurrLevel].piMaxRecIndex_bd500     = m_Node.bd600MaxRecIndex();
    }
}
/*---------------------------------------------------------------------------*/
inline cbd300_InvCurrentBasis &
cbd500_Tree::bd500GetTheCurrent () const
{
    ROUTINE_DBG_MEO00 ("bd500GetTheCurrent");
    return m_Node.m_Current;
}
/*---------------------------------------------------------------------------*/
inline void
cbd500_Tree::bd500_PrevIndexNode ()
{
    ROUTINE_DBG_MEO00 ("bd500_PrevIndexNode");

    ++m_CurrLevel; 
    m_Node.bd600Move (m_PathInfo[m_CurrLevel].piId_bd500);
    if ( e_ok != m_TrError ) return;
    if ( m_CurrLevel != m_Node.m_pNode->nd_level() )
        bd500_InvalidIndexStructure (csp3_bd_msg, "Invalid Index (prev)    ", m_Node.bd600GetId());

}
/*---------------------------------------------------------------------------*/
inline  void         
cbd500_Tree::bd500ReleaseTree (bool bCurrNodeIsChanged) const
{
    ROUTINE_DBG_MEO00 ("bd500ReleaseTree");
    
    m_Node.bd600Release(bCurrNodeIsChanged);
    m_Node.m_Current.bd300UnlockTree();
}
/*---------------------------------------------------------------------------*/
inline void
cbd500_Tree::bd500SetToFirstLeaf ()
{
    bd500SetToNodeOnEdge (true, LEAF_LEVEL_BD00);
}
/*---------------------------------------------------------------------------*/
inline void
cbd500_Tree::bd500SetToLastLeaf ()
{
    bd500SetToNodeOnEdge (false, LEAF_LEVEL_BD00);
}
/*---------------------------------------------------------------------------*/
inline cbd600_Node &
cbd500_Tree::bd500CurrentNode () const
{
    ROUTINE_DBG_MEO00 ("bd500CurrentNode");
    return m_Node;
}
/*---------------------------------------------------------------------------*/
inline tsp00_Int4
cbd500_Tree::bd500LeafCount() const
{
    return m_Node.m_Current.bd300GetRootLeafCount();
}
/*---------------------------------------------------------------------------*/
inline bool
cbd500_Tree::bd500IsClustered() const
{
    return m_Node.m_Current.currExtendedFileTypeSet_bd00.includes(eftsClustered_egg00);
}

/*---------------------------------------------------------------------------*/
inline
cbd500_SubTree::cbd500_SubTree (cbd500_Tree        &MotherTree,
                                cbd601_SubTreeNode &StartNode,
                                bool               *pTreeIsRequested)
:cbd500_Tree        (StartNode,
                     MotherTree.m_Interruptible, // PTS 1129329 TS 2004-04-26
                     MotherTree.m_pIOBuffer),
 m_MotherTree       (MotherTree),
 m_pTreeIsRequested (pTreeIsRequested)
{}
/*---------------------------------------------------------------------------*/
#endif  /* GBD500_H */
