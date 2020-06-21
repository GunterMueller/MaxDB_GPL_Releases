/*!
  @file     FrameCtrl_IDataCache.hpp
  @author   TorstenS
  @ingroup  FrameControl
  @brief    Interface to access io capable memory for the DataCache component.

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



#ifndef FRAMECTRL_IDATACACHE_HPP
#define FRAMECTRL_IDATACACHE_HPP



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
    @interface  FrameCtrl_IDataCache
    @brief      This interface is used to allocate and deallocate io capable
                memory for the component DataCache. The size of the memory
                frames is given by the data base coniguration parameter
                page size.
 */


class FrameCtrl_IDataCache
{
public:

    /*!
        @brief  This method returns the access to the FrameCtrl_IDataCache 
                interface of component FrameControl. It is assumed that the 
                FrameControl singleton is available! No CreateInstance call 
                will be done.
        @return Access to the FrameCtrl_IDataCache interface
     */

    static FrameCtrl_IDataCache& GetInstance();

    /*!
       @brief   This method is used to provide a data page frame.
                If no more memory is available the component frame control 
                executes an emergency shutdown.
       @param   taskId [in] Identification of calling task
       @return  (Data_PageFrame) New page frame handle
     */

    virtual Data_PageFrame NewDataFrame( 
        const tsp00_TaskId taskId ) = 0;

    /*!
       @brief   This method is used to release a data page frame.
       @param   taskId [in] Identification of calling task
       @param   frame  [in] Handle of the page frame to release
     */
    
    virtual void FreeDataFrame(  
        const tsp00_TaskId      taskId,
        Data_PageFrame &frame ) = 0;
};

#endif  /* FRAMECTRL_IDATACACHE_HPP */
