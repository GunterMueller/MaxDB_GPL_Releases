/*!
 * \file    LVC_LoadComponentLibraryCache.cpp
 * \author  FerdiF
 * \brief   Load a component library and calculate an MD5 footprint.
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

#include "liveCache/LVC_LoadComponentLibraryCache.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#define ZERO_PACKAGEID "\x00\x00\x00\x00\x00\x00\x00\x00"

static RTESync_Spinlock lock;

//-------------------------------------------------------------------------------------------------

LVC_ILoadComponentLibraryCache& 
LVC_ILoadComponentLibraryCache::GetInstance()
{
  return LVC_LoadComponentLibraryCache::Instance();;
}

//-------------------------------------------------------------------------------------------------

LVC_LoadComponentLibraryCache::LVC_LoadComponentLibraryCache()
{
  LibraryCache* ptr  = reinterpret_cast<LibraryCache*>(RTEMem_Allocator::Instance().Allocate(sizeof(LibraryCache)));
  if (ptr)
    m_LibraryCache = new(ptr) LibraryCache(RTEMem_Allocator::Instance());
}

//-------------------------------------------------------------------------------------------------

LVC_LoadComponentLibraryCache::~LVC_LoadComponentLibraryCache()
{
  destroy(m_LibraryCache, RTEMem_Allocator::Instance());
};

//-------------------------------------------------------------------------------------------------

bool 
LVC_LoadComponentLibraryCache::Find(const tsp00_C8 &packageId, HANDLE* hInstance, tsp00_C32 &MD5Footprint)
{
  RTESync_LockedScope scope(lock);

  if ((0 == m_LibraryCache) || !memcmp(&packageId[0], &ZERO_PACKAGEID[0], sizeof(packageId)))
    return false;

  LibraryCache::Iterator iter = m_LibraryCache->Find(packageId);
  if (iter != m_LibraryCache->End())
  {
    *hInstance    = iter->value.hInstance; 
    MD5Footprint  = iter->value.MD5Footprint;
    return true;
  }
  else
    return false;
};

//-------------------------------------------------------------------------------------------------

void 
LVC_LoadComponentLibraryCache::Insert(const tsp00_C8 &packageId, HANDLE const* hInstance, const tsp00_C32 &MD5Footprint)
{
  RTESync_LockedScope scope(lock);

  if ((0 == m_LibraryCache) || !memcmp(&packageId[0], ZERO_PACKAGEID, sizeof(packageId)))
    return;

  LibraryCacheEntry val;
  val.hInstance   = *hInstance;
  val.MD5Footprint = MD5Footprint;
  LibraryCache::Iterator iter = m_LibraryCache->Insert(packageId, val);
};