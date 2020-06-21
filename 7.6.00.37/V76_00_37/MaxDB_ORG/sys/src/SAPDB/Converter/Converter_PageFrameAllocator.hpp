/*!
  @file     Converter_PageFrameAllocator.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    This module offers the facility to request and release
            memory capable for converter page I/O operations.

            The Converter_PageFrameAllocator class is a convenient
            capsule of the component frame control, which is the only
            one manager for I/O capable memory.
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/

#ifndef CONVERTER_PAGEFRAMEALLOCATOR_HPP
#define CONVERTER_PAGEFRAMEALLOCATOR_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/


#include "KernelCommon/Kernel_Common.hpp"

#include "DataAccess/Data_PageFrame.hpp"

#include "RunTime/Tasking/RTETask_ITask.hpp"


/*!
   @class   Converter_PageFrameAllocator
   @brief   The Converter_PageFrameAllocator class is a convenient
            capsule of the component frame control, which is the only
            one manager for I/O capable memory.
 */
class Converter_PageFrameAllocator
{
public:

    /*!
        @brief  Constructor.
                Note that the given task Id is used to get resourcen like latches.
                Therefore it is needed that the running task is equal to the
                given taskId, if not the kernel will die.
        @param  task [in] own task
    */

    Converter_PageFrameAllocator( const RTETask_ITask  &task ) : m_Task( task )
    {}

    /*!
        @brief  Constructor.
                If no task id is given the task id will be determined. Note that
                the task id is used to get resourcen like latches. Therefore it is
                not allowed to give this handle from one task to another.
    */

    Converter_PageFrameAllocator() : m_Task( *RTETask_ITask::Self())
    {}

    /*!
       @brief   This method is used to get a piece of memory suitable to store
                a kernel page and to make I/O. If no more memory is available
                the componente frame control executes an emergency shutdown.
       @param   bIsRecoverable [in] count the allocation if the requested page
                                    frame is for a permanent converter page.
       @return  (Data_PageFrame) New page frame handle
     */

    Data_PageFrame New( const SAPDB_Bool bIsRecoverable = true );

    /*!
       @brief   This method is used to release a page frame which is not needed
                any more by a converter page handler. The given frame will be
                invalidated.
       @param   frame          [in] Handle of the page frame to release
       @param   bIsRecoverable [in] count the deallocation if the given page
                                    frame was used by a permanent converter page
       @return  none
     */
    void Free( Data_PageFrame    &frame,
               const SAPDB_Bool   bIsRecoverable = true );

private:

    ///task
    const RTETask_ITask &m_Task;
};


#endif  /* CONVERTER_PAGEFRAMEALLOCATOR_HPP */
