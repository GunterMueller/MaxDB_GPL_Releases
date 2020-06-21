/*!
 * \file    LVC_ProcServerExecutor.hpp
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

#ifndef __LVC_PROCSERVEREXECUTOR
#define __LVC_PROCSERVEREXECUTOR

#include "liveCache/LVC_ProcServerInterface.hpp"

struct IliveCacheSink;

/*!
 * \brief ProcServer executor class.
 *
 * This class executes actions requested from the front-end.
 *
 * \ingroup procserver
 */
class LVC_ProcServerExecutor {
public:
  LVC_ProcServerExecutor(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
    : m_sink(sink), m_interf(interf)
  {
    if (!m_packetsRegistered) {
      // register packets
      // no sync, since initializing more than once is OK
      registerPackets();
      m_packetsRegistered = true;
    }
  }

  /*!
   * \brief Send a COM-routine call, process OMS requests and return result.
   *
   * This method sends a COM routine call request to the ProcServer. ProcServer
   * should then execute the COM routine.
   *
   * COM routine parameter size may not exceed buffer size minus some overhead.
   * Structure IN parameters will be copied in the buffer after the description
   * data, with proper alignment.
   *
   * LVCPS_Dispatcher will pick up this information, prepare parameter
   * stack locally and start appropriate COM routine.
   *
   * After making this call, the executor waits for OMS/SQL requests from the
   * COM object in ProcServer and executes them on behalf of it, returning
   * results back. The last received buffer from ProcServer will contain
   * return code and procedure OUT parameters. These will be read from the packet
   * and stored at appropriate places in debug stack.
   *
   * \param nargs count of arguments,
   * \param stack parameter stack description,
   * \param clsid COM object GUID,
   * \param iid COM object interface GUID,
   * \param methodname method name to call,
   * \param methodnameAscii method name to call (in ASCII),
   * \param err DbpError code, if one was thrown, 0 otherwise.
   */
  HRESULT executorExecute(size_t nargs, void *stack, const char *pLibname, const tsp00_C32 &md5, 
    const CLSID &clsid, const IID &iid, tsp00_Int4 dispid,
    tsp00_KnlIdentifier &methodname, tsp00_KnlIdentifier &methodnameAscii, short &err);

  /*!
   * \brief Send transaction end request.
   *
   * This method sends a transaction end request to the ProcServer. ProcServer
   * will call all registered COM objects in turn to do their transaction end
   * processing and to flush changed data into the underlying database.
   *
   * \param err DbpError code, if one was thrown while processing trans end,
   * \param commit \c true = commit, \c false = rollback.
   *
   * \return HRESULT of the operation.
   */
  HRESULT executorTransEnd(short &err, bool commit);

  /*!
   * \brief Send monitor data collection request.
   *
   * This method sends a monitor data collection request. ProcServer
   * will call all registered COM objects in turn to do their monitor
   * data collection.
   *
   * \param err DbpError code, if one was thrown while processing trans end.
   *
   * \return HRESULT of the operation.
   */
  HRESULT executorMonitor();

  /*!
   * \brief Send termination request.
   *
   * This method sends a termination request to the ProcServer. ProcServer
   * should then close itself, after sending a confirmation packet.
   *
   * \return HRESULT of the operation.
   */
  HRESULT executorTerminate();

private:
  friend class LVC_ProcServerExtender;
  friend class LVC_ProcServerInterface;

  enum {
    MAX_CODE  = 128 ///< Max. # of packet types.
  };

  /// Loop and process OMS calls, until termination packet received.
  void executorLoop();

  /// Register packet.
  static void registerPacket(int code, void *vtable)
  {
    if (code >= 0 && code < MAX_CODE)
      m_registry[code] = vtable;
  }

  /// Register all packet types.
  static void registerPackets();

  /// Advise communication interface to use.
  void executorAdviseInterface(LVC_ProcServerInterface	*interf)
  {
    m_interf = interf;
  }

  IliveCacheSink	          *m_sink;    ///< Underlying sink to use to execute requests.
  LVC_ProcServerInterface	  *m_interf;  ///< Communication interface to use to read/write data.

  static void*  m_registry[MAX_CODE];   ///< VTable registry for packet types.
  static bool   m_packetsRegistered;    ///< \c true, if packet types already registered.
};

#endif	// __LVC_PROCSERVEREXECUTOR
