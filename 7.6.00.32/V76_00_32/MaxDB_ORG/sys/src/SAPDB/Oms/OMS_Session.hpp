/*!
 * \file    OMS_Session.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS session
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

#ifndef __OMS_SESSION_HPP
#define __OMS_SESSION_HPP

#include "Oms/OMS_SessionDef.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_ISessionLockObjects.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_FreeListHeader.hpp"
#include "Oms/OMS_SinkCriticalSection.hpp"
#include "Oms/OMS_RWLock.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

class OMS_SessionCriticalSection : public OMS_SinkCriticalSection {
public:
  OMS_SessionCriticalSection(OMS_Session* session,int regionId) 
    : OMS_SinkCriticalSection(session->m_lcSink, regionId) 
  {} 
};

/*----------------------------------------------------------------------*/
/*               Implementation of OMS_Session                      */
/*----------------------------------------------------------------------*/

inline void* OMS_Session::operator new(size_t sz)
{
  return OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(sz);
}

/*----------------------------------------------------------------------*/
#if defined(OMS_PLACEMENT_DELETE)
inline void OMS_Session::operator delete(void* p)
{
  REINTERPRET_CAST(OMS_Session*, p)->DeleteSession();
}
#endif

/*----------------------------------------------------------------------*/

inline void OMS_Session::ActivateDefaultContext() 
{
  SetContext(m_defaultContext);
  this->ChangedConsistentView();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::AssertNotReadOnly(const char* callerMsg) 
{
  if (m_read_only)
  {
    this->ThrowDBError (e_oms_read_only, callerMsg, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::ClientSupportsOptimizedStreams() const // PTS 1130281
{
    return m_optimizedStreamCommunication;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::CloseVersion()
{
  this->IncCloseVersion();
  m_context->CloseVersion();
  this->ActivateDefaultContext();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::ClearTransVersions(bool isCommit, int &phase)
{
  if (!m_versionsBoundToTrans.empty()) {

    cgg251dclIterator<OMS_Context*,OMS_Session> iter = m_versionsBoundToTrans.begin();

    while (iter) {
      cgg251dclIterator<OMS_Context*,OMS_Session> delIter = iter;
      OMS_Context* pContext = *iter();
      ++iter;

      // !"§$ ONLY FOR DEBUGGING
      if (pContext->GetSession() != this){
        DbpBase opMsg(m_lcSink);
        opMsg.dbpOpError("OMS_Session::ClearTransVersions: Session-pointer in context %s is wrong. (session=%p session-ptr=%p)",
                          pContext->GetVersionId(), this, pContext->GetSession());   
      }

      bool isDropped = false;
      { // Begin of exclusive critical section.    // PTS 1124533
        OMS_InternalLockScope scope(this, LIBOMS_RWLOCK_AREA, 
          OMS_Globals::m_globalsInstance->GetLockIdForVersionDirectory(pContext->GetVersionId()), true/*exclusive*/);

        phase = 41;
        // Remove objects which are marked as deleted
        pContext->FlushVersionObjCache(isCommit); 

        if (!pContext->IsVersionOpen()) {
          
          // If version has been marked as deleted by another transaction, after the version has
          // been closed by the current transaction, then the version is only marked as dropped.
          // As the version can now be released from the current transaction, drop is now.
          if (pContext->IsDropped()){
            phase = 42;
            // Remove entry from the version directory. As it now can't be accessed by other sessions,
            // the physical drop can be executed outside the critical section. 
            OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(m_lcSink, pContext);
            OMS_Globals::m_globalsInstance->m_versionDictionary.DropVersion(pContext->GetVersionId());
            isDropped = true;
          }
          else {
            phase = 43;
            // Remove container-entries which are not valid anymore
            pContext->GetContainerDir().CleanupAfterTransEnd(isCommit);

            phase = 44;
            // Release version from current transaction. 
            // (Reset session pointer, as version might be open in a different session the next time.)
            m_versionsBoundToTrans.remove(delIter);
            pContext->MarkNotBoundToTrans();
          }
        }
      } // End of exclusive critical section.
      
      if (isDropped){
        // Attention:
        // Normally the setting of the class-entry-pointer in the container-directory is
        // not allowed in the trans-end-processing. This should prevent, that the pointer
        // is pointing to the wrong session at the beginning of the next transaction. 
        // But as the version (together with all its objects) is dropped now, and for 
        // dropping the objects, the class properties must be read, it must be allowed to
        // set the class entry pointer if it was not already set during the transaction.
        pContext->GetContainerDir().AllowChangeOfClassEntryPtr();

        //// Release version from current transaction.
        phase = 45;
        m_versionsBoundToTrans.remove(delIter);

        phase = 46;
        pContext->DeleteSelf();
        IncDropVersion();
      }
    }
  }

  // clear list of created versions in transaction
  if (!m_createdVersionsInTrans.empty()) {
    phase = 47;
    cgg251dclIterator<OMS_Context*,OMS_Session> iter = m_createdVersionsInTrans.begin();
    while (iter) {
      cgg251dclIterator<OMS_Context*,OMS_Session> delIter = iter;
      ++iter;
      m_createdVersionsInTrans.remove(delIter);
    }
  }

  if (InVersion()){
    phase = 48;
    CheckHeapThresholds(); /* PTS 1110149 */
  }
}

/*----------------------------------------------------------------------*/

void OMS_Session::CheckHeapThresholds() /* PTS 1110149 */
{
  bool forceUnload = false;
  if ((OMS_Globals::KernelInterfaceInstance->HeapThresholdReached())
     || (m_context->HeapUsed() > (tsp00_8ByteCounter) (OMS_Globals::m_globalsInstance->GetOmsVersionThreshold() * 1024))
     || forceUnload)
  {
    m_context->ReduceHeapUsage();
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::CreateDefaultContext() 
{
  m_defaultContext = new OMS_Context(this);
  SetContext(m_defaultContext);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::CommitSubtrans(int requiredLevel)
{

  if (!IsSubtransOpen() && !InVersion()) {
	  return;
  }
  if (-1 == requiredLevel) {
    requiredLevel = this->CurrentSubtransLevel();
  }
  this->IncSubtransCommit();
  this->ExecuteSubtransCommit(requiredLevel);
}

/*----------------------------------------------------------------------*/

#if defined(OMSTST)
inline void OMS_Session::CheckCancelCountDown()
{
  if (m_cancelCountDown > 0)
  {
    --m_cancelCountDown;
    if (0 == m_cancelCountDown)
    {
       m_cancelCountDown         = -1;
       SetCancelFlag(true);
       SetCancelledByProgram(true);
       SetProvokedByTestkernel(true);
       SetTestKnlExceptionActive(true);
    }
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::CheckExceptionCountDown()
{
  if (m_exceptionCountDown > 0)
  {
    --m_exceptionCountDown;
    if (0 == m_exceptionCountDown)
    {
       m_exceptionCountDown      = -1;
       SetProvokedByTestkernel(true);
       SetTestKnlExceptionActive(true);
       ThrowDBError (e_requested_dbperror, "user requested exception", __MY_FILE__, __LINE__);
    }
  }
}
#endif

/*----------------------------------------------------------------------*/

inline OMS_Context* OMS_Session::CurrentContext() const
{
  return m_context;
}

/*----------------------------------------------------------------------*/

inline int OMS_Session::CurrentSubtransLevel() const 
{
  return m_subtrans_lvl;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::DecSubtransLevel()
{
  --m_subtrans_lvl;
}

/*----------------------------------------------------------------------*/

inline OmsAbstractObject* OMS_Session::DeRef (const OMS_ObjectId8& oid, ClassID guid, bool forUpd, bool doLock, bool shared) 
{
  const char* msg = "OMS_Session::DeRef";
   
  OmsObjectContainerPtr found = m_context->GetObj (guid, oid, doLock, shared);
  if (found) {
    return forUpd ? ForUpdPtr(found)
                  : &found->m_pobj;
  }
  else {
    this->ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    return NULL;
  }
}

/*----------------------------------------------------------------------*/

inline OmsAbstractObject* OMS_Session::DeRefViaKey (unsigned char*  key, 
                                                        ClassID         guid, 
                                                        bool            forUpd, 
                                                        bool            doLock, 
                                                        bool            shared,
                                                        OmsSchemaHandle Schema,
                                                        OmsContainerNo  ContainerNo)
{
  OMS_ContainerEntry*  pContainerInfo = m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo);
  OmsObjectContainerPtr found = NULL;
  if (pContainerInfo->UseCachedKeys()) {    // PTS 1117571
    found = m_context->GetObjViaCachedKey(*pContainerInfo, key, doLock, shared);
  }
  else {
    found   = m_context->GetObjViaKey(*pContainerInfo, key, doLock, shared);
  }


  if (NULL != found) {
    if (forUpd) {
      return this->ForUpdPtr(found);
    }
    return &found->m_pobj;
  }
  return NULL;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::DeleteSession()
{
  --m_refCnt;
  if (0 == m_refCnt) {
    this->OMS_Session::~OMS_Session();
    OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(this);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::ExecuteSubtransCommit(int requiredLevel)
{
  tsp00_Int2     DBError;
  OMS_HResult hr;
  m_beforeImages.subtransCommit (requiredLevel); 
  while (this->CurrentSubtransLevel() >= requiredLevel) {
    if (this->CurrentSubtransLevel() > 1) {
      hr = m_lcSink->SubtransCommit( &DBError);
      if ( 0 != DBError ) { 
        OMS_Globals::Throw(DBError, "OMS_Session::ExecuteSubtransCommit", __MY_FILE__, __LINE__);
      }
    }
    this->DecSubtransLevel();
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::deallocate(void* p) 
{
#ifdef USE_SYSTEM_ALLOC_CO13
  delete[] (char*) p;
#else
  m_heap.Deallocate(p);
#endif
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::ForceReadOnly()
{
  bool ret        = m_read_only;
  m_read_only     = true;
  m_allowReadOnly = false;
  return ret;
}

/*----------------------------------------------------------------------*/

inline OmsAbstractObject* OMS_Session::ForUpdPtr(OmsObjectContainerPtr pObj)
{
  const char* msg = "OMS_Session::ForUpdPtr";
  this->AssertNotReadOnly(msg);
  this->InsertBeforeImage (pObj);
  return &pObj->m_pobj;
}

/*----------------------------------------------------------------------*/

inline OmsCallbackInterface* OMS_Session::GetCallbackInterface() const
{
  return m_callbackInterface;
}

/*----------------------------------------------------------------------*/

inline OmsCallbackInterface* OMS_Session::GetCallbackInterface(ClassID guid) const
{
  if (m_callbackInterfaceGuid == guid) {
    return m_callbackInterface;
  }
  else {
    return NULL;
  }
}

/*----------------------------------------------------------------------*/

inline OMS_ClassDirectory&  OMS_Session::GetClassDir()
{
  return m_classDir;
}

/*----------------------------------------------------------------------*/

inline OMS_RWLockDirectory&  OMS_Session::GetRWLockDir()
{
  return m_rwLockDir;
}

/*----------------------------------------------------------------------*/

inline OMS_ISessionLockObjects* OMS_Session::GetSessionLockObject() const
{
  return m_lockObjects; 
}

/*----------------------------------------------------------------------*/

inline long OMS_Session::GetStoredErrorCode() const  // PTS 1122839
{
  return m_errorCode;
}

/*----------------------------------------------------------------------*/

inline OMS_StreamRegistry& OMS_Session::GetStreamRegistry() 
{
    return m_streamRegistry;
}

/*----------------------------------------------------------------------*/

inline tsp00_TaskId  OMS_Session::GetTaskId() const // PTS 1110315
{
  return m_taskId;
}

/*----------------------------------------------------------------------*/

inline unsigned char* OMS_Session::GetVarObjMemory(size_t sz)
{
  this->MonitorVarObjSize((tsp00_Int4) sz);
  return REINTERPRET_CAST(unsigned char*, m_context->allocate(sz));
}

/*----------------------------------------------------------------------*/

inline size_t OMS_Session::GetVarObjSize(const OMS_ObjectId8& oid) 
{

  const char* msg = "OMS_Context::GetVarObjSize";
  OmsObjectContainerPtr pObj = m_context->FindObjInContext (&oid);
  if (NULL != pObj) {
    if (pObj->DeletedFlag()) {
      ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }
    OMS_VarObjInfo* pObjInfo;
    pObjInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &pObj->m_pobj);
    return pObjInfo->m_vobjSize;
  }
  size_t objSize = m_context->GetVarObjFromLiveCacheBase(oid, false);
  m_context->LoadVarObject(oid, VarObjNoLock, objSize, m_currVarObjChunk, NULL);
  return objSize;
}

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/

OmsAllocatorInterface &OMS_Session::GetSessionUserAllocator()
{
  return m_sessionUserAllocatorWrapper;
}

/*----------------------------------------------------------------------*/

OmsAllocatorInterface &OMS_Session::GetTransactionUserAllocator()
{
  return m_transactionUserAllocatorWrapper;
}

/*----------------------------------------------------------------------*/

OmsAllocatorInterface &OMS_Session::GetCOMRoutineUserAllocator()
{
  return m_comRoutineUserAllocatorWrapper;
}

/*----------------------------------------------------------------------*/

void OMS_Session::UserAllocatorCtl(
  int    msg, 
  void  *param)
{
    switch (msg) {
    case OmsCallbackInterface::BlockSession:
        m_sessionUserAllocatorWrapper.omsCheckAllocatorEmpty();
        // no break;
    case OmsCallbackInterface::BlockTransaction:
        m_transactionUserAllocatorWrapper.omsCheckAllocatorEmpty();
        // no break;
    case OmsCallbackInterface::BlockCOMRoutine:
        m_comRoutineUserAllocatorWrapper.omsCheckAllocatorEmpty();
        break;
    case OmsCallbackInterface::BlockUnregister:
        // remove all callbacks
        m_sessionUserAllocatorWrapper.omsRegisterCallback(NULL);
        m_transactionUserAllocatorWrapper.omsRegisterCallback(NULL);
        m_comRoutineUserAllocatorWrapper.omsRegisterCallback(NULL);
        break;
    }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncRefCnt()
{
  ++m_refCnt;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncCacheHit(const OMS_ObjectId8& oid)
{
  if (!InVersion()) 
  {
    m_monitor.IncOmsReadCacheHit();
  }
  else if (CurrentContext()->IsVersionOid(oid)){
    m_monitor.IncOmsReadVersion();
  }
  else {
    m_monitor.IncOmsReadCacheHit();
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncCacheMiss()
{
    m_monitor.IncOmsReadCacheMiss();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncDeref()
{
  if (--m_rescheduleDistance <= 0) { 
    RescheduleAndCheckCancelFlag();
  }  
  m_monitor.IncDeref();
}

/*----------------------------------------------------------------------*/

inline void  OMS_Session::IncDerefIter_LC()
{ // PTS 1107819 TA 13/09/2000 
  m_monitor.IncDerefIter();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncDereflC()
{
  if (InVersion()) 
  {
    m_monitor.IncLCDerefInVersion();
  } 
  else 
  {
    m_monitor.IncDerefLCBase();
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncDerefKey()
{
  if (InVersion()) 
  {
    m_monitor.IncLCKeyDerefInVersion();
  }
  else {
    m_monitor.IncKeyDeref();
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncOmsTerminate()
{
  m_monitor.IncOmsTerminate();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncStore() 
{
  m_monitor.IncStore();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncStore_lC() 
{
  if (InVersion())
  {
    m_monitor.IncStoreInVersion();
    
  }
  else 
  {
    m_monitor.IncLCStore();
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncReadStreamBuffer()
{
  m_monitor.IncReadSteam();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncRehash()
{
  m_monitor.IncRehash();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncWriteStreamBuffer()
{
  m_monitor.IncWriteSteam();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncReadStreamRow()
{
  m_monitor.IncReadStreamRow();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncWriteStreamRow()
{
  m_monitor.IncWriteStreamRow();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncDelete ()
{
  m_monitor.IncDelete();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncDelete_lC(tsp00_Int4 cntDeleted) 
{
  if (InVersion()) 
  {
    m_monitor.IncDeleteInVersion(cntDeleted);
  }
  else 
  {
    m_monitor.IncLCDelete(cntDeleted);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncDelete_lC() 
{
  IncDelete_lC(1);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncLock() 
{
  m_monitor.IncLock();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncLogHop(int cnt) 
{
  m_monitor.IncLogHops(cnt);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncLogHopIter(int cnt) 
{ // PTS 1107819 TA 13/09/2000
  m_monitor.IncLogHopsIter(cnt);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncExceptions() 
{
  m_monitor.IncExceptions();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncOutOfDate() 
{
  m_monitor.IncOutOfDate();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncOutOfMemory() 
{
  m_monitor.IncBadAlloc();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncTimeout() 
{
  m_monitor.IncTimeout();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncSubtransCommit() 
{
  m_monitor.IncSubtransCommit();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncSubtransRollback()
{
  m_monitor.IncSubtransRollback();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncLoadVarObj() 
{
  m_monitor.IncLoadVarObject();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncLoadVarObjLC() 
{
  m_monitor.IncLoadVarObjectLC();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncStoreVarObj()
{
  m_monitor.IncStoreVarObject();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncStoreVarObjLC() 
{
  m_monitor.IncLCStoreVarObj();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::MaxHashChainLen (int len)
{ // PTS 1118855
  m_monitor.MaxHashChainLen(len);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::MonitorSubtransLevel() 
{
  m_monitor.MaxSubtransLevel(m_subtrans_lvl);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::SetCacheSize(tsp00_Uint4 sz) 
{
  m_monitor_curr->cmiCounters_gg00[cmiCacheSize] = sz;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncCreateVersion () 
{
  m_monitor.IncCreateVersion();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncOpenVersion () {
  m_monitor.IncOpenVersion();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncCloseVersion () 
{
  m_monitor.IncCloseVersion();
}   

/*----------------------------------------------------------------------*/
        
inline void OMS_Session::IncDropVersion () 
{
  m_monitor.IncDropVersion();
}             

/*----------------------------------------------------------------------*/          

inline void OMS_Session::IncDeleteVarObject ()
{
   m_monitor.IncDeleteVarObject();
}      

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncDeleteVarObjectLC () 
{
   m_monitor.IncLCDeleteVarObj();
}              

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncSubtransLevel () 
{
  ++m_subtrans_lvl;
  this->MonitorSubtransLevel();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncCntWaitOmsLockObj() {
      m_monitor.IncCntWaitOmsLockObj();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::IncWaitOmsLockObjSecs(tsp00_Int4 secs)
{
    m_monitor.IncWaitOmsLockObjSecs(secs);
}
 
/*----------------------------------------------------------------------*/

inline bool OMS_Session::InsertBeforeImage (OmsObjectContainerPtr p)
{
  int subtransLvl = this->CurrentSubtransLevel();
  if (!(p->existBeforeImage (subtransLvl, m_context->IsVersion() || OMS_Globals::m_globalsInstance->InSimulator()))) {
    if ((subtransLvl > m_min_subtrans_lvl) || (this->InVersion())) {
      m_beforeImages.insertBeforeImage (p, subtransLvl);
      return true;
    }
  }
  return false;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::InsertNewBeforeImage (OmsObjectContainerPtr p, OMS_ContainerEntry* pContainerInfo)
{
  int subtransLvl = this->CurrentSubtransLevel();
  if ((subtransLvl > m_min_subtrans_lvl) || (this->InVersion())) {
    m_beforeImages.insertNewBeforeImage (p, pContainerInfo, subtransLvl);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::InsertTransVersion(OMS_Context* context, bool create)
{
  if (!this->VersionBoundByMe(context)) {
    m_versionsBoundToTrans.insert(context);
  }
  context->MarkBoundToTrans(this);
  if (create) {
    m_createdVersionsInTrans.insert(context);
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::InVersion()
{
  return (m_context != m_defaultContext);
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsDefaultContext( OMS_Context* context)
{
  return (context == m_defaultContext);
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsSubtransOpen()
{
  return (m_subtrans_lvl > 1);
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_Session::LoadVarObject (const OMS_ObjectId8& oid, OMS_VarObjLockType lockReq, size_t size, void* buf) 
{
  const char* msg = "OMS_Session::LoadVarObject";
  
  this->IncLoadVarObj();
  OmsObjectContainerPtr p = m_context->FindVarObjInContext(oid, lockReq, size, buf);
  if (NULL != p) {
    return p;
  }
  size_t objSize = m_context->GetVarObjFromLiveCacheBase(oid, false);
  if ((size < objSize) && (buf != NULL)) {
    this->ThrowDBError (e_too_small_packet_size, msg, oid, __MY_FILE__, __LINE__);
  }
  p = m_context->LoadVarObject(oid, lockReq, objSize, m_currVarObjChunk, buf);
  m_currVarObjChunk.Clear();
  return p;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::LockObj(OmsObjectContainerPtr p)
{
  tgg00_BasisError    DBError;
  OMS_UnknownContainerId FileId;
  OMS_HResult hr = m_lcSink->LockObj (
    (unsigned char*) &m_context->m_consistentView,
    (unsigned char*) &FileId, //TODO so lassen ??
    REINTERPRET_CAST(SAPDB_UInt8*, &p->m_oid),
    (unsigned char*) &p->m_objseq,
    &DBError);
  m_context->LockResult (DBError, p, p->m_oid, "OMS_Session::LockObj", /*shared=*/false);
  this->IncLock();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::LockSharedObj(OmsObjectContainerPtr p)
{
  tgg00_BasisError    DBError;
  OMS_UnknownContainerId FileId;
  OMS_HResult hr = m_lcSink->LockSharedObj (
    (unsigned char*) &m_context->m_consistentView,
    (unsigned char*) &FileId, //TODO so lassen ??
    REINTERPRET_CAST(SAPDB_UInt8*, &p->m_oid),
    (unsigned char*) &p->m_objseq,
    &DBError);
  m_context->LockResult (DBError, p, p->m_oid, "OMS_Session::LockSharedObj", /*shared=*/true);
  this->IncLock();
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::TryLockObj(
  OmsObjectContainerPtr p,
  bool                  shared)
{
  tgg00_BasisError    DBError;
  OMS_UnknownContainerId FileId;
  if (!shared){
    OMS_HResult hr = m_lcSink->LockObj (
      (unsigned char*) &m_context->m_consistentView,
      (unsigned char*) &FileId, //TODO so lassen ??
      REINTERPRET_CAST(SAPDB_UInt8*, &p->m_oid),
      (unsigned char*) &p->m_objseq,
      &DBError);
  }
  else {
    OMS_HResult hr = m_lcSink->LockSharedObj (
      (unsigned char*) &m_context->m_consistentView,
      (unsigned char*) &FileId, //TODO so lassen ??
      REINTERPRET_CAST(SAPDB_UInt8*, &p->m_oid),
      (unsigned char*) &p->m_objseq,
      &DBError);
  }

  bool retval = m_context->TryLockResult (DBError, shared, p, p->m_oid, "OMS_Session::TryLockObj");
  
  IncLock();
  return retval;
}

/*----------------------------------------------------------------------*/

inline void* OMS_Session::allocate(size_t sz)
{
#ifdef USE_SYSTEM_ALLOC_CO13
    return new char[sz];
#else
    return m_heap.Allocate((SAPDB_ULong) sz);
#endif
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::MonitorAlloc(SAPDB_ULong chunkSize, bool isUsrRequest)
{
  m_monitor.MonitorAlloc(chunkSize, isUsrRequest);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::MonitorDealloc(SAPDB_ULong chunkSize, bool isUsrRequest)
{
  m_monitor.MonitorDealloc(chunkSize, isUsrRequest);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::MonitorVarObjSize(tsp00_Int4 size)
{
  m_monitor.MonitorVarObjSize(size);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::MonitorWaitNewConsistentView(tsp00_Int4 runTime)
{
  m_monitor.MonitorNewConsistentView(runTime);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::OpenVersionProlog(const OmsVersionId &versionId, bool create)   // PTS 1129082
{
  const char * msg = "OmsSession::OpenVersion";

  bool changedObjAreNotAllowed = false;
  if (create) {
    // no modifications allowed before create
    changedObjAreNotAllowed = true; 
  } 
  else {
    // check for empty only if exists in created trans
    cgg251dclIterator<OMS_Context*,OMS_Session> iter = m_createdVersionsInTrans.begin();
    while (iter) {
      OMS_Context* pContext = *iter();
      if (!memcmp(pContext->GetVersionId(), versionId, sizeof(OmsVersionId))) {
        // created in this trans
        changedObjAreNotAllowed = true;
        break;
      }
      ++iter;
    }
  }

  if (changedObjAreNotAllowed && IsDataChanged()) {
    // already flushed something, cannot clear cache before opening version
    OMS_Globals::Throw(e_transaction_end_required, msg, __MY_FILE__, __LINE__);
  }

  // As the consistent view is closed when opening a version, remove all non-changed objects
  // out of the hash. Otherwise these objects would become visible again (in a different 
  // consistent view) after the version has been closed.
  // If the version was not newly created in the current transaction, then objects which has 
  // been changed can remain in the cache, as these objects could not be modified by concurrent 
  // transactions, as they are locked. 
  // If the version was newly created, then an exception is thrown if an object is found, which 
  // has been modified.
  m_defaultContext->VersionClearObjCache(changedObjAreNotAllowed);
}

/*----------------------------------------------------------------------*/
 
inline void OMS_Session::OpenVersionEpilog()  // PTS 1129082
{

  /* current context is default context, release current consistent view */
  if (m_defaultContext->ExistsConsistentView()) {
    short   DBError;
    m_lcSink->EndConsistentView((unsigned char*) &m_defaultContext->m_consistentView, &DBError);
    if ( 0 != DBError ) { 
      OMS_Globals::Throw(DBError, "OmsSession::OpenVersionEpilog", __MY_FILE__, __LINE__);
    }
  }
}
 
/*----------------------------------------------------------------------*/

inline void OMS_Session::OpenVersion(OMS_Context* context, bool create) 
{
  SetContext(context);
  InsertTransVersion(context, create);
  m_context->OpenVersion(this);
  ChangedConsistentView();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::ReadUnchangedObjectsStart() 
{
  m_read_only_old = m_read_only;
  m_read_only     = true;
  m_readReadUnchangedObjectsActive = true;
  m_context->ReadUnchangedObjectsStart();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::ReadUnchangedObjectsEnd() 
{
  m_context->ReadUnchangedObjectsEnd();
  m_read_only = m_read_only_old;
  m_readReadUnchangedObjectsActive = false;
}

/*----------------------------------------------------------------------*/


inline void OMS_Session::ReleaseObj(const OMS_ObjectId8& oid) 
{
  OmsObjectContainerPtr found = m_context->FindObjInContext(&oid);
  if (found) {
    this->ReleaseObj(found);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::ReleaseObj(OmsObjectContainerPtr p, bool unchangedOnly)
{
  // Objects are NOT released from the cache if:
  // 1)  the object is 'changed' AND
  // 2a) a version is open OR
  // 2b) a subtransaction is open
  // 2c) or the simulator is active (before image of lowest subtrans-level is needed 
  //     to check, whether an updated object is not stored during commit-processing) 
  //
  // An object is considered as 'changed' if:
  // * A lock has been aquired on the object AND the object is no var-object OR
  // * the object is marked as to be stored (omsStore) OR
  // * the object is marked as to be deleted OR
  // * a before-image of this object exists (omsForUpd)  

  //const bool releaseUpdatedObj 
  //  =  (unchangedOnly ? false
  //                    : (!m_context->IsVersion() && !IsSubtransOpen() && !OMS_Globals::m_globalsInstance->InSimulator()));

  // Because of PTS 1137538 is must be ensured, that no updated objects are
  // flushed to the kernel before transaction end!
  const bool releaseUpdatedObj = false;

  m_monitor.OmsIncOmsRelease(); // PTS 1133314

  bool updated = false;
  if (p->ReplacedFlag()){
    updated = true;
  }
  else if (p->DeletedFlag()){
    updated = true;
    if (releaseUpdatedObj) {
      //if (p->IsNewObject()){
      //  OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink, 
      //    "OMS_Session::ReleaseObj 1 : dec flush: " << p->m_oid 
      //    << ", class: " << p->GetContainerInfo(__MY_FILE__, __LINE__)->GetClassEntry().GetClassName());
      //  m_context->DecNewObjectsToFlush(p);
      //}
      m_context->FlushDelete (p, false /*deleteInHash*/);
    }
  }
  else if (p->StoredFlag()){
    updated = true;
    if (releaseUpdatedObj){ 
      //if (p->IsNewObject()){
      //  OMS_DETAIL_TRACE(omsTrNewObj, m_lcSink,
      //    "OMS_Session::ReleaseObj 2 : dec flush: " << p->m_oid 
      //    << ", class: " << p->GetContainerInfo(__MY_FILE__, __LINE__)->GetClassEntry().GetClassName());
      //  m_context->DecNewObjectsToFlush(p);
      //}
      m_context->FlushObj(p);
    }
  }
  else if (p->LockedFlag()){
    updated = true;
    if (releaseUpdatedObj) {
      m_context->FlushLockUpdObj (p);
    }
  }
  else if (p->GetBeforeImageMask() != 0){  // PTS 1140951
    updated = true;
  }

  if (!updated || releaseUpdatedObj){
    m_monitor.OmsIncOmsReleaseExecuted(); // PTS 1133314
    m_context->DeleteObjInContext(p, p->GetContainerInfo(__MY_FILE__, __LINE__));
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::ReleaseObjUnconditional(const OMS_ObjectId8& oid)
{
  OmsObjectContainerPtr found = m_context->FindObjInContext(&oid);
  if (found) {
    m_context->DeleteObjInContext(found, found->GetContainerInfo(__MY_FILE__, __LINE__));
  }
}
/*----- PTS 1132163, PG ------------------------------------------------*/

inline void OMS_Session::Sleep(const OmsTypeUInt1 sleepTime) {
  m_lcSink->Sleep(sleepTime);
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::RollbackSubtrans(int requiredLvl, bool callFromRollbackTrans)
{
  tgg00_BasisError DBError;
  OMS_HResult    hr;
 
  if (-1 == requiredLvl) {
    requiredLvl = this->CurrentSubtransLevel();
  }
  m_beforeImages.subtransRollback (requiredLvl); 
  while (this->CurrentSubtransLevel() >= requiredLvl) {
    if ((this->CurrentSubtransLevel() > m_min_subtrans_lvl) && (!callFromRollbackTrans)) 
    {
      hr = m_lcSink->SubtransRollback( &DBError);
      if ( e_ok != DBError ) { 
        OMS_Globals::Throw(DBError, "RollbackSubtrans", __MY_FILE__, __LINE__, this);
      }
    }
    this->DecSubtransLevel();
  }
  m_currVarObjChunk.Clear();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::ResetSubtransLevel () 
{
  m_subtrans_lvl = 1;
}

/*----------------------------------------------------------------------*/
#if defined(OMSTST)

inline void OMS_Session::SetCancelCountDown(int countDown)
{
  if (0 == countDown) 
  {
    countDown = 1;
  }
  else
  {
    if (countDown < 0)
    {
      countDown = -1;
    }
  }
  m_cancelCountDown = countDown;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::SetExceptionCountDown(int countDown)
{
  if (0 == countDown) 
  {
    countDown = 1;
  }
  else
  {
    if (countDown < 0)
    {
      countDown = -1;
    }
  }
  m_exceptionCountDown = countDown;
}
#endif

/*----------------------------------------------------------------------*/

inline void OMS_Session::SetSessionLockObject(OMS_ISessionLockObjects* pLockObj)
{
  m_lockObjects = pLockObj;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::Signal(tsp00_TaskId TaskId, tsp00_Bool ok) // PTS 1110315
{
  m_lcSink->Signal(TaskId, ok);
}

/*----------------------------------------------------------------------*/

inline int OMS_Session::StartSubtrans()
{
  const char* msg = "OMS_Session::StartSubtrans";
  tsp00_Int2  DBError;
  if (this->CurrentSubtransLevel() >= OMS_BEFOREIMAGE_MAX_SUBTRANS_LEVEL) {
    this->ThrowDBError (e_OMS_too_many_subtrans, msg, __MY_FILE__, __LINE__);      
  }
  OMS_HResult hr = m_lcSink->SubtransStart(&DBError);
  if ( 0 != DBError ) { 
    this->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
  }
  this->IncSubtransLevel();
  return this->CurrentSubtransLevel();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::StoreErrorCode(long errorCode)   // PTS 1122839
{
  m_errorCode = errorCode;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session:: StoreVarObject (const OMS_ObjectId8& oid, const void* pVarObj, unsigned long objLength) 
{
  OmsObjectContainerPtr found           = LoadVarObject (oid, VarObjNoLock, 0, NULL);
  if (NULL == found) {
      this->ThrowDBError (e_object_not_found, "StoreVarObject", oid, __MY_FILE__, __LINE__);
  }

  if (!IsLocked(found, /*shared=*/false)) {
    this->ThrowDBError (e_object_not_locked, "StoreVarObject", oid, __MY_FILE__, __LINE__);
  }

  IncStoreVarObj(); 

  if (m_currVarObjChunk.m_oid == oid) {
    m_currVarObjChunk.Clear();
  }

  OMS_VarObjInfo* objInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &found->m_pobj);
  if (!(found->existBeforeImage (this->CurrentSubtransLevel(), this->InVersion()))) { 
    // Old im-mem-representation is used as before image
    ForUpdPtr(found); 
    objInfo->m_pvobj = NULL;
  }
  else if (objInfo->m_vobjSize != objLength){
    // As there is already a before image on this level and the size can not be reused
    // for the new image, the old in-mem-representation is freed
    objInfo->freeVarObjData(m_context);
    objInfo->m_pvobj = NULL;
  }

  if (objInfo->m_pvobj == NULL) {
    // Ensure alignment of object and add space for check-pattern
    size_t sz = ((objLength + 3) & ~3) + sizeof(tsp00_Uint4);
    objInfo->m_pvobj = GetVarObjMemory(sz);

    // Write check-pattern into the last 4 bytes of the allocated memory.
    // This pattern will be checked during store and during the flush.
    reinterpret_cast<tsp00_Uint4*>(objInfo->m_pvobj+sz)[-1] = OMS_VAROBJ_CHECK_PATTERN;
  }
  objInfo->m_vobjSize = objLength;
  if (objLength != 0){
    SAPDB_MemCopyNoCheck(objInfo->m_pvobj, pVarObj, objLength);
  }
  found->MarkStored();
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::UnforceReadOnly(bool readOnly)
{
  m_read_only     = readOnly;
  m_allowReadOnly = true;
}

/*----------------------------------------------------------------------*/

inline void  OMS_Session::FreeBeforeImages(int level)
{
  if (CurrentSubtransLevel() >= level){
    /* free before images */
    m_beforeImages.freeBeforeImages(level);
    m_subtrans_lvl = level;
  }
}

/*----------------------------------------------------------------------*/

void inline OMS_Session::ReleaseAllUnchanged()    // PTS 1128262
{
  m_context->ReleaseAllUnchanged();
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsLocked (const OmsObjectContainer* pObj, bool shared)
{
  if (pObj == NULL){
    ThrowDBError (e_nil_pointer, "IsLocked: NULL-Pointer ", __MY_FILE__, __LINE__);
  }

  if (m_context->IsVersion()) {
    return true;
  }

  // If flag is set in the object container, then the object is locked in
  // the kernel. As the invers is not garantueed, the kernel must be asked.
  if (!shared && pObj->LockedFlag() || shared && pObj->LockedSharedFlag()) {
    return true;
  }
  else {
    return IsLockedInKernel(pObj->m_oid, shared);
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsLocked (const OMS_ObjectId8& oid, bool shared)
{
  if (m_context->IsVersion()) {
    return true;
  }

  // Check whether object is already stored in local cache
  OmsObjectContainerPtr found = m_context->FindObjInContext (&oid);

  // If flag is set in the object container, then the object is locked in
  // the kernel. As the invers is not garantueed, the kernel must be asked.
  if (found && ( !shared && found->LockedFlag()
               || shared && found->LockedSharedFlag())) {
    return true;
  }
  else {
    return IsLockedInKernel(oid, shared);
  }
}

/*----------------------------------------------------------------------*/

inline unsigned int OMS_Session::GetObjectRevisionNumber (const OMS_ObjectId8& oid)
{
  const char* msg = "OMS_Session::GetObjectRevisionNumber";

  OmsObjectContainerPtr found = m_context->GetObj (oid, /*doLock=*/false, /*shared=*/false);
  if (found) {
    return found->GetRevisionNumber();
  }
  else {
    ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    return 0;
  }
}


/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsCancelledByProgram() const
{
  return m_cancelledByProgram;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::SetCancelledByProgram(bool b)
{
  m_cancelledByProgram = b;
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsProvokedByTestkernel() const
{
  return m_requiredExceptionThrown;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::SetProvokedByTestkernel(bool b)
{
  m_requiredExceptionThrown = b;
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsTestKnlExceptionActive() const
{
  return m_testKnlExceptionActive;
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsTestKnlBadAllocActive() const
{
  bool active;
  m_lcSink->IsTestKnlBadAllocActive(active);
  return active;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::SetTestKnlExceptionActive(bool b)
{
  // Set flag in the kernel
  unsigned char flag  = (b ? 1 : 0);
  tsp00_Int4    len = 1;
  short         error;
  m_lcSink->MultiPurpose (m_diagnose, mm_provoked_error, &len, &flag, &error);

  // Set flag in the session
  m_testKnlExceptionActive = b;
}
/*----------------------------------------------------------------------*/

inline void OMS_Session::SetContext(OMS_Context* context) 
{
  GetClassDir().CleanupAfterContextChange();
    
  m_context = context;
}

/*----------------------------------------------------------------------*/

inline void OMS_Session::SetCancelFlag(bool b)
{
  m_toCancel = (b ? 1 : 0);
}

/*----------------------------------------------------------------------*/

inline bool OMS_Session::IsCancelled() const
{
  return (m_toCancel != 0);
}

/*----------------------------------------------------------------------*/

#endif  // __OMS_SESSION_HPP
