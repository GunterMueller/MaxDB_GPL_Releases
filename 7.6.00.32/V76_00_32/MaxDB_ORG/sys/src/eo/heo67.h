/*!
  @file           heo67.h
  @author         JoergM
  @special area   Eventing KERNEL Interfaces
  @brief          Prototypedefinition for interface routines called by kernel
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



#ifndef HEO67_H
#define HEO67_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp0031.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
  @brief Insert a event into event ring buffer
  Called by any task or thread to insert an event into ring buffer
  @param returnEvent [inevent to put into queue
 */
externC void vinsert_event(tsp31_event_description VAR_VALUE_REF returnEvent);

/*!
  @brief called by event task to wait for next event

  The call is blocking the calling event task until the next event is available
  @param pid [in] task id of event task
  @param returnEvent [out] event to be returned
  @param errText [out] error message if call failed
  @param pOk [out] return flag for success (1) or failure (0)
 */
externC void vwait_for_event(
const tsp00_TaskId                    pid,           /* Task id of event task */
tsp31_event_description VAR_VALUE_REF returnEvent,   /* event to be returned */
tsp00_ErrText           VAR_ARRAY_REF errtext,       /* returned error message */
tsp00_Bool              VAR_VALUE_REF pOk);          /* returned flag for success/failure */

#endif  /* HEO67_H */
