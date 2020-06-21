/*!
 * \file    LVC_DispatcherCOMClassHash.hpp
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
#ifndef __LVC_DISPATCHERCOMCLASSHASH_HPP
#define __LVC_DISPATCHERCOMCLASSHASH_HPP

#include "liveCache/LVC_DispatcherHashEntry.hpp"

#define   LVC_COM_HASH_SESS_HDR_SIZE               3277

class LVC_DispatcherCOMClassHash {
public :
  typedef enum 
  {
    SearchALL,
    SearchEQ,
    SearchNE
  } HashSearchType;

  LVC_DispatcherCOMClassHash ();
  inline tsp00_Uint4 SessionHashIdx( tsp00_Uint4   session );
  tsp00_Uint4        HashInit (const tsp00_Uint4   lcSessionHashHdrSize = LVC_COM_HASH_SESS_HDR_SIZE );
  void               chainEntry(LVC_DispatcherHashEntryPtr p);
  void               ResetEntrySearch(tsp00_Uint4   session, 
    HashSearchType searchType );
  LVC_DispatcherHashEntryPtr getFreeEntry();
  LVC_DispatcherHashEntryPtr NextEntry( void );
  LVC_DispatcherHashEntryPtr GetRechainFirstEntry( 
    LVC_DispatcherHashEntry::HashEntryType *lpEntryType );
  LVC_DispatcherHashEntryPtr FindHashEntry(const GUID *lpGuid, tsp00_Uint4 session);
  void chainFreeEntry(LVC_DispatcherHashEntryPtr p);
  void* operator new( size_t sz );  /* PTS 1110403 */
  void operator delete(void* p);    /* PTS 1110403 */

private:
  tsp00_Uint4                 m_HashHdrSize;
  LVC_DispatcherHashEntryPtr  *m_HashHdrPtr;
  LVC_DispatcherHashEntryPtr  m_HashFreePtr;
  LVC_DispatcherHashEntryPtr  m_HashCurrPtr;
  tsp00_Uint4                 m_HashCurrIdx;
  tsp00_Uint4                 m_session;
  HashSearchType              m_searchType;
};

typedef LVC_DispatcherCOMClassHash *LVC_DispatcherCOMClassHashPtr;

/*---------------------------------------------------------------------------*/

inline tsp00_Uint4 LVC_DispatcherCOMClassHash::SessionHashIdx(tsp00_Uint4 session)
{
  if ( 0 != session )
    return  ( session % (m_HashHdrSize - 1)); 
  else
    return (m_HashHdrSize - 1);
}

#endif // __LVC_DISPATCHERCOMCLASSHASH_HPP
