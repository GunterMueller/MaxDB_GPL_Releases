/*!
  @file           ProcRTE_ServerSession.hpp
  @author         DanielD
  @brief          class ServerSession implements the communication
    between the kernel and the ProcServer for one session.

\if EMIT_LICENCE

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



\endif
*/


#ifndef PROCRTE_SERVERSESSION_HPP
#define PROCRTE_SERVERSESSION_HPP

#include "gsp00.h"

#include "RunTime/ProcServer/ProcRTE_Socket.hpp"

class ProcRTE_ServerControl;
class ProcRTE_SocketServer;

/*!
  @class          ProcRTE_ServerSession
  @brief          contains the server side endpoint of
                    a proc server session

  In the current implementation, the ServerControl manages
  the communication packet.
 */

class ProcRTE_ServerSession
{

public:
    /*!
      @brief        creates a  new ProcRTE_ServerSession
     */
    ProcRTE_ServerSession (ProcRTE_ServerControl * serverControl);

    /*!
      @brief        destroys the object and releases all ressources
     */
    ~ProcRTE_ServerSession ();

    /*!
        @brief      release all ressources
     */
    void close ();

    /*!
       @returns     the ServerControl that created this object

     */

    ProcRTE_ServerControl * getServerControl () const;

    /*!
        @brief      wait for the client to connect
     */
    bool acceptConnection ();


    /*!
       @brief       get the address and the size of the communication packet


           Because the request can be overwritten with the reply, only
           one packet address is needed.

     */

    void getPacket (void ** packetAddr, int * packetSize) const;

    /*!
        @brief      return the size of the communication packet
     */
    int getPacketSize () const;

    /*!
       @brief       gets the next request from the ProcServer


           This is either
            - an empty packet, waiting for a procedure call
            - a packet containing the output parameters
            - a SQL command

     */

    bool receive (int * requestLen);

    /*!
       @brief       sends bytesToSend bytes to the ProcServer

     */

    bool send (int bytesToSend);


private:
    ProcRTE_ServerControl * serverControl;
    ProcRTE_Socket          sessionSocket;
    char                  * packet;
    int                     packetSize;
    bool                    isConnected;
};


#endif
