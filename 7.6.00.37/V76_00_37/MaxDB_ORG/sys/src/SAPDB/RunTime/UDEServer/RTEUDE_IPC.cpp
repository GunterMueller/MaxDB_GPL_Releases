/*!*********************************************************************

    @file         RTEUDE_IPC.cpp

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

#include "RunTime/UDEServer/RTEUDE_IPC.hpp"

#include <string.h>

/*----------------------------------------*/

typedef unsigned char byte_t;

/*!
  Interprocess communication header
 */
typedef struct IpcHeaderT {
    byte_t protocolVersion [2];  // big endian uint2
    byte_t packetKind;
    byte_t options;              // set
    byte_t fullLength [4];       // big endian uint4
        // fullLength = sizeof (header) + dataLength
        // means that the header can be extended and
        // older libs can still skip correctly to
        // the end of the packet
} IpcHeaderT;

const unsigned int currentProtocolVersionC = 1;

const unsigned int dataPacketC = 1;
const unsigned int errorPacketC = 2;

const int noOptions = 0;
const int noReplyC = 1;    // set by client to signal that no reply is expected

/*----------------------------------------*/

RTEUDE_IPC::
RTEUDE_IPC ()
{
    this->protocolVersion = currentProtocolVersionC;
}

/*----------------------------------------*/

RTEUDE_IPC::
~RTEUDE_IPC ()
{
}

/*----------------------------------------*/

inline void
setUint2 (
    byte_t * target,
    unsigned int value)
{
    target [0] = value / 256;
    target [1] = value % 256;
}

/*----------------------------------------*/

inline void
setUint4 (
    byte_t * target,
    unsigned int value)
{
    target [3] = value % 256;
    target [2] = (value >>  8) % 256;
    target [1] = (value >> 16) % 256;
    target [0] = (value >> 24) % 256;
}

/*----------------------------------------*/

inline unsigned int
getUint2 (
    const byte_t * source)
{
    return source [0] * 256 + source [1];
}

/*----------------------------------------*/

inline unsigned int
getUint4 (
    const byte_t * source)
{
    return (source [0] << 24)
         + (source [1] << 16)
         + (source [2] <<  8)
         +  source [3];
}

/*----------------------------------------*/

bool RTEUDE_IPC::
send (
    const void * buf,
    int length)
{
    return this->sendSpecialpacket (buf, length, dataPacketC);
}

/*----------------------------------------*/

bool RTEUDE_IPC::
sendSpecialpacket (
    const void * buf,
    int length,
    int kind)
{
    IpcHeaderT header;

    setUint2 (header.protocolVersion, currentProtocolVersionC);
    header.packetKind = kind;
    header.options = 0;
    setUint4 (header.fullLength, length + sizeof (header));
    bool ok = this->rawSend (&header, sizeof (header));
    if (!ok) {
        return ok;
    }
    ok = this->rawSend (buf, length);
    return ok;
}

/*----------------------------------------*/

bool RTEUDE_IPC::
skipOnReceive (
    int bytesRemaining)
{
    while (bytesRemaining > 0) {
        byte_t buf [128];
        int bytesToRead = sizeof (buf);
        if (bytesToRead > bytesRemaining) {
            bytesToRead = bytesRemaining;
        }
        int chunkRead;
        bool ok = this->rawReceive (buf, bytesToRead, chunkRead);
        if (! ok) {
            return false;
        }
        bytesRemaining -= chunkRead;
    }
    return true;
}

/*----------------------------------------*/

bool RTEUDE_IPC::
receive (
    void * target,
    int targetSize,
    int & outLength)
{
    IpcHeaderT header;
    int chunkReceived;

    outLength = 0;
    /*
     * get protocol header
     */
    bool ok = this->rawReceive (&header, sizeof (header), chunkReceived);
    if (!ok || (chunkReceived != sizeof (header))) {
        return false;
    }
    int peerProtocolVersion = getUint2 (header.protocolVersion);
    int packetKind = header.packetKind;
    int fullLength = getUint4 (header.fullLength);
    /*
     * error handling
     */
    if (peerProtocolVersion > currentProtocolVersionC) {
        this->skipOnReceive (fullLength - sizeof (header));
        const char * errorMsg = "unsupported protocol version, require:1";
        this->sendSpecialpacket (errorMsg, strlen (errorMsg), errorPacketC);
        return false;
    }
    if (peerProtocolVersion < currentProtocolVersionC) {
        this->skipOnReceive (fullLength - sizeof (header));
        this->protocolVersion = peerProtocolVersion;
        return false;
    }
    if (packetKind != dataPacketC) {
        this->skipOnReceive (fullLength - sizeof (header));
        return false;
    }
    /*
     *
     */
    int bytesRemaining = fullLength - sizeof (header);
    int overflowToSkip = bytesRemaining - targetSize;
    if (overflowToSkip > 0) {
        bytesRemaining = targetSize;
    }
    byte_t * targetBuf = (byte_t *) target;
    while (ok && (bytesRemaining > 0)) {
        int chunkLen;
        ok = this->rawReceive (targetBuf, bytesRemaining, chunkLen);
        if (!ok) {
            return ok;
        }
        outLength += chunkLen;
        bytesRemaining =- chunkLen;
        targetBuf += chunkLen;
    }
    if (overflowToSkip > 0) {
        this->skipOnReceive (overflowToSkip);
    }
    return true;
}

/*----------------------------------------*/

bool RTEUDE_IPC::
call (
    const void * input,
    int inlength,
    void * output,
    int outSize,
    int & outLength)
{
    bool ok = this->send (input, inlength);
    if (!ok) {
        return ok;
    }
    ok = this->receive (output, outSize, outLength);
    return ok;
}

/*----------------------------------------*/

RTEUDE_MessageIPC::
RTEUDE_MessageIPC (
    RTEUDE_IPCListener & listener_init)
: listener (listener_init)
{
    this->currentConnection = NULL;
}

/*----------------------------------------*/

RTEUDE_MessageIPC::
~RTEUDE_MessageIPC ()
{
}

/*----------------------------------------*/

bool RTEUDE_MessageIPC::
isValid () const
{
    return true;
}

/*----------------------------------------*/

void RTEUDE_MessageIPC::
close ()
{
    // don't close external ipc channel
    // this object is only a wrapper
}

/*----------------------------------------*/

void RTEUDE_MessageIPC::
destroy ()
{
    // don't destroy external ipc channel
    // this object is only a wrapper
}

/*----------------------------------------*/

bool RTEUDE_MessageIPC::
rawSend (
    const void * buf,
    int length)
{
    return this->currentConnection->rawSend (buf, length);
}

/*----------------------------------------*/

bool RTEUDE_MessageIPC::
rawReceive (
    void * target,
    int targetSize,
    int & outLength)
{
    return this->currentConnection->rawReceive (target, targetSize, outLength);
}

/*----------------------------------------*/

