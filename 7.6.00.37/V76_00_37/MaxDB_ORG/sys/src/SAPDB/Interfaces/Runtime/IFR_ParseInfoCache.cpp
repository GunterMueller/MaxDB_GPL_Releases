/*!
  @file           IFR_ParseInfoCache.cpp
  @author         D039759
  @ingroup        IFRUtil
  @brief          A cache of parse information
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

// Prevents a Warning 5004 Uninitialized variable "static_#classcvt#693
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
// See http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1130907 - pragma not longer necessary
// #ifdef HPUX
// #pragma OPT_LEVEL 1
// #endif

#include "Interfaces/Runtime/IFR_ParseInfoCache.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "Interfaces/Runtime/Util/IFRUtil_HashMap.h"

IFR_BEGIN_NAMESPACE

// LRU list entry.
struct IFR_ParseInfoCacheData
{
    inline IFR_ParseInfoCacheData()
    :data(0),
     next(0),
     previous(0)
    {}


    IFR_ParseInfoData *data;     // parseinfo data
    IFR_ParseInfoCacheData *next;     // next in lru list
    IFR_ParseInfoCacheData *previous; // previous in lru list
};


//----------------------------------------------------------------------
// hash table key
struct IFR_ParseInfoKey
{
    const IFR_String* command;
    IFR_Int4          isolationlevel;
    
    IFR_ParseInfoKey()
    :command(0),
     isolationlevel(0)
    {}

    IFR_ParseInfoKey(const IFR_String* pcommand, IFR_Int4 pisolationlevel)
    :command(pcommand),
     isolationlevel(pisolationlevel)
    {}
    
};

//----------------------------------------------------------------------
// The hash function. 
struct IFR_ParseInfoKey_HashFunction
{
    IFR_Int4 operator () (const IFR_ParseInfoKey& x)
    {
        return x.command->hashCode() + x.isolationlevel;
    }
};

//----------------------------------------------------------------------
// The KeyEquals function
struct IFR_ParseInfoKey_KeyEquals
{
    IFR_Bool operator() (const IFR_ParseInfoKey& a, const IFR_ParseInfoKey& b)
    {
        if(b.command && a.command) {
            return a.isolationlevel==b.isolationlevel &&
                IFR_String::equalsWithEncoding(*(a.command), *(b.command));
        } else {
            return a.isolationlevel==b.isolationlevel &&
                a.command == b.command;
        }
    }
};


//----------------------------------------------------------------------
// Parse info cache implementation class.
class IFR_ParseInfoCacheImpl
    : public IFRUtil_RuntimeItem
{
public:
    IFR_ParseInfoCacheImpl(IFR_Int4 maxcachesize, 
                           IFRUtil_RuntimeItem& runtimeitem,
                           IFR_Bool& memory_ok)
    :IFRUtil_RuntimeItem(runtimeitem),
     m_maxcachesize(maxcachesize),
     m_hashtable(allocator, memory_ok)
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, IFR_ParseInfoCacheImpl);
        DBUG_PRINT(maxcachesize);
        
        SQLDBC_IRuntime::Error ignored;
        runtime.createMutex(m_datalock, allocator, ignored);
        m_first.data=NULL;
        m_first.next=&m_first; 
        m_first.previous=&m_first;
    }
    
    ~IFR_ParseInfoCacheImpl()
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, ~IFR_ParseInfoCacheImpl);
        IFR_Bool memory_ok=true;
        if(m_maxcachesize) {
            if(m_datalock) {
                runtime.lockMutex(m_datalock);
                // The cache is destroyed when the connection is killed.
                // This means it is senseless to drop anything, but the
                // memory must be freed.
                IFR_ParseInfoCacheData *l=m_first.next;
                while(l != &m_first) {
                    IFR_ParseInfoCacheData *l_next=l->next; // save next as l is deleted
                    l->data->m_connection=0; // avoid anything being dropped
                    // The data lock will prevent that someone attaches in between.
                    // Only if no statement is attached the parse info is shred completely,
                    // otherwise we will leave this task to that statement
                    if(l->data->m_refcount <= 0) {
                        l->data->runDecomission(memory_ok); // shred the data
                        if(!memory_ok) {
                            runtime.releaseMutex(m_datalock);
                            goto exit;
                        }
                        IFRUtil_Delete(l->data, allocator);
                    } else {
                        l->data->setCached(false);
                    }
                    IFRUtil_Delete(l, allocator);
                    l=l_next;
                }
                runtime.releaseMutex(m_datalock);
            }
        }
    exit:
        if(m_datalock) {
            SQLDBC_IRuntime::Error ignored;
            runtime.destroyMutex(m_datalock, allocator, ignored);
        }
    }
    

    /**
     * Throws a parse info out of the cache.
     */
    void reduceCacheByOne(IFR_Bool& memory_ok)
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, reduceCacheByOne);
        IFR_ParseInfoCacheData *l=m_first.previous;
        
        // decouple it from the list.
        l->previous->next=&m_first;
        m_first.previous=l->previous;
        
        // remove it from the hash 
        IFR_ParseInfoKey k(&(l->data->m_sqlcommand), l->data->m_isolationlevel);
        m_hashtable.erase(k);
        getProfile()->m_counter[IFR_PARSEINFOSQUEEZECOUNT]++;
        
        // get rid of the LRU list entry
        IFR_ParseInfoData *data=l->data;
        IFRUtil_Delete(l, allocator);
        
        // set to uncached, if the reference count is < 0,
        // it may be safely deleted, otherwise the 
        // prepared statement that still owns 
        // the data will delete it when it is ready.
        data->lock();
        data->setCached(false); 
        if(data->m_refcount<=0) {
            data->runDecomission(memory_ok);
            data->unlock();
            IFRUtil_Delete(data, allocator);
        } else {
            data->unlock();
        }
    }

    void addParseInfo(IFR_ParseInfoData* data, IFR_Bool& memory_ok)
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, addParseInfo);
        if(!memory_ok) {
            return;
        }
        if(m_maxcachesize) {
            runtime.lockMutex(m_datalock);
            
            if(m_maxcachesize < m_hashtable.size()) {
                reduceCacheByOne(memory_ok);
                if(!memory_ok) {
                    runtime.releaseMutex(m_datalock);
                    return;
                }
            }

            IFR_ParseInfoKey k(&(data->m_sqlcommand), data->m_isolationlevel);
            IFR_ParseInfoCacheData *cached_data=new IFR_ALLOCATOR(allocator) IFR_ParseInfoCacheData();
            cached_data->data=data;
            putTop(cached_data);
            m_hashtable.get(k, memory_ok)=cached_data;
            if(!memory_ok) {
                runtime.releaseMutex(m_datalock);
                return;
            }
            data->setCached(true);
            runtime.releaseMutex(m_datalock);
        }
    }


    IFR_ParseInfo* getParseInfo(const IFR_String& statement, 
                                IFR_Int4 isolationlevel, 
                                IFR_Int4 paraminfohash, 
                                IFR_Bool& parameternotmatch)
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, getParseInfo);
        DBUG_PRINT(statement);
        DBUG_PRINT(isolationlevel);
        parameternotmatch = false;
        if(m_maxcachesize) {
            runtime.lockMutex(m_datalock);
            IFR_ParseInfoKey k(&(statement), isolationlevel);
            ParseInfoHash::Iterator i=m_hashtable.find(k);
            if(i==m_hashtable.end() ) {
                runtime.releaseMutex(m_datalock);
                return 0;
            } else {
                if(paraminfohash != i->second->data->m_paraminfohash) {
                    runtime.releaseMutex(m_datalock);
                    parameternotmatch = true;
                    return 0;
                }
                IFR_ParseInfo *result=new IFR_ALLOCATOR(allocator) IFR_ParseInfo(i->second->data, i->second->data->m_reparsestamp);
                i->second->data->incRef();
                dataToTop(i->second);
                runtime.releaseMutex(m_datalock);
                return result;
            }
        } else {
            return 0;
        }
    }
    


    /** 
     * Adds a new entry at the top of the list.
     */
    void putTop(IFR_ParseInfoCacheData* data)
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, putTop);
        data->next=m_first.next;
        data->previous=&m_first;
        m_first.next->previous=data;
        m_first.next=data;
    }

    /**
     * Moves an entry to the top of the list.
     */
    void dataToTop(IFR_ParseInfoCacheData *data)
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, dataToTop);
        if(data->previous!=&m_first) {
            // unlink
            data->previous->next=data->next;
            data->next->previous=data->previous;
            // insertfront
            data->previous=&m_first;
            m_first.next->previous=data;
            data->next=m_first.next;
            m_first.next=data;
        }
    }
    
    /**
     * Moves data item to the bottom.
     */
    void dataToBottom(IFR_ParseInfoCacheData *data)
    {
        DBUG_METHOD_ENTER(IFR_ParseInfoCacheImpl, dataToBottom);
        if(data->next!=&m_first) {
            // unlink
            data->previous->next=data->next;
            data->next->previous=data->previous;
            // insertend
            data->next=&m_first;
            m_first.previous->next=data;
            data->previous=m_first.previous;
            m_first.previous=data;
        }
    }

    inline IFR_Profile* getProfile()
    {
        IFR_STATIC_ASSERT(sizeof(IFR_Profile) == sizeof(SQLDBC_IRuntime::Profile));
        IFR_STATIC_ASSERT(sizeof(IFRUtil_LinkedList<IFR_Environment>) == sizeof(SQLDBC_IRuntime::EnvironmentList));
        return reinterpret_cast<IFR_Profile *>( &(runtime.m_profile) );
    }    
    
private:
    typedef IFRUtil_HashMap<IFR_ParseInfoKey, 
                            IFR_ParseInfoCacheData *, 
                            IFR_ParseInfoKey_HashFunction,
                            IFR_ParseInfoKey_KeyEquals> ParseInfoHash;
                            
 
    IFR_ParseInfoCacheData m_first;        // first entry of LRU list
    IFR_UInt4              m_maxcachesize; // maximum size of cache
    ParseInfoHash          m_hashtable;    // hashtable for lookup
    SQLDBC_IRuntime::MutexHandle m_datalock; // lock for concurrent access   
};

#define DBUG_PICACHE_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x,y, m_impl)

//----------------------------------------------------------------------
IFR_ParseInfoCache::IFR_ParseInfoCache(IFR_Int4 maxcachesize, 
                                       IFRUtil_RuntimeItem& runtimeitem,
                                       IFR_Bool& memory_ok)
:m_impl(0)
{
    DBUG_PICACHE_METHOD_ENTER(IFR_ParseInfoCache, IFR_ParseInfoCache);
    DBUG_PRINT(maxcachesize);
    if(!memory_ok) {
        return;
    }
    m_impl=new IFR_ALLOCATOR(runtimeitem.allocator) 
        IFR_ParseInfoCacheImpl(maxcachesize, runtimeitem, memory_ok);
    if(m_impl == 0) {
        memory_ok = false;
    } else if(!memory_ok) {
        IFRUtil_Delete(m_impl, runtimeitem.allocator);
        m_impl = 0;
    }
}

//----------------------------------------------------------------------
IFR_ParseInfoCache::~IFR_ParseInfoCache()
{
    DBUG_PICACHE_METHOD_ENTER(IFR_ParseInfoCache, ~IFR_ParseInfoCache);
    if(m_impl) {
        IFRUtil_Delete(m_impl, m_impl->allocator);
    }
}

//----------------------------------------------------------------------
void 
IFR_ParseInfoCache::checkReleaseParseInfo(IFR_ParseInfoData* data, IFR_Bool& memory_ok)
{
    DBUG_PICACHE_METHOD_ENTER(IFR_ParseInfoCache, checkReleaseParseInfo);
    data->lock();
    if(!(data->isCached())) {
        data->runDecomission(memory_ok);
        data->unlock();
        IFRUtil_Delete(data, m_impl->allocator);
    } else {
        data->unlock();
    }
    // otherwise it will be cleaned up when a new parseinfo 
    // is to be inserted.
}

//----------------------------------------------------------------------
void
IFR_ParseInfoCache::addParseInfo(IFR_ParseInfo* parseinfo, IFR_Bool& memory_ok)
{
    DBUG_PICACHE_METHOD_ENTER(IFR_ParseInfoCache, addParseInfo);
    
    m_impl->addParseInfo(parseinfo->m_data, memory_ok);
}

//----------------------------------------------------------------------
IFR_ParseInfo *
IFR_ParseInfoCache::getParseInfo(const IFR_String& statement, 
                                 IFR_Int4 isolationlevel,
                                 IFR_Int4 paraminfohash,
                                 IFR_Bool& parameternotmatch)
{
    DBUG_PICACHE_METHOD_ENTER(IFR_ParseInfoCache, getParseInfo);
    DBUG_PRINT(statement);
    DBUG_PRINT(isolationlevel);
        
    return m_impl->getParseInfo(statement, isolationlevel, paraminfohash, parameternotmatch);
}

IFR_END_NAMESPACE
