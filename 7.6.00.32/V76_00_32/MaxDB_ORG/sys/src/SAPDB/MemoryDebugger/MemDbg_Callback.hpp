/*!
 * \file    MemDbg_Callback.hpp
 * \author  IvanS
 * \brief   Memory debugger callback.
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

#ifndef __MEMDBG_CALLBACK_HPP
#define __MEMDBG_CALLBACK_HPP

#include "MemoryDebugger/MemDbg_Exports.hpp"
#include <stddef.h> // size_t

class MemDbg_Interface;

/*-----------------------------------------------------*/

/// Helper class - exclusive lock.
class MemDbg_Lock {
public:
  /// Lock the lock object, waiting if necessary.
  virtual void lock() = 0;
  /// Unlock the lock object.
  virtual void unlock() = 0;
};

/*-----------------------------------------------------*/

/// Helper class - locked scope.
class MemDbg_LockedScope {
public:
  MemDbg_LockedScope(MemDbg_Lock *lck) : m_lck(lck)
  {
    if (lck) {
      lck->lock();
    }
  }

  ~MemDbg_LockedScope()
  {
    if (m_lck) {
      m_lck->unlock();
    }
  }

  void unlock()
  {
    if (m_lck) {
      m_lck->unlock();
      m_lck = 0;
    }
  }

private:
  MemDbg_Lock *m_lck;
};

/*-----------------------------------------------------*/

/// Callback.
class MEMDBG_EXPORTS MemDbg_Callback {
public:
  /*!
   * \brief Get stack trace tracing current location.
   *
   * This method should fill in the traceptr with pointers to stack
   * frames up to current location.
   *
   * Default implementation makes fastest possible trace, however,
   * it works only on some platforms (for now Win32). Override this
   * method to implement tracing on other platforms.
   *
   * \param traceptr array to receive traced frames.
   * \param levels array size (maximum stack depth).
   * \return count of filled frames.
   */
  virtual size_t mdFillTrace(void **traceptr, size_t levels)
  {
    return implFillTrace(traceptr, levels);
  }

  /*!
   * \brief Get stack trace index tracing current location.
   *
   * This method should return an identificator for stack trace to current
   * location. One possibility is to hold all stacktraces in hashtable and
   * when a new trace is created, put this new one into hash and return
   * index of the new trace.
   *
   * Default implementation keeps the data in a hash table. Trace ID is
   * always incremented when there comes an unknown trace.
   *
   * \return trace index to the current location (>0) as a 4B integer.
   */
  virtual int mdGetTraceID()
  {
    return implGetTraceID();
  }

  /*!
   * \brief Get trace belonging to given ID.
   *
   * \param id trace ID.
   * \param data where to store trace pointers.
   * \param dataSize size of data array.
   * \return count of stored levels.
   */
  virtual size_t mdGetTrace(int id, void **data, size_t dataSize)
  {
    return implGetTrace(id, data, dataSize);
  }

  /*!
   * \brief Check if the pointer points into the code segment (for VTable checking).
   *
   * \param ptr pointer to check.
   * \return \c true, if the pointer points to code segment.
   */
  virtual bool mdInCodeSegment(const void *ptr)
  {
    return false;
  }

  /*!
   * \brief Debug allocator callback.
   *
   * \param msg message from allocator.
   * \param allocator allocator interface pointer (may be NULL) which caused the call.
   * \param blk pointer to the failed block (may be NULL).
   * \param size size of the block (may be 0 if not possible to get or address invalid).
   * \param allocTrace trace to the allocation location of the block (or 0 if not known).
   * \param freeTrace trace to the deallocation location of the block (or 0 if not known).
   *    This can be previous deallocation if the block is still in use.
   * \param currentTrace trace to the current location (if it makes sense, 0 otherwise).
   */
  virtual void mdAllocatorMessage(const char *msg, MemDbg_Interface *allocator, const void *blk,
    size_t size, int allocTrace, int freeTrace, int currentTrace) = 0;

  /*!
   * \brief Called by the memory debugger when it cannot allocate resources for itself.
   *
   * This method MAY NOT RETURN and may not call memory debugger either directly or
   * indirectly! It should only write log entry (e.g., by calling mdAllocatorMessage)
   * and then abort the program.
   *
   * \param msg detail message.
   */
  virtual void mdAbort(const char *msg) = 0;

  /*!
   * \brief Abstract method callback.
   *
   * This callback becomes *potential* information about a block that caused
   * abstract method call. The information *may* be correct, but it also may
   * contain totally wrong data. In any case, you may write a stack trace
   * leading to this abstract call.
   *
   * \param blk potential object this pointer (may be NULL).
   * \param vtable potential VTable of the object (normally interface with abstract methods; may be NULL).
   * \param allocTrace potential allocation trace of the object (if known; may be invalid).
   * \param size potential block size (if known; may be invalid).
   * \param freeTrace potential freeing trace of the object (if known; may be invalid).
   */
  virtual void mdAbstractCall(const void *blk, const void *vtable, size_t size, int allocTrace, int freeTrace) = 0;

  virtual void mdInvalidFree(MemDbg_Interface *allocator, const void *blk, size_t size, int allocTrace, int freeTrace)
  {
    mdAllocatorMessage("Freeing invalid block", allocator, blk, size,
      allocTrace, freeTrace, mdGetTraceID());
  }

  virtual void mdNotInUseFree(MemDbg_Interface *allocator, const void *blk, size_t size, int allocTrace, int freeTrace)
  {
    mdAllocatorMessage("Freeing not in use block", allocator, blk, size,
      allocTrace, freeTrace, mdGetTraceID());
  }

  virtual void mdPostFenceOverwritten(MemDbg_Interface *allocator, const void *blk, size_t size, int allocTrace, int freeTrace)
  {
    mdAllocatorMessage("Fence after the block overwritten", allocator, blk, size,
      allocTrace, freeTrace, 0);
  }

  virtual void mdFreeDataOverwritten(MemDbg_Interface *allocator, const void *blk, size_t size, int allocTrace, int freeTrace)
  {
    mdAllocatorMessage("Free data overwritten", allocator, blk, size,
      allocTrace, freeTrace, 0);
  }

  /*!
   * \brief Create lock for synchronizing internal structures of memory debugger.
   *
   * This method is guaranteed to be called single-user. The user must create some
   * sort of lock (e.g., spinlock or mutex) that memory debugger can use to
   * synchronize its internal state.
   *
   * \return newly allocated lock instance.
   */
  virtual MemDbg_Lock *mdCreateLock() = 0;

  /*!
   * \brief Destroy synchronization lock.
   *
   * \param lock lock to destroy.
   */
  virtual void mdDestroyLock(MemDbg_Lock *lock) = 0;

  /*!
   * \brief Allocate memory from the system.
   *
   * This allocation is used to allocate memory in Purify case as well as to
   * allocate memory for internal structures. Default calls malloc().
   *
   * \param size block size in bytes.
   * \return newly allocated block address.
   */
  virtual void *mdSystemAllocate(size_t size);

  /*!
   * \brief Deallocate system memory.
   *
   * Default calls free().
   *
   * \param ptr pointer to deallocate.
   */
  virtual void mdSystemFree(void *ptr);

private:
  /// Implementation for stacktrace.
  static size_t implFillTrace(void **traceptr, size_t levels);
  static size_t implGetTrace(int id, void **data, size_t dataSize);
  int implGetTraceID();
};

#endif  // __MEMDBG_CALLBACK_HPP
