/*!*********************************************************************

    @file         RTEUDE_IPC.hpp

    @author       DanielD

    @brief        communicate between processes using simple
                    request/reply pairs

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

#ifndef RTEUDE_IPC_HPP
#define RTEUDE_IPC_HPP

/*!

  @class RTEUDE_IPC
  @brief        communicate between processes using simple
                    request/reply pairs

 */
class RTEUDE_IPC
{
public:
    virtual ~RTEUDE_IPC ();

    /*!
        @brief send a package of length bytes to the communication partner
     */
    virtual bool send (const void * buf, int length);

    /*!
        @receive the next communication packet

        If the package size exceeds targetSize, the remainder of
        the package is skipped.
     */
    virtual bool receive (void * target, int targetSize, int & outLength);

    /*!
        @brief execute send/receive in one call
     */
    bool call (const void * input, int inlength, void * output, int outSize, int & outLength);

    /*!
        @brief returns true if communication through this channel is possible
     */
    virtual bool isValid () const = 0;

    /*!
        @brief close the communication channel
     */
    virtual void close () = 0;

    /*!
        @brief close and destroy the object

        This method should be called if the object has been created
        by RTEUDE_IPCListener::accept. because the owner of the object
        does not know the Allocator needed to delete the object.
     */
    virtual void destroy () = 0;

protected:
    /*!
       @brief raw data buffer sending
       Must be overloaded by deriving class
       @param buf [in] buffer to send
       @param length [in] number of bytes to send
       @return true sending succeeded, false sending failed
     */
    virtual bool rawSend (const void * buf, int length) = 0;
    /*!
       @brief  raw data buffer receiving
       Must be overloaded by deriving class
       @param target [inout] buffer to fill
       @param targetSize [in] number of bytes in buffer
       @param outLength [out] actually received number of bytes
       @return true sending succeeded, false sending failed
     */
    virtual bool rawReceive (void * target, int targetSize, int & outLength) = 0;

protected:
    /*!
      @brief protected default ctor
     */
    RTEUDE_IPC ();
    /*!
      @brief skip given number of bytes in input stream
      @return true if skipping succeeded, false if failed
     */
    bool skipOnReceive (int byteCount);
    /*!
       @brief send a special packet
       @param buf [in] buffer to send
       @param length [in] number of bytes to send
       @param kind [in] type of special packet
       @return true sending succeeded, false sending failed
     */
    bool sendSpecialpacket (const void * buf, int length, int kind);
    int protocolVersion; ///< protocol version

friend class RTEUDE_MessageIPC;
};

/*!

  @class RTEUDE_Listener
  @brief listens on a connection and creates new RTEUDE_IPC objects

 */
class RTEUDE_IPCListener
{
public:
    /*!
        @brief block until a new client connects to the server and
            return the resulting channel

        Subclasses can return NULL in case of timeouts or errors.
     */
    virtual RTEUDE_IPC * accept () = 0;

    /*!
        @brief close the connection so that no fursther connections
            can be made.
     */
    virtual void close () = 0;

protected:
    RTEUDE_IPCListener () {};
    virtual ~RTEUDE_IPCListener () {};
};

/*!

  @class RTEUDE_MessageReader
  @brief wait for new connection, read one package, reply, close connection

 */
class RTEUDE_MessageIPC
    : public RTEUDE_IPC
{
public:
    /*!
        @brief ctor
        @param listener [in] the listener for the connection
     */
    RTEUDE_MessageIPC (RTEUDE_IPCListener & listener);
    /*!
       @brief dtor
     */
    ~RTEUDE_MessageIPC ();
    virtual bool isValid () const;
    virtual void close ();
    virtual void destroy ();

protected:
    virtual bool rawSend (const void * buf, int length);
    virtual bool rawReceive (void * target, int targetSize, int & outLength);
    RTEUDE_IPCListener & listener;
    RTEUDE_IPC * currentConnection;
};

#endif

