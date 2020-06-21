/*
  @file         gbd494.h
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


#ifndef GBD494_H
#define GBD494_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"   // PASCAL: BD_kernel_constants_and_types

#include "gbd400.h"  // CPP   : InvTree Handling
#include "gbd495.h"  // CPP   : DoubleQueueHandling
#include "gbd497.h"  // CPP   : StackDescInfo
#include "gbd500.h"  // CPP   : Tree Handling
#include "gbd600.h"  // CPP   : NodeHandling
#include "ggg200.h"  // CPP   : SortTemplates

#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;
class Trans_Context;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct tbd494_SortItem // Size is 16 or 24 Bytes
{
    SAPDB_Byte*   pPrimKey_bd494;
    SAPDB_Byte*   pSecKey_bd494;
    SAPDB_Int2    PrimKeyLen_bd494;
    SAPDB_Int2    SecKeyLen_bd494;
    pasbool       bInvListRefNeeded_bd494;
    pasbool       bNextSecKey_bd494;
    pasbool       bLastPrimKey_bd494;
    pasbool       Filler1_bd494;
};

typedef tbd494_SortItem* tbd494_SortItemPtr;

/*---------------------------------------------------------------------------*/

class cbd494_CompareOperator : public cgg200ComparisonOperator <tbd494_SortItemPtr>
{
public:

    cbd494_CompareOperator(    // PTS 1124759 2003-10-21
        tgg00_BasisError            &basisError,
        const cbd497_StackDescInfo  &stackDescInfo )
    :
    m_TrError( basisError ),
    m_Stack( stackDescInfo ),
    m_IsIndexUnique( m_Stack.bd497IsInvUnique()),
    m_CompResult( tsp00_LcompResult::fromConst( l_undef ))
    {}

    tsp00_LcompResult_Enum gg200Compare(
        const tbd494_SortItemPtr    &pSortItem1,
        const tbd494_SortItemPtr    &pSortItem2 );
    
private:

    bool AllColumnsAreNull( const SAPDB_Byte* pSecKey ) const // PTS 1124759 2003-10-21
    {
        const SAPDB_Int4 lastColumn  = m_Stack.bd497LastColumn();
        SAPDB_Int4       offset = 0;

        for( SAPDB_Int4 currColumn = m_Stack.bd497FirstColumn();
                currColumn <= lastColumn; currColumn++ )
        {
            const SAPDB_Byte defineByte =
                m_Stack.bd497IsColumnDescending( currColumn ) ?
                ~csp_undef_byte : csp_undef_byte;

            if( defineByte != *( pSecKey + offset )){
                return false; // at least one column is not null
            }
            offset += m_Stack.bd497GetColumnLength( currColumn );
        }
        return true;
    };

private:

    tgg00_BasisError            &m_TrError;
    const cbd497_StackDescInfo  &m_Stack;  // PTS 1124759 2003-10-21
    const bool                  m_IsIndexUnique;
    tsp00_LcompResult           m_CompResult;
};

/*---------------------------------------------------------------------------*/

class cbd494_SortableDoubleWriteQueue :
            public cbd495_DoubleWriteQueue,
            public cgg200_MergeTarget <tbd494_SortItemPtr>
{
public:

    cbd494_SortableDoubleWriteQueue(  //Accessing an existing DoubleWriteQueue
        tbd_current_tree&   queueCurrent,
        const Data_PageNo&  primQueuePno,
        const Data_PageNo&  secQueuePno )
    :
    cbd495_DoubleWriteQueue( queueCurrent, primQueuePno, secQueuePno ),
    m_TrError( queueCurrent.curr_trans->trError_gg00 ),
    m_NumPrimKeys( 0 ),
    m_MinRequiredPrimKeySpace( 0 )
    {}

private:

    void PushPrimKey(
        const SAPDB_Byte* const  pPrimKey,
        const SAPDB_Int4         primKeyLen ){
        m_PrimKeyQueue.bd550Push( primKeyLen, pPrimKey );
    }

    void PushSecKey(
        const SAPDB_Byte* const   pSecKey,
        const SAPDB_Int4          secKeyLen ){
        m_SecKeyQueue.bd550Push( secKeyLen, pSecKey );
    }

#ifdef SAPDB_SLOW
    bool gg200SetNextElement( tbd494_SortItemPtr  pSortItem );
#else
    bool gg200SetNextElement( tbd494_SortItemPtr  pSortItem )
    {
        if( e_ok != m_TrError ){
            return false;
        }
        ++m_NumPrimKeys;
        m_MinRequiredPrimKeySpace += pSortItem->PrimKeyLen_bd494;
        this->PushPrimKey (pSortItem->pPrimKey_bd494, pSortItem->PrimKeyLen_bd494);
        if(  pSortItem->bLastPrimKey_bd494 )
        {
            bd495PushNumPrimKeys (m_NumPrimKeys);
            bd495PushMinRequiredPrimKeySpace (m_MinRequiredPrimKeySpace);
            this->PushSecKey (pSortItem->pSecKey_bd494, pSortItem->SecKeyLen_bd494);
            m_NumPrimKeys             = 0;
            m_MinRequiredPrimKeySpace = 0;
        }
        return e_ok == m_TrError;
    }
#endif

protected:

    tgg00_BasisError    &m_TrError;

private:

    SAPDB_Int4  m_NumPrimKeys;
    SAPDB_Int4  m_MinRequiredPrimKeySpace;
};

/*---------------------------------------------------------------------------*/

class cbd494_SortableInvTree :
            public cbd400_InvTree,
            public cgg200_MergeTarget <tbd494_SortItemPtr>
{
public:

    cbd494_SortableInvTree(
        cbd600_Node &leafNode,
        SAPDB_Int4  &secKeyCount,
        SAPDB_Int4  &secLeafCount )
    :
    cbd400_InvTree( leafNode ),
    m_SecKeyCount( secKeyCount ),
    m_SecLeafCount( secLeafCount),
    m_InvRecIndex( NIL_RECINDEX_BD00 )
    {}

    ~cbd494_SortableInvTree()
    {
        // Maintain leaf count information of the rightmost leaf up to
        // the root, if the last append has been made.

        if( e_ok != m_TrError ){  // PTS 1120130 TS 2003-01-17
            return;
        }
        bd520LeafCountPropagation();
        m_SecLeafCount = bd500LeafCount();
    }

private:

    bool gg200SetNextElement( tbd494_SortItemPtr  pSortItem );

private:

    SAPDB_Int4  &m_SecKeyCount;
    SAPDB_Int4  &m_SecLeafCount;

    SAPDB_Int4  m_InvRecIndex;
};

/*---------------------------------------------------------------------------*/

class cbd494_Funnel : public cgg200_MergeSource <tbd494_SortItemPtr>
{
public:

    cbd494_Funnel( SAPDBMem_IRawAllocator& allocator );
    
    ~cbd494_Funnel(){
        this->Delete();
    }

    // base error is not affected if an error occur
    bool Initialize( const SAPDB_UInt numFunnelItems );

    // Used for subsequent funnel merge call to reinitialize funnel
    // without given the allocated memory away!
    void ReInitialize(){
        this->ReInitializeTempData();
    }

    void AddDoubleQueueFile(
        Trans_Context&      trans,
        const Data_PageNo&  primQueueRoot,
        const Data_PageNo&  secQueueRoot );

    void AddTempInvFile(
        Trans_Context&      trans,
        const Data_PageNo&  tempInvRoot );

    bool MemoryAvailable(
        const SAPDB_Int4  numQueues,
        const SAPDB_Int4  numTrees ) const
    {
        const SAPDB_UInt neededSpace =
            numQueues * cbd495_DoubleReadQueue::bd495GetSize() +
            numTrees  * cbd495_TempInvTree::bd495GetSize();
        return neededSpace <= m_InvQueueSizes;
    }

    void Merge(
        Trans_Context&                           trans,
        const cbd497_StackDescInfo&              stackDescInfo,
        cgg200_MergeTarget <tbd494_SortItemPtr>& destinationFile );

    void RemoveAddedFiles( Trans_Context& trans );
    
private:

    // Return false if no more key pair is available
    bool gg200GetCurrentListElement(
        unsigned int         currQueueIndex,
        tbd494_SortItemPtr  &pSortItem )
    {
        pSortItem = m_pSortItem + currQueueIndex;

        return (*( m_ppQueue + currQueueIndex ))->bd495GetCurrentItem(
                   pSortItem->pPrimKey_bd494,
                   pSortItem->pSecKey_bd494,
                   pSortItem->PrimKeyLen_bd494,
                   pSortItem->SecKeyLen_bd494,
                   pSortItem->bNextSecKey_bd494,
                   pSortItem->bLastPrimKey_bd494,
                   pSortItem->bInvListRefNeeded_bd494 );
    }

    void gg200GotoNextListElement( unsigned int    CurrQueueIndex ){
        (*(m_ppQueue + CurrQueueIndex))->bd495GotoNextItem();
    }

    unsigned int gg200GetListCount(){
        return m_NumMergeQueues;
    }

    SAPDB_Int4 GetFunnelItemSize() const
    {
#       ifdef BIT64
        return SAPDB_ALIGN_8BYTE( sizeof( cgg200_MergeNode <tbd494_SortItemPtr> ));
#       else
        return SAPDB_ALIGN_4BYTE( sizeof( cgg200_MergeNode <tbd494_SortItemPtr> ));
#       endif
    }

    SAPDB_Int4 GetSortItemSize() const
    {
#       ifdef BIT64
        return SAPDB_ALIGN_8BYTE( sizeof( tbd494_SortItem ));
#       else
        return SAPDB_ALIGN_4BYTE( sizeof( tbd494_SortItem ));
#       endif
    }

    bool CheckSpaceAndCancelFlag(
        Trans_Context&    trans,
        const SAPDB_UInt  neededSpace ) const;

    void Delete();
    
    void ReInitializeTempData()
    {
        SAPDBERR_ASSERT_STATE( 128 <= m_NumFunnelItems ); 
        m_NumMergeQueues      = 0;
        m_QueuePos            = 0;
        m_QueueSpaceAvailable = m_NumFunnelItems * cbd495_DoubleReadQueue::bd495GetSize();
    }

private:

    // Assignment operator is not supported
    cbd494_Funnel& operator=( const cbd494_Funnel &fun );

    // Copy constructor is not supported
    cbd494_Funnel( const cbd494_Funnel &fun );

private:

    SAPDBMem_IRawAllocator  &m_Allocator;
    // Attention: must be a member data, because it is  needed for the  
    // RemoveAddedFiles() method to free unreleased page chains in
    // particular (cbd495_DoubleReadQueue)
    tbd_current_tree    m_QueueCurrent;

    SAPDB_Int4  m_NumFunnelItems;

    // Temporary data must be initialized with each new merge() call
    SAPDB_UInt  m_NumMergeQueues;
    SAPDB_UInt  m_QueuePos; // interacts with m_pQueue
    SAPDB_UInt  m_QueueSpaceAvailable;

    // FUNNEL SORT AREA DATA
    // should be fixed as long as the funnel exist
    // if more than one merge step is needed the same funnel
    // is used without new creation!
    SAPDB_UInt  m_FunnelItemSizes;
    SAPDB_UInt  m_SortItemSizes;  
    SAPDB_UInt  m_InvQueuePtrsSizes; // pointer to file handler
    SAPDB_UInt  m_InvQueueSizes; // file handler

    SAPDB_Byte*             m_pFunnel;
    SAPDB_Byte*             m_pQueue;
    tbd495_InvQueuePtr*     m_ppQueue;
    tbd494_SortItem*        m_pSortItem;
};


/*===========================================================================*
*  DEFINITION OF METHODS                                                    *
*===========================================================================*/


#endif  /* GBD494_H */

