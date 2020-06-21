/*!
  @file           gen73.h
  @author         JoergM
  @special area   Queue Handling
  @brief          Queue handling routines
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



#ifndef GEN73_H
#define GEN73_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gen50.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC	void e73_dev_ioc_queue_init ( struct IOC_QUEUE * ioc ,
									  tsp00_Int4           number );

externC void e73_xxx_queue_init ( struct XXX_QUEUE_HEAD * xxx ,
							 	  int                     nr_of_elem );

externC void e73_ioc_queue_init ( struct IOC_QUEUE_HEAD * head ,
								  struct IOC_QUEUE      * ioc ,
								  tsp00_Int4                number );

externC void e73_ior_queue_init ( struct IOR_QUEUE_HEAD * head ,
								  struct  IOR_QUEUE     * ior ,
								  tsp00_Int4                number );

externC int e73_ioc_enqu ( struct IOC_QUEUE_HEAD * head ,
						   struct DOUBLY_LINKED  * new_entry );

externC void e73_dl_init ( struct DOUBLE_Q_HEAD * head ,
						   char                 * name );

#endif  /* GEN73_H */
