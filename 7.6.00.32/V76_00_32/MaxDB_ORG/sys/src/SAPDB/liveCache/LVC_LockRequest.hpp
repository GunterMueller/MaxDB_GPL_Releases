/*!
 * \file    LVC_LockRequest.hpp
 * \author  IvanS
 * \brief   Lock request for RW lock.
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

#ifndef __LVC_LOCKREQUEST_HPP
#define __LVC_LOCKREQUEST_HPP

#include "ggg00.h"

/// Lock request class.
class LVC_LockRequest {
public:
  enum {
    MAX_RW_SPINLOCKS = 32   ///< # of R/W spinlocks to use
  };

  /// Lock action type.
  typedef enum {
    RWLOCK_CREATE,            ///< Create the lock with given ID
    RWLOCK_LOCK_EXCLUSIVE,    ///< lock exclusive
    RWLOCK_LOCK_SHARED,       ///< lock shared
    RWLOCK_TRYLOCK_EXCLUSIVE, ///< try to lock exclusive (return e_request_timeout if cannot lock)
    RWLOCK_TRYLOCK_SHARED,    ///< try to lock shared (return e_request_timeout if cannot lock)
    RWLOCK_UNLOCK_EXCLUSIVE,  ///< unlock exclusive
    RWLOCK_UNLOCK_SHARED,     ///< unlock shared
    RWLOCK_IS_LOCKED,         ///< check if is locked by own session

    RWLOCK_ACTION_MAX = RWLOCK_IS_LOCKED  ///< Maximum action ID
  } LockAction;

  /// Create new lock request.
  LVC_LockRequest(LockAction a, SAPDB_Int4 areaId, SAPDB_Int4 lockId, void *lockAddress, tsp00_TaskId taskId = 0)
	  : m_action(a), m_areaId(areaId), m_lockId(lockId), m_lockAddress(lockAddress), m_taskId(taskId)
  {
  }

  LockAction  m_action;   ///< Lock action to perform
  SAPDB_Int4  m_areaId;   ///< Area ID
  SAPDB_Int4  m_lockId;   ///< Lock ID
  tsp00_TaskId m_taskId;  ///< Session task id
  void *m_lockAddress;	  ///< Lock address

  // here come enhancements for other lock types later
};

#endif  // __LVC_LOCKREQUEST_HPP
