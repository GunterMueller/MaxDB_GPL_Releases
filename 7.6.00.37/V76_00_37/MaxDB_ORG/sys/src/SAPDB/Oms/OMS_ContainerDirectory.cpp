/*!
 * \file    OMS_ContainerDirectory.cpp
 * \author  MarkusSi, Roterring
 * \brief   Implementation of local container dictionary
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



#include "Oms/OMS_ContainerDirectory.hpp"

#include "Oms/OMS_FreeListHeader.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_ArrayObject.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_Trace.hpp"
#include "hsp77.h"
#include "geo573.h"

/*----------------------------------------------------------------------*/

OMS_ContainerDirectory::OMS_ContainerDirectory () 
: m_headentries(0)
, m_count(0)
, m_ppContHandleHead(NULL)
, m_ppContIdHead(NULL)
, m_pFreeListHeader(NULL)
, m_pContext(NULL)
{
}

/*----------------------------------------------------------------------*/

OMS_ContainerDirectory::~OMS_ContainerDirectory () 
{
  if (NULL != m_pContext)
  {
    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Destruction of ContainerDirectory" <<
      " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

    // Delete entries from both container hashes
    OMS_ContainerEntry* pCurr;
    OMS_ContainerEntry* pDel;
    for (int ix = 0; ix < m_headentries; ix++) {
      pCurr = m_ppContHandleHead[ix];
      m_ppContHandleHead[ix] = NULL;
      m_ppContIdHead[ix]     = NULL;
      while (pCurr != NULL) {
        pDel   = pCurr;
        pCurr  = pCurr->m_pContHandleHashNext;
        pDel->DeleteSelf();
      }
    }

    m_pContext->deallocate(m_ppContHandleHead);
    m_pContext->deallocate(m_ppContIdHead);
      
    DeleteFreeList(1);
    m_pContext->deallocate(m_pFreeListHeader);
  }

  m_pContext = NULL;
#if defined(_ASSERT_OMS)
  memset (this, 0x4b, sizeof(*this));
#endif

  //new(this) OMS_ContainerDirectory();
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::Create (OMS_Context* pContext) 
{
  if (pContext){
    OMS_TRACE(omsTrContainerDir, pContext->GetSession()->m_lcSink, "OMS Creation of ContainerDirectory: " 
                    << " Context=" << OMS_CharBuffer(&pContext->GetVersionId()[0], sizeof(OmsVersionId)));
  }
  
  m_pContext   = pContext;

  // Allocate memory for hash arrays
  m_ppContHandleHead = reinterpret_cast<OMS_ContainerEntry**>(m_pContext->allocate(sizeof(OMS_ContainerEntry*) * HEAD_ENTRIES));
  m_ppContIdHead     = reinterpret_cast<OMS_ContainerEntry**>(m_pContext->allocate(sizeof(OMS_ContainerEntry*) * HEAD_ENTRIES));
  m_headentries = HEAD_ENTRIES;

  // Initialize hash arrays
  for (int ix = 0; ix < m_headentries; ++ix) {
    m_ppContHandleHead[ix]= NULL;
    m_ppContIdHead[ix]    = NULL;
  }

  m_pFreeListHeader = new(m_pContext) OMS_FreeListHeader(0 /* PTS 1109561 */);
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::RemoveDroppedContainers() 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Remove dropped Container "
                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // remove all infos of dropped containers
  OMS_ContainerDirectory::Iter iter = First();
  while (iter) {
    OMS_ContainerEntry* pCurr = iter();
    ++iter;
    if (pCurr->IsDropped()) {
      Delete(pCurr);
    }
  }
}

/*----------------------------------------------------------------------*/

OMS_ContainerEntry* OMS_ContainerDirectory::AutoRegisterContainer (ClassID          guid, 
                                                                   OmsSchemaHandle  schema, 
                                                                   OmsContainerNo   containerNo,
                                                                   short           &error) 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Auto-register Container:" 
                                   << " GUID=" << guid
                                   << " Schema=" << schema
                                   << " CNo=" << containerNo
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));
  
  // Determine corresponding class entry
  OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(guid);

  // Check whether container exists in the kernel
  bool  useCachedKeys   = false;
  bool  partitionedKeys = false;
  union {
    // ensure, that containerId is aligned correctly.
    void*             align;   
    tgg01_ContainerId containerId;
  }c;
  KERNEL_GUID(kernelGuid, guid);
  m_pContext->GetSession()->m_lcSink->ExistsContainer(schema, kernelGuid, containerNo, 
                                        pClassEntry->GetPersistentSize(),
                                        pClassEntry->GetKeyDesc().GetLen(),
                                        c.containerId, useCachedKeys, partitionedKeys, error);

  if (error != e_ok){  
    return NULL;
  }
  else {
    // Create new local container entry
    OMS_ContainerEntry *pContainerEntry = new(m_pContext) OMS_ContainerEntry (m_pContext, 
                                                          pClassEntry,
                                                          c.containerId,
                                                          schema,
                                                          containerNo,
                                                          useCachedKeys,
                                                          partitionedKeys);
    // Insert entry into local container directory
    Insert(pContainerEntry);

    return pContainerEntry;
  }
}

/*----------------------------------------------------------------------*/

OMS_ContainerEntry* OMS_ContainerDirectory::AutoRegisterContainer (OMS_ContainerHandle &containerHandle) 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Auto-register Container: " 
                                   << containerHandle
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // Get guid, schema, container for given container-handle
  GUID             kernelGuid;
  OmsSchemaHandle  schema;
  OmsContainerNo   cno;
  short            DBError;
  m_pContext->GetSession()->m_lcSink->GetContainerInfo(containerHandle, kernelGuid, schema, cno, DBError);
  if (DBError != e_ok){
    OMS_Globals::Throw(DBError, "AutoRegisterContainer: Error when converting container handle", __MY_FILE__, __LINE__); 
  }

  // Convert kernel guid into liboms guid
  LIBOMS_GUID(guid,kernelGuid);

  if (omsIsArrayGuid(guid)){
    ClassID baseGuid;
    short   category;
    omsSplitArrayGuid(guid, baseGuid, category);

    // Find class entry for base guid
    OMS_ClassEntry *pBaseClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(baseGuid);

    int arrayByteSize = OmsArrayObjectBase::GetMaxByteSize(category);
    return AutoRegisterArrayContainer(baseGuid, schema, cno, arrayByteSize);
  }
  else {
    short error = e_ok;
    OMS_ContainerEntry *pContainerInfo =  AutoRegisterContainer(guid, schema, cno, error);
    if (error == e_ok){
      return pContainerInfo;
    }
    else if (error == e_OMS_sysinfo_not_found) {
      ThrowUnknownContainer(guid, schema, cno, __MY_FILE__, __LINE__);
      return NULL;
    }
    else {
      char buf[256];
      sp77sprintf(buf, sizeof(buf), "AutoRegisterContainer: Guid: %d  Schema: %d  Container: %d",
        guid, schema, cno);
      OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
      return NULL;
    }
  }
}

/*----------------------------------------------------------------------*/
// Returnvalues:
// -1: a timeout has occurred and the noWait option is switched on
//  0: an error occurred during the creation
//  1: the container has been created (if it did not exist already)
int OMS_ContainerDirectory::CreateContainer (ClassID          guid, 
                                             OmsSchemaHandle  schema, 
                                             OmsContainerNo   containerNo,
                                             bool             useCachedKeys,
                                             bool             partitionedKeys,
                                             bool             noWait) 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Create Container: " 
                                   << " GUID=" << guid
                                   << " Schema=" << schema
                                   << " CNo=" << containerNo
                                   << " CachedKeys=" << (useCachedKeys   ? "Y" : "N")
                                   << " PartKeys="   << (partitionedKeys ? "Y" : "N")
                                   << " NoWait="     << (noWait          ? "Y" : "N")
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // Performance-Optimization: To prevent too many request to the kernel, it is checked,
  // whether the container has been accessed already in the current transaction, and if
  // so no further checks must be done.
  OMS_ContainerEntry *pContainerEntry = Find(guid, schema, containerNo, /*checkDropFlag=*/false);
  if (pContainerEntry != NULL && pContainerEntry->IsLocked() && !pContainerEntry->IsDropped()){
    return 1;
  }

  // Determine corresponding class entry
  OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(guid);

  // Create the container in the kernel
  short error = 0;
  union{
    // ensure, that containerId is aligned correctly. Otherwise the
    // reinterpret_cast to OMS_ContainerHandle will fail with bus-error
    void*             align; 
    tgg01_ContainerId containerId;  
  } c;
  KERNEL_GUID(kernelGuid, guid);
  tsp00_KnlIdentifier kernelClassName;
  OMS_Globals::MakeKnlIdentifier(pClassEntry->GetClassName(), kernelClassName);
  m_pContext->GetSession()->m_lcSink->CreateContainer(schema, &kernelGuid, 
                                        &kernelClassName,
                                        pClassEntry->GetPersistentSize(),
                                        containerNo, 
                                        pClassEntry->GetKeyDesc().GetPos(),
                                        pClassEntry->GetKeyDesc().GetLen(),
                                        partitionedKeys,                                                                                                                          
                                        useCachedKeys,
                                        pClassEntry->IsVarObject(),
                                        false, // useInternalTrans
                                        noWait,
                                        (unsigned char*)&c.containerId,
                                        &error);

  bool existsAlready = false;
  if (noWait && (  error == e_OMS_lock_request_timeout
                || error == e_OMS_lock_collision
                || error == e_OMS_request_timeout)){
    // Container is currently locked, but as the option noWait is switched on, do not
    // throw an exception, but return error-code directly.
    return -1;
  }
  else if (error != e_ok && error != e_container_already_exists){
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "CreateContainer: Class (GUID=%d) Schema: %d Container %d",
                                    guid, schema, containerNo);
    OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
    return 0; 
  }
  else if (error == e_container_already_exists){
    // The container exists already in the global directory in the kernel. 
    // Therefore only a local copy must be created and no rollback information
    // must be written
    existsAlready = true;
    error = 0;
  }

  if (pContainerEntry != NULL && !pContainerEntry->IsDropped()){
    // If there is already a local entry, check whether it is out-dated. 
    // This is only possible if an other transaction has deleted and recreated the container.
    const OMS_ContainerHandle *pKernelContainerHandle = reinterpret_cast<const OMS_ContainerHandle*>(&c.containerId.fileDirFileId_gg00());
    if (*pKernelContainerHandle != pContainerEntry->GetContainerHandle()){
      // The locally cached entry is out-dated
      OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "   => Local container entry is dropped: " 
        << pContainerEntry->GetContainerHandle());
    
      if (pContainerEntry->GetNewObjectsToFlush() != 0){
        char buf[256];
        sp77sprintf(buf, sizeof(buf), "Local container entry (handle=%d) is out of sync, but contains new objects: %d",
          pContainerEntry->GetContainerHandle().GetUint8(), pContainerEntry->GetNewObjectsToFlush());
        OMS_Globals::Throw(e_unknown_guid, buf, __MY_FILE__, __LINE__); 
      }

      Delete(pContainerEntry);
      pContainerEntry = NULL;
    }
    else {
      // Entry does already exist locally and as it has been read from the kernel now,
      // mark it as 'locked'. So the next time no access to the kernel is needed to check
      // whether the entry is outdated.
      pContainerEntry->SetLocked();
    }
  }

  if (pContainerEntry == NULL || pContainerEntry->IsDropped()){
    // Create new local container entry
    OMS_ContainerEntry *pNew = new(m_pContext) OMS_ContainerEntry (m_pContext, 
                                                          pClassEntry,
                                                          c.containerId,
                                                          schema,
                                                          containerNo,
                                                          useCachedKeys,
                                                          partitionedKeys);
    // Insert entry into local container directory
    Insert(pNew);
    pNew->SetLocked();

    if (!existsAlready){
      // Insert before-image for subtrans rollback
      m_pContext->GetSession()->m_beforeImages.insertCreateContainerBeforeImage(
        &pNew->GetContainerHandle(),
        pNew,
        m_pContext->GetSession()->CurrentSubtransLevel());

      pNew->MarkCreated();
    }


    if (pContainerEntry != NULL){
      // Append container entry of dropped container to the history chain of the
      // newly created container and delete the entry in the directory
      pNew->m_nextInHistory = pContainerEntry;
      Delete(pContainerEntry, /*deleteEntry=*/false);
    }

    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "   => New container entry has been created: " 
      << pNew->GetContainerHandle());
  }

  return 1;
}

/*----------------------------------------------------------------------*/
  
OMS_ContainerEntry* OMS_ContainerDirectory::AutoRegisterArrayContainer(ClassID         baseGuid, 
                                                                       OmsSchemaHandle schema, 
                                                                       OmsContainerNo  containerNo, 
                                                                       int          arrayByteSize)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Auto-register Arraycontainer: " 
                                   << " Base-GUID=" << baseGuid
                                   << " Schema="    << schema
                                   << " CNo="       << containerNo
                                   << " Size="      << arrayByteSize
                                   << " Context="   << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));
  
  // Find class entry for base guid
  OMS_ClassEntry *pBaseClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(baseGuid);

  // Determine category for given number of array-elements
  short category = OmsArrayObjectBase::GetCategory(arrayByteSize);

  // convert guid (encode size)
  // As a guid at kernel side must correspond to a fixed size, the different sizes, which are
  // used in the liboms using the same logical guid, must be converted to different guids.
  ClassID guid = omsMakeArrayGuid(baseGuid, category);

  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink,  
                                   "    GUID="      << guid
                                   << " Category="  << category
                                   << " MaxSize="   << arrayByteSize);

  OMS_ContainerEntry *pContainerEntry = NULL;

  // Check whether a container for this size-category already exists in the kernel
  bool              useCachedKeys           = false;
  bool              partitionedKeys         = false;
  bool              useInternalTrans        = true;
  short             error = 0;
  union {
    // ensure, that containerId is aligned correctly. Otherwise the
    // reinterpret_cast to OMS_ContainerHandle will fail with bus-error
    void*             align;  
    tgg01_ContainerId containerId;  
  } c;
  KERNEL_GUID(kernelGuid, guid);
  m_pContext->GetSession()->m_lcSink->ExistsContainer(schema, kernelGuid, containerNo, 
                                        /*size=*/0, /*key-len=*/0,
                                        c.containerId, useCachedKeys, partitionedKeys, error);
  
  if (error == e_ok){
    OMS_ContainerEntry *pOld = Find(guid, schema, containerNo, /*checkDropped=*/false);
    if (pOld != NULL){
      // If there is already a local entry, check whether it is out-dated. 
      // This is only possible if an other transaction has deleted and recreated the container.
      const OMS_ContainerHandle *pKernelContainerHandle = reinterpret_cast<const OMS_ContainerHandle*>(&c.containerId.fileDirFileId_gg00());
      if (*pKernelContainerHandle == pOld->GetContainerHandle()){
        // nothing to do
        pContainerEntry = pOld;
      }
      else {
        // As a container for this category exists already, the class entry must exist, too.
        OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().Find(guid);

        // Create new container entry for the converted guid
        pContainerEntry = new(m_pContext) OMS_ContainerEntry (m_pContext, 
                                                              pClassEntry,
                                                              c.containerId,
                                                              schema,
                                                              containerNo,
                                                              useCachedKeys,
                                                              partitionedKeys);
        Insert( pContainerEntry );

        // delete the entry in the directory
        Delete( pOld, /*deleteEntry=*/ true );
      }
    }
    else {
      // Up to now there is no local entry
      
      // Check if class entry of converted guid already exists locally and if not create one
      OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().Find(guid);
      if( pClassEntry == NULL )
      {
        char className[256];
        sp77sprintf(className, sizeof(className), "%s(%d)", 
                    pBaseClassEntry->GetClassName(), category);

        pClassEntry = m_pContext->GetSession()->GetClassDir().RegisterClass (
                                  &className[0], guid, baseGuid, OMS_KeyDesc(), 
                                  arrayByteSize,
                                  pBaseClassEntry->GetVTblPtr(),
                                  /*isVarObject=*/false, /*isArrayObject=*/true);
      }
      
      // now create new container entry
      pContainerEntry = new(m_pContext) OMS_ContainerEntry (m_pContext, 
                                                            pClassEntry,
                                                            c.containerId,
                                                            schema,
                                                            containerNo,
                                                            useCachedKeys,
                                                            partitionedKeys);
      Insert( pContainerEntry );
    }
  }
  else if (error == e_OMS_sysinfo_not_found){  
    // Sub-container for given size does not exist in the kernel yet.
    // Therefore check if base class exists in the kernel and throw an exception if not.
    union {
      // ensure, that containerId is aligned correctly. Otherwise the
      // reinterpret_cast to OMS_ContainerHandle will fail with bus-error
      void*             align;  
      tgg01_ContainerId containerId; 
    } base;
    KERNEL_GUID(kernelBaseGuid, baseGuid);
    m_pContext->GetSession()->m_lcSink->ExistsContainer(schema, kernelBaseGuid, containerNo, 
                                          /*size=*/0, /*key-len=*/0,
                                          base.containerId, useCachedKeys, partitionedKeys, error);
    if( error == e_ok )
    {
      OMS_ContainerEntry *pBaseContainerEntry = Find(baseGuid, schema, containerNo, /*checkDropped=*/false);
      if (pBaseContainerEntry != NULL){
        // If there is already a local entry, check whether it is out-dated. 
        // This is only possible if an other transaction has deleted and recreated the container.
        const OMS_ContainerHandle *pKernelContainerHandle = reinterpret_cast<const OMS_ContainerHandle*>(&base.containerId.fileDirFileId_gg00());
        if (*pKernelContainerHandle == pBaseContainerEntry->GetContainerHandle()){
          // nothing to do
        }
        else {
          // Create new container entry for the base guid
          OMS_ContainerEntry *pBase = new(m_pContext) OMS_ContainerEntry (m_pContext, 
                                                                        pBaseClassEntry,
                                                                        base.containerId,
                                                                        schema,
                                                                        containerNo,
                                                                        useCachedKeys,
                                                                        partitionedKeys);
          Insert(pBase);

          // delete the entry in the directory
          Delete(pBaseContainerEntry, /*deleteEntry=*/false);
        }
      }
      else {
        // Up to now there is no local entry
            
        // now create new container entry
        OMS_ContainerEntry *pBase = new(m_pContext) OMS_ContainerEntry (m_pContext, 
                                                                      pBaseClassEntry,
                                                                      base.containerId,
                                                                      schema,
                                                                      containerNo,
                                                                      useCachedKeys,
                                                                      partitionedKeys);
        Insert(pBase);
      }
    }
    else if (error == e_OMS_sysinfo_not_found){ 
      ThrowUnknownContainer(baseGuid, schema, containerNo, __MY_FILE__, __LINE__);
    }
    else {
      char buf[256];
      sp77sprintf(buf, sizeof(buf), "AutoRegisterArrayContainer-1-: Guid: %d  Schema: %d  Container: %d",
        guid, schema, containerNo);
      OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
    }

    // Check if class entry of converted guid already exists locally and if not create one
    char className[256];
    sp77sprintf(className, sizeof(className), "%s(%d)", pBaseClassEntry->GetClassName(), category);
    OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().Find(guid);
    if (pClassEntry == NULL){
      pClassEntry = m_pContext->GetSession()->GetClassDir().RegisterClass (
                                &className[0], guid, baseGuid, OMS_KeyDesc(), 
                                arrayByteSize,
                                pBaseClassEntry->GetVTblPtr(),
                                /*isVarObject=*/false, /*isArrayObject=*/true);
    }

    // Create sub-container in the kernel
    tsp00_KnlIdentifier kernelClassName;
    OMS_Globals::MakeKnlIdentifier(className, kernelClassName);
    m_pContext->GetSession()->m_lcSink->CreateContainer(schema, &kernelGuid, 
                                          &kernelClassName,
                                          pClassEntry->GetPersistentSize(),  
                                          containerNo, 
                                          0,     //GetKeyDesc().GetPos(),
                                          0,     //GetKeyDesc().GetLen(),
                                          partitionedKeys,                                                                                                                          
                                          useCachedKeys, 
                                          false, //isVarObject,
                                          useInternalTrans,  
                                          false, //noWait
                                          (unsigned char*)&c.containerId, 
                                          &error);
    if (error != 0 && error != e_container_already_exists){
      char buf[256];
      sp77sprintf(buf, sizeof(buf), "AutoRegisterArrayContainer-2-: Guid: %d  Schema: %d  Container: %d",
        guid, schema, containerNo);
      OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
    }
    error = 0;

    // check whether there is already an (outdated) entry
    OMS_ContainerEntry *pOld = Find(guid, schema, containerNo, /*checkDropped=*/false);

    // Create new container entry for the converted guid
    pContainerEntry = new(m_pContext) OMS_ContainerEntry (m_pContext, 
                                                          pClassEntry,
                                                          c.containerId,
                                                          schema,
                                                          containerNo,
                                                          useCachedKeys,
                                                          partitionedKeys);
    Insert(pContainerEntry);

    // Insert before-image for subtrans rollback
    m_pContext->GetSession()->m_beforeImages.insertCreateContainerBeforeImage(
      &pContainerEntry->GetContainerHandle(),
      pContainerEntry,
      m_pContext->GetSession()->CurrentSubtransLevel());

    pContainerEntry->MarkCreated();

    if (pOld != NULL){
      // Append container entry of dropped container to the history chain of the
      // newly created container and delete the entry in the directory
      pContainerEntry->m_nextInHistory = pOld;
      Delete(pOld, /*deleteEntry=*/false);
    }
  }
  else {
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "AutoRegisterArrayContainer-3-: Guid: %d  Schema: %d  Container: %d",
      guid, schema, containerNo);
    OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
  }

  return pContainerEntry;
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::Insert (OMS_ContainerEntry *pContainerEntry)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Insert Container: " 
                                   << " GUID=" << pContainerEntry->GetGuid()
                                   << " Schema=" << pContainerEntry->GetSchema()
                                   << " CNo=" << pContainerEntry->GetContainerNo()
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));
#ifdef _ASSERT_OMS
  OMS_ContainerEntry *p = Find(pContainerEntry->GetGuid(), pContainerEntry->GetSchema(), pContainerEntry->GetContainerNo(), /*checkDropped=*/false);
  if (p != NULL && !p->IsDropped()){
    OMS_Globals::Throw(e_requested_dbperror, "Entry does already exist-1-", __MY_FILE__, __LINE__);
  }
  p = Find(pContainerEntry->GetContainerHandle(), /*checkDropFlag=*/false); 
  if (p != NULL && !p->IsDropped()){
    OMS_Globals::Throw(e_requested_dbperror, "Entry does already exist-2-", __MY_FILE__, __LINE__);
  }
#endif

  int hashSlot;
  hashSlot                               = (pContainerEntry->GetContainerHandle()) % m_headentries;
  pContainerEntry->m_pContHandleHashNext = m_ppContHandleHead[hashSlot];
  m_ppContHandleHead[hashSlot]           = pContainerEntry;

  hashSlot = HashValue(pContainerEntry->GetGuid(), 
                       pContainerEntry->GetSchema(), pContainerEntry->GetContainerNo(), 
                       m_headentries);
  pContainerEntry->m_pContIdHashNext = m_ppContIdHead[hashSlot];
  m_ppContIdHead[hashSlot]           = pContainerEntry;

  // Resize if more than twice as many containers are registered as there are hash slots.
  ++m_count;
  if (m_count > 2 * m_headentries){
    Resize();
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::Delete (OMS_ContainerEntry* pContainerEntry, bool deleteEntry) 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Delete Container: " 
                                   << " GUID=" << pContainerEntry->GetGuid()
                                   << " Schema=" << pContainerEntry->GetSchema()
                                   << " CNo=" << pContainerEntry->GetContainerNo()
                                   << " Delete=" << (deleteEntry ? "Y" : "N")
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // remove from ContainerHandle Hash 
  int hashSlot              = pContainerEntry->GetContainerHandle() % m_headentries;
  OMS_ContainerEntry **prev = &m_ppContHandleHead[hashSlot];
  OMS_ContainerEntry  *curr = *prev;
  while (curr != NULL) {
    if (curr == pContainerEntry) {
      --m_count;
      *prev = curr->m_pContHandleHashNext;
      break;
    }
    else {
      prev = &curr->m_pContHandleHashNext;
      curr = curr->m_pContHandleHashNext;
    }
  }

  // remove from Guid,Schema,ContainerNo Hash
  hashSlot = HashValue(pContainerEntry->GetGuid(), 
                       pContainerEntry->GetSchema(), 
                       pContainerEntry->GetContainerNo(), 
                       m_headentries);
  prev     = &m_ppContIdHead[hashSlot];
  curr     = *prev;
  while (curr != NULL) {
    if (curr == pContainerEntry) {
      *prev = curr->m_pContIdHashNext;
      break;
    }
    else {
      prev = &curr->m_pContIdHashNext;
      curr = curr->m_pContIdHashNext;
    }
  }

  if (deleteEntry){
    // delete local ContainerInfo
    pContainerEntry->DeleteSelf();
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::Resize()
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Resize Container Directory: " 
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerEntry**  ppOldContHandleHead = m_ppContHandleHead;
  OMS_ContainerEntry**  ppOldContIdHead     = m_ppContIdHead;
  //try
  //{
      int ix;
      // calculate new header size
      int newHeadEntries = m_count - 1;
      bool     isPrime   = false;
      while (!isPrime) {
          newHeadEntries++;
          isPrime = (1 == (newHeadEntries % 2)) ? true : false; 
          ix = 3;
          while (((ix * ix) <= newHeadEntries) && (isPrime)) {
              isPrime = (0 == (newHeadEntries % ix)) ? false : true;
              ix += 2;
          }
      }

      // allocate new hash arrays
      m_ppContIdHead     = NULL;  // Set value to null, so that in case of bad-alloc deallocate works correctly
      m_ppContHandleHead = reinterpret_cast<OMS_ContainerEntry**>(m_pContext->allocate(sizeof(OMS_ContainerEntry*) * newHeadEntries));
      m_ppContIdHead     = reinterpret_cast<OMS_ContainerEntry**>(m_pContext->allocate(sizeof(OMS_ContainerEntry*) * newHeadEntries)); 

      // Initialize hash arrays
      int oldHeadEntries = m_headentries;
      m_headentries      = newHeadEntries;
      for (ix = 0; ix < m_headentries; ++ix) {
          m_ppContHandleHead[ix]= NULL;
          m_ppContIdHead[ix]    = NULL;
      }

      // rehash 
      m_count = 0;
      for (ix = 0; ix < oldHeadEntries; ++ix)
      {
          OMS_ContainerEntry* pNext = ppOldContHandleHead[ix];
          while (pNext)
          {
              OMS_ContainerEntry* pCurr = pNext; 
              pNext = pNext->m_pContHandleHashNext;
              int hashSlot = (pCurr->GetContainerHandle()) % m_headentries;
              pCurr->m_pContHandleHashNext   = m_ppContHandleHead[hashSlot];
              m_ppContHandleHead[hashSlot]  = pCurr;

              hashSlot = HashValue(pCurr->GetGuid(), pCurr->GetSchema(), pCurr->GetContainerNo(), m_headentries);
              pCurr->m_pContIdHashNext = m_ppContIdHead[hashSlot];
              m_ppContIdHead[hashSlot] = pCurr;
          }
      }
      // free old headers
      m_pContext->deallocate(ppOldContHandleHead);
      m_pContext->deallocate(ppOldContIdHead);
  //}
  //catch (BAD_ALLOC_GEO573&)
  //{
  //  DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
  //  if (pCBInterface){
  //    pCBInterface->dbpCaughtBadAlloc();
  //  }

  //  // reset old state in case of missing memory
  //  m_pContext->deallocate(m_ppContHandleHead);
  //  m_ppContHandleHead = ppOldContHandleHead;

  //  m_pContext->deallocate(m_ppContIdHead);
  //  m_ppContIdHead  = ppOldContIdHead;
  //}
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::ThrowUnknownContainer(OMS_ContainerEntry *pContainerEntry, 
                                                   const char *pFile, unsigned int line) 
{
  this->ThrowUnknownContainer(pContainerEntry->GetGuid(), 
                              pContainerEntry->GetSchema(), 
                              pContainerEntry->GetContainerNo(),
                              pFile, line);
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::ThrowUnknownContainer(OMS_ContainerHandle &containerHandle,
                                                   const char *pFile, unsigned int line) 
{
  const int BUFSIZE = 256;
  const char msg[] = "Unknown container-handle: ";
  char buf[BUFSIZE];

  int i;
  for (i=0; i<sizeof(msg) && i < sizeof(buf); ++i){
    buf[i] = msg[i];
  }
  char *p = reinterpret_cast<char*>(&containerHandle);
  for (int j=0; j<sizeof(OMS_ContainerHandle) && i+1<sizeof(buf); ++j)
  {
    char s = p[j]>>4;  // High-Bits
    buf[i++] = s + (s>=10 ? 'A' : '0');
    s = p[j]&0xF;      // Low-Bits
    buf[i++] = s + (s>=10 ? 'A' : '0');
  }
  if (i<BUFSIZE) 
    buf[i]         = 0;
  else
    buf[BUFSIZE-1] = 0;

  OMS_Globals::Throw(e_unknown_guid, buf, pFile, line); 
}

/*----------------------------------------------------------------------*/


void OMS_ContainerDirectory::ThrowUnknownContainer(ClassID          guid, 
                                                   OmsSchemaHandle  schema, 
                                                   OmsContainerNo   containerNo,
                                                   const char      *pFile, 
                                                   unsigned int     line) 
{
  char buf[256];
  OmsTypeWyde         Identifier[OMS_MAX_SCHEMA_NAME_LENGTH];
  tgg00_BasisError    DBError;
  m_pContext->GetSession()->m_lcSink->GetSchemaName(schema, &Identifier[0], &DBError);
  if (0 != DBError){
    sp77sprintf (buf, sizeof(buf),
      "guid : %#8.8X, Schema(%d) CNo(%d)", guid, schema, containerNo);
  }
  else { 
    sp77sprintfUnicode(sp77encodingUTF8, buf, sizeof(buf), 
      "guid : %#8.8X, Schema(%d:'%S') CNo(%d)", guid, schema, &Identifier[0], containerNo);
  }
  OMS_Globals::Throw(e_unknown_guid, buf, pFile, line); 
}

/*----------------------------------------------------------------------*/
// PTS 1127661
void OMS_ContainerDirectory::ClearReusableOids() 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS ClearReusableOids: " 
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerDirectory::Iter containerIter = First();
  while (containerIter) {
    OMS_ContainerEntry* pCurrContainer = containerIter();
    pCurrContainer->ClearReusableOids();

    ++containerIter;
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::ClearNewVersionObjects()
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS ClearNewVersionObjects: " 
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerDirectory::Iter containerIter = First();
  while (containerIter) {
    OMS_ContainerEntry* pCurrContainer = containerIter();
    pCurrContainer->ClearNewVersionObjects();

    ++containerIter;
  }
}

/*----------------------------------------------------------------------*/

// PTS 1117571
// Clear structures which are used for cached keys and cache misses.
// As the structure for the cached keys is also used for those objects
// which are newly created in a version, delete only those objects, which
// do not belong to a version. 
void OMS_ContainerDirectory::ClearCachedKeys() 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS ClearCachedKeys: " 
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerEntry*     pCurrContainer = NULL; 
  OMS_ContainerDirectory::Iter containerIter = First();
  while (containerIter) {
    pCurrContainer = containerIter();
    if (pCurrContainer->UseCachedKeys() && pCurrContainer->GetClassEntry().GetKeyDesc().IsKeyed()) {
      // Reset structure for cache hits 
      if (!m_pContext->IsVersion()) {
        // As we are not in a version, objects which are created in a version
        // cannot exist. Therefore the complete structure can be reset
        pCurrContainer->VersionDelIndex(false);
      }
      else {
        // Delete only those entries which are not created in a version
        pCurrContainer->VersionDelIndex(true);
      }

      // Reset structure for cache misses if requested
      pCurrContainer->DropCacheMisses();
    }
    ++containerIter;
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::DeleteFreeList(int caller)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS DeleteFreeList: " 
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // Invalidate pointers in the class entries, which are pointing to the freelistheaders
  OMS_Session *pSession = m_pContext->GetSession(/*checkNULL=*/false);
  if (pSession != NULL){
    pSession->GetClassDir().CleanupAfterContextChange();
  }

  // Delete free list headers except sentinel.
  // The first entry (with object size 0) is kept.
  if (0 != m_pFreeListHeader){
    OMS_FreeListHeader* pDel = NULL;
    OMS_FreeListHeader* p    = m_pFreeListHeader->next;
    
    m_pFreeListHeader->Clear(m_pContext, caller);
    m_pFreeListHeader->next = NULL;

    while (p != NULL) {
      pDel = p;
      p    = p->next;
      pDel->DeleteSelf(m_pContext, caller);
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::ClearFreeList(int caller)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS ClearFreeList: " 
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));


  OMS_FreeListHeader* p = m_pFreeListHeader;
  while (p != NULL) {
    // Remove object frames 
    p->Clear(m_pContext, caller);  

    p = p->next;
  }
}

/*----------------------------------------------------------------------*/

OMS_FreeListHeader* OMS_ContainerDirectory::GetFreeList(size_t size) 
{
  // Look whether there is already an entry for the given size
  OMS_FreeListHeader* p = m_pFreeListHeader;
  while (p != NULL) {
    if (p->size == size) {
      return p;
    }
    else {
      p = p->next;
    }
  }

  // No entry was found, therefore create a new one and insert this new entry
  // behind the sentinel (object size 0) at second position in the list.
  p       = new(m_pContext) OMS_FreeListHeader(size);
  p->next = m_pFreeListHeader->next;
  m_pFreeListHeader->next = p;

  return p;
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::UnregisterAll()
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS UnregisterAll: " 
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // Delete entries from both container hashes
  OMS_ContainerEntry* pCurr;
  OMS_ContainerEntry* pDel;
  for (int ix = 0; ix < m_headentries; ix++) {
    pCurr = m_ppContHandleHead[ix];
    m_ppContHandleHead[ix] = NULL;
    m_ppContIdHead[ix]     = NULL;
    while (pCurr != NULL) {
      pDel   = pCurr;
      pCurr  = pCurr->m_pContHandleHashNext;
      pDel->DeleteSelf();
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::DropSchema (OmsSchemaHandle schema) 
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS DropSchema: " 
                                   << " Schema=" << schema
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  tgg00_BasisError DBError = e_ok;
  m_pContext->GetSession()->m_lcSink->DropSchema (schema, &DBError);
  if (DBError != e_ok){
    // if an error occurred, then the before state is rekonstructed by the kernel,
    // this means, that all containers exists, therefore no update of the local
    // directory is necessary.
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "Error while dropping schema: %d", schema);
    OMS_Globals::Throw(DBError, buf, __MY_FILE__, __LINE__); 
  }

  OMS_ContainerDirectory::Iter iter = First();
  while (iter) {
    OMS_ContainerEntry* pCurr = iter();
    ++iter;
    if (pCurr->GetSchema() == schema) {
      DropPhysicalContainer(pCurr->GetGuid(), 
                            pCurr->GetSchema(), 
                            pCurr->GetContainerNo(),
                            false);  // kernelUpdate
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::DropContainer (ClassID           guid, 
                                            OmsSchemaHandle   schema,
                                            OmsContainerNo    cno)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS DropContainer: " 
                                   << " GUID=" << guid
                                   << " Schema=" << schema
                                   << " CNo=" << cno
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // Do not allow direct dropping of the subcontainers of array-objects
  if (omsIsArrayGuid(guid)){
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "Not allowed to delete objects in subcontainer (GUID:%d, Schema:%d, Container:%d)",
      guid, schema, cno);
    OMS_Globals::Throw(e_missing_privilege, buf, __MY_FILE__, __LINE__); 
    return;
  }

  // Drop sub-container which belong to the same logical container
  OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(guid);
  if (pClassEntry->IsArrayObject()){
    OmsArrayObjectIteratorBase iter(*m_pContext->GetSession()->m_handleList.front(), guid, schema, cno);
    while (iter) {
      short category = iter.omsGetCategory();
      ++iter;

      OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "    SUB-Container=" << category);
      ClassID arrayGuid = omsMakeArrayGuid(guid, category);
      DropPhysicalContainer(arrayGuid, schema, cno);
    }
  }

  // Drop main container
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "    MAIN-Container");
  DropPhysicalContainer(guid, schema, cno);
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::DropPhysicalContainer (ClassID           guid, 
                                                    OmsSchemaHandle   schema,
                                                    OmsContainerNo    cno,
                                                    bool              kernelUpdate)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS DropPhysicalContainer: " 
                                   << " GUID=" << guid
                                   << " Schema=" << schema
                                   << " CNo=" << cno
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  if (kernelUpdate){
    tgg00_BasisError DBError = e_ok;
    // Delete container in the kernel
    KERNEL_GUID(kernelGuid, guid);
    m_pContext->GetSession()->m_lcSink->DropContainer (schema, kernelGuid, cno, DBError);
    if (DBError != e_ok && DBError != 100 /*row_not_found*/){
      // As the distributed local oms directories are not synchronized, it might happen, that
      // the container is already dropped from a concurrent running transaction. Therefore
      // ignore error 100 and only update local information in this case. (PTS 1134828)
      //
      // If an error has occurred in the kernel , then the before state is rekonstructed during
      // the rollback (the container exists again). Therefore no update of the local oms-directory 
      // is necessary.
      char buf[256];
      ClassID baseGuid = guid;
      short category = -1;
      if (omsIsArrayGuid(guid)){
        omsSplitArrayGuid(guid, baseGuid, category);
      }
      sp77sprintf(buf, sizeof(buf), "Error when dropping container: GUID:%d, Schema:%d, Container:%d, Category=%d",
        baseGuid, schema, cno, category);
      OMS_Globals::Throw(DBError, buf, __MY_FILE__, __LINE__); 
    }
  }

  // Mark local entry as deleted 
  OMS_ContainerEntry* p = Find(guid, schema, cno);
  if (NULL != p) {
    // Insert before-image for subtrans rollback
    m_pContext->GetSession()->m_beforeImages.insertDropContainerBeforeImage(
        &p->GetContainerHandle(),
        p,
        m_pContext->GetSession()->CurrentSubtransLevel());
    p->MarkDropped();

    m_pContext->ChangeNewObjectsToFlush(-1 * p->GetNewObjectsToFlush());
  }
}



/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::DeleteAllObjects (ClassID           guid, 
                                               OmsSchemaHandle   schema,
                                               OmsContainerNo    cno)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS DeleteAllObjects: " 
                                   << " GUID=" << guid
                                   << " Schema=" << schema
                                   << " CNo=" << cno
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  tgg00_BasisError DBError = e_ok;

  // Do not allow direct deletion of the objects in the subcontainers of array-objects
  if (omsIsArrayGuid(guid)){
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "Not allowed to delete objects in subcontainer (GUID:%d, Schema:%d, Container:%d)",
      guid, schema, cno);
    OMS_Globals::Throw(e_missing_privilege, buf, __MY_FILE__, __LINE__);
    return;
  }

  // Delete the entries in the sub-container which belong to the same logical container
  OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(guid);
  if (pClassEntry->IsArrayObject()){
    OmsArrayObjectIteratorBase iter(*m_pContext->GetSession()->m_handleList.front(), guid, schema, cno);
    while (iter) {
      short category = iter.omsGetCategory();
      ++iter;

      OMS_TRACE (omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "    SUB-Container=" << category);
      ClassID arrayGuid = omsMakeArrayGuid(guid, category);
      m_pContext->GetSession()->DeleteAll (arrayGuid, schema, cno);
    }
  }

  // Delete objects in the main container
  OMS_TRACE (omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "    MAIN-Container");
  m_pContext->GetSession()->DeleteAll (guid, schema, cno);
}


/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::RollbackDropContainer(OMS_ContainerHandle *pContainerHandle)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Rollback DropContainer: "
                  << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerEntry *p = Find(*pContainerHandle, /*checkDropFlag=*/false); 
  if (NULL != p) {
    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, 
                    "   GUID=" << p->GetGuid()
                    << " Schema=" << p->GetSchema()
                    << " CNo=" << p->GetContainerNo());
    
    // Reset drop flag
    p->ResetDropped();
    m_pContext->ChangeNewObjectsToFlush(p->GetNewObjectsToFlush());
  }
  else {
    //char buf[256];
    //sp77sprintf(buf, sizeof(buf), "RollbackDropContainer: ContainerEntry not found");
    //OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::RollbackCreateContainer(OMS_ContainerHandle *pContainerHandle)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Rollback CreateContainer: "
                  << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerEntry *p1 = Find(*pContainerHandle, /*checkDropFlag=*/false); 
  if (NULL != p1) {
    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, 
                    "   GUID=" << p1->GetGuid()
                    << " Schema=" << p1->GetSchema()
                    << " CNo=" << p1->GetContainerNo());

    m_pContext->ChangeNewObjectsToFlush(-1 * p1->GetNewObjectsToFlush());

    // Get the next entry in the history
    OMS_ContainerEntry *p2 = p1->m_nextInHistory;

    // Delete old entry from the directory and the object itself
    Delete(p1);

    if (NULL != p2){
      // If history is not empty, then insert next oldest entry
      Insert(p2);
    }
  }
  else {
    //char buf[256];
    //sp77sprintf(buf, sizeof(buf), "RollbackCreateContainer: ContainerEntry not found");
    //OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::CommitDropContainer(OMS_ContainerHandle *pContainerHandle)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Commit DropContainer: "
                  << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerEntry *p1 = Find(*pContainerHandle, /*checkDropFlag=*/false); 
  if (NULL != p1) {
    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, 
                    "   GUID=" << p1->GetGuid()
                    << " Schema=" << p1->GetSchema()
                    << " CNo=" << p1->GetContainerNo());
  }
  else {
    //char buf[256];
    //sp77sprintf(buf, sizeof(buf), "CommitDropContainer: ContainerEntry not found");
    //OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
  }
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::CommitCreateContainer(OMS_ContainerHandle *pContainerHandle)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Commit CreateContainer: "
                  << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerEntry *p1 = Find(*pContainerHandle, /*checkDropFlag=*/false); 
  if (NULL != p1) {
    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, 
                    "   GUID=" << p1->GetGuid()
                    << " Schema=" << p1->GetSchema()
                    << " CNo=" << p1->GetContainerNo());
  }
  else {
    //char buf[256];
    //sp77sprintf(buf, sizeof(buf), "CommitDropContainer: ContainerEntry not found");
    //OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
  }
}

/*----------------------------------------------------------------------*/

bool OMS_ContainerDirectory::ExistsContainer (ClassID           guid, 
                                              OmsSchemaHandle   schema,
                                              OmsContainerNo    cno)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS Exists Container: "
                  << " GUID=" << guid
                  << " Schema=" << schema
                  << " CNo=" << cno
                  << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  // Determine corresponding class entry
  OMS_ClassEntry *pClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(guid);

  // Check whether container exists in the kernel
  bool  useCachedKeys   = false;
  bool  partitionedKeys = false;
  short error           = e_ok;
  union {
    // ensure, that containerId is aligned correctly. Otherwise the
    // reinterpret_cast to OMS_ContainerHandle will fail with bus-error
    void*             align;  
    tgg01_ContainerId containerId;
  } c;
  KERNEL_GUID(kernelGuid, guid);
  m_pContext->GetSession()->m_lcSink->ExistsContainer(schema, kernelGuid, cno, 
                                        pClassEntry->GetPersistentSize(),
                                        pClassEntry->GetKeyDesc().GetLen(),
                                        c.containerId, useCachedKeys, partitionedKeys, error);

  // As there is no explicit synchronization of the local container directory and the kernel
  // directory, check whether the directories are still in sync concerning the current entry. 
  OMS_ContainerEntry *pOld = Find(guid, schema, cno, /*checkDropFlag=*/false);
  if (error == e_ok){
    // The container exists in the kernel
    const OMS_ContainerHandle *pContainerHandle = reinterpret_cast<const OMS_ContainerHandle*>(&c.containerId.fileDirFileId_gg00());
    if (pOld != NULL && *pContainerHandle != pOld->GetContainerHandle()){
      // The locally cached entry is out-dated
      OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "   => Local container entry is dropped: " 
        << pOld->GetContainerHandle());
      
      if (pOld->GetNewObjectsToFlush() != 0){
        char buf[256];
        sp77sprintf(buf, sizeof(buf), "Local container entry (handle=%d) is out of sync, but contains new objects: %d",
                    pOld->GetContainerHandle().GetUint8(), pOld->GetNewObjectsToFlush());
        OMS_Globals::Throw(e_unknown_guid, buf, __MY_FILE__, __LINE__); 
      }

      Delete(pOld);
    }

    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "   => Container exists: " 
      << (reinterpret_cast<const OMS_ContainerHandle*>(&c.containerId.fileDirFileId_gg00())));

    return true;
  }
  else if (error == e_OMS_sysinfo_not_found){
    // The container does not exist in the kernel
    if (pOld != NULL && !pOld->IsDropped()){
      // If the container is not already marked as dropped, then delete it.
      OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "   => Local container entry is dropped : "
        << pOld->GetContainerHandle());
      
      if (pOld->GetNewObjectsToFlush() != 0){
        char buf[256];
        sp77sprintf(buf, sizeof(buf), "Local container entry (handle=%d) is out of sync, but contains new objects: %d",
                    pOld->GetContainerHandle().GetUint8(), pOld->GetNewObjectsToFlush());
        OMS_Globals::Throw(e_unknown_guid, buf, __MY_FILE__, __LINE__); 
      }
      
      Delete(pOld);
    }

    OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "   => Container does not exists");
    
    return false;
  }
  else {
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "Error when checking for existance: Guid %d, Schema: %d, CNo: %d",
      guid, schema, cno);
    OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__, m_pContext->GetSession());
    return false;
  }
}

/*----------------------------------------------------------------------*/

bool OMS_ContainerDirectory::ExistsSubContainer (ClassID           guid, 
                                                 OmsSchemaHandle   schema,
                                                 OmsContainerNo    cno,
                                                 short             category)
{
  ClassID arrayGuid = omsMakeArrayGuid(guid, category);
  OMS_ContainerEntry *pContainerEntry = Find(arrayGuid, schema, cno, /*checkDropped=*/false);
  if (pContainerEntry != NULL){
    // Entry is already in local directory. Check if dropped.
    return !pContainerEntry->IsDropped();
  }
  else {
    // Read information from kernel
    tgg00_BasisError  DBError;
    union {
      // ensure, that containerId is aligned correctly. Otherwise the
      // reinterpret_cast to OMS_ContainerHandle will fail with bus-error
      void*             align;  
      tgg01_ContainerId containerId;
    } c;
    size_t objectSize      = 0;
    bool   useCachedKeys   = false;
    bool   partitionedKeys = false;
    KERNEL_GUID(kernelGuid, arrayGuid);
    m_pContext->GetSession()->m_lcSink->ExistsContainer (schema, kernelGuid, cno, /*objectSize=*/0, /*keyLen=*/0,
                                           c.containerId, useCachedKeys, partitionedKeys, DBError);
    if (0 == DBError){
      return true;
    }
    else {
      if (e_OMS_sysinfo_not_found != DBError) {
        OMS_Globals::Throw(DBError, "omsExistsContainer", __MY_FILE__, __LINE__); 
      }
      return false;
    }
  }
}

/*----------------------------------------------------------------------*/

OmsObjectContainer* OMS_ContainerDirectory::GetMemory(size_t size) 
{
  size += (size_t)OmsObjectContainer::headerSize();

  OMS_TRACE(omsTrMemory, m_pContext->GetSession()->m_lcSink, "OMS GetMemory in ContainerDir: " 
                                   << " Size=" << (long)size
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  bool reuseFrame = true;

  // First check whether there is an unused frame in the freelist
  OmsObjectContainer *p = GetFreeList(size)->GetFrame();

  if (NULL == p) { 
    reuseFrame = false;
    // Now allocate new frame
    if (m_pContext->IsVersion()) {
      p = (OmsObjectContainerPtr) m_pContext->allocate(size);  
    }
    else {
      p = (OmsObjectContainerPtr) m_pContext->StackHeapMalloc(size);
    }
  }

  if (p != NULL){
    p->Init();
    p->MarkHeaderOnly();
  }

  OMS_TRACE(omsTrMemory, m_pContext->GetSession()->m_lcSink, "    => Object="  << (void*)p 
           << (reuseFrame?" Reused":" New")); 

  return p;
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::ChainFree(OmsObjectContainer *&pObj, size_t size, int caller) 
{
  size += (size_t)OmsObjectContainer::headerSize();

  OMS_TRACE(omsTrMemory, m_pContext->GetSession()->m_lcSink, "OMS ChainFree in ContainerDir: " 
                                   << " Object=" << (void*)pObj
                                   << " Size=" << (long)size
                                   << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  if (pObj == NULL){
    return;
  }

#ifdef USE_SYSTEM_ALLOC_CO13
  m_pContext->deallocate(pObj);
#else

  OMS_FreeListHeader *pFreeHead = GetFreeList(size); 
//#ifdef _ASSERT_OMS
  pObj->InitializeForFreeList(caller);
//#endif
  pFreeHead->Insert(pObj);
#endif
  pObj = NULL;
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::CleanupAfterTransEnd(bool isCommit)
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS CleanupAfterTransEnd: "
                  << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerDirectory::Iter iter = First();
  while (iter) {
    OMS_ContainerEntry* pCurr = iter();
    ++iter;

    // Reset 
    pCurr->ResetLocked();

    // Delete all entries of the history
    OMS_ContainerEntry *p = pCurr->m_nextInHistory;
    while (p != NULL){
      OMS_ContainerEntry *next = p->m_nextInHistory;
      p->DeleteSelf();
      p = next;
    }
    pCurr->m_nextInHistory = NULL;

    if(m_pContext->IsVersion()){
      pCurr->ResetClassEntry();
    }

    if (  pCurr->IsDropped() &&  isCommit   
       || pCurr->IsCreated() && !isCommit){
      Delete(pCurr);
    }
    else if (pCurr->IsDropped() && !isCommit){
      pCurr->ResetDropped();
    }
    else if (pCurr->IsCreated() && isCommit){
      pCurr->ResetCreated();
    }
  }

  ForbitChangeOfClassEntryPtr();

  ClearReusableOids();
  ClearFreeList(2);
}

/*----------------------------------------------------------------------*/

void OMS_ContainerDirectory::ResetNewObjectsToFlush()
{
  OMS_TRACE(omsTrContainerDir, m_pContext->GetSession()->m_lcSink, "OMS ResetNewObjectsToFlush: "
                  << " Context=" << OMS_CharBuffer(&m_pContext->GetVersionId()[0], sizeof(OmsVersionId)));

  OMS_ContainerDirectory::Iter iter = First();
  while (iter) {
    OMS_ContainerEntry* pCurr = iter();
    ++iter;
    pCurr->ResetNewObjectsToFlush();
  }
}

/*----------------------------------------------------------------------*/

bool OMS_ContainerDirectory::CheckForUnregisteredClasses(OMS_Session *pSession)
{
  OMS_ContainerDirectory::Iter iter = First();
  while (iter) {
    OMS_ContainerEntry* pCurr = iter();
    ++iter;

    if (pSession->GetClassDir().Find(pCurr->GetGuid()) == NULL){
      return true;
    }
  }

  return false;
}

/*----------------------------------------------------------------------*/

