/*!
  @file           gbd510.h
  @author         UweH
  @ingroup        RecordIterator
  @brief          navigates within a page over records

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
#ifndef GBD510_H
#define GBD510_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"

#include "gbd600.h"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

#define ASCENDING_KEY_ORDER_BD510 true
#define INCLUDE_START_KEY_BD510   true

/// the iterator over records in a tree
class cbd510_RecordIterator
{
public:
    cbd510_RecordIterator (cbd500_Tree  &Tree,
		                   tsp00_KeyPtr  StartKey,
						   tsp00_Int4    StartKeyLen,
		                   tsp00_KeyPtr  StopKey,
						   tsp00_Int4    StopKeyLen,
						   bool          bAscendingKeyOrder,
		                   bool          bIncludeStartKey,
		                   bool         &bStartKeyFound);

    /// returns false and pKey is 0 if there is no next key
    bool bd510GetKey(tsp00_KeyPtr &pKey,
                     tsp00_Int2   &keyLength);
    /// returns 0 if there is no next record
    tgg00_RecPtr bd510GetRecord();
	cbd510_RecordIterator & operator ++ ();

    tsp00_KeyPtr  bd510LookAhead                 ();
	void          bd510Break ( tsp00_KeyPtr  rePositionKey,      // PTS 1121589 UH 2003-04-11 added
                               tsp00_Int2   &rePositionKeyLen ); // PTS 1121589 UH 2003-04-11 added
    tbd_searchresult bd510Continue ( tsp00_KeyPtr rePositionKey,
                                  tsp00_Int2   rePositionKeyLen);
	tsp00_Int4    bd510NumberOfRecordsInNode     ();
	tsp00_Int4    bd510NumberOfRecordsInKeyScope ();

    cbd600_Node & bd510Node() const;

    void bd510SkipRecord ();
private:
	cbd500_Tree         & m_Tree;
	cbd600_Node         & m_Node;

	tsp00_Int4            m_RecIndex;
	tsp00_Int4            m_StopRecIndex;
	tsp00_KeyPtr          m_pStopKey;
	tsp00_Int4            m_StopKeyLen;
	tbd_searchresult      m_SearchResult;
	bool                  m_bAscendingKeyOrder;

	cbd510_RecordIterator &bd510_NextRecord();
	cbd510_RecordIterator &bd510_PrevRecord();
    void                   bd510_Refresh       ();
	void                   bd510_SearchStopKey (bool bAscendingKeyOrder);
    bool                   bd510_BeyondStopKey ();
};

/*---------------------------------------------------------------------------*/

inline void cbd510_RecordIterator::bd510_SearchStopKey (bool bAscendingKeyOrder)
{
    ROUTINE_DBG_MEO00 ("bd510_SearchStopKey");

	if (NULL != m_pStopKey)
	{
		tbd_searchresult SearchResult;

		m_Node.bd600SearchStopKey (m_pStopKey, m_StopKeyLen, m_StopRecIndex, SearchResult);

		if (bAscendingKeyOrder)
		{
			if (lastfound == SearchResult) 
			{
				// StopKey is not located on the current leaf
				m_StopRecIndex = NIL_RECINDEX_BD00;
			}
			else
			{
				/*StopRecIndex references the key behind StopKey
				therefore StopRecIndex must reference the preceding key */
				if (nextfound == SearchResult) m_StopRecIndex --;
			}
		}
		else
		{
			if (nextfound == SearchResult) 
			{
				if (FIRST_REC_INDEX_BD00 == m_StopRecIndex)
				{
					// StopKey is not located on the current leaf
					m_StopRecIndex = NIL_RECINDEX_BD00;
				}
				// else: last valid key on page found
			}
			else
			{
				if (lastfound == SearchResult)
				{
					// every key is smaller then the StopKey: Beyond is true
					m_StopRecIndex = m_Node.bd600MaxRecIndex() + 1;
				}
				// if thisfound: last valid key on page found
			}
		}
	}
	else
		m_StopRecIndex = NIL_RECINDEX_BD00;
}
	
/*---------------------------------------------------------------------------*/

inline bool cbd510_RecordIterator::bd510_BeyondStopKey ()
{
	if (NIL_RECINDEX_BD00 == m_StopRecIndex) return false;

	if (m_bAscendingKeyOrder)
	{
		return m_RecIndex > m_StopRecIndex;
	}
	else
	{
		return m_RecIndex < m_StopRecIndex;
	}
}

/*---------------------------------------------------------------------------*/

inline void cbd510_RecordIterator::bd510_Refresh ()
{
	if (m_bAscendingKeyOrder)
		m_RecIndex = FIRST_REC_INDEX_BD00;
	else
		m_RecIndex = m_Node.bd600MaxRecIndex();
	bd510_SearchStopKey (m_bAscendingKeyOrder);
}


/*---------------------------------------------------------------------------*/

inline bool cbd510_RecordIterator::bd510GetKey(tsp00_KeyPtr &pKey,
                                               tsp00_Int2   &keyLength)
{
    if (thisfound == m_SearchResult)
        return m_Node.bd600GetKey (m_RecIndex, pKey, keyLength);
    else
    {
        pKey      = 0;
        keyLength = 0;
        return false;
    }
}

/*---------------------------------------------------------------------------*/

inline tgg00_RecPtr cbd510_RecordIterator::bd510GetRecord()
{
	if (thisfound == m_SearchResult)
        return m_Node.bd600RecPtr(m_RecIndex);
    else
        return 0;
}

/*---------------------------------------------------------------------------*/

inline cbd510_RecordIterator& cbd510_RecordIterator::bd510_NextRecord()
{
	m_RecIndex ++;

	if ( m_RecIndex > m_Node.bd600MaxRecIndex() )
	{
		// Next non empty Page is required

		do
		{
			m_Tree.bd500NextNode();
			if (e_ok != m_Node.bd600TrError() || ! m_Node.bd600IsAccessable() )
			{
				m_SearchResult.becomes (nextfound);
				return *this;
			}

		}
		while ( m_Node.bd600NumRecords() <= 0 );

		bd510_Refresh ();
	}

	if ( bd510_BeyondStopKey() )
		m_SearchResult.becomes (nextfound);
	else
		m_SearchResult.becomes (thisfound);

	return *this;
}

/*---------------------------------------------------------------------------*/

inline void cbd510_RecordIterator::bd510SkipRecord ()
{
    tsp00_KeyPtr      pKey;
    tsp00_Int2        SkipKeyLength;
    tsp00_KeyPtr      pNextKey;
    tsp00_Int2        NextKeyLength;
    tsp00_LcompResult compResult;

    m_Node.bd600GetKey(m_RecIndex, pKey, SkipKeyLength);

    while ( m_RecIndex + 1 <= m_Node.bd600MaxRecIndex() )
    {
        bd510_NextRecord();
        if (thisfound != m_SearchResult)
            return;

        m_Node.bd600GetKey(m_RecIndex, pNextKey, NextKeyLength);

        if ( SkipKeyLength == NextKeyLength )
        {
            s30cmp (pKey,     1, SkipKeyLength - sizeof(tgg91_TransNo), 
                    pNextKey, 1, SkipKeyLength - sizeof(tgg91_TransNo), compResult);
            if (l_equal != compResult)
                return;
        }
    }

    tsp00_Key skipKey;
    SAPDB_Int2 e = e_ok;
    SAPDB_RangeMove ( __FILE__, 1,    
                      sizeof(tsp00_Key), sizeof (skipKey),
                      pKey,    1,
                      skipKey, 1,
                      SkipKeyLength - sizeof(tgg91_TransNo), e); 

    while (true)
    {
        bd510_NextRecord();
        if (thisfound != m_SearchResult)
            return;

        m_Node.bd600GetKey(m_RecIndex, pNextKey, NextKeyLength);
        if ( SkipKeyLength == NextKeyLength )
        {
            s30cmp (&skipKey[0], 1, SkipKeyLength - sizeof(tgg91_TransNo), 
                    pNextKey,    1, SkipKeyLength - sizeof(tgg91_TransNo), compResult);
            if (l_equal != compResult)
                return;
        }
    }
}

/*---------------------------------------------------------------------------*/

inline cbd510_RecordIterator& cbd510_RecordIterator::bd510_PrevRecord()
{
	m_RecIndex --;

	if ( m_RecIndex < FIRST_REC_INDEX_BD00)
	{
		// Next non empty Page is required

		do
		{
			m_Tree.bd500PrevNode();

			if (e_ok != m_Node.bd600TrError() || ! m_Node.bd600IsAccessable() )
			{
				m_SearchResult.becomes (nextfound);
				return *this;
			}
		}
		while ( m_Node.bd600NumRecords() <= 0 );

		bd510_Refresh ();
	}

	if ( bd510_BeyondStopKey() )
		m_SearchResult.becomes (nextfound);
	else
		m_SearchResult.becomes (thisfound);

	return *this;
}

/*---------------------------------------------------------------------------*/

inline cbd510_RecordIterator& cbd510_RecordIterator::operator ++ ()
{
    ROUTINE_DBG_MEO00 ("bd510_++");
	if (m_bAscendingKeyOrder)
		return bd510_NextRecord();
	else
		return bd510_PrevRecord();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4 cbd510_RecordIterator::bd510NumberOfRecordsInNode ()
{
    ROUTINE_DBG_MEO00 ("bd510NumberOfRecordsInNode");

	if (m_bAscendingKeyOrder)
	{
		if (NIL_RECINDEX_BD00 == m_StopRecIndex)
			return m_Node.bd600MaxRecIndex() - m_RecIndex + 1;
		else
			return m_StopRecIndex - m_RecIndex + 1;
	}
	else
	{
		if (NIL_RECINDEX_BD00 == m_StopRecIndex)
			return m_RecIndex + 1;
		else
			return m_RecIndex - m_StopRecIndex + 1;
	}
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4 cbd510_RecordIterator::bd510NumberOfRecordsInKeyScope ()
{
    ROUTINE_DBG_MEO00 ("bd510NumberOfRecordsInKeyScope");

	tsp00_Int4 iAux = 0;

	while ( e_ok == m_Node.bd600TrError() && thisfound == m_SearchResult && m_Node.bd600IsAccessable () )
	{
		iAux += bd510NumberOfRecordsInNode ();
		if (m_bAscendingKeyOrder)
			m_Tree.bd500NextNode();
		else
			m_Tree.bd500PrevNode();
		if (e_ok != m_Node.bd600TrError() || ! m_Node.bd600IsAccessable() )
		{
			m_SearchResult.becomes (nextfound);
			break;
		}
		bd510_Refresh ();
	}

	return iAux;
}

/*---------------------------------------------------------------------------*/

inline void cbd510_RecordIterator::bd510Break ( tsp00_KeyPtr  rePositionKey,     // PTS 1121589 UH 2003-04-11 added
                                                tsp00_Int2   &rePositionKeyLen ) // PTS 1121589 UH 2003-04-11 added
{
    ROUTINE_DBG_MEO00 ("bd510Break");
	m_Tree.bd500Break (m_RecIndex, rePositionKey, rePositionKeyLen); 
}

/*---------------------------------------------------------------------------*/

inline tbd_searchresult cbd510_RecordIterator::bd510Continue (tsp00_KeyPtr rePositionKey,
								                              tsp00_Int2   rePositionKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd510Continue");

	m_Tree.bd500Continue (rePositionKey, rePositionKeyLen);
	if ( e_ok != m_Node.bd600TrError() ) 
        return m_SearchResult;

	bd510_SearchStopKey (m_bAscendingKeyOrder);
	m_Node.bd600SearchRecord (rePositionKey, rePositionKeyLen, m_RecIndex, m_SearchResult);
    
    if ( lastfound == m_SearchResult || nonefound == m_SearchResult )
    {
        // PTS 1139295 UH the record may be deleted meanwhile
        // Search for next greater key which is in the given interval
        
        if ( m_Node.bd600RightNeighbor() == NIL_PAGE_NO_GG00 )
            return m_SearchResult;

        do
        {
            m_Tree.bd500NextNode();
            if ( e_ok != m_Node.bd600TrError() || ! m_Node.bd600IsAccessable()) 
                return m_SearchResult;

            if ( m_Node.bd600NumRecords() > 0 )
            {
                m_RecIndex = FIRST_REC_INDEX_BD00;
                m_SearchResult.becomes (nextfound);
                bd510_SearchStopKey (m_bAscendingKeyOrder);
                break;
            }
        }
        while ( m_Node.bd600RightNeighbor() != NIL_PAGE_NO_GG00 );
    }

    if ( bd510_BeyondStopKey() )
        m_SearchResult.becomes (nextfound);
    else
        m_SearchResult.becomes (thisfound);
    
    return m_SearchResult;
}

/*---------------------------------------------------------------------------*/

inline tsp00_KeyPtr cbd510_RecordIterator::bd510LookAhead()
{
    ROUTINE_DBG_MEO00 ("bd510LookAhead");

	// mm_direct and return_knf (key join optimization)
	// KeyOrder must be ascending

	if ( m_RecIndex+1 > m_Node.bd600MaxRecIndex() )
		return NULL;
	else
		return reinterpret_cast<tsp00_KeyPtr>(m_Node.bd600SpacePtr (m_RecIndex+1));
}

/*---------------------------------------------------------------------------*/

inline  cbd600_Node & cbd510_RecordIterator::bd510Node() const
{
	return m_Node;
}

/*---------------------------------------------------------------------------*/

inline cbd510_RecordIterator::cbd510_RecordIterator (cbd500_Tree   &Tree,
                                                     tsp00_KeyPtr   pStartKey,
                                                     tsp00_Int4     StartKeyLen,
                                                     tsp00_KeyPtr   pStopKey,
                                                     tsp00_Int4     StopKeyLen,
                                                     bool           bAscendingKeyOrder,
                                                     bool           bIncludeStartKey,
                                                     bool          &bStartKeyFound)
: m_Tree (Tree),
  m_Node     (Tree.bd500CurrentNode())
{
    ROUTINE_DBG_MEO00 ("cbd510_RecordIterator");


    if (e_ok != m_Node.bd600TrError()) return; // PTS 1104721 TS 1999-11-23

	// SELECT DIRECT: precondition is that both pointers to StartKey and StopKey are equal
	// (mm_direct)

    /* initialize member variables */
	m_RecIndex     = NIL_RECINDEX_BD00;
	m_StopRecIndex = NIL_RECINDEX_BD00;
	m_pStopKey     = pStopKey;
	m_StopKeyLen   = StopKeyLen;
    m_SearchResult.becomes  (nonefound);
	m_bAscendingKeyOrder = bAscendingKeyOrder;

	/* search start key in index */
	if (NULL == pStartKey)
	{
		if ( m_bAscendingKeyOrder )
		{
			Tree.bd500SetToFirstLeaf ();
			if (m_Node.bd600TrError() != e_ok) return;

			m_RecIndex = FIRST_REC_INDEX_BD00;
			if ( m_Node.bd600NumRecords() > 0 )
				m_SearchResult.becomes(nextfound);
		}
		else
		{
			Tree.bd500SetToLastLeaf ();
			if (m_Node.bd600TrError() != e_ok) return;

			m_RecIndex = m_Node.bd600MaxRecIndex();
			if ( m_Node.bd600NumRecords() > 0 )
				m_SearchResult.becomes(lastfound);
		}
	}
	else
	{
		Tree.bd500FindNode (pStartKey, StartKeyLen, LEAF_LEVEL_BD00);
		if (m_Node.bd600TrError() != e_ok) return;

        if (bIncludeStartKey)
        {
		    m_Node.bd600SearchRecord (pStartKey, StartKeyLen, m_RecIndex, m_SearchResult);
        }
        else
        {
            m_Node.bd600SearchStopKey (pStartKey, StartKeyLen, m_RecIndex, m_SearchResult);
        }
	}

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01p2int4 (bd_inv, "CurrNodeId  ", m_Node.bd600GetId(), "RecIndex    ", m_RecIndex);
	t01p2int4 (bd_inv, "SearchResult", m_SearchResult,      "Ascending   ", bAscendingKeyOrder);
#   endif

	if ( pStartKey != pStopKey )
		bd510_SearchStopKey (bAscendingKeyOrder);
	else if ( NULL != pStartKey )
	{
		// select direct => prevent "++" from being successfull
		if (thisfound == m_SearchResult)
		    m_StopRecIndex = m_RecIndex;
		else
		{
			m_SearchResult.becomes (nextfound);
			m_StopRecIndex = m_RecIndex - 1;
		}
		return;
	}

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01p2int4 (bd_inv, "SearchResult", m_SearchResult, "StopRecIndex", m_StopRecIndex);
#   endif

	bStartKeyFound = true;

	if (bAscendingKeyOrder)
	{
		if ( nextfound == m_SearchResult )
		{
			if (! bd510_BeyondStopKey() )
				m_SearchResult.becomes (thisfound);
			bStartKeyFound = false;
		}
		else
			if ( (lastfound == m_SearchResult) ||
				 (! bIncludeStartKey && (thisfound == m_SearchResult)) )
			{
				++(*this);
				bStartKeyFound = false;
			}

			// if the startkey is not found on current leaf,
			// we have to move to the next leaf by "++"
			// if the starkey was found and we don't want the starkey,
			// we have to move to the next record by "++"

	}
	else
	{
		if ( lastfound == m_SearchResult )
		{
			if (! bd510_BeyondStopKey() )
				m_SearchResult.becomes (thisfound);
			bStartKeyFound = false;
		}
		else
			if ( nextfound == m_SearchResult  ||
				 (! bIncludeStartKey && (thisfound == m_SearchResult)) )
			{
				++(*this);
				bStartKeyFound = false;
			}
	}
}

#endif  /* GBD510_H */