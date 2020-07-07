/*!
 * \file    LVC_InProcDispatcher.hpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   OMS Stored Procedure dispatcher for kernel.
 *
 * \todo Remove ak341GetDebugContext -> LVC_InProcDispatcher::dispGetSink
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

#ifndef __LVC_INPROCDISPATCHER_HPP
#define __LVC_INPROCDISPATCHER_HPP

#include "liveCache/LVC_Dispatcher.hpp"
#include "ggg91.h"
#include "heo52.h"  // vclock

class LVC_ProcServerExtender;

/*!
 * \brief OMS stored procedure dispatcher for inproc case.
 *
 * This is the class for dispatching stored procedure requests within the
 * kernel.
 *
 * \see \ref omscallinterface
 *
 * \ingroup oms
 */
class LVC_InProcDispatcher : public LVC_Dispatcher {
public:

  /// Get instance of inproc dispatcher.
  static LVC_InProcDispatcher  &Instance();

  /*!
   * \brief Create liveCache sink for session.
   *
   * \param addr liveCache sink address (where to put it),
   * \param size size of reserved address space,
   * \param acvaddr address of session data structure.
   */
  void dispCreateSink(void *addr, size_t size, void *acvaddr);

  /*!
   * \brief Release liveCache sink.
   *
   * Calls LVC_Dispatcher::dispReleaseInstance().
   *
   * \param params session-specific parameters.
   */
  void dispReleaseInstanceWrapper(tsp_dcom_release_instance_parms *parms);

  /*!
   * \brief Release all COM object instances.
   */
  void dispReleaseAllWrapper();

  /*!
   * \brief Open monitors.
   *
   * \param parms parameters.
   */
  void dispMonitorOpenWrapper(tsp_dcom_monitor_parms *parms);

  /*!
   * \brief Next monitors.
   *
   * \param parms parameters.
   */
  void dispMonitorNextWrapper(tsp_dcom_monitor_parms *parms);

  /*!
   * \brief Close monitors.
   *
   * \param parms parameters.
   */
  void dispMonitorCloseWrapper(tsp_dcom_monitor_parms *parms);

  /*!
   * \brief Get parameter info.
   *
   * \param params session-specific parameters.
   */
  void dispGetParamInfoWrapper(tsp_dcom_paraminfo_parms *params);

  /*!
   * \brief Get COM object class factory.
   *
   * \param params factory creation parameters.
   */
  void dispGetClassObjectWrapper(tsp_dcom_co_create_instance_parms *parms);

  /*!
   * \brief Dispatch a call to a DB procedure.
   *
   * \param params session-specific parameters.
   */
  void dispDispatchDBProcWrapper(tsp_dcom_dispatch_parms *params);

  /*!
   * \brief Dispatch a transaction end call.
   *
   * \param params session-specific parameters.
   */
  void dispTransEndWrapper(tsp_dcom_dispatch_parms *params);

  /*!
   * \brief Call invalidate.
   *
   * \param parms parameters.
   */
  void dispInvalidateWrapper(tsp_dcom_co_create_instance_parms *parms);

  /*!
   * \brief Get dispatcher address.
   *
   * \param params parameters.
   */
  void dispGetDispAddrWrapper(tsp_dcom_co_create_instance_parms *params);

  /*!
   * \brief Create a COM object instance.
   *
   * \param params creation parameters.
   */
  void dispCreateInstance(tsp_dcom_co_create_instance_parms *parms);

  /// Initialize the COM hash
  static bool dispInitComCacheStatic();

protected:
  virtual void dispSetupRoutine(IliveCacheSink *sink, const tsp00_KnlIdentifier &methodName,
    const tsp00_KnlIdentifier &methodNameAscii, void *setupParams);
  virtual void dispCleanupRoutine(IliveCacheSink *sink, void *setupParams);
  virtual void dispGetClock(tsp00_Int4 &sec, tsp00_Int4 &msec);
  virtual void dispAbort();
#ifdef WIN32
  virtual void dispDumpWin32Exception(struct _EXCEPTION_POINTERS *einfo);
#endif
  virtual int dispEvalException(HRESULT &hresult, void *userData);
  virtual void dispReleaseInstanceLL(LVC_DispatcherHashEntry* currEntry, tsp00_Addr sessionContext);
  virtual void dispDestroySink(IliveCacheSink *sink);

  inline static void dispGetSink(tak_all_command_glob *acv,
    tgg91_SessionNo &session,
    tsp00_Addr      &sink_addr,
    tsp1_packet_ptr &packet)
  {
    session   = acv->a_transinf.tri_trans.trSessionId_gg00;
    sink_addr = REINTERPRET_CAST(tsp00_Addr, &acv->a_kernel_sink);
    packet    = acv->a_dbp_packet_ptr;
  }

private:
  /// Create new inproc dispatcher.
  LVC_InProcDispatcher(SAPDBMem_IRawAllocator &alloc) : LVC_Dispatcher(alloc)
  {
  }

  /// Return true, if it should start in UDE-Server.
  bool                    UseUDEServer(IliveCacheSink* pSink) const;
  void                    SetUDEServer(IliveCacheSink* pSink, bool inproc);
  LVC_ProcServerExtender* GetUDEServerExtender(IliveCacheSink* pSink) const;
  void                    SetUDEServerExtender(IliveCacheSink* pSink, LVC_ProcServerExtender* pExtender);


  /// Global instance.
  static LVC_InProcDispatcher *m_instance;

  /// Allocator for memory allocation.
  SAPDBMem_IRawAllocator  *m_alloc;
};

extern "C" 
bool LVC_InProcDispatcher_dispInitComCache(void);

extern "C"
void vdcom_init_sink(void *);

#endif  // __LVC_INPROCDISPATCHER_HPP
