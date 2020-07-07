/*!
 * \file    OMS_LockScopes.hpp
 * \author  ChristophR
 * \brief   shared/exclusive lock scopes for lcapps.
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

#ifndef __OMS_LOCKSCOPES_HPP
#define __OMS_LOCKSCOPES_HPP

#include "Oms/OMS_AFX.h"
#include "livecachetypes.h" // nocheck

#include <stddef.h>  // size_t

class OmsHandle;
class OMS_RWLock;

/*!
 * \brief Lock scope for lcapps.
 *
 * Use this class to protect a critical section with shared or exclusive access.
 */
class AFX_EXT_CLASS OmsLockScope{
public:
  /*!
   * \brief Lock the specified lock shared in constructor.
   *
   * \param h OMS handle,
   * \param areaId area ID (user-defined) (0 < areaId < MAXINT).
   * \param lockId lock ID (user-defined) (0 < lockId < MAXINT).
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  OmsLockScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId, bool exclusive);

  /*!
   * \brief Unlock in destructor.
   */
  ~OmsLockScope();

private:
  OMS_RWLock *m_lock;

  // private copy constructor, assignment operator and new operator
  // this methods are not allowed for a scope
  OmsLockScope(const OmsLockScope& scope);
  void operator =(const OmsLockScope& scope);
  void *operator new(size_t size);
};

/*!
 * \brief Releasable lock scope for lcapps.
 *
 * Use this class to protect a critical section with shared or exclusive access. It is possible
 * to leave or reenter the lock scope.
 */
class AFX_EXT_CLASS OmsReleasableScope{
public:
  /*!
   * \brief Create new reader-writer lock scope.
   *
   * \param h OMS handle,
   * \param areaId area ID (user-defined) (0 < areaId < MAXINT).
   * \param lockId lock ID (user-defined) (0 < lockId < MAXINT).
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  OmsReleasableScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId, bool exclusive);
  
  /*!
   * \brief If locked, unlock in destructor.
   */ 
  ~OmsReleasableScope();
  
  /*!
   * \brief Reenter the protected region.
   *
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  void reenter(bool exclusive);

  /*!
   * \brief Leave the region.
   */
  void leave();

private:
  OMS_RWLock *m_lock;
 
  // private copy constructor, assignment operator and new operator
  // this methods are not allowed for a scope
  OmsReleasableScope(const OmsReleasableScope& scope);
  void operator =(const OmsReleasableScope& scope);
  void *operator new(size_t size);
};

/*!
 * \brief Multi lock scope for lcapps.
 *
 * Use this class to protect a critical section with shared or exclusive access. You can lock
 * multiple locks at once.
 */

class AFX_EXT_CLASS OmsMultiScope{
public:
  /*!
   * \brief Create new multi lock scope.
   *
   * \param h OMS handle,
   * \param areaId area ID (user-defined) (0 < areaId < MAXINT).
   * \param lockId array of ascending ordered lock IDs (user-defined) (0 < lockId < MAXINT).
   * \param lockCnt size of lock ID array
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  OmsMultiScope(OmsHandle &h, OmsTypeInt4 areaId, const OmsTypeInt4 *lockId, OmsTypeInt4 lockCnt, bool exclusive);
  /*!
   * \brief Leave the scope, unlock all locks.
   */
  ~OmsMultiScope();

private:
  /*!
   * \brief Unlock all allready locked RWlocks (internal use only).
   */
  void unlock(OmsTypeInt4 lockCnt);

  OmsHandle *m_handle;
  OMS_RWLock **m_locks;
  OmsTypeInt4 m_lockCnt;
  OmsTypeInt4 m_areaId;

  // private copy constructor, assignment operator and new operator
  // this methods are not allowed for a scope
  OmsMultiScope(const OmsMultiScope& scope);
  void operator =(const OmsMultiScope& scope);
  void *operator new(size_t size);
};

/*---- OLD INTERFACES - NO LONGER SUPPORTED------------*/
/*-----------------------------------------------------*/

/*!
 * \brief OmsRWLock for lcapps.
 *
 * Use this class to protect a critical section with shared or exclusive access. 
 * The Lock is at least unlocked at transaction end.
 */
class AFX_EXT_CLASS OmsRWLock{
public:
  /*!
   * \brief Create new reader-writer lock.
   *
   * \param h OMS handle,
   * \param areaId area ID (user-defined) (MININT < areaId < MAXINT).
   * \param lockId lock ID (user-defined) (0 < lockId < MAXINT).
   */
  OmsRWLock(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId);

  /*!
   * \brief If locked, unlock in destructor.
   */ 
  ~OmsRWLock();

  /*!
   * \brief Enter the protected region.
   *
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   */
  void enter(bool exclusive);

  /*!
   * \brief Try to enter the protected region.
   *
   * \param exclusive if \c true, enter as writer, if \c false, as reader
   * \return \c true, if successfully entered the region, \c false, if it would have to wait.
   */
  bool tryEnter(bool exclusive);

  /*!
   * \brief Leave the region.
   */
  void leave();

  /*!
   * \brief Return true, if the lock is locked.
   */ 
  inline bool getLocked();

  /*!
   * \brief Return true, if the lock is exclusively locked.
   */ 
  inline bool getExclusive();

private:
  OMS_RWLock *m_lock;
};


/*!
 * \brief Exclusive lock scope for lcapps.
 *
 * Use this class to protect a critical section with exclusive write access.
 */
class AFX_EXT_CLASS OmsExclusiveScope{
public:
  /// Lock the specified lock exclusively in constructor.
  OmsExclusiveScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId);

  /// Unlock the lock in destructor.
  ~OmsExclusiveScope();

private:
  OMS_RWLock *m_lock;
};

/*!
 * \brief Shared lock scope for lcapps.
 *
 * Use this class to protect a critical section with shared, read-only access.
 */
class AFX_EXT_CLASS OmsSharedScope{
public:
  /// Lock the specified lock shared in constructor.
  OmsSharedScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId);

  /// Unlock the lock in destructor.
  ~OmsSharedScope();

private:
  OMS_RWLock *m_lock;
};

/*-----------------------------------------------------*/
/*-----------------------------------------------------*/

#endif  // __OMS_LOCKSCOPES_HPP
