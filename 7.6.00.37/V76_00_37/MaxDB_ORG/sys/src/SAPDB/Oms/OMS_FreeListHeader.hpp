/*!
 * \file    OMS_FreeListHeader.hpp
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
#ifndef __OMS_FREELISTHEADER_HPP
#define __OMS_FREELISTHEADER_HPP

#include "Oms/OMS_Defines.h"
#include "Oms/OMS_ObjectContainerDef.hpp"

class OMS_Context;

class OMS_FreeListHeader {
public:
  OMS_FreeListHeader *next;
  OmsObjectContainer *free;
  size_t              size;

  OMS_FreeListHeader(size_t sz);
  void* operator new(size_t sz, OMS_Context* pContext);
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete(void* p, OMS_Context* pContext);
#endif
  void DeleteSelf(OMS_Context* context, int caller);

  void                       Clear(OMS_Context* pContext, int caller); 
  inline void                Insert(OmsObjectContainer* pObj);
  inline OmsObjectContainer* GetFrame();

  void operator delete(void*p); // not implemented, attempt to call will force linker error
};


/*----------------------------------------------------------------------*/

inline void OMS_FreeListHeader::Insert(OmsObjectContainer* pObj)
{
  // Insert frame at first position
  pObj->SetNextFreeList(free);
  free = pObj;
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainer* OMS_FreeListHeader::GetFrame()
{
  // Remove and return first entry from the list
  OmsObjectContainer* p = free;
  if (free != NULL){
    free = free->GetNextFreeList();
  }

//#ifdef _ASSERT_OMS
  if (p != NULL){
    p->CheckFreeListState();
  }
//#endif

  return p;
}

/*----------------------------------------------------------------------*/

#endif // __OMS_FREELISTHEADER_HPP
