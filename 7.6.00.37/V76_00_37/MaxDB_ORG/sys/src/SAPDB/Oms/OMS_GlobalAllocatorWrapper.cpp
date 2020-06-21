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
#include "Oms/OMS_GlobalAllocatorWrapper.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_Malloc.hpp"

#ifdef USE_SYSTEM_ALLOC_CO13
#include "geo573.h"
#endif


OMS_GlobalAllocatorWrapper::OMS_GlobalAllocatorWrapper(bool isUserAllocator):
  m_isUserAllocator(isUserAllocator)
{}

/*----------------------------------------------------------------------*/

void OMS_GlobalAllocatorWrapper::CalcStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled)
{
  OMS_Globals::KernelInterfaceInstance->CalcAllocatorStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
}

/*----------------------------------------------------------------------*/

void OMS_GlobalAllocatorWrapper::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
  OMS_Globals::KernelInterfaceInstance->GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
}

/*----------------------------------------------------------------------*/

void OMS_GlobalAllocatorWrapper::GetCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
  OMS_Globals::KernelInterfaceInstance->GetAllocatorCallStatistics(CountAlloc, CountDealloc);
}

/*----------------------------------------------------------------------*/

const SAPDB_UTF8* OMS_GlobalAllocatorWrapper::GetIdentifier() const
{
  return OMS_Globals::KernelInterfaceInstance->GetAllocatorIdentifier();
}

/*----------------------------------------------------------------------*/

void* OMS_GlobalAllocatorWrapper::Allocate(SAPDB_ULong ByteCount){
  return OMS_Globals::KernelInterfaceInstance->Allocate(ByteCount, m_isUserAllocator);
}

/*----------------------------------------------------------------------*/

void* OMS_GlobalAllocatorWrapper::AllocateArray(SAPDB_ULong ByteCount){
  return OMS_Globals::KernelInterfaceInstance->AllocateArray(ByteCount, m_isUserAllocator);
}

/*----------------------------------------------------------------------*/

void* OMS_GlobalAllocatorWrapper::Allocate(SAPDB_ULong ByteCount, const void* Hint){
  return OMS_Globals::KernelInterfaceInstance->Allocate(ByteCount, m_isUserAllocator);
}

/*----------------------------------------------------------------------*/

void* OMS_GlobalAllocatorWrapper::AllocateThrowsNothing(SAPDB_ULong ByteCount) {
  return OMS_Globals::KernelInterfaceInstance->AllocateThrowsNothing(ByteCount);
}

/*----------------------------------------------------------------------*/

void OMS_GlobalAllocatorWrapper::Deallocate(void* p){
  OMS_Globals::KernelInterfaceInstance->Deallocate(p, m_isUserAllocator);
}

/*----------------------------------------------------------------------*/

void OMS_GlobalAllocatorWrapper::DeallocateArray(void* p){
  OMS_Globals::KernelInterfaceInstance->DeallocateArray(p, m_isUserAllocator);
}

#if defined( USE_SYSTEM_ALLOC_CO13 ) && defined( WIN32 )
void *operator new(size_t sz) THROW_NEW_GEO573 
{
	return omsMalloc(sz);
}

void *operator new[](size_t sz) THROW_NEW_GEO573
{
	return omsMallocArray(sz);
}

void operator delete(void *p) THROW_DELETE_GEO573
{
	omsFree(p);
}

void operator delete[](void *p) THROW_DELETE_GEO573
{
	omsFreeArray(p);
}
#endif
