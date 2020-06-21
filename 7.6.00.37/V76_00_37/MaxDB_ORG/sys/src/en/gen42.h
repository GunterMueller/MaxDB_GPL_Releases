/*!
  @file           gen42.h
  @author         JoergM
  @special area   remote_sql_functions
  @brief          create/extract/read/write remote sql_packets
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



#ifndef GEN42_H
#define GEN42_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp01.h"
#include "gen00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

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

externC tsp00_Uint1 sql42_new_swap_type ();

#ifdef SOCKETS

externC void sql42_put_port_no ( rte_header *header ,
								 struct sockaddr *address );

externC void sql42_get_port_no ( rte_header *header ,
								 struct sockaddr *address );

#endif

externC void sql42_put_string ( rte_header *header ,
								char        type ,
								char       *str );

externC void sql42_get_string ( rte_header *header ,
							    char        type ,
							    int         size ,
								char       *str );

externC void sql42_put_int1 ( rte_header  *header ,
							  char         type ,
							  tsp00_Uint1     value );

externC void sql42_get_int1 ( rte_header  *header ,
						      char         type ,
							  tsp00_Uint1    *valuep );

externC void sql42_put_int4 ( rte_header  *header ,
							  char         type ,
							  tsp00_Int4     value );

externC void sql42_get_int4 ( rte_header  *header ,
							  char         type ,
							  tsp00_Int4    *valuep );

externC void sql42_put_long ( rte_header  *header ,
							  char         type ,
							  long         value );

externC void sql42_get_long ( rte_header  *header ,
						 	  char         type ,
							  long        *valuep );

externC void sql42_create_conpkt ( rte_header *header ,
								   int         messclass ,
								   tsp00_Int4    myref ,
								   tsp00_Int4    peerref ,
								   int         retcode ,
								   int         service ,
								   long        maxsegsiz ,
								   long        maxdata ,
								   long        pktsize ,
								   long        minreply ,
							       char       *mydb ,
								   char       *peerdb );

externC void sql42_extract_conpkt ( rte_header  *header ,
									int         *messclass ,
									tsp00_Int4    *myref ,
									tsp00_Int4    *peerref ,
									int         *retcode ,
									int         *service ,
									long        *maxsegsiz ,
									long        *maxdata ,
									long        *pktsize ,
									long        *minreply ,
									int         *swap_type ,
									char        *mydb ,
									char        *peerdb );

externC int  sql42_SendCertificatePacket    (int                            sendHandle,
                                             int			                messclass,
                                             tsp00_Int4                     myref,
                                             tsp00_Int4                     peerref,
                                             int			                retcode,
                                             SAPDB_Byte VAR_VALUE_REF       pServerCertificate,
                                             SAPDB_UInt4                    lenCertificate,
                                             tsp00_ErrTextc VAR_ARRAY_REF   pErrText);

externC int  sql42_send_conpkt ( int              sd ,
								 rte_header      *header ,
								 tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int  sql42_recv_conpkt ( int              sd ,
								 rte_header      *header ,
								 int             *statep ,
								 tsp00_ErrTextc VAR_ARRAY_REF pErrText );

#ifdef SOCKETS

externC void sql42_send_control ( int                 sd ,
								  int                 messclass ,
								  tsp00_Int4            myref ,
								  tsp00_Int4            peerref ,
								  int                 retcod );

externC int sql42_send_packet ( int         sd ,
								rte_header      *header ,
								long            maxsegsiz ,
	   			                tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int sql42_recv_packet ( int         sd ,
								rte_header      *header ,
								int         swap_type ,
								long            siz ,
	   			                tsp00_ErrTextc VAR_ARRAY_REF pErrText,
								char            *cache_buf ,
								long            *cache_lgt );

#endif /* SOCKETS */

externC void sql42_unpack_int2 ( int             typ ,
								 tsp00_Int2        src ,
								 tsp00_Int2       *dst );

externC void sql42_unpack_int4 ( char            typ ,
								 tsp00_Int4        src ,
								 tsp00_Int4       *dst );

#ifdef SOCKETS

externC tsp01_CommErr_Enum sql42_GetServicePort (char *          pszServerNode,
                                                 tsp00_Uint2 *   pusServicePort,
                                                 SAPDB_Char *    defaultService,
                                                 tsp00_ErrTextc  pErrText);

externC int sql42_get_server_address ( struct sockaddr  *sa ,
									   char             *node ,
									   tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int sql42_info_request ( char            *dbname ,
								 int              service ,
								 long            *maxdatap ,
								 long            *packetsizep ,
								 long            *minreplyp ,
								 struct sockaddr *sa ,
                                 tsp00_ErrTextc VAR_ARRAY_REF pErrText );

/* PTS 1109405 */
/*
  Function: en42pingRequest
  Description: Send a PING Request packet via TCP/IP to the given server
  Retrieve Server version from anser
  Arguments: pszServerNode [in] the name of the server
             szServerVersion [out] the retrieved version
             errText [out] error message if error occured
  Return value: commErrOk_esp01 on success (szServerVersion valid), commErrNotOk on error (errText valid)
 */
externC tsp01_CommErr_Enum en42pingRequest (char *                          pszServerNode ,
                                            tsp00_Versionc VAR_ARRAY_REF    szServerVersion,
                                            SAPDB_Byte VAR_VALUE_REF        pServerCertificate,
                                            SAPDB_UInt4                     lenCertificate,
                                            SAPDB_Bool                      captureServerCert,
			                                tsp00_ErrTextc VAR_ARRAY_REF    errText );


externC int sql42_connect_server ( struct sockaddr  *sa ,
								   int              *sdp ,
                                   tsp00_ErrTextc VAR_ARRAY_REF pErrText );

externC int sql42_socket_buffer ( int   sd ,
								  long *maxsegsizp );

externC int sql42_socket_live ( int sd );

#endif /* SOCKETS */

externC tsp00_Longuint
en42SocketSendPacket ( void                         *SendHandle,
                       char                         *pData,
                       int                           Len,
                       tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC tsp00_Longuint 
en42Receive ( void                         *RecvHandle,
              char                         *pData,
              tsp00_Longuint                ulMaxDataLen,
              tsp00_Longuint               *BytesReceived,
              tsp00_ErrTextc VAR_ARRAY_REF  pErrText );

externC void
en42FillErrText(tsp00_ErrTextc VAR_ARRAY_REF pErrText, char const *format, ...);

#endif  /* GEN42_H */
