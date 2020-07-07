/*!
 * \file    OMS_CacheMissEntry.hpp
 * \author  MarkusSi
 * \brief   Entries for cache misses (see CachedKeys).
 */
/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_CACHEMISSENTRY_HPP
#define __OMS_CACHEMISSENTRY_HPP

#include "Oms/OMS_Defines.h"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

class OMS_Context;

class OMS_CacheMissEntry {   // PTS 1117571
public:
  size_t        m_keyLen;
  // Attention! m_key must be the last member in the declaration list.
  //   As the key-length is unknown in advance, the necessary space is 
  //   allocated in the construktor and placed directly behind (and 'coalesced') 
  //   with the member m_key. Because m_key has already length 1, the additionally
  //   allocated space is one byte less than the keyLen. 
  unsigned char m_key[1];

  static void* operator new (size_t sz, size_t keyLen, OMS_Context* pContext);
#if defined(OMS_PLACEMENT_DELETE)
  static void  operator delete (void* p, size_t keyLen, OMS_Context* pContext);
#endif

  inline     OMS_CacheMissEntry (const unsigned char*, size_t);
  inline    ~OMS_CacheMissEntry ();
  void       setKey(const unsigned char*, size_t);
  void       deleteSelf (OMS_Context* pContext);
  inline int Compare(const OMS_CacheMissEntry*, const OMS_CacheMissEntry*);
};

//----------------------------------------------------------------------/

inline OMS_CacheMissEntry::OMS_CacheMissEntry (const unsigned char *key,     
                                               size_t               keyLen)
{
  if (keyLen > 0){
    m_keyLen = keyLen;
  }
  else {
    m_keyLen = 0;
  }

  if (key != NULL){
    SAPDB_MemCopyNoCheck(&m_key, key, m_keyLen);
  }
  else {
    memset(&m_key, '\0', m_keyLen);   
  }
}

//----------------------------------------------------------------------/

inline OMS_CacheMissEntry::~OMS_CacheMissEntry ()
{
}

//----------------------------------------------------------------------/

inline int OMS_CacheMissEntry::Compare(const OMS_CacheMissEntry* l, 
                                       const OMS_CacheMissEntry* r) 
{
  int cmp = memcmp (l->m_key, r->m_key, m_keyLen);
  if (cmp) {
    return cmp > 0 ? 1 : -1;
  }
  else {
    return 0;
  }
}

//----------------------------------------------------------------------/

#endif  // __OMS_CACHEMISSENTRY_HPP