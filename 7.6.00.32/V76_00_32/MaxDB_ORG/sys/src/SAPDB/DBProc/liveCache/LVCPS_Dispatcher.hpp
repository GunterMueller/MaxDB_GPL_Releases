/*!
 * \file    LVCPS_Dispatcher.hpp
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

#ifndef __LVCPS_DISPATCHER_HPP
#define __LVCPS_DISPATCHER_HPP

#include "liveCache/LVC_Dispatcher.hpp"

class LVC_ProcServerInterface;
class LVC_ProcServerStart;

/*!
 * \brief OMS stored procedure dispatcher for ProcServer case (client side).
 *
 * This is the class for dispatching stored procedure requests within the
 * ProcServer.
 *
 * \see \ref omscallinterface, \ref procserver
 *
 * \ingroup oms procserver
 */
class LVCPS_Dispatcher : public LVC_Dispatcher {
public:

  LVCPS_Dispatcher(SAPDBMem_IRawAllocator &alloc) :
    LVC_Dispatcher(alloc)
  {
  }

  /// Main loop for ProcServer. Loop until termination request received.
  static bool dispProcServerLoop(IliveCacheSink *sink, LVC_ProcServerInterface *psinterface);

  /// Dispatch COM routine call.
  void dispProcServerDispatch(IliveCacheSink *sink,
    LVC_ProcServerInterface *psinterface, LVC_ProcServerStart *start);

  /// Get instance of ProcServer dispatcher.
  static LVCPS_Dispatcher &Instance();

protected:
  // overrides:
  virtual void dispGetClock(tsp00_Int4 &sec, tsp00_Int4 &msec);
  virtual void dispAbort();
#ifdef WIN32
  virtual void dispDumpWin32Exception(struct _EXCEPTION_POINTERS *einfo);
#endif
  virtual int  dispEvalException(HRESULT &hresult, void *userData);
  virtual void dispReleaseInstanceLL(LVC_DispatcherHashEntry *currEntry, tsp00_Addr sessionContext);
  virtual void dispDestroySink(IliveCacheSink *sink);
};

#endif  // __LVCPS_DISPATCHER_HPP
