/*!*********************************************************************

    @file         RTEUDE_SocketIPC.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#include "RunTime/UDEServer/RTEUDE_SocketIPC.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*----------------------------------------*/

#if defined (_WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#endif

#if defined (_WIN32)
const RTEUDE_SocketIPC::socketHandle_t invalidHandleC
    = (RTEUDE_SocketIPC::socketHandle_t) INVALID_SOCKET;
typedef DWORD counter_t;

const int socketErrorC = SOCKET_ERROR;
static bool socketsInitialized = false;
static WSADATA wsaData;
#define INIT_SOCKET_LIB() if (!socketsInitialized) {\
    WSAStartup (MAKEWORD(1, 1), &wsaData);\
    socketsInitialized = true;}
#define LASTERROR GetLastError()

typedef int socklen_t;

#else

const RTEUDE_SocketIPC::socketHandle_t invalidHandleC = -1;
typedef int counter_t;

const int socketErrorC = -1;
#define closesocket ::close
#define INIT_SOCKET_LIB()
#define LASTERROR errno

#if defined (HPUX) || defined (OSF1)
typedef int local_socklen_t;
#define socklen_t local_socklen_t
#endif

#endif

/*----------------------------------------*/

RTEUDE_SocketIPC::
RTEUDE_SocketIPC ()
{
    this->handle = invalidHandleC;
    this->allocator = 0;
}

/*----------------------------------------*/

RTEUDE_SocketIPC::
RTEUDE_SocketIPC (
    socketHandle_t handle_init,
    SAPDBMem_IRawAllocator * allocator_init)
{
    this->handle = handle_init;
    this->allocator = allocator_init;
}

/*----------------------------------------*/

RTEUDE_SocketIPC::
~RTEUDE_SocketIPC ()
{
    this->close ();
}

/*----------------------------------------*/

bool RTEUDE_SocketIPC::
isValid () const
{
    return this->handle != invalidHandleC;
}

/*----------------------------------------*/

void RTEUDE_SocketIPC::
close ()
{
    if (this->isValid ()) {
        closesocket (this->handle);
        this->handle = invalidHandleC;
    }
}

/*----------------------------------------*/

void RTEUDE_SocketIPC::
destroy ()
{
    if (this->allocator != 0) {
        SAPDBMem_IRawAllocator * alloc = this->allocator;
        this->~RTEUDE_SocketIPC ();
        alloc->Deallocate (this);
    }
    else {
        this->close ();
    }
}

/*----------------------------------------*/

bool RTEUDE_SocketIPC::
rawSend (
    const void * buf_in,
    int length)
{
    const char * buf = (const char *) buf_in;
    while (length > 0) {
        counter_t bytesSent = ::send (this->handle, buf, length, 0);
        if (bytesSent == socketErrorC) {
            int rc = LASTERROR;
            return false;
        }
        length -= bytesSent;
        buf += bytesSent;
    }
    return true;
}

/*----------------------------------------*/

bool RTEUDE_SocketIPC::
rawReceive (
    void * target,
    int targetSize,
    int & outLength)
{
    char * buf = (char *) target;
    outLength = 0;
    while (targetSize > 0) {
        counter_t bytesReceived = ::recv (this->handle, buf, targetSize, 0);
        if (bytesReceived == socketErrorC) {
            return false;
        }
        outLength += bytesReceived;
        targetSize -= bytesReceived;
        buf += bytesReceived;
    }
    return true;
}

/*----------------------------------------*/

RTEUDE_SocketServer::
RTEUDE_SocketServer (
    SAPDBMem_IRawAllocator * allocator_init,
    int portParam)
{
    this->allocator = allocator_init;
    this->port = portParam;
}

/*----------------------------------------*/

RTEUDE_SocketServer::
~RTEUDE_SocketServer ()
{
    this->close ();
}

/*----------------------------------------*/

bool RTEUDE_SocketServer::
listenOnDefaultPort ()
{
    return this->listenOnPort (this->port);
}

/*----------------------------------------*/

bool RTEUDE_SocketServer::
listenOnPort (
    int portParam)
{
    sockaddr_in serverSocketAddr;
    socklen_t   sockAddrSize = sizeof (sockaddr_in);
    int rc;

    INIT_SOCKET_LIB();
    memset (&serverSocketAddr, sizeof (serverSocketAddr), '\0');
    serverSocketAddr.sin_port = htons (portParam);
    serverSocketAddr.sin_family = AF_INET;
    serverSocketAddr.sin_addr.s_addr = htonl (INADDR_ANY);

    this->listenSocket = socket (AF_INET, SOCK_STREAM, 0);
    if (this->listenSocket == invalidHandleC) {
        return false;
    }
    rc = bind (this->listenSocket, reinterpret_cast<sockaddr *>(&serverSocketAddr),
        sizeof (serverSocketAddr));
    if (rc == socketErrorC) {
        this->invalidate ();
        return false;
    }
    rc = getsockname (this->listenSocket, reinterpret_cast<sockaddr *>(&serverSocketAddr),
        &sockAddrSize);
    if (rc == socketErrorC) {
        this->invalidate ();
        return false;
    }
    this->port = ntohs (serverSocketAddr.sin_port);
    rc = listen (this->listenSocket, 0);
    if (rc == socketErrorC) {
        this->invalidate ();
        return false;
    }
    return true;
}

/*----------------------------------------*/

bool RTEUDE_SocketServer::
listenOnDynamicPort ()
{
    return this->listenOnPort (0);
}

/*----------------------------------------*/

bool RTEUDE_SocketServer::
accept (
    RTEUDE_SocketIPC::socketHandle_t & handle)
{
    sockaddr_in clientSocketAddr;
    socklen_t   addrlen = sizeof (clientSocketAddr);
    handle = ::accept (this->listenSocket,
        reinterpret_cast<sockaddr *>(&clientSocketAddr), &addrlen);
    if (handle == invalidHandleC) {
        int error = LASTERROR;
        return false;
    }
#if defined TCP_NODELAY
    unsigned char noDelay = true;
    setsockopt (handle, IPPROTO_TCP, TCP_NODELAY,
            (const char *) &noDelay, sizeof (noDelay));
#endif
    return true;
}

/*----------------------------------------*/

bool RTEUDE_SocketServer::
accept (
    RTEUDE_SocketIPC * socket)
{

    RTEUDE_SocketIPC::socketHandle_t handle;
    if (!this->accept (handle)) {
        return false;
    }
    RTEUDE_SocketIPC tmp (handle, this->allocator);
    *socket = tmp;
    return true;
}

/*----------------------------------------*/

RTEUDE_SocketIPC * RTEUDE_SocketServer::
accept ()
{

    RTEUDE_SocketIPC::socketHandle_t handle;
    if (!this->accept (handle)) {
        return NULL;
    }
    RTEUDE_SocketIPC * result = new (*this->allocator)
        RTEUDE_SocketIPC (handle, this->allocator);
    if (result == NULL) {
        closesocket (handle);
    }
    return result;
}

/*----------------------------------------*/

int RTEUDE_SocketServer::
getPort () const
{
    return this->port;
}

/*----------------------------------------*/

bool RTEUDE_SocketServer::
isValid () const
{
    return (this->listenSocket != invalidHandleC);
}

/*----------------------------------------*/

void RTEUDE_SocketServer::
close ()
{
    this->invalidate ();
}

/*----------------------------------------*/

void RTEUDE_SocketServer::
invalidate ()
{
    if (this->isValid ()) {
        closesocket (this->listenSocket);
        this->listenSocket = invalidHandleC;
    }
}

/*----------------------------------------*/

RTEUDE_ClientSocket::
RTEUDE_ClientSocket ()
{
}

/*----------------------------------------*/

RTEUDE_ClientSocket::
RTEUDE_ClientSocket (
    int port,
    const char * host)
{
    this->connect (port, host);
}

/*----------------------------------------*/

bool RTEUDE_ClientSocket::
connect (
    int port,
    const char * host)
{
    INIT_SOCKET_LIB();
    if (host == NULL) {
        host = "";
    }
    /*
     * convert address
     */
    struct hostent * addrinfo = gethostbyname (host);
    sockaddr_in destSockAddr;
    destSockAddr.sin_port = htons (port);
    destSockAddr.sin_family = AF_INET;

    this->handle = socket (AF_INET, SOCK_STREAM, 0);
    if (this->handle == invalidHandleC) {
        return false;
    }
    char * asIP = inet_ntoa (*(struct in_addr *)*addrinfo->h_addr_list);
    unsigned long destAddr = inet_addr(asIP);
    memcpy (&destSockAddr.sin_addr, &destAddr, sizeof (destAddr));
    /*
     * connect
     */
    int rc = ::connect (this->handle, reinterpret_cast<const sockaddr *>(&destSockAddr),
        sizeof (destSockAddr));
    if (rc == socketErrorC) {
        int diagrc = LASTERROR;
        closesocket (this->handle);
        this->handle = invalidHandleC;
        return false;
    }
    /*
     * set socket options
     */
    struct linger   ling;
    ling.l_onoff = 1 ;
    ling.l_linger = 20 ; /* seconds */
    rc = setsockopt (this->handle, SOL_SOCKET , SO_LINGER ,
			(char*) &ling , sizeof(ling) );
    if (rc != 0) {
        int diagrc = LASTERROR;
    }
#if defined TCP_NODELAY
    struct protoent * protocol = getprotobyname ("TCP");
    char nodelay = 1;
    rc = setsockopt (this->handle, protocol->p_proto, TCP_NODELAY, &nodelay, sizeof (nodelay));
    if (rc != 0) {
        int diagrc = LASTERROR;
    }
#endif
    /*
     *
     */
    return true;
}

/*----------------------------------------*/

void RTEUDE_ClientSocket::
detach ()
{
    this->handle = invalidHandleC;
}

/*----------------------------------------*/

RTEUDE_MessageSocketIPC::
RTEUDE_MessageSocketIPC (
    RTEUDE_SocketServer & server)
: RTEUDE_MessageIPC (server)
{
}

/*----------------------------------------*/

bool RTEUDE_MessageSocketIPC::
receive (
    void * target,
    int targetSize,
    int & outLength)
{
    this->currentConnection = this->listener.accept ();
    if (this->currentConnection == NULL) {
        return false;
    }
    bool result = this->currentConnection->receive (target, targetSize, outLength);
    this->currentConnection->destroy ();
    this->currentConnection = NULL;
    return result;
}

