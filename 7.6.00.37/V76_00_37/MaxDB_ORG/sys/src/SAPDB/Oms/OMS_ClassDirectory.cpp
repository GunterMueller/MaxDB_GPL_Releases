/*!
 * \file    OMS_ClassDirectory.cpp
 * \author  MarkusSi, Roterring
 * \brief   OMS Class Dictionary.
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
#include "Oms/OMS_ClassDirectory.hpp"

#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ClassEntry.hpp"
#include "Oms/OMS_VarObjInfo.hpp"
#include "Oms/OMS_Trace.hpp"
#include "Oms/OMS_ArrayObject.hpp"


/*----------------------------------------------------------------------*/

OMS_ClassDirectory::OMS_ClassDirectory() 
: OMS_IGuidHash()
, m_pSession(NULL)
{
  for (int ix = 0; ix < HEAD_ENTRIES; ++ix) {
    m_head[ix] = NULL;
  }
}

/*----------------------------------------------------------------------*/

OMS_ClassDirectory::~OMS_ClassDirectory() 
{
  if (m_pSession){
    OMS_TRACE(omsTrContainerDir, m_pSession->m_lcSink, "OMS Destruction of ClassDirectory ");
  }

  for (int ix = 0; ix < HEAD_ENTRIES; ++ix) {
    OMS_ClassEntry *pDel;
    OMS_ClassEntry *pCurr = m_head[ix];
    m_head[ix] = NULL;
    while (NULL != pCurr) {
      pDel  = pCurr;
      pCurr = pCurr->m_pHashNext;
      pDel->DeleteSelf();
    }
  }
  
  m_pSession = NULL;
#if defined(_ASSERT_OMS)
  memset (this, 0x0b, sizeof(*this));
#endif

  //new(this) OMS_ClassDirectory();   
}

/*----------------------------------------------------------------------*/

void OMS_ClassDirectory::Create(OMS_Session* pSession)
{
  if (pSession)
    OMS_TRACE(omsTrContainerDir, pSession->m_lcSink, "OMS Creation of ClassDirectory ");

  m_pSession = pSession;
}

/*----------------------------------------------------------------------*/

OMS_ClassEntry* OMS_ClassDirectory::RegisterClass (
  const char             *pClassName, 
  ClassID                 guid,
  ClassID                 baseClassGuid,
  const OMS_KeyDesc      &keyDesc,
  size_t                  objectSize, 
  void                   *pVTblPtr,
  bool                    isVarObject,
  bool                    isArrayObject)
{
  OMS_TRACE(omsTrContainerDir, m_pSession->m_lcSink, "OMS Registering Class: " 
                  << " Guid=" << guid);

  if (guid < -1){
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "It is tried to register class %s with negative guid (%d)", pClassName, guid);
    DbpBase base(m_pSession->m_lcSink);
    base.dbpOpError (buf);
    OMS_Globals::Throw(e_unknown_guid, buf, __MY_FILE__, __LINE__);  
  }

  // First check whether a local entry is already existing
  OMS_ClassEntry  *pClassEntry = Find(guid);

  if (pClassEntry != NULL){
    // Check to ensure, that class to be registered is the same as class that 
    // was found for specified guid.
    //if (pVTblPtr != pClassEntry->GetVTblPtr()){ // Unfortunately the vtable might be different although the object is the same
    if (  baseClassGuid != pClassEntry->GetBaseGuid()
       || keyDesc       != pClassEntry->GetKeyDesc()
       || objectSize    != pClassEntry->GetObjectSize()
       || isVarObject   != pClassEntry->IsVarObject()
       || isArrayObject != pClassEntry->IsArrayObject()){
    char buf[256];
      sp77sprintf(buf, sizeof(buf), "It is tried to register class '%s' with the same guid (%d) as class '%s'",
                  pClassName, guid, pClassEntry->GetClassName());
      DbpBase base(m_pSession->m_lcSink);
      base.dbpOpError (buf);
      OMS_Globals::Throw(e_unknown_guid, buf, __MY_FILE__, __LINE__, m_pSession);
    }
  }
  else {
    // Create new local class entry 
    pClassEntry = new(m_pSession) OMS_ClassEntry(m_pSession,
        pClassName, guid, baseClassGuid, keyDesc, objectSize, 
        pVTblPtr, isVarObject, isArrayObject);

    // Insert entry into local class directory
    Insert(pClassEntry);
  }

  return pClassEntry;
}

/*----------------------------------------------------------------------*/

OMS_ClassEntry*  OMS_ClassDirectory::AutoRegisterSubClass(ClassID guid)
{
  OMS_TRACE(omsTrContainerDir, m_pSession->m_lcSink, "OMS Auto-registering Sub-class: " 
                  << " Guid=" << guid);

  ClassID baseGuid;
  short   category;
  omsSplitArrayGuid(guid, baseGuid, category);
 
  OMS_ClassEntry *pBaseClassInfo = Find(baseGuid);
  if (pBaseClassInfo == NULL){
    char buf[256];
    sp77sprintf(buf, sizeof(buf), "AutoRegisterSubClass: BaseClass (GUID=%d) for SubClass (GUID=%d) is not registered in current session", 
                baseGuid, guid);
    OMS_Globals::Throw(e_unknown_guid, buf, __MY_FILE__, __LINE__);
    return NULL;
  }
  else {
    char className[256];
    sp77sprintf(className, sizeof(className), "%s(%d)", 
                pBaseClassInfo->GetClassName(), category);

    size_t arrayByteSize = OmsArrayObjectBase::GetMaxByteSize(category); 
//    int maxArraySize = OmsArrayObjectBase::GetMaxByteSize(category)
//                     / pBaseClassInfo->GetObjectSize();

    OMS_ClassEntry *pClassInfo = RegisterClass(&className[0], guid, baseGuid, OMS_KeyDesc(), 
                                arrayByteSize,
                                pBaseClassInfo->GetVTblPtr(),
                                /*isVarObject=*/false, /*isArrayObject=*/true);
    return pClassInfo;
  }
}


/*----------------------------------------------------------------------*/

void OMS_ClassDirectory::Insert(OMS_ClassEntry *pClassEntry)
{
  OMS_TRACE(omsTrContainerDir, m_pSession->m_lcSink, "OMS Inserting ClassEntry:"
                  << " Guid=" << pClassEntry->GetGuid());

  int slot = HashValue(pClassEntry->GetGuid(), HEAD_ENTRIES);
  pClassEntry->m_pHashNext = m_head[slot];
  m_head[slot]             = pClassEntry;
}

/*----------------------------------------------------------------------*/

OMS_ClassDirectory::Iter OMS_ClassDirectory::First ()
{
  Iter iter(this);
  return iter;
}

/*----------------------------------------------------------------------*/

bool OMS_ClassDirectory::IsBaseClassOf(ClassID baseGuid, ClassID derivedGuid) {
  return  IsDerivedClassOf(baseGuid, derivedGuid);
}

/*----------------------------------------------------------------------*/

bool OMS_ClassDirectory::IsDerivedClassOf(ClassID baseGuid, ClassID derivedGuid) {
  OMS_ClassEntry* pDerivedClassEntry = Find(derivedGuid);
  do {
    if (pDerivedClassEntry->IsDerivedClassOf(baseGuid)) {
      return true;
    }
    if (pDerivedClassEntry->IsDerivedClass()) {
      pDerivedClassEntry = Find(pDerivedClassEntry->GetBaseGuid());
    }
    else {
      return false;
    }
  }
  while(1);
}

/*----------------------------------------------------------------------*/

void OMS_ClassDirectory::CleanupAfterContextChange()
{
  Iter iter(this);
  while (iter){
    iter()->ResetFreeList();
    ++iter;
  }
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

OMS_ClassDirectory::Iter::Iter(OMS_ClassDirectory* pClassDir) 
: m_pClassDir(pClassDir)
, m_pCurr(NULL)
, m_headIndex(0) 
{
  // Position the iterator on the first non-empty hash bucket
  for (m_headIndex = 0; m_headIndex < HEAD_ENTRIES; ++m_headIndex) {
    m_pCurr = m_pClassDir->m_head[m_headIndex];
    if (NULL != m_pCurr) {
      break;
    }
  }
}

/*----------------------------------------------------------------------*/

OMS_ClassDirectory::Iter::Iter(const Iter& iter) 
{
  *this = iter;
}

/*----------------------------------------------------------------------*/

void OMS_ClassDirectory::Iter::operator++() 
{
  if (NULL != m_pCurr) {
    // Go to next entry in hash chain
    m_pCurr = m_pCurr->m_pHashNext;
  }

  if (NULL == m_pCurr) {
    // If there was no next entry in the hash chain, then go to next
    // non-empty hash bucket
    ++m_headIndex; 
    while (m_headIndex < HEAD_ENTRIES) {
      m_pCurr = m_pClassDir->m_head[m_headIndex];
      if (NULL != m_pCurr) {
        break;
      }
      ++m_headIndex;
    }
  }
}

/*----------------------------------------------------------------------*/

OMS_ClassDirectory::Iter::operator bool() const 
{
  return (m_pCurr != NULL);
}

/*----------------------------------------------------------------------*/

OMS_ClassEntry* OMS_ClassDirectory::Iter::operator()() const
{
  return m_pCurr;
}
