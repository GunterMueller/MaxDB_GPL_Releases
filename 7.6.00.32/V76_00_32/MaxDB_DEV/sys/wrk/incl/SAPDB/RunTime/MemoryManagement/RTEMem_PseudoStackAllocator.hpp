/*!
  @file           RTEMem_PseudoStackAllocator.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Pseudo Allocator for statistics about stack space consumption
  
                  Singleton based on SAPDBMem_SynchronizedPseudoAllocator

                  This allocator allows the system view ALLOCATOR_STATISTICS to 
                  display stack space consuption for all task stacks and the 
                  internal thread stacks too.
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
#ifndef RTEMEM_PSEUDOSTACKALLOCATOR_HPP
#define RTEMEM_PSEUDOSTACKALLOCATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_PseudoAllocator.hpp"
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! 
  @class RTEMem_PseudoStackAllocator

  @brief Basic synchronised allocator for stack space consuming statistic

  The allocator only produces statistic information visible in systemview ALLOCATOR_STATISTICS
 */
class RTEMem_PseudoStackAllocator : public SAPDBMem_SynchronizedPseudoAllocator
{
public:
  /*!
    @brief Return reference to single instance of RTEMem_Allocator

    The InitializeInstance function must have been called before.
    
    @return Reference to RTEMem_Allocator instance
   */
  static SAPDBMem_SynchronizedPseudoAllocator & Instance( );

private:
  // private allocator only used once during initialization
  RTEMem_PseudoStackAllocator();

  static RTEMem_PseudoStackAllocator *m_Instance; //!< the singleton instance
}; /*! endclass: RTEMem_PseudoStackAllocator */

#endif  /* RTEMEM_PSEUDOSTACKALLOCATOR_HPP */
