/*!
  @file           Catalog_SessionCache.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          Catalog Session Cache

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


#ifndef CATALOG_SESSIONCACHE_HPP
#define CATALOG_SESSIONCACHE_HPP

#include "SQLManager/Catalog/Catalog_Object.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/Catalog/Catalog_ISessionCache.hpp"

#include "Container/Container_Hash.hpp"

class SharedSQL_PrepareHandle;
class SharedSQL_SQLCache;
class SQLMan_Context;
class Catalog_MessBlockObject;
class Catalog_DBProcObject;
/*!

  @brief encapsulation of pascal AK cache
*/

class Catalog_SessionCache : public Catalog_ISessionCache
{
public :
    enum { ObjectHeader = mxak_cache_dir_entry };
    /*!
    @brief iterator over all entries in cache in lru order
    */
    class LruIterator
    {
        friend class Catalog_SessionCache;
    public :
        inline operator bool() const; //!< true if iterator didn't reached the end
        inline void operator++();     //!< steps to next cache entry
        inline tak_cache_dir_ptr operator()() const; //!< returns current entry
        inline Catalog_Object*   GetCatalogObject() const; //!< returns catalog object of current entry
    private :
        LruIterator(SQLMan_Context& context);
        SQLMan_Context&   m_context;  //!< SQLManager session and statement context
        tak_cache_dir_ptr m_curr;     //!< current cache entry
		tak_cache_dir_ptr m_next;     //!< next cache entry
    };

    /*!
    @brief constructor
    */
    Catalog_SessionCache(SQLMan_Context& context);
    /*!
    @brief adds a row into the catalog
     */
    bool AddIntoCatalog (const tgg00_Rec& Row, bool DoInsert);
    /*!
       @brief decrements the reference counter of a reference to the shared catalog
     */
    void DecrementReferenceRefCount(const Catalog_Object::Catalog_Key& key);
    /*!
       @brief diables access to the catalog tables. Catalog request are directed to the catalog files only
     */
    virtual void DisableCatalogTables();
    /*!
       @brief enables access to the catalog tables. Catalog request are directed to the catalog tables if required
     */
    virtual void EnableCatalogTables();
    /*!
    @brief returns a table desctiptor for a table identified by Schame and Tablename. The kernel order for name
           resoluation is used internally.
    */
    virtual Catalog_TableDescriptor GetTableDescriptor (const SQLMan_Identifier& Schema, const SQLMan_Identifier Tablename);
    /*!
       @brief increments the reference counter of a reference to the shared catalog
     */
    void IncrementReferenceRefCount(const Catalog_Object::Catalog_Key& key);
    /*!
    @brief removes the column information of a select into statement and returns a handle to that info
           The information can be restored using method RestoreColumnInfo.
    */
    virtual void* KeepColumnInfo();
    /*!
    @brief removes the parameter information of a select into statement and returns a handle to that info
           The information can be restored using method RestoreIntoParameterInfo.
    */
    virtual void* KeepIntoParameterInfo();
     /*!
    @brief loads the description of an authorization (role, schema, user) into the session cache
    @param authId the identification of the authorization
    */
    virtual Catalog_AuthorizationObject* LoadAuthorizationObject(const SQLMan_Surrogate& authId);
    /*!
    @brief loads the description of an authorization (role, schema, user) into the session cache
    @param authName the name of the authorization
    */
    virtual Catalog_AuthorizationObject* LoadAuthorizationObject(const SQLMan_Identifier& authName);
    /*!
    @brief loads the description of an oms container
    @param SchemaId the schema containing the oms container
    @param ContainerNo the container number of the container
    @param Guid the class identification 
    @param NoWait don't wait for 
    */
    virtual Catalog_Reference<Catalog_ContainerObject> LoadContainerObject (const Catalog_ObjectId& SchemaId, SAPDB_UInt4 ContainerNo, const Catalog_GUID& Guid, bool NoWait);
    /*!
    @brief loads the description of a db-procedure into the session cache
    */
    virtual Catalog_DBProcObject* LoadDBProcObject(const SQLMan_Surrogate& procId);
    /*!
    @brief loads the info description of a db-procedure into the session cache
    */
    virtual Catalog_DBProcInfoObject* LoadDBProcInfoObject(const SQLMan_Surrogate& procId);
    /*!
    @brief loads the file description of a db-procedure into the session cache
    */
    virtual Catalog_DBProcFileObject* LoadDBProcFileObject(const SQLMan_Surrogate& procId);
    /*!
    @brief loads the byte code of a stored procedure
    @param procId  the internal name of the procedure
    @return a pointer to the code or NULL if the code does not exist
    */
    virtual Catalog_MessBlockObject* LoadCode(const SQLMan_Surrogate& procId);
    /*!
       @brief read a row from the catalog
       @param Row[in,out] contains the key of the row to be read. As result Row contains the complete row.
       @return true, if the row has been read; false otherwise
     */
    bool ReadFromCatalog (tgg00_Rec& Row);
    /*!
    @brief restored the parameter information of a select into statement. The parameter info has to
           be identified by a handle returned by a call of KeepIntoParameterInfo 
    */
    virtual void RestoreIntoInfo(void* handle);
    /*!
    @brief  stores the current plan into the shared sql plan cache
	@return returns true if the plan could be stored successfully, otherwise false
    */
    virtual bool StorePlan(SharedSQL_SQLCache&   sharedSQLManager, 
		           SharedSQL_PrepareHandle*  handle, 
				   SQLMan_ParseId&           parseId,
                   bool                      prepareFlag);
	/*!
	   @brief destroys a catalog object stored in memory of a given allocator
    */
    static void DestroyPlanObject(SAPDBMem_IRawAllocator& allocator, void* p);
    /*!
       @brief returns the error code of the last operation
     */
    virtual SAPDB_Int2 GetLastError() const;
    /*!
    @brief returns the address of the Catalog_Object contained in the instance
    */
    inline static const Catalog_Object* GetObject(const void* p);
    /*!
    @brief loads a catalog object with given key into session cache
    */
    Catalog_Object* Load (Catalog_Object::Catalog_Key & key);
    /*!
       @brief defines a reference to a catalog key
     */
    class KeyReference
    {
    public :
        /*!
           @brief default contructor
         */
        KeyReference()
            : m_pKey(0)
        {
        }
        /*!
           @brief copy constructor
         */
        KeyReference(const Catalog_Object::Catalog_Key* pKey)
            : m_pKey(pKey)
        {
        }
        /*!
           @brief computes a hash value from the key
         */
        SAPDB_UInt HashValue() const
        {
            return m_pKey->HashValue();
        }
        /*!
           @brief checks, if current and given key are equal
         */
        bool operator== (const KeyReference& Cmp) const
        {
            if ((0 == m_pKey) || (0 == Cmp.m_pKey))
            {
                return false;
            }
            return (*m_pKey == *Cmp.m_pKey);
        }
    private :
        const Catalog_Object::Catalog_Key* m_pKey; //!< pointer to the key
    };

    private :
        /*!
         @brief returns an iterator of cache entries in lru order
         */
        LruIterator Begin() const;
        /*!
         @brief copies the plan from the shared sql component into the session cache
         */
        void RestorePlan (SharedSQL_PrepareHandle* cmdHandle);
        /*!
        @brief stores a part (record) of a plan into the shared sql manager
        */
        bool StorePlanObject (
            Catalog_Object&          planObject,
            SAPDBMem_IRawAllocator&  allocator,
            SharedSQL_ParseID&       PID,
            SharedSQL_PrepareHandle* cmdHandle);

        class GlobalCacheReference
        {
        public :
            GlobalCacheReference(const Catalog_Cache::Catalog_ICacheHandle* pHandle)
                : m_pHandle(pHandle)
                , m_refCount(1)
            {
            }
            ~GlobalCacheReference()
            {
            }
            int DecrementRefCount()
            {
                --m_refCount;
                return m_refCount;
            }
            void IncrementRefCount()
            {
                ++m_refCount;
            }
            Catalog_Cache::Catalog_ICacheHandle* GetHandle() const
            {
                return const_cast<Catalog_Cache::Catalog_ICacheHandle*>(m_pHandle);
            }
        private :
            const Catalog_Cache::Catalog_ICacheHandle* m_pHandle;
            SAPDB_Int4                                 m_refCount;
        };

        /*!
        @brief container storing references into the shared catalog
        */
        typedef Container_Hash<KeyReference, GlobalCacheReference> ReferenceHash;

        SQLMan_Context& m_context;    //!< sql manager context
        SAPDB_Int2      m_lastError;  //!< last occured error 
        ReferenceHash   m_referenceDirectory; 
};

#endif