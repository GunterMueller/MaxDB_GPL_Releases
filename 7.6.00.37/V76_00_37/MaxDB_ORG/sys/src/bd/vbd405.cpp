/****************************************************************************

  module      : vbd405.cpp

  -------------------------------------------------------------------------

  author      : TorstenS
  responsible : TorstenS

  special area: Inverted Lists
  description : Create/Diagnose Index


  last changed: 1999-08-18  19:12
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



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


*****************************************************************************/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files
#include "hbd405.h"

#include "gbd450.h"  // CPP   : InvListRefSubTree Base Class
#include "gbd460.h"  // CPP   : InvListRefSubTree Iterator Class
#include "gbd494.h"  // CPP   : FunnelHandling
#include "gbd495.h"  // CPP   : DoubleQueueHandling
#include "gbd500.h"  // CPP   : Tree Handling
#include "gbd510.h"  // CPP   : RecordIterator
#include "gbd600.h"  // CPP   : Node Handling
#include "gbd490.h"  // CPP   : CreateIndexCoordinator
#include "gbd491.h"  // CPP   : CreateIndexParticipant
#include "ggg11.h"   // PASCAL: VTraceFormats
#include "gsp03.h"   // PASCAL: SP_message_constants_and_types
#include "gsp03_3.h" // PASCAL: SP_message_constants_and_types_for_bd_layer
/* */
#include "hbd13.h"   // PASCAL: Nodehandling
#include "hbd30.h"   // PASCAL: TreeHandling
#include "hbd400.h"  // CPP   : Inverted Lists Add/Delete
#include "hgg01_1.h" // PASCAL: Configuration_Parameter

#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "Trace/Trace_Entry.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

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

inline void
bd405_CheckInvListForOneSecKey (
    cbd300_InvCurrent      &InvCurrent,        // [IN]
    tbd_current_tree       &PrimCurrent,       // [IN]
    tbd_current_tree       &TempCurrent,       // [IN]
    cbd460_InvListIterator &PrimKeyIterator,   // [IN]
    tgg00_Rec              &RecBuffer,         // [IN]
    tgg00_RecPtr            pRec,              // [IN]
    tsp00_Int4             &PrimKeyDuplicates, // [OUT]
    tsp00_Int4             &PrimKeyMissing     // [OUT]
);

/*---------------------------------------------------------------------------*/

inline void
bd405_CreateTempRoot (tbd_current_tree  &InvCurrent);

/*---------------------------------------------------------------------------*/

inline bool bd405_IsFunctionBasedIndex(const tgg00_StackDesc &StackDesc);

/*---------------------------------------------------------------------------*/

static SAPDB_Bool bd405_CheckDoParallelCreateIndex(
    tgg00_TransContext      &Trans,            // [IN|OUT]
    tgg00_FileId            &PrimFileId,       // [IN|OUT]
    const tgg00_IndexMode   IndexMode,         // [IN]
    tgg00_StackDesc         &StackDesc         // [IN]
);

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp void
bd405DiagnoseIndex (
    cbd300_InvCurrent      &InvCurrent,        // [IN]
    tbd_current_tree       &PrimCurrent,       // [IN]
    tbd_current_tree       &TempCurrent,       // [IN]
    tgg00_Rec              &RecBuffer,         // [IN]
    tsp00_Int4             &PrimKeyDuplicates, // [OUT]
    tsp00_Int4             &PrimKeyMissing     // [OUT]
)
{
    ROUTINE_DBG_MEO00 ("bd405DiagnoseIndex");

    tgg00_BasisError   &TrError = InvCurrent.curr_trans->trError_gg00;
    /* */
    const bool          bAscendingKeyOrder   = true;
    const bool          bIncludeFirstPrimKey = true;
    const bool          bIncludeFirstSecKey  = true;
    bool                bUseFirstPrimKey     = true;
    /* */
    cbd600_Node         InvLeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree         InvTree (InvLeafNode);
    /* */
    tsp00_BytePtr       pStartSecKey  = NULL;
    tsp00_BytePtr       pStopSecKey   = NULL;
    tsp00_BytePtr       pStartPrimKey = NULL;
    tsp00_BytePtr       pStopPrimKey  = NULL;
    tgg00_RecPtr        pRec          = NULL;
    /* */
    tsp00_Int4          StartPrimKeyLen = 0;
    tsp00_Int4          StartSecKeyLen  = 0;
    tsp00_Int4          StopPrimKeyLen  = 0;
    tsp00_Int4          StopSecKeyLen   = 0;
    /* */
    PrimKeyDuplicates = 0;
    PrimKeyMissing    = 0;

    if (e_ok != TrError) return;

    cbd510_RecordIterator RecIter (InvTree, pStartSecKey, StartSecKeyLen, pStopSecKey,
                                   StopSecKeyLen, bAscendingKeyOrder, bIncludeFirstSecKey, bUseFirstPrimKey);
    if ( e_ok != TrError ) return;

    do
    {
        pRec = RecIter.bd510GetRecord();
        if (NULL == pRec)
        {
            TrError = e_ok;
            return; // In case of no next record
        }

        switch (pRec->recInvListKind_gg00())
        {
        case ilkArray_egg00 :
            {
                cbd460_InvListArrayIterator  PrimKeyIterator (pRec,
                        pStartPrimKey, StartPrimKeyLen, pStopPrimKey, StopPrimKeyLen,
                        bAscendingKeyOrder, bIncludeFirstPrimKey, InvCurrent.bd300ConsistentReadSupport());
                if (e_ok != TrError) return;

                bd405_CheckInvListForOneSecKey (InvCurrent, PrimCurrent, TempCurrent,
                                                PrimKeyIterator, RecBuffer, pRec, PrimKeyDuplicates, PrimKeyMissing);
                if (e_ok != TrError) return;
            }
            break;
        case ilkSubTree_egg00 :
            {
                cbd450_InvListRefSubTree InvListRefSubTree (pRec);

                cbd460_InvListSubtreeIteratorNoBreak  SubTreeIterator (InvTree, InvListRefSubTree,
                        pStartPrimKey, StartPrimKeyLen, pStopPrimKey , StopPrimKeyLen,
                        bAscendingKeyOrder, bIncludeFirstPrimKey);
                if (e_ok != TrError) return;

                bd405_CheckInvListForOneSecKey (InvCurrent, PrimCurrent, TempCurrent,
                                                SubTreeIterator, RecBuffer, pRec, PrimKeyDuplicates, PrimKeyMissing);
                if (e_ok != TrError) return;
            }
            break;
        default:
            TrError = e_invalid_invlistpos;

            InvLeafNode.bd600Dump (BD405_C1_INVALID_INVLIST_POS_SP03,
                                   "bd405DiagnoseIndex      ");
            return;
        }
        ++RecIter;
    }
    while (e_ok == TrError);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd405CreateIndex (
    cbd300_InvCurrent      &InvCurrent,        // [IN]
    tgg00_FileId           &PrimFileId,        // [IN]
    tgg00_StackDesc        &StackDesc,         // [IN]
    tgg00_IndexMode         IndexMode,         // [IN]
    tgg00_IndexCounter     &IndexCounter       // [OUT]
)
{
    ROUTINE_DBG_MEO00 ("bd405CreateIndex");


    Trans_Context       &transContext = Trans_Context::GetContext( *InvCurrent.curr_trans );
    tgg00_BasisError    &TrError      = transContext.OldTrans().trError_gg00;
      
    bool  bUseServerTasks = bd405_CheckDoParallelCreateIndex( transContext.OldTrans(), 
        PrimFileId, IndexMode, StackDesc );

    if(( e_ok == TrError ) && bUseServerTasks )
    {
        cbd600_Node		        LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_update));
        cbd494_SortableInvTree  InvTree (LeafNode, IndexCounter.idc_sec_keycount, IndexCounter.idc_sec_leafnodes);

        cbd490_CreateIndexParallel  Coordinator( transContext, InvTree,
                PrimFileId, StackDesc, IndexCounter, IndexMode );

        Coordinator.Initialize( transContext );

        if( e_ok == TrError ){
            Coordinator.BuildIndex( transContext );
        }
        if(( e_sysbuffer_overflow == TrError ) || ( e_no_more_memory == TrError ))
        {
            TrError         = e_ok;
            bUseServerTasks = false;
            RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_USE_SEQUENTIAL_1 ));
        }
    }
    if(( e_ok == TrError ) && ( ! bUseServerTasks ))
    {
        InvCurrent.curr_action.becomes( m_create_index );

        cbd490_CreateIndexSequential Coordinator( transContext, InvCurrent,
                PrimFileId, StackDesc, IndexCounter, IndexMode );

        Coordinator.BuildIndex( transContext );
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd405ParticipantCreateIndex (
    tbd_current_tree       &InvCurrent,        // [IN]
    tbd_current_tree       &PrimCurrent,       // [IN]
    tgg00_StackDesc        &StackDesc,         // [IN]
    tsp00_BytePtr           pStartPrimKey,     // [IN]
    tsp00_BytePtr           pStopPrimKey,      // [IN]
    tsp00_Int4              StartPrimKeyLen,   // [IN]
    tsp00_Int4              StopPrimKeyLen,    // [IN]
    tsp00_Int4              NumLeavesToScan,   // [IN]
    tsp00_Int4              AvgRecordsPerLeaf, // [IN]
    tsp00_PageNo            PrimQueuePno,      // [IN]
    tsp00_PageNo            SecQueuePno,       // [IN]
    tsp00_Bool              bUseMemorySort,    // [IN]
    tgg00_IndexMode         IndexMode,         // [IN]
    tgg00_IndexCounter     &IndexCounter,      // [OUT]
    tsp00_Int4             &TempLeafPageCount, // [OUT]
    tsp00_PageNo           &TempInvRoot,       // [OUT]
    tsp00_Bool             &bEntriesExist      // [OUT]
)
{
    ROUTINE_DBG_MEO00 ("bd405ParticipantCreateIndex");


    Trans_Context       &transContext      = Trans_Context::GetContext( *PrimCurrent.curr_trans );
    tgg00_BasisError    &TrError           = PrimCurrent.curr_trans->trError_gg00;
    bool                 bMemorySortFailed = false;

    if (bUseMemorySort)
    {
        cbd491_CreateIndexParticipant   Participant(transContext, PrimCurrent, StackDesc, IndexCounter );
        cbd495_DoubleWriteQueue         InvQueue (InvCurrent, PrimQueuePno, SecQueuePno);


        Participant.MemorySort( transContext, InvQueue, pStartPrimKey, pStopPrimKey, StartPrimKeyLen,
                                StopPrimKeyLen, NumLeavesToScan, AvgRecordsPerLeaf, bEntriesExist );

        if( e_ok == TrError ) 
        {
            TempLeafPageCount = InvQueue.bd495GetPageCount();
            // if ((0 == (PrimQueuePno % 2)) && (0 == SecQueuePno % 2))
            //     TrError = e_no_more_memory; // +++ TEST TEST
        }
        if (e_ok != TrError)
        {
            if(( e_sysbuffer_overflow == TrError ) || ( e_no_more_memory == TrError ))
            {
                bMemorySortFailed = true;
                RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_USE_SEQUENTIAL_1 ));
            }
            const tgg00_BasisError AuxError = TrError;
            TrError  = e_ok;

            // Drop all pages added during the memory sort. This will increase
            // the probability that the pages are still in the Data Cache. The
            // head pages will be deleted by the maintenance of the filehandling
            // structures carried out by the coordinator

            InvQueue.bd495Free (!INCLUSIVE_HEAD_PNO_BD495);

            if( e_sysbuffer_overflow == TrError )
            {
                RTE_Message( Data_Exception( __CONTEXT__, DATA_ERR_CREATE_INDEX_FREE_FAILED_1 ));
                return;
            }
            else if (e_ok == TrError)
            {
                if (bMemorySortFailed)
                    bd405_CreateTempRoot (InvCurrent);
                else
                    TrError = AuxError;
                if (e_ok != TrError) return;
            }
        }
    }

    if ((bMemorySortFailed) || (!bUseMemorySort))
    {
        cbd300_InvCurrent InvTreeCurrent (*PrimCurrent.curr_trans, InvCurrent.curr_tree_id,
                                          tbd_node_request::fromConst (nr_for_update),
                                          tgg00_MessType::fromConst (m_create_index), // PTS 1110414 TS 2001-05-29
                                          tgg00_VtraceType::fromConst (b03pcreate), !IGNORE_INV_ERROR_BD300);

        if (InvCurrent.curr_tree_id.fileType_gg00().includes (ftsTemp_egg00))
            InvTreeCurrent.bd300SkipErrorHandling();

        if (e_ok == TrError)
        {
            cbd491_CreateIndexParticipant Participant( transContext, PrimCurrent, StackDesc, IndexCounter, false);

            Participant.InsertionSort( transContext, InvTreeCurrent, IndexMode, pStartPrimKey,
                                       pStopPrimKey, StartPrimKeyLen, StopPrimKeyLen, bEntriesExist);

            if (bMemorySortFailed)
            {
                if (e_ok == TrError)
                    TempInvRoot = InvCurrent.curr_tree_id.fileRoot_gg00();
                else
                {
                    const tgg00_BasisError AuxError = TrError;
                    TrError = e_ok;
                    bd400DropInvTree (InvTreeCurrent);
                    TrError = AuxError; // Ignore DropInvTree errors
                }
            }
        }
        else if (bMemorySortFailed)
            bd13FreeNewRoot (InvCurrent);
    }
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

inline void
bd405_CheckInvListForOneSecKey (
    cbd300_InvCurrent      &InvCurrent,        // [IN]
    tbd_current_tree       &PrimCurrent,       // [IN]
    tbd_current_tree       &TempCurrent,       // [IN]
    cbd460_InvListIterator &PrimKeyIterator,   // [IN]
    tgg00_Rec              &RecBuffer,         // [IN]
    tgg00_RecPtr            pRec,              // [IN]
    tsp00_Int4             &PrimKeyDuplicates, // [OUT]
    tsp00_Int4             &PrimKeyMissing     // [OUT]
)
{
    const pasbool cCountDuplicates = true;
    tgg00_BasisError    &TrError = InvCurrent.curr_trans->trError_gg00;
    /* */
    tsp00_BytePtr       pPrimKey;
    tbd_node_ptrs       pPrimNptr;
    /* */
    tbd_neighbors       Neighbors;
    tsp00_Int2          PrimKeyLen;
    tsp00_Int4          PrimRecIndex;
    /* */
    tbd_searchresult    PrimKeySearchResult;
    /* */
    pPrimNptr.np_ptr()   = NULL;
    pPrimNptr.np_cbptr() = NULL;

    do
    {
        bool hasDeletedFlag;
        PrimKeyIterator.bd460Deref (pPrimKey, PrimKeyLen, hasDeletedFlag); // TOTO T.A.
        if (NULL == pPrimKey) return;
        if (hasDeletedFlag)
        {
            PrimKeyIterator.bd460SkipCurrentKey();
            continue;
        }
        if (InvCurrent.bd300ConsistentReadSupport())
        {
            PrimKeyLen -= sizeof(tgg91_TransNo);
        }

        bd30SearchRecord (PrimCurrent, pPrimKey, PrimKeyLen,
                          ! LOCK_LEAF_EXCL_BD00, pPrimNptr,
                          PrimRecIndex, Neighbors, PrimKeySearchResult);

        if (NULL != pPrimNptr.np_ptr())
        {
            b13r_release_node (pPrimNptr, PrimCurrent, lru_normal);
        }

        bd30ReleaseSubTree( PrimCurrent );

        if (e_ok != TrError) return;

        if (thisfound != PrimKeySearchResult)
        {
            ++PrimKeyMissing;
        }
        else
        {
            RecBuffer.recLen_gg00()          = PrimKeyLen + cgg_rec_key_offset;
            RecBuffer.recKeyLen_gg00()       = PrimKeyLen;
            RecBuffer.recVarcolOffset_gg00() = 0;
            RecBuffer.recVarcolCnt_gg00()    = 0;

            SAPDB_RangeMove( __FILE__, 1,
                             PERM_KEY_MXSP00, sizeof (RecBuffer.recBuf_gg00()),
                             pPrimKey,                POS_OFF_DIFF_BD00,
                             RecBuffer.recBuf_gg00(), cgg_rec_key_offset + POS_OFF_DIFF_BD00,
                             RecBuffer.recKeyLen_gg00(), TrError);
            if (e_ok != TrError) return;

            bd30AddToTempTree (! cCountDuplicates, RecBuffer, TempCurrent);

#           if COMPILEMODE_MEO00 >= SLOW_MEO00 
            t01basis_error (bi, "b405AddTemp ", TrError);
#           endif
        }

        if (
            ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)) &&
            ((thisfound != PrimKeySearchResult) || (e_duplicate_key == TrError))
        )
        {
            tgg00_BasisError        AuxError = e_ok;
            /* */
            tsp00_Int4      MaxVtraceKey = sizeof (tgg00_VtraceKey);
            tsp00_Int4      OutKeyLen;
            tsp00_Int4      SecKeyLen    = pRec->recKey_gg00().keyLen_gg00();
            /* */
            tsp00_BytePtr   pSecKey = REINTERPRET_CAST (tsp00_BytePtr, &pRec->recKey_gg00().keyVal_gg00());

            /* */
            tgg11_BdTwoKeysTrace    KeyTrace;


            KeyTrace.bdtkTrType_gg11.becomes(bdtrTwoKeys_egg11);
            KeyTrace.bdtkKeyLen_gg11 [0] = SecKeyLen;
            KeyTrace.bdtkKeyLen_gg11 [1] = PrimKeyLen;

            OutKeyLen = (SecKeyLen > MaxVtraceKey) ? MaxVtraceKey : SecKeyLen;

            SAPDB_RangeMove( __FILE__, 2,
                             PERM_KEY_MXSP00, sizeof(KeyTrace.bdtkKey_gg11 [0]),
                             pSecKey,                   POS_OFF_DIFF_BD00,
                             KeyTrace.bdtkKey_gg11 [0], POS_OFF_DIFF_BD00,
                             OutKeyLen, AuxError);
            AuxError = e_ok; // ignore move error

            OutKeyLen = (PrimKeyLen > MaxVtraceKey) ? MaxVtraceKey : PrimKeyLen;

            SAPDB_RangeMove( __FILE__, 3,
                             PERM_KEY_MXSP00, sizeof(KeyTrace.bdtkKey_gg11 [1]),
                             pPrimKey,                  POS_OFF_DIFF_BD00,
                             KeyTrace.bdtkKey_gg11 [1], POS_OFF_DIFF_BD00,
                             OutKeyLen, AuxError);
            AuxError = e_ok; // ignore move error

            Trace_CommonEntry entry(  tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03test),
                                      REINTERPRET_CAST( tsp00_BytePtr, &KeyTrace ), sizeof (KeyTrace) );
        }

        if (e_duplicate_key == TrError)
        {
            ++PrimKeyDuplicates;
            TrError = e_ok;
        }
        if (InvCurrent.bd300ConsistentReadSupport())
        {
            PrimKeyIterator.bd460SkipCurrentKey();
        }
        else
        {
            ++PrimKeyIterator;
        }
    }
    while (e_ok == TrError);
}

/*---------------------------------------------------------------------------*/

inline void
bd405_CreateTempRoot (tbd_current_tree  &InvCurrent)
{
    tgg00_BasisError    &TrError = InvCurrent.curr_trans->trError_gg00;
    tbd_node_ptrs       pRoot;

    pRoot.np_ptr()   = NULL;
    pRoot.np_cbptr() = NULL;

    b13new_root (pRoot, InvCurrent);

    if (e_ok != TrError)
    {
        if (NULL != pRoot.np_ptr())
            b13r_release_node (pRoot, InvCurrent, lru_normal);
        return;
    }
    InvCurrent.curr_tree_id.fileRoot_gg00() = pRoot.np_ptr()->nd_id();
    b13w_release_node (pRoot, InvCurrent);
}

/*---------------------------------------------------------------------------*/

inline bool bd405_IsFunctionBasedIndex(const tgg00_StackDesc &StackDesc)
{
    return (st_func == (*StackDesc.mst_addr())[StackDesc.mmult_pos-1].etype());
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool bd405_CheckDoParallelCreateIndex(
    tgg00_TransContext      &Trans,             // [IN|OUT]
    tgg00_FileId            &PrimFileId,        // [IN|OUT]
    const tgg00_IndexMode   IndexMode,          // [IN]
    tgg00_StackDesc         &StackDesc         // [IN]
)
{
    ROUTINE_DBG_MEO00 ("bd405_CheckDoParallelCreateIndex");

    tbd_current_tree   PrimCurrent;

    bd30GetTree( Trans, PrimFileId, PrimCurrent, m_create_index_parallel,
                 ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );

    if ( e_ok != Trans.trError_gg00 )
    {
        bd30ReleaseTree( PrimCurrent );
        return false;
    }
    PrimFileId = PrimCurrent.curr_tree_id;
    const tsp00_Int4 rootLevel = PrimCurrent.currRootNptrs_bd00.np_ptr()->nd_level();
    const SAPDB_Bool bUseServerTasks =
        (FIRST_INDEX_LEVEL_BD00 < rootLevel     ) &&
        (create_index == IndexMode              ) &&
        (! bd405_IsFunctionBasedIndex(StackDesc));

    bd30ReleaseTree( PrimCurrent );

    return bUseServerTasks;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/


