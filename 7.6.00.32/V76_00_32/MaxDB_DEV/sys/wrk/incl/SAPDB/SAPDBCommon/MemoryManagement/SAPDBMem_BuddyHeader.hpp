/*!
  @file           SAPDBMem_BuddyHeader.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Part of implementation of a 'buddy allocator'.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



\endif
*/


#ifndef SAPDBMEM_BUDDYHEADER_HPP
#define SAPDBMEM_BUDDYHEADER_HPP

#include <stddef.h>

/*!
  @class          SAPDBMem_BuddyHeader
  @brief          Header class for memory chunks managed by the
                  'SAPDBMem_BuddyAllocator'.

 */


class SAPDBMem_BuddyHeader
{
private:
   /*!
      @brief The status that a memory chunk can have.
    */

   enum {free = 0, reserved = 1}; 
   

   /*!
      @brief Log2 of size of chunk, including the header.

             Memory chunks are always sized as a power of 2.
             The size and the status of the chunk are stored in one
             member of type unsigned in. All but one bits are used for 
             the size.
    */

   unsigned int m_Log2Size : 
      static_cast<unsigned int>(sizeof(unsigned int) * 8) - 1;
   

   /*!
      @brief The status (free/reserved) that a memory chunk can have.
             One bit is used to store the status.
    */

   unsigned int m_Status : 1; 
   

public: 
   /*!
      @brief Default constructor.
    */

   SAPDBMem_BuddyHeader()
    : m_Status(reserved), m_Log2Size(0)
   {}

   /*!
      @brief Sets the log2 of the size of the memory chunk
             (including the header) described by this header.
    */

   void SetLog2Size(unsigned int Log2Size)
      { m_Log2Size = Log2Size; }

   /*!
      @brief Increments the log2 of the size.
    */

   void IncrementLog2Size()
      { ++m_Log2Size; }

   /*!
      @brief Decrements the log2 of the size.
    */

   void DecrementLog2Size()
      { --m_Log2Size; }

   /*!
      @brief Returns the log2 of the size of the memory chunk
             (including the header) described by this header.
    */

   unsigned int GetLog2Size() const
      { return m_Log2Size; }

   /*!
      @brief Returns the size of the memory chunk (including
             the header) described by this header.
    */

   ptrdiff_t GetSize() const
      { return (static_cast<ptrdiff_t>(1) << m_Log2Size); }

   /*!
      @brief  Sets the status of the memory chunk described by
             this header to 'free'.
    */

   void SetFree()
      { m_Status = free; }

   /*!
      @brief  Sets the status of the memory chunk described by
             this header to 'reserved'.
   */

   void SetReserved()
      { m_Status = reserved; }

   /*!
      @brief Returns whether the memory chunk described
             by this header is free or not.
    */

   bool IsFree() const
      { return (m_Status == free); }

};



#endif

