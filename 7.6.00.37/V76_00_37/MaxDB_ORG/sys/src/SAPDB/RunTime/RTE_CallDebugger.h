/*!***************************************************************************

  module       : RTE_CallDebugger.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Debugging
  description : Debugger calling function


  last changed: 2000-07-25  16:54
  first created:2000-07-17  13:54

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


#ifndef RTE_CALLDEBUGGER_H
#define RTE_CALLDEBUGGER_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/SAPDB_C_Cplusplus.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/
 
/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
  Function: RTE_CallDebugger()

  Description: Trigger debugger even if disabled

    On NT a call to DebugBreak() is done. If program is debugged this forces the 
    debugger to take over triggered by an breakpoint exception.

    On UNIX if the debugger is attachable, a command is spawned, that attaches a debugger
    to the current running process. If no debugger attachable, an abort() is force.
 */
externC void RTE_CallDebugger();

/*!
  Function: RTE_DebugBreak()

  Description: Trigger debugger, but only  if enabled

    If debugger is disabled nothing is done!

    On NT a call to DebugBreak() is done. If program is debugged this forces the 
    debugger to take over triggered by an breakpoint exception.

    On UNIX if the debugger is attachable, a command is spawned, that attaches a debugger
    to the current running process. If no debugger attachable nothing is done.
 */
externC void RTE_DebugBreak();

/*!
  Function: RTE_SetupDebugger

  Description: Setup debugger access and enable/disable DebugBreak

    Select the debugger filename and enables/disables the debugger calling via RTE_DebugBreak

    If both arguments are NULL pointer, reset to default values.

  Arguments: Filename [in] The filename of the debugger command file 
                           or NULL if Filename is not to be modified.
             Enabled [in] Setup pointer to a flag for debugger enabling or NULL if not to be modified
  (Not MTSave)
 */
externC void RTE_SetupDebugger(const SAPDB_UTF8 *Filename, SAPDB_Bool *Enabled);

#endif  /* RTE_CALLDEBUGGER_H */
