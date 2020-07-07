/*!
 * \file    OMS_GlobalAnchorDirectory.hpp
 * \author  IvanS
 * \brief   LiveCache global user allocator register.
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

#ifndef __OMS_GLOBALANCHORDIRECTORY_HPP
#define __OMS_GLOBALANCHORDIRECTORY_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Oms/OMS_Defines.h"

class OMS_GlobalAnchorItem;
class OmsHandle;
class Msg_List;

/// Hash item for global user allocator list.
class OMS_AnchorDirectoryItem {
public:
  OMS_GlobalAnchorItem    *m_anchorItem;
  int                      m_areaId;
  int                      m_anchorId;
  OMS_AnchorDirectoryItem *m_next;      ///< next item in lvcGlobalAnchorHash bucket

  OMS_AnchorDirectoryItem(OMS_GlobalAnchorItem *anchorItem, 
     int areaId, int anchorId, OMS_AnchorDirectoryItem *next)
    : m_anchorItem(anchorItem), m_next(next), m_areaId(areaId), m_anchorId(anchorId)
  {}
  
  void *operator new(size_t size, SAPDBMem_IRawAllocator &alloc);
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete(void *ptr, SAPDBMem_IRawAllocator &alloc);
#endif
  void deleteSelf(SAPDBMem_IRawAllocator &alloc);
};

typedef OMS_AnchorDirectoryItem *OMS_AnchorDirectoryItemPtr;

class OMS_GlobalAnchorDirectory {
public:
  /// Bucket count for allocator lvcGlobalAnchorHash.
  enum {
    BUCKET_CNT  = 127
  };
  OMS_GlobalAnchorDirectory();
  OMS_GlobalAnchorItem *get(OmsHandle *pHandle, int areaId, int anchorId);

private:
  OMS_AnchorDirectoryItemPtr m_globalAnchorItems[BUCKET_CNT];
  int m_lockIdCounter;
};

#endif  // __OMS_GLOBALANCHORDIRECTORY_HPP
