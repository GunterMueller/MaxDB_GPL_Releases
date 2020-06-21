/*!
 * \file    LVCPS_KernelInterface.hpp
 * \author  MarkusSi
 * \brief   Interface for special functions in ProcServer, like memory allocation.
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

#ifndef __LVCPS_KERNELINTERFACE_HPP
#define __LVCPS_KERNELINTERFACE_HPP

#include "liveCache/LVC_KernelInterface.hpp"

class LVC_ProcServerInterface;

/*!
 * \brief ProcServer kernel interface.
 *
 * \see \ref omsprocserver, LVC_KernelInterface for member description.
 *
 * \ingroup oms procserver
 */
class LVCPS_KernelInterface : public LVC_KernelInterface
{
public:
  LVCPS_KernelInterface(LVC_ProcServerInterface *pConn,
                        IliveCacheSink          *pSink) 
  : m_pConn(pConn)
  , m_pSink(pSink)
  {
  }

  virtual void* Allocate(SAPDB_ULong count, bool isUserRequest);

  virtual void* AllocateThrowsNothing(SAPDB_ULong count);

  virtual void CalcAllocatorStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled);

  virtual tsp00_Int4 Clock();

  virtual void Deallocate(void* p, bool isUserRequest);

  virtual void DeregisterAllocator(RTEMem_AllocatorInfo& allocatorInfo);

  virtual void GetAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const; 
  
  virtual const SAPDB_UTF8* GetAllocatorIdentifier() const;

  virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const;

  virtual void* GetKnlVersion(void* pBuffer,int bufferLength);

  virtual tsp00_Int4 GetLockRequestTimeout();

  virtual int   GetOmsRegionCnt();

  virtual tsp00_Int4 GetPacketMaxUsableArea();

  virtual IliveCacheSink* GetSinkPtr();

  virtual tsp00_Uint4 GetOmsVersionThreshold();
  
  virtual bool  HeapThresholdReached();

  virtual void IncOmsVersionUnloadCounter();

  virtual bool IsUnicodeInstance();

  virtual void RegisterAllocator(RTEMem_AllocatorInfo& allocatorInfo);

  virtual void Signal(tsp00_TaskId taskId, bool timeout);

  virtual void SignalUnloadLiboms();

  virtual void TestBadAlloc();

  virtual void SimCtlAttachSink(class OmsHandle &handle);

  virtual void ConsoleMessage(const char *msg);

  virtual void* AllocateArray(SAPDB_ULong count, bool isUserRequest);

  virtual void DeallocateArray(void* p, bool isUserRequest);

  virtual void DumpStackTrace();

  virtual unsigned int SaveStackTrace(void **buffer, unsigned int levels);

  virtual void EvalStackTrace(void **buffer, 
                              unsigned int levels,
                              OmsStackTraceCallbackInterface *cb);

  virtual SAPDB_UInt8 GetMicroTime();

  virtual bool StatFile(const char  *fn,
                        SAPDB_UInt8 &ctime,
                        SAPDB_UInt8 &fsize);

  virtual void FormatTime(char        *buffer,
                          size_t      bufsize,
                          SAPDB_UInt8 time);

  virtual short LockRequest(LVC_LockRequest &lockReq);

  virtual bool UseReaderWriterLocks();

  virtual void SetErrorCallback(DbpErrorCallbackInterface *cb);

private:
  LVC_ProcServerInterface *m_pConn;  ///< Connection with kernel
  IliveCacheSink          *m_pSink;
};

#endif  // __LVCPS_KERNELINTERFACE_HPP