/*!
 * \file    LVC_KernelInterface.hpp
 * \author  ThomasA
 * \brief   Session-free kernel interface for use in liboms.
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

#ifndef LVC_KERNELINTERFACE_HPP
#define LVC_KERNELINTERFACE_HPP

#include "gsp00.h"

// unfortunately didn't find another way than including
// because could not do forward declaration of RTEMem_AllocatorInfo  
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"

struct  IliveCacheSink;
class   SAPDBMem_IRawAllocator;
class   OmsStackTraceCallbackInterface;
class   DbpErrorCallbackInterface;
class   LVC_LockRequest;
class   RTE_IInterface;
/*!
 * \brief LiveCache session-less kernel interface.
 *
 * This class defines the interface of methods, that can be called from the
 * LibOms. An Instance of this interface will made known to the LibOms 
 * directly after the lib has been loaded.
 */
class LVC_KernelInterface 
{
public :
  /*!-------------------------------------------------------------------------
  function:     Allocate
  description:  Allocates contiguous memory for 'count' bytes from LVCMem_Allocator.
  
  arguments:    count [in] number of requested bytes
                isUserRequest [in] \c true if request from application, \c false if 
                request from liboms.
    
  return value: pointer to memory chunk of null
  --------------------------------------------------------------------------*/
  
  virtual void* Allocate(SAPDB_ULong count, bool isUserRequest) = 0;

  /*!
   * \brief Allocate memory.
   *
   * Allocates contiguous memory for \c count bytes from LVCMem_Allocator.
   * This method doesn't set the DbpErrror if no memory can be allocated.
   *
   * \param count number of bytes requested.
   *
   * \return pointer to memory chunk of null.
   */
  
  virtual void* AllocateThrowsNothing(SAPDB_ULong count)= 0;

  /*!
   * \brief Outputs statistics about the memory usage of the LVCMem_Allocator.
   * 
   * \param BytesUsed [out] The number of bytes used by this allocater.
   * \param BytesControlled [out] The number of bytes controlled by this allocator. 
   *        The default implementation returns GetInvalidSize() for all values.
   */
  virtual void CalcAllocatorStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled) = 0;


  /*!
   * \brief Return time in seconds.
   *
   * Returns the number of seconds since system start.
   *
   * \return number of seconds since system start.
   */
  virtual tsp00_Int4 Clock() = 0;

   /*!
   * \brief crashes the kernel.
   *
   * writes msg into knldiag and crashes the kernel.
   */
  virtual void Crash(const char* msg) = 0;

  /*!
   * \brief Deallocate memory.
   *
   * Deallocates contiguous memory identified by \c p from LVCMem_Allocator.
   *
   * \param p memory to be deallocated.
   * \param isUsreRequest \c true if request from application, \c false if request from liboms.
   */
  virtual void Deallocate(void* p, bool isUserRequest) = 0;

  /*!
   * \brief Deregister an allocator.
   *
   * Deregisters an allocator in the kernel allocator registry.
   *
   * \param allocatorInfo a structure containing allocator info.
   */
  virtual void DeregisterAllocator(RTEMem_AllocatorInfo& allocatorInfo) = 0;

  /*!
   * \brief dumps a memory chunk
   *
   * \param p address of the chunk
   * \size  size of the chunk
   *
   * \return the value of the parameter HEAP_CHECK_LEVEL
   */
  virtual void DumpChunk (void* p, int size) = 0;

  /*!
   * \brief Outputs statistics about the number of Allocate and Deallocate calls.
   *
   * \param CountAlloc [out] The number of calls of method Allocate.
   * \param CountDealloc [out] The number of calls of method Deallocate.
   *
   */
  virtual void GetAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const = 0;

  /*!
   * \brief Returns the identifier associated with this allocator
   *        This function is called to identify an allocator instance. The derived
   *        class is responsible to implement this feature (but must not do so...)
   * \return const SAPDB_UTF8 *
   */
  virtual const SAPDB_UTF8* GetAllocatorIdentifier() const = 0;
  
  /*!
   * \brief Outputs statistics about the number of Allocate and Deallocate calls of
   *        the allocator to the underlying base allocator.
   *
   * \param CountAlloc [out] The number of calls of method Allocate of the base allocator.
   * \param CountDealloc [out] The number of calls of method Deallocate of the base.
   *
   */
  virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const = 0;

  /*!
   * \brief returns the value of the parameter HEAP_CHECK_LEVEL
   *
   * returns the value of the parameter HEAP_CHECK_LEVEL
   *
   * \return the value of the parameter HEAP_CHECK_LEVEL
   */
  virtual SAPDB_UInt4 GetHeapCheckLevel() = 0;

  /*!
   * \brief checks if the liveCache is a OneDB system
   *
   * tells whether liveCache is a OneDB system.
   *
   * \return true, if liveCache is a OneDB system, false otherwise
   */
  virtual bool IsOneDBSystem(short &rc) = 0;

  /*!
   * \brief Get kernel version.
   *
   * Fills a buffer with liveCache version information.
   *
   * \param pBuffer       address of the buffer to be filled,
   * \param bufferLength  the size of the buffer.
   *
   * \return pointer to the buffer, if successful, NULL otherwise.
   */

  virtual void* GetKnlVersion(void* pBuffer,int bufferLength) = 0;

  /*!
   * \brief Get lock request timeout.
   *
   * Yields the value of the parameter REQUEST_TIMEOUT.
   *
   * \return the request timeout value.
   */
  virtual tsp00_Int4 GetLockRequestTimeout() = 0;

  /*!
   * \brief Get OMS region count.
   *
   * Yields the value of the parameter _OMS_REGIONS.
   *
   * \return the count of OMS regions.
   */
  virtual int   GetOmsRegionCnt() = 0;

  /*!
   * \brief Get maximum usable area for communication packet.
   *
   * Yields the maximal usable area of the packet, i.e. the
   * number of bytes, that can be transferred via a packet
   * containing one data part.
   *
   * \return the number of bytes, that can be transferred via a packet
   *    containing one data part.
   */
  virtual tsp00_Int4 GetPacketMaxUsableArea() = 0;

  /*!
   * \brief Get sink pointer.
   *
   * Yields the address of the current kernel sink, i.e. the
   * kernel callback of the current session.
   *
   * \return the address of the kernel sink.
   */
  virtual IliveCacheSink* GetSinkPtr() = 0;

  /*!
   * \brief Get version threshold.
   *
   * Yields the value of the parameter OMS_VERS_THRESHOLD.
   * 
   * \return the OMS version threshold value.
   */
  virtual tsp00_Uint4 GetOmsVersionThreshold() = 0;
  
  /*!
   * \brief Check heap threshold.
   *
   * Returns \c true, if the heap threshold has been reached, i.e.
   * OMS_HEAP_THRESHOLD percent of the heap are in use.
   *
   * \return \c true, if threshold value has been reached, \c false otherwise
   */
  virtual bool  HeapThresholdReached() = 0;

  /*!
   * \brief Increment version unload.
   *
   * Increments the counter counting number of oms version unloads.
   */
  virtual void IncOmsVersionUnloadCounter() = 0;

  /*!
   * \brief check if OneDB system
   *
   * Tells whether liveCache is a OneDB system.
   *
   * \return true, if liveCache is a OneDB system, false otherwise
   */
  virtual bool IsUnicodeInstance() = 0;

  /*!
   * \brief Register allocator.
   *
   * Registers an allocator in the kernel allocator registry.
   *
   * \param allocatorInfo a structure containing allocator info.
   */
  virtual void RegisterAllocator(RTEMem_AllocatorInfo& allocatorInfo) = 0;

  /*!
   * \brief Signal a task.
   *
   * Wakes up or timeouts a task waiting for an OMS Lock.
   *
   * \param taskId the task identification of the waiting task,
   * \param timeout if \c true, waiting task is timed out
   */
  virtual void Signal(tsp00_TaskId taskId, bool timeout) = 0;

  /*!
   * \brief Signal OMS unload.
   *
   * The liboms calls this method, when it starts to become unloaded, i.e.
   * if it starts to destruct the globals. 
   */      
  virtual void SignalUnloadLiboms() = 0;

  /*!
   * \brief Bad alloc testing
   *
   * Tests, if bad alloc count down has reached 0. (for test
   * purposes only (test kernel)).
   */
  virtual void TestBadAlloc() = 0;

  /*!
   * \brief Attach sink in simulator.
   *
   * Attach a new sink to a handle in simulator to ease writing tests.
   *
   * \param handle  OMS handle to attach new sink to.
   */
  virtual void SimCtlAttachSink(class OmsHandle &handle) = 0;

  /*!
   * \brief Console error.
   *
   * Print a console error, used by allocator tracer.
   * 
   * \param msg message to print.
   */
  virtual void ConsoleError(const char *msg) = 0;

  /*!
   * \brief Console message.
   *
   * Print a console message, used in test programs in simulator.
   * 
   * \param msg message to print.
   */
  virtual void ConsoleMessage(const char *msg) = 0;

  /*!
   * \brief Allocate memory.
   *
   * Allocates contiguous memory for \c count bytes from LVCMem_Allocator for
   * use by array new[].
   *
   * \param count number of bytes requested.
   * \param isUserRequest \c true if request from application, \c false if request from liboms.
   *
   * \return pointer to memory chunk.
   */
  virtual void* AllocateArray(SAPDB_ULong count, bool isUserRequest) = 0;

  /*!
   * \brief Deallocate memory.
   *
   * Deallocates contiguous memory identified by \c p from LVCMem_Allocator
   * for use by array delete[].
   *
   * \param p memory to be deallocated.
   * \param isUserRequest \c true if request from application, \c false if request from liboms.
   */
  virtual void DeallocateArray(void* p, bool isUserRequest) = 0;

  /// Dump stack trace to knldiag (debugging function).
  virtual void DumpStackTrace() = 0;

  /// Save stack trace into provided buffer.
  virtual unsigned int SaveStackTrace(void **buffer, unsigned int levels) = 0;

  /// Evaluate stack trace by calling callback for each level.
  virtual void EvalStackTrace(void **buffer, unsigned int levels,
    OmsStackTraceCallbackInterface *cb) = 0;

  /// Get time in microseconds.
  virtual SAPDB_UInt8 GetMicroTime() = 0;

  /// Get file information.
  virtual bool StatFile(
      const char  *fn,
      SAPDB_UInt8 &ctime,
      SAPDB_UInt8 &fsize) = 0;

  /// Format file time into ASCII.
  virtual void FormatTime(
      char        *buffer,
      size_t      bufsize,
      SAPDB_UInt8 time) = 0;

  /// Lock request, return error code.
  virtual short LockRequest(LVC_LockRequest &lockReq) = 0;

  /// Register area in global directory
  virtual void RegisterArea(SAPDB_Int4 areaId, const char *name) = 0;

  /// Use readerWriterLocks or CriticalSections for synchonization
  /*! Use readerWriterLocks or CriticalSections for synchonization
  **  of the version dictionary 
  **
  **  \since PTS 1124170)
  */
  virtual bool UseReaderWriterLocks() = 0;

  /*!
   * \brief Get kernel runtime interface
   *
   * Returns a pointer to the liveCache kernel runtime interface
   *
   * \return pointer to the liveCache kernel runtime interface
   */
  virtual RTE_IInterface &GetKernelRunTimeInterface() = 0;

  /*!
   * \brief Set error callback interface
   *
   * Set the error callback interface pointer in the kernel
   *
   * \since PTS 1133630
   */
  virtual void SetErrorCallback(DbpErrorCallbackInterface *cb) = 0;
   /*!
   * \brief writes into the kernel vtrace
   *
   * \param msg the message to be written into the vtrace
   */
  virtual void VTrace(const char* msg) = 0;
};

#endif
