/*!
 * \file    LVC_ProcServerExecutor.cpp
 * \author  MarkusSi
 * \brief   Executor of requests received from LVC_ProcServerInterface.
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

#include "liveCache/LVC_ProcServerExecutor.hpp"
#include "DBProc/liveCache/LVCPS_PacketConsoleMsg.hpp"
#include "DBProc/liveCache/LVCPS_PacketSingleObj.hpp"
#include "DBProc/liveCache/LVCPS_PacketContainer.hpp"
#include "DBProc/liveCache/LVCPS_PacketVersion.hpp"
#include "DBProc/liveCache/LVCPS_PacketSchema.hpp"
#include "DBProc/liveCache/LVCPS_PacketRegion.hpp"
#include "DBProc/liveCache/LVCPS_PacketSubtrans.hpp"
#include "DBProc/liveCache/LVCPS_PacketConsistentView.hpp"
#include "DBProc/liveCache/LVCPS_PacketMassObj.hpp"
#include "DBProc/liveCache/LVCPS_PacketIterator.hpp"
#include "DBProc/liveCache/LVCPS_PacketSequence.hpp"
#include "DBProc/liveCache/LVCPS_PacketFile.hpp"
#include "DBProc/liveCache/LVCPS_PacketABAPStream.hpp"
#include "DBProc/liveCache/LVCPS_PacketTasking.hpp"
#include "DBProc/liveCache/LVCPS_PacketSQL.hpp"
#include "DBProc/liveCache/LVCPS_PacketVersionDict.hpp"
#include "vak001.h"

#define HRESULT_OVERFLOW  1

/*----------------------------------------------------------------------------*/

void* LVC_ProcServerExecutor::m_registry[MAX_CODE] = {NULL};
bool LVC_ProcServerExecutor::m_packetsRegistered = false;

/*----------------------------------------------------------------------------*/

HRESULT LVC_ProcServerExecutor::executorExecute(size_t nargs, void *stack, 
  const char *pLibname, const tsp00_C32 &md5, const CLSID &clsid, const IID &iid,
  tsp00_Int4 dispid, tsp00_KnlIdentifier &methodname, tsp00_KnlIdentifier &methodnameAscii, 
  short &err)
{
  void *recvAddr, *sendAddr;
  size_t rmaxSize, smaxSize;

  // get send area where to write reply packets
  m_interf->getSendArea(sendAddr, smaxSize);

  // write the packet for COM call to the send buffer
  LVC_ProcServerStart *start = reinterpret_cast<LVC_ProcServerStart*>(sendAddr);
  
  start->m_method       = methodname;
  start->m_method_ascii = methodnameAscii;
  start->m_libmd5       = md5;
  start->m_clsid        = clsid;
  start->m_iid          = iid;
  start->m_dispid       = dispid;
  start->m_paramcnt     = nargs;

  size_t sz = sizeof(LVC_ProcServerStart);
  if (nargs) {
    size_t parsize = sizeof(tak_dbproc_stack_param_debug) * nargs;
    if (sz + parsize > smaxSize) {
      // return error - overflow
      return HRESULT_OVERFLOW;
    }
    memcpy(start + 1, stack, parsize);
    sz += parsize;
  }

  // now copy library name
  {
    start->m_libnameofs = sz;
    size_t libsize = strlen(pLibname) + 1;
    if (sz + libsize > smaxSize) {
      // return error - overflow
      return HRESULT_OVERFLOW;
    }
    memcpy(reinterpret_cast<char*>(start) + sz, pLibname, libsize);
    sz += libsize;
  }

  // now copy all pointer IN parameters into the packet
  start->m_paramofs = sz;
  tak_dbproc_stack_param_debug *pp = reinterpret_cast<tak_dbproc_stack_param_debug*>(stack);
  unsigned int i;
  for (i = 0; i < nargs; ++i) {
    if (pp->dbg_is_ptr && pp->dbg_inout != sp1io_output) {
      // get properly aligned offset
      size_t paramalign = pp->dbg_length;
      if (paramalign > 4)
        paramalign = 8;
      else if (paramalign > 2)
        paramalign = 4;
      else if (paramalign > 1)
        paramalign = 2;
      sz = (sz + paramalign - 1) & ~(paramalign - 1);
      // copy the parameter
      if (sz + pp->dbg_length > smaxSize) {
        // return error - overflow
        return HRESULT_OVERFLOW;
      }
      if (pp->dbg_value.ptr() == NULL) {
        // ERROR in parameter, ignore here, it will be announced before starting COM routine
      } else {
        memcpy(reinterpret_cast<char*>(start) + sz, pp->dbg_value.ptr(), pp->dbg_length);
      }
      sz += pp->dbg_length;
    }
    ++pp;
  }
  start->m_size = sz;
  start->m_code = LVC_ProcServerStart::CODE;

  // send the packet to the ProcServer
  m_interf->send(sz);

  for (;;) {
    // wait for a packet to come from the ProcServer
    m_interf->receive();

    // get receive area where to look for packets and the packet
    m_interf->getReceiveArea(recvAddr, rmaxSize);
    LVC_ProcServerPacketHeader *packet =
      reinterpret_cast<LVC_ProcServerPacketHeader*>(recvAddr);

    if (packet->m_code == LVC_ProcServerReturn::CODE) {
      break;  // break only if terminate/finish packet
    }

    // process packets, until terminate packet comes

    if (packet->m_code < 0 || packet->m_code >= MAX_CODE) {
      // error - invalid code
      m_interf->sendError(e_dbproc_invalid_packet);
      continue;
    }

    // set up the VTable correctly
    void *vtable = m_registry[packet->m_code];
    if (!vtable) {
      // error - undefined code
      m_interf->sendError(e_dbproc_invalid_packet);
      continue;
    }
    *reinterpret_cast<void**>(packet) = vtable;

    // execute the packet action
    packet->execute(m_sink, m_interf);
  }

  const LVC_ProcServerReturn *finish =
    reinterpret_cast<const LVC_ProcServerReturn*>(recvAddr);
  err = finish->m_retcode;
  if (finish->m_retcode != 0) {
    // return error result code only
    return S_OK;
  }
  // read output from terminate packet
  sz = sizeof(LVC_ProcServerReturn);
  pp = reinterpret_cast<tak_dbproc_stack_param_debug*>(stack);
  for (i = 0; i < nargs; ++i) {
    if (pp->dbg_inout == sp1io_input) {
      ++pp;
      continue; // skip input parameters
    }
    // get properly aligned offset
    size_t paramalign = pp->dbg_length;
    if (paramalign > 4)
      paramalign = 8;
    else if (paramalign > 2)
      paramalign = 4;
    else if (paramalign > 1)
      paramalign = 2;
    sz = (sz + paramalign - 1) & ~(paramalign - 1);
    // copy the output parameter
    if (pp->dbg_value.ptr() == NULL) {
      // ERROR in parameter, ignore here, it was announced before starting COM routine
    } else {
      memcpy(pp->dbg_value.ptr(), reinterpret_cast<const char*>(finish) + sz, pp->dbg_length);
    }
    sz += pp->dbg_length;
    ++pp;
  }
  return S_OK;
}

/*----------------------------------------------------------------------------*/

HRESULT LVC_ProcServerExecutor::executorTerminate()
{
  void *sendAddr;
  size_t smaxSize;

  // get send area where to write reply packets
  m_interf->getSendArea(sendAddr, smaxSize);

  // write the packet for terminate request
  LVC_ProcServerTerminateReq *req = 
    reinterpret_cast<LVC_ProcServerTerminateReq*>(sendAddr);
  req->m_code = LVC_ProcServerTerminateReq::CODE;
  req->m_size = sizeof(LVC_ProcServerTerminateReq);
  m_interf->send(sizeof(LVC_ProcServerTerminateReq));

  // wait for answer (unchecked)
  m_interf->receive();
  return S_OK;
}

/*----------------------------------------------------------------------------*/

HRESULT LVC_ProcServerExecutor::executorTransEnd(short &err,bool)
{
  // TODO
  err = 0;
  return S_OK;
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerExecutor::registerPackets()
{
  LVC_ProcServerStart::registerSelf();             // CODE = 1
  LVC_ProcServerReturn::registerSelf();            //  = 2
  LVC_ProcServerTerminateReq::registerSelf();      //  = 3
  LVC_ProcServerTerminateRep::registerSelf();      //  = 4
  LVC_ProcServerSimpleRep::registerSelf();         //  = 5
  LVCPS_PacketConsoleMsg::registerSelf();          //  = 10

  LVCPS_PacketSingleObj::registerSelf();           //  = 100
  LVC_ProcServerSingleObjRep::registerSelf();      //  = 200
  LVCPS_PacketVersion::registerSelf();             //  = 101
  LVC_ProcServerVersionRep::registerSelf();        //  = 201
  LVCPS_PacketContainer::registerSelf();           //  = 102
  LVC_ProcServerContainerRep::registerSelf();      //  = 202
  LVCPS_PacketSchema::registerSelf();              //  = 103
  LVC_ProcServerSchemaRep::registerSelf();         //  = 203
  LVCPS_PacketRegion::registerSelf();              //  = 104
  LVCPS_PacketSubtrans::registerSelf();            //  = 105
  LVCPS_PacketConsistentView::registerSelf();      //  = 106
  LVC_ProcServerConsistentViewRep::registerSelf(); //  = 206
  LVCPS_PacketMassObj::registerSelf();             //  = 107
  LVC_ProcServerMassObjRep::registerSelf();        //  = 207
  LVCPS_PacketIterator::registerSelf();            //  = 108
  LVC_ProcServerIteratorRep::registerSelf();       //  = 208
  LVCPS_PacketSequence::registerSelf();            //  = 109
  LVC_ProcServerSequenceRep::registerSelf();       //  = 209
  LVCPS_PacketFile::registerSelf();                //  = 110
  LVC_ProcServerFileRep::registerSelf();           //  = 210
  LVCPS_PacketABAPStream::registerSelf();          //  = 111
  LVC_ProcServerABAPStreamRep::registerSelf();     //  = 211
  LVCPS_PacketTasking::registerSelf();             //  = 112
  LVCPS_PacketSQL::registerSelf();                 //  = 113
  LVCPS_PacketVersionDict::registerSelf();         //  = 114
  LVC_ProcServerVersionDictRep::registerSelf();    //  = 214

  LVC_ProcServerErrorStateRep::registerSelf();     //  = 299
}

/*----------------------------------------------------------------------------*/
