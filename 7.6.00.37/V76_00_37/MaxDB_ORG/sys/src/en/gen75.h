/*!
  @file           gen75.h
  @author         JoergM
  @special area   communication kernel
  @brief          Communication Functions for Kernel
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



#ifndef GEN75_H
#define GEN75_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"
#include "geo002.h"
#include "geo70k.h"
#include "gen00.h"
#include "gen003.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int e75_wake_peer ( connection_info *cip );

externC void e75_release_all_ukp_tasks ( );

externC void e75_release_connection ( int reason ,
                                      int waitflag );
externC int e75_is_connected ( struct TASK_TYPE *tcb );

externC int e75_connect_request ( struct TASK_TYPE  * tcb, 
                                  connection_info   * cip );

/* PTS 1113008 */
/*!
   @Description    Release client connections from Watchdog thread after _exit of kernel
   @param          kernelExitCode [in] final kernl exit code != 0 on crash, == 0 on normal shutdown

 */

externC void en75FinalReleaseConnections( int kernelExitCode );

#endif  /* GEN75_H */
