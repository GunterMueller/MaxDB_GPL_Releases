/*!
  @file           gen45.h
  @author         JoergM
  @special area   kernel_kill_and_info
  @brief          Kill Kernel Processes and Remove Ipc Resources
                              Kill Kernel Tasks
                              Fetch Communication Info
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



#ifndef GEN45_H
#define GEN45_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gen00.h"
#include "gen50.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void  sql45_killall ( struct KERNEL_GLOBAL_SECTION  * lkgs );
externC void  sql45_kill_task ( struct TASK_TYPE * tcb );
externC int   sql45_has_remote_pid ( struct TASK_TYPE *tcb );
externC char *sql45_peer_pid ( struct TASK_TYPE	*tcb, char *peer_pid_buffer );
externC char *sql45_peer_node ( struct TASK_TYPE *tcb );

#endif  /* GEN45_H */
