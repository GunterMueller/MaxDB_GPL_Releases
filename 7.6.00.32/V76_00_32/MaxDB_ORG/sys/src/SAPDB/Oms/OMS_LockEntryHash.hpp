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
#ifndef __OMS_LOCKENTRYHASH_HPP
#define __OMS_LOCKENTRYHASH_HPP

#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "ggg00.h"
#include "ggg01.h"

class  OMS_LockEntry;
struct OmsLockHandle;

/*!
  class      :  OMS_LockEntryHash

  description:  defines a hash list of instances of class OMS_LockEntry. The hash
                allows access to a OMS_LockEntry by a given OmsLockHandle.
                The OMS_LockEntryHash is a singleton, i.e. there exists exactly one instance of
                OMS_LockEntryHash in the liboms, called OMS_LockEntryHash::m_instance.
                Concurrent operations on this instance are synchronized via spinlocks.
*/

class OMS_LockEntryHash
{
public :
  typedef OMS_LockEntry* LockEntryPtr;
  OMS_LockEntryHash();
private :
  class HashSlot
  {
  public :
     RTESync_Spinlock m_slotLock;
     LockEntryPtr     m_list;
     int              m_lockRef;
     HashSlot() 
       : m_slotLock()
       , m_list(0)
       , m_lockRef(0)
     {
     }
     void AddRef()
     {
       ++m_lockRef;
     }
     void Lock()
     {
       m_slotLock.Lock();
       AddRef();
     }
     void Unlock()
     {
       --m_lockRef;
       if (0 == m_lockRef)
       {
         m_slotLock.Unlock();
       }
     }
  };

  class HashSlotLockedScope; 
  friend class OMS_LockEntryHash::HashSlotLockedScope;

  class HashSlotLockedScope
  {
  public :
    HashSlot& m_hashSlot;
    HashSlotLockedScope(HashSlot& hashSlot) 
      : m_hashSlot(hashSlot)
    {
      m_hashSlot.Lock();
    }
    HashSlotLockedScope(const HashSlotLockedScope& source) 
      : m_hashSlot(source.m_hashSlot)
    {
      m_hashSlot.AddRef();
    }
    ~HashSlotLockedScope()
    {
      m_hashSlot.Unlock();
    }
  };
#define CNT_HEAD_ENTRIES 213
  LockEntryPtr* m_headCurr;
  int           m_requestTimeout;
  HashSlot      m_head[CNT_HEAD_ENTRIES];
  inline HashSlotLockedScope GetHeadPtr(const OmsLockHandle& handle);
  void   BlockLockEntryHash();
  void   UnBlockLockEntryHash();
public:
  inline LockEntryPtr Insert(const OmsLockHandle& handle);
  void   InspectTimeout();
  bool   NextLockInfo(void** pLockInfo, tgg01_OmsLockInfo& lockInfo);
  void   Release(void* pLockEntry);
  void   SetTimeout(int timeout) 
  {
    m_requestTimeout = timeout;
  }
  int GetTimeout() const
  {
    return m_requestTimeout;
  }
public:
  static OMS_LockEntryHash &m_instance;
};

/*! endclass: OMS_LockEntryHash */

#endif  // __OMS_LOCKENTRYHASH_HPP
