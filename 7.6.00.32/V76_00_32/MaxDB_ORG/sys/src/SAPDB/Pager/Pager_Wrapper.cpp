/****************************************************************************

  module      : Pager_Wrapper.cpp

  -------------------------------------------------------------------------

  responsible : Henrik

  special area: Pager_Wrapper
  description : 


  last changed: 2002-02-16
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Pager/Pager_Controller.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/* called from vbd01 */
externC void bd999EnableDataWriter(){
    (Pager_Controller::GetInstance()).EnablePagerWriter();
}

/* called from vbd20 */
externC void bd999FlushDataCache(
    tsp00_TaskId TaskId) {

    Pager_Controller::GetInstance().FlushDataCacheWithTempFromLRURechain(TaskId);
 }


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
