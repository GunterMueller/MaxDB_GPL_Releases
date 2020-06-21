/*!
 * \file    LVC_ProcServerExtender.cpp
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

#include "liveCache/LVC_ProcServerExtender.hpp"
#include "hsp77.h"

/*---------------------------------------------------------------------------*/

LVC_ProcServerExtender::LVC_ProcServerExtender(SAPDBMem_IRawAllocator *pAlloc,
                                               IliveCacheSink         *pSink)
: m_procServerExecutor(pSink, NULL)
, m_procServerInterface(pAlloc)
, m_pAlloc(pAlloc)
, m_pIUnknown(pSink)
{
  m_procServerExecutor.executorAdviseInterface(&m_procServerInterface);
}

/*---------------------------------------------------------------------------*/

void LVC_ProcServerExtender::StartInUDEServer(
  tsp00_Uint                    paramcnt,
  tak_dbproc_stack_param_debug *pParamStack,
  const char                   *pLibname, 
  const tsp00_C32              &md5, 
  const CLSID                  &clsid, 
  const IID                    &iid,
  tsp00_Int4                    dispid,
  tsp00_KnlIdentifier          &methodname, 
  tsp00_KnlIdentifier          &methodnameAscii,
  short                        &err)
{
  // let the COM routine execute in the executor
  tsp00_KnlIdentifier knlName;
  knlName.c2p(methodname);
  m_procServerExecutor.executorExecute(paramcnt, pParamStack, pLibname, md5, clsid, iid,
    dispid, methodname, methodnameAscii, err);
  if (err != 0) {
    // Uncaught DbpError in COM routine
    throw DbpError(err);
  }
}

