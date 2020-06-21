/*
  @file         gbd495.h
  @author       TorstenS
  @ingroup      create index
  @brief        This modul implements temporary result files which
                which come into being during the parallel index
                creation to hold the temporary sorted index keys.
                There are two kinds of results double queue files and 
                temporary index tree files.

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


#ifndef GBD495_H
#define GBD495_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files
#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"   // PASCAL: BD_kernel_constants_and_types
/* */
#include "gbd300.h"  // CPP   : Currents for Tree's
#include "gbd450.h"  // CPP   : InvListHandling
#include "gbd460.h"  // CPP   : InvList Iterator
#include "gbd500.h"  // CPP   : Tree Handling
#include "gbd510.h"  // CPP   : RecordIterator
#include "gbd550.h"  // CPP   : QueueHandling
#include "gbd600.h"  // CPP   : NodeHandling

#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define DROP_TREE_BD495             true
#define INCLUSIVE_HEAD_PNO_BD495    true

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#ifdef BIT64
# define ALIGNMENT_BD495 8
#else
# define ALIGNMENT_BD495 4
#endif

#define PRIM_KEY_ITER_SIZE_BD495    ALIGNMENT_BD495 + (MAX_EO00 (               \
                                    sizeof(cbd460_InvListArrayIterator),        \
                                    sizeof(cbd460_InvListSubtreeIteratorNoBreak)))

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

externCpp tgg00_FileId bd495InitTempFileId ( tsp00_PageNo  Root );


class cbd495_InvQueue
{
public:

    virtual ~cbd495_InvQueue();


    virtual bool bd495GetCurrentItem(
        tsp00_BytePtr	&pPrimKey,
        tsp00_BytePtr   &pSecKey,
        tsp00_Int2		&PrimKeyLen,
        tsp00_Int2		&SecKeyLen,
        tsp00_Bool      &bNextSecKey,
        tsp00_Bool      &bLastPrimKey,
        tsp00_Bool      &bInvListRefNeeded) = 0;

    virtual void bd495GotoNextItem() = 0;

    virtual	void bd495Free( bool    bInclusiveHeadPno = true) = 0;
};

typedef	cbd495_InvQueue* tbd495_InvQueuePtr;

/*---------------------------------------------------------------------------*/

class cbd495_DoubleReadQueue : public cbd495_InvQueue
{
public:

    cbd495_DoubleReadQueue(
        tbd_current_tree	&QueueCurrent,
        tsp00_PageNo		 PrimQueuePno,
        tsp00_PageNo		 SecQueuePno);

    ~cbd495_DoubleReadQueue()
    {}

    bool bd495GetCurrentItem(
        tsp00_BytePtr	&pPrimKey,
        tsp00_BytePtr   &pSecKey,
        tsp00_Int2		&PrimKeyLen,
        tsp00_Int2		&SecKeyLen,
        tsp00_Bool      &bNextSecKey,
        tsp00_Bool      &bLastPrimKey,
        tsp00_Bool      &bInvListRefNeeded);

    static tsp00_Int4 bd495GetSize()
    {
    #   ifdef BIT64
        return ALIGN_8BYTE_EO00 (sizeof (cbd495_DoubleReadQueue));
    #   else
        return ALIGN_4BYTE_EO00 (sizeof (cbd495_DoubleReadQueue));
    #	endif
    }

    void bd495GotoNextItem();

    void bd495Free( bool bInclusiveHeadPno = true )
    {
        m_PrimKeyQueue.bd550Free( bInclusiveHeadPno );
        m_SecKeyQueue.bd550Free( bInclusiveHeadPno );
    }

private:

    void bd495_ReadInvKeyInfo();

private:

    tsp00_Bool          m_bInvListRefNeeded;  // Per current secondary key
    tsp00_Bool          m_bNextSecKey;
    tsp00_Int4			m_NumPrimKeys;        // Per current secondary key
    cbd550_ReadQueue	m_PrimKeyQueue;
    cbd550_ReadQueue	m_SecKeyQueue;
};

/*---------------------------------------------------------------------------*/

class cbd495_DoubleWriteQueue
{
public:

    cbd495_DoubleWriteQueue( tbd_current_tree	&QueueCurrent)
    :
    m_PrimKeyQueue( QueueCurrent ),
    m_SecKeyQueue( QueueCurrent ),
    m_PageCount( 2 )
    {
        if( e_ok == QueueCurrent.curr_trans->trError_gg00 ){
            return;
        }
        // Error handling, if a called constructor has failed
        m_PageCount = 0;
        m_PrimKeyQueue.bd550Free();
        m_SecKeyQueue.bd550Free ();
    }

    cbd495_DoubleWriteQueue(
        tbd_current_tree	&QueueCurrent,
        tsp00_PageNo		 PrimQueuePno,
        tsp00_PageNo		 SecQueuePno )
    :
    m_PrimKeyQueue( QueueCurrent, PrimQueuePno ),
    m_SecKeyQueue( QueueCurrent, SecQueuePno ),
    m_PageCount( 2 )
    {}

    ~cbd495_DoubleWriteQueue()
    {}

    tsp00_PageNo bd495GetPrimPno(){
        return m_PrimKeyQueue.bd550GetPno();
    }

    tsp00_PageNo bd495GetSecPno(){
        return m_SecKeyQueue.bd550GetPno();
    }

    void bd495Free( bool bInclusiveHeadPno = true )
    {
        m_PrimKeyQueue.bd550Free (bInclusiveHeadPno);
        m_SecKeyQueue.bd550Free (bInclusiveHeadPno);
        m_PageCount = 0;
    }

    static tsp00_Int4 bd495GetSize(){
        return sizeof (cbd495_DoubleWriteQueue);
    }

    void bd495PushMinRequiredPrimKeySpace( tsp00_Int4	MinRequiredPrimKeySpace ){
        m_SecKeyQueue.bd550Push (sizeof (MinRequiredPrimKeySpace),
                                 REINTERPRET_CAST (tsp00_BytePtr, &MinRequiredPrimKeySpace));
    }

    void bd495PushNumPrimKeys( tsp00_Int4 NumPrimKeys ){
        m_SecKeyQueue.bd550Push (sizeof (NumPrimKeys), REINTERPRET_CAST (tsp00_BytePtr, &NumPrimKeys));
    }

    void bd495ReservePrimKeySpace(
        tsp00_BytePtr	&pPrimKey,
        tsp00_Int4		 PrimKeyLen )
    {
        this->bd495_ReserveSpace( m_PrimKeyQueue, pPrimKey, PrimKeyLen );
    }

    void bd495ReserveSecKeySpace(
        tsp00_BytePtr	&pSecKey,
        tsp00_Int4		SecKeyLen )
    {
        this->bd495_ReserveSpace( m_SecKeyQueue, pSecKey, SecKeyLen );
    }

    tsp00_Int4 bd495GetPageCount() const{
        return m_PageCount;
    }

private:

    void bd495_ReserveSpace(
        cbd550_WriteQueue&  queue,
        tsp00_BytePtr&      pKey,
        tsp00_Int4          keyLen )
    {
        bool bNewPageAdded = false;
        queue.bd550PushWithoutCopy( keyLen, pKey, bNewPageAdded );
        if( bNewPageAdded ){
            m_PageCount+=1;
        }
    }

protected:

    cbd550_WriteQueue	m_PrimKeyQueue;
    cbd550_WriteQueue	m_SecKeyQueue;
    tsp00_Int4          m_PageCount;
};

/*---------------------------------------------------------------------------*/

class cbd495_TempInvTree : public cbd495_InvQueue
{
public:

    cbd495_TempInvTree(
        tgg00_TransContext  &Trans,
        tgg00_FileId        &InvFileId,
        bool                bDropTree = !DROP_TREE_BD495);

    ~cbd495_TempInvTree()
    {}

    bool bd495GetCurrentItem(
        tsp00_BytePtr	&pPrimKey,
        tsp00_BytePtr   &pSecKey,
        tsp00_Int2		&PrimKeyLen,
        tsp00_Int2		&SecKeyLen,
        tsp00_Bool      &bNextSecKey,
        tsp00_Bool      &bLastPrimKey,
        tsp00_Bool      &bInvListRefNeeded);

    static tsp00_Int4 bd495GetSize()
    {
    #   ifdef BIT64
        return ALIGN_8BYTE_EO00 (sizeof (cbd495_TempInvTree));
    #   else
        return ALIGN_4BYTE_EO00 (sizeof (cbd495_TempInvTree));
    #	endif
    }

    void bd495GotoNextItem();

    void bd495Free( bool bInclusiveHeadPno = true );

private:

    void bd495_NextPrimKeyIter(
        tgg00_RecPtr    pRec);

private:

    tgg00_BasisError	    &m_TrError;
    /* */
    bool                    m_bDummy;
    tsp00_Bool              m_bInvListRefNeeded;
    tsp00_Bool              m_bNextSecKey;
    tsp00_Int4              m_NumPrimKeys;
    cbd300_InvCurrent       m_InvCurrent;
    cbd600_Node             m_LeafNode;
    cbd500_Tree             m_Tree;
    cbd510_RecordIterator   m_SecKeyIter;
    cbd460_InvListIterator* m_PrimKeyIter;
    char                    m_PrimKeyIterSpace [PRIM_KEY_ITER_SIZE_BD495];
};

#endif  /* GBD495_H */
