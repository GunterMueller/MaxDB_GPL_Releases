/*!
 * \file    LVC_DispatcherCOMClassHash.cpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   OMS Stored Procedure dispatcher COM class hash.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include "liveCache/LVC_DispatcherCOMClassHash.hpp"
#include <stdlib.h>

/*---------------------------------------------------------------------------*/

LVC_DispatcherCOMClassHash::LVC_DispatcherCOMClassHash ()
{
  m_HashHdrSize   = 0;
  m_HashHdrPtr    = NULL;
  m_HashFreePtr   = NULL;
  m_HashCurrPtr   = NULL;
  m_HashCurrIdx   = 0;
  m_session       = 0;
  m_searchType    = LVC_DispatcherCOMClassHash::SearchEQ;
}

/*---------------------------------------------------------------------------*/

tsp00_Uint4 LVC_DispatcherCOMClassHash::HashInit(
  const tsp00_Uint4 lcSessionHashHdrSize )
{
  tsp00_Uint4    ix;

  /* m_HashHdrPtr    = new LVC_DispatcherHashEntryPtr [ lcSessionHashHdrSize ];        PTS 1110403 */ 
  m_HashHdrPtr = (LVC_DispatcherHashEntryPtr*) malloc( sizeof( LVC_DispatcherHashEntryPtr )      /* PTS 1110403 */
    * lcSessionHashHdrSize );                                          /* PTS 1110403 */
  if ( NULL == m_HashHdrPtr )
  {
    return ERROR_NOT_ENOUGH_MEMORY;
  }
#if defined(DEBUG_MEM_ALLOC)
  MSGD(( 11999, INFO_TYPE, "new   ", "veo38.cpp:HashInit - 0x%lx, sizeInBytes %ld",
    m_HashHdrPtr, sizeof(LVC_DispatcherHashEntryPtr) * lcSessionHashHdrSize ));
#endif /* DEBUG_MEM_ALLOC */

  m_HashHdrSize = lcSessionHashHdrSize;
  for ( ix = 0; ix < lcSessionHashHdrSize; ix++ )
    m_HashHdrPtr[ix] = NULL;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

void LVC_DispatcherCOMClassHash::chainEntry(LVC_DispatcherHashEntryPtr p)
{
  tsp00_Uint4    ix;
  ix = SessionHashIdx(p->getSession());
  p->setNextEntry( m_HashHdrPtr[ix] );
  m_HashHdrPtr[ix] = p;
}

/*---------------------------------------------------------------------------*/

void LVC_DispatcherCOMClassHash::ResetEntrySearch(tsp00_Uint4 session, 
  HashSearchType searchType )
{
  m_session              = session;
  m_HashCurrPtr   = NULL;
  m_HashCurrIdx   = 0;
  m_searchType           = searchType;
}

/*---------------------------------------------------------------------------*/

LVC_DispatcherHashEntryPtr LVC_DispatcherCOMClassHash::getFreeEntry()
{
  LVC_DispatcherHashEntryPtr p;
  p = m_HashFreePtr;
  if ( NULL == p ) {
    p = new (LVC_DispatcherHashEntry);
#if defined(DEBUG_MEM_ALLOC)
    MSGD(( 11999, INFO_TYPE, "new   ", "veo38.cpp:getFreeEntry() - 0x%lx, sizeInBytes %ld",
      p, sizeof(LVC_DispatcherHashEntry) ));
#endif /* DEBUG_MEM_ALLOC */
  }
  else {
    m_HashFreePtr = p->getNextEntry();
  }
  return p;
}

/*---------------------------------------------------------------------------*/

LVC_DispatcherHashEntryPtr LVC_DispatcherCOMClassHash::NextEntry( void )
{
  if ( LVC_DispatcherCOMClassHash::SearchEQ == m_searchType ) {
    if ( NULL == m_HashCurrPtr ) 
      m_HashCurrPtr = m_HashHdrPtr[SessionHashIdx( m_session )];
    else
      m_HashCurrPtr = m_HashCurrPtr->getNextEntry();
    while ( NULL != m_HashCurrPtr ) {
      if ( m_HashCurrPtr->sessionEQ( m_session )) 
        return m_HashCurrPtr;
      m_HashCurrPtr = m_HashCurrPtr->getNextEntry();
    }
  }
  else {
    for ( ; m_HashCurrIdx < m_HashHdrSize; m_HashCurrIdx++ ){
      bool  found;

      if ( NULL == m_HashCurrPtr ) 
        m_HashCurrPtr = m_HashHdrPtr[m_HashCurrIdx];
      else
        m_HashCurrPtr = m_HashCurrPtr->getNextEntry();
      while ( NULL != m_HashCurrPtr ) {
        switch ( m_searchType )
        {
        case LVC_DispatcherCOMClassHash::SearchALL:
          found = true;
          break;
        case LVC_DispatcherCOMClassHash::SearchNE:
          found = m_HashCurrPtr->sessionNE( m_session );
          break;
        default:
          found = false;
        }
        if ( found )
        {
          return m_HashCurrPtr;
        }
        m_HashCurrPtr = m_HashCurrPtr->getNextEntry();
      }
    }
  }
  m_HashCurrPtr   = NULL;
  m_HashCurrIdx   = 0;
  return NULL;    
}

/*---------------------------------------------------------------------------*/

LVC_DispatcherHashEntryPtr LVC_DispatcherCOMClassHash::GetRechainFirstEntry( 
  LVC_DispatcherHashEntry::HashEntryType *lpEntryType)
{
  tsp00_Uint4           ix;
  bool                  found;
  LVC_DispatcherHashEntryPtr  prev = NULL;
  LVC_DispatcherHashEntryPtr  curr = NULL;

  for ( ix = 0; ix < m_HashHdrSize; ix++) {
    if ( NULL == curr ) 
      curr = m_HashHdrPtr[ix];
    else {
      prev = curr;
      curr = curr->getNextEntry();
    }

    while ( NULL != curr ) {
      switch ( m_searchType )
      {
      case LVC_DispatcherCOMClassHash::SearchALL:
        found = true;
        break;
      case LVC_DispatcherCOMClassHash::SearchEQ:
        found = curr->sessionEQ( m_session );
        break;
      case LVC_DispatcherCOMClassHash::SearchNE:
        found = curr->sessionNE( m_session );
        break;
      default:
        found = false;
      }
      if ( found && ( NULL != lpEntryType ))
      {
        if ( curr->getType() != *lpEntryType )
          found = false;
      }
      if ( found )
      {
        if ( NULL == prev )
          m_HashHdrPtr[ix] = curr->getNextEntry();
        else
          prev->setNextEntry(curr->getNextEntry());
        /* chain into FreeChain */
        curr->setNextEntry(m_HashFreePtr);
        m_HashFreePtr = curr;
        return curr;
      }
      prev = curr;
      curr = curr->getNextEntry();
    }
    prev = NULL;
  }
  return NULL;    
}

/*---------------------------------------------------------------------------*/

void LVC_DispatcherCOMClassHash::chainFreeEntry(LVC_DispatcherHashEntryPtr p)
{
  p->setNextEntry(m_HashFreePtr);
  m_HashFreePtr = p;
}

/*---------------------------------------------------------------------------*/

LVC_DispatcherHashEntryPtr LVC_DispatcherCOMClassHash::FindHashEntry(
  const GUID *lpGuid, tsp00_Uint4 session) 
{
  LVC_DispatcherHashEntryPtr  curr;

  curr = m_HashHdrPtr[ SessionHashIdx(session) ];
  while ( NULL != curr ) {
    if ( curr->sessionEQ( session ) && curr->GuidEQ( lpGuid ) )
      return curr;
    curr = curr->getNextEntry();
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/

void* LVC_DispatcherCOMClassHash::operator new( size_t sz )    /* PTS 1110403 */
{                                                              /* PTS 1110403 */
  return malloc( sz );                                         /* PTS 1110403 */
}                                                              /* PTS 1110403 */

/*---------------------------------------------------------------------------*/

void LVC_DispatcherCOMClassHash::operator delete(void* p)      /* PTS 1110403 */
{                                                              /* PTS 1110403 */
  free( p );                                                   /* PTS 1110403 */
}                                                              /* PTS 1110403 */

/*---------------------------------------------------------------------------*/
