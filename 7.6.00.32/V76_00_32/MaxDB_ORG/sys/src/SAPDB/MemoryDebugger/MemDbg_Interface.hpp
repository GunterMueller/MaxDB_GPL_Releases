/*!
 * \file    MemDbg_Interface.hpp
 * \author  IvanS
 * \brief   Memory debugger interface.
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

#ifndef __MEMDBG_INTERFACE_HPP
#define __MEMDBG_INTERFACE_HPP

#include "MemoryDebugger/MemDbg_Exports.hpp"
#include <stddef.h> // size_t

class MemDbg_Config;
class MemDbg_Callback;
class MemDbg_Lock;
class MemDbg_BlockHash;
class MemDbg_Header;

#ifdef WIN32
/// Register pure call function on Win32
#define MEMDBG_REGISTER_PURECALL MemDbg_Interface::mdRegisterPurecall(_purecall)
#else
// No pure call registration needed on Unix
#define MEMDBG_REGISTER_PURECALL
#endif

/*!
 * \brief Memory debugger interface.
 *
 * This class serves as an interface to the memory debugger. Allocators
 * use it to communicate with the memory debugger.
 *
 * An allocator would implement memory debugger interface in following
 * way. Let's assume that the allocator MyAllocator has allocation and
 * deallocation methods "void *allocate(size_t)" and "void free(void*)".
 * We rename these methods to mdAllocatePrim and mdDealloactePrim and
 * implement them anew as follows:
 *
 * \code
 *  class MyAllocator : public MemDbg_Interface {
 *  public:
 *    ...
 *    void *allocate(size_t size)
 *    {
 *      return mdAllocate(size);
 *    }
 *    void free(void *ptr)
 *    {
 *      return mdFree(ptr);
 *    }
 *    void *mdAllocatePrim(size_t size)
 *    {
 *      // own method to allocate space, original allocate()
 *    }
 *    void mdDeallocatePrim(void *ptr)
 *    {
 *      // own method to deallocate space, original free()
 *    }
 *    ~MyAllocator()
 *    {
 *      mdCleanup();
 *      // own destructor code
 *    }
 *  }
 * \endcode
 *
 * You must call mdCleanup() between the last allocation/deallocation and
 * destruction of the allocator. This method will free up delayed-free blocks
 * and check for memory leaks.
 *
 * Each library should also register pure virtual call handler using macro
 * MEMDBG_REGISTER_PURECALL in initialization code. It is no error to try
 * to register it more times.
 *
 * \see MemDbg_Config for info on configuration of the allocator and
 *  checks that will be made.
 */
class MEMDBG_EXPORTS MemDbg_Interface {
protected:
  /*!
   * \brief Create new debugger interface instance.
   *
   * \param config configuration.
   * \param hasIterator if \c true, the allocator supports iterating over blocks.
   */
  MemDbg_Interface(MemDbg_Config &config, bool hasIterator);
  
  /*!
   * \brief Destructor of the memory debugger interface.
   */
  virtual ~MemDbg_Interface();

  /*!
   * \brief Allocation routine wrapping original allocator routine.
   *
   * \param size size of the block to allocate.
   * \return pointer to newly-allocated block.
   */
  void *mdAllocate(size_t size);

  /*!
   * \brief Deallocation routine wrapping original deallocation routine.
   *
   * \param ptr pointer to deallocate.
   */
  void mdDeallocate(void *ptr);

  /*!
   * \brief Cleanup routine.
   *
   * This routine frees up not-yet-freed blocks in backlog and checks for memory
   * leaks. Eventual errors are reported. You should call it as the first call
   * in destructor. It is no error to call it more times.
   *
   * \param checkLeaks if true, check for memory leaks using mdCheckLeaks()
   *  (may be expensive). If your allocator supports optimized checking for
   *  memory leaks, override mdCheckLeaks() and use its functions instead.
   *
   * \return true, if any leaks found.
   *
   * \see mdCheckLeaks
   */
  bool mdCleanup(bool checkLeaks = true);

  /*!
   * \brief Free up all delay-free marked blocks.
   *
   * \param freeFreelist if true, free also underlying freelist (at cleanup).
   */
  void mdFreeDelayed(bool freeFreelist = false);

  /*!
   * \brief Check for memory leaks.
   *
   * This routine should be called when the allocator should be empty. It reports
   * any memory leaks (any left-over blocks). Default implementation uses iterator,
   * if available. If not, it will use information stored in doublekeeping (if any).
   *
   * Memory leaks will be printed out repeatedly if called again, except if
   * you implement mdMarkBadBlock() which marks the block bad in such a way that
   * it will not be reported by the iterator next time. This is called from
   * mdReportLeak().
   *
   * Default implementation routine is expensive. You should probably record
   * count or size of allocated/freed blocks and call it only if the count or size
   * doesn't match (or use your own leak-checking routine by overriding this
   * method). If you override the method, don't forget to call mdFreeDelayed()
   * to properly free up blocks from delay-free list to prevent reporting them
   * as memory leaks.
   *
   * If using default implementation, you must ensure external synchronization
   * on the allocator against parallel allocate/deallocate requests.
   *
   * \return true, if any leaks found.
   *
   * \see mdReportLeak
   *
   * \todo Make sure analyse checks for readable memory to prevent crashes.
   */
  virtual bool mdCheckLeaks();

  /*!
   * \brief Report memory leak to the user.
   *
   * This method also calls mdMarkBadBlock() to mark the block as bad to prevent
   * displaying it again.
   *
   * You may call this method directly from your own implementation of checking
   * for memory leaks.
   *
   * \param ptr pointer to the leaked block.
   *
   * \see mdCheckLeaks
   *
   * \todo Make sure analyse checks for readable memory to prevent crashes.
   */
  void mdReportLeak(void *ptr);

  /*!
   * \brief Report block with some message.
   *
   * This method reads debug header of the block (if any) and calls callback
   * function to display the block with any stack traces recorded with it.
   *
   * \param ptr block to report
   * \param message message to print in trace
   * \param curStack display also backtrace to this location
   *
   * \todo Make sure analyse checks for readable memory to prevent crashes.
   */
  void mdReportBlock(void *ptr, const char *message, bool curStack);

  /*!
   * \brief Mark block in allocator as "bad", so it won't be displayed when
   * checking for memory leaks.
   *
   * This method removes the block from double-keeping and sets debug header
   * in such a way that following mdReportLeak won't report it again. If no
   * debug header is present, then reporting the block again may be suppressed
   * by implementing mdMarkBadBlockPrim() for the allocator.
   *
   * \param ptr block to mark as bad.
   *
   * \see mdMarkBadBlockPrim
   */
  void mdMarkBadBlock(void *ptr);

  /*!
   * \brief Original allocation routine.
   *
   * \param size size in bytes to allocate.
   * \return address of the newly allocated block (16B-aligned).
   */
  virtual void *mdAllocatePrim(size_t size) = 0;

  /*!
   * \brief Original deallocation routine.
   *
   * \param ptr pointer to the block to free.
   */
  virtual void mdDeallocatePrim(void *ptr) = 0;

  /*!
   * \brief Mark block as bad in allocator (so it won't be reported at leak check).
   *
   * Implement this method to prevent repeated dump of leaked blocks in allocator.
   * Default implementation does nothing.
   *
   * \param ptr block to mark bad.
   *
   * \see mdMarkBadBlock
   */
  virtual void mdMarkBadBlockPrim(void *ptr) { }

  /*!
   * \brief Check if the block is currently in use.
   *
   * You don't have to implement this method. If you don't, inuse checks will
   * only be performed when double-keeping of used block is turned on.
   *
   * \param ptr block pointer to check.
   */
  virtual bool mdInUse(const void *ptr)
  {
    return true;
  }

  /*!
   * \brief Check free block for overwrites before re-allocation.
   *
   * If the allocator knows that the block has been previously deallocated,
   * it can call this method to check pattern and/or fences in block.
   *
   * \param ptr pointer to block start.
   * \param size size of the block.
   * \param skipBytes how many bytes in block are used to store
   *  allocator infos and shouldn't be checked.
   */
  virtual void mdCheckFree(const void *ptr, size_t size, size_t skipBytes = 0);

  /*!
   * \brief Get size of the allocated block.
   *
   * \param ptr block pointer.
   * \return size of the block (as allocated).
   */
  virtual size_t mdGetBlockSize(const void *ptr) = 0;

  /*!
   * \brief Get next allocated block in allocator (if supported).
   *
   * \param last [in] block (or NULL to start iterating).
   * \param size [out] size of the next block (0 if it was last).
   * \return next allocated block pointer or NULL if it was last.
   */
  virtual const void *mdGetNextBlock(const void *last, size_t &size);

  /*!
   * \brief Method called when cannot allocate memory (or invalid alignment from allocator).
   *
   * This method may return NULL or throw an exception.
   * Default implementation returns just NULL.
   */
  virtual void *mdBadAllocHandling();
public:
  /*!
   * \brief Get allocator name.
   */
  virtual const char *mdGetName() = 0;

  /*!
   * \brief Set global callback.
   *
   * This method will NOT overwrite the callback. If the callback is already
   * registered, you'll become old callback. This method is NOT thread-safe
   * and thus the caller must ensure he is the only one calling this method.
   *
   * \param cb callback to set.
   * \return NULL if no callback yet, old callback if already set.
   */
  static MemDbg_Callback *mdRegisterCallback(MemDbg_Callback *cb);

  /*!
   * \brief Query current callback.
   *
   * \return current callback or NULL if none registered.
   */
  inline static MemDbg_Callback *mdQueryCallback()
  {
    return m_callback;
  }

  /*!
   * \brief Get Purify running status.
   *
   * \return \c true, iff Rational Purify is currently running (only on Windows).
   */
  inline static bool mdIsPurifyRunning()
  {
    return purifyRunning;
  }
#ifdef WIN32
  /*!
   * \brief Register _purecall function to handle pure virtual function calls.
   *
   * This method registers _purecall function of given module to be handled
   * internally by the memory debugger. Calling pure virtual function will
   * result in a callback and then abort. You can log the message and/or
   * analyse the problem while in callback (in debug mode).
   *
   * This method works only on Win32 with VS 6.0 and VS .Net.
   *
   * \param purecallfnc must be _purecall.
   */
  static void mdRegisterPurecall(void *purecallfnc);
#endif

private:
  friend class MemDbg_Callback;

  // needed for pure virtual catching
  friend class MemDbg_PureInterface;

  enum {
    MD_MAX_DELAYED_COUNT  = 512   ///< Max. # of delayed free blocks.
  };

  /*!
   * \brief Decode block address.
   *
   * \note This routine may produce false hits for header (with very low probability).
   *
   * \param ptr block address (either from allocator or application pointer)
   * \param size block size (or 0 if not known)
   * \param header decoded header (or NULL if not debug block)
   * \param realBlock pointer to the beginning of the real allocator block
   * \return true, if ptr OK, false if ptr not aligned
   */
  bool decodeAddr(void *ptr, size_t size, MemDbg_Header* &header, void* &realBlock);

  /*!
   * \brief Decode block address.
   *
   * \note This routine may produce false hits for header (with very low probability).
   *
   * \param ptr block address (either from allocator or application pointer)
   * \param size block size (or 0 if not known)
   * \param header decoded header (or NULL if not debug block)
   * \param realBlock pointer to the beginning of the real allocator block
   * \return true, if ptr OK, false if ptr not aligned
   */
  bool decodeAddr(const void *ptr, size_t size, const MemDbg_Header* &header, const void* &realBlock);

  /*!
   * \brief Method called when an abstract method is called.
   */
  static void mdAbstractCall(void *arg);

  /*!
   * \brief Save the block into delayed freelist for later deallocation.
   *
   * \param ptr pointer to the allocated block (not real beginning).
   */
  void manageDelayFree(void *ptr);

  /*!
   * \brief Free oldest delayed free block.
   */
  void freeLastDelayFree();

  MemDbg_Config          &m_config;   ///< Configuration of this allocator.
  size_t                  m_freesize; ///< Currently held free size.
  size_t                  m_freestart;///< Start index of free blocks in free list.
  size_t                  m_freeend;  ///< End index of free blocks in free list.
  void*                  *m_freelist; ///< List of currently held free blocks.
  MemDbg_Lock            *m_instLock; ///< Instance lock on memory debugger.
  MemDbg_BlockHash       *m_usedBlks; ///< List of used blocks.
  bool                    m_hasIter;  ///< true, if has iterator.

  static MemDbg_Callback *m_callback; ///< Callback for various memory debugger actions.
  static MemDbg_Lock     *m_lock;     ///< Global lock on memory debugger.
  static void            *mdAbstractTable[];     ///< Pseudo-VTable for abstract call detection.
  static bool             purifyRunning;         ///< true, if running with Purify.
};

#endif  // __MEMDBG_INTERFACE_HPP
