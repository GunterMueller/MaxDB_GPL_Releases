/*!
 * \file    OMS_ContainerEntry.cpp
 * \author  MarkusSi, Roterring
 * \brief   Single entry of OMS Container Dictionary.
 */
/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#include "Oms/OMS_ContainerEntry.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_ClassEntry.hpp"
#include "Oms/OMS_FreeListHeader.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_Session.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*----------------------------------------------------------------------------------*/

OMS_ContainerEntry::OMS_ContainerEntry (OMS_Context               *pContext,
                                        OMS_ClassEntry            *pClassEntry, 
                                        const tgg01_ContainerId   &containerId, 
                                        OmsSchemaHandle            schema, 
                                        OmsContainerNo             containerNo,
                                        bool                       useCachedKeys,
                                        bool                       partitionedKeys) 
: m_pContext(pContext)
, m_pClassEntry(pClassEntry)
, m_classEntryVersion(pClassEntry->GetVersion())
, m_containerNo(containerNo) 
, m_schema(schema)
, m_dropped(false) 
, m_created(false)
, m_index() 
, m_useCachedKeys(false)
, m_partitionedKeys(partitionedKeys)
, m_cacheMiss()           
, m_cacheMissCmp(NULL)   
, m_reusableOids() 
, m_newVersionObjects(pClassEntry->GetTransientSize())
, m_pContHandleHashNext(NULL) 
, m_pContIdHashNext(NULL)
, m_pFreeHead(NULL)
, m_pFreeHeadInVersion(NULL)
, m_nextInHistory(NULL)
, m_cntNewObjectsToFlush(0)
, m_guid(pClassEntry->GetGuid())
, m_containerHandle(containerId.fileDirFileId_gg00())
{
  SAPDB_MemCopyNoCheck(&m_containerId, &containerId, sizeof (m_containerId));

  m_index.AdviseCompare(this);
  m_index.AdviseAllocator(m_pContext);

  // PTS 1127661  
  // 0=Reuse is switched off.
  m_reusableOids.Advise(m_pContext, 0); 

  SetCachedKeys(useCachedKeys);

  // Get pointer to free list headers with the needed size for this class
  m_pFreeHead           = m_pContext->GetContainerDir().GetFreeList(m_pClassEntry->GetTransientSize()); 
  m_pFreeHeadInVersion  = m_pContext->GetContainerDir().GetFreeList(OMS_List<OmsObjectContainer>::GetListEntrySize(m_pClassEntry->GetTransientSize()));
}

/*----------------------------------------------------------------------*/

OMS_ContainerEntry::~OMS_ContainerEntry()
{
}

/*----------------------------------------------------------------------*/

void* OMS_ContainerEntry::operator new (size_t sz, OMS_Context* pContext)
{
#if defined(_ASSERT_OMS)
  void *p = pContext->allocate(sz);
  memset (p, 0x6b, sz);
  return p;
#else
  return pContext->allocate(sz);
#endif
}

/*----------------------------------------------------------------------*/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_ContainerEntry::operator delete (void* p, OMS_Context *pContext)
{
  pContext->deallocate(p);
}
#endif

/*----------------------------------------------------------------------*/

void OMS_ContainerEntry::DeleteSelf() 
{
  // Delete corresponding persistent objects out of the oms cache
  m_pContext->m_oidDir.Clean(this);
  m_pContext->ClearReadOnlyObjCache(/*deleteObjects=*/true, this);

  // Delete complete structure for new version objects / cached keys 
  VersionDelIndex(false);

  // PTS 1117571
  // Delete complete structure of the cache misses
  if (UseCachedKeys()){
    DropCacheMisses();
    if (m_cacheMissCmp != NULL){
      m_cacheMissCmp->deleteSelf(m_pContext);
      m_cacheMissCmp = NULL;
    }
  }

  ClearReusableOids();   // PTS 1127661

  OMS_Context *pContext = m_pContext;
#if defined(_ASSERT_OMS)
  memset (this, 0x7b, sizeof(*this));
#endif
  pContext->deallocate(this);
}

/*----------------------------------------------------------------------*/
// PTS 1117571
// Delete complete cache miss structure                           
void OMS_ContainerEntry::DropCacheMisses() 
{ 
  const char* msg = "OMS_ContainerEntry::DropCacheMisses";

  // Delete dynamically created cache-miss-entries 
  cgg250AvlTree<OMS_CacheMissEntry*,OMS_CacheMissEntry,OMS_Context>::Iterator nodeIter;
  OMS_CacheMissEntry*const* pCurrEntry;

  // Delete the cache miss entries
  nodeIter = m_cacheMiss.First();
  while (nodeIter) {
    pCurrEntry = nodeIter();
    (*pCurrEntry)->deleteSelf(m_pContext); 

    ++nodeIter;
  }

  // Delete the tree itself
  m_cacheMiss.DeleteAll();
  OMS_DETAIL_TRACE(omsTrKey, m_pContext->GetSession()->m_lcSink, msg << " DeleteAll");
}

/*----------------------------------------------------------------------*/

void OMS_ContainerEntry::VersionDelIndex(bool cachedKeysOnly) 
{
  const char* msg = "OMS_ContainerEntry::VersionDelIndex";

  if (!cachedKeysOnly || !m_pContext->IsVersion()){
    // Delete the complete tree. This is possible, when either
    // - all entries should be deleted and not only those which are added by the
    //   cached key feature, or
    // - when no version is active
    m_index.DeleteAll();
    OMS_DETAIL_TRACE(omsTrKey, m_pContext->GetSession()->m_lcSink, msg << " DeleteAll");
  }
  else {
    // Delete only those entries which were added by the cached key feature
    // As it is not possible to delete entries while looping over an iterator
    // a temporary list is created with those entries which should be remain,
    // and after the whole index is deleted those entries are inserted again.
    cgg250AvlTree<unsigned char*, OMS_ContainerEntry, OMS_Context>::Iterator objectIter;
    cgg251DCList<OmsObjectContainer*, OMS_Context> tmpList;    
    OmsObjectContainer* pCurrObject;

    tmpList.advise_allocator(m_pContext);

    objectIter = m_index.First();
    while (objectIter) {
      pCurrObject = VersionGetInfoFromNode(objectIter());          
      if (!pCurrObject) {
        OMS_Globals::Throw(e_OMS_system_error, msg, __MY_FILE__, __LINE__);
      }

      if (m_pContext->IsVersionOid (pCurrObject->m_oid)) {
        tmpList.push_back(pCurrObject);
      }
      else {
        OMS_DETAIL_TRACE(omsTrKey, m_pContext->GetSession()->m_lcSink, msg << " Delete Object " << pCurrObject->m_oid);
      }

      ++objectIter;
    }

    m_index.DeleteAll();
    for (cgg251dclIterator<OmsObjectContainer*,OMS_Context> listIter 
      = tmpList.begin(); listIter; ++listIter) {
        VersionAddKey(*listIter());
      }
      tmpList.remove_all();
  }
}

/*----------------------------------------------------------------------*/

// PTS 1117571
bool OMS_ContainerEntry::SetCachedKeys(bool on) 
{
  // Cached Keys are only defined on keyed objects
  //if (!IsKeyedObject()){
  if (!GetClassEntry().IsKeyedObject()){
    return false;
  }

  // No change => nothing to do
  if (on == UseCachedKeys()){
    return false;
  }

  if (on) {
    // Cached Keys are switched on

    // Initialization of structures for cache misses
    m_cacheMissCmp = new (GetClassEntry().GetKeyDesc().GetLen(), m_pContext) 
      OMS_CacheMissEntry (NULL, GetClassEntry().GetKeyDesc().GetLen());
    m_cacheMiss.AdviseCompare(m_cacheMissCmp);               
    m_cacheMiss.AdviseAllocator(m_pContext);  

    m_useCachedKeys = true;
  }
  else {
    // Cached Keys are switched off

    // Delete all entries which are not created in a version
    VersionDelIndex(true);

    // Reset of structures for cache misses
    DropCacheMisses();
    if (m_cacheMissCmp != NULL){
      m_cacheMissCmp->deleteSelf(m_pContext); 
      m_cacheMissCmp = NULL;
    }
    m_cacheMiss.AdviseCompare(NULL);               
    m_cacheMiss.AdviseAllocator(NULL);  

    m_useCachedKeys = false;
  }

  return true;
}

/*----------------------------------------------------------------------*/

void  OMS_ContainerEntry::ClearNewVersionObjects()
{
  if (m_pContext->IsVersion()){
    OMS_TRACE (omsTrNewObj, m_pContext->GetSession()->m_lcSink, "NewVersionObj Clear: " 
      << " Guid:"   << GetGuid()
      << " Schema:" << GetSchema()
      << " CNo:"    << GetContainerNo());  

    m_newVersionObjects.SetEmpty();
  }
}

/*----------------------------------------------------------------------*/
OMS_ClassEntry& OMS_ContainerEntry::SetClassEntry()
{
  // Check whether it is allowed to change the class-entry-pointer in the current situation
  if (!m_pContext->GetContainerDir().IsChangeOfClassEntryPtrAllowed()){
    DbpBase b(OMS_Globals::GetCurrentLcSink());
    char buf[200];
    sp77sprintf(buf, sizeof(buf), "OMS_ContainerEntry::GetClassEntry: Try to set class-entry although this is not allowed. (%d, %d, %d)",
      GetGuid(), GetSchema(), GetContainerNo());
    b.dbpOpError(buf);
    Throw(e_wrong_class_entry_ptr, buf, __MY_FILE__, __LINE__);       
  }

  if (m_pContext->GetSession(/*checkNULL=*/false) == NULL){
    char buf[128];
    sp77sprintf(buf, sizeof(buf), "Session pointer in context (%s) equals NULL", m_pContext->GetVersionId());
    Throw(e_nil_pointer, buf, __MY_FILE__, __LINE__);       
  }
  else {
    m_pClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(m_guid);
    if (m_pClassEntry != NULL){
      m_classEntryVersion = m_pClassEntry->GetVersion();
    }
  }

  return *m_pClassEntry;
}

/*----------------------------------------------------------------------*/

void OMS_ContainerEntry::CheckClassEntryVersionError()
{
  if (m_pClassEntry != NULL && m_pClassEntry->GetVersion() != m_classEntryVersion){
    DbpBase b(OMS_Globals::GetCurrentLcSink());
    char buf[200];
    sp77sprintf(buf, sizeof(buf), "OMS_ContainerEntry::GetClassEntry: Wrong version: actual=%d; registered=%d",
      m_pClassEntry->GetVersion(), m_classEntryVersion);
    b.dbpOpError(buf);
    Throw(e_wrong_class_entry_ptr, buf, __MY_FILE__, __LINE__);       
  }
}

/*----------------------------------------------------------------------*/

cgg250AvlTree<unsigned char*,OMS_ContainerEntry,OMS_Context>::Iterator OMS_ContainerEntry::VersionIterStart(unsigned char* pStartKey, OmsIterDirection direction)
{
  return m_index.Locate(pStartKey, direction);
}

/*----------------------------------------------------------------------*/

void OMS_ContainerEntry::Throw(short errorNo, const char* msg, const char* pFile, unsigned int line)
{
  OMS_Globals::Throw(errorNo, msg, pFile, line, m_pContext->GetSession());   
}

/*----------------------------------------------------------------------*/

void OMS_ContainerEntry::Throw(short errorNo, const char* msg, const OMS_ObjectId8 &oid, const char* pFile, unsigned int line)
{
  OMS_Globals::Throw(errorNo, msg, oid, pFile, line, m_pContext->GetSession());   
}

