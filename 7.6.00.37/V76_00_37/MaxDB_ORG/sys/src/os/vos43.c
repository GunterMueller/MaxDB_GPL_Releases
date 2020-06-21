/*!
  @file           vos43.c
  @author         RaymondR
  @brief          socket address resolution
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

#if defined (KERNEL)
# include "gos00k.h"  /* nocheck */
#endif

#include "geo007_1.h"
#include "geo007_2.h"
#include "gos003.h"
#include "hsp100.h"
#include "geo43_0.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS43C : "
#define MF__   MOD__"UNDEFINED"

#define MX_DOT_STR   256

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
static LONG            fTCPIPInstalled;

/*
 * LOCAL FUNCTION PROTOTYPES
 */


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

void sqlhostname ( tsp00_NodeId      host,
                   tsp00_ErrText     errtext,
                   tsp01_CommErr    *returncode )
  {
  #undef  MF__
  #define MF__ MOD__"sqlhostname"
  LONG      rc = NO_ERROR;
  char      szHostName [ sizeof ( tsp00_NodeId ) + 1 ];

  rc = sql43_get_my_host_name ( szHostName, sizeof(tsp00_NodeId) );

  if ( rc != NO_ERROR )
    {
    sql46c_build_error_string ( errtext, ERRMSG_GET_HOST_NAME, rc );
    *returncode = commErrNotOk_esp01;
    }
  else
    {
    eo46CtoP ( host, szHostName, sizeof(tsp00_NodeId) );
    *returncode = commErrOk_esp01;
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

void sqlnodename ( tsp00_NodeId        host,
                   tsp00_NodeId        node,
                   tsp00_ErrText       errtext,
                   tsp01_CommErr      *returncode )
  {
  #undef  MF__
  #define MF__ MOD__"sqlnodename"
  LONG      rc = NO_ERROR;
  char      szHostName [ sizeof ( tsp00_NodeId ) + 1 ];
  char      szNodeName [ sizeof ( tsp00_NodeId ) + 1 ];


  DBGIN;

  eo46PtoC ( szHostName, host, sizeof(tsp00_NodeId) );

  rc = sql43_get_official_node_name ( szHostName, szNodeName, sizeof(tsp00_NodeId) );

  if ( rc != NO_ERROR )
    {
    sql46c_build_error_string ( errtext, ERRMSG_GET_OFFICIAL_NODE_NAME, rc );
    *returncode = commErrNotOk_esp01;
    }
  else
    {
    eo46CtoP ( node, szNodeName, sizeof(tsp00_NodeId) );
    *returncode = commErrOk_esp01;
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

int   sql43_tcpip_init  ()
  {
  #undef  MF__
  #define MF__ MOD__"sql43_tcpip_init"
  ULONG					 rc = NO_ERROR;
  tsp100_VersionID1		 DLLVersion;
  tsp100_VersionResult	 CmpResult;
  #if !defined (USER)
   UCHAR				 cMajorSockVersion;
   UCHAR                 cMinorSockVersion;
  #endif

  DBGIN;
  if ( fTCPIPInstalled )
    {
    DBGOUT;
    return (TCPIP_INIT_OK);
    }


  rc = sql40c_get_dll_version ( &DLLVersion );

  if ( rc == NO_ERROR ) 
    {
	CmpResult = sp100_CompareVersionIDs( &DLLVersion );

    if (( CmpResult == IsOlder_esp100 ) || ( sql40c_sock_init() != 0 ))
	  {
	  DBGOUT;
	  return (TCPIP_INIT_WRONG_VERSION);
	  }

    #if !defined (USER)
     sql40c_get_sock_version ( &cMajorSockVersion, &cMinorSockVersion );

     MSGCD (( INFO_WINSOCK_AND_DLL_VERSION, cMajorSockVersion,
                                            cMinorSockVersion,
                                            DLLVersion.MajorVersion_sp100,
                                            DLLVersion.MinorVersion_sp100,
                                            DLLVersion.CorrLevel_sp100 ))
    #endif
    fTCPIPInstalled = TRUE;

    DBGOUT;
    return (TCPIP_INIT_OK);
    }

  DBGOUT;
  return (TCPIP_INIT_NOTOK);
  }

/*------------------------------*/

void sql43_tcpip_cleanup ()
  {
  #undef  MF__
  #define MF__ MOD__"sql43_tcpip_cleanup"

  DBGIN;

  if ( fTCPIPInstalled == TRUE )
  {
    sql40c_sock_cleanup ();
    fTCPIPInstalled = FALSE;
  }

  DBGOUT;
  return;
  }

/*------------------------------*/

int sql43_is_inet_addr ( char  *InetAddr )
  {
  #undef  MF__
  #define MF__ MOD__"sql43_is_inet_addr"

  DBGPAS;


  return (sql40c_is_inet_addr_valid (InetAddr) == 0);
  }

/*------------------------------*/

int sql43_get_my_host_name ( char  *pszHostName, 
                             int    Size )
  {
  #undef  MF__
  #define MF__ MOD__"sql43_get_my_host_name"
  PSZ             EnvResultPointer;
  LONG            rc = NO_ERROR;

  DBGIN;

  pszHostName[0]      = '\0';
  pszHostName[Size-1] = '\0';

  #if defined(_WIN32)
    if (!GetComputerName( pszHostName, &Size ))
      rc = GetLastError();

    EnvResultPointer = pszHostName;
  #else
    rc = DosScanEnv(HOSTNAME_ENV_VAR, &EnvResultPointer);
  #endif

  if ((rc != NO_ERROR) || (EnvResultPointer[0] == '\0'))
    {
    MSGD ((ERR_MISSING_HOSTNAME));
    DBG1 ((MF__, "'HOSTNAME' not found, rc = %d", rc));
    rc = -1;
    }
  else if (Size < (INT)strlen(EnvResultPointer))
    {
    MSGD ((ERR_HOSTNAME_TOO_LONG, Size));
    DBG1 ((MF__, "'HOSTNAME' too long, allowed are %u characters", Size));
    rc = -1;
    }
  else
    strncpy (pszHostName, EnvResultPointer, Size);


  DBGOUT;
  return (rc);
  }

/*------------------------------*/

int sql43_get_official_node_name ( char *pszHostName, 
                                   char *psNodeName,
                                   int   Size )
  {
  #undef  MF__
  #define MF__ MOD__"sql43_get_official_node_name"
  LONG            rc = NO_ERROR;
  UCHAR           ucAddr[64];

  DBGIN;

  memset ( ucAddr, 0, sizeof(ucAddr) );

  rc = sql43_get_host_by_name ( pszHostName, ucAddr, sizeof(ucAddr) );

  if ( rc == NO_ERROR )
    {
    rc = sql43_get_host_by_address ( ucAddr, psNodeName, Size );
    }

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

int  sql43_get_service_by_name ( char           *pszService, 
                                 unsigned short *pusServicePort )
  {
  #undef  MF__
  #define MF__ MOD__"sql43_get_service_by_name"
  LONG           rc = NO_ERROR;

  DBGIN;

  rc = sql40c_getservbyname ( pszService, pusServicePort );

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

int  sql43_get_host_by_name ( char          *pszHostName, 
                              unsigned char *pucAddr, 
                              int            Size )
  {
  #undef  MF__
  #define MF__ MOD__"sql43_get_host_by_name"
  LONG          rc = NO_ERROR;

  DBGIN;


  memset ( pucAddr , 0 , Size );

  rc = sql40c_gethostbyname ( pszHostName, pucAddr, &Size );

  if ( rc != NO_ERROR )
    {
    if ( rc == EINVAL )
      {
      MSGD ((ERR_TCPIP_ADDR_TOO_LONG, Size));
      DBG1 ((MF__, "TCP/IP address too long, allowed size: %u ", Size));
      }
    else
      {
      DBG1 ((MF__, "TCP/IP host name '%s' not found", pszHostName));
      MSGD ((ERR_GET_HOST_BY_NAME, pszHostName));
      }
    }
  else
    {
    #ifdef DEBUG_RTE
    {
    INT     l , i ;
    CHAR    szDotAddr [ MX_DOT_STR ];

    for ( l = 0 , i = 0 ; i < Size - 1; i ++ )
      {
      sprintf ( szDotAddr + l , "%d." , pucAddr[ i ] );
      l += strlen  ( szDotAddr + l );
      }
    sprintf ( szDotAddr + l , "%d" , pucAddr[ i ] );

    DBG3((MF__, "The address of '%s' is: %s", pszHostName, szDotAddr));
    }
    #endif
    ;
    }

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

int sql43_get_host_by_address ( unsigned char *pucAddr,
                                char          *pszHostName,
                                int            Size )
  {
  #undef  MF__
  #define MF__ MOD__"sql43_get_host_by_address"
  LONG             rc = NO_ERROR;
  INT              l , i;
  CHAR             szDotAddr [ MX_DOT_STR ];

  DBGIN;

  pszHostName[0]      = '\0';
  pszHostName[Size-1] = '\0';


  rc = sql40c_gethostbyaddr ( pszHostName, pucAddr, Size );

  if ( rc != NO_ERROR )
    {
    if ( rc == EINVAL )
      {
      MSGD ((ERR_TCPIP_HOSTNAME_TOO_LONG, Size));
      DBG1 ((MF__, "TCP/IP host name too long, allowed size: %u", Size));
      }
    else
      {
      for ( l = 0 , i = 0 ; i < INET_ADDR_SIZE - 1; i ++ )
        {
        sprintf ( szDotAddr + l , "%d.", pucAddr [ i ] & 0xFF  );
        l += (INT)strlen  ( szDotAddr + l );
        }
      sprintf ( szDotAddr + l , "%d", pucAddr [ i ] & 0xFF  );

      DBG1 ((MF__, "Error getting TCP/IP host by address: '%s'", szDotAddr));
      MSGD ((ERR_GET_HOST_BY_ADDR, szDotAddr));
      }
    }
  else
    {
    #ifdef DEBUG_RTE
    {
    for ( l = 0 , i = 0 ; i < INET_ADDR_SIZE - 1; i ++ )
      {
      sprintf ( szDotAddr + l , "%d." , pucAddr [ i ] & 0xFF );
      l += strlen  ( szDotAddr + l );
      }
    sprintf ( szDotAddr + l , "%d" , pucAddr [ i ] & 0xFF );
    DBG3((MF__, "The name of address %s is: '%s'", szDotAddr, pszHostName));
    }
    #endif
    ;
    }

  DBGOUT;
  return (rc);
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

