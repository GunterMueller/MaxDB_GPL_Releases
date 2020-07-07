/*!
  @file           geo67.h
  @author         JoergM
  @special area   logging ...
  @brief          description ...
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



#ifndef GEO67_H
#define GEO67_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp0031.h"

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

externC bool  eo67ConnectEventTask( tsp00_TaskId pid );

externC bool  eo67ReleaseEventTask( tsp00_TaskId pid );

externC tsp00_Int4 eo67SpaceNeeded( tsp00_Int4 maxNumberOfEventTasks,
                                    tsp00_Int4 maxNumberOfEvents );

externC bool  eo67Initialize( tsp00_Int4 maxNumberOfEventTasks,
                              tsp00_Int4 maxNumberOfEvents );

externC void  eo67InitializeEventInfo( );

/*!
  @brief Insert an error message event into event ring buffer
  Called by any task or thread to insert an error message event into ring buffer
  All error messages written in knldiag are using this interface.

  @param msgNumber [in] internal message number
  @param msgText   [in] the zero terminated message test (truncated to 255 chracters if needed)
 */
externC void eo67InsertMessageEvent( SAPDB_Int4  msgNumber ,
                                     const char *msgText );

/*!
  @brief Insert an out of session event into event ring buffer
  Called by requestor if connect request must be rejected due to insufficient number of user tasks  
 */
externC void eo67InsertOutOfSessionEvent();

/*!
  @brief Insert an standby event event into event ring buffer
  Called after standby state is reached
 */
externC void eo67InsertStandbyEvent();

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO67_H */
