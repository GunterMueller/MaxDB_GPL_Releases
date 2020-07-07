/*!
 * \file    OMS_ClassEntry.cpp
 * \author  MarkusSi, Roterring
 * \brief   Single entry of OMS Class Dictionary.
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


#include "Oms/OMS_ClassEntry.hpp"

#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ArrayObject.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*----------------------------------------------------------------------------------*/

unsigned int OMS_ClassEntry::m_versionCounter = 0;

/*----------------------------------------------------------------------------------*/

OMS_ClassEntry::OMS_ClassEntry(OMS_Session             *pSession,
                               const char              *pClassName, 
                               ClassID                  guid,
                               ClassID                  baseClassGuid,
                               const OMS_KeyDesc       &keyDesc,
                               size_t                   objSize, 
                               void                    *pVTblPtr,
                               bool                     isVarObject,
                               bool                     isArrayObject)
: m_pSession(pSession)
, m_guid(guid)
, m_objectSize(objSize)
, m_pVTblPtr(pVTblPtr)
, m_pBaseClass(NULL)
, m_isBaseClass(false) 
, m_isVarObject(isVarObject)
, m_isArrayObject(isArrayObject)
, m_keyDesc(keyDesc) 
, m_pHashNext(NULL)
, m_pFreeList(NULL)
, m_version(++m_versionCounter)
{
  if (isVarObject){
    m_persistentSize = 0;
    m_transientSize  = m_objectSize + (size_t)OmsObjectContainer::headerSize();
  }
  else if (isArrayObject){
    // array + variable to store array-size
    // As not every size is allowed in the kernel, search the smallest category which
    // can be used to store the array
    m_persistentSize  = OmsArrayObjectBase::GetMaxByteSize(OmsArrayObjectBase::GetCategory(m_objectSize));
    // header + sizeof(vtbl-pointer)+ max persistent size 
    // Transient size must be large enough to store max size persistent object
    m_transientSize  = (size_t)OmsObjectContainer::headerSize() + sizeof(void*) + m_persistentSize;    
  }
  else {
    // object-size + key-length - sizeof(vtbl-pointer) 
    m_persistentSize =  m_objectSize + m_keyDesc.GetLen() - sizeof(void*);

    // object-size + key-length + header of frame 
    m_transientSize = m_objectSize + m_keyDesc.GetLen() + (size_t)OmsObjectContainer::headerSize();
  }
 
  size_t classNameLength = strlen(pClassName);
  if (classNameLength >= sizeof(m_className)) {
    classNameLength = sizeof(m_className) - 1;
  }
  SAPDB_MemCopyNoCheck(&m_className[0], pClassName, classNameLength);
  m_className[classNameLength] = 0;

  // Get pointer to class entry for base class, if class is derived      
  if (baseClassGuid != 0){
    m_pBaseClass = m_pSession->GetClassDir().GetClassEntry(baseClassGuid);
    m_pBaseClass->SetBaseClass();
    if (m_isArrayObject){
      // 'Sub' container for array-objects
      m_objectSize = m_pBaseClass->GetObjectSize();
    }
  }
  else if (m_isArrayObject){
    // 'Base' container for array-objects
    SetBaseClass();
  }
}

/*----------------------------------------------------------------------------------*/

void* OMS_ClassEntry::operator new(size_t sz, OMS_Session *pSession) 
{
#if defined(_ASSERT_OMS)
  void *p = pSession->allocate(sz);
  memset (p, 0x1b, sz);
  return p;
#else
  return pSession->allocate(sz);
#endif
}

/*----------------------------------------------------------------------------------*/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_ClassEntry::operator delete (void* p, OMS_Session *pSession)
{
  pSession->deallocate(p);
}
#endif

/*----------------------------------------------------------------------------------*/

void OMS_ClassEntry::DeleteSelf()
{
  OMS_Session *pSession = m_pSession;
#if defined(_ASSERT_OMS)
  memset (this, 0x2b, sizeof(*this));
#endif
  pSession->deallocate(this);
}

/*----------------------------------------------------------------------------------*/

OmsObjectContainer* OMS_ClassEntry::GetMemory()
{
  size_t size = GetTransientSize(); 

  OMS_TRACE(omsTrMemory, m_pSession->m_lcSink, "OMS GetMemory in ClassEntry: " 
                                   << " Size=" << (long)size
                                   << " Context=" << OMS_CharBuffer(&m_pSession->m_context->GetVersionId()[0], sizeof(OmsVersionId)));

  bool reuseFrame = true;
  // First check whether there is an unused frame in the freelist
  if (GetFreeList() == NULL){
    SetFreeList(m_pSession->m_context->GetContainerDir().GetFreeList(size));

    if (GetFreeList() == NULL){
      // As a new Header is created if it does not exists already, 
      // this should never occur
      OMS_Globals::Throw(e_nil_pointer, "OMS_ClassEntry::GetMemory: Error when creating FreeListHeader", __MY_FILE__, __LINE__);
    }
  }

  OmsObjectContainer *p = GetFreeList()->GetFrame();

  if (NULL == p) { 
    reuseFrame = false;
    if (m_pSession->InVersion()) {
      p = (OmsObjectContainerPtr) m_pSession->m_context->allocate(size);  
    }
    else {
      p = (OmsObjectContainerPtr) m_pSession->m_context->StackHeapMalloc(size);
    }
  }

  if (NULL != p) {
    p->InitObjContainer(*this);
  }

  OMS_TRACE(omsTrMemory, m_pSession->m_lcSink, "    => Object="  << (void*)p 
           << (reuseFrame?" Reused":" New"));
  return p;
}

/*----------------------------------------------------------------------------------*/

void OMS_ClassEntry::ChainFree(OmsObjectContainer *&pObj, int caller)
{
  size_t size = GetTransientSize();

  OMS_TRACE(omsTrMemory, m_pSession->m_lcSink, "OMS ChainFree in ClassEntry: " 
                                   << " Object=" << (void*)pObj
                                   << " Size=" << (long)size
                                   << " Context=" << OMS_CharBuffer(&m_pSession->m_context->GetVersionId()[0], sizeof(OmsVersionId)));

  if (pObj == NULL){
    return;
  }

#ifdef USE_SYSTEM_ALLOC_CO13
  m_pSession->m_context->deallocate(pObj);
#else
  if (GetFreeList() == NULL){
    SetFreeList(m_pSession->m_context->GetContainerDir().GetFreeList(size));

    if (GetFreeList() == NULL){
      // As a new Header is created if it does not exists already, 
      // this should never occur
      OMS_Globals::Throw(e_nil_pointer, "OMS_ClassEntry::ChainFree: Error when creating FreeListHeader", __MY_FILE__, __LINE__);
    }
  }

//#ifdef _ASSERT_OMS
  pObj->InitializeForFreeList(caller);
//#endif
  GetFreeList()->Insert(pObj);
#endif
  pObj = NULL;
}

/*----------------------------------------------------------------------------------*/
