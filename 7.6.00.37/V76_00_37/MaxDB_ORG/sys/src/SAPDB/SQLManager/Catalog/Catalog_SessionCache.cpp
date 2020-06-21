/*!***************************************************************************

  module      : Catalog_SessionCache.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog
  description : encapsulation of pascal AK cache

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

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

#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"
#include "SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_SessionCacheInline.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Cache.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_ContainerObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_DBProcObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_DBProcFileObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_DBProcInfoObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_MessBlockObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_SharedSqlObject.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_Handles.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp"

#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"

#include "hak01.h"
#include "hak10.h"
#include "hak260.h"
#include "hak51.h"

//----------------------------------------------------------------------

template <> inline
SAPDB_UInt Container_HashFnc(const Catalog_SessionCache::KeyReference& Key)
{
    return Key.HashValue();
};

//----------------------------------------------------------------------

inline static void* KeepInfo (SQLMan_Context& context, int entryType)
{
    Catalog_Object::Catalog_Key key;
    key.SetEntryType(entryType);
    tgg00_Surrogate s;
    memset (&s[0], ' ', sizeof(s));
    key.SetSurrogate(s);
    key.SetResultId (1);
    return a10KeepRecord(context, key);
}

//----------------------------------------------------------------------

Catalog_ISessionCache::Reference::Reference(SQLMan_Context& Context, const Catalog_Object* pObject) 
: m_pObject(pObject)
, m_pContext(&Context)
{
}

//----------------------------------------------------------------------

Catalog_ISessionCache::Reference::Reference()
: m_pObject (0)
, m_pContext(0)
{
}

//----------------------------------------------------------------------

Catalog_ISessionCache::Reference::Reference(const Reference& Source)
: m_pObject (Source.m_pObject)
, m_pContext(Source.m_pContext)
{
    if (Source.m_pObject)
    {
        Catalog_SessionCache& sessionCache = reinterpret_cast<SQLMan_ContextSessionMembers*>(m_pContext->a_cpp_session_members)->m_sessionCache;
        sessionCache.IncrementReferenceRefCount(Source.m_pObject->GetCatalogKey());
    }
}

//----------------------------------------------------------------------

Catalog_ISessionCache::Reference::~Reference()
{
    if (m_pObject)
    {
        Catalog_SessionCache& sessionCache = reinterpret_cast<SQLMan_ContextSessionMembers*>(m_pContext->a_cpp_session_members)->m_sessionCache;
        sessionCache.DecrementReferenceRefCount(m_pObject->GetCatalogKey());
    }
}

//----------------------------------------------------------------------

void Catalog_ISessionCache::Reference::Assign (const Reference& Source)
{
  m_pObject  = Source.m_pObject;
  m_pContext = Source.m_pContext;
  if (Source.m_pObject)
  {
      Catalog_SessionCache& sessionCache = reinterpret_cast<SQLMan_ContextSessionMembers*>(m_pContext->a_cpp_session_members)->m_sessionCache;
      sessionCache.IncrementReferenceRefCount(Source.m_pObject->GetCatalogKey());
  }
}

//----------------------------------------------------------------------

Catalog_SessionCache::Catalog_SessionCache(SQLMan_Context& context)
: m_context(context)
, m_lastError(0)
, m_referenceDirectory(m_context.GetAllocator())
{
}

//----------------------------------------------------------------------

bool Catalog_SessionCache::AddIntoCatalog (const tgg00_Rec& Row, bool DoInsert)
{
    if (m_context.IsOk())
    {
        a10InsertIntoCatalog (m_context, Row, DoInsert);
    }
    return m_context.IsOk();
}

//----------------------------------------------------------------------

void* Catalog_SessionCache::KeepColumnInfo()
{
    return KeepInfo(m_context, TYPE_COLUMNNAMES);
}

//----------------------------------------------------------------------

void* Catalog_SessionCache::KeepIntoParameterInfo()
{
    return KeepInfo(m_context, TYPE_SHORTFIELDINFO);
}

//----------------------------------------------------------------------

void Catalog_SessionCache::DecrementReferenceRefCount(const Catalog_Object::Catalog_Key& key)
{
    KeyReference keyRef(&key);
    ReferenceHash::Iterator iter = m_referenceDirectory.Find(keyRef);
    if (iter != m_referenceDirectory.End()) 
    {
        int refCount = iter->value.DecrementRefCount();
        if (0 == refCount)
        {
             iter->value.GetHandle()->Release(m_context);
             m_referenceDirectory.Delete(iter);
        }
    }
}

//----------------------------------------------------------------------

void Catalog_SessionCache::DisableCatalogTables()
{
    a10DisableTableAccess();
}

//----------------------------------------------------------------------

void Catalog_SessionCache::EnableCatalogTables()
{
    a10EnableTableAccess();
}

//----------------------------------------------------------------------

void Catalog_SessionCache::IncrementReferenceRefCount(const Catalog_Object::Catalog_Key& key)
{
    KeyReference keyRef(&key);
    ReferenceHash::Iterator iter = m_referenceDirectory.Find(keyRef);
    if (iter != m_referenceDirectory.End()) 
    {
        iter->value.IncrementRefCount();
    }
}

//----------------------------------------------------------------------

Catalog_TableDescriptor Catalog_SessionCache::GetTableDescriptor (const SQLMan_Identifier& Schema, const SQLMan_Identifier Tablename) 
{
    return Catalog_TableDescriptor(m_context, Schema, Tablename, /* no results */ false, /* no sysdd */ false);
}

//----------------------------------------------------------------------

Catalog_AuthorizationObject* Catalog_SessionCache::LoadAuthorizationObject(const SQLMan_Surrogate& authId)
{
    Catalog_AuthorizationObject::Catalog_Key key(*REINTERPRET_CAST(const Catalog_ObjectId*, &authId));
    Catalog_Object* pAuthorization = this->Load(key);
    return REINTERPRET_CAST(Catalog_AuthorizationObject*, pAuthorization);
}

//----------------------------------------------------------------------

Catalog_AuthorizationObject* Catalog_SessionCache::LoadAuthorizationObject(const SQLMan_Identifier& authName)
{
    Catalog_Object::Catalog_Key key;
    a51build_userkey (authName, key);
    Catalog_Object* pRef = this->Load(key);
    if (pRef)
    {
        return this->LoadAuthorizationObject (pRef->suserref().ru_user_id);
    }
    return 0;
}

//----------------------------------------------------------------------

Catalog_SessionCache::LruIterator::LruIterator(SQLMan_Context& context)
: m_context(context)
{
    m_curr = context.a_sysdir.ci_cache_dir_header.cdir_ptr()->cd_lru_next().cdir_ptr();
	m_next = m_curr->cd_lru_next().cdir_ptr();
}

//----------------------------------------------------------------------

Catalog_SessionCache::LruIterator Catalog_SessionCache::Begin() const
{
   SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::Begin", SQLMan_Trace, 5);
   LruIterator iter(m_context);
   return iter;
}

//----------------------------------------------------------------------

void Catalog_SessionCache::DestroyPlanObject(SAPDBMem_IRawAllocator& allocator, void* p)
{
  Catalog_Object* pObject = REINTERPRET_CAST(Catalog_Object*, p);
  if (0 == pObject->KeyLength())
  {
	  // catalog object with header
	  pObject = REINTERPRET_CAST(Catalog_Object*, REINTERPRET_CAST(unsigned char*, p) + Catalog_SessionCache::ObjectHeader);
      SQLMAN_TRACE_BUFFER (Catalog_Trace, 5, "DestroyPlanObject", pObject->GetKey(),  pObject->KeyLength());
	  switch (pObject->Type())
	  {
	  case TYPE_RESULT:
	  case TYPE_TABLE :
		  {
			  Catalog_TableObject& Table = *pObject;
			  Table.DestroyColumns(allocator);
			  break;
		  }
	  case TYPE_MESSBLOCK :
		  {
			  Catalog_MessBlockObject& MessBlock = *pObject;
			  MessBlock.DestroyParts(allocator);
			  break;
		  }
	  }
  }
  allocator.Deallocate(p);
}

//----------------------------------------------------------------------

SAPDB_Int2 Catalog_SessionCache::GetLastError() const
{
    return m_lastError;
}

//----------------------------------------------------------------------

Catalog_Object* Catalog_SessionCache::Load (Catalog_Object::Catalog_Key & key)
{
    tak_sysbufferaddress p;
    a10get_sysinfo (m_context, key, d_fix, p, m_lastError);
    if (e_ok == m_lastError)
    {
        return REINTERPRET_CAST(Catalog_Object*, p);
    }
    return 0;
}

//----------------------------------------------------------------------

Catalog_MessBlockObject* Catalog_SessionCache::LoadCode(const SQLMan_Surrogate& procId)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::LoadCode", Catalog_Trace, 1);
    Catalog_Object::Catalog_Key key;
    a260BuildCodeSyskey (procId, key);
    return REINTERPRET_CAST(Catalog_MessBlockObject*, this->Load(key));
}

//----------------------------------------------------------------------

Catalog_SessionCache::Catalog_Reference<Catalog_ContainerObject> Catalog_SessionCache::LoadContainerObject 
(
 const Catalog_ObjectId& SchemaId, 
 SAPDB_UInt4             ContainerNo, 
 const Catalog_GUID&     Guid,
 bool                    NoWait
 )
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::LoadContainerObject", Catalog_Trace, 1);
    Catalog_ContainerObject::Catalog_Key key(SchemaId, ContainerNo, Guid);
    KeyReference          keyRef(&key);
    bool                  objectExists;
    const Catalog_Object* pObj;
    ReferenceHash::Iterator iter = m_referenceDirectory.Find(keyRef);
    if (iter == m_referenceDirectory.End()) 
    {
        const Catalog_Cache::Catalog_ICacheHandle* pHandle = Catalog_Cache::Access (m_context, key, NoWait, pObj, objectExists); 
        if (pHandle && objectExists) 
        {
            GlobalCacheReference globalRef(pHandle);
            iter = m_referenceDirectory.Insert(KeyReference(&pObj->GetCatalogKey()), globalRef);
            if (iter == m_referenceDirectory.End())
            {
                Catalog_Reference<Catalog_ContainerObject> ref(m_context, 0);
                return ref;
            }
        }
        else
        {
            // an error occurred or object does not exist
            Catalog_Reference<Catalog_ContainerObject> ref(m_context, 0);
            return ref;
        }
    }
    Catalog_Reference<Catalog_ContainerObject> ref(m_context, 
        reinterpret_cast<const Catalog_ContainerObject*> (iter->value.GetHandle()->GetObjectPointer()));
    return ref;
}

//----------------------------------------------------------------------

Catalog_DBProcInfoObject* Catalog_SessionCache::LoadDBProcInfoObject(const SQLMan_Surrogate& procId)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::LoadDBProcInfoObject", Catalog_Trace, 1);
    Catalog_DBProcInfoObject::Catalog_Key key(procId);
    return REINTERPRET_CAST(Catalog_DBProcInfoObject*, this->Load(key));
 }

//----------------------------------------------------------------------

Catalog_DBProcObject* Catalog_SessionCache::LoadDBProcObject(const SQLMan_Surrogate& procId)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::LoadDBProcObject", Catalog_Trace, 1);
    Catalog_DBProcObject::Catalog_Key key(procId);
    return REINTERPRET_CAST(Catalog_DBProcObject*, this->Load(key));
 }

//----------------------------------------------------------------------

Catalog_DBProcFileObject* Catalog_SessionCache::LoadDBProcFileObject(const SQLMan_Surrogate& procId)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::LoadDBProcFileObject", Catalog_Trace, 1);
    Catalog_DBProcFileObject::Catalog_Key key(procId);
    return REINTERPRET_CAST(Catalog_DBProcFileObject*, this->Load(key));
}

//----------------------------------------------------------------------

bool Catalog_SessionCache::ReadFromCatalog (tgg00_Rec& Row)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::ReadFromCatalog", Catalog_Trace, 1);
    a10ReadFromCatalog (m_context, Row);
    return m_context.IsOk();
}

//----------------------------------------------------------------------

void Catalog_SessionCache::RestoreIntoInfo(void* handle)
{
    if (handle)
    {
        a10InsertKeptRecord (m_context, reinterpret_cast<tak_sysbufferaddress>(handle));
    }
}

//----------------------------------------------------------------------

bool Catalog_SessionCache::StorePlan (SharedSQL_SQLCache&      sharedSQLCache, 
									  SharedSQL_PrepareHandle* cmdHandle, 
									  SQLMan_ParseId&           parseId,
                                      bool                      prepareFlag)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::StorePlan", SQLMan_Trace, 5);
	SharedSQL_ParseID& PID = *REINTERPRET_CAST(SharedSQL_ParseID*, &parseId); 
    SAPDBMem_IRawAllocator* pAllocator = SharedSQL_CommandCache::Instance(SAPDB_TRUE);
    if (0 == pAllocator)
    {
        return false;
    }
    // store the describe id
    tak_sysbufferaddress pDescId;
    tgg00_BasisError     e;
    a10find_result (m_context, parseId.pid_parsk(), d_release, pDescId, e);
    if (e_ok == e)
    {
        Catalog_SharedSqlObject* pSharedSqlObject = new(*pAllocator) Catalog_SharedSqlObject(pDescId->syskey().sstandardkey()); 
        if (!pSharedSqlObject)
        {
            return false;
        }
        SQLMAN_TRACE_BUFFER(SQLMan_Trace, 5, "Put into plan", pSharedSqlObject, pSharedSqlObject->Length());
        if ( !cmdHandle->PutPlan (PID, pSharedSqlObject, pSharedSqlObject->Length(), m_context.GetErrorList()))
        {
            return false;
        }
    }
    // store all plan parts (records) created by the current command lying in the session cache
	for (LruIterator iter = Begin(); iter; ++iter)
    {
        if (iter()->cd_state().includes(dreference))
        {
            continue;
        }
        
        if (iter()->cd_create_cmd_no() == m_context.GetCurrentCommandId())
        {
            if (!this->StorePlanObject (*iter.GetCatalogObject(), *pAllocator, PID, cmdHandle))
            {
                return false;
            }
        }
    }
    // store all plan parts (records) created by the current command which have already been unloaded 
    // into the data cache
    Catalog_Object::Catalog_Key key;
    while (a10NextDisplacedKey (m_context, key)) 
    {
        Catalog_Object* pObj = this->Load(key);
        if (pObj)
        {
            if (!this->StorePlanObject (*pObj, *pAllocator, PID, cmdHandle))
            {
                return false;
            }
        }
    }
	// store the return packet into plan
	int segmentLength       = m_context.a_return_segm->sp1s_segm_len();
	int offset              = Catalog_Object::DefaultOffset;
	Catalog_Object* pObject = REINTERPRET_CAST(Catalog_Object*, pAllocator->Allocate(offset + segmentLength));
	if (!pObject)
	{
		return false;
	}
    pObject = new(pObject) Catalog_Object(Catalog_Object::DefaultKeyLen, segmentLength);
	pObject->PutInfo(m_context.a_return_segm, segmentLength);
	SQLMAN_TRACE_BUFFER (SQLMan_Trace, 5, "put segment into plan :",  pObject, offset + segmentLength);
	if ( !cmdHandle->PutPlan (PID, pObject, offset + segmentLength, m_context.GetErrorList()))
    {
        RestorePlan (cmdHandle);
        if (!m_context.IsOk())
        {
             Msg_List msg (Msg_List::Error, SDBMSG_CATALOG_STORE_PLAN_OBJECT_FAILED);
             m_context.GetErrorList().AppendNewMessage(msg);
        }
        return false;
    }
   
    SQLMan_ParseId originalParseId = parseId;
    // finally put the parse id
    parseId.pid_parsk().p_kind.becomes(m_sharedSQLParseid);

    SharedSQL_ModuleName Module;
    Module.mName = m_context.a_modul_name; 
    Module.mSize = sizeof(m_context.a_modul_name);
    if (!sharedSQLCache.PutParseID(cmdHandle, PID, Module, prepareFlag, m_context.GetErrorList()))
    {
        RestorePlan (cmdHandle);
        if (!m_context.IsOk())
        {
             Msg_List msg (Msg_List::Error, SDBMSG_CATALOG_STORE_PLAN_OBJECT_FAILED);
             m_context.GetErrorList().AppendNewMessage(msg);
        }
        parseId = originalParseId;
        return false;
    }
    return true;
}

//----------------------------------------------------------------------

void Catalog_SessionCache::RestorePlan (SharedSQL_PrepareHandle* cmdHandle)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_SessionCache::RestorePlan", SQLMan_Trace, 5);
  
    SharedSQL_Plan::PlanPartIterator iter = cmdHandle->GetPlanPartIterator();
    while (iter)
    {
        const Catalog_Object* p = Catalog_SessionCache::GetObject(*iter);
        if ((p->Type() != TYPE_SHARED_SQL) && (p->Type() != 0))
        {
            SQLMAN_TRACE_BUFFER(SQLMan_Trace, 5, "RestorePlan", p, 20);
            a10CopyFromSharedSQL (m_context, *p);
            if (!m_context.IsOk()) 
            {
                return;
            }
        }
        ++iter;
    }
}

//----------------------------------------------------------------------

bool Catalog_SessionCache::StorePlanObject (
    Catalog_Object&             planObject,
    SAPDBMem_IRawAllocator&     allocator,
    SharedSQL_ParseID&          PID,
    SharedSQL_PrepareHandle*    cmdHandle
    )
{
    if ((TYPE_RESULTNAME == planObject.Type()) || (TYPE_TRIGGERPARSEINFO == planObject.Type()))
    {
        // don't copy these records, they are session local required only
        return true;
    }
    SAPDB_UInt4 memoryUsed;
    // this is a plan information of the current plan
    void* p = planObject.CreateCopy(allocator, Catalog_SessionCache::ObjectHeader, memoryUsed);
    if (p)
    {
        tgg00_BasisError e;
        SQLMAN_TRACE_BUFFER(SQLMan_Trace, 5, "Put into plan", p, Catalog_SessionCache::ObjectHeader + 20);
        // invalidate transaction of current plan info
        Catalog_Object::InvalidateTransaction(p, Catalog_SessionCache::ObjectHeader);
        // store plan info in shared sql manager
        if ( cmdHandle->PutPlan (PID, p, memoryUsed, m_context.GetErrorList()))
        {
            // finally delete plan info from session cache
            a10del_sysinfo (m_context, planObject.syskey(), e);
            return true;
        }
        else
        {
            this->DestroyPlanObject(allocator, p);
        }
    }
   
    RestorePlan (cmdHandle);
    if (!m_context.IsOk())
    {
        Msg_List msg (Msg_List::Error, SDBMSG_CATALOG_STORE_PLAN_OBJECT_FAILED);
        m_context.GetErrorList().AppendNewMessage(msg);
    }
    return false;
}

