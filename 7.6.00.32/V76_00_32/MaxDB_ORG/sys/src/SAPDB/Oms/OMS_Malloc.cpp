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
#include "Oms/OMS_Malloc.hpp"
#include "Oms/OMS_Globals.hpp"

/*----------------------------------------------------------------------*/

void* omsMalloc(size_t size) 
{
  return OMS_Globals::m_globalsInstance->m_userAllocatorWrapper.Allocate(size);
}

/*----------------------------------------------------------------------*/

void* omsMallocArray(size_t size) 
{
  return OMS_Globals::m_globalsInstance->m_userAllocatorWrapper.AllocateArray(size);
}

/*----------------------------------------------------------------------*/

void omsFree(void* p) 
{
  OMS_Globals::m_globalsInstance->m_userAllocatorWrapper.Deallocate(p);
}

/*----------------------------------------------------------------------*/

void omsFreeArray(void* p) 
{
  OMS_Globals::m_globalsInstance->m_userAllocatorWrapper.DeallocateArray(p);
}

