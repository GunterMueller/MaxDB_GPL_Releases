/*!
 * \file    OMS_Context.hpp
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


#ifndef __OMS_CONTEXT_HPP
#define __OMS_CONTEXT_HPP

#include "Oms/OMS_ContextDef.hpp"

#include "Oms/OMS_ContainerDirectory.hpp"
#include "Oms/OMS_ContainerEntry.hpp"
#include "Oms/OMS_ClassDirectory.hpp"
#include "Oms/OMS_ClassEntry.hpp"
#include "Oms/OMS_SessionDef.hpp"
#include "Oms/OMS_VarObjInfo.hpp"
#include "Oms/OMS_UnknownContainerId.hpp"
#include "Oms/OMS_HResult.hpp"
#include "Oms/OMS_Exceptions.hpp"
#include "Oms/OMS_VarOid.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*----------------------------------------------------------------------*/

inline void OMS_Context::CheckDBError(tsp00_Int2 e, const char* msg, const OMS_ObjectId8& oid) 
{
  if (0 != e) {
    GetSession()->ThrowDBError (e, msg, oid, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::CheckCompatibility(ClassID guid, OmsObjectContainerPtr pObj) 
{
  OMS_ContainerEntry *pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
  OMS_ClassEntry     &classInfo     = pContainerInfo->GetClassEntry();

  if (  NULL == pContainerInfo
    || (  !omsIsCompatibleGuid(classInfo.GetGuid(), guid)
    && !classInfo.IsDerivedClassOf(guid))) {
      GetSession()->ThrowDBError (e_object_not_found, "OMS_Context::CheckCompatibility", pObj->m_oid, __MY_FILE__, __LINE__);
    }
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::CheckOid (const OMS_ObjectId8& oid, ClassID guid) 
{
  const char msg[] = "OMS_Context::CheckOid";

  if (oid.isTrue()) {
    OMS_ContainerEntry* pContainerInfo = NULL;
    OmsObjectContainer *pObj = FindObjInContext (&oid, /*ignoreGeneration=*/false, /*checkDropped=*/false, /*checkLocallyCreated=*/false);
    if (NULL != pObj) {
      // Object exits already in the liboms-cache
      if (pObj->DeletedFlag()) {
        // Object is marked as deleted
        return false;
      } 
      else if (IsReadOnlyContext() && pObj->IsNewObject()){
        // Object has been created in the current transaction; therefore the object is not visible in the 
        // read-unchanged-mode.
        return false;
      }
      pContainerInfo = pObj->GetContainerInfoNoCheck();
    }
    else {
      // Object does not exist in the liboms-cache. 

      if ((!IsVersion() || !IsUnloaded()) && (OMS_VERSION_OBJ_PAGE_NO == oid.getPno())){
        // Oid corresponds to an object which has been created in a version. As either no version is
        // active or version is not unloaded the object must have been found in the liboms-cache
        // if it exists.
        return false;
      }

      GetSession()->m_currVarObjChunk.Clear();
      GetSession()->m_currVarObjChunk.m_seq.gg91SetNilRef();  

      // Try to read object from the kernel.
      size_t                 objLen;
      int                    objHistReadCount;
      tgg00_BasisError       DBError;
      OMS_UnknownContainerId FileId;
      OMS_HResult hr = GetSession()->m_lcSink->GetObj (
        m_consistentView,
        FileId,
        VersionContext(),
        *REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)),
        false,  // lock   
        false,  // shared
        GetSession()->m_currVarObjChunk.m_seq,
        GetSession()->m_currVarObjChunk.m_updTransId,
        sizeof(GetSession()->m_currVarObjChunk.m_container), 
        ((unsigned char*) &GetSession()->m_currVarObjChunk.m_container), 
        objLen,
        objHistReadCount,
        DBError);

      if (0 == DBError) {
        // Object was found in the kernel
        GetSession()->IncLogHop(objHistReadCount);

        if (FileId.IsVarObjFile()) {
          // Specified oid corresponds to a var-object although a normal object is expected
          return false;
        }
        else {
          // Insert object into the OMS-cache
          OMS_ContainerHandle handle = FileId.GetContainerHandle();
          pContainerInfo = GetContainerDir().GetContainerEntry(handle);
          pObj = pContainerInfo->GetMemory();
          SAPDB_MemCopyNoCheck (REINTERPRET_CAST(unsigned char*, &pObj->m_pobj) + sizeof(void*), &GetSession()->m_currVarObjChunk.m_container,
            pContainerInfo->GetClassEntry().GetPersistentSize());
          pObj->m_oid    = oid;
          pObj->m_objseq = GetSession()->m_currVarObjChunk.m_seq;
          pObj->SetRevisionNumber(GetSession()->m_currVarObjChunk.m_updTransId);
          PutObjectIntoContext(pObj, handle);
        }
      }
      else if (OMS_Globals::MapErrorCodeToExceptionClass(DBError) == OMS_EXCEPTION_OBJECT_NOT_FOUND){
        return false;
      }
      else {
        OMS_Globals::m_globalsInstance->Throw(DBError, msg, oid, __MY_FILE__, __LINE__, GetSession());
        return false;
      }
    }

    // Check whether container is dropped
    if (pContainerInfo->IsDropped()){
      return false;
    }

    // Check whether given guid is compatible with the guid of the object.
    OMS_ClassEntry &classInfo = pContainerInfo->GetClassEntry();
    if (  !omsIsCompatibleGuid(classInfo.GetGuid(), guid)
        && !classInfo.IsDerivedClassOf(guid)) {
        return false;
    }
  }

  return true;
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::CheckOid (const OMS_ObjectId8& oid)
{
  const char msg[] = "OMS_Context::CheckOid (VAR)";

  if (oid.isTrue()) {
    OMS_ContainerEntry* pContainerInfo = NULL;
    OmsObjectContainerPtr pObj = FindObjInContext (&oid, /*ignoreGeneration=*/false, /*checkDropped=*/false, /*checkLocallyCreated=*/false);
    if (NULL != pObj) {
      // Object exists in the OMS-cache
      if (pObj->DeletedFlag()) {
        return false;
      } 
      else if (IsReadOnlyContext() && pObj->IsNewObject()){
        // Object has been created in the current transaction; therefore the object is not visible in the 
        // read-unchanged-mode.
        return false;
      }

      pContainerInfo = pObj->GetContainerInfoNoCheck();
    }
    else {
      // Object does not exist in the OMS-Cache

      if ((!IsVersion() || !IsUnloaded()) && (OMS_VERSION_OBJ_PAGE_NO == oid.getPno())){
        // Oid corresponds to an object which has been created in a version. As either no version is
        // active or version is not unloaded the object must have been found in the liboms-cache
        // if it exists.
        return false;
      }

      GetSession()->m_currVarObjChunk.Clear();
      GetSession()->m_currVarObjChunk.m_seq.gg91SetNilRef(); 

      size_t           objLen;
      int              objHistReadCount;
      tgg00_BasisError DBError;
      OMS_UnknownContainerId FileId;
      OMS_HResult hr = GetSession()->m_lcSink->GetObj (
        m_consistentView,
        FileId,
        VersionContext(),
        *REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)),
        false,  // doLock 
        false,  // shared
        GetSession()->m_currVarObjChunk.m_seq,
        GetSession()->m_currVarObjChunk.m_updTransId,
        sizeof(GetSession()->m_currVarObjChunk.m_container), 
        ((unsigned char*) &GetSession()->m_currVarObjChunk.m_container), 
        objLen,
        objHistReadCount,
        DBError);

      if (0 == DBError) {
        // Object was found in the kernel
        GetSession()->IncLogHop(objHistReadCount);

        if (!FileId.IsVarObjFile()) {
          // Specified oid corresponds to a normal object although a var-object is expected
          return false;
        }
        else {
          // Load object (inclusive all continuation objects) into the liboms cache
          OMS_ContainerHandle handle = FileId.GetContainerHandle();
          pContainerInfo = GetContainerDir().GetContainerEntry(handle);
          GetSession()->m_currVarObjChunk.m_containerHandle = handle;
          if (objLen <= VAR_OBJ_CHUNK_SIZE) {
            GetSession()->m_currVarObjChunk.m_container.m_vobjSize = objLen;
          }
          pObj = LoadVarObject (*REINTERPRET_CAST(const OMS_ObjectId8*, &oid), 
            VarObjNoLock, 
            GetSession()->m_currVarObjChunk.m_container.m_vobjSize, 
            GetSession()->m_currVarObjChunk, 
            NULL);
        }
      }
      else if (OMS_Globals::MapErrorCodeToExceptionClass(DBError) == OMS_EXCEPTION_OBJECT_NOT_FOUND) {
        return false;
      }
      else {
        OMS_Globals::m_globalsInstance->Throw(DBError, msg, oid, __MY_FILE__, __LINE__, GetSession());
        return false;
      }
    }

    // Check whether container is dropped
    if (pContainerInfo->IsDropped()){
      return false;
    }

    // Check whether the object is a var-object
    if (!pContainerInfo->GetClassEntry().IsVarObject()) {
      return false;
    }
  }

  return true;  
}

/*----------------------------------------------------------------------*/

inline void  OMS_Context::ClearObjCache(bool deleteObjects, bool adaptOidHash) 
{
  if (deleteObjects){
    m_oidDir.Clear(/*callFromDestructor=*/false);
  }
  else {
    m_oidDir.SetEmpty(adaptOidHash);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::ClearReadOnlyObjCache(bool deleteObjects)
{
  if (m_pOidDirReadOnly != NULL){
    if (deleteObjects){
      // Deletes the objects
      OMS_OidHashReadOnlyType::Iterator iter = m_pOidDirReadOnly->Begin();
      while (iter.IsValid()){
        OmsObjectContainer *pObj = (*iter).value;
        ++iter;

        OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfoNoCheck();
        pContainerInfo->ChainFree(pObj, 31);
      }
    }

    // Deletes only the hash-structure
    m_pOidDirReadOnly->Delete();
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::ClearReadOnlyObjCache(
  bool deleteObjects,
  OMS_ContainerEntry *pContainerInfo)
{
  OMS_OidHashReadOnlyType *pNewOidDir = NULL;

  if (m_pOidDirReadOnly != NULL){
    if (deleteObjects){
      // Deletes the objects
      OMS_OidHashReadOnlyType::Iterator iter = m_pOidDirReadOnly->Begin();
      while (iter.IsValid()){
        OmsObjectContainer *pObj = (*iter).value;
        ++iter;

        OMS_ContainerEntry* pInfo = pObj->GetContainerInfoNoCheck();
        if (pInfo == pContainerInfo){
          pInfo->ChainFree(pObj, 32);
        }
        else {
          // Insert 'remaining'-objects into a new hash
          if (!pNewOidDir){
            // If hnew ash does not exist already, then create it
            pNewOidDir = (OMS_OidHashReadOnlyType*) allocate(sizeof(OMS_OidHashReadOnlyType));
            new(pNewOidDir) OMS_OidHashReadOnlyType(m_heap);
          }
          pNewOidDir->Insert(pObj->m_oid, pObj);
        }
      }
    }

    if (pNewOidDir){
      // If objects remain after the deletion, then these objects have been inserted into
      // a new structure. Therefore delete old one and change pointer to the new one.
      m_pOidDirReadOnly->Delete();
      deallocate(m_pOidDirReadOnly);

      m_pOidDirReadOnly = pNewOidDir;
    }
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::ClearVarObjData()
{
  OMS_OidHash::OidIter iter = m_oidDir.First();
  while (iter){
    OmsObjectContainer *p = iter();
    ++iter;
    if(p->VarObjFlag())
      reinterpret_cast<OMS_VarObjInfo*>(&p->m_pobj)->freeVarObj(this);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::CloseVersion()
{
  GetContainerDir().ClearFreeList(4);
  m_isOpen = false;
}

/*----------------------------------------------------------------------*/

inline tsp00_Int4 OMS_Context::CurrentVersionKey() const 
{
  return m_versionContext.ovc_key;
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::DecNewObjectsToFlush(OmsObjectContainer *p)
{
  --m_cntNewObjectsToFlush;

  // Change counter in the corresponding container entry
  p->GetContainerInfoNoCheck()->DecNewObjectsToFlush();
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::DeleteObjInContext (OmsObjectContainerPtr p, OMS_ContainerEntry* pContainerInfo)
{
  OMS_DETAIL_TRACE(p->IsNewObject() ? (omsTrContext | omsTrNewObj) : omsTrContext, 
    GetSession()->m_lcSink, "OMS_Context::DeleteObjInContext : " << p->m_oid
    << ", class: " << pContainerInfo->GetClassEntry().GetClassName());

  if (p->IsVerNewRegistered()){
    pContainerInfo->RemoveNewVersionObject(p);
  }

  const bool c_updateKeyCache = true;
  bool found = m_oidDir.HashDelete(p->m_oid, c_updateKeyCache);

  if (NULL != pContainerInfo && found) {
    pContainerInfo->ChainFree(p, 33);
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::ExistsConsistentView() 
{
  return (!(m_consistentView.gg90IsNil()));
}

/*----------------------------------------------------------------------*/

// PTS 1125361 added new parameter ignoreGeneration
inline OmsObjectContainerPtr OMS_Context::FindObjInContext (const OMS_ObjectId8* pOid, 
                                                            bool ignoreGeneration,
                                                            bool checkDropped,
                                                            bool checkLocallyCreated) 
{
  // If the 'read-unchanged-mode' is active, then always read exactly the object 
  // which is requested, even if the object-id has been reused by another object.
  if (ignoreGeneration && IsReadOnlyContext())
    ignoreGeneration = false;

  OmsObjectContainer *pObj = m_oidDir.HashFind(pOid, ignoreGeneration);

  if (IsReadOnlyContext()){
    if (pObj != NULL && pObj->IsNewObject()){
      if (checkLocallyCreated){
        // Object is created in the current active transaction. Therefore if we are in the mode where
        // we want to read without local changes, this object is not visible. 
        // As the object is already existing in the kernel (but with an invalid state) we must consider
        // this special case here.
        OMS_Globals::Throw(e_object_not_found, "object is locally created", *pOid, __MY_FILE__, __LINE__);
      }
      else {
        // Prevent throwing an exception. 'checkLocallyCreated' equals false when this method is called
        // during the key-deref of an object. If no object for a given key is found, then a NULL pointer
        // is returned and no exception is thrown. 
        return pObj;
      }
    }
    else if (pObj == NULL || pObj->GetBeforeImageMask() != 0 || pObj->Changed()){
      // Either object does not exists in the 'normal' hash or it has been changed locally.
      // => Check whether there is a copy of the object in the read-only hash
      OMS_OidHashReadOnlyType::Iterator iter = m_pOidDirReadOnly->Find(*pOid);
      if (iter.IsValid()){
        pObj = (*iter).value;
      }
      else {
        pObj = NULL;
      }
    }
  }

  if (pObj != NULL && checkDropped){
    // As objects are not deleted out of the cache if the corresponding container is dropped, 
    // the existence of the container must be checked for each deref.
    pObj->GetContainerInfoNoCheck()->CheckIfNotDropped(__MY_FILE__, __LINE__);
  }

  return pObj;
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_Context::FindVarObjInContext (const OMS_ObjectId8& oid, OMS_VarObjLockType lockReq, size_t size, void* buf) 
{
  const char* msg = "OMS_Context::FindVarObjInContext";

  OmsObjectContainerPtr po = FindObjInContext (&oid);
  if (NULL != po) {
    if (po->DeletedFlag()) {
      GetSession()->ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }
    OMS_VarObjInfo* pObjInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &po->m_pobj);
    pObjInfo->lock(lockReq);
    if (pObjInfo->m_vobjSize > size && size != 0) {
      GetSession()->ThrowDBError (e_too_small_packet_size, msg, oid, __MY_FILE__, __LINE__);
    }
    if (NULL != buf && pObjInfo->m_vobjSize != 0) {
      SAPDB_MemCopyNoCheck (buf, pObjInfo->m_pvobj, pObjInfo->m_vobjSize);
    }
    return po;
  }
  return NULL;
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::deallocate(void* p) 
{
#ifdef USE_SYSTEM_ALLOC_CO13
  delete[] (char*) p;
#else
  m_heap.Deallocate(p);
#endif
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::FreeStackHeap()
{
#ifndef USE_SYSTEM_ALLOC_CO13
  if(m_useStackHeap)
    m_stackHeap.Free();
  else{
    void **p = NULL;
    while((p = (void**)m_memBlockStack.Pop()) != NULL)
      deallocate(*p);
  }
#endif
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::FreeVarObj(unsigned char* p, size_t sz) 
{
  this->deallocate (p);
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerDirectory& OMS_Context::GetContainerDir() 
{
  return m_containerDir;
}

/*----------------------------------------------------------------------*/

inline tsp00_Int4 OMS_Context::GetHashSize() const
{
  return m_oidDir.GetHashSize();
}

/*----------------------------------------------------------------------*/

inline OMS_Context* OMS_Context::GetNext() const
{
  return m_next;
}

/*----------------------------------------------------------------------*/

inline OMS_Context* OMS_Context::GetNextUnloaded() const
{
  return m_nextUnloaded;
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_Context::GetObj(const OMS_ObjectId8& oid, bool doLock, bool shared) 
{
  GetSession()->IncDeref();
  OmsObjectContainerPtr pObj = FindObjInContext(&oid);
  if (NULL != pObj) {
    if (pObj->DeletedFlag()) {
      return NULL; // PTS 1107036
    }
    if ((doLock) && (!pObj->LockedFlag()) && (!this->IsVersion())) {
      if (!shared){
        this->LockObj(pObj->m_oid);
      }
      else {
        this->LockSharedObj(pObj->m_oid);
      }
    }
  }
  else {
    pObj = this->GetObjFromLiveCacheBase(oid, 
                                         doLock ? (shared ? OMS_Context::LockShared
                                                          : OMS_Context::Lock)
                                                : OMS_Context::NoLock);
  }
  return pObj;
}


/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_Context::GetObj(ClassID guid, 
                                                 const OMS_ObjectId8& oid, 
                                                 bool doLock,
                                                 bool shared) 
{
  GetSession()->IncDeref();
  OmsObjectContainerPtr pObj = FindObjInContext(&oid);
  if (NULL != pObj) {
    if (pObj->DeletedFlag()) {
      return NULL; // PTS 1107036
    }
    if (doLock && !IsVersion()) {
      if (!shared && !pObj->LockedFlag()){
        GetSession()->LockObj(pObj);
      }
      else if (shared && ! pObj->LockedSharedFlag()){
        GetSession()->LockSharedObj(pObj);
      }
    }
  }
  else {
    pObj = GetObjFromLiveCacheBase(guid, oid, 
                                   doLock ? (shared ? OMS_Context::LockShared
                                                    : OMS_Context::Lock)
                                          : OMS_Context::NoLock);
  }
  
  if (pObj != NULL){
    CheckCompatibility(guid, pObj);
  }
  
  return pObj;
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_Context::GetObjFromLiveCacheBase(const OMS_ObjectId8& oid,
                                                                  LockType           doLockObj,
                                                                  bool               mayBeVarObject)
                                                                      
{
  const char* msg = "OMS_Context::GetObjFromLiveCacheBase_1";
  size_t           objLen;
  int              objHistReadCount;
  tgg00_BasisError DBError;
  if (this->IsVersion()) {
    doLockObj = OMS_Context::NoLock;
  }
  OMS_UnknownContainerId FileId;
  GetSession()->m_currVarObjChunk.Clear();
  GetSession()->m_currVarObjChunk.m_seq.gg91SetNilRef();  // PTS 1121449

  bool shared = (doLockObj == OMS_Context::LockShared || doLockObj == OMS_Context::TryLockShared);

  OMS_HResult hr = GetSession()->m_lcSink->GetObj (
    m_consistentView,
    FileId,
    VersionContext(),
    *REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)),
    doLockObj != OMS_Context::NoLock, 
    shared,
    GetSession()->m_currVarObjChunk.m_seq,
    GetSession()->m_currVarObjChunk.m_updTransId,
    sizeof(GetSession()->m_currVarObjChunk.m_container), 
    ((unsigned char*) &GetSession()->m_currVarObjChunk.m_container), 
    objLen,
    objHistReadCount,
    DBError);
  if (0 == DBError) {
    OmsObjectContainerPtr pObj;
    if (FileId.IsVarObjFile()) {
      if (!mayBeVarObject)
      {
          return NULL;
      }
      GetSession()->m_currVarObjChunk.m_containerHandle = FileId.GetContainerHandle();
      if (objLen <= VAR_OBJ_CHUNK_SIZE) {
        GetSession()->m_currVarObjChunk.m_container.m_vobjSize = objLen;
      }
      pObj = this->LoadVarObject (oid, 
                                  VarObjNoLock, 
                                  GetSession()->m_currVarObjChunk.m_container.m_vobjSize, 
                                  GetSession()->m_currVarObjChunk, 
                                  NULL);
    }
    else {
      OMS_ContainerHandle handle = FileId.GetContainerHandle();
      OMS_ContainerEntry* pContainerInfo = GetContainerDir().GetContainerEntry(handle);
      pObj = pContainerInfo->GetMemory();
      SAPDB_MemCopyNoCheck (REINTERPRET_CAST(unsigned char*, &pObj->m_pobj) + sizeof(void*), &GetSession()->m_currVarObjChunk.m_container,
        pContainerInfo->GetClassEntry().GetPersistentSize());
      pObj->m_oid    = oid;
      pObj->m_objseq = GetSession()->m_currVarObjChunk.m_seq;
      pObj->SetRevisionNumber(GetSession()->m_currVarObjChunk.m_updTransId);
      PutObjectIntoContext(pObj, handle);
    }

    if (doLockObj == OMS_Context::Lock || doLockObj == OMS_Context::TryLock) {
      pObj->MarkLocked();
    }
    else if (doLockObj == OMS_Context::LockShared || doLockObj == OMS_Context::TryLockShared){
      pObj->MarkLockedShared();
    }

    return pObj;
  }
  else {
    if (e_object_not_found != DBError) {
      switch (doLockObj) {
      case OMS_Context::Lock:
        LockResult (DBError, NULL, oid, msg, /*shared=*/false);
        break;
      case OMS_Context::LockShared:             
        LockResult (DBError, NULL, oid, msg, /*shared=*/true);
        break;
      case OMS_Context::TryLock:
        if (!TryLockResult (DBError, /*shared=*/false, NULL, oid, msg)) {
          // retry without the lock
          return GetObjFromLiveCacheBase(oid, NoLock, mayBeVarObject);
        }
        break;
      case OMS_Context::TryLockShared:
        if (!TryLockResult (DBError, /*shared=*/true, NULL, oid, msg)) {
          // retry without the lock
          return GetObjFromLiveCacheBase(oid, NoLock, mayBeVarObject);
        }
        break;
      }
      this->CheckDBError(DBError, msg, oid);
    }
    return NULL;
  }
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_Context::GetObjFromLiveCacheBase(ClassID            guid, 
                                                                  const OMS_ObjectId8& oid,
                                                                  LockType           doLockObj,
                                                                  tgg91_PageRef*     pObjVers)
{
  size_t           dummyLen;
  int              objHistReadCount;
  tgg00_BasisError DBError;
  const char* msg = "OMS_Context::GetObjFromLiveCacheBase_2";
  
  GetSession()->IncDereflC();
  OMS_ClassEntry* pClassInfo = GetSession()->GetClassDir().GetClassEntry(guid);
  if (pClassInfo->IsBaseClass()) {
    // oid may identify an object of a derived class, i.e. size is unknown
    return this->GetObjFromLiveCacheBase(oid, doLockObj);
  }
  else {
    OmsObjectContainer *pObj = pClassInfo->GetMemory();

    if (pObjVers)   // PTS 1121449
      pObj->m_objseq = *pObjVers;
    else
      pObj->m_objseq.gg91SetNilRef();

    if (this->IsVersion()) {
      doLockObj = OMS_Context::NoLock;
    }

    bool shared = (doLockObj == OMS_Context::LockShared || doLockObj == OMS_Context::TryLockShared);

    OMS_UnknownContainerId FileId;
    tgg91_TransNo          updTransId;
    OMS_HResult hr = GetSession()->m_lcSink->GetObj (
      m_consistentView,
      FileId,
      VersionContext(),
      *REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)),
      doLockObj != OMS_Context::NoLock,  
      shared,
      pObj->m_objseq,
      updTransId,
      pClassInfo->GetPersistentSize(), // object size - header - vtblptr  
      ((unsigned char*) &pObj->m_pobj) + sizeof(void*), 
      dummyLen,
      objHistReadCount,
      DBError);
    if (0 != DBError) {
      pClassInfo->ChainFree(pObj, 34); 
      if (e_object_not_found != DBError) {
        switch (doLockObj) {
        case OMS_Context::Lock:
          LockResult(DBError, NULL, oid, msg, /*shared=*/false);
          break;
        case OMS_Context::LockShared:        
          LockResult(DBError, NULL, oid, msg, /*shared=*/true);
          break;
        case OMS_Context::TryLock:
          if (!TryLockResult(DBError, /*shared=*/false, NULL, oid, msg)) {
            // retry without the lock
            return GetObjFromLiveCacheBase(guid, oid, NoLock, pObjVers);
          }
          break;
        case OMS_Context::TryLockShared:
          if (!TryLockResult(DBError, /*shared=*/true, NULL, oid, msg)) {
            // retry without the lock
            return GetObjFromLiveCacheBase(guid, oid, NoLock, pObjVers);
          }
          break;
        }
        this->CheckDBError(DBError, msg, oid);
      }
      return NULL;
    }
    pObj->m_oid = oid;
    pObj->SetRevisionNumber(updTransId);

    if (doLockObj == OMS_Context::Lock || doLockObj == OMS_Context::TryLock) {
      pObj->MarkLocked();
    }
    else if (doLockObj == OMS_Context::LockShared || doLockObj == OMS_Context::TryLockShared) {
      pObj->MarkLockedShared();
    }

    //if (addIntoObjCache) {
    PutObjectIntoContext(pObj, FileId.GetContainerHandle());
    //};
    GetSession()->IncLogHop(objHistReadCount);
    return pObj;
  }
}

/*----------------------------------------------------------------------*/

inline int OMS_Context::LoadBaseClsObjsFromLiveCacheBase(int                 noOfOid,
                                                        const OMS_ObjectId8 *pOid,
                                                        tgg91_PageRef       *pObjVers,
                                                        tgg91_TransNo       *pUpdTransId,
                                                        bool                 doLock,
                                                        bool                 shared,
                                                        OmsObjectContainer **ppObjContainer,
                                                        tgg00_BasisError    *pDBError,
                                                        OmsAbstractObject  **ppObj,
                                                        OMS_ClassEntry     **ppClassInfo,
                                                        tgg01_ContainerId   *pContainerId,
                                                        size_t              *pObjSize)
{
  // oid may identifiy an object of a derived class, i.e. size is unknown.
  // Therefore first get container (and thereby the size) from the kernel and
  // then deref objects with known sizes.

  // Get container id from kernel 
  OMS_HResult hr = GetSession()->m_lcSink->GetContainerId (
      pContainerId,
      VersionContext(),
      noOfOid,
      REINTERPRET_CAST(const SAPDB_UInt8*, pOid),
      pDBError);

  // Determine correct class-info for oids
  for (int i=0; i<noOfOid; ++i){
    // replace fileObjFileNo_gg00 by fileDirFileId_gg00  FF 2003-09-23 new FileDir
    if (i>0 && pContainerId[i].fileDirFileId_gg00() == pContainerId[i-1].fileDirFileId_gg00()){
      ppClassInfo[i] = ppClassInfo[i-1];
    }
    else {
      // replace fileObjFileNo_gg00 by fileDirFileId_gg00  FF 2003-09-23 new FileDir
      OMS_ContainerEntry* pContainerEntry = GetContainerDir().GetContainerEntry(*reinterpret_cast<OMS_ContainerHandle*>(&pContainerId[i].fileDirFileId_gg00()));
      ppClassInfo[i] = const_cast<OMS_ClassEntry*>(&pContainerEntry->GetClassEntry());
    }
  }

  // Mass-Deref with known sizes
  int errorCnt 
    = LoadObjsFromLiveCacheBase(noOfOid,
                                pOid,
                                pObjVers,
                                pUpdTransId,
                                doLock,
                                shared,
                                ppObjContainer,
                                pDBError,
                                ppObj,
                                ppClassInfo,
                                pContainerId,
                                pObjSize);

  return errorCnt;
}

/*----------------------------------------------------------------------*/

inline int OMS_Context::LoadObjsFromLiveCacheBase(int                  noOfOid,
                                                  const OMS_ObjectId8 *pOid,
                                                  tgg91_PageRef       *pObjVers,
                                                  tgg91_TransNo       *pUpdTransId,
                                                  bool                 doLock,
                                                  bool                 shared,
                                                  OmsObjectContainer **ppObjContainer,
                                                  tgg00_BasisError    *pDBError,
                                                  OmsAbstractObject  **ppObj,
                                                  OMS_ClassEntry     **ppClassInfo,
                                                  tgg01_ContainerId   *pContainerId,
                                                  size_t              *pObjSize)

{
  const char* msg = "OMS_Context::LoadObjsFromLiveCacheBase ";

  int            i;
  int            objHistReadCount = 0;
  int            DBErrorCnt       = 0;
  
  GetSession()->IncDereflC();

  for(i=0; i<noOfOid; ++i){
    // Determine size of the object
    pObjSize[i] = ppClassInfo[i]->GetTransientSize();   

    // Get object frame to store the objects
    ppObjContainer[i] = ppClassInfo[i]->GetMemory();

    // Determine position where the kernel should write the object to. As the vtbl-pointer
    // is stored at the beginning of the abstract object the position must be incremented
    // by the size of a pointer.)
    ppObj[i] = (OmsAbstractObject*) ((unsigned char*) (&ppObjContainer[i]->m_pobj) + sizeof(void*));
  }

  // Read objects from the kernel
  OMS_HResult hr = GetSession()->m_lcSink->GetObjects (
      m_consistentView,
      pContainerId,
      VersionContext(),
      noOfOid,
      REINTERPRET_CAST(const SAPDB_UInt8*, pOid),
      pObjVers,
      pUpdTransId,
      pObjSize, 
      IsVersion() ? false : doLock,
      shared, 
      REINTERPRET_CAST(unsigned char**, ppObj),
      objHistReadCount,
      DBErrorCnt,
      pDBError);

  // Error handling
  if (DBErrorCnt > 0) {
    for (i=0; i<noOfOid; ++i){
      if (pDBError[i] != 0){
        // Release allocated memory
        ppClassInfo[i]->ChainFree(ppObjContainer[i], 35); 
        ppObjContainer[i] = NULL;

        if (pDBError[i] == e_object_not_found){
        }
        else if (pDBError[i] == e_object_dirty) {
          GetSession()->IncOutOfDate();
        }
        else {
          // Release allocated memory for all remaining objects
          for(int j=i ; j<noOfOid; ++j){
            ppClassInfo[j]->ChainFree(ppObjContainer[j], 208);   
            ppObjContainer[j] = NULL;
            if (pDBError[j] == 0){
              pDBError[j] = -1;
            }
          }

          GetSession()->ThrowDBError (pDBError[i], msg, pOid[i], __MY_FILE__, __LINE__);
          return DBErrorCnt;
        }
      }
    }
  }

  for (i=0; i<noOfOid; ++i){
    if (ppObjContainer[i] != NULL){
      ppObjContainer[i]->m_oid    = pOid[i];
      ppObjContainer[i]->m_objseq = pObjVers[i];
      ppObjContainer[i]->SetRevisionNumber(pUpdTransId[i]);

      if (doLock && !IsVersion()) {
        // Locally mark object as locked if lock is requested
        if (!shared){
          ppObjContainer[i]->MarkLocked();
        }
        else {
          ppObjContainer[i]->MarkLockedShared();
        }
      }

      // Insert Object into the oid-hash
      // replace fileObjFileNo_gg00 by fileDirFileId_gg00  FF 2003-09-23 new FileDir
      PutObjectIntoContext(ppObjContainer[i], pContainerId[i].fileDirFileId_gg00());
    }
  }
  
  // Update statistics
  GetSession()->IncLogHop(objHistReadCount);

  return DBErrorCnt;
}


/*======================================================================*/
/// Reading object with the specified key. 
/*! If not being in a version, the object is requested from the livecache 
**  kernel. It it's found then the oid is known and with this oid it is
**  checked whether this object is already stored in the local context
**  cache. If it is not already stored, then it is copied.
**  If a version is active, then it is first checked whether the key is
**  registerd in the search structure m_index. If so a pointer to the 
**  location in memory can be found there. (This check is needed, as 
**  objects which are newly created in a version are not stored in the 
**  livecache kernel and therefore they are accessible only via this
**  structure. 
**  \param ContainerInfo Reference to the container information in the 
**    local container dictionary
**  \param key Key of the requested object
**  \param doLock Should the method aquire a lock on the object. If the 
**    current transaction already owns a lock and a lock is requested
**    again, then this request is ignored. 
**  \param shared If a lock should be aquired, then this parameter determines
**    whether a shared or an exclusive lock is requested.
**  \return A pointer to the object in the context cache is returned if 
**    an object for the specified key exist; otherwise NULL
**  \see GetObjViaCachedKey 
**  \since 7.4.3  PTS 1117571 
*/
/*======================================================================*/
inline OmsObjectContainerPtr OMS_Context::GetObjViaKey(OMS_ContainerEntry &ContainerInfo, 
                                                       unsigned char      *key, 
                                                       bool                doLock,
                                                       bool                shared) 
{
  const char* msg = "OMS_Context::GetObjViaKey";

  // Don't lock the object if context is version. PTS 1131943
  if(IsVersion())                 
    doLock = false;

  GetSession()->IncDerefKey();

  // Get memory into which the kernel can copy the object
  OmsObjectContainer* obj = ContainerInfo.GetMemory();

  // transform key into binary representation
  unsigned char* pBinaryKey = ContainerInfo.GetClassEntry().GetKeyPtr(obj);
  // introduce pAbstractObject, without this code call will not be virtual on some platforms
  OmsAbstractObject* pAbstractObject = &obj->m_pobj;
  pAbstractObject->omsKeyToBinary(key, pBinaryKey);

  if (IsVersion()) {
    // If object has been created in a version, then it is stored in the search tree for all
    // version objects. As access to this structure is much faster than access to the kernel
    // first check this structure. (see PTS 1121876)
    OmsObjectContainer* found = ContainerInfo.VersionFindKey(pBinaryKey);
    if (found) {
      if (GetSession()->IsReadUnchangedObjectsActive()){
        if (IsVersionOid(found->m_oid) || found->Changed()){
          // If 'IsReadUnchangedObjectsActive' then the objects should be read in the state, 
          // they were when the consistent view was started. If the object was created in the
          // version, this object is not visible, but it is possible, that this object has
          // replaced an object (with the same key) which was created outside the version.
          // Therefore set found to zero and try to read object from the kernel.
          found = NULL;
        }
        else {
          // Non-version object which hasn't been changed yet
          ContainerInfo.ChainFree(obj, 36);
          return found;
        }
      }
      else {
        GetSession()->IncCacheHit(found->m_oid);

        return found->DeletedFlag() 
                ? NULL
                : found;
      }
    }
  }

  // Request object from the kernel
  int                 ObjHistReadCount;
  tgg00_BasisError    DBError;
  tgg91_TransNo       updTransId;
  OMS_HResult hr = GetSession()->m_lcSink->GetObjWithKey (
    (unsigned char*) &m_consistentView,
    (unsigned char*) &ContainerInfo.GetFileId(),
    (unsigned char*) VersionContext(),
    ContainerInfo.GetClassEntry().GetKeyDesc().GetLen(),   // PTS 1122540
    pBinaryKey, 
    doLock, 
    shared,
    ContainerInfo.GetClassEntry().GetPersistentSize(),
    ((unsigned char*) &obj->m_pobj) + sizeof(void*),
    REINTERPRET_CAST(SAPDB_UInt8*, &obj->m_oid),
    (unsigned char*) &obj->m_objseq,
    &updTransId,
    &ObjHistReadCount,
    &DBError);
  GetSession()->IncLogHop(ObjHistReadCount);  

  if (DBError) {
    OMS_ObjectId8 oid(obj->m_oid);

    // Release memory
    ContainerInfo.ChainFree(obj, 37);

    if (e_hash_key_not_found != DBError) {
      // Error handling
      LockResult (DBError, NULL, oid, msg, shared);
      return NULL;
    }
    else {
      // There is no object with such a key 
      return NULL;
    }
  }
  else {
    if (doLock) {
      // Lock object if requested
      LockResult (DBError, obj, obj->m_oid, msg, shared);
    }

    // Check whether the object with the corresponding oid is already stored in the oms-cache
    OmsObjectContainer* found = FindObjInContext(&obj->m_oid, /*ignoreGeneration=*/true, /*checkDropped=*/true, /*checkLocallyCreated=*/false);  
    if (NULL == found) {
      // Object does not exist locally. Therefore insert newly loaded object into oms-cache
      obj->SetRevisionNumber(updTransId);
      PutObjectIntoContext (obj, ContainerInfo.GetContainerHandle());
      return obj;
    }
    else {
      if (GetSession()->IsReadUnchangedObjectsActive() && (found->IsNewObject() || IsVersionOid(found->m_oid))){
        // As object is locally created or was created in an unloaded version, this object is not
        // visible in the 'read-unchanged'-mode
        return NULL;
      }

      // Object exists already in the oms-cache
      found->m_objseq = obj->m_objseq;
      if (doLock) {
        // Mark object in the object container as locked 
        if (!shared){
          found->MarkLocked();
        }
        else {
          found->MarkLockedShared();
        }
      }

      // Release memory of object read from the kernel
      ContainerInfo.ChainFree(obj, 38);
      
      return found->DeletedFlag()
             ? NULL
             : found;
    }  
  }
}


/*======================================================================*/
/// Reading object with the specified key using cached keys. 
/*! First the search structure m_index is checked whether the object does 
**  already exist in the current context. 
**  If so no access to the kernel is needed. Otherwise the livecache kernel
**  is requested and if the object exists there it is copied into the context
**  cache. If an object is not found at all,
**  then this is also registered in a search structure (m_cacheMiss), so
**  that later requests for this object can be answered quickly, too.
**  \param ContainerInfo Reference to the container information in the local
**    container dictionary
**  \param key Key of the requested object
**  \param doLock should the method acquire a lock on the object. If the 
**    current transaction already owns a lock and a lock is requested
**    again, then this request is ignored. 
**  \param shared If a lock should be acquired, then this parameter determines
**    whether a shared or an exclusive lock is requested.
**  \return A pointer to the object in the context cache is returned if an  
**    object for the specified key exist; otherwise NULL
**  \see GetObjViaKey 
**  \since 7.4.3  PTS 1117571 
*/
/*======================================================================*/
inline OmsObjectContainerPtr 
OMS_Context::GetObjViaCachedKey(OMS_ContainerEntry  &ContainerInfo, 
                                unsigned char       *key, 
                                bool                 doLock,
                                bool                 shared) 
{   
    const char* msg = "OMS_Context::GetObjViaCachedKey";

    // Don't lock the object if context is version. PTS 1131943
    if(IsVersion())                 
      doLock = false;
    
    int                 ObjHistReadCount;    
    tgg00_BasisError    DBError;
    
    GetSession()->IncDerefKey();
    
    OmsObjectContainer* obj = ContainerInfo.GetMemory();
    
    // transform key into binary representation
    unsigned char* pBinaryKey = ContainerInfo.GetClassEntry().GetKeyPtr(obj);
    
    // introduce pAbstractObject, without this code call will not be virtual 
    // on some platforms
    OmsAbstractObject* pAbstractObject = &obj->m_pobj;
    pAbstractObject->omsKeyToBinary(key, pBinaryKey);
    
    // ********** Try to read from cache **********
    OmsObjectContainerPtr found = ContainerInfo.VersionFindKey(pBinaryKey);
    if (found) {
      if (GetSession()->IsReadUnchangedObjectsActive()){
        if (!IsVersionOid(found->m_oid) && !found->Changed()){
          ContainerInfo.ChainFree(obj, 39);
          return found;
        }
        else {
          found = NULL;
        }
      }
      else {
        GetSession()->IncCacheHit(found->m_oid);
 
        ContainerInfo.ChainFree(obj, 40);

        if (doLock ){  
          // Get lock if requested. As the object has been read from the kernel
          // without acquiring a lock, the kernel must be informed about the lock, too.
          if (!shared && !found->LockedFlag()){
            GetSession()->LockObj(found); 
            found->MarkLocked();
          }
          else if (shared && !found->LockedSharedFlag()){
            GetSession()->LockSharedObj(found); 
            found->MarkLockedShared();
          }
        }
        
        // If object is not marked as deleted, then return cached Entry.
        return found->DeletedFlag() ? NULL : found;
      }
    }
    
    // ********** Check whether key is known as a cache miss **********
    else if (!GetSession()->IsReadUnchangedObjectsActive() && ContainerInfo.IsCacheMiss(pBinaryKey)) {
        GetSession()->IncCacheMiss();		
        
        // Release memory
        ContainerInfo.ChainFree(obj, 41);
        
        return NULL;
    }

    // ********** Get object from kernel **********
    //else {  // Obj might be found in version tree, but as it has been changed, it must be 
              // dereferenced from the kernel if 'ReadOnlyVersion' is active.
        tgg91_TransNo  updTransId;
        OMS_HResult hr = GetSession()->m_lcSink->GetObjWithKey (
            (unsigned char*) &m_consistentView,
            (unsigned char*) &ContainerInfo.GetFileId(),
            (unsigned char*) VersionContext(),
            ContainerInfo.GetClassEntry().GetKeyDesc().GetLen(),  // PTS 1122540
            pBinaryKey, 
            doLock && !IsVersion(),  // lock only if not in version 
            shared,
            ContainerInfo.GetClassEntry().GetPersistentSize(),
            ((unsigned char*) &obj->m_pobj) + sizeof(void*),
            REINTERPRET_CAST(SAPDB_UInt8*, &obj->m_oid),
            (unsigned char*) &obj->m_objseq,
            &updTransId,
            &ObjHistReadCount,
            &DBError);
        
        GetSession()->IncLogHop(ObjHistReadCount);  
        
        if (DBError) {
            // Read from kernel was not successful 
                       
            if (DBError == e_hash_key_not_found) {
                // Object does not exist neither in the cache nor in the kernel
                // => Insert Object into the cache miss structure
                if (!GetSession()->IsReadUnchangedObjectsActive()){
                  ContainerInfo.InsertCacheMiss(pBinaryKey);   
                }
            }
            else {
                // ONLY Errorhandling as object==NULL
                LockResult (DBError, NULL, OMS_ObjectId8(), msg, shared);    
            }
            
            // Release Memory
            ContainerInfo.ChainFree(obj, 42);
            
            return NULL;
        }
        else {
            // Read from kernel was successful      
            found = FindObjInContext(&obj->m_oid, /*ignoreGeneration=*/true, /*checkDropped=*/true, /*checkLocallyCreated=*/false);  
            if (!found) {
                if (!GetSession()->IsReadUnchangedObjectsActive()) {
                  // Insert entry into cached key structure 
                  ContainerInfo.VersionAddKey(obj);  
                }
                
                obj->SetRevisionNumber(updTransId);
                PutObjectIntoContext (obj, ContainerInfo.GetContainerHandle()); 
                
                if (doLock) {
                    LockResult (DBError, obj, obj->m_oid, msg, shared);
                }
                
                return obj;
            }
            else {
              if (GetSession()->IsReadUnchangedObjectsActive() && (found->IsNewObject() || IsVersionOid(found->m_oid))){
                // As object is locally created or was created in an unloaded version, this object is not
                // visible in the 'read-unchanged'-mode
                ContainerInfo.ChainFree(obj, 43);
                return NULL;
              }
 
                // Object is already in the context without being registered in
                // the cache structure. This can either happen if object was not read
                // via the key or if the caching was switched on after objects have 
                // already been loaded into the context
                
                found->m_objseq = obj->m_objseq;
                ContainerInfo.ChainFree(obj, 44);
                
                if (doLock){
                  if (!shared && !found->LockedFlag()) {
                    found->MarkLocked();
                  }
                  else if (shared && !found->LockedSharedFlag()){
                    found->MarkLockedShared();
                  }
                }

                if (!GetSession()->IsReadUnchangedObjectsActive()) {
                  // Insert info for cached keys 
                  ContainerInfo.VersionAddKey(found);   
                }
                
                return found->DeletedFlag() ? NULL : found;
            }
        }
    //}	
}


/*----------------------------------------------------------------------*/
// PTS 1122194
inline int OMS_Context::LoadObjsViaKeyFromLCBase(int                   noOfObj,
                                                  size_t              *pKeyLen,
                                                  unsigned char      **ppBinaryKey,
                                                  OMS_ObjectId8         *pOid,
                                                  tgg91_PageRef       *pObjVers,
                                                  tgg91_TransNo       *pUpdTransId,
                                                  bool                 doLock,
                                                  bool                 shared,
                                                  OmsObjectContainer **ppObjContainer,
                                                  tgg00_BasisError    *pDBError,
                                                  OmsAbstractObject  **ppObj,
                                                  OMS_ContainerEntry **ppContainerInfo,
                                                  tgg01_ContainerId   *pContainerId,
                                                  size_t              *pObjSize)
{
  const char* msg = "OMS_Context::LoadObjsViaKeyFromLCBase ";

  tsp00_Int4 DBErrorCnt = 0;

  // Read objects from the kernel
  tsp00_Int4 objHistReadCount = 0;
  OMS_HResult hr = GetSession()->m_lcSink->GetObjectsWithKey (
      &m_consistentView,
      pContainerId,
      VersionContext(),
      pKeyLen,
      noOfObj,
      ppBinaryKey,
      IsVersion() ? false : doLock,
      shared,
      pObjSize, 
      REINTERPRET_CAST(unsigned char**, ppObj),
      REINTERPRET_CAST(SAPDB_UInt8*, pOid),
      pObjVers, 
      pUpdTransId,
      objHistReadCount,
      DBErrorCnt,
      pDBError);

  // Error handling
  if (DBErrorCnt > 0) {
    for (int i=0; i<noOfObj; ++i){
      if (pDBError[i] != 0){
        // Release allocated memory
        ppContainerInfo[i]->ChainFree(ppObjContainer[i], 45); 
        ppObjContainer[i] = NULL;

        if (pDBError[i] == e_hash_key_not_found){
        }
        else if (pDBError[i] == e_object_dirty) {
          GetSession()->IncOutOfDate();
        }
        else {
          //// Release allocated memory for all remaining objects
          //for(int j=i ; j<noOfObj; ++j){
          //  ppContainerInfo[j]->GetClassEntry().ChainFree(*this, ppObjContainer[j], 46); 
          //  ppObjContainer[j] = NULL;
          //  pDBError[j]       = -1;
          //}

          //GetSession()->ThrowDBError (pDBError[i], msg, pOid[i], __MY_FILE__, __LINE__);
          //return DBErrorCnt;
        }
      }
    }
  }

  for (int i=0; i<noOfObj; ++i){
    if (ppObjContainer[i] != NULL){
      OmsObjectContainer* found = FindObjInContext(&pOid[i], /*ignoreGeneration=*/true);
      if (found){
        // Object is already stored in the oms-cache
        ppContainerInfo[i]->ChainFree(ppObjContainer[i], 47);
        ppObjContainer[i] = found;

        ppObjContainer[i]->m_objseq = pObjVers[i];
      }
      else {
        // Object is not stored in the oms-cache
        ppObjContainer[i]->m_oid    = pOid[i];
        ppObjContainer[i]->m_objseq = pObjVers[i];
        ppObjContainer[i]->SetRevisionNumber(pUpdTransId[i]);

        // replace fileObjFileNo_gg00 by fileDirFileId_gg00  FF 2003-09-23 new FileDir
        PutObjectIntoContext(ppObjContainer[i], pContainerId[i].fileDirFileId_gg00());
      }

      if (doLock && !IsVersion()) {
        // Locally mark object as locked if lock is requested
        if (!shared){
          ppObjContainer[i]->MarkLocked();
        }
        else {
          ppObjContainer[i]->MarkLockedShared();
        }
      }
    }
  }
  
  // Update statistics
  GetSession()->IncLogHop(objHistReadCount);

  return DBErrorCnt;
}

/*----------------------------------------------------------------------*/

inline size_t OMS_Context::GetVarObjFromLiveCacheBase(const OMS_ObjectId8 &oid, 
                                                      bool               doLock, 
                                                      bool               shared)
{
  const char* msg = "OMS_Context::GetVarObjFromLiveCacheBase";

  size_t              objLen;
  int                 objHistReadCount;
  tgg00_BasisError    DBError;
  OMS_UnknownContainerId FileId;
  
  GetSession()->IncLoadVarObjLC();
  GetSession()->m_currVarObjChunk.m_seq.gg91SetNilRef();  // PTS 1121449

  OMS_HResult hr = GetSession()->m_lcSink->GetObj (
    m_consistentView,
    FileId,
    VersionContext(),
    *REINTERPRET_CAST(SAPDB_UInt8*, CONST_CAST(OMS_ObjectId8*, &oid)),
    doLock, 
    shared,
    GetSession()->m_currVarObjChunk.m_seq, 
    GetSession()->m_currVarObjChunk.m_updTransId,
    sizeof(GetSession()->m_currVarObjChunk.m_container),
    (unsigned char*) &GetSession()->m_currVarObjChunk.m_container,
    objLen, objHistReadCount, DBError);
  if ( DBError != 0 ) { 
    if (e_object_not_found != DBError) {
      if (doLock) {
        LockResult (DBError, NULL, oid, msg, shared);
      }
    }
    this->CheckDBError(DBError, msg, oid);
    return 0;
  }
  if (!FileId.IsVarObjFile()) {
    GetSession()->ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
  }
  GetSession()->m_currVarObjChunk.m_containerHandle = FileId.GetContainerHandle();

  // Check that container, belonging to returned container handle, is already 
  // registered, otherwise autoregister now
  OMS_ContainerEntry *pContainerInfo = GetContainerDir().GetContainerEntry(GetSession()->m_currVarObjChunk.m_containerHandle); 
  
  GetSession()->IncLogHop(objHistReadCount);
  GetSession()->m_currVarObjChunk.m_oid  = oid;
  if (objLen <= VAR_OBJ_CHUNK_SIZE) {
    GetSession()->m_currVarObjChunk.m_container.m_vobjSize = objLen;
  }
  return GetSession()->m_currVarObjChunk.m_container.m_vobjSize;
}

/*----------------------------------------------------------------------*/

inline tsp00_8ByteCounter OMS_Context::HeapUsed()
{
#ifdef USE_SYSTEM_ALLOC_CO13
  return 0;
#else
  SAPDB_ULong BytesUsed;
  SAPDB_ULong MaxBytesUsed;
  SAPDB_ULong BytesControlled;
  m_heap.CalcStatisticsConst(BytesUsed, MaxBytesUsed, BytesControlled); 
  return BytesUsed;
#endif
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::IncNewObjectsToFlush(OmsObjectContainer *p)
{
  ++m_cntNewObjectsToFlush;

  // Change counter in the corresponding container entry
  p->GetContainerInfoNoCheck()->IncNewObjectsToFlush();
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::ChangeNewObjectsToFlush(int cnt)
{
  m_cntNewObjectsToFlush += cnt;
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::IncVersionKey ()
{
  ++m_versionContext.ovc_key;
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::IsOpenVersion (const OmsVersionId& versionId) 
{
  return (0 == memcmp (versionId, m_version, sizeof(versionId)));
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::IsUnloadable() 
{
  return ((!m_isOpen) && (!m_boundToTrans));
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::IsVersion() 
{
  return m_isVersion;
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::IsVersionOid (const OMS_ObjectId8& oid) const 
{
  return (oid.getPno() == OMS_VERSION_OBJ_PAGE_NO);
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::IsVersionOpen() 
{
	return m_isOpen;
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_Context::LoadVarObject (const OMS_ObjectId8 &oid, 
                                                         OMS_VarObjLockType   lockReq, 
                                                         size_t               objSize, 
                                                         OMS_VarObjChunk     &chunk, 
                                                         void                *buf) 
{
  OMS_VarObjInfo         *pObjInfo;
  unsigned char          *p;
  tsp00_Int2              DBError;
  OMS_VarObjInfoNode     *pInfoNode;
  OMS_ObjectId8           nextOid;
  size_t                  objLen;
  int                     ObjHistReadCount;
  size_t                  moveLen;

  // ASSUMPTION:
  //   first chunk of var object is expected in parameter chunk 
  //   container handle of chunk is set correctly

  const char* msg = "OMS_Context::LoadVarObject";
  
  OMS_ContainerEntry* pContainerInfo  = GetContainerDir().GetContainerEntry(chunk.m_containerHandle); 
  OmsObjectContainer *po              = pContainerInfo->GetMemory();

  po->m_oid     = oid;
  po->m_objseq  = chunk.m_seq;
  po->SetRevisionNumber(chunk.m_updTransId);
  po->MarkVarObj();
  pObjInfo = new(&po->m_pobj) OMS_VarObjInfo(oid, objSize, chunk.m_seq); 
  pObjInfo->lock(lockReq);

  // Ensure alignment of object and add space for check-pattern
  size_t sz = ((objSize + 3) & ~3) + sizeof(tsp00_Uint4);
  pObjInfo->m_pvobj  = GetSession()->GetVarObjMemory(sz);
  
  // Write check-pattern into the last 4 bytes of the allocated memory.
  // This pattern will be checked during store and during the flush.
  reinterpret_cast<tsp00_Uint4*>(pObjInfo->m_pvobj+sz)[-1] = OMS_VAROBJ_CHECK_PATTERN;

  pObjInfo->m_vobjInfo.m_vobjOid  = oid;
  pObjInfo->m_vobjInfo.m_vobjSeq  = chunk.m_seq;
  pObjInfo->m_vobjInfo.m_nodeCnt  = 1;   // PTS 1121449
  pObjInfo->m_vobjInfo.m_vobjNext = NULL;
  pInfoNode = &pObjInfo->m_vobjInfo;
  PutObjectIntoContext(po, pContainerInfo->GetContainerHandle());
  
  moveLen  = OMS_Globals::Min(objSize, VAR_OBJ_CHUNK_SIZE);
  SAPDB_MemCopyNoCheck (pObjInfo->m_pvobj, &chunk.m_container, moveLen);

  objSize  -= moveLen;
  p  = pObjInfo->m_pvobj + moveLen;
  try {
    bool overflow = false;                            // PTS 1121449
    OMS_VarObjInfoNode *pFirstInfoNode = pInfoNode;   // PTS 1121449
    
    while (objSize > 0) {
      nextOid = chunk.m_container.m_vobjNext;
      chunk.m_seq.gg91SetNilRef();             // PTS 1121449
      
      // Read continuation object from kernel
      OMS_UnknownContainerId  FileId;
      tgg91_TransNo  dummy_updTransId;
      GetSession()->m_lcSink->GetObj (
        m_consistentView,
        FileId,
        VersionContext(),
        *REINTERPRET_CAST(SAPDB_UInt8*, &nextOid),
        false,  // doLock
        false,  // shared
        chunk.m_seq,
        dummy_updTransId,
        sizeof (chunk.m_container),
        (unsigned char*) &chunk.m_container, 
        objLen,
        ObjHistReadCount,
        DBError);
      if (DBError){
        // "§%$ Meldung anhängen
        char buf[256];
        sp77sprintf (buf, sizeof(buf), "Error=%d when dereferencing continuation object %d.%d(%d) (class=%d, schema=%d, container=%d)",
          DBError, nextOid.getPno(), nextOid.getPagePos(), nextOid.getGeneration(),
          pContainerInfo->GetGuid(), pContainerInfo->GetSchema(), pContainerInfo->GetContainerNo());
        DBError = e_internal_obj_link_corruption;
        OMS_Globals::m_globalsInstance->Throw(DBError, buf, oid, __CONTEXT__, GetSession());
      }

      moveLen = OMS_Globals::Min(objSize, VAR_OBJ_CHUNK_SIZE);
      SAPDB_MemCopyNoCheck (p, &chunk.m_container, moveLen);

      p       += moveLen;
      objSize -= moveLen;
      pInfoNode->m_vobjNext = new(*this) OMS_VarObjInfoNode (nextOid, chunk.m_seq);
      
      // Remember how many pages or infonodes belong to current object (PTS 1121449)
      // As objects can be arbitrary large prevent overflow of 'unsigned short'-counter.
      if (!overflow && pFirstInfoNode->m_nodeCnt < (1<<(sizeof(unsigned short)<<3))-2){
        ++pFirstInfoNode->m_nodeCnt;   
      }
      else {
        pFirstInfoNode->m_nodeCnt = 0;
        overflow = true;
      }

      pInfoNode = pInfoNode->m_vobjNext;
    }
    if (buf != NULL) {
      SAPDB_MemCopyNoCheck (buf, pObjInfo->m_pvobj, pObjInfo->m_vobjSize);
    }
  } 
  catch (DbpError &e) {
    // remove varobj from context in case of error
    pObjInfo->unlock();
    DeleteObjInContext(po, pContainerInfo);
    throw e;
  }
  return po;
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::LockObj( const OMS_ObjectId8&oid ) 
{
  const char* msg = "OMS_Context::LockObj";
    
  GetSession()->AssertNotReadOnly(msg);
  if (this->IsVersion()) {
    return;
  }
  OmsObjectContainerPtr pObj = this->FindObjInContext(&oid);
  if (NULL != pObj) {
    if (pObj->DeletedFlag()) {
      GetSession()->ThrowDBError(e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }
    if (pObj->LockedFlag()) {
      // Object already locked
      return;
    }
    GetSession()->LockObj(pObj);
  }
  else {
    pObj = this->GetObjFromLiveCacheBase(oid, OMS_Context::Lock); 
    if (NULL == pObj) {
      GetSession()->ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::LockSharedObj( const OMS_ObjectId8&oid ) 
{
  const char* msg = "OMS_Context::LockSharedObj";
    
  //GetSession()->AssertNotReadOnly(msg);
  if (this->IsVersion()) {
    return;
  }
  OmsObjectContainerPtr pObj = this->FindObjInContext(&oid);
  if (NULL != pObj) {
    if (pObj->DeletedFlag()) {
      GetSession()->ThrowDBError(e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }
    if (pObj->LockedSharedFlag()) {
      // Object already locked (either shared or exclusive)
      return;
    }
    GetSession()->LockSharedObj(pObj);
  }
  else {
    pObj = this->GetObjFromLiveCacheBase(oid, OMS_Context::LockShared); 
    if (NULL == pObj) {
      GetSession()->ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::TryLockObj( 
  const OMS_ObjectId8 &oid,
  bool               shared) 
{
  const char* msg = "OMS_Context::TryLockObj";
    
  GetSession()->AssertNotReadOnly(msg);
  if (IsVersion()) {
    return true;
  }

  OmsObjectContainerPtr pObj = this->FindObjInContext(&oid);
  if (NULL != pObj) {
    if (pObj->DeletedFlag()) {
      GetSession()->ThrowDBError(e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }

    if (!shared && pObj->LockedFlag() || shared && pObj->LockedSharedFlag()) {
      // Object already locked
      return true;
    }

    return GetSession()->TryLockObj(pObj, shared);
  }
  else {
    pObj = this->GetObjFromLiveCacheBase(oid, shared ? OMS_Context::TryLockShared : OMS_Context::TryLock); 
    if (NULL == pObj) {
      GetSession()->ThrowDBError (e_object_not_found, msg, oid, __MY_FILE__, __LINE__);
    }

    return (!shared && pObj->LockedFlag() || shared && pObj->LockedSharedFlag());
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::LockResult (tsp00_Int2 DBError, OmsObjectContainerPtr p, const OMS_ObjectId8& oid, const char* msg, bool shared) 
{
  if (DBError != 0) {
    GetSession()->ThrowDBError (DBError, msg, oid, __MY_FILE__, __LINE__);
  }
  else {
    if (NULL != p) {
      if (!shared){
        p->MarkLocked();
      }
      else {
        p->MarkLockedShared();
      }
    }
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::LockResult (int cnt, tsp00_Int2* pDBError, OmsObjectContainer** p, const OMS_ObjectId8* pOid, const char* msg, bool shared) 
{
  int  err   = -1;
  for (int i=0; i<cnt; ++i){
    if (pDBError[i] == 0 && p && p[i]) {
      // Set local lock-flag
      if (!shared){
        p[i]->MarkLocked();
      }
      else {
        p[i]->MarkLockedShared();
      }
      GetSession()->IncLock();
    }
    else if (pDBError[i] != 0){
      // Check error code
      if (e_object_dirty == pDBError[i]) {
        GetSession()->IncOutOfDate();
        if (err == -1){
          err = i;
        }
      }
      else {
        // Errors, which are not out-of-date, have a higher priority
        err = i;
      }
    }
  }
  // If an error occurred throw exception 
  if (err != -1){
    GetSession()->ThrowDBError (pDBError[err], msg, pOid[err], __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_Context::TryLockResult (tsp00_Int2 DBError, bool shared, OmsObjectContainerPtr p, const OMS_ObjectId8& oid, const char* msg) 
{
  if (DBError != 0) {
    if (e_object_dirty == DBError) {
      GetSession()->IncOutOfDate();
      return false;
    }
    else if (  e_OMS_request_timeout == DBError
            || e_OMS_lock_collision  == DBError) {
      return false;
    }
    else {
      GetSession()->ThrowDBError (DBError, msg, oid, __MY_FILE__, __LINE__);
      return false;
    }
  }
  else {
    if (NULL != p) {
      if (!shared){
        p->MarkLocked();
      }
      else {
        p->MarkLockedShared();
      }
    }
    return true;
  }
}

/*----------------------------------------------------------------------*/

inline void* OMS_Context::allocate(size_t sz) 
{
    if (OMS_Globals::m_globalsInstance->IsOmsTestLib())
    {
        OMS_Globals::GetKernelInterface()->TestBadAlloc();
    }
#ifdef USE_SYSTEM_ALLOC_CO13
    return new char[sz];
#else
    return m_heap.Allocate((SAPDB_ULong) sz);
#endif
}

/*----------------------------------------------------------------------*/
//FF 2003-09-23 new FileDir
inline void OMS_Context::PutObjectIntoContext (OmsObjectContainerPtr p, OMS_ContainerHandle ContainerHandle) 
{
  const char* msg = "PutObjectIntoContext";
  
  OMS_ContainerEntry* pContainerInfo = GetContainerDir().GetContainerEntry(ContainerHandle);

  if (NULL != pContainerInfo) {
    p->SetContainerInfo(pContainerInfo);
    OMS_DETAIL_TRACE(p->IsNewObject() ? (omsTrContext | omsTrNewObj) : omsTrContext, 
      GetSession()->m_lcSink, "OMS_Context::PutObjectIntoContext : " << p->m_oid
      << ", class: " << pContainerInfo->GetClassEntry().GetClassName());

    if (!IsReadOnlyContext()){
      m_oidDir.HashInsert(p);
    }
    else {
      m_pOidDirReadOnly->Insert(p->m_oid, p);
    }

    if (p->IsVerNewObject()) {
      pContainerInfo->RegisterNewVersionObject(p);
      p->MarkVerNewRegistered();
    }
  }
  else {
    GetSession()->ThrowDBError(e_unknown_guid, msg, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::RenameVersion(const OmsVersionId& OldId, const OmsVersionId& NewId) 
{
  SAPDB_MemCopyNoCheck (m_version, NewId, sizeof(m_version));
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::ResetNewObjectsToFlush()
{
  m_cntNewObjectsToFlush = 0;
  GetContainerDir().ResetNewObjectsToFlush();
}
/*----------------------------------------------------------------------*/

inline void OMS_Context::ResetConsistentView () 
{
  m_consistentView.gg90SetNil();
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::SetConsistentView(const tgg91_TransNo& v) 
{
   m_consistentView = v;
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::SetNext(OMS_Context* pContext)
{
  m_next = pContext;
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::SetNextUnloaded(OMS_Context* pContext)
{
  m_nextUnloaded = pContext;
}

/*----------------------------------------------------------------------*/
// PTS 1117690
inline void OMS_Context::SetVersionDesc(const OmsTypeWyde* desc)
{
  if (m_versionDesc != NULL){
    deallocate(m_versionDesc);
    m_versionDesc = NULL;
  }

  if (desc == NULL){
    return;
  }

  // Get length of desciption
  int len = 0;
  while (desc[len] != 0 && len < OMS_MAX_VERSION_DESC_LENGTH){
    ++len;
  }

  // Allocate memory and copy description.
  // Every desciption longer than MAX_... will be truncated.
  // Memory will be freed in destructor of omsContext_co12.
  m_versionDesc = reinterpret_cast<OmsTypeWyde*>(allocate((len + 1) * sizeof(OmsTypeWyde)));
  SAPDB_MemCopyNoCheck(m_versionDesc, desc, len * sizeof(OmsTypeWyde));
  m_versionDesc[len] = 0;  // ensure that string in null-terminated
}

/*----------------------------------------------------------------------*/

inline void* OMS_Context::StackHeapMalloc(size_t size)
{
#ifdef USE_SYSTEM_ALLOC_CO13
  return new char[size];
#else
  if(m_useStackHeap)
    return m_stackHeap.Malloc(size);
  else{
    void *p = allocate(size);
    m_memBlockStack.Push(p);
    return p;
  }
#endif
}

/*----------------------------------------------------------------------*/

void OMS_Context::RemoveUnlockedObjFromCache() 
{
  m_oidDir.RemoveUnlockedObjFromCache();
}

/*----------------------------------------------------------------------*/

inline tgg01_OmsVersionContext*  OMS_Context::VersionContext() 
{
  // To prevent reading unchanged objects from an unloaded version in the kernel,
  // the VersionContext is always returned as NULL if we are in a Read-Only-Context.
  return IsReadOnlyContext() 
    ? NULL
    : m_pVersionContext;
}

/*----------------------------------------------------------------------*/

inline OMS_ObjectId8 OMS_Context::VersionNewOid()
{
  IncVersionKey();
  OMS_ObjectId8 oid(OMS_VERSION_OBJ_PAGE_NO, 
                    CurrentVersionKey() % (256 * 256),
                    CurrentVersionKey() / (256 * 256));
  return oid;
}

/*----------------------------------------------------------------------*/

inline void* OMS_Context::operator new(size_t sz)
{
  return OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(sz);
}

/*----------------------------------------------------------------------*/

// PTS 1125361
inline OmsObjectContainer* OMS_Context::ReactivateReplacedVersion(OmsObjectContainer* p)
{
  return m_oidDir.ReactivateReplacedVersion(p);
}

/*----------------------------------------------------------------------*/
#if defined(OMS_PLACEMENT_DELETE)
inline void OMS_Context::operator delete (void* p)
{
  REINTERPRET_CAST(OMS_Context*, p)->DeleteSelf();
}
#endif

/*----------------------------------------------------------------------*/

#include "OMS_DbpBase.hpp"

inline OMS_Session* OMS_Context::GetSession(bool checkNull) const
{
#ifdef _ASSERT_OMS
  if (m_session == NULL && checkNull){

    // !"§$ ONLY FOR DEBUGGING
    DbpBase b(OMS_Globals::GetCurrentLcSink());
    char versionId[sizeof(OmsVersionId)+1];
    SAPDB_MemCopyNoCheck(&versionId[0], &m_version[0], sizeof(OmsVersionId));
    versionId[sizeof(OmsVersionId)] = 0;
    b.dbpOpMsg("Session-pointer of context %s equals NULL", &versionId[0]);

    // Provoke a dump with callstack
    CMyAbort().Abort();

    OMS_Globals::Throw(e_nil_pointer, "Session-pointer equals NULL", __MY_FILE__, __LINE__);
    return NULL;
  }
#endif

  return m_session;
} 

/*----------------------------------------------------------------------*/

inline void OMS_Context::SetSession(OMS_Session *pSession) 
{ 
  if (pSession != NULL && m_session != NULL && m_session != pSession){

    // !"§$ ONLY FOR DEBUGGING
    DbpBase b(OMS_Globals::GetCurrentLcSink());
    char versionId[sizeof(GetVersionId())+1];
    SAPDB_MemCopyNoCheck(&versionId[0], &GetVersionId()[0], sizeof(OmsVersionId));
    versionId[sizeof(OmsVersionId)] = 0;
    b.dbpOpMsg("Overwriting non-NULL Session-pointer (old=%d  new=%d) of context %s", 
      m_session?m_session->GetTaskId():0,
      pSession?pSession->GetTaskId():0,
      &versionId[0]);

    // Provoke a dump with callstack
    CMyAbort().Abort();

    OMS_Globals::Throw(e_nil_pointer, "Overwriting non-NULL Session-pointer", __MY_FILE__, __LINE__);
  }

  m_session = pSession; 
}

/*----------------------------------------------------------------------*/

inline void OMS_Context::MarkBoundToTrans(OMS_Session *pSession) 
{ 
  m_boundToTrans = true; 
  SetSession(pSession);  
  GetContainerDir().AllowChangeOfClassEntryPtr();
}

/*----------------------------------------------------------------------*/

#endif // __OMS_CONTEXT_HPP
