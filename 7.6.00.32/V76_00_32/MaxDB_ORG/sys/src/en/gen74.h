/*!
  @file           gen74.h
  @author         JoergM
  @special area   communication_shm
  @brief          Initialize Shared Memory Section for Communication
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



#ifndef GEN74_H
#define GEN74_H

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

externC	unsigned long	e74_comdata_size ( RTE_XPARAM_REC * XParam ,
										   long			    kgs_ukp_count );

externC	void	e74_init_comdata ( RTE_XPARAM_REC	  * XParam,
                                   int                  max_sock_conns,
                                   teo70k_ClusterInfo * pCluInfo );

externC  int    e74_create_big_comsegs ( void );

externC	void	e74_init_big_comseg ( struct ten50_UKT_Control *this_ukt );

externC	void	e74_release_comseg ( connection_info * cip, struct TASK_TYPE  *tcb );

externC	void	e74_print_all_conns ( char * hdr );

externC	void	e74_print_connection ( char            * hdr ,
									   connection_info * cip );

#endif  /* GEN74_H */
