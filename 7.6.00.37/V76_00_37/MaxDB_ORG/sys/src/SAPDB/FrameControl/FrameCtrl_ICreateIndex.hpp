/*!
  @file     FrameCtrl_ICreateIndex.hpp
  @author   TorstenS
  @ingroup  FrameCrontrol
  @brief    Interface to access io capable memory for the parallel
            index creation.

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



#ifndef FRAMECTRL_ICREATEINDEX_HPP
#define FRAMECTRL_ICREATEINDEX_HPP


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
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class RTETask_ITask;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @interface   FrameCtrl_ICreateIndex
  @brief       This interface is used to allocate and deallocate io capable
               memory for the parallel index creation. The size of the memory
               frames is given by the data base coniguration parameter
               page size.
*/

class FrameCtrl_ICreateIndex
{
public:

    /*!
        @brief  This method returns the access to the FrameCtrl_ICreateIndex 
                interface of component FrameControl. It is assumed that the 
                FrameControl singleton is available! No CreateInstance call 
                will be done.
        @return Access to the FrameCtrl_ICreateIndex interface
     */

    static FrameCtrl_ICreateIndex& GetInstance();

    /*!
       @brief   This method is used to get a page frame. If no more memory
                is available an invalid frame is returned.
       @param   task [in] Identification of calling task
       @return  (Data_PageFrame) new page frame handle
     */

    virtual Data_PageFrame NewIndexFrame( RTETask_ITask&  task ) = 0;

    /*!
       @brief   This method is used to release a page frame.
       @param   task      [in] Identification of calling task
       @param   frame [in|out] Handle of the page frame to release
       @return  none
     */

    virtual void FreeIndexFrame(
       RTETask_ITask&  task,
       Data_PageFrame& frame ) = 0;

};

#endif  /* FRAMECTRL_ICREATEINDEX_HPP */
