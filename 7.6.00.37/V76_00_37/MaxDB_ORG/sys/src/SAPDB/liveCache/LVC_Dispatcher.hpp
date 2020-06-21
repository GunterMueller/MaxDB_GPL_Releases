/*!
 * \file    LVC_Dispatcher.hpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   OMS Stored Procedure dispatcher interface.
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

#ifndef __LVC_DISPATCHER_HPP
#define __LVC_DISPATCHER_HPP

#include "Container/Container_Hash.hpp"
#include "liveCache/LVC_DispatcherCOMClassHash.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "vak001.h"

#if defined(WIN32)
#include "gos00.h" /* nocheck */
#else
#include "gen00.h" /* nocheck */
#endif

#include "ilivecachesink.h" // nocheck

class SAPDBMem_IRawAllocator;
class RTESys_SharedLib;

// !"§$ TODO  needed??
///// GUID for liveCache sink
//DEFINE_GUID(IID_IliveCacheSink,
//            0x8580E621, 0x243F, 0x11d1, 0xB5, 0xA1, 0xAC, 0x7F, 0x01, 0x06, 0x8C, 0x36);
//
///// GUID for liveCache specification
//DEFINE_GUID(IID_IliveCacheSpec,
//            0x0D4BFA71, 0x5A99, 0x11D1, 0xA9, 0x43, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);

#define   RC_DISPADDR_INVALIDATE_FAILED     -2
#define   RC_DISPADDR_FAILED                -1
#define   RC_DISPADDR_OK                     0
#define   RC_DISPADDR_FACTORY_FOUND          1
#define   RC_DISPADDR_INVALID_FOUND          2
#define   RC_DISPADDR_INUSE                  3

/// Dispatcher info for one COM object.
class LVC_DispatcherInfo {
public:
  /// Initialize dispatcher info.
  LVC_DispatcherInfo(RTESys_SharedLib *lib, const char *name,
    RTE_SharedLibProcAddress io, RTE_SharedLibProcAddress im,
    bool unicode);

  RTESys_SharedLib          *m_lib;             ///< Library containing the COM object.
  RTE_SharedLibProcAddress  m_introspectObject; ///< Function to get basic info about the object.
  RTE_SharedLibProcAddress  m_introspectMethod; ///< Function to get info about a method.
  bool                      m_unicodeCapable;   ///< Is is Unicode capable?

  char                      m_name[1];          ///< Library name (variable-sized).
};

/// Memory for DCOM hash & lock.
class LVC_DispatcherDCOMMemory
{
public:
  LVC_DispatcherDCOMMemory() : lphead(NULL), lpfree(NULL), lpdebug(NULL)
  {
  }

  LVC_DispatcherCOMClassHashPtr   lphead;            
  void*                     lpfree;
  void*                     lpdebug;
  RTESync_Spinlock          lExclusive;
};

/// Transaction end message.
struct LVC_DispatcherTransEndBufMsg {
  tgg00_MessType  mtype;
  tgg00_MessType2 mtype2;
};

/// Buffer with message for transaction end control block.
struct LVC_DispatcherTransEndBuf {
  union {
    LVC_DispatcherTransEndBufMsg  msg;
    tgg00_BasisError              err;
  };
};

/// Transaction end control block.
class LVC_DispatcherTransEnd {
public:
  union {
    tsp00_Int4  buflen;
    tsp00_Int4* lpbuflen;
  };
  LVC_DispatcherTransEndBuf* lpbuf;
};


/*!
 * \brief OMS stored procedure dispatcher interface.
 *
 * This is the base class for various dispatchers. Currently, there are these interfaces:
 *  - LVC_InProcDispatcher - for dispatching procedure calls within kernel
 *  - LVC_ProcServerExtender - for dispatching procedure calls from kernel into ProcServer
 *  - OMS_ProcServerDispatcher - OMS part of the dispatcher in ProcServer
 *
 * \see \ref omscallinterface
 *
 * \todo This interface is not yet finished.
 *
 * \ingroup oms
 */
class LVC_Dispatcher {
public:
  typedef Container_Hash<const char *, LVC_DispatcherInfo*> InfoHash;

  LVC_Dispatcher(SAPDBMem_IRawAllocator &alloc);

  /*!
   * \brief Initialize COM object cache, with lock.
   */
  bool dispInitComCache();

protected:
  /*!
   * \brief Set up monitoring info for the COM routine.
   *
   * Default implementation is empty.
   *
   * \param sink task's sink,
   * \param methodName name of the method about to be executed,
   * \param methodNameAscii name of the method about to be executed in ASCII,
   * \param params user-defined parameters for the routine.
   */
  virtual void dispSetupRoutine(IliveCacheSink *sink, const tsp00_KnlIdentifier &methodName,
    const tsp00_KnlIdentifier &methodNameAscii, void *params);

  /*!
   * \brief Clean up monitoring info after COM routine.
   *
   * Default implementation is empty.
   *
   * \param sink task's sink,
   * \param params user-defined parameters for the routine.
   */
  virtual void dispCleanupRoutine(IliveCacheSink *sink, void *params);

  /*!
   * \brief Get current time.
   *
   * \param sec where to put seconds,
   * \param msec where to put microseconds.
   */
  virtual void dispGetClock(tsp00_Int4 &sec, tsp00_Int4 &msec) = 0;

  /// Abort program execution - fatal error.
  virtual void dispAbort() = 0;

  /*!
   * \brief Evaluate exception.
   *
   * \param hresult reference to hresult,
   * \param userData user-specified data passed to methods that do exception handling.
   *
   * \return status of the handler.
   */
  virtual int dispEvalException(HRESULT &hresult, void *userData) = 0;


  /*!
   * \brief Dispatch a call to a COM routine (including internal ones).
   *
   * This method actually calls a COM routine. Before calling the COM routine,
   * dispSetupRoutine() is called to do eventual preprocessing. After the
   * call, COM allocator is checked by calling IliveCacheSink::UserAllocatorCtl()
   * and dispCleanupRoutine() is called to clean up the action made by
   * dispSetupRoutine().
   *
   * \param sink underlying liveCache sink,
   * \param disp_addr dispatch address (COM object this pointer),
   * \param method_name method to call (ASCII or Unicode, this will be given to other routines),
   * \param method_name_ascii method to call (in ASCII format),
   * \param dispid method number,
   * \param param_num count of parameters,
   * \param param_data pointer to parameters (with types),
   * \param monitor \c true, if we want monitoring info (call to sysMonitorProlog
   *        and sysMonitorEpilog), \c false if not,
   * \param monitor_slot_id ID of monitor to use in monitor prolog/epilog,
   * \param parms user-specific data (in kernel case, tsp_dcom_dispatch_parms pointer),
   * \param setupParms setup and cleanup user-specific data (in inproc a task info structure).
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispDispatch(
    IliveCacheSink                  *sink,
    IUnknown                        *disp_addr,
    const tsp00_KnlIdentifier       &method_name,
    const tsp00_KnlIdentifier       &method_name_ascii,
    tsp00_Int4                      dispid,
    tsp00_Uint                      param_num,
    tak_dbproc_stack_param_debug    *param_data,
    bool                            monitor,
    tsp00_Int2                      monitor_slot_id,
    void                            *parms,
    void                            *setupParms);

  /*!
   * \brief Dispatch transaction end to all registered COM objects.
   *
   * \param sink underlying liveCache sink,
   * \param disp_addr dispatch address (COM object this pointer),
   * \param session session ID,
   * \param session_context opaque session context,
   * \param param_data pointer to a stack to use to pass parameters,
   * \param parms user-specific data (in kernel case, tsp_dcom_dispatch_parms pointer),
   * \param setupParms setup and cleanup user-specific data (in inproc a task info structure).
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispTransEnd(
    IliveCacheSink                  *sink, 
    tsp00_Uint4                     session,
    tsp00_Addr                      session_context,
    LVC_DispatcherTransEnd          *param_stack,
    bool                            monitor,
    tsp00_Int2                      monitor_slot_id,
    void                            *parms,
    void                            *setupParms,
    const tsp00_KnlIdentifier       &methodNameAscii,
    const tsp00_KnlIdentifier       &methodNameUni);

  /*!
   * \brief Query special interface for COM object.
   *
   * \param lpIUnknown COM object,
   * \param lplpIUnknown pointer where to store pointer to special interface,
   * \param msg error message if the interface cannot be found.
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispQuerySpecInterface(
    IUnknown *lpIUnknown, IUnknown **lplpIUnknown,
    const char* msg);

  /*!
   * \brief Release COM instance.
   *
   * \param lpIUnknown COM object,
   * \param msg error message if the release fails.
   */
  void dispRelease(IUnknown *lpIUnknown, const char *msg);

  /*!
   * \brief Initialize COM object cache.
   */
  bool dispInitComCacheLL();

  /*!
   * \brief Initialize COM object cache, with lock.
   */
  bool dispInitComCacheLocked();

  /*!
   * \brief Put COM object into cache.
   *
   * \param lpGuid GUID of the COM object,
   * \param lpInterface COM object interface pointer,
   * \param session session ID,
   * \param hType hash entry type.
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispComCache(
    GUID *lpGuid,
    void *lpInterface, 
    tsp00_Uint4 session,
    LVC_DispatcherHashEntry::HashEntryType hType);

  /*!
   * \brief Get class object (COM object factory).
   *
   * Create a ClassFactory.
   * ClassFactories are globally used within liveCache
   * They are only released on *SHUTDOWN* within
   * dispReleaseAll.
   *
   * This is not supported anymore and will go away.
   *
   * \param CoClsId   GUID of the COM object,
   * \param ifactory  reference, where to store the factory instance.
   *
   * \return HRESULT of the operation.
   */
  virtual HRESULT dispGetClassObjectFromReg(
    GUID          &CoClsId,
    tsp00_Addr    &ifactory);

  /*!
   * \brief Get class object (COM object factory).
   *
   * Create a ClassFactory.
   * ClassFactories are globally used within liveCache
   * They are only released on *SHUTDOWN* within
   * dispReleaseAll.
   *
   * \param libname   library name with COM object,
   * \param packageId id assigned to package by catalog
   * \param CoClsId   GUID of the COM object,
   * \param MD5mark   MD5 checksum of the library,
   * \param ifactory  reference, where to store the factory instance.
   *
   * \return HRESULT of the operation.
   */
  /* PTS 1121178 FF 2003-Mar-20 */
  virtual HRESULT dispGetClassObject(
    char const      *libname,
    const tsp00_C8  &packageId,
    GUID            &CoClsId, 
    const tsp00_C32 &MD5mark, 
    tsp00_Addr &ifactory);

  /*!
   * \brief Get dispatcher address (COM object instance).
   *
   * Get an instance of the COM object, if it exists. If not, return an error
   * code and let the user create the instance using dispCreateInstanceLL()
   * and enter it into cache (COM hash) using dispComCache().
   *
   * \param CoClsId         GUID of the COM object,
   * \param interface_id    GUID of the interface,
   * \param session         session ID,
   * \param idispatch       reference, where to store the COM object instance,
   * \param ifactory        reference, where to store the factory instance,
   * \param params          user-defined parameters.
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispGetDispAddr(const GUID &CoClsId, 
    const IID &interface_id, tsp00_Uint4 session, tsp00_Addr &idispatch,
    tsp00_Addr &ifactory, void *params);

  /*!
   * \brief Create a new instance of the COM object.
   *
   * \param interface_id IID of the interface to create,
   * \param factory class factory for creating COM objects,
   * \param new_instance pointer to pointer to receive the pointer to the new COM object instance.
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispCreateInstanceLL(
    IID *interface_id,
    LPCLASSFACTORY factory,
    void **new_instance);

  /*!
   * \brief Release instance (low-level).
   *
   * \param currEntry current entry in interface hash,
   * \param sessionContext session context.
   */
  virtual void dispReleaseInstanceLL(LVC_DispatcherHashEntry *currEntry, tsp00_Addr sessionContext) = 0;

  /*!
   * \brief Destroy the sink.
   *
   * Called after destroying COM object instances in dispReleaseInstance().
   *
   * \param sink pointer to the sink.
   */
  virtual void dispDestroySink(IliveCacheSink *sink) = 0;

  /*!
   * \brief Release instance.
   *
   * Releases all COM objects associated with the session and then destroys session sink.
   *
   * \param sink session sink,
   * \param session session ID,
   * \param session_context session context.
   */
  void dispReleaseInstance(
    IliveCacheSink                  *sink, 
    tsp00_Uint4                     session,
    tsp00_Addr                      session_context);

  /*!
   * \brief Release all COM object instances.
   */
  void dispReleaseAll();

  /*!
   * \brief Invalidate COM object hash entry for given COM object GUID.
   *
   * \param CoClsId         GUID of the COM object,
   * \param interface_id    GUID of the interface.
   */
  void dispInvalidate(const GUID &CoClsId, 
    const IID &interface_id);

  /*!
   * \brief Call Register method of the COM object.
   * 
   * This method should be called in dispCreateInstance() after creating the instance
   * with dispCreateInstanceLL().
   *
   * \param pILC liveCache special interface,
   * \param parms user-specific data (in kernel case, tsp_dcom_dispatch_parms pointer).
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispRegister(IliveCacheSpec *pILC, void *parms);

  /*!
   * \brief This method is used to cleanup after a DB-Procedure crash.
   *
   * \param session session ID,
   * \param session_context opaque session context,
   * \param parms user-specific data (in kernel case, tsp_dcom_dispatch_parms pointer).
   */
  void dispDisposeInstances(
    tsp00_Uint4                     session,
    tsp00_Addr                      session_context,
    void                            *parms);

  /*!
   * \brief Procedure created to allow dispDisposeInstances to use C++ Objects.
   *
   * SEH and C++ Destructor won't work otherwise. PTS 1109279.
   *
   * \param pILC pointer to liveCache special interface,
   * \param parms user-specific data (in kernel case, tsp_dcom_dispatch_parms pointer).
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispSysCleanup(IliveCacheSpec *pILC, void *parms);

  /*!
   * \brief Get COM object implementation.
   *
   * This method will load given library into memory, if not yet loaded.
   *
   * \param name COM object name.
   *
   * \return pointer to implementation details (list of procedures, etc),
   *    or NULL if failed (shared lib error or memory allocation error).
   */
  virtual const LVC_DispatcherInfo *getImplementation(const char *fname);

  /// Get underlying allocator.
  inline SAPDBMem_IRawAllocator &getAllocator()
  {
    return m_alloc;
  }

public:
  /// Get memory for DCOM hash & friends. 
  inline static LVC_DispatcherDCOMMemory &getDCOMMemory()
  {
    return m_dcomMemory;
  }

  /// Get spinlock for DCOM hash & friends.
  inline static RTESync_Spinlock &getDCOMSpinlock()
  {
    return m_dcomMemory.lExclusive;
  }
private:
  /// Compute required size in stack for all platforms except Win64
  tsp00_Uint4 dispComputeStackSize(tsp00_Uint param_num,
    tak_dbproc_stack_param_debug *param_data);
  /// Check stack for correctness before calling COM routine.
  bool dispCheckStack(const tsp00_KnlIdentifier &method_name_ascii, tsp00_Uint4 param_num, 
    tak_dbproc_stack_param_debug *param_data);

  /// Compute Win64 stack and floating-point stack.
  void dispComputeWin64Stack(tsp00_Uint4 param_num, 
    tak_dbproc_stack_param_debug *param_data,
    tak_dbproc_stack_param *param_stack,
    tak_dbproc_stack_param *fparam_stack, int &len_ret, int &flen_ret);

  /*!
   * \brief Actual call of Register method of the COM object.
   * 
   * This method must be extra due to Win32 exception handling limitations.
   *
   * \param pILC liveCache special interface.
   *
   * \return HRESULT of the operation.
   */
  HRESULT dispRegisterLL(IliveCacheSpec *pILC);

  /// Allocator to use.
  SAPDBMem_IRawAllocator &m_alloc;

  /// Hash table for keeping implementation objects.
  InfoHash m_instances;

  /// DCOM memory for hash & lock.
  static LVC_DispatcherDCOMMemory m_dcomMemory;
};

class LVC_DispatcherLock {
public:
  LVC_DispatcherLock()
  {
    LVC_Dispatcher::getDCOMSpinlock().Lock();
    inRegion = 1;
  }

  ~LVC_DispatcherLock() {
    if ( inRegion )
      LVC_Dispatcher::getDCOMSpinlock().Unlock();
    inRegion = 0;
  }

  void Enter() {
    if ( !inRegion ) 
      LVC_Dispatcher::getDCOMSpinlock().Lock();
    inRegion = 1;
  };

  void Leave() {
    if ( inRegion ) 
      LVC_Dispatcher::getDCOMSpinlock().Unlock();
    inRegion = 0;
  };
  int       inRegion;
};

#endif  // __LVC_DISPATCHER_HPP
