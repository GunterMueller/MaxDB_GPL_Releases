/*!
 * \file    OMS_GlobalAnchorItem.hpp
 * \author  ChristophR
 * \brief   LiveCache global anchor item.
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

#ifndef __OMS_GLOBALANCHORITEM_HPP
#define __OMS_GLOBALANCHORITEM_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Oms/OMS_Defines.h"

class OMS_GlobalUserAllocator;

class OMS_GlobalAnchorItem {
public:
  OMS_GlobalAnchorItem(OMS_GlobalUserAllocator* alloc, int lockId) 
    : m_pAlloc(alloc),m_lockId(lockId),m_pAnchor(NULL),m_isValid(true),m_anchorClassId(0),m_anchorClassSize(0)
  {}
  const int                   m_lockId;           // Id of corresponding R/W lock
  OMS_GlobalUserAllocator    *m_pAlloc;           // Pointer to global shared memory allocator
  void                       *m_pAnchor;          // Pointer to entry anchor object
  int                         m_anchorClassId;    // Class id of entry anchor object
  size_t                      m_anchorClassSize;  // Size of entry anchor object
  bool                        m_isValid;          // Is global shared memory area valid?

  void *operator new(size_t size, SAPDBMem_IRawAllocator &alloc);
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete(void *ptr, SAPDBMem_IRawAllocator &alloc);
#endif
  void deleteSelf(SAPDBMem_IRawAllocator &alloc);
};

#endif  // __OMS_GLOBALANCHORITEM_HPP
