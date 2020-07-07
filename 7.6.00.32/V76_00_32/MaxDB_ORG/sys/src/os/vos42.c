/*!
  @file           vos42.c
  @author         RaymondR
  @brief          create/extract/read/write remote sql_packets
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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




/*
 * INCLUDE FILES
 */
#define RTE_USE_SOCK

#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "gos003.h"
#include "geo007_1.h"
#include "geo007_2.h"

#if defined (KERNEL)
# include "gos00k.h"            /* nocheck */  
#endif

#include "heo420.h"
#include "hos42.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS42C : "
#define MF__   MOD__"UNDEFINED"



/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

/*------------------------------*/
/* PTS 1001697 */

ULONG sql42c_pipe_send_conpkt ( POLD_CONPKT_PARAM_REC            pConPktParamRec,
                                HANDLE                           writePipe,
                                tsp00_ErrTextc                   pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql42c_pipe_send_conpkt"

  LONG                            rc;
  INT                             BytesSend;
  INT                             SendLen = sizeof(OLD_CONPKT_PARAM_REC);

  rc = WRITE_FILE (writePipe, pConPktParamRec, SendLen, &BytesSend);

  if ( rc != NO_ERROR )
    {
    MSGD (( ERR_CANT_WRITE_TO_PIPE, rc ))
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CANT_WRITE_TO_PIPE, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( BytesSend != SendLen )
    {
    MSGD (( ERR_PACK_WRITE_SIZE_ERROR, BytesSend ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_PACK_WRITE_SIZE_ERROR, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );

  }

/*------------------------------*/
/* PTS 1001697 */
ULONG sql42c_pipe_recv_conpkt ( POLD_CONPKT_PARAM_REC            pConPktParamRec,
                                HANDLE                           readPipe,
                                tsp00_ErrTextc                   pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql42c_pipe_recv_conpkt"

  INT                             BytesRec;
  LONG                            rc;

  rc = READ_FILE (readPipe, pConPktParamRec, sizeof(OLD_CONPKT_PARAM_REC), &BytesRec );

  if ( rc != NO_ERROR )
    {
    MSGD (( ERR_CONN_PACKET_READ_ERR, rc ))
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_PACKET_READ_ERR, rc );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( BytesRec == 0 )
    {
    MSGD (( ERR_CONN_SERV_REJECT ))
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_SERV_REJECT, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( BytesRec <  sizeof (OLD_CONPKT_PARAM_REC) )

    {
    MSGD (( ERR_CONN_PACKET_GARBLED, BytesRec ))
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_CONN_PACKET_GARBLED, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( commErrOk_esp01 );

  }

/*------------------------------*/

void sql42c_new2old_conpkt( POLD_CONPKT_PARAM_REC     pOldConPktParamRec,
                            PCONPKT_PARAM_REC         pConPktParamRec )
{
  #undef  MF__
  #define MF__ MOD__"sql42c_new2old_conpkt"

  pOldConPktParamRec->sd                = -1;
  pOldConPktParamRec->ulMessClass       = pConPktParamRec->ulMessClass;
  pOldConPktParamRec->ulSenderRef       = pConPktParamRec->ulSenderRef;
  pOldConPktParamRec->ulReceiverRef     = pConPktParamRec->ulReceiverRef;
  pOldConPktParamRec->ulServiceType     = pConPktParamRec->ulServiceType;
  pOldConPktParamRec->ulMaxSegmentSize  = pConPktParamRec->ulMaxSegmentSize;
  pOldConPktParamRec->ulPacketSize      = pConPktParamRec->ulPacketSize;
  pOldConPktParamRec->ulMaxDataLen      = pConPktParamRec->ulMaxDataLen;
  pOldConPktParamRec->ulMinReplySize    = pConPktParamRec->ulMinReplySize;
  pOldConPktParamRec->ulSwapType        = pConPktParamRec->ulSwapType;
  pOldConPktParamRec->pidSenderPID      = pConPktParamRec->pidSenderPID;
  pOldConPktParamRec->usServicePort     = pConPktParamRec->usServicePort;
  pOldConPktParamRec->ulCommState       = pConPktParamRec->ulCommState;
  pOldConPktParamRec->fAcknowledge      = pConPktParamRec->fAcknowledge;

  strncpy ( pOldConPktParamRec->szSenderServerDB,
            pConPktParamRec->szSenderServerDB,
            sizeof(pOldConPktParamRec->szSenderServerDB) - 1);
  strncpy ( pOldConPktParamRec->szReceiverServerDB,
            pConPktParamRec->szReceiverServerDB,
            sizeof(pOldConPktParamRec->szReceiverServerDB) - 1);
  strncpy ( pOldConPktParamRec->szServerDBRoot,
            pConPktParamRec->szServerDBRoot,
            sizeof(pOldConPktParamRec->szServerDBRoot) - 1);
  strncpy ( pOldConPktParamRec->szServerPgm,
            pConPktParamRec->szServerPgm,
            sizeof(pOldConPktParamRec->szServerPgm) - 1);
}


/*------------------------------*/

tsp00_Uint     
os42SocketSend ( void                         *SendHandle,
                 void                         *pData,
                 int                           Len,
                 tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"os42SocketSend"
  LONG                            rc;
  INT                             BytesSend;
  teo00_Socket                    sd = *( (teo00_Socket *) SendHandle );

  DBGIN;

  for (BytesSend = 0; Len > 0; (char *)pData += BytesSend, Len -= BytesSend )
    {
    rc = sql40c_send_packet ( sd, pData, Len, &BytesSend );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SOCKET_SND_ERR, rc ));
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SOCKET_SND_ERROR, rc );

      DBGOUT;
      return ( commErrNotOk_esp01 );
      }
    }

  DBGOUT;
  return ( commErrOk_esp01 );
  }

/*------------------------------*/

tsp00_Uint     
os42SocketReceive ( void                         *RecvHandle,
                    void                         *pData,
                    tsp00_Uint                    ulMaxDataLen,
                    tsp00_Uint                   *BytesReceived,
                    tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"os42SocketReceive"
  LONG                            rc;
  teo00_Socket                    sd = *( (teo00_Socket *) RecvHandle );
  tsp00_Uint                      CommState = commErrOk_esp01;


  DBGIN;

  // --- Receive the RTE Header first!
  //
  rc = sql40c_recv_packet ( sd, pData, ulMaxDataLen, BytesReceived );

  if ( rc != NO_ERROR )
  {
    MSGD (( ERR_SOCKET_REC_ERR, rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SOCKET_REC_ERROR, rc );
    CommState = commErrNotOk_esp01 ;
  }

  DBGOUT;
  return ( CommState );

  }

/*------------------------------*/

tsp00_Uint     
os42SocketSetBuf ( teo00_Socket                  sd,
                   tsp00_Uint                    MinBufSize,
                   tsp00_Uint                   *MaxBufSize,
                   tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
  {
  #undef  MF__
  #define MF__ MOD__"os42SocketSetBuf"

  LONG                            rc;
  tsp00_Uint                      CommState = commErrOk_esp01;

  rc = sql40c_set_send_buffer_size ( sd, MaxBufSize, MinBufSize );

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_SET_BUF_SIZE, "SNDBUF", rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SOCKET_SET_SNDBUF_ERROR, rc );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // --- set receive buffer size
  //
  rc = sql40c_set_recv_buffer_size ( sd, MaxBufSize, MinBufSize );

  if ( rc != NO_ERROR )
    {
    MSGALL (( ERR_XSER_SET_BUF_SIZE, "RCVBUF", rc ));
    eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SOCKET_SET_RCVBUF_ERROR, rc );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  DBGOUT;
  return ( CommState );

  }

/*
 * =============================== END ========================================
 */
