/*!
  @file           Catalog_CacheImplementation.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          implementation of the shared catalog cache

\if EMIT_LICENCE

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



\endif
*/

#ifndef CATALOG_CACHEIMPLEMENTATION_HPP
#define CATALOG_CACHEIMPLEMENTATION_HPP

#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "Container/Container_Vector.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPool.hpp"

class RTESync_IRWRegion;
class Catalog_CacheLRUList;
class Catalog_CatalogTableDescriptor;
class Catalog_LockedLruListScope;

/*!
   @brief implementation of the shared catalog cache
 */
class Catalog_Cache::Catalog_CacheImplementation : public SAPDBMem_RawAllocator
{
private :
    /*!
       @brief definition of a hash slot  
     */
	struct HeadEntry
	{
       HeadEntry(); //!< default constructor;
       Catalog_CacheHandle* m_pEntry; //!< first entry in slot
	   RTESync_IRWRegion*   m_pLock;  //!< synchronization object for slot
	};
    
    /*!
       @brief constructor
     */
	Catalog_CacheImplementation();
    /*!
       @brief accesses a catalog object identified by its key. If necessary the object is read
              from the corresponding catalog tables
     */
	const Catalog_Cache::Catalog_ICacheHandle* Access (SQLMan_Context& Context, 
        const Catalog_Object::Catalog_Key  &Key, 
        bool                                NoWait,
        const Catalog_Object*              &pObj,
        bool                               &ObjectExists);
    /*!
       @brief the cache atcs as an allocator. This is the allocate method
     */
    virtual void* Allocate(SAPDB_ULong ByteCount);
    /*!
       @brief method called when any catalog row has been deleted
     */
    tgg00_BasisError CallDeleteTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage);
    /*!
       @brief method called when any catalog row has been inserted
     */
    void CallInsertTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& AfterImage);
    /*!
       @brief method called when any catalog row has been updated
     */
    void CallUpdateTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage, const tgg00_Rec& AfterImage);
    /*!
       @brief tries to delete all invalid handle
     */
    void ClearInvalidHandleList(RTE_TaskId TaskId);
    /*!
        @brief creates a reader/writer lock using the given allocator
     */
    static RTESync_IRWRegion* CreateRWLock(SAPDBMem_IRawAllocator& Allocator);
    /*!
       @brief deallocates memory
    */
     virtual void Deallocate(void* p);
     /*!
      @brief deletes a reader/writer lock 
     */
    inline static void DeleteRWLock(RTESync_IRWRegion* pLock);
    /*!
      @brief inserts handle into list of invalidated handles
     */
    void EnterInvalidHandle (Catalog_CacheHandle*);
    /*!
      @brief searches for a key in the shared catalog hash table
     */
    inline Catalog_CacheHandle* Find (
        const Catalog_Object::Catalog_Key& Key, 
        int                                HashIndex, 
        Catalog_CacheHandle**             &ppPrev,
        Catalog_CacheHandle*               pRequiredHandle = 0);
    /*!
      @brief inserts a key into the shared catalog hash table
     */
    Catalog_Cache::Catalog_CacheHandle* InsertEntry (int HashIndex, SQLMan_Context& Context, const Catalog_Object::Catalog_Key& Key);
    /*!
       @brief returns the allocator used by the catalog cache
     */
    static SAPDBMem_RawAllocator& GetAllocator();
    /*!
      @brief yields complete cache statistics
    */
    void GetAllStatistics (Cache_Statistics& Statistics) const;
    /*!
       @brief returns the required lru list
    */
    inline static Catalog_CacheLRUList& GetLruList(int idx);
    /*!
       @brief returns the spinlock pool
     */
    inline static RTESync_SpinlockPool& GetSpinlockPool();
    /*!
       @brief returns hit/miss statistics
     */
    void GetStatistics(bool GetResetStatistics, SAPDB_UInt8& HitCount, SAPDB_UInt8& MissCount);
    /*!
       @brief returns a global counter interpreted as a timestamp
     */
    inline static SAPDB_UInt4 GetTimeStamp();
    /*!
       @brief returns the trigger identification for a given catalog table. This identification is used to
              call a trigger function, whenever a row of the table is modified.
     */
    int GetTriggerId(const SQLMan_Identifier& TableName);
    /*!
       @brief marks the given handle as invalid
     */
    static bool HandleInvalid(Catalog_CacheHandle* pHandle);
    /*!
       @brief comuputes the hash key for a given key
     */
	SAPDB_UInt4 Hash(const Catalog_Object::Catalog_Key& Key);
    /*!
       @brief increments the counter for lru list inserts
     */
    inline static void IncEnterLru ();
     /*!
       @brief increments the counter for lru list removes
     */
    inline static void IncRemoveLru ();
    /*!
       @brief marks the specified authorization object as invalid, if present in the cache
     */
    void InvalidateAuthorization (RTE_TaskId TaskId, const SQLMan_Surrogate* pAuthId, const SQLMan_Identifier* pName);
    /*!
       @brief marks the specified table object as invalid, if present in the cache
     */
    void InvalidateTable (RTE_TaskId TaskId, const SQLMan_Surrogate& TableId);
    /*!
       @brief if the catalog object identified by Key is present in the cache, it is marked as invalid
     */
    void Invalidate (RTE_TaskId TaskId, const Catalog_Object::Catalog_Key& Key, Catalog_CacheHandle* pRequiredHandle);
    /*!
       @brief registers all catalog tables
     */
    void RegisterCatalogTables();
    /*!
       @brief releases the given handle. By calling this method the caller tells that he does not use this
              handle any more.
     */
    void Release (SQLMan_Context& Context, Catalog_CacheHandle* pHandle, bool DecRefCount);
    /*!
       @brief a call of this methods tries to reduce the amount of memory used by the catalog cache.
       @return true, if memory requirements could be reduced; otherwise false
     */
    bool ReleasedMemory();
    /*!
       @brief resets the statistics
     */
    void ResetStatistics();
    /*!
       @brief tells that the caller is going to use or not to use the handle, i.e. the caller will or will not access members of
              the corresponding catalog object.
       @param Context [in] the current sql manager context
       @param pHandle [in] the handle. Must be the result of a prior Access call.
       @param InUse [in] if true, the caller tells, that he is going to use the handle. If false, the caller
                         tells, that he not using the handle until the next call of SetHandleInUse
     */
    void SetHandleInUse(SQLMan_Context& Context, Catalog_CacheHandle* pHandle, bool InUse);
    /*!
       @brief tries to reduce the memory requirements of the cache. This method is called from the timeout process.
     */
    void Shrink(RTE_TaskId TaskId);
    /*!
       @brief computes the distance between two timestamps
     */
    inline static SAPDB_UInt4 TimeDiff(SAPDB_UInt4 T1, SAPDB_UInt4 T2);

	int                                                      m_headEntryCount;  //!< number of hash slots
    bool                                                     m_opMessages;      //!< if true, op messages are written in case of lack of memory
	SAPDB_UInt4                                              m_lockId;          //!< number generator for RW lock identifications
    SAPDB_UInt4                                              m_maxSize;         //!< max chunk size delivered by allocator
	Container_Vector<Catalog_CacheImplementation::HeadEntry> m_head;            //!< the hash slots
    Container_Vector<Catalog_CatalogTableDescriptor*>        m_tableDescriptor; //!< the catalog table descriptor
    Cache_Statistics                                         m_statistics;      //!< shared catalog statistics
    SAPDB_UInt4                                              m_lruClock;        //!< clock for lru handling
    Catalog_CacheLRUList*                                    m_lruList;         //!< the lru lists
    Catalog_CacheLRUList*                                    m_invalidateList;  //!< the list of invalidated entries
    RTESync_SpinlockPool                                     m_spinlockPool;    //!< pool of spinlocks used for synchronization
    RTESync_IRWRegion*                                       m_allocatorLock;   //!< synchronization of allocate/deallocate
    RTESync_IRWRegion*                                       m_invalidateLock;  //!< synchronization of list of invalidated handles
    static Catalog_Cache::Catalog_CacheImplementation*       m_pInstance;       //!< pointer to shared catalog instance
	friend class Catalog_Cache;
	friend class Catalog_Cache::Catalog_CacheHandle;
    friend class Catalog_LockedLruListScope;
};

#endif