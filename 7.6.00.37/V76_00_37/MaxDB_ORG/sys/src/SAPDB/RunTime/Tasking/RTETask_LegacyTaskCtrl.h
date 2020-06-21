/*!========================================================================

  @file         RTETask_LegacyTaskCtrl.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access Task object methodes from old C-Code
                via extern "C" functions

  @since        2003-10-28  16:48
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


#ifndef RTETASK_LEGACYTASKCTRL_H
#define RTETASK_LEGACYTASKCTRL_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include         "geo50_0.h"

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
@brief         Get the task doubly linked queue element.

               This queue element is part of the task object.
               In the past the element named qa or SpecDLQElem1
               was part of the task control.

    @param     pTaskCtrl     [in]     - Task control
*/
externC DLQ_ELEMENT*  GetTaskDLQElement1       ( TASK_CTRL   pTaskCtrl );

/*!
@brief         Get the task doubly linked queue element.

               This queue element is part of the task object.
               In the past the element named qb or SpecDLQElem2
               was part of the task control.

    @param     pTaskCtrl     [in]     - Task control
*/
externC DLQ_ELEMENT*  GetTaskDLQElement2       ( TASK_CTRL   pTaskCtrl );

externC void RTETask_ProcessDebugTaskRequest ( UKT_CTRL *ppUKT );

#endif  /* RTETASK_LEGACYTASKCTRL_H */
