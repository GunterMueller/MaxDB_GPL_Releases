/* @lastChanged: "1999-04-21  16:14"

  * @filename:   OMS_LockObject.cpp
  * @purpose:    "Implementation of OmsLockObject"
  * @release:    7.2.0.0
  *
  * @Copyright (c) 1998-2005 SAP AG"



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

#include "Oms/OMS_LockObject.hpp"
#include "Oms/OMS_LockEntryHash.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_ISessionLockObjects.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_Handle.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "gsp00.h"
#include "ggg251.h"

/*----------------------------------------------------------------------*/

typedef OMS_SessionLockObject *OMS_SessionLockObjectPtr;

/*----------------------------------------------------------------------*/

static
void omsSignal(tsp00_TaskId TaskId, bool timeout) // PTS 1110287
{
  OMS_Globals::KernelInterfaceInstance->Signal(TaskId, timeout);
}

/*----------------------------------------------------------------------*/

static inline tsp00_Int4 omsGetClock() // PTS 1110287
{
  return OMS_Globals::KernelInterfaceInstance->Clock();
}

/*----------------------------------------------------------------------*/

class OMS_LockEntry;

/*!
  class      :  RequestQueue

  description:  implements a queue of tasks waiting for OmsLockObjects  
  
*/

class RequestQueue
{
public :
  /*!-----------------------------------------------------------------------
  
  function : Constructor

  description : constructs an empty request queue 

  parameters :  none

  return value : none

  -------------------------------------------------------------------------*/

  RequestQueue();

  /*!-----------------------------------------------------------------------
  
  function : Enter

  description : enters a task into the request queue. 

  parameters : 
    taskId [in] the identification of the task
    OMS_SessionLockObjectPtr[in] a pointer to the requested session lock object, i.e.
                             the lock object causing the wait operation.
                             
    exclusiveRequest[in] true, if an exclusive lock is requested; otherwise false
    timeout[in] maximal time to wait for lock request
  return value : none

  -------------------------------------------------------------------------*/

  void Enter(tsp00_TaskId taskId, OMS_SessionLockObjectPtr requestor, bool exclusiveRequest, int timeout);

  /*!-----------------------------------------------------------------------
  
  function : InspectTimeout

  description : traverses the queue and checks for each task, if a request timeout has accured,
                In this case the task is removed from the queue and waked up, indicating that
                a timeout has occured.

  parameters : OMS_LockEntry[in] lock for which tasks in queue are waiting for
 
  return value : none

  -------------------------------------------------------------------------*/

  void InspectTimeout(OMS_LockEntry& OMS_LockEntry);

   /*!-----------------------------------------------------------------------
  
  function : IsEmpty

  description : returns true, if the queue is empty; otherwise false

  parameters : none 

  return value : true, if the queue is empty; otherwise false

  -------------------------------------------------------------------------*/

  inline bool IsEmpty() const;

  /*!-----------------------------------------------------------------------
  
  function : WakeUpAndRemoveFirst

  description : wakes up the task identified by the first queue element, (i.e. the
                last one entered) und removes it from the queue. The task is waked up
                by a call of the Signal method indicating success.

  parameters : 
    session[in] a reference to the OMS session calling this method
    allowExclusiveLock[in] true, if a task waiting for an exclusive Lock may be
                           waked up
    taskId[out] the task identification of the task that has been waked up.
    OMS_SessionLockObjectPtr [out] a refenence to requesting session lock object 
    isExclusiveRequest [out] true, if the waked up task has been waiting for an
                             exclusive lock; otherwise false.

  return value : true, if a task has been waked up; otherwise false

  -------------------------------------------------------------------------*/

  inline bool RemoveFirst(
    bool                      allowExclusiveLock,
    tsp00_TaskId&             taskId,
    OMS_SessionLockObjectPtr& requestor,
    bool&                     isExclusiveRequest);

  friend class RequestQueueIterator;
private :
  // struct Queue Element defines an element of the queue
  struct QueueElement
  {
    QueueElement(tsp00_TaskId taskId, OMS_SessionLockObjectPtr requestor, 
      bool exclusiveRequest, int timeout)
      : m_taskId(taskId)
      , m_requestor(requestor)
      , m_next(0)
      , m_requestClock(omsGetClock() + timeout)
      , m_exclusiveRequest(exclusiveRequest)
    {
    }
    // construct instances of Queue Element on global OMS heap
    void* operator new(size_t size) 
    {
      return OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(size);
    }
    void operator delete (void* p)
    {
      OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(p);
    }
    tsp00_TaskId         m_taskId;
    OMS_SessionLockObjectPtr m_requestor;
    QueueElement*        m_next;
    tsp00_Int4           m_requestClock; // timestamp, when QueueElement times out
    bool                 m_exclusiveRequest;
  };
  QueueElement* m_last;
  QueueElement  m_sentinel;
};

/*! endclass: RequestQueue */

/*----------------------------------------------------------------------*/

/*!
  class      :  RequestQueueIterator

  description:  implements an iterator for a RequestQueue  
  
*/

class RequestQueueIterator
{
public :
  RequestQueueIterator(RequestQueue& queue)
    : m_curr(queue.m_sentinel.m_next)
  {
  }
  RequestQueueIterator(const RequestQueueIterator& source)
  {
    m_curr = source.m_curr; 
  }
  void operator++()
  {
    m_curr = m_curr->m_next;
  }
  operator bool() const
  {
    return (NULL != m_curr);
  }
  tsp00_TaskId GetTaskId() const
  {
    return m_curr->m_taskId;
  }
  int GetTimeoutClock() const
  {
    return m_curr->m_requestClock;
  }
  bool IsExclusiveRequest() const
  {
    return m_curr->m_exclusiveRequest;
  }
private :
  RequestQueue::QueueElement* m_curr;
};

/*! endclass: RequestQueueIterator */



/*!
  class      :  LockedScope

  description:  defines a synronization object based on a spin lock, i.e.
                this class allows to synchronize access to regions of code that are not 
                allowed to be entered by several tasks of context at a time.  
                The main purpose is to ensure, that the lock is freed, when an
                instance of LockedScope is destructed.
  
*/
class LockedScope
{
public :
  /*!-----------------------------------------------------------------------
  
  function : constructor

  description : constructs the LockedScope object an locks the spinlock
 
  parameters : 
    lockObj[in] a reference to the spinlock to be used

  return value : none

  -------------------------------------------------------------------------*/
  LockedScope(RTESync_Spinlock& lockObj)
    : m_pLock(&lockObj)
  {
    lockObj.Lock();
  }
  /*!-----------------------------------------------------------------------
  
  function : leave

  description : leaves the locked scope, i.e. unlocks the spinlock
 
  parameters : none 

  return value : none

  -------------------------------------------------------------------------*/
  
  void Leave()
  {
    if (m_pLock)
    {
      m_pLock->Unlock();
      m_pLock = 0;
    }
  }
  /*!-----------------------------------------------------------------------
  
  function : destructor

  description : leaves the locked scope and destructs the object
 
  parameters : none 

  return value : none

  -------------------------------------------------------------------------*/
  ~LockedScope()
  {
    this->Leave();
  }
private :
  RTESync_Spinlock* m_pLock;
};

/*! endclass: LockedScope */

/*----------------------------------------------------------------------*/

/*!
  class      :  OMS_LockEntry

  description:  an instance of class OMS_LockEntry is a shared object, that describes the
                state of an OmsLock. The OmsLock is identified by a OmsLockHandle and
                may be referenced by serveral OmsLockObjects of one or more session.
                A OMS_LockEntry may be exclusive locked by exactly one session or share
                locked by an unlimit number of sessions. 
                The OMS_LockEntry keeps book of all those sessions holding locks on the
                corresponding lock object.
                If a session holds an exclusive lock for a OMS_LockEntry and another
                session requires any lock for the same OMS_LockEntry, the requestor has to 
                wait, until the lock owner releases the lock, or until a defined
                period of time has passed (timeout).
                For this purpose the OMS_LockEntry holds a queue of tasks waiting for
                that OMS_LockEntry.
                Instances of class OMS_LockEntry are organised in a hash, which provides
                access to a OMS_LockEntry by a given OmsLockHandle (see class OMS_LockEntryHash)
*/

class OMS_LockEntry
{
public :
  typedef cgg251dclIterator<tsp00_TaskId, OMS_GlobalAllocatorWrapper> ReadLockListIterator;
  /*!-----------------------------------------------------------------------
  
  function : GetExclusiveLock

  description : requires an exclusive lock for the OMS_LockEntry and waits at least
                timeout seconds in case of lock collision
 
  parameters : 
    session[in] session which requires the lock
    isShareLocked[in] true, if the session already holds a share lock for this OMS_LockEntry
    timeout[in] wait at least timeout seconds in case of lock collision

  return value : none

  -------------------------------------------------------------------------*/
  
  void GetExclusiveLock(OMS_Session& session, bool isShareLocked, int timeout);
  
  /*!-----------------------------------------------------------------------
  
  function : GetShareLock

  description : requires a share lock for the OMS_LockEntry and waits at least
                timeout seconds in case of lock collision
 
  parameters : 
    requestor[in] pointer to OMS_SessionLockObject that corresponds with this OMS_LockEntry
    session[in] session which requires the lock
    timeout[in] wait at least timeout seconds in case of lock collision

  return value : none

  -------------------------------------------------------------------------*/

  void GetShareLock(OMS_SessionLockObjectPtr requestor, OMS_Session& session, int timeout);

  inline void GrantShareLock(OMS_SessionLockObjectPtr, tsp00_TaskId);

  inline bool IsExclusiveLocked() const;

 /*!-----------------------------------------------------------------------
  
  function : UnlockExclusiveLock

  description : removes the exclusive lock from the OMS_LockEntry. If there are any
                tasks waiting for this unlock, there lock requests are satisfied
                and they continue execution.
 
  parameters : 
    session[in] session which unlocks the lock. This must be the same session that
                required the lock via GetExclusiveLock

  return value : none

  -------------------------------------------------------------------------*/

  void UnlockExclusiveLock(OMS_Session& session);

  /*!-----------------------------------------------------------------------
  
  function : UnlockShareLock

  description : removes a share lock from the OMS_LockEntry. If there are any
                tasks waiting for this unlock, there lock requests are satisfied
                and they continue execution.
 
  parameters : 
    session[in] session which unlocks the lock. This must be the same session that
                required the lock via GetShareLock

  return value : none

  -------------------------------------------------------------------------*/

  void UnlockShareLock(ReadLockListIterator iter, OMS_Session& session);

  /*!-----------------------------------------------------------------------
  
  function : WakeUpWaitingTasks

  description : wakes up tasks waiting for an unlock operation.
 
  parameters : 
    session[in] session triggering the call

  return value : none

  -------------------------------------------------------------------------*/

  void WakeUpWaitingTasks(OMS_Session& session);
private :
  OMS_LockEntry*       m_hashNext;           // next lock entry in hash list
  RTESync_Spinlock m_lock;               // spinlock for exclusive access
  int              m_refCnt;             // number of sessions holding references 
//  ReadLockList   m_readLocks;
  typedef cgg251DCList<tsp00_TaskId, OMS_GlobalAllocatorWrapper> ReadLockList;
  
  ReadLockList   m_readLocks;          // list of tasks holding share locks
  tsp00_TaskId   m_exclusiveLockOwner; // task holding exclusive lock
  tsp00_Int2     m_cntReadLocks;       // number of read locks
  bool           m_exclusiveLocked;    // true, if exclusive locked; otherwise false
  bool           m_isSpinLocked;       // true, after a call of LockSpinLock
  OmsLockHandle  m_handle;             // OmsLockHandle identifying the lock
  RequestQueue   m_requestQueue;       // tasks waiting for unlock
  OMS_LockEntry(const OmsLockHandle&, OMS_LockEntry* next);
  inline int  DecRefCnt();
  inline void IncRefCnt();
  inline void InspectTimeout();
  inline void LockSpinLock();
  inline void UnlockSpinLock();
  inline void* operator new(size_t size);
  inline void operator delete(void* p);
  friend class OMS_LockEntryHash;
};

/*! endclass: OMS_LockEntry */

/*----------------------------------------------------------------------*/

class OMS_SessionLockObject
{
public :
  OMS_SessionLockObject(const OmsLockHandle& lh)
    : m_lockHandle(lh)
    , m_lockHashPtr(NULL)
    , m_readLockHandle(NULL,NULL)
    , m_refCnt(0)
    , m_shareLocked(false)
    , m_exclusiveLocked(false)
  {
  }
  void DecRefCnt() { --m_refCnt; }
  int  GetRefCnt() const { return m_refCnt; }
  void IncRefCnt() { ++m_refCnt; }
  bool IsExclusiveLocked() const { return m_exclusiveLocked; }
  bool IsLocked() const { return (this->IsShareLocked() || this->IsExclusiveLocked()); }
  bool IsShareLocked() const { return m_shareLocked; }
  void LockExclusive(OMS_Session& session, int timeout);
  void LockShare(OMS_Session& session, int timeout);
  void SetLockHashPtr(OMS_LockEntry* pLockHashPtr) { m_lockHashPtr = pLockHashPtr; }
  void SetReadLockHandle(OMS_LockEntry::ReadLockListIterator iter) { m_readLockHandle = iter; }
  void Unlock(OMS_Session& session);
  OmsLockHandle m_lockHandle;
private :
  OMS_LockEntry*                      m_lockHashPtr;
  OMS_LockEntry::ReadLockListIterator m_readLockHandle; 
  int                             m_refCnt;
  bool                            m_shareLocked;
  bool                            m_exclusiveLocked;
};

/*----------------------------------------------------------------------*/

class OMS_SessionLockObjects : public OMS_ISessionLockObjects
{
public :
  OMS_SessionLockObjects(OMS_Session& session);
  OMS_SessionLockObjectPtr Attach(OmsLockObject*, const OmsLockHandle&);
  virtual void Destroy();
  virtual void TransactionEnd();
          void Unregister(OmsLockObject*);         
private :
  void* operator new(size_t size, OMS_Session& session);
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete (void* p, OMS_Session& session) { };
#endif
  OMS_SessionLockObjectPtr FindLock(const OmsLockHandle&);
  cgg251DCList<OMS_SessionLockObject, OMS_Session> m_locks;
  cgg251DCList<OmsLockObject*, OMS_Session>    m_refOmsLockObjects;
  OMS_Session&                                 m_session;
friend class OMS_ISessionLockObjects;
};

/*----------------------------------------------------------------------*/
/*               globals                                                */
/*----------------------------------------------------------------------*/

static OMS_LockEntryHash omsLockEntryHashInstance;
OMS_LockEntryHash &OMS_LockEntryHash::m_instance = omsLockEntryHashInstance;

/*----------------------------------------------------------------------*/
/* impementation of class RequestQueue                                  */
/*----------------------------------------------------------------------*/

RequestQueue::RequestQueue()
: m_sentinel(0, 0, false, -1)/*----------------------------------------------------------------------*/

, m_last(&m_sentinel)
{
}

/*----------------------------------------------------------------------*/

void RequestQueue::Enter
(
 tsp00_TaskId         taskId,
 OMS_SessionLockObjectPtr requestor,
 bool                 exclusiveRequest,
 int                  timeout
)
{
  m_last->m_next = new QueueElement(taskId, requestor, exclusiveRequest, timeout);
  m_last = m_last->m_next;
}

/*----------------------------------------------------------------------*/

void RequestQueue::InspectTimeout(OMS_LockEntry& OMS_LockEntry)
{
  bool          timeout;
  bool          grantShareLocks;
  tsp00_Int4    currClock = omsGetClock();
  QueueElement* prev = &m_sentinel;
  QueueElement* curr = m_sentinel.m_next;
  grantShareLocks    = !OMS_LockEntry.IsExclusiveLocked();
  while (NULL != curr)
  {
    bool grantShareLock = grantShareLocks && !curr->m_exclusiveRequest;
    timeout             = (currClock > curr->m_requestClock);
    if (grantShareLock || timeout)
    {
      // wake up or timed out
      if (grantShareLock)
      {
        OMS_LockEntry.GrantShareLock(curr->m_requestor, curr->m_taskId);
        timeout = false;
      }
      omsSignal(curr->m_taskId, timeout);
      prev->m_next = curr->m_next;
      if (curr == m_last)
      {
        m_last = prev;
      }
      delete curr;
      curr = prev->m_next;
    }
    else
    {
      if (curr->m_exclusiveRequest)
      {
        grantShareLocks = false;
      }
      prev = curr;
      curr = curr->m_next;
    }
  }
}

/*----------------------------------------------------------------------*/

inline bool RequestQueue::IsEmpty() const
{
  return (NULL == m_sentinel.m_next);
}

/*----------------------------------------------------------------------*/

inline bool RequestQueue::RemoveFirst
(
 bool                      allowExclusiveLock,
 tsp00_TaskId&             taskId,
 OMS_SessionLockObjectPtr& requestor,
 bool&                     isExclusiveRequest
 )
{
  QueueElement* first = m_sentinel.m_next;
  if (NULL != first)
  {
    if ((first->m_exclusiveRequest) && (!allowExclusiveLock))
    {
      return false;
    }
    taskId             = first->m_taskId;
    requestor          = first->m_requestor;
    isExclusiveRequest = first->m_exclusiveRequest;
    m_sentinel.m_next  = first->m_next;
    if (m_last == first)
    {
      m_last = &m_sentinel;
    }
    delete first;
    return true;
  }
  return false;
}

/*----------------------------------------------------------------------*/

OMS_LockEntry::OMS_LockEntry
(
 const OmsLockHandle& handle,
 OMS_LockEntry*           next
 )
: m_hashNext(next)
, m_lock()
, m_refCnt(0)
, m_readLocks(&OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper)
, m_exclusiveLockOwner(0)
, m_exclusiveLocked(false)
, m_cntReadLocks(0)
, m_isSpinLocked(false)
, m_handle(handle)
, m_requestQueue()
{
}

/*----------------------------------------------------------------------*/

inline int OMS_LockEntry::DecRefCnt()
{
  --m_refCnt;
  return m_refCnt;
}

/*----------------------------------------------------------------------*/

void OMS_LockEntry::GetExclusiveLock(OMS_Session& session, bool isShareLocked, int timeout)
{
  LockedScope sync(m_lock);
  if (!m_exclusiveLocked)
  {
    if (0 == m_cntReadLocks - isShareLocked)//(m_readLocks.empty() || (isShareLocked && (1 == m_cntReadLocks)))
    {
      m_exclusiveLocked    = true;
      m_exclusiveLockOwner = session.GetTaskId();
      return;
    }
  }
  m_requestQueue.Enter(session.GetTaskId(), 0, true, timeout);
  sync.Leave();
  session.Wait();
}

/*----------------------------------------------------------------------*/

void OMS_LockEntry::GetShareLock(OMS_SessionLockObjectPtr requestor, OMS_Session& session, int timeout)
{
  LockedScope sync(m_lock);
  if (m_exclusiveLocked || (!m_requestQueue.IsEmpty()))
  {
    m_requestQueue.Enter(session.GetTaskId(), requestor, false, timeout);
    sync.Leave();
    session.Wait();
  }
  else
  {
    this->GrantShareLock(requestor, session.GetTaskId());
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_LockEntry::IncRefCnt()
{
  ++m_refCnt;
}

/*----------------------------------------------------------------------*/

inline void OMS_LockEntry::GrantShareLock(OMS_SessionLockObjectPtr requestor, tsp00_TaskId taskId)
{
  m_readLocks.push_front(taskId);
  ++m_cntReadLocks;
  requestor->SetReadLockHandle(m_readLocks.begin());
}

/*----------------------------------------------------------------------*/

inline void OMS_LockEntry::InspectTimeout()
{
  LockedScope sync(m_lock);
  m_requestQueue.InspectTimeout(*this);
}

/*----------------------------------------------------------------------*/

inline bool OMS_LockEntry::IsExclusiveLocked() const
{
  return m_exclusiveLocked;
}

/*----------------------------------------------------------------------*/

inline void OMS_LockEntry::LockSpinLock()
{
  m_lock.Lock();
  m_isSpinLocked = true;
}

/*----------------------------------------------------------------------*/

void OMS_LockEntry::UnlockExclusiveLock(OMS_Session& session)
{
  LockedScope sync(m_lock);
  if (m_exclusiveLocked)
  {
    m_exclusiveLocked = false;
    this->WakeUpWaitingTasks(session);
  }
}

/*----------------------------------------------------------------------*/

void OMS_LockEntry::UnlockShareLock(OMS_LockEntry::ReadLockListIterator iter, OMS_Session& session)
{
  LockedScope sync(m_lock);
  m_readLocks.remove(iter);
  --m_cntReadLocks;
  if (m_readLocks.empty())
  {
    this->WakeUpWaitingTasks(session);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_LockEntry::UnlockSpinLock()
{
  if (m_isSpinLocked)
  {
    m_lock.Unlock();
    m_isSpinLocked = false;
  }
}

/*----------------------------------------------------------------------*/

void OMS_LockEntry::WakeUpWaitingTasks(OMS_Session& session)
{
  if (!m_requestQueue.IsEmpty())
  {
    OMS_SessionLockObjectPtr requestor;
    tsp00_TaskId         taskId;
    bool                 allowExclusiveLock = true;
    while (m_requestQueue.RemoveFirst( 
      allowExclusiveLock, taskId, requestor, m_exclusiveLocked))
    {
      if (m_exclusiveLocked)
      {
        m_exclusiveLockOwner = taskId;
        session.Signal(taskId, true);
        break;
      }
      else
      {
        m_readLocks.push_front(taskId);
        ++m_cntReadLocks;
        requestor->SetReadLockHandle(m_readLocks.begin());
        session.Signal(taskId, true);
        allowExclusiveLock = false;
      }
    }
  }
}

/*----------------------------------------------------------------------*/

inline void* OMS_LockEntry::operator new(size_t size)
{
  return OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(size);
}

/*----------------------------------------------------------------------*/

inline void OMS_LockEntry::operator delete(void* p)
{
  OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(p);
}

/*----------------------------------------------------------------------*/

OMS_LockEntryHash::OMS_LockEntryHash()
{
}

/*----------------------------------------------------------------------*/

inline OMS_LockEntryHash::HashSlotLockedScope OMS_LockEntryHash::GetHeadPtr(const OmsLockHandle& handle)
{
  int hashValue = 0;
  for (int ix = 0; ix < sizeof(handle.m_handle); ++ix)
  {
    hashValue = 13 * hashValue + handle.m_handle[ix];
  }
  if (hashValue < 0)
  {
    hashValue = -hashValue;
  }
  int hashSlot = hashValue % CNT_HEAD_ENTRIES;
  return HashSlotLockedScope (m_head[hashSlot]);
}

/*----------------------------------------------------------------------*/

OMS_LockEntryHash::LockEntryPtr OMS_LockEntryHash::Insert(const OmsLockHandle& handle)
{
  HashSlotLockedScope slotScope = this->GetHeadPtr(handle);
  LockEntryPtr found = slotScope.m_hashSlot.m_list;
  while (0 != found) {
    if (0 == memcmp(&found->m_handle, &handle, sizeof(handle)))
			break;
		else
			found = found->m_hashNext;
  }
  if (!found)
  {
    found = new OMS_LockEntry(handle, slotScope.m_hashSlot.m_list);
    slotScope.m_hashSlot.m_list = found; 
  }
  found->IncRefCnt();
  return found;
}

/*----------------------------------------------------------------------*/

void OMS_LockEntryHash::InspectTimeout()
{
  for (int slot = 0; slot < CNT_HEAD_ENTRIES; ++slot)
  {
    HashSlotLockedScope slotScope(m_head[slot]);
    LockEntryPtr curr = slotScope.m_hashSlot.m_list;
    while (NULL != curr)
    {
      curr->InspectTimeout();
      curr = curr->m_hashNext;
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_LockEntryHash::BlockLockEntryHash()
{
  for (int slot = 0; slot < CNT_HEAD_ENTRIES; ++slot)
  {
    m_head[slot].Lock();
    LockEntryPtr curr = m_head[slot].m_list;
    while (NULL != curr)
    {
      curr->LockSpinLock();
      curr = curr->m_hashNext;
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_LockEntryHash::UnBlockLockEntryHash()
{
  for (int slot = 0; slot < CNT_HEAD_ENTRIES; ++slot)
  {
    LockEntryPtr curr = m_head[slot].m_list;
    while (NULL != curr)
    {
      curr->UnlockSpinLock();
      curr = curr->m_hashNext;
    }
    m_head[slot].Unlock();
  }
}

/*----------------------------------------------------------------------*/

bool OMS_LockEntryHash::NextLockInfo(void** pVoidLockInfo, tgg01_OmsLockInfo& lockInfo)
{
  struct LockInfoRow
  {
    enum LockKind { LOCK_SHARE, LOCK_EXCLUSIVE, REQ_SHARE, REQ_EXCLUSIVE };

    OmsLockHandle m_lockHandle;
    tsp00_TaskId  m_taskId;
    LockKind      m_lockKind;
    int           m_timeout;
    LockInfoRow*  m_next;
    LockInfoRow(const OmsLockHandle& h, tsp00_TaskId taskId, LockKind lockKind, int timeout, LockInfoRow* next)
      : m_lockHandle(h)
      , m_taskId(taskId)
      , m_lockKind(lockKind)
      , m_timeout(timeout)
      , m_next(next)
    {
    }
    void* operator new(size_t size) { return OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(size); }
    void  operator delete (void* p) { OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(p); }
  };

  LockInfoRow* pLockInfo = REINTERPRET_CAST(LockInfoRow*, *pVoidLockInfo);
  if (NULL == pLockInfo)
  {
    this->BlockLockEntryHash();
    try
    {
      for (int slot = 0; slot < CNT_HEAD_ENTRIES; ++slot)
      {
        LockEntryPtr curr = m_head[slot].m_list;
        while (NULL != curr)
        {
           if (curr->m_exclusiveLocked)
           {
              pLockInfo = new LockInfoRow(curr->m_handle, curr->m_exclusiveLockOwner,
                LockInfoRow::LOCK_EXCLUSIVE, -1, pLockInfo);
           }
           // describe task having read locks
           for (OMS_LockEntry::ReadLockListIterator iter = curr->m_readLocks.begin(); iter; ++iter)
           {
             pLockInfo = new LockInfoRow(curr->m_handle, *iter(),
                LockInfoRow::LOCK_SHARE, -1, pLockInfo);
           }
           // describe tasks waiting for locks
           RequestQueueIterator requestIter(curr->m_requestQueue);
           while (requestIter)
           {
             int timeout = requestIter.GetTimeoutClock() - omsGetClock();
             if (timeout < 0)
             {
               timeout = 1;
             }
             pLockInfo = new LockInfoRow(curr->m_handle, requestIter.GetTaskId(),
               (requestIter.IsExclusiveRequest()) ? LockInfoRow::REQ_EXCLUSIVE : LockInfoRow::REQ_SHARE,
               timeout, pLockInfo);
             ++requestIter;
           }
           curr = curr->m_hashNext;
        }
      }
    }
    catch(...)
    {
      this->UnBlockLockEntryHash();
      while (NULL != pLockInfo)
      {
        LockInfoRow* pDel = pLockInfo;
        pLockInfo = pLockInfo->m_next;
        delete pDel;
      }
      throw;
    }
    this->UnBlockLockEntryHash();
  }
  if (NULL != pLockInfo)
  {
    *pVoidLockInfo = pLockInfo->m_next;
    SAPDB_MemCopyNoCheck(&lockInfo.oli_handle, &pLockInfo->m_lockHandle, sizeof(lockInfo.oli_handle));
    switch (pLockInfo->m_lockKind)
    {
    case LockInfoRow::LOCK_SHARE : 
      SAPDB_MemCopyNoCheck(&lockInfo.oli_lockmode, "LOCK SHARE        ", sizeof(lockInfo.oli_lockmode));
      memset(&lockInfo.oli_requestmode, ' ', sizeof(lockInfo.oli_requestmode));
      break;
    case LockInfoRow::LOCK_EXCLUSIVE : 
      SAPDB_MemCopyNoCheck(&lockInfo.oli_lockmode, "LOCK EXCLUSIVE    ", sizeof(lockInfo.oli_lockmode));
      memset(&lockInfo.oli_requestmode, ' ', sizeof(lockInfo.oli_requestmode));
      break;
    case LockInfoRow::REQ_EXCLUSIVE :
      SAPDB_MemCopyNoCheck(&lockInfo.oli_requestmode, "REQUEST EXCLUSIVE ", sizeof(lockInfo.oli_requestmode));
      memset(&lockInfo.oli_lockmode, ' ', sizeof(lockInfo.oli_lockmode));
      break;
    case LockInfoRow::REQ_SHARE :
      SAPDB_MemCopyNoCheck(&lockInfo.oli_requestmode, "REQUEST SHARE     ", sizeof(lockInfo.oli_requestmode));
      memset(&lockInfo.oli_lockmode, ' ', sizeof(lockInfo.oli_lockmode));
      break;
    }
		lockInfo.oli_taskid  = pLockInfo->m_taskId;
		lockInfo.oli_timeout = pLockInfo->m_timeout;
    delete pLockInfo;
    return true;
  }
  else
  {
    return false;
  }
}

/*----------------------------------------------------------------------*/

void OMS_LockEntryHash::Release(void* pLockEntry)
{
  LockEntryPtr pReleaseLockEntry = REINTERPRET_CAST(LockEntryPtr, pLockEntry);
  HashSlotLockedScope slotScope  = this->GetHeadPtr(pReleaseLockEntry->m_handle);
  if (0 == pReleaseLockEntry->DecRefCnt())
  {
    LockEntryPtr* prev = &slotScope.m_hashSlot.m_list;
    LockEntryPtr curr = *prev;
    while (curr) {
      if (curr == pReleaseLockEntry)
      {
        *prev = curr->m_hashNext;
        delete curr;
        return;
      }
      else {
        prev = &curr->m_hashNext;
        curr = curr->m_hashNext;
      }
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_SessionLockObject::LockExclusive(OMS_Session& session, int timeout)
{
  if (!m_exclusiveLocked)
  {
    m_lockHashPtr->GetExclusiveLock(session, m_shareLocked, timeout);
    m_exclusiveLocked = true; 
  }
}

/*----------------------------------------------------------------------*/
 
void OMS_SessionLockObject::LockShare(OMS_Session& session, int timeout)
{
  if ((!m_shareLocked) && (!m_exclusiveLocked))
  {
    m_lockHashPtr->GetShareLock(this, session, timeout);
    m_shareLocked = true; 
  }
}

/*----------------------------------------------------------------------*/

void OMS_SessionLockObject::Unlock(OMS_Session& session)
{
  if (m_shareLocked)
  {
    m_lockHashPtr->UnlockShareLock(m_readLockHandle, session);
    m_shareLocked = false;
  }
  if (m_exclusiveLocked)
  {
    m_lockHashPtr->UnlockExclusiveLock(session);
    m_exclusiveLocked = false;
  }
}

/*----------------------------------------------------------------------*/

OMS_SessionLockObjects::OMS_SessionLockObjects(OMS_Session& session)
: m_session(session)
, m_locks(&session)
, m_refOmsLockObjects(&session)
{
}

/*----------------------------------------------------------------------*/

void* OMS_SessionLockObjects::operator new(size_t size, OMS_Session& session)
{
  return session.allocate(size);
}

/*----------------------------------------------------------------------*/

OMS_SessionLockObjectPtr OMS_SessionLockObjects::Attach(OmsLockObject* pLockObject, const OmsLockHandle& lh)
{
  OMS_TRACE(omsTrLock, m_session.m_lcSink, "Attach " 
    << OMS_UnsignedCharBuffer(REINTERPRET_CAST(const unsigned char*, &lh), sizeof(lh))
    << " to session " << &m_session);
  for (cgg251dclIterator<OmsLockObject*,OMS_Session> iter = m_refOmsLockObjects.begin();
  iter; ++iter)
  {
    if (*iter() == pLockObject)
    {
      // OmsLockObject already registered, allow only same OmsLockHandle
      if (this->FindLock(lh) != pLockObject->m_pLock)
      {
        pLockObject->m_omsHandle->m_pSession->ThrowDBError(e_OMS_invalid, "duplicate omsEnable", __MY_FILE__, __LINE__);
      }
      return NULL;
    }
  }
  OMS_TRACE(omsTrLock, m_session.m_lcSink, "Register " << pLockObject);
  m_refOmsLockObjects.push_front(pLockObject);
  OMS_SessionLockObjectPtr pLock = this->FindLock(lh);
  if (NULL == pLock)
  {
    OMS_SessionLockObject lockObj(lh); 
    m_locks.push_front(lockObj);
    pLock = &m_locks.front();
    OMS_TRACE(omsTrLock, m_session.m_lcSink, "Created Session Lock Obj " << pLock);
    pLock->SetLockHashPtr(OMS_LockEntryHash::m_instance.Insert(lh));
  }
  pLock->IncRefCnt();
  return pLock;
}

/*----------------------------------------------------------------------*/

OMS_ISessionLockObjects* OMS_ISessionLockObjects::Create(OMS_Session& session)
{
  return new(session) OMS_SessionLockObjects(session);
}

/*----------------------------------------------------------------------*/

void OMS_SessionLockObjects::Destroy()
{
  OMS_TRACE(omsTrLock, m_session.m_lcSink, "OMS_SessionLockObjects::Destroy");
  this->TransactionEnd();
  for (cgg251dclIterator<OmsLockObject*,OMS_Session> iter = m_refOmsLockObjects.begin();
  iter; ++iter)
  {
    OMS_TRACE(omsTrLock, m_session.m_lcSink, "Invalidate " << (*iter()));
    (*iter())->Invalidate();
  }
  m_session.deallocate(this);
}

/*----------------------------------------------------------------------*/

OMS_SessionLockObjectPtr OMS_SessionLockObjects::FindLock(const OmsLockHandle& lockHandle)
{
  for (cgg251dclIterator<OMS_SessionLockObject,OMS_Session> iter = m_locks.begin();
    iter; ++iter)
  {
      if (0 == memcmp(&lockHandle, &iter()->m_lockHandle, sizeof(lockHandle)))
      {
        return iter();
      }
  }
  return NULL;
}

/*----------------------------------------------------------------------*/

void OMS_SessionLockObjects::TransactionEnd()
{
  cgg251dclIterator<OMS_SessionLockObject,OMS_Session> iter = m_locks.begin();
  while (iter)
  {
    cgg251dclIterator<OMS_SessionLockObject,OMS_Session> curr = iter;
    ++iter;
    curr()->Unlock(m_session);
    if (0 == curr()->GetRefCnt())
    {
      m_locks.remove(curr);
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_SessionLockObjects::Unregister(OmsLockObject* pOmsLockObject)
{
  for (cgg251dclIterator<OmsLockObject*,OMS_Session> iter = m_refOmsLockObjects.begin();
  iter; ++iter)
  {
    if (*iter() == pOmsLockObject)
    {
      OMS_TRACE(omsTrLock, m_session.m_lcSink, "Unregister " << pOmsLockObject);
      m_refOmsLockObjects.remove(iter);
      return;
    }
  }
}

/*----------------------------------------------------------------------*/

OmsLockObject::OmsLockObject(OmsHandle& h)
: m_omsHandle(&h)
, m_pLock(NULL) 
{
 
}

/*----------------------------------------------------------------------*/

OmsLockObject::~OmsLockObject()
{
  if (NULL != m_pLock)
  {
    m_pLock->DecRefCnt();
    OMS_SessionLockObjects* pSessionLocks = 
      REINTERPRET_CAST(OMS_SessionLockObjects*, m_omsHandle->m_pSession->GetSessionLockObject());
    pSessionLocks->Unregister(this);
  }
}

/*----------------------------------------------------------------------*/

void OmsLockObject::omsEnable ()
{
  if (NULL != m_omsHandle) 
  {
    tsp00_Int2  sqlCode;
    tsp00_Int4  BufLen = sizeof(m_lockHandle);
    OMS_HResult hr = (*m_omsHandle->m_session)->MultiPurpose (m_get, mm_key, &BufLen, 
      (unsigned char*) &m_lockHandle, &sqlCode);
    if (0 != sqlCode) {
      OMS_Globals::Throw(sqlCode, "OmsLockObject::omsEnable", __MY_FILE__, __LINE__);
    }
    this->omsEnable(m_lockHandle);
  }
}

/*----------------------------------------------------------------------*/

void OmsLockObject::omsEnable (const OmsLockHandle& lh) {
  OMS_SessionLockObjects* pSessionLocks = 
    REINTERPRET_CAST(OMS_SessionLockObjects*, m_omsHandle->m_pSession->GetSessionLockObject());
  m_pLock      = pSessionLocks->Attach(this, lh);
  m_lockHandle = lh;
}

/*----------------------------------------------------------------------*/

const OmsLockHandle& OmsLockObject::omsGetLockHandle() const
{
  return m_lockHandle;
}

/*----------------------------------------------------------------------*/

void OmsLockObject::omsLockShare() 
{
  if (NULL != m_pLock)
  {
    m_pLock->LockShare(*m_omsHandle->m_pSession, OMS_LockEntryHash::m_instance.GetTimeout());
  }
}

/*----------------------------------------------------------------------*/

void OmsLockObject::omsLockExclusive() 
{
  if (NULL != m_pLock)
  {
    m_pLock->LockExclusive(*m_omsHandle->m_pSession, OMS_LockEntryHash::m_instance.GetTimeout());
  }
}

/*----------------------------------------------------------------------*/

void OmsLockObject::omsUnlock()
{
  if (NULL != m_pLock)
  {
    m_pLock->Unlock(*m_omsHandle->m_pSession);
  }
}

/*----------------------------------------------------------------------*/

bool  OmsLockObject::omsIsLocked() const 
{
  if (NULL != m_pLock)
  {
    return m_pLock->IsLocked();
  }
  return false;
}

/*----------------------------------------------------------------------*/

bool  OmsLockObject::omsShareLocked() const 
{
  if (NULL != m_pLock)
  {
    return m_pLock->IsShareLocked();
  }
  return false;
}

/*----------------------------------------------------------------------*/

bool OmsLockObject::omsExclusiveLocked() const 
{
  if (NULL != m_pLock)
  {
    return m_pLock->IsExclusiveLocked();
  }
  return false;
}

/*----------------------------------------------------------------------*/

void OmsLockObject::Invalidate() {
  m_omsHandle = NULL;
  m_pLock     = NULL;
}

/*----------------------------------------------------------------------*/
/*                      global functions                                */
/*----------------------------------------------------------------------*/

#if 0
remove the following:

void OMS_LockEntryHash::m_instance.InspectTimeout()
{
  OMS_LockEntryHash::m_instance.InspectTimeout();
}

/*----------------------------------------------------------------------*/

bool OMS_LockEntryHash::m_instance.NextLockInfo(void** pVoidLockInfo, tgg01_OmsLockInfo& lockInfo)
{
  return OMS_LockEntryHash::m_instance.NextLockInfo (pVoidLockInfo, lockInfo);
}

/*----------------------------------------------------------------------*/

void OMS_LockEntryHash::m_instance.SetTimeout(int timeout)
{
  OMS_LockEntryHash::m_instance.SetTimeout(timeout);
}
#endif