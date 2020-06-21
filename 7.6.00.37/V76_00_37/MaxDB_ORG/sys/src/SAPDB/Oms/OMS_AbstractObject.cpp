/*!
 * \file    OMS_AbstractObject.cpp
 * \author  MarkusSi, Roterring
 * \brief   Defines Functions used by SQL-Class and OMS
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

#include "Oms/OMS_AbstractObject.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Trace.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_ContainerEntry.hpp"
#include "Oms/OMS_ClassEntry.hpp"
#include "Oms/OMS_KeyDesc.hpp"

#define FOR_UPD_CO10       true


static inline OmsObjectContainerPtr omsGetContainer(const OmsAbstractObject* pObj)
{
  if (NULL == pObj) 
    OMS_Globals::Throw(e_nil_pointer, "omsGetContainer", __MY_FILE__, __LINE__);

  return (OmsObjectContainerPtr) ((unsigned char*) pObj - OmsObjectContainer::headerSize());
}

//
// Implementation of OmsAbstractObject
//

OmsAbstractObject::OmsAbstractObject() {}

/*----------------------------------------------------------------------*/

OmsObjByClsIterBase OmsAbstractObject::omsAllOids(OmsHandle       &h, 
                                                  ClassID guid, 
                                                  OmsSchemaHandle  Schema, 
                                                  OmsContainerNo   ContainerNo,
                                                  int              maxBufferSize)
{
  OMS_TRACE(omsTrInterface, h.m_pSession->m_lcSink, "OmsAbstractObject::omsAllOids : " << guid << "," << Schema << "," << ContainerNo); 
  return h.omsAllOids (guid, Schema, ContainerNo, maxBufferSize);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsBinaryToKey(void* pSource, void* pKey) const
{
  OMS_Globals::Throw(e_OMS_not_implemented, "OmsAbstractObject::omsBinaryToKey", __MY_FILE__, __LINE__);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsCleanUp (OmsHandle&) {}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsDelete (OmsHandle& Handle)
{
  OMS_CHECK_EXCEPTION(Handle);

  const char* msg = "OmsAbstractObject::omsDelete";

  Handle.m_pSession->AssertNotReadOnly(msg);
  Handle.m_pSession->IncDelete ();
  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  OMS_TRACE(omsTrInterface, Handle.m_pSession->m_lcSink, msg << " : " << pObjContainer->m_oid);

  // Check that container is not dropped already  PTS 1127338
  OMS_ContainerEntry *pInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pInfo->IsDropped()){
    Handle.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  if (pObjContainer->DeletedFlag()) {
    Handle.m_pSession->ThrowDBError (e_object_not_found, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }
  if (!Handle.m_pSession->IsLocked(pObjContainer, /*shared=*/false))
  {
    Handle.m_pSession->ThrowDBError (e_object_not_locked, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }
  omsCleanUp (Handle);
  pObjContainer->MarkDeleted();

  // PTS 1127661
  // Insert oid of deleted object into container-specific reuse structure. 
  if (!Handle.m_pSession->InVersion()){
    if (!pInfo->GetClassEntry().IsKeyedObject()){
      pInfo->InsertReusableOid(pObjContainer->m_oid); 
    }
  }
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsDeleteAll(OmsHandle& h, ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo) 
{
  const char* msg = "OmsAbstractObject::omsDeleteAll";

  OMS_TRACE(omsTrInterface, h.m_pSession->m_lcSink, msg << " : "
    << guid << "," << Schema << "," << ContainerNo);
  h.omsDeleteAll (guid, Schema, ContainerNo);
}

/*----------------------------------------------------------------------*/

const OmsAbstractObject* OmsAbstractObject::omsDeRef (const OmsObjectId& oid, OmsHandle& Handle, ClassID guid)
{
  const char *msg = "OmsAbstractObject::omsDeRef";

  OMS_TRACE(omsTrInterface, Handle.m_pSession->m_lcSink, "OmsAbstractObject::omsDeRef : " << oid << ", " << guid); 
  OMS_CHECK_EXCEPTION(Handle);

  if (!oid.isTrue()) {
    return NULL;
  }
  else {
    ALIGN_OID_CONST (oid8, oid);

    return REINTERPRET_CAST(OmsAbstractObject*, Handle.m_pSession->DeRef (oid8, guid, !FOR_UPD_CO10, /*doLock=*/false, /*shared=*/false));
  }
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsAbstractObject::omsDeRefForUpd (const OmsObjectId& oid, 
                                                      OmsHandle& Handle, 
                                                      ClassID guid, 
                                                      bool doLock, 
                                                      bool shared)
{
  const char *msg = "OmsAbstractObject::omsDeRefForUpd";

  OMS_TRACE(omsTrInterface, Handle.m_pSession->m_lcSink, "OmsAbstractObject::omsDeRefForUpd : " << oid 
    << ", " << guid << ", doLock : " << doLock); 
  OMS_CHECK_EXCEPTION(Handle);


  if (oid.isTrue()) {
    ALIGN_OID_CONST (oid8, oid);

    return REINTERPRET_CAST(OmsAbstractObject*, Handle.m_pSession->DeRef(oid8, guid, FOR_UPD_CO10, doLock, shared));
  }
  else {
    return NULL;
  }
}

/*----------------------------------------------------------------------*/

const OmsAbstractObject* OmsAbstractObject::omsDeRefKey (unsigned char  * key,
                                                         OmsHandle      &Handle, 
                                                         ClassID         guid, 
                                                         OmsSchemaHandle h,
                                                         OmsContainerNo  ContainerNo) 
{
  OMS_TRACE(omsTrInterface, Handle.m_pSession->m_lcSink, "OmsAbstractObject::omsDeRefKey : " << guid
    << "CNo : " << ContainerNo << " Key : " << OMS_UnsignedCharBuffer(key, Handle.m_pSession->GetClassDir().GetClassEntry(guid)->GetKeyDesc().GetLen()));
  OMS_CHECK_EXCEPTION(Handle);
  return REINTERPRET_CAST(OmsAbstractObject*, Handle.m_pSession->DeRefViaKey(key, guid, !FOR_UPD_CO10, /*doLock=*/false, /*shared=*/false, h, ContainerNo));
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsAbstractObject::omsDeRefKeyForUpd (unsigned char*  key,
                                                         OmsHandle&      Handle, 
                                                         ClassID guid, 
                                                         bool            doLock,
                                                         OmsSchemaHandle h,
                                                         OmsContainerNo  ContainerNo,
                                                         bool            shared)
{
  OMS_TRACE(omsTrInterface, Handle.m_pSession->m_lcSink, "OmsAbstractObject::omsDeRefKeyForUpd : " << guid
    << "CNo : " << ContainerNo << " Key : " << OMS_UnsignedCharBuffer(key, Handle.m_pSession->GetClassDir().GetClassEntry(guid)->GetKeyDesc().GetLen())
    << " doLock : " << doLock);
  OMS_CHECK_EXCEPTION(Handle);

  return REINTERPRET_CAST(OmsAbstractObject*, Handle.m_pSession->DeRefViaKey(key, guid, FOR_UPD_CO10, doLock, shared, h, ContainerNo));
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsDump (OmsHandle& Handle, const char* flags) const
{
  OMS_ObjectId8 oid = omsOid();
  Handle.omsTracef ("OID : %d.%d (vers %d) %s", 
    oid.getPno(), oid.getPagePos(), oid.getGeneration(), flags); 
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsFlush (OmsHandle& Handle)
{
  // If memory should be freed, then use omsRelease.
  // It does not make sense to trigger flush explicitly. => NOP-operation

  //const char* msg = "OmsAbstractObject::omsFlush";

  //OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  //// Check that container is not dropped already  PTS 1127338
  //OMS_ContainerEntry *pInfo = pObjContainer->GetContainerInfoNoCheck();
  //if (pInfo->IsDropped()){
  //  Handle.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  //}

  //Handle.m_pSession->m_context->FlushObj(pObjContainer);
}

/*----------------------------------------------------------------------*/

const OmsObjectId OmsAbstractObject::omsGetOid() const
{
  return reinterpret_cast<const OmsObjectId&>(omsOid());
}

/*----------------------------------------------------------------------*/

unsigned int OmsAbstractObject::omsGetObjectRevisionNumber() const
{
  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);
  
  return pObjContainer->GetRevisionNumber();
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsAbstractObject::omsForUpdPtr(OmsHandle& h, bool doLock) const
{
  const char* msg = "OmsAbstractObject::omsForUpdPtr";

  OMS_CHECK_EXCEPTION(h);

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  h.m_pSession->AssertNotReadOnly(msg); 

  // Check that container is not dropped already // PTS 1127338
  OMS_ContainerEntry *pInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pInfo->IsDropped()){
    h.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  if (doLock) {
    if (!(pObjContainer->LockedFlag()) && (!h.m_pSession->InVersion())) {
      ((OmsAbstractObject*) this)->omsLock(h);
    }
  }
  h.m_pSession->InsertBeforeImage (pObjContainer);
  return (OmsAbstractObject*) this;
}

/*----------------------------------------------------------------------*/

OmsSchemaHandle OmsAbstractObject::omsGetSchemaHandle(OmsHandle& Handle) const
{
  const char* msg = "OmsAbstractObject::omsGetSchemaHandle";

  OMS_CHECK_EXCEPTION(Handle);

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  // Check that container is not dropped already  PTS 1127338
  OMS_ContainerEntry *pContainerInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    Handle.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }  
  
  return pContainerInfo->GetSchema();
}

/*----------------------------------------------------------------------*/

bool OmsAbstractObject::omsHistoryInUse(OmsHandle& h) const
{
  // PTS 1130354 FF
	OmsTypeUInt8  OldestTransNo;
	return omsHistoryInUse(h, false /* ignoreVersions */, OldestTransNo);
}

/*----------------------------------------------------------------------*/
// *** new ***  PTS 1130354 FF
bool OmsAbstractObject::omsHistoryInUse(OmsHandle& h, bool ignoreVersions, OmsTypeUInt8& OldestTransNo) const
{
	const char* msg = "OmsAbstractObject::omsHistoryInUse";

	OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

	OMS_TRACE(omsTrInterface, h.m_pSession->m_lcSink, "OmsAbstractObject::omsHistoryInUse : " 
		<< pObjContainer->m_oid << "ignoreVers :" << ignoreVersions);
	OMS_CHECK_EXCEPTION(h);

	// Check that container is not dropped already  PTS 1127338
	OMS_ContainerEntry *pInfo = pObjContainer->GetContainerInfoNoCheck();
	if (pInfo->IsDropped()){
		h.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
	}

  return h.m_pSession->HistoryInUse(pObjContainer->m_oid, ignoreVersions, OldestTransNo);
}

/*----------------------------------------------------------------------*/

bool OmsAbstractObject::omsIsLocked (OmsHandle& Handle, bool shared) const
{
  const char* msg = "OmsAbstractObject::omsIsLocked";

  _TRACE_METHOD_ENTRY(&Handle,"OmsAbstractObject::omsIsLocked");

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  // Check that container is not dropped already  PTS 1127338
  OMS_ContainerEntry *pInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pInfo->IsDropped()){
    Handle.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  return Handle.m_pSession->IsLocked(pObjContainer, shared);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsKey(OmsHandle& Handle, unsigned char* key) const
{
  const char *msg = "OmsAbstractObject::omsKey";

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  // Check that container is not dropped already // PTS 1127338
  OMS_ContainerEntry *pContainerInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    Handle.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  this->omsBinaryToKey(pContainerInfo->GetClassEntry().GetKeyPtr(pObjContainer), key);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsKeyToBinary(const void* pKey, void* pDest) const
{
  OMS_Globals::Throw(e_OMS_not_implemented, "OmsAbstractObject::omsKeyToBinary", __MY_FILE__, __LINE__);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsLock (OmsHandle& h)
{
  OMS_CHECK_EXCEPTION(h);

  const char* msg = "OmsAbstractObject::omsLock"; 

  h.m_pSession->AssertNotReadOnly(msg);  

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  OMS_TRACE(omsTrInterface, h.m_pSession->m_lcSink, msg << " : " << pObjContainer->m_oid);

  // Check that container is not dropped already // PTS 1127338
  OMS_ContainerEntry *pContainerInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    h.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  if ((pObjContainer->LockedFlag()) || (h.m_pSession->InVersion())) {
    // instance already locked
    return;
  }
  h.m_pSession->LockObj(pObjContainer);
}

/*----------------------------------------------------------------------*/

bool OmsAbstractObject::omsTryLock (OmsHandle& h)
{
  OMS_CHECK_EXCEPTION(h);

  const char* msg = "OmsAbstractObject::omsTryLock"; 

  h.m_pSession->AssertNotReadOnly(msg);  

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  OMS_TRACE(omsTrInterface, h.m_pSession->m_lcSink, msg << " : " << pObjContainer->m_oid);

  // Check that container is not dropped already // PTS 1127338
  OMS_ContainerEntry *pContainerInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    h.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  if ((pObjContainer->LockedFlag()) || (h.m_pSession->InVersion())) {
    // instance already locked
    return true;
  }
  return h.m_pSession->TryLockObj(pObjContainer, /*shared=*/false);
}

/*----------------------------------------------------------------------*/

bool OmsAbstractObject::omsTryLockShared (OmsHandle& h)
{
  OMS_CHECK_EXCEPTION(h);

  const char* msg = "OmsAbstractObject::omsTryLockShared"; 

  h.m_pSession->AssertNotReadOnly(msg);  

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  OMS_TRACE(omsTrInterface, h.m_pSession->m_lcSink, msg << " : " << pObjContainer->m_oid);

  // Check that container is not dropped already // PTS 1127338
  OMS_ContainerEntry *pContainerInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    h.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  if ((pObjContainer->LockedSharedFlag()) || (h.m_pSession->InVersion())) {
    // instance already locked
    return true;
  }
  return h.m_pSession->TryLockObj(pObjContainer, /*shared=*/true);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsLockShared (OmsHandle& h) const
{
  OMS_CHECK_EXCEPTION(h);

  const char* msg = "OmsAbstractObject::omsLockShared"; 

  //h.m_pSession->AssertNotReadOnly(msg);  

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  OMS_TRACE(omsTrInterface, h.m_pSession->m_lcSink, msg << " : " << pObjContainer->m_oid);

  // Check that container is not dropped already // PTS 1127338
  OMS_ContainerEntry *pContainerInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    h.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  if (pObjContainer->LockedSharedFlag() || h.m_pSession->InVersion()) {
    // instance already shared locked
    return;
  }
  h.m_pSession->LockSharedObj(pObjContainer);
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsAbstractObject::omsNewObject (OmsHandle      &handle, 
                                                    ClassID         guid, 
                                                    OmsSchemaHandle schema, 
                                                    OmsContainerNo  containerNo,
                                                    size_t          arrayByteSize)
{
  OMS_TRACE(omsTrInterface, handle.m_pSession->m_lcSink, "OmsAbstractObject::omsNewObject : " << guid 
    << " Schema : " << schema << " CNo : " << containerNo);

  return handle.omsNewObject ( guid, schema, containerNo, arrayByteSize ); 
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsAbstractObject::omsNewRegistryObject(size_t sz, OmsHandle& Handle, ClassID guid)
{
  return Handle.omsNewRegistryObject (sz, guid);
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsAbstractObject::omsNewKeyedObject (OmsHandle           &handle, 
                                                         ClassID              guid, 
                                                         const unsigned char *key,
                                                         OmsSchemaHandle      schema,
                                                         OmsContainerNo       containerNo )
{
  OMS_TRACE(omsTrInterface, handle.m_pSession->m_lcSink, "OmsAbstractObject::omsNewKeyedObject : " << guid
    <<  " Schema : " << schema << " CNo : " << containerNo << " Key : " 
    << OMS_UnsignedCharBuffer(key, handle.m_pSession->GetClassDir().GetClassEntry(guid)->GetKeyDesc().GetLen()));

  return handle.omsNewKeyedObject ( guid, key, schema, containerNo ); 
}

/*----------------------------------------------------------------------*/

const OMS_ObjectId8& OmsAbstractObject::omsOid() const
{
  return omsGetContainer(this)->m_oid;
}

/*----------------------------------------------------------------------*/

// Old interface, which registers the class info AND creates a container.
// Should not be used any more.
void OmsAbstractObject::omsRegClass (OmsHandle         &handle,
                                     ClassID            guid, 
                                     const char        *pClassName, 
                                     size_t             persistentSize, 
                                     size_t             objectSize,
                                     ClassID            baseClass,
                                     OmsAbstractObject *pObj,
                                     OmsSchemaHandle    schema,
                                     OmsContainerNo     containerNo,
                                     size_t             arrayByteSize) 
{
  OmsAbstractObject::omsRegClass(handle, guid, pClassName, objectSize, /*keySize=*/0, 
                                 baseClass, pObj);
  handle.omsCreateContainer(guid, schema, containerNo, /*useCachedKeys=*/false, /*partitionedKey=*/false);
}

/*----------------------------------------------------------------------*/

// Old interface, which registers the class info AND creates a container.
// Should not be used any more.
void OmsAbstractObject::omsRegClassAndKey (OmsHandle&         handle,
                                           ClassID            guid,
                                           const char        *pClassName, 
                                           size_t             persistentSize, 
                                           size_t             keySize, 
                                           size_t             objectSize,
                                           ClassID            baseClass, 
                                           OmsAbstractObject *pObj,
                                           OmsSchemaHandle    schema,
                                           OmsContainerNo     containerNo,
                                           bool               partitionedKey)
{
  OmsAbstractObject::omsRegClass(handle,guid, pClassName, objectSize, keySize, 
                                 baseClass, pObj);
  handle.omsCreateContainer(guid, schema, containerNo, /*useCachedKeys=*/false, partitionedKey);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsRegClass(OmsHandle         &handle,
                                    ClassID            guid,
                                    const char        *pClassName,
                                    size_t             objectSize,
                                    size_t             keySize,
                                    ClassID            baseGuid,
                                    OmsAbstractObject *pObj)
{
  size_t keyPos = (keySize != 0 
                  ? objectSize + 1 - sizeof(void*)/*vtbl-pointer*/ 
                  : 0);
  handle.omsRegClass(guid, pClassName, objectSize, OMS_KeyDesc(keyPos,keySize),
                     baseGuid, pObj);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsRegArrayClass(OmsHandle         &handle,
                                         ClassID            guid,
                                         const char        *pClassName,
                                         size_t             objectSize,
                                         OmsAbstractObject *pObj)
{
  handle.omsRegArrayClass(guid, pClassName, objectSize, pObj);
}


/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsRelease(OmsHandle& Handle)
{
  const char* msg = "OmsAbstractObject::omsRelease";

  OMS_TRACE(omsTrInterface, Handle.m_pSession->m_lcSink, "OmsAbstractObject::omsRelease : " << omsGetContainer(this)->m_oid);
  
  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  // Check that container is not dropped already  PTS 1127338
  OMS_ContainerEntry *pInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pInfo->IsDropped()){
    Handle.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  Handle.m_pSession->ReleaseObj(pObjContainer);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsStore (OmsHandle& Handle)
{
  OMS_TRACE(omsTrInterface, Handle.m_pSession->m_lcSink, "OmsAbstractObject::omsStore : " << omsGetContainer(this)->m_oid);
  OMS_CHECK_EXCEPTION(Handle);

  const char* msg = "OmsAbstractObject::omsStore"; 

  Handle.m_pSession->AssertNotReadOnly(msg);  

  Handle.m_pSession->IncStore();
  // omsVerify(Handle); PTS 1121779

  OmsObjectContainerPtr pObjContainer = omsGetContainer(this);

  // Check that container is not dropped already // PTS 1127338
  OMS_ContainerEntry *pContainerInfo = pObjContainer->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    Handle.m_pSession->ThrowDBError(e_container_dropped, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }

  _TRACE_OID(&Handle, pObjContainer->m_oid);
  if (!Handle.m_pSession->IsLocked(pObjContainer, /*shared=*/false))
  {
    Handle.m_pSession->ThrowDBError (e_object_not_locked, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }
  if (!(pObjContainer->existBeforeImage (Handle.m_pSession->CurrentSubtransLevel(), Handle.m_pSession->InVersion()))) {
    Handle.m_pSession->ThrowDBError (e_missing_before_image, msg, pObjContainer->m_oid, __MY_FILE__, __LINE__);
  }
  else {
    pObjContainer->MarkStored();
  }
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsVerify (OmsHandle& h) const {}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsSimDumpToHtmlShort(OmsHandle &Handle, OmsSimDumpCollector &str) const
{
  Handle.m_pSession->m_lcSink->SimCtlDumpToHtml(Handle, (SAPDB_UInt8&)omsOid(), str);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsSimDumpToHtmlLong(OmsHandle &Handle, OmsSimDumpCollector &str) const
{
  omsSimDumpToHtmlShort(Handle, str);
}

/*----------------------------------------------------------------------*/

void OmsAbstractObject::omsSimSetObjectName(OmsHandle &Handle, const char *name)
{
  Handle.m_pSession->m_lcSink->SimCtlSetObjectName(Handle, (SAPDB_UInt8&)omsOid(), name);
}

/*----------------------------------------------------------------------*/

const char* OmsAbstractObject::omsSimGetObjectName(OmsHandle &Handle) const
{
  const char *name;
  Handle.m_pSession->m_lcSink->SimCtlGetObjectName(Handle, (SAPDB_UInt8&)omsOid(), &name);
  return name;
}

/*----------------------------------------------------------------------*/

const char *OmsAbstractObject::omsSimDumpMakeOidRef(OmsHandle &h, const OmsObjectId &oid, int flags)
{
  const char *ref;

  ALIGN_OID_CONST (oid8, oid);

  h.m_pSession->m_lcSink->SimCtlGetHTMLRef(h, (SAPDB_UInt8&)oid8, &ref, flags);
  return ref;
}

/*----------------------------------------------------------------------*/

const char *OmsAbstractObject::omsSimDumpMakeOidRef(const OmsObjectId &oid, int flags)
{
  ALIGN_OID_CONST (oid8, oid);

  return omsSimDumpMakeOidRef(*OmsHandle::omsGetOmsHandle(), oid8, flags);
}
