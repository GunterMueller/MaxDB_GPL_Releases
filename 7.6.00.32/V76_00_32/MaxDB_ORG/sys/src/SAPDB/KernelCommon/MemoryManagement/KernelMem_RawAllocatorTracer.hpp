/*!
  @file           KernelMem_RawAllocatorTracer.hpp
  @author         ThomasA
  @ingroup        Memory Mangement
  @brief          Implementations a tracer for SAPDBMem_RawAllocator

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

#ifndef KERNELMEM_RAWALLOCATOR_HPP
#define KERNELMEM_RAWALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"

/*!
   @brief provides a tracer for messages written by a SAPDBMem_RawAllocator instance
 */

class KernelMem_RawAllocatorTracer
{
public :
    /*!
       @brief returns the tracer
     */
    static SAPDBMem_RawAllocator::ITracerAndExceptionHandler& GetTracer();
};

#endif