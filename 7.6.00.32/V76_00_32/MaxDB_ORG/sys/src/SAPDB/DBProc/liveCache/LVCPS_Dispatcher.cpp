/*!
 * \file    LVCPS_Dispatcher.cpp
 * \author  IvanS
 * \brief   OMS Stored Procedure dispatcher for ProcServer client side.
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

#include "DBProc/liveCache/LVCPS_Dispatcher.hpp"
#include "liveCache/LVC_ProcServerInterface.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp"
#include "Oms/OMS_Defines.h"

/*---------------------------------------------------------------------------*/

static LVCPS_Dispatcher dispatcherInstance(SAPDBMem_DefaultRawAllocator::Instance());

/*---------------------------------------------------------------------------*/

/*!
 * Main loop for ProcServer. Loop until termination request received.
 *
 * \param sink liveCache sink to use for the COM objects,
 * \param psinterface communication interface to use.
 *
 * \return \c true, if successfully executed/clean termination,
 *  \c false if erroneous packet received.
 */
bool LVCPS_Dispatcher::dispProcServerLoop(IliveCacheSink *sink,
  LVC_ProcServerInterface *psinterface)
{
  for (;;) {
    psinterface->receive();
    // this must be the call of a COM routine or terminate request
    void *addr;
    size_t size;
    psinterface->getReceiveArea(addr, size);
    LVC_ProcServerStart *start = reinterpret_cast<LVC_ProcServerStart*>(addr);
    if (start->m_code != LVC_ProcServerStart::CODE) {
      // Either termination packet received, or error

      int code = start->m_code;

      // send termination reply anyway, in case the other side listens
      psinterface->getSendArea(addr, size);
      LVC_ProcServerTerminateRep *rep = 
        reinterpret_cast<LVC_ProcServerTerminateRep*>(addr);
      rep->m_code = LVC_ProcServerTerminateRep::CODE;
      rep->m_size = sizeof(LVC_ProcServerTerminateRep);

      // release registered COM objects
      Instance().dispReleaseInstance(sink, 1 /* TODO: session */, NULL);

      if (code == LVC_ProcServerTerminateReq::CODE) {
        // termination, send reply packet
        psinterface->send(sizeof(LVC_ProcServerTerminateRep));
        return true;
      } else {
        // error
        return false;
      }
    }
    // process COM routine
    Instance().dispProcServerDispatch(sink, psinterface, start);
  }
}

/*---------------------------------------------------------------------------*/

/*!
 * Dispatch COM routine call.
 *
 * \param sink liveCache sink to use for the COM objects,
 * \param psinterface communication interface to use,
 * \param start start packet.
 */
void LVCPS_Dispatcher::dispProcServerDispatch(IliveCacheSink *sink,
  LVC_ProcServerInterface *psinterface, LVC_ProcServerStart *start)
{
  // compute space needed for output-only parameters
  size_t ostart = start->m_size, oofs = ostart;
  tak_dbproc_stack_param_debug *pp = reinterpret_cast<tak_dbproc_stack_param_debug*>(start + 1);
  int i;
  for (i = 0; i < start->m_paramcnt; ++i) {
    if (pp->dbg_inout == sp1io_output) {
        // get properly aligned offset
        size_t paramalign = pp->dbg_length;
        if (paramalign > 4)
          paramalign = 8;
        else if (paramalign > 2)
          paramalign = 4;
        else if (paramalign > 1)
          paramalign = 2;
        oofs = (oofs + paramalign - 1) & ~(paramalign - 1);
        oofs += pp->dbg_length;
    }
    ++pp;
  }  

  // copy debug stack and parameters to some other place
  {
    void *p = getAllocator().Allocate(oofs);
    memcpy(p, start, start->m_size);
    start = reinterpret_cast<LVC_ProcServerStart*>(p);
  }

  // adjust debug stack pointers
  pp = reinterpret_cast<tak_dbproc_stack_param_debug*>(start + 1);
  size_t sz = start->m_paramofs;
  oofs = ostart;
  for (i = 0; i < start->m_paramcnt; ++i) {
    if (pp->dbg_is_ptr) {
      size_t paramalign = pp->dbg_length;
      if (paramalign > 4)
        paramalign = 8;
      else if (paramalign > 2)
        paramalign = 4;
      else if (paramalign > 1)
        paramalign = 2;
      if (pp->dbg_inout != sp1io_output) {
        // get properly aligned offset
        sz = (sz + paramalign - 1) & ~(paramalign - 1);
        // adjust the parameter address to point to copied space
        if (pp->dbg_value.ptr() == NULL) {
          // ERROR: ignore it here, it will be traced before actual dispatching
        } else {
          pp->dbg_value.ptr() = reinterpret_cast<tsp00_MoveObjPtr>(
            reinterpret_cast<char*>(start) + sz);
        }
        sz += pp->dbg_length;
      } else {
        // output-only parameter
        oofs = (oofs + paramalign - 1) & ~(paramalign - 1);
        if (pp->dbg_value.ptr() == NULL) {
          // ERROR: ignore it here, it will be traced before actual dispatching
        } else {
          pp->dbg_value.ptr() = reinterpret_cast<tsp00_MoveObjPtr>(
            reinterpret_cast<char*>(start) + oofs);
        }
        oofs += pp->dbg_length;
      }
    }
    ++pp;
  }
  
  // load appropriate library and query COM object, use from cache, if possible
  IUnknown *disp_addr = NULL;
  LPCLASSFACTORY factory;
  HRESULT res = dispGetDispAddr(start->m_clsid, start->m_iid, 1 /* TODO: session ID */,
    reinterpret_cast<tsp00_Addr&>(disp_addr), reinterpret_cast<tsp00_Addr&>(factory), NULL);
  if (res != RC_DISPADDR_OK) {
    if (res == RC_DISPADDR_FACTORY_FOUND) {
      // needs to create new item only
      if (dispCreateInstanceLL(&start->m_iid, factory, (void**) &disp_addr) != S_OK) {
        disp_addr = NULL;
      } else {
        dispComCache(&start->m_iid, disp_addr, 1 /* TODO: session ID */,
          LVC_DispatcherHashEntry::ENTRY_IID);
        IliveCacheSpec *pILC;
        HRESULT hr = disp_addr->QueryInterface ( IID_IliveCacheSpec, (void **) &pILC);
        if (SUCCEEDED (hr))
        {
          pILC->Advise ( (IUnknown *) sink);
          //pILC->sysMonitorInit (); /* !"§$ TODO */

          dispRegister(pILC, NULL);
          pILC->Release();
        } else {
          // TODO: error - cannot find special interface
        }
      }
    } else {
      // load COM object, not found at all
      tsp00_C8 packageId;
      SAPDB_MemFillNoCheck(&packageId[0], 0, sizeof(packageId));
      if (dispGetClassObject(reinterpret_cast<char*>(start) + start->m_libnameofs,
        packageId,
        start->m_clsid, start->m_libmd5, reinterpret_cast<tsp00_Addr&>(factory)) != S_OK) {
        // error
        disp_addr = NULL;
      } else {
        dispComCache(&start->m_clsid, factory, 1 /* TODO: session ID */,
          LVC_DispatcherHashEntry::ENTRY_CLSID);
        if (dispCreateInstanceLL(&start->m_iid, factory, (void**) &disp_addr) != S_OK) {
          disp_addr = NULL;
        } else {
          dispComCache(&start->m_iid, disp_addr, 1 /* TODO: session ID */,
            LVC_DispatcherHashEntry::ENTRY_IID);
        }
        IliveCacheSpec *pILC;
        HRESULT hr = disp_addr->QueryInterface ( IID_IliveCacheSpec, (void **) &pILC);
        if (SUCCEEDED (hr))
        {
          pILC->Advise ( (IUnknown *) sink);
          //pILC->sysMonitorInit (); /* !"§$ TODO */

          dispRegister(pILC, NULL);
          pILC->Release();
        } else {
          // TODO: error - cannot find special interface
        }
      }
    }
  }

  short error = 0;
  if (disp_addr) {
    // start COM routine
    pp = reinterpret_cast<tak_dbproc_stack_param_debug*>(start + 1);
    HRESULT hr = dispDispatch(sink, disp_addr, start->m_method,
      start->m_method_ascii, start->m_dispid, start->m_paramcnt,
      pp, false, 0, NULL, NULL);
    if (hr != S_OK) {
      error = e_com_object_crashed;
    }
  } else {
    // error handling
    error = e_dbproc_proc_not_found;
  }

  // create output packet with results
  void *addr;
  size_t maxSize;
  psinterface->getSendArea(addr, maxSize);
  LVC_ProcServerReturn *finish = reinterpret_cast<LVC_ProcServerReturn*>(addr);
  sz = sizeof(LVC_ProcServerReturn);
  finish->m_code = LVC_ProcServerReturn::CODE;
  finish->m_retcode = error;
  pp = reinterpret_cast<tak_dbproc_stack_param_debug*>(start + 1);
  for (i = 0; i < start->m_paramcnt; ++i) {
    if (pp->dbg_is_ptr && pp->dbg_inout != sp1io_input) {
      size_t paramalign = pp->dbg_length;
      if (paramalign > 4)
        paramalign = 8;
      else if (paramalign > 2)
        paramalign = 4;
      else if (paramalign > 1)
        paramalign = 2;
      // get properly aligned offset
      sz = (sz + paramalign - 1) & ~(paramalign - 1);
      // copy the data into packet
      if (sz + pp->dbg_length > maxSize) {
        // too big data, send error packet instead
        finish->m_retcode = 9999;
        sz = sizeof(LVC_ProcServerReturn);
        break;
      }
      if (pp->dbg_value.ptr() == NULL) {
        // ERROR
      } else {
        memcpy(reinterpret_cast<char*>(finish) + sz, pp->dbg_value.ptr(), pp->dbg_length);
      }
      sz += pp->dbg_length;
    }
    ++pp;
  }
  finish->m_size = sz;

  // send the reply packet to the other side
  psinterface->send(sz);

  // TODO: make sure exception handling is here to prevent potential memory leak
  getAllocator().Deallocate(start);
}

/*---------------------------------------------------------------------------*/

LVCPS_Dispatcher &LVCPS_Dispatcher::Instance()
{
  return dispatcherInstance;
}

/*---------------------------------------------------------------------------*/

void LVCPS_Dispatcher::dispGetClock(tsp00_Int4 &sec, tsp00_Int4 &msec)
{
  // TODO
  sec = msec = 0;
}

/*---------------------------------------------------------------------------*/

void LVCPS_Dispatcher::dispAbort()
{
  // TODO: stacktrace
  abort();
}

/*---------------------------------------------------------------------------*/
#ifdef WIN32
/*---------------------------------------------------------------------------*/

void LVCPS_Dispatcher::dispDumpWin32Exception(struct _EXCEPTION_POINTERS *einfo)
{
  // TODO
}

/*---------------------------------------------------------------------------*/
#endif
/*---------------------------------------------------------------------------*/

int LVCPS_Dispatcher::dispEvalException(HRESULT &hresult, void *userData)
{
  // TODO
  return 0;
}

/*---------------------------------------------------------------------------*/

void LVCPS_Dispatcher::dispReleaseInstanceLL(LVC_DispatcherHashEntry *currEntry,
                           tsp00_Addr sessionContext)
{
  tsp_dcom_dispatch_parms   disp_parms;
  IUnknown                  *lpIUnknown;
  HRESULT                   hr;
  IliveCacheSpec            *pILC;

  disp_parms.idispatch  = REINTERPRET_CAST(char *,(currEntry->getIUnknown()));
  disp_parms.ifactory   = REINTERPRET_CAST(char *,(currEntry->getIUnknown()));
  disp_parms.dispid     = 2;
  disp_parms.session_context = sessionContext;
  try {
    lpIUnknown = currEntry->getIUnknown();
    hr = lpIUnknown->QueryInterface ( IID_IliveCacheSpec, (void **) &pILC);
    if (SUCCEEDED (hr))
    {
      hr = pILC->Unadvise();
      if ( FAILED(hr)) {
        // TODO: error
        //MSGD ((ERR_DCOM_HRESULT, "dispReleaseInstanceLL:Unadvise", hr ));
      }
      pILC->Release();
    } else {
      // TODO: error
      //MSGD ((ERR_DCOM_HRESULT, "dispReleaseInstanceLL:QILC", hr ));
    }
  }
  catch (DbpError e) { 
    char    msgBuffer[132];
    sprintf(&msgBuffer[0],"dispReleaseInstanceLL:Release: DbpError:%d", e.dbpError() );
    // TODO: error
    //MSGD ((ERR_DCOM_DUMP_INFO,msgBuffer));
  }
}

/*---------------------------------------------------------------------------*/

void LVCPS_Dispatcher::dispDestroySink(IliveCacheSink *sink)
{
  // TODO: ignore, since sink is created on the stack
}

/*---------------------------------------------------------------------------*/
