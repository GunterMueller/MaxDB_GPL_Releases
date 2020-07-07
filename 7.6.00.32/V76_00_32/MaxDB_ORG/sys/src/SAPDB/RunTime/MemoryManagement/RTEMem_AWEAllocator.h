/*!
  @file           RTEMem_AWEAllocator.h
  @author         RaymondR
  @ingroup        RunTime
  @brief          Address Windowing Extensions (AWE)

    AWE provides 32-bit applications to address more than 32-bit regions
    of physical memory.

    AWE is supported on all platforms supported by Windows 2000, including
    the 64-bit version of Windows 2000. 

    Windows 2000 (x86) provides applications with a flat 32-bit virtual 
    address space, which describes 4 GB of virtual memory. This address space 
    is usually splitted into two parts. The first part (2GB of address space) 
    is directly accessible to the application, the other 2GB memory is reserved 
    for the operating system.
    On Windows 2000 Advanced Server x86 systems an 'boot.ini' option (/3GB) 
    is provided, so that the operating system reserves only 1 GB of the address 
    space, and the application can access up to 3 GB.

    AWE solves the problem accessing more than 3GB of memory by allowing 
    applications to directly address huge amounts of physical memory while 
    continuing to use 32-bit pointers (where sufficient physical memory is 
    present). 

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
#ifndef RTEMEM_AWEALLOCATOR_H
#define RTEMEM_AWEALLOCATOR_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined(__cplusplus)

# include "SAPDBCommon/SAPDB_Types.h"

 /*! 
    @brief  Allocates one mapping page and mappes a physical page into it.

    @see RTEMem_AWEAllocator::AWEAvailable
   */
extern bool          RTEMem_AWEAvailable              ();

/*!
   @brief Release instance
   @return none
 */
extern void          RTEMem_AWEReleaseInstance        ();

#endif

#endif  /* RTEMEM_AWEALLOCATOR_H */
