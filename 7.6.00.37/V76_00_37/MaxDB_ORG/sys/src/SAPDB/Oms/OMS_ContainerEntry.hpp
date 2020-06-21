/*!
 * \file    OMS_ContainerEntry.hpp
 * \author  MarkusSi, Roterring
 * \brief   Single entry of the container directory.
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


#ifndef __OMS_CONTAINERENTRY_HPP
#define __OMS_CONTAINERENTRY_HPP

#include "Oms/OMS_ContainerEntryDef.hpp"
#include "Oms/OMS_ContextDef.hpp"
#include "Oms/OMS_SessionDef.hpp"    
#include "Oms/OMS_Trace.hpp"
#include "Oms/OMS_FreeListHeader.hpp"
#include "Oms/OMS_VarObjInfo.hpp"
#include "Oms/OMS_DbpBase.hpp"



/*----------------------------------------------------------------------*/
/// Check whether key is already registered as a cache miss     
inline bool OMS_ContainerEntry::IsCacheMiss(const unsigned char *key)
{ 
  m_cacheMissCmp->setKey(key, GetClassEntry().GetKeyDesc().GetLen());    
  return m_cacheMiss.Find(m_cacheMissCmp) != NULL ? true : false;
}

/*----------------------------------------------------------------------*/
/// Delete specified entry from the cache miss structure           
inline void OMS_ContainerEntry::DeleteCacheMiss(const unsigned char *key)
{
  m_cacheMissCmp->setKey(key, GetClassEntry().GetKeyDesc().GetLen());  
  
  // Find pointer to the node in the AVL-Tree
  OMS_CacheMissEntry*const* pCacheMissEntry = m_cacheMiss.Find(m_cacheMissCmp); 
  OMS_CacheMissEntry*       pDelEntry       = const_cast<OMS_CacheMissEntry*>(*pCacheMissEntry);

  // Delete the node in the AVL-Tree
  tgg00_BasisError rc = m_cacheMiss.Delete(m_cacheMissCmp);
  if (e_ok != rc) {
    // Object does not existent is the structure
    Throw(rc, "OMS_ContainerEntry::DeleteCacheMiss", __MY_FILE__, __LINE__); 
  }

  // Delete the cache miss entry 
  pDelEntry->deleteSelf(m_pContext);
}

/*----------------------------------------------------------------------*/
/// Insert entry into the cache miss structure 
inline void OMS_ContainerEntry::InsertCacheMiss(const unsigned char *key)
{
  // Create object for cache miss structure
  OMS_CacheMissEntry* pCacheMiss 
    = new (GetClassEntry().GetKeyDesc().GetLen(), m_pContext) 
          OMS_CacheMissEntry (key, GetClassEntry().GetKeyDesc().GetLen());  

  tgg00_BasisError rc 
    = m_cacheMiss.Insert(const_cast<OMS_CacheMissEntry*>(pCacheMiss));
  if (e_ok != rc) {
    // Object is already existent is the structure
    Throw(rc, "OMS_ContainerEntry::InsertCacheMiss", __MY_FILE__, __LINE__); 
  }
}

/*----------------------------------------------------------------------*/

inline int OMS_ContainerEntry::Compare(const unsigned char* l, const unsigned char* r) 
{
  int cmp = memcmp (l, r, GetClassEntry().GetKeyDesc().GetLen());  
  if (0 != cmp) {
    return (cmp > 0 ? 1 : -1);
  }
  else {
    return 0;
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_ContainerEntry::UseCachedKeys() const {  
  return m_useCachedKeys;
}

/*----------------------------------------------------------------------*/

tgg00_BasisError OMS_ContainerEntry::VersionAddKey(OmsObjectContainer* p, bool noThrowIfExist) 
{
  const char* msg = "OMS_ContainerEntry::VersionAddKey";

  tgg00_BasisError rc;
  rc = m_index.Insert(GetClassEntry().GetKeyPtr(p));
  if (rc != e_ok && (noThrowIfExist == false || rc != e_duplicate_key)){
    if (rc == e_duplicate_key){
      rc = e_duplicate_hash_key;
    }
    Throw(rc, "OMS_ContainerEntry::VersionAddKey", __MY_FILE__, __LINE__); 
  }

  OMS_DETAIL_TRACE(omsTrKey, m_pContext->GetSession()->m_lcSink, msg 
                       << " Insert Object " << p->m_oid
                       << " RC: " << rc);

  return rc;
}

/*----------------------------------------------------------------------*/

void OMS_ContainerEntry::VersionDelKey(OmsObjectContainer* p) 
{
  const char* msg = "OMS_ContainerEntry::VersionDelKey";

  tgg00_BasisError    rc    = 0;
  OmsObjectContainer* found = NULL;

  bool check = false;

  if (check){
    // Check that the object frame, to which the entry with the given key in the tree
    // is pointing to, is the same as the given object frame
    found = VersionFindKey(GetClassEntry().GetKeyPtr(p));
  }

  if (!check || (found && found == p)){
    rc = m_index.Delete(GetClassEntry().GetKeyPtr(p));

    OMS_DETAIL_TRACE(omsTrKey, m_pContext->GetSession()->m_lcSink, msg 
                       << " Delete Object " << p->m_oid
                       << " RC: " << rc);
  }
  else if (check && found && found != p) {
    OMS_DETAIL_TRACE(omsTrKey, m_pContext->GetSession()->m_lcSink, msg 
                       << " Object " << p->m_oid << " not deleted as tree is pointing to other object");
  }
}

/*----------------------------------------------------------------------*/

OmsObjectContainer* OMS_ContainerEntry::VersionFindKey(const unsigned char* k) {
  unsigned char* const* p = m_index.Find(CONST_CAST(unsigned char*, k));
    
  return p != NULL ? VersionGetInfoFromNode(p) : NULL;
}

/*----------------------------------------------------------------------*/

void OMS_ContainerEntry::VersionReplaceOrAddKeyContainerPtr(OmsObjectContainer* pObj)
{
  const char* msg = "OMS_ContainerEntry::VersionDelKey";

  unsigned char* const* p = m_index.Find(GetClassEntry().GetKeyPtr(pObj));
  if (NULL != p)
  {
    *CONST_CAST(unsigned char**, p) = GetClassEntry().GetKeyPtr(pObj);
    OMS_DETAIL_TRACE(omsTrKey, m_pContext->GetSession()->m_lcSink, msg 
                       << " Object changed " << pObj->m_oid);
  }
  else
  {
     VersionAddKey(pObj);
  }
}

/*----------------------------------------------------------------------*/
// Returns the user-data (pointer to a object-container) from a node of 
// the version/cached-key search structure 
OmsObjectContainer* OMS_ContainerEntry::VersionGetInfoFromNode(const unsigned char* const* p) {
    return reinterpret_cast<OmsObjectContainer*>(
           const_cast<unsigned char*>((*p) 
                                      + GetClassEntry().GetKeyDesc().GetLen()
                                      - GetClassEntry().GetTransientSize()));  
}

/*----------------------------------------------------------------------*/
// PTS 1127661
inline void OMS_ContainerEntry::ClearReusableOids()
{
  OMS_Session    *pSession = m_pContext->GetSession(/*checkNull=*/false);
  IliveCacheSink *pSink    = (pSession ? pSession->m_lcSink : OMS_Globals::GetCurrentLcSink());
  OMS_TRACE(omsTrNewObj, pSink, "REUSE-CACHE(" << (void*)m_pContext << ","  << GetContainerHandle() << "," << GetGuid() << "): ClearReusableOids");

  m_reusableOids.Clear(); 
}

/*----------------------------------------------------------------------*/
// PTS 1127661
inline bool OMS_ContainerEntry::ExistsReusableOid() const
{
  bool empty = m_reusableOids.IsEmpty();

  OMS_Session    *pSession = m_pContext->GetSession(/*checkNull=*/false);
  IliveCacheSink *pSink    = (pSession ? pSession->m_lcSink : OMS_Globals::GetCurrentLcSink());
  OMS_TRACE(omsTrNewObj, pSink, "REUSE-CACHE(" << (void*)m_pContext << "," << GetContainerHandle() << "," << const_cast<OMS_ContainerEntry*>(this)->GetGuid() << "): ExistsReusableOid Empty=" << (empty ? "YES" : "NO"));
  
  return !empty;
}

/*----------------------------------------------------------------------*/
// PTS 1127661
inline const OMS_ObjectId8* OMS_ContainerEntry::GetReusableOid()
{
  const OMS_ObjectId8* oid = m_reusableOids.Pop();

  OMS_Session    *pSession = m_pContext->GetSession(/*checkNull=*/false);
  IliveCacheSink *pSink    = (pSession ? pSession->m_lcSink : OMS_Globals::GetCurrentLcSink());
  OMS_TRACE(omsTrNewObj, pSink, "REUSE-CACHE(" << (void*)m_pContext << "," << GetContainerHandle() << "," << GetGuid() << "): GetReusableOid " << *oid);

  return oid;
}

/*----------------------------------------------------------------------*/
// PTS 1127661
inline void OMS_ContainerEntry::InsertReusableOid(const OMS_ObjectId8 &oid)
{
  OMS_Session    *pSession = m_pContext->GetSession(/*checkNull=*/false);
  IliveCacheSink *pSink    = (pSession ? pSession->m_lcSink : OMS_Globals::GetCurrentLcSink());
  OMS_TRACE(omsTrNewObj, pSink, "REUSE-CACHE(" << (void*)m_pContext << "," << GetContainerHandle() << "," << GetGuid() << "): InsertReusableOid " << oid);
   
  m_reusableOids.Push(oid);
}

/*----------------------------------------------------------------------*/

inline void OMS_ContainerEntry::RegisterNewVersionObject(OmsObjectContainer *p)
{
  if (m_pContext->IsVersion())
  {
    OMS_TRACE (omsTrNewObj, m_pContext->GetSession()->m_lcSink, "NewVersionObj Insert: " << p->m_oid  
      << " Guid:"   << GetGuid()
      << " Schema:" << GetSchema()
      << " CNo:"    << GetContainerNo()
      << " Ptr:"    << p);  
    m_newVersionObjects.Insert(p);
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_ContainerEntry::RemoveNewVersionObject(OmsObjectContainer *p)
{
  if (m_pContext->IsVersion()){
    OMS_TRACE (omsTrNewObj, m_pContext->GetSession()->m_lcSink, "NewVersionObj Remove: " << p->m_oid
      << " Guid:"   << GetGuid()
      << " Schema:" << GetSchema()
      << " CNo:"    << GetContainerNo()
      << " Ptr:"    << p);  

    bool error = m_newVersionObjects.Remove(p);
    if (error){
      Throw(e_nil_pointer, "OMS_ContainerEntry::RemoveNewVersionObject", p->m_oid, __MY_FILE__, __LINE__);
    }
  }
}

/*----------------------------------------------------------------------*/

inline OMS_FreeListHeader& OMS_ContainerEntry::GetFreeList(bool inVersion) const
{
  return inVersion ? *m_pFreeHeadInVersion
                   : *m_pFreeHead;
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_ContainerEntry::GetMemory(bool forNewVersionObj) 
{ 
  size_t size = 0;
  if (forNewVersionObj){
    // New version objects own two additional pointers to chain these entries (see OMS_ContainerEntry::m_newVersionObjects)
    size  = OMS_List<OmsObjectContainer>::GetListEntrySize(GetClassEntry().GetTransientSize());  
  }
  else {
    size  = GetClassEntry().GetTransientSize();   
  }

  OMS_TRACE(omsTrMemory, m_pContext->GetSession()->m_lcSink, "OMS GetMemory in ContainerEntry: " 
                                   << " Size=" << (long)size
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  bool reuseFrame = true;

  // Look for empty frame in reuse list
  OmsObjectContainer *p = GetFreeList(forNewVersionObj).GetFrame();
 
  if (p == NULL) {
    reuseFrame = false;

    // No frame for reuse found
    //getFromFreeList = false;
    if (m_pContext->IsVersion()) {
      // Allocate memory in the version allocator
      p = (OmsObjectContainerPtr) m_pContext->allocate(size);  
    }
    else {
      // Allocate memory in the stack allocator of the context
      p = (OmsObjectContainerPtr) m_pContext->StackHeapMalloc(size);   
    }
  }

  if (NULL != p) {
    p->InitObjContainer(GetClassEntry());

    if (forNewVersionObj){
      // Initialize pointers, which are used to chain new version objects
      p->InitVerNew(size - 2*sizeof(void*));  
    }
  }

  OMS_TRACE(omsTrMemory, m_pContext->GetSession()->m_lcSink, "    => Object="  << (void*)p 
           << (reuseFrame?" Reused":" New") 
           << (forNewVersionObj?" VersionObject":" NormalObject")); 

  return p;
}

/*----------------------------------------------------------------------*/

inline void OMS_ContainerEntry::ChainFree (OmsObjectContainer  *&pObj, int caller) 
{
  if (NULL == pObj) {
    return;
  }

  OMS_TRACE(omsTrMemory, m_pContext->GetSession()->m_lcSink, "OMS ChainFree in ContainerEntry: " 
                                   << " Object=" << (void*)pObj
                                   << " VersionObject=" << (pObj->IsVerNewObject()?"Yes":"No")
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  if (pObj->IsHeaderOnly()){
    Throw(e_invalid_deallocate, "ChainFree: unvalid freeing of 'header-only'-object", __MY_FILE__, __LINE__);       
    return;
  }

#ifdef USE_SYSTEM_ALLOC_CO13
  if (p->VarObjFlag()) {
    REINTERPRET_CAST(OMS_VarObjInfo*, &pObj->m_pobj)->freeVarObj(m_pContext); // PTS 1134841
  }
  m_pContext->deallocate(pObj);
#else
  if (pObj->VarObjFlag()) {
      REINTERPRET_CAST(OMS_VarObjInfo*, &pObj->m_pobj)->freeVarObj(m_pContext);
    }
//#ifdef _ASSERT_OMS
  pObj->InitializeForFreeList(caller);
//#endif
  if (pObj->IsVerNewObject()) {
    m_pFreeHeadInVersion->Insert(pObj);
  } 
  else {
    m_pFreeHead->Insert(pObj);
  }
#endif
  pObj = NULL;
}

/*----------------------------------------------------------------------*/

inline void OMS_ContainerEntry::CheckIfNotDropped(const char *pFile, unsigned int line) 
{
  if (IsDropped()) {
    m_pContext->GetContainerDir().ThrowUnknownContainer(this, pFile, line);
  }
}

/*----------------------------------------------------------------------*/

inline bool OMS_ContainerEntry::IsDropped()  
{ 
  const char *msg = "OMS_ContainerEntry::IsDropped";

  if (!m_pContext->IsVersion() || m_pContext->GetSession()->IsAfterKernelTransEnd()){
    // As a container is locked when an object is dereferenced from the kernel, no separate
    // call to lock the container is needed if we are not in a version. 
    // If the transaction has been ended in the kernel already, then no locks should be 
    // acquired as these would be hold until the end of the next transaction.
    return m_dropped;     
  }
  else {
    // As versions might store objects in their caches together with the corresponding
    // entries in the container-directory at the very begin of a transaction
    // it is possible to dereference an object without communication with the kernel. 
    // To ensure, that the container still exists, the first call of 'IsDropped' is
    // send to the kernel and a shared lock on the container is acquired. To reduce
    // the performance penalty later requests (for a non-dropped container) are answered 
    // without kernel-communication by using a local indicator flag.
    if (!m_sharedLockInKernel){
      short error = 0;
      m_pContext->GetSession()->m_lcSink->LockContainer(GetFileId(), error); 
      if (error == e_container_dropped){
        return true;
      }
      else if (error != 0){
        Throw(error, msg, __MY_FILE__, __LINE__); 
      }
      m_sharedLockInKernel = true;
    }
  }
  return false;
}

/*----------------------------------------------------------------------*/


#endif  // __OMS_CONTAINERENTRY_HPP
