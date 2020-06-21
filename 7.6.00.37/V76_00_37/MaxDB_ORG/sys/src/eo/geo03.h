/*!
  @file           geo03.h
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



#ifndef GEO03_H
#define GEO03_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo003.h"
#include "geo40.h"
#include "geo401.h"


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
externC void 
eo03NiBuildTracefileName    (tsp00_Pathc VAR_ARRAY_REF  TracefileName) ;

externC tsp00_Uint     
eo03NiConnect ( teo003_ConnectParamRecord    *pConnParam,
                teo40_NiConnectInfoRecord    *pNIConnInfo,
                tsp00_ErrTextc  VAR_ARRAY_REF ErrTextC  );

externC tsp00_Uint     
eo03NiRelease ( teo40_NiConnectInfoRecord *pNIConnInfo,
                tsp00_ErrTextc VAR_ARRAY_REF  ErrTextC  );

externC tsp00_Uint     
eo03NiRequest  ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                 tsp00_Uint                    ulCmdPacket,
                 tsp00_Uint                    ulCmdDataLen,
                 tsp00_Uint1                   RTEFlags,
                 tsp00_ErrTextc  VAR_ARRAY_REF ErrTextC );

externC tsp00_Uint     
eo03NiReceive  ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                 teo003_CommPacketRecord     **ppResPacket,
                 tsp00_Uint VAR_VALUE_REF      ulResDataLen,
                 tsp00_ErrTextc VAR_ARRAY_REF  ErrTextC );

externC tsp00_Uint     
eo03NiSqlCancelDump       ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                            char                         *szSapRouterString,   /* PTS 1105116 */
                            tsp00_Uint                    ulReqType,
                            tsp00_ErrTextc  VAR_ARRAY_REF  ErrTextC );

externC tsp00_Uint     
eo03NiSqlReplyAvailable   ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                            tsp00_ErrTextc VAR_ARRAY_REF  ErrTextC );
/* PTS 1105295 */
externC tsp00_Uint     
eo03NiXserverPing( char                         *szSapRouterString,
                   tsp00_Versionc VAR_ARRAY_REF  szServerVersion,
                   SAPDB_Byte VAR_VALUE_REF      pServerCertificate,
                   SAPDB_UInt4                   lenCertificate,
                   SAPDB_Bool                    isSSL,
                   SAPDB_Bool                    isNISSL,
                   SAPDB_Bool                    ignoreHostnameInServerCert,  
                   SAPDB_Bool                    captureServerCert,  
                   tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC void eo03Finish ();

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO03_H */
