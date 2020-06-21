/*!
 * \file    OMS_GlobalAnchorItem.cpp
 * \author  Roterring
 * \brief   oms global anchor item
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

#include "Oms/OMS_GlobalAnchorItem.hpp"

/*----------------------------------------------------------------------*/

void *OMS_GlobalAnchorItem::operator new(size_t size, SAPDBMem_IRawAllocator &alloc){
  return alloc.Allocate(size);
}
  
/*----------------------------------------------------------------------*/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_GlobalAnchorItem::operator delete(void *ptr, SAPDBMem_IRawAllocator &alloc)
{
  alloc.Deallocate(ptr);
}
#endif
  
/*----------------------------------------------------------------------*/

void OMS_GlobalAnchorItem::deleteSelf(SAPDBMem_IRawAllocator &alloc){
  this->~OMS_GlobalAnchorItem();
  alloc.Deallocate(this);
}

/*---------------------------------------------------------------------------*/
