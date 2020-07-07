/*!
  @file           Catalog_Cache.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          interface of the shared catalog cache

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


#ifndef CATALOG_CACHE_HPP
#define CATALOG_CACHE_HPP

#include "SQLManager/Catalog/Catalog_Object.hpp"

class SQLMan_Context;


/*!
   @brief defines the shared catalog cache and its interface
 */
class Catalog_Cache
{
public :
    /*!
       @brief structure used to return statistics from the shared catalog cache
     */
    struct Cache_Statistics 
    {
        SAPDB_UInt8  m_hitCount;              //!< no of successful cache accesses
        SAPDB_UInt8  m_missCount;             //!< no of unsuccessful cache accesses
        SAPDB_UInt8  m_hitResetCount;         //!< no of successful cache accesses
        SAPDB_UInt8  m_missResetCount;        //!< no of unsuccessful cache accesses
        SAPDB_UInt8  m_releasedMemoryCount;   //!< no of memory release
        SAPDB_UInt8  m_enterLruCount;         //!< no of inserts into a lru list
        SAPDB_UInt8  m_removeLruCount;        //!< no of deletes from a lru list
        SAPDB_UInt4  m_lruEntryCount;         //!< no of elements in all lru list
        SAPDB_UInt4  m_entryCount;            //!< no of catalog objects stored in the cache
        SAPDB_UInt4  m_maxEntryCount;         //!< maximal no of catalog objects in the cache
        SAPDB_UInt4  m_invalidateCount;       //!< no of invalidates
        SAPDB_UInt4  m_invalidHandleCount;    //!< no of handles stored in the invalidated list
        /*!
           @brief default constructor, initializes all members to 0
         */
        Cache_Statistics();
        /*!
           @brief resets statistics
         */
        void Reset();
    };
    /*!
       @brief defines the interface of a handle to the catalog cache. A handle represents a catalog
              object which lies in the shared catalog cache
     */
    class Catalog_ICacheHandle {
    public : 
        /*!
           @brief returns a pointer to the catalog object identified by the handle 
         */
        virtual const Catalog_Object* GetObjectPointer() const = 0;
        /*!
           @brief the owner of the handle tells that he will no longer use it
         */
        virtual void Release(SQLMan_Context& Context) = 0;
    };

	class Catalog_CacheHandle;
    /*!
       @brief  Accesses a catalog object in the cache. If the object is not already present, it is
               read from the corresponding catalog tables. 
       @param  Key [in] identifies the object to be accessed
       @param  NoWait [in] don't wait for locks
       @param  pObj [out] a pointer to the required object
       @param  ObjectExists [out] true, if the required object exists; otherwise false
       @return a handle identifying the control block of the object. The handle must be released, 
               when the caller gives up the reference to the object.
     */
	static const Catalog_ICacheHandle* Access (SQLMan_Context& Context, 
        const Catalog_Object::Catalog_Key  &Key, 
        bool                               NoWait,
        const Catalog_Object*              &pObj,
        bool                               &ObjectExists);
    /*!
       @brief yields hit and miss statistics of the catalog cache
     */
    static void GetStatistics(bool GetResetStatistics, SAPDB_UInt8& HitCount, SAPDB_UInt8& MissCount);
    /*!
       @brief yields all statistics of the catalog cache
     */
    static void GetAllStatistics (Cache_Statistics& Statistics);
    /*!
       @brief yields the id of a trigger to be called, if a row of a catalog table is changed. If
              the given catalog table does not need a trigger, 0 is returned.
       @param TableName [in] the table name of the catalog table which is going to change any of its rows.
     */
    static int  GetTriggerId(const SQLMan_Identifier& TableName);
    /*!
       @brief checks, whether a handle is still valid. If a handle is invalid, the called shoud release the
              handle immediatly.
     */
    static bool HandleInvalid(Catalog_CacheHandle* pHandle);
    /*!
       @brief marks the specified object as invalid.
       @param TaskId [in] the task identification of the caller
       @param key [in] the ke yof the object to be invalidated
      */
    static void Invalidate (RTE_TaskId TaskId, const Catalog_Object::Catalog_Key& Key);
    /*!
       @brief marks the specified authorization object as invalid.
       @param TaskId [in] the task identification of the caller
       @param pAuthid [in] the objectid of the object to be invalidated (may be 0)
       @param pName [in] the name of the object to be invalidated (may be 0)
     */
    static void InvalidateAuthorization (RTE_TaskId TaskId, const SQLMan_Surrogate* pAuthId, const SQLMan_Identifier* pName);
    /*!
       @brief marks the specified table object as invalid.
       @param TaskId [in] the task identification of the caller
       @param TableId [in] the objectid of the table to be invalidated 
     */
    static void InvalidateTable (RTE_TaskId TaskId, const SQLMan_Surrogate& TableId);
    /*!
       @brief registers all catalog tables, that require invalidation of objects inside the catalog cache whenever rows of
              the table are deleted, inserted or updated.
     */
    static void RegisterCatalogTables();
    /*!
       @brief releases a handle
       @param Context [in] the current sql manager context
       @param pHandle [in] the handle to be released. Must be the result of a prior Access call.
       @param DecRefCount [in] must be true, if is handle is in use from the callers point of view.  
     */
	static void Release(SQLMan_Context& Context, Catalog_CacheHandle* pHandle, bool DecRefCount);
    /*!
       @brief resets the statistics
     */
    static void ResetStatistics();
    /*!
       @brief tells that the caller is going to use or not to use the handle, i.e. the caller will or will not access members of
              the corresponding catalog object.
       @param Context [in] the current sql manager context
       @param pHandle [in] the handle. Must be the result of a prior Access call.
       @param InUse [in] if true, the caller tells, that he is goiung to use the handle. If false, the caller
                         tells, that he not using the handle until the next call of SetHandleInUse
     */
    static void SetHandleInUse(SQLMan_Context& Context, Catalog_CacheHandle* pHandle, bool InUse);
    /*!
       @brief tries to reduce the memory requirements of the cache
      */
    static void Shrink(RTE_TaskId TaskId);
    /*!
       @brief calls the corresponding trigger function, if a row of a catalog table is deleted
     */
    static tgg00_BasisError CallDeleteTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage);
    /*!
       @brief calls the corresponding trigger function, if a row of a catalog table has been inserted
     */
    static void CallInsertTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& AfterImage);
    /*!
       @brief calls the corresponding trigger function, if a row of a catalog table has been updated
     */
    static void CallUpdateTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage, const tgg00_Rec& AfterImage);
    class Catalog_CacheImplementation;
private :
    static Catalog_CacheImplementation* m_impl;
};

#endif