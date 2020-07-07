/*!
 * \file    OMS_LibOmsInterfaceInstance.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS LibOmsInterface.
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



#ifndef OMS_LIBOMSINTERFACEINSTANCE
#define OMS_LIBOMSINTERFACEINSTANCE

#include "ggg01.h"
#include "Oms/OMS_LibOmsInterface.hpp"

#include "livecachetypes.h"  /* nocheck */    

class OMS_LibOmsInterfaceInstance : public OMS_LibOmsInterface
{
  public :

  /*!-----------------------------------------------------------------------

  function: GetFirstVersion
  description: starts an iterator, which yields information about all oms 
               versions. 
         
  arguments:
    lcSink         [in ] Pointer to the kernel sink
    versionId      [out] the name of the first version
    createDate     [out] date when version has been created
    createTime     [out] time when version has been created
    openDate       [out] last open date of the version
    openTime       [out] last open time of the version
    consistentView [out] identification of the consistent view defined by the version
    isMarked       [out] true, if the version is marked via a call of omsMarkVersion
    isOpen         [out] true, if version is currently open
    isUnloaded     [out] true, if the version has been unloaded due to a lack of memory
    heapUsage      [out] memory occupied by the version (in bytes)
    hashSize       [out] size of oid hash header
    versionTree    [out] identification of the b* tree, which contains the changed objects
                         of the version. Has no meaning, if isUnloaded is false
    versionInvTree [out] identification of the b* tree, that implements keyed objects, if the
                         version is unloaded
    versionDesc    [out] user-defined description of a version  
    error          [out] error-code if an error occurred

  return value : true, if at least one version exists; false otherwise
    
  -------------------------------------------------------------------------*/

  virtual tsp00_Bool GetFirstVersion(
    IliveCacheSink*     lcSink,
    tsp00_C24&          versionId,
    tsp00_Date&         createDate,
    tsp00_Time&         createTime,
    tsp00_Date&         openDate,
    tsp00_Time&         openTime,
    tgg91_TransNo&      consistentView,
    tsp00_Bool&         isMarked,
    tsp00_Bool&         isOpen,
    tsp00_Bool&         isUnloaded,
    tsp00_8ByteCounter& heapUsage,
    tsp00_Int4&         hashSize,
    tgg00_FileId&       versionTree,
	  tgg00_FileId&       versionInvTree,
    tsp00_C512&         versionDesc,    // PTS 1117690
    short&              error);         // PTS 1126695

  /*!-----------------------------------------------------------------------

  function: GetMonitorInfo
  description: implements an iterator which returns monitor info of dbprocedures. Every call of the function
               yields a handle, that must be provided for the next call. In the first call
               the handle must be NULL.
          
  arguments:
    lcSink  [in]     pointer to the kernel sink
    handle  [in,out] handle, i.e. state of the iterator. Must be NULL for the first call.
    iid     [out]    Interface Identification
    dispid  [out]    displacement identification of the dbprocedure, i.e. index in vtable.
    monInfo [out]    monitor data

  return value : false, if end of iterator is reached, true otherwise
    
  -------------------------------------------------------------------------*/

  virtual tsp00_Bool GetMonitorInfo(
    IliveCacheSink*       lcSink,
    tsp00_Addr&           handle,
    tsp00_C16&            iid,
    tsp00_Int4&           dispid,
    tgg01_COMMonitorInfo& monInfo);

  /*!-----------------------------------------------------------------------
  
  function: GetNextVersion
  
  description: yields information about the next oms version. After a call of
               GetFirstVersion GetNextVersion must be called until it returns
               false.
         
  arguments:
    lcSink         [in ] pointer to the kernel sink
    versionId      [out] the name of the version
    createDate     [out] date when version has been created
    createTime     [out] time when version has been created
    openDate       [out] last open date of the version
    openTime       [out] last open time of the version
    consistentView [out] identification of the consistent view defined by the version
    isMarked       [out] true, if the version is marked via a call of omsMarkVersion
    isOpen         [out] true, if version is currently open
    isUnloaded     [out] true, if the version has been unloaded due to a lack of memory
    heapUsage      [out] memory occupied by the version (in bytes)
    hashSize       [out] size of oid hash header
    versionTree    [out] identification of the b* tree, which contains the changed objects
                         of the version. Has no meaning, if isUnloaded is false
    versionInvTree [out] identification of the b* tree, that implements keyed objects, if the
                         version is unloaded
    versionDesc    [out] user-defined description of a version  
    error          [out] error-code if an error occurred

  return value : false, if the iterator stands behind the last version, true otherwise
    
  -------------------------------------------------------------------------*/

  virtual tsp00_Bool GetNextVersion(
    IliveCacheSink*     lcSink,
    tsp00_C24&          versionId,
    tsp00_Date&         createDate,
    tsp00_Time&         createTime,
    tsp00_Date&         openDate,
    tsp00_Time&         openTime,
    tgg91_TransNo&      consistentView,
    tsp00_Bool&         isMarked,
    tsp00_Bool&         isOpen,
    tsp00_Bool&         isUnloaded,
    tsp00_8ByteCounter& heapUsage,
    tsp00_Int4&         hashSize,
    tgg00_FileId&       versionTree,
    tgg00_FileId&       versionInvTree, 
    tsp00_C512&         versionDesc,  // PTS 1117690
    short&              error);       // PTS 1126695

  /*!-----------------------------------------------------------------------*/
  // PTS 1117690
  virtual tsp00_Bool GetNextDiagnoseEntry(
    tsp00_Int2       &phase,
    IliveCacheSink   *lcSink,
    tsp00_Int4       &session,
    tsp00_C24        &component,
    tsp00_C512       &entry,
    short            &error);

  /*!-----------------------------------------------------------------------
  
  function : AdviseKernelInterface

  description : tells the liboms about the kernel interface, which allows callbacks from
                liboms to kernel.
             
  parameters :
    knlInterface[in] the kernel interface
    isKernel[in]     true, if called from liveCache kernel, else false
    isUDEServer[in]  true, if called from ude server, else false

  return value : none

  -------------------------------------------------------------------------*/

  virtual void AdviseKernelInterface(
    LVC_KernelInterface &knlInterface, 
    bool                 isKernel,
    bool                 isUDEServer); 

  /*!-----------------------------------------------------------------------
  
  function : AdviseTracer

  description : registers a tracer object in the liboms.

  parameters :
    pTraceObj[in] pointer to the tracer object

  return value : none

  -------------------------------------------------------------------------*/
 
  virtual void AdviseTracer(OMS_TraceInterface* pTraceObj); 

  /*!-----------------------------------------------------------------------
  
  function : Dump

  description : dumps global structures of liboms.

  parameters :
    dumpObj[in]  reference to dump object in kernel environment

  return value : none

  -------------------------------------------------------------------------*/

  virtual void Dump(OMS_DumpInterface&); 


  /*!-----------------------------------------------------------------------
  
  function : InitLibOms

  description : initializes global data structures of the liboms.

  parameters :
    lcSink[in]  pointer to the kernel sink

  return value : none

  -------------------------------------------------------------------------*/

  virtual void InitLibOms(IliveCacheSink* lcSink); 

  /*!-----------------------------------------------------------------------
  
  function : InspectTimeout

  description : checks timeouts of OmsLockObjects.

  parameters : none

  return value : none

  -------------------------------------------------------------------------*/

  virtual void InspectTimeout();

  /*!-----------------------------------------------------------------------
  
  function : IsMonitorOn

  description : returns the state of the OMS monitor.

  parameters : none

  return value : true, if the monitor is enabled; false otherwise

  -------------------------------------------------------------------------*/

  virtual bool IsMonitorOn() const;



  virtual bool NextOmsLockObjInfo(void**, tgg01_OmsLockInfo&);

  /*!-----------------------------------------------------------------------
  
  function : ResetMonitor

  description : Initializes the OMS monitor, i.e resets all counters.

  parameters : 
    lcSink[in]  pointer to the kernel sink

  return value : none

  -------------------------------------------------------------------------*/

  virtual void ResetMonitor(IliveCacheSink* lcSink);

  /*!-----------------------------------------------------------------------
  
  function:     SetDumpError
  
  description:  enable dump whenever an exception of specified error occurs

  arguments:
    errorNo [in] the error code that causes dump

  return value: none
  -------------------------------------------------------------------------*/

  virtual void SetDumpError(tsp00_Int4 errorNo, OMS_DiagnoseAction action);
  

  /*!-----------------------------------------------------------------------
  
  function : SetTraceLevel

  description : enables or disables liboms tracing.

  parameters : 
    lvl[in]    NULL terminated string describing the trace level ("OMS_ERROR",
               "OMS_INTERFACE","OMS_CONTAINERDIR","OMS_MEMORY","OMS_VAROBJECT",
               "OMS_SESSION")
    enable[in] true, if trace level has to be enabled, false otherwise

  return value : none

  -------------------------------------------------------------------------*/

  virtual bool SetTraceLevel(const char* lvl, bool enable);

  /*!-----------------------------------------------------------------------
  
  function : StartStopMonitor

  description : starts or stops the OMS Monitor 

  parameters : 
    doStart[in]  true, if monitor has to be started

  return value : none

  -------------------------------------------------------------------------*/

  virtual void StartStopMonitor(bool doStart);

  /*!-----------------------------------------------------------------------
  
  function : UnAdviseKernelInterface

  description : tells the liboms that the kernel interface becomes invalid
             
  parameters :   none

  return value : none

  -------------------------------------------------------------------------*/

  virtual void UnAdviseKernelInterface();

  virtual bool VersionUnloaded(); 

  /*-------------------------------------------------------------------------*/

  virtual void UserAllocatorCtl(
    int msg, 
    void  *param);

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictInsert(
    const OmsVersionId &versionId, 
    const OMS_Context  *pVersionContext,
    tgg00_BasisError   &error);

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictFind(
    const OmsVersionId  &versionId, 
    OMS_Context        **ppVersionContext);

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictDrop(
    const OmsVersionId &versionId, 
    tgg00_BasisError   &error);

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictShutdown();

  /*-------------------------------------------------------------------------*/

  virtual bool VersionDictUnloadOldestVersion();

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictMarkNotUnloadable(
    OMS_Context *pVersionContext);

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictMarkUnloadable(
    OMS_Context  *pVersionContext);

  /*-------------------------------------------------------------------------*/
  
  virtual void VersionDictCreateIter(
    OMS_LockMode   lockMode, 
    void         **ppIter, 
    OMS_Context  **ppContext);

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictGetNext(
    void         *pIter, 
    OMS_Context **ppContext);

  /*-------------------------------------------------------------------------*/

  virtual void VersionDictDestructIter(
    void  *pIter); 
	
  /*-------------------------------------------------------------------------*/

  virtual void CancelVersion(
    tgg91_TransNo &ConsistViewNo);

  /*-------------------------------------------------------------------------*/
};

#endif
