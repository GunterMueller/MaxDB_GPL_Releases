/*!
 * \file    OMS_AllocatorInterface.hpp
 * \author  IvanS
 * \brief   OMS allocator interface
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

#ifndef __OMS_ALLOCATORINTERFACE_HPP
#define __OMS_ALLOCATORINTERFACE_HPP

// general interface class for application allocators provided by liboms

#include "Oms/OMS_AFX.h"
#include "Oms/OMS_Defines.h"

class OmsCallbackInterface;

/*!
 * \brief User-specific allocator interface.
 *
 * This class defines an interface for user allocators in liboms.
 *
 * \see \ref omsuserallocator
 */
class AFX_EXT_CLASS OmsAllocatorInterface {
public:
  // Dummy constructor in order to prevent Win32 link errors.
  // inline OmsAllocatorInterface() {}
  OmsAllocatorInterface();

  /*!
   * \brief Allocate memory.
   *
   * Allocate memory in allocator. If the memory couldn't be allocated,
   * a bad_alloc exception will be thrown. Memory must be deallocated
   * in the same allocator using omsDeallocate().
   *
   * \param size size in bytes to allocate.
   *
   * \return pointer to newly allocated block.
   *
   * \see \ref omsuserallocator
   */
  virtual void *omsAllocate(size_t size) = 0;

  /*!
   * \brief Deallocate memory.
   *
   * Return memory to the allocator.
   *
   * \param p pointer to memory block previously allocated by omsAllocate().
   *
   * \see \ref omsuserallocator
   */
  virtual void omsDeallocate(void *p) = 0;

  /*!
   * \brief Check for emptyness.
   *
   * This routine is called internally to check, if there are any left-over
   * blocks in the allocator. For each left-over block, a callback is called
   * (see OmsCallbackInterface) and the block is marked as invalid (so it
   * won't be reported twice).
   *
   * You can also call this routine manually, when you think, the allocator
   * should be empty (e.g., for debug purposes).
   *
   * \return \c true, if the allocator doesn't have any leftovers, \c false otherwise.
   *
   * \see \ref omsuserallocator
   */
  virtual bool omsCheckAllocatorEmpty() = 0;

  /*!
   * \brief Register callback interface.
   *
   * Each allocator may have associated callback interface to be called when
   * left-overs are found in emptyness check or when trying to free invalid
   * block.
   *
   * \param cb callback interface to set.
   *
   * \see \ref omsuserallocator
   */
  virtual void omsRegisterCallback(OmsCallbackInterface *cb) = 0;

  /*!
   * \brief Get current callback interface.
   *
   * \return current callback interface.
   *
   * \see \ref omsuserallocator
   */
  virtual OmsCallbackInterface *omsGetCallback() const = 0;

  /*!
   * \brief Get allocator type of this interface.
   *
   * \return allocator type (one of OmsAllocatorInterface::BlockType constants).
   *
   * \since 7.4.3.15
   */
  virtual int omsGetAllocatorType() const = 0;
};

/*!
 * \brief New operator for allocators.
 *
 * \param sz size of the block,
 * \param i allocator interface.
 *
 * \return address of the new block.
 */
AFX_EXT_CLASS void *operator new(size_t sz, OmsAllocatorInterface &i);

/*!
 * \brief Delete operator for allocators.
 *
 * This operator is called if constructor throws an exception.
 *
 * \param ptr block address,
 * \param i allocator interface.
 */
AFX_EXT_CLASS void operator delete(void *ptr, OmsAllocatorInterface &i);


#endif  // __OMS_ALLOCATORINTERFACE_HPP