/*!*********************************************************************

    @file         ProcRTE_Socket.hpp
    
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

#ifndef PROCRTE_SOCKET_HPP
#define PROCRTE_SOCKET_HPP

#include "RunTime/ProcServer/ProcRTE_StdTypes.hpp"

/*!

  @class ProcRTE_Socket
  @brief exchanging variable sized packages over a socket
  
  All communication is done in ping pong style, with the first
  four bytes of every package containing the length of the remaining 
  package in big endian order.

 */
class ProcRTE_Socket
{
public:
    ProcRTE_Socket ();
    ProcRTE_Socket (procrte_handle socketHandle);
    virtual ~ProcRTE_Socket ();
    bool send (const char * sendBuf, int sendLen);
    bool send (const char * sendBuf);
    bool receive (char * recvBuf, int maxRecvBufSize, int * recvLen);
    bool call (const char * sendBuf, int sendLen, char * recvBuf, int maxRecvSize, int * recvLen);
    bool isValid () const;
    bool close ();
    ProcRTE_Socket & operator= (ProcRTE_Socket & source);
    
protected:
    procrte_handle handle;
};

/*!

  @class ProcRTE_SocketServer

 */
class ProcRTE_SocketServer
{
public:
    ProcRTE_SocketServer (int portParam = 0);
    ~ProcRTE_SocketServer ();
    bool listenOnDefaultPort ();
    bool listenOnPort (int portParam);
    bool listenOnDynamicPort ();
    bool accept (ProcRTE_Socket * socket);
    int getPort () const;
    bool isValid () const;
    bool close ();
private:
    int port;
    procrte_handle listenSocket;
    void invalidate ();
    void operator=(const ProcRTE_SocketServer & dummy) {};
};

/*!

  @class ProcRTE_ClientSocket

 */
class ProcRTE_ClientSocket
    : public ProcRTE_Socket
{
public:
    ProcRTE_ClientSocket ();
    explicit ProcRTE_ClientSocket (int port, const char * host = 0);
    bool connect (int port, const char * host = 0);
};

#endif
