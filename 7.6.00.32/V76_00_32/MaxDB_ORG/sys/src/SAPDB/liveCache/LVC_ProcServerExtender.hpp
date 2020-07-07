/*!
 * \file    LVC_ProcServerExtender.hpp
 * \author  MarkusSi
 * \brief   Extender for processing calls from out-of-process procedures in liveCache kernel.
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

#ifndef __LVC_PROCSERVER_EXTENDER
#define __LVC_PROCSERVER_EXTENDER

#include "liveCache/LVC_ProcServerInterfaceInstance.hpp"
#include "liveCache/LVC_ProcServerExecutor.hpp"
#include "liveCache/LVC_Dispatcher.hpp"
#include "Oms/OMS_DbpBase.hpp"
#include "geo38.h"

#define LC_DBPROC_PROTO
#include "Oms/OMS_DbpGlobals.hpp"

class SAPDBMem_IRawAllocator;

/*!
 * \brief Extender for processing calls from out-of-process procedures.
 *
 * This class handles the requests from an out-of-process procedure and
 * processes them locally in kernel. The results are sent back to the
 * out-of-process procedure.
 *
 * It also handles startup of new stored procedures by sending the call
 * to the ProcServer.
 *
 * There is one instance of this class per one ProcServer and one session.
 *
 * \see \ref omsprocserver
 *
 * \ingroup oms procserver
 */
class LVC_ProcServerExtender 
{
public:
  /*!
   * \brief Construct a new instance.
   *
   * \param alloc allocator for memory allocation,
   * \param sink local sink to use.
   */
  LVC_ProcServerExtender(SAPDBMem_IRawAllocator *alloc, IliveCacheSink *sink);

  /*!
   * \brief Process one request packet.
   *
   * This method reads the packet from communication area and processes
   * the command contained in the packet. Then, it sends a reply to the
   * ProcServer.
   */
  void processPacket();

  void StartInUDEServer(
    tsp00_Uint                    paramcnt,
    tak_dbproc_stack_param_debug *pParamStack,
    const char                   *pLibname, 
    const tsp00_C32              &md5, 
    const CLSID                  &clsid, 
    const IID                    &iid,
    tsp00_Int4                    dispid,
    tsp00_KnlIdentifier          &methodname, 
    tsp00_KnlIdentifier          &methodnameAscii,
    short                        &err);


private:  
  SAPDBMem_IRawAllocator  *m_pAlloc; ///< Allocator for memory allocation.

  LVC_ProcServerExecutor           m_procServerExecutor;
  LVC_ProcServerInterfaceInstance  m_procServerInterface;

  IUnknown  *m_pIUnknown;  ///< Reference to the sink.
};

#endif  // __LVC_PROCSERVER_EXTENDER

