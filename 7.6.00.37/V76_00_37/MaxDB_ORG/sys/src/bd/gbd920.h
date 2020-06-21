/****************************************************************************

  module      : gbd920.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: container for object file info
  description : the container bases on a chained list hash


  last changed: 2002-12-02  9:33
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    (c) 1999-2004 SAP AG



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


#ifndef GBD920_H
#define GBD920_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <new.h>

#include "gsp00.h"
#include "ggg00.h"
#include "ggg01.h"  // OMS errors
#include "hgg08.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp" 
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  FORWARD CLASS DECLARATION                                                *
 *===========================================================================*/

// K: item key type
// T: type of the item to be contained 
// N: width of the hash
// M: number of item frames the hash allocates if a new Item should be inserted but no emty frame is available

template <class K, class T, int N, int M> class cbd920_ChainedListHash;
template <class K, class T, int N, int M> class cbd920_HashIter;

/*===========================================================================*
 *  CLASS DECLARATION                                                        *
 *===========================================================================*/

template <class K, class T, int N, int M> class cbd920_HashItemFrame 
{	
public:
	T                        m_Item;     
	K                        m_ItemKey;
	cbd920_HashItemFrame<K,T,N,M> *m_pNextItemFrame;
	cbd920_HashItemFrame() : m_pNextItemFrame (NULL), m_ItemKey(0) {};
};

/*---------------------------------------------------------------------------*/


template <class K, class T, int N, int M> class cbd920_ChainedListHash
{
	friend class cbd920_HashIter <K,T,N,M>;

public: // class definitions

	/* --- local class CItemFrame  ---- */
	typedef cbd920_HashItemFrame<K,T,N,M> CItemFrame;

private: // class definitions

    /* --- local class CItemFrameChunk ---*/
	struct CItemFrameChunk
	{
        CItemFrameChunk * m_pNextItemFrameChunk;
		tsp00_Int4        m_ChunkIndex;
		CItemFrame        m_ItemFrame [M];
		CItemFrameChunk(){};
	};
	/* --------------------------------- */
	
public: // methods
	T*         bd920InsItem     (const tsp00_TaskId TaskId, const K &Key, T *Item, tgg00_BasisError &TrError);  
	void       bd920DelItem     (const tsp00_TaskId TaskId, const K &Key);
	void       bd920DelAllItems (const tsp00_TaskId TaskId);
	unsigned int bd920Verify    () const;	
	T         &operator[]       (const K &Key);
	tsp00_Bool bd920IsUsed      (const K &Key);   // PTS 1118512 2-Dec-2002 FF
	int        bd920NumItems    ();
	cbd920_ChainedListHash      ();
    cbd920_ChainedListHash      (SAPDBMem_IRawAllocator& alloc);
	
private:// members
    SAPDBMem_IRawAllocator& m_alloc;
	int              m_NumFreeFrames;
	int              m_NumItemFrameChunks;
	T                m_NullItem;
	CItemFrame      *m_pFirstItemFrame [N];
	CItemFrame      *m_pFirstFreeItemFrame;
	CItemFrameChunk *m_FirstItemFrameChunk;
	
private:// methods
	int    Hash  (const K & Key) const {return (int)( Key % N);};
	void   bd920_DelItem (CItemFrame* &pItemFrame);
	void   bd920_AllocateNewFreeItemFrames (tgg00_BasisError   &TrError);
};

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> class cbd920_HashIter
{
	typedef cbd920_ChainedListHash<K,T,N,M> C;
	typedef cbd920_HashItemFrame  <K,T,N,M> CItemFrame;

private: 
	C              *m_pChainedHashList;
    int             m_CurrHashSlotNo;
    CItemFrame     *m_pCurrItemFrame;

public:
    cbd920_HashIter        (C & ChainedHashList);
	      T * operator  () ();
	      T * operator  ++ ();
	      T * bd920Begin   ();
   tsp00_Bool bd920End     ();
	const K & bd920Key     () const; 
};

/*---------------------------------------------------------------------------*/
/*
class cbd920_HashError
{
public:
	tsp00_Int4       m_ErrNo;
	cbd920_HashError (ErrNo) :m_ErrNo(ErrNo){};
};
*/
/*===========================================================================*
 *  CLASS METHOD IMPLEMENTATION                                              *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/* class cbd920_HashIter method implementation                               */
/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline
cbd920_HashIter <K,T,N,M>::cbd920_HashIter (C & ChainedHashList)
: m_pChainedHashList (&ChainedHashList),
  m_pCurrItemFrame   (NULL)
{};

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline T *
cbd920_HashIter <K,T,N,M>::operator () ()
{
    /* check, if in FREE chain */
    if (0 == m_pCurrItemFrame->m_ItemKey)
        return NULL;
    else
        return &m_pCurrItemFrame->m_Item;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline T *
cbd920_HashIter <K,T,N,M>::operator ++ ()
{
	if (NULL == m_pCurrItemFrame)
		return NULL;

	m_pCurrItemFrame = m_pCurrItemFrame->m_pNextItemFrame;
	while (NULL == m_pCurrItemFrame)
	{
		if(N - 1 <= m_CurrHashSlotNo)
			return NULL;
		else
		{
			++ m_CurrHashSlotNo; 
			m_pCurrItemFrame = m_pChainedHashList->m_pFirstItemFrame [m_CurrHashSlotNo];
		}
	}
    /* check, if in FREE chain */
    if (0 == m_pCurrItemFrame->m_ItemKey)
        return NULL;
    else
        return &m_pCurrItemFrame->m_Item;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline T *
cbd920_HashIter <K,T,N,M>::bd920Begin()
{
	m_CurrHashSlotNo = 0;

	while (NULL == m_pChainedHashList->m_pFirstItemFrame [m_CurrHashSlotNo])
	{
		if(N - 1 <= m_CurrHashSlotNo)
			return NULL;
		else
			++ m_CurrHashSlotNo; 
	}

   m_pCurrItemFrame = m_pChainedHashList->m_pFirstItemFrame [m_CurrHashSlotNo];
   return &m_pCurrItemFrame->m_Item;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline tsp00_Bool
cbd920_HashIter <K,T,N,M>::bd920End()
{
	return ((NULL == m_pCurrItemFrame) || (0 == m_pCurrItemFrame->m_ItemKey));
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline const K &
cbd920_HashIter <K,T,N,M>::bd920Key() const
{
	return m_pCurrItemFrame->m_ItemKey;
}

/*---------------------------------------------------------------------------*/
/* class cbd920_ChainedListHash method implementation                        */
/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline
cbd920_ChainedListHash <K,T,N,M>::cbd920_ChainedListHash()
:
m_alloc               (RTEMem_Allocator::Instance()),
m_NumFreeFrames       (0),
m_NumItemFrameChunks  (0), 
m_pFirstFreeItemFrame (NULL),
m_FirstItemFrameChunk (NULL)
{
	/* initialize hash table */
	for (int i = 0; i < N; ++i)
		m_pFirstItemFrame[i] = NULL;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline
cbd920_ChainedListHash <K,T,N,M>::cbd920_ChainedListHash(SAPDBMem_IRawAllocator& alloc)
:
m_alloc               (alloc),
m_NumFreeFrames       (0),
m_NumItemFrameChunks  (0), 
m_pFirstFreeItemFrame (NULL),
m_FirstItemFrameChunk (NULL)
{
	/* initialize hash table */
	for (int i = 0; i < N; ++i)
		m_pFirstItemFrame[i] = NULL;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline T* 
cbd920_ChainedListHash <K,T,N,M>::bd920InsItem (const tsp00_TaskId  TaskId, 
												const K            &Key,
												T                  *pItem, 
												tgg00_BasisError   &TrError)
{
	/* determine chain where the items has to be inserted (outside a critical section) */
	CItemFrame * &pFirstItemFrame = (m_pFirstItemFrame [Hash(Key)]);
	
	/* check if the key is unique */
	for (CItemFrame* pItemFrame = pFirstItemFrame;
	pItemFrame != NULL;
	pItemFrame  = pItemFrame->m_pNextItemFrame)
	{
		if (Key == pItemFrame->m_ItemKey)
			TrError = e_duplicate_hash_key; //?? in future this should be done by exceptions
	}
	
	/* check if there is still a free frame. if not try to allocate new free items*/
	if (m_pFirstFreeItemFrame == NULL) 
	{
		/* allocate new free items */
		bd920_AllocateNewFreeItemFrames (TrError);
		
		/* return if allocation failed */
		if (e_ok != TrError) return NULL;
	}
	
	/* get a free item frame from the chain of free frames */
	--m_NumFreeFrames;
	CItemFrame* pNewItemFrame = m_pFirstFreeItemFrame;
	pNewItemFrame->m_Item.assign(*pItem,TrError);
    if ( e_ok == TrError )
    {
        m_pFirstFreeItemFrame = pNewItemFrame->m_pNextItemFrame;
        
        /* insert new item frame at the beginning of the appropriate hash chain */
        pNewItemFrame->m_pNextItemFrame = pFirstItemFrame;
        pFirstItemFrame                 = pNewItemFrame;
        
        /* copy item and key into item frame and mark frame as used */
        pNewItemFrame->m_ItemKey     = Key;
        return &pNewItemFrame->m_Item;
    }
    return NULL;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline T &
cbd920_ChainedListHash <K,T,N,M>::operator[] (const K & Key) 
{
	bool      bTryAgain;
	const int HashSlotNo                  = Hash(Key);
	CItemFrame* volatile &pFirstItemFrame = m_pFirstItemFrame [HashSlotNo];
	
	do
	{
		bTryAgain = false;
		for (CItemFrame* pItemFrame = pFirstItemFrame;
	    	pItemFrame != NULL;
	    	pItemFrame  = pItemFrame->m_pNextItemFrame)
		{				 
			if (Key == pItemFrame->m_ItemKey) 
				return pItemFrame->m_Item;
			
			/* check for writer/reader collision */
			if (HashSlotNo != Hash (pItemFrame->m_ItemKey))
			{
				/* return control to an other task hoping */
				/* that the writer finishs his work       */  
				// ??? Sleep (0);
				
				/* repeat search for key */
				bTryAgain = true;
				break;
			}
		}
	}
	while (bTryAgain);

	/* no item for this key found */
	return m_NullItem;
}

/*---------------------------------------------------------------------------*/
// PTS 1118512 2-Dec-2002 FF
template <class K, class T, int N, int M> inline tsp00_Bool
cbd920_ChainedListHash <K,T,N,M>::bd920IsUsed (const K & Key) 
{
	bool      bTryAgain;
	const int HashSlotNo                  = Hash(Key);
	CItemFrame* volatile &pFirstItemFrame = m_pFirstItemFrame [HashSlotNo];
	
	do
	{
		bTryAgain = false;
		for (CItemFrame* pItemFrame = pFirstItemFrame;
	    	pItemFrame != NULL;
	    	pItemFrame  = pItemFrame->m_pNextItemFrame)
		{				 
			if (Key == pItemFrame->m_ItemKey) 
				return true;
			
			/* check for writer/reader collision */
			if (HashSlotNo != Hash (pItemFrame->m_ItemKey))
			{
				/* return control to an other task hoping */
				/* that the writer finishs his work       */  
				// ??? Sleep (0);
				
				/* repeat search for key */
				bTryAgain = true;
				break;
			}
		}
	}
	while (bTryAgain);

	/* no item for this key found */
	return false;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline void  
cbd920_ChainedListHash <K,T,N,M>::bd920_DelItem (CItemFrame* &pItemFrame)
{
	CItemFrame* pItemFrameToDelete = pItemFrame;

    /* check, if already if FREE chain */
    if (0 == pItemFrameToDelete->m_ItemKey)
        return;

    /* remove frame from chained hash list */
	pItemFrame = pItemFrameToDelete->m_pNextItemFrame;
    pItemFrameToDelete->m_ItemKey        = 0;
	
	/* insert frame into free chain */
	pItemFrameToDelete->m_pNextItemFrame = m_pFirstFreeItemFrame;
	m_pFirstFreeItemFrame                = pItemFrameToDelete;
	++m_NumFreeFrames;
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline void  
cbd920_ChainedListHash <K,T,N,M>::bd920DelItem (const tsp00_TaskId TaskId, const K & Key)
{
	CItemFrame**   ppItemFrame = &m_pFirstItemFrame [Hash(Key)];

	/* enter critical section */
	// already done outside the hash CSynchObject   SynchObj(TaskId);
	
	for (;*ppItemFrame != NULL; ppItemFrame = &((*ppItemFrame)->m_pNextItemFrame))
	{
		if (Key == (*ppItemFrame)->m_ItemKey)
		{
			bd920_DelItem (*ppItemFrame);
			return;
		}
	}
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline void  
cbd920_ChainedListHash <K,T,N,M>::bd920DelAllItems (const tsp00_TaskId TaskId)
{
	for (int HashSlot=0; HashSlot<N; ++ HashSlot)
	{
		while (NULL != m_pFirstItemFrame [HashSlot])
		{
			bd920_DelItem (m_pFirstItemFrame [HashSlot]);
		}
	}
}

/*---------------------------------------------------------------------------*/


template <class K, class T, int N, int M> inline int  
cbd920_ChainedListHash <K,T,N,M>::bd920NumItems ()
{
	return (m_NumItemFrameChunks*M - m_NumFreeFrames);
}

/*---------------------------------------------------------------------------*/

template <class K, class T, int N, int M> inline unsigned int  
cbd920_ChainedListHash <K,T,N,M>::bd920Verify() const
{
	int       i;
	int       NumUsedFrames  = 0;
	int       NumFreeFrames  = 0;	
	int       Num2FreeFrames = 0;

	CItemFrame*  pItemFrame;

	/* check that the null item is still like is unchanged */
	T NullItem;
	if (memcmp (&NullItem, &m_NullItem, sizeof(m_NullItem)) != 0)
		return 1;
	
	/* goto chains of occupied frames */
	for (i=0; i<N; ++i)
	{
		for (pItemFrame=m_pFirstItemFrame [i];
		     NULL != pItemFrame;
		     pItemFrame = pItemFrame->m_pNextItemFrame)
		{
			++NumUsedFrames;
			if (NumUsedFrames > (M*m_NumItemFrameChunks))
				return 2;
			
			/* check if this items belongs really to this chain */
			if (i != Hash(pItemFrame->m_ItemKey)) 
				return 3;
		}
	}
	
	/* goto chain of free frames */
	for (pItemFrame = m_pFirstFreeItemFrame;
	     NULL != pItemFrame;
	     pItemFrame = pItemFrame->m_pNextItemFrame)
	{
		++NumFreeFrames;
		if (NumFreeFrames > (M*m_NumItemFrameChunks))
			return 4;
		
		/* check if item is free,i.e.  key is null */
		if (pItemFrame->m_ItemKey != 0)
			return 5;
	};

	
    /* go through all chunks of items and count free Items */
	CItemFrameChunk * pItemFrameChunk = m_FirstItemFrameChunk;
	int NumFoundChunks = 0;
	while (NULL != pItemFrameChunk)
	{
		/* check index of chunk */
		if ((pItemFrameChunk ->m_ChunkIndex) != (m_NumItemFrameChunks - NumFoundChunks -1))
			return 6;

		++NumFoundChunks;
		
		/* count number of free item frames */
		for (i=0;i<M;++i)
		{
			if (pItemFrameChunk->m_ItemFrame[i].m_ItemKey == 0)
				++Num2FreeFrames;
		}
		pItemFrameChunk = pItemFrameChunk->m_pNextItemFrameChunk;		
	}
	
	/* check counters */	
	if (NumFreeFrames  != m_NumFreeFrames)
		return 7;
	
	if (Num2FreeFrames != m_NumFreeFrames)
		return 8;
	
	if ((NumFreeFrames + NumUsedFrames) != (M*m_NumItemFrameChunks))
		return 9;

    return 0;
}

/*---------------------------------------------------------------------------*/
 

template <class K, class T, int N, int M> inline void  
cbd920_ChainedListHash <K,T,N,M>::bd920_AllocateNewFreeItemFrames (tgg00_BasisError   &TrError)
{            	
	/* get memory block */
	CItemFrameChunk* pItemFrameChunk = new (m_alloc) CItemFrameChunk;
	
	if (NULL == pItemFrameChunk)
		TrError = e_sysbuf_storage_exceeded; // in future this should be done by an exception
	else
	{
		/* insert chunk into chain of chunks */
		pItemFrameChunk->m_pNextItemFrameChunk = m_FirstItemFrameChunk;
		m_FirstItemFrameChunk                  = pItemFrameChunk;
		pItemFrameChunk->m_ChunkIndex          = m_NumItemFrameChunks;
		++m_NumItemFrameChunks;
		
		/*  concatinate free item frames */
		for (int i = 0; i < M - 1; ++i) 
		{
			pItemFrameChunk->m_ItemFrame[i].m_pNextItemFrame =
				&(pItemFrameChunk->m_ItemFrame[i+1]);
		}
		
		/* insert new free items frames into chain of free items frame */
		pItemFrameChunk->m_ItemFrame[M-1].m_pNextItemFrame = m_pFirstFreeItemFrame;
		m_pFirstFreeItemFrame = &(pItemFrameChunk->m_ItemFrame[0]);
		
		/* update counter of free item frames */
		m_NumFreeFrames += M;
	}
}

/*---------------------------------------------------------------------------*/

#endif  /* GBD920_H */
