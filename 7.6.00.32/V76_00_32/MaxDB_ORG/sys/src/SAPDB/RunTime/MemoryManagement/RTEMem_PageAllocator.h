/*!**************************************************************************

  module       : RTEMem_PageAllocator.h

  -------------------------------------------------------------------------

  responsible : J. Mensing (jrg)

  special area: RTE I/O Page Allocator
  description : C Interface for Singleton I/O Page allocation

  first created:2000-11-24  9:22
  last changed: 2000-11-24  9:27
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#ifndef RTEMEM_PAGEALLOCATOR_H
#define RTEMEM_PAGEALLOCATOR_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
  Function: RTEMem_InitializePageAllocator
  Description: Initialize the RTEMem_PageAllocator

  The database page size is dynamically setup. After it is known this call is used to
  collect memory in chunks of maximum size up to amount of memory wanted to be reserved
  for IO Pages. The allocatation reduces the chunk size each time, an allocation fails
  until either all IO Pages are allocated or a chunk is smaller than a database page.
  Minimum chunk size is system dependend handled via RTEMem_BlockAllocator.
  
  Arguments: dbPageSize [in] Size of a database page in bytes
             totalIOPages [in] total number of IO Pages needed
  Return value: true allocator initialized, false allocator not initialized
 */
#if defined (__cplusplus)
extern "C" {
#endif
extern bool RTEMem_InitializePageAllocator( SAPDB_Int4 dbPageSize,
                                            SAPDB_UInt4 totalIOPages);
#ifdef AIX
/*
  AIX forces usage of MAP_SHARED|MAP_ANONYMOUS. Therefore memory already allocated in watchdog process
  gets modified during allocation in running kernel. These reset routines reset the allocator instances
  so that 'en81_FamousLastWords' will not crash accidiently by using wrong freelist references...
 */
extern void RTEMem_Allocator_Reset();
extern void RTEMem_RteAllocator_Reset();
extern void RTEMem_RawAllocator_Reset();
extern void RTEMem_BlockAllocator_Reset();
extern void RTEMem_PageAllocator_Reset();
#endif

#if defined (__cplusplus)
}
#endif

#endif  /* RTEMEM_PAGEALLOCATOR_H */
