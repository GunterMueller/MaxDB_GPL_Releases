/*!
  @file           geo908.h
  @author         RaymondR
  @special area   XSERVER/DBMSERVER communication
  @brief          XSERVER/DBMSERVER communication
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



#ifndef GEO908_H
#define GEO908_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


# if defined( __cplusplus )
extern "C" {
#endif

tsp01_CommErr_Enum eo908_Connect( char const * const          pServerPgm,
                                  char const * const          pDBRoot,
                                  char const * const          pServerDB,
                                  void **                     ppDBM,
                                  void **                     ppCommPacket,
                                  tsp00_Uint4 *               pMaxCmdDataLen,
                                  tsp00_Uint4 *               pTransferBufferSize,
                                  tsp00_Uint4 *               pServerRef,
                                  char const * const          senderNodeId,
                                  RTE_OSPid                   senderPID);

tsp01_CommErr_Enum eo908_Request( void *                      pDBM,
                                  tsp00_Uint4  const          DataLen );

tsp01_CommErr_Enum eo908_Receive( void *                      pDBM,
                                  void ** const               ppReceivePacketBuffer,
                                  tsp00_Uint4 * const         pDataLen );

tsp01_CommErr_Enum eo908_Cancel ( void *                      pDBM );

tsp01_CommErr_Enum eo908_Release( void *                      pDBM );

# if defined( __cplusplus )
}
#endif


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO908_H */
