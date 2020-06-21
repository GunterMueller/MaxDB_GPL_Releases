/*



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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





*/
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

/* some types used by Bill */
#ifndef WIN32

typedef int			        SOCKET;
typedef struct sockaddr_in	SOCKADDR_IN;
typedef struct in_addr		IN_ADDR;

#ifdef LINUX
#define SD_SEND	1
#define SD_BOTH	2
#pragma SHUT_RDWR
/*#define SD_BOTH	SHUT_RDWR*/
#else
#define SD_SEND	SHUT_WR
#define SD_BOTH	SHUT_RDWR
#endif

/* return codes from sockent functions used by Bill */
#define SOCKET_ERROR	-1
#define INVALID_SOCKET	-1

#endif

#define CONNECTIOM_TIMEOUT 5

struct st_tcp_connection
{
 SOCKET		s;
 int		Connected;
 int		KeepAlive;
 SOCKADDR_IN	sa;
};

struct st_http
{
 struct st_tcp_connection	Client;
 struct st_tcp_connection	Server;
};

int ProxyServer
(
 char	IniFilePath[]
);

unsigned long TcpGetIp
(
 struct st_tcp_connection	*tcp_con
);
