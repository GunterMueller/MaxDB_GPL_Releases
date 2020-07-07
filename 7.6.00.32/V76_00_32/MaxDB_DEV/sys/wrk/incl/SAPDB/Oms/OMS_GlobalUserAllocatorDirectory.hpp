/*!
 * \file    OMS_GlobalUserAllocatorDirectory.hpp
 * \author  Roterring
 * \brief   Oms global user allocator register.
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

#ifndef __OMS_GLOBALUSERALLOCATORDIRECTORY_HPP
#define __OMS_GLOBALUSERALLOCATORDIRECTORY_HPP
  
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Oms/OMS_Defines.h"

class OMS_GlobalUserAllocator;
class OmsAllocatorInterface;
//class OmsHandle;
class OMS_Session;
class Msg_List;


/// Hash item for global user allocator list.
class OMS_UserAllocatorDirectoryItem {
public:
  char                            m_name[40+1];  ///< allocator name
  OMS_GlobalUserAllocator         *m_alloc;       ///< allocator
  OMS_UserAllocatorDirectoryItem  *m_next;        ///< next item in lvcGlobalUserAllocHash bucket

  OMS_UserAllocatorDirectoryItem(OMS_GlobalUserAllocator *alloc, const char *name, OMS_UserAllocatorDirectoryItem *next)
    : m_alloc(alloc), m_next(next)
  {
    SAPDB_MemCopyNoCheck(&m_name[0], name, 40);
    m_name[40] = 0;
  }

  void * operator new(size_t size);
  void deleteSelf();
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete(void *p);
#endif
};

typedef OMS_UserAllocatorDirectoryItem *OMS_UserAllocatorDirectoryItemPtr;

class OMS_GlobalUserAllocatorDirectory {
public:
  /// Bucket count for allocator GlobalUserAllocHash.
  enum {
    BUCKET_CNT  = 127
  };
  OMS_GlobalUserAllocatorDirectory();
  OmsAllocatorInterface *get(/*OmsHandle *pHandle,*/ const char *name);
  size_t hash(const char *name);
private:
  void init();
  bool m_isInitialized;
  OMS_UserAllocatorDirectoryItemPtr m_globalUserAllocItems[BUCKET_CNT];

};

#endif  // __OMS_GLOBALUSERALLOCATORDIRECTORY_HPP
