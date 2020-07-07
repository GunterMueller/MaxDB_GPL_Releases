/*!
 * \file    OMS_FreeListHeader.cpp
 * \author  MarkusSi, Roterring
 * \brief   FreeList for empty object frames.
 */

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


#include "Oms/OMS_FreeListHeader.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ObjectContainer.hpp"

/*----------------------------------------------------------------------*/

OMS_FreeListHeader::OMS_FreeListHeader(size_t sz) 
: next(NULL)
, free(NULL) 
, size(sz)
{
}

/*----------------------------------------------------------------------*/

void* OMS_FreeListHeader::operator new(size_t sz, OMS_Context* pContext) 
{
  return pContext->allocate(sz);
}

/*----------------------------------------------------------------------*/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_FreeListHeader::operator delete (void* p, OMS_Context* pContext) 
{
  pContext->deallocate(p);
}
#endif

/*----------------------------------------------------------------------*/

void OMS_FreeListHeader::DeleteSelf (OMS_Context* pContext, int caller) 
{
  Clear(pContext, caller);
  pContext->deallocate(this);
}

/*----------------------------------------------------------------------*/

void OMS_FreeListHeader::Clear(OMS_Context* pContext, int caller) 
{
  // If in version, then deallocate the object frames. (If not in version
  // the stack allocator will free all object frames at once at end of
  // transaction
  if (pContext->IsVersion()) {
    OmsObjectContainer *pCurr = free;
    OmsObjectContainer *pDel;
    while (NULL != pCurr) {
      pDel    = pCurr;
      pCurr   = pCurr->GetNextFreeList();
      free    = pCurr;      // Change pointer in header. Otherwise if an error occurs
                            // during the deallocation, a later processing of the already
                            // freed objects will lead to errors in the allocator.

      pDel->InitializeForAllocator(caller);
      pContext->deallocate(pDel);
    }
  }
  free = NULL;
}

/*----------------------------------------------------------------------*/
