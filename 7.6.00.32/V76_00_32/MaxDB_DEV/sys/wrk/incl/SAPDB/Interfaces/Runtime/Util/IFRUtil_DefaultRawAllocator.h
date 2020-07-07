/*!
  @file           IFRUtil_DefaultRawAllocator.h
  @author         D039759
  @ingroup        IFR_Mem
  @brief          Memory allocator using the client allocation functions.
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
#ifndef IFRUTIL_DEFAULTRAWALLOCATOR_H
#define IFRUTIL_DEFAULTRAWALLOCATOR_H

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/**
 * @ingroup IFR_Mem
 * The default allocator instance for use in the client runtime.
 *
 * @internal
 * The client runtime functions for memory allocation
 * (<code>sqlallocat</code>, <code>sqlfree</code>) are used.
 */
class IFRUtil_DefaultRawAllocator
    : public SAPDBMem_IRawAllocator
{ 
public:
    /**
     * Constructor.
     */
    IFRUtil_DefaultRawAllocator();
    
    /**
     * Destructor.
     */
    virtual ~IFRUtil_DefaultRawAllocator();

    // -- SAPDBMem_IRawAllocator methods

    virtual void* Allocate(SAPDB_ULong byteCount);
    virtual void* Allocate(SAPDB_ULong byteCount, const void *hint);
    virtual void Deallocate(void *ptr);
    
    // -- SAPDBMem_IAllocatorInfo methods
    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                                SAPDB_ULong &CountDealloc) const;
    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                   SAPDB_ULong &CountDealloc) const;

private:
    IFRUtil_DefaultRawAllocator(const IFRUtil_DefaultRawAllocator&);
    IFRUtil_DefaultRawAllocator& operator =(const IFRUtil_DefaultRawAllocator&);
};



#endif // IFRUTIL_DEFAULTRAWALLOCATOR_H

