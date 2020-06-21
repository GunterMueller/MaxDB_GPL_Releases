/*!
  @file           gos42.h
  @author         RaymondR
  @brief          socket communication
  @see            

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



#ifndef GOS42_H
#define GOS42_H



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


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

tsp00_Uint     
os42SocketSend    ( void                         *SendHandle,
                    void                         *pData,
                    int                           Len,
                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText ) ;

tsp00_Uint     
os42SocketReceive ( void                         *RecvHandle,
                    void                         *pData,
                    tsp00_Uint                    ulMaxDataLen,
                    tsp00_Uint                   *BytesReceived,
                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText ) ;

tsp00_Uint     
os42SocketSetBuf ( teo00_Socket                  SocketHandle,
                   tsp00_Uint                    MinBufSize,
                   tsp00_Uint                   *MaxBufSize,
                   tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS42_H */
