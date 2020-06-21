/*!
  @file     FrameCtrl_IBlockAllocator.hpp
  @author   TorstenS
  @ingroup  FrameCrontrol
  @brief    Interface to access io capable memory for the block allocator.
  @see            

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



#ifndef FRAMECTRL_IBLOCKALLOCATOR_HPP
#define FRAMECTRL_IBLOCKALLOCATOR_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_PageFrame.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

 /*!
   @interface   FrameCtrl_IBlockAllocator
   @brief       This interface is used to allocate and deallocate io capable
                memory for the block allocator. The size of the memory
                frames is given by the data base coniguration parameter
                page size.
 */

class FrameCtrl_IBlockAllocator : public SAPDBMem_IBlockAllocator
{
public:

    /*!
        @brief  This method returns the access to the FrameCtrl_IBlockAllocator 
                interface of component FrameControl. It is assumed that the 
                FrameControl singleton is available! No CreateInstance call 
                will be done.
        @return Access to the FrameCtrl_IBlockAllocator interface
     */

    static FrameCtrl_IBlockAllocator& GetInstance();
    
    /*!
       @brief   This method is used to get a page frame. If no more memory
                is available the kernel executes an emergency shutdown as long
                as the given flag bStopIfMemoryExhausted is set to true. If the
                flag is set to false and no more memory is available an invalid
                frame is returned.
       @param   taskId                 [in] Identification of calling task
       @param   bStopIfMemoryExhausted [in] Stop kernel if memory is exhausted
       @return  (Data_PageFrame) new page frame handle
     */

    virtual Data_PageFrame NewBlockAllocatorFrame( 
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bStopIfMemoryExhausted = true ) = 0;

    /*!
       @brief   This method is used to release a give page frame.
       @param   taskId      [in] Identification of calling task
       @param   frame       [in] Handle of the page frame to release
       @param   bIsShutdown [in] Memory is given backup, because of system shutdown
       @return  none
     */
    
    virtual void FreeBlockAllocatorFrame(  
        const tsp00_TaskId      taskId,
        Data_PageFrame &frame,
        const SAPDB_Bool        bIsShutdown = true ) = 0;
};

#endif  /* FRAMECTRL_IBLOCKALLOCATOR_HPP */