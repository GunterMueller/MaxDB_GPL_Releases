/*!*********************************************************************

    @file         ProcRTE_Socket.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "RunTime/ProcServer/ProcRTE_Socket.hpp"

#include "RunTime/ProcServer/ProcRTE_Runtime.hpp"

#if defined (WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#endif

const int lenBytesC = 4;

#if defined (WIN32)

const procrte_handle invalidHandleC = INVALID_SOCKET;
const int socketErrorC = SOCKET_ERROR;
static bool socketsInitialized = false;
static WSADATA wsaData;
#define INIT_SOCKET_LIB() if (!socketsInitialized) {\
    WSAStartup (MAKEWORD(1, 1), &wsaData);\
    socketsInitialized = true;}
#define LASTERROR GetLastError()
typedef int socklen_t;

#else

const procrte_handle invalidHandleC = -1;
const int socketErrorC = -1;
#define closesocket ::close
#define INIT_SOCKET_LIB()
#define LASTERROR errno

#if defined (OSF1)
typedef int socklen_t;
#endif

#if defined (HPUX)
typedef int hp_socklen_t;
#define socklen_t hp_socklen_t
#endif

#endif

/*----------------------------------------*/

static void
logSocketResult (
    const char * call,
    procrte_handle handle,
    int rc,
    int argument)
{
    if (rc < 0) {
        ProcRTE_Runtime::logError ("socket::%s (%d, %d) ERR %d", call, handle, argument, LASTERROR);
    }
    else {
        ProcRTE_Runtime::log ("socket::%s (%d, %d) %d", call, handle, argument, rc);
    }
}

/*----------------------------------------*/

ProcRTE_Socket::ProcRTE_Socket ()
{
    this->handle = invalidHandleC;
}

/*----------------------------------------*/

ProcRTE_Socket::ProcRTE_Socket (
    procrte_handle socketHandle)
{
    this->handle = socketHandle;
}

/*----------------------------------------*/

ProcRTE_Socket::~ProcRTE_Socket ()
{
    this->close ();
    if (this->handle != invalidHandleC) {
        closesocket (this->handle);
    }
}


/*----------------------------------------*/
#if 0
#include <stdio.h>

static FILE * tracer = NULL;

static const char hexDigitsC [] = "0123456789abcdef";

static void
traceSocketComm (
    const char * comment,
    const char * sendBuf,
    int sendLen)
{
    if (tracer == NULL) {
        tracer = fopen ("D:/dbs/wrk/DB76/ude-comm.log", "w");
    }
    fprintf (tracer, "%s %d bytes\n", comment, sendLen);
    const unsigned int bytesPerLineC = 16;
    const int lineSizeC = 79;
    char line [lineSizeC + 2];
    const unsigned char * bufPtr = (const unsigned char *) sendBuf;
    unsigned int endPos = sendLen;
    int chunkPos;

    for (unsigned int pos = 0; pos < endPos; pos += bytesPerLineC, bufPtr += bytesPerLineC) {
        memset (line, ' ', lineSizeC);
        int linePos;
        /*
         * write position (4 bytes)
         */
        linePos = 0;
        unsigned int tmpPos = pos;
        for (int posDigit = 3; posDigit >= 0; --posDigit) {
            char hexDigit = hexDigitsC [tmpPos % 16];
            line [linePos + posDigit] = hexDigit;
            tmpPos /= 16;
        }
        /*
         * write 16 * (hex hex blank)
         */
        linePos = 6;
        int bytesInLine = (endPos - pos) > bytesPerLineC
                ? bytesPerLineC
                : (endPos - pos);
        for (chunkPos = 0; chunkPos < bytesInLine; ++chunkPos) {
            unsigned char byte = bufPtr [chunkPos];
            line [linePos] = hexDigitsC [byte / 16];
            line [linePos + 1] = hexDigitsC [byte % 16];
            linePos += 3;
        }
        /*
         * write | (16 * plain) |
         */
        linePos = 54;
        line [linePos] = '|';
        ++linePos;
        for (chunkPos = 0; chunkPos < bytesInLine; ++chunkPos) {
            unsigned char byte = bufPtr [chunkPos];
            if (byte < 32) {
                byte = '.';
            }
            line [linePos] = byte;
            ++linePos;
        }
        line [linePos] = '|';
        line [linePos + 1] = '\n';
        line [linePos + 2] = '\0';
        /*
         * output
         */
        fputs (line, tracer);
        fflush (tracer);
    }
}
#endif
/*----------------------------------------*/

bool
ProcRTE_Socket::send (
    const char * sendBuf,
    int sendLen)
{
    int rc;
    bool ok = false;
    unsigned char lenbuf [lenBytesC];

    ProcRTE_Runtime::log ("sending %d bytes on %d", sendLen, this->handle);
    lenbuf [3] = sendLen % 256;
    lenbuf [2] = (sendLen >>  8) % 256;
    lenbuf [1] = (sendLen >> 16) % 256;
    lenbuf [0] = (sendLen >> 24) % 256;

    // traceSocketComm ("sending", (const char *) lenbuf, sizeof (lenbuf));
    rc = ::send (this->handle, (const char *) lenbuf, sizeof (lenbuf), 0);
    logSocketResult ("send", this->handle, rc, sizeof (lenbuf));
    if (rc == sizeof (lenbuf)) {
        if (sendLen > 0) {
            // traceSocketComm ("sending", sendBuf, sendLen);
            rc = ::send (this->handle, sendBuf, sendLen, 0);
            logSocketResult ("send", this->handle, rc, sendLen);
            ok = (rc == sendLen);
        }
        else {
            ok = true;
        }
    }
    return ok;
}

/*----------------------------------------*/

bool
ProcRTE_Socket::send (
    const char * sendBuf)
{
    return this->send (sendBuf, strlen (sendBuf));
}

/*----------------------------------------*/

bool
ProcRTE_Socket::receive (
    char * recvBuf,
    int maxRecvBufSize,
    int * recvLen)
{
    int rc;
    bool ok = false;
    unsigned char lenbuf [lenBytesC];
    int bytesRemaining;
    int bytesRead = 0;
    char * receivePtr;

    rc = ::recv (this->handle, (char *) lenbuf, sizeof (lenbuf), 0);
    logSocketResult ("recv", this->handle, rc, sizeof (lenbuf));
    if (rc == sizeof (lenbuf)) {
        bytesRemaining = (lenbuf [0] << 24)
                       + (lenbuf [1] << 16)
                       + (lenbuf [2] <<  8)
                       + (lenbuf [3]);
        if (bytesRemaining > 0) {
            ProcRTE_Runtime::log ("waiting for %d bytes", bytesRemaining);
        }
        else {
            ProcRTE_Runtime::log ("no bytes to read");
        }
        receivePtr = recvBuf;
        while ((rc > 0) && (bytesRemaining > 0)) {
            rc = ::recv (this->handle, receivePtr, bytesRemaining, 0);
            logSocketResult ("recv", this->handle, rc, bytesRemaining);
            if (rc > 0) {
                bytesRemaining -= rc;
                bytesRead += rc;
                receivePtr += rc;
            }
        }
        if (bytesRemaining == 0) {
            ok = true;
        }
    }
    *recvLen = bytesRead;
    return ok;
}

/*----------------------------------------*/

bool
ProcRTE_Socket::call (
    const char * sendBuf,
    int    sendLen,
    char * recvBuf,
    int    maxRecvBufSize,
    int  * recvLen)
{
    bool ok;

    ok = this->send (sendBuf, sendLen);
    if (ok) {
        ok = this->receive (recvBuf, maxRecvBufSize, recvLen);
    }
    return ok;
}

/*----------------------------------------*/

bool
ProcRTE_Socket::isValid () const
{
    return (this->handle != invalidHandleC);
}

/*----------------------------------------*/

bool
ProcRTE_Socket::close ()
{

    if (this->isValid ()) {
        closesocket (this->handle);
        this->handle = invalidHandleC;
    }
    return true;
}

/*----------------------------------------*/

ProcRTE_Socket & ProcRTE_Socket::operator= (
    ProcRTE_Socket & source)
{
    /* no need to check for self assign as the
       same handle should never appear in two socket objects
     */
    this->close ();
    this->handle = source.handle;
    source.handle = invalidHandleC;
    return *this;
}

/*----------------------------------------*/

ProcRTE_SocketServer::ProcRTE_SocketServer (
    int portParam)
{
    this->port = portParam;
}

/*----------------------------------------*/

ProcRTE_SocketServer::~ProcRTE_SocketServer ()
{
    this->close ();
}

/*----------------------------------------*/

bool
ProcRTE_SocketServer::listenOnDefaultPort ()
{
    return this->listenOnPort (this->port);
}

/*----------------------------------------*/

bool
ProcRTE_SocketServer::listenOnPort (
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
    ProcRTE_Runtime::log ("listening on port %d (was %d)",
        this->port, portParam);
    rc = listen (this->listenSocket, 0);
    if (rc == socketErrorC) {
        this->invalidate ();
        return false;
    }
    return true;
}

/*----------------------------------------*/

bool
ProcRTE_SocketServer::listenOnDynamicPort ()
{
    return this->listenOnPort (0);
}

/*----------------------------------------*/

bool
ProcRTE_SocketServer::accept (
    ProcRTE_Socket * socket)
{
    sockaddr_in clientSocketAddr;
    socklen_t   addrlen = sizeof (clientSocketAddr);
    procrte_handle handle = ::accept (this->listenSocket,
        reinterpret_cast<sockaddr *>(&clientSocketAddr), &addrlen);
    if (handle == invalidHandleC) {
        ProcRTE_Runtime::logError ("accept: %d", LASTERROR);
        return false;
    }
#if defined TCP_NODELAY
    unsigned char noDelay = true;
    setsockopt (handle, IPPROTO_TCP, TCP_NODELAY,
            (const char *) &noDelay, sizeof (noDelay));
#endif
    ProcRTE_Socket tmp (handle);
    *socket = tmp;
    return true;
}

/*----------------------------------------*/

int
ProcRTE_SocketServer::getPort () const
{
    return this->port;
}

/*----------------------------------------*/

bool
ProcRTE_SocketServer::isValid () const
{
    return (this->listenSocket != invalidHandleC);
}

/*----------------------------------------*/

bool
ProcRTE_SocketServer::close ()
{
    this->invalidate ();
    return true;
}

/*----------------------------------------*/

void
ProcRTE_SocketServer::invalidate ()
{
    if (this->isValid ()) {
        closesocket (this->listenSocket);
        this->listenSocket = invalidHandleC;
    }
}

/*----------------------------------------*/

ProcRTE_ClientSocket::ProcRTE_ClientSocket ()
{
}

/*----------------------------------------*/

ProcRTE_ClientSocket::ProcRTE_ClientSocket (
    int port,
    const char * host)
{
    this->connect (port, host);
}

/*----------------------------------------*/

bool
ProcRTE_ClientSocket::connect (
    int port,
    const char * host)
{
    INIT_SOCKET_LIB();
    if (host == NULL) {
        host = "127.0.0.1";
    }
    unsigned long destAddr = inet_addr (host);
    sockaddr_in destSockAddr;
    memcpy (&destSockAddr.sin_addr, &destAddr, sizeof (destAddr));
    destSockAddr.sin_port = htons (port);
    destSockAddr.sin_family = AF_INET;

    this->handle = socket (AF_INET, SOCK_STREAM, 0);
    if (this->handle == invalidHandleC) {
        return false;
    }
    int rc = ::connect (this->handle, reinterpret_cast<const sockaddr *>(&destSockAddr),
        sizeof (destSockAddr));
    if (rc == socketErrorC) {
        closesocket (this->handle);
        this->handle = invalidHandleC;
        return false;
    }
    return true;
}

/*----------------------------------------*/


