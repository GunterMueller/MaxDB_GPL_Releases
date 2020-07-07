/*!
 * \file    LVC_ProcServerInterfaceInstance.hpp
 * \author  MarkusSi
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

#ifndef __LVC_PROCSERVER_INTERFACE_INSTANCE
#define __LVC_PROCSERVER_INTERFACE_INSTANCE

#include "liveCache/LVC_ProcServerInterface.hpp"

class RTETask_ITaskClientConnection;

/*!
 * \brief Interface for communicating between ProcServer and kernel.
 *
 * Implementation derived from this interface enables the client and server
 * to communicate. Instance of this class is used as parameter to sink
 * and extender classes in OMS and liveCache.
 *
 * Both server (kernel) and client (OMS) will derive a class from this
 * interface, with appropriate additional routines for shared memory allocator
 * management and stored procedure call.
 *
 * \see \ref omsprocserver
 *
 * \ingroup oms procserver
 */
class LVC_ProcServerInterfaceInstance : public LVC_ProcServerInterface 
{
public:
  /// Construct the instance.
  LVC_ProcServerInterfaceInstance(SAPDBMem_IRawAllocator *pAlloc); 

  ~LVC_ProcServerInterfaceInstance(); 

  /*!
   * \brief Get send area.
   *
   * This method queries the address and size of the send area. Send
   * area is used to send data to the other side.
   *
   * \param addr filled with the address of the send buffer,
   * \param maxSize filled with the size of the send buffer.
   */
  void getSendArea(void* &addr, size_t &maxSize);

  /*!
   * \brief Get secondary send area.
   *
   * This method queries the address and size of the secondary send
   * area. Send area is used to send data to the other side.
   *
   * \param addr filled with the address of the send buffer,
   * \param maxSize filled with the size of the send buffer.
   */
  void getSecondarySendArea(void* &addr, size_t &maxSize);

  /*!
   * \brief Get receive area.
   *
   * This method queries the address and size of the receive area. Receive
   * area is used to get data from the other side.
   *
   * \param addr filled with the address of the receive buffer,
   * \param maxSize filled with the size of the receive buffer.
   */
  void getReceiveArea(void* &addr, size_t &maxSize);

  /*!
   * \brief Send data.
   *
   * Send data to the other party. Eventually wait for communication
   * buffer to be written to underlying system buffers and/or for the
   * segment to be read by the other side (if not read yet).
   *
   * \param size size of the packet in send area (needed only for
   *  non-SHM communication).
   */
  void sendData(size_t size);

  /*!
   * \brief Receive data.
   *
   * If the packet is not ready, wait for packet to come. Data are then
   * stored in receive area. Receiver must be able to get data size from
   * the packet itself.
   */
  void receiveData();

  /*!
   * \brief Map an address in ProcServer onto offset (local mode).
   *
   * This method is used in local communication to map an absolute object
   * address in ProcServer address space (but in shared memory segment)
   * into an offset into shared memory segment.
   *
   * \param addr absolute address to map.
   *
   * \return offset associated with the address.
   */
  size_t getOffsetForAddress(void *addr);

  /*!
   * \brief Map an offset from ProcServer into address in kernel (local mode).
   *
   * This method is used in local communication to map an object offset
   * from ProcServer in shared memory segment into kernel address space
   * (actually in the same shared memory segment).
   *
   * \param offs offset to map.
   *
   * \return absolute address for the offset.
   */
  void *getAddressForOffset(size_t offs);

private:
  RTETask_ITaskClientConnection *m_pConnection;
  SAPDB_Byte                    *m_pBuffer;
  SAPDB_UInt4                    m_maxSendDataLength;
};

#endif  // __LVC_PROCSERVER_INTERFACE_INSTANCE
