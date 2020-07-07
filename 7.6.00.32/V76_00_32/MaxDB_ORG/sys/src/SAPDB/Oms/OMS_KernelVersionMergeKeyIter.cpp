/*!
 * \file    OMS_KernelVersionMergeKeyIter.cpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which merges the kernel iterator and the version iterators
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

#include "Oms/OMS_KernelVersionMergeKeyIter.hpp"
#include <new>


/*===================================================================================*/
/*! Construtor.                                                                      **
**  In this method the iterators for kernel objects and for version objects (if      **
**  current session is in a version) are created.                                    **
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
**  \param pContainerInfo pointer to the container info of the corresponding class   **
**  \param maxBufferSize Number of oids which are bufferd in the oms-layer. This     **
**                   parameter is only relevant for the kernel iterator.             */
/*************************************************************************************/
OMS_KernelVersionMergeKeyIter::OMS_KernelVersionMergeKeyIter(
  const void*       pStartKey,
  const void*       pLowerKey,
  const void*       pUpperKey,
  OmsIterDirection  direction,
  OMS_Session*      pSession,
  OMS_ContainerEntry* pContainerInfo,
  int               maxBufferSize
  )
  : OMS_MergeKeyIterBase(pSession, pContainerInfo)
  , m_pVersionIterator(NULL)
  , m_versionActive(false)
  , m_pKernelIterator(NULL)
  , m_kernelActive(false)
  , m_direction(direction)
  , m_refCnt(1)
{
  const char* msg = "OMS_KernelVersionMergeKeyIter::Ctor ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  // Check that lower key <= upper key and lower key < start key and start key < upper key
  if (pLowerKey && 0 < memcmp( pLowerKey, pStartKey, m_keyLen)) {
    return;
  }
  if (pUpperKey && 0 < memcmp( pStartKey, pUpperKey, m_keyLen)) {
    return;
  }
  if (pLowerKey && pUpperKey && 0 < memcmp( pLowerKey, pUpperKey, m_keyLen)) {
    return;
  }

  // Create iterator for objects stored in the kernel
  m_pKernelIterator = new(m_pSession->allocate(sizeof(OMS_KernelKeyIter)))
                          OMS_KernelKeyIter(pStartKey, pLowerKey, pUpperKey, direction,
                                             m_pSession, m_pContainerInfo, maxBufferSize);
  if (*m_pKernelIterator){
    m_kernelActive = true;
  }
  else {
    // Iterator does not contain any entry in the specified interval. Therefore the
    // iterator can be deleted as even in the case of a change in the direction there
    // will never be any entry.
    m_kernelActive = false;
    m_pKernelIterator->DeleteSelf();
    m_pKernelIterator = NULL;
  }

  // Create iterator for objects created in a version
  if (m_pSession->InVersion() && !m_pSession->IsReadUnchangedObjectsActive()) {
    // If 'IsReadUnchangedObjectsActive' then the objects should be read in the state, 
    // they were when the consistent view was started. As the version iterator contains only
    // objects which have been created in the version, they did not exist, when the version
    // was created, and therefore it is not necessary, that the iterator is created.

    m_pVersionIterator = new(m_pSession->allocate(sizeof(OMS_VersionMergeKeyIter)))
                            OMS_VersionMergeKeyIter(pStartKey, pLowerKey, pUpperKey, direction,
                                                m_pSession, m_pContainerInfo);
    if (*m_pVersionIterator){
      m_versionActive = true;
    }
    else {
      // Iterator does not contain any entry in the specified interval. Therefore the
      // iterator can be deleted as even in the case of a change in the direction there
      // will never be any entry.
      m_versionActive = false;
      m_pVersionIterator->DeleteSelf();
      m_pVersionIterator = NULL;
    }
  }

  // Determine first object
  if (m_kernelActive  == true && m_versionActive == true){
    // Get smallest object either from kernel or from version
    int cmpResult = memcmp(m_pKernelIterator->GetCurrKey(),
                           m_pVersionIterator->GetCurrKey(),
                           m_keyLen);
    if (  cmpResult <= 0 && direction == OMS_ASCENDING
       || cmpResult >= 0 && direction == OMS_DESCENDING) {
      m_pCurrIterator = m_pKernelIterator;
    }
    else {
      m_pCurrIterator = m_pVersionIterator;
    }
  }
  else if (m_kernelActive  == true) {
    // Only kernel iterator is non-empty
    m_pCurrIterator = m_pKernelIterator;
  }
  else if (m_versionActive == true) {
    // Only version iterator is non-empty
    m_pCurrIterator = m_pVersionIterator;
  }
  else {
    // Iterators are empty
    m_pCurrIterator = NULL;
  }
}

/*************************************************************************************/

void OMS_KernelVersionMergeKeyIter::DeleteSelf()
{
  const char* msg = "OMS_KernelVersionMergeKeyIter::DeleteSelf ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (m_refCnt == 0) {
    this->~OMS_KernelVersionMergeKeyIter();
    m_pSession->deallocate(this);
  }
}

/*************************************************************************************/

OMS_KernelVersionMergeKeyIter::~OMS_KernelVersionMergeKeyIter()
{
  const char* msg = "OMS_KernelVersionMergeKeyIter::Dtor ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (m_refCnt == 0) {
    if (NULL != m_pKernelIterator){
      m_pKernelIterator->DeleteSelf();
      m_pKernelIterator = NULL;
    }

    if (NULL != m_pVersionIterator){
      m_pVersionIterator->DeleteSelf();
      m_pVersionIterator = NULL;
    }
  }
}

/*************************************************************************************/

