/*!
  @file           vos40.c
  @author         RaymondR
  @brief          Socket API
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
#include "heo46.h"
#include "gos003.h"
#include "hsp100.h"

#ifndef NO_SOCKET
#include         <sys/types.h>
#endif

#include "hsp100.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS40C : "
#define MF__   MOD__"UNDEFINED"

#ifndef NO_SOCKET
#define  TCPErrno                WSAGetLastError()
#endif

#define OLD_TCPIP_CONNECT_REC_SIZE  300

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */
#ifndef NO_SOCKET
 typedef struct hostent          hostent_type;
 typedef struct servent          servent_type;
 typedef struct sockaddr         sockaddr_type;
 typedef INT                     sockopt_type;
 typedef struct timeval          timeval_type;
 typedef struct linger           linger_type;
#endif

/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */
static CHAR cMajorSockVersion;
static CHAR cMinorSockVersion;
static BOOL AlreadyStarted = FALSE;

/*
 * LOCAL FUNCTION PROTOTYPES
 */

#ifndef NO_SOCKET
static SOCKET RTE_WSASocket( int addressFamily, int protocolType, int protocol, int *pRc, DWORD   flags );

#endif


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

ULONG sql40c_get_dll_version ( tsp100_VersionID1 *pVersion )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_dll_version"

  DBGPAS;

  sp100_GetVersionID ( VersionIDType1_esp100, s100buildnumberEx, pVersion );

  return(NO_ERROR);
  }

/*------------------------------*/

INT sql40c_send_conpkt ( teo00_Socket                    sd,
                         PCHAR                           pConnectPacket,
                         INT2                            SendLen,
                         PINT                            pBytesSend )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_send_conpkt"

  DBGPAS;

  // --- This function is needed to be compatible to 3.12 and 6.11
  //     and should not longer be used by newer versions
  //
  return ( sql40c_send_packet ( sd, pConnectPacket, SendLen, pBytesSend ) );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_send_conpkt )
#endif

/*------------------------------*/

INT sql40c_recv_conpkt ( teo00_Socket                    sd,
                         PCHAR                           pConnectPacket,
                         PINT                            pBytesReceived )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_recv_conpkt"

  DBGPAS;

  // --- This function is needed to be compatible to 3.12 and 6.11
  //     and should not longer be used by newer versions.
  //
  return ( sql40c_recv_packet ( sd, pConnectPacket, OLD_TCPIP_CONNECT_REC_SIZE, pBytesReceived ));
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_recv_conpkt )
#endif

/*------------------------------*/

INT sql40c_send_pkt ( teo00_Socket                    sd,
                      PCHAR                           pPacket,
                      INT2                            SendLen,
                      PINT                            pBytesSend )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_send_pkt"

  DBGPAS;

  // --- This function is needed to be compatible to 3.12 and 6.11
  //     and should not longer be used by newer versions.
  //

  return ( sql40c_send_packet ( sd, pPacket, SendLen, pBytesSend ) );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_send_pkt )
#endif

/*------------------------------*/

INT sql40c_recv_pkt ( teo00_Socket                    sd,
                      PCHAR                           pPacket,
                      INT2                            ReceiveLen,
                      PINT                            pBytesReceived )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_recv_pkt"

  DBGPAS;

  // --- This function is needed to be compatible to 3.12 and 6.11
  //     and should not longer be used by newer versions.
  //

  return ( sql40c_recv_packet ( sd, pPacket, ReceiveLen, pBytesReceived ) );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_recv_pkt )
#endif

/*------------------------------*/

INT sql40c_gethostname ( PSZ pszHostName, LONG lNameLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_gethostname"

  DBGPAS;

  // --- This function is needed to be compatible to 3.12 and 6.11
  //     and should not longer be used by newer versions.
  //
  if (!GetComputerName(pszHostName, &lNameLen))
    return (GetLastError());

  return (NO_ERROR);
  }


/*------------------------------*/

#ifdef NO_SOCKET

INT sql40c_sock_init ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_sock_init"

  DBGPAS;
  return ( -1 );
  }

/*------------------------------*/


INT sql40c_get_sock_version ( UCHAR *pcMajorSockVersion,
                              UCHAR *pcMinorSockVersion )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_sock_version"

  *pcMajorSockVersion = 0;
  *pcMinorSockVersion = 0;

  DBGPAS;
  return ( -1 );
  }

/*------------------------------*/


INT sql40c_sock_cleanup ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_sock_cleanup"

  DBGPAS;
  return ( -1 );
  }

/*------------------------------*/


INT sql40c_getservbyname ( PCHAR pszService, PUSHORT pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_getservbyname"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_gethostbyname ( PSZ      pszHostName,
                           PUCHAR   pucAddr,
                           PINT     pAddrLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_gethostbyname"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_gethostbyaddr ( PSZ      pszHostName,
                           PUCHAR   pucAddr,
                           LONG     lAddrLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_gethostbyaddr"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_stream_socket2 ( teo00_Socket *psd,  
                            DWORD         flags)
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_stream_socket2"

  DBGPAS;

  return ( -1 );
  }
/*------------------------------*/

INT sql40c_stream_socket ( teo00_Socket *psd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_socket"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_socket_close ( teo00_Socket sd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_socket_close"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_accept_conn_request ( teo00_Socket    sd,
                                 teo00_Socket	 *psdClient,
                                 PVOID			 pSockAddr,
                                 PINT			 pLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_accept"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_keep_alive ( teo00_Socket sd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_keep_alive"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_linger ( teo00_Socket sd, BOOL fLinger, INT iSeconds )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_linger"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_bind ( teo00_Socket sd, USHORT usServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_bind"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_listen ( teo00_Socket sd, INT Count )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_listen"

  DBGPAS;

  return ( -1 );
  }


/*------------------------------*/

INT sql40c_set_send_buffer_size ( teo00_Socket sd, PINT pMaxSize, INT MinSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_set_send_buffer_size"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_set_recv_buffer_size ( teo00_Socket sd, PINT pMaxSize, INT MinSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_set_recv_buffer_size"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_connect( teo00_Socket sd, USHORT usServicePort, PUCHAR pucName, INT NameLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_connect"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_send_packet ( teo00_Socket                    sd,
                         PCHAR                           pPacket,
                         INT                             SendLen,
                         PINT                            pBytesSend )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_send_packet"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_recv_packet ( teo00_Socket                    sd,
                         PCHAR                           pPacket,
                         INT                             ReceiveLen,
                         PINT                            pBytesReceived )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_recv_packet"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_select ( PINT                            pSDCount,
                    PVOID                           pReadSDMask,
                    PVOID                           pWriteSDMask,
                    PVOID                           pExceptSDMask,
                    USHORT                          usTimeOut )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_select"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_get_serviceport ( teo00_Socket sd, PUSHORT pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_serviceport"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_is_inet_addr_valid ( PCHAR Addr )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_is_inet_addr_valid"

  DBGPAS;

  return ( -1 );
  }

/*------------------------------*/

INT sql40c_bind_on_port_and_inet_addr ( teo00_Socket  sd, 
                                        USHORT        usServicePort,
                                        PCHAR         pszInetAddr )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_bind_on_port_and_inet_addr"

  DBGPAS;

  return ( -1 );
  }


/*------------------------------*/

#else

/*------------------------------*/

INT sql40c_sock_init ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_sock_init"
  static WSADATA WsaData;
  static INT     Err = NO_ERROR;
  WORD           wVersionRequested;


  DBGPAS;


   if (AlreadyStarted)
     return (Err);

   wVersionRequested = MAKEWORD( 2, 0 );

   if (WSAStartup(wVersionRequested, &WsaData))
     return (Err = WSAGetLastError());

   #if defined (WINSOCK20)
    /* Confirm that the WinSock DLL supports 2.0.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.0 in addition to 2.0, it will still return */
    /* 2.0 in wVersion since that is the version we      */
    /* requested.                                        */

    if ( LOBYTE( WsaData.wVersion ) != 2 ||
         HIBYTE( WsaData.wVersion ) != 0 )
      {
      WSACleanup( );
      return (-1);
      }
   #endif

   cMajorSockVersion = LOBYTE( WsaData.wVersion );
   cMinorSockVersion = HIBYTE( WsaData.wVersion );

   AlreadyStarted    = TRUE;

   return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_sock_init )
#endif

/*------------------------------*/

INT sql40c_get_sock_version ( UCHAR *pcMajorSockVersion,
                              UCHAR *pcMinorSockVersion )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_sock_version"

  *pcMajorSockVersion = cMajorSockVersion;
  *pcMinorSockVersion = cMinorSockVersion;

  DBGPAS;
  return ( NO_ERROR );
  }
#ifdef STATIC_LINK
 #pragma handler ( sql40c_get_sock_version )
#endif


/*------------------------------*/

INT sql40c_sock_cleanup ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_sock_cleanup"

  DBGPAS;

  AlreadyStarted = FALSE;

  WSACleanup();
  
  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_sock_cleanup )
#endif

/*------------------------------*/

INT sql40c_getservbyname ( PCHAR pszService, PUSHORT pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_getservbyname"
  servent_type   *sp;

  DBGPAS;

  sp = getservbyname ( pszService , SOCKET_PROTOCOL_EO003 );

  if ( sp != NULL )
    {
    *pusServicePort = ntohs (sp->s_port);
    DBG3((MF__, "The '%s' service port is: %d", pszService, *pusServicePort));
    }
  else
    {
        /* PTS 1110452 */
        if ( strcmp( pszService, SOCKET_SERVICE_SQL6_EO003) == 0 )
        {
            *pusServicePort = DEFAULT_PORT_SERVICE_SQL6_EO003;
        } else if ( strcmp( pszService, SOCKET_SERVICE_SQL30_EO003) == 0 )
        {
            *pusServicePort = DEFAULT_PORT_SERVICE_SQL30_EO003;
        } else if ( strcmp( pszService, SOCKET_SERVICE_NI72_EO003) == 0 )
        {
            *pusServicePort = DEFAULT_PORT_SERVICE_NI72_EO003;
        } else if ( strcmp( pszService, SOCKET_SERVICE_NISSL76_EO003) == 0 )
        {
            *pusServicePort = DEFAULT_PORT_SERVICE_NISSL76_EO003;
        } else
        {
    		return ( -1 );
        }
        DBG3 (( MF__,"service port for %s default to %d \n" , pszService, *pusServicePort ));
        DBGOUT;
        return ( 0 );
    }

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_getservbyname )
#endif

/*------------------------------*/

INT sql40c_gethostbyname ( PSZ      pszHostName,
                           PUCHAR   pucAddr,
                           PINT     pAddrLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_gethostbyname"
  hostent_type  *hp ;

  DBGPAS;

  hp = gethostbyname ( pszHostName );

  if ( hp != NULL )
    {
    if ( hp->h_length > *pAddrLen )
      return ( EINVAL );

    *pAddrLen = hp->h_length;
    SAPDB_memcpy ( pucAddr, (PUCHAR)hp->h_addr , hp->h_length );
    }
  else
    return ( -1 );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_gethostbyname )
#endif

/*------------------------------*/

INT sql40c_gethostbyaddr ( PSZ      pszHostName,
                           PUCHAR   pucAddr,
                           LONG     lAddrLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_gethostbyaddr"
  hostent_type  *hp ;
  PCHAR         pszHName;
  PCHAR         *ppszAlias;

  DBGPAS;

  hp = gethostbyaddr ( (PCHAR) pucAddr , INET_ADDR_SIZE , AF_INET );

  if ( hp != NULL )
    {
    pszHName = (PCHAR)hp->h_name;

    for ( ppszAlias = (PCHAR*)hp->h_aliases;
          ppszAlias && *ppszAlias;
          ppszAlias++ )
      {
      if ( strchr ( *ppszAlias, '.' ) )
        {
        pszHName = *ppszAlias;
        break;
        }
      }

    if ( (LONG) strlen (pszHName) > lAddrLen )
      return ( EINVAL );
    else
      strncpy ( pszHostName, pszHName, lAddrLen );
    }
  else
    return ( -1 );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_gethostbyaddr )
#endif

/*------------------------------*/

INT sql40c_stream_socket2 (teo00_Socket *psd,  
                           DWORD         flags)
{
    #undef  MF__
    #define MF__ MOD__"sql40c_stream_socket2"
    INT                     rc;
    #if defined ( NO_SOCKET_DELAY )
    BOOL                    NoDelay = TRUE;
    #endif


    DBGPAS;


    *psd = RTE_WSASocket ( AF_INET, SOCK_STREAM, 0, &rc, flags);

    if ( *psd == INVALID_SOCKET )
    {
        return ( rc );
    }
    /* WSASocket used, since socket() does not allow to specify non-overlapped-IO !!!! */

    #if defined ( NO_SOCKET_DELAY )
    rc = setsockopt ( *psd, IPPROTO_TCP, TCP_NODELAY, (PUCHAR)&NoDelay, sizeof(BOOL));

    if ( rc < 0 )
        return ( TCPErrno );

    #endif

    return ( NO_ERROR );
}

/*------------------------------*/

INT sql40c_stream_socket ( teo00_Socket *psd )
{
    #undef  MF__
    #define MF__ MOD__"sql40c_socket"

    DBGPAS;
    // No IO-Overlapped
    return (sql40c_stream_socket2 (psd, 0));
}

#ifdef STATIC_LINK
 #pragma handler ( sql40c_stream_socket )
#endif

/*------------------------------*/

INT sql40c_socket_close ( teo00_Socket  sd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_socket_close"

  DBGPAS;

  shutdown ( sd, 1 );

  if ( closesocket( sd ) < 0 )
     return ( TCPErrno );
  
  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_socket_close )
#endif

/*------------------------------*/

INT sql40c_accept_conn_request ( teo00_Socket    sd,
                                 teo00_Socket    *psdClient,
                                 PVOID			 pSockAddr,
                                 PINT		     pLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_accept_conn_request"

  DBGPAS;

  *psdClient = accept ( sd, (sockaddr_type*)pSockAddr, pLen );

  if ( *psdClient == INVALID_SOCKET )
    {
    return ( TCPErrno );
    }

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_accept_conn_request )
#endif

/*------------------------------*/

INT sql40c_keep_alive ( teo00_Socket  sd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_keep_alive"
  sockopt_type            KeepAlive = (sockopt_type) 1;
  INT                     rc;

  DBGPAS;

  rc = setsockopt ( sd, SOL_SOCKET, SO_KEEPALIVE, (PUCHAR)&KeepAlive, sizeof(sockopt_type));

  if ( rc < 0 )
    return ( TCPErrno );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_keep_alive )
#endif

/*------------------------------*/

INT sql40c_linger ( teo00_Socket  sd, BOOL fLinger, INT iSeconds )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_linger"
  INT                     rc;
  linger_type             Linger;

  DBGPAS;

  Linger.l_onoff  = fLinger;
  Linger.l_linger = iSeconds;

  if ( fLinger )
    rc = setsockopt ( sd, SOL_SOCKET, SO_LINGER, (PUCHAR)&Linger, sizeof(Linger));
  else
    rc = setsockopt ( sd, SOL_SOCKET, SO_DONTLINGER, (PUCHAR)&Linger, sizeof(Linger));

  if ( rc < 0 )
    return ( TCPErrno );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_linger )
#endif

/*------------------------------*/

INT sql40c_bind ( teo00_Socket  sd, USHORT usServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_bind"
  sockaddr_type                   SockAddr;
  INT                             rc;

  DBGPAS;

  // --- host address is implicit, so only address family and
  //     port number are to be determined
  memset ( &SockAddr, 0, sizeof(sockaddr_type) );

  SockAddr.sa_family = AF_INET;

  if ( usServicePort != 0 )
    {
    SockAddr.sa_data [ 0 ] = ( usServicePort >> 8 & 0xFF );   // - high byte
    SockAddr.sa_data [ 1 ] = ( usServicePort      & 0xFF );   // - low  byte
    }

  // --- bind the socket to the server address
  rc = bind ( sd, &SockAddr, sizeof(sockaddr_type) );

  if ( rc < 0 )
    return ( TCPErrno );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_bind )
#endif

/*------------------------------*/

INT sql40c_listen ( teo00_Socket  sd, INT  Count )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_listen"
  INT                             rc;

  DBGPAS;

  rc = listen ( sd, Count );

  if ( rc < 0 )
    return ( TCPErrno );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_listen )
#endif

/*------------------------------*/

INT sql40c_set_send_buffer_size ( teo00_Socket  sd, PINT pMaxSize, INT MinSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_set_send_buffer_size"
  sockopt_type                    MaxSize  = *pMaxSize;
  sockopt_type                    CurrSize = 0;
  sockopt_type                    OptLen   = sizeof(CurrSize);
  INT                             rc,
                                  i;
  DBGPAS;

  #if defined (OS2)
   rc = getsockopt ( sd, SOL_SOCKET, SO_SNDBUF,
                     (CHAR*) &CurrSize, &OptLen );

   if ((rc >= 0) && ( CurrSize >= MaxSize ))
     return ( NO_ERROR );
  #endif

  for (; MaxSize > MinSize; )
    {
    rc = setsockopt ( sd, SOL_SOCKET, SO_SNDBUF,
                      (CHAR*) &MaxSize, sizeof(MaxSize) );

    if ( (rc >= 0) || ( (rc < 0) && ( TCPErrno != SOCENOBUFS ) ))
      break;


    for ( i = -2; MaxSize; i++ )
      MaxSize = MaxSize >> 1;

    MaxSize = 1 << i;
    }

  if ( rc < 0 )
    return ( TCPErrno );

  *pMaxSize = MaxSize;

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_set_send_buffer_size )
#endif

/*------------------------------*/

INT sql40c_set_recv_buffer_size ( teo00_Socket  sd, PINT pMaxSize, INT MinSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_set_recv_buffer_size"
  sockopt_type                    MaxSize  = *pMaxSize;
  sockopt_type                    CurrSize = 0;
  sockopt_type                    OptLen   = sizeof(CurrSize);
  INT                             rc,
                                  i;

  DBGPAS;

  #if defined (OS2)
   rc = getsockopt ( sd, SOL_SOCKET, SO_RCVBUF,
                     (CHAR*) &CurrSize, &OptLen );

   if ((rc >= 0) && ( CurrSize >= MaxSize ))
     return ( NO_ERROR );
  #endif

  for (; MaxSize > MinSize; )
    {
    rc = setsockopt ( sd, SOL_SOCKET, SO_RCVBUF,
                      (CHAR*) &MaxSize, sizeof(MaxSize) );

    if ( (rc >= 0) || ( (rc < 0) && ( TCPErrno != SOCENOBUFS ) ))
      break;

    for ( i = -2; MaxSize; i++ )
      MaxSize = MaxSize >> 1;

    MaxSize = 1 << i;
    }

  if ( rc < 0 )
    return ( TCPErrno );

  *pMaxSize = MaxSize;

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_set_recv_buffer_size )
#endif

/*------------------------------*/

INT sql40c_connect( teo00_Socket  sd, USHORT usServicePort, PUCHAR pucName, INT NameLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_connect"
  sockaddr_type                   SockAddr;
  INT                             rc;

  DBGPAS;

  if ( NameLen > sizeof ( SockAddr.sa_data ) - 2)
    return ( EINVAL );

  //
  // --- get socketaddress of server
  //
  memset ( &SockAddr, 0, sizeof(sockaddr_type) );

  SockAddr.sa_family = AF_INET;
  SockAddr.sa_data [ 0 ] = ( usServicePort >> 8 & 0xFF );   // - high byte
  SockAddr.sa_data [ 1 ] = ( usServicePort      & 0xFF );   // - low  byte

  SAPDB_memcpy ( &SockAddr.sa_data [ 2 ], pucName, NameLen );

  rc = connect ( sd, (sockaddr_type*)&SockAddr, sizeof(SockAddr) );

  if ( rc < 0 )
    return ( TCPErrno );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_connect )
#endif

/*------------------------------*/

INT sql40c_send_packet ( teo00_Socket                    sd,
                         PCHAR                           pPacket,
                         INT                             SendLen,
                         PINT                            pBytesSend )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_send_packet"

  DBGPAS;

  //
  // --- Send packet.
  //
  *pBytesSend = send ( sd, (PCHAR) pPacket, SendLen, 0 );

  if ( *pBytesSend < 0 )
    return ( TCPErrno );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_send_packet )
#endif

/*------------------------------*/

INT sql40c_recv_packet ( teo00_Socket                    sd,
                         PCHAR                           pPacket,
                         INT                             ReceiveLen,
                         PINT                            pBytesReceived )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_recv_packet"

  DBGPAS;

  //
  // ---  Receive connect packet.
  //
  do
    {
    *pBytesReceived = recv ( sd, (PCHAR) pPacket,
                             ReceiveLen, 0 );
    }
  while ( TCPErrno == SOCEINTR);

  if ( *pBytesReceived < 0 )
    {
    return ( TCPErrno );
    }

  return ( NO_ERROR );
  }


#ifdef STATIC_LINK
 #pragma handler ( sql40c_recv_packet )
#endif

/*------------------------------*/

INT sql40c_select ( PINT                            pSDCount,
                    PVOID                           pReadSDMask,
                    PVOID                           pWriteSDMask,
                    PVOID                           pExceptSDMask,
                    USHORT                          usTimeOut )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_select"
  timeval_type                   TimeOut;
  timeval_type                   *pTimeOut = NULL;

  DBGPAS;

  if ( usTimeOut != (USHORT)UNDEF )
    {
    TimeOut.tv_sec  = usTimeOut;
    TimeOut.tv_usec = 0;
    pTimeOut        = &TimeOut;
    }

  *pSDCount = select( *pSDCount,
                      (fd_set*)pReadSDMask,
                      (fd_set*)pWriteSDMask,
                      (fd_set*)pExceptSDMask,
                      pTimeOut );



  if ( *pSDCount < 0 )
    return ( TCPErrno );

  if ( *pSDCount == 0 )
    return ( ERROR_TIMEOUT );

  return ( NO_ERROR );
  }


#ifdef STATIC_LINK
 #pragma handler ( sql40c_select )
#endif

/*------------------------------*/

INT sql40c_get_serviceport ( teo00_Socket sd, PUSHORT pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_serviceport"
  sockaddr_type                   SockAddr;
  INT                             Len;
  INT                             rc;

  DBGPAS;

  // --- host address is implicit, so only address family and
  //     port number are to be determined
  memset ( &SockAddr, 0, sizeof(sockaddr_type) );
  SockAddr.sa_family = AF_INET;

  Len = sizeof(sockaddr_type);

  // --- bind the socket to the server address
  rc = getsockname ( sd, &SockAddr, &Len );

  if ( rc < 0 )
    return ( TCPErrno );

  *pusServicePort  = (USHORT)(SockAddr.sa_data [ 0 ] & 0xff) << 8;
  *pusServicePort |= (USHORT)(SockAddr.sa_data [ 1 ] & 0xff);

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_get_serviceport )
#endif

/*------------------------------*/

INT sql40c_is_inet_addr_valid ( PCHAR InetAddr )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_is_inet_addr_valid"

  DBGPAS;

  if ( inet_addr( InetAddr ) == INADDR_NONE )
    return -1;

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_is_inet_addr_valid )
#endif

/*------------------------------*/

INT sql40c_bind_on_port_and_inet_addr ( teo00_Socket  sd, 
                                        USHORT        usServicePort,
                                        PCHAR         pszInetAddr )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_bind_on_port_and_inet_addr"
  struct sockaddr_in              Service;
  INT                             rc;

  DBGPAS;

  memset ( &Service, 0, sizeof(Service) );

  Service.sin_family      = AF_INET;
  Service.sin_addr.s_addr = inet_addr(pszInetAddr);
  Service.sin_port        = htons(usServicePort);

  // --- bind the socket to the server address
  rc = bind ( sd, (SOCKADDR*)&Service, sizeof(Service) );

  if ( rc < 0 )
    return ( TCPErrno );

  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
 #pragma handler ( sql40c_bind_on_port_and_inet_addr )
#endif

#endif

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

#ifndef NO_SOCKET

static SOCKET RTE_WSASocket (int     addressFamily,
                             int     protocolType, 
                             int     protocol, 
                             int    *pRc, 
                             DWORD   flags)

{
    SOCKET            sd;                     /* return socket descriptor */
    int               i;
    int               nRet;                   /* number of protocols to parse */

    DWORD             dwBufLen;
    WSAPROTOCOL_INFO  *lpProtocolBuf;
    BOOL              bProtocolFound;       /* indicate if valid protocol found */
    GROUP             group;                /* reserved */
    DWORD             BufRetry = 3;
    
    memset(&group, 0, sizeof(GROUP));

    dwBufLen      = 10 * sizeof(WSAPROTOCOL_INFO);
    lpProtocolBuf = (WSAPROTOCOL_INFO *)HeapAlloc ( GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufLen );

    if ( !lpProtocolBuf )
    {
        *pRc = GetLastError();
        return INVALID_SOCKET;
    }
    
    // First, have WSAEnumProtocols tell you how big a buffer you need.
    nRet = WSAEnumProtocols(NULL, lpProtocolBuf, &dwBufLen);

    if (SOCKET_ERROR == nRet )
    {

        if ( WSAENOBUFS != TCPErrno )
        {
            *pRc = TCPErrno;
            // WSAEnumProtocols failed for some reason not relating to buffer
            // size - also odd.
            HeapFree ( GetProcessHeap(), 0, lpProtocolBuf );
            return INVALID_SOCKET;
        }
        else
        {
            do
            {
                HeapFree ( GetProcessHeap(), 0, lpProtocolBuf );
                
                // WSAEnumProtocols failed for the "expected" reason. Therefore,
                // you need to allocate a buffer of the appropriate size.
                lpProtocolBuf = (WSAPROTOCOL_INFO *)HeapAlloc ( GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufLen );
                
                if ( !lpProtocolBuf )
                {
                    *pRc = GetLastError();
                    return INVALID_SOCKET;
                }
                
                // Now you can call WSAEnumProtocols again with the expectation
                // that it will succeed because you have allocated a big enough
                // buffer.
                nRet = WSAEnumProtocols(NULL, lpProtocolBuf, &dwBufLen);
                
                if (((SOCKET_ERROR == nRet ) && ( WSAENOBUFS != TCPErrno )) || ( BufRetry-- == 0 ) )
                {
                    *pRc = TCPErrno;
                    HeapFree ( GetProcessHeap(), 0, lpProtocolBuf );
                    return INVALID_SOCKET;
                }
            }
            while ( SOCKET_ERROR == nRet );
        }
    }

    
    // Loop through protocols, looking for the first service
    // provider that meets the matching criteria.
    bProtocolFound = FALSE;

    for ( i = 0; i < nRet; i++ )
    {
        if (   ( addressFamily == lpProtocolBuf[i].iAddressFamily)
            && ( protocolType  == lpProtocolBuf[i].iSocketType) )
        {
            bProtocolFound = TRUE;
            break;
        }
    }

    if ( !bProtocolFound )
    {
        HeapFree ( GetProcessHeap(), 0, lpProtocolBuf );
        *pRc = WSAEPFNOSUPPORT;
        return INVALID_SOCKET;
    }
    
    sd = WSASocket( AF_INET, SOCK_STREAM, protocol, &lpProtocolBuf[i], group, flags );

    if ( sd == INVALID_SOCKET )
    {
        *pRc = TCPErrno;
        HeapFree ( GetProcessHeap(), 0, lpProtocolBuf );
        return INVALID_SOCKET;
    }

    HeapFree ( GetProcessHeap(), 0, lpProtocolBuf );

    return sd;
} 

#endif
/*
 * =============================== END ========================================
 */


