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

#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/Catalog/Catalog_CacheImplementation.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

Catalog_Cache::Cache_Statistics::Cache_Statistics()
: m_hitCount(0)
, m_missCount(0)
, m_hitResetCount(0)
, m_missResetCount(0)
, m_releasedMemoryCount(0)
, m_enterLruCount(0)
, m_removeLruCount(0)
, m_entryCount(0)
, m_maxEntryCount(0)
, m_invalidateCount(0)
{
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Cache_Statistics::Reset()
{
    m_hitCount  += m_hitResetCount;
    m_missCount += m_missResetCount;
    m_hitResetCount  = 0;
    m_missResetCount = 0;
}

//-----------------------------------------------------------------------------

class Catalog_Cache::Catalog_CacheHandle
{
public :
    Catalog_CacheHandle()
	{
	}
private :
	void* m_pObj;
};

//-----------------------------------------------------------------------------

Catalog_Cache::Catalog_CacheImplementation* Catalog_Cache::m_impl = 0;

const Catalog_Cache::Catalog_ICacheHandle* Catalog_Cache::Access (SQLMan_Context                    &Context,
                                                                 const Catalog_Object::Catalog_Key &Key, 
                                                                 bool                               NoWait,
                                                                 const Catalog_Object*             &pObj,
                                                                 bool                              &ObjectExists)
{
	if (0 == m_impl)
	{
		m_impl = new (RTEMem_Allocator::Instance()) Catalog_CacheImplementation;
	}
	return m_impl->Access(Context, Key, NoWait, pObj, ObjectExists);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::GetStatistics(bool GetResetStatistics, SAPDB_UInt8& HitCount, SAPDB_UInt8& MissCount)
{
    if (m_impl)
    {
        m_impl->GetStatistics(GetResetStatistics, HitCount, MissCount);
    }
    else
    {
        HitCount  = 0;
        MissCount = 0;
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::GetAllStatistics (Cache_Statistics& Statistics)
{
   if (m_impl)
    {
        m_impl->GetAllStatistics(Statistics);
    }
}

//-----------------------------------------------------------------------------

int  Catalog_Cache::GetTriggerId(const SQLMan_Identifier& TableName)
{
    if (m_impl)
    {
        return m_impl->GetTriggerId(TableName);
    }
    return 0;
}

//-----------------------------------------------------------------------------

bool Catalog_Cache::HandleInvalid(Catalog_CacheHandle* pHandle)
{
    return m_impl->HandleInvalid(pHandle);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Invalidate (RTE_TaskId TaskId, const Catalog_Object::Catalog_Key& Key)
{
    if (m_impl)
    {
        m_impl->Invalidate (TaskId, Key, 0);
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::InvalidateAuthorization (RTE_TaskId TaskId, const SQLMan_Surrogate* pAuthId, const SQLMan_Identifier* pName)
{
    if (m_impl)
    {
        m_impl->InvalidateAuthorization (TaskId, pAuthId, pName);
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::InvalidateTable (RTE_TaskId TaskId, const SQLMan_Surrogate& TableId)
{
    if (m_impl)
    {
       m_impl->InvalidateTable (TaskId, TableId);
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::RegisterCatalogTables()
{
  	if (0 == m_impl)
	{
		m_impl = new (RTEMem_Allocator::Instance()) Catalog_CacheImplementation;
	}
    m_impl->RegisterCatalogTables();
}

//-----------------------------------------------------------------------------

void Catalog_Cache::ResetStatistics()
{
    if (0 != m_impl)
    {
        m_impl->ResetStatistics();
    }
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Release(SQLMan_Context& Context, Catalog_Cache::Catalog_CacheHandle* pHandle, bool DecRefCount)
{
    m_impl->Release(Context, pHandle, DecRefCount);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::SetHandleInUse(SQLMan_Context& Context, Catalog_CacheHandle* pHandle, bool InUse)
{
    m_impl->SetHandleInUse(Context, pHandle, InUse);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::Shrink(RTE_TaskId TaskId)
{
    if (m_impl)
    {
        m_impl->Shrink(TaskId);
    }
}

//-----------------------------------------------------------------------------

tgg00_BasisError Catalog_Cache::CallDeleteTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage)
{
    return m_impl->CallDeleteTrigger(TaskId, FuncIdx, BeforeImage);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::CallInsertTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& AfterImage)
{
    m_impl->CallInsertTrigger(TaskId, FuncIdx, AfterImage);
}

//-----------------------------------------------------------------------------

void Catalog_Cache::CallUpdateTrigger(RTE_TaskId TaskId, int FuncIdx, const tgg00_Rec& BeforeImage, const tgg00_Rec& AfterImage)
{
    m_impl->CallUpdateTrigger(TaskId, FuncIdx, BeforeImage, AfterImage);
}