/*!
 * \file    MemDbg_Config.hpp
 * \author  IvanS
 * \brief   Memory debugger configuration.
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

#ifndef __MEMDBG_CONFIG_HPP
#define __MEMDBG_CONFIG_HPP

#include <stddef.h> // size_t

/*!
 * \brief Configuration for the memory debugger.
 *
 * Each allocator can have its own configuration.
 *
 * Performed checks:
 *  - post-fence check
 *  - allocation stack trace (to show allocation of block that has overwritten fences)
 *  - deallocation stack trace (needed to show deallocation of overwritten freed block)
 *  - check for pointer validity (double-keeping of allocated blocks)
 *  - fill new block with pattern (to detect uninitialized reads)
 *  - fill freed block with pattern and check pattern later
 *  - delayed freeing of blocks (with checking for overwrites of freed blocks)
 *  - checking of overwrites of freed blocks before re-allocating (needs cooperation from allocator)
 *  - setting of memory "color" for Purify (integration of own allocators with purify)
 */
class MemDbg_Config {
public:
  enum {
    FLG_INITIALIZE      = (1<<0),     ///< Initialize block to some pattern (0B).

    FLG_FENCE           = (1<<2),     ///< Create fences around the block (16B).
    FLG_DELAYFREE       = (1<<3),     ///< Delay freeing the block (8B).

    FLG_FREECHECK       = (1<<4),     ///< Check free space on actual free (after delayed free) (8B).
    FLG_DELAYFREECHECK  = (1<<5),     ///< Check free space on realloc, triggered from allocator (8B).
    FLG_QUICKFREECHECK  = (1<<6),     ///< If true, check only 2nd and 3rd word on free check.

    FLG_ALLOCTRACE      = (1<<7),     ///< Save stack trace on allocation (8B).
    FLG_FREETRACE       = (1<<8),     ///< Save stack trace on deallocation (8B).

    FLG_DOUBLEKEEP      = (1<<9),     ///< Double-keeping of used blocks in a hashtable (8B).

    /// The following can be packed into one 8B word:
    FLG_PACK  = FLG_ALLOCTRACE | FLG_FREETRACE | FLG_DOUBLEKEEP | FLG_FREECHECK |
                FLG_DELAYFREE | FLG_DELAYFREECHECK
  };

  MemDbg_Config(int flags, size_t maxsize) : m_flags(flags), m_noleaks(0), m_maxFreeSize(maxsize)
  {
  }

  /// Check for need for debugging infos.
  bool needsHeader() const
  {
    return ((m_flags & FLG_PACK) != 0) || (m_noleaks != 0);
  }

  /// Check for need for delayed free.
  bool isDelayFree() const
  {
    return (m_flags & FLG_DELAYFREE) != 0;
  }

  bool isNoLeak() const
  {
    return m_noleaks != 0;
  }

  bool isAllocTrace() const
  {
    return (m_flags & FLG_ALLOCTRACE) != 0;
  }

  bool isFreeTrace() const
  {
    return (m_flags & FLG_FREETRACE) != 0;
  }

  bool isInitialize() const
  {
    return (m_flags & FLG_INITIALIZE) != 0;
  }

  bool isFence() const
  {
    return (m_flags & FLG_FENCE) != 0;
  }

  bool isDoublekeep() const
  {
    return (m_flags & FLG_DOUBLEKEEP) != 0;
  }

  bool isDelayFreeCheck() const
  {
    return (m_flags & FLG_DELAYFREECHECK) != 0;
  }

  bool isFreeCheck() const
  {
    return (m_flags & FLG_FREECHECK) != 0;
  }

  bool isQuickFreeCheck() const 
  {
    return (m_flags & FLG_QUICKFREECHECK) != 0;
  }

  /// Get maximum delayed free size.
  size_t getMaxFreeSize() const
  {
    return m_maxFreeSize;
  }

  /// Turn on no-leaks option.
  void noLeaksOn()
  {
    ++m_noleaks;
  }

  /// Turn off no-leaks option.
  void noLeaksOff()
  {
    --m_noleaks;
  }

  /// Compute space needed for debug infos.
  int computeDbgSpace(size_t size) const;
private:
  int       m_flags;        ///< Allocator flags.
  int       m_noleaks;      ///< Counter for no-leaks.
  size_t    m_maxFreeSize;  ///< Maximum total free size in free queue.
};

#endif  // __MEMDBG_CONFIG_HPP
