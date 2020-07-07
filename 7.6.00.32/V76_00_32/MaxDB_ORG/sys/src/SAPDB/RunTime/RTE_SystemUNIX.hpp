/*!
  @file           RTE_SystemUNIX.hpp
  @author         IvanS
  @ingroup        Runtime
  @brief          System support for UNIX only
  
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
#ifndef RTE_SYSTEMUNIX_HPP
#define RTE_SYSTEMUNIX_HPP

/*---------------------------------------------------------------------------*/

/*!
  @brief wrapper for memory allocation bookkeeping
  If memory was allocated from 'outside' (i.e. IPC shared memory or stack allocation
  the internal bookkeeping is not aware of it. This call make it aware.

  @param SizeInBytes [in] stack size
  @return true if memory allocation is allowed, false if limit would be exceeded
 */
SAPDB_Bool RTE_PseudoAllocMemory(SAPDB_ULong SizeInBytes);

/*---------------------------------------------------------------------------*/

/*!
  @brief wrapper for memory deallocation bookkeeping
  @param SizeInBytes [in] size of memory freed
  @return none
 */
void RTE_PseudoFreeMemory(SAPDB_ULong SizeInBytes);

#endif  /* RTE_SYSTEMUNIX_HPP */
