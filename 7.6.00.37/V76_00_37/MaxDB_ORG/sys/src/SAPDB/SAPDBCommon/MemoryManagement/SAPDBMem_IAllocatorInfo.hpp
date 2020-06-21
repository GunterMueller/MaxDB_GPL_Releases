/*!
  @file           SAPDBMem_IAllocatorInfo.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Interface for allocator statistics

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef SAPDBMEM_IALLOCATORINFO_HPP
#define SAPDBMEM_IALLOCATORINFO_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
@brief defines statistics interface of an allocator 
*/

class SAPDBMem_IAllocatorInfo
{
public : 
  /*!
     @brief          Outputs statistics about the memory usage of the allocator.
     @param          BytesUsed [out] The number of bytes used by this allocater.
     @param          BytesControlled [out] The number of bytes controlled by this allocator. 
                     The default implementation returns GetInvalidSize() for               all values.
     @return         none

   */

   virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                               SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled)
   { 
       BytesUsed       = GetInvalidSize();
       MaxBytesUsed    = GetInvalidSize();
       BytesControlled = GetInvalidSize();
   }

   /*!
      @brief          Outputs statistics about the number of Allocate and Deallocate calls of
                      the allocator to the underlying base allocator.
      @param          CountAlloc [out] The number of calls of method Allocate of the base allocator.
      @param          CountDealloc [out] The number of calls of method Deallocate of the base.
      @return         none

    */


   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                               SAPDB_ULong &CountDealloc) const = 0;

  /*!
     @brief          Outputs statistics about the number of Allocate and Deallocate calls.
     @param          CountAlloc [out] The number of calls of method Allocate.
     @param          CountDealloc [out] The number of calls of method Deallocate.
     @return         none

   */

   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const = 0;

   /*!
      @brief          Returns the number of errors found and corrected by the allocator
                      The derived class is responsible to implement this feature
      @return         integer
    */

  
   virtual SAPDB_ULong GetErrorCount() const { return 0; }

   /*!
      @brief          Returns the number of failed allocation attemps
                      The derived class is responsible to implement this feature (but must not do so...)
      @return         integer
    */

  
   virtual SAPDB_ULong GetAllocFailedCount() const { return 0; }

  /*!
     @brief          Returns the identifier associated with this allocator
                     This function is called to identify an allocator instance. The derived
                     class is responsible to implement this feature (but must not do so...)
     @return         const SAPDB_UTF8 *
   */

  
   virtual const SAPDB_UTF8 *GetIdentifier() const
      { return (const SAPDB_UTF8 *)""; }

   /*!
      @brief          Returns the value for an invalid size.
                      The is defined as 0xFFFF... .
      @return         SAPDB_ULong

    */

   static SAPDB_ULong GetInvalidSize()
      { return (((SAPDB_ULong)((SAPDB_Long)(-1)))); }
};

#endif
