/*
  @file         vbd494.cpp
  @author       TorstenS
  @ingroup      create index
  @brief        This modul is used to merge all temporary result
                files of the parallel index creation to the permanent
                result file which contains the new index. The funnel
                merge algorithm is used for this job.

\if EMIT_LICENCE


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

\endif
*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


// Content of include files
#include "gbd494.h"
#include "hsp30.h"   // PASCAL: Buffer handling and comparison routines

#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"


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



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd494.h (CODE)                        * 
 *===========================================================================*/


/*---------------------------------------------------------------------------*/
/* CLASS cbd494_Funnel                                                       */
/*---------------------------------------------------------------------------*/

cbd494_Funnel::cbd494_Funnel( SAPDBMem_IRawAllocator& allocator )
        :
        m_Allocator( allocator ),
        m_NumFunnelItems( 0 ),
        m_NumMergeQueues( 0 ),
        m_QueuePos( 0 ),
        m_QueueSpaceAvailable( 0 ),
        m_FunnelItemSizes( 0 ),
        m_SortItemSizes( 0 ),
        m_InvQueuePtrsSizes( 0 ),
        m_InvQueueSizes( 0 ),
        m_pFunnel( 0),
        m_pQueue( 0 ),
        m_ppQueue( 0 ),
        m_pSortItem( 0 )
{
    m_QueueCurrent              = b01nilcurrent;
    m_QueueCurrent.curr_trans   = 0; // not set because session specific; see Add()
    m_QueueCurrent.curr_tree_id = bd495InitTempFileId( NIL_PAGE_NO_GG00 );
}

/*---------------------------------------------------------------------------*/

bool
cbd494_Funnel::Initialize( const SAPDB_UInt numFunnelItems )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd494_Funnel::Init", DataIndex_Trace, 5 );

    this->Delete();

    // m_pSortItem: memory for SortItems because the Funnel Merge works with
    // Pointers only for two reasons:
    // [1] to reduce copy costs
    // [2] to enable modifing parts of the SortItem within the Compare(!) methode

    const SAPDB_UInt funnelItemSizes  = this->GetFunnelItemSize() * numFunnelItems;
    const SAPDB_UInt sortItemSizes    = this->GetSortItemSize() * numFunnelItems;
    const SAPDB_UInt invQueuePtrSizes = sizeof( tbd495_InvQueuePtr )* numFunnelItems;
    const SAPDB_UInt invQueueSizes    = cbd495_DoubleReadQueue::bd495GetSize()* numFunnelItems;

    if( DataIndex_Trace.TracesLevel( 6 ))
    {
        const SAPDB_UInt requestedBytes = funnelItemSizes + sortItemSizes
                                          + invQueuePtrSizes + invQueueSizes;
        Kernel_VTrace() << "> cbd494_Funnel::Init() Requestes bytes: "
        << requestedBytes << " numFunnelItems: " << numFunnelItems << FlushLine;
    }
    // m_pFunnel: memory for funnel merge nodes
    m_pFunnel = reinterpret_cast < SAPDB_Byte* >( m_Allocator.Allocate( funnelItemSizes ));
    if( 0 == m_pFunnel ){
        return false;
    }
    m_pSortItem =  reinterpret_cast < tbd494_SortItem* >( m_Allocator.Allocate( sortItemSizes ));
    if( 0 == m_pSortItem ){
        return false;
    }
    // m_ppQueue: memory for cbd495InvQueue pointers
    m_ppQueue = reinterpret_cast < tbd495_InvQueuePtr* >( m_Allocator.Allocate( invQueuePtrSizes ));
    if( 0 == m_ppQueue ){
        return false;
    }
    // m_pQueue: memory for cbd495InvQueues itself (placement new)
    m_pQueue = reinterpret_cast < SAPDB_Byte* >( m_Allocator.Allocate( invQueueSizes ));
    if( 0 == m_pQueue ){
        return false;
    }
    m_FunnelItemSizes   = funnelItemSizes;
    m_SortItemSizes     = sortItemSizes;
    m_InvQueuePtrsSizes = invQueuePtrSizes;
    m_InvQueueSizes     = invQueueSizes;

    m_NumFunnelItems    = numFunnelItems;
    this->ReInitializeTempData();
    return true;
}

/*---------------------------------------------------------------------------*/

void
cbd494_Funnel::AddDoubleQueueFile(
    Trans_Context&     trans,
    const Data_PageNo& primQueueRoot,
    const Data_PageNo& secQueueRoot )
{
    const SAPDB_UInt    neededSpace = cbd495_DoubleReadQueue::bd495GetSize();
    const SAPDB_UInt    cMaxRetries = 10;

    if( ! this->CheckSpaceAndCancelFlag( trans, neededSpace )){
        return; // base error code is set
    }
    m_QueueCurrent.curr_trans = &trans.OldTrans(); // set correct trans into 'shared' current
    for( SAPDB_UInt retryCount  = 0; retryCount < cMaxRetries; retryCount++ )
    {
        // placement new for double queue index file handler in given memory chunk
        *( m_ppQueue + m_NumMergeQueues ) =
            REINTERPRET_CAST (tbd495_InvQueuePtr, new (m_pQueue + m_QueuePos)
                              cbd495_DoubleReadQueue( m_QueueCurrent, primQueueRoot, secQueueRoot));
        if( e_ok == trans.OldTrans().trError_gg00 )
        {
            m_NumMergeQueues+=1;
            m_QueuePos += neededSpace;
            m_QueueSpaceAvailable -= neededSpace;
            SAPDBERR_ASSERT_STATE( m_QueuePos <= m_InvQueueSizes );
            return;
        }else{ // placement delete
            (*( m_ppQueue + m_NumMergeQueues ))->~cbd495_InvQueue(); // PTS 1105716 TS 2000-02-23
        }
        if( e_sysbuffer_overflow != trans.OldTrans().trError_gg00 ){
            return;
        }
        trans.OldTrans().trError_gg00 = e_ok;
        RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_FUNNEL_FAILED_1 ));
        trans.Task().Sleep( 1 );
    }
}
/*---------------------------------------------------------------------------*/

void
cbd494_Funnel::AddTempInvFile(
    Trans_Context&      trans,
    const Data_PageNo&  tempInvRoot )
{
    const SAPDB_UInt    neededSpace = cbd495_TempInvTree::bd495GetSize();
    const SAPDB_UInt    cMaxRetries = 10;

    if( ! this->CheckSpaceAndCancelFlag( trans, neededSpace )){
        return; // base error code is set
    }
    for( SAPDB_UInt retryCount  = 0; retryCount < cMaxRetries; retryCount++ )
    {
        // placement new for temporary index tree file handler in given memory chunk
        tgg00_FileId    tempFileId = bd495InitTempFileId( tempInvRoot );

        *( m_ppQueue + m_NumMergeQueues ) =
            REINTERPRET_CAST (tbd495_InvQueuePtr, new ( m_pQueue + m_QueuePos )
                              cbd495_TempInvTree (trans.OldTrans(), tempFileId, ! DROP_TREE_BD495));
        if( e_ok == trans.OldTrans().trError_gg00 )
        {
            m_NumMergeQueues+=1;
            m_QueuePos += neededSpace;
            m_QueueSpaceAvailable -= neededSpace;
            SAPDBERR_ASSERT_STATE( m_QueuePos <= m_InvQueueSizes );
            return;
        }else{ // placement delete
            (*( m_ppQueue + m_NumMergeQueues ))->~cbd495_InvQueue(); // PTS 1105716 TS 2000-02-23
        }
        if( e_sysbuffer_overflow != trans.OldTrans().trError_gg00 ){
            return;
        }
        trans.OldTrans().trError_gg00 = e_ok;
        RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_FUNNEL_FAILED_1 ));
        trans.Task().Sleep( 1 );
    }
}

/*---------------------------------------------------------------------------*/

void
cbd494_Funnel::Merge(
    Trans_Context&                              trans,
    const cbd497_StackDescInfo&                 stackDescInfo,
    cgg200_MergeTarget <tbd494_SortItemPtr>&    destinationFile )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd494_Funnel::Merge", DataIndex_Trace, 5 );

    if( 0 > m_NumMergeQueues ){
        return;
    }
    if( e_ok != trans.OldTrans().trError_gg00 ){
        return;
    }
    const SAPDB_Int4        sizeOfWorkingSpace = this->GetFunnelItemSize() * m_NumFunnelItems;
    cbd494_CompareOperator  comperator( trans.OldTrans().trError_gg00, stackDescInfo );

    const bool bMergeSuccessful = gg200Merge( *this, destinationFile, comperator,
                                  m_pFunnel, sizeOfWorkingSpace );

    if(( ! bMergeSuccessful ) && ( e_ok == trans.OldTrans().trError_gg00 ))
    {
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_FUNNEL_FAILED_2 ));
    }
}

/*---------------------------------------------------------------------------*/

void
cbd494_Funnel::RemoveAddedFiles( Trans_Context& trans )
{
    // Destroy all InvQueue objects generated before in Funnel Merge memory.
    // This is necessary only if gg200Merge failed, because the Funnel Merge
    // frees page after page if it isn't needed anymore.

    if( 0 == m_ppQueue ){
        return;
    }
    m_QueueCurrent.curr_trans = &trans.OldTrans(); // set correct trans into 'shared' current

    for( SAPDB_UInt currIndex = 0; currIndex < m_NumMergeQueues; ++currIndex ){
        (*(m_ppQueue + currIndex))->bd495Free();
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

 bool
cbd494_Funnel::CheckSpaceAndCancelFlag(
    Trans_Context&    trans,
    const SAPDB_UInt  neededSpace ) const
{
    if( trans.Task().CancelPending())
    {
        trans.OldTrans().trError_gg00 = e_cancelled;
        RTE_Message( Data_Exception( __CONTEXT__, DATA_FUNNEL_CANCELED_1 ));
        return false;
    }
    if( 0 > static_cast< SAPDB_Int>( m_QueueSpaceAvailable - neededSpace ))
    {
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        RTE_Message( Data_Exception( __CONTEXT__, DATA_FUNNEL_OUT_OF_MEMORY_1 ));
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

 void
cbd494_Funnel::Delete()
{
    // No placement delete for the handler instantiated in the
    // memory pointed by m_pQueue! This will be done with the
    // explicit call of RemoveAddedFiles()

    if( 0 != m_pFunnel ){
        m_Allocator.Deallocate( m_pFunnel );
    }
    if( 0 != m_pSortItem ){
        m_Allocator.Deallocate( m_pSortItem );
    }
    if( 0 != m_ppQueue ){
        m_Allocator.Deallocate( m_ppQueue );
    }
    if( 0 != m_pQueue ){
        m_Allocator.Deallocate( m_pQueue );
    }
    m_FunnelItemSizes   = 0;
    m_SortItemSizes     = 0;
    m_InvQueuePtrsSizes = 0;
    m_InvQueueSizes     = 0;
}

/*---------------------------------------------------------------------------*/
/* CLASS cbd494_CompareOperator                                              */
/*---------------------------------------------------------------------------*/

tsp00_LcompResult_Enum
cbd494_CompareOperator::gg200Compare(
    const tbd494_SortItemPtr    &pSortItem1,
    const tbd494_SortItemPtr    &pSortItem2 )
{
    s30cmp (pSortItem1->pSecKey_bd494, POS_OFF_DIFF_BD00, pSortItem1->SecKeyLen_bd494,
            pSortItem2->pSecKey_bd494, POS_OFF_DIFF_BD00, pSortItem2->SecKeyLen_bd494,
            m_CompResult);

    if( l_equal != m_CompResult ){
        return m_CompResult;
    }
    if(
        ( m_IsIndexUnique )
        &&
        ( ! AllColumnsAreNull( pSortItem1->pSecKey_bd494 )) // PTS 1124759 2003-10-21
    )
    {
        m_TrError = e_duplicate_invkey;
        return m_CompResult;
    }
    s30cmp (
        pSortItem1->pPrimKey_bd494, POS_OFF_DIFF_BD00, pSortItem1->PrimKeyLen_bd494,
        pSortItem2->pPrimKey_bd494, POS_OFF_DIFF_BD00, pSortItem2->PrimKeyLen_bd494,
        m_CompResult);

    // Note that the Funnel Merge uses the smaller key as the winner
    if (l_greater == m_CompResult)
    {
        pSortItem1->bNextSecKey_bd494  = false; // The greater one couldn't contain the next secondary key
        pSortItem2->bLastPrimKey_bd494 = false; // The smaller one couldn't contain the last primary key
    }
    else // could be l_less only;
    {
        pSortItem1->bLastPrimKey_bd494 = false; // see above
        pSortItem2->bNextSecKey_bd494  = false; // see above
    }
    return m_CompResult;
}

/*---------------------------------------------------------------------------*/
/* CLASS cbd494_SortableDoubleWriteQueue                                     */
/*---------------------------------------------------------------------------*/

#ifdef SAPDB_SLOW
bool
cbd494_SortableDoubleWriteQueue::gg200SetNextElement( tbd494_SortItemPtr    pSortItem )
{
    /*
    #	if COMPILEMODE_MEO00 >= SLOW_MEO00 
        
        t01sname (bd_idx_create, "Sec Key     ");
        t01buf   (bd_idx_create, pSortItem->pSecKey_bd494, 
            POS_OFF_DIFF_BD00, pSortItem->SecKeyLen_bd494);
        
        t01sname (bd_idx_create, "Prim Key    ");
        t01buf   (bd_idx_create, pSortItem->pPrimKey_bd494, 
            POS_OFF_DIFF_BD00, pSortItem->PrimKeyLen_bd494);
    #   endif    
    */
    if( e_ok != m_TrError ){
        return false;
    }
    ++m_NumPrimKeys;
    m_MinRequiredPrimKeySpace += pSortItem->PrimKeyLen_bd494;

    this->PushPrimKey (pSortItem->pPrimKey_bd494, pSortItem->PrimKeyLen_bd494);

    if (pSortItem->bLastPrimKey_bd494)
    {
        /*
        #           if COMPILEMODE_MEO00 >= SLOW_MEO00 
                    t01bool (bd_idx_create, "LastPrimKey ", true);
                    t01int4 (bd_idx_create, "NumPrimKeys ", m_NumPrimKeys);
        #           endif
        */            
        bd495PushNumPrimKeys (m_NumPrimKeys);
        bd495PushMinRequiredPrimKeySpace (m_MinRequiredPrimKeySpace);

        this->PushSecKey (pSortItem->pSecKey_bd494, pSortItem->SecKeyLen_bd494);

        m_NumPrimKeys             = 0;
        m_MinRequiredPrimKeySpace = 0;
    }
    /*
    #   if COMPILEMODE_MEO00 >= SLOW_MEO00 
     t01basis_error (bd_idx_create, "SetNextDWQ  ", m_TrError);
    #   endif
    */
    return e_ok == m_TrError;
}
#endif

/*---------------------------------------------------------------------------*/
/* CLASS cbd494_SortableInvTree                                              */
/*---------------------------------------------------------------------------*/

bool
cbd494_SortableInvTree::gg200SetNextElement( tbd494_SortItemPtr pSortItem )
{
    /*
    #	if COMPILEMODE_MEO00 >= SLOW_MEO00 
        
        t01sname (bd_idx_create, "Sec Key     ");
        t01buf   (bd_idx_create, pSortItem->pSecKey_bd494, 
            POS_OFF_DIFF_BD00, pSortItem->SecKeyLen_bd494);
        
        t01sname (bd_idx_create, "Prim Key    ");
        t01buf   (bd_idx_create, pSortItem->pPrimKey_bd494, 
            POS_OFF_DIFF_BD00, pSortItem->PrimKeyLen_bd494);
    #   endif    
    */ 
    if( e_ok != m_TrError ){
        return false;
    }
    /*
    #       if COMPILEMODE_MEO00 >= SLOW_MEO00 
            t01bool (bd_idx_create, "NextSecKey  ", pSortItem->bNextSecKey_bd494);
    #       endif
    */
    if (pSortItem->bNextSecKey_bd494)
    {
        ++m_SecKeyCount;
        /*
        #           if COMPILEMODE_MEO00 >= SLOW_MEO00 
                    t01bool (bd_idx_create, "InvListRef  ", pSortItem->bInvListRefNeeded_bd494);
        #           endif
        */
    }

    if (m_SubTreeCurrent.curr_trans->trRteCommPtr_gg00->to_cancel)
    {
        m_TrError = e_cancelled;
        RTE_Message( Data_Exception( __CONTEXT__, DATA_FUNNEL_CANCELED_1 ));
    }
    else
        this->bd400AppendPrimKey (
            pSortItem->pSecKey_bd494, pSortItem->SecKeyLen_bd494,
            pSortItem->pPrimKey_bd494, pSortItem->PrimKeyLen_bd494,
            0 != pSortItem->bNextSecKey_bd494,       /* PTS 1104438 UH 03-12-1999 */
            0 != pSortItem->bInvListRefNeeded_bd494, /* PTS 1104438 UH 03-12-1999 */
            m_InvRecIndex);

    /*
    #   if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01basis_error (bd_idx_create, "SetNextInv  ", m_TrError);
    #   endif
    */
    return e_ok == m_TrError;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
