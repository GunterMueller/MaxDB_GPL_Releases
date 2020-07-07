/*!
 * \file    LVC_ProcServerInterface.cpp
 * \author  IvanS
 * \brief   Interface for communicating between ProcServer and kernel.
 */
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

#include "liveCache/LVC_ProcServerInterface.hpp"
#include "liveCache/LVC_ProcServerExecutor.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include <string.h>

/*----------------------------------------------------------------------------*/

void LVC_ProcServerPacketHeader::execute(
  IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  // TODO: not implemented
  throw;
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::getObjectForRead(void* &addr, size_t offssize)
{
  if (m_isLocal) {
    // just map the address from offset
    addr = getAddressForOffset(offssize);
  } else {
    // pick the next packet from queue
    if (m_recvQueuePtr >= m_recvQueueCount) {
      throw DbpError(e_dbproc_no_packet);
    }
    QueueItem *i = m_recvQueue + m_recvQueuePtr++;
    if (i->m_objSize != offssize) {
      throw DbpError(e_dbproc_packet_size);
    }
    addr = i->m_addr;
  }
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::getObjectForWrite(void* &addr, size_t offssize)
{
  if (m_isLocal) {
    // just map the address from offset
    addr = getAddressForOffset(offssize);
  } else {
    // return temporary address where to put the data
    size_t maxSize;
    getSecondarySendArea(addr, maxSize);
    if (offssize > maxSize) {
      // too big size
      throw DbpError(e_dbproc_packet_too_big);
    }
    if (m_writePending) {
      // flush old packet
      sendSecondary(addr, m_writePending);
    }
    m_writePending = offssize;
  }
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::copyObjectForWrite(const void *addr, size_t offssize, size_t size)
{
  if (m_isLocal) {
    memcpy(getAddressForOffset(offssize), addr, size);
  } else {
    if (offssize != size) {
      throw DbpError(e_dbproc_packet_size);
    }
    if (m_writePending) {
      // flush old packet
      void *old;
      getSecondarySendArea(old, offssize /*reuse*/);
      sendSecondary(old, m_writePending);
      m_writePending = 0;
    }
    sendSecondary(addr, size);
  }
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::sendError(int errcode)
{
  LVC_ProcServerPacketHeader *packet;
  size_t maxSize;
  getSendArea(reinterpret_cast<void*&>(packet), maxSize);

  packet->m_code = errcode;
  packet->m_size = sizeof(LVC_ProcServerPacketHeader);
  send(sizeof(LVC_ProcServerPacketHeader));
}

void LVC_ProcServerInterface::queueObjectForRead(void *addr, size_t size)
{
  if (m_recvQueueCount == QUEUE_MAXSIZE) {
    // too many packets to send - queue full
    throw DbpError(e_dbproc_too_many_packets);
  }
  QueueItem *i = m_recvQueue + m_recvQueueCount++;
  i->m_addr = addr;
  i->m_objSize = size;
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::resetQueue()
{
  QueueItem *i = m_recvQueue + m_recvQueueCount;
  while (i > m_recvQueue) {
    --i;
    if (i->m_objSize & QUEUE_FREEMARK) {
      m_alloc->Deallocate(i->m_addr);
    }
  }
  m_recvQueuePtr = m_recvQueueCount = 0;
  m_writePending = 0;
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::sendSecondary(const void *data, size_t dataSize)
{
  // this is not allowed for local communication
  throw DbpError(e_dbproc_invalid_operation);
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::send(size_t size)
{
  if (m_writePending) {
    void *addr;
    size_t maxSize;
    getSecondarySendArea(addr, maxSize);
    sendSecondary(addr, m_writePending);
    m_writePending = 0;
  }
  sendData(size);
}

void LVC_ProcServerInterface::skipErrors()
{
  LVC_ProcServerPacketHeader *packet;
  size_t maxSize;
  getReceiveArea(reinterpret_cast<void*&>(packet), maxSize);
  while (packet->m_code == 0) {
    receiveData();
  }
  resetQueue();
  // error, no error message in packet as such
  throw DbpError(e_dbproc_comm_error);
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::receive()
{
  if (m_isLocal) {
    // only local receive
    receiveData();
    return;
  }

  LVC_ProcServerPacketHeader *packet;
  size_t maxSize;
  getReceiveArea(reinterpret_cast<void*&>(packet), maxSize);

  // receive any outstanding object packets before actual packet
  QueueItem *i = m_recvQueue;
  const QueueItem *stop = m_recvQueue + (m_recvQueueCount ? m_recvQueueCount : QUEUE_MAXSIZE);
  for (;;) {
    receiveData();
    if (packet->m_code == 0) {
      // found one continuation block
      if (i == stop) {
        // no space to receive data, skip other packets
        skipErrors();
      }
      if (m_recvQueueCount) {
        // copy the packet into place
        if (i->m_objSize == packet->m_size - sizeof(LVC_ProcServerPacketHeader)) {
          // size OK
          memcpy(i->m_addr, packet + 1, i->m_objSize);
          ++i;
        } else {
          skipErrors();
        }
      } else {
        // allocate new packet in receive queue
        i->m_objSize = packet->m_size - sizeof(LVC_ProcServerPacketHeader);
        i->m_addr = m_alloc->Allocate(i->m_objSize);
        memcpy(i->m_addr, packet + 1, i->m_objSize);
        i->m_objSize |= QUEUE_FREEMARK;
        ++i;
      }
    } else if (packet->m_code < 0) {
      // have an error message
      resetQueue();
      return;
    } else {
      // have block as such, finish
      break;
    }
  }
  if (m_recvQueueCount) {
    // reset queue
    m_recvQueuePtr = m_recvQueueCount = 0;
    if (i != stop) {
      // not enough packets received
      // error, no error message in packet as such
      throw DbpError(e_dbproc_packet_missing);
    }
  } else {
    // set count
    m_recvQueueCount = i - m_recvQueue;
    m_recvQueuePtr = 0;
  }
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterface::registerPacket(int code, void *vtable)
{
  LVC_ProcServerExecutor::registerPacket(code, vtable);
}

/*----------------------------------------------------------------------------*/
