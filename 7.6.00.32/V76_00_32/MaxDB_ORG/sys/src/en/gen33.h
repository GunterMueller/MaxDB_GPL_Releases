/*!
  @file           gen33.h
  @author         JoergM
  @special area   Remote communication
  @brief          Communation functions
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



#ifndef GEN33_H
#define GEN33_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int sql33_connect ( connection_info              *pConnInfo,
							tsp00_ErrTextc VAR_ARRAY_REF  pErrText,
							connection_info               conn_info [ ],
							int                           count);

externC int sql33_release ( connection_info              *pConnInfo ,
							tsp00_ErrTextc VAR_ARRAY_REF  pErrText,
						    connection_info               conn_info [ ],
						    int                           count);

externC int sql33_request ( connection_info              *pConnInfo ,
							tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC int sql33_replyavailable ( connection_info              *pConnInfo ,
                                   tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC int sql33_receive ( connection_info             *pConnInfo ,
                            tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int en33CheckedReceive ( int (*checkCallback)(void *),
                                 void                        *arg,
                                 connection_info             *pConnInfo ,
                                 tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int sql33_cancel ( connection_info             *pConnInfo ,
                           tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int sql33_dump   ( connection_info             *pConnInfo ,
                           tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int sql33_clear ( connection_info             *pConnInfo ,
                          tsp00_ErrTextc VAR_ARRAY_REF pErrText,
						  connection_info              conn_info [ ],
						  int                          count);

externC void sql33_cleanup ( connection_info *pConnInfo );

externC teo00_Longuint 
en33Request ( connection_info                *pConnInfo, 
              teo00_Longuint                  ulDataLen,
              tsp00_ErrTextc  VAR_ARRAY_REF   szErrText ) ;

externC teo00_Longuint 
en33Receive ( connection_info                *pConnInfo, 
              teo003_CommPacketRecord       **ppResultPacket,
              teo00_Longuint                 *pulResultLen,
              tsp00_ErrTextc  VAR_ARRAY_REF   szErrText );

#endif  /* GEN33_H */
