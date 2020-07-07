/*!
  @file           RTEMem_AllocatorInfoReader.hpp
  @author         UlrichJ
  @ingroup        Runtime
  @brief          RTE Memory Allocator Statistics Info Reader

    This is a Reader class for sequential reading of the memory allocator information.
    For more information about how to use this reader class, please take a look at: 
    SysView_MemoryAllocatorStatistics.cpp -> Execute()

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
#ifndef RTEMEM_ALLOCATORINFOREADER_HPP
#define RTEMEM_ALLOCATORINFOREADER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "Container/Container_List.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! container for list of AllocatorInfoItems */
typedef Container_List<RTEMem_AllocatorRegister::StatisticInfo> RTEMem_AllocatorInfoList;

/*!
  @class RTEMem_AllocatorInfoReader
  @brief Helper class to iterate through allocator infos
 */
class RTEMem_AllocatorInfoReader
{
public:
    /* -----------------------------------------------------------------------*/
    /*  chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief ctor
      @param mAlloc [in] raw allocator to use
     */
    RTEMem_AllocatorInfoReader (SAPDBMem_IRawAllocator &mAlloc);
    
    /*!
      @brief dtor
     */
    ~RTEMem_AllocatorInfoReader (void);

    /* -----------------------------------------------------------------------*/
    /*  endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*  chapter: Iteration                                                    */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief reset current to first item

      Use 'Next' member function to retrieve the first item

      @return true if any items, false if empty list
     */
    SAPDB_Bool First (void);

    /*!
      @brief set current to next item
      @return true if more items, false if at end of list
     */
    SAPDB_Bool Next (RTEMem_AllocatorRegister::StatisticInfo * &pInfo);

    /* -----------------------------------------------------------------------*/
    /*  endchapter: Iteration                                                 */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*  chapter: Data Access                                                  */
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*  endchapter: Data Access                                               */
    /* -----------------------------------------------------------------------*/


private:

    void FreeAll (void);

    SAPDBMem_IRawAllocator              &m_Allocator; ///< the internally used allocator
    RTEMem_AllocatorInfoList            *m_List;      ///< the list of allocator infos
    RTEMem_AllocatorInfoList::Iterator  m_Current;    ///< current iterator
        
};

/*!
   @endclass: RTEMem_AllocatorInfoReader
 */

#endif  /* RTEMEM_ALLOCATORINFOREADER_HPP */

