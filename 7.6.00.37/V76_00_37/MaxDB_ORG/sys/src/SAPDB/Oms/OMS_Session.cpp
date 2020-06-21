/*!
 * \file    OMS_Session.cpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS session implementation
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


#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_DumpInterface.hpp"
#include "Oms/OMS_CallbackInterface.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Defines.h"
#include "hsp77.h"      // PTS 1125307
#include "hsp78_0.h"    // PTS 1135058, PG
#include "geo573.h"
#include "Oms/OMS_DynamicStack.hpp"
#include "Oms/OMS_Globals.hpp"

#define REGISTER_ALLOCATOR true

/*----------------------------------------------------------------------*/

#ifndef USE_SYSTEM_ALLOC_CO13


OMS_Session::OMS_SessionAllocator::OMS_SessionAllocator(tsp00_TaskId taskId)
: SAPDBMem_RawAllocator((const SAPDB_UTF8*) "", OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper, 
                        OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize(), OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize(),
                        SAPDBMem_RawAllocator::FREE_RAW_EXTENDS, SAPDBMEM_ALLOCATOR_UNLIMITED, !REGISTER_ALLOCATOR)
, m_heapTracer(OMS_AllocatorTracer::KERNEL_CRASH)
{
  SAPDB_UTF8 heapName[41];
  sp77sprintf (REINTERPRET_CAST(char*, &heapName[0]), sizeof(heapName), "OMS default session T%03d", taskId);

  SetIdentifier (&heapName[0]);
  SetTracer     (&m_heapTracer);
  SetBadAllocHandler(SAPDBMem_RawAllocator::THROW_STD_BAD_ALLOC);
  SetCheckLevel (OMS_Globals::GetKernelInterface()->GetHeapCheckLevel());
  OMS_Globals::GetKernelInterface()->RegisterAllocator(GetAllocatorInfo());
}

#endif

/*----------------------------------------------------------------------*/

OMS_Session::OMS_Session
(
  IliveCacheSink* lcSink, 
  int             cntRegions,    
  pasbool*        pToCancel,
  bool&           optimizedStreamCommunication, /* PTS 1130281 */
  tsp00_TaskId    taskId) 
:
  m_callbackInterface(NULL),
  m_lcSink(lcSink), m_sizeVarobj(0), m_cntVarobj(0), m_timeout(0), m_refCnt(0),
  m_subtrans_lvl(1), m_min_subtrans_lvl(OMS_Globals::m_globalsInstance->InSimulator() ? 0 : 1),
  m_stream_io(true), m_read_only(false), m_allowReadOnly(true), m_inMethodCallEpilog(false),
  m_isDataChanged(false),
  m_lockObjects(NULL),
  m_context(NULL), m_defaultContext(NULL), m_beforeImages(),
  #ifndef USE_SYSTEM_ALLOC_CO13
  m_heap(taskId),
  #endif
  m_userAllocator("OMS User", taskId, OMS_Globals::m_globalsInstance->m_userAllocatorWrapper, OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize(), OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize()),
  m_comRoutineUserAllocator("OMS User COMRoutine", taskId, OMS_Globals::m_globalsInstance->m_userAllocatorWrapper, OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize(), OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize()),
  m_sessionUserAllocatorWrapper("OMS User Session", taskId, &m_userAllocator, OmsCallbackInterface::BlockSession, REGISTER_ALLOCATOR),
  m_transactionUserAllocatorWrapper("OMS User Transaction", taskId, &m_userAllocator, OmsCallbackInterface::BlockTransaction, REGISTER_ALLOCATOR),
  m_comRoutineUserAllocatorWrapper("OMS User COMRoutine", taskId, &m_comRoutineUserAllocator, OmsCallbackInterface::BlockCOMRoutine, !REGISTER_ALLOCATOR), 
  m_streamRegistry(optimizedStreamCommunication), 
  m_monitor(taskId),
  m_monitor_curr(NULL),
  m_versionsBoundToTrans(NULL), m_createdVersionsInTrans(NULL),
  m_rescheduleDistance(RESCHEDULE_DISTANCE), m_heapAtMethodBegin(0),
  m_toCancel(*pToCancel), /* PTS 1107849 */
  m_optimizedStreamCommunication(optimizedStreamCommunication), /* PTS 1130281 */
  m_taskId(taskId),
  m_exceptionCountDown(-1),
  m_cancelCountDown(-1),   
  m_requiredExceptionThrown(false),
  m_testKnlExceptionActive(false),
  m_handleList(NULL),       // PTS 1116693
  m_errorCode(0),           // PTS 1122839
  m_rwLockDir(),
  m_classDir(),
  m_readReadUnchangedObjectsActive(false),
  m_read_only_old(false),
  m_corrupt(false)
{
  m_sessionUserAllocatorWrapper.SetSession(this);
  m_transactionUserAllocatorWrapper.SetSession(this);
  m_comRoutineUserAllocatorWrapper.SetSession(this);

  memset(&m_callbackInterfaceGuid, 0, sizeof(m_callbackInterfaceGuid));
  int ix;
  m_lockObjects = OMS_ISessionLockObjects::Create(*this);
  m_beforeImages.init(this);
  m_versionsBoundToTrans.advise_allocator(this);
  m_createdVersionsInTrans.advise_allocator(this);
  m_handleList.advise_allocator(this);
  
  // Array is used to check whether the critical sections of the application 
  // are entered twice or are left without being entered before
  int cntApplRegions = cntRegions-OMS_INTERNAL_CRITICAL_SECTIONS;
  m_critical_section_in_use = REINTERPRET_CAST(bool*, m_heap.Allocate(cntApplRegions * sizeof(bool)));
  for (ix = 0; ix < cntApplRegions; ++ix) {
    m_critical_section_in_use[ix] = false;
  }
  m_classDir.Create(this);
  m_rwLockDir.setSession(this);
}

/*----------------------------------------------------------------------*/

OMS_Session::~OMS_Session()
{
  m_lockObjects->Destroy();

  if (InVersion() && !m_handleList.empty()){
    m_handleList.front()->omsForceDropVersion(m_context->GetVersionId());
  }

  if (!m_versionsBoundToTrans.empty()) {

    cgg251dclIterator<OMS_Context*,OMS_Session> iter = m_versionsBoundToTrans.begin();
    while (iter){
      OMS_Context *pContext = *iter();
      ++iter;

      char vId[sizeof(OmsVersionId)+1];
      memcpy (&vId, pContext->GetVersionId(), sizeof(OmsVersionId));
      vId[sizeof(OmsVersionId)] = 0;
      DbpBase(m_lcSink).dbpOpError("OMS_Session::Dtor: Version %s is still bound to session T%2d", 
        vId, GetTaskId());   
    }
  }
  
  m_lcSink->SetDefaultContext(NULL);
  if (NULL != m_defaultContext) {
    m_defaultContext->DeleteSelf();
  }
  deallocate(m_critical_section_in_use);

  // Set Pointer in OmsHandles to this instance to NULL   // PTS 1116693
  for (cgg251dclIterator<OmsHandle*,OMS_Session> iter = m_handleList.begin(); iter; ++iter) {
 	  (*iter())->m_pSession = NULL; 
  }

#ifndef USE_SYSTEM_ALLOC_CO13
  OMS_Globals::GetKernelInterface()->DeregisterAllocator(m_heap.GetAllocatorInfo());
#endif
}

/*----------------------------------------------------------------------*/

void OMS_Session::AssignLcSink(IliveCacheSink* pSink) /* PTS 1109371 */
{
  m_lcSink = pSink;
}

/*----------------------------------------------------------------------*/

const OMS_ObjectId8& OMS_Session::CastOid(ClassID castToGuid, const OMS_ObjectId8& oid)
{
  if (!oid) { // Null Oid given
    return oid;
  }
  OmsObjectContainerPtr p = m_context->GetObj(oid, /*doLock=*/false, /*shared=*/false);
  if (p == NULL){
    ThrowDBError (e_object_not_found, "OMS_Session::CastOid", oid, __MY_FILE__, __LINE__);
  }

  OMS_ContainerEntry*   pFrom = p->GetContainerInfo(__MY_FILE__, __LINE__);
  if (pFrom->GetGuid() == castToGuid) {
    return oid;
  }
  if (pFrom->GetClassEntry().IsDerivedClassOf(castToGuid)) {
    return oid;
  }
  if (GetClassDir().IsBaseClassOf(castToGuid, pFrom->GetGuid())) {
    return oid;
  }
  char msg[80];
  //sprintf (msg, "guid : %8X", castToGuid);                  // PTS 1125307
  sp77sprintf (msg, sizeof(msg), "guid : %8X", castToGuid);   // PTS 1125307
  this->ThrowDBError (e_incompatible_oid, msg, oid, __MY_FILE__, __LINE__);
  return oid;
}

/*----------------------------------------------------------------------*/

void OMS_Session::ChangedConsistentView()
{
  m_currVarObjChunk.Clear();
}

/*----------------------------------------------------------------------*/

void OMS_Session::ClearDefaultContext()
{
  // Dependent on the allocator either only the hash-structures must be 
  // cleared or the objects themselves must be freed, too.
#ifdef USE_SYSTEM_ALLOC_CO13
  const bool deleteObjects = true;
#else
  const bool deleteObjects = false;
#endif

  m_defaultContext->ClearObjCache(deleteObjects, /*adaptOidHash=*/true);
  m_defaultContext->ClearReadOnlyObjCache(deleteObjects);
  m_defaultContext->FreeStackHeap();
}

/*----------------------------------------------------------------------*/
  
bool OMS_Session::IsLockedInKernel (const OMS_ObjectId8& oid, bool shared)
{
  tgg00_BasisError         DBError;
  OMS_UnknownContainerId UnknownFileId;
  OMS_HResult hr = m_lcSink->IsLocked ((unsigned char*) &UnknownFileId, 
    REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)), shared, &DBError);
  if (e_ok == DBError) {
    return true;
  } 
  else {
    if (e_object_not_locked == DBError) {
      return false;
    }
    else {
      this->ThrowDBError (DBError, "IsLockedInKernel", oid, __MY_FILE__, __LINE__);
    }
  }
  return false;
}

/*----------------------------------------------------------------------*/

void OMS_Session::CreateVersion(const OmsVersionId& versionId, const OmsTypeWyde* desc)
{
  const char* msg = "OMS_Session::CreateVersion";

  tsp00_Int2              DBError;
  tgg01_OmsVersionContext versionContext;
  OMS_Context*        context = NULL;

  this->IncCreateVersion   ();
  OMS_HResult hr = m_lcSink->CreateVersion (
      (unsigned char*) &m_context->m_consistentView,
      (unsigned char*) &versionContext,
      &DBError);
  if (DBError != 0) {
      this->ThrowDBError (DBError, msg, versionId, __MY_FILE__, __LINE__);
  }
  /* create a new context for the version */
  try {
    context = new OMS_Context(this, &versionId, &versionContext);
    context->NewConsistentView();
    OpenVersionProlog(versionId, true);  // PTS 1129082
    this->OpenVersion(context, true);
    OpenVersionEpilog();  // PTS 1129082 

    context->SetVersionDesc(desc);     // PTS 1117690
  }
  catch (DbpError& e) {
    if (context != NULL) {
      context->SetSession(this);  // PTS 1134529
      context->DeleteSelf();
    }
    OMS_TRACE(omsTrError, m_lcSink, "Create Version Error : " << e.dbpError());
    throw;
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::CurrentMethodCallEpilog(tsp00_Int4 runTime)
{
  tsp00_8ByteCounter heapUsed;
  m_lcSink->GetSessionHeapUsage(heapUsed);
  if (runTime > 0) // PTS 1126797
  {
      m_monitor.SetRunTime(runTime);
  }
  if (NULL != m_monitor_curr) // PTS 1112146
  {
    m_monitor.AggregateCallStatistics(m_monitor_curr);
    OmsMonitorData info;
    const bool AllocatorOnly = true;
    m_lcSink->EvalCallStatistics(AllocatorOnly, info);
    m_monitor_curr->cmiCounters_gg00[cmiOutOfMemory] += info.m_cntOutOfMemory; // PTS 1126797
    //if (info.m_cntOperatorNew > 0)
    //{
    //  m_monitor_curr->cmiCounters_gg00[cmiMallocCnt] += info.m_cntOperatorNew;
    //  if (info.m_minChunkAllocated < m_monitor_curr->cmiCounters_gg00[cmiMallocMin])
    //  {
    //    m_monitor_curr->cmiCounters_gg00[cmiMallocMin] = info.m_minChunkAllocated; 
    //  }
    //  if (info.m_maxChunkAllocated > m_monitor_curr->cmiCounters_gg00[cmiMallocMax])
    //  {
    //    m_monitor_curr->cmiCounters_gg00[cmiMallocMax] = info.m_maxChunkAllocated; 
    //  }
    //  m_monitor_curr->cmiCounters_gg00[cmiMallocSum] += info.m_avgChunkAllocated * info.m_cntOperatorNew;
    //}
    //if (info.m_cntOperatorDelete > 0)
    //{
    //    m_monitor_curr->cmiCounters_gg00[cmiFreeCnt] += info.m_cntOperatorDelete;
    //    if (info.m_minChunkDeleted < m_monitor_curr->cmiCounters_gg00[cmiFreeMin])
    //    {
    //        m_monitor_curr->cmiCounters_gg00[cmiFreeMin] = info.m_minChunkDeleted; 
    //    }
    //    if (info.m_maxChunkDeleted > m_monitor_curr->cmiCounters_gg00[cmiFreeMax])
    //    {
    //        m_monitor_curr->cmiCounters_gg00[cmiFreeMax] = info.m_maxChunkDeleted; 
    //    }
    //    m_monitor_curr->cmiCounters_gg00[cmiFreeSum] += info.m_avgChunkDeleted * info.m_cntOperatorDelete;
    //}
    if ((heapUsed > m_heapAtMethodBegin) &&
        ( heapUsed > m_monitor_curr->cmiCounters_gg00[cmiCacheSize]))
    {
        m_monitor_curr->cmiCounters_gg00[cmiCacheSize] = heapUsed;
    }
    if ( info.m_maxHeapUsage > m_monitor_curr->cmiCounters_gg00[cmiCacheSize])
    {
        m_monitor_curr->cmiCounters_gg00[cmiCacheSize] = (tsp00_8ByteCounter)info.m_maxHeapUsage;
    }
    tsp00_8ByteCounter heapDelta = heapUsed - m_heapAtMethodBegin;  
    tsp00_8ByteCounter callCnt = ++m_monitor_curr->cmiCallCnt_gg00;
    if (callCnt > 0)
    {
        if (heapDelta < m_monitor_curr->cmiCounters_gg00[cmiDeltaMin])
        {
            m_monitor_curr->cmiCounters_gg00[cmiDeltaMin] = heapDelta; 
        }
        if (heapDelta > m_monitor_curr->cmiCounters_gg00[cmiDeltaMax])
        {
            m_monitor_curr->cmiCounters_gg00[cmiDeltaMax] = heapDelta;
        }
        tsp00_8ByteCounter cnt = m_monitor_curr->cmiCounters_gg00[cmiDeltaAvg];
        m_monitor_curr->cmiCounters_gg00[cmiDeltaAvg] = 
            (m_monitor_curr->cmiCounters_gg00[cmiDeltaAvg] * (callCnt - 1) + heapDelta) / callCnt;
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::DeleteAll(ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo)
{
  const char* msg = "OMS_Session::DeleteAll"; 

  tsp00_Int4  cntDeleted = 0;

  AssertNotReadOnly(msg);
  OMS_ContainerEntry* pContainerInfo = m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo);
  
  // Check whether there are updated objects. Remember pointer to the objects, which belong to the
  // deleted container, in a separate structure for later deletion. So the second time it is not
  // needed to read all objects in the cache, but only the objects belonging to the container.
  OMS_DynamicStack<OmsObjectContainer*, OMS_Session> containerObjects;
  containerObjects.Advise(this, 1000);
  OMS_ContainerHandle containerHandle = pContainerInfo->GetContainerHandle();
  bool updated = m_context->CheckForUpdatedObj(containerHandle, containerObjects);

  if (!updated){
    // No objects are updated => use mass-command
    tsp00_Int2  DBError;
    OMS_ObjectId8  errorOid;
    OMS_HResult hr = m_lcSink->DeleteAllObj (m_context->m_consistentView, (unsigned char*) &pContainerInfo->GetFileId(), 
      (unsigned char*)  m_context->VersionContext(), cntDeleted, DBError, (SAPDB_UInt8&) errorOid);
    m_context->LockResult (DBError, NULL, errorOid, msg, /*shared=*/false);
  
    // Delete objects, which belong to the container, out of the oms-cache 
    m_context->EmptyObjCache(containerObjects, pContainerInfo);  
  }
  else {
    // In the container there are objects which are updated
    // =>Iterate over container and delete each object separately
    OmsHandle *pHandle = m_handleList.front();
    if (pHandle == NULL){
      OMS_Globals::Throw(e_nil_pointer, "OMS_Session::DeleteAll: No omsHandle found", __MY_FILE__, __LINE__, this);
    }

    OmsObjByClsIterBase iter(this, pContainerInfo, 20);
    while (iter){
      OmsAbstractObject *p = iter.omsDeRef(true, true);
      pHandle->omsDelete(p);
      ++iter;
      ++cntDeleted;
    }
  }

  IncDelete_lC (cntDeleted);
}

/*----------------------------------------------------------------------*/

void OMS_Session::DeleteVarObject(const OMS_ObjectId8& oid) 
{
  const char* msg = "OMS_Session::DeleteVarObject";

  this->AssertNotReadOnly(msg);
  this->IncDeleteVarObject();
  OmsObjectContainerPtr found = LoadVarObject(oid, VarObjExclusiveLock, 0, NULL);
  if (!found) {
    this->ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
  }
  OMS_VarObjInfo* pObjInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &found->m_pobj); /* PTS 1106727 */
  pObjInfo->unlock();
  if (!IsLocked(found, /*shared=*/false)) {
    this->ThrowDBError (e_object_not_locked, msg, oid, __MY_FILE__, __LINE__);
  }
  if (!this->InsertBeforeImage (found)) {
    m_context->FreeVarObj(pObjInfo->m_pvobj, pObjInfo->m_vobjSize);
  }
  pObjInfo->m_pvobj     = NULL;
  pObjInfo->m_vobjSize  = 0;
  found->MarkDeleted();

  // PTS 1127661
  // Insert oid of deleted object into reuse structure. 
  if (!InVersion()){
    OMS_ContainerEntry *pInfo = found->GetContainerInfo(__MY_FILE__, __LINE__);
    pInfo->InsertReusableOid(oid); 
  }

  if (oid == m_currVarObjChunk.m_oid) {
    m_currVarObjChunk.Clear();
  }
}

/*----------------------------------------------------------------------*/

const  void* OMS_Session::DeRefVarObject(const OMS_ObjectId8& oid)
{
  this->IncLoadVarObj();
  OmsObjectContainerPtr p = m_context->FindVarObjInContext(oid, VarObjShareLock, 0, NULL);
  if (NULL == p) {
    size_t objSize = m_context->GetVarObjFromLiveCacheBase(oid, false);
    p = m_context->LoadVarObject(oid, VarObjShareLock, objSize, m_currVarObjChunk, NULL);
    m_currVarObjChunk.Clear();
  }
  OMS_VarObjInfo* pObjInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &p->m_pobj);
  return pObjInfo->m_pvobj;
}

/*----------------------------------------------------------------------*/

void* OMS_Session::DeRefVarObjForUpd(const OMS_ObjectId8& oid, unsigned long &objSize)
{
  this->IncLoadVarObj();
  objSize = 0;

  OmsObjectContainerPtr p = m_context->FindVarObjInContext(oid, VarObjShareLock, 0, NULL);
  if (p) {
    // Lock object if it is not locked already
    if (!p->LockedFlag()){
      LockObj(p);
    }
  }
  else {
    // Load object from the kernel and acquire lock
    size_t size = m_context->GetVarObjFromLiveCacheBase(oid, /*doLock=*/true);
    p = m_context->LoadVarObject(oid, VarObjShareLock, size, m_currVarObjChunk, NULL);
    p->MarkLocked();
    m_currVarObjChunk.Clear();
  }

  OMS_VarObjInfo* pObjInfo = reinterpret_cast<OMS_VarObjInfo*>(&p->m_pobj);
  objSize = pObjInfo->m_vobjSize;

  // Remember pointer to old 'in-mem'-representation of the object
  unsigned char *pOld = pObjInfo->m_pvobj;

  if (InsertBeforeImage(p)){
    // As no before image exists already on this level, a new before-images was written

    // As the before-image is pointing to the old 'in-mem'-representation of the object,
    // space for the new object must be allocated and initialized with the old object.

    // Ensure alignment of object and add space for check-pattern
    size_t sz = ((pObjInfo->m_vobjSize + 3) & ~3) + sizeof(tsp00_Uint4);
    pObjInfo->m_pvobj = GetVarObjMemory(sz);

    // Write check-pattern into the last 4 bytes of the allocated memory.
    // This pattern will be checked during store and during the flush.
    reinterpret_cast<tsp00_Uint4*>(pObjInfo->m_pvobj+sz)[-1] = OMS_VAROBJ_CHECK_PATTERN;

    SAPDB_MemCopyNoCheck(pObjInfo->m_pvobj, pOld, pObjInfo->m_vobjSize);
  }

  return pObjInfo->m_pvobj;
}

/*----------------------------------------------------------------------*/

void OMS_Session::DropVersionProlog(OMS_Context* context)
{
  const char* msg = "OMS_Session::DropVersionProlog";
  if (context->IsBoundToTrans()) {
    /* version is bound to trans, bound by myself ? */
    if ((context != CurrentContext()) && (!VersionBoundByMe(context))) {
      ThrowDBError (e_OMS_missing_privilege, msg, context->GetVersionId(), __MY_FILE__, __LINE__);
    }
  }
  else {
    OpenVersion(context, /*create=*/false);
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::DropVersionEpilog(OMS_Context* context)
{
  m_beforeImages.removeContext(context);

  if (context->IsBoundToTrans()) {    // PTS 1129082
    context->CloseVersion();
    RemoveFromTransVersion(context);
  }

  /* free context */
  context->SetSession(this);  // PTS 1134529
  context->DeleteSelf();
  /* return to default context */
  this->ActivateDefaultContext();
  this->IncDropVersion();
}

/*----------------------------------------------------------------------*/

void OMS_Session::Dump(OMS_DumpInterface& dumpObj) const
{
  struct SessionDumpInfo 
  {
    OMS_Session*          m_this;
    ClassID				      m_callbackInterfaceGuid;
    OmsCallbackInterface*     m_callbackInterface;
    IliveCacheSink*           m_lcSink;
    bool*                     m_critical_section_in_use;
    OMS_ISessionLockObjects* m_lockObjects;
    OMS_Context*          m_context;
    OMS_Context*          m_defaultContext;
    tsp00_Int4                m_timeout;
    tsp00_Int2                m_refCnt;
    tsp00_Int2                m_subtrans_lvl;
    bool                      m_stream_io;
    bool                      m_read_only;
    tsp00_Int2                m_filler1;
    tsp00_Int4                m_filler2;
  } sessionDumpInfo;
  sessionDumpInfo.m_this                     = CONST_CAST(OMS_Session*, this);
  sessionDumpInfo.m_callbackInterfaceGuid    = m_callbackInterfaceGuid;
  sessionDumpInfo.m_callbackInterface        = m_callbackInterface; 
  sessionDumpInfo.m_lcSink                   = m_lcSink;
  sessionDumpInfo.m_critical_section_in_use  = m_critical_section_in_use;
  sessionDumpInfo.m_lockObjects              = m_lockObjects;
  sessionDumpInfo.m_context                  = m_context;
  sessionDumpInfo.m_defaultContext           = m_defaultContext;
  sessionDumpInfo.m_timeout                  = m_timeout;
  sessionDumpInfo.m_refCnt                   = m_refCnt;
  sessionDumpInfo.m_subtrans_lvl             = m_subtrans_lvl;
  sessionDumpInfo.m_stream_io                = m_stream_io;
  sessionDumpInfo.m_read_only                = m_read_only;
  dumpObj.SetDumpLabel(LABEL_OMS_SESSION);
  dumpObj.Dump(&sessionDumpInfo, sizeof(sessionDumpInfo));
  m_context->Dump(dumpObj);
  m_beforeImages.Dump(dumpObj);
  dumpObj.SetDumpLabel(LABEL_OMS_BOUND_VERSION);
  OMS_Session* self = CONST_CAST(OMS_Session*, this);
  for (cgg251dclIterator<OMS_Context*,OMS_Session> iter = self->m_versionsBoundToTrans.begin();
    iter; ++iter)
  {
    struct ListDumpInfo 
    {
      void*            m_thisPtr;
      void*            m_nextPtr;
      void*            m_prevPtr;
      OMS_Context* m_context;
    } listDumpInfo;
    
    iter.Dump(listDumpInfo.m_thisPtr, listDumpInfo.m_nextPtr, listDumpInfo.m_prevPtr); 
    listDumpInfo.m_context = *iter();
    dumpObj.Dump(&listDumpInfo, sizeof(listDumpInfo));
  }
}

/*----------------------------------------------------------------------*/

bool OMS_Session::GetMethodCallMonitorInfo (OmsMonitorData& info) const
{
  if (m_inMethodCallEpilog)
  {
    m_monitor.GetCallStatistics(info);
    const bool AllocatorOnly = true;
    OMS_HResult hr = m_lcSink->EvalCallStatistics(!AllocatorOnly, info);
    SAPDB_MemCopyNoCheck(&info.m_methodName[0], &m_monitor_curr->cmiMethodName_gg00[0], sizeof(info.m_methodName));
    return true;
  }
  return false;
}

/*----------------------------------------------------------------------*/
// PTS 1130354
bool OMS_Session::HistoryInUse(const OMS_ObjectId8& oid, bool ignoreVersions, OmsTypeUInt8& OldestTransNo) 
{
  tgg00_BasisError         LCError;
  bool                     isUsed = true;
  OMS_UnknownContainerId UnknownFileId;    
  OMS_HResult hr = m_lcSink->IsObjHistoryUsed(*REINTERPRET_CAST(const SAPDB_UInt8*, &oid), ignoreVersions, 
		isUsed, LCError, OldestTransNo);
  if (0 != LCError) {
    this->ThrowDBError(LCError, "OMS_Session::HistoryInUse", oid, __MY_FILE__, __LINE__);
  }
  return isUsed;
}

/*----------------------------------------------------------------------*/

/// Tries to lock all objects which are enumerated by the iterator 
/*!
** This method tries to lock all objects whose oid is enumerated by the iterator pOids. 
** If some of these objects are out-of-date, then the corresponding oids are appended
** to the iterator pErrOids. If more objects are out-of-date than the iterator can contain,
** then an exception OmsOutOfDate is thrown and the already aquired locks of all objects, 
** which are not stored in the local oms-cache, are freed.
** The timeout specified, is used for the complete routine. If a timeout occurs, then the
** exception e_request_timeout is thrown.
**
** \param oids [in] Iterator which enumerates all objects for which a lock shoud be aquired.
** \param timeout [in/out] In-parameter: the maximal execution time in sec for getting all the locks.
**                            If timeout==0, then it will not be waited for locks to be freed and
**                            objects, for which locks can not be aquired because of another already
**                            existing lock, will be append to the error-iterator, too.
**                         Out-parameter: the execution time in sec needed by the routine
** \param errOids [out] Iterator which enumerates all objects for which no lock could be
**                      aquired because they have been deleted.
** \param newConsistentView [in] If true: Special handling as the mass-lock is called from newConsistentView:
**                                 * if an error occurs all aquired locks will be released
**                                 * the kernel does not check against OutOfDate because of an update of 
**                                   an object. This is used when a new consistent view is created. 
**                                   Nevertheless the kernel might return an OutOfDate in this case 
**                                   which then means, that the object has been deleted. 
**
** \since 7.4.4 (PTS 1121449)
*/
void OMS_Session::LockObjMass (OmsIOidReadIterator &oids, 
                               short               &timeout, 
                               OmsIOidAppender     &errOids,
                               bool                 shared,
                               bool                 newConsistentView,
                               cgg250AvlTree<OMS_ObjectId8,OMS_ObjectId8,OMS_Session> *pUnlockedObjects)
{
  const char * msg = "OMS_Session::LockObjMass ";

  int              overallCnt    = 0;
  int              cnt           = 0;
  int              errCnt        = 0;
  tsp00_Int8       remainingTime = timeout * 1000000;
  tgg00_BasisError currError     = e_ok;
  OmsObjectContainer *pObj       = NULL;

  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pOid_u         [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8            *pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef           pObjVers       [OMS_MASS_OPERATION_CNT];
  tgg00_FileId           *ppContainerId  [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  //OMS_UnknownContainerId  pContainerId [OMS_MASS_OPERATION_CNT];
  void*                   pContainerId_u [OMS_MASS_OPERATION_CNT * sizeof(OMS_UnknownContainerId)/sizeof(void*)];
  OMS_UnknownContainerId *pContainerId = reinterpret_cast<OMS_UnknownContainerId*>(&pContainerId_u[0]);
  tgg00_BasisError        pDBError       [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer     *ppObj          [OMS_MASS_OPERATION_CNT];

  // Try to get a lock for all objects which are requested
  while (bool(oids)) {
    while (bool(oids) && cnt < OMS_MASS_OPERATION_CNT) {
      OMS_ObjectId8   currOid (oids.omsGet());

      // Remember entries for locking
      pOid[cnt] = currOid;
      if (newConsistentView){
        // The special setting of pObjVers signals the kernel that no check for 
        // OutOfDate, caused by an update, should be performed. If an OutOfDate
        // is returned, then the corresponding object has been deleted.
        pObjVers     [cnt].gg91SetDummyRef(); 
        new(&pContainerId [cnt]) OMS_UnknownContainerId();
        ppContainerId[cnt] = pContainerId[cnt].GetFileIdPointer();
        ppObj        [cnt] = NULL;
        ++cnt;
      }
      else {
        pObj = m_context->FindObjInContext(&currOid);
        if (pObj
           && (!shared && pObj->LockedFlag() || shared && pObj->LockedSharedFlag())){
          // Object is stored in the local cache and is already locked
          // => nothing must be done
        }
        else if (pObj){
          // Object is stored in the cache but not locked
          pObjVers     [cnt] = pObj->m_objseq;
          ppContainerId[cnt] = &pObj->GetContainerInfo(__MY_FILE__, __LINE__)->GetFileId();
          ppObj        [cnt] = pObj;
          ++cnt;
        }
        else {
          // Object is not stored locally and must be dereferenced 
          // TODO: Should be later replaced by a mass deref with lock
          try {
            pObj = m_context->GetObj(currOid, /*doLock=*/true, /*shared=*/shared);
          }
          catch (DbpError &e){
            if (   e.m_errorNo == e_object_dirty
               || !newConsistentView && (  e.m_errorNo == e_object_not_found
                                       ||  e.m_errorNo == e_OMS_lock_collision
                                       || (e.m_errorNo == e_OMS_request_timeout && timeout == 0))){
              // Try to append outdated object to error structure
              if (!errOids.omsAppend(currOid)) {
                // Too many object have already been appended. Therefore release locks, which has already  
                // been acquired, and stop further processing

                if (newConsistentView){
                  // Release locks which have been aquired already
                  ReleaseLocks(oids, overallCnt, shared);
                }

                ThrowDBError(e.m_errorNo, msg, currOid, __MY_FILE__, __LINE__);
              }
              else {
                DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
                if (pCBInterface){
                  pCBInterface->dbpCaughtError(e);
                }       
              }
            }
            else {
              // Unacceptable error occurred
              ThrowDBError(e.m_errorNo, msg, currOid, __MY_FILE__, __LINE__);
            }
          }
          if (pObj){
            if (!shared){
              pObj->MarkLocked();
            }
            else {
              pObj->MarkLockedShared();
            }
          }
        }
      }
      ++oids;
      ++overallCnt;
    }


    if (cnt > 0) {
      // Mass lock
      if (!shared){
        OMS_HResult hr = m_lcSink->LockObjMass(
          cnt, &CurrentContext()->m_consistentView, ppContainerId, (SAPDB_UInt8*)pOid, 
          pObjVers, remainingTime, errCnt, pDBError);
      }
      else {
        OMS_HResult hr = m_lcSink->LockSharedObjMass(
          cnt, &CurrentContext()->m_consistentView, ppContainerId, (SAPDB_UInt8*)pOid, 
          pObjVers, remainingTime, errCnt, pDBError);
      }

      for (int i=0; i<cnt; ++i){
        if (   pDBError[i] == e_object_dirty
            || !newConsistentView && (  pDBError[i] == e_object_not_found
                                    ||  pDBError[i] == e_OMS_lock_collision
                                    || (pDBError[i] == e_OMS_request_timeout && timeout == 0))){
          // Try to append outdated object to error structure
          if (!errOids.omsAppend(pOid[i])) {
            // Too many object have already been appended. Therefore release locks, which has already  
            // been aquired, and stop further processing

            if (newConsistentView){
              // Release locks which have been aquired already
              ReleaseLocks(oids, overallCnt, shared);
            }

            ThrowDBError(pDBError[i], msg, pOid[i], __MY_FILE__, __LINE__);
          }
          else {
            if (pUnlockedObjects != NULL){
              pUnlockedObjects->Insert(pOid[i]);
            }

            // Object has been appended to the error-list
            pDBError[i] = e_ok;
            --errCnt;
          }
        }
        else if (pDBError[i] != e_ok){
          if (newConsistentView){
            // Release locks which have been aquired already
            ReleaseLocks(oids, overallCnt, shared);
          }

          // Unacceptable error occured
          ThrowDBError(pDBError[i], msg, pOid[i], __MY_FILE__, __LINE__);
        }
        else if (ppObj[i] != NULL){
          // Mark object as locked in the liboms
          ppObj[i]->MarkLocked();
        }
      }
      cnt = 0;

      // Check whether timeout has occured
      if (timeout > 0 && remainingTime < 0) {
        if (newConsistentView){
          // Release locks which have been aquired already
          ReleaseLocks(oids, overallCnt, shared);
        }

        ThrowDBError(400 /*e_request_timeout*/, msg, __MY_FILE__, __LINE__);
      }

      // Check that all errors have been handled   PTS 1123704
      if (errCnt > 0){  
        ThrowDBError(e_unknown_error, msg, __MY_FILE__, __LINE__);
      }
    }
  }

  if (timeout > 0){
    // Return the execution time
    timeout -= (short) (remainingTime / 1000000);
  }
}

/*----------------------------------------------------------------------*/

int OMS_Session::MassDeref(OmsIDerefIter &derefIter)
{
  const char* msg = "OMS_Session::MassDeref ";

  int errCnt = 0;

  // Arrays for mass deref
  int noOfObj   = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8              pOid_u        [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8              *pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef             pObjVers      [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo             pUpdTransId   [OMS_MASS_OPERATION_CNT];
  OMS_ClassEntry           *ppClassInfo   [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  //OMS_UnknownContainerId  pContainerId  [OMS_MASS_OPERATION_CNT];
  void*                     pContainerId_u[OMS_MASS_OPERATION_CNT * sizeof(OMS_UnknownContainerId)/sizeof(void*)];
  OMS_UnknownContainerId   *pContainerId = reinterpret_cast<OMS_UnknownContainerId*>(&pContainerId_u[0]);

  // arrays for mass deref of base class oids
  int noOfObjBC = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8              pOidBC_u      [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8              *pOidBC = reinterpret_cast<OMS_ObjectId8*>(&pOidBC_u[0]);
  tgg91_PageRef             pObjVersBC    [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo             pUpdTransIdBC [OMS_MASS_OPERATION_CNT];
  OMS_ClassEntry           *ppClassInfoBC [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  //OMS_UnknownContainerId  pContainerIdBC[OMS_MASS_OPERATION_CNT];
  char                      pContainerIdBC_u[OMS_MASS_OPERATION_CNT * sizeof(OMS_UnknownContainerId)];
  OMS_UnknownContainerId   *pContainerIdBC = reinterpret_cast<OMS_UnknownContainerId*>(&pContainerIdBC_u[0]);

  // arrays used by both methods (exporting parameters of kernel call)
  OmsAbstractObject        *ppObj         [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer       *ppObjContainer[OMS_MASS_OPERATION_CNT];
  size_t                    pObjSize      [OMS_MASS_OPERATION_CNT];
  tgg00_BasisError          pDBError      [OMS_MASS_OPERATION_CNT];

  // Arrays to remember information over mass-deref call
  const OmsAbstractObject **pppAddr       [OMS_MASS_OPERATION_CNT];
  const OmsAbstractObject **pppAddrBC     [OMS_MASS_OPERATION_CNT];
  long                     *ppErrAddr     [OMS_MASS_OPERATION_CNT];
  long                     *ppErrAddrBC   [OMS_MASS_OPERATION_CNT];

  OMS_ClassEntry *pClassInfo = NULL;
  ClassID guid = -1;

  int DBErrorCnt = 0;
  while (bool(derefIter)){
    // Read packages of maximal size OMS_MASS_OPERATION_CNT from the kernel
    while (noOfObj<OMS_MASS_OPERATION_CNT && noOfObjBC<OMS_MASS_OPERATION_CNT && bool(derefIter)) {
      OMS_ObjectId8               oid      = derefIter.omsGetOid();
      const OmsAbstractObject **ppAddr   = derefIter.omsGetObjAddr();
      long                     *pErrAddr = derefIter.omsGetErrAddr();
      
      // Check whether object is already stored in the local oms-cache
      IncDeref();
      OmsObjectContainer* found = m_context->FindObjInContext(&oid);
      if (found) {
        if (found->DeletedFlag()){
          // Object is already marked as deleted in the local cache
          OmsObjectId oid4 (oid);
          derefIter.omsSetResult(ppAddr, oid4, NULL, pErrAddr, e_object_not_found);
        }
        else {
          // Append object to the return structure
          OmsObjectId oid4 (oid);
          derefIter.omsSetResult(ppAddr, oid4, 
                                 const_cast<const OmsAbstractObject*>(&found->m_pobj), 
                                 pErrAddr, e_ok);
        }  
      }
      else {
        // Buffer last classInfo to prevent unnecessary accesses to the class-hash
        if (pClassInfo == NULL || derefIter.omsGetGuid() != guid){
          guid        = derefIter.omsGetGuid();
          pClassInfo  = GetClassDir().GetClassEntry(guid);
        } 

        if (pClassInfo->IsBaseClass()) {
          // oid may identify an object of a derived class, i.e. size is unknown.
          // Therefore first get container (and thereby the size) from the kernel and
          // then deref objects with known size.
          pOidBC[noOfObjBC]    = oid;
          new(&pContainerIdBC[noOfObjBC]) OMS_UnknownContainerId();
          pObjVersBC[noOfObjBC].gg91SetNilRef();
          pppAddrBC[noOfObjBC] = ppAddr;
          ppErrAddrBC[noOfObjBC] = pErrAddr;
          ++noOfObjBC;
        }
        else {
          pOid[noOfObj]        = oid;
          ppClassInfo[noOfObj] = pClassInfo;
          new(&pContainerId[noOfObj]) OMS_UnknownContainerId();
          pObjVers[noOfObj].gg91SetNilRef();
          pppAddr[noOfObj]     = ppAddr;
          ppErrAddr[noOfObj]   = pErrAddr;
          ++noOfObj;
        }
      }
      ++derefIter;
    }


    if (noOfObj == OMS_MASS_OPERATION_CNT || (noOfObj > 0 && !bool(derefIter))) {
      // mass deref of objects which are not already stored in the local oms-cache
      int errorCnt = m_context->LoadObjsFromLiveCacheBase(noOfObj, pOid, pObjVers, pUpdTransId,
                       /*doLock=*/false, /*shared=*/false, ppObjContainer, pDBError, 
                       ppObj, ppClassInfo, pContainerId->GetFileIdPointer(), pObjSize);
      DBErrorCnt += errorCnt;

      for (int i=0; i<noOfObj; ++i){
        // Append object, that have been read from the kernel, to the output
        OmsObjectId oid4 (pOid[i]);
        derefIter.omsSetResult(pppAddr[i], oid4, 
                               const_cast<const OmsAbstractObject*>(&ppObjContainer[i]->m_pobj), 
                               ppErrAddr[i], pDBError[i]);
      }
      noOfObj = 0;
    }

    if (noOfObjBC == OMS_MASS_OPERATION_CNT || (noOfObjBC > 0 && !bool(derefIter))) {
      // Mass-Deref from the kernel of objects which belong to a base class
      int errorCnt = m_context->LoadBaseClsObjsFromLiveCacheBase(noOfObjBC, pOidBC, pObjVersBC, pUpdTransIdBC,
                       /*doLock=*/false, /*shared=*/false, ppObjContainer, pDBError, ppObj, 
                       ppClassInfoBC, pContainerIdBC->GetFileIdPointer(), pObjSize);
      DBErrorCnt += errorCnt;

      for (int i=0; i<noOfObjBC; ++i){
        // Append object, that have been read from the kernel, to the output
        OmsObjectId oid4 (pOidBC[i]);
        derefIter.omsSetResult(pppAddrBC[i], oid4, 
                      const_cast<const OmsAbstractObject*>(&ppObjContainer[i]->m_pobj), 
                      ppErrAddrBC[i], pDBError[i]);
      }
      noOfObjBC = 0;
    }
  }

  return DBErrorCnt;
}

/*----------------------------------------------------------------------*/

int OMS_Session::MassDerefForUpd(OmsIDerefIterForUpd &derefIter,
                                 bool                 doLock,
                                 bool                 shared)
{
  const char* msg = "OMS_Session::MassDerefForUpd ";

  int errCnt = 0;

  // Arrays for mass deref
  int noOfObj   = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pOid_u        [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8            *pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef           pObjVers      [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo           pUpdTransId   [OMS_MASS_OPERATION_CNT];
  OMS_ClassEntry         *ppClassInfo   [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  //OMS_UnknownContainerId  pContainerId  [OMS_MASS_OPERATION_CNT];
  void*                   pContainerId_u[OMS_MASS_OPERATION_CNT * sizeof(OMS_UnknownContainerId)/sizeof(void*)];
  OMS_UnknownContainerId *pContainerId = reinterpret_cast<OMS_UnknownContainerId*>(&pContainerId_u[0]);
  
  // arrays for mass deref of base class oids
  int noOfObjBC = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8              pOidBC_u      [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8              *pOidBC = reinterpret_cast<OMS_ObjectId8*>(&pOidBC_u[0]);
  tgg91_PageRef             pObjVersBC    [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo             pUpdTransIdBC [OMS_MASS_OPERATION_CNT];
  OMS_ClassEntry           *ppClassInfoBC [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  //OMS_UnknownContainerId  pContainerIdBC[OMS_MASS_OPERATION_CNT];
  char                      pContainerIdBC_u[OMS_MASS_OPERATION_CNT * sizeof(OMS_UnknownContainerId)];
  OMS_UnknownContainerId   *pContainerIdBC = reinterpret_cast<OMS_UnknownContainerId*>(&pContainerIdBC_u[0]);

  // arrays for locking
  int lockCnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pLockOid_u         [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8            *pLockOid = reinterpret_cast<OMS_ObjectId8*>(&pLockOid_u[0]);
  tgg91_PageRef           pLockObjVers       [OMS_MASS_OPERATION_CNT];
  tgg00_FileId           *ppLockContainerId  [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer     *ppLockContainerPtr [OMS_MASS_OPERATION_CNT];

  // arrays used by all methods (exporting parameters of kernel call)
  OmsAbstractObject      *ppObj         [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer     *ppObjContainer[OMS_MASS_OPERATION_CNT];
  size_t                  pObjSize      [OMS_MASS_OPERATION_CNT];
  tgg00_BasisError        pDBError      [OMS_MASS_OPERATION_CNT];

  // Arrays to remember information over mass-deref call
  OmsAbstractObject **pppAddr     [OMS_MASS_OPERATION_CNT];
  OmsAbstractObject **pppAddrBC   [OMS_MASS_OPERATION_CNT];
  long               *ppErrAddr   [OMS_MASS_OPERATION_CNT];
  long               *ppErrAddrBC [OMS_MASS_OPERATION_CNT];

  OMS_ClassEntry *pClassInfo = NULL;
  ClassID guid = -1;

  int DBErrorCnt = 0;
  while (bool(derefIter)){
    // Read packages of maximal size OMS_MASS_OPERATION_CNT from the kernel
    while (noOfObj<OMS_MASS_OPERATION_CNT && noOfObjBC<OMS_MASS_OPERATION_CNT && bool(derefIter)) {
      OMS_ObjectId8               oid(derefIter.omsGetOid());
      OmsAbstractObject       **ppAddr   = derefIter.omsGetObjAddr();
      long                     *pErrAddr = derefIter.omsGetErrAddr();
      
      // Check whether object is already stored in the local oms-cache
      IncDeref();
      OmsObjectContainer* found = m_context->FindObjInContext(&oid);
      if (found) {
        if (found->DeletedFlag()){
          // Object is already marked as deleted in the local cache
          OmsObjectId oid4(oid);
          derefIter.omsSetResult(ppAddr, oid4, NULL, pErrAddr, e_object_not_found);
        }
        else {
          // Object is found in the local cache
          if (doLock && !m_context->IsVersion() && !found->LockedFlag()){
            // remember necessary information for later mass lock
            pLockOid          [lockCnt] = oid;
            pLockObjVers      [lockCnt] = found->m_objseq;
            ppLockContainerId [lockCnt] = &found->GetContainerInfo(__MY_FILE__, __LINE__)->GetFileId();
            ppLockContainerPtr[lockCnt] = found;
            ++lockCnt;
          }
          // Append object to the return structure
          OmsObjectId oid4(oid);
          derefIter.omsSetResult(ppAddr, oid4, ForUpdPtr(found), pErrAddr, e_ok);
        }  
      }
      else {
        // Buffer last classInfo to prevent unnecessary accesses to the class-hash
        if (pClassInfo == NULL || derefIter.omsGetGuid() != guid){
          guid        = derefIter.omsGetGuid();
          pClassInfo  = GetClassDir().GetClassEntry(guid);
        } 

        if (pClassInfo->IsBaseClass()) {
          // oid may identify an object of a derived class, i.e. size is unknown.
          // Therefore first get container (and thereby the size) from the kernel and
          // then deref objects with known size.
          pOidBC[noOfObjBC]      = oid;
          new(&pContainerIdBC[noOfObjBC]) OMS_UnknownContainerId();
          pObjVersBC[noOfObjBC].gg91SetNilRef();
          pppAddrBC[noOfObjBC]   = ppAddr;
          ppErrAddrBC[noOfObjBC] = pErrAddr;
          ++noOfObjBC;
        }
        else {
          pOid[noOfObj]        = oid;
          ppClassInfo[noOfObj] = pClassInfo;
          new(&pContainerId[noOfObj]) OMS_UnknownContainerId();
          pObjVers[noOfObj].gg91SetNilRef();
          pppAddr[noOfObj]     = ppAddr;
          ppErrAddrBC[noOfObj] = pErrAddr;
          ++noOfObj;
        }
      }
      ++derefIter;
    }


    if (noOfObj == OMS_MASS_OPERATION_CNT || (noOfObj > 0 && !bool(derefIter))) {
      // mass deref of objects which are not already stored in the local oms-cache
      int errorCnt = m_context->LoadObjsFromLiveCacheBase(noOfObj, pOid, pObjVers, pUpdTransId, doLock, shared,
                       ppObjContainer, pDBError, ppObj, ppClassInfo, pContainerId->GetFileIdPointer(), pObjSize);
      DBErrorCnt += errorCnt;

      for (int i=0; i<noOfObj; ++i){
        // Append object, that have been read from the kernel, to the output
        OmsObjectId oid4(pOid[i]);
        derefIter.omsSetResult(pppAddr[i], oid4, 
                               ppObjContainer[i] == NULL ? NULL : ForUpdPtr(ppObjContainer[i]), 
                               ppErrAddr[i], pDBError[i]);
      }
      noOfObj = 0;
    }

    if (noOfObjBC == OMS_MASS_OPERATION_CNT || (noOfObjBC > 0 && !bool(derefIter))) {
      // Mass-Deref from the kernel of objects which belong to a base class
      int errorCnt = m_context->LoadBaseClsObjsFromLiveCacheBase(noOfObjBC, pOidBC, pObjVersBC, pUpdTransIdBC, doLock, shared,
                       ppObjContainer, pDBError, ppObj, ppClassInfoBC, pContainerIdBC->GetFileIdPointer(), pObjSize);
      DBErrorCnt += errorCnt;

      for (int i=0; i<noOfObjBC; ++i){
        // Append object, that have been read from the kernel, to the output
        OmsObjectId oid4(pOidBC[i]);
        derefIter.omsSetResult(pppAddrBC[i], oid4, 
                               ppObjContainer[i] == NULL ? NULL : ForUpdPtr(ppObjContainer[i]), 
                               ppErrAddrBC[i], pDBError[i]);
      }
      noOfObjBC = 0;
    }

    if (lockCnt == OMS_MASS_OPERATION_CNT || (lockCnt > 0 && !bool(derefIter))) {
      // Mass lock
      tsp00_Int8 timeout = 0;
      if (!shared){
        OMS_HResult hr = m_lcSink->LockObjMass( lockCnt, &m_context->m_consistentView, ppLockContainerId,
                           (SAPDB_UInt8*)pLockOid, pLockObjVers, timeout, errCnt, pDBError);
      }
      else {
        OMS_HResult hr = m_lcSink->LockSharedObjMass( lockCnt, &m_context->m_consistentView, ppLockContainerId,
                           (SAPDB_UInt8*)pLockOid, pLockObjVers, timeout, errCnt, pDBError);
      }
      m_context->LockResult (lockCnt, pDBError, ppLockContainerPtr, pOid, msg, shared); 

      lockCnt = 0;
    }
  }

  return DBErrorCnt;
}

/*----------------------------------------------------------------------*/
// PTS 1122194
int OMS_Session::MassDerefViaKey(OmsIDerefKeyIter &derefIter)
{
  const char* msg = "OMS_Session::MassDerefViaKey ";

  int errCnt = 0;

  // Arrays for mass deref
  int noOfObj   = 0;
  unsigned char            *ppBinaryKey   [OMS_MASS_OPERATION_CNT];
  size_t                    pKeyLen       [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8              pOid_u        [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8              *pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef             pObjVers      [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo             pUpdTransId   [OMS_MASS_OPERATION_CNT];
  OMS_ContainerEntry       *ppContainerInfo[OMS_MASS_OPERATION_CNT];
  tgg01_ContainerId         pContainerId  [OMS_MASS_OPERATION_CNT];

  // exporting parameters of kernel call
  OmsAbstractObject        *ppObj         [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer       *ppObjContainer[OMS_MASS_OPERATION_CNT];
  size_t                    pObjSize      [OMS_MASS_OPERATION_CNT];
  tgg00_BasisError          pDBError      [OMS_MASS_OPERATION_CNT];

  const OmsAbstractObject **pppAddr       [OMS_MASS_OPERATION_CNT];
  const unsigned char      *ppKey         [OMS_MASS_OPERATION_CNT];
  long                     *ppErrAddr     [OMS_MASS_OPERATION_CNT];

  int DBErrorCnt = 0;
  while (bool(derefIter)){
    // Read packages of maximal size OMS_MASS_OPERATION_CNT from the kernel
    while (noOfObj<OMS_MASS_OPERATION_CNT && bool(derefIter)) {
      IncDerefKey();

      // Remember original key and address where to store the result
      const OmsAbstractObject **ppAddr   = derefIter.omsGetObjAddr();
      const unsigned char      *pKey     = derefIter.omsGetKey();
      long                     *pErrAddr = derefIter.omsGetErrAddr();

      // Get container info
      OMS_ContainerEntry*  pContainerInfo = m_context->GetContainerDir().GetContainerEntry(
                                                             derefIter.omsGetGuid(), 
                                                             derefIter.omsGetSchema(), 
                                                             derefIter.omsGetContainerNo());

      // Convert key into binary representation
      OmsObjectContainer *pObj         = pContainerInfo->GetMemory();
      unsigned char      *pBinaryKey   = pContainerInfo->GetClassEntry().GetKeyPtr(pObj);
      OmsAbstractObject  *pAbstractObj = &pObj->m_pobj;
      pAbstractObj->omsKeyToBinary(pKey, pBinaryKey);

      // Check whether object is already stored in the local oms-cache.
      // A key-access to the oms cache is possible if either objects are created
      // in a version, or the cached key feature is switched on.
      OmsObjectContainer* found = NULL;
      if (pContainerInfo->UseCachedKeys() || m_context->IsVersion()){
        found = pContainerInfo->VersionFindKey(pBinaryKey);
        if (found){
          if (IsReadUnchangedObjectsActive()){
            if (m_context->IsVersionOid(found->m_oid)){
              // If 'IsReadUnchangedObjectsActive' then the objects should be read in the state, 
              // they were when the consistent view was started. If the object was created in the
              // version, it did not exist, when the version was created, and therefore "e_object_not_found".
              pContainerInfo->ChainFree(pObj, 64);
              derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),
                NULL, pErrAddr, e_object_not_found);
            }
            else {
              // Check if object has been changed
              found = m_context->FindObjInContext(&found->m_oid, /*ignoreGeneration=*/false, /*checkDropped=*/true,
                /*checkLocallyCreated=*/false);
              if (found){
                pContainerInfo->ChainFree(pObj, 65);
                if (found->IsNewObject()){
                  // As object is locally created in the current version, this object is not
                  // visible in the 'read-unchanged'-mode
                  derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),
                    NULL, pErrAddr, e_object_not_found);
                }
                else {
                  derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),
                    &found->m_pobj, pErrAddr, e_ok);
                }
              }
              // found == NULL 
              // Object exists in local cache, but has been changed. Therefore read it from kernel.
            }
          }
          else {
            // Statistics
            IncCacheHit(found->m_oid);

            // release memory
            pContainerInfo->ChainFree(pObj, 66);

            if (found->DeletedFlag()) 
              derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),
                                    NULL, pErrAddr, e_object_not_found);
            else 
              derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),
                                    &found->m_pobj, pErrAddr, e_ok);
          }
        }
        else if (pContainerInfo->UseCachedKeys() && pContainerInfo->IsCacheMiss(pBinaryKey)) {
          // Statistics
          IncCacheMiss();

          // release memory
          pContainerInfo->ChainFree(pObj, 67);

          derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(), 
                                 NULL, pErrAddr, e_object_not_found);
        }
      }

      if (!found){
        // Remember data which is needed to store the result after the deref
        pppAddr     [noOfObj]        = ppAddr;
        ppKey       [noOfObj]        = pKey;
        ppErrAddr   [noOfObj]        = pErrAddr;

        // insert key into array for mass deref            
        ppBinaryKey [noOfObj]        = pBinaryKey;
        pObjVers    [noOfObj].gg91SetNilRef();
        ppContainerInfo[noOfObj]     = pContainerInfo;
        pContainerId[noOfObj]        = pContainerInfo->GetFileId(); 
        pKeyLen     [noOfObj]        = pContainerInfo->GetClassEntry().GetKeyDesc().GetLen();
        pObjSize    [noOfObj]        = pContainerInfo->GetClassEntry().GetTransientSize();  

        // Get object frame to store the objects
        ppObjContainer[noOfObj]     = pObj;

        // Determine position where the kernel should write the object to. As the vtbl-pointer
        // is stored at the beginning of the abstract object the position must be incremented
        // by the size of a pointer.)
        ppObj[noOfObj] = (OmsAbstractObject*) ((unsigned char*) (&ppObjContainer[noOfObj]->m_pobj) + sizeof(void*));
        ++noOfObj;
      }

      ++derefIter;
    }

    if (noOfObj == OMS_MASS_OPERATION_CNT || (noOfObj > 0 && !bool(derefIter))) {
      // mass deref of objects which are not already stored in the local oms-cache
      int errorCnt = m_context->LoadObjsViaKeyFromLCBase(noOfObj, pKeyLen, ppBinaryKey, pOid, 
                        pObjVers, pUpdTransId, /*doLock=*/false, /*shared=*/false, ppObjContainer, pDBError, 
                        ppObj, ppContainerInfo, pContainerId, pObjSize);

      DBErrorCnt += errorCnt;

      for (int i=0; i<noOfObj; ++i){
        // Append object, that have been read from the kernel, to the output
        derefIter.omsSetResult(pppAddr[i], ppKey[i] , ppContainerInfo[i]->GetClassEntry().GetKeyDesc().GetLen(), 
                               const_cast<const OmsAbstractObject*>(&ppObjContainer[i]->m_pobj), 
                               ppErrAddr[i], pDBError[i]);

        if (ppContainerInfo[i]->UseCachedKeys() && ppObjContainer[i] != NULL){
          // Insert object into search structure for cached keys
          ppContainerInfo[i]->VersionAddKey(ppObjContainer[i], true);
        }
      }
      noOfObj = 0;
    }
  }

  return DBErrorCnt;
}

/*----------------------------------------------------------------------*/
// PTS 1122194
int OMS_Session::MassDerefViaKeyForUpd(OmsIDerefKeyIterForUpd &derefIter, bool doLock, bool shared)
{
  const char* msg = "OMS_Session::MassDerefViaKeyForUpd ";

  int errCnt = 0;

  // Arrays for mass deref
  int noOfObj   = 0;
  unsigned char            *ppBinaryKey   [OMS_MASS_OPERATION_CNT];
  size_t                    pKeyLen       [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8              pOid_u        [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8              *pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef             pObjVers      [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo             pUpdTransId   [OMS_MASS_OPERATION_CNT];
  OMS_ContainerEntry       *ppContainerInfo[OMS_MASS_OPERATION_CNT];
  tgg01_ContainerId         pContainerId  [OMS_MASS_OPERATION_CNT];

  // arrays for locking
  int lockCnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pLockOid_u         [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8            *pLockOid = reinterpret_cast<OMS_ObjectId8*>(&pLockOid_u[0]);
  tgg91_PageRef           pLockObjVers       [OMS_MASS_OPERATION_CNT];
  tgg00_FileId           *ppLockContainerId  [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer     *ppLockContainerPtr [OMS_MASS_OPERATION_CNT];

  // exporting parameters of kernel call
  OmsAbstractObject        *ppObj         [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer       *ppObjContainer[OMS_MASS_OPERATION_CNT];
  size_t                    pObjSize      [OMS_MASS_OPERATION_CNT];
  tgg00_BasisError          pDBError      [OMS_MASS_OPERATION_CNT];

  OmsAbstractObject       **pppAddr       [OMS_MASS_OPERATION_CNT];
  const unsigned char      *ppKey         [OMS_MASS_OPERATION_CNT];
  long                     *ppErrAddr     [OMS_MASS_OPERATION_CNT];

  int DBErrorCnt = 0;
  while (bool(derefIter)){
    // Read packages of maximal size OMS_MASS_OPERATION_CNT from the kernel
    while (noOfObj<OMS_MASS_OPERATION_CNT && bool(derefIter)) {
      IncDerefKey();

      // Remember original key and address where to store the result
      OmsAbstractObject   **ppAddr   = derefIter.omsGetObjAddr();
      const unsigned char  *pKey     = derefIter.omsGetKey();
      long                 *pErrAddr = derefIter.omsGetErrAddr();

      // Get container info
      OMS_ContainerEntry*  pContainerInfo = m_context->GetContainerDir().GetContainerEntry(
                                                             derefIter.omsGetGuid(), 
                                                             derefIter.omsGetSchema(), 
                                                             derefIter.omsGetContainerNo());

      // Convert key into binary representation
      OmsObjectContainer *pObj         = pContainerInfo->GetMemory();
      unsigned char      *pBinaryKey   = pContainerInfo->GetClassEntry().GetKeyPtr(pObj);
      OmsAbstractObject  *pAbstractObj = &pObj->m_pobj;
      pAbstractObj->omsKeyToBinary(pKey, pBinaryKey);

      // Check whether object is already stored in the local oms-cache.
      // A key-access to the oms cache is possible if either objects are created
      // in a version, or the cached key feature is switched on.
      OmsObjectContainer* found = NULL;
      if (pContainerInfo->UseCachedKeys() || m_context->IsVersion()){
        found = pContainerInfo->VersionFindKey(pBinaryKey);
        if (found){
          // Statistics
          IncCacheHit(found->m_oid);

          // release memory
          pContainerInfo->ChainFree(pObj, 68);

          if (found->DeletedFlag()) 
            // Object is stored in local cache, but marked as deleted
            derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),
                                   NULL, pErrAddr, e_object_not_found);
          else {
            // Object is stored in the local cache
            if (doLock && !m_context->IsVersion() && !found->LockedFlag()){
              // remember necessary information for later mass lock
              pLockOid          [lockCnt] = found->m_oid;
              pLockObjVers      [lockCnt] = found->m_objseq;
              ppLockContainerId [lockCnt] = &pContainerInfo->GetFileId();
              ppLockContainerPtr[lockCnt] = found;
              ++lockCnt;
            }

            derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),
                                   ForUpdPtr(found), pErrAddr, e_ok);
          }
        }
        else if (pContainerInfo->UseCachedKeys() && pContainerInfo->IsCacheMiss(pBinaryKey)) {
          // Statistics
          IncCacheMiss();

          // release memory
          pContainerInfo->ChainFree(pObj, 69);

          derefIter.omsSetResult(ppAddr, pKey, pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(), 
                                 NULL, pErrAddr, e_object_not_found);
        }
      }

      if (!found){
        // Remember data which is needed to store the result after the deref
        pppAddr     [noOfObj]        = ppAddr;
        ppKey       [noOfObj]        = pKey;
        ppErrAddr   [noOfObj]        = pErrAddr;

        // insert key into array for mass deref            
        ppBinaryKey [noOfObj]        = pBinaryKey;
        pObjVers    [noOfObj].gg91SetNilRef();
        ppContainerInfo[noOfObj]     = pContainerInfo;
        pContainerId[noOfObj]        = pContainerInfo->GetFileId(); 
        pKeyLen     [noOfObj]        = pContainerInfo->GetClassEntry().GetKeyDesc().GetLen();
        pObjSize    [noOfObj]        = pContainerInfo->GetClassEntry().GetTransientSize();  

        // Get object frame to store the objects
        ppObjContainer[noOfObj]     = pObj;

        // Determine position where the kernel should write the object to. As the vtbl-pointer
        // is stored at the beginning of the abstract object the position must be incremented
        // by the size of a pointer.)
        ppObj[noOfObj] = (OmsAbstractObject*) ((unsigned char*) (&ppObjContainer[noOfObj]->m_pobj) + sizeof(void*));
        ++noOfObj;
      }

      ++derefIter;
    }

    if (lockCnt == OMS_MASS_OPERATION_CNT || (lockCnt > 0 && !bool(derefIter))) {
      // Mass lock
      tsp00_Int8 timeout = 0;
      if (!shared){
        OMS_HResult hr = m_lcSink->LockObjMass( lockCnt, &m_context->m_consistentView, ppLockContainerId,
                                               (SAPDB_UInt8*)pLockOid, pLockObjVers, timeout, errCnt, pDBError);
      }
      else {
        OMS_HResult hr = m_lcSink->LockSharedObjMass( lockCnt, &m_context->m_consistentView, ppLockContainerId,
                                                     (SAPDB_UInt8*)pLockOid, pLockObjVers, timeout, errCnt, pDBError);
      }
      m_context->LockResult (lockCnt, pDBError, ppLockContainerPtr, pOid, msg, shared); 
      lockCnt = 0;
    }

    if (noOfObj == OMS_MASS_OPERATION_CNT || (noOfObj > 0 && !bool(derefIter))) {
      // mass deref of objects which are not already stored in the local oms-cache
      int errorCnt = m_context->LoadObjsViaKeyFromLCBase(noOfObj, pKeyLen, ppBinaryKey, pOid, 
                        pObjVers, pUpdTransId, doLock, shared, ppObjContainer, pDBError, ppObj, ppContainerInfo, 
                        pContainerId, pObjSize);

      DBErrorCnt += errorCnt;

      for (int i=0; i<noOfObj; ++i){
        // Append object, that have been read from the kernel, to the output
        derefIter.omsSetResult(pppAddr[i], ppKey[i] , ppContainerInfo[i]->GetClassEntry().GetKeyDesc().GetLen(), 
                               ppObjContainer[i] == NULL ? NULL : ForUpdPtr(ppObjContainer[i]), 
                               ppErrAddr[i], pDBError[i]);

        if (ppContainerInfo[i]->UseCachedKeys() && ppObjContainer[i] != NULL){
          // Insert object into search structure for cached keys
          ppContainerInfo[i]->VersionAddKey(ppObjContainer[i], true);
        }
      }
      noOfObj = 0;
    }
  }

  return DBErrorCnt;
}

/*----------------------------------------------------------------------*/

/// Releases locks which have been acquired
/*!
** This method releases the first 'cnt' locks which are enumerated by the
** iterator 'pOid'. This functionality is used by LockObjMass in case of an error.
*/
void OMS_Session::ReleaseLocks (OmsIOidReadIterator &oids, int cnt, bool shared)
{
  oids.omsReset();
  for (int j = 0; j < cnt; ++j) {
    OMS_ObjectId8 currErrOid(oids.omsGet());
    try {
      if (!CurrentContext()->FindObjInContext(&currErrOid))
        UnlockObj(currErrOid, shared);
    }
    catch (DbpError &e){
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtError(e);
      }  
    }
    ++oids;
  }
}

/*----------------------------------------------------------------------*/
// PTS 1115027
// PTS 1121449
void OMS_Session::NewConsistentView (OmsIOidReadIterator &exclOids, 
                                     OmsIOidReadIterator &sharedOids,
                                     short                timeout, 
                                     OmsIOidAppender     &exclErrOids,
                                     OmsIOidAppender     &sharedErrOids)
{
  const char * msg = "OMS_Session::NewConsistentView ";

  // A version always keeps her initial consistent view as long as it exists
  if (CurrentContext()->IsVersion()) {
    return;
  }

  // Get locks on all objects for which there are before-images but which are not locked.
  // PTS 1128108
  CurrentContext()->LockObjWithBeforeImage();

  CurrentContext()->RemoveUnlockedObjFromCache();
  CurrentContext()->ClearReadOnlyObjCache(/*deleteObjects=*/true);
 
  // All objects, which can not be locked in the kernel are inserted into a tree. Later
  // only those objects, which are not found in this tree are dereferenced for update 
  // to ensure a pseudo update (see below).
  OMS_ObjectId8 cmp;
  cgg250AvlTree<OMS_ObjectId8,OMS_ObjectId8,OMS_Session> unlockedObjects;
  unlockedObjects.AdviseAllocator(this);
  unlockedObjects.AdviseCompare(&cmp);

  short timeout1 = timeout;
  short timeout2 = 0;
  try {
    // Try to get a lock for each of the objects in pOid. timeout specifies the maximal time
    // for getting all locks. After the execution timeout contains the time needed to process
    // the call
    LockObjMass(exclOids,   timeout1, exclErrOids,   /*shared=*/false, /*newConsistentView=*/true, &unlockedObjects);
    timeout2 = timeout - timeout1;
    LockObjMass(sharedOids, timeout2, sharedErrOids, /*shared=*/true,  /*newConsistentView=*/true);
  }
  catch (OmsLockTimeout&){ 
    // Write statistics
    MonitorWaitNewConsistentView((tsp00_Int4)(timeout1+timeout2));
    throw;
  }

  // Write statistics
  timeout = timeout1+timeout2; // return duration of operation
  MonitorWaitNewConsistentView((tsp00_Int4)timeout); 
  
  // Register new consistent view in the kernel
  CurrentContext()->NewConsistentView();

  // Dereference object with lock. This is needed to ensure, that a pseudo-
  // update is send to the kernel during the oms-commit. This is only needed
  // for exclusive locks, as the pseudo-update for shared locks is done
  // implicitly in the kernel.   PTS 1135545
  // TODO: If locking without dereferencing the object into the liboms is
  // implemented, this should be used instead here.
  exclOids.omsReset();
  while (bool(exclOids)) {
    const OMS_ObjectId8 &oid = exclOids.omsGet();
    if (!unlockedObjects.Find(oid)){
      m_context->GetObj(oid, /*doLock=*/true, /*shared=*/false);
    }
    ++exclOids;
  }
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OMS_Session::NewKeyedObject(ClassID     guid, 
                                                   const unsigned char* key, 
                                                   OmsSchemaHandle      Schema, 
                                                   OmsContainerNo       ContainerNo)
{
  const char* msg = "OMS_Session::NewKeyedObject";

  m_monitor.IncNewKeyedObject();

  bool        writeNewBeforeImage = true;
  bool        incNewObjectCnt     = true;

  HRESULT hr;
  OMS_ContainerEntry *pContainerInfo = m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo);
  OmsObjectContainer *po = pContainerInfo->GetMemory(m_context->IsVersion());

  // transform key into binary representation
  unsigned char* pBinaryKey = pContainerInfo->GetClassEntry().GetKeyPtr(po);
  // introduce pAbstractObject, without this code call will not be virtual on some platforms
  OmsAbstractObject* pAbstractObject = &po->m_pobj;
  pAbstractObject->omsKeyToBinary(key, pBinaryKey);

  if (m_context->IsVersion()) {
    int                   ObjHistReadCount;
    OmsObjectContainerPtr found;
    found = pContainerInfo->VersionFindKey(pBinaryKey);
    bool keyInCache = (NULL != found);
    if (NULL != found && m_context->IsVersionOid(found->m_oid)) { // PTS 1117571
      if (!found->DeletedFlag()) {
        pContainerInfo->ChainFree(po, 70);
        ThrowDBError(e_duplicate_hash_key, msg, found->m_oid, __MY_FILE__, __LINE__);
      }
    }
    else {
      // Check whether the object exists already in the kernel
      found = NULL;
      tsp00_Int2  DBError;
      tgg91_TransNo dummy_updTransId;
      hr = m_lcSink->GetObjWithKey (
        (unsigned char*) &m_context->m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        (unsigned char*) m_context->VersionContext(),
        pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(), 
        (unsigned char*) pBinaryKey, 
        /*doLock=*/false, /*shared=*/false, 
        pContainerInfo->GetClassEntry().GetPersistentSize(), 
        ((unsigned char*) &po->m_pobj) + sizeof(void*),
        REINTERPRET_CAST(SAPDB_UInt8*, &po->m_oid), 
        (unsigned char*) &po->m_objseq, 
        &dummy_updTransId,
        &ObjHistReadCount, 
        &DBError);
      if FAILED (hr) {
        pContainerInfo->ChainFree(po, 71);
      }
      if ( DBError != 0 ) {
        if ( e_hash_key_not_found != DBError ) {
          pContainerInfo->ChainFree(po, 72);
          ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
        }
      }
      else {
        this->IncLogHop(ObjHistReadCount);
        OmsObjectContainerPtr found = m_context->FindObjInContext(&po->m_oid, true /*ignoreGeneration*/);  // PTS 1125361
        if (found) {
          if (!found->DeletedFlag()) {
            // Object exists locally and is not marked as deleted
            pContainerInfo->ChainFree(po, 73);
            ThrowDBError(e_duplicate_hash_key, msg, found->m_oid, __MY_FILE__, __LINE__);
          }
        }
        else {
          // Object exists in the kernel 
          OMS_ObjectId8 oid = po->m_oid;
          pContainerInfo->ChainFree(po, 74);
          ThrowDBError(e_duplicate_hash_key, msg, oid, __MY_FILE__, __LINE__);
        }
      }
    }
    po->m_oid = m_context->VersionNewOid();
    po->m_objseq.gg91SetNilRef();
    
    if (pContainerInfo->UseCachedKeys()) { 
        // Check if object is registerd as cache-miss and if so 
        // remove it from cache miss structure
        if (pContainerInfo->IsCacheMiss(pBinaryKey)) {
            pContainerInfo->DeleteCacheMiss(pBinaryKey);
        }
    }
    
    if (keyInCache){
        // replace pointer to key 
        pContainerInfo->VersionReplaceOrAddKeyContainerPtr(po);
    }
    else{
        // Always add entry to cached key structure, because this object
        // is created in a version
        pContainerInfo->VersionAddKey(po); 
    }

    m_context->PutObjectIntoContext (po, pContainerInfo->GetContainerHandle());
  }

  else {  // no version
    // Create new object in kernel
    tsp00_Int2  DBError1;
    tgg91_TransNo updTransId;
    HRESULT hr = m_lcSink->NewObj (
      (unsigned char*) &m_context->m_consistentView,
      (unsigned char*) &pContainerInfo->GetFileId(),
      (unsigned char*) m_context->VersionContext(),
      pContainerInfo->GetClassEntry().GetKeyDesc().GetPos(), 
      pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(), 
      (unsigned char*) pBinaryKey,
      false /*contObj*/,
      REINTERPRET_CAST(SAPDB_UInt8*, &po->m_oid), 
      (unsigned char*) &po->m_objseq,
      &updTransId,
      &DBError1);

    if (0 == DBError1) {
      if (pContainerInfo->UseCachedKeys()) {
        // Check whether object exists already in the context.
        // This might be possible in the following scenario
        // Tr 1:  Create(a) Commit         Delete(a) Commit
        // Tr 2:                   Read(a)                  Create(a)
        OmsObjectContainerPtr found = pContainerInfo->VersionFindKey(pBinaryKey);
        if (found) {
          //m_context->DeleteObjInContext(found, pContainerInfo);
          pContainerInfo->VersionDelKey(found);
        }

        // Check if object is registerd as cache-miss and if so 
        // remove it from cache miss structure
        if (pContainerInfo->IsCacheMiss(pBinaryKey)) {
          pContainerInfo->DeleteCacheMiss(pBinaryKey);
        }
        
        // Insert object into cached key structure
        pContainerInfo->VersionAddKey(po);  
      }

      po->SetRevisionNumber(updTransId);
      m_context->PutObjectIntoContext (po, pContainerInfo->GetContainerHandle());
    }

    else if (e_duplicate_hash_key == DBError1 ) {
      // Check, if current transcation has already deleted object.
      int            dummy;
      tsp00_Int2     DBError2;
      tgg91_TransNo  updTransId;
      hr = m_lcSink->GetObjWithKey (
        (unsigned char*) &m_context->m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        (unsigned char*) m_context->VersionContext(),
        pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(), 
        CONST_CAST(unsigned char*, pBinaryKey), 
        /*doLock=*/false, /*shared=*/false,
        pContainerInfo->GetClassEntry().GetPersistentSize(), 
        ((unsigned char*) &po->m_pobj) + sizeof(void*),
        REINTERPRET_CAST(SAPDB_UInt8*, &po->m_oid),
        (unsigned char*) &po->m_objseq,
        &updTransId,
        &dummy,
        &DBError2);

      if (0 == DBError2) {
        // Get pointer to locally stored object
        OmsObjectContainer* found  = m_context->FindObjInContext(&po->m_oid, true /*ignoreGeneration*/);  // PTS 1125361
        if (found) {
           if (found->DeletedFlag()) {
              // Use a new frame for the new object although there is an object frame
              // of a deleted object. The generation counter of the oid of the new object 
              // will be changed, so that the oids are different.
              found->MarkReplaced();
              po->m_oid = found->m_oid; 
              po->MarkReused();   /* PTS 1127661 */
              ++po->m_oid;
              po->SetRevisionNumber(updTransId);

              if (found->IsNewObject()) {
                // oid has already been counted as new
                incNewObjectCnt = false;
              }

              if (pContainerInfo->UseCachedKeys()) {
                // Adapt pointer in Cached-Key tree.
                // Check if object is registerd as cache-miss and if so 
                // remove it from cache miss structure...
                if (pContainerInfo->IsCacheMiss(pBinaryKey)) {
                  pContainerInfo->DeleteCacheMiss(pBinaryKey);
                }
              
                // Replace or insert new object frame into cached key structure
                pContainerInfo->VersionReplaceOrAddKeyContainerPtr(po);  
              }

              // Insert object into the oid-hash
              m_context->PutObjectIntoContext (po, pContainerInfo->GetContainerHandle());
           }
           else {
             // Object does already exist, is stored in the cache and is not marked as deleted
             pContainerInfo->ChainFree(po, 75);
             ThrowDBError(e_duplicate_hash_key, msg, found->m_oid, __MY_FILE__, __LINE__);
             return NULL;
           }
        }  
        else {
          // Object does already exist in the kernel
          OMS_ObjectId8 oid = po->m_oid;
          pContainerInfo->ChainFree(po, 76);
          ThrowDBError(e_duplicate_hash_key, msg, oid, __MY_FILE__, __LINE__);
          return NULL;
        }
      }
      else if (e_hash_key_not_found == DBError2) {
        // 1.) Object is already created and committed by a concurrent running transaction,
        //     but because of the consistent view, this object is not visible by the own
        //     transaction.
        // 2.) Object was deleted by another transaction between the calls of 'NewObj' and 
        //     the 'GetObjWithKey' of this transaction. 
        //     As a 'e_hash_key_not_found' is not expected, when creating a new object,  
        //     this error is mapped to a e_duplicate_hash_key. 
        //     Trans 1: create(a,k) commit              del(a) commit
        //     Trans 2:                    |----------- NewKeyedObject(b,k) -----------|
        //                                  NewObj(b,k)              GetObjWithKey(k)
        //                                  dup_key                  not_found
        OMS_ObjectId8 oid = po->m_oid;
        pContainerInfo->ChainFree(po, 77);
        ThrowDBError(e_duplicate_hash_key, msg, oid, __MY_FILE__, __LINE__);
        return NULL;
      }
      else {
        // Unhandled error occured while executing GetObjWithKey
        OMS_ObjectId8 oid = po->m_oid;
        pContainerInfo->ChainFree(po, 78);
        ThrowDBError(DBError2, msg, oid, __MY_FILE__, __LINE__);
        return NULL;
      }
    }

    else if (e_key_still_visible == DBError1){
      // The object is existing in the current consistent view, but a concurrent running
      // transaction has deleted the object and is committed. 
      //  Trans 1: read(a)                 new(a)
      //  Trans 2:          del(a) commit
      OMS_ObjectId8 oid = po->m_oid;
      pContainerInfo->ChainFree(po, 79);
      ThrowDBError(DBError1, msg, oid, __MY_FILE__, __LINE__);
      return NULL;
    }

    else if (  e_OMS_request_timeout == DBError1    // PTS 1124935
            || e_OMS_lock_collision  == DBError1){  // PTS 1124935
      // The keyed object can not be created because another session has 
      // already created an object with the same key, but without having 
      // committed the creation.
      OMS_ObjectId8 oid = po->m_oid;
      pContainerInfo->ChainFree(po, 80);
      ThrowDBError(e_hash_key_in_use, msg, oid, __MY_FILE__, __LINE__);
      return NULL;
    }

    else {
      // Unhandled error occured while executing NewObj
      pContainerInfo->ChainFree(po, 81);
      ThrowDBError(DBError1, msg, __MY_FILE__, __LINE__);
      return NULL;
    }
  }

  // Set flags in the object container
  po->MarkLocked();
  po->MarkStored();
  po->MarkAsNew();
  if (writeNewBeforeImage) {
    // A new object frame is used and therefore write a before image, which 
    // indicates, that on rollback, the object should be deleted
    InsertNewBeforeImage (po, pContainerInfo);
  }
  if (incNewObjectCnt) {
    // If an object with the same key has been created and deleted in the same
    // transaction before creating this object, then only the new object will
    // be flushed (resp. the old object frame is reused) and as the new-counter 
    // is already incremented for the old object a new increment is not needed.
    m_context->IncNewObjectsToFlush(po);
  }

  OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink, "OMS_Session::NewKeyedObject : "
    << (incNewObjectCnt ? "to flush: " : "no flush: ")
    << po->m_oid << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
  
  return &po->m_pobj;
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OMS_Session::NewObject(ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo, int arrayByteSize)
{
  const char* msg = "OMS_Session::NewObject";

  AssertNotReadOnly(msg);

  m_monitor.IncNewObject();

  bool incNewObjectCnt  = true;

  OMS_ContainerEntry *pContainerInfo = NULL;
  if (arrayByteSize == 0){
    pContainerInfo = m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo);
  }
  else {
    pContainerInfo = m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo, arrayByteSize);
  }

  OmsObjectContainer *po = pContainerInfo->GetMemory(m_context->IsVersion());

  if (InVersion()) {
    po->m_oid = m_context->VersionNewOid();
    po->m_objseq.gg91SetNilRef();
  }
  else {
    OmsObjectContainer *found = NULL;
    while (found == NULL && pContainerInfo->ExistsReusableOid()){
      // Reuse oid of deleted object  PTS 1127661
      po->m_oid = *pContainerInfo->GetReusableOid();
      found     = m_context->FindObjInContext(&po->m_oid, true /*ignoreGeneration*/);
      if (found != NULL && found->DeletedFlag() && found->m_oid == po->m_oid){
        found->MarkReplaced();
        po->MarkReused();
        po->m_objseq = found->m_objseq;
        ++po->m_oid;
        if (found->IsNewObject()){
          incNewObjectCnt = false;
        }
        OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink,
          "OMS_Session::NewObject : reuse oid : " << found->m_oid << " IsNew=" << (found->IsNewObject()?"Y":"N"));
      }
      else {
        OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink,
          "OMS_Session::NewObject : reuse oid : " << po->m_oid << " skipped");
        found = NULL;
      }
    }

    if (found == NULL) {
      // There is no oid which can be reused, so get new oid from the kernel
      tsp00_Int2     DBError;
      tgg91_TransNo  updTransId;
      OMS_HResult hr = m_lcSink->NewObj (
        (unsigned char*) &m_context->m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        (unsigned char*) m_context->VersionContext(),
        0,
        0,
        NULL,
        false /*contObj*/,
        REINTERPRET_CAST(SAPDB_UInt8*, &po->m_oid), 
        (unsigned char*) &po->m_objseq,
        &updTransId,
        &DBError);
      if ( DBError != 0 ) {
        pContainerInfo->ChainFree(po, 82);
        this->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
      }
      else {
        po->SetRevisionNumber(updTransId);
      }
    }
  }
  po->MarkLocked();
  po->MarkStored();
  po->MarkAsNew();
  this->IncStore(); 
  m_context->PutObjectIntoContext (po, pContainerInfo->GetContainerHandle());
  this->InsertNewBeforeImage (po, pContainerInfo);
  
  if (incNewObjectCnt){
    // As a newly created object has been reused, the counter must not be incremented.
    m_context->IncNewObjectsToFlush(po);
  }

  OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink, "OMS_Session::NewObject : " 
    << (incNewObjectCnt ? "to flush: " : "no flush: ")
    << po->m_oid << ", class: " << pContainerInfo->GetClassEntry().GetClassName());

  return &po->m_pobj;
}

/*----------------------------------------------------------------------*/

OMS_ObjectId8 OMS_Session::NewVarObject(OmsSchemaHandle Schema, OmsContainerNo ContainerNo, ClassID guid)
{
  const char* msg = "OMS_Session::NewVarObject";
  bool incNewObjectCnt = true;
  AssertNotReadOnly(msg);

  m_monitor.IncNewObject();

  OMS_ContainerEntry *pContainerInfo = m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo);
  OmsObjectContainer *po             = pContainerInfo->GetMemory(InVersion());

  if (InVersion()){
    po->m_oid = m_context->VersionNewOid(); 
    po->m_objseq.gg91SetNilRef();

    OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink,
       "OMS_Session::NewVarObject : new version oid : " << po->m_oid);
  }
  else {
    OmsObjectContainer *found = NULL;
    while (found == NULL && pContainerInfo->ExistsReusableOid()){
      // Reuse oid of deleted object  PTS 1127661
      po->m_oid = *pContainerInfo->GetReusableOid();
      found     = m_context->FindObjInContext(&po->m_oid, true /*ignoreGeneration*/);
      if (found != NULL && found->DeletedFlag() && found->m_oid == po->m_oid){
        found->MarkReplaced();
        po->MarkReused();
        po->m_objseq = found->m_objseq;
        ++po->m_oid;
        if (found->IsNewObject()){
          incNewObjectCnt = false;
        }
        OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink,
          "OMS_Session::NewVarObject : reuse oid : " << found->m_oid << " IsNew=" << (found->IsNewObject()?"Y":"N"));
      }
      else {
        OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink,
          "OMS_Session::NewVarObject : reuse oid : " << po->m_oid << " skipped");
        found = NULL;
      }
    }

    if (found == NULL) {
      tgg00_BasisError DBError;
      tgg91_TransNo    updTransId;
      OMS_HResult hr = m_lcSink->NewObj (
        (unsigned char*) &m_context->m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        (unsigned char*) m_context->VersionContext(),
        0, 0, NULL,
        false /*contObj*/,
        REINTERPRET_CAST(SAPDB_UInt8*, &po->m_oid),
        (unsigned char*) &po->m_objseq,
        &updTransId,
        &DBError);
      if ( DBError != 0 ) {
        pContainerInfo->ChainFree(po, 83);
        this->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
      }
      else {
        po->SetRevisionNumber(updTransId);
        OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink,
          "OMS_Session::NewVarObject : new kernel oid : " << po->m_oid);
      }
    }
  }
  po->MarkLocked();
  po->MarkVarObj();
  po->MarkAsNew();
  OMS_VarObjInfo* pObjInfo = new(&po->m_pobj) OMS_VarObjInfo(po->m_oid, 0, po->m_objseq);
  m_context->PutObjectIntoContext (po, pContainerInfo->GetContainerHandle());
  this->InsertNewBeforeImage (po, pContainerInfo);
  
  if (incNewObjectCnt){
    // As a newly created object has been reused, the counter must not be incremented.
    m_context->IncNewObjectsToFlush(po);
  }

  OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink, "OMS_Session::NewVarObject : "
    << (incNewObjectCnt ? "to flush: " : "no flush: ")
    << po->m_oid << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
  
  return po->m_oid;
}

/*----------------------------------------------------------------------*/

void OMS_Session::RegClass(ClassID            guid,
                           const char        *pClassName,
                           size_t             objectSize,
                           const OMS_KeyDesc &keyDesc,
                           ClassID            baseGuid,
                           OmsAbstractObject *pObj,
                           bool               isVarObject,
                           bool               isArrayObject)
{
  struct VirtualObject {
    void* vtptr;
  }; 

  GetClassDir().RegisterClass(pClassName, guid, baseGuid, 
    keyDesc, objectSize, 
    reinterpret_cast<VirtualObject*>(pObj)->vtptr,
    isVarObject, isArrayObject);

  if (!isVarObject){
    OMS_ClassEntry *pClassInfo = GetClassDir().GetClassEntry(guid);
    OmsObjectContainer *p = OmsObjectContainer::containerPtr(pObj);
    pClassInfo->ChainFree(p, 84);
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::RegisterCallbackInterface(ClassID guid, OmsCallbackInterface* pInterface)
{
  if (NULL != m_callbackInterface) {
    if (m_callbackInterfaceGuid == guid) {
      return;
    }
    ReleaseCallbackInterface();
  }
  m_callbackInterface     = pInterface;
  m_callbackInterfaceGuid = guid;
}

/*----------------------------------------------------------------------*/

void OMS_Session::ReleaseVarObject(const OMS_ObjectId8& oid)
{
  OmsObjectContainerPtr found = m_context->FindObjInContext (&oid);
  if (NULL != found) {
    OMS_VarObjInfo* pObjInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &found->m_pobj);
    pObjInfo->unlock();
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::RemoveFromTransVersion(OMS_Context* context)
{
  for (cgg251dclIterator<OMS_Context*,OMS_Session> iter = m_versionsBoundToTrans.begin(); iter; ++iter) {
    if (*iter() == context) {
      m_versionsBoundToTrans.remove(iter);
      return;
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::ReleaseCallbackInterface()
{
  // PTS 1128148
  // Ensure, that callback interface is set to null before calling 'omsDestroySelf'.
  OmsCallbackInterface *pInterface = m_callbackInterface;
  m_callbackInterface = NULL;
  if (NULL != pInterface) {
    try {
      pInterface->omsDestroySelf();
    } 
    catch (DbpError &e) {
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtError(e);
      }

      DbpBase(m_lcSink).dbpOpError("omsDestroySelf failed with error %d", e.dbpError());
      // make sure we don't have partially destroyed class as callback
      // this makes memory leaks possible
    } 
    catch (BAD_ALLOC_GEO573&) {
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtBadAlloc();
      }

      DbpBase(m_lcSink).dbpOpError("omsDestroySelf failed with BAD_ALLOC");
      // make sure we don't have partially destroyed class as callback
      // this makes memory leaks possible
    } 
    //m_callbackInterface = NULL; // PTS 1128148
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::SetCurrentMonitorMethod(tgg01_COMMonitorInfo* p) 
{
  /* PTS 1107731 */
  tsp00_8ByteCounter heapUsed;
  m_lcSink->GetSessionHeapUsage(heapUsed);
  // start of procedure call
  m_heapAtMethodBegin = heapUsed;
  m_monitor_curr      = p;
  m_monitor.Reset();
}
 
/*----------------------------------------------------------------------*/

void OMS_Session::SetReadOnly(bool readOnly)
{
  const char *msg = "OMS_Session::SetReadOnly";

  if (IsReadUnchangedObjectsActive()){
    // if this mode is active, then read-only is always true and it is not allowed
    // to change this. But after the mode is ended, then the previous value is
    // restored. 
    if (readOnly == false){
      ThrowDBError (e_OMS_missing_privilege, msg, __MY_FILE__, __LINE__);
    }
    else {
      // After the mode is ended, the session will be in read-only mode 
      m_read_only_old = true;
    }
  }
  else {
    if (readOnly && !m_allowReadOnly){
      ThrowDBError (e_OMS_missing_privilege, msg, __MY_FILE__, __LINE__);
    }
    m_read_only = readOnly;
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::SetTimeout(short timeout)
{
  short       DummyError;
  tsp00_Int4  BufLen = sizeof(timeout);
  this->m_timeout = timeout;
  OMS_HResult hr = m_lcSink->MultiPurpose (m_set, mm_lock_timeout, &BufLen, 
    (unsigned char*) &timeout, &DummyError);
}

/*----------------------------------------------------------------------*/

void OMS_Session::ThrowDBError(tsp00_Int2 e, const char* msg, const OmsVersionId& v, const char* pFile, unsigned int line)
{
    char resultMsg[256];
    int msgLen = (int) strlen(msg);
    if (msgLen < sizeof(resultMsg) - sizeof(v) - 2)
    {
        char* pChar = &resultMsg[0];
        SAPDB_MemCopyNoCheck(pChar, &msg[0], msgLen);
        pChar += msgLen;
        *pChar = ' ';
        ++pChar;
        SAPDB_MemCopyNoCheck(pChar, &v, sizeof(v));
        pChar += sizeof(v);
        *pChar = 0;
        OMS_Globals::Throw(e, &resultMsg[0], pFile, line, this);
    }
    else
    {
        OMS_Globals::Throw(e, msg, pFile, line, this);
    }
}
  
/*----------------------------------------------------------------------*/

void OMS_Session::ThrowDBError(tsp00_Int2 e, const char* msg, const char* pFile, unsigned int line)
{
  OMS_Globals::Throw(e, msg, pFile, line, this);
}

/*----------------------------------------------------------------------*/

void OMS_Session::ThrowDBError(tsp00_Int2 e, const char* msg, const OMS_ObjectId8& oid, const char* pFile, unsigned int line)
{
  OMS_Globals::Throw(e, msg, oid, pFile, line, this);
}

/*----------------------------------------------------------------------*/

void OMS_Session::Trace( const char*fmt, ... )
{
#if defined(WIN32)
  char buffer[256];
#else
  char buffer[1024];
#endif

  va_list args;
  va_start (args, fmt);
#if defined(WIN32)
  _vsnprintf ( buffer, sizeof(buffer), fmt, args );
#else
  vsprintf ( buffer, fmt, args );
  buffer[256] = '\0';
#endif
  m_lcSink->Vtrace ((tsp00_Int4) strlen(buffer), (unsigned char*) &buffer[0]);
  va_end (args);
}

/*----------------------------------------------------------------------*/

void OMS_Session::TransEnd()
{
  short DBError = 0;
  if (m_defaultContext->ExistsConsistentView()) {
    OMS_HResult hr = m_lcSink->EndConsistentView(
      (unsigned char*) &m_defaultContext->m_consistentView, &DBError);
  }

  //m_currVarObjChunk.Clear();
  //m_defaultContext->ResetNewObjectsToFlush();
  //m_isDataChanged = false;
  ResetDataChanged();

  if ( 0 != DBError ) { 
    this->ThrowDBError (DBError, "omsTransEnd", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::UnlockObj (const OMS_ObjectId8& oid, bool shared)
{
  tsp00_Int2               DBError;
  OMS_UnknownContainerId UnknownFileId; 
  if (!shared){
    OMS_HResult hr = m_lcSink->UnlockObj ((unsigned char*) &UnknownFileId,
            REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)), &DBError);
  }
  else {
    OMS_HResult hr = m_lcSink->UnlockSharedObj ((unsigned char*) &UnknownFileId,
            REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)), &DBError);
  }
  if ( DBError != 0 ) { 
    this->ThrowDBError (DBError, "OMS_Session::Unlock", oid, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

bool OMS_Session::VersionBoundByMe(OMS_Context* context)
{
  for (cgg251dclIterator<OMS_Context*,OMS_Session> iter = m_versionsBoundToTrans.begin(); iter; ++iter) {
    if (*iter() == context) {
      return true;
    }
  }
  return false;
}

/*----------------------------------------------------------------------*/

void OMS_Session::Wait()
{
  tsp00_Bool ok;
  this->IncCntWaitOmsLockObj();
  tsp00_Int4 waitStart = OMS_Globals::GetKernelInterface()->Clock();
  m_lcSink->Wait(ok);
  this->IncWaitOmsLockObjSecs(OMS_Globals::GetKernelInterface()->Clock() - waitStart);
  if (!ok)
  {
    this->ThrowDBError(e_OMS_lock_collision, "OMS_Session::Wait", OMS_ObjectId8(), __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

OMS_Session::MethodCallEplilogScope::MethodCallEplilogScope(OMS_Session& session)
  : m_session(session)
{
  m_session.m_inMethodCallEpilog = true;
}

/*----------------------------------------------------------------------*/

OMS_Session::MethodCallEplilogScope::~MethodCallEplilogScope()
{
  m_session.m_inMethodCallEpilog = false;
}

/*----------------------------------------------------------------------*/

void OMS_Session::TransEndEpilog(bool isCommit, int &phase)
{
  // In versions the level 1 represents the 'normal' transaction (in contrast to 
  // subtransactions) and therefore this level is committed / rolled back after the
  // kernel commit has been executed.
  if (isCommit){
    phase = 10;
    CommitSubtrans(1);  
  }
  else {
    phase = 20;
    RollbackSubtrans(1, /*callFromRollbackTrans=*/true);
  }

  // The before images of the level 1 haven't been freed so far.
  phase = 30;
  FreeBeforeImages(1);

  // cleanup the versions, which are bound to the session
  phase = 40;
  ClearTransVersions(isCommit, phase);

  // 'Commit' / 'Rollback' changes in the container-directory.
  phase = 50;
  m_defaultContext->GetContainerDir().CleanupAfterTransEnd(isCommit);

  // As the before images for the container-directory are stored in normal frames,
  // the frames must not be freed before the commit / rollback of the container
  // has been finished.
  phase = 60;

  // In the simulator we have to free the var object data after a rollback 
  if(OMS_Globals::m_globalsInstance->InSimulator() && !isCommit)
    m_defaultContext->ClearVarObjData();

  ClearDefaultContext();

  // Release the lockobjects, which are held by the session
  phase = 70;
  m_lockObjects->TransactionEnd();

  phase = 80;
  m_rwLockDir.transactionEnd();

  m_currVarObjChunk.Clear();
  m_defaultContext->ResetNewObjectsToFlush();
  ResetDataChanged();
  ResetSubtransLevel();
  
  phase = 90;
  // Reset the flag, which indicated, that the error was provoked by the test-kernel
  SetTestKnlExceptionActive(false);
}

/*----- PTS 1135058, PG ------------------------------------------------*/

void OMS_Session::setCurrentMethod(unsigned char* name)
{
  // Store name of currently running routine in ASCII7 representation
  // with null-terminator in m_currMethod.

  memset( m_currMethod, 0, sizeof(m_currMethod) );

  if (OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance()) {
    tsp00_Uint4           destBytesWritten;
    tsp00_Uint4           srcBytesParsed;
    tsp78ConversionResult error;
    error = sp78convertString( sp77encodingAscii,            /* destEndcoding           */
                               m_currMethod,                 /* destBuffer              */
                               sizeof(m_currMethod) - 1,     /* destBufferLengthInBytes */
                               &destBytesWritten,            /* destBytesWritten        */
                               false,                        /* addZeroTerminator       */
                               sp77encodingUCS2,             /* srcEncoding             */
                               name,                         /* srcBuffer               */
                               sizeof(tsp00_KnlIdentifier),  /* srcBufferLengthInBytes  */
                               &srcBytesParsed );            /* srcBytesParsed          */

    if (sp78_Ok == error) {
      int pos = destBytesWritten - 1;
      while ( (0 <= pos) && (' ' == m_currMethod[pos]) ) {
        pos--;
      }
      m_currMethod[pos+1] = 0;
    }
    else {
      memset( m_currMethod, 0, sizeof(m_currMethod) );
    }
  }
}

/*----- PTS 1135058, PG ------------------------------------------------*/

void OMS_Session::resetCurrentMethod()
{
  memset( m_currMethod, 0, sizeof(m_currMethod) );
}

/*----------------------------------------------------------------------*/

void OMS_Session::RescheduleAndCheckCancelFlag()
{
  if (IsCancelled()) 
  {
    SetCancelFlag(false);
    ThrowDBError(e_OMS_cancelled, "RescheduleAndCheckCancelFlag", __MY_FILE__, __LINE__);
  }
  m_rescheduleDistance = RESCHEDULE_DISTANCE;
  m_lcSink->Reschedule();
}

/*----------------------------------------------------------------------*/

 
