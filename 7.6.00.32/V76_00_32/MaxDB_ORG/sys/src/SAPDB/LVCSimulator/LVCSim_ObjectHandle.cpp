/*!***************************************************************************

  module      : LVCSim_ObjectHandle.cpp

  -------------------------------------------------------------------------

  responsible : ChristophR

  special area: liveCache Simulator
  description : Internal object handle 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*****************************************************************************/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "LVCSimulator/LVCSim_ObjectHandle.hpp"
#include "LVCSimulator/LVCSim_OIDAllocator.hpp"
#include "LVCSimulator/LVCSim_liveCacheSink.hpp"
#include "LVCSimulator/LVCSim_ObjectTree.hpp"
#include "LVCSimulator/LVCSim_Internal.hpp"


/*!
 * Locker class for concurrent object access.
 *
 * Instance of this class protects object metadata from concurrent access by
 * multiple transactions. It does NOT do locking of objects as such, there
 * is LVCSim_ObjectHandle::m_lock member for that.
 *
 * This class doesn't try to do garbage collection.
 */
class LVCSim_ObjectHandleObjLockNoGC {
public:
    enum {
        /// Count of different object spinlocks.
        MAX_OBJ_LOCKS   = 59
    };

    /*!
     * Initialize lock instance and lock the lock.
     *
     * \param ref reference to object handle to lock,
     * \param sid sink ID.
     */
    LVCSim_ObjectHandleObjLockNoGC(LVCSim_ObjectHandle *ref, LVCSim_SID sid)
        : m_spinlock(&locks[ref->hash() % MAX_OBJ_LOCKS])
    {
        m_spinlock->Lock();
    }

    /// Unlock the lock and destroy the instance.
    ~LVCSim_ObjectHandleObjLockNoGC()
    {
        if (m_spinlock) {
            m_spinlock->Unlock();
        }
    }
private:
	static RTESync_Spinlock	locks[MAX_OBJ_LOCKS];   ///< Underlying spinlocks.
protected:
    RTESync_Spinlock        *m_spinlock;            ///< Spinlock for this instance.
};

/// Garbage-collecting version of LVCSim_ObjectHandleObjLockNoGC.
class LVCSim_ObjectHandleObjLock : public LVCSim_ObjectHandleObjLockNoGC {
public:
    /*!
     * Initialize lock instance and lock the lock.
     *
     * Before locking the handle, try to garbage-collect unneeded versions.
     *
     * \param ref reference to object handle to lock,
     * \param sid sink ID.
     */
	LVCSim_ObjectHandleObjLock(LVCSim_ObjectHandle *ref, LVCSim_SID sid)
		: LVCSim_ObjectHandleObjLockNoGC(ref, sid)
	{
   		// optimization & GC - called before lock itself
        if (ref->m_lock == NULL && !ref->isCleared()) {
        	LVCSim_TID tid = LVCSim_liveCacheSink::getOldestTID();
            if (ref->m_tid_fmax < tid) {
                ref->GC(tid, sid, m_spinlock);
            }
        }
	}
};


RTESync_Spinlock	LVCSim_ObjectHandleObjLockNoGC::locks[MAX_OBJ_LOCKS];



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
 * Garbage collection. Remove all versions older than the oldest transaction ID,
 * since no transaction can see them. In case the object as such cannot be seen,
 * remove the object as a whole and return the OID to OID allocator.
 *
 * \param tid oldest transaction ID,
 * \param sid sink ID doing the garbage collection,
 * \param spinlock spinlock protecting the object.
 */
void LVCSim_ObjectHandle::GC(LVCSim_TID tid, LVCSim_SID sid, RTESync_Spinlock* &spinlock)
{
	if (m_tid_max < tid) {
		// the whole object is not valid anymore
        m_lock = sid;
        spinlock->Unlock();
        spinlock = NULL;
		destroy(LVCSim_liveCacheSink::getContainer(getContainer()), sid);
		return;
	}

	// remove all too old m_versions
	LVCSim_ObjectVerHandle *v = m_versions;
	LVCSim_ObjectVerHandle *last = NULL;
	while (v) {
		if (v->m_tid_min < tid) {
			// we do not need older versions
            if (last) {
                m_tid_fmax = last->m_tid_min;
            } else {
                m_tid_fmax = m_tid_max;
            }
			last = v->m_next;
			v->m_next = NULL;
            m_tid_min = v->m_tid_min;
			delete last;
			return;
		}
		last = v;
		v = v->m_next;
	}

	// we shouldn't get here
    LVCSIM_ASSERT(false);
}

/*!
 * Check lock status of the object.
 *
 * \param sid sink for which we are working.
 *
 * \return error code:
 *  - e_object_not_found when the object is not visible in given transaction,
 *  - e_object_not_locked when the object is not locked.
 */
short LVCSim_ObjectHandle::checkLock(LVCSim_SID sid)	// m_lock the object
{
	if (m_lock == sid) return e_ok;
    if (!isVisibleNoLock(sid)) {
        return e_object_not_found;
    }

	return e_object_not_locked;	// we can't m_lock ourselves - app must do it self
}

/*!
 * Check share lock status of the object.
 *
 * \param sid sink for which we are working.
 *
 * \return error code:
 *  - e_object_not_found when the object is not visible in given transaction,
 *  - e_object_not_locked when the object is not locked.
 */
short LVCSim_ObjectHandle::checkShareLock(LVCSim_SID sid)	// m_lock the object
{
	for (LVCSim_SharedLockListItem *o = m_sharedLockList; o != NULL; o = o->m_next)
    if (o->m_sid == sid) //we already have a shared lock  
      return e_ok;
  if (!isVisibleNoLock(sid)) {
      return e_object_not_found;
  }
	return e_object_not_locked;	// we can't m_lock ourselves - app must do it self
}

/*!
 * Test for visibility.
 *
 * This variation doesn't spin-lock the object before the test. Use where
 * the object is already locked.
 *
 * \param sid sink ID
 *
 * \return \c true, if the object is visible within the consistent view of
 *  the sink, \c false otherwise.
 */
bool LVCSim_ObjectHandle::isVisibleNoLock(LVCSim_SID sid) const
{
    if (m_lock == sid && sid->m_activeVersion == NULL) {
        // locked by us, we see it, if not yet deleted
        return (m_tid_max == LVCSim_TID_MAX);
    }

    if (!sid->sees(m_tid_min)) {
        // we do not see minimum TID
        return false;
    }
    if (sid->sees(m_tid_max)) {
        // we see maximum TID, ie, deleted
        return false;
    }
    // else there is a version that we see
    return true;
}



/*!
 * Test for visibility.
 *
 * \param sid sink ID
 *
 * \return \c true, if the object is visible within the consistent view of
 *  the sink, \c false otherwise.
 */
bool LVCSim_ObjectHandle::isVisible(LVCSim_SID sid)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);
	return isVisibleNoLock(sid);
}

/*!
 * Test if object is a variable-sized continuation object
 *
 * \return \c true, if the object is a variable-sized continuation object,
 * \c false otherwise.
 */
bool LVCSim_ObjectHandle::isContObj(LVCSim_SID sid)
{
   if (!isVisibleNoLock(sid)) {
        return false;
   }
	LVCSim_TID tid = sid->m_readView;
	LVCSim_ObjectVerHandle *v = m_versions;
	while (v) {
		if (v->m_tid_min <= tid && sid->sees(v->m_tid_min)) {
			// ok, we see this object version
      return v->m_contObj;
		}
		v = v->m_next;
	}
  return false; //version not found
}

/*!
 * Test if object history is used
 *
 * \return \c true, if any sids use the object version history,
 * \c false otherwise.
 */
bool LVCSim_ObjectHandle::isObjectHistoryUsed(LVCSim_SID sid)
{
  LVCSim_ObjectHandleObjLock objectLock(this, sid);
  if(!m_versions->m_next)
    return false; // no version history

  LVCSim_ObjectVerHandle *oldestVersion =m_versions->m_next;
  while(oldestVersion->m_next) // get oldest object version
      oldestVersion = oldestVersion->m_next;
  
  RTESync_LockedScope sessionLock(LVCSim_liveCacheSink::txCounterLock); // we access session list, so lock txCounterLock
  
  for( sid = sid->firstSession; sid != NULL; sid = sid->m_next){ // check all current sessions
    if(!sid->sees(m_versions->m_tid_min) && sid->sees(oldestVersion->m_tid_min))
      // sid doesn't see the current object version but sees the oldest object version
      // so object history is used
      return true;
  }

  // all sids see the current object version or the object not at all
  return false;
}

/*!
 * Test for visibility for changes.
 *
 * This variation doesn't spin-lock the object before the test. Use where
 * the object is already locked.
 *
 * \param sid sink ID
 *
 * \return \c e_ok, if the object can be changed by the sink, \c e_object_not_found or
 *  \c e_object_dirty otherwise.
 */
short LVCSim_ObjectHandle::isVisibleForChangeNoLock(LVCSim_SID sid) const
{
    if (m_lock == sid) {
        // already locked by us
        if (m_tid_max == LVCSim_TID_MAX) {
            return e_ok;
        } else {
            // already deleted - we see it not anymore
            return e_object_not_found;
        }
    }
    if (sid->hasReadView()) {
        if (!isVisibleNoLock(sid)) {
            return e_object_not_found;
        }
    }
    if (m_lock != sid && m_lock != NULL) {
    	// TODO: now wait until the m_lock is possible, then retest
    	return e_OMS_request_timeout;
    }
    if (m_sharedLockList != NULL) {
      if( m_sharedLockList->m_sid != sid || m_sharedLockList->m_next != NULL)
    	  return e_OMS_request_timeout;
    }
    if (m_tid_max != LVCSim_TID_MAX || (sid->m_readView != -1 && sid->m_readView < m_versions->m_tid_min) ) { // PTS 1127855
        return e_object_dirty;
    }
    if (sid->hasReadView()) {
        if (!sid->sees(m_versions->m_tid_min)) {
            return e_object_dirty;
        }
    }
    return e_ok;
}

/*!
 * Test for visibility for shared access.
 *
 * This variation doesn't spin-lock the object before the test. Use where
 * the object is already locked.
 *
 * \param sid sink ID
 *
 * \return \c e_ok, if the object can be share locked by the sink, \c e_object_not_found or
 *  \c e_object_dirty otherwise.
 */
short LVCSim_ObjectHandle::isVisibleForSharedAccessNoLock(LVCSim_SID sid) const
{
    if (m_lock == sid) {
        // already locked by us
        if (m_tid_max == LVCSim_TID_MAX) {
            return e_ok;
        } else {
            // already deleted - we see it not anymore
            return e_object_not_found;
        }
    }
    if (sid->hasReadView()) {
        if (!isVisibleNoLock(sid)) {
            return e_object_not_found;
        }
    }
    if (m_lock != sid && m_lock != NULL) {
    	// TODO: now wait until the m_lock is possible, then retest
    	return e_OMS_request_timeout;
    }
    if (m_tid_max != LVCSim_TID_MAX) {
      return e_object_dirty;
    }
    if (m_tid_max == LVCSim_TID_MAX){
      LVCSim_ObjectVerHandle *v = m_versions;
      while( v != NULL && v->m_isShared)
        v = v->m_next;
      if (v != NULL){
        if(!sid->sees(v->m_tid_min)) {
          return e_object_dirty;
        }
      }
    }
    return e_ok;
}

/*!
 * Test for visibility for changes.
 *
 * \param sid sink ID
 *
 * \return \c true, if the object can be changed by the sink, \c false otherwise.
 */
bool LVCSim_ObjectHandle::isVisibleForChange(LVCSim_SID sid)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);
	return isVisibleForChangeNoLock(sid) == e_ok;
}


/*!
 * Test if this object's key can be reused.
 *
 * \param sid sink ID of the transaction.
 *
 * \return \c e_ok, if the object is not visible and it's key can be
 *  reused, \c e_duplicate_hash_key or \c e_OMS_lock_collision otherwise.
 */
short LVCSim_ObjectHandle::isNewable(LVCSim_SID sid)
{
	LVCSim_ObjectHandleObjLockNoGC lck(this, sid);

    if (m_lock == sid) {
      // if we deleted the last version, we may insert object with the same key
      if (m_tid_max != LVCSim_TID_MAX){
        return e_ok;
      }
      else {
        return e_duplicate_hash_key;
      }
    }

    if (sid->hasReadView()) {
        if (!sid->sees(m_tid_min)) {
          // we do not see minimum TID - created after us
          return e_OMS_lock_collision;
        }
        if (sid->sees(m_tid_max)) {
          // we see maximum TID, ie, deleted -> we may create new
          return e_ok;
        }
    } else {
        // no read view, use only max time for comparison
        if (sid->getCommitTime() > m_tid_max) {
            // we work after the TX that deleted the object, check if commited
            if (LVCSim_liveCacheSink::checkCommited(m_tid_max)) {
                return e_ok;
            }
        }
        else if (m_lock && m_lock->getCommitTime() == m_tid_min) {
          return e_OMS_lock_collision;
        }

    }
    // else there is a version (not last) that we see or some other problem
    return e_duplicate_hash_key;
}

/*!
 * Try to exclusively lock this object.
 *
 * This function locks the object. However, as a special function,
 * one can request marking the object as updated in this consistent view, in
 * order to prevent concurrent transactions from updating the object.
 *
 * \param sid sink ID of transaction,
 * \param updateLen if nonzero, simulate update (updateLen is then object size),
 * \param ignoreView ignore current consistent view when locking the object.
 *
 * \return \c e_ok, if locked successfully, error otherwise (\c e_object_not_found,
 *  \c e_OMS_request_timeout or \c e_object_dirty).
 */
short LVCSim_ObjectHandle::lockObject(LVCSim_SID sid, size_t updateLen, bool ignoreView)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);

   if (ignoreView) {
        if (m_lock == sid) {
            // already locked by us
            if (m_tid_max == LVCSim_TID_MAX) {
                return e_ok;
            } else {
                // already deleted - we don't see it anymore
                return e_object_not_found;
            }
        }
        if (m_lock != sid && m_lock != NULL) {
            // TODO: now wait until the m_lock is possible, then retest
            return e_OMS_request_timeout;
        }
        if (m_tid_max != LVCSim_TID_MAX) {
            return e_object_dirty;
        }
        if (m_sharedLockList != NULL){  // PTS 1131580
          if(m_sharedLockList->m_sid != sid || m_sharedLockList->m_next != NULL)
            // TODO: now wait until the m_lock is possible, then retest
            return e_OMS_request_timeout;
        }
    } else {
        short err = isVisibleForChangeNoLock(sid);
        if (err != e_ok) {
            return err;
        }
    }

    // now we know, we either have the lock already, or noone has the lock
	if (!m_lock) {
      // not yet locked, lock it
      // 
   		m_lock = sid;

      // add object to the transaction lock chain
      if(m_sharedLockList != NULL){ // we have a share lock and so a LockChainItem exists
        m_sharedLockList->m_correspondingLockChainItem->m_isExclusive = true;
        m_correspondingLockChainItem = m_sharedLockList->m_correspondingLockChainItem;
      }
      else {
        // add object to the transaction lock chain
        m_correspondingLockChainItem = new LVCSim_ObjectLockChainItem(this,true /*exclusive*/,NULL,sid->m_lockChain);
        sid->m_lockChain = m_correspondingLockChainItem;
      }
    }
	if (updateLen) {
        // simulate update on the object by creating a new version
    OMS_ObjectId8 oid;
    getCurrentOid(&oid);
		m_versions = new(updateLen) LVCSim_ObjectVerHandle(
            sid->getCommitTime(),
			m_versions->m_data.fix, updateLen, false, false, m_versions->m_contObj, oid, m_versions);
	}

	return e_ok;	// locked
}

/*!
 * Try to share lock this object.
 *
 * This function share lock the object. 
 *
 * \param sid sink ID of transaction,
 * \param ignoreView ignore current consistent view when locking the object.
 *
 * \return \c e_ok, if locked successfully, error otherwise (\c e_object_not_found,
 *  \c e_OMS_request_timeout or \c e_object_dirty).
 */
short LVCSim_ObjectHandle::lockObjectShared(LVCSim_SID sid, bool ignoreView)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);

  if (ignoreView) {
    if (m_lock == sid) {
        // locked by us
        if (m_tid_max != LVCSim_TID_MAX) {
            // already deleted - we don't see it anymore
            return e_object_not_found;
        }
    }
    if (m_lock != sid && m_lock != NULL) {
        // TODO: now wait until the m_lock is possible, then retest
        return e_OMS_request_timeout;
    }
    if (m_tid_max != LVCSim_TID_MAX) {
        return e_object_dirty;
    }
  } else {
      short err = isVisibleForSharedAccessNoLock(sid);
      if (err != e_ok) {
        return err;
      }
  }

  for (LVCSim_SharedLockListItem *o = m_sharedLockList; o != NULL; o = o->m_next)
    if (o->m_sid == sid) //we already have a shared lock  
      return e_ok;
    
  // now we know, no other transaction has a exclusive lock and we don't have a shared lock
  // lock it

  // add sid to the object shared lock list
  m_sharedLockList = new LVCSim_SharedLockListItem(sid,NULL,m_sharedLockList);
  if (m_sharedLockList->m_next != NULL)
    m_sharedLockList->m_next->m_prev = m_sharedLockList;

  // add object to the transaction lock chain
  if(m_correspondingLockChainItem == NULL){
    sid->m_lockChain = new LVCSim_ObjectLockChainItem(this,false /*share*/,m_sharedLockList,sid->m_lockChain);
    m_sharedLockList->m_correspondingLockChainItem = sid->m_lockChain;
  }else {
    m_correspondingLockChainItem->m_isShare = true;
    m_correspondingLockChainItem->m_correspondingObjectLock = m_sharedLockList;
    m_sharedLockList->m_correspondingLockChainItem = m_correspondingLockChainItem;
  }
	return e_ok;	// locked
}

/*!
 * Unlock the object.
 *
 * \param sid sink ID.
 *
 * This method unlocks the object, if it is locked by the given sink.
 * Currently, the locks are held in a singly-linked list, so this operation
 * is relatively expensive. However, it is called very seldom.
 *
 * \return \c e_ok, if unlocked successfully, error otherwise.
 */
short LVCSim_ObjectHandle::unlockObject(LVCSim_SID sid)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);
	if (m_lock == sid) {
    // test if modified
    if (m_tid_max != LVCSim_TID_MAX || 
        (m_versions && m_versions->m_tid_min == sid->getCommitTime())) {
        return e_object_dirty;
    }
	  // must remove from m_chain

    // delete lock chain item of the object
    LVCSim_LockChainItem *curr = m_correspondingLockChainItem;
    m_correspondingLockChainItem = NULL;
    if(curr != NULL && curr->m_isExclusive){
      if(curr->m_isShare)
        curr->m_isExclusive = false;
      else {
        if(curr->m_prev)
          curr->m_prev->m_next = curr->m_next;
        if(curr->m_next)
          curr->m_next->m_prev = curr->m_prev;
        if(!curr->m_prev)
          sid->m_lockChain = curr->m_next;
        delete curr;
      }
    }else{
      LVCSIM_THROW("unlocking not locked object???");
	  }
    m_lock = NULL;
		return e_ok;
  }
  if (!isVisibleNoLock(sid)) {
    return e_object_not_found;
  } 
  return e_object_not_locked;	    
}

/*!
 * Unlock the object.
 *
 * \param sid sink ID.
 *
 * This method unlocks the object, if it is locked by the given sink.
 * Currently, the locks are held in a singly-linked list, so this operation
 * is relatively expensive. However, it is called very seldom.
 *
 * \return \c e_ok, if unlocked successfully, error otherwise.
 */
short LVCSim_ObjectHandle::unlockSharedObject(LVCSim_SID sid)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);
  short error = checkShareLock(sid);
  if(error == e_ok) {
    
    // get shared lock from sharedLockList
    LVCSim_SharedLockListItem *sharedLock = m_sharedLockList;
    while(sharedLock){
      if(sharedLock->m_sid == sid)
        break;
      sharedLock = sharedLock->m_next;
    }
    if(sharedLock){
      // delete lock chain item of the object
      LVCSim_LockChainItem *curr = sharedLock->m_correspondingLockChainItem;
      if(curr != NULL && curr->m_isShare){
        if(curr->m_isExclusive)
          curr->m_isShare = false;
        else {
          if(curr->m_prev)
            curr->m_prev->m_next = curr->m_next;
          if(curr->m_next)
            curr->m_next->m_prev = curr->m_prev;
          if(!curr->m_prev)
            sid->m_lockChain = curr->m_next;
          delete curr;
        }
      }else{
        LVCSIM_THROW("unlocking not locked object???");
	    }
      // delete shared lock from sharedLockList
      if(sharedLock->m_prev)
        sharedLock->m_prev->m_next = sharedLock->m_next;
      if(sharedLock->m_next)
        sharedLock->m_next->m_prev = sharedLock->m_prev;
      if(!sharedLock->m_prev)
        m_sharedLockList = sharedLock->m_next;
      delete sharedLock;
    }
    else{
      LVCSIM_THROW("unlocking not locked object???");
	  }
  }
  return error;	    
}

/*!
 * Check if object is locked by given sink.
 *
 * \param sid sink ID.
 * \param shared Check shared lock.
 *
 * This methods checks if the object is locked by the given sink.
 * If shared flag is false, the method checks if the sink holds a exclusive lock,
 * otherwise it checks if the sink holds a shared lock.
 *
 * \return \c e_ok, if sink holds lock, error code otherwise.
 */
short LVCSim_ObjectHandle::isLocked(LVCSim_SID sid, bool shared)
{
  LVCSim_ObjectHandleObjLock lck(this, sid);

  short error;
  if(!shared)
    error = checkLock(sid);
  else
    error = checkShareLock(sid);
  
  return error;
}

/*!
 * Looks up the object for consistent view identified by transaction ID.
 *
 * \param sid sink ID of the transaction,
 * \param hist increased for each history read (in/out).
 *
 * \return pointer to the object version header or NULL if the object
 *  is invisible in this consistent view.
 */
LVCSim_ObjectVerHandle *LVCSim_ObjectHandle::lookupObject(LVCSim_SID sid, int &hist)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);

    if (!isVisibleNoLock(sid)) {
        return NULL;
    }
	LVCSim_TID tid = sid->m_readView;
	LVCSim_ObjectVerHandle *v = m_versions;
	while (v) {
		if (v->m_tid_min <= tid && sid->sees(v->m_tid_min)) {
			// ok, we see this object
			return v;
		}
		v = v->m_next;
		++hist;
	}

	return NULL;	// visible version not found??? shouldn't happen
}


/*!
 * Update the object by creating new version. Instead of copying current
 * status to a new before image, we create a new version of the object
 * and link it to the old version, which becomes before image.
 *
 * \param sid sink ID for the transaction,
 * \param data pointer to new data,
 * \param len data size
 * \param varsize \c true if variable-sized object, \c false otherwise.
 * \param contObj \c true if variable-sized continuation object, \c false otherwise
 *
 * \return \c e_ok, if successful, error code otherwise.
 */
short LVCSim_ObjectHandle::update(LVCSim_SID sid, const void *data, size_t len,
	bool varsize, bool contObj)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);

	short e = checkLock(sid);
    if (e != e_ok) {
        return e;
    }

    if (m_tid_max != LVCSim_TID_MAX) {
        e = e_object_dirty;
    } else {
        LVCSim_TID ctime = sid->getCommitTime();
        if (m_versions) {
            if (m_tid_fmax == LVCSim_TID_MAX) {
                m_tid_fmax = ctime;
            }
        } else {
            m_tid_fmax = LVCSim_TID_MAX;
        }
    OMS_ObjectId8 oid;
    getCurrentOid(&oid);
		m_versions = new(len + (varsize ? sizeof(size_t) : 0)) 
			LVCSim_ObjectVerHandle(ctime, data, len, varsize, false, contObj, oid, m_versions);
    }
	return e;
}

/*!
 * Pseudo update the object after shared access by creating new version. Instead of copying current
 * status to a new before image, we create a new version of the object
 * and link it to the old version, which becomes before image.
 *
 * \param sid sink ID for the transaction,
 * \param len data size

 *
 * \return \c e_ok, if successful, error code otherwise.
 */
short LVCSim_ObjectHandle::updateShare(LVCSim_SID sid, size_t len)
{
	//LVCSim_ObjectHandleObjLock lck(this, sid);

	short e = checkShareLock(sid);
    if (e != e_ok) {
        return e;
    }

    if (m_tid_max != LVCSim_TID_MAX) {
        e = e_object_dirty;
    } else {
        LVCSim_TID ctime = sid->getCommitTime();
        if (m_versions) {
            if (m_tid_fmax == LVCSim_TID_MAX) {
                m_tid_fmax = ctime;
            }
        } else {
            m_tid_fmax = LVCSim_TID_MAX;
        }
    // simulate update on the object by creating a new version
    OMS_ObjectId8 oid;
    getCurrentOid(&oid);
		m_versions = new(len) LVCSim_ObjectVerHandle(
      sid->getCommitTime(),m_versions->m_data.fix, len, false, true, m_versions->m_contObj, oid, m_versions);
    }
	return e;
}

/*!
 * Logically delete the object.
 *
 * \param sid sink ID for current transaction.
 *
 * \return \c e_ok if successful, error code otherwise.
 */
short LVCSim_ObjectHandle::remove(LVCSim_SID sid)
{
	LVCSim_ObjectHandleObjLock lck(this, sid);

	short e = checkLock(sid);
    if (e != e_ok) {
        return e;
    }

    if (m_tid_max == LVCSim_TID_MAX) {
        m_tid_max = sid->getCommitTime();
        if (m_tid_fmax == LVCSim_TID_MAX) {
            m_tid_fmax = m_tid_max;
        }
    } else {
		e = e_object_dirty;
    }
	return e;
}

/*!
 * Do commit/rollback for this item (low-level).
 *
 * This method undoes the changes made for rollback and does nothing for commit.
 *
 * \param sid sink ID,
 * \param commit \c true, if commiting, \c false if doing rollback,
 * \param operation one of OpType constants, representing operation.
 */
void LVCSim_ObjectHandle::doEndTxLL(LVCSim_SID sid, bool commit, char operation)
{
    if (commit) {
        // no need to do anything
        return;
    }

   	LVCSim_ObjectHandleObjLockNoGC lck(this, sid);

    switch (operation) {
    case o_delete:
        // set max time back
        if (m_versions && m_versions->m_next) {
            // there is more than one version - find the oldest
            LVCSim_ObjectVerHandle *vh = m_versions;
            while (vh->m_next) {
                m_tid_fmax = vh->m_tid_min;
                vh = vh->m_next;
            }
        } else {
            // only single version
            m_tid_fmax = LVCSim_TID_MAX;
        }
        m_tid_max = LVCSim_TID_MAX;
        break;

    case o_new:
        // clear - no need for this object anymore, it was just created in this TX
        clear();
        break;

    case o_update:
        // remove last added version
        {
            LVCSim_ObjectVerHandle *vh = m_versions, *vd = vh;
            m_tid_fmax = LVCSim_TID_MAX;
            m_versions = vh = vh->m_next;
            while (vh) {
                m_tid_fmax = vh->m_tid_min;
                vh = vh->m_next;
            }
            vd->m_next = NULL;
            delete vd;
        }
        break;

    default:
        LVCSIM_ASSERT(false);
    }
}

/*!
 * Check, if the object has been cleared.
 *
 * \return \c true, if already cleared, \c false otherwise.
 */
bool LVCSim_ObjectHandle::isCleared() const
{
    return (m_tid_min == LVCSim_TID_MAX && m_tid_max == 0 && m_tid_fmax == 0);
}

/*!
 * Clear object metadata.
 *
 * Clearing the metadata is required for proper interaction between GC and
 * transaction handling.
 */
void LVCSim_ObjectHandle::clear()
{
	m_tid_min = LVCSim_TID_MAX;
	m_tid_max = m_tid_fmax = 0; 

	LVCSim_ObjectVerHandle *v = m_versions;
	m_versions = NULL;
	//m_lockChain = NULL;
	delete v;
}

/*!
 * Destroy the object handle.
 */
LVCSim_ObjectHandle::~LVCSim_ObjectHandle()
{
	clear();

    m_lock = NULL;
}

void *LVCSim_ObjectHandle::operator new(size_t sz, LVCSim_ContainerMeta *cont)
{
	return cont->allocate(sz);
}

void *LVCSim_ObjectHandle::operator new(size_t sz, LVCSim_OIDAllocator *alloc, unsigned int oid)
{
	return alloc->map(oid);
}

void LVCSim_ObjectHandle::operator delete(void *ptr, LVCSim_ContainerMeta *cont)
{
	if (ptr) cont->deallocate(ptr);
}

void LVCSim_ObjectHandle::operator delete(void *ptr, LVCSim_OIDAllocator *alloc, unsigned int oid)
{
	// nothing to do, this is only to suppress warning
}

/*!
 * Destroy the object and associated information.
 *
 * This method frees all resources associated with the object and returns the
 * object space (and its OID) back to the containing container.
 *
 * \param cont container containing the object,
 * \param sid sink destroying the object.
 */
void LVCSim_ObjectHandle::destroy(LVCSim_ContainerMeta *cont, LVCSim_SID sid)
{
	// do key delete here as well
  if (cont != NULL)
  {
    if (cont->m_tree) {
        cont->m_tree->delKey(this, sid);
    }

	  //LVCSim_ObjectHandleObjLockNoGC lck(this, sid);
    clear();

    m_lock = NULL;
	  cont->deallocate(this);
  }
}


/*!
 * Create a new object handle from checkpoint.
 * 
 * \param i checkpoint reader (data source),
 * \param objsize object size in bytes.
 */
LVCSim_ObjectHandle::LVCSim_ObjectHandle(LVCSim_CheckpointReader &i, size_t objsize) 
  : m_lock(NULL), m_node(NULL), m_lockChain(NULL), m_sharedLockList(NULL)
{
	i.read(this, sizeof(LVCSim_OIDObject));
	i.read(&m_tid_max, sizeof(m_tid_max));
	i.read(&m_tid_min, sizeof(m_tid_min));
	i.read(&m_tid_fmax, sizeof(m_tid_fmax));
	//i.read(&m_operation, 1);

	size_t size = 0;
	i.read(&size, sizeof(size));
	if (size) {
		m_versions = new(size) LVCSim_ObjectVerHandle(i, objsize);
	} else m_versions = NULL;
}

void LVCSim_ObjectHandle::writeToStream(LVCSim_CheckpointWriter &o, size_t objsize) const
{
	o.write(this, sizeof(LVCSim_OIDObject));
	o.write(&m_tid_max, sizeof(m_tid_max));
	o.write(&m_tid_min, sizeof(m_tid_min));
	o.write(&m_tid_fmax, sizeof(m_tid_fmax));
	//o.write(&m_operation, 1);

	if (m_versions) {
		m_versions->writeToStream(o, objsize);
	} else {
		size_t size = 0;
		o.write(&size, sizeof(size));
	}
}

/*!
 * Unlock the Object
 */
void LVCSim_ObjectLockChainItem::unlock(LVCSim_SID sid){
  LVCSim_ObjectHandleObjLock lck(m_objectHandle, sid);
  m_objectHandle->m_correspondingLockChainItem = NULL;
  if( m_isShare) {
    // get object size
    LVCSim_ContainerMeta *cont = sid->getContainer(m_objectHandle->getContainer());

    // TODO: check - when new varobj with no data will be locked
    size_t updateLen = cont->m_size ? cont->m_size :
      (m_objectHandle->m_versions ? m_objectHandle->m_versions->m_data.var.size + sizeof(size_t) : 0);

    // update object, create new object version
    m_objectHandle->updateShare(sid,updateLen);

    // unlock shared lock
    LVCSim_SharedLockListItem *curr = m_correspondingObjectLock;
    if(curr){
      if(curr->m_prev)
        curr->m_prev->m_next = curr->m_next;
      if(curr->m_next)
        curr->m_next->m_prev = curr->m_prev;
      if(!curr->m_prev)
        m_objectHandle->m_sharedLockList = curr->m_next;
      delete curr;
    }
  }
  if (m_isExclusive){
    // unlock the object
    m_objectHandle->m_lock = NULL;
    if( m_objectHandle->isCleared()){
      // object is cleared, destroy it
      LVCSim_ContainerMeta *meta = sid->getContainer(m_objectHandle->getContainer());
      if(meta != NULL)
        m_objectHandle->destroy(meta, sid);
    }
  }
}
