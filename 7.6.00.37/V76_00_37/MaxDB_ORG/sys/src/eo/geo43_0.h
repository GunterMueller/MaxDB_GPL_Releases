/*!
  @file           geo43_0.h
  @author         JoergM
  @special area   socket_address_resolution
  @brief          Get Socket Addresses
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



#ifndef GEO43_0_H
#define GEO43_0_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#ifdef WIN32

externC  int   sql43_tcpip_init              ();
externC  void  sql43_tcpip_cleanup           ();

#endif

externC  int  sql43_get_my_host_name       ( char *host ,
									         int   size );

externC  int  sql43_get_official_node_name ( char *host ,
											 char *node ,
											 int   size );
externC  int  sql43_get_service_by_name    ( char           *service ,
										     unsigned short *port );
externC  int  sql43_get_host_by_name       ( char          *host ,
									         unsigned char *addr ,
									         int            size );

externC  int  sql43_get_host_by_address   ( unsigned char *addr ,
										    char          *host ,
										    int            size );

externC  int  sql43_is_inet_addr           ( char  *InetAddr );

#endif  /* GEO43_0_H */
