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

#include "heo670.h"

#include "SAPDB/SQLManager/Catalog/Catalog_CacheImplementation.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationRefObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationDescriptor.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_ContainerDescriptor.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IncrementalBufferAllocator.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"

#include "SAPDB/RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDB/RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "SAPDB/RunTime/System/RTESys_AtomicOperation.hpp"
#include "SAPDB/RunTime/RTE_Crash.hpp"

#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"
#include "SAPDB/KernelCommon/Kernel_OpMsg.hpp"

#include "SAPDB/FrameControl/FrameCtrl_IBlockAllocator.hpp"

//-----------------------------------------------------------------------------

static const bool cAlign       = true;
static const bool cDecRefCount = true;
static const bool cExclusive   = true;
static const bool cWithLock    = true;
static const bool cIsKeyOnly   = true;

#define SPINLOCK_POOL_SIZE 16

#define CATALOG_TRACE(X) SAPDBTRACE_WRITELN( Catalog_Trace, 5, X );
//#define CATALOG_TRACE(X) { Kernel_VTrace() << X; }

static int RWRegionSize = 0;

//-----------------------------------------------------------------------------

/*!
   @brief locks a slot of the catalog cache hash
 */
class Catalog_CacheSlotLockedScope
{
public :
    /*!
       @brief constructor, locks the slot
     */
    Catalog_CacheSlotLockedScope(RTE_TaskId TaskId, RTESync_IRWRegion& Lock, bool ExclusiveLock)
        : m_taskId(TaskId)
        , m_lock(Lock)
        , m_exclusive(ExclusiveLock)
    {
        Lock.enter(ExclusiveLock, TaskId);
    }
    /*!
       @brief destructor, unlocks the slot
     */
    ~Catalog_CacheSlotLockedScope()
    {
        m_lock.leave(m_exclusive, m_taskId);
    }
private :
    RTE_TaskId         m_taskId; 
    RTESync_IRWRegion& m_lock;
    bool               m_exclusive; 
};

//-----------------------------------------------------------------------------

/*!
   @brief defines a handle representing a catalog object stored in the shared catalog cache
 */
class Catalog_Cache::Catalog_CacheHandle : public Catalog_Cache::Catalog_ICacheHandle
{
public :
    /*!
       @brief constructs a handle
     */
    Catalog_CacheHandle(bool WithLock)
        : m_lockIndex(0)
        , m_hashNext(0)
        , m_pObject (0)
        , m_lruPrev (0)
        , m_lruNext (0)
        , m_refCount(1)
        , m_exists(false)
        , m_invalid(false)
        , m_incopy(true)
        , m_timestamp(0)
        , m_keyOnly(true)
    {
        m_lockIndex = Catalog_Cache::Catalog_CacheImplementation::GetSpinlockPool().GetLockIndex(this);
        if (WithLock)
        {
            SAPDB_Byte* p = reinterpret_cast<SAPDB_Byte*>(this) + sizeof(*this);
            SAPDBMem_IncrementalBufferAllocator allocator(p, SizeOf() - sizeof(*this), !cAlign);
            if (!Catalog_Cache::Catalog_CacheImplementation::CreateRWLock(allocator))
            {
                Msg_List msgList; // TODO 
                RTE_Crash(msgList);
            }
        }
    }
    /*!
       @brief destructs a handle
     */
    inline ~Catalog_CacheHandle();
    /*!
       @brief returns the catalog key identified by the handle
     */
 	const Catalog_Object::Catalog_Key& GetKey() const
	{
		return m_pObject->GetCatalogKey();
	}
    /*!
       @brief assigns a pointer to the catalog object to the handle
     */
	void AssignObject (const Catalog_Object* pObj, bool IsKeyOnly)
	{
		m_pObject = pObj;
        m_keyOnly = IsKeyOnly;
	}
    /*!
       @brief decrements to reference counter. This happens when the owner of the handle does not
              longer ask for properties of the corresponding catalog object. However, the owner
              may still reference the object. In this case the owner still holds a read lock for
              the handle.
     */
    inline int DecRefCount();
    /*!
       @brief decrements the timeout value. If 0, the handle may be deleted, if no longer in use
     */
    int DecTimeout()
    {
        --m_timeout;
        return m_timeout;
    }
    /*!
       @brief returns true, if the catalog object identified by the handle has to be copied into the cache
     */
    bool DoIncopy() 
    {
        bool incopy = m_incopy;
        m_incopy    = false;
        return incopy;
    }
    /*!
       @brief returns true, if the catalog object identified by the handle does exist.
     */
    bool ExistsObject() const
    {
        return m_exists;
    }
    /*!
       @brief returns a pointer to the catalog object associated with the handle
     */
	const Catalog_Object* GetObject() const
	{
		return m_pObject;
	}
    /*!
       @brief returns the predecessor of the current handle in the lru list. Caution, the
              current handle must be member of the the lru list, when this method is called
     */
    Catalog_Cache::Catalog_CacheHandle* GetLruPrev() const
    {
        return m_lruPrev;
    }
     /*!
       @brief returns the successor of the current handle in the lru list. Caution, the
              current handle must be member of the the lru list, when this method is called
     */
    Catalog_Cache::Catalog_CacheHandle* GetLruNext() const
    {
        return m_lruNext;
    }
    /*!
       @brief returns the current reference counter, i.e. the number of tasks currently using this handle
     */
    inline int GetRefCount() const;
    /*!
       @brief returns the current timestamp stored in the handle
     */
    SAPDB_UInt4 GetTimestamp() const
    {
        return m_timestamp;
    }
    /*!
       @brief returns the address of the read/write lock of the handle
     */
    RTESync_IRWRegion* GetRwLockPtr() const
    {
        return reinterpret_cast<RTESync_IRWRegion*>(const_cast<SAPDB_Byte*>(reinterpret_cast<const SAPDB_Byte*>(this) + sizeof(*this)));
    }
    /*!
       @brief increments the reference counter. This happens before the user of the handle is going to
              access members of the catalog object associated with the handle
     */
    inline bool IncRefCount();
    /*!
       @brief returns true, if the handle is marked as invalid
     */
    bool IsInvalid() const
    {
        return m_invalid;
    }
    /*!
       @brief locks the handle in exclusive mode
     */
	void LockExclusive(RTE_TaskId TaskId)
	{
        CATALOG_TRACE( ">LockExcl : " << ToStr(GetRwLockPtr()) );
        GetRwLockPtr()->enter(cExclusive, TaskId);
        CATALOG_TRACE( "<LockExcl : " );
    }
    /*!
       @brief locks the handle in share mode
     */
	void LockShare(RTE_TaskId TaskId)
	{
       CATALOG_TRACE (">LockShare : " << ToStr(GetRwLockPtr()) );
       GetRwLockPtr()->enter(!cExclusive, TaskId);
       CATALOG_TRACE ("<LockShare : " );
	}
    /*!
       @brief releases some memory of the catalog objects associated with the handle
     */
    void ReleaseMemory()
    {
        CATALOG_TRACE( "ReleaseMemory(" << ToStr(this) << ")");
        m_invalid = true;
        // TODO free memory
    }
    /*!
       @brief removes the handle from the lru list.
     */
    inline void RemoveFromLruList();
    /*!
       @brief enqueues the handle into a hash list
     */
    void SetHashNext(Catalog_Cache::Catalog_CacheHandle* hashNext)
    {
        m_hashNext = hashNext;
    }
    /*!
       @brief set the lru pointers, used to enqueue the handle into a lru list
     */
    void SetLru (Catalog_Cache::Catalog_CacheHandle* lruPrev, Catalog_Cache::Catalog_CacheHandle* lruNext)
    {
        m_lruPrev = lruPrev;
        m_lruNext = lruNext;
    }
    /*!
       @brief set the lru prev pointer, used to enqueue the handle into a lru list
    */
    void SetLruPrev(Catalog_Cache::Catalog_CacheHandle* lruPrev)
    {
        m_lruPrev = lruPrev;
    }
    /*!
       @brief set the lru next pointer, used to enqueue the handle into a lru list
    */
    void SetLruNext(Catalog_Cache::Catalog_CacheHandle* lruNext)
    {
        m_lruNext = lruNext;
    }
     /*!
       @brief marks the current handle as invalid and removes it from the lru list, if necessary
     */
    void SetInvalid()
    {
        Catalog_Cache::Catalog_CacheImplementation::GetSpinlockPool().Lock(m_lockIndex);
        m_invalid = true;
        RemoveFromLruList();
        Catalog_Cache::Catalog_CacheImplementation::GetSpinlockPool().Unlock(m_lockIndex);
    }
    /*!
       @brief returns the size of an handle
     */
    static int SizeOf()
    {
        return sizeof(Catalog_Cache::Catalog_CacheHandle) + RWRegionSize;
    }
    /*!
       @brief gives up the exclusive lock an requires a share lock
     */
	void SwitchToNonExclusiveAccess (RTE_TaskId TaskId)
	{
        CATALOG_TRACE( ">SwitchToNonExclusiveAccess : " << ToStr(GetRwLockPtr()));
		GetRwLockPtr()->SwitchToNonExclusiveAccess(TaskId);
        CATALOG_TRACE( "<SwitchToNonExclusiveAccess");
	}
    /*!
       @brief tries to obtain an exclusive lock for the handle
     */
    bool TryLockExclusive(RTE_TaskId TaskId)
    {
        bool ok = GetRwLockPtr()->tryEnter(cExclusive, TaskId);
        CATALOG_TRACE( "TryLockExclusive : " << ToStr(GetRwLockPtr()) << " entered " << ok );
        return ok;
    }
    /*!
       @brief releases the exclusive lock
     */
    void UnlockExclusive(RTE_TaskId TaskId)
    {
        CATALOG_TRACE( "UnlockExcl : " << ToStr(GetRwLockPtr()) );
        GetRwLockPtr()->leave(cExclusive, TaskId);
    }
    /*!
       @brief releases the share lock
     */
    void UnlockShare(RTE_TaskId TaskId)
    {
        CATALOG_TRACE( "UnlockShare : " << ToStr(GetRwLockPtr()) );
        GetRwLockPtr()->leave(!cExclusive, TaskId);
    }
    /*!
       @brief returns the next handle of the hash list
    */
    Catalog_Cache::Catalog_CacheHandle* Next() const
    {
        return m_hashNext;
    }
    /*!
       @brief sets the exists property, i.e. marks the objects as existent
     */
    void SetExists(bool Exists)
    {
        m_exists = Exists;
    }
    /*!
       @brief new operator, returns memory from the catalog cache allocator
     */
    static void* operator new (size_t Size, SAPDBMem_IRawAllocator& Allocator)
    {
        return Allocator.Allocate( SizeOf() );
    }
    //!< methods from Catalog_ICacheHandle
    virtual const Catalog_Object* GetObjectPointer() const
    {
        return m_pObject;
    }
    virtual void Release(SQLMan_Context& Context)
    {
        Catalog_Cache::Release (Context,  reinterpret_cast<Catalog_Cache::Catalog_CacheHandle*>(this), cDecRefCount);
    }
    friend class Catalog_Cache::Catalog_CacheImplementation;
private :
    SAPDB_Int2                          m_lockIndex;
    SAPDB_UInt2                         m_timeout;
    SAPDB_UInt4                         m_timestamp;
    Catalog_Cache::Catalog_CacheHandle* m_hashNext;
    Catalog_Cache::Catalog_CacheHandle* m_lruPrev;
    Catalog_Cache::Catalog_CacheHandle* m_lruNext;
	SAPDB_UInt4                         m_refCount;
    bool                                m_exists;
    bool                                m_invalid;
    bool                                m_incopy;
    bool                                m_keyOnly; //!< m_pObject is a pointer to a key, not to an Catalog_Object
  	const Catalog_Object*               m_pObject;
};

//-----------------------------------------------------------------------------

/*!
   @brief implement a lru list for Catalog_CacheHandles
 */
class Catalog_CacheLRUList
{
public :
    /*!
       @brief constructor, creates an emty lru list
     */
    Catalog_CacheLRUList()
        : m_entryCount(0)
        , m_head(!cWithLock)
    {
        m_head.SetLruNext(&m_head);
        m_head.SetLruPrev(&m_head);
    }
    /*!
       @brief enters a handle into the lru list 
     */
    void Enter(Catalog_Cache::Catalog_CacheHandle* pHandle)
    {
         SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheLRUList::Catalog_Key::Enter", Catalog_Trace, 1);  
         CATALOG_TRACE( "Enter " << ToStr(pHandle));
         pHandle->SetLru(&m_head, m_head.GetLruNext());
         m_head.GetLruNext()->SetLruPrev(pHandle);
         m_head.SetLruNext(pHandle);
         ++m_entryCount;
    }
    /*!
      @brief returns the number of handles stored in the lru list
     */
    SAPDB_UInt4 GetEntryCount()
    {
        return m_entryCount;
    }
    /*!
       @brief retuns the last handle of the lru list, 0 if list is empty
     */
    Catalog_Cache::Catalog_CacheHandle* Last()
    {
        Catalog_Cache::Catalog_CacheHandle* last = m_head.GetLruPrev();
        return (last == &m_head) ? 0 : last;
    }
    /*!
       @brief returns the previous handle from the given handle
     */
    Catalog_Cache::Catalog_CacheHandle* Prev(Catalog_Cache::Catalog_CacheHandle* pCurr) 
    {
        if (0 == pCurr)
        {
            return Last();
        }
        Catalog_Cache::Catalog_CacheHandle* pPrev = pCurr->GetLruPrev();
        return (pPrev == &m_head) ? 0 : pPrev;
    }
    /*!
       @brief removes the given handle from the lru list
     */
    void Remove(Catalog_Cache::Catalog_CacheHandle* pHandle)
    {
        SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheLRUList::Catalog_Key::Remove", Catalog_Trace, 1);  
        CATALOG_TRACE( "Remove " << ToStr(pHandle));
        pHandle->GetLruPrev()->SetLruNext(pHandle->GetLruNext());
        pHandle->GetLruNext()->SetLruPrev(pHandle->GetLruPrev());
        pHandle->SetLruPrev(0);
        pHandle->SetLruNext(0);
        --m_entryCount;
    }
    /*!
       @brief releases some handle from the tail of the list 
     */
    void RemoveTail(Catalog_Cache::Catalog_CacheHandle* pHandle, int RemoveEntryCount)
    {
        Catalog_Cache::Catalog_CacheHandle* pPrev = pHandle->GetLruPrev();
        pPrev->SetLruNext(&m_head);
        m_head.SetLruPrev(pPrev);
        m_entryCount -= RemoveEntryCount;
    }
private :
    SAPDB_UInt4                        m_entryCount;
    Catalog_Cache::Catalog_CacheHandle m_head;
};

/*!
   @brief locks a lru list
 */
class Catalog_LockedLruListScope 
{
public :
    /*!
       @brief contructor, locks the lru list identified by idx
     */
    Catalog_LockedLruListScope (int idx)
        : m_idx(idx)
    {
        Catalog_Cache::Catalog_CacheImplementation::GetSpinlockPool().Lock(m_idx);
    }
    /*!
       @brief destructor, unlocks the lru list
     */
    ~Catalog_LockedLruListScope()
    {
        Catalog_Cache::Catalog_CacheImplementation::GetSpinlockPool().Unlock(m_idx); 
    }
private :
    int m_idx;
};

//-----------------------------------------------------------------------------

inline void Catalog_Cache::Catalog_CacheHandle::RemoveFromLruList()
{
    if (m_lruNext)
    {
        Catalog_Cache::Catalog_CacheImplementation::GetLruList(m_lockIndex).Remove(this);
    }
}

//-----------------------------------------------------------------------------

inline Catalog_Cache::Catalog_CacheHandle::~Catalog_CacheHandle()
{
    CATALOG_TRACE( "~Catalog_CacheHandle : " << ToStr(this) );
    if (m_keyOnly)
    {
        Catalog_Cache::Catalog_CacheImplementation::GetAllocator().Deallocate(const_cast<Catalog_Object*>(m_pObject));
    }
    else
    {
        const_cast<Catalog_Object*>(m_pObject)->Destroy(Catalog_Cache::Catalog_CacheImplementation::GetAllocator());
    }
    SAPDB_Byte* p = reinterpret_cast<SAPDB_Byte*>(this) + sizeof(*this);
    SAPDBMem_IncrementalBufferAllocator allocator(p, SizeOf() - sizeof(*this), !cAlign);
    RTESync_DestroyRWRegion(GetRwLockPtr(), allocator);
}

//-----------------------------------------------------------------------------


inline int Catalog_Cache::Catalog_CacheHandle::DecRefCount()
{
    Catalog_LockedLruListScope lockedScope(m_lockIndex);
    CATALOG_TRACE( "DecRefCount(" << ToStr(this) << ") : " << m_refCount);
    --m_refCount;
    if (0 == m_refCount && !m_invalid)
    {
        m_timestamp = Catalog_Cache::Catalog_CacheImplementation::GetTimeStamp();
        Catalog_Cache::Catalog_CacheImplementation::IncEnterLru();
        m_timeout = 12 * 2; // 1 Minute
        Catalog_Cache::Catalog_CacheImplementation::GetLruList(m_lockIndex).Enter(this);
    }
    return m_refCount;
}

//-----------------------------------------------------------------------------

 inline int Catalog_Cache::Catalog_CacheHandle::GetRefCount() const
 {
     Catalog_LockedLruListScope lockedScope(m_lockIndex);
     return m_refCount;
 }

 //-----------------------------------------------------------------------------

inline bool Catalog_Cache::Catalog_CacheHandle::IncRefCount()
{
    Catalog_LockedLruListScope lockedScope(m_lockIndex);
    CATALOG_TRACE( "IncRefCount(" << ToStr(this) << ") : " << m_refCount);
    if (m_invalid)
    {
        // this handle is no longer valid
        return false;
    }
    if (0 == m_refCount)
    {
        // caller is first one currently using this handle, i.e. handle must not be removed in case of a memory bottleneck
        Catalog_Cache::Catalog_CacheImplementation::IncRemoveLru();
        Catalog_Cache::Catalog_CacheImplementation::GetLruList(m_lockIndex).Remove(this);
    }
    ++m_refCount;
    return true;
}

//-----------------------------------------------------------------------------

Catalog_Cache::Catalog_CacheImplementation::HeadEntry:: HeadEntry()
:  m_pEntry(0)
,  m_pLock(0)
{
}

//-----------------------------------------------------------------------------

Catalog_Cache::Catalog_CacheImplementation* Catalog_Cache::Catalog_CacheImplementation::m_pInstance = 0;

Catalog_Cache::Catalog_CacheImplementation::Catalog_CacheImplementation()
: SAPDBMem_RawAllocator( (SAPDB_UTF8*) "CatalogCache", 
              FrameCtrl_IBlockAllocator::GetInstance(),
              FrameCtrl_IBlockAllocator::GetInstance().GetBlockSize(), 
              FrameCtrl_IBlockAllocator::GetInstance().GetBlockSize(), 
              SAPDBMem_RawAllocator::FREE_RAW_EXTENDS_EXCEPT_INITIAL)
              , m_head(RTEMem_Allocator::Instance())
              , m_tableDescriptor(RTEMem_Allocator::Instance())
              , m_lockId(0)
              , m_maxSize(0)
              , m_opMessages(false)
              , m_headEntryCount(2017)
              , m_statistics()
              , m_lruClock(0)
              , m_lruList(0)
              , m_spinlockPool ((SAPDB_UTF8 *) "Catalog_CachePool", SPINLOCK_POOL_SIZE, RTEMem_Allocator::Instance())
              , m_allocatorLock(0)
              , m_invalidateLock(0)
{
    DisableOpMessages();
    AssignToTask(RTE_UnknownTaskId); 
    if (!m_tableDescriptor.Reserve(10))
    {
        RTE_Crash(Msg_List(Msg_List::Error,	SDBMSG_CATALOG_NO_MEMORY));
    }
	if (m_head.Initialize (m_headEntryCount))
	{
        m_pInstance      = this;
        m_allocatorLock  = RTESync_CreateRWRegion(++m_lockId, m_spinlockPool, RTEMem_Allocator::Instance());
        m_invalidateLock = RTESync_CreateRWRegion(++m_lockId, m_spinlockPool, RTEMem_Allocator::Instance());
        if (m_allocatorLock && m_invalidateLock)
        {
            int ix = 0;
            RWRegionSize = RTESync_IRWRegion::sizeOf();
            while (true)
            {
                m_head[ix].m_pEntry = NULL;
                m_head[ix].m_pLock  = RTESync_CreateRWRegion(++m_lockId, m_spinlockPool, *this);
                if (0 == m_head[ix].m_pLock)
                {
                    break;
                }
                ++ix;
                if (ix == m_headEntryCount)
                {
                    m_lruList = newarray(m_lruList, SPINLOCK_POOL_SIZE, RTEMem_Allocator::Instance());
                    if (m_lruList)
                    {
#ifdef SAPDB_SLOW
                        this->SetFullChecking();
#endif
                        m_maxSize = this->ComputeAndSetMaxAllocatableSize();
#ifdef SAPDB_SLOW
                        void* p = this->Allocate(m_maxSize);
                        SAPDBERR_ASSERT_STATE( 0 != p );
                        this->Deallocate(p); 
#endif
                        m_opMessages = true;
                        m_invalidateList = new(RTEMem_Allocator::Instance()) Catalog_CacheLRUList;
                        if (m_invalidateList)
                        {
                            return;
                        }
                    }
                }
            }
        }
	}
    RTE_Crash(Msg_List(Msg_List::Error,	SDBMSG_CATALOG_NO_MEMORY));
}

//-----------------------------------------------------------------------------

const Catalog_Cache::Catalog_ICacheHandle* Catalog_Cache::Catalog_CacheImplementation::Access 
(
 SQLMan_Context&                    Context, 
 const Catalog_Object::Catalog_Key& Key,
 bool                               NoWait,
 const Catalog_Object*            & pObj,
 bool                             & ObjectExists
 )
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::Access", Catalog_Trace, 1);
	const bool cExclusive = true;

    int hashIndex = this->Hash(Key);
	RTE_TaskId taskId = Context.GetTaskId();
    // search required information
    Catalog_Cache::Catalog_CacheHandle* pCurr;
    {
        Catalog_CacheSlotLockedScope lockedScope (taskId, *m_head[hashIndex].m_pLock, !cExclusive); 
        Catalog_Cache::Catalog_CacheHandle** ppPrev;
        pCurr = this->Find(Key, hashIndex, ppPrev);
        if (0 != pCurr)
        {
            if (!pCurr->IncRefCount())
            {
                Context.ThrowError (e_old_fileversion);
                return 0;
            }
        }
    }
	if (0 == pCurr)
	{
        // information not found in cache, read it from catalog tables
		pCurr = this->InsertEntry (hashIndex, Context, Key);
        if (pCurr)
        {
            if ( pCurr->DoIncopy())
            {
                ++m_statistics.m_missCount;
                pObj = Catalog_Object::Incopy (Context, Key, *this, NoWait);
                if (!Context.IsOk())
                {
                    // an error occured
                    pCurr->SetInvalid();
                    pCurr->UnlockExclusive(taskId);
                    Invalidate (taskId, Key, pCurr);
                    return 0;
                }
                if (pObj)
                {
                    this->Deallocate (const_cast<Catalog_Object*>(pCurr->GetObject()));
                    pCurr->AssignObject(pObj, !cIsKeyOnly);
                    pCurr->SetExists(true);
                }
                //pCurr->UnlockExclusive(taskId);
                //pCurr->LockShare(taskId);
                pCurr->SwitchToNonExclusiveAccess(taskId);
            }
            else
            {
                ++m_statistics.m_hitCount;
            }
        }
        else
        {
            return 0;
        }
	}
	else
	{
        pCurr->LockShare(taskId);
        if (pCurr->IsInvalid())
        {
            Context.ThrowError (e_old_fileversion);
            pCurr->UnlockShare(taskId);
        }
        else
        {
            ++m_statistics.m_hitCount;
        }
	}
	pObj          = pCurr->GetObject();
    ObjectExists  = pCurr->ExistsObject();
    CATALOG_TRACE( "Access : handle " << ToStr(pCurr) << ", pObj " << ToStr(pObj) );    
    return pCurr;
}

//-----------------------------------------------------------------------------

void* Catalog_Cache::Catalog_CacheImplementation::Allocate(SAPDB_ULong ByteCount)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::Allocate", Catalog_Trace, 1);
    m_allocatorLock->enter(cExclusive);
    void* p = SAPDBMem_RawAllocator::Allocate(ByteCount);
    m_allocatorLock->leave(cExclusive);
    if (0 == p)
    {
        Kernel_OpInfo opInfo ("CatCache");
        if (ByteCount > m_maxSize)
        {
            opInfo << "too big chunk required, required " << ByteCount << ", max " << m_maxSize << FlushLine;
            eo670_CTraceStack();
            return 0;
        }
        while (0 == p)
        {
            if (ReleasedMemory())
            {
                ++m_statistics.m_releasedMemoryCount;
                m_allocatorLock->enter(cExclusive);
                p = SAPDBMem_RawAllocator::Allocate(ByteCount);
                m_allocatorLock->leave(cExclusive);
            }
            else
            {
                break;
            }
        }
        if ((0 == p) && m_opMessages)
        {
            THeapInfo info;
            GetInfo (info);
            opInfo << "bad alloc, required " << ByteCount << " Bytes" << FlushLine;;
            opInfo << "allocated " << info.used << " Bytes" << FlushLine;;
            opInfo << "reseverd  " << info.allocated << " Bytes" << FlushLine;;
        }
    }
    return p;
}

//-----------------------------------------------------------------------------

tgg00_BasisError Catalog_Cache::Catalog_CacheImplementation::CallDeleteTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage)
{
    return m_tableDescriptor[FuncIdx]->DeleteTrigger(TaskId, BeforeImage);
}

//-----------------------------------------------------------------------------
    
void Catalog_Cache::Catalog_CacheImplementation::CallInsertTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& AfterImage)
{
    m_tableDescriptor[FuncIdx]->InsertTrigger(TaskId, AfterImage);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::CallUpdateTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage, const tgg00_Rec& AfterImage)
{
    m_tableDescriptor[FuncIdx]->UpdateTrigger(TaskId, BeforeImage, AfterImage);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::ClearInvalidHandleList(RTE_TaskId TaskId)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::ClearInvalidHandleList", Catalog_Trace, 1);
    Catalog_Cache::Catalog_CacheHandle* pInvalidHandle = 0;
    m_invalidateLock->enter(cExclusive);
    pInvalidHandle = m_invalidateList->Prev(pInvalidHandle);
    while (pInvalidHandle)
    {
        CATALOG_TRACE ("invalid handle " << ToStr(pInvalidHandle));
        Catalog_Cache::Catalog_CacheHandle* pCurrHandle = pInvalidHandle;
        pInvalidHandle = m_invalidateList->Prev(pInvalidHandle);
        if (0 != pCurrHandle->GetRefCount())
        {
            continue;
        }
        bool locked = pCurrHandle->TryLockExclusive(TaskId);
        if (locked)
        {
            m_invalidateList->Remove(pCurrHandle); 
            destroy (pCurrHandle, *this);
        }
    }
    m_invalidateLock->leave(cExclusive);
}

//-----------------------------------------------------------------------------

RTESync_IRWRegion* Catalog_Cache::Catalog_CacheImplementation::CreateRWLock(SAPDBMem_IRawAllocator& Allocator)
{
    return RTESync_CreateRWRegion(++m_pInstance->m_lockId, m_pInstance->m_spinlockPool, Allocator);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::Deallocate(void* p)
{
    m_allocatorLock->enter(cExclusive);
    SAPDBMem_RawAllocator::Deallocate(p);
    m_allocatorLock->leave(cExclusive);
}

//-----------------------------------------------------------------------------


inline void Catalog_Cache::Catalog_CacheImplementation::DeleteRWLock(RTESync_IRWRegion* pLock)
{
    RTESync_DestroyRWRegion(pLock, *m_pInstance);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::EnterInvalidHandle (Catalog_CacheHandle* pHandle)
{
    m_invalidateLock->enter(cExclusive);
    CATALOG_TRACE("EnterInvalidHandle " << ToStr(pHandle))
    m_invalidateList->Enter(pHandle); 
    m_invalidateLock->leave(cExclusive);
}

//-----------------------------------------------------------------------------

inline  Catalog_Cache::Catalog_CacheHandle* Catalog_Cache::Catalog_CacheImplementation::Find 
(
 const Catalog_Object::Catalog_Key    &Key,
 int                                   HashIndex,
 Catalog_Cache::Catalog_CacheHandle** &ppPrev,
 Catalog_CacheHandle*                  pRequiredHandle
 )
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::Find", Catalog_Trace, 1);
    ppPrev = &m_head[HashIndex].m_pEntry;
    Catalog_Cache::Catalog_CacheHandle* pCurr = m_head[HashIndex].m_pEntry;
    while (pCurr)
    {
        if (pCurr->GetKey() == Key)
        {
            if (pRequiredHandle == pCurr)
            {
                // required handle found
                break;
            }
            else
            {
                if (!pRequiredHandle && !pCurr->IsInvalid())
                {
                    // skip invalid handles
                    break;
                }
            }
        }
        ppPrev = &pCurr->m_hashNext; 
        pCurr  = pCurr->Next();
    }
    return pCurr;
}

//-----------------------------------------------------------------------------

SAPDBMem_RawAllocator& Catalog_Cache::Catalog_CacheImplementation::GetAllocator()
{
    return *m_pInstance;
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::GetAllStatistics (Cache_Statistics& Statistics) const
{
    Statistics = m_statistics;
    Statistics.m_lruEntryCount = 0;
    for (int ix = 0; ix < SPINLOCK_POOL_SIZE; ++ix)
    {
       Statistics.m_lruEntryCount += m_lruList[ix].GetEntryCount();
    }
    Statistics.m_invalidHandleCount = m_invalidateList->GetEntryCount();
}

//-----------------------------------------------------------------------------

inline Catalog_CacheLRUList& Catalog_Cache::Catalog_CacheImplementation::GetLruList(int idx) 
{
    return m_pInstance->m_lruList[idx];
}

//-----------------------------------------------------------------------------

inline RTESync_SpinlockPool& Catalog_Cache::Catalog_CacheImplementation::GetSpinlockPool()
{
    return m_pInstance->m_spinlockPool;
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::GetStatistics(bool GetResetStatistics, SAPDB_UInt8& HitCount, SAPDB_UInt8& MissCount)
{
    if (GetResetStatistics)
    {
        HitCount  = m_statistics.m_hitResetCount;
        MissCount = m_statistics.m_missResetCount;
    }
    else
    {
        HitCount  = m_statistics.m_hitCount  + m_statistics.m_hitResetCount;
        MissCount = m_statistics.m_missCount + m_statistics.m_missResetCount;
    }
}

//-----------------------------------------------------------------------------

inline SAPDB_UInt4 Catalog_Cache::Catalog_CacheImplementation::GetTimeStamp()
{
    // unsynchronized, it does not matter that result is inaccurate
    ++m_pInstance->m_lruClock;
    return m_pInstance->m_lruClock;
}

//-----------------------------------------------------------------------------

int Catalog_Cache::Catalog_CacheImplementation::GetTriggerId(const SQLMan_Identifier& TableName)
{
    Container_Vector<Catalog_CatalogTableDescriptor*>::ConstIterator end = m_tableDescriptor.End();
    int funcIdx = 1;
    for (Container_Vector<Catalog_CatalogTableDescriptor*>::ConstIterator iter = m_tableDescriptor.Begin();
        iter != end; ++iter)
    {
        if ((*iter)->IsEqualTablename(TableName))
        {
            return funcIdx;
        }
        ++funcIdx;
    }
    return 0;
}

//-----------------------------------------------------------------------------

Catalog_Cache::Catalog_CacheHandle* Catalog_Cache::Catalog_CacheImplementation::InsertEntry (int HashIndex, SQLMan_Context& Context, const Catalog_Object::Catalog_Key& Key)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::InsertEntry", Catalog_Trace, 1);
	RTE_TaskId taskId = Context.GetTaskId();
    // memory allocation must be done outside locked scope. Otherwise we collide
    // on spinlocks in case of a memory bottleneck
    SAPDB_AutoPtr<Catalog_Cache::Catalog_CacheHandle> pNewEntry(this,  new(*this) Catalog_Cache::Catalog_CacheHandle(cWithLock));
    if (0 == pNewEntry.GetPtr())
    {
        Context.SetNoMoreMemoryError(Msg_List(Msg_List::Error, SDBMSG_CATALOG_NO_MEMORY, Msg_OptArg("MEMREQUEST", Catalog_Cache::Catalog_CacheHandle::SizeOf())));
        return 0;
    }
    Catalog_AuthorizationObject* pObj = reinterpret_cast<Catalog_AuthorizationObject*> (Key.CreateCopy(*this));
    if (!pObj)
    {
        Context.SetNoMoreMemoryError(Msg_List(Msg_List::Error, SDBMSG_CATALOG_NO_MEMORY, Msg_OptArg("MEMREQUEST", Key.Length())));
        return 0;
    }
	pNewEntry->AssignObject(pObj, cIsKeyOnly);
    // lock hash slot exclusive and try to find required key again. The key now may be present
    // if another task has been faster
    m_head[HashIndex].m_pLock->enter(cExclusive, taskId);
    Catalog_Cache::Catalog_CacheHandle** ppPrev;
	Catalog_Cache::Catalog_CacheHandle* pCurr = this->Find (Key, HashIndex, ppPrev);
	if (0 == pCurr)
	{
        // required key still not found in hash, insert the new handle into hash
        pNewEntry->LockExclusive(taskId);
        pNewEntry->SetHashNext(m_head[HashIndex].m_pEntry);
		m_head[HashIndex].m_pEntry = pNewEntry.GetPtr();
        ++m_statistics.m_entryCount;
        if (m_statistics.m_entryCount > m_statistics.m_maxEntryCount)
        {
            m_statistics.m_maxEntryCount = m_statistics.m_entryCount;
        }
		m_head[HashIndex].m_pLock->leave(cExclusive, taskId);
        pCurr = pNewEntry.Release();
	}
	else
	{
        // someone else has been faster
		bool incremented = pCurr->IncRefCount();
 		m_head[HashIndex].m_pLock->leave(cExclusive, taskId);
        if (!incremented)
        {
            Context.ThrowError (e_old_fileversion);
            return 0;
        }
        pCurr->LockShare(taskId);
        if (pCurr->IsInvalid())
        {
            Release (Context, pCurr, cDecRefCount);
            Context.ThrowError (e_old_fileversion);
            return 0;
        }
	}
    return pCurr;
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::Invalidate (RTE_TaskId                         TaskId,
                                                             const Catalog_Object::Catalog_Key& Key, 
                                                             Catalog_CacheHandle*               pRequiredHandle)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::Invalidate", Catalog_Trace, 1);
    int hashIndex = this->Hash(Key);
    Catalog_Cache::Catalog_CacheHandle* pCurr;
    {
        // search required information
        Catalog_CacheSlotLockedScope(TaskId, *m_head[hashIndex].m_pLock, cExclusive); 
        Catalog_Cache::Catalog_CacheHandle** ppPrev;
        pCurr = this->Find (Key, hashIndex, ppPrev, pRequiredHandle);
        if (0 != pCurr)
        {
            // remove from hash
            CATALOG_TRACE( "Invalidate(" << ToStr(pCurr) << ")");
            if ( Catalog_Trace.TracesLevel(5) )
            {
                Kernel_VTrace() << "invalidated " << SAPDBTrace_Hex (&Key.all(), Key.Length());
            }
            --m_statistics.m_entryCount;
            *ppPrev = pCurr->Next();
            ++m_statistics.m_invalidateCount;
            pCurr->SetInvalid();
            EnterInvalidHandle(pCurr);
        }
        else
        {
            if ( Catalog_Trace.TracesLevel(5) )
            {
                Kernel_VTrace() << "invalidate : not found " << SAPDBTrace_Hex (&Key.all(), Key.Length());
            }
        }
    }
    if (0 != pCurr)
    {
        SQLMan_Context* pContext = SQLMan_Context::GetContext();
        if (pContext)
        {
            // the task invalidating the entry must not use the shared cache beginning from now, because
            // otherwise information produced by this task may be read into the shared cached. If this
            // action is rolled back afterwards, the cache contains garbage
            pContext->SetLocalCacheUsageOnly();
        }
        int refCount = pCurr->GetRefCount(); 
        // it may happen that m_refCount is not 0, because in method Access the refCount is incremented
        // before share lock is required
        if (0 == refCount)
        {
            pCurr->ReleaseMemory();
        }
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::InvalidateAuthorization (RTE_TaskId TaskId, const SQLMan_Surrogate* pAuthId, const SQLMan_Identifier* pName)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::InvalidateAuthorization", Catalog_Trace, 1);
    if (pAuthId)
    {
        Catalog_AuthorizationObject::Catalog_Key key(*pAuthId);
        this->Invalidate(TaskId, key, 0);
    }
    if (pName)
    {
        Catalog_AuthorizationRefObject::Catalog_Key refKey(*pName);
        this->Invalidate(TaskId, refKey, 0);
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::InvalidateTable (RTE_TaskId TaskId, const SQLMan_Surrogate& TableId)
{
    SAPDBTRACE_WRITELN( Catalog_Trace, 5, "Catalog_CacheImplementation::InvalidateTable ");
    Catalog_TableObject::Catalog_Key key(TableId);
    this->Invalidate(TaskId, key, 0);
}

//-----------------------------------------------------------------------------

inline void Catalog_Cache::Catalog_CacheImplementation::IncEnterLru ()
{
   ++m_pInstance->m_statistics.m_enterLruCount;
}

//-----------------------------------------------------------------------------

inline void Catalog_Cache::Catalog_CacheImplementation::IncRemoveLru ()
{
   ++m_pInstance->m_statistics.m_removeLruCount;
}

//-----------------------------------------------------------------------------

bool Catalog_Cache::Catalog_CacheImplementation::HandleInvalid(Catalog_CacheHandle* pHandle)
{
    return pHandle->IsInvalid(); 
}

//-----------------------------------------------------------------------------

SAPDB_UInt4 Catalog_Cache::Catalog_CacheImplementation::Hash(const Catalog_Object::Catalog_Key& Key)
{
	return Key.HashValue() % m_headEntryCount;
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::RegisterCatalogTables()
{
    m_tableDescriptor.InsertEnd(new( *this ) Catalog_AuthorizationDescriptor (*SQLMan_Context::GetContext()));
    m_tableDescriptor.InsertEnd(new( *this ) Catalog_ContainerDescriptor (*SQLMan_Context::GetContext()));
    Container_Vector<Catalog_CatalogTableDescriptor*>::ConstIterator end = m_tableDescriptor.End();
    int funcIdx = 1;
    for (Container_Vector<Catalog_CatalogTableDescriptor*>::ConstIterator iter = m_tableDescriptor.Begin();
        iter != end; ++iter)
    {
        if (0 == (*iter))
        {
            Msg_List msgList; 
            RTE_Crash(msgList);
        }
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::Release (SQLMan_Context&                     Context, 
                                                          Catalog_Cache::Catalog_CacheHandle* pHandle,
                                                          bool                                DecRefCount)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::Release", Catalog_Trace, 1);
    if (DecRefCount)
    {
        pHandle->DecRefCount();
    }
    pHandle->UnlockShare(Context.GetTaskId());
}

//-----------------------------------------------------------------------------

inline SAPDB_UInt4 Catalog_Cache::Catalog_CacheImplementation::TimeDiff (SAPDB_UInt4 T1, SAPDB_UInt4 T2)
{
    return (T1 > T2) ? T1 - T2 : T2 - T1;
}

//-----------------------------------------------------------------------------

bool Catalog_Cache::Catalog_CacheImplementation::ReleasedMemory()
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::ReleasedMemory", Catalog_Trace, 1);
    SAPDB_UInt4                 maxTimeDiff = 0;
    SAPDB_UInt4                 oldestTimestamp;
    int                         oldestIdx;
    int                         ix;
    Catalog_Object::Catalog_Key key;
    SAPDB_UInt4                 currClock   = m_lruClock;
    // find oldest entry from lru lists. Caution : this is done without any synchronisation !!
    // the result therefore may be unaccurate
    for (ix = 0; ix < SPINLOCK_POOL_SIZE; ++ix)
    {

        Catalog_CacheHandle* pLast = m_lruList[ix].Last();
        if (pLast)
        {
            SAPDB_UInt4 currTimestamp = pLast->GetTimestamp();
            if (TimeDiff(currTimestamp, currClock) > maxTimeDiff)
            {
                maxTimeDiff     = TimeDiff(currTimestamp, currClock);
                oldestIdx       = ix;
                oldestTimestamp = currTimestamp;
            }
        }
    }
    if (maxTimeDiff > 0)
    {
        m_spinlockPool.Lock(oldestIdx);
        Catalog_Cache::Catalog_CacheHandle* pHandle = m_lruList[oldestIdx].Last();
        if (pHandle && (pHandle->GetTimestamp() == oldestTimestamp))
        {
            // verify that found entry is still the same
            m_lruList[oldestIdx].Remove(pHandle);
            pHandle->ReleaseMemory();
            Catalog_Object::Catalog_Key key = pHandle->GetObject()->GetCatalogKey();
            m_spinlockPool.Unlock(oldestIdx);
            this->Invalidate (SQLMan_Context::GetContext()->GetTaskId(), key, pHandle);
            return true;
        }
        m_spinlockPool.Unlock(oldestIdx);
    }
    for (ix = 0; ix < SPINLOCK_POOL_SIZE; ++ix)
    {
        m_spinlockPool.Lock(ix);
    }
    oldestIdx = -1;
    for (ix = 0; ix < SPINLOCK_POOL_SIZE; ++ix)
    {
        Catalog_CacheHandle* pLast = m_lruList[ix].Last();
        if (pLast)
        {
            SAPDB_UInt4 currTimestamp = pLast->GetTimestamp();
            if (TimeDiff(currTimestamp, currClock) > maxTimeDiff)
            {
                maxTimeDiff     = TimeDiff(currTimestamp, currClock);
                oldestIdx       = ix;
            }
        }
    }
    Catalog_Cache::Catalog_CacheHandle* pHandle;
    if (oldestIdx >= 0)
    {
        pHandle = m_lruList[oldestIdx].Last();
        pHandle->ReleaseMemory();
        key = pHandle->GetObject()->GetCatalogKey();
    }
    for (ix = 0; ix < SPINLOCK_POOL_SIZE; ++ix)
    {
        m_spinlockPool.Unlock(ix);
    }
    if (oldestIdx >= 0)
    {
        this->Invalidate (SQLMan_Context::GetContext()->GetTaskId(), key, pHandle);
        ClearInvalidHandleList(SQLMan_Context::GetContext()->GetTaskId());
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::ResetStatistics()
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::ResetStatistics", Catalog_Trace, 1);
    m_statistics.Reset();
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::SetHandleInUse(SQLMan_Context& Context, Catalog_CacheHandle* pHandle, bool InUse)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::SetHandleInUse", Catalog_Trace, 1);
    CATALOG_TRACE("pHandle = " << ToStr(pHandle) << "InUse = " << InUse)
    if (InUse)
    {
        if (!pHandle->IncRefCount())
        {
            Context.ThrowError (e_old_fileversion);
        }
    }
    else
    {
        pHandle->DecRefCount();
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Catalog_CacheImplementation::Shrink(RTE_TaskId TaskId)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_CacheImplementation::Shrink", Catalog_Trace, 1);
    for (int ix = 0; ix < SPINLOCK_POOL_SIZE; ++ix)
    {
        m_spinlockPool.Lock(ix);
        Catalog_Cache::Catalog_CacheHandle* pHandle = 0;
        Catalog_Cache::Catalog_CacheHandle* pLast = 0;
        int releaseCnt = 0;
        while (pHandle = m_lruList[ix].Prev(pHandle))
        {
            if (0 == pHandle->DecTimeout())
            {
                ++releaseCnt;
                pHandle->ReleaseMemory();
                pLast = pHandle;
            }
        }
        if (releaseCnt > 0)
        {
            m_lruList[ix].RemoveTail(pLast, releaseCnt);
        }
        m_spinlockPool.Unlock(ix);
        Catalog_Cache::Catalog_CacheHandle* pCurr = pLast;
        for (int jx = 0; jx < releaseCnt; ++jx)
        {
            pHandle = pCurr;
            pCurr   = pCurr->GetLruNext();
            Catalog_Object::Catalog_Key key = pHandle->GetObject()->GetCatalogKey();
            pHandle->SetLruNext(0);
            this->Invalidate (TaskId, key, pHandle);
        }
    }
    ClearInvalidHandleList(TaskId);
}

