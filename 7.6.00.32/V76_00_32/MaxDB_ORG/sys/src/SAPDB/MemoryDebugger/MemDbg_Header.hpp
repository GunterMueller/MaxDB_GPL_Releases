/*!
 * \file    MemDbg_Header.hpp
 * \author  IvanS,ChristophR
 * \brief   Memory debugger block header.
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

#ifndef __MEMDBG_HEADER_HPP
#define __MEMDBG_HEADER_HPP

#include <stddef.h> // size_t

/*!
 * \brief Memory debugger block header.
 *
 */
class MemDbg_Header {
public:
  enum {
    SIZE_BITS     = 14,                     ///< Bits/small size in block.
    MAX_SIZE      = (1 << SIZE_BITS) - 1,   ///< Maximum small size.
    BLKSIZE_SHIFT = 23,                     ///< Bits used by trace ID.
    SIZE_OVERFLOW = SIZE_BITS - (32 - BLKSIZE_SHIFT),///< Size bits in 2nd word.
    FLAG_SHIFT    = BLKSIZE_SHIFT + SIZE_OVERFLOW,///< Flag shift.
    TRACE_MASK    = (1<<BLKSIZE_SHIFT) - 1, ///< Mask for stack traces.
    FLAG_MASK     = -1 << FLAG_SHIFT,       ///< Mask for flags.
    NOLEAK        = TRACE_MASK,             ///< Special trace ID for no-leak blocks.

    FLAG_DBKP     = (1 << FLAG_SHIFT),      ///< Doublekeeping flag.
    FLAG_FENCE    = (1 << (FLAG_SHIFT + 1)),///< Post-fence flag.
    FLAG_FREECHK  = (1 << (FLAG_SHIFT + 2)),///< Free check.
    FLAG_QUICKCHK = (int)(1UL << (FLAG_SHIFT + 3)) ///< Quick free check (2nd and 3rd word).
  };

  /// Initialize header.
  void init()
  {
    w1 = w2 = 0;
  }

  /*!
   * \brief Get allocation trace.
   *
   * \return ID of the allocation trace or 0.
   */
  int getAllocTrace() const
  {
    return w1 & TRACE_MASK;
  }

  /*!
   * \brief Get deallocation trace.
   *
   * \return ID of the deallocation trace or 0.
   */
  int getFreeTrace() const
  {
    return w2 & TRACE_MASK;
  }

  /*!
   * \brief Get allocated block size.
   *
   * \param subtract if true, use address below header, if false, after header
   *  to find big block size.
   * \return block size
   */
  size_t getSize(bool subtract) const
  {
    size_t ret = (w1 >> BLKSIZE_SHIFT) |
      (((w2 & ~FLAG_MASK) >> BLKSIZE_SHIFT) << (32-BLKSIZE_SHIFT));
    if (ret == 0) {
      if (subtract) {
        return *reinterpret_cast<const size_t*>(this - 1);
      } else {
        return *reinterpret_cast<const size_t*>(this + 1);
      }
    } else {
      return ret;
    }
  }

  /// Check if double-keeping on.
  bool isDoubleKeep() const
  {
    return (w2 & FLAG_DBKP) != 0;
  }

  /// Check if fence on.
  bool isFence() const
  {
    return (w2 & FLAG_FENCE) != 0;
  }

  /*!
   * \brief Set allocation trace.
   *
   * \param id ID of the allocation trace or 0.
   */
  void setAllocTrace(int id)
  {
    w1 = id | (w1 & ~TRACE_MASK);
  }

  /*!
   * \brief Set deallocation trace.
   *
   * \param id ID of the deallocation trace or 0.
   */
  void setFreeTrace(int id)
  {
    w2 = id | (w2 & ~TRACE_MASK);
  }

  /*!
   * \brief Set block size.
   *
   * \param blksize block size.
   * \param subtract if true, use address below header, if false, after header
   *  to store big block size.
   */
  void setSize(size_t blksize, bool subtract)
  {
    if (blksize > MAX_SIZE) {
      if (subtract) {
        *reinterpret_cast<size_t*>(this - 1) = blksize;
      } else {
        *reinterpret_cast<size_t*>(this + 1) = blksize;
      }
      blksize = 0;
    }
    w1 = (w1 & TRACE_MASK) | (blksize << BLKSIZE_SHIFT);
    w2 = (w2 & (TRACE_MASK | FLAG_MASK)) |
      (((blksize >> (32 - BLKSIZE_SHIFT)) << BLKSIZE_SHIFT) & ~(TRACE_MASK | FLAG_MASK));
  }

  /// Set fence flag.
  void setFence()
  {
    w2 |= FLAG_FENCE;
  }

  /// Clear fence flag.
  void clearFence()
  {
    w2 &= ~FLAG_FENCE;
  }

  /// Set doublekeeping flag.
  void setDoublekeep()
  {
    w2 |= FLAG_DBKP;
  }

  /// Clear doublekeeping flag.
  void clearDoublekeep()
  {
    w2 &= ~FLAG_DBKP;
  }

  /// Set free check flag.
  void setFreeCheck()
  {
    w2 |= FLAG_FREECHK;
  }

  /// Clear free check flag.
  void clearFreeCheck()
  {
    w2 &= ~FLAG_FREECHK;
  }

  /// Check for quick free check flag.
  bool isQuickCheck() const
  {
    return (w2 & FLAG_QUICKCHK) != 0;
  }

  /// Set quick free check flag.
  void setQuickCheck()
  {
    w2 |= FLAG_QUICKCHK;
  }

  /// Clear quick free check flag.
  void clearQuickCheck()
  {
    w2 &= ~FLAG_QUICKCHK;
  }
private:
  // underlying structure
  unsigned int w1;
  unsigned int w2;
};

#endif  // __MEMDBG_HEADER_HPP
