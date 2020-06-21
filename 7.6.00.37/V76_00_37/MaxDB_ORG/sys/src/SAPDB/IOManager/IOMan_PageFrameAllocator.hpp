/*!
  @file     IOMan_PageFrameAllocator.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module offers the facility to request and release
            memory capable for I/O management page operations.
            The IOMan_PageFrameAllocator class is a convenient 
            capsule of the component frame control, which is the only
            one manager for I/O capable memory.

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



#ifndef IOMAN_PAGEFRAMEALLOCATOR_HPP
#define IOMAN_PAGEFRAMEALLOCATOR_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "FrameControl/FrameCtrl_IIOManager.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "KernelCommon/Kernel_Common.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/* -----------------------------------------------------------------------*/
/*!
  @class    IOMan_PageFrameAllocator
  @brief    The IOMan_PageFrameAllocator class is a convenient 
            capsule of the component frame control, which is the only
            one manager for I/O capable memory.
 */
/* -----------------------------------------------------------------------*/


class IOMan_PageFrameAllocator
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get a piece of memory suitable to store
                a kernel page and to make I/O. If no more memory is available
                the component frame control executes an emergency shutdown.
       @param   taskId [in] Identification of the calling task
       @return  (Data_PageFrame) New page frame handle
     */
    /* -----------------------------------------------------------------------*/

    Data_PageFrame New( const tsp00_TaskId taskId )
    {
        return( FrameCtrl_IIOManager::GetInstance().NewIOManFrame( taskId ));
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to release a page frame which is not needed 
                any more. The given frame will be invalidated.
       @param   taskId [in] Identification of the calling task
       @param   frame [in] Handle of the page frame to release
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void Free( 
        const tsp00_TaskId      taskId, 
        Data_PageFrame &frame )
    {
        FrameCtrl_IIOManager::GetInstance().FreeIOManFrame( taskId, frame );
    }
};




#endif  /* IOMAN_PAGEFRAMEALLOCATOR_HPP */
