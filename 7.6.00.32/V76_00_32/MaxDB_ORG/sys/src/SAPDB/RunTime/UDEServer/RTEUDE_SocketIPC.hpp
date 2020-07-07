/*!*********************************************************************

    @file         RTEUDE_SocketIPC.hpp

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

#ifndef RTEUDE_SOCKETIPC_HPP
#define RTEUDE_SOCKETIPC_HPP

#include "RunTime/UDEServer/RTEUDE_IPC.hpp"

class SAPDBMem_IRawAllocator;

/*!

  @class RTEUDE_SocketIPC

 */
class RTEUDE_SocketIPC
    : public RTEUDE_IPC
{
public:
#if defined (_WIN32)
    typedef unsigned long socketHandle_t;
#else
    typedef int socketHandle_t;
#endif

public:
    RTEUDE_SocketIPC ();
    explicit RTEUDE_SocketIPC (socketHandle_t handle, SAPDBMem_IRawAllocator * allocator);
    ~RTEUDE_SocketIPC ();
    virtual bool isValid () const;
    virtual void close ();
    virtual void destroy ();

protected:
    virtual bool rawSend (const void * buf, int length);
    virtual bool rawReceive (void * target, int targetSize, int & outLength);
    socketHandle_t handle;
    SAPDBMem_IRawAllocator  * allocator;
};

/*----------------------------------------*/

/*!

  @class RTEUDE_SocketServer

 */
class RTEUDE_SocketServer
    : public RTEUDE_IPCListener
{
public:
    RTEUDE_SocketServer (SAPDBMem_IRawAllocator * allocator, int portParam = 0);
    ~RTEUDE_SocketServer ();
    bool listenOnDefaultPort ();
    bool listenOnPort (int portParam);
    bool listenOnDynamicPort ();
    virtual RTEUDE_SocketIPC *  accept ();
    bool accept (RTEUDE_SocketIPC * socket);
    int getPort () const;
    bool isValid () const;
    virtual void close ();

private:
    SAPDBMem_IRawAllocator * allocator;
    int port;
    RTEUDE_SocketIPC::socketHandle_t listenSocket;
    void invalidate ();
    bool accept (RTEUDE_SocketIPC::socketHandle_t & handle);
    void operator=(const RTEUDE_SocketServer & dummy) {};
};

/*!

  @class RTEUDE_ClientSocket

 */
class RTEUDE_ClientSocket
    : public RTEUDE_SocketIPC
{
public:
    RTEUDE_ClientSocket ();
    explicit RTEUDE_ClientSocket (int port, const char * host = 0);
    bool connect (int port, const char * host = 0);
    void detach ();
};

/*!

  @class RTEUDE_MessageSocketIPC

 */
class RTEUDE_MessageSocketIPC
    : public RTEUDE_MessageIPC
{
public:
    RTEUDE_MessageSocketIPC (RTEUDE_SocketServer & server);
    virtual bool receive (void * target, int targetSize, int & outLength);
};


#endif
