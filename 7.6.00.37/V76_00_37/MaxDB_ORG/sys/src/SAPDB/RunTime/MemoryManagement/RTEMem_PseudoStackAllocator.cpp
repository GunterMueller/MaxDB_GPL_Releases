/****************************************************************************

  module      : RTEMem_PseudoStackAllocator.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Pseudo Allocator for statistics about stack space consumption
  description : Singleton based on SAPDBMem_PseudoAllocator

                This allocator allows the system view ALLOCATOR_STATISTICS to 
                display stack space consuption for all task stacks and the 
                internal thread stacks too.
                
  last changed: 2000-12-06  19:05
  see also    : RTEMem_Services.html
  first created:2000-03-09  18:21

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

#   FreeBSD portions added by Kai Mosebach, 
#   For more informations see : http://www.komadev.de/sapdb
#

*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/MemoryManagement/RTEMem_PseudoStackAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/

  /*! 
    function: Instance

    description: Return reference to single instance of RTEMem_Allocator

    If never called before, default parameters are used to setup allocator.

    return value: Reference to RTEMem_Allocator instance
   */
SAPDBMem_SynchronizedPseudoAllocator & RTEMem_PseudoStackAllocator::Instance( )
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_PseudoStackAllocator));

  if ( !m_Instance )
  {
      m_Instance = new (Space) RTEMem_PseudoStackAllocator();
  }

  return *m_Instance;
}

RTEMem_PseudoStackAllocator * RTEMem_PseudoStackAllocator::m_Instance     = NULL;

/*---------------------------------------------------------------------------*/

// private allocator only used once during initialization
RTEMem_PseudoStackAllocator::RTEMem_PseudoStackAllocator()
: SAPDBMem_SynchronizedPseudoAllocator((const SAPDB_UTF8 *)"StackSpace", (const SAPDB_UTF8 *)"SystemHeap")
{
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
