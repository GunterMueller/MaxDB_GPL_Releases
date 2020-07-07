/*!
  @file           ven42.c
  @author         JoergM
  @brief          Client RunTime: remote_sql_functions
  @see


  create/extract/read/write remote sql_packets

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



//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//
\endif
*/



#define MOD__ "ven42.c:"

#include    "gen00.h"
#include    "heo00.h"
#include    "heo420.h"
#include    "vsp001c.h"
#include    "gen003.h"
#include    "hsp77.h"

#include "gen500.h"
/* exported functions */

#include "gen42.h"
#include "hen42.h"

/* imported functions */

#include "gen43.h"
#ifdef DEBUG_DEBUG
#include "gen46.h"
#endif /*DEBUG_DEBUG*/
#include "heo46.h"
#include    "hen40.h"
#include "RunTime/RTE_saveUNIXcalls.h"
#include "gen60.h"
#include "geo007_2.h"


#ifdef FREEBSD  /* found somewhere in the net, but not in the devil itself */
#define EPROTO 71
#endif

/* local functions */

static  int     sql42_rcvpkt ( int         sd ,
							   rte_header *header ,
						   	   int         swap_type ,
							   long        siz ,
							   tsp00_ErrTextc pErrText,
							   char       *cache_buf ,
							   long       *cache_lgt );

static  int     sql42_set_tcp_nodelay_flag ( int sd );

static  void    sql42CreatePacketHeader         (rte_header *       header,
                                                 int			    messclass,
                                                 tsp00_Int4         myref,
                                                 tsp00_Int4         peerref,
                                                 SAPDB_UInt4        sendLength,
                                                 int			    retcode);

static void     sql42_ExtractRTEHeader          (int                swapType,
                                                 rte_header *       pHeader);

static SAPDB_UInt1  sql42_OldSwapType           ();

#ifdef SOCKETS
static tsp00_Uint sql42ReceiveCertificatePacket  (void *                    pRecvHandle,
                                                  RECV_PKT_FUNC_PTR_EO003   pRecvFunc,
                                                  SAPDB_Byte *              pServerCertificate,
                                                  SAPDB_UInt4               maxLenCertificate,
                                                  tsp00_ErrTextc            pErrText);
#endif
/* ==================================================================== */

tsp00_Uint1 sql42_new_swap_type ()
{
    static  tsp00_Uint1               SwapType = (tsp00_Uint1)UNDEF_SP00;

    if ( SwapType == (tsp00_Uint1)UNDEF_SP00 )
    {
        union { tsp00_Int8         int8;
                tsp00_C8           c8; }  SwapCheck;

        /* --- set int8 to 0x100000000 */
        SwapCheck.int8   = 0x01;
        SwapCheck.int8 <<= 32;

        for ( SwapType = 0;
              ( SwapType < 8 ) && ( SwapCheck.c8[ SwapType ] != 1 );
              SwapType++ )
        { ; }
    }

    return SwapType;
}


/*
 *  Here, we have some functions which use the var_part of the connect
 *  packet. The var_part can contain several information and is
 *  structured as follows:
 *
 *  var_part: [argument][argument]...
 *
 *  Each argument is structured as follows:
 *
 *  argument: |length|id|information|
 *
 *            'length'      is a one byte integer.
 *                          It includes the length and id-bytes.
 *            'id'          is a one byte character
 *            'information' is coded argument dependent
 *
 *  Example:
 *
 *      var_part: 04 50 1b 58 06 49 31 32 33 00 (hexadecimal)
 *                ^  ^  ^     ^  ^  ^
 *                |  |  |     |  |  |
 *                |  |  |     |  |  NUL terminated string ("123")
 *                |  |  |     |  Argument id 'I' for remote pid
 *                |  |  |     Argument length (6 bytes: length,id,string,\0)
 *                |  |  |
 *                |  |  TCP/IP port number (0x1b58)
 *                |  Argument id 'P' for TCP/IP port number
 *                Argument length (4 bytes: length,id,port number)
 *
 *  There is no terminator for the var_part since the length of the
 *  connect packet includes the var_part and thus specifies its length.
 */

/* ==================================================================== */
#ifdef SOCKETS

void	sql42_put_port_no (
rte_header      *header ,
struct sockaddr *address )
{
#undef	MF__
#define	MF__	"sql42_put_port"
    int                 idx ;
    rte_connect_packet *cpack ;

    /*
     *  Put the TCP/IP port number into the var_part
     *  of a connect packet.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the first free byte of the var_part.
     */
    idx = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;

    /*
     *  The port number is located at the first two bytes of the
     *  data part of the socket address.
     */
    cpack->cp_var_part[idx++] = 4 ; /* length, argument id, 2 byte address */
    cpack->cp_var_part[idx++] = RSQL_OPT_PORT_NO_EO003 ;
    SAPDB_memcpy ( cpack->cp_var_part + idx , address->sa_data , 2 );

    /*
     *  Extend the packet length.
     */
    cpack->cp_connect_length += 4 ;
    header->rh_act_send_len  += 4 ;
    header->rh_max_send_len  += 4 ;

    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x %02x \n",
	    idx-2 ,
        cpack->cp_var_part[idx-2]&0xff ,
        cpack->cp_var_part[idx-1] ,
	    cpack->cp_var_part[idx  ]&0xff ,
        cpack->cp_var_part[idx+1]&0xff ))
}

/* ==================================================================== */

void sql42_get_port_no (
rte_header      *header ,
struct sockaddr *address )
{
#undef	MF__
#define	MF__	"sql42_get_port"
    int				    idx ;
    int				    lgt ;
    rte_connect_packet *cpack ;

    /*
     *  Get the TCP/IP port number from the var_part
     *  of a connect packet.
     *
     *  If the packet has no port number, the 'address' is unchanged.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the argument in the var_part.
     *  'lgt' holds the total length of the var_part.
     */
    lgt = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;
    DBG1 (( MF__,"var_part length %d \n", lgt ));
    for ( idx = 0 ; idx < lgt ; idx += cpack->cp_var_part[idx]&0xff )
    {
	    DBG1 (( MF__,"off %d, lgt %d, id '%c' \n", idx ,
		    cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ));
	    if ( (cpack->cp_var_part[idx]&0xff) < 2 )
	    {
	        idx = lgt ; /* illegal var_part contents */
	        break ;
	    }
	    if ( cpack->cp_var_part[idx+1] == RSQL_OPT_PORT_NO_EO003 ) break ;
    }
    if ( idx >= lgt )
    {
	    /*
	     *  This is kind of a warning. 'address' remains unchanged.
	     */
	    DBG1 (( MF__,"no port number in connect packet \n" ));
	    return ;
    }
    if ( (cpack->cp_var_part[idx]&0xff) != 4 )
    {
	    /*
	     *  This is kind of a warning. 'address' remains unchanged.
	     */
	    DBG1 (( MF__,"ill portno arg lgt %d \n", cpack->cp_var_part[idx]&0xff ));
	    return ;
    }
    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x %02x \n",
	    idx , cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ,
	    cpack->cp_var_part[idx+2]&0xff , cpack->cp_var_part[idx+3]&0xff ))

    /*
     *  The port number is located at the first two bytes of the
     *  data part of the socket address.
     */
    SAPDB_memcpy ( address->sa_data , cpack->cp_var_part + idx + 2 , 2 );
}

/*
 * ===========================================================================
 */

void sqldbgaddress (
char            * text ,
struct sockaddr * sap )
{
    int                     i ;
    int                     lgt ;
    char                    line [ 256 ] ;


    lgt = 0 ;
    sp77sprintf ( line, sizeof(line), "f %d " , sap->sa_family );
    lgt += strlen ( line + lgt );

    sp77sprintf ( line + lgt , sizeof(line) - lgt, "p %d " ,
	    (((sap->sa_data[0]<<8)&0xFF00) | ((sap->sa_data[1])&0x00FF)) );
    lgt += strlen ( line + lgt );

    sp77sprintf ( line + lgt , sizeof(line) - lgt, "h %d.%d.%d.%d" ,
	   sap->sa_data [ 2 ] & 0xFF , sap->sa_data [ 3 ] & 0xFF ,
	   sap->sa_data [ 4 ] & 0xFF , sap->sa_data [ 5 ] & 0xFF );
    lgt += strlen ( line + lgt );

    sqldbgwrite ( text , line );
}

#else	/*SOCKETS*/
/*
 * ===========================================================================
 */

/*VARARGS2*/
void sqldbgaddress (
char                            * text ,
char /*struct sockaddr*/       	* sap )
{
    sqldbgwrite ( text , "ven42c: -DSOCKETS missing" );
}

#endif /*SOCKETS*/

/* ==================================================================== */

void sql42_put_string (
rte_header			*header ,
char				type ,
char				*str )
{
#undef	MF__
#define	MF__	"sql42_put_string"
    int				idx ;
    int				lgt ;
    rte_connect_packet		*cpack ;


    /*
     *  Put a string into the var_part of a connect packet.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the first free byte of the var_part.
     */
    idx = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;

    /*
     *  Copy the string to the packet.
     */
    lgt = strlen ( str ) + 1 ; /* include '\0' */
    cpack->cp_var_part[idx++] = 2 + lgt ; /* length, argument id, string */
    cpack->cp_var_part[idx++] = type ;
    SAPDB_memcpy ( cpack->cp_var_part + idx , str , lgt );

    /*
     *  Extend the packet length.
     */
    lgt += 2 ; /* length byte, argument id */
    cpack->cp_connect_length += lgt ;
    header->rh_act_send_len  += lgt ;
    header->rh_max_send_len  += lgt ;
    DBG1 (( MF__,"off %d, lgt %d, id '%c', string '%s' \n",
	    idx-2 , cpack->cp_var_part[idx-2]&0xff , cpack->cp_var_part[idx-1] ,
	    cpack->cp_var_part + idx ));
}

/* ==================================================================== */

void sql42_get_string (
rte_header *header ,
char        type ,
int         size ,
char       *str )
{
#undef	MF__
#define	MF__	"sql42_get_string"
    int				idx ;
    int				lgt ;
    rte_connect_packet		*cpack ;


    /*
     *  Get a string from the var_part of a connect packet.
     *
     *  If the packet has no string of requested type, string is unchanged.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the argument in the var_part.
     *  'lgt' holds the total length of the var_part.
     */
    lgt = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;
    DBG1 (( MF__,"var_part length %d \n", lgt ));
    for ( idx = 0 ; idx < lgt ; idx += cpack->cp_var_part[idx]&0xff )
    {
	    DBG1 (( MF__,"off %d, lgt %d, id '%c' \n", idx ,
		    cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ));
	    if ( (cpack->cp_var_part[idx]&0xff) < 2 )
	    {
	        idx = lgt ; /* illegal var_part contents */
	        break ;
	    }
	    if ( cpack->cp_var_part[idx+1] == type ) break ;
    }
    if ( idx >= lgt )
    {
	    /*
	     *  This is kind of a warning. 'string' remains unchanged.
	     */
	    DBG1 (( MF__,"no option '%c' in connect packet \n", type ));
	    return ;
    }
    /* we expect at least one character and the NUL terminator */
    lgt = (cpack->cp_var_part[idx]&0xff) - 2 ; /* length byte, argument id */
    if ( (lgt < 1) || (lgt > size) ||
	 (cpack->cp_var_part[idx+2+lgt-1] != '\0') )
    {
	    /*
	     *  This is kind of a warning. 'string' remains unchanged.
	     */
	    DBG1 (( MF__,": ill string '%c' arg lgt %d siz %d \n", type , lgt+2 , size ));
	    return ;
    }
    DBG1 (( MF__,"off %d, lgt %d, id '%c', str '%s' \n",
	    idx, cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ,
					    cpack->cp_var_part + idx + 2 ));

    /*
     *  Copy the string from the packet.
     */
    SAPDB_memcpy ( str , cpack->cp_var_part + idx + 2 , lgt );
}

/* ==================================================================== */

void sql42_put_int1 (
rte_header *header ,
char        type ,
tsp00_Uint1    value )
{
#undef	MF__
#define	MF__	"sql42_put_int1"
    int				idx ;
    rte_connect_packet		*cpack ;

    /*
     *  Put an int4 into the var_part of a connect packet.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the first free byte of the var_part.
     */
    idx = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;

    /*
     *  Set the option entry.
     */
    cpack->cp_var_part[idx++] = 3 ; /* length, argument id, 1 byte value */
    cpack->cp_var_part[idx++] = type ;
    SAPDB_memcpy ( cpack->cp_var_part + idx , &value , 1 );

    /*
     *  Extend the packet length.
     */
    cpack->cp_connect_length += 3 ;
    header->rh_act_send_len  += 3 ;
    header->rh_max_send_len  += 3 ;
    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x \n",
	    idx-2 , cpack->cp_var_part[idx-2]&0xff , cpack->cp_var_part[idx-1] ,
	    cpack->cp_var_part[idx]&0xff ));
}

/* ==================================================================== */

void sql42_get_int1 (
rte_header *header ,
char        type ,
tsp00_Uint1   *valuep )
{
#undef	MF__
#define	MF__	"sql42_get_int1"
    int				idx ;
    int				lgt ;
    rte_connect_packet		*cpack ;

    /*
     *  Get an int4 from the var_part of a connect packet.
     *
     *  If the packet has no int4 of requested type, '*valuep' is unchanged.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the argument in the var_part.
     *  'lgt' holds the total length of the var_part.
     */
    lgt = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;
    DBG1 (( MF__,"var_part length %d \n", lgt ));
    for ( idx = 0 ; idx < lgt ; idx += cpack->cp_var_part[idx]&0xff )
    {
	    DBG1 (( MF__,"off %d, lgt %d, id '%c' \n", idx ,
		    cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ));
	    if ( (cpack->cp_var_part[idx]&0xff) < 2 )
	    {
	        idx = lgt ; /* illegal var_part contents */
	        break ;
	    }
	    if ( cpack->cp_var_part[idx+1] == type ) break ;
    }
    if ( idx >= lgt )
    {
	    /*
	     *  This is kind of a warning. '*valuep' remains unchanged.
	     */
	    DBG1 (( MF__,"no option '%c' in connect packet \n", type ));
	    return ;
    }
    if ( (cpack->cp_var_part[idx]&0xff) != 3 )
    {
	    /*
	     *  This is kind of a warning. '*valuep' remains unchanged.
	     */
	    DBG1 (( MF__,"ill int4 arg lgt %d \n", cpack->cp_var_part[idx]&0xff ));
	    return ;
    }
    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x \n",
	    idx , cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ,
	    cpack->cp_var_part[idx+2]&0xff ));

    /*
     *  Copy the value from the packet.
     */
    SAPDB_memcpy ( valuep , cpack->cp_var_part + idx + 2 , 1 );
}

/* ==================================================================== */

void sql42_put_int4 (
rte_header *header ,
char        type ,
tsp00_Int4    value )
{
#undef	MF__
#define	MF__	"sql42_put_int4"
    int				idx ;
    rte_connect_packet		*cpack ;

    /*
     *  Put an int4 into the var_part of a connect packet.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the first free byte of the var_part.
     */
    idx = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;

    /*
     *  Set the option entry.
     */
    cpack->cp_var_part[idx++] = 6 ; /* length, argument id, 4 byte value */
    cpack->cp_var_part[idx++] = type ;
    SAPDB_memcpy ( cpack->cp_var_part + idx , &value , 4 );

    /*
     *  Extend the packet length.
     */
    cpack->cp_connect_length += 6 ;
    header->rh_act_send_len  += 6 ;
    header->rh_max_send_len  += 6 ;
    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x %02x %02x %02x \n",
	    idx-2 , cpack->cp_var_part[idx-2]&0xff , cpack->cp_var_part[idx-1] ,
	    cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1]&0xff ,
	    cpack->cp_var_part[idx+2]&0xff , cpack->cp_var_part[idx+3]&0xff ));
}

/* ==================================================================== */

void sql42_get_int4 (
rte_header *header ,
char        type ,
tsp00_Int4   *valuep )
{
#undef	MF__
#define	MF__	"sql42_get_int4"
    int				idx ;
    int				lgt ;
    rte_connect_packet		*cpack ;

    /*
     *  Get an int4 from the var_part of a connect packet.
     *
     *  If the packet has no int4 of requested type, '*valuep' is unchanged.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the argument in the var_part.
     *  'lgt' holds the total length of the var_part.
     */
    lgt = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;
    DBG1 (( MF__,"var_part length %d \n", lgt ));
    for ( idx = 0 ; idx < lgt ; idx += cpack->cp_var_part[idx]&0xff )
    {
    	DBG1 (( MF__,"off %d, lgt %d, id '%c' \n", idx ,
	    	cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ));
	    if ( (cpack->cp_var_part[idx]&0xff) < 2 )
	    {
	        idx = lgt ; /* illegal var_part contents */
	        break ;
	    }
	    if ( cpack->cp_var_part[idx+1] == type ) break ;
    }
    if ( idx >= lgt )
    {
	    /*
	     *  This is kind of a warning. '*valuep' remains unchanged.
	     */
	    DBG1 (( MF__,"no option '%c' in connect packet \n", type ));
	    return ;
    }
    if ( (cpack->cp_var_part[idx]&0xff) != 6 )
    {
	    /*
	     *  This is kind of a warning. '*valuep' remains unchanged.
	     */
	    DBG1 (( MF__,"ill int4 arg lgt %d \n", cpack->cp_var_part[idx]&0xff ));
	    return ;
    }
    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x %02x %02x %02x \n",
	    idx , cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ,
	    cpack->cp_var_part[idx+2]&0xff , cpack->cp_var_part[idx+3]&0xff ,
	    cpack->cp_var_part[idx+4]&0xff , cpack->cp_var_part[idx+5]&0xff ));

    /*
     *  Copy the value from the packet.
     */
    SAPDB_memcpy ( valuep , cpack->cp_var_part + idx + 2 , 4 );
}

/* ==================================================================== */

void sql42_put_long (
rte_header *header ,
char        type ,
long        value )
{
#undef	MF__
#define	MF__	"sql42_put_long"
    int				idx ;
    int				lgt ;
    rte_connect_packet	*cpack ;
    ten_com_long		clvalue;

    /*
     *  Put an long int into the var_part of a connect packet.
     *  CAUTION: This is for local use only !!!
     *           Machines may have different sized longs !!!
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the first free byte of the var_part.
     */
    idx = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;

    /*
     *  Set the option entry.
     */
    clvalue = (ten_com_long)value;
    lgt = sizeof(ten_com_long);
    cpack->cp_var_part[idx++] = 2 + lgt ; /* length, argument id, value */
    cpack->cp_var_part[idx++] = type ;
    SAPDB_memcpy ( cpack->cp_var_part + idx , &clvalue , lgt );

    /*
     *  Extend the packet length.
     */
    lgt += 2 ; /* length byte, argument id */
    cpack->cp_connect_length += lgt ;
    header->rh_act_send_len  += lgt ;
    header->rh_max_send_len  += lgt ;
    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x %02x %02x %02x \n",
	    idx-2 , cpack->cp_var_part[idx-2]&0xff , cpack->cp_var_part[idx-1] ,
	    cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1]&0xff ,
	    cpack->cp_var_part[idx+2]&0xff , cpack->cp_var_part[idx+3]&0xff ));
}

/* ==================================================================== */

void sql42_get_long (
rte_header *header ,
char        type ,
long       *valuep )
{
#undef	MF__
#define	MF__	"sql42_get_long"
    int				idx ;
    int				lgt ;
    rte_connect_packet		*cpack ;
    ten_com_long	clvalue;
    long 			lvalue;

    /*
     *  Get an long int from the var_part of a connect packet.
     *  CAUTION: This is for local use only !!!
     *           Machines may have different sized longs !!!
     *
     *  If the packet has no option of requested type, '*valuep' is unchanged.
     */
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Find the argument in the var_part.
     *  'lgt' holds the total length of the var_part.
     */
    lgt = cpack->cp_connect_length + RTE_HEADER_SIZE - RTE_CONPKT_MINSIZ ;
    DBG1 (( MF__,"var_part length %d \n", lgt ));
    for ( idx = 0 ; idx < lgt ; idx += cpack->cp_var_part[idx]&0xff )
    {
	    DBG1 (( MF__,"off %d, lgt %d, id '%c' \n", idx ,
		    cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ));
	    if ( (cpack->cp_var_part[idx]&0xff) < 2 )
	    {
	        idx = lgt ; /* illegal var_part contents */
	        break ;
	    }
	    if ( cpack->cp_var_part[idx+1] == type ) break ;
    }
    if ( idx >= lgt )
    {
	    /*
	     *  This is kind of a warning. '*valuep' remains unchanged.
	     */
	    DBG1 (( MF__,"no option '%c' in connect packet \n", type ));
	    return ;
    }
    if ( (cpack->cp_var_part[idx]&0xff) != 2 + sizeof(ten_com_long) )
    {
	    /*
	     *  This is kind of a warning. '*valuep' remains unchanged.
	     */
	    DBG1 (( MF__,": ill long arg lgt %d \n", cpack->cp_var_part[idx]&0xff ));
	    return ;
    }
    DBG1 (( MF__,"off %d, lgt %d, id '%c', %02x %02x %02x %02x \n",
	    idx , cpack->cp_var_part[idx]&0xff , cpack->cp_var_part[idx+1] ,
	    cpack->cp_var_part[idx+2]&0xff , cpack->cp_var_part[idx+3]&0xff ,
	    cpack->cp_var_part[idx+4]&0xff , cpack->cp_var_part[idx+5]&0xff ));

    /*
     *  Copy the value from the packet.
     */
    SAPDB_memcpy ( &clvalue , cpack->cp_var_part + idx + 2 , sizeof(ten_com_long) );
    lvalue = (long) clvalue;	
    SAPDB_memcpy ( valuep , &lvalue, sizeof(*valuep) );
}

/*
 * ===========================================================================
 */

void sql42_create_conpkt (
rte_header *header ,
int			messclass ,
tsp00_Int4    myref ,
tsp00_Int4    peerref ,
int			retcode ,
int			service ,
long        maxsegsiz ,
long        maxdata ,
long        pktsize ,
long        minreply ,
char       *mydb ,
char       *peerdb )
{
#undef	MF__
#define	MF__	"sql42_crepkt"
    int				idx ;
    union   map_i4_c4
    {
        tsp00_Int4                      i4 ;
        tsp00_C4                        c4 ;
    } map;

    rte_connect_packet		*cpack ;


    DBGIN;
    cpack = (rte_connect_packet*) (header + 1);

    /*
     *  Create connect packet.
     */
    sql42CreatePacketHeader (header, messclass, myref, peerref, RTE_CONPKT_MINSIZ, retcode);

    idx = ' ' ;
    if ( idx == 32 )
            cpack->cp_mess_code[0] = 0 ;
    else    cpack->cp_mess_code[0] = 1 ;

    map.i4 = 65536 ;
    for ( idx = 0 ; idx < 4 ; idx ++ ) if ( map.c4[idx] == 1 ) break ;
    cpack->cp_mess_code[1] = (char) idx ;

    cpack->cp_connect_length   = RTE_CONPKT_MINSIZ - RTE_HEADER_SIZE ;
    cpack->cp_service_type     = (tsp00_Uint1) service ;
    cpack->cp_os_type          = RSQL_UNIX_OS_EO003 ;
    cpack->cp_filler1          = 0 ;
    cpack->cp_filler2          = 0 ;
    cpack->cp_max_segment_size = (tsp00_Int4) maxsegsiz ;
    cpack->cp_max_data_size    = (tsp00_Int4) maxdata ;
    cpack->cp_packet_size      = (tsp00_Int4) pktsize ;
    cpack->cp_min_reply_size   = (tsp00_Int4) minreply ;
    eo46CtoP ( cpack->cp_server_db , peerdb , sizeof(cpack->cp_server_db) );
    eo46CtoP ( cpack->cp_client_db , mydb   , sizeof(cpack->cp_client_db) );

    DBG1 (( MF__,"act_send_len      %d \n",   header->rh_act_send_len ));
    DBG1 (( MF__,"max_send_len      %d \n",   header->rh_max_send_len ));
    DBG1 (( MF__,"protocol_id       %d \n",   header->rh_protocol_id ));
    DBG1 (( MF__,"mess_class        %d \n",   header->rh_mess_class ));
    DBG1 (( MF__,"rte_flags         %d \n",   header->rh_rte_flags ));
    DBG1 (( MF__,"residual_packets  %d \n",   header->rh_residual_packets ));
    DBG1 (( MF__,"sender_ref        %d \n",   header->rh_sender_ref ));
    DBG1 (( MF__,"receiver_ref      %d \n",   header->rh_receiver_ref ));
    DBG1 (( MF__,"rte_return_code   %d \n",   header->rh_rte_return_code ));
    DBG1 (( MF__,"rh_new_swap_type  %d \n",   header->rh_new_swap_type ));
    DBG1 (( MF__,"filler1           %d \n",   header->rh_filler1 ));

    DBG1 (( MF__,"message code      %d \n",     cpack->cp_mess_code[0] ));
    DBG1 (( MF__,"message swap      %d \n",     cpack->cp_mess_code[1] ));
    DBG1 (( MF__,"connect_lgt       %d \n",     cpack->cp_connect_length ));
    DBG1 (( MF__,"service_type      %d \n",     cpack->cp_service_type ));
    DBG1 (( MF__,"os_type           %d \n",     cpack->cp_os_type ));
    DBG1 (( MF__,"max_segment_size  %d \n",     cpack->cp_max_segment_size ));
    DBG1 (( MF__,"max_data_size     %d \n",     cpack->cp_max_data_size ));
    DBG1 (( MF__,"packet_size       %d \n",     cpack->cp_packet_size ));
    DBG1 (( MF__,"min_reply_size    %d \n",     cpack->cp_min_reply_size ));
    DBG1 (( MF__,"server_db         '%.8s' \n", cpack->cp_server_db ));
    DBG1 (( MF__,"client_db         '%.8s' \n", cpack->cp_client_db ));
}

/*
 * ===========================================================================
 */

void sql42_extract_conpkt (
rte_header  *header ,
int			*messclass ,
tsp00_Int4    *myref ,
tsp00_Int4    *peerref ,
int			*retcode ,
int			*service ,
long        *maxsegsiz ,
long        *maxdata ,
long        *pktsize ,
long        *minreply ,
int			*swap_type ,
char        *mydb ,
char        *peerdb )
{
#undef	MF__
#define	MF__	"sql42_extpkt"
    int				idx ;
    union   map_i4_c4
    {
        tsp00_Int4                      i4 ;
        tsp00_C4                        c4 ;
    } map;

    rte_connect_packet		*cpack ;


    DBGIN;
    cpack = (rte_connect_packet*) (header + 1);

    *swap_type  = cpack->cp_mess_code[1];

    sql42_ExtractRTEHeader (*swap_type, header);

    /*
     *  After swapping, mess_code should reflect the new swap type.
     */
    idx = ' ' ;
    if ( idx == 32 )
            cpack->cp_mess_code[0] = 0 ;
    else    cpack->cp_mess_code[0] = 1 ;

    map.i4 = 65536 ;
    for ( idx = 0 ; idx < 4 ; idx ++ ) if ( map.c4[idx] == 1 ) break ;
    cpack->cp_mess_code[1] = (char) idx ;

    sql42_unpack_int2 (  *swap_type , cpack->cp_connect_length ,
                                     &cpack->cp_connect_length );
    sql42_unpack_int4 (  *swap_type , cpack->cp_max_segment_size ,
                                     &cpack->cp_max_segment_size );
    sql42_unpack_int4 (  *swap_type , cpack->cp_max_data_size ,
                                     &cpack->cp_max_data_size );
    sql42_unpack_int4 (  *swap_type , cpack->cp_packet_size ,
                                     &cpack->cp_packet_size );
    sql42_unpack_int4 (  *swap_type , cpack->cp_min_reply_size ,
                                     &cpack->cp_min_reply_size );

    *messclass  = header->rh_mess_class ;
    *myref      = header->rh_receiver_ref ;
    *peerref    = header->rh_sender_ref ;
    *retcode    = header->rh_rte_return_code ;
    *service    = cpack->cp_service_type ;
    *maxsegsiz  = (long) cpack->cp_max_segment_size ;
    *maxdata    = (long) cpack->cp_max_data_size ;
    *pktsize    = (long) cpack->cp_packet_size ;
    *minreply   = (long) cpack->cp_min_reply_size ;

    eo46PtoC ( mydb   , cpack->cp_server_db , sizeof(cpack->cp_server_db) );
    eo46PtoC ( peerdb , cpack->cp_client_db , sizeof(cpack->cp_client_db) );

    DBG1 (( MF__,"act_send_len      %d \n",   header->rh_act_send_len ));
    DBG1 (( MF__,"max_send_len      %d \n",   header->rh_max_send_len ));
    DBG1 (( MF__,"protocol_id       %d \n",   header->rh_protocol_id ));
    DBG1 (( MF__,"mess_class        %d \n",   header->rh_mess_class ));
    DBG1 (( MF__,"rte_flags         %d \n",   header->rh_rte_flags ));
    DBG1 (( MF__,"residual_packets  %d \n",   header->rh_residual_packets ));
    DBG1 (( MF__,"sender_ref        %d \n",   header->rh_sender_ref ));
    DBG1 (( MF__,"receiver_ref      %d \n",   header->rh_receiver_ref ));
    DBG1 (( MF__,"rte_return_code   %d \n",   header->rh_rte_return_code ));
    DBG1 (( MF__,"rh_new_swap_type  %d \n",   header->rh_new_swap_type ));
    DBG1 (( MF__,"filler1           %d \n",   header->rh_filler1 ));

    DBG1 (( MF__,"message code      %d \n",     cpack->cp_mess_code[0] ));
    DBG1 (( MF__,"message swap      %d \n",     cpack->cp_mess_code[1] ));
    DBG1 (( MF__,"connect_lgt       %d \n",     cpack->cp_connect_length ));
    DBG1 (( MF__,"service_type      %d \n",     cpack->cp_service_type ));
    DBG1 (( MF__,"os_type           %d \n",     cpack->cp_os_type ));
    DBG1 (( MF__,"max_segment_size  %d \n",     cpack->cp_max_segment_size ));
    DBG1 (( MF__,"max_data_size     %d \n",     cpack->cp_max_data_size ));
    DBG1 (( MF__,"packet_size       %d \n",     cpack->cp_packet_size ));
    DBG1 (( MF__,"min_reply_size    %d \n",     cpack->cp_min_reply_size ));
    DBG1 (( MF__,"server_db         '%.8s' \n", cpack->cp_server_db ));
    DBG1 (( MF__,"client_db         '%.8s' \n", cpack->cp_client_db ));
}
/*
 * ===========================================================================
 */

static void sql42_ExtractRTEHeader  (int                     swapType,
                                     rte_header *            pHeader)
{
#undef	MF__
#define	MF__	"sql42_ExtractRTEHeader"

    DBGIN;

    sql42_unpack_int4 (swapType, pHeader->rh_act_send_len ,
                                 &pHeader->rh_act_send_len );
    sql42_unpack_int4 (swapType, pHeader->rh_max_send_len ,
                                 &pHeader->rh_max_send_len );
    sql42_unpack_int4 (swapType, pHeader->rh_sender_ref ,
                                 &pHeader->rh_sender_ref );
    sql42_unpack_int4 (swapType, pHeader->rh_receiver_ref ,
                                 &pHeader->rh_receiver_ref );
    sql42_unpack_int2 (swapType, pHeader->rh_rte_return_code ,
                                 &pHeader->rh_rte_return_code );
}

/*
 * ===========================================================================
 */

static SAPDB_UInt1 sql42_OldSwapType ()
{
  SAPDB_UInt1    i;

  union map_check_i4_b4         MapCheck;

  DBGIN;

  MapCheck.int4 = 65536;
  for ( i = 0; (i < 4) && ( MapCheck.c4[ i ] != 1 ); i++ ) { ; }

  return i;
}

/*
 * ===========================================================================
 */

int sql42_SendCertificatePacket  (int               sendHandle,
                                  int			    messclass,
                                  tsp00_Int4        myref,
                                  tsp00_Int4        peerref,
                                  int			    retcode,
                                  SAPDB_Byte *      pServerCertificate,
                                  SAPDB_UInt4       lenCertificate,
                                  tsp00_ErrTextc    pErrText)
{
#undef	MF__
#define	MF__	"sql42_SendCertificatePacket"

    tsp00_Uint                      commState;
    SAPDB_Char *                    pPacket;
    rte_header *                    pRTEHeader;
    RTESec_CertificateReplyPacket * pCertPacket;
    SAPDB_UInt4                     packetSize;
    int                             rc = SP1CE_OK;


    packetSize = sizeof (*pRTEHeader) + sizeof (RTESec_CertificateReplyPacket) - 1 + lenCertificate;
    pPacket = (SAPDB_Char *) alloca (packetSize);  
    memset (pPacket, 0, packetSize);
    pRTEHeader = (rte_header *)pPacket;
    pCertPacket    = (RTESec_CertificateReplyPacket *)(pPacket + sizeof (*pRTEHeader));

    sql42CreatePacketHeader (pRTEHeader, messclass, myref, peerref, sizeof (*pRTEHeader), retcode);

    pCertPacket->swapType = sql42_OldSwapType ();
 
    if (0 != lenCertificate)
    {
        memcpy ((SAPDB_Char *)(pCertPacket->pCertificate), (SAPDB_Char *)pServerCertificate, lenCertificate);
    }

    pRTEHeader->rh_act_send_len     += lenCertificate;
    pRTEHeader->rh_max_send_len     += lenCertificate;


    rc = sql42_send_conpkt (sendHandle, pRTEHeader, pErrText );

    return rc;
}

/*
 * ===========================================================================
 */

/*
 This routine is used to write to a scoket or a pipe. In kernel it is used by requestor
 thread to send the connection reply packet to the named pipe of the client
 */
tsp00_Longuint
en42SocketSendPacket ( void            *SendHandle,
                       char            *pData,
                       int              Len,
                       tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"en42SocketSendPacket"
    int				rc ;

    int sd = * (int *) SendHandle ;
    DBG1 (( MF__,"SendHandle %d lgt %d \n", sd , Len ));

#ifdef DEBUG_DEBUG
	{
	char hexbuffer[64];
    DBG1 (( MF__,"   %s \n", sql46hexstr(pData,min(16,Len),hexbuffer) ));
    DBG1 (( MF__,"   %s \n", sql46hexstr(pData+16,min(16,Len-16),hexbuffer) ));
	}
#endif /*DEBUG_DEBUG*/

    for ( ; Len > 0 ; pData += rc , Len -= rc )
    {
        DBG1 (( MF__,"send sd %d p 0x%p Len %d \n", sd , pData , Len ));
        rc = RTE_save_write ( sd , pData , Len );
        DBG1 (( MF__,"send rc %d \n", rc ));
        if ( rc == UNDEF )
        {
	        if ( errno == ECONNRESET )
            {
		        en42FillErrText(pErrText, "connection closed (send:ECONNRESET)") ;
		        rc = SP1CE_RELEASED ;
            }
            else if (errno == EPIPE )
	        {
		        en42FillErrText(pErrText, "connection closed (send:EPIPE)") ;
		        rc = SP1CE_RELEASED ;
	        }
            else
            {
		        en42FillErrText(pErrText, "send error:%s", sqlerrs()) ;
    	        rc = SP1CE_NOTOK ;
            }
            DBG1 (( MF__,": %s \n",pErrText ));
            return rc;
        }
    }

    DBG1 (( MF__,"returning %d \n", SP1CE_OK ));
    return ( SP1CE_OK );
}
/*
 * ===========================================================================
 */

int sql42_send_conpkt (
int                  sd ,
rte_header			*header ,
tsp00_ErrTextc VAR_ARRAY_REF pErrText )
{
#undef	MF__
#define	MF__	"sql42_send_conpkt"
    return en42SocketSendPacket( &sd, (char *)header, (int)(header->rh_act_send_len), pErrText);
}

/*
 * ===========================================================================
 */

int sql42_recv_conpkt (
int            sd ,
rte_header    *header ,
int           *statep ,
tsp00_ErrTextc pErrText )
{
#undef	MF__
#define	MF__	"sql42_recv_con"
    int                 rc ;
    int                 lgt ;
    int                 len ;
    int                 swap_type ;
    tsp00_Int4            len4 ;
    rte_connect_packet *cpack ;
    char               *ptr ;

    DBGIN;
    cpack = (rte_connect_packet*) (header + 1);
    ptr = (char*) header ;
    /*
     *  In the fist place, try to receive the minimal packet only.
     *  This is to avoid reading parts of packets that follow
     *  immediately after the connect packet.
     *  After the minimal part of the connect packet is received,
     *  we can determine the exact packet length.
     */
    lgt = RTE_CONPKT_MINSIZ ;
    len = 0 ;
    len4 = 0 ;

    /*
     *  Receive connect packet.
     */
    for ( ; lgt > 0 ; )
    {
	    /*
	     *  Use 'read' here, because this function is also used
	     *  to read from a named pipe.
	     */
	    DBG1 (( MF__,"read sd %d size %d \n", sd , lgt ));
        rc = read ( sd , ptr , lgt );
	    DBG1 (( MF__,"read sd %d rc %d \n", sd , rc ));
        if ( rc == UNDEF )
        {
            if ( errno == EINTR )
            {
		        if ( statep && (*statep == CON_TIMEDOUT) )
		        {
		            en42FillErrText(pErrText, "connect timed out (read interupted)" );
		            DBG1 (( MF__,"%s \n", pErrText ));
		            return ( SP1CE_NOTOK );
		        }
                DBG1 (( MF__,"interrupted, continuing \n" ));
                rc = 0 ;
                continue ;
            }

	        if ( errno == ECONNRESET )
            {
		        en42FillErrText(pErrText, "connection closed (read:ECONNRESET)") ;
		        rc = SP1CE_RELEASED ;
            }
            else if (errno == EPIPE )
	        {
		        en42FillErrText(pErrText, "connection closed (read:EPIPE)") ;
		        rc = SP1CE_RELEASED ;
	        }
    	    else
	        {
		        en42FillErrText(pErrText, "read error:%s", sqlerrs()) ;
        		rc = SP1CE_NOTOK ;
            }
		    DBG1 (( MF__,"%s \n", pErrText ));
            return ( rc );
	    }

	    if ( rc == 0 )
	    {
		    en42FillErrText(pErrText, "connection closed (read:EOF)") ;
		    DBG1 (( MF__,"%s \n", pErrText ));
	        return ( SP1CE_RELEASED );
	    }

        DBG1 (( MF__,"received %d bytes \n", rc ));
	    ptr += rc ;
	    lgt -= rc ;
	    len += rc ;
	    if ( ! len4 && (len >= RTE_CONPKT_MINSIZ) )
	    {
	        swap_type = cpack->cp_mess_code[1];
            sql42_unpack_int4 ( swap_type , header->rh_act_send_len , &len4 );
	        if ( (len4 < RTE_CONPKT_MINSIZ) || (len4 > RTE_CONPKT_SIZE) )
	        {
#if defined(SAPDB_DEBUG_BAD_REPLY_PACKET) /* normally not defined !!! */
                {
                    char outputLine[1024];
                    int fd = open("/SAP_DB/data/wrk/remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
                    if ( fd < 0 )
                    {
                        fd = open("remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
                    }
                    if ( fd >= 0 )
                    {
                        time_t now = time(0);
                        ctime_r(&now, outputLine);
                        sp77sprintf(outputLine + strlen(outputLine), sizeof(outputLine) - strlen(outputLine),
                            "recv: PID %8ld Bad Connect Reply Packet dump (%d bytes, act_send_len %d len4 %d): %*.*r\n",
                            getpid(), len, header->rh_act_send_len, len4, len, len, header );
                        write(fd, outputLine, strlen(outputLine));
                        close(fd);
                    }
                }
#endif
                en42FillErrText(pErrText, "bad connect reply:len %d", len4) ;
    		    DBG1 (( MF__,"%s \n", pErrText ));
		        return ( SP1CE_NOTOK );
	        }
	        lgt = len4 - len ;
	    }
    }

    DBG1 (( MF__,"returning %d \n", SP1CE_OK ));
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */

static void sql42CreatePacketHeader     (rte_header *   header ,
                                         int			messclass ,
                                         tsp00_Int4     myref ,
                                         tsp00_Int4     peerref ,
                                         SAPDB_UInt4    sendLength,
                                         int			retcode)
{
#undef	MF__
#define	MF__	"sql42_crepkt"

    DBGIN;

    header->rh_act_send_len     = sendLength; /*RTE_CONPKT_MINSIZ*/
    header->rh_max_send_len     = sendLength;/*RTE_CONPKT_MINSIZ*/
    header->rh_protocol_id      = RSQL_RTE_PROT_TCP_EO003 ;
    header->rh_mess_class       = (tsp00_Uint1) messclass ;
    header->rh_rte_flags        = RSQL_NORMAL_EO003 ;
    header->rh_residual_packets = 0 ;
    header->rh_sender_ref       = myref ;
    header->rh_receiver_ref     = peerref ;
    header->rh_rte_return_code  = (tsp00_Int2) retcode ;
    header->rh_new_swap_type    = sql42_new_swap_type();
    header->rh_filler1          = 0 ;
}

#ifdef  SOCKETS
/*
 * ===========================================================================
 */

void sql42_send_control (
int      sd ,
int      messclass ,
tsp00_Int4 myref ,
tsp00_Int4 peerref ,
int      retcod )
{
#undef	MF__
#define	MF__ "sql42_send_ctl"
    rte_header *header ;
    char        packet [ RTE_HEADER_SIZE ];

    DBGIN;
    header = (rte_header*) packet ;

    /*
     *  Create control packet.
     */
    header->rh_act_send_len     = RTE_HEADER_SIZE ;
    header->rh_max_send_len     = RTE_HEADER_SIZE ;
    header->rh_protocol_id      = RSQL_RTE_PROT_TCP_EO003 ;
    header->rh_mess_class       = (tsp00_Uint1) messclass ;
    header->rh_rte_flags        = RSQL_NORMAL_EO003 ;
    header->rh_residual_packets = 0 ;
    header->rh_sender_ref       = myref ;
    header->rh_receiver_ref     = peerref ;
    header->rh_rte_return_code  = (tsp00_Int2) retcod ;
    header->rh_new_swap_type    = sql42_new_swap_type();
    header->rh_filler1          = 0 ;

    /*
     *  Send control packet.
     */
    RTE_save_write ( sd , packet , RTE_HEADER_SIZE );

    DBGOUT;
}

/*
 * ===========================================================================
 */

int sql42_send_packet (
int			sd ,
rte_header		*header ,
long			maxsegsiz ,
tsp00_ErrTextc  pErrText )
{
#undef	MF__
#define	MF__	"sql42_send_pkt"
    int				rc ;
    int				lgt ;
    int				remain ;
    rte_header      *hdr ;
    char			*src ;
    char			savbuf [ RTE_HEADER_SIZE ];

    /*
     *  sql42_send_packet splits packets into maxsegsiz packets.
     *  It uses sql42_send_conpkt to send the smaller packets.
     */

    DBG1 (( MF__,"sd %d maxseg %d lgt %d \n", sd , maxsegsiz , header->rh_max_send_len ));

    if ( maxsegsiz <= RTE_HEADER_SIZE )
    {
        DBG1 (( MF__,": illegal maxsegsiz %ld \n", maxsegsiz ));
        en42FillErrText( pErrText,
                         "protocol error (max segment size %ld)",
                         maxsegsiz) ;
        return ( SP1CE_NOTOK );
    }

    if ( header->rh_max_send_len < RTE_HEADER_SIZE )
    {
        DBG1 (( MF__,": illegal packet size %d \n", header->rh_max_send_len ));
        en42FillErrText( pErrText,
                         "protocol error (max send len %d)",
                         header->rh_max_send_len) ;
        return ( SP1CE_NOTOK );
    }

    /*
     *  Update RTE header.
     */
    header->rh_protocol_id      = RSQL_RTE_PROT_TCP_EO003 ;
    header->rh_new_swap_type    = sql42_new_swap_type();
    header->rh_filler1          = 0 ;

    if ( header->rh_max_send_len <= maxsegsiz + RTE_HEADER_SIZE_EO003 )
    {
        header->rh_act_send_len = header->rh_max_send_len ;
        header->rh_residual_packets = 0 ;
    }
    else
    {
	    header->rh_act_send_len = (tsp00_Int4) maxsegsiz ;
	    /*
	     *  The total length of data to be carried (excluding header) is
	     *      tot_data_len = (header->rh_max_send_len - RTE_HEADER_SIZE).
	     *
	     *  The size of data that each segment can carry is
	     *      seg_data_siz = (maxsegsiz - RTE_HEADER_SIZE).
	     *
	     *  The number of segments required is
	     *      ((tot_data_len + seg_data_siz - 1) / seg_data_siz).
	     *
	     *  The number of residual segments is one less:
	     *      ((tot_data_len - seg_data_siz + seg_data_siz - 1)
	     *                                             / seg_data_siz).
	     *  better:
	     *      ((tot_data_len - 1) / seg_data_siz).
	     */
        header->rh_residual_packets =
	    ( header->rh_max_send_len - RTE_HEADER_SIZE - 1 )
                             / ( maxsegsiz - RTE_HEADER_SIZE );
        DBG1 (( MF__,"residual packets %d \n", header->rh_residual_packets ));
    }

    rc = sql42_send_conpkt ( sd , header , pErrText );
    if ( rc != SP1CE_OK )
    {
    	return ( rc );
    }

    if ( header->rh_max_send_len > maxsegsiz + RTE_HEADER_SIZE_EO003 )
    {
	    src = (char*) header + header->rh_act_send_len - RTE_HEADER_SIZE ;
	    remain = header->rh_max_send_len - header->rh_act_send_len ;

	    while( remain > 0 )
        {
			SAPDB_memcpy ( savbuf , src , RTE_HEADER_SIZE );
            header->rh_residual_packets -- ;
    	    SAPDB_memcpy ( src , header , RTE_HEADER_SIZE );
            lgt = min ( maxsegsiz - RTE_HEADER_SIZE , remain );
	        hdr = (rte_header*) src ;
            hdr->rh_act_send_len = lgt + RTE_HEADER_SIZE ;
            DBG1 (( MF__,"remain %d lgt %d respkt %d \n",
		    remain , lgt , hdr->rh_residual_packets ));

            rc = sql42_send_conpkt ( sd , hdr , pErrText );
	        SAPDB_memcpy ( src , savbuf , RTE_HEADER_SIZE );
            if ( rc != SP1CE_OK )
	        {
    		    break ;
	        }
	        src += lgt ;
	        remain -= lgt ;
        }
    }

    DBG1 (( MF__,"returning %d \n", rc ));
    return ( rc ) ;
}

/*
 * ===========================================================================
 */

int sql42_recv_packet (
int			     sd ,
rte_header		*header ,
int			     swap_type ,
long			 siz ,
tsp00_ErrTextc	 pErrText ,
char			*cache_buf ,
long			*cache_lgt )
{
#undef	MF__
#define	MF__	"sql42_recv_pkt"
    int				rc ;
    int				segcnt ;
    int				lgt ;
    long			remain ;
    rte_header		*hdr ;
    char			*dst ;
    char			savbuf [ RTE_HEADER_SIZE ];

    /*
     *  sql42_recv_packet combines split packets.
     *  It uses sql42_rcvpkt to receive the smaller packets,
     *  and reconstructs the original packet.
     */

    DBG1 (( MF__,"sd %d siz %d clgt %ld \n", sd , siz , *cache_lgt ));

    if ( siz <= RTE_HEADER_SIZE )
    {
        DBG1 (( MF__,": illegal buffer size %ld \n", siz ));
    	en42FillErrText(pErrText, "protocol error: buffer size %ld", siz );
        return ( SP1CE_NOTOK );
    }

    rc = sql42_rcvpkt ( sd , header , swap_type , siz , pErrText ,
					    cache_buf , cache_lgt );
    if ( rc != SP1CE_OK )
    {
    	return ( rc );
    }

    /*
     *  Should never happen, since sql42_rcvpkt returns SP1CE_OK only if
     *  this condition is false.
     */
    if ( header->rh_max_send_len < RTE_HEADER_SIZE )
    {
    	en42FillErrText(pErrText, "received a garbled packet:len %d", header->rh_max_send_len  );
		DBG1 (( MF__,"%s \n", pErrText ));
        return ( SP1CE_NOTOK );
    }
    DBG1 (( MF__," total lgt %d respkt %d \n",
	    header->rh_max_send_len , header->rh_residual_packets ));

    if ( header->rh_max_send_len > header->rh_act_send_len )
    {
        dst = (char*) header + header->rh_act_send_len - RTE_HEADER_SIZE ;
	    siz -= header->rh_act_send_len - RTE_HEADER_SIZE ;
	    hdr = header ; /* some compilers require this to be initialized */
        remain = header->rh_max_send_len - header->rh_act_send_len ;
    	segcnt = 1 ;

	    hdr = header ; /* if remain == 0 */
        while ( remain > 0 )
        {
	        segcnt ++ ;
	        SAPDB_memcpy ( savbuf , dst , RTE_HEADER_SIZE );
	        hdr = (rte_header*) dst ;

            rc = sql42_rcvpkt ( sd ,
                                hdr ,
                                swap_type ,
                                siz ,
                                pErrText ,
					            cache_buf ,
                                cache_lgt );
            if ( rc != SP1CE_OK )
	        {
    		    return ( rc );
	        }
            lgt = hdr->rh_act_send_len - RTE_HEADER_SIZE ;
    	    if ( hdr->rh_max_send_len != header->rh_max_send_len ) break ;
	        remain -= lgt ;
	        if ( 0 == remain && hdr->rh_residual_packets )
            {    	    
                DBG1 (( MF__,": seg %d remain %ld respkt %d \n",
	    	        segcnt , remain , hdr->rh_residual_packets ));
                en42FillErrText(pErrText, "received a garbled packet:residuals %d", hdr->rh_residual_packets);
	            rc = SP1CE_NOTOK ;
            }

    	    SAPDB_memcpy ( dst , savbuf , RTE_HEADER_SIZE );
	        dst    += lgt ;
	        siz    -= lgt ;
        }

        if ( remain )
        {
            en42FillErrText(pErrText, "received a garbled packet:remain %d", remain);
	        rc = SP1CE_NOTOK ;
        }

        header->rh_act_send_len = header->rh_max_send_len ;
    }

    DBG1 (( MF__,"returning %d \n", rc ));
    return ( rc );
}

/*
 * ===========================================================================
 */
tsp00_Longuint
en42Receive ( void                         *RecvHandle,
              char                         *pData,
              tsp00_Longuint                ulMaxDataLen,
              tsp00_Longuint               *BytesReceived,
              tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
    tsp00_Longuint   CommState = commErrOk_esp01;
    int sd = *((int *)RecvHandle) ;
    int rc ;

    *BytesReceived = 0 ;

    rc = RTE_save_read ( sd , pData , ulMaxDataLen );
    DBG1 (( MF__,"read sd %d rc %d \n", sd , rc ));

    if ( rc > 0 )
    {
        *BytesReceived = rc ;
    }
    else if ( rc == 0 )
    {
		en42FillErrText(pErrText, "connection closed (read:EOF)");
        CommState = commErrReleased_esp01 ;
    }
    else
    {
	    if ( errno == ECONNRESET )
        {
		    en42FillErrText(pErrText, "connection closed (read:ECONNRESET)") ;
            CommState = commErrReleased_esp01 ;
        }
        else if (errno == EPIPE )
	    {
		    en42FillErrText(pErrText, "connection closed (read:EPIPE)") ;
            CommState = commErrReleased_esp01 ;
	    }
        else
        {
		    en42FillErrText(pErrText, "socket receive error:%s", sqlerrs()) ;
            CommState = commErrNotOk_esp01 ;
    	    rc = SP1CE_NOTOK ;
        }
    }
    return ( CommState );
}


/*
 * ===========================================================================
 */

static tsp00_Uint sql42ReceiveCertificatePacket  (void *                    pRecvHandle,
                                                  RECV_PKT_FUNC_PTR_EO003   pRecvFunc,
                                                  SAPDB_Byte *              pServerCertificate,
                                                  SAPDB_UInt4               maxLenCertificate,
                                                  tsp00_ErrTextc            pErrText)
{
#undef	MF__
#define	MF__	"sql42ReceiveCertificatePacket"
  tsp00_Uint                      bytesRec;
  tsp00_Uint                      ulCommState = commErrOk_esp01;
  rte_header *                    pRTEHeader;
  tsp00_Int                       bytesRemaining;
  RTESec_CertificateReplyPacket * pCertPacket;
  char                            ucPacket[sizeof(*pRTEHeader) +
                                           sizeof(*pCertPacket) - 1];

  DBGIN;
  /*
   * - Structure of the remote connect packet:
   *
   *      +------------------------------------+
   *      |     rte_header_record              |
   *      +------------------------------------+
   *      |     RTESec_CertificateReplyPacket  |
   *      |     (has dynamic length)           |
   *      +------------------------------------+
   *
   */
 
  pRTEHeader     = (rte_header *)&ucPacket[0];
  pCertPacket    = (RTESec_CertificateReplyPacket *) &ucPacket[sizeof(*pRTEHeader)];

  ulCommState = pRecvFunc (pRecvHandle, ucPacket + 0, sizeof(ucPacket), &bytesRec, pErrText);

  DBG3 (( MF__, "%d Bytes received", bytesRec ));

  if (bytesRec < sizeof(ucPacket))
  {
      MSGD (( ERR_CONN_SERV_REJECT )) ;
      strcpy(pErrText, ERRMSG_COM_CONN_SERV_REJECT );
      ulCommState = commErrNotOk_esp01 ;
  }
  else
  {
      sql42_ExtractRTEHeader (pCertPacket->swapType, pRTEHeader);
  }

 /*
  *  Check the received information.
  */
  if (pRTEHeader->rh_mess_class != RSQL_CERT_REPLY_EO003 )
  {
      DBG1 (( MF__,": server messClass %d expected %d \n",
          pRTEHeader->rh_mess_class , RSQL_CERT_REPLY_EO003 ))
      en42FillErrText(pErrText, "Cert reply expected");
      ulCommState == commErrNotOk_esp01;
  }

  if (commErrOk_esp01 == ulCommState)
  {
      switch (pRTEHeader->rh_rte_return_code)
      {
      case SP1CE_NOTOK :
          en42FillErrText(pErrText, "protocol error" );
          ulCommState = commErrNotOk_esp01;
          break ;
      case SP1CE_TIMEOUT :
          en42FillErrText(pErrText, "connect timed out" );
          ulCommState = commErrNotOk_esp01;
          break ;
      case SP1CE_RELEASED :
          en42FillErrText(pErrText, "server rejected connection" );
          ulCommState = commErrNotOk_esp01;
          break ;
      case SP1CE_OK:
          ulCommState = commErrOk_esp01;
          break;
      default :
          en42FillErrText(pErrText, "Cert request failed: retcod %d", pRTEHeader->rh_rte_return_code);
          ulCommState = commErrNotOk_esp01;
          break ;
       }
 
      if (commErrOk_esp01 == ulCommState)
      {
          SAPDB_Byte *  pData = pServerCertificate;

          if (maxLenCertificate < pRTEHeader->rh_act_send_len - sizeof(ucPacket))
          {
              MSGD (( ERR_COM_CERT_BUFFER_TOO_SMALL, maxLenCertificate, pRTEHeader->rh_act_send_len - sizeof(ucPacket))) ;
              strcpy( pErrText, ERRMSG_COM_CERT_BUFFER_TOO_SMALL );
              ulCommState = commErrNotOk_esp01 ;
          }

          for( bytesRec = 0, bytesRemaining = pRTEHeader->rh_act_send_len - sizeof(ucPacket);
              bytesRemaining > 0 && ulCommState == commErrOk_esp01 ;
              pData += bytesRec, bytesRemaining -= bytesRec )
          {
              ulCommState = pRecvFunc (pRecvHandle, (char *) pData, bytesRemaining, &bytesRec, pErrText);
 
              DBG3 (( MF__, "%d bytes received, CommState = %d", bytesRec, ulCommState));
              if ( ulCommState == commErrOk_esp01  )
              {
                  if ( bytesRec == 0 )
                  {
                      MSGD (( ERR_CONN_BROKEN ));
                      strcpy( pErrText, ERRMSG_COM_CONN_BROKEN );
                      ulCommState = commErrNotOk_esp01 ;
                  }
              }
          }

          if (commErrOk_esp01 == ulCommState)
          {
              if (commErrOk_esp01 != (ulCommState = eo420UnpackSAPUCString (pCertPacket->swapType, 
                                                                            pServerCertificate, 
                                                                            pServerCertificate, 
                                                                            pRTEHeader->rh_act_send_len - sizeof(ucPacket),
                                                                            pErrText)))
              {
                  MSGD (( ERR_COM_CERT_UNPACK_CERT ));
              }
          }
      }
  }

  DBGOUT;
  return ( ulCommState );
}

/*
 * ===========================================================================
 */

static  int     sql42_rcvpkt (
int             sd ,
rte_header     *header ,
int             swap_type ,
long            siz ,
tsp00_ErrTextc  pErrText ,
char           *cache_buf ,
long           *cache_lgt )
{
#undef	MF__
#define	MF__	"sql42_rcvpkt"
    int				rc ;
    int				lgt ;
    int				len ;
    tsp00_Int4			len4 ;
    char			*ptr ;


    DBG1 (( MF__,"sd %d siz %d clgt %ld \n", sd , siz , *cache_lgt ))

    ptr  = (char*) header ;
    len  = 0 ;
    len4 = 0 ;

    /*
     *  In the case of the kernel to kernel communication (distribution),
     *  and in the case of segmentation (IST 1996-09-16),
     *  it is possible that two or more packets are sent so close in time
     *  that a single recv() call receives more than one packet.
     *  In this case the data received past the first packet is
     *  copied to a cache. The next rcvpkt will first examine
     *  the cache contents and then recv() the rest of the
     *  packet if it is not already complete.
     *  The following is FALSE in case of segmentation (IST 1996-09-16):
     *  For normal REMOTE SQL there in no cache required
     *  since the packets are always sent in a strong ping-pong protocol.
     *  cache_buf is then NULL.
     *  In case of segmentation, the segments are sent without waiting
     *  for any acknowledge. Hence, more than one segment can be received
     *  with a single read.
     */
    if ( cache_buf && *cache_lgt )
    {
	    DBG1 (( MF__,"cache revealed %ld bytes \n", *cache_lgt ))
	    SAPDB_memcpy ( ptr , cache_buf , *cache_lgt );
	    ptr += *cache_lgt ;
	    len = *cache_lgt ;
	    *cache_lgt = 0 ;

        /*
         *  After the rte header is complete,
         *  we know exactly which size to receive.
         */
        if ( len >= RTE_HEADER_SIZE )
        {
            sql42_unpack_int4 ( swap_type , header->rh_act_send_len , &len4 );
	        DBG1 (( MF__,"cached complete header, len4=%d \n", len4 ))

	        if ( (len4 < RTE_HEADER_SIZE) || (len4 > siz) )
	        {
		        en42FillErrText(pErrText, "received a garbled packet" );
		        DBG1 (( MF__,": %s, length %d\n", *errm , len4 ))
		        return ( SP1CE_NOTOK );
	        }
        }
    }

    if ( len4 )
	lgt = len4 - len ;
    else
	lgt = siz - len ;

    for ( ; lgt > 0 ; )
    {
    	DBG1 (( MF__,"recv sd %d p 0x%08lx sz %d \n", sd , (long) ptr , lgt ))
        rc = RTE_save_read ( sd , ptr , lgt );
	    DBG1 (( MF__,"recv rc %d \n", rc ))
        if ( rc == UNDEF )
        {
	        if ( (errno == ECONNRESET) || (errno == EPIPE) )
	        {
		        en42FillErrText(pErrText, "connection closed by counterpart" );
		        DBG1 (( MF__,"%s \n", *errm ))
		        rc = SP1CE_RELEASED ;
	        }
	        else
	        {
		        en42FillErrText(pErrText, "socket recv error (%d:%s)", errno, sqlerrs()) ;
		        DBG1 (( MF__,": %s: %s\n", *errm , sqlerrs() ))
		        rc = SP1CE_NOTOK ;
	        }
            return ( rc );
        }
        if ( rc == 0 )
        {
            if ( len )
            {
		        en42FillErrText(pErrText, "connection broken" );
                DBG1 (( MF__,": %s \n", *errm ))
				rc = SP1CE_NOTOK ;
            }
            else
            {
		        en42FillErrText(pErrText, "connection closed by counterpart" );
                DBG1 (( MF__,": %s \n", *errm ))
				SAPDB_memset ( header , 0 , sizeof(rte_header) );
				rc = SP1CE_RELEASED ;
            }
    	    return ( rc );
        }

	    ptr += rc ;
	    lgt -= rc ;
	    len += rc ;

        /*
         *  After the rte header is complete,
         *  we know exactly which size to receive.
         */
        if ( ! len4 && (len >= RTE_HEADER_SIZE) )
        {
            sql42_unpack_int4 ( swap_type , header->rh_act_send_len , &len4 );
    	    DBG1 (( MF__,"recved complete header, len4=%d \n", len4 ))

	        if ( (len4 < RTE_HEADER_SIZE) || (len4 > siz) )
	        {
		        en42FillErrText(pErrText, "received a garbled packet" );
		        DBG1 (( MF__,": %s, length %d\n", *errm , len4 ))
		        return ( SP1CE_NOTOK );
	        }
    	    lgt = len4 - len ;
        }
    }

    /*
     *  If a receive cache is present and we received more than one packet
     *  then store the remaining data in the cache.
     */
    if ( len > len4 )
    {
	    if ( cache_buf )
	    {
	        SAPDB_memcpy ( cache_buf , ((char*) header + len4) , len - len4 );
	        *cache_lgt = len - len4 ;
	    }
	    else
	    {
	        DBG1 (( MF__,": %d bytes overrun, closing \n", len - len4 ))
	    }
    }

    sql42_unpack_int4 ( swap_type , header->rh_act_send_len ,
                                   &header->rh_act_send_len );
    sql42_unpack_int4 ( swap_type , header->rh_max_send_len ,
                                   &header->rh_max_send_len );
    sql42_unpack_int4 ( swap_type , header->rh_sender_ref ,
                                   &header->rh_sender_ref );
    sql42_unpack_int4 ( swap_type , header->rh_receiver_ref ,
                                   &header->rh_receiver_ref );
    sql42_unpack_int2 ( swap_type , header->rh_rte_return_code ,
                                   &header->rh_rte_return_code );

#ifdef  DEBUG_DEBUG
    ptr = (char*) header ;
    lgt = header->rh_act_send_len ;
    DBG1 (( MF__,"packet length     %d \n", lgt ))
	{
	    char hexbuffer[64];
        DBG1 (( MF__,"   %s \n", sql46hexstr(ptr,min(16,lgt),hexbuffer) ))
        DBG1 (( MF__,"   %s \n", sql46hexstr(ptr+16,min(16,lgt-16),hexbuffer) ))
	}
#endif  /*DEBUG_DEBUG*/

    if ( header->rh_rte_return_code != SP1CE_OK )
    {
	    switch ( header->rh_rte_return_code )
	    {
	    case SP1CE_NOTOK :
		    en42FillErrText(pErrText, "protocol error" );
	        break ;
	    case SP1CE_TASKLIMIT :
		    en42FillErrText(pErrText, "task limit" );
	        break ;
	    case SP1CE_TIMEOUT :
		    en42FillErrText(pErrText, "command timeout" );
	        break ;
	    case SP1CE_SHUTDOWN :
		    en42FillErrText(pErrText, "database shutdown" );
	        break ;
	    case SP1CE_CRASH :
		    en42FillErrText(pErrText, "connection aborted" );
	        break ;
	    case SP1CE_RELEASED :
		    en42FillErrText(pErrText, "connection broken" );
	        break ;
	    default :
		    en42FillErrText(pErrText, "database is down" );
	        break ;
	    }
	    DBG1 (( MF__,"rte_return_code: %s \n", *errm ))
    }

    DBG1 (( MF__,"returning %d \n", header->rh_rte_return_code ))
    return ( header->rh_rte_return_code );
}

#endif  /*SOCKETS*/

/*
 * ===========================================================================
 */

void sql42_unpack_int2 (
int                     typ ,
tsp00_Int2                src ,
tsp00_Int2                *dst )
{
#undef	MF__
#define	MF__	"sql42_unpack_int2"
    unsigned char		*ptr ;


    ptr = (unsigned char *) &src ;

    switch ( typ )
    {
    case 1:
            /* no swap: hi-lo */
            *dst = (ptr[0] << 8) |
                   (ptr[1] << 0) ;
            break ;
    case 2:
            /* full swap: lo-hi */
	    /*FALLTHROUGH*/
    case 3:
            /* half swap: lo-hi */
            *dst = (ptr[0] << 0) |
                   (ptr[1] << 8) ;
            break ;
    default:
            DBG1 (( MF__,": illegal swap type %d \n", typ ))
            *dst = 0 ;
    }
}

/*
 * =========================================================================
 */


void sql42_unpack_int4 (
char			typ ,
tsp00_Int4		src ,
tsp00_Int4		*dst )
{
#undef	MF__
#define	MF__	"sql42_unpack_int4"
    unsigned char		*ptr ;


    ptr = (unsigned char *) &src ;

    switch ( typ )
    {
    case 1:
	    /* no swap: hihi-hilo-lohi-lolo */
	    *dst = (ptr[0] << 24) |
		   (ptr[1] << 16) |
		   (ptr[2] <<  8) |
		   (ptr[3] <<  0) ;
	    break ;
    case 2:
	    /* full swap: lolo-lohi-hilo-hihi */
	    *dst = (ptr[0] <<  0) |
		   (ptr[1] <<  8) |
		   (ptr[2] << 16) |
		   (ptr[3] << 24) ;
	    break ;
    case 3:
	    /* half swap: lohi-lolo-hihi-hilo */
	    *dst = (ptr[0] <<  8) |
		   (ptr[1] <<  0) |
		   (ptr[2] << 24) |
		   (ptr[3] << 16) ;
	    break ;
    default:
	    DBG1 (( MF__,": illegal swap type %d \n", typ ))
	    *dst = 0 ;
    }
}

/*
 * ===========================================================================
 */
#ifdef SOCKETS
#undef	MF__
#define	MF__	MOD__"sql42_GetServicePort"
tsp01_CommErr_Enum sql42_GetServicePort (char *          pszServerNode,
                                         tsp00_Uint2 *   pusServicePort,
                                         SAPDB_Char *    defaultService,
                                         tsp00_ErrTextc  pErrText)
{
    int     rc;
    char *  ServiceString = NULL ;
    char *  pServiceInfo = NULL;
        

    DBGIN;

    pServiceInfo = strchr(pszServerNode, ':');

    if ( NULL != pServiceInfo )
    {
        
        if ( pServiceInfo[1] )
        {
            switch( pServiceInfo[1] )
            {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                *pusServicePort = atoi(&pServiceInfo[1]);
                ServiceString = 0;
                break;
            default:
                ServiceString = &pServiceInfo[1];
                break;
            }
        }
        else
        {
            ServiceString = pServiceInfo;
        }

        /* port is removed from pszServerNode */
        memset (pServiceInfo, 0, 1);
    }
    else
    {
        ServiceString = defaultService;
    }

    if ( NULL != ServiceString )
    {
        rc = sql43_get_service_by_name (ServiceString,
                                        pusServicePort);

        if ( rc != 0 )
        {
    	    en42FillErrText(pErrText, "unknown service '%s' (see /etc/services)", ServiceString);
    	    DBG1 (( MF__,"leaving %d \n", SP1CE_NOTOK ))
            DBGOUT;
	        return commErrNotOk_esp01;
        }
    }

    DBGOUT;
    return commErrOk_esp01;
}


#undef	MF__
#define	MF__	MOD__"sql42_get_server_address"
int sql42_get_server_address (
struct sockaddr     *sa ,
char				*node ,
tsp00_ErrTextc       pErrText )
{
    int             rc ;
    unsigned short  sql42_serv_port ;
    tsp00_NodeIdc   szServerNode;
    tsp01_CommErr_Enum      commState = commErrOk_esp01;

    DBGIN;

    strcpy (szServerNode, node);

    commState = sql42_GetServicePort (szServerNode,
                                      &sql42_serv_port,
                                      SOCKET_SERVICE_SQL6_EO003,
                                      pErrText);
    if (commErrOk_esp01 != commState)
    {
        DBGOUT;
        return ( SP1CE_NOTOK ) ;
    }
    else
    {
        /*
        *  Build socketaddress of server.
        */
        memset ( sa , 0 , sizeof(struct sockaddr) );
        sa->sa_family = AF_INET ;

        DBG1 (( MF__,"service port %d \n" , sql42_serv_port ))
        sa->sa_data[0] = ( sql42_serv_port >> 8 & 0xFF ); /* high byte */
        sa->sa_data[1] = ( sql42_serv_port      & 0xFF ); /* low  byte */

        /*
        *  Get internet address of host.
        */
        rc = sql43_get_host_by_name ( szServerNode,
            (unsigned char*) sa->sa_data + 2,
            sizeof(sa->sa_data) - 2 );
        if ( rc )
        {
            en42FillErrText(pErrText, "unknown host %s (see /etc/hosts)", szServerNode);
            DBG1 (( MF__,"leaving %d \n", SP1CE_NOTOK ))
            DBGOUT;
            return ( SP1CE_NOTOK ) ;
        }
    }

    DBGOUT;
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */

int sql42_info_request (
char			*dbname ,
int			service ,
long			*maxdatap ,
long			*packetsizep ,
long			*minreplyp ,
struct sockaddr		*sa ,
tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"sql42_info_req"
    int				rc ;
    int				sd ;
    int				messclass ;
    int				serv ;
    int				peerswap ;
    long			maxsegsiz ;
    int				retcod ;
    int				state ;
    tsp00_Int4			myref ;
    tsp00_Int4			peerref ;
    rte_header			*header ;
    SQL_DBNAMEC			mydbname ;
    SQL_DBNAMEC			serverdbname ;
    char			packet [ RTE_CONPKT_SIZE ];
    char			pidbuf [ 32 ];


    DBG1 (( MF__,"entered \n" ))
    header = (rte_header*) packet ;
    maxsegsiz = 16384 ; /* sufficient for connect pkt, HP needs minimum 8192 */

    /*
     *  connect to server socket
     */
    rc = sql42_connect_server ( sa , &sd , pErrText );
    if ( rc )
    {
	    DBG1 (( MF__,"leaving %d \n", rc ))
	    return ( rc );
    }

    /*
     *  send connect packet for info request
     */
    sql42_create_conpkt ( header , RSQL_INFO_REQUEST_EO003 ,
			    (tsp00_Int4)0 , (tsp00_Int4)0 ,
			    SP1CE_OK , service , maxsegsiz , 0L , 0L , 0L ,
			    "" , dbname );
    sp77sprintf ( pidbuf , sizeof(pidbuf), "%ld" , (long) getpid() );
    sql42_put_string ( header , RSQL_OPT_REM_PID_EO003 , pidbuf );
    /* Client intends not to use a reply part */
    sql42_put_int1 (header, RSQL_OPT_OMIT_REPLY_PART_EO003, (tsp00_Uint1) true);
    rc = sql42_send_conpkt ( sd , header , pErrText );
    if ( rc )
    {
    	DBG1(( MF__,": send_conpkt: %s \n", pErrText ));
	    RTE_save_close ( sd );
    	return ( rc );
    }

    /*
     *  Receive reply from server.
     *  In case of an interrupt, give up.
     */
    state = CON_TIMEDOUT ;
    rc = sql42_recv_conpkt ( sd , header , &state , pErrText );
    if ( rc )
    {
	    DBG1(( MF__,": recv_conpkt: %s \n", pErrText ));
	    RTE_save_close ( sd );
	    return ( rc );
    }

    /*
     *  Close the socket. We use another socket for the final session.
     */
    (void) close ( sd );

    sql42_extract_conpkt ( header , &messclass , &myref , &peerref ,
			    &retcod , &serv , &maxsegsiz ,
			    maxdatap , packetsizep , minreplyp ,
			    &peerswap , (char *)mydbname , (char *)serverdbname );

    /*
     *  Check the received information.
     */
    rc = SP1CE_OK ;
    if ( messclass != RSQL_INFO_REPLY_EO003 )
    {
	    DBG1 (( MF__,": server messclass %d expected %d \n",
		        messclass , RSQL_INFO_REPLY_EO003 ))
		en42FillErrText(pErrText, "protocol error: %d no INFO REPLY", messclass );
	    /*
	     *  Treat this as a warning.
	     *  The server might be an old one, not knowing info request.
	     *  Don't do further checks on the packet, since it cannot be
	     *  expected that they are correct for an unknown request.
	     *  Return here with SP1CE_OK: The server's address remains unchanged.
	     */
	    DBG1 (( MF__,"leaving %d \n", rc ))
	    return ( rc );
    }
    /*
     *  After we passed this check, the server is able to handle
     *  the info request. We now check the protocol elements.
     */
    if ( serv != service )
    {
    	DBG1 (( MF__,": server service %d expected %d \n", serv , service ))
		en42FillErrText(pErrText, "protocol error: service %d/%d", serv, service );
	    rc = SP1CE_NOTOK ;
    }
    if ( strcmp ( (const char *)serverdbname , (const char *)dbname ) )
    {
	    DBG1 (( MF__,": server dbname '%s' expected '%s' \n",
				    serverdbname , dbname ))
		en42FillErrText(pErrText, "protocol error: dbname %s/%s", serverdbname, dbname );
	    rc = SP1CE_NOTOK ;
    }
    if ( retcod != SP1CE_OK )
    {
	    if ( rc == SP1CE_OK )
	    {
	        switch ( retcod )
	        {
	            case SP1CE_NOTOK :
    		        en42FillErrText(pErrText, "protocol error");
		            break ;
	            case SP1CE_TIMEOUT :
    		        en42FillErrText(pErrText, "connect timed out");
		            break ;
	            case SP1CE_TASKLIMIT :
    		        en42FillErrText(pErrText, "task limit");
		            break ;
	            case SP1CE_RELEASED :
    		        en42FillErrText(pErrText, "server rejected connection");
		            break ;
	            default :
    		        en42FillErrText(pErrText, "database not running");
		            break ;
	        }
	        rc = (int) retcod ;
	        DBG1 (( MF__,"server returned %d \n", rc ))
	    }
    }
    /* end if retcod != SP1CE_OK */

    /*
     *  Extract TCP/IP port number from info reply packet.
     */
    if ( rc == SP1CE_OK ) sql42_get_port_no ( header , sa );

    return ( rc );
}

/*
 * ===========================================================================
 */

/* PTS 1109405 */
    #undef MF__
    #define MF__ MOD__"en42pingRequest"
tsp01_CommErr_Enum en42pingRequest( char *              pszServerNode,
                                    tsp00_Versionc      szServerVersion, 
                                    SAPDB_Byte *        pServerCertificate,
                                    SAPDB_UInt4         lenCertificate,
                                    SAPDB_Bool          captureServerCert,  
                                    tsp00_ErrTextc      pErrText )
{
    time_t			    date ;
    pid_t			    pid ;
    struct sockaddr	    peerSocketAddr ;
    char                packet [ RTE_CONPKT_SIZE ];
    rte_header		   *header ;
    int				    sd ;
    int				    rc ;
    int                 state ;
    long                maxdata;
    long                packetsize;
    long                minreply;
    int			        messClass;
    tsp01_CommErr_Enum  commState = commErrOk_esp01 ;


    DBGIN;

    header = (rte_header*) packet ;
    date = time ( (time_t*)0 );
    pid = getpid ();

    /*
     *  Get address of server socket.
     */
    rc = sql42_get_server_address ( &peerSocketAddr, pszServerNode, pErrText ); /* PTS 1113308 */
    if ( rc )
    {
	    DBG1 (( MF__,"leaving %d \n", rc ))
        return commErrNotOk_esp01;
    }

    /*
     *  connect to server socket
     */
    rc = sql42_connect_server ( &peerSocketAddr , &sd, pErrText ); /* PTS 1113308 */
    if ( rc )
    {
	    DBG1 (( MF__,"leaving %d \n", rc ))
	    return commErrNotOk_esp01;
    }

    /*
     *  send connect packet for info request
     */
    if (captureServerCert)
    {
        messClass = RSQL_CERT_REQUEST_EO003;
    }
    else
    {
        messClass = RSQL_PING_REQUEST_EO003;
    }

    sql42_create_conpkt (header , 
                         messClass,
			             (tsp00_Int4)0 , 
                         (tsp00_Int4)0 ,
			             SP1CE_OK , 
                         srvUser_esp01 , 
                         16384 , 
                         0L , 0L , 0L , "" , "" );
    rc = sql42_send_conpkt ( sd , header , pErrText ); /* PTS 1113308 */
    if ( rc )
    {
        DBG1(( MF__,": send_conpkt: %s \n", pErrText ));
        RTE_save_close ( sd );
        return ( commErrNotOk_esp01 );
    }

    /*
     *  Receive reply from server.
     *  In case of an interrupt, give up.
     */

    state = CON_TIMEDOUT ;

    if (captureServerCert)
    {
        commState = sql42ReceiveCertificatePacket ((void *) &sd,
                                                   (RECV_PKT_FUNC_PTR_EO003) en42Receive,
                                                   pServerCertificate, 
                                                   lenCertificate,
                                                   pErrText);
    }
    else
    {
        rc = sql42_recv_conpkt ( sd , header , &state , pErrText ); /* PTS 1113308 */
        if ( rc )
        {
            DBG1(( MF__,": recv_conpkt: %s \n", pErrText ));
            RTE_save_close ( sd );
            return commErrNotOk_esp01;
        }

        {
            tsp00_Int4	myref ;
            tsp00_Int4	peerref ;
            int		    retcod ;
            int			serv ;
            long		maxsegsiz ;
            int			peerswap ;
            SQL_DBNAMEC	mydbname ;
            SQL_DBNAMEC	serverdbname ;

            sql42_extract_conpkt ( header , &messClass , &myref , &peerref ,
                &retcod , &serv , &maxsegsiz ,
                &maxdata , &packetsize , &minreply ,
                &peerswap , (char *)mydbname , (char *)serverdbname );

            /*
             *  Check the received information.
             */
            if ( messClass != RSQL_PING_REPLY_EO003 )
            {
                DBG1 (( MF__,": server messClass %d expected %d \n",
                    messClass , RSQL_PING_REPLY_EO003 ))
                    en42FillErrText(pErrText, "PING reply expected");
                return ( commErrNotOk_esp01 );
            }
            /*
             *  After we passed this check, the server is able to handle
             *  the info request. We now check the protocol elements.
             */
            if ( serv != srvUser_esp01 )
            {
                DBG1 (( MF__,": server service %d expected %d \n", serv , srvUser_esp01 ))
                    en42FillErrText(pErrText, "Unexpected service %d", serv);
                return ( commErrNotOk_esp01 );
            }

            if ( retcod != SP1CE_OK )
            {
                if ( rc == SP1CE_OK )
                {
                    switch ( retcod )
                    {
                    case SP1CE_NOTOK :
                        en42FillErrText(pErrText, "protocol error" );
                        break ;
                    case SP1CE_TIMEOUT :
                        en42FillErrText(pErrText, "connect timed out" );
                        break ;
                    case SP1CE_RELEASED :
                        en42FillErrText(pErrText, "server rejected connection" );
                        break ;
                    default :
                        en42FillErrText(pErrText, "PING request failed: retcod %d", retcod );
                        break ;
                    }
                    return commErrNotOk_esp01;
                }
            } /* end if retcod != SP1CE_OK */
        }

        sql42_get_string( header , RSQL_OPT_VERSION_EO003, sizeof(tsp00_Versionc), szServerVersion );
    }
    /* 
     *  Close the socket. No longer needed
     */

    RTE_save_close ( sd );
    return commState;
}

/*
 * ===========================================================================
 */

int sql42_connect_server (
struct sockaddr *sa ,
int				*sdp ,
tsp00_ErrTextc   pErrText )
{
#undef	MF__
#define	MF__	"sql42_connect_server"
    int                         rc ;
    int				retry ;


    DBGIN;

    /*
     *  create socket to connect to server
     */
    *sdp = socket ( AF_INET , SOCK_STREAM , 0 );
    if ( *sdp == UNDEF )
    {
		DBG1 (( MF__,": socket create error: %s\n", sqlerrs() ))
		en42FillErrText(pErrText, "Cannot create socket (%d:%s)", errno, sqlerrs() ) ;
		return ( SP1CE_NOTOK );
    }
    DBG1 (( MF__,"created socket %d \n", *sdp ))

    /*
     *  Connect to server socket.
     *  Sometimes, a protocol error happens. Simply retry then.
     */
    DBG1 (( MF__,"connecting to server sd %d \n", *sdp ))
/*    DBGADR (( MF__,"addr %s \n", sa )) */
    for ( retry = 0 ; retry < 3 ; retry ++ )
    {
		rc = connect ( *sdp , sa , sizeof(struct sockaddr) );
        if ( (rc == -1) && (errno == EPROTO) )
		{
			MSGD(( WRN_CONNECT_SERVER_RETRY, sqlerrs() ));
		}
		else
		{
			break ;
		}
    }
    if ( rc == UNDEF )
    {
	    if ( errno == ECONNREFUSED )
	    {
		    en42FillErrText(pErrText, "connection refused: x_server not running" ) ;
	        DBG1 (( MF__,"x_server not running \n" ))
	        rc = SP1CE_START_REQUIRED ;
	    }
	    else
	    {
		    en42FillErrText(pErrText, "cannot connect to server socket" ) ;
	        DBG1 (( MF__,": socket connect error: %s\n", sqlerrs() ))
	        rc = SP1CE_NOTOK ;
	    }
	    RTE_save_close ( *sdp );
	    *sdp = -1 ;
	    return ( rc );
    }
    DBG1 (( MF__,"connected to server \n" ))

    (void) sql42_socket_live ( *sdp );

    DBG1 (( MF__,"leaving %d \n", SP1CE_OK ))
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */

int sql42_socket_buffer (
int	  sd ,
long *maxsegsizp )
{
#undef	MF__
#define	MF__	"sql42_sockbuf"
    int				rc ;
    int				maxsegsiz ;
    int				idx ;
    char			*ptr ;


    DBG1 (( MF__,"trying maxsegsiz %ld \n", *maxsegsizp ))

    maxsegsiz = (int) *maxsegsizp ;
    if ( maxsegsiz <= 0 )
    {
		maxsegsiz = 65536 ;	/* select a default buffer size */
    }

#ifdef hpux
    if ( maxsegsiz < 8192 )
    {
		maxsegsiz = 8192 ;
    }
#endif
#ifdef SCO
    if ( maxsegsiz > 32768 )
    {
		maxsegsiz = 32768 ;
    }
#endif
#ifdef SQL42_MAXSEGSIZ
    maxsegsiz = SQL42_MAXSEGSIZ ;
#endif

    for ( ; maxsegsiz > 1024 ; )
    {
		rc = setsockopt ( sd , SOL_SOCKET , SO_SNDBUF ,
			    (char*) &maxsegsiz , sizeof(maxsegsiz) );
		if ( (rc != -1) || (errno != EINVAL) ) break ;
		for ( idx = -2 ; maxsegsiz ; idx ++ ) maxsegsiz = maxsegsiz >> 1 ;
		maxsegsiz = 1 << idx ;
    }
    if ( rc == -1 )
    {
		DBG1 (( MF__,": setsockopt (SNDBUF) warning: %s\n", sqlerrs() ))
		rc = -2 ;
    }
    DBG1 (( MF__,"send   maxsegsiz %ld \n", maxsegsiz ))
    for ( ; maxsegsiz > 1024 ; )
	{
		rc = setsockopt ( sd , SOL_SOCKET , SO_RCVBUF ,
					(char*) &maxsegsiz , sizeof(maxsegsiz) );
		if ( (rc != -1) || (errno != EINVAL) ) break ;
		for ( idx = -2 ; maxsegsiz ; idx ++ ) maxsegsiz = maxsegsiz >> 1 ;
		maxsegsiz = 1 << idx ;
	}
    if ( rc == -1 )
    {
		DBG1 (( MF__,": setsockopt (RCVBUF) warning: %s\n", sqlerrs() ))
		rc = -2 ;
    }
    DBG1 (( MF__,"recv   maxsegsiz %ld \n", maxsegsiz ))

    /*
     *  If setsockopt suceeded for both directions,
     *  the segment size is adapted to the buffer size.
     *  Otherwise we work with an undefined socket buffer size
     *  and try to work with an unchanged segment size
     *  (may be inefficient, but possibly works).
     */
    if ( rc != -2 )
    {
	    /*
	     *  OLD: The socket buffer may have a certain minimum size.
	     *  OLD: Nevertheless the segment size must not be increased.
	     *  SCO (& perhaps others ?): Socket buffer must not exceed
	     *      about 63K. send(3N) and recv(3N) can do bigger transfers.
	     *      Therefore, 'maxsegsiz' will not be decreased.
	     */
	    if ( (*maxsegsizp <= 0)
	      || (*maxsegsizp > (long) maxsegsiz)
           )
	    {
	        DBG1 (( MF__,"max_segment_size %ld -> %ld \n", *maxsegsizp , maxsegsiz ))
	        *maxsegsizp = (long) maxsegsiz ;
	    }
    }

    ptr = getenv ( "DBSEGMENTSIZE" );
    if ( ptr )
    {
	    maxsegsiz = atoi ( ptr );
	    if ( maxsegsiz <= RTE_HEADER_SIZE )
	    {
	        DBG1 (( MF__,": illegal DBSEGMENTSIZE %s \n", ptr ))
	    }
	    else
	    {
	        DBG1 (( MF__,": DBSEGMENTSIZE %d \n", maxsegsiz ))
	        *maxsegsizp = maxsegsiz ;
	    }
    }

    DBG1 (( MF__,"leaving %d max_segment_size %d \n", SP1CE_OK , *maxsegsizp ))
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */

int sql42_socket_live (
int			sd )
{
#undef	MF__
#define	MF__	"sql42_socket_live"
    int				rc ;
    int				value ;
#ifdef LINGER
    struct linger		ling ;
#endif


    DBG1 (( MF__,"entered \n" ))

    value = TRUE ;
    rc = setsockopt ( sd , SOL_SOCKET , SO_KEEPALIVE ,
			(char*) &value , sizeof(value) );
    if ( rc == -1 )
    {
	DBG1 (( MF__,": setsockopt (KEEPALIVE) warning: %s\n", sqlerrs() ))
    }

    value = TRUE ;
    rc = setsockopt ( sd , SOL_SOCKET , SO_REUSEADDR ,
			(char*) &value , sizeof(value) );
    if ( rc == -1 )
    {
	DBG1 (( MF__,": setsockopt (REUSEADDR) warning: %s\n", sqlerrs() ))
    }

#ifdef LINGER
    ling.l_onoff = 1 ;
    ling.l_linger = 20 ; /* seconds */
    rc = setsockopt ( sd , SOL_SOCKET , SO_LINGER ,
			(char*) &ling , sizeof(ling) );
    if ( rc == -1 )
    {
	DBG1 (( MF__,": setsockopt (LINGER) warning: %s\n", sqlerrs() ))
    }
#endif

    sql42_set_tcp_nodelay_flag ( sd ) ;

    DBG1 (( MF__,"leaving %d \n", SP1CE_OK ))
    return ( SP1CE_OK );
}

/*
 * ===========================================================================
 */

#include <netinet/tcp.h>

static  int     sql42_set_tcp_nodelay_flag (
int                             sd )
{
    int SetOk = FALSE ;

#ifdef TCP_NODELAY

    struct protoent *Protocol = getprotobyname ( "TCP" ) ;

    if ( Protocol )
    {
      int                             rc,value ;

      DBG1 (( MF__,"prot_name: %s, prot_no: %d\n",
			 Protocol->p_name, Protocol->p_proto )) ;

      value = TRUE ;

      rc = setsockopt ( sd, Protocol->p_proto, TCP_NODELAY,
                                    (char*) &value , sizeof(value) );

      if ( rc == UNDEF )
      {
           DBG1 (( MF__, "setsockopt (TCP_NODELAY) error: %s\n", sqlerrs() ));
      }
      else
        SetOk = TRUE ;
    }

#else

        DBG1 (( MF__,"TCP_NODELAY not defined for this machine\n" )) ;

#endif

    return ( SetOk ) ;
}

#endif /* SOCKETS */

externC void en42FillErrText(tsp00_ErrTextc VAR_ARRAY_REF pErrText, char const *format, ...)
{
    va_list args ;
	
	va_start ( args , format );
	sp77vsprintf(pErrText, sizeof(tsp00_ErrTextc), format, args);
    va_end ( args );
}
