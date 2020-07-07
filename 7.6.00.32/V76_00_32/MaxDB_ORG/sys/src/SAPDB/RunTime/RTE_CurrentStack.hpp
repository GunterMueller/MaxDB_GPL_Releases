/*!***************************************************************************

  module      : RTE_CurrentStack.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RunTime
  description : Stack Checking


  last changed: 2001-07-13  12:00
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


#ifndef RTE_CURRENTSTACK_HPP
#define RTE_CURRENTSTACK_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"

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
  Function: RTE_CurrentStackUsage
  Description: Returns current amount of stack consumed

  This uses the internal knowledge of the stack bottom and return
  the difference between the address of a local variable and the stack bottom.
  Arguments: taskId [in] optional task id (if not known, it is determined)
  Return value: Number of bytes consumed on current stack
 */
SAPDB_UInt4 RTE_CurrentStackUsage(RTE_TaskId taskId=0);

/*!
  Function: RTE_CurrentStackFree
  Description: Returns current amount of stack free

  This uses the internal knowledge of the stack bottom and return
  the difference between the stack top and the address of a local variable.
  Arguments: taskId [in] optional task id (if not known, it is determined)
  Return value: Number of bytes free on current stack
 */
SAPDB_UInt4 RTE_CurrentStackFree(RTE_TaskId taskId=0);
/*!
  Function: RTE_ReinitializeStack
  Description: Fill the stack with a defined patterned

  The pattern is filled up to maxUsed bytes or up to top of stack if maxUsed was not
  given.

  Arguments: taskId [in] optional task id (if not known, it is determined)
             maxUsed [in] optional maximum stack usage retrieved by RTE_MaximumStackUsage
 */ 
void RTE_ReinitializeStack(RTE_TaskId taskId=0, SAPDB_UInt4 maxUsed=0);

/* Pascal interface wrapper */
extern "C" void vsReinit(RTE_TaskId taskid, SAPDB_Int4 maxUsed);

/*!
  Function: RTE_MaximumStackUsage
  Description: Retrieve value of maximum command stack usage

  The last modification in the stack pattern is searched and determines to value returned.
  The maximum is searched beginning with the pointer stored during each dispatcher entry.

  Arguments: taskId [in] optional task id (if not known, it is determined)
  Return value: Number of bytes consumed on current stack during last command
 */ 
SAPDB_UInt4 RTE_MaximumStackUsage(RTE_TaskId taskId=0);

/*!
  Function: RTE_ResetCommandMaxStack
  Description: Reset internal value of maximum command stack to 0

  Each call to RTE_CurrentStackUsage() tests the maximum and increases it, if
  current value is exceeded.

  Arguments: taskId [in] task id
 */ 
void RTE_ResetCommandMaxStack(RTE_TaskId taskId);

/*!
  Function: RTE_GetCommandMaxStack
  Description: Retrieve value of maximum command stack

  Each call to RTE_CurrentStackUsage() tests the maximum and increases it, if
  current value is exceeded.

  Arguments: taskId [in] optional task id (if not known, it is determined)
  Return value: Number of bytes consumed on current stack during last command
 */ 
SAPDB_UInt4 RTE_GetCommandMaxStack(RTE_TaskId taskId);

#endif  /* RTE_CURRENTSTACK_HPP */
