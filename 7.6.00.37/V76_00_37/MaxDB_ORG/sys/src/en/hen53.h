/*!
  @file           hen53.h
  @author         JoergM
  @special area   communication
  @brief          vcmd_wait_time, vconnect, vconnect2, vreceive, vreply
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



#ifndef HEN53_H
#define HEN53_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp01.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC tsp00_Int4 vcmd_wait_time ( tsp00_TaskId pid );

externC void    vconnect  ( tsp00_TaskId                pid ,
						    tsp00_Int4    VAR_VALUE_REF max_data_len ,
						    tsp01_CommErr VAR_VALUE_REF returncode );

externC void    vreceive (  tsp00_TaskId                pid ,
							tsp00_Int4                  timeout ,
							void                      **rcv_packet_ptr ,
							tsp00_Int4    VAR_VALUE_REF rcv_length ,
							void                      **reply_packet_ptr ,
							tsp00_Int4    VAR_VALUE_REF reply_size ,
							tsp01_CommErr VAR_VALUE_REF returncode );

externC void    vreply (    tsp00_TaskId                pid ,
							void                       *reply_packet_ptr ,
							tsp00_Int4                  reply_length ,
							tsp01_CommErr VAR_VALUE_REF returncode );

externC	void	vrelease ( tsp00_TaskId	pid );

externC void	vgetapplpid( tsp00_TaskId               pid ,
                             tsp00_NodeId VAR_ARRAY_REF appl_node ,
                             tsp00_Int4   VAR_VALUE_REF appl_pid ,
                             tsp00_Bool   VAR_VALUE_REF ok );

externC	 tsp00_Int4  vconnectedusertasks ();

#endif  /* HEN53_H */
