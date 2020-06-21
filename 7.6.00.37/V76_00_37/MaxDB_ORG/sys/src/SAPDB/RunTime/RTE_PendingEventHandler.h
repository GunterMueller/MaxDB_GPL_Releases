/*!
  @file           RTE_PendingEventHandler.h
  @author         JoergM
  @ingroup        RunTime
  @brief          Contains all common RunTime Type definitions

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

#ifndef RTE_PENDINGEVENTHANDLER_H
#define RTE_PENDINGEVENTHANDLER_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp0031.h"
#include "SAPDBCommon/SAPDB_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! Default one every minute */
#define RTE_PENDING_EVENT_INTERVAL_DEFAULT 60

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined(__cplusplus)
#include "RunTime/RTE_PendingEventHandler.hpp"
#endif /* defined(__cplusplus) */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
 @brief register given event

 copy the given event and register it in the pending event list. Use given time interval
 to reissue the event until it is deregistered. Every event type can be registered only once.
 If the event is already registered, only the reissue_interval is updated.

 @param event [in] the event to send
 @param reissue_interval_in_seconds [in] number of seconds between two events
 */
externC void RTE_RegisterPendingEvent( tsp31_event_description const VAR_VALUE_REF event
                                     , SAPDB_UInt4 reissue_interval_in_seconds);

/*!
 @brief deregister given event using event iden identifier

 search for given event and deregister it from the pending event list.
 
 @param eventIdentifier [in] the identifier of the event to deregister
 */
externC void RTE_DeregisterPendingEvent( tsp31_event_ident_Param const eventIdentifier );

#endif  /* RTE_PENDINGEVENTHANDLER_H */
