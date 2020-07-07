/*!
 * \file    OMS_Context.cpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS context.
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

#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_DumpInterface.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_VersionDictionary.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Globals.hpp"
#include "hsp77.h"
#include "geo573.h"

#define REGISTER_ALLOCATOR true

// if this define is set, then before flushing the objects to the kernel, 
// objects with the same page are grouped together
#undef OMS_CLUSTERED_COMMIT 

#define OMS_NO_ERROR   0
#define OMS_EXCEPTION  1
#define OMS_BAD_ALLOC  2

/*----------------------------------------------------------------------*/

#ifndef USE_SYSTEM_ALLOC_CO13

OMS_Context::OMS_ContextAllocator::OMS_ContextAllocator(tsp00_TaskId taskId, const OmsVersionId* pVersionId)
: SAPDBMem_RawAllocator((const SAPDB_UTF8*) "", OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper, 
                        OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize(), OMS_Globals::m_globalsInstance->GetDefaultRawChunkSize(), 
                        SAPDBMem_RawAllocator::FREE_RAW_EXTENDS, SAPDBMEM_ALLOCATOR_UNLIMITED, !REGISTER_ALLOCATOR)
, m_heapTracer(OMS_AllocatorTracer::THROW_BAD_DEALLOC)
{
  SAPDB_UTF8 heapName[41];
  if (NULL != pVersionId) {
    sp77sprintf (REINTERPRET_CAST(char*, &heapName[0]), sizeof(heapName), "OMS Version %.22s", &pVersionId[0]);
  }
  else
  {
    sp77sprintf (REINTERPRET_CAST(char*, &heapName[0]), sizeof(heapName), "OMS default context T%03d", taskId);
  }
  SetIdentifier (&heapName[0]);
  SetTracer     (&m_heapTracer);
  SetBadAllocHandler(SAPDBMem_RawAllocator::THROW_STD_BAD_ALLOC);
  SetCheckLevel (OMS_Globals::GetKernelInterface()->GetHeapCheckLevel());
  OMS_Globals::GetKernelInterface()->RegisterAllocator(GetAllocatorInfo());
}

#endif

/*----------------------------------------------------------------------*/

OMS_Context::OMS_Context(OMS_Session* session, 
                         const OmsVersionId* vid,
                         const tgg01_OmsVersionContext* vctxt) 
 : m_session(session)
 , m_oidDir() 
 , m_containerDir()
 , m_consistentView()
 , m_pVersionContext(NULL)
 , m_isOpen(false)
 , m_isDropped(false)
 , m_isVersion(false)
#ifndef USE_SYSTEM_ALLOC_CO13
 , m_heap(session->GetTaskId(), vid)
 , m_stackHeap()
 , m_useStackHeap(!(OMS_Globals::GetKernelInterface()->GetHeapCheckLevel() & 64))/*PTS 1134642*/
 , m_memBlockStack(&m_heap)
#endif
 , m_next(0)
 , m_nextUnloaded(0) 
 , m_boundToTrans(false)
 , m_marked(false)
 , m_date()
 , m_time()
 , m_lastOpenDate()
 , m_lastOpenTime()
 , m_cntNewObjectsToFlush(0)
 , m_versionDesc(NULL)
 , m_isReadOnlyContext(false)
 , m_pOidDirReadOnly(NULL)
{
  session->m_lcSink->GetDateTime(&m_date, &m_time);
  ResetConsistentView();

  if (NULL != vid) {
    //THROW_BAD_ALLOC_GEO573;

    m_isVersion      = true;
    m_versionContext = *vctxt;
    SAPDB_MemCopyNoCheck (m_version, vid, sizeof(OmsVersionId));
    m_isOpen         = true;
  }
  else
  {
    SAPDB_MemCopyNoCheck (m_version, "Default-Context       ", sizeof(OmsVersionId));
  }

#ifndef USE_SYSTEM_ALLOC_CO13
  m_stackHeap.InitStackAllocator(&m_heap, OMS_DEFAULT_CHUNK_SIZE);
#endif

  m_containerDir.Create(this);
  m_oidDir.Create(this);
};

/*----------------------------------------------------------------------*/

OMS_Context::~OMS_Context() 
{
#ifdef USE_SYSTEM_ALLOC_CO13
  if (m_isVersion)
  {
    m_oidDir.Clear(/*adaptOidHash=*/false);
    if (m_versionDesc != NULL){  
      deallocate(m_versionDesc);
    }
  }
#else
  OMS_Globals::GetKernelInterface()->DeregisterAllocator(m_heap.GetAllocatorInfo());
#endif
}

/*----------------------------------------------------------------------*/

void OMS_Context::DeleteSelf() 
{
  if (m_isVersion) {                /* FF 16.Nov.99  PTS 1104668 */
    tgg00_BasisError DBError;
    m_versionContext.ovc_trans_version = m_consistentView;
    OMS_HResult hr = OMS_Globals::GetCurrentLcSink()->DropVersion((unsigned char*) &m_versionContext, &DBError);
    if (0 != DBError)
    {
      // just write a message into knldiag and ignore error
      DbpBase opMsg(OMS_Globals::GetCurrentLcSink());
      char versionId[sizeof(OmsVersionId)+1];
      memcpy(&versionId[0], GetVersionId(), sizeof(OmsVersionId));
      versionId[sizeof(OmsVersionId)] = 0;
      opMsg.dbpOpError("drop version %s, error %d ignored", versionId, DBError);
    }

    // Check that context is no longer registered in "unloadable-version-list".
    // As this should have been occurred already before, write a message into
    // the knldiag.
    OMS_SinkCriticalSection unloadRegion(OMS_Globals::GetCurrentLcSink(), RGN_UNLOAD_VERSION);
    unloadRegion.Enter();
    if (OMS_Globals::m_globalsInstance->m_versionDictionary.m_UnloadableVersions.ExistsVersion(this)){
      DbpBase opMsg(OMS_Globals::GetCurrentLcSink());
      char versionId[sizeof(OmsVersionId)+1];
      memcpy(&versionId[0], GetVersionId(), sizeof(OmsVersionId));
      versionId[sizeof(OmsVersionId)] = 0;
      opMsg.dbpOpMsg("version %s is still registered in unloadable-list; remove it now", versionId);
      OMS_Globals::m_globalsInstance->m_versionDictionary.m_UnloadableVersions.RemoveVersion(this);
    }
  }

  this->OMS_Context::~OMS_Context();
  OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(this);
}

/*----------------------------------------------------------------------*/

void OMS_Context::Dump(OMS_DumpInterface& dumpObj) const
{
  struct ContextDumpInfo
  {
    void*                    m_this;
    OmsVersionId             m_version;
    tsp00_Int2               m_filler1;
    OMS_Session*             m_session;
    IliveCacheSink*          m_dummy; 
    tgg01_OmsVersionContext* m_pVersionContext;
    tgg91_TransNo            m_consistentView;
    bool                     m_isOpen;
    bool                     m_isDropped;
    bool                     m_isVersion;
    bool                     m_boundToTrans;
    bool                     m_marked;
    bool                     m_filler2;
  } contextDumpInfo;

  contextDumpInfo.m_this            = CONST_CAST(OMS_Context*, this);
  SAPDB_MemCopyNoCheck(&contextDumpInfo.m_version[0], &m_version[0], sizeof(m_version));
  contextDumpInfo.m_filler1         = 0;
  contextDumpInfo.m_session         = m_session;
  contextDumpInfo.m_pVersionContext = m_pVersionContext;
  contextDumpInfo.m_consistentView  = m_consistentView;
  contextDumpInfo.m_isOpen          = m_isOpen;
  contextDumpInfo.m_isDropped       = m_isDropped;
  contextDumpInfo.m_isVersion       = m_isVersion;
  contextDumpInfo.m_boundToTrans    = m_boundToTrans;
  contextDumpInfo.m_marked          = m_marked;
  contextDumpInfo.m_filler2         = false;
  dumpObj.SetDumpLabel(LABEL_OMS_CONTEXT);
  dumpObj.Dump(&contextDumpInfo, sizeof(contextDumpInfo));
  m_oidDir.Dump(dumpObj);
}

/*----------------------------------------------------------------------*/

void OMS_Context::DumpObjCache(OmsHandle& h) {
  char                  flags[4];
  flags[sizeof(flags)-1] = 0;
  OmsObjectContainerPtr curr;
  for (OMS_OidHash::OidIter iter = m_oidDir.First(); iter; ++iter) {
    curr = iter();
    if (curr->DeletedFlag()) {
      flags[0] = 'D';
    }
    else {
      flags[0] = ' ';
    }
    if (curr->StoredFlag()) {
      flags[1] = 'U';
    }
    else {
      flags[1] = ' ';
    }
    if (curr->LockedFlag()) {
      flags[2] = 'L';
    }
    else {
      flags[2] = ' ';
    }
    curr->m_pobj.omsDump(h, flags);
  }
}

/*----------------------------------------------------------------------*/

void OMS_Context::EmptyObjCache(OMS_ContainerHandle &containerHandle)   //PTS 1124356 FF 2003-09-23 new FileDir
{
  OMS_OidHash::OidIter iter = m_oidDir.First();
  while (iter) {
    OmsObjectContainer *curr = iter();
    ++iter;

    OMS_ContainerHandle NilContainerHandle; //PTS 1124356 FF 2003-09-23 new FileDir
    OMS_ContainerEntry *pContainerInfo = curr->GetContainerInfoNoCheck();    
    if (/*0xFFFFFFFF*/NilContainerHandle == containerHandle || pContainerInfo->GetContainerHandle() == containerHandle) 
    {
      // PTS 1127661: Condition on replace-flag added
      if (curr->IsNewObject() && !curr->ReplacedFlag())
      {
        OMS_DETAIL_TRACE(omsTrNewObj, GetSession()->m_lcSink, 
          "OMS_Context::EmptyObjCache : new obj " << curr->m_oid 
          << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
        this->DecNewObjectsToFlush(curr);
      }
      DeleteObjInContext(curr, pContainerInfo);
    }
  }
}

/*----------------------------------------------------------------------*/

int OMS_Context::EmptyObjCache(OMS_DynamicStack<OmsObjectContainer*,OMS_Session> &containerObjects,
                               OMS_ContainerEntry *pContainerInfo)
{
  int cnt = 0;

  while (!containerObjects.IsEmpty()){
    OmsObjectContainer *const*pp = containerObjects.Pop();   

    // PTS 1127661: Condition on replace-flag added
    if ((*pp)->IsNewObject() && !(*pp)->ReplacedFlag())
    {
      OMS_DETAIL_TRACE(omsTrNewObj, GetSession()->m_lcSink, 
        "OMS_Context::EmptyObjCache-2- : new obj " << (*pp)->m_oid 
        << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
      DecNewObjectsToFlush((*pp));
    }
    
    DeleteObjInContext(*pp, pContainerInfo);
    ++cnt;
  }
  return cnt;
}

/*----------------------------------------------------------------------*/

bool OMS_Context::CheckForUpdatedObj(OMS_ContainerHandle &containerHandle,
                                     OMS_DynamicStack<OmsObjectContainer*,OMS_Session> &containerObjects)
{
  bool updated = false;
  OMS_ContainerHandle NilContainerHandle; 

  OMS_OidHash::OidIter iter = m_oidDir.First();
  while (iter) {
    OmsObjectContainer *curr = iter();
    ++iter;

    OMS_ContainerEntry *pContainerInfo = curr->GetContainerInfoNoCheck();    
    if (/*0xFFFFFFFF*/NilContainerHandle == containerHandle || pContainerInfo->GetContainerHandle() == containerHandle) 
    {
      if (curr->GetBeforeImageMask() != 0){
        updated = true;
      }
      containerObjects.Push(curr);
    }
  }
  return updated;
}

/*----------------------------------------------------------------------*/

void OMS_Context::FlushObj(OmsObjectContainerPtr p) 
{
  if (p->IsNewObject()){
    OMS_DETAIL_TRACE(omsTrNewObj, GetSession()->m_lcSink,
      "OMS_Context::FlushObj : dec flush: " << p->m_oid << ", class: " 
      << p->GetContainerInfo(__MY_FILE__, __LINE__)->GetClassEntry().GetClassName());
    DecNewObjectsToFlush(p);
  }

  if (p->VarObjFlag()) {
    FlushVarObject (p);
  }
  else {
    tsp00_Int2  DBError;
    OMS_ContainerEntry* pContainerInfo = p->GetContainerInfo(__MY_FILE__, __LINE__);
    GetSession()->SetDataChanged();
    OMS_HResult hr = GetSession()->m_lcSink->UpdateObj (
      (unsigned char*) &m_consistentView,
      (unsigned char*) &pContainerInfo->GetFileId(),
      (unsigned char*) VersionContext(),
      (unsigned char*) &p->m_oid,
      (unsigned char*) &p->m_objseq, 
      pContainerInfo->GetClassEntry().GetPersistentSize(), 
      ((unsigned char*) &p->m_pobj) + sizeof(void*), 
      pContainerInfo->GetClassEntry().GetKeyDesc().GetPos(),  // PTS 1122540
      pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(),  // PTS 1122540
      false,                                          // PTS 1124278
      p->ReusedFlag(),                                // PTS 1127661 
      &DBError);
    this->CheckDBError(DBError, "OMS_Context::FlushObj", p->m_oid); 
    p->RemoveStoredFlag();
    GetSession()->IncStore_lC();
  }
}

/*----------------------------------------------------------------------*/

void OMS_Context::FlushDelete (OmsObjectContainerPtr pObj, bool deleteInHash) {

  tsp00_Int2     DBError;
  OMS_HResult hr;
  OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
  GetSession()->SetDataChanged();

  if (pObj->IsNewObject()){
    OMS_DETAIL_TRACE(omsTrNewObj, GetSession()->m_lcSink, 
      "OMS_Context::FlushDelete : dec flush: " << pObj->m_oid << ", class: " 
      << pContainerInfo->GetClassEntry().GetClassName());
    DecNewObjectsToFlush(pObj);
  }

  if (pObj->VarObjFlag()) {
    GetSession()->IncDeleteVarObjectLC ();
    bool                    isFirst = true;
    OMS_VarObjInfoNode* pInfoNode; 
    OMS_VarObjInfo*     objInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &pObj->m_pobj);
    pInfoNode = &objInfo->m_vobjInfo;
    while (NULL != pInfoNode) {
      if ((!isFirst) && (!IsVersion())) {
        // ADIS 1001523, T.A. 24.09.1998
        hr = GetSession()->m_lcSink->LockObj (
          (unsigned char*) &m_consistentView,
          (unsigned char*) &pContainerInfo->GetFileId(),
          REINTERPRET_CAST(SAPDB_UInt8*, &pInfoNode->m_vobjOid),
          (unsigned char*) &pInfoNode->m_vobjSeq,
          &DBError);
        this->CheckDBError(DBError, "OMS_Context::FlushDelete (VarObj)", pInfoNode->m_vobjOid);
      }
      hr = GetSession()->m_lcSink->DeleteObj (
        (unsigned char*) &m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        (unsigned char*) VersionContext(),
        (unsigned char*) &pInfoNode->m_vobjOid,
        (unsigned char*) &pInfoNode->m_vobjSeq, 
        (pObj->ReusedFlag() && pObj->m_oid==pInfoNode->m_vobjOid), // PTS 1127661 
        pObj->m_oid!=pInfoNode->m_vobjOid,
        &DBError);
      this->CheckDBError(DBError, "OMS_Context::FlushDelete (VarObj)", pInfoNode->m_vobjOid);
      pInfoNode = pInfoNode->m_vobjNext;
      isFirst   = false;
    }
  } 
  else { // no Var Object
    GetSession()->IncDelete_lC();
    hr = GetSession()->m_lcSink->DeleteObj (
      (unsigned char*) &m_consistentView,
      (unsigned char*) &pContainerInfo->GetFileId(),
      (unsigned char*) VersionContext(),
      (unsigned char*) &pObj->m_oid, 
      (unsigned char*) &pObj->m_objseq,
      pObj->ReusedFlag(),                  // PTS 1127661
      false /*contObj*/,
      &DBError);
    this->CheckDBError(DBError, "OMS_Context::FlushDelete", pObj->m_oid);
  }
  if (deleteInHash) {
    DeleteObjInContext (pObj, pContainerInfo);
  }
}


/*----------------------------------------------------------------------*/

void OMS_Context::FlushLockUpdObj(OmsObjectContainerPtr p) {
  tsp00_Int2  DBError;
  OMS_ContainerEntry* pContainerInfo = p->GetContainerInfo(__MY_FILE__, __LINE__);
  GetSession()->SetDataChanged();
  OMS_HResult hr = GetSession()->m_lcSink->LockUpdObj (
    (unsigned char*) &m_consistentView,
    (unsigned char*) &pContainerInfo->GetFileId(),
    REINTERPRET_CAST(SAPDB_UInt8*, &p->m_oid),
    (unsigned char*) &p->m_objseq, 
    &DBError);
  this->CheckDBError(DBError, "OMS_Context::FlushLockUpdObj", p->m_oid); 
}

/*----------------------------------------------------------------------*/

void OMS_Context::FlushVarObject (OmsObjectContainerPtr pObj) {

  OMS_HResult hr;
  tsp00_Int2     DBError;
  size_t         moveLen;
  size_t         vObjLen;
  tgg91_PageRef  newObjSeq;
  bool           isNew    = true;
  bool           isLocked = false;
  bool           isFirst  = true;
  size_t         objLength;
  OMS_VarObjContainer varObjContainer;
  OMS_VarObjInfoNode*  pInfoNode; 
  OMS_VarObjInfoNode*  pLastNode;

  GetSession()->SetDataChanged();
  GetSession()->IncStoreVarObjLC();
  OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
  OMS_VarObjInfo* objInfo  = REINTERPRET_CAST (OMS_VarObjInfo*, &pObj->m_pobj);
  unsigned char* p             = objInfo->m_pvobj;
  pInfoNode                    = &objInfo->m_vobjInfo;
  objLength                    = objInfo->m_vobjSize;
  varObjContainer.m_vobjSize   = objInfo->m_vobjSize;
  do {
    isLocked = isNew;
    isNew    = false;
    moveLen = OMS_Globals::Min(VAR_OBJ_CHUNK_SIZE, objLength);
    if (moveLen >= objLength) {
      varObjContainer.m_vobjNext.setNil();
    }
    else {
      if (pInfoNode->m_vobjNext) {
        SAPDB_MemCopyNoCheck (&varObjContainer.m_vobjNext, &pInfoNode->m_vobjNext->m_vobjOid, 
          sizeof (pInfoNode->m_vobjNext->m_vobjOid));
      }
      else {
        if (IsVersion()) /* PTS 1110149 */
        {
          OMS_ObjectId8 newOid = VersionNewOid();
          SAPDB_MemCopyNoCheck(&varObjContainer.m_vobjNext, &newOid, sizeof(varObjContainer.m_vobjNext));
          newObjSeq.gg91SetNilRef();
        }
        else
        {
          tgg91_TransNo dummy_updTransId;
          hr = GetSession()->m_lcSink->NewObj (
            (unsigned char*) &m_consistentView,
            (unsigned char*) &pContainerInfo->GetFileId(), 
            (unsigned char*) VersionContext(),
            0,
            0,
            NULL,
            true /*contObj*/,
            REINTERPRET_CAST(SAPDB_UInt8*, &varObjContainer.m_vobjNext),
            (unsigned char*) &newObjSeq,
            &dummy_updTransId,
            &DBError);
          this->CheckDBError(DBError, "OMS_Context::FlushVarObject", pObj->m_oid); 
        }
        isNew                 = true;
        pInfoNode->m_vobjNext = new(*this) OMS_VarObjInfoNode (varObjContainer.m_vobjNext, newObjSeq);
      }
    }
    SAPDB_MemCopyNoCheck (&varObjContainer.vobjInfo[0], p, moveLen);
    if ((!isLocked) && (!IsVersion())) {
      hr = GetSession()->m_lcSink->LockObj (
        (unsigned char*) &m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        REINTERPRET_CAST(SAPDB_UInt8*, &pInfoNode->m_vobjOid),
        (unsigned char*) &pInfoNode->m_vobjSeq, &DBError);
      this->CheckDBError(DBError, "OMS_Context::FlushVarObject", pInfoNode->m_vobjOid); 
    }
    objLength -= moveLen;
    vObjLen    = (objLength > 0) ? (int) sizeof(varObjContainer) : moveLen;
    bool contObj = (pObj->m_oid != pInfoNode->m_vobjOid);  // PTS 1124278
    hr = GetSession()->m_lcSink->UpdateObj (
      (unsigned char*) &m_consistentView,
      (unsigned char*) &pContainerInfo->GetFileId(),
      (unsigned char*) VersionContext(),
      (unsigned char*) &pInfoNode->m_vobjOid,
      (unsigned char*) &pInfoNode->m_vobjSeq,
      vObjLen, 
      (unsigned char*) &varObjContainer, 
      pContainerInfo->GetClassEntry().GetKeyDesc().GetPos(), // PTS 1122540
      pContainerInfo->GetClassEntry().GetKeyDesc().GetLen(), // PTS 1122540
      contObj,                                        // PTS 1124278
      (pObj->ReusedFlag() && pObj->m_oid==pInfoNode->m_vobjOid), // PTS 1127661 
      &DBError);
    this->CheckDBError(DBError, "OMS_Context::FlushVarObject", pInfoNode->m_vobjOid); 
    if (isFirst) {
      pObj->m_objseq = pInfoNode->m_vobjSeq;
      isFirst        = false;
    }
    p         += moveLen;
    pLastNode  = pInfoNode;
    pInfoNode  = pInfoNode->m_vobjNext;
  }
  while (objLength > 0);
  pLastNode->m_vobjNext = NULL;
  if (!IsVersion()) {
    objInfo->freeVarObj(this);
  }
  while (pInfoNode) {
    if (!IsVersion()) {
      hr = GetSession()->m_lcSink->LockObj (
        (unsigned char*) &m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        REINTERPRET_CAST(SAPDB_UInt8*, &pInfoNode->m_vobjOid),
        (unsigned char*) &pInfoNode->m_vobjSeq, &DBError);
    }
    if (0 == DBError) {
      hr = GetSession()->m_lcSink->DeleteObj (
        (unsigned char*) &m_consistentView,
        (unsigned char*) &pContainerInfo->GetFileId(),
        (unsigned char*) VersionContext(),
        (unsigned char*) &pInfoNode->m_vobjOid,
        (unsigned char*) &pInfoNode->m_vobjSeq, 
        (pObj->ReusedFlag() && pObj->m_oid==pInfoNode->m_vobjOid), // PTS 1127661 
        pObj->m_oid!=pInfoNode->m_vobjOid,
        &DBError);
    }
    this->CheckDBError(DBError, "OMS_Context::FlushVarObject", pInfoNode->m_vobjOid); 
    pLastNode = pInfoNode;
    pInfoNode = pInfoNode->m_vobjNext;
    pLastNode->deleteSelf(*this);
  }
}

/*----------------------------------------------------------------------*/

// PTS 1121449
void OMS_Context::FlushObjCache(bool emptyObjCache) 
{
  const char *msg = "OMS_Context::FlushObjCache ";

  bool               errorOccured  = false;
  int                newObjFlushed = 0;
  int                errCnt        = 0;
  int                errorKind     = OMS_NO_ERROR;
  DbpError           dbpError(0);
  OmsObjectContainer *pFlushVarHead             = NULL;
  OmsObjectContainer *pFlushVarHeadOtherClasses = NULL;
  OmsObjectContainer *pFreeHead                 = NULL;
  OMS_ContainerEntry *pContainerInfoVarObj      = NULL;

  int delCnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pDelOid_u        [OMS_MASS_COMMIT_CNT];
  OMS_ObjectId8          *pDelOid = reinterpret_cast<OMS_ObjectId8*>(&pDelOid_u[0]);
  tgg91_PageRef           pDelObjVers       [OMS_MASS_COMMIT_CNT];
  tgg00_FileId           *ppDelContainerId  [OMS_MASS_COMMIT_CNT];
  bool                    pDelReused        [OMS_MASS_COMMIT_CNT];  // PTS 1127661
  bool                    pDelContObj       [OMS_MASS_COMMIT_CNT];  // PTS 1124278
  int storeCnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pStoreOid_u       [OMS_MASS_COMMIT_CNT];
  OMS_ObjectId8          *pStoreOid = reinterpret_cast<OMS_ObjectId8*>(&pStoreOid_u[0]);
  tgg91_PageRef           pStoreObjVers     [OMS_MASS_COMMIT_CNT];
  size_t                  pStoreBodySize    [OMS_MASS_COMMIT_CNT];
  unsigned char          *ppStoreObj        [OMS_MASS_COMMIT_CNT];  
  size_t                  pKeyPos           [OMS_MASS_COMMIT_CNT];
  size_t                  pKeyLen           [OMS_MASS_COMMIT_CNT];
  tgg00_FileId           *ppStoreContainerId[OMS_MASS_COMMIT_CNT];
  bool                    pStoreReused      [OMS_MASS_COMMIT_CNT];  // PTS 1127661

  int lockUpdCnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pLockUpdOid_u     [OMS_MASS_COMMIT_CNT];
  OMS_ObjectId8          *pLockUpdOid = reinterpret_cast<OMS_ObjectId8*>(&pLockUpdOid_u[0]);  
  tgg91_PageRef           pLockUpdObjVers   [OMS_MASS_COMMIT_CNT];
  tgg00_FileId           *ppLockUpdContainerId[OMS_MASS_COMMIT_CNT];

  tgg00_BasisError        pDBError          [OMS_MASS_COMMIT_CNT];

  OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, "Start of " << msg);

#if defined(_ASSERT_OMS)
    // Check whether oid hash structure still fulfills some invariants. 
    // Check is normally switched off
    m_oidDir.HashCheck();
#endif

#ifdef OMS_CLUSTERED_COMMIT
  const bool clusterCommit = true;
  OMS_ClusteredOidHash hash(this);
  OMS_OidHash::OidIter iter1 = m_oidDir.First();
  while (iter1){
    OmsObjectContainer *p = iter1();
    ++iter1;
    hash.Insert(p);
  }
  OMS_ClusteredOidHash::Iter iter = hash.First();
#else
  const bool clusterCommit = false;
  OMS_OidHash::OidIter iter = m_oidDir.First();
#endif

  try {
    while (iter) {
      bool newPage = false;
      while (delCnt < OMS_MASS_COMMIT_CNT  && storeCnt < OMS_MASS_COMMIT_CNT && lockUpdCnt < OMS_MASS_COMMIT_CNT && iter){

        OmsObjectContainer *curr = iter();

#if OMS_CLUSTERED_COMMIT == 1
        newPage = iter.Next();
#else
        ++iter;
#endif

        OMS_ContainerEntry *pContainerInfo = curr->GetContainerInfoNoCheck();

        curr->SetBeforeImageMask(0);  // ??
        
        bool appendToFreeList = true;
        if (pContainerInfo->IsDropped()){
          // If container is dropped, then ignore possible changes of the object
        }
        else if (curr->ReplacedFlag() && !curr->VarObjFlag()){    // PTS 1125361
          // Object is replaced by a newer version and therefore nothing should be
          // done with this object besides deleting the object from the cache.
          // In case of var-objects, only the anchor object is reused, therefore
          // possible follow-objects must be freed.
        }
        else if (curr->DeletedFlag()){
          // Object is marked as deleted 
          if (curr->IsNewObject() && !curr->ReplacedFlag()) {
            ++newObjFlushed;
          }
          if (curr->VarObjFlag()){
            appendToFreeList = false;

            // Collect all variable sized objects in a list for later processing
            if (pContainerInfoVarObj == NULL){
              pContainerInfoVarObj = curr->GetContainerInfoNoCheck();
            }

            if (curr->GetContainerInfoNoCheck() == pContainerInfoVarObj){
              // Object belongs to the 'container' which is currently collected in the 
              // the list pFlushVarHead.
              curr->SetNext(pFlushVarHead);
              pFlushVarHead = curr;
              OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " append to var list1 (delete)");
            }
            else {
              // Object belongs to another container and therefore this object is inserted
              // into the 'mixed' list 
              curr->SetNext(pFlushVarHeadOtherClasses);
              pFlushVarHeadOtherClasses = curr;
              OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " append to var list2 (delete)");
            }
          }
          else {
            // Append entry to delete-array
            pDelOid         [delCnt] = curr->m_oid;
            pDelObjVers     [delCnt] = curr->m_objseq;
            ppDelContainerId[delCnt] = &pContainerInfo->GetFileId();
            pDelReused      [delCnt] = curr->ReusedFlag();                  // PTS 1127661
            pDelContObj     [delCnt] = false;
            ++delCnt; 
            OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " marked for mass delete");
          }
        }
        else if (curr->StoredFlag()){
          // Object is marked as to be stored
          if (curr->IsNewObject()) {
            ++newObjFlushed;
          }
          if (curr->VarObjFlag()){
            appendToFreeList = false;

            OMS_VarObjInfo *pObjInfo = reinterpret_cast<OMS_VarObjInfo*>(&curr->m_pobj); 

            // Check pattern to recognize writing behind the object.
            size_t sz = ((pObjInfo->m_vobjSize + 3) & ~3) + sizeof(tsp00_Uint4);
            if (reinterpret_cast<tsp00_Uint4*>(pObjInfo->m_pvobj+sz)[-1] != OMS_VAROBJ_CHECK_PATTERN){
              OMS_Globals::Throw(e_var_obj_overwritten, msg, curr->m_oid, __MY_FILE__, __LINE__, GetSession());
            }

            // Collect all variable sized objects in a list for later processing
            if (pContainerInfoVarObj == NULL){
              pContainerInfoVarObj = curr->GetContainerInfoNoCheck();
            }

            if (curr->GetContainerInfoNoCheck() == pContainerInfoVarObj){
              // Object belongs to the 'container' which is currently collected in the 
              // the list pFlushVarHead.
              curr->SetNext(pFlushVarHead);
              pFlushVarHead = curr;
              OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " append to var list1 (update)");
            }
            else {
              // Object belongs to another container and therefore this object is inserted
              // into the 'mixed' list 
              curr->SetNext(pFlushVarHeadOtherClasses);
              pFlushVarHeadOtherClasses = curr;
              OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " append to var list2 (update)");
            }
          }
          else {
            // Append entry to store-array
            pStoreOid         [storeCnt] = curr->m_oid;
            pStoreObjVers     [storeCnt] = curr->m_objseq;
            ppStoreContainerId[storeCnt] = &pContainerInfo->GetFileId();
            pStoreBodySize    [storeCnt] = pContainerInfo->GetClassEntry().GetPersistentSize();
            ppStoreObj        [storeCnt] = ((unsigned char*)&curr->m_pobj) + sizeof(void*);  
            pKeyPos           [storeCnt] = pContainerInfo->GetClassEntry().GetKeyDesc().GetPos();     // PTS 1122540
            pKeyLen           [storeCnt] = pContainerInfo->GetClassEntry().GetKeyDesc().GetLen();     // PTS 1122540
            pStoreReused      [storeCnt] = curr->ReusedFlag();                  // PTS 1127661
            ++storeCnt;
            OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " marked for mass update");

            curr->RemoveStoredFlag();  // ??
          }
        }
        else if (curr->LockedFlag() && !IsVersion()){
          // Append entry to lockUpd-array
          pLockUpdOid         [lockUpdCnt] = curr->m_oid;
          pLockUpdObjVers     [lockUpdCnt] = curr->m_objseq;
          ppLockUpdContainerId[lockUpdCnt] = &pContainerInfo->GetFileId();
          ++lockUpdCnt;
          OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " marked for mass lock-update");
       }

        // Append object to a list for later deletion of memory. 
        // For fixed sized objects this is only necessary if the context is not 
        // deleted as a whole. Variable sized objects are only added if they are
        // not later handled by the methods FlushVarObjMass or
        // DeleteVarObjMass. Those objects will be added in the corresponding 
        // method to the list.
        if ((emptyObjCache || curr->VarObjFlag()) && appendToFreeList) {
          curr->SetNext(pFreeHead);
          pFreeHead = curr;
          OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " appended to free list");
        }

        if (delCnt == OMS_MASS_COMMIT_CNT || (delCnt > 0 && (newPage || !iter))){
          // Mass-delete from the kernel
          GetSession()->IncDelete_lC();
          OMS_HResult hr = GetSession()->m_lcSink->DeleteObjMass(
            delCnt, &m_consistentView, ppDelContainerId, (unsigned char*) VersionContext(),
            (SAPDB_UInt8*)pDelOid, pDelObjVers, pDelReused, pDelContObj, clusterCommit,
            errCnt, pDBError);
          OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Execute mass delete with " << delCnt << " entries of page " << pDelOid[0].getPno());
          if (errCnt > 0){
            for (int i=0; i<delCnt; ++i){
              if (pDBError[i] != e_ok){
                OMS_Globals::Throw(pDBError[i], msg, pDelOid[i], __MY_FILE__, __LINE__, GetSession());
              }
            }
          }
          delCnt = 0;
        }

        if (storeCnt == OMS_MASS_COMMIT_CNT || (storeCnt > 0 && (newPage || !iter))){
          // Mass-store into the kernel
          GetSession()->IncStore_lC();
          OMS_HResult hr = GetSession()->m_lcSink->UpdateObjMass(
            storeCnt, &m_consistentView, ppStoreContainerId, (unsigned char*) VersionContext(),
            (SAPDB_UInt8*)pStoreOid, pStoreObjVers, pStoreBodySize, ppStoreObj, pKeyPos, pKeyLen, 
            pStoreReused, clusterCommit, errCnt, pDBError);
          OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Execute mass update with " << storeCnt << " entries of page " << pStoreOid[0].getPno());
          if (errCnt > 0){
            for (int i=0; i<storeCnt; ++i){
              if (pDBError[i] != e_ok){
                OMS_Globals::Throw(pDBError[i], msg, pStoreOid[i], __MY_FILE__, __LINE__, GetSession());
              }
            }
          }
          storeCnt = 0;
        }

        if (lockUpdCnt == OMS_MASS_COMMIT_CNT || (lockUpdCnt > 0 && (newPage || !iter))){
          // Mass-lock-update in the kernel
          OMS_HResult hr = GetSession()->m_lcSink->LockUpdObjMass(
            lockUpdCnt, &m_consistentView, ppLockUpdContainerId, (SAPDB_UInt8*)pLockUpdOid, 
            pLockUpdObjVers, clusterCommit, errCnt, pDBError);
          OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Execute mass lock-update with " << lockUpdCnt << " entries of page " << pLockUpdOid[0].getPno());
          if (errCnt > 0){
            for (int i=0; i<lockUpdCnt; ++i){
              if (pDBError[i] != e_ok){
                OMS_Globals::Throw(pDBError[i], msg, pLockUpdOid[i], __MY_FILE__, __LINE__, GetSession());
              }
            }
          }
          lockUpdCnt = 0;
        }
      }
    }

    // Flush variable sized objects
    if (pFlushVarHead)
      FlushVarObjMass(pFlushVarHead, pFlushVarHeadOtherClasses, pFreeHead);
  }
  catch (DbpError &e){
    // No call of dbpCaughtException() needed, as exception is thrown again later
    errorKind = OMS_EXCEPTION;
    dbpError  = e;
    OMS_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Error caught: " << e.m_errorNo << e.m_errorText);
  }
  catch (BAD_ALLOC_GEO573&){
    // No call of dbpCaughtBadAlloc() needed, as exception is thrown again later
    errorKind = OMS_BAD_ALLOC;
    OMS_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Bad-Alloc caught");
  }
   
  if (errorKind != OMS_NO_ERROR){
    // Append objects, which haven't been considered so far, to the list of
    // objects for which memory should be freed
    while (iter){
      OmsObjectContainer *curr = iter();
#if OMS_CLUSTERED_COMMIT == 1
      iter.Next();
#else
      ++iter;
#endif

      if (emptyObjCache || curr->VarObjFlag()){
        curr->SetNext(pFreeHead);
        pFreeHead = curr;
        OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << curr->m_oid << " appended to free list");
      }
    }

    while (pFlushVarHead){
      OmsObjectContainer *next = pFlushVarHead->GetNext();
      pFlushVarHead->SetNext(pFreeHead);
      pFreeHead = pFlushVarHead;
      pFlushVarHead = next;
    }
    while (pFlushVarHeadOtherClasses){
      OmsObjectContainer *next = pFlushVarHeadOtherClasses->GetNext();
      pFlushVarHeadOtherClasses->SetNext(pFreeHead);
      pFreeHead = pFlushVarHeadOtherClasses;
      pFlushVarHeadOtherClasses = next;
    }
  }

  // Release all memory of objects  
  while (pFreeHead){
    OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Free memory of object: " << pFreeHead->m_oid);
    OmsObjectContainer *pNext = pFreeHead->GetNext();
    if (pFreeHead->VarObjFlag()){
      reinterpret_cast<OMS_VarObjInfo*>(&pFreeHead->m_pobj)->freeVarObj(this);
    }
    if (emptyObjCache){
      OMS_ContainerEntry *pContainerInfo = pFreeHead->GetContainerInfoNoCheck();
      pContainerInfo->ChainFree(pFreeHead, 21);
    }
    pFreeHead = pNext;
  }

  ClearObjCache(/*deleteObjects=*/false, /*adaptOidHash=*/false);
  GetContainerDir().ClearNewVersionObjects();
  GetContainerDir().ClearReusableOids();    // PTS 1127661

  if (errorKind == OMS_EXCEPTION) {
    ResetNewObjectsToFlush();
    throw dbpError;
  }
  else if (errorKind == OMS_BAD_ALLOC){
    ResetNewObjectsToFlush();
    throw BAD_ALLOC_GEO573();
  }
  else if ((!IsVersion()) && (newObjFlushed != m_cntNewObjectsToFlush)){
    // Check whether number of new flushed objects match the expected number,
    // which is stored as variable in the context 
    OMS_TRACE(omsTrFlushCache | omsTrNewObj, GetSession()->m_lcSink, 
      "OMS_Context::FlushObjCache : flushed new objects: "
      << newObjFlushed << ", expected to flush: " << m_cntNewObjectsToFlush);
    DbpBase b(GetSession()->m_lcSink);
    b.dbpOpError ("Flushed new objects           : %d", newObjFlushed);
    b.dbpOpError ("expected new objects to flush : %d", m_cntNewObjectsToFlush);

    //m_cntNewObjectsToFlush = 0;
    ResetNewObjectsToFlush();
    OMS_Globals::Throw(e_missing_object_flush, msg, __MY_FILE__, __LINE__, GetSession());
  }
  ResetNewObjectsToFlush();
}

/*----------------------------------------------------------------------*/

/// Flushes a list of var objects 
/*!
** This method flushes a list of objects with variable size to the kernel; the objects
** are either updated or deleted in the kernel.
** The method can be split roughly into 4 phases:
** In phase ONE for each object it is checked how many pages are needed to store the 
** object and how many pages are already assigned to the object. (Objects which are marked
** as deleted need 1 page (because of program logic) which is then deleted in phase 4.
** If the object owns more pages than it needs, than these pages are appended to a freelist. 
** Objects which needs more pages tries to get them from the freelist and if there are not enough, 
** then these objects are collected in a separate list for later processing.
** In phase TWO new pages are allocated per mass operation from the kernel for all those
** objects which still need pages. After the pages have been allocated, these pages are
** assigned to the objects which needs additional pages.
** If there had been more pages assigned to the object than are needed to store them,
** then these superfluous pages are freed per mass-operation in phase THREE.
** This means that there will never be phase TWO and phase THREE active in the same call;
** furthermore if the sizes don't change, then none of them will be needed.
** Finally in phase FOUR the objects are updated resp. deleted per mass-operations in the kernel.
**
** To support the reuse of pages (in phase ONE and TWO) it is necessary, that all objects
** considered in this phase belong to the same schema, class, and container. Therefore
** the objects to be flushed, are divided into two list: one which contains only objects
** belonging to the same schema, class and container and the other list, which contains
** all the rest. The algorithm (all 4 phases) is executed solely for the first list and
** at the end it is checked whether there are other objects left and if so, this list
** is split again into one with only one class and the rest.
** This means, that if there are a very large number of different schema, class, container
** combinations, then the algorithm will have a quadratic running time and should be
** changed.
**
** \param pHeadOneClass Pointer to the head of a list of the objects which should be flushed.
**    All objects in this list must belong to the same schema and container number.
** \param pHeadOtherClasses If there are objects which belong to other classes than the
**    objects of the list pHeadOneClass, then these objects are listed in this list.
** \param pFreeList All processed objects are linked to this list. So this list can be 
**    used to free the memory which is allocated by the corresponding objects.
**
** \assumption All objects in the list, headed by pHead, belong to the same
** schema, class, and container
**
** \attention After normal execution the lists pHeadOneClass and pHeadOtherClasses should
** be empty. But in case of an exception, this might not be true, and therefore they are 
** passed as reference.
**
** \since PTS 1121449
*/
void OMS_Context::FlushVarObjMass (OmsObjectContainer  *&pHeadOneClass, 
                                   OmsObjectContainer  *&pHeadOtherClasses,
                                   OmsObjectContainer  *&pFreeList) 
{
  const char *msg = "OMS_Context::FlushVarObjMass ";

  int cnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pOid_u         [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8          *pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef           pObjVers       [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo           pUpdTransId    [OMS_MASS_OPERATION_CNT];
  tgg00_FileId           *ppContainerId  [OMS_MASS_OPERATION_CNT];
  size_t                  pObjSize       [OMS_MASS_OPERATION_CNT];
  size_t                  pBodySize      [OMS_MASS_OPERATION_CNT];
  unsigned char          *ppObj          [OMS_MASS_OPERATION_CNT];
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pNextOid_u     [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8          *pNextOid = reinterpret_cast<OMS_ObjectId8*>(&pNextOid_u[0]);
  short                   pDBError       [OMS_MASS_OPERATION_CNT];
  bool                    pContObj       [OMS_MASS_OPERATION_CNT];
  bool                    pReused        [OMS_MASS_OPERATION_CNT];

  int lockCnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pLockOid_u         [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8          *pLockOid = reinterpret_cast<OMS_ObjectId8*>(&pLockOid_u[0]);
  tgg91_PageRef           pLockObjVers       [OMS_MASS_OPERATION_CNT];
  tgg00_FileId           *ppLockContainerId  [OMS_MASS_OPERATION_CNT];

  int delCnt = 0;
  // Prevent the calling of the constructor for every entry. An assignment is done in the loop.
  OmsTypeUInt8            pDelOid_u         [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8          *pDelOid = reinterpret_cast<OMS_ObjectId8*>(&pDelOid_u[0]);
  tgg91_PageRef           pDelObjVers       [OMS_MASS_OPERATION_CNT];
  tgg00_FileId           *ppDelContainerId  [OMS_MASS_OPERATION_CNT];
  bool                    pDelReused        [OMS_MASS_OPERATION_CNT];  // PTS 1127661
  bool                    pDelContObj       [OMS_MASS_OPERATION_CNT];

  OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "Start FLUSH of VAR-Objects");
  while (pHeadOneClass){
    int                 errCnt              = 0;
    int                 availInfoNodeCnt    = 0;          
    int                 neededInfoNodeCnt   = 0;
    size_t              remainingSize       = 0;
    unsigned char      *pCurrAddr           = NULL;
    OmsObjectContainer *pNext               = NULL;
    OmsObjectContainer *pPrev               = NULL;
    OmsObjectContainer *pObjContainer       = NULL;
    OmsObjectContainer *pUnfinishedObjs     = NULL;
    OMS_VarObjInfo     *pObjInfo            = NULL;
    OMS_VarObjInfoNode *pFirstInfoNode      = NULL;
    OMS_VarObjInfoNode *pAvailInfoNodes     = NULL;

    // All Objects share the same ContainerInfo. See assumption!
    OMS_ContainerEntry   *pContainerInfo = pHeadOneClass->GetContainerInfoNoCheck();
    OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "Schema: " << pContainerInfo->GetSchema()
      << " Class: " << pContainerInfo->GetClassEntry().GetClassName()
      << " Container: " << pContainerInfo->GetContainerNo());

    // ********* PHASE I **********
    OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "Start of PHASE I");

    pObjContainer = pHeadOneClass;
    while (pObjContainer){
      pNext = pObjContainer->GetNext();

      pObjInfo       = REINTERPRET_CAST (OMS_VarObjInfo*, &pObjContainer->m_pobj);
      pFirstInfoNode = &pObjInfo->m_vobjInfo;

      // Compute number of pages needed to store the object.
      int pagesNeeded = 0;
      if (pObjContainer->DeletedFlag()){
        pagesNeeded = 1;
      }
      else {
        // pagesNeeded = floor (pObjInfo->m_vobjSize / VAR_OBJ_CHUNK_SIZE);
        pagesNeeded = (int)((pObjInfo->m_vobjSize + VAR_OBJ_CHUNK_SIZE - 1) / VAR_OBJ_CHUNK_SIZE);
      }

      // Determine number of pages which are currently assigned to the object
      int pagesCurr = 0;
      if (pFirstInfoNode->m_nodeCnt > 0){
        // Normally the number of infonodes is stored in this variable
        pagesCurr = pFirstInfoNode->m_nodeCnt;
      }
      else {
        // In the very unlikely case, that the size of a single object is larger then 0.5 GB
        // then the variable is too small to store the number and therefore the number is 
        // determined by running over the chain
        OMS_VarObjInfoNode *p = pFirstInfoNode;
        pagesCurr = 1;
        while (p->m_vobjNext){
          p = p->m_vobjNext;
          ++pagesCurr;
        }
      }
      OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink,
          "  Object: " << pObjContainer->m_oid
          << " pages existing: " << pagesCurr  << " pages needed: " << pagesNeeded); 

      if (pagesNeeded < pagesCurr) {
        // The objects owns more infonodes as it needs to store the current data.

        // Position the pointer on the last infonode which is needed
        OMS_VarObjInfoNode *pLastNeededInfoNode = pFirstInfoNode;
        for(int i=0; i<pagesNeeded-1; ++i) {
          pLastNeededInfoNode  = pLastNeededInfoNode->m_vobjNext;
        }

        // Append superfluous infonodes to a separate list for possible later reuse
        OMS_VarObjInfoNode *pLastInfoNode = pLastNeededInfoNode;
        while (pLastInfoNode->m_vobjNext){
          pLastInfoNode = pLastInfoNode->m_vobjNext;
          ++availInfoNodeCnt;
          OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "    Infonode: " << pLastInfoNode->m_vobjOid << "  appended to freelist ");
        }
        // pLastNeededInfoNode is still pointing to the last element which is needed and
        // pLastInfoNode is now pointing to the last infonode for the current object. 
        // Append already existing list behind this object and initialize head 
        // with new starting point of the list.
        pLastInfoNode->m_vobjNext   = pAvailInfoNodes;
        pAvailInfoNodes             = pLastNeededInfoNode->m_vobjNext;

        // Terminate the list of infonodes for the current object
        pLastNeededInfoNode->m_vobjNext = NULL;
      }
      else if (pagesNeeded > pagesCurr) {
        // The object does not own enough infonodes to store the current data

        if (pagesNeeded-pagesCurr > availInfoNodeCnt){
          // There are not enough infonodes in the available list for the current
          // object. Therefore store the object in a separate list for later processing.
          if (pPrev == NULL)
            pHeadOneClass             = pNext;
          else
            pPrev->SetNext(pNext);
          pObjContainer->SetNext(pUnfinishedObjs);
          pUnfinishedObjs             = pObjContainer;
          neededInfoNodeCnt          += (pagesNeeded-pagesCurr);

          OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "    pages in freelist " << availInfoNodeCnt << " => Objects is marked for later processing");
        }
        else {
          // Position the pointer at the end of the chain
          OMS_VarObjInfoNode *pLastInfoNode = pFirstInfoNode;
          while (pLastInfoNode->m_vobjNext){
            pLastInfoNode = pLastInfoNode->m_vobjNext;
          }

          // Take infonodes from the reuse list
          for (int i=0; i<pagesNeeded-pagesCurr; ++i) {
            OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "    Infonode: " << pAvailInfoNodes->m_vobjOid << "  appended to object ");

            pLastInfoNode->m_vobjNext = pAvailInfoNodes;
            pAvailInfoNodes           = pAvailInfoNodes->m_vobjNext;
            pLastInfoNode             = pLastInfoNode->m_vobjNext;
            pLastInfoNode->m_vobjNext = NULL;
            --availInfoNodeCnt;
          }
        }
      }

      if (!(pNext == NULL && pUnfinishedObjs != NULL)){
        // Initialization for next iteration
        if (pObjContainer != pUnfinishedObjs){
          // Remember previous entry in the original list. Necessary for deletion of entries out
          // of the single linked list. This entry only changes if the last object was not appended
          // to the list of unfinished objects
          pPrev = pObjContainer;
        }
        pObjContainer = pNext;
      }
      else {
        // ********* PHASE II **********
        OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "Start of PHASE II");

        // All objects have been considered, but there are still some objects which does
        // not own enough infonodes for their current object size. Either take infonodes
        // from the availableInfoNode list or if there are not enough allocate some from 
        // the kernel.

        OMS_VarObjInfoNode *pInfoNode = NULL;
        tgg91_PageRef       ObjVers;
        while (neededInfoNodeCnt > availInfoNodeCnt){
          // Create new infonodes, with oid ...
          if (IsVersion()){
            // ... from the version
            while (neededInfoNodeCnt > availInfoNodeCnt){
              OMS_ObjectId8   newOid = VersionNewOid();
              OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Infonode: " << newOid << "  allocated in version");
              ObjVers.gg91SetNilRef();
              pInfoNode             = new(*this) OMS_VarObjInfoNode(newOid, ObjVers);
              ++availInfoNodeCnt;

              pInfoNode->m_vobjNext = pAvailInfoNodes;
              pAvailInfoNodes       = pInfoNode;
            }
          }
          else {
            // ... from the kernel
            cnt = 0;
            while (cnt < OMS_MASS_OPERATION_CNT && neededInfoNodeCnt > availInfoNodeCnt){
              ppContainerId[cnt] = &pContainerInfo->GetFileId();
              pContObj[cnt]      = true;
              ++cnt;
              ++availInfoNodeCnt;
            }
            
            // Mass-New from the kernel
            GetSession()->m_lcSink->NewObjMass(cnt, &m_consistentView, ppContainerId, NULL, NULL, NULL, NULL,
              pContObj, (SAPDB_UInt8*)pOid, pObjVers, pUpdTransId, errCnt, pDBError);

            // Error handling
            if (errCnt > 0){
              for (int j=0; j<cnt; ++j){
                if (pDBError[j] != e_ok){
                  OMS_Globals::Throw(pDBError[j], msg, pOid[j], __MY_FILE__, __LINE__, GetSession());
                }
              }
            }

            // Create new infonodes and append then to the list of available infonodes
            for (int j=0; j<cnt; ++j){
              pInfoNode             = new(*this) OMS_VarObjInfoNode(pOid[j], pObjVers[j]);
              pInfoNode->m_newFlag  = true; 
              pInfoNode->m_vobjNext = pAvailInfoNodes;
              pAvailInfoNodes       = pInfoNode;
              OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Infonode: " << pOid[j] << "  allocated in kernel");
            }
          }
        }

        // Now append list with unfinished objects to original list and continue processing
        // with these objects. So these objects are considered twice.
        if (pObjContainer == pUnfinishedObjs){
          if (pPrev != NULL)
            pPrev->SetNext(pUnfinishedObjs);
          else
            pHeadOneClass = pUnfinishedObjs;
        }
        else {
          pObjContainer->SetNext(pUnfinishedObjs);
          pPrev = pObjContainer;
        }
        pObjContainer   = pUnfinishedObjs;
        pUnfinishedObjs = NULL;
      } 
    }


    // ********* PHASE III **********
    OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "Start of PHASE III");

    cnt     = 0;
    lockCnt = 0;
    // Remove infonodes which are not needed anymore
    while (pAvailInfoNodes){
      while (cnt < OMS_MASS_OPERATION_CNT && pAvailInfoNodes){
        if (!IsVersion()) {
          // Remember entries for locking
          pLockOid         [lockCnt] = pAvailInfoNodes->m_vobjOid;
          pLockObjVers     [lockCnt] = pAvailInfoNodes->m_vobjSeq;
          ppLockContainerId[lockCnt] = &pContainerInfo->GetFileId();
          ++lockCnt;
          OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Infonode: " << pAvailInfoNodes->m_vobjOid << "  locked");
        }

        // Append entry to delete-array
        pOid         [cnt] = pAvailInfoNodes->m_vobjOid;
        pObjVers     [cnt] = pAvailInfoNodes->m_vobjSeq;
        ppContainerId[cnt] = &pContainerInfo->GetFileId();
        pReused      [cnt] = false; 
        pContObj     [cnt] = true;
        ++cnt; 
        OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Infonode: " << pAvailInfoNodes->m_vobjOid << "  deleted");

        OMS_VarObjInfoNode *p = pAvailInfoNodes;
        pAvailInfoNodes = pAvailInfoNodes->m_vobjNext;
        p->deleteSelf(*this);
      }

      if (cnt == OMS_MASS_OPERATION_CNT || (cnt > 0 && pAvailInfoNodes == NULL)){
        // Mass lock
        tsp00_Int8 timeout = 0;
        OMS_HResult hr = GetSession()->m_lcSink->LockObjMass(
          lockCnt, &m_consistentView, ppLockContainerId, (SAPDB_UInt8*)pLockOid, 
          pLockObjVers, timeout, errCnt, pDBError);
        LockResult(lockCnt, pDBError, NULL, pLockOid, msg, /*shared=*/false);
        lockCnt = 0;

        // Mass deletion
        hr = GetSession()->m_lcSink->DeleteObjMass(
          cnt, &m_consistentView, ppContainerId, (unsigned char*) VersionContext(),
          (SAPDB_UInt8*)pOid, pObjVers, pReused, pContObj, /*clustered=*/false, 
          errCnt, pDBError);
        if (errCnt > 0){
          for (int i=0; i<cnt; ++i){
            if (pDBError[i] != e_ok){
              OMS_Globals::Throw(pDBError[i], msg, pOid[i], __MY_FILE__, __LINE__, GetSession());
            }
          }
        }
        cnt = 0;
      }
    }


    // ********* PHASE IV **********
    OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "Start of PHASE IV");

    OMS_VarObjInfoNode *pInfoNode = NULL;
    pObjContainer = pHeadOneClass;
    cnt           = 0;
    lockCnt       = 0;
    delCnt        = 0;
    while (pObjContainer) {
      while (pObjContainer && cnt < OMS_MASS_OPERATION_CNT && delCnt < OMS_MASS_OPERATION_CNT){

        if (pInfoNode == NULL){
          // Consider first/next object in list
          pObjInfo      = REINTERPRET_CAST (OMS_VarObjInfo*, &pObjContainer->m_pobj);
          pInfoNode     = &pObjInfo->m_vobjInfo; 
          pCurrAddr     = pObjInfo->m_pvobj;
          remainingSize = pObjInfo->m_vobjSize;
        }

        if (pObjContainer->StoredFlag()){
          while (NULL != pInfoNode && cnt < OMS_MASS_OPERATION_CNT) {
            if (!IsVersion() && pObjContainer->m_oid != pInfoNode->m_vobjOid && !pInfoNode->m_newFlag){            
              // Entries must be locked if no version is active and if either the current infonode is
              // a following node (which is not newly created) or if the base node is not locked.
              pLockOid         [lockCnt] = pInfoNode->m_vobjOid;
              pLockObjVers     [lockCnt] = pInfoNode->m_vobjSeq;
              ppLockContainerId[lockCnt] = &pContainerInfo->GetFileId();
              ++lockCnt;
              OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Infonode: " << pInfoNode->m_vobjOid << "  locked");
            }

            // Insert entry into array
            pOid         [cnt] = pInfoNode->m_vobjOid;
            pObjVers     [cnt] = pInfoNode->m_vobjSeq;
            ppContainerId[cnt] = &pContainerInfo->GetFileId();
            pObjSize     [cnt] = pObjInfo->m_vobjSize;
            pBodySize    [cnt] = (remainingSize > VAR_OBJ_CHUNK_SIZE ? VAR_OBJ_CHUNK_SIZE : remainingSize);
            ppObj        [cnt] = pCurrAddr;
            pContObj     [cnt] = (pObjContainer->m_oid != pInfoNode->m_vobjOid);  // PTS 1124278
            // PTS 1127661 
            // Reusing (with an incremented generation counter) is only active for the anchor objects.
            pReused      [cnt] = pObjContainer->ReusedFlag() && (pObjContainer->m_oid == pInfoNode->m_vobjOid);

            if (pInfoNode->m_vobjNext){
              pNextOid   [cnt] = pInfoNode->m_vobjNext->m_vobjOid;
            }
            else {
              pNextOid   [cnt].setNil();
            }
            OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Infonode: " << pInfoNode->m_vobjOid << "  updated");

            pCurrAddr      += pBodySize[cnt]; 
            remainingSize  -= pBodySize[cnt]; 
            ++cnt; 
            pInfoNode = pInfoNode->m_vobjNext;
          }
        }

        else if (pObjContainer->ReplacedFlag()){
          // As oid is reused by another object, the kernel must not be informed about the deletion of the object.
          // In phase I all following objects have already been appended to the reuse list. 
          pInfoNode = NULL;
          OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Object: " << pObjContainer->m_oid << "  has been replaced");
        }

        else  if (pObjContainer->DeletedFlag()){
          // In phase I all following objects have already been appended to the reuse list.  Therefore only the
          // anchor object itself must be considered.
          // Append entry to delete-array
          pDelOid         [delCnt] = pObjContainer->m_oid;
          pDelObjVers     [delCnt] = pObjContainer->m_objseq;
          ppDelContainerId[delCnt] = &pContainerInfo->GetFileId();
          pDelReused      [delCnt] = pObjContainer->ReusedFlag();
          pDelContObj     [delCnt] = false;
          ++delCnt; 
          pInfoNode = NULL;
          OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "  Object: " << pObjContainer->m_oid << "  deleted");
        }


        if (pInfoNode == NULL){
          // Previous object is completely flushed, therefore consider next object
          pNext = pObjContainer->GetNext();

          // Append object to list for later freeing of memory
          pObjContainer->SetNext(pFreeList);
          pFreeList = pObjContainer;
          OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, " Object " << pObjContainer->m_oid << " appended to free list");

          pObjContainer = pNext; 
          pHeadOneClass = pObjContainer;
        }
      }

      if (cnt == OMS_MASS_OPERATION_CNT || (cnt > 0 && pObjContainer == NULL)){
        int errCnt = 0;

        // Mass lock
        if (lockCnt > 0){
          tsp00_Int8 timeout = 0;
          OMS_HResult hr = GetSession()->m_lcSink->LockObjMass(
            lockCnt, &m_consistentView, ppLockContainerId, (SAPDB_UInt8*)pLockOid, 
            pLockObjVers, timeout, errCnt, pDBError);
          LockResult(lockCnt, pDBError, NULL, pLockOid, msg, /*shared=*/false);
          lockCnt = 0;
        }

        // Mass update
        OMS_HResult hr = GetSession()->m_lcSink->UpdateVarObjMass(
          cnt, &m_consistentView, ppContainerId, (unsigned char*) VersionContext(), (SAPDB_UInt8*)pOid, pObjVers, 
          pObjSize, pBodySize, ppObj, (SAPDB_UInt8*)pNextOid, pContObj, pReused, errCnt, pDBError);
        if (errCnt > 0){
          for (int i=0; i<cnt; ++i){
            if (pDBError[i] != e_ok){
              OMS_Globals::Throw(pDBError[i], msg, pOid[i], __MY_FILE__, __LINE__, GetSession());
            }
          }
        }
        cnt = 0;
      }

      if (delCnt == OMS_MASS_OPERATION_CNT || (delCnt > 0 && pObjContainer == NULL)){
        int errCnt = 0;
        GetSession()->IncDeleteVarObjectLC ();

        // Mass deletion
        OMS_HResult hr = GetSession()->m_lcSink->DeleteObjMass(
          delCnt, &m_consistentView, ppDelContainerId, (unsigned char*) VersionContext(),
          (SAPDB_UInt8*)pDelOid, pDelObjVers, pDelReused, pDelContObj, /*clustered=*/false,
          errCnt, pDBError);
        if (errCnt > 0){
          for (int i=0; i<delCnt; ++i){
            if (pDBError[i] != e_ok){
              OMS_Globals::Throw(pDBError[i], msg, pDelOid[i], __MY_FILE__, __LINE__, GetSession());
            }
          }
        }
        delCnt = 0;
      }
    }

    pHeadOneClass = NULL;
    if (pHeadOtherClasses){
      // Take all objects which belong to the same class (as the first object) from the 
      // list pHeadOtherClasses and insert them into the list pHeadOneClass for next 
      // iteration of algorithm. All objects which do not belong to this class remain
      // in the old list. 
      // Assumption: It is assumed that (almost) all objects, which are stored in the local
      //   cache during a single transaction, belong to the same container. If there are many
      //   containers for var-objects this algorithm must be changed because of its
      //   quadratic running time in such a case.
      OMS_ContainerEntry   *pContainerInfo = pHeadOtherClasses->GetContainerInfoNoCheck();

      OmsObjectContainer *next = NULL;
      OmsObjectContainer *prev = NULL;
      OmsObjectContainer *curr = pHeadOtherClasses;
      while (curr){
        next = curr->GetNext();
        if (curr->GetContainerInfoNoCheck() == pContainerInfo){
          if (prev)
            prev->SetNext(next);
          else 
            pHeadOtherClasses = curr->GetNext();
          curr->SetNext(pHeadOneClass); 
          pHeadOneClass = curr;
        }
        else {
          prev = curr;
        }
        curr = next;
      }
    }
  }

  OMS_DETAIL_TRACE(omsTrFlushCache | omsTrVarObject, GetSession()->m_lcSink, "End FLUSH of VAR-Objects");
}

/*----------------------------------------------------------------------*/
// PTS 1122280
void OMS_Context::FlushVersionObjCache(bool isCommit) 
{
  const char *msg = "OMS_Context::FlushVersionObjCache ";

  OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, "Start of " << msg << " isCommit=" << (isCommit?"Y":"N"));

#if defined(_ASSERT_OMS)
      // Check whether oid hash structure still fulfills some invariants. 
      // Check is normally switched off
        m_oidDir.HashCheck();
#endif

  int         phase=0;
  OMS_ObjectId8 oid;

  try {
    OMS_OidHash::OidIter iter = m_oidDir.First();
    while (iter) {
      OmsObjectContainer *curr = iter();
      oid = curr->m_oid;
      ++iter;

      /* This is needed, as in versions the level 1 is not used for sub-transactions, 
      ** but for the basic transactions. Because of this the before images might get
      ** freed in TransEndEpilog, but the corresponding flag in the after-images will
      ** not be reset. As this method is iterating over all entries, the reset of the
      ** flag is executed here.
      */
      curr->SetBeforeImageMask(0);

      if (IsVersionOid(curr->m_oid) 
          && ((curr->DeletedFlag() && isCommit) || (curr->NewFlag() && !isCommit))) {
        // Object is created in a version and marked as deleted
        // or the object is newly created and the transaction is rolled back
        OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, "  Delete object " << curr->m_oid);

        phase=1;
        OMS_ContainerEntry* pContainerInfo = curr->GetContainerInfoNoCheck();

        // Delete entry in the oid-directory. Do not adapt key-structure as this is done
        // in the method VersionDelKey
        phase=2;
        m_oidDir.HashDelete(curr->m_oid, false);

        if (pContainerInfo->GetClassEntry().IsKeyedObject()){
          // Check is necessary as there might be several objects with the same key,
          // but only one with the delete-flag = false. If there are several entries
          // with the same key, then the corresponding entry in the key-tree is pointing 
          // to the non-deleted object and therefore it must be checked whether the
          // current object is equal with the object the tree-entry is pointing to. 
          // ASSUMPTION: The number of objects with the delete flag is quite small 
          //   compared with the overall number. Therefore the running time of O(2*m*log(n))
          //   (where m is the number of deleted objects and n is the number of objects
          //   in the version) is acceptable. If it is expected, that the number of deleted
          //   objects is about the number of objects, then it is better to remember the
          //   non-deleted objects, drop the whole tree and reinsert the remembered objects
          //   again. Unfortunately it is not possible to iterate over the tree and delete
          //   all objects which are marked as deleted (running time O(n)), because the 
          //   iterator will get confused when updates are executed during iteration. 
          if (curr == pContainerInfo->VersionFindKey(pContainerInfo->GetClassEntry().GetKeyPtr(curr))) {
            // Delete entry out of the tree with version objects
            phase=3;
            pContainerInfo->VersionDelKey(curr);
          }
        }

        if (curr->IsVerNewObject()){
          // Delete entry out of the list of version objects
          phase=4;
          pContainerInfo->RemoveNewVersionObject(curr);
        }

        // Append object frame to free-list
        phase=5;
        pContainerInfo->ChainFree(curr, 22);
      }
    }
  }
  catch (DbpError &e){
    int error = e.m_errorNo;  // Assignment just to prevent compiler warning
    DbpBase opMsg(GetSession()->m_lcSink);
    opMsg.dbpOpError("%s: Error %d (%s) in phase %d", msg, e.dbpDBError(), e.m_errorText, phase);  
    throw;
  }
  OMS_DETAIL_TRACE(omsTrFlushCache, GetSession()->m_lcSink, "End of " << msg);
}

/*----------------------------------------------------------------------*/

void OMS_Context::MarkNotBoundToTrans() { 
  m_boundToTrans = false;
  if (!m_isOpen) {
    OMS_Globals::m_globalsInstance->m_versionDictionary.MarkUnloadable(this);
  }
  SetSession(NULL);
  GetContainerDir().ForbitChangeOfClassEntryPtr();
}

/*----------------------------------------------------------------------*/

void  OMS_Context::OpenVersion(OMS_Session* session) {
  session->m_lcSink->GetDateTime(&m_lastOpenDate, &m_lastOpenTime);
  m_isOpen     = true;
}

/*----------------------------------------------------------------------*/

void OMS_Context::ReduceHeapUsage() /* PTS 1110149 */
{
  DbpBase b(this->GetSession()->m_lcSink);
  char versionId[sizeof(GetVersionId())+1];
  SAPDB_MemCopyNoCheck(&versionId[0], &GetVersionId()[0], sizeof(OmsVersionId));
  versionId[sizeof(OmsVersionId)] = 0;
  b.dbpOpMsg("Unloading oms version %s to reduce heap usage", &versionId[0]);

  this->UnLoad(/*badAllocHandling=*/false);
  GetContainerDir().UnregisterAll();
}

/*----------------------------------------------------------------------*/

/* PTS 1115134 */

void  OMS_Context::ResetVersion(OMS_Session& session) 
{
  if (m_isVersion)
  {
    OMS_ContainerHandle NilContainerHandle;  //PTS 1124356 FF 2003-09-23 new FileDir
    EmptyObjCache(NilContainerHandle);       //PTS 1124356 FF 2003-09-23 new FileDir
    ClearReadOnlyObjCache(/*deleteObjects=*/true);
    
    for (OMS_ContainerDirectory::Iter iter = GetContainerDir().First(); iter; ++iter){
      OMS_ContainerEntry *pContainerEntry = iter();
      pContainerEntry->VersionDelIndex(false);
    }

    tgg00_BasisError DBError;
    session.m_lcSink->ResetVersion((unsigned char*) &m_versionContext, DBError);
    CheckDBError(DBError, "OMS_Context::ResetVersion", OMS_ObjectId8());
  }
}

/*----------------------------------------------------------------------*/

void OMS_Context::UnLoad(bool badAllocHandling) 
{
  m_pVersionContext = &m_versionContext;
  FlushObjCache(/*emptyObjCache=*/true);  // No hash-adaptation PTS 1118855 
  ClearReadOnlyObjCache(/*deleteObjects=*/true);
  
  if (!badAllocHandling){
    // As in 'HashResize' memory for the new hash array will be allocated, this method must
    // not be called when there was already a bad-alloc. Otherwise this may end in a deadlock
    // as the r/w region, which protects the structure of the unloadable versions would be
    // entered twice.
    m_oidDir.HashResize(DEFAULT_OMS_HASH_SIZE, /*rehash=*/false);  
  }

  GetContainerDir().ClearFreeList(3);
  for (OMS_ContainerDirectory::Iter iter = GetContainerDir().First(); iter; ++iter) {
    iter()->VersionDelIndex(false);
  }
  OMS_Globals::GetKernelInterface()->IncOmsVersionUnloadCounter();
}

/*----------------------------------------------------------------------*/

void  OMS_Context::VersionClearObjCache(bool changedObjAreNotAllowed) 
{
  for (OMS_OidHash::OidIter iter = m_oidDir.First(); iter; ++iter) {
    OmsObjectContainer *p = iter();
    ++iter;

    if (p->StoredFlag() || p->DeletedFlag()) {
      if (changedObjAreNotAllowed){
        CheckDBError(e_transaction_end_required, "OMS_Context::VersionClearObjCache", p->m_oid); 
      }
      else {
        // skip object
      }
    }
    else if (p->LockedFlag()){
      if (changedObjAreNotAllowed){
        FlushObj(p);
      }
      else {
        // skip object
      }
    }
    else {
      DeleteObjInContext(p, p->GetContainerInfo(__MY_FILE__, __LINE__));
    }
  }  

  // Clear the read-only structure
  ClearReadOnlyObjCache(/*deleteObjects=*/true);
}

/*----------------------------------------------------------------------*/

void OMS_Context::NewConsistentView() {
  tsp00_Int2 DBError;

  tgg91_TransNo consistentView = m_consistentView;
  OMS_HResult hr = GetSession()->m_lcSink->NewConsistentView (this->IsVersion(), 
	(unsigned char*) &m_consistentView, &DBError);
  if (0 != DBError) {
    m_consistentView = consistentView;
    GetSession()->ThrowDBError (DBError, "omsNewConsistentView", OMS_ObjectId8(), __MY_FILE__, __LINE__);
    return;
  }
  GetSession()->ChangedConsistentView();
}

/*----------------------------------------------------------------------*/
/*! Try to aquire locks for all objects in the local oms-cache, for which
**  there exists before-images, but which are not locked. 
**  This might happen, as it is possible to get an 'forupd'-pointer without
**  getting a lock (see PTS 1128108).
*/
void OMS_Context::LockObjWithBeforeImage()   
{
  const char* msg = "OMS_Context::LockObjWithBeforeImage";

  int                  cnt        = 0;
  int                  errCnt     = 0;
  OmsObjectContainer  *pObj       = NULL;

  OmsTypeUInt8            pOid_u         [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8          *pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef           pObjVers       [OMS_MASS_OPERATION_CNT];
  tgg00_FileId           *ppContainerId  [OMS_MASS_OPERATION_CNT];
  char                    pContainerId_u [OMS_MASS_OPERATION_CNT * sizeof(OMS_UnknownContainerId)];
  OMS_UnknownContainerId *pContainerId = reinterpret_cast<OMS_UnknownContainerId*>(&pContainerId_u[0]);
  tgg00_BasisError        pDBError       [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer     *ppObj          [OMS_MASS_OPERATION_CNT];

  OMS_OidHash::OidIter iter       = m_oidDir.First();
  while (iter) {
    while (iter && cnt < OMS_MASS_OPERATION_CNT) {
      pObj = iter();

      if (pObj->GetBeforeImageMask() != 0 && !pObj->LockedFlag()){
        pOid         [cnt] =  pObj->m_oid;
        pObjVers     [cnt] =  pObj->m_objseq;
        ppContainerId[cnt] = &pObj->m_pContainerInfo->GetFileId();
        ppObj        [cnt] =  pObj;
        ++cnt;
      }
      ++iter;
    }

    if (cnt > 0) {
      // Mass lock
      tsp00_Int8 remainingTime = 0;
      OMS_HResult hr = GetSession()->m_lcSink->LockObjMass(
        cnt, &m_consistentView, ppContainerId, (SAPDB_UInt8*)pOid, 
        pObjVers, remainingTime, errCnt, pDBError);

      for (int i=0; i<cnt; ++i){
        if (pDBError[i] != e_ok) {
          GetSession()->ThrowDBError(pDBError[i], msg, pOid[i], __MY_FILE__, __LINE__);
        }
        else {
          // Mark object as locked in the liboms
          ppObj[i]->MarkLocked();
        }
      }
      cnt = 0;
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_Context::ReleaseAllUnchanged()   // PTS 1128262
{
  OMS_OidHash::OidIter iter = m_oidDir.First();
  while (iter) {
    OmsObjectContainer *pObj = iter();

    ++iter;
    GetSession()->ReleaseObj(pObj, /*unchangedOnly=*/true);

  }
}

/*----------------------------------------------------------------------*/

void OMS_Context::ReadUnchangedObjectsStart()
{
  if (m_pOidDirReadOnly == NULL){
    m_pOidDirReadOnly = (OMS_OidHashReadOnlyType*) this->allocate(sizeof(OMS_OidHashReadOnlyType));
    new(m_pOidDirReadOnly) OMS_OidHashReadOnlyType(m_heap);
  }
  m_isReadOnlyContext = true;
}
/*----------------------------------------------------------------------*/

void OMS_Context::ReadUnchangedObjectsEnd()
{
  m_isReadOnlyContext = false;
}

/*----------------------------------------------------------------------*/

void OMS_Context::RemoveCorruptVarObjects(
  bool             checkOnly,
  const ClassID   &guid,
  OmsSchemaHandle  schema,
  OmsContainerNo   containerNo)
{
  DbpBase b(GetSession()->m_lcSink);
  b.dbpOpError("Started: Removal of corrupt var-objects with parameters: checkOnly=%s guid=%d schema=%d containerNo=%d", 
    (checkOnly?"T":"F"), guid, schema, containerNo);


  if (IsVersion()){
    b.dbpOpError("Stopped, as a version is active");
    return;
  }

  OMS_ContainerEntry *pContainerInfo = GetContainerDir().GetContainerEntry(guid, schema, containerNo);

  if (pContainerInfo->GetFileId().fileObjFileType_gg00() != oftVarLenObjFile_egg00){
    // Container does not belong to a var-object
    b.dbpOpError("Stopped, as specified container does not belong to var-objects");
    return;
  }

  // Data-structure to store the oids of all sub-objects, which belong to non-corrupt objects
  cgg250AvlTree<OMS_ObjectId8,OMS_ObjectId8,OMS_Context> oidTree;
  oidTree.AdviseAllocator(this);
  OMS_ObjectId8 cmp;
  oidTree.AdviseCompare(&cmp);

  // Data-structure to store the oids of all continuation sub-objects, which belong to the object under consideration
  OMS_DynamicStack<OMS_ObjectId8,OMS_Context> currentOids;
  currentOids.Advise(this, /*entriesPerChunk=*/20);

  //************** First pass of reading the objects (read starting from header objects only) ********************

  // All sub-objects, which are found in this phase, belong to a valid object and therefore these 
  // sub-objects are inserted into a search-structure for later checks in the second phase.

  // Reserve space to store dereferenced sub-objects
  OMS_VarObjChunk chunk; 

  const int maxNoOfOid = 200;

  int           noOfOid = maxNoOfOid;
  OMS_ObjectId8   lastOid;
  OmsTypeUInt8  pOid_u[maxNoOfOid];  // Prevent running the constructor
  OMS_ObjectId8*pOid = reinterpret_cast<OMS_ObjectId8*>(&pOid_u[0]);
  tgg91_PageRef pObjVers[maxNoOfOid];
  tsp00_Int4    objHistReadCount;

  bool allOidsReadFromKernel = false;
  while (!allOidsReadFromKernel){
    // Read next oids from the iterator
    short error = 0;
    GetSession()->m_lcSink->NextOids (
      m_consistentView,
      pContainerInfo->GetFileId(),
      NULL,   // VersionContext
      true,   // bOnlyHdrVarObjects
      (SAPDB_UInt8&)lastOid, 
      noOfOid,
      (SAPDB_UInt8*)pOid, 
      pObjVers,
      objHistReadCount,
      &error);
    if (error == e_no_next_object){
      allOidsReadFromKernel = true;
    }
    else if (error){
      OMS_Globals::m_globalsInstance->Throw(error, NULL, __CONTEXT__, GetSession());
    }

    // Try to dereference Var-Objects (Object are not included into the oms-cache!)
    for (int i=0; i<noOfOid; ++i){
      size_t  subObjLen = 0;
      bool corrupt = false;

      chunk.Clear();
      chunk.m_seq.gg91SetNilRef();  

      currentOids.Clear();

      // Read header sub-object
      short error = 0; 
      GetSession()->m_lcSink->GetObj(m_consistentView,
        pContainerInfo->GetFileId(),
        NULL,  // VersionContext
        *REINTERPRET_CAST(SAPDB_UInt8*, &pOid[i]),
        /*doLock=*/false, 
        /*shared=*/false,
        chunk.m_seq,
        chunk.m_updTransId,
        sizeof(chunk.m_container), 
        ((unsigned char*) &chunk.m_container), 
        subObjLen,
        objHistReadCount,
        error);
      if (OMS_Globals::MapErrorCodeToExceptionClass(error) == OMS_EXCEPTION_OBJECT_NOT_FOUND){
        b.dbpOpError("Var-object header %d.%d(%d): Deref of header failed with error-code=%d", 
          pOid[i].getPno(), pOid[i].getPagePos(), pOid[i].getGeneration(), error);
        corrupt = true;
      }
      else if (error){
        OMS_Globals::m_globalsInstance->Throw(error, NULL, pOid[i], __CONTEXT__, GetSession());
      }

      // Read continuation sub-objects
      if (!corrupt && subObjLen == sizeof(OMS_VarObjContainer)) {
        size_t storedObjLen  = chunk.m_container.m_vobjSize;
        size_t currentObjLen = OMS_Globals::Min(subObjLen,VAR_OBJ_CHUNK_SIZE);
        while (!corrupt && subObjLen == sizeof(OMS_VarObjContainer) && currentObjLen < storedObjLen){
          OMS_ObjectId8 nextOid = chunk.m_container.m_vobjNext;

          // Check if sub-object belong already to another object
          if (oidTree.Find(nextOid) != NULL){
            b.dbpOpError("Var-object header %d.%d(%d): ", 
              pOid[i].getPno(), pOid[i].getPagePos(), pOid[i].getGeneration());
            b.dbpOpError(" - found sub-object:  %d.%d(%d) is already registered as sub-object of another object", 
              nextOid.getPno(), nextOid.getPagePos(), nextOid.getGeneration());
            corrupt = true;
            break;
          }

          chunk.Clear();
          chunk.m_seq.gg91SetNilRef();            

          short error = 0;
          GetSession()->m_lcSink->GetObj(m_consistentView,
            pContainerInfo->GetFileId(),
            NULL,  // VersionContext
            *REINTERPRET_CAST(SAPDB_UInt8*, &nextOid),
            /*doLock=*/false, 
            /*shared=*/false,
            chunk.m_seq,
            chunk.m_updTransId,
            sizeof(chunk.m_container), 
            ((unsigned char*) &chunk.m_container), 
            subObjLen,
            objHistReadCount,
            error);
          if (OMS_Globals::MapErrorCodeToExceptionClass(error) == OMS_EXCEPTION_OBJECT_NOT_FOUND){
            b.dbpOpError("Var-object header %d.%d(%d): Deref of continuation-object %d.%d(%d) failed with error-code=%d", 
              pOid[i].getPno(), pOid[i].getPagePos(), pOid[i].getGeneration(),
              nextOid.getPno(), nextOid.getPagePos(), nextOid.getGeneration(), error);
            while (!currentOids.IsEmpty()){
              const OMS_ObjectId8 *p = currentOids.Pop();
              b.dbpOpError(" - found sub-object:  %d.%d(%d)", p->getPno(), p->getPagePos(), p->getGeneration());
            }
            corrupt = true;
          }
          else if (error){
            OMS_Globals::m_globalsInstance->Throw(error, NULL, nextOid, __CONTEXT__, GetSession());
          }
          else {
            currentOids.Push(nextOid);
          }

          currentObjLen += OMS_Globals::Min(subObjLen,VAR_OBJ_CHUNK_SIZE);
        }

        if (storedObjLen != currentObjLen){
          b.dbpOpError("Var-object header %d.%d(%d): Stored (%d) and found object size (%d) are different: ", 
            pOid[i].getPno(), pOid[i].getPagePos(), pOid[i].getGeneration(),
            storedObjLen, currentObjLen);
          while (!currentOids.IsEmpty()){
            const OMS_ObjectId8 *p = currentOids.Pop();
            b.dbpOpError(" - found sub-object:  %d.%d(%d)", p->getPno(), p->getPagePos(), p->getGeneration());
          }
          corrupt = true;
        }
      }

      // Insert all oids, which belong to a non-corrupt object into the tree.
      if (!corrupt){
        oidTree.Insert(pOid[i]);
        while (!currentOids.IsEmpty()){
          oidTree.Insert(*currentOids.Pop());
        }
      }
    }

    lastOid = pOid[noOfOid-1];
  }


  //************** Second pass of reading objects (read all objects (header and continuation)) ********************

  // Now it is checked whether each sub-object, which is found in the container, belong to a valid object. 

  allOidsReadFromKernel = false;
  lastOid = OMS_ObjectId8();
  noOfOid = maxNoOfOid;
  while (!allOidsReadFromKernel){
    // Read next oids from the iterator
    short error = 0;
    GetSession()->m_lcSink->NextOids (
      m_consistentView,
      pContainerInfo->GetFileId(),
      NULL,  // VersionContext
      false,   // bOnlyHdrVarObjects
      (SAPDB_UInt8&)lastOid, 
      noOfOid,
      (SAPDB_UInt8*)pOid, 
      pObjVers,
      objHistReadCount,
      &error);
    if (error == e_no_next_object){
      allOidsReadFromKernel = true;
    }
    else if (error){
      OMS_Globals::m_globalsInstance->Throw(error, NULL, __CONTEXT__, GetSession());
    }

    for (int i=0; i<noOfOid; ++i){
      if (oidTree.Find(pOid[i]) == NULL){
        // sub-object does not belong to a regular object
        b.dbpOpError("sub-object %d.%d(%d) does not belong to a regular object.", pOid[i].getPno(), pOid[i].getPagePos(), pOid[i].getGeneration());

        if (!checkOnly){
          // Delete sub-object
          //GetSession()->DeleteObj(pOid[i], NULL);
          GetSession()->IncDelete ();
          OmsObjectContainer *found = GetObj(pOid[i], /*doLock=*/true, /*shared=*/false);
          if (found) {
            GetSession()->InsertBeforeImage (found);
            found->MarkDeleted();
          }
        }
      }
      //else {
      //  b.dbpOpError("sub-object %d.%d(%d) is ok.", pOid[i].getPno(), pOid[i].getPagePos(), pOid[i].getGeneration());
      //}
    }

    lastOid = pOid[noOfOid-1];
  }

  b.dbpOpError("Finished: Removal of corrupt var-objects with parameters: checkOnly=%s guid=%d schema=%d containerNo=%d", 
    (checkOnly?"T":"F"), guid, schema, containerNo);
}

