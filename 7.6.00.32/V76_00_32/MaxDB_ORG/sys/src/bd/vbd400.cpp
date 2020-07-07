/*!
  @file           vbd400.cpp
  @author         UweH
  @ingroup        Index
  @brief          Add / Delete

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
#include <memory.h>

#include "gbd600.h" // cbd600_Node
#include "gbd601.h" // cbd601_SubTreeNode
#include "gbd300.h" // cbd300_InvCurrentBasis, cbd300_InvCurrent, cbd300_InvSubTreeCurrent
#include "gbd400.h" // cbd400_InvTree
#include "gbd450.h" // cbd450_InvListArray, ...
#include "gbd500.h" // cbd500_Tree, cbd500_SubTree
#include "gbd510.h"  //   RecordIterator
#include "gbd520.h"  // cbd520_Separator

#include "gsp03_3.h"
#include "ggg01.h"   
#include "hgg01_1.h"   // g01opmsg
#include "hgg01_3.h"   // g01glob,g01vtrace
#include "hbd102.h"  // PTS 1103980 JA 1999-09-15
#include "hbd06.h"
#include "hbd17.h"
#include "hbd400.h"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "Converter/Converter_IVerify.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#endif

#define IS_SUBTREE_BD400   true
/* PTS 1106992 UH 2000-06-21 */
#define ANYTHING_DONE      true
/* PTS 1106992 UH 2000-06-21 */
#define PROPAGATION_NEEDED true

/*---------------------------------------------------------------------------*/
static inline void
bd400_AddPrimKeyToSubTree (cbd300_InvCurrent &InvCurrent,
                           cbd600_Node       &LeafNode,
                           tsp00_KeyPtr       PrimKey,
                           tsp00_Int4         PrimKeyLen,
                           tsp00_Int4         RecIndex);

/*---------------------------------------------------------------------------*/
static inline void
bd400_CreateNewInvList (cbd500_Tree  &InvTree,
                        cbd600_Node  &LeafNode,
                        tsp00_Int4   &RecIndex,
                        tsp00_KeyPtr  pSecKey,
                        tsp00_Int4    SecKeyLen,
                        tsp00_KeyPtr  pPrimKey,
                        tsp00_Int4    PrimKeyLen,
                        bool          bPropagationNeeded, /* PTS 1106992 UH 2000-06-21 */
                        bool          bConsistentReadSupport);

/*---------------------------------------------------------------------------*/
static inline void
bd400_DeleteSubTrees (cbd300_InvCurrent &InvCurrent,
                      cbd600_Node        &InvLeaf);

/*---------------------------------------------------------------------------*/

static inline void
bd400_DelPrimKeyFromSubRoot (cbd300_InvCurrent        &InvCurrent,
                             cbd600_Node              &LeafNode,
                             cbd300_InvSubTreeCurrent &InvSubtreeCurrent,
                             cbd601_SubTreeNode       &SubTreeRoot,
                             tsp00_KeyPtr              PrimKey,
                             tsp00_Int4                PrimKeyLen,
                             tsp00_Int4                SecKeyRecIndex,
                             tbd00_DelInvReason_Param  CallReason);

/*---------------------------------------------------------------------------*/

static inline void
bd400_DelPrimKeyFromSubTree (
    cbd300_InvCurrent       &InvCurrent,
    cbd600_Node             &LeafNode,
    tsp00_KeyPtr             PrimKey,
    tsp00_Int4               PrimKeyLen,
    tsp00_Int4               SecKeyIndex,
    tbd00_DelInvReason_Param CallReason);

/*---------------------------------------------------------------------------*/

static void
bd400_Verify (cbd300_InvCurrentBasis &InvCurrent,
              cbd500_Tree            &Tree1,
              cbd500_Tree            &Tree2,
              bool                    bIsCold,
              bool                    bWithExtendedCheck,
              bool                    bIsSubTree,
              tsp00_Int4             &NumPages,
              tgg00_IndexCounter     &counter);

/*---------------------------------------------------------------------------*/

static inline void
bd400_ConstructSubTreeRecord (tsp00_KeyPtr      pKeySpace,
                              tsp00_KeyPtr      pKey,
                              tsp00_Int4        KeyLen,
                              tgg00_BasisError &TrError);

/*---------------------------------------------------------------------------*/

static inline void
bd400_ConstructSubTreeRecord (bool              ConsistentReadSupport,
                              tgg91_TransNo     &TransNo,
                              tsp00_KeyPtr      pKeySpace,
                              tsp00_KeyPtr      pKey,
                              tsp00_Int4        KeyLen,
                              tgg00_BasisError &TrError);

/*---------------------------------------------------------------------------*/

static inline void
bd400_InvListArrayOverflow (cbd500_Tree        &InvTree,
                            cbd600_Node        &InvLeafNode,
                            cbd500_Tree        &SubTree,
                            cbd601_SubTreeNode &SubTreeLeaf,
                            tsp00_Int4          SecKeyRecIndex,
                            tsp00_KeyPtr        pNewPrimKey,
                            tsp00_Int4          NewPrimKeyLen,
                            tsp00_Int4          PrimKeyIndex,
                            bool                ConsistentReadSupport);

/*---------------------------------------------------------------------------*/

static inline void
bd400_SubRootUnderflow (cbd300_InvCurrent  &InvCurrent,
                        cbd600_Node        &LeafNode,
                        cbd601_SubTreeNode &SubTreeRootNode,
                        tsp00_Int4         &SecKeyRecIndex,
                        tsp00_Int4          PrimKeyRecIndex,
                        tsp00_Int4          RecSizeDelta);

/*---------------------------------------------------------------------------*/

externCpp void
bd400AddToInvTree (cbd300_InvCurrent &InvCurrent,
                   tsp00_KeyPtr       SecKey,
                   tsp00_Int4         SecKeyLen,
                   tsp00_KeyPtr       PrimKey,
                   tsp00_Int4         PrimKeyLen,
                   bool              &SecKeyFound)

{
    ROUTINE_DBG_MEO00 ("bd400AddToInvTree");

    tgg00_BasisError    &TrError                  = InvCurrent.curr_trans->trError_gg00;
    bool                 SubTreeExist             = false;
    bool                 PrimKeyFound             = false;
    bool                 bUpgradeInvListToSubtree = false;
    tbd_searchresult     SecKeySearchResult;
    tbd_searchresult     PrimKeySearchResult;
    tsp00_Int4           PrimKeyIndex;
    tsp00_Int4           RecIndex                 = FIRST_REC_INDEX_BD00;
    tgg00_RecPtr         pRec;
    cbd600_Node          LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_update));
    bool                 bIsRootExclusiveLocked   = false;
    cbd500_Tree          Tree (LeafNode);

    /* initialize input parameters */
    SecKeyFound = false;

    /* search secondary key in index */
    Tree.bd500FindNode (SecKey, SecKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    SecKeySearchResult.becomes  (nonefound);
    PrimKeySearchResult.becomes (nonefound);
    LeafNode.bd600SearchRecord  (SecKey, SecKeyLen, RecIndex, SecKeySearchResult);
    do
    {
        if (SecKeySearchResult != thisfound)
        {
            /* check if the new record suits into current page */
            if ( ! bIsRootExclusiveLocked
                    &&
                    ! LeafNode.bd600SpaceIsAvailable
                    (cbd450_InvListArray::bd450NewSize (InvCurrent.bd300ConsistentReadSupport(), SecKeyLen, PrimKeyLen) + POINTERSIZE_BD00)
               )
            {
                /* OVERFLOW HANDLING */

                /* release current Leaf Node */
                LeafNode.bd600Release (! IS_CHANGED_BD600);

                /* get exclusive tree lock if the record does not suit into current page */
                if ( InvCurrent.bd300LockTreeExclusive() )
                    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                        Tree.bd500_WriteTrace("bd400AddToInvTree: OverFlow1",28);
                if (TrError != e_ok) return;

                Tree.bd500FindNode (SecKey, SecKeyLen, LEAF_LEVEL_BD00);
                if (TrError != e_ok) return;

                LeafNode.bd600SearchRecord (SecKey, SecKeyLen, RecIndex, SecKeySearchResult);

                bIsRootExclusiveLocked = true; // if seckey was found now, check/search for primkey
            }
            else
                /* pages are properly locked, a new list can be inserted */
                break;
        }
        else
        {
            /* get pointer to record */
            pRec = LeafNode.bd600RecPtr (RecIndex);

            /* check record kind */
            if ( ilkSubTree_egg00 == pRec->recInvListKind_gg00() )     // InvList = sub tree
            {
                SubTreeExist = true;
                /* pages are properly locked, the primkey can be inserted into subtree */
                break;
            }
            else                                                     // InvList = array
            {
                tsp00_Int4          NewInvListArraySize;
                cbd450_InvListArray InvListArray (InvCurrent.bd300ConsistentReadSupport(), pRec);

                if (InvCurrent.bd300ConsistentReadSupport())
                {
                    InvListArray.bd450SearchPrimKey (PrimKey, PrimKeyLen, InvCurrent.curr_trans->trWriteTransId_gg00,
                        PrimKeyIndex, PrimKeySearchResult);
                    if (thisfound == PrimKeySearchResult)
                    {
                        break;
                    }
                }
                else
                {
                    InvListArray.bd450SearchPrimKey (PrimKey, PrimKeyLen, 0 /*false*/, PrimKeyIndex, PrimKeySearchResult);
                }
                if (TrError != e_ok) return;

                /* check if the expanded record suits into the current page */

                NewInvListArraySize = InvListArray.bd450Size (PrimKeyLen);

                if  (NewInvListArraySize > MAX_INVLISTLENGTH_BD00)
                {
                    bUpgradeInvListToSubtree = true;
                    /* pages are properly locked, the list can be expanded to a subtree */
                    break;
                }
                else  if ( ! bIsRootExclusiveLocked
                           &&
                           ! LeafNode.bd600SpaceIsAvailable (NewInvListArraySize)
                         )
                {
                    /* release current node */
                    LeafNode.bd600Release (! IS_CHANGED_BD600);

                    if ( InvCurrent.bd300LockTreeExclusive() )
                        if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                            Tree.bd500_WriteTrace("bd400AddToInvTree: OverFlow2",28);
                    if (TrError != e_ok) return;

                    Tree.bd500FindNode (SecKey, SecKeyLen, LEAF_LEVEL_BD00);
                    if (TrError != e_ok) return;

                    LeafNode.bd600SearchRecord (SecKey, SecKeyLen, RecIndex, SecKeySearchResult);

                    bIsRootExclusiveLocked = true; // search for primkey again

#                   if COMPILEMODE_MEO00 >= SLOW_MEO00
                    pRec = LeafNode.bd600RecPtr (RecIndex);
                    InvListArray.bd450Update (pRec); // Only for Destructor Check of InvListArray
#                   endif
                }
                else
                    /* pages are properly locked, the primkey can be inserted into the existing list */
                    break;
            }
        }
    }
    while (bIsRootExclusiveLocked);

    /* PTS 1106992 UH 2000-06-21 */
    /* bPropagationNeeded must be testet everytime, a leaf was searched and not only on */
    /* access to a subtree record                                                       */
    bool bPropagationNeeded = Tree.bd500PropagationNeeded();

    switch (SecKeySearchResult)
    {
    case  lastfound :
    case  nextfound :
    case  nonefound :
        if ( lastfound == SecKeySearchResult ) ++RecIndex;
        bd400_CreateNewInvList (Tree, LeafNode, RecIndex, SecKey, SecKeyLen, PrimKey, PrimKeyLen,
                                bPropagationNeeded, InvCurrent.bd300ConsistentReadSupport());
        InvCurrent.bd300UpdateEntryCounter(1);
        break;
    case thisfound :
        SecKeyFound = true;

        if (SubTreeExist)
        {
            /* insert primary key to InvListSubtree */
            bd400_AddPrimKeyToSubTree (InvCurrent, LeafNode, PrimKey, PrimKeyLen, RecIndex);
            /* LeafNode may be released and Tree may be unlocked now */
            if (e_ok != TrError) break; /* PTS 1106992 UH 2000-06-21 */
            if ( bPropagationNeeded )    /* PTS 1106992 UH 2000-06-21 */
            {
				// PTS 1120380 UH 2003-02-03 enter tree again for propagation
                if ( ! LeafNode.bd600IsAccessable() )
                {
                    if ( InvCurrent.bd300LockTreeExclusive() )
                        if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                            Tree.bd500_WriteTrace("bd400AddToInvTree: Propagation1",31);
                    if (TrError != e_ok) return; // PTS 1121867 UH 2003-05-02
                    Tree.bd500FindNode (SecKey, SecKeyLen, LEAF_LEVEL_BD00);
                    if (TrError != e_ok) return;
                }
                Tree.bd520LeafCountPropagation (SecKey, SecKeyLen);
            }
        }
        else
        {
            pRec = LeafNode.bd600RecPtr (RecIndex);
            cbd450_InvListArray InvListArray (InvCurrent.bd300ConsistentReadSupport(), pRec);

            switch (PrimKeySearchResult)
            {
            case  thisfound :
                if ( bPropagationNeeded )   /* PTS 1106992 UH 2000-06-21 */
                    Tree.bd520LeafCountPropagation (SecKey, SecKeyLen);
                if (e_ok != TrError) break; /* PTS 1106992 UH 2000-06-21 */
                TrError = e_duplicate_key;
                break;
            case  lastfound :
            case  nextfound :
                if ( lastfound == PrimKeySearchResult )
                    ++PrimKeyIndex;

                if (bUpgradeInvListToSubtree)
                {
                    /* upgrade InvListArray to InvListSubtree and */
                    /* insert prim key into InvListSubtree        */

                    cbd300_InvSubTreeCurrent InvSubTreeCurrent (InvCurrent);
                    if (e_ok != TrError) break;
                    cbd601_SubTreeNode       SubTreeLeaf       (InvSubTreeCurrent);
                    if (e_ok != TrError) break;
                    cbd500_Tree              SubTree           (SubTreeLeaf);
                    if (e_ok != TrError) break;

                    bd400_InvListArrayOverflow (Tree, LeafNode, SubTree, SubTreeLeaf,
                                                RecIndex, PrimKey, PrimKeyLen, PrimKeyIndex, InvCurrent.bd300ConsistentReadSupport());
                    if (e_ok != TrError) break;

                    /* completely release the SubTree */
                    SubTreeLeaf.bd600Release (IS_CHANGED_BD600); /* PTS 1104207 UH 14-10-1999 */
                    if (e_ok != TrError) break;
                    
                    InvSubTreeCurrent.bd300UnlockTree();         /* PTS 1104207 UH 14-10-1999 */
                    if (e_ok != TrError) break;

                    /* check UnderflowCondition, lock InvTree exclusive */
                    if ( Tree.bd520LeafUnderFlow (SecKey, SecKeyLen) != ANYTHING_DONE )
                    {
                        /* PTS 1106992 UH 2000-06-21 */
                        if (e_ok != TrError) break;
                        if ( bPropagationNeeded )
                            Tree.bd520LeafCountPropagation ();
                    }
                }
                else
                { /* insert prim key into InvListArry */
                    const tsp00_Int4 RecSizeDelta = InvListArray.bd450Size (PrimKeyLen)
                                                    -
                                                    pRec->recLen_gg00();

                    if ( bIsRootExclusiveLocked &&
                            ! LeafNode.bd600SpaceIsAvailable (RecSizeDelta) )
                    {
                        if ( Tree.bd520LeafOverflow (SecKey, SecKeyLen, ! FOR_INSERT_GBD500,
                                                     RecSizeDelta, RecIndex) != ANYTHING_DONE )
                        {
                            /* PTS 1106992 UH 2000-06-21 */
                            if (e_ok != TrError) break;
                            if ( bPropagationNeeded )
                                Tree.bd520LeafCountPropagation ();
                        }
                        if (e_ok != TrError) break;
                    }
                    pRec = LeafNode.bd600RecPtr (RecIndex);
                    LeafNode.bd600UpdRecordSpace (RecSizeDelta, RecIndex, pRec);
                    if (e_ok != TrError) break;

                    InvListArray.bd450Update (pRec);
                    InvListArray.bd450InsertPrimKey (*InvCurrent.curr_trans, PrimKeyIndex, PrimKey, PrimKeyLen);
                }
                break;
            default:
                TrError = e_not_implemented; break; // impossible
            } // end of switch
        }
        // bd300UpdatePrimKeyEntryCounter(1); if prim keys would be counted
        break;
    default:
        TrError = e_not_implemented; break; // impossible
    } // end of switch
}

/*---------------------------------------------------------------------------*/

externCpp void
bd400DelFromInvTree (cbd300_InvCurrent       &InvCurrent,
                     tsp00_KeyPtr             SecKey,
                     tsp00_Int4               SecKeyLen,
                     tsp00_KeyPtr             PrimKey,
                     tsp00_Int4               PrimKeyLen,
                     bool                     unlock,
                     tbd00_DelInvReason_Param CallReason)
{
    ROUTINE_DBG_MEO00 ("bd400DelFromInvTree");

    tgg00_BasisError     &TrError       = InvCurrent.curr_trans->trError_gg00;
    tsp00_Int4            RecIndex      = FIRST_REC_INDEX_BD00;
    tbd_searchresult      SecKeySearchResult;
    tbd_searchresult      PrimKeySearchResult;
    cbd600_Node           LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_update));
    cbd500_Tree           Tree (LeafNode);
    if (TrError != e_ok) return;

    SecKeySearchResult.becomes  (nonefound);
    PrimKeySearchResult.becomes (nonefound);

    /* search secondary key in index */

    Tree.bd500FindNode (SecKey, SecKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    LeafNode.bd600SearchRecord (SecKey, SecKeyLen, RecIndex, SecKeySearchResult);
    if (TrError != e_ok) return;

    /* PTS 1106992 UH 2000-06-21 */
    /* bPropagationNeeded must be testet everytime, a leaf was searched and not only on */
    /* access to a subtree record                                                       */
    const bool bPropagationNeeded = Tree.bd500PropagationNeeded();

    if (thisfound != SecKeySearchResult)
    {
        TrError = e_key_not_found;
    }
    else
    {
        tgg00_RecPtr pRec = LeafNode.bd600RecPtr (RecIndex);

        switch ( pRec->recInvListKind_gg00() )
        {
        case ilkArray_egg00:
            {
                cbd450_InvListArray InvListArray (InvCurrent.bd300ConsistentReadSupport(), pRec);
                tsp00_Int4          PrimKeyIndex;

                tgg91_TransNo  writeTrans;
                tgg91_TransNo* pWriteTrans = 0;
                if (InvCurrent.bd300ConsistentReadSupport() &&
                   ((dirGarbageCollection_ebd00 == CallReason) || (dirDeletePrimKey_ebd00 == CallReason))
                   )
                {
                   InvCurrent.curr_trans->trWriteTransId_gg00.gg91BuildComplement (writeTrans);
                   pWriteTrans = &writeTrans;
                }
                InvListArray.bd450SearchPrimKey (PrimKey, PrimKeyLen, pWriteTrans,
                    PrimKeyIndex, PrimKeySearchResult);
                if (TrError != e_ok) break;
                if ((lastfound == PrimKeySearchResult) && (PrimKeyIndex + 1 < InvListArray.bd450RecPtr()->recNumPrimKeys_gg00()))
                {
                    ++PrimKeyIndex;
                    PrimKeySearchResult.becomes(nextfound);
                }

                if (nextfound == PrimKeySearchResult && 
                    (dirSetDeletedFlag_ebd00 == CallReason || dirRemoveDeletedFlag_ebd00 == CallReason  || dirGarbageCollection_ebd00 == CallReason))
                {
                    tsp00_KeyPtr pPrimKey;
                    tsp00_Int2   primKeyLen;
                    InvListArray.bd450GetPrimKey (PrimKeyIndex, pPrimKey, primKeyLen);
                    if (primKeyLen == PrimKeyLen + sizeof(tgg91_TransNo))
                    {
                        tsp00_LcompResult CompResult;
                        s30cmp (PrimKey, 1, PrimKeyLen, pPrimKey, 1, PrimKeyLen, CompResult);
                        if (l_equal == CompResult)
                        {
                            PrimKeySearchResult.becomes(thisfound);
                            if (dirGarbageCollection_ebd00 != CallReason)
                            {
                                // check if this is a key inserted by the current transaction
                                PrimKeySearchResult.becomes(thisfound);
                                for (int ix = 0; ix < sizeof(tgg91_TransNo); ++ix)
                                {
                                    if (pPrimKey[PrimKeyLen+ix] != 255 - InvCurrent.curr_trans->trWriteTransId_gg00[ix])
                                    {
                                        PrimKeySearchResult.becomes(nextfound);
                                        break;
                                    }
                                    
                                }
                                if (nextfound == PrimKeySearchResult)
                                {
                                    if (dirRemoveDeletedFlag_ebd00 == CallReason)
                                    {
                                        InvListArray.bd450RemoveDeletedFlag (PrimKeyIndex);
                                    }
                                    else
                                    {
                                        InvListArray.bd450MarkPrimKeyDeleted (PrimKeyIndex);
                                        
                                    }
                                    if (bPropagationNeeded)
                                    {
                                        Tree.bd520LeafCountPropagation (SecKey, SecKeyLen);
                                    }
                                    return;
                                }
                            }
                        }
                    }
                }
                if (thisfound != PrimKeySearchResult)
                {
                    if (bPropagationNeeded) /* PTS 1106992 UH 2000-06-21 */
                        Tree.bd520LeafCountPropagation (SecKey, SecKeyLen);
                    if (TrError != e_ok) break;
                    TrError = e_key_not_found;
                }
                else
                {
                    tsp00_Int4 OldRecLen = pRec->recLen_gg00();
                    bool       bRemoveRecord = false;

                    InvListArray.bd450DeletePrimKey (*InvCurrent.curr_trans, PrimKeyIndex, bRemoveRecord);
                    if (e_ok != TrError) break;

                    if (bRemoveRecord)
                        LeafNode.bd600DelRecordSpace (RecIndex);
                    else
                        LeafNode.bd600UpdRecordSpace (pRec->recLen_gg00() - OldRecLen, RecIndex, pRec);
                    if (e_ok != TrError) break;

                    /* PTS 1106992 UH 2000-06-21 */
                    if ( Tree.bd520LeafUnderFlow (SecKey, SecKeyLen) != ANYTHING_DONE )
                    {
                        if (bPropagationNeeded) /* PTS 1106992 UH 2000-06-21 */
                            Tree.bd520LeafCountPropagation (SecKey, SecKeyLen);
                    }
                    if (e_ok != TrError) break;

#                   if COMPILEMODE_MEO00 >= SLOW_MEO00
                    /* Only for Destructor Check of InvListArray */
                    InvListArray.bd450Update (NULL);
#                   endif
                    if (bRemoveRecord)
                        InvCurrent.bd300UpdateEntryCounter(-1);
                }
                break;
            }
        case ilkSubTree_egg00:
            /* delete primary key from InvListSubtree */
            bd400_DelPrimKeyFromSubTree (InvCurrent, LeafNode, PrimKey, PrimKeyLen, RecIndex, CallReason);
            if ( TrError == e_ok && bPropagationNeeded ) // PTS 1121867 UH 2003-05-02
            {
				// PTS 1120380 UH 2003-02-03 enter tree again for propagation
                if ( ! LeafNode.bd600IsAccessable() )
                {
                    if ( InvCurrent.bd300LockTreeExclusive() )
                        if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                            Tree.bd500_WriteTrace("bd400DelFromInvTree: Propagation",32);
                    if (TrError != e_ok) return; // PTS 1121867 UH 2003-05-02
                    Tree.bd500FindNode (SecKey, SecKeyLen, LEAF_LEVEL_BD00);
                    if (TrError != e_ok) return;
                }
                Tree.bd520LeafCountPropagation (SecKey, SecKeyLen);
            }
            // bd300UpdatePrimKeyEntryCounter(-1); if prim keys would be counted
            break;
        default:
            TrError = e_not_implemented;
        }
    }
}


/*---------------------------------------------------------------------------*/

externCpp void
bd400DropInvTree (cbd300_InvCurrent &InvCurrent)
{
    ROUTINE_DBG_MEO00 ("bd400DropInvTree");

    tgg00_BasisError &TrError   = InvCurrent.curr_trans->trError_gg00;
    tgg00_FileId     &InvFileId = InvCurrent.curr_tree_id;
    cbd600_Node       CurrNode (InvCurrent);
    cbd500_Tree       Tree     (CurrNode);

    if (
        (
        (FileDir_ISharedDirectory::Instance().GetRootPageNo() == InvFileId.fileRoot_gg00()) ||
        (bd17GetBlobFdirRoot() == InvFileId.fileRoot_gg00())
        ) &&
        (InvFileId.fileType_gg00().includes (ftsPerm_egg00))
        )
    {
        TrError = e_do_not_drop_fdir;
        return;
    }

    InvFileId.fileBdUse_gg00().addElement(bd_release_acc);

    Tree.bd500SetToRoot ();

    if ( (e_ok == TrError) && CurrNode.bd600IsAccessable() )
    {
        if ( CurrNode.bd600IsLeaf())
        {
            // Root Is Leaf
            bd400_DeleteSubTrees (InvCurrent, CurrNode);
            if ( e_ok == TrError )
                CurrNode.bd600Free();
        }
        else
        {
            Tree.bd500NextNode();

            if ( e_ok == TrError )
            {
                /* drop the root at first */
                InvCurrent.bd300FreeRoot(); /* PTS 1105868 UH 07-03-2000 */

                if ( e_ok == TrError )
                {
                    while ( CurrNode.bd600IsAccessable() )
                    {
                        if ( CurrNode.bd600IsLeaf() )
                            bd400_DeleteSubTrees (InvCurrent, CurrNode);
                        if ( e_ok != TrError ) break;
                        const tsp00_PageNo FreePno         = CurrNode.bd600GetId();
                        const tsp00_Int4   FreePageVersion = CurrNode.bd600PageConverterVersion();
                        const SAPDB_Int4   FreeLevel       = CurrNode.bd600Level();

                        if ( e_ok != TrError ) break;
                        Tree.bd500NextNode();
                        if ( e_ok != TrError ) break;
                        bd13FreePageNo (FreePno, FreePageVersion, FreeLevel, InvCurrent);
                        if ( e_ok != TrError ) break;
                    }
                }
            }
        }
    }

    InvFileId.fileBdUse_gg00().delElement(bd_release_acc);
}


/*---------------------------------------------------------------------------*/

externCpp void
bd400TestUniqueInv (cbd300_InvCurrent &InvCurrent,
                    tsp00_KeyPtr       SecKey,
                    tsp00_Int4         SecKeyLen,
                    tsp00_KeyPtr       PrimKey,
                    tsp00_Int4         PrimKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd400TestUniqueInv");

    tgg00_BasisError   &TrError       = InvCurrent.curr_trans->trError_gg00;
    tsp00_Int4          SecKeyIndex   = MAX_POINTERINDEX_BD00;
    tsp00_Int4          PrimKeyIndex;
    tbd_searchresult    SecKeySearchResult;
    tbd_searchresult    PrimKeySearchResult;
    cbd600_Node         LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree         Tree (LeafNode);
    tgg00_RecPtr        pRec;

    if (TrError != e_ok) return;

    SecKeySearchResult.becomes  (nonefound);
    PrimKeySearchResult.becomes (nonefound);

    /* search secondary key in index */

    Tree.bd500FindNode (SecKey, SecKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    LeafNode.bd600SearchRecord (SecKey, SecKeyLen, SecKeyIndex, SecKeySearchResult);

    if (thisfound != SecKeySearchResult) return;

    /* search for PrimKey within the found Secondary Key List */
    pRec = LeafNode.bd600RecPtr (SecKeyIndex);

    switch ( pRec->recInvListKind_gg00() )
    {
    case ilkArray_egg00:
        {
            if (pRec->recNumPrimKeys_gg00() > 1)
            {
                TrError = e_duplicate_invkey;
                break;
            }

            cbd450_InvListArray InvListArray (InvCurrent.bd300ConsistentReadSupport(), pRec);

            InvListArray.bd450SearchPrimKey (PrimKey, PrimKeyLen, 0, PrimKeyIndex, PrimKeySearchResult); // TODO
            if (TrError != e_ok) break;

            if (thisfound != PrimKeySearchResult)
            {
                TrError = e_duplicate_invkey;
                break;
            }
            break;
        }
    case ilkSubTree_egg00:
        TrError = e_duplicate_invkey;
        break;
    default:
        TrError = e_not_implemented;
    }
}

/*===========================================================================*
 *  PASCAL INTERFACE TO FUNCTIONS (CODE)                                     *
 *===========================================================================*/

/* Only to have a pascal interface to call */
/* PRECONDITION: InvCurrent is constructed as a class in vbd03.cpp */

externC void
bd400AddToInvTreePascal (tbd_current_tree *InvCurrent,
                         tgg00_Lkey       *SecKey,
                         tgg00_Lkey       *PrimKey,
                         bool             *SecKeyFound)
{
    cbd300_InvCurrent *AuxCurrent = STATIC_CAST(cbd300_InvCurrent*,InvCurrent);

    bd400AddToInvTree (*AuxCurrent, tsp00_KeyPtr(&(SecKey->keyVal_gg00())), SecKey->keyLen_gg00(),
                       tsp00_KeyPtr(&(PrimKey->keyVal_gg00())), PrimKey->keyLen_gg00(), *SecKeyFound);

    if ( e_ok == InvCurrent->curr_trans->trError_gg00 ) AuxCurrent->bd300UpdateLeafCount();
}

/*---------------------------------------------------------------------------*/

static inline void
bd400_DeleteSubTrees (cbd300_InvCurrent &InvCurrent,
                      cbd600_Node       &InvLeaf)
{
    ROUTINE_DBG_MEO00 ("bd400_DeleteSubTrees");

    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;

    if (
        (0 < InvLeaf.bd600NumRecords())
        || (tfnTempMulti_egg00 == InvCurrent.curr_tree_id.fileTfn_gg00())
    )
    {
        tgg00_RecPtr pRec;

        for (tsp00_Int4 RecIndex = FIRST_REC_INDEX_BD00;
                RecIndex <= InvLeaf.bd600MaxRecIndex();
                ++RecIndex)
        {
            pRec = InvLeaf.bd600RecPtr (RecIndex);

            switch (pRec->recInvListKind_gg00())
            {
            case ilkArray_egg00: break;
            case ilkSubTree_egg00:
                {
                    cbd450_InvListRefSubTree InvListRefSubTree (pRec);
                    if ( InvLeaf.bd600IsUpdateable() )
                        InvLeaf.bd600LeafCount() -= InvListRefSubTree.bd450NumLeaves();
                    cbd300_InvSubTreeCurrent InvSubTreeCurrent (
                        InvCurrent,
                        tbd_node_request::fromConst(nr_for_update),
                        InvListRefSubTree,
                        InvLeaf.bd600IsUpdateable());
                    if ( e_ok != TrError ) return;
                    if ( InvLeaf.bd600IsUpdateable() )
                        InvLeaf.bd600LeafCount() += InvListRefSubTree.bd450NumLeaves();
                    cbd600_Node SubTreeNode (InvSubTreeCurrent);
                    cbd500_Tree SubTree     (SubTreeNode);

                    SubTree.bd500SetToRoot();

                    if ( (e_ok == TrError) && SubTreeNode.bd600IsAccessable() )
                    {
                        if ( SubTreeNode.bd600IsLeaf() )
                        {
                            // Root Is Leaf
                            SubTreeNode.bd600Free();
                        }
                        else
                        {
                            SubTree.bd500NextNode();
                            if ( e_ok != TrError ) break;

                            /* drop the root at first */
                            InvSubTreeCurrent.bd300FreeRoot(); /* PTS 1105868 UH 07-03-2000 */

                            while ( SubTreeNode.bd600IsAccessable() )
                            {
                                const tsp00_PageNo  FreePno         = SubTreeNode.bd600GetId();
                                const tsp00_Int4    FreePageVersion = SubTreeNode.bd600PageConverterVersion();
                                const SAPDB_Int4    FreeLevel       = SubTreeNode.bd600Level();

                                SubTree.bd500NextNode();
                                if ( e_ok != TrError ) break;
                                bd13FreePageNo (FreePno, FreePageVersion, FreeLevel, InvSubTreeCurrent);
                                if ( e_ok != TrError ) break;
                            }
                        }
                    }
                }
                break;

            default:
                TrError = e_illegal_record;
                InvLeaf.bd600Dump (bd400c1IllegalRecord_csp03, "bd400_DeleteSubTrees    ");
                return;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
/* Only to have a pascal interface to call */
/* PRECONDITION: InvCurrent is constructed as a class in vbd03.cpp */

externC void
bd400TestUniqueInvPascal (tbd_current_tree *InvCurrent,
                          tgg00_Lkey       *SecKey,
                          tgg00_Lkey       *PrimKey)
{
    cbd300_InvCurrent *AuxCurrent = STATIC_CAST(cbd300_InvCurrent*,InvCurrent);

    bd400TestUniqueInv (*AuxCurrent, tsp00_KeyPtr(&(SecKey->keyVal_gg00())), SecKey->keyLen_gg00(),
                        tsp00_KeyPtr(&(PrimKey->keyVal_gg00())), PrimKey->keyLen_gg00());
}

/*---------------------------------------------------------------------------*/

externCpp void
bd400Verify (cbd300_InvCurrent  &InvCurrent,
             bool			 	 bIsCold,
             bool                bWithExtendedCheck,
             tsp00_Int4         &NumPages,
             tgg00_IndexCounter &counter)
{
    ROUTINE_DBG_MEO00 ("bd400Verify");
    
    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;
    cbd600_Node       Node1     (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd600_Node       Node2     (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree       Tree1     (Node1);
    if (e_ok != TrError) return;
    cbd500_Tree       Tree2     (Node2);
    if (e_ok != TrError) return;
    
    b06file_opmsg (csp3_check_file, InvCurrent.curr_tree_id);
    
    bd400_Verify (InvCurrent, Tree1, Tree2, bIsCold, bWithExtendedCheck, ! IS_SUBTREE_BD400, NumPages, counter);
    
    if (  e_ok                  != TrError
          &&
          e_disk_not_accessible != TrError
          &&
          e_cancelled           != TrError
          &&
          e_shutdown            != TrError )
    { 
        b06write_filename_and_root (InvCurrent.curr_tree_id);
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd400ExtractIndex ( 
           cbd300_InvCurrent      &invCurrent,
           const tsp00_VFilename  &hostFileName )
{
    ROUTINE_DBG_MEO00 ("bd400ExtractIndex");
    
    tgg00_BasisError &trError = invCurrent.curr_trans->trError_gg00;
    
    cbd600_Node       node( invCurrent, tbd_node_request::fromConst( nr_for_read ));
    cbd500_Tree       tree( node );
    
    if( e_ok != trError ) 
        return;
    
    tree.bd500SetToRoot();
    
    Kernel_FileIO hostFile( hostFileName );

    while ( node.bd600IsAccessable() && ( e_ok == trError ))
    {
        Data_BasePage   page( node.bd600GetNodePtr());

        if( ! hostFile.Write( page ))
        {
            trError = e_hostfile_error;
            return;
        }

        if( node.bd600IsLeaf())
        {
            const tsp00_Int4 maxRecIndex = node.bd600MaxRecIndex();

            for( tsp00_Int4 recIndex = FIRST_REC_INDEX_BD00; recIndex <= maxRecIndex; ++recIndex )
            {
                if( ilkSubTree_egg00 != node.bd600RecPtr( recIndex)->recInvListKind_gg00() )
                    continue;
                
                cbd450_InvListRefSubTree    invListRefSubTree ( node.bd600RecPtr( recIndex ));
                cbd300_InvSubTreeCurrent    subTreeCurrent( invCurrent, tbd_node_request::fromConst( nr_for_read ),
                                                            invListRefSubTree, false);
                
                cbd600_Node  subTreeNode( subTreeCurrent, tbd_node_request::fromConst( nr_for_read ));
                cbd500_Tree  subTree( subTreeNode );
                
                if( e_ok != trError ) 
                    return;
                
                subTree.bd500SetToRoot();

                if( e_ok != trError ) 
                    return;

                while( subTreeNode.bd600IsAccessable())
                {
                    Data_BasePage   subPage( subTreeNode.bd600GetNodePtr());
                    
                    if( hostFile.Write( subPage ))
                        subTree.bd500NextNode();
                    else
                        trError = e_hostfile_error;
                    
                    if ( e_ok != trError ) 
                        return;
                }
            }
        }
        tree.bd500NextNode();
    }
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static inline void
bd400_AddPrimKeyToSubTree (cbd300_InvCurrent &InvCurrent,
                           cbd600_Node       &LeafNode,
                           tsp00_KeyPtr       PrimKey,
                           tsp00_Int4         PrimKeyLen,
                           tsp00_Int4         RecIndex)
{
    ROUTINE_DBG_MEO00 ("bd400_AddPrimKeyToSubTree");

    tgg00_BasisError         &TrError = InvCurrent.curr_trans->trError_gg00;
    cbd450_InvListRefSubTree  InvListRefSubTree (LeafNode.bd600RecPtr (RecIndex));
    const bool                updateStatistics = LeafNode.bd600IsUpdateable();
    if ( updateStatistics )
        LeafNode.bd600LeafCount() -= InvListRefSubTree.bd450NumLeaves();
    tbd_searchresult          PrimKeySearchResult;
    bool                      bIsRootExclusiveLocked = false;

    /* in the constructor cbd300_InvSubTreeCurrent InvListRefSubTree */
    /* is updated from the SubTreeRoot                               */
    cbd300_InvSubTreeCurrent  InvSubTreeCurrent (InvCurrent,
            tbd_node_request::fromConst(nr_for_update),
            InvListRefSubTree,
            LeafNode.bd600IsUpdateable());

    if (TrError != e_ok) return;

    if ( updateStatistics )
    {
        LeafNode.bd600LeafCount() += InvListRefSubTree.bd450NumLeaves();
        InvListRefSubTree.bd450IncrNumPrimKeys();
    }
	// PTS 1131150 UH prepare to revert increment of primkey count
    // save inv base record information in case an error ocurs we can rollback changes made
    const tsp00_PageNo leafPageNo = LeafNode.bd600GetId();
          tgg00_Lkey   invKey;
    if ( updateStatistics )
    {
        tsp00_KeyPtr pSecKey   = 0;
		tsp00_Int2   SecKeyLen = 0;
        InvListRefSubTree.bd450GetSecKey(pSecKey,SecKeyLen);
        SAPDB_RangeMove(__FILE__, 1,
                        SecKeyLen, sizeof(invKey.keyVal_gg00()),
                        pSecKey, 1, &invKey.keyVal_gg00(), 1, SecKeyLen,
                        TrError );
        if (TrError != e_ok)
            return;
        invKey.keyLen_gg00() = SecKeyLen;
    }

    /* release all resources of the primary index tree */
    LeafNode.bd600Release (IS_CHANGED_BD600);

    /* do not reference LeafNode any more */
    const bool treeIsUnlocked = m_create_index_parallel != InvCurrent.curr_action
                                &&
                                m_create_index          != InvCurrent.curr_action;
    if ( treeIsUnlocked )
    {
        InvCurrent.bd300UnlockTree ();
        if (e_ok != TrError) return;
    }

    /* construct new subtree node */
    cbd601_SubTreeNode SubTreeLeaf (InvSubTreeCurrent, tbd_node_request::fromConst(nr_for_update));
    cbd500_Tree SubTreeTree (SubTreeLeaf);
    if (e_ok != TrError) return;

    /* search primary key in subtree */
    SubTreeTree.bd500FindNode (PrimKey, PrimKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    PrimKeySearchResult.becomes (nonefound);

    if (InvCurrent.bd300ConsistentReadSupport())
    {
        SubTreeLeaf.bd600SearchRecordConsistent (PrimKey, PrimKeyLen, &InvCurrent.curr_trans->trWriteTransId_gg00, RecIndex, PrimKeySearchResult);
    }
    else
    {
        SubTreeLeaf.bd600SearchRecord (PrimKey, PrimKeyLen, RecIndex, PrimKeySearchResult);
    }

    /* return with error if prim key exists already */
    if (thisfound == PrimKeySearchResult)
    {
        if ( updateStatistics ) 	// PTS 1131150 UH revert increment of primkey count
        {
            SubTreeLeaf.bd600Release(IS_CHANGED_BD600);
            InvSubTreeCurrent.bd300UnlockTree(); // PTS 1137139 UH 2005-08-23 added to avoid deadlocks
            if (e_ok != TrError) return;

            // reset the PrimKeyCount in the invBaseRecord
            tsp00_Int4       recindex;
            tbd_searchresult result;
            if ( ! treeIsUnlocked )
			{
                LeafNode.bd600GetNode(leafPageNo);
                if (e_ok != TrError) return;
			}
            else
            {
                InvCurrent.bd300LockTree();
                if (TrError != e_ok) return;
                cbd500_Tree InvTree (LeafNode);
                if (e_ok != TrError) return;
                InvTree.bd500FindNode ((tsp00_KeyPtr)&invKey.keyVal_gg00(), invKey.keyLen_gg00(), LEAF_LEVEL_BD00);
                if (e_ok != TrError) return;
            }
            LeafNode.bd600SearchRecord((tsp00_KeyPtr)&invKey.keyVal_gg00(), invKey.keyLen_gg00(), recindex, result);
            if ( thisfound == result ) // is a different result possible ?!
            {
                cbd450_InvListRefSubTree invBaseRec (LeafNode.bd600RecPtr(recindex));
                invBaseRec.bd450DecrNumPrimKeys();
            }
			LeafNode.bd600Release (IS_CHANGED_BD600);
            if ( treeIsUnlocked )
            {
                InvCurrent.bd300UnlockTree();
                if (TrError != e_ok) return;
            }
        }
        TrError = e_duplicate_key;
        return;
    }

    /* upgrade tree lock in case a page split is required to introduce new prim key */
    tsp00_Int4 RequiredLen;
    
    RequiredLen = PrimKeyLen + POINTERSIZE_BD00 +
                  (InvCurrent.bd300ConsistentReadSupport()?sizeof(tgg91_TransNo):0);

    if ( ! SubTreeLeaf.bd600SpaceIsAvailable ( RequiredLen ))
    {
        /* SubTree is always locked EXCLUSIVE - look the SubtreeCurrentConstructor */
        bIsRootExclusiveLocked = true;
    }

    /* insert prim key into subtree */
    switch (PrimKeySearchResult)
    {
    case  lastfound :
    case  nextfound :
    case  nonefound :
        {
            tsp00_BytePtr pKeySpace;

            if ( lastfound == PrimKeySearchResult )
                ++RecIndex;

            if ( bIsRootExclusiveLocked && ! SubTreeLeaf.bd600SpaceIsAvailable ( RequiredLen ) )
            {
                if (InvCurrent.bd300ConsistentReadSupport())
                {
                    // create the final primkey. This may be required for separatpr construction
                    tsp00_Key key;
                    bd400_ConstructSubTreeRecord (InvCurrent.bd300ConsistentReadSupport(),
                                                  InvCurrent.curr_trans->trWriteTransId_gg00,
                                                  reinterpret_cast<tsp00_KeyPtr>(&key), PrimKey, PrimKeyLen, TrError);
                    if (e_ok != TrError) break;
                    SubTreeTree.bd520LeafOverflow (reinterpret_cast<tsp00_KeyPtr>(&key),
                                                   PrimKeyLen, FOR_INSERT_GBD500,
                                                   RequiredLen, RecIndex);
                }
                else
                {
                    SubTreeTree.bd520LeafOverflow (PrimKey, PrimKeyLen, FOR_INSERT_GBD500,
                                                   RequiredLen, RecIndex);
                }
                if (e_ok != TrError) break;
            }
            SubTreeLeaf.bd600AddRecordSpace (RequiredLen, RecIndex, pKeySpace, InvCurrent.bd300ConsistentReadSupport());
            if (e_ok != TrError) break;

            bd400_ConstructSubTreeRecord (InvCurrent.bd300ConsistentReadSupport(),
                                          InvCurrent.curr_trans->trWriteTransId_gg00,
                                          pKeySpace, PrimKey, PrimKeyLen, TrError);
        }
        break;

    case thisfound :
        TrError = e_duplicate_key;
        break;
    default :
        TrError = e_not_implemented;
        break;
    }
}


/*---------------------------------------------------------------------------*/

static inline void
bd400_DelPrimKeyFromSubTree (cbd300_InvCurrent       &InvCurrent,
                             cbd600_Node             &LeafNode,
                             tsp00_KeyPtr             PrimKey,
                             tsp00_Int4               PrimKeyLen,
                             tsp00_Int4               SecKeyIndex,
                             tbd00_DelInvReason_Param CallReason)
{

    ROUTINE_DBG_MEO00 ("bd400_DelPrimKeyFromSubTree");

    tgg00_BasisError  &TrError = InvCurrent.curr_trans->trError_gg00;

    cbd450_InvListRefSubTree InvListRefSubTree (LeafNode.bd600RecPtr(SecKeyIndex));
    if ( LeafNode.bd600IsUpdateable() )
        LeafNode.bd600LeafCount() -= InvListRefSubTree.bd450NumLeaves();

    cbd300_InvSubTreeCurrent InvSubTreeCurrent (InvCurrent,
            tbd_node_request::fromConst(nr_for_update),
            InvListRefSubTree,
            LeafNode.bd600IsUpdateable());
    if (TrError != e_ok) return;

    /* in the constructor cbd300_InvSubTreeCurrent InvListRefSubTree */
    /* is updated from the SubTreeRoot                               */

    if ( LeafNode.bd600IsUpdateable() )
        LeafNode.bd600LeafCount() += InvListRefSubTree.bd450NumLeaves();

    InvListRefSubTree.bd450DecrNumPrimKeys ();

    /* check if this is the last page in the subtree */
    cbd601_SubTreeNode SubTreeRoot (InvSubTreeCurrent);

    if ( SubTreeRoot.bd600IsLeaf() && SubTreeRoot.bd600Covering () < UNDER_FLOW_LIMIT_BD600 )
    {
        bd400_DelPrimKeyFromSubRoot (InvCurrent, LeafNode, InvSubTreeCurrent, SubTreeRoot,
                                     PrimKey, PrimKeyLen, SecKeyIndex, CallReason);
    }
    else
    {
        /* release all resources of the primary index tree */
        LeafNode.bd600Release (IS_CHANGED_BD600);
        InvCurrent.bd300UnlockTree();

        /* search primary key in subtree */
        tsp00_Int4          PrimKeyRecIndex;
        tbd_searchresult    PrimKeySearchResult;
        cbd600_Node         SubTreeLeaf (InvSubTreeCurrent, tbd_node_request::fromConst(nr_for_update));
        cbd500_Tree         SubTreeTree (SubTreeLeaf);
        if (TrError != e_ok) return;

        SubTreeTree.bd500FindNode (PrimKey, PrimKeyLen, LEAF_LEVEL_BD00);
        if (TrError != e_ok) return;
        if (
            (dirGarbageCollection_ebd00 == CallReason)
            ||
            (InvCurrent.bd300ConsistentReadSupport() && dirDeletePrimKey_ebd00 == CallReason)
            )
        {

            SubTreeLeaf.bd600SearchRecordConsistent (PrimKey, PrimKeyLen,
                &InvCurrent.curr_trans->trWriteTransId_gg00,
                PrimKeyRecIndex, PrimKeySearchResult);
        }
        else
        {
            SubTreeLeaf.bd600SearchRecord (PrimKey, PrimKeyLen, PrimKeyRecIndex, PrimKeySearchResult);
        }

        if ( nextfound == PrimKeySearchResult
             && 
             (    dirSetDeletedFlag_ebd00    == CallReason
               || dirRemoveDeletedFlag_ebd00 == CallReason
               || dirGarbageCollection_ebd00 == CallReason) )
        {
            tsp00_KeyPtr  pCheckKey;
            tsp00_Int2    CheckKeyLength;
            SubTreeLeaf.bd600GetKey (PrimKeyRecIndex, pCheckKey, CheckKeyLength);

            if (CheckKeyLength == PrimKeyLen + sizeof(tgg91_TransNo))
            {
                tsp00_LcompResult CompResult;
                s30cmp ( PrimKey,   1, PrimKeyLen,
                         pCheckKey, 1, PrimKeyLen, CompResult);
                if (l_equal == CompResult)
                {
                    PrimKeySearchResult.becomes(thisfound);
                    if (dirGarbageCollection_ebd00 != CallReason)
                    {
                        // check if this is a key inserted by the current transaction
                        PrimKeySearchResult.becomes(thisfound);
                        for (int ix = 0; ix < sizeof(tgg91_TransNo); ++ix)
                        {
                            if ( *(pCheckKey+PrimKeyLen+ix)
                                 !=
                                 255 - InvCurrent.curr_trans->trWriteTransId_gg00[ix] )
                            {
                                PrimKeySearchResult.becomes(nextfound);
                                break;
                            }
                        }
                        
                        if (nextfound == PrimKeySearchResult)
                        {
                            SubTreeLeaf.bd600SetDeleted( PrimKeyRecIndex,
                                                         dirRemoveDeletedFlag_ebd00 != CallReason );
                            return;
                        }
                    }
                }
            }
        }
        /* return if prim key does not exist */
        if (thisfound != PrimKeySearchResult)
        {
            TrError = e_key_not_found;
            return;
        }

        /* remove record from page in subtree */
        SubTreeLeaf.bd600DelRecordSpace (PrimKeyRecIndex);
        if  (e_ok != TrError) return;

        /* balance subtree if the covering of the page is too small */
        SubTreeTree.bd520LeafUnderFlow (PrimKey, PrimKeyLen);
        if (e_ok != TrError) return;
    }
}


/*---------------------------------------------------------------------------*/

static inline void
bd400_DelPrimKeyFromSubRoot (cbd300_InvCurrent        &InvCurrent,
                             cbd600_Node              &LeafNode,
                             cbd300_InvSubTreeCurrent &InvSubtreeCurrent,
                             cbd601_SubTreeNode       &SubTreeRoot,
                             tsp00_KeyPtr              PrimKey,
                             tsp00_Int4                PrimKeyLen,
                             tsp00_Int4                SecKeyRecIndex,
                             tbd00_DelInvReason_Param  CallReason)
{
    ROUTINE_DBG_MEO00 ("bd400_DelPrimKeyFromSubRoot");

    /* this function removes a key from the subroot, in particular it is */
    /* considered that the subtree can be transformed to a InvListArray  */

    tgg00_BasisError  &TrError  = InvSubtreeCurrent.curr_trans->trError_gg00;
    tsp00_Int4         PrimKeyRecIndex;
    tbd_searchresult   PrimKeySearchResult;
    tgg00_RecPtr       pRec     = LeafNode.bd600RecPtr(SecKeyRecIndex);

    /* search primary key in subtree */
    if (
        (dirGarbageCollection_ebd00 == CallReason)
        ||
        (InvSubtreeCurrent.bd300ConsistentReadSupport() && dirDeletePrimKey_ebd00 == CallReason)
        )
    {
        SubTreeRoot.bd600SearchRecordConsistent (PrimKey, PrimKeyLen,
            &InvSubtreeCurrent.curr_trans->trWriteTransId_gg00,
            PrimKeyRecIndex, PrimKeySearchResult);
    }
    else
    {
        SubTreeRoot.bd600SearchRecord (PrimKey, PrimKeyLen, PrimKeyRecIndex, PrimKeySearchResult);
    }

    if ( nextfound == PrimKeySearchResult
         && 
         ( dirSetDeletedFlag_ebd00 == CallReason
           ||
           dirRemoveDeletedFlag_ebd00 == CallReason
           ||
           dirGarbageCollection_ebd00 == CallReason ) )
    {
        tsp00_KeyPtr  pCheckKey;
        tsp00_Int2    CheckKeyLength;
        SubTreeRoot.bd600GetKey (PrimKeyRecIndex, pCheckKey, CheckKeyLength);

        if (CheckKeyLength == PrimKeyLen + sizeof(tgg91_TransNo))
        {
            tsp00_LcompResult CompResult;
            s30cmp ( PrimKey,   1, PrimKeyLen,
                     pCheckKey, 1, PrimKeyLen, CompResult);
            if (l_equal == CompResult)
            {
                PrimKeySearchResult.becomes(thisfound);
                if (dirGarbageCollection_ebd00 != CallReason)
                {
                    // check if this is a key inserted by the current transaction
                    PrimKeySearchResult.becomes(thisfound);
                    for (int ix = 0; ix < sizeof(tgg91_TransNo); ++ix)
                    {
                        if ( *(pCheckKey+PrimKeyLen+ix)
                             !=
                             255 - InvCurrent.curr_trans->trWriteTransId_gg00[ix] )
                        {
                            PrimKeySearchResult.becomes(nextfound);
                            break;
                        }
                    }
                    if (nextfound == PrimKeySearchResult)
                    {
                        SubTreeRoot.bd600SetDeleted( PrimKeyRecIndex,
                                                     dirRemoveDeletedFlag_ebd00 != CallReason );
                        return;
                    }
                }
            }
        }
    }
  
    /* return if prim key does not exist */
    if (thisfound != PrimKeySearchResult)
    {
        TrError = e_key_not_found;
        return;
    }

    /* check if the subtree is small enough to be accommodated */
    /* in a InvListArray on the current primary index page     */
    /* after the  current prim key has been deleted            */

    const tsp00_Int4 NewNumPrimKeys = SubTreeRoot.bd600NumRecords() - 1;

    if ( 0 == NewNumPrimKeys )
    {
        // There is only one PrimKeyRecord in the SubRootLeaf
        // The InvList must be completely deleted

        /* return subroot page which is not needed any more into the free page pool */
		SubTreeRoot.bd600Release (! IS_CHANGED_BD600);
        InvSubtreeCurrent.bd300FreeRoot();
        if (e_ok != TrError) return;

        /* update the LeafCount of current InvLeaf  PTS 1103896 AK 03-09-1999 */
        cbd450_InvListRefSubTree InvListSubTreeRec (pRec);
        LeafNode.bd600LeafCount() -= InvListSubTreeRec.bd450NumLeaves();

        //--(LeafNode.bd600LeafCount());  /* PTS 1103896 AK 03-09-1999 */

        /* remove SubTreeRefRecord */
        LeafNode.bd600DelRecordSpace (SecKeyRecIndex); /* PTS 1103896 UH 13-09-1999 */

        InvCurrent.bd300UpdateEntryCounter(-1);
    }
    else
    {
        /* add every Len of all PrimKeys minus the one which will be deleted */

        tsp00_KeyPtr pPrimKey;
        tsp00_Int2   PrimKeyLength;
        SubTreeRoot.bd600GetKey (PrimKeyRecIndex, pPrimKey, PrimKeyLength);
        const tsp00_Int4 SumPrimKeyLens  = SubTreeRoot.bd600SumKeyLen () - PrimKeyLength;
        const bool ConsistentReadSupport = true;
        /* RequiredLen is the additional space needed to insert the InvListArrayRec instead of the InvSubtreeRec */
        const tsp00_Int4 RecSizeDelta =
            cbd450_InvListArray::bd450NewSize (!ConsistentReadSupport /* keys already contain consistency info */, 
                                               pRec->recKeyLen_gg00(),
                                               SumPrimKeyLens, NewNumPrimKeys)
            - pRec->recLen_gg00();

        if ( LeafNode.bd600SpaceIsAvailable (RecSizeDelta) )
        {
            /* PTS 1104320 UH 27-10-1999 */
            /* update the LeafCount of current InvLeaf  */
            cbd450_InvListRefSubTree InvListSubTreeRec (pRec);
            LeafNode.bd600LeafCount() -= InvListSubTreeRec.bd450NumLeaves();

            bd400_SubRootUnderflow (InvCurrent, LeafNode,
                                    SubTreeRoot, SecKeyRecIndex, PrimKeyRecIndex, RecSizeDelta);
            if (e_ok != TrError) return;

            /* return subroot page which is not needed any more into the free page pool */
            SubTreeRoot.bd600Release (! IS_CHANGED_BD600);
            InvSubtreeCurrent.bd300FreeRoot();
            //--(LeafNode.bd600LeafCount()); /* PTS 1103896 AK 03-09-1999 */
        }
        else
        {
            /* release all resources of the primary index tree */
            LeafNode.bd600Release (! IS_CHANGED_BD600);
            InvCurrent.bd300UnlockTree();

            /* remove prim key from subroot */
            SubTreeRoot.bd600DelRecordSpace (PrimKeyRecIndex);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void
bd400_CheckSubTreesOfCurrentNode (cbd300_InvCurrent  &InvCurrent,
                                  cbd600_Node        &Node,
                                  bool                bIsCold,
                                  bool                bWithExtendedCheck,
                                  tsp00_Int4         &NumPages,
                                  tgg00_IndexCounter &counter)
{
    ROUTINE_DBG_MEO00 ("bd400_CheckSubTreesOfCurrentNode");

    tgg00_BasisError &TrError = Node.bd600TrError();
    tsp00_Int4        RecIndex;

    counter.idc_sec_keycount += Node.bd600NumRecords();

    for ( RecIndex =  FIRST_REC_INDEX_BD00;
          RecIndex <= Node.bd600MaxRecIndex();
          ++RecIndex)
    {
        if ( ilkSubTree_egg00 == Node.bd600RecPtr(RecIndex)->recInvListKind_gg00() )
        {
            cbd450_InvListRefSubTree InvListRefSubTree ( Node.bd600RecPtr(RecIndex) );
            cbd300_InvSubTreeCurrent SubTreeCurrent    ( InvCurrent,
                                                         tbd_node_request::fromConst(nr_for_read),
                                                         InvListRefSubTree,
                                                         Node.bd600IsUpdateable() );

            counter.idc_prim_keycount += InvListRefSubTree.bd450NumPrimKeys();

            cbd601_SubTreeNode SubTreeNode1 (SubTreeCurrent, tbd_node_request::fromConst(nr_for_read));
            cbd601_SubTreeNode SubTreeNode2 (SubTreeCurrent, tbd_node_request::fromConst(nr_for_read));
            cbd500_Tree  SubTree1 (SubTreeNode1);
            if (e_ok != TrError) break;
            cbd500_Tree  SubTree2 (SubTreeNode2);
            if (e_ok != TrError) break;

            bd400_Verify (SubTreeCurrent, SubTree1, SubTree2, bIsCold, 
                bWithExtendedCheck, IS_SUBTREE_BD400, NumPages, counter);
            if ( e_ok != TrError ) break;
            
            InvCurrent.currLeafPageCountDelta_bd00  += SubTreeCurrent.currLeafPageCountDelta_bd00;
            InvCurrent.currIndexPageCountDelta_bd00 += SubTreeCurrent.currIndexPageCountDelta_bd00;
        }
        else
            if ( ilkArray_egg00 == Node.bd600RecPtr(RecIndex)->recInvListKind_gg00() )
                counter.idc_prim_keycount += Node.bd600RecPtr(RecIndex)->recNumPrimKeys_gg00();
    }
}

/*---------------------------------------------------------------------------*/

static inline void
bd400_VerifyError (tgg00_BasisError TrError,
                   cbd600_Node      &Node1,
                   cbd600_Node      &Node2,
                   tsp00_Int4       value)
{
    switch( TrError )
    {
    case e_illegal_key:
        g01opmsg (sp3p_knldiag, sp3m_error, bd400c1IllegalEntryPos_csp03,
            csp3_n_btree, "Wrong key order         ", value );
            break;
    case e_invalid_index_structure:
        g01opmsg (sp3p_knldiag, sp3m_error, bd400c1IllegalEntryPos_csp03,
            csp3_n_btree, "Invalid index structure ", value);
            break;
    case e_invalid_leaves_structure:
        g01opmsg (sp3p_knldiag, sp3m_error, bd400c1IllegalEntryPos_csp03,
            csp3_n_btree, "Invalid leaves structure", value);
            break;
    case e_illegal_page_no:
        g01opmsg (sp3p_knldiag, sp3m_error, bd400c1IllegalEntryPos_csp03,
            csp3_n_btree, "Invalid leaves structure", value);
            break;
    default:
        g01opmsg (sp3p_knldiag, sp3m_error, bd400c1IllegalEntryPos_csp03,
            csp3_n_btree, "unexpected base error   ", TrError);
    }
        
    Node1.bd600Dump (bd400c1DataPageCorrupted_csp03, "Parent index pageNo     ");
    Node2.bd600Dump (bd400c1DataPageCorrupted_csp03, "Corrupted index page    ");
}

/*---------------------------------------------------------------------------*/

static void
bd400_Verify (cbd300_InvCurrentBasis &InvCurrent,
              cbd500_Tree            &Tree1,
              cbd500_Tree            &Tree2,
              bool                    bIsCold,
              bool                    bWithExtendedCheck,
              bool                    bIsSubTree,
              tsp00_Int4             &NumPages,
              tgg00_IndexCounter     &counter)
{
    ROUTINE_DBG_MEO00 ("bd400_Verify");

    tgg00_BasisError    &TrError    = InvCurrent.curr_trans->trError_gg00;
    tgg00_TransContext  &Trans      = *InvCurrent.curr_trans;
    cbd600_Node         &Node1      = Tree1.bd500CurrentNode();
    cbd600_Node         &Node2      = Tree2.bd500CurrentNode();
    tsp00_Int4           RecIndex1  = FIRST_REC_INDEX_BD00;
    cbd300_InvCurrent   *AuxCurrent = STATIC_CAST(cbd300_InvCurrent*,&InvCurrent);
    Converter_IVerify   &converter  = Converter_IVerify::Instance();
    tsp00_PageNo         leftNeighborId = NIL_PAGE_NO_GG00;

    const RTETask_ITask &task  = *RTETask_ITask::ByTaskID( Trans.trTaskId_gg00 );

    Tree1.bd500SetToRoot();
    Tree2.bd500SetToRoot();

    if( e_ok != TrError )
        return;

    Node1.bd600Check( bWithExtendedCheck ); // PTS 1125096 TS 2003-11-21

    if ( Node1.bd600IsLeaf() )
    {
        ++InvCurrent.currLeafPageCountDelta_bd00;
        if ( ! bIsSubTree )
        {
            bd400_CheckSubTreesOfCurrentNode (*AuxCurrent, Node1, bIsCold, bWithExtendedCheck, NumPages, counter);
            if ( e_ok != TrError ) 
                return;
        }

        NumPages += 1;
        if (bIsCold)
            converter.MarkPageNoAsUsed (task, Node1.bd600GetId(), Data_Dynamic);

        return;
    }
    else
    {
        ++InvCurrent.currIndexPageCountDelta_bd00;
        Tree2.bd500NextNode();
        if ( e_ok != TrError ) 
            return;
    }

    cbd520_Separator Separator;

    while ( Node1.bd600IsAccessable() && Node2.bd600IsAccessable() )
    {
        Node2.bd600Check( bWithExtendedCheck ); // PTS 1125096 TS 2003-11-21
   
        if( e_ok != TrError )
            return;

        if ( Node1.bd600Level() != Node2.bd600Level() + 1)
        {
            TrError = e_invalid_index_structure;
            bd400_VerifyError (TrError, Node1, Node2, Node1.bd600RecOffset( RecIndex1 )+1);
            return;
        }

        Separator.Assign(Node1.bd600SepPtr(RecIndex1));
        
        if ( Separator.bd520SubNodeId() != Node2.bd600GetId() )
        {
            TrError = e_illegal_page_no;
            bd400_VerifyError (TrError, Node1, Node2, Node1.bd600RecOffset( RecIndex1 )+1);
            return;
        }

        // PTS 1134979 UH 2005-04-18 Removed Update of LeafCount.

        if(  bWithExtendedCheck // PTS 1125096 TS 2003-11-21
             &&
             0 < Node2.bd600NumRecords() )   // PTS 1131554 TS 2004-10-06
        {
            // recPos and recLength were checked before
            tsp00_LcompResult   compResult;
            tsp00_KeyPtr        pIndexKey;
            tsp00_KeyPtr        pSubNodeKey;
            tsp00_Int2          indexKeyLength;
            tsp00_Int2          subNodeKeyLength;
            
            Node1.bd600GetKey (RecIndex1,            pIndexKey,   indexKeyLength);
            Node2.bd600GetKey (FIRST_REC_INDEX_BD00, pSubNodeKey, subNodeKeyLength);
            
            s30cmp( pIndexKey,   POS_OFF_DIFF_BD00, indexKeyLength,
                    pSubNodeKey, POS_OFF_DIFF_BD00, subNodeKeyLength,
                    compResult);
            
            if( l_greater == compResult ){
                TrError = e_illegal_key; // invalid key order
            }
            else if( RecIndex1 +1 <= Node1.bd600MaxRecIndex())
            {
                Node1.bd600GetKey (RecIndex1 + 1,            pIndexKey,   indexKeyLength);
                Node2.bd600GetKey (Node2.bd600MaxRecIndex(), pSubNodeKey, subNodeKeyLength);
                
                s30cmp( pSubNodeKey, POS_OFF_DIFF_BD00, subNodeKeyLength,
                        pIndexKey,   POS_OFF_DIFF_BD00, indexKeyLength,
                        compResult);
                
                if( l_less != compResult )
                    TrError = e_illegal_key; // invalid key order
            }
            if( e_ok != TrError )
            {
                bd400_VerifyError (TrError, Node1, Node2, Node1.bd600RecOffset( RecIndex1 )+1);
                return;
            }
        }

        if ( Node2.bd600IsLeaf() && ! bIsSubTree )
        {
            bd400_CheckSubTreesOfCurrentNode (*AuxCurrent, Node2, bIsCold, bWithExtendedCheck, NumPages, counter);

            if ( e_ok != TrError ) 
                return;
        }

        if ( RecIndex1 < Node1.bd600MaxRecIndex() )
            ++RecIndex1;
        else
        {
            NumPages += 1;
            if (bIsCold)
                converter.MarkPageNoAsUsed (task, Node1.bd600GetId(), Data_Dynamic);


            Tree1.bd500NextNode();
            if ( e_ok != TrError ) 
                return;
            RecIndex1 = FIRST_REC_INDEX_BD00;
        }

        if ( Node2.bd600IsLeaf() )
        {
            NumPages += 1;
            if ( bIsCold )
                converter.MarkPageNoAsUsed (task, Node2.bd600GetId(), Data_Dynamic);

            ++InvCurrent.currLeafPageCountDelta_bd00;
        }
        else
            ++InvCurrent.currIndexPageCountDelta_bd00;

        if ( InvCurrent.currExtendedFileTypeSet_bd00.includes(eftsClustered_egg00) )
        {
            if ( leftNeighborId != NIL_PAGE_NO_GG00
                 &&
                 leftNeighborId != Node2.bd600LeftNeighbor() )
            {
                TrError = e_illegal_page_no;
                bd400_VerifyError (TrError, Node1, Node2, leftNeighborId);
                g01abort (csp3_bd_msg, csp3_n_btree, "Invalid Left Neighbor   ", leftNeighborId);
            }
            leftNeighborId = Node2.bd600RightNeighbor() != NIL_PAGE_NO_GG00 ? Node2.bd600GetId() : NIL_PAGE_NO_GG00;
        }
        
        Tree2.bd500NextNode();
        if ( e_ok != TrError ) 
            return;
    }

    if ( ! Node1.bd600IsAccessable() || ! Node1.bd600IsLeaf() || Node2.bd600IsAccessable() )
    {
        if ( Node2.bd600IsAccessable() && Node2.bd600IsLeaf() )
            TrError = e_invalid_leaves_structure;
        else
            TrError = e_invalid_index_structure;
        
        bd400_VerifyError (TrError, Node1, Node2, 0);
        return;
    }
}

/*---------------------------------------------------------------------------*/

static inline void
bd400_ConstructSubTreeRecord (tsp00_KeyPtr      pKeySpace,
                              tsp00_KeyPtr      pKey,
                              tsp00_Int4        KeyLen,
                              tgg00_BasisError &TrError)
{
    SAPDB_RangeMove( __FILE__,   2,
           sizeof(tsp00_Key), sizeof(tbd_node),
           pKey,      POS_OFF_DIFF_BD00,
           pKeySpace, POS_OFF_DIFF_BD00,
           KeyLen,    TrError);
}

/*---------------------------------------------------------------------------*/

static inline void
bd400_ConstructSubTreeRecord (bool              ConsistentReadSupport,
                              tgg91_TransNo     &TransNo,
                              tsp00_KeyPtr      pKeySpace,
                              tsp00_KeyPtr      pKey,
                              tsp00_Int4        KeyLen,
                              tgg00_BasisError &TrError)
{
    bd400_ConstructSubTreeRecord (pKeySpace, pKey, KeyLen, TrError);

    if ( ConsistentReadSupport && e_ok == TrError )
    {
        for (int ix = 0; ix < sizeof(TransNo); ++ix)
            *(pKeySpace+KeyLen+ix) = 255 - TransNo[ix];
    }
}

/*---------------------------------------------------------------------------*/

static inline void

bd400_InvListArrayOverflow (cbd500_Tree         &InvTree,
                            cbd600_Node         &LeafNode,
                            cbd500_Tree         &SubTree,
                            cbd601_SubTreeNode  &SubTreeLeaf,
                            tsp00_Int4           SecKeyRecIndex,
                            tsp00_KeyPtr         pNewPrimKey,
                            tsp00_Int4           NewPrimKeyLen,
                            tsp00_Int4           PrimKeyIndex,
                            bool                 ConsistentReadSupport)
{
    ROUTINE_DBG_MEO00 ("bd400_InvListArrayOverflow");

    tgg00_BasisError    &TrError    = InvTree.bd500GetTheCurrent().curr_trans->trError_gg00;
    tgg00_RecPtr         pSecKeyRec = LeafNode.bd600RecPtr (SecKeyRecIndex);
    cbd450_InvListArray  InvListArray      (ConsistentReadSupport, pSecKeyRec);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01moveobj (bi, pSecKeyRec, POS_OFF_DIFF_BD00, pSecKeyRec->recLen_gg00());
#   endif

    /* copy all entries from the current InvListArray to the root page of a subtree */

    tsp00_Int4     RequiredLen;
    tsp00_Int2     PrimKeyLen;
    tsp00_Int4     PrimKeyRecIndex = FIRST_REC_INDEX_BD00;
    tsp00_BytePtr  pPrimKeySpace;
    tsp00_KeyPtr   pPrimKey;
    tsp00_Int4     iPKIndex;

    /* copy all prim keys less than the new prim key into the subroot */

    for (iPKIndex = FIRST_PRIM_KEY_INDEX_BD450; iPKIndex < pSecKeyRec->recNumPrimKeys_gg00(); ++iPKIndex)
    {
        if (iPKIndex == PrimKeyIndex)
        {
            /* insert the new prim key into the subroot */
            PrimKeyLen  = NewPrimKeyLen;
            pPrimKey    = pNewPrimKey;
            RequiredLen = PrimKeyLen + POINTERSIZE_BD00;

            if (ConsistentReadSupport)
                RequiredLen += sizeof(tgg91_TransNo);

            SubTreeLeaf.bd600AddRecordSpace (RequiredLen, PrimKeyRecIndex, pPrimKeySpace, ConsistentReadSupport);
            if ( e_no_more_space == TrError )
            {
                TrError = e_ok;
                SubTree.bd520LeafOverflow (pPrimKey, PrimKeyLen,
                                           FOR_INSERT_GBD500, RequiredLen, PrimKeyRecIndex);
                if (TrError != e_ok) return;
                SubTreeLeaf.bd600AddRecordSpace (RequiredLen, PrimKeyRecIndex, pPrimKeySpace, ConsistentReadSupport);
            }
            if (TrError != e_ok) return;
            ++PrimKeyRecIndex;

            bd400_ConstructSubTreeRecord (ConsistentReadSupport,
                                          InvTree.bd500GetTheCurrent().curr_trans->trWriteTransId_gg00, 
                                          pPrimKeySpace, pPrimKey, PrimKeyLen, TrError);
            if (TrError != e_ok) return;
        }

        InvListArray.bd450GetPrimKey (iPKIndex, pPrimKey, PrimKeyLen);
        if(e_ok != TrError) return;

#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01moveobj (bi, pPrimKey, POS_OFF_DIFF_BD00, PrimKeyLen);
#       endif

        RequiredLen = PrimKeyLen + POINTERSIZE_BD00;
        SubTreeLeaf.bd600AddRecordSpace (RequiredLen, PrimKeyRecIndex, pPrimKeySpace, true);
        if ( e_no_more_space == TrError )
        {
            TrError = e_ok;
            SubTree.bd520LeafOverflow (pPrimKey, PrimKeyLen,
                                       FOR_INSERT_GBD500, RequiredLen, PrimKeyRecIndex);
            if (TrError != e_ok) return;
            
            SubTreeLeaf.bd600AddRecordSpace (RequiredLen, PrimKeyRecIndex, pPrimKeySpace, true);
        }
        if(e_ok != TrError) return;
        ++PrimKeyRecIndex;

        bd400_ConstructSubTreeRecord (pPrimKeySpace, pPrimKey, PrimKeyLen, TrError);
        if (TrError != e_ok) return;
    }

    if ( (e_ok == TrError) && (PrimKeyIndex == pSecKeyRec->recNumPrimKeys_gg00()) )
    {
        /* insert the new prim key into the subroot */
        PrimKeyLen  = NewPrimKeyLen;
        pPrimKey    = pNewPrimKey;
        RequiredLen = PrimKeyLen + POINTERSIZE_BD00;

        if (ConsistentReadSupport)
            RequiredLen += sizeof(tgg91_TransNo);

        SubTreeLeaf.bd600AddRecordSpace (RequiredLen, PrimKeyRecIndex, pPrimKeySpace, ConsistentReadSupport);
        if ( e_no_more_space == TrError )
        {
            TrError = e_ok;
            SubTree.bd520LeafOverflow (pPrimKey, PrimKeyLen,
                                       FOR_INSERT_GBD500, RequiredLen, PrimKeyRecIndex);
            if (TrError != e_ok) return;
            SubTreeLeaf.bd600AddRecordSpace (RequiredLen, PrimKeyRecIndex, pPrimKeySpace, ConsistentReadSupport);
        }
        if (TrError != e_ok) return;

        bd400_ConstructSubTreeRecord ( ConsistentReadSupport,
                                       InvTree.bd500GetTheCurrent().curr_trans->trWriteTransId_gg00,
                                       pPrimKeySpace, pPrimKey, PrimKeyLen, TrError);
        if (TrError != e_ok) return;
    }

    /* change the InvListArray record into a InvListRefSubTree record */
    {
        tsp00_KeyPtr pKey      = tsp00_KeyPtr(tsp00_BytePtr(pSecKeyRec)+cgg_rec_key_offset);
        tsp00_Int4   OldRecLen = pSecKeyRec->recLen_gg00();
        cbd450_InvListRefSubTree InvListRefSubTreeHandle (InvListArray,
                SubTree.bd500GetTheCurrent().bd300GetRootId(),
                SubTree.bd500GetTheCurrent().bd300GetRootLeafCount());

        LeafNode.bd600UpdRecordSpace (pSecKeyRec->recLen_gg00() - OldRecLen, SecKeyRecIndex, pSecKeyRec);

        LeafNode.bd600LeafCount() += InvListRefSubTreeHandle.bd450NumLeaves();
    }
}


/*---------------------------------------------------------------------------*/

static inline void
bd400_SubRootUnderflow (cbd300_InvCurrent  &InvCurrent,
                        cbd600_Node        &LeafNode,
                        cbd601_SubTreeNode &SubTreeRootNode,
                        tsp00_Int4         &SecKeyRecIndex,
                        tsp00_Int4          PrimKeyRecIndex,
                        tsp00_Int4          RecSizeDelta)
{
    ROUTINE_DBG_MEO00 ("bd400_SubRootUnderflow");

    tgg00_BasisError &TrError   = InvCurrent.curr_trans->trError_gg00;
    tgg00_RecPtr      pRec      = LeafNode.bd600RecPtr(SecKeyRecIndex);
    tsp00_KeyPtr      pKey      = tsp00_KeyPtr(&(pRec->recKey_gg00().keyVal_gg00()));
    tsp00_Int2       &RecLen    = pRec->recLen_gg00();
    const tsp00_Int4  NewRecLen = RecLen + RecSizeDelta;
    const bool        ConvertToInvListArray = true;

    LeafNode.bd600UpdRecordSpace (RecSizeDelta, SecKeyRecIndex, pRec);
    if (e_ok != TrError) return;

    /* construct new InvListArray using the existing InvListRefSubtreeRecord */

    const bool ConsistentReadSupport = true;
    // don't treat the array as consistent because all keys already contain consistentcy info
    cbd450_InvListArray InvListArray (!ConsistentReadSupport, pRec, ConvertToInvListArray);

    /* copy each record of the subroot into the InvListArray except the one to be deleted */
    tsp00_Int4 InvListArrayIndex = FIRST_PRIM_KEY_INDEX_BD450;

    for (tsp00_Int4 SubRootRecIndex   = FIRST_REC_INDEX_BD00;
            SubRootRecIndex <= SubTreeRootNode.bd600MaxRecIndex();
            ++SubRootRecIndex)
    {
        if (SubRootRecIndex != PrimKeyRecIndex)
        {
            tsp00_Int2   PrimKeyLength;
            SubTreeRootNode.bd600GetKey(SubRootRecIndex, pKey, PrimKeyLength);

            InvListArray.bd450InsertPrimKey (*InvCurrent.curr_trans, InvListArrayIndex,
                                             pKey, PrimKeyLength);

            if (e_ok != TrError) break;
            ++InvListArrayIndex;
        }
    }

    if ( NewRecLen != RecLen ) // RecLen is automatically updated by bd450InsertPrimKey
    {
        TrError = e_data_page_corrupted;
        g01opmsg (sp3p_knldiag, sp3m_error, bd600c1DataPageCorrupted_csp03,
                  csp3_n_btree, "400 Data page corrupted ", LeafNode.bd600GetId());
        g01opmsg (sp3p_knldiag, sp3m_error, bd600c1DataPageCorrupted_csp03,
                  csp3_n_btree, "SubRoot                 ", SubTreeRootNode.bd600GetId());
        g01opmsg (sp3p_knldiag, sp3m_error, bd600c1DataPageCorrupted_csp03,
                  csp3_n_btree, "NewRecLen               ", NewRecLen);
        b06dump_bad_page (InvCurrent.curr_trans->trTaskId_gg00, 'd',
                          FILE_EXT_COR_BD00, LeafNode.bd600GetId(), LeafNode.bd600GetNodePtr(), 1);
        b06write_filename_and_root (InvCurrent.curr_tree_id);
    }
}

/*---------------------------------------------------------------------------*/

static inline void
bd400_CreateNewInvList (cbd500_Tree  &InvTree,
                        cbd600_Node  &LeafNode,
                        tsp00_Int4   &RecIndex,
                        tsp00_KeyPtr  pSecKey,
                        tsp00_Int4    SecKeyLen,
                        tsp00_KeyPtr  pPrimKey,
                        tsp00_Int4    PrimKeyLen,
                        bool          bPropagationNeeded, /* PTS 1106992 UH 2000-06-21 */
                        bool          bConsistentReadSupport)
{
    ROUTINE_DBG_MEO00 ("bd400_CreateNewInvList");

    const tsp00_Int4 RequiredLen =
        cbd450_InvListArray::bd450NewSize (bConsistentReadSupport, SecKeyLen, PrimKeyLen) + POINTERSIZE_BD00;

    if ( ! LeafNode.bd600SpaceIsAvailable (RequiredLen) )
    {
        if ( InvTree.bd520LeafOverflow (pSecKey, SecKeyLen, FOR_INSERT_GBD500, RequiredLen, RecIndex)
                != ANYTHING_DONE )
        {
            /* PTS 1106992 UH 2000-06-21 */
            if ( e_ok != LeafNode.bd600TrError() ) return;
            if ( bPropagationNeeded )
                InvTree.bd520LeafCountPropagation ();
        }
        if ( e_ok != LeafNode.bd600TrError() ) return;
    }

    tsp00_BytePtr pSpace;

    LeafNode.bd600AddRecordSpace (RequiredLen, RecIndex, pSpace, false);
    if ( e_ok != LeafNode.bd600TrError() ) return;

    cbd450_InvListArray NewInvListArray (*(InvTree.bd500GetTheCurrent().curr_trans), bConsistentReadSupport,
                                         reinterpret_cast<tgg00_RecPtr>(pSpace),
                                         pSecKey, SecKeyLen, pPrimKey, PrimKeyLen);
}

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/

void
cbd400_InvTree::bd400AppendPrimKey (tsp00_KeyPtr  pSecKey,
                                    tsp00_Int4    SecKeyLen,
                                    tsp00_KeyPtr  pPrimKey,
                                    tsp00_Int4    PrimKeyLen,
                                    bool          bNextSecKey,       /* PTS 1104438 UH 03-12-1999 */
                                    bool          bInvListRefNeeded, /* PTS 1104438 UH 03-12-1999 */
                                    tsp00_Int4   &RecIndex)
{
    /* PTS 1104438 UH 03-12-1999 rewritten */

    ROUTINE_DBG_MEO00 ("bd400AppendPrimKey");

    if ( g01glob.treecheck )
    {
        // PTS 1109967 UH 2001-04-09
        if ( bd500GetTheCurrent().curr_action != m_create_index_parallel )
            g01abort (csp3_bd_msg, csp3_n_btree,
                      "CreIdxPar: wrong action.",
                      bd500GetTheCurrent().curr_action);
    }

    tgg00_RecPtr pRec; // InvListRecord

    if ( nr_for_update != m_Node.m_Current.bd300RootRequestKind() )
    {
        m_TrError = e_lock_dirty;
        return;
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01sname   (bd_inv, "SecKey      ");
    t01moveobj (bd_inv, pSecKey,  POS_OFF_DIFF_BD00, SecKeyLen );
    t01sname   (bd_inv, "PrimKey     ");
    t01moveobj (bd_inv, pPrimKey, POS_OFF_DIFF_BD00, PrimKeyLen);
    t01bool    (bd_inv, "NextSecKey  ", bNextSecKey);
    t01bool    (bd_inv, "RefNeeded   ", bInvListRefNeeded);
    t01int4    (bd_inv, "RecIndex    ", RecIndex);
#   endif

    if ( NIL_RECINDEX_BD00 == RecIndex )
    {
        /* determine RecIndex => search InvList */

        if ( ! m_Node.bd600IsAccessable() ||
                ! m_Node.bd600IsLeaf()       ||
                (m_Node.m_pNode->nd_right() != NIL_PAGE_NO_GG00) )
        {
            bd500FindNode (pSecKey, SecKeyLen, LEAF_LEVEL_BD00);
            if (m_TrError != e_ok) return;
        }

        tbd_searchresult SecKeySearchResult;
        SecKeySearchResult.becomes  (nonefound);
        m_Node.bd600SearchRecord (pSecKey, SecKeyLen, RecIndex, SecKeySearchResult);

        if ( thisfound == SecKeySearchResult )
        {
            pRec = m_Node.bd600RecPtr (RecIndex);
        }
        else if ( nonefound == SecKeySearchResult )
        {
            /* if no record is found RecIndex is set to FIRST_REC_INDEX */
            --RecIndex;
        }
        else if ( nextfound == SecKeySearchResult )
        {
            /* this must not happen => SecKey order is wrong */
            m_TrError = e_not_implemented;
            return;
        }
        else if ( lastfound == SecKeySearchResult )
        {
            /* nothing to do, RecIndex is incremented by bNexSecKey */
        }
    }

    /* PRE: RecIndex is is set to last InvListRecord                            */
    /*      m_Node is pointing to the correct node = rightmost leaf in the tree */

    if (g01glob.treecheck)
    {
        tsp00_Int4 MaxRecIndex = m_Node.bd600MaxRecIndex();

#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        t01p2int4 (bd_inv, "RecIndex    ", RecIndex, "MaxRecIndex ", MaxRecIndex);
#       endif

        if ( m_Node.m_pNode->nd_right() != NIL_PAGE_NO_GG00
                ||
                ((m_Node.m_pNode->nd_record_cnt() > 0) && (RecIndex != MaxRecIndex)) )
        {
#           if COMPILEMODE_MEO00 >= SLOW_MEO00
            t01p2int4      (bd_inv, "right       ", m_Node.m_pNode->nd_right(),
                            "record_cnt  ", m_Node.m_pNode->nd_record_cnt());
            t01basis_error (bd_inv, "AppendPrimKe", m_TrError);
#           endif
            g01abort (csp3_bd_msg, csp3_n_btree, "CreIdxPar: wrong RecInd.", RecIndex);
            m_TrError = e_not_implemented;
            return;
        }
    }

    if ( bNextSecKey )
    {
        /* a new InvList must be created */
        
        if ( bInvListRefNeeded )
        {
            const tsp00_Int4 ReqiredLen =
                cbd450_InvListRefSubTree::bd450NewSize (SecKeyLen);

            bd530AppendRecordSpace (pSecKey, SecKeyLen, ReqiredLen, reinterpret_cast<tsp00_BytePtr&>(pRec));
            if (e_ok != m_TrError) return;

            RecIndex = m_Node.bd600MaxRecIndex();

            /* create a new SubtreeRef record */
            cbd450_InvListRefSubTree InvList (*m_Node.m_Current.curr_trans,
                                              pRec, pSecKey, SecKeyLen,
                                              NIL_PAGE_NO_GG00 /* SubTreeRoot */,
                                              1                /* NumPrimKeys */, 	// PTS 1131150 UH initialize with 1 instead of 0
                                              1                /* NumLeaves   */);

            ++(m_Node.m_pNode->ndLeafCount_bd00());

            bd400_SetToNewSubRoot ();
            if (e_ok != m_TrError) return;

            /* assign SubRootId to InvListReferenceRecord */
            InvList.bd450SetSubRoot (m_SubTreeLeaf.bd600GetId());

            /* create a new PrimKeyRecord */
            tsp00_KeyPtr pPrimKeySpace;

            m_SubTree.bd530AppendRecordSpace (pPrimKey, PrimKeyLen, PrimKeyLen, pPrimKeySpace);
            if (e_ok != m_TrError) return;
            bd400_ConstructSubTreeRecord (pPrimKeySpace, pPrimKey, PrimKeyLen, m_TrError);
        }
        else
        {
            ++RecIndex;
            bd400_CreateNewInvList (*this, m_Node, RecIndex,
                                    pSecKey, SecKeyLen, pPrimKey, PrimKeyLen, ! PROPAGATION_NEEDED,
                                    m_SubTreeCurrent.bd300ConsistentReadSupport());
        }
        return;
    }

    /* RecIndex is given, so m_Node is the correct/last LeafNode (RightNeighbor==nil) */
    /* The record may be an InvListArray or the last PrimKey in a SubTree.            */
    /* The append operation in the meaning of the words is done in the following.     */

    pRec = m_Node.bd600RecPtr(RecIndex);

    /* Check, if the SecKey is given ascending */
    if ( g01glob.treecheck ) // PTS 1109967 UH 2001-04-09
    {
        tsp00_LcompResult CompResult;

        s30cmp (pSecKey, POS_OFF_DIFF_BD00, SecKeyLen,
                pRec, POS_OFF_DIFF_BD00 + cgg_rec_key_offset,
                pRec->recKeyLen_gg00(),    CompResult);

        if ( l_less == CompResult )
        {
#           if COMPILEMODE_MEO00 >= SLOW_MEO00
            t01int4        (bd_inv, "CompResult  ", CompResult);
            t01basis_error (bd_inv, "AppendPrimKe", m_TrError);
            t01sname       (bd_inv, "SecKey      ");
            t01moveobj     (bd_inv, pSecKey, POS_OFF_DIFF_BD00, SecKeyLen );
            t01sname       (bd_inv, "RecKey      ");
            t01moveobj     (bd_inv, pRec, POS_OFF_DIFF_BD00 + cgg_rec_key_offset, pRec->recKeyLen_gg00());
#           endif
            g01abort (csp3_bd_msg, csp3_n_btree, "CreIdxPar: wrong SecKey ", RecIndex);
            m_TrError = e_not_implemented;
            return;
        }
    }

    if ( ilkSubTree_egg00 == pRec->recInvListKind_gg00() )
    {
        /* create new PrimKeyRecord */

        tsp00_KeyPtr pPrimKeySpace;

        m_SubTree.bd530AppendRecordSpace (pPrimKey, PrimKeyLen, PrimKeyLen, pPrimKeySpace);
        if (e_ok != m_TrError) return;
        bd400_ConstructSubTreeRecord     (pPrimKeySpace, pPrimKey, PrimKeyLen, m_TrError);

        /* update InvList-Record in InvTreeLeaf */
        cbd450_InvListRefSubTree InvList (pRec);
        InvList.bd450IncrNumPrimKeys ();

        /* update Leafcount Info in InvTreeLeaf */
        if ( InvList.bd450NumLeaves() < m_SubTree.bd500LeafCount() )
        {
            m_Node.m_pNode->ndLeafCount_bd00() -= InvList.bd450NumLeaves();
            m_Node.m_pNode->ndLeafCount_bd00() += m_SubTree.bd500LeafCount();
            InvList.bd450SetNumLeaves (m_SubTree.bd500LeafCount());
            /* it is propagated with a next AppendNode */
        }
    }
    else
    {
        /* append PrimKey to InvListArray */

        cbd450_InvListArray InvListArray (m_SubTreeCurrent.bd300ConsistentReadSupport(), pRec);

        if ( InvListArray.bd450Size (PrimKeyLen) <= MAX_INVLISTLENGTH_BD00 )
        {
            const tsp00_Int4 RecSizeDelta = InvListArray.bd450Size(PrimKeyLen) - pRec->recLen_gg00();

            if ( ! m_Node.bd600SpaceIsAvailable (RecSizeDelta) )
            {
                bd520LeafOverflow (pSecKey, SecKeyLen, ! FOR_INSERT_GBD500, RecSizeDelta, RecIndex);
                if (e_ok != m_TrError) return;
            }
            pRec = m_Node.bd600RecPtr (RecIndex);
            m_Node.bd600UpdRecordSpace (RecSizeDelta, RecIndex, pRec);
            if (e_ok != m_TrError) return;

            InvListArray.bd450Update (pRec);
            InvListArray.bd450InsertPrimKey (*(m_Node.m_Current.curr_trans),
                                             pRec->recNumPrimKeys_gg00(), pPrimKey, PrimKeyLen);
        }
        else
        {
            bd400_SetToNewSubRoot ();
            if (e_ok != m_TrError) return;
            bd400_InvListArrayOverflow (*this, m_Node, m_SubTree, m_SubTreeLeaf, RecIndex,
                                        pPrimKey, PrimKeyLen, pRec->recNumPrimKeys_gg00(),
                                        m_SubTreeCurrent.bd300ConsistentReadSupport());
            /* no UnderFlowHandling for the InvTree, more appends are assumed */
        }
    }
}

/*===========================================================================*
 *  OMS KEY INTERFACE                                                        *
 *===========================================================================*/

externCpp void
bd400AddToOmsInvTree (cbd300_InvCurrent &InvCurrent,
                      tsp00_KeyPtr       pSecKey,
                      tsp00_Int4         SecKeyLen,
                      tsp00_KeyPtr       pPrimKey,
                      tsp00_Int4         PrimKeyLen,
                      tsp00_Int4         FoundPrimKeySize,
                      tsp00_KeyPtr       pFoundPrimKey,
                      tsp00_Int4        &FoundPrimKeyLen)

{
    ROUTINE_DBG_MEO00 ("bd400AddToOmsInvTree");

    tgg00_BasisError    &TrError = InvCurrent.curr_trans->trError_gg00;
    tbd_searchresult     SecKeySearchResult;
    tsp00_Int4           RecIndex;
    cbd600_Node          LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_update));
    cbd500_Tree          Tree     (LeafNode);

    /* search secondary key in index */
    Tree.bd500FindNode (pSecKey, SecKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    LeafNode.bd600SearchRecord  (pSecKey, SecKeyLen, RecIndex, SecKeySearchResult);

    /* check if the new record suits into current page */
    if ( (thisfound != SecKeySearchResult)
            &&
            ! LeafNode.bd600SpaceIsAvailable
            (cbd450_InvListArray::bd450NewSize (InvCurrent.bd300ConsistentReadSupport(), SecKeyLen, PrimKeyLen) + POINTERSIZE_BD00) )
    {
        /* release current Leaf Node */
        LeafNode.bd600Release (! IS_CHANGED_BD600);

        /* get exclusive tree lock if the record does not suit into current page */
        if ( InvCurrent.bd300LockTreeExclusive() )
            if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                Tree.bd500_WriteTrace("bd400AddToOmsInvTree: Overflow",30);
        if (TrError != e_ok) return;

        Tree.bd500FindNode (pSecKey, SecKeyLen, LEAF_LEVEL_BD00);
        if (TrError != e_ok) return;

        LeafNode.bd600SearchRecord (pSecKey, SecKeyLen, RecIndex, SecKeySearchResult);
    }

    if ( thisfound == SecKeySearchResult )
    {
        tgg00_RecPtr pRec = LeafNode.bd600RecPtr(RecIndex);

        if ( (pRec->recNumPrimKeys_gg00() > 1) ||
                (ilkSubTree_egg00 == pRec->recInvListKind_gg00()) )
        {
            TrError = e_not_implemented;
            LeafNode.bd600Dump (csp3_bd_msg, "OMS KEY not unique      ");
            return;
        }

        tsp00_KeyPtr        pAuxPrimKey;
        cbd450_InvListArray InvListArray (InvCurrent.bd300ConsistentReadSupport(), pRec);
        tsp00_Int2          AuxPrimKeyLen;

        InvListArray.bd450GetPrimKey (FIRST_PRIM_KEY_INDEX_BD450, pAuxPrimKey, AuxPrimKeyLen);
        FoundPrimKeyLen = AuxPrimKeyLen;

        SAPDB_RangeMove( __FILE__,   3,
               AuxPrimKeyLen,   FoundPrimKeySize,
               pAuxPrimKey,     POS_OFF_DIFF_BD00,
               pFoundPrimKey,   POS_OFF_DIFF_BD00,
               FoundPrimKeyLen, TrError);
        if ( e_ok != TrError ) return;

        /* OMS Keys are forced to be unique */
        TrError = e_duplicate_invkey;
        return;
    }

    if ( lastfound == SecKeySearchResult )
        ++RecIndex;

    bd400_CreateNewInvList (Tree, LeafNode, RecIndex, pSecKey, SecKeyLen, pPrimKey, PrimKeyLen,
                            ! PROPAGATION_NEEDED, InvCurrent.bd300ConsistentReadSupport());
}

/*---------------------------------------------------------------------------*/
/* conditional delete - PTS 1131960 FF */
externCpp void
bd400DelFromOmsInvTree (cbd300_InvCurrent &InvCurrent,
                        tsp00_KeyPtr       pSecKey,
                        tsp00_Int4         SecKeyLen,
                        tsp00_KeyPtr       pPrimKey,
                        tsp00_Int4         PrimKeyLen,
                        tsp00_Int4         FoundPrimKeySize,
                        tsp00_KeyPtr       pFoundPrimKey,
                        tsp00_Int4         FoundPrimKeyToCompareSize,
                        tsp00_Int4        &FoundPrimKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd400DelFromOmsInvTree");

    tgg00_BasisError     &TrError = InvCurrent.curr_trans->trError_gg00;
    tsp00_Int4            SecKeyIndex;
    tbd_searchresult      SecKeySearchResult;
    cbd600_Node           LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_update));
    cbd500_Tree           Tree     (LeafNode);

    Tree.bd500FindNode (pSecKey, SecKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    LeafNode.bd600SearchRecord (pSecKey, SecKeyLen, SecKeyIndex, SecKeySearchResult);

    if (thisfound != SecKeySearchResult)
    {
        TrError = e_key_not_found;
        return;
    }

    tsp00_KeyPtr        pAuxPrimKey;
    cbd450_InvListArray InvListArray (false, LeafNode.bd600RecPtr (SecKeyIndex));
    tsp00_Int2          AuxPrimKeyLen;
    tsp00_LcompResult   CompResult;

    InvListArray.bd450GetPrimKey (FIRST_PRIM_KEY_INDEX_BD450, pAuxPrimKey, AuxPrimKeyLen);
    FoundPrimKeyLen = AuxPrimKeyLen;

    s30cmp (pPrimKey, POS_OFF_DIFF_BD00, (FoundPrimKeyToCompareSize < PrimKeyLen ? FoundPrimKeyToCompareSize : PrimKeyLen),
        pAuxPrimKey, POS_OFF_DIFF_BD00,
        (FoundPrimKeyToCompareSize < FoundPrimKeyLen ? FoundPrimKeyToCompareSize : FoundPrimKeyLen), CompResult);
    if ( l_equal == CompResult )
    {
        LeafNode.bd600DelRecordSpace (SecKeyIndex);
        if (e_ok != TrError) return;

        Tree.bd520LeafUnderFlow (pSecKey, SecKeyLen);
    }
    else
    {
        SAPDB_RangeMove( __FILE__, 4,
            PrimKeyLen,  FoundPrimKeySize,
            pAuxPrimKey, POS_OFF_DIFF_BD00,
            pFoundPrimKey, POS_OFF_DIFF_BD00,
            FoundPrimKeyLen,  TrError);
        TrError = e_primkey_does_not_match;
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd400GetFromOmsInvTree (cbd300_InvCurrent   &InvCurrent,
                        tsp00_KeyPtr         pSecKey,
                        tsp00_Int4           SecKeyLen,
                        tsp00_Int4           PrimKeySize,
                        tsp00_KeyPtr         pPrimKey,
                        tsp00_Int4          &PrimKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd400GetFromOmsInvTree ");

    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;

    tbd_searchresult    SecKeySearchResult;
    tsp00_Int4          SecKeyIndex;
    cbd600_Node         LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree         Tree     (LeafNode);

    Tree.bd500FindNode (pSecKey, SecKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    LeafNode.bd600SearchRecord (pSecKey, SecKeyLen, SecKeyIndex, SecKeySearchResult);

    if (thisfound != SecKeySearchResult)
    {
        TrError = e_key_not_found;
        return;
    }

    tsp00_KeyPtr        pAuxPrimKey;
    cbd450_InvListArray InvListArray (InvCurrent.bd300ConsistentReadSupport(), LeafNode.bd600RecPtr (SecKeyIndex));
    tsp00_Int2          AuxPrimKeyLen;

    InvListArray.bd450GetPrimKey (FIRST_PRIM_KEY_INDEX_BD450, pAuxPrimKey, AuxPrimKeyLen);
    PrimKeyLen = AuxPrimKeyLen;

    SAPDB_RangeMove( __FILE__,   5,
           PrimKeyLen,  PrimKeySize,
           pAuxPrimKey, POS_OFF_DIFF_BD00,
           pPrimKey,    POS_OFF_DIFF_BD00,
           PrimKeyLen,  TrError);
}
