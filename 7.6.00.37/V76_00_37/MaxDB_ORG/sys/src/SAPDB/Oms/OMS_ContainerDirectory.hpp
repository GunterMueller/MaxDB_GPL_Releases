/*!
 * \file    OMS_ContainerDirectory.hpp
 * \author  MarkusSi, Roterring
 * \brief   Local Container Directory.
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
#ifndef __OMS_CONTAINERDIRECTORY_HPP
#define __OMS_CONTAINERDIRECTORY_HPP

#include "Oms/OMS_ContainerDirectoryDef.hpp"

#include "Oms/OMS_ContainerEntry.hpp"
#include "Oms/OMS_ContextDef.hpp"
#include "Oms/OMS_ArrayObject.hpp"

/*----------------------------------------------------------------------*/
/* Implementation of class OMS_ContainerDirectory inline methods        */
/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OMS_ContainerDirectory::GetContainerEntry(
  ClassID          baseGuid, 
  OmsSchemaHandle  schema, 
  OmsContainerNo   containerNo,
  int              arrayByteSize)
{
  // Find class entry for base guid
  OMS_ClassEntry *pBaseClassEntry = m_pContext->GetSession()->GetClassDir().GetClassEntry(baseGuid);

  // Determine category for given number of array-elements
  short category = OmsArrayObjectBase::GetCategory( arrayByteSize );

  // convert guid (encode size)
  // As a guid at kernel side must correspond to a fixed size, the different sizes, which are
  // used in the liboms using the same logical guid, must be converted to different guids.
  ClassID guid = omsMakeArrayGuid(baseGuid, category);

  OMS_ContainerEntry *pContainerEntry = Find(guid, schema, containerNo);
  if (pContainerEntry ){
    return pContainerEntry;
  }
  else {
    // Try to read info from kernel and if found, insert into local directory 
    return AutoRegisterArrayContainer(baseGuid, schema, containerNo, arrayByteSize);
  }
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OMS_ContainerDirectory::GetContainerEntry(
  ClassID          guid, 
  OmsSchemaHandle  schema, 
  OmsContainerNo   containerNo)
{
  OMS_ContainerEntry *pContainerEntry = Find(guid, schema, containerNo);
  if (pContainerEntry == NULL){
    // Try to read info from kernel and if found, insert into local directory 
    short error = e_ok;
    pContainerEntry = AutoRegisterContainer(guid, schema, containerNo, error);
    if (error == e_ok){
      return pContainerEntry;
    }
    else if (error == e_OMS_sysinfo_not_found) {
      ThrowUnknownContainer(guid, schema, containerNo, __MY_FILE__, __LINE__);
      return NULL;
    }
    else {
      char buf[256];
      sp77sprintf(buf, sizeof(buf), "GetContainerEntry: Guid: %d  Schema: %d  Container: %d",
        guid, schema, containerNo);
      OMS_Globals::Throw(error, buf, __MY_FILE__, __LINE__); 
      return NULL;
    }
  }
  else {
    return pContainerEntry;
  }
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OMS_ContainerDirectory::GetContainerEntry(OMS_ContainerHandle &containerHandle)
{
  OMS_ContainerEntry *pContainerEntry = Find(containerHandle);

  if (pContainerEntry == NULL){
    // Try to read info from kernel and if found, insert into local directory 
    pContainerEntry = AutoRegisterContainer(containerHandle);
  }

  return pContainerEntry;
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OMS_ContainerDirectory::Find (OMS_ContainerHandle containerHandle, 
                                                         bool checkDropFlag) 
{
  int hashSlot              = containerHandle % m_headentries;
  OMS_ContainerEntry* pCurr = m_ppContHandleHead[hashSlot];
  while (pCurr != NULL) {
    if (pCurr->GetContainerHandle() == containerHandle) {
      if (checkDropFlag){
        pCurr->CheckIfNotDropped(__MY_FILE__, __LINE__);
      }
      return pCurr;
    }
    pCurr = pCurr->m_pContHandleHashNext;
  }

  // Container does not exist in the local container directory 
  return NULL;
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OMS_ContainerDirectory::Find (ClassID          guid, 
                                                         OmsSchemaHandle  sh, 
                                                         OmsContainerNo   containerNo,
                                                         bool             checkDropFlag) 
{
  int hashSlot = HashValue(guid, sh, containerNo, m_headentries);
  OMS_ContainerEntry* pCurr = m_ppContIdHead[hashSlot];
  while (pCurr != NULL) {
    if (  (pCurr->GetGuid()                 == guid)  
       && (pCurr->GetSchema()               == sh) 
       && (pCurr->GetContainerNo()          == containerNo)) {
      if (checkDropFlag){
        pCurr->CheckIfNotDropped(__MY_FILE__, __LINE__);
      }
      return pCurr;
    }
    pCurr = pCurr->m_pContIdHashNext;
  }

  // Container does not exist in the local container directory 
  return NULL;
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerDirectory::Iter OMS_ContainerDirectory::First ()
{
  Iter iter(this);
  return iter;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

inline OMS_ContainerDirectory::Iter::Iter(OMS_ContainerDirectory* pContainerDir) 
: m_pContainerDir(pContainerDir)
, m_pCurr(NULL)
, m_headIndex(0) 
{
  // Position iterator on first non-empty hash bucket
  for (m_headIndex = 0; m_headIndex < m_pContainerDir->m_headentries; ++m_headIndex) {
    m_pCurr = m_pContainerDir->m_ppContHandleHead[m_headIndex];
    if (NULL != m_pCurr) {
      break;
    }
  }
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerDirectory::Iter::Iter(const Iter& iter) 
{
  *this = iter;
}

/*----------------------------------------------------------------------*/

inline void OMS_ContainerDirectory::Iter::operator++() 
{
  if (NULL != m_pCurr) {
    // Go to next entry in hash chain
    m_pCurr = m_pCurr->m_pContHandleHashNext;
  }

  if (NULL == m_pCurr) {
    // If there was no next entry in the hash chain, then go to next
    // non-empty hash bucket
    ++m_headIndex; 
    while (m_headIndex < m_pContainerDir->m_headentries) {
      m_pCurr = m_pContainerDir->m_ppContHandleHead[m_headIndex];
      if (NULL != m_pCurr) {
        break;
      }
      ++m_headIndex;
    }
  }
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerDirectory::Iter::operator bool() const 
{
  return (m_pCurr != NULL);
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OMS_ContainerDirectory::Iter::operator()() const
{
  return m_pCurr;
}

/*----------------------------------------------------------------------*/

#endif  // __OMS_CONTAINERDIRECTORY_HPP
