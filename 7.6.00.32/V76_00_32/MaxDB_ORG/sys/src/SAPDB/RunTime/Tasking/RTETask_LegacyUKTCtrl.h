/*!========================================================================

  @file         RTETask_LegacyUKTCtrl.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access TaskScheduler object methodes from old C-Code
                via extern "C" functions

  @since        2003-09-19  11:04
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef RTETASK_LEGACYUKTCTRL_H
#define RTETASK_LEGACYUKTCTRL_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "geo50_0.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*!
@brief         Update the current task member variable of the 
               TaskScheduler

               Its called inside the legacy dispatcher after a
               new task control has been assign to the UKT.

@param         pUKT     [in]     - UKT control
*/
externC void  UpdateCurrentTaskInTaskScheduler( UKT_CTRL   pUKT );

/*!
@brief         Number of elements in TaskSchedulers communication queue

@param         pUKT     [in]     - UKT control

@return        number of elements
*/
externC SAPDB_UInt4  NumberOfCommunicationQueueElements( UKT_CTRL   pUKT );

#endif  /* RTETASK_LEGACYUKTCTRL_H */
