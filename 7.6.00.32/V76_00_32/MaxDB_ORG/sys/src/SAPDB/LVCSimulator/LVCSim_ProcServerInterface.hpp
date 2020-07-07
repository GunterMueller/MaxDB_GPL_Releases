/*!
 * \file    LVCSim_ProcServerInterface.hpp
 * \author  IvanS
 * \brief   Simulator ProcServer interface.
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

#ifndef __LVCSIM_PROCSERVERINTERFACE_HPP
#define __LVCSIM_PROCSERVERINTERFACE_HPP

#include "LVCSimulator/LVCSim_Client.hpp"
#include "liveCache/LVC_ProcServerInterface.hpp"

class LVCSim_ProcServerChannel {
public:
  LVCSim_ProcServerChannel(char *rbuf, char *wbuf, size_t size,
    LVCSim_Semaphore *rsem, LVCSim_Semaphore *wsem)
    : m_readBuf(rbuf), m_writeBuf(wbuf), m_bufSize(size),
      m_rsem(rsem), m_wsem(wsem)
  {
  }

  void getSendArea(void* &addr, size_t &maxSize)
  {
    addr = m_writeBuf;
    maxSize = m_bufSize;
  }

  void getReceiveArea(void* &addr, size_t &maxSize)
  {
    addr = m_readBuf;
    maxSize = m_bufSize;
  }

  /// Signal other end
  void signalChannel()
  {
    LVCSim::SemSignal(m_wsem);
  }

  /// Wait for other end.
  void waitChannel()
  {
    LVCSim::SemWait(m_rsem);
  }

private:
  char              *m_readBuf;   ///< Read buffer.
  char              *m_writeBuf;  ///< Write buffer.
  size_t            m_bufSize;    ///< Size of the buffer.
  LVCSim_Semaphore  *m_rsem;      ///< Semaphore for channel read operation synchronization.
  LVCSim_Semaphore  *m_wsem;      ///< Semaphore for channel write operation synchronization.
};

class LVCSim_ProcServerBuffer {
public:
  enum {
    BUF_SIZE = 32768 ///< Send/receive buffer size.
  };

  LVCSim_ProcServerBuffer();
  ~LVCSim_ProcServerBuffer();

  /// Get channel from buf1->buf2.
  LVCSim_ProcServerChannel getChannel1()
  {
    return LVCSim_ProcServerChannel(m_buf1, m_buf2, BUF_SIZE, m_sem1, m_sem2);
  }

  /// Get channel from buf2->buf1.
  LVCSim_ProcServerChannel getChannel2()
  {
    return LVCSim_ProcServerChannel(m_buf2, m_buf1, BUF_SIZE, m_sem2, m_sem1);
  }
private:
  char m_buf1[BUF_SIZE];  ///< First buffer.
  char m_buf2[BUF_SIZE];  ///< Second buffer.

  LVCSim_Semaphore  *m_sem1;      ///< Semaphore for buf1->buf2 synchronization.
  LVCSim_Semaphore  *m_sem2;      ///< Semaphore for buf2->buf1 synchronization.
};

class LVCSim_ProcServerThreadParam {
public:
  LVCSim_ProcServerThreadParam(LVC_ProcServerInterface *i, pasbool *c)
    : m_iface(i), m_pCancel(c) {}

  LVC_ProcServerInterface  *m_iface;   ///< ProcServer interface.
  pasbool                  *m_pCancel; ///< Cancel byte.
};

class LVCSim_ProcServerInterface : public LVC_ProcServerInterface {
public:
  LVCSim_ProcServerInterface(SAPDBMem_IRawAllocator *alloc,
    const LVCSim_ProcServerChannel &channel);
  ~LVCSim_ProcServerInterface();

  // Inherited from LVC_ProcServerInterface:
  virtual void getSendArea(void* &addr, size_t &maxSize);
  virtual void getSecondarySendArea(void* &addr, size_t &maxSize);
  virtual void getReceiveArea(void* &addr, size_t &maxSize);
  virtual void sendData(size_t size);
  virtual void receiveData();
  virtual size_t getOffsetForAddress(void *addr);
  virtual void *getAddressForOffset(size_t offs);

  /// Main thread function for procserver thread
  static unsigned long thread(void *param);

private:
  LVCSim_ProcServerChannel  m_channel;  ///< Data channel.
};

#endif  // __LVCSIM_PROCSERVERINTERFACE_HPP
