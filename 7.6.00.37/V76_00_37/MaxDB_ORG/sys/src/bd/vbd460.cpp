/****************************************************************************

  module      : vbd460.cpp

  -------------------------------------------------------------------------

  author:       AlexanderK
  responsible : TorstenS

  special area: InvList  Iterator
  description : navigates over InvLists


  last changed: 1999-09-15  19:45
  see also    : example.html ...

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

#include "gbd460.h"
#include "heo51.h"
#include "heo56.h"

#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
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
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

static inline void TaskReschedule()
{
    tsp00_TaskId taskId;
    vgetpid( taskId );
    vsleep( taskId, 0 );
}

/*---------------------------------------------------------------------------*/

void
cbd460_InvListArrayIterator::bd460_Init(
    tgg00_RecPtr  pRec,
    tsp00_KeyPtr  StartKey,
    tsp00_Int4    StartKeyLen,
    tsp00_KeyPtr  StopKey,
    tsp00_Int4    StopKeyLen,
    bool          bIncludeStartKey )
{
    ROUTINE_DBG_MEO00 ("bd460_Init");

    // SELECT DIRECT: precondition is that both pointers to StartKey and StopKey are equal
    // (mm_direct)

    const bool      c_convert = true;
    new ((void*)    &m_InvListArray) cbd450_InvListArray (m_bConsistentReadSupport, pRec, !c_convert);

    /* initialize member variables */
    if ( m_bAscendingKeyOrder )
        m_StopPKIndex = m_InvListArray.bd450RecPtr()->recNumPrimKeys_gg00() - 1;
    else
        m_StopPKIndex = FIRST_PRIM_KEY_INDEX_BD450;

    m_SearchResult.becomes  (nonefound);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname   (bd_inv, "StartKey    ");
    if ( NULL != StartKey )
        t01moveobj (bd_inv, StartKey, POS_OFF_DIFF_BD00, StartKeyLen);
    t01sname   (bd_inv, "StopKey     ");
    if ( NULL != StopKey )
        t01moveobj (bd_inv, StopKey,  POS_OFF_DIFF_BD00, StopKeyLen );
    t01bool    (bd_inv, "Ascending   ", m_bAscendingKeyOrder );
    t01bool    (bd_inv, "IncludeStart", bIncludeStartKey     );
#   endif

    if ( NULL == StartKey )
    {
        if ( m_bAscendingKeyOrder )
        {
            m_PKIndex = FIRST_PRIM_KEY_INDEX_BD450;
            m_SearchResult.becomes (nextfound);
        }
        else
        {
            m_PKIndex = m_InvListArray.bd450RecPtr()->recNumPrimKeys_gg00() - 1;
            m_SearchResult.becomes (lastfound);
        }
    }
    else
    {
        static SAPDB_Byte MaxWriteTrans[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        m_InvListArray.bd450SearchPrimKey (StartKey, StartKeyLen,
                                           (bIncludeStartKey) ? 0 : reinterpret_cast<tgg91_TransNo*> (&MaxWriteTrans[0]), m_PKIndex, m_SearchResult);
#if defined(oldcode)
        if (bIncludeStartKey && m_bConsistentReadSupport && (nextfound == m_SearchResult) && (m_PKIndex > FIRST_PRIM_KEY_INDEX_BD450))
        {
            tsp00_KeyPtr pPrimKey;
            tsp00_Int2   primKeyLen;

            m_InvListArray.bd450GetPrimKey (m_PKIndex - 1, pPrimKey, primKeyLen);

            if (primKeyLen - (int) sizeof(tgg91_TransNo) >= StartKeyLen)
            {
                tsp00_LcompResult searchResult;
                s30cmp (pPrimKey, 1, primKeyLen,
                        StartKey, 1, StartKeyLen, searchResult);

                if (l_equal == searchResult)
                {
                    --m_PKIndex;
                    m_SearchResult.becomes(thisfound);
                }
            }
        }
        if (m_bConsistentReadSupport
                &&
                (nextfound == m_SearchResult || lastfound == m_SearchResult)
                &&
                false
                &&
                ! bIncludeStartKey)
        {
            if (lastfound == m_SearchResult)
            {
                ++m_PKIndex;
            }
            m_InvListArray.bd450SkipPrimKey (StartKey, StartKeyLen, m_PKIndex, m_SearchResult);
        }
#endif
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01int4   (bd_inv, "NumPrimKeys ", m_InvListArray.bd450RecPtr()->recNumPrimKeys_gg00());
    t01p2int4 (bd_inv, "PKindex     ", m_PKIndex, "SearchResult", m_SearchResult);
#   endif

    if ( StartKey != StopKey )
        bd460_SearchStopKey (StopKey, StopKeyLen);
    else if ( NULL != StartKey )
    {
        // select direct => prevent "++" from being successfull
        if (thisfound == m_SearchResult)
            m_StopPKIndex  = m_PKIndex;
        else
        {
            if ((nextfound == m_SearchResult) && m_bConsistentReadSupport &&
                    m_InvListArray.bd450IsEqualKey (m_PKIndex, StartKey, StartKeyLen))
            {
                m_StopPKIndex  = m_PKIndex;
                m_SearchResult.becomes(thisfound);
                return;
            }
            m_SearchResult.becomes (nextfound);
            m_StopPKIndex  = m_PKIndex - 1;
        }

#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01int4 (bd_inv, "StopPKIndex ", m_StopPKIndex);
#       endif

        return;
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01int4 (bd_inv, "StopPKIndex ", m_StopPKIndex);
#   endif

    if ( m_bAscendingKeyOrder )
    {
        if ( nextfound == m_SearchResult )
        {
            if ( ! bd460_BeyondStopKey() )
                m_SearchResult.becomes (thisfound);
        }
        else if ( (lastfound == m_SearchResult) ||
                  (! bIncludeStartKey && (thisfound == m_SearchResult)) )
            ++(*this);
        // if the startkey is not found in current list,
        // we have to move to the next list by "++"
        // if the starkey was found and we don't want the starkey,
        // we have to move to the next record by "++"
    }
    else
    {
        if ( lastfound == m_SearchResult )
        {
            if ( ! bd460_BeyondStopKey() )
                m_SearchResult.becomes (thisfound);
        }
        else if ( (nextfound == m_SearchResult) ||
                  (! bIncludeStartKey && (thisfound == m_SearchResult)) )
            ++(*this);
    }
}

/*---------------------------------------------------------------------------*/

void
cbd460_InvListSubtreeIterator::bd460_LeaveAndReEnterInvTree() // PTS 1116651 TS 2002-07-15
{
    ROUTINE_DBG_MEO00 ("bd460_LeaveAndReEnterInvTree");

    tgg00_BasisError   &trError = m_InvListSubtreeIteratorNoBreak.bd460TrError();

    tsp00_Key       reEntrancePrimKey;        // PTS 1116626 TS 2002-07-15
    tsp00_Key       reEntranceSecKey;         // PTS 1116626 TS 2002-07-15
    tsp00_Int2      reEntrancePrimKeyLen = 0; // PTS 1116626 TS 2002-07-15
    tsp00_Int2      reEntranceSecKeyLen  = 0; // PTS 1116626 TS 2002-07-15
    tsp00_Int2      keyLen               = 0;
    tsp00_KeyPtr    pKey                 = NULL;

    const tsp00_KeyPtr    pReEntrancePrimKey = ( tsp00_KeyPtr ) &reEntrancePrimKey;
    const tsp00_KeyPtr    pReEntranceSecKey  = ( tsp00_KeyPtr ) &reEntranceSecKey;


    /* check whether one should realy break */
    bool hasDeletedFlag;
    bd460Deref( pKey, reEntrancePrimKeyLen, hasDeletedFlag ); // TODO T.A. think about deleted flag

    if(( NULL == pKey ) || ( 0 == reEntrancePrimKeyLen ))
        return;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    bd460GetSecKey( pKey, keyLen ); // current secondary key to be treated next

    t01sname  ( bd_inv, "sec key:    " );
    t01moveobj( bd_inv, pKey, 1, keyLen );

    bd460Deref( pKey, keyLen, hasDeletedFlag); // current primary key to be treated next
    t01sname   (bd_inv, "prim key:   ");
    t01moveobj (bd_inv, pKey,  1, keyLen);
#   endif              

    m_InvListSubtreeIteratorNoBreak.bd460Break( pReEntrancePrimKey, reEntrancePrimKeyLen );

    if( e_ok != trError )
        return;

    m_SecKeyIter.bd510Break( pReEntranceSecKey, reEntranceSecKeyLen );

    if( e_ok != trError )
        return;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  ( bd_inv, "re-sec key: " );
    t01moveobj( bd_inv, pReEntranceSecKey, 1, reEntranceSecKeyLen );

    t01sname   (bd_inv, "re-prim key:");
    t01moveobj (bd_inv, pReEntrancePrimKey,  1, reEntrancePrimKeyLen);
#   endif     

    // TorstenS 2002-07-16: reduce data cache priority problems by rescheduling the task
    TaskReschedule();

    /* reenter mother tree */
    const tbd_searchresult secKeySearchResult =
        m_SecKeyIter.bd510Continue( pReEntranceSecKey, reEntranceSecKeyLen );

    if( e_ok != trError )
        return;

    if( thisfound != secKeySearchResult ) // as long as stop key isn't reached thisfound is returned
    {
        trError = e_inv_list_not_found;
        return;
    }

    // create new inv list handler
    const tgg00_RecPtr pRec = m_SecKeyIter.bd510GetRecord();

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  ( bd_inv, "cur-sec key:" );
    t01moveobj( bd_inv, pRec->recKey_gg00().keyVal_gg00(), 1, pRec->recKeyLen_gg00() );
#   endif              

    switch (pRec->recInvListKind_gg00())
    {
    case ilkArray_egg00: // get primkey iterator on arrays
        {
            m_InvListArrayIterator.bd460_Init( pRec, pReEntrancePrimKey, reEntrancePrimKeyLen,
                                               m_pStopPrimKey, m_StopPrimKeyLen,
                                               INCLUDE_START_KEY_BD510 );

            m_pUnivIterator = &m_InvListArrayIterator;
        }
        break;

    case ilkSubTree_egg00: // get primkey iterator on subtrees
        {
            cbd450_InvListRefSubTree InvListRefSubTree( pRec );

            m_InvListSubtreeIteratorNoBreak.bd460Continue( InvListRefSubTree,
                                                    pReEntrancePrimKey, reEntrancePrimKeyLen );

            m_pUnivIterator = &m_InvListSubtreeIteratorNoBreak;
        }
        break;

    default:
        trError = e_illegal_record;
        m_SecKeyIter.bd510Node().bd600Dump( bd460c1IllegalRecord_csp03, "bd460_LeaveAndReEnterInv" );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
