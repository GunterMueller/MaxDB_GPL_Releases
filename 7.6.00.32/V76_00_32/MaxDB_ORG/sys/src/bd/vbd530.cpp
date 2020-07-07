/*!
  @file           vbd530.cpp
  @author         UweH
  @ingroup        cbd500_Tree
  @brief          Methods for managing record-space

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
#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd500.h"
#include "gbd600.h"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd530AddRecordSpace (tsp00_KeyPtr  pKey,
		                          tsp00_Int4    KeyLen,
						          tsp00_Int4    RequiredRecLen,
								  tgg00_RecPtr &pRec)
{
	ROUTINE_DBG_MEO00 ("bd530AddRecordSpace");

	tsp00_Int4       RecIndex;
	tbd_searchresult SearchResult;
	
	bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
    if (m_TrError != e_ok) return;

	m_Node.bd600SearchRecord (pKey, KeyLen, RecIndex, SearchResult);
	if (m_TrError != e_ok) return;

	if ( thisfound == SearchResult )
	{
		m_TrError = e_duplicate_key;
		return;
	}

	/* check if the new record suits into current page */

	if ( ! m_Node.bd600SpaceIsAvailable(RequiredRecLen + POINTERSIZE_BD00) )
	{
		if ( m_Node.m_Current.bd300RootRequestKind() != nr_for_update )
		{
			/* release current Leaf Node */
			m_Node.bd600Release (! IS_CHANGED_BD600);

			/* get exclusive tree lock  */
			if ( m_Node.m_Current.bd300LockTreeExclusive() )
                if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                    bd500_WriteTrace("bd530AddRecordSpace: Overflow",29);
			if (m_TrError != e_ok) return;

			/* search leaf again */
			bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
			if (m_TrError != e_ok) return;

			m_Node.bd600SearchRecord (pKey, KeyLen, RecIndex, SearchResult);
			if (m_TrError != e_ok) return;

			if ( thisfound == SearchResult )
			{
				m_TrError = e_duplicate_key;
				return;
			}
		}

		(void) bd520LeafOverflow (pKey, KeyLen, true, RequiredRecLen, RecIndex);
		if (m_TrError != e_ok) return;
	}

    bool        withConsistentReadInfo;
    SAPDB_Int4  Length;
    SAPDB_Byte* pRecord;
    
    m_Node.m_RecordList.GetRecord (RecIndex, withConsistentReadInfo, Length, pRecord);
    
	m_Node.bd600AddRecordSpace (RequiredRecLen + POINTERSIZE_BD00, RecIndex, pRecord, false);
    
    pRec = reinterpret_cast<tgg00_RecPtr>(pRecord);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd530AppendRecordSpace (tsp00_KeyPtr   pKey,
		                             tsp00_Int4     KeyLen,
						             tsp00_Int4     RequiredRecLen,
									 tsp00_BytePtr &pSpace)
{
	ROUTINE_DBG_MEO00 ("bd530AppendRecordSpace");

	/* POST: no duplicate key check is done */

    if ( ! m_Node.bd600IsAccessable() ||
         ! m_Node.bd600IsLeaf()       ||
         (NIL_PAGE_NO_GG00 != m_Node.m_pNode->nd_right()))
    {
	    bd500SetToLastLeaf();
        if (m_TrError != e_ok) return;
    }

	tsp00_Int4 RecIndex = m_Node.bd600MaxRecIndex() + 1;

	/* check if the new record suits into current page */

	if ( ! m_Node.bd600SpaceIsAvailable(RequiredRecLen + POINTERSIZE_BD00) )
	{
		if ( m_Node.m_Current.bd300RootRequestKind() != nr_for_update )
		{
			/* release current Leaf Node */
			m_Node.bd600Release (! IS_CHANGED_BD600);

			/* get exclusive tree lock  */
			if ( m_Node.m_Current.bd300LockTreeExclusive() )
                if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                    bd500_WriteTrace("bd530AppendRecordSpace: Overflow",32);
			if (m_TrError != e_ok) return;

			/* search leaf again */
			bd500SetToLastLeaf();
			if (m_TrError != e_ok) return;
		}

		(void) bd520LeafOverflow (pKey, KeyLen, true, RequiredRecLen, RecIndex);
		if (m_TrError != e_ok) return;
	}
	m_Node.bd600AddRecordSpace (RequiredRecLen + POINTERSIZE_BD00, RecIndex, pSpace, false);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd530UpdRecordSpace (tsp00_KeyPtr  pKey,
		                          tsp00_Int4    KeyLen,
						          tsp00_Int4    RecSizeDelta,
								  tgg00_RecPtr &pRec)
{
	ROUTINE_DBG_MEO00 ("bd530UpdRecordSpace");

	tsp00_Int4       RecIndex;
	tbd_searchresult SearchResult;
	
	bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
    if (m_TrError != e_ok) return;

	m_Node.bd600SearchRecord (pKey, KeyLen, RecIndex, SearchResult);
	if (m_TrError != e_ok) return;

	if ( thisfound != SearchResult )
	{
		m_TrError = e_key_not_found;
		return;
	}

    tsp00_Int4  oldRecLength;
    bool        dummy;
    SAPDB_Byte *pSpace;

    m_Node.m_RecordList.GetRecord(RecIndex, dummy, oldRecLength, pSpace);

    pRec = reinterpret_cast<tgg00_RecPtr>(pSpace);
    
	if ( RecSizeDelta > 0 )
	{
		/* check if the new record suits into current page */
        
		if ( ! m_Node.bd600SpaceIsAvailable( RecSizeDelta) )
		{
			if ( m_Node.m_Current.bd300RootRequestKind() != nr_for_update )
			{
				/* release current Leaf Node */
				m_Node.bd600Release (! IS_CHANGED_BD600);

				/* get exclusive tree lock  */
				if ( m_Node.m_Current.bd300LockTreeExclusive() )
                    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                        bd500_WriteTrace("bd530UpdRecordSpace: Overflow",29);
				if (m_TrError != e_ok) return;

				/* search leaf again */
				bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
				if (m_TrError != e_ok) return;

				m_Node.bd600SearchRecord (pKey, KeyLen, RecIndex, SearchResult);
				if (m_TrError != e_ok) return;

				if ( thisfound != SearchResult )
				{
					m_TrError = e_key_not_found;
					return;
				}
			}

			(void) bd520LeafOverflow (pKey, KeyLen, true, RecSizeDelta, RecIndex);
			if (m_TrError != e_ok) return;
		}
	}

	m_Node.bd600UpdRecordSpace (RecSizeDelta, RecIndex, pRec);
	if (m_TrError != e_ok) return;

	if ( RecSizeDelta < 0 )
		(void) bd520LeafUnderFlow (pKey, KeyLen);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd530DelRecord (tsp00_KeyPtr    pKey,
		                     tsp00_Int4      KeyLen)
{
	ROUTINE_DBG_MEO00 ("bd530DelRecord");

	tsp00_Int4       RecIndex;
	tbd_searchresult SearchResult;
	
	bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
    if (m_TrError != e_ok) return;

	m_Node.bd600SearchRecord (pKey, KeyLen, RecIndex, SearchResult);
	if (m_TrError != e_ok) return;

	if ( thisfound != SearchResult )
	{
		m_TrError = e_key_not_found;
		return;
	}

	m_Node.bd600DelRecordSpace (RecIndex);
	if (m_TrError != e_ok) return;

	(void) bd520LeafUnderFlow (pKey, KeyLen);
}

/*---------------------------------------------------------------------------*/

void
cbd500_Tree::bd530GetRecord (tsp00_KeyPtr  pKey,
		                     tsp00_Int4    KeyLen,
						     tgg00_RecPtr &pRec)
{
	ROUTINE_DBG_MEO00 ("bd530GetRecord");

	tsp00_Int4       RecIndex;
	tbd_searchresult SearchResult;
	
	bd500FindNode (pKey, KeyLen, LEAF_LEVEL_BD00);
    if (m_TrError != e_ok) return;

	m_Node.bd600SearchRecord (pKey, KeyLen, RecIndex, SearchResult);
	if (m_TrError != e_ok) return;

	if ( thisfound != SearchResult )
	{
		m_TrError = e_key_not_found;
		return;
	}
	pRec = m_Node.bd600RecPtr(RecIndex);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
