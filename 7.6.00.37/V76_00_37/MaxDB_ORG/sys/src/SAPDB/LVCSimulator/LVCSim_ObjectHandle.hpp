/*!***************************************************************************

  module      : LVCSim_ObjectHandle.hpp

  -------------------------------------------------------------------------

  responsible : ChristophR

  special area: liveCache Simulator
  description : Internal object handle 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG




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




*****************************************************************************/


#ifndef LVCSIM_OBJECTHANDLE_HPP
#define LVCSIM_OBJECTHANDLE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

//#include "SAPDBCommon/SAPDB_Types.hpp"
#include "LVCSimulator/LVCSim_ObjectVerHandle.hpp"
#include "LVCSimulator/LVCSim_CheckpointIO.hpp"
#include "LVCSimulator/LVCSim_liveCacheSink.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * \brief Object handle
 *
 * Defines the control block of a persistent object in memory.
 * This handle is allocated for each persistent object exactly once,
 * independent of how many past versions (before images) exist. OIDs
 * in simulator are actually logical pointers to objects of this type.
 */

class LVCSim_ObjectHandle;
class LVCSim_SharedLockListItem;

//class LVCSim_ObjectLockChainItem{
//public:
//  LVCSim_ObjectLockChainItem(LVCSim_ObjectHandle *objectHandle, bool isExclusive, LVCSim_SharedLockListItem *lockItem, LVCSim_ObjectLockChainItem *next)
//    : m_objectHandle(objectHandle), m_isExclusive(isExclusive), m_correspondingObjectLock(lockItem), m_next(next)
//  {}
//  LVCSim_ObjectHandle *m_objectHandle;
//  bool m_isExclusive;
//  LVCSim_SharedLockListItem *m_correspondingObjectLock;
//  LVCSim_ObjectLockChainItem *m_next;
//};
   
class LVCSim_ObjectHandle : public LVCSim_OIDObject
{
public:
    /// Operation type, what has happened to the object in transaction.
    typedef enum {
        o_new,      ///< The object was created.
        o_update,   ///< The object was updated.
        o_delete    ///< The object was deleted.
    } OpType;

    /// Create a new object handle from checkpoint.
    LVCSim_ObjectHandle(LVCSim_CheckpointReader &r, size_t objsize);
    /// Create a new object handle.
    LVCSim_ObjectHandle(OMS_ObjectId8 *oid, tgg91_PageRef *ref, LVCSim_SID tx)
        : m_lock(tx), m_lockChain(NULL),
          m_tid_max(LVCSim_TID_MAX), m_tid_min(tx->m_commitTime), m_tid_fmax(LVCSim_TID_MAX),
          m_node(NULL), m_versions(NULL)
    {
        getCurrentOid(oid);
        getRef(ref);
        m_correspondingLockChainItem = new LVCSim_ObjectLockChainItem(this,true,NULL,tx->m_lockChain);
        tx->m_lockChain = m_correspondingLockChainItem;
    }

    /// Destroy the object handle.
    ~LVCSim_ObjectHandle();

    /// Lookup visible object version.
    LVCSim_ObjectVerHandle *lookupObject(LVCSim_SID sid, int &histCount);

    // Get oid of object version visible in given transaction.
    void getOid(LVCSim_SID sid, OMS_ObjectId8 *lpoid){
      int histCount=0;
      LVCSim_ObjectVerHandle *version = lookupObject(sid,histCount);
      if(version)
        version->getOid(lpoid);
      else
        getCurrentOid(lpoid);
    }

    /// Test for visibility.
    bool isVisible(LVCSim_SID sid);
    /// Test for visibility for changes.
    bool isVisibleForChange(LVCSim_SID sid);
    /// Test if this object's key can be reused.
    short isNewable(LVCSim_SID sid);
    /// Test if object is a variable-sized continuation object
    bool isContObj(LVCSim_SID sid);
    /// Test if object history is used
    bool isObjectHistoryUsed(LVCSim_SID sid);

    /// Returns true, if the object is a keyed object
    inline bool isKeyedObj() const
    {
      return m_node != NULL;
    }

    /// Check if object is locked by given sink.
    short isLocked(LVCSim_SID sid, bool shared = false);
    /// Try to exclusively lock this object.
    short lockObject(LVCSim_SID sid, size_t updateLen = 0, bool ignoreView = false);
    /// Try to exclusively lock this object.
    short lockObjectShared(LVCSim_SID sid, bool ignoreView = false);
    /// Unlock the exclusive lock.
    short unlockObject(LVCSim_SID sid);
    /// Unlock the share lock.
    short unlockSharedObject(LVCSim_SID sid);

    /// Update the object.
    short update(LVCSim_SID sid, const void *data, size_t len, bool varsize, bool contObj);
    /// Pseudo update the object after shared access.
    short updateShare(LVCSim_SID sid, size_t len);
    /// Logically delete the object.
    short remove(LVCSim_SID sid);

    /// Do commit/rollback for this item (low-level).
    void doEndTxLL(LVCSim_SID sid, bool commit, char operation);
    
    /// Operator new allocating the object in container.
    static void *operator new(size_t sz, LVCSim_ContainerMeta *cont);
    /// Operator delete deallocating the object in container.
    static void operator delete(void *ptr, LVCSim_ContainerMeta *cont);
    /// Operator new allocating the object in OID allocator.
    static void *operator new(size_t sz, LVCSim_OIDAllocator *alloc, unsigned int oid);
    /// Operator delete deallocating the object in OID allocator.
    static void operator delete(void *ptr, LVCSim_OIDAllocator *alloc, unsigned int oid);

    /// Destroy the object and associated information.
    void destroy(LVCSim_ContainerMeta *cont, LVCSim_SID sid);

    /// Dump the object to checkpoint writer stream.
    void writeToStream(LVCSim_CheckpointWriter &o, size_t objsize) const;


private:
    friend	class LVCSim_liveCacheSink;
    friend	class LVCSim_ObjectHandleObjLock;
    friend	class LVCSim_ObjectTree;
    friend	class LVCSim_ObjectTreeNode;
    friend  class LVCSim_ObjectLockChainItem;

    /// Check if we have the object lock for given transaction.
	  short checkLock(LVCSim_SID sid);
    /// Check if we have the object share locked for given transaction.
	  short checkShareLock(LVCSim_SID sid);
    /// Clear metadata before delete.
	  void clear();
    /// Check, if already cleared.
    bool isCleared() const;

    /// Test for visibility.
    bool isVisibleNoLock(LVCSim_SID sid) const;
    /// Test for visibility for changes.
    short isVisibleForChangeNoLock(LVCSim_SID sid) const;
    /// Test for visibility for shred access.
    short isVisibleForSharedAccessNoLock(LVCSim_SID sid) const;
    /// Garbage collection.
    void GC(LVCSim_TID tid, LVCSim_SID sid,
        RTESync_Spinlock* &spinlock);


	LVCSim_SID				m_lock;		    ///< Sink ID of transaction that holds a exclusive lock on the object.
  LVCSim_SharedLockListItem *m_sharedLockList;  ///< Sink ID of transactions that hold a shared lock on the object.
  LVCSim_ObjectHandle       *m_lockChain;   ///< Next object in lock chain.
	LVCSim_TID				m_tid_max;	    ///< Maximum TX time (to detect deleted).
	LVCSim_TID				m_tid_min;	    ///< Minimum TX time (to detect new).
	LVCSim_TID				m_tid_fmax;	    ///< Maximum TX time of the oldest version (optimization).
  LVCSim_LockChainItem    *m_correspondingLockChainItem;  /// Lock chain item that is stored in the sink, provides faster unlock
    /*!
     * \brief Containing object tree node, if any.
     *
     * This attribute contains a back-reference to the containing node in the tree.
     * It is stored here to provide faster iteration in key range iterators (to
     * prevent descending the tree anew for each increment).
     */
	class LVCSim_ObjectTreeNode	*m_node;	

    /*!
     * \brief Object version chain.
     *
     * Object version chain contains current and before images of the object.
     *
     * \see LVCSim_ObjectVerHandle for information about one object version.
     */
	LVCSim_ObjectVerHandle	*m_versions;
};


#endif
