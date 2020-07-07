/*!
 * \file    OMS_GlobaöUserAllocator.hpp
 * \author  Roterring
 * \brief   LiveCache user allocator (for session-specific memory).
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

#ifndef __OMS_GLOBALUSERALLOCATOR_HPP
#define __OMS_GLOBALUSERALLOCATOR_HPP

#include "Oms/OMS_AllocatorInterface.hpp"
#include "Oms/OMS_RawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "Oms/OMS_CallbackInterface.hpp"
#include "Oms/OMS_AllocatorTracer.hpp"

class Msg_List;

class OMS_GlobalUserAllocator : public SAPDBMem_RawAllocator, public OmsAllocatorInterface
{
  friend class OMS_GlobalUserAllocatorDirectory;
  friend class OMS_GlobalAnchorDirectory;
public:
  OMS_GlobalUserAllocator(
    const char                *Identifier,
    SAPDBMem_IRawAllocator    &BaseAllocator,
    SAPDB_ULong                FirstBlockSize,
    SAPDB_ULong                SupplementBlockSize); 
  virtual ~OMS_GlobalUserAllocator();

  virtual void *omsAllocate(size_t size);
  virtual void omsDeallocate(void *p);
  virtual bool omsCheckAllocatorEmpty();
  virtual int omsGetAllocatorType() const;

  virtual void omsRegisterCallback(OmsCallbackInterface *cb);
  virtual OmsCallbackInterface *omsGetCallback() const;

private:
  RTESync_Spinlock      m_lock;
  //friend class OMS_CheckedUserAllocator;

  OmsCallbackInterface *m_callback;
  OMS_AllocatorTracer   m_heapTracer;

  /*!
   * \todo Remove this method and use memory debugger's callbacks instead, when implemented.
   */
  void memLeftOver(const void *blk, SAPDB_ULong stack, size_t size, OmsCallbackInterface::BlockType blockType);
  /*!
   * \todo Remove this method and use memory debugger's callbacks instead, when implemented.
   */
  void memInvalidFree(const void *blk, SAPDB_ULong stack, size_t size, OmsCallbackInterface::BlockType blockType, OmsCallbackInterface::BlockType freeType);

  void *operator new(size_t sz, SAPDBMem_IRawAllocator &alloc);
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete(void *ptr, SAPDBMem_IRawAllocator &alloc);
#endif
  void operator delete(void *ptr) {}
  void deleteSelf(SAPDBMem_IRawAllocator &alloc);
};

#endif  // __OMS_GLOBALUSERALLOCATOR_HPP