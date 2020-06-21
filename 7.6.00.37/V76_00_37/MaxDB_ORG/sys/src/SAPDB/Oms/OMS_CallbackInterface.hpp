/*!
 * \file    OMS_CallbackInterface.hpp
 * \author  IvanS, MarkusS, PeterG
 * \brief   OMS callback interface
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

#ifndef __OMS_CALLBACKINTERFACE_HPP
#define __OMS_CALLBACKINTERFACE_HPP

class OmsHandle;
class SqlHandle;     /* PTS 1120794 */

/// OMS callback interface.
/*!
**  This class provides an interface for callback methods, which are called at
**  certain events.
**
**  \see \ref oms_CallbackInterfaceAPI
*/
class OmsCallbackInterface {
  friend class OmsHandle;
  friend class OMS_Session;
  friend class LVCSim_liveCacheSink;
  friend class LVCSim_CheckedAllocator;
  friend class LVCMem_UserAllocator;
  friend class LVCMem_CheckedUserAllocator;
  friend class OMS_UserAllocator;
  friend class OMS_CheckedUserAllocator;
  friend class OMS_GlobalUserAllocator;
public:
  /// Callback event.
  enum Event {
    BeforeCommit,   ///< Called before COMMIT.
    AfterCommit,    ///< Called after COMMIT.
    AfterRollback,  ///< Called after ROLLBACK.
    ByApplication   ///< Called by application.
  };
  /// Block type for memory allocator callback.
  enum BlockType {
    BlockFreed        = -1,   ///< Freeing already freed block.
    BlockInvalid      = 0,    ///< Freeing a block using invalid allocator.
    BlockGlobal       = 1,    ///< Global allocator.
    BlockSession      = 2,    ///< Session-specific allocator.
    BlockTransaction  = 3,    ///< Transaction-specific allocator.
    BlockCOMRoutine   = 4     ///< COM routine-specific allocator.
#ifdef OMS_IMPLEMENTATION
    ,BlockUnregister  = 15    ///< Unregister callback interfaces at session end.
#endif
  };

protected:
  virtual ~OmsCallbackInterface() {};

  /*!
   * \brief Deletes the callback interface.
   */
  virtual void omsDestroySelf() = 0;

  /*!
   * \brief Called, after the execution of a stored procedure.
   *
   * \param h current handle
   */
  virtual void omsMethodCallEpilog(OmsHandle &h) {};

  /*!
   * \brief Called, after the execution of a stored procedure.
   *
   * \param oh current OMS handle
   * \param sh current SQL handle
   */
  /* new method since PTS 1120794 */
  virtual void omsMethodCallEpilog(OmsHandle& oh, SqlHandle& sh) {
    omsMethodCallEpilog(oh);
  };

  /*!
   * \brief Called, in the very first phase of the commit handling.
   * Called, in the very first phase of the commit handling, before the subtransactions
   * are committed, the cache-buffer is flushed and the before-images are freed.
   *
   * \param h current handle
   * \param event event that have triggered the call 
   * \param inVersion Is currently a version active?
   */
  virtual void omsValidate (OmsHandle &h, Event event, bool inVersion) = 0;

  /*!
   * \brief Called, in a later phase of the commit and rollback handling.
   * Called, in a later phase of the commit and rollback handling, after the subtransactions
   * are either committed or rolled back, the cache-buffer is flushed and the before-images 
   * are freed.
   *
   * \param h current handle
   * \param event event that have triggered the call 
   * \param inVersion Is currently a version active?
   */
  virtual void omsInvalidate (OmsHandle &h, Event event, bool inVersion) = 0;

  /*!
   * \brief Called, when some block is left over in the allocator.
   *
   * \param blk pointer to the block,
   * \param size size of the block,
   * \param blockType allocator ID (1=global, 2=session, 3=transaction, 4=COM routine).
   *
   * \see \ref omsuserallocator.
   */
  virtual void omsMemLeftOver(const void *blk, size_t size, BlockType blockType) = 0;

  /*!
   * \brief Called, when some block is freed in an incorrect allocator.
   *
   * In case of invalid free, blockType parameter of omsMemInvalidFree identifies
   * allocator block type (which allocator we used for free), parameter freeType
   * block type in the block itself. If freeType == BlockInvalid, the block has been
   * (either now or in the past) freed using an invalid allocator.
   *
   * \param blk pointer to the block,
   * \param size size of the block,
   * \param blockType allocator ID (1=global, 2=session, 3=transaction, 4=COM routine),
   * \param freeType allocator ID in the block (like blockType; -1/0=invalid allocator used to free).
   *
   * \see \ref omsuserallocator.
   */
  virtual void omsMemInvalidFree(const void *blk, size_t size, BlockType blockType, BlockType freeType) = 0;
};

#endif  // __OMS_CALLBACKINTERFACE_HPP
