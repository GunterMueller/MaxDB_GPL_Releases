/****************************************************************************

  module      : LVCMem_DefaultAllocator.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: liveCache memory management 
  description : default allocator of liboms

  last changed: 2000-07-11  13:54
  see also    : example.html ...
  first created:2000-05-26  19:32

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "liveCache/MemoryManagement/LVCMem_DefaultAllocator.hpp"

#include "geo573.h"
#include <stdlib.h>

LVCMem_DefaultAllocator::LVCMem_DefaultAllocator()
{
}

/*-----------------------------------------------------*/

void* LVCMem_DefaultAllocator::Allocate(SAPDB_ULong Count)
{
	void* p = malloc(Count);
	if (0 == p)
	{
		THROW_BAD_ALLOC_GEO573;
	}
	return p;
}

/*-----------------------------------------------------*/

void LVCMem_DefaultAllocator::Deallocate(void *p)
{
	free(p);
}

/*-----------------------------------------------------*/

void LVCMem_DefaultAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
  CountAlloc   = 0;
  CountDealloc = 0;
}

/*-----------------------------------------------------*/

void LVCMem_DefaultAllocator::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                                SAPDB_ULong &CountDealloc) const
{
  CountAlloc   = 0;
  CountDealloc = 0;
}
