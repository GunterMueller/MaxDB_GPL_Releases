/*!
  @file     FrameCtrl_ILog.hpp
  @author   TorstenS
  @ingroup  FrameControl
  @brief    Interface to access io capable memory for the log component.

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



#ifndef FRAMECTRL_ILOG_HPP
#define FRAMECTRL_ILOG_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_PageFrame.hpp"

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
   @interface  FrameCtrl_ILog
   @brief      This interface is used to allocate and deallocate io capable
               memory for the component log. The size of the memory
               frames is given by the data base coniguration parameter
               page size.
*/

class FrameCtrl_ILog
{
public:

    /*!
        @brief  This method returns the access to the FrameCtrl_ILog 
                interface of component FrameControl. It is assumed that the 
                FrameControl singleton is available! No CreateInstance call 
                will be done.
        @return Access to the FrameCtrl_ILog interface
     */

    static FrameCtrl_ILog& GetInstance();

    /*!
       @brief   This method is used to get a log page frame. If no more memory
                is available the kernel executes an emergency shutdown as long
                as the given flag bStopIfMemoryExhausted is set to true. If the
                flag is set to false and no more memory is available an invalid
                frame is returned.
       @param   taskId                 [in] Identification of calling task
       @param   bStopIfMemoryExhausted [in] Stop kernel if memory is exhausted
       @return  (Data_PageFrame) new page frame handle
     */

    virtual Data_PageFrame NewLogFrame(
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bStopIfMemoryExhausted = true ) = 0;

    /*!
        @brief  This method is used to release a log page frame.
        @param  taskId      [in] Identification of calling task
        @param  frame       [in] Handle of the page frame to release
        @param   bIsShutdown [in] Memory is given backup, because of system shutdown
     */

    virtual void FreeLogFrame(
        const tsp00_TaskId      taskId,
        Data_PageFrame &frame,
        const SAPDB_Bool        bIsShutdown = true ) = 0;
};

#endif  /* FRAMECTRL_ILOG_HPP */
