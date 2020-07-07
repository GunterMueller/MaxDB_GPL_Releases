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
#ifndef __OMS_GLOBALALLOCATORWRAPPER_HPP
#define __OMS_GLOBALALLOCATORWRAPPER_HPP

#include "gsp00.h"
#include "ggg01.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


class Msg_List;

class OMS_GlobalAllocatorWrapper : public SAPDBMem_IRawAllocator
{
public :
  OMS_GlobalAllocatorWrapper(bool isUserAllocator);

  // RawAllocator interface
  virtual void* Allocate(SAPDB_ULong ByteCount);
  virtual void* AllocateArray(SAPDB_ULong ByteCount);
  virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint);
  virtual void* AllocateThrowsNothing(SAPDB_ULong ByteCount);
  virtual void Deallocate(void* p);
  virtual void DeallocateArray(void* p);

  // OMS allocator interface
  virtual inline void *allocate(size_t size) { return Allocate(size);}
  virtual inline void deallocate(void *p) { Deallocate(p);}

  // AllocatorInfo interface
  virtual void CalcStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled);
  virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const;
  virtual void GetCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const;
  virtual const SAPDB_UTF8* GetIdentifier() const;
private :
  bool m_isUserAllocator;
};

/*----------------------------------------------------------------------*/

class OMS_GlobalAllocatorWrapperThrowsNothing : public OMS_GlobalAllocatorWrapper
{
public:
  OMS_GlobalAllocatorWrapperThrowsNothing(bool isUserAllocator): 
      OMS_GlobalAllocatorWrapper(isUserAllocator)
  {}
  
  virtual void* Allocate(SAPDB_ULong ByteCount) {return AllocateThrowsNothing(ByteCount);}
  virtual void* AllocateArray(SAPDB_ULong ByteCount){return AllocateThrowsNothing(ByteCount);}
  virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint){return AllocateThrowsNothing(ByteCount);}
};

/*----------------------------------------------------------------------*/

#endif //__OMS_GLOBALALLOCATORWRAPPER_HPP
