/*	istnet.h	

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

*/

#ifndef	_NETWORK_DEFINITIONS
#define	_NETWORK_DEFINITIONS

#if vax
#   include				<socket.h>
#   include				<sockerr.h>
#   include				<netdb.h>
#else
#   if  apollo
#       define	u_short			ushort
#   endif
#   if  T31 || T35
#       include				<sys/uio.h>
#   endif
#   if  mips_mips
#       include				<bsd/sys/socket.h>
#       include				<bsd/netdb.h>
#   else
#       if  WIN32
#           include			<winsock.h>
#       else
#           include			<sys/socket.h>
#           include			<netinet/in.h>
#           include			<arpa/inet.h>
#           include			<netdb.h>
#           if  SVR4 || SOLARIS || _IBMR2
#             if ! defined(linux)
#               include			<sys/select.h>
#             endif
#           endif
#       endif
#   endif
#endif

#ifndef	SOMAXCONN
#   define	SOMAXCONN	5
#endif

#if ! vax
#   define	skt_open		socket
#   define	skt_close		close
#   define	skt_connect		connect
#   define	skt_bind		bind
#   define	skt_listen		listen
#   define	skt_accept		accept
#   define	skt_shutdown		shutdown
#   define	skt_send		send
#   define	skt_recv		recv
#   define	skt_getname		getsockname
#   define	skt_getopt		getsockopt
#   define	skt_setopt		setsockopt
#   define	skt_geterrno		geterrno
#   define	skt_seterrno		seterrno
#   define	skt_serrno		serrno
#   define	skt_gpbyname		getprotobyname
#   define	skt_gsbyname		getservbyname
#   define	skt_ghbyname		gethostbyname
#   define	skt_ghbyaddr		gethostbyaddr
#   define	skt_hostname		gethostname
#   define	skt_select		select
#   define	skt_htons		htons
#   define	skt_ntohs		ntohs
#   define	skt_inet_addr		inet_addr
#   define	skt_inet_ntoa		inet_ntoa
#   define	skt_peername		getpeername
#endif

#if WIN32
#   undef	skt_close
#   define	skt_close		closesocket
#   undef	skt_geterrno
#   define	skt_geterrno		WSAGetLastError
#   undef	skt_seterrno
#   define	skt_seterrno		WSASetLastError
#   undef	skt_serrno
#   define	EADDRINUSE		WSAEADDRINUSE
#   define	ECONNREFUSED		WSAECONNREFUSED
#   define	EADDRNOTAVAIL		WSAEADDRNOTAVAIL
#endif

#if OS2
#   undef	skt_close
#   define	skt_close		soclose
#endif

extern	char			*skt_serrno ();

#if	defined(unix)
#if	!defined(SVR4) && !defined(SOLARIS)
    extern	int			skt_open ();
    extern	int			skt_close ();
    extern	int			skt_connect ();
    extern	int			skt_bind ();
    extern	int			skt_listen ();
    extern	int			skt_accept ();
    extern	int			skt_shutdown ();
    extern	int			skt_send ();
    extern	int			skt_recv ();
    extern	int			skt_getname ();
    extern	int			skt_getopt ();
    extern	int			skt_setopt ();
    extern	struct protoent		*skt_gpbyname ();
    extern	struct servent		*skt_gsbyname ();
    extern	struct hostent		*skt_ghbyname ();
    extern	struct hostent		*skt_ghbyaddr ();
    extern	int			skt_select ();
    extern	int			skt_peername ();
#endif	/*!SVR4  && !defined(SOLARIS) */
    extern	int			skt_hostname ();
#endif	/*!unix*/


struct ISOCKET
{
    int				isk_sd ;
    unsigned			isk_timeout ;
    unsigned			isk_bufsiz ;
    unsigned			isk_recv_lgt ;
    unsigned			isk_send_lgt ;
    char			*isk_recv_buf ;
    char			*isk_send_buf ;
    char			*isk_recv_pos ;
    char			*isk_send_pos ;
};
typedef	struct ISOCKET		ISOCKET ;

/*	connect.c	*/
extern	int			connect_server ();
extern	int			release_server ();

/*	get_node.c	*/
extern	int			get_node_name ();
extern	int			get_node_by_name ();
extern	int			get_node_by_addr ();

/*	isocket.c	*/
extern	ISOCKET			*create_isocket ();
extern	int			drop_isocket ();
extern	int			fetch_isocket ();
extern	int			flush_isocket ();
extern	int			get_line_isocket ();
extern	int			put_isocket ();

/*	listen.c	*/
extern	int			open_listen_socket ();

/*	live.c		*/
extern	void			socket_live ();

#endif /*_NETWORK_DEFINITIONS*/

