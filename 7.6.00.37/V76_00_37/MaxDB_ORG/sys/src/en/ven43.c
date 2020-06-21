/*!
  @file           ven43.c
  @author         JoergM
  @brief          Client RunTime: socket_address_resolution
  @see            


  Get Socket Addresses

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



#define MOD__ "ven43.c:"

#include <sys/utsname.h>

#include "gen00.h"
#include "heo00.h"
#include "vsp001c.h"
#include "gen003.h"
#include "gen43.h"
#include "hen43.h"
#include "geo007_1.h"
#include "hen40.h"
#include "heo46.h"
#include <arpa/inet.h>

#if !defined(INADDR_NONE)
# define INADDR_NONE             -1
#endif

/*===========================================================================*/

void    sqlhostname (
tsp00_NodeId		host ,
tsp00_ErrText		errtext ,
tsp01_CommErr		*returncode )
{
    int				rc ;
    tsp00_NodeIdc	host_c ;

    DBGIN;

    rc = sql43_get_my_host_name ( host_c , sizeof(tsp00_NodeId) );
    if ( rc )
    {
        eo46CtoP ( errtext , "cannot get host name" , sizeof(tsp00_ErrText) );
        *returncode = SP1CE_NOTOK ;
    }
    else
    {
        eo46CtoP ( host , host_c , sizeof(tsp00_NodeId) );
         *returncode = SP1CE_OK ;
    }

    DBG1 (( MF__,"rc %d \n", (int) *returncode ));
    DBGOUT;
}

/*===========================================================================*/

void sqlnodename (
tsp00_NodeId		host ,
tsp00_NodeId		node ,
tsp00_ErrText		errtext ,
tsp01_CommErr		*returncode )
{
    int				rc ;
    tsp00_NodeIdc	host_c ;
    tsp00_NodeIdc	node_c ;

    DBGIN;

    eo46PtoC ( host_c , host , sizeof(tsp00_NodeId) );
    DBG1 (( MF__,"host '%s' \n", host_c ));

#ifdef  SOCKETS
    rc = sql43_get_official_node_name ( host_c , node_c , sizeof(tsp00_NodeId) );
#else	/*SOCKETS*/
    rc = SP1CE_NOTOK ;
#endif
    if ( rc )
    {
        eo46CtoP ( errtext , "cannot get node name" , sizeof(tsp00_ErrText) );
        *returncode = SP1CE_NOTOK ;
    }
    else
    {
        eo46CtoP ( node , node_c , sizeof(tsp00_NodeId) );
        *returncode = SP1CE_OK ;
    }

    DBG1 (( MF__,"rc %d \n", (int) *returncode ));
    DBGOUT;
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql43_get_my_host_name"
int sql43_get_my_host_name (
char                    *host ,
int                     size )
{
    int                     rc ;
    int                     lgt ;
    struct  utsname         unam ;

    DBGIN;

    SAPDB_memset ( host , 0 , size );

    rc = uname ( &unam );
    if ( rc < 0 )
    {
	    MSGD (( ERR_MGH_UNAME_FAILED, sqlerrs() ));
	    return ( - 1 );
    }

    lgt = strlen ( unam.nodename );
    if ( lgt >= size )
    {
	    MSGD (( ERR_MGH_UNAME_TOO_LONG, unam.nodename ));
	    return ( - 1 );
    }

    SAPDB_memcpy ( host , unam.nodename , lgt );

    DBG1 (( MF__,"my host name '%s' \n" , host ));
    DBGOUT;
    return ( 0 );
}


#ifdef  SOCKETS
/*==========================================================================*/


int sql43_get_official_node_name (
char                    *host ,
char                    *node ,
int                     size )
{
    int			    rc ;
    tsp00_C64c		    addr ;

    DBGIN;

    rc = sql43_get_host_by_name ( host ,
					(unsigned char *)addr , sizeof(tsp00_C64c) );
    if ( rc )
    {
		return ( rc );
    }

    rc = sql43_get_host_by_address ( (unsigned char *)addr , node , size );
    if ( rc )
    {
		return ( rc );
    }

    DBG1 (( MF__,"host '%s' node '%s' \n" , host , node ));
    DBGOUT;

    return ( 0 );
}

/*==========================================================================*/
#undef MF__
#define MF__ MOD__"sql43_get_service_by_name"
int sql43_get_service_by_name (
char                    *service ,
unsigned short          *port )
{
    struct servent		*sp ;
    union   map_i2_c2
	{
		tsp00_Int2 i2 ;
		tsp00_C2   c2 ;
	} map;

    DBGIN;

    *port = 0 ;


#ifdef TEST_PORT
    if ( strcmp( service, SOCKET_SERVICE_SQL6_EO003) == 0 )
    {
        *port = 7272;
    } else if ( strcmp( service, SOCKET_SERVICE_SQL30_EO003) == 0 )
    {
        *port = 6161;
    } else if ( strcmp( service, SOCKET_SERVICE_NI72_EO003) == 0 )
    {
        *port = DEFAULT_PORT_SERVICE_NI72_EO003;
    } else
    {
    	MSGD (( ERR_GET_SERV_BY_NAME, service ));
        DBGOUT;
    	return ( -1 );
    }
#else
	sp = getservbyname ( service , SOCKET_PROTOCOL_EO003 );
    if ( ! sp )
    {
        /* PTS 1110452 */
        if ( strcmp( service, SOCKET_SERVICE_SQL6_EO003) == 0 )
        {
            *port = DEFAULT_PORT_SERVICE_SQL6_EO003;
        } else if ( strcmp( service, SOCKET_SERVICE_SQL30_EO003) == 0 )
        {
            *port = DEFAULT_PORT_SERVICE_SQL30_EO003;
        } else if ( strcmp( service, SOCKET_SERVICE_NI72_EO003) == 0 )
        {
            *port = DEFAULT_PORT_SERVICE_NI72_EO003;
        } else if ( strcmp( service, SOCKET_SERVICE_NISSL76_EO003) == 0 )
        {
            *port = DEFAULT_PORT_SERVICE_NISSL76_EO003;
        } else
        {
    		MSGD (( ERR_GET_SERV_BY_NAME, service ));
            DBGOUT;
    		return ( -1 );
        }

        DBG1 (( MF__,"service port for %s default to %d \n" , service, *port ));
        DBGOUT;
        return ( 0 );
    }

    map.i2 = sp->s_port ;
    *port = ((map.c2[0]<<8)&0xff00) | ((map.c2[1])&0x00ff);
#endif /* TEST_PORT */
    DBG1 (( MF__,"service port is      %d \n" , *port ));
    DBGOUT;
    return ( 0 );
}


/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql43_get_host_by_name"
int sql43_get_host_by_name (
char                    *host ,
unsigned char           *addr ,
int                     size )
{
    struct hostent		*hp ;

    DBGIN;
    SAPDB_memset ( addr , 0 , size );

	hp = gethostbyname ( host );
    if ( ! hp )
    {
		MSGD (( ERR_GET_HOST_BY_NAME, host ));
		return ( -1 );
    }

    if ( hp->h_length > size )
    {
		MSGD (( ERR_TCPIP_ADDR_TOO_LONG, size ));
		return ( -1 );
    }

    SAPDB_memcpy ( addr , hp->h_addr , hp->h_length );

#ifdef DEBUG_RTE
    {
	int                     lgt ;
	int                     idx ;
	char                    line [ 20 ];

    	for ( lgt = 0 , idx = 0 ; idx < hp->h_length ; idx ++ )
    	{
    	    (void) sprintf ( line + lgt , "%d." , hp->h_addr[idx]&0xff );
    	    lgt += strlen  ( line + lgt );
    	}
    	DBG1 (( MF__,"host address '%s' \n", line ));
    }
#endif
    DBGOUT;
    return ( 0 );
}


/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql43_get_host_by_address"
int sql43_get_host_by_address (
unsigned char           *addr ,
char                    *host ,
int                     size )
{
    int				rc ;
    int				lgt ;
    struct hostent		*hp ;
    char			*ptr ;
    char			**alipp ;
    char			line [ 20 ];

    DBGIN;

    SAPDB_memset ( host , 0 , size );
	hp = gethostbyaddr ( (char*) addr , INET_ADDR_SIZE , AF_INET );
    if ( ! hp )
    {
		for ( lgt = 0 , rc = 0 ; rc < 4 ; rc ++ )
		{
			(void) sprintf ( line + lgt , "%d." , addr[rc]&0xff );
			lgt += strlen  ( line + lgt );
		}
		MSGD (( ERR_GET_HOST_BY_ADDR, line ));
		return ( -1 );
    }

    ptr = hp->h_name ;
    if ( ! strchr ( ptr , '.' ) )
    {
		for ( alipp = hp->h_aliases ; alipp && *alipp ; alipp ++ )
		{
			if ( strchr ( *alipp , '.' ) )
			{
				ptr = *alipp ;
				break ;
			}
		}
    }

    lgt = strlen ( ptr );
    if ( lgt >= size )
    {
		MSGD (( ERR_TCPIP_HOSTNAME_TOO_LONG, ptr ));
		return ( -1 );
    }

    SAPDB_memcpy ( host , ptr , lgt );
    DBG1 (( MF__,"host name '%s' \n" , ptr ));
    DBGOUT;
    return ( 0 );
}

/*==========================================================================*/

int sql43_is_inet_addr ( char  *InetAddr )
{

  if ( inet_addr( InetAddr ) == INADDR_NONE )
    return FALSE;

  return TRUE;
}


#endif  /*SOCKETS*/
