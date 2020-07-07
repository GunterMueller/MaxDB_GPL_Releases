/*!***************************************************************************

  module      : LVCSim_KernelInterface.hpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : session free kernel interface implementation for simulator

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*****************************************************************************/


#ifndef LVCSIM_KERNELINTERFACE_HPP
#define LVCSIM_KERNELINTERFACE_HPP

#include "liveCache/LVC_KernelInterface.hpp"
#include "liveCache/LVC_LockRequest.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp"
#include "LVCSimulator/LVCSim_RWLockManager.hpp"

class OmsStackTraceCallbackInterface;

/*!*****************************************************************************

   class:        LVCSim_KernelInterface
   description:  This class defines the methods, that can be called from the LibOms.
                 An Instance of this interface will made known to the LibOms
                 directly after the lib has been loaded.

*******************************************************************************/

class LVCSim_KernelInterface : public LVC_KernelInterface 
{
private:
	static tsp00_Int4	startupClock;	// save startup time here for Clock()

public:

	LVCSim_KernelInterface();

  /*!-------------------------------------------------------------------------
  function:     Allocate
  description:  Allocates contiguous memory for 'count' bytes from LVCMem_Allocator.
  
  arguments:    count [in] number of requested bytes
                isUsrRequest [in] \c true if request from application, \c false if 
                request from liboms.
    
  return value: pointer to memory chunk
  --------------------------------------------------------------------------*/
  
  virtual void* Allocate(SAPDB_ULong count, bool isUserRequest);

  /*!-------------------------------------------------------------------------
  function:     AllocateThrowsNothing
  description:  Allocates contiguous memory for 'count' bytes from LVCMem_Allocator.
                This method doesn't throw a exception if no memory can be allocated.

  arguments:    count [in] number of requested bytes
    
  return value: pointer to memory chunk or null
  --------------------------------------------------------------------------*/
  
  virtual void* AllocateThrowsNothing(SAPDB_ULong count);

  /*!-------------------------------------------------------------------------
  function:     CalcAllocatorStatistics
  description:  Outputs statistics about the memory usage of the LVCMem_Allocator.
  
  arguments:    BytesUsed [out] The number of bytes used by this allocater.
                BytesControlled [out] The number of bytes controlled by this allocator. 
                The default implementation returns GetInvalidSize() for all values.
    
  return value: none
  --------------------------------------------------------------------------*/
  virtual void CalcAllocatorStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled);

	/*!-------------------------------------------------------------------------
	function:	  Clock
	description:  returns the number of seconds since system start

	arguments:	  none
	return value: number of seconds
	--------------------------------------------------------------------------*/

	virtual tsp00_Int4 Clock();

    /*!
     * \brief crashes the kernel.
     *
     * writes msg into knldiag and crashes the kernel.
     */
    
    virtual void Crash(const char* msg);

    /*!-------------------------------------------------------------------------
    function:     Deallocate
    description:  Deallocates contiguous memory identified by p from LVCMem_Allocator.
    
    arguments:    p [in] memory to be deallocated
                  isUsrRequest [in] \c true if request from application, \c false if 
                  request from liboms.
    return value: none
    --------------------------------------------------------------------------*/
    
    virtual void Deallocate(void* p, bool isUserRequest);

    /*!-------------------------------------------------------------------------
	  function:     DeregisterAllocator
	  description:  deregisters an allocator in the kernel allocator registry.  
  		   
	  arguments:    allocatorInfo [in] a structure containing allocator info.
	  return value: none
    --------------------------------------------------------------------------*/

    virtual void DeregisterAllocator(RTEMem_AllocatorInfo& allocatorInfo);

  /*!
   * \brief dumps a memory chunk
   *
   * \param p address of the chunk
   * \size  size of the chunk
   *
   * \return the value of the parameter HEAP_CHECK_LEVEL
   */
   virtual void DumpChunk (void* p, int size);

  /*!
   * \brief returns the value of the parameter HEAP_CHECK_LEVEL
   *
   * returns the value of the parameter HEAP_CHECK_LEVEL
   *
   * \return the value of the parameter HEAP_CHECK_LEVEL
   */
   virtual SAPDB_UInt4 GetHeapCheckLevel();

  /*!
   * \brief checks if the liveCache is a OneDB system
   *
   * tells whether liveCache is a OneDB system.
   *
   * \return true, if liveCache is a OneDB system, false otherwise
   */
   virtual bool IsOneDBSystem(short &rc);

  /*!-------------------------------------------------------------------------
	function:     GetAllocatorCallStatistics
	description:  Outputs statistics about the number of Allocate and Deallocate calls.
		   
	arguments:    CountAlloc [out] The number of calls of method Allocate.
                CountDealloc [out] The number of calls of method Deallocate.

	return value: none
   --------------------------------------------------------------------------*/
  virtual void GetAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const;

  /*!-------------------------------------------------------------------------
	function:     GetAllocatorIdentifier
	description:  Returns the identifier associated with this allocator
                This function is called to identify an allocator instance. The derived
                class is responsible to implement this feature (but must not do so...)

	return value: const SAPDB_UTF8 *
   --------------------------------------------------------------------------*/
  virtual const SAPDB_UTF8* GetAllocatorIdentifier() const;

  /*!-------------------------------------------------------------------------
	function:     GetBaseAllocatorCallStatistics
	description:  Outputs statistics about the number of Allocate and Deallocate calls of
                the allocator to the underlying base allocator.
		   
	arguments:    CountAlloc [out] The number of calls of method Allocate of the base allocator.
                CountDealloc [out] The number of calls of method Deallocate of the base.

	return value: none
   --------------------------------------------------------------------------*/
  virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const;

	/*!-------------------------------------------------------------------------
	function:	  GetKnlVersion
	description:  fills a buffer with liveCache version information 

	arguments:	  pBuffer [out] address of the buffer to be filled
				  bufferLength [in] the size of the buffer
	return value: pointer to the buffer, if successful, NULL otherwise
	--------------------------------------------------------------------------*/

	virtual void* GetKnlVersion(void* pBuffer,int bufferLength);

	/*!-------------------------------------------------------------------------
	function:	  GetLockRequestTimeout
	description:  yields the value of the parameter REQUEST_TIMEOUT 

	arguments:	  none
	return value: the request timeout value
	--------------------------------------------------------------------------*/

	virtual tsp00_Int4 GetLockRequestTimeout();

	/*!-------------------------------------------------------------------------
	function:	  GetOmsRegionCnt
	description:  yields the value of the parameter _OMS_REGIONS 

	arguments:	  none
	return value: the value of oms regions
	--------------------------------------------------------------------------*/

	virtual int   GetOmsRegionCnt();

	/*!-------------------------------------------------------------------------
	function:	  GetPacketMaxUsableArea
	description:  yields the maximal usable area of the packet, i.e. the
				  number of bytes, that can be transferred via a packet
				  containing one data part.

	arguments:	  none
	return value: the number of bytes, that can be transferred via a packet
				  containing one data part.
	--------------------------------------------------------------------------*/

	virtual tsp00_Int4 GetPacketMaxUsableArea();

	/*!-------------------------------------------------------------------------
	function:	  GetSinkPtr
	description:  yields the address of the current kernel sink, i.e. the
				  kernel callback of the current session.

	arguments:	  none
	return value: the address of the kernel sink
	--------------------------------------------------------------------------*/

	virtual IliveCacheSink* GetSinkPtr();

	/*!-------------------------------------------------------------------------
	function:	  GetOmsVersionThreshold
	description:  yields the value of the parameter OMS_VERS_THRESHOLD 

	arguments:	  none
	return value: the oms version threshold value
	--------------------------------------------------------------------------*/

	virtual tsp00_Uint4 GetOmsVersionThreshold();

	/*!-------------------------------------------------------------------------
	function:	  HeapThresholdReached
	description:  returns true, if the heap threshold has been reached, i.e.
				  OMS_HEAP_THRESHOLD percent of the heap are in use 

	arguments:	  none
	return value: true, if threshold value has been reached, false otherwise
	--------------------------------------------------------------------------*/

	virtual bool  HeapThresholdReached();


    /*!-------------------------------------------------------------------------
    function:     IncOmsVersionUnloadCounter
    description:  increments the counter counting number of oms version unloads
   
    arguments:    none
     
    return value: none
    --------------------------------------------------------------------------*/
 
    virtual void IncOmsVersionUnloadCounter();

	/*!-----------------------------------------------------------------------
	 function:	   IsUnicodeInstance
	 description:  tells whether liveCache is runnung in UNICODE mode
	 arguments:    none
	 return value: true, if parameter _UNICODE='YES', otherwise false
	 -------------------------------------------------------------------------*/  

	virtual bool IsUnicodeInstance();

   /*!-------------------------------------------------------------------------
	function:     RegisterAllocator
	description:  registers an allocator in the kernel allocator registry.  
		   
	arguments:    allocatorInfo [in] a structure containing allocator info.
	return value: none
    --------------------------------------------------------------------------*/

    virtual void RegisterAllocator(RTEMem_AllocatorInfo& allocatorInfo);

	/*!-------------------------------------------------------------------------
	function:	  Signal
	description:  wakes up or timeouts a task waiting for an OMS Lock.	

	arguments:	  taskId [in] the task identification of the waiting task.
				  timeout[in] if true, waitung task is timed out
	return value: none
	--------------------------------------------------------------------------*/

	virtual void Signal(tsp00_TaskId taskId, bool timeout);

	/*!-------------------------------------------------------------------------
	function:	  SignalUnloadLiboms
	description:  the liboms calls this method, when it starts to become unloaded, i.e.
				  if it starts to destruct the globals. 

	arguments:	  none
	return value: none
	--------------------------------------------------------------------------*/

	virtual void SignalUnloadLiboms();

	/*!-------------------------------------------------------------------------
	function:	  TestBadAlloc
	description:  tests, if bad alloc count down has reached 0. (for test
				  purposes only (quickknl))

	arguments:	  none
	return value: none
	--------------------------------------------------------------------------*/

	virtual void TestBadAlloc();

	/*!-------------------------------------------------------------------------
	function:	  SimCtlAttachSink
	description:  attach a new sink to a handle in simulator to ease
				  writing tests

	arguments:	  handle [in]	OMS handle to attach new sink to
	return value: none
	--------------------------------------------------------------------------*/

	virtual void SimCtlAttachSink(class OmsHandle &handle);

  /*!
   * \brief Console error.
   *
   * Print a console error, used by allocator tracer.
   * 
   * \param msg message to print.
   */
   virtual void ConsoleError(const char *msg);


	  /*!-------------------------------------------------------------------------
	  function:	  ConsoleMessage
	  description:  print a console message, used in test programs in simulator

	  arguments:	  msg [in]		message to print
	  return value: none
	  --------------------------------------------------------------------------*/

	  virtual void ConsoleMessage(const char *msg);

    /*!-------------------------------------------------------------------------
	  function:     AllocateArray
	  description:  Allocates contiguous memory for 'count' bytes from LVCMem_Allocator
                  for use by array new[].

    arguments:    count [in] number of requested bytes
                  isUserRequest [in] \c true if request from application, \c false if 
                  request from liboms.
      
	  return value: pointer to memory chunk
    --------------------------------------------------------------------------*/
    
    virtual void* AllocateArray(SAPDB_ULong count, bool isUserRequest);

    /*!-------------------------------------------------------------------------
	  function:     DeallocateArray
	  description:  Deallocates contiguous memory identified by p from LVCMem_Allocator
                  for use by array delete[].

    arguments:    p [in] memory to be deallocated
                  isUserRequest [in] \c true if request from application, \c false if 
                  request from liboms.
      
	  return value: none
    --------------------------------------------------------------------------*/

    virtual void DeallocateArray(void* p, bool isUserRequest);

    static LVCSim_KernelInterface *Instance();

    virtual void DumpStackTrace();

    virtual unsigned int SaveStackTrace(void **buffer, unsigned int levels);

    virtual void EvalStackTrace(void **buffer, unsigned int levels,
      OmsStackTraceCallbackInterface *cb);

	static void DebugAllocatorErrorTrace(const char *errMsg, const void *blk);

  virtual SAPDB_UInt8 GetMicroTime();

  virtual bool StatFile(
    const char  *fn,
    SAPDB_UInt8 &ctime,
    SAPDB_UInt8 &fsize);

  virtual void FormatTime(
    char        *buffer,
    size_t      bufsize,
    SAPDB_UInt8 time);

  virtual short LockRequest(LVC_LockRequest &lockReq);

  virtual void RegisterArea(SAPDB_Int4 areaId, const char *name);

  virtual bool UseReaderWriterLocks();

  virtual RTE_IInterface &GetKernelRunTimeInterface();

  virtual void SetErrorCallback(DbpErrorCallbackInterface *cb);
 
  /*!
   * \brief writes into the kernel vtrace
   *
   * \param msg the message to be written into the vtrace
   */
  virtual void VTrace(const char* msg);

protected:
  friend class LVCSim_liveCacheSink;

  static SAPDBMem_IRawAllocator *kernel_allocator;
  static LVCSim_KernelInterface *m_instance;

private:
  static SAPDBMem_IRawAllocator *getAllocator();

  /// Manager for R/W locks.
  LVCSim_RWLockManager m_rw_locks;
};


/*!*****************************************************************************

   endclass: LVCSim_KernelInterface

*******************************************************************************/

#endif