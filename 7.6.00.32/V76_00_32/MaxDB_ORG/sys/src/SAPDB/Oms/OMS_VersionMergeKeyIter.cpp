/*!
 * \file    OMS_VersionMergeKeyiter.cpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which merges the two version iterators
 */

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "Oms/OMS_VersionMergeKeyIter.hpp"
#include <new>


/*===================================================================================*/
/*! Construtor.                                                                      **
**  In this the iterators for objects of normal (if current session is in a version) **
**  and for unloaded versions (if the version is marked as unloaded) are created.    **
**  If one of the iterators do not contain any object in the specified interval,     **
**  then the corresponding iterator is destroyed again and the corresponding pointer **
**  is set to NULL.                                                                  **
**  Finally comparing the current keys of the iterators the iterator with the        **
**  smallest (direction = OMS_ASCENDING) resp. the largest (direction =              **
**  OMS_DESCENDING) key the pointer to the current iterator is set.                  **
**  \param pStartKey pointer to the key which should be used to position the         **
**                   iterator after the creation. Currently this key must equal      **
**                   either the lower or the upper key.                              **
**  \param pLowerKey pointer to the key which represents the lower boundary of the   **
**                   interval the iterator should read from. The interval boundary   **
**                   is inclusive.                                                   **
**  \param pUpperKey pointer to the key which represents the upper boundary of the   **
**                   interval the iterator should read from. The interval boundary   **
**                   is inclusive.                                                   **
**  \param direction the direction (OMS_ASCENDING or OMS_DESCENDING) of the iterator **
**  \param pSession  pointer to the session                                          **
**  \param pContainerInfo pointer to the container info of the corresponding class   */
/*************************************************************************************/
OMS_VersionMergeKeyIter::OMS_VersionMergeKeyIter
(
 const void*       pStartKey,
 const void*       pLowerKey,
 const void*       pUpperKey,
 OmsIterDirection  direction,
 OMS_Session*      pSession,
 OMS_ContainerEntry* pContainerInfo
 )
 : OMS_MergeKeyIterBase(pSession, pContainerInfo)
 , m_cmpEqual(false)
 , m_pInMemVersionIter(NULL)
 , m_inMemVersionActive(false)
 , m_pUnloadedVersionIter(NULL)
 , m_unloadedVersionActive(false)
{
  const char* msg = "OMS_VersionMergeKeyIter::Ctor ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  m_pInMemVersionIter = REINTERPRET_CAST(OMS_InMemVersionKeyIter*, 
      pSession->allocate(sizeof(OMS_InMemVersionKeyIter)));
  new(m_pInMemVersionIter) OMS_InMemVersionKeyIter (pStartKey, pLowerKey, pUpperKey, direction,
                                                         pSession, pContainerInfo);
  if (*m_pInMemVersionIter){ 
    // If iterator contains objects, which lie between the interval bounds, then set iterator active
    m_inMemVersionActive = true;
  }
  else {
    // Iterator does not contain any entry in the specified interval. Therefore the
    // iterator can be deleted as even in the case of a change in the direction there
    // will never be any entry.
    m_inMemVersionActive = false;
    m_pInMemVersionIter->DeleteSelf();
    m_pInMemVersionIter = NULL;
  }

  if (m_pSession->CurrentContext()->IsUnloaded()) {
    m_pUnloadedVersionIter = REINTERPRET_CAST(OMS_UnloadedVersionKeyIter*, 
      pSession->allocate(sizeof(OMS_UnloadedVersionKeyIter)));
    new(m_pUnloadedVersionIter) OMS_UnloadedVersionKeyIter(pStartKey, pLowerKey, pUpperKey, direction,
                                                            pSession, pContainerInfo);
    if (*m_pUnloadedVersionIter){ 
    // If iterator contains objects, which lie between the interval bounds, then set iterator active
      m_unloadedVersionActive = true;
    }
    else {
      // Iterator does not contain any entry in the specified interval. Therefore the
      // iterator can be deleted as even in the case of a change in the direction there
      // will never be any entry.
      m_unloadedVersionActive = false;
      m_pUnloadedVersionIter->DeleteSelf();
      m_pUnloadedVersionIter = NULL;
    }
  }

  // Determine first object 
  if (m_unloadedVersionActive == true && m_inMemVersionActive == true){
    // Get smallest object either from unloaded or in-memory iterator
    int cmpResult = memcmp(m_pInMemVersionIter->GetCurrKey(),
                           m_pUnloadedVersionIter->GetCurrKey(),
                           m_keyLen);
    if (cmpResult == 0){
      m_pCurrIterator = m_pInMemVersionIter; 
      m_cmpEqual      = true;   
    }
    else if (cmpResult < 0){
      m_pCurrIterator = m_pInMemVersionIter;
    }
    else {
      m_pCurrIterator = m_pUnloadedVersionIter;
    }
  }
  else if (m_inMemVersionActive == true) {
    // Only in-memory iterator is non-empty
    m_pCurrIterator = m_pInMemVersionIter;
  }
  else if (m_unloadedVersionActive == true) { 
    // Only version iterator is non-empty
    m_pCurrIterator = m_pUnloadedVersionIter;
  }
  else {
    // Iterators is empty
    m_pCurrIterator = NULL;
  }
}

/*===================================================================================*/

void OMS_VersionMergeKeyIter::DeleteSelf()
{
  const char* msg = "OMS_VersionMergeKeyIter::DeleteSelf ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);
    
  this->~OMS_VersionMergeKeyIter();
  m_pSession->deallocate(this);
}

/*===================================================================================*/

OMS_VersionMergeKeyIter::~OMS_VersionMergeKeyIter()
{
  const char* msg = "OMS_VersionMergeKeyIter::Dtor ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);
    
  if (NULL != m_pUnloadedVersionIter) {
    m_pUnloadedVersionIter->DeleteSelf();
    m_pUnloadedVersionIter = NULL;
  }
  if (NULL != m_pInMemVersionIter) {
    m_pInMemVersionIter->DeleteSelf();
    m_pInMemVersionIter = NULL;
  }
}

/*===================================================================================*/
