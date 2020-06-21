/****************************************************************************

  module      : gbd460.h

  -------------------------------------------------------------------------

  author      : AlexanderK
  responsible : TorstenS

  special area: InvList  Iterator
  description : navigates over InvLists


  last changed: 1999-09-16  13:52
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


#ifndef GBD460_H
#define GBD460_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"

#include "gbd600.h"
#include "gbd601.h"
#include "gbd450.h"
#include "gbd500.h"
#include "gbd510.h"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h" 
#endif

class cbd460_InvListSubtreeIterator;

class cbd460_InvListIterator
{
public:

    virtual cbd460_InvListIterator&  operator ++ () = 0;

    virtual void bd460SkipCurrentKey() = 0;

    virtual void bd460Deref(
        tsp00_KeyPtr &pPrimKey,
        tsp00_Int2   &PrimKeyLen,
        bool         &HasDeletedFlag ) = 0;

    virtual tsp00_Int4 bd460NumPrimKeysInKeyScope() = 0;

    virtual void bd460GetSecKey( // PTS 1116651 TS 2002-07-15
        tsp00_KeyPtr &pSecKey,
        tsp00_Int2   &SecKeyLen ) const = 0;

    virtual ~cbd460_InvListIterator(){};
};

/*---------------------------------------------------------------------------*/

class cbd460_InvListArrayIterator : public cbd460_InvListIterator
{
public:

    cbd460_InvListArrayIterator(
        tgg00_RecPtr  pRec,
        tsp00_KeyPtr  StartKey,
        tsp00_Int4    StartKeyLen,
        tsp00_KeyPtr  StopKey,
        tsp00_Int4    StopKeyLen,
        bool          bAscendingKeyOrder,
        bool          bIncludeStartKey,
        bool          bConsistentReadSupport );

    cbd460_InvListArrayIterator(
        const bool bAscendingKeyOrder,
        const bool bConsistentReadSupport )
    :
    m_bAscendingKeyOrder( bAscendingKeyOrder || bConsistentReadSupport ),
    m_bConsistentReadSupport( bConsistentReadSupport )
    {};

    cbd460_InvListIterator& operator ++ ();

    void bd460SkipCurrentKey();

    void bd460Deref(
        tsp00_KeyPtr    &pPrimKey,
        tsp00_Int2      &PrimKeyLen,
        bool            &HasDeletedFlag );

    tsp00_Int4  bd460NumPrimKeysInKeyScope();


    void bd460GetSecKey(
        tsp00_KeyPtr    &pSecKey,
        tsp00_Int2      &SecKeyLen ) const; // PTS 1116651 TS 2002-07-15

    void bd460_Init(
        tgg00_RecPtr  pRec,
        tsp00_KeyPtr  StartKey,
        tsp00_Int4    StartKeyLen,
        tsp00_KeyPtr  StopKey,
        tsp00_Int4    StopKeyLen,
        bool          bIncludeStartKey );

private:

    void bd460_SearchStopKey(
        tsp00_KeyPtr    StopKey,
        tsp00_Int4      StopKeyLen );

    bool bd460_BeyondStopKey() const;

private:

    const bool             m_bAscendingKeyOrder;
    const bool             m_bConsistentReadSupport;

    cbd450_InvListArray    m_InvListArray;
    tsp00_Int4             m_PKIndex;
    tsp00_Int4             m_StopPKIndex;
    tbd_searchresult       m_SearchResult;
};

/*---------------------------------------------------------------------------*/

class cbd460_InvListArrayIteratorSave : public cbd460_InvListArrayIterator
{
public:

    cbd460_InvListArrayIteratorSave(
        cbd510_RecordIterator & RecordIterator,
        tsp00_KeyPtr            StartKey,
        tsp00_Int4              StartKeyLen,
        tsp00_KeyPtr            StopKey,
        tsp00_Int4              StopKeyLen,
        bool                    bAscendingKeyOrder,
        bool                    bIncludeStartKey )
    :
    cbd460_InvListArrayIterator( RecordIterator.bd510GetRecord(),
                                 StartKey, StartKeyLen,
                                 StopKey, StopKeyLen,
                                 bAscendingKeyOrder, bIncludeStartKey,
                                 false /* TODO */),
    m_Node( RecordIterator.bd510Node())
    {}

private:

    cbd600_Node  m_Node;
};

/*---------------------------------------------------------------------------*/

class cbd460_InvListSubtreeIteratorNoBreak : public cbd460_InvListIterator
{
    friend class cbd460_InvListSubtreeIterator; // because of private methods

public:

    cbd460_InvListSubtreeIteratorNoBreak(
        cbd500_Tree                   &MotherTree,
        cbd450_InvListRefSubTree      &InvListRefSubTree,
        tsp00_KeyPtr                   pStartPrimKey,
        tsp00_Int4                     StartPrimKeyLen,
        tsp00_KeyPtr                   pStopPrimKey,
        tsp00_Int4                     StopPrimKeyLen,
        bool                           bAscendingKeyOrder,
        bool                           bIncludeFirstPrimKey,
        bool                          *pTreeIsRequested = NULL );

    cbd460_InvListIterator& operator ++ ();

    void bd460SkipCurrentKey();

    void bd460Deref(
        tsp00_KeyPtr &pPrimKey,
        tsp00_Int2   &PrimKeyLen,
        bool         &HasDeletedFlag );

    tsp00_Int4 bd460NumPrimKeysInKeyScope();

    void bd460GetSecKey(
        tsp00_KeyPtr &pSecKey,
        tsp00_Int2   &SecKeyLen ) const; // PTS 1116651 TS 2002-07-15
private:

    void bd460Break(
        const tsp00_KeyPtr   rePositionPrimKey,
        tsp00_Int2           &rePositionPrimKeyLen );

    void bd460Continue(
        cbd450_InvListRefSubTree  &InvListRefSubTree,
        tsp00_KeyPtr               rePositionPrimKey,
        tsp00_Int4                 rePositionPrimKeyLen );

    tgg00_BasisError& bd460TrError();

private:

    cbd300_InvSubTreeCurrent  m_InvSubTreeCurrent;
    cbd601_SubTreeNode        m_SubTreeLeaf;
    cbd500_SubTree            m_SubTree;
    cbd510_RecordIterator     m_PrimKeyIter;
    bool                      m_bStartKeyFound_Dummy; // required by m_PrimKeyIter, but not used

    tsp00_KeyPtr              m_pSecKey;   // PTS 1116651 TS 2002-07-15
    tsp00_Int2                m_SecKeyLen;  // PTS 1116651 TS 2002-07-15
};

/*---------------------------------------------------------------------------*/

class cbd460_InvListSubtreeIterator : public cbd460_InvListIterator
{
public:

    cbd460_InvListSubtreeIterator(
        cbd500_Tree                 &MotherTree,
        cbd510_RecordIterator       &SecKeyIter,
        cbd450_InvListRefSubTree    &InvListRefSubTree,
        tsp00_KeyPtr                pStartPrimKey,
        tsp00_Int4                  StartPrimKeyLen,
        tsp00_KeyPtr                pStopPrimKey,
        tsp00_Int4                  StopPrimKeyLen,
        bool                        bAscendingKeyOrder,
        bool                        bIncludeFirstPrimKey,
        bool                        bConsistentReadSupport );

    cbd460_InvListIterator& operator ++ ();

    void bd460SkipCurrentKey();

    void bd460Deref(
        tsp00_KeyPtr    &pPrimKey,
        tsp00_Int2      &PrimKeyLen,
        bool            &HasDeletedFlag );

    tsp00_Int4 bd460NumPrimKeysInKeyScope();

    void bd460GetSecKey(
        tsp00_KeyPtr    &pSecKey,
        tsp00_Int2      &SecKeyLen ) const;  // PTS 1116651 TS 2002-07-15

private:

    void bd460_LeaveAndReEnterInvTree();

private:

    const tsp00_KeyPtr                      m_pStopPrimKey;
    const tsp00_Int4                        m_StopPrimKeyLen;

    // inversion list iterator of the proper index tree
    cbd510_RecordIterator                  &m_SecKeyIter; // PTS 1132245 TS 2004-11-09

    bool                                    m_bTreeIsRequested;

    cbd460_InvListSubtreeIteratorNoBreak    m_InvListSubtreeIteratorNoBreak;
    cbd460_InvListArrayIterator             m_InvListArrayIterator;
    cbd460_InvListIterator                  *m_pUnivIterator; // pointer to the current inv list member
};

/*===========================================================================*
 *  METHOD IMPLEMENTATION                                                    *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION OF cbd460_InvListArrayIterator                      */
/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListArrayIterator::bd460_SearchStopKey(
    tsp00_KeyPtr  StopKey,
    tsp00_Int4    StopKeyLen )
{
    ROUTINE_DBG_MEO00 ("bd460_SearchStopKey");

    if (NULL != StopKey)
    {
        tbd_searchresult SearchResult;

        static SAPDB_Byte MaxWriteTrans[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

        m_InvListArray.bd450SearchPrimKey (StopKey, StopKeyLen, reinterpret_cast<tgg91_TransNo*>(&MaxWriteTrans[0]), m_StopPKIndex, SearchResult);

        if ((nextfound == SearchResult) && m_bConsistentReadSupport && false)
        {
            tsp00_KeyPtr pPrimKey;
            tsp00_Int2   primKeyLen;

            m_InvListArray.bd450GetPrimKey (m_PKIndex, pPrimKey, primKeyLen);

            if (primKeyLen - sizeof(tgg91_TransNo) == StopKeyLen)
            {
                // actually thisfound
                this->bd460SkipCurrentKey();
            }
        }

        if (m_bAscendingKeyOrder)
        {
            /* m_StopPKIndex references the key behind StopKey
               therefore m_StopPKIndex must reference the preceding key */
            if (nextfound == SearchResult)
                m_StopPKIndex --;
        }
        else
        {
            if (lastfound == SearchResult)
                m_StopPKIndex ++;
        }
    }
}

/*---------------------------------------------------------------------------*/

inline bool
cbd460_InvListArrayIterator::bd460_BeyondStopKey() const
{
    if (m_bAscendingKeyOrder)
        return m_PKIndex > m_StopPKIndex;
    else
        return m_PKIndex < m_StopPKIndex;
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListArrayIterator::bd460Deref(
    tsp00_KeyPtr    &pPrimKey,
    tsp00_Int2      &PrimKeyLen,
    bool            &HasDeletedFlag )
{
    if (thisfound == m_SearchResult)
    {
        m_InvListArray.bd450GetPrimKey (m_PKIndex, pPrimKey, PrimKeyLen);
    }
    else
    {
        pPrimKey   = NULL;
        PrimKeyLen = 0;
    }
}

/*---------------------------------------------------------------------------*/

inline cbd460_InvListIterator &
cbd460_InvListArrayIterator::operator ++ ()
{
    ROUTINE_DBG_MEO00 ("cbd460_NextPrimKey_++");

    if (m_bAscendingKeyOrder)
        m_PKIndex ++;
    else
        m_PKIndex --;

    if ( bd460_BeyondStopKey() )
        m_SearchResult.becomes (nextfound);
    else
        m_SearchResult.becomes (thisfound);

    return *this;
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListArrayIterator::bd460SkipCurrentKey()
{
    ROUTINE_DBG_MEO00 ("bd460SkipCurrentKey++");

    tsp00_KeyPtr pPrimKey;
    tsp00_Int2   primKeyLen;
    m_InvListArray.bd450GetPrimKey (m_PKIndex, pPrimKey, primKeyLen);

    ++(*this);

    if (thisfound == m_SearchResult)
    {
        m_InvListArray.bd450SkipPrimKey(pPrimKey, primKeyLen - sizeof(tgg91_TransNo), m_PKIndex, m_SearchResult);

        if (thisfound == m_SearchResult)
        {
            ++(*this);
        }
        else
        {
            if ( bd460_BeyondStopKey() )
                m_SearchResult.becomes (nextfound);
            else
                m_SearchResult.becomes (thisfound);
        }
    }
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd460_InvListArrayIterator::bd460NumPrimKeysInKeyScope()
{
    ROUTINE_DBG_MEO00 ("bd460NumPrimKeysInKeyScope_Array");

    if (m_bAscendingKeyOrder)
    {
        return m_StopPKIndex - m_PKIndex + 1;
    }
    else
    {
        if (NIL_PRIM_KEY_INDEX_BD450  == m_StopPKIndex)
            return m_PKIndex + 1;
        else
            return m_PKIndex - m_StopPKIndex + 1;
    }
}
/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListArrayIterator::bd460GetSecKey(
    tsp00_KeyPtr &pSecKey,
    tsp00_Int2   &SecKeyLen ) const // PTS 1116651 TS 2002-07-15
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListArrayIterator::bd460GetSecKey");

    m_InvListArray.bd450GetSecKey( pSecKey, SecKeyLen );
}

/*---------------------------------------------------------------------------*/

inline
cbd460_InvListArrayIterator::cbd460_InvListArrayIterator(
    tgg00_RecPtr  pRec,
    tsp00_KeyPtr  StartKey,
    tsp00_Int4    StartKeyLen,
    tsp00_KeyPtr  StopKey,
    tsp00_Int4    StopKeyLen,
    bool          bAscendingKeyOrder,
    bool          bIncludeStartKey,
    bool          bConsistentReadSupport )
        :
        m_bAscendingKeyOrder( bAscendingKeyOrder || bConsistentReadSupport),
        m_bConsistentReadSupport( bConsistentReadSupport )
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListArrayIterator");

    bd460_Init (pRec, StartKey, StartKeyLen,  StopKey, StopKeyLen, bIncludeStartKey );
}

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION OF cbd460_InvListSubtreeIteratorNoBreak             */
/*---------------------------------------------------------------------------*/

inline
cbd460_InvListSubtreeIteratorNoBreak::cbd460_InvListSubtreeIteratorNoBreak (
    cbd500_Tree                   &MotherTree,
    cbd450_InvListRefSubTree      &InvListRefSubTree,
    tsp00_KeyPtr                   pStartPrimKey,
    tsp00_Int4                     StartPrimKeyLen,
    tsp00_KeyPtr                   pStopPrimKey,
    tsp00_Int4                     StopPrimKeyLen,
    bool                           bAscendingKeyOrder,
    bool                           bIncludeFirstPrimKey,
    bool                          *pTreeIsRequested)
        :
        m_InvSubTreeCurrent( MotherTree.bd500GetTheCurrent(),
                             tbd_node_request::fromConst( nr_for_read ),
                             InvListRefSubTree, false ),
        m_SubTreeLeaf( m_InvSubTreeCurrent,
                       tbd_node_request::fromConst( nr_for_read ),
                       tbd00_PageLockMode::fromConst( plmLockWithoutReschedule_ebd00 )), // PTS 1115174 TS 2002-04-09),
        m_SubTree( MotherTree, m_SubTreeLeaf, pTreeIsRequested ),
        m_PrimKeyIter( m_SubTree,
                       pStartPrimKey, StartPrimKeyLen,
                       pStopPrimKey, StopPrimKeyLen, bAscendingKeyOrder,
                       bIncludeFirstPrimKey, m_bStartKeyFound_Dummy )
{
    InvListRefSubTree.bd450GetSecKey( m_pSecKey, m_SecKeyLen ); // PTS 1116651 TS 2002-07-15
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIteratorNoBreak::bd460Break(
    const tsp00_KeyPtr   rePositionPrimKey,
    tsp00_Int2           &rePositionPrimKeyLen )
{
    ROUTINE_DBG_MEO00 ("bd460Break_Subtree");

    m_PrimKeyIter.bd510Break( rePositionPrimKey, rePositionPrimKeyLen );
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIteratorNoBreak::bd460Continue(
    cbd450_InvListRefSubTree      &InvListRefSubTree,
    tsp00_KeyPtr                   rePositionPrimKey,
    tsp00_Int4                     rePositionPrimKeyLen )
{
    ROUTINE_DBG_MEO00 ("bd460Continue_Subtree");

    m_InvSubTreeCurrent.bd300SetSubRoot( InvListRefSubTree, false );
    InvListRefSubTree.bd450GetSecKey( m_pSecKey, m_SecKeyLen ); // PTS 1116651 TS 2002-07-15

    m_PrimKeyIter.bd510Continue( rePositionPrimKey, rePositionPrimKeyLen);
}

/*---------------------------------------------------------------------------*/

inline cbd460_InvListIterator &
cbd460_InvListSubtreeIteratorNoBreak::operator ++ ()
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIter_++");

    ++m_PrimKeyIter;
    return *this;
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIteratorNoBreak::bd460SkipCurrentKey()
{
    ROUTINE_DBG_MEO00( "bd460SkipCurrentKey_Subtree" );

    m_PrimKeyIter.bd510SkipRecord();
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIteratorNoBreak::bd460Deref(
    tsp00_KeyPtr   &pPrimKey,
    tsp00_Int2     &PrimKeyLen,
    bool           &HasDeletedFlag )
{
    ROUTINE_DBG_MEO00 ("bd460Deref_Subtree");
    m_PrimKeyIter.bd510GetKey(pPrimKey, PrimKeyLen);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd460_InvListSubtreeIteratorNoBreak::bd460NumPrimKeysInKeyScope()
{
    ROUTINE_DBG_MEO00 ("bd460NumPrimKeysInKeyScope_Subtree");

    return m_PrimKeyIter.bd510NumberOfRecordsInKeyScope ();
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIteratorNoBreak::bd460GetSecKey (
    tsp00_KeyPtr &pSecKey,
    tsp00_Int2   &SecKeyLen ) const // PTS 1116651 TS 2002-07-15
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIteratorNoBreak::bd460GetSecKey");

    pSecKey   = m_pSecKey;
    SecKeyLen = m_SecKeyLen;
}

/*---------------------------------------------------------------------------*/

inline tgg00_BasisError&
cbd460_InvListSubtreeIteratorNoBreak::bd460TrError()
{
    return m_InvSubTreeCurrent.curr_trans->trError_gg00;
}

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION OF cbd460_InvListSubtreeIterator                    */
/*---------------------------------------------------------------------------*/

inline
cbd460_InvListSubtreeIterator::cbd460_InvListSubtreeIterator(
    cbd500_Tree               &MotherTree,
    cbd510_RecordIterator     &SecKeyIter,
    cbd450_InvListRefSubTree  &InvListRefSubTree,
    tsp00_KeyPtr               pStartPrimKey,
    tsp00_Int4                 StartPrimKeyLen,
    tsp00_KeyPtr               pStopPrimKey,
    tsp00_Int4                 StopPrimKeyLen,
    bool                       bAscendingKeyOrder,
    bool                       bIncludeFirstPrimKey,
    bool                       bConsistentReadSupport )
        :
        m_pStopPrimKey( pStopPrimKey ),
        m_StopPrimKeyLen( StopPrimKeyLen ),
        m_SecKeyIter( SecKeyIter ),
        m_bTreeIsRequested(false),
        m_InvListSubtreeIteratorNoBreak( MotherTree,
                                         InvListRefSubTree,
                                         pStartPrimKey, StartPrimKeyLen,
                                         pStopPrimKey, StopPrimKeyLen,
                                         bAscendingKeyOrder,
                                         bIncludeFirstPrimKey,
                                         &m_bTreeIsRequested ),
        m_InvListArrayIterator( bAscendingKeyOrder, bConsistentReadSupport ),
        m_pUnivIterator( &m_InvListSubtreeIteratorNoBreak )
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIter");
}

/*---------------------------------------------------------------------------*/

inline  cbd460_InvListIterator&
cbd460_InvListSubtreeIterator::operator++ ()
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIter::operator ++");

    m_pUnivIterator->operator++();

    if( m_bTreeIsRequested ) // is set in cbd500SubTree::bd500_CheckStopKeyAndInterrupt in case of
    {                        // a root exclusive request. Precondition: mother tree is interruptable
        bd460_LeaveAndReEnterInvTree();
        m_bTreeIsRequested = false; // PTS 1132245 TS 2004-11-16
    }
    return (*this);
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIterator::bd460SkipCurrentKey()
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIter::bd460SkipCurrentKey");

    m_pUnivIterator->bd460SkipCurrentKey();

    if( m_bTreeIsRequested ) // is set in cbd500SubTree::bd500_CheckStopKeyAndInterrupt in case of
    {                        // a root exclusive request. Precondition: mother tree is interruptable
        bd460_LeaveAndReEnterInvTree();
        m_bTreeIsRequested = false; // PTS 1132245 TS 2004-11-16
    }
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIterator::bd460Deref(
    tsp00_KeyPtr &pPrimKey,
    tsp00_Int2   &PrimKeyLen,
    bool         &HasDeletedFlag )
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIter::bd460Deref");

    m_pUnivIterator->bd460Deref (pPrimKey, PrimKeyLen, HasDeletedFlag);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd460_InvListSubtreeIterator::bd460NumPrimKeysInKeyScope ()
{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIter::bd460NumPrim");

    return (m_pUnivIterator->bd460NumPrimKeysInKeyScope());
}

/*---------------------------------------------------------------------------*/

inline void
cbd460_InvListSubtreeIterator::bd460GetSecKey (
    tsp00_KeyPtr &pSecKey,
    tsp00_Int2   &SecKeyLen) const  // PTS 1116651 TS 2002-07-15

{
    ROUTINE_DBG_MEO00 ("cbd460_InvListSubtreeIterator::bd460GetSecKey");

    m_pUnivIterator->bd460GetSecKey( pSecKey, SecKeyLen );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif  /* GBD460_H */
