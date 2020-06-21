/*!
  @file           vos40l.c
  @author         RaymondR
  @brief          TCP/IP Stub
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
#include "geo007_1.h"
#include "hsp100.h"
#include "heo01.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS40LC : "
#define MF__   MOD__"UNDEFINED"



/*
 *  MACROS
 */
#define MK_PA(f)       (*lp##f)

#define LOAD_FUNC(f)   ((lp##f = sql40c_load_tcp_func(#f)) == NULL) ? \
                         -1 : NO_ERROR

#define CALL_FUNC(f,p) ( lp##f == NULL ) ? sql40c_func_err(#f),-1 :   \
                                           lp##f##p;

/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

/*
 * EXTERNAL VARIABLES
 */


extern HANDLE       hModule;


/*
 * LOCAL VARIABLES
 */
static HINSTANCE    hinstTCPIPDLL = NULL;

static BOOL MK_PA (sql40c_sock_init)           ( VOID );

static BOOL MK_PA (sql40c_get_dll_version)     ( tsp100_VersionID1 *pVersion );

static BOOL MK_PA (sql40c_get_sock_version)    ( UCHAR             *pcMajorSockVersion,
                                                 UCHAR             *pcMinorSockVersion );
static BOOL MK_PA (sql40c_sock_cleanup)        ( VOID );

static INT  MK_PA (sql40c_getservbyname)       ( PCHAR             pszService,
                                                 PUSHORT           pusServicePort );
static INT  MK_PA (sql40c_gethostbyname)       ( PSZ               pszHostName,
                                                 PUCHAR            pucAddr,
                                                 PINT              pAddrLen );
static INT  MK_PA (sql40c_gethostbyaddr)       ( PSZ               pszHostName,
                                                 PUCHAR            pucAddr,
                                                 LONG              lAddrLen );
static INT  MK_PA (sql40c_gethostbyaddr)       ( PSZ               pszHostName,
                                                 PUCHAR            pucAddr,
                                                 LONG              lAddrLen );
static INT  MK_PA (sql40c_stream_socket)       ( teo00_Socket      *psd );
static INT  MK_PA (sql40c_stream_socket2)      ( teo00_Socket      *psd,
                                                 DWORD             flags);

static INT  MK_PA (sql40c_socket_close)        ( teo00_Socket      sd );
static INT  MK_PA (sql40c_accept_conn_request) ( teo00_Socket      sd,
                                                 teo00_Socket      *psdClient,
                                                 PVOID             pSockAddr,
                                                 PINT              pLen );
static INT  MK_PA (sql40c_keep_alive)          ( teo00_Socket      sd );
static INT  MK_PA (sql40c_linger)              ( teo00_Socket      sd,
                                                 BOOL              fLinger,
                                                 INT               iSeconds );
static INT  MK_PA (sql40c_bind)                ( teo00_Socket      sd,
                                                 USHORT            usServicePort );
static INT  MK_PA (sql40c_listen)              ( teo00_Socket      sd,
                                                 INT               Count );
static INT  MK_PA (sql40c_set_send_buffer_size)( teo00_Socket      sd,
                                                 PINT              pMaxSize,
                                                 INT               MinSize );
static INT  MK_PA (sql40c_set_recv_buffer_size)( teo00_Socket      sd,
                                                 PINT              pMaxSize,
                                                 INT               MinSize );
static INT  MK_PA (sql40c_connect)             ( teo00_Socket      sd,
                                                 USHORT            usServicePort,
                                                 PUCHAR            pucName,
                                                 INT               NameLen );
static INT  MK_PA (sql40c_send_packet)         ( teo00_Socket      sd,
                                                 PCHAR             pPacket,
                                                 INT               SendLen,
                                                 PINT              pBytesSend );
static INT  MK_PA (sql40c_recv_packet)         ( teo00_Socket      sd,
                                                 PCHAR             pPacket,
                                                 INT               ReceiveLen,
                                                 PINT              pBytesReceived );
static INT  MK_PA (sql40c_select)              ( PINT              pSDCount,
                                                 PVOID             pReadSDMask,
                                                 PVOID             pWriteSDMask,
                                                 PVOID             pExceptSDMask,
                                                 USHORT            usTimeOut );
static INT  MK_PA (sql40c_get_serviceport)     ( teo00_Socket      sd,
                                                 PUSHORT           pusServicePort );

static INT  MK_PA (sql40c_is_inet_addr_valid)         ( PCHAR InetAddr );

static INT  MK_PA (sql40c_bind_on_port_and_inet_addr) ( teo00_Socket  sd, 
                                                        USHORT        usServicePort,
                                                        PCHAR         pszInetAddr );


/*
 * LOCAL FUNCTION PROTOTYPES
 */
static VOID* sql40c_load_tcp_func ( PSZ  pszFuncName );
static VOID  sql40c_func_err      ( PSZ  pszFuncName );
static ULONG sql40c_load_functions( VOID );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

ULONG sql40c_get_dll_version ( tsp100_VersionID1 *pVersion )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_dll_version"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = LOAD_FUNC( sql40c_get_dll_version );

  if ( rc != NO_ERROR )
    {
    memset ( pVersion, 0, sizeof(*pVersion));
    return ( rc );
    }

  CALL_FUNC( sql40c_get_dll_version,( pVersion ) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_sock_init ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_sock_init"
  INT  rc = NO_ERROR;

  rc = sql40c_load_functions();
  if ( rc != NO_ERROR ) return ( rc );

  rc = CALL_FUNC( sql40c_sock_init,() );

  return ( rc );
  }


/*------------------------------*/

INT sql40c_get_sock_version ( UCHAR *pcMajorSockVersion,
                              UCHAR *pcMinorSockVersion )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_sock_version"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_get_sock_version,(pcMajorSockVersion,
                                           pcMinorSockVersion) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_sock_cleanup ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_sock_cleanup"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_sock_cleanup, () );

  return ( rc );
  }


/*------------------------------*/

INT sql40c_getservbyname ( PCHAR pszService, PUSHORT pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_getservbyname"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_getservbyname, (pszService, pusServicePort) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_gethostbyname ( PSZ      pszHostName,
                           PUCHAR   pucAddr,
                           PINT     pAddrLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_gethostbyname"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_gethostbyname, (pszHostName, pucAddr, pAddrLen) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_gethostbyaddr ( PSZ      pszHostName,
                           PUCHAR   pucAddr,
                           LONG     lAddrLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_gethostbyaddr"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_gethostbyaddr, (pszHostName, pucAddr, lAddrLen) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_stream_socket ( teo00_Socket  *psd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_socket"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_stream_socket, (psd) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_stream_socket2 ( teo00_Socket  *psd,
                            DWORD          flags)
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_stream_socket2"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_stream_socket2, (psd, flags) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_socket_close ( teo00_Socket  sd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_socket_close"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_socket_close, (sd) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_accept_conn_request ( teo00_Socket    sd,
                                 teo00_Socket    *psdClient,
                                 PVOID		     pSockAddr,
                                 PINT			 pLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_accept"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC(sql40c_accept_conn_request,(sd, psdClient, pSockAddr, pLen));

  return ( rc );
  }

/*------------------------------*/

INT sql40c_keep_alive ( teo00_Socket  sd )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_keep_alive"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_keep_alive, (sd) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_linger ( teo00_Socket  sd, BOOL fLinger, INT iSeconds )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_linger"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_linger, (sd, fLinger, iSeconds) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_bind ( teo00_Socket  sd, USHORT usServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_bind"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_bind, (sd, usServicePort) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_listen ( teo00_Socket  sd, INT Count )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_listen"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_listen, (sd, Count ) );

  return ( rc );
  }


/*------------------------------*/

INT sql40c_set_send_buffer_size ( teo00_Socket  sd, PINT pMaxSize, INT MinSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_set_send_buffer_size"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_set_send_buffer_size,
                  (sd, pMaxSize, MinSize) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_set_recv_buffer_size ( teo00_Socket  sd, PINT pMaxSize, INT MinSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_set_recv_buffer_size"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_set_recv_buffer_size,
                  (sd, pMaxSize, MinSize) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_connect( teo00_Socket  sd, USHORT usServicePort, PUCHAR pucName, INT NameLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_connect"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_connect,
                  (sd, usServicePort, pucName, NameLen) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_send_packet ( teo00_Socket                    sd,
                         PCHAR                           pPacket,
                         INT                             SendLen,
                         PINT                            pBytesSend )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_send_packet"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_send_packet,
                  (sd, pPacket, SendLen, pBytesSend) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_recv_packet ( teo00_Socket                    sd,
                         PCHAR                           pPacket,
                         INT                             ReceiveLen,
                         PINT                            pBytesReceived )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_recv_packet"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_recv_packet,
                  (sd, pPacket, ReceiveLen, pBytesReceived) );

  return ( rc );
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
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_select, (pSDCount, pReadSDMask,
                  pWriteSDMask, pExceptSDMask, usTimeOut) );

  return ( rc );
  }

/*------------------------------*/

INT sql40c_get_serviceport ( teo00_Socket sd, PUSHORT pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_serviceport"
  INT  rc = NO_ERROR;

  DBGPAS;

  rc = CALL_FUNC( sql40c_get_serviceport, (sd, pusServicePort) );

  return ( rc );
  }


/*------------------------------*/

INT sql40c_is_inet_addr_valid ( PCHAR InetAddr )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_is_inet_addr_valid"

  DBGPAS;

  return CALL_FUNC( sql40c_is_inet_addr_valid, (InetAddr) );
  }

/*------------------------------*/

INT sql40c_bind_on_port_and_inet_addr ( teo00_Socket  sd, 
                                        USHORT        usServicePort,
                                        PCHAR         pszInetAddr )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_bind_on_port_and_inet_addr"

  DBGPAS;

  return CALL_FUNC( sql40c_bind_on_port_and_inet_addr, (sd, usServicePort, pszInetAddr ) );
  }

/*------------------------------*/

LONG sql40c_set_tcpip_dll_path ( PSZ  pszTCPIPDLLName )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_set_tcpip_dll_path"
  REG_ENTRY_REC    RegistryEntries[1];
  LONG             rc    = NO_ERROR;

  RegistryEntries[0].pszValueName = REG_VN_TCPIP_DLL;
  RegistryEntries[0].pValue       = pszTCPIPDLLName;
  RegistryEntries[0].ulValueSize  = (ULONG)strlen(pszTCPIPDLLName);
  RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;

  rc = sql50_reg_put_applic_values ( NULL, NULL, HKEY_LOCAL_MACHINE,
                                     REG_SK_DLLS, NULL, NULL, 1, RegistryEntries );

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

ULONG sql40c_get_tcpip_dll_path ( PSZ  *ppszTCPIPDLLName )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_get_tcpip_dll_path"
  REG_ENTRY_REC    RegistryEntries[1];
  LONG             rc    = NO_ERROR;
  static PATHNAME  szDLL;

  RegistryEntries[0].pszValueName = REG_VN_TCPIP_DLL;
  RegistryEntries[0].pValue       = szDLL;
  RegistryEntries[0].ulValueSize  = sizeof(szDLL);
  RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;

  rc = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE, REG_SK_DLLS,
                                     1, RegistryEntries );

  if ( rc != NO_ERROR )
    {
    RegistryEntries[0].ulValueType  = REG_SZ;

    rc = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE, REG_SK_DLLS,
                                       1, RegistryEntries );
    }

  if ((rc == NO_ERROR) && (szDLL[0] != '\0' ))
    *ppszTCPIPDLLName = szDLL;
  else
    *ppszTCPIPDLLName = "";


  DBGOUT;
  return ( rc );
  }


/*------------------------------*/

BOOL sql40c_free_dll( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_free_dll"
  BOOL    fOK = FALSE;

  DBGIN;

  if ( hinstTCPIPDLL != NULL )
    {
    fOK = FreeLibrary( hinstTCPIPDLL );

    hinstTCPIPDLL = NULL;
    }

  DBGOUT;
  return ( fOK );
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static ULONG sql40c_load_functions(  VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_load_functions"
  LONG             rc                  = NO_ERROR;
  static BOOL      fAllFunctionsLoaded = FALSE;

  DBGIN;

  if ( !fAllFunctionsLoaded )
  {
    rc = LOAD_FUNC( sql40c_sock_init );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_get_dll_version );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_get_sock_version );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_sock_cleanup );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_getservbyname );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_gethostbyname );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_gethostbyaddr );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_gethostbyaddr );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_stream_socket );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_stream_socket2 );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_socket_close );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_accept_conn_request );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_keep_alive );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_linger );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_bind );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_listen );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_set_send_buffer_size );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_set_recv_buffer_size );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_connect );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_send_packet );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_recv_packet );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_select );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_get_serviceport );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_is_inet_addr_valid );
    if ( rc != NO_ERROR ) return ( rc );

    rc = LOAD_FUNC( sql40c_bind_on_port_and_inet_addr );
    if ( rc != NO_ERROR ) return ( rc );

    fAllFunctionsLoaded = TRUE;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static VOID* sql40c_load_tcp_func(  PSZ  pszFuncName )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_load_tcp_func"
  static PSZ       pszDLL;
  HINSTANCE        hinst = NULL;
  LONG             rc    = NO_ERROR;
  FARPROC          pFunc;
  tsp00_Pathc      szDLLPath;
  CHAR             *pszTmp;
  DWORD            dwLen;

  DBGIN;

  if ( hinstTCPIPDLL == NULL )
    {
    rc = sql40c_get_tcpip_dll_path ( &pszDLL );

    if ((rc != NO_ERROR) || ( !pszDLL[0] ))
      {
      pszDLL = DEFAULT_TCPIP_DLL;
      MSGCD (( INFO_LOADING_DEFAULT_TCPIP, pszDLL ));
      }


    if (strchr(pszDLL, '\\' ) == NULL)
      {
      // if there is no absolut path, check the directory of the calling program
      if ( hModule != INVALID_HANDLE_VALUE )
      {
        dwLen = GetModuleFileName(hModule, szDLLPath, sizeof(szDLLPath) - 1);
      }
      else
      {
        dwLen = GetModuleFileName(NULL, szDLLPath, sizeof(szDLLPath) - 1);
      }

      if (dwLen != 0) 
        {
        pszTmp = strrchr(szDLLPath, '\\' );
        
        if ( pszTmp != NULL )
          {
          pszTmp[1] = 0;
          strcat( szDLLPath,pszDLL );
          hinst = LoadLibrary( szDLLPath );
          }
	    }
        // PTS  1104558
        if ( hinst == NULL )
        {
          // now try the global independent path ...
          tsp01_RteError          RteError ;
          if ( sqlGetIndependentPgmPath ( szDLLPath, TERM_WITH_DELIMITER_EO01, &RteError ) )
          {
            strcat( szDLLPath,pszDLL );
            hinst = LoadLibrary( szDLLPath );
          }
        }
      }

    if ( hinst == NULL )
      hinst = LoadLibrary( pszDLL );

    if ( hinst == NULL )
      {
      rc = GetLastError();

      sql60_enable_message_box ();

      MSGALL (( ERR_LOADING_TCPIP_DLL, rc ));

      sql60_disable_message_box ();

      DBGOUT;
      return ( NULL );
      }

    hinstTCPIPDLL = hinst;
    }

  pFunc = GetProcAddress( hinstTCPIPDLL, pszFuncName);

  if ( pFunc == NULL )
    {
    sql60_enable_message_box ();

    MSGALL (( ERR_LOADING_FUNCTION, pszFuncName, pszDLL ));

    sql60_disable_message_box ();
    }

  DBGOUT;
  return ( pFunc );
  }

/*------------------------------*/

static VOID sql40c_func_err( PSZ  pszFuncName )
  {
  #undef  MF__
  #define MF__ MOD__"sql40c_func_err"

  sql60_enable_message_box();

  MSGALL (( ERR_FUNCTION_NOT_LOADED, pszFuncName ));

  sql60_disable_message_box();

  return;
  }


/*
 * =============================== END ========================================
 */
