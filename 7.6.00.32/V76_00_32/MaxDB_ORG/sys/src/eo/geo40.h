/*!
  @file           geo40.h
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



#ifndef GEO40_H
#define GEO40_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo003.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define NI_TRACEFILE_EO40            "NI_TRACEFILE"
#define NI_TRACE_ERRORS_ONLY_EO40 1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*
 * -- RTE HEADER for communication packets.
 */

typedef struct teo40_NiConnectInfoRecord
  {
  tsp00_Int4                                 pidClientPID;
  tsp00_DbNamec                              szServerDB;
  tsp00_NodeIdc                              szServerNode;
  char                                      *pszServerDBRoot;           /* control server only */
  char                                      *pszServerPgm;              /* control server only */
  tsp00_Bool                                 fAcknowledge;
  char                                      *szSapRouterString;
  tsp00_Uint                                 ulServerRef;
  tsp00_Uint                                 ulClientRef;
  tsp00_Uint                                 ulServiceType;

  tsp00_Uint                                 ulCurrentPacket;
  tsp00_Uint                                 ulCmdDataLen;
  tsp00_Uint                                 ulPacketCnt;
  struct teo003_CommPacketRecord            *pCommPacketList[PACKET_CNT_MXSP01];

  tsp00_Uint                                 ulPacketSize;
  tsp00_Uint                                 ulMaxDataLen;
  tsp00_Uint                                 ulMinReplySize;

  char                                       authenticationAllow[256];

  void                                      *NiHandle;
  tsp00_Int4                                 SwapType;
  char                                       ucName[12];
  tsp00_Uint2                                usServicePort;
  tsp00_Uint                                 ulSwapType;
  tsp00_Uint                                 ulMinSegmentSize;
  tsp00_Uint                                 ulMaxSegmentSize;
  SAPDB_Bool                                 isSSL;                     /* SSL connection */
  SAPDB_Bool                                 isSSLoverSAPRouterString;  /* SSL connection with SAPRouter string */
  SAPDB_Bool                                 ignoreHostnameInServerCert;
  void *                                     pSSLHandle;
  } teo40_NiConnectInfoRecord ;


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
externC int
eo40NiIsLoaded();

externC int
eo40NiIsAvailable(); 

externC int
eo40NiIsInitialized();

externC tsp00_Uint     
eo40NiInit        ( tsp00_Pathc     VAR_ARRAY_REF  TraceFile,
                    int                            TraceLevel,
                    tsp00_ErrTextc  VAR_ARRAY_REF  ErrText );

externC tsp00_Uint     
eo40NiHandleToSocket        ( void                         *NiHandle ,
                              int                          *Sd,
                              tsp00_ErrTextc  VAR_ARRAY_REF  ErrText );

/*-----------------------------------------------------------------------------------*/
externC int   
eo40NiIsValidHandle      ( void                    *NiHandle );
/*-----------------------------------------------------------------------------------*/

externC tsp00_Uint     
eo40NiClose     ( void                         **NiHandle);

externC tsp00_Uint     
eo40NiFinish    (tsp00_ErrTextc  VAR_ARRAY_REF  ErrText);

externC tsp00_Uint     
eo40NiListen    ( void                        **NiHandle,
                  unsigned short               *Service,
                  tsp00_ErrTextc VAR_ARRAY_REF  pErrText) ;

externC tsp00_Uint     
eo40NiAccept    ( void                         *NiListenHandle,
                  void                        **NiCommHandle,
                  char                         *ServerNode, 
                  int                           MaxServerNodeLen,
                  tsp00_ErrTextc VAR_ARRAY_REF  ErrText );

externC tsp00_Uint     
eo40NiConnectToServer( teo40_NiConnectInfoRecord    *pNIConnInfo,
                       tsp00_ErrTextc VAR_ARRAY_REF  ErrText );

/*-----------------------------------------------------------------------------------*/


externC tsp00_Uint     
eo40NiSend                ( void                         *SendHandle,
                            char                         *pData,
                            int                           Len,
                            tsp00_ErrTextc  VAR_ARRAY_REF pErrText );


externC tsp00_Uint     
eo40NiReceive             ( void                         *RecvHandle,
                            char                         *pData,
                            tsp00_Uint                    ulMaxDataLen,
                            tsp00_Uint                   *BytesReceived,
                            tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC tsp00_Uint     
eo40NiDataAvailable      ( void                          *NiHandle ,
                           tsp00_ErrTextc  VAR_ARRAY_REF  pErrText );

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO40_H */
