/*!
 * \file    LVCSim_ProcServerInterface.cpp
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

#include "LVCSimulator/LVCSim_ProcServerInterface.hpp"
#include "DBProc/liveCache/LVCPS_LiveCacheSink.hpp"
#include "DBProc/liveCache/LVCPS_Dispatcher.hpp"

/*---------------------------------------------------------------------------*/

LVCSim_ProcServerBuffer::LVCSim_ProcServerBuffer()
{
  m_sem1 = LVCSim::SemCreate(0);
  m_sem2 = LVCSim::SemCreate(0);
}

/*---------------------------------------------------------------------------*/

LVCSim_ProcServerBuffer::~LVCSim_ProcServerBuffer()
{
  LVCSim::SemDestroy(m_sem1);
  LVCSim::SemDestroy(m_sem2);
}

/*---------------------------------------------------------------------------*/

LVCSim_ProcServerInterface::LVCSim_ProcServerInterface(SAPDBMem_IRawAllocator *alloc,
  const LVCSim_ProcServerChannel &channel)
  : LVC_ProcServerInterface(alloc), m_channel(channel)
{
}

/*---------------------------------------------------------------------------*/

LVCSim_ProcServerInterface::~LVCSim_ProcServerInterface()
{
}

/*---------------------------------------------------------------------------*/

void LVCSim_ProcServerInterface::getSendArea(void* &addr, size_t &maxSize)
{
  m_channel.getSendArea(addr, maxSize);
}

/*---------------------------------------------------------------------------*/

void LVCSim_ProcServerInterface::getSecondarySendArea(void* &addr, size_t &maxSize)
{
  // unused
  throw DbpError(e_dbproc_invalid_operation, "unused");
}

/*---------------------------------------------------------------------------*/

void LVCSim_ProcServerInterface::getReceiveArea(void* &addr, size_t &maxSize)
{
  m_channel.getReceiveArea(addr, maxSize);
}

/*---------------------------------------------------------------------------*/

void LVCSim_ProcServerInterface::sendData(size_t size)
{
  // signal other end
  m_channel.signalChannel();
}

/*---------------------------------------------------------------------------*/

void LVCSim_ProcServerInterface::receiveData()
{
  // wait for other end
  m_channel.waitChannel();
}

/*---------------------------------------------------------------------------*/

size_t LVCSim_ProcServerInterface::getOffsetForAddress(void *addr)
{
  // identity mapping
  return reinterpret_cast<size_t>(addr);
}

/*---------------------------------------------------------------------------*/

void *LVCSim_ProcServerInterface::getAddressForOffset(size_t offs)
{
  // identity mapping
  return reinterpret_cast<void*>(offs);
}

/*---------------------------------------------------------------------------*/

unsigned long LVCSim_ProcServerInterface::thread(void *param)
{
  LVCSim_ProcServerThreadParam *params = (LVCSim_ProcServerThreadParam*) param;
  LVCPS_LiveCacheSink sink(params->m_iface, params->m_pCancel);

  // register sink in TLS in order to satisfy omsGetOmsHandle()
  LVCSim::SetCurrentSink(&sink);

  // start ProcServer loop
  if (LVCPS_Dispatcher::Instance().dispProcServerLoop(&sink, params->m_iface)) {
    LVCSim::SetCurrentSink(NULL);
    return 0;
  } else {
    LVCSim::SetCurrentSink(NULL);
    return 1;
  }
}
