/*!
 * \file    OMS_RWLock.hpp
 * \author  ChristophR
 * \brief   Reader-writer lock.
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

#ifndef __OMS_RWLOCK_HPP
#define __OMS_RWLOCK_HPP

#include "Oms/OMS_AFX.h"
#include "livecachetypes.h" // nocheck
#include "ggg00.h"
#include "ggg251.h"
#include <memory.h>

class OmsHandle;
class OMS_Session;

/*!
 * \brief Session-specific reader-writer lock handle.
 */
class OMS_RWLock {
public:

  /*!
   * \brief Enter the protected region.
   *
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   *
   * \return error code.
   */
  short enter(bool exclusive);

  /*!
   * \brief Try to enter the protected region.
   *
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   * \return \c true, if successfully entered the region, \c false, if it would have to wait.
   *
   * \return error code.
   */
  short tryEnter(bool exclusive);

  /*!
   * \brief Leave the region.
   *
   * \return error code.
   */
  short leave();

  /*!
   * \brief Get area ID.
   *
   * \return area ID.
   */
  inline OmsTypeInt4 getAreaId()
  {
    return m_areaId;
  }

  /*!
   * \brief Get lock ID.
   *
   * \return lock ID.
   */
  inline OmsTypeInt4 getLockId()
  {
    return m_lockId;
  }


  /// Return true, if the lock is locked.
  inline bool getLocked()
  {
    return m_isLocked;
  }

  /// Return true, if the lock is exclusively locked.
  inline bool getExclusive()
  {
    return m_isExclusive;
  }

  OMS_RWLock *m_pHashNext;

private:
  friend class OMS_RWLockDirectory;
  /*!
   * \brief Create new reader-writer lock.
   *
   * \param h OMS handle,
   * \param areaId area ID (user-defined) (MININT < areaId < MAXINT).
   * \param lockId lock ID (user-defined) (0 < lockId < MAXINT).
   */
  OMS_RWLock( OmsTypeInt4 areaId, OmsTypeInt4 lockId, tsp00_TaskId taskId);

  /*!
   * \brief new operator to create a OMS_RWLock in the session.
   *
   * \param sz OMS_RWLock size,
   * \param pSession OMS Session pointer
   */
  void* operator new(size_t sz, OMS_Session *pSession);

  /*!
   * \brief delete operator to free a OMS_RWLock in the session.
   *
   * \param ptr pointer to OMS_RWLock,
   * \param pSession OMS Session pointer
   */
  void operator delete(void *ptr, OMS_Session *pSession);

  //OmsHandle &m_handle;    ///< Underlying OMS handle
  OmsTypeInt4 m_areaId;     ///< Area ID
  OmsTypeInt4 m_lockId;     ///< Lock ID
  bool      m_isLocked;     ///< Is currently locked?
  bool      m_isExclusive;  ///< Is exclusively locked?
  bool      m_isCreated;    ///< Is RWLock created?
  tsp00_TaskId m_taskId;    ///< Session task id
  void     *m_lockAddress;

};


/*!
 * \brief Lock scope for internal use.
 *
 * Use this class to protect a critical section with shared or exclusive access.
 */
class OMS_InternalLockScope{
public:
  /*!
   * \brief Lock the specified lock in constructor.
   *
   * \param h pSession Pointer to OMS session,
   * \param areaId area ID (user-defined) (0 < areaId < MAXINT).
   * \param lockId lock ID (user-defined) (0 < lockId < MAXINT).
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  OMS_InternalLockScope(OMS_Session *pSession, OmsTypeInt4 areaId, OmsTypeInt4 lockId, bool exclusive);

  /*!
   * \brief Unlock in destructor.
   */
  ~OMS_InternalLockScope();

private:
  OMS_RWLock *m_lock;

  // private copy constructor, assignment operator and new operator
  // this methods are not allowed for a scope
  OMS_InternalLockScope(const OMS_InternalLockScope& scope);
  void operator =(const OMS_InternalLockScope& scope);
  void *operator new(size_t size);
};

/*!
 * \brief Lock scope for internal use without OMS session access.
 * 
 * Use this class to protect a critical section with shared or exclusive access if the OMS session is not accessible. 
 * Every request is directly send to the kernel. This access is more expensive the the OMS_InternalLockScope because 
 * the session lock directory, which buffers the lock pointer, is not used.
 *
 */
class OMS_InternalKernelLockScope{
public:
  /*!
   * \brief Lock the specified lock in constructor.
   *
   * \param areaId area ID (user-defined) (0 < areaId < MAXINT).
   * \param lockId lock ID (user-defined) (0 < lockId < MAXINT).
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  OMS_InternalKernelLockScope( OmsTypeInt4 areaId, OmsTypeInt4 lockId, bool exclusive);

  /*!
   * \brief Unlock in destructor.
   */
  ~OMS_InternalKernelLockScope();

private:
  void  *m_lockAddress;
  bool  m_exclusive;
  OmsTypeInt4  m_areaId; 
  OmsTypeInt4  m_lockId; 

  // private copy constructor, assignment operator and new operator
  // this methods are not allowed for a scope
  OMS_InternalKernelLockScope(const OMS_InternalKernelLockScope& scope);
  void operator =(const OMS_InternalKernelLockScope& scope);
  void *operator new(size_t size);
};

/*!
 * \brief Multi lock scope for internal use.
 *
 * Use this class to protect a critical section with shared or exclusive access. You can lock
 * multiple locks at once.
 */

class OMS_InternalMultiScope{
public:
  /*!
   * \brief Create new multi lock scope.
   *
   * \param pSession Pointer to OMS session,
   * \param areaId area ID (user-defined) (0 < areaId < MAXINT).
   * \param lockId array of ascending ordered lock IDs (user-defined) (0 < lockId < MAXINT).
   * \param lockCnt size of lock ID array
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  OMS_InternalMultiScope(OMS_Session *pSession, OmsTypeInt4 areaId, const OmsTypeInt4 *lockId, OmsTypeInt4 lockCnt, bool exclusive);
  /*!
   * \brief Leave the scope, unlock all locks.
   */
  ~OMS_InternalMultiScope();

private:
  /*!
   * \brief Unlock all allready locked RWlocks (internal use only).
   */
  void unlock(OmsTypeInt4 lockCnt);

  OMS_Session *m_pSession;
  OMS_RWLock **m_locks;
  OmsTypeInt4 m_lockCnt;
  OmsTypeInt4 m_areaId;

  // private copy constructor, assignment operator and new operator
  // this methods are not allowed for a scope
  OMS_InternalMultiScope(const OMS_InternalMultiScope& scope);
  void operator =(const OMS_InternalMultiScope& scope);
  void *operator new(size_t size);
};


/*!
 * \brief RWLock Directory.
 *
 * Directory for the RWLocks in the session
 */

class OMS_RWLockDirectory {
public:
  enum {HEAD_ENTRIES=501};
  OMS_RWLockDirectory();
  ~OMS_RWLockDirectory();
  OMS_RWLock &getLock(int areaId, int lockId);
  OMS_RWLock * find(int areaId, int lockId);
  OMS_RWLock * create(int areaId, int lockId);
  void setSession(OMS_Session *pSession);
  void cleanup ();
  int hash(int areaId, int lockId);
  void transactionEnd();
 
private:
  OMS_Session *m_pSession;
  OMS_RWLock  *m_head[HEAD_ENTRIES];
  cgg251DCList<OMS_RWLock, OMS_Session>  m_transactionLockList;
  int m_lockCnt;
};

inline OMS_RWLock &OMS_RWLockDirectory::getLock(int areaId, int lockId)
{
  OMS_RWLock *p = find(areaId, lockId);
  if(!p){
    p = create(areaId,lockId);
  }
  return *p;
}

inline OMS_RWLock * OMS_RWLockDirectory::find(int areaId, int lockId)
{
  int slot = hash(areaId,lockId);
  OMS_RWLock *p = m_head[slot];
  while (NULL != p) {
    if (p->getAreaId() == areaId && p->getLockId() == lockId) {
      return p;
    }
    p = p->m_pHashNext;
  }
  return NULL;
}

inline void OMS_RWLockDirectory::setSession(OMS_Session *pSession)
{
  m_pSession = pSession;
  m_transactionLockList.advise_allocator(pSession);
}

inline int OMS_RWLockDirectory::hash(int areaId, int lockId)
{
  return (int)(((((OmsTypeUInt8)areaId)<< 32) | ((OmsTypeUInt8)lockId)) % HEAD_ENTRIES);
}


#endif  // __OMS_RWLOCK_HPP
