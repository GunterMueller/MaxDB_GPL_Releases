/*!
 * \file    LVC_DispatcherHashEntry.hpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   OMS Stored Procedure dispatcher hash entry.
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

#ifndef __LVC_DISPATCHERHASHENTRY_HPP
#define __LVC_DISPATCHERHASHENTRY_HPP

#include "Oms/OMS_Defines.h"
#include "gsp00.h"

class LVC_DispatcherHashEntry{
public:
  typedef enum { 
    ENTRY_Free,   
    ENTRY_CLSID,
    ENTRY_Invalid_CLSID,
    ENTRY_IID,
    ENTRY_Invalid_IID
  } HashEntryType;


  LVC_DispatcherHashEntry ();
  bool        getDebugFlag( void );
  bool        getRemoteDbgFlag( void );
  GUID        *getGuid();
  LPUNKNOWN   getIUnknown( void );
  bool        getIsCommittedFlag( void );
  LVC_DispatcherHashEntry  *getNextEntry();
  tsp00_Uint4 getSession();
  HashEntryType getType( void );
  bool        GuidEQ(const GUID *lpguid);
  void        init(tsp00_Uint4   session, 
    const LPGUID lpGuid, 
    LPUNKNOWN    lpInterface, 
    bool         remoteDebugFlag,
    LVC_DispatcherHashEntry::HashEntryType hType );
  bool sessionEQ( tsp00_Uint4   session );
  bool sessionNE( tsp00_Uint4   session );
  void setDebugFlag(bool flag);
  void setIsCommittedFlag(bool flag);
  void setNextEntry( LVC_DispatcherHashEntry* next );
  void setType( LVC_DispatcherHashEntry::HashEntryType type );
  void setSession( tsp00_Uint4   session );
  void* operator new( size_t sz );  /* PTS 1110403 */
  void operator delete(void* p);    /* PTS 1110403 */
  const LVC_DispatcherHashEntry & operator= (const LVC_DispatcherHashEntry& right); /* PTS 1116802 */
protected:
  LVC_DispatcherHashEntry   *m_lpnext;
  tsp00_Uint4               m_session;
  GUID                      m_iid;
  LPUNKNOWN                 m_IUnknown;
  LVC_DispatcherHashEntry::HashEntryType       m_type;
  bool                      m_idebug_used_flag;
  bool                      m_is_committed;
  bool                      m_is_remote_debug;
};
typedef LVC_DispatcherHashEntry *LVC_DispatcherHashEntryPtr;

inline bool         LVC_DispatcherHashEntry::getDebugFlag( void ) { 
  return m_idebug_used_flag; }
inline bool         LVC_DispatcherHashEntry::getRemoteDbgFlag( void ) { 
  return m_is_remote_debug; }
inline bool         LVC_DispatcherHashEntry::getIsCommittedFlag( void ) { 
  return m_is_committed; }
inline GUID   *LVC_DispatcherHashEntry::getGuid() { 
  return &m_iid; }
inline LVC_DispatcherHashEntry   *LVC_DispatcherHashEntry::getNextEntry(){ 
  return m_lpnext;}
inline LPUNKNOWN    LVC_DispatcherHashEntry::getIUnknown( void ) { 
  return m_IUnknown; }
inline tsp00_Uint4  LVC_DispatcherHashEntry::getSession() { 
  return m_session; }
inline LVC_DispatcherHashEntry::HashEntryType LVC_DispatcherHashEntry::getType( void ){ 
  return m_type; }
inline bool         LVC_DispatcherHashEntry::GuidEQ(const GUID *lpguid) {
  if (memcmp (lpguid, &m_iid, sizeof (GUID)) ) return false;
  else return true; }
inline bool         LVC_DispatcherHashEntry::sessionEQ( tsp00_Uint4   session ) {
  if ( session == m_session )  return true;
  else return false; }
inline bool LVC_DispatcherHashEntry::sessionNE( tsp00_Uint4   session ) {
  if ( session != m_session ) return true;
  else return false; }
inline void LVC_DispatcherHashEntry::setDebugFlag(bool flag) { 
  m_idebug_used_flag  = flag; };
  inline void LVC_DispatcherHashEntry::setIsCommittedFlag(bool flag) { 
    m_is_committed  = flag; };
    inline void LVC_DispatcherHashEntry::setNextEntry( LVC_DispatcherHashEntry* next ) { 
      m_lpnext = next;}
    inline void LVC_DispatcherHashEntry::setType( LVC_DispatcherHashEntry::HashEntryType type ){ 
      m_type = type; }
    inline void LVC_DispatcherHashEntry::setSession( tsp00_Uint4   session ){ 
      m_session = session; }


#endif  // __LVC_DISPATCHERHASHENTRY_HPP
